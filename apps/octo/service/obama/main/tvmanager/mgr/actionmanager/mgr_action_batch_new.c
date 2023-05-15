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

enum
{
	 eMEVT_BATCH			 = eMEVT_BATCH_START,

	 /* - Description : 각 사양, 시나리오별 cold boot 처리 완료를 통보하는 메시지.
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_BATCH_RELEASE,
	eMEVT_BATCH_RESTART,

	 /* - Description : 각 사양, 시나리오별 cold boot next process 처리 하라는 메시지..
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_BATCH_KILLED
};



#define	PROCTOSTR(eProc)		(\
								  ((eProc) == eDxBAT_PROC_EMMUPDATE) 		? DxNAMEOF(eDxBAT_PROC_EMMUPDATE)		\
								: ((eProc) == eDxBAT_PROC_LCNCHECK) 		? DxNAMEOF(eDxBAT_PROC_LCNCHECK)		\
								: ((eProc) == eDxBAT_PROC_OTACHECK) 		? DxNAMEOF(eDxBAT_PROC_OTACHECK)		\
								: ((eProc) == eDxBAT_PROC_EPGTVTV) 			? DxNAMEOF(eDxBAT_PROC_EPGTVTV)			\
								: ((eProc) == eDxBAT_PROC_TIMERECOVERY)		? DxNAMEOF(eDxBAT_PROC_TIMERECOVERY)	\
								: ((eProc) == eDxBAT_PROC_DESC_REC)			? DxNAMEOF(eDxBAT_PROC_DESC_REC)		\
								: ((eProc) == eDxBAT_PROC_SATRECUPDATE) 	? DxNAMEOF(eDxBAT_PROC_SATRECUPDATE)	\
								: ((eProc) == eDxBAT_PROC_EPGGATHERING) 	? DxNAMEOF(eDxBAT_PROC_EPGGATHERING)	\
								: ((eProc) == eDxBAT_PROC_DYNAMICUPDATE) 	? DxNAMEOF(eDxBAT_PROC_DYNAMICUPDATE)	\
								: ((eProc) == eDxBAT_PROC_COLDBOOT) 		? DxNAMEOF(eDxBAT_PROC_COLDBOOT)		\
								: ((eProc) == eDxBAT_PROC_MAX)				? DxNAMEOF(eDxBAT_PROC_MAX)				\
								: NULL)


#define	BATCHTYPECTOSTR(eProc)		(\
								  ((eProc) == eDxBAT_TYPE_COLDBOOT) 	? DxNAMEOF(eDxBAT_TYPE_COLDBOOT)	\
								: ((eProc) == eDxBAT_TYPE_USER) 		? DxNAMEOF(eDxBAT_TYPE_USER)		\
								: ((eProc) == eDxBAT_TYPE_STANDBY) 		? DxNAMEOF(eDxBAT_TYPE_STANDBY)		\
								: NULL)

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

typedef enum
{
	eBATCH_START_First,
	eBATCH_START_Action,
	eBATCH_START_Timer,
} MgrActionBatch_StartType_e;

typedef enum
{
	eBATCH_STOP_Batch,
	eBATCH_STOP_Action,
	eBATCH_STOP_Timer,
	eBATCH_STOP_Normal
} MgrActionBatch_StoptType_e;

typedef enum
{
	eBATCH_RESCHEDULE_All,
	eBATCH_RESCHEDULE_Only
} MgrActionBatch_RescheduleType_e;

typedef enum
{
	eBATCH_STATUS_START,
	eBATCH_STATUS_STOP
} BatchStatus_e;

typedef	struct
{
	HBOOL							bLocked;
	HBOOL							bRelease;
	HBOOL							bSeriality;
	BatchStatus_e					eBatchStatus;
	DxBatchType_e					eBatchType;
	HUINT32							ulCreateCount;
	HxVector_t						* pstProcList;
	HxVector_t						* pstFinishedProcList;
} Mgr_Action_BatchContext_t;

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC Mgr_Action_BatchContext_t		* s_pstBatchContext = NULL;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
STATIC BUS_Result_t	mgr_action_BatchMsgDestroy(Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	proc_action_Batch(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);

#ifdef CONFIG_DEBUG
int mgr_action_Batch_PrintStatus(void);
#endif

STATIC void mgr_action_PrintBatchContext(Mgr_Action_BatchContext_t *pstBatchContext, const char *szFunc, const int nLine)
{
#ifdef CONFIG_DEBUG
	HxLOG_Warning(HxANSI_COLOR_YELLOW("--------------------------------\n"));
	HxLOG_Warning(HxANSI_COLOR_YELLOW("from <%s:%d>\n"), szFunc, nLine);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("--------------------------------\n"));
	HxLOG_Warning(HxANSI_COLOR_YELLOW("bLocked             = %d\n"), pstBatchContext->bLocked);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("bRelease            = %d\n"), pstBatchContext->bRelease);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("bSeriality          = %d\n"), pstBatchContext->bSeriality);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("eBatchStatus        = %d\n"), pstBatchContext->eBatchStatus);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("eBatchType          = %d\n"), pstBatchContext->bLocked);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("ulCreateCount       = %d\n"), pstBatchContext->ulCreateCount);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("pstProcList         = 0x%x\n"), pstBatchContext->pstProcList);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("pstFinishedProcList = 0x%x\n"), pstBatchContext->pstFinishedProcList);
	HxLOG_Warning(HxANSI_COLOR_YELLOW("--------------------------------\n\n"));
#endif
}

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
STATIC Mgr_Action_BatchContext_t*	mgr_action_GetBatchContext(void)
{
	return s_pstBatchContext;
}

STATIC Mgr_Action_BatchContext_t*	mgr_action_SetBatchContext(Mgr_Action_BatchContext_t* pstBatchContext)
{
	s_pstBatchContext = pstBatchContext;
	return s_pstBatchContext;
}

STATIC DxBatchType_e	mgr_action_GetBatchType(void)
{
	if(s_pstBatchContext)
		return s_pstBatchContext->eBatchType;
	else
		return eDxBAT_TYPE_NONE;
}



STATIC void mgr_action_RmProc(void *element)
{
	MgrAction_ProcInfo *pProc = (MgrAction_ProcInfo *)element;

	if(pProc)
	{
		// 각 proc의 user data는 각 proc이 알아서 free 해야함. 자기가 필요한 걸 fnAppInit에서 alloc하고 fnAppDeinit에서 free 해야함.
//		if(pProc->pvUserData) OxAP_Free(pProc->pvUserData);
		OxAP_Free(pProc);
	}
}

STATIC HBOOL mgr_action_StartProc (void *data, void *userdata)
{
	MgrAction_ProcInfo *pProc = (MgrAction_ProcInfo *)data;
	MgrActionBatch_StartType_e eStartType = (MgrActionBatch_StartType_e)userdata;
	HERROR ulRet=ERR_OK;

	eStartType;


	if(pProc)
	{
		UNIXTIME ulUnixTime = 0;
		ulRet=VK_CLOCK_GetTime((HULONG *)&ulUnixTime);
		if(ulRet != ERR_OK)
		{
			HxLOG_Error("VK_CLOCK_GetTime error!!\n");
		}
 		if(pProc->fnAppProc == NULL/*Not Running*/)
		{
			if(pProc->stAppCBList.fnAppInit)
			{
				if(pProc->ulStartTime == 0
					// There is no Start Time.
					|| (ulUnixTime >= pProc->ulStartTime && ulUnixTime < (pProc->ulStartTime + pProc->ulDuration))
					// Already past the proc start time, and Not past Duration.
					)
				{
					pProc->fnAppProc = pProc->stAppCBList.fnAppInit(&pProc->pvUserData, &pProc->ulUserDataLength, mgr_action_GetBatchType());
					if(pProc->fnAppProc)
					{
						pProc->ulCreateCount++;
					}

					HxLOG_Print("START Proc fnAppProc[%x] [%s] Create Count[%d] CurTime[%x] duration[%x]\n",pProc->fnAppProc, PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, ulUnixTime, pProc->ulDuration);

					if(pProc->fnAppProc && pProc->ulDuration)
					{
						// 꺼지는 시간.
						if(pProc->ulStartTime == 0)
						{
							BUS_SetTimer (pProc->eProcType, (pProc->ulDuration)*1000);
						}
						else
						{
							BUS_SetTimer (pProc->eProcType, ((pProc->ulStartTime + pProc->ulDuration) - ulUnixTime)*1000);
						}
						HxLOG_Print("Set Bus Timer to Stop Proc [%s] Create Count[%d] CurTime[%x] duration[%x]\n", PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, ulUnixTime, pProc->ulStartTime, pProc->ulDuration);
					}
				}
				else if(ulUnixTime < pProc->ulStartTime)
				{
					BUS_SetTimer (pProc->eProcType, (pProc->ulStartTime - ulUnixTime)*1000);
					HxLOG_Print("Set Bus Timer to Start Proc [%s] Create Count[%d] CurTime[%x] start[%x] duration[%x]\n", PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, ulUnixTime, pProc->ulStartTime, pProc->ulDuration);
					// Set Timer
				}
			}
			else
			{
				HxLOG_Error("No fnAppInit\n");
				return FALSE;
			}
		}
		else	/*Running*/
		{
			HxLOG_Print("Already runing this Proc [%s] Create Count[%d] CurTime[%x] duration[%x]\n", PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, ulUnixTime, pProc->ulStartTime, pProc->ulDuration);
		}
	}
	else
	{
		HxLOG_Error("ERROR Proc is NULL\n");
	}
	return FALSE;
}

