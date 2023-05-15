/**************************************************************************************/
/**
 * @file oplrecorder.h
 *
 * The record player
 *
**************************************************************************************
**/

#ifndef __OPLRECORDER_H__
#define __OPLRECORDER_H__

#include "bpltype.h"
#include "oplchannel.h"

/******************************************************************
 	Define
******************************************************************/

/******************************************************************
 	Enum
******************************************************************/
typedef enum {
	/** user has not made a request to start presenting a channel */
	OPLRECORDER_PLAY_STATE_UNREALIZED        = 0,
	/** the receiver is connecting to the media source in order to begin playback */
	OPLRECORDER_PLAY_STATE_CONNECTING,
	/** the media is currently being presented to the user */
	OPLRECORDER_PLAY_STATE_PRESENTING,
	/** the terminal is not presenting media */
	OPLRECORDER_PLAY_STATE_STOPPED,
} eOplPlaybackState;

typedef enum {
	OPLRECORDER_REC_STATE_UNREALIZED         = 0,
	OPLRECORDER_REC_STATE_SCHEDULED,
	OPLRECORDER_REC_STATE_REC_PRESTART,
	OPLRECORDER_REC_STATE_REC_ACQUIRING_RESOURCES,
	OPLRECORDER_REC_STATE_REC_STARTED,
	OPLRECORDER_REC_STATE_REC_UPDATED,
	OPLRECORDER_REC_STATE_REC_COMPLETED,
	OPLRECORDER_REC_STATE_TS_ACQUIRING_RESOURCES,
	OPLRECORDER_REC_STATE_TS_STARTED,
	OPLRECORDER_REC_STATE_TSRBUFFER_COPYEND,
	OPLRECORDER_REC_STATE_ERROR
} eOplRecorderState;

typedef enum {
	OPLRECORDER_PLAY_SPEED_0               = 0x01,
	OPLRECORDER_PLAY_SPEED_F1              = 0x02,
	OPLRECORDER_PLAY_SPEED_F2              = 0x04,
	OPLRECORDER_PLAY_SPEED_F4              = 0x08,
	OPLRECORDER_PLAY_SPEED_F8              = 0x10,
	OPLRECORDER_PLAY_SPEED_F16             = 0x11,
	OPLRECORDER_PLAY_SPEED_F32             = 0x12,
	OPLRECORDER_PLAY_SPEED_R1              = 0x14,
	OPLRECORDER_PLAY_SPEED_R2              = 0x18,
	OPLRECORDER_PLAY_SPEED_R4              = 0x20,
	OPLRECORDER_PLAY_SPEED_R8              = 0x21,
	OPLRECORDER_PLAY_SPEED_R16             = 0x22,
	OPLRECORDER_PLAY_SPEED_R32             = 0x24,
} eOplRecorderPlaySpeeds;


/******************************************************************
 	Typedef
 ******************************************************************/
typedef void OplRecorder_t;

typedef void (*OplRecorderListener_t)(const void *hRecorder);

/******************************************************************
 	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

OplRecorder_t *OPL_Recorder_Create(void);

void OPL_Recorder_Destroy(OplRecorder_t *hRecorder);

void OPL_Recorder_SetListener(OplRecorder_t *hRecorder, OplRecorderListener_t listener);

BPLStatus OPL_Recorder_GetRecordingState(OplRecorder_t *hRecorder, eOplRecorderState *eState);

BPLStatus OPL_Recorder_RecordNow(OplRecorder_t *hRecorder, OPL_Channel_t channel, unsigned int ulDurationSec, char *pszRecordID);

BPLStatus OPL_Recorder_StopRecording(OplRecorder_t *hRecorder, OPL_Channel_t channel);

BPLStatus OPL_Recorder_Pause(OplRecorder_t *hRecorder);

BPLStatus OPL_Recorder_Resume(OplRecorder_t *hRecorder);

BPLStatus OPL_Recorder_Seek(OplRecorder_t *hRecorder, unsigned int ulOffsetSec, int reference);

BPLStatus OPL_Recorder_GetPlayState(OplRecorder_t *hRecorder, eOplPlaybackState *eState);

BPLStatus OPL_Recorder_GetPlaybackOffset(OplRecorder_t *hRecorder, unsigned int *pulOffset);

BPLStatus OPL_Recorder_GetMaxOffset(OplRecorder_t *hRecorder, unsigned int *pulMaxOffset);

BPLStatus OPL_Recorder_GetPlayPosition(OplRecorder_t *hRecorder, unsigned int *pulPosition);

BPLStatus OPL_Recorder_GetPlaySpeed(OplRecorder_t *hRecorder, unsigned int *pulSpeed);

BPLStatus OPL_Recorder_SetPlaySpeed(OplRecorder_t *hRecorder, unsigned int pulSpeed);

BPLStatus OPL_Recorder_GetPlaySpeedsMask(OplRecorder_t *hRecorder, unsigned int *pulSpeedMask);

BPLStatus OPL_Recorder_GetTimeShiftMode(OplRecorder_t *hRecorder, BPL_BOOL *pbTSR);

BPLStatus OPL_Recorder_StopTimeShift(OplRecorder_t *hRecorder);

#ifdef __cplusplus
};


/******************************************************************
 	class OplRecorder
******************************************************************/
class OplRecorder {
private:
	bool create()
	{
		m_recorderHandle = OPL_Recorder_Create();
		return m_recorderHandle != NULL;
	}

