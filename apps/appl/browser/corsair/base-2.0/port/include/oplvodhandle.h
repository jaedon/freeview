#ifndef __OPLVODHANDLE_H__
#define __OPLVODHANDLE_H__

#include <string>

#include <oplvideohandle.h>
#include <oplavcomponent.h>
#include <oplvodhandle_def.h>

#include "autoconf.h" // temporarily included for global configuration

class OplVod;
class OplVodHandle : public OplVideoHandle, public IMediaPlayerListener {
protected:
	OplVodHandle(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
public:
    virtual ~OplVodHandle();

    virtual const char *getUrl();
    virtual void setUrl(const char *url);
    virtual const char *getMimeType();
    virtual void setMimeType(const char *mime_type);
    virtual void setListener(void *hPlayer, OplVodListener_t listener);
    virtual bool probe();
    virtual bool play();
    virtual bool stop(bool aInternal=false);
#if defined(CONFIG_TIMESTAMP_64)
    virtual bool seek(unsigned long long position);
#else
    virtual bool seek(unsigned int position);
#endif
    virtual bool seekSegment(unsigned int pos_seg);
    virtual bool isPVRPlay();
    virtual bool isFullScreen();
    virtual bool setFullScreen(bool full_screen);
    virtual bool setVideoPosition(int x, int y, int width, int height);
    virtual bool getWindow(int *pWidth, int *pHeight);
    virtual bool getVideoResolution(int *pWidth, int *pHeight);
#if defined(CONFIG_TIMESTAMP_64)
    virtual unsigned long long getPlayPosition();
    virtual unsigned long long getDuration();
#else
	virtual unsigned int getPlayPosition();
	virtual unsigned int getDuration();
#endif


	virtual bool getDurationInfinity();
    virtual int getPlaySpeed();
    virtual bool setPlaySpeed(int speed_sec);
    virtual eOplVodPlayState getPlayState();
    virtual void setPlayState(eOplVodPlayState state);
    virtual eOplVodPlayError getPlayError();
    virtual void setPlayError(eOplVodPlayError error);
    virtual eOplVodBufferState getBufferState();
    virtual void setBufferState(eOplVodBufferState state);
	virtual unsigned int getBufferingInfo(unsigned  int **start_times,
						unsigned int **end_times);
	virtual unsigned int getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end);
	virtual unsigned int getBufferingRemainedTime();
	virtual unsigned int getBufferingPercent();
	virtual const char * getPlayingThumbnail(unsigned int pos);
	virtual int getPlaySpeedCount();
	virtual bool getPlaySpeeds(int **speeds_out);
	virtual bool getTrickRestrictMode(OplVodTrickRestrictMode_e *peMode);
	virtual bool getStorageRight(OplVodStorageRight_e *peRight);

	virtual void callback(eOplVodEventType eEventType);
    virtual bool isPlaying();

    /* Subtitle */
    virtual bool getSubtitleDisplay();
    virtual bool setSubtitleDisplay(bool display);
    virtual int getSubtitleLanguageCount();
    virtual int getSubtitleActiveLanguageIndex();
    virtual bool getSubtitleLanguage(int index, char *out_lang);
    virtual bool setSubtitleLanguage(int index);
    virtual int getSubtitleTextPosition();
    virtual bool setSubtitleTextPosition(int pos);
    virtual int getSubtitleFontSize();
    virtual bool setSubtitleFontSize(int size);
    virtual int getSubtitleSyncTime();
    virtual bool setSubtitleSyncTime(int sync_time);

    /* AVComponent */
    virtual eOplAVComponentType getSelectedComponentType(void);
    virtual unsigned int getComponentsCount(eOplAVComponentType comp_type);
    virtual bool getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp);
    virtual bool getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp);
	virtual int getActiveComponentIndex(eOplAVComponentType comp_type);
    virtual bool setActiveComponent(eOplAVComponentType comp_type, int pid, char *pszLang);
	virtual bool setActiveComponent(eOplAVComponentType comp_type, int index);
	virtual bool unsetActiveComponent(eOplAVComponentType comp_type, int pid);
	virtual bool offComponent(eOplAVComponentType comp_type, int pid);
	virtual bool setSelectedComponentTypeIndex(eOplAVComponentType *comp_type, int *index);

	/* Parental Rating */
	virtual void setParentalRating(bool blocked, unsigned int rating, const char *cas_id);
	virtual bool getParentalRating(bool *blocked, unsigned int *rating, char **cas_id);

	/* DRM Rights Error */
	virtual void setDrmRightsError(eOplVodDrmRightsError error, const char *cas_id, const char *content_id, const char *rights_issuer_id);
	virtual bool getDrmRightsError(eOplVodDrmRightsError *error, char **cas_id, char **content_id, char **rights_issuer_id);

	/* for Maxdome */
	virtual eOplVodPlayErrorDetails getPlayErrorDetails();
	virtual void setPlayErrorDetails(eOplVodPlayErrorDetails error);


protected: // from OplVideoHandle
	void processVideoResourceAllocated(CVideoResource* aVideoResource);
	void processVideoResourceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType);
protected: // IMediaPlayerListener
	void playStateChanged(const eOplVodEventType& eEventType);
protected:
	virtual OplVod* createVodObject(CVideoResource* aVideoResource) = 0;

protected:
	bool hasVodObject();
    OplVod& getVodObject();
	void applyCachedValue(bool is_new_url=true);

public:
	static bool isEmptyVod(void);

protected:
	OplVod* mOplVod;

private:
    OplVodListener_t mListener;
	std::string mCachedUrl;
	TWindowRect mCachedVideoPosition;
	int mVideoWidth;
	int mVideoHeight;
	void *mPlayerHandle;
};

#endif // __OPLVODHANDLE_H__
