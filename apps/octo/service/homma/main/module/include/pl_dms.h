/**
 *************************************************************
 *	@file		roma_int.h
 *	HumaxDataManager internal header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/04/12
 *	@author			humax
 *	@brief			pl_dms external header
 *************************************************************
 */

#ifndef	__PL_DMS_H__
#define	__PL_DMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>
#include <define.h>
#include <common.h>

#include "common.h"

/* MXDLNA_DMS_ICON ????????*/


#define	PL_DMS_NAME_MAX				( 512 )

#define PL_DMS_TYPE_VIDEO 				0x100
#define PL_DMS_TYPE_AUDIO 				0x200
#define PL_DMS_TYPE_IMAGE 				0x300
#define PL_DMS_TYPE_TEXT 				0x400
#define PL_DMS_TYPE_SYSTEM 			0x500

#define	PL_DMS_NAME_MAX				( 512 )
#define PL_DMS_NAME_128				( 128 )
#define PL_DMS_NAME_64				( 64 )
#define PL_DMS_NAME_32				( 32 )
#define PL_DMS_URL_MAX				( 512 )

#define	PL_DMS_UDN_LEN				( 36 )			/* UDN?? 보통 8-4-4-4-12???????36????????? */
#define	PL_DMS_UDN_MAX				( 48 )			/* Prefix????????경우?????? Length */
#define PL_DMS_SCAN_PATH_MAX		( 32 )
#define PL_DMS_SCAN_DIR_MAX		( 24 )

#define PL_DMS_LIVE_STREAM_URL_MAX	( 1024 )
#define PL_DMS_LIVE_STREAM_MIME_MAX	( 256 )

#define PL_DMS_DEFAULT_MODEL_DESCRIPTION	"HUMAX Set-Top Box"

#define	PL_DMS_DEFAULT_UDN_PREFIX			"1E96FF1A-6771-2F44-A0C5-"
#define PL_DMS_DEFAULT_UDN					"1E96FF1A-6771-2F44-A0C5-123456"

#define PL_DMS_UNKNOWN_MODEL_NAME			"Unknown model"
#define PL_DMS_DEFAULT_MODEL_URL			"www.humaxdigital.com/"
#define	PL_DMS_DEFAULT_MODEL_NUMBER			"undefined"
#define	PL_DMS_DEFAULT_SERIAL_NUMBER		"undefined"

/* Default Service Port */
#define PL_DMS_DEFAULT_PORT_UPNP			( 50001 )
#define PL_DMS_DEFAULT_PORT_WEBSERVER		( 9000 )
#define PL_DMS_DEFAULT_PORT_DTCP			( 9001 )
#define PL_DMS_DEFAULT_PORT_UPLOAD			( 9002 )

/* Default ReScan Interval */
#define PL_DMS_DEFAULT_RESCAN_INTERVAL			( 4 * 60 * 60 * 1000 )	// hour * min * sec * milli
#define PL_DMS_DEFAULT_RESCAN_INTERVAL_FILE		( 100 / 5 )					// interval for item scanning, reduced from 100 mill-sec

#define PL_DMS_DEFAULT_THUMBNAIL_TMPNAME		"__thumbnailData"
#define PL_DMS_DEFAULT_THUMBNAIL_TMPNAME_LEN	( 15 )
//#define PL_DMS_DEFAULT_THUMBNAIL_PATH			NULL

#define PL_DMS_DEFAULT_SCANFILTER_MEDIA		"asf;wmv;divx;avi;mp4;m4v;mkv;ts;trp;tp;vob;mpg;mpeg;3gp;mov;flv;jpeg;jpg;png;gif;bmp;mp3;pcm;wma;m4a;wav;flac;"
#define PL_DMS_DEFAULT_SCANFILTER_VIDEO		"asf;wmv;divx;avi;mp4;m4v;mkv;ts;trp;tp;vob;mpg;mpeg;3gp;mov;flv;"
#define PL_DMS_DEFAULT_SCANFILTER_PHOTO		"jpeg;jpg;png;gif;bmp;"
#define PL_DMS_DEFAULT_SCANFILTER_MUSIC		"mp3;pcm;wma;m4a;wav;flac;"
#define PL_DMS_DEFAULT_SCANFILTER_RECORD	"hjm;"
#define PL_DMS_DEFAULT_SCANFILTER_ALL		"*;"


