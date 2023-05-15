/**
 * @file		di_media.h
 * @brief		Media module DI layer header file
 * @version	$Revision: 0.9.0 $
 * @date 	$Date: 2009/11/02 16:00:00 $
 * @author	humax di project members
 */

/** @defgroup	DI_MEDIA	DI_MEDIA MODULE
 *  This is MEDIA module of DI layer.
 *  @{
 */


#ifndef	__DI_MEDIA_H__
#define	__DI_MEDIA_H__

#include <htype.h>
#include <di_err.h>

#include <di_video.h>
#include <di_audio.h>
#include <di_media_types.h>

#define	DI_MEDIA_DATA_ARRAY_NUM			(10)
#define	DI_MEDIA_VIDEO_PES_ID			(0xE0)	/* 0b 1110 xxxx */
#define	DI_MEDIA_AUDIO_PES_ID			(0xC0)	/* 0b 1100 xxxx */
#define	DI_MEDIA_VIDEO_PID			(0x100)
#define	DI_MEDIA_AUDIO_PID			(0x101)

#define	DI_MEDIA_MAX_NUM				(1)
#define DI_MEDIA_DIVX_MAX_VIDEO_TRACK	8
#define DI_MEDIA_DIVX_MAX_AUDIO_TRACK	8
#define DI_MEDIA_DIVX_MAX_XSUB_TRACK	8
#define DI_MEDIA_CRYPTO_KEY_LEN 16

#define DI_MEDIA_DRMINFO_MAXNUM (2)

#define	DI_MEDDIA_AVI_CMD_BUFSIZE 256

#define  DI_MEDIA_FRAMERATE_LENGTH	(8)

#define DI_MEDIA_PLAYSPEED_MAX_COUNT 128

#if 1 //-> debug 1222
typedef enum MEDIA_PLAYER_TYPE
{
	DI_MEDIA_TYPE_UNKNOWN,
	DI_MEDIA_TYPE_AUDIO,
	DI_MEDIA_TYPE_VIDEO,
	DI_MEDIA_TYPE_IMAGE
}DI_MEDIA_PLAYER_TYPE;

typedef enum
{
	eMedia_MP3		= 0,
	eMedia_JPEG,
	eMedia_TYPE_MAX
} DI_MEDIA_TYPE_e;

typedef struct
{
	HUINT32			x;
	HUINT32			y;
	HUINT32			w;
	HUINT32			h;
} DI_MEDIA_RECT_t;

typedef enum
{
	eMedia_Rotate_0			= 0,
	eMedia_Rotate_90,
	eMedia_Rotate_180,
	eMedia_Rotate_270
} DI_MEDIA_IMG_ROTATE_e;

typedef enum
{
	DI_EVT_MP_INITED			= 0x10100000,		// MP module이 초기화 됨
	DI_EVT_MP_ERROR,								// decoding processing중 오류 발생
	DI_EVT_MP_IDLE,								// decoding processing이 없는 상태

	/* for MP3 decoding */
	DI_EVT_MP3_PLAYING,							// decoding 중인 상태
	DI_EVT_MP3_PLAYDONE,							// decoding 완료 상태의 notification
	DI_EVT_MP3_PAUSED,							// decoding paused 상태
	DI_EVT_MP3_ELAPSED,							// playback 진행 상태
													// callback의 parameter로 elapsed time 전달

	/* for JPEG decoding */
	DI_EVT_JPG_DECODE_DONE,						// image decoding 완료
													// callback parameter로 scaled된 image resolution 전달
	DI_EVT_JPG_DECODE_FAIL,						// image decoding 실패

	DI_EVT_MP_MAX
} DI_MEDIA_EVENT_t;

typedef struct
{
	HUINT32			width;					// basic image dimension
	HUINT32			height;

	//HUINT32			horiz_res;				// basic image resolution
	//HUINT32			vert_res;

	HUINT32			color_model; // DPS

	// EXIF information

} DI_MEDIA_IMG_INFO_t;

typedef struct
{
	int version;				/**< MPEG version */
	int protection;			/**< true if the frame is protected by CRC */
	int bitrate;				/**< bitrate in Kbps */
	int sample_rate;			/**< sample rate in Hz */
	int padding;				/**< true if the frame is padded */
	int channel_mode;		/**< channel mode */
	int header_offset;		/**< mp3 header offset */

	int frame_size;			/**< size of the frame in bytes */
	int samples_per_frame;	/**< number of audio samples in the frame */

	/* From Xing VBR header for VBR files, From file size for CBR files. */
	int number_of_frames;	/**< number of frames */
	int data_size;			/**< data size. */
	int vbr_bitrate;			/**< bitrate in Kbps */

	int duration;

	//ID3Tag	*pId3Tag;		/**< ID3 Tag info */
} DI_MEDIA_MP3_INFO_t;
#endif

#if 0 //-> debug 1238
#define _MAX_COMMENT 2000
#define _MAX_DATE_COPIES     10

typedef struct
{
	HUINT32 jpeg_handle;
	char  CameraMake   [32];
	char  CameraModel  [40];
	char  DateTime     [20];
	int   Height, Width;
	int   Orientation;
	int   IsColor;
	int   Process;
	int   FlashUsed;
	float FocalLength;
	float ExposureTime;
	float ApertureFNumber;
	float Distance;
	float CCDWidth;
	float ExposureBias;
	float DigitalZoomRatio;
	int   FocalLength35mmEquiv; // Exif 2.2 tag - usually not present.
	int   Whitebalance;
	int   MeteringMode;
	int   ExposureProgram;
	int   ExposureMode;
	int   ISOequivalent;
	int   LightSource;
	char  Comments[_MAX_COMMENT];

	unsigned ThumbnailOffset;          // Exif offset to thumbnail
	unsigned ThumbnailSize;            // Size of thumbnail.
	unsigned LargestExifOffset;        // Last exif data referenced (to check if thumbnail is at end)
	int ThumbnailDataRead;
	int ThumbnailDataLeft;
	char  ThumbnailAtEnd;              // Exif header ends with the thumbnail
								// (we can only modify the thumbnail if its at the end)
	int   ThumbnailSizeOffset;

	int  DateTimeOffsets[_MAX_DATE_COPIES];
	int  numDateTimeTags;

	int GpsInfoPresent;
	char GpsLat[31];
	char GpsLong[31];
	char GpsAlt[20];
	int32_t JpegOrExif; //0 ==jpeg 1 ==EXif
	int32_t tWd;
	int32_t tHt;
}DI_MEDIA_EXIF;
#endif

