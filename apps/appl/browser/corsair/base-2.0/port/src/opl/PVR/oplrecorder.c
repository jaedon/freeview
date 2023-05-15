/**************************************************************************************/
/**
 * @file oplrecorder.c
 *
 * The record player
 *
**************************************************************************************
**/

#include "oplrecorder.h"

#include "hlib.h"
#include "apk.h"


/******************************************************************
	Interfaces
******************************************************************/
OplRecorder_t *OPL_Recorder_Create(void)
{
	return NULL;
}

void OPL_Recorder_Destroy(OplRecorder_t *hRecorder)
{
}

void OPL_Recorder_SetListener(OplRecorder_t *hRecorder, OplRecorderListener_t listener)
{
}

BPLStatus OPL_Recorder_GetRecordingState(OplRecorder_t *hRecorder, eOplRecorderState *eState)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_RecordNow (OplRecorder_t *hRecorder, OPL_Channel_t channel, unsigned int ulDurationSec, char *pszRecordID)
{
#if 0
	DxService_t			 stService;
	HERROR					 hErr;

	if (NULL != pszRecordID)
	{
		pszRecordID[0] = '\0';
	}

	HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
	stService.uid = nSvcUid;

	hErr = APK_MEDIACTRL_USRREC_StartSync (&stService, ulDurationSec, pszRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_StopRecording (OplRecorder_t *hRecorder, OPL_Channel_t channel)
{
#if 0
	DxService_t			 stService;
	HERROR					 hErr;

	HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
	stService.uid = nSvcUid;

	hErr = APK_MEDIACTRL_USRREC_Stop (&stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_Pause(OplRecorder_t *hRecorder)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_Resume(OplRecorder_t *hRecorder)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_Seek(OplRecorder_t *hRecorder, unsigned int ulOffsetSec, int reference)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetPlayState(OplRecorder_t *hRecorder, eOplPlaybackState *eState)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetPlaybackOffset(OplRecorder_t *hRecorder, unsigned int *pulOffset)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetMaxOffset(OplRecorder_t *hRecorder, unsigned int *pulMaxOffset)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetPlayPosition(OplRecorder_t *hRecorder, unsigned int *pulPosition)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetPlaySpeed(OplRecorder_t *hRecorder, unsigned int *pulSpeed)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_SetPlaySpeed(OplRecorder_t *hRecorder, unsigned int pulSpeed)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetPlaySpeedsMask(OplRecorder_t *hRecorder, unsigned int *pulSpeedMask)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_GetTimeShiftMode(OplRecorder_t *hRecorder, BPL_BOOL *pbTSR)
{
	return BPL_STATUS_OK;
}

BPLStatus OPL_Recorder_StopTimeShift(OplRecorder_t *hRecorder)
{
	return BPL_STATUS_OK;
}
