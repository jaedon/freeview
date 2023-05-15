/**
 * @file	di_video.h
 * @brief	Video module DI layer header file
 * @version $Revision: 0.9.0 $
 * @date 	$Date: 2008/06/24 12:00:00 $
 * @author
 */

/** @defgroup	DI_VIDEO	VIDEO COMMON IF OF DI LAYER
 *  This is VIDEO module of DI layer.
 *  @{
 */

#ifndef __DI_VIDEO_H__
#define __DI_VIDEO_H__
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "di_err.h" /* if di module is included, di_err.h is included first!Later this line will be removed.*/

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/**
 * Define video format
 */
typedef enum
{
	DI_VIDEO_FORMAT_MPEG1         	= 0x00000001,	/**< MPEG1 Video */
	DI_VIDEO_FORMAT_MPEG2         	= 0x00000002,	/**< MPEG2 Video */
	DI_VIDEO_FORMAT_MPEG4Part2    	= 0x00000010,	/**< MPEG4 Part2 Video */
	DI_VIDEO_FORMAT_H261          	= 0x00000020,	/**< H.261 Video */
	DI_VIDEO_FORMAT_H263          	= 0x00000040,	/**< H.263 Video */
	DI_VIDEO_FORMAT_H264          	= 0x00000080,	/**< H.264 Video */
	DI_VIDEO_FORMAT_VC1SimpleMain 	= 0x00000100,	/**< VC1 Simple Main Video */
	DI_VIDEO_FORMAT_VC1           	= 0x00000200,	/**< VC1 Video */
	DI_VIDEO_FORMAT_AVS						= 0x00000400, 	/**< AVS Video */
	DI_VIDEO_FORMAT_DIVX_311				= 0x00000800,
	DI_VIDEO_FORMAT_STILL_MPEG2				= 0x00001000, 	/**< Still Video, MPEG 2 */
	DI_VIDEO_FORMAT_STILL_H264				= 0x00002000,	/**< H.264  */
	DI_VIDEO_FORMAT_STILL_VC1				= 0x00004000,	/**< VC1 */
	DI_VIDEO_FORMAT_H265          			= 0x00008000,	/**< H.265 Video */
	DI_VIDEO_FORMAT_VP8						= 0x00010000,	/**< VP8 Video */
	DI_VIDEO_FORMAT_VP9						= 0x00020000,	/**< VP9 Video */
	DI_VIDEO_FORMAT_MEDIA_MPEG1				= 0x10000001,	/**< MPEG1 Video */
	DI_VIDEO_FORMAT_MEDIA_MPEG2				= 0x10000002,	/**< MPEG2 Video */
	DI_VIDEO_FORMAT_MEDIA_MPEG4Part2		= 0x10000010,	/**< MPEG4 Part2 Video */
	DI_VIDEO_FORMAT_MEDIA_H261				= 0x10000020,	/**< H.261 Video */
	DI_VIDEO_FORMAT_MEDIA_H263				= 0x10000040,	/**< H.263 Video */
	DI_VIDEO_FORMAT_MEDIA_H264				= 0x10000080,	/**< H.264 Video */
	DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain		= 0x10000100,	/**< VC1 Simple Main Video */
	DI_VIDEO_FORMAT_MEDIA_VC1				= 0x10000200,	/**< VC1 Video */
	DI_VIDEO_FORMAT_MEDIA_AVS				= 0x10000400, 	/**< AVS Video */
	DI_VIDEO_FORMAT_MEDIA_DIVX_311			= 0x10000800,
	DI_VIDEO_FORMAT_MEDIA_H265          	= 0x10008000,	/**< H.265 Video */
	DI_VIDEO_FORMAT_MEDIA_VP8				= 0x10010000,	/**< VP8 Video */
	DI_VIDEO_FORMAT_MEDIA_VP9				= 0x10020000,	/**< VP9 Video */
	DI_VIDEO_FORMAT_NONE					= 0x00000000
}DI_VIDEO_FORMAT;


/**
 * Define active format, ETSI TS 101 154 V1.7.1 (2005-06)
 */
typedef enum
{
	DI_VIDEO_AFD_RESERVED				= 0x0,	/* 0000 ~ 0001, 0101 ~0111, 1100 Reserved */
	DI_VIDEO_AFD_BOX_16_9_TOP			= 0x2,	/**< box 16:9 (top) */
	DI_VIDEO_AFD_BOX_14_9_TOP			= 0x3,	/**< box 14:9 (top) */
	DI_VIDEO_AFD_BOX_16_9_CENTRE   		= 0x4,	/**< box 16:9 (centre) */
	DI_VIDEO_AFD_SAME_AS_CODED_FRAME	= 0x8,	/**< Active format is the same as the coded frame */
	DI_VIDEO_AFD_4_3_CENTRE				= 0x9,	/**< 4:3 (centre) */
	DI_VIDEO_AFD_16_9_CENTRE			= 0xA,	/**< 16:9 (centre) */
	DI_VIDEO_AFD_14_9_CENTRE			= 0xB,	/**< 14:9 (centre) */
	DI_VIDEO_AFD_4_3_SNP_14_9_CENTRE	= 0xD,	/**< 4:3 (with shoot & protect 14:9 centre) */
	DI_VIDEO_AFD_16_9_SNP_14_9_CENTRE	= 0xE,	/**< 16:9 (with shoot & protect 14:9 centre) */
	DI_VIDEO_AFD_16_9_SNP_4_3_CENTRE	= 0xF	/**< 16:9 (with shoot & protect 4:3 centre) */
}DI_VIDEO_ACTIVE_FORMAT;


