/**************************************************************************************/
/**
 * @file oplbroadcast.h
 *
 * Video Broadcast
 *
 **************************************************************************************
 **/

#ifndef __OPLBROADCAST_H__
#define __OPLBROADCAST_H__

#include "bpltype.h"
#include "oplchannel.h"
#include "oplprogramme.h"
#include "oplavoutput.h"
#include "oplavcomponent.h"
#include "oplrecorder.h"
#include "oplmail.h"
#include "oplmultiviewcomponent.h"
#include "oplvideobroadcasthandle_def.h"
#include "oplbookmark.h"

/******************************************************************
	Typedef
******************************************************************/

typedef void OplBroadcast_t;

/******************************************************************
	Interfaces
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_OP_UK_DTT)
typedef struct
{
	unsigned int sizeOf;				///< @see macro OPL_RctItem_InstanceOf()

	OPL_BOOL			isRctDisplay;
	int					linkType;
	int					schemeId;
	int					termId;
	int					groupId;
	int					precedence;

	char				*mediaUri;
	char				*promotionalText;
	char				*shorteventName;
	char				*shorteventText;
} OPL_RctItem_t;

#define	OPL_RctItem_InstanceOf(instance, type)	\
	(((OPL_RctItem_t *)(instance))->sizeOf == sizeof(type))
#endif

/**
 * Create the broadcast instance
 *
 * @param[in] hVbcastObj the handle of oplvideobroadcast object
 *
 * @return the handle of broadcast instance on success, and NULL on error
 */
OplBroadcast_t *OPL_Broadcast_Create(OPL_HANDLE hVbcastObj, int aResourceId);

/**
 * Destroy the broadcast instance.
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return none
 */
void OPL_Broadcast_Destroy(OplBroadcast_t *hBroadcast);

int OPL_Broadcast_GetResourceId(OplBroadcast_t *hBroadcast);

/**
 * Set broadcast live event listener
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] listener the callback function to receive the channel changed event.
 *
 * @return none
 */
void OPL_Broadcast_SetLiveEventListener (OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener);
void OPL_Broadcast_SetTsrEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener);


/**
 * Set cas message event listener
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] listener the callback function to receive the cas message event.
 *
 * @return none
 */
void OPL_Broadcast_SetCasEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener);


/**
 * Set record message event listener
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] listener the callback function to receive the cas message event.
 *
 * @return none
 */
void OPL_Broadcast_SetRecEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener);

/**
 * Set mail event listener
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] listener the callback function to receive the mail message event.
 *
 * @return none
 */
void OPL_Broadcast_SetMailEventListener( OplBroadcast_t *hBroadcast , OplBroadcastListener_t listener);


/**
 * Set bml event listener
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] listener the callback function to receive the mail message event.
 *
 * @return none
 */
void OPL_Broadcast_SetMhegEventListener( OplBroadcast_t *hBroadcast , OplBroadcastListener_t listener);

/**
 * Set mheg event listener
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] listener the callback function to receive the mheg message event.
 *
 * @return none
 */
void OPL_Broadcast_SetBmlEventListener( OplBroadcast_t *hBroadcast , OplBroadcastListener_t listener);

