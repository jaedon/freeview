#ifndef __OPLVIDEOBROADCASTHANDLE_H__
#define __OPLVIDEOBROADCASTHANDLE_H__

#include <bpltype.h>
#include <oplchannel.h>
#include <oplprogramme.h>
#include <oplavoutput.h>
#include <oplavcomponent.h>
#include <oplrecorder.h>
#include <oplmail.h>
#include <oplmultiviewcomponent.h>
#include <oplbookmark.h>

#include <oplvideohandle.h>
#include <oplvideobroadcasthandle_def.h>

class IVideoBroadcastListener
{
public:
	enum TEventType
	{
		EEventLive,
		EEventTsr,
		EEventCas,
		EEventRec,
		EEventMail,
		EEventBml,
		EEventMheg
	};

	virtual void handleVideoBroadcastResourceEvent(const TEventType& aEventType, const eOplBroadcastEventType& aOplEventType) = 0;
};

class OplVideoBroadcastHandle;

class BroadcastState {
public:
	typedef enum {
		BROADCAST_ACTION_CREATE = 0,
		BROADCAST_ACTION_SET_CHANNEL,
		BROADCAST_ACTION_BIND_CHANNEL,
		BROADCAST_ACTION_NEXT_PREV_CHANNEL,
		BROADCAST_ACTION_RELEASE,
		BROADCAST_ACTION_STOP,
	} eBroadcastAction;

	typedef enum {
		BROADCAST_STATE_UNREALIZED		= 0,
		BROADCAST_STATE_CONNECTING,
		BROADCAST_STATE_PRESENTING,
		BROADCAST_STATE_STOPPED,
	} eBroadcastState;

	BroadcastState();
	~BroadcastState();

	eBroadcastAction getAction(void);
	bool setAction(OplVideoBroadcastHandle *handle, eBroadcastAction);
	eBroadcastState getState(void);
	bool setState(eBroadcastState);
	bool stateChanged(OplVideoBroadcastHandle *handle, IVideoBroadcastListener::TEventType, eOplBroadcastEventType);

private:
	eBroadcastAction m_broadcast_action;
	eBroadcastState m_broadcast_state;
	eOplBroadcastEventType m_opl_event;
};


