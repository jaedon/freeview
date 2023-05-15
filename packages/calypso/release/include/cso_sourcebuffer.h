/** 
 * @defgroup CSO SourceBuffer
 * @brief 
 */ 
 
/** 
 * @author Young-Ki,Kim(ykkim3@humaxdigital.com)
 * @brief 
 */ 
 
/** 
 * @note
 * Copyright (C) 2013 Humax Corporation. All Rights Reserved. 
 * This software is the confidential and proprietary information 
 * of Humax Corporation. You may not use or distribute this software 
 * except in compliance with the terms and conditions of any applicable license 
 * agreement in writing between Humax Corporation and you. 
 */ 
 
/*@{*/
 
/** 
 * @file cso.h
 */ 
 
#ifndef __CSO_SOURCEBUFFER_H__
#define __CSO_SOURCEBUFFER_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <stdint.h> /* temp : will be replaced to cso_def.h*/

#include "cso_def.h"

typedef void* CSO_SRCBUF;

typedef struct CSO_FRAME_DATA
{
	uint32_t stream_id;
	double pts;
	double dts;
	double duration;
	uint32_t data_size;
	void *data;
	uint32_t ext_size;	
	void *ext_data;
	bool_t b_secure_buffer;
} CSO_FRAME_DATA_t;

CSO_SRCBUF CSO_SRCBUF_Create(uint32_t sourceId, CSO_MEDIATYPE avtype, uint32_t codec, uint32_t size);
RETURN_CODE CSO_SRCBUF_Destroy(CSO_SRCBUF srcBuf);
void CSO_SRCBUF_Flush(CSO_SRCBUF srcBuf);
RETURN_CODE CSO_SRCBUF_EOS(CSO_SRCBUF srcBuf, bool_t bEos);
RETURN_CODE CSO_SRCBUF_WriteFrame(CSO_SRCBUF srcBuf, int32_t stream_id, double pts, double dts, double duration, uint32_t data_size, void *data, uint32_t ext_size,void *ext_data);
CSO_FRAME_DATA_t* CSO_SRCBUF_ReadFrame(CSO_SRCBUF srcBuf);
void CSO_SRCBUF_FreeFrame(CSO_FRAME_DATA_t *frame);
uint32_t CSO_SRCBUF_Depth(CSO_SRCBUF srcBuf);
uint32_t CSO_SRCBUF_Count(CSO_SRCBUF srcBuf);
void CSO_SRCBUF_SetReadLock(CSO_SRCBUF srcBuf,bool_t bLock);
bool_t CSO_SRCBUF_IsReadLocked(CSO_SRCBUF srcBuf);
bool_t CSO_SRCBUF_IsEmpty(CSO_SRCBUF srcBuf);
bool_t CSO_SRCBUF_IsEos(CSO_SRCBUF srcBuf);
RETURN_CODE CSO_SRCBUF_GetCurDts(CSO_SRCBUF srcBuf, uint64_t *pDts);
RETURN_CODE CSO_SRCBUF_GetCurPts(CSO_SRCBUF srcBuf, uint64_t *pPts);
RETURN_CODE CSO_SRCBUF_Duration(CSO_SRCBUF srcBuf, uint64_t *pDuration);
uint32_t CSO_SRCBUF_FlushPastByDts(CSO_SRCBUF srcBuf,uint64_t dts);
uint32_t CSO_SRCBUF_FlushPastByPts(CSO_SRCBUF srcBuf,uint64_t pts);
RETURN_CODE CSO_SRCBUF_SetCodec(CSO_SRCBUF srcBuf, uint32_t codec);

#ifdef  __cplusplus
}
#endif

#endif

