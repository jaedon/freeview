/**************************************************************************************/
/**
 * @file oplbroadcast.h
 *
 * Video Broadcast
 *
 **************************************************************************************
 **/

#ifndef __OPL_VIDEOBROADCAST_H__
#define __OPL_VIDEOBROADCAST_H__

#include "bpltype.h"
#include "oplchannel.h"
#include "oplprogramme.h"
#include "oplavoutput.h"
#include "oplavcomponent.h"
#include "oplrecorder.h"
#include "oplmail.h"
#include "oplmultiviewcomponent.h"
#include "OplVideoBroadcastDef.h"
#include "oplbookmark.h"

#include "oapi.h"
#include "apk.h"

#define MAX_SPEED_NUM                       (24)
#if !defined(CONFIG_OP_JAPAN)
#define BPLBROADCAST_AUDIO_DUAL_MONO        1
#endif
#define IsHardOfHearingSubtitle(type)           (((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))

/******************************************************************
	Typedef
******************************************************************/

typedef enum
{
    eOplBroadcastMctrlMode_None             = 0,

    eOplBroadcastMctrlMode_Live,
    eOplBroadcastMctrlMode_TsrPb,

    eOplBroadcastMctrlMode_EndOfCase
} xOplBroadcastMctrlMode_e;

typedef enum
{
    eOplBroadcastBufType_None               = 0,

    eOplBroadcastBufType_Tsr,
    eOplBroadcastBufType_Usr,

    eOplBroadcastBufType_EndOfCase
} xOplBroadcastBufferingType_e;

typedef struct  {
    /*OIPF�԰��� string�̱� ������ contentID�� ��Ȯ�� �� �ǹ��ϴ��� ���� �ӽ÷� HUINT32�� ����. �ʿ��� ��ｿ���ڿ���?����ｿ����?*/
    HUINT32 contentId;
    HBOOL   bBlocked;
    HUINT32 ratingValue;
    HUINT32 caSystemId;
#if defined(CONFIG_OP_SES)
	HBOOL	bBlockedByCas;
#endif
}OplBroadcastParentalRatingStatus_t;

typedef struct  {
    HUINT32     errorState;
    HCHAR*  contentId;
    HCHAR*  rightsIssuerUrl;
    HCHAR*  drmSystemId;
}OplBroadcastDrmRightsStatus_t;

typedef struct {

    HUINT32 ulViewId;
    HUINT32 ulRecSessionId;

    OplBroadcastPlayState_e ePlayState;
    eOplRecorderState eRecState;

    HINT32 nSvcIndex;
    BPL_BOOL bFullScreen;

    OPL_HANDLE hVbcastHandle;
    OPL_HANDLE hQueryHandle;
    OPL_ProgrammeHandle ahPF[12];
    HINT32 nPFLen;
    HUINT32 ulSessionHandle;
    OplBroadcastSessionState_e eSessionState;

    // Recording 
    HBOOL bRecordNotifyEnabled;

    OxMediaPlay_Event_e eLastLiveEvent;
    OxMediaPlay_LiveViewState_e eViewState, eLastViewState;
    OxMediaPlay_LiveLockState_e eLockState, eLastLockState;

    OplBroadcastListener_t liveEventListener;
    OplBroadcastListener_t tsrEventListener;
    OplBroadcastListener_t casEventListener;
    OplBroadcastListener_t recEventListener;
    OplBroadcastListener_t mailEventListener;
    OplBroadcastListener_t bmlEventListener;
    OplBroadcastListener_t mhegEventListener;

    OplRecorder_t *recorder;

    // mail
    OPL_MailHandle hMail;

    // event realy
    HINT32 eventRelaySvcUid;

    // ews - 
    HINT32 ewsSvcUid;
    HINT32 ewsCurSvcUid;

    eOplPlaybackError ePlaybackError;

    // bml
    char *pcBMLName;
    char *pcBMLParam;

    // pinctrl
    OplBroadcastParentalRatingStatus_t stParentalRatingStatus;

    // drm
    OplBroadcastDrmRightsStatus_t stDrmRightsStatus;

    // store the last window size
    HINT32 nLastWindowPositionX;
    HINT32 nLastWindowPositionY;
    HINT32 nLastWindowPositionWidth;
    HINT32 nLastWindowPositionHeight;

    // play position
    HINT32 nTsrPlaytime;
    HINT32 nTsrPlaytimeTick;

    HINT32 nTsrStarttime;
    HINT32 nTsrStarttimeTick;

    Handle_t hCasMsgSession;

    // RCT
    HUINT32 nLinkCount;
    HUINT32 nImgSize;

    //NCD
    HUINT32 nStartTimeofChange;
    HUINT32 nASODuration;
    HUINT8 *pszNetworkMsg;

    //MHEG
    HUINT8 *pszPincodeMsg;
    HINT32 nMhegSvcUid;
    OxMediaPlay_TuningMode_e eMhegTuneMode;
    OPL_Channel_t hMhegChannel;
    HBOOL bInterruptible;
} OplBroadcast_t;

