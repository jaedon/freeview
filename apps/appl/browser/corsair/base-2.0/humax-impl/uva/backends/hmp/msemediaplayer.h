/** **********************************************************************************************************
	@file 		msemediaplayer.h
	
	@date		2013/06/15
	@author		Solution - STB Component (tstbcomp@humaxdigital.com)
	@breif		the Media Player for MSE (Youtube v4)
	
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

#ifndef __MSE_MEDIA_PLAYER_H__
#define __MSE_MEDIA_PLAYER_H__

#include "mediaplayer.h"
#include "msemediasource.h"
#include <vector>
#include <map>
#include <list>
#include <pthread.h>

#define MSE_MP	"MSE"

typedef struct {
public:
	double pts;
	double dts;
	double duration;
	unsigned int data_size;
	void *data;
} MseBuffer;

typedef struct {
public:
	void *src_buf;
	bool eos;
} MseSourceMap;

/*--------------------------------------------------------------------------------------------
 * class MseMediaPlayer
 *-------------------------------------------------------------------------------------------*/
class OplVodHtml5Handle;

class MseMediaPlayer : public MediaPlayer {
public:
	MseMediaPlayer();
	virtual ~MseMediaPlayer();

	virtual bool init(MediaSource *source);
	virtual bool deinit();

	virtual bool play();
	virtual bool resume();	
	virtual bool pause();
	virtual bool seek(double pos);
	virtual bool setPlaybackRate(double rate);
	virtual bool attachSource(const MediaSource *source);
	virtual bool detachSource(unsigned int id=0);
	virtual MediaSource *getActiveVideoSource();
	virtual MediaSource *getActiveAudioSource();
	virtual double getPosition();
	virtual void setPosition(double position);
	virtual double getDuration();
	virtual bool getDurationInfinity();
	virtual void setDuration(double duration);

	virtual bool getWindowSize(int *width, int *height);
	virtual bool setVideoWindow(int x, int y, int w, int h);
	virtual bool setFullScreen(bool full_screen);
	virtual unsigned int getBufferingInfo(unsigned  int **start_times, unsigned int **end_times);
	virtual unsigned int getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end);
	virtual bool setVolume(double volume);

	virtual int getComponentCount(eMpPlayAVComponentType type);
	virtual bool getComponent(eMpPlayAVComponentType type, int index, MpPlayAVComponent_t *component);
	virtual bool selectComponent(eMpPlayAVComponentType type, unsigned int id);
	virtual bool unselectComponent(eMpPlayAVComponentType type, unsigned int id);
	virtual bool getSelectedChangedTypeIndex(eMpPlayAVComponentType *type, int *index);
	virtual int getActiveComponentIndex(eMpPlayAVComponentType type);

	/* extension for MSE/EME */
	bool pushVideo(MseBuffer *buffer);
	bool pushAudio(MseBuffer *buffer);
	bool setEos(unsigned int source_id);
	void __seek(OplVodHtml5Handle *vod, double pos);

	/* for NeedMoreData management */
	MseMediaSource *findSource(unsigned int source_id);
	void lockSource(unsigned int source_id);
	void unlockSource(unsigned int source_id);
	bool isSourceLocking(unsigned int source_id);
	void unlockAllSources();

	/* for DRM */
	void releaseAllDecrypter(void* drmbackend);	

	/* getter/setter */
	unsigned int getSessionID() { return m_session_id; }
	int getPumpIndex() { return m_pump_index; }
	void setPumpIndex(int pump_index) { m_pump_index = pump_index; }
	OplVodHtml5Handle *getVod() { return m_player; }
	static bool clearPlayers();

private:
	bool push(MseMediaSource * s, MseBuffer *buffer);
	int __to_cso_mediatype(MseMediaSource::eMediaType type);
	int __to_cso_codec(MseMediaSource::eCodec codec);
	int __to_mediapb_video_codec(unsigned int codec);	
	int __to_mediapb_audio_codec(unsigned int codec);
	bool cso_open();
	bool cso_close();

private:
	static void onCsoEventCallback(unsigned int session_id, int evt, long long int param);
	static void onEventCallback(void *hPlayer, void *handle, int type);
	void onPlayStateChanged(void *handle);
	void onPlayPositionChanged(void *handle);
	void onPlayPumpIndexChanged(void *handle);
	void onSeekReady(void *handle);
	void onBufferStateChanged(void *handle);

private:
	static std::list<MseMediaPlayer *>	s_players;


private:
	OplVodHtml5Handle	*m_player;	
	std::vector<MseMediaSource *>		m_sources;
	std::map<unsigned int, MseSourceMap *>		m_source_map;
	MseMediaSource 		*m_active_video_source;
	MseMediaSource 		*m_active_audio_source;
	double				m_position;		/* in second */
	double				m_duration;		/* in second */
	unsigned int		m_session_id;
	int					m_pump_index;
	pthread_mutex_t		m_mtx;
	bool				m_seeking;
};

#endif	/* __MSE_MEDIA_PLAYER_H__ */
