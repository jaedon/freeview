/** **********************************************************************************************************
	@file 		mediaplayer.h
	
	@date		2013/06/15
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		Base class for Media Player
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*********************************************************************************************************** */

#ifndef __MEDIA_PLAYER_H__
#define __MEDIA_PLAYER_H__

#include "mediasource.h"
#include "stdio.h"
#include <vector>



#define MP_AVCOMPONENT_LANG_SIZE 34
#define MP_AVCOMPONENT_TEXT_SIZE 37
#define MP_AVCOMPONENT_NAME_SIZE 256
#define MP_AVCOMPONENT_ROLE_SIZE 256


typedef enum {
	MB_PLAYEVENT_CANPLAY			= 0,
	MB_PLAYEVENT_FINISH,
	MB_PLAYEVENT_ERROR,
	MB_PLAYEVENT_PAUSED,
	MB_PLAYEVENT_PLAYING,
	MB_PLAYEVENT_SEEK_READY,
	MB_PLAYEVENT_SEEK_COMPLETE,
	MB_PLAYEVENT_UNDERRUN,
	MB_PLAYEVENT_BUFFERED,
	MB_PLAYEVENT_POSITION_CHANGED,
	MB_PLAYEVENT_DURATION_CHANGED,
	MB_PLAYEVENT_FLUSH_COMPLETED,
	MB_PLAYEVENT_NEEDMOREDATA,
	MB_PLAYEVENT_BUFFER_PROGRESS,
	MB_PLAYEVENT_BUFFER_IDLE,
	MB_PLAYEVENT_MSE_UNDERRUN,
	MB_PLAYEVENT_MSE_BUFFERED,
	MB_PLAYEVENT_VIDEOWINDOW_CHANGED,
	MB_PLAYEVENT_COMPONENT_CHANGED
} eMpPlayEvent;

typedef enum
{
	MP_AVCOMPONENT_SUBTYPE_NONE						= 0,

	// Audio
	MP_AVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO			= 1,
	MP_AVCOMPONENT_SUBTYPE_AUDIO_DUALMONO			= 2,
	MP_AVCOMPONENT_SUBTYPE_AUDIO_STEREO				= 3,
	MP_AVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL		= 4,
	MP_AVCOMPONENT_SUBTYPE_AUDIO_SURROUND			= 5,
	MP_AVCOMPONENT_SUBTYPE_AUDIO_DOLBY		 		= 6,

	// Video

	// Subtitle
	MP_AVCOMPONENT_SUBTYPE_SUBTITLE_DVB				= 201,
	MP_AVCOMPONENT_SUBTYPE_SUBTITLE_EBU				= 202,

	MP_AVCOMPONENT_SUBTYPE_MAX
} eMpPlayComponentSubType;


enum eMpPlayAVComponentType
{
	MP_COMPONENT_TYPE_VIDEO = 0, ///< Component containing video data.
	MP_COMPONENT_TYPE_AUDIO = 1, ///< Component containing audio data.
	MP_COMPONENT_TYPE_SUBTITLE = 2, ///< Component containing subtitles.
	MP_COMPONENT_TYPE_MAX = 3
};

typedef struct
{
	unsigned int					 ulComponentTag;
	unsigned int					 ulPid;
	eMpPlayAVComponentType		 	 eType;						// OIPF¿¡ ¸ÂÃã.
	eMpPlayComponentSubType	 		 eSubType;					// Humax Extension¿ë SubType
	bool							 bHardOfHearing_Impaired;	// Humax Hard of Hearing for subtitle or hearing impaired 
	float						 	 fAspectRatio;
	char							 szLanguage[MP_AVCOMPONENT_LANG_SIZE];				// ISO639
	bool							 bAudioDescription;
	unsigned int					 ulAudioChannels;
	char							 encoding[MP_AVCOMPONENT_TEXT_SIZE];
	char							 compName[MP_AVCOMPONENT_NAME_SIZE];
	char							 compRole[MP_AVCOMPONENT_ROLE_SIZE];
} MpPlayAVComponent_t;

