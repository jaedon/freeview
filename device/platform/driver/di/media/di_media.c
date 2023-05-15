#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <fcntl.h>
#include <unistd.h>

#include "nexus_platform.h"
#include "nexus_platform_features.h"
#include "nexus_display.h"

//audio stream
#include "nexus_audio_playback.h"

//divx & mp3
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#include "nexus_playback.h"
#include "nexus_file.h"

#include "nexus_security.h"
#if defined(CONFIG_DSC)
#include "nexus_keyladder.h"
#endif

#include "vkernel.h"
#include "taskdef.h"
#include "di_media.h"
#include "di_video.h"
#include "di_audio.h"

/* DI_IPTUNER_GetUnderflowState(), DI_IPTUNER_SetUnderflowState()ÀÇ È£Ãâ·Î ÀÎÇØ di_iptuner.c¿Í Cross includeÇÏ°í ÀÖ°í, ¼öÁ¤ÀÌ ÇÊ¿äÇÕ´Ï´Ù. */
#if defined(CONFIG_IPTUNER)
#include "di_iptuner.h"
#endif
#include "drv_media_10.h"
#include "drv_audio.h"
#include "drv_video.h"
#include "drv_pvr.h"

#define B_HAS_AVI 1
#define B_HAS_ASF 1
#include "bfifo.h"
#include "bfile_stdio.h"
#include "biobits.h"
#include "bioatom.h"
#include "bpool.h"
#include "barena.h"
#include "bpool.h"
#include "bmedia_filter.h"
#include "bmedia_player.h"
#include "bfile_stdio.h"
#include "bfile_buffer.h"
#include "bfile_async.h"
#include "bmedia_probe.h"
#include "bmp4_probe.h"
#include "bmkv_probe.h"
#include "bavi_probe.h"
#include "bmpeg2ts_parser.h"
#include "bmpeg2pes_parser.h"

#if defined(CONFIG_MEDIA_PROBE_PMT)
#include "bmpeg2ts_psi_probe.h"
#endif

#include "di_uart.h"

#if defined (USE_DRM)
	#include "blst_slist.h"         /* due to basf_parser.h*/
	#include "drmdecryptor.h"
	#include "drmcore.h"
	#include "drmhttp.h"
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
	#include "drmcommon.h"
	#include "drmtypes.h"
	#include "drmresults.h"
	#include "drmcipher.h"
	#include "drmhds.h"
	#include "drmcontextsizes.h"
	#include "drmlicense.h"
	#include "drmblackbox.h"
	#include "drmlicacq.h"
	#include "drmoutputleveltypes.h"
	#include "drmenv.h"
	#include "drmmanager.h"
#endif
#endif

#if defined (CONFIG_PLAYREADY)
	#include "prdy_core.h"
	#include "prdy_http.h"
#if defined (CONFIG_PLAYREADY_20)  // for using secure clock
	#undef CERT  // CERT is also defined in below microsoft drm headers (oemcryptctx.h)
	#include <drmtypes.h>
	#include <drmmanager.h>
	#include <drmxmlparser.h>
	#include <drmutf.h>
	#include <drmsecureclockstatus.h>
	#include <drmsecureclockconstants.h>
	#include "oem_brcm.h"
	#include "bmp4_util.h"
	#include "util_url.h"
#else
	#include "drmtypes.h"
	#include "drmmanager.h"
	#include "nexus_dma.h"
	#include "bmp4_util.h"
	#include "oem_brcm.h"
#endif
#endif

#if defined (CONFIG_DSC)
#include "di_dsc.h"
#endif

#include "di_uart.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/*#define CONFIG_VFIO*/

/* OLD Media´Â ÀÌÀü ST Project·Î ÀÎÇØ Á¸ÀçÇÏ´Â ºÎºÐ. ÇâÈÄ ÀÌÀü ST Project(Duglas)°¡ ¿µÇâÀ» ÁÖÁö ¾ÊÀ¸¸é »èÁ¦ÇÏ´Â °ÍÀÌ ¿ÇÀ½. */
#define SUPPORT_OLD_MEDIA

#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
#include "drmcrt.h"
#define _UNICODEtoANSI_		(1)
#endif

//#define DI_MEDIA_DEBUG
//#define DI_MEDIA_CRITICAL_DEBUG

#ifdef DI_MEDIA_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif

#ifdef DI_MEDIA_DEBUG
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter
#define PrintExit
#endif

#ifdef DI_MEDIA_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAMETER(x)	(void)x


#define HUMAX_COOKIE_LINE_BUFFER_SIZE		(10240)
#if defined(CONFIG_HDD_SMART) && !defined(USE_ESATA)
#define HUMAX_COOKIE_FILE_FULL_PATH	"/mnt/hd3/cookie.txt"
#else
#define HUMAX_COOKIE_FILE_FULL_PATH	"/var/lib/humaxtv/cookie.txt"
#endif
#define HUMAX_COOKIE_SEP "\t"
#define MAXDOME_MAN_STRING		"MAN\t"
#define MAXDOME_MANUSER_STRING 	"MANUser\t"

#define MEDIA_MPEG2TS_188PKT_LEN		(188)
#define MEDIA_MPEG2TS_192PKT_LEN		(192)
#define MEDIA_MPEG2TS_PKT_SYNC			(0x47)

#define DI_MEDIA_MSG_SIZE			MSGQ_SIZE_NORMAL
#define DI_MEDIA_INFINITIVE_LOOP	0

/* junsy comment 2009.05.07 :
	ÇöÀç PositionÀ» ¾ò±â À§ÇØ ±âÁ¸ÀÇ °æ¿ì bmedia ÇÔ¼ö¸¦ »ç¿ëÇÏ¿´À¸³ª ´ÙÀ½ ¹®Á¦ Á¸Àç
	1) ASFÀÇ °æ¿ì ÀÏ¹Ý Àç½Ì½Ã bmedia_player_tell ÇÔ¼ö¿¡ ÀÇÇØ position Á¤º¸°¡ ³Ñ¾î¿ÀÁö ¾ÊÀ½
	   AVIÀÇ °æ¿ì´Â Á¤»ó µ¿ÀÛ, ASFÀÇ °æ¿ì´Â Trick¸¸ Á¤»ó µ¿ÀÛ
	2) ¶ÇÇÑ ½ÇÁ¦ È­¸é¿¡ ³ª¿À´Â Video¿Í bmedia ¿ÍÀÇ GAPÀÌ Á¸ÀçÇÏ¹Ç·Î ½ÇÁ¦ Position°ú ¸¹ÀÌ ´Ù¸£°Ô µÊ
*/
/** Define MFP Stream Url Length */
#ifdef CONFIG_DI_MAX_URL_LENGTH
#define DI_MEDIA_STREAM_URL_LEN CONFIG_DI_MAX_URL_LENGTH
#else
#define	DI_MEDIA_STREAM_URL_LEN			(2048)
#endif
/** Define MFP Feed Block Buffer Size */
#define DI_MFP_FEED_BLOCK_BUF_SIZE			(4096*15)
/** Define MFP Feed Total(Block*Segment number)Buffer Size */
#define DI_MFP_FEED_TOTAL_BUF_SIZE			(4096*15*32)
#define	B_PUMP_DEMUX_FACTORY_ATOMS		(128)
/** Define Video Main Decoder */
#define	DI_VIDEO_MAIN_DECODER			(0)

#define DI_MEDIA_ENCRYPTION_KEY_LEN		(32)

#define DI_MEDIA_TSTYPE_SEARCH_LEN		(1024*3)
#define DI_MEDIA_TSINFO_LEN		(1024*2)

#if defined(HDR_FOX_T2) || defined(HD_FOX_T2)
#define SUPPORT_USE_STC_AUTOMODE_FIRSTAVAILABLE 1
#else
#define SUPPORT_USE_STC_AUTOMODE_FIRSTAVAILABLE 0
#endif

#define MEDIA_PLAYPUMP_MAX_READ (32*1024)

/*
Summary:
Returns bits 'e'..'b' from word 'w',

Example:
	B_GET_BITS(0xDE,7,4)==0x0D
	B_GET_BITS(0xDE,3,0)=0x0E
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

#ifdef USE_LPCM
#define SUPPORT_AUDIOCODEC_LPCM 1
#else
#define SUPPORT_AUDIOCODEC_LPCM 0
#endif

#define DI_MEDIA_DLNA_FILESEEK_SET			0
#define DI_MEDIA_DLNA_FILESEEK_CUR			1
#define DI_MEDIA_DLNA_FILESEEK_END			2

#define DI_MEDIA_DLNA_FILEACCESS_NONE 0
#define DI_MEDIA_DLNA_FILEACCESS_FILE 1
#define DI_MEDIA_DLNA_FILEACCESS_HTTP 2
#define DI_MEDIA_DLNA_FILEACCESS_WMSP 3
#define DI_MEDIA_DLNA_FILEACCESS_RTSP 4
#define DI_MEDIA_DLNA_FILEACCESS_USER 10

#if defined (CONFIG_PLAYREADY)   /* contents */
typedef struct DI_license_challenge {
    HUINT32 len;                       /* bytes total */
    HINT8* data;                         /* contents */
    HINT8* url;                          /* url to send the challenge to */
    HUINT8 non_quiet;                  /* non-quiet license acquisition */
    HUINT32 app_security;              /* application security level */
} DI_license_challenge;

typedef struct DI_license_response {
    HUINT32 len;                       /* bytes total */
    HUINT8* data;                /* contents */
    HUINT8* resp_start;          /* start of response within data */
} DI_license_response;
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
typedef enum
{
	DI_MEDIA_STATE_NO_SETUP,
	DI_MEDIA_STATE_READY,
	DI_MEDIA_STATE_PLAY,
	DI_MEDIA_STATE_PAUSE,
	DI_MEDIA_STATE_STOP,
	DI_MEDIA_STATE_TRICK
} DI_MEDIA_STATE_e;

typedef struct _stMediaInfo_t{
	HUINT8				opened;
	char				szStreamUrl[DI_MEDIA_STREAM_URL_LEN];

	batom_pipe_t		pipe_pes;
	bmedia_stream_t		video;
	bmedia_stream_t		audio;

#ifdef  CONFIG_VFIO
	VFIO_Handle_t		tHandle;
#else
	FILE				*fin;
#endif

	bmedia_filter_t		filter;
	batom_factory_t		factory;
	batom_pipe_t		pipe_media;
	size_t				alloc_cnt;
	size_t				free_cnt;

	batom_t				atom;

	bool				async;
	bmedia_player_pos	seek;
	bmedia_player_pos	seek2;
	bmedia_player_step	direction;
	bmedia_time_scale	time_scale;
	bool				key_frame_only;
	bmedia_player_decoder_mode	decoder_mode;

	bfile_io_read_t			fd;
	bmedia_player_t			player; /* media player is used to play various container formats */
	bmedia_player_stream	stream;
	bfile_buffer_t			buffer;
	bfile_buffer_cfg		buffer_cfg;

	bool					no_player;

	DI_MEDIA_INFO_t	StreamInfo;

	HUINT64		ullPrevPosition;
	HUINT64		ullEosPosition;

} stMediaInfo;

typedef enum
{
	DI_MEDIA_PLAY_FROM_UNKNOWN,
	DI_MEDIA_PLAY_AUDIO_FROM_FILE,
	DI_MEDIA_PLAY_AUDIO_FROM_MEMORY,
	DI_MEDIA_PLAY_VIDEO_FROM_FILE,
	DI_MEDIA_PLAY_VIDEO_FROM_MEMORY,
	DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP
} DI_MEDIA_MSG_SENDER;

typedef enum
{
	DI_MEDIA_PLAYPUMPPLAY_NOT_READY,
	DI_MEDIA_PLAYPUMPPLAY_STOP,
	DI_MEDIA_PLAYPUMPPLAY_PLAY,
	DI_MEDIA_PLAYPUMPPLAY_SETUP,
	DI_MEDIA_PLAYPUMPPLAY_STOPWAITING
} DI_MEDIA_PLAYPUMP_STREAM_STATE;

typedef enum
{
	DI_MEDIA_STREAM_UNKNOWN,
	DI_MEDIA_FILE_STREAM_PLAY,
	DI_MEDIA_FILE_STREAM_STOP,
	DI_MEDIA_FILE_STREAM_PAUSE,
	DI_MEDIA_FILE_STREAM_RESUME,
	DI_MEDIA_FILE_STREAM_MONITOR,

	DI_MEDIA_MEMORY_STREAM_PLAY,
	DI_MEDIA_MEMORY_STREAM_STOP,
	DI_MEDIA_MEMORY_STREAM_PAUSE,
	DI_MEDIA_MEMORY_STREAM_RESUME,
	DI_MEDIA_MEMORY_STREAM_MONITOR,
} DI_MEDIA_STEAM_EVENT;

typedef struct
{
	NEXUS_FilePlayHandle 	file;
	unsigned long long		fileSize;
	unsigned char			*pbuffStartPoint;
	unsigned long			esDataSize;
	unsigned long long		playOffset;
	unsigned long long		remainDataSize;
	unsigned long			loopCount;
	unsigned char			pauseFlag;
	unsigned long			emptyChkCnt;
}DI_MEDIA_STREAM_PLAY_INFO;

typedef struct
{
	unsigned long		ulSender;
	unsigned long		ulStreamHandle;
	unsigned long	   	ulEvent;
	unsigned char		*pucParam;
	unsigned long		ulParam2;
	unsigned long		ulParam3;
	unsigned long		ulParam4;
	unsigned long		ulParam5;
	unsigned long		ulParam6;
}DI_MEDIA_MSG;

typedef enum
{
	DI_MEDIA_AUDIO_FREE=0,
	DI_MEDIA_AUDIO_ALLOCATED,
	DI_MEDIA_AUDIO_PAUSED,
	DI_MEDIA_AUDIO_STOP,
	DI_MEDIA_AUDIO_PLAY
}	DI_MEDIA_AUDIO_STATUS;

typedef struct
{
#if defined (USE_DRM)
	bdrm_t tDrm;
	bdrm_cfg tDrm_cfg;
	bdrm_decryptor_t tDcryptor;
	bdrm_license_state tDrm_license_state;
	bdrm_http_state tDrm_http_state;
	HUINT8 aucEncryptionKey[DI_MEDIA_ENCRYPTION_KEY_LEN];
#endif
	HUINT8						valid;
	HUINT8						*mediaurl;
	DI_MEDIA_STATE_e			mediastate;
	DI_MEDIA_PLAYPUMP			mediaType;
	DI_MEDIA_AUDIO_STATUS		audioStatus;
	NEXUS_AudioPlaybackHandle 	audioPlayback;
	NEXUS_PlaypumpHandle		playpump[DI_MEDIA_PLAYPUMP_MAX];
	NEXUS_PlaybackHandle		playback;
	NEXUS_PidChannelHandle		audioPidChannel;
	NEXUS_PidChannelHandle		audioSubPidChannel;
	NEXUS_PidChannelHandle		audioTrackPidChannels[DI_MEDIA_DIVX_MAX_AUDIO_TRACK];
	NEXUS_PidChannelHandle		videoPidChannel;
	// NEXUS_PidChannelHandle	videoTrackPidChannels[DI_MEDIA_DIVX_MAX_VIDEO_TRACK];
	NEXUS_PidChannelHandle		sectionPidChannel;
	NEXUS_PidChannelHandle      pcrPidChannel; /* pcr_pid */
	NEXUS_FilePlayHandle 		file;
	NEXUS_VideoDecoderHandle	vDecoder;
	NEXUS_AudioDecoderHandle	aDecoder;
	DI_MEDIA_TrickSpeed_e		trickrate;

	HUINT32						unFirstPts;
	HBOOL                       bPtsInstalled;

} DI_MediaInstance;

typedef struct b_test_pid {
        bmpeg2ts_parser_pid ts_pid;
        bmpeg2pes_parser pes;
        unsigned pcr_count;
        unsigned ts_bytes;
        unsigned pes_bytes;
        unsigned pts_count;
        void *mfp_pointer;
} b_test_pid;

#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
static const DRM_WCHAR g_wcMasterFile [] =
{
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('r', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('b', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('m', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('v', '\0'),
    ONE_WCHAR('/', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'), ONE_WCHAR('/', '\0'),
    ONE_WCHAR('c', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('e', '\0'),
    ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('.', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')
};

static const DRM_CONST_STRING   g_dstrMasterFile           = CREATE_DRM_STRING( g_wcMasterFile );

#endif

#ifdef SUPPORT_OLD_MEDIA
static DI_MediaInstance		stMediaInstance;

static unsigned long		s_ulMediaStreamTId ;
static unsigned long		s_ulMediaStreamMId;

static unsigned long		s_ulMediaEventTId ;
static unsigned long		s_ulMediaEventEId;

static DI_MEDIA_STREAM_PLAY_INFO		stMediaStreamInfo;

static MEDIA_DLNA_FileFunction_t		s_tMEDIADLNAFileFn;

/* 1KHz sine wave at 48 KHz */
#define audioToneSamplesize	48

//temp
static int16_t audioToneSamples[48] =
{
0,
4276,
8480,
12539,
16383,
19947,
23169,
25995,
28377,
30272,
31650,
32486,
32767,
32486,
31650,
30272,
28377,
25995,
23169,
19947,
16383,
12539,
8480,
4276,
0,
-4277,
-8481,
-12540,
-16384,
-19948,
-23170,
-25996,
-28378,
-30273,
-31651,
-32487,
-32767,
-32487,
-31651,
-30273,
-28378,
-25996,
-23170,
-19948,
-16384,
-12540,
-8481,
-4277
};
static void MediaAudPbDataCallback(void *pParam1, int param2);
#endif

/* Start static variablee */
/** Static Media Semaphore Id */
static unsigned long			s_ulMediaSemId;
/** Static Play Time Task Id */
static unsigned long 			s_ulMediaPlayTimeTaskId;
static HUINT32 					s_ulMediaId;	/* Media SessionÀÌ ÇÏ³ªÀÌ¹Ç·Î ¹«Á¶°Ç 0ÀÌ´Ù. */

/** Static Stream Info */
static DI_MEDIA_INFO_t			s_tStreamInfo[DI_MEDIA_MAX_NUM];
/** Static Event Callback */
static DI_MEDIA_EventCallback_t	s_tEventCallback[DI_MEDIA_MAX_NUM];
static DI_MEDIA_FirstPtsCallback_t	s_tEventFirstPtsCallback[DI_MEDIA_MAX_NUM];

/** Static Media StreamInfo */
static stMediaInfo       		s_MfpInfo[DI_MEDIA_MAX_NUM];
/** Static Temp */
static const uint8_t      		s_eos_data[256] = {0xE0, 0x55};
/** Static Media Instance */
static DI_MediaInstance		s_tMediaInstance[DI_MEDIA_MAX_NUM];

/** Static Stream Info */
static HUINT32					s_nCurrentPlayTime[DI_MEDIA_MAX_NUM];

/** Static Stream Info */
static DI_MEDIA_TIMERMODE_e			s_bPlayTimerMode[DI_MEDIA_MAX_NUM];

/** Static LicenseData Callback */
static DI_MEDIA_LicenseDataCallback_t		s_tLicenseDataCallback=NULL;

static const uint8_t tTempWavHeader[] = {
        'R','I','F','F',
        0xFF,0xFF,0xFF,0xFF,     /* unbounded size */
        'W','A','V','E',
        'f','m','t',' ',
        0x12, 0x00, 0x00, 0x00, /* length of fmt block */
        0x01, 0x00,             /* FormatTag -> LPCM */
        0x00, 0x00,             /* Channels */
        0x00, 0x00, 0x00, 0x00, /* sample rate */
        0x00, 0x00, 0x00, 0x00, /* average byte rate */
        0x00, 0x00,             /* block align */
        0x00, 0x00,             /* bits per sample */
        0x00, 0x00,             /* size of extra block */
        'd','a','t','a',
        0xFF,0xFF,0xFF,0x00     /* unbounded size */
};

#if SUPPORT_AUDIOCODEC_LPCM
static unsigned long		s_ulMediaPlaypumpStreamTId = 0;
static unsigned long		s_ulMediaPlaypumpStreamMId = 0;
static DI_MEDIA_PLAYPUMP_STREAM_STATE	s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_NOT_READY;
#endif

/** License Data(Ex> DRM) */
static char					*s_pcLisenceData = NULL;

/** PlayReady Custom Data */
static char					*s_pcPlayReadyCustomData = NULL;

/** Support DRV_MEDIA_CheckStart() */
static HUINT32 s_ulMediaStart = 0;


/* End static variable */
extern HBOOL DI_STREAM_HTTP_GetASFWrmHeader(HUINT8* pucUrl, HUINT8** pucWrmHeader, HUINT32* puiSize, HBOOL *bCocktail);

#if 1 /* FirstPts Detection */
static bmpeg2ts_parser_action
P_MEDIA_GettingTSPayload (bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *cursor, size_t len);

static void P_MEDIA_GettingPESPacket(void *packet_cnxt, batom_accum_t src, batom_cursor *payload, size_t len, const bmpeg2pes_atom_info *info);
#endif

static DI_ERR_CODE		P_MEDIA_ProbeOpen (HUINT8 *pszStreamUrl, stMediaInfo *pMfpInfo);
static DI_ERR_CODE		P_MEDIA_ProbeClose (stMediaInfo *pMfpInfo);
static void				P_MEDIA_ProbeFreeAtom (batom_t atom, void *user);
static void				P_MEDIA_ProbeAsync (void *cntx, bmedia_player_entry *entry);

#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)

typedef enum __enhanced_setposition_accuracy__
{
	DRV_MEDIA_SETPOSITION_ACCURACY_ENTER_MEDIA =0 ,
	DRV_MEDIA_SETPOSITION_ACCURACY_QUIT_MEDIA,
	DRV_MEDIA_SETPOSITION_ACCURACY_ENTER_SET_POS
} DRV_MEDIA_SETPOSITION_ACCURACY_CMD_t;

typedef enum __enhanced_setposition_accuracy_status__
{
	DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_SETPOSITION =0 ,
	DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_ENDSETPOSITION = 0xFF
} DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_t;

static unsigned long sulStatusMonitorThreadQ = 0;
static unsigned long sulStatusMonitorThreadT = 0;
static void DRV_MEDIA_StatusMonitorThread(void *pParam);
static DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_t sulStatusMonitorValue = DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_ENDSETPOSITION;

static DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_t DRV_MEDIA_StatusGet(void);
static void DRV_MEDIA_StatusSet(DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_t status);



#endif

const HINT8 g_dcertPath[] = "/var/lib/humaxtv/drm/";

static const HINT8 *g_SyscmdRemovePRCert = "rm -f /var/lib/humaxtv/drm/bdevcert.dat";
static const HINT8 *g_SyscmdRemovePDCert = "rm -f /var/lib/humaxtv/drm/devcert.dat";
static const HINT8 *g_SyscmdRemovePrivKeys = "rm -f /var/lib/humaxtv/drm/zpriv*.dat";
static const HINT8 *g_SyscmdRemoveSecureStore = "rm -f /var/lib/humaxtv/drm/sample.*";
#if defined(CONFIG_PLAYREADY_20)
static const HINT8 *g_SyscmdRemoveSSPwd   = "rm -f /var/lib/humaxtv/drm/datastore_pwd.dat";
static const HINT8 *g_SyscmdRemoveKeyFile = "rm -f /var/lib/humaxtv/drm/keyfile.dat";
static const HINT8 *g_SyscmdRemoveSRM     = "rm -f /var/lib/humaxtv/drm/pr_srm_data.bin";
#endif

static HUINT8* pucLicenseServerUrl = NULL;

#if defined (CONFIG_PLAYREADY)

#if defined(CONFIG_PLAYREADY_20)
#define SC_PROTOCOL_STRING "drm_clock_acquire_redirect: "

#define HDR_LICGET_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\""
#define HDR_LICACK_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcknowledgeLicense\""

#ifdef DI_MEDIA_DEBUG
	#define PrintDrmDbg dbg_
#else
	#define PrintDrmDbg
#endif

#endif // defined(CONFIG_PLAYREADY_20)

/* var/lib/humaxtv/drm/sample.hd	*/
const DRM_WCHAR rgwchStoreName [] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s', '\0')
};

const HINT8 g_prdysstPath[] = "/var/lib/humaxtv/drm/";
#if 0 /* Caution to warning: 'static' is not at beginning of declaration */
const static HINT8 g_SyscmdRemovePrdySst[] = "rm -f /var/lib/humaxtv/drm/*";
#else
static const HINT8 g_SyscmdRemovePrdySst[] = "rm -f /var/lib/humaxtv/drm*";
#endif

//const HINT8 g_prdysstFile[] = "/usr/bin/prdydrm/sample.hd";

static DRM_DECRYPT_CONTEXT  pDecryptContext;
static NEXUS_DmaHandle dmaHandle;
static NEXUS_KeySlotHandle keyWmdrm;
static NEXUS_KeySlotHandle keyPlayready;
static DI_MEDIA_PLAYREADY_OPL_CB f_OPLcb;

#endif // defined(CONFIG_PLAYREADY)




#if COMMENT
____PRDY20____(){}
#endif

#if defined(CONFIG_PLAYREADY_20)

#define MAX_URL_SIZE	1024
#define MAX_RCVBUFFER_SIZE (64*1024)

#define OEM_SAFE_MEM_FREE(p) do { \
		if (p) { Oem_MemFree((void *)(p)); (p) = NULL; } \
	} while(0)

#define OEM_SAFE_DESTROY(p) do { \
		if (p) { Oem_Destroy(p); (p) = NULL; } \
	} while(0)

static DRV_Error P_DRM_RequestSecureClock2(char *secure_url, char *buf)
{
	URL_t *pTmpUrl=NULL;
	bhttp_engine http;
	int32_t ret;
	int32_t len = 0;
	int32_t resp_len = MAX_RCVBUFFER_SIZE;

	PrintDrmDbg("## %s:\n", __func__);

	pTmpUrl = (URL_t*) Oem_MemAlloc (sizeof(URL_t));
	if(pTmpUrl == NULL)
	{
		PrintError ("%s(%d) Error> can't allocate URL_t. \n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	VK_memset(pTmpUrl, 0x0 ,sizeof(URL_t));
	UTIL_UrlParse (pTmpUrl, (char*)secure_url, 0);

#if 1
	PrintDebug ("[URL_t] psz_protocol  = [%s]\n",pTmpUrl->psz_protocol);
	PrintDebug ("[URL_t] psz_username  = [%s]\n",pTmpUrl->psz_username);
	PrintDebug ("[URL_t] psz_password  = [%s]\n",pTmpUrl->psz_password);
	PrintDebug ("[URL_t] psz_host      = [%s]\n",pTmpUrl->psz_host);
	PrintDebug ("[URL_t] i_port        = [%d]\n",pTmpUrl->i_port);
	PrintDebug ("[URL_t] psz_path      = [%s]\n",pTmpUrl->psz_path);
	PrintDebug ("[URL_t] psz_option    = [%s]\n",pTmpUrl->psz_option);
	PrintDebug ("[URL_t] psz_buffer    = [%s]\n",pTmpUrl->psz_buffer);
	PrintDebug ("[URL_t] psz_cookie    = [%s]\n",pTmpUrl->psz_cookie);
#endif

	/* initialize http engine and get */
	VK_MEM_Memset(&http, 0x0, sizeof(bhttp_engine));
	bhttpengine_init(&http);

	ret = bhttpclient_send_request(&http, "GET", secure_url);
	if (ret != 0)
	{
		PrintError("failed on bhttpclient_send_request\n");
		Oem_MemFree(pTmpUrl);
		return DRV_ERR;
	}

	/* set headers, read response */
	bhttpengine_set_headers(&http, "User-Agent", "Client-User-Agent");
	bhttpengine_write(&http, buf, len); bhttpengine_flush(&http);
	if (bhttpclient_read_responsehdr(&http)) {
		PrintError("failed on readResponseHeader\n");
		Oem_MemFree(pTmpUrl);
		return DRV_ERR;
	}

	/* look for a secure clock url */
	bzero(buf, resp_len);
	len = bhttpengine_read(&http, buf, resp_len);
	bhttpengine_headers_cleanup(&http);
	bhttpengine_close(&http);
#if 1
	PrintDebug("HTTP default get:: resp_len <%d>, buf<%s>\n", len, buf);
#endif
	DRM_BYT_MoveBytes( buf, 0, buf, DRMCRT_strlen( SC_PROTOCOL_STRING ), len - DRMCRT_strlen( SC_PROTOCOL_STRING ) );
	PUT_CHAR( buf, len - DRMCRT_strlen( SC_PROTOCOL_STRING ), '\0' );

	PrintDebug("buf : %s\n", buf);

	VK_snprintf(secure_url, MAX_URL_SIZE-1, "%s", buf);

	Oem_MemFree(pTmpUrl);

	return DRV_OK;
}

static DRV_Error P_DRM_RequestSecureClock(DRM_APP_CONTEXT *pAppContext, DRM_CHAR *pszChallenge, char *rgchURL)
{
	DRM_RESULT dResult;
	DRM_RESULT		 drResponse 	= DRM_SUCCESS;

	bhttp_engine http;
	int32_t ret;
	int32_t len = 0;
	int32_t resp_len = MAX_RCVBUFFER_SIZE;
	char *secure_url = NULL;
	const char* clstr = NULL;
	char* buf;

	PrintDrmDbg("## %s:\n", __func__);

	buf = Oem_MemAlloc((DRM_DWORD)resp_len + 1);
	if (buf == NULL) return DRV_ERR;

	/* append drm specific tokens */
	PrintDebug("getting to : %s\n", rgchURL);

	/* initialize http engine and get */
	VK_MEM_Memset(&http, 0x0, sizeof(bhttp_engine));
	bhttpengine_init(&http);
	ret = bhttpclient_send_request(&http, "GET", rgchURL);
	if (ret != 0)
	{
		PrintError("failed on bhttpclient_send_request\n");
		goto error_2;
	}

	/* set headers, read response */
	bhttpengine_set_headers(&http, "User-Agent", "Client-User-Agent");
	bhttpengine_write(&http, buf, len); bhttpengine_flush(&http);
	if (bhttpclient_read_responsehdr(&http)) {
		PrintError("failed on readResponseHeader\n");
		goto error_2;
	}

	PrintDrmDbg("## sclk http resp code: %d\n", http._responseCode);

	/* look for a secure clock url */
	VK_MEM_Memset(buf, 0x0, resp_len + 1);
	len = bhttpengine_read(&http, buf, resp_len);
#if 1
	PrintDebug("HTTP default get:: resp_len <%d>, buf<%s>\n", len, buf);
#endif
	PrintDrmDbg("## http response\n");

	clstr = bhttpengine_get_header((bhttp_engine*)&http, "Location");
	if(clstr == NULL)
	{
		PrintError("Location is not found\n");
		goto error_2;
	}

	secure_url = (char *) Oem_MemAlloc( MAX_URL_SIZE );
	VK_memset(secure_url, 0x0, MAX_URL_SIZE);

	VK_snprintf(secure_url, MAX_URL_SIZE-1, "%s", clstr);
	len = 0;

	bhttpengine_headers_cleanup(&http);
	bhttpengine_close(&http);

	if(P_DRM_RequestSecureClock2(secure_url, buf) != DRV_OK)
	{
		goto error_1;
	}

	PrintDebug("posting to : %s\n", secure_url);

	/* initialize http engine and post */
	bhttpengine_init(&http);
	if ((ret = bhttpclient_post(&http, (char*)secure_url)) != 0) {
		PrintError("bhttp_license_post failed on POST\n");
		goto error_1;
	}

	len = 0;

	len += VK_snprintf(buf + len, resp_len -1 ,"%s", pszChallenge);

	/* set headers, read response */
	bhttpengine_set_headers(&http, "Accept", "Accept: */*");
	bhttpengine_set_headers(&http, "Content-Type", "application/x-www-form-urlencoded");
	bhttpengine_set_header(&http,  "Content-Length", len);
	bhttpengine_set_headers(&http, "User-Agent", "Client-User-Agent");
	bhttpengine_set_headers(&http, "Proxy-Connection", "Keep-Alive");
	bhttpengine_set_headers(&http, "Pragma", "no-cache");
	bhttpengine_set_headers(&http, "Cache-Control", "no-cache");

	bhttpengine_write(&http, buf, len); bhttpengine_flush(&http);
	if (bhttpclient_read_responsehdr(&http)) {
		PrintError("failed on readResponseHeader\n");
		goto error_1;
	}

	/* look for a license */
	VK_MEM_Memset(buf, 0x0, resp_len + 1);
	len = bhttpengine_read(&http, buf, resp_len);
	bhttpengine_headers_cleanup(&http);
	bhttpengine_close(&http);
#if 1
	PrintDebug("HTTP default post:: resp_len <%d>, buf<%s>\n", len, buf);
#endif

	 dResult = Drm_SecureClock_ProcessResponse( pAppContext,
										  (DRM_BYTE *)buf,
										  len,
										 &drResponse );

	if ( drResponse != DRM_SUCCESS )
	{
		PrintError( "Response contains error %d\n", drResponse );
		goto error_1;
	}
	if ( dResult != DRM_SUCCESS )
	{
		PrintError( "Error Processing response %d\n", dResult );
		goto error_1;
	}
	else
	{
		/* if dr is SUCCESS then report drResponse */

	}
	Oem_MemFree((DRM_VOID *)secure_url);
	Oem_MemFree(buf);

	PrintDrmDbg("## secure clk ok\n");

	return DRV_OK;

error_1:
	Oem_MemFree((DRM_VOID *)secure_url);
error_2:
	Oem_MemFree(buf);
	return DRV_ERR;
}


static DRV_Error P_DRM_PlayReady_GetSecureClock(void *pContext)
{
	DRV_Error ret = DRV_OK;
	DRM_RESULT dResult;
/*1*/
	DRM_WCHAR		*pwszSecTime		= NULL;
	DRM_DWORD		 cchSecTime 		= 0;
	DRM_DWORD		 dwFlag 			= 0;
	DRM_BYTE		*pbTimeStatus		= NULL;
	DRM_DWORD		 cbTimeStatus		= 0;
	DRM_DWORD		 cchChallenge		= 0;
	DRM_DWORD		 cchURL 			= 0;
	DRM_CHAR		*pszChallenge		= NULL;
	DRM_WCHAR		*pwszURL			= NULL;

/*2*/
	DRM_CONST_STRING dstrChallenge	 = EMPTY_DRM_STRING;
	DRM_CONST_STRING dstrTemp		 = EMPTY_DRM_STRING;
	DRM_CONST_STRING dstrData		 = EMPTY_DRM_STRING;
	DRM_CONST_STRING dstrURL		 = EMPTY_DRM_STRING;
	DRM_CHAR		 *rgchURL		 = NULL;
	DRM_BYTE		*pbTemp 		 = NULL;
	DRM_DWORD		 cbTemp 		 = 0;
	DRM_SUBSTRING	 dasstrChallenge = EMPTY_DRM_SUBSTRING;
	DRM_APP_CONTEXT* pAppContext=NULL;

	PrintDrmDbg("## %s:\n", __func__);

	pAppContext = (DRM_APP_CONTEXT*)pContext;
/*1*/
	dResult = Drm_SecureClock_GetValue( pAppContext, pwszSecTime, &cchSecTime, &dwFlag, pbTimeStatus, &cbTimeStatus );

	if ( dResult != DRM_E_BUFFERTOOSMALL )
	{
		PrintError("dResult != DRM_E_BUFFERTOOSMALL, dResult = %x\n", dResult);
		return DRV_ERR;
	}

	//pbTimeStatus = (DRM_BYTE *) Oem_MemAlloc( cbTimeStatus + 10 );
	// ¸Þ¸ð¸® ÇÒ´ç ¾øÀÌ ¹Ù·Î ÀÌ·¸°Ô ¶Ç ºÒ·¯ºÁ¾ß ÀÇ¹Ì ¾øÀ» µí..
	// ¿©ÀüÈ÷ DRM_E_BUFFERTOOSMALL ¿¡·¯ ¹ß»ýÇÔ.
	dResult = Drm_SecureClock_GetValue( pAppContext, pwszSecTime, &cchSecTime, &dwFlag, pbTimeStatus, &cbTimeStatus );
	PrintDrmDbg("## sc getvalue: dr 0x%x, sectime sz %d, flag %d, timestat sz %d\n", (int)dResult, cchSecTime, dwFlag, cbTimeStatus);

	if ( DRM_CLK_NOT_SET == dwFlag || DRM_CLK_NEEDS_REFRESH == dwFlag )
	{
		PrintDrmDbg("## sclk, gen challenge..\n");
		dResult = Drm_SecureClock_GenerateChallenge( pAppContext,
												NULL,
											   &cchURL,
												NULL,
											   &cchChallenge );

		if( dResult != DRM_E_BUFFERTOOSMALL )
		{
			PrintError("dResult != DRM_E_BUFFERTOOSMALL\n");
			return DRV_ERR;
		}
		pszChallenge = (DRM_CHAR *)  Oem_MemAlloc(cchChallenge);
		VK_memset(pszChallenge, 0x0, cchChallenge);
		pwszURL 	 = (DRM_WCHAR *) Oem_MemAlloc(cchURL * SIZEOF (DRM_WCHAR));
		VK_memset(pwszURL, 0x0, cchURL * SIZEOF (DRM_WCHAR));
		dResult = Drm_SecureClock_GenerateChallenge( pAppContext,
												  pwszURL,
												 &cchURL,
												  (DRM_BYTE *) pszChallenge,
												 &cchChallenge);
//		VK_printf("cchURL = %d\n", cchURL);
//		VK_printf("pwszURL = %s\n", pwszURL);
		PrintDebug("pszChallenge = %s\n", pszChallenge);
	}
	else
	{
		PrintError("! (DRM_CLK_NOT_SET == dwFlag || DRM_CLK_NEEDS_REFRESH == dwFlag)\n");
		return DRV_OK;
	}

/*2*/

	/* Decode base64 encoded challenge */
   pbTemp = (DRM_BYTE *) Oem_MemAlloc( cchChallenge + 10 ); // ±æÀÌ´Â ´õ ÁÙ¾îµé °ÍÀÌ¹Ç·Î overflow µÇÁø ¾ÊÀ» °ÍÀÓ.
   VK_memset(pbTemp, 0x0, cchChallenge);
   cbTemp  = cchChallenge;
   dasstrChallenge.m_cch = cchChallenge;
   dResult = DRM_B64_DecodeA( (DRM_CHAR*)pszChallenge, &dasstrChallenge, &cbTemp, pbTemp, 0 );

	PrintDrmDbg("## base64 decode: dr 0x%x, decoded size %d\n", (int)dResult, cbTemp);

#if 0
	FILE *fp;
	fp = fopen("/sclkchange.txt", "wb");
	if (fp) {
		VK_printf("sclkchange is dummped to file\n");
		fwrite(pbTemp, 1, cbTemp, fp);
		fclose(fp);
	}
#endif

   dstrChallenge.pwszString = (DRM_WCHAR*)pbTemp;
   dstrChallenge.cchString = cbTemp / SIZEOF(DRM_WCHAR);

	dResult = DRM_XML_GetNode( &dstrChallenge, &g_dstrTagDRMReqRoot, NULL, NULL, 0, NULL, &dstrTemp );

	dResult = DRM_XML_GetNode( &dstrTemp, &g_dstrTagData, NULL, NULL, 0, &dstrData, NULL );

	/* Read Data section */
	dResult = DRM_XML_GetNode( &dstrTemp, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrData );

	/* Get URL */
	dResult = DRM_XML_GetNode( &dstrData, &g_dstrTagURL, NULL, NULL, 0, NULL, &dstrURL );

	cchURL = 1024;
	rgchURL = (DRM_CHAR *) Oem_MemAlloc( cchURL );
	VK_memset(rgchURL, 0x0, cchURL);
	dResult = DRM_STR_UTF16toUTF8( dstrURL.pwszString, dstrURL.cchString, rgchURL, 0, &cchURL );

	PrintDebug("%s %d - rgchURL = %s\n", __FUNCTION__, __LINE__, rgchURL);


	if(P_DRM_RequestSecureClock(pAppContext, pszChallenge, rgchURL) != DRV_OK)
	{
		PrintError("get_secure_clock error\n");
		ret = DRV_ERR;
	}

	Oem_MemFree(pszChallenge);
	Oem_MemFree(pwszURL);
	Oem_MemFree(pbTemp);
	Oem_MemFree(rgchURL);

	return ret;
}


DRM_RESULT PlayReadyBindCallback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType, void *f_pv)
{
	Playready_OutputControl_t stOPL;
	DI_MEDIA_PLAYREADY_OPL_CB fpOPL = f_OPLcb;

	(void)f_pv; // not used

	if(f_dwCallbackType == DRM_PLAY_OPL_CALLBACK)
	{
		PrintError("PLAY OPL!\n");
		if(fpOPL != NULL)
		{
			//DRM_PLAY_OPL_EX2  type == Playready_OutputControl_t type
			VK_MEM_Memcpy(&stOPL, f_pvPolicyCallbackData, sizeof(Playready_OutputControl_t));
			(*fpOPL)(&stOPL);
		}
	}
	else if(f_dwCallbackType == DRM_COPY_OPL_CALLBACK)
	{
		PrintError("COPY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK)
	{
		PrintError("INCLUSION OPL!\n");
		if(((DRM_INCLUSION_LIST_CALLBACK_STRUCT*)f_pvPolicyCallbackData)->dwChainDepth != 0)
		{
			//drm_handle->private.playready.bInclusionListExist = TRUE;
		}
		else
		{
			//drm_handle->private.playready.bInclusionListExist = FALSE;
		}
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK)
	{
		PrintError("CONDITION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK)
	{
		PrintError("ACTION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK)
	{
		PrintError("QUERY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK)
	{
		PrintError("RESOLVE OPL!\n");
	}
	else
	{
		PrintError("Invalid Callback Type!\n");
	}
	return DRM_SUCCESS;
}

DRM_RESULT PlayReadyCommitCallback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType,void *f_pv)
{
	f_pvPolicyCallbackData = f_pvPolicyCallbackData;
	f_pv = f_pv;

	if(f_dwCallbackType == DRM_PLAY_OPL_CALLBACK)
	{
		PrintError("PLAY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_COPY_OPL_CALLBACK)
	{
		PrintError("COPY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK)
	{
		PrintError("INCLUSION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK)
	{
		PrintError("CONDITION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK)
	{
		PrintError("ACTION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK)
	{
		PrintError("QUERY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK)
	{
		PrintError("RESOLVE OPL!\n");
	}
	else
	{
		PrintError("Invalid Callback Type!\n");
	}
	return DRM_SUCCESS;
}

DRM_CONTENT_SET_PROPERTY P_DRM_PlayReady_CheckHeaderVersion(HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize)
{
	HUINT32 uiOffset=0;
	DRM_CONTENT_SET_PROPERTY eVersion=DRM_CSP_HEADER_NOT_SET;

/*************************
* WRM Validation check
*************************/
	if(uiWrmHeaderSize < 60)
	{
		PrintError("Invalid WRM HEADER! too small!\n");
		return eVersion;
	}

/*************************
* WRMHEADER tag with version info
*************************/
	while(uiOffset <= (uiWrmHeaderSize-18))
	{
		if((pucWrmHeader[uiOffset] == 'v') && (pucWrmHeader[uiOffset+2] == 'e') && (pucWrmHeader[uiOffset+4] == 'r') &&
			(pucWrmHeader[uiOffset+6] == 's') && (pucWrmHeader[uiOffset+8] == 'i') && (pucWrmHeader[uiOffset+10] == 'o') && (pucWrmHeader[uiOffset+12] == 'n'))
		{
			break;
		}
		uiOffset++;
	}

	if(pucWrmHeader[uiOffset+18] == '2')
	{
		eVersion = DRM_CSP_V2_HEADER;
	}
	else if(pucWrmHeader[uiOffset+18] == '4')
	{
		eVersion = DRM_CSP_V4_HEADER;
	}
	else
	{
		PrintError("Invalid version!\n");
	}

	return eVersion;
}

void P_DRM_PlayReady_CleanupCaches(void)
{
	PrintError("Clean caches\n");

	// below files are playready 1.x data files.
	VK_SYSTEM_Command(g_SyscmdRemovePRCert);
	VK_SYSTEM_Command(g_SyscmdRemovePDCert);
	VK_SYSTEM_Command(g_SyscmdRemovePrivKeys);
	VK_SYSTEM_Command(g_SyscmdRemoveSecureStore);

	// below files are playready 2.0 data files.
	VK_SYSTEM_Command(g_SyscmdRemoveSSPwd);
	VK_SYSTEM_Command(g_SyscmdRemoveKeyFile);
	VK_SYSTEM_Command(g_SyscmdRemoveSRM);
}

HINT8* P_DRM_PlayReady_AddMaxdomeCookie(HINT8* pUrl)
{
	HINT32 new_string_size = 0;
	HINT8 *man = NULL;
	int nRet;

	if (s_tLicenseDataCallback)
	{
		(*s_tLicenseDataCallback)(pUrl, 0);
		if (s_pcLisenceData == NULL)
		{
			PrintError("s_pcLisenceData is NULL!!\n");
			return NULL;
		}

		new_string_size = VK_strlen(pUrl) + VK_strlen(s_pcLisenceData) + 1 + 1;

		man = Oem_MemAlloc(new_string_size);
		if(man == NULL)
		{
			PrintError("Error while VK_malloc new url in %s(%d)\n", __FUNCTION__, __LINE__);
			return NULL;
		}

		nRet = VK_snprintf ((char*)man, new_string_size, "%s&%s", pUrl, s_pcLisenceData);
		if(nRet<0)
		{
			PrintError("can't make new url in %s(%d)\n", __FUNCTION__, __LINE__);
			return NULL;
		}
	}

	return man;
}

DRV_Error P_DRM_PlayReady_GetLicense(HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize)
{
	DRM_RESULT dResult;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
	DRM_DWORD f_cRights=0;
	DRM_CHAR* f_pchSilentURL=NULL;
	DRM_DWORD f_pcchSilentURL=0;
	DRM_BYTE* f_pbChallenge=NULL;
	DRM_DWORD f_pcbChallenge=0;
	DRM_LICENSE_RESPONSE LicenseResponse;
	DRM_BYTE *pbRevocationBuffer=NULL;
	HINT32 iResult;
	HUINT8* pucResponseData=NULL;
	HUINT8* pucResponseStart=NULL;
	HUINT32 uiResponseSize=0;
	DRM_DWORD cRights = 1;
	const DRM_CONST_STRING *apdcsRights [cRights];
	DRM_APP_CONTEXT* pAppContext = NULL;
	HBOOL bAppContextInitialized = FALSE;
	//DRM_Session_t *drm_handle=NULL;
	DRM_DECRYPT_CONTEXT *pstDecryptContext=NULL;
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;
	DRM_VOID *f_pOEMContext = NULL;
	HUINT8* pucServerUrlOverride=NULL;
	DRM_CHAR* pcChallengeUrl = NULL;
	const DRM_CHAR *f_pchCustomData = NULL;
	DRM_DWORD f_cchCustomData = 0;
	DRM_BYTE *__macro_pbOpaqueBuffer = NULL;
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	HINT32 lRemainingOemCreateRetryCount = 1; /* retry if create failed. */
#endif
	HINT32 lRemainingDrmInitRetryCount = 1; /* retry if initialize failed. */

	PrintDrmDbg("## %s: WrmHeaderSize %d\n", __func__, uiWrmHeaderSize);

	if(pucWrmHeader == NULL)
	{
		PrintError("Invalid Parameter!\n");
		return DRV_ERR;
	}

	pstDecryptContext = &pDecryptContext;
	apdcsRights[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;

	pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	if(pAppContext == NULL)
	{
		PrintError("Out of memory for AppContext!\n");
		P_DRM_PlayReady_CleanupCaches();
		return DRV_ERR;
	}
	VK_memset(pAppContext, 0x0, SIZEOF(DRM_APP_CONTEXT));

	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);
	if (__macro_pbOpaqueBuffer == NULL)
	{
		PrintError("Out of memory for OpaqueBuffer!\n");
		dResult = DRV_ERR;
		goto label_exit;
	}
	VK_memset(__macro_pbOpaqueBuffer, 0x0, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);

/********************
*	1. Initialize handle
********************/
label_retry_init:
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	PrintDrmDbg("## 1.0 Oem_Create\n");
	OEM_SAFE_DESTROY(f_pOEMContext); // destroy first if already created.
	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		PrintError("OEMContext create failed!\n");
		P_DRM_PlayReady_CleanupCaches();
		if (lRemainingOemCreateRetryCount-- > 0)
		{
			PrintError("Retry Oem_Create\n");
			goto label_retry_init;
		}
		dResult = DRV_ERR;
		goto label_exit;
	}
#endif

	PrintDrmDbg("## 1.5 Drm_Initialize\n");
	dResult = Drm_Initialize(pAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Initialize failed! 0x%X\n", dResult);
		P_DRM_PlayReady_CleanupCaches();
		if (lRemainingDrmInitRetryCount-- > 0)
		{
			PrintError("Retry Drm_Initialize\n");
			goto label_retry_init;
		}
		dResult = DRV_ERR;
		goto label_exit;
	}
	bAppContextInitialized = TRUE;

/********************
* 	1.5 Get Secure Clock
********************/
	if(P_DRM_PlayReady_GetSecureClock(pAppContext) != DRV_OK)
	{
		PrintError("GetSecureClock failed\n");
		dResult = DRV_ERR;
		goto label_exit;
	}

/********************
*	2. Set WRM Header
********************/
	eHeaderVersion = P_DRM_PlayReady_CheckHeaderVersion(pucWrmHeader, uiWrmHeaderSize);
	PrintDebug("HeaderVersion %s\n", eHeaderVersion==DRM_CSP_V4_HEADER ? "V4" : "??");

	dResult = Drm_Content_SetProperty(pAppContext, eHeaderVersion, pucWrmHeader, uiWrmHeaderSize);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Content_SetProperty failed! 0x%X\n", dResult);
		dResult = DRV_ERR;
		goto label_exit;
	}

/********************
*	3. check the local store first
********************/
	dResult = Drm_Reader_Bind (pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)PlayReadyBindCallback,
				NULL, pstDecryptContext);
	if(dResult == DRM_SUCCESS)
	{
		PrintError("Local Store already has the license!\n");
		dResult = DRV_OK;
		goto label_exit;
	}
	else
	{
		PrintDebug("License doesn't exist in SecureStore.\n");
	}

/*************************
*	3.5 Ticket Check
*************************/
	PrintDrmDbg("## 3.5 Ticket Check\n");
	if (s_pcPlayReadyCustomData)
	{
		PrintDebug("Add CustomData\n");
		f_cchCustomData = VK_strlen(s_pcPlayReadyCustomData);
		f_pchCustomData = Oem_MemAlloc(f_cchCustomData+1);
		if (f_pchCustomData == NULL)
		{
			PrintError("Out of memory for CustomData!\n");
			dResult = DRV_ERR;
			goto label_exit;
		}
		VK_MEM_Memcpy((void *)f_pchCustomData, s_pcPlayReadyCustomData, f_cchCustomData+1);
	}

/********************
*	4. Generate Challenge
********************/
	PrintDrmDbg("## 4. Generate challenge..\n");
	rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
	f_cRights = NO_OF(rgpdstrRights);

	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		rgpdstrRights,
		f_cRights,
		NULL, /* f_poDomainID */
		f_pchCustomData, /* const DRM_CHAR *f_pchCustomData */
		f_cchCustomData,   /* DRM_DWORD f_cchCustomData */
		NULL, /* DRM_CHAR *f_pchSilentURL */
		&f_pcchSilentURL,
		NULL, /* DRM_CHAR *f_pchNonSilentURL */
		NULL, /* DRM_DWORD *f_pcchNonSilentURL */
		NULL, /* DRM_BYTE *f_pbChallenge */
		&f_pcbChallenge);

	if(dResult == DRM_E_BUFFERTOOSMALL)
	{
		PrintDrmDbg("## small buffer. needed url:%d, chl:%d. retry..\n", f_pcchSilentURL, f_pcbChallenge);
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(f_pcchSilentURL * SIZEOF (DRM_CHAR));
		VK_memset(f_pchSilentURL, 0x0, f_pcchSilentURL * SIZEOF (DRM_CHAR));
	    f_pbChallenge = (DRM_BYTE*) Oem_MemAlloc(f_pcbChallenge * SIZEOF(DRM_BYTE));
		VK_memset(f_pbChallenge, 0x0, f_pcbChallenge * SIZEOF(DRM_BYTE));

		if (f_pchSilentURL == NULL || f_pbChallenge == NULL)
		{
			PrintError("Out of memory for url and challenge!\n");
			dResult = DRV_ERR;
			goto label_exit;
		}
		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			rgpdstrRights,
			f_cRights,
			NULL,
			f_pchCustomData,
			f_cchCustomData,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);

		if(dResult != DRM_SUCCESS)
		{
			PrintError("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult);
			dResult = DRV_ERR;
			goto label_exit;
		}
		f_pbChallenge[f_pcbChallenge] = '\0';
		PrintDrmDbg("## Generate challenge OK. challenge size %d\n", f_pcbChallenge);
		//PrintDebug("Challenge: [%s]\n", f_pbChallenge);
	}
	else
	{
		PrintError("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult);
		dResult = DRV_ERR;
		goto label_exit;
	}

/********************
*	5. Override URL & Add Cookie
********************/
	pucServerUrlOverride = pucLicenseServerUrl;

	PrintDrmDbg("## 5. overwrite url: '%s'\n", pucServerUrlOverride);
	if(pucServerUrlOverride)
	{
		Oem_MemFree(f_pchSilentURL);
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(VK_strlen(pucServerUrlOverride) + 1);
		VK_MEM_Memset(f_pchSilentURL, 0x0, VK_strlen(pucServerUrlOverride) + 1);
		VK_MEM_Memcpy(f_pchSilentURL, pucServerUrlOverride, VK_strlen(pucServerUrlOverride));
	}

	pcChallengeUrl = P_DRM_PlayReady_AddMaxdomeCookie(f_pchSilentURL);
	if(pcChallengeUrl)
	{
		Oem_MemFree(f_pchSilentURL);
		f_pchSilentURL = pcChallengeUrl;
		pcChallengeUrl = NULL;
	}

/********************
*	6. POST Challenge (soap)
********************/
	if(eHeaderVersion == DRM_CSP_V4_HEADER)
	{
		//bhttpclient_set_certificate(pPrivateKeyPath, pCertificatePath);
		PrintDrmDbg("post_soap\n");
		iResult = bhttpclient_license_post_soap(
			(char*)f_pchSilentURL,
			(char*)f_pbChallenge,
			0,
			2000,
			HDR_LICGET_ACTION_VALUE,
			&pucResponseData,
			&pucResponseStart,
			&uiResponseSize);
		if(iResult != 0)
		{
			PrintError("bhttpclient_license_post_soap failed!\n");
			dResult = DRV_ERR;
			goto label_exit;
		}
	}
	else
	{
		iResult = bhttpclient_license_post_default(
			(char*)f_pchSilentURL,
			(char*)f_pbChallenge,
			0,
			2000,
			NULL,
			&pucResponseData,
			&pucResponseStart,
			&uiResponseSize);
		if(iResult != 0)
		{
			PrintError("bhttpclient_license_post_default failed!\n");
			dResult = DRV_ERR;
			goto label_exit;
		}
	}
	//PrintDebug("pucResponseData[%d] %s\n", uiResponseSize, pucResponseData);
#if 0
	FILE *fp = fopen("/prdyresp.dat", "wb");
	if (fp) {
		VK_printf("dump process response to file\n");
		fwrite(pucResponseStart, 1, uiResponseSize, fp);
		fclose(fp);
	}
#endif

/********************
*	7. Process Response
********************/
	PrintDrmDbg("## Set RevocationBuffer..\n");
	pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc(REVOCATION_BUFFER_SIZE);
	if (pbRevocationBuffer == NULL)
	{
		PrintError("Out of memory for for RevocationBuffer sz %d\n", REVOCATION_BUFFER_SIZE);
		dResult = DRV_ERR;
		goto label_exit;
	}
	dResult = Drm_Revocation_SetBuffer(pAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Revocation_SetBuffer failed! [%x]\n", dResult);
	}

	PrintDrmDbg("## ProcessResponse..\n");
	VK_MEM_Memset(&LicenseResponse, 0x0, sizeof(DRM_LICENSE_RESPONSE));
	dResult = Drm_LicenseAcq_ProcessResponse(pAppContext,
		DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
		NULL,
		NULL,
		pucResponseStart,
		uiResponseSize,
		&LicenseResponse);

	PrintDebug("---LicenseResponse.m_eType = %d\n", LicenseResponse.m_eType);

	if(dResult == DRM_SUCCESS)
	{
		PrintDebug("License is Stored well! [%d]\n", LicenseResponse.m_cAcks);
	}
	else if (dResult == DRM_S_MORE_DATA)
	{
		PrintDebug("License is Stored well, and some of custom data exist! [%d]\n", LicenseResponse.m_cAcks);
	}
	else
	{
		PrintError("Drm_LicenseAcq_ProcessResponse returned 0x%X\n", dResult);
		dResult = DRV_ERR;
		goto label_exit;
	}

/*************************
*	8. License Acknowledgment
*	Note: Sending Ack fail doesn't cause the error return!
*************************/
	PrintDebug("LicenseResponse.m_cbTransactionID = %d\n", LicenseResponse.m_cbTransactionID);

	if(LicenseResponse.m_cbTransactionID > 0)
	{
		DRM_DWORD cbAckChallenge;
		DRM_BYTE* pbAckChallenge=NULL;
		DRM_DWORD cbAckResponse;
		DRM_BYTE* pbAckResponse=NULL;

		dResult = Drm_LicenseAcq_GenerateAck(pAppContext, &LicenseResponse, NULL, &cbAckChallenge);
		if(dResult == DRM_E_BUFFERTOOSMALL)
		{
			pbAckChallenge = (DRM_BYTE*) Oem_MemAlloc(cbAckChallenge + 1);
			if(pbAckChallenge == NULL)
			{
				PrintError("Out of Memory for AckChallenge!\n");
				goto label_skip_ack;
			}

			VK_memset(pbAckChallenge, 0x0, cbAckChallenge + 1);
			dResult = Drm_LicenseAcq_GenerateAck(pAppContext, &LicenseResponse, pbAckChallenge, &cbAckChallenge);
			if(dResult != DRM_SUCCESS)
			{
				PrintError("Drm_LicenseAcq_GenerateAck returned 0x%X\n", dResult);
				goto label_skip_ack;
			}
			pbAckChallenge[cbAckChallenge] = '\0';
			//PrintDebug("pbAckChallenge[%d] = %s\n", cbAckChallenge, pbAckChallenge);

			iResult = bhttpclient_license_post_soap(
				(char*)f_pchSilentURL,
				(char*)pbAckChallenge,
				0,
				0,
				HDR_LICACK_ACTION_VALUE,
				&pbAckResponse,
				&pbAckResponse,
				&cbAckResponse);

			//PrintDebug("pbAckResponse[%d] = %s\n", cbAckResponse, pbAckResponse);
			if(iResult != 0)
			{
				PrintError("bhttpclient_license_post_soap failed!\n");
				goto label_skip_ack;
			}

			dResult = Drm_LicenseAcq_ProcessAckResponse(pAppContext, pbAckResponse, cbAckResponse, NULL);
			if(dResult == DRM_SUCCESS)
			{
				PrintError("Drm_LicenseAcq_ProcessAckResponse Succeeded!\n");
			}
#if 1 /* ignore "licenses are not veritied" message for dark nkght stream(SSTR). */
			else if (dResult == DRM_S_MORE_DATA)
			{
				PrintError("Drm_LicenseAcq_ProcessAckResponse Succeeded, and some of data exist!\n");
			}
#endif
			else
			{
				PrintError("Drm_LicenseAcq_ProcessAckResponse returned 0x%X\n", dResult);
			}
		}
		else
		{
			PrintError("Drm_LicenseAcq_GenerateAck returns 0x%x\n", dResult);
		}

label_skip_ack:
		OEM_SAFE_MEM_FREE(pbAckChallenge);
		OEM_SAFE_MEM_FREE(pbAckResponse);
	}

/*************************
*	9. Bind the license again
*************************/
	PrintDrmDbg("## 9. Reader Bind\n");
	dResult = Drm_Reader_Bind (pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)PlayReadyBindCallback,
				NULL /*media_session_handle*/, pstDecryptContext);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Reader_Bind returns 0x%x\n", dResult);
		dResult = DRV_ERR;
		goto label_exit;
	}

/*************************
*	10. Commit, License processing all done!
*************************/
	PrintDrmDbg("## 10. Reader Commit\n");
	dResult = Drm_Reader_Commit(pAppContext, (DRMPFNPOLICYCALLBACK)PlayReadyCommitCallback, NULL);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Reader_Commit returns 0x%x\n", dResult);
		dResult = DRV_ERR;
		goto label_exit;
	}

	PrintDrmDbg("## License Process OK\n");
	dResult = DRV_OK;

label_exit:

	OEM_SAFE_MEM_FREE(f_pbChallenge);
	OEM_SAFE_MEM_FREE(f_pchSilentURL);
	OEM_SAFE_MEM_FREE(pcChallengeUrl); // this should be null actually.
	OEM_SAFE_MEM_FREE(pbRevocationBuffer);
	OEM_SAFE_MEM_FREE(f_pchCustomData);
	OEM_SAFE_MEM_FREE(__macro_pbOpaqueBuffer); // fix memory leak!

	if (pucResponseData != NULL)
	{
		BKNI_Free(pucResponseData);
		pucResponseData = NULL;
		pucResponseStart = NULL;
	}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	OEM_SAFE_DESTROY(f_pOEMContext);
#endif
	if (pAppContext != NULL && bAppContextInitialized)
	{
		Drm_Uninitialize(pAppContext);
	}
	OEM_SAFE_MEM_FREE(pAppContext);

	return DRV_OK;
}

#endif // #if defined(CONFIG_PLAYREADY_20)


static void P_MEDIA_DMA_CompletedCallback(void *context, int param)
{
	unsigned long		vkEvtId = (unsigned long)context;
	unsigned long		vkEvt = (unsigned long)param;
	int					vkRet;

	vkRet = VK_EVENT_Send(vkEvtId, vkEvt);
	if (VK_OK != vkRet)
	{
		PrintError("%s VK_EVENT_Send error\n", __FUNCTION__);
	}
}

#if COMMENT
____PRDY10____(){}
#endif

#if defined(CONFIG_PLAYREADY)

// ¾Æ·¡´Â ±¸ Broadcom Á¦°ø DRM library¸¦ »ç¿ëÇÒ °æ¿ìÀÇ ÄÚµåÀÓ.
#if !defined(CONFIG_PLAYREADY_20)

DRM_RESULT DummyBindCallback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType,void *f_pv)
{
	Playready_OutputControl_t stOPL;
	DRM_POLICY_CALLBACK_TYPE eType = f_dwCallbackType;
	f_pv = f_pv;

	if(eType == DRM_PLAY_OPL_CALLBACK)
	{
		PrintDebug("PLAY OPL!\n");
		VK_MEM_Memcpy(&stOPL, f_pvPolicyCallbackData, sizeof(Playready_OutputControl_t));
		if(f_OPLcb != NULL)
		{
			f_OPLcb(&stOPL);
		}
	}
	else if(eType == DRM_COPY_OPL_CALLBACK)
	{
		PrintDebug("COPY OPL!\n");
	}
	else if(eType == DRM_INCLUSION_LIST_CALLBACK)
	{
		PrintDebug("INCLUSION OPL!\n");
	}
	else if(eType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK)
	{
		PrintDebug("CONDITION OPL!\n");
	}
	else if(eType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK)
	{
		PrintDebug("ACTION OPL!\n");
	}
	else if(eType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK)
	{
		PrintDebug("QUERY OPL!\n");
	}
	else if(eType == DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK)
	{
		PrintDebug("RESOLVE OPL!\n");
	}
	else
	{
		PrintError("Invalid Callback Type!\n");
	}

     return DRM_SUCCESS;
}

DRM_CONTENT_SET_PROPERTY P_MEDIA_PlayReady_CheckHeaderVersion(HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize)
{
	HUINT32 uiOffset=0;
	DRM_CONTENT_SET_PROPERTY eVersion=DRM_CSP_HEADER_NOT_SET;

/*************************
* WRM Validation check
*************************/
	if(uiWrmHeaderSize < 60)
	{
		PrintError("Invalid WRM HEADER! too small!\n");
		return eVersion;
	}

/*************************
* WRMHEADER tag with version info
*************************/
	while(uiOffset <= (uiWrmHeaderSize-18))
	{
		if((pucWrmHeader[uiOffset] == 'v') && (pucWrmHeader[uiOffset+2] == 'e') && (pucWrmHeader[uiOffset+4] == 'r') &&
			(pucWrmHeader[uiOffset+6] == 's') && (pucWrmHeader[uiOffset+8] == 'i') && (pucWrmHeader[uiOffset+10] == 'o') && (pucWrmHeader[uiOffset+12] == 'n'))
		{
			break;
		}
		uiOffset++;
	}

	if(pucWrmHeader[uiOffset+18] == '2')
	{
		eVersion = DRM_CSP_V2_HEADER;
	}
	else if(pucWrmHeader[uiOffset+18] == '4')
	{
		eVersion = DRM_CSP_V4_HEADER;
	}
	else
	{
		PrintError("Invalid version!\n");
	}

	return eVersion;
}

DRV_Error P_MEDIA_ProcessLicense(DRM_APP_CONTEXT* poAppContext, HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize, DI_license_response* response)
{
	DRM_RESULT dr;
	DRM_BYTE* pbData;
	DRM_DWORD cbData;
	DRM_LICENSE_RESPONSE oLicenseResponse;
	DRM_BYTE *pbRevocationBuffer = NULL;
	DRM_DWORD cRights = 1;
	const DRM_CONST_STRING *apdcsRights [cRights];
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;

	apdcsRights[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;

/*************************
*	1. DRM handle check
*************************/
	if(poAppContext == NULL)
	{
		PrintError("poAppContext is NULL!\n");
		return DRV_ERR;
	}

/*************************
*	2. Drm session init.
*************************/
	dr = Drm_Reinitialize(poAppContext);
	if(dr != DRM_SUCCESS)
	{
		PrintError("Drm_Reinitialize failed![0x%X]\n", dr);
		return DRV_ERR;
	}

/*************************
*	3. process response
*************************/
	if(response != NULL)
	{
		pbData = response->resp_start;
		cbData = response->len;
		pbRevocationBuffer = ( DRM_BYTE * )Oem_MemAlloc( REVOCATION_BUFFER_SIZE );

		dr = Drm_Revocation_SetBuffer( poAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE );
		if(dr != DRM_SUCCESS)
		{
			PrintError("Drm_Revocation_SetBuffer failed! [%x]\n", dr);
		}

		dr = Drm_LicenseAcq_ProcessResponse( poAppContext,
			NULL,
			NULL,
			pbData,
			cbData,
			&oLicenseResponse);
		if(dr == DRM_SUCCESS)
		{
			PrintError("License is Stored well!\n");
		}
		else if (dr == DRM_S_MORE_DATA)
		{
			PrintError("License is Stored well, and some of custom data exist!\n");
		}
		else
		{
			PrintError("Drm_LicenseAcq_ProcessResponse returned 0x%X\n", dr);
			Drm_Uninitialize(poAppContext);
			if(pbRevocationBuffer != NULL)
			{
				Oem_MemFree(pbRevocationBuffer);
			}
			return dr;
		}
	}

/*************************
*	4. Header setting
*************************/
	eHeaderVersion = P_MEDIA_PlayReady_CheckHeaderVersion(pucWrmHeader, uiWrmHeaderSize);
	dr = Drm_Content_SetProperty(poAppContext, eHeaderVersion, pucWrmHeader, uiWrmHeaderSize);
	if(dr != DRM_SUCCESS)
	{
		PrintError("Drm_Content_SetProperty returns 0x%x\n", dr);
		Drm_Uninitialize(poAppContext);
		if(pbRevocationBuffer != NULL)
		{
			Oem_MemFree(pbRevocationBuffer);
		}
		return dr;
	}

/*************************
*	5. Reader Bind
*************************/
	dr = Drm_Reader_Bind (poAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)DummyBindCallback, NULL, &pDecryptContext );
	if(dr != DRM_SUCCESS)
	{
		PrintError("Drm_Reader_Bind returns 0x%x\n", dr);
		Drm_Uninitialize(poAppContext);
		if(pbRevocationBuffer != NULL)
		{
			Oem_MemFree(pbRevocationBuffer);
		}
		return dr;
	}

/*************************
*	6. Commit, License processing all done!
*************************/
	dr = Drm_Reader_Commit(poAppContext, (DRMPFNPOLICYCALLBACK)DummyBindCallback, NULL);
	if(dr != DRM_SUCCESS)
	{
		PrintError("Drm_Reader_Commit returns 0x%x\n", dr);
		Drm_Uninitialize(poAppContext);
		if(pbRevocationBuffer != NULL)
		{
			Oem_MemFree(pbRevocationBuffer);
		}
		return dr;
	}
	Drm_Uninitialize(poAppContext);
	if(pbRevocationBuffer != NULL)
	{
		Oem_MemFree(pbRevocationBuffer);
	}
	return DRM_SUCCESS;
}

void P_MEDIA_CleanupPrdyCaches(void)
{
	VK_SYSTEM_Command(g_SyscmdRemovePRCert);
	VK_SYSTEM_Command(g_SyscmdRemovePDCert);
	VK_SYSTEM_Command(g_SyscmdRemovePrivKeys);
	VK_SYSTEM_Command(g_SyscmdRemoveSecureStore);
}

HINT8* P_MEDIA_AddMaxdomeCookie(HINT8* pUrl)
{
	HINT32 new_string_size = 0;
	HINT8 *man = NULL;
	int nRet;

	if (s_tLicenseDataCallback)
	{
		s_tLicenseDataCallback(pUrl, 0);
		if (s_pcLisenceData == NULL)
		{
			PrintError("s_pcLisenceData is NULL!!\n\n\n");
			return NULL;
		}

		new_string_size = VK_strlen(pUrl) + VK_strlen(s_pcLisenceData) + 1 + 1;

		man = Oem_MemAlloc(new_string_size);
		if(man == NULL)
		{
			PrintError("Error while VK_malloc new url in %s(%d)\n", __FUNCTION__, __LINE__);
			return NULL;
		}

		nRet = VK_snprintf ((char*)man ,new_string_size, "%s&%s", pUrl, s_pcLisenceData);
		if(nRet<0)
		{
			PrintError("can't make new url in %s(%d)\n", __FUNCTION__, __LINE__);
			return NULL;
		}
	}

	return man;
}

DRV_Error P_MEDIA_GetLicense(HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize, DI_license_response* response)
{
	DRM_RESULT dr;
	DRV_Error dResult = DRV_OK;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
    DRM_DWORD   cchUrl = 0, cchChallenge = 0;
	DRM_CHAR *rgchURL = NULL;
    DRM_CHAR  *pszChallenge = NULL;
	unsigned int post_ret=0;
	DI_license_challenge challenge;
	DRM_CHAR* pTicket = NULL;
	DRM_DWORD cchTicket=0;
	DRM_APP_CONTEXT* poAppContext = NULL;
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;
	DRM_VOID *f_pOEMContext = NULL;
	HBOOL bMaxdomeCookie = FALSE;

	VK_MEM_Memset(&challenge, 0, sizeof(DI_license_challenge));

/*************************
*	1. DRM handle check
*************************/
	poAppContext = Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	if(poAppContext == NULL)
	{
		PrintError("Oem_MemAlloc failed!\n");
		return DRV_ERR;
	}

/*************************
*	2. Drm Header check
*************************/
	if(pucWrmHeader == NULL)
	{
		PrintError("pucWrmHeader is NULL!\n");
		Oem_MemFree(poAppContext);
		return DRV_ERR;
	}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		PrintError("Oem_Create failed!\n");
		Oem_MemFree(poAppContext);
		return DRV_ERR;
	}
#endif

/*************************
*	3. Drm session init.
*************************/
	dr = Drm_Initialize(poAppContext, f_pOEMContext, &dstrStoreName);
	if(dr != DRM_SUCCESS)
	{
		if(dr == DRM_E_INVALID_SECURESTORE_PASSWORD
		|| dr == DRM_E_SECURESTORE_CORRUPT
		|| dr == DRM_E_PRIVKEYREADERROR
		|| dr == DRM_E_LICENSENOTFOUND )
		{
			PrintError("Drm_Initialize failed![0x%X]\nCertification is Changed. SecureStore Cleanup.\n", dr);
			P_MEDIA_CleanupPrdyCaches();
		}
		else
		{
			PrintError("Drm_Initialize failed![0x%X]\nSecureStore Cleanup.\n", dr);
			P_MEDIA_CleanupPrdyCaches();
		}
		PrintError("Retrying Drm_Initialize.\n");
		dr = Drm_Initialize(poAppContext, f_pOEMContext, &dstrStoreName);
		if(dr != DRM_SUCCESS)
		{
			PrintError("Drm_Initialize failed![0x%X]\n", dr);
			P_MEDIA_CleanupPrdyCaches();
			Oem_MemFree(poAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
			Oem_Destroy(f_pOEMContext);
#endif
			return DRV_ERR;
		}
	}

/*************************
*	4. License Processing(SecureStore)
*************************/
	dr = P_MEDIA_ProcessLicense(poAppContext, pucWrmHeader, uiWrmHeaderSize, NULL);
	if(dr != DRM_SUCCESS)
	{
		PrintError("License doesn't exist in SecureStore.\nPOST Challenge Mode.\n");
/*************************
*	5. Drm session Reinit.
*************************/
		dr = Drm_Initialize(poAppContext, f_pOEMContext, &dstrStoreName);
		if(dr != DRM_SUCCESS)
		{
			if(dr == DRM_E_INVALID_SECURESTORE_PASSWORD
			|| dr == DRM_E_SECURESTORE_CORRUPT
			|| dr == DRM_E_PRIVKEYREADERROR
			|| dr == DRM_E_LICENSENOTFOUND )
			{
				PrintError("Drm_Initialize failed![0x%X]\nCertification is Changed. SecureStore Cleanup.\n", dr);
				P_MEDIA_CleanupPrdyCaches();
			}
			else
			{
				PrintError("Drm_Initialize failed![0x%X]\nSecureStore Cleanup.\n", dr);
				P_MEDIA_CleanupPrdyCaches();
			}
			PrintError("Retrying Drm_Initialize.\n");
			dr = Drm_Initialize(poAppContext, f_pOEMContext, &dstrStoreName);
			if(dr != DRM_SUCCESS)
			{
				PrintError("Drm_Initialize failed![0x%X]\n", dr);
				P_MEDIA_CleanupPrdyCaches();
				Oem_MemFree(poAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
				Oem_Destroy(f_pOEMContext);
#endif
				return DRV_ERR;
			}
		}
/*************************
*	6. Ticket Check
*************************/
		if(s_pcPlayReadyCustomData == NULL)
		{
			PrintError("CustomData (ex. Ticket) is abscent...\n");
			cchTicket = 0;
			pTicket = NULL;
		}
		else
		{
			cchTicket = VK_strlen(s_pcPlayReadyCustomData);
			pTicket = VK_StrDup(s_pcPlayReadyCustomData);
		}
/*************************
*	7. WrmHeader Setting
*************************/
		eHeaderVersion = P_MEDIA_PlayReady_CheckHeaderVersion(pucWrmHeader, uiWrmHeaderSize);
		dr = Drm_Content_SetProperty(poAppContext, eHeaderVersion, pucWrmHeader, uiWrmHeaderSize);
		if(dr != DRM_SUCCESS)
		{
			PrintError("Drm_Content_SetProperty returns 0x%x\n", dr);
			Drm_Uninitialize(poAppContext);
			if(pTicket != NULL)
			{
				DD_MEM_Free(pTicket);
			}
			Oem_MemFree(poAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
			Oem_Destroy(f_pOEMContext);
#endif
			return DRV_ERR;
		}

/*************************
*	8. Generate challenge
*************************/
		rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
		dr = Drm_LicenseAcq_GenerateChallenge(poAppContext, rgpdstrRights,
			1,
	        NULL,
			pTicket,
			cchTicket,
			NULL,
			&cchUrl,
			NULL,
			0,
			NULL,
			&cchChallenge);
		if(dr == DRM_E_BUFFERTOOSMALL)
		{
			rgchURL = (DRM_CHAR*) Oem_MemAlloc(cchUrl * SIZEOF (DRM_CHAR));
		    pszChallenge = (DRM_CHAR*) Oem_MemAlloc(cchChallenge * SIZEOF(DRM_CHAR));

			dr = Drm_LicenseAcq_GenerateChallenge(poAppContext, rgpdstrRights,
				NO_OF(rgpdstrRights),
				NULL,
				pTicket,
				cchTicket,
		        rgchURL,
				&cchUrl,
				NULL,
				0,
				(DRM_BYTE *)pszChallenge,
		        &cchChallenge);
			if(dr != DRM_SUCCESS)
			{
				PrintError("Drm_LicenseAcq_GenerateChallenge failed![0x%x]\n", dr);
				Drm_Uninitialize(poAppContext);
				Oem_MemFree(poAppContext);
				Oem_MemFree(rgchURL);
				Oem_MemFree(pszChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
				Oem_Destroy(f_pOEMContext);
#endif
				if(pTicket != NULL)
				{
					DD_MEM_Free(pTicket);
				}
				return DRV_ERR;
			}
/*************************
*	Url Overwrite
*************************/
			if(pucLicenseServerUrl != NULL)
			{
				HUINT32 uiServerUrlLen = VK_strlen(pucLicenseServerUrl);

				Oem_MemFree(rgchURL);
				rgchURL = (DRM_CHAR*) Oem_MemAlloc(uiServerUrlLen+1);
				VK_MEM_Memcpy(rgchURL, pucLicenseServerUrl, uiServerUrlLen);
				rgchURL[uiServerUrlLen] = '\0';
			}

			challenge.url = P_MEDIA_AddMaxdomeCookie(rgchURL);
			if(challenge.url == NULL)
			{
				challenge.url = (HINT8 *)rgchURL;
			}
			else
			{
				bMaxdomeCookie = TRUE;
			}

			challenge.len = cchChallenge;
			challenge.data = (HINT8 *)pszChallenge;
		}

/*************************
*	9. POST Challenge (Soap)
*************************/
		if(eHeaderVersion == DRM_CSP_V4_HEADER)
		{
			post_ret = bhttpclient_license_post_soap(
							(char* )challenge.url,
							(char* )challenge.data,
							0,
							2000,
							&response->data, &response->resp_start, &response->len);
			if(post_ret != 0)
			{
				PrintError("post failed! 0x%x\n", post_ret);
				Drm_Uninitialize(poAppContext);
				Oem_MemFree(poAppContext);
				Oem_MemFree(rgchURL);
				Oem_MemFree(pszChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
				Oem_Destroy(f_pOEMContext);
#endif
				if(pTicket != NULL)
				{
					DD_MEM_Free(pTicket);
				}
				return DRV_ERR;
			}
		}
		else
		{
			post_ret = bhttpclient_license_post_default(
							(char* )challenge.url,
							(char* )challenge.data,
							0,
							2000,
							&response->data, &response->resp_start, &response->len);
			if(post_ret != 0)
			{
				PrintError("post failed! 0x%x\n", post_ret);
				Drm_Uninitialize(poAppContext);
				Oem_MemFree(poAppContext);
				Oem_MemFree(rgchURL);
				Oem_MemFree(pszChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
				Oem_Destroy(f_pOEMContext);
#endif
				if(pTicket != NULL)
				{
					DD_MEM_Free(pTicket);
				}
				return DRV_ERR;
			}
		}

		Oem_MemFree(rgchURL);
		Oem_MemFree(pszChallenge);
		if(bMaxdomeCookie == TRUE)
		{
			Oem_MemFree(challenge.url);
		}
		if(pTicket != NULL)
		{
			DD_MEM_Free(pTicket);
		}

/******************************
*	10. License Processing(Response Data)
******************************/
		dr = P_MEDIA_ProcessLicense(poAppContext, pucWrmHeader, uiWrmHeaderSize, response);
		if(dr != DRM_SUCCESS)
		{
			PrintError("P_MEDIA_ProcessLicense failed!\nSecureStore Cleanup.\n");
			P_MEDIA_CleanupPrdyCaches();
			dResult = DRV_ERR;
		}
		if(response->data != NULL)
		{
			BKNI_Free(response->data);
			response->data = NULL;
		}
	}
	Drm_Uninitialize(poAppContext);
	Oem_MemFree(poAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	Oem_Destroy(f_pOEMContext);
#endif
	return dResult;
}

#endif // !defined(CONFIG_PLAYREADY_20)

#if COMMENT
________(){}
#endif

static size_t P_MEDIA_atom_cursor_refill(batom_cursor *cursor)
{
	if (cursor->left==0)
	{
		unsigned pos;
		const batom_vec *vec;

		for(pos=cursor->pos,vec=&cursor->vec[pos];pos < cursor->count;vec++)
		{
			pos++;
			cursor->pos = pos;
			if (vec->len>0)
			{
				cursor->left = vec->len;
				cursor->cursor = vec->base;
				return (size_t)cursor->left;
			}
		}

		cursor->left = BATOM_EOF;
	}

	return 0;
}

size_t P_MEDIA_map_cursor_to_dmablks(batom_cursor *cursor, size_t count, NEXUS_DmaJobBlockSettings *blks, uint32_t nb_blks_avail, uint32_t *nb_blks_used)
{
    size_t left;
    uint32_t ii = 0;

    for(left=count; ii < nb_blks_avail; ) {
        const uint8_t *src=cursor->cursor;
        int src_left = cursor->left;

        if(src_left>=(int)left)
		{
            cursor->cursor = src+left;
            cursor->left = src_left-left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = left;

            left = 0;
            ii++;
            break;
        }
        if(src_left>=0)
		{
            cursor->cursor = src+src_left;
            cursor->left = 0;
            left -= src_left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = src_left;
            ii++;

            if (P_MEDIA_atom_cursor_refill(cursor)==0)
			{
                break;
            }
        } else {
            break;
        }
    }
    (*nb_blks_used) = ii;
	return count-left;
}

void P_MEDIA_DoDecryptSample(void* pHSession, void* sample_enc_box, unsigned int sample_no, void* pucData, unsigned int uiSize, unsigned char *pIV)
{
	bmp4_ext_sample_encryption_box* sample_encryption_box = (bmp4_ext_sample_encryption_box*)sample_enc_box;
	batom_cursor* data = (batom_cursor*)pucData;
	batom_checkpoint start;
	DRM_CIPHER_CONTEXT *cipher_ctx;
	bmp4_ext_sample_encryption* pSample;
	HUINT8* pEkl=NULL;
	NEXUS_DmaJobBlockSettings stBlkSettings[100];
	unsigned jj=0;
	size_t bytes_ref;
	uint32_t nb_Blks=1;
	uint32_t nb_blks_used=0;
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_Error nResult;
	NEXUS_DmaJobHandle dmaJob = NULL;
	NEXUS_DmaJobStatus jobStatus;
	pHSession = NULL;
	unsigned long				vkEvtId = 0;
	unsigned long				vkEvt;
	int							vkRet;

/*************************
*	1. NULL Check
*************************/
	if((sample_encryption_box == NULL) || (data == NULL) || (pIV == NULL))
	{
		PrintError("Invalid Parameter!\n");
		goto done;
	}

	NEXUS_Memory_Allocate(32, NULL, (void**)&pEkl);
	if(pEkl == NULL)
	{
		PrintError("Out of Memory!\n");
		goto done;
	}

	batom_cursor_save(data, &start);

/*************************
*	2. get key & iv
*************************/
	cipher_ctx = (DRM_CIPHER_CONTEXT* )pDecryptContext.rgbBuffer;
	pSample = &sample_encryption_box->samples[sample_no];
#if defined(CONFIG_PLAYREADY_20)
	VK_MEM_Memcpy(pEkl,   cipher_ctx->cipher.cipherAES.rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	VK_MEM_Memcpy(pEkl+8, cipher_ctx->cipher.cipherAES.rgbContentKey, 8);
#else
	VK_MEM_Memcpy(pEkl,   cipher_ctx->rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	VK_MEM_Memcpy(pEkl+8, cipher_ctx->rgbContentKey, 8);
#endif
	VK_MEM_Memcpy(&pEkl[16], pSample->InitializationVector_128, 16);
	VK_MEM_Memset(&pEkl[16], 0, 8);

	NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
	stBlkSettings[0].pSrcAddr  = pEkl;
	stBlkSettings[0].pDestAddr = pEkl;
	stBlkSettings[0].blockSize = 32;
	stBlkSettings[0].resetCrypto			  = true;
	stBlkSettings[0].scatterGatherCryptoStart = true;

	NEXUS_FlushCache(stBlkSettings[0].pSrcAddr, 32);

/*************************
*	3. dma setting
*************************/
	if(sample_encryption_box->fullbox.flags &= 0x000002)
	{
		for(jj = 0; jj < pSample->NumberOfEntries; jj++)
		{
			batom_cursor_skip((batom_cursor *)data, pSample->entries[jj].BytesOfClearData);
			bytes_ref = P_MEDIA_map_cursor_to_dmablks((batom_cursor *)data, pSample->entries[jj].BytesOfEncryptedData, &stBlkSettings[nb_Blks], 99, &nb_blks_used);
			if(bytes_ref != pSample->entries[jj].BytesOfEncryptedData)
			{
				PrintError("%s - out of DMA blks, return without decrypting\n", __FUNCTION__);
				goto done;
			}
			nb_Blks += nb_blks_used;
		}
		stBlkSettings[nb_Blks-1].scatterGatherCryptoEnd = true;
	}
	else
	{
		{
			bytes_ref = P_MEDIA_map_cursor_to_dmablks((batom_cursor *)data, uiSize, &stBlkSettings[1], 99, &nb_blks_used);
			if(bytes_ref != uiSize)
			{
				PrintError("%s - out of DMA blks, return without decrypting\n", __FUNCTION__);
				goto done;
			}
		}
		nb_Blks += nb_blks_used;
		stBlkSettings[nb_Blks-1].scatterGatherCryptoEnd = true;
	}


/*************************
*	4. decrypt
*************************/
	vkRet = VK_EVENT_Create(&vkEvtId, "MEDIA_DMA_EVT");
	if (VK_OK != vkRet)
	{
		goto done_cpy;
	}

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks					= nb_Blks;
	jobSettings.keySlot 					= keyPlayready;
	jobSettings.dataFormat					= NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = P_MEDIA_DMA_CompletedCallback;
	jobSettings.completionCallback.context = (void*)vkEvtId;

	dmaJob = NEXUS_DmaJob_Create(dmaHandle, &jobSettings);
	if(dmaJob == NULL)
	{
		PrintError("%s - NEXUS_DmaJob_Create failed\n", __FUNCTION__);
		goto done_cpy;
	}

	nResult = NEXUS_DmaJob_ProcessBlocks(dmaJob, (NEXUS_DmaJobBlockSettings *)stBlkSettings, nb_Blks);
	vkRet = VK_EVENT_ReceiveTimeout(vkEvtId, &vkEvt, 200);
	if (VK_OK != vkRet)
	{
		PrintError("DMA transfer failed\n");
		goto done_cpy;
	}

done_cpy:

	batom_cursor_rollback(data, &start);

done:

	if(dmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy(dmaJob);
	}

	if(pEkl != NULL)
	{
		NEXUS_Memory_Free(pEkl);
	}

	if (vkEvtId != 0)
	{
		(void)VK_EVENT_Destroy(vkEvtId);
	}

	return;
}

void P_MEDIA_DoDecryptAESCtr(void* pHSession, HUINT8* pucData, HUINT32 ulSize, HUINT8* pIV)
{
	DRM_CIPHER_CONTEXT *cipher;
	HUINT8* pEkl=NULL;
	HUINT32 ekl_size;
	NEXUS_Error nResult;
	NEXUS_DmaJobBlockSettings stBlkSettings[2];
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob = NULL;
	NEXUS_DmaJobStatus jobStatus;
	pHSession = NULL;
	unsigned long				vkEvtId = 0;
	unsigned long				vkEvt;
	int							vkRet;


/*************************
*	1. NULL Check
*************************/
	if(pucData == NULL)
	{
		PrintError("Buffer is NULL!\n");
		goto done;
	}

/*************************
*	2. AES Counter decrypt
*************************/
	cipher = (DRM_CIPHER_CONTEXT* )pDecryptContext.rgbBuffer;

	NEXUS_Memory_Allocate(32, NULL, (void**)&pEkl);
	if(pEkl == NULL)
	{
		PrintError("Out of Memory!\n");
		goto done;
	}
#if defined(CONFIG_PLAYREADY_20)
	VK_MEM_Memcpy(pEkl,   cipher->cipher.cipherAES.rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	VK_MEM_Memcpy(pEkl+8, cipher->cipher.cipherAES.rgbContentKey, 8);
#else
	VK_MEM_Memcpy(pEkl,   cipher->rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	VK_MEM_Memcpy(pEkl+8, cipher->rgbContentKey, 8);
#endif
	VK_MEM_Memset(&pEkl[16], 0, 8);
	VK_MEM_Memcpy(&pEkl[24], pIV, 8);

	ekl_size = 32;

	NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
	stBlkSettings[0].pSrcAddr                 = pEkl;
	stBlkSettings[0].pDestAddr                = pEkl;
	stBlkSettings[0].blockSize                = ekl_size;
	stBlkSettings[0].cached                   = true;
	stBlkSettings[0].resetCrypto              = true;
	stBlkSettings[0].scatterGatherCryptoStart = true;
	stBlkSettings[0].scatterGatherCryptoEnd   = false;

	NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[1]);
	stBlkSettings[1].pSrcAddr                 = pucData;
	stBlkSettings[1].pDestAddr                = pucData;
	stBlkSettings[1].blockSize                = ulSize;
	stBlkSettings[1].cached                   = true;
	stBlkSettings[1].resetCrypto              = true;
	stBlkSettings[1].scatterGatherCryptoStart = false;
	stBlkSettings[1].scatterGatherCryptoEnd   = true;

/*************************
*	4. DmaJob setting
*************************/
	vkRet = VK_EVENT_Create(&vkEvtId, "MEDIA_DMA_EVT");
	if (VK_OK != vkRet)
	{
		goto done;
	}

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks                   = 2;
	jobSettings.keySlot                     = keyPlayready;
	jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = P_MEDIA_DMA_CompletedCallback;
	jobSettings.completionCallback.context = (void*)vkEvtId;

	dmaJob = NEXUS_DmaJob_Create(dmaHandle, &jobSettings);
	if(dmaJob == NULL){
	    PrintError("NEXUS_DmaJob_Create failed!\n");
		goto done;
	}

/*************************
*	5. Dma transfer
*************************/
	nResult = NEXUS_DmaJob_ProcessBlocks(dmaJob, (NEXUS_DmaJobBlockSettings *)&stBlkSettings, 2);

	vkRet = VK_EVENT_ReceiveTimeout(vkEvtId, &vkEvt, 200);
	if (VK_OK != vkRet)
	{
		PrintError("DMA transfer failed\n");
		goto done;
	}

done:
	if(dmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy(dmaJob);
	}

	if(pEkl != NULL)
	{
		NEXUS_Memory_Free(pEkl);
	}

	if (vkEvtId != 0)
	{
		(void)VK_EVENT_Destroy(vkEvtId);
	}

	return;
}

void P_MEDIA_DoDecryptRC4_sw(void* pHSession, unsigned char* pucData, unsigned int uiSize)
{
	DRM_BYTE *last_15_bytes;
    DRM_RESULT dr;
	DRM_CIPHER_CONTEXT *cipher;
	pHSession = NULL;

/*************************
*	1. NULL Check
*************************/
	if(pucData == NULL)
	{
		PrintError("Buffer is NULL!\n");
		return;
	}

/*************************
*	2. Cocktail decrypt
*************************/
	cipher = (DRM_CIPHER_CONTEXT* )pDecryptContext.rgbBuffer;
	if (uiSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	}

	dr = Drm_Reader_InitDecrypt( &pDecryptContext, last_15_bytes, uiSize);
	if (dr == DRM_SUCCESS)
	{
		dr = Drm_Reader_Decrypt( &pDecryptContext, NULL, pucData, uiSize);
	}
	else
	{
		PrintError("InitDecrypt failed = 0x%x\n", dr);
	}
	return;
}

void P_MEDIA_DoDecryptRC4_hw(void* pHSession, unsigned char* pucData, unsigned int uiSize)
{
	DRM_CIPHER_CONTEXT *cipher;
	HUINT8* pEkl=NULL;
	DRM_BYTE *last_15_bytes;
    HUINT32 cbClear=0;
	NEXUS_Error nResult;
    NEXUS_DmaJobBlockSettings stBlkSettings[2];
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob = NULL;
	NEXUS_DmaJobStatus jobStatus;
	pHSession = NULL;
	unsigned long				vkEvtId = 0;
	unsigned long				vkEvt;
	int							vkRet;

/*************************
*	1. NULL Check
*************************/
	if(pucData == NULL)
	{
		PrintError("Buffer is NULL!\n");
		goto done;
	}

	if(uiSize < 16)
	{
		P_MEDIA_DoDecryptRC4_sw(pHSession, pucData, uiSize);
		goto done;
	}

	cipher = (DRM_CIPHER_CONTEXT* )pDecryptContext.rgbBuffer;
	NEXUS_Memory_Allocate(80, NULL, (void**)&pEkl);
	if(pEkl == NULL)
	{
		goto done;
	}

/*************************
*	2. EKL setting
*************************/

	if (uiSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	}

	/*8 byte DES key*/
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.shaOut[12]),8);
	pEkl+=8;

	/*8 byte RC4 content key (the one at the end of of the ASF paylaod */
	cbClear = uiSize % 8;	/*Find Number of clear bytes*/
	VK_MEM_Memcpy(pEkl, &(last_15_bytes[15-8-cbClear]),8);
	pEkl+=8;

    /*8 byte D1 */
    /*D1 = "9.	From the 64 bytes obtained in step 4, use the last 8 bytes to XOR with the result of the DES encryption in step 8."*/
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.desS2[0]),8);
	pEkl+=8;

    /*8 byte D0 */
    /*D0 = "From the 64 bytes obtained in step 4, the 8 bytes following the 48 bytes MAC key are used to XOR the RC4 content key obtained in step 5."*/
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.desS1[0]),8);
	pEkl+=8;

    /*48 byte MAC key */
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.a1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.b1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.c1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.d1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.e1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.f1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.a2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.b2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.c2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.d2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.e2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.f2), 4);
	pEkl-=76;

/*************************
*	3. Decrypt
*************************/
    NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
    stBlkSettings[0].pSrcAddr                 = pEkl;
    stBlkSettings[0].pDestAddr                = pEkl;
    stBlkSettings[0].blockSize                = 80;
    stBlkSettings[0].cached                   = true;
    stBlkSettings[0].resetCrypto              = true;
    stBlkSettings[0].scatterGatherCryptoStart = true;
    stBlkSettings[0].scatterGatherCryptoEnd   = false;

    NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[1]);
    stBlkSettings[1].pSrcAddr                 = pucData;
    stBlkSettings[1].pDestAddr                = pucData;
    stBlkSettings[1].blockSize                = uiSize;
    stBlkSettings[1].cached                   = true;
    stBlkSettings[1].resetCrypto              = true;
    stBlkSettings[1].scatterGatherCryptoStart = false;
    stBlkSettings[1].scatterGatherCryptoEnd   = true;

/*************************
*	4. DmaJob setting
*************************/
	vkRet = VK_EVENT_Create(&vkEvtId, "MEDIA_DMA_EVT");
	if (VK_OK != vkRet)
	{
		goto done;
	}

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks                   = 2;
    jobSettings.keySlot                     = keyWmdrm;
    jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = P_MEDIA_DMA_CompletedCallback;
	jobSettings.completionCallback.context = (void*)vkEvtId;

    dmaJob = NEXUS_DmaJob_Create(dmaHandle, &jobSettings);
    if(dmaJob == NULL){
        PrintError("NEXUS_DmaJob_Create failed!\n");
		goto done;
    }

/*************************
*	5. Dma transfer
*************************/
	nResult = NEXUS_DmaJob_ProcessBlocks(dmaJob, (NEXUS_DmaJobBlockSettings *)&stBlkSettings, 2);
	vkRet = VK_EVENT_ReceiveTimeout(vkEvtId, &vkEvt, 200);
	if (VK_OK != vkRet)
	{
		PrintError("DMA transfer failed\n");
		goto done;
	}

done:
	if(dmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy(dmaJob);
	}

	if(pEkl != NULL)
	{
		NEXUS_Memory_Free(pEkl);
	}

	if (vkEvtId != 0)
	{
		(void)VK_EVENT_Destroy(vkEvtId);
	}

	return;
}

#endif // defined(CONFIG_PLAYREADY)

#if COMMENT
________(){}
#endif

DI_ERR_CODE P_MEDIA_DRM_ParseWebInitiator(HUINT8* msg, HUINT32* puiUrlOffset, HUINT32* puiUrlLength)
{
	HUINT32 uiIndex=0;
	HUINT32 uiMsgLength=0;
	HUINT32 uiTagStartOffset = 0;
	HBOOL bTagStart = FALSE;
	HBOOL bURLFound = FALSE;

	if((msg == NULL) || (puiUrlOffset == NULL) || (puiUrlLength == NULL))
	{
		PrintError("invalid parameter!\n");
		return DI_ERR_ERROR;
	}

	uiMsgLength = VK_strlen(msg);

/************************
*	<LA_URL>
************************/
	while(uiIndex < uiMsgLength)
	{
		if(msg[uiIndex] == '<')
		{
			bTagStart = TRUE;
			uiTagStartOffset = uiIndex;
		}
		else if(msg[uiIndex] == '>')
		{
			if(bTagStart != TRUE)
			{
				PrintError("Invalid syntax!\n");
				return DI_ERR_ERROR;
			}
			else
			{
				if((msg[uiTagStartOffset+1] == 'L')	&& (msg[uiTagStartOffset+2] == 'A') && (msg[uiTagStartOffset+3] == '_') &&
					(msg[uiTagStartOffset+4] == 'U') && (msg[uiTagStartOffset+5] == 'R') && (msg[uiTagStartOffset+6] == 'L'))
				{
					*puiUrlOffset = uiIndex+1;
				}
				else if((msg[uiTagStartOffset+1] == '/')	&& (msg[uiTagStartOffset+2] == 'L') && (msg[uiTagStartOffset+3] == 'A') && (msg[uiTagStartOffset+4] == '_') &&
					(msg[uiTagStartOffset+5] == 'U') && (msg[uiTagStartOffset+6] == 'R') && (msg[uiTagStartOffset+7] == 'L'))
				{
					bURLFound = TRUE;
					*puiUrlLength = uiTagStartOffset - *puiUrlOffset;
					break;
				}
			}
			bTagStart = FALSE;
		}
		uiIndex++;
	}

	if(bURLFound == FALSE)
	{
		PrintError("url not found!\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE P_MEDIA_DRM_SetLicenseUrl(HUINT8* pucUrl, HUINT32 uiUrlLength)
{
	if(pucLicenseServerUrl != NULL)
	{
		VK_MEM_Free(pucLicenseServerUrl);
		pucLicenseServerUrl = NULL;
	}

	pucLicenseServerUrl = VK_MEM_Alloc(uiUrlLength+1);
	if(pucLicenseServerUrl == NULL)
	{
		PrintError("Out of memory!\n");
		return DI_ERR_ERROR;
	}

	VK_MEM_Memcpy(pucLicenseServerUrl, pucUrl, uiUrlLength);
	pucLicenseServerUrl[uiUrlLength] = '\0';
	return DI_ERR_OK;
}

/* b_play_getpts() is defined at nexus_playback_impl.h() with private internal function */
extern NEXUS_Error b_play_getpts(NEXUS_PlaybackHandle p, uint32_t *pts);

#if defined (USE_DRM)
static DI_ERR_CODE   P_MEDIA_DRM_Open (HUINT8 *pszStreamUrl, DI_MediaInstance *pMediaInstance);
static DI_ERR_CODE   P_MEDIA_DRM_Close (DI_MediaInstance *pMediaInstance);

/* from playengine.cpp in brutus =========================================== */
/* bdrm_util_license_extern */
bdrm_err
P_MEDIA_DRM_bdrm_util_license_extern (const bdrm_license_challenge* lic_challenge,
		bdrm_license_response* lic_response,
		const void * context);

/* bdrm_util_license_callback */
bdrm_err
P_MEDIA_DRM_bdrm_util_license_callback (bdrm_acq_state* lic_acq_state,
		bdrm_license_t license,
		void * context);
/* ========================================================================= */

static bdrm_license_state P_MEDIA_DRM_GetLicenseState (DI_MediaInstance *pMediaInstance);
#if 0
static bdrm_http_state P_MEDIA_DRM_GetHTTPState (void);
#endif
static void P_MEDIA_DRM_SetLicenseState(DI_MediaInstance *pMediaInstance,bdrm_license_state state);
static void P_MEDIA_DRM_SetHTTPState(DI_MediaInstance *pMediaInstance, bdrm_http_state state);

#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)

//#define	CMD_DEBUG

#if defined(CMD_DEBUG)
static DRM_RESULT P_MEDIA_DRM_ProcessResponse( DRM_CONST_STRING *pwszResponseFilename )
{
	DRM_RESULT dr = DRM_SUCCESS;
	DRM_DWORD dw;
	DRM_MANAGER_CONTEXT *poManagerContext = NULL;
	DRM_BYTE* pbData = NULL;
	DRM_DWORD iIO=0;
	DRM_CONST_STRING dsrDataStoreFile;
	FILE *fpResponse=NULL;


	ChkMem( poManagerContext = OEM_malloc( SIZEOF(DRM_MANAGER_CONTEXT) ) );
	ZEROMEM(poManagerContext, SIZEOF(DRM_MANAGER_CONTEXT));

	wcsncpy( (DRM_WCHAR*)poManagerContext, pwszResponseFilename->pwszString, pwszResponseFilename->cchString );
	((DRM_WCHAR*)poManagerContext)[pwszResponseFilename->cchString] = g_wchNull;

	fpResponse = fopen(pwszResponseFilename->pwszString, "rb");
	if(fpResponse== NULL)
	{
		PrintError("Error P_MEDIA_DRM_ProcessResponse %s\n", pwszResponseFilename->pwszString);
		goto ErrorExit;

	}

	PrintDebug("Before OEM_GetFileSize\n");
	OEM_GetFileSize(fpResponse, &dw);

	OEM_SetFilePointer(fpResponse, 0, OEM_FILE_BEGIN, NULL);
	ChkMem( pbData = (DRM_BYTE*) OEM_malloc( dw ) );

	PrintDebug("Before OEM_ReadFile\n");
	if (!OEM_ReadFile(fpResponse, pbData, dw, &iIO) || iIO!=dw)
	{
	    ChkDR(DRM_E_FILEREADERROR);
	}

	bdrm_env_datastore(&dsrDataStoreFile);
	PrintDebug("Before DRM_MGR_Initialize\n");
	ChkDR (DRM_MGR_Initialize(poManagerContext, &dsrDataStoreFile));

	PrintDebug("Before DRM_MGR_ProcessLicenseResponse\n");
	ChkDR( DRM_MGR_ProcessLicenseResponse( poManagerContext, NULL, NULL, pbData, dw ) );


	ErrorExit:
	PrintError("hit ErrorExit\n");
	DRM_MGR_Uninitialize(poManagerContext);

	if (fpResponse != NULL)
	{
	    OEM_CloseFile (fpResponse);
	}

	if (pbData != NULL)
	{
	    OEM_free (pbData);
	}

	if (poManagerContext != NULL)
	{
	    OEM_free (poManagerContext);
	}

	return dr;

}
#else
static DRM_RESULT P_MEDIA_DRM_ProcessResponse( HINT8 *pwszResponseData )
{
	DRM_RESULT dr = DRM_SUCCESS;
	DRM_DWORD dw;
	DRM_MANAGER_CONTEXT *poManagerContext = NULL;
	DRM_CONST_STRING dsrDataStoreFile;

	ChkMem( poManagerContext = OEM_malloc( SIZEOF(DRM_MANAGER_CONTEXT) ) );
	ZEROMEM(poManagerContext, SIZEOF(DRM_MANAGER_CONTEXT));

	dw = VK_strlen(pwszResponseData);

	bdrm_env_datastore(&dsrDataStoreFile);


	PrintDebug("Before DRM_MGR_Initialize\n");
	ChkDR (DRM_MGR_Initialize(poManagerContext, &dsrDataStoreFile));

	ChkDR(DRM_MGR_CleanupLicenseStore( poManagerContext,   NULL,   5,   NULL));

	PrintDebug("Before DRM_MGR_ProcessLicenseResponse\n");
	ChkDR( DRM_MGR_ProcessLicenseResponse( poManagerContext, NULL, NULL, (DRM_BYTE *)pwszResponseData, dw ) );

ErrorExit:

	DRM_MGR_Uninitialize(poManagerContext);
	if (poManagerContext != NULL)
	{
	    OEM_free (poManagerContext);
	}

	return dr;
}
#endif

#if 0
static DI_ERR_CODE P_MEDIA_DRM_PreDeliveryLicense_Open(HUINT8 *pszStreamUrl,DI_MediaInstance *pMediaInstance)
{

   	DRM_RESULT dr = DRM_SUCCESS;
#if 0 /* ÇöÀç´Â »ç¿ëµÇÁö ¾Ê´Â ºÎºÐÀ¸·Î Â÷ÈÄ¿¡ »ç¿ë °¡´É */
	const DRM_CONST_STRING    *apdcsRights [1];
	DRM_MANAGER_CONTEXT *poManagerContext = NULL;
	DRM_MANAGER_DECRYPT_CONTEXT oDecryptContext;
	const DRM_CONST_STRING *proparray[] = { &g_dstrWMDRM_RIGHT_PLAYBACK };
	DRM_LICENSE_STATE_DATA statedatavales[NO_OF(proparray)];
	DRM_DWORD cbXML   = 0;
	DRM_BYTE *pbXML   = NULL;
	OEM_FILEHDL Infp  = OEM_INVALID_HANDLE_VALUE;
	DRM_DWORD cbRead  = 0;
	DRM_CONST_STRING pdsrDataStoreFile;

	VK_MEM_Memset(&pdsrDataStoreFile, 0x0, sizeof(pdsrDataStoreFile));

	/* Â÷ÈÄ¿¡ »ç¿ë½Ã pdsrDataStoreFileÀÇ °ªÀ» ¹Þ¾Æ¿À´Â ºÎºÐÀÌ ÇÊ¿äÇÔ */
	Infp = OEM_OpenFile (pdsrDataStoreFile.pwszString,
                     OEM_GENERIC_READ,
                     OEM_FILE_SHARE_READ,
                     OEM_OPEN_EXISTING,
                     OEM_ATTRIBUTE_NORMAL);

	if (OEM_INVALID_HANDLE_VALUE == Infp)
	{
		PrintError( "Input file not opened =========>%s\n", pszStreamUrl);
		dr = DRM_S_FALSE;
		goto ErrorExit;
	}

	//TODO : ASF Header Object parsing
	cbXML = 5 * 1024;			//DRM HEAD MAX Size

	ChkMem (pbXML   = OEM_malloc (cbXML));

	if (OEM_ReadFile(Infp, pbXML, cbXML, &cbRead) == FALSE
	||  cbRead != cbXML)
	{
		PrintError( "Header couldn't be read\n");
		dr = DRM_S_FALSE;
		goto ErrorExit;
	}

	ChkMem (poManagerContext = OEM_malloc( SIZEOF(DRM_MANAGER_CONTEXT) ) );
	ZEROMEM(poManagerContext, SIZEOF (DRM_MANAGER_CONTEXT));

	bdrm_env_datastore(&pdsrDataStoreFile);                        /* read the datastore file from the env. */
	ChkDR (DRM_MGR_Initialize  (poManagerContext, &pdsrDataStoreFile));
	ChkDR (DRM_MGR_SetV2Header (poManagerContext, pbXML, cbXML));
	ChkDR( DRM_MGR_GetLicenseData( poManagerContext, proparray, statedatavales, NO_OF( proparray ) ) );

	apdcsRights [0] = (DRM_CONST_STRING *) &g_dstrWMDRM_RIGHT_PLAYBACK;

    /*
    ** Decrypting the content key and calling DRM_CPHR_Init() will be done
    ** by DRM_MGR_Bind(). The bound DRM_CIPHER_CONTEXT will contain the
    ** initialized structs we need for DRM_CPHR_InitDecrypt() below.
    ** DRM_MGR_Bind() casts DRM_MANAGER_DECRYPT_CONTEXT to DRM_CIPHER_CONTEXT,
    ** so that's where the initialized structs wind up. We'll make our own
    ** copy for our use, since we're running outside the BlackBox module.
    */

	if (DRM_MGR_Bind (poManagerContext, apdcsRights,  NO_OF (apdcsRights),  NULL, NULL, &oDecryptContext ) == DRM_E_LICENSENOTFOUND)
	{
		PrintError( "No licenses found\n");
		dr = DRM_S_FALSE;
		goto ErrorExit;
	}

	ChkDR( DRM_MGR_Commit( poManagerContext ) );                    /* commit license changes to the store */

//	VK_memcpy(pMediaInstance->tDrm->poDecryptCtx, &oDecryptContext,sizeof(DRM_MANAGER_DECRYPT_CONTEXT));
//	bdrm_decryptor_assign(pMediaInstance->tDcryptor,pMediaInstance->tDrm);
       VK_memcpy(&pMediaInstance->aucEncryptionKey, &oDecryptContext , sizeof(DRM_MANAGER_DECRYPT_CONTEXT));


	ErrorExit:

	if (Infp != OEM_INVALID_HANDLE_VALUE)
	{
		OEM_CloseFile (Infp);
	}
#if 0
	if (pbChunk != NULL)
	{
		OEM_free (pbChunk);
	}
#endif
	if (pbXML != NULL)
	{
		OEM_free (pbXML);
	}

	if (poManagerContext != NULL)
	{
		DRM_MGR_Uninitialize (poManagerContext);
		OEM_free (poManagerContext);
	}
#else
	UNUSED_PARAMETER(pszStreamUrl);
	UNUSED_PARAMETER(pMediaInstance);
#endif

	return	(DI_ERR_CODE)dr;
}
#endif

#endif

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_Open
 * Function Description:
 * Parameter:
 * Return Value:
 */

static DI_ERR_CODE   P_MEDIA_DRM_Open (HUINT8 *pszStreamUrl, DI_MediaInstance *pMediaInstance)
{
    DI_ERR_CODE  nRet = DI_ERR_OK;
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
	DRM_CONST_STRING dsrDataStoreFile ;
#endif

	PrintEnter;

#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
	pMediaInstance->tDrm_cfg.lic_extern = NULL;
	pMediaInstance->tDrm_cfg.lic_extern_ctx = (void*)pMediaInstance;
	bdrm_env_datastore(&dsrDataStoreFile);
	pMediaInstance->tDrm_cfg.licstore = (char *)dsrDataStoreFile.pwszString;
	pMediaInstance->tDrm_cfg.ext_asf_scanning = 0;
//	pMediaInstance->tDrm_cfg.opl_cback = bdrm_opl_callback;
//	pMediaInstance->tDrm_cfg.opl_ctx = (void *)pMediaInstance->tDrm;
#else
    pMediaInstance->tDrm_cfg.lic_extern = P_MEDIA_DRM_bdrm_util_license_extern;
    pMediaInstance->tDrm_cfg.lic_extern_ctx = (void*)pMediaInstance;
    pMediaInstance->tDrm_cfg.ext_asf_scanning = 0;
#endif

    if ((pMediaInstance->tDrm = bdrm_open((char*)pszStreamUrl, &pMediaInstance->tDrm_cfg)) != NULL)
    {
        /* set up drm callbacks and contexts */
        pMediaInstance->tDrm_cfg.lic_callback     = P_MEDIA_DRM_bdrm_util_license_callback;
        pMediaInstance->tDrm_cfg.lic_callback_ctx = (void*)pMediaInstance;
        bdrm_init_handlers(pMediaInstance->tDrm);

        if ( (bdrm_lic_failtransfer == P_MEDIA_DRM_GetLicenseState(pMediaInstance)) ||
        		(bdrm_lic_faillocal == P_MEDIA_DRM_GetLicenseState(pMediaInstance)) ||
        		(bdrm_lic_pending == P_MEDIA_DRM_GetLicenseState(pMediaInstance)) ||
        		(bdrm_lic_expired == P_MEDIA_DRM_GetLicenseState(pMediaInstance)))
        {
            if ( NULL != pMediaInstance->tDrm )
            {
                (void)bdrm_close (pMediaInstance->tDrm);
                pMediaInstance->tDrm = NULL;
                pMediaInstance->tDcryptor = NULL;
            }
            /* else - do nothing */

            PrintError("%s(%d) Error - bdrm_lic_failtransfer \n",__FUNCTION__,__LINE__);
            nRet = DI_ERR_ERROR;
        }
        else
        {
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD) && defined(CONFIG_DEBUG)
			unsigned int i;
#endif
            bdrm_dcrypt_instance(pMediaInstance->tDrm, &pMediaInstance->tDcryptor);
            VK_memcpy(&pMediaInstance->aucEncryptionKey, &pMediaInstance->tDcryptor, sizeof(&pMediaInstance->tDcryptor));
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD) && defined(CONFIG_DEBUG)
		    PrintError("aucEncryptionKey \n");
	    for(i=0; i < 	DI_MEDIA_ENCRYPTION_KEY_LEN; i++)
			PrintError("%x ",pMediaInstance->aucEncryptionKey[i]);
		    PrintError(" \n");
#endif
        }
    }
    else /* Drm handle Open Fail½Ã FailÀÌ ¾Æ´Ñ Á¤»óÀûÀÎ °úÁ¤À¸·Î ÁøÀÔÇÏ¿© Segmentation Fault°¡ ¹ß»ýÇÏ´Â ÀÌ½´ Fatch */
    {
    	nRet = DI_ERR_ERROR;
    }

	PrintExit;
    return nRet;
}
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_Close
 * Function Description:
 * Parameter:
 * Return Value:
 */
static DI_ERR_CODE   P_MEDIA_DRM_Close (DI_MediaInstance *pMediaInstance)
{
    DI_ERR_CODE  nRet = DI_ERR_OK;

	PrintEnter;

    if ( NULL != pMediaInstance->tDrm )
    {
        (void)bdrm_close (pMediaInstance->tDrm);
        pMediaInstance->tDrm = NULL;
        pMediaInstance->tDcryptor = NULL;
    }
    /* else - do nothing */
	PrintExit;

    return nRet;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_bdrm_util_license_extern
 * Function Description:
 * Parameter:
 * Return Value:
 */
bdrm_err
P_MEDIA_DRM_bdrm_util_license_extern (const bdrm_license_challenge* lic_challenge,
                          bdrm_license_response* lic_response,
                          const void * context)
{
	bdrm_err ret = bdrm_err_fail;
	int32_t post_ret = 0;

	FILE *fd = NULL;
	HINT32 new_string_size = 0;
	char *start_of_tok = NULL;
	char *line_buffer = NULL;
	char *man = NULL;
	char *savepoint = NULL;
	int nRet;
#if 0
	char *manuser = NULL;
#endif
	PrintEnter;

    if (lic_challenge == NULL)
    {
    	PrintDebug("lic_challenge is NULL %s(%d)\n", __FUNCTION__, __LINE__);
    	goto failed;
    }

    PrintDebug ("%s(%d) extern grabber: challenge len=(%d) url=%s data=%c\n",
                      __FUNCTION__,__LINE__,lic_challenge->len, lic_challenge->url, lic_challenge->data[0]);

	if ( s_tLicenseDataCallback )
	{
		/* call the license callback with the channllenge URL information and then
		 * the application will set the license data */
		/* Fixed warning : typedef void (*DI_MEDIA_LicenseDataCallback_t)(HINT8 *pParam1, HUINT32 unParam2); */
		/*               : char* url; */
		s_tLicenseDataCallback((HINT8 *)lic_challenge->url, 0);

		if ( s_pcLisenceData == NULL )
		{
			goto read_cookie_error;
		}

		new_string_size = VK_strlen(lic_challenge->url) + VK_strlen(s_pcLisenceData) + 1 + 1;

		man = DD_MEM_Alloc(new_string_size);
		if(man == NULL)
		{
			PrintDebug("Error while VK_malloc new url in %s(%d)\n", __FUNCTION__, __LINE__);
			goto read_cookie_error;
		}

		nRet = VK_snprintf (man ,new_string_size, "%s&%s", lic_challenge->url, s_pcLisenceData);
		if(nRet<0)
		{
			PrintDebug("can't make new url in %s(%d)\n", __FUNCTION__, __LINE__);
			goto read_cookie_error;
		}
	}
	else
	{
    /* open cookie */
	fd = fopen(HUMAX_COOKIE_FILE_FULL_PATH, "r");
	if(fd == NULL)
	{
		PrintDebug("Error while open cookie.txt file in %s\n", HUMAX_COOKIE_FILE_FULL_PATH);
		goto license_request;
	}

	/* read cookie */
	line_buffer = DD_MEM_Alloc(HUMAX_COOKIE_LINE_BUFFER_SIZE);
	if(line_buffer == NULL)
	{
		PrintDebug("Error while VK_malloc read_line_buffer for cookie.txt file in %s\n", HUMAX_COOKIE_FILE_FULL_PATH);
		goto read_cookie_error;
	}
	VK_memset(line_buffer, 0, HUMAX_COOKIE_LINE_BUFFER_SIZE);
	while(fgets(line_buffer, HUMAX_COOKIE_LINE_BUFFER_SIZE,fd))
	{
		if(VK_memcmp(line_buffer, MAXDOME_MAN_STRING, VK_strlen(MAXDOME_MAN_STRING)) == 0)
		{
			//start_of_tok = strtok(line_buffer, HUMAX_COOKIE_SEP); /* strtok replaces sep with NULL */
			//start_of_tok = strtok(NULL, HUMAX_COOKIE_SEP);
			start_of_tok = strtok_r(line_buffer, HUMAX_COOKIE_SEP,&savepoint); /* strtok replaces sep with NULL */
			if(start_of_tok == NULL)
			{
				PrintDebug("%s(%d) start_of_tok is NULL\n", __FUNCTION__, __LINE__);
				continue;
			}
			start_of_tok = strtok_r(NULL, HUMAX_COOKIE_SEP,&savepoint);
			if(start_of_tok == NULL)
			{
				PrintDebug("%s(%d) start_of_tok is NULL\n", __FUNCTION__, __LINE__);
				continue;
			}
			PrintDebug(MAXDOME_MAN_STRING " string is [%s]\n", start_of_tok);

			new_string_size = VK_strlen(lic_challenge->url) + VK_strlen(start_of_tok) + 1 + 1;
			if(man)
			{
				DD_MEM_Free(man);
				man = NULL;
			}
			man = DD_MEM_Alloc(new_string_size);
			if(man == NULL)
			{
				PrintDebug("Error while VK_malloc new url in %s(%d)\n", __FUNCTION__, __LINE__);
				goto read_cookie_error;
			}

			nRet = VK_snprintf (man ,new_string_size, "%s&%s", lic_challenge->url, start_of_tok);
			if(nRet<0)
			{
				PrintDebug("can't make new url in %s(%d)\n", __FUNCTION__, __LINE__);
				goto read_cookie_error;
			}

			PrintDebug("New String is [%s]\n", man);
		}
	}

	/* close cookie */
	if(fd)
	{
		fclose(fd);
		fd = NULL;
	}
	}

license_request:

#define DI_MEDIA_MS_DRM_RETRY_COUNT (10)
	{
		char* chllenge_url = NULL;
		int retry_counter = DI_MEDIA_MS_DRM_RETRY_COUNT;
		if(man)
		{
			chllenge_url = man;
		}
		else
		{
			chllenge_url = lic_challenge->url;
		}

		while(TRUE)
		{
			retry_counter--;

			post_ret = bhttpclient_license_post_default(
				chllenge_url, //lic_challenge->url,
				lic_challenge->data,
				0, //lic_challenge->non_quiet,
				2000, //lic_challenge->app_security,
				&lic_response->data, &lic_response->resp_start, &lic_response->len);


			PrintDebug("License response length=%d\n", lic_response->len);

			ret = (post_ret >= (0)) ? bdrm_err_ok : bdrm_err_fail;

			if( ret == bdrm_err_fail)
			{
				PrintDebug("License Grabbing FAILED on %s(%d). Retrying...\n", __FUNCTION__, __LINE__);
				VK_TASK_Sleep(1000);
			}
			else
			{
				/* success case */
				break;
			}

			if(retry_counter <= 0)
			{
				PrintError("License Grabbing FAILED on %s(%d)\n", __FUNCTION__, __LINE__);
				goto failed;
			}
		}
	}

	/* change state and call license call back */
	bdrm_util_call_change_lic_http_state_and_license_callback(context, post_ret);

read_cookie_error:
;

failed:
	if(line_buffer)
	{
		DD_MEM_Free(line_buffer);
		line_buffer = NULL;
	}
	if(man)
	{
		DD_MEM_Free(man);
		man = NULL;
	}
	if(fd)
	{
		fclose(fd);
		fd = NULL;
	}
	PrintExit;

    return(ret);
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_bdrm_util_license_callback
 * Function Description:
 * Parameter:
 * Return Value:
 */
bdrm_err
P_MEDIA_DRM_bdrm_util_license_callback (bdrm_acq_state* lic_acq_state,
                            bdrm_license_t license,
                            void * context)
{
    bdrm_license_info   lic_info;
    bdrm_license_state  lic_state;
    bdrm_http_state     http_state;
    HUINT8             count       = 0;
    HUINT8             indx       = 0;
    bdrm_err            ret         = bdrm_err_ok;

	DI_MediaInstance *pMediaInstance = (DI_MediaInstance *)context;

    PrintEnter;

    if (lic_acq_state)
    {
        lic_state  = lic_acq_state->license_state;
        http_state = lic_acq_state->http_state;
        PrintDebug ("%s(%d) setting drm license state:%d http state: %d\n",__FUNCTION__,__LINE__, lic_state, http_state);
    }
    else
    {
        PrintError ("%s(%d) Error - 1\n",__FUNCTION__,__LINE__);
        return (ret) = bdrm_err_fail;
    }


    (void)P_MEDIA_DRM_SetLicenseState (pMediaInstance, lic_state);
    (void)P_MEDIA_DRM_SetHTTPState (pMediaInstance, http_state);

	PrintDebug("%s(%d) \n",__FUNCTION__,__LINE__);
    if ((lic_state == bdrm_lic_acquired)  ||
        (lic_state == bdrm_lic_faillocal) ||
        (lic_state == bdrm_lic_failtransfer))
    {
#if 1 /* for debug printing */
		if(lic_state == bdrm_lic_acquired)
		{
			PrintDebug("%s(%d) licstate bdrm_lic_acquired !!!\n",__FUNCTION__,__LINE__);
		}
		else if ( lic_state == bdrm_lic_faillocal)
		{
			PrintDebug("%s(%d) licstate bdrm_lic_faillocal !!!\n",__FUNCTION__,__LINE__);
		}
		else if ( lic_state == bdrm_lic_failtransfer)
		{
			PrintDebug("%s(%d) licstate bdrm_lic_failtransfer !!!\n",__FUNCTION__,__LINE__);
		}
#endif
        count = bdrm_license_get_count(license);
#if 1
		PrintDebug("%s(%d) licstate count=%d !!!\n",__FUNCTION__,__LINE__,count);
#endif
        for (indx = 0; indx < count; indx++)
        {
            VK_memset(&lic_info, 0, sizeof(bdrm_license_info));
            bdrm_license_get_item(license, indx, &lic_info);
            switch (lic_info.type)
            {
                case bdrm_lictype_from:
                    break;
                case bdrm_lictype_until:
                    break;
                case bdrm_lictype_fromuntil:
                    break;
                case bdrm_lictype_count:
                    break;
                case bdrm_lictype_unlimited:
                    break;
                case bdrm_lictype_noright:
                    break;
                case bdrm_lictype_unknown:
                    break;
            }
            PrintDebug ("%s(%d) stream id=%d type=(%d) "
                      "play_count=(%d) plays=%d times_count=(%d) times=%s\n",
                      __FUNCTION__,__LINE__,lic_info.stream_id,  lic_info.type,
                      lic_info.plays_count, lic_info.plays[0], lic_info.times_count, asctime(&lic_info.times[0]));
        }
    }
    else
    {
        ret = bdrm_err_fail;
    }

	PrintExit;
    return(ret);
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_GetLicenseState
 * Function Description:
 * Parameter:
 * Return Value:
 */
static bdrm_license_state P_MEDIA_DRM_GetLicenseState (DI_MediaInstance *pMediaInstance)
{
    bdrm_license_state state = pMediaInstance->tDrm_license_state;
    return state;
}

#if 0
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_GetHTTPState
 * Function Description:
 * Parameter:
 * Return Value:
 */
static bdrm_http_state P_MEDIA_DRM_GetHTTPState (void)
{
    bdrm_http_state state = pMediaInstance->tDrm_http_state;
    return state;
}
#endif

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_SetLicenseState
 * Function Description:
 * Parameter:
 * Return Value:
 */
static void P_MEDIA_DRM_SetLicenseState(DI_MediaInstance *pMediaInstance,bdrm_license_state state)
{

	PrintEnter;
    pMediaInstance->tDrm_license_state = state;
	PrintExit;

    return;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:           P_MEDIA_DRM_SetHTTPState
 * Function Description:
 * Parameter:
 * Return Value:
 */
static void P_MEDIA_DRM_SetHTTPState(DI_MediaInstance *pMediaInstance, bdrm_http_state state)
{
	PrintEnter;

    pMediaInstance->tDrm_http_state = state;

	PrintExit;
    return;
}



#endif


static const batom_user P_MEDIA_Probe_AtomUser =
{
	P_MEDIA_ProbeFreeAtom,
	sizeof(void **)
};


#if __________p__________
#endif

#ifdef SUPPORT_OLD_MEDIA
#define __________P_OLD_DI_MEDIA__________

DRV_Error P_MEDIA_PlayAudioMemory( unsigned char *pBuf,
						unsigned long ulRemainDataSize,unsigned long ulRemainLoopCount)
{
	DI_MEDIA_MSG	audMsg;
	DRV_Error eDRVError = DRV_OK;
	BERR_Code errCode = BERR_SUCCESS;
	int result,i;

	char 				*pBuffer;		//void *´Â segment fault
	unsigned long		ulBufferSize;
	size_t				bufSize;
	void 				*pvBuf;

	/* Check available buffer space */
	errCode = NEXUS_AudioPlayback_GetBuffer(stMediaInstance.audioPlayback, (void **)&pvBuf, &bufSize);
	pBuffer = (char*) pvBuf;	// remove compile warning in release mode (#61392)
	ulBufferSize = (unsigned long) bufSize;
	if( ( errCode != BERR_SUCCESS ) || (ulBufferSize == 0) || ( stMediaStreamInfo.pauseFlag == TRUE ))
	{
		audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;
		audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_PLAY;
		audMsg.pucParam = pBuf;
		audMsg.ulParam2 = ulRemainDataSize;
		audMsg.ulParam3 = ulRemainLoopCount;

		goto RETURN;
	}

	if ( ulBufferSize < ulRemainDataSize)
	{
		VK_memcpy((void *)pBuffer,pBuf , ulBufferSize);
		PrintCriticalDebug("[01;36m[%s] ulBufferSize= %lu, ulRemainDataSize = %lu[00m\r\n",__FUNCTION__,ulBufferSize,ulRemainDataSize);

		NEXUS_AudioPlayback_ReadComplete(stMediaInstance.audioPlayback, ulBufferSize);

		ulRemainDataSize -= ulBufferSize;
		pBuf += ulBufferSize;
		audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;
		audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_PLAY;
		audMsg.pucParam = pBuf;
		audMsg.ulParam2 = ulRemainDataSize;
		audMsg.ulParam3 = ulRemainLoopCount;
	}
	else
	{
		unsigned long totalWriteDataSize=0;
		PrintCriticalDebug("[01;36m[%s] ulBufferSize= %lu, ulRemainDataSize = %lu[00m\r\n",__FUNCTION__,ulBufferSize,ulRemainDataSize);

		VK_memcpy((void *)pBuffer,pBuf , ulRemainDataSize);
		totalWriteDataSize += ulRemainDataSize;

		if( stMediaStreamInfo.loopCount !=0 )
		{
			ulRemainLoopCount --;
		}

		PrintCriticalDebug("[01;36m[%s] Remained buffer Size = %lu[00m\r\n",__FUNCTION__,(ulBufferSize - ulRemainDataSize ));

		if(( stMediaStreamInfo.loopCount==0 ) || (ulRemainLoopCount))
		{
			if( (ulBufferSize - ulRemainDataSize )> stMediaStreamInfo.esDataSize)
			{
				pBuffer+=ulRemainDataSize;

				PrintCriticalDebug("\r\n\r\n[%s] Loop = %d, ulRemainLoopCount = %lu\r\n\r\n\r\n",__FUNCTION__,
							(ulBufferSize - ulRemainDataSize ) /stMediaStreamInfo.esDataSize,
							ulRemainLoopCount);

				for(i=0;i<(int)((ulBufferSize - ulRemainDataSize ) /stMediaStreamInfo.esDataSize);i++)
				{

					VK_memcpy((void *)pBuffer,
							(unsigned char *)stMediaStreamInfo.pbuffStartPoint , stMediaStreamInfo.esDataSize);

					totalWriteDataSize += stMediaStreamInfo.esDataSize;

					pBuffer+=(stMediaStreamInfo.esDataSize);

					if( stMediaStreamInfo.loopCount !=0 )
					{
						ulRemainLoopCount --;
						if(ulRemainLoopCount ==0)
							break;
					}
				}
				PrintCriticalDebug("\r\n\r\n[%s] Loop = %d, totalWriteDataSize = %lu\r\n\r\n\r\n",__FUNCTION__,i,totalWriteDataSize);

			}
		}

		ulRemainDataSize = 0;

		PrintCriticalDebug("[%s] Write Date size = %lu\r\n",__FUNCTION__,totalWriteDataSize);

		NEXUS_AudioPlayback_ReadComplete(stMediaInstance.audioPlayback, totalWriteDataSize);

		PrintCriticalDebug("[%s] RAM Play Probably EOF.....!\r\n",__FUNCTION__);

		PrintCriticalDebug("\r\n\r\n[%s] s_ulRamPlayLoopCnt = %lu\r\n\r\n\r\n",__FUNCTION__,ulRemainLoopCount);
		audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;

		if (( ulRemainLoopCount ) || ( stMediaStreamInfo.loopCount ==0 ))
		{
			audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_PLAY;
			audMsg.pucParam = (unsigned char *)stMediaStreamInfo.pbuffStartPoint;
			audMsg.ulParam2 = stMediaStreamInfo.esDataSize;
			audMsg.ulParam3 = ulRemainLoopCount;
		}
		else
		{
			audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_MONITOR;
		}
	}

RETURN:

	result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
	if(result != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,result);
		eDRVError = DRV_ERR;
	}

	return eDRVError;
}

DRV_Error P_MEDIA_StopAudioMemory( void)
{
	PrintDebug("[%s] To Stop Audio Memory Stream\r\n",__FUNCTION__);

	DD_MEM_Free(stMediaStreamInfo.pbuffStartPoint);
	stMediaStreamInfo.esDataSize= 0;
	stMediaStreamInfo.loopCount = 0;
	stMediaStreamInfo.pauseFlag = FALSE;
	stMediaStreamInfo.emptyChkCnt = 0;

	stMediaInstance.audioStatus = DI_MEDIA_AUDIO_STOP;

	NEXUS_AudioPlayback_Stop(stMediaInstance.audioPlayback);
#if 0 /* disable DRV_AUDIO_SetInput call */
	DRV_AUDIO_SetInput( NEXUS_AudioPlayback_GetConnector(stMediaInstance.audioPlayback) , FALSE );
#endif

	return DRV_OK;
}

DRV_Error P_MEDIA_MonitorAudioMemory( void)
{
	DI_MEDIA_MSG	audMsg;
	DRV_Error eDRVError = DRV_OK;
	int result;
	NEXUS_AudioPlaybackStatus audioPbStatus;
	char 				*pBuffer;		//void *´Â segment fault
	unsigned long		ulBufferSize;
	size_t				bufSize;
	void 				*pvBuf;


	if ( stMediaStreamInfo.pauseFlag == TRUE )
	{
		PrintDebug("[01;36m[%s] Only Break??????\r\n[00m",__FUNCTION__);
		return eDRVError;
	}

	NEXUS_AudioPlayback_GetStatus(stMediaInstance.audioPlayback, &audioPbStatus);

	PrintDebug("[01;36m[%s] Fifo = %d ,Queue=%d\r\n[00m",__FUNCTION__,
				audioPbStatus.fifoSize,audioPbStatus.queuedBytes);
	/* Check available buffer space */
	NEXUS_AudioPlayback_GetBuffer(stMediaInstance.audioPlayback, (void **)&pvBuf, &bufSize);
	pBuffer = (char*) pvBuf;	// remove compile warning in release mode (#61392)
	ulBufferSize = (unsigned long) bufSize;
	PrintDebug("[01;36m[%s] ulBufferSize= %lu[00m\r\n",__FUNCTION__,ulBufferSize);

	if ( audioPbStatus.queuedBytes == 18815)	//why??????
	{
		stMediaStreamInfo.emptyChkCnt ++;
		if ( stMediaStreamInfo.emptyChkCnt > 10)
		{
			PrintDebug("[01;36m[%s] Audio Playback Buffer Empty...PlayStream play done!! [00m\r\n",__FUNCTION__);
			audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;
			audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_STOP;

			result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(audMsg), 0);
			if ( result != VK_OK)
			{
				PrintError("[AudioStreamPlayTask] Send MSG Error_%s__, _%d_\n", __FILE__, __LINE__);
				eDRVError = DRV_ERR;
			}

			return eDRVError;
		}
	}
	else 	if ( audioPbStatus.queuedBytes < 18815)
	{
		stMediaStreamInfo.emptyChkCnt = 0;
	}

	audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;
	audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_MONITOR;

	result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(audMsg), 0);
	if ( result != VK_OK)
	{
		PrintError("[AudioStreamPlayTask] Send MSG Error_%s__, _%d_\n", __FILE__, __LINE__);
		eDRVError = DRV_ERR;
	}

	return eDRVError;
}

DRV_Error P_MEDIA_PlayAudioFileStream( void)
{
	return DRV_OK;
}

DRV_Error P_MEDIA_StopAudioFileStream( void)
{
//	NEXUS_PlaybackSettings playbackSettings;

	PrintDebug("[%s] To Stop Audio File Stream\r\n",__FUNCTION__);

	DRV_AUDIO_Stop (0);
	DRV_AUDIO_Stop (2);

//	NEXUS_Playback_GetDefaultStartSettings (&playbackSettings);
//	NEXUS_Playback_SetSettings(stMediaInstance.playback, &playbackSettings);

	NEXUS_Playback_Stop(stMediaInstance.playback);

	NEXUS_Playback_ClosePidChannel(stMediaInstance.playback, stMediaInstance.audioPidChannel );

	stMediaInstance.audioPidChannel = NULL;

//	NEXUS_Playback_Destroy(stMediaInstance.playback);

	DRV_PVR_ReleasePlaypumpHandle(stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_AUDIO_MAIN]);
	stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_AUDIO_MAIN] = NULL;

//	NEXUS_Playpump_Close(stMediaInstance.playpump[DI_MEDIA_MP3]);
//	stMediaInstance.playback = NULL;
	NEXUS_FilePlay_Close(stMediaStreamInfo.file);

	return DRV_OK;
}



DRV_Error P_MEDIA_PlayVideoFileStream( void)
{
	return DRV_OK;
}

DRV_Error P_MEDIA_StopVideoFileStream( void)
{
	DI_ERR_CODE  nRet = DI_ERR_OK;

	PrintDebug("[%s] To Stop Video File Stream\r\n",__FUNCTION__);

	DRV_AUDIO_Stop (0);
	DRV_AUDIO_Stop (2);

	nRet = DI_VIDEO_Stop(0);
	if ( nRet != DI_ERR_OK )
	{
		PrintDebug("[%s] DI_VIDEO_Stop() fail\r\n",__FUNCTION__);
	}

	NEXUS_Playback_Stop(stMediaInstance.playback);

	NEXUS_Playback_ClosePidChannel(stMediaInstance.playback, stMediaInstance.audioPidChannel );
	NEXUS_Playback_ClosePidChannel(stMediaInstance.playback, stMediaInstance.videoPidChannel );

	stMediaInstance.audioPidChannel = NULL;
	stMediaInstance.videoPidChannel = NULL;

	DRV_PVR_ReleasePlaypumpHandle(stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_DIVX]);
	stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_DIVX] = NULL;

	NEXUS_FilePlay_Close(stMediaStreamInfo.file);

	return DRV_OK;
}
#endif

#define __________P_NEW_DI_MEDIA__________

/**
 * @brief		ÇöÀç »ç¿ëµÇÁö ¾Ê´Â Media ID¸¦ ¾ò´Â´Ù.
 *
 * @param[out]	punMediaId 	Media ID
 * @return		DI_ERR_CODE
 * @see
 */
static DI_ERR_CODE P_MEDIA_GetMediaId(HUINT32 *punMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	HUINT32			i=0;
	PrintEnter;

	for(i = 0;i<DI_MEDIA_MAX_NUM;i++)
	{
		if(s_tMediaInstance[i].valid == FALSE)
		{
			*punMediaId = i;
			s_tMediaInstance[i].valid = TRUE;
			break;
		}
	}

	if(i == DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] Media ID is full.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}

	PrintExit;

	return eDIError;
}

/**
 * @brief		Media Id°¡ À¯È¿ÇÑÁö °Ë»çÇÑ´Ù.
 *
 * @param[in]	unMediaId 	Media ID
 * @return		DI_ERR_CODE
 * @see
 */
static DI_ERR_CODE P_MEDIA_CheckValidMediaId(HUINT32 unMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
/*
	PrintEnter;
*/
	if(unMediaId >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] Invalid Media ID.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if(s_tMediaInstance[unMediaId].valid != TRUE)
		{
			PrintError("[%s] Invalid Media ID.\n", __func__);
			eDIError = DI_ERR_ERROR;
		}
		else
		{
			eDIError = DI_ERR_OK;
		}
	}
/*
	PrintExit;
*/
	return eDIError;
}

/**
 * @brief		Media ID¿¡ ´ëÇÑ PlaybackÀ» »ý¼ºÇÑ´Ù.
 *
 * @param[in]	unMediaId 	Media ID
 * @return		DI_ERR_CODE
 * @see			P_MEDIA_DeletePlayback
 */
static DI_ERR_CODE P_MEDIA_CreatePlayback(HUINT32 unMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;

	PrintEnter;

	if(s_tMediaInstance[unMediaId].valid == FALSE)
	{
		PrintError("[%s] Media ID is full.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		s_tMediaInstance[unMediaId].playback = NEXUS_Playback_Create();
		if(s_tMediaInstance[unMediaId].playback == NULL)
		{
			PrintError("[%s] Fail NEXUS_Playback_Create.\n", __func__);
			eDIError = DI_ERR_ERROR;
		}
	}

	PrintExit;

	return eDIError;
}

/**
 * @brief		Media ID¿¡ ´ëÇÑ PlaybackÀ» »èÁ¦ÇÑ´Ù.
 *
 * @param[in]	unMediaId 	Media ID
 * @return		DI_ERR_CODE
 * @see			P_MEDIA_CreatePlayback
 */
static DI_ERR_CODE P_MEDIA_DeletePlayback(HUINT32 unMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;

	PrintEnter;

	if(s_tMediaInstance[unMediaId].valid == FALSE)
	{
		PrintError("[%s] Invalid Media ID.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		(void)NEXUS_Playback_Destroy(s_tMediaInstance[unMediaId].playback);
	}

	PrintExit;

	return eDIError;
}



/**
 * @brief		CallbackÇÔ¼ö·Î µî·Ï½Ã Stream Àç»ýÀÌ ½ÃÀÛµÇ¸é¼­ È£ÃâµÈ´Ù.
 *
 * @param[in]	context 	º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @param[in]	param 		º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @return		void
 * @see			DI_MEDIA_RegisterCallback
 */
static void P_MEDIA_BeginOfStreamCallback(void *context, int param)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	BSTD_UNUSED(context);

	s_nCurrentPlayTime[param] = 0;

	if(s_tMediaInstance[param].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		s_bPlayTimerMode[param] = DI_MEDIA_TIMERMODE_UPDATE_FULLTIME;

		if(s_tMediaInstance[param].mediastate == DI_MEDIA_STATE_TRICK)
		{
			if(NULL == VK_strstr((char *)s_tMediaInstance[param].mediaurl, "mms://"))
			{
				(void)DI_MEDIA_MP_SetPosition(param, 0, DI_MEDIA_TIME_SEC);
			}

			(void)DI_MEDIA_MP_SetSpeed(param, DI_MEDIA_TRICK_PAUSE);
		}

		if ( NULL != s_tEventCallback[param] )
		{
			s_tEventCallback[param] ((HUINT32)param, DI_MEDIA_EVENT_BEGINOFSTREAM, 0);
			s_tEventCallback[param]((HUINT32)param, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
		}
		/* else - do nothing */

	}

	PrintExit;

	return;
}

/**
 * @brief		CallbackÇÔ¼ö·Î µî·Ï½Ã Stream Àç»ý¿¡ ¹®Á¦°¡ ¹ß»ýÇÏ¸é È£ÃâµÈ´Ù.
 *
 * @param[in]	context 	º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @param[in]	param 		º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @return		void
 * @see			DI_MEDIA_RegisterCallback
 */
static void P_MEDIA_ErrorOfStreamCallback(void *context, int param)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	BSTD_UNUSED(context);

	if(s_tMediaInstance[param].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if ( NULL != s_tEventCallback[param] )
		{
			s_tEventCallback[param] ((HUINT32)param, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
		}
		/* else - do nothing */
	}

	PrintExit;

	return;
}

/**
 * @brief		CallbackÇÔ¼ö·Î µî·Ï½Ã Stream Àç»ýÀÌ ³ª¸é È£ÃâµÈ´Ù.
 *
 * @param[in]	context 	º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @param[in]	param	 	º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @return		void
 * @see			DI_MEDIA_RegisterCallback
 */
static void P_MEDIA_EndOfStreamCallback(void *context, int param)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	BSTD_UNUSED(context);

	if(s_tMediaInstance[param].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if ( NULL != s_tEventCallback[param] )
		{
			s_tEventCallback[param] ((HUINT32)param, DI_MEDIA_EVENT_ENDOFSTREAM, 0);
		}
		/* else - do nothing */
	}

	PrintExit;

	return;
}

/**
 * @brief		DI Video CodecÀ» NexusÀÇ Video Codec TypeÀ¸·Î º¯È¯ÇÑ´Ù.
 *
 * @param[in]	eVideoCodec 		Di Media Video Codec Type
 * @return		NEXUS_VideoCodec	Nexus Video Codec Type
 * @see			DRV_MEDIA_GetPlaybackVideoPidChannel
 */
static NEXUS_VideoCodec P_MEDIA_ConvertMediaVideoCodecToNexus(DI_MEDIA_VIDEO_FORMAT_e eVideoCodec)
{
	NEXUS_VideoCodec nexusVideoCodec;

	PrintEnter;

	switch(eVideoCodec)
	{
		case DI_MEDIA_VIDEO_FORMAT_MPEG1:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg1;
			break;
		case DI_MEDIA_VIDEO_FORMAT_MPEG2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			break;
		case DI_MEDIA_VIDEO_FORMAT_MPEG4Part2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg4Part2;
			break;
		case DI_MEDIA_VIDEO_FORMAT_H261:
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			break;
		case DI_MEDIA_VIDEO_FORMAT_H263:
			nexusVideoCodec = NEXUS_VideoCodec_eH263;
			break;
		case DI_MEDIA_VIDEO_FORMAT_H264:
			nexusVideoCodec = NEXUS_VideoCodec_eH264;
			break;
		case DI_MEDIA_VIDEO_FORMAT_VC1SimpleMain:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1SimpleMain;
			break;
		case DI_MEDIA_VIDEO_FORMAT_VC1:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1;
			break;
		case DI_MEDIA_VIDEO_FORMAT_AVS:
			nexusVideoCodec = NEXUS_VideoCodec_eAvs;
			break;
		case DI_MEDIA_VIDEO_FORMAT_DIVX_311:
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311;
			break;
		default:
			nexusVideoCodec = NEXUS_VideoCodec_eUnknown;
			break;
	}

	PrintExit;

	return nexusVideoCodec;
}

/**
 * @brief		DI Audio CodecÀ» NexusÀÇ Audio Codec TypeÀ¸·Î º¯È¯ÇÑ´Ù.
 *
 * @param[in]	eAudioCodec 	Di Media Audio Codec Type
 * @return		NEXUS_AudioCodec	Nexus Audio Codec Type
 * @see			DRV_MEDIA_GetPlaybackAudioPidChannel
 */
NEXUS_AudioCodec P_MEDIA_ConvertMediaAudioCodecToNexus(DI_MEDIA_AUDIO_FORMAT_e eAudioCodec)
{
	NEXUS_AudioCodec nexusCodec = NEXUS_AudioCodec_eUnknown;

	PrintEnter;

	switch (eAudioCodec)
	{
		case DI_MEDIA_AUDIO_FORMAT_MPEG:
			nexusCodec = NEXUS_AudioCodec_eMpeg;
			break;
		case DI_MEDIA_AUDIO_FORMAT_MP3:
			nexusCodec = NEXUS_AudioCodec_eMp3;
			break;
		case DI_MEDIA_AUDIO_FORMAT_AAC:
			nexusCodec = NEXUS_AudioCodec_eAac;
			break;
		case DI_MEDIA_AUDIO_FORMAT_AAC_PLUS:
			nexusCodec = NEXUS_AudioCodec_eAacPlus;	//load???? adts????
			break;
		case DI_MEDIA_AUDIO_FORMAT_AC3:
			nexusCodec = NEXUS_AudioCodec_eAc3;
			break;
		case DI_MEDIA_AUDIO_FORMAT_AC3_PLUS:
			nexusCodec = NEXUS_AudioCodec_eAc3Plus;
			break;
		case DI_MEDIA_AUDIO_FORMAT_PCM:
			nexusCodec = NEXUS_AudioCodec_ePcm;//pcm???
			break;
		case DI_MEDIA_AUDIO_FORMAT_PCMWAVE:
			nexusCodec = NEXUS_AudioCodec_ePcmWav;//pcm???
			break;
		case DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_ADTS:
			nexusCodec = NEXUS_AudioCodec_eAacPlusAdts;
			break;
		case DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_LOAS:
			nexusCodec = NEXUS_AudioCodec_eAacPlusLoas;
			break;
		case DI_MEDIA_AUDIO_FORMAT_DTS:
			nexusCodec = NEXUS_AudioCodec_eDts;
			break;
		case DI_MEDIA_AUDIO_FORMAT_LPCM_HD_DVD:
			nexusCodec = NEXUS_AudioCodec_eLpcmHdDvd;
			break;
		case DI_MEDIA_AUDIO_FORMAT_LPCM_BLUERAY:
			nexusCodec = NEXUS_AudioCodec_eLpcmBluRay;
			break;
		case DI_MEDIA_AUDIO_FORMAT_DTS_HD:
			nexusCodec = NEXUS_AudioCodec_eDtsHd;
			break;
		case DI_MEDIA_AUDIO_FORMAT_WMA_STD:
			nexusCodec = NEXUS_AudioCodec_eWmaStd;
			break;
		case DI_MEDIA_AUDIO_FORMAT_WMA_PRO:
			nexusCodec = NEXUS_AudioCodec_eWmaPro;
			break;
		case DI_MEDIA_AUDIO_FORMAT_LPCM_DVD:
			nexusCodec = NEXUS_AudioCodec_eLpcmDvd;
			break;
		default:
			nexusCodec = NEXUS_AudioCodec_eUnknown;
			break;
	}

	PrintExit;

	return nexusCodec;
}

/**
 * @brief		DI Transport TypeÀ» NexusÀÇ Transport TypeÀ¸·Î º¯È¯ÇÑ´Ù.
 *
 * @param[in]	eTransportType 		Di Transport Type
 * @return		NEXUS_TransportType	Nexus Transport Type
 * @see			DRV_MEDIA_GetPlaybackAudioPidChannel
 */
NEXUS_TransportType P_MEDIA_ConvertTransportTypeToNexus(DI_MEDIA_STREAM_TYPE_e eTransportType)
{
	NEXUS_TransportType eNexusTrasportType = NEXUS_TransportType_eUnknown;

	PrintEnter;

	switch (eTransportType)
	{
		case DI_MEDIA_MPEG_TYPE_ES:
			eNexusTrasportType = NEXUS_TransportType_eEs;
			break;
		case DI_MEDIA_MPEG_TYPE_BES:
			eNexusTrasportType = NEXUS_TransportType_eEs;
			break;
		case DI_MEDIA_MPEG_TYPE_PES:
			eNexusTrasportType = NEXUS_TransportType_eMpeg2Pes;
			break;
		case DI_MEDIA_MPEG_TYPE_TS:
		case DI_MEDIA_MPEG_TYPE_TS_192:
			eNexusTrasportType = NEXUS_TransportType_eTs;	//load???? adts????
			break;
		case DI_MEDIA_MPEG_TYPE_DSS_ES:
			eNexusTrasportType = NEXUS_TransportType_eDssEs;
			break;
		case DI_MEDIA_MPEG_TYPE_DSS_PES:
			eNexusTrasportType = NEXUS_TransportType_eDssPes;
			break;
		case DI_MEDIA_MPEG_TYPE_VOB:
			eNexusTrasportType = NEXUS_TransportType_eVob;//pcm???
			break;
		case DI_MEDIA_MPEG_TYPE_ASF:
			eNexusTrasportType = NEXUS_TransportType_eAsf;
			break;
		case DI_MEDIA_MPEG_TYPE_AVI:
			eNexusTrasportType = NEXUS_TransportType_eAvi;
			break;
		case DI_MEDIA_MPEG_TYPE_MPEG1:
			eNexusTrasportType = NEXUS_TransportType_eMpeg1Ps;
			break;
		case DI_MEDIA_MPEG_TYPE_MP4:
			eNexusTrasportType = NEXUS_TransportType_eMp4;
			break;
#ifdef CONFIG_MEDIA_SSTR
		case DI_MEDIA_MPEG_TYPE_MP4FRAGMENT: /* for SSTR */
			eNexusTrasportType = NEXUS_TransportType_eMp4Fragment; /* Fragmented MP4 Container */
			break;
#endif
		case DI_MEDIA_MPEG_TYPE_FLV:
			eNexusTrasportType = NEXUS_TransportType_eFlv;
			break;
		case DI_MEDIA_MPEG_TYPE_MKV:
			eNexusTrasportType = NEXUS_TransportType_eMkv;
			break;
		case DI_MEDIA_MPEG_TYPE_WAVE:
			eNexusTrasportType = NEXUS_TransportType_eWav;
			break;
		default:
			eNexusTrasportType = NEXUS_TransportType_eUnknown;
			break;
	}

	PrintExit;

	return eNexusTrasportType;
}


/**
 * @brief		NexusÀÇ TrickSpeed¸¦ DI¿¡ Á¤ÀÇµÈ Trick Speed·Î º¯È¯ÇÑ´Ù.
 *
 * @param[in]	nTrickSpeed 			Nexus Trick Speed
 * @return		DI_MEDIA_TrickSpeed_t	Di Trick Speed
 * @see
 */
DI_MEDIA_TrickSpeed_e P_MEDIA_ConvertTrickSpeedToDI(int nTrickSpeed)
{
	DI_MEDIA_TrickSpeed_e eReturnTrickSpeed = 0;

	PrintEnter;

	switch(nTrickSpeed)
	{
		case 0:
			eReturnTrickSpeed = DI_MEDIA_TRICK_PAUSE;
			break;
		/** Trick(Forward) */
		case 1000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_1FFSTEP;
			break;
		case 2000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_2FFSTEP;
			break;
		case 3000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_3FFSTEP;
			break;
		case 4000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_4FFSTEP;
			break;
		case 5000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_5FFSTEP;
			break;
		case 6000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_6FFSTEP;
			break;
		case 7000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_7FFSTEP;
			break;
		case 8000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_8FFSTEP;
			break;
		case 9000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_9FFSTEP;
			break;
		case 10000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_10FFSTEP;
			break;
		case 11000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_11FFSTEP;
			break;
		case 12000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_12FFSTEP;
			break;
		case 13000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_13FFSTEP;
			break;
		case 16000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_14FFSTEP;
			break;
		case 32000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_15FFSTEP;
			break;
		case 64000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_16FFSTEP;
			break;
		/** Trick(Rewind) */
		case -1000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_1RWSTEP;
			break;
		case -2000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_2RWSTEP;
			break;
		case -3000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_3RWSTEP;
			break;
		case -4000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_4RWSTEP;
			break;
		case -5000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_5RWSTEP;
			break;
		case -6000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_6RWSTEP;
			break;
		case -7000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_7RWSTEP;
			break;
		case -8000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_8RWSTEP;
			break;
		case -9000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_9RWSTEP;
			break;
		case -10000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_10RWSTEP;
			break;
		case -11000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_11RWSTEP;
			break;
		case -12000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_12RWSTEP;
			break;
		case -13000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_13RWSTEP;
			break;
		case -16000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_14RWSTEP;
			break;
		case -32000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_15RWSTEP;
			break;
		case -64000:
			eReturnTrickSpeed = DI_MEDIA_TRICK_16RWSTEP;
			break;
		default:
			{
				PrintError("[%s] Error - Invalid TrickSpeed:%d\n", __func__, nTrickSpeed);
				eReturnTrickSpeed = 0;
			}
			break;
	}

	PrintExit;

	return eReturnTrickSpeed;
}

/**
 * @brief		NexusÀÇ TrickSpeed¸¦ DI¿¡ Á¤ÀÇµÈ Trick Speed·Î º¯È¯ÇÑ´Ù.
 *
 * @param[in]	nTrickSpeed 			Nexus Trick Speed
 * @return		DI_MEDIA_TrickSpeed_t	Di Trick Speed
 * @see
 */
int P_MEDIA_ConvertDITrickSpeedToNexus(DI_MEDIA_TrickSpeed_e nTrickSpeed)
{
	int eNexusTrickSpeed = 0;

	PrintEnter;

	switch(nTrickSpeed)
	{
		case DI_MEDIA_TRICK_PAUSE:
			eNexusTrickSpeed = 0;
			break;
		/** Trick(Forward) */
		case DI_MEDIA_TRICK_1FFSTEP:
			eNexusTrickSpeed = 1000;
			break;
		case DI_MEDIA_TRICK_2FFSTEP:
			eNexusTrickSpeed = 2000;
			break;
		case DI_MEDIA_TRICK_3FFSTEP:
			eNexusTrickSpeed = 3000;
			break;
		case DI_MEDIA_TRICK_4FFSTEP:
			eNexusTrickSpeed = 4000;
			break;
		case DI_MEDIA_TRICK_5FFSTEP:
			eNexusTrickSpeed = 5000;
			break;
		case DI_MEDIA_TRICK_6FFSTEP:
			eNexusTrickSpeed = 6000;
			break;
		case DI_MEDIA_TRICK_7FFSTEP:
			eNexusTrickSpeed = 7000;
			break;
		case DI_MEDIA_TRICK_8FFSTEP:
			eNexusTrickSpeed = 8000;
			break;
		case DI_MEDIA_TRICK_9FFSTEP:
			eNexusTrickSpeed = 9000;
			break;
		case DI_MEDIA_TRICK_10FFSTEP:
			eNexusTrickSpeed = 10000;
			break;
		case DI_MEDIA_TRICK_11FFSTEP:
			eNexusTrickSpeed = 11000;
			break;
		case DI_MEDIA_TRICK_12FFSTEP:
			eNexusTrickSpeed = 12000;
			break;
		case DI_MEDIA_TRICK_13FFSTEP:
			eNexusTrickSpeed = 13000;
			break;
		case DI_MEDIA_TRICK_14FFSTEP:
			eNexusTrickSpeed = 16000;
			break;
		case DI_MEDIA_TRICK_15FFSTEP:
			eNexusTrickSpeed = 32000;
			break;
		case DI_MEDIA_TRICK_16FFSTEP:
			eNexusTrickSpeed = 64000;
			break;
		/** Trick(Rewind) */
		case DI_MEDIA_TRICK_1RWSTEP:
			eNexusTrickSpeed = -1000;
			break;
		case DI_MEDIA_TRICK_2RWSTEP:
			eNexusTrickSpeed = -2000;
			break;
		case DI_MEDIA_TRICK_3RWSTEP:
			eNexusTrickSpeed = -3000;
			break;
		case DI_MEDIA_TRICK_4RWSTEP:
			eNexusTrickSpeed = -4000;
			break;
		case DI_MEDIA_TRICK_5RWSTEP:
			eNexusTrickSpeed = -5000;
			break;
		case DI_MEDIA_TRICK_6RWSTEP:
			eNexusTrickSpeed = -6000;
			break;
		case DI_MEDIA_TRICK_7RWSTEP:
			eNexusTrickSpeed = -7000;
			break;
		case DI_MEDIA_TRICK_8RWSTEP:
			eNexusTrickSpeed = -8000;
			break;
		case DI_MEDIA_TRICK_9RWSTEP:
			eNexusTrickSpeed = -9000;
			break;
		case DI_MEDIA_TRICK_10RWSTEP:
			eNexusTrickSpeed = -10000;
			break;
		case DI_MEDIA_TRICK_11RWSTEP:
			eNexusTrickSpeed = -11000;
			break;
		case DI_MEDIA_TRICK_12RWSTEP:
			eNexusTrickSpeed = -12000;
			break;
		case DI_MEDIA_TRICK_13RWSTEP:
			eNexusTrickSpeed = -13000;
			break;
		case DI_MEDIA_TRICK_14RWSTEP:
			eNexusTrickSpeed = -16000;
			break;
		case DI_MEDIA_TRICK_15RWSTEP:
			eNexusTrickSpeed = -32000;
			break;
		case DI_MEDIA_TRICK_16RWSTEP:
			eNexusTrickSpeed = -64000;
			break;
		default:
			{
				PrintError("[%s] Error - Invalid TrickSpeed:%d\n", __func__, nTrickSpeed);
				eNexusTrickSpeed = 0;
			}
			break;
	}

	PrintExit;

	return eNexusTrickSpeed;
}

/**
 * @brief		DI Transport TypeÀ» NexusÀÇ Transport TypeÀ¸·Î º¯È¯ÇÑ´Ù.
 *
 * @param[in]	eTransportType 		Di Transport Type
 * @return		NEXUS_TransportType	Nexus Transport Type
 * @see			DRV_MEDIA_GetPlaybackAudioPidChannel
 */
DI_AUDIO_DECODER_TYPE P_MEDIA_ConvertAudioTypeToDiAudio(DI_MEDIA_AUDIO_TYPE_e eAudioType)
{
	DI_AUDIO_DECODER_TYPE eDiAudioType = DI_AUDIO_DECODER_MAIN;

	PrintEnter;

	switch (eAudioType)
	{
		case DI_MEDIA_AUDIO_DECODER_MAIN:
			eDiAudioType = DI_AUDIO_DECODER_MAIN;
			break;
		case DI_MEDIA_AUDIO_DECODER_SUB:
			eDiAudioType = DI_AUDIO_DECODER_SUB;
			break;
		case DI_MEDIA_AUDIO_DECODER_SPDIF:
			eDiAudioType = DI_AUDIO_DECODER_SPDIF;
			break;
		default:
			eDiAudioType = NEXUS_TransportType_eUnknown;
			break;
	}

	PrintExit;

	return eDiAudioType;
}

#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)

static DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_t DRV_MEDIA_StatusGet(void)
{
	return sulStatusMonitorValue;
}
static void DRV_MEDIA_StatusSet(DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_t status)
{
	sulStatusMonitorValue = status;
}


static void DRV_MEDIA_StatusMonitorThread(void *pParam)
{
	unsigned int msg_timeout = 200;
	unsigned int set_position_timeout = 0;
	unsigned long msg[4];
	int return_value = 0;
	NEXUS_PlaybackStatus tPlaybackStatus;
	int media_id = 0xFF;
	int status = 0xFF;
	if ( pParam == NULL )
	{
	}
	while (1)
	{
		return_value = VK_MSG_ReceiveTimeout(sulStatusMonitorThreadQ, msg, sizeof(unsigned long)*4, msg_timeout);
		if ( return_value == VK_OK )
		{
			switch(msg[0])
			{
				case DRV_MEDIA_SETPOSITION_ACCURACY_ENTER_MEDIA:
					media_id = msg[1];
					msg_timeout = 1000;
					PrintDebug ("media id is (%d)\n", msg[1]);
					break;
				case DRV_MEDIA_SETPOSITION_ACCURACY_QUIT_MEDIA:
					media_id = 0xFF;
					break;
				case DRV_MEDIA_SETPOSITION_ACCURACY_ENTER_SET_POS:
					DRV_MEDIA_StatusSet(DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_SETPOSITION); /* Set Position */
					break;
				default:
					break;
			}
		}
		else if ( return_value == VK_TIMEOUT )
		{
			if ( media_id != 0xFF )
			{
				msg_timeout = 300;
				NEXUS_Playback_GetStatus(s_tMediaInstance[media_id].playback, &tPlaybackStatus);
				PrintDebug ("(MONITOR_VALUE(%d,%ld,%ld,%ld)\n", status, tPlaybackStatus.fifoDepth, tPlaybackStatus.fifoSize,s_MfpInfo[media_id].StreamInfo.max_bitrate);

				if ( DRV_MEDIA_StatusGet() == DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_SETPOSITION )
				{
					if ( (tPlaybackStatus.fifoDepth * 3)  > ( s_MfpInfo[media_id].StreamInfo.max_bitrate) )
					{
						DRV_MEDIA_StatusSet(DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_ENDSETPOSITION);
						set_position_timeout = 0;
						msg_timeout = 10000;

						NEXUS_Playback_Play(s_tMediaInstance[media_id].playback);

					}
					else
					{
						set_position_timeout = set_position_timeout + msg_timeout;
						if ( set_position_timeout > 20000 )
						{
							DRV_MEDIA_StatusSet(DRV_MEDIA_SETPOSITION_ACCURACY_STATUS_ENDSETPOSITION);
							set_position_timeout = 0;
							msg_timeout = 10000;

							NEXUS_Playback_Play(s_tMediaInstance[media_id].playback);
						}
					}

				}
				else
				{
					msg_timeout = 10000;
					continue;

				}

			}


		}
	}
}

#endif


/**
 * @brief		Stream Probe¸¦ À§ÇØ Resource¸¦ ÇÒ´çÇÏ°í Probe¸¦ ÁøÇàÇÑ´Ù.
 *
 * @param[in]	pszStreamUrl 	Todo
 * @param[in]	pMfpInfo 		Todo
 * @return		void
 * @see			DI_MEDIA_Probe
 */
static DI_ERR_CODE P_MEDIA_ProbeOpen (HUINT8 *pszStreamUrl, stMediaInfo *pMfpInfo)
{
	DI_ERR_CODE 				nRet = DI_ERR_OK;

	bmedia_probe_t 			probe = NULL;
	bmedia_probe_config 		probe_config;
#if 0	/* warning: assignment discards qualifiers from pointer target type */
	/* warning: cast discards qualifiers from pointer target type       */
	bmedia_probe_stream 		*stream = NULL;
	const bmedia_probe_track 	*track = NULL;
#else
	const bmedia_probe_stream 	*stream = NULL;
	bmedia_probe_track		*track = NULL;
#endif
	bfile_io_read_t  		fd=NULL;
	unsigned 			audio_track = 0, video_track = 0, track_no = 0;
	char 				stream_info[512] = {0,};
	bmedia_filter_cfg		filter_cfg;
	bstream_mpeg_type		stream_type = bstream_mpeg_type_unknown;
	bmedia_player_config		player_config;
	HUINT8				ucTotalAudioTrack=0;
	HUINT8				ucTotalVideoTrack=0;
	HINT32				i=0, j=0;
	HUINT8				*pucData=NULL;
	HUINT32				Count_TS_192Type=0;
#ifdef CONFIG_VFIO
	int rc = 0;
	off_t lCurOffset = 0;
	size_t nReadSize = 0;
#else
    FILE				*pFile = NULL;
#endif
#if defined (USE_DRM)
	bool		bIsDrm = FALSE;
#endif

	PrintEnter;

	pMfpInfo->fd			= NULL;
	pMfpInfo->seek			= BMEDIA_PLAYER_INVALID;
	pMfpInfo->seek2			= BMEDIA_PLAYER_INVALID;
	pMfpInfo->buffer		= NULL;
	pMfpInfo->buffer_cfg.buf	= NULL;

	/* normal decode */
	pMfpInfo->direction		= 0;
	pMfpInfo->time_scale		= BMEDIA_TIME_SCALE_BASE;

	pMfpInfo->StreamInfo.audio_codec = DI_MEDIA_AUDIO_FORMAT_NONE;
	pMfpInfo->StreamInfo.audio_stream_id = 0;
	pMfpInfo->StreamInfo.video_stream_id = 0;	/* VIDEO_STREAM_ID */
	pMfpInfo->StreamInfo.video_codec = DI_MEDIA_VIDEO_FORMAT_NONE;

#if defined(CONFIG_MEDIA_PROBE_CA_DESC)
	pMfpInfo->StreamInfo.ca_system_id_cnt = 0;
#endif

	if(pszStreamUrl == NULL)
	{
		PrintError ("[P_MEDIA_ProbeOpen] Error - pszStreamUrl is null");
		goto error;
	}

	if((VK_strlen(pszStreamUrl) >DI_MEDIA_STREAM_URL_LEN) && (NULL == VK_strstr((char *)pszStreamUrl,"/HMXCOOKIE=")))
	{
		PrintError ("[P_MEDIA_ProbeOpen] Error - pszStreamUrl Length is Over Stack[%d](%u)", DI_MEDIA_STREAM_URL_LEN, VK_strlen(pszStreamUrl));
		goto error;
	}
	else
	{
		PrintData("[P_MEDIA_ProbeOpen] pszStreamUrl=%s, Length[%u]\n", pszStreamUrl, VK_strlen(pszStreamUrl));
	}

#ifdef	CONFIG_VFIO
	{
		rc = VFIO_Open ((char *)pszStreamUrl, "rb", &(pMfpInfo->tHandle));
		if (rc)
		{
			PrintError ("[P_MEDIA_ProbeOpen] Error - VFIO_Open()=%d", rc);
			goto error;
		}
		/* else { continue } */
	}
#else
	pMfpInfo->fin = fopen(pszStreamUrl, "rb");
	if (pMfpInfo->fin == NULL)
	{
		goto error;
	}
	/* else { continue } */
#endif

	pMfpInfo->factory = batom_factory_create(bkni_alloc, B_PUMP_DEMUX_FACTORY_ATOMS);
	if (pMfpInfo->factory == NULL)
	{
		goto error;
	}
	/* else { continue } */

	(void)bfile_async_init();

	/* search stream type */
	probe = bmedia_probe_create();
	if (probe == NULL)
	{
		goto error;
	}
	/* else { continue } */

#ifdef	CONFIG_VFIO
	fd = bfile_stdio_read_attach(pMfpInfo->tHandle);
#else
	fd = bfile_stdio_read_attach(pMfpInfo->fin);
#endif

	VK_MEM_Memset(&probe_config, 0, sizeof(probe_config));

	(void)bmedia_probe_default_cfg(&probe_config);
	probe_config.file_name		= (char *)pszStreamUrl;
	probe_config.type		= bstream_mpeg_type_unknown;
	probe_config.probe_es = false;
	probe_config.probe_payload = false;

	/* Fixed warning: assignment discards qualifiers from pointer target type */
	/* const bmedia_probe_stream *bmedia_probe_parse(bmedia_probe_t probe, bfile_io_read_t fd, const bmedia_probe_config *config) */
	stream = bmedia_probe_parse(probe, fd, &probe_config);

	(void)bfile_stdio_read_detach(fd);

	if(stream == NULL)
	{
		if(NULL != VK_strstr((char *)pszStreamUrl, "[DLNA]"))
		{
			pMfpInfo->opened	= true;
			(void)bmedia_probe_destroy(probe);
			return DI_ERR_OK;
		}

		PrintError ("[%s] Error>> unknown stream format|\n",__FUNCTION__);
		if((NULL != VK_strstr((char *)pszStreamUrl, ".mp3")) || (NULL != VK_strstr((char *)pszStreamUrl, ".MP3")))
		{
			pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_ES;
			pMfpInfo->StreamInfo.nprograms = 1;
			pMfpInfo->StreamInfo.ntracks = 1;
			pMfpInfo->StreamInfo.video_pes_id = 0;
			pMfpInfo->StreamInfo.video_stream_id = 0;
			pMfpInfo->StreamInfo.video_codec = 0;
			pMfpInfo->StreamInfo.audio_pes_id = 192;
			pMfpInfo->StreamInfo.audio_stream_id = 1;
			pMfpInfo->StreamInfo.audio_codec = DI_MEDIA_AUDIO_FORMAT_MP3;
			pMfpInfo->StreamInfo.pcr_pid = 8191;

			pMfpInfo->opened	= true;
			(void)bmedia_probe_destroy(probe);
			return DI_ERR_OK;
		}
		else if((NULL != VK_strstr((char *)pszStreamUrl, ".LPCM")) || (NULL != VK_strstr((char *)pszStreamUrl, ".lpcm")))
		{
			pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_WAVE;
			pMfpInfo->StreamInfo.nprograms = 1;
			pMfpInfo->StreamInfo.ntracks = 1;
			pMfpInfo->StreamInfo.video_pes_id = 0;
			pMfpInfo->StreamInfo.video_stream_id = 0;
			pMfpInfo->StreamInfo.video_codec = 0;
			pMfpInfo->StreamInfo.audio_pes_id = 192;
			pMfpInfo->StreamInfo.audio_stream_id = 1;
			pMfpInfo->StreamInfo.audio_codec = DI_MEDIA_AUDIO_FORMAT_PCMWAVE;
			pMfpInfo->StreamInfo.pcr_pid = 8191;

			pMfpInfo->opened	= true;
			(void)bmedia_probe_destroy(probe);
			return DI_ERR_OK;
		}
		else if((NULL != VK_strstr((char *)pszStreamUrl, ".PCM")) || (NULL != VK_strstr((char *)pszStreamUrl, ".pcm")))
		{
			pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_WAVE;
			pMfpInfo->StreamInfo.nprograms = 1;
			pMfpInfo->StreamInfo.ntracks = 1;
			pMfpInfo->StreamInfo.video_pes_id = 0;
			pMfpInfo->StreamInfo.video_stream_id = 0;
			pMfpInfo->StreamInfo.video_codec = 0;
			pMfpInfo->StreamInfo.audio_pes_id = 192;
			pMfpInfo->StreamInfo.audio_stream_id = 1;
			pMfpInfo->StreamInfo.audio_codec = DI_MEDIA_AUDIO_FORMAT_PCMWAVE;
			pMfpInfo->StreamInfo.pcr_pid = 8191;

			pMfpInfo->opened	= true;
			(void)bmedia_probe_destroy(probe);
			return DI_ERR_OK;
		}

		goto error;
	}
	/* else { continue } */
//	int bmedia_stream_to_string_forDRM(const bmedia_probe_stream *stream, char *buf, size_t size, bool *pbIsDRM);
//(bool*)&pMfpInfo->StreamInfo.is_drm

#if defined (USE_DRM)
#if 1
	(void)bmedia_stream_to_string_forDRM(stream, stream_info, sizeof(stream_info), &bIsDrm);
	pMfpInfo->StreamInfo.is_drm = bIsDrm;
#else
	{
		// Pre-delivery license method with XML data.
		// Let's assume that the contents are always  encrypted. am i right?
		pMfpInfo->StreamInfo.is_drm = TRUE;
	}
#endif
#else
	(void)bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
#endif

	PrintData ("[%s] stream_info : %s|\n",__FUNCTION__,stream_info);
	PrintData ("[%s] max_bitrate : %u|\n",__FUNCTION__,stream->max_bitrate);
	PrintData ("[%s] index : %d|\n",__FUNCTION__,stream->index);
	PrintData ("[%s] duration : %d|\n",__FUNCTION__,stream->duration);
	PrintData ("[%s] nprograms : %d|\n",__FUNCTION__,stream->nprograms);
	PrintData ("[%s] ntracks : %d|\n",__FUNCTION__,stream->ntracks);
	PrintData ("[%s] type : %d|\n",__FUNCTION__,stream->type);
	PrintData ("[%s] probe_id : %d|\n",__FUNCTION__,stream->probe_id);
	PrintData ("[%s] is_drm : %d\n",__FUNCTION__, pMfpInfo->StreamInfo.is_drm);

	stream_type = stream->type;

	pMfpInfo->StreamInfo.max_bitrate	= stream->max_bitrate;
	pMfpInfo->StreamInfo.duration		= stream->duration;
	pMfpInfo->StreamInfo.nprograms		= stream->nprograms;
	pMfpInfo->StreamInfo.ntracks		= stream->ntracks;

	switch (stream_type)
	{
		case bstream_mpeg_type_avi:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_AVI;
			break;
		case bstream_mpeg_type_asf:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_ASF;
			break;
		case bstream_mpeg_type_vob:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_VOB;
			break;
		case bstream_mpeg_type_ts:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_TS;
			break;
		case bstream_mpeg_type_pes:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_PES;
			break;
		case bstream_mpeg_type_es:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_ES;
			break;
		case bstream_mpeg_type_bes:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_BES;
			break;
		case bstream_mpeg_type_dss_es:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_DSS_ES;
			break;
		case bstream_mpeg_type_dss_pes:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_DSS_PES;
			break;
		case bstream_mpeg_type_mpeg1:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_MPEG1;
			break;
		case bstream_mpeg_type_mp4:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_MP4;
			break;
		case bstream_mpeg_type_flv:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_FLV;
			break;
		case bstream_mpeg_type_mkv:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_MKV;
			break;
		case bstream_mpeg_type_wav:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_WAVE;
			break;
		default:
			pMfpInfo->StreamInfo.stream_type	= DI_MEDIA_MPEG_TYPE_UNKNOWN;
			break;
	}

#if defined(HDR_FOX_T2) || defined(HD_FOX_T2)
	if((pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF) && (NULL != VK_strstr((char *)pszStreamUrl, "http://")))
	{
		pMfpInfo->StreamInfo.duration		= stream->SendTime;
	}
#endif

	pMfpInfo->StreamInfo.pcr_pid	= 0x1fff; /* NULL_PID;*/


#if defined(CONFIG_MEDIA_PROBE_CA_DESC)
	BKNI_Memset(pMfpInfo->StreamInfo.ca_system_id, 0, (sizeof(HUINT16) * DI_MEDIA_MAX_NUM_OF_CA_SYSTEM_ID));
#endif

	for(audio_track=0,video_track=0,track_no=1,track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link),track_no++)
	{
		PrintData ("track->number = %d\n", track->number);

		switch(track->type)
		{
			case bmedia_track_type_audio:
				if(ucTotalAudioTrack < DI_MEDIA_DIVX_MAX_AUDIO_TRACK)
				{
					PrintData ("audio track %u codec:0x%x channel count %d sample size %d bitrate %d smaple rate %d\n",
							track->number, track->info.audio.codec, track->info.audio.channel_count, track->info.audio.sample_size, track->info.audio.bitrate, track->info.audio.sample_rate);

					pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
					pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_stream_id = track->number;

					switch (track->info.audio.codec)
					{
						case baudio_format_mpeg:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_MPEG;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_MPEG;
							audio_track = track->number;
							break;
						case baudio_format_mp3:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_MP3;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_MP3;
							audio_track = track->number;
							break;
						case baudio_format_aac:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_AAC;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_AAC;
							audio_track = track->number;
							break;
						case baudio_format_aac_plus:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_AAC_PLUS;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS;
							audio_track = track->number;
							break;
						case baudio_format_aac_plus_adts:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_ADTS;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_ADTS;
							audio_track = track->number;
							break;
						case baudio_format_ac3:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_AC3;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_AC3;
							audio_track = track->number;
							break;
						case baudio_format_ac3_plus:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_AC3_PLUS;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_AC3_PLUS;
							audio_track = track->number;
							break;
						case baudio_format_dts:
#if 0		/* ÇöÀç DTS CodecÀº Áö¿øÇÏÁö ¾Ê½À´Ï´Ù. */
							pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_DTS;
							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_DTS;
#else
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_NONE;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_NONE;
#endif
							break;
						case baudio_format_lpcm_hddvd:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_LPCM_HD_DVD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_LPCM_HD_DVD;
							audio_track = track->number;
							break;
						case baudio_format_lpcm_bluray:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_LPCM_BLUERAY;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_LPCM_BLUERAY;
							audio_track = track->number;
							break;
						case baudio_format_dts_hd:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_DTS_HD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_DTS_HD;
							audio_track = track->number;
							break;
						case baudio_format_wma_std:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_WMA_STD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_WMA_STD;
							audio_track = track->number;
							break;
						case baudio_format_wma_pro:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_WMA_PRO;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_WMA_PRO;
							audio_track = track->number;
							break;
						case baudio_format_lpcm_dvd:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_LPCM_DVD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_LPCM_DVD;
							audio_track = track->number;
							break;
						case baudio_format_pcm:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_MEDIA_AUDIO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_PCM;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_PCM;
							audio_track = track->number;
							break;
						default:
							if(ucTotalAudioTrack == 0)
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_MEDIA_AUDIO_FORMAT_NONE;
							}
							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_MEDIA_AUDIO_FORMAT_NONE;
							break;
					}

					if((pMfpInfo->StreamInfo.audio_codec != DI_MEDIA_AUDIO_FORMAT_NONE) && (pMfpInfo->StreamInfo.audio_stream_id == 0))
					{
						pMfpInfo->StreamInfo.audio_stream_id	= audio_track;	/* AUDIO_STREAM_ID */
					}

					pMfpInfo->StreamInfo.audio_pes_id	= DI_MEDIA_AUDIO_PES_ID;

					if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_AVI)
					{
						/* Removed warning: cast discards qualifiers from pointer target type */
						/* Caution to const declared vars and casting */
						VK_strncpy(pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].uacAudioLang, ((bavi_probe_track *)track)->language, 16);
					}
#if defined(CONFIG_MEDIA_PROBE_PMT)
					else if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
					{
						HUINT32 uiAudLangLen = (HUINT32)(((bmpeg2ts_psi_probe_track *)track)->descriptor_size);
						HUINT32 uiAudLangMax = (HUINT32)(sizeof(pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].uacAudioLang));

						if(uiAudLangLen > 0 && uiAudLangLen < uiAudLangMax){
							VK_strncpy(pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].uacAudioLang, ((bmpeg2ts_psi_probe_track *)track)->descriptor, uiAudLangLen);
						}else{
							PrintError("This audio track descriptor has a wrong data! (size: %d)\n", uiAudLangLen);
						}
						PrintData("MPEG2 TS Audio Language : [%s]\n", pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].uacAudioLang);
					}
#endif
					ucTotalAudioTrack++;
				}
				break;
			case bmedia_track_type_video:
				if(ucTotalVideoTrack < DI_MEDIA_DIVX_MAX_VIDEO_TRACK)
				{
					PrintData ("video track %u codec:0x%x width %d height %d\n", track->number, track->info.video.codec, track->info.video.width, track->info.video.height);

					pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
					pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_stream_id = track->number;

					switch (track->info.video.codec)
					{
						case bvideo_codec_divx_311:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_DIVX_311;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_DIVX_311;
							video_track = track->number;
							break;
						case bvideo_codec_mpeg1:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_MPEG1;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_MPEG1;
							video_track = track->number;
							break;
						case bvideo_codec_mpeg2:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_MPEG2;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_MPEG2;
							video_track = track->number;
							break;
						case bvideo_codec_mpeg4_part2:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_MPEG4Part2;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_MPEG4Part2;
							video_track = track->number;
							break;
						case bvideo_codec_h263:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_H263;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_H263;
							video_track = track->number;
							break;
						case bvideo_codec_h264:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_H264;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_H264;
							video_track = track->number;
							break;
						case bvideo_codec_vc1:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_VC1;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_VC1;
							video_track = track->number;
							break;
						case bvideo_codec_vc1_sm:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_MEDIA_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_VC1SimpleMain;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_VC1SimpleMain;
							video_track = track->number;
							break;
						default:
							if(ucTotalVideoTrack == 0)
							{
								pMfpInfo->StreamInfo.video_codec	= DI_MEDIA_VIDEO_FORMAT_NONE;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_MEDIA_VIDEO_FORMAT_NONE;
							break;
					}

					if((pMfpInfo->StreamInfo.video_codec != DI_MEDIA_VIDEO_FORMAT_NONE) && (pMfpInfo->StreamInfo.video_stream_id == 0))
					{
						pMfpInfo->StreamInfo.video_stream_id	= video_track;	/* VIDEO_STREAM_ID */
					}

					pMfpInfo->StreamInfo.video_pes_id	= DI_MEDIA_VIDEO_PES_ID;

					if(pMfpInfo->StreamInfo.video_codec != DI_MEDIA_VIDEO_FORMAT_NONE)
					{
						pMfpInfo->StreamInfo.usVideoWidth = track->info.video.width;
						pMfpInfo->StreamInfo.usVideoHeight = track->info.video.height;
					}

					ucTotalVideoTrack++;
				}
				break;
			case bmedia_track_type_pcr:
				pMfpInfo->StreamInfo.pcr_pid	= track->number;
				break;

#if defined(CONFIG_MEDIA_PROBE_PMT)
			case bmedia_track_type_pmt:
				pMfpInfo->StreamInfo.pmt_pid = track->number;
				pMfpInfo->StreamInfo.program_number = track->program;
				break;
#endif
			default:
				break;
		}

#if defined(CONFIG_MEDIA_PROBE_CA_DESC)
		if (track->ca_system_id_cnt > 0)
		{
			for (i = 0; i < track->ca_system_id_cnt; i++)
			{
				for (j = 0; j < pMfpInfo->StreamInfo.ca_system_id_cnt; j++)
				{
					/* check same ca_system_id */
					if (track->ca_system_id[i] == pMfpInfo->StreamInfo.ca_system_id[j])
					{
						break;
					}
				}
				if (j == pMfpInfo->StreamInfo.ca_system_id_cnt)
				{
					/* new ca_system_id */
					if (pMfpInfo->StreamInfo.ca_system_id_cnt < DI_MEDIA_MAX_NUM_OF_CA_SYSTEM_ID)
					{
						pMfpInfo->StreamInfo.ca_system_id[j] = track->ca_system_id[i];
						pMfpInfo->StreamInfo.ca_system_id_cnt++;
					}
				}
			}
		}
#endif
	}

	PrintData ("[%s] Detected format:%u video:%u audio:%u|\n",__FUNCTION__, pMfpInfo->StreamInfo.stream_type, pMfpInfo->StreamInfo.video_stream_id, pMfpInfo->StreamInfo.audio_stream_id);
	PrintData ("[%s] Total Audio Track:%u Total Video Track:%u|\n",__FUNCTION__, ucTotalAudioTrack, ucTotalVideoTrack);

	pMfpInfo->StreamInfo.ucTotalAudioTrack = ucTotalAudioTrack;
	pMfpInfo->StreamInfo.ucTotalVideoTrack = ucTotalVideoTrack;

	(void)bmedia_probe_stream_free(probe, stream);
	(void)bmedia_probe_destroy(probe);
	stream	= NULL;
	probe	= NULL;

	if((pMfpInfo->StreamInfo.video_codec==DI_MEDIA_VIDEO_FORMAT_NONE ) && (ucTotalVideoTrack >0))
	{
		PrintError ("Error>> Cannot support video format\n");
		goto error;
	}

	if((ucTotalAudioTrack == 0 ) && (ucTotalVideoTrack == 0))
	{
		PrintError ("Error>> Cannot support video format\n");
		goto error;
	}

#ifdef	CONFIG_VFIO
	if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
	{
		rc = VFIO_Tell(pMfpInfo->tHandle, &lCurOffset);
		rc |= VFIO_Seek(pMfpInfo->tHandle, 0, 0);
		if (rc)
		{
			PrintError ("%s(%d) Error>	VFIO_Tell or VFIO_Seek()=%d",__FUNCTION__,__LINE__, rc);
			pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS;
		}
		else
		{
			pucData = DD_MEM_Alloc(DI_MEDIA_TSTYPE_SEARCH_LEN);
			if (pucData == NULL)
			{
				PrintError ("Error>> Fail heap allocation (%s)\n", pszStreamUrl);

				pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS;
			}
			else
			{
				VK_MEM_Memset(pucData, 0x0, (DI_MEDIA_TSTYPE_SEARCH_LEN));
				(void)VFIO_Read(pMfpInfo->tHandle, pucData, 1, DI_MEDIA_TSTYPE_SEARCH_LEN, &nReadSize);
				if(nReadSize != DI_MEDIA_TSTYPE_SEARCH_LEN)
				{
					PrintError ("Error>> Fail fread(%s)\n", pszStreamUrl);
					if(pucData != NULL)
					{
						DD_MEM_Free(pucData);
						pucData = NULL;
					}

					pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS;
				}
				else
				{
					/* TSÆÄÀÏ¿¡ ¾Õ¿¡ Á¤º¸°¡ µé¾î°£ °æ¿ì¸¦ ÆÇ´ÜÇÏ±â À§ÇØ 2K´Â TS PacketÀÎÁö °ËÅäÇÕ´Ï´Ù. */
					for(i=0; i<DI_MEDIA_TSINFO_LEN; i++)
					{
						if(pucData[i] == MEDIA_MPEG2TS_PKT_SYNC)
						{
							for(j=1;j<=5;j++)
							{
								/* TSÆÄÀÏÀÌ 192packetÀÎÁö 188packetÀÎÁö °ËÅäÇÕ´Ï´Ù. */
								if(pucData[i+(j*MEDIA_MPEG2TS_192PKT_LEN)] == MEDIA_MPEG2TS_PKT_SYNC)
								{
									Count_TS_192Type = Count_TS_192Type + 1;
									if(Count_TS_192Type == 4)
									{
										break;
									}
								}
							}

							if(Count_TS_192Type == 4)
							{
								break;
							}
							else
							{
								Count_TS_192Type = 0;
								i = i + MEDIA_MPEG2TS_188PKT_LEN;
							}
						}
					}

					if(Count_TS_192Type == 4)
					{
						PrintError ("##### [%s] DI_MEDIA_MPEG_TYPE_TS_192 #####\n", __FUNCTION__);
						pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS_192;
					}
					else
					{
						PrintError ("##### [%s] DI_MEDIA_MPEG_TYPE_TS #####\n", __FUNCTION__);
						pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS;
					}

					if(pucData != NULL)
					{
						DD_MEM_Free(pucData);
						pucData = NULL;
					}

					(void)VFIO_Seek(pMfpInfo->tHandle, lCurOffset, 0);
				}
			}
		}
	}
#else
	if((pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS) && (NULL == VK_strstr((char *)pszStreamUrl, "http://")) && (NULL == VK_strstr((char *)pszStreamUrl, "https://")) &&
			(NULL == VK_strstr((char *)pszStreamUrl, "mms://")) && (NULL == VK_strstr((char *)pszStreamUrl, "rtsp://")))
	{
		pFile = fopen((char *)pszStreamUrl, "rb");
	 	if(pFile == NULL)
		{
			PrintError ("Error>> Fail fopen(%s)\n", pszStreamUrl);
			goto error;
		}

		pucData = DD_MEM_Alloc(DI_MEDIA_TSTYPE_SEARCH_LEN);
		if (pucData == NULL)
		{
			PrintError ("Error>> Fail heap allocation (%s)\n", pszStreamUrl);

			fclose(pFile);

			goto error;
		}

		VK_MEM_Memset(pucData, 0x0, (DI_MEDIA_TSTYPE_SEARCH_LEN));
		if(fread(pucData, 1, DI_MEDIA_TSTYPE_SEARCH_LEN, pFile) != DI_MEDIA_TSTYPE_SEARCH_LEN)
		{
			PrintError ("Error>> Fail fread(%s)\n", pszStreamUrl);
			if(pucData != NULL)
			{
				DD_MEM_Free(pucData);
				pucData = NULL;
			}

			fclose(pFile);

			goto error;
		}
		else
		{
			/* TSÆÄÀÏ¿¡ ¾Õ¿¡ Á¤º¸°¡ µé¾î°£ °æ¿ì¸¦ ÆÇ´ÜÇÏ±â À§ÇØ 2K´Â TS PacketÀÎÁö °ËÅäÇÕ´Ï´Ù. */
			for(i=0; i<DI_MEDIA_TSINFO_LEN; i++)
			{
				if(pucData[i] == MEDIA_MPEG2TS_PKT_SYNC)
				{
					for(j=1;j<=5;j++)
					{
						/* TSÆÄÀÏÀÌ 192packetÀÎÁö 188packetÀÎÁö °ËÅäÇÕ´Ï´Ù. */
						if(pucData[i+(j*MEDIA_MPEG2TS_192PKT_LEN)] == MEDIA_MPEG2TS_PKT_SYNC)
						{
							Count_TS_192Type = Count_TS_192Type + 1;
							if(Count_TS_192Type == 4)
							{
								break;
							}
						}
					}

					if(Count_TS_192Type == 4)
					{
						break;
					}
					else
					{
						Count_TS_192Type = 0;
						i = i + MEDIA_MPEG2TS_188PKT_LEN;
					}
				}
			}

			if(Count_TS_192Type == 4)
			{
				PrintError ("##### [%s] DI_MEDIA_MPEG_TYPE_TS_192 #####\n", __FUNCTION__);
				pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS_192;
			}
			else
			{
				PrintError ("##### [%s] DI_MEDIA_MPEG_TYPE_TS #####\n", __FUNCTION__);
				pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS;
			}

			if(pucData != NULL)
			{
				DD_MEM_Free(pucData);
				pucData = NULL;
			}

			fclose(pFile);
		}
	}
#endif
	if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_UNKNOWN)
	{
		nRet = DI_ERR_INVALID_PARAM;
		goto error;
	}
#if 1 /* First PTS Detection */
	else if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS
	 || pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS_192 )
	{
#if 1
#if 1 	/* workaround : junsy */
		/* ÀÌ ±â´ÉÀº ÃßÈÄ DI_IPTUNER_Probe ¿¡¼­ drm content µé¿¡ ´ëÇÑ firstpts ÃßÃâÀÌ ¿Ï·áµÇ´Â´ë·Î
		 °ü·Ã ÇÔ¼ö ¹× parameter µé »èÁ¦ ÇÊ¿äÇÔ.  */
		if(pMfpInfo->StreamInfo.ca_pid != 0)
		{
			PrintDebug ("%s(%d) we can't extract the first pts about this stream because it is encrypted content."
			,__FUNCTION__,__LINE__);
		}
		else
#endif
		{
			b_test_pid test_pid;
			bmpeg2ts_parser_t parser;
			bmpeg2ts_parser_cfg parser_cfg;

			off_t bytes_parsed;
			off_t off;
//			unsigned i;
			const size_t block_size=4*BIO_BLOCK_SIZE;
			const size_t buf_size=128*block_size;

#ifdef CONFIG_VFIO
			fd = bfile_stdio_read_attach(pMfpInfo->tHandle);
#else
			fd = bfile_stdio_read_attach(pMfpInfo->fin);
#endif
			if(fd == NULL)
			{
				PrintError ("%s(%d) Error> bfile_stdio_read_attach fail!\n",__FUNCTION__,__LINE__);
				goto error;
			}

			pMfpInfo->pipe_media= batom_pipe_create(pMfpInfo->factory);
			if (pMfpInfo->pipe_media == NULL)
			{
				PrintError ("%s(%d) Error> batom_pipe_create fail!\n",__FUNCTION__,__LINE__);
				bfile_stdio_read_detach(fd);
				goto error;
			}

			bmpeg2ts_parser_default_cfg(&parser_cfg);
			if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
			{
				parser_cfg.header_size = 4;
			}
			parser = bmpeg2ts_parser_create(pMfpInfo->factory, &parser_cfg);
			if(parser == NULL)
			{
				PrintError ("%s(%d) Error> bmpeg2ts_parser_create fail!\n",__FUNCTION__,__LINE__);
				bfile_stdio_read_detach(fd);
				goto error;
			}
			if(parser == NULL)
			{
				PrintError ("%s(%d) Error> bmpeg2ts_parser_create fail!\n",__FUNCTION__,__LINE__);
				bfile_stdio_read_detach(fd);
				goto error;
			}
			test_pid.pcr_count = 0;
			test_pid.ts_bytes = 0;
			test_pid.pts_count = 0;
			test_pid.pes_bytes = 0;

#if 1
			bmpeg2ts_parser_pid_init (&test_pid.ts_pid, pMfpInfo->StreamInfo.video_stream_id);
#else
			bmpeg2ts_parser_pid_init (&test_pid.ts_pid, pMfpInfo->StreamInfo.audio_stream_id);
#endif

			bfile_buffer_default_cfg(&pMfpInfo->buffer_cfg);

			pMfpInfo->buffer_cfg.buf_len = buf_size;
			pMfpInfo->buffer_cfg.buf = DD_MEM_Alloc(pMfpInfo->buffer_cfg.buf_len);
			if (pMfpInfo->buffer_cfg.buf == NULL)
			{
				PrintError ("%s(%d) Error>> DD_MEM_Alloc fail \n",__FUNCTION__,__LINE__);
				bmpeg2ts_parser_destroy(parser);
				bfile_stdio_read_detach(fd);
				goto error;
			}

			pMfpInfo->buffer_cfg.fd = fd;
			pMfpInfo->buffer_cfg.nsegs = buf_size/block_size;

			pMfpInfo->buffer = bfile_buffer_create(pMfpInfo->factory, &pMfpInfo->buffer_cfg);

			if (pMfpInfo->buffer == NULL)
			{
				PrintError ("%s(%d) Error>> bfile_buffer_create fail \n",__FUNCTION__,__LINE__);
				bmpeg2ts_parser_destroy(parser);
				bfile_stdio_read_detach(fd);
				goto error;
			}

			rc = bmpeg2pes_parser_init(pMfpInfo->factory, &test_pid.pes, BMPEG2PES_ID_ANY);
			if(rc!=0)
			{
				PrintError ("%s(%d) Error>> bmpeg2pes_parser_init fail\n",__FUNCTION__,__LINE__);
				bmpeg2ts_parser_destroy(parser);
				bfile_stdio_read_detach(fd);
				goto error;
			}

			test_pid.mfp_pointer = (void*)pMfpInfo;
			test_pid.pes.packet_cnxt = &test_pid;
			test_pid.pes.packet = P_MEDIA_GettingPESPacket;
			for(bytes_parsed=0,i=0;i<1;i++)
			{
				test_pid.ts_pid.payload = P_MEDIA_GettingTSPayload;
				bmpeg2ts_parser_add_pid(parser, &test_pid.ts_pid);
				for(off=0;/* off<1024*1024*1024 */;) {
					batom_t atom;
					bfile_buffer_result result;
					size_t atom_len;
					size_t feed_len;

					atom = bfile_buffer_read(pMfpInfo->buffer, off, block_size, &result);
					if(!atom) {
						break;
					}
					atom_len = batom_len(atom);
					off += atom_len;
					batom_pipe_push(pMfpInfo->pipe_media, atom);
					feed_len = bmpeg2ts_parser_feed(parser, pMfpInfo->pipe_media);
					if(feed_len!=atom_len)
					{
						PrintDebug ("%s(%d) Info> %u %u %lld",__FUNCTION__,__LINE__, feed_len, block_size, off);
					}

					BDBG_ASSERT(feed_len==atom_len);

					if(test_pid.pts_count>1)
					{
						PrintDebug(" First PTS installed successfully in MFP structure #1 \n");
						break;
					}

					if(pMfpInfo->StreamInfo.max_bitrate != 0)
					{
						if(off > (pMfpInfo->StreamInfo.max_bitrate/8)*5) /* ReadÇÑ Size°¡ Content bitrate 5ÃÊºÐ·® º¸´Ù Å« °æ¿ì Fail·Î ReturnÇÔ */
						{
							break;
						}
					}
					else
						break;
				}

				if(test_pid.pts_count>1)
				{
					PrintDebug(" First PTS installed successfully in MFP structure #2\n");
					break;
				}

				bytes_parsed += off;
				batom_pipe_flush(pMfpInfo->pipe_media);
				bmpeg2ts_parser_reset(parser);
				bmpeg2pes_parser_reset(&test_pid.pes);
			}
			bmpeg2ts_parser_destroy(parser);
			bmpeg2pes_parser_shutdown(&test_pid.pes);
			bfile_stdio_read_detach(fd);
#if 0 /* release routine for reference */
			bfile_buffer_destroy(pMfpInfo->buffer);
			batom_pipe_destroy(pMfpInfo->pipe_media);
			bfile_stdio_read_detach(fd);
#ifdef	__USE_VFIO__
			if ( pMfpInfo->tHandle.ucAvailable )
			{
				(void)VFIO_Close (pMfpInfo->tHandle);
			}
			/* else { continue } */
#else
			if (pMfpInfo->fin != NULL)
			{
				(void)fclose (pMfpInfo->fin);
				pMfpInfo->fin	= NULL;
			}
			/* else { continue } */
#endif
			batom_factory_destroy(pMfpInfo->factory);
#endif /* for reference */
			/**************************************
			 * END OF FIRSTPTS Patch
			 **************************************/
		}
#endif
		pMfpInfo->no_player 			= true;
		pMfpInfo->StreamInfo.support_trick	= TRUE;
		/* 2008.07.08 - by Dylan, Check the seek support */
		pMfpInfo->StreamInfo.support_seek 	= TRUE;
	}
#endif
	else
	{
		pMfpInfo->pipe_media= batom_pipe_create(pMfpInfo->factory);
		if (pMfpInfo->pipe_media == NULL)
		{
			goto error;
		}
		/* else { continue } */

		(void)bmedia_filter_default_cfg(&filter_cfg);
		filter_cfg.eos_len	= sizeof(s_eos_data);
		filter_cfg.eos_data	= s_eos_data;
		pMfpInfo->filter	= bmedia_filter_create(pMfpInfo->factory, bkni_alloc, &filter_cfg);
		if (pMfpInfo->filter == NULL)
		{
			goto error;
		}
		/* else { continue } */

		if((stream_type == bstream_mpeg_type_mp4) || (stream_type == bstream_mpeg_type_mkv) || (stream_type == bstream_mpeg_type_es))
		{
			pMfpInfo->pipe_pes = bmedia_filter_start(pMfpInfo->filter, bstream_mpeg_type_pes);
		}
		else
		{
			pMfpInfo->pipe_pes = bmedia_filter_start(pMfpInfo->filter, stream_type);
		}

		if (pMfpInfo->pipe_pes == NULL)
		{
			goto error;
		}
		/* else { continue } */

		if(pMfpInfo->StreamInfo.video_stream_id)
		{
#if defined(NEXUS_VERSION) && NEXUS_VERSION >= 700
			bmedia_stream_cfg stream_cfg;
			bmedia_stream_default_cfg(&stream_cfg);
			pMfpInfo->video = bmedia_filter_stream_open (pMfpInfo->filter, pMfpInfo->StreamInfo.video_stream_id, pMfpInfo->StreamInfo.video_pes_id, &stream_cfg);
#else
			pMfpInfo->video = bmedia_filter_stream_open (pMfpInfo->filter, pMfpInfo->StreamInfo.video_stream_id, pMfpInfo->StreamInfo.video_pes_id);
#endif
			if (pMfpInfo->video == NULL)
			{
				goto error;
			}
			/* else { continue } */
		}
		/* else { continue } */

		if(pMfpInfo->StreamInfo.audio_stream_id)
		{
#if defined(NEXUS_VERSION) && NEXUS_VERSION >= 700
			bmedia_stream_cfg stream_cfg;
			bmedia_stream_default_cfg(&stream_cfg);
			pMfpInfo->audio = bmedia_filter_stream_open (pMfpInfo->filter, pMfpInfo->StreamInfo.audio_stream_id, pMfpInfo->StreamInfo.audio_pes_id, &stream_cfg);
#else
			pMfpInfo->audio = bmedia_filter_stream_open (pMfpInfo->filter, pMfpInfo->StreamInfo.audio_stream_id, pMfpInfo->StreamInfo.audio_pes_id);
#endif
			if (pMfpInfo->audio == NULL)
			{
				goto error;
			}
			/* else { continue } */
		}
		/* else { continue } */

#ifdef	CONFIG_VFIO
		pMfpInfo->fd = bfile_stdio_read_attach(pMfpInfo->tHandle);
#else
		pMfpInfo->fd = bfile_stdio_read_attach(pMfpInfo->fin);
#endif

		(void)bfile_buffer_default_cfg(&pMfpInfo->buffer_cfg);
#if 1	/* Nexus¿¡¼­ »ç¿ëÇÏ´Â Buffer¸¦ CheckÇÏ¿© Àû¿ëÇØ¾ßÇÔ. ÇöÀç´Â 32*15*4096À¸·Î µÊ. */
		/* buf_lenÀº Á¶Á¤°¡´ÉÇÑ »çÀÌÁî·Î ÇöÀç´Â Default¸¦ È°¿ëÇÔ. */
		pMfpInfo->buffer_cfg.buf_len = DI_MFP_FEED_TOTAL_BUF_SIZE;
		pMfpInfo->buffer_cfg.buf = DD_MEM_Alloc(pMfpInfo->buffer_cfg.buf_len);
#else
		pMfpInfo->buffer_cfg.buf = DD_MEM_Alloc(DI_MFP_FEED_BLOCK_BUF_SIZE);
#endif
		if (pMfpInfo->buffer_cfg.buf == NULL)
		{
			PrintError ("Error>> Cannot allocate memory\n");
			goto error;
		}

		pMfpInfo->async				= true;
		pMfpInfo->buffer_cfg.async		= true;
		pMfpInfo->buffer_cfg.async_read		= bfile_async_read;
		pMfpInfo->buffer_cfg.fd			= pMfpInfo->fd;
		pMfpInfo->buffer 			= bfile_buffer_create(pMfpInfo->factory, &pMfpInfo->buffer_cfg);

		if (pMfpInfo->buffer == NULL)
		{
			PrintError ("Error>> bfile_buffer_create\n");
			goto error;
		}
		/* else { continue } */

		(void)bmedia_player_init_stream(&pMfpInfo->stream);
		pMfpInfo->stream.format		= stream_type;
		pMfpInfo->stream.master		= pMfpInfo->StreamInfo.video_stream_id;
		pMfpInfo->stream.other[0]	= pMfpInfo->StreamInfo.audio_stream_id;

		if(stream_type == bstream_mpeg_type_ts || (pMfpInfo->StreamInfo.video_stream_id && (stream_type == bstream_mpeg_type_vob || stream_type == bstream_mpeg_type_pes || stream_type == bstream_mpeg_type_mpeg1)))
		{
			/* ³ìÈ­ÇÑ Content¿¡ ´ëÇÑ IndexÆÄÀÏ(Ex> ~~.nav)ÀÌ Á¸ÀçÇÏ´Â °æ¿ì ÀÌ Ç×¸ñÀº False°¡ µÇ¾î¾ß ÇÔ. */
			pMfpInfo->stream.without_index   = true;
		}
		else
		{
			pMfpInfo->stream.without_index	= false;
		}

		(void)bmedia_player_init_config(&player_config);
		player_config.buffer 		= pMfpInfo->buffer;

		if (pMfpInfo->async)
		{
			player_config.cntx		= pMfpInfo;
			player_config.atom_ready	= P_MEDIA_ProbeAsync;
		}
		/* else { continue } */

		if(pMfpInfo->StreamInfo.video_stream_id && (stream_type == bstream_mpeg_type_vob || stream_type == bstream_mpeg_type_pes))
		{
			if(pMfpInfo->factory)
				player_config.factory = pMfpInfo->factory;
		}

		if(VK_strstr((char *)pszStreamUrl,"[dnp]") && (stream_type == bstream_mpeg_type_asf))
		{
			pMfpInfo->no_player = true;
			pMfpInfo->StreamInfo.support_trick  = TRUE; /* progressive down */
			pMfpInfo->StreamInfo.support_seek   = TRUE;
		}
		else
		{
			pMfpInfo->player = bmedia_player_create(pMfpInfo->fd, &player_config, &pMfpInfo->stream);
			if(pMfpInfo->player == NULL)
			{
				PrintError ("Can't use index for the media file\n");

				pMfpInfo->fd->seek(pMfpInfo->fd, 0, SEEK_SET);

				if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
				{
					pMfpInfo->no_player 			= true;
					pMfpInfo->StreamInfo.support_trick	= TRUE;
					/* 2008.07.08 - by Dylan, Check the seek support */
					pMfpInfo->StreamInfo.support_seek 	= TRUE;
				}
				else
				{
					pMfpInfo->no_player = true;
					pMfpInfo->StreamInfo.support_trick	= FALSE;
					/* 2008.07.08 - by Dylan, Check the seek support */
					pMfpInfo->StreamInfo.support_seek 	= pMfpInfo->fd->seek(pMfpInfo->fd, 0, SEEK_SET) != -1 ? TRUE : FALSE;
				}
			}
			else
			{
				if(pMfpInfo->seek!=BMEDIA_PLAYER_INVALID)
				{
					bmedia_player_seek(pMfpInfo->player, pMfpInfo->seek);
				}

				/* 2008.07.08 - by Dylan, Check the seek support */
				pMfpInfo->StreamInfo.support_seek = bmedia_player_seek(pMfpInfo->player, 0) != -1 ? TRUE : FALSE;

				/*if(pMfpInfo->seek2!=BMEDIA_PLAYER_INVALID)
				  {
				  bmedia_player_seek(pMfpInfo->player, pMfpInfo->seek2);
				  }*/

				if(pMfpInfo->direction != 0)
				{
					bmedia_filter_set_keyframeonly(pMfpInfo->filter, true);
				}
				/* else { continue } */

				(void)bmedia_player_set_direction(pMfpInfo->player, pMfpInfo->direction, pMfpInfo->time_scale, &pMfpInfo->decoder_mode);

				if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
				{
					pMfpInfo->no_player 			= true;
				}
				else
				{
					pMfpInfo->no_player = false;
				}

				pMfpInfo->StreamInfo.support_trick	= TRUE;
			}
		}
	}

	(void)VK_strncpy (pMfpInfo->szStreamUrl, (char *)pszStreamUrl, DI_MEDIA_STREAM_URL_LEN-1);
	pMfpInfo->opened	= true;
	pMfpInfo->ullPrevPosition	= 0;
	pMfpInfo->ullEosPosition	= 0;

#if defined (CONFIG_PLAYREADY)
/*************************
*	Parse & get the WRM Header
************************/
	HBOOL bHeaderExist = FALSE;
	HBOOL bCocktail = FALSE;

	if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF)
	{
		bHeaderExist = DI_STREAM_HTTP_GetASFWrmHeader(pszStreamUrl, &(pMfpInfo->StreamInfo.pucWrmHeader), (HUINT32*)&(pMfpInfo->StreamInfo.nWrmHeaderLen), &bCocktail);
		if(bHeaderExist == TRUE)
		{
			pMfpInfo->StreamInfo.is_drm = TRUE;
			if(bCocktail == TRUE)
			{
				pMfpInfo->StreamInfo.eDrmType = DI_MEDIA_DRM_TYPE_WMDRM;
			}
			else
			{
				pMfpInfo->StreamInfo.eDrmType = DI_MEDIA_DRM_TYPE_PLAYREADY;
			}
		}
	}
#endif

	PrintExit;

	return nRet;

error:

	if (pMfpInfo->buffer != NULL)
	{
		(void)bfile_buffer_destroy(pMfpInfo->buffer);
		pMfpInfo->buffer = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->buffer_cfg.buf != NULL)
	{
		(void) DD_MEM_Free (pMfpInfo->buffer_cfg.buf);
		pMfpInfo->buffer_cfg.buf = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->fd != NULL)
	{
		(void)bfile_stdio_read_detach(pMfpInfo->fd);
		pMfpInfo->fd = NULL;
	}
	/* else { continue } */

	if (probe != NULL)
	{
		if (stream != NULL)
		{
			(void)bmedia_probe_stream_free (probe, stream);
		}
		/* else { continue } */

		(void)bmedia_probe_destroy(probe);
	}
	/* else { continue } */

	if (pMfpInfo->filter != NULL)
	{
		if(pMfpInfo->audio != NULL)
		{
			(void)bmedia_filter_stream_close (pMfpInfo->filter, pMfpInfo->audio);
			pMfpInfo->audio	= NULL;
		}
		/* else { continue } */

		if(pMfpInfo->video != NULL)
		{
			(void)bmedia_filter_stream_close (pMfpInfo->filter, pMfpInfo->video);
			pMfpInfo->video	= NULL;
		}
		/* else { continue } */

		(void)bmedia_filter_stop (pMfpInfo->filter);
		(void)bmedia_filter_destroy (pMfpInfo->filter);
		pMfpInfo->filter	= NULL;
	}
	/* else { continue } */

	if (pMfpInfo->pipe_media != NULL)
	{
		(void)batom_pipe_destroy (pMfpInfo->pipe_media);
		pMfpInfo->pipe_media	= NULL;
	}
	/* else { continue } */

	if (pMfpInfo->factory != NULL)
	{
		(void)batom_factory_destroy (pMfpInfo->factory);
		pMfpInfo->factory	= NULL;
	}
	/* else { continue } */

	(void)bfile_async_shutdown();
#ifdef	CONFIG_VFIO
	if ( pMfpInfo->tHandle.ucAvailable )
	{
		(void)VFIO_Close (pMfpInfo->tHandle);
	}
	/* else { continue } */
#else
	if (pMfpInfo->fin != NULL)
	{
		(void)fclose (pMfpInfo->fin);
		pMfpInfo->fin	= NULL;
	}
	/* else { continue } */
#endif

	if (nRet == DI_ERR_OK)
	{
		nRet = DI_ERR_ERROR;
	}
	/* else { continue } */

	PrintExit;

	return nRet;
}

/**
 * @brief		Stream Probe¸¦ À§ÇØ ÇÒ´çÇÑ Resource¸¦ ÇØÁ¦ÇÑ´Ù.
 *
 * @param[in]	pMfpInfo 	Todo
 * @return		void
 * @see			DI_MEDIA_Probe
 */
static DI_ERR_CODE P_MEDIA_ProbeClose (stMediaInfo *pMfpInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if (!pMfpInfo->opened)
	{
		PrintExit;

		return DI_ERR_OK;
	}
	/* else { continue } */

	if (pMfpInfo->atom != NULL)
	{
		(void)batom_release (pMfpInfo->atom);
		pMfpInfo->atom = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->buffer != NULL)
	{
		(void)bfile_buffer_destroy(pMfpInfo->buffer);
		pMfpInfo->buffer = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->buffer_cfg.buf != NULL)
	{
		(void) DD_MEM_Free (pMfpInfo->buffer_cfg.buf);
		pMfpInfo->buffer_cfg.buf = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->player != NULL)
	{
		bmedia_player_destroy(pMfpInfo->player);
		pMfpInfo->player = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->fd != NULL)
	{
		(void)bfile_stdio_read_detach(pMfpInfo->fd);
		pMfpInfo->fd = NULL;
	}
	/* else { continue } */

	if (pMfpInfo->filter != NULL)
	{
		if(pMfpInfo->audio != NULL)
		{
			(void)bmedia_filter_stream_close (pMfpInfo->filter, pMfpInfo->audio);
			pMfpInfo->audio	= NULL;
		}
		/* else { continue } */

		if(pMfpInfo->video != NULL)
		{
			(void)bmedia_filter_stream_close (pMfpInfo->filter, pMfpInfo->video);
			pMfpInfo->video	= NULL;
		}
		/* else { continue } */

		(void)bmedia_filter_stop (pMfpInfo->filter);
		(void)bmedia_filter_destroy (pMfpInfo->filter);
		pMfpInfo->filter	= NULL;
	}
	/* else { continue } */

	if (pMfpInfo->pipe_media != NULL)
	{
		(void)batom_pipe_destroy (pMfpInfo->pipe_media);
		pMfpInfo->pipe_media	= NULL;
	}
	/* else { continue } */

	if (pMfpInfo->factory != NULL)
	{
		(void)batom_factory_destroy (pMfpInfo->factory);
		pMfpInfo->factory	= NULL;
	}
	/* else { continue } */

	(void)bfile_async_shutdown();

#ifdef	CONFIG_VFIO
	if ( pMfpInfo->tHandle.ucAvailable )
	{
		(void)VFIO_Close (pMfpInfo->tHandle);
	}
	/* else { continue } */
#else
	if (pMfpInfo->fin != NULL)
	{
		(void)fclose (pMfpInfo->fin);
		pMfpInfo->fin	= NULL;
	}
	/* else { continue } */
#endif

	/* 2008.12.01 by Dylan - struct should be cleared, this is the static define !!! */
	VK_memset(pMfpInfo, 0x0, sizeof(stMediaInfo));
	pMfpInfo->szStreamUrl[0]	= 0;
	pMfpInfo->opened			= FALSE;

	PrintExit;

	return nRet;
}

/**
 * @brief		Todo
 *
 * @param[in]	atom 	Todo
 * @param[in]	user 	Todo
 * @return		void
 * @see			DI_MEDIA_Probe
 */
static void P_MEDIA_ProbeFreeAtom (batom_t atom, void *user)
{
	void *ptr = *(void **)user;

	PrintEnter;

	PrintData ("Free Media %#lx|\n", (unsigned long)atom);

	/* s_MfpInfo.free_cnt++ */;
	BSTD_UNUSED(atom);

	if(ptr != NULL)
	{
		(void) DD_MEM_Free (ptr);
		ptr = NULL;
	}

	PrintExit;

	return;
}

/**
 * @brief		Todo
 *
 * @param[in]	cntx 	Todo
 * @param[in]	entry 	Todo
 * @return		void
 * @see			DI_MEDIA_Probe
 */
static void P_MEDIA_ProbeAsync (void *cntx, bmedia_player_entry *entry)
{
	stMediaInfo *pMfpInfo = cntx;
	void *b;
	batom_t atom;

	PrintEnter;

	if (pMfpInfo == NULL || entry == NULL)
	{
		return;
	}

	b = DD_MEM_Alloc(entry->length);
	if (b == NULL)
	{
		return;
	}

	pMfpInfo->alloc_cnt++;

	if(entry->atom)
	{
		batom_cursor cursor;
		int rc;

		batom_cursor_from_atom(&cursor, entry->atom);
		rc = batom_cursor_copy(&cursor, b, entry->length);
		if (rc != (int)entry->length)
		{
		}
		/* else { continue } */

		batom_release(entry->atom);
	}
	/* else { continue } */

	atom = batom_from_range(pMfpInfo->factory, b, entry->length, &P_MEDIA_Probe_AtomUser, &b);
	if (atom != NULL)
	{
		batom_pipe_push(pMfpInfo->pipe_media, atom);
	}
	/* else { continue } */

	PrintExit;

	return;
}
#if 1 /* FirstPts Detection */
static bmpeg2ts_parser_action
P_MEDIA_GettingTSPayload(bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *cursor, size_t len)
{
	b_test_pid *test=(b_test_pid *)pid;

	if(flags&BMPEG2TS_PCR_FLAG) {
		test->pcr_count++;
	}
	test->ts_bytes += len;
	return bmpeg2pes_parser_feed(&test->pes, flags, src, cursor, len);
}

static void
P_MEDIA_GettingPESPacket(void *packet_cnxt, batom_accum_t src, batom_cursor *payload, size_t len, const bmpeg2pes_atom_info *info)
{
	b_test_pid *test=(b_test_pid *)packet_cnxt;
	stMediaInfo *pMfp;

	pMfp = (stMediaInfo *)test->mfp_pointer;
	BSTD_UNUSED(src);
	test->pes_bytes += len;

	if(info->flags&BMPEG2PES_PTS_VALID) {
		test->pts_count++;
		if(pMfp)
		{
			if(test->pts_count==1)
			{
				pMfp->StreamInfo.bProbedFirstPts = TRUE;
				pMfp->StreamInfo.unProbeFirstPts = info->pts;
				PrintDebug ("%s(%d) PES_ID %u pts_count %d ..... getting firstpts %u\n"
				,__FUNCTION__
				,__LINE__
				,info->pes_id
				,test->pts_count
				,pMfp->StreamInfo.unProbeFirstPts);
			}
		}
	}

	batom_cursor_skip(payload, len);
	return;
}
#endif
static DI_ERR_CODE P_MEDIA_InstallFirstPts (HUINT32 unMediaId, HUINT32 unFirstPts)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;
/*
	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}
*/
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		// (void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	s_tMediaInstance[unMediaId].unFirstPts = unFirstPts;
	s_tMediaInstance[unMediaId].bPtsInstalled = TRUE;

	PrintError("[%s] Debug - First Pts Installed : %u \n", __func__,unFirstPts);
//	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;

}



static void P_MEDIA_FirstPtsCallback (HUINT32 unMediaId, HUINT32 unFirstPts)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;

	eDiRet = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return;
	}

	/****************************************************
	 * SEEK ÀÌÈÄ¿¡µµ STARTED Event ´Â emit µÇ¾î¾ß ÇÑ´Ù.
	 ****************************************************/
	if(s_tMediaInstance[unMediaId].bPtsInstalled == FALSE)
	{
		P_MEDIA_InstallFirstPts(unMediaId, unFirstPts);
	}
	return;
}

static DI_ERR_CODE P_MEDIA_RegisterFirstPtsCB (HUINT32 unMediaId, DI_MEDIA_FirstPtsCallback_t firstPtsCallback)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

/*	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	} */

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
//		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	s_tEventFirstPtsCallback[unMediaId] = firstPtsCallback;

//	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;

}

static DI_ERR_CODE P_MEDIA_UnregisterFirstPtsCB (HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;
/*	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}*/

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
//		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	s_tEventFirstPtsCallback[unMediaId] = NULL;

	// (void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;

}

static void P_MEDIA_AdjustTime(HUINT32 *pts)
{
	HUINT32 TrickSpeed;

	DI_MEDIA_MP_GetSpeed(0, &TrickSpeed);

	switch(TrickSpeed)
	{
		case DI_MEDIA_TRICK_2RWSTEP:		// FR x2
			*pts = (HUINT32)((HINT32)*pts * (-1)) * 2;
			break;
		case DI_MEDIA_TRICK_4RWSTEP:		// FR x4
			*pts = (HUINT32)((HINT32)*pts * (-1)) * 4;
			break;
		case DI_MEDIA_TRICK_8RWSTEP:		// FR x8
			*pts = (HUINT32)((HINT32)*pts * (-1)) * 8;
			break;
		case DI_MEDIA_TRICK_14RWSTEP:		// FR x16
			*pts = (HUINT32)((HINT32)*pts * (-1)) * 16;
			break;
		case DI_MEDIA_TRICK_15RWSTEP:		// FR x32
			*pts = (HUINT32)((HINT32)*pts * (-1)) * 32;
			break;
		case DI_MEDIA_TRICK_PAUSE:			// pause
		case DI_MEDIA_TRICK_1FFSTEP:		// FF x1
		case DI_MEDIA_TRICK_2FFSTEP:		// FF x2
		case DI_MEDIA_TRICK_4FFSTEP:		// FF x4
			break;
		case DI_MEDIA_TRICK_8FFSTEP:		// FF x8
			*pts = *pts * 8;
			break;
		case DI_MEDIA_TRICK_14FFSTEP:		// FF x16
			*pts = *pts * 16;
			break;
		case DI_MEDIA_TRICK_15FFSTEP:		// FF x32
			*pts = *pts * 32;
			break;
		default :
			PrintError("[%s] Not Supported Play Speed [%d]\n", __func__, TrickSpeed);
			break;
	}
}

/*
 * @notes: CONFIG_SEND_CHANGEPLAYTIME_PERIOD_250MS (http://svn.humaxdigital.com:3000/issues/64851)
 * 	- change playtime event¸¦ 250 milli second¸¶´Ù(VK_TASK_Sleep(250)¿¡ ¸ÂÃß¾î) º¸³½´Ù. (for HTML5 videobackend)
 *	- ±âÁ¸ ¹æ½Ä È£È¯À» À§ÇØ playtime paramÀº sec´ÜÀ§·Î º¸³»¹Ç·Î browser_media¿¡¼­ msec´ÜÀ§·Î °»½Å
*/
static void P_MEDIA_PlayTimeTask(void *param)
{
	NEXUS_Error				tNexusError = NEXUS_SUCCESS;
	HUINT32					nTempPlayTime[DI_MEDIA_MAX_NUM] = {0,};
	HUINT32					unPtsSnapShot[DI_MEDIA_MAX_NUM] = {0,};

	int i = 0;

	(void)UNUSED_PARAMETER(param);

	while(1)
	{
		for(i=0;i<DI_MEDIA_MAX_NUM;i++)
		{
			if((s_tEventCallback[i] != NULL) &&
				((s_tMediaInstance[i].mediastate == DI_MEDIA_STATE_PLAY)
				 || (s_tMediaInstance[i].mediastate == DI_MEDIA_STATE_TRICK) || (s_tMediaInstance[i].mediastate == DI_MEDIA_STATE_PAUSE)))
			{
				if(s_tMediaInstance[i].playback == NULL)
				{
					continue;
				}
				else
				{
					tNexusError = b_play_getpts (s_tMediaInstance[i].playback, &unPtsSnapShot[i]);
					if(tNexusError != NEXUS_SUCCESS)
					{
						/* Nothing to do */
						VK_TASK_Sleep(10);
						continue;
					}
					else
					{
						P_MEDIA_AdjustTime(&unPtsSnapShot[i]);
						/* firstStarted Stream */
						if(s_tMediaInstance[i].bPtsInstalled == FALSE)
						{
							PrintDebug ("## NOT PREPARED PTS\n\n");
							if(s_tEventFirstPtsCallback[i] != NULL)
								s_tEventFirstPtsCallback[i] ((HUINT32)i, unPtsSnapShot[i]);
						}
						/* else : we had got a first pts for this stream */
						/* Get Playback Play Position */
						nTempPlayTime[i] = bmedia_pts2time(unPtsSnapShot[i] - s_tMediaInstance[i].unFirstPts, BMEDIA_TIME_SCALE_BASE);
					}
				}
				/** Get Playback Play Position */
#if defined(CONFIG_SEND_CHANGEPLAYTIME_PERIOD_250MS)
#else
				if(s_nCurrentPlayTime[i] != (nTempPlayTime[i]/1000))
#endif
				{
					if(s_tMediaInstance[i].bPtsInstalled == FALSE)
					{
						if(s_tEventFirstPtsCallback[i] != NULL)
							s_tEventFirstPtsCallback[i] ((HUINT32)i, nTempPlayTime[i]);
					}

					if((s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_NONE)
						|| ((nTempPlayTime[i]  == 0) && (s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0)))
					{
						PrintDebug("[%s] Not Update Play Time\n", __func__);
					}
					else
					{
						s_nCurrentPlayTime[i] = (nTempPlayTime[i] /1000);
					}
					if((s_nCurrentPlayTime[i] != 0) && (s_tEventCallback[i] != NULL) &&
					((s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_UPDATE_FULLTIME) || (s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0)))
					{
						s_tEventCallback[i](i, DI_MEDIA_EVENT_CHANGEPLAYTIME, s_nCurrentPlayTime[i]);
					}
				}
			}
		}
		VK_TASK_Sleep(250);
	}
}

#if SUPPORT_AUDIOCODEC_LPCM
int P_MEDIA_MakeWaveHeader(uint8_t *buf, size_t buf_size, unsigned bits_per_sample, unsigned sample_rate, unsigned nchannels)
{
    unsigned block_align = (bits_per_sample/8)*nchannels;
    unsigned byte_rate = block_align*sample_rate;

	PrintEnter;

    if(buf_size<sizeof(tTempWavHeader)) {
        /* not enough size for rhe header */
        return -1;
    }

    BKNI_Memcpy(buf, tTempWavHeader, sizeof(tTempWavHeader));

    /* populate variable fields */
    buf[22] = B_GET_BITS(nchannels, 7, 0);
    buf[23] = B_GET_BITS(nchannels, 15, 8);

    buf[24] = B_GET_BITS(sample_rate, 7, 0);
    buf[25] = B_GET_BITS(sample_rate, 15, 8);
    buf[26] = B_GET_BITS(sample_rate, 23, 16);
    buf[27] = B_GET_BITS(sample_rate, 31, 24);

    buf[28] = B_GET_BITS(byte_rate, 7, 0);
    buf[29] = B_GET_BITS(byte_rate, 15, 8);
    buf[30] = B_GET_BITS(byte_rate, 23, 16);
    buf[31] = B_GET_BITS(byte_rate, 31, 24);

    buf[32] = B_GET_BITS(block_align, 7, 0);
    buf[33] = B_GET_BITS(block_align, 15, 8);

    buf[34] = B_GET_BITS(bits_per_sample, 7, 0);
    buf[35] = B_GET_BITS(bits_per_sample, 15, 8);

	PrintExit;

    return sizeof(tTempWavHeader);
}

DI_ERR_CODE P_MEDIA_SetupMemoryStream(HUINT32 unMediaId, DI_MEDIA_STREAM_TYPE_e eStreamType, HUINT32 SampleRate, HUINT32 BitsPerSample, HINT8 IsStereo)
{
	NEXUS_PlaypumpSettings		playpumpSettings;
	NEXUS_Error					rc = NEXUS_SUCCESS;
	bool						header = TRUE;
	size_t						buffer_size = 0;
	void						*buffer = NULL;
	HUINT32						unSampleRate=48000;		/* sampling rate */
	HUINT32						unBitsPerSample=16;		/* bits per sample */
	HINT8						nChannel = 0;				/* number of channels, 2 is stereo */

	PrintEnter;

	if(SampleRate != 0)
	{
		unSampleRate = SampleRate;
	}

	if(BitsPerSample != 0)
	{
		unBitsPerSample = BitsPerSample;
	}

	if(IsStereo != FALSE)
	{
		nChannel = 2;
	}
	else
	{
		nChannel = 1;
	}

	PrintData("unSampleRate=%d, unBitsPerSample=%d, nChannel=%d\n", unSampleRate, unBitsPerSample, nChannel);

    (void)NEXUS_Playpump_GetSettings(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &playpumpSettings);
    playpumpSettings.transportType = P_MEDIA_ConvertTransportTypeToNexus(eStreamType); /* we convert raw LPCM to WAV */
    rc = NEXUS_Playpump_SetSettings(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &playpumpSettings);
    if(rc != NEXUS_SUCCESS)
    {
		PrintError("[%s] Error - Fail NEXUS_Playpump_SetSettings:%d\n", __func__, rc);
		goto Error;
    }
    else
    {
	    (void)NEXUS_Playpump_Start(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);

		do
		{
			if (NEXUS_Playpump_GetBuffer(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &buffer, &buffer_size))
			{
				PrintError("playpump error\n");
				break;
			}

			buffer_size -= buffer_size%2; /* make buffer 2 bytes alligned */

			if (buffer_size == 0)
			{
				VK_TASK_Sleep(200);
				continue;
			}

			/* The first call to get_buffer will return the entire XPT playback buffer.
			If we use it, we're going to have to wait until the descriptor is complete,
			and then we're going to underflow. So use a max size. */
			if (buffer_size > MEDIA_PLAYPUMP_MAX_READ)
				buffer_size = MEDIA_PLAYPUMP_MAX_READ;

			if(header)
			{ /* to play LPCM data we first need to proovide metadata (sampling rate, number of channels, etc) by constructint the wave header */
				int header_len;
				PrintData("Call make_wave_header\n");
				header_len = P_MEDIA_MakeWaveHeader(buffer, buffer_size, unBitsPerSample , unSampleRate , nChannel );
				if(header_len<0)
				{
					PrintError("not enough space for the header\n");
					goto Error;
				}
				rc = NEXUS_Playpump_ReadComplete(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], 0, header_len);
				if(rc != NEXUS_SUCCESS)
				{
					PrintError("Fail 1\n");
				}
				header = false;
			}
		}
		while(header == true);
    }

	PrintExit;

	return DI_ERR_OK;
Error:
	return DI_ERR_ERROR;
}

DI_ERR_CODE P_MEDIA_StopMemoryStream(HUINT32 unMediaId)
{
	DI_ERR_CODE		nRet = DI_ERR_OK;

	PrintEnter;

	if(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] != NULL)
	{
	    (void)NEXUS_Playpump_Stop(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
	}

	PrintExit;

	return nRet;
}

DI_ERR_CODE P_MEDIA_FileOpenforMemoryStream(HINT8 *pcFilePath, HINT8	*pbIsDLNAFileHandle, void **phFileHandle)
{
	DI_ERR_CODE		nRet = DI_ERR_OK;
	DI_MEDIA_DLNA_FILEDESC		tFileDescription;
	DI_MEDIA_DLNA_FILEHANDLE	hDLNAHandle=NULL;
    FILE	*pFileHandle=NULL;

	PrintEnter;

	VK_MEM_Memset(&tFileDescription, 0x0, sizeof(DI_MEDIA_DLNA_FILEDESC));

    if(NULL != VK_strstr((char *)pcFilePath, "[DLNA]"))
    {
		/* DLNAÀÎ °æ¿ì File OpenÇÏ´Â ºÎºÐ */
		if(s_tMEDIADLNAFileFn.Open != NULL)
		{
			tFileDescription.url = (char *)pcFilePath;
			if(NULL != VK_strstr((char *)pcFilePath, "http://"))
			{
				tFileDescription.access_type = DI_MEDIA_DLNA_FILEACCESS_HTTP;
			}

			nRet = s_tMEDIADLNAFileFn.Open(&tFileDescription, &hDLNAHandle);
			if ( nRet != DI_ERR_OK )
			{
				PrintError ("%s (%d) Error>  %s open Error, please check file path\n",__FUNCTION__,__LINE__,tFileDescription.url);
			}
			else
			{
				PrintData ("%s (%d) callback handle=0x%x\n",__FUNCTION__,__LINE__, hDLNAHandle);
				*pbIsDLNAFileHandle = TRUE;
				*phFileHandle = hDLNAHandle;
			}
		}
		else
		{
			PrintData ("%s (%d) Unregist Open Function\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
		}
    }
    else
    {
		/* DLNA°¡ ¾Æ´Ñ °æ¿ì File OpenÇÏ´Â ºÎºÐ */
	    pFileHandle = fopen((char *)pcFilePath, "rb");
	    if(pFileHandle == NULL)
	    {
			nRet = DI_ERR_ERROR;
	    }
	    else
	    {
			*phFileHandle = pFileHandle;
			nRet = DI_ERR_OK;
	    }

		*pbIsDLNAFileHandle = FALSE;
    }

	PrintExit;

	return nRet;
}

DI_ERR_CODE P_MEDIA_FileReadforMemoryStream(void *phFileHandle, HINT8	bIsDLNA, HINT32	*pnRead_Length, size_t *pBufferSize, void *pDataBuffer)
{
	DI_ERR_CODE		nRet = DI_ERR_OK;
	HUINT64			lCurOffset = 0;
	HINT64			lFileSize = 0;

	PrintEnter;

	if(bIsDLNA == TRUE)
	{
		/* DLNAÀÎ °æ¿ì Data ReadÇÏ´Â ºÎºÐ */
		if(s_tMEDIADLNAFileFn.Read != NULL)
		{
			if(s_tMEDIADLNAFileFn.Tell(phFileHandle, &lCurOffset) != 0)
			{
				nRet = DI_ERR_ERROR;
			}
			else
			{
				if(s_tMEDIADLNAFileFn.GetFileSize(phFileHandle, &lFileSize) != 0)
				{
					nRet = DI_ERR_ERROR;
				}
				else
				{
					if((lFileSize - lCurOffset) <= 0)
					{
						*pBufferSize = 0;
					}
					else if((lFileSize - lCurOffset) < *pBufferSize)
					{
						*pBufferSize = (HUINT32)(lFileSize - lCurOffset);
					}

//					PrintData ("%s (%d) buffer_size=%d\n",__FUNCTION__,__LINE__, *pBufferSize);

					if(*pBufferSize != 0)
					{
						if(s_tMEDIADLNAFileFn.Read(phFileHandle, pDataBuffer, *pBufferSize, (HUINT32 *)pnRead_Length) != 0)
						{
							nRet = DI_ERR_ERROR;
						}
					}
					else
					{
						*pnRead_Length = 0;
					}
				}
			}

//			PrintData("%s (%d) readCnt(%d)\n",__FUNCTION__,__LINE__, *pnRead_Length);
		}
		else
		{
			PrintData ("%s (%d) Unregist Read Function\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
		}
	}
	else
	{
		/* DLNA°¡ ¾Æ´Ñ °æ¿ì Data ReadÇÏ´Â ºÎºÐ */
		*pnRead_Length=fread(pDataBuffer, 1, *pBufferSize, phFileHandle);
		nRet = DI_ERR_OK;
	}

	PrintExit;

	return nRet;
}

DI_ERR_CODE P_MEDIA_FileCloseforMemoryStream(void *phFileHandle, HINT8	bIsDLNA)
{
	DI_ERR_CODE		nRet = DI_ERR_OK;

	PrintEnter;

	if(bIsDLNA == TRUE)
	{
		/* DLNAÀÎ °æ¿ì File CloseÇÏ´Â ºÎºÐ */
		if((s_tMEDIADLNAFileFn.Close != NULL) && (phFileHandle != NULL))
		{
			PrintError("Close hDLNAHandle\n");
			if(s_tMEDIADLNAFileFn.Close(phFileHandle) != 0)
			{
				nRet = DI_ERR_ERROR;
			}
			else
			{
				nRet = DI_ERR_OK;
				phFileHandle = NULL;
			}
		}
		else
		{
			PrintData ("%s (%d) Unregist Close Function\n",__FUNCTION__,__LINE__);
		}
	}
	else
	{
		/* DLNA°¡ ¾Æ´Ñ °æ¿ì File CloseÇÏ´Â ºÎºÐ */
		if(phFileHandle != NULL)
		{
			fclose(phFileHandle);
			phFileHandle = NULL;
		}
	}

	PrintExit;

	return nRet;
}

static void P_MediaPlaypumpStreamTask(void* param)
{
	DI_MEDIA_MSG						audMsg;
	int									nRet=0;
	HINT32								nMsgTimeout = 0;
	size_t								buffer_size = 0;
	void								*buffer = NULL;
	uint8_t								*TempBuffer = NULL;
	uint8_t								TempPacket = 0;
	int									nCount=0;
	int									nRead_Length = 0;
	HUINT32								unMediaId=0;
	DI_MEDIA_STREAM_TYPE_e				StreamType=DI_MEDIA_MPEG_TYPE_UNKNOWN;
	HINT8								IsPlaypumpStop=FALSE;
	DI_AUDIO_STREAM_INFO_t				tAudioStreamInfo;
    unsigned							last_depth = 0;
    unsigned							depth_count = 0;
	NEXUS_PlaypumpStatus				tPlaypumpStatus;
	HINT8								bIsDLNAFileHandle = FALSE;
	HINT8								*pcFilePath = NULL;
	HUINT32								sampleRate=0;
	HUINT32								bitsPerSample=0;
	HINT8								isStereo=FALSE;
	HUINT32								unCurrentPlayTime = 0;
	HUINT32								unPreviousPlayTime = 0;
	void								*phFileHandle = NULL;
 	HUINT32								nTempCurrent_Time = 0;
 	HUINT32								nTempPrevious_Time = 0;
    struct timeval tCurrent_Time;
    struct timeval tPrevious_Time;

	NEXUS_AudioDecoderHandle			hAudioDecode=NULL;
	NEXUS_AudioDecoderStatus			tAudioStatus;

	BSTD_UNUSED(param);

	VK_MEM_Memset(&tAudioStreamInfo, 0x0, sizeof(DI_AUDIO_STREAM_INFO_t));
	VK_MEM_Memset(&tAudioStatus, 0x0, sizeof(NEXUS_AudioDecoderStatus));

	while(1)
	{
		if(s_eMediaPlaypumpStreamState != DI_MEDIA_PLAYPUMPPLAY_PLAY)
		{
			nMsgTimeout = 200;
		}
		else
		{
			nMsgTimeout = 20;
		}

		nRet = VK_MSG_ReceiveTimeout(s_ulMediaPlaypumpStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), nMsgTimeout);
		if(nRet != VK_OK)
		{
			if(nRet != VK_TIMEOUT)
			{
				PrintError ("%s (%d) Error>  Error VK_MSG_ReceiveTimeout\n",__FUNCTION__,__LINE__);
				s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_NOT_READY;
				if(pcFilePath != NULL)
				{
					DD_MEM_Free(pcFilePath);
					pcFilePath = NULL;
				}
			}
			else
			{
				if(s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PLAY)
				{
					unCurrentPlayTime += nMsgTimeout;
				}
			}
		}
		else
		{
			switch( audMsg.ulSender)
			{
				case DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP:
					PrintDebug("[%s] [%d] DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP\n", __FUNCTION__,__LINE__);
					switch( audMsg.ulEvent )
					{
						case DI_MEDIA_PLAYPUMPPLAY_SETUP:
							{
								/* Play¸¦ À§ÇØ È¯°æÀ» SetupÇÏ´Â °úÁ¤(Format¼³Á¤, File Openµîµî) */
								PrintDebug("[%s] [%d] DI_MEDIA_PLAYPUMPPLAY_SETUP\n", __FUNCTION__,__LINE__);
								unMediaId = audMsg.ulParam2;
								StreamType = audMsg.ulParam3;
								sampleRate = audMsg.ulParam4;
								bitsPerSample = audMsg.ulParam5;
								isStereo = audMsg.ulParam6;
								pcFilePath = VK_StrDup(audMsg.pucParam);
								if(audMsg.pucParam != NULL)
								{
									DD_MEM_Free(audMsg.pucParam);
									audMsg.pucParam = NULL;
								}

								if(pcFilePath == NULL)
								{
									PrintError("[%s] pcFilePath is null.\n", __func__);
									s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_NOT_READY;
									if ( NULL != s_tEventCallback[unMediaId] )
									{
										s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
									}
									/* else - do nothing */
								}
								else
								{
									PrintDebug("[%s] [%d] Url=%s\n", __FUNCTION__,__LINE__, pcFilePath);
									(void)P_MEDIA_SetupMemoryStream(unMediaId, StreamType, sampleRate, bitsPerSample, isStereo);
								    /* quick and dirty read from file into memory, then do playpump from memory */

									nRet = P_MEDIA_FileOpenforMemoryStream(pcFilePath, &bIsDLNAFileHandle, &phFileHandle);
									if ( nRet != DI_ERR_OK )
									{
										s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_NOT_READY;
										if(pcFilePath != NULL)
										{
											DD_MEM_Free(pcFilePath);
											pcFilePath = NULL;
										}

										if ( NULL != s_tEventCallback[unMediaId] )
										{
											s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
										}
										/* else - do nothing */
									}
									else
									{
										s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_SETUP;
									}

									if(s_tEventCallback[unMediaId] != NULL)
									{
										s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
									}
								}

								unCurrentPlayTime = 0;
								unPreviousPlayTime = 0;

								break;
							}
						case DI_MEDIA_PLAYPUMPPLAY_PLAY:
							{
								/* Play°¡ ½ÃÀÛµÇ´Â ºÎºÐ */
								PrintDebug("[%s] [%d] DI_MEDIA_PLAYPUMPPLAY_PLAY\n", __FUNCTION__,__LINE__);
								if(s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_SETUP)
								{
									PrintData ("%s (%d) DI_MEDIA_PLAYPUMPPLAY_SETUP\n",__FUNCTION__,__LINE__);
									s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_PLAY;

									if(s_tEventCallback[unMediaId] != NULL)
									{
										s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_START, 0);
									}
#if defined(CONFIG_IPTUNER)
									if(DI_IPTUNER_GetUnderflowState() != 33)
									{
										DI_IPTUNER_SetUnderflowState(33);
									}
#endif
									s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
								}
								else
								{
									PrintData ("%s (%d) DI_MEDIA_PLAYPUMPPLAY_NOT_READY\n",__FUNCTION__,__LINE__);
								}

								unCurrentPlayTime = 0;
								unPreviousPlayTime = 0;

								break;
							}
						case DI_MEDIA_PLAYPUMPPLAY_STOP:
							{
								/* Play Á¾·á */
								PrintDebug("[%s] [%d] DI_MEDIA_PLAYPUMPPLAY_STOP\n", __FUNCTION__,__LINE__);
								if((s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_PLAY) ||
									(s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_SETUP))
								{
									s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
								}
								IsPlaypumpStop = FALSE;
								break;
							}
						case DI_MEDIA_PLAYPUMPPLAY_NOT_READY:
							{
								/* Play¸¦ À§ÇØ ¼³Á¤µÈ ResourceÇØÁ¦ */
								PrintDebug("[%s] [%d] DI_MEDIA_PLAYPUMPPLAY_NOT_READY\n", __FUNCTION__,__LINE__);
								if((s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_STOP) ||
									(s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_SETUP))
								{
									s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_NOT_READY;
								}

								if(pcFilePath != NULL)
								{
									DD_MEM_Free(pcFilePath);
									pcFilePath = NULL;
								}

								unMediaId = 0;
								StreamType = 0;
								sampleRate = 0;
								bitsPerSample = 0;
								isStereo = FALSE;
								unCurrentPlayTime = 0;
								unPreviousPlayTime = 0;

								break;
							}
						default:
							break;
					}
					break;
				default:
					PrintDebug("[01;36m[%s] Receive Unknown Msg![00m\r\n",__FUNCTION__);
					break;
			}
		}

		if((s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_PLAY) && (phFileHandle == NULL))
		{
			s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
		}
		else if(s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_PLAY)
		{
			if(s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PLAY)
			{
				/* Play TimeÀ» App¿¡ Àü´ÞÇÏ´Â ºÎºÐ */
				if(((unCurrentPlayTime/1000) != (unPreviousPlayTime/1000)) || (unPreviousPlayTime == 0))
				{
					if ( NULL != s_tEventCallback[unMediaId] )
					{
						s_tEventCallback[unMediaId]((HUINT32)unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, (unCurrentPlayTime/1000));
					}

					unPreviousPlayTime = unCurrentPlayTime;
					/* else - do nothing */
				}
			}

			/* Playpump¿¡ ³²Àº Buffer·® Check */
			if (NEXUS_Playpump_GetBuffer(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &buffer, &buffer_size))
			{
				PrintError("[%s] playpump error.\n", __func__);
				if ( NULL != s_tEventCallback[unMediaId] )
				{
					s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
				}
				/* else - do nothing */

				s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
			}

			buffer_size -= buffer_size%2; /* make buffer 2 bytes alligned */

			if (buffer_size == 0)
			{
				continue;
			}

			/* The first call to get_buffer will return the entire XPT playback buffer.
			If we use it, we're going to have to wait until the descriptor is complete,
			and then we're going to underflow. So use a max size. */
			if (buffer_size > MEDIA_PLAYPUMP_MAX_READ)
				buffer_size = MEDIA_PLAYPUMP_MAX_READ;

			gettimeofday(&tPrevious_Time, NULL);

			/* Playpump¿¡ FeedingÇÒ Data¸¦ ÀÐ´Â ºÎºÐ */
			nRet = P_MEDIA_FileReadforMemoryStream(phFileHandle, bIsDLNAFileHandle, &nRead_Length, &buffer_size, buffer);
			if ( nRet != DI_ERR_OK )
			{
				s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
				if ( NULL != s_tEventCallback[unMediaId] )
				{
					s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
				}
				/* else - do nothing */
			}
			else
			{
				s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_PLAY;
			}

			gettimeofday(&tCurrent_Time, NULL);

			nTempPrevious_Time = (tPrevious_Time.tv_sec*1000) + (tPrevious_Time.tv_usec/1000);
			nTempCurrent_Time = (tCurrent_Time.tv_sec*1000) + (tCurrent_Time.tv_usec/1000);

			unCurrentPlayTime += nTempCurrent_Time - nTempPrevious_Time;
			unCurrentPlayTime += 5; /* ÇÔ¼ö ¼öÇà ¹× ½Ã°£ ¾ò¾î¿À´Â °úÁ¤Áß¿¡ ¼öÇàµÇ´Â ½Ã°£°ª º¸Á¤ */
			if(nRead_Length<0)
			{
				PrintError("[%s] read error\n", __func__);
				if ( NULL != s_tEventCallback[unMediaId] )
				{
					s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
				}
				/* else - do nothing */
				s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
			}
			else if(nRead_Length>0)
			{
				TempBuffer = buffer;
				for(nCount=0;nCount<nRead_Length;nCount+=2)
				{ /* bytes swap payload to convert from BE to LE data */
					TempPacket = TempBuffer[0];
					TempBuffer[0] = TempBuffer[1];
					TempBuffer[1] = TempPacket;
					TempBuffer+=2;
				}

				if(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] != NULL)
				{
					(void)NEXUS_Playpump_ReadComplete(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], 0, nRead_Length);
				}
				else
				{
					PrintData ("%s (%d) Error> Play pump Handle is null\n",__FUNCTION__,__LINE__);
					if ( NULL != s_tEventCallback[unMediaId] )
					{
						s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
					}
					/* else - do nothing */
					s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
				}
			}
			else /* nRead_Length==0 */
			{
				if(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] != NULL)
				{
					(void)NEXUS_Playpump_GetStatus(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &tPlaypumpStatus);

					if((tPlaypumpStatus.fifoDepth == last_depth) && (tPlaypumpStatus.fifoDepth<MEDIA_PLAYPUMP_MAX_READ))
					{
						depth_count++;
						PrintData("Increase depth_count(%d)\n", depth_count);
					}
					else
					{
						depth_count = 0;
						last_depth = tPlaypumpStatus.fifoDepth;
					}

					if(depth_count >= 10)
					{
						if(s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PLAY)
						{
							if((unCurrentPlayTime/1000) != (unPreviousPlayTime/1000))
							{
								if ( NULL != s_tEventCallback[unMediaId] )
								{
									s_tEventCallback[unMediaId]((HUINT32)unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, (unCurrentPlayTime/1000));
								}
								/* else - do nothing */
							}
						}

					    /* then wait for the audio fifo to stay static for a period of time. */
					    for(last_depth=0,depth_count=0;depth_count<5;)
					    {
					        VK_TASK_Sleep(100);
							unCurrentPlayTime += 100;
							(void)DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_MAIN, &hAudioDecode);
					        NEXUS_AudioDecoder_GetStatus(hAudioDecode, &tAudioStatus);
					        PrintData("audio fifo %d\n", tAudioStatus.fifoDepth);
					        if(tAudioStatus.fifoDepth == last_depth)
					        {
					            depth_count++;
					        }
					        else
					        {
					            depth_count = 0;
					            last_depth = tAudioStatus.fifoDepth;
					        }
					    }
					    PrintData("audio decode is done\n");

						VK_TASK_Sleep(500);
						unCurrentPlayTime += 500;

						if ( NULL != s_tEventCallback[unMediaId] )
						{
							s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ENDOFSTREAM, 0);
						}
						/* else - do nothing */

						s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
						PrintData("end of file\n");
					}
				}
				else
				{
					PrintData ("%s (%d) Error> Play pump Handle is null\n",__FUNCTION__,__LINE__);
					if ( NULL != s_tEventCallback[unMediaId] )
					{
						s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
					}
					/* else - do nothing */
					s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
				}

#if 0
				(void)DRV_AUDIO_GetStreamInfo(0, &tAudioStreamInfo);

				if(tAudioStreamInfo.ulRemainDataSize == last_depth)
				{
					depth_count++;
					PrintError("Increase depth_count(%d)\n", depth_count);
				}
				else
				{
					depth_count = 0;
					last_depth = tAudioStreamInfo.ulRemainDataSize;
				}

				if(depth_count >= 10)
				{
					PrintError("end of file\n");
				}
#endif
			}
		}
		else if(s_eMediaPlaypumpStreamState == DI_MEDIA_PLAYPUMPPLAY_STOP)
		{
			/* Play°¡ StopµÇ°í Resource¸¦ ÇØÁ¦ÇÏ´Â ºÎºÐÀ¸·Î ÇÑ¹ø Á¾·á°úÁ¤ÀÌ ¿Ï·áµÇ°í ³ª¼­ ´Ù½Ã ¿äÃ»½Ã ¼öÇàÇÏÁö ¾Êµµ·Ï ÇÔ. */
			if((IsPlaypumpStop == FALSE) && (phFileHandle != NULL))
			{
				(void)P_MEDIA_StopMemoryStream(unMediaId);

				nRet = P_MEDIA_FileCloseforMemoryStream(phFileHandle, bIsDLNAFileHandle);
				if ( nRet != DI_ERR_OK )
				{
					s_eMediaPlaypumpStreamState = DI_MEDIA_PLAYPUMPPLAY_STOP;
					if ( NULL != s_tEventCallback[unMediaId] )
					{
						s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
					}
					/* else - do nothing */
				}
				else
				{
					phFileHandle = NULL;
				}

				IsPlaypumpStop = TRUE;
			}

			if(s_tEventCallback[unMediaId] != NULL)
			{
				s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_STOP, 0);
			}

			s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_STOP;
		}
	}
}
#endif

#if __________task__________
#endif
#ifdef SUPPORT_OLD_MEDIA
static void MediaStreamTask(void* param)
{
	DI_MEDIA_MSG	audMsg;
	unsigned long 		ulFileHandle;

	int nRet;

	BSTD_UNUSED(param);

	while(1)
	{
		nRet = VK_MSG_Receive(s_ulMediaStreamMId, &audMsg, sizeof(DI_MEDIA_MSG));
		if(nRet != VK_OK)
		{
			PrintError("[%s] [%d]\n", __FUNCTION__,__LINE__);
			continue;
		}

		switch( audMsg.ulSender)
		{
			case DI_MEDIA_PLAY_AUDIO_FROM_FILE:
				PrintDebug("[01;36m[%s] Receive Msg [ %lu ] from Audio PlayFileStream![00m\r\n",__FUNCTION__,audMsg.ulEvent);
				ulFileHandle = audMsg.ulStreamHandle;

				switch( audMsg.ulEvent )
				{
					case DI_MEDIA_FILE_STREAM_PLAY:
						PrintDebug("[01;36m[%s] Start file play![00m\r\n",__FUNCTION__);
						break;
					case DI_MEDIA_FILE_STREAM_STOP:
						PrintDebug("[01;36m[%s] Stop file play![00m\r\n",__FUNCTION__);
						P_MEDIA_StopAudioFileStream();
						break;
					default:
						break;
				}
				break;
			case DI_MEDIA_PLAY_VIDEO_FROM_FILE:	//only video ?? video+audio ??
				PrintDebug("[01;36m[%s] Receive Msg [ %lu ] from Video(+AUDIO) PlayFileStream![00m\r\n",__FUNCTION__,audMsg.ulEvent);
				ulFileHandle = audMsg.ulStreamHandle;

				switch( audMsg.ulEvent )
				{
					case DI_MEDIA_FILE_STREAM_PLAY:
						PrintDebug("[01;36m[%s] Start file play![00m\r\n",__FUNCTION__);
						break;
					case DI_MEDIA_FILE_STREAM_STOP:
						PrintDebug("[01;36m[%s] Stop file play![00m\r\n",__FUNCTION__);
						P_MEDIA_StopVideoFileStream();
						break;
					default:
						break;
				}
				break;

			case DI_MEDIA_PLAY_AUDIO_FROM_MEMORY:
//				PrintDebug("[01;36m[%s] Receive Msg from PlayMemoryStream!\r\n",__FUNCTION__);

				switch( audMsg.ulEvent )
				{
					case DI_MEDIA_MEMORY_STREAM_PLAY:
						if(stMediaInstance.audioStatus < DI_MEDIA_AUDIO_PLAY)
						{
							PrintDebug("[01;36m[%s] Audio status is not to play..!!! [00m\r\n",__FUNCTION__);
							break;
						}

						P_MEDIA_PlayAudioMemory(
									(unsigned char *)audMsg.pucParam,
									audMsg.ulParam2,
									audMsg.ulParam3);
						break;
					case DI_MEDIA_MEMORY_STREAM_STOP:
						PrintDebug("[01;36m[%s] Receive Msg DI_MEDIA_MEMORY_STREAM_STOP![00m\r\n",__FUNCTION__);
						P_MEDIA_StopAudioMemory();
						break;

					case DI_MEDIA_MEMORY_STREAM_MONITOR:
						P_MEDIA_MonitorAudioMemory();
						break;

					default:
						break;
				}
				break;
			default:
				PrintDebug("[01;36m[%s] Receive Unknown Msg![00m\r\n",__FUNCTION__);
				break;
		}

		VK_TASK_Sleep(50);
	}
}

static void MediaEventTask(void* param)
{
	unsigned long event;
	BSTD_UNUSED(param);
	int err = 0;
	while(1)
	{
		err = VK_EVENT_Receive(s_ulMediaEventEId, &event);
//		PrintDebug("[01;31m[%s] Receive event %d[00m\n\r",__FUNCTION__,(int)event);
		if(err)
		{
			PrintDebug("[MediaEventTask] (%d) in VK_EVENT_Receive()\n\r", err);
		}
		VK_TASK_Sleep(1000);
	}
}

// Callback when space becomes available. param2 is settings.dataCallback.param
static void MediaAudPbDataCallback(void *pParam1, int param2)
{
	BSTD_UNUSED(pParam1);
	BSTD_UNUSED(param2);

//	PrintDebug("[01;31m[%s] Receive Data callback - %d[00m\n\r",__FUNCTION__,param2);

//	VK_EVENT_Send(s_ulMediaEventEId, (unsigned long)pParam1);
}

static void MediaPlayFileEOFCallback(void *context, int param)
{
	DI_MEDIA_MSG	audMsg;
	int result;

	BSTD_UNUSED(param);		//stMediaInstance.mediaType
	BSTD_UNUSED(context);	//NULL

	PrintDebug("[01;31m[%s] Receive EOF callback - %d[00m\n\r",__FUNCTION__,param);

	if(param == DI_MEDIA_PLAYPUMP_AUDIO_MAIN)
	{
		audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_FILE;
		audMsg.ulEvent = DI_MEDIA_FILE_STREAM_STOP;
	}
	else if(param == DI_MEDIA_PLAYPUMP_DIVX)
	{
		audMsg.ulSender = DI_MEDIA_PLAY_VIDEO_FROM_FILE;
		audMsg.ulEvent = DI_MEDIA_FILE_STREAM_STOP;
	}

	result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
	if(result != VK_OK)
	{
		PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,result);
	}
}
#endif
#if __________drv__________
#endif

#define __________DRV_COMMON_DI_MEDIA__________
DRV_Error DRV_MEDIA_Init(void)
{
	int result;
#ifdef SUPPORT_OLD_MEDIA
	int i=0;
#endif

	/* Create DRM Temp Directory for sample.hd devcert.dat*/
	mkdir((char *)&g_dcertPath,S_IRUSR | S_IWUSR );

	VK_memset(&s_tStreamInfo,0,sizeof(s_tStreamInfo));
	VK_memset(s_bPlayTimerMode,0,sizeof(s_bPlayTimerMode));

#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)
VK_MSG_Create(4*sizeof(unsigned long)*8 , sizeof(unsigned long)*4, "StatusMQ",
	&sulStatusMonitorThreadQ, VK_SUSPENDTYPE_FIFO);
VK_TASK_Create(DRV_MEDIA_StatusMonitorThread, USER_PRIORITY5, 128*1024,
	"StatusMT", NULL, &sulStatusMonitorThreadT, FALSE);
VK_TASK_Start(sulStatusMonitorThreadT);
#endif

#ifdef SUPPORT_OLD_MEDIA
	VK_memset(&stMediaInstance,0,sizeof(DI_MediaInstance));
	VK_memset(&stMediaStreamInfo,0,sizeof(DI_MEDIA_STREAM_PLAY_INFO));

#if 0
	stMediaInstance.audioPlayback= NEXUS_AudioPlayback_Open(0, NULL);
	if ( NULL == stMediaInstance.audioPlayback )
	{
		PrintError("[%s]Unable to open playback channel %d\n",__FUNCTION__, i);
		return DRV_ERR;
	}
#endif

	for(i=0;i<DI_MEDIA_PLAYPUMP_MAX;i++)
	{
		stMediaInstance.playpump[i]= NULL;
	}

	stMediaInstance.playback = NEXUS_Playback_Create();
	s_tStreamInfo[0].audiodecodertype = DI_MEDIA_AUDIO_DECODER_MAIN;
	s_tStreamInfo[0].videodecodertype = DI_MEDIA_VIDEO_DECODER_MAIN;

	//Media Stream Task & Msg
	result = VK_MSG_Create(DI_MEDIA_MSG_SIZE, sizeof(DI_MEDIA_MSG),
						"MSMsgQ", &s_ulMediaStreamMId, VK_SUSPENDTYPE_FIFO);
	if(result != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

    	result = VK_TASK_Create( MediaStreamTask,
								AUDIO_CLIP_TASK_PRIORITY,
								AUDIO_CLIP_TASK_STACK_SIZE,
								"MSTask",
								NULL,
								&s_ulMediaStreamTId,
								FALSE);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMediaStreamTId);

	//Media Event Task & Event
	result = VK_EVENT_Create(&s_ulMediaEventEId, "MEEvent");
	if(result != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

#if SUPPORT_AUDIOCODEC_LPCM
	//Media Playpump Stream Task & Msg
	result = VK_MSG_Create(DI_MEDIA_MSG_SIZE, sizeof(DI_MEDIA_MSG),
						"MSMsgQ", &s_ulMediaPlaypumpStreamMId, VK_SUSPENDTYPE_FIFO);
	if(result != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	result = VK_TASK_Create( P_MediaPlaypumpStreamTask,
							MEDIA_STREAMPLAY_TASK_PRIORITY,
							MEDIA_STREAMPLAY_TASK_STACK_SIZE,
							"MSTask",
							NULL,
							&s_ulMediaPlaypumpStreamTId,
							FALSE);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMediaPlaypumpStreamTId);
#endif

    	result = VK_TASK_Create( MediaEventTask,
								AUDIO_CLIP_TASK_PRIORITY,
								AUDIO_CLIP_TASK_STACK_SIZE,
								"METask",
								NULL,
								&s_ulMediaEventTId,
								FALSE);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMediaEventTId);
#endif

	result = VK_SEM_Create(&s_ulMediaSemId,"di_media", DEFAULT_SUSPEND_TYPE);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	result = VK_TASK_Create(P_MEDIA_PlayTimeTask,
						MEDIA_TIME_TASK_PRIORITY,
						MEDIA_TIME_TASK_STACK_SIZE,
						"MediaPlayTime",
						NULL,
						&s_ulMediaPlayTimeTaskId,
						FALSE);
	if(result != VK_OK)
	{
		PrintError("[DRV_PVR_PLAY_Init] Error(0x%x) in VK_TASK_Create()\n\r", result);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMediaPlayTimeTaskId);

	VK_MEM_Memset(s_tMediaInstance, 0x0, sizeof(s_tMediaInstance));

/*************************
*	 PLAYREADY
*************************/
#if defined (CONFIG_PLAYREADY)
	NEXUS_Error nResult;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings nexusConfig;

	VK_MEM_Memset(&dmaSettings, 0, sizeof(NEXUS_DmaSettings));

	dmaHandle = NEXUS_Dma_Open(0, &dmaSettings);
	if(dmaHandle == NULL)
	{
		PrintError("NEXUS_Dma_Open failed!\n");
		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	keyWmdrm = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(keyWmdrm == NULL)
	{
	   PrintError("NEXUS_Security_AllocateKeySlot failed\n");
	   return DRV_ERR;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

	nexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eWMDrmPd;
	nexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eEcb;
	nexusConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
	nexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
	nexusConfig.enableExtKey		= TRUE;
	nexusConfig.enableExtIv 		= TRUE;

	nResult = NEXUS_Security_ConfigAlgorithm(keyWmdrm, &nexusConfig);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ConfigAlgorithm failed!\n");
		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	keyPlayready = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(keyPlayready == NULL)
	{
		PrintError("NEXUS_Security_AllocateKeySlot failed\n");
		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

	nexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
	nexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
	nexusConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
	nexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
	nexusConfig.enableExtKey		= TRUE;
	nexusConfig.enableExtIv 		= TRUE;
	nexusConfig.aesCounterSize		= NEXUS_SecurityAesCounterSize_e64Bits;

	nResult = NEXUS_Security_ConfigAlgorithm(keyPlayready, &nexusConfig);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ConfigAlgorithm failed!\n");
		return DRV_ERR;
	}

	#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_MEDIA_Uninit, DRV_MEDIA_Reinit);
	#endif

#endif
	return DRV_OK;
}

#ifdef SUPPORT_OLD_MEDIA
#define __________DRV_OLD_DI_MEDIA__________

NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackPidChannel(DI_MEDIA_PLAYPUMP mediaType ,void* decoderHandle  )
{
	NEXUS_PlaybackPidChannelSettings openPidSettings;

	NEXUS_Playback_GetDefaultPidChannelSettings(&openPidSettings);
#if (NEXUS_VERSION >= 1250)
	openPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif

	if(mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN)
	{
		openPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
		openPidSettings.pidTypeSettings.audio.primary = (NEXUS_AudioDecoderHandle)decoderHandle;

		if(stMediaInstance.mediaType == DI_MEDIA_PLAYPUMP_DIVX)
		{
			stMediaInstance.audioPidChannel = NEXUS_Playback_OpenPidChannel(stMediaInstance.playback, 2, &openPidSettings);	//temp
		}
		else
		{
			stMediaInstance.audioPidChannel = NEXUS_Playback_OpenPidChannel(stMediaInstance.playback, 1, &openPidSettings);	//temp
		}

		return stMediaInstance.audioPidChannel;
	}
	else if(mediaType == DI_MEDIA_PLAYPUMP_DIVX)
	{
		openPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
		openPidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eDivx311;
		openPidSettings.pidTypeSettings.video.decoder = (NEXUS_VideoDecoderHandle)decoderHandle;
		openPidSettings.pidTypeSettings.video.index = true;
		stMediaInstance.videoPidChannel = NEXUS_Playback_OpenPidChannel(stMediaInstance.playback, 1, &openPidSettings);	//temp

		return stMediaInstance.videoPidChannel;
	}

	return NULL;
}

NEXUS_StcChannelHandle DRV_MEDIA_GetPlaybackStcChannelHandle(DI_MEDIA_PLAYPUMP mediaType )
{
	 NEXUS_StcChannelSettings stcSettings;

	if((mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN) && (stMediaInstance.mediaType != DI_MEDIA_PLAYPUMP_DIVX))
	{
		return NULL;
	}

	NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
	stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
	stcSettings.modeSettings.pcr.pidChannel = 0;//stMediaInstance.videoPidChannel;
	NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);

	return DRV_DEMUX_GetStcChannelHandle();
}

DRV_Error DRV_MEDIA_PlayBeepTone( HBOOL controlFlag)
{
	DI_MEDIA_MSG	audMsg;
	DRV_Error eDRVError = DRV_OK;
	int result;

	BERR_Code errCode = BERR_SUCCESS;
	NEXUS_AudioPlaybackStartSettings settings;

	if(VK_SEM_Get(s_ulMediaSemId) == VK_OK)
	{

		if(controlFlag == TRUE)
		{
			PrintDebug("[%s] %d START\n\r",__FUNCTION__,__LINE__);
#if 0 /* disable DRV_AUDIO_SetInput call */
			DRV_AUDIO_SetInput( NEXUS_AudioPlayback_GetConnector(stMediaInstance.audioPlayback) , TRUE );
#endif

			NEXUS_AudioPlayback_GetDefaultStartSettings(&settings);
			settings.sampleRate = 48000;
			settings.bitsPerSample = 16;
			settings.signedData = true;
			settings.stereo = true;
			settings.dataCallback.callback = MediaAudPbDataCallback;
			settings.dataCallback.context = stMediaInstance.audioPlayback;
			settings.dataCallback.param = (int)s_ulMediaEventEId;	//??????

			errCode = NEXUS_AudioPlayback_Start(stMediaInstance.audioPlayback, &settings);

			stMediaStreamInfo.pbuffStartPoint = (unsigned char *)DD_MEM_Alloc(audioToneSamplesize);
			VK_memcpy(stMediaStreamInfo.pbuffStartPoint ,audioToneSamples , audioToneSamplesize);
			stMediaStreamInfo.esDataSize = audioToneSamplesize;
//			stMediaStreamInfo.loopCount = DI_MEDIA_INFINITIVE_LOOP;
			stMediaStreamInfo.loopCount = 5000;	//test
			stMediaInstance.audioStatus = DI_MEDIA_AUDIO_PLAY;

			audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;
			audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_PLAY;
			audMsg.pucParam = (unsigned char *)stMediaStreamInfo.pbuffStartPoint;
			audMsg.ulParam2 = stMediaStreamInfo.esDataSize;
			audMsg.ulParam3 = stMediaStreamInfo.loopCount;
		}
		else
		{
			PrintDebug("[%s] %d STOP\n\r",__FUNCTION__,__LINE__);
			audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_MEMORY;
			audMsg.ulEvent = DI_MEDIA_MEMORY_STREAM_STOP;
		}

		result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(result != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,result);
			eDRVError = DRV_ERR;
			goto RETURN;
		}
	}
RETURN:
	VK_SEM_Release(s_ulMediaSemId);
	return eDRVError;
}

DRV_Error DRV_MEDIA_PlayAudioFileStream( HBOOL controlFlag)
{
	DI_MEDIA_MSG	audMsg;
	DRV_Error eDRVError = DRV_OK;
	int result;

	if(VK_SEM_Get(s_ulMediaSemId) == VK_OK)
	{
		if(controlFlag == TRUE)
		{
			NEXUS_PlaybackSettings playbackSettings;
			const char *fname = "/usr/bin/test.mp3";
			stMediaInstance.mediaType = DI_MEDIA_PLAYPUMP_AUDIO_MAIN;

			stMediaStreamInfo.file = NEXUS_FilePlay_OpenPosix(fname, NULL);

			if (!stMediaStreamInfo.file)
			{
				PrintError("[%s] can't open file:%s\n", __FUNCTION__,fname);
				eDRVError = DRV_ERR;
				goto RETURN;
			}

			NEXUS_Playback_GetSettings(stMediaInstance.playback, &playbackSettings);
			DRV_PVR_GetPlaypumpHandle(&stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_AUDIO_MAIN]);
			playbackSettings.playpump = stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_AUDIO_MAIN];
			playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eEs;
			playbackSettings.endOfStreamCallback.callback = MediaPlayFileEOFCallback;
			playbackSettings.endOfStreamCallback.context = NULL;
			playbackSettings.endOfStreamCallback.param= stMediaInstance.mediaType;
			NEXUS_Playback_SetSettings(stMediaInstance.playback, &playbackSettings);

			DRV_AUDIO_Start(0 ,DI_AUDFORMAT_MP3);
			DRV_AUDIO_Start(2 ,DI_AUDFORMAT_MP3);

			NEXUS_Playback_Start(stMediaInstance.playback, stMediaStreamInfo.file, NULL);

			audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_FILE;
			audMsg.ulEvent = DI_MEDIA_FILE_STREAM_PLAY;
		}
		else
		{
			audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_FILE;
			audMsg.ulEvent = DI_MEDIA_FILE_STREAM_STOP;
		}

		result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(result != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,result);
			eDRVError = DRV_ERR;
			goto RETURN;
		}
	}
RETURN:
	VK_SEM_Release(s_ulMediaSemId);
	return eDRVError;
}

DRV_Error DRV_MEDIA_PlayFileStream( HBOOL controlFlag)
{
	DI_MEDIA_MSG	audMsg;
	DRV_Error eDRVError = DRV_OK;
	int result;

	if(VK_SEM_Get(s_ulMediaSemId) == VK_OK)
	{
		if(controlFlag == TRUE)
		{
			NEXUS_PlaybackSettings playbackSettings;
			const char *fname = "/usr/bin/test.avi";

			stMediaInstance.mediaType = DI_MEDIA_PLAYPUMP_DIVX;

			stMediaStreamInfo.file = NEXUS_FilePlay_OpenPosix(fname, NULL);

			if (!stMediaStreamInfo.file)
			{
				PrintError("[%s] can't open file:%s\n", __FUNCTION__,fname);
				eDRVError = DRV_ERR;
				goto RETURN;
			}

			NEXUS_Playback_GetSettings(stMediaInstance.playback, &playbackSettings);
			DRV_PVR_GetPlaypumpHandle(&stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_DIVX]);
			playbackSettings.playpump = stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_DIVX];
			playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eAvi;
			playbackSettings.endOfStreamCallback.callback = MediaPlayFileEOFCallback;
			playbackSettings.endOfStreamCallback.context = NULL;
			playbackSettings.endOfStreamCallback.param= stMediaInstance.mediaType;
			NEXUS_Playback_SetSettings(stMediaInstance.playback, &playbackSettings);

			DRV_AUDIO_Start(0 ,DI_AUDFORMAT_MP3);
			DRV_AUDIO_Start(2 ,DI_AUDFORMAT_MP3);

			DI_VIDEO_Start(0,0x00000020);	//DI_VIDEO_CODEC_H261 //temp

			result=DI_VIDEO_Show(0,1);
			PrintError("[%s] DI_VIDEO_Show:%d\n", __FUNCTION__,result);

			result = NEXUS_Playback_Start(stMediaInstance.playback, stMediaStreamInfo.file, NULL);
			if (result) {
				PrintError("[%s] NEXUS_Playback_Start:%d\n", __FUNCTION__,result);
				/* NEXUS_Playback_Start may fail if the AVI file has no index. If so, reopen without an index and try again. */
				NEXUS_FilePlay_Close(stMediaStreamInfo.file);
				stMediaStreamInfo.file = NEXUS_FilePlay_OpenPosix(fname, NULL);
				NEXUS_Playback_Start(stMediaInstance.playback, stMediaStreamInfo.file, NULL);
			}

			audMsg.ulSender = DI_MEDIA_PLAY_AUDIO_FROM_FILE;
			audMsg.ulEvent = DI_MEDIA_FILE_STREAM_PLAY;
		}
		else
		{
			audMsg.ulSender = DI_MEDIA_PLAY_VIDEO_FROM_FILE;
			audMsg.ulEvent = DI_MEDIA_FILE_STREAM_STOP;
		}

		result = VK_MSG_SendTimeout(s_ulMediaStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(result != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,result);
			eDRVError = DRV_ERR;
			goto RETURN;
		}
	}
RETURN:
	VK_SEM_Release(s_ulMediaSemId);
	return eDRVError;
}
#endif

#define __________DRV_NEW_DI_MEDIA__________
/**
 * @brief		Media ¼³Á¤À» Á¾·áÇÑ´Ù.
 *
 * @param		None
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_Init
 */
DRV_Error DRV_MEDIA_Term(void)
{
	int result;

#ifdef SUPPORT_OLD_MEDIA
	int i=0;

	/** Media Event Task & Event */
	(void)VK_TASK_Stop(s_ulMediaEventTId);

   	result = VK_TASK_Destroy(s_ulMediaEventTId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	result = VK_EVENT_Destroy(s_ulMediaEventEId);
	if(result != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	/** Media Stream Task & Msg */
	VK_TASK_Stop(s_ulMediaStreamTId);

   	result = VK_TASK_Destroy( s_ulMediaStreamTId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

#if SUPPORT_AUDIOCODEC_LPCM
	/** Media Playpump Stream Task & Msg */
	VK_TASK_Stop(s_ulMediaPlaypumpStreamTId);

   	result = VK_TASK_Destroy( s_ulMediaPlaypumpStreamTId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}
#endif
/*
	result = VK_MSG_Delete(s_ulMediaStreamMId, "MSMsgQ", sizeof(DI_MEDIA_MSG));
	if(result != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}
*/
	for(i=0;i<DI_MEDIA_PLAYPUMP_MAX;i++)
	{
//		(void)NEXUS_Playpump_Close(stMediaInstance.playpump[i]);
		if(stMediaInstance.playpump[i] != NULL)
		{
			DRV_PVR_ReleasePlaypumpHandle(stMediaInstance.playpump[i]);
			stMediaInstance.playpump[i] = NULL;
		}
	}

	(void)NEXUS_AudioPlayback_Close(stMediaInstance.audioPlayback);

	VK_memset(&stMediaInstance,0,sizeof(DI_MediaInstance));
#endif
	/** Media Time Event Task & Event */
	(void)VK_TASK_Stop(s_ulMediaPlayTimeTaskId);

   	result = VK_TASK_Destroy( s_ulMediaPlayTimeTaskId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	result = VK_SEM_Release(s_ulMediaSemId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	return DRV_OK;
}

/**
 * @brief		OpenµÈ Stc ChannelÀ» Media Type¿¡ ¸Â°Ô ¼³Á¤ ÈÄ ¾ò´Â´Ù.
 *
 * @param[in]	unMediaId	media Id
 * @param[in]	mediaType	media type
 * @return		NEXUS_StcChannelHandle
 * @see
 */
NEXUS_StcChannelHandle DRV_MEDIA_MP_GetPlaybackStcChannelHandle(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, DI_MEDIA_PLAYPUMP mediaType)
{
	NEXUS_StcChannelSettings stcSettings;
	DI_MEDIA_AUDIO_TYPE_e eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_NONE;
	NEXUS_PlaybackPidChannelSettings openPidSettings;

	HUINT32 unMediaId;
	HUINT32 i =0;

	PrintEnter;

	NEXUS_Playback_GetDefaultPidChannelSettings(&openPidSettings);
#if (NEXUS_VERSION >= 1250)
	openPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif


	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		if(s_tStreamInfo[i].videodecodertype == eVideoDecoderType)
		{
			unMediaId = i;
			break;
		}
	}

	if(i >= DI_MEDIA_MAX_NUM)
	{
		if(eAudioDecoderType != DI_MEDIA_AUDIO_DECODER_NONE)
		{
			eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_MAIN;
		}

		for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
		{
			if(s_tStreamInfo[i].audiodecodertype == eTempAudioDecoderType)
			{
				unMediaId = i;
				break;
			}
		}
	}

	if(i >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] invalid eVideoDecoderType and eAudioDecoderType.\n", __func__);
	}
	else
	{

		NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0;
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		stcSettings.modeSettings.pcr.pidChannel = NULL;

		if((mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN) && (s_tMediaInstance[unMediaId].mediaType != DI_MEDIA_PLAYPUMP_DIVX))
		{
			NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
			return NULL;
		}

		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE)
		{
			NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
			return NULL;
		}

		/* pcr_pid */
		if( s_tMediaInstance[unMediaId].pcrPidChannel != NULL ) /* duplication check */
		{
			stcSettings.modeSettings.pcr.pidChannel = s_tMediaInstance[unMediaId].pcrPidChannel;
		}
#if 1
		/**
			pcr pid ¿Í video/audio pid °¡ µ¿ÀÏÇÑ °æ¿ì´Â Á¦¿ÜÇÔ.
		    stcChannel ¿¡ pidChannel Setting ½Ã Video/Audio PID¿Í PCR PID °¡ µ¿ÀÏ ÇÑ°æ¿ì
			nexus_playback.c ÀÇ NEXUS_Playback_OpenPidChannel ¿¡¼­ PID Áßº¹ Ã³¸® ÄÚµå ³»¿¡¼­
			play_pid->cfg.pidSettings.pidType = pSettings->pidSettings.pidType; (±âÁ¸ ÄÚµå) °¡ ¾Æ´Ñ
			play_pid->cfg = *pSettings; (º¯°æ ÄÚµå) ·ÎÀÇ º¯°æÀÌ ÇÊ¿ä.
			ÇöÀç ¸ðµç BRCM Driver ¹Ý¿µ¿¡ ¾î·Á¿òÀÌ ÀÖÀ¸¹Ç·Î Workaround Ã³¸®ÇÔ.
		*/
		else if(s_tStreamInfo[unMediaId].pcr_pid != 0x1fff &&
		s_tStreamInfo[unMediaId].pcr_pid != s_tStreamInfo[unMediaId].video_stream_id &&
		s_tStreamInfo[unMediaId].pcr_pid != s_tStreamInfo[unMediaId].audio_stream_id)
    	{
	        stcSettings.modeSettings.pcr.pidChannel =
				NEXUS_Playback_OpenPidChannel (s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].pcr_pid, &openPidSettings);
			s_tMediaInstance[unMediaId].pcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
		}
#else
        else if( s_tStreamInfo[unMediaId].pcr_pid == s_tStreamInfo[unMediaId].video_stream_id )
        {
            if (s_tMediaInstance[unMediaId].videoPidChannel)
				stcSettings.modeSettings.pcr.pidChannel = s_tMediaInstance[unMediaId].videoPidChannel;
            else
           	{
				stcSettings.modeSettings.pcr.pidChannel =
					NEXUS_Playback_OpenPidChannel (s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].pcr_pid, &openPidSettings);
				s_tMediaInstance[unMediaId].pcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
            }
        }
		else if ( s_tStreamInfo[unMediaId].pcr_pid == s_tStreamInfo[unMediaId].audio_stream_id )
        {
            if (s_tMediaInstance[unMediaId].audioPidChannel)
				stcSettings.modeSettings.pcr.pidChannel = s_tMediaInstance[unMediaId].audioPidChannel;
            else
            {
				stcSettings.modeSettings.pcr.pidChannel =
					NEXUS_Playback_OpenPidChannel (s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].pcr_pid, &openPidSettings);
				s_tMediaInstance[unMediaId].pcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
            }
        }
        else
        {
        	if(s_tStreamInfo[unMediaId].pcr_pid != 0x1fff)
        	{
		        stcSettings.modeSettings.pcr.pidChannel =
					NEXUS_Playback_OpenPidChannel (s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].pcr_pid, &openPidSettings);
				s_tMediaInstance[unMediaId].pcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
			}
        }
#endif

#if SUPPORT_USE_STC_AUTOMODE_FIRSTAVAILABLE
		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)
		{
			switch(stcSettings.modeSettings.Auto.behavior)
			{
				case NEXUS_StcChannelAutoModeBehavior_eFirstAvailable:
					{
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
						PrintData("[%s] Current Mode: FirstAvailable, Modify Mode: FirstAvailable\n", __func__);
						break;
					}
				case NEXUS_StcChannelAutoModeBehavior_eVideoMaster:
					{
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
						PrintData("[%s] Current Mode: Video Master, Modify Mode: FirstAvailable\n", __func__);
						break;
					}
				case NEXUS_StcChannelAutoModeBehavior_eAudioMaster:
					{
						stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
						PrintData("[%s] Current Mode: Audio Master, Modify Mode: FirstAvailable\n", __func__);
						break;
					}
				default:
					PrintError("[%s] unknown mode %d\n", __func__, stcSettings.modeSettings.Auto.behavior);
					break;
			}
		}
		else
		{
			stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
			PrintData("[%s] Modify Mode: Video Master\n", __func__);
		}
#endif
		stcSettings.modeSettings.Auto.transportType = P_MEDIA_ConvertTransportTypeToNexus(s_tStreamInfo[unMediaId].stream_type);
#ifdef CONFIG_MEDIA_SSTR
		if(stcSettings.modeSettings.Auto.transportType == NEXUS_TransportType_eMp4Fragment) /* for SSTR */
		{
			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
		}
#endif
		NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	}

	PrintExit;

	return DRV_DEMUX_GetStcChannelHandle();
}

/**
 * @brief 		media Àç»ýÀ» À§ÇØ Video Pid ChannelÀ» ¾ò´Â´Ù.
 *
 * @param[in]	unMediaId				media Id
 * @param[in]	eVideoCodec				Video Codec
 * @param[in]	VideoDecoderHandle		Video Decoder Handle
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_GetPlaybackAudioPidChannel
 */
NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackVideoPidChannel(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_VIDEO_FORMAT_e eVideoCodec, void *VideoDecoderHandle)
{
	NEXUS_PlaybackPidChannelSettings openPidSettings;
	HUINT32 unMediaId = 0;
	HUINT32 i =0;

	PrintEnter;

	if(VideoDecoderHandle == NULL)
	{
		PrintError("[%s] VideoDecoderHandle is null.\n", __func__);
		return NULL;
	}

	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		if(s_tStreamInfo[i].videodecodertype == eVideoDecoderType)
		{
			unMediaId = i;
			break;
		}
	}

	if(i >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] invalid eVideoDecoderType.\n", __func__);
	}
	else
	{
		if(s_tMediaInstance[unMediaId].videoPidChannel != NULL)
		{
			(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].videoPidChannel);
			s_tMediaInstance[unMediaId].videoPidChannel = NULL;
		}

		/** Nexus Pid ChannelÀ» OpenÇÑ´Ù. */
		NEXUS_Playback_GetDefaultPidChannelSettings(&openPidSettings);
		openPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
#if (NEXUS_VERSION >= 1250)
		openPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
		openPidSettings.pidTypeSettings.video.codec = P_MEDIA_ConvertMediaVideoCodecToNexus(eVideoCodec);
		openPidSettings.pidTypeSettings.video.decoder = (NEXUS_VideoDecoderHandle)VideoDecoderHandle;
		openPidSettings.pidTypeSettings.video.index = true;
		s_tMediaInstance[unMediaId].videoPidChannel = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].video_stream_id, &openPidSettings);
		s_tMediaInstance[unMediaId].vDecoder = (NEXUS_VideoDecoderHandle)VideoDecoderHandle;
	}

	PrintExit;

	return s_tMediaInstance[unMediaId].videoPidChannel;
}

/**
 * @brief		media Àç»ýÀ» À§ÇØ Audio Pid ChannelÀ» ¾ò´Â´Ù.
 *
 * @param[in]	unMediaId				media Id
 * @param[in]	eAudioCodec				Audio Codec
 * @param[in]	eDecoderType			Audio Decoder Handle Type
 * @param[in]	AudioDecoderHandle		Audio Decoder Handle
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_GetPlaybackVideoPidChannel
 */
NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackAudioPidChannel(DI_MEDIA_AUDIO_FORMAT_e eAudioCodec, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, void *AudioDecoderHandle)
{
	NEXUS_PlaybackPidChannelSettings openPlaybackPidSettings;
	NEXUS_PlaypumpOpenPidChannelSettings openPlaypumpPidSettings;

	DI_MEDIA_AUDIO_TYPE_e eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_NONE;
	NEXUS_PidChannelHandle audioPidChannel = NULL;
#if 0				/* Di audio module¿äÃ»»çÇ×¿¡ ÀÇÇØ »ç¿ëÇÏÁö ¾ÊÀ½. secondary settingÀº ÀÌÈÄ¿¡ audio pid channel settingÇÔ¼ö¸¦ ÅëÇØ SettingÇÔ. */
	NEXUS_Error		nNexusError = NEXUS_SUCCESS;
#endif
	HUINT32 unMediaId = 0;
	HUINT32 i =0;
	HUINT32 j =0;
	PrintEnter;

	NEXUS_Playback_GetDefaultPidChannelSettings(&openPlaybackPidSettings);
	NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&openPlaypumpPidSettings);
#if (NEXUS_VERSION >= 1250)
	openPlaybackPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
	openPlaypumpPidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif

	if(AudioDecoderHandle == NULL)
	{
		PrintError("[%s] AudioDecoderHandle is null.\n", __func__);
		return NULL;
	}

	if(eAudioDecoderType != DI_MEDIA_AUDIO_DECODER_NONE)
	{
		eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_MAIN;
	}

	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		if(s_tStreamInfo[i].audiodecodertype == eTempAudioDecoderType)
		{
			unMediaId = i;
			break;
		}
	}

	if(i >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] invalid eAudioDecoderType.\n", __func__);
	}
	else
	{
		if(eAudioDecoderType == DI_MEDIA_AUDIO_DECODER_MAIN)
		{
			/** Nexus Pid ChannelÀ» OpenÇÑ´Ù. */
			for(j = 0; j < s_tStreamInfo[unMediaId].ucTotalAudioTrack ; j++)
			{
				/*if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] != NULL)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[j]);
					s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NULL;
				}*/
				openPlaybackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
				openPlaybackPidSettings.pidSettings.pidTypeSettings.audio.codec = P_MEDIA_ConvertMediaAudioCodecToNexus(s_tStreamInfo[unMediaId].stAudioTrack[j].audio_codec);
				openPlaybackPidSettings.pidTypeSettings.audio.primary = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
				if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] == NULL)
				{
					if((eAudioCodec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE) && (s_tStreamInfo[unMediaId].video_codec == DI_MEDIA_VIDEO_FORMAT_NONE))
					{
							s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NEXUS_Playpump_OpenPidChannel(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id, &openPlaypumpPidSettings);
					}
					else
					{
							s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id, &openPlaybackPidSettings);
					}
				}
				if(s_tStreamInfo[unMediaId].audio_stream_id == s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id)
				{
					audioPidChannel = s_tMediaInstance[unMediaId].audioPidChannel = s_tMediaInstance[unMediaId].audioTrackPidChannels[j];
				}
			}
		}
		else if(eAudioDecoderType == DI_MEDIA_AUDIO_DECODER_SUB)
		{
			/** Nexus Pid ChannelÀ» OpenÇÑ´Ù. */
			for(j = 0; j < s_tStreamInfo[unMediaId].ucTotalAudioTrack ; j++)
			{
				/*if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] != NULL)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[j]);
					s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NULL;
				}*/
				openPlaybackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
				openPlaybackPidSettings.pidSettings.pidTypeSettings.audio.codec = P_MEDIA_ConvertMediaAudioCodecToNexus(s_tStreamInfo[unMediaId].stAudioTrack[j].audio_codec);
				openPlaybackPidSettings.pidTypeSettings.audio.primary = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
				if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] == NULL)
				{
					s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id, &openPlaybackPidSettings);
				}
				if(s_tStreamInfo[unMediaId].audio_stream_id == s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id)
				{
					audioPidChannel = s_tMediaInstance[unMediaId].audioSubPidChannel = s_tMediaInstance[unMediaId].audioTrackPidChannels[j];
				}
			}
		}
		else
		{
#if 0				/* Di audio module¿äÃ»»çÇ×¿¡ ÀÇÇØ »ç¿ëÇÏÁö ¾ÊÀ½. secondary settingÀº ÀÌÈÄ¿¡ audio pid channel settingÇÔ¼ö¸¦ ÅëÇØ SettingÇÔ. */
			/** OpenµÈ Pid Channel¿¡ secondary Audio HandleÀ» Ãß°¡ÇÑ´Ù. */
			nNexusError = NEXUS_Playback_GetPidChannelSettings(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioPidChannel, &pidSettings);
			if(nNexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Error > Fail NEXUS_Playback_GetPidChannelSettings.\n", __func__);
				PrintExit;
				return NULL;
			}
			pidSettings.pidTypeSettings.audio.secondary = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
			nNexusError = NEXUS_Playback_SetPidChannelSettings(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioPidChannel, &pidSettings);
			if(nNexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Error > Fail NEXUS_Playback_SetPidChannelSettings.\n", __func__);
			}
#endif
			audioPidChannel = s_tMediaInstance[unMediaId].audioPidChannel;
		}
		s_tMediaInstance[unMediaId].aDecoder = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
	}
	PrintDebug("%s(%d) Info> Created audioPidChannel (0x%x) pid(0x%x)\n",__FUNCTION__,__LINE__,audioPidChannel,s_tStreamInfo[unMediaId].audio_stream_id);

	PrintExit;

	return audioPidChannel;
}

#if defined(CONFIG_FAST_AUDIO_CHANGE)
DRV_Error DRV_MEDIA_GetPlaybackHandle(NEXUS_PlaybackHandle *hPlaybackHandle)
{
	if(hPlaybackHandle == NULL)
	{
		PrintError("[%s] ERROR : hPlaybackHandle is NULL \n", __func__);
		return DRV_ERR;
	}

	*hPlaybackHandle = s_tMediaInstance[s_ulMediaId].playback;

	return DRV_OK;
}

DRV_Error DRV_MEDIA_GetAudioTrackPidChannel(HUINT32 ulPIDChannelIndex, NEXUS_PidChannelHandle *hPidChannel)
{
	if(hPidChannel == NULL)
	{
		PrintError("[%s] ERROR : hPidChannel is NULL \n", __func__);
		return DRV_ERR;
	}

#if defined(CONFIG_MHEG_SUPPORT) && defined(CONFIG_ICS)
	*hPidChannel = s_tMediaInstance[s_ulMediaId].audioPidChannel;
#else
	*hPidChannel = s_tMediaInstance[s_ulMediaId].audioTrackPidChannels[ulPIDChannelIndex];
#endif

	return DRV_OK;
}
#endif

DRV_Error DRV_MEDIA_SetAudioPidChannel(NEXUS_PidChannelHandle ulPidChannelHandle, NEXUS_AudioDecoderHandle ulAudioDecHandle,
														NEXUS_AudioDecoderHandle ulAudioPassThroughDecHandle)
{
	NEXUS_PlaybackPidChannelSettings pidSettings;
	DI_MEDIA_AUDIO_TYPE_e eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_NONE;
	NEXUS_Error		nNexusError = NEXUS_SUCCESS;
	HUINT32 unMediaId = 0;
	HUINT32 i =0;

	PrintEnter;

	if(ulPidChannelHandle == NULL)
	{
		PrintError("[%s] ulPidChannelHandle is null.\n", __func__);

		PrintExit;

		return DRV_ERR;
	}

	if(ulAudioDecHandle == NULL)
	{
		PrintError("[%s] AudioDecoderHandle is null.\n", __func__);

		PrintExit;

		return DRV_ERR;
	}

	if(ulAudioPassThroughDecHandle == NULL)
	{
		PrintError("[%s] ulAudioPassThroughDecHandle is null.\n", __func__);

		PrintExit;

		return DRV_ERR;
	}

	eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_MAIN;

	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		if(s_tStreamInfo[i].audiodecodertype == eTempAudioDecoderType)
		{
			unMediaId = i;
			break;
		}
	}

	nNexusError = NEXUS_Playback_GetPidChannelSettings(s_tMediaInstance[unMediaId].playback, ulPidChannelHandle, &pidSettings);
	if(nNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error > Fail NEXUS_Playback_GetPidChannelSettings.\n", __func__);

		PrintExit;

		return DRV_ERR;
	}

	pidSettings.pidTypeSettings.audio.primary = (NEXUS_AudioDecoderHandle)ulAudioDecHandle;
	pidSettings.pidTypeSettings.audio.secondary = (NEXUS_AudioDecoderHandle)ulAudioPassThroughDecHandle;
	nNexusError = NEXUS_Playback_SetPidChannelSettings(s_tMediaInstance[unMediaId].playback, ulPidChannelHandle, &pidSettings);
	if(nNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error > Fail NEXUS_Playback_SetPidChannelSettings.\n", __func__);

		PrintExit;

		return DRV_ERR;
	}

	PrintExit;

	return DRV_OK;
}

/**
 * @brief		Demux¿¡¼­ TS¿¡ ´ëÇÑ PAT, PMT ParsingÀ» À§ÇØ Playback Pid ChannelÀ» OpenÇÑ´Ù.
 *
 * @param[in]	nDemuxId				Demux Id
 * @param[in]	nPid					Pid Number
 * @param[in]	nChannelType			Channel Type
 * @param[out]	phPidChannel			Pid Channel Handle
 * @return		DRV_Error
 * @see			DRV_MEDIA_ClosePlaybackPidChannel
 */
DRV_Error DRV_MEDIA_OpenPlaybackPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	NEXUS_PlaybackPidChannelSettings pidSettings;
	NEXUS_Error	neResult;
	HUINT32 unMediaId = 0;
	HUINT32 i =0;

	PrintEnter;

	PrintDebug("%s(%d) Info> nDemuxId(%d) , nChannelType(%d) , nPid(0x%x)\n",__FUNCTION__,__LINE__,nDemuxId, nChannelType, nPid);

	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		PrintDebug("%s(%d) s_tStreamInfo[%d].demuxId = %d\n",__FUNCTION__,__LINE__,i,s_tStreamInfo[i].demuxId );
		if(s_tStreamInfo[i].demuxId == nDemuxId)
		{
			unMediaId = i;
			break;
		}
	}

	if(i >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] invalid nDemuxId.\n", __func__);
		nDrvRet = DRV_ERR;
	}
	else
	{
		/* validation checking */
		if(s_tMediaInstance[unMediaId].mediastate < DI_MEDIA_STATE_READY)
		{
			PrintError("[%s] Media doesn't ready.)\n", __func__);
			PrintExit;
			return DRV_ERR;
		}

		NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO:
				if(!s_tMediaInstance[unMediaId].videoPidChannel)
				{
					pidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
					pidSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eUnknown;
					pidSettings.pidTypeSettings.video.decoder = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[%s] Media video PID channel is already opened!\n\r",__FUNCTION__);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO:
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF:
				if(!s_tMediaInstance[unMediaId].audioPidChannel)
				{
					pidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
					pidSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
					pidSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
					pidSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
				}
				else
				{
					PrintError("[%s] Media audio PID channel is already opened!\n\r",__FUNCTION__);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_RECORD:
				{
					PrintError("[%s] Not Support Type\n", __func__);
					PrintExit;
					return DRV_ERR;
				}
			case DI_DEMUX_CHANNELTYPE_PCR:
					*phPidChannel = DRV_DEMUX_GetPcrPidChannel();
					return DRV_OK;
			default:
					/* audio/video PID ¿Í µ¿ÀÏÇÑ °æ¿ì ¼³Á¤ÀÌ ¹Ù²îÁö ¾Êµµ·Ï ÇÔ */
					if( s_tStreamInfo[unMediaId].video_stream_id == (HUINT32)nPid && s_tMediaInstance[unMediaId].videoPidChannel)
					{
						neResult = NEXUS_Playback_GetPidChannelSettings(s_tMediaInstance[unMediaId].playback
						, s_tMediaInstance[unMediaId].videoPidChannel, &pidSettings);
				        if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[%s] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r",__FUNCTION__, neResult);
							return DRV_ERR;
					    }
					}
					else if(s_tStreamInfo[unMediaId].audio_stream_id == (HUINT32)nPid && s_tMediaInstance[unMediaId].audioPidChannel)
					{
						neResult = NEXUS_Playback_GetPidChannelSettings(s_tMediaInstance[unMediaId].playback
						, s_tMediaInstance[unMediaId].audioPidChannel, &pidSettings);
				        if(neResult != NEXUS_SUCCESS)
						{
							PrintError("[%s] Error(0x%x) in NEXUS_Playback_GetPidChannelSettings()\n\r",__FUNCTION__, neResult);
							return DRV_ERR;
					    }
					}

					pidSettings.pidSettings.pidType = NEXUS_PidType_eUnknown;
					*phPidChannel = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, nPid, &pidSettings);
					if(*phPidChannel == NULL)
					{
						PrintError("[%s] Error in NEXUS_Playback_OpenPidChannel\n\r",__FUNCTION__);
						return DRV_ERR;
					}

					break;
		}

	}

	PrintExit;

	return nDrvRet;
}

/**
 * @brief		Demux¿¡¼­ TS¿¡ ´ëÇÑ PAT, PMT ParsingÀ» À§ÇØ OpenÇÑ Playback Pid ChannelÀ» CloseÇÑ´Ù.
 *
 * @param[in]	nDemuxId				Demux Id
 * @param[in]	nChannelType			Channel Type
 * @param[in]	hPidChannel				Pid Channel Handle
 * @return		DRV_Error
 * @see			DRV_MEDIA_OpenPlaybackPidChannel
 */
DRV_Error DRV_MEDIA_ClosePlaybackPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, NEXUS_PidChannelHandle hPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	HUINT32 unMediaId = 0;
	HUINT32 i =0;

	PrintEnter;
	PrintDebug("%s(%d) Info> nDemuxId(%d) , nChannelType(%d) \n",__FUNCTION__,__LINE__,nDemuxId, nChannelType);

	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		if(s_tStreamInfo[i].demuxId == nDemuxId)
		{
			unMediaId = i;
			break;
		}
	}

	if(i >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] invalid nDemuxId.\n", __func__);
		nDrvRet = DRV_ERR;
	}
	else
	{
		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO:
			case DI_DEMUX_CHANNELTYPE_AUDIO:
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF:
				{
					PrintError("[%s] Not Support Type (Please Use GetPlaybackVideoPidChannel or GetPlaybackAudioPidChannel.)\n", __func__);
					return DRV_ERR;
				}
			case DI_DEMUX_CHANNELTYPE_PCR:
			case DI_DEMUX_CHANNELTYPE_RECORD:
				{
					PrintError("[%s] Not Support Type\n", __func__);
					return DRV_ERR;
				}
			default:
				{
					/** Nexus Pid ChannelÀ» CloseÇÑ´Ù. */
					if(s_tMediaInstance[unMediaId].sectionPidChannel != NULL)
					{
						(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, hPidChannel);
						s_tMediaInstance[unMediaId].sectionPidChannel = NULL;
					}
				}
		}
	}

	PrintExit;

	return nDrvRet;
}

HBOOL DRV_MEDIA_CheckStart(void)
{
	return s_ulMediaStart==0 ? FALSE : TRUE;
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT) && defined (CONFIG_PLAYREADY)
void DRV_MEDIA_Uninit(void)
{
	NEXUS_Security_FreeKeySlot(keyWmdrm);
	NEXUS_Security_FreeKeySlot(keyPlayready);
	keyWmdrm = NULL;
	keyPlayready = NULL;
}

void DRV_MEDIA_Reinit(void)
{
	int i;
	NEXUS_SecurityKeySlotSettings keySettings;

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	keyWmdrm = NEXUS_Security_AllocateKeySlot(&keySettings);
	keyPlayready = NEXUS_Security_AllocateKeySlot(&keySettings);


}
#endif


#if __________di__________
#endif

#ifdef SUPPORT_OLD_MEDIA
#define __________DI_OLD_DI_MEDIA__________

DI_ERR_CODE DI_Media_Init(MediaPlayerEvtCb_t callback)
{
	BSTD_UNUSED(callback);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_Term(void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_Play(HUINT32 mpId, DI_MEDIA_TYPE_e  mediaType, char *ucPath, DI_MEDIA_RECT_t destRect)
{
	BSTD_UNUSED(mpId);
	BSTD_UNUSED(mediaType);
	BSTD_UNUSED(ucPath);
	BSTD_UNUSED(destRect);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_GetMp3Info(HUINT32 mpId, char *utl, DI_MEDIA_MP3_INFO_t *outInfo )
{
	BSTD_UNUSED(mpId);
	BSTD_UNUSED(utl);
	BSTD_UNUSED(outInfo);

        return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_GetImageInfo(HUINT32 mpId ,char *url ,DI_MEDIA_IMG_INFO_t *outInfo)
{
	BSTD_UNUSED(mpId);
	BSTD_UNUSED(url);
	BSTD_UNUSED(outInfo);

        return DI_ERR_OK;

}

DI_ERR_CODE DI_Media_Stop(HUINT32 mpId, DI_MEDIA_TYPE_e  mediaType)
{
	BSTD_UNUSED(mpId);
	BSTD_UNUSED(mediaType);

        return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_Resume(HUINT32 mpId)
{
	BSTD_UNUSED(mpId);

        return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_Pause(HUINT32 mpId)
{
	BSTD_UNUSED(mpId);

        return DI_ERR_OK;
}

DI_ERR_CODE DI_Media_RotateImage( HUINT32 mpId, DI_MEDIA_IMG_ROTATE_e direction, DI_MEDIA_RECT_t destRect )
{
	BSTD_UNUSED(mpId);
	BSTD_UNUSED(direction);
	BSTD_UNUSED(destRect);

        return DI_ERR_OK;
}
#endif

#define __________DI_NEW_DI_MEDIA__________
/**
 * @brief		Media file Àç»ýÀ» À§ÇÑ È¯°æ¼³Á¤À» ÇÑ´Ù.
 * @brief		- media fileÀÇ Á¤º¸¸¦ ¾ò´Â´Ù.
 * @brief		- media file Àç»ý¿¡ ÇÊ¿äÇÑ resource¸¦ ¼³Á¤ÇÑ´Ù.
 *
 * @param[in]	pucStreamUrl	StreamÀÇ PathÀÌ´Ù.
 * @param[in]	tStreamInfo		Media Àç»ýÀ» À§ÇÑ Stream¿¡ ´ëÇÑ Á¤º¸ÀÌ´Ù.
 * @param[out]	punMediaId			Di Media ID
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_Setup(HUINT8 *pucStreamUrl, DI_MEDIA_INFO_t *ptStreamInfo, HUINT32 *punMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	DRV_VIDEO_PRESET_DATA_t tVideoPresetData;
	NEXUS_Error		tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackSettings playbackSettings;
	HUINT32 		unMediaId = 0;
	HUINT8			*pucURI_Data = NULL;
	HUINT8			*pucURI_Index = NULL;
	HINT32			nMediaCount=0;
	HBOOL			bUseIndex = FALSE; /* FirstPts º¸Á¤ »ç¿ë ¿©ºÎ¸¦ Index Open ¿©ºÎ·Î ±¸ºÐ (Index Open ½Ã FirstPts º¸Á¤ »ç¿ë X ) */
	PrintEnter;

	if((punMediaId == NULL) || (ptStreamInfo == NULL))
	{
		PrintError("[%s] Error - punMediaId or ptStreamInfo is null\n", __func__);
		return DI_ERR_ERROR;
	}

	if(ptStreamInfo->ucTotalVideoTrack>0)
	{
		for(nMediaCount=0;nMediaCount<ptStreamInfo->ucTotalVideoTrack;nMediaCount++)
		{
			if(ptStreamInfo->stVideoTrack[nMediaCount].video_codec != DI_MEDIA_VIDEO_FORMAT_NONE)
			{
				PrintData("[%s] video_codec=%d\n", __func__, ptStreamInfo->stVideoTrack[nMediaCount].video_codec);
				break;
			}
		}

		if(nMediaCount == ptStreamInfo->ucTotalVideoTrack)
		{
			PrintError("Fail Video Codec is Unknown!\n");
			return DI_ERR_ERROR;
		}

		for(nMediaCount=0;nMediaCount<ptStreamInfo->ucTotalVideoTrack;nMediaCount++)
		{
			if(ptStreamInfo->stVideoTrack[nMediaCount].video_stream_id != 0)
			{
				PrintData("[%s] video_stream_id=%d\n", __func__, ptStreamInfo->stVideoTrack[nMediaCount].video_stream_id);
				break;
			}
		}

		if(nMediaCount == ptStreamInfo->ucTotalVideoTrack)
		{
			PrintError("Fail Video Stream Id is Unknown!\n");
			return DI_ERR_ERROR;
		}
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_GetMediaId(&unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Fail P_MEDIA_GetMediaId(%d)\n", __func__, eDIError);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	s_ulMediaId = unMediaId;

	VK_MEM_Memset(&s_tStreamInfo[unMediaId], 0x0, sizeof(DI_MEDIA_INFO_t));
	s_tMediaInstance[unMediaId].mediaurl = DD_MEM_Alloc(VK_strlen((char *)pucStreamUrl)+1);
	VK_MEM_Memset(s_tMediaInstance[unMediaId].mediaurl, 0x0, VK_strlen((char *)pucStreamUrl)+1);
	s_tMediaInstance[unMediaId].mediastate=DI_MEDIA_STATE_NO_SETUP;
	s_tMediaInstance[unMediaId].mediaType=DI_MEDIA_PLAYPUMP_DIVX;
	s_tMediaInstance[unMediaId].audioStatus=DI_MEDIA_AUDIO_FREE;
	s_tMediaInstance[unMediaId].audioPlayback=NULL;
	s_tMediaInstance[unMediaId].playback=NULL;
	s_tMediaInstance[unMediaId].audioPidChannel=NULL;
	s_tMediaInstance[unMediaId].videoPidChannel=NULL;
	s_tMediaInstance[unMediaId].sectionPidChannel=NULL;
	s_tMediaInstance[unMediaId].pcrPidChannel=NULL; /* pcr_pid */
	s_tMediaInstance[unMediaId].file=NULL;
	s_tMediaInstance[unMediaId].trickrate=DI_MEDIA_TRICK_PAUSE;
#if defined (USE_DRM)
	VK_MEM_Memset(&s_tMediaInstance[unMediaId].tDrm_cfg, 0x0, sizeof(bdrm_cfg));
	VK_MEM_Memset(&s_tMediaInstance[unMediaId].tDrm, 0x0, sizeof(bdrm_t));
	s_tMediaInstance[unMediaId].tDcryptor = NULL;
	s_tMediaInstance[unMediaId].tDrm_license_state = bdrm_lic_pending;
	s_tMediaInstance[unMediaId].tDrm_http_state = bdrm_http_status_ok;
	VK_MEM_Memset(s_tMediaInstance[unMediaId].aucEncryptionKey, 0x0, DI_MEDIA_ENCRYPTION_KEY_LEN);
#endif

	VK_memcpy(&s_tStreamInfo[unMediaId], ptStreamInfo, sizeof(DI_MEDIA_INFO_t));
	VK_memcpy(s_tMediaInstance[unMediaId].mediaurl, pucStreamUrl, VK_strlen((char *)pucStreamUrl));

	*punMediaId = unMediaId;
#if SUPPORT_AUDIOCODEC_LPCM
	HINT32	nResult = VK_OK;
	DI_MEDIA_MSG	audMsg;
	if((ptStreamInfo->stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (ptStreamInfo->audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		audMsg.ulSender = DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP;
		audMsg.ulEvent = DI_MEDIA_PLAYPUMPPLAY_SETUP;
		audMsg.pucParam = VK_StrDup(pucStreamUrl);
		audMsg.ulParam2 = unMediaId;
		audMsg.ulParam3 = ptStreamInfo->stream_type;
		audMsg.ulParam4 = ptStreamInfo->sampleRate;
		audMsg.ulParam5 = ptStreamInfo->bitsPerSample;
		audMsg.ulParam6 = ptStreamInfo->isStereo;

		PrintError("audMsg.ulParam2=%lu, audMsg.ulParam3=%lu, audMsg.pucParam=%s\n", audMsg.ulParam2, audMsg.ulParam3, audMsg.pucParam);
		DRV_PVR_GetPlaypumpHandle(&s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
		nResult = VK_MSG_SendTimeout(s_ulMediaPlaypumpStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nResult);
		}

		/* goto Success; */
		(void)VK_SEM_Release(s_ulMediaSemId);
		PrintExit;
		return eDIError;
	}
#endif
	eDIError = P_MEDIA_CreatePlayback(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Fail P_MEDIA_GetMediaId(%d)\n", __func__, eDIError);
		s_tMediaInstance[unMediaId].valid = FALSE;
		if(s_tMediaInstance[unMediaId].mediaurl != NULL)
		{
			DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
			s_tMediaInstance[unMediaId].mediaurl = NULL;
		}

		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	/** Set Media Type */
	if((s_tStreamInfo[unMediaId].audio_stream_id != 0) && (s_tStreamInfo[unMediaId].audio_codec != 0) &&
		(s_tStreamInfo[unMediaId].video_stream_id != 0) && (s_tStreamInfo[unMediaId].video_codec != 0))
	{
		s_tMediaInstance[unMediaId].mediaType = DI_MEDIA_PLAYPUMP_DIVX;
	}
	else if((s_tStreamInfo[unMediaId].audio_stream_id != 0) && (s_tStreamInfo[unMediaId].audio_codec != 0))
	{
		s_tMediaInstance[unMediaId].mediaType = DI_MEDIA_PLAYPUMP_AUDIO_MAIN;
	}
	else if((s_tStreamInfo[unMediaId].video_stream_id != 0) && (s_tStreamInfo[unMediaId].video_codec != 0))
	{
		s_tMediaInstance[unMediaId].mediaType = DI_MEDIA_PLAYPUMP_VIDEO;
	}
	else
	{
		PrintError("[%s] Error - Not exist Video & Audio Pid\n", __func__);
		eDIError = DI_ERR_ERROR;
		(void)P_MEDIA_DeletePlayback(unMediaId);
		if(s_tMediaInstance[unMediaId].mediaurl != NULL)
		{
			DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
			s_tMediaInstance[unMediaId].mediaurl = NULL;
		}

		s_tMediaInstance[unMediaId].valid = FALSE;
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

#if defined (USE_DRM)
	if ( TRUE == ptStreamInfo->is_drm )
	{
		/* playback¿¡ playpump¸¦ ¼³Á¤ÇÏ±â Àü¿¡ È£ÃâµÇ¾î¾ß ÇÑ´Ù.
		 * => DRM contents¸¦ Àç»ýÇÏ±â À§ÇÑ key°¡ ¾Æ·¡ ÇÔ¼ö È£ÃâÀ» ÅëÇØ ¾ò¾îÁö¸ç,
		 *   ÀÌ key °ªÀÌ bmedia module¿¡ Àü´ÞµÇ¾î¾ß ÇÔ.
		 * => nexus¿¡¼­ DRM¿ë key °ªÀº
		 *    NEXUS_PlaybackSettings.playpumpSettings.securityContextÀ» ÅëÇØ Àü´ÞµÈ´Ù.
		 *
		 * ¶ÇÇÑ NEXUS FilePlay_OpenPosix ÀÌÀü¿¡ È£ÃâµÇ¾î¾ß ÇÔ.
		 * drm_open ¿¡¼­ header Á¤º¸¸¦ ¾ò±âÀ§ÇØ open ÇÏ¹Ç·Î open sequence °¡ °ãÄ¡°Ô µÊ.
		 */
		eDIError = P_MEDIA_DRM_Open (s_tMediaInstance[unMediaId].mediaurl,&s_tMediaInstance[unMediaId]);
		if (eDIError != DI_ERR_OK)
		{
			PrintError ("%s(%d) Error - : %d\n",__FUNCTION__,__LINE__, eDIError);
			if(s_tMediaInstance[unMediaId].mediaurl != NULL)
			{
				DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
				s_tMediaInstance[unMediaId].mediaurl = NULL;
			}

			(void)P_MEDIA_DeletePlayback(unMediaId);
			s_tMediaInstance[unMediaId].valid = FALSE;
			goto drm_error_exit;
		}
		/* else - do nothing */
		PrintDebug ("%s(%d) MS-DRM is opend.\n",__FUNCTION__,__LINE__);
	}
	/* else - do nothing */
#endif

	/** Open File Posix */
	if((NULL != VK_strstr((char *)pucStreamUrl, "http://")) || (NULL != VK_strstr((char *)pucStreamUrl, "https://"))
		|| (NULL != VK_strstr((char *)pucStreamUrl, "mms://")) || (NULL != VK_strstr((char *)pucStreamUrl, "rtsp://")))
	{

		/* DNP Player looks like Local file Play */
		if (NULL != VK_strstr((char*)pucStreamUrl,"[dnp]"))
		{
			if(s_tStreamInfo[unMediaId].stream_type != DI_MEDIA_MPEG_TYPE_ASF)
			{
				PrintData("IPTUNER (DNP) Play(%s)\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucStreamUrl, (char *)pucStreamUrl);
				bUseIndex = TRUE;
			}
			else
			{
				PrintData("IPTUNER (DNP-progressive down) Play(%s)\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucStreamUrl, (char *)pucStreamUrl); /* progressive down */
				bUseIndex = TRUE;
			}
		}
		/* DLNA Player looks like Local file Play */
		else if (NULL != VK_strstr((char*)pucStreamUrl,"[DLNA]"))
		{
#if SUPPORT_AUDIOCODEC_LPCM
			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)
				|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES)
				|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MPEG1)
				|| (s_tStreamInfo[unMediaId].RangeAvaliable == FALSE))
#else
			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES) || (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MPEG1))
#endif
			{
				/* URL¿¡ DATA ¹× INDEX¸¦ ºÙ¿© File Open½Ã ¾î´À°ÍÀ» OpenÇÏ´ÂÁö ±¸ºÐÇÔ */
				/* ºÙÀÎ String¿¡ ´ëÇØ DLNA File Library »ç¿ëÇÏ´Â Module¿¡¼­ Á¦°ÅÇÏ¿© »ç¿ëÇÔ */
				pucURI_Data = DD_MEM_Alloc((VK_strlen(pucStreamUrl)+8));
				if(pucURI_Data == NULL)
				{
					PrintError("[%s] Error - Memory Allocation Error (pucURI_Data)\n", __func__);
					(void)P_MEDIA_DeletePlayback(unMediaId);
					if(s_tMediaInstance[unMediaId].mediaurl != NULL)
					{
						DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
						s_tMediaInstance[unMediaId].mediaurl = NULL;
					}

					s_tMediaInstance[unMediaId].valid = FALSE;
					(void)VK_SEM_Release(s_ulMediaSemId);
					return DI_ERR_ERROR;
				}
				else
				{
					VK_memset(pucURI_Data, 0x0, VK_strlen(pucStreamUrl)+8);
					VK_snprintf((char *)pucURI_Data, VK_strlen(pucStreamUrl)+7, "[DLNA][DATA]%s", (char *)pucStreamUrl+6);
				}

				PrintData("IPTUNER (DLNA) Play(%s)\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucURI_Data, NULL);

				if (NULL != pucURI_Data)
				{
					DD_MEM_Free(pucURI_Data);
					pucURI_Data = NULL;
				}
			}
			else
			{
				/* URL¿¡ DATA ¹× INDEX¸¦ ºÙ¿© File Open½Ã ¾î´À°ÍÀ» OpenÇÏ´ÂÁö ±¸ºÐÇÔ */
				/* ºÙÀÎ String¿¡ ´ëÇØ DLNA File Library »ç¿ëÇÏ´Â Module¿¡¼­ Á¦°ÅÇÏ¿© »ç¿ëÇÔ */
				pucURI_Data = DD_MEM_Alloc((VK_strlen(pucStreamUrl)+8));
				if(pucURI_Data == NULL)
				{
					PrintError("[%s] Error - Memory Allocation Error (pucURI_Data)\n", __func__);
					(void)P_MEDIA_DeletePlayback(unMediaId);
					if(s_tMediaInstance[unMediaId].mediaurl != NULL)
					{
						DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
						s_tMediaInstance[unMediaId].mediaurl = NULL;
					}

					s_tMediaInstance[unMediaId].valid = FALSE;
					(void)VK_SEM_Release(s_ulMediaSemId);
					return DI_ERR_ERROR;
				}
				else
				{
					VK_memset(pucURI_Data, 0x0, VK_strlen(pucStreamUrl)+8);
					VK_snprintf((char *)pucURI_Data, VK_strlen(pucStreamUrl)+7, "[DLNA][DATA]%s", (char *)pucStreamUrl+6);
				}

				pucURI_Index = DD_MEM_Alloc((VK_strlen(pucStreamUrl)+8));
				if(pucURI_Index == NULL)
				{
					PrintError("[%s] Error - Memory Allocation Error (pucURI_Index)\n", __func__);
					(void)P_MEDIA_DeletePlayback(unMediaId);
					if(s_tMediaInstance[unMediaId].mediaurl != NULL)
					{
						DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
						s_tMediaInstance[unMediaId].mediaurl = NULL;
					}

					s_tMediaInstance[unMediaId].valid = FALSE;
					(void)VK_SEM_Release(s_ulMediaSemId);
					DD_MEM_Free(pucURI_Data);
					pucURI_Data = NULL;

					return DI_ERR_ERROR;
				}
				else
				{
					VK_memset(pucURI_Index, 0x0, VK_strlen(pucStreamUrl)+8);
					VK_snprintf((char *)pucURI_Index, VK_strlen(pucStreamUrl)+8, "[DLNA][INDEX]%s", (char *)pucStreamUrl+6);
				}

				PrintData("IPTUNER (DLNA) Play(%s)\n", pucURI_Data);
				PrintData("IPTUNER (DLNA) Index(%s)\n", pucURI_Index);

				/* 40nm nexus unified model patch */
				if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS)
					|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
					|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_VOB)
					|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES))
				{
					s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucURI_Data, NULL);
				}
				else
				{
					s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucURI_Data, (char *)pucURI_Index);
					if(s_tMediaInstance[unMediaId].file == NULL)
					{
						s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucURI_Data, NULL);
					}
					else
					{
						bUseIndex = TRUE;
					}
				}

				if (NULL != pucURI_Data)
				{
					DD_MEM_Free(pucURI_Data);
					pucURI_Data = NULL;
				}

				if (NULL != pucURI_Index)
				{
					DD_MEM_Free(pucURI_Index);
					pucURI_Index = NULL;
				}
			}
		}
		/* Streaming IPTUNER MUST be opend as Sing File for VFIO Operation.*/
		else
		{
			PrintData("IPTUNER (Streaming) Play(%s)\n", pucStreamUrl);

			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS) ||
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192) ||
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES) ||
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MPEG1) ||
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT) || /* for SSTR */
				((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)&&(NULL != VK_strstr((char *)pucStreamUrl, "mms://"))) ||
				((s_tMediaInstance[unMediaId].mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN)&&(s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_MP3)))
			{
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)pucStreamUrl, NULL);
			}
			else
			{
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)pucStreamUrl, (char *)pucStreamUrl);
				bUseIndex = TRUE;
			}
		}
	}
	else
	{
		PrintData("Local File Play(%s)\n", pucStreamUrl);

		if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS)				/* 40nm nexus unified model patch */
			|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192)	/* 40nm nexus unified model patch */
			|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_VOB)		/* 40nm nexus unified model patch */
			|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES)
			|| (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MPEG1)
			|| (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCM)
			|| (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
		{
			s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)pucStreamUrl, NULL);
		}
		else
		{
			s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucStreamUrl, (char *)pucStreamUrl);
			bUseIndex = TRUE;
		}
	}
#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)
	{
		int msg[4];
		msg[0] = DRV_MEDIA_SETPOSITION_ACCURACY_ENTER_MEDIA;
		msg[1] = unMediaId;
		VK_MSG_Send(sulStatusMonitorThreadQ, msg, 4*sizeof(unsigned long));

	}
#endif
	if (!s_tMediaInstance[unMediaId].file)
	{
		PrintError("[%s] Error - can't open file:%s\n", __func__, pucStreamUrl);
		eDIError = DI_ERR_ERROR;
		(void)P_MEDIA_DeletePlayback(unMediaId);
		if(s_tMediaInstance[unMediaId].mediaurl != NULL)
		{
			DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
			s_tMediaInstance[unMediaId].mediaurl = NULL;
		}

		s_tMediaInstance[unMediaId].valid = FALSE;
#if defined (USE_DRM)
		if (s_tMediaInstance[unMediaId].tDrm != NULL)
		{
			(void) P_MEDIA_DRM_Close (&s_tMediaInstance[unMediaId]);
			PrintDebug ("%s(%d) MS-DRM is closed.\n",__FUNCTION__,__LINE__);
		}
#endif
	}
	else
	{
//		s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] = stMediaInstance.playpump[DI_MEDIA_PLAYPUMP_DIVX];

		/** Set Playback Option */
		(void)NEXUS_Playback_GetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT)
		{
			DRV_PVR_GetPlaypumpHandleByClass(&s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],BUFF_GROUP_1);
		}
		else
		{
			DRV_PVR_GetPlaypumpHandle(&s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
		}
		playbackSettings.playpump = s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX];
		playbackSettings.playpumpSettings.transportType = P_MEDIA_ConvertTransportTypeToNexus(s_tStreamInfo[unMediaId].stream_type);

		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
		{
			playbackSettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eMod300;
			playbackSettings.playpumpSettings.timestamp.timebase = NEXUS_Timebase_eInvalid;
			playbackSettings.playpumpSettings.timestamp.pacing = FALSE;
			playbackSettings.playpumpSettings.timestamp.pacingMaxError = 2048;
		}
		else
		{
			playbackSettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_eNone;
		}

#if defined (USE_DRM)
		if ( TRUE == ptStreamInfo->is_drm)
		{
			playbackSettings.playpumpSettings.securityContext = (NEXUS_KeySlotHandle) (*(void **)s_tMediaInstance[unMediaId].aucEncryptionKey);
		}
#endif
		playbackSettings.startPaused = true;

		playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
		playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePause;

		if((s_tStreamInfo[unMediaId].video_stream_id == 0) && (s_tStreamInfo[unMediaId].audio_stream_id != 0)
			&& ((s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_MP3) ||
 (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCM)))
		{
			playbackSettings.playpumpSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
			playbackSettings.enableStreamProcessing = TRUE;
		}
		else
		{
			playbackSettings.playpumpSettings.mode = NEXUS_PlaybackMode_eIndexed;
		}

#if SUPPORT_AUDIOCODEC_LPCM
		if((s_tStreamInfo[unMediaId].RangeAvaliable == FALSE) && (NULL != VK_strstr((char *)s_tMediaInstance[unMediaId].mediaurl, "[DLNA]")))
		{
			playbackSettings.playpumpSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
			playbackSettings.enableStreamProcessing = TRUE;
		}
#endif
		playbackSettings.endOfStreamCallback.callback = P_MEDIA_EndOfStreamCallback;
		playbackSettings.endOfStreamCallback.context = NULL;
		playbackSettings.endOfStreamCallback.param= unMediaId;
		playbackSettings.beginningOfStreamCallback.callback = P_MEDIA_BeginOfStreamCallback;
		playbackSettings.beginningOfStreamCallback.context = NULL;
		playbackSettings.beginningOfStreamCallback.param= unMediaId;
		playbackSettings.errorCallback.callback = P_MEDIA_ErrorOfStreamCallback;
		playbackSettings.errorCallback.context = NULL;
		playbackSettings.errorCallback.param= unMediaId;
#if 0  /* Nexus Phase 6.5ÀÌ»ó¿¡¼­ »ç¿ëµÉ ºÎºÐ */
		playbackSettings.playErrorHandling = NEXUS_PlaybackErrorHandlingMode_eEndOfStream;
		playbackSettings.seekErrorHandling = NEXUS_PlaybackErrorHandlingMode_eIgnore;
		playbackSettings.trickErrorHandling = NEXUS_PlaybackErrorHandlingMode_eIgnore;
		playbackSettings.ioTimeout = 5000; /* 5 sec */
#endif

#ifdef CONFIG_TEMP_DTG_HDR2000T
		/* BBC CATAL TEST HLS-6 Music AssetÀÇ pause/play ½Ã ioTimeout °É¸®´Â ÀÌ½´ workaround.*/
		if(NULL != VK_strstr(pucStreamUrl,".m3u8"))
		{
			playbackSettings.ioTimeout = 20000; /* 20 sec, let's say maxium segment duration is 20s, the the ioTimeout is 20s. */
		}
#endif

		tNexusError = NEXUS_Playback_SetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
		(void)NEXUS_Playback_GetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
		if(s_tMediaInstance[unMediaId].mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN)
		{
			playbackSettings.stcTrick = false;
		}
		else
		{
			/* WMV PAUSE/RESUME Patch */
			playbackSettings.stcChannel = DRV_MEDIA_MP_GetPlaybackStcChannelHandle(s_tStreamInfo[unMediaId].videodecodertype, s_tStreamInfo[unMediaId].audiodecodertype, DI_MEDIA_PLAYPUMP_DIVX);
			playbackSettings.stcTrick = true;
		}
		tNexusError = NEXUS_Playback_SetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);

		if(NEXUS_SUCCESS != tNexusError)
		{
			PrintError("[%s] Error - Fail NEXUS_PlaybackSetSetting:%d\n", __func__,tNexusError);
			(void)NEXUS_FilePlay_Close(s_tMediaInstance[unMediaId].file);
			(void)P_MEDIA_DeletePlayback(unMediaId);
			if(s_tMediaInstance[unMediaId].mediaurl != NULL)
			{
				DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
				s_tMediaInstance[unMediaId].mediaurl = NULL;
			}

			s_tMediaInstance[unMediaId].valid = FALSE;
#if defined (USE_DRM)
			if (s_tMediaInstance[unMediaId].tDrm != NULL)
			{
				(void) P_MEDIA_DRM_Close (&s_tMediaInstance[unMediaId]);
				PrintDebug ("%s(%d) MS-DRM is closed.\n",__FUNCTION__,__LINE__);
			}
#endif
			eDIError = DI_ERR_ERROR;
	/* else { continue; } */
		}
		else
		{
			s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_READY;
			s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_UPDATE_FULLTIME;
		}

		/* VIDEO Setting for Smooth Streaming */
		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT)
		{
			VK_MEM_Memset(&tVideoPresetData,0x00,sizeof(DRV_VIDEO_PRESET_DATA_t));
			tVideoPresetData.bOnOff = true;
			DRV_VIDEO_SetPresetProperties(DI_VIDEO_DECODER_MAIN, DRV_VIDEO_PRESET_SSTR ,&tVideoPresetData); // TODO: Decoder Setting È®ÀÎ
		}
#if defined (CONFIG_PLAYREADY)
		DRV_Error dResult;
		DI_license_response response;

		VK_MEM_Memset(&response, 0, sizeof(DI_license_response));
		NEXUS_Playpump_SetWmdrmDecryptCb(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																NULL);
		NEXUS_Playpump_SetPlayReadyAsfDecryptCb(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																NULL);
		NEXUS_Playpump_SetPlayReadySstrDecryptCb(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																NULL);
		if(ptStreamInfo->is_drm == TRUE)
		{
			if(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_WMDRM)
			{
				NEXUS_Playpump_SetWmdrmDecryptCb(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																P_MEDIA_DoDecryptRC4_hw);
#if defined(CONFIG_PLAYREADY_20)
				dResult = P_DRM_PlayReady_GetLicense(ptStreamInfo->pucWrmHeader, ptStreamInfo->nWrmHeaderLen);
#else
				dResult = P_MEDIA_GetLicense(ptStreamInfo->pucWrmHeader, ptStreamInfo->nWrmHeaderLen, &response);
#endif
				if(dResult != DRV_OK)
				{
					PrintError("P_MEDIA_GetLicense failed Exit.\n");
					goto drm_error_exit;
				}
			}
			else if(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_PLAYREADY)
			{
				if(ptStreamInfo->stream_type == DI_MEDIA_MPEG_TYPE_ASF)
				{
					NEXUS_Playpump_SetPlayReadyAsfDecryptCb(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																P_MEDIA_DoDecryptAESCtr);
				}
				else
				{
					NEXUS_Playpump_SetPlayReadySstrDecryptCb(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																P_MEDIA_DoDecryptSample);
				}

#if defined(CONFIG_PLAYREADY_20)
				dResult = P_DRM_PlayReady_GetLicense(ptStreamInfo->pucWrmHeader, ptStreamInfo->nWrmHeaderLen);
#else
				dResult = P_MEDIA_GetLicense(ptStreamInfo->pucWrmHeader, ptStreamInfo->nWrmHeaderLen, &response);
#endif
				if(dResult != DRV_OK)
				{
					PrintError("P_MEDIA_GetLicense failed Exit.\n");
#if 0 //hdrfoxc use async setlicense by dhkim2
					goto drm_error_exit;
#endif
				}
			}
			else
			{
				PrintError("Unknown DRMType = %d\n", ptStreamInfo->eDrmType);
			}
		}
#endif

		if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS
				|| s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_VOB
				|| s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
				&& bUseIndex == FALSE)
		{
			if(s_tStreamInfo[unMediaId].bProbedFirstPts == TRUE)
			{
				P_MEDIA_InstallFirstPts(unMediaId, s_tStreamInfo[unMediaId].unProbeFirstPts);
			}
			else
				(void) P_MEDIA_RegisterFirstPtsCB (unMediaId, (DI_MEDIA_FirstPtsCallback_t) &P_MEDIA_FirstPtsCallback);
		}
		else
		{
			P_MEDIA_InstallFirstPts(unMediaId, 0);
		}
	}

	/* s_ulMediaStart for DI_AUDIO, DI_VIDEO : DRV_MEDIA_CheckStart() */
	++s_ulMediaStart;

/* Success: */
	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;

#if defined (CONFIG_PLAYREADY)
drm_error_exit:
	s_tMediaInstance[unMediaId].valid = FALSE;
	if(s_tMediaInstance[unMediaId].mediaurl != NULL)
	{
		DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
		s_tMediaInstance[unMediaId].mediaurl = NULL;
	}
	(void)VK_SEM_Release(s_ulMediaSemId);
	return DI_ERR_ERROR;
#endif
}


#if defined (CONFIG_DSC)
/* must be free keyslot */
DI_ERR_CODE DI_MEDIA_MP_SetDecryption (HUINT32 unMediaId, DI_MEDIA_CRYPTO_SETTINGS *pstDecryptSettings)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	HUINT32 vidDescId;
	HUINT32 audDescId;

	unsigned char iv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("%s(%d) Error> Invalid Media Id.\n", __FUNCTION__,__LINE__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	if((s_tMediaInstance[unMediaId].playback == NULL) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_NO_SETUP))
	{
		PrintError("%s(%d) Error - Not initialized Media.\n", __FUNCTION__,__LINE__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		eDIError = DI_DSC_Open (s_tStreamInfo[unMediaId].demuxId, DI_DSC_DESC_AES, &vidDescId, NULL);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_Open Fail (vidDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}

		eDIError = DI_DSC_Open (s_tStreamInfo[unMediaId].demuxId, DI_DSC_DESC_AES, &audDescId, NULL);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_Open Fail (audDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}

		eDIError = DI_DSC_SetProtectionKey (vidDescId, pstDecryptSettings->ucProtectionKey, 16);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetProtectionKey Fail (vidDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}
		eDIError = DI_DSC_SetProtectionKey (audDescId, pstDecryptSettings->ucProtectionKey, 16);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetProtectionKey Fail (audDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}

		eDIError = DI_DSC_SetPid(vidDescId, s_tStreamInfo[unMediaId].video_stream_id);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetPid Fail (vidDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}
		eDIError = DI_DSC_SetKey(vidDescId, DI_DSC_DESC_ODD, pstDecryptSettings->ucKey, 16, iv, sizeof(iv));
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetKey (ODD) Fail (vidDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}
		eDIError = DI_DSC_SetKey(vidDescId, DI_DSC_DESC_EVEN, pstDecryptSettings->ucKey, 16, NULL, 0);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetKey (EVEN) Fail (vidDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}

		eDIError = DI_DSC_SetPid(audDescId, s_tStreamInfo[unMediaId].audio_stream_id);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetPid Fail (audDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}
		eDIError = DI_DSC_SetKey(audDescId, DI_DSC_DESC_ODD, pstDecryptSettings->ucKey, 16, NULL, 0);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetKey (ODD) Fail (audDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}
		eDIError = DI_DSC_SetKey(audDescId, DI_DSC_DESC_EVEN, pstDecryptSettings->ucKey, 16, NULL, 0);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("%s(%d) Error> DI_DSC_SetKey (EVEN) Fail (audDescId)\n", __FUNCTION__,__LINE__);
			(void)VK_SEM_Release(s_ulMediaSemId);
			return eDIError;
		}

		pstDecryptSettings->vidDescId = vidDescId;
		pstDecryptSettings->audDescId = audDescId;

		if( s_tStreamInfo[unMediaId].pstDSC )
		{
			PrintError ("%s(%d) Alert!> s_tStreamInfo[unMediaId].pstDSC is already allocated, so it must be free.\n",__FUNCTION__,__LINE__ );
			DD_MEM_Free (s_tStreamInfo[unMediaId].pstDSC);
		}

		/* save DSC information in the static structure heap . It must be deallocated when the cleardecryption is called.*/
		s_tStreamInfo[unMediaId].pstDSC = (DI_MEDIA_CRYPTO_SETTINGS*) DD_MEM_Alloc ( sizeof(DI_MEDIA_CRYPTO_SETTINGS) );
		if (NULL==s_tStreamInfo[unMediaId].pstDSC)
		{
			(void)VK_SEM_Release(s_ulMediaSemId);
			PrintError("%s(%d) Error> pstDSC Allocation Fail\n", __FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}
		else
		{
			VK_memcpy (s_tStreamInfo[unMediaId].pstDSC, pstDecryptSettings ,sizeof(DI_MEDIA_CRYPTO_SETTINGS));
		}
	}

    PrintExit;
	(void)VK_SEM_Release(s_ulMediaSemId);

    return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_MP_ClearDecryption (HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("%s(%d) Error> Invalid Media Id.\n", __FUNCTION__,__LINE__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	if((s_tMediaInstance[unMediaId].playback == NULL) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_NO_SETUP))
	{
		PrintError("%s(%d) Error - Not initialized Media.\n", __FUNCTION__,__LINE__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if(s_tStreamInfo[unMediaId].pstDSC)
		{
			/* We don't need to check the return value of DSC Funcs. So (void) type functions were used in Key clearing.*/
			(void) DI_DSC_ClearProtectionKey (s_tStreamInfo[unMediaId].pstDSC->vidDescId);
			(void) DI_DSC_ClearProtectionKey (s_tStreamInfo[unMediaId].pstDSC->audDescId);

			(void) DI_DSC_ClearPid(s_tStreamInfo[unMediaId].pstDSC->vidDescId, s_tStreamInfo[unMediaId].video_stream_id);
			(void) DI_DSC_ClearPid(s_tStreamInfo[unMediaId].pstDSC->audDescId, s_tStreamInfo[unMediaId].audio_stream_id);

			(void) DI_DSC_Close (s_tStreamInfo[unMediaId].pstDSC->vidDescId);
			(void) DI_DSC_Close (s_tStreamInfo[unMediaId].pstDSC->audDescId);

			PrintError ("%s(%d) Alert!> s_tStreamInfo[unMediaId].pstDSC is already allocated, so it must be free.\n",__FUNCTION__,__LINE__ );
			DD_MEM_Free (s_tStreamInfo[unMediaId].pstDSC);
			s_tStreamInfo[unMediaId].pstDSC = NULL;
		}
		else
		{
			PrintError("%s(%d) Error - pstDSC is NULL .\n", __FUNCTION__,__LINE__);
			eDIError = DI_ERR_ERROR;
		}
	}

    PrintExit;
	(void)VK_SEM_Release(s_ulMediaSemId);

    return DI_ERR_OK;
}
#endif


/**
 * @brief 		Media file Àç»ýÀ» À§ÇØ ¼³Á¤µÈ È¯°æ¼³Á¤À» Á¾·áÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_Release(HUINT32 unMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	DRV_VIDEO_PRESET_DATA_t tVideoPresetData;
	NEXUS_StcChannelSettings stcSettings;

#if SUPPORT_AUDIOCODEC_LPCM
	HINT32			nResult = VK_OK;
	DI_MEDIA_MSG	audMsg;
	HINT32			nCount=0;
#endif

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("[%s] Error - Invalid Media Id.\n", __func__);
			eDIError = DI_ERR_ERROR;
		}
		else
		{
#if SUPPORT_AUDIOCODEC_LPCM
			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
			{
				audMsg.ulSender = DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP;
				audMsg.ulEvent = DI_MEDIA_PLAYPUMPPLAY_NOT_READY;
				audMsg.ulParam2 = unMediaId;

				nResult = VK_MSG_SendTimeout(s_ulMediaPlaypumpStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
				if(nResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nResult);
				}

				do
				{
					if(nCount <=5)
					{
						nCount++;
					}
					else
					{
						PrintError("[%s] Error - Not stop Playpump Streaming\n", __func__);
						break;
					}

					VK_TASK_Sleep(500);
				}while(s_eMediaPlaypumpStreamState != DI_MEDIA_PLAYPUMPPLAY_NOT_READY);

				(void)NEXUS_Playpump_ClosePidChannel(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], s_tMediaInstance[unMediaId].audioPidChannel);
				eDIError = DI_ERR_OK;
			}
			else
			{
#endif
#if SUPPORT_USE_STC_AUTOMODE_FIRSTAVAILABLE
				(void)NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
				if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)
					&& (stcSettings.modeSettings.Auto.behavior == NEXUS_StcChannelAutoModeBehavior_eFirstAvailable))
				{
					stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
					NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
				}
#endif
				if(s_tMediaInstance[unMediaId].mediaType == DI_MEDIA_PLAYPUMP_DIVX)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].videoPidChannel);
					for(HUINT32 i = 0; i < s_tStreamInfo[unMediaId].ucTotalAudioTrack ; i++)
					{
						if(s_tMediaInstance[unMediaId].audioTrackPidChannels[i] != NULL)
						{
							(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[i]);
							s_tMediaInstance[unMediaId].audioTrackPidChannels[i] = NULL;
						}
					}
					s_tMediaInstance[unMediaId].audioPidChannel = NULL;
					s_tMediaInstance[unMediaId].audioSubPidChannel = NULL;
				}
				else if(s_tMediaInstance[unMediaId].mediaType == DI_MEDIA_PLAYPUMP_VIDEO)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].videoPidChannel);

				}
				else if(s_tMediaInstance[unMediaId].mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioPidChannel);
				}
				if(s_tStreamInfo[unMediaId].pcr_pid != 0x1fff &&
				s_tStreamInfo[unMediaId].pcr_pid != s_tStreamInfo[unMediaId].video_stream_id &&
				s_tStreamInfo[unMediaId].pcr_pid != s_tStreamInfo[unMediaId].audio_stream_id &&
				s_tMediaInstance[unMediaId].pcrPidChannel != NULL)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].pcrPidChannel);					}
				s_tMediaInstance[unMediaId].pcrPidChannel = NULL;

				NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
				stcSettings.timebase = NEXUS_Timebase_e0;
				stcSettings.mode = NEXUS_StcChannelMode_eAuto;
				stcSettings.modeSettings.pcr.pidChannel = NULL;
				NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);

				(void)NEXUS_FilePlay_Close(s_tMediaInstance[unMediaId].file);

				(void)P_MEDIA_DeletePlayback(unMediaId);
#if SUPPORT_AUDIOCODEC_LPCM
			}
#endif
			DRV_PVR_ReleasePlaypumpHandle(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
			s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] = NULL;

			if(s_tEventCallback[unMediaId] != NULL)
			{
				s_tEventCallback[unMediaId] = NULL;
			}

#if defined (USE_DRM)
			if (s_tMediaInstance[unMediaId].tDrm != NULL)
			{
				(void) P_MEDIA_DRM_Close (&s_tMediaInstance[unMediaId]);
				/* else { continue; } */
			}
#endif

#if defined (USE_PLAYREADY_DRM)
			if(pucLicenseServerUrl != NULL)
			{
				VK_MEM_Free(pucLicenseServerUrl);
				pucLicenseServerUrl = NULL;
			}
#endif
			if(s_tMediaInstance[unMediaId].mediaurl != NULL)
			{
				DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
				s_tMediaInstance[unMediaId].mediaurl = NULL;
			}

			if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT)
			{
				VK_MEM_Memset(&tVideoPresetData,0x00,sizeof(DRV_VIDEO_PRESET_DATA_t));
				tVideoPresetData.bOnOff = false;
				DRV_VIDEO_SetPresetProperties(DI_VIDEO_DECODER_MAIN, DRV_VIDEO_PRESET_SSTR ,&tVideoPresetData); // TODO: Decoder Setting È®ÀÎ
			}
			if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS ||
			s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192 )
			{
				(void)P_MEDIA_UnregisterFirstPtsCB(unMediaId);
			}
			VK_MEM_Memset(&s_tMediaInstance[unMediaId], 0x0, sizeof(DI_MediaInstance));
			VK_MEM_Memset(&s_tStreamInfo[unMediaId], 0x0, sizeof(DI_MEDIA_INFO_t));
		}
	}

	if( VK_SEM_Release(s_ulMediaSemId) != VK_OK )
	{
		PrintError("[%s] VK_SEM_Release failed\n\r",__FUNCTION__);
		eDIError = DI_ERR_ERROR;
	}

	s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_NONE;
	s_pcLisenceData = NULL;

	/* s_ulMediaStart for DI_AUDIO, DI_VIDEO : DRV_MEDIA_CheckStart() */
	if(s_ulMediaStart > 0)
		--s_ulMediaStart;

	PrintExit;

	return eDIError;
}


/**
 * @brief		media¸¦ Àç»ýÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see			DI_MEDIA_Stop
 */
DI_ERR_CODE DI_MEDIA_MP_Start (HUINT32 unMediaId, HBOOL bPause)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStartSettings playbackstartSettings;
	HUINT8			*pucURI_Data = NULL;
#if SUPPORT_AUDIOCODEC_LPCM
	HINT32			nResult = VK_OK;
	DI_MEDIA_MSG	audMsg;
#endif

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		audMsg.ulSender = DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP;
		audMsg.ulEvent = DI_MEDIA_PLAYPUMPPLAY_PLAY;
		audMsg.ulParam2 = unMediaId;
		audMsg.ulParam3 = s_tStreamInfo[unMediaId].audio_codec;

		VK_TASK_Sleep(1000);
		nResult = VK_MSG_SendTimeout(s_ulMediaPlaypumpStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nResult);
		}

		/* goto Success; */
		(void)VK_SEM_Release(s_ulMediaSemId);
		PrintExit;
		return eDIError;
	}
#endif

	if((s_tMediaInstance[unMediaId].playback == NULL) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_NO_SETUP))
	{
		PrintError("[%s] Error - Not initialized Media.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if((s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_READY) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_STOP))
		{
			if(s_tEventCallback[unMediaId] != NULL)
			{
				s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
			}

			/** Start & Play Playback */
			(void)NEXUS_Playback_GetDefaultStartSettings(&playbackstartSettings);

			if((s_tStreamInfo[unMediaId].video_stream_id == 0) && (s_tStreamInfo[unMediaId].audio_stream_id != 0)
				&& (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_MP3))
			{
				playbackstartSettings.indexFileIo.mode = NEXUS_PlaybackIndexFileIo_eModule;
				playbackstartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
			}
			else
			{
				playbackstartSettings.mode = NEXUS_PlaybackMode_eIndexed;
			}

#if SUPPORT_AUDIOCODEC_LPCM
			if((s_tStreamInfo[unMediaId].RangeAvaliable == FALSE) && (NULL != VK_strstr((char *)s_tMediaInstance[unMediaId].mediaurl, "[DLNA]")))
			{
				playbackstartSettings.indexFileIo.mode = NEXUS_PlaybackIndexFileIo_eModule;
				if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_VOB)||(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES))
				{
					playbackstartSettings.mode = NEXUS_PlaybackMode_eIndexed;
				}
				else
				{
					playbackstartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
				}

				if(s_tStreamInfo[unMediaId].max_bitrate == 0)
				{
					playbackstartSettings.bitrate = 1*1024*1024;
				}
				else
				{
					playbackstartSettings.bitrate = s_tStreamInfo[unMediaId].max_bitrate;
				}
			}
			else
			{
				playbackstartSettings.bitrate = s_tStreamInfo[unMediaId].max_bitrate;
			}
#else
			playbackstartSettings.bitrate = s_tStreamInfo[unMediaId].max_bitrate;
#endif
			/* Force the bitrate value if we can't get it under HTTP chunked live streaming */
			/* because of nexus_playback failture */
			if ((s_tStreamInfo[unMediaId].isHttpChunked == TRUE) && (s_tStreamInfo[unMediaId].max_bitrate == 0))
			{
				playbackstartSettings.bitrate = (1*1024*1024);
			}
			tNexusError = NEXUS_Playback_Start(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].file, &playbackstartSettings);
			if (tNexusError != NEXUS_SUCCESS)
			{
				PrintData("[%s] Error - (1) Fail NEXUS_Playback_Start:%d\n", __func__,tNexusError);
				/* NEXUS_Playback_Start may fail if the AVI file has no index. If so, reopen without an index and try again. */
				(void)NEXUS_FilePlay_Close(s_tMediaInstance[unMediaId].file);
				PrintData("[%s] NEXUS_FilePlay_Close:%d\n", __func__,tNexusError);
				s_tMediaInstance[unMediaId].file = NULL;

				if((NULL != VK_strstr((char *)s_tMediaInstance[unMediaId].mediaurl, "[DLNA]")) && (VK_strlen((char *)s_tMediaInstance[unMediaId].mediaurl) > 12))
				{
					/* URL¿¡ DATA ¹× INDEX¸¦ ºÙ¿© File Open½Ã ¾î´À°ÍÀ» OpenÇÏ´ÂÁö ±¸ºÐÇÔ */
					/* ºÙÀÎ String¿¡ ´ëÇØ DLNA File Library »ç¿ëÇÏ´Â Module¿¡¼­ Á¦°ÅÇÏ¿© »ç¿ëÇÔ */
					pucURI_Data = DD_MEM_Alloc((VK_strlen((char *)s_tMediaInstance[unMediaId].mediaurl)+8));
					if(pucURI_Data == NULL)
					{
						PrintError("[%s] Error - Memory Allocation Error (pucURI_Data)\n", __func__);
						(void)VK_SEM_Release(s_ulMediaSemId);
						return DI_ERR_ERROR;
					}
					else
					{
						VK_memset(pucURI_Data, 0x0, VK_strlen((char *)s_tMediaInstance[unMediaId].mediaurl)+8);
						VK_snprintf((char *)pucURI_Data, (VK_strlen((char *)s_tMediaInstance[unMediaId].mediaurl)+7), "[DLNA][DATA]%s", (char *)(char *)s_tMediaInstance[unMediaId].mediaurl+6);
					}

					PrintData("IPTUNER (DLNA) url(%s)\n", pucURI_Data);
					s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucURI_Data, NULL);

					if (NULL != pucURI_Data)
					{
						DD_MEM_Free(pucURI_Data);
						pucURI_Data = NULL;
					}
				}
				else
				{
					PrintData("IPTUNER url(%s)\n", s_tMediaInstance[unMediaId].mediaurl);

					if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS) ||
						(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192) ||
						(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES) ||
						(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_AVI) ||
						(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT) || /* for SSTR */
						((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)&&(NULL != VK_strstr((char *)s_tMediaInstance[unMediaId].mediaurl, "mms://"))) ||
						((s_tMediaInstance[unMediaId].mediaType == DI_MEDIA_PLAYPUMP_AUDIO_MAIN)&&(s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_MP3)))
					{
						s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)s_tMediaInstance[unMediaId].mediaurl, NULL);
					}
					else
					{
						s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)s_tMediaInstance[unMediaId].mediaurl, (char *)s_tMediaInstance[unMediaId].mediaurl);
					}
				}

				tNexusError = NEXUS_Playback_Start(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].file, &playbackstartSettings);
				if (tNexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] Error - (2) Fail NEXUS_Playback_Start:%d\n", __func__,tNexusError);
					eDIError = DI_ERR_ERROR;
				}
				else
				{
					s_tStreamInfo[unMediaId].support_trick = FALSE;
				}

				PrintData("[%s] Exit NEXUS_Playback_Start Set:%d\n", __func__,tNexusError);
				/*
				* NEXUS_Playback_GetStatusÀÇ Mp3 Position Patching (PH 5.0 ÀÌÈÄ·Î Áö¼Ó ¹Ý¿µµÊ)À¸·Î ÀÎÇØ
				* bmpeg2ts_player »ç¿ë ½Ã PositionÀ» DecoderÀÇ pts Á¤º¸°¡ ¾Æ´Ñ playback positionÀ¸·Î
				* Play TimeÀ» »ç¿ëÇÏ°í ÀÖ±â ¶§¹®¿¡ bmpeg2ts_player°¡ ¿­¸®´Â °æ¿ì(Index Open)¿¡´Â FirstPts
				* º¸Á¤À» ÇÏÁö ¾Êµµ·Ï ¼öÁ¤ÇÔ.
				*/
				if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS ||
				s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
				{
					if(s_tStreamInfo[unMediaId].bProbedFirstPts == TRUE)
					{
						P_MEDIA_InstallFirstPts(unMediaId, s_tStreamInfo[unMediaId].unProbeFirstPts);
					}
					else
						(void) P_MEDIA_RegisterFirstPtsCB (unMediaId, (DI_MEDIA_FirstPtsCallback_t) &P_MEDIA_FirstPtsCallback);
				}
			}

			if(eDIError == DI_ERR_OK)
			{
				if((s_tStreamInfo[unMediaId].eStartCondition == DI_MEDIA_PLAY_START_MODE_LASTVIEW)
					&& (s_tStreamInfo[unMediaId].support_seek == TRUE))
				{
					/** Set Playback Play Position */
					(void)NEXUS_Playback_Seek(s_tMediaInstance[unMediaId].playback, (NEXUS_PlaybackPosition)s_tStreamInfo[unMediaId].ulStartTime);
				}

				/** Start Playback */
				if(bPause != TRUE)
				{
					tNexusError = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
					if(tNexusError != NEXUS_SUCCESS)
					{
						PrintError("[%s] Error - Fail NEXUS_Playback_Play:%d\n", __func__, tNexusError);
						eDIError = DI_ERR_ERROR;
					}
					else
					{
						if(s_tEventCallback[unMediaId] != NULL)
						{
							s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_START, 0);
						}
						else
						{
							PrintDebug("event callback is NULL\n");
						}
#if defined(CONFIG_IPTUNER)
						if(DI_IPTUNER_GetUnderflowState() != 33)
						{
							DI_IPTUNER_SetUnderflowState(33);
						}
#endif
						s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
						s_tMediaInstance[unMediaId].trickrate = DI_MEDIA_TRICK_1FFSTEP;
					}
				}
				else
				{
					tNexusError = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
					if(tNexusError != NEXUS_SUCCESS)
					{
						PrintError("[%s] Error - Fail NEXUS_Playback_Play:%d\n", __func__, tNexusError);
						eDIError = DI_ERR_ERROR;
					}
					else
					{
						(void)NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);

						if(s_tEventCallback[unMediaId] != NULL)
						{
							s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PAUSE, 0);
						}
						else
						{
							PrintDebug("event callback is NULL\n");
						}

						s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PAUSE;
						s_tMediaInstance[unMediaId].trickrate = DI_MEDIA_TRICK_PAUSE;
					}
				}
			}
		}
		else if(s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PAUSE)
		{
			/** Start Playback */
			tNexusError = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
			if(tNexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Error - Fail NEXUS_Playback_Play:%d\n", __func__, tNexusError);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
				if(s_tEventCallback[unMediaId] != NULL)
				{
					s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_START, 0);
				}
#if defined(CONFIG_IPTUNER)
				if(DI_IPTUNER_GetUnderflowState() != 33)
				{
					DI_IPTUNER_SetUnderflowState(33);
				}
#endif
				s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
				s_tMediaInstance[unMediaId].trickrate = DI_MEDIA_TRICK_1FFSTEP;
			}
		}
		else
		{
			PrintError("[%s] Error - Already Playback is start state.:%d\n", __func__, tNexusError);
			eDIError = DI_ERR_ERROR;
		}
	}

/* Success: */
	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		media Àç»ýÀ» StopÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see			DI_MEDIA_Play
 */
DI_ERR_CODE DI_MEDIA_MP_Stop(HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
#if SUPPORT_AUDIOCODEC_LPCM
	HINT32			nResult = VK_OK;
	DI_MEDIA_MSG	audMsg;
#endif

	PrintEnter;

#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tMediaInstance[unMediaId].playback == NULL) && (s_tStreamInfo[unMediaId].stream_type != DI_MEDIA_MPEG_TYPE_WAVE))
#else
	if(s_tMediaInstance[unMediaId].playback == NULL)
#endif
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}
#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)
	{
		int msg[4];
		msg[0] = DRV_MEDIA_SETPOSITION_ACCURACY_QUIT_MEDIA;
		VK_MSG_Send(sulStatusMonitorThreadQ, msg, 4*sizeof(unsigned long));
	}
#endif
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)
	s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_NONE;
#endif
#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		audMsg.ulSender = DI_MEDIA_PLAY_STREAM_FROM_PLAYPUMP;
		audMsg.ulEvent = DI_MEDIA_PLAYPUMPPLAY_STOP;
		audMsg.ulParam2 = unMediaId;
		audMsg.ulParam3 = s_tStreamInfo[unMediaId].audio_codec;

		nResult = VK_MSG_SendTimeout(s_ulMediaPlaypumpStreamMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nResult);
		}

		/* goto Success; */
		(void)VK_SEM_Release(s_ulMediaSemId);
		PrintExit;
		return eDIError;
	}
#endif
	if((s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PAUSE) ||
		(s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_TRICK) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PLAY))
	{
		/** Stop Playback */
		(void)NEXUS_Playback_Stop(s_tMediaInstance[unMediaId].playback);

		if(s_tEventCallback[unMediaId] != NULL)
		{
			s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_STOP, 0);
		}

		s_nCurrentPlayTime[unMediaId] = 0;
		s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_STOP;
	}
	else
	{
		PrintError("[%s] Error - Not Play Media.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}

/* Success:*/

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		Àç»ý ¼Óµµ¸¦ º¯°æÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	eTrickSpeed 		º¯°æÇÏ°íÀÚ ÇÏ´Â ¹è¼Ó Á¤º¸¸¦ ÁöÁ¤ÇÑ´Ù.
 * @return		DI Error Code
 * @see			DI_MEDIA_GetSpeed
 */
DI_ERR_CODE DI_MEDIA_MP_SetSpeed(HUINT32 unMediaId, DI_MEDIA_TrickSpeed_e eTrickSpeed)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackTrickModeSettings tTrickModeSettings;

	PrintEnter;

#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		if(eTrickSpeed == DI_MEDIA_TRICK_PAUSE)
		{
			if(s_tEventCallback[unMediaId] != NULL)
			{
				s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PAUSE, 0);
			}

			s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PAUSE;
		}
		else if(eTrickSpeed == DI_MEDIA_TRICK_1FFSTEP)
		{
			if(s_tEventCallback[unMediaId] != NULL)
			{
				s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PLAYING, 0);
			}
#if defined(CONFIG_IPTUNER)
			if(DI_IPTUNER_GetUnderflowState() != 33)
			{
				DI_IPTUNER_SetUnderflowState(33);
			}
#endif
			s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
		}
		else
		{
			PrintData("[%s] Playpump Stream isn't support playback trick\n", __FUNCTION__);
		}

		return DI_ERR_OK;
	}
#endif

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	/** Set Trick Rate */
	(void)NEXUS_Playback_GetDefaultTrickModeSettings(&tTrickModeSettings);

	if((eTrickSpeed >= DI_MEDIA_TRICK_1FFSTEP) && (eTrickSpeed <= DI_MEDIA_TRICK_4FFSTEP))
	{
		tTrickModeSettings.maxDecoderRate = (unsigned)P_MEDIA_ConvertDITrickSpeedToNexus(eTrickSpeed);
		tTrickModeSettings.rate = P_MEDIA_ConvertDITrickSpeedToNexus(eTrickSpeed);
	}
	else
	{
		tTrickModeSettings.rate = P_MEDIA_ConvertDITrickSpeedToNexus(eTrickSpeed);
	}

	switch(eTrickSpeed)
	{
		case DI_MEDIA_TRICK_PAUSE:
			tNexusError = NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);
			if(NEXUS_SUCCESS != tNexusError)
			{
				PrintError("[%s] Error - Fail NEXUS_Playback_Pause:%d\n", __func__, tNexusError);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
				if(s_tEventCallback[unMediaId] != NULL)
				{
					s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PAUSE, 0);
				}

				s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PAUSE;
			}
			break;
		case DI_MEDIA_TRICK_1FFSTEP:
			/** Start Playback */
			tNexusError = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
			if(tNexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Error - Fail NEXUS_Playback_Play:%d\n", __func__, tNexusError);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
#if defined(CONFIG_IPTUNER)
				/* À§¿¡¼­ Play ¿äÃ»ÀÌ µé¾î¿À¸é Ready State¸¦ °­Á¦·Î BUFFERING COMPLETE·Î ¹Ù²ß´Ï´Ù. Underflow·Î ºüÁö°Ô ÇÏ±â À§ÇÔÀÔ´Ï´Ù.
				33¹øÀº DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED ÀÔ´Ï´Ù. Header Include¸¦ Á¤¸®ÇÏ¸é¼­ ¼öÁ¤ÀÌ ÇÊ¿äÇÕ´Ï´Ù. */
				if(DI_IPTUNER_GetUnderflowState() != 33)
				{
					DI_IPTUNER_SetUnderflowState(33);
				}
#endif
				if(s_tEventCallback[unMediaId] != NULL)
				{
					s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PLAYING, 0);
				}

				s_tMediaInstance[unMediaId].trickrate = DI_MEDIA_TRICK_1FFSTEP;
				s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
			}
			break;
		/** Trick(Forward) */
		case DI_MEDIA_TRICK_2FFSTEP:
		case DI_MEDIA_TRICK_3FFSTEP:
		case DI_MEDIA_TRICK_4FFSTEP:
		case DI_MEDIA_TRICK_5FFSTEP:
		case DI_MEDIA_TRICK_6FFSTEP:
		case DI_MEDIA_TRICK_7FFSTEP:
		case DI_MEDIA_TRICK_8FFSTEP:
		case DI_MEDIA_TRICK_9FFSTEP:
		case DI_MEDIA_TRICK_10FFSTEP:
		case DI_MEDIA_TRICK_11FFSTEP:
		case DI_MEDIA_TRICK_12FFSTEP:
		case DI_MEDIA_TRICK_13FFSTEP:
		case DI_MEDIA_TRICK_14FFSTEP:
		case DI_MEDIA_TRICK_15FFSTEP:
		case DI_MEDIA_TRICK_16FFSTEP:
		/** Trick(Rewind) */
		case DI_MEDIA_TRICK_1RWSTEP:
		case DI_MEDIA_TRICK_2RWSTEP:
		case DI_MEDIA_TRICK_3RWSTEP:
		case DI_MEDIA_TRICK_4RWSTEP:
		case DI_MEDIA_TRICK_5RWSTEP:
		case DI_MEDIA_TRICK_6RWSTEP:
		case DI_MEDIA_TRICK_7RWSTEP:
		case DI_MEDIA_TRICK_8RWSTEP:
		case DI_MEDIA_TRICK_9RWSTEP:
		case DI_MEDIA_TRICK_10RWSTEP:
		case DI_MEDIA_TRICK_11RWSTEP:
		case DI_MEDIA_TRICK_12RWSTEP:
		case DI_MEDIA_TRICK_13RWSTEP:
		case DI_MEDIA_TRICK_14RWSTEP:
		case DI_MEDIA_TRICK_15RWSTEP:
		case DI_MEDIA_TRICK_16RWSTEP:
			if(s_tStreamInfo[unMediaId].support_trick != TRUE)
			{
				PrintError("[%s] Error - Not Support Trick in The Media Stream.\n", __func__);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
				if((s_nCurrentPlayTime[unMediaId] == 0) && ((eTrickSpeed <= DI_MEDIA_TRICK_16RWSTEP) && (eTrickSpeed >= DI_MEDIA_TRICK_1RWSTEP)))
				{
					PrintError("[%s] Error - When current Position is 0, Fast Rewind is not supported\n", __func__);
					eDIError = DI_ERR_ERROR;
				}
				else
				{
					tNexusError = NEXUS_Playback_TrickMode(s_tMediaInstance[unMediaId].playback, &tTrickModeSettings);
					if(NEXUS_SUCCESS != tNexusError)
					{
						PrintError("[%s] Error - Fail NEXUS_Playback_TrickMode:%d\n", __func__, tNexusError);
						eDIError = DI_ERR_ERROR;
					}
					else
					{
						if(s_tEventCallback[unMediaId] != NULL)
						{
							s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PLAYING, s_nCurrentPlayTime[unMediaId]);
						}

						if(s_tEventCallback[unMediaId] != NULL)
						{
							s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_TRICK, s_nCurrentPlayTime[unMediaId]);
						}

						s_tMediaInstance[unMediaId].trickrate = eTrickSpeed;
						s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_TRICK;
					}
				}
			}
			break;
		default:
			{
				PrintError("[%s] Error - Invalid Type:%d\n", __func__, eTrickSpeed);
				eDIError = DI_ERR_ERROR;
			}
			break;
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		ÇöÀç Àç»ý ¼Óµµ¸¦ ¹ÝÈ¯ÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[out]	eTrickSpeed			ÇöÀç ¹è¼Ó Á¤º¸¸¦ ´ã´Â´Ù.
 * @return		DI Error Code
 * @see			DI_MEDIA_SetSpeed
 */
DI_ERR_CODE DI_MEDIA_MP_GetSpeed(HUINT32 unMediaId, DI_MEDIA_TrickSpeed_e *peTrickSpeed)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStatus tPlaybackStatus;

	PrintEnter;

#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		PrintData("[%s] Playpump Stream isn't support playback trick\n", __FUNCTION__);
		return DI_ERR_OK;
	}
#endif

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	(void)VK_memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));
	/** Get Playback Status */
	tNexusError = NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
	if(tNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error - Fail NEXUS_Playback_GetStatus:%d\n", __func__, tNexusError);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		/** Get Trick Rate */
		if(tPlaybackStatus.state == NEXUS_PlaybackState_eTrickMode)
		{
			*peTrickSpeed = s_tMediaInstance[unMediaId].trickrate;
		}
		else if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
		{
			*peTrickSpeed = DI_MEDIA_TRICK_PAUSE;
		}
		else if(tPlaybackStatus.state == NEXUS_PlaybackState_ePlaying)
		{
			*peTrickSpeed = DI_MEDIA_TRICK_1FFSTEP;
		}
		else
		{
			PrintError("[%s] Error - Not start Playback.\n", __func__);
			eDIError = DI_ERR_ERROR;
		}
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		millisecond ´ÜÀ§·Î ÁöÁ¤µÇ´Â positionÀ¸·Î Àç»ý À§Ä¡¸¦ º¯°æÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	ullTime 			ÀÌµ¿ÇÏ°íÀÚ ÇÏ´Â À§Ä¡¸¦ millisecond ´ÜÀ§·Î ÁöÁ¤ÇÑ´Ù.
 * @return		DI Error Code
 * @see			DI_MEDIA_GetPosition
 */
DI_ERR_CODE DI_MEDIA_MP_SetPosition(HUINT32 unMediaId, HUINT32 ulTime, DI_MEDIA_TIME_TYPE eTimeType)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	HUINT32 ulSetTime = 0;

	PrintEnter;

#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		PrintData("[%s] Playpump Stream isn't support playback trick\n", __FUNCTION__);
		return DI_ERR_OK;
	}
#endif

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}
#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)


		s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_NONE;


		NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);


#endif

	if(s_tStreamInfo[unMediaId].support_seek != TRUE)
	{
		PrintError("[%s] Error - Not Support Seek in The Media Stream.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		/** Set Playback Play Position */
		if(eTimeType == DI_MEDIA_TIME_SEC)
		{
			ulSetTime = ulTime * 1000;
		}
		else if(eTimeType == DI_MEDIA_TIME_MSEC)
		{
			ulSetTime = ulTime;
		}
		else
		{
			PrintError("[%s] Error - Not Support Time Type:%d\n", __func__, eTimeType);
			eDIError = DI_ERR_ERROR;
		}

		tNexusError = NEXUS_Playback_Seek(s_tMediaInstance[unMediaId].playback, (NEXUS_PlaybackPosition)ulSetTime);
		if(NEXUS_SUCCESS != tNexusError)
		{
			PrintError("[%s] Error - Fail NEXUS_Playback_Seek:%d\n", __func__, tNexusError);
			eDIError = DI_ERR_ERROR;
		}
	}
#if defined(USE_ENHANCED_SETPOSITION_ACCURACY)
	s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0;
	{
			int msg[4];
			msg[0] = DRV_MEDIA_SETPOSITION_ACCURACY_ENTER_SET_POS;
			VK_MSG_Send(sulStatusMonitorThreadQ, msg, 4*sizeof(long));
	}
#endif
	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		ÇöÀç Àç»ý ÁßÀÎ À§Ä¡¸¦ millisecond ´ÜÀ§·Î ¹ÝÈ¯ÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[out]	pullTime			ÇöÀç Àç»ý ÁßÀÎ À§Ä¡ Á¤º¸¸¦ ´ã´Â´Ù.
 * @return		DI Error Code
 * @see			DI_MEDIA_SetPosition
 */
DI_ERR_CODE DI_MEDIA_MP_GetPosition(HUINT32 unMediaId, HUINT32 *pulTime, DI_MEDIA_TIME_TYPE eTimeType)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStatus tPlaybackStatus;
	HUINT64 ullPosition = 0;
	HUINT32 unPtsSnapShot = 0;
	PrintEnter;

#if SUPPORT_AUDIOCODEC_LPCM
	if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_WAVE) && (s_tStreamInfo[unMediaId].audio_codec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE))
	{
		PrintData("[%s] Playpump Stream isn't support playback trick\n", __FUNCTION__);
		return DI_ERR_OK;
	}
#endif

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	if(s_bPlayTimerMode[unMediaId] == DI_MEDIA_TIMERMODE_NONE)
	{
		PrintData("[%s] TimeMode=FALSE, s_nCurrentPlayTime=%d\n", __func__, s_nCurrentPlayTime[unMediaId]);
		if(eTimeType == DI_MEDIA_TIME_SEC)
		{
			*pulTime = s_nCurrentPlayTime[unMediaId];
		}
		else if(eTimeType == DI_MEDIA_TIME_MSEC)
		{
			*pulTime = s_nCurrentPlayTime[unMediaId]*1000;
		}
	}
	else
	{
#if 0 /* Play TimeÀº Index°¡ ¾ø¾îµµ µ¿ÀÛÇÏµµ·Ï Patch */
		if(s_tStreamInfo[unMediaId].support_seek != TRUE)
		{
			PrintError("[%s] Error - Not Support Seek in The Media Stream.\n", __func__);
			eDIError = DI_ERR_ERROR;
		}
		else
		{
			(void)VK_memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));
			/** Get Playback Status */
			tNexusError = NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
			if(tNexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Error - Fail NEXUS_Playback_GetStatus:%d\n", __func__, tNexusError);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
				/** Get Playback Play Position */
				*pulTime = ((HUINT32)tPlaybackStatus.position/1000);
				PrintData("position = %d\n", *pulTime);
			}
		}
#else
		(void)VK_memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));
		/** Get Playback Status */
		if((s_tMediaInstance[unMediaId].trickrate > DI_MEDIA_TRICK_4FFSTEP)||
			   (s_tMediaInstance[unMediaId].trickrate < DI_MEDIA_TRICK_16RWSTEP))
		{
			tNexusError= NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
			if(tNexusError != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Playback_GetStatus failed\n");
				(void)VK_SEM_Release(s_ulMediaSemId);
				return DI_ERR_ERROR;
			}
			ullPosition = tPlaybackStatus.position;
		}
		else
		{
			tNexusError = b_play_getpts (s_tMediaInstance[unMediaId].playback, &unPtsSnapShot);
			if(tNexusError != NEXUS_SUCCESS)
			{
				/* workaournd */
				if(eTimeType == DI_MEDIA_TIME_SEC)
				{
					ullPosition = s_nCurrentPlayTime[unMediaId];
				}
				else if(eTimeType == DI_MEDIA_TIME_MSEC)
				{
					ullPosition = s_nCurrentPlayTime[unMediaId]*1000;
				}
			}
			else
			{
				ullPosition = bmedia_pts2time(unPtsSnapShot-s_tMediaInstance[unMediaId].unFirstPts, BMEDIA_TIME_SCALE_BASE);
				if (ullPosition > s_tStreamInfo[unMediaId].duration)
				{
					ullPosition = s_tStreamInfo[unMediaId].duration;
				}
			}
		}
		if((s_bPlayTimerMode[unMediaId] == DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0) && ((HUINT32)ullPosition == 0))
		{
			PrintData("[%s] TimeMode=FALSE, s_nCurrentPlayTime=%d\n", __func__, s_nCurrentPlayTime[unMediaId]);
			if(eTimeType == DI_MEDIA_TIME_SEC)
			{
				*pulTime = s_nCurrentPlayTime[unMediaId];
			}
			else if(eTimeType == DI_MEDIA_TIME_MSEC)
			{
				*pulTime = s_nCurrentPlayTime[unMediaId]*1000;
			}
		}
		else
		{
			/** Get Playback Play Position */
			if(eTimeType == DI_MEDIA_TIME_SEC)
			{
				*pulTime = ((HUINT32)ullPosition/1000);
			}
			else if(eTimeType == DI_MEDIA_TIME_MSEC)
			{
				*pulTime = ((HUINT32)ullPosition);
			}
			else
			{
				PrintError("[%s] Error - Not Support Time Type:%d\n", __func__, eTimeType);
				eDIError = DI_ERR_ERROR;
			}
		}
#if 0	// Playback Àº º¸Á¤ÇÒ ÇÊ¿ä ¾øÀ½. decoder ¿¡¼­ °¡Á®¿Â pts ´Â bmedia_pts2time ·Î ÀÌ¹Ì º¸Á¤µÇ¾úÀ½. °ü·Ã redmine : #71821
		if( s_tMediaInstance[unMediaId].bPtsInstalled == TRUE)
		{
			if(((HUINT32)ullPosition) > s_tMediaInstance[unMediaId].unFirstPts)
			{
				if(eTimeType == DI_MEDIA_TIME_SEC)
				{
					*pulTime = ((HUINT32)ullPosition/1000) - s_tMediaInstance[unMediaId].unFirstPts / 1000;
				}
				else if(eTimeType == DI_MEDIA_TIME_MSEC)
				{
					*pulTime = ((HUINT32)ullPosition) - s_tMediaInstance[unMediaId].unFirstPts;
				}
				else
				{
					PrintError("[%s] Error - Not Support Time Type:%d\n", __func__, eTimeType);
					eDIError = DI_ERR_ERROR;
				}
			}
			else
			{
				*pulTime = 0;
			}
		}
#endif
		PrintData("position = %d\n", *pulTime);
#endif
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		media fileÀÇ Á¤º¸¸¦ ¹ÝÈ¯ÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[out]	pStreamInfo			media fileÀÇ Á¤º¸¸¦ Æ÷ÇÔÇÏ´Â ±¸Á¶Ã¼ pointer
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_GetStreamInfo(HUINT32 unMediaId, DI_MEDIA_INFO_t *pStreamInfo)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	if(pStreamInfo == NULL)
	{
		PrintError("[%s] Error - pStreamInfo is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	/** Get Stream Info */
	*pStreamInfo = s_tStreamInfo[unMediaId];

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		Playpump¸¦ FlushÇÑ´Ù. (ÇöÀç´Â Test FunctionÀÓ)
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see			DI_MEDIA_SetPosition
 */
DI_ERR_CODE DI_MEDIA_MP_FlushPlaypump(HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	tNexusError = NEXUS_Playpump_Flush(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
	if(tNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error - Fail NEXUS_Playback_GetStatus:%d\n", __func__, tNexusError);
		eDIError = DI_ERR_ERROR;
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		callback ÇÔ¼ö¸¦ µî·ÏÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	callback	BOS(Begin of Stream), EOS(End of Stream) event¸¦ ¹Þ±â À§ÇÑ Callback
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_RegisterCallback(HUINT32 unMediaId, DI_MEDIA_EventCallback_t MediaEventcallback)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	if(MediaEventcallback == NULL)
	{
		PrintError("[%s] Error - MediaEventcallback is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	s_tEventCallback[unMediaId] = MediaEventcallback;

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		µî·ÏµÈ callback ÇÔ¼ö¸¦ ÇØÁ¦ÇÑ´Ù.
 *
 * @param[in]	unMediaId			Di Media ID
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_MP_UnregisterCallback(HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		return eDIError;
	}

	if(s_tEventCallback[unMediaId] != NULL)
	{
		s_tEventCallback[unMediaId] = NULL;
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return eDIError;
}

/**
 * @brief		StreamÀÇ Path¸¦ ÀÌ¿ëÇÏ¿© Probe¸¦ ÇÑ´Ù.
 *
 * @param[in]	url 				StreamÀÇ Path
 * @param[out]	stMediaFileInfo 	ProbeµÈ StreamÀÇ Á¤º¸
 * @return		DI Error Code
 * @see			DI_MEDIA_PrintProbeInfo
 */
DI_ERR_CODE DI_MEDIA_Probe (HUINT8 *url, DI_MEDIA_INFO_t *stMediaFileInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	stMediaInfo  *pMfpInfo = NULL;
	HINT32 i=0;

	PrintEnter;

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	for (i=0;i<DI_MEDIA_MAX_NUM;i++)
	{
		if (!s_MfpInfo[i].opened)
		{
			pMfpInfo = &s_MfpInfo[i];
			break;
		}
		/* else { continue } */
	}

	if (pMfpInfo == NULL)
	{
		nRet = DI_ERR_NO_RESOURCE;
	}
	else
	{
		nRet = P_MEDIA_ProbeOpen (url, pMfpInfo);
		if (nRet == DI_ERR_OK)
		{
			VK_memcpy(stMediaFileInfo, &pMfpInfo->StreamInfo, sizeof(DI_MEDIA_INFO_t));
		}
		else
		{
			PrintError ("[%s] Error - Fail in P_MEDIA_ProbeOpen\n",__FUNCTION__);
		}
		(void) P_MEDIA_ProbeClose (pMfpInfo);
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

	PrintExit;

	return nRet;

}

/**
 * @brief		parameter·Î ¹ÞÀº Stream Info¸¦ log·Î DisplayÇÑ´Ù.
 *
 * @param[in]	stMediaFileInfo	log·Î DisplayÇÏ·Á´Â Stream Info
 * @return		void
 * @see			DI_MEDIA_Probe
 */
void DI_MEDIA_PrintProbeInfo(DI_MEDIA_INFO_t *stMediaFileInfo)
{
	HUINT32 i;

	PrintEnter;

	PrintData ("stream_type       (%d)\n",stMediaFileInfo->stream_type);
	PrintData ("max_bitrate       (%u)\n",stMediaFileInfo->max_bitrate);
	PrintData ("duration          (%u)\n",    stMediaFileInfo->duration);
	PrintData ("nprograms         (%u)\n",    stMediaFileInfo->nprograms);
	PrintData ("ntracks           (%u)\n",    stMediaFileInfo->ntracks);
	PrintData ("video_pes_id      (%u)\n",stMediaFileInfo->video_pes_id);
	PrintData ("video_stream_id   (%u)\n",    stMediaFileInfo->video_stream_id);
	PrintData ("video_codec       (%u)\n",stMediaFileInfo->video_codec);
	PrintData ("audio_pes_id      (%u)\n",stMediaFileInfo->audio_pes_id);
	PrintData ("audio_stream_id   (%u)\n",    stMediaFileInfo->audio_stream_id);
	PrintData ("audio_codec       (%u)\n",stMediaFileInfo->audio_codec);
	PrintData ("support_trick     (%u)\n",stMediaFileInfo->support_trick);
	PrintData ("support_seek      (%u)\n",stMediaFileInfo->support_seek);
	PrintData ("audiodecodertype     (%u)\n",stMediaFileInfo->audiodecodertype);
	PrintData ("videodecodertype      (%u)\n",stMediaFileInfo->videodecodertype);
	PrintData ("eStartCondition     (%u)\n",stMediaFileInfo->eStartCondition);
	PrintData ("ulStartTime      (%u)\n",stMediaFileInfo->ulStartTime);
	PrintData ("demuxId      (%u)\n",stMediaFileInfo->demuxId);
	PrintData ("pcr_pid           (%u)\n",    stMediaFileInfo->pcr_pid);
	PrintData ("is_drm            (%u)\n",    stMediaFileInfo->is_drm);

#if defined(CONFIG_MEDIA_PROBE_PMT)
	PrintData ("program_number	   (%u)\n",    stMediaFileInfo->program_number);
	PrintData ("pmt_pid 		   (%u)\n",    stMediaFileInfo->pmt_pid);
#endif

#if defined(CONFIG_MEDIA_PROBE_CA_DESC)
	PrintData ("ca_system_id_cnt   (%u)\n",    stMediaFileInfo->ca_system_id_cnt);
	for(i=0;i<stMediaFileInfo->ca_system_id_cnt;i++)
	{
		PrintData ("ca_system_id   (0x%04x)\n",    stMediaFileInfo->ca_system_id[i]);
	}
#endif

	PrintData ("Total Video Track	   (%u)\n",stMediaFileInfo->ucTotalVideoTrack);
	for(i=0;i<stMediaFileInfo->ucTotalVideoTrack;i++)
	{
		PrintData ("------------------------------------------------\n");
		PrintData ("Video Track Num      (%u)\n", i);
		PrintData ("video_pes_id      (%u)\n",stMediaFileInfo->stVideoTrack[i].video_pes_id);
		PrintData ("video_stream_id   (%u)\n",stMediaFileInfo->stVideoTrack[i].video_stream_id);
		PrintData ("video_codec       (%u)\n",stMediaFileInfo->stVideoTrack[i].video_codec);
	}

	PrintData ("Total audio track	   (%u)\n",stMediaFileInfo->ucTotalAudioTrack);
	for(i=0;i<stMediaFileInfo->ucTotalAudioTrack;i++)
	{
		PrintData ("------------------------------------------------\n");
		PrintData ("audio track Num	   (%u)\n", i);
		PrintData ("audio_pes_id	   (%u)\n",stMediaFileInfo->stAudioTrack[i].audio_pes_id);
		PrintData ("audio_stream_id   (%u)\n",stMediaFileInfo->stAudioTrack[i].audio_stream_id);
		PrintData ("audio_codec	   (%u)\n",stMediaFileInfo->stAudioTrack[i].audio_codec);
		PrintData ("audio_language	   (%s)\n",stMediaFileInfo->stAudioTrack[i].uacAudioLang);
	}

	PrintData ("------------------------------------------------\n");
	PrintData ("Total Xsub track	   (%u)\n",stMediaFileInfo->ucTotalXsubTrack);
	for(i=0;i<stMediaFileInfo->ucTotalXsubTrack;i++)
	{
		PrintData ("------------------------------------------------\n");
		PrintData ("Divx Subtitle Track       (%u)\n", i);
		PrintData ("Xsub Type      (%u)\n", stMediaFileInfo->stXsubTrack[i].eXsubType);
		PrintData ("Xsub Language       (%s)\n",stMediaFileInfo->stXsubTrack[i].uacXsubLang);
	}

	PrintExit;
}

/**
 * @brief		Media Device ¼ö¸¦ ¹ÝÈ¯ÇÑ´Ù.
 *
 * @param[out]	pulNumOfDeviceId	Media Device Number
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_GetCapability(HUINT32 *pulNumOfDeviceId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;

	PrintEnter;

	if(pulNumOfDeviceId == NULL)
	{
		PrintError("[%s] Error -pulNumOfDeviceId is null\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		*pulNumOfDeviceId = DI_MEDIA_MAX_NUM;
	}

	PrintExit;

	return eDIError;
}

/**
 * @brief		Playback BufferÀÇ Á¤º¸¸¦ ¹ÝÈ¯ÇÑ´Ù.
 *
 * @param[in]	unMediaId				Media Device Id
 * @param[out]	pnCurrentUsedBufferSize	Playback Current Buffer
 * @param[out]	pnMaxBufferSize			Playback Max Buffer
 * @return		DI Error Code
 * @see
 */
DI_ERR_CODE DI_MEDIA_GetPlaybackBuffer(HUINT32 unMediaId, HUINT32 *pnCurrentUsedBufferSize,HUINT32 *pnMaxBufferSize)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	NEXUS_PlaybackStatus tPlaybackStatus;

	VK_MEM_Memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));

	PrintEnter;

	(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
	*pnCurrentUsedBufferSize = tPlaybackStatus.fifoDepth;
	*pnMaxBufferSize = tPlaybackStatus.fifoSize;

	PrintExit;

	return eDIError;
}

/**
 * @brief			multi track audioÀÇ °æ¿ì trackÀ» º¯°æÇÑ´Ù.
 *
 * @param[in]		pcMediaPath		Media Path
 * @param[out]		ptMediaInfo		Media Info
 * @return			HERROR
 * @date 			2010/08/16
 * @author 			kimdh1@humaxdigital.com
 * @see
 */
DI_ERR_CODE DI_MEDIA_SetAudioTrack(HUINT32 unMediaId, HUINT8	ucAudioTrack)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;

	PrintEnter;

	s_tStreamInfo[unMediaId].audio_pes_id = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_pes_id;
	s_tStreamInfo[unMediaId].audio_stream_id = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_stream_id;
	s_tStreamInfo[unMediaId].audio_codec = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_codec;

	PrintExit;

	return eDIError;
}

/**
 * @brief			PMT Update ½Ã ¹Ý¿µÇÑ DI_MEDIA_INFO_t ±¸Á¶Ã¼ ³»¿ëÀÇ ÀÏºÎ¸¦
 *					s_tStreamInfo Àü¿ª º¯¼ö¿¡ Àû¿ëÇÑ´Ù.
 *
 * @param[in]		unMediaId			Media Device ID
 * @param[in]		pStreamInfo			Media Info (DI_MEDIA_INFO_t ±¸Á¶Ã¼)
 * @return			DI_ERR_CODE
 * @date 			2012/05/03
 * @author 			thoh@humaxdigital.com
 * @see
 * @remark			¸¸¾à A/V TrackÀÌ PMT Update ÈÄ 2°³¿¡¼­ 1°³·Î ÁÙ¾îµé¸é, Áö¿öÁ®¾ßÇÏ´Â Æ®·¢ÀÇ Á¤º¸°¡ ÃÊ±âÈ­µÇÁö´Â ¾ÊÁö¸¸,
 *					Total Track °³¼ö¿¡¼­ °É·¯ÁÖ¹Ç·Î ¹®Á¦´Â ¾ø´Ù. ÇÏÁö¸¸ ÃßÈÄ ÃÊ±âÈ­¿¡ ´ëÇØ¼­ »ý°¢À» ÇÒ ÇÊ¿ä°¡ ÀÖ´Ù.
 */
DI_ERR_CODE DI_MEDIA_UpdatePmtInfo(HUINT32 unMediaId, DI_MEDIA_INFO_t *pStreamInfo)
{
	HUINT32 idx = 0;
	DI_ERR_CODE eDiRet = DI_ERR_OK;

	if(pStreamInfo == NULL)
	{
		PrintError("[%s] pStreamInfo is NULL\n", __FUNCTION__);
		eDiRet = DI_ERR_ERROR;
		goto error;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		eDiRet = DI_ERR_ERROR;
		goto error;
	}

	eDiRet = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __FUNCTION__);
		(void)VK_SEM_Release(s_ulMediaSemId);
		goto error;
	}

	s_tStreamInfo[unMediaId].pcr_pid = pStreamInfo->pcr_pid;

	s_tStreamInfo[unMediaId].ucTotalVideoTrack = pStreamInfo->ucTotalVideoTrack;
	for(idx = 0; idx < s_tStreamInfo[unMediaId].ucTotalVideoTrack; idx++)
	{
		s_tStreamInfo[unMediaId].stVideoTrack[idx].video_stream_id = pStreamInfo->stVideoTrack[idx].video_stream_id;
		s_tStreamInfo[unMediaId].stVideoTrack[idx].video_codec = pStreamInfo->stVideoTrack[idx].video_codec;
	}

	s_tStreamInfo[unMediaId].ucTotalAudioTrack = pStreamInfo->ucTotalAudioTrack;
	for(idx = 0; idx < s_tStreamInfo[unMediaId].ucTotalAudioTrack; idx++)
	{
		s_tStreamInfo[unMediaId].stAudioTrack[idx].audio_stream_id = pStreamInfo->stAudioTrack[idx].audio_stream_id;
		s_tStreamInfo[unMediaId].stAudioTrack[idx].audio_codec = pStreamInfo->stAudioTrack[idx].audio_codec;
		VK_strncpy(s_tStreamInfo[unMediaId].stAudioTrack[idx].uacAudioLang, pStreamInfo->stAudioTrack[idx].uacAudioLang, 256);
	}

	(void)VK_SEM_Release(s_ulMediaSemId);

error:
	return eDiRet;
}

/**
 * @brief		Current Time±â´ÉÀ» ¼³Á¤ÇÑ´Ù.
 *
 * @param[in]		unMediaId		Media Id
 * @param[in]		bTimerMode		Time Mode
 * @return			HERROR
 * @date 			2010/10/13
 * @author 			kimdh1@humaxdigital.com
 * @see
 */
DI_ERR_CODE DI_MEDIA_SetPlayTimer(HUINT32 unMediaId, DI_MEDIA_TIMERMODE_e	bTimerMode)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;

	PrintEnter;

	s_bPlayTimerMode[unMediaId] = bTimerMode;

	PrintExit;

	return eDIError;
}

/**
 * @brief			UrlÀ» ÀÌ¿ëÇÏ¿© Media¿¡ ´ëÇÑ MediaInfo¸¦ ¾ò¾î¿Â´Ù.
 *
 * @param[in]		pcMediaPath		Media Path
 * @param[out]		ptMediaInfo		Media Info
 * @return			HERROR
 * @date 			2009/06/11
 * @author 			kimdh1@humaxdigital.com
 * @see
 */
DI_ERR_CODE DI_MEDIA_GetMediaInfo(HINT8 *pcMediaPath, DI_MEDIA_MEDIAINFO_t *ptMediaInfo)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	DI_MEDIA_MEDIAINFO_t tMediaInfo;
	FILE *fp = NULL;
	HINT8 buf[DI_MEDDIA_AVI_CMD_BUFSIZE];
	HINT8 cmd[DI_MEDDIA_AVI_CMD_BUFSIZE];
	HINT8 command[DI_MEDDIA_AVI_CMD_BUFSIZE];
	char *cMediaPath_Buf = NULL;
	char *cParsing_Buf = NULL;
	char *cTemp_Buf = NULL;
	int nCmdLength = 0;
	int n = 0;
	int nFileLength=0;
	unsigned int i, j = 0;

	HINT8 cTmpVideoSize[10];
	HINT8 cTmpVideoWidth[5];
	HINT8 cTmpVideoHeight[5];

	PrintEnter;

	VK_memset(buf,0,sizeof(buf));
	VK_memset(cmd,0,sizeof(cmd));
	VK_memset(cmd,0,sizeof(command));

	VK_memset(&tMediaInfo,0,sizeof(tMediaInfo));
	VK_memset(cTmpVideoSize,0,sizeof(cTmpVideoSize));
	VK_memset(cTmpVideoWidth,0,sizeof(cTmpVideoWidth));
	VK_memset(cTmpVideoHeight,0,sizeof(cTmpVideoHeight));

	if(pcMediaPath == NULL)
	{
		PrintError("[%s] Error -pcMediaPath is null\n", __func__);
		return DI_ERR_ERROR;
	}

	if((NULL != VK_strstr((char *)pcMediaPath, "http://")) || (NULL != VK_strstr((char *)pcMediaPath, "mms://"))
		|| (NULL != VK_strstr((char *)pcMediaPath, "https://")) || (NULL != VK_strstr((char *)pcMediaPath, "rtsp://")))
	{
		PrintData("[%s] Error -Not support Internet Protocol\n", __func__);
		return DI_ERR_ERROR;
	}

	nFileLength = VK_strlen(pcMediaPath);
	if((DI_MEDDIA_AVI_CMD_BUFSIZE-7) < nFileLength)
	{
		PrintData("[%s] Error -Over Max Length(Max Length=%d, nFileLength = %d)\n", __func__, (DI_MEDDIA_AVI_CMD_BUFSIZE-7), nFileLength);
		return DI_ERR_ERROR;
	}
	else
	{
		PrintData("[%s] Success -Max Length=%d, nFileLength = %d\n", __func__, (DI_MEDDIA_AVI_CMD_BUFSIZE-7), nFileLength);
	}

	cMediaPath_Buf = VK_strstr((char *)pcMediaPath, "file:/");
	if(cMediaPath_Buf != NULL)
	{
		cMediaPath_Buf = cMediaPath_Buf+6;
	}
	else
	{
		cMediaPath_Buf = (char *)pcMediaPath;
	}

	cmd[0] = 'a';
	cmd[1] = 'v';
	cmd[2] = 'i';
	cmd[3] = 'n';
	cmd[4] = 'f';
	cmd[5] = 'o';
	cmd[6] = ' ';

	for(i = 7, j = 0; j<VK_strlen(cMediaPath_Buf); i++, j++)
	{
		if((cMediaPath_Buf[j] != ' ') && (cMediaPath_Buf[j] != '(') && (cMediaPath_Buf[j] != ')') &&
			(cMediaPath_Buf[j] != ',') && (cMediaPath_Buf[j] != '"') && (cMediaPath_Buf[j] != '&') && (cMediaPath_Buf[j] != '!'))
		{
			cmd[i] = cMediaPath_Buf[j];
		}
		else
		{
			cmd[i] = '\\';
			cmd[i+1] = cMediaPath_Buf[j];
			i++;
		}
	}

	PrintData("Media Path = %s\n", cmd+7);

	nCmdLength = VK_strlen(cmd);
	nCmdLength = nCmdLength + 21;
	if(DI_MEDDIA_AVI_CMD_BUFSIZE < nCmdLength)
	{
		PrintError("[%s] Error -pcMediaPath is invalid\n", __func__);
		return DI_ERR_ERROR;
	}

	VK_snprintf((char *)command, nCmdLength, "%s> /tmp/.avicodecinfo", cmd);

	(void)VK_SYSTEM_Command((char *)command);

	fp=fopen("/tmp/.avicodecinfo", "rb");
	if(fp == NULL)
	{
		PrintError("[%s] Error -Fail popen\n", __func__);
		return DI_ERR_ERROR;
	}

	n = fread(buf,sizeof(char),DI_MEDDIA_AVI_CMD_BUFSIZE,fp);
	if(n == 0)
	{
		PrintError("[%s] Error -Fail fread\n", __func__);
		fclose(fp);
		return DI_ERR_ERROR;
	}

	buf[n] ='\0';

	cParsing_Buf = VK_strstr((char *)buf, "video:");
	if(NULL != cParsing_Buf)
	{
		/* Parsing Video Codec */
		if(NULL != VK_strstr((char *)cParsing_Buf, "XviD"))
		{
			PrintData("Video Codec : XviD\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_XVID;
		}

		if((NULL != VK_strstr((char *)cParsing_Buf, "DivX")) || (NULL != VK_strstr((char *)cParsing_Buf, "DivX3")) || (NULL != VK_strstr((char *)cParsing_Buf, "DivX5")))
		{
			PrintData("Video Codec : DivX\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_DIVX;
		}

		if(NULL != VK_strstr((char *)cParsing_Buf, "MPEG1"))
		{
			PrintData("Video Codec : MPEG1\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_MPEG1;
		}

		if(NULL != VK_strstr((char *)cParsing_Buf, "MPEG2"))
		{
			PrintData("Video Codec : MPEG2\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_MPEG2;
		}

		if((NULL != VK_strstr((char *)cParsing_Buf, "V_MPEG4")) || (NULL != VK_strstr((char *)cParsing_Buf, "MPEG4")))
		{
			PrintData("Video Codec : MPEG2\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_MPEG4Part2;
		}

		if(NULL != VK_strstr((char *)cParsing_Buf, "H261"))
		{
			PrintData("Video Codec : H261\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_H261;
		}

		if(NULL != VK_strstr((char *)cParsing_Buf, "H263"))
		{
			PrintData("Video Codec : H263\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_H263;
		}

		if(NULL != VK_strstr((char *)cParsing_Buf, "H264"))
		{
			PrintData("Video Codec : H264\n");

			tMediaInfo.video_codec = DI_MEDIA_VIDEOCODEC_H264;
		}

		/* Parsing Audio Codec */
		if(NULL != VK_strstr((char *)cParsing_Buf, "mp3"))
		{
			PrintData("Audio Codec : mp3\n");

			tMediaInfo.audio_codec= DI_MEDIA_AUDIOCODEC_MP3;
		}

		if((NULL != VK_strstr((char *)cParsing_Buf, "A_AAC")) || (NULL != VK_strstr((char *)cParsing_Buf, "AAC")))
		{
			PrintData("Audio Codec : mp3\n");

			tMediaInfo.audio_codec= DI_MEDIA_AUDIOCODEC_AAC;
		}

		/* Parsing Video Size */
		for(i = 7; i<VK_strlen(cParsing_Buf); i++)
		{
			if(cParsing_Buf[i] != ' ')
			{
				cTmpVideoSize[i-7] = cParsing_Buf[i];
			}
			else
			{
				break;
			}
		}

		if(NULL != VK_strstr((char *)cTmpVideoSize, "x"))
		{
			for(i = 0, j = 0; i<VK_strlen(cTmpVideoSize); i++)
			{
				if(cTmpVideoSize[i] != 'x')
				{
					if(j == 0)
					{
						cTmpVideoWidth[i] = cTmpVideoSize[i];
					}
					else
					{
						cTmpVideoHeight[i-j] = cTmpVideoSize[i];
					}
				}
				else
				{
					j = i+1;
				}
			}

			tMediaInfo.width = VK_atoi((char *)cTmpVideoWidth);
			tMediaInfo.height = VK_atoi((char *)cTmpVideoHeight);
		}
		else
		{
			tMediaInfo.width = 0;
			tMediaInfo.height = 0;
		}

		PrintData("cTmpVideoSize : %s\n", cTmpVideoSize);
#if 0	/* Test Code */
		PrintData("cVideoWidth : %s\n", cTmpVideoWidth);
		PrintData("cVideoHeight : %s\n", cTmpVideoHeight);
#endif
		PrintData("VideoWidth : %d\n", tMediaInfo.width);
		PrintData("VideoHeight : %d\n", tMediaInfo.height);

		cTemp_Buf = VK_strstr((char *)cParsing_Buf, "fps");
		if(NULL != cTemp_Buf)
		{
			/* Parsing Video Frame Rate */
			for(i = 0; i<=DI_MEDIA_FRAMERATE_LENGTH; i++)
			{
				if(cTemp_Buf[-i] == ' ')
				{
					break;
				}
			}

			if(i != DI_MEDIA_FRAMERATE_LENGTH)
			{
				i=i-1;

				for(j=0;j<i;j++)
				{
					tMediaInfo.Video_FrameRate[j] = cTemp_Buf[-(i-j)];
				}
			}

			PrintData("tMediaInfo.Video_FrameRate=%s\n", tMediaInfo.Video_FrameRate);
		}
	}

	VK_memcpy(ptMediaInfo, &tMediaInfo, sizeof(tMediaInfo));

	fclose(fp);

	PrintExit;

	return eDIError;
}


/**
 * @brief			XMLÀ» ÀÌ¿ëÇÏ¿© MSDRM License info¸¦ ¾ò¾î¿Â´Ù.
 *
 * @param[in]		pcMediaFileName		Media file Name
 * @param[out]	pLicenseStatus		License Status Info
 * @return		HERROR
 * @date
 * @author
 * @see
 */

DI_ERR_CODE DI_MEDIA_MP_GetDRMLicenseState(HINT8 *pcLicenseData, DI_MEDIA_DRM_LICENSE_TYPE_e *pLicenseStatus)
{
	PrintDebug("DI_MEDIA_MP_GetDRMLicenseState().\n");
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)

	DRM_RESULT		eDRMError = DI_MEDIA_DRMENGINE_RESULT_UNKNOWN_ERROR;
#if defined(CMD_DEBUG)
	DRM_CONST_STRING dstrContentFilename;
#endif

	if(pcLicenseData == NULL)
	{
		PrintError("DRM license File is not OK !!!! \n");
		return DI_ERR_ERROR;
	}
#if defined(CMD_DEBUG)
	dstrContentFilename.cchString = VK_strlen(pcLicenseData);
	dstrContentFilename.pwszString = (const DRM_WCHAR *)pcLicenseData;
//	VK_strcpy(dstrContentFilename.pwszString, pcMediaFileName);
	PrintDebug("Before P_MEDIA_DRM_ProcessResponse()\n");
	eDRMError = P_MEDIA_DRM_ProcessResponse(&dstrContentFilename);
#else
	PrintDebug("Before P_MEDIA_DRM_ProcessResponse()\n");
	eDRMError = P_MEDIA_DRM_ProcessResponse(pcLicenseData);
#endif
	switch(eDRMError)
	{
		case DRM_SUCCESS:
		case DRM_E_HDSSLOTEXIST:
			*pLicenseStatus = DI_MEDIA_DRMENGINE_RESULT_OK;
			break;
		case DRM_E_NOXMLCDATA:
			*pLicenseStatus = DI_MEDIA_DRMENGINE_RESULT_DATA_INVALID;
			break;
		case DRM_E_INVALIDLICENSE:
			*pLicenseStatus = DI_MEDIA_DRMENGINE_RESULT_LICENCE_INVALID;
			break;
		case DRM_E_LICENSEEXPIRED:
			*pLicenseStatus = DI_MEDIA_DRMENGINE_RESULT_LICENCE_EXPIRED;
			break;
		default:
			*pLicenseStatus = DI_MEDIA_DRMENGINE_RESULT_UNKNOWN_ERROR;
			break;
	}
#else
	UNUSED_PARAMETER(pcLicenseData);
	UNUSED_PARAMETER(pLicenseStatus);
#endif

	return DI_ERR_OK;
}


#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
static void GenerateXML_ClientInfo	(DRM_CONST_STRING *pdstrOutputFileName, DRM_CONST_STRING *pdstrDataStoreFile)
{
    DRM_MANAGER_CONTEXT    *poManagerContext = NULL;
    DRM_DWORD   cbProperty    = 0x8000;
    DRM_BYTE   *pbProperty    = NULL;
    DRM_DWORD   cchClientInfo = 0;
    DRM_CHAR   *pszClientInfo = NULL;
    DRM_DWORD   cbRead        = 0;
    OEM_FILEHDL hfileOut      = OEM_INVALID_HANDLE_VALUE;
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkMem(poManagerContext = OEM_malloc (SIZEOF (DRM_MANAGER_CONTEXT)));
    ZEROMEM(poManagerContext, SIZEOF (DRM_MANAGER_CONTEXT));

    ChkDR(DRM_MGR_Initialize(poManagerContext, pdstrDataStoreFile));

    ChkMem(pbProperty = (DRM_BYTE *) OEM_malloc(cbProperty * SIZEOF (DRM_BYTE)));
    ChkDR(DRM_MGR_GetDeviceProperty(poManagerContext,
									WM_DRM_CLIENTINFO,
									pbProperty,
									&cbProperty));

#if _UNICODEtoANSI_
	cchClientInfo = DRMCRT_wcslen((const DRM_WCHAR *)pbProperty );
	ChkMem(pszClientInfo = (DRM_CHAR *) OEM_malloc(cchClientInfo * SIZEOF (DRM_CHAR)));
	DRM_UTL_DemoteUNICODEtoANSI( (const DRM_WCHAR *)pbProperty, pszClientInfo, cchClientInfo );
#else
	pszClientInfo = (DRM_CHAR*)pbProperty;
	cchClientInfo = (DRM_DWORD)cbProperty;
#endif

    hfileOut = OEM_OpenFile(pdstrOutputFileName->pwszString,
                                  OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                                  OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                  OEM_CREATE_ALWAYS,
                                  OEM_ATTRIBUTE_NORMAL);

    if (hfileOut != OEM_INVALID_HANDLE_VALUE)
    {
        if (!OEM_WriteFile(hfileOut, pszClientInfo, cchClientInfo, &cbRead)
        || cbRead != cchClientInfo)
        {
            PrintError("Failed to write challenge\n");
            goto ErrorExit;
        }
    }
    else
    {
        PrintError("Challenge file not opened\n");
        goto ErrorExit;
    }

ErrorExit:

    if (hfileOut != OEM_INVALID_HANDLE_VALUE)
    {
        OEM_CloseFile(hfileOut);
    }

#if _UNICODEtoANSI_
    if (pszClientInfo != NULL)
{
        OEM_free(pszClientInfo);
    }
#endif

    if (pbProperty != NULL)
    {
        OEM_free(pbProperty);
    }

    if (poManagerContext != NULL)
	{
        DRM_MGR_Uninitialize(poManagerContext);
        OEM_free(poManagerContext);
    }
	}
#endif

DI_ERR_CODE DI_MEDIA_MP_GetDRMClientInfo(HINT8 *pcOutPutFileName)
{
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
	DRM_CONST_STRING  dstrDataStoreFile;
	DRM_CONST_STRING dstrContentFilename;
	int	i;

	if(pcOutPutFileName == NULL)
	{
		PrintError("DI_MEDIA_MP_GetDRMClientInfo() pcOutPutFileName  is not OK !!!! \n");
		return DI_ERR_ERROR;
	}

	dstrContentFilename.cchString = VK_strlen(pcOutPutFileName);
	dstrContentFilename.pwszString = (const DRM_WCHAR *)pcOutPutFileName;

	bdrm_env_datastore(&dstrDataStoreFile);

	GenerateXML_ClientInfo(&g_dstrMasterFile, &dstrDataStoreFile);
	for(i=0; i < (int)g_dstrMasterFile.cchString; i++)
		pcOutPutFileName[i] = (HUINT8)(g_dstrMasterFile.pwszString[i] & 0xFF);
#else
	UNUSED_PARAMETER(pcOutPutFileName);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_MP_GetDRMFileName(HINT8 *pcOutPutFileName)
{
#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
	DRM_CONST_STRING  dstrDataStoreFile;
	int	i;

	if(pcOutPutFileName == NULL)
	{
		PrintError("DI_MEDIA_MP_GetDRMClientInfo() pcOutPutFileName  is not OK !!!! \n");
		return DI_ERR_ERROR;
	}

	bdrm_env_datastore(&dstrDataStoreFile);

	for(i=0; i < (int)dstrDataStoreFile.cchString; i++)
		pcOutPutFileName[i] = (HUINT8)(dstrDataStoreFile.pwszString[i] & 0xFF);
	pcOutPutFileName[dstrDataStoreFile.cchString] = '\0';
#else
	UNUSED_PARAMETER(pcOutPutFileName);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_InstallDLNAFunction (MEDIA_DLNA_FileFunction_t stDLNAFileFn)
{
	PrintEnter;

	s_tMEDIADLNAFileFn.Open = stDLNAFileFn.Open;
	s_tMEDIADLNAFileFn.Close = stDLNAFileFn.Close;
	s_tMEDIADLNAFileFn.Read = stDLNAFileFn.Read;
	s_tMEDIADLNAFileFn.Seek = stDLNAFileFn.Seek;
	s_tMEDIADLNAFileFn.Tell = stDLNAFileFn.Tell;
	s_tMEDIADLNAFileFn.IsEOF = stDLNAFileFn.IsEOF;
	s_tMEDIADLNAFileFn.GetFileSize = stDLNAFileFn.GetFileSize;
	s_tMEDIADLNAFileFn.OpenForNexus = stDLNAFileFn.OpenForNexus;
	s_tMEDIADLNAFileFn.SeekForNexus = stDLNAFileFn.SeekForNexus;
	s_tMEDIADLNAFileFn.BoundsForNexus = stDLNAFileFn.BoundsForNexus;

	PrintExit;

	return DI_ERR_OK;
}

void DI_MEDIA_MP_SetPlayReadyCustomData(HINT8 *pcCustomData)
{
	PrintEnter;

	if(s_pcPlayReadyCustomData != NULL)
	{
		DD_MEM_Free(s_pcPlayReadyCustomData);
		s_pcPlayReadyCustomData = NULL;
	}

	s_pcPlayReadyCustomData = VK_StrDup(pcCustomData);

	PrintExit;

	return;
}
/**
 * @brief			License°úÁ¤Áß Ãß°¡ Á¤º¸°¡ ÇÊ¿äÇÑ °æ¿ì »ç¿ëµÉ Data (Ex> Maxdome DRM). Data´Â License°úÁ¤Áß Àü´ÞµÇ´Â urlµÚ¿¡ ºÙ°ÔµÊ.
 *
 * @param[in]		pcMediaFileName		Media file Name
 * @return			None
 * @date
 * @author
 * @see
 */
void DI_MEDIA_MP_SetLicenseData(HINT8 *pcLicenseData)
{
	PrintEnter;

	if(s_pcLisenceData != NULL)
	{
		DD_MEM_Free(s_pcLisenceData);
		s_pcLisenceData = NULL;
	}

	s_pcLisenceData = VK_StrDup(pcLicenseData);

	PrintExit;

	return;
}

void DI_MEDIA_LicenseDataRegisterCallback(DI_MEDIA_LicenseDataCallback_t LicenseDataCallback)
{
	PrintEnter;

	if(LicenseDataCallback == NULL)
	{
		PrintError("[%s] Error - LicenseDataCallback is null.\n", __func__);
		return;
	}

	s_tLicenseDataCallback = LicenseDataCallback;

	PrintExit;

	return;
}

void DI_MEDIA_LicenseDataUnregisterCallback(void)
{
	PrintEnter;

	s_tLicenseDataCallback = NULL;
	if(s_pcLisenceData != NULL)
	{
		DD_MEM_Free(s_pcLisenceData);
		s_pcLisenceData = NULL;
	}
	PrintExit;

	return;
}

#if defined (CONFIG_PLAYREADY)
void DI_MEDIA_RegisterOutputControlCallback(DI_MEDIA_PLAYREADY_OPL_CB callback)
{
	f_OPLcb = callback;
}

void DI_MEDIA_UnregisterOutputControlCallback(void)
{
	f_OPLcb = NULL;
}
#endif

#define PLAYREADY_DRMSYSTEMID "urn:dvb:casystemid:19212"
#define PLAYREADY_MSGTYPE "application/vnd.ms-playready.initiator+xml"
DI_ERR_CODE DI_MEDIA_sendDRMMessage(HUINT8* msgType, HUINT8* msg, HUINT8* DRMSystemID)
{
	HUINT32 uiUrlOffset, uiUrlLength;
	DI_ERR_CODE dResult;

/******************************
*	1. Ignore the DRMSystemID unless it's CSPG-CI+
******************************/
	PrintDebug("msgtype=%s, msg=%s, drmsystemid=%s\n", msgType, msg, DRMSystemID);

	if(DRMSystemID == NULL)
	{
		PrintError("DRMSystemID is NULL!\n");
		return DI_ERR_ERROR;
	}

/******************************
*	2. Check msgType
******************************/
	if(msgType == NULL)
	{
		PrintError("msgType is NULL!\n");
		return DI_ERR_ERROR;
	}

	if(VK_strcmp(msgType, PLAYREADY_MSGTYPE) != 0)
	{
		PrintError("msgType is not the PLAYREADY MIME type (%s)\n", msgType);
		return DI_ERR_OK;
	}

/******************************
*	3. parse the msg and get the License URL
******************************/
	if(msg == NULL)
	{
		PrintError("msg is NULL!\n");
		return DI_ERR_ERROR;
	}

	dResult = P_MEDIA_DRM_ParseWebInitiator(msg, &uiUrlOffset, &uiUrlLength);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_DRM_ParseWebInitiator failed!\n");
		return DI_ERR_ERROR;
	}

/******************************
*	4. keep the License URL
******************************/
	dResult = P_MEDIA_DRM_SetLicenseUrl(msg+uiUrlOffset, uiUrlLength);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_DRM_SetLicenseUrl failed!\n");
	}

#if defined (USE_PLAYREADY_DRM)
	if(s_tStreamInfo[s_ulMediaId].eDrmType == DI_MEDIA_DRM_TYPE_PLAYREADY)
	{
		DI_license_response response;

		VK_MEM_Memset(&response, 0, sizeof(DI_license_response));

		if(s_tStreamInfo[s_ulMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)
		{
			NEXUS_Playpump_SetPlayReadyAsfDecryptCb(s_tMediaInstance[s_ulMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																P_MEDIA_DoDecryptAESCtr);
		}
		else
		{
			NEXUS_Playpump_SetPlayReadySstrDecryptCb(s_tMediaInstance[s_ulMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX],
																NULL,
																P_MEDIA_DoDecryptSample);
		}

		/* Somtimes fail to get license at valid */
		{
			HINT32	i = 0;
			for (i=0;i<3;i++)
			{
				dResult = P_MEDIA_GetLicense(s_tStreamInfo[s_ulMediaId].pucWrmHeader, s_tStreamInfo[s_ulMediaId].nWrmHeaderLen, &response);
				if(dResult != DRV_OK)
				{
					PrintError("P_MEDIA_GetLicense failed retry count=%d\n", i);
				}
				else
				{
					break;
				}
			}
		}

		if(dResult != DRV_OK)
		{
			if ( NULL != s_tEventCallback[s_ulMediaId] )
			{
				s_tEventCallback[s_ulMediaId] ((HUINT32)s_ulMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
			}
			else
			{
				PrintError("s_tEventCallback is NULL\n");
			}
		}
	}
#endif

	return dResult;
}

DI_ERR_CODE DI_MEDIA_GetPumpBufferDepth(HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStatus tPlaybackStatus;

	if(ulSize == NULL)
	{
		PrintError("@@@@@ [%s:%d] ulSize is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	if(s_tMediaInstance[s_ulMediaId].playback != NULL)
	{
		nNexusError = NEXUS_Playback_GetStatus(s_tMediaInstance[s_ulMediaId].playback, &tPlaybackStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			*ulSize = tPlaybackStatus.fifoDepth;
		}
	}

exit:
	return nRet;
}

#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
DI_ERR_CODE DI_MEDIA_IsVideoDecoderPaused(HBOOL *bPaused)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_VIDEO_RUNNING_STATE videoRunningState = DI_VIDEO_RUNNING_STATE_DECODING;

	if(bPaused == NULL)
	{
		PrintError("@@@@@ [%s:%d] bPaused is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	nRet = DI_VIDEO_GetState(DI_VIDEO_DECODER_MAIN, &videoRunningState);
	if (nRet == DI_ERR_OK)
	{
		if (videoRunningState == DI_VIDEO_RUNNING_STATE_NODATA)
		{
			*bPaused = TRUE;
		}
	}

exit:
	return nRet;
}


DI_ERR_CODE DI_MEDIA_IsAudioDecoderPaused(HBOOL *bPaused)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_AUDIO_RUNNING_STATE audioRunningState = DI_AUDIO_STATE_DECODING;

	if(bPaused == NULL)
	{
		PrintError("@@@@@ [%s:%d] bPaused is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	nRet = DI_AUDIO_GetState(DI_AUDIO_DECODER_MAIN, &audioRunningState);
	if (nRet == DI_ERR_OK)
	{
		if (audioRunningState == DI_AUDIO_STATE_NODATA)
		{
			*bPaused = TRUE;
		}
	}

exit:
	return nRet;
}
#endif

DI_ERR_CODE DI_MEDIA_GetVideoBufferDepth(HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_VideoDecoderStatus tDecoderStatus;

	if(ulSize == NULL)
	{
		PrintError("@@@@@ [%s:%d] ulSize is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	if(s_tMediaInstance[s_ulMediaId].vDecoder != NULL)
	{
		nNexusError = NEXUS_VideoDecoder_GetStatus(s_tMediaInstance[s_ulMediaId].vDecoder, &tDecoderStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			*ulSize = tDecoderStatus.fifoDepth;			/* size in bytes of the compressed buffer */
		//	*ulSize += tDecoderStatus.queueDepth;		/* the number of decoded pictures currently ready to be displayed */
		//	*ulSize += tDecoderStatus.cabacBinDepth;	/* depth in bytes of the cabac bin buffer */
			/* *ulSize += tDecoderStatus.enhancementFifoDepth; */	/* ÀÌ ¹öÀü NEXUS¿¡´Â ¾ø´Â ¸â¹ö, depth in bytes of the enhancement compressed buffer */
		}
	}

exit:
	return nRet;
}

DI_ERR_CODE DI_MEDIA_GetAudioBufferDepth(HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_AudioDecoderStatus tDecoderStatus;

	if(ulSize == NULL)
	{
		PrintError("@@@@@ [%s:%d] ulSize is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	VK_memset(&tDecoderStatus, 0x00, sizeof(NEXUS_AudioDecoderStatus));

	if(s_tMediaInstance[s_ulMediaId].aDecoder != NULL)
	{
		nNexusError = NEXUS_AudioDecoder_GetStatus(s_tMediaInstance[s_ulMediaId].aDecoder, &tDecoderStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			*ulSize = tDecoderStatus.fifoDepth;			/* depth in bytes of the compressed buffer */
		}
	}

exit:
	return nRet;
}

DI_ERR_CODE DI_MEDIA_GetMaxBufferSize(HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_VideoDecoderStatus tvDecoderStatus;
	NEXUS_AudioDecoderStatus taDecoderStatus;

	if(ulSize == NULL)
	{
		PrintError("@@@@@ [%s:%d] ulSize is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	*ulSize = 0;

	VK_memset(&tvDecoderStatus, 0x00, sizeof(NEXUS_VideoDecoderStatus));

	if(s_tMediaInstance[s_ulMediaId].vDecoder != NULL)
	{
		nNexusError = NEXUS_VideoDecoder_GetStatus(s_tMediaInstance[s_ulMediaId].vDecoder, &tvDecoderStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			*ulSize += tvDecoderStatus.fifoSize;
		}
	}

	VK_memset(&taDecoderStatus, 0x00, sizeof(NEXUS_AudioDecoderStatus));

	if(s_tMediaInstance[s_ulMediaId].aDecoder != NULL)
	{
		nNexusError = NEXUS_AudioDecoder_GetStatus(s_tMediaInstance[s_ulMediaId].aDecoder, &taDecoderStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			*ulSize += taDecoderStatus.fifoSize;
		}
	}

exit:
	return nRet;
}

#if defined(CONFIG_MHEG_SUPPORT)
#if defined(CONFIG_ICS)

void		DI_MEDIA_SetPlayback_IcsMedia(void* pMediaInstance, void* pStreamInfo)
{
	if((pMediaInstance == NULL) || (pStreamInfo == NULL))
	{
		PrintError("[%s] Error - pMediaInstance of pStreamInfo is null.\n", __func__);
		return;
	}

	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return;
	}

	s_ulMediaId = 0;
	s_ulMediaStart++;

	/** Clear MediaInstance Info */
	VK_memset((void*)&s_tMediaInstance[s_ulMediaId], 0, sizeof(DI_MediaInstance));

	/** Clear Stream Info */
	VK_memset((void*)&s_tStreamInfo[s_ulMediaId], 0, sizeof(DI_MEDIA_INFO_t));

	/** Set MediaInstance Info */
	VK_memcpy((void*)&s_tMediaInstance[s_ulMediaId], pMediaInstance, sizeof(DI_MediaInstance));

	/** Set Stream Info */
	VK_memcpy((void*)&s_tStreamInfo[s_ulMediaId], pStreamInfo, sizeof(DI_MEDIA_INFO_t));

	(void)VK_SEM_Release(s_ulMediaSemId);

}

void		DI_MEDIA_RestorePlayback_IcsMedia(void* pMediaInstance, void* pStreamInfo)
{
	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return;
	}

	if(s_ulMediaStart != 0)
	{
		s_ulMediaStart--;

		/** Resotre MediaInstance Info for release*/
		VK_memcpy(pMediaInstance, (void*)&s_tMediaInstance[s_ulMediaId], sizeof(DI_MediaInstance));

		/** Resotre Stream Info for release*/
		VK_memcpy(pStreamInfo, (void*)&s_tStreamInfo[s_ulMediaId], sizeof(DI_MEDIA_INFO_t));
	}

	(void)VK_SEM_Release(s_ulMediaSemId);
}

void		DI_MEDIA_ClearPlayback_IcsMedia(void)
{
	if(VK_SEM_Get(s_ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return;
	}

	/** Clear MediaInstance Info */
	VK_memset((void*)&s_tMediaInstance[0], 0, sizeof(DI_MediaInstance));

	/** Clear Stream Info */
	VK_memset((void*)&s_tStreamInfo[0], 0, sizeof(DI_MEDIA_INFO_t));

	s_tStreamInfo[0].audiodecodertype = DI_MEDIA_AUDIO_DECODER_MAIN;
	s_tStreamInfo[0].videodecodertype = DI_MEDIA_VIDEO_DECODER_MAIN;

	(void)VK_SEM_Release(s_ulMediaSemId);
}

#endif
#endif

