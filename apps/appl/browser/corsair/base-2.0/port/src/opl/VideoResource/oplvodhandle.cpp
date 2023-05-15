
#include <hlib.h>
#include "oplvodhandle.h"
#include "cvideoresource.h"
#include "bplglobal.h"
#include "oplvideoutil.h"


OplVodHandle::OplVodHandle(
		const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
	: OplVideoHandle(aHandleOwnerType, OplVideoHandle::EVideoHandleTypeVod),
	  mOplVod(NULL),
	  mListener(NULL),
	  mVideoWidth(1000),
	  mVideoHeight(1000)
{
    
}

OplVodHandle::~OplVodHandle()
{
	if (mOplVod)
	{
		delete mOplVod;
		mOplVod = NULL;
	}
}

void OplVodHandle::processVideoResourceAllocated(CVideoResource* aVideoResource)
{
	HxLOG_Debug("[VRM] VOD[%d].processVideoResourceAllocated\n", getIndex());
	if (mOplVod && mOplVod->getResourceId() != aVideoResource->getVideoResourceId())
	{
		delete mOplVod;
		mOplVod = NULL;
	}
	if (!mOplVod)
	{
		mOplVod = createVodObject(aVideoResource);
	}
	mOplVod->setListener(this);
	applyCachedValue();
}

void OplVodHandle::processVideoResourceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType)
{
	HxLOG_Debug("[VRM] VOD[%d].processVideoResourceDeAllocated\n", getIndex());
    if ( EHandleOwnerTypeUserOipf == mHandleOwnerType || EHandleOwnerTypeUserHbbtv == mHandleOwnerType )
    {
#if defined(CONFIG_PROD_HGS1000S)
        mOplVod->m_is_active = false;
#else
		// multi-object problem 
        // mOplVod->m_is_active = false;
#endif
    }
}

void OplVodHandle::playStateChanged(const eOplVodEventType& eEventType)
{
	if (mListener)
	{
		mListener(mPlayerHandle, this, eEventType);
	}
}

const char *OplVodHandle::getUrl()
{
	const char *ret = NULL;
	if (mOplVod)
	{
		ret = mOplVod->getUrl();
	}
	return ret;
}

void OplVodHandle::setUrl(const char *url)
{
	if ( url == NULL )
	{
		mCachedUrl.clear();
	}
	else
	{
		mCachedUrl = url;
	}

	if ( mCachedUrl.size() > 0 )
		videoResourceRequired("setUrl");

	if (mOplVod)
	{
		mOplVod->setUrl(url);
	}
}

const char *OplVodHandle::getMimeType()
{
	const char *ret = NULL;
	if (mOplVod)
	{
		ret = mOplVod->getMimeType();
	}
	return ret;
}

void OplVodHandle::setMimeType(const char *mime_type)
{
	if (mOplVod)
	{
		mOplVod->setMimeType(mime_type);
	}
}

void OplVodHandle::setListener(void *hPlayer, OplVodListener_t listener)
{
	mPlayerHandle = hPlayer;
	mListener = listener;
}

bool OplVodHandle::probe()
{
	bool ret(false);
	videoResourceRequired("probe");
	if (mOplVod)
	{
		applyCachedValue();
		ret = mOplVod->probe();
	}
	return ret;
}

bool OplVodHandle::play()
{
	bool ret = false;
	videoResourceRequired("play");
	HxLOG_Debug("[VRM] VOD[%d].play\n", getIndex());
	if (mOplVod)
	{
		HxLOG_Debug("[VRM] VOD[%d].play applyCachedValue\n", getIndex());
		applyCachedValue();
		ret = mOplVod->play();
		HxLOG_Debug("[VRM] VOD[%d].play = %d\n", getIndex(), ret);
	}
	return ret;
}

bool OplVodHandle::stop(bool aInternal)
{
	bool ret = true;
	HxLOG_Debug("[VRM] VOD[%d].stop\n", getIndex());
	if (mOplVod)
	{
		ret = mOplVod->stop();
	}
	if (!aInternal)
	{
		videoResourceNotRequired("stop");
	}
	return ret;
}

#if defined(CONFIG_TIMESTAMP_64)
bool OplVodHandle::seek(unsigned long long position)
{
	bool ret = false;
	videoResourceRequired("seek");
	if (mOplVod)
	{
		applyCachedValue();
		ret = mOplVod->seek(position);
	}
	return ret;
}
#else
bool OplVodHandle::seek(unsigned int position)
{
	bool ret = false;
	videoResourceRequired("seek");
	if (mOplVod)
	{
		applyCachedValue();
		ret = mOplVod->seek(position);
	}
	return ret;
}
#endif

