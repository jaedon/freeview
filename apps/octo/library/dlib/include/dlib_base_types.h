/* $Header: $ */

#ifndef DLIB_BASE_TYPES_H__
#define DLIB_BASE_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#define	DxNAMEOF(type)	#type
#define	DxGROUP_LEN							16
#define	DxBOUQUET_LEN						16
#define	DxNAME_LEN							256
#define	DxANTINFO_NAME_LEN						24
#define	DxCAMID_LEN							8
#define	DxLANG_CODE_LEN						4

#define	DxFSAT_LANG_CODE_LEN				3
#define	DxDEFAULT_AUTHORITY_LEN				33
#define	DxCRID_DATA_LEN						29
#define	DxCRID_IMI_LEN						 3
#if defined(CONFIG_OP_TDC)
#define	DxCRID_LEN							64	//</><1><8><0~54>	: max 64 bytes
#else
#define	DxCRID_LEN							(DxDEFAULT_AUTHORITY_LEN+ DxCRID_DATA_LEN+ DxCRID_IMI_LEN + 3)

#endif

#if defined(CONFIG_OP_UK_DTT_DBOOK_VER_5)
// UK Freesat specification and ZIGGO testing
#define DxCRID_TYPE_PROG					(0x31)
#define	DxCRID_TYPE_SERIES					(0x32)
#define DxCRID_TYPE_RECOMMEND				(0x33)
#define DxCRID_TYPE_ALL						(0x3F)
#else
// TS 102 323 - Base TVAnytime CRID Type
#define DxCRID_TYPE_PROG					(0x01)
#define	DxCRID_TYPE_SERIES					(0x02)
#define DxCRID_TYPE_RECOMMEND				(0x03)
#define DxCRID_TYPE_ALL						(0x0F)
#endif

#define	DxFSAT_SVC_SHORT_NAME_LEN			(27)
#define	DxFSAT_SVC_GUIDANCE_LEN				(255)
#define	DxFSAT_SVC_PREFIX_URI_LEN			(255)
#define	DxBATCH_PROC_MAX_COUNT				16


#define 	DxJP_MAX_AFFILIATION_ID_NUM			16
#define 	DxJP_MAX_TS_NAME_LEN				31 /* 20 spec * 1.5(code convert buffer) + 1	(30+1)	 only for ARIB Digital Terrestrial, MAX_TS_NAME_LEN와 동일 */

#define 	DxLCN_SVC_LIST_NAME_LEN			64

#define 	DxDB_APPL_TIME_REGION_ID				"region_id"

#define 	DxDB_NETWORK_ID_FOR_NETWORK_SEARCH				"network_id"


//	SVC Operator Type
typedef enum
{
	eDxOPTYPE_ALL			= 0,
	eDxOPTYPE_NONE		    = 1,
	eDxOPTYPE_APSHD		    = 2,
	eDxOPTYPE_FREESAT		= 3,
	eDxOPTYPE_CANALSAT	    = 4,
	eDxOPTYPE_NTVPLUS		= 5,
	eDxOPTYPE_UPC			= 6,
	eDxOPTYPE_ADTV		    = 7,
	eDxOPTYPE_ALJAZEERA	    = 8,
	eDxOPTYPE_CNE			= 9,
	eDxOPTYPE_DIGITURK	    = 10,
	eDxOPTYPE_NONDIGITURK	= 11,
	eDxOPTYPE_TIVUSAT		= 12,
	eDxOPTYPE_MOSTELECOM	= 13,
	eDxOPTYPE_MAX 	    	= 0XFF
} DxOPTYPE_e;

//	Global Operator Type
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eDxOPERATOR_None		= 0,
	eDxOPERATOR_NOT_DEFINED	= 1,
	eDxOPERATOR_Ziggo		= 2,
	eDxOPERATOR_Comhem		= 3,
	eDxOPERATOR_KDG			= 4,
	eDxOPERATOR_SkyD		= 5,
	eDxOPERATOR_Ses	= 6,
	eDxOPERATOR_FiCom 		= 7,
	eDxOPERATOR_Freesat		= 8,
	eDxOPERATOR_CanalReady	= 9,
	eDxOPERATOR_Rcs			= 10,
	eDxOPERATOR_NtvPlus		= 11,
	eDxOPERATOR_BBC			= 12,
	eDxOPERATOR_UPC			= 13,
	eDxOPERATOR_Digiturk	= 14,
	eDxOPERATOR_ADTV		= 15,
	eDxOPERATOR_Aljazeera	= 16,
	eDxOPERATOR_CNE			= 17,
	eDxOPERATOR_SpainTdt	= 18,
	eDxOPERATOR_Truevisions	= 19,
	eDxOPERATOR_Boxer		= 20,
	eDxOPERATOR_TIVU		= 21,
	eDxOPERATOR_JCOM		= 22,
	eDxOPERATOR_SKAPA		= 23,
	eDxOPERATOR_Mostelecom	= 24,
	eDxOPERATOR_SwissCanal	= 25,
	eDxOPERATOR_RussiaTvt 	= 26,
	eDxOPERATOR_Almajd		= 27,
	eDxOPERATOR_NOVA		= 28,
	eDxOPERATOR_SkyCable	= 29,
	eDxOPERATOR_GermanCab 	= 30,
	eDxOPERATOR_Primacom	= 31,
	eDxOPERATOR_VTC			= 32,
	eDxOPERATOR_ERtelecom 	= 33,
	eDxOPERATOR_MiddleEast 	= 34,
	eDxOPERATOR_ORF			= 35,
	eDxOPERATOR_Tdc			= 36,
	eDxOPERATOR_Telenor		= 37,
	eDxOPERATOR_AMX			= 38,
	eDxOPERATOR_MBC			= 39,
	/* operator가 추가되는 경우 계속 추가해가면 됨 */
	eDxOPERATOR_Max,
	eDxOPERATOR_All			= -1
} DxOperator_e;

