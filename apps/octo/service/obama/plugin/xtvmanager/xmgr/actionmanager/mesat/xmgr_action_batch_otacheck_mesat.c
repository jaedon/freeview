/****************************************************************************
*
* File Name   : manual_action_otacheck.c
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
#include <xmgr_swup.h>

#include <dlib_swupdate.h>

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
typedef	struct
{
	Handle_t	hSwUpdate;
} XmgrActionBatch_OtaCheck_Info_t;
/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
#define _____Static_Function_ProtoType_____

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

#define _____Static_Function_____

#define _____Module_Function_____

// eMEVT_BUS_CREATE : MgrAction_InitCb_t
BUS_Result_t xmgr_action_BatchOtaCheckProcessInit_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void **ppvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrActionBatch_OtaCheck_Info_t	*pstOTACheckInfo;
	Handle_t							hSwUpdate;
	if (NULL == pstStepControl)
	{
		HxLOG_Error("pstStepControl is NULL\n");
		return ERR_BUS_INITIAL_FAILED;
	}

	HxLOG_Debug("== (+) \n");

#if 1 // TODO: check 와 download 합쳐진후 MGR_SWUP_InitBatch 만 불러주게끔 하고 제거.
	if(MGR_SWUP_Start() == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Start error \n");
		return ERR_BUS_INITIAL_FAILED;
	}
#endif

	MGR_SWUP_SetSpec(eDxSwUpdate_SPEC_HUMAX);

	pstOTACheckInfo = HLIB_STD_MemAlloc(sizeof(XmgrActionBatch_OtaCheck_Info_t));
	if(pstOTACheckInfo == NULL)
	{
		HxLOG_Error("pstOTACheckInfo MemAlloc Error \n");
		return ERR_BUS_INITIAL_FAILED;
	}

//	hSwUpdate = MGR_SWUP_Create(NULL, TRUE);
	// TODO: TRUE로 변경
	hSwUpdate = MGR_SWUP_Create(NULL, FALSE);

	if(hSwUpdate == HANDLE_NULL)
	{
		if(pstOTACheckInfo!= NULL)
			HLIB_STD_MemFree(pstOTACheckInfo);

		HxLOG_Error("hSwUpdate is NULL \n");

		return ERR_BUS_INITIAL_FAILED;
	}

	HxLOG_Debug("hSwUpdate(0x%08x) eProcess(0x%08x) \n", hSwUpdate, eProcess);

	pstOTACheckInfo->hSwUpdate = hSwUpdate;

	*ppvContext = (void*)pstOTACheckInfo;

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY : MgrAction_DestroyCb_t
HERROR xmgr_action_BatchOtaCheckProcessDestory_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR									hErr;
	SvcSwUpdate_DetectResult_t					stDetectResultInfo ;
	XmgrActionBatch_OtaCheck_Info_t			*pstOTACheckInfo = (XmgrActionBatch_OtaCheck_Info_t*)pvContext;

	if (pstOTACheckInfo == NULL || pstStepControl == NULL)
	{
		HxLOG_Error("[%s][%04d] pstOTACheckInfo or pstStepControl is NULL  ( pstOTACheckInfo : %p, pstStepControl : %p\n"
			, __FUNCTION__, __LINE__, pstOTACheckInfo, pstStepControl);

		if(pstOTACheckInfo != NULL)
			HLIB_STD_MemFree(pstOTACheckInfo);

		return ERR_FAIL;
	}

	HxSTD_MemSet(&stDetectResultInfo, 0x00, sizeof(SvcSwUpdate_DetectResult_t));

	HxLOG_Debug("[%s][%04d pstOTACheckInfo : [%x]\n", __FUNCTION__, __LINE__, pstOTACheckInfo);

	hErr = MGR_SWUP_GetDetectInfo(pstOTACheckInfo->hSwUpdate, &stDetectResultInfo);
	if (hErr == ERR_OK)
	{
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)&stDetectResultInfo, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
		}
	}
	else
	{
		HxLOG_Error("MGR_SWUP_GetDetectInfo Error  MemAlloc Error(0x%x) \n", hErr);
	}

	if(pstOTACheckInfo != NULL)
	{
		HLIB_STD_MemFree(pstOTACheckInfo);
	}

	return ERR_OK;
}

// eMEVT_BUS_MGR_KILLED : MgrAction_MgrKilledCb_t
HERROR xmgr_action_BatchOtaCheckProcessMgrKilled_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrActionBatch_OtaCheck_Info_t	*pstOTACheckInfo = (XmgrActionBatch_OtaCheck_Info_t*)pvContext;
	BUS_Callback_t	pfnAppCb;
	HBOOL			bReturn = FALSE;

	if (NULL == pstOTACheckInfo)
	{
		HxLOG_Error("pstOTACheckInfo is NULL \n");
		return ERR_FAIL;
	}

	if (NULL == pstStepControl)
	{
		HxLOG_Error("pstStepControl is NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("pstOTACheckInfo :%p,  hSwUpdate : %08x]\n", pstOTACheckInfo, pstOTACheckInfo->hSwUpdate);
	
	return (!MGR_SWUP_CheckMainProcExist());
	#if 0

	pfnAppCb = (BUS_Callback_t)p1;

	//	IP 우선 이후에 RF Check시도.
	bReturn = MGR_SWUP_CheckProc(eSWUP_TYPE_Detect, (HINT32)eDxSWUPDATE_SOURCE_Ip, pfnAppCb);
	if (bReturn == TRUE)
	{
		SvcSwUpdate_DetectResult_t	stDetectResultInfo;

		HxSTD_MemSet(&stDetectResultInfo, 0x00, sizeof(SvcSwUpdate_DetectResult_t));

		MGR_SWUP_GetDetectInfo(pstOTACheckInfo->hSwUpdate, &stDetectResultInfo);

		if ((stDetectResultInfo.eSource == eDxSWUPDATE_SOURCE_Ip) && stDetectResultInfo.bFound && stDetectResultInfo.bAvailable)
		{
			HxLOG_Debug("Found OTA in IP...\n");
			return ERR_OK;
		}
	}

	bReturn = MGR_SWUP_CheckProc(eSWUP_TYPE_Detect, (HINT32)eDxSWUPDATE_SOURCE_Rf, pfnAppCb);
	if (bReturn == TRUE)
	{
		HxLOG_Debug("BUSMGR_Destroy : Detect RF Action.......\n");
		return ERR_OK;
	}

	return ERR_FAIL;
	#endif
}

// eMEVT_ACTION_SHUTDOWN_NEXTPROCESS : MgrAction_DoNextProcessCb_t
BUS_Result_t xmgr_action_BatchOtaCheckProcessDoProcess_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrActionBatch_OtaCheck_Info_t	*pstOTACheckInfo = (XmgrActionBatch_OtaCheck_Info_t*)pvContext;

	HxLOG_Debug("pstOTACheckInfo->hSwUpdate [%x]\n", pstOTACheckInfo->hSwUpdate);

	MGR_SWUP_InitState(pstOTACheckInfo->hSwUpdate, eDxSWUPDATE_SOURCE_Ip,0,0);
	MGR_SWUP_PostMessage(pstOTACheckInfo->hSwUpdate, eMEVT_SWUP_CONNECT_START, (Handle_t) 0 , 0, 0, 0);


	return ERR_BUS_NO_ERROR;
}

// eMEVT_ACTION_SHUTDOWN_DESTROY : MgrAction_BatchDestroyCb_t
HERROR xmgr_action_BatchOtaCheckProcessShutdownDestroy_MeSat(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Error("Nothing to do....\n");

	return ERR_OK;
}

// User Defined Message : MgrAction_MsgProcessCb_t
BUS_Result_t xmgr_action_BatchOtaCheckProcessMsg_MeSat(HINT32 nMsg, Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(nMsg)
	{
		case eMEVT_ACTION_NOTIFY_RESULT:
			{
				Mgr_Event_e eMgrEvent = p1;
				//BUS_Callback_t pfBatchProc = MGR_Action_BatchGetProc();

				HxLOG_Print("[%s][%04d] eMEVT_ACTION_NOTIFY_RESULT -- \n", __FUNCTION__, __LINE__);
				if (eMgrEvent == eMGR_EVENT_SWUP_DETECT_FINISH)
				{
					HxLOG_Print("eMGR_EVENT_SWUP_DETECT_FINISH \n");
					//eRes = xmgr_coldboot_DestroyOtaAction_MeSat(pstContents, hAct, p1, p2, p3);
				}

				//BUS_SendMessage(pfBatchProc, eMEVT_ACTION_SHUTDOWN_NEXTPROCESS, HANDLE_NULL, 0, 0, 0);
			}
			break;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}
#endif
/* End of File. ---------------------------------------------------------- */


