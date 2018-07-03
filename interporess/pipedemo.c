#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

int myPipe[2];

void* func(void *args)
{
	printf("this is func \n");
	int loop = 1;
	char buffer[256] = {0};
	while(loop) {
		memset(buffer, 0, sizeof(buffer));
		int res = read(myPipe[0], buffer, sizeof(buffer) - 1);
		printf("have read buffer[%s][%d] \n", buffer, res);
		if (strcmp(buffer, "exit") == 0) {
			loop = 0;
		}
		sleep(1);
	}
	return NULL;
}

int main()
{
	pthread_t thr;
	pipe(myPipe);
	pthread_create(&thr, NULL, func, NULL);
	while(1) {
		char input[256] = {0};
		printf("please input:\n");
		scanf("%s", input);
		printf("have input buffer[%s] \n", input);
		int res = write(myPipe[1], input, strlen(input));
		printf("have write buffer[%s][%d] \n", input, res);
		sleep(1);
	}
	return 0;
}
