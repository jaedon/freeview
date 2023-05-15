/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_exe.c
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
#include <vkernel.h>
#include <oapi.h>
#include <apk.h>
#include <oapi_exe.h>
#include <apk_exe.h>
#include <hapi.h>
#include <dapi.h>
#include <papi.h>


#define ___PRIVATE_FUNCTION___
#define OxEXE_PROCNAME 30

typedef void	(* apkExe_Notifier_t) (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata);


static HERROR apk_exe_startProc(const HCHAR * szProcName, const HCHAR *szSignature, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	HxLOG_Trace();
	HxLOG_Print("[%s] proc[%s] p1[%d], p2[%d], p3[%d]\n", __FUNCTION__,szProcName, param1, param2, param3);
	OAPI_EXE_Start(szProcName, szSignature, param1, param2, param3);
	return ERR_OK;
}

static HERROR apk_exe_setProc(const HCHAR * szProcName, const HCHAR *szSignature, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	HxLOG_Trace();
	HxLOG_Print("[%s] proc[%s] p1[%d], p2[%d], p3[%d]\n", __FUNCTION__,szProcName, param1, param2, param3);
	OAPI_EXE_Set(szProcName, szSignature, param1, param2, param3);
	return ERR_OK;
}

static HERROR apk_exe_setProcByBin(const HCHAR * szProcName, void* pvBin, HUINT32 ulSize)
{
	HxLOG_Trace();
	HxLOG_Print("[%s] proc[%s] bin[%x] BinSize[%d]\n", __FUNCTION__,szProcName, pvBin, ulSize);
	OAPI_EXE_SetByBin(szProcName, pvBin, ulSize);
	return ERR_OK;
}


static HERROR apk_exe_stopProc(const HCHAR *szProcName, const HCHAR *szSignature, HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	HxLOG_Trace();
	HxLOG_Print("[%s] proc[%s] p1[%d], p2[%d], p3[%d]\n", __FUNCTION__,szProcName, param1, param2, param3);
	OAPI_EXE_Stop(szProcName, szSignature, param1, param2, param3);
	return ERR_OK;
}


static HERROR apk_exe_notiProc(const HCHAR *szProcName, apkExe_Notifier_t pfnCallback)
{
	return OAPI_EXE_Notify(szProcName, (HxRPC_Noti_t)pfnCallback);
}


#define ___API_FUNCTION___



/////////////////////////////////////////////////////////////////////// BATCH
HERROR APK_EXE_BATCH_Start(DxBatchType_e eType)
{
	HxLOG_Trace();
	return apk_exe_startProc(OxEXE_PROC_BATCH, "iii" ,(HUINT32)eType,0,0);
}


HERROR APK_EXE_BATCH_Install(HUINT32 ulCount/*,DxBatchType_e eType, DxBatchProc_b eProc*/, ...)
{
	HUINT32 pProcBuf[DxBATCH_PROC_MAX_COUNT];
	HUINT32 i = 0;
	va_list ap;
	HxLOG_Trace();

	if(ulCount >= DxBATCH_PROC_MAX_COUNT +1)
		return ERR_FAIL;

	memset(pProcBuf, 0x00, DxBATCH_PROC_MAX_COUNT);
	va_start(ap, ulCount);
	while(i < ulCount)
	{
		pProcBuf[i++] = va_arg(ap, HUINT32);
	}
	va_end(ap);
	return apk_exe_setProcByBin(OxEXE_PROC_BATCH, pProcBuf, sizeof(HUINT32)*DxBATCH_PROC_MAX_COUNT);
}

HERROR APK_EXE_BATCH_Stop(void)
{
	HxLOG_Trace();
	return apk_exe_stopProc(OxEXE_PROC_BATCH, "iii" ,0,0,0);
}


static APK_EXE_BATCH_Notifier_t		s_BATCH_Notifier = NULL;

void apk_exe_Notifier_batch(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	DxBatchType_e eType	= HxOBJECT_INT(apArgv[0]);
	DxBatchProc_b eProc = HxOBJECT_INT(apArgv[1]);
	HERROR eERR = HxOBJECT_INT(apArgv[2]);
	HxLOG_Trace();

	HxLOG_Print("[%s] eType[%d] eProc[%d] eERR[%d]\n", __FUNCTION__,eType, eProc, eERR);

	if(s_BATCH_Notifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)s_BATCH_Notifier;
		stApkEvtArgs.apArgV[1] = (void *)eType;
		stApkEvtArgs.apArgV[2] = (void *)eProc;
		stApkEvtArgs.apArgV[3] = (void *)eERR;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Print("[%s] No s_BATCH_Notifier\n", __FUNCTION__);
	}
}