typedef enum
{
	DI_MEDIA_TIME_SEC,
	DI_MEDIA_TIME_MSEC,
	DI_MEDIA_TIME_USEC
}DI_MEDIA_TIME_TYPE;

typedef enum
{
	DI_MEDIA_PLAY_START_MODE_BEGIN,
	DI_MEDIA_PLAY_START_MODE_LASTVIEW,
	DI_MEDIA_PLAY_START_MODE_END
}	DI_MEDIA_PLAY_START_MODE;

/**
 * Define media Event Type
 */
typedef enum
{
	DI_MEDIA_EVENT_BEGINOFSTREAM	= 0,
	DI_MEDIA_EVENT_ERROROFSTREAM,
	DI_MEDIA_EVENT_ENDOFSTREAM,
	DI_MEDIA_EVENT_CHANGEPLAYTIME,
	DI_MEDIA_EVENT_START,
	DI_MEDIA_EVENT_PAUSE,
	DI_MEDIA_EVENT_PLAYING,
	DI_MEDIA_EVENT_TRICK,
	DI_MEDIA_EVENT_STOP
} DI_MEDIA_Event_t;

typedef enum
{
	DI_MEDIA_TYPE_MP3	= 0,
	DI_MEDIA_TYPE_AVI,
	DI_MEDIA_TYPE_MAX
} DI_MEDIA_MP_TYPE_e;

/**
 * Define Timer Mode
 */
typedef enum
{
	DI_MEDIA_TIMERMODE_NONE					= 0x0,
	DI_MEDIA_TIMERMODE_UPDATE_FULLTIME,	/** Update full time */
	DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0	/** Update time except 0 */
}DI_MEDIA_TIMERMODE_e;

/**
 * Define video codec for Media Info
 */
typedef enum
{
	DI_MEDIA_VIDEOCODEC_NONE					= 0x0,
	DI_MEDIA_VIDEOCODEC_MPEG1,	/**< MPEG1 Video */
	DI_MEDIA_VIDEOCODEC_MPEG2,	/**< MPEG2 Video */
	DI_MEDIA_VIDEOCODEC_MPEG4Part2,	/**< MPEG4 Part2 Video */
	DI_MEDIA_VIDEOCODEC_H261,	/**< H.261 Video */
	DI_MEDIA_VIDEOCODEC_H263,	/**< H.263 Video */
	DI_MEDIA_VIDEOCODEC_H264,	/**< H.264 Video */
	DI_MEDIA_VIDEOCODEC_VC1,	/**< VC1 Video */
	DI_MEDIA_VIDEOCODEC_AVS,	/**< AVS Video */
	DI_MEDIA_VIDEOCODEC_XVID,
	DI_MEDIA_VIDEOCODEC_DIVX
}DI_MEDIA_VIDEOCODEC_e;

/**
 * Define Audio codec for Media Info
 */
typedef enum
{
	DI_MEDIA_AUDIOCODEC_NONE					= 0x0,
	DI_MEDIA_AUDIOCODEC_MPEG,
	DI_MEDIA_AUDIOCODEC_MP3,
	DI_MEDIA_AUDIOCODEC_AAC,
	DI_MEDIA_AUDIOCODEC_AAC_PLUS,
	DI_MEDIA_AUDIOCODEC_AC3,
	DI_MEDIA_AUDIOCODEC_AC3_PLUS,
	DI_MEDIA_AUDIOCODEC_PCM,
    DI_MEDIA_AUDIOCODEC_DTS,            /* Digital Digital Surround sound. */
    DI_MEDIA_AUDIOCODEC_LPCM,
    DI_MEDIA_AUDIOCODEC_WMA_STD,         /* WMA Standard */
    DI_MEDIA_AUDIOCODEC_WMA_PRO,         /* WMA Professional */
    DI_MEDIA_AUDIOCODEC_FLAC         /* FLAC */
}DI_MEDIA_AUDIOCODEC_e;

/**
 * Define media Trick Mode
 */
typedef enum
{
	DI_MEDIA_TRICK_PAUSE,
	DI_MEDIA_TRICK_1RWSTEP,
	DI_MEDIA_TRICK_2RWSTEP,
	DI_MEDIA_TRICK_3RWSTEP,
	DI_MEDIA_TRICK_4RWSTEP,
	DI_MEDIA_TRICK_5RWSTEP,
	DI_MEDIA_TRICK_6RWSTEP,
	DI_MEDIA_TRICK_7RWSTEP,
	DI_MEDIA_TRICK_8RWSTEP,
	DI_MEDIA_TRICK_9RWSTEP,
	DI_MEDIA_TRICK_10RWSTEP,
	DI_MEDIA_TRICK_11RWSTEP,
	DI_MEDIA_TRICK_12RWSTEP,		/* Rewind 12배속 */
	DI_MEDIA_TRICK_13RWSTEP,		/* Rewind 13배속 */
	DI_MEDIA_TRICK_14RWSTEP,		/* Rewind 16배속 */
	DI_MEDIA_TRICK_15RWSTEP,		/* Rewind 32배속 */
	DI_MEDIA_TRICK_16RWSTEP,		/* Rewind 64배속 */
	DI_MEDIA_TRICK_1FFSTEP,
	DI_MEDIA_TRICK_2FFSTEP,
	DI_MEDIA_TRICK_3FFSTEP,
	DI_MEDIA_TRICK_4FFSTEP,
	DI_MEDIA_TRICK_5FFSTEP,
	DI_MEDIA_TRICK_6FFSTEP,
	DI_MEDIA_TRICK_7FFSTEP,
	DI_MEDIA_TRICK_8FFSTEP,
	DI_MEDIA_TRICK_9FFSTEP,
	DI_MEDIA_TRICK_10FFSTEP,
	DI_MEDIA_TRICK_11FFSTEP,
	DI_MEDIA_TRICK_12FFSTEP,
	DI_MEDIA_TRICK_13FFSTEP,		/* Forward 13배속 */
	DI_MEDIA_TRICK_14FFSTEP,		/* Forward 16배속 */
	DI_MEDIA_TRICK_15FFSTEP,		/* Forward 32배속 */
	DI_MEDIA_TRICK_16FFSTEP			/* Forward 64배속 */
} DI_MEDIA_TrickSpeed_e;

