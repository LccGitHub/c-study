

/* ------------------------------- includes --------------------------------- */

#include <pthread.h>
#include "Handler.h"

/* -------------------------------- Defines --------------------------------- */

/* ----------------------------- Data Types ------------------------------- */

/* --------------------------- Routine prototypes --------------------------- */


static void RdCondWait(void);
static void RdCondSignal(void);
static void WrCondWait(void);
static void WrCondSignal(void);
static uint32_t NextSign(uint32_t sign);
static bool IsArrayEmpty(void);
static bool IsArraryFull(void);



/* ---------------------------- Global Variables ---------------------------- */

#define MAX_MESSAGE_CNT  ((uint32_t)11)

static MNG_REQ_MESSAGE_T MsgArray[MAX_MESSAGE_CNT];
/* Next sign should be read*/
static uint32_t read_sign = 0;
/*Current sign should be write*/
static uint32_t write_sign = 0;

static bool rd_cond_sign = false;
static pthread_cond_t rd_cond;
static pthread_mutex_t rd_mutex;

static bool wr_cond_sign = false;
static pthread_cond_t wr_cond;
static pthread_mutex_t wr_mutex;

static pthread_mutex_t send_mutex;

static bool loop_exit = true;

/* -------------------------------- Routines -------------------------------- */

static void RdCondWait(void)
{
    pthread_mutex_lock(&rd_mutex);
    if (!rd_cond_sign) {
        pthread_cond_wait(&rd_cond, &rd_mutex);
    }
    else {
        rd_cond_sign = false;
    }
    pthread_mutex_unlock(&rd_mutex);
}

static void RdCondSignal(void)
{
    pthread_mutex_lock(&rd_mutex);
    rd_cond_sign = true;
    pthread_cond_signal(&rd_cond);
    pthread_mutex_unlock(&rd_mutex);
}

static void WrCondWait(void)
{
    pthread_mutex_lock(&wr_mutex);
    if (!wr_cond_sign) {
        pthread_cond_wait(&wr_cond, &wr_mutex);
    }
    else {
        wr_cond_sign = false;
    }
    pthread_mutex_unlock(&wr_mutex);
}

static void WrCondSignal(void)
{
    pthread_mutex_lock(&wr_mutex);
    wr_cond_sign = true;
    pthread_cond_signal(&wr_cond);
    pthread_mutex_unlock(&wr_mutex);
}

static uint32_t NextSign(uint32_t sign)
{
    return (uint32_t)((sign + (uint32_t)1) % MAX_MESSAGE_CNT);
}

static bool IsArrayEmpty(void)
{
    printf("%s read sign %d, write sign %d.\n", __func__, read_sign, write_sign);
    return (bool)(read_sign == write_sign);
}

static bool IsArraryFull(void)
{
    printf("%s read sign %d, write sign %d.\n", __func__, read_sign, write_sign);
    return (bool)(NextSign(write_sign) == read_sign);
}

void MessageInit(void)
{
    pthread_cond_init(&rd_cond, NULL);
    pthread_cond_init(&wr_cond, NULL);

    pthread_mutex_init(&rd_mutex, NULL);
    pthread_mutex_init(&wr_mutex, NULL);

    pthread_mutex_init(&send_mutex, NULL);

    write_sign = 0;
    read_sign = 0;
    loop_exit = false;
}

void MessageRelease( void )
{
    pthread_cond_destroy(&rd_cond);
    pthread_cond_destroy(&wr_cond);

    pthread_mutex_destroy(&rd_mutex);
    pthread_mutex_destroy(&wr_mutex);

    pthread_mutex_destroy(&send_mutex);

    write_sign = 0;
    read_sign = 0;
}

MNG_REQ_MESSAGE_T* MessageLoop(void)
{
    MNG_REQ_MESSAGE_T* ret = NULL;
    while ((!loop_exit) && (IsArrayEmpty())) {
        printf("%s array is empty, Wait Send New Message\n", __func__);
        RdCondWait();
        printf("%s wake up\n", __func__);
    }

    if (!loop_exit) {
        ret = &MsgArray[read_sign];
    }

    return ret;
}

void MessageLoopExit(void)
{
    loop_exit = true;
    RdCondSignal();
}

void MessageFree(void)
{
    /*Must be current read message*/
    read_sign = NextSign(read_sign);

    /*Notify Alloc Message*/
    WrCondSignal();
}

bool MessageSend(MNG_REQ_MESSAGE_T* _msg)
{
    bool ret = true;
    if (!loop_exit) {
        printf("%s with opcode %.2x \n", __func__, _msg->opcode);

        while (1) {
            pthread_mutex_lock(&send_mutex);
            /*double check*/
            if (!IsArraryFull()) {
                /*Copy _msg to array*/
                memcpy(&MsgArray[write_sign], _msg, sizeof(MNG_REQ_MESSAGE_T));
                write_sign = NextSign(write_sign);
                pthread_mutex_unlock(&send_mutex);
                RdCondSignal();
                break;
            }
            else {
                pthread_mutex_unlock(&send_mutex);
                printf("%s array is Full, wait for message free.\n", __func__);
                WrCondWait();
            }
        }
    }
    else {
        printf("%s thread is exited.\n", __func__);
        ret = false;
    }

    return ret;
}

