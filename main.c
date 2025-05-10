#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

int main() {
	DIR *dir;
	dir = opendir(".");
	if(dir == NULL) {
		perror("unable to open directory");
		return EXIT_FAILURE;
	}

	printf("Files in the current directory:\n");

	struct dirent *entry;
	errno = 0;
	while((entry = readdir(dir)) != NULL) {
		printf("%s\n", entry->d_name);
	}

	if(errno != 0) {
		perror("Error reading directory");
		closedir(dir);
		return EXIT_FAILURE;
	}

	closedir(dir);
	return EXIT_SUCCESS;
// system("gcc -c -Wall -Werror -fpic server.c")
}