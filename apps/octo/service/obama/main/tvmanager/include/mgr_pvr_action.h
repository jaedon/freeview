#ifndef	___MGR_PVR_ACTION_H___
#define	___MGR_PVR_ACTION_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#include <mgr_common.h>

#include <mgr_recorder.h>
#include <mgr_playback.h>
#include <mgr_transcoder.h>

typedef enum
{
	eTsrType_None = 0,

	eTsrType_LiveTracing,
	eTsrType_Streaming,
	eTsrType_ExternalRequest,

	eTsrType_EndOfCase
} MgrPvr_TsrType_e;

typedef struct
{
	// Source: PVR ContentID
	HUINT32					 ulContentId;

	// Target: Ring Buffer
	Handle_t				 hRingBuf;

	// Options:
	HBOOL					 bTranscode;
	HBOOL					 bPcrTsPacketInsert;
	HUINT32					 ulStartTime;

	MgrTrcd_StreamProfile	 stProfile;

} MgrPvr_PbTrcdToMemSetup_t;


#define ___PVR_ACTION___
HERROR			MGR_PVR_Init (void);
HERROR			MGR_PVR_CheckHddEnable (void);
HERROR			MGR_PVR_CheckPVRDeivceUsed (HUINT32 ulPvrIdx);
HERROR 			MGR_PVR_StopPvrFunction (HUINT32 ulPvrIdx);
HERROR			MGR_PVR_MakeThumbnail (HUINT32 ulContentId);

#define ___PVR_PLAYBACK___
HERROR			MGR_PVR_StartPlayback (HUINT32 ulContentId, HUINT32 ulStartTime);
HERROR			MGR_PVR_StartTsrChasePlayback (HUINT32 ulActionId, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPos, HUINT32 ulOffset, MgrPb_Option_e ePbOption);
HBOOL			MGR_PVR_GetTsrPlaybackEnable(void);
HBOOL			MGR_PVR_GetChasePlaySurferEnable(void);

#define ___PVR_RECORDER___
HERROR 			MGR_PVR_StartSectionGathering (HUINT32 ulContentId);
HERROR			MGR_PVR_CheckRecordPossible (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason);
HERROR			MGR_PVR_GetCurrentRecSvcHandles (HUINT32 *pulRecNum, Handle_t *phSvcArray);
MgrRec_EndReasonType_e		MGR_PVR_StartRecording (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId);
HERROR			MGR_PVR_GetTsrAutoTracking (HUINT32 ulViewId, HBOOL *pbAutoTrack);
HERROR			MGR_PVR_SetTsrAutoTracking (HUINT32 ulViewId, HBOOL bbAutoTrack);
HERROR			MGR_PVR_GetTsrRecActionId (Handle_t hSvcHandle, HUINT32 *pulActionId);
HERROR			MGR_PVR_StartTsrRecording (HUINT32 ulRelatedViewId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, MgrPvr_TsrType_e eTsrType, HUINT32 ulStartTime);
HERROR			MGR_PVR_StopTsrRecording (Handle_t hMasterSvc, Handle_t hSuppleSvc, MgrPvr_TsrType_e eTsrType);
HERROR			MGR_PVR_StartLiveStreaming (Handle_t hSvcHandle, HUINT32 *pulActionId, Handle_t *phLiveStream);
HERROR			MGR_PVR_StopLiveStreaming (Handle_t hLiveStream);

#define ___PVR_TRANSCODER___
HERROR			MGR_PVR_GetProfileFromJSON(const HCHAR *pProfileString, MgrTrcd_StreamProfile *pstProfile);
HERROR			MGR_PVR_StartPlaybackTranscodeToRingBuf (MgrPvr_PbTrcdToMemSetup_t *pstSetup, HUINT32 *pulActionId);
HERROR 			MGR_PVR_StopPlaybackTranscodeToRingBuf (HUINT32 ulActionId);
HERROR 			MGR_PVR_StartTrRecording (HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId);

#endif /* !___MGR_PVR_ACTION_H___ */
