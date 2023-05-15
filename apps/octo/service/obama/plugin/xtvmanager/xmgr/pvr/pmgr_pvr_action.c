/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
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
#include <mgr_live.h>

#include <mgr_pvr_action.h>
#include <pmgr_pvr_action.h>

#include <_xmgr_pvr_action.h>
#include "local_include/_xmgr_pvr_action.h"

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
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/******************* Extern Function Prototypes  *******************/
/*******************************************************************/


/*******************************************************************/
/************************ Static Function *******************************/
/*******************************************************************/

#define ___PVR_ACTION___
HERROR pmgr_pvr_Create (void)
{
	return xmgr_pvr_Create_Base();
}

HERROR pmgr_pvr_CheckPVRDeivceUsed(HUINT32	ulPvrIdx)
{
	return xmgr_pvr_CheckPVRDeivceUsed_Base(ulPvrIdx);
}

// 해당 PvrIdx HDD에 대한 Rec/Pb 동작을 정지하는 API
HERROR pmgr_pvr_StopPvrFunction(HUINT32	ulPvrIdx)
{
	return xmgr_pvr_StopPvrFunction_Base(ulPvrIdx);
}

HERROR pmgr_pvr_CheckPvrDevStatus(HBOOL *pbIsAvailable)
{
	return xmgr_pvr_CheckPvrDevStatus_Base(pbIsAvailable);
}

HERROR pmgr_pvr_PrintPvrStatus (void)
{
#if defined(CONFIG_DEBUG)
	xmgr_pvr_PrintPvrStatus_Base();
	return ERR_OK;
#else
	return ERR_OK;
#endif
}

HERROR pmgr_pvr_MakeThumbnail (HUINT32 ulContentId)
{
	return xmgr_pvr_MakeThumbnail_Base (ulContentId);
}

#define ___PVR_PLAYBACK___
HERROR pmgr_pvr_StartTsrChasePlayback (HUINT32 ulActionId, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPos, HUINT32 ulOffset, MgrPb_Option_e ePbOption)
{
	return xmgr_pvr_StartTsrChasePlayback_Base (ulActionId, eTrick, ePbPos, ulOffset, ePbOption);
}

HERROR pmgr_pvr_StartPlayback (HUINT32 ulContentId, HUINT32 ulStartTime)
{
	return xmgr_pvr_StartPlayback_Base (ulContentId, ulStartTime);
}

#define ___PVR_RECORDER___
HERROR pmgr_pvr_GetCurrentRecSvcHandles (HUINT32 *pulRecNum, Handle_t *phSvcArray)
{
	return xmgr_pvr_GetCurrentRecSvcHandles_Base (pulRecNum, phSvcArray);
}

MgrRec_EndReasonType_e pmgr_pvr_StartRecording (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId)
{
	return xmgr_pvr_StartRecording_Base (pstRecSetup, pulActionId);
}

HERROR pmgr_pvr_GetTsrAutoTracking (HUINT32 ulViewId, HBOOL *pbTsrAutoTrack)
{
	return xmgr_pvr_GetTsrAutoTracking(ulViewId, pbTsrAutoTrack);
}

HERROR pmgr_pvr_SetTsrAutoTracking (HUINT32 ulViewId, HBOOL bTsrAutoTrack)
{
	return xmgr_pvr_SetTsrAutoTracking(ulViewId, bTsrAutoTrack);
}

HERROR pmgr_pvr_GetTsrRecActionId (Handle_t hSvcHandle, HUINT32 *pulActionId)
{
	return xmgr_pvr_GetTsrRecActionId_Base (hSvcHandle, pulActionId);
}

HERROR pmgr_pvr_StartLiveStreaming (Handle_t hSvcHandle, HUINT32 *pulActionId, Handle_t *phLiveStream)
{
	return xmgr_pvr_StartLiveStreaming_Base (hSvcHandle, pulActionId, phLiveStream);
}

HERROR pmgr_pvr_StopLiveStreaming (Handle_t hLiveStream)
{
	return xmgr_pvr_StopLiveStreaming_Base (hLiveStream);
}

HERROR pmgr_pvr_StartTsrRecording (HUINT32 ulRelatedViewId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, MgrPvr_TsrType_e eTsrType, HUINT32 ulStartTime)
{
	return xmgr_pvr_StartTsrRecording_Base(ulRelatedViewId, hMasterSvc, hSuppleSvc, eSessionType, eTsrType, ulStartTime);
}

HERROR pmgr_pvr_StopTsrRecording (Handle_t hMasterSvc, Handle_t hSuppleSvc,MgrPvr_TsrType_e eTsrType)
{
	return xmgr_pvr_StopTsrRecording_Base(hMasterSvc, hSuppleSvc, eTsrType);
}

HERROR pmgr_pvr_CheckRecordPossible (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason)
{
	if (pstRecSetup == NULL)			{ return ERR_FAIL; }

	return xmgr_pvr_CheckRecordPossible_Base (pstRecSetup, pulActionId, pulFailReason);
}

#define ___PVR_TRANSCODER___
HERROR	pmgr_pvr_GetProfileFromJSON(const HCHAR *pProfileString, MgrTrcd_StreamProfile *pstProfile)
{
	return xmgr_pvr_GetProfileFromJSON_Base (pProfileString, pstProfile);
}

HERROR pmgr_pvr_StartTrRecording (HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId)
{
	return xmgr_pvr_StartTrRecording_Base (ulContentId, ulRecMode, ulPvrIdx, pulActionId);
}

HERROR pmgr_pvr_StartPlaybackTranscodeToRingBuf (MgrPvr_PbTrcdToMemSetup_t *pstSetup, HUINT32 *pulActionId)
{
	return xmgr_pvr_StartPlaybackTranscodeToRingBuf_Base (pstSetup, pulActionId);
}

HERROR pmgr_pvr_StopPlaybackTranscodeToRingBuf (HUINT32 ulActionId)
{
	return xmgr_pvr_StopPlaybackTranscodeToRingBuf_Base (ulActionId);
}
