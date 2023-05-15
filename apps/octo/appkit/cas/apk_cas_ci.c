/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_cas_ci.c
	@brief

	Description:  									\n
	Module: cas ci			\n

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

#include <hlib.h>
#include <dlib_cas.h>
#include <oapi_cas_ci.h>

#include <_apk_int.h>
#include <apk_event.h>
#include <apk_cas_ci.h>

#define ___INTERNAL_STRUCTURES___



#define ___INTERNAL_VARIABLES___

STATIC	HxList_t		*apkCasCi_ObjectHandlerList_t = NULL;

STATIC	HxList_t		*apkCasCi_AppMmiObjectHandlerList_t = NULL;





#define ___PRIVATE_FUNCTION___


#if 0
STATIC	void	apk_cas_ci_SetBoxSetting(APKS_CI_CIInfo_t *pstInfo)
{
	HCHAR	*encoded_data;
	encoded_data = APKI_OBJDEC_EncodeObject(pstInfo, OBJECT_PROPERTY, _sqc_ci_info_data_table);

	APKI_SQC_Call(SQC_CI, OBJECT_OPERATION_SET, encoded_data);
	APK_Free(encoded_data);
}
#endif

#define _____NOTIFIER_FUNCTION_____

STATIC void	apk_cas_ci_freeOnStatusChanged(HINT32 nArgc, void *apArgV[])
{
	HCHAR	*pszStr;

	HxLOG_Assert(nArgc == 4);

	pszStr = (HCHAR*)apArgV[3];
	if (pszStr != NULL)
	{
		APK_Free(pszStr);
	}
}

STATIC void apk_cas_ci_onStatusChanged(void)
{
	HxList_t	*list;
	APK_CAS_CI_Notifier_t 	fnNotifier;
	HUINT32		i = 0;
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);

	if(hErr == ERR_OK)
	{
		list = apkCasCi_ObjectHandlerList_t;
		while (list)
		{
			fnNotifier = (APK_CAS_CI_Notifier_t)HLIB_LIST_Data(list);
			if(stStatus.astCamInfoList != NULL)
			{
				if (fnNotifier)
				{
					for(i = 0;i < stStatus.ulSlotCount;i+=1)
					{
						if(&(stStatus.astCamInfoList[i]) != (APKS_CI_CAMInfo_t *)NULL)
						{
							HCHAR *	pcDupData = NULL;
							if(stStatus.astCamInfoList[i].szCamName != NULL && stStatus.astCamInfoList[i].szCamName[0] != 0x00)
							{
								APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

								pcDupData = APK_StrDup(stStatus.astCamInfoList[i].szCamName);

								stApkEvtArgs.pfnFreeFunc = apk_cas_ci_freeOnStatusChanged;
								stApkEvtArgs.ulArgCount = 4;
								stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
								stApkEvtArgs.apArgV[1] = (void *)stStatus.astCamInfoList[i].ulSlotNumber;
								stApkEvtArgs.apArgV[2] = (void *)stStatus.astCamInfoList[i].eStatus;
								stApkEvtArgs.apArgV[3] = (void *)pcDupData;

								APK_EVENT_Emit(&stApkEvtArgs);
							}
							else
							{
								HxLOG_Critical("ERROR!! Check szCamName!!\n");
							}
						}
					}
				}
			}
			list = list->next;
		}
	}

}


// CI plus related
STATIC void	apk_cas_ci_freeUserData(HINT32 nArgc, void *apArgV[])
{
	void	*pvData = NULL;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];
	if (pvData != NULL)
	{
		APK_Free(pvData);
	}
}

