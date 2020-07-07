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
 * @file uComLog.h
 * @brief Declaration file of class uComLog.
 *
 * This file includes the declaration of class uComLog, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */
#ifndef SOCKET_LOG_H
#define SOCKET_LOG_H

#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "LogBase.h"

namespace nutshell
{
#ifdef SOCKET_LOG_TAG
#undef SOCKET_LOG_TAG
#endif

#define SOCKET_LOG_TAG "socket"

#define SOCKET_LOG_E(...) UComLog(SOCKET_LOG_TAG, LOG_BASEE, __func__, __LINE__, __VA_ARGS__)
#define SOCKET_LOG_W(...) UComLog(SOCKET_LOG_TAG, LOG_BASEW, __func__, __LINE__, __VA_ARGS__)
#define SOCKET_LOG_I(...) UComLog(SOCKET_LOG_TAG, LOG_BASEI, __func__, __LINE__, __VA_ARGS__)
#define SOCKET_LOG_D(...) UComLog(SOCKET_LOG_TAG, LOG_BASED, __func__, __LINE__, __VA_ARGS__)
#define SOCKET_LOG_V(...) UComLog(SOCKET_LOG_TAG, LOG_BASEV, __func__, __LINE__, __VA_ARGS__)



} /* nutshell */

#endif // THREAD_LOG_H
/* EOF */