/**
 * Define media stream type
 */
typedef	enum
{
    DI_MEDIA_MPEG_TYPE_UNKNOWN,
    DI_MEDIA_MPEG_TYPE_ES,        /* Elementary stream */
    DI_MEDIA_MPEG_TYPE_BES,       /* Broadcom elementary stream */
    DI_MEDIA_MPEG_TYPE_PES,       /* Packetized elementary stream */
    DI_MEDIA_MPEG_TYPE_TS,        /* Transport stream(188) */
    DI_MEDIA_MPEG_TYPE_TS_192,    /* Transport stream(192) */
    DI_MEDIA_MPEG_TYPE_DSS_ES,    /* DSS with ES payload (used for SD) */
    DI_MEDIA_MPEG_TYPE_DSS_PES,   /* DSS with PES payload (used for HD) */
    DI_MEDIA_MPEG_TYPE_VOB,       /* video object, used with DVD */
    DI_MEDIA_MPEG_TYPE_ASF,       /* Advanced Systems Format */
    DI_MEDIA_MPEG_TYPE_AVI,       /* Audio Video Interleave */
    DI_MEDIA_MPEG_TYPE_MPEG1,     /* MPEG1 System Stream */
    DI_MEDIA_MPEG_TYPE_MP4,       /* MP4 (MPEG-4 Part12) container */
	DI_MEDIA_MPEG_TYPE_MP4FRAGMENT, /* Fragmented MP4 Container */ /* for SSTR ykkim */    
    DI_MEDIA_MPEG_TYPE_FLV,       /* Flash video container */
	DI_MEDIA_MPEG_TYPE_MKV,        /* Matroska container */
	DI_MEDIA_MPEG_TYPE_WAVE			/* WAVE audio container */
}DI_MEDIA_STREAM_TYPE_e;

/**
 * Define divx event type
 */
typedef enum
{
	DI_MEDIA_DIVX_EVENT_AUTH_ERROR = 0,			/**< Authorization Error */
	DI_MEDIA_DIVX_EVENT_RENTAL_EXPIRED,			/**< Rental Expired */
	DI_MEDIA_DIVX_EVENT_RENTAL_REMAINGS,		/**< Rental Remains */
	DI_MEDIA_DIVX_EVENT_XSUB,					/**< Xsub Callback */
	DI_MEDIA_DIVX_EVENT_MAX
}DI_MEDIA_DIVX_EVENT_e;


/**
 * Media Divx DRM Auth State
 */
typedef enum
{
	DI_MEDIA_DIVX_DRM_NOT_AUTHORIZED,
	DI_MEDIA_DIVX_DRM_AUTHORIZED,
	DI_MEDIA_DIVX_DRM_RENTAL_EXPIRED,
	DI_MEDIA_DIVX_DRM_RENTAL_CHOICE,
	DI_MEDIA_DIVX_DRM_MAX
}DI_MEDIA_DIVX_DRM_AUTH_STATUS_e;


/**
 * Media Divx Subtitle Type
 */
typedef enum
{
	DI_MEDIA_DIVX_XSUB_TYPE_DXSA,		/**< XSUB */
	DI_MEDIA_DIVX_XSUB_TYPE_DXSB,		/**< XSUB + */
	DI_MEDIA_DIVX_XSUB_TYPE_MAX,		/**< MAX*/
}DI_MEDIA_DIVX_XSUB_TYPE_e;


/**
 * Media DRM License status
 */
typedef enum
{
	DI_MEDIA_DRMENGINE_RESULT_OK,		
	DI_MEDIA_DRMENGINE_RESULT_UNKNOWN_ERROR,		
	DI_MEDIA_DRMENGINE_RESULT_DATA_INVALID,		
	DI_MEDIA_DRMENGINE_RESULT_LICENCE_INVALID,	
	DI_MEDIA_DRMENGINE_RESULT_LICENCE_EXPIRED,
}DI_MEDIA_DRM_LICENSE_TYPE_e;


/**
 * Media Divx DRM Information
 */
typedef struct DI_MEDIA_DivxDrmInfos_t_{
	HBOOL							bIsActivationStream;		/**< Is DRM Activation Stream */
	DI_MEDIA_DIVX_DRM_AUTH_STATUS_e eAuthStatus;  		/**< DRM Authorization Status */
	HUINT8							ucRentalUsed;		/**< Rental Used */
	HUINT8							ucRentalLimit;		/**< Rental Remains */
} DI_MEDIA_DivxDrmInfos_t;

typedef enum
{
	DI_MEDIA_ENCRYPTION_TYPE_NONE,
	DI_MEDIA_ENCRYPTION_TYPE_3DES,
	DI_MEDIA_ENCRYPTION_TYPE_AES,
	DI_MEDIA_ENCRYPTION_TYPE_CURRENT, /* Used for delayed record(or split) */
	DI_MEDIA_ENCRYPTION_TYPE_3DES_SECURED_KEY,
	DI_MEDIA_ENCRYPTION_TYPE_AES_SECURED_KEY
} DI_MEDIA_ENCRYPTION_TYPE;

typedef struct
{
	DI_MEDIA_ENCRYPTION_TYPE eEncType;
	HUINT8 ucKey[DI_MEDIA_CRYPTO_KEY_LEN];
	HUINT8 ucIv[DI_MEDIA_CRYPTO_KEY_LEN];
	HUINT8 ucProtectionKey[DI_MEDIA_CRYPTO_KEY_LEN];
	HUINT32	vidDescId;
	HUINT32	audDescId;
} DI_MEDIA_CRYPTO_SETTINGS;
/**
 * Define Media file information
 */