#define PL_DMS_TYPE_UNKONWN			0

#define PL_DMS_FORMAT_UNKONWN			0

/* Video */
#define PL_DMS_FORMAT_VIDEO_WMV 		0x110
#define PL_DMS_FORMAT_VIDEO_MPG 		0x120

#define PL_DMS_FORMAT_VIDEO_MPEG1 		PL_DMS_FORMAT_VIDEO_MPG
#define PL_DMS_FORMAT_VIDEO_MPEG_TS188 0x121
#define PL_DMS_FORMAT_VIDEO_MPEG_TS192 0x122
#define PL_DMS_FORMAT_VIDEO_DVD 		0x123
#define PL_DMS_FORMAT_VIDEO_VCD 		0x124
#define PL_DMS_FORMAT_VIDEO_MPEG_PS 	0x125

#define PL_DMS_FORMAT_VIDEO_AVI 		0x130
#define PL_DMS_FORMAT_VIDEO_MP4 		0x140
#define PL_DMS_FORMAT_VIDEO_MKV 		0x150
#define PL_DMS_FORMAT_VIDEO_K3G 		0x160

/* Audio */
#define PL_DMS_FORMAT_AUDIO_WMA 		0x210
#define PL_DMS_FORMAT_AUDIO_MP3 		0x220
#define PL_DMS_FORMAT_AUDIO_WAV 		0x240
#define PL_DMS_FORMAT_AUDIO_AC3 		0x250
#define PL_DMS_FORMAT_AUDIO_AAC 		0x260        /* MPEG4 */
#define PL_DMS_FORMAT_AUDIO_OGG 		0x270
#define PL_DMS_FORMAT_AUDIO_FLAC 		0x280
#define PL_DMS_FORMAT_AUDIO_AMR 		0x2B0
#define PL_DMS_FORMAT_AUDIO_ATRAC3 	0x2C0
#define PL_DMS_FORMAT_AUDIO_LPCM 		0x2D0
#define PL_DMS_FORMAT_AUDIO_MP4 		0x2E0

/* Image */
#define PL_DMS_FORMAT_IMAGE_JPG 		0x310
#define PL_DMS_FORMAT_IMAGE_BMP 		0x320
#define PL_DMS_FORMAT_IMAGE_PNG 		0x330
#define PL_DMS_FORMAT_IMAGE_GIF 		0x330

/* Document */
#define PL_DMS_FORMAT_TEXT_TXT 		0x410
#define PL_DMS_FORMAT_TEXT_DOC 		0x420

/* System */
#define PL_DMS_FORMAT_SYSTEM_DTCP		0x510
#define PL_DMS_FORMAT_SYSTEM_SMI		0x520
#define PL_DMS_FORMAT_SYSTEM_IFO		0x540


#define PL_DMS_MASK_MEDIATYPE( a )  	( (a) & 0xf00 )

/* GetCodecInfo ????Option ???*/
#define PL_DMS_DETECT_CMD_DLNA			(0xa1)
#define PL_DMS_DETECT_CMD_ACCESS		(0xa2)
#define PL_DMS_DETECT_CMD_MIME			( PL_DMS_DETECT_CMD_DLNA )

// for live channel
#define PL_DMS_DEFAULT_CHANNEL_CONTAINER_TITLE		"Channel"
#define PL_DMS_DEFAULT_CHANNEL_PREFIX      			"tuner/HCHANNEL:"
// for pvr content
#define PL_DMS_DEFAULT_CONTENT_CONTAINER_TITLE		"Recordings"
#define PL_DMS_DEFAULT_CONTENT_PREFIX				"pvr/HCONTENT:"

#define PL_DMS_TRANSBUFFER_SIZE_DEFAULT     ( 0 )
#define PL_DMS_UPLOAD_PATH                  "/mnt/hd2/Upload"
#define PL_DMS_UPLOAD_TEMP_PATH            NULL    // temp upload folder is not used. http://svn:3000/issues/111269
#define PL_DMS_DEFAULT_THUMBNAIL_PATH       NULL

#define PL_DMS_DEFAULT_DB_PATH              "/var/lib/humaxtv_user/dms_cds.db" // 2016/02/24 Change RULE: main db should be on flash of STB.

#define PL_DMS_THIRDPARTY_PVR_FEATURE

