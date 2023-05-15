/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef PLAYBACKGROUPNATIVE_H
#define PLAYBACKGROUPNATIVE_H

/** @file PlaybackGroupNative.h - Reference implementation of the IPlaybackGroup
 * interface.
 *
 * PlaybackGroupNative is the reference application's implementation of the
 * IPlaybackGroup interface (see IElementaryStreamPlayer.h). A device partner
 * may use or modify this header and accompanying .cpp as needed.
 *
 * A PlaybackGroupNative is created each time a movie is loaded for viewing and
 * persists until the movie is unloaded. The playback group is used to
 * instantiate video and audio elementary stream players,to set the playback
 * state of the movie presentation to play or pause, and to control the video
 * window as defined in IElementaryStreamPlayer.h. PlaybackGroupNative has an
 * instance of ReferenceClock which stores the current playback positions of the
 * audio and video players.
 *
 */


#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/IDeviceError.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/NFErr.h>
#include <set>

#include "cso_def.h"
#include "cso_uva.h"

namespace netflix {
namespace device {
namespace esplayer {


class ESManagerNative;
class ESPlayerNative;
class AudioESPlayerNative;
class VideoESPlayerNative;
class ReferenceClockNative;

class AVController;
class AVStatusManager;

typedef struct PlaybackGroupProbeInfo
{
	int32_t video_codec;
	int32_t audio_codec;
	uint32_t video_pid;
	uint32_t audio_pid;
} PlaybackGroupProbeInfo_t;

typedef struct PlaybackGroupNativeResourceMap
{
	unsigned int *hPlaypumpList;
	unsigned int *hDmaList;
	unsigned int uiNumOfPlaypump;
	unsigned int uiNumOfDma;
}PlaybackGroupNativeResourceMap_t;

/* 	Below enumerations(A/V codec) reference oapi_types.h. 
	If oapi_types.h changes, then checks below enumerations */
typedef enum
{
	PLAYBACKGROUP_VIDEO_CODEC_UNKNOWN 			= 0,
	PLAYBACKGROUP_VIDEO_CODEC_MPEG1				= 1,		/**< MPEG-1 Video (ISO/IEC 11172-2) */
	PLAYBACKGROUP_VIDEO_CODEC_MPEG2				= 2, 	/**< MPEG-2 Video (ISO/IEC 13818-2) */
	PLAYBACKGROUP_VIDEO_CODEC_MPEG4Part2		= 3,		/**< MPEG-4 Part 2 Video */
	PLAYBACKGROUP_VIDEO_CODEC_H261				= 4,		/**< H.261 Video. The value of the enum is not based on PSI standards. */
	PLAYBACKGROUP_VIDEO_CODEC_H263				= 5,		/**< H.263 Video. The value of the enum is not based on PSI standards. */
	PLAYBACKGROUP_VIDEO_CODEC_H264				= 6,		/**< H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	PLAYBACKGROUP_VIDEO_CODEC_H264_Svc			= 7, 	/**< Scalable Video Codec extension of H.264  */
	PLAYBACKGROUP_VIDEO_CODEC_H264_Mvc			= 8, 	/**< Multi View Coding extension of H.264 */
	PLAYBACKGROUP_VIDEO_CODEC_VC1				= 9, 	/**< VC-1 Advanced Profile */
	PLAYBACKGROUP_VIDEO_CODEC_VC1_SimpleMain	= 10, 	/**< VC-1 Simple & Main Profile */
	PLAYBACKGROUP_VIDEO_CODEC_AVS				= 11,	/**< AVS coded video */
	PLAYBACKGROUP_VIDEO_CODEC_RV40				= 12,	/**< RV 4.0 coded video */
	PLAYBACKGROUP_VIDEO_CODEC_VP6				= 13,	/**< VP6 coded video */
	PLAYBACKGROUP_VIDEO_CODEC_DIVX311			= 14,	/**< DivX 3.11 coded video */
	PLAYBACKGROUP_VIDEO_CODEC_XVID				= 15,	/**< XVid coded video */
	PLAYBACKGROUP_VIDEO_CODEC_Max
} PlaybackGroup_VideoCodec_e;

typedef enum
{
	PLAYBACKGROUP_AUDIO_CODEC_UNKNOWN 				= 0,	/* unknown/not supported audio format */
	PLAYBACKGROUP_AUDIO_CODEC_MPEG					= 1,	/* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
	PLAYBACKGROUP_AUDIO_CODEC_MP3					= 2,	/* MPEG1/2, layer 3. */
	PLAYBACKGROUP_AUDIO_CODEC_DOLBY_AC3				= 3,	/* Dolby Digital AC3 audio */
	PLAYBACKGROUP_AUDIO_CODEC_DOLBY_AC3P 			= 4,	/* Dolby Digital Plus (AC3+ or DDP) audio */
	PLAYBACKGROUP_AUDIO_CODEC_PCM					= 5,	/* PCM audio - Generally used only with inputs such as SPDIF or HDMI. */
	PLAYBACKGROUP_AUDIO_CODEC_AAC					= 6,	/* Advanced audio coding. Part of MPEG-4 */
	PLAYBACKGROUP_AUDIO_CODEC_AAC_LOAS				= 7,	/* Advanced audio coding. Part of MPEG-4 */
	PLAYBACKGROUP_AUDIO_CODEC_AAC_PLUS				= 8,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) with ADTS (Audio Data Transport Format) */
	PLAYBACKGROUP_AUDIO_CODEC_AAC_PLUS_ADTS			= 9,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) sync and LATM mux */
	PLAYBACKGROUP_AUDIO_CODEC_DTS					= 10,	/* Digital Digital Surround sound, uses non-legacy frame-sync */
	PLAYBACKGROUP_AUDIO_CODEC_DTS_HD				= 11,	/* Digital Digital Surround sound, HD, uses non-legacy frame-sync, decodes only DTS part of DTS-HD stream */
	PLAYBACKGROUP_AUDIO_CODEC_LPCM_DVD				= 12,	/* LPCM, DVD mode */
	PLAYBACKGROUP_AUDIO_CODEC_LPCM_HD_DVD			= 13,	/* LPCM, HD-DVD mode */
	PLAYBACKGROUP_AUDIO_CODEC_LPCM_BLUERAY			= 14,	/* LPCM, Blu-Ray mode */
	PLAYBACKGROUP_AUDIO_CODEC_WMA_STD				= 15,	/* WMA Standard */
	PLAYBACKGROUP_AUDIO_CODEC_WMA_PRO				= 16,	/* WMA Professional */
	PLAYBACKGROUP_AUDIO_CODEC_PCMWAVE				= 17,	/* PCM audio with Wave header - Used with streams containing PCM audio */
	PLAYBACKGROUP_AUDIO_CODEC_DRA					= 18,   /* Dynamic Resolution Adaptation.  Used in Blu-Ray and China Broadcasts. */
	PLAYBACKGROUP_AUDIO_CODEC_AIFF					= 19,
	PLAYBACKGROUP_AUDIO_CODEC_Max
} PlaybackGroup_AudioCodec_e;

class PlaybackGroupNative : public IPlaybackGroup
{
public:
    virtual ~PlaybackGroupNative();
    PlaybackGroupNative(ESManagerNative& ESManager, uint32_t pipelineId);