typedef struct DI_MEDIA_MEDIA_FILE_INFO_s {
	DI_MEDIA_STREAM_TYPE_e		stream_type;
	HUINT32						max_bitrate;		/**< maximum stream bitreate, bps or 0 if unknown  */
	HUINT32						duration;       	/**< duration of stream, in milliseconds or 0 if unknown */
	HUINT32						nprograms;      	/**< total number of programs */
	HUINT32						ntracks;        	/**< total number of tracks */
	HUINT32						video_pes_id;
	HUINT32						video_stream_id;    /**< or Video PID */
	DI_VIDEO_FORMAT					video_codec;
	HUINT16 					usVideoWidth;		/**< Video Width */
	HUINT16 					usVideoHeight;		/**< Video Height */

	HUINT8						ucTotalAudioTrack;	/**< Total Audio Track */
	struct {
		HUINT32 					audio_pes_id;
		HUINT32 					audio_stream_id;/**< or Audio PID */
		DI_MEDIA_AUDIO_FORMAT_e		audio_codec;
		HUINT8						uacAudioLang[256];
		HUINT32						sampleRate;
		HUINT32						bitsPerSample;
		HINT8						isStereo;
	}stAudioTrack[DI_MEDIA_DIVX_MAX_AUDIO_TRACK];

	HUINT8							ucTotalVideoTrack;	/**< Total Video Track */
	struct {
		HUINT32 					video_pes_id;
		HUINT32 					video_stream_id;/**< or Video PID */
		DI_VIDEO_FORMAT					video_codec;
	}stVideoTrack[DI_MEDIA_DIVX_MAX_VIDEO_TRACK];

	HUINT8							ucTotalXsubTrack; /**< Total Xsub Track Number */
	struct {
		DI_MEDIA_DIVX_XSUB_TYPE_e	eXsubType;			/**< Xsub Type */
		HUINT8						uacXsubLang[256];	/**< Xsub Language */
	}stXsubTrack[DI_MEDIA_DIVX_MAX_XSUB_TRACK];

	HUINT32						audio_pes_id;
	HUINT32						audio_stream_id;    /**< or Audio PID */
	DI_MEDIA_AUDIO_FORMAT_e		audio_codec;
	HUINT32						sampleRate;
	HUINT32						bitsPerSample;
	HINT8						isStereo;

	HUINT8						support_trick;
	HUINT8						support_seek;
	HUINT32						pcr_pid;        	/**< for TS stream */
	DI_MEDIA_AUDIO_TYPE_e		audiodecodertype;
	DI_MEDIA_VIDEO_TYPE_e		videodecodertype;
	DI_MEDIA_PLAY_START_MODE	eStartCondition;
	HUINT32						ulStartTime;		/**< Specify start time  */
	HINT32						demuxId;
	HINT32						is_drm;
	HUINT8						drminfo[DI_MEDIA_DRMINFO_MAXNUM];
	HUINT8						is_expired;
	DI_MEDIA_DivxDrmInfos_t		stDivxDrmInfo;

	HINT32						protoStatus;		/* Expansion of iptuner error status.(current http only) */
	HUINT8						isIcyProtocol;		/* HTTP Type(ICY? TRUE:FALSE) */
	HUINT64						stream_size;		/* Value for Ip Tuner(HTTP,MMS) 삭제 될 부분 */

	DI_MEDIA_CRYPTO_SETTINGS 	*pstDSC;
	//20110315-- for ctt range play
	HUINT32 					RangeAvaliable;	
} DI_MEDIA_INFO_t;

/**
 * Media Info
 */
typedef struct DI_MEDIA_MEDIAINFO_s {
	DI_MEDIA_VIDEOCODEC_e		video_codec;
	DI_MEDIA_AUDIOCODEC_e		audio_codec;
	HUINT32						width, height;	/* Video Resolution */
	HINT8						Video_FrameRate[DI_MEDIA_FRAMERATE_LENGTH];	/* Video FrameRate */
} DI_MEDIA_MEDIAINFO_t;


typedef	struct _DI_MEDIA_DivxCallabckData_t
{
	HUINT32					unMediaId;
	DI_MEDIA_DIVX_EVENT_e	eDivxEvent;
	HUINT32					ulLength;
	void					*pvData; /* Sequence Header Changed, Format Changed, Running State Changed */
}DI_MEDIA_DivxCallbackData_t;

typedef void	(*MediaPlayerEvtCb_t)(HUINT32 id, HUINT32 event, HUINT32 p1, HUINT32 p2);

/**
 * Define Callback Function
 */
typedef void (*DI_MEDIA_EventCallback_t)(HUINT32 deviceId, HINT32 ev, HUINT32 param1);

/**
 * Define Divx Event Callback Function
 */
typedef void (*DI_MEDIA_DivxEventCallback_t)(void *pvDivxEventData);

/**
 * Define Callback Function for License Data
 */
typedef void (*DI_MEDIA_LicenseDataCallback_t)(HINT8 *pParam1, HUINT32 unParam2);

typedef struct MEDIA_DLNA_FILECANCLE_TAG
{
	unsigned char cancel_code;

} DI_MEDIA_DLNA_FILECANCEL_OP  ;

typedef struct MEDIA_DLNA_FILEDESC_TAG
{
	char			*url;   // read only 로만 쓰시오
	long			access_type;
	long			linear_access;
	DI_MEDIA_DLNA_FILECANCEL_OP *cancel_op;
	long			option;
	unsigned char	user_data[32];
} DI_MEDIA_DLNA_FILEDESC;

typedef struct MEDIA_DLNA_FILEHANDLE_TAG
{
	void			*apis;
	DI_MEDIA_DLNA_FILEDESC desc;
	long			file_type;
	DI_MEDIA_DLNA_FILECANCEL_OP static_cancel_op;
	DI_MEDIA_DLNA_FILECANCEL_OP *cancel_op;
	long			error;
	long			eof;
	long			linear_access;
	long long		filesize;
	void			*extra_data;
} *DI_MEDIA_DLNA_FILEHANDLE;


typedef HUINT32 (*MEDIA_DLNA_OpenFile) (DI_MEDIA_DLNA_FILEDESC *ptFileDescription, DI_MEDIA_DLNA_FILEHANDLE *ptDLNAHandle);
typedef HUINT32 (*MEDIA_DLNA_CloseFile) (DI_MEDIA_DLNA_FILEHANDLE tDLNAHandle);
typedef HUINT32 (*MEDIA_DLNA_ReadFile) (DI_MEDIA_DLNA_FILEHANDLE tDLNAHandle, void *pBuffer, HUINT32 unBuffSize, HUINT32 *punReadSize);
typedef HUINT32 (*MEDIA_DLNA_SeekFile) (DI_MEDIA_DLNA_FILEHANDLE tDLNAHandle, HINT64 offset, HINT32 Origin);
typedef HUINT32 (*MEDIA_DLNA_TellFile) (DI_MEDIA_DLNA_FILEHANDLE tDLNAHandle, HUINT64 *plCurOffset);
typedef HUINT32 (*MEDIA_DLNA_IsEOFFile) (DI_MEDIA_DLNA_FILEHANDLE tDLNAHandle);
typedef HUINT32 (*MEDIA_DLNA_GetFileSize) (DI_MEDIA_DLNA_FILEHANDLE tDLNAHandle, HINT64 *pulFileSize);

