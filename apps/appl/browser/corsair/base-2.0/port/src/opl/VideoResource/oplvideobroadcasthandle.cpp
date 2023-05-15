#include <iostream>
#include <hlib.h>
#include "oplvideobroadcasthandle.h"
#include "cvideoresource.h"
#include "oplbroadcast.h"
#include "oplvideoutil.h"

OplVideoBroadcastHandle* OplVideoBroadcastHandle::create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
{
	OplVideoBroadcastHandle* ret = new OplVideoBroadcastHandle(aHandleOwnerType);
	OplVideoHandle::regist(ret);
	// videobroadcast 의 playstate를 state manager 에서 관리 하도록 hbbtv application 에서만 적용 ( 주석을 풀어 사용 )
	if( aHandleOwnerType == EHandleOwnerTypeUserHbbtv )
		ret->setBroadcastState(new BroadcastState());
	//
	return ret;
}

OplVideoBroadcastHandle::OplVideoBroadcastHandle(
		const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
	: OplVideoHandle(aHandleOwnerType, EVideoHandleTypeBroadcast),
	  mBindedResource(NULL),
	  mOplVideoBroadcast(NULL),
	  mListenerLive(NULL),
	  mListenerTsr(NULL),
	  mListenerCas(NULL),
	  mListenerRec(NULL),
	  mListenerMail(NULL),
	mListenerMheg(NULL),
	m_state_manager(NULL)
{

}

OplVideoBroadcastHandle::~OplVideoBroadcastHandle()
{
	if( m_state_manager ) delete m_state_manager;
}

void OplVideoBroadcastHandle::init()
{

}

void OplVideoBroadcastHandle::deInit()
{
	if (mBindedResource)
	{
		mBindedResource->removeVideoBroadcastListener(this);
		mBindedResource = NULL;
	}
}

void OplVideoBroadcastHandle::processVideoResourceAllocated(CVideoResource* aVideoResource)
{
	HxLOG_Critical("[VRM] VB[%d].processVideoResourceAllocated %d\n", getIndex(), aVideoResource->getVideoResourceId());
	mBindedResource = aVideoResource;
	mOplVideoBroadcast = aVideoResource->getVideoBroadcastResource();
	HxLOG_Critical("[VRM]   previouse rect (%d, %d, %d, %d)\n",mCachedVideoPosition.mX, \
                                            				   mCachedVideoPosition.mY, \
                                            				   mCachedVideoPosition.mWidth, \
                                            				   mCachedVideoPosition.mHeight);
	if (mCachedVideoPosition.isValid())
	{
		HxLOG_Critical("[VRM]   apply\n");
		mOplVideoBroadcast->setVideoPosition(mCachedVideoPosition.mX, \
                						     mCachedVideoPosition.mY, \
                						     mCachedVideoPosition.mWidth, \
                						     mCachedVideoPosition.mHeight);
	}
	else
	{
	    mOplVideoBroadcast->setVideoPosition(0, 0, OSD_WIDTH, OSD_HEIGHT);
	}
	
	mBindedResource->addVideoBroadcastListener(this);
}

void OplVideoBroadcastHandle::processVideoResourceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType)
{
	HxLOG_Assert(mOplVideoBroadcast != NULL);
	HxLOG_Critical("[VRM] VB[%d].processVideoResourceDeAllocated\n", getIndex());
	if (aVideoHandleType == OplVideoHandle::EVideoHandleTypeVod)
	{
	}
#if 0
	aVideoResource->removeVideoBroadcastListener(this);
	if (mListenerLive)
	{
		mListenerLive(this, OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
	}
	if (mListenerTsr)
	{
		// TODO: Deliver proper release event
	}
	if (mListenerCas)
	{
		// TODO: Deliver proper release event
	}
	if (mListenerRec)
	{
		// TODO: Deliver proper release event
	}
	if (mListenerMail)
	{
		// TODO: Deliver proper release event
	}
	if (mListenerBml)
	{
		// TODO: Deliver proper release event
	}
#endif
}

void OplVideoBroadcastHandle::handleVideoBroadcastResourceEvent(const TEventType& aEventType, const eOplBroadcastEventType& aOplEventType)
{
	if(m_state_manager){
		if( !m_state_manager->stateChanged(this, aEventType, aOplEventType) )
		{
			return;
		}
	}

	switch (aEventType)
	{
	case IVideoBroadcastListener::EEventLive:
		if (mListenerLive)
		{
			mListenerLive(this, aOplEventType);
		}
		break;
	case IVideoBroadcastListener::EEventTsr:
		if (mListenerTsr)
		{
			mListenerTsr(this, aOplEventType);
		}
		break;
	case IVideoBroadcastListener::EEventCas:
		if (mListenerCas)
		{
			mListenerCas(this, aOplEventType);
		}
		break;
	case IVideoBroadcastListener::EEventRec:
		if (mListenerRec)
		{
			mListenerRec(this, aOplEventType);
		}
		break;
	case IVideoBroadcastListener::EEventMail:
		if (mListenerMail)
		{
			mListenerMail(this, aOplEventType);
		}
		break;
	case IVideoBroadcastListener::EEventBml:
		if (mListenerBml)
		{
			mListenerBml(this, aOplEventType);
		}
		break;
	case IVideoBroadcastListener::EEventMheg:
		if (mListenerMheg)
		{
			mListenerMheg(this, aOplEventType);
		}
		break;
	default:
		HxLOG_Assert(false);
		break;
	}
}

void OplVideoBroadcastHandle::setLiveEventListener(OplBroadcastListener_t listener)
{
	mListenerLive = listener;
}

void OplVideoBroadcastHandle::setTsrEventListener(OplBroadcastListener_t listener)
{
	mListenerTsr = listener;
}

void OplVideoBroadcastHandle::setCasEventListener(OplBroadcastListener_t listener)
{
	mListenerCas = listener;
}

void OplVideoBroadcastHandle::setRecEventListener(OplBroadcastListener_t listener)
{
	mListenerRec = listener;
}

void OplVideoBroadcastHandle::setMailEventListener(OplBroadcastListener_t listener)
{
	mListenerMail = listener;
}

void OplVideoBroadcastHandle::setBmlEventListener(OplBroadcastListener_t listener)
{
	mListenerBml = listener;
}
void OplVideoBroadcastHandle::setMhegEventListener(OplBroadcastListener_t listener)
{
	mListenerMheg = listener;
}

bool OplVideoBroadcastHandle::getCasMessage(unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession, char **ppszMessage, unsigned int *pulMessageLength)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getCasMessage(pzModule, pzSlot, pulSession, ppszMessage, pulMessageLength);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getCasUiEvtInfo(unsigned int *pzEvt, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getCasUiEvtInfo(pzEvt, pzModule, pzSlot, pulSession);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseCasMessage(char *pszMessage)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseCasMessage(pszMessage);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getDrmError(unsigned int *pulDrmError, char **ppszCaSystemId)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getDrmError(pulDrmError, ppszCaSystemId);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseDrmError(char *pszCaSystemId)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseDrmError(pszCaSystemId);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getDrmRightError(unsigned int *pulErrorState, char ** ppszContentId, char **ppszCaSystemId, char **ppszRightsIssuerURL)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getDrmRightError(pulErrorState, ppszContentId, ppszCaSystemId, ppszRightsIssuerURL);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseDrmRightError(char *pszContentId, char *pszCaSystemId, char *pszRightsIssuerURL)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseDrmRightError(pszContentId, pszCaSystemId, pszRightsIssuerURL);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getAutoDispMsg(OPL_AutoDispMsg_t **pphAutoDispMsg)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getAutoDispMsg(pphAutoDispMsg);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getNewMailMsg(OPL_MailHandle *hMail)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getNewMailMsg(hMail);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getCaLinkMsg(HUINT32 *pOnId, HUINT32 *pTsId, HUINT32 *pSvcId, HUINT8 **ppszCaLinkMsg)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getCaLinkMsg(pOnId, pTsId, pSvcId, ppszCaLinkMsg);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getFullScreen()
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getFullScreen();
	}
	return ret;
}