    virtual netflix::NFErr
    createStreamPlayer(const struct StreamPlayerInitData& initData,
                       shared_ptr<IESPlayerCallback> callback,
                       IElementaryStreamPlayer*& pStreamPlayer);
    virtual void destroyStreamPlayer(IElementaryStreamPlayer* streamPlayer);
    virtual bool setPlaybackState(PlaybackState state);
    virtual IPlaybackGroup::PlaybackState getPlaybackState();
    virtual void flush();
    virtual Rect setVideoWindow(const Rect& targetRect, uint32_t transitionDuration); // to be deprecated
    virtual Rect setVideoWindow(const Rect& targetRect, uint32_t transitionDuration, uint32_t zorder = 0);
    virtual void bringVideoToFront();
    virtual void sendVideoToBack();

    ReferenceClockNative* getReferenceClock();
    ESManagerNative* getESManager();

    // The audio player, if it is flushed independent of the playback group, calls
    // this method so that the reference clock's current audio presentation time
    // can be invalidated.
    void audioFlushed();
    uint32_t getPipelineId() const;
    void audioCodecSwitch(int32_t nCsoAudioCodec);
    bool getWaitForSeekFlag();
    void setWaitForSeekFlag(bool bFlag);

    static PlaybackGroupNative* getPlaybackGroup();

