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

#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <net/if.h>

#include <poll.h>

#include "SocketBase.h"
#include "SocketLog.h"


#define BUFFER_MAX_LEN 1*1024 // 1k

namespace nutshell {


uint8_t gRecBuffer[BUFFER_MAX_LEN];    
SocketBase* SocketBase::mInstance = NULL;
SocketBase::SocketBase(char* const interface, char* const ip, int port)
    : mSocketReplier(NULL)
    , mFd(-1)
    , mServerFd(-1)
    , mExitState(E_SOCKET_EXIT_FLAG_NONE)
    , mPipeFd()
    , mInterface(interface)
    , mIp(ip)
    , mPort(port)
{
    mInstance = this;
    if (NULL != mInterface && NULL != mIp) {
        SOCKET_LOG_I("SocketBase::IF[%s]IP[%s]Port[%d]", mInterface, mIp, mPort);
    }
    else if (NULL == mInterface && NULL != mIp) {
        SOCKET_LOG_I("SocketBase::IF[none]IP[%s]Port[%d]", mIp, mPort);
    }
    else if (NULL != mInterface && NULL == mIp) {
         SOCKET_LOG_I("SocketBase::IF[%s]IP[any]Port[%d]", mInterface, mPort);
    }
    else {
        SOCKET_LOG_I("SocketBase::IF[not check]IP[any]Port[%d]", mPort);
    }
}

SocketBase::~SocketBase()
{

}

SocketBase* SocketBase::getInstance()
{
    return mInstance;
}

void SocketBase::registerCallBack(SocketReplier* mReplier)
{
    if (mReplier != NULL) {
        mSocketReplier = mReplier;
        SOCKET_LOG_I("SocketBase::registerCallBack");
    }
    else {
        SOCKET_LOG_E("mReplier is NULL... ");
    }
}

void SocketBase::init(E_SOCKET_TYPE type)
{
    pipe(mPipeFd); // init pipe for while exit
    while(mExitState == E_SOCKET_EXIT_FLAG_NONE) {
        // judy interface have up,if not, will wait for 2s cycle
        if ((mInterface != NULL) && (!isInterfaceUp(mInterface))) {
            sleep(1);
            continue;
        }
        else {
            if (NULL == mInterface) {
                SOCKET_LOG_I("mInterface is NULL, no need to check");
            }
            else {
                SOCKET_LOG_I("interface[%s] have up", mInterface);
            }

            mServerFd = socket(AF_INET, SOCK_STREAM, 0);
            if (mServerFd == -1) {
                SOCKET_LOG_E("create server socket failed... ");
                continue;
            }

            SOCKET_LOG_I("socket success, fd[%d] type [%d]", mServerFd, type);
            
            if (type == E_SOCKET_TYPE_SERVER) {
                initServerSocket(mServerFd, mIp, mPort);
            }
            else if (type == E_SOCKET_TYPE_CLIENT) {
                initClientSocket(mServerFd, mIp, mPort);
            }
            else {
                SOCKET_LOG_E("type[%d] is error", type);
                break;
            }

            if (mExitState == E_SOCKET_EXIT_FLAG_NONE) {
                if (mSocketReplier != NULL) {
                    mSocketReplier->connectStateInd(E_SOCKET_STATE_CONNECT);
                }
                recvData();
                SOCKET_LOG_I("no need to recv data, state = %d", mExitState);
            }
            else {
                SOCKET_LOG_I("no need to recv data, state = %d", mExitState);
            }
        }
        SOCKET_LOG_I("recv data finish, state = %d", mExitState);
    }
    SOCKET_LOG_I("socket communcation finish, state = %d", mExitState);
}

void SocketBase::deinit()
{
    size_t res = write((int)mPipeFd[1], "exit", strlen("exit"));
    SOCKET_LOG_I("SocketBase::deinit, send exit result=%d\n", res);
    close(mServerFd); /* close rild socket */
    mServerFd = -1;
}

void SocketBase::setSocketFd(int fd)
{
    SOCKET_LOG_I("SocketBase::setSocketFd, [%d]-->[%d]\n", mFd, fd);
    mFd = fd;
}

int SocketBase::getSocketFd()
{
    return mFd;
}

void SocketBase::initServerSocket(int fd, const char* ip, int port)
{
    while (mExitState == E_SOCKET_EXIT_FLAG_NONE) {

        int nReuse = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &nReuse, sizeof(nReuse)) < 0) {
            SOCKET_LOG_E("setsockopt failed....err[%s]", strerror(errno));
            continue;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(struct sockaddr_in));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port  = htons(port);
        if (NULL == ip) {
            serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
            SOCKET_LOG_I("socket server monitor ip is any");
        }
        else {
            serverAddr.sin_addr.s_addr = inet_addr(ip);
        }
        memset(serverAddr.sin_zero, 0, 8);

        int ret = bind(fd, (struct sockaddr*)(&serverAddr), sizeof(serverAddr));
        if (ret == -1) {
            SOCKET_LOG_E("bind failed... errno[%s]", strerror(errno));
            sleep(2);
            continue;
        }
        SOCKET_LOG_I("bind success ");

        if (listen(fd, 10) == -1) {
            SOCKET_LOG_E("listen failed... errno[%s]", strerror(errno));
            continue;
        }
        else {
            SOCKET_LOG_I("listen success ");
        }

