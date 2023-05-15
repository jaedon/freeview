/**
	@file     mgr_satrecord.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_satrecord.h>
#include <pmgr_satrecord.h>

#include <pmgr_model.h>

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

BUS_Callback_t		MGR_SATRECORD_GetDownloadAction(void)
{
	return pmgr_satrecord_GetDownloadAction();
}

HERROR MGR_SATRECORD_StartDetectPID (void)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32 		ulActionId = MGR_ACTION_GetMainActionId();
	Handle_t		hAction = SVC_PIPE_GetActionHandle (ulActionId);
	BUS_Callback_t	pfnDetectProc = NULL;

	pfnDetectProc	= pmgr_satrecord_GetDetectPIDAction();

	//create MSG Proc
	if(pfnDetectProc != NULL)
	{
		hErr = BUS_MGR_Create ( NULL, APP_ACTION_PRIORITY, pfnDetectProc, hAction, 0, 0, 0);

		if (hErr != ERR_OK)
		{
			HxLOG_Error("Error : Create Sat Record Proc\n");
		}
	}

	return hErr;
}

HERROR	MGR_SATRECORD_StartDownloadSatRec(void)
{
	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, MGR_SATRECORD_GetDownloadAction(), HANDLE_NULL, 0, 0, 0) == ERR_OK)
	{
		BUS_SendMessage(MGR_SATRECORD_GetDownloadAction(), eMEVT_SATRECORD_ACTION_START, HANDLE_NULL, 0, 0, 0);
	}

	return ERR_FAIL;
}

HERROR	MGR_SATRECORD_StopDownloadSatRec(BUS_DestroyReason_t eDestroyReason)
{
	if (BUS_MGR_Get(MGR_SATRECORD_GetDownloadAction()) != NULL)
	{
		BUS_MGR_DestroyWithReason(MGR_SATRECORD_GetDownloadAction(), eDestroyReason, 0);
	}
	return ERR_OK;
}

HERROR MGR_SATRECORD_GetSatRecSiSpec (SvcSi_SatRecordSpec_t *pstSiSpec)
{
	return MGR_MODEL_SatrecGetSiSpec (pstSiSpec);
}

HERROR MGR_SATRECORD_DestroyDetectPid(void)
{
	BUS_Callback_t	pfnDetectProc = NULL;

	pfnDetectProc	= pmgr_satrecord_GetDetectPIDAction();

	//create MSG Proc
	if(pfnDetectProc != NULL)
	{
		BUS_MGR_Destroy(pfnDetectProc);
	}

	return ERR_OK;
}

//For Batch
BUS_Callback_t MGR_SATRECORD_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType) // Batch 에서만 사용할 init 함수
{
	eBatchType;

	MGR_SATRECORD_StartDownloadSatRec();

	return MGR_SATRECORD_GetDownloadAction();
}

HERROR			MGR_SATRECORD_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType) // Batch 에서만 사용할 deinit 함수
{
	HERROR			hErr = ERR_FAIL;
	eBatchType;
	HxLOG_Trace();

	hErr = MGR_SATRECORD_StopDownloadSatRec(eDESTROY_BUS_MYSELF);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("stop lcn update fail because it's already destroyed.\n");
	}

	//set next time

	return hErr;
}

HERROR			MGR_SATRECORD_RescheduleBatch(DxBatchType_e eBatchType) // Batch 에서만 사용할 reschedule 함수
{
	eBatchType;
	return pmgr_satrecord_RescheduleBatch();
}


HERROR MGR_SATRECORD_GetBatchSatRecCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HxLOG_Trace();

	fnBatchProcCBList->fnAppInit = NULL;
	fnBatchProcCBList->fnAppDeinit = NULL;
	fnBatchProcCBList->fnAppReSchedule = MGR_SATRECORD_RescheduleBatch;

	return ERR_OK;
}

HERROR MGR_SATRECORD_GetBatchSatRecTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}
/* End of File. ---------------------------------------------------------- */
