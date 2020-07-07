/**
 * Copyright @ 2019 iAuto (Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * iAuto (Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
/**
 * @file uComDefine.h                                            
 * @brief Declaration file of uCom module.                  
 *
 * This file includes the declaration of uCom module, and   
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */
#ifndef SOCKET_DEFINE_H
#define SOCKET_DEFINE_H

#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <stdint.h>

namespace nutshell
{



enum E_SOCKET_TYPE
{
    E_SOCKET_TYPE_SERVER = 0, // socket is server
    E_SOCKET_TYPE_CLIENT = 1  // socket is client
};


// socket connect state
enum E_SOCKET_STATE
{
    E_SOCKET_STATE_CONNECT = 0x0000,          //<  socket have connected
    E_SOCKET_STATE_DISCONNECT = 0x0001,       //<  socket have disconnected
};

// send result
enum E_SOCKET_SEND_RES
{
    E_SOCKET_SEND_RES_OK = 0x0000,            //<  socket send success
    E_SOCKET_SEND_RES_FAILED = 0x0001,        //<  socket send failed
};

} /* nutshell */

#endif // SOCKET_DEFINE_H
/* EOF */
