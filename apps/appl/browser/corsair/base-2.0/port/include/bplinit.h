/**************************************************************************************/
/**
 * @file bplinit.h
 *
 * Initialization
 *
 * @author  ipm(@humaxdigital.com)
 * @date    2012/09/24
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __BPL_INIT_H__
#define __BPL_INIT_H__

#include "bpltype.h"

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus BPL_Init(void);

BPLStatus BPL_GetHumaxUIString(char *buf, int bufsize);

#ifdef __cplusplus
}
#endif

#endif
