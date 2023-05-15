/****************************************************************************
*
* File Name   : mgr_action_batch.c
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

#include <hapi.h>

#include <db_param.h>

#include <svc_pipe.h>
#include <svc_sys.h>
#include <svc_output.h>
#include <svc_resources.h>

#include <bus.h>
#include <pmgr_action.h>
#include <mgr_common.h>
#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_frontcontrol.h>
/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */
#define	PROCTOSTR(eProc)		(\
								  ((eProc) == eDxBAT_PROC_EMMUPDATE) 	? DxNAMEOF(eDxBAT_PROC_EMMUPDATE)		\
								: ((eProc) == eDxBAT_PROC_LCNCHECK) 	? DxNAMEOF(eDxBAT_PROC_LCNCHECK)		\
								: ((eProc) == eDxBAT_PROC_OTACHECK) 		? DxNAMEOF(eDxBAT_PROC_OTACHECK)		\
								: ((eProc) == eDxBAT_PROC_OTADOWNLOAD) 		? DxNAMEOF(eDxBAT_PROC_OTADOWNLOAD)		\
								: ((eProc) == eDxBAT_PROC_EPGTVTV) 		? DxNAMEOF(eDxBAT_PROC_EPGTVTV)		\
								: ((eProc) == eDxBAT_PROC_TIMERECOVERY)		? DxNAMEOF(eDxBAT_PROC_TIMERECOVERY)		\
								: ((eProc) == eDxBAT_PROC_DESC_REC)		? DxNAMEOF(eDxBAT_PROC_DESC_REC)		\
								: ((eProc) == eDxBAT_PROC_MAX)		? DxNAMEOF(eDxBAT_PROC_MAX) 	\
								: NULL)


