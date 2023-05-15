/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_verimatrix.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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
 * (c) 2011-2013 Humax Co., Ltd.
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

#ifndef __OAPI_CAS_VERIMATRIX_H__
#define __OAPI_CAS_VERIMATRIX_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_CAS_VERIMATRIX_Init							"oapi.cas.verimatrix.Init"
//#define RPC_OAPI_CAS_IRDETO_SetVirtualSecureCwMode		"oapi.cas.irdeto.SetVirtualSecureCwMode"


#define RPC_OAPI_CAS_VERIMATRIX_RequestWebVersion				"obama.cas.verimatrix.RequestWebVersion"

#define RPC_OBAMA_CAS_VERIMATRIX_onStatusChanged			"obama.cas.verimatrix.onStatusChanged"
#define RPC_OBAMA_CAS_VERIMATRIX_onWebVersionChanged			"obama.cas.verimatrix.onWebVersionChanged"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


#define 	OAPI_CAS_VERIMATRIX_VERSION_LENGTH			(256)

//#define	OAPI_CAS_IRDETO_CLIENT_VERSION_LENG		256
//#define	OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG		16
typedef struct
{
	HCHAR						szIptvHybridVersion[OAPI_CAS_VERIMATRIX_VERSION_LENGTH];
	HCHAR						szWebVersion[OAPI_CAS_VERIMATRIX_VERSION_LENGTH];
	HCHAR						szGatewayVersion[OAPI_CAS_VERIMATRIX_VERSION_LENGTH];
#if 0
	HBOOL						bVirtualSecureCW;

	HCHAR						szClientVer[OAPI_CAS_IRDETO_CLIENT_VERSION_LENG];

	HBOOL						bCWE;
	HBOOL						bJTAG;
	HBOOL						bSBOOT;
	HBOOL						bOTP;

	HUINT8						szMancode[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG];
	HUINT8						szHWCode[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG];
	HUINT8						szVariant[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG];

	HUINT8						szSysId[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG];
	HUINT8						szKeyVer[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG];
	HUINT8						szSigVer[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG];

	HUINT8						ucControlByte;

	HCHAR						szPrivateData[OAPI_CAS_IRDETO_CLIENT_VERSION_LENG];
#endif
} OxCasVerimatrixStatus_t;

typedef void (*OxCasVerimatrix_Notifier_t)(void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR 		OAPI_CAS_VERIMATRIX_Init(OxCasVerimatrix_Notifier_t pfListener);
HERROR 		OAPI_CAS_VERIMATRIX_GetStatus(OxCasVerimatrixStatus_t *pstStatus);


#endif // __OAPI_CAS_VERIMATRIX_H__