STATIC void apk_cas_ci_onAppMmiEvent(CI_AMMI_MSG_TYPE enMsgType, void* pvUserData, HUINT32 ulUserDataSize)
{
	HxList_t	*list;
	APK_CAS_CI_AppMmiNotifier_t 	fnNotifier;


#if 0//def CONFIG_DEBUG
	{
		HxObject_t		*pstObject;
		CI_AMMI_CONTENT 	stContent;

		HxLOG_Debug("appkit...debug print AppMMI Event... \n");
		pstObject = HLIB_SERIALIZER_Unpack(pvUserData, ulUserDataSize);
		if (pstObject == NULL)
		{
			HxLOG_Error("cannot unpack the data \n");
			return ;
		}
		HxLOG_Debug("enMsgType: %d \n", enMsgType);
		switch(enMsgType)
		{
			case CI_AMMI_MSG_REQ_START:
				if (HxOBJECT_ARR_LEN(pstObject) != 3)
				{
					HxLOG_Error("invalid object arr len. \n");
					return ;
				}
				stContent.typeReqStart.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
				stContent.typeReqStart.ucAppDomainIdLen = HxOBJECT_ARR_VAL(pstObject, 1)->u.bin.size;
				stContent.typeReqStart.pucAppDomainId = HxOBJECT_ARR_VAL(pstObject, 1)->u.bin.data;
				stContent.typeReqStart.ucInitialObjectLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size;
				stContent.typeReqStart.pucInitialObject = HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeReqStart.usSessionId);

				break;

			case CI_AMMI_MSG_FILE_ACK:
				if (HxOBJECT_ARR_LEN(pstObject) != 4)
				{
					HxLOG_Error("invalid object arr len. \n");
				}
				stContent.typeFileAck.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
				stContent.typeFileAck.ucFileOK = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
				stContent.typeFileAck.enReqType = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
				stContent.typeFileAck.ulBytesLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size;
				stContent.typeFileAck.pucBytes = HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeFileAck.usSessionId);

				break;

			case CI_AMMI_MSG_REQ_APP_ABORT:
				if (HxOBJECT_ARR_LEN(pstObject) != 2)
				{
					HxLOG_Error("invalid object arr len. \n");
				}
				stContent.typeReqAppAbort.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
				stContent.typeReqAppAbort.enAbortReqCode = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeReqAppAbort.usSessionId);

				break;

			case CI_AMMI_MSG_APP_ABORT_ACK:
				if (HxOBJECT_ARR_LEN(pstObject) != 1)
				{
					HxLOG_Error("invalid object arr len. \n");
				}
				stContent.typeAppAbortAck.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeAppAbortAck.usSessionId);

				break;

			default:
				HxLOG_Error("ERROR : invalid message type (%d) \n", enMsgType);

		}
	}
#endif


	list = apkCasCi_AppMmiObjectHandlerList_t;
	while (list)
	{
		fnNotifier = (APK_CAS_CI_AppMmiNotifier_t)HLIB_LIST_Data(list);

		if (fnNotifier)
		{
			void * pvDupData = NULL;
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			if(pvUserData != NULL)
			{
				pvDupData = APK_MemDup(pvUserData, ulUserDataSize);
			}
			else
			{
				HxLOG_Warning("pvUserData is NULL!! Check it!!\n");
				pvDupData = NULL;
			}


			stApkEvtArgs.pfnFreeFunc = apk_cas_ci_freeUserData;
			stApkEvtArgs.ulArgCount = 4;
			stApkEvtArgs.apArgV[0] = (void *)fnNotifier;
			stApkEvtArgs.apArgV[1] = (void *)enMsgType;
			stApkEvtArgs.apArgV[2] = (void *)pvDupData;
			stApkEvtArgs.apArgV[3] = (void *)ulUserDataSize;

			APK_EVENT_Emit(&stApkEvtArgs);
		}

		list = list->next;
	}


}

#define ___API_FUNCTION___

HERROR	APK_CAS_CI_Init(void)
{

	OAPI_CAS_CI_Init(apk_cas_ci_onStatusChanged, apk_cas_ci_onAppMmiEvent);

	return ERR_OK;
}

void	APK_CAS_CI_RegisterNotifier(APK_CAS_CI_Notifier_t fnNotifier)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(apkCasCi_ObjectHandlerList_t, fnNotifier);
	if (list)
	{
		HxLOG_Error("fnNotifier(%X) is already registered!!!\n", fnNotifier);
		return;
	}

	apkCasCi_ObjectHandlerList_t = HLIB_LIST_Append(apkCasCi_ObjectHandlerList_t, (void*)fnNotifier);

}

