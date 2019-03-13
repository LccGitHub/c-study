
#include <pthread.h>
#include <unistd.h>
#include "Handler.h"
#include "Cmd.h"



void* TaskEventHandler(uint16_t opcode, void* msg)
{
    printf("%s, %d, receive opcode %.2x.", __FUNCTION__, __LINE__, opcode);
    switch ((uint32_t)opcode) {
    case CMD1_REQ: {
        break;
    }
    default: {
        break;
    }
    }
    return NULL;
}

void* ThreadMain(void* arg)
{
    while (1) {
        MNG_REQ_MESSAGE_T* msg = MessageLoop();
        if (msg == NULL) {
            printf("thread exit.");
            break;
        }

        TaskEventHandler(msg->opcode, (void*)&(msg->req));

        MessageFree();
    }
    MessageRelease();
    return NULL;
}

void* testFun1(void* arg)
{
    int i = 0;
    for (i = 0; i < 10; i++) {
        Cmd1Req();
        usleep(100*1000);
    }
    return NULL;
}

void* testFun2(void* arg)
{
    int i = 0;
    for (i = 0; i < 10; i++) {
        Cm2Req(10, (const uint8_t*)"111");
        usleep(50*1000);
    }
    return NULL;
}

int main(void)
{
    MessageInit();
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, ThreadMain, NULL);

    pthread_t testThread1;
    pthread_create(&testThread1, NULL, testFun1, NULL);

    pthread_t testThread2;
    pthread_create(&testThread2, NULL, testFun2, NULL);
    
    pthread_join(mainThread,NULL);
    pthread_join(testThread1,NULL);
    pthread_join(testThread2,NULL);

    return 0;
}
