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

#ifndef __OPLVIDEOONDEMAND_H__
#define __OPLVIDEOONDEMAND_H__


#include <string>
#include <list>
#include <oplavcomponent.h>
#include <OplVideoOnDemandDef.h>

#include "autoconf.h"		// temporarily included for global configuration 


/****************************************************************************************
 * OplVideoOnDemand class
 ***************************************************************************************/
class OplVideoOnDemand {
protected:
	typedef enum {
		VOD_ACTION_NONE		= 0,
		VOD_ACTION_PROBE,
		VOD_ACTION_PLAY,
		VOD_ACTION_STOP,
		VOD_ACTION_INTERNAL_STOP,
	} ePlayAction;

public:
	OplVideoOnDemand(int aVideoResourceId = 0);
	virtual ~OplVideoOnDemand();

	/* Playback */
	int getResourceId() { return m_dest; }
	virtual void setListener(void *hPlayer, OplVideoOnDemandListener_t listener);
	virtual const char *getUrl() { return m_url.c_str(); }
	virtual void setUrl(const char *url);
	virtual const char *getMimeType() { return m_mime_type.c_str(); }
	virtual void setMimeType(const char *mime_type) { if ( mime_type ) m_mime_type = mime_type; }
	virtual bool setPlayerType(eOplVideoOnDemandPlayerType type);

	virtual bool probe();
	virtual bool play();
	virtual bool play(OplVideoOnDemandProbeInfo *probe_info);
	virtual bool stop(bool internal=false);
#if defined(CONFIG_TIMESTAMP_64)
	virtual bool seek(unsigned long long position);
#else
	virtual bool seek(unsigned int position);
#endif
	virtual bool seekSegment(unsigned int pos_segment);
#if defined(CONFIG_TIMESTAMP_64)
	virtual unsigned long long getPlayPosition();
	virtual unsigned long long getDuration();
#else
	virtual unsigned int getPlayPosition();
	virtual unsigned int getDuration();
#endif
	virtual bool getDurationInfinity();
	virtual void setDuration(unsigned int duration);
	virtual int getPlaySpeed();
	virtual bool setPlaySpeed(int speed_sec);
	virtual int getPlaySpeedCount();
	virtual bool getPlaySpeeds(int **speeds_out);

	virtual eOplVideoOnDemandPlayState getPlayState(){ return m_play_state; }
	virtual void setPlayState(eOplVideoOnDemandPlayState state);
	virtual eOplVideoOnDemandPlayError getPlayError() { return m_play_error; }
	virtual void setPlayError(eOplVideoOnDemandPlayError error) { m_play_error = error; }
	virtual eOplVideoOnDemandBufferState getBufferState() { return m_buffer_state; }
	virtual void setBufferState(eOplVideoOnDemandBufferState state) { m_buffer_state = state; }
	virtual unsigned int getBufferingInfo(unsigned  int **start_times, unsigned int **end_times);
	virtual unsigned int getBufferingRemainedTime();
	virtual unsigned int getBufferingPercent();

	// download
	virtual int	startDownload();
	virtual int	probeDownload();
	virtual bool stopDownload();
	virtual bool resumeDownload();
	virtual bool pauseDownload();
	virtual const char *getDownloadUrl() { return m_dnld_url.c_str(); }
	virtual eOplVideoOnDemandDownloadState getDownloadState() { return m_download_state; }
	virtual void setDownloadState(eOplVideoOnDemandDownloadState state) { m_download_state = state; }
	virtual bool getDownloadStatus(OplDownloadStatus_t *status);
#if defined(CONFIG_TIMESTAMP_64)
	virtual void setDownloadInfo(unsigned int id, unsigned long long duration);
#else
	virtual void setDownloadInfo(unsigned int id, unsigned duration);
#endif
	virtual const char *getDownloadStateString(eOplVideoOnDemandDownloadState state);

	/* Video Window */
	virtual bool setVisibility(bool bVisible);
	virtual bool getFullScreen() { return m_is_fullscreen; }
	virtual bool setFullScreen(bool full_screen);
	virtual bool setWindow(int x, int y, int width, int height);
	virtual bool getVideoResolution(int *pWidth, int *pHeight);
	virtual double getStartDate(void);

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
	virtual bool getParentalRating(bool *blocked, unsigned int *rating, char **cas_id);
	virtual void setParentalRating(bool blocked, unsigned int rating, const char *cas_id);

