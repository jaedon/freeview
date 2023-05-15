/**
	@file     ap_ota_dwnld_dvbssu.c
		---> xmgr_sw_update_download_action_dvbssu.c
	@brief   OTA download application \n

	Description: OTA download application \n
	Module: Application \n
	Remarks: \n
	 Date			Modification						Name \n
	-----------	-------------------------------------	------------------ \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/


/******************************************************************************/
/******************************* Header Files *********************************/
/******************************************************************************/
#include <mgr_action.h>
#include <mgr_live.h>
#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_download.h>
#include <mgr_swup.h>
#include <xmgr_swup.h>
#include "../local_include/_xmgr_swup.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/********************************** Defines ***********************************/
/******************************************************************************/

/******************************************************************************/
/********************************** Typedef ***********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Static Variables ******************************/
/******************************************************************************/
STATIC HUINT8	*s_pszCheckSystem_BGOta = (HUINT8 *)"DOWNLOAD DATA..";
STATIC Handle_t	s_hSWUpdateHandle = HANDLE_NULL;
STATIC Handle_t	s_hDownloadActionHandle = HANDLE_NULL;
STATIC HCHAR 		s_szDownloadVfd[32];

/******************************************************************************/
/************************ Static Functions Declaration ************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global Functions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Static Functions ******************************/
/******************************************************************************/

STATIC void xmgr_swup_download_MsgGwmCreated(Handle_t hAction, DxSwUpdate_Spec_e eSwUpdateSpec, HUINT16 usPid, HUINT32 ulSignalVersion)
{
	SvcSwUpdate_DownloadSourceInfoRf_t		stSwUpRfParam;
	HUINT8								szSwUpImagePath[256];

	HxSTD_memset(szSwUpImagePath, 0, sizeof(szSwUpImagePath));
	HLIB_STD_StrUtf8NCpy((HCHAR *)szSwUpImagePath, "/tmp", 256);

	stSwUpRfParam.hAction = hAction;
	stSwUpRfParam.usPid = usPid;
	stSwUpRfParam.usTsUniqId = DB_SVC_GetAllUniqueId();
	stSwUpRfParam.ulSignalVersion = ulSignalVersion;
	stSwUpRfParam.ulArg1 = 0;
	stSwUpRfParam.ulArg2 = 0;

	s_hDownloadActionHandle = SVC_SWUPDATE_StartDownloadRf(&stSwUpRfParam, szSwUpImagePath);

	HxLOG_Debug("SVC_SWUPDATE_StartDownloadRf\n");
}