HERROR APK_EXE_BATCH_RegisterNotifier(APK_EXE_BATCH_Notifier_t	fnNotifier)
{
	HxLOG_Trace();
	s_BATCH_Notifier = fnNotifier;
	return apk_exe_notiProc(OxEXE_PROC_BATCH, apk_exe_Notifier_batch);
}

static APK_EXE_FACTORYDEFAULT_Notifier_t		s_FACTORYDEFAULT_Notifier = NULL;

void apk_exe_Notifier_factorydefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	if(s_FACTORYDEFAULT_Notifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)s_FACTORYDEFAULT_Notifier;
		stApkEvtArgs.apArgV[1] = (void *)0;
		stApkEvtArgs.apArgV[2] = (void *)0;
		stApkEvtArgs.apArgV[3] = (void *)0;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Print("[%s] No s_BATCH_Notifier\n", __FUNCTION__);
	}
}


HERROR APK_EXE_FACTORYDEFAULT_RegisterNotifier(APK_EXE_FACTORYDEFAULT_Notifier_t	fnNotifier)
{
	HxLOG_Trace();
	s_FACTORYDEFAULT_Notifier = fnNotifier;
	return apk_exe_notiProc(OxEXE_PROC_FACTORYDEFAULT, apk_exe_Notifier_factorydefault);
}

/////////////////////////////////////////////////////////////////////// Factory Default
void apk_exe_Notifier_factoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HxLOG_Trace();
	HxLOG_Print("[%s] apArgv[%d]\n", __FUNCTION__,HxOBJECT_INT(apArgv[0]));

	if (HxOBJECT_INT(apArgv[0]) != 0)
	{
		PAPI_RequestReboot();
	}
}

HERROR APK_EXE_StartFactoryDefault(DxFactoryDefault_type_e eFactoryDefaultMode)
{
	HxLOG_Print("APK_EXE_StartFactoryDefault [%d] [%d]\n", eFactoryDefaultMode, (HUINT32)eFactoryDefaultMode);
	HxLOG_Trace();
	HAPI_BroadcastSignal("signal:onFactoryDefault", 0, "i", eFactoryDefaultMode);

	DAPI_ResetAll();

	apk_exe_notiProc(OxEXE_PROC_FACTORYDEFAULT, apk_exe_Notifier_factoryDefault);
	return apk_exe_startProc(OxEXE_PROC_FACTORYDEFAULT , "iii", (HUINT32)eFactoryDefaultMode,0,0);
}


///////////////////////////////////////////////////////////////////////  MHEGIB
HERROR	APK_EXE_MHEGIB_RequestFile(HUINT32 ulSessionId, HINT32 nRequestNameLen, const HCHAR *pszRequestName)
{
	HxLOG_Trace();

	return apk_exe_setProc(OxEXE_PROC_MHEGIB_FILEREQ, "iis", (HINT32)ulSessionId, nRequestNameLen, (HUINT32)pszRequestName);
}

HERROR	APK_EXE_MHEGIB_RequestData(HUINT32 ulSessionId, HINT32 nRequestNameLen, const HCHAR *pszRequestName)
{
	HxLOG_Trace();

	return apk_exe_setProc(OxEXE_PROC_MHEGIB_DATEREQ, "iis", (HINT32)ulSessionId, nRequestNameLen, (HUINT32)pszRequestName);
}

HERROR	APK_EXE_MHEGIB_AbortByUser(HUINT32 ulSessionId)
{
	HxLOG_Trace();

	return apk_exe_setProc(OxEXE_PROC_MHEGIB_ABORTBYUSER, "iii", (HINT32)ulSessionId, 0,0);
}

HERROR	APK_EXE_MHEGIB_AbortBySystem(HUINT32 ulSessionId)
{
	HxLOG_Trace();

	return apk_exe_setProc(OxEXE_PROC_MHEGIB_ABORTBYSYSTEM, "iii", (HINT32)ulSessionId, 0,0);
}