/**
 * Define video decoder type
 */
typedef enum
{
	DI_VIDEO_DECODER_MAIN,							/**< video decoder main */
	DI_VIDEO_DECODER_SUB							/**< video decoder sub */
} DI_VIDEO_DECODER_TYPE;


/**
 * Define running state of video
 */

typedef enum
{
	DI_VIDEO_RUNNING_STATE_UNKNOWN = 0,				/**< unknown video state */
	DI_VIDEO_RUNNING_STATE_DECODING ,				/**< decoding video state */
	DI_VIDEO_RUNNING_STATE_NODATA					/**< nodata video state */
} DI_VIDEO_RUNNING_STATE;


/**
 * Define Closed Caption data state of exitence in video
 */

typedef enum
{
	DI_VIDEO_CC_DATA_STATE_UNKNOWN = 0,				/**< unknown closed caption data state */
	DI_VIDEO_CC_DATA_STATE_DATAFLOW,				/**< incoming closed caption data state */
	DI_VIDEO_CC_DATA_STATE_NODATA					/**< nodata closed caption data state */
} DI_VIDEO_CC_DATA_STATE;

/**
 * Define Video Analog Output Mode
 */

typedef struct
{
   HUINT32 bCVBS:1;
   HUINT32 bSVHS:1;
   HUINT32 bRGB:1;
   HUINT32 bYPbPr:1;
   HUINT32 bYPbPr_Downscale:1;
} DI_VIDEO_AOUT_MODE_t;

/**
 * Define HD Video Resolution
 */

typedef enum
{
	DI_VIDEO_HD_RESOLUTION_480I = 0,
	DI_VIDEO_HD_RESOLUTION_480P,
	DI_VIDEO_HD_RESOLUTION_576I,
	DI_VIDEO_HD_RESOLUTION_576P,
	DI_VIDEO_HD_RESOLUTION_720P,
	DI_VIDEO_HD_RESOLUTION_1080I,
	DI_VIDEO_HD_RESOLUTION_1080P,			/* 1080p 60Hz */
	DI_VIDEO_HD_RESOLUTION_720P_50HZ,
	DI_VIDEO_HD_RESOLUTION_1080I_50HZ,
	DI_VIDEO_HD_RESOLUTION_1080P_50HZ,
	DI_VIDEO_HD_RESOLUTION_480I_J,
	DI_VIDEO_HD_RESOLUTION_1080P_24HZ,
	DI_VIDEO_HD_RESOLUTION_1080P_25HZ,
	DI_VIDEO_HD_RESOLUTION_1080P_30HZ,
	DI_VIDEO_HD_RESOLUTION_2160P_24HZ,
	DI_VIDEO_HD_RESOLUTION_2160P_25HZ,
	DI_VIDEO_HD_RESOLUTION_2160P_30HZ,
	DI_VIDEO_HD_RESOLUTION_2160P_50HZ,
	DI_VIDEO_HD_RESOLUTION_2160P,	/* 2160p 60Hz*/
	DI_VIDEO_HD_RESOLUTION_MAX
}DI_VIDEO_HD_RESOLUTION;

/**
 * Define Video Window Type
 */
typedef enum
{
	DI_VIDEO_WIN_HD=0x01,
	DI_VIDEO_WIN_SD=0x02,
	DI_VIDEO_WIN_ALL=0x03
}DI_VIDEO_WIN_TYPE;

/**
 * Define SD Video Format
 */

typedef enum
{
	DI_VIDEO_SD_STANDARD_PAL = 0,
	DI_VIDEO_SD_STANDARD_NTSC,
	DI_VIDEO_SD_STANDARD_NTSC_J,
	DI_VIDEO_SD_STANDARD_SECAM
}DI_VIDEO_SD_STANDARD;

/**
 * Define Video Color
 */

typedef struct
{
	HUINT8	a;
	HUINT8	r;
	HUINT8	g;
	HUINT8	b;
} DI_VIDEO_COLOR_t;

/**
 * Define WSS Group 1 Format (AFD)
 */
typedef enum
{
	DI_VIDEO_WSS_AFD_FULL_FORMAT_4_3,
	DI_VIDEO_WSS_AFD_BOX_14_9_CENTRE,
	DI_VIDEO_WSS_AFD_BOX_14_9_TOP,
	DI_VIDEO_WSS_AFD_BOX_16_9_CENTRE,
	DI_VIDEO_WSS_AFD_BOX_16_9_TOP,
	DI_VIDEO_WSS_AFD_BOX_16_9_OVER,
	DI_VIDEO_WSS_AFD_FULL_FORMAT_4_3_SNP,
	DI_VIDEO_WSS_AFD_FULL_FORMAT_16_9_ANAMORPHIC,
	DI_VIDEO_WSS_AFD_UNDEFINED
} DI_VIDEO_WSS_AFD;