/**
 * Get the screen mode.
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[out] pbFullScreen the buffer for the screen mode.
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_GetFullScreen(OplBroadcast_t *hBroadcast, BPL_BOOL *pbFullScreen);

/**
 * Set the screen mode.
 * If bscreen is TRUE, the current screen mode will be full-screen mode.
 * If bscreen is FALSE, the current screen mode will be chagned to original mode.
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] bscreen the full-screen mode.
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetFullScreen(OplBroadcast_t *hBroadcast, BPL_BOOL bFullScreen);

/**
 * Request the channel to move to previous channel in channel list.
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_PrevChannel(OplBroadcast_t *hBroadcast);

/**
 * Request the channel to move to next channel in channel list.
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_NextChannel(OplBroadcast_t *hBroadcast);

/**
 * Bind the current channel :
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_BindToCurrentChannel(OplBroadcast_t *hBroadcast);
/**
 * Request the current channel to move to the channel matched to the triplet ID.
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] usServiceID the service ID of channel
 * @param[in] usTSID the transport ID
 * @param[in] usNetID the original network ID
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetChannelByTripleID(OplBroadcast_t *hBroadcast, unsigned short usServiceID, unsigned short usTSID, unsigned short usNetID) ;

/*
 * Set the channel using the specific channel instance
 *
 * @param[in] pChannel the specific channel instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetChannelHandle(OPL_Channel_t pChannel);

/*
 * Get the channel using the specific channel instance
 *
 * @param[in] pChannel the specific channel instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_GetChannelHandle(void **pChannel);



/*
 * Set the channel using the specific channel instance
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] pChannel the specific channel instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetChannelByChannelObject(OplBroadcast_t *hBroadcast, OPL_Channel_t pChannel);

/*
 * Set the FCC channel using the specific channel instance
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] pChannel the specific channel instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetChannelByChannelObjectFCC(OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulViewID);

/*
 * Set the visibility of this instance
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] bVisible the on/off value of broadcast instance visibility
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetVisibility(OplBroadcast_t *hBroadcast, HBOOL bVisible);

/*
 * Stop the FCC channel
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_StopByViewID(OplBroadcast_t *hBroadcast, unsigned int ulViewID);

/*
 * Stop the channel
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_Stop (OplBroadcast_t *hBroadcast);

/*
 * Get system volume
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[out] pulVolume the volume value
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_GetVolume(OplBroadcast_t *hBroadcast, unsigned int *pulVolume);

/*
 * Set system volume
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] pulVolume the volume value
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetVolume(OplBroadcast_t *hBroadcast, unsigned int ulVolume);

/*
 * Get system Mute
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[out] pbMute the global mute value
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_GetMute(OplBroadcast_t *hBroadcast, int *pnMute);

/*
 * Set system Mute
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] bMute the global value
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_SetMute(OplBroadcast_t *hBroadcast, int nMute);

/*
 * Set the window(screen) size
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] x the start position of the abscissa
 * @param[in] y the start position of the ordinate
 * @param[in] width the width of the screen
 * @param[in] height the height of the screen
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus OPL_Broadcast_GetWindowSize(OplBroadcast_t *hBroadcast, int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL *pbIsFullScreen);
BPLStatus OPL_Broadcast_SetVideoPosition(OplBroadcast_t *hBroadcast, int x, int y, int width, int height);
BPLStatus OPL_Broadcast_SetSupportScalerByVideoFrame(OplBroadcast_t *hBroadcast, BPL_BOOL enable);

/*
 * Get the current channel instance
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[out] ptChannel the current channel instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
OPL_Channel_t	 OPL_Broadcast_GetCurrentChannel(OplBroadcast_t *hBroadcast);
OPL_Channel_t	 OPL_Broadcast_GetLatestChannel(OplBroadcast_t *hBroadcast);

/*
 * Get the channel instance by major/minor channel
 *
 * @param[in] hBroadcast the handle of broadcast instance
 * @param[in] usMajorChannel the major channel number(lcn)
 * @param[in] usMinorChannel the minor channel number
 * @param[out] ptChannel the current channel instance
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
OPL_Channel_t OPL_Broadcast_CreateChannelObject(OplBroadcast_t *hBroadcast, unsigned short usMajorChannel, unsigned short usMinorChannel);
OPL_Channel_t OPL_Broadcast_CreateChannelObjectByTripleID(OplBroadcast_t *hBroadcast, int onid, int tsid, int sid);


BPLStatus OPL_Broadcast_GetAVOutputsCount(OplBroadcast_t *hBroadcast, int *pnCount);

OplAVOutput_t *OPL_Broadcast_GetAVOutputByIndex(OplBroadcast_t *hBroadcast, int nIndex);

void OPL_Broadcast_FreeAVOutput(OplBroadcast_t *hBroadcast, OplAVOutput_t *pOutput);

/*
 * Get the default record player
 *
 * @param[in] hBroadcast the handle of broadcast instance
 *
 * @return the recorder instance handle on success, and NULL on error.
 */
OplRecorder_t *OPL_Broadcast_GetRecorder(OplBroadcast_t *hBroadcast);

BPLStatus OPL_Broadcast_TvOn(OplBroadcast_t *hBroadcast);

BPLStatus OPL_Broadcast_TvOff(OplBroadcast_t *hBroadcast);

BPLStatus OPL_Broadcast_CountComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 *pulCompNum);

BPLStatus OPL_Broadcast_CountMutliViewComponent (OplBroadcast_t *hBroadcast, HUINT32 *pulCompNum);

BPLStatus OPL_Broadcast_GetComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent);

BPLStatus OPL_Broadcast_GetMultiViewComponent (OplBroadcast_t *hBroadcast, HINT32 index, OplMultiViewComponent_t *pstComponent);

BPLStatus OPL_Broadcast_GetCurrentComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, OplAVComponent_t *pstComponent);

BPLStatus OPL_Broadcast_SetCurrentComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang);

BPLStatus OPL_Broadcast_GetChangedComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType *eType);

BPLStatus OPL_Broadcast_offComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 ulP1);

const char* OPL_Broadcast_requestThumbnail(OplBroadcast_t *hBroadcast, const char* url, HUINT32 time);
BPLStatus OPL_Broadcast_ResultPincode (OplBroadcast_t *hBroadcast, HBOOL permit, int *retVal);
BPLStatus OPL_Broadcast_GetWidth (OplBroadcast_t *hBroadcast, HUINT32 *pulWidth);

BPLStatus OPL_Broadcast_GetHeight (OplBroadcast_t *hBroadcast, HUINT32 *pulHeight);

eOplPlaybackError OPL_Broadcast_GetErrorType (OplBroadcast_t *hBroadcast);
BPLStatus OPL_Broadcast_SetErrorType (OplBroadcast_t *hBroadcast, eOplPlaybackError type);


BPLStatus OPL_Broadcast_GetPlayState (OplBroadcast_t *hBroadcast, OplBroadcastPlayState_e *peState);

BPLStatus OPL_Broadcast_SetPlayState (OplBroadcast_t *hBroadcast, OplBroadcastPlayState_e peState);

BPLStatus OPL_Broadcast_GetPlaySpeed (OplBroadcast_t *hBroadcast, signed int *pnSpeed);

BPLStatus OPL_Broadcast_SetPlaySpeed (OplBroadcast_t *hBroadcast, signed int nSpeed);

BPLStatus OPL_Broadcast_GetTimeShiftMode (OplBroadcast_t *hBroadcast, unsigned int *pulMode);

BPLStatus OPL_Broadcast_SetTimeShiftMode (OplBroadcast_t *hBroadcast, unsigned int ulMode);

BPLStatus OPL_Broadcast_GetCurrentTimeShiftMode (OplBroadcast_t *hBroadcast, unsigned int *pulMode);

BPLStatus OPL_Broadcast_IsTimeshiftEnabled (OplBroadcast_t *hBroadcast, HBOOL *bEnabled);

