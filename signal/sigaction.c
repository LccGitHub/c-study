#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
void dealFunc(int sig)
{
	printf("\n have receive Ctrl+C \n");
}
int main()
{
	struct sigaction act;
	act.sa_handler = dealFunc;

	//act.sa_flags = SA_RESETHAND;

	sigaction(SIGINT, &act, 0);
	while(1) {
		printf("I am runing \n");
		sleep(1);
	}
	return 0;
}