#define	BATCHTYPECTOSTR(eProc)		(\
								  ((eProc) == eDxBAT_TYPE_COLDBOOT) 	? DxNAMEOF(eDxBAT_TYPE_COLDBOOT)		\
								: ((eProc) == eDxBAT_TYPE_USER) 		? DxNAMEOF(eDxBAT_TYPE_USER)		\
								: ((eProc) == eDxBAT_TYPE_STANDBY) 	? DxNAMEOF(eDxBAT_TYPE_STANDBY)		\
								: NULL)

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
typedef	struct
{
	HBOOL		bComplete;
	HBOOL		bAllowKilledMsg;

	void		*pvUserData;
	DxBatchType_e 					eBatchType;
	MgrAction_StepControl_t			stStepControl;
	MgrAction_BatchModuleCb_t		stModuleCallback;

	HUINT32							ulNumProcess;
	MgrAction_BatchProcessItem_t	astProcessList[DxBATCH_PROC_MAX_COUNT];
} Mgr_Action_BatchContents_t;

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC Mgr_Action_BatchContents_t		s_stBatchContents;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
STATIC BUS_Result_t proc_action_Batch(HINT32 nMessage, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	mgr_action_BatchMsgDestroy(Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
BUS_Callback_t	MGR_Action_BatchGetProc(void)
{
	return proc_action_Batch;
}

STATIC INLINE Mgr_Action_BatchContents_t*	mgr_action_BatchGetContents(void)
{
	return &s_stBatchContents;
}

STATIC MgrAction_BatchProcessItem_t*	mgr_action_BatchFindProcess(HUINT32 eProcess)
{
	HINT32		n;
	Mgr_Action_BatchContents_t	*pstContents = mgr_action_BatchGetContents();
	MgrAction_BatchProcessItem_t		*pstProcess;

	pstProcess = NULL;
	for (n = 0; n < pstContents->ulNumProcess; n++)
	{
		if (eProcess == pstContents->astProcessList[n].eProcess)
		{
			pstProcess = &pstContents->astProcessList[n];
			break;
		}
	}

	return pstProcess;
}

STATIC BUS_Result_t	mgr_action_BatchGetNextProcess (Mgr_Action_BatchContents_t *pstContents, HUINT32 *peNextProcess)
{
	BUS_Result_t	eRes = ERR_FAIL;
	HUINT32			eShutdownProcess = eDxBAT_PROC_MAX;
	HUINT8			ucIndex = 0;

	if(peNextProcess == NULL)
	{
		HxLOG_Error("[local_shutdown_getNextProcess] param fail\n");
		goto EXIT_SHUTDOWN_FUNC;
	}

	/** get next process loop */

#if 1
	ucIndex = pstContents->stStepControl.ulCurrentProcIndex;
	ucIndex++;
	for(;ucIndex < pstContents->ulNumProcess; ucIndex++)
	{
		if(pstContents->stStepControl.ulProcessMask & pstContents->astProcessList[ucIndex].eProcess)
		{
			pstContents->stStepControl.ulCurrentProcIndex = ucIndex;
			eShutdownProcess = pstContents->astProcessList[ucIndex].eProcess;
			break;
		}
	}
#else
	for(ucIndex = 0; ucIndex < 32; ucIndex++)
	{
		if(pstContents->stStepControl.ulProcessMask & (1<<ucIndex))
		{
			eShutdownProcess = (1<<ucIndex);
			break;
		}
	}
#endif
	*peNextProcess = eShutdownProcess;
	eRes = ERR_BUS_NO_ERROR;

EXIT_SHUTDOWN_FUNC :

	return eRes;
}

STATIC HERROR mgr_action_BatchModuleInit(Handle_t hAction, Mgr_Action_BatchContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (NULL == pstContents->stModuleCallback.pfnInit)
	{
		return ERR_FAIL;
	}

	return pstContents->stModuleCallback.pfnInit(hAction, pstContents->pvUserData, p1, p2, p3);
}

// eMEVT_BUS_DESTROY
STATIC HERROR mgr_action_BatchModuleDestroy(Handle_t hAction, Mgr_Action_BatchContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	err;

	if (NULL == pstContents->stModuleCallback.pfnDestroy)
	{
		return ERR_FAIL;
	}

	err = pstContents->stModuleCallback.pfnDestroy(hAction, &pstContents->stStepControl, pstContents->pvUserData, p1, p2, p3);

	return err;
}

STATIC BUS_Result_t mgr_action_BatchMsgCreate(Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_MemSet(pstContents, 0, sizeof(Mgr_Action_BatchContents_t));

	pstContents->bComplete = FALSE;
	pstContents->bAllowKilledMsg = TRUE;
	pstContents->eBatchType = (DxBatchType_e)p1;

	// Module Callback 등록 : Init, Destroy
	// 각 Process 등록 : 각 Module에서 정의한 Process ...
	pmgr_action_BatchInit(pstContents->eBatchType);
	//	XMGR_INIT_ShutDown에서 AP_SHUTDOWN_Setup을 호출하고 AP_SHUTDOWN_SetUP에서 전체 Process수를 관리한다.
	HxLOG_Debug("Initialize Shutdown Process Num[%d] Flag[%x]\n", pstContents->ulNumProcess, pstContents->stStepControl.ulProcessMask);

	pstContents->stStepControl.eCurrentProcess = eDxBAT_PROC_NONE;
	if (pstContents->stStepControl.ulProcessMask == 0)
	{
		pstContents->bComplete = TRUE;
		BUS_MGR_Destroy(NULL);
		mgr_action_BatchMsgDestroy(pstContents, hAction, p1, p2, p3);

		return MESSAGE_BREAK;
	}

	mgr_action_BatchModuleInit(hAction, pstContents, p1, p2, p3);

	HxLOG_Print("%s eMEVT_ACTION_SHUTDOWN_NEXTPROCESS\n", __FUNCTION__);
	BUS_PostMessage(proc_action_Batch, eMEVT_ACTION_SHUTDOWN_NEXTPROCESS, HANDLE_NULL, 0, 0, 0);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	mgr_action_BatchMsgDestroy(Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_BatchProcessItem_t	*pstProcess;
	DxBatchType_e eBatchType = pstContents ? pstContents->eBatchType : eDxBAT_TYPE_MAX;
	DxBatchProc_b eBatchProc = pstContents ? pstContents->stStepControl.eCurrentProcess : eDxBAT_PROC_MAX;

	//pstProcess = mgr_action_BatchFindProcess(pstContents->stStepControl.eCurrentProcess);
	pstProcess = mgr_action_BatchFindProcess((HUINT32)eBatchProc);
	if (pstProcess)
	{
		pstProcess->stCbList.pfnDestroy(hAction, pstProcess->eProcess, &pstContents->stStepControl,
									pstProcess->pvContext, p1, p2, p3);
		pstContents->stStepControl.ulProcessMask &= ~(pstContents->stStepControl.eCurrentProcess);
	}
	mgr_action_BatchModuleDestroy(hAction, pstContents, p1, p2, p3);

	if (pstContents->bComplete == TRUE)
	{
		HxLOG_Critical("FINISH BATCH JOP : SUCCESS ==> TYPE[%s] PROC[%s] !!!!\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		BUS_SendMessage (NULL, eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS, hAction, eBatchType, eBatchProc, 0/*SUCCESS*/);
	}
	else
	{
		HxLOG_Critical("FINISH BATCH JOP : FAIL ==> TYPE[%s] PROC[%s] !!!!\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		BUS_SendMessage (NULL, eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS, hAction, eBatchType, eBatchProc, 1/*FAIL*/);
	}

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_MGR_KILLED
STATIC BUS_Result_t	mgr_action_BatchMsgMgrKilled (Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_BatchProcessItem_t	*pstProcess;

	HxLOG_Debug("MgrKilled [%s]\n", PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
	pstProcess = mgr_action_BatchFindProcess(pstContents->stStepControl.eCurrentProcess);
	if (NULL == pstProcess)
		return ERR_FAIL;

	if (pstContents->bAllowKilledMsg == TRUE)
	{
		HERROR	hErr;
		HxLOG_Debug("pvContents : [%x]\n", pstProcess->pvContext);
		hErr = pstProcess->stCbList.pfnMgrKilled(hAction, pstProcess->eProcess,
														&pstContents->stStepControl, pstProcess->pvContext,
														p1, p2, p3);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("%s pstProcess->stCbList.pfnMgrKilled result[%d]\n", __FUNCTION__, hErr);
			return ERR_OK;
		}
	} else
		return ERR_OK;

	HxLOG_Print("%s eMEVT_ACTION_SHUTDOWN_NEXTPROCESS\n", __FUNCTION__);

	BUS_PostMessage(NULL, eMEVT_ACTION_SHUTDOWN_NEXTPROCESS, HANDLE_NULL, 0, 0, 0);

	return MESSAGE_BREAK;
}

// eMEVT_ACTION_SHUTDOWN_NEXTPROCESS
STATIC BUS_Result_t	mgr_action_BatchMsgNextProcess (Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eRes = ERR_BUS_INITIAL_FAILED;
	HUINT32				eShutdownProcess = eDxBAT_PROC_MAX;

	MgrAction_BatchProcessItem_t	*pstProcess;

	HxLOG_Debug("Next Process.... current[%x]\n", PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
	if (pstContents->stStepControl.eCurrentProcess != eDxBAT_PROC_NONE)
	{
		pstProcess = mgr_action_BatchFindProcess(pstContents->stStepControl.eCurrentProcess);
		if (pstProcess)
		{
			HxLOG_Debug("pfnDestroy pvContext : [%x]\n", pstProcess->pvContext);
			pstProcess->stCbList.pfnDestroy(hAction, pstProcess->eProcess, &pstContents->stStepControl,
										pstProcess->pvContext, p1, p2, p3);
			pstContents->stStepControl.ulProcessMask &= ~(pstContents->stStepControl.eCurrentProcess);
		}
	}

	if (pstContents->stStepControl.ulProcessMask == 0)
	{
		HxLOG_Debug("ShutDown process is done...\n");

		pstContents->bComplete = TRUE;
		pstContents->stStepControl.eCurrentProcess = eDxBAT_PROC_NONE;
		BUS_MGR_Destroy(0);
		mgr_action_BatchMsgDestroy(pstContents, hAction, p1, p2, p3);
		goto EXIT_SHUTDOWN_FUNC;
	}

	eRes = mgr_action_BatchGetNextProcess(pstContents, &eShutdownProcess);
	if (eRes != ERR_BUS_NO_ERROR)
	{
		HxLOG_Critical("msgNextProcess is FAILED!!\n");
		goto EXIT_SHUTDOWN_FUNC;
	}
	else
	{
		eRes = ERR_BUS_INITIAL_FAILED;
		pstProcess = mgr_action_BatchFindProcess(eShutdownProcess);
		if (pstProcess)
		{

			pstContents->stStepControl.eCurrentProcess = eShutdownProcess;
			eRes = pstProcess->stCbList.pfnInit(hAction, eShutdownProcess, &pstContents->stStepControl,
										&pstProcess->pvContext, p1, p2, p3);
			eRes |= pstProcess->stCbList.pfnDoProcess(hAction, eShutdownProcess, &pstContents->stStepControl,
										pstProcess->pvContext, p1, p2, p3);
		}
	}

	if (eRes != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("%s eMEVT_ACTION_SHUTDOWN_NEXTPROCESS\n", __FUNCTION__);
		BUS_PostMessage(proc_action_Batch, eMEVT_ACTION_SHUTDOWN_NEXTPROCESS, HANDLE_NULL, 0, 0, 0);
	}

EXIT_SHUTDOWN_FUNC :

	return eRes;
}

// eMEVT_ACTION_SHUTDOWN_DESTROY, eMEVT_BUS_GO_OPERATION
STATIC BUS_Result_t mgr_action_BatchMsgShutdownDestroy (HINT32 nMsg, Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);

	BUS_MGR_Destroy (NULL);

	return mgr_action_BatchMsgDestroy(pstContents, hAction, p1, p2, p3);
}

// User Messages : Handler for process's Specific Message
STATIC BUS_Result_t mgr_action_BatchMsgUserMsg(HINT32 nMsg, Mgr_Action_BatchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eRes;
	MgrAction_BatchProcessItem_t	*pstProcess;

	HxLOG_Debug("eCurrentProcess [%s]\n", PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
	pstProcess = mgr_action_BatchFindProcess(pstContents->stStepControl.eCurrentProcess);
	if (NULL == pstProcess)
		return ERR_FAIL;

	eRes = pstProcess->stCbList.pfnProcessMsg(nMsg, hAction, pstProcess->eProcess,
										&pstContents->stStepControl,
										pstProcess->pvContext,
										p1, p2, p3);

	return eRes;
}


#define	_______GLOBAL_API_______
BUS_Result_t	proc_action_Batch(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Mgr_Action_BatchContents_t		*pstContents = mgr_action_BatchGetContents();
	BUS_Result_t							 eRes = ERR_FAIL;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("[proc_action_Batch] eMEVT_BUS_CREATE 		: BATCH TYPE[%s]\n", BATCHTYPECTOSTR(p1));
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "proc_action_Batch");
		eRes = mgr_action_BatchMsgCreate (pstContents, hAct, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("[proc_action_Batch] eMEVT_BUS_DESTROY \n");
		eRes = ERR_BUS_NO_ERROR;
		break;

	case eMEVT_ACTION_SHUTDOWN_NEXTPROCESS:
		HxLOG_Debug("[proc_action_Batch] MSG_ACTION_NEXTPROCESS	: BATCH TYPE[%s] PROC[%s]\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		eRes = mgr_action_BatchMsgNextProcess (pstContents, hAct, p1, p2, p3);
		break;

	case eMEVT_BUS_MGR_KILLED:
		HxLOG_Debug("[proc_action_Batch] eMEVT_BUS_MGR_KILLED 	: BATCH TYPE[%s] PROC[%s]\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		eRes = mgr_action_BatchMsgMgrKilled (pstContents, hAct, p1, p2, p3);
		break;

	case eMEVT_ACTION_SHUTDOWN_DESTROY:
		HxLOG_Debug("[proc_action_Batch] SHUTDOWN_DESTROY	 	: BATCH TYPE[%s] PROC[%s]\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		eRes = mgr_action_BatchMsgShutdownDestroy (message, pstContents, hAct, p1, p2, p3);
		break;

	case eMEVT_BUS_READY_SHUTDOWN :
	case eMEVT_BUS_GO_SHUTDOWN :
		HxLOG_Debug("[proc_action_Batch] eMEVT_BUS_GO_SHUTDOWN 	: BATCH TYPE[%s] PROC[%s]\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		MGR_ACTION_WaitShutDown(3000);
		eRes = MESSAGE_BREAK;
		break;
	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Debug("[proc_action_Batch] eMEVT_BUS_GO_OPERATION	: BATCH TYPE[%s] PROC[%s]\n", BATCHTYPECTOSTR(pstContents->eBatchType), PROCTOSTR(pstContents->stStepControl.eCurrentProcess));
		eRes = mgr_action_BatchMsgShutdownDestroy (message, pstContents, hAct, p1, p2, p3);
		break;

	default:
		HxLOG_Debug("[proc_action_Batch] default : message[%08x] [%08x] (%08x:%08x:%08x)\n",message, hAct, p1, p2, p3);
		eRes = mgr_action_BatchMsgUserMsg(message, pstContents, hAct, p1, p2, p3);
		break;
	}

	if (eRes == MESSAGE_BREAK)
	{
		return MESSAGE_BREAK;
	}

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_Action_BatchInstallProc(DxBatchType_e eBatchType, DxBatchProc_b * eBatchProc)
{
	return pmgr_action_BatchInstallProc(eBatchType, eBatchProc);
}

HERROR MGR_Action_BatchSetUp(void *pvUserData, MgrAction_BatchCB_t *pstCallback)
{
	HINT32	i;
	Mgr_Action_BatchContents_t *pstContents = mgr_action_BatchGetContents();

	if (NULL == pstCallback)
		return ERR_FAIL;

	if (NULL == pstCallback->astProcessList)
		return ERR_FAIL;

	if (0 == pstCallback->ulNumProcess || DxBATCH_PROC_MAX_COUNT < pstCallback->ulNumProcess)
		return ERR_FAIL;

	pstContents->pvUserData = pvUserData;

	HxSTD_memcpy(&pstContents->stModuleCallback, &pstCallback->stModuleCallback, sizeof(MgrAction_BatchModuleCb_t));

	pstContents->ulNumProcess = pstCallback->ulNumProcess;
	HxSTD_memset(pstContents->astProcessList, 0, sizeof(MgrAction_BatchProcessItem_t)*DxBATCH_PROC_MAX_COUNT);
	HxSTD_memcpy(pstContents->astProcessList, pstCallback->astProcessList, sizeof(MgrAction_BatchProcessItem_t)*pstCallback->ulNumProcess);

	pstContents->stStepControl.ulProcessMask = 0;
	pstContents->stStepControl.ulCurrentProcIndex = -1;
	for (i = 0; i < pstContents->ulNumProcess; i++)
	{
		pstContents->stStepControl.ulProcessMask |= pstContents->astProcessList[i].eProcess;
		HxLOG_Debug("[%s] 	: %d : PROC[%s]\n", __FUNCTION__, i, PROCTOSTR(pstContents->astProcessList[i].eProcess));
	}

	HxLOG_Debug("[%s]	: Batch Type[%s] PROC Count [%d]\n", __FUNCTION__, BATCHTYPECTOSTR(pstContents->eBatchType), pstContents->ulNumProcess);

	return ERR_OK;
}

HERROR	mgr_action_BatchStart (DxBatchType_e eBatchType)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_action_Batch", APP_ACTION_PRIORITY, proc_action_Batch, 0, eBatchType, 0, 0);

	return (ERR_BUS_NO_ERROR != eRes) ? ERR_FAIL : ERR_OK;
}
#endif