    static void MutexLock()
    {
        int32_t nRet = 0;

        nRet = pthread_mutex_lock(&s_mtxPlaybackGroup);
        if(nRet)
        {
            NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to lock s_mtxPlaybackGroup, nRet = %d\n", __func__, __LINE__, nRet);
        }
    }
    
    static void MutexUnlock()
    {
        int32_t nRet = 0;
        
        nRet = pthread_mutex_unlock(&s_mtxPlaybackGroup);
        if(nRet)
        {
            NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to unlock s_mtxPlaybackGroup, nRet = %d\n", __func__, __LINE__, nRet);
        }
    }

	bool m_bPlaybackStopped;
	
private:
    bool streamPlayersAreReadyForPlaybackStart();

    bool         avOpen();
    bool         avClose();
    bool         csoOpen();
    bool         csoClose();
    bool         csoProbe();
    bool         csoResume();
    bool         getResourceMap();
    void         setPumpIndex(uint32_t unIndex);
    uint32_t     getPumpIndex();
    void         setCurrentAudioTrack(uint32_t unCurrentAudioTrack);
    uint32_t     getCurrentAudioTrack();
	void 		 setNeedToFlushAudio(bool bNeedToFlushAudio);
	bool 		 getNeedToFlushAudio();

    PlaybackGroup_AudioCodec_e convertAudioCodecFromCsoToPlaybackGroup(CSO_ACODEC nAudioCodec);
    PlaybackGroup_VideoCodec_e convertVideoCodecFromCsoToPlaybackGroup(CSO_VCODEC nVideoCodec);

    static void     onPlaytimeChanged(void *handle);
    static void     onPlayStateChanged(void *handle);
    static void     onPlaypumpIndexChanged(void *handle);
    static void     onSeekReady(void *handle);
    static void     onAudioCodecSwitched(void *handle);
	static void 	onBufferStateChanged(void *handle);
	static void		onAudioFadeCompleted(void *handle);
	static void		onAudioFlushCompleted(void *handle);
	static void		onAudioRestartCompleted(void *handle);
	static void 	onAudioFadeFailed(void *handle);
	static void 	onAudioDecoderVolumeChanged(void *handle);

    static void     onOctoEventCallback(void *handle, int nEventType);
    static void     onCsoEventCallback(unsigned int session_id, int evt, long long int param);
    static void     onCsoEventEndOfStream();
    static void     onCsoEventAudioFlush(uint32_t unTryToChangeAudioTrack);
    static void     onCsoEventAudioRestart();
    static void     onCsoEventProbeComplete();
    
    // The video audio rendering threads access mPlaybackState. The state is
    // set in and SDK thread. The mPlaybackStateMutex should be held when
    // mPlaybackState is accessed.
    IPlaybackGroup::PlaybackState  		mPlaybackState;
    Mutex                    			mPlaybackStateMutex;
    Mutex                    			mWaitForEventMutex;

    std::auto_ptr<ReferenceClockNative> mReferenceClock;
    ESManagerNative&                    mESManager;
    std::set<ESPlayerNative*>           mStreamPlayers;

    AVStatusManager                     *m_hStatusManager;
    uint32_t                            m_session_id;
    PlaybackGroupNativeResourceMap_t    m_stResourceMap;
    uint32_t                            m_unPumpIndex;
    uint32_t                            m_unCurrentAudioTrack;

    static PlaybackGroupNative          *s_hPlaybackGroup;
    static bool                         s_bWaitForSeek;
    static pthread_mutex_t              s_mtxPlaybackGroup;
    uint32_t 							mPipelineId;
	bool								m_bNeedToFlushAudio;
};

} // namespace esplayer
} // device
} // namespace netflix



#endif