bool OplVodHandle::seekSegment(unsigned int pos_seg)
{
	bool ret = false;
	videoResourceRequired("seekSegment");
	if (mOplVod)
	{
		applyCachedValue();
		ret = mOplVod->seekSegment(pos_seg);
	}
	return ret;
}

bool OplVodHandle::isPVRPlay()
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->isPVRPlay();
	}
	return ret;
}

bool OplVodHandle::isFullScreen()
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->isFullScreen();
	}
	return ret;
}

bool OplVodHandle::setFullScreen(bool full_screen)
{
	bool ret = true;
	//videoResourceRequired("setFullScreen");
	if (mOplVod)
	{
	    if ( full_screen )
        {
		    ret = mOplVod->setFullScreen(full_screen);
		}
		else
		{
		    ret = mOplVod->setVideoPosition( mCachedVideoPosition.mX, \
    									     mCachedVideoPosition.mY, \
    									     mCachedVideoPosition.mWidth, \
    									     mCachedVideoPosition.mHeight ); 
		}
	}
	return ret;
}

bool OplVodHandle::setVideoPosition(int x, int y, int width, int height)
{
	bool ret = true; //, patial_input_window = false;

	mCachedVideoPosition.mX = x;
	mCachedVideoPosition.mY = y;
	mCachedVideoPosition.mWidth = width;
	mCachedVideoPosition.mHeight = height;

	HxLOG_Debug("[VRM] VOD[%d].setWindow(%d, %d, %d, %d)\n", getIndex(), \
                                            				 mCachedVideoPosition.mX, \
                                            				 mCachedVideoPosition.mY, \
                                            				 mCachedVideoPosition.mWidth, \
                                            				 mCachedVideoPosition.mHeight);

	if (mOplVod)//(isResourceAllocated() && mOplVod)/ // (kskim2) iTV multi object로 인한 workaround, videoresourcemanager 관련 수정 필요
	{
		ret = mOplVod->setVideoPosition( mCachedVideoPosition.mX, \
									     mCachedVideoPosition.mY, \
									     mCachedVideoPosition.mWidth, \
									     mCachedVideoPosition.mHeight );
	}
	return ret;
}

bool OplVodHandle::getWindow(int *pWidth, int *pHeight)
{
	bool ret = false;

	if( (pWidth == NULL) || (pHeight == NULL ))
	{
		HxLOG_Error( "[%s,%d] argument is NULL!\n" , __FUNCTION__, __LINE__ );
		return ret;
	}

	*pWidth =0;
	*pHeight =0;

	if (mOplVod)//(isResourceAllocated() && mOplVod)/ // (kskim2) iTV multi object로 인한 workaround, videoresourcemanager 관련 수정 필요
	{
		ret = mOplVod->getMediaWindow(pWidth, pHeight);
	}
	HxLOG_Debug("[VRM] VOD[%d].getWindow(width : %d, height : %d)\n",
			   getIndex(),
			   *pWidth,
			   *pHeight);

	return ret;
}

bool OplVodHandle::getVideoResolution(int *pWidth, int *pHeight)
{
	bool ret = false;

	if( (pWidth == NULL) || (pHeight == NULL ))
	{
		HxLOG_Error( "[%s,%d] argument is NULL!\n" , __FUNCTION__, __LINE__ );
		return ret;
	}

	if (mOplVod)
	{
		ret = mOplVod->getVideoResolution(pWidth, pHeight);
	}

	return ret;
}

#if defined(CONFIG_TIMESTAMP_64)
unsigned long long OplVodHandle::getPlayPosition()
{
	unsigned long long ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getPlayPosition();
	}
	return ret;
}
#else
unsigned int OplVodHandle::getPlayPosition()
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getPlayPosition();
	}
	return ret;
}
#endif

bool OplVodHandle::getDurationInfinity()
{
	if (mOplVod)
		return mOplVod->getDurationInfinity();
	else
		return 0;
}

#if defined(CONFIG_TIMESTAMP_64)
unsigned long long OplVodHandle::getDuration()
{
	unsigned long long ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getDuration();
	}
	return ret;
}
#else
unsigned int OplVodHandle::getDuration()
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getDuration();
	}
	return ret;
}
#endif

int OplVodHandle::getPlaySpeed()
{
	int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getPlaySpeed();
	}
	return ret;
}

bool OplVodHandle::setPlaySpeed(int speed_sec)
{
	bool ret = false;
	videoResourceRequired("setPlaySpeed");
	if (mOplVod)
	{
		applyCachedValue(false);
		ret = mOplVod->setPlaySpeed(speed_sec);
	}
	return ret;
}