        unsigned int addrlen = 0;
        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0, sizeof(struct sockaddr_in));
        addrlen = sizeof(struct sockaddr_in);
        int clientFd = accept(fd, (struct sockaddr*)(&clientAddr), &addrlen);
        if (clientFd == -1) {
            SOCKET_LOG_E("accept failed... errno[%s]", strerror(errno));
            sleep(2);
            continue;
        }
        else {
            setSocketFd(clientFd);
            SOCKET_LOG_I("accept success clientFd=[%d]", clientFd);
            break;
        }
    }
}

void SocketBase::initClientSocket(int fd, const char* ip, int port)
{
    while (mExitState == E_SOCKET_EXIT_FLAG_NONE) {
        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0, sizeof(struct sockaddr_in));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port  = htons(port);
        clientAddr.sin_addr.s_addr = inet_addr(ip);
        memset(clientAddr.sin_zero, 0, 8);
        if (connect(fd, (struct sockaddr*)&clientAddr, sizeof(struct sockaddr)) < 0) {
            // SOCKET_LOG_E("client cannot connect server...  errno[%s]", strerror(errno));
            sleep(2);
            continue;
        }

        setSocketFd(fd);
        SOCKET_LOG_I("connect success ");
        break;
    }
}

void SocketBase::recvData(void)
{
    uint32_t begin_seat = 0;
    uint32_t allLen = 0;
    struct pollfd rdfs[2] = { { 0, 0, 0 } };
    rdfs[0].fd = getSocketFd();
    rdfs[0].events = POLLIN;

    rdfs[1].fd = mPipeFd[0];
    rdfs[1].events = POLLIN;
    SOCKET_LOG_I("state = %d", mExitState);
    while (mExitState == E_SOCKET_EXIT_FLAG_NONE) {
        int resPoll = poll(rdfs, sizeof(rdfs) / sizeof(rdfs[0]), -1);
        if (resPoll < 0) {
            SOCKET_LOG_E("poll failed");
        }
        else  {
            if (rdfs[0].revents & POLLIN) {
                memset(gRecBuffer, 0, BUFFER_MAX_LEN);
                int ret = recv(getSocketFd(), gRecBuffer + begin_seat, BUFFER_MAX_LEN - begin_seat, 0);
                if (ret <= 0) {
                    close(getSocketFd());
                    setSocketFd(-1);
                    close(mServerFd);
                    SOCKET_LOG_E("client closed or rece data error...err[%s]", strerror(errno));

                    // notice ucomclient socket disconnect
                    if (mSocketReplier != NULL) {
                        mSocketReplier->connectStateInd(E_SOCKET_STATE_DISCONNECT);
                    }
                    break;

                    mExitState = E_SOCKET_EXIT_FLAG_ACCIDENT;
                }
                else {
                    // deal with data
                    SOCKET_LOG_I("read len: %d, begin: %d, thefirst data: %x, %x", ret, begin_seat, gRecBuffer[0], gRecBuffer[1]);
                    allLen = ret + begin_seat;
#if 1                        
                    // analyze data
                    if (mSocketReplier != NULL) {
                        mSocketReplier->recvDataInd(allLen, begin_seat, gRecBuffer);
                    }
#endif
                }
            }
            else  if (rdfs[1].revents & POLLIN) {
                char pipeBuffer[20] = { 0 };
                memset(pipeBuffer, 0, sizeof(pipeBuffer));
                read(rdfs[1].fd, pipeBuffer, sizeof(pipeBuffer));
                if (strcmp(pipeBuffer, "exit") == 0) {
                    close(getSocketFd());
                    mExitState = E_SOCKET_EXIT_FLAG_USER;
                    SOCKET_LOG_E("receive exit from pipe, will loop exit");                    
                }
                else  {
                    SOCKET_LOG_E("receive [%s] from pipe", pipeBuffer);
                }
            }
            else  {
                SOCKET_LOG_E("refs have event!!!");
            }
        }
    }
}

void SocketBase::sendDataReq(const uint8_t* data, uint32_t len)
{
    E_SOCKET_SEND_RES ret = E_SOCKET_SEND_RES_OK;
    if (getSocketFd() != -1) {
        // MSG_NOSIGNAL: avoid mClientFd invalid
        if (send(getSocketFd(), data, len, MSG_NOSIGNAL) == -1) {
            ret = E_SOCKET_SEND_RES_FAILED;
            SOCKET_LOG_E("send data error... ERR[%s]", strerror(errno));
        }
    }
    else {
        ret = E_SOCKET_SEND_RES_FAILED;
        SOCKET_LOG_E("client is not connect server... ");
    }

    if (mSocketReplier != NULL) {
        mSocketReplier->sendDataCfm(ret);
    }
}




bool SocketBase::isInterfaceUp(const char* interface)
{
    bool res = false;
    if (interface == NULL) {
        SOCKET_LOG_E("interface is NULL");
    }
    else {
        struct ifreq ifr;
        int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, interface);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)  {
            // SOCKET_LOG_E("SOCGIFGLAGS");
        }
        close(sock);
        if (!!(ifr.ifr_flags & IFF_UP)) {
            res = true;
        }
    }
    return res;
}

} // namespace
