#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
int father_loop = 1;
pid_t pid;
void signal_handler(int sig)
{
	printf("have receive sig[%d]\n", sig);
	pid_t pid_t;
#if 0
	while(pid = waitpid(pid, NULL, WNOHANG) > 0){
		printf("SIGCLHD pid %d\n", pid);
	}
#endif
	wait(pid, NULL, 0);
	printf("SIGCLHD pid %d\n", pid);
	father_loop = 0;
}
int main()
{
	pid = fork();
	signal(SIGCHLD, signal_handler);
	if (pid< 0) {
		printf("fork error \n");
	}
	else if (pid > 0) {
		printf("this is father process \n");
		while(father_loop){
			sleep(1);
		}
		//return 0;
	}
	else {
		printf("this is child process \n");
		int i = 0;
		for (i = 0; i< 10; i++) {
			printf("child is runing \n");
			sleep(1);
		}
		exit(-1);
	}
	printf("main have exit \n");
	return 0;
}
