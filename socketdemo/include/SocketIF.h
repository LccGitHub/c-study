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
#ifndef SOCKET_IF_H
#define SOCKET_IF_H

#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "SocketComDef.h"

namespace nutshell {


class SocketImp;
class SocketIF
{
public:
    /**
     * SocketIF(const char* interface, const char* ip, int port)
     *
     * SocketIF construct funciton
     *
     * @param [IN] interface  : if interface is NULL, will not check, else will monitor interface is up
     * @param [IN] ip  : if ip is NULL, will monitor all local interface, else only monitor ip
     * @param [IN] port  : monitor port
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */
    explicit SocketIF(const char* interface, const char* ip, int port);
    SocketIF() = delete;
    virtual ~SocketIF();


    /**
     * void start(E_SOCKET_TYPE type)
     *
     * initliaize socket and create thread to receive data
     *
     * @param [IN] type  : define in SocketComDef.h
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */
    void start(E_SOCKET_TYPE type);

    /**
     * void stop();
     *
     * deinitliaize socket, exit thread and stop receive data
     *
     * @param [IN] 
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */
    void stop();

     /**
     * sendDataReq(const uint8_t* data, uint32_t len);
     *
     * send data by socket
     *
     * @param [IN] data: send data pointer
     * @param [IN] len: length of send data
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */   
    void sendDataReq(const uint8_t* data, uint32_t len);

    /**
     * recvDataInd(const uint8_t* data, const uint32_t len)
     *
     * this api will be called when socket have receive data
     *
     * @param [IN] data: receive data pointer
     * @param [IN] len: length of receive data
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */
    virtual void recvDataInd(const uint8_t* data, const uint32_t len) = 0;

    /**
     * void connectStateInd(E_SOCKET_STATE result)
     *
     * this api will be called when have socket client have connected or have connect socket server
     *
     * @param [IN] result: define in SocketComDef.h
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */
    virtual void connectStateInd(E_SOCKET_STATE result) = 0;

    /**
     * void sendDataCfm(E_SOCKET_SEND_RES result)
     *
     * this api will be called when send data have finish
     *
     * @param [IN] result: define in SocketComDef.h
     *
     * @return void
     * @retval
     * @retval
     *
     * @attention Synchronous I/F.
     */    
    virtual void sendDataCfm(E_SOCKET_SEND_RES result) = 0;



private:

    SocketImp* mImp;


private:


};


} /* namespace */

#endif // SOCKET_IF_H
/* EOF */
