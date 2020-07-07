/**
 * Copyright @ 2020 -  Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include <string.h>

#include "SocketImp.h"
#include "SocketLog.h"



namespace nutshell {


SocketImp::SocketImp(SocketIF* IF, const char* interface, const char* ip, const int port)
    : SocketBase((char* const)interface, (char* const)ip, port)
    , mThread(-1)
    , mIF(IF)
{
    registerCallBack(this);
    SOCKET_LOG_I("SocketBase::IF[%s]IP[%s]Port[%d]", interface, ip, port);
}

SocketImp::~SocketImp()
{

}


void SocketImp::start(E_SOCKET_TYPE type)
{
    E_SOCKET_TYPE tmpeType = type;
    SOCKET_LOG_I(" SocketImp::start type[%d]", tmpeType);
    // create thread
    int ret = pthread_create(&mThread, NULL, threadHandler, &tmpeType);
    if (ret != 0) {
        SOCKET_LOG_I("[SocketImp] pthread_create failed... ");
    }
#if 0
    ret = pthread_detach(mThread);
    if (ret != 0) {
        SOCKET_LOG_E("[SocketImp] pthread_detach failed... ret[%d]", ret);
    }
    else {
        SOCKET_LOG_I("[SocketImp] pthread_detach success ");
    }
#endif
}

void SocketImp::stop()
{
    deinit();
    SOCKET_LOG_I(" SocketImp::stop");
    pthread_cancel(mThread);
    SOCKET_LOG_I(" SocketImp::pthread_cancel");
    pthread_join(mThread, NULL);
    SOCKET_LOG_I(" SocketImp::pthread_join");
}

void SocketImp::recvDataInd(uint32_t& allLen, uint32_t& begin_seat, uint8_t* RecBuffer)
{
    uint32_t datalen = 0;
    const uint32_t COMMAND_PACKET_HEAD = 0; // this is fixed header length
    const uint32_t COMMAND_PACKET_TAIL = 0;
    uint32_t COMMAND_PACKET_INFO = COMMAND_PACKET_HEAD + COMMAND_PACKET_TAIL;
    int dealPackMinLength = COMMAND_PACKET_HEAD;
    while (allLen > dealPackMinLength) {
#ifdef HAVE_HEADER_TAIL       
        if (RecBuffer[TYPE32_LOWL] == 0xA5 && RecBuffer[TYPE32_LOWH] == 0xFF && RecBuffer[TYPE32_HIGHL] == 0x5A && RecBuffer[TYPE32_HIGHH] == 0xFF) {

            int len = COMMAND_PACKET_HEAD + DATA_TYPE_LEN + DATA_SUB_TYPE_LEN + DATA_SUB_TYPE_1_LEN;
            int applicationDataLen = MCUARRAY2INT32(RecBuffer + len);
            datalen = DATA_TYPE_LEN + DATA_SUB_TYPE_LEN + DATA_SUB_TYPE_1_LEN + APPLICATION_DATA_LEN + CRC_CHECK_LEN + applicationDataLen;
            SOCKET_LOG_D("analysis data[%d, %d], the len:%d, allLen:%d", RecBuffer[2], RecBuffer[3], datalen, allLen);
        }
        else {
            SOCKET_LOG_E("[data protocol] the begin is error code, not 0xA5 0xFF 0x5A 0xFF, the val is:[0x%x, 0x%x, 0x%x, 0x%x]", RecBuffer[0], RecBuffer[1], RecBuffer[2], RecBuffer[3]);
            begin_seat = allLen - COMMAND_NEXT_SEAT; // delete not protocol package
            allLen = begin_seat;
            memcpy(RecBuffer, RecBuffer + COMMAND_NEXT_SEAT, allLen);
            continue;
        }
#else
        datalen = allLen;
#endif

        if ((datalen + COMMAND_PACKET_INFO) <= allLen) {
            uint8_t tempBuf[datalen + 1] = { 0 };
            memset(tempBuf, 0, sizeof(tempBuf));
            memcpy(tempBuf, RecBuffer + COMMAND_PACKET_HEAD, datalen);
            tempBuf[datalen] = '\0';

            // analyze data
            if (mIF != NULL) {
                mIF->recvDataInd(tempBuf, datalen);
            }
            else {
                SOCKET_LOG_E("SocketImp::mIF is NULL");  
            }

            begin_seat = allLen - datalen - COMMAND_PACKET_INFO;
            allLen = begin_seat;
            memcpy(RecBuffer, RecBuffer + datalen + COMMAND_PACKET_INFO, allLen);
        }
        else {
            begin_seat = allLen;
            break;
        }
    }

    if (allLen <= dealPackMinLength) {
        begin_seat = allLen;
    }
}

void SocketImp::connectStateInd(E_SOCKET_STATE result)
{
    if (mIF != NULL) {
        mIF->connectStateInd(result);
    }
    else {
        SOCKET_LOG_E("SocketImp::mIF is NULL");  
    }
}

void SocketImp::sendDataCfm(E_SOCKET_SEND_RES result)
{
    if (mIF != NULL) {
        mIF->sendDataCfm(result);
    }
    else {
        SOCKET_LOG_E("SocketImp::mIF is NULL");  
    }
}

void* SocketImp::threadHandler(void* arg)
{
    E_SOCKET_TYPE* type = (E_SOCKET_TYPE*)arg;
    SOCKET_LOG_I("[SocketImp] threadHandler start, type = %d", *type);
    if (E_SOCKET_TYPE_SERVER == *type) {
        SocketBase::getInstance()->init(E_SOCKET_TYPE_SERVER);
    }
    else if (E_SOCKET_TYPE_CLIENT == *type) {
        SocketBase::getInstance()->init(E_SOCKET_TYPE_CLIENT);
    }
    else {
        SOCKET_LOG_E("[SocketImp] threadHandler start, err type = %d", *type);
    }
}

} // namespace
