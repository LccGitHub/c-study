
#include<pthread.h>
#include<stdbool.h>

#define MAX_REPRODUCE 10

/* this is array to implement queue, read_sign=front, write=rear*/
static int buffer[MAX_REPRODUCE];
static int read_sign = 0;
static int write_sign = 0;

static bool rd_cond_sign = false;
static pthread_cond_t rd_cond;
static pthread_mutex_t rd_mutex;


static bool wr_cond_sign = false;
static pthread_cond_t wr_cond;
static pthread_mutex_t wr_mutex;

/*this is for multi thread to run this loop*/
static pthread_mutex_t reproduce_mutex;

static void RdCondWait();
static void RdCondSignal();
static void WrCondWait();
static void WrCondSignal();
static int NextSign(int sign);
static bool IsQueueEmpty();
static bool IsQueueFull();
static void Init();
static void Deinit();
static int ConsumerLoop();
static void OneReadFinish();
static void ReproduceLoop();
void* ConsumerThread(void* arg);
void* ReproduceThread(void* arg);



static void RdCondWait()
{
	pthread_mutex_lock(&rd_mutex);
	if (rd_cond_sign == false) {
		pthread_cond_wait(&rd_cond, &rd_mutex);
	}
	else {
		rd_cond_sign = false;
	}
	pthread_mutex_unlock(&rd_mutex);

}

static void RdCondSignal()
{
	pthread_mutex_lock(&rd_mutex);
	rd_cond_sign = true;
	pthread_cond_signal(&rd_cond);
	pthread_mutex_unlock(&rd_mutex);
}


static void WrCondWait()
{
	pthread_mutex_lock(&wr_mutex);
	if (wr_cond_sign == false) {
		pthread_cond_wait(&wr_cond, &wr_mutex);
	}
	else {
		wr_cond_sign = false;
	}
	pthread_mutex_unlock(&wr_mutex);

}

static void WrCondSignal()
{
	pthread_mutex_lock(&wr_mutex);
	wr_cond_sign = true;
	pthread_cond_signal(&wr_cond);
	pthread_mutex_unlock(&wr_mutex);
}

static int NextSign(int sign)
{
	return (sign + 1) % MAX_REPRODUCE;
}

static bool IsQueueEmpty()
{
	printf("[%s],read_sign = %d, write_sign = %d \n", __func__, read_sign, write_sign);
	return read_sign == write_sign;
}

static bool IsQueueFull()
{
	printf("[%s],read_sign = %d, write_sign = %d \n", __func__, read_sign, write_sign);
	return NextSign(write_sign) == read_sign;
}

static void Init()
{
	pthread_cond_init(&rd_cond, NULL);
	pthread_cond_init(&wr_cond, NULL);

	pthread_mutex_init(&rd_mutex, NULL);
	pthread_mutex_init(&wr_mutex, NULL);

	pthread_mutex_init(&reproduce_mutex, NULL);


	read_sign = 0;
	write_sign = 0;
}


static void Deinit()
{
	pthread_cond_destroy(&rd_cond);
	pthread_cond_destroy(&wr_cond);

	pthread_mutex_destroy(&rd_mutex);
	pthread_mutex_destroy(&wr_mutex);
	pthread_mutex_destroy(&reprodec_mutex);


	read_sign = 0;
	write_sign = 0;
}


static int ConsumerLoop()
{
	int ret = -1;
	while(IsQueueEmpty()) {
		printf("queue is empty , should wait...\n");
		RdCondWait();
		printf("RdCondWait is wakup! \n");

	}

	ret = buffer[read_sign];
	return ret;
}


static void OneReadFinish()
{
	read_sign = NextSign(read_sign);

	WrCondSignal();
	printf("one read finish , send write signal \n");
}

static void ReproduceLoop()
{
	static cnt = 1;
	while(1) {
		pthread_mutex_lock(&reproduce_mutex);
		if (IsQueueFull()){
			printf("queue is full, write cond wait");
			WrCondWait();
			pthread_mutex_unlock(&reproduce_mutex);
		}
		else {
			buffer[write_sign] = cnt;
			cnt++;
			write_sign = NextSign(write_sign);
			RdCondSignal();
			pthread_mutex_unlock(&reproduce_mutex);
			break;
		}
	}
}

void* ConsumerThread(void* arg)
{
	while(1) {
		int ret = ConsumerLoop();
		printf("[%s], read result is %d \n", __func__, ret);
		OneReadFinish();
	}
	Deinit();
	return NULL;
}

void* ReproduceThread(void* arg)
{
	while(1) {
		printf("this is reproduce thread ... \n");
		ReproduceLoop();
		sleep(1);
	}
	return NULL;
}

int main()
{
	pthread_t thr_consumer, thr_rep1, thr_rep2, thr_rep3;
	pthread_create(&thr_consumer, NULL, ConsumerThread, NULL);
	pthread_create(&thr_rep1, NULL, ReproduceThread, NULL);
	pthread_create(&thr_rep2, NULL, ReproduceThread, NULL);

	pthread_join(thr_consumer, NULL);
	pthread_join(thr_rep1, NULL);

	return 0;
}