#if !defined(CONFIG_PROD_HGS1000S)
#define PL_DMS_LIVETV_APP_SUPPORT
#endif

typedef enum
{
	PL_DMS_EPG_TARGET_TUNER_ONLY = 0,
	PL_DMS_EPG_TARGET_EPG_ONLY = 1,
	PL_DMS_EPG_TARGET_BOTH = 2
}PL_DMS_EPG_TARGET_e;

// SectionInfo's TitleCmd
typedef enum
{
	PL_DMS_SECTION_MP3_TITLE = 1,
	PL_DMS_SECTION_MP3_ARTIST,
	PL_DMS_SECTION_MP3_ALBUM,
	PL_DMS_SECTION_MP3_GENRE,
	PL_DMS_SECTION_MP3_COMPOSER,
	PL_DMS_SECTION_MP3_PUBLISHER,
	PL_DMS_SECTION_MP3_TRACK,
	PL_DMS_SECTION_MP3_YEAR,
	PL_DMS_SECTION_MP3_TYPE,
	PL_DMS_SECTION_MP3_THUMBNAIL = 100,

} PL_DMS_SECTION_MP3_e;

typedef enum
{

	PL_DMS_SECTION_WMA_TITLE = 1,
	PL_DMS_SECTION_WMA_AUTHOR,
	PL_DMS_SECTION_WMA_COPYRIGHT,
	PL_DMS_SECTION_WMA_DESC,
	PL_DMS_SECTION_WMA_RATING,

	PL_DMS_SECTION_WMA_YEAR,
	PL_DMS_SECTION_WMA_GENRE,
	PL_DMS_SECTION_WMA_COMPOSER,
	PL_DMS_SECTION_WMA_PUBLISHER,
	PL_DMS_SECTION_WMA_ALBUMTITLE,
	PL_DMS_SECTION_WMA_ALBUMARTIST,
	PL_DMS_SECTION_WMA_PROVIDERRATING,

	PL_DMS_SECTION_WMA_THUMBNAIL = PL_DMS_SECTION_MP3_THUMBNAIL,
} PL_DMS_SECTION_WMA_e;

typedef enum
{
	PL_DMS_SECTION_IMG_EXIF_CAMERA_MAKER = 1,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_MODEL,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_DATE,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_RESOLUTION,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_ORIENTATION,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_FLASH,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_FOCAL_LENGTH,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_FOCUS_DIST,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_APERTURE,
	PL_DMS_SECTION_IMG_EXIF_CAMERA_ISO,

	PL_DMS_SECTION_IMG_JPG,
	PL_DMS_SECTION_IMG_BMP,
	PL_DMS_SECTION_IMG_GIF,
	PL_DMS_SECTION_IMG_PNG,

	PL_DMS_SECTION_IMG_EXIF_THUMBNAIL = PL_DMS_SECTION_MP3_THUMBNAIL,

} PL_DMS_SECTION_IMG_e;

typedef enum
{
	PL_DMS_FILEMGR_ADD,
	PL_DMS_FILEMGR_REMOVE,

} PL_DMS_FILEMGR_TYPE_e;


#define PL_DMS_M3U8RES_USE                          	0x04
#define PL_DMS_M3U8RES_POS_BASIC					 	0x00
#define PL_DMS_M3U8RES_POS_REMOVE_SOURCE			 	0x01
#define PL_DMS_M3U8RES_POS_ADDFRONT					0x02
#define PL_DMS_M3U8RES_POS_FILTER					 	0x03

typedef enum
{
	ePL_DMS_StreamingConfig_None        = 0x00000000,
	ePL_DMS_StreamingConfig_Tuner_NTLS  = 0x00000001,
	ePL_DMS_StreamingConfig_Tuner_HLS   = 0x00000002,
	ePL_DMS_StreamingConfig_Record_NTLS = 0x00000004,
	ePL_DMS_StreamingConfig_Record_HLS  = 0x00000008, /* PL_DMS_M3U8RES_USE */
	ePL_DMS_StreamingConfig_Max         = 0x10000000
} PL_DMS_StreamingConfig_e;