/******************************************************************
	Interfaces
******************************************************************/
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

#ifdef __cplusplus
extern "C" {
BPLStatus OPL_Broadcast_ReleaseRctInfo(void *rctInfo);
BPLStatus OPL_Broadcast_HasSDTeletext();
};
#endif


/******************************************************************
	class OplVBroadcast
******************************************************************/
class OplVBroadcast {
private:
	bool create(void* aVideoHandle, int aResourceId);
	void destroy();
public:
	static OplVBroadcast* getInstance();
	static void destroyInstance();

	bool getResourceId();

	void setLiveEventListener(OplBroadcastListener_t listener);
	void setTsrEventListener(OplBroadcastListener_t listener);
	void setCasEventListener(OplBroadcastListener_t listener);
	void setRecEventListener(OplBroadcastListener_t listener);
	void setMailEventListener(OplBroadcastListener_t listener);
	void setBmlEventListener(OplBroadcastListener_t listener);
	void setMhegEventListener(OplBroadcastListener_t listener);
	bool getCasMessage(unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession, char **ppszMessage, unsigned int *pulMessageLength);
	bool getCasUiEvtInfo(unsigned int *pzEvt, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession);
	bool releaseCasMessage(char *pszMessage);
	bool getDrmError(unsigned int *pulDrmError, char **ppszCaSystemId);
	bool releaseDrmError(char *pszCaSystemId);
	bool getDrmRightError(unsigned int *pulErrorState, char ** ppszContentId, char **ppszCaSystemId, char **ppszRightsIssuerURL);
	bool releaseDrmRightError(char *pszContentId, char *pszCaSystemId, char *pszRightsIssuerURL);
	bool getAutoDispMsg(OPL_AutoDispMsg_t **pphAutoDispMsg);
	bool getNewMailMsg(OPL_MailHandle *hMail);
	bool getCaLinkMsg(HUINT32 *pOnId, HUINT32 *pTsId, HUINT32 *pSvcId, HUINT8 **ppszCaLinkMsg);
	bool getFullScreen(void);
	bool setFullScreen(BPL_BOOL bFullScreen);
	bool bindToCurrentChannel(void);
	bool prevChannel(void);
	bool nextChannel(void);
	bool setChannelHandle(OPL_Channel_t channel);
	bool setChannel(unsigned short usServiceID, unsigned short usTSID, unsigned short usNetID);
	bool stop(void);
	bool setChannel(OPL_Channel_t channel);
	bool setChannel(OPL_Channel_t channel, unsigned int tuneInfo);
	bool setChannelFCC(OPL_Channel_t channel, unsigned int ulViewID);
	bool stopByViewID(unsigned int ulViewID);
	bool setVisibility(bool bVisible);
	bool getVolume(unsigned int *pulVolume);
	bool setVolume(unsigned int ulVolume);
    bool getMute(int *pnMute);
	bool setMute(int nMute);
	bool getWindow(int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL *pbIsFullScreen);
	bool setVideoPosition(int x, int y, int width, int height);
	bool tvOn(void);
	bool tvOff(void);
	bool getWidth (unsigned int *pulWidth);
	bool getHeight (unsigned int *pulHeight);
	bool getTimeShiftMode (unsigned int *pulTimeShiftMode);
	bool setTimeShiftMode (unsigned int ulTimeShiftMode);
	bool getCurrentTimeShiftMode (unsigned int *pulTimeShiftMode);
	bool getPlaybackOffset (unsigned int *pulPlaybackOffset);
	bool getPlayPosition (unsigned int *pulPlayPosition);
	bool getMaxOffset (unsigned int *pulMaxOffset);
	bool getStartOffset (unsigned int *pulStartOffset);
	bool seek (int nOffset, unsigned int ulReference);
	bool stopTimeshift (void);
	bool recordNow(OPL_Channel_t channel, unsigned int ulDurationSec, char *recordID);
	bool recordNowAt(OPL_Channel_t channel, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID);
	bool recordDelayed(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, char *recordID);
	bool recordDelayedAt(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID);
	bool stopRecording(OPL_Channel_t channel);
	bool getRecordId (char *recordID);
	bool getRecordingState(eOplRecorderState *peState);
	bool getPlayState (OplBroadcastPlayState_e *peState);
	bool getPlaySpeed (signed int *pnSpeed);
	bool setPlaySpeed (signed int nSpeed);
	bool countComponent (eOplAVComponentType eType, unsigned int *pulCompNum);
	bool countMultiViewComponent (unsigned int *pulCompNum);
	bool getComponent (eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent);
	bool getMultiViewComponent (unsigned int ulIndex, OplMultiViewComponent_t *pstComponent);
	bool getCurrentComponent (eOplAVComponentType eType, OplAVComponent_t *pstComponent);
	bool setCurrentComponent (eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang);
	bool offComponent (eOplAVComponentType eType, HUINT32 ulP1);
	const char *requestThumbnail(const char* url, HUINT32 time);
	eOplPlaybackError getErrorType(void);
	bool controlLive(bool bControl);
	bool getProgrammes (int *count, OPL_ProgrammeHandle **programmes);
	bool CheckAitSection (void);
	bool checkChannelChangable (OPL_Channel_t channel, HBOOL *pbChangable);
	const char* getPlayingThumbnail (unsigned int pos);
	bool resultPincode (HBOOL permit, int *retVal);
	bool getMaxDuration (unsigned int *pulMaxDuration);
	int getPlaySpeedCount();
	/* speeds_out should be freed by user
	 * delete speeds_out */
	bool getPlaySpeeds(int **speeds_out);
	bool getTrickRestrictMode (OplBroadcastTrickRestrictMode_e *peMode);
	bool getTsrPausedPoints (HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks);
	bool getTsrPaused (HBOOL *pbIsPaused);
	//JLABS - start
	bool getEventRelayChannel(OPL_Channel_t *eventRelayChannel);
	bool getEwsChannel(OPL_Channel_t *ewsChannel);
	bool getEwsCurTuner(HINT32 *tunerId);
	bool getBmlStartFlag(HBOOL *pbAutoFlag);
	bool getBmlEndParam(int *pnCause, char *pcParam);
	bool showVideoMessage (int type);
	bool hideVideoMessage (int type);
	bool startBML ();
	bool endBML ();
	bool setBMLKeyControlMode (HBOOL permit, int *retVal);
	bool getBmlVodHandle(OPL_HANDLE *hBml);
	bool getBmlVodName(char **appName);
	bool getBmlVodParam(char **param);
	//JLABS - end
	bool getParentalRatingChangeStatus(char **ppszContentId, HBOOL *pbBlocked, unsigned int *pRating, char **ppszSystemId);
	bool releaseParentalRatingChangeStatus(char *pszContentId, char *pszSystemId);
	bool getRctInfoCount(HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize);
	bool getRctInfo(void **rctInfo);
	bool releaseRctInfo(void *rctInfo);
	bool getRequestPincodeMsg(const char **msg);
	bool releaseRequestPincodeMsg(const char *msg);
	bool getNcdChangedMsg(const char **msg,int *pStarttime, int *pDuration);
	bool releaseNcdChangedMsg(const char *msg);
	bool getChannelChangeError (int *pnError);
	bool getTuneInfoByMheg(HUINT32 *MhegSvcUid, HUINT32 *TuneMode, void**hMhegChannel);
	bool getMhegInterruptFlag(HUINT32 *ulInterruptFlag);
#if 1 // HUMAX EXTENSION
	OPL_Channel_t	createChannelObject(unsigned short nIdType, unsigned short usMajorChannel, unsigned short usMinorChannel);
#endif
	OPL_Channel_t	createChannelObject(unsigned short  nIdType, int onid, int tsid, int sid);


