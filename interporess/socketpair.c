#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<pthread.h>
#include<string.h>
int sv[2];
void* func(void* args)
{
	int loop = 1;
	while(loop) {
		char buffer[256] = {0};
		memset(buffer, 0, sizeof(buffer));
		int res = read(sv[1], buffer, sizeof(buffer) - 1);
		printf("fuc have recevie [%s]\n", buffer);
		res = write(sv[1], "fuc receviced", 30);
		if (strcmp(buffer, "exit") == 0)
			loop = 0;
	}
	return NULL;
}

int main()
{
	pthread_t thr;
	pthread_create(&thr, NULL, func, NULL);
	socketpair(AF_UNIX,	SOCK_STREAM, 0, sv);
	while(1) {
		char buff[256] = {0};
		memset(buff, 0, sizeof(buff));
		printf("please input you need send data\n");
		scanf("%s", buff);
		int res = write(sv[0], buff, strlen(buff));
		int loop = 1;
		while(loop){
			memset(buff,0, sizeof(buff));
			res = read(sv[0], buff, sizeof(buff)-1);
			printf("main read [%s]\n", buff);
			loop = 0;
		}
	}
	return 0;
}