typedef struct
{
	HCHAR szFriendlyName[ PL_DMS_NAME_64 ];

	/* Server Config */
	HCHAR szUDN[ PL_DMS_UDN_MAX ] ;
	HCHAR szModelName[ PL_DMS_NAME_32 ];
	HCHAR szModelDescription[ PL_DMS_NAME_128 ];
	HCHAR szModelURL[ PL_DMS_URL_MAX ];
	HCHAR szModelNumber[ PL_DMS_NAME_32 ];

	HUINT16 upnp_port;		// UPnP ????????????????????????? default  0???????
	HUINT16 webserver_port;		// webserver??port
	HUINT16 upnp_dtcp_port;		// dtcp??port
	HUINT16 uploadserver_port;	// upload port

	/* Model Config */
	HBOOL bSupportRecordItem;
} PL_DMS_Config_t;

typedef struct
{
	HCHAR		cUri[1024];
	HUINT32		unProfileId;
	HBOOL		bTranscoding;
	HCHAR		cUseragent[PL_DMS_NAME_64];
	HCHAR		cIP[PL_DMS_NAME_32];
	HCHAR		cHumaxKey[PL_DMS_NAME_128];
} PL_DMS_LiveStreaming_t;

typedef struct
{
	HCHAR 	*pMimeType;
	HINT32 	nWidth;
	HINT32 	nHeight;
	HINT32 	nDepth;
	HCHAR 	*pUrl;
} PL_DMS_ICON_t;

/* PL DMS status: sync with DMS_Status_e */
typedef enum
{
	ePL_DMS_Status_Stopped,
	ePL_DMS_Status_Stopping,
	ePL_DMS_Status_Starting,
	ePL_DMS_Status_Started,
} PL_DMS_Status_e;

/* PL_DMS??Return???????*/
typedef enum {
	ePL_DMS_Result_Ok=0,    /* ERR_OK */
	ePL_DMS_Result_Error=1, /* ERR_FAIL */

	ePL_DMS_Result_Max,
} PL_DMS_Result_e;
/* 8859, 6937, 300468 차이가 ???? ??????구분?????*/
typedef int			STR_LEN;
typedef	char			*CH_POINTER;
typedef	HUINT8	CH_8859;
typedef	HUINT8	CH_6937;
typedef	HUINT8		CH_300468;
typedef	HUINT8		CH_ASCII;
typedef	HUINT8		CH_1BYTE;
typedef	HUINT8	CH_UTF8;
typedef	HUINT16	CH_UCS2;
typedef	unsigned long		CH_UCS4;		/**< 4byte 보장?? unsigned long???가?? unsigned int??system dependent ???? C99 */



/* ScanFilter ???Add????Callback */
//typedef HLONG ( *PL_DMS_CbAddScanFilter )( void *hDms, void *hDB, HCHAR *pLocation, HCHAR *pFileName, HCHAR *pScanPath );
typedef HLONG 	( *PL_DMS_CbAddScanFilter )		( void *dmsHandle, void *db_h, HCHAR *location, HCHAR *filename, HCHAR *scanningPath );

/* ScanFilter ???Remove????Callback */
typedef HLONG 	( *PL_DMS_CbRemoveScanFilter )	( void *hDms, void *hDB, HCHAR *pLocation );


/* DMS????????????????Callback */
/* MXDLNA_DMSSTATUS_CALLBACK ????????*/
typedef void 	( *PL_DLNA_DMSSTATUS_CALLBACK )	( HINT32 nStatus, void* pData );


/* DMP????????????Humax ??????????처리??????경우 File Operation??콜백?????????*/
typedef HLONG 	( *PL_DMS_Cb_user_is_userfile )( const HCHAR *pathname, HLONG *isUserFile );
typedef HULONG	( *PL_DMS_Cb_user_open ) 		( const HCHAR *pathname, HULONG flags, HULONG *filedes );
typedef void 	( *PL_DMS_Cb_user_close)		( HULONG filedes );
typedef HLONG	( *PL_DMS_Cb_user_read ) 		( HULONG filedes, HUINT8 *buf, HULONG size, HULONG *sizep );
typedef HINT64 	( *PL_DMS_Cb_user_seek ) 		( HULONG filedes, HINT64 offset, HULONG whence );
typedef HLONG 	( *PL_DMS_Cb_user_tell ) 		( HULONG filedes, HUINT64 *poffset );
typedef HLONG 	( *PL_DMS_Cb_user_get_filesize )(HULONG filedes, HINT64 *pulSize );
typedef HLONG 	( *PL_DMS_Cb_user_is_eof ) 		( HULONG filedes );
typedef HCHAR* 	( *PL_DMS_Cb_user_processcmd )	( HCHAR *pUrlNParam );