///////////////////////////////////////////////////////////////////////  MHEG
static	APK_EXE_MHEG_Notifier_t	s_MHEG_Notifier = NULL;
HERROR APK_EXE_MHEG_Start(HUINT32 nWndId, HUINT32 nStillWndId)
{
	HxLOG_Trace();
	return apk_exe_startProc(OxEXE_PROC_MHEG, "iii" ,(HUINT32)nWndId,(HUINT32)nStillWndId,0);
}

HERROR APK_EXE_MHEG_SetKey(HUINT32 key)
{
	HxLOG_Trace();
	return apk_exe_setProc(OxEXE_PROC_MHEG_KEY, "iii" ,key,0,0);
}

HERROR APK_EXE_MHEG_DeepLink(const HCHAR *pszURL, HUINT32 prevLcn)
{
	HxLOG_Trace();
	return apk_exe_setProc(OxEXE_PROC_MHEG_DEEPLINK, "sii" ,(HUINT32)pszURL,prevLcn,0);
}

HERROR APK_EXE_MHEG_ControlOP(HBOOL bControl)
{
	HxLOG_Trace();
	return apk_exe_setProc(OxEXE_PROC_MHEG_OPCTRL, "iii" ,(HUINT32)bControl,0,0);
}

void apk_exe_Notifier_mheg(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HxLOG_Trace();
	if(s_MHEG_Notifier != NULL)
	{
		DxMhegEvent_e eMhegEvent = HxOBJECT_INT(apArgv[0]);

		if(eMhegEvent == eDxMHEGEVT_FLIPWIN)
		{
			HUINT32	ulWndId;
			DxRect_t	stRect;
			ulWndId = HxOBJECT_INT(apArgv[1]);
			stRect.nX = HxOBJECT_INT(apArgv[2]);
			stRect.nY = HxOBJECT_INT(apArgv[3]);
			stRect.nW = HxOBJECT_INT(apArgv[4]);
			stRect.nH = HxOBJECT_INT(apArgv[5]);
			s_MHEG_Notifier((HUINT32)eMhegEvent, (HUINT32)ulWndId, (HUINT32)&stRect);
		}
		else if(eMhegEvent == eDxMHEGEVT_LAUNCHAPP)
		{
			HCHAR * appName = HxOBJECT_STR(apArgv[1]);
			s_MHEG_Notifier((HUINT32)eMhegEvent, (HUINT32)appName, (HUINT32)NULL);
		}
		else if(eMhegEvent == eDxMHEGEVT_APPSTATE)
		{
			HUINT32	ulAppState, ulUserData;
			ulAppState = HxOBJECT_INT(apArgv[1]);
			ulUserData = HxOBJECT_INT(apArgv[2]);
			s_MHEG_Notifier((HUINT32)eMhegEvent, (HUINT32)ulAppState, (HUINT32)ulUserData);
		}
	}
}

