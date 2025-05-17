#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <ftw.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>


// keep two arrays, one of time stamps one of hash values
// if time stamps differ then compare the hashed value
// if the hash is different a file was deleted
// if it matches then it was modified

//int i = 0;
long time_stamps[512];
long hashes[512];
struct stat buff;
int file_changed = 0;
char name[512];
pid_t server_pid = -1;
int current_file_index = 0;

int hash (char*) {

}

bool has_extension(const char *filename, const char *extension){
	const char *dot = strrchr(filename,'.');
	if (dot == NULL || dot[1] == '\0') {
		return false;
	}
//	printf("extension %s\n",dot);
	return strcmp(dot+1,extension)==0;
}

int display_info(const char *fpath, const struct stat *sb, int typeflag) {
	bool root_dir = strcmp(".",fpath)==0;
	bool in_git_dir = strncmp("./.git",fpath,5)==0;
	if(root_dir || in_git_dir) {
		return 0;
	}

	int file_index = current_file_index++;
	if(stat(fpath,&buff) == 0) {
		if(time_stamps[file_index] != buff.st_mtime) {
			if(has_extension(fpath, "c")){
			printf("%d -%ld- =%ld=\n",file_index,time_stamps[file_index],buff.st_mtime);
			strcpy(name,fpath);
			time_stamps[file_index] = buff.st_mtime;
			file_changed = 1;
			return 1;
			}
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

			if(server_pid > 0) {
				kill(server_pid, SIGTERM);
				int time_out = 0;
				while (time_out < 100000) {
					if(kill(server_pid,0)==0) {
						time_out++;
					}
					if(time_out > 10000){
						kill(server_pid, SIGKILL);	
					}
				}
				server_pid = -1;
			}
			const char *compile_cmd = "gcc server.c -o server";
			int result = system(compile_cmd);
			if (result==0) {
				server_pid = fork();
				if(server_pid==0){
					execl("./server", "./server",NULL);
					perror("Failed to start server");
				}else if(server_pid < 0){
					perror("Failed to fork process");
				}
			}
			else {
				puts("build failed");
			}
		}
		usleep(10000);
	}
}
