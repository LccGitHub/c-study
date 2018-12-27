/**
 * gcc timer.c -lrt
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define CLOCKID (12) // can define different vaule


void timerThread(union sigval v)
{
	struct timespec t =
	{
		0,
		0
	};

	clock_gettime(CLOCK_REALTIME, &t);
	printf("tick = %d.%d\n", (int)t.tv_sec, (int)t.tv_nsec);
	printf("timer_thread function. %d\n", v.sival_int);
}

int main(void)
{
	timer_t timerId;
	struct sigevent evp;
	
	memset(&evp, 0, sizeof(struct sigevent));
	evp.sigev_value.sival_int = 111; // this can be passed into callback
	evp.sigev_notify = SIGEV_THREAD; // The thread notifying way,  dispath a new thread
	evp.sigev_notify_function = timerThread; // The thread address

	if (timer_create(CLOCKID, &evp, &timerId) == -1) {
		perror("timer_create filed:\n");
		exit(-1);
	}

	struct itimerspec it;
	it.it_interval.tv_sec = 5; 
	it.it_interval.tv_nsec = 0;
	it.it_value.tv_sec = 5;
	it.it_value.tv_nsec = 0;

	if (timer_settime(timerId, 0, &it, NULL) == -1) {
		printf("timer_settime failed\n");
	}

	pause();

	return 0;




	return 0;
}



