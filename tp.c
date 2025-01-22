#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

// -h message
void printh() {
	printf("NAME\n\t%s", "tp - teleport to user-defined flags\n");
	printf("--------------------------------\n");
	printf("SYNOPSIS\n\ttp [OPTION]...\n");
	printf("DESCRIPTION\n\t%s %s\n", \
			"Jump to a directory defined by a flag named by the user.", \
			"The flags are stored in a file called .tpfile in the home directory.", \
			"The delimiter for the file is comma (,)");
	printf("\t%s\n\t\t%s\n\n", "-h", 					"Display this message");
	printf("\t%s\n\t\t%s\n\n", "-a [flagname] [path]", 	"Add a new entry");
	printf("\t%s\n\t\t%s\n\n", "-r [flagname]", 		"Remove the given entry");
	printf("\t%s\n\t\t%s\n\n", "-x", 					"Remove all known entries");
	printf("\t%s\n\t\t%s\n\n", "-l [flagname]", 		"List flag if known, display all otherwise");
	printf("--------------------------------\n");
}

// malloc complete filepath to ~/.tpfile
char *tpFilePath() {
	char *homedir = getenv("HOME");
	if (homedir == NULL) {
		perror("getenv failed\n");
		exit(1);
	}
	char *filename = "/.tpfile";

	size_t len = strlen(homedir) + strlen(filename) + 1;

	char *path = malloc(len);
	if (path == NULL) {
		perror("malloc failed\n");
		exit(1);
	}
	snprintf(path, len, "%s%s", homedir, filename);
	printf("debug: tpFilePath() path addr: %p\n", path);
	return path;
}


int addEntry(FILE *file, char *flag, char *path) {
	fprintf(flag, file);
	fprintf(",", file);
	fprintf(path, file);
	fprintf("\n", file);
	return 0;
}

int removeEntry(FILE *file, char *flag) {
	char *tmpfilename = "tmp.txt";
	FILE *tempfile = fopen(tmpfilename, "w");
	if (tempfile == NULL) {
		perror("Error creating tempfile in -r\n");
		return 1;
	}
	
	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), file)) {
		if (strstr(line, flag) == NULL) {
			fputs(line, tempfile);
		}
	}

	// look for newline char?
    // search_string[strcspn(search_string, "\n")] = 0; 

	fclose(tempfile);
	fclose(file);

	remove(file);
	rename(tempfile, file);
	return 0;
}

int removeAll(FILE *file) {
	fclose(file);
	remove(file);
	return 0;
}

int listEntries(FILE *file, char *flag) {
	if (flag == NULL) {
		perror("listEntries attempted to read NULL\n");
		return 1;
	}

	char *readflag[100];
	char *readpath[100];
	while (fscanf(file, "%s,%s", readflag, readpath) == 1) {
		if(strstr(readflag, flag) != 0) {//if match found
			printf("%s,%s", readflag, readpath);
		}
	}

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printh();
	}

	int hflag = 0;  // -h help
	int aflag = 0;  // -a [flagname] [dir] - add userflag, default dir is $home
	int rflag = 0;  // -r [flagname] - remove listed userflag
	int xflag = 0;  // -x removeall userflags
	int lflag = 0;  // -l (no args=listall, flag = flag + dir)
	int op;
	char *arg1 = NULL;
	char *arg2 = NULL;
	char *filepath;
	FILE* datafile;

	// references(?) address from function to free properly l8r on
	filepath = tpFilePath();
	datafile = fopen(filepath, "a");
	if (datafile == NULL) {
		perror("fopen failed\n");
		return 1;
	}
	printf("debug: main filepath addr: %p\n", datafile);

	// flag options
	while ((op = getopt(argc, argv, "ha:r:xl::")) != -1) {
		switch (op) {
			case 'h':   // -h help
				hflag = 1;
				break;
			case 'a':   // -a [flagname] [dir] - add flag, default is home dir
				aflag = 1;
				arg1 = optarg;
				arg2 = argv[optind];
				optind++;
				break;
			case 'r':   // -r [flagname] - remove flag
				rflag = 1;
				arg1 = optarg;
				break;
			case 'x':   // -x removeall flags
				xflag = 1;
				break;
			case 'l':   // -l (no args=listall, flag = flag + dir)
				lflag = 1;
				arg1 = optarg;
				break;
			case '?':   // TODO etc + arg fix this
			if (optopt == 'a') 
				fprintf(stderr, "Option -%d requires an arg.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option '-%d'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character '-%d'.\n", optopt);
			return 1;
		default:
			abort();
		}

		// DEBUGGING
		printf("h:%d, a:%d, r:%d, x:%d, l:%d, arg:%s | \n", 
			hflag, aflag, rflag, xflag, lflag, arg1);
		for (; optind < argc; optind++)
			printf("Non-option arg %s\n\n", argv[optind]);
	}
	// ensure only 1 flag is used as input
	if (hflag + aflag + rflag + xflag + lflag > 1) {
		perror("Only 1 flag allowed per use.\n");
		fclose(datafile);
		free(filepath);
		return 1;
	}

	if (hflag) {
		printh();
	} else if (aflag) {
		addEntry(datafile, arg1, arg2);
	} else if (rflag) {
		removeEntry(datafile, arg1);
	} else if (xflag) {
		removeAll(datafile);
	} else if (lflag) {
		listEntries(datafile, arg1);
	}

	printf("debug: Ending tpFilePath() addr: %p\ndebug: Ending main filepath addr: %p", filepath, datafile);
	fclose(datafile);
	free(filepath);
	return 0;
}