typedef struct {
	MEDIA_DLNA_OpenFile			Open;
	MEDIA_DLNA_CloseFile		Close;
	MEDIA_DLNA_ReadFile			Read;
	MEDIA_DLNA_SeekFile			Seek;
	MEDIA_DLNA_TellFile			Tell;
	MEDIA_DLNA_IsEOFFile		IsEOF;
	MEDIA_DLNA_GetFileSize		GetFileSize;
	MEDIA_DLNA_OpenFile			OpenForNexus;
	MEDIA_DLNA_SeekFile			SeekForNexus;
	MEDIA_DLNA_GetFileSize		BoundsForNexus;
} MEDIA_DLNA_FileFunction_t;


DI_ERR_CODE DRV_MediaPlayer_Init(void);

#if 1 //-> debug 1231
DI_ERR_CODE DI_ThumbTest(HUINT32 mpId, char *url, DI_MEDIA_RECT_t destRect, void *outPtr);
#endif

DI_ERR_CODE DI_Media_Init(MediaPlayerEvtCb_t callback);
DI_ERR_CODE DI_Media_Term(void);
DI_ERR_CODE DI_Media_Play(HUINT32 mpId, DI_MEDIA_TYPE_e  mediaType, char *ucPath, DI_MEDIA_RECT_t destRect);
DI_ERR_CODE DI_Media_GetMp3Info(HUINT32 mpId, char *utl, DI_MEDIA_MP3_INFO_t *outInfo );
DI_ERR_CODE DI_Media_GetImageInfo(HUINT32 mpId ,char *url ,DI_MEDIA_IMG_INFO_t *outInfo);
DI_ERR_CODE DI_Media_Stop(HUINT32 mpId, DI_MEDIA_TYPE_e  mediaType);
DI_ERR_CODE DI_Media_Resume(HUINT32 mpId);
DI_ERR_CODE DI_Media_Pause(HUINT32 mpId);
DI_ERR_CODE DI_Media_RotateImage( HUINT32 mpId, DI_MEDIA_IMG_ROTATE_e direction, DI_MEDIA_RECT_t destRect );

/**
 * @brief Media file 재생을 위한 환경설정을 한다.
 * @brief - media file의 정보를 얻는다.
 * @brief - media file 재생에 필요한 resource를 설정한다.
 *
 * @param[in]	pucStreamUrl	Stream의 Path이다.
 * @param[in]	tStreamInfo		Media 재생을 위한 Stream에 대한 정보이다.
 * @param[out]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_Setup(HUINT8 *pucStreamUrl, DI_MEDIA_INFO_t *ptStreamInfo, HUINT32 *punMediaId);

/* TODO : will be more complexible for several types of key 
 * 		this function work well for aes128 key 
 * 		for hd5500t model
 */

#if defined (USE_DSC)
DI_ERR_CODE DI_MEDIA_MP_SetDecryption(HUINT32 unMediaId, DI_MEDIA_CRYPTO_SETTINGS *pstDecryptSettings);
DI_ERR_CODE DI_MEDIA_MP_ClearDecryption(HUINT32 unMediaId);
#endif

/**
 * @brief 		Media file 재생을 위해 설정된 환경설정을 종료한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @return	DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_Release(HUINT32 unMediaId);

/**
 * @brief media를 재생한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param	void
 * @return	DI Error Code
 * @see		DI_MEDIA_Stop
 */
DI_ERR_CODE DI_MEDIA_MP_Start (HUINT32 unMediaId, HBOOL bPause);

/**
 * @brief media 재생을 Stop한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param	void
 * @return	DI Error Code
 * @see		DI_MEDIA_Play
 */
DI_ERR_CODE DI_MEDIA_MP_Stop(HUINT32 unMediaId);

/**
 * @brief 재생 속도를 변경한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	eTrickSpeed 	변경하고자 하는 배속 정보를 지정한다.
 * @return	DI Error Code
 * @see		DI_MEDIA_GetSpeed
 */
DI_ERR_CODE DI_MEDIA_MP_SetSpeed(HUINT32 unMediaId, DI_MEDIA_TrickSpeed_e eTrickSpeed);

/**
 * @brief 현재 재생 속도를 반환한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[out]	eTrickSpeed	현재 배속 정보를 담는다.
 * @return	DI Error Code
 * @see		DI_MEDIA_SetSpeed
 */
DI_ERR_CODE DI_MEDIA_MP_GetSpeed(HUINT32 unMediaId, DI_MEDIA_TrickSpeed_e *peTrickSpeed);

/**
 * @brief millisecond 단위로 지정되는 position으로 재생 위치를 변경한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	ullTime 	이동하고자 하는 위치를 millisecond 단위로 지정한다.
 * @return	DI Error Code
 * @see		DI_MEDIA_GetPosition
 */
DI_ERR_CODE DI_MEDIA_MP_SetPosition(HUINT32 unMediaId, HUINT32 ulTime, DI_MEDIA_TIME_TYPE eTimeType);

/**
 * @brief 현재 재생 중인 위치를 millisecond 단위로 반환한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[out]	pullTime	현재 재생 중인 위치 정보를 담는다.
 * @return	DI Error Code
 * @see		DI_MEDIA_SetPosition
 */
DI_ERR_CODE DI_MEDIA_MP_GetPosition(HUINT32 unMediaId, HUINT32 *pulTime, DI_MEDIA_TIME_TYPE eTimeType);

/**
 * @brief		Playpump를 Flush한다. (현재는 Test Function임)
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see			DI_MEDIA_SetPosition
 */
DI_ERR_CODE DI_MEDIA_MP_FlushPlaypump(HUINT32 unMediaId);

