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
 * @file UcomLog.h
 * @brief Declaration file of class UcomLog.
 *
 * This file includes the declaration of class uComLog, and
 * the definitions of the macros, struct, enum and so on.
 *
 * @attention used for C++ only.
 */
#ifndef LOG_BASE_H
#define LOG_BASE_H

#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif



namespace nutshell
{
enum UComLogTag {
    LOG_BASEV,
    LOG_BASED,
    LOG_BASEI,
    LOG_BASEW,
    LOG_BASEE,
};

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_BASEV
#else
#endif

void UComLog(const char* log_tag, UComLogTag tag, const char* func, int line, const char* format, ...);

void slog2Init();

} /* nutshell */

#endif // LOG_BASE_H
/* EOF */
