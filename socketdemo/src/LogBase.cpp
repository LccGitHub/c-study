/**
* Copyright @ 2015 - 2020 iAuto Software(Shanghai) Co., Ltd.
* All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are NOT permitted except as agreed by
* iAuto Software(Shanghai) Co., Ltd.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/


/********************************* include ************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>



#include "LogBase.h"


/********************************* Micro Define ********************************************/


/********************************* Static Constant Declaration  ****************************/



/********************************* Constant var or var Declaration  ************************/




/******************************** Static Function Declarations *****************************/






/******************************** Function Define ********************************************/


extern char *__progname;
namespace nutshell
{

#define MAX_LOG_SIZE 1028

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "socket"





void UComLog(const char* log_tag, UComLogTag tag, const char* func, int line, const char* format, ...)
{
    if (tag < LOG_LEVEL) {
        return; // when log level is lower, no need to printf
    }
    va_list arg;
    va_start(arg, format);
    char log[MAX_LOG_SIZE] = { 0 };
    char log_lever[8] = { 0 };

    int index = sprintf(log, "[%s][%s:%d]: ", log_tag, func, line);
    vsnprintf(&log[index], MAX_LOG_SIZE - index, format, arg);
    switch (tag) {
    case LOG_BASEV:
    {
        // ALOGV("%s", log);
        strncpy(log_lever, "V", 1);
        break;
    }
    case LOG_BASED:
    {
        // ALOGD("%s", log);
        strncpy(log_lever, "D", 1);
        break;
    }
    case LOG_BASEI:
    {
        // ALOGI("%s", log);
        strncpy(log_lever, "I", 1);
        break;
    }
    case LOG_BASEW:
    {
        // ALOGW("%s", log);
        strncpy(log_lever, "W", 1);
        break;
    }
    case LOG_BASEE:
    {
        // ALOGE("%s", log);
        strncpy(log_lever, "E", 1);
        break;
    }
    default:
    {
        break;
    }
    }

    va_end(arg);

    char buf_time[40] = { 0 };
    struct  timeb   stTimeb;
    ftime(&stTimeb);
    struct tm* tmp = localtime(&stTimeb.time);
    sprintf(buf_time, "%04d-%02d-%02d %02d:%02d:%02d.%03d", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday,
            tmp->tm_hour, tmp->tm_min, tmp->tm_sec, stTimeb.millitm);

    pid_t pid = getpid();
    


    /* when no need, can commet this line */
#if 0
    pid_t tid = gettid();
    printf("%s (%s)%s (%d,%d):%s\n", buf_time, log_lever, LOG_TAG, pid, tid, log);
#else
    printf("%s (%s)%s (%d):%s\n", buf_time, log_lever, LOG_TAG, pid, log);
#endif



}


} /* namespace nutshell */
/* EOF */
