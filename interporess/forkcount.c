#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/unistd.h>

int main()
{
	fork();
	fork() && fork() || fork();
	fork();
	printf("+\n");/*must add \n*/
	return 0;
}