typedef HINT32	(*PL_DMS_StatusEventCallback) (HINT32 evt, HINT32 p1, HINT32 p2, HUINT64 p3, void *p4);

/* DMS EVENT Type */
typedef enum
{
	PL_DMS_StatusEvent_UNKNOWN	= 0,
	PL_DMS_StatusEvent_Started,
	PL_DMS_StatusEvent_Stopped,
	PL_DMS_StatusEvent_Open,
	PL_DMS_StatusEvent_Read,
	PL_DMS_StatusEvent_Seek,
	PL_DMS_StatusEvent_Tell,
	PL_DMS_StatusEvent_Close,
	PL_DMS_StatusEvent_SetupLS,
	PL_DMS_StatusEvent_StartLS,
	PL_DMS_StatusEvent_StopLS,
	PL_DMS_StatusEvent_ReleaseLS,
	PL_DMS_StatusEvent_GetChannelInfo,
	PL_DMS_StatusEvent_StackStreamingSend,     /* MXDLNADMS_CALLSTATUS_STREAMMING_SEND */
	PL_DMS_StatusEvent_StackScanOneCycleStart, /* MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_START */
	PL_DMS_StatusEvent_StackScanOneCycleEnd,   /* MXDLNADMS_CALLSTATUS_SCAN_ETERNAL_ONECYCLE_END */
	PL_DMS_StatusEvent_Max
} PL_DMS_STATUS_EVENT_e;

/* Humax File Operation 콜백 ??*/
typedef struct
{
	PL_DMS_Cb_user_is_userfile		cbIsUserFile;
	PL_DMS_Cb_user_open			cbUserOpen;
	PL_DMS_Cb_user_read			cbUserRead;
	PL_DMS_Cb_user_close			cbUserClose;
	PL_DMS_Cb_user_seek			cbUserSeek;
	PL_DMS_Cb_user_tell			cbUserTell;
	PL_DMS_Cb_user_get_filesize	cbUserGetFileSize;
	PL_DMS_Cb_user_is_eof			cbUserIsEOF;
	PL_DMS_Cb_user_processcmd		cbUserProcCMD;
} PL_DMS_UserCallback_t;

/* MXDLNA_DMS_VIRTUAL_MULTIRESOURCE_ITEM ????????*/
typedef struct
{
	const HCHAR * PostFix;				// Lib???????????기본 URL ????붙이??????? ???http://ipaddr:port/tuner/1235.tsPostfix
	const HCHAR * DlnaProtocolInfo;		// ex) "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_OP=10;DLNA.ORG_CI=1;DLNA.ORG_FLAGS=01500000000000000000000000000000"
	HINT64 Size_byte;					// Size					, ??????????0
	HINT32 Duration;						// Duration=3600		, ??????????0
	HINT32 Bitrate;						// bitrate=250000		, ??????????0
	HINT32 resX;							// 720					, ??????????0
	HINT32 resY;							// 480					, ??????????0
} PL_DMS_VIRTUAL_MULTIRESOURCE_ITEM_t;

/* MXDLNA_PROTOCOLTYPE ????????*/
typedef enum
{
	PL_DMS_ProtocolType_UNKNOWN	= 0,
	PL_DMS_ProtocolType_ANY		= 3,
	PL_DMS_ProtocolType_HTTP		= 1,
	PL_DMS_ProtocolType_RTP		= 2
} PL_DMS_PROTOCOLTYPE_e;

/* MXDLNA_ADDRES_TYPE????????*/
typedef enum
{
	ePL_DMS_ResType_All = 0,	// 모든 리소???? ??집어 ?????- ????Physical ?????존재???? ????경우??????? ???????base64???????????값이 보인?
	ePL_DMS_ResType_OnlyPhysical = 1,
} PL_DMS_ADDRES_TYPE_e;

