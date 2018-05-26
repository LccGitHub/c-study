/*************************************************************************
  > File Name: printA-b.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: Sat 26 May 2018 05:05:26 PM CST
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>

pthread_cond_t cond;
pthread_mutex_t lock;
static int flag = 0;

void* printA(void* argv)
{
	while(1) {
		pthread_mutex_lock(&lock);
		if (flag == 0) {
			printf("A\n");
			flag = 1;
			pthread_cond_signal(&cond);
		}
		pthread_mutex_unlock(&lock);
	}
}

void* printB(void* argv)
{
	while(1) {
//		printf("printB, flag = %d \n", flag);
		pthread_mutex_lock(&lock);
		while(flag == 0 )  {
			pthread_cond_wait(&cond, &lock);
		}
		printf("B\n");
		flag = 0;
		pthread_mutex_unlock(&lock);
	}
}


int main()
{

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_t threadA, threadB;
	pthread_create(&threadA, NULL, printA, NULL);
	pthread_create(&threadB, NULL, printB, NULL);

	pthread_join(threadA, NULL);
	pthread_join(threadB, NULL);
	return 0;
}

