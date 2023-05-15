/** **********************************************************************************************************
	@file 		hmxmediaplayer.h
	
	@date		2013/06/15
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		Humax Media Player
	
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

#ifndef __HMX_MEDIA_PLAYER_H__
#define __HMX_MEDIA_PLAYER_H__

#include "mediaplayer.h"

#include <pthread.h>
#include <queue>

#define HMX_MP	"HMX"
/*--------------------------------------------------------------------------------------------
 * class HmxMediaPlayer
 *-------------------------------------------------------------------------------------------*/
class OplVodHtml5Handle;
class MediaSource;

class HmxMediaPlayer : public MediaPlayer {
public:
	class HmxBufferState {
	public:
		typedef enum {
			BUFFER_EVENT_PROGRESS	= 0
		} eBufferEvent;

	public:
		HmxBufferState(HmxMediaPlayer *player);
		virtual ~HmxBufferState();
		void touch();
		void doPeriod();

	private:
		static void *threadFunc(void *arg);
	
	private:
		HmxMediaPlayer	*m_hmx_player;
		unsigned int 	m_buffer_endpos;
		unsigned long	m_buffer_progress;
		pthread_t		m_thread;

		pthread_mutex_t	m_mtx;
		bool			m_thread_exit;
		std::queue<eBufferEvent>	m_buffer_eventq;
		unsigned int	m_no_buffered;
	};

public:
	HmxMediaPlayer();
	virtual ~HmxMediaPlayer();

	virtual bool init(MediaSource *source);
	virtual bool deinit();

	virtual bool play();
	virtual bool pause();
	virtual bool seek(double pos);
	virtual bool setPlaybackRate(double rate);
	virtual bool attachSource(const MediaSource *source);
	virtual bool detachSource(unsigned int id=0);
	virtual double getPosition();
	virtual void setPosition(double position);
	virtual double getDuration();
	virtual bool getDurationInfinity(void);
	virtual void setDuration(double duration);

	virtual const MediaSource *getSource() { return NULL; }
	virtual bool getWindowSize(int *width, int *height);
	virtual bool setVideoWindow(int x, int y, int w, int h);
	virtual bool setFullScreen(bool full_screen);
	virtual bool setVolume(double volume);

	virtual unsigned int getBufferingInfo(unsigned  int **start_times,
					unsigned int **end_times);

	virtual unsigned int getSeekableInfo(unsigned long long **seekable_start,
				unsigned long long **seekable_end);
	virtual int getComponentCount(eMpPlayAVComponentType type);
	virtual bool getComponent(eMpPlayAVComponentType type, int index, MpPlayAVComponent_t *component);
	virtual int getActiveComponentIndex(eMpPlayAVComponentType type);
	
	virtual bool selectComponent(eMpPlayAVComponentType type, unsigned int id);
	virtual bool unselectComponent(eMpPlayAVComponentType type, unsigned int id);
	virtual bool getSelectedChangedTypeIndex(eMpPlayAVComponentType *type, int *index);

	virtual double getStartDate(void);

	virtual eMPPlayError getErrorType(void);

	HmxBufferState *getBufferState() { return m_buffer_state; }

protected:
	static void onEventCallback(void *hPlayer, void *handle, int type);
	void onPlayStateChanged(void *handle);
	void onBufferStateChanged(void *handle);
	void onPlayPositionChanged(void *handle);
	void onPlayDurationChanged(void *handle);
	void onBufferInfoChanged(void *handle);
	void onVideoWindowChanged(void *handle);
	void onSelectedComponentChanged(void *handle);

private:
	OplVodHtml5Handle	*m_player;
	MediaSource			*m_source;
	double				m_position;		/* in second */
	double				m_duration;		/* in second */
	HmxBufferState		*m_buffer_state;
	eMPPlayError		m_error_code;
};

#endif	/* __HMX_MEDIA_PLAYER_H__ */