typedef enum
{
	eDxANTCONN_ONECABLE		= 0,		// Satellite Loop through case
	eDxANTCONN_TWOSAME,					// Satellite dual same case
	eDxANTCONN_TWODIFF,					// Satellite dual different case
	eDxANTCONN_NONSATONECABLE,			// Terrestrial and CAble's One tuner case
	eDxANTCONN_NONSATTWOCABLE,			// Terrestrial and CAble's Two tuner case
	eDxANTCONN_MAX						 // max of antenna connection type,
} DxAntennaConnection_e;

typedef enum
{
	eDxANT_TYPE_LNB_ONLY			= 0,	/**< LNB only */
	eDxANT_TYPE_DISEQC				= 1,	/**< Diseqc switch, Motor */
	eDxANT_TYPE_SCD					= 2,	/**< Single Cable Distribution */
	eDxANT_TYPE_SMATV 				= 3,	/**< SMATV */
	eDxANT_TYPE_MONOBLOCK_LNB 		= 4
} DxAntennaType_e;

/* This is for use of bitwise-OR for capability. */
typedef	enum
{
	eDxDELIVERY_TYPE_ALL		= 0x00,
	eDxDELIVERY_TYPE_SAT		= 0x01,
	eDxDELIVERY_TYPE_TER		= 0x02,
	eDxDELIVERY_TYPE_CAB		= 0x04,
	eDxDELIVERY_TYPE_MULTICAST	= 0x08
} DxDeliveryType_b;

/* This is for use of one value enumerator. */
typedef DxDeliveryType_b	DxDeliveryType_e;

typedef enum
{
	eDxVIDEO_TYPE_ALL,
	eDxVIDEO_TYPE_SD,
	eDxVIDEO_TYPE_HD,
} DxVideoType_e;

/* This is for use of bitwise-OR for capability. */
typedef	enum
{
	eDxCAS_TYPE_All			= 0,
	eDxCAS_TYPE_FTA			= 0x0001,
	eDxCAS_TYPE_NAGRA		= 0x0002,
	eDxCAS_TYPE_IRDETO		= 0x0004,
	eDxCAS_TYPE_NDS			= 0x0008,
	eDxCAS_TYPE_VIACCESS	= 0x0010,
	eDxCAS_TYPE_CONAX		= 0x0020,
	eDxCAS_TYPE_MEDIAGUARD	= 0x0040,
	eDxCAS_TYPE_CRYPTOWORKS	= 0x0080,
	eDxCAS_TYPE_JPBCAS		= 0x0100,
	eDxCAS_TYPE_JPCCAS		= 0x0200,
	eDxCAS_TYPE_CIPLUS		= 0x0400,
	eDxCAS_TYPE_VERIMATRIX	= 0x0800,
	eDxCAS_TYPE_ALLCAS		= 0x1000,
} DxCasType_b;

/* This is for use of one value enumerator. */
typedef DxCasType_b	DxCasType_e;

/*
 * encode
 */
typedef enum
{
	eDxVIDEO_ENC_PROFILE_SIMPLE = 0,
	eDxVIDEO_ENC_PROFILE_MAIN,
	eDxVIDEO_ENC_PROFILE_HIGH,
	eDxVIDEO_ENC_PROFILE_ADVANCED,
	eDxVIDEO_ENC_PROFILE_JIZHUN,
	eDxVIDEO_ENC_PROFILE_SNRSCALABLE,
	eDxVIDEO_ENC_PROFILE_SPATIALLYSCALABLE,
	eDxVIDEO_ENC_PROFILE_ADVANCEDSIMPLE,
	eDxVIDEO_ENC_PROFILE_BASELINE,
	eDxVIDEO_ENC_PROFILE_AUTO, //depend on its bitrate

	eDxVIDEO_ENC_PROFILE_NONE,
	eDxVIDEO_ENC_PROFILE_MAX_NUM
} DxVideoEncodeProfile_e;

typedef enum
{
	eDxVIDEO_ENC_LEVEL_00 = 0,
	eDxVIDEO_ENC_LEVEL_10,
	eDxVIDEO_ENC_LEVEL_1B,
	eDxVIDEO_ENC_LEVEL_11,
	eDxVIDEO_ENC_LEVEL_12,
	eDxVIDEO_ENC_LEVEL_13,
	eDxVIDEO_ENC_LEVEL_20,
	eDxVIDEO_ENC_LEVEL_21,
	eDxVIDEO_ENC_LEVEL_22,
	eDxVIDEO_ENC_LEVEL_30,
	eDxVIDEO_ENC_LEVEL_31,
	eDxVIDEO_ENC_LEVEL_32,
	eDxVIDEO_ENC_LEVEL_40,
	eDxVIDEO_ENC_LEVEL_41,
	eDxVIDEO_ENC_LEVEL_42,
	eDxVIDEO_ENC_LEVEL_50,
	eDxVIDEO_ENC_LEVEL_51,
	eDxVIDEO_ENC_LEVEL_60,
	eDxVIDEO_ENC_LEVEL_62,
	eDxVIDEO_ENC_LEVEL_LOW,
	eDxVIDEO_ENC_LEVEL_MAIN,
	eDxVIDEO_ENC_LEVEL_HIGH,
	eDxVIDEO_ENC_LEVEL_HIGH1440,
	eDxVIDEO_ENC_LEVEL_AUTO, //depend on its bitrate

	eDxVIDEO_ENC_LEVEL_NONE,
	eDxVIDEO_ENC_LEVEL_MAX
} DxVideoEncodeLevel_e;


