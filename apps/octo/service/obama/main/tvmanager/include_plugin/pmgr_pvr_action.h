#ifndef	___PMGR_PVR_ACTION_H___
#define	___PMGR_PVR_ACTION_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_pvr_action.h>

#define ___PVR_ACTION___
HERROR			pmgr_pvr_Create (void);
HERROR			pmgr_pvr_CheckPVRDeivceUsed(HUINT32	ulPvrIdx);
HERROR 			pmgr_pvr_StopPvrFunction (HUINT32 ulPvrIdx);
HERROR			pmgr_pvr_CheckPvrDevStatus(HBOOL *pbIsAvailable);
HERROR			pmgr_pvr_MakeThumbnail (HUINT32 ulContentId);
HERROR			pmgr_pvr_PrintPvrStatus (void);

#define ___PVR_PLAYBACK___
HERROR			pmgr_pvr_StartPlayback (HUINT32 ulContentId, HUINT32 ulStartTime);
HERROR			pmgr_pvr_StartTsrChasePlayback (HUINT32 ulActionId, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPos, HUINT32 ulOffset, MgrPb_Option_e ePbOption);
HBOOL			pmgr_pvr_GetTsrPlaybackEnable(void);
HBOOL			pmgr_pvr_GetChasePlaySurferEnable(void);

#define ___PVR_RECORDER___
HERROR 			pmgr_pvr_StartSectionGathering (HUINT32 ulContentId);
HERROR			pmgr_pvr_CheckRecordPossible (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason);
HERROR			pmgr_pvr_GetCurrentRecSvcHandles (HUINT32 *pulRecNum, Handle_t *phSvcArray);
MgrRec_EndReasonType_e		pmgr_pvr_StartRecording (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId);
HERROR			pmgr_pvr_GetTsrAutoTracking (HUINT32 ulViewId, HBOOL *pbTsrAutoTrack);
HERROR			pmgr_pvr_SetTsrAutoTracking (HUINT32 ulViewId, HBOOL bTsrAutoTrack);
HERROR			pmgr_pvr_GetTsrRecActionId (Handle_t hSvcHandle, HUINT32 *pulActionId);
HERROR			pmgr_pvr_StartLiveStreaming (Handle_t hSvcHandle, HUINT32 *pulActionId, Handle_t *phLiveStream);
HERROR			pmgr_pvr_StopLiveStreaming (Handle_t hLiveStream);
HERROR			pmgr_pvr_StartTsrRecording (HUINT32 ulRelatedViewId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, MgrPvr_TsrType_e eTsrType, HUINT32 ulStartTime);
HERROR			pmgr_pvr_StopTsrRecording (Handle_t hMasterSvc, Handle_t hSuppleSvc,MgrPvr_TsrType_e eTsrType);

#define ___PVR_TRANSCODER___
HERROR			pmgr_pvr_GetProfileFromJSON(const HCHAR *pProfileString, MgrTrcd_StreamProfile *pstProfile);
HERROR			pmgr_pvr_StartTrRecording (HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId);
HERROR			pmgr_pvr_StartPlaybackTranscodeToRingBuf (MgrPvr_PbTrcdToMemSetup_t *pstSetup, HUINT32 *pulActionId);
HERROR 			pmgr_pvr_StopPlaybackTranscodeToRingBuf (HUINT32 ulActionId);
HERROR 			pmgr_pvr_StartTrRecording (HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId);


#endif /* !___PMGR_PVR_ACTION_H___ */