eOplVodPlayState OplVodHandle::getPlayState()
{
	eOplVodPlayState ret(OPLVOD_PLAY_STATE_STOPPED);
	if (mOplVod)//(isResourceAllocated() && mOplVod)/ // (kskim2) iTV multi object로 인한 workaround, videoresourcemanager 관련 수정 필요
	{
		ret = mOplVod->getPlayState();
	}
	return ret;
}

void OplVodHandle::setPlayState(eOplVodPlayState state)
{
}

eOplVodPlayError OplVodHandle::getPlayError()
{
	eOplVodPlayError ret(OPLVOD_PLAY_ERROR_INSUFFICIENT_RESOURCE);
	if (mOplVod)
	{
		ret = mOplVod->getPlayError();
	}
	return ret;
}

void OplVodHandle::setPlayError(eOplVodPlayError error)
{
	if (mOplVod)
	{
		mOplVod->setPlayError(error);
	}
}

eOplVodBufferState OplVodHandle::getBufferState()
{
	eOplVodBufferState ret(OPLVOD_BUFFER_STATE_NONE);
	if (mOplVod)
	{
		ret = mOplVod->getBufferState();
	}
	return ret;
}

void OplVodHandle::setBufferState(eOplVodBufferState state)
{
	if (mOplVod)
	{
		mOplVod->setBufferState(state);
	}
}

unsigned int OplVodHandle::getBufferingInfo(unsigned  int **start_times, unsigned int **end_times)
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getBufferingInfo(start_times, end_times);
	}
	return ret;
}

unsigned int OplVodHandle::getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end)
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getSeekableInfo(seekable_start, seekable_end);
	}
	return ret;
}

unsigned int OplVodHandle::getBufferingRemainedTime()
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getBufferingRemainedTime();
	}
	return ret;
}

unsigned int OplVodHandle::getBufferingPercent()
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getBufferingPercent();
	}
	return ret;
}

int OplVodHandle::getPlaySpeedCount()
{
	int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getPlaySpeedCount();
	}
	return ret;
}

bool OplVodHandle::getPlaySpeeds(int **speeds_out)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getPlaySpeeds(speeds_out);
	}
	return ret;
}

const char* OplVodHandle::getPlayingThumbnail(unsigned int pos)
{
	const char* ret = NULL;
	if (mOplVod)
	{
		ret = mOplVod->getPlayingThumbnail(pos);
	}
	return ret;
}

bool OplVodHandle::getTrickRestrictMode(OplVodTrickRestrictMode_e *peMode)
{
	int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getTrickRestrictMode(peMode);
	}
	return ret;
}

bool OplVodHandle::getStorageRight(OplVodStorageRight_e *peRight)
{
	int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getStorageRight(peRight);
	}
	return ret;
}

void OplVodHandle::callback(eOplVodEventType eEventType)
{
	if (mOplVod)
	{
		mOplVod->callback(eEventType);
	}
}

bool OplVodHandle::isPlaying()
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->isPlaying();
	}
	return ret;
}

bool OplVodHandle::getSubtitleDisplay()
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleDisplay();
	}
	return ret;
}

bool OplVodHandle::setSubtitleDisplay(bool display)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setSubtitleDisplay(display);
	}
	return ret;
}

int OplVodHandle::getSubtitleLanguageCount()
{
	int ret = 0;
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleLanguageCount();
	}
	return ret;
}

int OplVodHandle::getSubtitleActiveLanguageIndex()
{
	int ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleActiveLanguageIndex();
	}
	return ret;
}

bool OplVodHandle::getSubtitleLanguage(int index, char *out_lang)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleLanguage(index, out_lang);
	}
	return ret;
}

bool OplVodHandle::setSubtitleLanguage(int index)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setSubtitleLanguage(index);
	}
	return ret;
}

int OplVodHandle::getSubtitleTextPosition()
{
	int ret = -1;
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleTextPosition();
	}
	return ret;
}

bool OplVodHandle::setSubtitleTextPosition(int pos)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setSubtitleTextPosition(pos);
	}
	return ret;
}

int OplVodHandle::getSubtitleFontSize()
{
	int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleFontSize();
	}
	return ret;
}

bool OplVodHandle::setSubtitleFontSize(int size)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setSubtitleFontSize(size);
	}
	return ret;
}

int OplVodHandle::getSubtitleSyncTime()
{
	int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getSubtitleSyncTime();
	}
	return ret;
}

