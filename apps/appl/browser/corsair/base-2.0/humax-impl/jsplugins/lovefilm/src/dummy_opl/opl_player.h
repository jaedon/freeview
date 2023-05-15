/*
 *
 *
 *
 *
 */

#ifndef __OPL_PLAYER_H__
#define __OPL_PLAYER_H__

typedef enum {
	OPLPLAYER_EVENT_TYPE_CONNECTION_ATTEMPTED = 0,
	OPLPLAYER_EVENT_TYPE_CONNECTION_ESTABLISHED,
	OPLPLAYER_EVENT_TYPE_METADATA,
	OPLPLAYER_EVENT_TYPE_STREAM_STARTED_DOWNLOADING,
	OPLPLAYER_EVENT_TYPE_STREAM_STOPPED_DOWNLOADING,
	OPLPLAYER_EVENT_TYPE_STREAM_BECAME_ACTIVE,
	OPLPLAYER_EVENT_TYPE_BUFFER_STARTED,
	OPLPLAYER_EVENT_TYPE_BUFFER_COMPLETED,
	OPLPLAYER_EVENT_TYPE_FRAGMENTS_DOWNLOADED,
	OPLPLAYER_EVENT_TYPE_READY,
	OPLPLAYER_EVENT_TYPE_STREAM_SWITCH_STARTED,
	OPLPLAYER_EVENT_TYPE_STREAM_SWITCH_COMPLETED,
	OPLPLAYER_EVENT_TYPE_PLAYING,
	OPLPLAYER_EVENT_TYPE_PAUSED, 
	OPLPLAYER_EVENT_TYPE_SEEK_STARTED, 
	OPLPLAYER_EVENT_TYPE_SEEK_COMPLETED, 
	OPLPLAYER_EVENT_TYPE_PLAYBACK_SPEED_CHANGED, 
	OPLPLAYER_EVENT_TYPE_VOLUME_CHANGED,
	OPLPLAYER_EVENT_TYPE_STOPPED,
	OPLPLAYER_EVENT_TYPE_ENDED,
	OPLPLAYER_EVENT_TYPE_UNLOADED
} eOplPlayerEventType;

typedef void (*OplPlayerListener_t)(void *hPlayerHandle, eOplPlayerEventType eEventType);

class OplPlayer {
	public:
		OplPlayer();
		~OplPlayer();
		
		virtual void setListener(OplPlayerListener_t listener);
		virtual void setAssetUrl(const char *url);
		char * getAssetUrl(){ return m_url; }
        void setDrmConfig(const char * drmType);
        void setBandwidthMultiplier(int multiplier);
        void setPlayReadyLicense(const char * license);
        void play();
        void pause();
        void seek(int time);
        void stop();
        void unload();
        void setPlaybackSpeed(int speed);
        void setBufferTimeRequiredForInitialPlayback(int time);
        void setBufferTimeRequiredAfterExhaustion(int time);
        void setBufferTimeRequiredAfterStreamSwitch(int time);
        void setBufferTimeRequiredAfterSeeking(int time);
        void setFragmentsDownloadedEventFrequency(int frequency);
        void setPosition(int x, int y);
        void setSize(int width, int height);

	private:
		OplPlayerListener_t m_listener;
		char * m_url;
};


#endif // __OPL_PLAYER_H__

