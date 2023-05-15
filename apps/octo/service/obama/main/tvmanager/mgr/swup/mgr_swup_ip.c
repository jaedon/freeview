/**
	@file     ap_ota_ip.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <svc_pipe.h>
#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>
#include <linkedlist.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_swup.h>
#include <svc_resources.h>

#include <isosvc.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>
#include <mgr_swup.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSWUP_IPMODE_UNKNOWN,
	eSWUP_IPMODE_DETECT,
	eSWUP_IPMODE_DOWNLOAD
} SWUP_IPMODE_e;

typedef	struct tagIPUpdate_Contents_t
{
	Handle_t		hSWUpdate;
	Handle_t		hIpHandle;

	SWUP_IPMODE_e	eCurrentControlMode;

	SvcSwUpdate_DetectParamIp_t	stDetectParam;
	SvcSwUpdate_DetectResult_t	stDetectResult;
} IPUpdateContents_t;

static HCHAR pszDownloadVfd[32];
static IPUpdateContents_t	s_stIpOTAContents;

/* internal function prototype */

#define	_________________________Body___________________________________

static IPUpdateContents_t*	mgr_swup_IpControlGetContents(void)
{
	return &s_stIpOTAContents;
}

#define	____________________for__UI_____________________


#define _________________________________________________________________________________

#define	_____CONTENTS_INITIATION_____
#define		CHECK_IF_MODE(a)			if (!a) {HxLOG_Error("IPControl Mode is not same...\n"); break;}

