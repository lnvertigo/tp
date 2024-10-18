#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

// -h message
void printh() {
	printf("NAME\n\t%s", "tp - teleport to user-defined flags\n");
	printf("--------------------------------\n");
	printf("SYNOPSIS\n\ttp [OPTION]...\n");
	printf("DESCRIPTION\n\t%s %s\n", \
			"Jump to a directory defined by a flag named by the user.", \
			"The flags are stored in a file called .tpfile in the home directory.");
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
		perror("getenv failed");
		exit(1);
	}
	char *filename = "/.tpfile";

	size_t len = strlen(homedir) + strlen(filename) + 1;

	char *path = malloc(len);
	if (path == NULL) {
		perror("malloc failed");
		exit(1);
	}
	snprintf(path, len, "%s%s", homedir, filename);
	printf("tpFilePath() path addr: %p\n", path);
	return path;
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
	int c;
	char *arg_value = NULL;
	char *filepath;
	FILE* datafile;

	// references(?) address from function to free properly l8r on
	filepath = tpFilePath();
	datafile = fopen(filepath, "a");
	if (datafile == NULL) {
		perror("fopen failed");
		return 1;
	}
	printf("main filepath addr: %p\n", datafile);

	// flag options
	while ((c = getopt(argc, argv, "ha:r:xl::")) != -1) {
		switch (c) {
			case 'h':   // -h help
				hflag = 1;
				printh();
				break;
			case 'a':   // -a [flagname] [dir] - add flag, default is home dir
				aflag = 1;
				arg_value = optarg;
				break;
			case 'r':   // -r [flagname] - remove flag
				rflag = 1;
				arg_value = optarg;
				break;
			case 'x':   // -x removeall flags
				xflag = 1;
				break;
			case 'l':   // -l (no args=listall, flag = flag + dir)
				lflag = 1;
				arg_value = optarg;
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
		// DEBUG
		printf("h:%d, a:%d, r:%d, x:%d, l:%d, arg:%s | ", 
			hflag, aflag, rflag, xflag, lflag, arg_value);

		for (int index = optind; index < argc; index++)
			printf("Non-option arg %s\n", argv[index]);
		return 0;
	}

	fclose(datafile);
	free(filepath);
}