STATIC HBOOL mgr_action_StopProc (void *data, void *userdata)
{
	MgrAction_ProcInfo *pProc = (MgrAction_ProcInfo *)data;
	MgrActionBatch_StoptType_e eStopType = (MgrActionBatch_StoptType_e)userdata;
	HERROR ulRet=ERR_OK;

	eStopType;

	if(pProc)
	{
		HUINT32 ulUnixTime = 0;
		ulRet=VK_CLOCK_GetTime((HULONG *)&ulUnixTime);
		if(ulRet != ERR_OK)
		{
			HxLOG_Error("VK_CLOCK_GetTime error!!\n");
		}

 		if(pProc->fnAppProc != NULL/*Running*/)
		{
			if(pProc->stAppCBList.fnAppDeinit) pProc->stAppCBList.fnAppDeinit(pProc->pvUserData, pProc->ulUserDataLength, mgr_action_GetBatchType());
			pProc->fnAppProc = NULL;
			BUS_KillTimer((HUINT32)pProc->eProcType);
			HxLOG_Print("STOP Proc [%s] Create Count[%d] CurTime[%x] duration[%x]\n", PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, ulUnixTime, pProc->ulStartTime, pProc->ulDuration);
		}
		else	/*Running*/
		{
			HxLOG_Error("Proc Is Not Running Proc [%s] Create Count[%d] CurTime[%x] duration[%x]\n", PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, ulUnixTime, pProc->ulStartTime, pProc->ulDuration);
		}
	}
	else
	{
		HxLOG_Error("ERROR is NULL\n");
	}
	return FALSE;
}

STATIC HBOOL mgr_action_ReScheduleProc (void *data, void *userdata)
{
	MgrAction_ProcInfo *pProc = (MgrAction_ProcInfo *)data;
	MgrActionBatch_RescheduleType_e eRescheduleType = (MgrActionBatch_RescheduleType_e)userdata;

	HxLOG_Trace();

	if(pProc)
	{
		if(pProc->stAppCBList.fnAppReSchedule)
		{
			if(eRescheduleType == eBATCH_RESCHEDULE_All)
				pProc->stAppCBList.fnAppReSchedule(mgr_action_GetBatchType());
			else
			{
				if(pProc->stAppCBList.fnAppInit == NULL)
				{
					pProc->stAppCBList.fnAppReSchedule(mgr_action_GetBatchType());
				}
			}
		}
		HxLOG_Print("Reschedule Proc [%s] Create Count[%d] CurTime[%x] duration[%x] Only Schedule[%s]\n", PROCTOSTR(pProc->eProcType), pProc->ulCreateCount, pProc->ulStartTime, pProc->ulDuration, pProc->stAppCBList.fnAppInit?"NO":"YES");
	}
	else
	{
		HxLOG_Error("ERROR Proc is NULL\n");
	}
	return FALSE;
}