	void destroy()
	{ OPL_Recorder_Destroy(m_recorderHandle); }

public:
	OplRecorder()
	{
		create();
	}
	virtual ~OplRecorder()
	{
		destroy();
		m_recorderHandle = NULL;
	}

	void setListener(OplRecorderListener_t listener)
	{ return OPL_Recorder_SetListener(m_recorderHandle, listener); }

	bool getRecordingState(eOplRecorderState *eState)
	{ return OPL_Recorder_GetRecordingState(m_recorderHandle, eState) == BPL_STATUS_OK; }

	bool recordNow(OPL_Channel_t channel, unsigned int ulDurationSec, char *recordID)
	{
		return OPL_Recorder_RecordNow(m_recorderHandle, channel, ulDurationSec, recordID)\
				== BPL_STATUS_OK;
	}

	bool stopRecording(OPL_Channel_t channel)
	{ return OPL_Recorder_StopRecording(m_recorderHandle, channel) == BPL_STATUS_OK; }

	bool pause()
	{ return OPL_Recorder_Pause(m_recorderHandle) == BPL_STATUS_OK; }

	bool resume()
	{ return OPL_Recorder_Resume(m_recorderHandle) == BPL_STATUS_OK; }

	bool seek(unsigned int ulOffsetSec, int reference)
	{ return OPL_Recorder_Seek(m_recorderHandle, ulOffsetSec, reference) == BPL_STATUS_OK; }

	bool getPlayState(eOplPlaybackState *eState)
	{ return OPL_Recorder_GetPlayState(m_recorderHandle, eState) == BPL_STATUS_OK; }

	bool getPlaybackOffset(unsigned int *pulOffset)
	{ return OPL_Recorder_GetPlaybackOffset(m_recorderHandle, pulOffset) == BPL_STATUS_OK; }

	bool getMaxOffset(unsigned int *pulMaxOffset)
	{ return OPL_Recorder_GetMaxOffset(m_recorderHandle, pulMaxOffset) == BPL_STATUS_OK; }

	bool getPlayPosition(unsigned int *pulPosition)
	{ return OPL_Recorder_GetPlayPosition(m_recorderHandle, pulPosition) == BPL_STATUS_OK; }

	bool getPlaySpeed(unsigned int *pulSpeed)
	{ return OPL_Recorder_GetPlaySpeed(m_recorderHandle, pulSpeed) == BPL_STATUS_OK; }

	bool setPlaySpeed(unsigned int pulSpeed)
	{ return OPL_Recorder_SetPlaySpeed(m_recorderHandle, pulSpeed) == BPL_STATUS_OK; }

	bool getPlaySpeeds(unsigned int *pulSpeedMask)
	{ return OPL_Recorder_GetPlaySpeedsMask(m_recorderHandle, pulSpeedMask) == BPL_STATUS_OK; }

	bool getTimeShiftMode()
	{
		BPL_BOOL bTsr = FALSE;
		OPL_Recorder_GetTimeShiftMode(m_recorderHandle, &bTsr);
		return bTsr;
	}

	bool stopTimeShift()
	{ return OPL_Recorder_StopTimeShift(m_recorderHandle) == BPL_STATUS_OK; }

private:
	OplRecorder_t		*m_recorderHandle;
};


#endif

#endif