bool OplVideoBroadcastHandle::setFullScreen(BPL_BOOL bFullScreen)
{
	bool ret = false;
	//videoResourceRequired("setFullScreen");
	HxLOG_Critical("[VRM] VB[%d].setFullScreen %s\n", getIndex(), bFullScreen? "TRUE" : "FALSE");
	if (mOplVideoBroadcast)
	{
	    if ( bFullScreen )
	    {
		    ret = mOplVideoBroadcast->setFullScreen(bFullScreen);
		}
		else
		{
		    /* temporary code to cover VRM, even last vb is destroyed after moving to some position,
		       new vb is moved to the position last vb was. */
		    ret = mOplVideoBroadcast->setVideoPosition( mCachedVideoPosition.mX, \
    		                                            mCachedVideoPosition.mY, \
    		                                            mCachedVideoPosition.mWidth, \
    		                                            mCachedVideoPosition.mHeight);
		}
	}
	return ret;
}

bool OplVideoBroadcastHandle::bindToCurrentChannel()
{
	bool ret = false;
	videoResourceRequired("bindToCurrentChannel");
	HxLOG_Critical("[VRM] VB[%d].bindToCurrentChannel\n", getIndex());
	if (mOplVideoBroadcast)
	{
		if( m_state_manager ) m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_BIND_CHANNEL);
		ret = mOplVideoBroadcast->bindToCurrentChannel();
	}
	return ret;
}