STATIC HBOOL	mgr_action_FindProc(void *data, void *userdata)
{
	MgrAction_ProcInfo *pstProcInfo = (MgrAction_ProcInfo *)data;
	DxBatchProc_b eProcType = (DxBatchProc_b)userdata;

	if(pstProcInfo && pstProcInfo->eProcType == eProcType)
		return TRUE;
	else
		return FALSE;
}

STATIC HBOOL	mgr_action_FindProcByAddr (void *data, void *userdata)
{
	const MgrAction_ProcInfo *pstProcInfo = (MgrAction_ProcInfo *)data;
	BUS_Callback_t  fnAppProc = (BUS_Callback_t)userdata;

	if(pstProcInfo && pstProcInfo->fnAppProc == fnAppProc)
		return TRUE;
	else
		return FALSE;
}

STATIC HBOOL mgr_action_StartFirstProc (Mgr_Action_BatchContext_t* pstBatchContext)
{
	MgrAction_ProcInfo *pstProcInfo = pstBatchContext ? (MgrAction_ProcInfo *)HLIB_VECTOR_First(pstBatchContext->pstProcList): NULL;
	if(pstProcInfo)
	{
		mgr_action_StartProc(pstProcInfo, eBATCH_START_First);
		return TRUE;
	}
	else
	{
		HxLOG_Warning(" There is No Batch Proc. BatchType[%s] proc Vector[%x] Length[%d]\n", BATCHTYPECTOSTR(pstBatchContext->eBatchType), pstBatchContext->pstProcList, HLIB_VECTOR_Length(pstBatchContext->pstProcList));
		return FALSE;
	}
}


#define MAX_BATCH_COUNT 20
STATIC Mgr_Action_BatchContext_t*  mgr_action_InitContext(DxBatchType_e	eBatchType)
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();
	if(pstBatchContext && pstBatchContext->eBatchType != eDxBAT_TYPE_NONE)
	{
		return NULL;
	}
	else
	{
		if(!pstBatchContext)
		{
			pstBatchContext = (Mgr_Action_BatchContext_t *) OxAP_Calloc(sizeof(Mgr_Action_BatchContext_t));
			if(pstBatchContext) mgr_action_SetBatchContext(pstBatchContext);
			else return NULL;
		}
		pstBatchContext->bLocked = FALSE;
		pstBatchContext->bRelease= FALSE;

		pstBatchContext->bSeriality = pmgr_action_BatchIsSerial(eBatchType);
		HxLOG_Debug("BatchType [%s] proc's seriality is [%s].\n", BATCHTYPECTOSTR(eBatchType), (pstBatchContext->bSeriality == TRUE) ? "TRUE":"FALSE");

		pstBatchContext->eBatchType = eBatchType;
		if(pstBatchContext->pstProcList)
		{
			HLIB_VECTOR_RemoveAll(pstBatchContext->pstProcList);
		}

		pstBatchContext->pstProcList = HLIB_VECTOR_NewEasy (MAX_BATCH_COUNT, mgr_action_RmProc, NULL);

		if(pstBatchContext->pstFinishedProcList)
		{
			HLIB_VECTOR_RemoveAll(pstBatchContext->pstFinishedProcList);
		}

		pstBatchContext->pstFinishedProcList = HLIB_VECTOR_NewEasy ((HINT32)MAX_BATCH_COUNT, mgr_action_RmProc, NULL);
		pstBatchContext->ulCreateCount = 0;
		return pstBatchContext;
	}
}

STATIC void  mgr_action_DeinitContext(Mgr_Action_BatchContext_t* pstBatchContext)
{

	if(pstBatchContext)
	{
		if(pstBatchContext->pstProcList)
		{
			mgr_action_PrintBatchContext(pstBatchContext, __FUNCTION__, __LINE__);
			HxLOG_Print(HxANSI_COLOR_YELLOW("eBATCH_STOP_Batch, from <%s:%d>\n"), __FUNCTION__, __LINE__);

			HLIB_VECTOR_ForEach(pstBatchContext->pstProcList, mgr_action_StopProc, (void *)eBATCH_STOP_Batch);
			HLIB_VECTOR_RemoveAll(pstBatchContext->pstProcList);
			HLIB_VECTOR_Delete(pstBatchContext->pstProcList);
		}

		if(pstBatchContext->pstFinishedProcList)
		{
			HLIB_VECTOR_RemoveAll(pstBatchContext->pstFinishedProcList);
			HLIB_VECTOR_Delete(pstBatchContext->pstFinishedProcList);
		}

		OxAP_Free(pstBatchContext);
	}

	mgr_action_SetBatchContext(NULL);
}

#ifdef CONFIG_DEBUG
#define mgr_action_SetLocked(a)	_mgr_action_SetLocked(a, __FUNCTION__, __LINE__)
STATIC void _mgr_action_SetLocked(HBOOL bLocked, const char *szFunc, const int nLine)
#else
STATIC void mgr_action_SetLocked(HBOOL bLocked)
#endif
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();

#ifdef CONFIG_DEBUG
	HxLOG_Print(HxANSI_COLOR_YELLOW("bLocked = %d from <%s:%d>\n"), bLocked, szFunc, nLine);
#endif

	if(pstBatchContext)
	{
		pstBatchContext->bLocked = bLocked;
	}
}

STATIC HBOOL mgr_action_GetLocked(void)
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();
	if(pstBatchContext)
	{
		return pstBatchContext->bLocked;
	}
	return FALSE;
}

STATIC void mgr_action_SetRelease(HBOOL bRelease)
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();

	HxLOG_Print(HxANSI_COLOR_YELLOW("bRelease = %d\n"), bRelease);

	if(pstBatchContext)
	{
		pstBatchContext->bRelease = bRelease;
	}
}

STATIC void mgr_action_CheckRelease(void)
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();
	if(pstBatchContext && pstBatchContext->bRelease)
	{
		HxLOG_Print("eMEVT_BATCH_RELEASE Send message\n");
		BUS_PostMessage(proc_action_Batch,  eMEVT_BATCH_RELEASE, HANDLE_NULL,  0,  0,  0);
		pstBatchContext->bRelease = FALSE;
	}
}


