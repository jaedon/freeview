/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * @file	  		nx_core_system.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core_swup.h>
#include <nx_core_message.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static	NX_SwUpdate_t	*s_SingleSwUpContext	= NULL;

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NX_SwUpdate_t	*nx_core_swup_GetContext(void);
static HERROR		nx_core_swup_Create(NX_SwUpdate_t *pstSwUpContext, DxSwUpdate_Spec_e eSwupSpec);
static void			nx_core_swup_EventListener(DxSwUpdate_EventData_t *pvData);


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NX_SwUpdate_t	*nx_core_swup_GetContext(void)
{
	if (s_SingleSwUpContext == NULL)
	{
		s_SingleSwUpContext = (NX_SwUpdate_t*)NX_APP_Calloc(sizeof(NX_SwUpdate_t));
		return s_SingleSwUpContext;
	}
	else
	{
		return s_SingleSwUpContext;
	}
}

static HERROR	nx_core_swup_Create(NX_SwUpdate_t *pstSwUpContext, DxSwUpdate_Spec_e eSwupSpec)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_SWUPDATE_Create(eSwupSpec, &pstSwUpContext->hSwupHandle, NULL);

	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%04d] APK_SWUPDATE_Create() Error : hErr [0x%x] \n", __FUNCTION__, __LINE__, hErr);
		return ERR_FAIL;
	}
	if (HANDLE_NULL == pstSwUpContext->hSwupHandle)
	{
		HxLOG_Error("[%s:%04d] HANDLE_NULL == hSwupdate \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	return ERR_OK;
}

static void		nx_core_swup_EventListener(DxSwUpdate_EventData_t *pvData)
{
	NX_SwUpdate_t 	*pstSwUpContext = nx_core_swup_GetContext();

	if (pstSwUpContext == NULL)
	{
		HxLOG_Error("Failed Allocate NX_SwUpdate Context!!!\n");
		return;
	}

	if (pvData == NULL)
	{
		HxLOG_Error("pvData == NULL \n");
		return;
	}

	switch (pvData->eEvent)
	{
		case eDxSWUPDATE_EVENT_ConnectSuccess:
		case eDxSWUPDATE_EVENT_ConnectFail:
			// m/w 에서 state 처리 안함.
			break;

		case eDxSWUPDATE_EVENT_DetectSuccess:
		case eDxSWUPDATE_EVENT_DetectSuccessInLive:
		case eDxSWUPDATE_EVENT_DetectFail:
			{
				if (pvData != NULL)
				{
					HxLOG_Error("\n\n\n[%s.%d] eEvent(%d), signalType(%d), eSourceTYpe(%d), eState(%d), currentVer(%s), foundVer(%s)\n\n\n\n",
						__FUNCTION__,__LINE__,
						pvData->eEvent, pvData->eSignalType, pvData->eSourceType, pvData->eState,
						pvData->unEventData.stDetectEvent.szCurrentVer, pvData->unEventData.stDetectEvent.szFoundVer);
				}
				else
				{
					HxLOG_Error("\n\n\n\n[%s.%d] Detect but pvData is NULL\n\n\n",__FUNCTION__,__LINE__);
					return;
				}
			}
			HxLOG_Debug("");
			// Detect 관련은 복사해 놓는다.
			if (pstSwUpContext->stDetectedInfo == NULL)
			{
				pstSwUpContext->stDetectedInfo = (DxSwUpdate_EventData_t*)NX_APP_Calloc(sizeof(DxSwUpdate_EventData_t));
			}
			else
			{
				HxSTD_MemSet(pstSwUpContext->stDetectedInfo, 0, sizeof(DxSwUpdate_EventData_t));
			}
			HxSTD_MemCopy(pstSwUpContext->stDetectedInfo, pvData, sizeof(DxSwUpdate_EventData_t));
			ONDK_GWM_PostMessage(NULL, MSG_CORE_SWUP_NOTIFY, (Handle_t)NULL, pvData->eEvent, pvData->eState, pvData->eSourceType);
			break;

		case eDxSWUPDATE_EVENT_DownloadSuccess:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_SWUP_NOTIFY, (Handle_t)NULL, pvData->eEvent, pvData->eState, pvData->eSourceType);
			break;

		case eDxSWUPDATE_EVENT_DownloadFail:
		case eDxSWUPDATE_EVENT_DownloadProgress:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_SWUP_NOTIFY, (Handle_t)NULL, pvData->eEvent, pvData->unEventData.stProgressEvent.nProgress, pvData->unEventData.stProgressEvent.nProgressMax);
			break;

		case eDxSWUPDATE_EVENT_InstallSuccess:
		case eDxSWUPDATE_EVENT_InstallFail:
		case eDxSWUPDATE_EVENT_InstallProgress:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_SWUP_NOTIFY, (Handle_t)NULL, pvData->eEvent, pvData->unEventData.stProgressEvent.nProgress, pvData->unEventData.stProgressEvent.nProgressMax);
			break;

		case eDxSWUPDATE_EVENT_Erorr:
			break;

		case eDxSWUPDATE_EVENT_Finish:
			ONDK_GWM_PostMessage(NULL, MSG_CORE_SWUP_NOTIFY, (Handle_t)NULL, pvData->eEvent, pvData->eState, pvData->eSourceType);
			break;

		default:
			break;
	}
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
DxSwUpdate_Spec_e	NX_CORE_SWUP_GetSwUpSpec(void)
{
	DxSwUpdate_Spec_e 	eRetSpec = eDxSwUpdate_SPEC_NONE;

#if defined(CONFIG_OTA_NONE)
	eRetSpec = eDxSwUpdate_SPEC_NONE;
#elif defined(CONFIG_OTA_SINGLE)
		{
		#if defined(CONFIG_OTA_ASTRA)
			eRetSpec = eDxSwUpdate_SPEC_ASTRA;

		#elif defined(CONFIG_OTA_DVBSSU)
			eRetSpec = eDxSwUpdate_SPEC_DVBSSU;

		#elif defined(CONFIG_OTA_DTT_UK) || defined(CONFIG_OTA_DTT_EU)|| defined(CONFIG_OTA_DTT_UK_SSU)
			eRetSpec = eDxSwUpdate_SPEC_DTT;

		#elif defined(CONFIG_OTA_IRDETO)
			eRetSpec = eDxSwUpdate_SPEC_IRDETO;

		#elif defined(CONFIG_OTA_HUMAX)
			eRetSpec = eDxSwUpdate_SPEC_HUMAX;

		#elif defined(CONFIG_OTA_SES)
			eRetSpec = eDxSwUpdate_SPEC_SES;

		// 새 사양이 추가될 때마다 여기에 추가한다.

		// 추가 사양 끝
		#else
			eRetSpec = eDxSwUpdate_SPEC_NONE;

		#endif
	}
#elif defined(CONFIG_OTA_MULTIPLE)
	{
		//HxLOG_Debug("s_eMultipleOtaSpec : %d\n", s_eMultipleOtaSpec);
		//eRetSpec = s_eMultipleSwUpdateSpec;
	}
#else
	{
		eRetSpec = eDxSwUpdate_SPEC_NONE;
	}
#endif

	return eRetSpec;
}

