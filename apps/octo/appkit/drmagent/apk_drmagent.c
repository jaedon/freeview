/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_drmagent.c
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <_apk_int.h>

STATIC APK_DRMAGENT_MessageResultNotifier_t	s_pfMessageResultAppNotifier;
STATIC APK_DRMAGENT_StatusChangeNotifier_t	s_pfDrmSystemStatusChangedAppNotifier;




STATIC void	apk_drmagent_freeOnDataReceived(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

//	HxLOG_Assert(nArgc == 4);

	// apk_drmagent_onMessageResult(), apk_drmagent_onDrmSystemStatusChanged() 둘다 사용하므로 주의.
	pvData = apArgV[1];
	if (pvData != NULL)
	{
		APK_Free(pvData);
	}
}


STATIC void apk_drmagent_onMessageResult(const HCHAR *pzResultMsg, HINT32 nMsgId, HUINT32 ulResultCode)
{
	if(s_pfMessageResultAppNotifier != NULL)
	{
		HCHAR *	pcDupData = NULL;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		if(pzResultMsg != NULL)
		{
			pcDupData = APK_StrDup(pzResultMsg);
		}
		else
		{
			HxLOG_Warning("pzResultMsg is NULL!! Check it!!\n");
			pcDupData = NULL;
		}

		stApkEvtArgs.pfnFreeFunc = apk_drmagent_freeOnDataReceived;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)s_pfMessageResultAppNotifier;
		stApkEvtArgs.apArgV[1] = (void *)pcDupData;
		stApkEvtArgs.apArgV[2] = (void *)nMsgId;
		stApkEvtArgs.apArgV[3] = (void *)ulResultCode;

		APK_EVENT_Emit(&stApkEvtArgs);
 	}
}

STATIC void apk_drmagent_onDrmSystemStatusChanged(const HCHAR *pzDrmSystemId)
{
	if(s_pfDrmSystemStatusChangedAppNotifier != NULL)
	{
		HCHAR *	pcDupData = NULL;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		if(pzDrmSystemId != NULL)
		{
			pcDupData = APK_StrDup(pzDrmSystemId);
		}
		else
		{
			HxLOG_Warning("pzDrmSystemId is NULL!! Check it!!\n");
			pcDupData = NULL;
		}

		stApkEvtArgs.pfnFreeFunc = apk_drmagent_freeOnDataReceived;
		stApkEvtArgs.ulArgCount = 2;
		stApkEvtArgs.apArgV[0] = (void *)s_pfDrmSystemStatusChangedAppNotifier;
		stApkEvtArgs.apArgV[1] = (void *)pcDupData;

		APK_EVENT_Emit(&stApkEvtArgs);
 	}
}



#define ___PUBLIC_FUNCTIONS_______________________________________________________________________________


void	APK_DRMAGENT_Init(void)
{
	HxLOG_Trace();
	OAPI_DRMAGENT_Init(apk_drmagent_onMessageResult, apk_drmagent_onDrmSystemStatusChanged);
}

void	APK_DRMAGENT_SendDrmMessage(const HCHAR *pzMsgType, const HCHAR *pzMsg, const HCHAR *pzDrmSystemId, HUINT16 usMsgId)
{
	HxLOG_Trace();
	OAPI_DRMAGENT_SendDrmMessage(pzMsgType, pzMsg, pzDrmSystemId, usMsgId);
}

void	APK_DRMAGENT_SendDrmSystemStatusRequest(const HCHAR *pzDrmSystemId)
{
	HxLOG_Trace();
	OAPI_DRMAGENT_SendDrmSystemStatusRequest(pzDrmSystemId);
}



HERROR	APK_DRMAGENT_RegisterDrmMessageNotifier(APK_DRMAGENT_MessageResultNotifier_t fnNotifier)
{
	HxLOG_Trace();

	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_pfMessageResultAppNotifier = fnNotifier;
	}

	return ERR_OK;

}

HERROR	APK_DRMAGENT_RegisterDrmStatusNotifier(APK_DRMAGENT_StatusChangeNotifier_t fnNotifier)
{
	HxLOG_Trace();

	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_pfDrmSystemStatusChangedAppNotifier = fnNotifier;
	}

	return ERR_OK;

}


/* end of file */
