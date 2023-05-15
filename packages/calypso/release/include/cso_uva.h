/** 
 * @defgroup CSO Wrapper for UVA
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
 * @file cso_uva.h
 */ 
 
#ifndef __CSO_UVA_H__
#define __CSO_UVA_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <stdint.h> /* temp : will be replaced to cso_def.h*/

#include "cso_def.h"

typedef void* UVA_SRCHANDLE; 

CSO_ERR CSO_UVA_Init (CSO_Settings_t *settings);
CSO_ERR CSO_UVA_SetResourceMap (CSO_RESOURCE_MAP_t* pMap);
CSO_ERR CSO_UVA_Open (uint32_t *puSessionId);
CSO_ERR CSO_UVA_NETFLIX_Open (uint32_t *puSessionId);	/* For NETFLIX */
CSO_ERR CSO_UVA_AudioFlush(uint32_t ulSessionId);

CSO_ERR CSO_UVA_Probe (uint32_t ulSessionId);
CSO_ERR CSO_UVA_ProbeInfo(uint32_t ulSessionId, CSO_ProbeInfo_t *probeInfo);
CSO_ERR CSO_UVA_PlayInfo(uint32_t ulSessionId, CSO_PlayInfo_t *pInfo);
CSO_ERR CSO_UVA_Start (uint32_t ulSessionId, uint32_t ulDeviceId);
CSO_ERR CSO_UVA_Stop (uint32_t ulSessionId);
CSO_ERR CSO_UVA_Close (uint32_t ulSessionId);
CSO_ERR CSO_UVA_Control (uint32_t ulSessionId, CSO_CTRL_e eCtrl, CSO_CTRL_PARAM_t *pParam);
CSO_ERR CSO_UVA_ResigterEventCallback(uint32_t ulSessionId, void *function);
CSO_ERR CSO_UVA_AddSource(uint32_t ulSessionId, uint32_t unSourceId, CSO_MEDIATYPE avtype, uint32_t codec, UVA_SRCHANDLE *srcBuf);
CSO_ERR CSO_UVA_RemoveSource(uint32_t ulSessionId, UVA_SRCHANDLE srcBuf);
CSO_ERR CSO_UVA_SetSettings(uint32_t ulSessionId,CSO_Settings_t *settings);
CSO_ERR CSO_UVA_GetSettings(uint32_t ulSessionId,CSO_Settings_t *settings);

void *CSO_UVA_Malloc(uint32_t ulBytes);
void CSO_UVA_Free(void *pMemory);
void *CSO_UVA_SoC_Malloc(uint32_t ulSessionId, uint32_t ulBytes, CSO_SOC_MEMORY_TYPE_e eMemType);
void CSO_UVA_SoC_Free(uint32_t ulSessionId, void *pMemory, CSO_SOC_MEMORY_TYPE_e eMemType);
CSO_ERR CSO_UVA_SoC_Memcpy(uint32_t ulSessionId, void *pDest, CSO_SOC_MEMORY_TYPE_e eDestType, void *pSrc, CSO_SOC_MEMORY_TYPE_e eSrcType, size_t nSize);

CSO_ERR CSO_UVA_DoDecrypt(uint8_t *pucInputBuf, uint8_t *pucOutputBuf, uint32_t ulSize, uint8_t *pucKey, uint8_t *pucIv, CSO_DECRYPTOR_Type_e eType);
CSO_ERR CSO_UVA_GetSecureClock(void *pAppContext);

CSO_ERR CSO_UVA_SRCBUF_SecureWrite(UVA_SRCHANDLE srcBuf, double pts, double dts, double duration, uint32_t data_size, void *data, uint32_t ext_size,void *ext_data);
CSO_ERR CSO_UVA_SRCBUF_Write(UVA_SRCHANDLE srcBuf, double pts, double dts, double duration, uint32_t data_size, void *data, uint32_t ext_size,void *ext_data);
CSO_ERR CSO_UVA_SRCBUF_EOS(UVA_SRCHANDLE srcBuf);
CSO_ERR CSO_UVA_SRCBUF_AudioFlush(UVA_SRCHANDLE srcBuf);
CSO_ERR CSO_UVA_SRCBUF_SetCodec(UVA_SRCHANDLE srcBuf, uint32_t unCodec);

#ifdef  __cplusplus
}
#endif

#endif /* __CSO_UVA_H__ */