	/* DRM Rights Error */
	virtual bool getDrmRightsError(eOplVideoOnDemandDrmRightsError *error, char **cas_id,
				 char **content_id, char **rights_issuer_id);
	virtual void setDrmRightsError(eOplVideoOnDemandDrmRightsError error, const char *cas_id,
				const char *content_id, const char *rights_issuer_id);

	/* Media Source Extension */
	virtual bool setProbeInfo(OplVideoOnDemandProbeInfo *probe_info);
	int getPumpIndex();
	bool isPVRPlay(){ return m_is_pvr; };
	void setCache(bool cache) { m_cache = cache; };

private:
	void touch();
	bool isPlaying();
	void checkVodsStatus();

	bool getVideoComponent(int index, OplAVComponent_t *out_comp);
	bool getActiveVideoComponent(OplAVComponent_t *out_comp);
	bool setActiveVideoComponent(int pid);

	bool getAudioComponent(int index, OplAVComponent_t *out_comp);
	bool getActiveAudioComponent(OplAVComponent_t *out_comp);
	bool setActiveAudioComponent(int pid, char* pszLang);

	bool getSubtitleComponent(int index, OplAVComponent_t *out_comp);
	bool getActiveSubtitleComponent(OplAVComponent_t *out_comp);
	bool setActiveSubtitleComponent(int pid, char* pszLang);
	bool offSubtitleComponent(int pid);

	bool setDrmLicenseData(const char *data);
	bool setBufferingMode(bool bauto, int threshold_seconds);

/* for public static functions, it should be changed to private */
public:
	unsigned int getSessionID() { return m_session_id; }
	void setSessionID(unsigned int id){m_session_id = id;}
	bool isValidEvent(eOplVideoOnDemandPlayState state);
	void callback(eOplVideoOnDemandEventType eEventType);
	void setStartPos(int pos) { m_start_pos = pos; }

/* Static */
public:
	static bool getResourceMap(OplVideoOnDemandResourceMap *resource_map);
	static OplVideoOnDemand *getActiveVod();
	static OplVideoOnDemand *findVodByDownloadid(const unsigned int id);
	static OplVideoOnDemand *findVodByOapiSession(const unsigned int id);
	static void setUseragent(const char* useragent) { m_useragent = useragent; }
	static void setApplicationUrl(const char* url) { m_application_url = url; }
	static const char *getPlayStateString(eOplVideoOnDemandPlayState state);	/* for debug */
	static void printVodInfo();
	static void debugVodList(void);

private:
	std::string			m_url;
	std::string			m_mime_type;
	std::string			m_dnld_url;
	bool				m_is_fullscreen;
	bool				m_is_pvr;
	int					m_dest;
	void 				*m_user_data;
	unsigned long		m_latest_tick;
	bool				m_parental_blocked;
	unsigned int		m_parental_rating_value;
	char				m_cas_system_id[128];
	char				m_drm_content_id[1024];
	char				m_drm_rights_issuer_url[1024];
	unsigned int		m_session_id; /* sessionID assigned by OAPI_MEDIA */
	int					m_seleted_index;
#if defined(CONFIG_TIMESTAMP_64)
	long long int		m_start_pos;
	unsigned long long	m_last_pos;
	unsigned long long	m_duration;
#else
	int					m_start_pos;
	unsigned int		m_last_pos;
	unsigned int		m_duration;
#endif
	unsigned int 		m_download_id;

	void 							*m_player;
	OplVideoOnDemandListener_t 		m_listener;
	OplVideoOnDemandProbeInfo 		m_probe_info;
	eOplVideoOnDemandPlayState		m_play_state;
	eOplVideoOnDemandBufferState	m_buffer_state;
	eOplVideoOnDemandPlayError		m_play_error;
	eOplVideoOnDemandDrmRightsError	m_drm_rights_error;
	eOplVideoOnDemandDownloadState 	m_download_state;
	eOplAVComponentType m_seleted_comptype;
	ePlayAction			m_play_action;
	int 				m_cache;

public:
	std::list<std::string>	m_queue_url;
	bool				m_is_active;
	bool				m_is_download_vod;
	bool 				m_duration_Infinity;
	static std::list<OplVideoOnDemand *>	s_vods;
	static std::string 		m_application_url;
	static std::string		m_useragent;
};

#endif