STATIC HERROR xmgr_swup_download_SaveOtaDownloadCompleted(void)
{
	DbSvc_TsInfo_t	 stTsInfo;
	SvcSi_SwupSignal_t 	 stOtaInfo;
	HERROR			 hErr;

	// serched info 가 존재하는지 check
	hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_SWUPINFO_GetSearchedInfo is Error\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_download_destroy(Handle_t hAction, HINT32 message)
{
	HUINT32			ulActionId;
	HERROR			hError = ERR_FAIL;

	HxLOG_Print("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
	HxLOG_Print("++++++++++++++ (Stop By : %d) +++++++++++++++++++\n", message);
	HxLOG_Print("+++++++++++++++++++++++++++++++++++++++++++++++++\n");

	HxLOG_Debug("Destory DvbSSU Download !!! \n");

	ulActionId = SVC_PIPE_GetActionId (hAction);

	hError = SVC_CH_StopTune(hAction, (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StopTune fail!!\n");
	}

	MGR_ACTION_Release (ulActionId, eActionType_SEARCH);
	if(hError != ERR_OK)
	{
		HxLOG_Error("MGR_ACTION_Release fail!!\n");
	}

	SVC_SWUPDATE_StopDownloadRf(s_hDownloadActionHandle);
	s_hDownloadActionHandle = HANDLE_NULL;

	BUS_MGR_Destroy(xproc_swup_Download_Dvbssu);

	return ERR_OK;
}

/******************************************************************************
 *
 *  OTA download Procedure
 *
 ******************************************************************************/
BUS_Result_t xproc_swup_Download_Dvbssu (HINT32 nMessage, Handle_t hHandle, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
#if defined(CONFIG_OTA_DOWNLOAD_BG)
	STATIC Handle_t		s_hActionHandle;
	MgrSwup_DownloadArg_t *pstDownArg;
	HBOOL				bDownloadComplete = FALSE;
	HERROR				hErr;

	switch( nMessage )
	{
		case eMEVT_BUS_CREATE:
			s_hActionHandle = hHandle;
			pstDownArg = (MgrSwup_DownloadArg_t *)nParam1;

			HxLOG_Debug("eMEVT_BUS_CREATE : OtaSpec(%d), signalVer(0x%x), OtaPid(0x%x) \n", pstDownArg->eSwUpdateSpec, pstDownArg->ulSignalVer, pstDownArg->usPid);

			// Check version with prev Downloaded ota info
			hErr = SVC_DOWNLOAD_CheckDownloadComplete(hHandle, eDOWN_DOWNLOAD_OTA_IMAGE, pstDownArg->ulSignalVer, &bDownloadComplete);

			HxLOG_Debug("hErr = %d, pstDownArg->ulSignalVer = 0x%x, bDownloadComplete = %d\n", hErr, pstDownArg->ulSignalVer, bDownloadComplete);
			if (hErr == ERR_OK && bDownloadComplete == FALSE)
			{
				s_hSWUpdateHandle = pstDownArg->hSwUpdate;
				SVC_DOWNLOAD_ClearDownloadInfo(hHandle, eDOWN_DOWNLOAD_OTA_IMAGE);
				xmgr_swup_download_MsgGwmCreated(hHandle, pstDownArg->eSwUpdateSpec, (HUINT16)pstDownArg->usPid, pstDownArg->ulSignalVer);
				SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, s_pszCheckSystem_BGOta, eDxPANEL_ALIGN_CENTER);
			}
			else
			{
				// 이미 download 완료 됨. same version
				HxLOG_Print("BG OTA download completed previously !!!!!!!\n");
				MGR_ACTION_Release (SVC_PIPE_GetActionId(hHandle), eActionType_SEARCH);
				BUS_MGR_Destroy(0);
			}
			break;

		/* from MW Download. */
		case eSEVT_SWUP_DOWNLOAD_FAIL:
			HxLOG_Debug("eSEVT_DOWN_DOWNLOAD_FAIL::: handle 0x%x\n", hHandle);
			MGR_SWUP_PostMessage(s_hSWUpdateHandle, eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL, hHandle, 0, 0, 0);
			xmgr_swup_download_destroy(s_hActionHandle, nMessage);
			break;

		//download succes인 경우 처리.
		case eSEVT_SWUP_DOWNLOAD_SUCCESS:
			// save ota info
			HxLOG_Debug("eSEVT_SWUP_DOWNLOAD_SUCCESS::: handle 0x%x\n", hHandle);
			xmgr_swup_download_SaveOtaDownloadCompleted();
			MGR_SWUP_PostMessage(s_hSWUpdateHandle, eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS, hHandle, 0, 0, 0);
			xmgr_swup_download_destroy(s_hActionHandle, nMessage);
			break;

		/* download progress status */
		case eSEVT_SWUP_DOWNLOAD_PROGRESS:
			// send download status to AP
			// param3 is progress status value ==>  received block/total block * 100
			HxLOG_Debug("eSEVT_SWUP_DOWNLOAD_PROGRESS::: handle 0x%x\n", hHandle);
			HxSTD_MemSet(s_szDownloadVfd, '\0', 32);
			HxSTD_snprintf(s_szDownloadVfd, 32, "D%03d%%", nParam3);
			SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, (HUINT8 *)s_szDownloadVfd, eDxPANEL_ALIGN_CENTER);
			MGR_SWUP_PostMessage(s_hSWUpdateHandle, eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS, hHandle, nParam3, 0, 0);
			break;

		case eSEVT_DOWN_STOPPED:
			HxLOG_Debug("eSEVT_DOWN_STOPPED:::\n" );
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("eMEVT_BUS_DESTROY:::\n");
			xmgr_swup_download_destroy(s_hActionHandle, nMessage);
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(nMessage, hHandle, nParam1, nParam2, nParam3);
#else
	return MESSAGE_PASS;
#endif
}

/******************************** End of File *********************************/