/**
 * Define WSS Group 4 Format (Copyright)
 */
typedef enum
{
	DI_VIDEO_WSS_NO_CP,	/* no copyright asserted or status unknown */
	DI_VIDEO_WSS_CP,	/* copyright asserted */
	DI_VIDEO_WSS_CP_UNDEFINED
} DI_VIDEO_WSS_CP_BIT;

/**
 * Define WSS Group 4 Format (Generation)
 */
typedef enum
{
	DI_VIDEO_WSS_GE_NO_REST,	/* copying not asserted */
	DI_VIDEO_WSS_GE_REST,		/* copying asserted */
	DI_VIDEO_WSS_GE_UNDEFINED
} DI_VIDEO_WSS_GE_BIT;

/**
 * Define MACROVISION
 */

typedef enum
{
	DI_VIDEO_MV_NONE=0,
	DI_VIDEO_MV_AGC_ONLY,
	DI_VIDEO_MV_AGC_2LINES,
	DI_VIDEO_MV_AGC_4LINES,
	DI_VIDEO_MV_STANDARD_PAL,	/* Default Standard programming param for PAL */
	DI_VIDEO_MV_STANDARD_NTSC,	/* Default Standard programming param for NTSC */
	DI_VIDEO_MV_AGC_ONLY_RGB,	/* PAL cert MV AGC only RGB */
	DI_VIDEO_MV_TEST1,		/* PAL cert MV test 1 */
	DI_VIDEO_MV_TEST2		/* PAL cert MV test 2 */
} DI_VIDEO_MV_MODE;

/**
 * Define CGMS types
 */
typedef enum
{
	DI_VIDEO_CGMS_EIA_CEA_805 = 0,
	DI_VIDEO_CGMS_ARIB = 1	/* This standard applies only to Japan */
} DI_VIDEO_CGMS_TYPE;

/**
 * Define CGMS-A Format
 */
typedef enum
{                      /* bit 8 7 */
   DI_VIDEO_CGMS_COPY_NO_RSTRICT   = 0,   /* Copying is permitted without restriction */
   DI_VIDEO_CGMS_ONE_GEN_COPY      = 1,   /* One generation of copies may be made */
   DI_VIDEO_CGMS_NO_CONDITION      = 2,   /* No more copies */
   DI_VIDEO_CGMS_NO_COPY           = 3,   /* No copying is permitted */
   DI_VIDEO_CGMS_END
}DI_VIDEO_CGMS_A;


/**
 * Define APS Format
 */
typedef enum
{
	DI_VIDEO_CGMS_PSP_OFF				= 0,	/* No APS off */
	DI_VIDEO_CGMS_PSP_ON_2Line_SBURST	= 1,	/* PSP on, 2-line split burst on */
	DI_VIDEO_CGMS_PSP_ON_SBURST_OFF 	= 2,	/* PSP on, split burst off */
	DI_VIDEO_CGMS_PSP_ON_4Line_SBURST	= 3,	/* PSP on, 4-line split burst on */
	DI_VIDEO_CGMS_PSP_END
} DI_VIDEO_APS_TRIGGER;

/**
 * Define video's notifying event
 */
typedef	enum
{
	DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED,		/**< Video sequence info event is changed */
	DI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED,						/**< Video format event is changed */
	DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED,				/**< Video running state is changed */
	DI_VIDEO_NOTIFY_EVT_AFD_CHANGED,						/**< Video afd info event is changed */
	DI_VIDEO_NOTIFY_EVT_CC_DATA_STATE_CHANGED,				/**< Closed caption data state is changed */
	// IDTV API
	DI_VIDEO_NOTIFY_EVT_ATV_SYNC_CHANGED,					/**< Analog tv video sync status is changed */
	DI_VIDEO_NOTIFY_EVT_DATAEMPTY,						/**< Video data is Empty */
	DI_VIDEO_NOTIFY_EVT_DATAFLOW,						/**< Video data is Not Empty*/
	DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED,					/**< Video 3D info event is changed */
	// IDTV API <<
	DI_VIDEO_NOTIFY_EVT_ERROR,					/**< Video decode or display error */
	DI_VIDEO_NOTIFY_EVT_END
}DI_VIDEO_NOTIFY_EVT;


/**
 * Define video stream type
 */
typedef	enum
{
	// See nexus_types.h
	DI_VIDEO_STREAM_TYPE_ES = 1,						/**< MPEG 2 Video stream type is ES */
	DI_VIDEO_STREAM_TYPE_PES,							/**< MPEG 2 Video stream type is PES */
	DI_VIDEO_STREAM_TYPE_TS,							/**< MPEG 2 Video stream type is TS */
	DI_VIDEO_STREAM_TYPE_VOB,							/**< VOB */
	DI_VIDEO_STREAM_TYPE_MPEG1PS,						/**< MPEG1 PS */
	DI_VIDEO_STREAM_TYPE_DSSES,							/**< DSS ES */
	DI_VIDEO_STREAM_TYPE_DSSPES,						/**< DSS PES */
	DI_VIDEO_STREAM_TYPE_ASF,							/**< ASF */
	DI_VIDEO_STREAM_TYPE_AVI,							/**< AVI */
	DI_VIDEO_STREAM_TYPE_MP4,							/**< MP4 */
	DI_VIDEO_STREAM_TYPE_FLV,							/**< FLV */
	DI_VIDEO_STREAM_TYPE_MKV,							/**< MKV */
	DI_VIDEO_STREAM_TYPE_WAV,							/**< Wave */
	DI_VIDEO_STREAM_TYPE_MAX
	// Do not change the order.
}DI_VIDEO_STREAM_TYPE;