class OplVideoBroadcast;
class OplVideoBroadcastHandle : public OplVideoHandle, IVideoBroadcastListener {
private:
	OplVideoBroadcastHandle(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
public:
    static OplVideoBroadcastHandle* create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
    virtual ~OplVideoBroadcastHandle();
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
    bool stop(unsigned int ulViewID);
    bool pauseHbbtvMonitoring(void);
    bool setChannel(OPL_Channel_t channel);
    bool	setChannel(OPL_Channel_t channel, unsigned int tuneInfo);
    bool setChannelFCC(OPL_Channel_t channel, unsigned int ulViewID);
    bool getVolume(unsigned int *pulVolume);
    bool setVolume(unsigned int ulVolume);
    bool getWindow(int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL* pbIsFullScreen);
    bool setVideoPosition(int x, int y, int width, int height);
    bool tvOn(void);
    bool tvOff(void);
    bool getWidth (unsigned int *pulWidth);
    bool getHeight (unsigned int *pulHeight);
    bool getTimeShiftMode (unsigned int *pulTimeShiftMode);
    bool setTimeShiftMode (unsigned int ulTimeShiftMode);
    bool getCurrentTimeShiftMode (unsigned int *pulTimeShiftMode);
    bool isTimeshiftEnabled (bool *bEnabled);
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
    bool setPlayState (OplBroadcastPlayState_e peState);
    bool setErrorType (eOplPlaybackError type);
    bool getPlaySpeed (signed int *pnSpeed);
    bool setPlaySpeed (signed int nSpeed);
    bool countComponent (eOplAVComponentType eType, unsigned int *pulCompNum);
    bool countMultiViewComponent (unsigned int *pulCompNum);
    bool getComponent (eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent);
    bool getMultiViewComponent (unsigned int ulIndex, OplMultiViewComponent_t *pstComponent);
    bool getCurrentComponent (eOplAVComponentType eType, OplAVComponent_t *pstComponent);
    bool setCurrentComponent (eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang);
    bool getChangedComponent (eOplAVComponentType* eType);
    bool offComponent (eOplAVComponentType eType, HUINT32 ulP1);
    const char* requestThumbnail(const char* url, HUINT32 time);
    eOplPlaybackError getErrorType(void);
    bool controlLive(bool bControl);
    bool getProgrammes (int *count, OPL_ProgrammeHandle **programmes);
    bool CheckAitSection (void);
    bool checkChannelChangable (OPL_Channel_t channel, HBOOL *pbChangable);
    bool checkChannelRecordable (OPL_Channel_t channel, HBOOL bIgnoreLive, HBOOL *pbChangable);
    const char* getPlayingThumbnail (unsigned int pos);
    bool resultPincode (HBOOL permit, int *retVal);
    int getPlaySpeedCount();
    /* speeds_out should be freed by user
     * - delete speeds_out */
    bool getPlaySpeeds(int **speeds_out);
    bool getTrickRestrictMode (OplBroadcastTrickRestrictMode_e *peMode);
    bool getTsrPausedPoints (HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks);
    bool getTsrPaused (HBOOL *pbIsPaused);
    bool getStorageRight (OplBroadcastStorageRight_e *peRight);
    bool getMaxDuration (unsigned int *pulMaxDuration);

    //RCT - start
    bool getRctInfoCount(HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize);
    bool getRctInfo(void **rctInfo);
    bool releaseRctInfo(void *rctInfo);
    //RCT -end
    bool getRequestPincodeMsg(const char **msg);
    bool releaseRequestPincodeMsg(const char *msg);

    //ncd
    bool getNcdChangedMsg(const char **msg,int *pStarttime, int *pDuration);
    bool releaseNcdChangedMsg(const char *msg);

    // mheg
    bool getTuneInfoByMheg(HUINT32 *MhegSvcUid, HUINT32 *TuneMode, void **hMhegChannel);
    bool getMhegInterruptFlag(HUINT32 *bInterruptFlag);
    bool getMhegIcsFlag(HUINT32 *ulMhegIcsFlag);

    //network uri
    bool getNetworkUri(const char **uri);
    bool releaseNetworkUri(const char *uri);

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
    bool getParentalRatingChangeStatus(char **ppszContentId, HBOOL *pbBlocked, unsigned int *pRating, char **ppszSystemId);
    bool releaseParentalRatingChangeStatus(char *pszContentId, char *pszSystemId);
    OPL_Channel_t	createChannelObject(unsigned short nIdType, unsigned short usMajorChannel, unsigned short usMinorChannel);
    OPL_Channel_t	createChannelObject(unsigned short nIdType, int onid, int tsid, int sid);
    OPL_Channel_t	getCurrentChannel(void);
    OPL_Channel_t	getLatestChannel(void);
    int getAVOutputsCount(void);
    OplAVOutput *getAVOutputByIndex(int nIndex);
    OplRecorder *getRecorder();
    void release_object(void);
    void setBroadcastState(BroadcastState *state){ m_state_manager = state; };
    OplVideoBroadcast *getOplVideoBroadcast(){ return mOplVideoBroadcast;}
    OplBroadcastListener_t getBroadcastListener(void){ return mListenerLive; }
protected: // from OplVideoHandle
	virtual void init();
	virtual void deInit();
	void processVideoResourceAllocated(CVideoResource* aVideoResource);
	void processVideoResourceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType);
protected: // IVideoBroadcastListener
	void handleVideoBroadcastResourceEvent(const TEventType& aEventType, const eOplBroadcastEventType& aOplEventType);
private:
	CVideoResource* mBindedResource;
	OplVideoBroadcast* mOplVideoBroadcast;
	OplBroadcastListener_t mListenerLive;
	OplBroadcastListener_t mListenerTsr;
	OplBroadcastListener_t mListenerCas;
	OplBroadcastListener_t mListenerRec;
	OplBroadcastListener_t mListenerMail;
	OplBroadcastListener_t mListenerBml;
	OplBroadcastListener_t mListenerMheg;
	TWindowRect mCachedVideoPosition;
	BroadcastState *m_state_manager;
};


#endif // __OPLVIDEOBROADCASTHANDLE_H__
