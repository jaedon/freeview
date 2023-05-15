/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_cas_mmi.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

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

#include <oapi.h>

#include <apk.h>


STATIC void apk_cas_mmi_onNotifyoccured(OxCasMmi_Notifier_t pfnNotiFunc, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if(pfnNotiFunc != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)pfnNotiFunc;
		stApkEvtArgs.apArgV[1] = (void *)ulParam1;
		stApkEvtArgs.apArgV[2] = (void *)ulParam2;
		stApkEvtArgs.apArgV[3] = (void *)ulParam3;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}


#define ___API_FUNCTION___


void	APK_CAS_MMI_Init(void)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_Init(apk_cas_mmi_onNotifyoccured);
}


void	APK_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_RemoveEvtInfo(pstEvent);
}

void	APK_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_GetEvtInfo(ppstEvent);
}

void	APK_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_RemoveUiEvent(eSessionType, pstEvent);
}


void	APK_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_GetUiEvent(eSession, hSession, ppstEvent);
}

void	APK_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, const HCHAR *pzInputData)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_SendUiResponse(eModule, nSlot, eSession, hSession, pzInputData);
}

void	APK_CAS_MMI_CloseSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_CloseSession(eModule, nSlot, eSession, hSession);
}

Handle_t	APK_CAS_MMI_OpenSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, const char *pcParamData)
{
	HxLOG_Trace();

	return OAPI_CAS_MMI_OpenSession(eModule, nSlot, eSession, pcParamData);
}
void	APK_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_UnRegisterSessionNotifier(eSession);
}

void	APK_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, APK_CAS_MMI_Notifier_t fnNotifier)
{
	HxLOG_Trace();
	OAPI_CAS_MMI_RegisterSessionNotifier(eSession, (OxCasMmi_Notifier_t)fnNotifier);
}

void	APK_CAS_MMI_UnRegisterEventNotifier(HUINT32 objId)
{
	// not used.
	HxLOG_Trace();
	OAPI_CAS_MMI_UnRegisterEventNotifier(objId);
}


/* end of file */