/**
 * Define video profile type
 */
typedef	enum
{
	DI_VIDEO_PROTOCOL_PROFILE_UNKNOWN = 0,
	DI_VIDEO_PROTOCOL_PROFILE_SIMPLE,
	DI_VIDEO_PROTOCOL_PROFILE_MAIN,
	DI_VIDEO_PROTOCOL_PROFILE_HIGH,
	DI_VIDEO_PROTOCOL_PROFILE_ADVANCED,
	DI_VIDEO_PROTOCOL_PROFILE_JIZHUN,
	DI_VIDEO_PROTOCOL_PROFILE_SNRSCALABLE,
	DI_VIDEO_PROTOCOL_PROFILE_SPATIALLYSCALABLE,
	DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE,
	DI_VIDEO_PROTOCOL_PROFILE_BASELINE,
	DI_VIDEO_PROTOCOL_PROFILE_MAX

} DI_VIDEO_PROTOCOL_PROFILE_TYPE;

/**
 * Define video level type
 */
typedef	enum
{
	DI_VIDEO_PROTOCOL_LEVEL_UNKNOWN = 0,
	DI_VIDEO_PROTOCOL_LEVEL_00,
	DI_VIDEO_PROTOCOL_LEVEL_10,
	DI_VIDEO_PROTOCOL_LEVEL_1B,
	DI_VIDEO_PROTOCOL_LEVEL_11,
	DI_VIDEO_PROTOCOL_LEVEL_12,
	DI_VIDEO_PROTOCOL_LEVEL_13,
	DI_VIDEO_PROTOCOL_LEVEL_20,
	DI_VIDEO_PROTOCOL_LEVEL_21,
	DI_VIDEO_PROTOCOL_LEVEL_22,
	DI_VIDEO_PROTOCOL_LEVEL_30,
	DI_VIDEO_PROTOCOL_LEVEL_31,
	DI_VIDEO_PROTOCOL_LEVEL_32,
	DI_VIDEO_PROTOCOL_LEVEL_40,
	DI_VIDEO_PROTOCOL_LEVEL_41,
	DI_VIDEO_PROTOCOL_LEVEL_42,
	DI_VIDEO_PROTOCOL_LEVEL_50,
	DI_VIDEO_PROTOCOL_LEVEL_51,
	DI_VIDEO_PROTOCOL_LEVEL_60,
	DI_VIDEO_PROTOCOL_LEVEL_62,
	DI_VIDEO_PROTOCOL_LEVEL_LOW,
	DI_VIDEO_PROTOCOL_LEVEL_MAIN,
	DI_VIDEO_PROTOCOL_LEVEL_HIGH,
	DI_VIDEO_PROTOCOL_LEVEL_HIGH1440,
	DI_VIDEO_PROTOCOL_LEVEL_MAX
}	DI_VIDEO_PROTOCOL_LEVEL_TYPE;

/**
 * Define Macrovision Version type
 */
typedef	enum
{
	DI_VIDEO_MACROVISION_VERSION_7_1_D1 = 0x01,	// DVD
	DI_VIDEO_MACROVISION_VERSION_7_1_S1 = 0x02,	// DMR/STB/IRD
	DI_VIDEO_MACROVISION_VERSION_7_2_H1 = 0x04,	// Most other Products (e.g. AACS encrypted disc products, DME, AMR, etc.)
	DI_VIDEO_MACROVISION_VERSION_NONE = 0x00
}	DI_VIDEO_MACROVISION_VERSION_TYPE;

/**
 * Define Video Mode
 */
typedef enum
{
	DI_VIDEO_DECODER_MODE_NORMAL = 0,
	DI_VIDEO_DECODER_MODE_RFVOD,
	DI_VIDEO_DECODER_MODE_GAME,
	DI_VIDEO_DECODER_MODE_IP_CAMERA,
	DI_VIDEO_DECODER_MODE_MAX		
} DI_VIDEO_DECODER_MODE;

/**
 * Define video profile & level
 */
typedef struct
{
	DI_VIDEO_PROTOCOL_PROFILE_TYPE eProfile;
	DI_VIDEO_PROTOCOL_LEVEL_TYPE eLevel;
	HUINT32 ulMaximumDecodingCapability;
} DI_VIDEO_PROFILELEVEL_t;

/**
 * Define video codec capability
 */
typedef struct
{
	HUINT32 ulProfileLevelNumberOfEntries;
	DI_VIDEO_PROFILELEVEL_t *pstProfileLevel;
} DI_VIDEO_CODEC_CAP_t;

/**
 * Define video decoder status
 */