/**
 * @brief callback 함수를 등록한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	callback	BOS(Begin of Stream), EOS(End of Stream) event를 받기 위한 Callback
 * @return	DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_RegisterCallback(HUINT32 unMediaId, DI_MEDIA_EventCallback_t callback);

/**
 * @brief		등록된 callback 함수를 해제한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_UnregisterCallback(HUINT32 unMediaId);

/**
 * @brief media file의 정보를 반환한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[out]	pStreamInfo	media file의 정보를 포함하는 구조체 pointer
 * @return	DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_GetStreamInfo(HUINT32 unMediaId, DI_MEDIA_INFO_t *pStreamInfo);

/**
 * @brief driver license 정보를 반환한다.
 *
 * @param[in] pcMediaFileName : XML file
 * @param[out]	pLicenseStatus	: License Status 
 * @return	DI Error Code
 * @see
 */

DI_ERR_CODE DI_MEDIA_MP_GetDRMLicenseState(HINT8 *pcLicenseData, DI_MEDIA_DRM_LICENSE_TYPE_e *pLicenseStatus);

/**
 * @brief license  를 받기 위해서 ClientInfo 를 만들어 준다.
 *
 * @param[in] pcMediaFileName : XML file
 * @param[out]	pLicenseStatus	: License Status 
 * @return	DI Error Code
 * @see
 */

DI_ERR_CODE DI_MEDIA_MP_GetDRMClientInfo(HINT8 *pcOutPutFileName);

/**
 * @brief license  file name을 얻어온다 
 *
 * @param[out]	pLicenseStatus	: License file Name
 * @return	DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_GetDRMFileName(HINT8 *pcOutPutFileName);

/**
 * @brief Stream의 Path를 이용하여 Probe를 한다.
 *
 * @param[in]	url 	변경하고자 하는 배속 정보를 지정한다.
 * @param[out]	stMediaFileInfo 	Probe된 Stream의 정보
 * @return	DI Error Code
 * @see		DI_MEDIA_PrintProbeInfo
 */
DI_ERR_CODE DI_MEDIA_Probe (HUINT8 *pszStreamUrl, DI_MEDIA_INFO_t *stMediaFileInfo);

/**
 * @brief parameter로 받은 Stream Info를 log로 Display한다.
 *
 * @param[in]	stMediaFileInfo	log로 Display하려는 Stream Info
 * @return	void
 * @see		DI_MEDIA_Probe
 */
void DI_MEDIA_PrintProbeInfo (DI_MEDIA_INFO_t *stMediaFileInfo);

/**
 * @brief		Playback Buffer의 정보를 반환한다.
 *
 * @param[in]	unMediaId				Media Device Id
 * @param[out]	pnCurrentUsedBufferSize	Playback Current Buffer
 * @param[out]	pnMaxBufferSize			Playback Max Buffer
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_GetPlaybackBuffer(HUINT32 unMediaId, HUINT32 *pnCurrentUsedBufferSize,HUINT32 *pnMaxBufferSize);

/**
 * @brief		Media Device 수를 반환한다.
 *
 * @param[out]	pulNumOfDeviceId	Media Device Number
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_GetCapability(HUINT32 *pulNumOfDeviceId);

/**
 * @brief			Url을 이용하여 Media에 대한 MediaInfo를 얻어온다.
 *
 * @param[in]		pcMediaPath		Media Path
 * @param[out]		ptMediaInfo		Media Info
 * @return			HERROR
 * @date 			2009/06/11
 * @author 			kimdh1@humaxdigital.com
 * @see
 */
DI_ERR_CODE DI_MEDIA_GetMediaInfo(HINT8 *pcMediaPath, DI_MEDIA_MEDIAINFO_t *ptMediaInfo);


/**
 * @brief			multi track audio의 경우 track을 변경한다.
 *
 * @param[in]		pcMediaPath		Media Path
 * @param[out]		ptMediaInfo		Media Info
 * @return			HERROR
 * @date 			2009/06/11
 * @author 			kimdh1@humaxdigital.com
 * @see
 */

DI_ERR_CODE DI_MEDIA_SetAudioTrack(HUINT32 unMediaId, HUINT8	ucAudioTrack);


/**
 * @brief			Get Divx Drm Information
 *
 * @param[in]		NONE
 * @param[out]	pucLength	: Length of Regcode
 * @return		DI_ERR_CODE
 * @date 		2009/11/02
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_SetDivxDrmActivation, DI_MEDIA_RegisterDivxXsubCallback
*/

DI_ERR_CODE DI_MEDIA_GetDivxDrmRegCode(HUINT8 *pucLength, HUINT8 **pucRegCode);

/**
 * @brief			Get Divx Drm Deregistration Code
 *
 * @param[in]		NONE
 * @param[out]	pucLength	: Length of Deregcode
 * @return		DI_ERR_CODE
 * @date 		2009/11/02
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_SetDivxDrmActivation, DI_MEDIA_RegisterDivxXsubCallback
*/

DI_ERR_CODE DI_MEDIA_GetDivxDrmDeregCode(HUINT8 *pucLength, HUINT8 **pucRegCode);


/**
 * @brief			Set Divx DRM Activation or Deactivation
 *
 * @param[in]		bOnOff		: TRUE	Activation\n
 *							: FALSE	Deactivation
 * @param[out]	NONE
 * @return		DI_ERR_CODE
 * @date 		2009/11/02
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_GetDivxDrmRegCode, DI_MEDIA_RegisterDivxXsubCallback
*/


DI_ERR_CODE	DI_MEDIA_SetDivxDrmActivation(HBOOL bOnOff);


/**
 * @brief			Set Divx DRM Activation or Deactivation
 *
 * @param[in]		unMediaId		: unMediaId \n
 				ulXsubTrack		: X Sub number
 * @param[out]	NONE
 * @return		DI_ERR_CODE
 * @date 		2009/11/02
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_GetDivxDrmRegCode, DI_MEDIA_RegisterDivxXsubCallback
*/


DI_ERR_CODE DI_MEDIA_SetDivxXsubTrack(HUINT32 unMediaId, HBOOL bOnOff, HUINT32 ulXsubTrack);