BPLStatus OPL_Broadcast_GetPlaybackOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset);
BPLStatus OPL_Broadcast_GetPlayPosition (OplBroadcast_t *hBroadcast, unsigned int *pulPlayPosition);

BPLStatus OPL_Broadcast_GetMaxOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset);

BPLStatus OPL_Broadcast_GetStartOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset);

BPLStatus OPL_Broadcast_SetOffset (OplBroadcast_t *hBroadcast, int nOffset, unsigned int ulReference, int bStartPaused, int bUseLatestPbTime);

BPLStatus OPL_Broadcast_StopTimeshift (OplBroadcast_t *hBroadcast);

BPLStatus OPL_Broadcast_GetRecordingState (OplBroadcast_t *hBroadcast, eOplRecorderState *peState);

BPLStatus OPL_Broadcast_RecordNow (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulDurationSec, char *pszRecordID);
BPLStatus OPL_Broadcast_RecordNowAt (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulDurationSec, unsigned int ulTarget, char *pszRecordID);

BPLStatus OPL_Broadcast_RecordDelayed (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, char *pszRecordID);

BPLStatus OPL_Broadcast_RecordDelayedAt (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, unsigned int ulTarget, char *pszRecordID);

BPLStatus OPL_Broadcast_StopRecording (OplBroadcast_t *hBroadcast, OPL_Channel_t channel);

BPLStatus OPL_Broadcast_GetRecordId (OplBroadcast_t *hBroadcast, char *pszRecordID);

BPLStatus OPL_Broadcast_ControlLive (OplBroadcast_t *hBroadcast, unsigned int bControl);

BPLStatus OPL_Broadcast_GetCasMessage(OplBroadcast_t *hBroadcast, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession, char **ppszMessage,unsigned int *pulMessageLength);
BPLStatus OPL_Broadcast_GetCasUiEvtInfo(OplBroadcast_t *hBroadcast, unsigned int *pzEvt, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession);
BPLStatus OPL_Broadcast_ReleaseCasMessage(OplBroadcast_t *hBroadcast, char *pszMessage);

BPLStatus OPL_Broadcast_GetDrmError(OplBroadcast_t *hBroadcast, HUINT32 *pulDrmError, HCHAR **ppszCaSystemId);
BPLStatus OPL_Broadcast_ReleaseDrmError(OplBroadcast_t *hBroadcast, HCHAR *pszCaSystemId);
BPLStatus OPL_Broadcast_GetDrmRightError(OplBroadcast_t *hBroadcast, HUINT32 *pulErrorState, HCHAR** ppszContentId, HCHAR **ppszCaSystemId, HCHAR **ppszRightsIssuerURL);
BPLStatus OPL_Broadcast_ReleaseDrmRightError(OplBroadcast_t *hBroadcast, HCHAR *pszContentId, HCHAR *pszCaSystemId, HCHAR *pszRightsIssuerURL);
BPLStatus OPL_Broadcast_GetAutoDispMessage(OplBroadcast_t *hBroadcast, OPL_AutoDispMsg_t **phAutoDispMsg);
BPLStatus OPL_Broadcast_GetNewMailMsg(OplBroadcast_t *hBroadcast, OPL_MailHandle *hMail);
BPLStatus OPL_Broadcast_GetCaLinkMessage(OplBroadcast_t *hBroadcast, HUINT32 *pOnId, HUINT32 *pTsId, HUINT32 *pSvcId, HUINT8 **ppszCaLinkMsg);