typedef struct
{
	DI_VIDEO_PROTOCOL_PROFILE_TYPE eProfile;
	DI_VIDEO_PROTOCOL_LEVEL_TYPE eLevel;
	HUINT32 ulDepth;  /* depth in bytes of the decoder buffer */
	HUINT32 ulTotalSize;   /* total size in bytes of the decoder buffer */
} DI_VIDEO_DECODER_STATUS_t;

/**
 * Define video output capability
 */

typedef struct
{
	DI_VIDEO_AOUT_MODE_t stAnalogOutput;
	DI_VIDEO_SD_STANDARD eVideoSDStandard;
} DI_VIDEO_OUTPUT_CAP_t;

/**
 * Define video capability
 */
typedef struct
{
	HUINT32						ulGroupId;
	HUINT32						ulDecoderId;
	DI_VIDEO_DECODER_TYPE		eDecoderType;
	DI_VIDEO_FORMAT				eSupportedFormat;
	HUINT32						ulPrimerCount;
} DI_VIDEO_CAP_t;

/**
 * Define Video aspect ratio
 */

typedef enum
{
	DI_VIDEO_ASPECTRATIO_16_9,
	DI_VIDEO_ASPECTRATIO_4_3,
	DI_VIDEO_ASPECTRATIO_4_3_LETTER,
	DI_VIDEO_ASPECTRATIO_14_9,
	DI_VIDEO_ASPECTRATIO_1_1,
	DI_VIDEO_ASPECTRATIO_SAR,
	DI_VIDEO_ASPECTRATIO_UNKNOWN,
	DI_VIDEO_ASPECTRATIO_MAX
}DI_VIDEO_AspectRatio_t;


/**
 * Define Video frame rate
 */
typedef enum
{
	DI_VIDEO_FRAMERATE_24,
	DI_VIDEO_FRAMERATE_25,
	DI_VIDEO_FRAMERATE_30,
	DI_VIDEO_FRAMERATE_50,
	DI_VIDEO_FRAMERATE_60,
	DI_VIDEO_FRAMERATE_MAX
}DI_VIDEO_FrameRate_t;

/**
 * Define Video Capture Layer
 */
typedef enum
{
	DI_VIDEO_CAP_VIDEO,
	DI_VIDEO_CAP_OSD,
	DI_VIDEO_CAP_ALL,
	DI_VIDEO_CAP_MAX
}DI_VIDEO_CAPTURE_t;

/**
 * Define Video 3D Format
 */
typedef enum
{
	DI_VIDEO_3D_FORMAT_NONE,		/* 2D */
	DI_VIDEO_3D_FORMAT_SIDEBYSIDE,
	DI_VIDEO_3D_FORMAT_TOPANDBOTTOM,
	DI_VIDEO_3D_FORMAT_FRAMEPACKING
}DI_VIDEO_3D_FORMAT;

/**
 * Define Video Channel Change Mode
 */
typedef enum
{
	DI_VIDEO_CC_MUTE,
	DI_VIDEO_CC_HOLD_UNTIL_TSMLOCK,
	DI_VIDEO_CC_MUTE_UNTIL_FIRST_PICTURE,
	DI_VIDEO_CC_HOLD_UNTIL_FIRST_PICTURE
} DI_VIDEO_CC_MODE;

/******************************************************************************/
/***************************** IDTV API - START *******************************/
/******************************************************************************/

/**
 * Define video output port
 */
typedef	enum
{
	DI_VIDEO_OUTPUT_PANEL = 0,
	DI_VIDEO_OUTPUT_SCART1,
	DI_VIDEO_OUTPUT_SCART2,
	DI_VIDEO_OUTPUT_MAX
}DI_VIDEO_OUTPUT_PORT;

/**
 * video color standard
 */
typedef enum
{
	DI_VIDEO_COLSTD_NTSC,
	DI_VIDEO_COLSTD_PAL,
	DI_VIDEO_COLSTD_PAL_N,
	DI_VIDEO_COLSTD_PAL_M,
	DI_VIDEO_COLSTD_SECAM,
	DI_VIDEO_COLSTD_NTSC_443,
	DI_VIDEO_COLSTD_PAL_60,
	DI_VIDEO_COLSTD_UNKNOWN_525,
	DI_VIDEO_COLSTD_UNKNOWN_625,
	DI_VIDEO_COLSTD_UNKNOWN,
	DI_VIDEO_COLSTD_AUTO
} DI_VIDEO_COLOR_STANDARD;

/* analog tuner 입력의 sync 상태 */
typedef enum
{
	DI_VIDEO_TUNER_SYNC_UNLOCK,
	DI_VIDEO_TUNER_SYNC_LOCK
} DI_VIDEO_TUNER_SyncStatus_t;

/* analog video decoder 입력의 sync 상태 */
typedef enum
{
	DI_VIDEO_DECODER_SYNC_UNLOCK,
	DI_VIDEO_DECODER_SYNC_LOCK
} DI_VIDEO_DECODER_SyncStatus_t;

/******************************************************************************/
/****************************** IDTV API - END ********************************/
/******************************************************************************/

/**
 * Define Video rectangle
 */
typedef struct
{
	HUINT16 ulX;
	HUINT16 ulY;
	HUINT16 ulWidth;
	HUINT16 ulHeight;
} DI_VIDEO_RECT_t;

