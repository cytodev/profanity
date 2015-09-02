/**
 * Profanity.c
 *
 * Profanity is a small binary file that uses your alias and a small file to
 * keep track of how many times you have said a no-no word.
 *
 * Getting started:
 *     To get started with Profanity, you would use the command Profanity init.
 *     This command creates a little file in your home directory to store all
 *     of your profane actions. You will only have to type this once. In fact,
 *     you don't even have to type this since Profanity creates this file if it
 *     does not exist anyway.
 *
 * How to use Profanity, the basic commands:
 *     Profanity can be used in a variaty of ways, you can call the programm
 *     from your command line using `profanity`. This will display a little
 *     help text to get you started. Same goes for the -h and -help parameters
 *     
 *     To use Profanity like a basic pro:
 *     type `profanity addWord <yourWord>` in your terminal. This adds an alias
 *     for that word in your ~/.bash_aliases file along the lines of
 *     `alias <yourWord>='profanity add <yourWord>';`. The next time you type
 *     your word in the terminal you will get a message telling you that Santa
 *     is watching your every move.
 *     
 *     To use Profanity like a basic l33t h4x0r:
 *     Profanity has multiple ways to increment on your naughty-words list.
 *     For example, you can type `Profanity <naughtyWord>` or
 *     `Profanity add <naughtyWord>`. Both ways are valid, and both ways will
 *     make Santa sad.
 *
 * How to use Profanity, the advanced commands:
 *     Profanity comes with an array of tools to get your no-no word-list, and
 *     a few tools to erase your very naughty past.
 *
 *     To use Profanity like an advanced pro:
 *     You can type `Profanity get` to get a list of words you've added to the
 *     no-no list, and the times you've said them. Alternatively, to erase your
 *     dirty past, you can type `Profanity rem` to clear your murky history.
 *
 *     To use Profanity like an advanced l33t h4x0r:
 *     I hear you say: "An advanced l33t h4x0r, is this guy for real?". Yes,
 *     yes I am. To use Profanity like read the title again, you would type
 *     `Profanity get <naughtyWord>` to get the results of just that one word.
 *     To remove just a single word you would type
 *     `Profanity rem <naughtyWord>`.
 *
 * Using Profanity in combination with other "software":
 *     I have no idea how well Profanity plays along with other software. You
 *     can test it for me and report back if you wish, but I like to think
 *     that Profanity is the baddest motherducker on the block that steals
 *     your lunch money.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFSIZE 256

char* getFileLocation(int temp) {
	if(temp == 1) {
		size_t bufferSize = snprintf(NULL, 0, "%s/.profanity.tmp", getenv("HOME")) + 1;
		char *buffer = malloc(bufferSize);
		snprintf(buffer, bufferSize, "%s/.profanity.tmp", getenv("HOME"));

		return buffer;
	}
	
	size_t bufferSize = snprintf(NULL, 0, "%s/.profanity", getenv("HOME")) + 1;
	char *buffer = malloc(bufferSize);
	snprintf(buffer, bufferSize, "%s/.profanity", getenv("HOME"));

	return buffer;
}

int checkFile(char *fileLocation) {
	struct stat buffer;
	int exist = stat(fileLocation, &buffer);

	if(exist != 0) {
		FILE *filewtr;

		printf("%s not found. Creating...\n", fileLocation);
		if((filewtr = fopen(fileLocation, "w")) != NULL) {
			fclose(filewtr);
			printf("%s created sucessfully\n", fileLocation);
		} else {
			printf("Failed to create file %s\n", fileLocation);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void removeNewlines(char *s) {
	while(s=strstr(s, "\n")) {
		memmove(s, s+strlen("\n"), 1+strlen(s+strlen("\n")));
	}
}

void removeWord(char *s, char* wrd) {
	while(s=strstr(s, wrd)) {
		memmove(s, s+strlen(wrd), 1+strlen(s+strlen(wrd)));
	}
}

int addProfanity(char *word) {
	checkFile(getFileLocation(0));
	
	printf("adding a \"%s\" to %s\n", word, getFileLocation(0));

	FILE *fr = fopen(getFileLocation(0), "r");
	int ln = 0;
	int found = 0;
	char tmp[BUFSIZE] = {0x0};

	if(fr == NULL) {
		printf("couldn't add \"%s\" to %s\n", word, getFileLocation(0));
		return EXIT_FAILURE;
	}

	while(fr != NULL && fgets(tmp, sizeof(tmp), fr) != NULL) {
		++ln;

		if(strstr(tmp, word)) {
			found = 1;
		}
	}

	if(fr != NULL) {
		fclose(fr);
	}

	if(found == 1) {
		FILE *oldFile;
		FILE *newFile;

		if((oldFile = fopen(getFileLocation(0), "r")) == NULL) {
			fprintf(stderr, "Could not open %s for reading.\n", getFileLocation(0));
			return EXIT_FAILURE;
		}

		if((newFile = fopen(getFileLocation(1), "w")) == NULL) {
			fprintf(stderr, "Could not open %s for writing.\n", getFileLocation(1));
			return EXIT_FAILURE;
		}

		int lineNumber = 0;
		int len;
		char tmp[BUFSIZE] = {0x0};
		
		while(fgets(tmp, sizeof(tmp), oldFile) != NULL) {
			len = strlen(tmp);
			if(len && (tmp[len - 1] == '\n')) {
				lineNumber++;
				if(lineNumber != ln) {
					fputs(tmp, newFile);
				} else {
					size_t bufferSize = snprintf(NULL, 0, "%s ", word) + 1;
					char *wordSpace = malloc(bufferSize);
					snprintf(wordSpace, bufferSize, "%s ", word);
					removeWord(tmp, wordSpace);
					int wordVal = atoi(tmp);
					wordVal++;
					bufferSize = snprintf(NULL, 0, "%s %d\n", word, wordVal) + 1;
					wordSpace = malloc(bufferSize);
					snprintf(wordSpace, bufferSize, "%s %d\n", word, wordVal);
					fputs(wordSpace, newFile);
				}
			}
		}

		if(oldFile != NULL) {
			fclose(oldFile);
		}

		if(newFile != NULL) {
			fclose(newFile);
		}

		if(remove(getFileLocation(0)) != 0) {
			fprintf(stderr, "Could not remove %s\n", getFileLocation(0));
			return EXIT_FAILURE;
		}

		if(rename(getFileLocation(1), getFileLocation(0)) == -1) {
			fprintf(stderr, "Could not rename %s to %s\n", getFileLocation(1), getFileLocation(0));
			fprintf(stderr, "%d: %s\n", errno, strerror(errno));
			return EXIT_FAILURE;
		}
	} else {
		FILE *fw = fopen(getFileLocation(0), "a");
		fprintf(fw, "%s %d\n", word, 1);
	}

	return EXIT_SUCCESS;
}

int addProfanityWord(char *word) {
	size_t bufferSize = snprintf(NULL, 0, "%s/.bash_aliases", getenv("HOME")) + 1;
	char *file = malloc(bufferSize);
	snprintf(file, bufferSize, "%s/.bash_aliases", getenv("HOME"));
	
	FILE *aliases = fopen(file, "a");

	bufferSize = snprintf(NULL, 0, "alias %s='profanity add %s';\n", word, word) + 1;
	char *newAlias = malloc(bufferSize);
	snprintf(newAlias, bufferSize, "alias %s='profanity add %s';\n", word, word);

	fputs(newAlias, aliases);
	fclose(aliases);

	printf("added %s to the list of no-no words.\n", word);
}

int getProfanity(char *word) {
	checkFile(getFileLocation(0));

	FILE *fr = fopen(getFileLocation(0), "r");
	int found = 0;
	char tmp[BUFSIZE] = {0x0};

	if(fr == NULL) {
		printf("couldn't find \"%s\" in %s\n", word, getFileLocation(0));
		return EXIT_FAILURE;
	}

	if(strcmp(word, "all") == 0) {
		while(fr != NULL && fgets(tmp, sizeof(tmp), fr) != NULL) {
			removeNewlines(tmp);
			printf("You have said %s time(s)\n", tmp);
		}
		return EXIT_SUCCESS;
	}

	printf("getting number of times \"%s\" is said from %s\n", word, getFileLocation(0));

	while(fr != NULL && fgets(tmp, sizeof(tmp), fr) != NULL) {
		if(strstr(tmp, word)) {
			removeNewlines(tmp);
			printf("You have said %s time(s)\n", tmp);
			found = 1;
		}
	}

	if(fr != NULL) {
		fclose(fr);
	}

	if(found == 0) {
		printf("You have not said %s. Yet...\n", word);
	}

	return EXIT_SUCCESS;
}

int remProfanity(char *word) {
	checkFile(getFileLocation(0));

	if(strcmp(word, "all") == 0) {
		printf("removing all occurences of profane behaviour from %s\n", getFileLocation(0));

		if(remove(getFileLocation(0)) != 0) {
			fprintf(stderr, "Could not remove %s\n", getFileLocation(0));
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	printf("removing all occurences of \"%s\" from %s\n", word, getFileLocation(0));

	FILE *fr = fopen(getFileLocation(0), "r");
	int ln = 0;
	int found = 0;
	char tmp[BUFSIZE] = {0x0};

	if(fr == NULL) {
		printf("couldn't remove \"%s\" from %s\n", word, getFileLocation(0));
		return EXIT_FAILURE;
	}

	while(fr != NULL && fgets(tmp, sizeof(tmp), fr) != NULL) {
		++ln;

		if(strstr(tmp, word)) {
			found = 1;
		}
	}

	if(found == 1) {
		FILE *oldFile;
		FILE *newFile;

		if((oldFile = fopen(getFileLocation(0), "r")) == NULL) {
			fprintf(stderr, "Could not open %s for reading.\n", getFileLocation(0));
			return EXIT_FAILURE;
		}

		if((newFile = fopen(getFileLocation(1), "w")) == NULL) {
			fprintf(stderr, "Could not open %s for writing.\n", getFileLocation(1));
			return EXIT_FAILURE;
		}

		int lineNumber = 0;
		int len;
		char tmp[BUFSIZE] = {0x0};
		
		while(fgets(tmp, sizeof(tmp), oldFile) != NULL) {
			len = strlen(tmp);
			if(len && (tmp[len - 1] == '\n')) {
				lineNumber++;
				if(lineNumber != ln) {
					fputs(tmp, newFile);
				}
			}
		}

		if(oldFile != NULL) {
			fclose(oldFile);
		}

		if(newFile != NULL) {
			fclose(newFile);
		}

		if(remove(getFileLocation(0)) != 0) {
			fprintf(stderr, "Could not remove %s\n", getFileLocation(0));
			return EXIT_FAILURE;
		}

		if(rename(getFileLocation(1), getFileLocation(0)) == -1) {
			fprintf(stderr, "Could not rename %s to %s\n", getFileLocation(1), getFileLocation(0));
			fprintf(stderr, "%d: %s\n", errno, strerror(errno));
			return EXIT_FAILURE;
		}
	} else {
		printf("%s not found, so not removed from %s\n", word, getFileLocation(0));
		if(fr != NULL) {
			fclose(fr);
		}
	}

	return EXIT_SUCCESS;
}

int initProfanity() {
	checkFile(getFileLocation(0));
}

int help() {
	printf("profanity 0.1                                                                   \n");
	printf("Usage: profanity <command> [word]                                               \n");
	printf("                                                                                \n");
	printf("Commands:                                                                       \n");
	printf("   add     - adds or increments a word in the list.                             \n");
	printf("   addWord - adds an alias to bash_aliases with word as the alias.              \n");
	printf("   get     - gets the list of words, or a single word if specified.             \n");
	printf("   rem     - removes the list of words, or a single word if specified.          \n");
	printf("                                                                                \n");
}

int main(int argc, char** argv) {
	if(argc == 1) {
		help();
	} else {
		if(argc == 2) {
			     if(strcmp(argv[1], "get")   == 0) { getProfanity("all"); }
			else if(strcmp(argv[1], "rem")   == 0) { remProfanity("all"); }
			else if(strcmp(argv[1], "init")  == 0) { initProfanity();     }
			else if(strcmp(argv[1], "-h")    == 0
			     || strcmp(argv[1], "-help") == 0) { help();              }
			else {
				printf("\"%s\" is not a valid operator when using one argument.\n", argv[1]);
			}
		} else {
			char *word = argv[2];

			     if(strcmp(argv[1], "add")     == 0) { addProfanity(word);     }
			else if(strcmp(argv[1], "addWord") == 0) { addProfanityWord(word); }
			else if(strcmp(argv[1], "get")     == 0) { getProfanity(word);     }
			else if(strcmp(argv[1], "rem")     == 0) { remProfanity(word);     }
			else {
				printf("\"%s\" is not a valid operator for this program.\n", argv[1]);
			}
		}
	}

	return EXIT_SUCCESS;
}