STATIC HERROR mgr_action_AddBatchProc (DxBatchType_e	eBatchType, DxBatchProc_b eProcType, MgrAction_BatchProcCBList_t fnAppCBList,
	UNIXTIME ulStartTime, HUINT32 ulDuration, HBOOL bForced)
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();

	HxLOG_Print("Request BatchType[%s] ProcType[%s] CBList [%x] [%x] [%x] Time[%x] Dur[%x] bForced[%d]\n",
				BATCHTYPECTOSTR(eBatchType),PROCTOSTR(eProcType), fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule, ulStartTime, ulDuration, bForced);

	if(fnAppCBList.fnAppInit == NULL && fnAppCBList.fnAppDeinit == NULL && fnAppCBList.fnAppReSchedule == NULL)
		return ERR_FAIL;

	if(!pstBatchContext)
	{
		pstBatchContext = mgr_action_InitContext(eBatchType);
	}

	if(!pstBatchContext) return ERR_FAIL;

	if(pstBatchContext->eBatchType != eBatchType)
	//	If This function called at the First time, eBatchType is Same. So don't need to Deinit Context.
	{
		HxLOG_Print("Already other BatchType is Processing[%s] Request BatchType[%s] ProcType[%s]\n",
			BATCHTYPECTOSTR(pstBatchContext->eBatchType), BATCHTYPECTOSTR(eBatchType),PROCTOSTR(eProcType));

		return ERR_FAIL;
	}
	else	// Same BatchType . It is normal Mode.
	{
		MgrAction_ProcInfo *pstProcInfo = NULL;

		if(pstBatchContext->pstProcList)
		{
			pstProcInfo = HLIB_VECTOR_Find (pstBatchContext->pstProcList, mgr_action_FindProc, (void *)eProcType);
			if(pstProcInfo)
			{
				HxLOG_Print("Already Inserted This Proc. Request BatchType[%s] ProcType[%s]\n",
					BATCHTYPECTOSTR(eBatchType),PROCTOSTR(eProcType));

				if(bForced == FALSE || pstProcInfo->fnAppProc/*Running*/)
				{
					return ERR_FAIL;
				}
				else
				{
					HLIB_VECTOR_Detach(pstBatchContext->pstProcList, HLIB_VECTOR_IndexOf(pstBatchContext->pstProcList, pstProcInfo));
				}
			}
		}

		if(pstProcInfo == NULL && pstBatchContext->pstFinishedProcList)
		{
			pstProcInfo = HLIB_VECTOR_Find (pstBatchContext->pstFinishedProcList, mgr_action_FindProc, (void *)eProcType);

			if(pstProcInfo)
			{
				HxLOG_Print("Already Finished This Proc.\n Move This Proc From Finished List to Started list.\n Request BatchType[%s] ProcType[%s]\n",
					BATCHTYPECTOSTR(eBatchType),PROCTOSTR(eProcType));
				if(bForced == FALSE)
					return ERR_FAIL;
				else
					HLIB_VECTOR_Detach(pstBatchContext->pstFinishedProcList, HLIB_VECTOR_IndexOf(pstBatchContext->pstFinishedProcList, pstProcInfo));
			}
		}

		if(pstProcInfo == NULL) pstProcInfo = (MgrAction_ProcInfo *) OxAP_Calloc(sizeof(MgrAction_ProcInfo));
		else HxSTD_MemSet(pstProcInfo, 0x00, sizeof(*pstProcInfo));

		if(pstProcInfo)
		{
			pstProcInfo->eProcType = eProcType;
			pstProcInfo->stAppCBList.fnAppInit = fnAppCBList.fnAppInit;
			pstProcInfo->stAppCBList.fnAppDeinit = fnAppCBList.fnAppDeinit;
			pstProcInfo->stAppCBList.fnAppReSchedule = fnAppCBList.fnAppReSchedule;
			HxLOG_Print("CB List Init[%x] Deinit[%x] Reschedule[%x]\n" , fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule);
#if 0
			pstProcInfo->handle = handle;
			if(ulUserDataLength)
			{
				pstProcInfo->pvUserData = OxAP_MemDup(pvUserData, ulUserDataLength);
				pstProcInfo->ulUserDataLength = ulUserDataLength;
			}
#endif
			if(pstBatchContext->bSeriality)	// Serial mode does not need startTime;
				pstProcInfo->ulStartTime = 0;
			else
				pstProcInfo->ulStartTime = ulStartTime;
			pstProcInfo->ulDuration = ulDuration;

			pstProcInfo->bRetry = TRUE;	// default is true
			pstProcInfo->fnAppProc = NULL;
			pstProcInfo->ulCreateCount = 0;

			if(pstProcInfo->stAppCBList.fnAppInit != NULL)
			{
				if(HLIB_VECTOR_Add (pstBatchContext->pstProcList, pstProcInfo) == ERR_FAIL)
				{
					HxLOG_Print("Adding Proc FAIL FAIL\n");
				}
			}
			else
			{
				if(HLIB_VECTOR_Add (pstBatchContext->pstFinishedProcList, pstProcInfo) == ERR_FAIL)
				{
					HxLOG_Print("Adding Proc FAIL FAIL\n");
				}
			}

			HxLOG_Print("Adding Proc succeed. Request BatchType[%s] ProcType[%s] Proc Count[%d]\n",
				BATCHTYPECTOSTR(eBatchType),PROCTOSTR(eProcType), HLIB_VECTOR_Length(pstBatchContext->pstProcList));

			return ERR_OK;
		}
		else
		{
			HxLOG_Print("There is No Proc But Alloc Failed. Request BatchType[%s] ProcType[%s]\n",
				BATCHTYPECTOSTR(eBatchType),PROCTOSTR(eProcType));
			if(HLIB_VECTOR_Length(pstBatchContext->pstProcList) == 0
				&& HLIB_VECTOR_Length(pstBatchContext->pstFinishedProcList) == 0)
			// If Vetctor count is 0, pstBatchContext was built in this Function.
			{
				mgr_action_DeinitContext(pstBatchContext);
			}
			return ERR_FAIL;
		}
	}
}

#define	_______BUS_CONTROL_______