bool OplVideoBroadcastHandle::prevChannel()
{
	bool ret = false;
	videoResourceRequired("prevChannel");
	if (mOplVideoBroadcast)
	{
		if( m_state_manager ) m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_NEXT_PREV_CHANNEL);
		ret = mOplVideoBroadcast->prevChannel();
	}
	return ret;
}

bool OplVideoBroadcastHandle::nextChannel()
{
	bool ret = false;
	videoResourceRequired("nextChannel");
	if (mOplVideoBroadcast)
	{
		if( m_state_manager ) m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_NEXT_PREV_CHANNEL);
		ret = mOplVideoBroadcast->nextChannel();
	}
	return ret;
}
bool OplVideoBroadcastHandle::setChannelHandle(OPL_Channel_t channel)
{
	bool ret = false;
	ret = mOplVideoBroadcast->setChannelHandle(channel);
	return ret;
}

bool OplVideoBroadcastHandle::setChannel(unsigned short usServiceID, unsigned short usTSID, unsigned short usNetID)
{
	bool ret = false;
	HxLOG_Critical("[VRM] VB[%d].setChannel\n", getIndex());
	videoResourceRequired("setChannel");
	if (mOplVideoBroadcast)
	{
		if( m_state_manager ) m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_SET_CHANNEL);
		ret = mOplVideoBroadcast->setChannel(usServiceID, usTSID, usNetID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::stop()
{
    bool ret = false;
    if (mOplVideoBroadcast)
    {
    	if( m_state_manager )
    	{
    		m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_STOP);
    	}
    	ret = mOplVideoBroadcast->stop();
    }
    HxLOG_Critical("[VRM] VB[%d].stop\n", getIndex());
    videoResourceNotRequired("stop");
    return ret;
}

bool OplVideoBroadcastHandle::stop(unsigned int ulViewID)
{
	bool ret = false;
	HxLOG_Critical("[VRM] VB[%d].stopFCC\n", getIndex());
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->stopByViewID(ulViewID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::pauseHbbtvMonitoring()
{
    bool ret = false;
    if (mOplVideoBroadcast)
    {
        ret = mOplVideoBroadcast->pauseHbbtvMonitoring();
    }
    return ret;
}

bool OplVideoBroadcastHandle::setChannel(OPL_Channel_t channel)
{
	bool ret = false;
	videoResourceRequired("setChannel");
	HxLOG_Critical("[VRM] VB[%d].setChannel\n", getIndex());
	if (mOplVideoBroadcast)
	{
		if( m_state_manager )
		{
			if( channel )
				m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_SET_CHANNEL);
			else
				m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_RELEASE);
		}
		ret = mOplVideoBroadcast->setChannel(channel);
	}
	return ret;
}

bool OplVideoBroadcastHandle::setChannel(OPL_Channel_t channel, unsigned int tuneInfo)
{
	bool ret = false;
	videoResourceRequired("setChannel");
	HxLOG_Critical("[VRM] VB[%d].setChannel\n", getIndex());
	if (mOplVideoBroadcast)
	{
		if( m_state_manager )
		{
			if( channel )
				m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_SET_CHANNEL);
			else
				m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_RELEASE);
		}

		ret = mOplVideoBroadcast->setChannel(channel, tuneInfo);

	}
	return ret;
}