HERROR APK_EXE_MHEG_RegisterNotifier(APK_EXE_MHEG_Notifier_t fnNotifier)
{
	HxLOG_Trace();
	s_MHEG_Notifier = fnNotifier;
	return apk_exe_notiProc(OxEXE_PROC_MHEG, apk_exe_Notifier_mheg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
static APK_EXE_ImExportDBNotifier_t s_ImExportDbNotifier = NULL;

void apk_exe_Notifier_ImExportDB(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HxLOG_Trace();
	if(s_ImExportDbNotifier != NULL)
	{
		HUINT32	nType 	= 	HxOBJECT_INT(apArgv[0]);
		HUINT32 nResult	=	HxOBJECT_INT(apArgv[1]);
		s_ImExportDbNotifier(nType, nResult);
	}
}

HERROR APK_EXE_ExportDB(APK_EXE_ImExportDBNotifier_t fnNotifier, HBOOL bSync)
{
	HxLOG_Trace();

	s_ImExportDbNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_IMEXPORTDB, apk_exe_Notifier_ImExportDB);
	return apk_exe_startProc(OxEXE_PROC_EXPORTDB , "iii", (HUINT32)bSync,0,0);
}

HERROR APK_EXE_ImportDB(APK_EXE_ImExportDBNotifier_t fnNotifier, HBOOL bSync)
{
	HxLOG_Trace();

	s_ImExportDbNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_IMEXPORTDB, apk_exe_Notifier_ImExportDB);
	return apk_exe_startProc(OxEXE_PROC_IMPORTDB , "iii", (HUINT32)bSync,0,0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
static APK_EXE_LcnUpdateNotifier_t s_LcnUpdateNotifier = NULL;
void apk_exe_Notifier_LcnUpdate(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 eventType 		= HxOBJECT_INT(apArgv[0]);
	DxLcnUpdateInform_t *pResultEvent = NULL;

	HxLOG_Trace();

	if(s_LcnUpdateNotifier != NULL)
	{
		switch(eventType)
		{
			case eDxLCNUPDATE_START_OK:
			{
				HUINT32 ulSvcListNum = HxOBJECT_INT(apArgv[1]);
				DxLcnUpdateServiceList_t *pstsvcList = HxOBJECT_BIN_DATA(apArgv[2]);

				int i = 0;

				pResultEvent = (DxLcnUpdateInform_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateInform_t));
				HxSTD_MemSet(pResultEvent, 0, sizeof(DxLcnUpdateInform_t));

				pResultEvent->lcnUpdateStartData = (DxLcnUpdateStartData_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateStartData_t));
				HxSTD_MemSet(pResultEvent->lcnUpdateStartData, 0x00, sizeof(DxLcnUpdateStartData_t));

				HxLOG_Debug("\n[%s,%d] eventType : 0x%X , ulSvcListNum : %ld\n",__FUNCTION__,__LINE__, eventType, ulSvcListNum);

				pResultEvent->eventType		= eventType;
				pResultEvent->lcnUpdateStartData->ulSvcListNum 	= ulSvcListNum;

				if(ulSvcListNum > 0)
				{
					pResultEvent->lcnUpdateStartData->svcList = (DxLcnUpdateServiceList_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateServiceList_t) * ulSvcListNum);
					HxSTD_MemSet(pResultEvent->lcnUpdateStartData->svcList, 0x00, sizeof(DxLcnUpdateServiceList_t) * ulSvcListNum);

					for (i = 0; i< ulSvcListNum; i++)
					{
						HxSTD_memcpy(pResultEvent->lcnUpdateStartData->svcList[i].szSvcListName, pstsvcList[i].szSvcListName, DxLCN_SVC_LIST_NAME_LEN);
						HxLOG_Debug("[%s,%d][%d] LCN Svc NAME : %s \n",__FUNCTION__,__LINE__, i ,pResultEvent->lcnUpdateStartData->svcList[i].szSvcListName);
					}
				}
			}
			break;

			case eDxLCNUPDATE_SAVE_OK:
			{
				HUINT32 ulNewSvcNum = HxOBJECT_INT(apArgv[1]);
				DxLcnUpdateService_t *pstNewSvc = HxOBJECT_BIN_DATA(apArgv[2]);

				HUINT32 ulReplacedSvcNum = HxOBJECT_INT(apArgv[3]);
				DxLcnUpdateService_t *pstReplacedSvc = HxOBJECT_BIN_DATA(apArgv[4]);

				int i = 0;

				/* pResultEvent */
				pResultEvent = (DxLcnUpdateInform_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateInform_t));
				HxSTD_MemSet(pResultEvent, 0, sizeof(DxLcnUpdateInform_t));

				pResultEvent->eventType = eventType;

				/* pResultEvent->lcnUpdateSaveData */
				pResultEvent->lcnUpdateSaveData = (DxLcnUpdateSaveData_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateSaveData_t));
				HxSTD_MemSet(pResultEvent->lcnUpdateSaveData, 0x00, sizeof(DxLcnUpdateSaveData_t));

				pResultEvent->lcnUpdateSaveData->ulNewSvcNum = ulNewSvcNum;
				HxLOG_Print("\n[%s,%d]  ulNewSvcNum : %d\n",__FUNCTION__,__LINE__, ulNewSvcNum);

				/* pResultEvent->lcnUpdateSaveData->newSvc */
				if(ulNewSvcNum > 0)
				{
					pResultEvent->lcnUpdateSaveData->newSvc = (DxLcnUpdateService_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateService_t) * ulNewSvcNum);
					HxSTD_MemSet(pResultEvent->lcnUpdateSaveData->newSvc, 0x00, sizeof(DxLcnUpdateService_t) * ulNewSvcNum);

					for(i=0; i<ulNewSvcNum; i++)
					{
						// save lcn & service name
						pResultEvent->lcnUpdateSaveData->newSvc[i].lcn = pstNewSvc[i].lcn;
						HxSTD_memcpy(pResultEvent->lcnUpdateSaveData->newSvc[i].szSvcName, pstNewSvc[i].szSvcName, 128);

						// save triple id
						pResultEvent->lcnUpdateSaveData->newSvc[i].usOnId = pstNewSvc[i].usOnId;
						pResultEvent->lcnUpdateSaveData->newSvc[i].usTsId = pstNewSvc[i].usTsId;
						pResultEvent->lcnUpdateSaveData->newSvc[i].usSvcId = pstNewSvc[i].usSvcId;

						// save service uid
						pResultEvent->lcnUpdateSaveData->newSvc[i].ulSvcUid = pstNewSvc[i].ulSvcUid;

						printf("[%s,%d][%d] Svc NAME : %s \n",__FUNCTION__,__LINE__, i ,pstNewSvc[i].szSvcName);
					}
				}

				pResultEvent->lcnUpdateSaveData->ulReplacedSvcNum = ulReplacedSvcNum;
				printf("\n[%s,%d]  ulReplacedSvcNum : %d\n",__FUNCTION__,__LINE__, ulReplacedSvcNum);

				/* pResultEvent->lcnUpdateSaveData->replacedSvc */
				if(ulReplacedSvcNum > 0)
				{
					pResultEvent->lcnUpdateSaveData->replacedSvc = (DxLcnUpdateService_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateService_t) * ulReplacedSvcNum);
					HxSTD_MemSet(pResultEvent->lcnUpdateSaveData->replacedSvc, 0x00, sizeof(DxLcnUpdateService_t) * ulReplacedSvcNum);

					for(i=0; i<ulReplacedSvcNum; i++)
					{
						// save lcn & service name
						pResultEvent->lcnUpdateSaveData->replacedSvc[i].lcn = pstReplacedSvc[i].lcn;
						HxSTD_memcpy(pResultEvent->lcnUpdateSaveData->replacedSvc[i].szSvcName, pstReplacedSvc[i].szSvcName, 128);
						HxLOG_Print("[%s,%d][%d] Svc NAME : %s \n",__FUNCTION__,__LINE__, i ,pstReplacedSvc[i].szSvcName);

						// save triple id
						pResultEvent->lcnUpdateSaveData->replacedSvc[i].usOnId = pstReplacedSvc[i].usOnId;
						pResultEvent->lcnUpdateSaveData->replacedSvc[i].usTsId = pstReplacedSvc[i].usTsId;
						pResultEvent->lcnUpdateSaveData->replacedSvc[i].usSvcId = pstReplacedSvc[i].usSvcId;

						// save service uid
						pResultEvent->lcnUpdateSaveData->replacedSvc[i].ulSvcUid = pstReplacedSvc[i].ulSvcUid;
					}
				}
			}
			break;

			case eDxLCNUPDATE_STOP_OK:
			case eDxLCNUPDATE_START_FAIL:
			case eDxLCNUPDATE_SAVE_FAIL:
			case eDxLCNUPDATE_STOP_FAIL:
				pResultEvent = (DxLcnUpdateInform_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateInform_t));
				HxSTD_MemSet(pResultEvent, 0, sizeof(DxLcnUpdateInform_t));

				pResultEvent->eventType		= eventType;
				break;

			case eDxLCNUPDATE_GET_INFO:
				{
					DxLcnUpdateInfo_t *pstInfo = HxOBJECT_BIN_DATA(apArgv[1]);

					pResultEvent = (DxLcnUpdateInform_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateInform_t));
					HxSTD_MemSet(pResultEvent, 0, sizeof(DxLcnUpdateInform_t));

					pResultEvent->eventType = eventType;

					pResultEvent->lcnUpdateInfo = (DxLcnUpdateInfo_t*)HLIB_MEM_Malloc(sizeof(DxLcnUpdateInfo_t));
					HxSTD_MemSet(pResultEvent->lcnUpdateInfo, 0, sizeof(DxLcnUpdateInfo_t));

					pResultEvent->lcnUpdateInfo->ulTvNum = pstInfo->ulTvNum;
					pResultEvent->lcnUpdateInfo->ulRadioNum = pstInfo->ulRadioNum;
					pResultEvent->lcnUpdateInfo->bDisplayInfo = pstInfo->bDisplayInfo;
					HxSTD_memcpy(pResultEvent->lcnUpdateInfo->szServiceListName, pstInfo->szServiceListName, 64);
					HxSTD_memcpy(pResultEvent->lcnUpdateInfo->szLastUpdatedTime, pstInfo->szLastUpdatedTime, 20);
					HxSTD_memcpy(pResultEvent->lcnUpdateInfo->szLastCheckedTime, pstInfo->szLastCheckedTime, 20);
					pResultEvent->lcnUpdateInfo->ulColdbootResult = pstInfo->ulColdbootResult;
				}
				break;

			default:
				break;
		}

		s_LcnUpdateNotifier(pResultEvent);
	}
}