STATIC BUS_Result_t mgr_action_BatchMsgCreate(Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	mgr_action_SetLocked(TRUE);
	if(pstBatchContext && pstBatchContext->eBatchType != eDxBAT_TYPE_NONE)
	{
		pstBatchContext->eBatchStatus = eBATCH_STATUS_START;
		if( pstBatchContext->pstProcList && HLIB_VECTOR_Length(pstBatchContext->pstProcList) != 0)
		{
			if(pstBatchContext->bSeriality)	// Only start First Proc.
			{
				mgr_action_StartFirstProc (pstBatchContext);
			}
			else
				HLIB_VECTOR_ForEach (pstBatchContext->pstProcList, mgr_action_StartProc, eBATCH_START_First);
		}

		if(pstBatchContext->eBatchType == eDxBAT_TYPE_STANDBY
			&&  pstBatchContext->pstFinishedProcList
			&& HLIB_VECTOR_Length(pstBatchContext->pstFinishedProcList) != 0)
		{
			HLIB_VECTOR_ForEach(pstBatchContext->pstFinishedProcList, mgr_action_ReScheduleProc, (void *)eBATCH_RESCHEDULE_Only);
		}

		mgr_action_SetLocked(FALSE);
		return ERR_BUS_NO_ERROR;
	}
	else
	{
		HxLOG_Warning(" Initial Fail \n");
		if(pstBatchContext)
		{
			HxLOG_Warning(" Initial Fail BatchType[%s] proc Vector[%x] Length[%d]\n", BATCHTYPECTOSTR(pstBatchContext->eBatchType), pstBatchContext->pstProcList, HLIB_VECTOR_Length(pstBatchContext->pstProcList));
			mgr_action_BatchMsgDestroy(pstBatchContext, HANDLE_NULL, 0, 0, 0);
		}
		else
		{
			BUS_MGR_Destroy (NULL);
		}
		mgr_action_SetLocked(FALSE);
		return ERR_BUS_INITIAL_FAILED;
	}
}

