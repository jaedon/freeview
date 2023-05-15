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

/******************************************************************************/
/**
 * @file	  		nx_core_cas_ir.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_CAS_H__
#define __NX_CORE_CAS_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>
#include <nx_core_cas.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum
{
	eNxCasIrUCHandleKey_None = 0,
	eNxCasIrUCHandleKey_Banner,
	eNxCasIrUCHandleKey_Extension

} NX_CasIrHandleKey_t;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

HERROR	NX_CAS_IRDETO_Init(void);
HERROR	NX_CAS_IRDETO_RegisterNotifier(APK_CAS_IRDETO_Notifier_t fnNotifier);
HERROR	NX_CAS_IRDETO_UnregisterNotifier(void);
HERROR	NX_CAS_IRDETO_SetVirtualSecureCwMode(HBOOL bVirtualSecureCW);
HERROR	NX_CAS_IRDETO_GetVirturlSecureCwMode(HBOOL *pbVirtualSecureCwMode);
HERROR	NX_CAS_IRDETO_GetIrdetoClientVersionName(HUINT8 *pszVesionName);
HERROR	NX_CAS_IRDETO_IsCWE(HBOOL *pbCWE);
HERROR	NX_CAS_IRDETO_IsJTAG(HBOOL *pbJTAG);
HERROR	NX_CAS_IRDETO_IsSBOOT(HBOOL *pbSBOOT);
HERROR	NX_CAS_IRDETO_IsOTP(HBOOL *pbOTP);
HERROR	NX_CAS_IRDETO_GetIrdetoManCode(HUINT8 *pszMancode);
HERROR	NX_CAS_IRDETO_GetIrdetoHwCode(HUINT8 *pszHWCode);
HERROR	NX_CAS_IRDETO_GetIrdetoVariant(HUINT8 *pszVariant);
HERROR	NX_CAS_IRDETO_GetIrdetoSysId(HUINT8 *pszSysId);
HERROR	NX_CAS_IRDETO_GetIrdetoKeyVer(HUINT8 *pszKeyVer);
HERROR	NX_CAS_IRDETO_GetIrdetoSigVer(HUINT8 *pszSigVer);
HERROR	NX_CAS_IRDETO_GetControlByte(HUINT8 *pControlByte);
HERROR	NX_CAS_IRDETO_GetPrivateData(HUINT8 *pPrivateData);
HERROR	NX_CAS_IRDETO_GetFusingInfo(HBOOL *bIsSCS, HBOOL *bIsJTAG, HBOOL *bIsOTP, HBOOL *bIsCWE);

#endif // __NX_CORE_CAS_H__
