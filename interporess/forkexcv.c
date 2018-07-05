#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
int main()
{
	pid_t pid = fork();
	signal(SIGCHLD, SIG_IGN);
	if (pid< 0) {
		printf("fork error \n");
	}
	else if (pid > 0) {
		printf("this is father process \n");
	}
	else {
		printf("this is child process \n");
		char* argv[] = {"/bin/ls", NULL};
		if (execv("/bin/ls", argv) < 0) {
			exit(-1);
		}
	}
	printf("main have exit \n");
	return 0;
}