STATIC BUS_Result_t mgr_action_BatchMsgTimer(Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_ProcInfo *pProc = HLIB_VECTOR_Find (pstBatchContext->pstProcList, mgr_action_FindProc, (void *)p1/*DxBatchProc_b*/);

	if(pstBatchContext->eBatchStatus == eBATCH_STATUS_STOP)
	{
		HxLOG_Print("Batch is Stopped. Nothing To do.\n");
		return ERR_BUS_NO_ERROR;
	}

	mgr_action_SetLocked(TRUE);

	if(pProc)
	{
		if(pProc->fnAppProc)	// Running
		{
			mgr_action_StopProc(pProc, (void *)eBATCH_START_Timer);
			HLIB_VECTOR_Detach(pstBatchContext->pstProcList, HLIB_VECTOR_IndexOf(pstBatchContext->pstProcList, pProc));
			HLIB_VECTOR_Add (pstBatchContext->pstFinishedProcList, pProc);
			if(pstBatchContext->eBatchType == eDxBAT_TYPE_STANDBY)
				mgr_action_ReScheduleProc(pProc, (void *)eBATCH_RESCHEDULE_All);
		}
		else					// Not Running
		{
			mgr_action_StartProc(pProc, (void *)eBATCH_START_Timer);
		}
	}
	else
	{
		HxLOG_Error("mgr_action_BatchMsgTimer Proc Is NULL. Proc [%s]\n", PROCTOSTR(p1));
	}
	mgr_action_SetLocked(FALSE);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_action_BatchMsgActRelease(Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if( !pstBatchContext || pstBatchContext->eBatchStatus == eBATCH_STATUS_STOP)
	{
		HxLOG_Print("Batch is Null or Stopped. Nothing To do.\n");
		return ERR_BUS_NO_ERROR;
	}

	mgr_action_SetLocked(TRUE);
	if( pstBatchContext->eBatchType != eDxBAT_TYPE_NONE && pstBatchContext->pstProcList && HLIB_VECTOR_Length(pstBatchContext->pstProcList) != 0)
	{
		HxLOG_Print("Remained Action Proc will be restarted.\n");
		HLIB_VECTOR_ForEach	(pstBatchContext->pstProcList, mgr_action_StartProc, (void *)eBATCH_START_Action);
	}
	else if(pstBatchContext->pstProcList == NULL || HLIB_VECTOR_Length(pstBatchContext->pstProcList) == 0)
	{
		HxLOG_Print("There is No Proclist. Batch Proc will be Destroyed.\n");
		mgr_action_BatchMsgDestroy(pstBatchContext, hAction, p1, p2, 0);
	}
	mgr_action_SetLocked(FALSE);
	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t mgr_action_BatchMsgActRestart(Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstBatchContext->eBatchStatus == eBATCH_STATUS_START)
	{
		HxLOG_Print("Batch is Stopped. Nothing To do.\n");
		return ERR_BUS_NO_ERROR;
	}

	pstBatchContext->eBatchStatus = eBATCH_STATUS_START;

	mgr_action_BatchMsgActRelease(pstBatchContext, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_MGR_KILLED
STATIC BUS_Result_t	mgr_action_BatchMsgMgrKilled (Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_ProcInfo *pProc = NULL;
	if(!pstBatchContext || pstBatchContext->eBatchStatus == eBATCH_STATUS_STOP)
	{
		HxLOG_Print("Batch is Null or Stopped. Nothing To do.\n");
		return ERR_BUS_NO_ERROR;
	}

	mgr_action_SetLocked(TRUE);

	pProc = HLIB_VECTOR_Find (pstBatchContext->pstProcList, mgr_action_FindProcByAddr, (void *)p1/*DxBatchProc_b*/);

	if(pProc)
	{
		HxLOG_Debug("PROC addr[%x] action reason[%d] proc reason[%d]\n",p1, p2, p3);
		HxLOG_Print("Batch Proc Killed proc[%s] \n", PROCTOSTR(pProc->eProcType));
		if(p2 == eDESTROY_BUS_MYSELF)
		{
			mgr_action_StopProc(pProc, (void *)eBATCH_STOP_Normal);
			HLIB_VECTOR_Detach(pstBatchContext->pstProcList, HLIB_VECTOR_IndexOf(pstBatchContext->pstProcList, pProc));
			HLIB_VECTOR_Add (pstBatchContext->pstFinishedProcList, pProc);
			if(pstBatchContext->eBatchType == eDxBAT_TYPE_STANDBY)
				mgr_action_ReScheduleProc(pProc, (void *)eBATCH_RESCHEDULE_All);
			BUS_PostMessage (NULL, eMEVT_ACTION_BATCH_RESULT, hAction, pstBatchContext->eBatchType, pProc->eProcType, p3/*Proc Reason*/);
			// If This proc will be destroyed by action, Don't need to send a result. Because Batch will restart this proc.
		}
		else if(p2 == eDESTROY_BUS_ACTION)
		{
			mgr_action_StopProc(pProc, (void *)eBATCH_START_Action);
		}
	}

	if(HLIB_VECTOR_Length(pstBatchContext->pstProcList) == 0)
	{
		HxLOG_Print("There is No Proclist. Batch Proc will be Destroyed.\n");
		mgr_action_BatchMsgDestroy(pstBatchContext, hAction, p1, p2, 0);
	}
	else
	{

		if(pstBatchContext->bSeriality)// Only start First Proc.
		{
			mgr_action_StartFirstProc (pstBatchContext);
		}
	}
	mgr_action_SetLocked(FALSE);
	return MESSAGE_BREAK;
}

// eMEVT_BUS_DESTROY
STATIC BUS_Result_t	mgr_action_BatchMsgDestroy(Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Param 3 is Result. 0 : Success 1 : Fail

	HxLOG_Print("BatchType[%s] Result[%s].\n", BATCHTYPECTOSTR(pstBatchContext->eBatchType), p3?"FAil":"Success");
	BUS_SendMessage (NULL, eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS, hAction, pstBatchContext->eBatchType, 0, p3/*FAIL*/);
	BUS_MGR_Destroy (NULL);
	return ERR_BUS_NO_ERROR;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

// eMEVT_ACTION_SHUTDOWN_DESTROY, eMEVT_BUS_GO_OPERATION
STATIC BUS_Result_t mgr_action_BatchMsgShutdownDestroy (HINT32 nMsg, Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);
	if(!pstBatchContext || pstBatchContext->eBatchStatus == eBATCH_STATUS_STOP)
	{
		HxLOG_Print("Batch is Null or Stopped. Nothing To do.\n");
		return ERR_BUS_NO_ERROR;
	}

	if(pstBatchContext->eBatchType == eDxBAT_TYPE_USER)
	{
		HxLOG_Print("Batch is Running in User Mode. Nothing To do.\n");
		return ERR_BUS_NO_ERROR;
	}

	mgr_action_SetLocked(TRUE);
	pstBatchContext->eBatchStatus = eBATCH_STATUS_STOP;
	if(pstBatchContext->eBatchType != eDxBAT_TYPE_NONE)
	{
		if( pstBatchContext->pstProcList && HLIB_VECTOR_Length(pstBatchContext->pstProcList) != 0)
		{
			HLIB_VECTOR_ForEach(pstBatchContext->pstProcList, mgr_action_StopProc, (void *)eBATCH_STOP_Normal);
		}
	}
	mgr_action_SetLocked(FALSE);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_action_BatchMsgGoShutdownDestroy (HINT32 nMsg, Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);
	return mgr_action_BatchMsgDestroy(pstBatchContext, hAction, p1, p2, p3);
}


STATIC BUS_Result_t mgr_action_BatchMsgGoOperationDestroy (HINT32 nMsg, Mgr_Action_BatchContext_t *pstBatchContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);
	return mgr_action_BatchMsgDestroy(pstBatchContext, hAction, p1, p2, p3);
}

#define	_______BUS_PROC_______

BUS_Result_t	proc_action_Batch(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Mgr_Action_BatchContext_t		*pstContext = mgr_action_GetBatchContext();
	BUS_Result_t							 eRes = ERR_FAIL;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BUS_CREATE] %s\n"), BATCHTYPECTOSTR(p1));
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		eRes = mgr_action_BatchMsgCreate (pstContext, hAct, p1, p2, p3);
		mgr_action_CheckRelease();
		if( pstContext->pstProcList && HLIB_VECTOR_Length(pstContext->pstProcList) == 0)
		{
			// If There in no proc in the Starting Proc list, Batch Will be Destroyed.
			mgr_action_BatchMsgGoOperationDestroy (message, pstContext, hAct, p1, p2, 0);
		}
		break;

	case eMEVT_BUS_TIMER :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BUS_TIMER] PROC[%s]\n"), PROCTOSTR(p1));
		eRes = mgr_action_BatchMsgTimer(pstContext, hAct, p1, p2, p3);
		mgr_action_CheckRelease();
		break;

	case eMEVT_ACTION_NOTIFY_RELEASED :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_ACTION_NOTIFY_RELEASED] mgr_action_GetLocked() = %d\n"), mgr_action_GetLocked());
		if(pstContext && pstContext->bSeriality)
		{	// Not Use Release msg in serial mode.
			break;
		}

		if(mgr_action_GetLocked())
			mgr_action_SetRelease(TRUE);
		else
			BUS_PostMessage(proc_action_Batch,  eMEVT_BATCH_RELEASE, hAct,  p1,  p2,  p3);
		break;

	case eMEVT_BUS_MGR_KILLED :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BUS_MGR_KILLED]\n"));
		BUS_PostMessage(proc_action_Batch,  eMEVT_BATCH_KILLED, hAct,  p1,  p2,  p3);
		break;

	case eMEVT_BATCH_RELEASE :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BATCH_RELEASE]\n"));
		if (pstContext == NULL)
		{
			break;
		}
		if(pstContext && pstContext->bSeriality)
		{	// Not Use Release msg in serial mode.
			break;
		}
		eRes = mgr_action_BatchMsgActRelease(pstContext, hAct, p1, p2, p3);
		mgr_action_CheckRelease();
		break;

	case eMEVT_BATCH_KILLED :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BATCH_KILLED] p1 = 0x%x, p2 = 0x%x, p3 = 0x%x\n"), p1, p2, p3);
		eRes = mgr_action_BatchMsgMgrKilled (pstContext, hAct, p1, p2, p3);
		mgr_action_CheckRelease();
		break;
	case eMEVT_BATCH_RESTART :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BATCH_RESTART] p1 = 0x%x, p2 = 0x%x, p3 = 0x%x\n"), p1, p2, p3);
		eRes = mgr_action_BatchMsgActRestart(pstContext, hAct, p1, p2, p3);
		mgr_action_CheckRelease();
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Debug("eMEVT_BUS_DESTROY\n");
		mgr_action_SetLocked(TRUE);
		mgr_action_DeinitContext(pstContext);
		eRes = ERR_BUS_NO_ERROR;
		break;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case eMEVT_ACTION_SHUTDOWN_DESTROY:
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_ACTION_SHUTDOWN_DESTROY] p1 = 0x%x, p2 = 0x%x, p3 = 0x%x\n"), p1, p2, p3);
		eRes = mgr_action_BatchMsgShutdownDestroy (message, pstContext, hAct, p1, p2, p3);
		//MGR_ACTION_ReinitBatch();
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BUS_GO_OPERATION] p1 = 0x%x, p2 = 0x%x, p3 = 0x%x\n"), p1, p2, p3);
		if(pstContext && pstContext->eBatchType == eDxBAT_TYPE_STANDBY)
		{	// Cold Boot is running in operation mode.
			eRes = mgr_action_BatchMsgGoOperationDestroy (message, pstContext, hAct, p1, p2, 1);
		}

		break;

	case eMEVT_BUS_READY_SHUTDOWN :
	case eMEVT_BUS_GO_SHUTDOWN :
		HxLOG_Print(HxANSI_COLOR_YELLOW("[eMEVT_BUS_READY_SHUTDOWN or eMEVT_BUS_GO_SHUTDOWN] message = 0x%x\n"), message);
		if(pstContext && pstContext->eBatchType != eDxBAT_TYPE_STANDBY)
		{
			eRes = mgr_action_BatchMsgGoShutdownDestroy (message, pstContext, hAct, p1, p2, 1);
		}
		break;

	default:
		//HxLOG_Debug("[proc_action_Batch] default : message[%08x] [%08x] (%08x:%08x:%08x)\n",message, hAct, p1, p2, p3);
		break;
	}

	if (eRes == MESSAGE_BREAK
		|| message == eMEVT_BATCH_RELEASE
		|| message == eMEVT_BATCH_KILLED
		|| message == eMEVT_BATCH_RESTART
		)
	{
		return MESSAGE_BREAK;
	}

	return ERR_BUS_NO_ERROR;
}



