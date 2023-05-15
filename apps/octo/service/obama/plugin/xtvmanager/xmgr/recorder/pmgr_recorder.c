#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>

#include <mgr_recorder.h>
#include <pmgr_recorder.h>

#include <_xmgr_recorder.h>

HERROR pmgr_recorder_Init()
{
#if defined (CONFIG_MW_CAS_CONAX)
	return xmgr_recorder_ConaxInit_Base ();
#else
	return xmgr_recorder_Init_Base ();
#endif
}

HERROR pmgr_recorder_SetRecSetup(HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup)
{
#if defined(CONFIG_OP_ERTELECOM)
	return xmgr_recorder_SetRecSetup_Ertelecom (nRecIdx, pstRecSetup);
#else
	return xmgr_recorder_SetRecSetup_Base (nRecIdx, pstRecSetup);
#endif
}

BUS_Callback_t pmgr_recorder_GetProcFunc (HUINT32 ulActionId)
{
#if defined(CONFIG_OP_SES)
	return xmgr_recorder_GetProcFunc_Ses (ulActionId);
#elif defined(CONFIG_OP_UK_DTT)
	return xmgr_recorder_GetProcFunc_Ukdtt(ulActionId);
#else
	return xmgr_recorder_GetProcFunc_Base (ulActionId);
#endif
}

HCHAR* pmgr_recorder_GetProcName(HUINT32 ulActionId)
{
#if defined(CONFIG_OP_UK_DTT)
	return xmgr_recorder_GetProcName_Ukdtt (ulActionId);
#else
	return xmgr_recorder_GetProcName_Base (ulActionId);
#endif
}

HERROR pmgr_recorder_FindTsrRecordingSvcInfo(Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
	return xmgr_recorder_FindTsrRecordingSvcInfo_Base (hSvc, pulActionId, pstInfo);
}

HERROR pmgr_recorder_FindRecordingSvcInfo(Handle_t hSvcHandle, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
	return xmgr_recorder_FindRecordingSvcInfo_Base (hSvcHandle, pulActionId, pstInfo);
}

HERROR pmgr_recorder_GetRecordingSvcInfo(HUINT32 ulActionId, MgrRec_RecInfo_t *pstInfo)
{
	return xmgr_recorder_GetRecordingSvcInfo_Base (ulActionId, pstInfo);
}

HERROR pmgr_recorder_GetRecordSvcHandleByContentId(HUINT32 ulContentId, Handle_t *phHandle)
{
	return xmgr_recorder_GetRecordSvcHandleByContentId_Base (ulContentId, phHandle);
}

HBOOL pmgr_recorder_IsDuringDelayedRecCopy(Handle_t hAction)
{
	return xmgr_recorder_IsDuringDelayedRecCopy_Base (hAction);
}

HUINT8 *pmgr_recorder_GetRecordPath(HUINT32 ulPvrIdx)
{
	return xmgr_recorder_GetRecordPath_Base (ulPvrIdx);
}

HBOOL pmgr_recorder_IsTsrLockForDelayedRecord(HUINT32 ulContentId, HINT32	nEventId, HUINT16 nParental)
{
	return xmgr_recorder_IsTsrLockForDelayedRecord_Base (ulContentId, nEventId, nParental);
}

HBOOL pmgr_recorder_IsScrambleToDescrambleRecord(MgrRec_RecInfo_t *pstInfo)
{
	return xmgr_recorder_IsScrambleToDescrambleRecord_Base (pstInfo);
}

HERROR pmgr_recorder_CreateRecordFailReason(MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule)
{
#if defined(CONFIG_OP_UK_DTT)
	return xmgr_recorder_CreateRecordFailReason_Ukdtt(pstRecSetup, eEndReason, pstSchedule);
#elif defined(CONFIG_OP_TDC)
	return xmgr_recorder_CreateRecordFailReason_Tdc(pstRecSetup, eEndReason, pstSchedule);
#else
	return xmgr_recorder_CreateRecordFailReason_Base (pstRecSetup, eEndReason, pstSchedule);
#endif
}

HERROR pmgr_recorder_SetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e eEndReason)
{
	return xmgr_recorder_SetRecordFailReason_Base (ulContentsId, eEndReason);
}

HERROR pmgr_recorder_GetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e *peEndReason)
{
	return xmgr_recorder_GetRecordFailReason_Base (ulContentsId, peEndReason);
}

HUINT32	pmgr_recorder_GetRecorderParam (MgrRec_ParamType_e eType)
{
	return xmgr_recorder_GetRecorderParam_Base (eType);
}

HBOOL pmgr_recorder_IsDelayedRecCopyInProgress(void)
{
	return xmgr_recorder_IsDelayedRecCopyInProgress_Base();
}

HERROR pmgr_recorder_SetRecordSetup(MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo,Handle_t hAction)
{
#if defined (CONFIG_MW_CAS_CONAX)
	return xmgr_recorder_ConaxSetRecordSetup_Base (pstSetup, pstSvcRecSetup, pstMetaInfo, hAction);
#else
	return xmgr_recorder_SetRecordSetup_Base (pstSetup, pstSvcRecSetup, pstMetaInfo, hAction);
#endif
}

HBOOL pmgr_recorder_GetEncryptionMode(MgrRec_RecSetup_t *pstSetup)
{
#if defined (CONFIG_IRDETO_HMX_PVR)
	return xmgr_recorder_IrdetoGetEncryptionMode_Base(pstSetup);
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HD9000I) || defined(CONFIG_PROD_FVP4000T) || defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_CDIG2000C)
	return TRUE;
#else
	return xmgr_recorder_GetEncryptionMode_Base (pstSetup);
#endif
}


