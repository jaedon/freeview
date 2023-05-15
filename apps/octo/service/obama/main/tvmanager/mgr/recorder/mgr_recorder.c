/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_recorder.h>
#include <pmgr_recorder.h>
#include <pmgr_model.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************** Static Function Prototypes  **********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HERROR MGR_RECORDER_Init(void)
{
#if defined(CONFIG_MW_MM_PVR)
		return pmgr_recorder_Init();
#else
		return ERR_FAIL;
#endif
}

HERROR MGR_RECORDER_GetRecSiSpec (SvcSi_RecSpec_t *pstSiSpec)
{
	return MGR_MODEL_RecordGetSiSpec (pstSiSpec);
}

HERROR MGR_RECORDER_GetRecCasTypes (HUINT32 *pulCasTypes)
{
	return MGR_MODEL_RecordGetCasTypes (pulCasTypes);
}

HERROR MGR_RECORDER_SetRecSetup(HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_SetRecSetup(nRecIdx, pstRecSetup);
#else
	return ERR_FAIL;
#endif
}

BUS_Callback_t MGR_RECORDER_GetProcFunc (HUINT32 ulActionId)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetProcFunc(ulActionId);
#else
	return NULL;
#endif
}

HCHAR* MGR_RECORDER_GetProcName(HUINT32 ulActionId)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetProcName(ulActionId);
#else
	return NULL;
#endif
}

HERROR MGR_RECORDER_FindTsrRecordingSvcInfo(Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_FindTsrRecordingSvcInfo(hSvc, pulActionId, pstInfo);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_RECORDER_FindRecordingSvcInfo(Handle_t hSvcHandle, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_FindRecordingSvcInfo(hSvcHandle, pulActionId, pstInfo);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_RECORDER_GetRecordingSvcInfo(HUINT32 ulActionId, MgrRec_RecInfo_t *pstInfo)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetRecordingSvcInfo(ulActionId, pstInfo);
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_RECORDER_GetRecordSvcHandleByContentId(HUINT32 ulContentId, Handle_t *phHandle)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetRecordSvcHandleByContentId(ulContentId, phHandle);
#else
	return ERR_FAIL;
#endif
}

HBOOL MGR_RECORDER_IsDuringDelayedRecCopy(Handle_t hAction)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_IsDuringDelayedRecCopy(hAction);
#else
	return ERR_FAIL;
#endif
}

HUINT8	*MGR_RECORDER_GetRecordPath(HUINT32 ulPvrIdx)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetRecordPath(ulPvrIdx);
#else
	return ERR_FAIL;
#endif
}

HBOOL MGR_RECORDER_IsTsrLockForDelayedRecord(HUINT32 ulContentId, HINT32 nEventId, HUINT16 nParental)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_IsTsrLockForDelayedRecord(ulContentId, nEventId, nParental);
#else
	return FALSE;
#endif
}

HBOOL	MGR_RECORDER_IsScrambleToDescrambleRecord(MgrRec_RecInfo_t *pstInfo)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_IsScrambleToDescrambleRecord(pstInfo);
#else
	return ERR_FAIL;
#endif
}

// Recording 이 시작하기 전에 error 가 발생한 것을 meta 파일로 만든다.
HERROR	MGR_RECORDER_CreateRecordFailReason(MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t	 *pstSchedule)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_CreateRecordFailReason(pstRecSetup, eEndReason, pstSchedule);
#else
	return ERR_FAIL;
#endif
}

// Recording 도중에 error 가 발생한 것을 meta file 에 update 한다.
HERROR	MGR_RECORDER_SetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e eEndReason)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_SetRecordFailReason(ulContentsId, eEndReason);
#else
	return ERR_FAIL;
#endif
}

HERROR	MGR_RECORDER_GetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e *peEndReason)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetRecordFailReason(ulContentsId, peEndReason);
#else
	return ERR_FAIL;
#endif
}

HUINT32	MGR_RECORDER_GetRecorderParam (MgrRec_ParamType_e eType)
{
	return pmgr_recorder_GetRecorderParam (eType);
}

HERROR MGR_RECORDER_GetMakeSectionSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
	return MGR_MODEL_MakesectionGetSiSpec (pstSiSpec);
}

HBOOL   MGR_RECORDER_IsDelayedRecCopyInProgress(void)
{
	return pmgr_recorder_IsDelayedRecCopyInProgress();
}

HERROR	MGR_RECORDER_SetRecordSetup(MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo, Handle_t hAction)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_SetRecordSetup(pstSetup, pstSvcRecSetup, pstMetaInfo,hAction);
#else
	return ERR_FAIL;
#endif
}

HBOOL	MGR_RECORDER_GetEncryptionMode(MgrRec_RecSetup_t *pstSetup)
{
#if defined(CONFIG_MW_MM_PVR)
	return pmgr_recorder_GetEncryptionMode(pstSetup);
#else
	return FALSE;
#endif
}


/* end of file */
