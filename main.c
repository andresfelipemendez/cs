#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <ftw.h>
#include <string.h>
#include <unistd.h>
// keep two arrays, one of time stamps one of hash values
// if time stamps differ then compare the hashed value
// if the hash is different a file was deleted
// if it matches then it was modified

int i = 0;
long time_stamps[512];
long hashes[512];
struct stat buff;
int file_changed = 0;
char name[512];

int current_file_index = 0;

int hash (char*) {

}

int display_info(const char *fpath, const struct stat *sb, int typeflag) {
	
	if(strcmp(".",fpath)==0) {
		return 0;
	}
	if(strncmp("./.git",fpath,5)==0) {
		return 0;
	}

	int file_index = current_file_index++;
	if(stat(fpath,&buff) == 0) {
		if(time_stamps[file_index] != buff.st_mtime) {
			printf("%d -%d- =%d=\n",file_index,time_stamps[file_index],buff.st_mtime);
			strcpy(name,fpath);
			time_stamps[file_index] = buff.st_mtime;
			file_changed = 1;
			return 1;
		}
	}

	return 0;
}

int main() {

	while(true) {
		file_changed = 0;
		current_file_index = 0;

		ftw(".", display_info, 20);

		if(file_changed) {
			char *time_str = ctime(&buff.st_mtime);
			time_str[strlen(time_str) - 1] = '\0';
			printf("changed %s %s\n",time_str, name);
		}
		usleep(10000);
	}
}