/**
 * @brief			Register Divx Event Callback
 *
 * @param[in]		unMediaId			: Media Player ID\n
				 eEvent 			 	: Divx Event\n
 *				fnDivxEventCallback	: Callback function of Divx Event
 * @param[out] 	NONE
 * @return 		DI_ERR_CODE
 * @date			2009/11/02
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_SetDivxDrmActivation, DI_MEDIA_GetDivxDrmRegCode, DI_MEDIA_SetDivxXsubTrack
*/

DI_ERR_CODE DI_MEDIA_RegisterDivxEventCallback(HUINT32 unMediaId, DI_MEDIA_DIVX_EVENT_e eEvent, DI_MEDIA_DivxEventCallback_t fnDivxEventCallback, HUINT32 *ulCallerId);

/**
 * @brief			Unregister Divx Event Callback
 *
 * @param[in]		unMediaId			: Media Player ID\n
 				eEvent				: Divx Event\n
 * @param[out] 	NONE
 * @return 		DI_ERR_CODE
 * @date			2009/11/02
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_SetDivxDrmActivation, DI_MEDIA_GetDivxDrmRegCode, DI_MEDIA_SetDivxXsubTrack
*/

DI_ERR_CODE DI_MEDIA_UnregisterDivxEventCallback(HUINT32 unMediaId, DI_MEDIA_DIVX_EVENT_e eEvent);

/**
 * @brief			Get DRM Activation Status
 *
 * @param[in]		NONE
 * @param[out] 	*pbOnOff
 * @return 		void
 * @date			2010/01/11
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_SetDivxDrmActivation
*/

void DI_MEDIA_GetDivxDrmActivationStatus(HBOOL *pbOnOff);


/**
 * @brief			Get DRM Activation Status
 *
 * @param[in]		NONE
 * @param[out] 	*pbOnOff
 * @return 		void
 * @date			2010/01/11
 * @author 		kimjh2@humaxdigital.com
 * @see			DI_MEDIA_SetDivxDrmActivation
*/

DI_ERR_CODE DI_MEDIA_GetDivxDrmStatus(HUINT32 unMediaId, HINT32 *pIsDRM, DI_MEDIA_DivxDrmInfos_t *pstDivxDrmInfo);


/**
 * @brief			Set/unset thumbnail mode
 *
 * @param[in]		bThumbnailMode  TRUE to set, FALSE to unset
 * @return 		DI_ERR_CODE
 * @date			2010/01/21
 * @author 		kjeom@humaxdigital.com
*/

DI_ERR_CODE DI_MEDIA_SetThumbnailMode(HUINT32 unMediaId, HBOOL bThumbnailMode);

/**
 * @brief			Install DLNA Function
 *
 * @param[in]		stDLNAFileFn  DLNA Function
 * @return 		DI_ERR_CODE
 * @date			2010/01/21
 * @author 		kjeom@humaxdigital.com
*/

DI_ERR_CODE DI_MEDIA_InstallDLNAFunction (MEDIA_DLNA_FileFunction_t stDLNAFileFn);






typedef enum DLNA_MEDIAFORMAT
{
    DI_MEDIA_DLNA_MEDIAFORMAT_PROBE,
    DI_MEDIA_DLNA_MEDIAFORMAT_MANUAL
} DI_MEDIA_DLNA_MEDIAFORMAT_e;

typedef enum DLNA_LINKPROTECTIONMODE_e
{
    DI_MEDIA_DLNA_LINKPROTECTIONMODE_NONE,
    DI_MEDIA_DLNA_LINKPROTECTIONMODE_DTCPIP
} DI_MEDIA_DLNA_LINKPROTECTIONMODE_e;

typedef struct DLNAMEDIA_Settings_t {
	void*										displayHandle;
	HUINT32										preChargeTime;  /* how many seconds to precharge the buffers before display begings */
	HINT32										initialSeekTime; /* how many seconds to seek into the media before starting the initial playback */
	HUINT8										disableRangeHeader; /* If server specifies it doesn't support Range header, than the http client must not send it */

	DI_MEDIA_DLNA_MEDIAFORMAT_e					format;
	DI_VIDEO_FORMAT							videoCodec;
	DI_MEDIA_AUDIO_FORMAT_e						audioCodec;
	DI_MEDIA_STREAM_TYPE_e						transportType;
	HINT32										audioPid;
	HINT32										videoPid;
	HINT32										avgBitRate; /* estimate */
	HUINT8										transportTimeStampEnabled;
	HUINT32										sampleRate; /* used to pass LPCM info */
	HUINT8										channels; /* used to pass LPCM info */

    /* security related */
    DI_MEDIA_DLNA_LINKPROTECTIONMODE_e			lpMode;

	/* RVU server are out of spec so we must track if we are talking to RVU servers */
	HUINT8										supportRvu;

	/* When set, Nexus handles are not created until they are needed. This is used 
	   to integrate with existing software and is not recommended for production use */
	HUINT8										delayResourceAcqusition;
} DI_MEDIA_DLNAMEDIA_Settings_t;



/**
 * Error codes used in the PL interfaces.
 */
typedef enum {
    DI_DLNA_PLAY_SUCCESS = 0,      /**< Success.*/
    DI_DLNA_PLAY_NOT_SUPPORTED,    /**< The format of the media is not supportable.*/
    DI_DLNA_PLAY_CONNECTION_ERROR,    /**< Accessing the media is not possible,
                                  because of wrong URL or unplugged ethernet.*/
    DI_DLNA_PLAY_RESOURCE_UNAVAILABLE,    /**< Acquiring resources to render
                                      the media is not possible. */
    DI_DLNA_PLAY_DURATION_UNKNOWN,   /**< The duration information is not
                                 currently available at DI_DLNA_PLAY_getDuration.*/
    DI_DLNA_PLAY_DURATION_UNBOUND,   /**< The media source does not have a defined
                                 duration as in case of a live broadcast
                                 at DI_DLNA_PLAY_getDuration.*/
    DI_DLNA_PLAY_BUFFERING_DONE,     /**<The buffering of the media is aleardy done.*/
    DI_DLNA_PLAY_NO_BUFFERING_DATA,  /**< When the buffering information of the
                                 media is not available. */
    DI_DLNA_PLAY_FAIL                 /**< It failed because of the other reasons
                                 which is not mentioned above.*/
}DI_DLNA_PLAY_ERR;

/**
 * Event codes used in the PL interfaces, happening at being played.
 */