BPLStatus OPL_Broadcast_GetProgrammes (OplBroadcast_t *hBroadcast, int *count, OPL_ProgrammeHandle **programmes);
BPLStatus OPL_Broadcast_CheckAitSection(OplBroadcast_t *hBroadcast);
BPLStatus OPL_Broadcast_CheckChannelChangable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL *pbChangable);
BPLStatus OPL_Broadcast_CheckChannelRecordable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL bIgnoreLive, HBOOL *pbChangable);
const char* OPL_Broadcast_GetPlayingThumbnail(OplBroadcast_t *hBroadcast, unsigned int pos);
BPLStatus OPL_Broadcast_GetPlaySpeeds(OplBroadcast_t *hBroadcast, int **speeds_out, int *count_out);
BPLStatus OPL_Broadcast_GetTrickRestrictMode(OplBroadcast_t * hBroadcast, OplBroadcastTrickRestrictMode_e *peMode);
BPLStatus OPL_Broadcast_GetStorageRight(OplBroadcast_t * hBroadcast, OplBroadcastStorageRight_e *peRight);
BPLStatus OPL_Broadcast_GetTsrPausedPoints(OplBroadcast_t *hBroadcast, HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks);
BPLStatus OPL_Broadcast_GetTsrPaused(OplBroadcast_t *hBroadcast, HBOOL *pbIsPaused);
BPLStatus OPL_Broadcast_GetMaxDuration (OplBroadcast_t *hBroadcast, unsigned int *pulMaxDuration);
BPLStatus OPL_Broadcast_GetEventRelayChannel (OplBroadcast_t *hBroadcast, OPL_Channel_t *eventRelayChannel);
BPLStatus OPL_Broadcast_GetEwsChannel (OplBroadcast_t *hBroadcast, OPL_Channel_t *ewsChannel);
BPLStatus OPL_Broadcast_GetEwsTuner (OplBroadcast_t *hBroadcast, HINT32 *tunerId);
BPLStatus OPL_Broadcast_GetBmlStartFlag (OplBroadcast_t *hBroadcast, HBOOL *pbAutoFlag);
BPLStatus OPL_Broadcast_GetBmlEndParam (OplBroadcast_t *hBroadcast, int *pnCause, char *pcParam);
BPLStatus OPL_Broadcast_ShowVideoMessage (OplBroadcast_t *hBroadcast, int type);
BPLStatus OPL_Broadcast_HideVideoMessage (OplBroadcast_t *hBroadcast, int type);
BPLStatus OPL_Broadcast_StartBML (OplBroadcast_t *hBroadcast);
BPLStatus OPL_Broadcast_EndBML (OplBroadcast_t *hBroadcast);
BPLStatus OPL_Broadcast_SetBMLKeyControlMode (OplBroadcast_t *hBroadcast, HBOOL permit, int *retVal);
BPLStatus OPL_Broadcast_GetParentalRatingChangeStatus (OplBroadcast_t *hBroadcast, char **ppszContentId, HBOOL *pbBlocked, unsigned int *pRating,  char **ppszSystemId);
BPLStatus OPL_Broadcast_ReleaseParentalRatingChangeStatus (OplBroadcast_t *hBroadcast, char *pszContentId, char *pszSystemId);
BPLStatus OPL_Broadcast_GetBmlHandle(OplBroadcast_t *hBroadcast, OPL_HANDLE *hBml);
BPLStatus OPL_Broadcast_GetBmlVodName (OplBroadcast_t *hBroadcast, char **pcName);
BPLStatus OPL_Broadcast_GetBmlVodParam (OplBroadcast_t *hBroadcast, char **pcParam);
BPLStatus OPL_Broadcast_GetChannelChangeError (OplBroadcast_t *hBroadcast, HINT32 *pnError);
BPLStatus OPL_Broadcast_GetRctInfoCount (OplBroadcast_t *hBroadcast, HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize);
BPLStatus OPL_Broadcast_GetRctInfo (OplBroadcast_t *hBroadcast, void **rctInfo);
BPLStatus OPL_Broadcast_ReleaseRctInfo(void *rctInfo);
BPLStatus OPL_Broadcast_GetRequestPincodeMsg (OplBroadcast_t *hBroadcast, const char **ppszMsg);
BPLStatus OPL_Broadcast_ReleaseRequestPincodeMsg (OplBroadcast_t *hBroadcast, const char *pszMsg);
BPLStatus OPL_Broadcast_GetNcdChangedMsg (OplBroadcast_t *hBroadcast, const char **ppszMsg,int *pStartTime,int *pDuration);
BPLStatus OPL_Broadcast_ReleaseNcdChangedMsg (OplBroadcast_t *hBroadcast, const char *pszMsg);
BPLStatus OPL_Broadcast_GetNetworkUri (OplBroadcast_t *hBroadcast, const char **ppszUri);
BPLStatus OPL_Broadcast_ReleaseNetworkUri (OplBroadcast_t *hBroadcast, const char *pszUri);
BPLStatus OPL_Broadcast_GetTuneInfoByMheg (OplBroadcast_t *hBroadcast, HUINT32 *ulMhegSvcUid, HUINT32 *ulTuneMode, void **hMhegChannel );
BPLStatus OPL_Broadcast_GetMhegInterruptFlag(OplBroadcast_t *hBroadcast, HUINT32 *bInterruptibleFlag );
BPLStatus OPL_Broadcast_GetMhegIcsFlag(OplBroadcast_t *hBroadcast, HUINT32 *ulMhegIcsStatus);
BPLStatus OPL_Broadcast_SetChannelByUIDWithTuneInfo(OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int tuneInfo) ;
BPLStatus OPL_Broadcast_HasSDTeletext();
BPLStatus OPL_Broadcast_PauseHbbtvMonitoring(OplBroadcast_t *hBroadcast);


#ifdef __cplusplus
};

/******************************************************************
	class OplVideoBroadcast
******************************************************************/
class OplVideoBroadcast {
private:
	bool create(void* aVideoHandle, int aResourceId)
	{
		OPL_HANDLE handle = aVideoHandle ? aVideoHandle: this;
		m_vbcastHandle = OPL_Broadcast_Create(handle, aResourceId);
		return m_vbcastHandle != NULL;
	}

	void destroy()
	{ OPL_Broadcast_Destroy(m_vbcastHandle); }

public:
    OplVideoBroadcast(void* aVideoHandle = NULL, int aResourceId = 0)
    {
    	create(aVideoHandle, aResourceId);
    	m_recorder = NULL;
    	m_videoHandle = aVideoHandle;
    }

    virtual ~OplVideoBroadcast()
    {
    	if ( m_recorder ) delete m_recorder;
    	m_recorder = NULL;

    	destroy();
    	m_vbcastHandle = NULL;
    }

    bool getResourceId()
    { return OPL_Broadcast_GetResourceId(m_vbcastHandle); }