HERROR APK_EXE_StartLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier, HUINT32 ulSearchMode)
{
	HxLOG_Trace();

	s_LcnUpdateNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_LCNUPDATE, apk_exe_Notifier_LcnUpdate);
	return apk_exe_startProc(OxEXE_PROC_STARTLCNUPDATE , "iii", ulSearchMode,0,0);
}

HERROR APK_EXE_StopLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier)
{
	HxLOG_Trace();

	s_LcnUpdateNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_LCNUPDATE, apk_exe_Notifier_LcnUpdate);
	return apk_exe_startProc(OxEXE_PROC_STOPLCNUPDATE , "iii", 0,0,0);
}

HERROR APK_EXE_SaveLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier, HUINT32 ulSearchMode, HUINT32 ulIndex)
{
	HxLOG_Trace();

	s_LcnUpdateNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_LCNUPDATE, apk_exe_Notifier_LcnUpdate);
	return apk_exe_startProc(OxEXE_PROC_SAVELCNUPDATE , "iii", ulSearchMode,ulIndex,0);
}

HERROR APK_EXE_GetLcnUpdateInfo(APK_EXE_LcnUpdateNotifier_t fnNotifier)
{
	HxLOG_Trace();

	s_LcnUpdateNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_LCNUPDATE, apk_exe_Notifier_LcnUpdate);
	return apk_exe_startProc(OxEXE_PROC_GETLCNUPDATEINFO , "iii", 0, 0, 0);

}