/* MXDLNA_DMS_RESOURCE ????????*/
typedef struct _PL_DMS_RESOURCE_t
{
	HCHAR *contentURI;
	HCHAR *importURI;

	HCHAR *ifoFileUri;			// added : 090706
	HCHAR *importIfoFileUri;	// added : 090706

	HCHAR *mimeType;
	HCHAR *protocol;			// PN
	HINT64 size;
	HLONG bitrate;
	HLONG bitPerSample;
	HLONG colorDepth;
	HINT32 duration;

	HLONG nrAudioChannels;
	HCHAR *protection;			// drmtype
	HLONG resolutionX;
	HLONG resolutionY;
	HLONG sampleFrequency;

	HCHAR *userData;			// usrdata="sample:http://123123" >
	struct _PL_DMS_RESOURCE_t *pNext;

	HINT32 dtcp_emi;
	HLONG dtcp_pcplength;
	PL_DMS_ADDRES_TYPE_e resAddType;

} PL_DMS_RESOURCE_t;


#define PL_DMS_M3U8RES_USE                          	0x04
#define PL_DMS_M3U8RES_POS_BASIC					 	0x00
#define PL_DMS_M3U8RES_POS_REMOVE_SOURCE			 	0x01
#define PL_DMS_M3U8RES_POS_ADDFRONT					0x02
#define PL_DMS_M3U8RES_POS_FILTER					 	0x03


/* MXFILE_CANCEL_OP ????????*/
typedef struct
{
	HUINT8 cancel_code;
} PL_DMS_MXFILE_CANCEL_OP_t;

/* MXFILE_DESC ????????*/
typedef struct
{
	HCHAR *url;   // read only 로만 ????
	HLONG access_type;
	HLONG linear_access;
	PL_DMS_MXFILE_CANCEL_OP_t *cancel_op;
	HLONG option;
	HUINT8 user_data[32];

	HINT32 nTransferMode;

} PL_DMS_MXFILE_DESC_t;


/* MXFILE_HANDLE ????????*/
typedef struct
{
	void *apis;
	PL_DMS_MXFILE_DESC_t desc;
	HLONG file_type;

	PL_DMS_MXFILE_CANCEL_OP_t static_cancel_op;
	PL_DMS_MXFILE_CANCEL_OP_t *cancel_op;
	HLONG error;
	HLONG eof;
	HLONG linear_access;
	HINT64 filesize;
	void *extra_data;

	HINT32 flag_timeseek;
} *PL_DMS_MXFILE_HANDLE_t;

/* MXMEDIA_INFO ????????*/
typedef struct
{
	HLONG media_format;		// mandatory
	HLONG media_info;		// required
	HLONG duration;		// optional
	HUINT64 protocolInfo;	// optional
} PL_DMS_MXMEDIA_INFO_t;

typedef struct
{
	HINT32 SupportsTimeBasedSeek;
	HINT32 SupportsByteBasedSeek;

	HCHAR *Profile;
	HCHAR *MimeType;

	HCHAR **SupportedPlaySpeeds;
	HINT32 SupportedPlaySpeedsLength;

	HINT32 IsConvertedContent;
	PL_DMS_PROTOCOLTYPE_e Protocol;
	HINT32 IsPlaySingleUri;

	HINT32 SenderPaced;
	HINT32 LimitedOperations_TimeBasedSeek;
	HINT32 LimitedOperations_ByteBasedSeek;

	HINT32 DLNAPlayContainer;
	HINT32 S0_Increasing;
	HINT32 SN_Increasing;

	HINT32 RTSP_Pause;
	HINT32 HTTP_Stalling;

	HINT32 TM_S;
	HINT32 TM_I;
	HINT32 TM_B;

	HINT32 MaxSpeed_Major;
	HINT32 MaxSpeed_Minor;

	HINT32 LinkProtection;
	HINT32 ClearText;

	HINT32 DLNA_Major_Version;
	HINT32 DLNA_Minor_Version;

	void* reserved;
	void* reserved2;

} PL_DMS_PROTOCOLINFO_t;

typedef struct
{
	PL_DMS_ICON_t				*pIconInfo;
	HINT32						nNumberOfIcon;

	HCHAR						szDbPath[ PL_DMS_NAME_MAX ];
	PL_DMS_CbAddScanFilter		cbAddScanFilter;
	PL_DMS_CbRemoveScanFilter	cbRemoveScanFilter;
	PL_DMS_UserCallback_t		stUserCallbackSet;
	PL_DMS_VIRTUAL_MULTIRESOURCE_ITEM_t	*pstMultiRes;
} PL_DMS_Option_t;