typedef enum {
    DI_DLNA_PLAY_EV_DATA_STARVED = 0, /**< When the glue player has lost data or
                                  has stopped receiving data altogether */
    DI_DLNA_PLAY_EV_DURATION_UPDATED, /**< When the duration of the playing
                                  media changed */
    DI_DLNA_PLAY_EV_ENTERRING_LIVE,
    DI_DLNA_PLAY_EV_END_OF_MEDIA, /**< When the glue player has reached the the end
                                of its media and is stopping */
    DI_DLNA_PLAY_EV_INTERNAL_ERROR, /**< When the glue player has failed because of
                                implementation-specific reason */
    DI_DLNA_PLAY_EV_RATE_CHANGED, /**< When the playback rate of the player is changed
                             due to internal reasons
                             (This event is not generated by DI_DLNA_PLAY_setRate.) */
    DI_DLNA_PLAY_EV_BUFFERING_START, /**< When the buffering progress is started. */
    DI_DLNA_PLAY_EV_BUFFERING_END, /**< When the buffering progress is ended. */
    DI_DLNA_PLAY_EV_CONNECTION_LOST, /**< When the glue player has lost Connection */
    DI_DLNA_PLAY_EV_STREAM_START, /**< Application put up a loading banner while the Streamed Session is being established,
                            When this event received from PL and Application can take down the banner.*/
    DI_DLNA_PLAY_EV_BEGINNING_OF_MEDIA/**< When the glue player has reached the start
                             of its media and is stopping */
}DI_DLNA_PLAY_EVENT;

typedef enum DLNA_MEDIASTATE_e
{
    DI_MEDIA_DLNA_MEDIASTATE_NOMEDIAPRESENT,
    DI_MEDIA_DLNA_MEDIASTATE_STOPPED,
    DI_MEDIA_DLNA_MEDIASTATE_PLAYING,
    DI_MEDIA_DLNA_MEDIASTATE_PAUSEDPLAY,
    DI_MEDIA_DLNA_MEDIASTATE_TRANSITIONING,
    DI_MEDIA_DLNA_MEDIASTATE_RECORDING,
    DI_MEDIA_DLNA_MEDIASTATE_PAUSEDRECORDING
} DI_MEDIA_DLNA_MEDIASTATE_e;

typedef struct DLNA_MEDIASTATUS_t
{
    HUINT32 position;
    HUINT32 first;
    HUINT32 last;
    DI_MEDIA_DLNA_MEDIASTATE_e state;

	HUINT32 numPlaySpeedEntries;

    HINT32 playSpeed[DI_MEDIA_PLAYSPEED_MAX_COUNT];
	HUINT32 liveBufferStart;
	HUINT32 liveBufferEnd;

} DI_MEDIA_DLNA_MEDIASTATUS_t;

/**
 * This DI_DLNA_PLAY callback function is registered by the
 * #DI_DLNA_play_registerEventHandler and is called when the event
 * defind at #DI_DLNA_PLAY_EVENT is occured.
 *
 */
typedef void (*DI_DLNA_PLAY_EventCallBack)(HUINT32 deviceId, DI_DLNA_PLAY_EVENT ev);

/**
 *	Function: DI_DLNA_MEDIA_Setup
 *	Description: Play? ?? Media? ???? ??
 *
 *	The DI_DLNA_play_prefetch function prepares playing the media on the given url.
 *	The function allocates resources like a video plane and a decoder to play
 *	the given media url. The function needs to acquire resources as much as
 *	possible to reduce the start latency by the DI_DLNA_play_start function call.
 *
 *	Param[in]:	deviceId The device ID of the target video plane
 *	Param[in]:	urls The media urls to play.
 *	Param[in]:	pcProtocolInfo  - Protocol info
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_Setup(HUINT32 unDeviceId, HUINT8* pcUrls, HUINT8* pcProtocolInfo, DI_MEDIA_DLNAMEDIA_Settings_t *tMediaSettings);

/**
 *	Function: DI_DLNA_MEDIA_Release
 *	Description: Play? ?? Media? ??? ??? ???? ??
 *
 *	The DI_DLNA_play_prefetch function prepares playing the media on the given url.
 *	The function allocates resources like a video plane and a decoder to play
 *	the given media url. The function needs to acquire resources as much as
 *	possible to reduce the start latency by the DI_DLNA_play_start function call.
 *
 *	Param[in]:	void
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_Release(HUINT32 unDeviceId);

/**
 *	Function: DI_DLNA_MEDIA_Start
 *	Description: Play? ???? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_Start(HUINT32 deviceId);

/**
 *	Function: DI_DLNA_MEDIA_Stop
 *	Description: Play? ???? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_Stop(HUINT32 unDeviceId);

/**
 *	Function: DI_DLNA_MEDIA_SetSpeed
 *	Description:	Speed? ???? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *	Param[in]:	nRate		??? Speed
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_SetSpeed(HUINT32 unDeviceId, HINT32 nRate);

/**
 *	Function: DI_DLNA_MEDIA_GetSpeed
 *	Description:	?? ??? Speed? ?? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *	Param[in]:	pnRate		??? Speed
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_GetSpeed(HUINT32 unDeviceId, HINT32 *pnRate);

/**
 *	Function: DI_DLNA_MEDIA_GetPosition
 *	Description:	Position? ???? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *	Param[in]:	ulTime		??? Position
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_SetPosition(HUINT32 unDeviceId, HUINT64 ulTime);

/**
 *	Function: DI_DLNA_MEDIA_GetPosition
 *	Description:	?? ??? Position? ?? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *	Param[in]:	ulTime		??? Position
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_GetPosition(HUINT32 unDeviceId, HUINT64 *ulTime);

/**
 *	Function: DI_DLNA_MEDIA_GetDuration
 *	Description:	??? Duration? ?? ??
 *
 *	Param[in]:	deviceId The device ID of the target video plane.
 *	Param[in]:	ulTime		Duration
 *
 *	Return  DI_ERR_CODE.
 *
 */
DI_ERR_CODE DI_DLNA_MEDIA_GetDuration(HUINT32 unDeviceId, HUINT64 *ulTime);


void DI_DLNA_MEDIA_RegisterEventHandler(DI_DLNA_PLAY_EventCallBack handler);






#endif /* !__DI_MEDIA_H__ */

/** @} */