bool OplVideoBroadcastHandle::setChannelFCC(OPL_Channel_t channel, unsigned int ulViewID)
{
	bool ret = false;
	HxLOG_Critical("[VRM] VB[%d].setChannelFCC\n", getIndex());
	if (mOplVideoBroadcast)
	{
		if( m_state_manager )
		{
			if( channel )
				m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_SET_CHANNEL);
			else
				m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_RELEASE);
		}

		ret = mOplVideoBroadcast->setChannelFCC(channel, ulViewID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getVolume(unsigned int *pulVolume)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getVolume(pulVolume);
	}
	return ret;
}

bool OplVideoBroadcastHandle::setVolume(unsigned int ulVolume)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setVolume(ulVolume);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getWindow(int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL* pbIsFullScreen)
{
	bool ret = false;

	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getWindow(pX, pY, pWidth, pHeight,(BPL_BOOL*)pbIsFullScreen);
	}
	return ret;
}

bool OplVideoBroadcastHandle::setVideoPosition(int x, int y, int width, int height)
{
	bool ret = true;
	mCachedVideoPosition.mX = x;
	mCachedVideoPosition.mY = y;
	mCachedVideoPosition.mWidth = width;
	mCachedVideoPosition.mHeight = height;
	videoResourceRequired("setWindow");
	HxLOG_Critical("[VRM] VB[%d].setWindow(%d, %d, %d, %d)\n", getIndex(),
                                            				   mCachedVideoPosition.mX,
                                            				   mCachedVideoPosition.mY,
                                            				   mCachedVideoPosition.mWidth,
                                            				   mCachedVideoPosition.mHeight);
	if (isResourceAllocated() && mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setVideoPosition( mCachedVideoPosition.mX, \
		                                            mCachedVideoPosition.mY, \
		                                            mCachedVideoPosition.mWidth, \
		                                            mCachedVideoPosition.mHeight);
	}
	return ret;
}

bool OplVideoBroadcastHandle::tvOn()
{
	bool ret = false;
	videoResourceRequired("tvOn");
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->tvOn();
	}
	return ret;
}

bool OplVideoBroadcastHandle::tvOff()
{
	bool ret = false;
	videoResourceRequired("tvOff");
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->tvOff();
	}
	return ret;
}

bool OplVideoBroadcastHandle::getWidth (unsigned int *pulWidth)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getWidth(pulWidth);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getHeight (unsigned int *pulHeight)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getHeight(pulHeight);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getTimeShiftMode (unsigned int *pulTimeShiftMode)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getTimeShiftMode(pulTimeShiftMode);
	}
	return ret;
}

bool OplVideoBroadcastHandle::setTimeShiftMode (unsigned int ulTimeShiftMode)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setTimeShiftMode(ulTimeShiftMode);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getCurrentTimeShiftMode (unsigned int *pulTimeShiftMode)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getCurrentTimeShiftMode(pulTimeShiftMode);
	}
	return ret;
}

bool OplVideoBroadcastHandle::isTimeshiftEnabled (bool *bEnabled)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->isTimeshiftEnabled(bEnabled);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getPlaybackOffset (unsigned int *pulPlaybackOffset)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getPlaybackOffset(pulPlaybackOffset);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getPlayPosition (unsigned int *pulPlayPosition)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getPlayPosition(pulPlayPosition);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getMaxOffset (unsigned int *pulMaxOffset)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getMaxOffset(pulMaxOffset);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getStartOffset (unsigned int *pulStartOffset)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getStartOffset(pulStartOffset);
	}
	return ret;
}

bool OplVideoBroadcastHandle::seek (int nOffset, unsigned int ulReference)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->seek(nOffset, ulReference);
	}
	return ret;
}

bool OplVideoBroadcastHandle::stopTimeshift()
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->stopTimeshift();
	}
	return ret;
}

