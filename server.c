#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>

int main() {

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0x901f;
	addr.sin_addr.s_addr = INADDR_ANY;
	int s = socket(AF_INET,SOCK_STREAM,0);
	if( bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		perror("socket creation failed");
		return 1;
	}

	listen(s,10);

	int client_fd = accept(s, 0, 0);
	char buffer[256] = {0};
	recv(client_fd, buffer, 256, 0);

	char* f = buffer + 5;
	*strchr(f, ' ') = 0;

	int opened_fd = open(f, O_RDONLY);
	
	sendfile(client_fd,opened_fd,0,256);
	close(opened_fd);
	close(client_fd);
	close(s);

 	return 0;
}
