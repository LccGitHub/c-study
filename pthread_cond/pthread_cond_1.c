
#include<stdio.h>
#include<pthread.h>

int cnt = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

void *reproduce(void* arg)
{
	while(1){
		pthread_mutex_lock(&mutex);
		printf("%s,  lock \n", __func__);
		cnt++;
		//printf("%s, cnt = %d \n", __func__, cnt);
		printf("A \n");
		if(cnt)
		{
			pthread_cond_signal(&cond);
		}

		printf("%s, unlock \n", __func__);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return NULL;
}

void *consumer(void* arg)
{
	while(1){
		pthread_mutex_lock(&mutex);
		printf("%s,  lock \n", __func__);
		while(cnt <= 0)
		{
			pthread_cond_wait(&cond, &mutex);
		}

		cnt --;
		printf("B \n");
		//printf("%s, cnt = %d \n", __func__, cnt);
		printf("%s, unlock \n", __func__);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return NULL;
}

int main()
{
	pthread_t conThread, repThread;

	pthread_create(&conThread, NULL, consumer, NULL);
	pthread_create(&repThread, NULL, reproduce, NULL);

	pthread_join(conThread, NULL);
	pthread_join(repThread, NULL);

	pthread_cond_destroy(&cond);

	return 0;
}
