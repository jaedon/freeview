
#ifndef __OPLVODHANDLE_DEF__
#define __OPLVODHANDLE_DEF__


/****************************************************************************************
 * Enum
 ***************************************************************************************/

typedef enum
{
	OPLVOD_PLAYER_TYPE_NONE           = 0,
	OPLVOD_PLAYER_TYPE_HMX,
	OPLVOD_PLAYER_TYPE_MSE,
	OPLVOD_PLAYER_TYPE_ENDOFCASE
} eOplVodPlayerType;

typedef enum
{
	OPLVOD_VIDEO_CODEC_UNKNOWN,
	OPLVOD_VIDEO_CODEC_MPEG1,			/**< MPEG-1 Video (ISO/IEC 11172-2) */
	OPLVOD_VIDEO_CODEC_MPEG2, 			/**< MPEG-2 Video (ISO/IEC 13818-2) */
	OPLVOD_VIDEO_CODEC_MPEG4_PART2,			/**< MPEG-4 Part 2 Video */
	OPLVOD_VIDEO_CODEC_H261,			/**< H.261 Video. The value of the enum is not based on PSI standards. */
	OPLVOD_VIDEO_CODEC_H263,			/**< H.263 Video. The value of the enum is not based on PSI standards. */
	OPLVOD_VIDEO_CODEC_H264,			/**< H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	OPLVOD_VIDEO_CODEC_H264_SVC, 		/**< Scalable Video Codec extension of H.264  */
	OPLVOD_VIDEO_CODEC_H264_MVC, 		/**< Multi View Coding extension of H.264 */
	OPLVOD_VIDEO_CODEC_VC1, 			/**< VC-1 Advanced Profile */
	OPLVOD_VIDEO_CODEC_VC1_SIMPLEMAIN,	/**< VC-1 Simple & Main Profile */
	OPLVOD_VIDEO_CODEC_AVS,				/**< AVS coded video */
	OPLVOD_VIDEO_CODEC_RV40,			/**< RV 4.0 coded video */
	OPLVOD_VIDEO_CODEC_VP6,				/**< VP6 coded video */
	OPLVOD_VIDEO_CODEC_DIVX311,			/**< DivX 3.11 coded video */
	OPLVOD_VIDEO_CODEC_XVID,			/**< XVid coded video */
	OPLVOD_VIDEO_CODEC_VP8,
	OPLVOD_VIDEO_CODEC_VP9,
	OPLVOD_VIDEO_CODEC_MAX_NUM
} eOplVodVideoCodec;

typedef enum
{
	OPLVOD_AUDIO_CODEC_UNKNOWN,			/* unknown/not supported audio format */
	OPLVOD_AUDIO_CODEC_MPEG,			/* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
	OPLVOD_AUDIO_CODEC_MP3,				/* MPEG1/2, layer 3. */
	OPLVOD_AUDIO_CODEC_DOLBY_AC3,		/* Dolby Digital AC3 audio */
	OPLVOD_AUDIO_CODEC_DOLBY_AC3P,		/* Dolby Digital Plus (AC3+ or DDP) audio */
	OPLVOD_AUDIO_CODEC_PCM,				/* PCM audio - Generally used only with inputs such as SPDIF or HDMI. */
	OPLVOD_AUDIO_CODEC_AAC,				/* Advanced audio coding. Part of MPEG-4 */
	OPLVOD_AUDIO_CODEC_AAC_LOAS,		/* Advanced audio coding. Part of MPEG-4 */
	OPLVOD_AUDIO_CODEC_AAC_PLUS,		/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) with ADTS (Audio Data Transport Format) */
	OPLVOD_AUDIO_CODEC_AAC_PLUS_ADTS,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) sync and LATM mux */
	OPLVOD_AUDIO_CODEC_DTS,				/* Digital Digital Surround sound, uses non-legacy frame-sync */
	OPLVOD_AUDIO_CODEC_DTS_HD,			/* Digital Digital Surround sound, HD, uses non-legacy frame-sync, decodes only DTS part of DTS-HD stream */
	OPLVOD_AUDIO_CODEC_LPCM_DVD,		/* LPCM, DVD mode */
	OPLVOD_AUDIO_CODEC_LPCM_HD_DVD,		/* LPCM, HD-DVD mode */
	OPLVOD_AUDIO_CODEC_LPCM_BLUERAY,	/* LPCM, Blu-Ray mode */
	OPLVOD_AUDIO_CODEC_WMA_STD,			/* WMA Standard */
	OPLVOD_AUDIO_CODEC_WMA_PRO,			/* WMA Professional */
	OPLVOD_AUDIO_CODEC_PCMWAVE,			/* PCM audio with Wave header - Used with streams containing PCM audio */
	OPLVOD_AUDIO_CODEC_DRA,				/* Dynamic Resolution Adaptation.  Used in Blu-Ray and China Broadcasts. */
	OPLVOD_AUDIO_CODEC_AIFF,
	OPLVOD_AUDIO_CODEC_RIFF,			/* *.wav */
    OPLVOD_AUDIO_CODEC_OPUS,
	OPLVOD_AUDIO_CODEC_MAX_NUM
} eOplVodAudioCodec;

