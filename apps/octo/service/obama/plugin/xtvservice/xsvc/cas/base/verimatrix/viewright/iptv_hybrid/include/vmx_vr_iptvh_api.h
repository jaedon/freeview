/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_api.h
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2014 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __INT_VMXIPTVHAPI_H__
#define __INT_VMXIPTVHAPI_H__

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#include "htype.h"
#include "VMClient.h"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
typedef enum
{
	eSvcXxx_Type
} xsvcXxx_Type_e;

typedef struct
{
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR vmx_vr_iptvh_api_StartService(void* pVmxContext, HUINT8 ucSvcIdx, HUINT8 ucPidCount, HUINT16 *pusStreamPid);
HERROR vmx_vr_iptvh_api_GetCaSystemId(HUINT16* pusCaSystemId);
HERROR vmx_vr_iptvh_api_CreateContext(HINT8 *pucIniFileName, HBOOL bUseIniFile, void** pVmxContext);
HERROR vmx_vr_iptvh_api_RegisterControlWordCallback( void *pVmxContext , tControlWordCallback pControlWordCallback , void *pUserParam , int nAlgorithmCount , int *nAlgorithmList);
HERROR vmx_vr_iptvh_api_RegisterCopyControlCb(void* pVmxContext, tCopyControlCallback fnCopyCtrlCb, void *pUserParam);
HERROR vmx_vr_iptvh_api_RegisterUserDataCb(void* pVmxContext, tUserDataCallback fnUserDataCb, void *pUserParam);
HERROR vmx_vr_iptvh_api_RegisterOsdMessageCb(tOsdMessageCallback fnOsdMsgCb, void *pUserParam);
HERROR vmx_vr_iptvh_api_RegisterFingerPrintingCb(void* pVmxContext, tFingerPrintingCallback fnFingerPrintingCb, void *pUserParam);
HERROR vmx_vr_iptvh_api_RegisterIrdOpaqueDataCb(tIrdOpaqueDataCallback fnIrdOpaqueDataCb, void *pUserParam);
HERROR vmx_vr_iptvh_api_Client_SetEcm(void* pVmxContext, HINT32 nIsSection, HUINT16 usEcmPid, HUINT8* pucBuffer, HUINT16 usBufferLength);
HERROR vmx_vr_iptvh_api_RegisterDecodeFailureCb(void* pVmxContext, tDecodeFailureCallback fnDecodeFailureCb, void *pUserParam);
HERROR vmx_vr_iptvh_api_GetRatingLevel(void* pVmxContext, HUINT8 *pucOverride, HUINT8 *pucChannelRatingLevel, HUINT8 *pucCurrentStbRatingLevel);
HERROR vmx_vr_iptvh_api_SetEmm(HUINT8** ppucTSPacketBuffer, HUINT32* pulNumTSPackets);
HERROR vmx_vr_iptvh_api_ResetStream(void* pVmxContext);
HERROR vmx_vr_iptvh_api_StopService(void* pVmxContext, HUINT16 usSvcIdx);
HERROR vmx_vr_iptvh_api_SetConfigOffline(HUINT8* szStoreFilePath);
HERROR vmx_vr_iptvh_api_SetConfig(HUINT8* szStoreFilePath, HUINT8* szIpAddress, HUINT16 usPort);
HERROR vmx_vr_iptvh_api_SetVcasCommProtocol(eVcasVersion_t eVersion);
HERROR vmx_vr_iptvh_api_OverrideRatingLevel(void* pVmxContext, HUINT8 *pucCurrentStbRatingLevel);
HERROR vmx_vr_iptvh_api_SetRatingLevel(void* pVmxContext, HUINT8 ucRating);
HERROR vmx_vr_iptvh_api_SetErrorLevel(void* pVmxContext, HINT32 nErrLevel);


const HCHAR* vmx_vr_iptvh_api_GetVersionString(void);

// HERROR xsvc_xxx_yyy_DoSomething( ... );

#endif		//#ifndef __INT_VMXIPTVHAPI_H__