typedef enum {
	ERR_ENCODING_OK=0,
	ERR_ENCODING_FAIL,
	ERR_NOT_SUPPORTED_CODEPAGE,
	ERR_OUTOF_CODEPAGE,
	ERR_ILLEGAL_SEQ,
	ERR_TOOSHORT,
	ERR_CODEPAGE_CH,
	ERR_CONTROL_CH,
	ERR_WHITE_CH,
	ERR_USERDEFINE_CH,
	ERR_NULL_CH,
	ERR_NEWLINE_CH,
	ERR_BUFFER_TOOSMALL,
	ERR_CONV_MAX
} ConvEncoding_Err_t;


/* Default System Folder Name */
#define PL_DMS_DEFAULT_SF_ROOTTITLE			"My Contents"
#define PL_DMS_DEFAULT_SF_UNKNOWN			"Unknown"
#define PL_DMS_DEFAULT_SF_UNKNOWNDAY		"Unknown Day"


#define _____________PVR___________


/*********************** NEW Struct & Functions ***************/
typedef enum
{
	ePL_DMS_ScanDepth_Recursive		= 0,
	ePL_DMS_ScanDepth_OneDepth,

	ePL_DMS_ScanDepth_Unknown,
} PL_DMS_ScanDepth_e;

typedef enum
{
	ePL_DMS_ScanPath_Normal		= 0,
	ePL_DMS_ScanPath_External_Root,
	ePL_DMS_ScanPath_External_Sub
} PL_DMS_ScanPath_e;

typedef struct
{
	PL_DMS_ScanDepth_e eDepth;
	HCHAR	*pszFilter;
	HCHAR 	*pszUserData;
	PL_DMS_ScanPath_e ePath;

} PL_DMS_ScanOpt_t;

typedef struct
{
	/*  MXDLNA_DMS_HANDLE 	*/ 	void 	*hDms;
	/*  DMS_DB_HANDLE	 	*/	void 	*hDmsDB;
	/*  MXDLNA_FSS_HANDLE 	*/	void 	*hFSS;
} PL_DMS_MXDLNA_HANDLE_t;

/* APKS_DLNA_DMS_LiveStreamInfo_t -> HOMxDMS_LiveStreamInfo_t -> PL_DMS_LiveStreamInfo_t */
typedef	struct	{
	HCHAR	szURI[ PL_DMS_LIVE_STREAM_URL_MAX ];
	HCHAR	szMIME[ PL_DMS_LIVE_STREAM_MIME_MAX ];
} PL_DMS_LiveStreamInfo_t;

#define ________________Public_Interfaces__________________________
/**
 * Initializing for Digital Media Server (DMS).
 *
 * @param[in] friendly_name      UPnP device friendly name.
 * @param[in] manufacturer       UPnP device manufacturer.
 * @return                       Result Ok or Error.
 */
PL_DMS_Result_e		PL_DMS_Init( const PL_DMS_Config_t *pDmsConfig);
PL_DMS_Result_e		PL_DMS_DeInit( void );

/**
 * Starting for Digital Media Server (DMS).
 *
 * @param[in] friendly_name      UPnP device friendly name.
 * @param[in] manufacturer       UPnP device manufacturer.
 * @return                       Result Ok or Error.
 */
PL_DMS_Result_e		PL_DMS_Start( void );


/**
 * Stop for Digital Media Server (DMS).
 *
 * @param[in] friendly_name      UPnP device friendly name.
 * @param[in] manufacturer       UPnP device manufacturer.
 * @return                       Result Ok or Error.
 */
PL_DMS_Result_e		PL_DMS_Stop( void );

/**
 * FriendlyName ????
 *
 * @param[in] pszDmsName      UPnP device friendly name.
 * @return                    Result Ok or Error.
 */
HERROR	PL_DMS_SetDmsName( const HCHAR *pszDmsName );

/**
 * FriendlyName Get.
 *
 * @param[out] 	pszDmsName      UPnP device friendly name.
 * @param[in]	nBufSize		buf size of pszDmsName.
 * @return                    Result Ok or Error.
 */
HERROR	PL_DMS_GetDmsName( HCHAR *pszDmsName, HINT32 nBufSize );

/**
 * Added DMS scan directory
 *
 * @param[in] 	pszScanPath     directory path for add.
 * @param[in]		pstScanOpt		scan option - depth, filter.
 * @return                    	Result Ok or Error.
 */