#define	_____EXTERNAL_PROCESS_FUNCTION_____
static BUS_Result_t mgr_swup_IpControlProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	IPUpdateContents_t	*pstContents;

	pstContents = mgr_swup_IpControlGetContents();
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			{
				pstContents->hSWUpdate = hAction;
				pstContents->eCurrentControlMode = (p1 == eSWUP_STATE_Detect) ? eSWUP_IPMODE_DETECT : eSWUP_IPMODE_DOWNLOAD;
				switch (pstContents->eCurrentControlMode)
				{
					case eSWUP_IPMODE_DETECT:
						pstContents->stDetectParam.szUrl = (HINT8*)p2;
						pstContents->hIpHandle = SVC_SWUPDATE_StartDetectIp(&pstContents->stDetectParam);
						break;
					case eSWUP_IPMODE_DOWNLOAD:
						{
							SvcSwUpdate_DownloadSourceInfoIp_t	stDownloadInfo;

							stDownloadInfo.szUrl = (HINT8*)p2;
							pstContents->hIpHandle = SVC_SWUPDATE_StartDownloadIp(&stDownloadInfo, DDB_SAVE_FILE_PATH);
							if (pstContents->hIpHandle)
							{
								HCHAR pszDownloadPf[32];

								HxSTD_MemSet(pszDownloadPf, '\0', 32);
								HxSTD_snprintf(pszDownloadPf, 32, "D%03d%%", p3);

								SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, pszDownloadPf, eDxPANEL_ALIGN_CENTER);
							}
						}
						break;
					default:
						break;
				}
			}
			break;
		case eMEVT_BUS_DESTROY:
			if(pstContents->eCurrentControlMode == eSWUP_IPMODE_DETECT)
			{
				SVC_SWUPDATE_StopDetectIp(pstContents->hIpHandle);
			}
			else if(pstContents->eCurrentControlMode == eSWUP_IPMODE_DOWNLOAD)
			{
				SVC_SWUPDATE_StopDownloadIp(pstContents->hIpHandle);
			}
			else
				HxLOG_Error("Wrong control mode\n");
				
			break;

		case eMEVT_BUS_GO_OPERATION :
			SVC_SWUPDATE_StopDownloadIp(pstContents->hIpHandle);
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)pstContents->hSWUpdate, 0, 0);
			break;

		case eMEVT_BUS_READY_SHUTDOWN :
			SVC_SWUPDATE_StopDownloadIp(pstContents->hIpHandle);
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)pstContents->hSWUpdate, 0, 0);
			break;

		case eSEVT_SWUP_DETECT_SUCCESS:
		case eSEVT_SWUP_DETECT_FAIL:
			{	//	p1 : hIpHandle, p2 : 0, p3 : SvcSwUpdate_DetectResult_t
				CHECK_IF_MODE(pstContents->eCurrentControlMode == eSWUP_IPMODE_DETECT);

				HxSTD_MemSet(&pstContents->stDetectResult, 0, sizeof(SvcSwUpdate_DetectResult_t));

				if (p3)
					HxSTD_MemCopy(&pstContents->stDetectResult, (void*)p3, sizeof(SvcSwUpdate_DetectResult_t));

				pstContents->stDetectResult.eSource = eDxSWUPDATE_SOURCE_Ip;
				if (message == eSEVT_SWUP_DETECT_SUCCESS)
				{
					HERROR hErr = ERR_FAIL;
					hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)&pstContents->stDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
					if (hErr != ERR_OK)
					{
						HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
						MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
						return ERR_BUS_NO_ERROR;
					}
					else
					{
						MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Unknown, 0,0,0);
						MGR_SWUP_SetDetectInfo(pstContents->hSWUpdate, &pstContents->stDetectResult);
					}
				}
				else
				{
					MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0, 0, 0);
				}
			}
			break;

		case eSEVT_SWUP_DOWNLOAD_SUCCESS:
			{
				//	p1 : hIpHandle
				HxLOG_Debug(" eSEVT_SWUP_DOWNLOAD_SUCCESS in %s\n", __FUNCTION__);
				CHECK_IF_MODE(pstContents->eCurrentControlMode == eSWUP_IPMODE_DOWNLOAD);
				CHECK_IF_MODE(pstContents->hIpHandle == (Handle_t)p1);

				MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS, (Handle_t)NULL, 0, 0, 0);

				BUS_MGR_Destroy(NULL);
			}
			break;
		case eSEVT_SWUP_DOWNLOAD_FAIL:
			{	//	p1 : hIpHandle, p2 : FailReason
				HxLOG_Debug(" eSEVT_SWUP_DOWNLOAD_FAIL in %s\n", __FUNCTION__);
				CHECK_IF_MODE(pstContents->eCurrentControlMode == eSWUP_IPMODE_DOWNLOAD);
				CHECK_IF_MODE(pstContents->hIpHandle == (Handle_t)p1);

				MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL, (Handle_t)NULL, p2, 0, 0);

				BUS_MGR_Destroy(NULL);
			}
			break;
		case eSEVT_SWUP_DOWNLOAD_PROGRESS:
			{	//	p1 : hIpHandle, p2 : Total Size, p3 : receieved.
				CHECK_IF_MODE(pstContents->eCurrentControlMode == eSWUP_IPMODE_DOWNLOAD);
				if(p1==0)
				{
					SVC_SWUPDATE_StopDownloadIp(pstContents->hIpHandle);
					break;
				}
				CHECK_IF_MODE(pstContents->hIpHandle == (Handle_t)p1);

				HxSTD_MemSet(pszDownloadVfd, '\0', 32);
				HxSTD_snprintf(pszDownloadVfd, 32, "D%03d%%", p3);

				SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_BG_ACTION, pszDownloadVfd, eDxPANEL_ALIGN_CENTER);

				MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS, (Handle_t)NULL, p2, p3, 0);
			}
			break;
		case eSEVT_SWUP_DOWNLOAD_STOP_RESULT:
			{	//	p1 : hIpHandle, p2 : TRUE : stop Success, FALSE : stop FAIL
				CHECK_IF_MODE(pstContents->eCurrentControlMode == eSWUP_IPMODE_DOWNLOAD);
				CHECK_IF_MODE(pstContents->hIpHandle == (Handle_t)p1);

				MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_DOWNLOAD_STOP, (Handle_t)NULL, p2, 0, 0);

				BUS_MGR_Destroy(NULL);
			}
			break;
		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#define _____OTA_TP_____
HERROR	MGR_SWUP_IpCreate(Handle_t hSWUpdate, MgrSwup_State_e eState, HCHAR *pszServerUrl)
{
	BUS_Mgr_t	mgr;

	mgr = BUS_MGR_Get(mgr_swup_IpControlProc);
	if (mgr)
		BUS_MGR_Destroy(mgr_swup_IpControlProc);

	BUS_MGR_Create("IPSWUP_CONTROL_PROC", APP_DEFAULT_PRIORITY, mgr_swup_IpControlProc, hSWUpdate, (HINT32)eState, (HINT32)pszServerUrl, 0);

	return ERR_OK;
}

HERROR	MGR_SWUP_IpDestroy(Handle_t hSWUpdate)
{
	BUS_Mgr_t	mgr;

	mgr = BUS_MGR_Get(mgr_swup_IpControlProc);
	if (mgr)
		BUS_MGR_Destroy(mgr_swup_IpControlProc);

	return ERR_OK;
}

BUS_Callback_t	mgr_swup_IpGetProc(void)
{
	return (BUS_Callback_t)mgr_swup_IpControlProc;
}
/* End of File. ---------------------------------------------------------- */



