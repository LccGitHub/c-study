#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>

int main()
{
	char buf[128] = {0};
	memset(buf, 0, sizeof(buf));
	int socket_pair[2];
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair) == -1) {
		printf("socketpair failed \n");
		return 0;
	}
	pid_t pid = fork();
	if (pid < 0) {
		printf("fork failed \n");
	}
	else if (pid > 0) {
		int size = write(socket_pair[0], "hello", strlen("hello"));
		printf("fathter process write success, pid = %d\n", getpid());
		/*if no wite to socket_pair[0], read socket_pair[0] will be block!!!*/
		//read(socket_pair[0], buf, sizeof(buf));
		//printf("father process read [%s], pid = %d\n", buf, getpid());
	}
	else {
		read(socket_pair[1], buf, sizeof(buf));
		printf("child process read result [%s] ,pid = %d\n", buf, getpid());
	}
	while(1){
		sleep(1);
	}
	close(socket_pair[0]);
	close(socket_pair[1]);
	return 0;
}