bool OplVideoBroadcastHandle::recordNow(OPL_Channel_t channel, unsigned int ulDurationSec, char *recordID)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->recordNow(channel, ulDurationSec, recordID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::recordNowAt(OPL_Channel_t channel, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->recordNowAt(channel, ulDurationSec, ultargetID, recordID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::recordDelayed(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, char *recordID)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->recordDelayed(channel, ulStartPos, ulDurationSec, recordID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::recordDelayedAt(OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, unsigned int ultargetID, char *recordID)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->recordDelayedAt(channel, ulStartPos, ulDurationSec, ultargetID, recordID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::stopRecording(OPL_Channel_t channel)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->stopRecording(channel);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getRecordId (char *recordID)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getRecordId(recordID);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getRecordingState(eOplRecorderState *peState)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getRecordingState(peState);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getPlayState (OplBroadcastPlayState_e *peState)
{
	bool ret = true;
	*peState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
	if (mOplVideoBroadcast)
	{
		if( m_state_manager )
		{
			*peState = (OplBroadcastPlayState_e)m_state_manager->getState();
		}
		else
		{
			ret = mOplVideoBroadcast->getPlayState(peState);
		}
	}
	return ret;
}

bool OplVideoBroadcastHandle::setPlayState (OplBroadcastPlayState_e peState)
{
	bool ret = true;

	if (mOplVideoBroadcast)
	{
		if( m_state_manager )
		{
			m_state_manager->setState((BroadcastState::eBroadcastState)peState);
		}

		//ret = mOplVideoBroadcast->setPlayState(peState);

	}
	return ret;
}

bool OplVideoBroadcastHandle::setErrorType (eOplPlaybackError type)
{
	bool ret = true;

	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setErrorType(type);
	}
	return ret;
}


bool OplVideoBroadcastHandle::getPlaySpeed (signed int *pnSpeed)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getPlaySpeed(pnSpeed);
	}
	return ret;
}

bool OplVideoBroadcastHandle::setPlaySpeed (signed int nSpeed)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setPlaySpeed(nSpeed);
	}
	return ret;
}

bool OplVideoBroadcastHandle::countComponent (eOplAVComponentType eType, unsigned int *pulCompNum)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->countComponent(eType, pulCompNum);
	}
	return ret;
}

bool OplVideoBroadcastHandle::countMultiViewComponent (unsigned int *pulCompNum)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->countMultiViewComponent(pulCompNum);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getComponent (eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getComponent(eType, ulTrackIndex, pstComponent);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getMultiViewComponent (unsigned int ulIndex, OplMultiViewComponent_t *pstComponent)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getMultiViewComponent(ulIndex, pstComponent);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getCurrentComponent (eOplAVComponentType eType, OplAVComponent_t *pstComponent)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getCurrentComponent(eType, pstComponent);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getChangedComponent (eOplAVComponentType *eType)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getChangedComponent(eType);
	}
	return ret;
}

bool OplVideoBroadcastHandle::setCurrentComponent (eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setCurrentComponent(eType, ulPid, pszLang);
	}
	return ret;
}

bool OplVideoBroadcastHandle::offComponent (eOplAVComponentType eType, HUINT32 ulP1)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->offComponent(eType, ulP1);
	}
	return ret;
}

const char* OplVideoBroadcastHandle::requestThumbnail(const char* url, HUINT32 time)
{
	const char* ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->requestThumbnail(url, time);
	}
	return ret;
}

eOplPlaybackError OplVideoBroadcastHandle::getErrorType(void)
{
	eOplPlaybackError ret = OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getErrorType();
	}
	return ret;
}

bool OplVideoBroadcastHandle::controlLive(bool bControl)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->controlLive(bControl);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getProgrammes (int *count, OPL_ProgrammeHandle **programmes)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getProgrammes(count, programmes);
	}
	return ret;
}

bool OplVideoBroadcastHandle::CheckAitSection(void)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->CheckAitSection();
	}
	return ret;
}

bool OplVideoBroadcastHandle::checkChannelChangable (OPL_Channel_t channel, HBOOL *pbChangable)
{
	bool ret = false;
	*pbChangable = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->checkChannelChangable(channel, pbChangable);
	}
	return ret;
}