DxSwUpdate_EventData_t *NX_CORE_SWUP_GetDetectedInform(DxSwUpdate_Spec_e eSwupSpec)
{
	NX_SwUpdate_t 	*pstSwUpContext = nx_core_swup_GetContext();

	if (pstSwUpContext == NULL)
	{
		HxLOG_Error("Failed Allocate NX_SwUpdate Context!!!\n");
		return NULL;
	}

	if (pstSwUpContext->stDetectedInfo != NULL)
		return pstSwUpContext->stDetectedInfo;

	NX_COMMON_NOT_USED_PARAM(eSwupSpec);

	return NULL;

}

HERROR	NX_CORE_SWUP_StartDetectWithType(DxSwUpdate_Spec_e eSwupSpec, Handle_t *dstSwUpHandle)
{
	HERROR			hErr = ERR_FAIL;
	NX_SwUpdate_t 	*pstSwUpContext = nx_core_swup_GetContext();

	if (pstSwUpContext == NULL || dstSwUpHandle == NULL)
	{
		HxLOG_Error("Failed Allocate NX_SwUpdate Context or dstSwUpHandle is NULL!!!\n");
		return ERR_FAIL;
	}

	// s/w update context 는 start 시에만 memset
	HxSTD_MemSet(pstSwUpContext, 0, sizeof(NX_SwUpdate_t));

	pstSwUpContext->eSwUpSpec = eSwupSpec;
	*dstSwUpHandle = pstSwUpContext->hSwupHandle;

	hErr = nx_core_swup_Create(pstSwUpContext, eSwupSpec);
	if (hErr != ERR_OK)
		return ERR_FAIL;

	hErr = APK_SWUPDATE_StartDetect(nx_core_swup_EventListener, (Handle_t)pstSwUpContext->hSwupHandle);
	return	hErr;
}

HERROR	NX_CORE_SWUP_StartDownloadWithType(DxSwUpdate_Spec_e eSwupSpec, Handle_t *dstSwUpHandle)
{
	HERROR			hErr = ERR_FAIL;
	NX_SwUpdate_t 	*pstSwUpContext = nx_core_swup_GetContext();

	if (pstSwUpContext == NULL)
	{
		HxLOG_Error("Failed Allocate NX_SwUpdate Context!!!\n");
		return ERR_FAIL;
	}

	pstSwUpContext->eSwUpSpec = eSwupSpec;
	hErr = nx_core_swup_Create(pstSwUpContext, eSwupSpec);
	if (hErr != ERR_OK)
		return ERR_FAIL;

	return APK_SWUPDATE_StartDownload(nx_core_swup_EventListener, (Handle_t)pstSwUpContext->hSwupHandle);
}

HERROR	NX_CORE_SWUP_StartInstallWithType(Handle_t hSwUpHandle)
{
	NX_SwUpdate_t 	*pstSwUpContext = nx_core_swup_GetContext();

	if (pstSwUpContext == NULL)
	{
		HxLOG_Error("Failed Allocate NX_SwUpdate Context!!!\n");
		return ERR_FAIL;
	}

	return APK_SWUPDATE_StartInstall(nx_core_swup_EventListener, (Handle_t)hSwUpHandle);
}