/**
 * Define stream information
 */
typedef struct
{
	HUINT32					ulWidth;
	HUINT32					ulHeight;
	HUINT32					ulDisplayWidth;
	HUINT32					ulDisplayHeight;
	DI_VIDEO_AspectRatio_t	eAspectRatio;
	DI_VIDEO_FrameRate_t	eFrameRate;
	DI_VIDEO_FORMAT			eVideoFormat;
	HBOOL					bProgressive;
	HUINT8					ucChromaFormat;
	HUINT8					ucVideoFormat;
	HBOOL					ucColorPrimarie;
	HBOOL					ucAlpha;
} DI_VIDEO_StreamInfo_t;


typedef void (*EVENT_CALLBACK_FUNCTION )(void *pCallbackData);
typedef DI_ERR_CODE (* pfnDI_VIDEO_Notify)(DI_VIDEO_HD_RESOLUTION eHDOutput);
typedef void (*pfnDI_VIDEO_CcDataCallback )(void *pCallbackData);

/**
 * Define running state event
 */
typedef struct
{
	DI_VIDEO_RUNNING_STATE state;
} DI_VIDEO_RUNNING_STATE_EVT_t;

/**
 * Define stream information event
 */
typedef struct
{
	DI_VIDEO_FORMAT		format;
} DI_VIDEO_FORMAT_EVT_t;

/**
 * Define sequence header event
 */
typedef struct
{
	DI_VIDEO_StreamInfo_t info;
} DI_VIDEO_SEQ_HEADER_EVT_t;

/**
 * Define user data event
 */
typedef struct
{
	DI_VIDEO_ACTIVE_FORMAT format;
} DI_VIDEO_ACTIVE_FORMAT_EVT_t;

/**
 * Define 3D info event
 */
typedef struct
{
	DI_VIDEO_3D_FORMAT e3dFormat;
} DI_VIDEO_3D_INFO_EVT_t;

/**
 * Define video error info event
 */
typedef struct
{
	HUINT32 ulDecodeError;
	HUINT32 ulDisplayError;
} DI_VIDEO_ERROR_INFO_EVT_t;

/**
 * Define Closed Caption data state
 */
typedef	struct
{
	DI_VIDEO_CC_DATA_STATE 	eState;
} DI_VIDEO_CC_DATA_STATE_EVT_t;

/**
 * Define Closed Caption data entry info
 */
typedef	struct
{
	HUINT8					ccType;
	HUINT8					data[2];
}DI_VIDEO_CC_ENTRY_t;


/**
 * Define Video callback
 */
typedef	struct
{
	HUINT32					ulDecId;
	DI_VIDEO_NOTIFY_EVT		eEvent;
	void					*pvEventData; /* Sequence Header Changed, Format Changed, Running State Changed */
}DI_VIDEO_CALLBACK_DATA_t;

/**
 * Define Closed Caption data callback
 */
typedef	struct
{
	HUINT32					ulDecId;
	HCHAR					numValidEntries;
	DI_VIDEO_CC_ENTRY_t *	pCcEntry;	
}DI_VIDEO_CC_DATA_CALLBACK_DATA_t;

/**
 * settings for the copy generation management system
 */
typedef struct
{
	DI_VIDEO_CGMS_TYPE	eType;
	union
	{
		struct
		{
			DI_VIDEO_CGMS_A		eCgmsA;
			DI_VIDEO_APS_TRIGGER	eApsTrigger;
		}tEiaCea805;
		struct
		{
			DI_VIDEO_AspectRatio_t	eAspectRatio;
			DI_VIDEO_CGMS_A 	eCgmsA;
			DI_VIDEO_APS_TRIGGER	eApsTrigger;
		}tArib;
	}uTypeStatus;
}DI_VIDEO_CGMS_SETTINGS;

/**
 * settings for the wide screen signaling
 */
typedef struct
{
	DI_VIDEO_WSS_AFD	eAfd;
	DI_VIDEO_WSS_CP_BIT	eCpbit;
	DI_VIDEO_WSS_GE_BIT	eGebit;
} DI_VIDEO_WSS_SETTINGS_t;

typedef struct
{
	HUINT32 ulPrimerIndex;
	DI_VIDEO_FORMAT eFormat;
} DI_VIDEO_PRIMER_t;

/**
 * display parameter struct
 */
typedef enum
{
	DI_VIDEO_DIS_PARAM_CONTRAST = 0,
	DI_VIDEO_DIS_PARAM_SATURATION,
	DI_VIDEO_DIS_PARAM_HUE,
	DI_VIDEO_DIS_PARAM_BRIGHTNESS,
	DI_VIDEO_DIS_PARAM_SHARPNESS,
	DI_VIDEO_DIS_PARAM_MAX
} DI_VIDEO_DIS_PARAM_E;

/******************************************************************************/
/***************************** IDTV API - START *******************************/
/******************************************************************************/

/**
 * Define video sync status event
 */
typedef struct
{
	DI_VIDEO_TUNER_SyncStatus_t status;
} DI_VIDEO_SYNC_STATUS_EVT_t;

/**
 * adc setting
 */