	OPL_Channel_t	getCurrentChannel(void);
	OPL_Channel_t	getLatestChannel(void);
	int getAVOutputsCount(void);
	OplAVOutput *getAVOutputByIndex(int nIndex);

	OplRecorder *getRecorder();

public:
	OPL_Channel_t OPL_Broadcast_CreateChannelObject(OplBroadcast_t *hBroadcast, unsigned short usMajorChannel, \
	        unsigned short usMinorChannel);
	OPL_Channel_t OPL_Broadcast_CreateChannelObjectByTripleID(OplBroadcast_t *hBroadcast, int onid, int tsid, int sid);
	BPLStatus OPL_Broadcast_GetMhegInterruptFlag(OplBroadcast_t *hBroadcast, HUINT32 *ulInterruptibleFlag);
	BPLStatus OPL_Broadcast_GetTuneInfoByMheg (OplBroadcast_t *hBroadcast, HUINT32 *ulMhegSvcUid, \
	        HUINT32 *ulTuneMode, void **hMhegChannel );
	BPLStatus OPL_Broadcast_SetFullScreen(OplBroadcast_t *hBroadcast, BPL_BOOL bFullScreen);
	BPLStatus OPL_Broadcast_SetChannelByChannelObject(OplBroadcast_t *hBroadcast, OPL_Channel_t channel);
	BPLStatus OPL_Broadcast_GetPlaybackOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset);
	BPLStatus OPL_Broadcast_GetMaxOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset);
	BPLStatus OPL_Broadcast_SetOffset (OplBroadcast_t *hBroadcast, int nOffset, unsigned int ulReference, int bStartPaused, int bUseLatestPbTime);
	OPL_Channel_t OPL_Broadcast_GetCurrentChannel(OplBroadcast_t *hBroadcast);
	OPL_Channel_t OPL_Broadcast_GetLatestChannel(OplBroadcast_t *hBroadcast);
	BPLStatus OPL_Broadcast_GetPlaySpeeds(OplBroadcast_t *hBroadcast, int **speeds_out, int *count_out);
    HUINT32 opl_broadcast_getSvcUidByCurrOffset (HUINT32 ulViewId, HINT32 nOffset);
    static BPLStatus OPL_Broadcast_ReleaseRctInfo(void *rctInfo);
    static BPLStatus OPL_Broadcast_HasSDTeletext();

