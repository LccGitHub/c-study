
#include<stdio.h>
#include<pthread.h>

int cnt = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

void *reproduce(void* arg)
{
	pthread_mutex_lock(&mutex);
	printf("%s,  lock \n", __func__);
	cnt++;
	printf("%s, cnt = %d \n", __func__, cnt);
	if(cnt)
	{
		pthread_cond_signal(&cond);
	}

	printf("%s, unlock \n", __func__);
	pthread_mutex_unlock(&mutex);

	return NULL;
}

void *consumer(void* arg)
{
	pthread_mutex_lock(&mutex);
	printf("%s,  lock \n", __func__);
	if(cnt <= 0)
	{
		pthread_cond_wait(&cond, &mutex);
	}

	cnt --;
	printf("%s, cnt = %d \n", __func__, cnt);
	printf("%s, unlock \n", __func__);
	pthread_mutex_unlock(&mutex);

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