typedef struct
{
	HUINT16	AdcRedOffset1;
	HUINT16	AdcGreenOffset1;
	HUINT16	AdcBlueOffset1;
	HUINT16	AdcRedOffset2;
	HUINT16	AdcGreenOffset2;
	HUINT16	AdcBlueOffset2;
	HUINT16	AdcRedGain;
	HUINT16	AdcGreenGain;
	HUINT16	AdcBlueGain;
}DI_VIDEO_ADC_SETTING_t;

/******************************************************************************/
/****************************** IDTV API - END ********************************/
/******************************************************************************/

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/
DI_ERR_CODE DI_VIDEO_SetDisplayParam(DI_VIDEO_DIS_PARAM_E type, HINT32 value);
DI_ERR_CODE DI_VIDEO_GetCapability (HUINT32 *pulNumOfDevice, DI_VIDEO_CAP_t **ppCapInfo);
DI_ERR_CODE DI_VIDEO_Start(HUINT32 ulDeviceId, DI_VIDEO_FORMAT eVideoFormat);
DI_ERR_CODE DI_VIDEO_Stop(HUINT32 ulDeviceId);
DI_ERR_CODE DI_VIDEO_ForceDisableOutput(HUINT32 ulDeviceId, HUINT32 ulDisplayId, HBOOL bMute);
DI_ERR_CODE DI_VIDEO_Show (HUINT32 ulDeviceId, HBOOL bShow);
DI_ERR_CODE DI_VIDEO_Pause (HUINT32 ulDeviceId, HBOOL bOn );
DI_ERR_CODE DI_VIDEO_Flush(HUINT32 ulDeviceId);
DI_ERR_CODE DI_VIDEO_StartPrimer(HUINT32 ulDeviceId, DI_VIDEO_PRIMER_t *pPrimerSetting);
DI_ERR_CODE DI_VIDEO_StopPrimer(HUINT32 ulDeviceId, DI_VIDEO_PRIMER_t *pPrimerSetting);
DI_ERR_CODE DI_VIDEO_SelectPrimer(HUINT32 ulDeviceId, HUINT32 ulPrimerIndex);
DI_ERR_CODE DI_VIDEO_GetState (HUINT32 ulDeviceId, DI_VIDEO_RUNNING_STATE *peVideoState);
DI_ERR_CODE DI_VIDEO_SetBlank (HUINT32 ulDeviceId, unsigned int bFlag);
DI_ERR_CODE DI_VIDEO_SetBackground(HUINT32 unDeviceId , DI_VIDEO_COLOR_t stColor);
DI_ERR_CODE DI_VIDEO_SetPlaneAlpha (HUINT32 ulDeviceId, HUINT8 alphaValue );
DI_ERR_CODE DI_VIDEO_SetAspect(HUINT32 ulDeviceId, DI_VIDEO_AspectRatio_t eAspect);
DI_ERR_CODE DI_VIDEO_SetInOutWindow(HUINT32 ulDeviceId, DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pOutRect, HBOOL *pbTouched, DI_VIDEO_WIN_TYPE eVidWin);
DI_ERR_CODE DI_VIDEO_ApplySetInOutWindows(HBOOL bApply);
DI_ERR_CODE DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDResolution);
DI_ERR_CODE DI_VIDEO_SetOutputSettings (DI_VIDEO_AOUT_MODE_t stOutputMode);
DI_ERR_CODE DI_VIDEO_SetWSS (HBOOL bOn, DI_VIDEO_WSS_SETTINGS_t stSettings);
DI_ERR_CODE DI_VIDEO_GetWSS (DI_VIDEO_WSS_SETTINGS_t *pstSettings);
DI_ERR_CODE DI_VIDEO_SetCGMS(HBOOL bOn, DI_VIDEO_CGMS_SETTINGS tCgmsSettings, DI_VIDEO_WIN_TYPE eVidWin);
DI_ERR_CODE DI_VIDEO_SetVBITeletext (HBOOL bOn);
DI_ERR_CODE DI_VIDEO_SetVBIClosedCaption (HBOOL bOn);
DI_ERR_CODE DI_VIDEO_SetMacroVision (DI_VIDEO_MV_MODE eMode);
DI_ERR_CODE DI_VIDEO_RegisterEventCallback(HUINT32 ulDeviceId, DI_VIDEO_NOTIFY_EVT nEvent, EVENT_CALLBACK_FUNCTION fnVideoCallback, HUINT32* pulCallerId);
DI_ERR_CODE DI_VIDEO_RegisterCcDataCallback(HUINT32 ulDeviceId, pfnDI_VIDEO_CcDataCallback fnCallback);
DI_ERR_CODE DI_VIDEO_PlayMemoryStream (	HUINT32 ulDeviceId, DI_VIDEO_STREAM_TYPE eStreamType, DI_VIDEO_FORMAT nDiVideoFmt, HUINT8 *pucMemBuff, HULONG ulMemBuffSize, void* stTarget);
DI_ERR_CODE DI_VIDEO_StopMemoryStream (HUINT32 ulDeviceId);
DI_ERR_CODE DI_VIDEO_StartCapture (HUINT32 ulDeviceId, HUINT16 usTargetWidth, HUINT16 usTargetHeight, HUINT8 *pcuCapBitmap);
DI_ERR_CODE DI_VIDEO_StopCapture (HUINT32 ulDeviceId);
DI_ERR_CODE DI_VIDEO_GetSequenceHeaderInfo(HUINT32 ulDeviceId, DI_VIDEO_StreamInfo_t *pstStreamInfo);
DI_ERR_CODE DI_VIDEO_SetEventTimeOut(HUINT32 ulDeviceId, HUINT32 ulTimeout);
DI_ERR_CODE DI_VIDEO_GetAfdInfo(HUINT32 ulDeviceId, DI_VIDEO_ACTIVE_FORMAT *peActiveFormat);
DI_ERR_CODE DI_VIDEO_ControlDPort(HUINT32 ulDeviceId, DI_VIDEO_HD_RESOLUTION eHdResolution, DI_VIDEO_AspectRatio_t eAspectRatio);
DI_ERR_CODE DI_VIDEO_SetSync(HBOOL bOnOff);
DI_ERR_CODE DI_VIDEO_GetTrickState(void *handles, void *trickState);
DI_ERR_CODE DI_VIDEO_SetTrickState(void *handles, const void *trickState);
DI_ERR_CODE DI_VIDEO_StartScreenCapture(HUINT32 ulDeviceId, DI_VIDEO_CAPTURE_t eCap, HUINT16 tgtWidth, HUINT16 tgtHeight, HUINT8 *CapturedBitmap);
DI_ERR_CODE DI_VIDEO_GetThumbnail(HUINT32 ulDeviceId, HUINT16 tgtWidth, HUINT16 tgtHeight, HUINT8 *CapturedBitmap);
DI_ERR_CODE DI_VIDEO_EnableOutput(HBOOL bEnable);
DI_ERR_CODE DI_VIDEO_Enable3D(HUINT32 ulDeviceId, HBOOL bEnable);
DI_ERR_CODE DI_VIDEO_Set3DFormat(HUINT32 ulDeviceId, DI_VIDEO_3D_FORMAT e3dFormat);
DI_ERR_CODE DI_VIDEO_Get3DInfo(HUINT32 ulDeviceId, DI_VIDEO_3D_FORMAT *pe3dFormat);
DI_ERR_CODE DI_VIDEO_GetCurrentDecoderStatus(HUINT32 ulDeviceId, DI_VIDEO_DECODER_STATUS_t *pstVideoDecoderStatus);
DI_ERR_CODE DI_VIDEO_GetCodecCapability(DI_VIDEO_FORMAT eVideoCodec, DI_VIDEO_CODEC_CAP_t *pstCodecCapInfo);
DI_ERR_CODE DI_VIDEO_GetMacroVisionVersion(DI_VIDEO_MACROVISION_VERSION_TYPE *peMacrovisonVersion);
DI_ERR_CODE DI_VIDEO_GetOutputCapability(DI_VIDEO_OUTPUT_CAP_t *pstOuputCapInfo);
DI_ERR_CODE DI_VIDEO_StartMosaic(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec);
DI_ERR_CODE DI_VIDEO_StopMosaic(HUINT32 ulDeviceId);
DI_ERR_CODE DI_VIDEO_ShowMosaicWindow(HUINT32 ulDeviceId, HUINT32 ulMosaicWindowIndex, HBOOL bShow);
DI_ERR_CODE DI_VIDEO_SetMosaicWindowSettings(HUINT32 ulDeviceId, HUINT32 ulMosaicWindowIndex, DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pOutRect, DI_VIDEO_WIN_TYPE eVidWin);
DI_ERR_CODE DI_VIDEO_SetChannelChangeMode(HUINT32 ulDeviceId, DI_VIDEO_CC_MODE eMode);
DI_ERR_CODE DI_VIDEO_GetDecoderHandle(HUINT32 ulDeviceId,void *handle);
DI_ERR_CODE DI_VIDEO_GetWindowHandle(HUINT32 ulDeviceId,void *handle_hd,void *handle_sd);
DI_ERR_CODE DI_VIDEO_SetCaptureWindow(HUINT32 ulDeviceId,HBOOL bCapture);
DI_ERR_CODE DI_VIDEO_SetGameModeWindow(HUINT32 ulDeviceId,HBOOL bGame);
DI_ERR_CODE DI_VIDEO_GetVideoPts(HUINT32 *ulPts);
DI_ERR_CODE DI_VIDEO_SetDecoderMode(HUINT32 ulDeviceId, DI_VIDEO_DECODER_MODE eMode);
DI_ERR_CODE DI_VIDEO_SetWindowVisible(HUINT32 ulDeviceId, HBOOL bVisible);
DI_ERR_CODE DI_VIDEO_SetClosedCaptionDataFeed(HUINT32 ulDeviceId, HBOOL bOn);
DI_ERR_CODE DI_VIDEO_GetClosedCaptionDataState(HUINT32 ulDeviceId, DI_VIDEO_CC_DATA_STATE *eState);
DI_ERR_CODE DI_VIDEO_DelayOutput(HUINT32 ulDeviceId, HINT32 nDelayMs);
#endif /* _DI_VIDEO_H_ */

/** @} */ // end of DI_VIDEO