HERROR	PL_DMS_AddScanDirectory( HCHAR *pScanPath, PL_DMS_ScanOpt_t *pstScanOpt );

HERROR	PL_DMS_GetScanPathList( HINT32 *pnCount, HxList_t** ppScanPathList );

HERROR	PL_DMS_GetScanPath(void *pData, HCHAR **ppszScanPath);

HERROR	PL_DMS_GetScanOption(void *pData, HINT32 *pScanOption);

HERROR	PL_DMS_FreeScanPathList( HxList_t *pScanPathList);

/**
 * Remove DMS scan directory
 *
 * @param[in] 	pszScanPath     path for add.
 * @param[in]		pstScanOpt		scan option - depth, filter.
 * @return            	Result Ok or Error.
 */
HERROR		PL_DMS_RemoveScanDirectory( HCHAR *pScanPath );

/**
 * Remove DMS scan file
 *
 * @param[in] 	pszScanPath     file path for remove()
 * @return                    	Result Ok or Error.
 */
HERROR		PL_DMS_RemoveScanFile( HCHAR *pScanFile );

/**
 * Start DMS FSS scan()
 *
 * @param[in] 	nInterval     	term for scanning .
 * @param[in]		nInterval_item	term for scan each item.
 * @return                    	Result Ok or Error.
 */
HERROR		PL_DMS_StartScan( HLONG nInterval, HLONG nInterval_item );

/**
 * Stop previous DMS FSS scan()
 *
 * @param[in] 	nInterval     	term for scanning .
 * @param[in]	nInterval_item	term for scan each item.
 * @return                    	Result Ok or Error.
 */
HERROR		PL_DMS_StopScan( void );

/**
 * Do file based scan() for specific path
 *
 * @param[in] 	pScanPath     	target path.
 * @param[in]	pstScanOpt		scan option -filter.
 * @return                    	Result Ok or Error.
 */
HERROR		PL_DMS_ScanDirect( HCHAR *pScanPath, PL_DMS_FILEMGR_TYPE_e eMgrType );

/**
 * Do oneshot scan() for all FSS directories
 *
 * @return                    	Result Ok or Error.
 */
HERROR		PL_DMS_RescanFSS( void ); /* oneshot rescan */

/**
 * DMS ?????????????
 *
 * @param[in] 	nInterval     	term for scanning .
 * @return                    	Result Ok or Error.
 */
PL_DMS_Result_e		PL_DMS_GetDmsInfo( void  *pvData );


/* *
 * @brief
 *
 * @param pHandle
 *
 * @return
 **/
HERROR				PL_DMS_Get_MxDlna_Res(PL_DMS_MXDLNA_HANDLE_t *pHandle);

/**
 * @brief 			Uri?κ??? Local Path?? ??=.
 *
 * @param[in]			Uri
 *
 * @return
 **/
HCHAR				*PL_DMS_GetLocalPathfromUri(HCHAR *pUri);

/* *
 * @brief 			Local Path?κ??? Uri?? ??=.
 *
 * @param[in]			Local Path
 *
 * @return
 **/
HCHAR				*PL_DMS_GetUrifromLocalPath(HCHAR *pPath);

/**
 * Local Path?? ???? Function
 *
 * @param[in] 	pPath    	 		term for scanning .
 * @param[out] 	ppOutUri     		term for scanning .
 * @param[out] 	ppOutMeta     	term for scanning .
 * @return           	Result Ok or Error.
 */
PL_DMS_Result_e	PL_DMS_GetFlingInfoFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta );

PL_DMS_Result_e	PL_DMS_GetFlingInfoOfPVRFromPath( HCHAR *pPath, HCHAR **ppOutUri, HCHAR **ppOutMeta );

PL_DMS_Result_e	PL_DMS_GetFlingInfoFromSvcUid( HINT32 hSvc, HCHAR **ppOutUri, HCHAR **ppOutMeta );

PL_DMS_Result_e	PL_DMS_GetLiveStreamInfo( HINT32 hSvc, HUINT32 *pnCount, PL_DMS_LiveStreamInfo_t **pList );

HERROR	PL_DMS_RegisterStatusEventCallback(PL_DMS_StatusEventCallback cb);

HERROR	PL_DMS_UnregisterStatusEventCallback(void);

HBOOL	PL_DMS_IsStreaming(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __PL_DMS_H__

/* end od file */

