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
/**
 * @file UpdateInterface.h
 * @brief Declaration file of class UpdateInterface.
 */
#ifndef SOCKET_IMPL_H
#define SOCKET_IMPL_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <pthread.h>

#include "SocketIF.h"
#include "SocketBase.h"

namespace nutshell {



class SocketImp: public SocketBase, public SocketReplier
{
public:
    explicit SocketImp(SocketIF* IF, const char* interface, const char* ip, const int port);
    SocketImp() = delete; 
    virtual ~SocketImp();


    void start(E_SOCKET_TYPE type);
    void stop();

    virtual void recvDataInd(uint32_t& allLen, uint32_t& begin_seat, uint8_t* RecBuffer) override;
    virtual void connectStateInd(E_SOCKET_STATE result) override;
    virtual void sendDataCfm(E_SOCKET_SEND_RES result) override;

private:

    static void* threadHandler(void* arg);

private:

    pthread_t mThread;
    SocketIF* mIF;


private:


};


} /* namespace */

#endif // SOCKET_IMPL_H
/* EOF */