/*
 * decode
 */
typedef enum
{
	eDxVIDEO_CODEC_UNKNOWN,
	eDxVIDEO_CODEC_MPEG1,			/**< MPEG-1 Video (ISO/IEC 11172-2) */
	eDxVIDEO_CODEC_MPEG2, 			/**< MPEG-2 Video (ISO/IEC 13818-2) */
	eDxVIDEO_CODEC_MPEG4_PART2,			/**< MPEG-4 Part 2 Video */
	eDxVIDEO_CODEC_H261,			/**< H.261 Video. The value of the enum is not based on PSI standards. */
	eDxVIDEO_CODEC_H263,			/**< H.263 Video. The value of the enum is not based on PSI standards. */
	eDxVIDEO_CODEC_H264,			/**< H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	eDxVIDEO_CODEC_H264_SVC, 		/**< Scalable Video Codec extension of H.264  */
	eDxVIDEO_CODEC_H264_MVC, 		/**< Multi View Coding extension of H.264 */
	eDxVIDEO_CODEC_VC1, 			/**< VC-1 Advanced Profile */
	eDxVIDEO_CODEC_VC1_SIMPLEMAIN,	/**< VC-1 Simple & Main Profile */
	eDxVIDEO_CODEC_AVS,				/**< AVS coded video */
	eDxVIDEO_CODEC_RV40,			/**< RV 4.0 coded video */
	eDxVIDEO_CODEC_VP6,				/**< VP6 coded video */
	eDxVIDEO_CODEC_DIVX311,			/**< DivX 3.11 coded video */
	eDxVIDEO_CODEC_XVID,			/**< XVid coded video */
	eDxVIDEO_CODEC_H265,			/**< HEVC coded video */
	eDxVIDEO_CODEC_VP8,				/**< VP8 coded video */
	eDxVIDEO_CODEC_VP9,				/**< VP9 coded video */
	eDxVIDEO_CODEC_MAX_NUM
} DxVideoCodec_e;

typedef enum
{
	eDxAUDIO_CODEC_UNKNOWN,			/* unknown/not supported audio format */
	eDxAUDIO_CODEC_MPEG,			/* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
	eDxAUDIO_CODEC_MP3,				/* MPEG1/2, layer 3. */
	eDxAUDIO_CODEC_DOLBY_AC3,		/* Dolby Digital AC3 audio */
	eDxAUDIO_CODEC_DOLBY_AC3P,		/* Dolby Digital Plus (AC3+ or DDP) audio */
	eDxAUDIO_CODEC_PCM,				/* PCM audio - Generally used only with inputs such as SPDIF or HDMI. */
	eDxAUDIO_CODEC_AAC,				/* Advanced audio coding. Part of MPEG-4 */
	eDxAUDIO_CODEC_AAC_LOAS,		/* Advanced audio coding. Part of MPEG-4 */
	eDxAUDIO_CODEC_AAC_PLUS,		/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) with ADTS (Audio Data Transport Format) */
	eDxAUDIO_CODEC_AAC_PLUS_ADTS,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) sync and LATM mux */
	eDxAUDIO_CODEC_DTS,				/* Digital Digital Surround sound, uses non-legacy frame-sync */
	eDxAUDIO_CODEC_DTS_HD,			/* Digital Digital Surround sound, HD, uses non-legacy frame-sync, decodes only DTS part of DTS-HD stream */
	eDxAUDIO_CODEC_LPCM_DVD,		/* LPCM, DVD mode */
	eDxAUDIO_CODEC_LPCM_HD_DVD,		/* LPCM, HD-DVD mode */
	eDxAUDIO_CODEC_LPCM_BLUERAY,	/* LPCM, Blu-Ray mode */
	eDxAUDIO_CODEC_WMA_STD,			/* WMA Standard */
	eDxAUDIO_CODEC_WMA_PRO,			/* WMA Professional */
	eDxAUDIO_CODEC_PCMWAVE,			/* PCM audio with Wave header - Used with streams containing PCM audio */
	eDxAUDIO_CODEC_DRA,				/* Dynamic Resolution Adaptation.  Used in Blu-Ray and China Broadcasts. */
	eDxAUDIO_CODEC_AIFF,			/* Audio Interchange File Format */
	eDxAUDIO_CODEC_RIFF,			/* Resource Interchange File Format. *.wav */
	eDxAUDIO_CODEC_FLAC,			/* Free Lossless Audio Codec */
	eDxAUDIO_CODEC_ADPCM,			/* Adaptive Differential Pulse-Code Modulation */
	eDxAUDIO_CODEC_VORBIS,			/* Vorbis is a free and open-source software project headed by the Xiph. */
	eDxAUDIO_CODEC_APE,				/* Monkey's Audio Lossless Audio */
	eDxAUDIO_CODEC_OPUS,			/* OPUS codec */
	eDxAUDIO_CODEC_MAX_NUM
} DxAudioCodec_e;

/* This is for use of bitwise-OR for capability. */
typedef enum
{
	eDxSCART_FORMAT_UNKNOWN	= 0x0,
	eDxSCART_FORMAT_CVBS	= 0x1,
	eDxSCART_FORMAT_RGB		= 0x2,
	eDxSCART_FORMAT_SVIDEO	= 0x4
} DxScartFormat_b;

/* This is for use of one value enumerator. */
typedef DxScartFormat_b	DxScartFormat_e;