private:
    OplVBroadcast(void* aVideoHandle = NULL, int aResourceId = 0);
    virtual ~OplVBroadcast();

	void opl_broadcast_initialize(void);
	void UpdateInitialState(OplBroadcast_t* hBcast);
	BPLStatus opl_broadcast_getLastService (HUINT32 ulViewId, DxService_t *pstService);
	HERROR opl_broadcast_CheckChannelChangable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL *pbChangable);
	OPL_Channel_t opl_broadcast_getChangbleChannel_inTheRecording(OplBroadcast_t *hBcast);
	BPLStatus opl_broadcast_BindChannel (OplBroadcast_t *self, int uid, unsigned int tuneInfo);
	HUINT32 opl_broadcast_getAvailableSvcUidByCurrOffset (HUINT32 ulViewId, HINT32 nOffset);
	BPLStatus opl_broadcast_memorizeCurrentChannel (HUINT32 ulSvcUid);
	HERROR opl_broadcast_tvOn (OplBroadcast_t *hBcast);
	HERROR opl_broadcast_tvOff (OplBroadcast_t *hBcast);
	BPLStatus opl_broadcast_getCurrentMasterService (HUINT32    ulViewId, DxService_t *pstService);
	BPLStatus opl_broadcast_getBufferingTypeAndTime (OplBroadcast_t *hBroadcast, \
	        xOplBroadcastBufferingType_e *peType, HUINT32 *pulBufTime);
	BPLStatus opl_broadcast_getStartTime (OplBroadcast_t *hBroadcast, HUINT32 *pulBufTime);
	BPLStatus opl_broadcast_getCurrentSupplementaryService (HUINT32 ulViewId, DxService_t *pstService);
	HERROR opl_broadcast_StartRec(OplBroadcast_t *hBcast, DxService_t *pstService, \
	        HINT32 nStartOffset, HUINT32 ulDuration, OxMediaRec_TargetStorage_e eStorageType, \
	        HUINT8 *szStorageId, HUINT32 *pulSessionId, char *pszRecordID);
	BPLStatus opl_broadcast_getCurrentProgramme (OplBroadcast_t *hBroadcast, OPL_ProgrammeHandle *programme);
	OxMediaPlay_Component_e opl_broadcast_cvtComponentType(eOplAVComponentType eType);
	HERROR opl_broadcast_getComponent(HUINT32 ulViewId, OxMediaPlay_Component_e  eApkCompType, \
	        HINT32 nApkCompIndex, OplAVComponent_t *pstComponent);
	BPLStatus opl_broadcast_disableRecNotifier (OplBroadcast_t *hBcast);
	HERROR opl_broadcast_startLive(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 *pulSessionId);
	HERROR opl_broadcast_startLiveFcc(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 *pulSessionId);
	BPLStatus opl_broadcast_memorizeLastChannelByNetwork (OPL_Channel_t hChannel);
	HERROR opl_broadcast_startLiveByMheg(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 TuneMode, HUINT32 *pulSessionId);
	BPLStatus opl_broadcast_enableRecNotifier (OplBroadcast_t *hBcast, DxService_t *pstService);
	HERROR opl_broadcast_cvtVideoComponent (OxMediaPlay_VideoComp_t *pstDstVideo, OplAVComponent_t *pstSrcComp);
	HERROR opl_broadcast_cvtAudioComponent (OxMediaPlay_AudioComp_t *pstDstAudio, OplAVComponent_t *pstSrcComp);
	HERROR opl_broadcast_cvtSubtitleComponent (OxMediaPlay_SubtitleComp_t *pstDstSubtitle, OplAVComponent_t *pstSrcComp);
	HERROR opl_broadcast_cvtDataComponent (OxMediaPlay_DataComp_t *pstDstData, OplAVComponent_t *pstSrcComp);

    eOplAVComponentSubType opl_broadcast_convertAudioComponentTypeToOplSubType (OxMediaPlay_AudioMode_e eAudioComp);
    HCHAR *opl_broadcast_convertAudioCodecToOplEncoding (DxAudioCodec_e eCodec);

    static eOplPlaybackError opl_broadcast_getNextErrorByStates (OplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent);
    static BPLStatus opl_broadcast_getDrmRightsStatusFromJson(OplBroadcastDrmRightsStatus_t *pstStatus, HUINT32 error_status);
    static BPLStatus opl_broadcast_getErrorFromNotifier (OplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent, HBOOL *pbStateChanged);
    static void live_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_Event_e eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
    static void tsr_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_Event_e eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
	static HERROR opl_broadcase_NotifyMhegEventFromAppkit(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser);
	static void opl_broadcast_notifyRecFunc ( HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaPlay_Event_e eEvent, \
	        OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData );
	static void opl_broadcast_notifyCasMessageFuncFromAppkit(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
	static void opl_broadcast_notifyFuncFromAppkit ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, \
	                                        OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData );
	static void opl_broadcast_notifyMailMessage(APK_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata);
	static void opl_broadcast_PfMonitor (OPL_HANDLE search, OPL_ProgrammeHandle *pf, HINT32 n, OplBroadcast_t *self);
	static HBOOL opl_broadcast_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated);

	OplBroadcast_t *m_vbcastHandle;
	OplRecorder	*m_recorder;
	void *m_videoHandle;

	static HxList_t *s_pstBcastObjList;
    static HUINT32 s_ulOplBroadcast_LastMasterSvcUid;

    HCHAR s_szOplBroadcast_CurrSvcCcid[32];
	HUINT32 s_ulOplBroadcast_LastSuppleSvcUid;
	void *s_chHandleOfSetchannel;
	HBOOL s_bOplBcTvMemorized;
	HINT32 s_nOplBcTvMemorizedUid;

};


#endif