    void setLiveEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetLiveEventListener(m_vbcastHandle, listener); }

    void setTsrEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetTsrEventListener(m_vbcastHandle, listener); }

    void setCasEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetCasEventListener(m_vbcastHandle, listener); }

    void setRecEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetRecEventListener(m_vbcastHandle, listener); }

    void setMailEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetMailEventListener(m_vbcastHandle, listener); }

    void setBmlEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetBmlEventListener(m_vbcastHandle, listener); }

    void setMhegEventListener(OplBroadcastListener_t listener)
    { OPL_Broadcast_SetMhegEventListener(m_vbcastHandle, listener); }

    bool getCasMessage(unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession, char **ppszMessage, unsigned int *pulMessageLength)
    { return OPL_Broadcast_GetCasMessage(m_vbcastHandle, pzModule, pzSlot, pulSession, ppszMessage, pulMessageLength) == BPL_STATUS_OK; }

    bool getCasUiEvtInfo(unsigned int *pzEvt, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession)
    { return OPL_Broadcast_GetCasUiEvtInfo(m_vbcastHandle, pzEvt, pzModule, pzSlot, pulSession) == BPL_STATUS_OK; }

    bool releaseCasMessage(char *pszMessage)
    { return OPL_Broadcast_ReleaseCasMessage(m_vbcastHandle, pszMessage) == BPL_STATUS_OK; }

    bool getDrmError(unsigned int *pulDrmError, char **ppszCaSystemId)
    { return OPL_Broadcast_GetDrmError(m_vbcastHandle, pulDrmError, ppszCaSystemId) == BPL_STATUS_OK; }

    bool releaseDrmError(char *pszCaSystemId)
    { return OPL_Broadcast_ReleaseDrmError(m_vbcastHandle, pszCaSystemId) == BPL_STATUS_OK; }

    bool getDrmRightError(unsigned int *pulErrorState, char ** ppszContentId, char **ppszCaSystemId, char **ppszRightsIssuerURL)
    { return OPL_Broadcast_GetDrmRightError(m_vbcastHandle, pulErrorState, ppszContentId, ppszCaSystemId, ppszRightsIssuerURL) == BPL_STATUS_OK; }

    bool releaseDrmRightError(char *pszContentId, char *pszCaSystemId, char *pszRightsIssuerURL)
    { return OPL_Broadcast_ReleaseDrmRightError(m_vbcastHandle, pszContentId, pszCaSystemId, pszRightsIssuerURL) == BPL_STATUS_OK; }

    bool getAutoDispMsg(OPL_AutoDispMsg_t **pphAutoDispMsg)
    { return OPL_Broadcast_GetAutoDispMessage(m_vbcastHandle, pphAutoDispMsg) == BPL_STATUS_OK;}

    bool getNewMailMsg(OPL_MailHandle *hMail)
    { return OPL_Broadcast_GetNewMailMsg(m_vbcastHandle, hMail) == BPL_STATUS_OK;}

    bool getCaLinkMsg(HUINT32 *pOnId, HUINT32 *pTsId, HUINT32 *pSvcId, HUINT8 **ppszCaLinkMsg)
    { return OPL_Broadcast_GetCaLinkMessage(m_vbcastHandle, pOnId, pTsId, pSvcId, ppszCaLinkMsg) == BPL_STATUS_OK;}

    bool getFullScreen(void)
    {
    	BPL_BOOL bFullScreen = FALSE;
    	OPL_Broadcast_GetFullScreen(m_vbcastHandle, &bFullScreen);
    	return bFullScreen == TRUE;
    }

    bool setFullScreen(BPL_BOOL bFullScreen)
    { return OPL_Broadcast_SetFullScreen(m_vbcastHandle, bFullScreen) == BPL_STATUS_OK; }

    bool bindToCurrentChannel(void)
    { return OPL_Broadcast_BindToCurrentChannel(m_vbcastHandle) == BPL_STATUS_OK; }

    bool prevChannel(void)
    { return OPL_Broadcast_PrevChannel(m_vbcastHandle) == BPL_STATUS_OK; }

    bool nextChannel(void)
    { return OPL_Broadcast_NextChannel(m_vbcastHandle) == BPL_STATUS_OK; }

    bool setChannelHandle(OPL_Channel_t channel)
    { return OPL_Broadcast_SetChannelHandle(channel) == BPL_STATUS_OK; }

    bool setChannel(unsigned short usServiceID, unsigned short usTSID, unsigned short usNetID)
    { return OPL_Broadcast_SetChannelByTripleID(m_vbcastHandle, usServiceID,
    							usTSID, usNetID) == BPL_STATUS_OK; }

    bool stop(void)
    { return OPL_Broadcast_Stop(m_vbcastHandle) == BPL_STATUS_OK; }

    bool setChannel(OPL_Channel_t channel)
    { return OPL_Broadcast_SetChannelByChannelObject(m_vbcastHandle, channel) == BPL_STATUS_OK; }

    bool setChannel(OPL_Channel_t channel, unsigned int tuneInfo)
    { return OPL_Broadcast_SetChannelByUIDWithTuneInfo(m_vbcastHandle, channel, tuneInfo) == BPL_STATUS_OK; }

    bool setChannelFCC(OPL_Channel_t channel, unsigned int ulViewID)
    { return OPL_Broadcast_SetChannelByChannelObjectFCC(m_vbcastHandle, channel, ulViewID) == BPL_STATUS_OK; }

    bool stopByViewID(unsigned int ulViewID)
    { return OPL_Broadcast_StopByViewID(m_vbcastHandle, ulViewID) == BPL_STATUS_OK; }

    bool setVisibility(bool bVisible)
    { return OPL_Broadcast_SetVisibility(m_vbcastHandle, bVisible) == BPL_STATUS_OK; }

    bool getVolume(unsigned int *pulVolume)
    { return OPL_Broadcast_GetVolume(m_vbcastHandle, pulVolume) == BPL_STATUS_OK; }

    bool setVolume(unsigned int ulVolume)
    { return OPL_Broadcast_SetVolume(m_vbcastHandle, ulVolume) == BPL_STATUS_OK; }

    bool getMute(int *pnMute)
    { return OPL_Broadcast_GetMute(m_vbcastHandle, pnMute) == BPL_STATUS_OK; }

    bool setMute(int nMute)
    { return OPL_Broadcast_SetMute(m_vbcastHandle, nMute) == BPL_STATUS_OK; }

    bool getWindow(int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL *pbIsFullScreen)
    { return OPL_Broadcast_GetWindowSize(m_vbcastHandle, pX, pY, pWidth, pHeight, pbIsFullScreen) == BPL_STATUS_OK; }

    bool setVideoPosition(int x, int y, int width, int height)
    { return OPL_Broadcast_SetVideoPosition(m_vbcastHandle, x, y, width, height) == BPL_STATUS_OK; }

    bool tvOn(void)
    { return OPL_Broadcast_TvOn(m_vbcastHandle) == BPL_STATUS_OK; }

    bool tvOff(void)
    { return OPL_Broadcast_TvOff(m_vbcastHandle) == BPL_STATUS_OK; }

    bool getWidth (unsigned int *pulWidth)
    { return OPL_Broadcast_GetWidth (m_vbcastHandle, pulWidth) == BPL_STATUS_OK; }

    bool getHeight (unsigned int *pulHeight)
    { return OPL_Broadcast_GetHeight (m_vbcastHandle, pulHeight) == BPL_STATUS_OK; }

    bool getTimeShiftMode (unsigned int *pulTimeShiftMode)
    { return OPL_Broadcast_GetTimeShiftMode (m_vbcastHandle, pulTimeShiftMode) == BPL_STATUS_OK; }

    bool setTimeShiftMode (unsigned int ulTimeShiftMode)
    { return OPL_Broadcast_SetTimeShiftMode (m_vbcastHandle, ulTimeShiftMode) == BPL_STATUS_OK; }

    bool getCurrentTimeShiftMode (unsigned int *pulTimeShiftMode)
    { return OPL_Broadcast_GetCurrentTimeShiftMode (m_vbcastHandle, pulTimeShiftMode) == BPL_STATUS_OK; }

    bool isTimeshiftEnabled (bool *bEnabled)
    { return OPL_Broadcast_IsTimeshiftEnabled (m_vbcastHandle, (HBOOL*)bEnabled) == BPL_STATUS_OK; }

    bool getPlaybackOffset (unsigned int *pulPlaybackOffset)
    { return OPL_Broadcast_GetPlaybackOffset (m_vbcastHandle, pulPlaybackOffset) == BPL_STATUS_OK; }

    bool getPlayPosition (unsigned int *pulPlayPosition)
    	{ return OPL_Broadcast_GetPlayPosition (m_vbcastHandle, pulPlayPosition) == BPL_STATUS_OK; }

    bool getMaxOffset (unsigned int *pulMaxOffset)
    { return OPL_Broadcast_GetMaxOffset (m_vbcastHandle, pulMaxOffset) == BPL_STATUS_OK; }

    bool getStartOffset (unsigned int *pulStartOffset)
    { return OPL_Broadcast_GetStartOffset (m_vbcastHandle, pulStartOffset) == BPL_STATUS_OK; }

    bool seek (int nOffset, unsigned int ulReference)
    { return OPL_Broadcast_SetOffset (m_vbcastHandle, nOffset, ulReference, 0, 0) == BPL_STATUS_OK; }

    bool stopTimeshift (void)
    { return OPL_Broadcast_StopTimeshift (m_vbcastHandle) == BPL_STATUS_OK; }

    bool recordNow(OPL_Channel_t channel, unsigned int ulDurationSec, char *recordID)
    { return OPL_Broadcast_RecordNow(m_vbcastHandle, channel, ulDurationSec, recordID) == BPL_STATUS_OK; }

    bool recordNowAt(OPL_Channel_t channel, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID)
    { return OPL_Broadcast_RecordNowAt(m_vbcastHandle, channel, ulDurationSec, ultargetID, recordID) == BPL_STATUS_OK; }

    bool recordDelayed(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, char *recordID)
    { return OPL_Broadcast_RecordDelayed (m_vbcastHandle, channel, ulStartPos, ulDurationSec, recordID) == BPL_STATUS_OK; }

    bool recordDelayedAt(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID)
    { return OPL_Broadcast_RecordDelayedAt (m_vbcastHandle, channel, ulStartPos, ulDurationSec, ultargetID, recordID) == BPL_STATUS_OK; }

    bool stopRecording(OPL_Channel_t channel)
    { return OPL_Broadcast_StopRecording(m_vbcastHandle, channel) == BPL_STATUS_OK; }

    bool getRecordId (char *recordID)
    { return OPL_Broadcast_GetRecordId(m_vbcastHandle, recordID) == BPL_STATUS_OK; }

    bool getRecordingState(eOplRecorderState *peState)
    { return OPL_Broadcast_GetRecordingState (m_vbcastHandle, peState) == BPL_STATUS_OK; }

    bool getPlayState (OplBroadcastPlayState_e *peState)
    { return OPL_Broadcast_GetPlayState (m_vbcastHandle, peState) == BPL_STATUS_OK; }

    bool setPlayState (OplBroadcastPlayState_e peState)
    { return OPL_Broadcast_SetPlayState (m_vbcastHandle, peState) == BPL_STATUS_OK; }

    bool getPlaySpeed (signed int *pnSpeed)
    { return OPL_Broadcast_GetPlaySpeed (m_vbcastHandle, pnSpeed) == BPL_STATUS_OK; }

    bool setPlaySpeed (signed int nSpeed)
    { return OPL_Broadcast_SetPlaySpeed (m_vbcastHandle, nSpeed) == BPL_STATUS_OK; }

    bool countComponent (eOplAVComponentType eType, unsigned int *pulCompNum)
    { return OPL_Broadcast_CountComponent (m_vbcastHandle, eType, pulCompNum) == BPL_STATUS_OK; }

    bool countMultiViewComponent (unsigned int *pulCompNum)
    { return OPL_Broadcast_CountMutliViewComponent (m_vbcastHandle, pulCompNum) == BPL_STATUS_OK; }

    bool getComponent (eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent)
    { return OPL_Broadcast_GetComponent (m_vbcastHandle, eType, ulTrackIndex, pstComponent) == BPL_STATUS_OK; }

    bool getMultiViewComponent (unsigned int ulIndex, OplMultiViewComponent_t *pstComponent)
    { return OPL_Broadcast_GetMultiViewComponent (m_vbcastHandle, ulIndex, pstComponent) == BPL_STATUS_OK; }

    bool getCurrentComponent (eOplAVComponentType eType, OplAVComponent_t *pstComponent)
    { return OPL_Broadcast_GetCurrentComponent (m_vbcastHandle, eType, pstComponent) == BPL_STATUS_OK; }

    bool setCurrentComponent (eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang)
    { return OPL_Broadcast_SetCurrentComponent (m_vbcastHandle, eType, ulPid, pszLang) == BPL_STATUS_OK; }

    bool getChangedComponent (eOplAVComponentType *eType)
    { return OPL_Broadcast_GetChangedComponent (m_vbcastHandle, eType) == BPL_STATUS_OK; }

    bool offComponent (eOplAVComponentType eType, HUINT32 ulP1)
    { return OPL_Broadcast_offComponent (m_vbcastHandle, eType, ulP1) == BPL_STATUS_OK; }

    const char *requestThumbnail(const char* url, HUINT32 time)
    { return OPL_Broadcast_requestThumbnail(m_vbcastHandle, url, time);}

    eOplPlaybackError getErrorType(void)
    { return OPL_Broadcast_GetErrorType (m_vbcastHandle); }

    bool setErrorType(eOplPlaybackError type)
    { return OPL_Broadcast_SetErrorType(m_vbcastHandle, type); }

    bool controlLive(bool bControl)
    { return OPL_Broadcast_ControlLive(m_vbcastHandle, bControl) == BPL_STATUS_OK; }

    bool getProgrammes (int *count, OPL_ProgrammeHandle **programmes)
    { return OPL_Broadcast_GetProgrammes(m_vbcastHandle, count, programmes) == BPL_STATUS_OK; }

    bool CheckAitSection (void)
    { return OPL_Broadcast_CheckAitSection (m_vbcastHandle) == BPL_STATUS_OK; }

    bool checkChannelChangable (OPL_Channel_t channel, HBOOL *pbChangable)
    { return OPL_Broadcast_CheckChannelChangable (m_vbcastHandle, channel, pbChangable) == BPL_STATUS_OK; }

    bool checkChannelRecordable (OPL_Channel_t channel, HBOOL bIgnoreLive, HBOOL *pbChangable)
    { return OPL_Broadcast_CheckChannelRecordable (m_vbcastHandle, channel, bIgnoreLive, pbChangable) == BPL_STATUS_OK; }

    const char* getPlayingThumbnail (unsigned int pos)
    { return OPL_Broadcast_GetPlayingThumbnail (m_vbcastHandle, pos); }

    bool resultPincode (HBOOL permit, int *retVal)
    { return OPL_Broadcast_ResultPincode (m_vbcastHandle, permit, retVal) == BPL_STATUS_OK; }

    bool getMaxDuration (unsigned int *pulMaxDuration)
    { return OPL_Broadcast_GetMaxDuration (m_vbcastHandle, pulMaxDuration) == BPL_STATUS_OK; }

    int getPlaySpeedCount();
    /* speeds_out should be freed by user
     * delete speeds_out */
    bool getPlaySpeeds(int **speeds_out);

    bool getTrickRestrictMode (OplBroadcastTrickRestrictMode_e *peMode)
    { return OPL_Broadcast_GetTrickRestrictMode (m_vbcastHandle, peMode) == BPL_STATUS_OK; }

    bool getStorageRight (OplBroadcastStorageRight_e *peRight)
    { return OPL_Broadcast_GetStorageRight (m_vbcastHandle, peRight) == BPL_STATUS_OK; }
    bool getTsrPausedPoints (HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks)
    { return OPL_Broadcast_GetTsrPausedPoints (m_vbcastHandle, pulNum, pppstBookmarks) == BPL_STATUS_OK; }

    bool getTsrPaused (HBOOL *pbIsPaused)
    { return OPL_Broadcast_GetTsrPaused (m_vbcastHandle, pbIsPaused) == BPL_STATUS_OK; }


    //JLABS - start
    bool getEventRelayChannel(OPL_Channel_t *eventRelayChannel)
    { return OPL_Broadcast_GetEventRelayChannel(m_vbcastHandle, eventRelayChannel) == BPL_STATUS_OK; }

    bool getEwsChannel(OPL_Channel_t *ewsChannel)
    { return OPL_Broadcast_GetEwsChannel(m_vbcastHandle, ewsChannel) == BPL_STATUS_OK; }

    bool getEwsCurTuner(HINT32 *tunerId)
    { return OPL_Broadcast_GetEwsTuner(m_vbcastHandle, tunerId) == BPL_STATUS_OK; }

    bool getBmlStartFlag(HBOOL *pbAutoFlag)
    { return OPL_Broadcast_GetBmlStartFlag(m_vbcastHandle, pbAutoFlag) == BPL_STATUS_OK; }

    bool getBmlEndParam(int *pnCause, char *pcParam)
    { return OPL_Broadcast_GetBmlEndParam(m_vbcastHandle, pnCause, pcParam) == BPL_STATUS_OK; }

    bool showVideoMessage (int type)
    { return OPL_Broadcast_ShowVideoMessage (m_vbcastHandle, type) == BPL_STATUS_OK; }

    bool hideVideoMessage (int type)
    { return OPL_Broadcast_HideVideoMessage (m_vbcastHandle, type) == BPL_STATUS_OK; }

    bool startBML ()
    { return OPL_Broadcast_StartBML (m_vbcastHandle) == BPL_STATUS_OK; }

    bool endBML ()
    { return OPL_Broadcast_EndBML (m_vbcastHandle) == BPL_STATUS_OK; }

    bool setBMLKeyControlMode (HBOOL permit, int *retVal)
    { return OPL_Broadcast_SetBMLKeyControlMode (m_vbcastHandle, permit, retVal) == BPL_STATUS_OK; }

    bool getBmlVodHandle(OPL_HANDLE *hBml)
    { return OPL_Broadcast_GetBmlHandle(m_vbcastHandle, hBml) == BPL_STATUS_OK;}

    bool getBmlVodName(char **appName)
    { return OPL_Broadcast_GetBmlVodName(m_vbcastHandle, appName) == BPL_STATUS_OK;}

    bool getBmlVodParam(char **param)
    { return OPL_Broadcast_GetBmlVodParam(m_vbcastHandle, param) == BPL_STATUS_OK;}

    //JLABS - end

    bool getParentalRatingChangeStatus(char **ppszContentId, HBOOL *pbBlocked, unsigned int *pRating, char **ppszSystemId)
    { return OPL_Broadcast_GetParentalRatingChangeStatus(m_vbcastHandle, ppszContentId, pbBlocked, pRating, ppszSystemId) == BPL_STATUS_OK; }

    bool releaseParentalRatingChangeStatus(char *pszContentId, char *pszSystemId)
    { return OPL_Broadcast_ReleaseParentalRatingChangeStatus(m_vbcastHandle, pszContentId, pszSystemId) == BPL_STATUS_OK; }

    bool getRctInfoCount(HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize)
    { return OPL_Broadcast_GetRctInfoCount(m_vbcastHandle, totalLinkCount, imgDataByteSize) == BPL_STATUS_OK; }

    bool getRctInfo(void **rctInfo)
    { return OPL_Broadcast_GetRctInfo(m_vbcastHandle, rctInfo) == BPL_STATUS_OK; }

    bool releaseRctInfo(void *rctInfo)
    { return OPL_Broadcast_ReleaseRctInfo(rctInfo) == BPL_STATUS_OK; }

    bool getRequestPincodeMsg(const char **msg)
    { return OPL_Broadcast_GetRequestPincodeMsg(m_vbcastHandle, msg) == BPL_STATUS_OK; }

    bool releaseRequestPincodeMsg(const char *msg)
    { return OPL_Broadcast_ReleaseRequestPincodeMsg(m_vbcastHandle, msg) == BPL_STATUS_OK; }

    bool getNcdChangedMsg(const char **msg,int *pStarttime, int *pDuration)
    { return OPL_Broadcast_GetNcdChangedMsg(m_vbcastHandle, msg,pStarttime,pDuration) == BPL_STATUS_OK; }

    bool releaseNcdChangedMsg(const char *msg)
    { return OPL_Broadcast_ReleaseNcdChangedMsg(m_vbcastHandle, msg) == BPL_STATUS_OK; }

    bool getNetworkUri(const char **uri)
    { return OPL_Broadcast_GetNetworkUri(m_vbcastHandle, uri) == BPL_STATUS_OK; }

    bool releaseNetworkUri(const char *uri)
    { return OPL_Broadcast_ReleaseNetworkUri(m_vbcastHandle, uri) == BPL_STATUS_OK; }

    bool getChannelChangeError (int *pnError)
    { return OPL_Broadcast_GetChannelChangeError (m_vbcastHandle, pnError) == BPL_STATUS_OK; }

    bool getTuneInfoByMheg(HUINT32 *MhegSvcUid, HUINT32 *TuneMode, void**hMhegChannel)
    { return OPL_Broadcast_GetTuneInfoByMheg(m_vbcastHandle, MhegSvcUid,TuneMode, hMhegChannel) == BPL_STATUS_OK; }

    bool getMhegInterruptFlag(HUINT32 *ulInterruptFlag)
    { return OPL_Broadcast_GetMhegInterruptFlag(m_vbcastHandle, ulInterruptFlag) == BPL_STATUS_OK; }

    bool getMhegIcsFlag(HUINT32 *ulMhegIcsFlag)
    { return OPL_Broadcast_GetMhegIcsFlag(m_vbcastHandle, ulMhegIcsFlag) == BPL_STATUS_OK; }

    bool pauseHbbtvMonitoring()
    { return OPL_Broadcast_PauseHbbtvMonitoring(m_vbcastHandle) == BPL_STATUS_OK; }

#if 1 // HUMAX EXTENSION
	OPL_Channel_t	createChannelObject(unsigned short nIdType, unsigned short usMajorChannel, unsigned short usMinorChannel);
#endif
	OPL_Channel_t	createChannelObject(unsigned short  nIdType, int onid, int tsid, int sid);


	OPL_Channel_t	getCurrentChannel(void);
	OPL_Channel_t	getLatestChannel(void);
	int getAVOutputsCount(void);
	OplAVOutput *getAVOutputByIndex(int nIndex);

	OplRecorder *getRecorder();

private:
	OplBroadcast_t		*m_vbcastHandle;
	OplRecorder			*m_recorder;
	void				*m_videoHandle;
};

#endif

#endif
