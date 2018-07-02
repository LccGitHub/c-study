#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
void dealFunc(int sig) 
{
	printf("\n have receive Ctrl+C \n");
	signal(SIGINT, SIG_DFL);
}
int main()
{
	signal(SIGINT, dealFunc);
	while(1) {
		printf("I am running \n");
		sleep(1);
	}
	return 0;
}