bool OplVideoBroadcastHandle::checkChannelRecordable (OPL_Channel_t channel, HBOOL bIgnoreLive, HBOOL *pbChangable)
{
	bool ret = false;
	*pbChangable = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->checkChannelRecordable(channel, bIgnoreLive, pbChangable);
	}
	return ret;
}

const char* OplVideoBroadcastHandle::getPlayingThumbnail (unsigned int pos)
{
	const char* ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getPlayingThumbnail(pos);
	}
	return ret;
}

bool OplVideoBroadcastHandle::resultPincode (HBOOL permit, int *retVal)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->resultPincode(permit, retVal);
	}
	return ret;
}

int OplVideoBroadcastHandle::getPlaySpeedCount()
{
	int ret = 0;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getPlaySpeedCount();
	}
	return ret;
}

bool OplVideoBroadcastHandle::getPlaySpeeds(int **speeds_out)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getPlaySpeeds(speeds_out);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getTrickRestrictMode (OplBroadcastTrickRestrictMode_e *peMode)
{
	bool ret = true;
	*peMode = OPLBROADCAST_TRICK_MODE_NORESTRICT;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getTrickRestrictMode(peMode);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getTsrPausedPoints (HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks)
{
	bool ret = true;
	*pulNum = 0;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getTsrPausedPoints(pulNum, pppstBookmarks);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getTsrPaused (HBOOL *pbIsPaused)
{
	bool ret = true;
	*pbIsPaused = FALSE;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getTsrPaused(pbIsPaused);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getStorageRight (OplBroadcastStorageRight_e *peRight)
{
	bool ret = true;
	*peRight = OPLBROADCAST_STORAGE_RIGHT_NORESTRICT;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getStorageRight(peRight);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getEventRelayChannel(OPL_Channel_t *eventRelayChannel)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getEventRelayChannel(eventRelayChannel);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getEwsChannel(OPL_Channel_t *ewsChannel)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getEwsChannel(ewsChannel);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getEwsCurTuner(HINT32 *tunerId)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getEwsCurTuner(tunerId);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getBmlStartFlag(HBOOL *pbAutoFlag)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getBmlStartFlag(pbAutoFlag);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getBmlEndParam(int *pnCause, char *pcParam)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getBmlEndParam(pnCause, pcParam);
	}
	return ret;
}

bool OplVideoBroadcastHandle::showVideoMessage (int type)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->showVideoMessage(type);
	}
	return ret;
}

bool OplVideoBroadcastHandle::hideVideoMessage (int type)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->hideVideoMessage(type);
	}
	return ret;
}

bool OplVideoBroadcastHandle::startBML()
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->startBML();
	}
	return ret;
}

bool OplVideoBroadcastHandle::endBML()
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->endBML();
	}
	return ret;
}

bool OplVideoBroadcastHandle::setBMLKeyControlMode (HBOOL permit, int *retVal)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->setBMLKeyControlMode(permit, retVal);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getBmlVodHandle (OPL_HANDLE *hBml)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getBmlVodHandle(hBml);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getBmlVodName(char ** appName)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getBmlVodName(appName);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getBmlVodParam(char ** param)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getBmlVodParam(param);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getParentalRatingChangeStatus(char **ppszContentId, HBOOL *pbBlocked, unsigned int *pRating, char **ppszSystemId)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getParentalRatingChangeStatus(ppszContentId, pbBlocked, pRating, ppszSystemId);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseParentalRatingChangeStatus(char *pszContentId, char *pszSystemId)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseParentalRatingChangeStatus(pszContentId, pszSystemId);
	}
	return ret;
}

OPL_Channel_t	OplVideoBroadcastHandle::createChannelObject(unsigned short nIdType, unsigned short usMajorChannel, unsigned short usMinorChannel)
{
	OPL_Channel_t ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->createChannelObject(nIdType, usMajorChannel, usMinorChannel);
	}
	return ret;
}

OPL_Channel_t	OplVideoBroadcastHandle::createChannelObject(unsigned short nIdType, int onid, int tsid, int sid)
{
	OPL_Channel_t ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->createChannelObject(nIdType, onid, tsid, sid);
	}
	return ret;
}

