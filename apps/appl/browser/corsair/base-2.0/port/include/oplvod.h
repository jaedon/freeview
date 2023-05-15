/*************************************************************************************************************
	File 		: oplvod.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/16
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#ifndef __OPLVOD_H__
#define __OPLVOD_H__


#include <string>
#include <list>
#include <oplavcomponent.h>
#include <oplvodhandle_def.h>

#include "autoconf.h" // temporarily included for global configuration


/****************************************************************************************
 * OplVod class
 ***************************************************************************************/
class OplVod {
public:
	OplVod(int aVideoResourceId = 0);
	virtual ~OplVod();
	int getResourceId();
	virtual const char *getUrl();
	virtual void setUrl(const char *url, bool is_new_url = true);
	virtual const char *getMimeType();
	virtual void setMimeType(const char *mime_type);
	virtual void setListener(OplVodListener_t listener);
	virtual void setListener(IMediaPlayerListener* listener);
	virtual bool probe();
	virtual bool play();
	virtual bool play(OplVodProbeInfo *probe_info) { return true; }
	virtual bool stop(bool internal=false);
#if defined(CONFIG_TIMESTAMP_64)
	virtual bool seek(unsigned long long position);
#else
	virtual bool seek(unsigned int position);
#endif
	virtual bool seekSegment(unsigned int pos_segment);
	virtual bool isPVRPlay();
	virtual bool isFullScreen();
	virtual bool setFullScreen(bool full_screen);
	virtual bool setVideoPosition(int x, int y, int width, int height);

	virtual bool getMediaWindow(int *width, int *height);
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
	virtual unsigned int getSessionID();
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
	/* speeds_out should be freed by user
	 * delete speeds_out */
	virtual bool getPlaySpeeds(int **speeds_out);
	virtual bool getTrickRestrictMode(OplVodTrickRestrictMode_e *peMode);
	virtual void setStorageRight(OplVodStorageRight_e right);
	virtual bool getStorageRight(OplVodStorageRight_e *peRight);

	virtual void callback(eOplVodEventType eEventType);
	virtual bool isPlaying();
	virtual bool isValidEvent(eOplVodPlayState state);

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
	virtual eOplAVComponentType getSelectedComponentType();
	virtual bool setSelectedComponentType(eOplAVComponentType comp_type);
	virtual int getSelectedComponentIndex(void);
	virtual bool setSelectedComponentIndex(int index);
	virtual unsigned int getComponentsCount(eOplAVComponentType comp_type);
	virtual bool getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp);
	virtual int getActiveComponentIndex(eOplAVComponentType comp_type);
	virtual bool getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp);
	virtual bool setActiveComponent(eOplAVComponentType comp_type, int pid, char *pszLang);
	virtual bool setActiveComponent(eOplAVComponentType comp_type, int index);
	virtual bool unsetActiveComponent(eOplAVComponentType comp_type, int index);
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

	virtual bool setPlayerType(eOplVodPlayerType type);
	virtual bool getVideoResolution(int *pWidth, int *pHeight);
	virtual double getStartDate(void);
#if defined(CONFIG_TIMESTAMP_64)
	virtual bool setStartPos(long long int pos);
#else
	virtual bool setStartPos(int pos);
#endif


protected:
	void touch();

	virtual unsigned int getVideoComponentCount();
	virtual bool getVideoComponent(int index, OplAVComponent_t *out_comp);
	virtual bool getActiveVideoComponent(OplAVComponent_t *out_comp);
	virtual bool setActiveVideoComponent(int pid);

	virtual unsigned int getAudioComponentCount();
	virtual bool getAudioComponent(int index, OplAVComponent_t *out_comp);
	virtual bool getActiveAudioComponent(OplAVComponent_t *out_comp);
	virtual bool setActiveAudioComponent(int pid, char* pszLang);

	virtual unsigned int getSubtitleComponentCount();
	virtual bool getSubtitleComponent(int index, OplAVComponent_t *out_comp);
	virtual bool getActiveSubtitleComponent(OplAVComponent_t *out_comp);
	virtual bool setActiveSubtitleComponent(int pid, char* pszLang);
	virtual bool offSubtitleComponent(int pid);

	bool setDrmLicenseData(const char *data);
	bool setBufferingMode(bool bauto, int threshold_seconds);

	/* for Maxdome, Youtube, ... */
	void __apply_scenario();
public:
	void setSessionID(unsigned int id){m_session_id = id;}

public:
	static OplVod *getActiveVod();
	static OplVod *findVodByOapiSession(const unsigned int id);
	static void printVodInfo();
	static void setAutoMode(bool auto_mode);
	static bool __prepare_playback();
	static bool __complete_playback();
	static bool isEmptyVod(void);
	static void setUseragent(const char* useragent);
	static void setApplicationUrl(const char* url);
	static const char *getPlayStateString(eOplVodPlayState state);	/* for debug */

public:
	static std::list<OplVod *>	s_vods;
	static bool s_auto_mode;
	static std::string 		m_application_url;
	static std::string		m_useragent;


protected:
	typedef enum {
		VOD_ACTION_NONE		= 0,
		VOD_ACTION_PROBE,
		VOD_ACTION_PLAY,
		VOD_ACTION_STOP,
		VOD_ACTION_INTERNAL_STOP,
	} ePlayAction;

private:
	OplVodListener_t			m_callback;
	IMediaPlayerListener*		m_listener;

protected:
	std::string			m_url;
	std::string			m_mime_type;
	bool				m_is_fullscreen;
	bool				m_is_pvr;
	eOplVodPlayState	m_play_state;
	eOplVodBufferState	m_buffer_state;
	eOplVodPlayError	m_play_error;
	eOplVodPlayErrorDetails	m_play_errorDetails; /* for Maxdome */
	int					m_dest;
	void 				*m_user_data;
	unsigned long		m_latest_tick;
	bool				m_parental_blocked;
	unsigned int		m_parental_rating_value;
	char				m_cas_system_id[128];
	char				m_drm_content_id[1024];
	char				m_drm_rights_issuer_url[1024];
	eOplVodDrmRightsError	m_drm_rights_error;
	ePlayAction			m_play_action;
	unsigned int		m_session_id; /* sessionID assigned by OAPI_MEDIA */
	eOplAVComponentType m_seleted_comptype;
	int					m_seleted_index;
#if defined(CONFIG_TIMESTAMP_64)
	long long int 		m_start_pos;
#else
	int					m_start_pos;
#endif


	OplVodStorageRight_e		m_storage_right;

public:
	std::list<std::string>	m_queue_url;
	bool				m_is_active;
	bool				m_is_download_vod;
	int 				m_loop;
	bool 				m_duration_Infinity;
	bool 				m_is_buffered;
};

#endif