typedef enum {
	MP_ERROR_NO_ERROR					= -1,
	MP_ERROR_NOT_SUPPORTED_FORMAT		= 0,
	MP_ERROR_SERVER_FAIL,
	MP_ERROR_UNDEFINED,
	MP_ERROR_INSUFFICIENT_RESOURCE,
	MP_ERROR_CONTENT_CORRUPT,
	MP_ERROR_CONTENT_NOT_AVAILABLE,
	MP_ERROR_NOT_AVAILABLE_POSITION,
	MP_ERROR_BLOCK_PARENTAL_CONTROL,
	MP_ERROR_DRM,

	MP_ERROR_USER_DEFINE				= 100,
	MP_ERROR_PARENTAL_CONTROL
} eMPPlayError;

/*--------------------------------------------------------------------------------------------
 * class MediaPlayer
 *-------------------------------------------------------------------------------------------*/
class MediaPlayerListener
{
public:
	virtual void playStateChanged(eMpPlayEvent event, long long int param = 0) = 0;
};

class MediaPlayer {
public:
	MediaPlayer();
	virtual ~MediaPlayer();

	virtual bool init(MediaSource *source) = 0;
	virtual bool deinit() = 0;

	virtual bool play() = 0;
	virtual bool pause() = 0;
	virtual bool seek(double pos) = 0;
	virtual bool setPlaybackRate(double rate) = 0;
	virtual bool attachSource(const MediaSource *source) = 0;
	virtual bool detachSource(unsigned int id=0) = 0;
	virtual MediaSource *getActiveVideoSource() { return NULL; }
	virtual MediaSource *getActiveAudioSource() { return NULL; }	
	virtual std::string getName() { return m_name; }
	virtual double getPosition() = 0;
	virtual void setPosition(double position) = 0;
	virtual double getDuration() = 0;
	virtual bool getDurationInfinity() = 0;
	virtual void setDuration(double duration) = 0;
	virtual unsigned int getBufferingInfo(unsigned  int **start_times,
					unsigned int **end_times) { return 0; }
	virtual unsigned int getSeekableInfo(unsigned long long **seekable_start,
				unsigned long long **seekable_end) { return 0; }

	virtual bool getWindowSize(int *width, int *height) = 0;
	virtual bool setVideoWindow(int x, int y, int w, int h) = 0;
	virtual bool setFullScreen(bool full_screen) = 0;
	virtual bool setVolume(double volume) = 0;

	virtual int getComponentCount(eMpPlayAVComponentType type) = 0;
	virtual bool getComponent(eMpPlayAVComponentType type, int index, MpPlayAVComponent_t *component) = 0;
	virtual bool selectComponent(eMpPlayAVComponentType type, unsigned int id) = 0;
	virtual bool unselectComponent(eMpPlayAVComponentType type, unsigned int id) = 0;
	virtual bool getSelectedChangedTypeIndex(eMpPlayAVComponentType *type, int *index) = 0;
	virtual int getActiveComponentIndex(eMpPlayAVComponentType type) = 0;


	virtual double getStartDate(void){return 0;}
	virtual eMPPlayError getErrorType(void){return MP_ERROR_NO_ERROR;}
	
	void setListener(MediaPlayerListener *listener) { m_listener = listener; }

public:
	static void cleanup();

public:
	static std::vector<MediaPlayer *>	s_thePlayer;
	std::string			m_name;
	unsigned int m_session;

protected:
	MediaPlayerListener *m_listener;
};
 
static inline MediaPlayer *getPlayer() { return MediaPlayer::s_thePlayer.size() ? *(MediaPlayer::s_thePlayer.rbegin()) : NULL; }
static inline MediaPlayer *getPlayer(void *handle) 
{ 
	for(std::vector<MediaPlayer *>::iterator it = MediaPlayer::s_thePlayer.begin(); it!=MediaPlayer::s_thePlayer.end(); it++)
	{
		MediaPlayer *player = *(it);
		if( handle == (void *)player )
		{
			return player;
		}
	}
	return NULL;
}

static inline MediaPlayer *getPlayer(unsigned int session_id) 
{ 
	for(std::vector<MediaPlayer *>::iterator it = MediaPlayer::s_thePlayer.begin(); it!=MediaPlayer::s_thePlayer.end(); it++)
	{
		MediaPlayer *player = *(it);
		if( player->m_session == session_id )
		{
			return player;
		}
	}
	return NULL;
}


#endif	/* __MEDIA_PLAYER_H__ */