typedef enum
{
	eDxSCART_PATH_UNKNOWN,
	eDxSCART_PATH_STB2TV,
	eDxSCART_PATH_VCR2TV,
	eDxSCART_PATH_HWBYPASS,
	eDxSCART_PATH_MAX_NUM
} DxScartPath_e;

typedef enum
{
	eDxSTEREO_FORMAT_UNKNOWN,
	eDxSTEREO_FORMAT_2CH,
	eDxSTEREO_FORMAT_MULTI,
	eDxSTEREO_FORMAT_MONO,
	eDxSTEREO_FORMAT_DUALMONO,
	eDxSTEREO_FORMAT_MAX_NUM
} DxStereoFormat_e;

typedef enum
{
	eDxSTEREO_SELECT_UNKNOWN,
	eDxSTEREO_SELECT_STEREO,
	eDxSTEREO_SELECT_MONOLEFT,
	eDxSTEREO_SELECT_MONORIGHT,
	eDxSTEREO_SELECT_MONOMIXED,
	eDxSTEREO_SELECT_MAX_NUM
} DxStereoSelect_e;

typedef enum
{
	eDxDUALMONO_SELECT_UNKNOWN,
	eDxDUALMONO_SELECT_BOTH,
	eDxDUALMONO_SELECT_LEFT,
	eDxDUALMONO_SELECT_RIGHT,
	eDxDUALMONO_SELECT_MAX_NUM
} DxDualMonoSelect_e;

typedef enum
{
	eDxDIGITALAUDIO_FORMAT_UNKNOWN,
	eDxDIGITALAUDIO_FORMAT_PCM,
	eDxDIGITALAUDIO_FORMAT_MULTICHANNEL,
	eDxDIGITALAUDIO_FORMAT_MAX_NUM
} DxDigitalAudioFormat_e;

/* HDMI Audio SampleRate Flag. */
typedef enum
{
	eDxHDMI_AUDIO_SR_32KHz	= 0x01,
	eDxHDMI_AUDIO_SR_44KHz	= 0x02,
	eDxHDMI_AUDIO_SR_48KHz	= 0x04,
	eDxHDMI_AUDIO_SR_88KHz	= 0x08,
	eDxHDMI_AUDIO_SR_96KHz	= 0x10,
	eDxHDMI_AUDIO_SR_176KHz	= 0x20,
	eDxHDMI_AUDIO_SR_192KHz	= 0x40,
	eDxHDMI_AUDIO_SR_Max	= 0x80
} DxHdmiAudioSR_e;

/* This is for use of bitwise-OR for capability. */
typedef enum
{
	eDxVIDEO_3DFORMAT_NONE 			= 0x0000,
	eDxVIDEO_3DFORMAT_SIDEBYSIDE	= 0x0001,
	eDxVIDEO_3DFORMAT_TOPANDBOTTOM 	= 0x0002,
	eDxVIDEO_3DFORMAT_FRAMEPACKING	= 0x0004
} DxVideo3DFormat_b;


/* CEC Specs*/
typedef enum
{
	eDxHDMI_USER_CTRL_VOLUME_UP = 0x41,
    eDxHDMI_USER_CTRL_VOLUME_DOWN = 0x42,
    eDxHDMI_USER_CTRL_VOLUME_MUTE = 0x43,
	eDxHDMI_USER_CTRL_PRESSED_MAX_NUM = 0xff,
} DxHdmiUserCtrlPressed_e;


/* This is for use of one value enumerator. */
typedef DxVideo3DFormat_b	DxVideo3DFormat_e;

typedef enum
{
	eDxHDMI_3DSELECT_OFF,
	eDxHDMI_3DSELECT_SIDEBYSIDE,
	eDxHDMI_3DSELECT_TOPANDBOTTOM,
	eDxHDMI_3DSELECT_FRAMEPACKING,
	eDxHDMI_3DSELECT_AUTO,
	eDxHDMI_3DSELECT_MAX_NUM
} DxHdmi3DSelect_e;

typedef enum
{
	eDxRESOLUTION_SELECT_UNKNOWN,
	eDxRESOLUTION_SELECT_480I,
	eDxRESOLUTION_SELECT_576I,
	eDxRESOLUTION_SELECT_480P,
	eDxRESOLUTION_SELECT_576P,
	eDxRESOLUTION_SELECT_720P,
	eDxRESOLUTION_SELECT_1080I,
	eDxRESOLUTION_SELECT_1080P,
	eDxRESOLUTION_SELECT_2160P,
	eDxRESOLUTION_SELECT_ORIGINAL,
	eDxRESOLUTION_SELECT_EDID_MAX,
	eDxRESOLUTION_SELECT_MAX_NUM
} DxResolutionSelect_e;

/* This is for use of bitwise-OR for capability. */
typedef enum
{
	eDxRESOLUTION_STATUS_UNKNOWN	= 0x0,
	eDxRESOLUTION_STATUS_480I		= 0x1,
	eDxRESOLUTION_STATUS_576I		= 0x2,
	eDxRESOLUTION_STATUS_480P		= 0x4,
	eDxRESOLUTION_STATUS_576P		= 0x8,
	eDxRESOLUTION_STATUS_720P		= 0x10,
	eDxRESOLUTION_STATUS_1080I		= 0x20,
	eDxRESOLUTION_STATUS_1080P		= 0x40,
	eDxRESOLUTION_STATUS_2160P		= 0x80		/* 3840 x 2160 p */
} DxResolutionStatus_b;

/* This is for use of one value enumerator. */
typedef DxResolutionStatus_b	DxResolutionStatus_e;

