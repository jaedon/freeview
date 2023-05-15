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
#include <arpa/inet.h>

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
#include "nexus_keyladder.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_media20.h"
#include "iptuner/di_iptuner20.h"
#include "iptuner/di_iptuner_pool.h"
#include "iptuner/di_iptuner_dnp.h"
#include "iptuner/transport_tls.h"
#include "di_video.h"
#include "di_audio.h"
#include "di_media_types.h"
#include "di_demux.h"
#include "di_uart.h"

#include "drv_media20.h"
#include "drv_demux.h"
#include "drv_media.h"
#include "drv_audio.h"
#include "drv_video.h"
#include "drv_pvr.h"
#include "drv_drm.h"
#include "drv_nvram.h"
#include "drv_flash.h"

#include "di_crypt.h"
#include "drv_crypt.h"
#include "crc.h"

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

#if defined (USE_DSC)
#include "di_dsc.h"
#endif

#include "mxlog.h"
#include "mxdtcp.h"
#include "DtcpAppLib.h"

#if defined (CONFIG_PLAYREADY)
#if defined (CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
#include "di_fs.h"
#endif
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define __USE_VFIO__

//#define SUPPORT_OLD_MEDIA

#if	defined(SUPPORT_DRM_PRE_DELIVERY_LICENSE_METHOD)
#define _UNICODEtoANSI_		(1)
#endif

//#define DI_MEDIA_DEBUG
//#define DI_MEDIA_CRITICAL_DEBUG
#define __SUPPORT_FIRSTPTS_DETECTION__ /* 문제 없을 시 Default로 정리 필요 */

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
#define UNUSED_PARAM(x)	(void)x


#define HUMAX_COOKIE_LINE_BUFFER_SIZE		(10240)
#if defined(USE_HDD_SMART)
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
	현재 Position을 얻기 위해 기존의 경우 bmedia 함수를 사용하였으나 다음 문제 존재
	1) ASF의 경우 일반 재싱시 bmedia_player_tell 함수에 의해 position 정보가 넘어오지 않음
	   AVI의 경우는 정상 동작, ASF의 경우는 Trick만 정상 동작
	2) 또한 실제 화면에 나오는 Video와 bmedia 와의 GAP이 존재하므로 실제 Position과 많이 다르게 됨
*/

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

#define DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN		(1024*188*2*10)
#define DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN		(256)
#define DI_MEDIA_BITRATE_SEARCH_READ_LEN		(1024*60)
#define DI_MEDIA_BITRATE_TEMP		(1024*1024*4)

/* TS Injector */
#define INVALID_32BITS                          (0xFFFFFFFF)
#define _DEF_MEDIA_MAX_INJ_NUM                  DI_MEDIA_MAX_NUM  /* Depend on demux:deviceID */
#define _DEF_MEDIA_INVALID_INJ_NUM              (0xFFFF)
#define _DEF_MEDIA_INJ_MAX_OVERFLOWCNT         (3)
#define _DEF_MEDIA_INJ_WAIT_EVTTIME            (1000) 		/* 300msec = 0.3sec */
#define _DEF_MEDIA_INJ_MAX_LOOPCNT             (1000)

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

typedef enum
{
	DI_MEDIA_SUCCESSIVE_SEEK_NONE,
	DI_MEDIA_SUCCESSIVE_SEEK_START,
	DI_MEDIA_SUCCESSIVE_SEEK_RUNNING,
	DI_MEDIA_SUCCESSIVE_SEEK_STOP,
	DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE
} DI_MEDIA_SUCCESSIVE_SEEK_MODE_e;

typedef struct _stMediaInfo_t{
	HUINT8				opened;
	char				*pszStreamUrl;

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
	DI_MEDIA_AUDIO_FREE=0,
	DI_MEDIA_AUDIO_ALLOCATED,
	DI_MEDIA_AUDIO_PAUSED,
	DI_MEDIA_AUDIO_STOP,
	DI_MEDIA_AUDIO_PLAY
}	DI_MEDIA_AUDIO_STATUS;

typedef struct
{
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
	NEXUS_PidChannelHandle		audioTrackPidChannels[DI_MEDIA_MAX_AUDIO_TRACK];
	NEXUS_PidChannelHandle		videoPidChannel;
	// NEXUS_PidChannelHandle		videoTrackPidChannels[DI_MEDIA_DIVX_MAX_VIDEO_TRACK];
	NEXUS_PidChannelHandle		sectionPidChannel;
	NEXUS_PidChannelHandle      pcrPidChannel; /* pcr_pid */

	NEXUS_FilePlayHandle 		file;
	SESSION_HANDLE				pHSession;
	DI_MEDIA_PlaySpeed_e		trickrate;

	/* this field support first pts value which based nexus playback (b_play_getpts).
	 * if you getting more exact first pts value
	 */
	HUINT32						unFirstPts;
	HBOOL                       bPtsInstalled;
	HUINT64						ullPrevValidPosition;

	HBOOL						bEndOfStreamState; /* Currently, unsed flag */
	HBOOL						bSessionStopping;  /* For preventing remaining trick(setSpeed) action in msg_que while session stopping */
	NEXUS_VideoDecoderHandle 	vDecoder;	/* just for reference for getting decoder status */
	NEXUS_AudioDecoderHandle 	aDecoder;	/* just for reference for getting decoder status */
#if 1 /* workaround */
	HUINT32						unTmpSeekTime;
#endif
	HBOOL						bSuccesiveSeeking;
	HUINT32						unSuccesiveSeekTime;

	HULONG						ulMediaSemId;
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

/* Multicast TS Injector */
typedef	struct	stDI_MediaTSInjector
{
	/*
	 * This instance is made by purpose of playpump based TS injector
	 *      like multicast streaming(similar to above DI_MediaInstance).
	 * Currently, only for tuning control w/o playback control.
	 *      --> xxx
	 *      --> xxx DI_MEDIA_INFO_t
	 * So, this must be made after openSession(), not setupSession()
	 *     and don't have the mediaID.
	 *
	 * w/o handles compared with DI_MediaInstance :
	 *     + NEXUS_PlaybackHandle
	 *     + NEXUS_FilePlayHandle
	 *     + NEXUS_VideoDecoderHandle
	 *     + NEXUS_AudioDecoderHandle
	 *     + NEXUS_PidChannelHandle
	 *     + bEndOfStream
	 */
	unsigned long                    uSemID;
	HUINT8                           bRunning;
	NEXUS_PlaypumpHandle             tPlaypump;
	SESSION_HANDLE                   *pHSession;
	BKNI_EventHandle                 tEvent;
#if 0
	URL_t                            tUrl;
#endif
	HBOOL                            bRestorePumpSettings;
	NEXUS_TransportType              currTransportType;
	NEXUS_TransportType              prevTransportType;
	NEXUS_PlaypumpMode               currPlaypumpMode;
	NEXUS_PlaypumpMode               prevPlaypumpMode;

	/*P_PID_CHANNEL_INFO             pidChn;*/
	NEXUS_PidChannelHandle           hVideoPidChn;
	NEXUS_PidChannelHandle           hAudioMainPidChn;
	NEXUS_PidChannelHandle           hAudioSubPidChn;
	unsigned short                   uVideoPid;
	unsigned short                   uAudioMainPid;
	unsigned short                   uAudioSubPid;
	unsigned short                   uPcrPid;
	NEXUS_PlaypumpOpenPidChannelSettings openPidSettings;

	struct
	{
		NEXUS_VideoDecoderHandle decoder;
		NEXUS_VideoCodec codec;
		bool index;
	} videoSet;

	struct
	{
		NEXUS_AudioDecoderHandle primary;
		NEXUS_AudioDecoderHandle secondary;
	} audioSet;

	DI_IPTUNER_TSInjectorCallback_t dataCallbacks;

	// DRM related...
} DI_MediaTSInjector;

typedef	struct	P_MEDIA_TRICK_TABLE_s
{
	int	rate;
} P_MEDIA_TRICK_TABLE_t;

struct	P_MEDIA_DLNA_Flags_t
{
	unsigned int	Flags_Bit0	:1 ;
	unsigned int	Flags_Bit1	:1 ;
	unsigned int	Flags_Bit2	:1 ;
	unsigned int	Flags_Bit3	:1 ;
	unsigned int	Flags_Bit4	:1 ;
	unsigned int	Flags_Bit5	:1 ;
	unsigned int	Flags_Bit6	:1 ;
	unsigned int	Flags_Bit7	:1 ;
	unsigned int	Flags_Bit8	:1 ;
	unsigned int	Flags_Bit9	:1 ;
	unsigned int	Flags_Bit10	:1 ;
	unsigned int	Flags_Bit11	:1 ;
	unsigned int	Flags_Bit12	:1 ;
	unsigned int	Flags_Bit13	:1 ;
	unsigned int	Flags_Bit14	:1 ;
	unsigned int	Flags_Bit15	:1 ;
	unsigned int	Flags_Bit16	:1 ;
	unsigned int	Flags_Bit17	:1 ;
	unsigned int	Flags_Bit18	:1 ;
	unsigned int	Flags_Bit19	:1 ;
	unsigned int	Flags_Bit20	:1 ;
	unsigned int	Flags_Bit21	:1 ;
	unsigned int	Flags_Bit22	:1 ;
	unsigned int	Flags_Bit23	:1 ;
	unsigned int	Flags_Bit24	:1 ;
	unsigned int	Flags_Bit25	:1 ;
	unsigned int	Flags_Bit26	:1 ;
	unsigned int	Flags_Bit27	:1 ;
	unsigned int	Flags_Bit28	:1 ;
	unsigned int	Flags_Bit29	:1 ;
	unsigned int	Flags_Bit30	:1 ;
	unsigned int	Flags_Bit31	:1 ;
} ;

typedef struct P_MEDIA_DLNA_Flags_t* DI_MEDIA_DLNA_FlagBits;

static const P_MEDIA_TRICK_TABLE_t g_sTrickTable[2*eDI_MEDIA_Speed_FF_X128 + 1] =
{
	{-128000},	//eDI_MEDIA_Speed_RW_X128, -128x
	{-100000},	//eDI_MEDIA_Speed_RW_X100, -100x
	{-64000},	//eDI_MEDIA_Speed_RW_X64, -64x
	{-32000},  	//eDI_MEDIA_Speed_RW_X32, -32x
	{-30000},	//eDI_MEDIA_Speed_RW_X30, -30x
	{-16000},  	//eDI_MEDIA_Speed_RW_X16, -16x
	{-10000},	//eDI_MEDIA_Speed_RW_X10, -10x
	{-8000},  	//eDI_MEDIA_Speed_RW_X8, -8x
	{-4000},  	//eDI_MEDIA_Speed_RW_X4, -4x
	{-2000},  	//eDI_MEDIA_Speed_RW_X2, -2x
	{-1200},	//eDI_MEDIA_Speed_RW_X12_10, -12/10x
	{-1000},  	//eDI_MEDIA_Speed_RW_X1, -1x
	{-500},  	//eDI_MEDIA_Speed_RW_X_1_2, -1/2x
	{-250},  	//eDI_MEDIA_Speed_RW_X_1_4, -1/4x
	{-120},  	//eDI_MEDIA_Speed_RW_X_1_8, -1/8x
	{-100},  	//eDI_MEDIA_Speed_RW_X_1_10, -1/10x
	{0},  		//eDI_MEDIA_Speed_Pause
#ifdef CONFIG_DLNA_CERTIFICATION
 	{10},		//eDI_MEDIA_Speed_FF_X_1_10, 0.1x
	{50},		//eDI_MEDIA_Speed_FF_X_1_2, 0.2x
	{100},		//eDI_MEDIA_Speed_FF_X_1_4, 0.5x
	{200},		//eDI_MEDIA_Speed_FF_X_1_8, 0,8x
#else
	{100},		//eDI_MEDIA_Speed_FF_X_1_10, 1/10x
	{120},  	//eDI_MEDIA_Speed_FF_X_1_8, 1/8x
	{250},		//eDI_MEDIA_Speed_FF_X_1_4, 1/4x
	{500}, 		//eDI_MEDIA_Speed_FF_X_1_2, 1/2x
#endif
	{1000}, 	//eDI_MEDIA_Speed_Normal, 1x
	{1200},		//eDI_MEDIA_Speed_FF_X12_10, 12/10x
	{2000}, 	//eDI_MEDIA_Speed_FF_X2, 2x
	{4000}, 	//eDI_MEDIA_Speed_FF_X4, 4x
	{8000}, 	//eDI_MEDIA_Speed_FF_X8, 8x
	{10000}, 	//eDI_MEDIA_Speed_FF_X10, 10x
	{16000}, 	//eDI_MEDIA_Speed_FF_X16, 16x
	{30000},	//eDI_MEDIA_Speed_FF_X30, 30x
	{32000}, 	//eDI_MEDIA_Speed_FF_X32, 32x
	{64000},	//eDI_MEDIA_Speed_FF_X64, 64x
	{100000},	//eDI_MEDIA_Speed_FF_X100, 100x
	{128000} 	//eDI_MEDIA_Speed_FF_X128, reserved
};

typedef struct
{
	unsigned long		ulCmmand;
	char				*pHandle;
	unsigned long	   	ulEvent;
	unsigned char		*pucParam;
	unsigned long		ulParam2;
	unsigned long		ulParam3;
	unsigned long		ulParam4;
	unsigned long		ulParam5;
	unsigned long		ulParam6;
}DI_MEDIA_MSG;

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

/** Static Play Time Task Id */
static unsigned long 			s_ulMediaPlayTimeTaskId = 0;

/** Static Stream Info */
static DI_MEDIA_INFO_t			s_tStreamInfo[DI_MEDIA_MAX_NUM];
/** Static Event Callback */
static DI_MEDIA_EventCallback_t	s_tEventCallback[DI_MEDIA_MAX_NUM];
static DI_MEDIA_FirstPtsCallback_t	s_tEventFirstPtsCallback[DI_MEDIA_MAX_NUM];

/** Static Media StreamInfo */
//static stMediaInfo       		s_MfpInfo[DI_MEDIA_MAX_NUM];
/** Static Temp */
static const uint8_t      		s_eos_data[256] = {0xE0, 0x55};
/** Static Media Instance */
static DI_MediaInstance		s_tMediaInstance[DI_MEDIA_MAX_NUM];

/** Multicast Media Injector (Another Media Instance w/o mediaID) */
static DI_MediaTSInjector               s_tMediaTSInjector[_DEF_MEDIA_MAX_INJ_NUM];
static HBOOL                            s_bInitTSInjector = FALSE;

/* EndTime for playing position */
static HUINT32				s_ulMediaEndTime[DI_MEDIA_MAX_NUM];

/** Static Stream Info */
static HUINT32					s_nCurrentPlayTime[DI_MEDIA_MAX_NUM];

/** Static Stream Info */
static DI_MEDIA_TIMERMODE_e			s_bPlayTimerMode[DI_MEDIA_MAX_NUM];

/** Static Dtcp State */
static HBOOL				s_bDtcpEnable = FALSE;

static unsigned long		s_ulMediaSuccessiveSeekTId = 0;
static unsigned long		s_ulMediaSuccessiveSeekMId = 0;

static int 					g_NeedPRLicensURL = FALSE;
static HUINT32 s_ulMediaStart = 0;

static HINT32 	s_nSuccessiveSeekState = 0;

/* End static variable */

#ifdef __SUPPORT_FIRSTPTS_DETECTION__ /* Disable */
static bmpeg2ts_parser_action
P_MEDIA_GettingTSPayload (bmpeg2ts_parser_pid *pid, unsigned flags, batom_accum_t src, batom_cursor *cursor, size_t len);

static void P_MEDIA_GettingPESPacket(void *packet_cnxt, batom_accum_t src, batom_cursor *payload, size_t len, const bmpeg2pes_atom_info *info);
#endif

static DI_ERR_CODE P_MEDIA_ProbeOpen (DI_IPTUNER_t *pHTuner, HINT8 *pszStreamUrl, stMediaInfo *pMfpInfo);
static DI_ERR_CODE		P_MEDIA_ProbeClose (stMediaInfo *pMfpInfo);
static void				P_MEDIA_ProbeFreeAtom (batom_t atom, void *user);
static void				P_MEDIA_ProbeAsync (void *cntx, bmedia_player_entry *entry);

static void P_MEDIA_TSInjectorInit(void);
static DI_ERR_CODE P_MEDIA_TSInjectorGetID(HUINT32 *punTSInjectorID) ;
static void P_MEDIA_TSInjectorDataEventCallback(void *context, int param);
static DI_ERR_CODE P_MEDIA_TSInjectorOpen(HUINT32 uTSInjID, HINT8 *pURL);
static DI_ERR_CODE P_MEDIA_TSInjectorStart(HUINT32 uTSInjID);
static DI_ERR_CODE P_MEDIA_TSInjectorStop(HUINT32 uTSInjID);
static DI_ERR_CODE P_MEDIA_TSInjectorClose(HUINT32 uTSInjID);
static void P_MEDIA_TSInjectorDataReceivedCallback (HUINT32 uInjID, HUINT8 *pBuffer, HUINT32 uBufLen); /* (DI_IPTUNER_t *pHTuner) */

/* b_play_getpts() is defined at nexus_playback_impl.h() with private internal function */
extern NEXUS_Error b_play_getpts(NEXUS_PlaybackHandle p, uint32_t *pts);

static const batom_user P_MEDIA_Probe_AtomUser =
{
	P_MEDIA_ProbeFreeAtom,
	sizeof(void **)
};


#if __________p__________
#endif

#define __________P_NEW_DI_MEDIA__________
/**
 @brief			Source Data를 Key를 이용하여 AES Algorithm으로 Encrypt한다.
 @param[in]		pMainKey		Encrypt에 사용되는 Main Key이다.
 @param[in]		pSharedKey		Initial Vector Key이다.
 @param[in]		ulKeySize		Main 및 Shared Key의 Size이다.
 @param[in]		pSrcBuffer		Source Data이다.
 @param[in]		ulBuffSize		Encryption하려는 Data의 Size이다.
 @param[out]	pDstBuffer		Encryption되는 Data이다.
 @return		HERROR
 @date 			2010/08/30
 @author 		kimdh1@humaxdigital.com
 @see
 */
int P_MEDIA_AES_Encrypt(unsigned char *pMainKey, unsigned char *pSharedKey, unsigned int ulKeySize, unsigned char *pSrcBuffer, unsigned char *pDstBuffer, unsigned int ulBuffSize)
{
	HUINT32 nResult = ERR_OK;
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	stCrypt_t tCrytInfo;

//	PrintEnter;

//	PrintDebug ("### P_MEDIA_AES_Encrypt ###\n");

	VK_MEM_Memset(&tCrytInfo, 0x0, sizeof(tCrytInfo));
#if 0
	tCrytInfo.etCryptMode = DI_CRYPT_AES;
	tCrytInfo.etCryptKey = DI_CRYPT_USER_KEY;
	tCrytInfo.etCryptData = DI_CRYPT_BLOCK;
	tCrytInfo.pKey = (HUINT8 *)pMainKey;
	tCrytInfo.ulKeySize = (HUINT32)ulKeySize;
	tCrytInfo.ulBuffSize = (HUINT32)ulBuffSize;
	tCrytInfo.pSrcBuffer = (HUINT8 *)pSrcBuffer;
	tCrytInfo.pDstBuffer = (HUINT8 *)pDstBuffer;
	tCrytInfo.pIvKey = (HUINT8 *)pSharedKey;

	eDiRet = DI_CRYPT_Encrypt(&tCrytInfo);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError(("[%s] Error - Fail DI_FLASH_GetField\n",__func__));
		nResult = ERR_FAIL;
	}
	/* else - do nothing */
#else
	eDiRet = DRV_CRYPT_EndecryptMedia(DI_CRYPT_AES, (HUINT8 *)pSrcBuffer, (HUINT32)ulBuffSize, (HUINT8 *)pDstBuffer, (HUINT8 *)pMainKey, (HUINT32)ulKeySize, (HUINT8 *)pSharedKey, (HUINT32)ulKeySize,TRUE);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError(("[%s] Error - Fail DI_FLASH_GetField\n",__func__));
		nResult = ERR_FAIL;
	}
	/* else - do nothing */
#endif

//	PrintExit;

	return nResult;
}

/**
 @brief			Source Data를 Key를 이용하여 AES Algorithm으로 Decrypt한다.
 @param[in]		pMainKey		Decrypt에 사용되는 Main Key이다.
 @param[in]		pSharedKey		Initial Vector Key이다.
 @param[in]		ulKeySize		Main 및 Shared Key의 Size이다.
 @param[in]		pSrcBuffer		Source Data이다.
 @param[in]		ulBuffSize		Decryption하려는 Data의 Size이다.
 @param[out]	pDstBuffer		Decryption되는 Data이다.
 @return		HERROR
 @date 			2010/08/30
 @author 		kimdh1@humaxdigital.com
 @see
 */
int P_MEDIA_AES_Decrypt(unsigned char *pMainKey, unsigned char *pSharedKey, unsigned int ulKeySize, unsigned char *pSrcBuffer, unsigned char *pDstBuffer, unsigned int ulBuffSize)
{
	HUINT32 nResult = ERR_OK;
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	stCrypt_t tCrytInfo;

//	PrintEnter;

//	PrintDebug ("### P_MEDIA_AES_Decrypt ###\n");

	VK_MEM_Memset(&tCrytInfo, 0x0, sizeof(tCrytInfo));
#if 0
	tCrytInfo.etCryptMode = DI_CRYPT_AES;
	tCrytInfo.etCryptKey = DI_CRYPT_USER_KEY;
	tCrytInfo.etCryptData = DI_CRYPT_BLOCK;
	tCrytInfo.pKey = (HUINT8 *)pMainKey;
	tCrytInfo.ulKeySize = (HUINT32)ulKeySize;
	tCrytInfo.ulBuffSize = (HUINT32)ulBuffSize;
	tCrytInfo.pSrcBuffer = (HUINT8 *)pSrcBuffer;
	tCrytInfo.pDstBuffer = (HUINT8 *)pDstBuffer;
	tCrytInfo.pIvKey = (HUINT8 *)pSharedKey;

	eDiRet = DI_CRYPT_Decrypt(&tCrytInfo);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError(("[%s] Error - Fail DI_CRYPT_Decrypt\n",__func__));
		nResult = ERR_FAIL;
	}
	/* else - do nothing */
#else
	eDiRet = DRV_CRYPT_EndecryptMedia(DI_CRYPT_AES, (HUINT8 *)pSrcBuffer, (HUINT32)ulBuffSize, (HUINT8 *)pDstBuffer, (HUINT8 *)pMainKey, (HUINT32)ulKeySize, (HUINT8 *)pSharedKey, (HUINT32)ulKeySize,FALSE);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError(("[%s] Error - Fail DI_FLASH_GetField\n",__func__));
		nResult = ERR_FAIL;
	}
	/* else - do nothing */
#endif

//	PrintExit;

	return nResult;
}

/**
 @brief			Source Data를 Key를 이용하여 AES Algorithm으로 Encrypt한다.
 @param[in]		pMainKey		Encrypt에 사용되는 Main Key이다.
 @param[in]		pSharedKey		Initial Vector Key이다.
 @param[in]		ulKeySize		Main 및 Shared Key의 Size이다.
 @param[in]		pSrcBuffer		Source Data이다.
 @param[in]		ulBuffSize		Encryption하려는 Data의 Size이다.
 @param[out]	pDstBuffer		Encryption되는 Data이다.
 @return		HERROR
 @date 			2010/08/30
 @author 		kimdh1@humaxdigital.com
 @see
 */
void *P_MEDIA_CRYPT_MemAlloc(HUINT32 nMemSize)
{
	NEXUS_Error	nexusError;
	void		*pSrcBuf;

	nexusError = NEXUS_Memory_Allocate(nMemSize, NULL, (void **)&pSrcBuf);
	if ((pSrcBuf == NULL) || (nexusError != NEXUS_SUCCESS))
	{
		PrintError("[P_MEDIA_CRYPT_MemAlloc] error BMEM_AllocAligned for pSrcBuf NULL\n");

		return NULL;
	}

	return pSrcBuf;
}

/**
 @brief			Source Data를 Key를 이용하여 AES Algorithm으로 Decrypt한다.
 @param[in]		pMainKey		Decrypt에 사용되는 Main Key이다.
 @param[in]		pSharedKey		Initial Vector Key이다.
 @param[in]		ulKeySize		Main 및 Shared Key의 Size이다.
 @param[in]		pSrcBuffer		Source Data이다.
 @param[in]		ulBuffSize		Decryption하려는 Data의 Size이다.
 @param[out]	pDstBuffer		Decryption되는 Data이다.
 @return		HERROR
 @date 			2010/08/30
 @author 		kimdh1@humaxdigital.com
 @see
 */
void P_MEDIA_CRYPT_MemFree(void *pMemory)
{
	if (pMemory == NULL)
	{
		PrintError("[P_MEDIA_CRYPT_MemFree] NULL pointer\n");

		return;
	}

	NEXUS_Memory_Free(pMemory);
}

/**
 * @brief		현재 사용되지 않는 Media ID를 얻는다.
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
 * @brief		Media Id가 유효한지 검사한다.
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

static DI_ERR_CODE P_MEDIA_FindMediaId(HUINT32 unSessionId,HUINT32 *punMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	HUINT32			i=0;
/*
	PrintEnter;
*/
	for(i = 0;i<DI_MEDIA_MAX_NUM;i++)
	{
		if((HUINT32)s_tMediaInstance[i].pHSession == unSessionId)
		{
			*punMediaId = i;
			break;
		}
	}

	if(i == DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] Media ID is full.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
/*
	PrintExit;
*/

	return eDIError;
}

static DI_ERR_CODE P_MEDIA_ReleaseMediaInstance(HUINT32 unMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	HUINT32			i=0;
/*
	PrintEnter;
*/
	if(unMediaId >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] Invalid Media ID.\n", __func__);
		return DI_ERR_ERROR;
	}
	if(s_tMediaInstance[unMediaId].valid == FALSE)
	{
		PrintError("[%s] Warning> Media %u is not valid.\n", __func__,unMediaId);
	}
	s_tMediaInstance[unMediaId].valid = FALSE;
	//VK_MEM_Memset(s_tMediaInstance[unMediaId].mediaurl, 0x0, DI_MEDIA_STREAM_URL_LEN);
	if(s_tMediaInstance[unMediaId].mediaurl != NULL)
	{
		DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
		s_tMediaInstance[unMediaId].mediaurl = NULL;
	}
	s_tMediaInstance[unMediaId].mediastate=DI_MEDIA_STATE_NO_SETUP;
	s_tMediaInstance[unMediaId].mediaType=DI_MEDIA_PLAYPUMP_DIVX;
	s_tMediaInstance[unMediaId].audioStatus=DI_MEDIA_AUDIO_FREE;
	s_tMediaInstance[unMediaId].audioPlayback=NULL;
	for(i = 0 ; i < DI_MEDIA_PLAYPUMP_MAX; i++)
	{
		s_tMediaInstance[unMediaId].playpump[i]=NULL;

	}
	s_tMediaInstance[unMediaId].playback=NULL;
	s_tMediaInstance[unMediaId].audioPidChannel=NULL;
	s_tMediaInstance[unMediaId].audioSubPidChannel=NULL;
	for(i = 0 ; i < DI_MEDIA_MAX_AUDIO_TRACK; i++)
	{
		s_tMediaInstance[unMediaId].audioTrackPidChannels[i]=NULL;
	}
	s_tMediaInstance[unMediaId].videoPidChannel=NULL;
	s_tMediaInstance[unMediaId].sectionPidChannel=NULL;
	s_tMediaInstance[unMediaId].pcrPidChannel=NULL;
	s_tMediaInstance[unMediaId].file=NULL;
	s_tMediaInstance[unMediaId].pHSession=NULL;
	s_tMediaInstance[unMediaId].trickrate=eDI_MEDIA_Speed_Pause;

	s_tMediaInstance[unMediaId].unFirstPts=0;
	s_tMediaInstance[unMediaId].bPtsInstalled=FALSE;
	s_tMediaInstance[unMediaId].bEndOfStreamState=FALSE;
	s_tMediaInstance[unMediaId].bSessionStopping = FALSE;
	s_tMediaInstance[unMediaId].vDecoder = NULL;
	s_tMediaInstance[unMediaId].aDecoder = NULL;
	s_tMediaInstance[unMediaId].unTmpSeekTime=0;
	s_tMediaInstance[unMediaId].bSuccesiveSeeking=FALSE;
	s_tMediaInstance[unMediaId].unSuccesiveSeekTime=0;
	s_tMediaInstance[unMediaId].ullPrevValidPosition=0;
/*
	PrintExit;
*/
	return eDIError;
}



/**
 * @brief		Media ID에 대한 Playback을 생성한다.
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
 * @brief		Media ID에 대한 Playback을 삭제한다.
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
		s_tMediaInstance[unMediaId].playback = NULL;
	}

	PrintExit;

	return eDIError;
}

/**
 * @brief		Playback Buffer의 정보를 반환한다.
 *
 * @param[in]	unMediaId				Media Device Id
 * @param[out]	pnCurrentUsedBufferSize	Playback Current Buffer
 * @param[out]	pnMaxBufferSize			Playback Max Buffer
 * @return		DI Error Code
 * @see
 */
static DI_ERR_CODE P_MEDIA_GetPlaybackBuffer(HUINT32 unMediaId, HUINT32 *pnCurrentUsedBufferSize,HUINT32 *pnMaxBufferSize)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	NEXUS_PlaybackStatus tPlaybackStatus;

	VK_MEM_Memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));

	PrintEnter;

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
	*pnCurrentUsedBufferSize = tPlaybackStatus.fifoDepth;
	*pnMaxBufferSize = tPlaybackStatus.fifoSize;

	PrintExit;

	return eDIError;
}

static DI_ERR_CODE P_MEDIA_GetAVStatus (HUINT32 unMediaId, HUINT32 *pBytes)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	NEXUS_VideoDecoderStatus vDecoderStatus;
	NEXUS_AudioDecoderStatus aDecoderStatus;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return eDIError;
	}

	if(s_tMediaInstance[unMediaId].vDecoder)
	{
		tNexusError = NEXUS_VideoDecoder_GetStatus (s_tMediaInstance[unMediaId].vDecoder,&vDecoderStatus);
		if(tNexusError == NEXUS_SUCCESS)
		{
			*pBytes = vDecoderStatus.fifoDepth;				/* size in bytes of the compressed buffer */
			*pBytes += vDecoderStatus.queueDepth;			/* the number of decoded pictures currently ready to be displayed */
			*pBytes += vDecoderStatus.cabacBinDepth;		/* depth in bytes of the cabac bin buffer */
			*pBytes += vDecoderStatus.enhancementFifoDepth;	/* depth in bytes of the enhancement compressed buffer */
			*pBytes += vDecoderStatus.queueDepth;
		}
	}


	if (s_tMediaInstance[unMediaId].aDecoder)
	{
		tNexusError = NEXUS_AudioDecoder_GetStatus (s_tMediaInstance[unMediaId].aDecoder,&aDecoderStatus);
		if(tNexusError == NEXUS_SUCCESS)
		{
			*pBytes += aDecoderStatus.fifoDepth;
			PrintDebug ("%s(%d) : audio fifoDepth (%u) bytes \n",__FUNCTION__,__LINE__,vDecoderStatus.fifoDepth);
		}
	}

	/* always return success except for invalid mediaId. */
	return DI_ERR_OK;
}

/**
 * @brief		millisecond 단위로 지정되는 position으로 재생 위치를 변경한다.
 *
 * @param[in]	unMediaId			Di Media ID
 * @param[in]	ullTime 			이동하고자 하는 위치를 millisecond 단위로 지정한다.
 * @return		DI Error Code
 * @see			DI_MEDIA_GetPosition
 */
static DI_ERR_CODE P_MEDIA_SetPosition(HUINT32 unMediaId, HUINT32 ulTime)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	HUINT32 ulSetTime = 0;

	PrintEnter;

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return eDIError;
	}

	if(s_tStreamInfo[unMediaId].support_seek != TRUE)
	{
		PrintError("[%s] Error - Not Support Seek in The Media Stream.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		ulSetTime = ulTime;

		PrintDebug("[%s] Info > Try to seek (%u) msec \n",__FUNCTION__, ulSetTime );

		tNexusError = NEXUS_Playback_Seek (s_tMediaInstance[unMediaId].playback, (NEXUS_PlaybackPosition)ulSetTime);
		if(NEXUS_SUCCESS != tNexusError)
		{
			PrintError("[%s] Error - Fail NEXUS_Playback_Seek:%d\n", __func__, tNexusError);
			eDIError = DI_ERR_ERROR;
		}
		else
		{
#if 1 /* workaround */
			s_tMediaInstance[unMediaId].unTmpSeekTime = ulSetTime;
#endif
			/**************************************************************************************************
			 * YouView Specific parameters : junsy 2011.11.02
			 *
			 * Stream 은 두가지 종류로 구분된다.
			 * Start PTS 값이 Zero base 로 찍힌 Stream 과 Non-Zero base PTS 로 찍힌 Stream 으로 구분할 수 있다.
			 *
			 * 이값을 Seek 또는 PlayWithSeek Operation 에서 보정해주어야만 한다.
			 * 아래 Parameter들은 Seek Operation 이후 첫 PTS 값과 요구된 Seek Time 을 설정하는 용도로 사용한다.
			 *
			 * 이 값은 지금 설정된 이후 P_MEDIA_PlayTimeTask 에서 현재시간을 올려줄때 보정 값으로 사용한다.
			 **************************************************************************************************/
#if 0 /* DRAGON TEST for pts revising.*/
			s_tMediaInstance[unMediaId].unMediaStartTime=ulSetTime;
			s_tMediaInstance[unMediaId].unFirstPts = 0;
#endif
		}
	}

	PrintExit;

	return eDIError;
}



/**
 * @brief		Callback함수로 등록시 Stream 재생이 시작되면서 호출된다.
 *
 * @param[in]	context 	변경하고자 하는 배속 정보를 지정한다.
 * @param[in]	param 		변경하고자 하는 배속 정보를 지정한다.
 * @return		void
 * @see			DI_MEDIA_RegisterCallback
 */
static void P_MEDIA_BeginOfStreamCallback(void *context, int param)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_MEDIA_MSG	audMsg;
	int				nVkResult = VK_OK;
	HUINT32			unLoopCount = 0;

	PrintEnter;

	BSTD_UNUSED(context);

	s_nSuccessiveSeekState = 0;

	if(s_tMediaInstance[param].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if(s_tMediaInstance[param].bSuccesiveSeeking == TRUE)
		{
			PrintData("### [%s] Send DI_MEDIA_SUCCESSIVE_SEEK_STOP Message ###\n", __FUNCTION__);
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
			}
			else
			{
				audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
				audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
				nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
				if(nVkResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
				}
				else
				{
					for(unLoopCount = 0; unLoopCount < 10; unLoopCount++)
					{
						if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
						{
							break;
						}

						VK_TASK_Sleep(100);
					}
				}
			}
		}

		s_bPlayTimerMode[param] = DI_MEDIA_TIMERMODE_UPDATE_FULLTIME;

		if ( NULL != s_tEventCallback[param] )
		{
			s_tEventCallback[param] ((HUINT32)param, DI_MEDIA_EVENT_BEGINOFSTREAM, 0);
		}
		/* else - do nothing */

		if(s_tEventCallback[param] != NULL)
		{
			s_tEventCallback[param]((HUINT32)param, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
		}
		/* else - do nothing */

		//	(void)DI_MEDIA_MP_SetSpeed(param, DI_MEDIA_TRICK_PAUSE);
	}

	PrintExit;

	return;
}

/**
 * @brief		Callback함수로 등록시 Stream 재생에 문제가 발생하면 호출된다.
 *
 * @param[in]	context 	변경하고자 하는 배속 정보를 지정한다.
 * @param[in]	param 		변경하고자 하는 배속 정보를 지정한다.
 * @return		void
 * @see			DI_MEDIA_RegisterCallback
 */
static void P_MEDIA_ErrorOfStreamCallback(void *context, int param)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_MEDIA_MSG			audMsg;
	VK_ERR_CODE			nVkResult = VK_OK;

	PrintEnter;

	BSTD_UNUSED(context);

	if(s_tMediaInstance[param].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if(s_tMediaInstance[param].bSuccesiveSeeking == TRUE)
		{
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
				eDIError = DRV_ERR;
			}
		}

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
 * @brief		Callback함수로 등록시 Stream 재생이 나면 호출된다.
 *
 * @param[in]	context 	변경하고자 하는 배속 정보를 지정한다.
 * @param[in]	param	 	변경하고자 하는 배속 정보를 지정한다.
 * @return		void
 * @see			DI_MEDIA_RegisterCallback
 */
static void P_MEDIA_EndOfStreamCallback(void *context, int param)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_MEDIA_MSG	audMsg;
	int				nVkResult = VK_OK;
	HUINT32			unLoopCount = 0;

	PrintEnter;

	BSTD_UNUSED(context);

	s_nSuccessiveSeekState = 0;

	if(s_tMediaInstance[param].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		eDIError = DI_ERR_ERROR;
	}
	else
	{
		if(s_tMediaInstance[param].bSuccesiveSeeking == TRUE)
		{
			PrintData("### [%s] Send DI_MEDIA_SUCCESSIVE_SEEK_STOP Message ###\n", __FUNCTION__);
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
			/* 빠르고&연속된 trick, seek의 경우 que full 발생 가능성 높음 */
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
			}
			else
			{
				audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
				audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
				nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
				if(nVkResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
				}
				else
				{
					for(unLoopCount = 0; unLoopCount < 10; unLoopCount++)
					{
						if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
						{
							break;
						}

						VK_TASK_Sleep(100);
					}
				}
			}
		}

		if ( NULL != s_tEventCallback[param] )
		{
			s_tEventCallback[param] ((HUINT32)param, DI_MEDIA_EVENT_ENDOFSTREAM, 0);
//			s_tMediaInstance[param].bEndOfStreamState = TRUE;
		}
		/* else - do nothing */
	}

	PrintExit;

	return;
}

static void P_MEDIA_DelayedStartCallback (HUINT32 unSessionId)
{
	HUINT32 unMediaId = 0;
	NEXUS_Error nResult;
	DI_IPTUNER_t *pHTuner=NULL;
	DI_ERR_CODE dResult;

	PrintEnter;
	pHTuner = DI_IPTUNER_POOL_FindById(unSessionId);
	if( pHTuner == NULL )
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
		return;
	}

	if(P_MEDIA_FindMediaId(unSessionId,&unMediaId) != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return;
	}

	dResult = P_MEDIA_CheckValidMediaId(unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}

	if(pHTuner->ulStartTime != 0)
	{
		nResult = NEXUS_Playback_Seek(s_tMediaInstance[unMediaId].playback, (NEXUS_PlaybackPosition)pHTuner->ulStartTime);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Playback_Seek failed!\n");
			VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			return DI_ERR_ERROR;
		}
	}

	nResult = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Playback_Play failed\n");
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}

	s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
	s_tMediaInstance[unMediaId].trickrate = eDI_MEDIA_Speed_Normal;

	if(s_tEventCallback[unMediaId] != NULL)
	{
		s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_START, 0);
	}

	dResult = DI_IPTUNER_UnregisterDelayedStartCB(pHTuner);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_UnregisterDelayedStartCB failed\n");
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}
	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return;
}

static void P_MEDIA_PlayerPauseCallback(HUINT32 unSessionId)
{
	/*HUINT32 i = 0;*/
	HUINT32 unMediaId = DI_MEDIA_MAX_NUM;
	PrintEnter;
	if(P_MEDIA_FindMediaId(unSessionId,&unMediaId) != DI_ERR_OK)
	{
		return;
	}
	DI_MEDIA_SetSpeed((SESSION_HANDLE)unSessionId, unMediaId, eDI_MEDIA_Speed_Pause);
	return;
}

static void P_MEDIA_PlayerResumeCallback(HUINT32 unSessionId)
{
	/*HUINT32 i = 0;*/
	HUINT32 unMediaId = DI_MEDIA_MAX_NUM;
	PrintEnter;
	if(P_MEDIA_FindMediaId(unSessionId,&unMediaId) != DI_ERR_OK)
	{
		return;
	}
	DI_MEDIA_SetSpeed((SESSION_HANDLE)unSessionId, unMediaId, eDI_MEDIA_Speed_Normal);
	return;
}

static void P_MEDIA_DelayedSeekCallback (HUINT32 unSessionId)
{
	HUINT32 i = 0;
	HUINT32 unMediaId = DI_MEDIA_MAX_NUM;
	PrintEnter;
	if(P_MEDIA_FindMediaId(unSessionId,&unMediaId) != DI_ERR_OK)
	{
		return;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return;
	}

	if(P_MEDIA_CheckValidMediaId(unMediaId) != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}

	/****************************************************
	 * SEEK 이후에도 STARTED Event 는 emit 되어야 한다.
	 ****************************************************/
	if(	s_tEventCallback[unMediaId] != NULL )
	{
		s_tEventCallback[unMediaId]((HUINT32)unMediaId,DI_MEDIA_EVENT_START, 0);
	}

	s_tMediaInstance[i].mediastate = DI_MEDIA_STATE_PLAY;
	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	return;
}

static void P_MEDIA_InetRadioChangeTitleCallback (HUINT32 unSessionId)
{
	HUINT32 unMediaId = DI_MEDIA_MAX_NUM;
	PrintEnter;
	if(P_MEDIA_FindMediaId(unSessionId,&unMediaId) != DI_ERR_OK)
	{
		return;
	}

	if(	s_tEventCallback[unMediaId] != NULL )
	{
		s_tEventCallback[unMediaId]((HUINT32)unMediaId, DI_MEDIA_EVENT_CHANGETITLE, 0);
	}

	return;
}

static void P_MEDIA_LiveBufferEventCallback (HUINT32 unSessionId, DI_IPTUNER_CLS_BUFSTATE_e nBufStateEvent)
{
	HUINT32 unMediaId = DI_MEDIA_MAX_NUM;
	PrintEnter;
	if(P_MEDIA_FindMediaId(unSessionId,&unMediaId) != DI_ERR_OK)
	{
		return;
	}

	if (s_tEventCallback[unMediaId] != NULL)
	{
		if (nBufStateEvent == DI_IPTUNER_CLS_BUFSTATE_BUFFERING)
		{
			s_tEventCallback[unMediaId]((HUINT32)unMediaId, DI_MEDIA_EVENT_LIVE_BUFFERING_READY, DI_IPTUNER_CLS_BUFSTATE_BUFFERING);
		}
		else if (nBufStateEvent == DI_IPTUNER_CLS_BUFSTATE_COMPLETED)
		{
			s_tEventCallback[unMediaId]((HUINT32)unMediaId, DI_MEDIA_EVENT_LIVE_BUFFERING_READY, DI_IPTUNER_CLS_BUFSTATE_COMPLETED);
		}
	}

	return;
}

static DI_ERR_CODE P_MEDIA_InstallFirstPts (HUINT32 unMediaId, HUINT32 unFirstPts)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return eDIError;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)s_tMediaInstance[unMediaId].pHSession);
	if(pstTunerHdl == NULL)
	{
		PrintError("[%s] Error - IP Tuner is Invalid.\n", __func__);
		return eDIError;
	}

	pstTunerHdl->unFirstPts = unFirstPts;
	pstTunerHdl->bPtsInstalled = TRUE;

	s_tMediaInstance[unMediaId].unFirstPts = unFirstPts;
	s_tMediaInstance[unMediaId].bPtsInstalled = TRUE;

	PrintError("[%s] Debug - First Pts Installted : %u \n", __func__,unFirstPts);

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
	 * SEEK 이후에도 STARTED Event 는 emit 되어야 한다.
	 ****************************************************/
	if(s_tMediaInstance[unMediaId].bPtsInstalled  == FALSE)
	{
		P_MEDIA_InstallFirstPts(unMediaId, unFirstPts);
	}
	return;
}


static DI_ERR_CODE P_MEDIA_RegisterFirstPtsCB (HUINT32 unMediaId, DI_MEDIA_FirstPtsCallback_t firstPtsCallback)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;

	PrintEnter;

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return eDIError;
	}

	s_tEventFirstPtsCallback[unMediaId] = firstPtsCallback;

	PrintExit;

	return eDIError;

}

/**
 * @brief		DI Video Codec을 Nexus의 Video Codec Type으로 변환한다.
 *
 * @param[in]	eVideoCodec 		Di Media Video Codec Type
 * @return		NEXUS_VideoCodec	Nexus Video Codec Type
 * @see			DRV_MEDIA_GetPlaybackVideoPidChannel
 */
static NEXUS_VideoCodec P_MEDIA_ConvertMediaVideoCodecToNexus(DI_VIDEO_FORMAT eVideoCodec)
{
	NEXUS_VideoCodec nexusVideoCodec;

	PrintEnter;

	switch(eVideoCodec)
	{
		case DI_VIDEO_FORMAT_MPEG1:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg1;
			break;
		case DI_VIDEO_FORMAT_MPEG2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			break;
		case DI_VIDEO_FORMAT_MPEG4Part2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg4Part2;
			break;
		case DI_VIDEO_FORMAT_H261:
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			break;
		case DI_VIDEO_FORMAT_H263:
			nexusVideoCodec = NEXUS_VideoCodec_eH263;
			break;
		case DI_VIDEO_FORMAT_H264:
			nexusVideoCodec = NEXUS_VideoCodec_eH264;
			break;
#if defined(CONFIG_VIDEO_DEC_H265)
		case DI_VIDEO_FORMAT_H265:
			nexusVideoCodec = NEXUS_VideoCodec_eH265;
			break;
#endif
		case DI_VIDEO_FORMAT_VC1SimpleMain:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1SimpleMain;
			break;
		case DI_VIDEO_FORMAT_VC1:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1;
			break;
		case DI_VIDEO_FORMAT_AVS:
			nexusVideoCodec = NEXUS_VideoCodec_eAvs;
			break;
		case DI_VIDEO_FORMAT_DIVX_311:
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
 * @brief		DI Audio Codec을 Nexus의 Audio Codec Type으로 변환한다.
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
			nexusCodec = NEXUS_AudioCodec_eLpcmDvd;//pcm???
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
 * @brief		DI Transport Type을 Nexus의 Transport Type으로 변환한다.
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
		case DI_MEDIA_MPEG_TYPE_MP4FRAGMENT: /* for SSTR */
			eNexusTrasportType = NEXUS_TransportType_eMp4Fragment; /* Fragmented MP4 Container */ /* for SSTR ykkim */
			break;
		case DI_MEDIA_MPEG_TYPE_FLV:
			eNexusTrasportType = NEXUS_TransportType_eFlv;
			break;
		case DI_MEDIA_MPEG_TYPE_MKV:
			eNexusTrasportType = NEXUS_TransportType_eMkv;
			break;
		default:
			eNexusTrasportType = NEXUS_TransportType_eUnknown;
			break;
	}

	PrintExit;

	return eNexusTrasportType;
}

#if 0
/**
 * @brief		Nexus의 TrickSpeed를 DI에 정의된 Trick Speed로 변환한다.
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
#endif

/**
 * @brief		Nexus의 TrickSpeed를 DI에 정의된 Trick Speed로 변환한다.
 *
 * @param[in]	nTrickSpeed 			Nexus Trick Speed
 * @return		DI_MEDIA_TrickSpeed_t	Di Trick Speed
 * @see
 */
int P_MEDIA_ConvertDITrickSpeedToNexus(DI_MEDIA_PlaySpeed_e nTrickSpeed)
{
	HINT32 eNexusTrickSpeed = 0;
	HINT32 nTableIdx = 0;
	PrintEnter;
	nTableIdx = nTrickSpeed - eDI_MEDIA_Speed_RW_X128;
	if(nTableIdx < 0 || (nTableIdx >= (HINT32)(sizeof(g_sTrickTable) / sizeof(P_MEDIA_TRICK_TABLE_t))))
	{
		nTableIdx = eDI_MEDIA_Speed_Normal - eDI_MEDIA_Speed_RW_X128;
	}
	eNexusTrickSpeed = g_sTrickTable[nTableIdx].rate;
#if 0
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
#endif
	PrintExit;

	return eNexusTrickSpeed;
}

/**
 * @brief		DI Transport Type을 Nexus의 Transport Type으로 변환한다.
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

DI_MEDIA_DLNA_FlagBits P_MEDIA_DLNA_FlagBits(unsigned long *value)
{
	DI_MEDIA_DLNA_FlagBits tRetValue = (DI_MEDIA_DLNA_FlagBits)value;

	return(tRetValue);
}

/*
 * ProtocolInfo의 정보를 Parsing한다.
 */
static DI_ERR_CODE P_MEDIA_DLNA_Protocolinfo_Parser (DI_IPTUNER_t *pHTuner, HINT8 *ProtocolInfo)
{
	HUINT32 		ulRet = ERR_OK;
	char 			*pParser = NULL;
	int				nDtcpRet = 0;
	unsigned int		i = 0;
	char			*pszParsing_temp=NULL;
	char			szDTCP1HOST[40];
	char			szDTCP1PORT[7];
	char			szMaxSpeed_Major[10];
	char			szMaxSpeed_Minor[10];
	char			szProfile[32];
	char			szPrimaryFlags[9];
	unsigned long	lPrimaryFlags = 0;
	bool			bMaxSpeed_Major_Parser = FALSE;
	bool			bFowardTrick = FALSE;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if ( NULL == ProtocolInfo )
	{
		PrintError ("%s (%d) Error>  invalid Protocol Info (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		pHTuner->tDlnaSetting.bDlna = TRUE;
	}

	if(VK_strstr((char *)ProtocolInfo,"http-get") != NULL)
	{
		pHTuner->tDlnaSetting.eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_HTTP;
		pHTuner->tDlnaSetting.bPlaySingle = FALSE;
		PrintData ("[%s] eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_HTTP\n",__FUNCTION__);
	}
	else if(VK_strstr((char *)ProtocolInfo,"playsingle-http-get") != NULL)
	{
		pHTuner->tDlnaSetting.eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_HTTP;
		pHTuner->tDlnaSetting.bPlaySingle = TRUE;
		PrintData ("[%s] eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_HTTP(PlaySingle)\n",__FUNCTION__);
	}
	else if(VK_strstr((char *)ProtocolInfo,"rtp-get") != NULL)
	{
		pHTuner->tDlnaSetting.eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP;
		pHTuner->tDlnaSetting.bPlaySingle = FALSE;
		PrintData ("[%s] eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP\n",__FUNCTION__);
	}
	else if(VK_strstr((char *)ProtocolInfo,"rtsp-rtp-udp") != NULL)
	{
		pHTuner->tDlnaSetting.eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP;
		pHTuner->tDlnaSetting.bPlaySingle = FALSE;
		PrintData ("[%s] eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP\n",__FUNCTION__);
	}
	else if(VK_strstr((char *)ProtocolInfo,"playsingle-rtsp-rtp-udp") != NULL)
	{
		pHTuner->tDlnaSetting.eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP;
		pHTuner->tDlnaSetting.bPlaySingle = TRUE;
		PrintData ("[%s] eProtocoltype = DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP\n",__FUNCTION__);
	}

	if((VK_strstr((char *)ProtocolInfo,"DTCP1HOST") != NULL) && (VK_strstr((char *)ProtocolInfo,"DTCP1PORT") != NULL))
	{
		pHTuner->tDtcpIpSetting.bDtcpIp = TRUE;

		if(s_bDtcpEnable == FALSE)
		{
			pHTuner->tDtcpIpSetting.AkeHandle = NULL;
		}
		else if (pHTuner->tDtcpIpSetting.AkeHandle == NULL)
		{
			VK_MEM_Memset(&szDTCP1HOST, 0x0, sizeof(szDTCP1HOST));
			VK_MEM_Memset(&szDTCP1PORT, 0x0, sizeof(szDTCP1PORT));

			pszParsing_temp = VK_strstr((char *)ProtocolInfo,"DTCP1HOST=");
			if(pszParsing_temp != NULL)
			{
				pszParsing_temp = pszParsing_temp+10;
				for(i = 0; i<40; i++)
				{
					if(pszParsing_temp[i] == ';')
					{
						break;
					}
					else
					{
						szDTCP1HOST[i] = pszParsing_temp[i];
					}
				}

				if(VK_strlen(szDTCP1HOST) > 1)
				{
					pHTuner->tDtcpIpSetting.pcDTCP1HOST	= DD_MEM_Alloc(VK_strlen(szDTCP1HOST)+1);
					if(pHTuner->tDtcpIpSetting.pcDTCP1HOST != NULL)
					{
						VK_snprintf(pHTuner->tDtcpIpSetting.pcDTCP1HOST, VK_strlen(szDTCP1HOST)+1, "%s", szDTCP1HOST);
					}
				}
			}

			pszParsing_temp = VK_strstr((char *)ProtocolInfo,"DTCP1PORT=");
			if(pszParsing_temp != NULL)
			{
				pszParsing_temp = pszParsing_temp+10;
				for(i = 0; i<7; i++)
				{
					if(pszParsing_temp[i] == ';')
					{
						break;
					}
					else
					{
						szDTCP1PORT[i] = pszParsing_temp[i];
					}
				}

				if(VK_strlen(szDTCP1PORT) > 1)
				{
					pHTuner->tDtcpIpSetting.pcDTCP1PORT = DD_MEM_Alloc(VK_strlen(szDTCP1PORT)+1);
					if(pHTuner->tDtcpIpSetting.pcDTCP1PORT != NULL)
					{
						VK_snprintf(pHTuner->tDtcpIpSetting.pcDTCP1PORT, VK_strlen(szDTCP1PORT)+1, "%s", szDTCP1PORT);
					}
				}
			}

			nDtcpRet = DtcpAppLib_DoAke(szDTCP1HOST, VK_atoi(szDTCP1PORT), &pHTuner->tDtcpIpSetting.AkeHandle);
			if(nDtcpRet != MXDTCP_SUCCESS)
			{
				pHTuner->tDtcpIpSetting.AkeHandle = NULL;
				if(pHTuner->tDtcpIpSetting.pcDTCP1HOST != NULL)
				{
					DD_MEM_Free(pHTuner->tDtcpIpSetting.pcDTCP1HOST);
					pHTuner->tDtcpIpSetting.pcDTCP1HOST = NULL;
				}

				if(pHTuner->tDtcpIpSetting.pcDTCP1PORT != NULL)
				{
					DD_MEM_Free(pHTuner->tDtcpIpSetting.pcDTCP1PORT);
					pHTuner->tDtcpIpSetting.pcDTCP1PORT = NULL;
				}
			}

			PrintData("[%s] szDTCP1HOST = %s, szDTCP1PORT = %d\n", __FUNCTION__, szDTCP1HOST, VK_atoi(szDTCP1PORT));
			PrintData("[%s] DtcpAppLib_DoAke DtcpRet = %d, AkeHandle = x%x\n", __FUNCTION__, nDtcpRet, pHTuner->tDtcpIpSetting.AkeHandle);
		}
	}
	else
	{
		pHTuner->tDtcpIpSetting.bDtcpIp = FALSE;
		PrintData ("[%s] bDtcpIp = FALSE\n",__FUNCTION__);
	}

	/* Profile (Format) */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_PN=");
	if(pParser != NULL)
	{
		pParser = pParser+7;
		VK_MEM_Memset(&szProfile, 0x0, sizeof(szProfile));

		for(i = 0; i<32; i++)
		{
			if(pParser[i] == ';')
			{
				break;
			}
			else
			{
				szProfile[i] = pParser[i];
			}
		}

		if(VK_strlen(szProfile) != 0)
		{
			pHTuner->tDlnaSetting.pcProfile = DD_MEM_Alloc(VK_strlen(szProfile)+1);
			VK_MEM_Memset(pHTuner->tDlnaSetting.pcProfile, 0x0, VK_strlen(szProfile)+1);
			VK_MEM_Memcpy(pHTuner->tDlnaSetting.pcProfile, szProfile, VK_strlen(szProfile));
		}

		PrintData("[%s] pcProfile = %s\n", __FUNCTION__, pHTuner->tDlnaSetting.pcProfile);
	}

	/* OP Code */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_OP=");
	if(pParser != NULL)
	{
		pParser = pParser+7;
		if((pParser[0] == '0') && (pParser[1] == '1'))
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK;
		}
		else if((pParser[0] == '1') && (pParser[1] == '1'))
		{
#ifdef CONFIG_DLNA_CERTIFICATION
			if( pHTuner != NULL && VK_strstr(pHTuner->tDlnaSetting.pcProfile, "MPEG_TS_JP_T") )
				pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK;
			else
#endif
				pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK;

		}
		else if((pParser[0] == '1') && (pParser[1] == '0'))
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK;
		}
		else
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_NONE;
		}
	}
	else
	{
		pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_NONE; /* Dont use BYTESEEK (CTT-7.4.26.7, CTT-7.4.38.11) */
	}

#ifdef CONFIG_DLNA_CERTIFICATION
	if( pHTuner != NULL && VK_strstr(pHTuner->tDlnaSetting.pcProfile, "MPEG_TS_JP_T") && pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK )
		pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_NONE;
#endif

	PrintData ("[%s] eSeekMode = %d\n",__FUNCTION__, pHTuner->tDlnaSetting.eSeekMode);

	/* Conversion Indication */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_CI=");
	if(pParser != NULL)
	{
		pParser = pParser+7;
		if(pParser[0] == '1')
		{
			pHTuner->tDlnaSetting.eConversion = DI_IPTUNER_DLNA_CONVERSION_TRANSCODED;
		}
		else
		{
			pHTuner->tDlnaSetting.eConversion = DI_IPTUNER_DLNA_CONVERSION_NONE;
		}
	}
	else
	{
		pHTuner->tDlnaSetting.eConversion = DI_IPTUNER_DLNA_CONVERSION_NONE;
	}

	PrintData ("[%s] eConversion = %d\n",__FUNCTION__, pHTuner->tDlnaSetting.eConversion);

	/* Play Speed */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_PS=");
	if(pParser != NULL)
	{
		VK_MEM_Memset(&pHTuner->tDlnaSetting.tPlaySpeed, 0x0, sizeof(pHTuner->tDlnaSetting.tPlaySpeed));

		pHTuner->tDlnaSetting.tPlaySpeed.bForward1x = TRUE;
		pHTuner->tDlnaSetting.eSpeed = DI_IPTUNER_DLNA_SPEED_NORMAL;

		pParser = pParser+7;
		bFowardTrick = TRUE;

		PrintData ("### [%s] Start PlaySpeed Parsing ###\n",__FUNCTION__);
		for(i = 0; i<20; i++)
		{
			if(VK_strlen(pParser) < i)
			{
				break;
			}

			if(pParser[i] == ';')
			{
				break;
			}
			else if(pParser[i] == ',')
			{
				bFowardTrick = TRUE;
				continue;
			}
			else
			{
				if(pParser[i] == '-')
				{
					bFowardTrick = FALSE;
					PrintData ("### [%s] Backward PlaySpeed Parsing ###\n",__FUNCTION__);
					continue;
				}

				if((pParser[i] == '1') && (VK_strlen(pParser) >= i+2))
				{
					if(pParser[i+1] == '/')
					{
						if(pParser[i+2] == '2')
						{
							if(bFowardTrick == TRUE)
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bForward1_2x = TRUE;
							}
							else
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bBackward1_2x = TRUE;
							}

							i = i+2;
							PrintData ("### [%s] Complete (+-)1/2 PlaySpeed Parsing ###\n",__FUNCTION__);
						}
						else if(pParser[i+2] == '4')
						{
							if(bFowardTrick == TRUE)
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bForward1_4x = TRUE;
							}
							else
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bBackward1_4x = TRUE;
							}

							i = i+2;
							PrintData ("### [%s] Complete (+-)1/4 PlaySpeed Parsing ###\n",__FUNCTION__);
						}
						else
						{
							i = i+2;
						}
					}
					else
					{
						if(bFowardTrick != TRUE)
						{
							pHTuner->tDlnaSetting.tPlaySpeed.bBackward1x = TRUE;
							PrintData ("### [%s] Complete -1 PlaySpeed Parsing ###\n",__FUNCTION__);
						}
					}
				}
				else if(pParser[i] == '1')
				{
					if(bFowardTrick != TRUE)
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bBackward1x = TRUE;
						PrintData ("### [%s] Complete -1 PlaySpeed Parsing ###\n",__FUNCTION__);
					}
				}
				else if(pParser[i] == '2')
				{
					if(bFowardTrick == TRUE)
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bForward2x = TRUE;
					}
					else
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bBackward2x = TRUE;
					}
					PrintData ("### [%s] Complete (+-)2 PlaySpeed Parsing ###\n",__FUNCTION__);
				}
				else if(pParser[i] == '4')
				{
					if(bFowardTrick == TRUE)
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bForward4x = TRUE;
					}
					else
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bBackward4x = TRUE;
					}
					PrintData ("### [%s] Complete (+-)4 PlaySpeed Parsing ###\n",__FUNCTION__);
				}
			}
		}
	}
	else
	{
		pHTuner->tDlnaSetting.eSpeed = DI_IPTUNER_DLNA_SPEED_INVALID;
	}

	/* Primary Flags */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_FLAGS=");
	if(pParser != NULL)
	{
		VK_MEM_Memset(szPrimaryFlags, 0x0, sizeof(szPrimaryFlags));

		pParser = pParser+10;
		for(i = 0; i<8; i++)
		{
			if(pParser[i] == ';')
			{
				break;
			}
			else
			{
				szPrimaryFlags[i] = pParser[i];
			}
		}

		lPrimaryFlags = strtoul(szPrimaryFlags,NULL,16);

		/*
		 * DLNA v1.5 Versioning Flag
		*/
		if(P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit20)
		{
			PrintData ("### [%s] DLNA Version 1.5 ###\n",__FUNCTION__);
			pHTuner->tDlnaSetting.eDlnaVersion = DI_IPTUNER_DLNA_VERSION_15;
		}
		else
		{
			PrintData ("### [%s] DLNA Version 1.0 ###\n",__FUNCTION__);
			pHTuner->tDlnaSetting.eDlnaVersion = DI_IPTUNER_DLNA_VERSION_10;
		}

		if(pHTuner->tDlnaSetting.eDlnaVersion == DI_IPTUNER_DLNA_VERSION_15)
		{
			/*
			 * Sender Paced Flag
			 * False = The Content Source is not the Content Clock Source
			 * True = The Content Source is the Content Clock Source
			*/
			pHTuner->tDlnaSetting.bSenderPaced = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit31;
			PrintData ("### [%s] bSenderPaced =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bSenderPaced);
			/*
			 * Limited Operations Flag (Time Base)
			 * False = Not Limited Random Access Data Availablility (Full Random Access or Not Support Random Access)
			 * True = Limited Random Access Data Availablility
			*/
			pHTuner->tDlnaSetting.bLimitedTimeBasedSeek = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit30;
			PrintData ("### [%s] bLimitedTimeBasedSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bLimitedTimeBasedSeek);

			/*
			 * Limited Operations Flag (Range Base)
			 * False = Not Limited Random Access Data Availablility (Full Random Access or Not Support Random Access)
			 * True = Limited Random Access Data Availablility
			*/
			pHTuner->tDlnaSetting.bLimitedRangeBasedSeek = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit29;
			PrintData ("### [%s] bLimitedRangeBasedSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bLimitedRangeBasedSeek);

			/*
			 * DLNA PlayContainer Flag (for DMR Devices(Optional))
			 * False = Not support DLNA PlayContainer URI Operation
			 * True = Support DLNA PlayContainer URI Operation
			*/
			pHTuner->tDlnaSetting.bDlnaPlayContainer = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit28;
			PrintData ("### [%s] bDlnaPlayContainer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bDlnaPlayContainer);

			/*
			 * UCDAM S0(Begin Position) Increasing Flag
			 * False = The S0 Data boundary is fixed.
			 * True = The S0 Data boundary increase with time.
			*/
			pHTuner->tDlnaSetting.bS0_Increasing = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit27;
			PrintData ("### [%s] bS0_Increasing =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bS0_Increasing);

			/*
			 * UCDAM SN(End Position) Increasing Flag
			 * False = The SN Data boundary is fixed.
			 * True = The SN Data boundary increase with time.
			*/
			pHTuner->tDlnaSetting.bS1_Increasing = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit26;
			PrintData ("### [%s] bS1_Increasing =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bS1_Increasing);

			/*
			 * Pause media opeation support for RTP Serving Endpoints
			 * False = Not support RTP Pause Operation.
			 * True = Support RTP Pause Operation.
			*/
			pHTuner->tDlnaSetting.bRtspPause = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit25;
			PrintData ("### [%s] bRtspPause =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bRtspPause);

			/*
			 * Streaming Mode Flag
			 * False = Interactive or Background Mode.
			 * True = Streaming Mode.
			*/
			pHTuner->tDlnaSetting.bStreamingTransfer = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit24;
			PrintData ("### [%s] bStreamingTransfer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bStreamingTransfer);

			/*
			 * Interactive Mode Flag
			 * False = Streaming or Background Mode.
			 * True = Interactive Mode.
			*/
			pHTuner->tDlnaSetting.bInteractiveTransfer = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit23;
			PrintData ("### [%s] bInteractiveTransfer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bInteractiveTransfer);

			/*
			 * Background Mode Flag
			 * False = Streaming or Interactive Mode.
			 * True = Background Mode.
			*/
			pHTuner->tDlnaSetting.bBackgroundTransfer = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit22;
			PrintData ("### [%s] bBackgroundTransfer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bBackgroundTransfer);

			/*
			 * HTTP Connection Stalling Flag
			 * False = Not support HTTP Stalling.
			 * True = Support HTTP Stalling(Pause and Pause-Release).
			*/
			pHTuner->tDlnaSetting.bConnectionStall = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit21;
			PrintData ("### [%s] bConnectionStall =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bConnectionStall);

			/*
			 * Link Protected Content Flag
			 * False = Not protected Content.
			 * True = Link Protected Content.
			*/
			pHTuner->tDlnaSetting.bLinkProtection = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit16;
			PrintData ("### [%s] bLinkProtection =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bLinkProtection);

			/*
			 * Cleartext Byte Full Data Seek Flag
			 * False = Not protected Content.
			 * True = Link Protected Content.
			*/
			pHTuner->tDlnaSetting.bClearTextFullSeek = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit15;
			PrintData ("### [%s] bClearTextFullSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bClearTextFullSeek);

			/*
			 * Cleartext Byte Limited Data Seek Flag
			 * False = Not protected Content.
			 * True = Link Protected Content.
			*/
			pHTuner->tDlnaSetting.bClearTextLimitedSeek = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit14;
			PrintData ("### [%s] bClearTextLimitedSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bClearTextLimitedSeek);
		}
		else
		{
			/*
			 * HTTP Connection Stalling Flag
			 * False = Not support HTTP Stalling.
			 * True = Support HTTP Stalling(Pause and Pause-Release).
			*/
			pHTuner->tDlnaSetting.bConnectionStall = P_MEDIA_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit21;
		}
	}

	/* Rtsp Max Speed */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_MAXSP=");
	if(pParser != NULL)
	{
		VK_MEM_Memset(&szMaxSpeed_Major, 0x0, sizeof(szMaxSpeed_Major));
		VK_MEM_Memset(&szMaxSpeed_Minor, 0x0, sizeof(szMaxSpeed_Minor));

		bMaxSpeed_Major_Parser = TRUE;
		pParser = pParser+10;
		for(i = 0; i<10; i++)
		{
			if(pParser[i] == ';')
			{
				break;
			}
			else if(pParser[i] == '.')
			{
				break;
			}
			else if(bMaxSpeed_Major_Parser == TRUE)
			{
				szMaxSpeed_Major[i] = pParser[i];
			}
			else
			{
				szMaxSpeed_Minor[i] = pParser[i];
			}
		}

		pHTuner->tDlnaSetting.nMaxSpeedMajor = VK_atoi(szMaxSpeed_Major);
		pHTuner->tDlnaSetting.nMaxSpeedMinor = VK_atoi(szMaxSpeed_Minor);
	}

	PrintData ("[%s] eSpeed = %d\n",__FUNCTION__, pHTuner->tDlnaSetting.eSpeed);

	PrintExit;

	return ulRet;
}

/*
 * DLNA Time base Seek에 대해 Bitrate를 Parsing한다.
 */
static DI_ERR_CODE P_MEDIA_DLNA_CheckTimeBitrate (DI_IPTUNER_t *pHTuner, VFIO_Handle_t tHandle, HINT64 lCheckTime, HINT32 *pnBitrate)
{
	HINT32		nRet = ERR_OK;
	HINT32		i=0, j=0, k=0;
	HUINT8		*pucData=NULL;
	HUINT8		*pucCheckData=NULL;
	HINT32		i_DataReadSize = 0;
	HINT32		i_RealDataReadSize = 0;
	HINT32		i_DataReadSize_tmp = 0;
	off_t		llSeekOffset = 0;
	off_t		llSearchStartOffset = 0;
	off_t		llSearchEndOffset = 0;
	HINT32		i_CheckDataError = 0;

	PrintEnter;

	*pnBitrate = 0;
	llSeekOffset = DI_MEDIA_BITRATE_TEMP;
	llSeekOffset = (llSeekOffset*(lCheckTime+1000))/1000;
	PrintDebug ("%s(%d) llSeekOffset=%d\n",__FUNCTION__,__LINE__, llSeekOffset);

	nRet = VFIO_Seek(tHandle, llSeekOffset, 0);
	if (nRet)
	{
		PrintError ("%s(%d) Error>  VFIO_Tell or VFIO_Seek()=%d",__FUNCTION__,__LINE__, nRet);
		*pnBitrate = -1;
		nRet = ERR_FAIL;
	}
	else
	{
		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto BITRATE_PARSER_ERR;
		}

		pucData = DD_MEM_Alloc(DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN);
		pucCheckData = DD_MEM_Alloc(DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN);
		if((pucData == NULL) || (pucCheckData == NULL))
		{
			PrintError ("Error>> Fail heap allocation \n");

			*pnBitrate = -1;
			nRet = ERR_FAIL;
			goto BITRATE_PARSER_ERR;
		}
		else
		{
			VK_MEM_Memset(pucData, 0x0, (DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN));
			VK_MEM_Memset(pucCheckData, 0x0, (DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN));

			i_DataReadSize = DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN;

			i_RealDataReadSize = 0;
			do
			{
				(void)VFIO_Read(tHandle, pucCheckData+i_RealDataReadSize, 1, DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN - i_RealDataReadSize, &i_DataReadSize_tmp);
				if(i_DataReadSize_tmp < 0)
				{
					break;
				}

				i_RealDataReadSize += i_DataReadSize_tmp;
			}
			while (i_RealDataReadSize < i_DataReadSize);

			for(i=0;i<i_RealDataReadSize;i++)
			{
				if((pucCheckData[i] == 0xFF) || (pucCheckData[i] == 0x00))
				{
					i_CheckDataError++;
				}
			}

			if(i_CheckDataError > ((i_RealDataReadSize*3)/5))
			{
				PrintError ("Error>> Read Data Error\n");

				*pnBitrate = -1;
				nRet = ERR_FAIL;
				goto BITRATE_PARSER_ERR;
			}

			if(i_RealDataReadSize != DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN)
			{
				PrintError ("Error>> Fail fread\n");

				*pnBitrate = -1;
				nRet = ERR_FAIL;
				goto BITRATE_PARSER_ERR;
			}

			llSeekOffset = DI_MEDIA_BITRATE_TEMP;
			llSeekOffset = (llSeekOffset*lCheckTime)/1000;
			PrintDebug ("%s(%d) llSeekOffset=%d\n",__FUNCTION__,__LINE__, llSeekOffset);

			nRet = VFIO_Seek(tHandle, llSeekOffset, 0);
			if (nRet)
			{
				PrintError ("%s(%d) Error>  VFIO_Tell or VFIO_Seek()=%d",__FUNCTION__,__LINE__, nRet);

				*pnBitrate = -1;
				nRet = ERR_FAIL;
				goto BITRATE_PARSER_ERR;
			}

			for(k=0; k<10; k++)
			{
				i_DataReadSize = (DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10);

				i_RealDataReadSize = 0;
				do
				{
					if((i_DataReadSize - i_RealDataReadSize) < DI_MEDIA_BITRATE_SEARCH_READ_LEN)
					{
						(void)VFIO_Read(tHandle, pucData+i_RealDataReadSize+(k*(DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10)), 1, (i_DataReadSize - i_RealDataReadSize), &i_DataReadSize_tmp);
					}
					else
					{
						(void)VFIO_Read(tHandle, pucData+i_RealDataReadSize+(k*(DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10)), 1, DI_MEDIA_BITRATE_SEARCH_READ_LEN, &i_DataReadSize_tmp);
					}

					if(i_DataReadSize_tmp < 0)
					{
						break;
					}

					i_RealDataReadSize += i_DataReadSize_tmp;
				}
				while (i_RealDataReadSize < i_DataReadSize);

				if(i_RealDataReadSize != (DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10))
				{
					PrintError ("Error>> Fail fread\n");

					*pnBitrate = -1;
					nRet = ERR_FAIL;
					goto BITRATE_PARSER_ERR;
				}

				if(k == 0)
				{
					llSearchStartOffset = 0;
					llSearchEndOffset = ((DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10)-DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN);
					/* lCheckTime + 500 msec의 위치가 어디인지 Check한다. */
					for(i=llSearchStartOffset; i<llSearchEndOffset; i++)
					{
						if((pucData[i] == pucCheckData[0]) && (pucData[i+1] == pucCheckData[1]) && (pucData[i+2] == pucCheckData[2])
							&& (pucData[i+3] == pucCheckData[3]) && (pucData[i+4] == pucCheckData[4]) && (pucData[i+5] == pucCheckData[5]))
						{
							for(j=1;j<=(DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN-1);j++)
							{
								if(pucData[i+j] != pucCheckData[j])
								{
									PrintDebug ("### i =%d, j =%d ###\n", i, j);
									break;
								}
							}

							if(j >= DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN-1)
							{
								if(i < 1024*2)
								{
									*pnBitrate = -1;
									PrintDebug ("Error>> Not Found(i=%d)\n", i);
									nRet = ERR_FAIL;
									goto BITRATE_PARSER_ERR;
								}
								else
								{
									*pnBitrate = i;
									PrintError ("### pnBitrate = %d ###\n", *pnBitrate);
									break;
								}
							}
							else
							{
								*pnBitrate = -1;
								PrintDebug ("Error>> Not Found\n");
								nRet = ERR_FAIL;
							}
						}
					}
				}
				else
				{
					llSearchStartOffset = (k*(DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10))-DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN;
					llSearchEndOffset = llSearchStartOffset + (DI_MEDIA_BITRATE_SEARCH_TOTAL_LEN/10);
					PrintDebug ("### llSearchStartOffset=%"PRId64", llSearchEndOffset=%"PRId64" ###\n", llSearchStartOffset, llSearchEndOffset);
					/* lCheckTime + 500 msec의 위치가 어디인지 Check한다. */
					for(i=llSearchStartOffset; i<llSearchEndOffset; i++)
					{
						if((pucData[i] == pucCheckData[0]) && (pucData[i+1] == pucCheckData[1]) && (pucData[i+2] == pucCheckData[2])
							&& (pucData[i+3] == pucCheckData[3]) && (pucData[i+4] == pucCheckData[4]) && (pucData[i+5] == pucCheckData[5]))
						{
							for(j=1;j<=(DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN-1);j++)
							{
								if(pucData[i+j] != pucCheckData[j])
								{
									PrintDebug ("### i =%d, j =%d ###\n", i, j);
									break;
								}
							}

							if(j >= DI_MEDIA_BITRATE_SEARCH_CHECKBYTE_LEN-1)
							{
								*pnBitrate = i;
								PrintError ("### i = %d, pnBitrate = %d ###\n", i, *pnBitrate);
								break;
							}
							else
							{
								*pnBitrate = -1;
								PrintDebug ("Error>> Not Found\n");
								nRet = ERR_FAIL;
							}
						}
					}

					if(*pnBitrate > 0)
					{
						break;
					}
				}
			}

			if(*pnBitrate == 0)
			{
				*pnBitrate = -1;
				PrintDebug ("Error>> Not Found\n");
				nRet = ERR_FAIL;
			}
		}
	}

BITRATE_PARSER_ERR:
	if(pucData != NULL)
	{
		DD_MEM_Free(pucData);
		pucData = NULL;
	}

	if(pucCheckData != NULL)
	{
		DD_MEM_Free(pucCheckData);
		pucCheckData = NULL;
	}

	PrintExit;

	return nRet;
}

/*
 * Bitrate를 Parsing한다.
 */
static DI_ERR_CODE P_MEDIA_DLNA_Bitrate_Parser (DI_IPTUNER_t *pHTuner, VFIO_Handle_t tHandle, HINT32 *pnBitrate)
{
	HINT32		nRet = ERR_OK;
	HINT32		nBitrate_Avg = 0;
	HINT32		nBitrate_Tmp = 0;
	HINT32		nFoundCount = 0;

	PrintEnter;

	/* Duration을 기준으로 Bitrate를 Check한다. */
	if(pHTuner->tDlnaSetting.unDuration > 60)
	{
		nRet = P_MEDIA_DLNA_CheckTimeBitrate(pHTuner, tHandle, 30000, &nBitrate_Tmp);
		if(nRet == ERR_OK)
		{
			nFoundCount++;
			nBitrate_Avg += nBitrate_Tmp;
		}

		nRet = P_MEDIA_DLNA_CheckTimeBitrate(pHTuner, tHandle, 40000, &nBitrate_Tmp);
		if(nRet == ERR_OK)
		{
			nFoundCount++;
			nBitrate_Avg += nBitrate_Tmp;
		}

		nRet = P_MEDIA_DLNA_CheckTimeBitrate(pHTuner, tHandle, 50000, &nBitrate_Tmp);
		if(nRet == ERR_OK)
		{
			nFoundCount++;
			nBitrate_Avg += nBitrate_Tmp;
		}

		if(nBitrate_Avg != 0)
		{
			*pnBitrate = nBitrate_Avg/nFoundCount;
		}

		nRet = VFIO_Seek(tHandle, 0, 0);
	}
	else if(pHTuner->tDlnaSetting.unDuration > 31)
	{
		nRet = P_MEDIA_DLNA_CheckTimeBitrate(pHTuner, tHandle, 15000, &nBitrate_Tmp);
		if(nRet == ERR_OK)
		{
			nFoundCount++;
			nBitrate_Avg += nBitrate_Tmp;
		}

		nRet = P_MEDIA_DLNA_CheckTimeBitrate(pHTuner, tHandle, 25000, &nBitrate_Tmp);
		if(nRet == ERR_OK)
		{
			nFoundCount++;
			nBitrate_Avg += nBitrate_Tmp;
		}

		if(nBitrate_Avg != 0)
		{
			*pnBitrate = nBitrate_Avg/nFoundCount;
		}

		nRet = VFIO_Seek(tHandle, 0, 0);
	}
	else if(pHTuner->tDlnaSetting.unDuration >= 3)
	{
		nRet = P_MEDIA_DLNA_CheckTimeBitrate(pHTuner, tHandle, 2000, &nBitrate_Tmp);
		*pnBitrate = nBitrate_Tmp;

		nRet = VFIO_Seek(tHandle, 0, 0);
	}
	else
	{
		nRet = ERR_FAIL;
	}

	PrintExit;

	return nRet;
}

/*
 * TS Bitrate를 Parsing한다.
 */
static DI_ERR_CODE P_MEDIA_DLNA_TS_Type_Parser (DI_IPTUNER_t *pHTuner, VFIO_Handle_t tHandle, DI_MEDIA_STREAM_TYPE_e *pnTs_StreamType)
{
	HINT32		nRet = ERR_OK;
	HINT32		i=0, j=0;
	HUINT8		*pucData=NULL;
	HUINT32		Count_TS_192Type=0;
	off_t		lCurOffset = 0;
	ssize_t		nReadSize;

	PrintEnter;

	nRet = VFIO_Tell(tHandle, &lCurOffset);
	nRet |= VFIO_Seek(tHandle, 0, 0);
	if (nRet)
	{
		PrintError ("%s(%d) Error>  VFIO_Tell or VFIO_Seek()=%d",__FUNCTION__,__LINE__, nRet);
		*pnTs_StreamType = DI_MEDIA_MPEG_TYPE_TS;
	}
	else
	{
		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto TS_TYPE_PARSER_ERR;
		}

		pucData = DD_MEM_Alloc(DI_MEDIA_TSTYPE_SEARCH_LEN);
		if (pucData == NULL)
		{
			PrintError ("Error>> Fail heap allocation\n");

			*pnTs_StreamType = DI_MEDIA_MPEG_TYPE_TS;
		}
		else
		{
			VK_MEM_Memset(pucData, 0x0, (DI_MEDIA_TSTYPE_SEARCH_LEN));
			(void)VFIO_Read(tHandle, pucData, 1, DI_MEDIA_TSTYPE_SEARCH_LEN, &nReadSize);
			if(nReadSize != DI_MEDIA_TSTYPE_SEARCH_LEN)
			{
				PrintError ("Error>> Fail fread\n");
				*pnTs_StreamType = DI_MEDIA_MPEG_TYPE_TS;
			}
			else
			{
				/* TS파일에 앞에 정보가 들어간 경우를 판단하기 위해 2K는 TS Packet인지 검토합니다. */
				for(i=0; i<DI_MEDIA_TSINFO_LEN; i++)
				{
					if(pucData[i] == MEDIA_MPEG2TS_PKT_SYNC)
					{
						for(j=1;j<=5;j++)
						{
							/* TS파일이 192packet인지 188packet인지 검토합니다. */
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
					*pnTs_StreamType = DI_MEDIA_MPEG_TYPE_TS_192;
				}
				else
				{
					PrintError ("##### [%s] DI_MEDIA_MPEG_TYPE_TS #####\n", __FUNCTION__);
					*pnTs_StreamType = DI_MEDIA_MPEG_TYPE_TS;
				}

				(void)VFIO_Seek(tHandle, lCurOffset, 0);
			}
		}
	}

TS_TYPE_PARSER_ERR:

	if(pucData != NULL)
	{
		DD_MEM_Free(pucData);
		pucData = NULL;
	}

	PrintExit;

	return nRet;
}

/**
 * @brief		Stream Probe를 위해 Resource를 할당하고 Probe를 진행한다.
 *
 * @param[in]	pszStreamUrl 	Todo
 * @param[in]	pMfpInfo 		Todo
 * @return		void
 * @see			DI_MEDIA_Probe
 */
static DI_ERR_CODE P_MEDIA_ProbeOpen (DI_IPTUNER_t *pHTuner, HINT8 *pszStreamUrl, stMediaInfo *pMfpInfo)
{
	DI_ERR_CODE 				nRet = DI_ERR_OK;
	bmedia_probe_t 				probe = NULL;
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
	bmedia_filter_cfg		filter_cfg;
	bstream_mpeg_type		stream_type;
	bmedia_player_config	player_config;
	unsigned 	audio_track,video_track,track_no;
	char 		stream_info[512];
	HUINT8		ucTotalAudioTrack=0;
	HUINT8		ucTotalVideoTrack=0;
	HINT32		i=0;
#ifdef  CONFIG_VFIO
	int	rc;
#else
	FILE		*pFile = NULL;
#endif

	PrintEnter;

	pMfpInfo->fd				= NULL;
	pMfpInfo->seek				= BMEDIA_PLAYER_INVALID;
	pMfpInfo->seek2				= BMEDIA_PLAYER_INVALID;
	pMfpInfo->buffer			= NULL;
	pMfpInfo->buffer_cfg.buf	= NULL;

	/* normal decode */
	pMfpInfo->direction		= 0;
	pMfpInfo->time_scale	= BMEDIA_TIME_SCALE_BASE;

	pMfpInfo->StreamInfo.audio_codec = DI_MEDIA_AUDIO_FORMAT_NONE;
	pMfpInfo->StreamInfo.audio_stream_id = 0;	/* AUDIO_STREAM_ID */
	pMfpInfo->StreamInfo.video_stream_id = 0;	/* VIDEO_STREAM_ID */
	pMfpInfo->StreamInfo.video_codec = DI_VIDEO_FORMAT_NONE;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if(pszStreamUrl == NULL)
	{
		PrintError ("%s(%d) Error - pszStreamUrl is null",__FUNCTION__,__LINE__);
		goto error;
	}

	if((pHTuner->tDtcpIpSetting.bDtcpIp == TRUE) && (pHTuner->tDtcpIpSetting.AkeHandle == NULL))
	{
		PrintError ("%s(%d) Error - AkeHandle is null\n",__FUNCTION__,__LINE__);
		goto error;
	}

#ifdef  CONFIG_VFIO
	{
		rc = VFIO_Open ((char *)pszStreamUrl, "rb", &(pMfpInfo->tHandle));
		if (rc)
		{
			PrintError ("%s(%d) Error>  VFIO_Open()=%d",__FUNCTION__,__LINE__, rc);
			goto error;
		}
		/* else { continue } */
	}

	//for Internet radio probing
	if(pHTuner->eInetRadioType != DI_IPTUNER_INETRADIO_TYPE_NONE)
	{
		DI_IPTUNER_INETRADIO_Probe(pHTuner, &pMfpInfo->StreamInfo, FALSE);
		(void)VFIO_Close (pMfpInfo->tHandle);

		PrintExit;
		return DI_ERR_OK;
	}

#else
	pMfpInfo->fin = fopen(pszStreamUrl, "rb");
	if (pMfpInfo->fin == NULL)
	{
		goto error;
	}
	/* else { continue } */
#endif

	/* setting for HTTP chunked live streaming (CLS) */
	pMfpInfo->StreamInfo.isHttpChunked = pHTuner->bHttpChunked;
	pMfpInfo->StreamInfo.isRangeAccepted = pHTuner->bRangeAccepted;
	if ((pHTuner->bHttpChunked == TRUE) && (pHTuner->bRangeAccepted== FALSE)&&(!pHTuner->tDlnaSetting.bDlna))
	{
		pMfpInfo->StreamInfo.bLiveUnderflowControl = TRUE;
	}
	else
	{
		pMfpInfo->StreamInfo.bLiveUnderflowControl = FALSE;
	}

	/* DLNA Time Base Seek이며 Bitrate값이 없는 경우 Bitrate Parsing */
	if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->tDlnaSetting.nByteRate <= 0) && (pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK))
	{
		nRet = P_MEDIA_DLNA_Bitrate_Parser(pHTuner, pMfpInfo->tHandle, &pHTuner->tDlnaSetting.nByteRate);
		if(nRet != ERR_OK)
		{
			PrintError ("%s(%d) Error>  Fail P_MEDIA_DLNA_Bitrate_Parser\n",__FUNCTION__,__LINE__);
			pHTuner->tDlnaSetting.nByteRate = DI_MEDIA_BITRATE_TEMP;
			nRet = DI_ERR_OK;
		}
		else
		{
			if(pHTuner->tDlnaSetting.nByteRate <= 0)
			{
				pHTuner->tDlnaSetting.nByteRate = 1024*1024*4;
			}

			PrintDebug ("%s(%d) pHTuner->tDlnaSetting.nBitrate=%d\n",__FUNCTION__,__LINE__, pHTuner->tDlnaSetting.nByteRate);
		}
	}

	pMfpInfo->factory = batom_factory_create(bkni_alloc, B_PUMP_DEMUX_FACTORY_ATOMS);
	if (pMfpInfo->factory == NULL)
	{
		goto error;
	}
	/* else { continue } */

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
		goto error;
	}

	(void)bfile_async_init();

	/* search stream type */
	probe = bmedia_probe_create();
	if (probe == NULL)
	{
		goto error;
	}
	/* else { continue } */

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
		goto error;
	}

#ifdef  CONFIG_VFIO
	fd = bfile_stdio_read_attach(pMfpInfo->tHandle);
#else
	fd = bfile_stdio_read_attach(pMfpInfo->fin);
#endif
	if(!fd)
	{
		PrintError ("%s(%d) Error> bfile_stdio_read_attach fail!\n",__FUNCTION__,__LINE__);
		goto error;
	}

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
		goto error;
	}

	(void)bmedia_probe_default_cfg(&probe_config);
	probe_config.file_name		= (char *)pszStreamUrl;
	probe_config.type		= bstream_mpeg_type_unknown;

#ifdef  CONFIG_VFIO /* do not probe aux (only in streaming)*/
	if((NULL != VK_strstr((char *)pszStreamUrl, "http://"))
		|| (NULL != VK_strstr((char *)pszStreamUrl, "https://"))
		|| (NULL != VK_strstr((char *)pszStreamUrl, "mms://"))
		|| (NULL != VK_strstr((char *)pszStreamUrl, "rtsp://"))
		|| (NULL != VK_strstr((char *)pszStreamUrl, "file://"))
		|| (NULL != VK_strstr((char *)pszStreamUrl, "dvr://"))
		)
	{
		probe_config.probe_es = false;
		probe_config.probe_payload = false;
		PrintData ("[%s] do not probe aux : %s \n",__FUNCTION__, pszStreamUrl );
	}
	else
	{
		PrintData ("[%s] probe aux : %s \n",__FUNCTION__, pszStreamUrl );
	}
#endif

#if 0 /* DMS에 Duration이 정확하지 않은 경우가 발생하므로 Decoder의 Duration으로 변경함. */
	if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK))
	{
	    probe_config.probe_duration = FALSE;
	    probe_config.probe_index = FALSE;
	}
#endif

	PrintData ("[%s] bmedia_probe_parse\n",__FUNCTION__);
	stream = bmedia_probe_parse (probe, fd, &probe_config);

	(void)bfile_stdio_read_detach(fd);
	fd = NULL;

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
		goto error;
	}

	if(stream == NULL)
	{
		PrintError ("[%s] Error>> unknown stream format|\n",__FUNCTION__);
		goto error;
	}
	/* else { continue } */

	(void)bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
		goto error;
	}

	PrintData ("[%s] stream_info : %s|\n",__FUNCTION__,stream_info);
	PrintData ("[%s] max_bitrate : %u|\n",__FUNCTION__,stream->max_bitrate);
	PrintData ("[%s] index : %d|\n",__FUNCTION__,stream->index);
	PrintData ("[%s] duration : %d|\n",__FUNCTION__,stream->duration);
	PrintData ("[%s] nprograms : %d|\n",__FUNCTION__,stream->nprograms);
	PrintData ("[%s] ntracks : %d|\n",__FUNCTION__,stream->ntracks);
	PrintData ("[%s] type : %d|\n",__FUNCTION__,stream->type);
	PrintData ("[%s] probe_id : %d|\n",__FUNCTION__,stream->probe_id);

	if(pHTuner->tMediaInfo.eDrmType == DI_MEDIA_DRM_TYPE_MARLIN)
	{
		DRM_Key_Info_t drm_key_info;

		drm_key_info.offer.marlin.cmd = DI_DRM_MARLIN_SETDATA;
		drm_key_info.offer.marlin.stream = (void*)stream;

		if( DRV_DRM_GetKey(pHTuner, &drm_key_info) != DRV_OK )
		{
			PrintError ("%s(%d) Error>	Probe Marlin DRM failed",__FUNCTION__,__LINE__);
			goto error;
		}

		pMfpInfo->StreamInfo.ca_pid = drm_key_info.result.marlin.ecm_pid;
		pMfpInfo->StreamInfo.eDrmType = pHTuner->tMediaInfo.eDrmType;

	}

	stream_type = stream->type;
#ifdef CONFIG_DLNA_CERTIFICATION
	if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->tDlnaSetting.unDuration > 0))
#else
	if((pHTuner->tDlnaSetting.bDlna == TRUE) && (stream->duration == 0))
#endif
	{
		pMfpInfo->StreamInfo.duration = (pHTuner->tDlnaSetting.unDuration*1000);
	}
	else
	{
		pMfpInfo->StreamInfo.duration	= stream->duration;
	}

	pMfpInfo->StreamInfo.max_bitrate	= stream->max_bitrate;
	pMfpInfo->StreamInfo.nprograms		= stream->nprograms;
	pMfpInfo->StreamInfo.ntracks		= stream->ntracks;

	if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK))
	{
		pMfpInfo->StreamInfo.max_bitrate = pHTuner->tDlnaSetting.nByteRate*8;
	}
	else if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_NONE)
		 && (pMfpInfo->StreamInfo.max_bitrate == 0))
	{
		pMfpInfo->StreamInfo.max_bitrate = 1024*1024*8;
	}

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

	for(audio_track=0,video_track=0,track_no=1,track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link),track_no++)
	{
		PrintData ("track->number = %d\n", track->number);

		switch(track->type)
		{
			case bmedia_track_type_audio:
				if(ucTotalAudioTrack < DI_MEDIA_MAX_AUDIO_TRACK)
				{
					PrintData ("audio track %u codec:0x%x channel count %d sample size %d bitrate %d sample rate %d\n",
						track->number, track->info.audio.codec, track->info.audio.channel_count, track->info.audio.sample_size, track->info.audio.bitrate, track->info.audio.sample_rate);

					pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_pes_id = DI_MEDIA_AUDIO_PES_ID;
					pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_stream_id = track->number;

					switch (track->info.audio.codec)
					{
						case baudio_format_mpeg:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_MPEG;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_MPEG;
							audio_track = track->number;
							break;
						case baudio_format_mp3:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_MP3;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_MP3;
							audio_track = track->number;
							break;
						case baudio_format_aac:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_AAC;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_AAC;
							audio_track = track->number;
							break;
						case baudio_format_aac_plus:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_AAC_PLUS;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_AAC_PLUS;
							audio_track = track->number;
							break;
						case baudio_format_aac_plus_adts:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_AAC_PLUS_ADTS;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_AAC_PLUS_ADTS;
							audio_track = track->number;
							break;
						case baudio_format_ac3:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_AC3;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_AC3;
							audio_track = track->number;
							break;
						case baudio_format_ac3_plus:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_AC3_PLUS;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_AC3_PLUS;
							audio_track = track->number;
							break;
						case baudio_format_dts:
#if 0		/* 현재 DTS Codec은 지원하지 않습니다. */
							pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_DTS;
							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_DTS;
#else
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_NONE;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_NONE;
#endif
							break;
						case baudio_format_lpcm_hddvd:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_LPCM_HD_DVD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_LPCM_HD_DVD;
							audio_track = track->number;
							break;
						case baudio_format_lpcm_bluray:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	=  DI_AUDFORMAT_LPCM_BLUERAY;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_LPCM_BLUERAY;
							audio_track = track->number;
							break;
						case baudio_format_dts_hd:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_DTS_HD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_DTS_HD;
							audio_track = track->number;
							break;
						case baudio_format_wma_std:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_WMA_STD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_WMA_STD;
							audio_track = track->number;
							break;
						case baudio_format_wma_pro:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_WMA_PRO;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_WMA_PRO;
							audio_track = track->number;
							break;
						case baudio_format_lpcm_dvd:
							if((ucTotalAudioTrack == 0) || ((ucTotalAudioTrack != 0) && (pMfpInfo->StreamInfo.audio_codec == DI_AUDFORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_LPCM_DVD;
							}

							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_LPCM_DVD;
							audio_track = track->number;
							break;
						default:
							if(ucTotalAudioTrack == 0)
							{
								pMfpInfo->StreamInfo.audio_codec	= DI_AUDFORMAT_NONE;
							}
							pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].audio_codec = DI_AUDFORMAT_NONE;
							break;
					}

					if((pMfpInfo->StreamInfo.audio_codec != DI_AUDFORMAT_NONE) && (pMfpInfo->StreamInfo.audio_stream_id == 0))
					{
						pMfpInfo->StreamInfo.audio_stream_id	= audio_track;	/* AUDIO_STREAM_ID */
					}

					pMfpInfo->StreamInfo.audio_pes_id	= DI_MEDIA_AUDIO_PES_ID;

					if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_AVI)
					{
						VK_strncpy(pMfpInfo->StreamInfo.stAudioTrack[ucTotalAudioTrack].uacAudioLang, ((bavi_probe_track *)track)->language, 16);
					}

					ucTotalAudioTrack++;
				}

				break;
			case bmedia_track_type_video:
				if(ucTotalVideoTrack < DI_MEDIA_MAX_VIDEO_TRACK)
				{
					PrintData ("video track %u codec:0x%x width %d height %d\n", track->number, track->info.video.codec, track->info.video.width, track->info.video.height);

					pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_pes_id = DI_MEDIA_VIDEO_PES_ID;
					pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_stream_id = track->number;

					switch (track->info.video.codec)
					{
						case bvideo_codec_divx_311:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_DIVX_311;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_DIVX_311;
							video_track = track->number;
							break;
						case bvideo_codec_mpeg1:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_MPEG1;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_MPEG1;
							video_track = track->number;
							break;
						case bvideo_codec_mpeg2:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_MPEG2;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_MPEG2;
							video_track = track->number;
							break;
						case bvideo_codec_mpeg4_part2:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_MPEG4Part2;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_MPEG4Part2;
							video_track = track->number;
							break;
						case bvideo_codec_h263:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_H263;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_H263;
							video_track = track->number;
							break;
						case bvideo_codec_h264:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_H264;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_H264;
							video_track = track->number;
							break;
#if defined(CONFIG_VIDEO_DEC_H265)
						case bvideo_codec_h265:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_H265;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_H265;
							video_track = track->number;
							break;
#endif
						case bvideo_codec_vc1:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_VC1;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_VC1;
							video_track = track->number;
							break;
						case bvideo_codec_vc1_sm:
							if((ucTotalVideoTrack == 0) || ((ucTotalVideoTrack != 0) && (pMfpInfo->StreamInfo.video_codec == DI_VIDEO_FORMAT_NONE)))
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
							video_track = track->number;
							break;
						case bvideo_codec_mjpeg:
							continue;
						default:
							if(ucTotalVideoTrack == 0)
							{
								pMfpInfo->StreamInfo.video_codec	= DI_VIDEO_FORMAT_NONE;
							}
							pMfpInfo->StreamInfo.stVideoTrack[ucTotalVideoTrack].video_codec = DI_VIDEO_FORMAT_NONE;
							break;
					}

#if 0
					if((pMfpInfo->StreamInfo.video_codec != DI_AUDFORMAT_NONE) && (pMfpInfo->StreamInfo.video_stream_id == 0))
#else
					if((pMfpInfo->StreamInfo.video_codec != DI_VIDEO_FORMAT_NONE) && (pMfpInfo->StreamInfo.video_stream_id == 0))
#endif
					{
						pMfpInfo->StreamInfo.video_stream_id	= video_track;	/* VIDEO_STREAM_ID */
					}

					pMfpInfo->StreamInfo.video_pes_id	= DI_MEDIA_VIDEO_PES_ID;

					if(pMfpInfo->StreamInfo.video_codec != DI_VIDEO_FORMAT_NONE)
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
			default:
				break;
		}
	}

	PrintData ("[%s] Detected format:%u video:%u audio:%u|\n"
			,__FUNCTION__
			, pMfpInfo->StreamInfo.stream_type
			, pMfpInfo->StreamInfo.video_stream_id
			, pMfpInfo->StreamInfo.audio_stream_id);
	PrintData ("[%s] Total Audio Track:%u Total Video Track:%u|\n",__FUNCTION__, ucTotalAudioTrack, ucTotalVideoTrack);

	pMfpInfo->StreamInfo.ucTotalAudioTrack = ucTotalAudioTrack;
	pMfpInfo->StreamInfo.ucTotalVideoTrack = ucTotalVideoTrack;

	(void)bmedia_probe_stream_free(probe, stream);
	(void)bmedia_probe_destroy(probe);
	stream	= NULL;
	probe	= NULL;

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
		goto error;
	}

	if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
	{
		nRet = P_MEDIA_DLNA_TS_Type_Parser(pHTuner, pMfpInfo->tHandle, &pMfpInfo->StreamInfo.stream_type);
		if(nRet != ERR_OK)
		{
			PrintError ("%s(%d) Error>  P_MEDIA_DLNA_TS_Type_Parser",__FUNCTION__,__LINE__);
			pMfpInfo->StreamInfo.stream_type = DI_MEDIA_MPEG_TYPE_TS;
			nRet = ERR_OK;
		}
	}

	if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_UNKNOWN)
	{
		nRet = DI_ERR_INVALID_PARAM;
		goto error;
	}
#ifdef __SUPPORT_FIRSTPTS_DETECTION__
	else if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS
	 || pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS_192 )
	{
#if 1
#if 1 	/* workaround : junsy */
		/* 이 기능은 추후 DI_IPTUNER_Probe 에서 drm content 들에 대한 firstpts 추출이 완료되는대로
		 관련 함수 및 parameter 들 삭제 필요함.  */
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

#ifdef	__USE_VFIO__
			fd = bfile_stdio_read_attach(pMfpInfo->tHandle);
#else
			fd = bfile_stdio_read_attach(pMfpInfo->fin);
#endif
			if(fd == NULL)
			{
				PrintError ("%s(%d) Error> bfile_stdio_read_attach fail!\n",__FUNCTION__,__LINE__);
				goto error;
			}

			if(pHTuner->bProbeCancel == TRUE)
			{
				PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
				goto error;
			}

			pMfpInfo->pipe_media= batom_pipe_create(pMfpInfo->factory);
			if (pMfpInfo->pipe_media == NULL)
			{
				PrintError ("%s(%d) Error> batom_pipe_create fail!\n",__FUNCTION__,__LINE__);
				goto error;
			}

			if(pHTuner->bProbeCancel == TRUE)
			{
				PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
				goto error;
			}

			bmpeg2ts_parser_default_cfg(&parser_cfg);
			if(pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
			{
				parser_cfg.header_size = 4;
			}
			parser = bmpeg2ts_parser_create(pMfpInfo->factory, &parser_cfg);
			test_pid.pcr_count = 0;
			test_pid.ts_bytes = 0;
			test_pid.pts_count = 0;
			test_pid.pes_bytes = 0;

			if(pHTuner->bProbeCancel == TRUE)
			{
				PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
				goto error;
			}
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
				goto error;
			}

			pMfpInfo->buffer_cfg.fd = fd;
			pMfpInfo->buffer_cfg.nsegs = buf_size/block_size;

			pMfpInfo->buffer = bfile_buffer_create(pMfpInfo->factory, &pMfpInfo->buffer_cfg);
			if (pMfpInfo->buffer == NULL)
			{
				PrintError ("%s(%d) Error>> bfile_buffer_create fail \n",__FUNCTION__,__LINE__);
				goto error;
			}

			if(pHTuner->bProbeCancel == TRUE)
			{
				PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
				goto error;
			}

			rc = bmpeg2pes_parser_init(pMfpInfo->factory, &test_pid.pes, BMPEG2PES_ID_ANY);
			if(rc!=0)
			{
				PrintError ("%s(%d) Error>> bmpeg2pes_parser_init fail\n",__FUNCTION__,__LINE__);
				goto error;
			}

			test_pid.mfp_pointer = (void*)pMfpInfo;
			test_pid.pes.packet_cnxt = &test_pid;
			test_pid.pes.packet = P_MEDIA_GettingPESPacket;
			for(bytes_parsed=0,i=0;i<1;i++)
			{
				if(pHTuner->bProbeCancel == TRUE)
				{
					PrintDebug(" Probe state is cancel and Exit FirstPts Operation \n");
					break;
				}

				test_pid.ts_pid.payload = P_MEDIA_GettingTSPayload;
				bmpeg2ts_parser_add_pid(parser, &test_pid.ts_pid);
				for(off=0;/* off<1024*1024*1024 */;) {
					if(pHTuner->bProbeCancel == TRUE)
					{
						PrintDebug(" Probe state is cancel and Exit FirstPts Operation \n");
						break;
					}

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
						if(off > (pMfpInfo->StreamInfo.max_bitrate/8)*5) /* Read한 Size가 Content bitrate 5초분량 보다 큰 경우 Fail로 Return함 */
						{
							break;
						}
					}
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
		pMfpInfo->StreamInfo.support_seek	= TRUE;
	}
#else

#ifdef CONFIG_DLNA_CERTIFICATION
	else if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS || pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_MPEG1)
#else
	else if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
#endif
	{
		pMfpInfo->no_player 			= true;
		pMfpInfo->StreamInfo.support_trick	= TRUE;
		/* 2008.07.08 - by Dylan, Check the seek support */
		pMfpInfo->StreamInfo.support_seek 	= TRUE;
	}
#endif
	else
	{
		if(stream_type == bstream_mpeg_type_vob)
		{
#ifdef CONFIG_DLNA_CERTIFICATION
			pMfpInfo->StreamInfo.support_trick  = TRUE;
			pMfpInfo->StreamInfo.support_seek   = TRUE;
#endif
			goto success;
		}

		pMfpInfo->pipe_media= batom_pipe_create(pMfpInfo->factory);
		if (pMfpInfo->pipe_media == NULL)
		{
			goto error;
		}
		/* else { continue } */

		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto error;
		}

		(void)bmedia_filter_default_cfg(&filter_cfg);
		filter_cfg.eos_len	= sizeof(s_eos_data);
		filter_cfg.eos_data	= s_eos_data;
		pMfpInfo->filter	= bmedia_filter_create(pMfpInfo->factory, bkni_alloc, &filter_cfg);
		if (pMfpInfo->filter == NULL)
		{
			goto error;
		}
		/* else { continue } */

		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto error;
		}

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

		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto error;
		}

		if(pMfpInfo->StreamInfo.video_stream_id)
		{
#if defined(NEXUS_VERSION) && NEXUS_VERSION >= 410
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
#if defined(NEXUS_VERSION) && NEXUS_VERSION >= 410
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

#ifdef	__USE_VFIO__
		pMfpInfo->fd = bfile_stdio_read_attach(pMfpInfo->tHandle);
#else
		pMfpInfo->fd = bfile_stdio_read_attach(pMfpInfo->fin);
#endif

		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto error;
		}

		(void)bfile_buffer_default_cfg(&pMfpInfo->buffer_cfg);
#if 1	/* Nexus에서 사용하는 Buffer를 Check하여 적용해야함. 현재는 32*15*4096으로 됨. */
		/* buf_len은 조정가능한 사이즈로 현재는 Default를 활용함. */
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

		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto error;
		}

		(void)bmedia_player_init_stream(&pMfpInfo->stream);
		pMfpInfo->stream.format		= stream_type;
		pMfpInfo->stream.master		= pMfpInfo->StreamInfo.video_stream_id;
		pMfpInfo->stream.other[0]	= pMfpInfo->StreamInfo.audio_stream_id;

		pMfpInfo->stream.without_index	= false;

		if(pHTuner->tDlnaSetting.bDlna == TRUE)
		{
			pMfpInfo->stream.without_index	= TRUE;
			pMfpInfo->stream.stream.noindex.auto_rate = TRUE;
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
#ifdef CONFIG_DLNA_CERTIFICATION
		if (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_PES)
		{
			player_config.factory = pMfpInfo->factory;
		}
#endif
		if(pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
			goto error;
		}

#if 1 /* for YouView(Canvas) by ykkim */
		if((VK_strstr((char *)pszStreamUrl,"file://") || VK_strstr((char *)pszStreamUrl,"dvr://")) && (stream_type == bstream_mpeg_type_asf))
#else
		if(VK_strstr((char *)pszStreamUrl,"file://") && (stream_type == bstream_mpeg_type_asf))
#endif
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

				if(pHTuner->bProbeCancel == TRUE)
				{
					PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
					goto error;
				}

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
				if(pHTuner->bProbeCancel == TRUE)
				{
					PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
					goto error;
				}

				if(pMfpInfo->seek!=BMEDIA_PLAYER_INVALID)
				{
					bmedia_player_seek(pMfpInfo->player, pMfpInfo->seek);
				}

				if(pHTuner->bProbeCancel == TRUE)
				{
					PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
					goto error;
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

				if(pHTuner->bProbeCancel == TRUE)
				{
					PrintError ("%s(%d) Error>  Probe Cancel",__FUNCTION__,__LINE__);
					goto error;
				}

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
success:
	if(pHTuner->tDlnaSetting.bDlna == TRUE)
	{
		if((pHTuner->tDlnaSetting.bLimitedRangeBasedSeek == TRUE) && (pHTuner->tDlnaSetting.bLimitedRangeBasedSeek == TRUE))
		{
			pMfpInfo->StreamInfo.support_seek = true;
			pMfpInfo->StreamInfo.support_trick = true;
		}

		if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_NONE))
		{
			pMfpInfo->StreamInfo.support_seek = false;

#ifdef CONFIG_DLNA_CERTIFICATION // mklim added, 7.4.63.1+1 MT HTTP Slow Forward Scan Media Operation: AV
			if( pHTuner->tDlnaSetting.bConnectionStall == FALSE )
				pMfpInfo->StreamInfo.support_trick = false;
			else
#endif
				pMfpInfo->StreamInfo.support_trick = true;
		}
	}

	pMfpInfo->StreamInfo.demuxId = pHTuner->nDemuxId;

	/*
	 * bSuccesiveSeekForTrick field is only set to true in case of DLNA DMP.
	 * The trick behavior looks like seeking should be applied for TS format only.
	 *
	 * Futhermore, Trick flag is enabled at the probe-time when a media file is able to be opend with index.
	 */
	if((pHTuner->tDlnaSetting.bDlna == TRUE) && (pHTuner->bSuccesiveSeekForTrick == TRUE))
	{
		if( (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS)
				|| (pMfpInfo->StreamInfo.stream_type == DI_MEDIA_MPEG_TYPE_TS_192))
		{
			/* in case of TS format */
			pMfpInfo->StreamInfo.bSuccesiveSeekMode = TRUE;
			pHTuner->tMediaInfo.bSuccesiveSeekMode = TRUE;
		}
		/* such as AVI, MKV, ASF etc */
		else
		{
			/* clear flags */
			pHTuner->bSuccesiveSeekForTrick = FALSE;
			pMfpInfo->StreamInfo.bSuccesiveSeekMode = FALSE;
			pHTuner->tMediaInfo.bSuccesiveSeekMode = FALSE;
		}
	}
	else
	{
		/* clear flags */
		pHTuner->bSuccesiveSeekForTrick = FALSE;
		pMfpInfo->StreamInfo.bSuccesiveSeekMode = FALSE;
		pHTuner->tMediaInfo.bSuccesiveSeekMode = FALSE;
	}

	pMfpInfo->pszStreamUrl = DD_MEM_Alloc(VK_strlen(pszStreamUrl)+1);
	if(pMfpInfo->pszStreamUrl == NULL)
	{
		PrintError("%s(%d) Out of Memory\n", __FUNCTION__, __LINE__);
		goto error;
	}
	VK_memset(pMfpInfo->pszStreamUrl, 0x00, VK_strlen(pszStreamUrl)+1);

	(void)VK_strncpy (pMfpInfo->pszStreamUrl, (char *)pszStreamUrl, VK_strlen(pszStreamUrl));
	pMfpInfo->opened	= true;
	pMfpInfo->ullPrevPosition	= 0;
	pMfpInfo->ullEosPosition	= 0;

	PrintExit;

	return nRet;

error:
	if (fd != NULL)
	{
		(void)bfile_stdio_read_detach(fd);
		fd = NULL;
	}
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

	if (nRet == DI_ERR_OK)
	{
		nRet = DI_ERR_ERROR;
	}
	/* else { continue } */

	PrintExit;

	return nRet;
}

/**
 * @brief		Stream Probe를 위해 할당한 Resource를 해제한다.
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

	if (pMfpInfo->pszStreamUrl != NULL)
	{
		(void)DD_MEM_Free (pMfpInfo->pszStreamUrl);
		pMfpInfo->pszStreamUrl = NULL;
	}

	/* 2008.12.01 by Dylan - struct should be cleared, this is the static define !!! */
	VK_memset(pMfpInfo, 0x0, sizeof(stMediaInfo));
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

#ifdef __SUPPORT_FIRSTPTS_DETECTION__
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

DI_ERR_CODE P_MEDIA_SetEndTime (HUINT32 unMediaId, HUINT32 ulEndTime)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return eDIError;
	}

	s_ulMediaEndTime[unMediaId] = ulEndTime;
	PrintError("%s(%d) : Set ENDOFTIME to media (%d) !!\n",__FUNCTION__,__LINE__,(HUINT32)s_ulMediaEndTime[unMediaId]);

	return DI_ERR_OK;
}

static DI_ERR_CODE P_MEDIA_SetSeekMode (HUINT32 unMediaId, HBOOL bAccurate)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackSettings playbackSettings;

	PrintEnter;

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return eDIError;
	}

	PrintDebug ("[%s] Debug - bAccurate = %s\n",__func__,bAccurate?"TRUE":"FALSE");

	(void)NEXUS_Playback_GetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
	playbackSettings.accurateSeek = bAccurate;
	tNexusError = NEXUS_Playback_SetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
	if(NEXUS_SUCCESS != tNexusError)
	{
		PrintError("[%s] Error - Fail NEXUS_Playback_SetSettings:%d\n", __func__, tNexusError);
		eDIError = DI_ERR_ERROR;
	}

	PrintExit;
	return eDIError;
}

static HBOOL P_MEDIA_GetSeekMode (HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	/*NEXUS_Error tNexusError = NEXUS_SUCCESS;*/
	NEXUS_PlaybackSettings playbackSettings;
	HBOOL bAccurate = FALSE;
	PrintEnter;
	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return FALSE;
	}
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return FALSE;
	}
	(void)NEXUS_Playback_GetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
	bAccurate = playbackSettings.accurateSeek;
	PrintExit;
	return bAccurate;
}



/***************************************
 * PTS based playing-position in decoder
 *  : it is not compatable with multiple playback envirionments.
 *    support single playback only.
 ***************************************/
static void P_MEDIA_PlayTimeTask(void *param)
{
	NEXUS_Error				tNexusError = NEXUS_SUCCESS;
	HUINT32					nTempPlayTime[DI_MEDIA_MAX_NUM] = {0,};
	HUINT32					unPtsSnapShot[DI_MEDIA_MAX_NUM] = {0,};
	int i = 0;
	NEXUS_PlaybackStatus pStatus;

	(void)UNUSED_PARAMETER(param);

	while(1)
	{
		for(i=0;i<DI_MEDIA_MAX_NUM;i++)
		{
			/* The time event is only valid at a playing state. */
			if((s_tEventCallback[i] != NULL)
				&& (s_tMediaInstance[i].mediastate == DI_MEDIA_STATE_PLAY) )
			{
				if(s_tMediaInstance[i].playback == NULL)
				{
					continue;
				}
				if(((s_tMediaInstance[i].trickrate > eDI_MEDIA_Speed_FF_X4) ||
					(s_tMediaInstance[i].trickrate < eDI_MEDIA_Speed_FF_X_1_10)) && (s_tStreamInfo[i].bSuccesiveSeekMode != TRUE))
				{
					tNexusError= NEXUS_Playback_GetStatus(s_tMediaInstance[i].playback, &pStatus);
					if(tNexusError != NEXUS_SUCCESS)
					{
						/* Nothing to do */
						VK_TASK_Sleep(10);
						continue;
					}

					nTempPlayTime[i] = pStatus.position;
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
						/* firstStarted Stream */
						if(s_tMediaInstance[i].bPtsInstalled == FALSE)
						{
							if(s_tEventFirstPtsCallback[i] != NULL)
								s_tEventFirstPtsCallback[i] ((HUINT32)i, unPtsSnapShot[i]);
						}
						/* else : we had got a first pts for this stream */
						/* Get Playback Play Position */
						nTempPlayTime[i] =	bmedia_pts2time(unPtsSnapShot[i] - s_tMediaInstance[i].unFirstPts, BMEDIA_TIME_SCALE_BASE);
					}
				}

				/********************************************************
				 * Checking configured endtime & Emitting the EOF Event.
				 *
				 * If the playingPosition reached a configured endtime
				 * ,an EOF signal should be emitted to upper layer.
				 * And then the mediaRouter or
				 * Player will enter a stopping status by user or application.
				 ********************************************************/
				if (s_ulMediaEndTime[i] != 0)
				{
					if(nTempPlayTime[i] >= s_ulMediaEndTime[i])
					{
						if ( NULL != s_tEventCallback[i] )
						{
							PrintError("%s(%d) : SENDING END OF TIME.... !!\n",__FUNCTION__,__LINE__);
							s_tEventCallback[i] ((HUINT32)i, DI_MEDIA_EVENT_ENDOFSTREAM, 0);
//							s_tMediaInstance[i].bEndOfStreamState = TRUE;
						}
						else
						{
							PrintError("%s(%d) : reached endtime (%d) but we don't have any callback !!\n",__FUNCTION__,__LINE__,s_ulMediaEndTime[i]);
						}
					}
				}

				/* The event of time changed shouldn't be emitted in eof status */
				if(s_tMediaInstance[i].bEndOfStreamState != TRUE)
				{
					if(s_nCurrentPlayTime[i] != (nTempPlayTime[i]/1000))
					{
						if((s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_NONE)
									|| ((nTempPlayTime[i] == 0) && (s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0)))
						{
							PrintDebug("[%s] Not Update Play Time\n", __func__);
						}
						else
						{
								s_nCurrentPlayTime[i] = (nTempPlayTime[i]/1000);
						}

						PrintDebug ("%s(%d) : CUR Time (%d)msec \n",__FUNCTION__,__LINE__,s_nCurrentPlayTime[i]*1000);

						if((s_nCurrentPlayTime[i] != 0)
								&& (s_tEventCallback[i] != NULL)
								&& (s_tMediaInstance[i].bSuccesiveSeeking != TRUE)
								&& ((s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_UPDATE_FULLTIME)
									|| (s_bPlayTimerMode[i] == DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0)))
						{
							s_tEventCallback[i](i, DI_MEDIA_EVENT_CHANGEPLAYTIME, s_nCurrentPlayTime[i]*1000);
						}
					}
				}
			}
		} /* end of for loop */
		VK_TASK_Sleep(100);
	} /* end of while */

	return;
}

static void P_MEDIA_SuccessiveSeekTask(void* param)
{
	DI_MEDIA_MSG						audMsg;
	HINT32								nRet=0;
	HINT32								nBaseSeekRate=0;
	HINT32								nAdaptiveSeekRate=0;
	HINT32								nCheckRate=0;
	HINT32								nCheckModifyCount=0;
	HINT8								bSeekOperation = FALSE;
	HUINT32								unMediaId=0;
	NEXUS_Error							eNexusError = NEXUS_SUCCESS;
    struct timeval						tCurrent_Time;
 	HUINT32								nCurrent_Time = 0;
 	HUINT32								nPrevious_Time = 0;
	HUINT32								unTimeout = 500;
 	HINT32								nRealSeekPosition = 0;
 	HINT32								nCurrentSeekPosition = 0;
 	HINT32								nPreviousSeekPosition=0;
 	HBOOL								bAdaptiveRate = FALSE;
 	HBOOL								bLateUpdateMode = FALSE;
 	DI_IPTUNER_DLNA_SEEKMODE_e			nSeekMode = DI_IPTUNER_DLNA_SEEKMODE_NONE;
 	NEXUS_PlaybackStatus				tPlaybackStatus;
	HUINT32								unSeekRetryCount=0;
/*	HUINT32								unErrorCount=0;*/

	BSTD_UNUSED(param);

	while(1)
	{
		nRet = VK_MSG_ReceiveTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), unTimeout);
		if(nRet != VK_OK)
		{
			if(nRet != VK_TIMEOUT)
			{
				PrintError ("%s (%d) Error>  Error VK_MSG_ReceiveTimeout\n",__FUNCTION__,__LINE__);
			}
		}
		else
		{
			switch( audMsg.ulCmmand)
			{
				case DI_MEDIA_SUCCESSIVE_SEEK_START:
					unMediaId = audMsg.ulParam2;
					nBaseSeekRate = audMsg.ulParam3;
					nSeekMode = audMsg.ulParam4;
					bAdaptiveRate = audMsg.ulParam5;
					nAdaptiveSeekRate = nBaseSeekRate;
					PrintDebug("[%s] [%d] DI_MEDIA_SUCCESSIVE_SEEK_START(Seek Rate = %d)\n", __FUNCTION__,__LINE__, nBaseSeekRate);
					if (VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) == VK_OK)
					{
						if ((s_tMediaInstance[unMediaId].bSessionStopping == TRUE) ||
							(s_tMediaInstance[unMediaId].playback == NULL))
						{
							PrintError("[%s] Error - Invalid playback status(%d)!\n", __func__, __LINE__, s_tMediaInstance[unMediaId].bSessionStopping);
							(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
							continue;
						}

						(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
						if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
						{
							(void)NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
							VK_TASK_Sleep(200);
						}

						nCurrentSeekPosition = (HINT32)(s_nCurrentPlayTime[unMediaId]+nBaseSeekRate);
						if(nCurrentSeekPosition < 0)
						{
							nCurrentSeekPosition = 0;
						}

						eNexusError = NEXUS_Playback_Seek (s_tMediaInstance[unMediaId].playback, nCurrentSeekPosition*1000);
						if(eNexusError != NEXUS_SUCCESS)
						{
							PrintError("[%s] Error - Fail NEXUS_Playback_Seek:%d\n", __func__, eNexusError);
						}
						else
						{
							(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
							if(tPlaybackStatus.state == NEXUS_PlaybackState_ePlaying)
							{
								VK_TASK_Sleep(200);
								(void)NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);
							}
						}

						if((nBaseSeekRate == 3) || (nBaseSeekRate == 5))
						{
							nBaseSeekRate--;
						}

						s_tMediaInstance[unMediaId].unSuccesiveSeekTime = (HUINT32)nCurrentSeekPosition;
						(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
					}
					nPrevious_Time = 0;
					bSeekOperation = TRUE;
					bLateUpdateMode = FALSE;
					unTimeout = 100;
					break;
				case DI_MEDIA_SUCCESSIVE_SEEK_STOP:
					PrintDebug("[%s] [%d] DI_MEDIA_SUCCESSIVE_SEEK_STOP\n", __FUNCTION__,__LINE__);
					nBaseSeekRate = 0;
					nAdaptiveSeekRate = 0;
					if (VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) == VK_OK)
					{

						if((s_tMediaInstance[unMediaId].vDecoder != NULL) && (bSeekOperation == TRUE))
						{
							NEXUS_VideoDecoder_Flush(s_tMediaInstance[unMediaId].vDecoder);
						}
						bSeekOperation = FALSE;
						nPrevious_Time = 0;
						unTimeout = 500;
						bAdaptiveRate = FALSE;
						bLateUpdateMode = FALSE;
						s_tMediaInstance[unMediaId].bSuccesiveSeeking = FALSE;
						(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
					}
					break;
				case DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE:
					PrintDebug("[%s] [%d] DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE\n", __FUNCTION__,__LINE__);
					if(bSeekOperation == TRUE)
					{
						*audMsg.pucParam = DI_MEDIA_SUCCESSIVE_SEEK_RUNNING;
					}
					else
					{

						*audMsg.pucParam = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
					}

					break;
				default:
					PrintError("[%s] [%d] Error> Invalid Command\n", __FUNCTION__,__LINE__);
					break;
			}
		}

		if(bSeekOperation == TRUE)
		{
			gettimeofday(&tCurrent_Time, NULL);

			if(nPrevious_Time != 0)
			{
				if (VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) == VK_OK)
				{
					if ((s_tMediaInstance[unMediaId].bSessionStopping == TRUE) ||
						(s_tMediaInstance[unMediaId].playback == NULL))
					{
						PrintError("[%s:%d] Error - Invalid playback status(%d)!\n", __func__, __LINE__, s_tMediaInstance[unMediaId].bSessionStopping);
						(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
						continue;
					}

					nCurrent_Time = (tCurrent_Time.tv_sec*1000) + (tCurrent_Time.tv_usec/1000);
					if(((nCurrent_Time - nPrevious_Time) > 1000) && (bLateUpdateMode != TRUE))
					{
						if((s_nCurrentPlayTime[unMediaId] > 0) && (s_tEventCallback[unMediaId] != NULL) &&
							((s_bPlayTimerMode[unMediaId] == DI_MEDIA_TIMERMODE_UPDATE_FULLTIME) || (s_bPlayTimerMode[unMediaId] == DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0)))
						{
							nRealSeekPosition = s_nCurrentPlayTime[unMediaId];

							if(nRealSeekPosition < 0)
							{
								s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
							}
							else
							{
								s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, nRealSeekPosition*1000);
							}
						}

						if((nAdaptiveSeekRate < 0) && ((nRealSeekPosition+nAdaptiveSeekRate-1) >= 0))
						{
							nCurrentSeekPosition = nRealSeekPosition+nAdaptiveSeekRate-1;
						}
						else if((nAdaptiveSeekRate > 0) && (nSeekMode == DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK) && ((nRealSeekPosition+nAdaptiveSeekRate-1) >= 0))
						{
							nCurrentSeekPosition = nRealSeekPosition+nAdaptiveSeekRate-1;
						}
						else if((nRealSeekPosition+nAdaptiveSeekRate) >= 0)
						{
							nCurrentSeekPosition = nRealSeekPosition+nAdaptiveSeekRate;
						}
						else
						{
							nCurrentSeekPosition = 0;
						}

						if(nCurrentSeekPosition >= ((s_tStreamInfo[unMediaId].duration)/1000))
						{
							nBaseSeekRate = 0;
							nAdaptiveSeekRate = 0;
							bSeekOperation = FALSE;
							bLateUpdateMode = FALSE;
							nPrevious_Time = 0;
							unTimeout = 500;

							if (s_tMediaInstance[unMediaId].playback != NULL)
							{
								(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
								if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
								{
									(void)NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
									VK_TASK_Sleep(100);
								}

								(void)NEXUS_Playback_Seek (s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].duration-1000);

								VK_TASK_Sleep(500);
								(void)NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);

								if(s_tMediaInstance[unMediaId].vDecoder != NULL)
								{
									NEXUS_VideoDecoder_Flush(s_tMediaInstance[unMediaId].vDecoder);
								}
								/* else - do nothing */

								if ( NULL != s_tEventCallback[unMediaId] )
								{
									s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ENDOFSTREAM, 0);
								}
								/* else - do nothing */
							}

							(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
							continue;
						}

						if(nCurrentSeekPosition <= 0)
						{
							nBaseSeekRate = 0;
							nAdaptiveSeekRate = 0;
							bSeekOperation = FALSE;
							bLateUpdateMode = FALSE;
							nPrevious_Time = 0;
							unTimeout = 500;
							(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
							if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
							{
								(void)NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
								VK_TASK_Sleep(100);
							}

							(void)NEXUS_Playback_Seek (s_tMediaInstance[unMediaId].playback, 0);

							VK_TASK_Sleep(200);
							(void)NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);

							if(s_tMediaInstance[unMediaId].vDecoder != NULL)
							{
								NEXUS_VideoDecoder_Flush(s_tMediaInstance[unMediaId].vDecoder);
							}
							/* else - do nothing */

							if ( NULL != s_tEventCallback[unMediaId] )
							{
								s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_BEGINOFSTREAM, 0);
							}
							/* else - do nothing */

							s_tMediaInstance[unMediaId].unSuccesiveSeekTime = 0;
							s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
							(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
							continue;
						}

						eNexusError = NEXUS_Playback_Seek (s_tMediaInstance[unMediaId].playback, nCurrentSeekPosition*1000);

						if(bAdaptiveRate == TRUE)
						{
							if((nCurrentSeekPosition != nPreviousSeekPosition+nBaseSeekRate) && (nCurrentSeekPosition > 0))
							{
								nCheckRate = (HINT32)(nCurrentSeekPosition - (nPreviousSeekPosition+nBaseSeekRate));
								if(nCheckRate > 0)
								{
									if(nCheckModifyCount == 0)
									{
										nCheckModifyCount=1;

									}
									else if(nCheckRate >0)
									{
										nCheckModifyCount++;
									}
									else
									{
										nCheckModifyCount=0;
									}

									if(nCheckModifyCount >= 3)
									{
										nAdaptiveSeekRate--;
										nCheckModifyCount = 0;
									}

//									PrintData("### [%s] nBaseSeekRate=%d, nAdaptiveSeekRate=%d ###\n",__func__, nBaseSeekRate, nAdaptiveSeekRate);
								}
								else
								{
									if(nCheckModifyCount == 0)
									{
										nCheckModifyCount=1;

									}
									else if(nCheckRate < 0)
									{
										nCheckModifyCount++;
									}
									else
									{
										nCheckModifyCount=0;
									}

									if(nCheckModifyCount <= 3)
									{
										nAdaptiveSeekRate++;
										nCheckModifyCount = 0;
									}
								}

//								PrintData("### [%s] nBaseSeekRate=%d, nAdaptiveSeekRate=%d ###\n",__func__, nBaseSeekRate, nAdaptiveSeekRate);
							}
							else
							{
								nCheckModifyCount=0;
							}
						}

						(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
						if(tPlaybackStatus.state == NEXUS_PlaybackState_ePlaying)
						{
							VK_TASK_Sleep(100);
							(void)NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);
						}

						s_tMediaInstance[unMediaId].unSuccesiveSeekTime = (HUINT32)nCurrentSeekPosition;
						nPreviousSeekPosition = nCurrentSeekPosition;
						nPrevious_Time = nCurrent_Time;
						if(eNexusError != NEXUS_SUCCESS)
						{
							PrintError("[%s] Error - Fail NEXUS_Playback_Seek:%d\n", __func__, eNexusError);
							if(unSeekRetryCount < 5)
							{
								unSeekRetryCount ++;
								VK_TASK_Sleep(100);
							}
							else
							{
								unSeekRetryCount = 0;
								if ( NULL != s_tEventCallback[unMediaId] )
								{
									s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
									VK_printf("##### EVENT_ERROROFSTREAM 1 #####\n");
								}
								/* else - do nothing */

								nBaseSeekRate = 0;
								nAdaptiveSeekRate = 0;
								bSeekOperation = FALSE;
								bLateUpdateMode = FALSE;
								nPrevious_Time = 0;
								unTimeout = 500;
							}
						}
					}
					else if((nCurrent_Time - nPrevious_Time) > 700)
					{
						(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
						VK_printf("### first = %d, last = %ld, duration = %ld ###\n", (tPlaybackStatus.first/1000), (tPlaybackStatus.last/1000), (s_tStreamInfo[unMediaId].duration/1000));
						/* Time Stamp가 1분 위하 차이에 대해서는 Error로 보지 않음. 실제 이렇게 녹화된 Content들이 많이 존재함. */
						if((tPlaybackStatus.last/1000) > ((s_tStreamInfo[unMediaId].duration)/1000)+60)
						{
							nCurrent_Time = (tCurrent_Time.tv_sec*1000) + (tCurrent_Time.tv_usec/1000);
							/* Wifi의 경우 속도가 느린 경우에 대비하여 30초이상 이동이 없을시 Error로 봄. */
							if((nCurrent_Time - nPrevious_Time) > 30000)
							{
								if ( NULL != s_tEventCallback[unMediaId] )
								{
									s_tEventCallback[unMediaId] ((HUINT32)unMediaId, DI_MEDIA_EVENT_ERROROFSTREAM, 0);
									VK_printf("##### EVENT_ERROROFSTREAM 2 #####\n");
								}
								/* else - do nothing */

								nBaseSeekRate = 0;
								nAdaptiveSeekRate = 0;
								bSeekOperation = FALSE;
								bLateUpdateMode = FALSE;
								nPrevious_Time = 0;
								unTimeout = 500;
								unSeekRetryCount = 0;
							}

							bLateUpdateMode = TRUE;
							(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
							continue;
						}
						else
						{
							if(bLateUpdateMode == TRUE)
							{
								VK_TASK_Sleep(300);
							}

							bLateUpdateMode = FALSE;
						}

						if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
						{
							(void)NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
							VK_TASK_Sleep(200);
						}
					}
					(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				}
			}
			else
			{
				nCurrent_Time = (tCurrent_Time.tv_sec*1000) + (tCurrent_Time.tv_usec/1000);
				nPrevious_Time = nCurrent_Time;
			}
		}
	}
}

void P_MEDIA_GetVideoBufferDepth(HUINT32 ulTunerId, HUINT32 *uiSize, HUINT32 *uiFifoSize)
{
	NEXUS_Error nResult;
	HUINT32 unMediaId=0;
	DI_ERR_CODE dResult;
	NEXUS_VideoDecoderStatus nStatus;

	*uiSize = *uiFifoSize = 0;
	dResult = P_MEDIA_FindMediaId(ulTunerId, &unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}

	if(s_tMediaInstance[unMediaId].vDecoder)
	{
		nResult = NEXUS_VideoDecoder_GetStatus (s_tMediaInstance[unMediaId].vDecoder,&nStatus);
		if(nResult == NEXUS_SUCCESS)
		{
			*uiSize = nStatus.fifoDepth;				/* size in bytes of the compressed buffer */
			*uiSize += nStatus.queueDepth;				/* the number of decoded pictures currently ready to be displayed */
			*uiSize += nStatus.cabacBinDepth;			/* depth in bytes of the cabac bin buffer */
			*uiSize += nStatus.enhancementFifoDepth;	/* depth in bytes of the enhancement compressed buffer */

			*uiFifoSize = nStatus.fifoSize;
		}
	}
}

void P_MEDIA_GetVideoBufferDepthCallback(HUINT32 ulTunerId, HUINT32 *uiSize, HUINT32 *uiFifoSize)
{
	HUINT32 unMediaId=0;
	DI_ERR_CODE dResult;

	if(uiSize == NULL)
	{
		PrintError("[%s][%d] Invalid Parameter, uiSize is NULL!\n", __FUNCTION__,__LINE__);
		return;
	}

	dResult = P_MEDIA_FindMediaId(ulTunerId, &unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}
	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return;
	}

	if(P_MEDIA_CheckValidMediaId(unMediaId) != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}

	(void)P_MEDIA_GetVideoBufferDepth(ulTunerId, uiSize, uiFifoSize);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	return;
}

void P_MEDIA_GetAudioBufferDepth(HUINT32 ulTunerId, HUINT32 *uiSize, HUINT32 *uiFifoSize)
{
	NEXUS_Error nResult;
	HUINT32 unMediaId=0;
	DI_ERR_CODE dResult;
	NEXUS_AudioDecoderStatus tDecoderStatus;

	*uiSize = *uiFifoSize = 0;
	dResult = P_MEDIA_FindMediaId(ulTunerId, &unMediaId);
	if (dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}

	if (s_tMediaInstance[unMediaId].aDecoder)
	{
		nResult = NEXUS_AudioDecoder_GetStatus (s_tMediaInstance[unMediaId].aDecoder,&tDecoderStatus);
		if(nResult == NEXUS_SUCCESS)
		{
			*uiSize = tDecoderStatus.fifoDepth; /* depth in bytes of the compressed buffer */
			*uiFifoSize = tDecoderStatus.fifoSize;
		}
	}
}

void P_MEDIA_GetAudioBufferDepthCallback(HUINT32 ulTunerId, HUINT32 *uiSize, HUINT32 *uiFifoSize)
{
	HUINT32 unMediaId=0;
	DI_ERR_CODE dResult;

	if(uiSize == NULL)
	{
		PrintError("[%s][%d] Invalid Parameter, uiSize is NULL!\n", __FUNCTION__,__LINE__);
		return;
	}

	dResult = P_MEDIA_FindMediaId(ulTunerId, &unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}
	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return;
	}

	if(P_MEDIA_CheckValidMediaId(unMediaId) != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}

	(void)P_MEDIA_GetAudioBufferDepth(ulTunerId, uiSize, uiFifoSize);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	return;
}

void P_MEDIA_GetPumpBufferDepth(HUINT32 ulTunerId, HUINT32 *uiSize, HUINT32 *uiFifoSize)
{
	HUINT32 unMediaId=0;
	DI_ERR_CODE dResult;
	NEXUS_PlaybackStatus tPlaybackStatus;

	if(uiSize == NULL)
	{
		PrintError("[%s][%d] Invalid Parameter, uiSize is NULL!\n", __FUNCTION__,__LINE__);
		return;
	}

	*uiSize = *uiFifoSize = 0;
	dResult = P_MEDIA_FindMediaId(ulTunerId, &unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}

	if(P_MEDIA_CheckValidMediaId(unMediaId) != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		return;
	}

	if((s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_PLAY) &&
		(s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_PAUSE))
	{
		PrintDebug("[%s] Play state is no STATE_PLAY\n", __FUNCTION__);
		*uiSize = 0;
		return;
	}

	if(s_tMediaInstance[unMediaId].playback!=NULL)
	{
		(void)NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
		*uiSize = tPlaybackStatus.fifoDepth;
		*uiFifoSize = tPlaybackStatus.fifoSize;
	}
	else
	{
		*uiSize = 0;
	}
	return;
}

void P_MEDIA_GetPumpBufferDepthCallback(HUINT32 ulTunerId, HUINT32 *uiSize, HUINT32 *uiFifoSize)
{
	HUINT32 unMediaId=0;
	DI_ERR_CODE dResult;

	if(uiSize == NULL)
	{
		PrintError("[%s][%d] Invalid Parameter, uiSize is NULL!\n", __FUNCTION__,__LINE__);
		return;
	}

	dResult = P_MEDIA_FindMediaId(ulTunerId, &unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("P_MEDIA_FindMediaId failed!\n");
		return;
	}
	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return;
	}

	if(P_MEDIA_CheckValidMediaId(unMediaId) != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return;
	}

	(void)P_MEDIA_GetPumpBufferDepth(ulTunerId, uiSize, uiFifoSize);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	return;
}

#if 0
void P_MEDIA_CBCheck(SESSION_HANDLE pHSession, OutputControl_t *outputcontrol)
{
	OutputControl_t stOPL;
	VK_MEM_Memcpy(&stOPL, outputcontrol, sizeof(OutputControl_t));

	PrintError("wCompressedDigitalVideo: %d\n", stOPL.data.playready.minOPL.wCompressedDigitalVideo);
	PrintError("wUncompressedDigitalVideo: %d\n", stOPL.data.playready.minOPL.wUncompressedDigitalVideo);
	PrintError("wAnalogVideo: %d\n", stOPL.data.playready.minOPL.wAnalogVideo);
	PrintError("wCompressedDigitalAudio: %d\n", stOPL.data.playready.minOPL.wCompressedDigitalAudio);
	PrintError("wUncompressedDigitalAudio: %d\n", stOPL.data.playready.minOPL.wUncompressedDigitalAudio);
}
#endif

#if __________task__________
#endif
#if __________drv__________
#endif

#define __________DRV_COMMON_DI_MEDIA__________
DRV_Error DRV_MEDIA_Init(void)
{
	int result;
	DI_ERR_CODE nRet = DI_ERR_OK;
	int i=0;
	HINT8 sem_name[128]={0};
	PrintEnter;

	VK_memset(&s_tStreamInfo,0,sizeof(s_tStreamInfo));
	VK_memset(s_bPlayTimerMode,0,sizeof(s_bPlayTimerMode));
	nRet = DI_IPTUNER_Init(NULL);
	if (DI_ERR_OK != nRet)
	{
		PrintError ("%s(%d) Error> DI_IPTUNER_POOL_Init Faild.\n",__FUNCTION__,__LINE__);
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
		PrintError("[DRV_MEDIA_Init] Error(0x%x) in VK_TASK_Create()\n", result);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMediaPlayTimeTaskId);

	result = VK_MSG_Create(DI_MEDIA_MSG_SIZE, sizeof(DI_MEDIA_MSG),
						"MediaSeekMsgQ", &s_ulMediaSuccessiveSeekMId, VK_SUSPENDTYPE_FIFO);
	if(result != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	result = VK_TASK_Create(P_MEDIA_SuccessiveSeekTask,
						MEDIA_SEEK_TASK_PRIORITY,
						MEDIA_SEEK_TASK_STACK_SIZE,
						"MediaSeekTask",
						NULL,
						&s_ulMediaSuccessiveSeekTId,
						FALSE);
	if(result != VK_OK)
	{
		PrintError("[DRV_MEDIA_Init] Error(0x%x) in VK_TASK_Create()\n", result);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulMediaSuccessiveSeekTId);

	VK_MEM_Memset(s_tMediaInstance, 0x0, sizeof(s_tMediaInstance));


	for(i = 0; i < DI_MEDIA_MAX_NUM; i++)
	{
		VK_memset(sem_name,0x00,sizeof(sem_name));
		VK_snprintf((char*)sem_name,sizeof(sem_name), "media_%d_resource_sema", i);
		result = VK_SEM_Create(&s_tMediaInstance[i].ulMediaSemId,sem_name, DEFAULT_SUSPEND_TYPE);
		if( result != VK_OK )
		{
			PrintError("[%s] [%d] VK_SEM_Create  Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
			return DRV_ERR;
		}
	}

	P_MEDIA_TSInjectorInit();

	(void)TLS_Openssl_Init();

/*************************
*	PlayReady
*************************/
#if defined (CONFIG_PLAYREADY)
#if defined (CONFIG_PLAYREADY_25)
	if(DI_FS_IsFileExist("/var/lib/humaxtv/drm/playready25/playready25.bin") != TRUE)
	{
		VK_SYSTEM_Command("mkdir -p /var/lib/humaxtv/drm/playready25");
		VK_SYSTEM_Command("rm -rf /var/lib/humaxtv/drm/playready25/*");
		VK_SYSTEM_Command("cp -a /usr/drm/playready25.bin /var/lib/humaxtv/drm/playready25/");
	}
#else
#if defined (CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	if(DI_FS_IsFileExist("/var/lib/humaxtv/drm/playready.bin") != TRUE)
	{
		VK_SYSTEM_Command("mkdir /var/lib/humaxtv/drm");
		VK_SYSTEM_Command("rm -rf /var/lib/humaxtv/drm/*");
		VK_SYSTEM_Command("cp -a /usr/drm/playready.bin /var/lib/humaxtv/drm/");
	}
#else
	if(DI_FS_IsFileExist("/var/lib/humaxtv/drm/bgroupcert.dat") != TRUE)
	{
		VK_SYSTEM_Command("mkdir /var/lib/humaxtv/drm");
		VK_SYSTEM_Command("rm -rf /var/lib/humaxtv/drm/*");
		VK_SYSTEM_Command("cp -a /usr/drm/*.dat /var/lib/humaxtv/drm/");
	}
#endif
#endif
#endif
	PrintExit;
	return DRV_OK;
}

/**
 * @brief		Stream의 Path를 이용하여 Probe를 한다.
 *
 * @param[in]	url 				Stream의 Path
 * @param[out]	stMediaFileInfo 	Probe된 Stream의 정보
 * @return		DI Error Code
 * @see			DI_MEDIA_PrintProbeInfo
 */
DI_ERR_CODE DRV_MEDIA_Probe (void *pHTuner, HUINT8 *pUrl, DI_MEDIA_INFO_t *stMediaFileInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	stMediaInfo  *pMfpInfo = NULL;
	DI_IPTUNER_t *pstTunerHdl = pHTuner;
//	HINT32 i=0;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
#if 0
	for (i=0;i<DI_MEDIA_MAX_NUM;i++)
	{
		if (!s_MfpInfo[i].opened)
		{
			pMfpInfo = &s_MfpInfo[i];
			break;
		}
		/* else { continue } */
	}
#endif
	pMfpInfo = VK_MEM_Alloc(sizeof(stMediaInfo));
	if (pMfpInfo == NULL)
	{
		nRet = DI_ERR_NO_RESOURCE;
	}
	else
	{
		VK_MEM_Memset(pMfpInfo,0x0,sizeof(stMediaInfo));
		nRet = P_MEDIA_ProbeOpen (pstTunerHdl, (HINT8 *)pUrl, pMfpInfo);
		if (nRet == DI_ERR_OK)
		{
			VK_memcpy(stMediaFileInfo, &pMfpInfo->StreamInfo, sizeof(DI_MEDIA_INFO_t));
			(void) P_MEDIA_ProbeClose (pMfpInfo);
		}
		else
		{
			PrintError ("%s(%d) Error> - Fail in P_MEDIA_ProbeOpen\n",__FUNCTION__,__LINE__);
		}
		VK_MEM_Free(pMfpInfo);
	}
	PrintExit;
	return nRet;
}

/**
 * @brief		parameter로 받은 Stream Info를 log로 Display한다.
 *
 * @param[in]	pMediaInfo	log로 Display하려는 Stream Info
 * @return		void
 * @see			DI_MEDIA_Probe
 */
void DRV_MEDIA_PrintProbeInfo (DI_MEDIA_INFO_t *pMediaInfo)
{
	HUINT32 i;

	PrintEnter;

	PrintData ("stream_type             (%d)\n",pMediaInfo->stream_type);
#if 0 /* for CANVAS */
	PrintData ("stream_size             (%"PRId64")\n",pMediaInfo->stream_size);
#endif
	PrintData ("max_bitrate             (%u)\n",pMediaInfo->max_bitrate);
	PrintData ("duration                (%u)\n",pMediaInfo->duration);
	PrintData ("nprograms               (%u)\n",pMediaInfo->nprograms);
	PrintData ("ntracks                 (%u)\n",pMediaInfo->ntracks);
	PrintData ("video_pes_id            (%u)\n",pMediaInfo->video_pes_id);
	PrintData ("video_stream_id         (%u)\n",pMediaInfo->video_stream_id);
	PrintData ("video_codec             (%u)\n",pMediaInfo->video_codec);
	PrintData ("audio_pes_id            (%u)\n",pMediaInfo->audio_pes_id);
	PrintData ("audio_stream_id         (%u)\n",pMediaInfo->audio_stream_id);
	PrintData ("audio_codec             (%u)\n",pMediaInfo->audio_codec);
	PrintData ("support_trick           (%u)\n",pMediaInfo->support_trick);
	PrintData ("support_seek            (%u)\n",pMediaInfo->support_seek);
	PrintData ("audiodecodertype        (%u)\n",pMediaInfo->audiodecodertype);
	PrintData ("videodecodertype        (%u)\n",pMediaInfo->videodecodertype);
	PrintData ("eStartCondition         (%u)\n",pMediaInfo->eStartCondition);
	PrintData ("ulStartTime             (%u)\n",pMediaInfo->ulStartTime);
	PrintData ("demuxId                 (%u)\n",pMediaInfo->demuxId);
	PrintData ("pcr_pid                 (%u)\n",pMediaInfo->pcr_pid);

	PrintData ("Total Video Track       (%u)\n",pMediaInfo->ucTotalVideoTrack);
	for(i=0;i<pMediaInfo->ucTotalVideoTrack;i++)
	{
		PrintData ("------------------------------------------------\n");
		PrintData ("Video Track Num     (%u)\n",i);
		PrintData ("video_pes_id        (%u)\n",pMediaInfo->stVideoTrack[i].video_pes_id);
		PrintData ("video_stream_id     (%u)\n",pMediaInfo->stVideoTrack[i].video_stream_id);
		PrintData ("video_codec         (%u)\n",pMediaInfo->stVideoTrack[i].video_codec);
	}

	PrintData ("Total audio track       (%u)\n",pMediaInfo->ucTotalAudioTrack);
	for(i=0;i<pMediaInfo->ucTotalAudioTrack;i++)
	{
		PrintData ("------------------------------------------------\n");
		PrintData ("audio track Num     (%u)\n",i);
		PrintData ("audio_pes_id        (%u)\n",pMediaInfo->stAudioTrack[i].audio_pes_id);
		PrintData ("audio_stream_id     (%u)\n",pMediaInfo->stAudioTrack[i].audio_stream_id);
		PrintData ("audio_codec	        (%u)\n",pMediaInfo->stAudioTrack[i].audio_codec);
		PrintData ("audio_language      [%s]\n",pMediaInfo->stAudioTrack[i].uacAudioLang);
	}

	PrintData ("------------------------------------------------\n");
	PrintData ("Total Xsub track        (%u)\n",pMediaInfo->ucTotalXsubTrack);
	for(i=0;i<pMediaInfo->ucTotalXsubTrack;i++)
	{
		PrintData ("------------------------------------------------\n");
		PrintData ("Divx Subtitle Track (%u)\n",i);
		PrintData ("Xsub Type           (%u)\n",pMediaInfo->stXsubTrack[i].eXsubType);
		PrintData ("Xsub Language       (%s)\n",pMediaInfo->stXsubTrack[i].uacXsubLang);
	}

	PrintExit;

	return;
}

#define __________DRV_NEW_DI_MEDIA__________
/**
 * @brief		Media 설정을 종료한다.
 *
 * @param		None
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_Init
 */
DRV_Error DRV_MEDIA_Term(void)
{
	int result;
	int i = 0;
	/** Media Time Event Task & Event */
	(void)VK_TASK_Stop(s_ulMediaPlayTimeTaskId);

   	result = VK_TASK_Destroy( s_ulMediaPlayTimeTaskId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	/** Media Seek Task & Event */
	(void)VK_TASK_Stop(s_ulMediaSuccessiveSeekTId);

   	result = VK_TASK_Destroy( s_ulMediaSuccessiveSeekTId);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
		return DRV_ERR;
	}

	(void)VK_MSG_Destroy(s_ulMediaSuccessiveSeekMId);

	for(i = 0; i < DI_MEDIA_MAX_NUM; i++)
	{
		result = VK_SEM_Release(s_tMediaInstance[i].ulMediaSemId);
		if( result != VK_OK )
		{
			PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
			return DRV_ERR;
		}
	}

	return DRV_OK;
}

/**
 * @brief		Open된 Stc Channel을 Media Type에 맞게 설정 후 얻는다.
 *
 * @param[in]	unMediaId	media Id
 * @param[in]	mediaType	media type
 * @return		NEXUS_StcChannelHandle
 * @see
 */
NEXUS_StcChannelHandle DRV_MEDIA_MP_GetPlaybackStcChannelHandle(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType, DI_MEDIA_PLAYPUMP mediaType)
{
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_PlaybackPidChannelSettings openPidSettings;
	DI_MEDIA_AUDIO_TYPE_e eTempAudioDecoderType = DI_MEDIA_AUDIO_DECODER_NONE;
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
	else if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] playback Handle is Null\n", __func__);
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

		/* pcr_pid */
		if( s_tMediaInstance[unMediaId].pcrPidChannel != NULL ) /* duplication check */
		{
			stcSettings.modeSettings.pcr.pidChannel = s_tMediaInstance[unMediaId].pcrPidChannel;
		}
#if 1
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
#else
		/**
			pcr pid 와 video/audio pid 가 동일한 경우는 제외함.
		    stcChannel 에 pidChannel Setting 시 Video/Audio PID와 PCR PID 가 동일 한경우
			nexus_playback.c 의 NEXUS_Playback_OpenPidChannel 에서 PID 중복 처리 코드 내에서
			play_pid->cfg.pidSettings.pidType = pSettings->pidSettings.pidType; (기존 코드) 가 아닌
			play_pid->cfg = *pSettings; (변경 코드) 로의 변경이 필요.
			현재 모든 BRCM Driver 반영에 어려움이 있으므로 Workaround 처리함.
		*/
		else if(s_tStreamInfo[unMediaId].pcr_pid != 0x1fff &&
		s_tStreamInfo[unMediaId].pcr_pid != s_tStreamInfo[unMediaId].video_stream_id &&
		s_tStreamInfo[unMediaId].pcr_pid != s_tStreamInfo[unMediaId].audio_stream_id)
    	{
	        stcSettings.modeSettings.pcr.pidChannel =
				NEXUS_Playback_OpenPidChannel (s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].pcr_pid, &openPidSettings);
			s_tMediaInstance[unMediaId].pcrPidChannel = stcSettings.modeSettings.pcr.pidChannel;
		}

#endif

#if 0	/* 현재 AudioMaster를 사용할 경우 문제가 된다고 하여 막음 */
		stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
#endif
		stcSettings.modeSettings.Auto.transportType = P_MEDIA_ConvertTransportTypeToNexus(s_tStreamInfo[unMediaId].stream_type);
		if(stcSettings.modeSettings.Auto.transportType == NEXUS_TransportType_eMp4Fragment) /* for SSTR */
		{
			stcSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
		}
		NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	}

	PrintExit;

	return DRV_DEMUX_GetStcChannelHandle();
}

/**
 * @brief 		media 재생을 위해 Video Pid Channel을 얻는다.
 *
 * @param[in]	unMediaId				media Id
 * @param[in]	eVideoCodec				Video Codec
 * @param[in]	VideoDecoderHandle		Video Decoder Handle
 * @return		NEXUS_PidChannelHandle
 * @see			DRV_MEDIA_GetPlaybackAudioPidChannel
 */
NEXUS_PidChannelHandle DRV_MEDIA_GetPlaybackVideoPidChannel(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_VIDEO_FORMAT eVideoCodec, void *VideoDecoderHandle)
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

		/** Nexus Pid Channel을 Open한다. */
		NEXUS_Playback_GetDefaultPidChannelSettings(&openPidSettings);
#if (NEXUS_VERSION >= 1250)
		openPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
		openPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
		openPidSettings.pidTypeSettings.video.codec = P_MEDIA_ConvertMediaVideoCodecToNexus(eVideoCodec); /*DRV_VIDEO_GetCodecType(0,eVideoCodec);*/
		openPidSettings.pidTypeSettings.video.decoder = (NEXUS_VideoDecoderHandle)VideoDecoderHandle;
		s_tMediaInstance[unMediaId].vDecoder = (NEXUS_VideoDecoderHandle)VideoDecoderHandle;
		openPidSettings.pidTypeSettings.video.index = true;
		s_tMediaInstance[unMediaId].videoPidChannel = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, s_tStreamInfo[unMediaId].video_stream_id, &openPidSettings);
	}
	PrintExit;
	return s_tMediaInstance[unMediaId].videoPidChannel;
}

/* TODO */
DRV_Error DRV_MEDIA_GetPlaybackSpeed (DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType, DI_MEDIA_PlaySpeed_e *peSpeed)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DRV_Error nError = DRV_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStatus 	tPlaybackStatus;
	HUINT32	unMediaId;
	HINT32 i=0;

	PrintEnter;

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
		PrintError("[%s] invalid eVideoDecoderType and eAudioDecoderType.\n", __func__);
		return DRV_ERR;
	}

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] invalid playback Handle\n", __func__);
		return DRV_ERR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DRV_ERR;
	}

	(void)VK_memset(&tPlaybackStatus, 0x0, sizeof(tPlaybackStatus));
	/** Get Playback Status */
	tNexusError = NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &tPlaybackStatus);
	if(tNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error - Fail NEXUS_Playback_GetStatus:%d\n", __func__, tNexusError);
		nError = DRV_ERR;
	}
	else
	{
		/** Get Trick Rate */
		if(tPlaybackStatus.state == NEXUS_PlaybackState_eTrickMode)
		{
			*peSpeed = s_tMediaInstance[unMediaId].trickrate;
		}
		else if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
		{
			*peSpeed = eDI_MEDIA_Speed_Pause;
		}
		else if(tPlaybackStatus.state == NEXUS_PlaybackState_ePlaying)
		{
			*peSpeed = eDI_MEDIA_Speed_Normal;
		}
		else
		{
			PrintError("[%s] Error - Not start Playback.\n", __func__);
			nError = DRV_ERR;
		}
	}

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	PrintExit;

	return nError;
}

/**
 * @brief		media 재생을 위해 Audio Pid Channel을 얻는다.
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
#if 0				/* Di audio module요청사항에 의해 사용하지 않음. secondary setting은 이후에 audio pid channel setting함수를 통해 Setting함. */
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

	PrintDebug ("%s(%d) unMediaId = %d\n",__FUNCTION__,__LINE__,i);
	if(i >= DI_MEDIA_MAX_NUM)
	{
		PrintError("[%s] invalid eAudioDecoderType.\n", __func__);
	}
	else
	{
		if(eAudioDecoderType == DI_MEDIA_AUDIO_DECODER_MAIN)
		{
			/** Nexus Pid Channel을 Open한다. */
			for(j = 0; j < s_tStreamInfo[unMediaId].ucTotalAudioTrack ; j++)
			{
				/*
				if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] != NULL)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[j]);
					s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NULL;
				}
				*/
				openPlaybackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
				openPlaybackPidSettings.pidSettings.pidTypeSettings.audio.codec
					= DRV_AUDIO_ConvertCodecToNexus(s_tStreamInfo[unMediaId].stAudioTrack[j].audio_codec);
				openPlaybackPidSettings.pidTypeSettings.audio.primary = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
				if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] == NULL)
				{
					if((eAudioCodec == DI_MEDIA_AUDIO_FORMAT_PCMWAVE) && (s_tStreamInfo[unMediaId].video_codec == DI_VIDEO_FORMAT_NONE))
					{
						s_tMediaInstance[unMediaId].audioTrackPidChannels[j]
							= NEXUS_Playpump_OpenPidChannel(
									s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]
									, s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id
									, &openPlaypumpPidSettings);
					}
					else
					{
						s_tMediaInstance[unMediaId].audioTrackPidChannels[j]
							= NEXUS_Playback_OpenPidChannel(
									s_tMediaInstance[unMediaId].playback
									, s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id
									, &openPlaybackPidSettings);
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
			/** Nexus Pid Channel을 Open한다. */
			for(j = 0; j < s_tStreamInfo[unMediaId].ucTotalAudioTrack ; j++)
			{
				NEXUS_Playback_GetDefaultPidChannelSettings(&openPlaybackPidSettings);
				/*if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] != NULL)
				{
					(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[j]);
					s_tMediaInstance[unMediaId].audioTrackPidChannels[j] = NULL;
				}*/
				openPlaybackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
				openPlaybackPidSettings.pidSettings.pidTypeSettings.audio.codec
					= DRV_AUDIO_ConvertCodecToNexus(s_tStreamInfo[unMediaId].stAudioTrack[j].audio_codec);
				openPlaybackPidSettings.pidTypeSettings.audio.primary = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
				if(s_tMediaInstance[unMediaId].audioTrackPidChannels[j] == NULL)
				{
					s_tMediaInstance[unMediaId].audioTrackPidChannels[j]
						= NEXUS_Playback_OpenPidChannel(
								s_tMediaInstance[unMediaId].playback
								, s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id
								, &openPlaybackPidSettings);
				}
				if(s_tStreamInfo[unMediaId].audio_stream_id == s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id)
				{
					audioPidChannel = s_tMediaInstance[unMediaId].audioSubPidChannel = s_tMediaInstance[unMediaId].audioTrackPidChannels[j];
				}
			}
		}
		else
		{
			/** Open된 Pid Channel에 secondary Audio Handle을 추가한다. */
			for(j = 0; j < s_tStreamInfo[unMediaId].ucTotalAudioTrack ; j++)
			{
				if(s_tStreamInfo[unMediaId].audio_stream_id == s_tStreamInfo[unMediaId].stAudioTrack[j].audio_stream_id)
				{
#if 0				/* Di audio module요청사항에 의해 사용하지 않음. secondary setting은 이후에 audio pid channel setting함수를 통해 Setting함. */
					nNexusError = NEXUS_Playback_GetPidChannelSettings(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[j], &openPlaybackPidSettings);
					if(nNexusError != NEXUS_SUCCESS)
					{
						PrintError("[%s] Error > Fail NEXUS_Playback_GetPidChannelSettings.\n", __func__);
						PrintExit;
						return NULL;
					}
					openPlaybackPidSettings.pidTypeSettings.audio.secondary = (NEXUS_AudioDecoderHandle)AudioDecoderHandle;
					nNexusError = NEXUS_Playback_SetPidChannelSettings(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].audioTrackPidChannels[j], &openPlaybackPidSettings);
					if(nNexusError != NEXUS_SUCCESS)
					{
						PrintError("[%s] Error > Fail NEXUS_Playback_SetPidChannelSettings.\n", __func__);
						PrintExit;
						return NULL;
					}
#endif
					audioPidChannel = s_tMediaInstance[unMediaId].audioTrackPidChannels[j];
					break;
				}
			}
		}
	}
	PrintExit;
	return audioPidChannel;
}

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
 * @brief		Demux에서 TS에 대한 PAT, PMT Parsing을 위해 Playback Pid Channel을 Open한다.
 *
 * @param[in]	nDemuxId				Demux Id
 * @param[in]	nPid					Pid Number
 * @param[in]	nChannelType			Channel Type
 * @param[out]	phPidChannel			Pid Channel Handle
 * @return		DRV_Error
 * @see			DRV_MEDIA_ClosePlaybackPidChannel
 */
#if 0 /* for debugging on hd5500t model : junsy*/
DRV_Error DRV_MEDIA_OpenPlaybackPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	NEXUS_PlaybackPidChannelSettings pidSettings;
	HUINT32 unMediaId = 0;
	HUINT32 i =0;

	PrintEnter;

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
					/** Nexus Pid Channel을 Open한다. */
					NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
					pidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
					s_tMediaInstance[unMediaId].sectionPidChannel = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, nPid, &pidSettings);
					*phPidChannel = s_tMediaInstance[unMediaId].sectionPidChannel;
				}
		}
	}

	PrintExit;

	return nDrvRet;
}
#else
DRV_Error DRV_MEDIA_OpenPlaybackPidChannel(HINT32 nDemuxId, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	NEXUS_PlaybackPidChannelSettings pidSettings;
	NEXUS_Error	neResult;
	HUINT32 unMediaId = 0;
	HUINT32 i =0;

	PrintEnter;

	PrintDebug("%s(%d) Info> nDemuxId(%d) , nChannelType(%d) , nPid(0x%x)\n",__FUNCTION__,__LINE__,nDemuxId, nChannelType, nPid);

#if 0   /* Support SECTION Filtering */
	/* The Media module doesn't support the SECTION Filtering */
	if(nChannelType == DI_DEMUX_CHANNELTYPE_SECTION)
	{
		PrintError("[%s] Not Support Setion Filtering in MEDIA Interface!\n\r",__FUNCTION__);
		PrintExit;
		return DRV_ERR;
	}
#endif

	for(i = 0; i<DI_MEDIA_MAX_NUM ; i++)
	{
		PrintDebug ("%s(%d) s_tStreamInfo[%d].demuxId = %d\n",__FUNCTION__,__LINE__,i,s_tStreamInfo[i].demuxId );
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
#if 0   /* Support SECTION Filtering */
		/* validation checking */
		if(s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_READY)
		{
			PrintError("[%s] Media doesn't ready.)\n", __func__);
			PrintExit;
			return DRV_ERR;
		}
#endif
		NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
		switch(nChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO:
				if(!s_tMediaInstance[unMediaId].videoPidChannel)
				{
					pidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
#if (NEXUS_VERSION >= 1250)
					pidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
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
#if (NEXUS_VERSION >= 1250)
					pidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
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
			case DI_DEMUX_CHANNELTYPE_SECTION:
				if (s_tMediaInstance[unMediaId].playback)
				{
					/** Nexus Pid Channel을 Open한다. */
					NEXUS_Playback_GetDefaultPidChannelSettings(&pidSettings);
					pidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
					s_tMediaInstance[unMediaId].sectionPidChannel = NEXUS_Playback_OpenPidChannel(s_tMediaInstance[unMediaId].playback, nPid, &pidSettings);
					*phPidChannel = s_tMediaInstance[unMediaId].sectionPidChannel;
				}
				else
				{
					PrintError("[%s] Error> Media playback has invalid handle!\n\r",__FUNCTION__);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_PCR:
					*phPidChannel = DRV_DEMUX_GetPcrPidChannel();
					return DRV_OK;
			default:
					/* audio/video PID 와 동일한 경우 설정이 바뀌지 않도록 함 */
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
#endif

/**
 * @brief		Demux에서 TS에 대한 PAT, PMT Parsing을 위해 Open한 Playback Pid Channel을 Close한다.
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
					/** Nexus Pid Channel을 Close한다. */
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

DRV_Error DRV_MEDIA_OpenPlaypumpPidChannel(void *pPumpHandle, DI_DEMUX_CHANNELTYPE nChannelType, HINT32 nPid, NEXUS_PidChannelHandle *phPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	NEXUS_PlaypumpHandle hPlaypump;
	NEXUS_PlaypumpOpenPidChannelSettings pidSettings;

	PrintEnter;

	PrintDebug("%s(%d) Info> nChannelType(%d) , nPid(0x%x)\n",__FUNCTION__,__LINE__, nChannelType, nPid);

	hPlaypump = (NEXUS_PlaypumpHandle)pPumpHandle;

	NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
	switch(nChannelType)
	{
		case DI_DEMUX_CHANNELTYPE_VIDEO:
			{
				pidSettings.pidType = NEXUS_PidType_eVideo;
#if (NEXUS_VERSION >= 1250)
				pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
			}
			break;
		case DI_DEMUX_CHANNELTYPE_AUDIO:
		case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF:
			{
#if (NEXUS_VERSION >= 1250)
				pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
				pidSettings.pidType = NEXUS_PidType_eAudio;
				pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
			}
			break;
		default:
			{
				/** Nexus Pid Channel을 Open한다. */
				NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pidSettings);
				pidSettings.pidType = NEXUS_PidType_eOther;
			}
			break;
	}
	*phPidChannel = NEXUS_Playpump_OpenPidChannel(hPlaypump, nPid, &pidSettings);

	PrintExit;

	return nDrvRet;
}

DRV_Error DRV_MEDIA_ClosePlaypumpPidChannel(void *pPumpHandle, NEXUS_PidChannelHandle hPidChannel)
{
	DRV_Error nDrvRet = DRV_OK;
	NEXUS_PlaypumpHandle hPlaypump;

	PrintEnter;

	hPlaypump = (NEXUS_PlaypumpHandle)pPumpHandle;
	NEXUS_Playpump_ClosePidChannel(hPlaypump, hPidChannel);

	PrintExit;

	return nDrvRet;
}

HBOOL DRV_MEDIA_CheckStart(void)
{
	return s_ulMediaStart==0 ? FALSE : TRUE;
}

#if __________di__________
#endif

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*
 * DI_MEDIA usage (for PAL_MEDIA ).
 *
 * 1. Simple Downloading
 *   - DI_MEDIA_CreateSession			: Create tuner by URL.
 *   - DI_MEDIA_RegisterBufferingEventCallback: Register Buffering Event Callbacks for buf-information.
 *   - DI_MEDIA_StartBuffering	: start buffering task
 *   - DI_MEDIA_GetBufferingStatus		: get buffering progress information
 *   - if(EOF) Thread destroied automatically
 *   - DI_MEDIA_UnregisterBufferingEventCallback: Unregister Buffering Event Callbacks
 *   - DI_MEDIA_DestroySession			: After complete event occurred, release iptuner.
 *
 * 2. Streamed Content Playing.
 *   - DI_MEDIA_CreateSession
 *   - DI_MEDIA_ProbeSession
 *   - DI_MEDIA_OpenSession -> (internal : DI_IPTUNER_StreamOpen)
 *   - DI_MEDIA_RegisterStreamEventCallback
 *   - DI_MEDIA_SetupSession
 *   - DI_MEDIA_StartSession
 *   - DI_MEDIA_SetPosition/SetSpeed
 *   - DI_MEDIA_StopSession
 *   - DI_MEDIA_ReleaseSession
 *   - DI_MEDIA_UnregisterStreamEventCallback
 *   - DI_MEDIA_CloseSession -> (internal : DI_IPTUNER_StreamClose)
 *   - DI_MEDIA_DestroySession
 *
 * 3. Progressive Download Playing.
 *   - DI_MEDIA_CreateSession
 *   - DI_MEDIA_ProbeSession
 *   - DI_MEDIA_RegisterBufferingEventCallback
 *   - DI_MEDIA_StartBuffering -> (internal : DI_IPTUNER_DnpCreate)
 *   - DI_MEDIA_GetBufferingStatus	: get buffering progress information
 *   - DI_MEDIA_OpenSession -> (internal : DI_IPTUNER_DnpOpen)
 *   - DI_MEDIA_RegisterStreamEventCallback
 *   - DI_MEDIA_SetupSession
 *   - DI_MEDIA_StartSession
 *   - DI_MEDIA_SetPosition/SetSpeed : Playback Seek & SetSpeed
 *   - DI_MEDIA_StopSession
 *   - DI_MEDIA_ReleaseSession
 *   - DI_MEDIA_UnregisterStreamEventCallback
 *   - DI_MEDIA_CloseSession -> (internal : DI_IPTUNER_DnpClose)
 *   - DI_MEDIA_StopBuffering -> (internal : DI_IPTUNER_DnpDestroy)
 *   - DI_MEDIA_UnregisterBufferingEventCallback
 *   - DI_MEDIA_DestroySession
 *
 * 4. Multicast Live Channel Tuning(not Playing)
 *   - DI_MEDIA_CreateSession
 *   - DI_MEDIA_RegisterChannelEventCallback
 *   - DI_MEDIA_OpenSession -> (internal : DI_IPTUNER_MulticastOpen)
 *   - DI_MEDIA_CloseSession -> (internal : DI_IPTUNER_MulticastClose)
 *   - DI_MEDIA_UnregisterChannelEventCallback
 *   - DI_MEDIA_DestroySession
 */

/* System 시작시 한번 호출되며 Session Pool 을 생성한다. */
DI_ERR_CODE DI_MEDIA_Init ( DI_MEDIA_CONFIG_t stConfig )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* DI_IPTUNER_Init ( DI_IPTUNER_CONFIG_t *pConfig) */
	nRet = DI_IPTUNER_Init((DI_IPTUNER_CONFIG_t *)&stConfig);

	P_MEDIA_TSInjectorInit();

	PrintExit;
	return nRet;
}

/* Media Player의 Default Configuration 값을 반환한다. */
DI_ERR_CODE DI_MEDIA_GetDefaultConfiguration( DI_MEDIA_CONFIG_t *pstConfig )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;
	BSTD_UNUSED(pstConfig);
	PrintExit;
	return nRet;
}

/* System 종료시 한번 호출되어 생성되어 있는 모든 Session Pool 및 List를 삭제한다. */
DI_ERR_CODE DI_MEDIA_Uninit (void)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;
	nRet = DI_IPTUNER_Uninit();
	PrintExit;
	return nRet;
}

/* Initialized DTCP  */
#define ENCRYPTED_DTCPKEY_LEN		(8 + 348 + 4 + 8)
DI_ERR_CODE DI_MEDIA_DTCP_Init ( void )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HINT8	*pcBuffer=NULL;
	MXDTCP_CALLBACK tDtcpCallback;
	int dtcp_result = 0;

	PrintEnter;

#if defined (CONFIG_JAPAN_SPEC)
	/* SRM 이 없으면, Rootfs에 있는 기본 SRM을 복사 한다.  */
	if(DI_FS_IsFileExist("/var/lib/humaxtv/dtcp.srm") != TRUE)
	{
		if(DI_FS_IsFileExist("/usr/dlna/dtcp.srm") == TRUE)
		{
			VK_SYSTEM_Command("cp /usr/dlna/dtcp.srm /var/lib/humaxtv/");
		}
	}
#endif

	mxLog_init(MX_LEVEL_NONE, NULL, NULL);
	mxLog_setLevel(MX_LEVEL_NONE);

	pcBuffer = VK_MEM_Alloc(ENCRYPTED_DTCPKEY_LEN);
	if(pcBuffer != NULL)
	{
		VK_MEM_Memset(pcBuffer, 0x0, ENCRYPTED_DTCPKEY_LEN);
		dtcp_result = DRV_NVRAM_GetField(DI_NVRAM_FIELD_DTCP_KEY, 0, pcBuffer, ENCRYPTED_DTCPKEY_LEN);
		if (dtcp_result != DRV_OK)
		{
			PrintError("Can't read DTCP Key from NAND flash\n");
 		}
		/* if dtcp key is encrypted, make sure to decrypt */
		{
			if( (pcBuffer[0] == 'D') && (pcBuffer[1] == 'T') && (pcBuffer[2] == 'C') && (pcBuffer[3] == 'P') &&
				(pcBuffer[4] == '-') && (pcBuffer[5] == 'I') && (pcBuffer[6] == 'P') && (pcBuffer[7] == ':') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-8] == ':') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-7] == 'D') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-6] == 'T') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-5] == 'C') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-4] == 'P') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-3] == '-') &&
				(pcBuffer[ENCRYPTED_DTCPKEY_LEN-2] == 'I') && (pcBuffer[ENCRYPTED_DTCPKEY_LEN-1] == 'P'))

			{
				DRV_Error	drvErr = 0;
				stCrypt_t	CryptInfo;

				unsigned long crc = 0;
				unsigned long org_crc = 0;
				unsigned char * crc_temp = NULL;

				VK_MEM_Memset((void *)&CryptInfo, 0, sizeof(stCrypt_t));
				CryptInfo.etCryptMode = DI_CRYPT_TDES_DTCPIP;
				CryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
				CryptInfo.etCryptData = DI_CRYPT_BLOCK;
				CryptInfo.pKey = NULL;

				CryptInfo.ulKeySize = 16;
				CryptInfo.pSrcBuffer = DRV_CRYPT_MemAlloc(348 + 4);
				if (CryptInfo.pSrcBuffer == NULL)
				{
					PrintError("Error DI_CRYPT_MemAlloc pSrcBuffer NULL\n");
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				VK_MEM_Memcpy((void *)CryptInfo.pSrcBuffer, (void *)(pcBuffer+8), 352);

				CryptInfo.pDstBuffer = CryptInfo.pSrcBuffer;
				if (CryptInfo.pDstBuffer == NULL)
				{
					PrintError("Error DI_CRYPT_MemAlloc pDstBuffer NULL\n");
					DRV_CRYPT_MemFree(CryptInfo.pSrcBuffer);
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				CryptInfo.ulBuffSize = 352;

				drvErr = DRV_CRYPT_Decrypt(&CryptInfo);
				if (drvErr != DRV_OK)
				{
					PrintError("Error DI_CRYPT_Encrypt\n");
					DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				crc = DRV_UTIL_GetCrc32(CryptInfo.pDstBuffer, 348);
				org_crc = htonl(crc);
				crc_temp = (unsigned char *)&org_crc;

				PrintData("[%02x %02x %02x %02x == %02x %02x %02x %02x]\n", crc_temp[0], crc_temp[1], crc_temp[2], crc_temp[3],
					CryptInfo.pDstBuffer[348 + 0], CryptInfo.pDstBuffer[348 + 1], CryptInfo.pDstBuffer[348 + 2], CryptInfo.pDstBuffer[348 + 3]);

				if(VK_memcmp(crc_temp, CryptInfo.pDstBuffer + 348, 4))
				{
					PrintError("CRC did not match.");
					DRV_CRYPT_MemFree(CryptInfo.pSrcBuffer);
					VK_MEM_Free(pcBuffer);
					return DI_ERR_ERROR;
				}

				VK_MEM_Memset(pcBuffer, 0x0, ENCRYPTED_DTCPKEY_LEN);
				VK_MEM_Memcpy(pcBuffer, (void *)CryptInfo.pSrcBuffer, 348);

				DRV_CRYPT_MemFree(CryptInfo.pSrcBuffer);
			}
			else
			{
				PrintData("Encryption Header not found\n");
			}
		}

		dtcp_result = mxDtcp_SetSecureData((char *)pcBuffer, 348);
		PrintData("### mxDtcp_SetSecureData result=%d ###\n", dtcp_result);
		VK_MEM_Free(pcBuffer);

		if(dtcp_result == MXDTCP_SUCCESS)
		{
			dtcp_result = DtcpAppLib_Startup();
			if(dtcp_result == MXDTCP_SUCCESS)
			{
				PrintData("### DtcpAppLib_Startup result=%d ###\n", dtcp_result);

				tDtcpCallback.fnMemAlloc = P_MEDIA_CRYPT_MemAlloc;
				tDtcpCallback.fnMemFree = P_MEDIA_CRYPT_MemFree;
				tDtcpCallback.fnEncrypt = P_MEDIA_AES_Encrypt;
				tDtcpCallback.fnDecrypt = P_MEDIA_AES_Decrypt;
				tDtcpCallback.fnEncryptAesKey = NULL;
				(void)mxDtcp_SetCallback(&tDtcpCallback);

				s_bDtcpEnable = TRUE;
			}
			else
			{
				s_bDtcpEnable = FALSE;
			}
		}
		else
		{
			s_bDtcpEnable = FALSE;
		}
	}

	PrintExit;

	return nRet;
}

/* Deinitialized DTCP  */
void DI_MEDIA_DTCP_Term ( void )
{
	PrintEnter;

	(void)DtcpAppLib_Shutdown();
	s_bDtcpEnable = FALSE;
	(void)mxLog_deinit();

	PrintExit;

	return;
}

/* 하나의 Session 을 생성한다.
 * 생성된 Session 은 고유의 Handle 을 가지며 Session Pool 에 등록된다.
 * 입력되는 URL 을 통해 Session 의 type이 결정된다. (default : streaming 방식)
 */
DI_ERR_CODE DI_MEDIA_CreateSession (HINT8 *pURL, SESSION_HANDLE *pHSession)
{
	HUINT32 ulRet = ERR_OK;
	/*HUINT8	aucSemName1[32], aucSemName2[32];*/
	DRV_Error dResult;
	DI_MEDIA_INFO_t *ptStreamInfo = NULL;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	if(pURL == NULL)
	{
		PrintError(" [%s] Uri is NULL \n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	ulRet = DI_IPTUNER_Create(pURL, &pstTunerHdl); //Check for YouView by ykkim
	if(ulRet != ERR_OK)
	{
		PrintError(" [%s] DI_IPTUNER_Create fail\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	*pHSession = pstTunerHdl;

/*************************
*	DRM Handle should be created before SetupSession,
*	Because PAL would register OPL callback before Setup with this DRM session!
*************************/
	ptStreamInfo = &(pstTunerHdl->tMediaInfo);

	dResult = DRV_DRM_CreateSession(*pHSession);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_DRM_CreateSession failed!\n");
		PrintExit;
		return DI_ERR_ERROR;
	}

	PrintExit;
	return ulRet;
}

/* 생성된 Session Handle을 입력받아 해당 Session 을 삭제한 후 Session Pool 에서 삭제한다.*/
DI_ERR_CODE DI_MEDIA_DestroySession (SESSION_HANDLE pHSession)
{
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	else
	{
		/* PAL layer 에서 DI_MEDIA_StartBuffering 진입과 동시에
		 * Destroy routine 호출이 일어나는 경우
		 * StopBuffering 으로 resource clean up 시킴. (단 tuner type 이 download type 인경우만
		 */
		if (pstTunerHdl->eDownloadType == DI_IPTUNER_DOWNLOADING)
		{
			(void) DI_IPTUNER_StopBuffering (pstTunerHdl);
		}
#if 0 /* DI_IPTUNER_ProbeCancel 은 Probe 종료를 보장한다. */
		 DI_IPTUNER_Close/Destroy 는 ProbeCancel 이 종료된 후 서로 배타적으로 동작하도록
		 ClosSema 를 잡아 동시호출을 방지
	   */
		if(pstTunerHdl->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Current State is probe cancel",__FUNCTION__,__LINE__);
			PrintExit;
			return DI_ERR_ERROR;
		}
#endif

		if(pstTunerHdl->tDtcpIpSetting.bDtcpIp == TRUE)
		{
			if(pstTunerHdl->tDtcpIpSetting.AkeHandle != NULL)
			{
				(void)DtcpAppLib_CloseAke(pstTunerHdl->tDtcpIpSetting.AkeHandle);
				pstTunerHdl->tDtcpIpSetting.AkeHandle = NULL;

				if(pstTunerHdl->tDtcpIpSetting.pcDTCP1HOST != NULL)
				{
					DD_MEM_Free(pstTunerHdl->tDtcpIpSetting.pcDTCP1HOST);
					pstTunerHdl->tDtcpIpSetting.pcDTCP1HOST = NULL;
				}

				if(pstTunerHdl->tDtcpIpSetting.pcDTCP1PORT != NULL)
				{
					DD_MEM_Free(pstTunerHdl->tDtcpIpSetting.pcDTCP1PORT);
					pstTunerHdl->tDtcpIpSetting.pcDTCP1PORT = NULL;
				}
			}
		}

		(void) DI_IPTUNER_Destroy(pstTunerHdl);
	}

	/*************************
	*	Destroy DRM Session
	*************************/
	DRV_DRM_DestroySession(pHSession);

	g_NeedPRLicensURL = FALSE;

	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 Session Handle 을 입력받아 Session Type 에 맞게
 * Playback 아래에 VFIO 를 Install 한다.
 * Install 되는 VFIO 는 Playback 이 전용으로 사용하게 되며
 * index 파일까지 같이 Open 되는 경우에 대해 고려되어야 한다.
 * Multicast는 VFIO intall 대신TS Injector 구동 및
 * IGMP Join 동작을 수행한다.
 */
DI_ERR_CODE DI_MEDIA_OpenSession (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	HUINT32 uTSInjID = 0;

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById!!Handle : %d\n", __FUNCTION__,(HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	/* For multicast, start TS injector thread */
	if (pstTunerHdl->eDownloadType == DI_IPTUNER_MULTICAST)
	{
		DI_IPTUNER_TSInjectorCallback_t tTSInjectorCallbacks;

		if (s_bInitTSInjector == FALSE)
		{
			PrintError("[%s] Error - Unintialized TS injector\n", __FUNCTION__);
			P_MEDIA_TSInjectorInit();
		}

		eDIError = P_MEDIA_TSInjectorGetID(&uTSInjID);
		if ((eDIError != DI_ERR_OK)
			|| (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM))
		{
			PrintError("[%s] Error - Fail P_MEDIA_TSInjectorGetID(err:%d, id:%d)\n",
						__FUNCTION__, eDIError, uTSInjID);
			return eDIError;
		}

		eDIError = P_MEDIA_TSInjectorOpen(uTSInjID, pstTunerHdl->pUrl);
		if (eDIError != DI_ERR_OK)
		{
			PrintError("[%s] Error - Fail P_MEDIA_TSInjectorOpen(%d)\n", __FUNCTION__, eDIError);
			s_tMediaTSInjector[uTSInjID].bRunning = FALSE;
			return eDIError;
		}

		/* Update di_media params */
		s_tMediaTSInjector[uTSInjID].bRunning = TRUE;

		/* Update di_iptuner params */
		pstTunerHdl->uTSInjID = uTSInjID;
		pstTunerHdl->bUseTSInjInstance = TRUE;
		pstTunerHdl->bUseTSInjTask = FALSE;

		/* Decide the multicast thread: TSInjThread(di_media) or PktRecvThread(di_iptuner_mcast) */
		/* Currently, PktRecvThread in di_iptuner_mcast is only supported */
		if (pstTunerHdl->bUseTSInjTask == TRUE)
		{
			/*
			 * TS injector task will request TS data to di_iptuner
			 */
			PrintError("[%s] Error - Currently, cant support TS Injector thread\n", __FUNCTION__);
			s_tMediaTSInjector[uTSInjID].bRunning = FALSE;
			return eDIError;
		}
		else
		{
			/*
			 * Data will be forwared from di_iptuner to di_media.
			 * So, register TSInjector data callback
			 * - Internal callback between di_media and di_iptuner
			 */
			tTSInjectorCallbacks.cbDataReceived  = P_MEDIA_TSInjectorDataReceivedCallback;
			eDIError = DI_IPTUNER_RegisterTSInjectorCB (pstTunerHdl, &tTSInjectorCallbacks);
			if (eDIError != DI_ERR_OK)
			{
				PrintError("[%s] Error - Can't register TS injector callback(err:%d)\n", __FUNCTION__, eDIError);
				s_tMediaTSInjector[uTSInjID].bRunning = FALSE;
				return eDIError;
			}
		}

		eDIError = P_MEDIA_TSInjectorStart(uTSInjID);
		if (eDIError != DI_ERR_OK)
		{
			PrintError("[%s] Error - Fail P_MEDIA_TSInjectorStart(%d)\n", __FUNCTION__, eDIError);
			s_tMediaTSInjector[uTSInjID].bRunning = FALSE;
			return eDIError;
		}
	}
	else
	{
		pstTunerHdl->uTSInjID = _DEF_MEDIA_INVALID_INJ_NUM;
		pstTunerHdl->bUseTSInjInstance = FALSE;
	}

	eDIError = DI_IPTUNER_Open(pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_Open\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return eDIError;

}

/*
 * 생성된 Session Handle 을 입력받아 해당 Session을 Close 한다.
 * 이때 Install 되었던 VFIO Set 이 삭제되어야 한다.
 * Multicast는 VFIO unintall 대신 IGMP Leave 동작을 수행한다.
 */
DI_ERR_CODE DI_MEDIA_CloseSession (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE	eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	else
	{
#if 0 /* DI_IPTUNER_ProbeCancel 은 Probe 종료를 보장한다. */
		 DI_IPTUNER_Close/Destroy 는 ProbeCancel 이 종료된 후 서로 배타적으로 동작하도록
		 ClosSema 를 잡아 동시호출을 방지
	   */
		if(pstTunerHdl->bProbeCancel == TRUE)
		{
			PrintError ("%s(%d) Error>  Current State is probe cancel",__FUNCTION__,__LINE__);
			PrintExit;
			return DI_ERR_ERROR;
		}
#endif
		eDIError = DI_IPTUNER_Close (pstTunerHdl);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_IPTUNER_Close\n", __FUNCTION__);
			return DI_ERR_ERROR;
		}

		if (pstTunerHdl->eDownloadType == DI_IPTUNER_MULTICAST)
		{
			(void)P_MEDIA_TSInjectorStop(pstTunerHdl->uTSInjID);
			(void)DI_IPTUNER_UnregisterTSInjectorCB(pstTunerHdl);
			(void)P_MEDIA_TSInjectorClose(pstTunerHdl->uTSInjID);
		}
	}
	PrintExit;
	return eDIError;
}

/*
 * Session Play 를 위한 정보를 추출한다. 추출된 정보는 Session에 귀속된다.
 * Probe Event Callback Function 이 Install 되어 있는 경우 FAIL/COMPLETE event 를 emit 한다.
 */
DI_ERR_CODE DI_MEDIA_ProbeSession (SESSION_HANDLE pHSession)
{
	HUINT32 ulRet = ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	ulRet = DI_IPTUNER_Probe ( pstTunerHdl );
	if(ulRet != ERR_OK)
	{
		PrintError("[%s] Probe Fail\n", __FUNCTION__);
		PrintExit;
		return ulRet;
	}

	PrintExit;
	return ulRet;
}

DI_ERR_CODE DI_MEDIA_ProbeSessionComplete (SESSION_HANDLE pHSession)
{
	HUINT32 ulRet = ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DRV_Error dResult;
	DRM_Info_t drm_info;

	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(g_NeedPRLicensURL == TRUE)
	{
		if(pstTunerHdl->tMediaInfo.eDrmType == DI_MEDIA_DRM_TYPE_WMDRM
			|| pstTunerHdl->tMediaInfo.eDrmType == DI_MEDIA_DRM_TYPE_PLAYREADY)
		{
			/* url 이 존재하는지 확인 후 처리 */
			while(1)
			{
				dResult = DRV_DRM_GetInfo(pHSession, &drm_info);
				if(dResult != DRV_OK)
				{
					PrintError("DRV_DRM_GetInfo failed!\n");
					return DRV_ERR;
				}

				if(drm_info.drm.playready.licenseURL != NULL)
				{
					break;
				}
				else
				{
					VK_TASK_Sleep(100);
				}
			}
		}
	}

	PrintExit;
	return ulRet;
}


/*
 * Probing 동작을 중지 시킨다.
 */
DI_ERR_CODE DI_MEDIA_ProbeCancel (SESSION_HANDLE pHSession)
{
	HUINT32 ulRet = ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById!!Handle : %d\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	ulRet = DI_IPTUNER_ProbeCancel ( pstTunerHdl );
	if(ulRet != ERR_OK)
	{
		PrintError("[%s] Probe Fail\n", __FUNCTION__);
		PrintExit;
		return ulRet;
	}
	PrintExit;
	return ulRet;

}

/*
 * Session에 저장된 Probe 정보를 반환한다.
 */
DI_ERR_CODE DI_MEDIA_GetProbeInfo (SESSION_HANDLE pHSession, DI_MEDIA_INFO_t *pstProbeInfo )
{
	DI_IPTUNER_t 		*pstTunerHdl = NULL;
	/*DI_MEDIA_INFO_t		*ptStreamInfo = NULL;*/
	PrintEnter;
	if(pHSession == NULL)
	{
		PrintError("[%s] Error - pHSession is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	VK_MEM_Memcpy(pstProbeInfo, &pstTunerHdl->tMediaInfo, sizeof(DI_MEDIA_INFO_t));
//	pstProbeInfo = &pstTunerHdl->tMediaInfo;
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Resource를 할당하고 연결한다. Session 부터 Decoder (Demux) Path 가 설정된다.
 * Probe가 된 상태여야 한다.
 */
DI_ERR_CODE DI_MEDIA_SetupSession (SESSION_HANDLE pHSession, HUINT32 *punMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	NEXUS_Error		tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackSettings playbackSettings;
	HUINT32 		unMediaId = 0;
#if defined(SUPPORT_DYNAMIC_AV_RESOURCE)
	HUINT32			stcChannel = 0;
	HUINT32			playpumpChannel = 0;
#endif
	HINT8			*pucStreamUrl = NULL;
	HUINT8			*pucURI_Data = NULL;
	HUINT8			*pucURI_Index = NULL;
	HINT32			nMediaCount=0;
	DI_IPTUNER_t 		*pstTunerHdl = NULL;
	DI_MEDIA_INFO_t		*ptStreamInfo = NULL;
	DI_IPTUNER_PlayerControlCallback_t tPlayerControlCallbacks;
	DRV_Error dResult;
	DRM_Info_t drm_info;
	DRM_Key_Info_t drm_key_info;

	PrintEnter;

	VK_MEM_Memset(&drm_info,0x00,sizeof(DRM_Info_t));

	if((punMediaId == NULL) || (pHSession == NULL))
	{
		PrintError("[%s] Error - punMediaId or pHSession is null\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	ptStreamInfo = &pstTunerHdl->tMediaInfo;

	if(pstTunerHdl->eDownloadType == DI_IPTUNER_DOWNLOADING)
	{
		pucStreamUrl = pstTunerHdl->pSaveFileName;
	}
	else
	{
		pucStreamUrl = pstTunerHdl->pUrl;
	}

	if(ptStreamInfo->ucTotalVideoTrack>0)
	{
		for(nMediaCount=0;nMediaCount<ptStreamInfo->ucTotalVideoTrack;nMediaCount++)
		{
			if(ptStreamInfo->stVideoTrack[nMediaCount].video_codec != DI_VIDEO_FORMAT_NONE)
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

	/***************************************
	*	Playback Setting.
	****************************************/
	eDIError = P_MEDIA_GetMediaId(&unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Fail P_MEDIA_GetMediaId(%d)\n", __func__, eDIError);
		return eDIError;
	}

	/*********************************************
	 * Protection for each session.
	 *********************************************/
	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed (%u)!\n",__FUNCTION__,__LINE__,unMediaId);
		s_tMediaInstance[unMediaId].valid = FALSE;
		return DI_ERR_ERROR;
	}

#if defined(SUPPORT_DYNAMIC_AV_RESOURCE)
	/******************************************
	 * ALLOCATION : stc channel
	 *****************************************/
	eDIError = DRV_VIDEO_AcquireResources(&stcChannel, &playpumpChannel);
	if( eDIError != DRV_OK )
	{
		PrintError(" Can't get AV recource!!\n");
		s_tMediaInstance[unMediaId].valid = FALSE;
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_NO_RESOURCE;
	}
	DRV_DEMUX_SetStcChannelHandle(stcChannel);
#endif

	VK_MEM_Memset(&s_tStreamInfo[unMediaId], 0x0, sizeof(DI_MEDIA_INFO_t));
	//VK_MEM_Memset(s_tMediaInstance[unMediaId].mediaurl, 0x0, DI_MEDIA_STREAM_URL_LEN);
	s_tMediaInstance[unMediaId].mediastate=DI_MEDIA_STATE_NO_SETUP;
	s_tMediaInstance[unMediaId].mediaType=DI_MEDIA_PLAYPUMP_DIVX;
	s_tMediaInstance[unMediaId].audioStatus=DI_MEDIA_AUDIO_FREE;
	s_tMediaInstance[unMediaId].audioPlayback=NULL;
	s_tMediaInstance[unMediaId].playback=NULL;
	s_tMediaInstance[unMediaId].audioPidChannel=NULL;
	s_tMediaInstance[unMediaId].videoPidChannel=NULL;
	s_tMediaInstance[unMediaId].sectionPidChannel=NULL;
	s_tMediaInstance[unMediaId].file=NULL;
	s_tMediaInstance[unMediaId].trickrate=eDI_MEDIA_Speed_Pause;
	s_tMediaInstance[unMediaId].bSessionStopping = FALSE;

	VK_memcpy(&s_tStreamInfo[unMediaId], ptStreamInfo, sizeof(DI_MEDIA_INFO_t));

	s_tMediaInstance[unMediaId].mediaurl = DD_MEM_Alloc(VK_strlen(pucStreamUrl)+1);
	if(s_tMediaInstance[unMediaId].mediaurl == NULL)
	{
		PrintError("%s(%d) Out Of Memory!!\n", __FUNCTION__, __LINE__);
		s_tMediaInstance[unMediaId].valid = FALSE;
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_NO_RESOURCE;
	}
	VK_memset(s_tMediaInstance[unMediaId].mediaurl, 0x00, VK_strlen(pucStreamUrl)+1);
	VK_memcpy(s_tMediaInstance[unMediaId].mediaurl, pucStreamUrl, VK_strlen(pucStreamUrl));

	*punMediaId = unMediaId;

	/*********************************
	 * ALLOCATION : nexus_playback
	 *********************************/
	eDIError = P_MEDIA_CreatePlayback(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Fail P_MEDIA_GetMediaId(%d)\n", __func__, eDIError);
		s_tMediaInstance[unMediaId].valid = FALSE;
		DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
		s_tMediaInstance[unMediaId].mediaurl = NULL;
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
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
		s_tMediaInstance[unMediaId].valid = FALSE;
		DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
		s_tMediaInstance[unMediaId].mediaurl = NULL;
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}
	/** set Decoder for test */
	s_tStreamInfo[unMediaId].audiodecodertype = DI_MEDIA_AUDIO_DECODER_MAIN;
	s_tStreamInfo[unMediaId].videodecodertype = DI_MEDIA_VIDEO_DECODER_MAIN;

	/** Open File Posix */

	/*********************************
	 * ALLOCATION : nexus_file_posix
	 *              pucURI_Data
	 *              pucURI_Index
	 ********************************/
	if((NULL != VK_strstr((char *)pucStreamUrl, "http://"))
		|| (NULL != VK_strstr((char *)pucStreamUrl, "https://"))
		|| (NULL != VK_strstr((char *)pucStreamUrl, "mms://"))
		|| (NULL != VK_strstr((char *)pucStreamUrl, "rtsp://"))
		|| (NULL != VK_strstr((char *)pucStreamUrl, "file://"))
		|| (NULL != VK_strstr((char *)pucStreamUrl, "dvr://"))
		) /* TODO : junsy */
	{

		/* DNP Player looks like Local file Play */
		if ((NULL != VK_strstr((char*)pucStreamUrl,"file://")) || (NULL != VK_strstr((char*)pucStreamUrl,"dvr://")))
		{
			if(s_tStreamInfo[unMediaId].stream_type != DI_MEDIA_MPEG_TYPE_ASF)
			{
				PrintData("IPTUNER (DNP) Play(%s)\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucStreamUrl, (char *)pucStreamUrl);
			}
			else
			{
				PrintData("IPTUNER (DNP-progressive down) Play(%s)\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucStreamUrl, (char *)pucStreamUrl); /* progressive down */
			}
		}
		/* DLNA Player looks like Local file Play */
		else if (NULL != VK_strstr((char*)pucStreamUrl,"[DLNA]"))
		{
			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF) || (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES))
			{
				/* URL에 DATA 및 INDEX를 붙여 File Open시 어느것을 Open하는지 구분함 */
				/* 붙인 String에 대해 DLNA File Library 사용하는 Module에서 제거하여 사용함 */
				pucURI_Data = DD_MEM_Alloc((VK_strlen(pucStreamUrl)+8));
				if(pucURI_Data == NULL)
				{
					PrintError("[%s] Error - Memory Allocation Error (pucURI_Data)\n", __func__);
					(void)P_MEDIA_DeletePlayback(unMediaId);
					s_tMediaInstance[unMediaId].valid = FALSE;
					DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
					s_tMediaInstance[unMediaId].mediaurl = NULL;
					(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
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
				/* URL에 DATA 및 INDEX를 붙여 File Open시 어느것을 Open하는지 구분함 */
				/* 붙인 String에 대해 DLNA File Library 사용하는 Module에서 제거하여 사용함 */
				pucURI_Data = DD_MEM_Alloc((VK_strlen(pucStreamUrl)+8));
				if(pucURI_Data == NULL)
				{
					PrintError("[%s] Error - Memory Allocation Error (pucURI_Data)\n", __func__);
					(void)P_MEDIA_DeletePlayback(unMediaId);
					s_tMediaInstance[unMediaId].valid = FALSE;
					DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
					s_tMediaInstance[unMediaId].mediaurl = NULL;
					(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
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
					s_tMediaInstance[unMediaId].valid = FALSE;
					DD_MEM_Free(pucURI_Data);
					pucURI_Data = NULL;
					DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
					s_tMediaInstance[unMediaId].mediaurl = NULL;
					(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
					return DI_ERR_ERROR;
				}
				else
				{
					VK_memset(pucURI_Index, 0x0, VK_strlen(pucStreamUrl)+8);
					VK_snprintf((char *)pucURI_Index, VK_strlen(pucStreamUrl)+8, "[DLNA][INDEX]%s", (char *)pucStreamUrl+6);
				}

				PrintData("IPTUNER (DLNA) Play(%s)\n", pucURI_Data);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucURI_Data, (char *)pucURI_Index);

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
		/* Streaming Playback */
		else
		{
			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS) ||
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192) ||
#ifndef CONFIG_DLNA_CERTIFICATION
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES) ||
#endif
				(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT) || /* for SSTR */
				((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_ASF)&&(NULL != VK_strstr((char *)pucStreamUrl, "mms://"))))
			{
				/* chunked live streaming, also here without index param */
				PrintData("IPTUNER (Streaming) Play(%s) only Data\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)pucStreamUrl, (char *)NULL);
			}
			else
			{
				PrintData("IPTUNER (Streaming) Play(%s) with Index\n", pucStreamUrl);
				s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)pucStreamUrl, (char *)pucStreamUrl);
			}
		}
	}
	else
	{
		PrintData("Local File Play(%s)\n", pucStreamUrl);

		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_PES)
		{
			s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix ((char *)pucStreamUrl, NULL);
		}
		else
		{
			s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)pucStreamUrl, (char *)pucStreamUrl);
		}
	}

	/****************************************
	 * ALLOCATION : nexus_playpump, stcchnnel
	 ****************************************/
	if (!s_tMediaInstance[unMediaId].file)
	{
		PrintError("[%s] Error - can't open file:%s\n", __func__, pucStreamUrl);
		eDIError = DI_ERR_ERROR;
		(void)P_MEDIA_DeletePlayback(unMediaId);
		DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
		s_tMediaInstance[unMediaId].mediaurl = NULL;
		s_tMediaInstance[unMediaId].valid = FALSE;
	}
	else
	{
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

		if(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] == NULL)
		{
			PrintError("[%s][%d] DRV_PVR_GetPlaypumpHandle failed!\n", __FUNCTION__,__LINE__);
			goto ERROR_RET;
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

		playbackSettings.startPaused = true;
		playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
		playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePause;

		if((s_tStreamInfo[unMediaId].video_stream_id == 0) && (s_tStreamInfo[unMediaId].audio_stream_id != 0)
			&& (s_tStreamInfo[unMediaId].audio_codec == DI_AUDFORMAT_MP3))
		{
			playbackSettings.playpumpSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
			playbackSettings.enableStreamProcessing = TRUE;
		}
		else
		{
			playbackSettings.playpumpSettings.mode = NEXUS_PlaybackMode_eIndexed;
		}

		if(pstTunerHdl->tDlnaSetting.bDlna == TRUE)
		{
			/* 서버가 Disconnect되면서 문제되는 경우를 대비함. */
			playbackSettings.ioTimeout = 60 * 1000; /* 1 min */
		}

		playbackSettings.endOfStreamCallback.callback = P_MEDIA_EndOfStreamCallback;
		playbackSettings.endOfStreamCallback.context = NULL;
		playbackSettings.endOfStreamCallback.param= unMediaId;
		playbackSettings.beginningOfStreamCallback.callback = P_MEDIA_BeginOfStreamCallback;
		playbackSettings.beginningOfStreamCallback.context = NULL;
		playbackSettings.beginningOfStreamCallback.param= unMediaId;
		playbackSettings.errorCallback.callback = P_MEDIA_ErrorOfStreamCallback;
		playbackSettings.errorCallback.context = NULL;
		playbackSettings.errorCallback.param= unMediaId;
		tNexusError = NEXUS_Playback_SetSettings(s_tMediaInstance[unMediaId].playback, &playbackSettings);
		if(NEXUS_SUCCESS != tNexusError)
		{
			PrintError("[%s] Error - Fail NEXUS_PlaybackSetSetting:%d\n", __func__,tNexusError);
			(void)NEXUS_FilePlay_Close(s_tMediaInstance[unMediaId].file);
			(void)P_MEDIA_DeletePlayback(unMediaId);
			s_tMediaInstance[unMediaId].valid = FALSE;
			DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
			s_tMediaInstance[unMediaId].mediaurl = NULL;
			eDIError = DI_ERR_ERROR;
	/* else { continue; } */
		}
		else
		{
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
				s_tMediaInstance[unMediaId].valid = FALSE;
				DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
				s_tMediaInstance[unMediaId].mediaurl = NULL;
				eDIError = DI_ERR_ERROR;
				/* else { continue; } */
			}
			else
			{
				s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_READY;
				s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_UPDATE_FULLTIME;
			}
		}

		/* VIDEO Setting for Smooth Streaming */
		if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT)
		{
			DRV_VIDEO_SetSmoothStreaming(DI_VIDEO_DECODER_MAIN, TRUE); // TODO: Decoder Setting 확인
		}

/*************************
*	PlayReady License Acquisition
*************************/
		if(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_NONE)
		{
		}
		else if((ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_WMDRM)||(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_PLAYREADY))
		{
			dResult = DRV_DRM_GetInfo(pHSession, &drm_info);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_GetInfo failed!\n");
				goto ERROR_RET;
			}

			drm_info.drm.playready.pstPlaypump = s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX];
			drm_info.drm.playready.pucWrmHeader = ptStreamInfo->pucWrmHeader;
			drm_info.drm.playready.nWrmHeaderLen = ptStreamInfo->nWrmHeaderLen;
			drm_info.drm_type = ptStreamInfo->eDrmType;

			dResult = DRV_DRM_ResigterCallback(pHSession, &drm_info);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_ResigterCallback failed!\n");
				goto ERROR_RET;
			}

			dResult = DRV_DRM_GetLicence(pHSession);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_GetLicence failed!\n");
				goto ERROR_RET;
			}
		}
		else if(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_VIEWRIGHTWEB)
		{
		}
		else if(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_MARLIN)
		{
			drm_key_info.offer.marlin.pstPlayback = (void*)s_tMediaInstance[unMediaId].playback;
			drm_key_info.offer.marlin.video_stream_id = s_tStreamInfo[unMediaId].video_stream_id;
			drm_key_info.offer.marlin.audio_stream_id = s_tStreamInfo[unMediaId].audio_stream_id;

			drm_key_info.offer.marlin.videoPidChannel = (void**)&s_tMediaInstance[unMediaId].videoPidChannel;
			drm_key_info.offer.marlin.audioPidChannel = (void**)&s_tMediaInstance[unMediaId].audioPidChannel;

			drm_key_info.offer.marlin.cmd = DI_DRM_MARLIN_SETUP;
			dResult = DRV_DRM_GetKey(pHSession, &drm_key_info);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_GetKey failed!\n");
				goto ERROR_RET;
			}
		}
		else
		{
			PrintError("Invalid DRM Type!\n");
			goto ERROR_RET;
		}
	}

	s_tMediaInstance[unMediaId].pHSession = pHSession;

	/***************************************
	* YV Specific Setting. ( MUST Remove this Comment)
	****************************************/

	/* Okay!! Resources are assigned to Session for playing the media.*/

	/******************************************
	 * Setup Playing-Region
	 ******************************************/
	/* update a endtime to playback timer thread.*/
	eDIError = P_MEDIA_SetEndTime (unMediaId, pstTunerHdl->ulEndTime);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Fail DI_MEDIA_MP_SetEndTime\n", __FUNCTION__);
	}
#if 1
	/*********************************************************
	 * Setting Buffering EndTime
	 *
	 * When a buffering position reaches this timeoffset,
	 * should emit to media-router the buf-complete signal
	 * immediately after stopping buffer thread.
	 *********************************************************/
	eDIError = DI_IPTUNER_SetEndTime (pstTunerHdl, pstTunerHdl->ulEndTime);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Fail DI_IPTUNER_SetEndTime\n", __FUNCTION__);
	}
#endif

	/******************************************
	 * Register PlayerControlCallbacks
	 ******************************************/
	tPlayerControlCallbacks.cbPause  = P_MEDIA_PlayerPauseCallback;
	tPlayerControlCallbacks.cbResume = P_MEDIA_PlayerResumeCallback;

	(void) DI_IPTUNER_RegisterPlayerControlCB (pstTunerHdl, &tPlayerControlCallbacks);
	(void) DI_IPTUNER_RegisterDelayedSeekCB (pstTunerHdl, (DI_IPTUNER_DelayedSeekCallback)P_MEDIA_DelayedSeekCallback);

	if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS
			|| s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_VOB
			|| s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192)
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

	/******************************************
	 * Update Session State
	 ******************************************/
//	pstSession->eState = PAL_MEDIA_STATE_READY;
//	pstSession->eEof = PAL_MEDIA_DEFAULT; /* for endbyte */

	/* s_ulMediaStart for DI_AUDIO, DI_VIDEO : DRV_MEDIA_CheckStart() */
	++s_ulMediaStart;

	/* SUCCESS Return */
	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	PrintExit;

	return eDIError;

ERROR_RET:
	if(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] != NULL)
	{
		DRV_PVR_ReleasePlaypumpHandle(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
		s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] = NULL;
	}

	(void)P_MEDIA_DeletePlayback(unMediaId);
	s_tMediaInstance[unMediaId].valid = FALSE;
	if(pucURI_Data != NULL)
	{
		DD_MEM_Free(pucURI_Data);
		pucURI_Data = NULL;
	}

	DD_MEM_Free(s_tMediaInstance[unMediaId].mediaurl);
	s_tMediaInstance[unMediaId].mediaurl = NULL;
	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	return DI_ERR_ERROR;
}

/*
 * Resource를 해제한다.
 */
DI_ERR_CODE DI_MEDIA_ReleaseSession (SESSION_HANDLE pHSession, HUINT32 unMediaId)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	NEXUS_StcChannelSettings stcSettings;
	DI_IPTUNER_t 		*pstTunerHdl = NULL;
	// HUINT32 unMediaId = DI_MEDIA_MAX_NUM;

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
/*
	unMediaId = pstTunerHdl->unMediaId;
*/
	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
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
				(void)NEXUS_Playback_ClosePidChannel(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].pcrPidChannel);
			}

			s_tMediaInstance[unMediaId].pcrPidChannel = NULL;

			NEXUS_StcChannel_GetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
			stcSettings.timebase = NEXUS_Timebase_e0;
			stcSettings.mode = NEXUS_StcChannelMode_eAuto;
			stcSettings.modeSettings.pcr.pidChannel = NULL;
			NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);

			(void)NEXUS_FilePlay_Close(s_tMediaInstance[unMediaId].file);

			(void)P_MEDIA_DeletePlayback(unMediaId);

#if defined(SUPPORT_DYNAMIC_AV_RESOURCE)
			DRV_VIDEO_ReleaseResources();
#endif
#if 1 /* DRAGON */
	{
   		NEXUS_PlaypumpSettings pumpSets;
		NEXUS_Playpump_GetSettings(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &pumpSets);
		pumpSets.transportType = NEXUS_TransportType_eTs;
		(void)NEXUS_Playpump_SetSettings(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX], &pumpSets);
	}
#endif

			DRV_PVR_ReleasePlaypumpHandle(s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX]);
			s_tMediaInstance[unMediaId].playpump[DI_MEDIA_PLAYPUMP_DIVX] = NULL;

			if(s_tEventCallback[unMediaId] != NULL)
			{
				s_tEventCallback[unMediaId] = NULL;
			}

			/* VIDEO Setting for Smooth Streaming */
			if(s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT)
			{
				DRV_VIDEO_SetSmoothStreaming(DI_VIDEO_DECODER_MAIN, FALSE); // TODO: Decoder Setting 확인
			}
			(void)P_MEDIA_ReleaseMediaInstance(unMediaId);
			//VK_MEM_Memset(&s_tMediaInstance[unMediaId], 0x0, sizeof(DI_MediaInstance));
			VK_MEM_Memset(&s_tStreamInfo[unMediaId], 0x0, sizeof(DI_MEDIA_INFO_t));
		}
	}

	if(pstTunerHdl->tMediaInfo.eDrmType == DI_MEDIA_DRM_TYPE_MARLIN)
	{
		/* MARLIN DRM Filter Release */
		DRM_Key_Info_t drm_key_info;
		DRV_Error dResult;

		VK_memset(&drm_key_info, 0x00, sizeof(DRM_Key_Info_t));

		drm_key_info.offer.marlin.cmd = DI_DRM_MARLIN_RELEASE;

		dResult = DRV_DRM_GetKey(pHSession, &drm_key_info);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_DRM_GetKey failed!\n");
			eDIError = DI_ERR_ERROR;
		}
	}

	if( VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK )
	{
		PrintError("[%s] VK_SEM_Release failed\n\r",__FUNCTION__);
		eDIError = DI_ERR_ERROR;
	}

	s_bPlayTimerMode[unMediaId] = DI_MEDIA_TIMERMODE_NONE;

	(void) DI_IPTUNER_UnregisterPlayerControlCB(pHSession);
	(void) DI_IPTUNER_UnregisterDelayedSeekCB (pHSession);

	PrintExit;

	return eDIError;
}

DI_ERR_CODE DI_MEDIA_SetAudioTrack(SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT8 ucAudioTrack)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl->tMediaInfo.audio_pes_id = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_pes_id;
	pstTunerHdl->tMediaInfo.audio_stream_id = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_stream_id;
	pstTunerHdl->tMediaInfo.audio_codec = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_codec;

	s_tStreamInfo[unMediaId].audio_pes_id = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_pes_id;
	s_tStreamInfo[unMediaId].audio_stream_id = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_stream_id;
	s_tStreamInfo[unMediaId].audio_codec = s_tStreamInfo[unMediaId].stAudioTrack[ucAudioTrack].audio_codec;

	PrintExit;
	return eDIError;
}

DI_ERR_CODE DI_MEDIA_SetVideoTrack(SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT8 ucVideoTrack)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl->tMediaInfo.video_pes_id = s_tStreamInfo[unMediaId].stVideoTrack[ucVideoTrack].video_pes_id;
	pstTunerHdl->tMediaInfo.video_stream_id = s_tStreamInfo[unMediaId].stVideoTrack[ucVideoTrack].video_stream_id;
	pstTunerHdl->tMediaInfo.video_codec = s_tStreamInfo[unMediaId].stVideoTrack[ucVideoTrack].video_codec;

	s_tStreamInfo[unMediaId].video_pes_id = s_tStreamInfo[unMediaId].stVideoTrack[ucVideoTrack].video_pes_id;
	s_tStreamInfo[unMediaId].video_stream_id = s_tStreamInfo[unMediaId].stVideoTrack[ucVideoTrack].video_stream_id;
	s_tStreamInfo[unMediaId].video_codec = s_tStreamInfo[unMediaId].stVideoTrack[ucVideoTrack].video_codec;

	PrintExit;
	return eDIError;
}

/**
 * @brief 			PMT Update 시 반영한 DI_MEDIA_INFO_t 구조체 내용의 일부를 s_tStreamInfo 전역 변수에 적용한다.
 *
 * @param[in]		unMediaId				Media Device ID
 * @param[in]		pStreamInfo				Media Info (DI_MEDIA_INFO_t 구조체)
 * @return			DI_ERR_CODE
 * @see
 * @remark			만약 A/V Track이 PMT Update 후 2개에서 1개로 줄어들면, 지워져야하는 트랙의 정보가 초기화되지는 않지만,
 *					Total Track 개수에서 걸러주므로 문제는 없다. 하지만 추후 초기화에 대해서 생각을 할 필요가 있다.
 */
DI_ERR_CODE DI_MEDIA_UpdatePmtInfo(SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_INFO_t *pStreamInfo)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	HUINT32 idx = 0;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	if(pStreamInfo == NULL)
	{
		PrintError("[%s] pStreamInfo is NULL\n", __FUNCTION__);
		eDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		eDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		eDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		eDiRet = DI_ERR_ERROR;
		goto func_error;
	}

	eDiRet = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __FUNCTION__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		goto func_error;
	}

	pstTunerHdl->tMediaInfo.pcr_pid = pStreamInfo->pcr_pid;

	pstTunerHdl->tMediaInfo.ucTotalVideoTrack = pStreamInfo->ucTotalVideoTrack;
	for(idx = 0; idx < pstTunerHdl->tMediaInfo.ucTotalVideoTrack; idx++)
	{
		pstTunerHdl->tMediaInfo.stVideoTrack[idx].video_stream_id = pStreamInfo->stVideoTrack[idx].video_stream_id;
		pstTunerHdl->tMediaInfo.stVideoTrack[idx].video_codec = pStreamInfo->stVideoTrack[idx].video_codec;
	}

	pstTunerHdl->tMediaInfo.ucTotalAudioTrack = pStreamInfo->ucTotalAudioTrack;
	for(idx = 0; idx < pstTunerHdl->tMediaInfo.ucTotalAudioTrack; idx++)
	{
		pstTunerHdl->tMediaInfo.stAudioTrack[idx].audio_stream_id = pStreamInfo->stAudioTrack[idx].audio_stream_id;
		pstTunerHdl->tMediaInfo.stAudioTrack[idx].audio_codec = pStreamInfo->stAudioTrack[idx].audio_codec;
		VK_strncpy(pstTunerHdl->tMediaInfo.stAudioTrack[idx].uacAudioLang, pStreamInfo->stAudioTrack[idx].uacAudioLang, 256);
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

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

func_error:
	PrintExit;
	return eDiRet;
}

DI_ERR_CODE DI_MEDIA_SetUserAgentString(SESSION_HANDLE pHSession,  HINT8 *pUserAgentString, HINT32 nSize)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if( (pUserAgentString == NULL) || (nSize <= 0 ))
	{
		PrintError ("%s(%d) Error> Invalid input parameters .\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	PrintError("[%s] UA String [%s]\n",__FUNCTION__,pUserAgentString);
	eDIError = DI_IPTUNER_SetUserAgentString(pstTunerHdl,pUserAgentString,nSize);
	PrintExit;
	return eDIError;
}


/*
 * 생성된 Session Handle 을 입력받아 Seek(SetPosition) 기능을 수행한다.
 */
DI_ERR_CODE DI_MEDIA_SetPosition (SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT64 ullPosition)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;
#if 0
	HUINT32 ulCurrentPlayTime;
	HUINT32 nResult = ERR_OK;
#endif
	HUINT32 unDuration=0;
	/* YouView Specific Parameter */
#if 0
	HBOOL	bFilled_1 = FALSE;
	HBOOL	bFilled_2 = FALSE;
#else
	HBOOL	bFilled_1 = TRUE;
	HBOOL	bFilled_2 = TRUE;
#endif
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DI_MEDIA_MSG	audMsg;
	int				nVkResult = VK_OK;
	HUINT32			unLoopCount = 0;

	PrintEnter;

	s_nSuccessiveSeekState = 0;

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById!!Handle : %d\n", __FUNCTION__,(HUINT32)pHSession);
		return DI_ERR_ERROR;
	}

	eDiRet = DI_IPTUNER_GetDuration (pstTunerHdl, &unDuration);
	if( eDiRet != DI_ERR_OK )
	{
		PrintError("[%s] Fail DI_IPTUNER_GetDuration !! Handle : %d \n", __FUNCTION__,(HUINT32)pHSession);
		return DI_ERR_ERROR;
	}

	if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
	{
		audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
		nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nVkResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
		}
		else
		{
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
			audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
			}
			else
			{
				for(unLoopCount = 0; unLoopCount < 20; unLoopCount++)
				{
					if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
					{
						break;
					}

					VK_TASK_Sleep(100);
				}
			}
		}
	}

#if 1 /* workaround : AVI have Seek Issue (Ref.CSP555200, CSP554487) */
	if( (unDuration == ullPosition)
		&& (pstTunerHdl->eDownloadType == DI_IPTUNER_FILE)
		&& ( s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_AVI) )
	{
		ullPosition = ullPosition -1;
	}
#endif

	if(unDuration < ullPosition)
	{
		PrintError("[%s] Fail Position(%"PRId64") is bigger than content duration value (%u)!! Handle : %d \n"
		, __FUNCTION__
		,ullPosition
		,unDuration
		,(HUINT32)pHSession);
		return DI_ERR_ERROR;
	}

	if((s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_PLAY)&&
		(s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_PAUSE)&&
		(s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_READY))
	{
		PrintError("Media state is NOT PLAY or PAUSE!!!(%d)\n", s_tMediaInstance[unMediaId].mediastate);
		return DI_ERR_OK;
	}

	/*******************************************
	 * For Youview :  UPDATE ENDTIME
	 * playing-position (UPDATE)
	 ******************************************/
	if (s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_NO_SETUP)
	{
		if ( ullPosition >= pstTunerHdl->ulEndTime )
		{
			/* Clear EndTime for media */
			eDiRet = P_MEDIA_SetEndTime (unMediaId, 0);
			if(eDiRet != DI_ERR_OK)
			{
				PrintError("[%s] Fail DI_MEDIA_MP_SetEndTime\n", __FUNCTION__);
				return eDiRet;
			}
			/* Clear EndTime for iptuner*/
			eDiRet = DI_IPTUNER_SetEndTime (pstTunerHdl, 0);
			if(eDiRet != DI_ERR_OK)
			{
				PrintError("[%s] Fail DI_IPTUNER_SetEndTime\n", __FUNCTION__);
				return eDiRet;
			}
			/* Clear EndTime in PAL Layer. */
			//	pstSession->ulEndTime = 0;
		}
	}

	/**************************
	 * FILE Tuner
	 *************************/
	if (pstTunerHdl->eDownloadType == DI_IPTUNER_FILE)
	{
		/* local file seeking */
		if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
		{
			PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}

		eDiRet = P_MEDIA_SetPosition (unMediaId, ullPosition);
		if(eDiRet != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_MEDIA_MP_SetPosition\n", __FUNCTION__);
		}
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		goto seek_normal_return;
	}
	/**************************
	 * Download Tuner
	 *************************/
	else if (pstTunerHdl->eDownloadType == DI_IPTUNER_DOWNLOADING)
	{
		if ( pstTunerHdl->tMediaInfo.duration < ullPosition)
		{
			PrintError("[%s] seek position is invalid\n", __FUNCTION__);
			eDiRet = DI_ERR_ERROR;
		}
		else
		{
			/* if this playback spending a time in underflow status , it should be excaped.*/
			(void) DI_IPTUNER_EscapeUnderflow (pstTunerHdl);

			/***************************************************
			 * specified in YouView Playback
			 *
			 * 점프하려는 포지션부터 Endtime 까지 data 가 이미 다받아져 있는 경우는 playback 만 seek 한다.
			 * 그렇지 않고 해당 region 에 hole 이 있는 상태이면 buffering start event 와 함께 buffering 을 start 해준다음
			 * Playback 을 원하는 위치부터 재생한다.
			 *
			 * buffering region 의 hole 을 검사하는 기능은 IPTUNER 의 DNP module 에서
			 * OLT (Offset Lookup Table) 를 조사하여 확인한다.
			 */
			/* TODO : if it is remains in delayed starting status . not playing status???? */
			bFilled_1 = DI_IPTUNER_CheckBufferRegion (pstTunerHdl
										  , ullPosition
										  , ullPosition + pstTunerHdl->ulStartThreshold );
#if 1
			/* investigating a validation of buffer from seek position to endofmedia. */
			if( pstTunerHdl->ulEndTime > ullPosition + pstTunerHdl->ulStartThreshold)
			{
				bFilled_2 = DI_IPTUNER_CheckBufferRegion (pstTunerHdl, ullPosition + pstTunerHdl->ulStartThreshold, pstTunerHdl->ulEndTime);
			}

			if(bFilled_1 == FALSE || bFilled_2 == FALSE)
			{
				/* BUFFERING START EVENT */
				eDiRet = DI_IPTUNER_ResumeBufferingBySeek (pstTunerHdl);
				if(eDiRet != DI_ERR_OK)
				{
					PrintDebug("[%s] Fail DI_IPTUNER_ResumeBuffering. Anyway continue \n", __FUNCTION__);
					/* continue : The buffering condition may be normal status */
				}

				if(bFilled_1 == FALSE)
				{
					/* Ok!1 protection underflow status*/
					// TODO: State Check AV UnderFlow
					// pstSession->eState = PAL_MEDIA_STATE_DELAYED_STARTING;
				}
			}

			if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
			{
				PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
				return DI_ERR_ERROR;
			}

/*************************
*	Cancel Waiting I/O
*************************/
			if((pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_MP4)||
					(pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF))
			{
				(void)DI_IPTUNER_OnExitReadFlag(pstTunerHdl);
			}

/*************************
*	Actual seek
*************************/

#ifdef CONFIG_DLNA_CERTIFICATION
			DI_IPTUNER_ConnectForResume(pstTunerHdl);
#endif
			eDiRet = P_MEDIA_SetPosition (unMediaId, ullPosition);
			if(eDiRet != DI_ERR_OK)
			{
				PrintError("[%s] Fail DI_MEDIA_MP_SetPosition\n", __FUNCTION__);
			}

			eDiRet = DI_IPTUNER_SetCurrentPlayTime(pstTunerHdl, ullPosition);
			if(eDiRet != DI_ERR_OK)
			{
				PrintError("[%s] Fail DI_IPTUNER_SetCurrentOffset\n", __FUNCTION__);
			}

			(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

			/* Enter the download task to startup condition */
			if(bFilled_1 == FALSE)
			{
				/* Ok!1 it enters delayed start mode */
				/* START Event will be generated in DelayedSeekCallback. */
			}
			else
			{
				if (eDiRet == DI_ERR_OK)
				{
					/****************************************************
					 * SEEK 이후에도 STARTED Event 는 emit 되어야 한다.
					 ****************************************************/
					if(s_tEventCallback[unMediaId] != NULL)
					{
						s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_START, 0);
					}
					// s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;

					if (pstTunerHdl->bUseClsMonitor == TRUE)
					{
						DI_IPTUNER_CLS_BUFSTATE_e eLiveBufferState;
						if (DI_IPTUNER_GetLiveBufferState(pstTunerHdl, &eLiveBufferState) == DI_ERR_OK)
						{
							if (eLiveBufferState != DI_IPTUNER_CLS_BUFSTATE_COMPLETED)
							{
								(void)DI_IPTUNER_SetLiveBufferState(pstTunerHdl, DI_IPTUNER_CLS_BUFSTATE_COMPLETED);
							}
						}
					}
				}
			}

/*************************
*	1. Check if it has any holes.
*************************/
			bFilled_1 = DI_IPTUNER_CheckBufferRegion(pstTunerHdl, ullPosition, pstTunerHdl->ulEndTime);
			if(bFilled_1 == FALSE)
			{
				eDiRet = DI_IPTUNER_ForceMoveBufferingPosition(pstTunerHdl);
				if(eDiRet != DI_ERR_OK)
				{
					PrintError("DI_IPTUNER_ForceMoveBufferingPosition failed!\n");
					eDiRet = DI_ERR_ERROR;
				}
			}
#else
			eDiRet = DI_MEDIA_MP_SetPosition (pstSession->nDeviceIdforSession, ulTime, DI_MEDIA_TIME_MSEC);
			if(eDiRet != DI_ERR_OK)
			{
				PalMediaError(("[%s] Fail DI_MEDIA_MP_SetPosition\n", __FUNCTION__));
				ulRet = ERR_FAIL;
			}

			/* investigating a validation of buffer from seek position to endofmedia. */
			bFilled_2 = DI_IPTUNER_CheckBufferRegion (pstTunerHdl, ulTime, pstSession->ulEndTime);


			if(bFilled_1 == FALSE || bFilled_2 == FALSE)
			{
				/* Enter the download task to startup condition */
				if(bFilled_1 == FALSE)
				{
					/* Ok!1 it enters delayed start mode */
					pstSession->eState = PAL_MEDIA_STATE_DELAYED_STARTING;
					//DI_IPTUNER_DelayedSeekTrigger (pstTunerHdl);
					VK_TASK_Sleep(1000);
					(void) DI_IPTUNER_RegisterDelayedSeekCB (pstTunerHdl, &local_MEDIA_DelayedSeekCallback);
				}
				/* BUFFERING START EVENT */
				eDiRet = DI_IPTUNER_ResumeBufferingBySeek (pstTunerHdl);
				if(eDiRet != DI_ERR_OK)
				{
					PalMediaPrint(DBG_PRINT
						, ("[%s] Fail DI_IPTUNER_ResumeBuffering. Anyway continue \n", __FUNCTION__));
					/* continue : The buffering condition may be normal status */
				}
			}
			else
			{
				if (eDiRet == DI_ERR_OK)
				{
					/****************************************************
					 * SEEK 이후에도 STARTED Event 는 emit 되어야 한다.
					 ****************************************************/
					if( NULL != pstSession->tEventCallback )
					{
						pstSession->tEventCallback((HUINT32)pstSession->nDeviceIdforSession,PAL_MEDIA_EVENT_START, 0);
					}
				}
			}
#endif

		}
	}
	/**************************
	 * Stream Tuner
	 *************************/
	else if (pstTunerHdl->eDownloadType == DI_IPTUNER_STREAMING)
	{
		if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
		{
			PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
			return DI_ERR_ERROR;
		}

	#if 1
		/* FlushPump등 lowlevel API를 호출하기 보다는 NEXUS_Playback_Seek with 0sec 을 통해 PB이하 모든것 reset한다. */
		/* IPTUNER_Control에서는 SetPosition 다음 VFIO Read에 대해 ASF FileHeadef를 먼저 Feed하고 새로운 IFrame을 Feed할 수 잇게한다. */
		eDiRet = DI_IPTUNER_Control (pstTunerHdl, CMD_CONTROL_SEEK, ullPosition, 0);
		if(eDiRet != DI_ERR_OK)
		{
			(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			goto seek_normal_return;
		}
	#endif

		/* 2010.03.25 junsy
		 * HTTP Streaming Tuner는 ulTime을 그대로 사용
		 * MMS Streaming Tuner 는 0 를 설정하여 Playback Postion 을 0으로 초기화 한다.
		 * MMS 에서 time value 가 아닌 offset 으로 seek 가능한 시점에는 공통화 할 필요가 있음.
		 */
		if (pstTunerHdl->eProtocolType == DI_IPTUNER_PROTOCOL_HTTP)
		{
			if((pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_MP4)||
					(pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF))
			{
				(void)DI_IPTUNER_OnExitReadFlag(pstTunerHdl);
			}

			eDiRet = P_MEDIA_SetPosition (unMediaId, ullPosition);
			if(eDiRet != DI_ERR_OK)
			{
				PrintError("[%s] Fail DI_MEDIA_MP_SetPosition\n", __FUNCTION__);
			}
		}
		else
		{
			/* NEXUS_Playback_Seek with 0sec을 통해 PB이하 모든것 reset */
			eDiRet = P_MEDIA_SetPosition (unMediaId, 0);
			if(eDiRet != DI_ERR_OK)
			{
				PrintError("[%s] Fail DI_MEDIA_MP_SetPosition to ZERO\n", __FUNCTION__);
			}
#if 0
			if(ullPosition > (HUINT32)0) /* seek to 0 has no redundant frames */
			{
				/* fast skip redundant frames */
				// TODO: DI_VIDEO 컨트롤 처리
				nResult = DI_VIDEO_Show(0, FALSE);
				if(nResult != DI_ERR_OK)
				{
					PrintError("[%s] Error - Fail DI_VIDEO_Show with FALSE\n",__func__);
					return DI_ERR_ERROR;
				}

				eDiRet = DI_MEDIA_SetSpeed(pHSession, unMediaId ,(DI_MEDIA_TrickSpeed_e)DI_MEDIA_TRICK_4FFSTEP);
				if(eDiRet != DI_ERR_OK)
				{
					PrintError("[%s] Fail DI_MEDIA_MP_SetSpeed\n", __FUNCTION__);
				}

				while(TRUE)
				{
					DI_MEDIA_GetPosition(pHSession, unMediaId, &ulCurrentPlayTime);
					if(ulCurrentPlayTime >= ullPosition)
					{
						break;
					}
					VK_TASK_Sleep(10);
				}

				eDiRet = DI_MEDIA_SetSpeed(pHSession, unMediaId ,(DI_MEDIA_TrickSpeed_e)DI_MEDIA_TRICK_1FFSTEP);
				if(eDiRet != DI_ERR_OK)
				{
					PrintError("[%s] Fail DI_MEDIA_MP_SetSpeed\n", __FUNCTION__);
				}
				// TODO: DI_VIDEO 컨트롤 처리
				nResult = DI_VIDEO_Show(0, TRUE);
				if(nResult != DI_ERR_OK)
				{
					PrintError("[%s] Error - Fail DI_VIDEO_Show with TRUE\n",__func__);
				}
			}
#endif
		}
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	}
	/* CASE #4 : error case */
	else
	{
		PrintError("[%s] Fail DI_MEDIA_MP_SetPosition\n", __FUNCTION__);
		eDiRet = DI_ERR_ERROR;
	}
seek_normal_return:
	return eDiRet;
}

/*
 * 생성된 Session Handle 을 입력받아 현재 Position 을 반환한다.
 */
DI_ERR_CODE DI_MEDIA_GetPosition (SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT64 *pullPosition)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	HUINT32 				nTempPlayTime = 0;
	HUINT32 				unPtsSnapShot=0;
	NEXUS_PlaybackStatus pStatus;

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}

	if(s_bPlayTimerMode[unMediaId] == DI_MEDIA_TIMERMODE_NONE)
	{
		HUINT32 unDuration = 0;
		if( DI_IPTUNER_GetDuration (pHSession, &unDuration) != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_IPTUNER_GetDuration !! Handle : %d \n", __FUNCTION__,(HUINT32)pHSession);
			unDuration = s_nCurrentPlayTime[unMediaId] * 1000;
		}
		PrintData("[%s] TimeMode=FALSE, s_nCurrentPlayTime=%u\n", __func__, s_nCurrentPlayTime[unMediaId] * 1000);
		if(s_nCurrentPlayTime[unMediaId]*1000 > unDuration)
			*pullPosition = unDuration;
		else
			*pullPosition = s_nCurrentPlayTime[unMediaId]*1000;
	}
	else
	{
		if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
		{
			if(s_tMediaInstance[unMediaId].unSuccesiveSeekTime == 0)
			{
				*pullPosition = 0;
			}
			else
			{
				*pullPosition = s_tMediaInstance[unMediaId].unSuccesiveSeekTime*1000;
			}
		}
		else if((s_tMediaInstance[unMediaId].trickrate > eDI_MEDIA_Speed_FF_X4)||
			(s_tMediaInstance[unMediaId].trickrate < eDI_MEDIA_Speed_FF_X_1_10))
		{
			tNexusError= NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &pStatus);
			if(tNexusError != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Playback_GetStatus failed\n");
				(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				return DI_ERR_ERROR;
			}
			*pullPosition = pStatus.position;
		}
		else
		{
			tNexusError = b_play_getpts (s_tMediaInstance[unMediaId].playback, &unPtsSnapShot);
			if(tNexusError != NEXUS_SUCCESS)
			{
				/*workaournd patch by junsy */
				*pullPosition = s_tMediaInstance[unMediaId].ullPrevValidPosition;
			}
			else
			{
				/**************************************************
				 * YouView Specific parameters : junsy 2011.11.02
				 *************************************************/
				/** Get Playback Play Position */
				{
					nTempPlayTime = bmedia_pts2time(unPtsSnapShot-s_tMediaInstance[unMediaId].unFirstPts, BMEDIA_TIME_SCALE_BASE);
				}

				/********************************************************
				 * if this function called after ENDOFSTREAM Event
				 * getposition correction
				 ********************************************************/
				if( s_tMediaInstance[unMediaId].bEndOfStreamState == TRUE)
				{
					nTempPlayTime = s_ulMediaEndTime[unMediaId] ? s_ulMediaEndTime[unMediaId] : s_tStreamInfo[unMediaId].duration;
				}

				/* Exception Case #1 : the calculated time value is over than content duration */
				if (nTempPlayTime > s_tStreamInfo[unMediaId].duration)
				{
					*pullPosition = s_tStreamInfo[unMediaId].duration;
				}
				/* Exception Case #2 :
				 *	This function called after The ENDOFSTREAM event is generated. */
				else
				{
					/** Workaround : seeking after endofstream or endoftime event : detailed revising.*/
					if(nTempPlayTime+700 > s_tStreamInfo[unMediaId].duration)
					{
						*pullPosition = s_tStreamInfo[unMediaId].duration;
					}
					else
					{
						*pullPosition = nTempPlayTime;
					}
				}
			}
		}
		PrintData("[%s] s_nCurrentPlayTime=%llu\n", __func__, *pullPosition);
	}
	s_tMediaInstance[unMediaId].ullPrevValidPosition = *pullPosition;

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	return eDIError;
}

/*
 * 생성된 Session Handle 을 입력받아 기능을 수행한다.
 * 해당 Session 의 Play 배속을 설정한다.
 */
DI_ERR_CODE DI_MEDIA_SetSpeed (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_PlaySpeed_e eSpeed )
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStatus nStatus;
	NEXUS_PlaybackTrickModeSettings tTrickModeSettings;
	DI_MEDIA_MSG		audMsg;
	VK_ERR_CODE			nVkResult = VK_OK;
	HUINT32				unLoopCount = 0;
	HINT32				nDtcpRet = 0;
	struct timeval		tCurrent_Time;

	PrintEnter;

	s_nSuccessiveSeekState = 0;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		PrintExit;
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		PrintExit;
		return eDIError;
	}

/*************************
*	check current state
*************************/
	if((s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_PLAY) &&
		(s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_PAUSE))
	{
		PrintDebug ("Media state is NOT PLAY!!!(%d)\n", s_tMediaInstance[unMediaId].mediastate);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_OK;
	}

	if (pstTunerHdl->eDownloadType == DI_IPTUNER_FILE)
	{
	}
	else if (pstTunerHdl->eDownloadType == DI_IPTUNER_DOWNLOADING)
	{
	}
	else if (pstTunerHdl->eDownloadType == DI_IPTUNER_STREAMING)
	{
	}
	else
	{
		PrintError("[%s] Unknown Media Transfer Type.\n", __FUNCTION__);
		eDIError = DI_ERR_ERROR;
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		PrintExit;
		return eDIError;
	}

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		PrintExit;
		return DI_ERR_ERROR;
	}

	/** Check Limitation Speed*/
	switch(eSpeed)
	{
		/* Trick RW  (Slow) */
		case eDI_MEDIA_Speed_RW_X_1_2:
		case eDI_MEDIA_Speed_RW_X_1_4:
		case eDI_MEDIA_Speed_RW_X_1_8:
		case eDI_MEDIA_Speed_RW_X_1_10:
		case eDI_MEDIA_Speed_RW_X12_10:
#ifdef CONFIG_DLNA_CERTIFICATION
			if( pstTunerHdl->tDlnaSetting.pcProfile == NULL || (pstTunerHdl->tDlnaSetting.pcProfile != NULL && VK_strstr(pstTunerHdl->tDlnaSetting.pcProfile, "MPEG_TS_JP_T")))
#endif
			{
				PrintError("[%s] Error -  Fine speed(1/2 ~ 1/10) rewind is not supported.\n", __func__);
				(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				PrintExit;
				return DI_ERR_ERROR;
			}
		default:
			{
				PrintData("[%s] Support Speed List(%d)\n", __func__, eSpeed);
			}
			break;
	}

	/** Set Trick Rate */
	(void)NEXUS_Playback_GetDefaultTrickModeSettings(&tTrickModeSettings);
	if(eSpeed <= eDI_MEDIA_Speed_FF_X4
	&& eSpeed > eDI_MEDIA_Speed_Pause)
	{
		tTrickModeSettings.maxDecoderRate = P_MEDIA_ConvertDITrickSpeedToNexus(eSpeed);
		tTrickModeSettings.rate = P_MEDIA_ConvertDITrickSpeedToNexus(eSpeed);
	}
	else
	{
		tTrickModeSettings.rate = P_MEDIA_ConvertDITrickSpeedToNexus(eSpeed);
	}

	if(pstTunerHdl->tMediaInfo.eDrmType == DI_MEDIA_DRM_TYPE_MARLIN)
	{
		DRM_Key_Info_t drm_key_info;
		DRV_Error dResult;
		VK_memset(&drm_key_info, 0x00, sizeof(DRM_Key_Info_t));

		drm_key_info.offer.marlin.cmd = DI_DRM_MARLIN_SETSPEED;
		drm_key_info.offer.marlin.speed = (HINT32)eSpeed;

		dResult = DRV_DRM_GetKey(pHSession, &drm_key_info);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_DRM_GetKey failed!\n");
			(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			return DI_ERR_ERROR;
		}
	}

	switch(eSpeed)
	{
		case eDI_MEDIA_Speed_Pause:
#ifdef CONFIG_DLNA_CERTIFICATION
			if(pstTunerHdl->tDlnaSetting.bDlna == TRUE
			&& pstTunerHdl->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_NONE
			&& pstTunerHdl->tDlnaSetting.bConnectionStall == FALSE)
			{
				PrintError("not support pause operation. (NOT SUPPORT SEEK-MODE and ConnectionStall)\n");
				(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				return DI_ERR_ERROR;
			}
#endif

			if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
			{
				audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
				nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
				if(nVkResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
					eDIError = DRV_ERR;
				}
				else
				{
					audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
					audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
					nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
					if(nVkResult != VK_OK)
					{
						PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
						eDIError = DRV_ERR;
					}
					else
					{
						for(unLoopCount = 0; unLoopCount < 20; unLoopCount++)
						{
							if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
							{
								break;
							}

							VK_TASK_Sleep(100);
						}
					}
				}
			}

			tNexusError = NEXUS_Playback_Pause(s_tMediaInstance[unMediaId].playback);
			if(NEXUS_SUCCESS != tNexusError)
			{
				PrintError("[%s] Error - Fail NEXUS_Playback_Pause:%d\n", __func__, tNexusError);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
#ifdef CONFIG_DLNA_CERTIFICATION
				if(pstTunerHdl->tDlnaSetting.bDlna == TRUE)
				{
					DI_IPTUNER_DisconnectForPause(pstTunerHdl);
				}
#endif
				if(s_tEventCallback[unMediaId] != NULL)
				{
					s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PAUSE, 0);
				}

				s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PAUSE;

				if (pstTunerHdl->bUseClsMonitor == TRUE)
				{
					DI_IPTUNER_CLS_BUFSTATE_e eLiveBufferState;
					if (DI_IPTUNER_GetLiveBufferState(pstTunerHdl, &eLiveBufferState) == DI_ERR_OK)
					{
						if (eLiveBufferState != DI_IPTUNER_CLS_BUFSTATE_BUFFERING)
						{
							(void)DI_IPTUNER_SetLiveBufferState(pstTunerHdl, DI_IPTUNER_CLS_BUFSTATE_BUFFERING);
						}
					}
				}

				if(pstTunerHdl->tDtcpIpSetting.bDtcpIp == TRUE)
				{
					gettimeofday(&tCurrent_Time, NULL);
					pstTunerHdl->tDtcpIpSetting.nLastKeepAliveTime = tCurrent_Time.tv_sec;
				}
			}
			break;
		case eDI_MEDIA_Speed_Normal:
			/** Start Playback */
			if((pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_MP4)
			|| (pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF)
			|| (pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_MP4FRAGMENT))

			{
				(void)DI_IPTUNER_OnExitReadFlag(pstTunerHdl);
			}

			if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
			{
				audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
				nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
				if(nVkResult != VK_OK)
				{
					PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
					eDIError = DRV_ERR;
				}
				else
				{
					audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
					audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
					nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
					if(nVkResult != VK_OK)
					{
						PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
						eDIError = DRV_ERR;
					}
					else
					{
						for(unLoopCount = 0; unLoopCount < 20; unLoopCount++)
						{
							if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
							{
								break;
							}

							VK_TASK_Sleep(100);
						}
					}
				}
			}

			if((pstTunerHdl->tDtcpIpSetting.bDtcpIp == TRUE) && (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PAUSE))
			{
				gettimeofday(&tCurrent_Time, NULL);

				if(360 < (tCurrent_Time.tv_sec - pstTunerHdl->tDtcpIpSetting.nLastKeepAliveTime))
				{
					(void)DtcpAppLib_CloseAke(pstTunerHdl->tDtcpIpSetting.AkeHandle);
					pstTunerHdl->tDtcpIpSetting.AkeHandle = NULL;

					nDtcpRet = DtcpAppLib_DoAke(pstTunerHdl->tDtcpIpSetting.pcDTCP1HOST, VK_atoi(pstTunerHdl->tDtcpIpSetting.pcDTCP1PORT), &pstTunerHdl->tDtcpIpSetting.AkeHandle);
					if(nDtcpRet != MXDTCP_SUCCESS)
					{
						PrintError("### Ake Fail(%d) ###\n", nDtcpRet);
						pstTunerHdl->tDtcpIpSetting.AkeHandle = NULL;
					}
				}

				VK_printf("### Wating Time = %ld ###\n", (tCurrent_Time.tv_sec - pstTunerHdl->tDtcpIpSetting.nLastKeepAliveTime));
			}

			tNexusError = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
			if(tNexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Error - Fail NEXUS_Playback_Play:%d\n", __func__, tNexusError);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
#ifdef CONFIG_DLNA_CERTIFICATION
				DI_IPTUNER_ConnectForResume(pstTunerHdl);
#endif
				if(s_tEventCallback[unMediaId] != NULL)
				{
					s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PLAYING, 0);
				}

				s_tMediaInstance[unMediaId].trickrate = eDI_MEDIA_Speed_Normal;
				s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;

				if (pstTunerHdl->bUseClsMonitor == TRUE)
				{
					DI_IPTUNER_CLS_BUFSTATE_e eLiveBufferState;
					if (DI_IPTUNER_GetLiveBufferState(pstTunerHdl, &eLiveBufferState) == DI_ERR_OK)
					{
						if (eLiveBufferState != DI_IPTUNER_CLS_BUFSTATE_COMPLETED)
						{
							(void)DI_IPTUNER_SetLiveBufferState(pstTunerHdl, DI_IPTUNER_CLS_BUFSTATE_COMPLETED);
						}
					}
				}

/*************************
*	Support DNP Underrun AUTO mode
*************************/
				tNexusError = NEXUS_Playback_GetStatus(s_tMediaInstance[unMediaId].playback, &nStatus);
				if(tNexusError != NEXUS_SUCCESS)
				{
					PrintError("NEXUS_Playback_GetStatus failed!\n");
				}

				eDIError = DI_IPTUNER_SetCurrentPlayTime(pstTunerHdl, (HUINT32)nStatus.position);
				if(eDIError != DI_ERR_OK)
				{
					PrintError("[%s] Fail DI_IPTUNER_SetCurrentOffset\n", __FUNCTION__);
				}

/*************************
*	To handle the user resume in Underrun condition
*************************/
				eDIError = DI_IPTUNER_CheckUnderrun(pstTunerHdl);
				if(eDIError != DI_ERR_OK)
				{
					PrintError("[%s] Fail DI_IPTUNER_CheckUnderrun\n", __FUNCTION__);
				}
			}
			break;
		/* Trick RW */
		case eDI_MEDIA_Speed_RW_X128:
		case eDI_MEDIA_Speed_RW_X64:
		case eDI_MEDIA_Speed_RW_X32:
		case eDI_MEDIA_Speed_RW_X16:
		case eDI_MEDIA_Speed_RW_X8:
		case eDI_MEDIA_Speed_RW_X4:
		case eDI_MEDIA_Speed_RW_X2:
		case eDI_MEDIA_Speed_RW_X1:
		/* Trick RW  (Slow) */
		case eDI_MEDIA_Speed_RW_X_1_2:
		case eDI_MEDIA_Speed_RW_X_1_4:
		case eDI_MEDIA_Speed_RW_X_1_8:
		case eDI_MEDIA_Speed_RW_X_1_10:
		/* Trick FF (Slow) */
		case eDI_MEDIA_Speed_FF_X_1_10:
		case eDI_MEDIA_Speed_FF_X_1_8:
		case eDI_MEDIA_Speed_FF_X_1_4:
		case eDI_MEDIA_Speed_FF_X_1_2:
		/* Trick FF */
		case eDI_MEDIA_Speed_FF_X2:
		case eDI_MEDIA_Speed_FF_X4:
		case eDI_MEDIA_Speed_FF_X8:
		case eDI_MEDIA_Speed_FF_X16:
		case eDI_MEDIA_Speed_FF_X32:
		case eDI_MEDIA_Speed_FF_X64:
		case eDI_MEDIA_Speed_FF_X128:
			if(s_tStreamInfo[unMediaId].support_trick != TRUE)
			{
				PrintError("[%s] Error - Not Support Trick in The Media Stream.\n", __func__);
				eDIError = DI_ERR_ERROR;
			}
			else
			{
				if((pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_MP4)||
						(pstTunerHdl->tMediaInfo.stream_type == DI_MEDIA_MPEG_TYPE_ASF))
				{
					(void)DI_IPTUNER_OnExitReadFlag(pstTunerHdl);
				}

				if(pstTunerHdl->tDlnaSetting.bDlna == TRUE)
				{
					/*
					 * If iptuner opens the TS file through DLNA interaction,
					 * Playback uses a different trick mode that looks like seek, not smoothly.
					 *
					 * bSuccesiveSeekForTrick is enabled When a file format is TS and Streaming method is DLNA.
					 */
					if((s_tStreamInfo[unMediaId].bSuccesiveSeekMode == TRUE) && ((tTrickModeSettings.rate < 0) || (tTrickModeSettings.rate >= 2000)))
					{
						audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_START;
						audMsg.ulParam2 = unMediaId;
						audMsg.ulParam3 = tTrickModeSettings.rate/1000;
						audMsg.ulParam4 = pstTunerHdl->tDlnaSetting.eSeekMode;
						audMsg.ulParam5 = TRUE;
						nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
						if(nVkResult != VK_OK)
						{
							PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
							eDIError = DRV_ERR;
						}
						else
						{
							s_tMediaInstance[unMediaId].bSuccesiveSeeking = TRUE;
							s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
						}
					}
					else
					{
						tNexusError = NEXUS_Playback_TrickMode(s_tMediaInstance[unMediaId].playback, &tTrickModeSettings);
					}
				}
				else
				{
					tNexusError = NEXUS_Playback_TrickMode(s_tMediaInstance[unMediaId].playback, &tTrickModeSettings);
				}

				if((NEXUS_SUCCESS != tNexusError) || (nVkResult != VK_OK))
				{
					PrintError("[%s] Error - Fail NEXUS_Playback_TrickMode:%d\n", __func__, tNexusError);
					eDIError = DI_ERR_ERROR;
				}
				else
				{
#ifdef CONFIG_DLNA_CERTIFICATION
					DI_IPTUNER_ConnectForResume(pstTunerHdl);
#endif
					if(s_tEventCallback[unMediaId] != NULL)
					{
						s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_PLAYING, s_nCurrentPlayTime[unMediaId]);
					}

					if(s_tEventCallback[unMediaId] != NULL)
					{
						s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_TRICK, s_nCurrentPlayTime[unMediaId]);
					}

					s_tMediaInstance[unMediaId].trickrate = eSpeed;
				}
			}
			break;
		default:
			{
				PrintError("[%s] Error - Invalid Type:%d\n", __func__, eSpeed);
				eDIError = DI_ERR_ERROR;
			}
			break;
	}

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return eDIError;

}

/*
 * 생성된 Session Handle 을 입력받아 해당 Session 의 현재 Play 배속을 반환한다.
 */
DI_ERR_CODE DI_MEDIA_GetSpeed (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_PlaySpeed_e *peSpeed )
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	NEXUS_Error tNexusError = NEXUS_SUCCESS;
	NEXUS_PlaybackStatus 	tPlaybackStatus;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(s_tMediaInstance[unMediaId].playback == NULL)
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
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
			*peSpeed = s_tMediaInstance[unMediaId].trickrate;
		}
		else if(tPlaybackStatus.state == NEXUS_PlaybackState_ePaused)
		{
			*peSpeed = eDI_MEDIA_Speed_Pause;
		}
		else if(tPlaybackStatus.state == NEXUS_PlaybackState_ePlaying)
		{
			*peSpeed = eDI_MEDIA_Speed_Normal;
		}
		else
		{
			PrintError("[%s] Error - Not start Playback.\n", __func__);
			eDIError = DI_ERR_ERROR;
		}
	}

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	PrintExit;

	return eDIError;
}
#define str_eMEDIA_ERROR_R_NETWORK_LINK_DOWN                                  "Ethernet Link Down"
#define str_eMEDIA_ERROR_R_NETWORK_DNSLOOKUP_FAIL                             "DNS LookUp Failure"
#define str_eMEDIA_ERROR_R_NETWORK_CONNECTION_REFUSED                         "Connection Refused"
#define str_eMEDIA_ERROR_R_NETWORK_IGMP_JOIN_FAIL                             "Cannot Join IGMP"
#define str_eMEDIA_ERROR_R_NETWORK_SELECT_TIMEOUT                             "SELECT Timeout"
#define str_eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT                               "Seek Timeout"

#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CREATED                  			   "created"

#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MULTI_CHOICE                     "Multiple Choice"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNUSED                 			    "unused"

#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_REQUEST                     "Bad Request"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNAUTHORIZED                    "Unauthorized"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PAYMENT_REQUIRED                "Payment Required"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_FORBIDDEN                       "Forbidden"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND                       "Not Found"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED              "Method Not Allowed"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_ACCEPTABLE                  "Not Acceptable"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED             "Proxy Authentication Required"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_TIMEOUT                 "Request Time-out"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CONFILICT                       "Conflict"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GONE                            "Gone"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_LENGTH_REQUIRED                 "Length Required"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PRECONDITION_FAILED             "Precondition Failed"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE        "Request Entity Too Large"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE           "Request-URI Too Large"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE          "Unsupported Media Type"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE "Requested range not satisfiable"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_EXPECTATION_FAILED              "Expectation Failed"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR           "Internal Server Error"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_IMPLEMENTED                 "Not Implemented"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_GATEWAY                     "Bad Gateway"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE             "Service Unavailable"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT                 "Gateway Time-out"
#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED           "HTTP Version not supported"

#define str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MHEG_UNKNOWN                 			"Mheg unknown"

#define str_eMEDIA_ERROR_R_SERVER_RTSP_ERROR                                  "NOT Defined"
#define str_eMEDIA_ERROR_R_SERVER_MMS_ERROR                                   "NOT Defined"
#define str_eMEDIA_ERROR_R_SERVER_IIS_ERROR                                   "NOT Defined"
#define str_eMEDIA_ERROR_R_SYSTEM_NO_MEMORY                                   "Not Enough Memory"
#define str_eMEDIA_ERROR_R_IO_READ_FAIL                                       "IO Read Error"
#define str_eMEDIA_ERROR_R_IO_WRITE_FAIL                                      "IO Write Error"
#define str_eMEDIA_ERROR_R_IO_SEEK_FAIL                                       "IO Seek Error"
#define str_eMEDIA_ERROR_R_DATA_MALFORMED_MANIFEST                            "Malformed Manifest Format"
#define str_eMEDIA_ERROR_R_DATA_MALFORMED_MP4                                 "Malformed Mp4 Format"
#define str_eMEDIA_ERROR_R_DATA_MALFORMED_TS                                  "Malformed TransportStream Format"
#define str_eMEDIA_ERROR_R_DATA_NO_INDEX_FILE                                 "No Index File"

void * DI_MEDIA_GetErrorReason_StrDup (DI_MEDIA_ERROR_REASON_e eReason)
{
	switch (eReason)
	{
		case eMEDIA_ERROR_R_NETWORK_LINK_DOWN :
		return VK_StrDup (str_eMEDIA_ERROR_R_NETWORK_LINK_DOWN );
		case eMEDIA_ERROR_R_NETWORK_DNSLOOKUP_FAIL :
		return VK_StrDup (str_eMEDIA_ERROR_R_NETWORK_DNSLOOKUP_FAIL );
		case eMEDIA_ERROR_R_NETWORK_CONNECTION_REFUSED :
		return VK_StrDup (str_eMEDIA_ERROR_R_NETWORK_CONNECTION_REFUSED );
		case eMEDIA_ERROR_R_NETWORK_IGMP_JOIN_FAIL :
		return VK_StrDup (str_eMEDIA_ERROR_R_NETWORK_IGMP_JOIN_FAIL );
		case eMEDIA_ERROR_R_NETWORK_SELECT_TIMEOUT :
		return VK_StrDup (str_eMEDIA_ERROR_R_NETWORK_SELECT_TIMEOUT );
		case eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT :
		return VK_StrDup (str_eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT );

		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CREATED:
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CREATED );

		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MULTIPLE_CHOICE:
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MULTI_CHOICE);
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNUSED:
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNUSED);

		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_REQUEST  :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_REQUEST  );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNAUTHORIZED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNAUTHORIZED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PAYMENT_REQUIRED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PAYMENT_REQUIRED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_FORBIDDEN :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_FORBIDDEN );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_ACCEPTABLE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_ACCEPTABLE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_TIMEOUT :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_TIMEOUT );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CONFILICT :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CONFILICT );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GONE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GONE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_LENGTH_REQUIRED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_LENGTH_REQUIRED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PRECONDITION_FAILED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PRECONDITION_FAILED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_EXPECTATION_FAILED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_EXPECTATION_FAILED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_IMPLEMENTED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_IMPLEMENTED );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_GATEWAY :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_GATEWAY );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT );
		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED );

		case eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MHEG_UNKNOWN:
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MHEG_UNKNOWN);

		case eMEDIA_ERROR_R_SERVER_RTSP_ERROR :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_RTSP_ERROR );
		case eMEDIA_ERROR_R_SERVER_MMS_ERROR :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_MMS_ERROR );
		case eMEDIA_ERROR_R_SERVER_IIS_ERROR :
		return VK_StrDup (str_eMEDIA_ERROR_R_SERVER_IIS_ERROR );
		case eMEDIA_ERROR_R_SYSTEM_NO_MEMORY :
		return VK_StrDup (str_eMEDIA_ERROR_R_SYSTEM_NO_MEMORY );
		case eMEDIA_ERROR_R_IO_READ_FAIL :
		return VK_StrDup (str_eMEDIA_ERROR_R_IO_READ_FAIL );
		case eMEDIA_ERROR_R_IO_WRITE_FAIL :
		return VK_StrDup (str_eMEDIA_ERROR_R_IO_WRITE_FAIL );
		case eMEDIA_ERROR_R_IO_SEEK_FAIL :
		return VK_StrDup (str_eMEDIA_ERROR_R_IO_SEEK_FAIL );
		case eMEDIA_ERROR_R_DATA_MALFORMED_MANIFEST :
		return VK_StrDup (str_eMEDIA_ERROR_R_DATA_MALFORMED_MANIFEST );
		case eMEDIA_ERROR_R_DATA_MALFORMED_MP4 :
		return VK_StrDup (str_eMEDIA_ERROR_R_DATA_MALFORMED_MP4 );
		case eMEDIA_ERROR_R_DATA_MALFORMED_TS :
		return VK_StrDup (str_eMEDIA_ERROR_R_DATA_MALFORMED_TS );
		case eMEDIA_ERROR_R_DATA_NO_INDEX_FILE :
		return VK_StrDup (str_eMEDIA_ERROR_R_DATA_NO_INDEX_FILE );

		default : return NULL;
	}
	return NULL;
}



#if 0 /* YouView (CANVAS) */
/* 생성된 IPTUNER Handle 을 입력받아 Buffering Mode (specified on YouView) 를 설정한다.
 * 입력된 enum type 에 따라 buffering speed 및 size 를 결정하게 되며
 * 결정된 방식에 따라 IPTUNER Session speed 가 조절된다.
 */
DI_ERR_CODE DI_IPTUNER_SetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e eConstraint, HINT32 value);
/*
 * 생성된 IPTUNER handle 을 입력받아 Open된 iptuner 의 PlayingPosition 및 Buffering Position 관리를 위해
 * EndByte 를 설정한다.
 */
DI_ERR_CODE DI_IPTUNER_SetEndByte (DI_IPTUNER_t *pHTuner, HINT64 ulOffset );
#endif

/*
 * Session을 재생한다.
 * Play를 위해서는 Setup이 선행되어야 한다.
 */
DI_ERR_CODE DI_MEDIA_StartSession(SESSION_HANDLE pHSession, HUINT32 unMediaId, HULONG ulStartTime)
{
	DI_ERR_CODE dResult;
	DI_IPTUNER_t* pstTunerHdl=NULL;
	NEXUS_Error nResult;
	NEXUS_PlaybackStartSettings playbackstartSettings;

	PrintEnter;
/******************************
*	1. validate parameters
******************************/
	if(pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	dResult = P_MEDIA_CheckValidMediaId(unMediaId);
	if(dResult != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_ERROR;
	}

	if((s_tMediaInstance[unMediaId].playback == NULL) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_NO_SETUP))
	{
		PrintError("[%s] Error - Not initialized Media.\n", __func__);
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_ERROR;
	}

/******************************
*	2. Register callbacks to support Underrun
******************************/
	dResult = DI_IPTUNER_RegisterPumpBufferCB(pstTunerHdl, P_MEDIA_GetPumpBufferDepthCallback);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_RegisterPumpBufferCB failed!\n");
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_ERROR;
	}

	dResult = DI_IPTUNER_RegisterVideoBufferCB(pstTunerHdl, P_MEDIA_GetVideoBufferDepthCallback);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_RegisterVideoBufferCB failed!\n");
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_ERROR;
	}

	dResult = DI_IPTUNER_RegisterAudioBufferCB(pstTunerHdl, P_MEDIA_GetAudioBufferDepthCallback);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_RegisterAudioBufferCB failed!\n");
		VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_ERROR;
	}

/******************************
*	3. start playback
******************************/
	if((s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_READY) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_STOP))
	{
		if(s_tEventCallback[unMediaId] != NULL)
		{
			s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_CHANGEPLAYTIME, 0);
		}

		dResult = P_MEDIA_SetEndTime (unMediaId, pstTunerHdl->ulEndTime);
		if(dResult != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_MEDIA_MP_SetEndTime\n", __FUNCTION__);
			VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			return DI_ERR_ERROR;
		}
		(void)DI_IPTUNER_OffExitReadFlag(pstTunerHdl);

		NEXUS_Playback_GetDefaultStartSettings(&playbackstartSettings);

		if((s_tStreamInfo[unMediaId].video_stream_id == 0) && (s_tStreamInfo[unMediaId].audio_stream_id != 0)
				&& (s_tStreamInfo[unMediaId].audio_codec == DI_AUDFORMAT_MP3))
		{
			playbackstartSettings.indexFileIo.mode = NEXUS_PlaybackIndexFileIo_eModule;
			playbackstartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
		}
		else
		{
			playbackstartSettings.mode = NEXUS_PlaybackMode_eIndexed;

			if((s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS) || (s_tStreamInfo[unMediaId].stream_type == DI_MEDIA_MPEG_TYPE_TS_192))
			{
				playbackstartSettings.mpeg2TsIndexType = NEXUS_PlaybackMpeg2TsIndexType_eSelf;
			}
		}

		if (s_tStreamInfo[unMediaId].isHttpChunked == TRUE)
		{
			if (s_tStreamInfo[unMediaId].max_bitrate == 0)
			{
				s_tStreamInfo[unMediaId].max_bitrate = (2*1024*1024); /* Assumed with SD contents */
			}

			/* [EMEA:JPN] Commented because following cause poor playback issue for high bitrate streaming */
			if (0)
			{
				/* Fixed chunked live streaming issue for Sochi olympic
				   Force the bitrate value if we can't get it under HTTP chunked live streaming because of nexus_playback failure */
				if ((playbackstartSettings.bitrate > (4*1024*1024)) || (s_tStreamInfo[unMediaId].max_bitrate > (4*1024*1024)))
				{
					PrintError("Error> Changed too large max_bitrate(%d,%d)\n", playbackstartSettings.bitrate, s_tStreamInfo[unMediaId].max_bitrate);
					s_tStreamInfo[unMediaId].max_bitrate = (2*1024*1024);
				}
			}
		}
		playbackstartSettings.bitrate = s_tStreamInfo[unMediaId].max_bitrate;

		nResult = NEXUS_Playback_Start(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].file, &playbackstartSettings);
		if (nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Playback_Start failed with Index... Retrying...\n");
			NEXUS_FilePlay_Close(s_tMediaInstance[unMediaId].file);

			s_tMediaInstance[unMediaId].file = NEXUS_FilePlay_OpenPosix((char *)s_tMediaInstance[unMediaId].mediaurl, NULL);
			nResult = NEXUS_Playback_Start(s_tMediaInstance[unMediaId].playback, s_tMediaInstance[unMediaId].file, &playbackstartSettings);
			if (nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Playback_Start failed without Index\n");
				VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				return DI_ERR_ERROR;
			}
			s_tStreamInfo[unMediaId].support_trick = FALSE;
		}
	}

/******************************
*	4. seek to ulStartTime
******************************/
	if(ulStartTime != 0)
	{
		if(pstTunerHdl->eProtocolType == DI_IPTUNER_PROTOCOL_SSTR
		|| pstTunerHdl->eProtocolType == DI_IPTUNER_PROTOCOL_HLS
		|| pstTunerHdl->eProtocolType == DI_IPTUNER_PROTOCOL_DASH
		|| pstTunerHdl->eProtocolType == DI_IPTUNER_PROTOCOL_MMS
		|| pstTunerHdl->eProtocolType == DI_IPTUNER_PROTOCOL_RTSP)
		{
			dResult = DI_IPTUNER_Control (pstTunerHdl, CMD_CONTROL_SEEK, ulStartTime , 0);
			if(dResult != DI_ERR_OK)
			{
				VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				return DI_ERR_ERROR;
			}
		}
		else
		{
			nResult = NEXUS_Playback_Seek(s_tMediaInstance[unMediaId].playback, (NEXUS_PlaybackPosition)ulStartTime);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Playback_Seek failed!\n");
				VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
				return DI_ERR_ERROR;
			}
		}
	}

/******************************
*	5. check delayed start condition (only for dnp)
******************************/
	if(pstTunerHdl->eDownloadType == DI_IPTUNER_DOWNLOADING)
	{
		pstTunerHdl->ulStartTime = ulStartTime;
		dResult = DI_IPTUNER_RegisterDelayedStartCB (pstTunerHdl, (DI_IPTUNER_DelayedStartCallback)P_MEDIA_DelayedStartCallback);
		if(dResult != DI_ERR_OK)
		{
			PrintError("DI_IPTUNER_RegisterDelayedStartCB failed!\n");
			VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			return DI_ERR_ERROR;
		}
		pstTunerHdl->bDelayedStart = TRUE;
	}
	else
	{
		s_tMediaInstance[unMediaId].mediastate = DI_MEDIA_STATE_PLAY;
		s_tMediaInstance[unMediaId].trickrate = eDI_MEDIA_Speed_Normal;

		nResult = NEXUS_Playback_Play(s_tMediaInstance[unMediaId].playback);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Playback_Play failed\n");
			VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			return DI_ERR_ERROR;
		}

		if(s_tEventCallback[unMediaId] != NULL)
		{
			s_tEventCallback[unMediaId](unMediaId, DI_MEDIA_EVENT_START, 0);
		}

		if (pstTunerHdl->bUseClsMonitor == TRUE)
		{
			DI_IPTUNER_CLS_BUFSTATE_e eLiveBufferState;
			if (DI_IPTUNER_GetLiveBufferState(pstTunerHdl, &eLiveBufferState) == DI_ERR_OK)
			{
				if (eLiveBufferState != DI_IPTUNER_CLS_BUFSTATE_BUFFERING)
				{
					(void)DI_IPTUNER_SetLiveBufferState(pstTunerHdl, DI_IPTUNER_CLS_BUFSTATE_BUFFERING);
				}
			}
		}
	}

	VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 재생 중인 Session을 중지 한다.
 */
DI_ERR_CODE DI_MEDIA_StopSession (SESSION_HANDLE pHSession, HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DI_MEDIA_MSG	audMsg;
	VK_ERR_CODE		nVkResult = VK_OK;
	HUINT32			unLoopCount = 0;

	PrintEnter;

	s_nSuccessiveSeekState = 0;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if((s_tMediaInstance[unMediaId].playback == NULL) && (s_tStreamInfo[unMediaId].stream_type != DI_MEDIA_MPEG_TYPE_WAVE))
	{
		PrintError("[%s] Error - Playback handle is null.\n", __func__);
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}

	/* s_ulMediaStart for DI_AUDIO, DI_VIDEO : DRV_MEDIA_CheckStart() */
	if(s_ulMediaStart > 0)
		--s_ulMediaStart;

	/* Fast and continuous trick or seek actions cause too many remaining job in msg_que  */
	/* So, it is needed to prevent remaining trick(setSpeed) action in msg_que while session stopping */
	s_tMediaInstance[unMediaId].bSessionStopping = TRUE;

	if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
	{
		audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
		/* 빠르고&연속된 trick 동작의 경우 msg_send 시 que full 상황 가능성 높음 */
		nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nVkResult != VK_OK)
		{
			PrintError("[%s:%d] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__, __LINE__, nVkResult);
			/*VK_MSG_Clear(s_ulMediaSuccessiveSeekMId);*/
			eDIError = DRV_ERR;
		}
		else
		{
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
			audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
			/* 빠르고&연속된 trick 동작의 경우 msg_send 시 que full 상황 가능성 높음 */
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s:%d] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__, __LINE__, nVkResult);
				/*VK_MSG_Clear(s_ulMediaSuccessiveSeekMId);*/
				eDIError = DRV_ERR;
			}
			else
			{
				for(unLoopCount = 0; unLoopCount < 20; unLoopCount++)
				{
					if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
					{
						break;
					}

					VK_TASK_Sleep(100);
				}
			}
		}
	}

	if((s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PAUSE) || (s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_PLAY))
	{
		/** Stop Playback */
		(void) DI_IPTUNER_OnExitReadFlag (pstTunerHdl);
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

	if(pstTunerHdl->bDelayedStart == TRUE)
	{
		pstTunerHdl->bDelayedStart = FALSE;
	}

	(void) DI_IPTUNER_UnregisterVideoBufferCB(pstTunerHdl);
	(void) DI_IPTUNER_UnregisterAudioBufferCB(pstTunerHdl);
	(void) DI_IPTUNER_UnregisterPumpBufferCB(pstTunerHdl);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	PrintExit;

	return eDIError;

}

/*
 * 재생 중인 Session을 일시 정지 한다.
 */
DI_ERR_CODE DI_MEDIA_PauseSession (SESSION_HANDLE pHSession, HUINT32 unMediaId)
{
	DI_MEDIA_PlaySpeed_e eSpeed;
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DI_MEDIA_MSG	audMsg;
	int				nVkResult = VK_OK;
	HUINT32			unLoopCount = 0;

	PrintEnter;

	s_nSuccessiveSeekState = 0;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
	{
		audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
		nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nVkResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
			eDIError = DRV_ERR;
		}
		else
		{
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
			audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
				eDIError = DRV_ERR;
			}
			else
			{
				for(unLoopCount = 0; unLoopCount < 20; unLoopCount++)
				{
					if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
					{
						break;
					}

					VK_TASK_Sleep(100);
				}
			}
		}
	}

	eDIError = DI_MEDIA_GetSpeed(pHSession, unMediaId, &eSpeed);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_MEDIA_GetSpeed\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(eSpeed != eDI_MEDIA_Speed_Pause)
	{
		eDIError = DI_MEDIA_SetSpeed(pHSession, unMediaId, eDI_MEDIA_Speed_Pause);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_MEDIA_SetSpeed\n", __FUNCTION__);
			PrintExit;
			return DI_ERR_ERROR;
		}
	}
	else
	{
		PrintError("[%s] Fail Session(%d) is already paused\n", __FUNCTION__, (HUINT32)pHSession);
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 일시 정지 상태인 Session을 재생 한다.
 */
DI_ERR_CODE DI_MEDIA_ResumeSession (SESSION_HANDLE pHSession, HUINT32 unMediaId)
{
	DI_MEDIA_PlaySpeed_e eSpeed;
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DI_MEDIA_MSG	audMsg;
	int				nVkResult = VK_OK;
	HUINT32			unLoopCount = 0;

	PrintEnter;

	s_nSuccessiveSeekState = 0;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(s_tMediaInstance[unMediaId].bSuccesiveSeeking == TRUE)
	{
		audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_STOP;
		nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
		if(nVkResult != VK_OK)
		{
			PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
			eDIError = DRV_ERR;
		}
		else
		{
			audMsg.ulCmmand = DI_MEDIA_SUCCESSIVE_SEEK_GETSTATE;
			audMsg.pucParam = (unsigned char *)&s_nSuccessiveSeekState;
			nVkResult = VK_MSG_SendTimeout(s_ulMediaSuccessiveSeekMId, &audMsg, sizeof(DI_MEDIA_MSG), 0);
			if(nVkResult != VK_OK)
			{
				PrintError("[%s] VK_MSG_Send -error : ret(0x%x)\n", __FUNCTION__,nVkResult);
				eDIError = DRV_ERR;
			}
			else
			{
				for(unLoopCount = 0; unLoopCount < 20; unLoopCount++)
				{
					if(s_nSuccessiveSeekState == DI_MEDIA_SUCCESSIVE_SEEK_STOP)
					{
						break;
					}

					VK_TASK_Sleep(100);
				}
			}
		}
	}

	eDIError = DI_MEDIA_GetSpeed(pHSession,unMediaId,&eSpeed);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_MEDIA_GetSpeed\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(eSpeed != eDI_MEDIA_Speed_Pause)
	{
		PrintError("[%s] Fail Session(%d) is not paused\n", __FUNCTION__, (HUINT32)pHSession);
	}
	else
	{
		eDIError = DI_MEDIA_SetSpeed(pHSession,unMediaId,eDI_MEDIA_Speed_Normal);
		if(eDIError != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_MEDIA_SetSpeed\n", __FUNCTION__);
			PrintExit;
			return DI_ERR_ERROR;
		}
	}
	PrintExit;
	return DI_ERR_OK;

}

/*
 * 각 기능(SSTR/ABR/...)별 필요한 Setting Parameter를 RunTime 혹은 SetupTime에 설정한다.
 * Parameter 미정의 상태
 */
DI_ERR_CODE DI_MEDIA_SetSessionSettings (SESSION_HANDLE pHSession, const DI_MEDIA_SessionSettings_t *pSettings)
{
	HUINT64 ulPlayTime =0;
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	HUINT32 unMediaId;
	HUINT64 ullPosition = 0;
	HUINT32 ulEndTime = 0;
	DRV_Error dResult;
	DRM_Info_t drm_info;

	if(pSettings == NULL)
	{
		PrintError("[%s]  Fail Setting is NULL\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s]  Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

/*************************
*	Set the underflow mode
*************************/
	if(pSettings->eUnderflowMode == DI_MEDIA_UNDERFLOW_AUTO)
	{
		pstTunerHdl->eUnderflowMode = DI_IPTUNER_UNDERFLOW_AUTO;
	}
	else if(pSettings->eUnderflowMode == DI_MEDIA_UNDERFLOW_THRESHOLD)
	{
		pstTunerHdl->eUnderflowMode = DI_IPTUNER_UNDERFLOW_THRESHOLD;
	}
	else
	{
		PrintError("Invalid Underflow mode! changing to AUTO mode\n");
		pstTunerHdl->eUnderflowMode = DI_IPTUNER_UNDERFLOW_AUTO;
	}

/*************************
*	Set the buffer size
*************************/
	if(pSettings->uiBlockSize == 0)
	{
		pstTunerHdl->uiBlockSize = DEFAULT_DATA_BLOCK_SIZE;
	}
	else
	{
		pstTunerHdl->uiBlockSize = pSettings->uiBlockSize;
	}

	if(pSettings->uiRambufferSize == 0)
	{
		pstTunerHdl->uiRambufferSize = DEFAULT_RAM_BUFFER_SIZE;
	}
	else
	{
		pstTunerHdl->uiRambufferSize = pSettings->uiRambufferSize;
	}

	if(pSettings->uiProbebufferSize == 0)
	{
		pstTunerHdl->uiProbebufferSize = DEFAULT_PROBE_BUFFER_SIZE;
	}
	else
	{
		pstTunerHdl->uiProbebufferSize = pSettings->uiProbebufferSize;
	}

	/**
	 * Setting End Time
	 */
	ulEndTime = pSettings->ulEndTime;
	if ((P_MEDIA_FindMediaId((HUINT32)pHSession, &unMediaId) == DI_ERR_OK)
		&& P_MEDIA_CheckValidMediaId(unMediaId) == DI_ERR_OK)
	{
		if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) == VK_OK)
		{
			if (s_tMediaInstance[unMediaId].mediastate != DI_MEDIA_STATE_NO_SETUP)
			{
				/**********************************
				 * Setting EndTime
				 *
				 * When a playback position reaches this timeoffset,
				 * should emit to media-router the complete signal immediately.
				 * can handle not only playing-position but also buffering-position.
				 * and can handle three-types of timeline mode.
				 */
				if ( s_tMediaInstance[unMediaId].mediastate == DI_MEDIA_STATE_NO_SETUP)
				{
					/*
					 * If the playback is playing-status and ulTime is smaller than play-position,
					 * we can't apply a new endtime to playback, therefore it should be clear.
					 */
					eDiRet = DI_MEDIA_GetPosition (pHSession,unMediaId,&ulPlayTime);
					if(eDiRet == DI_ERR_OK)
					{
						if(ulPlayTime >= ulEndTime) ulEndTime = 0;
					}
				}

				eDiRet = P_MEDIA_SetEndTime (unMediaId, ulEndTime);
				if(eDiRet != DI_ERR_OK)
				{
					PrintError("[%s] Fail DI_MEDIA_MP_SetEndTime\n", __FUNCTION__);
				}
#if 0 /* for DEBUGGING */
				HINT64 testOffset=0;
				(void) DI_MEDIA_MP_GetOffsetByTime (pstSession->nDeviceIdforSession,ulTime,DI_MEDIA_TIME_MSEC,&testOffset);
				PalMediaData(DBG_DATA, ("[%s] Accurate byte position : %lu	about msec %d\n",__FUNCTION__, testOffset, ulTime));
#endif
				/**
				 * Setting SeekMode
				 */
				eDiRet = P_MEDIA_SetSeekMode(unMediaId,pSettings->bAccurateSeek);
				if(eDiRet != DI_ERR_OK)
				{
					PrintError("[%s] Fail P_MEDIA_SetSeekMode\n", __FUNCTION__);
				}
				(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
			}
			else
			{
				PrintError("Can't Set MediaSettings (VK_SEM_Get failed!!)\n");
			}
		}

	}
	eDiRet = DI_IPTUNER_SetEndTime (pstTunerHdl, ulEndTime);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Fail Fail DI_IPTUNER_SetEndTime\n", __FUNCTION__);
	}

	/**
	 * Setting PlaybackStartParams
	 */
	eDiRet = DI_IPTUNER_SetPlaybackStartParams (pstTunerHdl, pSettings->tPlaybackStartParams.ulStartThreshold, pSettings->tPlaybackStartParams.ulStartConstant);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_SetBufferingMode\n", __FUNCTION__);
	}

	/**
	 * Setting BufferingMode
	 */
	eDiRet = DI_IPTUNER_SetBufferingMode (pstTunerHdl, pSettings->eBufferingMode);
	if (eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_SetBufferingMode\n", __FUNCTION__);
	}

	/**
	 * Setting BufferingModeParams
	 */
	eDiRet = DI_IPTUNER_SetBufferingModeParams (
				pstTunerHdl
				, pSettings->tBufferingParams.nBufferThreshold
				, pSettings->tBufferingParams.bFastFillIsLimited
				, pSettings->tBufferingParams.dFastFillLimitMultiple
				, pSettings->tBufferingParams.dManagedFillLimitMultiple);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_SetBufferingModeParams\n", __FUNCTION__);
	}

	/**
	 * Setting Adaptive
	 */
	if(pSettings->tAdaptiveSettings.eControl != DI_MEDIA_ADAPTIVE_NONE
	&& pSettings->tAdaptiveSettings.eControl != DI_MEDIA_ADAPTIVE_MAX)
	{
		eDiRet = DI_IPTUNER_SetAdaptiveSettings(pstTunerHdl,&pSettings->tAdaptiveSettings);
		if(eDiRet != DI_ERR_OK)
		{
			PrintError("[%s] Fail DI_IPTUNER_SetAdaptiveSettings\n", __FUNCTION__);
		}
		else
		{
			if(pSettings->tAdaptiveSettings.eControl != DI_MEDIA_ADAPTIVE_NONE
			&& pSettings->tAdaptiveSettings.eControl != DI_MEDIA_ADAPTIVE_SSTR_TEXT_STREAM
			&& pSettings->tAdaptiveSettings.eControl != DI_MEDIA_ADAPTIVE_SSTR_TEXT_TRACK
			&& pSettings->tAdaptiveSettings.eControl != DI_MEDIA_ADAPTIVE_MAX)
			{
				if(DI_MEDIA_GetPosition(pstTunerHdl, unMediaId, &ullPosition) == DI_ERR_OK)
				{
					(void)DI_MEDIA_SetPosition(pstTunerHdl, unMediaId, ullPosition);
				}
			}
		}
	}

	/**
	 * Setting Tls Certi & Key Path
	 */
	if(pSettings->tTlsSetting.pTls_ca_name != NULL)
	{
		if(pstTunerHdl->pTls_ca_name != NULL)
		{
			DD_MEM_Free(pstTunerHdl->pTls_ca_name);
			pstTunerHdl->pTls_ca_name = NULL;
		}

		pstTunerHdl->pTls_ca_name = VK_StrDup(pSettings->tTlsSetting.pTls_ca_name);
		PrintDebug("[%s] pstTunerHdl->pTls_ca_name=%s\n", __FUNCTION__, pstTunerHdl->pTls_ca_name);
	}

	if(pSettings->tTlsSetting.pTls_ca_path != NULL)
	{
		if(pstTunerHdl->pTls_ca_path != NULL)
		{
			DD_MEM_Free(pstTunerHdl->pTls_ca_path);
			pstTunerHdl->pTls_ca_path = NULL;
		}

		pstTunerHdl->pTls_ca_path = VK_StrDup(pSettings->tTlsSetting.pTls_ca_path);
		PrintDebug("[%s] pstTunerHdl->pTls_ca_path=%s\n", __FUNCTION__, pstTunerHdl->pTls_ca_path);
	}

	if(pSettings->tTlsSetting.pTls_certi_path != NULL)
	{
		if(pstTunerHdl->pTls_certi_path != NULL)
		{
			DD_MEM_Free(pstTunerHdl->pTls_certi_path);
			pstTunerHdl->pTls_certi_path = NULL;
		}

		pstTunerHdl->pTls_certi_path = VK_StrDup(pSettings->tTlsSetting.pTls_certi_path);
		PrintDebug("[%s] pstTunerHdl->pTls_certi_path=%s\n", __FUNCTION__, pstTunerHdl->pTls_certi_path);
	}

	if(pSettings->tTlsSetting.pTls_prikey_path != NULL)
	{
		if(pstTunerHdl->pTls_prikey_path != NULL)
		{
			DD_MEM_Free(pstTunerHdl->pTls_prikey_path);
			pstTunerHdl->pTls_prikey_path = NULL;
		}

		pstTunerHdl->pTls_prikey_path = VK_StrDup(pSettings->tTlsSetting.pTls_prikey_path);
		PrintDebug("[%s] pstTunerHdl->pTls_prikey_path=%s\n", __FUNCTION__, pstTunerHdl->pTls_prikey_path);
	}

	if(pSettings->tTlsSetting.pTls_pripw != NULL)
	{
		if(pstTunerHdl->pTls_pripw != NULL)
		{
			DD_MEM_Free(pstTunerHdl->pTls_pripw);
			pstTunerHdl->pTls_pripw = NULL;
		}

		pstTunerHdl->pTls_pripw = VK_StrDup(pSettings->tTlsSetting.pTls_pripw);
		PrintDebug("[%s] pstTunerHdl->pTls_pripw=%s\n", __FUNCTION__, pstTunerHdl->pTls_pripw);
	}

	if(pSettings->tTlsSetting.cbTlsCertficate.cbVerify != NULL)
	{
		(void)DI_IPTUNER_UnregisterTlsCertificateCB(pstTunerHdl);
		(void)DI_IPTUNER_RegisterTlsCertificateCB(pstTunerHdl, (DI_IPTUNER_VerifyExternalCallback_t *)&pSettings->tTlsSetting.cbTlsCertficate);
	}

	if(pSettings->tDtcpIpSetting.AkeHandle != NULL)
	{
		pstTunerHdl->tDtcpIpSetting.AkeHandle = pSettings->tDtcpIpSetting.AkeHandle;
	}

	if(pSettings->pProtocoalInfo != NULL)
	{
		PrintDebug("[%s] pProtocoalInfo=%s\n", __FUNCTION__, pSettings->pProtocoalInfo);
		eDiRet = P_MEDIA_DLNA_Protocolinfo_Parser(pstTunerHdl, pSettings->pProtocoalInfo);
		PrintDebug("### DtcpAppLib_DoAke eDiRet = %d, AkeHandle = x%x ###\n", eDiRet, pstTunerHdl->tDtcpIpSetting.AkeHandle);
		if(eDiRet != DI_ERR_OK)
		{
			pstTunerHdl->pProtocolinfo = NULL;
		}
		else
		{
			pstTunerHdl->pProtocolinfo = VK_StrDup(pSettings->pProtocoalInfo);
		}
	}
	else
	{
		pstTunerHdl->pProtocolinfo = NULL;
	}

	pstTunerHdl->tDlnaSetting.unDuration = pSettings->tDlnaSetting.unDuration;
	pstTunerHdl->tDlnaSetting.nFileSize = pSettings->tDlnaSetting.nFileSize;

	if(pSettings->tDlnaSetting.nByteRate > 0)
	{
		pstTunerHdl->tDlnaSetting.nByteRate = pSettings->tDlnaSetting.nByteRate;
	}
	else
	{
		if((pstTunerHdl->tDlnaSetting.nFileSize > 0) && (pstTunerHdl->tDlnaSetting.unDuration > 0))
		{
			pstTunerHdl->tDlnaSetting.nByteRate = pstTunerHdl->tDlnaSetting.nFileSize / (pSettings->tDlnaSetting.unDuration);
		}
		else
		{
//			pstTunerHdl->tDlnaSetting.nByteRate = DI_MEDIA_BITRATE_TEMP;
			pstTunerHdl->tDlnaSetting.nByteRate = 0;
		}
	}

	pstTunerHdl->nDemuxId = pSettings->nDemuxId;

	pstTunerHdl->bSuccesiveSeekForTrick = pSettings->bSuccesiveSeekForTrick;

	if((pSettings->tPRDYSetting.pCertificatePath) != NULL && (pSettings->tPRDYSetting.pPrivateKeyPath))
	{
		dResult = DRV_DRM_GetInfo(pHSession, &drm_info);

		drm_info.drm.playready.pPrivateKeyPath = pSettings->tPRDYSetting.pPrivateKeyPath;
		drm_info.drm.playready.pCertificatePath = pSettings->tPRDYSetting.pCertificatePath;

		dResult = DRV_DRM_ResigterCallback(pHSession, &drm_info);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_DRM_ResigterCallback failed!\n");
			return DRV_ERR;
		}
	}

	return DI_ERR_OK;
}

/*
 * 해당 Session의 Setting Parameter를 반환한다.
 * Parameter 미정의 상태
 */
DI_ERR_CODE DI_MEDIA_GetSessionSettings (SESSION_HANDLE pHSession, DI_MEDIA_SessionSettings_t *pstSettings)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	HUINT32 unMediaId;
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s]  Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	VK_MEM_Memset(pstSettings, 0, sizeof(DI_MEDIA_SessionSettings_t));

	if(P_MEDIA_FindMediaId((HUINT32)pHSession, &unMediaId) == DI_ERR_OK
	&& P_MEDIA_CheckValidMediaId(unMediaId) == DI_ERR_OK)
	{
		if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) == VK_OK)
		{
			pstSettings->bAccurateSeek = P_MEDIA_GetSeekMode(unMediaId);
			VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		}
		else
		{
			PrintError("Can't Get SeekMode (VK_SEM_Get failed!!)\n");
		}
	}
	(void) DI_IPTUNER_GetEndTime(pstTunerHdl, &pstSettings->ulEndTime);
#if 0
	/* DI_IPTUNER_GetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e *peConstraint) */
	(void) DI_IPTUNER_GetBufferingMode(pstTunerHdl, &pstSettings->eBufferingMode);
#else
	(void) DI_IPTUNER_GetBufferingMode(pstTunerHdl, (DI_IPTUNER_BufferingMode_e *)&pstSettings->eBufferingMode);
#endif
	(void) DI_IPTUNER_GetBufferingModeParams(pstTunerHdl, &pstSettings->tBufferingParams.nBufferThreshold,
														&pstSettings->tBufferingParams.bFastFillIsLimited,
														&pstSettings->tBufferingParams.dFastFillLimitMultiple,
														&pstSettings->tBufferingParams.dManagedFillLimitMultiple);

	(void) DI_IPTUNER_GetPlaybackStartParams(pstTunerHdl, &pstSettings->tPlaybackStartParams.ulStartThreshold,
											&pstSettings->tPlaybackStartParams.ulStartConstant);

	/**
	 * Getting Adaptive
	 */
	eDiRet = DI_IPTUNER_GetAdaptiveSettings(pstTunerHdl,&pstSettings->tAdaptiveSettings);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_GetAdaptiveSettings\n", __FUNCTION__);
	}
	return DI_ERR_OK;
}

/* 생성된 Session 의 Handle 을 입력받아 Buffering Session 인 경우에 대해
 * 현재 Buffer 의 Status 를 전달해 준다. 정보는 Buffer structure 를 사용한다.
 */
DI_ERR_CODE DI_MEDIA_GetBufferingStatus (SESSION_HANDLE pHSession, DI_MEDIA_BufferStatus_t *pstBufferStatus)
{
	DI_ERR_CODE eDiRet = DI_ERR_OK;
	HUINT32 unMediaId = 0;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DI_IPTUNER_Buffer_t stBuffer;
	HUINT32 nCurrentUsedBufferSize=0;
	HUINT32 nMaxBufferSize=0;
	HUINT32 nDecoderInternalBuffer=0;

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s]  Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}

	if( pstBufferStatus == NULL )
	{
		PrintError("[%s]  Fail pstBufferStatus is NULL\n", __FUNCTION__);
		return DI_ERR_ERROR;
	}
	if(P_MEDIA_FindMediaId((HUINT32)pHSession,&unMediaId) != DI_ERR_OK)
	{
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if(P_MEDIA_CheckValidMediaId(unMediaId) != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return DI_ERR_ERROR;
	}

	/* gathering internal buffer */
	(void) P_MEDIA_GetPlaybackBuffer (unMediaId, &nCurrentUsedBufferSize,&nMaxBufferSize);
	(void) P_MEDIA_GetAVStatus (unMediaId , &nDecoderInternalBuffer);

	eDiRet = DI_IPTUNER_GetBufferStatus (pstTunerHdl, nCurrentUsedBufferSize, nDecoderInternalBuffer, &stBuffer);
	if(eDiRet != DI_ERR_OK)
	{
		PrintError("[%s]  Fail DI_IPTUNER_GetBufferStatus\n", __FUNCTION__);
	}
	else
	{
		pstBufferStatus->llBufferedBytes = stBuffer.llBufferedBytes;
		pstBufferStatus->llTotalBytesRemaining = stBuffer.llTotalBytesRemaining;
		pstBufferStatus->llTotalStreamBytes = stBuffer.llTotalStreamBytes;

		pstBufferStatus->nArrivalBytedPerSecond = stBuffer.nArrivalBytedPerSecond;
		pstBufferStatus->nStreamBytesPerSecond = stBuffer.nStreamBytesPerSecond;
		pstBufferStatus->nBufferedMilliseconds = stBuffer.nBufferedMilliseconds;
		pstBufferStatus->uiBufferPercentage = stBuffer.uiBufferPercentage;
		pstBufferStatus->uiRemainBufferSec = stBuffer.uiRemainBufferSec;
		eDiRet = DI_ERR_OK;
#if 0 /* dragon test */
		PrintDebug ("\n");
		PrintDebug ("%s(%d) : ----------------------------------------------------\n",__FUNCTION__,__LINE__);
		PrintDebug ("%s(%d) : PAL BUFFERING STATUS Information \n",__FUNCTION__,__LINE__);
		PrintDebug ("%s(%d) : llBufferedBytes		 =%lld bytes\n",__FUNCTION__,__LINE__	  ,pstBufferStatus->llBufferedBytes);
		PrintDebug ("%s(%d) : llTotalBytesRemaining	 =%lld bytes\n",__FUNCTION__,__LINE__	  ,pstBufferStatus->llTotalBytesRemaining);
		PrintDebug ("%s(%d) : llTotalStreamBytes 	 =%lld bytes\n",__FUNCTION__,__LINE__	  ,pstBufferStatus->llTotalStreamBytes);
		PrintDebug ("%s(%d) : nArrivalBytedPerSecond  =%d bytes\n"		,__FUNCTION__,__LINE__,pstBufferStatus->nArrivalBytedPerSecond);
		PrintDebug ("%s(%d) : nStreamBytesPerSecond	 =%d bytes\n"		,__FUNCTION__,__LINE__,pstBufferStatus->nStreamBytesPerSecond);
		PrintDebug ("%s(%d) : nBufferedMilliseconds	 =%d millisecs\n"	,__FUNCTION__,__LINE__,pstBufferStatus->nBufferedMilliseconds);
		PrintDebug ("%s(%d) : ----------------------------------------------------",__FUNCTION__,__LINE__);
		PrintDebug ("\n");
#endif
	}
	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);

	return eDiRet;
}

/*
 * 생성된 Session 의 Handle 을 입력받아 Buffering 을 Start 하게 된다.
 * BufferType 에는 RAMBUFFERING, HDDBuffering 두가지가 있으나 HDD Buffering에만 해당된다.
 * HTTP,RTSP 등 모든 protocol controller 에 대해 동일한 기능을 수행한다.
 */
DI_ERR_CODE DI_MEDIA_StartBuffering (SESSION_HANDLE pHSession, HINT8 *pSaveFileName)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_StartBuffering (pstTunerHdl, pSaveFileName);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_StartBuffering\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 Session 의 Handle 을 입력받아 Buffering Task 를 정지시킨다.
 * 정지된 Task 는 더이상 Stream 을 Buffering 하지 않게 되며
 * 이에 대한 정보는 Event 를 통해 알아내야한다.
 */
DI_ERR_CODE DI_MEDIA_PauseBuffering (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_PauseBuffering (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_StartBuffering\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 Session 의 Handle 을 입력받아 Buffering Task 를 재시작한다.
 * 재시작시 상위에서 등록한 BufferingEvent 를 통해 BUFFERING_START 가 emit 된다.
 * 이에 대한 정보는 Event 를 통해 알아내야한다.
 */
DI_ERR_CODE DI_MEDIA_ResumeBuffering (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_ResumeBuffering (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_StartBuffering\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 Session 의 Handle 을 입력받아 Buffering Task 를 종료한다.
 * 종료되지 않은 content 일지라도 강제로 Buffering 을 Stop 시키며
 * Buffering 이 종료되면 Playback 에 install 된 VFIO 를 통해서도 EOF 발생시기를 조절해야한다.
 */
DI_ERR_CODE DI_MEDIA_StopBuffering (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_StopBuffering (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_StartBuffering\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 Session 의 Handle 을 입력받아 Probe Event 를 수신할 수 있도록
 * Callback Function 을 install 한다.
 */
DI_ERR_CODE DI_MEDIA_RegisterProbeEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_ProbeCallback_t *pProbeCallbacks)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	DI_IPTUNER_ProbeCallback_t tCallbacks;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	tCallbacks.cbProbeComplete = (DI_IPTUNER_ProbeCompleteCallback) pProbeCallbacks->cbProbeComplete;
	tCallbacks.cbProbeFailure  = (DI_IPTUNER_ProbeFailureCallback) pProbeCallbacks->cbProbeFailure;

#if 0
	/* DI_IPTUNER_RegisterProbeEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_ProbeCallback_t *pCallbacks) */
	eDIError = DI_IPTUNER_RegisterProbeEventCB (pstTunerHdl, pProbeCallbacks);
#else
	eDIError = DI_IPTUNER_RegisterProbeEventCB (pstTunerHdl, &tCallbacks);
#endif
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_RegisterProbeEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Install 된 Probe Event Callback Function을 해제 한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterProbeEventCallback (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_UnregisterProbeEventCB (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_UnregisterProbeEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}


/* 생성된 Session 의 Handle 을 입력받아 Buffering Event 를 수신할 수 있도록
 * Callback Function 을 install 한다. 이 Function 은 Buffering Task 에서
 * 특정 Event 가 발생했을 때마다 호출해주게 되며 Event 의 type 은
 * enum 값으로 define 한다.
 */
DI_ERR_CODE DI_MEDIA_RegisterBufferingEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_BufferCallback_t *pBufferCallbacks)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
#if 1
	if(pstTunerHdl->tDlnaSetting.bDlna == TRUE)
	{
		PrintDebug("[%s] Debug> Buffering Event is not used on DLNA\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_OK;
	}
#endif
#if 0
	/* DI_IPTUNER_RegisterBufEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferCallback_t *pCallbacks); */
	eDIError = DI_IPTUNER_RegisterBufEventCB (pstTunerHdl, pBufferCallbacks);
#else
	eDIError = DI_IPTUNER_RegisterBufEventCB (pstTunerHdl, (DI_IPTUNER_BufferCallback_t *)pBufferCallbacks);
#endif
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Install 된 Buffering Event Callback Function을 해제 한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterBufferingEventCallback (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_UnregisterBufEventCB (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Adaptive Specific Event를 수신할 수 있도록
 * Callback Function 을 install 한다.
 */
DI_ERR_CODE DI_MEDIA_RegisterAdaptiveEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_AdaptiveCallback_t *pAdaptiveCallbacks)
{
#if 0
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
        eDiRet = DI_IPTUNER_RegisterBufEventCB (pstTunerHdl, pBufferCallbacks);
        if(eDiRet != DI_ERR_OK)
        {
		PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
       		return eDiRet;
        }
	PrintExit;
#else
	UNUSED_PARAMETER(pHSession);
	UNUSED_PARAMETER(pAdaptiveCallbacks);
#endif
	return DI_ERR_OK;
}


/*
 * Install 된 Adaptive Specific Event Callback Function을 해제 한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterAdaptiveEventCallback (SESSION_HANDLE pHSession)
{
#if 0
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
        eDiRet = DI_IPTUNER_UnregisterBufEventCB (pstTunerHdl);
        if(eDiRet != DI_ERR_OK)
        {
		PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
       		return eDiRet;
        }
	PrintExit;
#else
	UNUSED_PARAMETER(pHSession);
#endif
	return DI_ERR_OK;
}


/*
 * Streaming 관련 Event(Underrun/Overrun/Start/Stop/...) 를 수신할 수 있도록
 * Callback Function 을 install 한다.
 */
DI_ERR_CODE DI_MEDIA_RegisterStreamEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_StreamEventCallback_t *pStreamEventCallbacks)
{
#if 0
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
        eDiRet = DI_IPTUNER_RegisterBufEventCB (pstTunerHdl, pBufferCallbacks);
        if(eDiRet != DI_ERR_OK)
        {
		PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
       		return eDiRet;
        }
#else
	UNUSED_PARAMETER(pHSession);
	UNUSED_PARAMETER(pStreamEventCallbacks);
#endif
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Install 된 Streaming Event Callback Function을 해제 한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterStreamEventCallback (SESSION_HANDLE pHSession)
{
#if 0
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
        eDiRet = DI_IPTUNER_UnregisterBufEventCB (pstTunerHdl);
        if(eDiRet != DI_ERR_OK)
        {
			PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
       		return eDiRet;
        }
#else
	UNUSED_PARAMETER(pHSession);
#endif
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Media Event 를 수신할 수 있도록
 * Callback Function 을 install 한다.
 */
DI_ERR_CODE DI_MEDIA_RegisterMediaEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_EventCallback_t MediaEventCallback)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(MediaEventCallback == NULL)
	{
		PrintError("[%s] Error - MediaEventcallback is null.\n", __func__);
		return DI_ERR_ERROR;
	}
	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}
	s_tEventCallback[unMediaId] = MediaEventCallback;

	/* register EventCB to IPTUNER Module */
	DI_IPTUNER_RegisterInetRadioChangeTitleCB(pstTunerHdl,P_MEDIA_InetRadioChangeTitleCallback);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return eDIError;
}

/*
 * Install 된 Media Event Callback Function을 해제 한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterMediaEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}
	if(s_tEventCallback[unMediaId] != NULL)
	{
		s_tEventCallback[unMediaId] = NULL;
	}

	/* register EventCB to IPTUNER Module */
	DI_IPTUNER_UnregisterInetRadioChangeTitleCB(pstTunerHdl);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_RegisterLiveBufferEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_EventCallback_t MediaEventCallback)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}

	if (0) // Already done at MediaEvent
	{
		if (s_tEventCallback[unMediaId] == NULL)
		{
			s_tEventCallback[unMediaId] = MediaEventCallback;
		}
	}

	/* register EventCB to IPTUNER Module */
	DI_IPTUNER_RegisterLiveBufferEventCB(pstTunerHdl,P_MEDIA_LiveBufferEventCallback);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Install 된 Underflow Event Callback Function을 해제 한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterLiveBufferEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId)
{
	DI_ERR_CODE eDIError = DI_ERR_OK;
	DI_IPTUNER_t *pstTunerHdl = NULL;
	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_tMediaInstance[unMediaId].ulMediaSemId) != VK_OK)
	{
		PrintError("%s(%d) VK_SEM_Get failed!\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}
	eDIError = P_MEDIA_CheckValidMediaId(unMediaId);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Error - Invalid Media Id.\n", __func__);
		(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
		return eDIError;
	}

	if (0) // Already done at MediaEvent
	{
		if(s_tEventCallback[unMediaId] != NULL)
		{
			s_tEventCallback[unMediaId] = NULL;
		}
	}

	/* register EventCB to IPTUNER Module */
	DI_IPTUNER_UnregisterLiveBufferEventCB(pstTunerHdl);

	(void)VK_SEM_Release(s_tMediaInstance[unMediaId].ulMediaSemId);
	PrintExit;
	return DI_ERR_OK;
}

/*
 * 생성된 Session 의 Handle 을 입력받아 Channel Event 를 수신할 수 있도록
 * Callback Function 을 install 한다.
 */
DI_ERR_CODE DI_MEDIA_RegisterChannelEventCallback (SESSION_HANDLE pHSession,
				DI_MEDIA_ChannelTunerType_e eTunerType,
				DI_MEDIA_ChannelEventCallback_t *pChannelEventCallbacks)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;
#ifdef __MULTICAST_IPTUNER_OWN_CHANNEL_CALLBACK__ /* There is no iptuner's own channel callback; Just use this media callback */
	DI_IPTUNER_ChannelEventCallback_t tChannelCallbacks;
#endif

	PrintEnter;
	if (pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if (pstTunerHdl == NULL)
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById!!Handle : %d\n", __FUNCTION__, (int)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

#ifdef __MULTICAST_IPTUNER_OWN_CHANNEL_CALLBACK__
	tChannelCallbacks.cbTuningEvent = (DI_IPTUNER_ChannelTuningCallback) pChannelEventCallbacks->cbTuningEvent;
	tChannelCallbacks.cbNotifyEvent = (DI_IPTUNER_ChannelNotifyCallback) pChannelEventCallbacks->cbNotifyEvent;
#endif

	eDIError = DI_IPTUNER_RegisterChannelEventCB (pstTunerHdl,
					eTunerType,
					pChannelEventCallbacks);
	if (eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_RegisterChannelEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

/*
 * Install 된 Channel Event Callback Function을 해제한다.
 */
DI_ERR_CODE DI_MEDIA_UnregisterChannelEventCallback (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;
	if (pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if (pstTunerHdl == NULL)
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById!!Handle : %d\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	eDIError = DI_IPTUNER_UnregisterChannelEventCB (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_UnregisterChannelEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_RegisterErrorEventCB (SESSION_HANDLE pHSession, DI_MEDIA_ErrorEventCallback pErrEventCallback)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}


	eDIError = DI_IPTUNER_RegisterErrorEventCB (pstTunerHdl, (DI_IPTUNER_ErrorEventCallback)pErrEventCallback);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_RegisterBufferEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_UnregisterErrorEventCB (SESSION_HANDLE pHSession)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;
	if (pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if (pstTunerHdl == NULL)
	{
		PrintError("[%s] Fail DI_IPTUNER_POOL_FindById!!Handle : %d\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}

	eDIError = DI_IPTUNER_UnregisterErrorEventCB (pstTunerHdl);
	if(eDIError != DI_ERR_OK)
	{
		PrintError("[%s] Fail DI_IPTUNER_UnregisterChannelEventCB\n", __FUNCTION__);
		PrintExit;
		return eDIError;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_RegisterLicenseDataCallback(SESSION_HANDLE pHSession, DI_MEDIA_LicenseDataCallback_t callback)
{
	DRV_Error dResult;

	PrintEnter;
	if (pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	dResult = DRV_DRM_RegisterLicenseDataCallback(pHSession, callback);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Fail DRV_DRM_RegisterLicenseDataCallback\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_UnregisterLicenseDataCallback(SESSION_HANDLE pHSession)
{
	DRV_Error dResult;

	PrintEnter;
	if (pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	dResult = DRV_DRM_RegisterLicenseDataCallback(pHSession, NULL);
	if(dResult != DRV_OK)
	{
		PrintError("[%s] Fail DRV_DRM_RegisterLicenseDataCallback\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_SetLicenseData(SESSION_HANDLE pHSession, HUINT8 *pucLicenseData)
{
	DRV_Error dResult;

	PrintEnter;

	dResult = DRV_DRM_SetLicenseData(pHSession, pucLicenseData);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_DRM_SetLicenseData failed!\n");
		return DI_ERR_ERROR;
	}
	PrintExit;
	return DI_ERR_OK;
}

static void P_MEDIA_TSInjectorInit(void)
{
	HINT32 i = 0x0;
	int nerr = 0x0;
	unsigned long uSemID = 0x0;
	NEXUS_PlaypumpHandle tPlaypumpHandle = NULL;

	PrintEnter;

	if (s_bInitTSInjector == TRUE)
	{
		PrintError("[%s] Error - Already initialized\n", __FUNCTION__);
		goto func_error;
	}

	/* VK_MEM_Memset() or VK_memset() */
	VK_memset(s_tMediaTSInjector, 0x0, sizeof(DI_MediaTSInjector)*_DEF_MEDIA_MAX_INJ_NUM);
	for (i=0; i<_DEF_MEDIA_MAX_INJ_NUM; i++)
	{
		/* DEFAULT_SUSPEND_TYPE(VK_SUSPENDTYPE_PRIORITY) or VK_SUSPENDTYPE_FIFO */
		nerr = VK_SEM_Create(&uSemID, "di_media_tsinj", DEFAULT_SUSPEND_TYPE);
		if (nerr != VK_OK)
		{
			PrintError("[%s] Error> (0x%x) in VK_SEM_Create()\n", __FUNCTION__, (unsigned int)nerr);
			return;
		}

		PrintDebug("[%s] TSInjector[%d] : uSemID(0x%x)\n", __FUNCTION__, i, (unsigned int)uSemID);
		s_tMediaTSInjector[i].uSemID = uSemID;
		s_tMediaTSInjector[i].tPlaypump = tPlaypumpHandle;
		s_tMediaTSInjector[i].currTransportType = NEXUS_TransportType_eTs;
		s_tMediaTSInjector[i].prevTransportType = NEXUS_TransportType_eTs;
		s_tMediaTSInjector[i].currPlaypumpMode = NEXUS_PlaypumpMode_eFifo;
		s_tMediaTSInjector[i].prevPlaypumpMode = NEXUS_PlaypumpMode_eFifo;
		s_tMediaTSInjector[i].tEvent = NULL;
		s_tMediaTSInjector[i].bRunning = FALSE;

		s_tMediaTSInjector[i].hVideoPidChn     = NULL;
		s_tMediaTSInjector[i].hAudioMainPidChn = NULL;
		s_tMediaTSInjector[i].hAudioSubPidChn  = NULL;
		s_tMediaTSInjector[i].uVideoPid        = 0x0;
		s_tMediaTSInjector[i].uAudioMainPid    = 0x0;
		s_tMediaTSInjector[i].uAudioSubPid     = 0x0;
		s_tMediaTSInjector[i].uPcrPid          = 0x0;
	}
	s_bInitTSInjector = TRUE;

func_error:
	PrintExit;
	return;
}

/* Reference P_MEDIA_GetMediaId() */
static DI_ERR_CODE P_MEDIA_TSInjectorGetID(HUINT32 *punTSInjectorID)
{
	DI_ERR_CODE		eDIError = DI_ERR_OK;
	HUINT32			i=0;

	PrintEnter;

	*punTSInjectorID = _DEF_MEDIA_INVALID_INJ_NUM;
	for (i=0; i<_DEF_MEDIA_MAX_INJ_NUM; i++)
	{
		if (VK_SEM_Get(s_tMediaTSInjector[i].uSemID) == VK_OK)
		{
			if(s_tMediaTSInjector[i].bRunning == FALSE)
			{
				*punTSInjectorID = i;
				s_tMediaTSInjector[i].bRunning = TRUE;
				VK_SEM_Release(s_tMediaTSInjector[i].uSemID);
				break;
			}
			VK_SEM_Release(s_tMediaTSInjector[i].uSemID);
		}
	}

	if(i == _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> INJ ID is full.\n", __FUNCTION__);
		eDIError = DI_ERR_ERROR;
	}

	PrintExit;
	return eDIError;
}

static void P_MEDIA_TSInjectorDataEventCallback(void *context, int param)
{
	UNUSED_PARAMETER(param);

	if (context != NULL)
	{
		/* Check _DEF_MEDIA_INJ_WAIT_EVTTIME */
		BKNI_SetEvent((BKNI_EventHandle)context);
	}
}

/* (SESSION_HANDLE pHSession) */
static DI_ERR_CODE P_MEDIA_TSInjectorOpen(HUINT32 uTSInjID, HINT8 *pURL)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	DRV_Error drvErr = DRV_ERR;
   	NEXUS_PlaypumpSettings pumpSets;
   	/*NEXUS_PlaypumpOpenSettings pumpOpenSets;*/
	NEXUS_PlaypumpHandle tPlaypumpHandle = NULL;
	BKNI_EventHandle tEvent = NULL;
	NEXUS_Error neErr;

	PrintEnter;

	UNUSED_PARAMETER(pURL);

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if (s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
		{
			PrintError("[%s] Error> Already deactivated Injector(ID:%d)!\n",
						__FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		drvErr = DRV_PVR_GetPlaypumpHandle(&tPlaypumpHandle);
		if ((drvErr != DRV_OK) || (tPlaypumpHandle == NULL))
		{
			PrintError("[%s] Error> Cant get playpump!\n", __FUNCTION__);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		BKNI_CreateEvent(&tEvent);

		NEXUS_Playpump_GetSettings(tPlaypumpHandle, &pumpSets);
		/* For Live Modes, IP Applib directly feeds the network data to the Nexus Playpump */
		/*
		pumpOpenSets.fifoSize *= 2;
		pumpOpenSets.numDescriptors = 200;
		*/

		/* Store prev pump settings when 1st getSettings() */
		s_tMediaTSInjector[uTSInjID].bRestorePumpSettings = FALSE;
		s_tMediaTSInjector[uTSInjID].prevTransportType = pumpSets.transportType;
		s_tMediaTSInjector[uTSInjID].prevPlaypumpMode  = pumpSets.mode;

		/* Uses the FIFO mode to feed data */
		pumpSets.transportType = NEXUS_TransportType_eTs;
		pumpSets.mode		  = NEXUS_PlaypumpMode_eFifo;

		/* Store current pump settings for restoring */
		s_tMediaTSInjector[uTSInjID].currTransportType = pumpSets.transportType;
		s_tMediaTSInjector[uTSInjID].currPlaypumpMode  = pumpSets.mode;
		if ((s_tMediaTSInjector[uTSInjID].prevTransportType != s_tMediaTSInjector[uTSInjID].currTransportType)
			|| (s_tMediaTSInjector[uTSInjID].currPlaypumpMode != s_tMediaTSInjector[uTSInjID].currPlaypumpMode))
		{
			s_tMediaTSInjector[uTSInjID].bRestorePumpSettings = TRUE;
		}

		pumpSets.dataCallback.callback = P_MEDIA_TSInjectorDataEventCallback;
		pumpSets.dataCallback.context  = tEvent;
		neErr = NEXUS_Playpump_SetSettings(tPlaypumpHandle, &pumpSets);
		if (neErr != NEXUS_SUCCESS)
		{
			PrintError("%s(%d) Error> (err:%d) Cant get playpump settings\n",__FUNCTION__,__LINE__, neErr);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		s_tMediaTSInjector[uTSInjID].tPlaypump = tPlaypumpHandle;
		s_tMediaTSInjector[uTSInjID].tEvent	= tEvent;
		if (s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
		{
			s_tMediaTSInjector[uTSInjID].bRunning = TRUE;
		}
		eDIError = DI_ERR_OK;
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

func_error:
	PrintExit;
	return eDIError;
}


/* (SESSION_HANDLE pHSession) */
static DI_ERR_CODE P_MEDIA_TSInjectorStart(HUINT32 uTSInjID)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
   	NEXUS_Error neErr;

	PrintEnter;

	if (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid Media ID(%d)\n",
					__FUNCTION__, uTSInjID);
		goto func_error;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if (s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
		{
			PrintError("[%s] Error> Already deactivated Injector(ID:%d)!\n",
						__FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		if (s_tMediaTSInjector[uTSInjID].tPlaypump == NULL)
		{
			PrintError("[%s] Error> Invalid playpump(ID:%d)!\n",
						__FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		/* iptuner_mcast based */
		PrintError("[%s] --> +NEXUS_Playpump_Start(ID:%d)\n", __FUNCTION__, uTSInjID);
		neErr = NEXUS_Playpump_Start(s_tMediaTSInjector[uTSInjID].tPlaypump);
		PrintError("[%s] --> -NEXUS_Playpump_Start(ID:%d)\n", __FUNCTION__, uTSInjID);

		if (neErr != NEXUS_SUCCESS)
		{
			PrintError("[%s:%d] Error> playpump start failed, err:%d\n", __FUNCTION__, __LINE__, neErr);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}
		PrintError("[%s:%d] playpump is started\n", __FUNCTION__, __LINE__);
		eDIError = DI_ERR_OK;
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

func_error:
	PrintExit;
	return eDIError;
}

static DI_ERR_CODE P_MEDIA_TSInjectorStop(HUINT32 uTSInjID)
{
	DI_ERR_CODE eDIError = DI_ERR_ERROR;
	/*DRV_Error drvErr = DRV_ERR;*/

	PrintEnter;

	if (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid Media ID(%d)\n",
					__FUNCTION__, uTSInjID);
		goto func_error;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if(s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
		{
			PrintError("[%s] Error> Already deactivated Injector(ID:%d)\n",
						__FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		if (s_tMediaTSInjector[uTSInjID].tPlaypump == NULL)
		{
			PrintError("[%s] Error> Invalid playpump(ID:%d)!\n",
						__FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		NEXUS_Playpump_Stop(s_tMediaTSInjector[uTSInjID].tPlaypump);
		PrintError("[%s:%d] playpump is stopped\n", __FUNCTION__, __LINE__);
		eDIError = DI_ERR_OK;
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

func_error:
	PrintExit;
	return eDIError;
}

static DI_ERR_CODE P_MEDIA_TSInjectorClose(HUINT32 uTSInjID)
{
	DI_ERR_CODE eDIError   = DI_ERR_ERROR;
	DRV_Error drvErr	= DRV_ERR;
	HBOOL bRestorePumpMode = FALSE;
	HBOOL bRestorePumpTS   = FALSE;
	NEXUS_PlaypumpSettings pumpSets;

	PrintEnter;

	if (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid Media ID(%d)\n",
					__FUNCTION__, uTSInjID);
		goto func_error;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if(s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
		{
			PrintError("[%s] Error> Already deactivated Injector(ID:%d)\n",
						__FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			goto func_error;
		}

		bRestorePumpMode = bRestorePumpTS = FALSE;
		if (s_tMediaTSInjector[uTSInjID].tPlaypump != NULL)
		{
			if (s_tMediaTSInjector[uTSInjID].currTransportType != s_tMediaTSInjector[uTSInjID].prevTransportType)
			{
				bRestorePumpTS = TRUE;
			}

			if (s_tMediaTSInjector[uTSInjID].currPlaypumpMode != s_tMediaTSInjector[uTSInjID].prevPlaypumpMode)
			{
				bRestorePumpMode = TRUE;
			}

			if ((bRestorePumpTS == TRUE) || (bRestorePumpMode == TRUE))
			{
				NEXUS_Playpump_GetSettings(s_tMediaTSInjector[uTSInjID].tPlaypump, &pumpSets);
				if (bRestorePumpTS == TRUE)
				{
					pumpSets.transportType = s_tMediaTSInjector[uTSInjID].prevTransportType;
				}
				if (bRestorePumpMode == TRUE)
				{
					pumpSets.mode = s_tMediaTSInjector[uTSInjID].prevPlaypumpMode;
				}
				(void)NEXUS_Playpump_SetSettings(s_tMediaTSInjector[uTSInjID].tPlaypump, &pumpSets);

				s_tMediaTSInjector[uTSInjID].currTransportType = NEXUS_TransportType_eTs;
				s_tMediaTSInjector[uTSInjID].prevTransportType = NEXUS_TransportType_eTs;
				s_tMediaTSInjector[uTSInjID].currPlaypumpMode = NEXUS_PlaypumpMode_eFifo;
				s_tMediaTSInjector[uTSInjID].prevPlaypumpMode = NEXUS_PlaypumpMode_eFifo;
			}

			drvErr = DRV_PVR_ReleasePlaypumpHandle(s_tMediaTSInjector[uTSInjID].tPlaypump);
			if (drvErr != DRV_OK)
			{
				PrintError("[%s] Error> Failed to release playpump!\n", __FUNCTION__);
				VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
				goto func_error;
			}
			s_tMediaTSInjector[uTSInjID].tPlaypump = NULL;
		}

		if (s_tMediaTSInjector[uTSInjID].tEvent != NULL)
		{
			BKNI_DestroyEvent(s_tMediaTSInjector[uTSInjID].tEvent);
			s_tMediaTSInjector[uTSInjID].tEvent = NULL;
		}

		s_tMediaTSInjector[uTSInjID].bRunning = FALSE;
		eDIError = DI_ERR_OK;
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

func_error:
	PrintExit;
	return eDIError;
}

static void P_MEDIA_TSInjectorDataReceivedCallback (HUINT32 uInjID, HUINT8 *pBuffer, HUINT32 uBufLen) /* (DI_IPTUNER_t *pHTuner) */
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	HBOOL	bPrintDbg   = TRUE;
	HUINT32	uGetBufSize = 0;
	HUINT32	uRemainSize = 0;
	void	*pGetBuf    = NULL;
	HUINT8	*pCurBuf    = NULL;
	HUINT32	uTSInjID    = 0;
	HUINT32	uOverflCnt  = 0;
	HUINT32	uLoopCnt    = 0;
	HUINT32	uMaxLoopCnt = _DEF_MEDIA_INJ_MAX_LOOPCNT;
#if defined(__TSINJECT_CADRM_DECRYPT__)
	unsigned long ulDrmErr = 0;
#endif
	/*
	 * DESCRIPTION :
	 *   There are two way to implement the GetBuffer and WaitEvent.
	 *   In the examples code, WaitEvent was called only the time when buffer size was 0.
	 *   But event (available) was called everytime without reference to buffer size.
	 *   It's mean that event will be invoked every time when there are available spaces.
	 *   So for more precise implementation,
	 *   we need to match the sync between GetBuffer and Event.
	 *   Event will be stacked in the queue if you not call the WaitEvent.
	 *   We need to call WaitEvent after every call of GetBuffer for the make the sync
	 */
	HBOOL bWaitGetEvent = TRUE;
	/*DI_IPTUNER_t *pstTunerHdl = NULL;*/

	//PrintEnter;

	/* Avoid unSessionId based iptuner search */
	/*
	DI_IPTUNER_t *pHTuner = NULL;
	HBOOL bFindPool = TRUE;

	if (bFindPool == TRUE)
	{
		pHTuner = DI_IPTUNER_POOL_FindById(unSessionId);
		if (pHTuner == NULL)
		{
			PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById\n", __FUNCTION__);
			PrintExit;
			return;
		}
		uTSInjID = pHTuner->uTSInjID;
	}
	*/

	uTSInjID = uInjID;
	if (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid InjID(%d)\n", __FUNCTION__, uTSInjID);
		goto func_error;
	}

	if ((pBuffer == NULL) || (uBufLen == 0))
	{
		PrintError("[%s] Error> Invalid NULL buffer(blen:%d)\n", __FUNCTION__, uBufLen);
		goto func_error;
	}

	/* if CA/DRM decrypt is processed here... */
#if defined(__TSINJECT_CADRM_DECRYPT__)
	ulDrmErr = SOME_CADRM_Decrypt((unsigned char *)pBuffer, iRecvLen);
	if (ulDrmErr != 0)
	{
		switch(ulDrmErr) {
		case SOME_CADRM_ERR_INCORRECT_TS_PACKET_SIZE :
			break;
		case SOME_CADRM_ERR_INCORRECT_TS_PACKET :
			break;
		case SOME_CADRM_ERR_SCRAMBLED_TS_IN_BUFF :
			break;
		default:
			break;
		}
	}
#endif

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		uRemainSize = uBufLen;
		pCurBuf     = pBuffer;
		while (1)
		{
			if (s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
			{
				PrintError("[%s] Error> Invalid Injector status(ID:%d)\n", __FUNCTION__, uTSInjID);
				break;
			}

			if (s_tMediaTSInjector[uTSInjID].tPlaypump == NULL)
			{
				PrintError("[%s:%d] Error> Invalid NULL playpump(ID:%d)\n", __FUNCTION__, __LINE__, uTSInjID);
				break;
			}

			if ((uMaxLoopCnt) && (uLoopCnt > uMaxLoopCnt))
			{
				PrintError("[%s] Error> Exceed max loop count!\n", __FUNCTION__);
				break;
			}

			if (NEXUS_Playpump_GetBuffer(s_tMediaTSInjector[uTSInjID].tPlaypump,
							&pGetBuf, (size_t*)&uGetBufSize))
			{
				PrintError("[%s] Error> GetBuffer failed!\n", __FUNCTION__);
				break;
			}

			if (bWaitGetEvent == TRUE)
			{
				VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
				rc = BKNI_WaitForEvent(s_tMediaTSInjector[uTSInjID].tEvent, _DEF_MEDIA_INJ_WAIT_EVTTIME);
				if(rc)
				{
					PrintError ("[%s:%d] Error> BKNI_WaitForEvent Fail \n",__FUNCTION__,__LINE__);
					break;
				}
				(void)VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID);
			}

			if (uGetBufSize >= uRemainSize)
			{
				/* Success: Enough buffer size */
				(void)VK_memcpy(pGetBuf, pCurBuf, uRemainSize);
				rc = NEXUS_Playpump_ReadComplete(s_tMediaTSInjector[uTSInjID].tPlaypump, 0, uRemainSize); /* CID 10322 */
				if (rc != NEXUS_SUCCESS)
				{
					PrintError("[%s:%d] Error> ReadComplete failed(%d)\n", __FUNCTION__, __LINE__, uRemainSize);
					/* Just post error and don't return the failure */
				}

				if ((bPrintDbg == TRUE) && (uLoopCnt > 0))
				{
					PrintDebug("[%s:%d] readComplete(getbuf:%u, remain:%u, loop:%u)\n",
								__FUNCTION__, __LINE__,
								uGetBufSize, uRemainSize, uLoopCnt);
				}
				break;
			}
			else if (uGetBufSize == 0)
			{
				/* Buffer Full */
				uOverflCnt++;
				if (bPrintDbg == TRUE) {
					PrintError("[%s] Error> getbuf size 0\n", __FUNCTION__);
				}

				if (bWaitGetEvent == FALSE)
				{
					PrintError("[%s] Wait event(loop:%u)\n", __FUNCTION__, uLoopCnt);
					VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
					rc = BKNI_WaitForEvent(s_tMediaTSInjector[uTSInjID].tEvent, _DEF_MEDIA_INJ_WAIT_EVTTIME);
					if(rc)
					{
						PrintError ("[%s:%d] Error> BKNI_WaitForEvent Fail \n",__FUNCTION__,__LINE__);
						break;
					}
					(void)VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID);
				}

				if (uOverflCnt > _DEF_MEDIA_INJ_MAX_OVERFLOWCNT)
				{
					PrintError("[%s] Error> pump overflow(cnt:%u)!\n", __FUNCTION__, uOverflCnt);
					break;
				}
			}
			else
			{
				/* Too small buffer size */
				(void)VK_memcpy(pGetBuf, pCurBuf, uGetBufSize);
				rc = NEXUS_Playpump_ReadComplete(s_tMediaTSInjector[uTSInjID].tPlaypump, 0, uGetBufSize); /* CID 10322 */
				if (rc != NEXUS_SUCCESS)
				{
					PrintError("[%s:%d] Error> ReadComplete failed(%d)\n", __FUNCTION__, __LINE__, uGetBufSize);
					/* Just post error and don't return the failure */
				}
				else
				{
					uRemainSize = uRemainSize - uGetBufSize;
					pCurBuf += uGetBufSize;
				}

				if (bPrintDbg == TRUE) {
					PrintDebug("[%s:%d] readComplete(getbuf:%u, remain:%u, loop:%u)\n",
								__FUNCTION__, __LINE__,
								uGetBufSize, uRemainSize, uLoopCnt);
				}
			}

			uLoopCnt++;
		} /* while() */
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}
func_error:
	//PrintExit;
	return;
}

// tsInjID is deviceId
DRV_Error DRV_MEDIA_OpenMulticastPidChannel(HUINT32 uTSInjID, DI_DEMUX_CHANNELTYPE eChannelType, HUINT16 usPid, NEXUS_PidChannelHandle *phPidChannel)  // <-- from di_demux
{
	NEXUS_PlaypumpOpenPidChannelSettings openPidSettings; /* Not playback setting */

	PrintEnter;
	PrintDebug("[%s] ++[devID:%d, chnType:%d, pid:%d]\n", __FUNCTION__, uTSInjID, eChannelType, usPid);

	*phPidChannel = NULL;

	if (s_bInitTSInjector == FALSE)
	{
		PrintError("[%s] Error - Uninitialized TS injector\n", __FUNCTION__);
		P_MEDIA_TSInjectorInit();
	}

	if (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid device ID! (devID:%d)\n", __FUNCTION__, uTSInjID);
		return DRV_ERR;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if (s_tMediaTSInjector[uTSInjID].bRunning == TRUE)
		{
			PrintError("[%s] Error> Already running! (status : %d)\n",
						__FUNCTION__, s_tMediaTSInjector[uTSInjID].bRunning);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		if (s_tMediaTSInjector[uTSInjID].tPlaypump == NULL)
		{
			PrintError("[%s] Error> Invalid NULL playpump(ID:%d)\n", __FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&openPidSettings);
		switch(eChannelType) {
		case DI_DEMUX_CHANNELTYPE_VIDEO :
			if (!s_tMediaTSInjector[uTSInjID].hVideoPidChn)
			{
#if (NEXUS_VERSION >= 1250)
				openPidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
				openPidSettings.pidType = NEXUS_PidType_eVideo;
				s_tMediaTSInjector[uTSInjID].uVideoPid = usPid;
				s_tMediaTSInjector[uTSInjID].videoSet.codec = NEXUS_VideoCodec_eUnknown;
				s_tMediaTSInjector[uTSInjID].videoSet.decoder = NULL; /* Decode will set this later */
			}
			else
			{
				PrintError("[%s] Error> video PID channel is already opened!\n", __FUNCTION__);
				VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
				return DRV_ERR;
			}
			break;
		case DI_DEMUX_CHANNELTYPE_AUDIO :
		case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
			if (!s_tMediaTSInjector[uTSInjID].hAudioMainPidChn)
			{
#if (NEXUS_VERSION >= 1250)
				openPidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
				openPidSettings.pidType = NEXUS_PidType_eAudio;
				openPidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
				s_tMediaTSInjector[uTSInjID].uAudioMainPid = usPid;
				s_tMediaTSInjector[uTSInjID].audioSet.primary = NULL;/* Decode will set this later */
				s_tMediaTSInjector[uTSInjID].audioSet.secondary = NULL; /* Decode will set this later */
			}
			else
			{
				PrintError("[%s] Error> audio main PID channel is already opened!\n", __FUNCTION__);
				//VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
				return DRV_ERR;
			}
			break;
		case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
			if (!s_tMediaTSInjector[uTSInjID].hAudioSubPidChn)
			{
#if (NEXUS_VERSION >= 1250)
				openPidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE ;
#endif
				openPidSettings.pidType = NEXUS_PidType_eAudio;
				openPidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* audio.codec is required for VOB substream id remapping. So ignored. */
				s_tMediaTSInjector[uTSInjID].uAudioSubPid = usPid;
				s_tMediaTSInjector[uTSInjID].audioSet.primary = NULL;/* Decode will set this later */
				s_tMediaTSInjector[uTSInjID].audioSet.secondary = NULL; /* Decode will set this later */
			}
			else
			{
				PrintError("[%s] Error> audio sub PID channel is already opened!\n", __FUNCTION__);
				VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
				return DRV_ERR;
			}
			break;
		case DI_DEMUX_CHANNELTYPE_PCR :
			//openPidSettings.uPcrPid = usPid;
			s_tMediaTSInjector[uTSInjID].uPcrPid = usPid;
			*phPidChannel = DRV_DEMUX_GetPcrPidChannel();
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_OK;
		default :
			openPidSettings.pidType = NEXUS_PidType_eUnknown;
			break;
		}

		*phPidChannel = NEXUS_Playpump_OpenPidChannel(s_tMediaTSInjector[uTSInjID].tPlaypump, usPid, &openPidSettings);
		if (*phPidChannel == NULL)
		{
			PrintError("[%s] Error> in OpenPidChannel\n", __FUNCTION__);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		switch(eChannelType) {
		case DI_DEMUX_CHANNELTYPE_VIDEO :
			s_tMediaTSInjector[uTSInjID].hVideoPidChn = *phPidChannel;
			s_tMediaTSInjector[uTSInjID].uVideoPid = usPid;
			break;
		case DI_DEMUX_CHANNELTYPE_AUDIO :
		case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
			s_tMediaTSInjector[uTSInjID].hAudioMainPidChn = *phPidChannel;
			s_tMediaTSInjector[uTSInjID].uAudioMainPid = usPid;
			break;
		case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
			s_tMediaTSInjector[uTSInjID].hAudioSubPidChn = *phPidChannel;
			s_tMediaTSInjector[uTSInjID].uAudioSubPid = usPid;
			break;
		case DI_DEMUX_CHANNELTYPE_PCR :
			s_tMediaTSInjector[uTSInjID].uPcrPid = usPid;
			break;
		default :
			/* No Action */
			PrintError("[%s] Unsupport Type\n", __func__);
			break;
		}
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

	PrintExit;
	return DRV_OK;
}

/* deviceId is tsInjID */
DRV_Error DRV_MEDIA_CloseMulticastPidChannel(HUINT32 uTSInjID, NEXUS_PidChannelHandle hPidChannel, DI_DEMUX_CHANNELTYPE eChannelType)
{
	PrintEnter;
	PrintDebug("[%s] ++[devID:%d, chnType:%d]\n", __FUNCTION__, uTSInjID, eChannelType);

	if (uTSInjID >= _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid device ID! (devID:%d)\n", __FUNCTION__, uTSInjID);
		return DRV_ERR;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if (s_tMediaTSInjector[uTSInjID].bRunning == FALSE)
		{
			PrintError("[%s] Error> Inactive injector status! (status:%d)\n",
						__FUNCTION__, s_tMediaTSInjector[uTSInjID].bRunning);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		if (s_tMediaTSInjector[uTSInjID].tPlaypump == NULL)
		{
			PrintError("[%s] Error> Invalid NULL playpump(ID:%d)\n", __FUNCTION__, uTSInjID);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		switch(eChannelType)
		{
			case DI_DEMUX_CHANNELTYPE_VIDEO :
				if (s_tMediaTSInjector[uTSInjID].hVideoPidChn && (s_tMediaTSInjector[uTSInjID].hVideoPidChn == hPidChannel))
				{
					s_tMediaTSInjector[uTSInjID].hVideoPidChn = NULL;
					s_tMediaTSInjector[uTSInjID].uVideoPid = 0x2000;
				}
				else
				{
					PrintError("[%s] Error> Already closed or mismatched video with pidChannelHandle!(0x%x, 0x%x)\n",
								__FUNCTION__,
								(unsigned int)s_tMediaTSInjector[uTSInjID].hVideoPidChn,
								(unsigned int)hPidChannel);
					VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO :
			case DI_DEMUX_CHANNELTYPE_AUDIO_SPDIF : /* 동시에 2가지 type의 PID channel이 모두 open되지 않는다고 가정 */
				if (s_tMediaTSInjector[uTSInjID].hAudioMainPidChn && (s_tMediaTSInjector[uTSInjID].hAudioMainPidChn == hPidChannel))
				{
					s_tMediaTSInjector[uTSInjID].hAudioMainPidChn = NULL;
					s_tMediaTSInjector[uTSInjID].uAudioMainPid = 0x2000;
				}
				else
				{
					PrintError("[%s] Error> Already closed or mismatched audio main with pidChannelHandle!(0x%x, 0x%x)\n",
								__FUNCTION__,
								(unsigned int)s_tMediaTSInjector[uTSInjID].hAudioMainPidChn,
								(unsigned int)hPidChannel);
					VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION :
				if (s_tMediaTSInjector[uTSInjID].hAudioSubPidChn && (s_tMediaTSInjector[uTSInjID].hAudioSubPidChn == hPidChannel))
				{
					s_tMediaTSInjector[uTSInjID].hAudioSubPidChn = NULL;
					s_tMediaTSInjector[uTSInjID].uAudioSubPid = 0x2000;
				}
				else
				{
					PrintError("[%s] Error> Already closed or mismatched audio sub with pidChannelHandle!(0x%x, 0x%x)\n",
								__FUNCTION__,
								(unsigned int)s_tMediaTSInjector[uTSInjID].hAudioSubPidChn,
								(unsigned int)hPidChannel);
					VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
					return DRV_ERR;
				}
				break;
			case DI_DEMUX_CHANNELTYPE_PCR:
				PrintError("[%s] Unsupport PCR Type\n", __func__);
				VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
				return DRV_OK;
			default :
				/* No Action */
				PrintError("[%s] Unsupport Type\n", __func__);
				break;
		}
		NEXUS_Playpump_ClosePidChannel(s_tMediaTSInjector[uTSInjID].tPlaypump, hPidChannel);
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

	PrintExit;
	return DRV_OK;
}

DRV_Error DRV_MEDIA_SetMulticastVPidChannel(NEXUS_PidChannelHandle hPidChannel,
                                       NEXUS_VideoDecoderHandle hVideoDec,
                                       NEXUS_VideoCodec nVideoCodec)
{
	HBOOL bMatched = FALSE;
	HUINT32 i;
	HUINT32 uTSInjID;

	PrintEnter;
	PrintDebug("[%s] [vcodec:%d]\n", __FUNCTION__, nVideoCodec);

	bMatched = FALSE;
	for (i=0;i<_DEF_MEDIA_MAX_INJ_NUM;i++)
	{
		if (s_tMediaTSInjector[i].hVideoPidChn == hPidChannel)
		{
			uTSInjID = i;
			bMatched = TRUE;
			break;
		}
	}

	if (bMatched == FALSE)
	{
		PrintError("[%s] Error> Unmatched pidChn\n", __FUNCTION__);
		;
	}

	if (i == _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid pidChannel! (pidCh:0x%x)\n", __FUNCTION__, (unsigned int)hPidChannel);
		return DRV_ERR;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if (s_tMediaTSInjector[uTSInjID].hVideoPidChn != hPidChannel)
		{
			PrintError("[%s] Error> Maybe vPidChannel was changed! (vPid:0x%x, pidCh:0x%x)\n",
						__FUNCTION__,
						(unsigned int)s_tMediaTSInjector[uTSInjID].hVideoPidChn,
						(unsigned int)hPidChannel);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		if (s_tMediaTSInjector[uTSInjID].bRunning == TRUE)
		{
			s_tMediaTSInjector[uTSInjID].videoSet.decoder = hVideoDec;
			s_tMediaTSInjector[uTSInjID].videoSet.codec = nVideoCodec;
			s_tMediaTSInjector[uTSInjID].videoSet.index = true;
		}
		else
		{
			PrintError("[%s] Error> Invalid device (state:%d)\n", __FUNCTION__, s_tMediaTSInjector[uTSInjID].bRunning);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

	PrintExit;
	return DRV_OK;
}

DRV_Error DRV_MEDIA_SetMulticastAPidChannel(NEXUS_PidChannelHandle hPidChannel,
                                       HBOOL isSubAudioPid,
                                       NEXUS_AudioDecoderHandle hAudioDec,
                                       NEXUS_AudioDecoderHandle hAudioPassThroughDec)
{
	HUINT32 i;
	HUINT32 uTSInjID;
	NEXUS_PidChannelHandle pidChannelHandle;

	PrintEnter;

	PrintDebug("[%s] [apid:%d]\n", __FUNCTION__, isSubAudioPid);

	for (i=0; i<_DEF_MEDIA_MAX_INJ_NUM; i++)
	{
		pidChannelHandle = isSubAudioPid == false ? s_tMediaTSInjector[i].hAudioMainPidChn : s_tMediaTSInjector[i].hAudioSubPidChn;
		if (pidChannelHandle == hPidChannel)
		{
			uTSInjID = i;
			break;
		}
	}

	if (i == _DEF_MEDIA_MAX_INJ_NUM)
	{
		PrintError("[%s] Error> Invalid pidChannel! (pidCh:0x%x)\n", __FUNCTION__, (unsigned int)hPidChannel);
		return DRV_ERR;
	}

	if (VK_SEM_Get(s_tMediaTSInjector[uTSInjID].uSemID) == VK_OK)
	{
		if (!isSubAudioPid && (s_tMediaTSInjector[uTSInjID].hAudioMainPidChn != hPidChannel)) // get semaphore 후 한 번 더 확인
		{
			PrintError("[%s] Error> Maybe audio main pidChannel was changed! (hAudioMainPidChn : 0x%x, pidCh:0x%x)\n",
						__FUNCTION__,
						(unsigned int)s_tMediaTSInjector[uTSInjID].hAudioMainPidChn, (unsigned int)hPidChannel);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}
		else if (isSubAudioPid && (s_tMediaTSInjector[uTSInjID].hAudioSubPidChn != hPidChannel))
		{
			PrintError("[%s] Error> Maybe audio sub pidChannel was changed! (hAudioSubPidChn : 0x%x, pidCh:0x%x)\n",
					__FUNCTION__,
					(unsigned int)s_tMediaTSInjector[uTSInjID].hAudioSubPidChn, (unsigned int)hPidChannel);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}

		if (s_tMediaTSInjector[uTSInjID].bRunning == TRUE)
		{
			pidChannelHandle = hPidChannel;

			if (isSubAudioPid)
			{
				s_tMediaTSInjector[uTSInjID].audioSet.primary = hAudioDec;
				s_tMediaTSInjector[uTSInjID].audioSet.secondary = NULL;
			}
			else
			{
				s_tMediaTSInjector[uTSInjID].audioSet.primary = hAudioDec;
				s_tMediaTSInjector[uTSInjID].audioSet.secondary = hAudioPassThroughDec;
			}
		}
		else
		{
			PrintError("[%s] Error> Invalid device (state:%d)\n",
						__FUNCTION__,
						s_tMediaTSInjector[uTSInjID].bRunning);
			VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
			return DRV_ERR;
		}
		VK_SEM_Release(s_tMediaTSInjector[uTSInjID].uSemID);
	}

	PrintExit;
	return DRV_OK;
}

DRV_Error DRV_MEDIA_SetMulticastStcChannel(HUINT32 uTSInjID, NEXUS_StcChannelHandle hStcChannel)
{
	PrintEnter;

	/* STC is already set at
	   P_DEMUX_EnableChannel() in di_demux */
	/*
	 * stcSets.timebase = NEXUS_Timebase_e0;
	 * stcSets.mode = NEXUS_StcChannelMode_eAuto;
	 * stcSets.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
	 */
	UNUSED_PARAMETER(uTSInjID);
	UNUSED_PARAMETER(hStcChannel);

	PrintExit;
	return DRV_OK;
}

DRV_Error DRV_MEDIA_UnsetMulticastStcChannel(HUINT32 uTSInjID)
{
	PrintEnter;

	/* STC is already unset to default settings
	   at P_DEMUX_DisableChannel() in di_demux */
	UNUSED_PARAMETER(uTSInjID);

	PrintExit;
	return DRV_OK;
}

DI_ERR_CODE DI_MEDIA_GetBufferInfo(SESSION_HANDLE pHSession, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets)
{
	DI_ERR_CODE dResult;
	DI_IPTUNER_t *pstTunerHdl = NULL;

	PrintEnter;

	if((pHSession == NULL) || (uiChunks == NULL))
	{
		PrintError("Invalid parameter!\n");
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if(pstTunerHdl == NULL)
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		return DI_ERR_ERROR;
	}

	dResult = DI_IPTUNER_GetBufferInfo(pstTunerHdl, uiChunks, iStartOffsets, iEndOffsets);
	if(dResult != DI_ERR_OK)
	{
		PrintError("DI_IPTUNER_GetBufferInfo failed!\n");
		return DI_ERR_ERROR;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_SetPlayreadyLicenseURL (SESSION_HANDLE pHSession, HCHAR *licenseURL)
{
	DRV_Error dResult;
	DRM_Info_t drm_info;
	DI_IPTUNER_t 		*pstTunerHdl = NULL;
	DI_MEDIA_INFO_t		*ptStreamInfo = NULL;

	PrintError("DI_MEDIA_SetPlayreadyLicenseURL %s\n", licenseURL);

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	ptStreamInfo = &pstTunerHdl->tMediaInfo;

	if((ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_WMDRM)||(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_PLAYREADY))
	{
		dResult = DRV_DRM_GetInfo(pHSession, &drm_info);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_DRM_GetInfo failed!\n");
			return DRV_ERR;
		}

		drm_info.drm.playready.licenseURL = VK_MEM_Alloc(2048);
		VK_MEM_Memset(drm_info.drm.playready.licenseURL, 0x0, 2048);

		VK_snprintf(drm_info.drm.playready.licenseURL, 2048, "%s", licenseURL);

		dResult = DRV_DRM_ResigterCallback(pHSession, &drm_info);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_DRM_ResigterCallback failed!\n");
			return DRV_ERR;
		}
	}

	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_GetPlayreadyLicenseURL (SESSION_HANDLE pHSession, HCHAR *licenseURL)
{
	DRV_Error dResult;
	DRM_Info_t drm_info;
	DI_IPTUNER_t 		*pstTunerHdl = NULL;
	DI_MEDIA_INFO_t		*ptStreamInfo = NULL;

	PrintError("DI_MEDIA_GetPlayreadyLicenseURL\n");

	PrintEnter;
	if( pHSession == NULL)
	{
		PrintError("[%s] Fail Session Handle is NULL\n", __FUNCTION__);
		PrintExit;
		return DI_ERR_ERROR;
	}

	pstTunerHdl = DI_IPTUNER_POOL_FindById((HUINT32)pHSession);
	if( pstTunerHdl == NULL )
	{
		PrintError("[%s] Error> Fail DI_IPTUNER_POOL_FindById! [Handle : %d]\n", __FUNCTION__, (HUINT32)pHSession);
		PrintExit;
		return DI_ERR_ERROR;
	}
	ptStreamInfo = &pstTunerHdl->tMediaInfo;

	if((ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_WMDRM)||(ptStreamInfo->eDrmType == DI_MEDIA_DRM_TYPE_PLAYREADY))
	{
		dResult = DRV_DRM_GetInfo(pHSession, &drm_info);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_DRM_GetInfo failed!\n");
			return DRV_ERR;
		}

		VK_MEM_Memset(licenseURL, 0x0, 2048);
#if defined(CONFIG_PLAYREADY)
		(void) DRV_DRM_PlayReady_GetLicenseURL(pHSession, ptStreamInfo->pucWrmHeader, ptStreamInfo->nWrmHeaderLen, licenseURL);
		PrintError("licenseURL = %s!\n", licenseURL);
#endif
	}

	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MEDIA_GetPlayreadyNeedCallback (void)
{
	PrintError("DI_MEDIA_GetPlayreadyNeedCallback\n");
	g_NeedPRLicensURL = TRUE;

	return DI_ERR_OK;
}