bool OplVodHandle::setSubtitleSyncTime(int sync_time)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setSubtitleSyncTime(sync_time);
	}
	return ret;
}

eOplAVComponentType OplVodHandle::getSelectedComponentType()
{
	eOplAVComponentType ret((eOplAVComponentType)-1);
	if (mOplVod)
	{
		ret = mOplVod->getSelectedComponentType();
	}
	return ret;
}

unsigned int OplVodHandle::getComponentsCount(eOplAVComponentType comp_type)
{
	unsigned int ret(0);
	if (mOplVod)
	{
		ret = mOplVod->getComponentsCount(comp_type);
	}
	return ret;
}

bool OplVodHandle::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getComponent(comp_type, index, out_comp);
	}
	return ret;
}

bool OplVodHandle::getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getActiveComponent(comp_type, out_comp);
	}
	return ret;
}

int OplVodHandle::getActiveComponentIndex(eOplAVComponentType comp_type)
{
	int index = -1;
	if (mOplVod)
	{
		index = mOplVod->getActiveComponentIndex(comp_type);
	}
	return index;
}

bool OplVodHandle::setActiveComponent(eOplAVComponentType comp_type, int pid, char *pszLang)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setActiveComponent(comp_type, pid, pszLang);
	}
	return ret;
}

bool OplVodHandle::setActiveComponent(eOplAVComponentType comp_type, int index)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->setActiveComponent(comp_type, index);
	}
	return ret;
}

bool OplVodHandle::unsetActiveComponent(eOplAVComponentType comp_type, int index)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->unsetActiveComponent(comp_type, index);
	}
	return ret;
}


bool OplVodHandle::offComponent(eOplAVComponentType comp_type, int pid)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->offComponent(comp_type, pid);
	}
	return ret;
}

bool OplVodHandle::setSelectedComponentTypeIndex(eOplAVComponentType *comp_type, int *index)
{
	bool ret = false;
	if (mOplVod)
	{
		*comp_type = mOplVod->getSelectedComponentType();
		*index = mOplVod->getSelectedComponentIndex();
	}
	return ret;
}

void OplVodHandle::setParentalRating(bool blocked, unsigned int rating, const char *cas_id)
{
	if (mOplVod)
	{
		mOplVod->setParentalRating(blocked, rating, cas_id);
	}
}

bool OplVodHandle::getParentalRating(bool *blocked, unsigned int *rating, char **cas_id)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getParentalRating(blocked, rating, cas_id);
	}
	return ret;
}

void OplVodHandle::setDrmRightsError(eOplVodDrmRightsError error, const char *cas_id, const char *content_id, const char *rights_issuer_id)
{
	if (mOplVod)
	{
		mOplVod->setDrmRightsError(error, cas_id, content_id, rights_issuer_id);
	}
}

bool OplVodHandle::getDrmRightsError(eOplVodDrmRightsError *error, char **cas_id, char **content_id, char **rights_issuer_id)
{
	bool ret = false;
	if (mOplVod)
	{
		ret = mOplVod->getDrmRightsError(error, cas_id, content_id, rights_issuer_id);
	}
	return ret;
}

eOplVodPlayErrorDetails OplVodHandle::getPlayErrorDetails()
{
	eOplVodPlayErrorDetails ret(OPLVOD_PLAY_ERRORDETAILS_UNKNOWN_ERROR);
	if (mOplVod)
	{
		ret = mOplVod->getPlayErrorDetails();
	}
	return ret;
}

void OplVodHandle::setPlayErrorDetails(eOplVodPlayErrorDetails error)
{
	if (mOplVod)
	{
		mOplVod->setPlayErrorDetails(error);
	}
}

bool OplVodHandle::hasVodObject()
{
	return mOplVod ? true: false;
}

OplVod& OplVodHandle::getVodObject()
{
	return *mOplVod;
}

bool OplVodHandle::isEmptyVod(void)
{
	return OplVod::isEmptyVod();
}

void OplVodHandle::applyCachedValue(bool is_new_url)
{
	if (mCachedUrl.length())
	{
		mOplVod->setUrl(mCachedUrl.c_str(), is_new_url);
	}
	
	if (mCachedVideoPosition.isValid())
	{
		mOplVod->setVideoPosition( mCachedVideoPosition.mX, \
		                           mCachedVideoPosition.mY, \
		                           mCachedVideoPosition.mWidth, \
		                           mCachedVideoPosition.mHeight );
	}
	else
	{
	    mOplVod->setVideoPosition(0, 0, OSD_WIDTH, OSD_HEIGHT);   
	}
}

