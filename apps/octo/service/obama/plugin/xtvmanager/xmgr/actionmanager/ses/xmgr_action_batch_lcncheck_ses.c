/****************************************************************************
*
* File Name   : manual_action_lcncheck.c
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
#include <mgr_common.h>
#include <mgr_search.h>
#include <mgr_svclist.h>
#include <svc_resources.h>
#include <mgr_rscmap.h>

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
	HUINT32						 ulActionId;
//	DxBuilder_State_e			 eState;
	BUS_Callback_t				 pfnSearchProc;
} XmgrActionBatch_LcnCheck_Info_t;

/*
typedef struct tagLcnCheckContext
{
	HBOOL					bLcnUpdated;
	HINT32					nLastSvcNum;
} _LcnCheckContext;
*/

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
STATIC BUS_Result_t local_manualaction_startLcnCheckProcess_Ses(HUINT32 ulActionId);
STATIC BUS_Callback_t local_manualaction_getSearchProc_Ses(void);


/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
#define _____Static_Function_____

STATIC BUS_Callback_t local_manualaction_getSearchProc_Ses(void)
{
	return MGR_SEARCH_GetLcnUpdateSearchAction();
}


#define _____Module_Function_____


// eMEVT_BUS_CREATE : MgrAction_InitCb_t
BUS_Result_t xmgr_action_BatchCheckLcnProcessInit_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void **ppvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrActionBatch_LcnCheck_Info_t 	*pstLCNCheckInfo;
	BUS_Callback_t	pfnSrchProc = NULL;

	if (NULL == pstStepControl)
	{
		HxLOG_Error("pstStepControl is NULL\n");
		return ERR_BUS_INITIAL_FAILED;
	}

	if (NULL == ppvContext)
	{
		HxLOG_Error("ppvContext is NULL\n");
		return ERR_BUS_INITIAL_FAILED;
	}

	pstLCNCheckInfo = HLIB_STD_MemAlloc(sizeof(XmgrActionBatch_LcnCheck_Info_t));
	if(pstLCNCheckInfo == NULL)
	{
		HxLOG_Error("pstLCNCheckInfo MemAlloc Error \n");
		return ERR_BUS_INITIAL_FAILED;
	}

	pfnSrchProc = local_manualaction_getSearchProc_Ses();
	if (pfnSrchProc == NULL)
	{
		if(pstLCNCheckInfo!= NULL)
			HLIB_STD_MemFree(pstLCNCheckInfo);

		HxLOG_Error("pfnSrchProc MemAlloc Error \n");
		return ERR_BUS_INITIAL_FAILED;
	}

	HxLOG_Debug("pfnSrchProc(0x%08x) eProcess(0x%08x) \n", pfnSrchProc, eProcess);
	pstLCNCheckInfo->pfnSearchProc = pfnSrchProc;

	*ppvContext = (void*)pstLCNCheckInfo;

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY : MgrAction_DestroyCb_t
HERROR xmgr_action_BatchCheckLcnProcessDestory_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (NULL == pstStepControl)
		return ERR_FAIL;

	return ERR_OK;
}

// eMEVT_BUS_MGR_KILLED : MgrAction_MgrKilledCb_t
HERROR xmgr_action_BatchCheckLcnProcessMgrKilled_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t	pfnAppCb;

	if (NULL == pstStepControl)
		return ERR_FAIL;

	pfnAppCb = (BUS_Callback_t)p1;
	if (pfnAppCb == local_manualaction_getSearchProc_Ses())
	{
	}

	return ERR_OK;
}

// eMEVT_ACTION_SHUTDOWN_NEXTPROCESS : MgrAction_DoNextProcessCb_t
BUS_Result_t xmgr_action_BatchCheckLcnProcessDoProcess_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrActionBatch_LcnCheck_Info_t	*pstLCNCheckInfo = (XmgrActionBatch_LcnCheck_Info_t*)pvContext;

	pstLCNCheckInfo->ulActionId = MGR_ACTION_GetMainActionId();

	if (pstLCNCheckInfo->pfnSearchProc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_OBJECT;
	}

	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstLCNCheckInfo->pfnSearchProc, pstLCNCheckInfo->ulActionId, 0, 0, 0) == ERR_OK)
	{
//		s_stLcnCheckContext->eState = eDxBUILDER_STATE_RUNNING;
		BUS_SendMessage(pstLCNCheckInfo->pfnSearchProc, eMEVT_SEARCH_ACTION_START, pstLCNCheckInfo->ulActionId, 0, 0, 0);
	}

	return ERR_BUS_NO_ERROR;
}



// MSG_MGR_MANUALACTION_DESTROY : MgrAction_BatchDestroyCb_t
HERROR xmgr_action_BatchCheckLcnProcessShutdownDestroy_Ses(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t	pfnSrchProc = local_manualaction_getSearchProc_Ses();

	if (BUS_MGR_Get(pfnSrchProc) != NULL)
	{
		BUS_SendMessage (pfnSrchProc, eMEVT_SEARCH_CANCEL, (Handle_t)NULL, 0, 0, 0);
		BUS_SendMessage (pfnSrchProc, eMEVT_SEARCH_SAVE_RESULT, (Handle_t)NULL, 0, 0, 0);
		BUS_MGR_Destroy (pfnSrchProc);
	}

	return ERR_OK;
}

// User Defined Message : MgrAction_MsgProcessCb_t
BUS_Result_t xmgr_action_BatchCheckLcnProcessMsg_Ses(HINT32 nMsg, Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0		// not used
	BUS_Result_t	eRes;
#endif

	switch(nMsg)
	{
	case eMEVT_BUS_TIMER:
#if 0
		if (p1 == SHUTDOWN_PROCESS_TIMER_ID)
		{
			BUS_KillTimer (SHUTDOWN_PROCESS_TIMER_ID);

			BUS_PostMessage (MGR_ShutDown_GetProc(), eMEVT_ACTION_SHUTDOWN_NEXTPROCESS, HANDLE_NULL, 0, 0, 0);

			return MESSAGE_BREAK;
		}
#endif
		break;
#if 0

	case eMEVT_SEARCH_FINISHED:
		BUS_PostMessage (local_manualaction_getSearchProc(MGR_ACTION_GetMainActionId()), eMEVT_SEARCH_SAVE_RESULT, (Handle_t)NULL, 0, 0, 0);
		return MESSAGE_BREAK;

	case eMEVT_SEARCH_SAVE_FINISHED:
		{
#if 0	// Compile Error
			_LcnCheckContext	*pstContext = (_LcnCheckContext *)pvContext;
			if (pvContext->nLastSvcNum != MGR_SVCLIST_CountSvcInDB(MGR_SVCLIST_GetSvcTypeForListCount()))
			{
				pvContext->bLcnUpdated = TRUE;
			}

			BUS_MGR_Destroy (local_manualaction_getSearchProc(MGR_ACTION_GetMainActionId()));
#endif
			return MESSAGE_BREAK;
		}
	#endif
	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}
#endif

/* End of File. ---------------------------------------------------------- */



