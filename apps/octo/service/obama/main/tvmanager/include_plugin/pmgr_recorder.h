#ifndef	___PMGR_RECORDER_H___
#define	___PMGR_RECORDER_H___


#include <octo_common.h>
#include <mgr_playback.h>

HERROR 			pmgr_recorder_Init(void);
HERROR			pmgr_recorder_SetRecSetup(HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup);
BUS_Callback_t	pmgr_recorder_GetProcFunc (HUINT32 ulActionId);
HCHAR*			pmgr_recorder_GetProcName(HUINT32 ulActionId);
HERROR			pmgr_recorder_FindTsrRecordingSvcInfo(Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			pmgr_recorder_FindRecordingSvcInfo(Handle_t hSvcHandle, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			pmgr_recorder_GetRecordingSvcInfo(HUINT32 ulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			pmgr_recorder_GetRecordSvcHandleByContentId(HUINT32 ulContentId, Handle_t *phHandle);
HBOOL			pmgr_recorder_IsDuringDelayedRecCopy(Handle_t hAction);
HUINT8*			pmgr_recorder_GetRecordPath(HUINT32 ulPvrIdx);
HBOOL 			pmgr_recorder_IsTsrLockForDelayedRecord(HUINT32 ulContentId, HINT32	nEventId, HUINT16 nParental);
HBOOL			pmgr_recorder_IsScrambleToDescrambleRecord(MgrRec_RecInfo_t *pstInfo);
HERROR			pmgr_recorder_CreateRecordFailReason(MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule);
HERROR			pmgr_recorder_SetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e eEndReason);
HERROR			pmgr_recorder_GetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e *peEndReason);
HUINT32			pmgr_recorder_GetRecorderParam (MgrRec_ParamType_e eType);
HBOOL			pmgr_recorder_IsDelayedRecCopyInProgress(void);
HERROR			pmgr_recorder_SetRecordSetup(MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo,Handle_t hAction);
HBOOL			pmgr_recorder_GetEncryptionMode(MgrRec_RecSetup_t *pstSetup);


#endif /* ___PMGR_RECORDER_H___ */
