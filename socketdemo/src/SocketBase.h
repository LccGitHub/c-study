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
#ifndef SOCKET_BASE_H
#define SOCKET_BASE_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "SocketComDef.h"

namespace nutshell {




// send result
enum E_SOCKET_EXIT_FLAG
{
    E_SOCKET_EXIT_FLAG_NONE = 0x0000,     //<  no exit
    E_SOCKET_EXIT_FLAG_USER = 0x0001,     //<  user requset exit
    E_SOCKET_EXIT_FLAG_ACCIDENT = 0x0001, //<  accident exit, need to retry
};

class SocketReplier;
class SocketBase
{
public:
    explicit SocketBase(char* const interface, char* const ip, int port);
    SocketBase() = delete;
    virtual ~SocketBase();

    static SocketBase* getInstance();

    void registerCallBack(SocketReplier* mReplier);
    void init(E_SOCKET_TYPE type);
    void sendDataReq(const uint8_t* data, uint32_t len);

    void deinit();

private:

    void setSocketFd(int fd);
    int getSocketFd(void);
    void initServerSocket(int fd, const char* ip, int port);
    void initClientSocket(int fd, const char* ip, int port);
    void recvData(void);
    bool isInterfaceUp(const char* interface);

private:

    static SocketBase* mInstance;
    SocketReplier* mSocketReplier;
    int mFd;
    int mServerFd;
    E_SOCKET_EXIT_FLAG mExitState;
    int mPipeFd[2];
    char* mInterface;
    char* mIp;
    int mPort;

};

class SocketReplier
{
public:
    /*
     * allLen: RecvBuffer data size
     * begin_seat: the begin seat of next data save in RecBuffer
     *
     */
    virtual void recvDataInd(uint32_t& allLen, uint32_t& begin_seat, uint8_t* RecBuffer) = 0;
    virtual void connectStateInd(E_SOCKET_STATE result) = 0;
    virtual void sendDataCfm(E_SOCKET_SEND_RES result) = 0;
};

} /* namespace */

#endif // SOCKET_BASE_H
/* EOF */