typedef enum
{
	eDxASPECTRATIO_UNKNOWN,
	eDxASPECTRATIO_4X3,
	eDxASPECTRATIO_16X9,
	eDxASPECTRATIO_14X9,
	eDxASPECTRATIO_1X1,
	eDxASPECTRATIO_MAX_NUM
} DxAspectRatio_e;

typedef enum
{
	eDxPICTURERATE_UNKNOWN,
	eDxPICTURERATE_24P,
	eDxPICTURERATE_25P,
	eDxPICTURERATE_30P,
	eDxPICTURERATE_50I,
	eDxPICTURERATE_60I,
	eDxPICTURERATE_50P,
	eDxPICTURERATE_60P,
	eDxPICTURERATE_MAX,
} DxPictureRate_e;

typedef enum
{
	eDxDFC_SELECT_UNKNOWN,
/* 방송 사양에서 정의한 방법에 의해 아래 자동 모드 중 하나로 치환될 것임. 사양별로 config 필요. */
	eDxDFC_SELECT_DEFAULT,
/* Aspect ratio 파라미터 간의 계산에 의한 자동 조정 모드 */
	eDxDFC_SELECT_WIDESIGNAL,			/**< scaling 없이 WSS나 AFD 등의 wide signal을 사용한다. */
	eDxDFC_SELECT_SQUEEZE,				/**< 16:9 소스는 4:3 출력에 4:3 Letter box, 4:3소스는 16:9출력에 16:9 pillar box */
	eDxDFC_SELECT_SQUEEZE149,			/**< 16:9 소스는 4:3 출력에 14:9 Letter box, 4:3소스는 16:9출력에 14:9 pillar box */
	eDxDFC_SELECT_CENTERCUTOUT, 		/**< 16:9 소스는 4:3 출력에 4:3 side cut, 4:3소스는 16:9출력에 16:9 zoom */
	eDxDFC_SELECT_CENTERCUTOUT149, 		/**< 16:9 소스는 4:3 출력에 14:9 side cut, 4:3소스는 16:9출력에 14:9 zoom */
/* DFC 직접 지정에 의한 강제 조정 모드 */
	eDxDFC_SELECT_FULLSCREEN, 			/**< 출력 aspect ratio에 full stretch. */
	eDxDFC_SELECT_LETTERBOX,
	eDxDFC_SELECT_LETTERBOX149,
	eDxDFC_SELECT_PILLARBOX,
	eDxDFC_SELECT_PILLARBOX149,
	eDxDFC_SELECT_ZOOM,
	eDxDFC_SELECT_ZOOM149,
	eDxDFC_SELECT_SIDECUT,
	eDxDFC_SELECT_SIDECUT149,
	eDxDFC_SELECT_MAX_NUM
} DxDfcSelect_e;

typedef enum
{
	eDxDFC_STATUS_FULL			= 0,
	eDxDFC_STATUS_LETTERBOX		= 0x1,
	eDxDFC_STATUS_LETTERBOX149	= 0x2,
	eDxDFC_STATUS_PILLARBOX		= 0x4,
	eDxDFC_STATUS_PILLARBOX149	= 0x8,
	eDxDFC_STATUS_ZOOM			= 0x10,
	eDxDFC_STATUS_ZOOM149		= 0x20,
	eDxDFC_STATUS_SIDECUT		= 0x40,
	eDxDFC_STATUS_SIDECUT149	= 0x80
} DxDfcStatus_b;

typedef enum
{
	eDxCOMPOSITE_STANDARD_UNKNOWN,
	eDxCOMPOSITE_STANDARD_PAL,
	eDxCOMPOSITE_STANDARD_PAL_M,
	eDxCOMPOSITE_STANDARD_PAL_N,
	eDxCOMPOSITE_STANDARD_NTSC,
	eDxCOMPOSITE_STANDARD_SECAM,
	eDxCOMPOSITE_STANDARD_MAX_NUM
} DxCompositeStandard_e;

typedef enum
{
	eDxHDCP_VERSION_1_0	= 0,
	eDxHDCP_VERSION_1_1,
	eDxHDCP_VERSION_1_2,
	eDxHDCP_VERSION_1_3,
	eDxHDCP_VERSION_1_4,
	eDxHDCP_VERSION_2_0,
	eDxHDCP_VERSION_2_1,
	eDxHDCP_VERSION_2_2,
	eDxHDCP_VERSION_MAX,
} DxHdcpVersion_e;

/* Netflix DRM Status for HDCP*/
typedef enum
{
	eNFLIX_DRM_NA	= 0,
    eNFLIX_DRM_INIT,
    eNFLIX_DRM_RUNNING,
    eNFLIX_DRM_MAX,
} DxNflixDrmStatus_e;

typedef enum
{
	eDxHDCPSTATUS_OFF,
	eDxHDCPSTATUS_TRY,
	eDxHDCPSTATUS_OK,
	eDxHDCPSTATUS_FAIL,
	eDxHDCPSTATUS_NODEVICE,
	eDxHDCPSTATUS_MAX_NUM
} DxHdcpStatus_e;

/*
	Digital Audio Output 에 SCMS Control Mode For CI Plus(EMI) Requirement.
	embedded cas 등에서 digtial audio copyright control 시에도 사용 한다.

	digital audio 에 대해서 protection on/off가 다 인 경우, copy free와 copy never를 사용한다.
*/
typedef enum
{
	//CopyRight_t 와 같음
	eDxCOPYRIGHT_COPY_FREE,
	eDxCOPYRIGHT_COPY_ONCE,
	eDxCOPYRIGHT_COPY_NOMORE,
	eDxCOPYRIGHT_COPY_NEVER
} DxCopyRight_e;