HERROR APK_EXE_RemoveLcnUpdateInfo(HBOOL bSyncMode)
{
	HxLOG_Trace();

	return apk_exe_startProc(OxEXE_PROC_REMOVELCNUPDATEINFO , "iii", bSyncMode, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
HERROR APK_EXE_TVTVEPG_Initdirectory(void)
{
	HxLOG_Trace();
	DAPI_ResetTable(DxNAMEOF(eDxEPG_TYPE_TVTV));
	DAPI_ResetTable(DxNAMEOF(DxTvtvService_t));

	return apk_exe_startProc(OxEXE_PROC_TVTVEPGINITDIRECTORY , "iii", 0,0,0);
}

HERROR APK_EXT_TVTVEPG_Setting(HUINT32 ulEnable)
{
	HxLOG_Trace();

	if(ulEnable == FALSE)
	{
		return apk_exe_startProc(OxEXE_PROC_TVTVEPGREMOVEINFODIRECTORY , "iii", 0,0,0);
	}
	else
	{
		return ERR_FAIL;
	}
}

HERROR APK_EXE_Load_BaseEPG(void)
{
	HxLOG_Trace();

	return apk_exe_startProc(OxEXE_PROC_LOADBASEEPG , "iii", 0,0,0);
}

HERROR APK_EXE_Load_TVTVEPG(void)
{
	HxLOG_Trace();

	return apk_exe_startProc(OxEXE_PROC_LOADTVTVEPG , "iii", 0,0,0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////

HERROR APK_EXE_StartSatRecord(void)
{
	HxLOG_Trace();

	return apk_exe_startProc(OxEXE_PROC_STARTSATRECORD , "iii", 0,0,0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
static APK_EXE_TrdConflictNotifier_t s_TrdConflictNotifier = NULL;
void apk_exe_Notifier_TrdConflict(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 eventType 		= HxOBJECT_INT(apArgv[0]);
	DxTrdConflictData_t *pResultEvent = NULL;
	HUINT32 i;
	HUINT32 ulTrdConflictCount=0;
	DxTrdConflictList_t	*pstInfo = NULL;

	HxLOG_Trace();

	if(s_TrdConflictNotifier != NULL)
	{
		switch(eventType)
		{
			case eDxTRDCONFLICT_GET_INFO:
				{

					ulTrdConflictCount = HxOBJECT_INT(apArgv[1]);
					pstInfo = HxOBJECT_BIN_DATA(apArgv[2]);

					pResultEvent = (DxTrdConflictData_t*)HLIB_MEM_Malloc(sizeof(DxTrdConflictData_t));
					HxSTD_MemSet(pResultEvent, 0, sizeof(DxTrdConflictData_t));


					if(ulTrdConflictCount > 0)
					{
						pResultEvent->eventType = eventType;
						pResultEvent->ulTrdConflictCount = ulTrdConflictCount;
						pResultEvent->trdconflictInfo = (DxTrdConflictList_t*)HLIB_MEM_Malloc(sizeof(DxTrdConflictList_t));
						HxSTD_MemSet(pResultEvent->trdconflictInfo, 0, sizeof(DxTrdConflictList_t));

						pResultEvent->trdconflictInfo->ulArrayNum = pstInfo->ulArrayNum;
						for(i = 0;i<pstInfo->ulArrayNum;i++)
						{
							pResultEvent->trdconflictInfo->stTrdConflictedArray[i].ulLen = pstInfo->stTrdConflictedArray[i].ulLen;
							HxSTD_memcpy(pResultEvent->trdconflictInfo->stTrdConflictedArray[i].szTrdConflictedSvcName, pstInfo->stTrdConflictedArray[i].szTrdConflictedSvcName, pstInfo->stTrdConflictedArray[i].ulLen);
						}

					}
					else
					{
						pResultEvent->eventType = eventType;
						pResultEvent->ulTrdConflictCount = ulTrdConflictCount;
						pResultEvent->trdconflictInfo = NULL;
					}
				}
				break;

			default:
				break;
		}

		s_TrdConflictNotifier(pResultEvent);
	}
}
HERROR APK_EXE_GetTrdConflictInfo(APK_EXE_TrdConflictNotifier_t fnNotifier,HUINT32 depth)
{
	HxLOG_Trace();

	s_TrdConflictNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_TRDCONFLICT, apk_exe_Notifier_TrdConflict);
	return apk_exe_startProc(OxEXE_PROC_GETTRDCONFLICTINFO , "iii", depth, 0, 0);

}

HERROR APK_EXE_SetTrdConflictInfo(HUINT32 depth, HUINT32 index)
{
	HxLOG_Trace();
	return apk_exe_startProc(OxEXE_PROC_SETTRDCONFLICTINFO , "iii", depth, index, 0);
}

HERROR APK_EXE_SetNcdNotifyData(HUINT32 ulASONotifyFlag)
{
	HxLOG_Trace();
	return apk_exe_startProc(OxEXE_PROC_SETNCDNOTIFYDATA, "iii", ulASONotifyFlag, 0, 0);
}

static APK_EXE_NcdInfoNotifier_t s_NcdInfoNotifier = NULL;
void apk_exe_Notifier_NcdInfo(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 eventType		= HxOBJECT_INT(apArgv[0]);
	DxNcdNetworkInfo_t *pResultEvent = NULL;
	DxNcdNetworkInfo_t *pstInfo = NULL;

	HxLOG_Trace();

	if(s_NcdInfoNotifier != NULL)
	{
		switch(eventType)
		{
			case eDxNCDEVT_GET_INFO:
				{

					pstInfo = HxOBJECT_BIN_DATA(apArgv[1]);

					pResultEvent = (DxNcdNetworkInfo_t*)HLIB_MEM_Malloc(sizeof(DxNcdNetworkInfo_t));
					HxSTD_MemSet(pResultEvent, 0, sizeof(DxNcdNetworkInfo_t));

					pResultEvent->eventType = eventType;
					HxSTD_memcpy(pResultEvent->szNcdnetworkMsg, pstInfo->szNcdnetworkMsg, HxSTD_StrLen(pstInfo->szNcdnetworkMsg));
					printf("[apk_exe_Notifier_NcdInfo] pResultEvent->szNcdnetworkMsg : %s\n",pResultEvent->szNcdnetworkMsg);
				}
				break;

			default:
				break;
		}


		s_NcdInfoNotifier((HCHAR *)pResultEvent);
	}
}

HERROR APK_EXE_GetNcdInfo(APK_EXE_NcdInfoNotifier_t fnNotifier,HUINT32 param)
{
	HxLOG_Trace();

	s_NcdInfoNotifier	= fnNotifier;
	apk_exe_notiProc(OxEXE_PROC_NCDINFO, apk_exe_Notifier_NcdInfo);
	return apk_exe_startProc(OxEXE_PROC_GETNCDINFO , "iii", param, 0, 0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
HERROR APK_EXE_Init(void)
{

	return ERR_OK;
}
////////////////////////////////////////////////////////////////////////////////////


/* end of file */