HERROR mgr_action_BatchInstallProc(DxBatchType_e eBatchType, DxBatchProc_b *eBatchProc)
{
	if(eBatchType >= eDxBAT_TYPE_MAX)
		return ERR_FAIL;

	HxLOG_Debug("[%s] eBatchType[%d] eBatchProc[%x]\n", __FUNCTION__,eBatchType, eBatchProc);

	while(*eBatchProc)
	{
		HUINT32 i = 0;
		HUINT32 ulProcCount = pmgr_action_BatchCount(eBatchType);
		MgrAction_BatchInitInfo * pstBatchInfo = pmgr_action_BatchGetProcInfo(eBatchType);

		for(i = 0; i< ulProcCount && pstBatchInfo; i++)
		{
			if(pstBatchInfo[i].eProcType == eDxBAT_PROC_NONE
				|| pstBatchInfo[i].eProcType == eDxBAT_PROC_MAX
				|| pstBatchInfo[i].fnBatchGetCBList == NULL
				|| pstBatchInfo[i].fnBatchGetTime == NULL
				)
			continue;

			if(*eBatchProc == pstBatchInfo[i].eProcType)
			{
				MgrAction_BatchProcCBList_t fnAppCBList;
				UNIXTIME				ulStartTime = 0;
				HUINT32 					ulDuration = 0;

				HxSTD_MemSet(&fnAppCBList, 0x00, sizeof(MgrAction_BatchProcCBList_t));
				HxLOG_Print("CBLIST Init[%x] Deinit[%x] Reschedule[%x]\n", fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule);
				pstBatchInfo[i].fnBatchGetCBList(&fnAppCBList);
				pstBatchInfo[i].fnBatchGetTime((UNIXTIME*)&ulStartTime, (HUINT32 *)&ulDuration);

				HxLOG_Print("CBLIST Init[%x] Deinit[%x] Reschedule[%x]\n", fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule);
				mgr_action_AddBatchProc (eBatchType, pstBatchInfo[i].eProcType, fnAppCBList, ulStartTime, ulDuration, TRUE);
			}
		}

		eBatchProc++;
	}

	return ERR_OK;
}


HERROR mgr_action_BatchAutoInstallProc(DxBatchType_e eBatchType)
{
	HUINT32 i = 0;
	MgrAction_BatchInitInfo * pstBatchInfo = pmgr_action_BatchGetProcInfo(eBatchType);
	HUINT32 ulProcCount = pmgr_action_BatchCount(eBatchType);

	HxLOG_Print("eBatchType[%d] ulProcCount[%d]\n", eBatchType, ulProcCount);

	for(i = 0; i< ulProcCount && pstBatchInfo; i++)
	{
		MgrAction_BatchProcCBList_t fnAppCBList;
		UNIXTIME 					ulStartTime = 0;
		HUINT32 					ulDuration = 0;

		if(pstBatchInfo[i].eProcType == eDxBAT_PROC_NONE
			|| pstBatchInfo[i].eProcType == eDxBAT_PROC_MAX
			|| pstBatchInfo[i].fnBatchGetCBList == NULL
			|| pstBatchInfo[i].fnBatchGetTime == NULL
			)
			continue;

		pstBatchInfo[i].fnBatchGetCBList(&fnAppCBList);
		pstBatchInfo[i].fnBatchGetTime((UNIXTIME*)&ulStartTime, (HUINT32 *)&ulDuration);

		HxLOG_Print("CBLIST Init[%x] Deinit[%x] Reschedule[%x]\n", fnAppCBList.fnAppInit, fnAppCBList.fnAppDeinit, fnAppCBList.fnAppReSchedule);
		mgr_action_AddBatchProc (eBatchType, pstBatchInfo[i].eProcType, fnAppCBList, ulStartTime, ulDuration, FALSE);
	}
	return ERR_OK;
}

#define	_______GLOBAL_API_FOR_INSTALL______
HERROR MGR_Action_BatchInstallProc(DxBatchType_e eBatchType, DxBatchProc_b * eBatchProc)
{
	return mgr_action_BatchInstallProc(eBatchType, eBatchProc);
}

HERROR MGR_ACTION_AddBatchProc (DxBatchType_e eBatchType, DxBatchProc_b eBatchProc, MgrAction_BatchProcCBList_t fnAppCBList,
	UNIXTIME ulStartTime, HUINT32 ulDuration, HBOOL bForced)
{
	return mgr_action_AddBatchProc(eBatchType, eBatchProc, fnAppCBList, ulStartTime, ulDuration, bForced);
}