void	APK_CAS_CI_UnRegisterNotifier(APK_CAS_CI_Notifier_t fnNotifier)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(apkCasCi_ObjectHandlerList_t, fnNotifier);
	if (list == NULL)
	{
		HxLOG_Error("fnNotifier(%X) does not registered yet..\n", fnNotifier);
		return;
	}

	apkCasCi_ObjectHandlerList_t = HLIB_LIST_Remove(apkCasCi_ObjectHandlerList_t, fnNotifier);

}

HERROR		APK_CAS_CI_IsSupportedCI(HBOOL *pbSupported)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbSupported = stStatus.bSupportCI;
	}
	else
	{
		*pbSupported = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_CI_IsSupportedCIPlus(HBOOL *pbSupported)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbSupported = stStatus.bSupportCIPlus;
	}
	else
	{
		*pbSupported = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_CI_GetCIPlusVersionName(HCHAR *pszVesionName)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszVesionName, stStatus.szCIPlusVer, APKS_CI_PLUS_VER_LENG);
	}
	else
	{
		HxSTD_MemSet(pszVesionName, 0x00, APKS_CI_PLUS_VER_LENG);
	}

	return hErr;
}

HERROR		APK_CAS_CI_GetSlotCount(HUINT32 *pulSlotCount)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pulSlotCount = stStatus.ulSlotCount;
	}
	else
	{
		*pulSlotCount = 0;
	}

	return hErr;
}

HERROR		APK_CAS_CI_GetCamNumberBySlot(HUINT32 ulSlot, HUINT32 *pulCamNumber)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pulCamNumber = stStatus.astCamInfoList[ulSlot].ulSlotNumber;
	}
	else
	{
		*pulCamNumber = 0;
	}

	return hErr;

}

HERROR		APK_CAS_CI_GetCamStatusByIndex(HUINT32 ulSlot, DxCASCI_CamStatus_e *peCamStatus)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*peCamStatus = stStatus.astCamInfoList[ulSlot].eStatus;
	}
	else
	{
		*peCamStatus = eDxCASCI_CAM_STATUS_Extracted;
	}

	return hErr;
}

HERROR		APK_CAS_CI_GetCamNameByIndex(HUINT32 ulSlot, HCHAR *pszCamName)
{
	HERROR					hErr;
	APKS_CI_CIInfo_t			stStatus;

	hErr = OAPI_CAS_CI_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszCamName, stStatus.astCamInfoList[ulSlot].szCamName, APKS_CAM_NAME_LEN);
	}
	else
	{
		HxSTD_MemSet(pszCamName, 0x00, APKS_CAM_NAME_LEN);
	}

	return hErr;

}


// CI Plus related.

void APK_CAS_CI_RegisterAppMmiCallback (APK_CAS_CI_AppMmiNotifier_t pfnCallback)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(apkCasCi_AppMmiObjectHandlerList_t, pfnCallback);
	if (list)
	{
		HxLOG_Error("fnNotifier(%X) is already registered!!!\n", pfnCallback);
		return;
	}

	apkCasCi_AppMmiObjectHandlerList_t = HLIB_LIST_Append(apkCasCi_AppMmiObjectHandlerList_t, (void*)pfnCallback);


}

void	APK_CAS_CI_UnRegisterAppMmiCallback(APK_CAS_CI_AppMmiNotifier_t pfnCallback)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(apkCasCi_AppMmiObjectHandlerList_t, pfnCallback);
	if (list == NULL)
	{
		HxLOG_Error("fnNotifier(%X) does not registered yet..\n", pfnCallback);
		return;
	}

	apkCasCi_AppMmiObjectHandlerList_t = HLIB_LIST_Remove(apkCasCi_AppMmiObjectHandlerList_t, pfnCallback);

}


HERROR APK_CAS_CI_RequestFileorData (HUINT16 usSessionId, CI_AMMI_REQUEST_TYPE enReqType, HUINT8 *pucReqName, HULONG ulReqNameLen)
{
	return OAPI_CAS_CI_RequestFileorData (usSessionId, enReqType, pucReqName, ulReqNameLen);
}

HERROR APK_CAS_CI_RequestAppAbort (HUINT16 usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode)
{
	return OAPI_CAS_CI_RequestAppAbort(usSessionId, enAbortReqCode);
}
// EOF