typedef enum
{
	eDxLEDSTATUS_OFF,
	eDxLEDSTATUS_ON,
	eDxLEDSTATUS_BLINK,
	eDxLEDSTATUS_OTHER
} DxLedStatus_e;

typedef enum
{
	eDxRGBLED_COLOR_NONE,
	eDxRGBLED_COLOR_RED,
	eDxRGBLED_COLOR_GREEN,
	eDxRGBLED_COLOR_BLUE,
	eDxRGBLED_COLOR_YELLOW,
	eDxRGBLED_COLOR_MAX
}DxRgbLedColor_e;

typedef enum
{
	eDxPowerLED_OFF,
	eDxPowerLED_WATCHING,
	eDxPowerLED_STANDBY
} DxPowerLedStatus_e;

typedef enum
{
	eDxRGBLED_DIMMLEVEL_SOLID = 0,
	eDxRGBLED_DIMMLEVEL_DIMMED = 75,
	eDxRGBLED_DIMMLEVEL_OFF = 100,
}DxRgbLedDimmLevel_e;

typedef enum
{
	eDxPANEL_CATEGORY_LIVE		= 0x00000001,
	eDxPANEL_CATEGORY_MEDIA		= 0x00000002,
	eDxPANEL_CATEGORY_PVR		= 0x00000004,
	eDxPANEL_CATEGORY_AUDIO		= 0x00000008,
	eDxPANEL_CATEGORY_SYSTEM	= 0x00000010,

	eDxPANEL_CATEGORY_NONE		= 0x10000000,
} DxPanelCategory_e;

typedef enum
{
	eDxLED_ID_POWER,

/* Service type TV/Radio */
	eDxLED_ID_TV,
	eDxLED_ID_RADIO,		// or no signal in digiturk OP.

/* PVR */
	eDxLED_ID_REC,
	eDxLED_ID_PLAY,
	eDxLED_ID_PAUSE,
	eDxLED_ID_RESERVE,

/* Audio */
	eDxLED_ID_SRS,
	eDxLED_ID_SRC_TS,
	eDxLED_ID_SRC_XT,
	eDxLED_ID_DOLBY,

/* Video */
	eDxLED_ID_HD,
	eDxLED_ID_1080P,
	eDxLED_ID_720P,
	eDxLED_ID_576P,
	eDxLED_ID_480P,
	eDxLED_ID_1080I,
	eDxLED_ID_576I,
	eDxLED_ID_480I,

/* 추후 추가된 Spec이라 아래에 추가함. */
/* Audio */
	eDxLED_ID_DOLBY_PLUS,		// AUDIO가 DOLBY+일때 사용하는 LED
	eDxLED_ID_DOLBY_PULSE,		// AUDIO가 DOLBY PULSE일때 사용하는 LED

/* Network related */
	eDxLED_ID_WIFI,				// WIFI가 연결 되었을때 사용하는 LED

/* USB */
	eDxLED_ID_USB,				// USB가 연결 되었을때 사용하는 LED

/* HDD */
/*	eDxLED_ID_HDD_EMPTY,		// HDD Format후의 Icon표시. -> VFD에 EMPTY가 없음. */
	eDxLED_ID_HDD_25_FULL,		// HDD가 25%정도 찾을때의 표시
	eDxLED_ID_HDD_50_FULL,		// HDD가 50%정도 찾을때의 표시
	eDxLED_ID_HDD_75_FULL,		// HDD가 75%정도 찾을때의 표시
	eDxLED_ID_HDD_100_FULL,		// HDD가 100%정도 찾을때의 표시

/* CABLE */
	eDxLED_ID_LINK,				// 어떤 용도인지 명확치 않아 표시 이름을 사용.

/* DATA */
	eDxLED_ID_DATA,			// 어떤 용도인지 명확치 않아 표시 이름을 사용.

/* ETC  */
	eDxLED_ID_MAIL,				// 사용자에게 알림 메시지가 떴을때 사용하기 위한 LED
	eDxLED_ID_RED_COLOR,		// VFD에 RED Color가 보여지게 할 경우.
	eDxLED_ID_BLUE_COLOR,		// VFD에 BLUE Color가 보여지게 할 경우.
	eDxLED_ID_ORANGE_COLOR,	// VFD에 ORANGE Color가 보여지게 할 경우.

/* RGB LED */
	eDxLED_ID_RGB_REC,			// Rec dedicated RGB LED
	eDxLED_ID_RGB_NET,			// Net dedicated RGB LED

	NUM_MAX_DxLED
} DxLedId_e;

#if 0
typedef enum
{
	eDxPANEL_STR_PRIORITY_BG_ACTION		=	0,
	eDxPANEL_STR_PRIORITY_VIEW_ACTION,
	eDxPANEL_STR_PRIORITY_POPUP,
	eDxPANEL_STR_PRIORITY_WEB,
	eDxPANEL_STR_PRIORITY_MEDIA,
	eDxPANEL_STR_PRIORITY_EPG,
	eDxPANEL_STR_PRIORITY_MENU,
	eDxPANEL_STR_PRIORITY_WZD,
	eDxPANEL_STR_PRIORITY_CHLIST,
	eDxPANEL_STR_PRIORITY_MENU_POPUP,
	eDxPANEL_STR_PRIORITY_OTA,
	eDxPANEL_STR_PRIORITY_REC_ACTION,
	eDxPANEL_STR_PRIORITY_VOLUME,
	eDxPANEL_STR_PRIORITY_TOP,
	NUM_MAX_DxPanelStringPriority_e
} DxPanelStringPriority_e;
#endif

typedef	enum
{
	eDxPANEL_ALIGN_LEFT,
	eDxPANEL_ALIGN_CENTER,
	eDxPANEL_ALIGN_RIGHT,
	NUM_MAXDxPanelAlign_e
} DxPanelAlign_e;


