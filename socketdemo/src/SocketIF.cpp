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


// g++ src/*.cpp -I./include -std=c++11  --shared -fPIC -o libsocketif.so

#include "SocketIF.h"
#include "SocketLog.h"
#include "SocketImp.h"



namespace nutshell {


SocketIF::SocketIF(const char* const interface, const char* const ip, int port)
    : mImp(NULL)
{
    mImp = new SocketImp(this, interface, ip, port);
    SOCKET_LOG_I("SocketIF::IF[%s]IP[%s]Port[%d]", interface, ip, port);
}

SocketIF::~SocketIF()
{

}


void SocketIF::start(E_SOCKET_TYPE type)
{
    if (mImp == NULL) {
        SOCKET_LOG_E(" SocketServer::start mImp is NULL");
    }
    else {
        mImp->start(type);
        SOCKET_LOG_I(" SocketServer::start, type = %d", type);
    }

}

void SocketIF::stop()
{
    if (mImp == NULL) {
        SOCKET_LOG_E(" SocketServer::stop mImp is NULL");
    }
    else {
        mImp->stop();
        SOCKET_LOG_I(" SocketServer::stop");
    }
}

void SocketIF::sendDataReq(const uint8_t* data, uint32_t len)
{
    if (mImp == NULL) {
        sendDataCfm(E_SOCKET_SEND_RES_FAILED);
        SOCKET_LOG_E(" SocketServer::sendDataReq mImp is NULL");
    }
    else {
        mImp->sendDataReq(data, len);
    }    
}


} // namespace
