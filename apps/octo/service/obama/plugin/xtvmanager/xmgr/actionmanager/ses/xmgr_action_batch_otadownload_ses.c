/****************************************************************************
*
* File Name   : manual_action_otadownload.c
*
* Description :
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/
#if 0

/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_sys.h>
#include <db_param.h>
#include <bus.h>


#include <mgr_action.h>
#include <mgr_swup.h>
#include <mgr_common.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>

#include <mgr_swup.h>
#include <xmgr_swup.h>
//#include "../local_include/_xmgr_action_Batch.h"

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
typedef struct
{
	Handle_t				hSwUpdate;

} XmgrSwup_DownloadInfo_t;
/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

#define _____Static_Function_ProtoType_____

#define _____Static_Function_____

#define _____Module_Function_____

// eMEVT_BUS_CREATE : MgrAction_InitCb_t
BUS_Result_t xmgr_action_BatchOtaDownProcessInit_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void **ppvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSwup_DownloadInfo_t	*pstOTADownloadInfo= NULL;
	Handle_t					hSwUpdate;

	HxLOG_Debug("====== (+) \n");

	if (pstStepControl == NULL)
	{
		HxLOG_Error("pstStepControl is NULL \n");
		return ERR_BUS_INITIAL_FAILED;
	}
#if 1 // TODO: check 와 download 합쳐진후 MGR_SWUP_InitBatch 만 불러주게끔 하고 제거.
	if(MGR_SWUP_Start() == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Start error \n");
		return ERR_BUS_INITIAL_FAILED;
	}
#endif
	//	임시코드... Porting Layer정리 및 중간 Data 관리 모듈 생성 필요함..
	pstOTADownloadInfo = HLIB_STD_MemAlloc(sizeof(XmgrSwup_DownloadInfo_t));
	if(pstOTADownloadInfo== NULL)
	{
		HxLOG_Error("pstOTADownloadInfo MemAlloc Error \n");
		return ERR_BUS_INITIAL_FAILED;
	}

//	hSwUpdate = MGR_SWUP_Create(NULL, TRUE);
	// TODO: TRUE로 변경
	hSwUpdate = MGR_SWUP_Create(NULL, FALSE);

	if(hSwUpdate == HANDLE_NULL)
	{
		if(pstOTADownloadInfo != NULL)
		{
			HLIB_STD_MemFree(pstOTADownloadInfo);
		}
		HxLOG_Error("MGR_SWUP_Create Fail \n");
		return ERR_BUS_INITIAL_FAILED;
	}

	pstOTADownloadInfo->hSwUpdate = hSwUpdate;

	*ppvContext = (void*)pstOTADownloadInfo;

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY : MgrAction_DestroyCb_t
HERROR xmgr_action_BatchOtaDownProcessDestory_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSwup_DownloadInfo_t 	*pstOTADownloadInfo = (XmgrSwup_DownloadInfo_t*)pvContext;

	if (pstStepControl == NULL)
	{
		HxLOG_Error("pstStepControl is NULL \n");
		return ERR_FAIL;
	}

	if(pstOTADownloadInfo == NULL)
	{
		HxLOG_Error("pstOTADownloadInfo is NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("====== (+) \n");

	if(pstOTADownloadInfo != NULL)
	{
		HLIB_STD_MemFree(pstOTADownloadInfo);
	}

	return ERR_OK;
}

// eMEVT_BUS_MGR_KILLED : MgrAction_MgrKilledCb_t
HERROR xmgr_action_BatchOtaDownProcessMgrKilled_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t	pfnAppCb;
	HBOOL			bReturn = FALSE;

	HxLOG_Debug("====== (+) \n");

	if (pstStepControl == NULL)
	{
		HxLOG_Error("pstStepControl is NULL \n");
		return ERR_FAIL;
	}

	pfnAppCb = (BUS_Callback_t)p1;
	bReturn = MGR_SWUP_CheckProc(eSWUP_TYPE_Install, 0, pfnAppCb);
	if (bReturn == TRUE)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

// eMEVT_ACTION_SHUTDOWN_NEXTPROCESS : MgrAction_DoNextProcessCb_t
BUS_Result_t xmgr_action_BatchOtaDownProcessDoProcess_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSwup_DownloadInfo_t		*pstOTADownloadInfo = (XmgrSwup_DownloadInfo_t*)pvContext;
	SvcSwUpdate_DetectResult_t		stDetectResult;
	BUS_Result_t					eRes = ERR_BUS_INITIAL_FAILED;
	HERROR						hError ;

	HxLOG_Debug("====== (+) \n");

	if (pstStepControl == NULL)
	{
		HxLOG_Error("pstStepControl is NULL \n");
		return ERR_BUS_INITIAL_FAILED;
	}

	if(pstOTADownloadInfo == NULL)
	{
		HxLOG_Error("pstOTADownloadInfo is NULL \n");
		return ERR_BUS_INITIAL_FAILED;
	}

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32 *)&stDetectResult, sizeof(SvcSwUpdate_DetectResult_t)) ;

#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
	hError = ERR_OK;

	stDetectResult.bFound = TRUE;
	stDetectResult.bAvailable = TRUE;
	stDetectResult.eSource = eDxSWUPDATE_SOURCE_Ip;
	stDetectResult.ulDataVersion = 0x20000;
#endif

	if (hError == ERR_OK)
	{
		HxLOG_Print("[%s][%04d]  bFound[%d] bAvailable [%d] eSource[%d]\n", __FUNCTION__, __LINE__, stDetectResult.bFound, stDetectResult.bAvailable, stDetectResult.eSource);
		if ((stDetectResult.bFound == TRUE) && (stDetectResult.bAvailable == TRUE))
		{
			MGR_SWUP_SetDetectInfo(pstOTADownloadInfo->hSwUpdate, &stDetectResult);
			MGR_SWUP_InitState(pstOTADownloadInfo->hSwUpdate, stDetectResult.eSource, 0, 0);
			MGR_SWUP_PostMessage(pstOTADownloadInfo->hSwUpdate, eMEVT_SWUP_DOWNLOAD_START, (Handle_t)0, 0, 0, 0);
			eRes = ERR_BUS_NO_ERROR;
		}
	}
	else
	{
		HxLOG_Error("DB_PARAM_GetItem return (%d)  \n", hError);
	}


	if(eRes != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("[XMGR_SHUTDOWN_OTADOWN_DoProcess] local_stbyam_StartDownload fail \n");
	}

	return eRes;
}

// eMEVT_ACTION_SHUTDOWN_DESTROY : MgrAction_BatchDestroyCb_t
HERROR xmgr_action_BatchOtaDownProcessShutdownDestroy_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("====== (+) \n");

	return ERR_OK;
}

// User Defined Message : MgrAction_MsgProcessCb_t
BUS_Result_t xmgr_action_BatchOtaDownProcessMsg_Ses(HINT32 nMsg, Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//XmgrSwup_DownloadInfo_t 	*pstOTADownloadInfo = (XmgrSwup_DownloadInfo_t*)pvContext;

	HxLOG_Debug("====== (+) nMsg(0x%x) p1(0x%x) \n", nMsg, p1);

	switch(nMsg)
	{
		case eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS:
			HxLOG_Debug("OTA DOWNLOAD FLASH WRITE OPERATION is SUCCESS!!!\n");

			// standby에서 ota 받고 flash writing 하면 바로 리붓 시켜버림
			if (MGR_ACTION_GetState()== eAmState_ACTIVE_STANDBY)
			{
				HxLOG_Debug("reboot!!!\n");
				MGR_ACTION_RebootSystem();
			}
			break;
#if defined (CONFIG_NAGRA_SSD_OTA_TEST)
		// success or fail 이든 다음으로 넘어간다.. install to flash 하지 않는다...
		case eMEVT_ACTION_NOTIFY_RESULT:
		{
			Mgr_Event_e eMgrEvent = p1;
			HINT32 		nDownloadResult = p2;
			BUS_Callback_t pfBatchProc = MGR_Action_BatchGetProc();

			HxLOG_Print("[%s][%04d eMEVT_ACTION_NOTIFY_RESULT] ------ \n",__FUNCTION__,__LINE__);
			if (eMgrEvent == eMGR_EVENT_SWUP_DOWNLOAD_FINISH && nDownloadResult == eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL)
			{
				BUS_SendMessage(pfBatchProc, eMEVT_ACTION_SHUTDOWN_NEXTPROCESS, HANDLE_NULL, 0, 0, 0);
			}
		}
			break;
#endif
		default:
			break;
	}

	return ERR_BUS_NO_ERROR;
}
#endif
/* End of File. ---------------------------------------------------------- */