/* This is for use of bitwise-OR for capability. And is make same with dlib_service.h */
typedef	enum
{
	eDxAlphabetGroup_AlphabetAll	= 	0,
	eDxAlphabetGroup_AlphabetA		=	0x41,	/** Display All channel list starting with alphabet A */
	eDxAlphabetGroup_AlphabetB		=	0x42,	/** Display All channel list starting with alphabet B */
	eDxAlphabetGroup_AlphabetC		=	0x43,	/** Display All channel list starting with alphabet C */
	eDxAlphabetGroup_AlphabetD		=	0x44,	/** Display All channel list starting with alphabet D */
	eDxAlphabetGroup_AlphabetE		=	0x45,	/** Display All channel list starting with alphabet E */
	eDxAlphabetGroup_AlphabetF		=	0x46,	/** Display All channel list starting with alphabet F */
	eDxAlphabetGroup_AlphabetG		=	0x47,	/** Display All channel list starting with alphabet G */
	eDxAlphabetGroup_AlphabetH		=	0x48,	/** Display All channel list starting with alphabet H */
	eDxAlphabetGroup_AlphabetI		=	0x49,	/** Display All channel list starting with alphabet I */
	eDxAlphabetGroup_AlphabetJ		=	0x4A,	/** Display All channel list starting with alphabet J */
	eDxAlphabetGroup_AlphabetK		=	0x4B,	/** Display All channel list starting with alphabet K */
	eDxAlphabetGroup_AlphabetL		=	0x4C,	/** Display All channel list starting with alphabet L */
	eDxAlphabetGroup_AlphabetM		=	0x4D,	/** Display All channel list starting with alphabet M */
	eDxAlphabetGroup_AlphabetN		=	0x4E,	/** Display All channel list starting with alphabet N */
	eDxAlphabetGroup_AlphabetO		=	0x4F,	/** Display All channel list starting with alphabet O */
	eDxAlphabetGroup_AlphabetP		=	0x50,	/** Display All channel list starting with alphabet P */
	eDxAlphabetGroup_AlphabetQ		=	0x51,	/** Display All channel list starting with alphabet Q */
	eDxAlphabetGroup_AlphabetR		=	0x52,	/** Display All channel list starting with alphabet R */
	eDxAlphabetGroup_AlphabetS		=	0x53,	/** Display All channel list starting with alphabet S */
	eDxAlphabetGroup_AlphabetT		=	0x54,	/** Display All channel list starting with alphabet T */
	eDxAlphabetGroup_AlphabetU		=	0x55,	/** Display All channel list starting with alphabet U */
	eDxAlphabetGroup_AlphabetV		=	0x56,	/** Display All channel list starting with alphabet V */
	eDxAlphabetGroup_AlphabetW		=	0x57,	/** Display All channel list starting with alphabet W */
	eDxAlphabetGroup_AlphabetX		=	0x58,	/** Display All channel list starting with alphabet X */
	eDxAlphabetGroup_AlphabetY		=	0x59,	/** Display All channel list starting with alphabet Y */
	eDxAlphabetGroup_AlphabetZ		=	0x5A,	/** Display All channel list starting with alphabet A */
	eDxAlphabetGroup_AlphabetDigit	=	0x5B,	/** Display All channel list starting with digit(0~9) */
	eDxAlphabetGroup_AlphabetEtc	=	0x5C	/** Display All channel list starting exclusion (A~Z) and (0~9) */
} DxAlpabetGruopType_b;

typedef enum
{
	eDxFAC_DEF_TYPE_NONE	= 0,
	eDxFAC_DEF_TYPE_ALL,
	eDxFAC_DEF_TYPE_KEEP
}DxFactoryDefault_type_e;

typedef enum
{
	eDxWAKEUP_NONE			= 0,
	eDxWAKEUP_KEY,
	eDxWAKEUP_TIMER,
	eDxWAKEUP_ACPOWER,
	eDxWAKEUP_HDMICEC,
	eDxWAKEUP_REBOOT
}DxWakeUpReason_e;


// MHEG
#if 1
typedef enum
{
	eDxMHEGEVT_NONE, //eMhegApkEvt_None,
	eDxMHEGEVT_FLIPWIN, //eMhegApkEvt_FlipWindow,
	eDxMHEGEVT_LAUNCHAPP, //eMhegApkEvt_LaunchApp,
	eDxMHEGEVT_APPSTATE, //eMhegApkEvt_MhegAppState,
	eDxMHEGEVT_END
}	DxMhegEvent_e; //  APKE_MhegCtrl_Evt_e;
#endif
typedef enum
{
	eDxWAKEUP_2RD_UNDECIED		= 0,
	eDxWAKEUP_2RD_WATCHTV
} DxWakeUp2rdReason_e;


typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eDxRATING_VIEW_ALL		= 0,
	eDxRATING_AGE_4			= 1,		/* EN 300 468 - Parental rating descriptor (value = rating + 3 years) */
	eDxRATING_AGE_5			= 2,
	eDxRATING_AGE_6			= 3,
	eDxRATING_AGE_7			= 4,
	eDxRATING_AGE_8			= 5,
	eDxRATING_AGE_9			= 6,
	eDxRATING_AGE_10		= 7,
	eDxRATING_AGE_11		= 8,
	eDxRATING_AGE_12		= 9,
	eDxRATING_AGE_13		= 10,
	eDxRATING_AGE_14		= 11,
	eDxRATING_AGE_15		= 12,
	eDxRATING_AGE_16		= 13,
	eDxRATING_AGE_17		= 14,
	eDxRATING_AGE_18		= 15,
	eDxRATING_AGE_19		= 16,  /* japan sepecific 19age */
	eDxRATING_AGE_20		= 17,  /* japan sepecific adult */