HERROR MGR_ACTION_BatchInstallStandbyProc(void)
{
	return mgr_action_BatchAutoInstallProc(eDxBAT_TYPE_STANDBY);
}

HERROR MGR_ACTION_BatchInstallColdBootProc(void)
{
	return mgr_action_BatchAutoInstallProc(eDxBAT_TYPE_COLDBOOT);
}

HERROR MGR_ACTION_BatchInstallUserProc(void)
{
	return mgr_action_BatchAutoInstallProc(eDxBAT_TYPE_USER);
}


#define	_______GLOBAL_API_BATCH_CONTROL______
HERROR MGR_ACTION_InitContext(DxBatchType_e	eBatchType)
{
	if(mgr_action_InitContext(eBatchType))
		return ERR_OK;
	else
		return ERR_FAIL;
}

void * MGR_ACTION_GetBatchContext(void)
{
	return (void*)mgr_action_GetBatchContext();
}

HERROR	MGR_ACTION_BatchStart (DxBatchType_e eBatchType)
{
	BUS_Result_t	eRes = ERR_BUS_NO_ERROR;

	BUS_Callback_t callback =	BUS_MGR_GetMgrCallback(BUS_MGR_Get(MGR_ACTION_BatchGetProc()));

	if(callback)
	{
		BUS_SendMessage (callback, eMEVT_BATCH_RESTART, HANDLE_NULL, 0, 0, 0);
	}
	else
	{
		if(mgr_action_GetBatchContext())		eRes = BUS_MGR_Create("proc_action_Batch", APP_ACTION_PRIORITY, proc_action_Batch, 0, eBatchType, 0, 0);
		else return ERR_FAIL;
	}

	return (ERR_BUS_NO_ERROR != eRes) ? ERR_FAIL : ERR_OK;
}


HERROR	MGR_ACTION_BatchStop (DxBatchType_e	eBatchType)
{
	if(mgr_action_GetBatchType() == eBatchType)
	{
		Mgr_Action_BatchContext_t* pstContext = mgr_action_GetBatchContext();
		mgr_action_BatchMsgDestroy(pstContext, HANDLE_NULL, 0, 0, 1);
	}
	return ERR_OK;
}

HERROR	MGR_ACTION_BatchResume (void)
{
	BUS_Callback_t callback =	BUS_MGR_GetMgrCallback(BUS_MGR_Get(MGR_ACTION_BatchGetProc()));

	if(callback)
	{
		BUS_SendMessage (callback, eMEVT_BATCH_RESTART, HANDLE_NULL, 0, 0, 0);
	}
	return ERR_OK;
}

BUS_Callback_t	MGR_ACTION_BatchGetProc(void)
{
	return proc_action_Batch;
}

DxBatchType_e MGR_ACTION_GetBatchType(void)
{
	return mgr_action_GetBatchType();
}

#define	_______BATCH_CMD_FUNCTION______
#ifdef CONFIG_DEBUG
#define __STARTED_LIST__ 0
#define __FINISHED_LIST__ 1


STATIC HBOOL mgr_action_PrintProc (void *data, void *userdata)
{
	MgrAction_ProcInfo *pProc = (MgrAction_ProcInfo *)data;

	if(pProc)
	{
		HLIB_CMD_Printf("\t\t%s =>  [%s]  <=\n",(userdata == __STARTED_LIST__)?"Started List":"Finished List", PROCTOSTR(pProc->eProcType));
		HLIB_CMD_Printf("\t\t\t Create Count[%d] time[%x] dur[%x]\n", pProc->ulCreateCount, pProc->ulStartTime, pProc->ulDuration);
		if(userdata == __STARTED_LIST__) HLIB_CMD_Printf("\t\t\t Proc is %s.\n", pProc->fnAppProc?"Running":"Waiting");
		HLIB_CMD_Printf("\t\t\t Data[%x] length[%d]\n", pProc->pvUserData, pProc->ulUserDataLength);
		HLIB_CMD_Printf("\t\t\t CB Init[%x] Deinit[%x] Reschedule[%x]\n", pProc->stAppCBList.fnAppInit, pProc->stAppCBList.fnAppDeinit, pProc->stAppCBList.fnAppReSchedule);
	}
	else
	{
		HLIB_CMD_Printf("\t\tProc is NULL\n");
	}
	return FALSE;
}

int mgr_action_Batch_PrintStatus(void)
{
	Mgr_Action_BatchContext_t* pstBatchContext = mgr_action_GetBatchContext();

	HLIB_CMD_Printf("\n>> BATCH Status <<\n");
	HLIB_CMD_Printf("\tBatch Proc Is %s\n",BUS_MGR_GetMgrCallback(BUS_MGR_Get(MGR_ACTION_BatchGetProc()))?"Running.":"Not Running.");

	if(pstBatchContext)
	{
		HLIB_CMD_Printf("\tBatch Type is %s\n", BATCHTYPECTOSTR(pstBatchContext->eBatchType));
		if(pstBatchContext->pstProcList)
			HLIB_VECTOR_ForEach	(pstBatchContext->pstProcList, mgr_action_PrintProc, (void *)__STARTED_LIST__);
		else
			HLIB_CMD_Printf("\t\tNot Finished Proc list Is NULL. ERROR ERROR ERROR.\n");

		if(pstBatchContext->pstFinishedProcList)
			HLIB_VECTOR_ForEach	(pstBatchContext->pstFinishedProcList, mgr_action_PrintProc, (void *)__FINISHED_LIST__);
		else
			HLIB_CMD_Printf("\n\t\tFinished Proc list Is NULL. ERROR ERROR ERROR.\n");
	}
	else
	{
		HLIB_CMD_Printf("\tBatch ", HLIB_CMD_Printf("\tBatch Context is NULL\n"));
	}

	return 0;
}
#endif

void CMD_Register_mgr_Batch(void)
{
#define	hCmdHandle		"mgr_action"

#ifdef CONFIG_DEBUG
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)mgr_action_Batch_PrintStatus,
			"print_batch",	/* keyword */
			"status mgr action batch context informations", /*	 help  */
			"print_batch");	/*	usage  */
#endif
}