OPL_Channel_t	OplVideoBroadcastHandle::getCurrentChannel()
{
	OPL_Channel_t ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getCurrentChannel();
	}
	return ret;
}

OPL_Channel_t	OplVideoBroadcastHandle::getLatestChannel()
{
	OPL_Channel_t ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getLatestChannel();
	}
	return ret;
}

int OplVideoBroadcastHandle::getAVOutputsCount()
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getAVOutputsCount();
	}
	return ret;
}

OplAVOutput* OplVideoBroadcastHandle::getAVOutputByIndex(int nIndex)
{
	OplAVOutput* ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getAVOutputByIndex(nIndex);
	}
	return ret;
}

OplRecorder* OplVideoBroadcastHandle::getRecorder()
{
	OplRecorder* ret = NULL;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getRecorder();
	}
	return ret;
}

bool OplVideoBroadcastHandle::getMaxDuration (unsigned int *pulMaxDuration)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getMaxDuration(pulMaxDuration);
	}
	return ret;
}


bool OplVideoBroadcastHandle::getRctInfoCount(HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getRctInfoCount(totalLinkCount, imgDataByteSize);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getRctInfo(void **rctInfo)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getRctInfo(rctInfo);
	}
	return ret;
}


bool OplVideoBroadcastHandle::releaseRctInfo(void *rctInfo)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseRctInfo(rctInfo);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getRequestPincodeMsg(const char **msg)
{
	bool ret = false;
	if(mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getRequestPincodeMsg(msg);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseRequestPincodeMsg(const char *msg)
{
	bool ret = false;
	if(mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseRequestPincodeMsg(msg);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getNcdChangedMsg(const char **msg,int *pStarttime, int *pDuration)
{
	bool ret = false;
	if(mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getNcdChangedMsg(msg,pStarttime,pDuration);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseNcdChangedMsg(const char *msg)
{
	bool ret = false;
	if(mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseNcdChangedMsg(msg);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getNetworkUri(const char **uri)
{
	bool ret = false;
	if(mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getNetworkUri(uri);
	}
	return ret;
}

bool OplVideoBroadcastHandle::releaseNetworkUri(const char *uri)
{
	bool ret = false;
	if(mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->releaseNetworkUri(uri);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getTuneInfoByMheg(HUINT32 *MhegSvcUid, HUINT32 *TuneMode, void **hMhegChannel)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getTuneInfoByMheg(MhegSvcUid, TuneMode, hMhegChannel);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getMhegInterruptFlag(HUINT32 *ulInterruptFlag)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getMhegInterruptFlag(ulInterruptFlag);
	}
	return ret;
}

bool OplVideoBroadcastHandle::getMhegIcsFlag(HUINT32 *ulMhegIcsFlag)
{
	bool ret = false;
	if (mOplVideoBroadcast)
	{
		ret = mOplVideoBroadcast->getMhegIcsFlag(ulMhegIcsFlag);
	}
	return ret;
}


void OplVideoBroadcastHandle::release_object()
{
	if( m_state_manager )
	    m_state_manager->setAction(this, BroadcastState::BROADCAST_ACTION_RELEASE);
	//this->stop();
}


BroadcastState::BroadcastState()
	: m_broadcast_action(BROADCAST_ACTION_CREATE)
	, m_broadcast_state(BROADCAST_STATE_UNREALIZED)
	, m_opl_event(OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR)
{

}

BroadcastState::~BroadcastState()
{

}

BroadcastState::eBroadcastAction BroadcastState::getAction(void)
{
	return m_broadcast_action;
}

bool BroadcastState::setAction(OplVideoBroadcastHandle *handle, BroadcastState::eBroadcastAction action)
{
	m_broadcast_action = action;
	eBroadcastState state = this->getState();

	switch(action)
	{
		case BROADCAST_ACTION_SET_CHANNEL:
			{
				if( (state == BROADCAST_STATE_UNREALIZED)
					|| (state == BROADCAST_STATE_STOPPED)
					|| (state == BROADCAST_STATE_PRESENTING) )
				{
					this->setState(BROADCAST_STATE_CONNECTING);
					handle->getBroadcastListener()(handle, OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
					m_opl_event = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
				}
			}
			break;
		case BROADCAST_ACTION_BIND_CHANNEL:
			{
				if( (state == BROADCAST_STATE_UNREALIZED)
					|| (state == BROADCAST_STATE_STOPPED))
				{
					this->setState(BROADCAST_STATE_CONNECTING);
					handle->getBroadcastListener()(handle, OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
					m_opl_event = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
				}
			}
			break;
		case BROADCAST_ACTION_NEXT_PREV_CHANNEL:
			{
				if( state == BROADCAST_STATE_PRESENTING )
				{
					this->setState(BROADCAST_STATE_CONNECTING);
					handle->getBroadcastListener()(handle, OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
					m_opl_event = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
				}
			}
			break;
		case BROADCAST_ACTION_RELEASE:
			{
				if( (state == BROADCAST_STATE_PRESENTING)
					|| (state == BROADCAST_STATE_STOPPED)
					|| (state == BROADCAST_STATE_CONNECTING) )
				{
					this->setState(BROADCAST_STATE_UNREALIZED);
					handle->getBroadcastListener()(handle, OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
					m_opl_event = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
				}
				else
				{
					this->setState(BROADCAST_STATE_UNREALIZED);
				}
			}
			break;
		case BROADCAST_ACTION_STOP:
			{
				if( (state == BROADCAST_STATE_PRESENTING)
					|| (state == BROADCAST_STATE_CONNECTING) )
				{
					this->setState(BROADCAST_STATE_STOPPED);
					handle->getBroadcastListener()(handle, OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
					m_opl_event = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
				}
			}
			break;
		default:
			break;
	}

	return true;
}

BroadcastState::eBroadcastState BroadcastState::getState(void)
{
	return m_broadcast_state;
}

bool BroadcastState::setState(BroadcastState::eBroadcastState state)
{
	m_broadcast_state = state;
	return true;
}

bool BroadcastState::stateChanged(OplVideoBroadcastHandle *handle,IVideoBroadcastListener::TEventType type, eOplBroadcastEventType event)
{
	bool ret = true;
	eBroadcastState state = this->getState();
	eBroadcastAction action = this->getAction();

	switch(event)
	{
		case OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED:
			{
                // There are one problem if the hbbtv application continues when prism webapp calls setchannel.
                // The problem is that hbbtv application can not take ChannelChangaedSucceed event because below condition.
                // This can be checked in Das Erste Channels.
                // Apps kind of Das Erste change UI when they take ChannelChangaedSucceed event.
				//if( action == BROADCAST_ACTION_BIND_CHANNEL )
				//	ret = false;
			}
			break;
		case OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR:
			{
				//eOplPlaybackError error = handle->getErrorType(); //warning removed.
				if( state == BROADCAST_STATE_CONNECTING )
				{
					this->setState(BROADCAST_STATE_UNREALIZED);
				}
			}
			break;

		case OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE:
			{
				if( state == BROADCAST_STATE_UNREALIZED )
				{
					if( action == BROADCAST_ACTION_STOP )
						ret = false;
				}
				else if( state == BROADCAST_STATE_CONNECTING )
				{
					this->setState(BROADCAST_STATE_PRESENTING);
				}
				else if( state == BROADCAST_STATE_PRESENTING )
				{
					if( action == BROADCAST_ACTION_BIND_CHANNEL )
					{
						OplBroadcastPlayState_e ePlayState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
						handle->getOplVideoBroadcast()->getPlayState(&ePlayState);

						if(ePlayState == OPLBROADCAST_PLAY_STATE_UNREALIZED)
						{
							this->setState(BROADCAST_STATE_UNREALIZED);
						}
						else
						{
							ret = false;
						}
					}
					else if( action == BROADCAST_ACTION_RELEASE )
					{
						if( m_opl_event == event )
							ret = false;
					}
				}
				else if( state == BROADCAST_STATE_STOPPED )
				{
					if( action == BROADCAST_ACTION_STOP )
						ret = false;
				}
			}
			break;
		default:
			break;
	}

	m_opl_event = event;

	return ret;
}