// CONFIG_OP_SPAIN_TDT >>>
	eDxRATING_AGE_ALL_AGE	= 0x1D,	/* Spain specific. For all ages. 설정시 이것 이외에는 PIN 입력. 현재는 4세 rating과 동작이 같다. */
	eDxRATING_AGE_CHILDREN	= 0x1E,	/* Spain specific. Especially rated for children. 설정시 이것 이외에는 PIN 입력. 현재는 4세 rating과 동작이 같다. */
	eDxRATING_AGE_X_RATED	= 0x1F ,	/* Spain specific. View all 설정과 동작이 같고, 18세 초과 rating임. */

// TODO: 필요시 core팀에 문의	eDxRATING_AGE_ANY_PUBLIC	= 0x10, 	/* France specific. 4세 설정 및 4세 rating과 동작이 같다. */
	eDxRATING_AGE_LOCK_ALL	= 0x7F 		/* block all. */
} DxRatingAge_e;

// Freesat에서 사용
//CONFIG_OP_FREESAT
typedef enum
{ /* [WARN] forbid to change a db-saving enum value */
	eDxGUIDANCEPOLICY_OFF = 0,
	eDxGUIDANCEPOLICY_MARKEDCONTENTS,
	eDxGUIDANCEPOLICY_INTERNETCONTENTS,
	eDxGUIDANCEPOLICY_ALLCONTENTS,
}DxGuidancePolicy_e;

typedef enum
{
	eDxADULTCHANNEL_DISPLAY = 0,
	eDxADULTCHANNEL_HIDE,
}DxAdultChannelType_e;

typedef enum
{
	eDxSIMODE_FREESAT_G1,
	eDxSIMODE_FREESAT_G2,
	eDxSIMODE_UNKNOWN
} DxSIMode_e;


typedef enum
{
	eDxBAT_PROC_NONE 			= 0x00000000,
	eDxBAT_PROC_EMMUPDATE		= 0x00000001,
	eDxBAT_PROC_LCNCHECK		= 0x00000002,
	eDxBAT_PROC_OTACHECK		= 0x00000004,
	eDxBAT_PROC_EPGTVTV			= 0x00000008,
	eDxBAT_PROC_TIMERECOVERY	= 0x00000010,
	eDxBAT_PROC_DESC_REC		= 0x00000020,
	eDxBAT_PROC_SATRECUPDATE	= 0x00000040,
	eDxBAT_PROC_EPGGATHERING	= 0x00000080, // 모든 TP를 순회하면서.
	eDxBAT_PROC_DYNAMICUPDATE	= 0x00000100,
	eDxBAT_PROC_COLDBOOT		= 0x00000200,
	eDxBAT_PROC_MAX				= 0x80000000
}DxBatchProc_b;

typedef enum
{
	eDxBAT_TYPE_NONE,
	eDxBAT_TYPE_COLDBOOT,
	eDxBAT_TYPE_USER,
	eDxBAT_TYPE_STANDBY,
	eDxBAT_TYPE_MAX
}DxBatchType_e;

//CONFIG_MW_SATRECORD
typedef enum
{
	eDxSATREC_INTERVAL_OFF		= 0x00000000,
	eDxSATREC_INTERVAL_AUTO		= 0x10000000,
	eDxSATREC_INTERVAL_MANUAL	= 0x20000000
}DxSatRecIntervalType_e;

typedef struct
{
	HINT32		nX;
	HINT32		nY;
	HINT32		nW;
	HINT32		nH;
} DxRect_t;

typedef enum
{
	eDxPixelFormat_ARGB_8888,
	eDxPixelFormat_RGBA_8888,
	eDxPixelFormat_ARGB_4444,
	eDxPixelFormat_RGBA_4444,
	eDxPixelFormat_ARGB_1555,
	eDxPixelFormat_RGB_565,
	eDxPixelFormat_CLUT_8,
	eDxPixelFormat_A_8,
	eDxPixelFormat_YCBCR_444,
	eDxPixelFormat_YCBCR_422
} DxPixelFormat_e;

typedef enum
{
	eDxLsPriority_EQUAL_REC		= 0,
	eDxLsPriority_EQUAL_VIEW	= 1,
	eDxLsPriority_BELOW_VIEW	= 2,
	eDxLsPriority_UPPER_REC		= 3,

	eDxLsPriority_EndOfCase
} DxLiveStreamPriority_e;

typedef struct
{
	HUINT8 vendorID[2];		/* Vendor ID from Microsoft; compressed ASCII */
	HUINT8 productID[2];	/* Product ID assigned by Vendor */
	HUINT8 serialNum[4];	/* Serial # assigned by Vendor; may be undefined */
	HUINT8 monitorName[14];
	HUINT8 manufWeek;		/* Week of Manufture (1..53)			*/
	HINT16 manufYear;		/* Year of Manufacture + 1990 		*/
	HUINT8 edidVersion;		/* Version of Edid (version 1 or 2)	*/
	HUINT8 edidRevision;	/* Revision of Edid 1.3 or 2.0		*/
	HUINT32 firstVideoId;
	HUINT8 maxHorizSize;
	HUINT8 maxVertSize;
} DxHdmiEdidInfo_t;

extern HERROR	DLIB_BASE_Init(void);
extern HERROR	DLIB_BASE_Deprecated(void);

#ifdef __cplusplus
}
#endif

#endif // DLIB_BASE_TYPES_H__