typedef enum {
	OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED            = 0,
	OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED,
	OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED,
	OPLVOD_EVENT_TYPE_PLAY_DURATION_CHANGED,
	OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED,
	OPLVOD_EVENT_TYPE_BUFFER_INFO_CHANGED,
	OPLVOD_EVENT_TYPE_FULLSCREEN_CHANGED,
	OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED,
	OPLVOD_EVENT_TYPE_UNSELECTED_COMPONENT_CHANGED,
	OPLVOD_EVENT_TYPE_PROGRAMME_CHANGED,
	OPLVOD_EVENT_TYPE_BML_START,
	OPLVOD_EVENT_TYPE_BML_END,
	OPLVOD_EVENT_TYPE_PARENTAL_RATING_CHANGE,
	OPLVOD_EVENT_TYPE_DRM_RIGHT_ERROR,
	OPLVOD_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED,
	OPLVOD_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED,
	OPLVOD_EVENT_TYPE_VIDEOWINDOW_CHANGED,
	OPLVOD_EVENT_TYPE_AUDIO_OUTPUT_RESTRICT,
	OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED		= 0xF0,
	OPLVOD_EVENT_TYPE_SEEK_READY,
	OPLVOD_EVENT_TYPE_UNKWON                    = 0xff,
} eOplVodEventType;

typedef enum {
	OPLVOD_PLAY_STATE_STOPPED       = 0,
	OPLVOD_PLAY_STATE_PLAYING,
	OPLVOD_PLAY_STATE_PAUSED,
	OPLVOD_PLAY_STATE_CONNECTING,
	OPLVOD_PLAY_STATE_BUFFERING,
	OPLVOD_PLAY_STATE_FINISHED,
	OPLVOD_PLAY_STATE_ERROR,

	OPLVOD_PLAY_STATE_USER_DEFINE	= 100,
	OPLVOD_PLAY_STATE_PROBE_FAILED,
	OPLVOD_PLAY_STATE_BOF
} eOplVodPlayState;

typedef enum {
	OPLVOD_BUFFER_STATE_NONE,
	OPLVOD_BUFFER_STATE_CANPLAY,
	OPLVOD_BUFFER_STATE_NOT_ENOUGH,
	OPLVOD_BUFFER_STATE_ENOUGH,
	OPLVOD_BUFFER_STATE_FINISHED
} eOplVodBufferState;

typedef enum {
	OPLVOD_PLAY_ERROR_NO_ERROR					= -1,
	OPLVOD_PLAY_ERROR_NOT_SUPPORTED_FORMAT		= 0,
	OPLVOD_PLAY_ERROR_SERVER_FAIL,
	OPLVOD_PLAY_ERROR_UNDEFINED,
	OPLVOD_PLAY_ERROR_INSUFFICIENT_RESOURCE,
	OPLVOD_PLAY_ERROR_CONTENT_CORRUPT,
	OPLVOD_PLAY_ERROR_CONTENT_NOT_AVAILABLE,
	OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION,
	OPLVOD_PLAY_ERROR_BLOCK_PARENTAL_CONTROL,
	OPLVOD_PLAY_ERROR_DRM,

	OPLVOD_PLAY_ERROR_USER_DEFINE				= 100,
	OPLVOD_PLAY_ERROR_PARENTAL_CONTROL
} eOplVodPlayError;

typedef enum {
	/* no license, consumption of the content is blocked. */
	OPLVOD_DRM_RIGHTS_NO_LICENSE		= 0,
	/* invalid license, consumption of the content is blocked. */
	OPLVOD_DRM_RIGHTS_INVALID_LICENSE,
	/* valid license, consumption of the content is unblocked. */
	OPLVOD_DRM_RIGHTS_VALID_LICENSE,
} eOplVodDrmRightsError;

/* for Maxdome */
typedef enum {
	OPLVOD_PLAY_ERRORDETAILS_UNKNOWN_ERROR					= -1,			/* unknown error  */
	OPLVOD_PLAY_ERRORDETAILS_APPLICATION_START_PLAYER_ERROR		= 1000,		/* error in initializing/starting the native player( e.g. firmware specific implementation error  */
	OPLVOD_PLAY_ERRORDETAILS_VIDEO_NOT_SUPPORTED_ERROR		= 1100,			/* error rendering the video content (e.g. not supported video format, codec, resolution) */
	OPLVOD_PLAY_ERRORDETAILS_VIDEO_CONTENTS_ERROR,							/* error rendering the video content (e.g. error in content )  */
	OPLVOD_PLAY_ERRORDETAILS_VIDEO_LEVEL_SWITCHING_ERROR,					/* error while switching between smooth streaming levels */
	OPLVOD_PLAY_ERRORDETAILS_NETWORK_REACHING_ERROR			= 1200,			/* error reaching the streaming server */
	OPLVOD_PLAY_ERRORDETAILS_NETWORK_VALID_RESPONSE_ERROR,					/* error to get valid response from the streaming server */
	OPLVOD_PLAY_ERRORDETAILS_NETWORK_GET_FILE_ERROR,						/* error to get the file from the streaming server */
	OPLVOD_PLAY_ERRORDETAILS_NETWORK_GET_CHUNK_ERROR,						/* error to get video chunks while playback */
	OPLVOD_PLAY_ERRORDETAILS_DRM_REACHING_ERROR				= 1300,			/* error reaching the DRM server */
	OPLVOD_PLAY_ERRORDETAILS_DRM_VALID_RESPONSE_ERROR,						/* error to get valid response from DRM server */
	OPLVOD_PLAY_ERRORDETAILS_DRM_VALID_LICENSE_ERROR,						/* error to get a valid DRM license */
	OPLVOD_PLAY_ERRORDETAILS_DRM_HANDLE_LICENSE_ERROR,						/* error to handle the DRM license */
} eOplVodPlayErrorDetails;

typedef enum {
	OPLVOD_TRICK_MODE_NORESTRICT = 0,
	OPLVOD_TRICK_MODE_NOSKIP_FF2X = 1,
	OPLVOD_TRICK_MODE_NOSKIP_FF4X = 2,
	OPLVOD_TRICK_MODE_NOSKIP_FF8X = 3,
	OPLVOD_TRICK_MODE_NOSKIP_JUMP_FF = 7
} OplVodTrickRestrictMode_e;	/* SES extension */

typedef enum {
	OPLVOD_DOWNLOAD_STATE_NONE,
	OPLVOD_DOWNLOAD_STATE_CREATE,
	OPLVOD_DOWNLOAD_STATE_STARTED,
	OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED,
	OPLVOD_DOWNLOAD_STATE_PAUSE,
	OPLVOD_DOWNLOAD_STATE_COMPLETED,
} eOplVodDownloadState;

typedef enum {
	OPLVOD_STORAGE_RIGHT_NORESTRICT = 0,
	OPLVOD_STORAGE_RIGHT_NOMORE90MIN = 1,
	OPLVOD_STORAGE_RIGHT_NORECORDING = 2,
	OPLVOD_STORAGE_RIGHT_EXPIRE_SAMEDAY = 3,
	OPLVOD_STORAGE_RIGHT_EXPIRE_3DAY = 4,
	OPLVOD_STORAGE_RIGHT_EXPIRE_7DAY = 5,
	OPLVOD_STORAGE_RIGHT_EXPIRE_60DAY = 6,
} OplVodStorageRight_e;	/* SES extension */

typedef struct {
	unsigned int *playpump_list;
	unsigned int *dma_list;
	unsigned int num_of_playpump;
	unsigned int num_of_dma;
} OplVodResourceMap;

typedef struct {
	eOplVodVideoCodec video_codec;
	eOplVodAudioCodec audio_codec;
	unsigned int video_pid;
	unsigned int audio_pid;
} OplVodProbeInfo;

typedef struct {
	unsigned long ullBufferedBytes;
	unsigned long ullTotalBytesRemaining;
	unsigned long ullTotalStreamBytes;
	int nArrivalBytedPerSec;
	int nStreamBytesPerSec;
	int nBufferPercentage;
	int nRemainBufferSec;
} OplDownloadStatus_t;

typedef void (*OplVodListener_t)(void *hPlayer, void *hVodHandle, eOplVodEventType eEventType);

class IMediaPlayerListener
{
public:
	virtual void playStateChanged(const eOplVodEventType& eEventType) = 0;
};

#endif // __OPLVODHANDLE_DEF__
