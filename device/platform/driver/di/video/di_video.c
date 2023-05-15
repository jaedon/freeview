/**
 * di_video.c
 */

/**
 * @defgroup		DI_VIDEO : DI_VIDEO module
 * @author			Hyun Seok Choi
 * @note			DI_VIDEO APIs
 * @brief			Define DI_VIDEO APIs
 * @file 			di_video.c
 */


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "bstd.h"
#include "bkni.h"
#include "linden_trace.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#if defined(CONFIG_VIDEO_DECODER)
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_still_decoder.h"
#include "nexus_mosaic_video_decoder.h"
#endif
#include "nexus_video_adj.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_vbi.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_video_input.h"
#include "nexus_video_output.h"
#if defined(CONFIG_AUDIO_DECODER)
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#endif
#include "nexus_video_window.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_playpump.h"
#include "nexus_core_utils.h"
#include "nexus_video_types.h"
#include "nexus_hdmi_output.h"
#include "nexus_hdmi_output_hdcp.h"
#if defined(CONFIG_HDMI_IN)
#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_hdcp.h"
#endif
#include "nexus_picture_ctrl.h"
#if NEXUS_HAS_ASTM
#include "nexus_astm.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif
#include "nexus_mosaic_display.h"

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
#include "nexus_video_decoder_primer.h"
#endif

#if defined(CONFIG_CLOSED_CAPTION)
#include "nexus_video_input_vbi.h"
#endif

/* include  di headers */
#include "drv_err.h"
#include "di_video.h"
#include "di_audio.h"
#include "di_media.h"
#include "di_uart.h"
#include "di_nvram.h"
#include "drv_video.h"
#include "drv_osd.h"
#include "drv_demux.h"
#if defined(CONFIG_MEDIA)
#if defined(CONFIG_DI10)
#include "drv_media_10.h"
#else
#include "drv_media.h"
#endif
#endif
#if defined(CONFIG_DI20)
#include "drv_media20.h"
#endif
#if defined(CONFIG_CALYPSO)
#include "drv_calypso.h"
#endif

#include "drv_audio.h"
#include "drv_gpio.h"
#include "drv_pvr.h"
#include "drv_nvram.h"
#include "drv_hdmi.h"
#include "vkernel.h"
#include "taskdef.h"
#if defined(CONFIG_SYNC_CHANNEL)
#include "drv_sync_channel.h"
#endif
#if defined(CONFIG_ASTM)
#include "drv_astm.h"
#endif
#if defined(CONFIG_SYNC_RECOVERY)
#include "drv_sync_recovery.h"
#endif

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
#include "drv_sharedmem.h"
#endif

#include "drv_rfvod.h"

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#undef CONFIG_DIRECTFB
#endif

#if defined (CONFIG_DIRECTFB)
#include "platform_init.h"
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include "drv_pm.h"
#endif

#if defined(CONFIG_VIDEO_DEC_H265)
#include "di_config.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_VIDEO
//#define DI_VIDEO_DEBUG
//#define DI_VIDEO_ENTER_LEAVE_DEBUG
//#define DI_VIDEO_SEE_PARAMETERS
#define PARAM_COLOR_ON		"\033[01;35m"
#define PARAM_COLOR_OFF	"\033[00m"

#ifndef DEBUG_MODULE
#ifdef DI_VIDEO_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif

#ifdef DI_VIDEO_ENTER_LEAVE_DEBUG
#define PrintEnter()	DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()	DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif
#endif /* #ifndef DEBUG_MODULE */

#define PrintStatus(...)	DI_UART_Print (__VA_ARGS__)

//#define PRINT_VIDEO_PTS_INFO /* for debug pts info*/

/*
 * Conditional define for Video
 */
//#define SUPPORT_VSYNC			/* Vsync mode stc channel is null */
#if defined(CONFIG_DI10)
#define CONFIG_INRECT_RATIO 		/* Window inrect ratio value */
#define CONFIG_HDOUTRECT_TOSD		/* SD outrect based on HD outrect value */
#endif
//#define CONFIG_DEINTERLACE_ONLY_P	/* Only uses the deinterlace function to display out progressive(e.g. 576p,720p) */
//#define CONFIG_HDMI_576P_ONLY	/* Only use 576p output at hdmi */
//#define CONFIG_HDMI_480P_ONLY	/* Only use 480p output at hdmi */
//#define CONFIG_USE_THUMBNAIL_DECODING  /* Use bthumbnail libary to extract thumbnail image from stream.*/

#if defined(CONFIG_INRECT_RATIO)
#define CONFIG_HD_RESOLUTION_CHECK   //workaround for CVBS Scaling issue
#endif

/* End of Conditional defines for Video */

/*
 * WORK AROUND CODE Defines
 */

//#define CONFIG_WORKAROUND_DIRECT_VIDEO_SHOW /* kevin */
#define CONFIG_WORKAROUND_ACTIVE_VIDEO_SHOW /* kevin */
#define CONFIG_WORKAROUND_VIDEO_MUTE
#define CONFIG_WORKAROUND_VIDEO_WINDOW
//#define WORKAROUND_DEC_RESET		/* Unexpected decoder status, decoder reset */
#if defined(CONFIG_ASTM)
#else
#define CONFIG_WORKAROUND_DEC_RESTART		/* Unexpected decoder status, decoder stop/start */
#endif

#if defined(CONFIG_FIX_WIN_CLIP) /* Make variable from make/bcm_fix */
#define CONFIG_WORKAROUND_WIN_CLIP  /* Broadcom WorkAround Win Clip, when HD window SetInOut */
#endif

/* End of WORK AROUND CODE Defines */

#if defined(CONFIG_PIP)
#define NUM_VID_DECODER 	2
#else
#define NUM_VID_DECODER 	1
#endif
#define DEF_VID_INSTANCE 	0

#define HD_DISP 0
#define SD_DISP 1

#define MAIN_WINDOW	0
#define PIP_WINDOW	1

#define DI_NUM_DISPLAYS 2 /* HD-1, SD-1 */

#define DI_NUM_VIDEO_WINDOWS NEXUS_NUM_VIDEO_WINDOWS
#define DI_NUM_MOSAIC_WINDOWS CONFIG_VIDEO_MOSAIC_CNT

#define SD_DISP_WIDTH 720
#if defined(CONFIG_SDOUT_NTSC) || defined(CONFIG_SDOUT_NTSC_JAPAN)
#define SD_DISP_HEIGHT 480
#else
#define SD_DISP_HEIGHT 576
#endif

#define MIN_SCL_WIDTH  4
#define MIN_SCL_HEIGHT	4

#define pCAST(pX) \
	((DI_VIDEO_AOUT_MODE_t *)pX)
#define IS_COMPONENT_SETTING_CHANGED(OUTPUT_OLD, OUTPUT_NEW) ((pCAST(OUTPUT_OLD)->bRGB != pCAST(OUTPUT_NEW)->bRGB) || \
		(pCAST(OUTPUT_OLD)->bYPbPr != pCAST(OUTPUT_NEW)->bYPbPr) || \
		(pCAST(OUTPUT_OLD)->bYPbPr_Downscale != pCAST(OUTPUT_NEW)->bYPbPr_Downscale))

#define VIDEO_FORMAT(NAME) [NAME] = #NAME

#define DI_VIDEO_UNDERRUN_TIMEOUT_DEFAULT 2000 /*millisec*/
#define DI_VIDEO_UNDERRUN_TIMEOUT_RES 100  /*millisec*/

#define VIDEO_MESSAGE_QUEUE_SIZE 10

#define VIDEO_TASK_CHECK_STATE_TIME 10

#define VIDEO_ERROR_CHECK_TIME 2000 /* millisec */

#define CLOSED_CAPTION_DATA_CHECK_TIME 5000 /* millisec */

#define UNREFENCED_PARAM(x) (x=x)

#if defined(CONFIG_AFD_EVENT)
#define USER_DATA_START_CODE 0x000001B2
#define USER_DATA_START_CODE_AVC 0x00000106
#define AFD_IDENTIFIER 0x44544731
#define AFD_FORMAT_FLAG 0x40
#define AFD_ALIGNMENT_BITS 0x01
#define AFD_VALUE_MASK 0x0F
#endif

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
#define DEC_DIFF_DISPLAY_DROP_NUM (10)
#define DEC_DIFF_DECODE_ERROR_NUM (30)   /* change 30 from 20 for Nordig certification. "Task0531_v1.0.ts" with ASTM disabled */
#define DEC_DIFF_DISPLAY_ERROR_NUM (50)
#define NORMAL_PLAY_SPEED (1000)
#define DEC_RESET_CLEAR_TIME	(5000)
#endif

#if defined(CONFIG_VIDEO_ERROR_LOG)
#define VIDEO_ERROR_EVENT_TIME (1000)
#endif

#define SetWSS_NONE			0x00
#define SetWSS_FULL_FORMAT_4_3		0x08
#define SetWSS_BOX_14_9_CENTRE		0x01
#define SetWSS_BOX_16_9_CENTRE		0x0B
#define SetWSS_FULL_FORMAT_4_3_SNP	0x0E
#define SetWSS_FULL_FORMAT_16_9_ANAMORPHIC 0x07

#if defined (CONFIG_VIDEO_SYNC_OFFSET)
#define PTSOFFSET	CONFIG_VIDEO_SYNC_OFFSET
#else
#define PTSOFFSET	0
#endif

#define NVRAM_HD_RESOLUTION_SIZE	4

#define DRV_VIDEO_CHECK_INIT(x) do { \
	if (x >= NUM_VID_DECODER || s_stVideo[x].bInitialized==FALSE) { \
		PrintError(" %s decoder(idx=%d) is not supported or initialized.\n", \
		((ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Master" : "PIP"),ulDeviceId); \
		return DRV_ERR_INVALID_PARAMETER; \
	} \
} while (0)

#define DI_VIDEO_CHECK_INIT(x) do { \
	if (x >= NUM_VID_DECODER || s_stVideo[x].bInitialized==FALSE) { \
		PrintError(" %s decoder(idx=%d) is not supported or initialized.\n", \
		((ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Master" : "PIP"),ulDeviceId); \
		return DI_ERR_ERROR; \
	} \
} while (0)

#define DRV_VIDEO_GET_SEMA(x) do { \
	int VK_Err=VK_OK; \
	VK_Err=VK_SEM_Get(s_stVideo[x].ulSemId_DRV); \
	if(VK_Err != VK_OK) { \
		PrintError("Sema Get Error\n"); \
		return DRV_ERR; \
	} \
} while (0)

#define DI_VIDEO_GET_SEMA(x) do { \
	int VK_Err=VK_OK; \
	VK_Err=VK_SEM_Get(s_stVideo[x].ulSemId_DI); \
	if(VK_Err != VK_OK) { \
		PrintError("Sema Get Error\n"); \
		return DI_ERR_ERROR; \
	} \
} while (0)

#define DI_VIDEO_GET_SEMA_SHOW(x) do { \
	int VK_Err=VK_OK; \
	VK_Err=VK_SEM_Get(s_stVideo[x].ulSemId_ShowHide); \
	if(VK_Err != VK_OK) { \
		PrintError("Sema Get Error\n"); \
		return DI_ERR_ERROR; \
	} \
} while (0)

#define DRV_VIDEO_REL_SEMA(x) do { \
	int VK_Err=VK_OK; \
	VK_Err=VK_SEM_Release(s_stVideo[x].ulSemId_DRV); \
	if(VK_Err != VK_OK) { \
		PrintError("Sema Release Error\n"); \
		return DRV_ERR; \
	} \
} while (0)

#define DI_VIDEO_REL_SEMA(x) do { \
	int VK_Err=VK_OK; \
	VK_Err=VK_SEM_Release(s_stVideo[x].ulSemId_DI); \
	if(VK_Err != VK_OK) { \
		PrintError("Sema Release Error\n"); \
		return DI_ERR_ERROR; \
	} \
} while (0)

#define DI_VIDEO_REL_SEMA_SHOW(x) do { \
	int VK_Err=VK_OK; \
	VK_Err=VK_SEM_Release(s_stVideo[x].ulSemId_ShowHide); \
	if(VK_Err != VK_OK) { \
		PrintError("Sema Release Error\n"); \
		return DI_ERR_ERROR; \
	} \
} while (0)

#if defined(CONFIG_DI20)
#define VIDEO_3D_STATUS_TIMEOUT 1000
#endif

#define VIDEO_SCALER_APPLY_TIME 10


#if defined(CONFIG_BRCM_MOD_NEXUS_FRONTEND_3461)
// To fix CVBS Burst Frequency Fail
// W/A for Broadcom T2 demodulator's defect
#define SD_DISPLAY_TIMEBASE_FREERUN

//to block broken picture after channel change
#define VIDEODECODER_ERROR_HANDLING_ePROGNOSTIC
#endif


#ifdef SD_DISPLAY_TIMEBASE_FREERUN
//select e3 which is not used
#define T2_SD_TIMEBASE NEXUS_Timebase_e3
#endif

#ifdef CONFIG_HD_RESOLUTION_CHECK
NEXUS_VideoFormat HD_Format_Desired;
#endif


#ifndef HEVC_MAX_RES
//HEVC_MAX_RES should be defined in di_config.h if not support 4K
#define HEVC_MAX_RES NEXUS_VideoFormat_e3840x2160p30hz
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct _CALLBACK_INFO
{
	EVENT_CALLBACK_FUNCTION pfnCallback;
} CALLBACK_INFO;

typedef struct _CC_CALLBACK_INFO
{
	pfnDI_VIDEO_CcDataCallback	pfnCcDataCallback;
} CC_CALLBACK_INFO;

typedef struct _stMosaicWindowInfo
{
	HBOOL	bOpen;
	HBOOL	bUsed;
	HUINT32	ulIndex;
	NEXUS_VideoWindowHandle hMosaicWindow[DI_NUM_DISPLAYS];
	DI_VIDEO_RECT_t stInRect;
	DI_VIDEO_RECT_t stOutRect;
	DI_VIDEO_WIN_TYPE eVidWin;
}stMosaicInfo;

#define TOTAL_CALLER_NUM	5

typedef enum
{
	P_VIDEO_CC_DATA_TYPE_608 = 0,
	P_VIDEO_CC_DATA_TYPE_708,
	P_VIDEO_CC_DATA_TYPE_BOTH
} P_VIDEO_CC_DATA_TYPE;

typedef struct _stClosedCaptionInfo
{
	HBOOL					bEnabledCcData;
	DI_VIDEO_CC_DATA_STATE	eCurCcDataState;
	P_VIDEO_CC_DATA_TYPE	eCcDataType;
	unsigned long			ulClosedCaptionMsgQueueId;	
	unsigned long 			ulClosedCaptionTaskId;
	unsigned long			ulSemId_ClosedCaption;
} ClosedCaptionInfo_t;

typedef struct _stVideoInstance
{
	HUINT32 ulDeviceId;
#if defined(CONFIG_VIDEO_DECODER)
	NEXUS_VideoDecoderHandle hDecoder;
	NEXUS_StillDecoderHandle hStillDecoder;
	NEXUS_VideoDecoderStartSettings StartSettings;
#endif
	NEXUS_DisplayHandle hDisplay[DI_NUM_DISPLAYS];
	NEXUS_VideoWindowHandle  hWindow[DI_NUM_DISPLAYS][DI_NUM_VIDEO_WINDOWS];
	NEXUS_VideoInput hInput;
	NEXUS_DisplaySettings DisplaySettings;
	NEXUS_PlaypumpHandle PlayMemoryPlaypump;
	CALLBACK_INFO stCallbackInfo[TOTAL_CALLER_NUM][DI_VIDEO_NOTIFY_EVT_END];
	CC_CALLBACK_INFO stCcCallbackInfo;
	unsigned long ulUnderrunTimeout;
	HBOOL	bVbiTeletext;
	HBOOL	bVbiClosedCaption;
	HBOOL	bInitialized;
	HBOOL	bIsStill;
	HBOOL   bDecoding;
	HBOOL   bNeedToShow;
	HBOOL	bFirstPtsPassed;
	HBOOL	bIsPlayback;
	HBOOL	bFreeze;
	HBOOL	bDecState;
	HBOOL	bTsm;
	HBOOL	bForcedMadOff; /* To prevent Video Flickering for Smooth Streaming */
	HBOOL	bForcedMute[DI_NUM_DISPLAYS];
	HBOOL	bOpenDisplay[DI_NUM_DISPLAYS];
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	HBOOL	bIsMedia;
#endif
	HBOOL	bMosaicState;

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
	HBOOL	bDecReset;
	unsigned long	ulResetTimerId;
#endif
	unsigned long	ulApplyScalerTimerId;
	DI_VIDEO_FORMAT eVidCodec;
#if defined(CONFIG_DI20)
	DI_VIDEO_3D_FORMAT e3dDisplayFormat;
	DI_VIDEO_3D_FORMAT e3dFormat;
#endif
	unsigned long 	ulSemId_DI;
	unsigned long 	ulSemId_DRV;
	unsigned long 	ulSemId_P;
	unsigned long 	ulSemId_ShowHide;

	unsigned long ulVideoMonitorTaskId;
	unsigned long ulVideoMonMsgQueueId;

#if defined(CONFIG_VIDEO_ERROR_LOG)
	unsigned long	ulErrorTimerId;
#endif
	stMosaicInfo stMosaic[DI_NUM_MOSAIC_WINDOWS];
#if defined(CONFIG_IFRAME_RANDOM_ACCESS)
	HBOOL bTreatIFrameAsRap;
#endif
	ClosedCaptionInfo_t	stClosedCaptionInfo;
	DI_VIDEO_DECODER_MODE eDecoderMode;
	HINT32 nUserOffset;
} DI_VideoInstance;

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
#define VIDEO_PRIMER_MAX_COUNT CONFIG_PRIMER_COUNT
typedef struct
{
	/* Is started primer */
	HBOOL bStartPrimer;
	/* decode primer index */
	HUINT32 ulDecodePrimerIndex;
	/* primer setting info */
	HBOOL bOccupied[VIDEO_PRIMER_MAX_COUNT];
	DI_VIDEO_FORMAT eFormat[VIDEO_PRIMER_MAX_COUNT];
	NEXUS_VideoDecoderPrimerHandle hPrimer[VIDEO_PRIMER_MAX_COUNT];
	NEXUS_VideoDecoderStartSettings decoderSettings[VIDEO_PRIMER_MAX_COUNT];
}DI_VideoPrimerInstance;
#else
#define VIDEO_PRIMER_MAX_COUNT 0
#endif

#if defined(CONFIG_VIDEO_DECODER)
typedef struct _stSTILL_DECODER_SETTINGS
{
	NEXUS_StillDecoderHandle hStillDecoder;
	NEXUS_StillDecoderStartSettings StillStartSettings;
}stSTILL_DECODER_SETTINGS;
#endif

typedef enum
{
	P_VIDEO_OUTPUT_MODE_COMPOSITE,
#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 2
	P_VIDEO_OUTPUT_MODE_COMPOSITE2,
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS >= 1
	P_VIDEO_OUTPUT_MODE_SVIDEO,
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	P_VIDEO_OUTPUT_MODE_COMPONENT,
#endif
	P_VIDEO_OUTPUT_MODE_MAX
}P_VIDEO_OUTPUT_MODE;

typedef struct
{
	unsigned long ulParam0;
	unsigned long ulParam1;

} ST_VIDEO_MSG;

typedef struct
{
	HUINT8 ccType;
	HUINT8 data[2];
} P_VIDEO_CC_MSG;

typedef struct _stVideoInfo
{
	DI_VIDEO_ACTIVE_FORMAT afd;
	DI_VIDEO_AspectRatio_t eAspectRatio;
	DI_VIDEO_RECT_t stDesiredHdOutRect;
	DI_VIDEO_RECT_t stDesiredHdInRect;
	DI_VIDEO_RECT_t stDesiredSdOutRect;
	DI_VIDEO_RECT_t stDesiredSdInRect;
	DI_VIDEO_WSS_SETTINGS_t stWssSettings;
}stVideoInfo;

typedef struct _stVideoErrorStatus
{
    HUINT32 ulDecodeErrors;			/* total number of decoder errors since Start */
    HUINT32 ulDecodeOverflows;		/* total number of overflows of the input to the decoder since Start */
    HUINT32 ulDisplayErrors;		/* total number of display manager errors since Start. This includes parity mismatches which may result in glitching on the display. */
    HUINT32 ulDecodeDrops;			/* total number of pictures dropped by the decoder since Start */
    HUINT32 ulDisplayDrops;			/* total number of pictures dropped by the display manager due to TSM failures since Start */
    HUINT32 ulDisplayUnderflows;	/* total number of times the display manager was unable to deliver a new picture since Start */
    HUINT32 ulPtsErrors;			/* counter for number of PTS error interrupts since Start */
} stVideoErrorStatus;

static const struct {
	NEXUS_VideoFormat nexusVideoFormat;
	DI_NVRAM_VIDEO_RESOLUTION_E eRes;
	DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E eRate;
} stNvramVideoFormatTable[] = {
	{NEXUS_VideoFormat_eNtsc, DI_NVRAM_RESOLUTION_720x480I, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_e480p, DI_NVRAM_RESOLUTION_720x480P, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_ePal, DI_NVRAM_RESOLUTION_720x576I, DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{NEXUS_VideoFormat_e576p, DI_NVRAM_RESOLUTION_720x576P, DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{NEXUS_VideoFormat_e720p, DI_NVRAM_RESOLUTION_1280x720P, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_e720p24hz, DI_NVRAM_RESOLUTION_1280x720P, DI_NVRAM_HDMI_FIELD_RATE_24HZ},
	{NEXUS_VideoFormat_e720p25hz, DI_NVRAM_RESOLUTION_1280x720P, DI_NVRAM_HDMI_FIELD_RATE_25HZ},
	{NEXUS_VideoFormat_e720p30hz, DI_NVRAM_RESOLUTION_1280x720P, DI_NVRAM_HDMI_FIELD_RATE_30HZ},
	{NEXUS_VideoFormat_e720p50hz, DI_NVRAM_RESOLUTION_1280x720P, DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{NEXUS_VideoFormat_e1080i, DI_NVRAM_RESOLUTION_1920x1080I, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_e1080i50hz, DI_NVRAM_RESOLUTION_1920x1080I, DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{NEXUS_VideoFormat_e1080p, DI_NVRAM_RESOLUTION_1920x1080P, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_e1080p24hz, DI_NVRAM_RESOLUTION_1920x1080P, DI_NVRAM_HDMI_FIELD_RATE_24HZ},
	{NEXUS_VideoFormat_e1080p25hz, DI_NVRAM_RESOLUTION_1920x1080P, DI_NVRAM_HDMI_FIELD_RATE_25HZ},
	{NEXUS_VideoFormat_e1080p30hz, DI_NVRAM_RESOLUTION_1920x1080P, DI_NVRAM_HDMI_FIELD_RATE_30HZ},
	{NEXUS_VideoFormat_e1080p50hz, DI_NVRAM_RESOLUTION_1920x1080P, DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{NEXUS_VideoFormat_e240p60hz, DI_NVRAM_RESOLUTION_720x240P, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_e288p50hz, DI_NVRAM_RESOLUTION_720x288P, DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{NEXUS_VideoFormat_e1440x480p60hz, DI_NVRAM_RESOLUTION_1440x480P, DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{NEXUS_VideoFormat_e1440x576p50hz, DI_NVRAM_RESOLUTION_1440x576P, DI_NVRAM_HDMI_FIELD_RATE_50HZ}
};

#if defined(CONFIG_VIDEO_DECODER)

static const char* strCodec[NEXUS_VideoCodec_eMax]={
	"NEXUS_VideoCodec_eUnknown",
	"NEXUS_VideoCodec_eMpeg1",
	"NEXUS_VideoCodec_eMpeg2",
	"NEXUS_VideoCodec_eMpeg4Part2",
	"NEXUS_VideoCodec_eH263",
	"NEXUS_VideoCodec_eH264",
	"NEXUS_VideoCodec_eH264_Svc",
	"NEXUS_VideoCodec_eH264_Mvc",
	"NEXUS_VideoCodec_eVc1",
	"NEXUS_VideoCodec_eVc1SimpleMain",
	"NEXUS_VideoCodec_eDivx311",
	"NEXUS_VideoCodec_eAvs",
	"NEXUS_VideoCodec_eRv40",
#if (NEXUS_VERSION >= 1303)
	"NEXUS_VideoCodec_eVp6",
	"NEXUS_VideoCodec_eVp7",
	"NEXUS_VideoCodec_eVp8",
#if (NEXUS_VERSION >= 1501)
	"NEXUS_VideoCodec_eVp9",
#endif
	"NEXUS_VideoCodec_eSpark",
	"NEXUS_VideoCodec_eMotionJpeg",
	"NEXUS_VideoCodec_eH265",
#else
	"NEXUS_VideoCodec_eVp6",
#endif
};


static const char* strVideoFormat[NEXUS_VideoFormat_eMax]={
	VIDEO_FORMAT(NEXUS_VideoFormat_eUnknown),
	VIDEO_FORMAT(NEXUS_VideoFormat_eNtsc) ,
	VIDEO_FORMAT(NEXUS_VideoFormat_eNtsc443),
	VIDEO_FORMAT(NEXUS_VideoFormat_eNtscJapan),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalM),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalN),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalNc),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalB),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalB1),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalD),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalD1),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalDK2),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalDK3),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalG),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalH),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalK),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePalI),
	VIDEO_FORMAT(NEXUS_VideoFormat_ePal60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eSecamL),
	VIDEO_FORMAT(NEXUS_VideoFormat_eSecamB),
	VIDEO_FORMAT(NEXUS_VideoFormat_eSecamG),
	VIDEO_FORMAT(NEXUS_VideoFormat_eSecamD),
	VIDEO_FORMAT(NEXUS_VideoFormat_eSecamK),
	VIDEO_FORMAT(NEXUS_VideoFormat_eSecamH),
	VIDEO_FORMAT(NEXUS_VideoFormat_e480p),
	VIDEO_FORMAT(NEXUS_VideoFormat_e576p),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080i),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080i50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p24hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p25hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p30hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1250i50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p),
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p24hz),
#if (NEXUS_VERSION >= 1204)
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p25hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p30hz),
#endif
	VIDEO_FORMAT(NEXUS_VideoFormat_e240p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e288p50hz),
#if (NEXUS_VERSION >= 1204)
	VIDEO_FORMAT(NEXUS_VideoFormat_e1440x480p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e1440x576p50hz),
#if (NEXUS_VERSION >= 1304)
	VIDEO_FORMAT(NEXUS_VideoFormat_e3840x2160p24hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e3840x2160p25hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e3840x2160p30hz),
#if (NEXUS_VERSION >= 1403)
	VIDEO_FORMAT(NEXUS_VideoFormat_e3840x2160p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e3840x2160p60hz),
#endif
	VIDEO_FORMAT(NEXUS_VideoFormat_e4096x2160p24hz),
#if (NEXUS_VERSION >= 1403) || ((NEXUS_VERSION == 1402) && (NEXUS_VERSION_MINOR >= 2)) /* Above NEXUS 14.02 take 1 */
	VIDEO_FORMAT(NEXUS_VideoFormat_e4096x2160p25hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e4096x2160p30hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e4096x2160p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_e4096x2160p60hz),
#endif
#endif
#endif
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustomer1440x240p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustomer1440x288p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustomer1366x768p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustomer1366x768p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x480p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x768p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x768p60hzRed),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720p50hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720pReducedBlank),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x350p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x350p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x350p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x350p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x350p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x400p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x400p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x400p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x400p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x400p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x480p66hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x480p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x480p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x480p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa640x480p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa720x400p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa720x400p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa720x400p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa720x400p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa720x400p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p56hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p59hzRed),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768p66hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720p70hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720p72hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x720p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1024x768i87hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x864p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x768p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x768p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x960p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x960p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p69hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p85hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa832x624p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1360x768p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1366x768p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1400x1050p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1400x1050p75hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1600x1200p60hz),
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank),
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p_3DOU_AS),						/* HD 3D 720p */
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p50hz_3DOU_AS),					/* HD 3D 720p50 */
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p30hz_3DOU_AS),					/* HD 3D 720p30 */
	VIDEO_FORMAT(NEXUS_VideoFormat_e720p24hz_3DOU_AS),					/* HD 3D 720p24 */
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p24hz_3DOU_AS), 				/* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	VIDEO_FORMAT(NEXUS_VideoFormat_e1080p30hz_3DOU_AS), 				/* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */

	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1680x1050p_60Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red),			/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1600x1200p_75Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red),			/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red),			/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1920x1200p_60Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red),			/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x720p_60Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x720p_75Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x720p_85Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x864p_60Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x864p_85Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x870p_75Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x900p_66Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1152x900p_76Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1170x584p_50Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p_70Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p_72Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x1024p_76Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x768p_50Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1280x960p_75Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1600x1024p_60Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1600x1024p_76Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa1728x1080p_60Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p_100Hz),				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa800x600p_90Hz), 				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa848x480p_75Hz), 				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa848x480p_85Hz), 				/* DVI VESA mode for computer monitors */
	VIDEO_FORMAT(NEXUS_VideoFormat_eVesa852x480p_60Hz), 				/* DVI VESA mode for computer monitors */

	/* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom1920x2160i_48Hz), /* 3DTV panel format */

	/* Legacy 3D timing and display formats */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom_3D_720p),		/* HD 3D 720p with custom timing */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom_3D_720p_50hz),	/* HD 3D 720p50hz with custom timing */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom_3D_720p_30hz),	/* HD 3D 720p30hz with custom timing */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom_3D_720p_24hz),	/* HD 3D 720p24hz with custom timing */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom_3D_1080p_24hz),	/* HD 3D 1080p24hz with custom timing */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom_3D_1080p_30hz),	/* HD 3D 1080p30hz with custom timing */

#if (NEXUS_VERSION >= 1290)
	VIDEO_FORMAT(NEXUS_VideoFormat_e720x482_NTSC),						/* 720x482i NTSC-M for North America */
	VIDEO_FORMAT(NEXUS_VideoFormat_e720x482_NTSC_J),					/* 720x482i Japan */
	VIDEO_FORMAT(NEXUS_VideoFormat_e720x483p),							/* 720x483p */

#endif
	    /* dynamics: custom format */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom0),         /* Customized video format 0. This enums allows the user to select custom display settings programmed in
	                                           magnum/commonutils/fmt/bfmt_custom.c. This cannot be changed at run time.
	                                           Typically, the default bfmt_custom.c sets eCustom0 to be a 60Hz format and eCustom1 to be a 50Hz format.
	                                           Custom settings can be obtained upon request. */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom1),         /* Customized video format 1. See NEXUS_VideoFormat_eCustom0 for details. */
	VIDEO_FORMAT(NEXUS_VideoFormat_eCustom2)         /* eCustom2 is defined at run time by calling NEXUS_Display_SetCustomFormatSettings.
	                                           This runtime modification cannot be done for eCustom0 and eCustom1.
	                                           Typically, apps set eCustom2 to be a 48Hz format, which also supports 24Hz content. */
};

#endif /* #if defined(CONFIG_VIDEO_DECODER) */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
NEXUS_PlatformConfiguration platformConfig;  //temporary. will be deleted soon.

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
#if defined(CONFIG_IFRAME_RANDOM_ACCESS)
static unsigned long s_rapTimerId = 0;
static HBOOL s_bRunningrapTimer = FALSE;
#endif

#if defined(CONFIG_VIDEO_DECODER)
static unsigned long s_ulErrorCheckTimerId[NUM_VID_DECODER] = {0,};
#endif


#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
static void *sharedMem=NULL;
#endif

static DI_VideoInstance   	s_stVideo[NUM_VID_DECODER];
static DI_VIDEO_AOUT_MODE_t	s_currentOutputStatus;

#if defined(CONFIG_VIDEO_DECODER)
static DI_VIDEO_CAP_t 		s_stVideoCap[NUM_VID_DECODER] =
{//group_id	decoder_id	decoder_type				supported_format
	{0, 			0, 			DI_VIDEO_DECODER_MAIN,
		DI_VIDEO_FORMAT_MPEG1
		| DI_VIDEO_FORMAT_MPEG2
		| DI_VIDEO_FORMAT_MPEG4Part2
		| DI_VIDEO_FORMAT_H261
		| DI_VIDEO_FORMAT_H263
		| DI_VIDEO_FORMAT_H264
		| DI_VIDEO_FORMAT_VC1SimpleMain
		| DI_VIDEO_FORMAT_VC1
		| DI_VIDEO_FORMAT_AVS
		| DI_VIDEO_FORMAT_DIVX_311
#if defined(CONFIG_VIDEO_DEC_H265)
		| DI_VIDEO_FORMAT_H265
#endif
#if defined(CONFIG_VIDEO_DEC_VP8)
		| DI_VIDEO_FORMAT_VP8
#endif
#if defined(CONFIG_VIDEO_DEC_VP9)
		| DI_VIDEO_FORMAT_VP9
#endif
		,VIDEO_PRIMER_MAX_COUNT},
#if defined(CONFIG_PIP)
	{0, 			1, 			DI_VIDEO_DECODER_SUB,
		DI_VIDEO_FORMAT_MPEG1
		| DI_VIDEO_FORMAT_MPEG2
		| DI_VIDEO_FORMAT_MPEG4Part2
		| DI_VIDEO_FORMAT_H261
		| DI_VIDEO_FORMAT_H263
		| DI_VIDEO_FORMAT_H264
		| DI_VIDEO_FORMAT_VC1SimpleMain
		| DI_VIDEO_FORMAT_VC1
		| DI_VIDEO_FORMAT_AVS
#if defined(CONFIG_VIDEO_DEC_H265)
		| DI_VIDEO_FORMAT_H265
#endif
#if defined(CONFIG_VIDEO_DEC_VP8)
		| DI_VIDEO_FORMAT_VP8
#endif
#if defined(CONFIG_VIDEO_DEC_VP9)
		| DI_VIDEO_FORMAT_VP9
#endif
		,VIDEO_PRIMER_MAX_COUNT},
#endif
};

#endif /*  #if defined(CONFIG_VIDEO_DECODER) */

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
static DI_VideoPrimerInstance s_stVideoPrimer;
#endif

static stVideoInfo s_stVideoInfo[NUM_VID_DECODER];

#if defined(CONFIG_VIDEO_DECODER)
static stVideoErrorStatus s_stPrevVideoErrorStatus[NUM_VID_DECODER];
#endif /*  #if defined(CONFIG_VIDEO_DECODER) */

static DI_VIDEO_DECODER_MODE s_eRequestDecoderMode[NUM_VID_DECODER];

#if defined(CONFIG_VIDEO_DECODER)
static NEXUS_PidChannelHandle	hStillPidChannel = NULL;

static DI_VIDEO_PROFILELEVEL_t s_astMpeg2ProfileLevel[] =
{
	{DI_VIDEO_PROTOCOL_PROFILE_MAIN, DI_VIDEO_PROTOCOL_LEVEL_LOW, 4000},
	{DI_VIDEO_PROTOCOL_PROFILE_MAIN, DI_VIDEO_PROTOCOL_LEVEL_MAIN, 15000},
	{DI_VIDEO_PROTOCOL_PROFILE_MAIN, DI_VIDEO_PROTOCOL_LEVEL_HIGH, 80000},
};

static DI_VIDEO_PROFILELEVEL_t s_astMpeg4part2ProfileLevel[] =
{
	{DI_VIDEO_PROTOCOL_PROFILE_SIMPLE, DI_VIDEO_PROTOCOL_LEVEL_00, 64},
	{DI_VIDEO_PROTOCOL_PROFILE_SIMPLE, DI_VIDEO_PROTOCOL_LEVEL_10, 64},
	{DI_VIDEO_PROTOCOL_PROFILE_SIMPLE, DI_VIDEO_PROTOCOL_LEVEL_20, 128},
	{DI_VIDEO_PROTOCOL_PROFILE_SIMPLE, DI_VIDEO_PROTOCOL_LEVEL_30, 384},
	{DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE, DI_VIDEO_PROTOCOL_LEVEL_00, 128},
	{DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE, DI_VIDEO_PROTOCOL_LEVEL_10, 128},
	{DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE, DI_VIDEO_PROTOCOL_LEVEL_20, 384},
	{DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE, DI_VIDEO_PROTOCOL_LEVEL_30, 768},
	{DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE, DI_VIDEO_PROTOCOL_LEVEL_40, 3000},
	{DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE, DI_VIDEO_PROTOCOL_LEVEL_50, 8000}
};

static DI_VIDEO_PROFILELEVEL_t s_astH264ProfileLevel[] =
{
	{DI_VIDEO_PROTOCOL_PROFILE_MAIN, DI_VIDEO_PROTOCOL_LEVEL_41, 50000},
	{DI_VIDEO_PROTOCOL_PROFILE_MAIN, DI_VIDEO_PROTOCOL_LEVEL_42, 50000},
	{DI_VIDEO_PROTOCOL_PROFILE_HIGH, DI_VIDEO_PROTOCOL_LEVEL_41, 62500},
	{DI_VIDEO_PROTOCOL_PROFILE_HIGH, DI_VIDEO_PROTOCOL_LEVEL_42, 62500}
};
#endif /*  #if defined(CONFIG_VIDEO_DECODER) */

#if defined(CONFIG_CLOSED_CAPTION)
static HUINT32 s_ulCcDataChecker[NUM_VID_DECODER] = {0,};
#endif

/*******************************************************************/
/*********************** extern function prototypes ****************/
/*******************************************************************/
DRV_Error DRV_HDMI_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDResolution);
NEXUS_VideoFormat P_VIDEO_NvramConvert2NexusVideoFormat(DI_NVRAM_VIDEO_RESOLUTION_E eRes, DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E eRate);
#if defined(CONFIG_VIDEO_DECODER)
#if defined(CONFIG_HDMI_IN)
DRV_Error DRV_VIDEO_SetHdmiVideoInputToOutput(HUINT32 ulDeviceId, NEXUS_HdmiInputHandle hdmiInput);
DRV_Error DRV_VIDEO_UnsetHdmiVideoInputToOutput(HUINT32 ulDeviceId, NEXUS_HdmiInputHandle hdmiInput);
#endif

#if defined(CONFIG_HDOUT_2160P)
DRV_Error DRV_HDMI_SetColorInfo(DI_VIDEO_HD_RESOLUTION eFormat);
#endif

#if defined(CONFIG_DIRECTFB)
extern DFB_PlatformNexusHandle DFB_Platform_P_GetDisplayWindowHandle( DFB_PlatformNexusHandle displayHandle, int windowIndex );
#endif
#endif
/*******************************************************************/
/*********************** Static function prototypes ****************/
/*******************************************************************/
#if NEXUS_NUM_COMPOSITE_OUTPUTS > 0 || NEXUS_NUM_SVIDEO_OUTPUTS > 0 || NEXUS_NUM_COMPONENT_OUTPUTS > 0
static void *P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE outputMode);
#endif

#if defined(CONFIG_VIDEO_DECODER)
static void P_VIDEO_BuffMonitorTask(void *vpParam);
static void P_VIDEO_CopySeqHeaderInfo(HUINT32 ulDeviceId, DI_VIDEO_StreamInfo_t *stream_info);

#if defined(CONFIG_THUMBNAIL_DECODING)
DRV_Error DRV_VIDEO_GetThumbnailImage(HUINT32 ulDeviceId, NEXUS_SurfaceHandle *psurfaceHandle,DRV_PVR_THUMBNAIL_SETUP_t *pthumbSetup);
#endif

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
static pfnDI_VIDEO_Notify reschange_callback = NULL;
#endif

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
static void P_VIDEO_ResetDecoderClear(unsigned long TimerId, void *params)
{
	HUINT32 ulDeviceId=*(HUINT32 *)params;

	if (ulDeviceId>=NUM_VID_DECODER || s_stVideo[ulDeviceId].bInitialized==FALSE )
		return;

	PrintError("[%s] Decoder Reset Timer Clear %d\n",__func__,ulDeviceId);
	if(s_stVideo[ulDeviceId].ulResetTimerId == TimerId)
		s_stVideo[ulDeviceId].bDecReset=FALSE;

	s_stVideo[ulDeviceId].ulResetTimerId=VK_TIMERID_NULL;


}
#endif

#if defined(CONIFG_WORKAROUND_BVNSCALER_ERROR)
static void P_VIDEO_ApplyScalerTimer(unsigned long TimerId, void *params)
{
	HUINT32 ulDeviceId=*(HUINT32 *)params;
	DRV_Error nDrvErr = DRV_OK;

	if (ulDeviceId>=NUM_VID_DECODER || s_stVideo[ulDeviceId].bInitialized==FALSE )
		return;

	if(s_stVideo[ulDeviceId].ulApplyScalerTimerId == TimerId)
		s_stVideo[ulDeviceId].ulApplyScalerTimerId=VK_TIMERID_NULL;

	nDrvErr=DRV_VIDEO_ApplySetInOutWindows(ulDeviceId, TRUE);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] Display Apply Failed!\n",__func__);
	}
	else
	{
		PrintDebug("[%s] Display Apply Success!\n",__func__);
	}

}
#endif

#if defined(CONFIG_CLOSED_CAPTION)
static void P_VIDEO_ResetCcDataChecker(HUINT32 ulDeviceId)
{
	s_ulCcDataChecker[ulDeviceId] = 0;
}
#endif
static void P_VIDEO_EVTCall(HUINT32 ulDeviceId, DI_VIDEO_NOTIFY_EVT event, void* pvEventData)
{
	DI_VIDEO_CALLBACK_DATA_t stCallbackData;
	int iIndex;

	if ( ulDeviceId>=NUM_VID_DECODER 
	  || (s_stVideo[ulDeviceId].bInitialized==FALSE && event != DI_VIDEO_NOTIFY_EVT_CC_DATA_STATE_CHANGED) )
	{
		goto err;
	}

	for (iIndex=0 ; iIndex<TOTAL_CALLER_NUM ; iIndex++)
	{
		if (s_stVideo[ulDeviceId].stCallbackInfo[iIndex][event].pfnCallback == NULL)
			continue;

		/* Add event and state info to callback's param and call the event's callback */
		stCallbackData.ulDecId=ulDeviceId;
		stCallbackData.eEvent = event;
		stCallbackData.pvEventData = pvEventData;
		(*s_stVideo[ulDeviceId].stCallbackInfo[iIndex][event].pfnCallback)(&stCallbackData);
	}

err:
	return;
}
static void P_VIDEO_PtsPassedCallBack(void *context, int param)
{
#if defined(CONFIG_VIDEO_DECODER)
	HUINT32 ulDeviceId = param;
	NEXUS_VideoDecoderStatus decoderStatus;

	UNREFENCED_PARAM(context);

	PrintDebug("[%s] FirstPts is occurred %d \n",__func__,ulDeviceId);
	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder, &decoderStatus);

#if 0
	/* if being vsync mode, this bit is on */
	if(decoderStatus.tsm == false)
		s_stVideo[ulDeviceId].bFirstPtsPassed=TRUE;
#else
	s_stVideo[ulDeviceId].bFirstPtsPassed=TRUE;
#endif

#endif
}
static void P_VIDEO_FirstPtsPassedCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;	
	UNREFENCED_PARAM(context);
	PrintDebug("[%s] FirstPtsPassed is occurred %d \n",__func__,ulDeviceId);
#if defined(PRINT_VIDEO_PTS_INFO)
	NEXUS_VideoDecoderStatus decoderStatus;	
	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder, &decoderStatus);
	PrintError("FirstPtsPassed is occurred (%d) started (%d) TSM (%d) fifoDepth (%d)\n", ulDeviceId, decoderStatus.started, decoderStatus.tsm, decoderStatus.fifoDepth);
#endif	
	
	s_stVideo[ulDeviceId].bFirstPtsPassed=TRUE;
}

static void P_VIDEO_SourceChangedCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	UNREFENCED_PARAM(context);
	PrintDebug("[%s] Source Changed callback is occurred %d \n",__func__,ulDeviceId);
#if defined(CONFIG_DEINTERLACE_ONLY_P) /* Local define in this source */
	DRV_VIDEO_SetDeinterlaced(0,TRUE,FALSE);
#endif
	P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED, (void *)NULL);
}

static void P_VIDEO_StreamChangedCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	DI_VIDEO_StreamInfo_t stream_info;

	UNREFENCED_PARAM(context);

	/* check aspect ratio changed !*/
	P_VIDEO_CopySeqHeaderInfo(ulDeviceId, &stream_info);

	if(s_stVideoInfo[ulDeviceId].eAspectRatio == stream_info.eAspectRatio)
	{
		PrintDebug("[%s] aspect ratio is not changed\n",__func__);
	}
	else
	{
		s_stVideoInfo[ulDeviceId].eAspectRatio = stream_info.eAspectRatio;
		P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED, (void *)NULL);
		PrintDebug("[%s] Stream Changed callback is occurred\n",__func__);
	}


}

#if defined(CONFIG_CLOSED_CAPTION)
static void P_VIDEO_ClosedCaptionCallback(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	P_VIDEO_CC_MSG ccMsg; /*dummy*/

	HAPPY(context);

	if(VK_MSG_Send(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionMsgQueueId, &ccMsg, sizeof(P_VIDEO_CC_MSG)))
	{
		PrintError("[%s] VK_MSG_Send -error : ret\n", __FUNCTION__);
	}
}

#define CC_DATA_BUF_SIZE 128
static void P_VIDEO_ClosedCaptionTask(void *pvParam)
{
	NEXUS_Error	nexusError;

	NEXUS_ClosedCaptionData				ccEntry[CC_DATA_BUF_SIZE];
	DI_VIDEO_CC_ENTRY_t 				validEntry[CC_DATA_BUF_SIZE];
	DI_VIDEO_CC_DATA_CALLBACK_DATA_t 	stCallbackData;
	DI_VIDEO_CC_ENTRY_t 				*pCcDataBuf = NULL;
	P_VIDEO_CC_MSG	ccMsg;

	HUINT32 ulDeviceId;
	HUINT32 numEntries, numValidEntries;
	HUINT32 i;
	HBOOL	bNotify = FALSE;
		
	if(pvParam == NULL)
	{
		PrintError("Decoder ID parameter is NULL.\n");
		return;
	}

	ulDeviceId = *((HUINT32 *)pvParam);

	while (1)
	{
		VK_MEM_Memset(&ccMsg,0x00,sizeof(P_VIDEO_CC_MSG));
		
		if(VK_MSG_Receive(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionMsgQueueId, &ccMsg, sizeof(P_VIDEO_CC_MSG)))
		{	
			continue;
		}

		VK_SEM_Get(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);

		nexusError = NEXUS_VideoInput_ReadClosedCaption(NEXUS_VideoDecoder_GetConnector(s_stVideo[ulDeviceId].hDecoder), (NEXUS_ClosedCaptionData *)&ccEntry, CC_DATA_BUF_SIZE, &numEntries);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("can't get Closed Caption data. error (%d)\n",__func__,nexusError);
			VK_SEM_Release(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);
			continue;
		}
		
		bNotify = FALSE;
		for ( i=0, numValidEntries = 0; i < numEntries; i++ )
		{
			/* Check Closed Caption Data type */
#define EIA708_BIT_MASK  0x0002
			if(s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState != DI_VIDEO_CC_DATA_STATE_DATAFLOW)
			{
				if( ccEntry[ i ].field & EIA708_BIT_MASK )
				{
					/* Data type is EIA-708 */
					if( s_stVideo[ulDeviceId].stClosedCaptionInfo.eCcDataType != P_VIDEO_CC_DATA_TYPE_608 )
					{
						bNotify = TRUE;	
					}
				}
				else 
				{
					/* Data type is EIA-608 */
					if( s_stVideo[ulDeviceId].stClosedCaptionInfo.eCcDataType != P_VIDEO_CC_DATA_TYPE_708 )
					{
						bNotify = TRUE;		
					}
				}
			}

			/* Check Closed Caption Data */        	
			/* bit 1 of 'field' should be set for 708 data, clear for 608 data */
			/* NOTE THAT : Currently, Consider only 608. Todo implement 708 data prcessing*/
			if( (((ccEntry[ i ].field & EIA708_BIT_MASK) == 2) ^ ( s_stVideo[ulDeviceId].stClosedCaptionInfo.eCcDataType == P_VIDEO_CC_DATA_TYPE_608 )) 
				&& !ccEntry[i].noData )
			{
				/* DI_UART_Print("closed caption valid : field (%d) data (%#x) (%#x)\n",
								ccEntry[ i ].field,
								ccEntry[ i ].data[0],
								ccEntry[ i ].data[1]); */
				validEntry[ numValidEntries ].ccType  = (HUINT8)(ccEntry[ i ].field) ;
				validEntry[ numValidEntries ].data[0] = ccEntry[ i ].data[0] ;
				validEntry[ numValidEntries ].data[1] = ccEntry[ i ].data[1] ;
				numValidEntries++;
			}
		}

		if (numValidEntries == 0)
		{
			PrintDebug("No Valid Entries\n");
			VK_SEM_Release(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);
			continue;
		}

		/* Required CC data is exist, 
		    Therefore, Reset No Data checking count .*/
		P_VIDEO_ResetCcDataChecker(ulDeviceId);

		if(bNotify == TRUE)
		{    		
			/* Notify - Closed Caption Data State changed */
			static DI_VIDEO_CC_DATA_STATE_EVT_t ccDataState;
			ccDataState.eState = DI_VIDEO_CC_DATA_STATE_DATAFLOW;
			P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_CC_DATA_STATE_CHANGED, (void *)&ccDataState);

			PrintDebug("Notify DI_VIDEO_NOTIFY_EVT_CC_DATA_STATE_CHANGED (%d) --> (%d)\n",s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState,DI_VIDEO_CC_DATA_STATE_DATAFLOW);
			s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState = DI_VIDEO_CC_DATA_STATE_DATAFLOW;		
		}

		if(s_stVideo[ulDeviceId].stClosedCaptionInfo.bEnabledCcData == TRUE)
		{
			/* Deliver - Closed Caption Data using Callback*/
			if(s_stVideo[ulDeviceId].stCcCallbackInfo.pfnCcDataCallback)
			{
				pCcDataBuf = (DI_VIDEO_CC_ENTRY_t*)VK_MEM_Alloc(numValidEntries * sizeof(DI_VIDEO_CC_ENTRY_t));
				if(pCcDataBuf != NULL)
				{
					VK_memcpy(pCcDataBuf, validEntry, numValidEntries * sizeof(DI_VIDEO_CC_ENTRY_t));
					stCallbackData.ulDecId = ulDeviceId;
					stCallbackData.numValidEntries = numValidEntries;		
					stCallbackData.pCcEntry = pCcDataBuf;
					(*s_stVideo[ulDeviceId].stCcCallbackInfo.pfnCcDataCallback)(&stCallbackData);
					VK_MEM_Free(pCcDataBuf);
				}
			}
		}
		
		VK_SEM_Release(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);
	}
}
#endif

#if defined(CONFIG_AFD_EVENT)
static void P_VIDEO_AppUserDataReadyCallBack(void *context, int param)
{
	NEXUS_Error	nexusError;
	NEXUS_UserDataHeader *userDataHeader;
	unsigned appUserDataBufSize=0;
	HUINT8 *appUserDataBuf, *payload;
	HUINT32 ulData;
	HUINT32 ulappUserDataBufIdx=0;
	HUINT8 ucAfd=0;
	HUINT32 i=0;
	DI_VIDEO_ACTIVE_FORMAT_EVT_t activeFormat;
	HUINT32 ulDeviceId = param;

	UNREFENCED_PARAM(context);

	nexusError = NEXUS_VideoDecoder_GetUserDataBuffer(s_stVideo[ulDeviceId].hDecoder, (void **)&appUserDataBuf, &appUserDataBufSize);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] can't get user data. (%d)\n",__func__,nexusError);
		goto done;
	}

	if(appUserDataBufSize == 0 || appUserDataBufSize < (sizeof(NEXUS_UserDataHeader) + 10))
	{
		goto done;
	}

	userDataHeader = (NEXUS_UserDataHeader *)(appUserDataBuf + ulappUserDataBufIdx);
	if(userDataHeader->type == NEXUS_UserDataType_eSei)
	{
		while(ulappUserDataBufIdx < appUserDataBufSize)
		{
			userDataHeader = (NEXUS_UserDataHeader *)(appUserDataBuf + ulappUserDataBufIdx);

			payload = (HUINT8 *)((appUserDataBuf + ulappUserDataBufIdx) + (userDataHeader->blockSize - userDataHeader->payloadSize));

			for(i=0;i<(userDataHeader->payloadSize - 3);i++)
			{
				ulData = (payload[i] << 24) | (payload[i+1] << 16) | (payload[i+2] << 8) | (payload[i+3]);
				if(ulData == USER_DATA_START_CODE_AVC)
				{
					i+=4;
					break;
				}
			}

			ulappUserDataBufIdx += userDataHeader->blockSize;
			if(i >= userDataHeader->payloadSize - 6)
			{
				continue;
			}
			if(payload[i++] != 0x04)
			{
				continue;
			}
			i++; /*skip length info. **should be checked whether each header contains only 1 SEI or not. (assumed only one SEI is contained)*/
			for( ; i < (userDataHeader->payloadSize -5); i++)
			{
				ulData = (payload[i] << 24) | (payload[i+1] << 16) | (payload[i+2] << 8) | (payload[i+3]);
				if(ulData == AFD_IDENTIFIER)
				{
					i+=4;
					break;
				}
			}
			if(i >= userDataHeader->payloadSize -2)
			{
				continue;
			}

			/* check afd_alignment_bits '00 0001' */
			if((payload[i] & 0x3F) != AFD_ALIGNMENT_BITS)
			{
				continue;
			}

			if(payload[i++] & AFD_FORMAT_FLAG)
			{
				ucAfd = payload[i] & AFD_VALUE_MASK;
			}
			else
			{
				continue;
			}

			if(ucAfd != s_stVideoInfo[ulDeviceId].afd)
			{
				s_stVideoInfo[ulDeviceId].afd = ucAfd;
				activeFormat.format = (DI_VIDEO_ACTIVE_FORMAT)ucAfd;
				PrintDebug("[%d]%s ucAfd=%d\n",__LINE__,__func__,activeFormat.format);
				P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_AFD_CHANGED, (void *)&activeFormat);
			}
		}
	}
	else
	{
		while(ulappUserDataBufIdx < appUserDataBufSize)
		{
			userDataHeader = (NEXUS_UserDataHeader *)(appUserDataBuf + ulappUserDataBufIdx);

			payload = (HUINT8 *)((appUserDataBuf + ulappUserDataBufIdx) + (userDataHeader->blockSize - userDataHeader->payloadSize));

			for(i=0;i<(userDataHeader->payloadSize - 3);i++)
			{
				ulData = (payload[i] << 24) | (payload[i+1] << 16) | (payload[i+2] << 8) | (payload[i+3]);
				if(ulData == USER_DATA_START_CODE)
				{
					i+=4;
					break;
				}
			}

			ulappUserDataBufIdx += userDataHeader->blockSize;
			if(i >= userDataHeader->payloadSize - 6)
			{
				continue;
			}

			ulData = (payload[i] << 24) | (payload[i+1] << 16) | (payload[i+2] << 8) | (payload[i+3]);

			if(ulData == AFD_IDENTIFIER)
			{
				i+=4;
				if(payload[i++] & AFD_FORMAT_FLAG)
				{
					ucAfd = payload[i] & AFD_VALUE_MASK;
				}
			}
			else
			{
				continue;
			}

			if(ucAfd != s_stVideoInfo[ulDeviceId].afd)
			{
				s_stVideoInfo[ulDeviceId].afd = ucAfd;
				activeFormat.format = (DI_VIDEO_ACTIVE_FORMAT)ucAfd;
				PrintDebug("[%d]%s ucAfd=%d\n",__LINE__,__func__,activeFormat.format);
				P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_AFD_CHANGED, (void *)&activeFormat);
			}
		}
	}
done:
	NEXUS_VideoDecoder_UserDataReadComplete(s_stVideo[ulDeviceId].hDecoder, appUserDataBufSize);
	NEXUS_VideoDecoder_FlushUserData(s_stVideo[ulDeviceId].hDecoder);
}
#endif

static void P_VIDEO_ptsErrorCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	UNREFENCED_PARAM(context);
	PrintDebug("[%s] ptsError callback is occurred (%d)\n",__func__,ulDeviceId);
#if defined(PRINT_VIDEO_PTS_INFO)
	NEXUS_VideoDecoderStatus decoderStatus;	
	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder, &decoderStatus);
	PrintError("ptsError callback is occurred (%d) started (%d) TSM (%d) fifoDepth (%d)\n", ulDeviceId, decoderStatus.started, decoderStatus.tsm, decoderStatus.fifoDepth );
#endif	

/* 2010_08_05 Viaccess Driver test ,
Even/Odd Key중 하나만 사용할 경우 10초간 폴리고 10초가 풀리지 않는 테스트
풀리지 않는 10초 구간 진입 후 Video가 끊김 현상 발생
임시 패치 VA_DRIVER_TEST를 product.config에 사용하는 경우에만 적용 */

#if defined(VA_DRIVER_TEST)
	DRV_VIDEO_Restart(0,TRUE);
#endif
}

static void P_VIDEO_fifoEmptyCallBack(void *context, int param)
{
	UNREFENCED_PARAM(context);
	UNREFENCED_PARAM(param);
	PrintDebug("[%s] fifoEmpty callback is occurred\n",__func__);
}
#if defined(CONFIG_DI20) && defined(CONFIG_3D_VIDEO)
static void P_VIDEO_3DInfoChangeCallback(void *context, int param)
{
	NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
	NEXUS_VideoDecoder3DTVStatus status;
	DI_VIDEO_3D_INFO_EVT_t video3dInfo;
	HUINT32 ulDeviceId = param;

	NEXUS_VideoDecoder_Get3DTVStatus(videoDecoder, &status);

	switch(status.format)
	{
		case NEXUS_VideoDecoder3DTVFormat_eNone:
			video3dInfo.e3dFormat = DI_VIDEO_3D_FORMAT_NONE;
			break;
		case NEXUS_VideoDecoder3DTVFormat_eSideBySideHalf:
			video3dInfo.e3dFormat = DI_VIDEO_3D_FORMAT_SIDEBYSIDE;
			break;
		case NEXUS_VideoDecoder3DTVFormat_eTopAndBottomHalf:
			video3dInfo.e3dFormat = DI_VIDEO_3D_FORMAT_TOPANDBOTTOM;
			break;
		default:
			video3dInfo.e3dFormat = DI_VIDEO_3D_FORMAT_NONE;	/* 2D */
			break;
	}

	s_stVideo[ulDeviceId].e3dFormat = video3dInfo.e3dFormat;

	PrintDebug("[%s] : 3D format=%d\r\n", __func__, video3dInfo.e3dFormat);

	/* At this point, the application should set the necessary 3D configuration for its platform,
	   based on the reported 3DTV format. For example, DTVs should configure the video windows;
	   Settops should configure the HDMI VSI. */
	P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED, (void *)&video3dInfo);
}
#endif

#if defined(CONFIG_VIDEO_ERROR_LOG)
static void P_VIDEO_decodeErrorTimer(unsigned long TimerId, void *params)
{
#if defined(CONFIG_VIDEO_DECODER)
	HUINT32 ulDeviceId=*(HUINT32 *)params;
	DI_VIDEO_ERROR_INFO_EVT_t stErrInfo;
	NEXUS_VideoDecoderStatus decoderStatus;

	if(s_stVideo[ulDeviceId].ulErrorTimerId == TimerId)
	{
		NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);
		stErrInfo.ulDecodeError = decoderStatus.numDecodeErrors;
		stErrInfo.ulDisplayError = decoderStatus.numDisplayErrors;
		PrintDebug("decode error=%d, display error=%d\n", decoderStatus.numDecodeErrors, decoderStatus.numDisplayErrors);
		P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_ERROR, (void *)&stErrInfo);
		s_stVideo[ulDeviceId].ulErrorTimerId=VK_TIMERID_NULL;
	}
#endif
}

static void P_VIDEO_decodeErrorCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;

	UNREFENCED_PARAM(context);

	if(s_stVideo[ulDeviceId].ulErrorTimerId == VK_TIMERID_NULL)
	{
		VK_TIMER_EventAfter(VIDEO_ERROR_EVENT_TIME, P_VIDEO_decodeErrorTimer,&ulDeviceId,sizeof(ulDeviceId),(unsigned long *) &s_stVideo[ulDeviceId].ulErrorTimerId);
	}
}
#endif

#if defined(CONFIG_INRECT_RATIO)
static HUINT16 P_VIDEO_CalcPixelPosition(HUINT16 ref_pos, HUINT16 ratio, HUINT16 pos_max)
{
#define VIDEO_IN_MAX_RATIO 	0xFFFF
	HUINT32 position;
	HUINT32 lratio;
	HUINT32 maximum;
	HUINT32 result;
#if defined (CONFIG_MHEG_SUPPORT)
#else
	HUINT32 check;
#endif

	position = (HUINT32)ref_pos;
	lratio = (HUINT32)ratio;
	maximum = (HUINT32)pos_max;

#if defined (CONFIG_MHEG_SUPPORT)
/* Added by iwnam, 2009/04/23, use the typcasted variables */
	result = position + (lratio * maximum + VIDEO_IN_MAX_RATIO/2) / VIDEO_IN_MAX_RATIO;
#else
	if(ratio != 0)
	{
		check=VIDEO_IN_MAX_RATIO%ratio;
		if(check <= 1)
			lratio = VIDEO_IN_MAX_RATIO;
	}

	/* Added by iwnam, 2009/04/23, use the typcasted variables */
	result = position + (lratio * maximum) / VIDEO_IN_MAX_RATIO;
#endif

	return (HUINT16)result;
#undef VIDEO_IN_MAX_RATIO
}
#endif

/* Added by iwnam, 2009/04/23, don't need to calculate output pixel position */
#if 0
static HUINT16 P_VIDEO_CalcOutPixelPosition(HUINT16 pos, HUINT16 ref_pos, HUINT16 pos_max)
{
	HUINT32 ulpos = (HUINT32)pos;
	HUINT32 ulref_pos = (HUINT32)ref_pos;
	HUINT32 ulpos_max = (HUINT32)pos_max;
	HUINT32 result;

	result = (ulpos * ulpos_max) / ulref_pos;

	return (HUINT16)result;
}
#endif

static NEXUS_TransportType P_VIDEO_Convert2NexusTransport(DI_VIDEO_STREAM_TYPE nType)
{
	NEXUS_TransportType eTransportType;

	switch(nType)
	{
		case DI_VIDEO_STREAM_TYPE_ES:
			eTransportType = NEXUS_TransportType_eEs;
			break;

		case DI_VIDEO_STREAM_TYPE_PES:
			eTransportType = NEXUS_TransportType_eMpeg2Pes;
			break;

		case DI_VIDEO_STREAM_TYPE_TS:
			eTransportType = NEXUS_TransportType_eTs;
			break;

		case DI_VIDEO_STREAM_TYPE_VOB:
			eTransportType = NEXUS_TransportType_eVob;
			break;

		case DI_VIDEO_STREAM_TYPE_MPEG1PS:
			eTransportType = NEXUS_TransportType_eMpeg1Ps;
			break;

		case DI_VIDEO_STREAM_TYPE_DSSES:
			eTransportType = NEXUS_TransportType_eDssEs;
			break;

		case DI_VIDEO_STREAM_TYPE_DSSPES:
			eTransportType = NEXUS_TransportType_eDssPes;
			break;

		case DI_VIDEO_STREAM_TYPE_ASF:
			eTransportType = NEXUS_TransportType_eAsf;
			break;

		case DI_VIDEO_STREAM_TYPE_AVI:
			eTransportType = NEXUS_TransportType_eAvi;
			break;

		case DI_VIDEO_STREAM_TYPE_MP4:
			eTransportType = NEXUS_TransportType_eMp4;
			break;

		case DI_VIDEO_STREAM_TYPE_FLV:
			eTransportType = NEXUS_TransportType_eFlv;
			break;

		case DI_VIDEO_STREAM_TYPE_MKV:
			eTransportType = NEXUS_TransportType_eFlv;
			break;

		case DI_VIDEO_STREAM_TYPE_WAV:
			eTransportType = NEXUS_TransportType_eWav;
			break;

		default:
			PrintError("Invalid parameter nType(%d)\n", nType);
			eTransportType = NEXUS_TransportType_eUnknown;
			break;
	}

	return eTransportType;
}

static NEXUS_VideoCodec P_VIDEO_Convert2NexusCodec(DI_VIDEO_FORMAT VideoCodec)
{
	NEXUS_VideoCodec nexusVideoCodec;

	switch(VideoCodec)
	{
		case DI_VIDEO_FORMAT_MPEG1:
		case DI_VIDEO_FORMAT_MEDIA_MPEG1:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg1;
			break;
		case DI_VIDEO_FORMAT_MPEG2:
		case DI_VIDEO_FORMAT_MEDIA_MPEG2:
		case DI_VIDEO_FORMAT_STILL_MPEG2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg2;
			break;
		case DI_VIDEO_FORMAT_MPEG4Part2:
		case DI_VIDEO_FORMAT_MEDIA_MPEG4Part2:
			nexusVideoCodec = NEXUS_VideoCodec_eMpeg4Part2;
			break;
		case DI_VIDEO_FORMAT_H261:	//divx test
		case DI_VIDEO_FORMAT_MEDIA_DIVX_311:
			// nexusVideoCodec = NEXUS_VideoCodec_eUnknown; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			nexusVideoCodec = NEXUS_VideoCodec_eDivx311; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			break;
		case DI_VIDEO_FORMAT_H263:
		case DI_VIDEO_FORMAT_MEDIA_H263:
			nexusVideoCodec = NEXUS_VideoCodec_eH263;
			break;
		case DI_VIDEO_FORMAT_H264:
		case DI_VIDEO_FORMAT_MEDIA_H264:
		case DI_VIDEO_FORMAT_STILL_H264:
			nexusVideoCodec = NEXUS_VideoCodec_eH264;
			break;
#if defined(CONFIG_VIDEO_DEC_H265)
		case DI_VIDEO_FORMAT_H265:
		case DI_VIDEO_FORMAT_MEDIA_H265:
			nexusVideoCodec = NEXUS_VideoCodec_eH265;
			break;
#endif
#if defined(CONFIG_VIDEO_DEC_VP8)
		case DI_VIDEO_FORMAT_VP8:
		case DI_VIDEO_FORMAT_MEDIA_VP8:
			nexusVideoCodec = NEXUS_VideoCodec_eVp8;
			break;
#endif
#if defined(CONFIG_VIDEO_DEC_VP9)
		case DI_VIDEO_FORMAT_VP9:
		case DI_VIDEO_FORMAT_MEDIA_VP9:
			nexusVideoCodec = NEXUS_VideoCodec_eVp9;
			break;
#endif
		case DI_VIDEO_FORMAT_VC1SimpleMain:
		case DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1SimpleMain;
			break;
		case DI_VIDEO_FORMAT_VC1:
		case DI_VIDEO_FORMAT_MEDIA_VC1:
		case DI_VIDEO_FORMAT_STILL_VC1:
			nexusVideoCodec = NEXUS_VideoCodec_eVc1;
			break;
		case DI_VIDEO_FORMAT_AVS:
		case DI_VIDEO_FORMAT_MEDIA_AVS:
			nexusVideoCodec = NEXUS_VideoCodec_eAvs;
			break;
		default:
			nexusVideoCodec = NEXUS_VideoCodec_eUnknown;
			break;
	}
	return nexusVideoCodec;
}

static DI_VIDEO_FORMAT P_VIDEO_Convert2DICodec(NEXUS_VideoCodec VideoCodec)
{
	DI_VIDEO_FORMAT DiVideoCodec;

	switch(VideoCodec)
	{
		case NEXUS_VideoCodec_eMpeg1:
			DiVideoCodec = DI_VIDEO_FORMAT_MPEG1;
			break;
		case NEXUS_VideoCodec_eMpeg2:
			DiVideoCodec = DI_VIDEO_FORMAT_MPEG2;
			break;
		case NEXUS_VideoCodec_eMpeg4Part2:
			DiVideoCodec = DI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case NEXUS_VideoCodec_eDivx311: /* Divx3.11 Test */
			//			nexusVideoCodec = NEXUS_VideoCodec_eUnknown; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			DiVideoCodec = DI_VIDEO_FORMAT_H261; /*seemed to be unsupported in Nexus platform, but can be modified.*/
			break;
		case NEXUS_VideoCodec_eH263:
			DiVideoCodec = DI_VIDEO_FORMAT_H263;
			break;
		case NEXUS_VideoCodec_eH264:
			DiVideoCodec = DI_VIDEO_FORMAT_H264;
			break;
#if defined(CONFIG_VIDEO_DEC_H265)
		case NEXUS_VideoCodec_eH265:
			DiVideoCodec = DI_VIDEO_FORMAT_H265;
			break;
#endif
#if defined(CONFIG_VIDEO_DEC_VP8)
		case NEXUS_VideoCodec_eVp8:
			DiVideoCodec = DI_VIDEO_FORMAT_VP8;
			break;
#endif
#if defined(CONFIG_VIDEO_DEC_VP9)
		case NEXUS_VideoCodec_eVp9:
			DiVideoCodec = DI_VIDEO_FORMAT_VP9;
			break;
#endif
		case NEXUS_VideoCodec_eVc1SimpleMain:
			DiVideoCodec = DI_VIDEO_FORMAT_VC1SimpleMain;
			break;
		case NEXUS_VideoCodec_eVc1:
			DiVideoCodec = DI_VIDEO_FORMAT_VC1;
			break;
		case NEXUS_VideoCodec_eAvs:
			DiVideoCodec = DI_VIDEO_FORMAT_AVS;
			break;
		default:
			DiVideoCodec = DI_VIDEO_FORMAT_MPEG1; /* Need to define DI_VIDEO_FORMAT_UNKNOWN */
			break;
	}
	return DiVideoCodec;
}


static NEXUS_VideoFormat P_VIDEO_Convert2NexusVideoFormat(DI_VIDEO_HD_RESOLUTION eHDOutput)
{
	NEXUS_VideoFormat nexusVideoFormat;

	switch(eHDOutput)
	{
		case DI_VIDEO_HD_RESOLUTION_480I:
			nexusVideoFormat = NEXUS_VideoFormat_eNtsc;
			break;
		case DI_VIDEO_HD_RESOLUTION_480I_J:
			nexusVideoFormat = NEXUS_VideoFormat_eNtscJapan;
			break;
		case DI_VIDEO_HD_RESOLUTION_480P:
			nexusVideoFormat = NEXUS_VideoFormat_e480p;
			break;
		case DI_VIDEO_HD_RESOLUTION_576I:
			nexusVideoFormat = NEXUS_VideoFormat_ePal;
			break;
		case DI_VIDEO_HD_RESOLUTION_576P:
			nexusVideoFormat = NEXUS_VideoFormat_e576p;
			break;
		case DI_VIDEO_HD_RESOLUTION_720P:
			nexusVideoFormat = NEXUS_VideoFormat_e720p;
			break;
		case DI_VIDEO_HD_RESOLUTION_1080I:
			nexusVideoFormat = NEXUS_VideoFormat_e1080i;
			break;
		case DI_VIDEO_HD_RESOLUTION_720P_50HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e720p50hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_1080I_50HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e1080i50hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_1080P_50HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e1080p50hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_1080P:						/* 1080p 60Hz */
			nexusVideoFormat = NEXUS_VideoFormat_e1080p;		/* NEXUS_VideoFormat_e1080p is same with NEXUS_VideoFormat_e1080p60hz */
			break;
		case DI_VIDEO_HD_RESOLUTION_1080P_30HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e1080p30hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_1080P_24HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e1080p24hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_1080P_25HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e1080p25hz;
			break;
#if defined(CONFIG_HDOUT_2160P)
		case DI_VIDEO_HD_RESOLUTION_2160P_24HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e3840x2160p24hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_2160P_25HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e3840x2160p25hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_2160P_30HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e3840x2160p30hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_2160P_50HZ:
			nexusVideoFormat = NEXUS_VideoFormat_e3840x2160p50hz;
			break;
		case DI_VIDEO_HD_RESOLUTION_2160P:
			nexusVideoFormat = NEXUS_VideoFormat_e3840x2160p60hz;
			break;
#endif
		default:
			nexusVideoFormat = NEXUS_VideoFormat_e1080i50hz;
			break;
	}
	return nexusVideoFormat;
}

static NEXUS_DisplayAspectRatio P_VIDEO_Convert2NexusAspectRatio(DI_VIDEO_AspectRatio_t  eAspectRatio)
{
	NEXUS_DisplayAspectRatio nexusAspectRatio;

	switch(eAspectRatio)
	{
		case DI_VIDEO_ASPECTRATIO_4_3:
		case DI_VIDEO_ASPECTRATIO_4_3_LETTER:
			nexusAspectRatio=NEXUS_DisplayAspectRatio_e4x3;
			break;

		case DI_VIDEO_ASPECTRATIO_16_9:
			nexusAspectRatio=NEXUS_DisplayAspectRatio_e16x9;
			break;
		case DI_VIDEO_ASPECTRATIO_SAR:
			nexusAspectRatio=NEXUS_DisplayAspectRatio_eSar;
			break;
		case DI_VIDEO_ASPECTRATIO_14_9:
		case DI_VIDEO_ASPECTRATIO_1_1:
		case DI_VIDEO_ASPECTRATIO_UNKNOWN:
		case DI_VIDEO_ASPECTRATIO_MAX:
		default:
			nexusAspectRatio=NEXUS_DisplayAspectRatio_eAuto;
			break;
	}

	return nexusAspectRatio;
}

static DI_VIDEO_AspectRatio_t P_VIDEO_Convert2DIAspectRatio(HUINT32 ulDeviceId, NEXUS_VideoDecoderStreamInformation *streamInfo)
{
	int ratio=0;
	DI_VIDEO_AspectRatio_t DiAspectRatio=DI_VIDEO_ASPECTRATIO_16_9;

	switch(streamInfo->aspectRatio)
	{
		case NEXUS_AspectRatio_eUnknown:
			DiAspectRatio = DI_VIDEO_ASPECTRATIO_UNKNOWN;
			break;
		case NEXUS_AspectRatio_e4x3:
			DiAspectRatio = DI_VIDEO_ASPECTRATIO_4_3;
			break;
		case NEXUS_AspectRatio_e16x9:
			DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
			break;
		case NEXUS_AspectRatio_eSquarePixel:
			DiAspectRatio = DI_VIDEO_ASPECTRATIO_1_1;
			break;
		case NEXUS_AspectRatio_eSar:
			if(s_stVideo[ulDeviceId].eVidCodec == DI_VIDEO_FORMAT_MPEG4Part2 ||
			s_stVideo[ulDeviceId].eVidCodec == DI_VIDEO_FORMAT_MEDIA_MPEG4Part2)
			{
				ratio=((float)streamInfo->displayHorizontalSize/streamInfo->displayVerticalSize)*10;
				if(abs(ratio - 13) < abs(ratio - 17))
				{
					DiAspectRatio = DI_VIDEO_ASPECTRATIO_4_3;
				}
				else
				{
#if defined(CONFIG_DI20) && (defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO))
					if(s_stVideo[ulDeviceId].bIsMedia)
					{
						int ratio1=(streamInfo->sourceVerticalSize*streamInfo->sampleAspectRatioY);
						int ratio2=(streamInfo->sourceHorizontalSize*streamInfo->sampleAspectRatioX);
						ratio=((float)ratio2/ratio1)*10;

						if(ratio==17) /* 16/9*10 = 17.7777778 */
						{
							DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
						}
						else
						{
						DiAspectRatio = DI_VIDEO_ASPECTRATIO_UNKNOWN;
						}
					}
					else
					{
						DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
					}
#else
					DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
#endif
				}
			}
			else
			{
				switch(streamInfo->sampleAspectRatioX*streamInfo->sampleAspectRatioY)
				{
					case DRV_VIDEO_SAR_1211:
					case DRV_VIDEO_SAR_1011:
					case DRV_VIDEO_SAR_1811:
					case DRV_VIDEO_SAR_1511:
					case DRV_VIDEO_SAR_2215:
					case DRV_VIDEO_SAR_2218:
					case DRV_VIDEO_SAR_3211:
					case DRV_VIDEO_SAR_8033:
					case DRV_VIDEO_SAR_6433:
					case DRV_VIDEO_SAR_16099:
					case DRV_VIDEO_SAR_1611:
					case DRV_VIDEO_SAR_4033:
					case DRV_VIDEO_SAR_2411:
					case DRV_VIDEO_SAR_2011:
					case DRV_VIDEO_SAR_11:
					default:
						if((streamInfo->sampleAspectRatioX == 0)||(streamInfo->sourceHorizontalSize == 0))
						{
							DiAspectRatio = s_stVideoInfo[ulDeviceId].eAspectRatio;
							if(DiAspectRatio >= DI_VIDEO_ASPECTRATIO_UNKNOWN)
							{
								DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
							}
						}
						else
						{
							if(((streamInfo->sourceVerticalSize*streamInfo->sampleAspectRatioY)*10)/
								((streamInfo->sourceHorizontalSize*streamInfo->sampleAspectRatioX)) == 7)
							{
								DiAspectRatio = DI_VIDEO_ASPECTRATIO_4_3;
							}
							else
							{
#if defined(CONFIG_DI20) && (defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO))
								if(s_stVideo[ulDeviceId].bIsMedia)
								{

									int ratio1=(streamInfo->sourceVerticalSize*streamInfo->sampleAspectRatioY);
									int ratio2=(streamInfo->sourceHorizontalSize*streamInfo->sampleAspectRatioX);
									ratio=((float)ratio2/ratio1)*10;

									if(ratio==17) /* 16/9*10 = 17.7777778 */
									{
										DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
									}
									else
									{
									DiAspectRatio = DI_VIDEO_ASPECTRATIO_UNKNOWN;
									}
								}
								else
								{
									DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
								}
#else
								DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
#endif
							}
						}
						break;
				}
			}
			break;
		default:
			DiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
			break;
	}

	PrintDebug("\n\n[%d]%s SAR(X=%d:Y=%d (sv:%d) %s \n\n",__LINE__,__func__,streamInfo->sampleAspectRatioX,streamInfo->sampleAspectRatioY,
					streamInfo->sampleAspectRatioX*streamInfo->sampleAspectRatioY,(DiAspectRatio == 0) ? "16:9":((DiAspectRatio == 1) ? "4:3" : "Unknown"));

	return DiAspectRatio;
}

static DI_VIDEO_FrameRate_t P_VIDEO_Convert2DIFrameRate(NEXUS_VideoFrameRate frameRate)
{

	DI_VIDEO_FrameRate_t DiFrameRate;

	switch(frameRate)
	{
		case NEXUS_VideoFrameRate_e23_976:
		case NEXUS_VideoFrameRate_e24:
			DiFrameRate = DI_VIDEO_FRAMERATE_24;
			break;
		case NEXUS_VideoFrameRate_e25:
			DiFrameRate = DI_VIDEO_FRAMERATE_25;
			break;
		case NEXUS_VideoFrameRate_e29_97:
		case NEXUS_VideoFrameRate_e30:
			DiFrameRate = DI_VIDEO_FRAMERATE_30;
			break;
		case NEXUS_VideoFrameRate_e50:
			DiFrameRate = DI_VIDEO_FRAMERATE_50;
			break;
		case NEXUS_VideoFrameRate_e59_94:
		case NEXUS_VideoFrameRate_e60:
			DiFrameRate = DI_VIDEO_FRAMERATE_60;
			break;
		default:
			DiFrameRate = DI_VIDEO_FRAMERATE_50;
			break;
	}

	return DiFrameRate;

}

static NEXUS_VideoFrameRate P_VIDEO_Convert2NexusFrameRate(DI_VIDEO_FrameRate_t eFrameRate)
{

	NEXUS_VideoFrameRate NexusFrameRate;

	switch(eFrameRate)
	{
		case DI_VIDEO_FRAMERATE_24:
			NexusFrameRate = NEXUS_VideoFrameRate_e24;
			break;
#if 0
		case NEXUS_VideoFrameRate_e23_976:
			break;
#endif
		case DI_VIDEO_FRAMERATE_25:
			NexusFrameRate = NEXUS_VideoFrameRate_e25;
			break;
#if 0
		case NEXUS_VideoFrameRate_e29_97:
			break;
#endif
		case DI_VIDEO_FRAMERATE_30:
			NexusFrameRate = NEXUS_VideoFrameRate_e30;
			break;

		case DI_VIDEO_FRAMERATE_50:
			NexusFrameRate = NEXUS_VideoFrameRate_e50;
			break;
#if 0
		case NEXUS_VideoFrameRate_e59_94:
			break;
#endif

		case DI_VIDEO_FRAMERATE_60:
			NexusFrameRate = NEXUS_VideoFrameRate_e60;
			break;

		default:
			NexusFrameRate = NEXUS_VideoFrameRate_e50;
			break;
	}

	return NexusFrameRate;

}

static DI_VIDEO_PROTOCOL_PROFILE_TYPE P_VIDEO_Convert2DIProfile(NEXUS_VideoProtocolProfile eVideoProtocolProfile)
{
	DI_VIDEO_PROTOCOL_PROFILE_TYPE eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_UNKNOWN;

	switch(eVideoProtocolProfile)
	{
		case NEXUS_VideoProtocolProfile_eSimple:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_SIMPLE;
			break;
		case NEXUS_VideoProtocolProfile_eMain:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_MAIN;
			break;
		case NEXUS_VideoProtocolProfile_eHigh:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_HIGH;
			break;
		case NEXUS_VideoProtocolProfile_eAdvanced:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_ADVANCED;
			break;
		case NEXUS_VideoProtocolProfile_eJizhun:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_JIZHUN;
			break;
		case NEXUS_VideoProtocolProfile_eSnrScalable:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_SNRSCALABLE;
			break;
		case NEXUS_VideoProtocolProfile_eSpatiallyScalable:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_SPATIALLYSCALABLE;
			break;
		case NEXUS_VideoProtocolProfile_eAdvancedSimple:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_ADVANCEDSIMPLE;
			break;
		case NEXUS_VideoProtocolProfile_eBaseline:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_BASELINE;
			break;
		default:
			eDiVideoProtocolProfile = DI_VIDEO_PROTOCOL_PROFILE_UNKNOWN;
			break;
	}

	return eDiVideoProtocolProfile;
}

static DI_VIDEO_PROTOCOL_LEVEL_TYPE P_VIDEO_Convert2DILevel(NEXUS_VideoProtocolLevel eVideoProtocolLevel)
{
	DI_VIDEO_PROTOCOL_LEVEL_TYPE DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_UNKNOWN;

	switch(eVideoProtocolLevel)
	{
		case NEXUS_VideoProtocolLevel_e00:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_00;
			break;
		case NEXUS_VideoProtocolLevel_e10:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_10;
			break;
		case NEXUS_VideoProtocolLevel_e1B:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_1B;
			break;
		case NEXUS_VideoProtocolLevel_e11:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_11;
			break;
		case NEXUS_VideoProtocolLevel_e12:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_12;
			break;
		case NEXUS_VideoProtocolLevel_e13:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_13;
			break;
		case NEXUS_VideoProtocolLevel_e20:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_20;
			break;
		case NEXUS_VideoProtocolLevel_e21:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_21;
			break;
		case NEXUS_VideoProtocolLevel_e22:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_22;
			break;
		case NEXUS_VideoProtocolLevel_e30:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_30;
			break;
		case NEXUS_VideoProtocolLevel_e31:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_31;
			break;
		case NEXUS_VideoProtocolLevel_e32:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_32;
			break;
		case NEXUS_VideoProtocolLevel_e40:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_40;
			break;
		case NEXUS_VideoProtocolLevel_e41:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_41;
			break;
		case NEXUS_VideoProtocolLevel_e42:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_42;
			break;
		case NEXUS_VideoProtocolLevel_e50:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_50;
			break;
		case NEXUS_VideoProtocolLevel_e51:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_51;
			break;
		case NEXUS_VideoProtocolLevel_e60:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_60;
			break;
		case NEXUS_VideoProtocolLevel_e62:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_62;
			break;
		case NEXUS_VideoProtocolLevel_eLow:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_LOW;
			break;
		case NEXUS_VideoProtocolLevel_eMain:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_MAIN;
			break;
		case NEXUS_VideoProtocolLevel_eHigh:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_HIGH;
			break;
		case NEXUS_VideoProtocolLevel_eHigh1440:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_HIGH1440;
			break;
		default:
			DiVideoProtocolLevel = DI_VIDEO_PROTOCOL_LEVEL_UNKNOWN;
			break;
	}

	return DiVideoProtocolLevel;
}

static void P_VIDEO_GetDisplaySize(NEXUS_VideoFormat format, HUINT16 *usWidth, HUINT16 *usHeight)
{
	switch ( format )
	{
		case NEXUS_VideoFormat_e1080i:
		case NEXUS_VideoFormat_e1080i50hz:
		case NEXUS_VideoFormat_e1080p:
		case NEXUS_VideoFormat_e1080p24hz:
		case NEXUS_VideoFormat_e1080p25hz:
		case NEXUS_VideoFormat_e1080p50hz:
		case NEXUS_VideoFormat_e1080p30hz:
			*usWidth = 1920;
			*usHeight = 1080;
			break;
		case NEXUS_VideoFormat_e720p50hz:
		case NEXUS_VideoFormat_e720p:
			*usWidth = 1280;
			*usHeight = 720;
			break;
		case NEXUS_VideoFormat_ePal:
		case NEXUS_VideoFormat_ePalI:
		case NEXUS_VideoFormat_e576p:
			*usWidth = 720;
			*usHeight = 576;
			break;
		case NEXUS_VideoFormat_eNtsc:
		case NEXUS_VideoFormat_eNtscJapan:
		case NEXUS_VideoFormat_e480p:
			*usWidth = 720;
			*usHeight = 480;
			break;
#if defined(CONFIG_HDOUT_2160P)
		case NEXUS_VideoFormat_e3840x2160p24hz:
		case NEXUS_VideoFormat_e3840x2160p25hz:
		case NEXUS_VideoFormat_e3840x2160p30hz:
		case NEXUS_VideoFormat_e3840x2160p50hz:
		case NEXUS_VideoFormat_e3840x2160p60hz:
			*usWidth = 3840;
			*usHeight = 2160;
			break;
#endif
		default:
			/* Added by iwnam, 2009/04/23, change the default resolution */
			*usWidth = 720;
			*usHeight = 576;
			PrintError("Not supported disp format(0x%x)\n", format);
			break;
	}
}

static void P_VIDEO_CopySeqHeaderInfo(HUINT32 ulDeviceId, DI_VIDEO_StreamInfo_t *stream_info)
{
#if defined(CONFIG_VIDEO_DECODER)
	NEXUS_VideoDecoderStreamInformation streamInfo;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoDecoderStartSettings *startSettings;
	NEXUS_Error	nexusError;
	HUINT32 ulWindowId = ulDeviceId;

	if (ulDeviceId>=NUM_VID_DECODER || s_stVideo[ulDeviceId].bInitialized==FALSE || VK_SEM_Get(s_stVideo[ulDeviceId].ulSemId_P)!=VK_OK)
		return;

	/* Get current startsettings */
	startSettings = &s_stVideo[ulDeviceId].StartSettings;

	/* Get Stream info */
	nexusError=NEXUS_VideoDecoder_GetStreamInformation(s_stVideo[ulDeviceId].hDecoder, &streamInfo);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] can't get stream information. (%d)\n",__func__,nexusError);
		goto done;
	}

	/* Get one of the windows settings */
	NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);

	stream_info->ulWidth = streamInfo.sourceHorizontalSize;
	stream_info->ulHeight = streamInfo.sourceVerticalSize;
	stream_info->ulDisplayWidth = streamInfo.displayHorizontalSize;
	stream_info->ulDisplayHeight = streamInfo.displayVerticalSize;
	stream_info->eAspectRatio = P_VIDEO_Convert2DIAspectRatio(ulDeviceId,&streamInfo);
	stream_info->eFrameRate = P_VIDEO_Convert2DIFrameRate(streamInfo.frameRate);
	stream_info->eVideoFormat=P_VIDEO_Convert2DICodec(startSettings->codec);
	stream_info->bProgressive = streamInfo.streamProgressive;
	stream_info->ucColorPrimarie = streamInfo.colorPrimaries;
	stream_info->ucAlpha = windowSettings.alpha;

	PrintDebug("[%s]\t Stream info ulWidth = %d\n",__func__,stream_info->ulWidth);
	PrintDebug("[%s]\t Stream info ulHeight= %d\n",__func__,stream_info->ulHeight);
	PrintDebug("[%s]\t Stream info eAspectRatio = %d\n",__func__,stream_info->eAspectRatio);
	PrintDebug("[%s]\t Stream info eFrameRate = %d\n",__func__,stream_info->eFrameRate);
	PrintDebug("[%s]\t Stream info eVideoFormat = %d\n",__func__,stream_info->eVideoFormat);
	PrintDebug("[%s]\t Stream info bProgressive = %d\n",__func__,stream_info->bProgressive);
	PrintDebug("[%s]\t Stream info ulColorPrimarie = %d\n",__func__,stream_info->ucColorPrimarie);

	/* To Do : What should both of the parameters assigne */
	stream_info->ucVideoFormat = 0;
	stream_info->ucChromaFormat = 0;

done:
	VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_P);
	return;
#endif
}

DRV_Error P_VIDEO_DelayDecoder(HUINT32 ulDevId, HINT32 uiDelay)
{
	NEXUS_Error nResult;
	NEXUS_VideoDecoderSettings nSettings;

	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDevId].hDecoder, &nSettings);

	nSettings.ptsOffset = uiDelay*45;

	nResult = NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDevId].hDecoder, &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("P_VIDEO_DelayDecoder failed!\n");
		return DRV_ERR;
	}
	return DRV_OK;
}

static HBOOL P_VIDEO_CheckInOutWindow(DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pOutRect, DI_VIDEO_RECT_t *pPrevInRect, DI_VIDEO_RECT_t *pPrevOutRect)
{
	unsigned long ulSum=0;

	/*
	 * Compare with the previous setting. If same, ignore.
	 * Note : Do not use "memcmp", because it compares even the stuffing bytes that is put for alignment.
	 *
	 * 2009.5.10 dcchung
	 */
	if ((pInRect->ulX==pPrevInRect->ulX && pInRect->ulY==pPrevInRect->ulY
		&& pInRect->ulWidth==pPrevInRect->ulWidth && pInRect->ulHeight==pPrevInRect->ulHeight)
		&& (pOutRect->ulX==pPrevOutRect->ulX && pOutRect->ulY==pPrevOutRect->ulY
		&& pOutRect->ulWidth==pPrevOutRect->ulWidth && pOutRect->ulHeight==pPrevOutRect->ulHeight))
	{
		/*
		 * Be careful when the previous and the current settings are (0,0,0,0)
		 * It seems like that the current one can be ignored, because it is same with the previous one.
		 * It makes a problem, however, if HD resolution is changed.
		 * The setting is translated into digital width/height, and they turn out to be different values when the resolution is changed.
		 * Therefore, do not ignore the current setting even if it is same. So you can get the right digital width/height.
		 *
		 * 2009.6.21 dcchung
		 */
		ulSum=(pInRect->ulX+pInRect->ulY+pInRect->ulWidth+pInRect->ulHeight);
		ulSum+=(pOutRect->ulX+pOutRect->ulY+pOutRect->ulWidth+pOutRect->ulHeight);
		if (ulSum!=0)
		{
			DI_UART_Print("[%s] Same with the previous setting. Ignored.\n", __func__);
			return FALSE;
		}
	}

	return TRUE;
}

#if defined(CONFIG_WORKAROUND_VIDEO_WINDOW)
static DRV_Error P_VIDEO_ShowwithWindowID(HUINT32 ulDeviceId, HUINT32 ulDisplayId, HBOOL bShow)
{
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_Error	neResult;
	DRV_Error nDrvErr = DRV_OK;
	HUINT32 ulWindowId = ulDeviceId;

	window = s_stVideo[ulDeviceId].hWindow[ulDisplayId][ulWindowId];
	if(window == NULL)
	{
		nDrvErr = DRV_ERR;
		return nDrvErr;
	}

	NEXUS_VideoWindow_GetSettings(window, &windowSettings);
	windowSettings.visible = bShow;
#if defined(CONFIG_WORKAROUND_WIN_CLIP)
	windowSettings.vdcapply = true;
#endif

	neResult = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
	if(neResult != NEXUS_SUCCESS)
	{
		nDrvErr = DRV_ERR;
	}

	return nDrvErr;
}

static DRV_Error P_VIDEO_Show(HUINT32 ulDeviceId, HBOOL bShow)
{
	DRV_Error nDrvErr = DRV_OK;
	int i;

	if(s_stVideo[ulDeviceId].bMosaicState == TRUE)
	{
		return nDrvErr;
	}

	for(i=0 ; i<DI_NUM_DISPLAYS ; i++)
	{
		if( (s_stVideo[ulDeviceId].bForcedMute[i] == TRUE) && (bShow == TRUE) )
		{
			PrintDebug("%d window is forced mute!! SKIP~~~ \n");
			continue;
		}

		if(s_stVideo[ulDeviceId].hDisplay[i] != NULL)
			nDrvErr = P_VIDEO_ShowwithWindowID(ulDeviceId, i, bShow);
	}

	return nDrvErr;
}
#endif

#if defined(CONFIG_SDHD_FILTER_6346)
static void P_VIDEO_SetGpioFilter(HUINT32 ulDeviceId)
{
	NEXUS_DisplaySettings displaySettings;

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
		if((s_currentOutputStatus.bYPbPr == 1) && displaySettings.format >= NEXUS_VideoFormat_e480p)
			DRV_GPIO_Write(GPIO_ID_SD_HD_SEL,GPIO_HI);
		else
			DRV_GPIO_Write(GPIO_ID_SD_HD_SEL,GPIO_LOW);
	}
}
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
#if defined(CONFIG_YPBPR_PORT)
static void P_VIDEO_SetDecConnectHD(HUINT32 ulDeviceId,HBOOL bOnOff)
{
	NEXUS_Error nexusError;
	NEXUS_ComponentOutputHandle hComponentOutput;
	NEXUS_VideoOutput hVideoOutput;

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		hComponentOutput = P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT);
		hVideoOutput = NEXUS_ComponentOutput_GetConnector(hComponentOutput);
		if(bOnOff == TRUE)
		{
			nexusError=NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[HD_DISP],hVideoOutput);
			if(nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] YPBPR connect failed\n",__func__);
			}
		}
		else
		{
			nexusError=NEXUS_Display_RemoveOutput(s_stVideo[ulDeviceId].hDisplay[HD_DISP],hVideoOutput);
			if(nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] YPBPR disconnect failed\n",__func__);
			}
		}
	}

}
#endif
#endif

#if defined(CONFIG_DI20)
static DRV_Error P_VIDEO_Set3DFormat(HUINT32 ulDeviceId, DI_VIDEO_3D_FORMAT e3dFormat)
{
#if defined(CONFIG_VIDEO_DECODER)
	DRV_Error nDrvErr = DRV_OK;
#if !defined(CONFIG_DIRECTFB)
	NEXUS_VideoDecoderSettings decoderSettings;
	NEXUS_VideoDecoderSourceOrientation sourceOrientation;
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_Error nexusError;
	DI_VIDEO_RECT_t stInRect, stOutRect;
	HUINT32 ulWindowId = ulDeviceId;
	NEXUS_VideoOrientation  orientation;

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		/* decoder setting */
		switch(e3dFormat)
		{
			case DI_VIDEO_3D_FORMAT_SIDEBYSIDE:
			sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight;
			orientation = NEXUS_VideoOrientation_e3D_LeftRight;
				break;
			case DI_VIDEO_3D_FORMAT_TOPANDBOTTOM:
			sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder;
			orientation = NEXUS_VideoOrientation_e3D_OverUnder;
				break;
			case DI_VIDEO_3D_FORMAT_NONE:
			default:
			sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
			orientation = NEXUS_VideoOrientation_e2D;
				break;
		}

		NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		if(decoderSettings.customSourceOrientation != true || decoderSettings.sourceOrientation != sourceOrientation)
		{
			decoderSettings.customSourceOrientation = true;
			decoderSettings.sourceOrientation = sourceOrientation;
			nexusError = NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
			if(nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_VideoDecoder_SetSettings error \n",__func__);
				goto error;
				nDrvErr = DRV_ERR;
			}
		}
		/*
		 * display setting
		 * dfb must control the diplay. this code for abnormal video stop.
		 */
		if(e3dFormat == DI_VIDEO_3D_FORMAT_NONE)
		{
			NEXUS_DisplaySettings displaySettings;

			NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			displaySettings.display3DSettings.overrideOrientation = true;
			displaySettings.display3DSettings.orientation = orientation;
			nexusError = NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_SetSettings error!\n", __func__);
				goto error;
				nDrvErr = DRV_ERR;
			}
		}
		/* check window rect */
		VK_MEM_Memset(&stInRect,0xFF,sizeof(DI_VIDEO_RECT_t));
		VK_MEM_Memset(&stOutRect,0xFF,sizeof(DI_VIDEO_RECT_t));

		if(!VK_memcmp(&stInRect, &s_stVideoInfo[ulDeviceId].stDesiredHdInRect, sizeof(DI_VIDEO_RECT_t))
			|| !VK_memcmp(&stOutRect, &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect, sizeof(DI_VIDEO_RECT_t)))
		{
			DI_VIDEO_StreamInfo_t stStreamInfo;
			NEXUS_DisplaySettings displaySettings;

			P_VIDEO_CopySeqHeaderInfo(ulDeviceId, &stStreamInfo);
			s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulX = 0;
			s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulY = 0;
			s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulWidth = stStreamInfo.ulWidth;
			s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulHeight = stStreamInfo.ulHeight;

			NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulX = 0;
			s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulY = 0;
			P_VIDEO_GetDisplaySize(displaySettings.format, &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulWidth, &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulHeight);
		}
		else
		{
			nDrvErr = DRV_OK;
			goto done;
		}

		if((s_stVideo[ulDeviceId].e3dDisplayFormat==DI_VIDEO_3D_FORMAT_NONE)
			&& (e3dFormat==DI_VIDEO_3D_FORMAT_SIDEBYSIDE)) /* 2d => sbs */
		{
			stInRect = s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
			stOutRect.ulX = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulX/2;
			stOutRect.ulY = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulY;
			stOutRect.ulWidth = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulWidth/2;
			stOutRect.ulHeight = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulHeight;
		}
		else if((s_stVideo[ulDeviceId].e3dDisplayFormat==DI_VIDEO_3D_FORMAT_NONE)
			&& (e3dFormat==DI_VIDEO_3D_FORMAT_TOPANDBOTTOM)) /* 2d -> tab */
		{
			stInRect = s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
			stOutRect.ulX = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulX;
			stOutRect.ulY = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulY/2;
			stOutRect.ulWidth = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulWidth;
			stOutRect.ulHeight = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulHeight/2;
		}
		else if((s_stVideo[ulDeviceId].e3dDisplayFormat==DI_VIDEO_3D_FORMAT_SIDEBYSIDE)
			&& (e3dFormat==DI_VIDEO_3D_FORMAT_NONE)) /* sbs -> 2d */
		{
			stInRect = s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
			stOutRect = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect;
		}
		else if((s_stVideo[ulDeviceId].e3dDisplayFormat==DI_VIDEO_3D_FORMAT_TOPANDBOTTOM)
			&& (e3dFormat==DI_VIDEO_3D_FORMAT_NONE)) /* tab -> 2d */
		{
			stInRect = s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
			stOutRect = s_stVideoInfo[ulDeviceId].stDesiredHdOutRect;
		}
		else
		{
			PrintError("[%s] : Unsupported scaling display format(%d->%d)!!!\n", __func__,
				s_stVideo[ulDeviceId].e3dDisplayFormat, e3dFormat);
			nDrvErr = DRV_ERR;
			goto error;
		}
		PrintDebug("in rect(%d, %d, %d, %d)\n", stInRect.ulX, stInRect.ulY, stInRect.ulWidth, stInRect.ulHeight);

		/* window setting */
		window = s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId];
		NEXUS_VideoWindow_GetSettings(window, &windowSettings);
		windowSettings.clipBase.x =  (HUINT16)stInRect.ulX;
		windowSettings.clipBase.y =  (HUINT16)stInRect.ulY;
		windowSettings.clipBase.width = (HUINT16)stInRect.ulWidth;
		windowSettings.clipBase.height = (HUINT16)stInRect.ulHeight;
		windowSettings.position.x= (HUINT16)stOutRect.ulX;
		windowSettings.position.y = (HUINT16)stOutRect.ulY;
		windowSettings.position.width = (HUINT16)stOutRect.ulWidth;
		windowSettings.position.height = (HUINT16)stOutRect.ulHeight;
		nexusError = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
		if (nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] : NEXUS_VideoWindow_SetSettings Error!!\n",__func__);
			nDrvErr = DRV_ERR;
			goto error;
		}

done:
		s_stVideo[ulDeviceId].e3dDisplayFormat = e3dFormat;
	}
error:
#else
#if (NEXUS_VERSION >= 1404)
	{
		NEXUS_VideoDecoderSettings decoderSettings;
		NEXUS_VideoDecoderSourceOrientation sourceOrientation;
		NEXUS_VideoOrientation	orientation;

		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			/* decoder setting */
			switch(e3dFormat)
			{
				case DI_VIDEO_3D_FORMAT_SIDEBYSIDE:
					sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight;
					orientation = NEXUS_VideoOrientation_e3D_LeftRight;
					break;
				case DI_VIDEO_3D_FORMAT_TOPANDBOTTOM:
					sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder;
					orientation = NEXUS_VideoOrientation_e3D_OverUnder;
					break;
				case DI_VIDEO_3D_FORMAT_NONE:
				default:
					sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
					orientation = NEXUS_VideoOrientation_e2D;
					break;
			}

			NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
			decoderSettings.customSourceOrientation = true;
			decoderSettings.sourceOrientation = sourceOrientation;
			NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		}
	}
#endif
	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		s_stVideo[ulDeviceId].e3dDisplayFormat = e3dFormat;
#endif

	return nDrvErr;
#else
	return DRV_OK;
#endif
}
#endif
void P_VIDEO_Still_picture_ready(void *context, int unused)
{
	BSTD_UNUSED(unused);
	PrintDebug("still_picture_ready\n");
	BKNI_SetEvent((BKNI_EventHandle)context);
}

#define PADDING_BYTE		(256+4)		/* Must be 256 ? */
static NEXUS_Error P_VIDEO_GetPicture(HUINT32 ulDeviceId, NEXUS_VideoCodec codec, HUINT8 *pucMemBuff, HULONG ulMemBuffSize)
{
	NEXUS_Error rc;
	unsigned feed_zeros = PADDING_BYTE;
	bool feed_file = true;
	unsigned long ulCurPos=0;

	/* feeding 256 bytes of zeros is required to flush the last packet of data through transport */
	while (feed_file || feed_zeros)
	{
		size_t size;
		void *buffer;

		rc = NEXUS_Playpump_GetBuffer(s_stVideo[ulDeviceId].PlayMemoryPlaypump, &buffer, &size);
		if (rc)
		{
			PrintError("[%s] NEXUS_Playpump_GetBuffer failed\n",__func__);
			goto done;
		}
		if (!size)
		{
			BKNI_Sleep(10);
			continue;
		}

		if (feed_file)
		{
			if ((ulMemBuffSize-ulCurPos+1)>=size)
			{
				VK_memcpy(buffer, pucMemBuff+ulCurPos, size);
				ulCurPos+=size;
			}
			else
			{
				size=(ulMemBuffSize-ulCurPos+1);
				VK_memcpy(buffer, pucMemBuff+ulCurPos, size);
				ulCurPos+=size;
			}

			if (size <= 0) {
				/* done reading file. now feed some zeros to flush transport. */
				feed_file = false;
				continue;
			}
		}
		else if (!feed_file && feed_zeros)
		{
			if (size > feed_zeros) size = feed_zeros;
			BKNI_Memset(buffer, 0, size);

			if( feed_zeros == PADDING_BYTE) {
				unsigned char *buffer_ptr = (unsigned char *)buffer;
				if( codec == NEXUS_VideoCodec_eH264 ){
					buffer_ptr[0] = 0x00;
					buffer_ptr[1] = 0x00;
					buffer_ptr[2] = 0x01;
					buffer_ptr[3] = 0x0A;
				} else if( codec == NEXUS_VideoCodec_eMpeg2 ) {
					buffer_ptr[0] = 0x00;
					buffer_ptr[1] = 0x00;
					buffer_ptr[2] = 0x01;
					buffer_ptr[3] = 0xB7;
				} else if( codec == NEXUS_VideoCodec_eVc1 ) {
					buffer_ptr[0] = 0x00;
					buffer_ptr[1] = 0x00;
					buffer_ptr[2] = 0x01;
					buffer_ptr[3] = 0xBA;
				} else if( codec == NEXUS_VideoCodec_eMpeg2 ) {
					buffer_ptr[0] = 0x00;
					buffer_ptr[1] = 0x00;
					buffer_ptr[2] = 0x01;
					buffer_ptr[3] = 0xB1;
				}

				PrintDebug("[%s] add codec(%d) EOS\n",__func__, codec);
			}

			feed_zeros -= size;
		}

		rc = NEXUS_Playpump_ReadComplete(s_stVideo[ulDeviceId].PlayMemoryPlaypump, 0, size);
		if (rc)
		{
			PrintError("[%s] NEXUS_Playpump_ReadComplete failed\n",__func__);
			goto done;
		}
	}
done:
	return rc;

}

#if defined(CONFIG_DI20)
static DRV_Error P_VIDEO_FlushDecoder(HUINT32 ulDeviceId)
{
	NEXUS_VideoDecoder_Flush(s_stVideo[ulDeviceId].hDecoder);
	return DRV_OK;
}
#endif

#ifdef SD_DISPLAY_TIMEBASE_FREERUN
static void P_VIDEO_SetSDTimebaseFreerun(void)
{
	HUINT32 ulDeviceId = 0;

	NEXUS_Timebase tb = T2_SD_TIMEBASE;
	NEXUS_TimebaseSettings tbSettings;

	NEXUS_DisplaySettings displaySettings;

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
	{
		NEXUS_Timebase_GetSettings(tb, &tbSettings);
		tbSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
		NEXUS_Timebase_SetSettings(tb, &tbSettings);

		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&displaySettings);
		displaySettings.timebase = tb;
		NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &displaySettings);
	}

}
#endif

#if defined(CONFIG_IFRAME_RANDOM_ACCESS)
static void P_VIDEO_Check_DecodingStatus(unsigned long ulTimerId, void *pvParam)
{
#if defined(CONFIG_VIDEO_DECODER)
	HINT32 vResult;
	NEXUS_Error nResult;
	HUINT32 ulDeviceId;
	HUINT32 uiAvdStatus = 0;
	NEXUS_VideoDecoderStatus decoderStatus;
	NEXUS_VideoDecoderExtendedSettings extSettings;

	s_rapTimerId = 0;
	s_bRunningrapTimer = FALSE;

	BSTD_UNUSED(ulTimerId);

	VK_MEM_Memset(&decoderStatus, 0x0, sizeof(decoderStatus));
	VK_MEM_Memset(&extSettings, 0x0, sizeof(extSettings));
	VK_MEM_Memcpy(&ulDeviceId, pvParam, sizeof(ulDeviceId));

	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);
	uiAvdStatus = ((0x00000000 | NEXUS_CHANNELSTATUS_AVD_RAP_NOT_DETECTED) & decoderStatus.avdStatusBlock);
	PrintDebug("[%s] avdStatusBlock 0x%x, uiAvdStatus 0x%x \n", __func__, decoderStatus.avdStatusBlock, uiAvdStatus);

	/* If RAP is not exist in video stream, I-Frame is treated as RAP */
	/* For testing, use "TVC mpeg4" channel in following stream : \\stream\stream\DriverTeamTestStream\treatIFrameAsRap\1.ts */
	if((decoderStatus.started == TRUE) && (uiAvdStatus == NEXUS_CHANNELSTATUS_AVD_RAP_NOT_DETECTED))
	{
		vResult = VK_SEM_Get(s_stVideo[ulDeviceId].ulSemId_DRV);
		if(vResult != VK_OK)
		{
			PrintError("[%s] VK_SEM_Get failed!\n", __func__);
			return;
		}

		NEXUS_VideoDecoder_Stop(s_stVideo[ulDeviceId].hDecoder);

		NEXUS_VideoDecoder_GetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);
		extSettings.treatIFrameAsRap = true;
		NEXUS_VideoDecoder_SetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);

		nResult = NEXUS_VideoDecoder_Start(s_stVideo[ulDeviceId].hDecoder , &s_stVideo[ulDeviceId].StartSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_VideoDecoder_Start failed!\n");
			VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_DRV);
			return;
		}

		s_stVideo[ulDeviceId].bTreatIFrameAsRap = TRUE;
		VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_DRV);
	}
#endif
}
#endif

#if defined(CONFIG_DEBUG) && defined(CONFIG_VIDEO_DECODER)
static void P_VIDEO_CheckErrorStatus(unsigned long ulTimerId, void *pvParam)
{
	NEXUS_VideoDecoderStatus currDecoderStatus;
	HUINT32 ulDeviceId;

	BSTD_UNUSED(ulTimerId);

	if(pvParam == NULL)
	{
		PrintError("Decoder ID parameter is NULL.\n");
		return;
	}

	ulDeviceId = *((HUINT32 *)pvParam);

	if(ulDeviceId >= NUM_VID_DECODER || s_stVideo[ulDeviceId].bInitialized==FALSE)
	{
		PrintError(" %s decoder(idx=%d) is not supported or initialized.\n", ((ulDeviceId == DI_VIDEO_DECODER_MAIN) ? "Master" : "PIP"),ulDeviceId); \
		return;
	}

	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&currDecoderStatus);

	if(currDecoderStatus.started)
	{
		if(currDecoderStatus.numDecodeErrors > s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeErrors)
		{
			PrintError("Decode error occured [%d] times in the Decoder %d \n",currDecoderStatus.numDecodeErrors-s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeErrors,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeErrors = currDecoderStatus.numDecodeErrors;
		}

		if(currDecoderStatus.numDecodeOverflows > s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeOverflows)
		{
			PrintError("Decode overflow occured [%d] times in the Decoder %d \n",currDecoderStatus.numDecodeOverflows-s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeOverflows,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeOverflows = currDecoderStatus.numDecodeOverflows;
		}

		if(currDecoderStatus.numDisplayErrors > s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayErrors)
		{
			PrintError("Display error occured [%d] times in the Decoder %d \n",currDecoderStatus.numDisplayErrors-s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayErrors,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayErrors = currDecoderStatus.numDisplayErrors;
		}

		if(currDecoderStatus.numDecodeDrops > s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeDrops)
		{
			PrintError("Decode drop occured [%d] times in the Decoder %d \n",currDecoderStatus.numDecodeDrops-s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeDrops,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulDecodeDrops = currDecoderStatus.numDecodeDrops;
		}

		if(currDecoderStatus.numDisplayDrops > s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayDrops)
		{
			PrintError("Display drop occured [%d] times in the Decoder %d\n",currDecoderStatus.numDisplayDrops-s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayDrops,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayDrops = currDecoderStatus.numDisplayDrops;
		}

		if(currDecoderStatus.numDisplayUnderflows > s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayUnderflows)
		{
			PrintError("Display underflow occured [%d] times in the Decoder %d\n",currDecoderStatus.numDisplayUnderflows-s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayUnderflows,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulDisplayUnderflows = currDecoderStatus.numDisplayUnderflows;
		}

		if(currDecoderStatus.ptsErrorCount > s_stPrevVideoErrorStatus[ulDeviceId].ulPtsErrors)
		{
			PrintError("Video PTS error occured [%d] times in the Decoder %d\n",currDecoderStatus.ptsErrorCount-s_stPrevVideoErrorStatus[ulDeviceId].ulPtsErrors,ulDeviceId);
			s_stPrevVideoErrorStatus[ulDeviceId].ulPtsErrors = currDecoderStatus.ptsErrorCount;
		}
	}

	return;
}
#endif


static void P_VIDEO_SetDecoderMode(HUINT32 ulDeviceId, DI_VIDEO_DECODER_MODE eMode)
{
#if defined(CONFIG_VIDEO_DECODER) && defined(CONFIG_DI20)
	NEXUS_VideoDecoderExtendedSettings extSettings;

	NEXUS_VideoDecoder_GetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);

	switch(eMode)
	{
		case DI_VIDEO_DECODER_MODE_GAME :
			extSettings.treatIFrameAsRap = true;
			extSettings.ignoreNumReorderFramesEqZero = true;
			extSettings.ignoreDpbOutputDelaySyntax = true;
			extSettings.zeroDelayOutputMode = true;
			extSettings.earlyPictureDeliveryMode = true;
#if (NEXUS_VERSION >= 1301)
			/* Todo : Please check settings value  */
#elif (NEXUS_VERSION == 1290)
			extSettings.lowLatencyTsmMode = true;
#else
#endif
			break;

		case DI_VIDEO_DECODER_MODE_IP_CAMERA :
			 extSettings.zeroDelayOutputMode = true;
#if (NEXUS_VERSION >= 1301)
			extSettings.lowLatencySettings.mode = NEXUS_VideoDecoderLowLatencyMode_eFast;
#elif (NEXUS_VERSION == 1290)
			extSettings.lowLatencyTsmMode = true;
#else
#endif
			break;

		case DI_VIDEO_DECODER_MODE_MAX : /* Invalid video mode. force set Normal mode */
		case DI_VIDEO_DECODER_MODE_RFVOD :
		case DI_VIDEO_DECODER_MODE_NORMAL:
		default :
			extSettings.treatIFrameAsRap = false;
			extSettings.ignoreNumReorderFramesEqZero = false;
			extSettings.ignoreDpbOutputDelaySyntax = false;
			extSettings.zeroDelayOutputMode = false;
			extSettings.earlyPictureDeliveryMode = false;
#if (NEXUS_VERSION >= 1301)
			extSettings.lowLatencySettings.mode = NEXUS_VideoDecoderLowLatencyMode_eOff;
#elif (NEXUS_VERSION == 1290)
			extSettings.lowLatencyTsmMode = false;
#else
#endif
			break;
	}

	NEXUS_VideoDecoder_SetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);

	PrintDebug("Change Decoder Mode [%d] -> [%d]\n",s_stVideo[ulDeviceId].eDecoderMode,eMode);
	s_stVideo[ulDeviceId].eDecoderMode = eMode;
#endif
}

DI_VIDEO_FORMAT DRV_VIDEO_GetDiFormat(HUINT32 ulDeviceId, NEXUS_VideoCodec VideoCodec)
{
	DI_VIDEO_FORMAT eFormat;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	eFormat=P_VIDEO_Convert2DICodec(VideoCodec);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return eFormat;

}

DI_VIDEO_FrameRate_t DRV_VIDEO_GetDiFrameRate(HUINT32 ulDeviceId, NEXUS_VideoFrameRate FrameRate)
{
	DI_VIDEO_FrameRate_t eFrameRate;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	eFrameRate = P_VIDEO_Convert2DIFrameRate(FrameRate);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return eFrameRate;

}

NEXUS_DisplayAspectRatio DRV_VIDEO_GetAspectRatio(HUINT32 ulDeviceId, DI_VIDEO_AspectRatio_t AspectRatio)
{
	NEXUS_DisplayAspectRatio  NexusAspectRatio;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	NexusAspectRatio = P_VIDEO_Convert2NexusAspectRatio(AspectRatio);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return NexusAspectRatio;

}

NEXUS_VideoCodec DRV_VIDEO_GetCodecType(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec)
{
	NEXUS_VideoCodec NexusVideoCodec;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	NexusVideoCodec = P_VIDEO_Convert2NexusCodec(VideoCodec);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return NexusVideoCodec;

}

NEXUS_VideoFrameRate DRV_VIDEO_GetFrameRateType(HUINT32 ulDeviceId, DI_VIDEO_FrameRate_t eFrameRate)
{
	NEXUS_VideoFrameRate NexusVideoFrameRate;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	NexusVideoFrameRate = P_VIDEO_Convert2NexusFrameRate(eFrameRate);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return NexusVideoFrameRate;

}

NEXUS_VideoFormat DRV_VIDEO_GetFormatType(HUINT32 ulDeviceId, DI_VIDEO_HD_RESOLUTION eFormat)
{
	NEXUS_VideoFormat NexusVideoFormat;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	NexusVideoFormat = P_VIDEO_Convert2NexusVideoFormat(eFormat);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return NexusVideoFormat;

}

NEXUS_DisplayAspectRatio DRV_VIDEO_GetAspectRatioType(HUINT32 ulDeviceId, DI_VIDEO_HD_RESOLUTION eFormat)
{
	NEXUS_VideoFormat NexusVideoFormat;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	NexusVideoFormat = P_VIDEO_Convert2NexusVideoFormat(eFormat);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return NexusVideoFormat;

}

DRV_Error DRV_VIDEO_SetPresetProperties(HUINT32 ulDeviceId, DRV_VIDEO_PRESET ePreset, DRV_VIDEO_PRESET_DATA_t *pData)
{
	DRV_Error eErr = DRV_OK;
	HUINT32 ulWindowId = ulDeviceId;
	NEXUS_VideoWindowScalerSettings windowScalerSettings;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(ePreset == DRV_VIDEO_PRESET_SSTR)
	{
		if(pData->bOnOff == true)
		{
			if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
			{
				NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
				windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
				windowScalerSettings.bandwidthEquationParams.delta=1*1000*1000;
				NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
			}

			if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
			{
				NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
				windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
				windowScalerSettings.bandwidthEquationParams.delta=1*1000*1000;
				NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
			}
		}
		else
		{
			if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
			{
				NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
				windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eAuto;
				windowScalerSettings.bandwidthEquationParams.delta=0;
				NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
			}

			if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
			{
				NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
				windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eAuto;
				windowScalerSettings.bandwidthEquationParams.delta=0;
				NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
			}
		}
		s_stVideo[ulDeviceId].bForcedMadOff = pData->bOnOff;
	}
	else
	{
		eErr = DRV_ERR_INVALID_PARAMETER;
	}
	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return eErr;
}

DRV_Error DRV_VIDEO_SetSmoothStreaming(HUINT32 ulDeviceId, HBOOL OnOff)
{
	NEXUS_VideoWindowScalerSettings windowScalerSettings;
	HUINT32 ulWindowId = ulDeviceId;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(OnOff == true)
	{
		if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		{
			NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
			windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
			windowScalerSettings.bandwidthEquationParams.delta=1*1000*1000;
			NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
		}

		if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		{
			NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
			windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
			windowScalerSettings.bandwidthEquationParams.delta=1*1000*1000;
			NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
		}
	}
	else
	{
		if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		{
			NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
			windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eAuto;
			windowScalerSettings.bandwidthEquationParams.delta=0;
			NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
		}

		if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		{
			NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
			windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eAuto;
			windowScalerSettings.bandwidthEquationParams.delta=0;
			NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId],&windowScalerSettings);
		}
	}

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DRV_Error DRV_VIDEO_ResetDecoder(HUINT32 ulDeviceId)
{
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	PrintError("[%s] Decoder Reset \n",__func__);
	NEXUS_VideoDecoder_Reset(s_stVideo[ulDeviceId].hDecoder);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;

}

DRV_Error DRV_VIDEO_SetDeinterlaced(HUINT32 ulDeviceId, HBOOL OnOff, HBOOL bForce)
{
	HUINT32 ulWindowId = ulDeviceId;
	NEXUS_VideoWindowMadSettings madSettings;
	NEXUS_VideoDecoderStatus decoderStatus;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoFormatInfo videoFormatInfo;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);
		NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);

		if(bForce == TRUE)
		{
			madSettings.deinterlace = OnOff;
		}
		else
		{
			if(decoderStatus.interlaced == true &&
				decoderStatus.format != NEXUS_VideoFormat_e1080i &&
				decoderStatus.format != NEXUS_VideoFormat_e1080i50hz)
			{
				NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP],&displaySettings);
				NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
				if(videoFormatInfo.interlaced == false)
				{
					madSettings.deinterlace=true;
				}
				else
				{
					madSettings.deinterlace=false;
				}

			}
			else
			{
				madSettings.deinterlace=false;
			}

		}
		NEXUS_VideoWindow_SetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
	}

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;

}

DRV_Error DRV_VIDEO_GetCurrentStc(HUINT32 ulDeviceId, HUINT32 *uiStc)
{
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].StartSettings.stcChannel != NULL)
		NEXUS_StcChannel_GetStc(s_stVideo[ulDeviceId].StartSettings.stcChannel,uiStc);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DRV_Error DRV_VIDEO_GetVideoHandle(HUINT32 ulDeviceId, NEXUS_VideoDecoderHandle *hDecoder)
{
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	*hDecoder = s_stVideo[ulDeviceId].hDecoder;

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DRV_Error DRV_VIDEO_GetVideoInputHandle(HUINT32 ulDeviceId, NEXUS_VideoInput* hInput)
{
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	*hInput= s_stVideo[ulDeviceId].hInput;

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DRV_Error DRV_VIDEO_SetDispParam(short contrast, short saturation, short hue, short brightness)
{
	NEXUS_PictureCtrlCommonSettings Settings;
	int i,j,k;

	for(i=0; i<NUM_VID_DECODER; i++)
	{
		for(j=0; j< DI_NUM_DISPLAYS; j++)

	{
			for(k=0; k<DI_NUM_VIDEO_WINDOWS; k++)
			{
				if(s_stVideo[i].hWindow[j][k] != NULL)
				{
					NEXUS_PictureCtrl_GetCommonSettings(s_stVideo[i].hWindow[j][k], &Settings);
					Settings.contrast = contrast;
					Settings.saturation = saturation;
					Settings.hue = hue;
					Settings.brightness = brightness;
					NEXUS_PictureCtrl_SetCommonSettings(s_stVideo[i].hWindow[j][k], &Settings);
				}
			}
		}
	}

	return DRV_OK;

}

DRV_Error DRV_VIDEO_SetDisplayWindow(HUINT32 ulDeviceId, HUINT32 ulDisplayId, DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pHdWinOutRect, DI_VIDEO_HD_RESOLUTION *eHdResolution, DI_VIDEO_WIN_TYPE eVidWinType)
{
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	DI_VIDEO_StreamInfo_t stream_info;
	NEXUS_Error nexusError;
	DRV_Error nDrvErr = DRV_OK;
	HUINT16 usHdDigitalWidth;
	HUINT16 usHdDigitalHeight;
	DI_VIDEO_WIN_TYPE eDrvVidWinType=eVidWinType;
	HUINT32 ulWindowId = ulDeviceId;

	if ((s_stVideo[ulDeviceId].bInitialized == FALSE) || (VK_SEM_Get(s_stVideo[ulDeviceId].ulSemId_DRV) != VK_OK))
		return DRV_ERR_INVALID_PARAMETER;

	window = s_stVideo[ulDeviceId].hWindow[ulDisplayId][ulWindowId];
	if (window == NULL)
	{
		PrintError("[%s] window = %d is NULL.\n",__func__,ulDisplayId);
		nDrvErr = DRV_ERR;
		goto done;
	}

	display = s_stVideo[ulDeviceId].hDisplay[ulDisplayId];
	if (display == NULL)
	{
		PrintError("[%s] Display Handle is NULL\n",__func__);
		nDrvErr = DRV_ERR;
		goto done;
	}

	if (ulDisplayId==HD_DISP)
	{
		NEXUS_Display_GetSettings(display, &displaySettings);
		if (eHdResolution != NULL)
		{
			displaySettings.format = P_VIDEO_Convert2NexusVideoFormat(*eHdResolution);
			nexusError = NEXUS_Display_SetSettings(display, &displaySettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_SetSettings error!\n", __func__);
				nDrvErr = DRV_ERR;
				goto done;
			}
			else
			{
				goto done;
			}
		}
		P_VIDEO_GetDisplaySize(displaySettings.format, &usHdDigitalWidth, &usHdDigitalHeight);

		P_VIDEO_CopySeqHeaderInfo(ulDeviceId, &stream_info);
		NEXUS_VideoWindow_GetSettings(window, &windowSettings);

		if (pInRect != NULL)
		{
			windowSettings.clipBase.x = 0;
			windowSettings.clipBase.y = 0;
			windowSettings.clipBase.width = (HUINT16)stream_info.ulWidth;
			windowSettings.clipBase.height = (HUINT16)stream_info.ulHeight;

			if ((pInRect->ulX == 0) && (pInRect->ulY == 0) && (pInRect->ulWidth == 0) && (pInRect->ulHeight == 0))
			{
				windowSettings.clipRect.x = 0;
				windowSettings.clipRect.y = 0;
				windowSettings.clipRect.width = (HUINT16)stream_info.ulWidth;
				windowSettings.clipRect.height = (HUINT16)stream_info.ulHeight;
			}
			else
			{
#if defined(CONFIG_INRECT_RATIO)
				windowSettings.clipRect.x = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulX, (HUINT16)stream_info.ulWidth);
				windowSettings.clipRect.y = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulY, (HUINT16)stream_info.ulHeight);
				windowSettings.clipRect.width = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulWidth, (HUINT16)stream_info.ulWidth);
				windowSettings.clipRect.height = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulHeight, (HUINT16)stream_info.ulHeight);
#else
				windowSettings.clipRect.x = (HUINT16)pInRect->ulX;
				windowSettings.clipRect.y = (HUINT16)pInRect->ulY;
				windowSettings.clipRect.width = (HUINT16)pInRect->ulWidth;
				windowSettings.clipRect.height = (HUINT16)pInRect->ulHeight;
#endif
			}
		}

		if (pHdWinOutRect != NULL)
		{
			if (((pHdWinOutRect->ulX + pHdWinOutRect->ulWidth) > usHdDigitalWidth) || ((pHdWinOutRect->ulY + pHdWinOutRect->ulHeight) > usHdDigitalHeight))
			{
				/* Out of range */
				PrintError("[%s] %dth window, OutWindow is out of range, (%d,%d)-(%d,%d)\n",
						__func__, ulDisplayId, pHdWinOutRect->ulX, pHdWinOutRect->ulY, pHdWinOutRect->ulWidth, pHdWinOutRect->ulHeight);
				PrintError("[%s] Digital Width,Height : (%d,%d)\n", __func__, usHdDigitalWidth, usHdDigitalHeight);
				nDrvErr = DRV_ERR;
				goto done;
			}

			if ((pHdWinOutRect->ulX == 0) && (pHdWinOutRect->ulY == 0) && (pHdWinOutRect->ulWidth == 0) && (pHdWinOutRect->ulHeight == 0))
			{
				/* Full window */
				windowSettings.position.x= 0;
				windowSettings.position.y = 0;
				windowSettings.position.width = usHdDigitalWidth;
				windowSettings.position.height = usHdDigitalHeight;
			}
			else
			{
				windowSettings.position.x = pHdWinOutRect->ulX;
				windowSettings.position.y = pHdWinOutRect->ulY;
				windowSettings.position.width = pHdWinOutRect->ulWidth;
				windowSettings.position.height = pHdWinOutRect->ulHeight;
			}
		}
#if defined(CONFIG_WORKAROUND_WIN_CLIP)
		if(eDrvVidWinType == DI_VIDEO_WIN_HD)
			windowSettings.vdcapply=true;
		else
			windowSettings.vdcapply=false;
#else
		eDrvVidWinType=0;
#endif
		nexusError = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
		if (nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] %dth window can't set In-Out window\n",__func__,ulDisplayId);
			nDrvErr = DRV_ERR;
			goto done;
		}
	}
	else // SD_DISP
	{
		/*
		 * It is supposed that SetInOut Setting for HD display is already done.
		 * 2009.5.5 dcchung
		 */
		NEXUS_Display_GetSettings(display, &displaySettings);
		P_VIDEO_GetDisplaySize(displaySettings.format, &usHdDigitalWidth, &usHdDigitalHeight);
		P_VIDEO_CopySeqHeaderInfo(ulDeviceId, &stream_info);
		NEXUS_VideoWindow_GetSettings(window, &windowSettings);

		if (pInRect != NULL)
		{
			windowSettings.clipBase.x = 0;
			windowSettings.clipBase.y = 0;
			windowSettings.clipBase.width = (HUINT16)stream_info.ulWidth;
			windowSettings.clipBase.height = (HUINT16)stream_info.ulHeight;

			if ((pInRect->ulX == 0) && (pInRect->ulY == 0) && (pInRect->ulWidth == 0) && (pInRect->ulHeight == 0))
			{
				windowSettings.clipRect.x = 0;
				windowSettings.clipRect.y = 0;
				windowSettings.clipRect.width = (HUINT16)stream_info.ulWidth;
				windowSettings.clipRect.height = (HUINT16)stream_info.ulHeight;
			}
			else
			{
#if defined(CONFIG_INRECT_RATIO)
				windowSettings.clipRect.x = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulX, (HUINT16)stream_info.ulWidth);
				windowSettings.clipRect.y = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulY, (HUINT16)stream_info.ulHeight);
				windowSettings.clipRect.width = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulWidth, (HUINT16)stream_info.ulWidth);
				windowSettings.clipRect.height = P_VIDEO_CalcPixelPosition(0, (HUINT16)pInRect->ulHeight, (HUINT16)stream_info.ulHeight);
#else
				windowSettings.clipRect.x = (HUINT16)pInRect->ulX;
				windowSettings.clipRect.y = (HUINT16)pInRect->ulY;
				windowSettings.clipRect.width = (HUINT16)pInRect->ulWidth;
				windowSettings.clipRect.height = (HUINT16)pInRect->ulHeight;
#endif
			}
		}

		if (pHdWinOutRect != NULL)
		{
			DI_VIDEO_RECT_t stSdWinOutRect;
#if defined(CONFIG_HDOUTRECT_TOSD)
			NEXUS_DisplayHandle HdDisplay=s_stVideo[ulDeviceId].hDisplay[HD_DISP];

			if (HdDisplay == NULL)
			{
				PrintError("[%s] Display Handle is NULL\n",__func__);
				nDrvErr = DRV_ERR;
				goto done;
			}

			NEXUS_Display_GetSettings(HdDisplay, &displaySettings);

	#ifdef CONFIG_HD_RESOLUTION_CHECK
			if(HD_Format_Desired != displaySettings.format)
			{
				//mismatch means MW tried to change HD resoultion but not applied.
				//(because HDMI HPD is not connected.)
				PrintError("Mismatch between desired and last set Resolutions \n");
				displaySettings.format = HD_Format_Desired;
			}
	#endif

			P_VIDEO_GetDisplaySize(displaySettings.format, &usHdDigitalWidth, &usHdDigitalHeight);

			stSdWinOutRect.ulX=(pHdWinOutRect->ulX*SD_DISP_WIDTH)/usHdDigitalWidth;
			stSdWinOutRect.ulY=(pHdWinOutRect->ulY*SD_DISP_HEIGHT)/usHdDigitalHeight;
			stSdWinOutRect.ulWidth= (pHdWinOutRect->ulWidth*SD_DISP_WIDTH)/usHdDigitalWidth;
			stSdWinOutRect.ulHeight= (pHdWinOutRect->ulHeight*SD_DISP_HEIGHT)/usHdDigitalHeight;
#else
			stSdWinOutRect.ulX=pHdWinOutRect->ulX;
			stSdWinOutRect.ulY=pHdWinOutRect->ulY;
			stSdWinOutRect.ulWidth= pHdWinOutRect->ulWidth;
			stSdWinOutRect.ulHeight= pHdWinOutRect->ulHeight;
#endif

			if (((stSdWinOutRect.ulX + stSdWinOutRect.ulWidth) > SD_DISP_WIDTH) || ((stSdWinOutRect.ulY + stSdWinOutRect.ulHeight) > SD_DISP_HEIGHT))
			{
				/* Out of range */
				PrintError("[%s] %dth window, output window is out of range, (%d, %d)-(%d, %d)\n",
						__func__, ulDisplayId, stSdWinOutRect.ulX, stSdWinOutRect.ulY, stSdWinOutRect.ulWidth, stSdWinOutRect.ulHeight);
				nDrvErr = DRV_ERR;
				goto done;
			}

			if ((stSdWinOutRect.ulX == 0) && (stSdWinOutRect.ulY == 0) && (stSdWinOutRect.ulWidth == 0) && (stSdWinOutRect.ulHeight == 0))
			{
				/* Full window */
				windowSettings.position.x= 0;
				windowSettings.position.y = 0;
				windowSettings.position.width = SD_DISP_WIDTH;
				windowSettings.position.height = SD_DISP_HEIGHT;
			}
			else
			{
				windowSettings.position.x = stSdWinOutRect.ulX;
				windowSettings.position.y = stSdWinOutRect.ulY;
				windowSettings.position.width = stSdWinOutRect.ulWidth;
				windowSettings.position.height = stSdWinOutRect.ulHeight;
			}
		}
#if defined(CONFIG_WORKAROUND_WIN_CLIP)
		windowSettings.vdcapply=true;
#endif
		nexusError = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
		if (nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] %dth window can't set In-Out window %d\n",__func__,ulDisplayId);
			nDrvErr = DRV_ERR;
			goto done;
		}
	}

done:
	VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_DRV);
	return nDrvErr;
}

DRV_Error DRV_VIDEO_DelayOutput(HUINT32 ulDeviceId, HINT32 delayMs)
{
	DRV_Error dResult;
	NEXUS_Error nResult;

	PrintError("This Function is only for Test/Development!\n");
	PrintError("HUINT32 ulDeviceId: %d HINT32 delayMs: %d\n", ulDeviceId, delayMs);
/*************************
*	1. Set up the delay
*************************/
#if 1
	dResult = P_VIDEO_DelayDecoder(ulDeviceId, delayMs);
	if(dResult != DRV_OK)
	{
		PrintError("P_VIDEO_DelayDecoder failed!\n");
		return DRV_ERR;
	}
#endif
/*************************
*	2. Restart the decoder
*************************/

#if 1
	NEXUS_VideoDecoder_Stop(s_stVideo[ulDeviceId].hDecoder);

	nResult = NEXUS_VideoDecoder_Start(s_stVideo[ulDeviceId].hDecoder , &s_stVideo[ulDeviceId].StartSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_Start failed!\n");
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

DRV_Error DRV_VIDEO_ApplySetInOutWindows(HUINT32 ulDeviceId, HBOOL bApply)
{
	DRV_Error	nDrvErr=DRV_OK;
	NEXUS_Error nexusError;

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(bApply)
	{
		nexusError = NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_DisplayModule_SetUpdateMode failed!\n");
			nDrvErr=DRV_ERR;
			goto done;
		}
	}
	else
	{
		nexusError = NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_DisplayModule_SetUpdateMode failed!\n");
			nDrvErr=DRV_ERR;
			goto done;
		}
	}

done:
	PrintExit();
	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return nDrvErr;
}

DRV_Error DRV_VIDEO_PlayMemoryStreamPlaypumpStart(HUINT32 ulDeviceId, DI_VIDEO_STREAM_TYPE eStreamType)
{
	NEXUS_PlaypumpSettings playpumpSettings;
	NEXUS_Error rc;
	DRV_Error nDrvErr=DRV_OK;

	PrintEnter();

#if defined(CONFIG_PVR)
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	DRV_PVR_GetPlaypumpHandle(&s_stVideo[ulDeviceId].PlayMemoryPlaypump);
	if (s_stVideo[ulDeviceId].PlayMemoryPlaypump == NULL)
	{
		PrintError("[%s] playpump is null\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	NEXUS_Playpump_GetDefaultSettings(&playpumpSettings);

	PrintDebug("[%s] DI_VIDEO_STREAM_TYPE (%d)\n",__func__, eStreamType);

	playpumpSettings.transportType = P_VIDEO_Convert2NexusTransport(eStreamType);
	if(playpumpSettings.transportType == NEXUS_TransportType_eUnknown || playpumpSettings.transportType >= NEXUS_TransportType_eMax)
	{
		PrintError("[%s] playpumpSettings.transportType(%d) is invalid\n",__func__, playpumpSettings.transportType);
		nDrvErr=DRV_ERR;
		goto done;
	}

	rc=NEXUS_Playpump_SetSettings(s_stVideo[ulDeviceId].PlayMemoryPlaypump, &playpumpSettings);
	if (rc)
	{
		PrintError("[%s] playpump setting failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	rc = NEXUS_Playpump_Start(s_stVideo[ulDeviceId].PlayMemoryPlaypump);
	if (rc)
	{
		PrintError("[%s] NEXUS_Playpump_Start failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	hStillPidChannel = NEXUS_Playpump_OpenPidChannel(s_stVideo[ulDeviceId].PlayMemoryPlaypump, 0x1, NULL);
	if(hStillPidChannel == NULL)
	{
		PrintError("[%s] hStillPidChannel == NULL\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}
done:
	DRV_VIDEO_REL_SEMA(ulDeviceId);
#endif

	PrintExit();
	return nDrvErr;
}

DRV_Error DRV_VIDEO_PlayMemoryStreamPlaypumpStop(HUINT32 ulDeviceId)
{
	DRV_Error	nDrvErr=DRV_OK;

	PrintEnter();

#if defined(CONFIG_PVR)
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if (s_stVideo[ulDeviceId].PlayMemoryPlaypump == NULL)
	{
		PrintError("[%s] playpump is null\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	NEXUS_Playpump_Stop(s_stVideo[ulDeviceId].PlayMemoryPlaypump);
	if(hStillPidChannel != NULL)
	{
		NEXUS_Playpump_ClosePidChannel(s_stVideo[ulDeviceId].PlayMemoryPlaypump, hStillPidChannel);
		hStillPidChannel = NULL;
	}
	DRV_PVR_ReleasePlaypumpHandle(s_stVideo[ulDeviceId].PlayMemoryPlaypump);
	s_stVideo[ulDeviceId].PlayMemoryPlaypump = NULL;
done:

	DRV_VIDEO_REL_SEMA(ulDeviceId);
#endif
	PrintExit();
	return nDrvErr;
}

DRV_Error DRV_VIDEO_PlayMemoryStreamOsdPlane(HUINT32 ulDeviceId, DI_VIDEO_FORMAT nDiVideoFmt, HUINT8 *pucMemBuff, HULONG ulMemBuffSize, void *stTarget)
{

	NEXUS_SurfaceHandle StillSurface=NULL;
	NEXUS_StillDecoderStartSettings stillDecoderSettings;
	NEXUS_VideoCodec codec;
	BKNI_EventHandle event=NULL;
	NEXUS_Error rc;
	DRV_Error	nDrvErr=DRV_OK;
	DI_OSD_INSTANCE* pOsdInstance;

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	BKNI_CreateEvent(&event);

	if(event == NULL)
	{
		PrintError("[%s] BKNI_CreateEvent event==NULL\n",__func__);
		nDrvErr=DRV_ERR;
		goto err;
	}
	else
		BKNI_ResetEvent(event);

	NEXUS_StillDecoder_GetDefaultStartSettings(&stillDecoderSettings);
	stillDecoderSettings.pidChannel = hStillPidChannel;

	codec = P_VIDEO_Convert2NexusCodec(nDiVideoFmt);
	PrintDebug("[%s] DI_VIDEO_FORMAT (0x%08x), codec (%d)\n",__func__, nDiVideoFmt, codec);

	stillDecoderSettings.stillPictureReady.callback = P_VIDEO_Still_picture_ready;
	stillDecoderSettings.stillPictureReady.context = event;
	stillDecoderSettings.codec = codec;
	PrintDebug("[%s] codec(%D)\n",__func__, stillDecoderSettings.codec);

	rc = NEXUS_StillDecoder_Start(s_stVideo[ulDeviceId].hStillDecoder, &stillDecoderSettings);
	if (rc)
	{
		PrintError("[%s] NEXUS_StillDecoder_Start failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto err;
	}

	rc = P_VIDEO_GetPicture(ulDeviceId, codec, pucMemBuff, ulMemBuffSize);
	if(rc)
	{
		PrintError("[%s] NEXUS_StillDecoder_Start failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	/* now we can decode the still */
	rc = BKNI_WaitForEvent(event, 3000);
	if (rc)
	{
		PrintError("[%s] Unable to decode still\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}
	/* graphic resource 사용하므로, semaphore 에 의해 보호해야 함. */
	if (DRV_OSD_AcquireSemaphore() == DRV_OK)
	{
		NEXUS_Graphics2DHandle blitter;
		DRV_OSD_GetGraphics2DHandle(&blitter);

		NEXUS_StripedSurfaceHandle stripedSurface;
		NEXUS_StripedSurfaceStatus stripedSurfaceStatus;
		rc = NEXUS_StillDecoder_GetStripedSurface(s_stVideo[ulDeviceId].hStillDecoder, &stripedSurface);
		if (rc)
		{
			PrintError("[%s] NEXUS_Graphics2D_Blit failed\n",__func__);
			nDrvErr=DRV_ERR;
			DRV_OSD_ReleaseSemaphore();
			goto done;
		}

		rc = NEXUS_StripedSurface_GetStatus(stripedSurface, &stripedSurfaceStatus);
		if (rc)
		{
			PrintError("[%s] NEXUS_Graphics2D_Blit failed\n",__func__);
			nDrvErr=DRV_ERR;
			DRV_OSD_ReleaseSemaphore();
			goto done;
		}

		StillSurface = NEXUS_Graphics2D_Destripe(blitter, stripedSurface);
		if (StillSurface == NULL)
		{
			PrintError("[%s] NEXUS_Graphics2D_Destripe failed\n",__func__);
			nDrvErr=DRV_ERR;
			DRV_OSD_ReleaseSemaphore();
			goto done;
		}

		DRV_OSD_ReleaseSemaphore();
	}
	else
	{
		PrintError("[%s] DRV_OSD_AcquireSemaphore failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	pOsdInstance=(DI_OSD_INSTANCE*)(*(unsigned long*)stTarget);

	if (DRV_OSD_CopyImage(StillSurface, (NEXUS_SurfaceHandle)pOsdInstance->surface) != DRV_OK)
	{
		PrintError("[%s] DRV_OSD_CopyImage failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
	// new still image decoding 시, org->disp surface copy 작업을 위해, bOrgDirty bit "SET" 해주어야 한다.
	pOsdInstance->bOrgDirty = TRUE;
#endif

done:
	NEXUS_StillDecoder_Stop(s_stVideo[ulDeviceId].hStillDecoder);
err:
	if (StillSurface)
	{
		NEXUS_Surface_Destroy(StillSurface);
		StillSurface=NULL;
	}

	if(event != NULL)
		BKNI_DestroyEvent(event);

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	PrintExit();
	return nDrvErr;

}

DRV_Error DRV_VIDEO_PlayMemoryStreamVideoPlane(HUINT32 ulDeviceId, DI_VIDEO_FORMAT nDiVideoFmt, HUINT8 *pucMemBuff, HULONG ulMemBuffSize)
{

	HINT32	i=0;
	HUINT32 last_numDisplayed = 0;
	HUINT32 last_fifoDepth = 0;
	HUINT32 ulStillDispCount = 0;
	DRV_Error nDrvErr=DRV_OK;
	NEXUS_VideoCodec codec;
	NEXUS_Error rc;

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	codec = P_VIDEO_Convert2NexusCodec(nDiVideoFmt);
	PrintDebug("[%s] DI_VIDEO_FORMAT (0x%08x), codec (%d)\n",__func__, nDiVideoFmt, codec);

	rc = P_VIDEO_GetPicture(ulDeviceId, codec, pucMemBuff, ulMemBuffSize);
	if(rc)
	{
		PrintError("[%s] NEXUS_StillDecoder_Start failed\n",__func__);
		nDrvErr=DRV_ERR;
		goto done;
	}

	for(i=0; i<10; i++)
	{
		NEXUS_VideoDecoderStatus decoderStatus;
		NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);

		if ( (decoderStatus.fifoDepth==last_fifoDepth) && (decoderStatus.numDisplayed>0) && (decoderStatus.numDisplayed==last_numDisplayed) )
			ulStillDispCount++;

		if (ulStillDispCount>2)
		{
			PrintStatus("[%s] Break status : i(%d), last_fifoDepth(%d), last_numDisplayed(%d) \n\n", __func__, i, last_fifoDepth, last_numDisplayed);
			break;
		}

		last_fifoDepth = decoderStatus.fifoDepth;
		last_numDisplayed = decoderStatus.numDisplayed;

		VK_TASK_Sleep(50);
	}

	s_stVideo[ulDeviceId].bFirstPtsPassed=TRUE;
done:
	DRV_VIDEO_REL_SEMA(ulDeviceId);
	PrintExit();
	return nDrvErr;
}

#if 1
DRV_Error DRV_VIDEO_Show(HUINT32 ulDeviceId, HBOOL bShow)
{
	DRV_Error nDrvErr = DRV_OK;
	int i;

	if(s_stVideo[ulDeviceId].bMosaicState == TRUE)
	{
		return nDrvErr;
	}

	for(i=0 ; i<DI_NUM_DISPLAYS ; i++)
	{
		if( (s_stVideo[ulDeviceId].bForcedMute[i] == TRUE) && (bShow == TRUE) )
		{
			PrintDebug("%d window is forced mute!! SKIP~~~ \n");
			continue;
		}

		nDrvErr = P_VIDEO_ShowwithWindowID(ulDeviceId, i, bShow);
	}

	return nDrvErr;
}
#endif
#endif

DRV_Error DRV_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDOutput)
{
	DRV_Error nDrvErr = DRV_OK;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	DI_VIDEO_RECT_t* pstHdOutRect;
	DI_VIDEO_RECT_t* pstInRect;
	NEXUS_DisplaySettings displaySettings;
	DI_VIDEO_HD_RESOLUTION eHDOutput_backup;
#if defined(CONFIG_HDMI_REFACTORING)
	HBOOL SupportedFormat;
#endif

	PrintEnter();

#if defined(CONFIG_HDMI_576P_ONLY)
	/* HDCP Athentication */
	eHDOutput = DI_VIDEO_HD_RESOLUTION_576P;
#endif /* CONFIG_HDMI_576P_ONLY */
#if defined(CONFIG_HDMI_480P_ONLY)
	/* HDCP Athentication */
	eHDOutput = DI_VIDEO_HD_RESOLUTION_480P;
#endif /* CONFIG_HDMI_480P_ONLY */

#if defined(CONFIG_BCM_ONLY_1080I)
	if((eHDOutput == DI_VIDEO_HD_RESOLUTION_1080P_50HZ) || (eHDOutput == DI_VIDEO_HD_RESOLUTION_1080P_24HZ)
		|| (eHDOutput == DI_VIDEO_HD_RESOLUTION_1080P_30HZ)|| (eHDOutput == DI_VIDEO_HD_RESOLUTION_1080P_25HZ))
	{
		PrintError("[%d] Not support 1080P mode");
		return DI_ERR_ERROR;
	}
#endif
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	pstHdOutRect=&s_stVideoInfo[ulDeviceId].stDesiredHdOutRect;
	pstInRect=&s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
	eHDOutput_backup=eHDOutput;

#if defined(CONFIG_YPBPR_PORT)
	if(s_currentOutputStatus.bYPbPr==1)
		P_VIDEO_SetDecConnectHD(ulDeviceId,FALSE);
#endif

#if defined(DI_VIDEO_SEE_PARAMETERS)
	{
		char* strResolution[DI_VIDEO_HD_RESOLUTION_MAX]={
			"DI_VIDEO_HD_RESOLUTION_480I",
			"DI_VIDEO_HD_RESOLUTION_480P",
			"DI_VIDEO_HD_RESOLUTION_576I",
			"DI_VIDEO_HD_RESOLUTION_576P",
			"DI_VIDEO_HD_RESOLUTION_720P",
			"DI_VIDEO_HD_RESOLUTION_1080I",
			"DI_VIDEO_HD_RESOLUTION_1080P",
			"DI_VIDEO_HD_RESOLUTION_720P_50Hz",
			"DI_VIDEO_HD_RESOLUTION_1080I_50Hz",
			"DI_VIDEO_HD_RESOLUTION_1080P_50HZ",
			"DI_VIDEO_HD_RESOLUTION_480I_J",
			"DI_VIDEO_HD_RESOLUTION_1080P_24HZ"
			"DI_VIDEO_HD_RESOLUTION_1080P_25HZ"
			"DI_VIDEO_HD_RESOLUTION_1080P_30HZ"
#if defined(CONFIG_HDOUT_2160P)
			"DI_VIDEO_HD_RESOLUTION_2160P_24HZ",
			"DI_VIDEO_HD_RESOLUTION_2160P_25HZ",
			"DI_VIDEO_HD_RESOLUTION_2160P_30HZ",
			"DI_VIDEO_HD_RESOLUTION_2160P_50HZ",
			"DI_VIDEO_HD_RESOLUTION_2160P",
#endif
			};

		DI_UART_Print(PARAM_COLOR_ON"%s : HD output:%d (%s)\n"PARAM_COLOR_OFF, __FUNCTION__, eHDOutput, strResolution[eHDOutput]);
	}
#endif
	if(s_stVideo[DEF_VID_INSTANCE].hDisplay[HD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[DEF_VID_INSTANCE].hDisplay[HD_DISP], &displaySettings);
#if defined(CONFIG_VIDEO_DECODER)
		if (displaySettings.format==P_VIDEO_Convert2NexusVideoFormat(eHDOutput))
		{
			goto done;
		}
#endif
	}
	else
	{
		PrintError(" Not Support HD Display !!!\n");
		goto done;
	}

#if defined(CONFIG_HDMI_REFACTORING)
	nDrvErr = DRV_HDMI_CheckSupportedResolution(displaySettings.format,&SupportedFormat);

	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_HDMI_CheckSupportedResolution error!\n", __func__);
		goto done;
	}

	if(SupportedFormat == TRUE)
	{
		PrintError("[%s] Supported Resolution [%d]!\n", __func__,eHDOutput);
	}
	else
	{
		PrintError("[%s] Not Supported Resolution [%d]!\n", __func__,eHDOutput);
	}
#endif

#if !defined(CONFIG_DIRECTFB)
	nDrvErr = DRV_OSD_ResetDisplayFormatToHD();
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_OSD_ResetDisplayFormat error!\n", __func__);
		goto done;
	}

#if defined(CONFIG_VIDEO_DECODER)
	nDrvErr = DRV_VIDEO_SetDisplayWindow(ulDeviceId, HD_DISP, pstInRect, pstHdOutRect, &eHDOutput, DI_VIDEO_WIN_HD);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_VIDEO_SetDisplayWindow error!\n", __func__);
		// do not put "goto" here.
		// The following code "DRV_OSD_SetDisplayFormat" should be executed.
	}
#endif
#endif

#if defined(CONFIG_SDHD_FILTER_6346)
	P_VIDEO_SetGpioFilter(ulDeviceId);
#endif

#if defined(CONFIG_DEINTERLACE_ONLY_P) /* Local define in this source */
	nDrvErr=DRV_VIDEO_SetDeinterlaced(ulDeviceId,TRUE,FALSE);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_VIDEO_SetDeinterlaced error!\n", __func__);
	}
#endif

#if !defined(CONFIG_DIRECTFB)
#if defined(CONFIG_HDOUT_2160P)
	nDrvErr = DRV_HDMI_SetColorInfo(eHDOutput);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_HDMI_SetColorInfo error!\n", __func__);
		goto done;
	}
#endif
	nDrvErr = DRV_OSD_SetDisplayFormatToHD();
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_OSD_SetDisplayFormat error!\n", __func__);
		goto done;
	}
#else
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	nDrvErr = DRV_OSD_SetDisplayFormatToHDForDFB(P_VIDEO_Convert2NexusVideoFormat(eHDOutput));
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_OSD_SetDisplayFormat error!\n", __func__);
		goto done;
	}
#else
	if(reschange_callback)
	{
		if((nDrvErr = reschange_callback(eHDOutput)) != DRV_OK)
		{
			PrintError("[%s] reschange_callback error!\n", __func__);
			goto done;
		}
	}
#endif
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
		nDrvErr = DRV_HDMI_SetPreEmphasisConfiguration();
		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] DRV_HDMI_SetPreEmphasisConfiguration error!\n", __func__);
			goto done;
		}
#endif

done:

#if defined(CONFIG_YPBPR_PORT)
	if(s_currentOutputStatus.bYPbPr==1)
		P_VIDEO_SetDecConnectHD(ulDeviceId,TRUE);
#endif

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return nDrvErr;
}

DRV_Error DRV_VIDEO_Init(HUINT32 ulDeviceId)
{
	NEXUS_DisplaySettings displayCfg;
	NEXUS_VideoWindowSettings windowSettings;
#if defined(CONFIG_VIDEO_DECODER)
	NEXUS_VideoDecoderSettings decoderSettings;
#endif
#if defined(CONFIG_VBI_TELETEXT)
	NEXUS_DisplayVbiSettings vbiSettings;
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	NEXUS_ComponentOutputHandle hComponentOutput;
	NEXUS_ComponentOutputSettings ComponentOutputSettings;
#endif
#if defined(CONFIG_DI20) && defined(CONFIG_3D_VIDEO)
	NEXUS_VideoDecoderExtendedSettings extSettings;
#endif
	DRV_Error nRet = DRV_OK;
	DI_NVRAM_VIDEO_RESOLUTION_E eRes;
	DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E eRate;
	int i;
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	DRV_DISP_SHAREDMEM_t *sharedHandles;
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	NEXUS_VideoDecoderOpenSettings openSettings;
	NEXUS_VideoDecoderPrimerSettings videoDecoderPrimerSettings;
#elif defined(CONFIG_TEMP_HTR1000S)
	NEXUS_VideoDecoderOpenSettings openSettings;
#elif defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)	
	NEXUS_VideoDecoderOpenSettings openSettings;
#endif

#if defined(CONFIG_VIDEO_DEC_H265)
	NEXUS_VideoFormatInfo videoFormatInfo;
#endif
	NEXUS_VideoWindowMadSettings madSettings;
	/* Determined ulDeviceId by ulDeviceId */
	HUINT32 ulWindowId = ulDeviceId;
	char ucMsgName[25];
	char ucTaskName[30];
	char ucSemName[20];

	if(ulDeviceId>=NUM_VID_DECODER)
	{
		PrintError("[%s] Required ulDeviceId =%d ",__func__,ulDeviceId);
		PrintError("is same or over than the number of video decoder %d\n",NUM_VID_DECODER);
		return DRV_ERR_INVALID_PARAMETER;
	}
	/////////////////////////////////////////////////
	NEXUS_Platform_GetConfiguration(&platformConfig); //temporary. will be deleted soon.
	/////////////////////////////////////////////////


	if(s_stVideo[ulDeviceId].bInitialized == TRUE)
	{
		PrintDebug("[%s] Current Device Id = %d is already Initialized\n",__func__,ulDeviceId);
		return DRV_ERR;
	}

	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		VK_MEM_Memset(s_stVideoInfo,0,sizeof(s_stVideoInfo));
		VK_MEM_Memset(s_stVideo,0,sizeof(s_stVideo));

#if defined(CONFIG_DIRECTFB)
		s_stVideo[ulDeviceId].hDisplay[HD_DISP] = (NEXUS_DisplayHandle)DFB_Platform_P_GetDisplayHandle(0);
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = TRUE;
			NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP],&displayCfg);
			PrintError("[%s] HD Display open !!! \n",__FUNCTION__);
		}
		else
		{
			s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = FALSE;
			PrintError("[%s] HD Display is not supported!!! \n",__FUNCTION__);
		}

#else
		/* Open Nexus Display (HD and SD) */
		NEXUS_Display_GetDefaultSettings(&displayCfg);
#endif

#if defined(CONFIG_OS_UCOS) /* TEMP_LOADER30 */
		displayCfg.format = NEXUS_VideoFormat_e720p;
		HAPPY(eRate);
		HAPPY(eRes);
#else
		nRet = DRV_NVRAM_GetField(DI_NVRAM_FIELD_HD_RESOULTION, 0, &eRes, sizeof(eRes));
		if (nRet != DRV_OK)
		{
			eRes=DI_NVRAM_RESOLUTION_1920x1080I;
		}
#if defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
		eRes = DI_VIDEO_HD_RESOLUTION_1080P;
#endif
		nRet = DRV_NVRAM_GetField(DI_NVRAM_FIELD_FIELD_RATE, 0, &eRate, sizeof(eRate));
		if (nRet != DRV_OK)
		{
			eRate=DI_NVRAM_HDMI_FIELD_RATE_50HZ;
		}
#if defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
		eRate = DI_NVRAM_HDMI_FIELD_RATE_60HZ;
#endif
		displayCfg.format=P_VIDEO_NvramConvert2NexusVideoFormat(eRes,eRate);
#endif

		displayCfg.aspectRatio	= NEXUS_DisplayAspectRatio_e16x9; /* Aspect Ratio 16x9 */
		displayCfg.displayType = NEXUS_DisplayType_eAuto;

#ifdef CONFIG_HD_RESOLUTION_CHECK
		HD_Format_Desired =  displayCfg.format;
#endif

#if (NEXUS_VERSION >= 1404) || ((NEXUS_VERSION >= 1304) && defined(CONFIG_YPBPR_PORT))
		displayCfg.timingGenerator = NEXUS_DisplayTimingGenerator_eAuto;
#else
		displayCfg.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo;
#endif
#if defined(CONFIG_HDMI_TIMEBASE_SEPARATE)
#if (NEXUS_VERSION <= 1404)
		displayCfg.timebase = NEXUS_Timebase_e3;
#else
		displayCfg.timebase = NEXUS_Timebase_eMax - 1;
#endif
#endif
#if (NEXUS_VERSION >= 1501)
		displayCfg.dropFrame = NEXUS_TristateEnable_eEnable ;
#endif

#if defined (CONFIG_DIRECTFB)
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
			NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP],&displayCfg);
#else
		s_stVideo[ulDeviceId].hDisplay[HD_DISP] = NEXUS_Display_Open(HD_DISP, &displayCfg);
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = TRUE;
			PrintError("[%s] HD Display open !!! \n",__FUNCTION__);
		}
		else
		{
			s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = FALSE;
			PrintError("[%s] HD Display is not supported!!! \n",__FUNCTION__);
		}
#endif

#if defined(CONFIG_DIRECTFB)
		s_stVideo[ulDeviceId].hDisplay[SD_DISP] = (NEXUS_DisplayHandle)DFB_Platform_P_GetDisplayHandle(1);
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = TRUE;
			NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&displayCfg);
			PrintError("[%s] SD Display open !!! \n",__FUNCTION__);
		}
		else
		{
			s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = FALSE;
			PrintError("[%s] SD Display is not supported!!! \n",__FUNCTION__);
		}
#else
		/* Open Nexus Display (HD and SD) */
		NEXUS_Display_GetDefaultSettings(&displayCfg);
#endif

#if defined(CONFIG_SDOUT_NTSC)
		displayCfg.format = NEXUS_VideoFormat_eNtsc;
#elif defined(CONFIG_SDOUT_NTSC_JAPAN)
		displayCfg.format = NEXUS_VideoFormat_eNtscJapan;
#else
		displayCfg.format = NEXUS_VideoFormat_ePal; /* Default SD format ePal */
#endif

#if defined(CONFIG_HDMI_TIMEBASE_SEPARATE)
#if (NEXUS_VERSION <= 1404)
		displayCfg.timebase = NEXUS_Timebase_e3;
#else
		displayCfg.timebase = NEXUS_Timebase_eMax - 1;
#endif
#endif
#if (NEXUS_VERSION >= 1501)
		displayCfg.dropFrame = NEXUS_TristateEnable_eEnable ;
#endif

#if defined (CONFIG_DIRECTFB)
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
			NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&displayCfg);
#else
		s_stVideo[ulDeviceId].hDisplay[SD_DISP] = NEXUS_Display_Open(SD_DISP, &displayCfg);
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = TRUE;
			PrintError("[%s] SD Display open !!! \n",__FUNCTION__);
		}
		else
		{
			s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = FALSE;
			PrintError("[%s] SD Display is not supported!!! \n",__FUNCTION__);
		}
#endif
	}
	else
	{
		if(s_stVideo[DI_VIDEO_DECODER_MAIN].hDisplay[HD_DISP] != NULL)
		{
			s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = TRUE;
			s_stVideo[ulDeviceId].hDisplay[HD_DISP] = s_stVideo[DI_VIDEO_DECODER_MAIN].hDisplay[HD_DISP];
			PrintError("[%s] HD Secondary Display open !!! \n",__FUNCTION__);
		}
		else
		{
			s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = FALSE;
			s_stVideo[ulDeviceId].hDisplay[HD_DISP] = NULL;
			PrintError("[%s] HD Secondary Display is not supported!!! \n",__FUNCTION__);
		}


		if(s_stVideo[DI_VIDEO_DECODER_MAIN].hDisplay[SD_DISP] != NULL)
		{
			s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = TRUE;
			s_stVideo[ulDeviceId].hDisplay[SD_DISP] = s_stVideo[DI_VIDEO_DECODER_MAIN].hDisplay[SD_DISP];
			PrintError("[%s] SD Secondary Display open !!! \n",__FUNCTION__);
		}
		else
		{
			s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = FALSE;
			s_stVideo[ulDeviceId].hDisplay[SD_DISP] = NULL;
			PrintError("[%s] SD Secondary Display is not supported!!! \n",__FUNCTION__);
		}

	}
	/* Open Nexus Window (HD and SD) */
#if defined (CONFIG_DIRECTFB)
	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] = (NEXUS_VideoWindowHandle)DFB_Platform_P_GetDisplayWindowHandle(s_stVideo[ulDeviceId].hDisplay[HD_DISP], ulWindowId);

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] = (NEXUS_VideoWindowHandle)DFB_Platform_P_GetDisplayWindowHandle(s_stVideo[ulDeviceId].hDisplay[SD_DISP], ulWindowId);
#else
	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] = NEXUS_VideoWindow_Open(s_stVideo[ulDeviceId].hDisplay[HD_DISP], ulWindowId);

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] = NEXUS_VideoWindow_Open(s_stVideo[ulDeviceId].hDisplay[SD_DISP], ulWindowId);
#endif

#if (NEXUS_VERSION>600)
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		/* To prevent video blanking during SetOutWindow */
		NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);
		windowSettings.forceCapture = true;
		windowSettings.scaleFactorRounding.enabled = false;
#if defined(CONFIG_BCM_ONLY_1080I) || (BCHP_CHIP == 7358)
		windowSettings.minimumSourceFormat = NEXUS_VideoFormat_e720p50hz;
#else
		windowSettings.minimumSourceFormat = NEXUS_VideoFormat_e1080p50hz;
#endif
		windowSettings.allocateFullScreen=true;
		windowSettings.visible = false;
#if defined(CONFIG_WORKAROUND_WIN_CLIP)
		windowSettings.vdcapply=true;
#endif
		NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);
	}

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
	{
		/* To prevent video blanking during SetOutWindow */
		NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &windowSettings);
		windowSettings.forceCapture = true;
		windowSettings.scaleFactorRounding.enabled = false;
		windowSettings.minimumSourceFormat = NEXUS_VideoFormat_ePal;
		windowSettings.allocateFullScreen=true;
		windowSettings.visible = false;

#if defined(CONFIG_WORKAROUND_WIN_CLIP)
		windowSettings.vdcapply=true;
#endif
		NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &windowSettings);
	}
#endif

#if !defined(CONFIG_DEINTERLACE_ONLY_P) /* Local define in this source */
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
		madSettings.deinterlace=true;
		NEXUS_VideoWindow_SetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
	}
#endif

	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
#if !defined(CONFIG_DIRECTFB)
	/* When using DirecFB, the CVBS port is connected to SD Display Handle at initialzing directfb.
	* So there is no more need to add cvbs port at this time.
	*/

#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 1
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			if(NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[SD_DISP], NEXUS_CompositeOutput_GetConnector(P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPOSITE))) != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_AddOutput [P_VIDEO_OUTPUT_MODE_COMPOSITE].\n\r", __func__);
			}
		}
#endif

#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
#if defined(CONFIG_YPBPR_PORT)
		/* Connect Display and Output Ports (VDECs)*/
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			hComponentOutput=P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT);
			NEXUS_ComponentOutput_GetSettings(hComponentOutput, &ComponentOutputSettings);
			ComponentOutputSettings.type = NEXUS_ComponentOutputType_eYPrPb;
			NEXUS_ComponentOutput_SetSettings(hComponentOutput, &ComponentOutputSettings);
			if(NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[HD_DISP], NEXUS_ComponentOutput_GetConnector(P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT))) != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_AddOutput YPbPR [P_VIDEO_OUTPUT_MODE_COMPONENT].\n\r", __func__);
			}

			s_currentOutputStatus.bYPbPr = 1;
			s_currentOutputStatus.bRGB = 0;
#if defined(CONFIG_SDHD_FILTER_6346)
			P_VIDEO_SetGpioFilter(ulDeviceId);
#endif
		}
#else
#if defined(CONFIG_DIRECTFB)
	/* When using DirecFB, the YPBPR port is connected to HD Display Handle at initializing directfb.
	* So Using RGB port, must be remove YPBPR port from HD Display Handle.
	*/
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			if(NEXUS_Display_RemoveOutput(s_stVideo[ulDeviceId].hDisplay[HD_DISP], NEXUS_ComponentOutput_GetConnector(P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT))) != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_RemoveOutput YPbPR [P_VIDEO_OUTPUT_MODE_COMPONENT].\n\r", __func__);
			}
		}
#endif
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			hComponentOutput=P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT);
			NEXUS_ComponentOutput_GetSettings(hComponentOutput, &ComponentOutputSettings);
			ComponentOutputSettings.type = NEXUS_ComponentOutputType_eRGB;
			ComponentOutputSettings.dacs.RGB.dacHSync = NEXUS_VideoDac_eNone;
			ComponentOutputSettings.dacs.RGB.noSync = true;
			NEXUS_ComponentOutput_SetSettings(hComponentOutput, &ComponentOutputSettings);
			if(NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[SD_DISP], NEXUS_ComponentOutput_GetConnector(P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT))) != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_AddOutput RGB [P_VIDEO_OUTPUT_MODE_COMPONENT].\n\r", __func__);
			}
			s_currentOutputStatus.bYPbPr = 0;
			s_currentOutputStatus.bRGB = 1;
		}
#endif
#endif

#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 2
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
			NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[SD_DISP], NEXUS_CompositeOutput_GetConnector(P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPOSITE2)));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS >= 1
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			if (P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_SVIDEO))
			{
				NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[SD_DISP], NEXUS_SvideoOutput_GetConnector(P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_SVIDEO)));
				s_currentOutputStatus.bSVHS = 1;
			}
		}
#endif
#if defined(CONFIG_SCALER_BEFORE_CAPTURE)
		{
			if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
			{
				NEXUS_VideoWindowScalerSettings windowScalerSettings;
				HUINT32 ulWindowId = ulDeviceId;
				NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
				if(windowScalerSettings.bandwidthEquationParams.bias != NEXUS_ScalerCaptureBias_eScalerBeforeCapture)
				{
					windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
					windowScalerSettings.bandwidthEquationParams.delta=1*1000*1000;
					NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
				}
			}
		}
#endif
		/* Default Output CVBS, SVHS and YPbPr or RGB On */

		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
			s_currentOutputStatus.bCVBS = 1;
		else
			s_currentOutputStatus.bCVBS = 0;

#if (NEXUS_VERSION >= 1404)
#if !defined(CONFIG_DIRECTFB)
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			if(NEXUS_Display_AddOutput(s_stVideo[ulDeviceId].hDisplay[HD_DISP], NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0])) != NEXUS_SUCCESS)
			{
				PrintError("[%s] NEXUS_Display_AddOutput [P_VIDEO_OUTPUT_HD].\n\r", __func__);
			}
		}
#endif
#endif
	}

	/* Open Nexus Decoder */
#if defined(CONFIG_VIDEO_DECODER)
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
	openSettings.fifoSize = 8*1024*1024;
	s_stVideo[ulDeviceId].hDecoder = NEXUS_VideoDecoder_Open(ulDeviceId, &openSettings);
#elif defined(CONFIG_TEMP_HTR1000S)
//for HTR-1000S
//to fix No audio issue playing a media file - Wildlife.wmv(windows7 example file)
	NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
	openSettings.fifoSize = 3*1024*1024;
	s_stVideo[ulDeviceId].hDecoder = NEXUS_VideoDecoder_Open(ulDeviceId, &openSettings);
#else
#if defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)	
	NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
	openSettings.cdbHeap = platformConfig.heap[NEXUS_VIDEO_SECURE_HEAP];
	openSettings.fifoSize = 8*1024*1024;

	s_stVideo[ulDeviceId].hDecoder = NEXUS_VideoDecoder_Open(ulDeviceId, &openSettings); /* take default capabilities */
#else
	s_stVideo[ulDeviceId].hDecoder = NEXUS_VideoDecoder_Open(ulDeviceId, NULL); /* take default capabilities */
#endif
#endif
	BDBG_ASSERT(NULL != s_stVideo[ulDeviceId].hDecoder );
	/* Nexus Decoder Event Settings */
	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
	decoderSettings.sourceChanged.callback=P_VIDEO_SourceChangedCallBack;
	decoderSettings.sourceChanged.param=ulDeviceId;
	decoderSettings.streamChanged.callback=P_VIDEO_StreamChangedCallBack;
	decoderSettings.streamChanged.param=ulDeviceId;
	decoderSettings.firstPts.callback=P_VIDEO_PtsPassedCallBack;
	decoderSettings.firstPts.param=ulDeviceId;
	decoderSettings.firstPtsPassed.callback=P_VIDEO_FirstPtsPassedCallBack;
	decoderSettings.firstPtsPassed.param=ulDeviceId;
	decoderSettings.ptsError.callback=P_VIDEO_ptsErrorCallBack;
	decoderSettings.ptsError.param=ulDeviceId;
	decoderSettings.fifoEmpty.callback=P_VIDEO_fifoEmptyCallBack;
	decoderSettings.fifoEmpty.param=ulDeviceId;
#if defined(CONFIG_VIDEO_ERROR_LOG)
	decoderSettings.decodeError.callback=P_VIDEO_decodeErrorCallBack;
	decoderSettings.decodeError.param=ulDeviceId;
#endif
	decoderSettings.channelChangeMode=NEXUS_VideoDecoder_ChannelChangeMode_eMute;
#if defined(CONFIG_AFD_EVENT)
	/* Enable to get UserData */
	decoderSettings.userDataEnabled=TRUE;
	decoderSettings.appUserDataReady.callback=P_VIDEO_AppUserDataReadyCallBack;
	decoderSettings.appUserDataReady.param=ulDeviceId;
#endif
	decoderSettings.ptsOffset = PTSOFFSET*45;
#if defined(CONFIG_DI20) && defined(CONFIG_3D_VIDEO)
	decoderSettings.customSourceOrientation = true;
	decoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
#endif

#if defined(CONFIG_VIDEO_DEC_H265)
	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		if(decoderSettings.supportedCodecs[NEXUS_VideoCodec_eH265] == true )

		{
			NEXUS_VideoFormat_GetInfo(HEVC_MAX_RES, &videoFormatInfo);
#if !defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
			decoderSettings.maxWidth = videoFormatInfo.width;
			decoderSettings.maxHeight = videoFormatInfo.height;
#endif
		}
	}
#endif

	NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);

#if defined(CONFIG_DI20) && defined(CONFIG_3D_VIDEO)
	NEXUS_VideoDecoder_GetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);
	extSettings.s3DTVStatusEnabled = true;
	extSettings.s3DTVStatusChanged.callback = P_VIDEO_3DInfoChangeCallback;
	extSettings.s3DTVStatusChanged.context = s_stVideo[ulDeviceId].hDecoder;
	extSettings.s3DTVStatusTimeout = VIDEO_3D_STATUS_TIMEOUT;
	extSettings.s3DTVStatusChanged.param = ulDeviceId;
	NEXUS_VideoDecoder_SetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);
#endif
	s_stVideo[ulDeviceId].hInput = NEXUS_VideoDecoder_GetConnector(s_stVideo[ulDeviceId].hDecoder);
	BDBG_ASSERT(NULL != s_stVideo[ulDeviceId].hInput);
	/* Connect Window and Input */
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	/* Open Nexus Still Decoder */
	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		s_stVideo[ulDeviceId].hStillDecoder=NEXUS_StillDecoder_Open(s_stVideo[ulDeviceId].hDecoder, 0, NULL);
		BDBG_ASSERT(NULL != s_stVideo[ulDeviceId].hStillDecoder );
	}

	for(i=0 ; i<TOTAL_CALLER_NUM ; i++)
	{
		int iEventType;
		for (iEventType=0 ; iEventType<DI_VIDEO_NOTIFY_EVT_END ; iEventType++)
		{
			s_stVideo[ulDeviceId].stCallbackInfo[i][iEventType].pfnCallback = NULL;
		}
	}
#if defined(CONFIG_CLOSED_CAPTION)
	s_stVideo[ulDeviceId].stCcCallbackInfo.pfnCcDataCallback = NULL;
#endif

#endif

	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
#if defined(CONFIG_CLOSED_CAPTION)		
		NEXUS_VideoInputVbiSettings viVbiSettings;
		
		NEXUS_VideoInput_GetVbiSettings(s_stVideo[ulDeviceId].hInput, &viVbiSettings);
		viVbiSettings.closedCaptionEnabled = true;
		viVbiSettings.closedCaptionBufferSize = 400;
		viVbiSettings.closedCaptionDataReady.callback = P_VIDEO_ClosedCaptionCallback;
		viVbiSettings.closedCaptionDataReady.param = ulDeviceId;				
		NEXUS_VideoInput_SetVbiSettings(s_stVideo[ulDeviceId].hInput, &viVbiSettings);	
#endif
#if defined(CONFIG_VBI_TELETEXT) || defined(CONFIG_CLOSED_CAPTION)
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&vbiSettings);
			vbiSettings.vbiSource = s_stVideo[ulDeviceId].hInput;
			NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&vbiSettings);
		}

		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP],&vbiSettings);
			vbiSettings.vbiSource = s_stVideo[ulDeviceId].hInput;
			NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP],&vbiSettings);
		}
#endif

		s_stVideo[ulDeviceId].bVbiTeletext=FALSE;
		s_stVideo[ulDeviceId].bVbiClosedCaption=FALSE;

		VK_MEM_Memset(ucSemName, 0x00, sizeof(ucSemName));
		VK_snprintf(ucSemName, 13, "di_video%d_di", ulDeviceId);
		nRet = VK_SEM_Create(&s_stVideo[ulDeviceId].ulSemId_DI, ucSemName, DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("[%s] SEM create failed.\n\r", __func__);
			return DRV_ERR;
		}
		VK_MEM_Memset(ucSemName, 0x00, sizeof(ucSemName));
		VK_snprintf(ucSemName, 14, "di_video%d_drv", ulDeviceId);
		nRet = VK_SEM_Create(&s_stVideo[ulDeviceId].ulSemId_DRV, ucSemName, DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("[%s] SEM create failed.\n\r", __func__);
			return DRV_ERR;
		}
		VK_MEM_Memset(ucSemName, 0x00, sizeof(ucSemName));
		VK_snprintf(ucSemName, 12, "di_video%d_p", ulDeviceId);
		nRet = VK_SEM_Create(&s_stVideo[ulDeviceId].ulSemId_P, ucSemName, DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("[%s] SEM create failed.\n\r", __func__);
			return DRV_ERR;
		}
		VK_MEM_Memset(ucSemName, 0x00, sizeof(ucSemName));
		VK_snprintf(ucSemName, 15,"di_video%d_show", ulDeviceId);
		nRet = VK_SEM_Create(&s_stVideo[ulDeviceId].ulSemId_ShowHide, ucSemName, DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("[%s] SEM create failed.\n\r", __func__);
			return DRV_ERR;
		}

#if defined(CONFIG_CLOSED_CAPTION)
		VK_MEM_Memset(ucSemName, 0x00, sizeof(ucSemName));
		VK_snprintf(ucSemName, 15,"di_video%d_cc", ulDeviceId);
		nRet = VK_SEM_Create(&s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption, ucSemName, DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("[%s] SEM create failed.\n\r", __func__);
			return DRV_ERR;
		}
#endif
	}
	else
	{
		s_stVideo[ulDeviceId].ulSemId_DI = s_stVideo[DI_VIDEO_DECODER_MAIN].ulSemId_DI;
		s_stVideo[ulDeviceId].ulSemId_DRV = s_stVideo[DI_VIDEO_DECODER_MAIN].ulSemId_DRV;
		s_stVideo[ulDeviceId].ulSemId_P = s_stVideo[DI_VIDEO_DECODER_MAIN].ulSemId_P;
		s_stVideo[ulDeviceId].ulSemId_ShowHide = s_stVideo[DI_VIDEO_DECODER_MAIN].ulSemId_ShowHide;
#if defined(CONFIG_CLOSED_CAPTION)		
		s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption = s_stVideo[DI_VIDEO_DECODER_MAIN].stClosedCaptionInfo.ulSemId_ClosedCaption;
#endif		
	}

#if defined(CONFIG_OS_UCOS) /* TEMP_LOADER30 */
	HAPPY(ucTaskName);
	HAPPY(ucMsgName);
#else
	VK_MEM_Memset(ucMsgName, 0x00, sizeof(ucMsgName));
	VK_snprintf(ucMsgName, 18,"VideoMonMsgQueue%d", ulDeviceId);
	if(VK_MSG_Create(VIDEO_MESSAGE_QUEUE_SIZE, sizeof(ST_VIDEO_MSG),ucMsgName,&s_stVideo[ulDeviceId].ulVideoMonMsgQueueId,VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		PrintError("[%s] Creating Video Mon Msg Qeueu failed!\n",__func__);
		return DRV_ERR;
	}

	VK_MEM_Memset(ucTaskName, 0x00, sizeof(ucTaskName));
	VK_snprintf(ucTaskName, 25,"P_VIDEO_BuffMonitorTask%d", ulDeviceId);
	s_stVideo[ulDeviceId].ulDeviceId = ulDeviceId;
	if(VK_TASK_Create(P_VIDEO_BuffMonitorTask, VIDEO_MSG_TASK_PRIORITY, VIDEO_MSG_TASK_STACK_SIZE, ucTaskName, (void *)&s_stVideo[ulDeviceId].ulDeviceId, &s_stVideo[ulDeviceId].ulVideoMonitorTaskId, FALSE)!=VK_OK)
	{
		PrintError("[%s] Creating BuffMonitor Task failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	VK_TASK_Start(s_stVideo[ulDeviceId].ulVideoMonitorTaskId);

#if defined(CONFIG_CLOSED_CAPTION)
	VK_MEM_Memset(ucMsgName, 0x00, sizeof(ucMsgName));
	VK_snprintf(ucMsgName, 23,"ClosedCaptionMsgQueue%d", ulDeviceId);
	if(VK_MSG_Create(64/*VIDEO_MESSAGE_QUEUE_SIZE*/, sizeof(P_VIDEO_CC_MSG),ucMsgName,&s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionMsgQueueId,VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		PrintError("[%s] Creating ClosedCaption Msg Qeueu failed!\n",__func__);
		return DRV_ERR;
	}

	VK_MEM_Memset(ucTaskName, 0x00, sizeof(ucTaskName));
	VK_snprintf(ucTaskName, 27,"P_VIDEO_ClosedCaptionTask%d", ulDeviceId);
	s_stVideo[ulDeviceId].ulDeviceId = ulDeviceId;
	if(VK_TASK_Create(P_VIDEO_ClosedCaptionTask, VIDEO_MSG_TASK_PRIORITY, NORMAL_TASK_STACK_SIZE, ucTaskName, (void *)&s_stVideo[ulDeviceId].ulDeviceId, &s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionTaskId, FALSE)!=VK_OK)
	{
		PrintError("[%s] Creating ClosedCaption Task failed!\n",__FUNCTION__);
		return DRV_ERR;
	}

	VK_TASK_Start(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionTaskId);
#endif	

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	VK_TASK_RegisterPM(s_stVideo[ulDeviceId].ulVideoMonitorTaskId);
#endif

#endif

	s_stVideo[ulDeviceId].bDecoding=FALSE;
	s_stVideo[ulDeviceId].bIsPlayback=FALSE;
	s_stVideo[ulDeviceId].bNeedToShow=FALSE;
	s_stVideo[ulDeviceId].bInitialized=TRUE;
	s_stVideo[ulDeviceId].bFirstPtsPassed=FALSE;
	s_stVideo[ulDeviceId].bFreeze=FALSE;
	s_stVideo[ulDeviceId].bDecState=FALSE;
	s_stVideo[ulDeviceId].bTsm=TRUE;
	s_stVideo[ulDeviceId].ulApplyScalerTimerId = VK_TIMERID_NULL;
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	s_stVideo[ulDeviceId].bIsMedia=FALSE;
#endif
	s_stVideo[ulDeviceId].bMosaicState = FALSE;
#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
	s_stVideo[ulDeviceId].bDecReset=FALSE;
	s_stVideo[ulDeviceId].ulResetTimerId=VK_TIMERID_NULL;
#endif
	s_stVideo[ulDeviceId].bIsStill=FALSE;
	s_stVideo[ulDeviceId].ulUnderrunTimeout = DI_VIDEO_UNDERRUN_TIMEOUT_DEFAULT;
#if defined(CONFIG_DI20)
	s_stVideo[ulDeviceId].e3dFormat = DI_VIDEO_3D_FORMAT_NONE;
	s_stVideo[ulDeviceId].e3dDisplayFormat = DI_VIDEO_3D_FORMAT_NONE;
#endif
#if defined(CONFIG_VIDEO_ERROR_LOG)
	s_stVideo[ulDeviceId].ulErrorTimerId = VK_TIMERID_NULL;
#endif
	s_stVideo[ulDeviceId].nUserOffset = 0;

	for(i = 0; i < DI_NUM_MOSAIC_WINDOWS; i++)
	{
		s_stVideo[ulDeviceId].stMosaic[i].bOpen = FALSE;
		s_stVideo[ulDeviceId].stMosaic[i].bUsed = FALSE;
	}

	s_stVideo[ulDeviceId].eDecoderMode = DI_VIDEO_DECODER_MODE_NORMAL;
	s_eRequestDecoderMode[ulDeviceId] = DI_VIDEO_DECODER_MODE_NORMAL;
	s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState = DI_VIDEO_CC_DATA_STATE_UNKNOWN;

#if defined(CONFIG_HDMI_576P_ONLY)
	/*	hdcp athentication
	*	hdcp test device needs that the settop is set as 480P or 576P!!
	*/
	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		NEXUS_DisplaySettings displaySettings;
		NEXUS_Error nResult;
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			displaySettings.format = NEXUS_VideoFormat_e576p;
			nResult = NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Display_SetSettings failed!\n");
				return DRV_ERR;
			}
		}
	}
#endif /* CONFIG_HDMI_576P_ONLY */
#if defined(CONFIG_HDMI_480P_ONLY)
	/*	hdcp athentication
	*	hdcp test device needs that the settop is set as 480P or 576P!!
	*/
	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		NEXUS_DisplaySettings displaySettings;
		NEXUS_Error nResult;
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			displaySettings.format = NEXUS_VideoFormat_e480p;
			nResult = NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_Display_SetSettings failed!\n");
				return DRV_ERR;
			}
		}
	}
#endif /* CONFIG_HDMI_480P_ONLY */


#ifdef SD_DISPLAY_TIMEBASE_FREERUN
	P_VIDEO_SetSDTimebaseFreerun();
#endif

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	DRV_SHAREDMEM_Init(sizeof(DRV_DISP_SHAREDMEM_t),DRV_DISP_SHAREDMEM_ID);
	sharedMem=DRV_SHAREDMEM_Setup(sizeof(DRV_DISP_SHAREDMEM_t),DRV_DISP_SHAREDMEM_ID,true);
	sharedHandles=(DRV_DISP_SHAREDMEM_t *)sharedMem;
	sharedHandles->disp_handle0 = s_stVideo[ulDeviceId].hDisplay[HD_DISP];
	sharedHandles->disp_handle1 = s_stVideo[ulDeviceId].hDisplay[SD_DISP];
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		for(i=0; i<VIDEO_PRIMER_MAX_COUNT; i++)
		{
#if (NEXUS_VERSION >= 1401)
			s_stVideoPrimer.hPrimer[i] = NEXUS_VideoDecoderPrimer_Create(NULL);
#else
			s_stVideoPrimer.hPrimer[i] = NEXUS_VideoDecoderPrimer_Open(s_stVideo[ulDeviceId].hDecoder);
#endif
			NEXUS_VideoDecoderPrimer_GetSettings(s_stVideoPrimer.hPrimer[i], &videoDecoderPrimerSettings);
			videoDecoderPrimerSettings.pastTolerance=45000; /* PTS unit (45KHz tick), e.g. 45000 = 1s */
			videoDecoderPrimerSettings.futureTolerance=0; /* this comes at a slightly higher CDB size requirement (PTS > STC) */
			videoDecoderPrimerSettings.ptsStcDiffCorrectionEnabled=TRUE;
			NEXUS_VideoDecoderPrimer_SetSettings(s_stVideoPrimer.hPrimer[i], &videoDecoderPrimerSettings);
		}
	}
	s_stVideoPrimer.bStartPrimer = FALSE;
#endif
	return DRV_OK;
}

DRV_Error DRV_VIDEO_UnInit(HUINT32 ulDeviceId)
{

	HUINT32 ulWindowId = ulDeviceId;
	DRV_Error nDrvErr = DRV_OK;
	int ret = 0;

	/* Stop Video Decoder */
	nDrvErr = DRV_VIDEO_Stop(ulDeviceId);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] Stop failed!\n",__func__);
	}

	/* Stop Task */
	ret = VK_TASK_Stop(s_stVideo[ulDeviceId].ulVideoMonitorTaskId);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}

	/* Destroy Task */
	ret = VK_TASK_Destroy(s_stVideo[ulDeviceId].ulVideoMonitorTaskId);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}

	/* Destroy Message */
	ret = VK_MSG_Destroy(s_stVideo[ulDeviceId].ulVideoMonMsgQueueId);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}

#if defined(CONFIG_CLOSED_CAPTION)
	ret = VK_TASK_Stop(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionTaskId);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}

	ret=VK_MSG_Destroy(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionMsgQueueId);
	if (ret != VK_OK)
	{
		PrintError("[%s] VK_MSG_Destroy failed!\n", __FUNCTION__);
	}
	
	ret = VK_TASK_Destroy(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulClosedCaptionTaskId);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}
#endif

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_Close(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_Close(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId]);

#if defined (CONFIG_DIRECTFB)
	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] = FALSE;
	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		s_stVideo[ulDeviceId].bOpenDisplay[SD_DISP] = FALSE;
#else
	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		NEXUS_Display_Close(s_stVideo[ulDeviceId].hDisplay[HD_DISP]);

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		NEXUS_Display_Close(s_stVideo[ulDeviceId].hDisplay[SD_DISP]);
#endif


#if defined(CONFIG_VIDEO_DECODER)
	NEXUS_VideoDecoder_Close(s_stVideo[ulDeviceId].hDecoder);
#endif

	ret = VK_SEM_Destroy(s_stVideo[ulDeviceId].ulSemId_DI);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}

	ret = VK_SEM_Destroy(s_stVideo[ulDeviceId].ulSemId_DRV);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}
	ret = VK_SEM_Destroy(s_stVideo[ulDeviceId].ulSemId_P);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}
#if defined(CONFIG_CLOSED_CAPTION)
	ret = VK_SEM_Destroy(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);
	if( ret != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, ret);
	}
#endif
	s_stVideo[ulDeviceId].bInitialized=FALSE;

	return nDrvErr;
}

#if defined(CONFIG_VIDEO_DECODER)
#if 0
DI_ERR_CODE DI_VIDEO_Init(HUINT32 ulDeviceId)
{
	DRV_VIDEO_Init(ulDeviceId);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_UnInit(HUINT32 ulDeviceId)
{
	DRV_VIDEO_UnInit(ulDeviceId);
	return DI_ERR_OK;
}
#endif

DRV_Error DRV_VIDEO_Start(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec,HBOOL bTsm)
{
	NEXUS_Error nexusError;
	NEXUS_VideoDecoderStartSettings *startSettings;
#if (NEXUS_VERSION < 750)
	NEXUS_VideoDecoderSettings decoderSettings;
#endif
	ST_VIDEO_MSG VideoMonMsg;
	DRV_Error nDrvErr = DRV_OK;
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	DI_MEDIA_VIDEO_FORMAT_e eMediaVideoCodec = DI_VIDEO_FORMAT_NONE;
	DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType;
#endif
	HBOOL bIsPlayback = FALSE;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode = DRV_DEMUX_PLAYBACK_UNKNOWN;

#if defined(CONFIG_ASTM)
	HBOOL astmStarted = FALSE;
#endif
	HBOOL bIsRfvodMode = FALSE;
	char strTemp[100];

#if !defined(CONFIG_DEBUG)
	HAPPY(s_ulErrorCheckTimerId);
	HAPPY(s_stPrevVideoErrorStatus);
#endif

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);
	if(s_stVideo[ulDeviceId].bDecState == TRUE)
	{
		PrintError("video decoder is already started!!!\n");
		nDrvErr=DRV_ERR;
		goto done;
	}

	startSettings = &s_stVideo[ulDeviceId].StartSettings;
	/* Open the pid channels */
	NEXUS_VideoDecoder_GetDefaultStartSettings(startSettings);
	startSettings->codec = P_VIDEO_Convert2NexusCodec(VideoCodec);

	DRV_RFVOD_GetRfvodMode(&bIsRfvodMode);

	if(bIsRfvodMode == TRUE)
	{
		startSettings->errorHandling = NEXUS_VideoDecoderErrorHandling_ePrognostic;
	}
	else
	{
		startSettings->errorHandling = NEXUS_VideoDecoderErrorHandling_eNone;
	}
#if !defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
	DRV_VIDEO_GetVideoCodecStr(s_stVideo[ulDeviceId].StartSettings.codec, strTemp);
	DI_UART_Print(PARAM_COLOR_ON"%s : device id:%d, codec:%s\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId, strTemp);
#endif
	if(VideoCodec >= DI_VIDEO_FORMAT_MEDIA_MPEG1)
	{
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
		switch(ulDeviceId)
		{
			case 0:
				eVideoDecoderType = DI_MEDIA_VIDEO_DECODER_MAIN;
				break;
			default:
				eVideoDecoderType = DI_MEDIA_VIDEO_DECODER_NONE;
				break;
		}

		if(eVideoDecoderType == DI_MEDIA_VIDEO_DECODER_NONE)
		{
			PrintError("[%s] Invalid eVideoDecoderType\n",__func__);
			nDrvErr=DRV_ERR;
			goto done;
		}

		switch(VideoCodec)
		{
			case DI_VIDEO_FORMAT_MEDIA_MPEG1:
				eMediaVideoCodec = DI_VIDEO_FORMAT_MPEG1;
				break;
			case DI_VIDEO_FORMAT_MEDIA_MPEG2:
				eMediaVideoCodec = DI_VIDEO_FORMAT_MPEG2;
				break;
			case DI_VIDEO_FORMAT_MEDIA_MPEG4Part2:
				eMediaVideoCodec = DI_VIDEO_FORMAT_MPEG4Part2;
				break;
			case DI_VIDEO_FORMAT_MEDIA_H261:	//divx test
				eMediaVideoCodec = DI_VIDEO_FORMAT_H261; /*seemed to be unsupported in Nexus platform, but can be modified.*/
				break;
			case DI_VIDEO_FORMAT_MEDIA_H263:
				eMediaVideoCodec = DI_VIDEO_FORMAT_H263;
				break;
			case DI_VIDEO_FORMAT_MEDIA_H264:
				eMediaVideoCodec = DI_VIDEO_FORMAT_H264;
				break;
			case DI_VIDEO_FORMAT_MEDIA_H265:
				eMediaVideoCodec = DI_VIDEO_FORMAT_H265;
				break;
			case DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain:
				eMediaVideoCodec = DI_VIDEO_FORMAT_VC1SimpleMain;
				break;
			case DI_VIDEO_FORMAT_MEDIA_VC1:
				eMediaVideoCodec = DI_VIDEO_FORMAT_VC1;
				break;
			case DI_VIDEO_FORMAT_MEDIA_AVS:
				eMediaVideoCodec = DI_VIDEO_FORMAT_AVS;
				break;
			case DI_VIDEO_FORMAT_MEDIA_DIVX_311:
				eMediaVideoCodec = DI_VIDEO_FORMAT_DIVX_311;
				break;
#if defined(CONFIG_VIDEO_DEC_VP8)
			case DI_VIDEO_FORMAT_MEDIA_VP8:
				eMediaVideoCodec = DI_VIDEO_FORMAT_VP8;
				break;
#endif
#if defined(CONFIG_VIDEO_DEC_VP9)
			case DI_VIDEO_FORMAT_MEDIA_VP9:
				eMediaVideoCodec = DI_VIDEO_FORMAT_VP9;
				break;
#endif
			default:
				eMediaVideoCodec = DI_VIDEO_FORMAT_NONE;
				break;
		}

		if(eMediaVideoCodec == DI_MEDIA_VIDEO_FORMAT_NONE)
		{
			PrintError("[%s] Invalid eMediaVideoCodec\n",__func__);
			nDrvErr=DRV_ERR;
			goto done;
		}

		PrintDebug("[%s] MEDIA PLAY\n\r",__FUNCTION__);
#if defined(CONFIG_MEDIA)
		if(DRV_MEDIA_CheckStart())
		{
			startSettings->pidChannel = DRV_MEDIA_GetPlaybackVideoPidChannel(
					eVideoDecoderType,
					eMediaVideoCodec,
					(void *)s_stVideo[ulDeviceId].hDecoder);
			if(startSettings->pidChannel == NULL)
			{
				PrintError("[%s] PlaybackPidChannel is NULL\n",__func__);
				nDrvErr=DRV_ERR;
				goto done;
			}

			startSettings->stcChannel = DRV_MEDIA_MP_GetPlaybackStcChannelHandle(
					eVideoDecoderType,
					DI_MEDIA_AUDIO_DECODER_NONE,
					DI_MEDIA_PLAYPUMP_DIVX);
			if(startSettings->stcChannel == NULL)
			{
				PrintError("[%s] PlaybackStcChannel is NULL\n",__func__);
				nDrvErr=DRV_ERR;
				goto done;
			}
		}
#endif
#if defined(CONFIG_CALYPSO)
		if(DRV_CSO_CheckStart())
		{
			startSettings->pidChannel = DRV_CSO_GetPlaypumpVideoPidChannel(
					eVideoDecoderType,
					eMediaVideoCodec,
					(void *)s_stVideo[ulDeviceId].hDecoder);
			if(startSettings->pidChannel == NULL)
			{
				PrintError("[%s] PlaybackPidChannel is NULL\n",__func__);
				nDrvErr=DRV_ERR;
				goto done;
			}

			startSettings->stcChannel = DRV_CSO_GetPlaypumpStcChannelHandle(
					eVideoDecoderType,
					DI_MEDIA_AUDIO_DECODER_NONE,
					DI_MEDIA_PLAYPUMP_DIVX);
			if(startSettings->stcChannel == NULL)
			{
				PrintError("[%s] PlaypumpStcChannel is NULL\n",__func__);
				nDrvErr=DRV_ERR;
				goto done;
			}
		}
#endif

		s_stVideo[ulDeviceId].bIsMedia = TRUE;
#endif
	}
	else
	{
		PrintDebug("[%s] MPEG PLAY\n\r",__FUNCTION__);
		if (VideoCodec==DI_VIDEO_FORMAT_STILL_MPEG2 || VideoCodec==DI_VIDEO_FORMAT_STILL_H264 || VideoCodec==DI_VIDEO_FORMAT_STILL_VC1)
		{
			startSettings->pidChannel = hStillPidChannel;
		}
		else
		{
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO) || defined(CONFIG_PUMP)
			switch(ulDeviceId)
			{
				case 0:
					eVideoDecoderType = DI_MEDIA_VIDEO_DECODER_MAIN;
					break;
				default:
					eVideoDecoderType = DI_MEDIA_VIDEO_DECODER_NONE;
					break;
			}
#endif
#if defined(CONFIG_MEDIA)
			if(DRV_MEDIA_CheckStart())
			{
				if(eVideoDecoderType == DI_MEDIA_VIDEO_DECODER_NONE)
				{
					PrintError("[%s] Invalid eVideoDecoderType\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}
				startSettings->pidChannel = DRV_MEDIA_GetPlaybackVideoPidChannel(
						eVideoDecoderType,
						eMediaVideoCodec,
						(void *)s_stVideo[ulDeviceId].hDecoder);
				if(startSettings->pidChannel == NULL)
				{
					PrintError("[%s] PlaybackPidChannel is NULL\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}

				startSettings->stcChannel = DRV_MEDIA_MP_GetPlaybackStcChannelHandle(
						eVideoDecoderType,
						DI_MEDIA_AUDIO_DECODER_NONE,
						DI_MEDIA_PLAYPUMP_DIVX);
				if(startSettings->stcChannel == NULL)
				{
					PrintError("[%s] PlaybackStcChannel is NULL\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}
				s_stVideo[ulDeviceId].bIsMedia = TRUE;
			}
#endif
#if defined(CONFIG_CALYPSO)
			if(DRV_CSO_CheckStart() && DRV_DEMUX_CheckIfDecoderInputIsMediaPlayer())
			{
				if(eVideoDecoderType == DI_MEDIA_VIDEO_DECODER_NONE)
				{
					PrintError("[%s] Invalid eVideoDecoderType\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}
				startSettings->pidChannel = DRV_CSO_GetPlaypumpVideoPidChannel(
						eVideoDecoderType,
						eMediaVideoCodec,
						(void *)s_stVideo[ulDeviceId].hDecoder);
				if(startSettings->pidChannel == NULL)
				{
					PrintError("[%s] PlaybackPidChannel is NULL\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}

				startSettings->stcChannel = DRV_CSO_GetPlaypumpStcChannelHandle(
						eVideoDecoderType,
						DI_MEDIA_AUDIO_DECODER_NONE,
						DI_MEDIA_PLAYPUMP_DIVX);
				if(startSettings->stcChannel == NULL)
				{
					PrintError("[%s] PlaypumpStcChannel is NULL\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}
				s_stVideo[ulDeviceId].bIsMedia = TRUE;
			}
#endif

#if defined(CONFIG_PUMP)
			if(DRV_PUMP_CheckVideoStart())
			{
				if(eVideoDecoderType == DI_MEDIA_VIDEO_DECODER_NONE)
				{
					PrintError("[%s] Invalid eVideoDecoderType\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}

				startSettings->pidChannel = DRV_PUMP_GetPlaypumpVideoPidChannel(eVideoDecoderType);
				if(startSettings->pidChannel == NULL)
				{
					PrintError("[%s] PlaybackPidChannel is NULL\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}

				startSettings->stcChannel = DRV_PUMP_GetPlaypumpVideoStcChannel();
				s_stVideo[ulDeviceId].bIsMedia = TRUE;
			}
#endif

#if defined(CONFIG_CALYPSO)||defined(CONFIG_MEDIA)
			if(s_stVideo[ulDeviceId].bIsMedia == FALSE)
			{
#endif
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
				if(s_stVideoPrimer.bStartPrimer == FALSE)
				{
					startSettings->pidChannel = DRV_DEMUX_GetVideoPidChannel(ulDeviceId);
				}
				else
				{
					PrintDebug("[%s] s_stVideoPrimer.ulDecodePrimerIndex = %d\n",__func__, s_stVideoPrimer.ulDecodePrimerIndex);
					startSettings->pidChannel = s_stVideoPrimer.decoderSettings[s_stVideoPrimer.ulDecodePrimerIndex].pidChannel;
				}
#else
				startSettings->pidChannel = DRV_DEMUX_GetVideoPidChannel(ulDeviceId);
#endif
#if defined(CONFIG_CALYPSO)||defined(CONFIG_MEDIA)
			}
#endif
#if defined(CONFIG_CALYPSO)||defined(CONFIG_MEDIA)
			if(startSettings->pidChannel && s_stVideo[ulDeviceId].bIsMedia != TRUE)
#endif
			{
				bIsPlayback = DRV_DEMUX_IsPlaybackPidChannel(startSettings->pidChannel, &ePlaybackMode);
			}
		}
		if(startSettings->pidChannel == NULL)
		{
			PrintError("[%s] VideoPidChannel is NULL\n",__func__);
			nDrvErr=DRV_ERR;
			goto done;
		}

#if defined(CONFIG_CALYPSO)||defined(CONFIG_MEDIA)
		if(s_stVideo[ulDeviceId].bIsMedia == FALSE)
		{
#endif
#if defined(SUPPORT_VSYNC)
			startSettings->stcChannel = NULL;
#else
			if(bTsm == TRUE)
			{
				if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
				{
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
					if(s_stVideoPrimer.bStartPrimer == TRUE)
					{
						startSettings->stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(s_stVideoPrimer.ulDecodePrimerIndex);
					}
					else
					{
						startSettings->stcChannel = DRV_DEMUX_GetStcChannelHandle();
					}
#else
					startSettings->stcChannel = DRV_DEMUX_GetStcChannelHandle();
#endif
				}
#if defined(CONFIG_PIP)
				else
				{
					startSettings->stcChannel = DRV_DEMUX_GetPIPStcChannelHandle();
				}
#endif

				if(startSettings->stcChannel == NULL)
				{
					PrintError("[%s] VideoStcChannel is NULL\n",__func__);
					nDrvErr=DRV_ERR;
					goto done;
				}
			}
			else
			{
				startSettings->stcChannel = NULL;
				PrintDebug("[%s] Video are now Vsync mode setting\n",__func__);
			}
#endif

#if defined(CONFIG_CALYPSO)||defined(CONFIG_MEDIA)
		}
#endif
	}
#if defined(CONFIG_SYNC_CHANNEL)
	if ((bTsm == TRUE)
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	&& (s_stVideo[ulDeviceId].bIsMedia == FALSE)
#endif
#if defined(CONFIG_ASTM)
	&& (bIsRfvodMode == FALSE)
#endif
	&& (ulDeviceId == DI_VIDEO_DECODER_MAIN)
	)
	{
		DRV_SYNC_SetSyncChannel(DRV_SYNC_VID,0); /* When video sync channel is set, Second input is meaningless */
	}
#endif

	(void)P_VIDEO_DelayDecoder(ulDeviceId, s_stVideo[ulDeviceId].nUserOffset);

#if (NEXUS_VERSION < 750)
	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
	if(bIsPlayback && ePlaybackMode == DRV_DEMUX_PLAYBACK_EXTERNAL)
	{
		/* external path playback인 경우 video PID channel이 live parser와 연결되기 때문에 NEXUS에서 live mode로 동작함.
		따라서 liveOverride flag on하여 playback mode로 동작하도록 함. */
		decoderSettings.liveOverride = TRUE;
	}
	else
	{
		decoderSettings.liveOverride = FALSE;
	}
	NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
#endif

#if defined(CONFIG_SYNC_RECOVERY)
	NEXUS_Timebase_ResetStatus(NEXUS_Timebase_e0);
#endif

#if defined(CONFIG_ASTM)
	if((bTsm == TRUE) && (bIsPlayback == FALSE) && (ulDeviceId == DI_VIDEO_DECODER_MAIN)
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	&& (s_stVideo[ulDeviceId].bIsMedia == FALSE)
#endif
	&& (bIsRfvodMode == FALSE)
	)
	{
		astmStarted=DRV_SYNC_GetAstmStarted();
		if(astmStarted == TRUE)
			DRV_SYNC_AstmStop();
#if defined(CONFIG_ASTM_SYNC_AUDIO_MASTER)
		DRV_SYNC_SetAstmSettings(DRV_SYNC_AUDMAIN);
#else
		DRV_SYNC_SetAstmSettings(DRV_SYNC_VID);
#endif
		DRV_SYNC_AstmStart();
	}
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	if(s_stVideo[ulDeviceId].bIsMedia == TRUE)
	{
		DRV_SYNC_AstmStop();
	}
#endif
	if(bIsRfvodMode == TRUE)
	{
		DRV_SYNC_AstmStop();
	}
#endif

#if defined(CONFIG_DI20)
#if defined(CONFIG_SCALER_BEFORE_CAPTURE)

#else
	{
			NEXUS_VideoWindowScalerSettings windowScalerSettings;
			HUINT32 ulWindowId = ulDeviceId;
			if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
			{
				NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
				if(windowScalerSettings.bandwidthEquationParams.bias != NEXUS_ScalerCaptureBias_eScalerBeforeCapture)
				{
					windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
					windowScalerSettings.bandwidthEquationParams.delta=1*1000*1000;
					NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
				}
			}
	}
#endif
#endif

#if defined(VIDEODECODER_ERROR_HANDLING_ePROGNOSTIC)
	if((bIsPlayback == FALSE)
#if defined(CONFIG_MEDIA)
	&& (s_stVideo[ulDeviceId].bIsMedia == FALSE)
#endif
	)
	{
		/* Sometimes first picture is distorted or broken
 *		*  So, discard its picture data until next RAP.
 *		*/
		startSettings->errorHandling=NEXUS_VideoDecoderErrorHandling_ePrognostic;
	}
#endif

	if(s_eRequestDecoderMode[ulDeviceId] != s_stVideo[ulDeviceId].eDecoderMode)
	{
		P_VIDEO_SetDecoderMode(ulDeviceId, s_eRequestDecoderMode[ulDeviceId]);
	}

#if defined(CONFIG_TEMP_YSR2000)
	/* in case of playback, 
		if switch audio track, It has a long delay before audio is heard again */
	{
		NEXUS_VideoDecoderOpenSettings openSettings;		
		NEXUS_VideoDecoder_GetOpenSettings(s_stVideo[ulDeviceId].hDecoder, &openSettings);
		if(openSettings.fifoSize)
		{
			NEXUS_VideoDecoderSettings decoderSettings;
			NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
			if(bIsPlayback == TRUE)		
			{			
#if defined(CONFIG_VIDEO_DEC_H265)
				decoderSettings.fifoThreshold = 5*1024*1024;		/* 5 M (28nm default fifoSize)*/
#else
				decoderSettings.fifoThreshold = 15*1024*1024/10; /* 1.5 M (40nm default fifoSize)*/
#endif		
			}
			else
			{
				decoderSettings.fifoThreshold = 0; /* the entire FIFO can fill. */
			}
			NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		}
	}
#endif

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(s_stVideoPrimer.bStartPrimer	== TRUE)
	{
		PrintDebug("[%s] codec=%d  pidChannel=%d\n",__func__,startSettings->codec,(unsigned int)startSettings->pidChannel);
		nexusError = NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(s_stVideoPrimer.hPrimer[s_stVideoPrimer.ulDecodePrimerIndex], s_stVideo[ulDeviceId].hDecoder);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s->NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
			nDrvErr=DRV_ERR;
			goto done;
		}
		else
		{
			PrintDebug("%s->NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
			s_stVideo[ulDeviceId].bDecState=TRUE;
		}
	}
	else
#endif
	{

		PrintDebug("[%s] codec=%d  pidChannel=%d\n",__func__,startSettings->codec,(unsigned int)startSettings->pidChannel);
		nexusError = NEXUS_VideoDecoder_Start(s_stVideo[ulDeviceId].hDecoder , startSettings);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s->NEXUS_VideoDecoder_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
			nDrvErr=DRV_ERR;
			goto done;
		}
		else
		{
			PrintDebug("%s->NEXUS_VideoDecoder_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
			s_stVideo[ulDeviceId].bDecState=TRUE;
		}
	}

	if(VideoCodec < DI_VIDEO_FORMAT_MEDIA_MPEG1)
	{
		if(bIsPlayback)
		{
#if defined(CONFIG_PLAYBACK)
			nDrvErr = DRV_PVR_PLAY_VideoPidChannelSetting(startSettings->pidChannel, s_stVideo[ulDeviceId].hDecoder, startSettings->codec);
			if(nDrvErr != DRV_OK)
			{
				PrintError("[%s] DRV_PVR_PLAY_VideoPidChannelSetting error!\n", __func__);
				goto done;
			}
#endif
		}
	}

	VideoMonMsg.ulParam0 = DI_VIDEO_RUNNING_STATE_UNKNOWN;
	VK_MSG_Send(s_stVideo[ulDeviceId].ulVideoMonMsgQueueId, &VideoMonMsg, sizeof(VideoMonMsg));

	s_stVideo[ulDeviceId].bIsPlayback=bIsPlayback;

#if defined(CONFIG_IFRAME_RANDOM_ACCESS)
	if(s_bRunningrapTimer == FALSE)
	{
		VK_TIMER_EventAfter(2000, P_VIDEO_Check_DecodingStatus, (void *)&ulDeviceId, sizeof(ulDeviceId), (unsigned long *)&s_rapTimerId);
		s_bRunningrapTimer = TRUE;
	}
#endif

#if defined(CONFIG_DEBUG)
	/* Decode Error printing is implemented for SQE request */
	if(s_ulErrorCheckTimerId[ulDeviceId] == VK_TIMERID_NULL)
	{
		VK_MEM_Memset(s_stPrevVideoErrorStatus + ulDeviceId, 0x00, sizeof(stVideoErrorStatus));
		VK_TIMER_EventEvery(VIDEO_ERROR_CHECK_TIME, P_VIDEO_CheckErrorStatus, (void *)&ulDeviceId, sizeof(ulDeviceId), (unsigned long *)&s_ulErrorCheckTimerId[ulDeviceId]);
	}
#endif

done:
	PrintExit();
	DRV_VIDEO_REL_SEMA(ulDeviceId);

	return nDrvErr;
}
#if defined(CONFIG_HDMI_IN)
DRV_Error DRV_VIDEO_SetHdmiVideoInputToOutput(HUINT32 ulDeviceId, NEXUS_HdmiInputHandle hdmiInput)
{
	NEXUS_VideoInput videoInput;
	DRV_Error nDrvErr = DRV_OK;
	HUINT32 ulWindowId = ulDeviceId;

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);
		NEXUS_VideoWindow_Close(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);

		s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] = NEXUS_VideoWindow_Open(s_stVideo[ulDeviceId].hDisplay[HD_DISP], ulWindowId);

		videoInput = NEXUS_HdmiInput_GetVideoConnector(hdmiInput);
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], videoInput);
	}

	return nDrvErr;
}
DRV_Error DRV_VIDEO_UnsetHdmiVideoInputToOutput(HUINT32 ulDeviceId, NEXUS_HdmiInputHandle hdmiInput)
{
	NEXUS_VideoInput videoInput;
	DRV_Error nDrvErr = DRV_OK;
	HUINT32 ulWindowId = ulDeviceId;

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		videoInput = NEXUS_HdmiInput_GetVideoConnector(hdmiInput);
		NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], videoInput);
		NEXUS_VideoWindow_Close(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);

		s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] = NEXUS_VideoWindow_Open(s_stVideo[ulDeviceId].hDisplay[HD_DISP], ulWindowId);
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);
	}
	return nDrvErr;
}
#endif

DRV_Error DRV_VIDEO_Stop(HUINT32 ulDeviceId)
{
	DRV_Error	nDrvErr=DRV_OK;
	DI_VIDEO_RECT_t *stDesiredHdOutRect = &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect;
	DI_VIDEO_RECT_t *stDesiredHdInRect = &s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
	DI_VIDEO_RECT_t *stDesiredSdOutRect = &s_stVideoInfo[ulDeviceId].stDesiredSdOutRect;
	DI_VIDEO_RECT_t *stDesiredSdInRect = &s_stVideoInfo[ulDeviceId].stDesiredSdInRect;
#if defined(CONFIG_DI20)
#if defined(CONFIG_SCALER_BEFORE_CAPTURE)
#else
	NEXUS_VideoWindowScalerSettings windowScalerSettings;
#endif
#endif
#if defined(CONFIG_ASTM)
	HBOOL	bIsAudioValid = FALSE;
	HBOOL bIsRfvodMode = FALSE;
#endif
#if defined(CONFIG_SCALER_BEFORE_CAPTURE)
#else
	HUINT32 ulWindowId = ulDeviceId;
#endif

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].bDecState == FALSE)
	{
		PrintError("video decoder is already stopped!!!\n");
		nDrvErr=DRV_ERR;
		goto done;
	}

	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId);
#if defined(CONFIG_ASTM)
	DRV_RFVOD_GetRfvodMode(&bIsRfvodMode);

	if(ulDeviceId == DI_VIDEO_DECODER_MAIN
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	&& (s_stVideo[ulDeviceId].bIsMedia == FALSE)
#endif
	&& (bIsRfvodMode == FALSE)
	)
	{
		DRV_SYNC_AstmStop();
		DRV_SYNC_UnsetAstmSettings(DRV_SYNC_VID);

		/* If only video decoder is stopped but still working audio decoder, need to re-start ASTM module. */
		bIsAudioValid=DRV_SYNC_GetAstmAudioValid();
		if(bIsAudioValid == TRUE)
		{
			PrintDebug("Astm Restart even if stop in video\n");
			DRV_SYNC_AstmStart();
		}
		else
		{

			PrintDebug("Astm normally is stopped in video\n");
		}
	}
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	if(s_stVideo[ulDeviceId].bIsMedia == TRUE)
	{
		DRV_SYNC_AstmStop();
	}
#endif
	if(bIsRfvodMode == TRUE)
	{
		DRV_SYNC_AstmStop();
	}
#endif

#if defined(CONFIG_DI20)
	s_stVideo[ulDeviceId].e3dFormat = DI_VIDEO_3D_FORMAT_NONE;
	if(s_stVideo[ulDeviceId].e3dDisplayFormat != DI_VIDEO_3D_FORMAT_NONE)
	{
		P_VIDEO_Set3DFormat(ulDeviceId, DI_VIDEO_3D_FORMAT_NONE);
	}
#endif
	NEXUS_VideoDecoder_Stop(s_stVideo[ulDeviceId].hDecoder);

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	if(s_stVideoPrimer.bStartPrimer == TRUE)
	{
		NEXUS_Error nexusError;
		HUINT32 index = s_stVideoPrimer.ulDecodePrimerIndex;

		nexusError = NEXUS_VideoDecoderPrimer_Start(s_stVideoPrimer.hPrimer[index], &s_stVideoPrimer.decoderSettings[index]);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s->NEXUS_VideoDecoderPrimer_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
			nDrvErr=DRV_ERR;
			goto done;
		}
	}
#endif
	NEXUS_VideoDecoder_Flush(s_stVideo[ulDeviceId].hDecoder);

	NEXUS_VideoDecoderSettings decoderSettings;
	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);

#if defined(CONFIG_DI20)
#if defined(CONFIG_SCALER_BEFORE_CAPTURE)

#else
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		NEXUS_VideoWindow_GetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
		if(windowScalerSettings.bandwidthEquationParams.bias != NEXUS_ScalerCaptureBias_eAuto)
		{
			if(decoderSettings.channelChangeMode == NEXUS_VideoDecoder_ChannelChangeMode_eMute)
			{
				windowScalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eAuto;
				windowScalerSettings.bandwidthEquationParams.delta=0;
				NEXUS_VideoWindow_SetScalerSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId],&windowScalerSettings);
			}
		}
	}
#endif
#endif
	decoderSettings.stillContentInterpolationMode = NEXUS_StillContentInterpolationMode_eDefault;
	NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);

#if defined(CONFIG_SYNC_CHANNEL)
	if(ulDeviceId == DI_VIDEO_DECODER_MAIN)
	{
		DRV_SYNC_UnsetSyncChannel(DRV_SYNC_VID);
	}
#endif

	/* If video is stopped, then afd info must be initialized.
	 * That means when every channel changed, AFD info must be notified to MW,
	 * even if previous afd and current afd are the same.
	 */
	s_stVideoInfo[ulDeviceId].afd = 0;
	s_stVideoInfo[ulDeviceId].eAspectRatio = DI_VIDEO_ASPECTRATIO_MAX;
	s_stVideo[ulDeviceId].bFirstPtsPassed = FALSE;
	s_stVideo[ulDeviceId].bDecState=FALSE;
	s_stVideo[ulDeviceId].bDecoding=FALSE;
#if defined(CONFIG_MEDIA) || defined(CONFIG_CALYPSO)
	s_stVideo[ulDeviceId].bIsMedia=FALSE;
#endif

#if defined(CONFIG_CLOSED_CAPTION)
	s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState = DI_VIDEO_CC_DATA_STATE_UNKNOWN;
	P_VIDEO_ResetCcDataChecker(ulDeviceId);
#endif
	
	/* When Video is stopped, local rect info must be clear.
	*/
	VK_MEM_Memset(stDesiredHdOutRect,0xFF,sizeof(DI_VIDEO_RECT_t));
	VK_MEM_Memset(stDesiredHdInRect,0xFF,sizeof(DI_VIDEO_RECT_t));
	VK_MEM_Memset(stDesiredSdOutRect,0xFF,sizeof(DI_VIDEO_RECT_t));
	VK_MEM_Memset(stDesiredSdInRect,0xFF,sizeof(DI_VIDEO_RECT_t));

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
	s_stVideo[ulDeviceId].ulResetTimerId=VK_TIMERID_NULL;
	s_stVideo[ulDeviceId].bDecReset=FALSE;
#endif

#if defined(CONFIG_IFRAME_RANDOM_ACCESS)
	if(s_bRunningrapTimer == TRUE)
	{
		int ret = 0;

		ret = VK_TIMER_Cancel(s_rapTimerId);
		if(ret != VK_OK)
		{
			PrintError("[%s] VK_TIMER_Cancel was failed s_rapTimerId %d \n",__func__, s_rapTimerId);
		}
		else
		{
			s_rapTimerId = 0;
			s_bRunningrapTimer = FALSE;
		}
	}

	if(s_stVideo[ulDeviceId].eDecoderMode == DI_VIDEO_DECODER_MODE_NORMAL && s_stVideo[ulDeviceId].bTreatIFrameAsRap == TRUE)
	{
		NEXUS_VideoDecoderExtendedSettings extSettings;

		NEXUS_VideoDecoder_GetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);
		extSettings.treatIFrameAsRap = false;
		NEXUS_VideoDecoder_SetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);

		s_stVideo[ulDeviceId].bTreatIFrameAsRap = FALSE;
	}
#endif

#if defined(CONFIG_DEBUG)
	if(s_ulErrorCheckTimerId[ulDeviceId] != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(s_ulErrorCheckTimerId[ulDeviceId]);
		s_ulErrorCheckTimerId[ulDeviceId] = VK_TIMERID_NULL;
	}
#endif

done:
	PrintExit();
	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return nDrvErr;

}

DRV_Error DRV_VIDEO_Restart(HUINT32 ulDeviceId, HBOOL bTsm)
{
	DRV_Error	nDrvErr=DRV_OK;

	PrintEnter();

	VK_SEM_Get(s_stVideo[ulDeviceId].ulSemId_DI);

	if(s_stVideo[ulDeviceId].bDecState == FALSE)
	{
		PrintError("[%s] is not activated!\n",__func__);
		nDrvErr = DRV_ERR;
		goto done;
	}

	nDrvErr = DRV_VIDEO_Stop(ulDeviceId);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] Stop failed!\n",__func__);
		nDrvErr = DRV_ERR;
		goto done;
	}

	nDrvErr = DRV_VIDEO_Start(ulDeviceId,s_stVideo[ulDeviceId].eVidCodec, bTsm);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] Start failed!\n",__func__);
		nDrvErr = DRV_ERR;
		goto done;
	}

done:
	VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_DI);

	PrintExit();
	return nDrvErr;
}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
DRV_Error DRV_VIDEO_GetPrimerIndex(HUINT32 ulDeviceId, HUINT32 *pPrimerIndex)
{
	DRV_Error	nDrvErr=DRV_OK;

	HAPPY(ulDeviceId);

	PrintEnter();

	if(s_stVideoPrimer.bStartPrimer == TRUE)
	{
		*pPrimerIndex = s_stVideoPrimer.ulDecodePrimerIndex;
	}
	else
	{
		PrintDebug("[%s] Starting primer is not selected !!! \n",__func__);
		nDrvErr= DI_ERR_NO_ALLOC;
	}

	PrintExit();
	return nDrvErr;
}

HBOOL DRV_VIDEO_IsPrimerStarted(void)
{
	return s_stVideoPrimer.bStartPrimer;
}
#endif

DI_ERR_CODE  DI_VIDEO_GetCapability(HUINT32 *pulNumOfDevice, DI_VIDEO_CAP_t **ppCapInfo)
{
	PrintEnter();

	*pulNumOfDevice	= NUM_VID_DECODER;
	*ppCapInfo = s_stVideoCap;

	PrintDebug("[%s] Currently, the number of the decoder is %d\n",__func__,NUM_VID_DECODER);

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_Start(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
	DRV_Error	nDrvErr=DRV_OK;
	HBOOL		bTsm=s_stVideo[ulDeviceId].bTsm;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(CONFIG_SYNC_RECOVERY)
	DRV_SYNC_SetTsmMode();
	DRV_SYNC_SetVideoTsmMode(bTsm);
#endif
	nDrvErr=DRV_VIDEO_Start(ulDeviceId, VideoCodec, bTsm); /* Only set TSM mode */
	if(nDrvErr == DRV_ERR)
	{
		PrintError("[%s] Video Start Error\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}
	else
	{
		s_stVideo[ulDeviceId].eVidCodec=VideoCodec;
	}
done:
	if(rc == DI_ERR_ERROR)
	{
		nDrvErr=DRV_VIDEO_Stop(ulDeviceId);
		if(nDrvErr == DRV_ERR)
		{
			PrintError("[%s] Video Stop in DI_VIDEO_Start Error\n",__func__);
		}
#if defined(CONFIG_FAST_CHANNEL_CHANGE)
		s_stVideoPrimer.bStartPrimer = FALSE;
#endif
		s_stVideo[ulDeviceId].bTsm = TRUE;
	}
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}

DI_ERR_CODE DI_VIDEO_Stop(HUINT32 ulDeviceId)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
	DRV_Error	nDrvErr=DRV_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	nDrvErr=DRV_VIDEO_Stop(ulDeviceId);
	if(nDrvErr == DRV_ERR)
	{
		PrintError("[%s] Video Stop Error\n",__func__);
		rc=DI_ERR_ERROR;
	}

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	s_stVideoPrimer.bStartPrimer = FALSE;
#endif
	s_stVideo[ulDeviceId].bTsm=TRUE;
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}

DI_ERR_CODE DI_VIDEO_ForceDisableOutput(HUINT32 ulDeviceId, HUINT32 ulDisplayId, HBOOL bMute)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	DRV_Error nDrvErr = DRV_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);
	DI_VIDEO_GET_SEMA_SHOW(ulDeviceId);


	nDrvErr = P_VIDEO_ShowwithWindowID(ulDeviceId, ulDisplayId, !bMute);
	if(nDrvErr == DRV_ERR)
	{
		rc = DI_ERR_ERROR;
	}
	else
	{
		s_stVideo[ulDeviceId].bForcedMute[ulDisplayId] = bMute;
	}

	PrintExit();
	DI_VIDEO_REL_SEMA_SHOW(ulDeviceId);
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_Show(HUINT32 ulDeviceId, HBOOL bShow)
{
#if defined(CONFIG_WORKAROUND_VIDEO_MUTE)
	NEXUS_VideoDecoderSettings decoderSettings;
#endif
	DI_ERR_CODE rc=DI_ERR_OK;
#if defined(CONFIG_WORKAROUND_VIDEO_WINDOW)
	DRV_Error nDrvErr = DRV_OK;
#endif

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d, bshow:%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId, bShow);
#endif

	DI_VIDEO_GET_SEMA_SHOW(ulDeviceId);
	if (bShow==FALSE)
	{
		s_stVideo[ulDeviceId].bNeedToShow=FALSE;

#if defined(CONFIG_WORKAROUND_VIDEO_MUTE)
		NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		decoderSettings.mute=true;
		NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
#endif

#if defined(CONFIG_WORKAROUND_VIDEO_WINDOW)
		nDrvErr = P_VIDEO_Show(ulDeviceId,FALSE);
		if(nDrvErr == DRV_ERR)
		{
			rc = DI_ERR_ERROR;
		}
#endif
	}
	else
	{
#if defined(CONFIG_WORKAROUND_DIRECT_VIDEO_SHOW)	// kevin
#if defined(CONFIG_WORKAROUND_VIDEO_WINDOW)
		nDrvErr = P_VIDEO_Show(ulDeviceId,TRUE);
		if(nDrvErr == DRV_ERR)
		{
			rc = DI_ERR_ERROR;
		}
#endif

#if defined(CONFIG_WORKAROUND_VIDEO_MUTE)
		VK_TASK_Sleep(60);
		NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		decoderSettings.mute=false;
		NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
#endif
#else
		s_stVideo[ulDeviceId].bNeedToShow=TRUE;
#if defined(CONFIG_WORKAROUND_ACTIVE_VIDEO_SHOW)	// kevin
		{
			ST_VIDEO_MSG VideoMonMsg;
			VideoMonMsg.ulParam0 = DI_VIDEO_RUNNING_STATE_DECODING;
			VK_MSG_Send(s_stVideo[ulDeviceId].ulVideoMonMsgQueueId, &VideoMonMsg, sizeof(VideoMonMsg));
		}
#endif
#endif
	}

	PrintExit();
	DI_VIDEO_REL_SEMA_SHOW(ulDeviceId);
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}


/*
 * DI_VIDEO_Pause API
 * This API only takes care of 'Live mode', not 'PVR mode'
 */
DI_ERR_CODE DI_VIDEO_Pause(HUINT32 ulDeviceId, HBOOL bOn)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_VideoDecoderTrickState 	pTrickState;
	NEXUS_Error 					nResult;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d, bOn:%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId, bOn);
#endif

	NEXUS_VideoDecoder_GetTrickState(s_stVideo[ulDeviceId].hDecoder, &pTrickState);
	if(bOn == TRUE)
	{
		pTrickState.rate = 0;
	}
	else
	{
		pTrickState.rate = NEXUS_NORMAL_DECODE_RATE;
	}

	nResult = NEXUS_VideoDecoder_SetTrickState(s_stVideo[ulDeviceId].hDecoder, &pTrickState);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_SetTrickState Failed!\n");
	}
	s_stVideo[ulDeviceId].bFreeze=bOn;

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}


#if defined(CONFIG_DI20)
DI_ERR_CODE DI_VIDEO_Flush(HUINT32 ulDeviceId)
{
	DRV_Error dResult;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	dResult = P_VIDEO_FlushDecoder(ulDeviceId);
	if(dResult != DRV_OK)
	{
		PrintError("DI_VIDEO_Flush failed!\n");
		rc = DI_ERR_ERROR;
	}

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}
#endif

DI_ERR_CODE DI_VIDEO_SetDecoderMode(HUINT32 ulDeviceId, DI_VIDEO_DECODER_MODE eMode)
{
	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);

	if(eMode >= DI_VIDEO_DECODER_MODE_MAX)
	{
		PrintError("Invalid Decoder mode (%d)\n",eMode);
		return DI_ERR_INVALID_PARAM;
	}

	DI_VIDEO_GET_SEMA(ulDeviceId);
	s_eRequestDecoderMode[ulDeviceId] = eMode;
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_StartPrimer(HUINT32 ulDeviceId, DI_VIDEO_PRIMER_t *pPrimerSetting)
{
	NEXUS_Error nexusError;
	DRV_Error dResult;
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_VideoCodec codec;
	NEXUS_PidChannelHandle hVideoPrimerChannelHandle = NULL;
	HUINT32 ulPrimerIndex = 0;

	PrintEnter();

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(pPrimerSetting == NULL)
	{
		PrintError("[%s] ERROR : pPrimerSetting is NULL \n", __FUNCTION__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	ulPrimerIndex = pPrimerSetting->ulPrimerIndex;

	if(s_stVideoPrimer.bOccupied[ulPrimerIndex] == TRUE)
	{
		PrintError("[%s] ERROR : %d Primer is occupied.\n", __FUNCTION__, ulPrimerIndex);
		rc = DI_ERR_ERROR;
		goto done;
	}
	else
	{
		s_stVideoPrimer.bOccupied[ulPrimerIndex] = TRUE;
	}

	s_stVideoPrimer.eFormat[ulPrimerIndex] = pPrimerSetting->eFormat;

	dResult = DRV_DEMUX_GetVideoPrimerPidChannel(ulPrimerIndex, &hVideoPrimerChannelHandle);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_DEMUX_GetVideoPrimerPidChannel failed!\n");
		rc = DI_ERR_ERROR;
		goto done;
	}

	codec =	P_VIDEO_Convert2NexusCodec(s_stVideoPrimer.eFormat[ulPrimerIndex]);

	NEXUS_VideoDecoder_GetDefaultStartSettings(&s_stVideoPrimer.decoderSettings[ulPrimerIndex]);
	s_stVideoPrimer.decoderSettings[ulPrimerIndex].codec = codec;
	s_stVideoPrimer.decoderSettings[ulPrimerIndex].pidChannel = hVideoPrimerChannelHandle;
	s_stVideoPrimer.decoderSettings[ulPrimerIndex].stcChannel = DRV_DEMUX_GetPrimerStcChannelHandle(ulPrimerIndex);

	nexusError = NEXUS_VideoDecoderPrimer_Start(s_stVideoPrimer.hPrimer[ulPrimerIndex], &s_stVideoPrimer.decoderSettings[ulPrimerIndex]);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_VideoDecoderPrimer_Start error\n",__func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
#else
	BSTD_UNUSED(ulDeviceId);
	BSTD_UNUSED(pPrimerSetting);
	BSTD_UNUSED(nexusError);
	BSTD_UNUSED(dResult);
	BSTD_UNUSED(codec);
	BSTD_UNUSED(hVideoPrimerChannelHandle);
	BSTD_UNUSED(ulPrimerIndex);
#endif
	return rc;
}

DI_ERR_CODE DI_VIDEO_StopPrimer(HUINT32 ulDeviceId, DI_VIDEO_PRIMER_t *pPrimerSetting)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	HUINT32 ulPrimerIndex = 0;

	PrintEnter();

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(pPrimerSetting == NULL)
	{
		PrintError("[%s] ERROR : pPrimerSetting is NULL \n", __FUNCTION__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	ulPrimerIndex = pPrimerSetting->ulPrimerIndex;

	if(s_stVideoPrimer.bOccupied[ulPrimerIndex] == FALSE)
	{
		PrintError("[%s] ERROR : %d Primer is not occupied.\n", __FUNCTION__, ulPrimerIndex);
		rc = DI_ERR_ERROR;
		goto done;
	}
	else
	{
		s_stVideoPrimer.bOccupied[ulPrimerIndex] = FALSE;
	}

	NEXUS_VideoDecoderPrimer_Stop(s_stVideoPrimer.hPrimer[ulPrimerIndex]);

done:

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

#else
	BSTD_UNUSED(ulDeviceId);
	BSTD_UNUSED(pPrimerSetting);
	BSTD_UNUSED(ulPrimerIndex);
#endif

	return rc;
}

DI_ERR_CODE DI_VIDEO_SelectPrimer(HUINT32 ulDeviceId, HUINT32 ulPrimerIndex)
{
	DI_ERR_CODE rc=DI_ERR_OK;

#if defined(CONFIG_FAST_CHANNEL_CHANGE)
	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	s_stVideo[ulDeviceId].StartSettings.pidChannel = s_stVideoPrimer.decoderSettings[ulPrimerIndex].pidChannel;

	s_stVideoPrimer.bStartPrimer = TRUE;
	s_stVideoPrimer.ulDecodePrimerIndex = ulPrimerIndex;

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
#else
	BSTD_UNUSED(ulDeviceId);
	BSTD_UNUSED(ulPrimerIndex);
#endif
	return rc;
}

DI_ERR_CODE DI_VIDEO_GetState(HUINT32 ulDeviceId,  DI_VIDEO_RUNNING_STATE *pRunningState)
{
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if (s_stVideo[ulDeviceId].bDecoding==TRUE)
	{
		*pRunningState = DI_VIDEO_RUNNING_STATE_DECODING;
		PrintDebug("[%s] currently, %dth decoder status is decoding.\n",__func__,ulDeviceId);
	}
	else
	{
		*pRunningState = DI_VIDEO_RUNNING_STATE_NODATA;
		PrintDebug("[%s] currently, %dth decoder status is no_data.\n",__func__,ulDeviceId);
	}

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetBlank(HUINT32 ulDeviceId, unsigned int bFlag)
{
	NEXUS_VideoDecoderSettings decoderSettings;
	NEXUS_Error nexusError;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d, bFlag:%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId, bFlag);
#endif

	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);

	if(bFlag == 1)
		decoderSettings.mute = true;
	else
		decoderSettings.mute = false;

	nexusError = NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] %dth decoder can't set blank %s\n",__func__,ulDeviceId,(bFlag==0)?"on":"off");
		rc=DI_ERR_ERROR;
		goto done;
	}
	PrintDebug("[%s] %dth decoder is set of blank %s\n",__func__,ulDeviceId,(bFlag==0)?"on":"off");

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetBackground(HUINT32 ulDeviceId , DI_VIDEO_COLOR_t color)
{
	NEXUS_DisplaySettings displaySettings;
	NEXUS_Error nexusError;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d, color.a/r/g/b:%d/%d/%d/%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId, color.a, color.r, color.g, color.b);
#endif

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
		displaySettings.background=(color.a<<24|color.r<<16|color.g<<8|color.b);
		nexusError = NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
		if (nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Display_SetSettings failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &displaySettings);
		displaySettings.background=(color.a<<24|color.r<<16|color.g<<8|color.b);
		nexusError = NEXUS_Display_SetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &displaySettings);
		if (nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_Display_SetSettings failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}



DI_ERR_CODE DI_VIDEO_SetAspect(HUINT32 ulDeviceId, DI_VIDEO_AspectRatio_t eAspect)
{

	UNREFENCED_PARAM(ulDeviceId);

	NEXUS_DisplaySettings displaySettings;

	if(s_stVideo[0].hDisplay[HD_DISP] != NULL)
	{
		if (eAspect==DI_VIDEO_ASPECTRATIO_16_9)
		{
			NEXUS_Display_GetSettings(s_stVideo[0].hDisplay[HD_DISP], &displaySettings);
			displaySettings.aspectRatio=NEXUS_DisplayAspectRatio_e16x9;
			NEXUS_Display_SetSettings(s_stVideo[0].hDisplay[HD_DISP], &displaySettings);
		}
		else if (eAspect==DI_VIDEO_ASPECTRATIO_4_3)
		{
			NEXUS_Display_GetSettings(s_stVideo[0].hDisplay[HD_DISP], &displaySettings);
			displaySettings.aspectRatio=NEXUS_DisplayAspectRatio_e4x3;
			NEXUS_Display_SetSettings(s_stVideo[0].hDisplay[HD_DISP], &displaySettings);
		}
		else
		{
			//NEXUS_Display_GetSettings(s_stVideo[0].hDisplay[HD_DISP], &displaySettings);
			//displaySettings.aspectRatio=NEXUS_DisplayAspectRatio_eAuto;
			//NEXUS_Display_SetSettings(s_stVideo[0].hDisplay[HD_DISP], &displaySettings);
		}
	}

	return DI_ERR_OK;
}


DI_ERR_CODE DI_VIDEO_SetPlaneAlpha(HUINT32 ulDeviceId, HUINT8 ucAlpha)
{
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_Error nexusError;
	int i=0;
	DI_ERR_CODE rc=DI_ERR_OK;
	HUINT32 ulWindowId = ulDeviceId;


	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s :  deviceid:%d, ucAlpha:%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulDeviceId, ucAlpha);
#endif

	for(i=0 ; i<DI_NUM_DISPLAYS ; i++)
	{
		window=s_stVideo[ulDeviceId].hWindow[i][ulWindowId];
		if(window == NULL)
		{
			PrintError("[%s] window = %d is NULL.\n",__func__,i);
			if(s_stVideo[ulDeviceId].bOpenDisplay[i] == TRUE)
			{
				rc=DI_ERR_ERROR;
				goto done;
			}
			else
				continue;
		}

		NEXUS_VideoWindow_GetSettings(window, &windowSettings);
		windowSettings.alpha=ucAlpha;
#if defined(CONFIG_WORKAROUND_WIN_CLIP)
		windowSettings.vdcapply=true;
#endif
		nexusError=NEXUS_VideoWindow_SetSettings(window,&windowSettings);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] %dth video input can't set of PlaneAlpha value",__func__,i);
			rc=DI_ERR_ERROR;
			goto done;
		}
		PrintDebug("[%s] %dth video input is set of PlaneAlpha value",__func__,i);
		PrintDebug("=0x%x\n",windowSettings.alpha);
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetInOutWindow(HUINT32 ulDeviceId, DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pOutRect, HBOOL *pbTouched, DI_VIDEO_WIN_TYPE eVidWin)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	DRV_Error nDrvErr = DRV_OK;
	NEXUS_VideoDecoderStatus decoderStatus;
	HBOOL bApplySD=FALSE;
	HBOOL bApplyHD=FALSE;
#if (defined(CONFIG_MEDIA) && defined(CONFIG_MEDIA20)) || defined(CONFIG_CALYPSO)
	DI_MEDIA_PlaySpeed_e eMpPbSpeed;
	DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType;
#endif

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);
#if defined(CONFIG_TEMP_HDR4000T)
#else
	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);
	if(decoderStatus.started == false)
	{
		rc = DI_ERR_ERROR;
		PrintError("[%s]  decoderStatus.started == false \n", __func__);
		goto done;
	}
#endif
	if(s_stVideo[ulDeviceId].bDecoding == FALSE)
	{
#if (defined(CONFIG_MEDIA) && defined(CONFIG_MEDIA20)) || defined(CONFIG_CALYPSO)
		if(s_stVideo[ulDeviceId].bIsMedia == TRUE)
		{
			switch(ulDeviceId)
			{
				case 0:
					eVideoDecoderType = DI_MEDIA_VIDEO_DECODER_MAIN;
					break;
				default:
					eVideoDecoderType = DI_MEDIA_VIDEO_DECODER_NONE;
					break;
			}
#if defined(CONFIG_MEDIA20)
			if(DRV_MEDIA_CheckStart())
			{
				nDrvErr=DRV_MEDIA_GetPlaybackSpeed(eVideoDecoderType, &eMpPbSpeed);
				if(nDrvErr == DRV_ERR || eMpPbSpeed != eDI_MEDIA_Speed_Pause)
				{
					rc = DI_ERR_ERROR;
					goto done;
				}
			}
#endif
#if defined(CONFIG_CALYPSO)
			if(DRV_CSO_CheckStart())
			{
				nDrvErr=DRV_CSO_GetSpeed(eVideoDecoderType, &eMpPbSpeed);
				if(nDrvErr == DRV_ERR || eMpPbSpeed != eDI_MEDIA_Speed_Pause)
				{
					rc = DI_ERR_ERROR;
					goto done;
				}
			}
#endif
		}
		else
		{
			rc = DI_ERR_ERROR;
			PrintError("[%s]  decoder is not started !!\n", __func__);
			goto done;
		}
#else
		rc = DI_ERR_ERROR;
		PrintError("[%s]  decoder is not started !!\n", __func__);
		goto done;
#endif
	}


#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d\n", __FUNCTION__, ulDeviceId);
#endif

	if (pInRect==NULL || pOutRect==NULL)
	{
		rc = DI_ERR_ERROR;
		PrintError("[%s] Rect NULL \n", __func__);
		goto done;
	}

//#if defined(DI_VIDEO_SEE_PARAMETERS)
#if 1 //tykang. 20121207. Window size가 비정상적으로 되는 이슈 겈토? 위해 임시 open.
	DI_UART_Print(PARAM_COLOR_ON"%s : deviceid:%d, widnowtype:%d\n", __FUNCTION__, ulDeviceId, eVidWin);
	DI_UART_Print("pInRect (%d,%d,%d,%d)\n", pInRect->ulX, pInRect->ulY, pInRect->ulWidth, pInRect->ulHeight);
	DI_UART_Print("pOutRect (%d,%d,%d,%d)\n"PARAM_COLOR_OFF, pOutRect->ulX, pOutRect->ulY, pOutRect->ulWidth, pOutRect->ulHeight);
#endif

	if(pOutRect->ulWidth != 0 && pOutRect->ulWidth < MIN_SCL_WIDTH
		&& pOutRect->ulHeight != 0 && pOutRect->ulHeight < MIN_SCL_HEIGHT )
	{
		PrintError("[%s] OutRect width and height is not support under %d:%d.\n", __func__, MIN_SCL_WIDTH, MIN_SCL_HEIGHT);
		rc = DI_ERR_ERROR;
		goto done;
	}

	/*
	 * Compare with the previous setting. If same, ignore.
	 * Note : Do not use "memcmp", because it compares even the stuffing bytes that is put for alignment.
	 *
	 * 2009.5.10 dcchung
	 */
	switch (eVidWin)
	{
		case DI_VIDEO_WIN_HD:
			bApplyHD=P_VIDEO_CheckInOutWindow(pInRect, pOutRect, &s_stVideoInfo[ulDeviceId].stDesiredHdInRect, &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect);
			break;
		case DI_VIDEO_WIN_SD:
			bApplySD=P_VIDEO_CheckInOutWindow(pInRect, pOutRect, &s_stVideoInfo[ulDeviceId].stDesiredSdInRect, &s_stVideoInfo[ulDeviceId].stDesiredSdOutRect);
			break;
		case DI_VIDEO_WIN_ALL:
			bApplyHD=P_VIDEO_CheckInOutWindow(pInRect, pOutRect, &s_stVideoInfo[ulDeviceId].stDesiredHdInRect, &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect);
			bApplySD=P_VIDEO_CheckInOutWindow(pInRect, pOutRect, &s_stVideoInfo[ulDeviceId].stDesiredSdInRect, &s_stVideoInfo[ulDeviceId].stDesiredSdOutRect);
			if((bApplyHD == TRUE) || (bApplySD == TRUE))
			{
				bApplyHD = TRUE;
				bApplySD = TRUE;
			}
			else
			{
				bApplyHD = FALSE;
				bApplySD = FALSE;
			}
			break;
		default:
			break;
	}

#if defined(CONIFG_WORKAROUND_BVNSCALER_ERROR)
	if((ulDeviceId == DI_VIDEO_DECODER_MAIN) && (s_stVideo[ulDeviceId].bIsStill == FALSE))
	{
		nDrvErr=DRV_VIDEO_ApplySetInOutWindows(ulDeviceId, FALSE);
		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] Display Apply Manual Failed~! \n",__func__);
		}
		else
		{
			if(s_stVideo[ulDeviceId].ulApplyScalerTimerId != VK_TIMERID_NULL)
			{
				VK_TIMER_Cancel(s_stVideo[ulDeviceId].ulApplyScalerTimerId);
				s_stVideo[ulDeviceId].ulApplyScalerTimerId = VK_TIMERID_NULL;
			}
			VK_TIMER_EventAfter(VIDEO_SCALER_APPLY_TIME, P_VIDEO_ApplyScalerTimer,&ulDeviceId,sizeof(ulDeviceId),(unsigned long *)&s_stVideo[ulDeviceId].ulApplyScalerTimerId);
			PrintDebug("[%s] Display Apply Timer on!\n", __func__);
		}
	}
	else
#endif
	{
		nDrvErr=DRV_VIDEO_ApplySetInOutWindows(ulDeviceId, TRUE);
		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] Display Apply Auto Failed~! \n",__func__);
		}
	}

	if((eVidWin & DI_VIDEO_WIN_HD) && (bApplyHD == TRUE))
	{
#if defined(CONFIG_DI20)
		DI_VIDEO_RECT_t stOutRect;
#endif
		*pbTouched=FALSE;

#if defined(CONFIG_DI20)
		/*
		 * in the case of 3d display format, m/w requested rect differ from display rect.
		 * display rect is half of the requested rect.
		 */
		if(s_stVideo[ulDeviceId].e3dDisplayFormat == DI_VIDEO_3D_FORMAT_SIDEBYSIDE)
		{
			stOutRect.ulX = pOutRect->ulX/2;
			stOutRect.ulY = pOutRect->ulY;
			stOutRect.ulWidth = pOutRect->ulWidth/2;
			stOutRect.ulHeight = pOutRect->ulHeight;
		}
		else if(s_stVideo[ulDeviceId].e3dDisplayFormat == DI_VIDEO_3D_FORMAT_TOPANDBOTTOM)
		{
			stOutRect.ulX = pOutRect->ulX;
			stOutRect.ulY = pOutRect->ulY/2;
			stOutRect.ulWidth = pOutRect->ulWidth;
			stOutRect.ulHeight = pOutRect->ulHeight/2;
		}
		else
		{
			stOutRect = *pOutRect;
		}
#endif
#if defined(CONFIG_DI20)
		nDrvErr = DRV_VIDEO_SetDisplayWindow(ulDeviceId, HD_DISP, pInRect, &stOutRect, NULL, eVidWin);
#else
		nDrvErr = DRV_VIDEO_SetDisplayWindow(ulDeviceId, HD_DISP, pInRect, pOutRect, NULL, eVidWin);
#endif
		if (nDrvErr != DRV_OK)
		{
			PrintError("[%s] DRV_VIDEO_SetDisplayWindow error!\n", __func__);
			rc = DI_ERR_ERROR;
			goto done;
		}

		if (pInRect->ulX==s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulX && pInRect->ulY==s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulY
				&& pInRect->ulWidth==s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulWidth && pInRect->ulHeight==s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulHeight)
		{
			*pbTouched=TRUE;
			PrintDebug("InWindow Touched\n");
		}
		if (pOutRect->ulX==s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulX && pOutRect->ulY==s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulY
				&& pOutRect->ulWidth==s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulWidth && pOutRect->ulHeight==s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulHeight)
		{
			*pbTouched=TRUE;
			PrintDebug("OutWindow Touched\n");
		}

		s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulX = pOutRect->ulX;
		s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulY = pOutRect->ulY;
		s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulWidth = pOutRect->ulWidth;
		s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulHeight = pOutRect->ulHeight;

		s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulX = pInRect->ulX;
		s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulY = pInRect->ulY;
		s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulWidth = pInRect->ulWidth;
		s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulHeight = pInRect->ulHeight;
	}

	if((eVidWin & DI_VIDEO_WIN_SD) && (bApplySD == TRUE))
	{
		*pbTouched=FALSE;

		nDrvErr = DRV_VIDEO_SetDisplayWindow(ulDeviceId, SD_DISP, pInRect, pOutRect, NULL, eVidWin);
		if (nDrvErr != DRV_OK)
		{
			PrintError("[%s] DRV_VIDEO_SetDisplayWindow error!\n", __func__);
			rc = DI_ERR_ERROR;
			goto done;
		}

		if (pInRect->ulX==s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulX && pInRect->ulY==s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulY
				&& pInRect->ulWidth==s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulWidth && pInRect->ulHeight==s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulHeight)
		{
			*pbTouched=TRUE;
			PrintDebug("InWindow Touched\n");
		}
		if (pOutRect->ulX==s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulX && pOutRect->ulY==s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulY
				&& pOutRect->ulWidth==s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulWidth && pOutRect->ulHeight==s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulHeight)
		{
			*pbTouched=TRUE;
			PrintDebug("OutWindow Touched\n");
		}

		s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulX = pOutRect->ulX;
		s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulY = pOutRect->ulY;
		s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulWidth = pOutRect->ulWidth;
		s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulHeight = pOutRect->ulHeight;

		s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulX = pInRect->ulX;
		s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulY = pInRect->ulY;
		s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulWidth = pInRect->ulWidth;
		s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulHeight = pInRect->ulHeight;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_ApplySetInOutWindows(HBOOL bApply)
{
	DRV_Error	nDrvErr=DRV_OK;
	DI_ERR_CODE rc = DI_ERR_OK;
	HUINT32 ulDeviceId = DEF_VID_INSTANCE;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	nDrvErr=DRV_VIDEO_ApplySetInOutWindows(ulDeviceId,bApply);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] Display Apply error! (bApply = %d)\n",__func__,bApply);
		rc=DI_ERR_ERROR;
	}

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDOutput)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	DI_ERR_CODE result;

	PrintEnter();

#ifdef CONFIG_HD_RESOLUTION_CHECK
	HD_Format_Desired = P_VIDEO_Convert2NexusVideoFormat(eHDOutput);
#endif
	result = DRV_HDMI_SetDisplayStandard(eHDOutput);
	if(result) {
		PrintError("ERROR %s,%d\n", __FUNCTION__, __LINE__);
		goto done;
	}

	ret = DI_ERR_OK;

done:
	PrintExit();
	return ret;
}

DI_ERR_CODE DI_VIDEO_SetOutputSettings (DI_VIDEO_AOUT_MODE_t output)
{
	NEXUS_DisplayHandle dispHandle_HD, dispHandle_SD;
#if NEXUS_NUM_SVIDEO_OUTPUTS >= 1
	NEXUS_SvideoOutputHandle hSvideoOutput;
#endif
	NEXUS_CompositeOutputHandle hCompositeOutput;
	NEXUS_VideoOutput hVideoOutput;
	NEXUS_Error nexusError;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	NEXUS_ComponentOutputHandle hComponentOutput;
	NEXUS_ComponentOutputSettings ComponentOutputSettings;
	HBOOL	bComponentResetRequired = FALSE;
#endif
	HUINT32 modechk;
	DI_VIDEO_AOUT_MODE_t tempOutput;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	DI_ERR_CODE rc=DI_ERR_OK;
	DRV_Error nDrvErr = DRV_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : cvbs:%d, rgb:%d, svhs:%d, ypbpr:%d, ypbpr_down:%d\n"PARAM_COLOR_OFF, __FUNCTION__, output.bCVBS, output.bRGB, output.bSVHS, output.bYPbPr, output.bYPbPr_Downscale);
#endif

	tempOutput.bRGB = tempOutput.bYPbPr = tempOutput.bYPbPr_Downscale = 0;

	modechk=0;
	modechk += output.bRGB;
	modechk += output.bYPbPr;
	modechk += output.bYPbPr_Downscale;
	if(modechk >= 2)
	{
		PrintError("[%s] RGB,YPbPr,YPbPr-Downscale can't be output at the same time\n",__func__);
		rc=DI_ERR_INVALID_PARAM;
		goto done;
	}

	nDrvErr=DRV_VIDEO_GetDisplayHandle(ulDeviceId, &dispHandle_HD, &dispHandle_SD);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] Display Handles are not available, need to initialize display module\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	//Composite Output Settings
	hCompositeOutput = P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPOSITE);
	if(hCompositeOutput != NULL && dispHandle_SD != NULL)
	{
		if(s_currentOutputStatus.bCVBS != output.bCVBS)
		{
			hVideoOutput = NEXUS_CompositeOutput_GetConnector(hCompositeOutput);
			if(output.bCVBS==1)
			{
				nexusError = NEXUS_Display_AddOutput(dispHandle_SD, hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] composite port can't be added\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
			else
			{
				nexusError = NEXUS_Display_RemoveOutput(dispHandle_SD, hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] composite port can't be removed\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 2
#else
			s_currentOutputStatus.bCVBS = output.bCVBS;
#endif
			PrintDebug("[%s] composite port is %s\n",__func__,(output.bCVBS==1)?"added":"removed");
		}
	}
#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 2
	hCompositeOutput = P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPOSITE2);
	if(hCompositeOutput != NULL && dispHandle_SD != NULL)
	{
		if(s_currentOutputStatus.bCVBS != output.bCVBS)
		{
			hVideoOutput = NEXUS_CompositeOutput_GetConnector(hCompositeOutput);
			if(output.bCVBS==1)
			{
				nexusError = NEXUS_Display_AddOutput(dispHandle_SD, hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] composite port can't be added\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
			else
			{
				nexusError = NEXUS_Display_RemoveOutput(dispHandle_SD, hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] composite port can't be removed\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
			s_currentOutputStatus.bCVBS = output.bCVBS;
			PrintDebug("[%s] composite port is %s\n",__func__,(output.bCVBS==1)?"added":"removed");
		}
	}
#endif

#if NEXUS_NUM_SVIDEO_OUTPUTS >= 1
	//S-Video Output Settings
	hSvideoOutput = P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_SVIDEO);
	if(hSvideoOutput != NULL && dispHandle_SD != NULL)
	{
		if(s_currentOutputStatus.bSVHS != output.bSVHS)
		{
			hVideoOutput = NEXUS_SvideoOutput_GetConnector(hSvideoOutput);
			if(output.bSVHS==1)
			{
				nexusError = NEXUS_Display_AddOutput(dispHandle_SD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] svideo port can't be added\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
			else
			{
				nexusError = NEXUS_Display_RemoveOutput(dispHandle_SD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] svideo port can't be removed\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
			s_currentOutputStatus.bSVHS = output.bSVHS;
			PrintDebug("[%s] composite port is %s\n",__func__,(output.bCVBS==1)?"added":"removed");
		}
	}
#endif
	//Component Output Settings
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	if(IS_COMPONENT_SETTING_CHANGED(&s_currentOutputStatus, &output))
	{
		hComponentOutput = P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE_COMPONENT);
		if(hComponentOutput != NULL)
		{
			hVideoOutput = NEXUS_ComponentOutput_GetConnector(hComponentOutput);

			//Disconnect from output if required.
			if(s_currentOutputStatus.bRGB && dispHandle_SD != NULL)
			{
				nexusError = NEXUS_Display_RemoveOutput(dispHandle_SD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] SD RGB port can't be removed\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				tempOutput.bRGB = s_currentOutputStatus.bRGB;
				s_currentOutputStatus.bRGB = 0;
				PrintDebug("[%s] SD RGB port is removed\n",__func__);
			}
			if(s_currentOutputStatus.bYPbPr && dispHandle_HD != NULL)
			{
				nexusError = NEXUS_Display_RemoveOutput(dispHandle_HD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] HD YPbPr port can't be removed\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				tempOutput.bYPbPr = s_currentOutputStatus.bYPbPr;
				s_currentOutputStatus.bYPbPr = 0;
				PrintDebug("[%s] HD YPbPr port is removed\n",__func__);
			}
			if(s_currentOutputStatus.bYPbPr_Downscale && dispHandle_SD != NULL)
			{
				nexusError = NEXUS_Display_RemoveOutput(dispHandle_SD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] SD YPbPr_Downscale port can't be removed\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				tempOutput.bYPbPr_Downscale = s_currentOutputStatus.bYPbPr_Downscale;
				s_currentOutputStatus.bYPbPr_Downscale = 0;
				PrintDebug("[%s] SD YPbPr_Downscale port is removed\n",__func__);
			}

			NEXUS_ComponentOutput_GetSettings(hComponentOutput, &ComponentOutputSettings);
			if (output.bRGB == 1 && ComponentOutputSettings.type!=NEXUS_ComponentOutputType_eRGB)
			{
				ComponentOutputSettings.type = NEXUS_ComponentOutputType_eRGB;
				/**
				 * For RGB no sync on Green analog video line,
				 * must set 'dacHSync' to Video dac None
				 * 'NoSync' to be true
				 */
				ComponentOutputSettings.dacs.RGB.dacHSync = NEXUS_VideoDac_eNone;
				ComponentOutputSettings.dacs.RGB.noSync = true;
				bComponentResetRequired=TRUE;
			}

			else if ((output.bYPbPr ==1 || output.bYPbPr_Downscale == 1) && ComponentOutputSettings.type!=NEXUS_ComponentOutputType_eYPrPb)
			{
				ComponentOutputSettings.type = NEXUS_ComponentOutputType_eYPrPb;
				bComponentResetRequired=TRUE;
			}

			if (bComponentResetRequired==TRUE)
			{
				nexusError = NEXUS_ComponentOutput_SetSettings(hComponentOutput, &ComponentOutputSettings);
				if(nexusError != NEXUS_SUCCESS)
				{
					s_currentOutputStatus.bRGB = tempOutput.bRGB;
					s_currentOutputStatus.bYPbPr = tempOutput.bYPbPr;
					s_currentOutputStatus.bYPbPr_Downscale = tempOutput.bYPbPr_Downscale;
					PrintError("[%s] Component port can't be set\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				PrintDebug("[%s] Component port is set\n",__func__);
			}

			if(output.bRGB==1 && dispHandle_SD != NULL)
			{
				nexusError = NEXUS_Display_AddOutput(dispHandle_SD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] SD RGB port can't be added\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				s_currentOutputStatus.bRGB = output.bRGB;
				PrintDebug("[%s] SD RGB port is added\n",__func__);
			}
			if(output.bYPbPr==1 && dispHandle_HD != NULL)
			{
				nexusError = NEXUS_Display_AddOutput(dispHandle_HD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] HD YPbPr port can't be added\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				s_currentOutputStatus.bYPbPr = output.bYPbPr;
				PrintDebug("[%s] HD YPbPr port is added\n",__func__);
			}
			if(output.bYPbPr_Downscale==1 && dispHandle_SD != NULL)
			{
				nexusError = NEXUS_Display_AddOutput(dispHandle_SD,hVideoOutput);
				if(nexusError != NEXUS_SUCCESS)
				{
					PrintError("[%s] SD YPbPr downscale port can't be added\n",__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
				s_currentOutputStatus.bYPbPr_Downscale = output.bYPbPr_Downscale;
				PrintDebug("[%s] SD YPbPr downscale port is added\n",__func__);
			}
#if defined(CONFIG_SDHD_FILTER_6346)
			P_VIDEO_SetGpioFilter(ulDeviceId);
#endif
		}
	}
#endif
done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetWSS(HBOOL bOn, DI_VIDEO_WSS_SETTINGS_t stSettings)
{
	NEXUS_DisplayHandle display;
	NEXUS_DisplayVbiSettings vbiSettings;
	NEXUS_Error nResult;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	HUINT16 uiWss=0;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : Afd:%02X, CpBit:%02X, GeBit:%02X\n"PARAM_COLOR_OFF, __FUNCTION__, stSettings.eAfd, stSettings.eCpbit, stSettings.eGebit);
#endif

	switch(stSettings.eAfd)
	{
		case DI_VIDEO_WSS_AFD_FULL_FORMAT_4_3:
			uiWss = SetWSS_FULL_FORMAT_4_3;
			break;
		case DI_VIDEO_WSS_AFD_BOX_14_9_CENTRE:
			uiWss = SetWSS_BOX_14_9_CENTRE;
			break;
		case DI_VIDEO_WSS_AFD_BOX_16_9_CENTRE:
			uiWss = SetWSS_BOX_16_9_CENTRE;
			break;
		case DI_VIDEO_WSS_AFD_FULL_FORMAT_4_3_SNP:
			uiWss = SetWSS_FULL_FORMAT_4_3_SNP;
			break;
		case DI_VIDEO_WSS_AFD_FULL_FORMAT_16_9_ANAMORPHIC:
			uiWss = SetWSS_FULL_FORMAT_16_9_ANAMORPHIC;
			break;
		default:
			uiWss=SetWSS_NONE;
			vbiSettings.wssEnabled = false;
			break;
	}

	/* Write Copyright and Generation 12,13 bit */
	uiWss &= 0xCFFF;
	uiWss |= (stSettings.eCpbit << 12);
	uiWss |= (stSettings.eGebit << 13);

	display = s_stVideo[ulDeviceId].hDisplay[SD_DISP]; //SD display handle
	if(display != NULL)
	{
		NEXUS_Display_GetVbiSettings(display, &vbiSettings);
		vbiSettings.wssEnabled = bOn;
		nResult = NEXUS_Display_SetVbiSettings(display, &vbiSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			rc=DI_ERR_ERROR;
			goto done;
		}

		if (vbiSettings.wssEnabled==true)
		{
			nResult = NEXUS_Display_SetWss(display, uiWss);
			if(nResult != NEXUS_SUCCESS)
			{
				rc=DI_ERR_ERROR;
				goto done;
			}
		}

		/* If writing wss data into vbi line is sucess,
		 * then update current settings
		*/
		s_stVideoInfo[ulDeviceId].stWssSettings.eAfd = stSettings.eAfd;
		s_stVideoInfo[ulDeviceId].stWssSettings.eCpbit = stSettings.eCpbit;
		s_stVideoInfo[ulDeviceId].stWssSettings.eGebit = stSettings.eGebit;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_GetWSS(DI_VIDEO_WSS_SETTINGS_t *pstSettings)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	DI_VIDEO_WSS_SETTINGS_t *cpstSettings = pstSettings;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(pstSettings == NULL)
	{
		PrintError("[%s] pstSettings == NULL\n",__func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	cpstSettings->eAfd = s_stVideoInfo[ulDeviceId].stWssSettings.eAfd;
	cpstSettings->eCpbit = s_stVideoInfo[ulDeviceId].stWssSettings.eCpbit;
	cpstSettings->eGebit = s_stVideoInfo[ulDeviceId].stWssSettings.eGebit;

done:

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}




void DRV_VIDEO_GetCurrentOutputStatus(void* pParam)
{
	DI_VIDEO_AOUT_MODE_t *pStatus=pParam;

	pStatus->bCVBS = s_currentOutputStatus.bCVBS;
	pStatus->bRGB = s_currentOutputStatus.bRGB;
	pStatus->bSVHS = s_currentOutputStatus.bSVHS;
	pStatus->bYPbPr = s_currentOutputStatus.bYPbPr;
	pStatus->bYPbPr_Downscale = s_currentOutputStatus.bYPbPr_Downscale;
}


void DRV_VIDEO_GetVideoFormatStr(HUINT32 ulIndex, char* pStr)
{
	int len=0;

	/*
	 * "pStr" should be an array that is big enough.
	 * dcchung 2009.5.12
	 */

	if(strVideoFormat[ulIndex] == NULL)
	{
		ulIndex = NEXUS_VideoFormat_eUnknown;
	}

	if (ulIndex<NEXUS_VideoFormat_eMax)
	{
		len = VK_strlen(strVideoFormat[ulIndex]);
		if(len < 99)
		{
			VK_strncpy(pStr, strVideoFormat[ulIndex],len+1);
		}
	}
	else
	{
		VK_strncpy(pStr, "Undefined VideoFomat", 21);
	}
}


void DRV_VIDEO_GetVideoCodecStr(HUINT32 ulIndex, char* pStr)
{
	int len=0;

	/*
	 * "pStr" should be an array that is big enough.
	 * dcchung 2009.5.12
	 */

	if(strCodec[ulIndex] == NULL)
	{
		ulIndex = NEXUS_VideoCodec_eUnknown;
	}

	if (ulIndex<NEXUS_VideoCodec_eMax)
	{
		len = VK_strlen(strCodec[ulIndex]);
		if(len < 99)
			VK_strncpy(pStr, strCodec[ulIndex], len+1);
	}
	else
		VK_strncpy(pStr, "Undefined VideoCodec",21);
}

void DRV_VIDEO_SetDnr(void)
{
	NEXUS_VideoWindowDnrSettings stSettings;
	if(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
		PrintError("mnr.mode : %d\n", stSettings.mnr.mode);
		PrintError("mnr.level : %d\n", stSettings.mnr.level);
		PrintError("bnr.mode : %d\n", stSettings.bnr.mode);
		PrintError("bnr.level : %d\n", stSettings.bnr.level);
		PrintError("dcr.mode : %d\n", stSettings.dcr.mode);
		PrintError("dcr.level : %d\n", stSettings.dcr.level);
		PrintError("stSettings.qp : %d\n", stSettings.qp);

		stSettings.mnr.level=200;
		stSettings.mnr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		stSettings.bnr.level=200;
		stSettings.bnr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		stSettings.dcr.level=90;
		stSettings.dcr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		PrintError("return from dnr setting : %d \n", NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings));

		PrintError("bnr.mode : %d\n", stSettings.bnr.mode);
		PrintError("bnr.level : %d\n", stSettings.bnr.level);
	}
}

void DRV_VIDEO_SetMnr(int iMnr)
{
	NEXUS_VideoWindowDnrSettings stSettings;
	if(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW], &stSettings);
		stSettings.mnr.level=iMnr;
		stSettings.mnr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW], &stSettings);
	}

	if(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
		stSettings.mnr.level=iMnr;
		stSettings.mnr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
	}
	PrintError("mnr setting\n");
}

void DRV_VIDEO_SetBnr(int iBnr)
{
	NEXUS_VideoWindowDnrSettings stSettings;
	if(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW], &stSettings);
		stSettings.bnr.level=iBnr;
		stSettings.bnr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW], &stSettings);
	}

	if(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
		stSettings.bnr.level=iBnr;
		stSettings.bnr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
	}
	PrintError("bnr setting\n");
}


void DRV_VIDEO_SetDcr(int iDcr)
{
	NEXUS_VideoWindowDnrSettings stSettings;
	if(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW], &stSettings);
		stSettings.dcr.level=iDcr;
		stSettings.dcr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[SD_DISP][MAIN_WINDOW], &stSettings);
	}

	if(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW] != NULL)
	{
		NEXUS_VideoWindow_GetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
		stSettings.dcr.level=iDcr;
		stSettings.dcr.mode=NEXUS_VideoWindowFilterMode_eEnable;
		NEXUS_VideoWindow_SetDnrSettings(s_stVideo[0].hWindow[HD_DISP][MAIN_WINDOW], &stSettings);
	}
	PrintError("dcr setting\n");
}

static HBOOL s_bBufferTimeoutTimer = FALSE;

void DRV_VIDEO_ClearTimeoutTimer(void)
{
	s_bBufferTimeoutTimer = TRUE;
}



/*
 * Currently, NEXUS Teletext 2nd encoder is not working now.
 * Check it, when NEXUS phase would be updated (checked phase 4.0)
 */
DI_ERR_CODE DI_VIDEO_SetVBITeletext(HBOOL bOn)
{
	NEXUS_Error nexusError;
	NEXUS_DisplayVbiSettings vbiSettings;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();
#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : bOn:%d\n"PARAM_COLOR_OFF, __FUNCTION__, bOn);
#endif
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
	{
		NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&vbiSettings);
		vbiSettings.teletextEnabled = bOn;
		vbiSettings.teletextRouting = bOn;
		nexusError=NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&vbiSettings);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] VBI Teletext setting error \n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}

		s_stVideo[ulDeviceId].bVbiTeletext=bOn;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

#if defined(CONFIG_CLOSED_CAPTION)
DI_ERR_CODE DI_VIDEO_SetVBIClosedCaption(HBOOL bOn)
{
	NEXUS_Error nexusError;
	NEXUS_DisplayVbiSettings vbiSettings;
	HUINT32 ulDeviceId = DEF_VID_INSTANCE;
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();
#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : bOn:%d\n"PARAM_COLOR_OFF, __FUNCTION__, bOn);
#endif
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
	{
		NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&vbiSettings);

		/* Tell Display to encode incoming CC to VEC */
		vbiSettings.closedCaptionEnabled = bOn;		
		//vbiSettings.closedCaptionRouting = 0;
		nexusError=NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP],&vbiSettings);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] VBI ClosedCaption setting error \n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}

		s_stVideo[ulDeviceId].bVbiClosedCaption=bOn;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}
#endif

DI_ERR_CODE DI_VIDEO_SetMacroVision (DI_VIDEO_MV_MODE eMode)
{
	NEXUS_DisplayVbiSettings nDisplayVbiSettings;
	NEXUS_DisplayMacrovisionType type;
	NEXUS_DisplayMacrovisionTables macrovision_tables;
	NEXUS_Error nexus_rc;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	DI_ERR_CODE rc=DI_ERR_OK;
	DI_VIDEO_AOUT_MODE_t OutputStatus;
	NEXUS_DisplaySettings displaySettings;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : Mode:%d\n"PARAM_COLOR_OFF, __FUNCTION__, eMode);
#endif
	VK_MEM_Memset(macrovision_tables.cpsTable,0x00,33);
	VK_MEM_Memset(macrovision_tables.cpcTable,0x00,2);

	switch (eMode)
	{
		case DI_VIDEO_MV_NONE:
			type = NEXUS_DisplayMacrovisionType_eNone;
			break;
		case DI_VIDEO_MV_AGC_ONLY:
			type = NEXUS_DisplayMacrovisionType_eAgcOnly;
			break;
		case DI_VIDEO_MV_AGC_2LINES:
			type = NEXUS_DisplayMacrovisionType_eAgc2Lines;
			break;
		case DI_VIDEO_MV_AGC_4LINES:
			type = NEXUS_DisplayMacrovisionType_eAgc4Lines;
			break;
		case DI_VIDEO_MV_STANDARD_PAL:
			type = NEXUS_DisplayMacrovisionType_eCustom;
			/* DMRTest Standard 625-50 Sept 22 w-filed.doc 문서 참조 */
			/* CPC table 0~1 */
			macrovision_tables.cpcTable[0] = 0x3;
			macrovision_tables.cpcTable[1] = 0x6;
			/* CPS table 0~32 */
			/* CPS[5] : Add 0x4 to turn on Macrovision encoding on RGB outputs (e.g. CPS[5:4] = 0x25 becomes 0x65 */
			macrovision_tables.cpsTable[0] = 0x5;
			macrovision_tables.cpsTable[1] = 0xA;
			macrovision_tables.cpsTable[2] = 0x4;
			macrovision_tables.cpsTable[3] = 0x5;
			macrovision_tables.cpsTable[4] = 0x5;
			macrovision_tables.cpsTable[5] = 0x2;
			macrovision_tables.cpsTable[6] = 0x2;
			macrovision_tables.cpsTable[7] = 0xA;
			macrovision_tables.cpsTable[8] = 0x8;
			macrovision_tables.cpsTable[9] = 0x7;

			macrovision_tables.cpsTable[10] = 0x3;
			macrovision_tables.cpsTable[11] = 0xD;
			macrovision_tables.cpsTable[12] = 0x5;
			macrovision_tables.cpsTable[13] = 0x0;
			macrovision_tables.cpsTable[14] = 0x1;
			macrovision_tables.cpsTable[15] = 0xF;
			macrovision_tables.cpsTable[16] = 0xF;
			macrovision_tables.cpsTable[17] = 0x0;
			macrovision_tables.cpsTable[18] = 0x1;
			macrovision_tables.cpsTable[19] = 0x5;

			macrovision_tables.cpsTable[20] = 0x4;
			macrovision_tables.cpsTable[21] = 0xF;
			macrovision_tables.cpsTable[22] = 0xE;
			macrovision_tables.cpsTable[23] = 0x7;
			macrovision_tables.cpsTable[24] = 0xE;
			macrovision_tables.cpsTable[25] = 0x6;
			macrovision_tables.cpsTable[26] = 0x0;
			macrovision_tables.cpsTable[27] = 0x4;
			macrovision_tables.cpsTable[28] = 0x0;
			macrovision_tables.cpsTable[29] = 0x2;

			macrovision_tables.cpsTable[30] = 0x7;
			macrovision_tables.cpsTable[31] = 0x5;
			macrovision_tables.cpsTable[32] = 0x5;
			break;
		case DI_VIDEO_MV_STANDARD_NTSC:
			type = NEXUS_DisplayMacrovisionType_eCustom;
			/* DMRTest Standard NTSC Sept 22 w-filed.doc 문서 참조 */
			/* CPC table 0~1 */
			macrovision_tables.cpcTable[0] = 0x3;
			macrovision_tables.cpcTable[1] = 0xE;
			/* CPS table 0~32 */
			macrovision_tables.cpsTable[0] = 0xD;
			macrovision_tables.cpsTable[1] = 0xD;
			macrovision_tables.cpsTable[2] = 0x2;
			macrovision_tables.cpsTable[3] = 0x3;
			macrovision_tables.cpsTable[4] = 0x2;
			macrovision_tables.cpsTable[5] = 0xD;
			macrovision_tables.cpsTable[6] = 0x1;
			macrovision_tables.cpsTable[7] = 0x3;
			macrovision_tables.cpsTable[8] = 0xC;
			macrovision_tables.cpsTable[9] = 0x6;

			macrovision_tables.cpsTable[10] = 0xD;
			macrovision_tables.cpsTable[11] = 0xB;
			macrovision_tables.cpsTable[12] = 0x9;
			macrovision_tables.cpsTable[13] = 0x0;
			macrovision_tables.cpsTable[14] = 0x3;
			macrovision_tables.cpsTable[15] = 0xF;
			macrovision_tables.cpsTable[16] = 0xC;
			macrovision_tables.cpsTable[17] = 0x0;
			macrovision_tables.cpsTable[18] = 0x0;
			macrovision_tables.cpsTable[19] = 0x0;

			macrovision_tables.cpsTable[20] = 0x0;
			macrovision_tables.cpsTable[21] = 0x0;
			macrovision_tables.cpsTable[22] = 0xF;
			macrovision_tables.cpsTable[23] = 0x7;
			macrovision_tables.cpsTable[24] = 0xF;
			macrovision_tables.cpsTable[25] = 0x6;
			macrovision_tables.cpsTable[26] = 0x0;
			macrovision_tables.cpsTable[27] = 0xD;
			macrovision_tables.cpsTable[28] = 0x0;
			macrovision_tables.cpsTable[29] = 0x2;

			macrovision_tables.cpsTable[30] = 0xC;
			macrovision_tables.cpsTable[31] = 0xF;
			macrovision_tables.cpsTable[32] = 0xF;
			break;

		case DI_VIDEO_MV_AGC_ONLY_RGB:
			type = NEXUS_DisplayMacrovisionType_eAgcOnlyRgb;
			break;
		case DI_VIDEO_MV_TEST1:
			type = NEXUS_DisplayMacrovisionType_eTest01;
			break;
		case DI_VIDEO_MV_TEST2:
			type = NEXUS_DisplayMacrovisionType_eTest02;
			break;
		default:
			type = NEXUS_DisplayMacrovisionType_eNone;
			break;
	}

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
	{
		// apply to SD display. Always SD display is MacroVision Available Format~!
		nexus_rc = NEXUS_Display_SetMacrovision(s_stVideo[ulDeviceId].hDisplay[SD_DISP], type, &macrovision_tables);
		if (nexus_rc)
		{
			PrintDebug("[%d]%s err from NEXUS_Display_SetMacrovision",__LINE__,__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}

		NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &nDisplayVbiSettings);
		nDisplayVbiSettings.macrovisionEnabled =  (type==NEXUS_DisplayMacrovisionType_eNone)? 0:1;
		nexus_rc = NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &nDisplayVbiSettings);
		if (nexus_rc)
		{
			PrintDebug("[%d]%s err from NEXUS_Display_SetVbiSettings",__LINE__,__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		// get video DAC setting for knowing HD Display Output
		DRV_VIDEO_GetCurrentOutputStatus(&OutputStatus);

		// And get HD video display format for knowing whether HD display format is MacroVision Available or not.
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);

		if (OutputStatus.bYPbPr && (displaySettings.format<=NEXUS_VideoFormat_e576p))
		{
			// apply to HD display,
			// if exist analog HD output (YPbPr) & Macrovision available HD display format
			nexus_rc = NEXUS_Display_SetMacrovision(s_stVideo[ulDeviceId].hDisplay[HD_DISP], type, &macrovision_tables);
			if (nexus_rc)
			{
				PrintDebug("[%d]%s err from NEXUS_Display_SetMacrovision",__LINE__,__func__);
				rc=DI_ERR_ERROR;
				goto done;
			}

			NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &nDisplayVbiSettings);
			nDisplayVbiSettings.macrovisionEnabled =  (type==NEXUS_DisplayMacrovisionType_eNone)? 0:1;
			nexus_rc = NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &nDisplayVbiSettings);
			if (nexus_rc)
			{
				PrintDebug("[%d]%s err from NEXUS_Display_SetVbiSettings",__LINE__,__func__);
				rc=DI_ERR_ERROR;
				goto done;
			}
		}
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}




DI_ERR_CODE DI_VIDEO_SetCGMS(HBOOL bOn, DI_VIDEO_CGMS_SETTINGS tCgmsSettings , DI_VIDEO_WIN_TYPE eVidWin)
{

	NEXUS_DisplayVbiSettings displayVbiSettings;
	NEXUS_Error nexus_rc;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	HUINT32 ulValue = 0;
	HUINT32 ulARValue = 0;
	HUINT32 ulCgmsAValue = 0;
	HUINT32 ulApsTriggerValue = 0;

	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if ( (eVidWin!= DI_VIDEO_WIN_HD) &&  (eVidWin!= DI_VIDEO_WIN_SD) &&  (eVidWin!= DI_VIDEO_WIN_ALL) )
	{
		PrintError("[%d]%s Err : eVidWin %d\n",__LINE__,__func__,eVidWin);
		rc=DI_ERR_INVALID_PARAM;
		goto done;
	}
	if( bOn == TRUE )
	{
		if( tCgmsSettings.eType == DI_VIDEO_CGMS_ARIB )
		{
			if ( tCgmsSettings.uTypeStatus.tArib.eCgmsA >= DI_VIDEO_CGMS_END || tCgmsSettings.uTypeStatus.tArib.eApsTrigger >= DI_VIDEO_CGMS_PSP_END )
			{
				rc=DI_ERR_INVALID_PARAM;
				goto done;
			}

			/* Aspect Ratio */
			switch( tCgmsSettings.uTypeStatus.tArib.eAspectRatio)
			{
				case DI_VIDEO_ASPECTRATIO_16_9:
					ulARValue = 1;
					break;
				case DI_VIDEO_ASPECTRATIO_4_3:
					ulARValue = 0;
					break;
				case DI_VIDEO_ASPECTRATIO_4_3_LETTER:
					ulARValue = 2;
					break;
				default:
					rc=DI_ERR_INVALID_PARAM;
					goto done;

			}

			/* word 2: CGMS-A bit 6 and 7 */
			ulCgmsAValue = tCgmsSettings.uTypeStatus.tArib.eCgmsA;
			/* APS trigger bit 8 and 9*/
			ulApsTriggerValue = tCgmsSettings.uTypeStatus.tArib.eApsTrigger;

			ulValue = ( ulARValue & 0x3 );
			ulValue |= (ulCgmsAValue & 0x3) << 6;
			ulValue |= (ulApsTriggerValue & 0x3) << 8;
		}
		else
		{
			rc=DI_ERR_NOT_SUPPORTED;
			goto done;
		}
	}

	/**************SD*******************************************/
	if ( eVidWin != DI_VIDEO_WIN_HD )
	{
		/* DI_VIDEO_WIN_SD or DI_VIDEO_WIN_ALL */
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &displayVbiSettings);
			displayVbiSettings.cgmsEnabled = bOn;
			nexus_rc = NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &displayVbiSettings);
			if (nexus_rc)
			{
				PrintError("[%d]%s err from NEXUS_Display_SetVbiSettings",__LINE__,__func__);
				rc=DI_ERR_ERROR;
				goto done;
			}

			if (displayVbiSettings.cgmsEnabled)
			{
				nexus_rc = NEXUS_Display_SetCgms(s_stVideo[ulDeviceId].hDisplay[SD_DISP], ulValue);
				if (nexus_rc)
				{
					PrintError("[%d]%s err from NEXUS_Display_SetCgms",__LINE__,__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
		}
	}
	/**************HD*******************************************/
	if ( eVidWin != DI_VIDEO_WIN_SD )
	{
		/* DI_VIDEO_WIN_HD or DI_VIDEO_WIN_ALL */
		if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
		{
			NEXUS_Display_GetVbiSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displayVbiSettings);
			displayVbiSettings.cgmsEnabled = bOn;
			nexus_rc = NEXUS_Display_SetVbiSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displayVbiSettings);
			if (nexus_rc)
			{
				PrintError("[%d]%s err from NEXUS_Display_SetVbiSettings",__LINE__,__func__);
				rc=DI_ERR_ERROR;
				goto done;
			}


			if (displayVbiSettings.cgmsEnabled)
			{
				nexus_rc = NEXUS_Display_SetCgms(s_stVideo[ulDeviceId].hDisplay[HD_DISP], ulValue);
				if (nexus_rc)
				{
					PrintError("[%d]%s err from NEXUS_Display_SetCgms",__LINE__,__func__);
					rc=DI_ERR_ERROR;
					goto done;
				}
			}
		}
	}
done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

#if defined(CONFIG_CLOSED_CAPTION)
DI_ERR_CODE DI_VIDEO_SetClosedCaptionDataFeed(HUINT32 ulDeviceId, HBOOL bOn)
{
	PrintEnter();
#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : bOn:%d\n"PARAM_COLOR_OFF, __FUNCTION__, bOn);
#endif
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);
	VK_SEM_Get(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);

	s_stVideo[ulDeviceId].stClosedCaptionInfo.bEnabledCcData = bOn;	

	PrintExit();
	VK_SEM_Release(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);
	DI_VIDEO_REL_SEMA(ulDeviceId);
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_GetClosedCaptionDataState(HUINT32 ulDeviceId, DI_VIDEO_CC_DATA_STATE *eState)
{
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);
	VK_SEM_Get(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);

	*eState = s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState;

	VK_SEM_Release(s_stVideo[ulDeviceId].stClosedCaptionInfo.ulSemId_ClosedCaption);
	DI_VIDEO_REL_SEMA(ulDeviceId);
	
	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_VIDEO_RegisterEventCallback(HUINT32 ulDeviceId, DI_VIDEO_NOTIFY_EVT nEvent, EVENT_CALLBACK_FUNCTION fnVideoCallback, HUINT32* pulCallerId)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	int iIndex;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : event:%d\n"PARAM_COLOR_OFF, __FUNCTION__, nEvent);
#endif

	if(nEvent >= DI_VIDEO_NOTIFY_EVT_END)
	{
		PrintError("[%s] the number of event is too many or Callback is NULL.\n",__func__);
		rc=DI_ERR_INVALID_PARAM;
		goto done;
	}

	if (pulCallerId==NULL)
	{
		rc=DI_ERR_INVALID_PARAM;
		goto done;
	}

	if (*pulCallerId==0) // null
	{
		for (iIndex=0 ; iIndex<TOTAL_CALLER_NUM ; iIndex++)
		{
			int iEventType;
			for (iEventType=0 ; iEventType<DI_VIDEO_NOTIFY_EVT_END ; iEventType++)
			{
				if (s_stVideo[ulDeviceId].stCallbackInfo[iIndex][nEvent].pfnCallback!=NULL)
				{
					break;
				}
			}
			if (iEventType>=DI_VIDEO_NOTIFY_EVT_END)
				break;
		}

		if (iIndex>=TOTAL_CALLER_NUM)
		{
			rc=DI_ERR_ERROR;
			goto done;
		}

		s_stVideo[ulDeviceId].stCallbackInfo[iIndex][nEvent].pfnCallback=fnVideoCallback;
		*pulCallerId=iIndex+1; // return the assigned caller ID. It starts from 1.
	}
	else
	{
		if (0<*pulCallerId && *pulCallerId<=TOTAL_CALLER_NUM)
		{
			iIndex=*pulCallerId-1;
			s_stVideo[ulDeviceId].stCallbackInfo[iIndex][nEvent].pfnCallback=fnVideoCallback;
		}
		else
		{
			rc=DI_ERR_ERROR;
			goto done;
		}
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

#if defined(CONFIG_CLOSED_CAPTION)
DI_ERR_CODE DI_VIDEO_RegisterCcDataCallback(HUINT32 ulDeviceId, pfnDI_VIDEO_CcDataCallback fnCallback)
{
	s_stVideo[ulDeviceId].stCcCallbackInfo.pfnCcDataCallback=fnCallback;

	return DI_ERR_OK;
}
#endif

DI_ERR_CODE DI_VIDEO_PlayMemoryStream ( HUINT32 ulDeviceId, DI_VIDEO_STREAM_TYPE eStreamType, DI_VIDEO_FORMAT nDiVideoFmt, HUINT8 *pucMemBuff, HULONG ulMemBuffSize, void* stTarget)
{

	DI_ERR_CODE rc = DI_ERR_OK;
	DRV_Error nDrvErr = DRV_OK;
	NEXUS_VideoDecoderSettings decoderSettings;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : eStreamType:%d\n"PARAM_COLOR_OFF, __FUNCTION__, eStreamType);
#endif

	nDrvErr=DRV_VIDEO_PlayMemoryStreamPlaypumpStart(ulDeviceId, eStreamType);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] PlayPump Start failed !\n",__func__);
		rc = DI_ERR_ERROR;
		goto err;
	}

	if (stTarget)	// Still display To OSD Plane
	{
		nDrvErr=DRV_VIDEO_PlayMemoryStreamOsdPlane(ulDeviceId, nDiVideoFmt, pucMemBuff, ulMemBuffSize, stTarget);
		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] for OSD plane failed !\n",__func__);
			rc = DI_ERR_ERROR;
			goto done;
		}

	}
	else		// Still display To VIDEO Plane
	{
#if defined(CONFIG_SYNC_RECOVERY)
		DRV_SYNC_SetVideoTsmMode(FALSE);
#endif
		NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		decoderSettings.mute=false;
		decoderSettings.stillContentInterpolationMode = NEXUS_StillContentInterpolationMode_eBothField;
		NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
		s_stVideo[ulDeviceId].bIsStill=TRUE;

		nDrvErr=DRV_VIDEO_Start(ulDeviceId, nDiVideoFmt, FALSE); /* Only set TSM mode */
		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] Video Start Error\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
		else
		{
			s_stVideo[ulDeviceId].eVidCodec=nDiVideoFmt;
		}

		nDrvErr=DRV_VIDEO_PlayMemoryStreamVideoPlane(ulDeviceId, nDiVideoFmt, pucMemBuff, ulMemBuffSize);
		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] for Video plane failed \n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}

done:
	nDrvErr=DRV_VIDEO_PlayMemoryStreamPlaypumpStop(ulDeviceId);
	if(nDrvErr != DRV_OK)
		rc=DI_ERR_ERROR;
err:
	DI_VIDEO_REL_SEMA(ulDeviceId);
	PrintExit();
	return rc;
}


DI_ERR_CODE DI_VIDEO_StopMemoryStream(HUINT32 ulDeviceId)
{
	DRV_Error	nDrvErr=DRV_OK;
	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if (s_stVideo[ulDeviceId].bIsStill)
	{
		s_stVideo[ulDeviceId].bIsStill=FALSE;
		s_stVideo[ulDeviceId].bFirstPtsPassed=FALSE;
		nDrvErr=DRV_VIDEO_Stop(ulDeviceId);
		if(nDrvErr == DRV_ERR)
		{
			PrintError("[%s] Stop failed!\n",__func__);
		}
	}

	s_stVideo[ulDeviceId].bTsm=TRUE;

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_StartCapture(HUINT32 ulDeviceId, HUINT16 tgtWidth, HUINT16 tgtHeight, HUINT8 *CapturedBitmap)
{
#if defined(CONFIG_CAPTURE_DECODING)
	NEXUS_StripedSurfaceHandle stripedSurface;
	NEXUS_Graphics2DHandle gfx;
	int i = 0;
#else
	NEXUS_SurfaceHandle nSurface;
	HUINT32 ulWindowId = ulDeviceId;
#endif
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_Error NexusErr = NEXUS_SUCCESS;
	DI_OSD_INSTANCE* pOsdInstance;

	BSTD_UNUSED(tgtWidth);
	BSTD_UNUSED(tgtHeight);

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s \n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

#if defined(CONFIG_CAPTURE_DECODING)
	for( i = 0; i < 3; i++)
	{
		stripedSurface = NEXUS_VideoDecoder_CreateStripedSurface(s_stVideo[ulDeviceId].hDecoder);
		if (stripedSurface)
		{
			break;
		}
		VK_TASK_Sleep(50);
	}

	if (stripedSurface==NULL)
	{
		PrintDebug("nSurface is NULL\n");
		rc=DI_ERR_ERROR;
		goto done;
	}
	DRV_OSD_GetGraphics2DHandle(&gfx);

	pOsdInstance=(DI_OSD_INSTANCE*)(*(unsigned long*)CapturedBitmap);
	NexusErr = NEXUS_Graphics2D_DestripeToSurface(gfx, stripedSurface, (NEXUS_SurfaceHandle)pOsdInstance->surface, NULL);
	if (NexusErr != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_VideoWindow_ReleaseVideoBuffer failed\n",__func__);
		NEXUS_VideoDecoder_DestroyStripedSurface(s_stVideo[ulDeviceId].hDecoder, stripedSurface);
		rc=DI_ERR_ERROR;
		goto done;
	}
	DRV_OSD_Sync();
	NEXUS_VideoDecoder_DestroyStripedSurface(s_stVideo[ulDeviceId].hDecoder, stripedSurface);
#else
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		nSurface= NEXUS_VideoWindow_CaptureVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);
		if (nSurface==NULL)
		{
			PrintDebug("nSurface is NULL\n");
			rc=DI_ERR_ERROR;
			goto done;
		}

		VK_TASK_Sleep(100);

		pOsdInstance=(DI_OSD_INSTANCE*)(*(unsigned long*)CapturedBitmap);

		if (DRV_OSD_CopyImage(nSurface, (NEXUS_SurfaceHandle)pOsdInstance->surface) != DRV_OK)
		{
			PrintError("[%s] DRV_OSD_CopyImage failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}

		NexusErr = NEXUS_VideoWindow_ReleaseVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], nSurface);
		if (NexusErr != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_VideoWindow_ReleaseVideoBuffer failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}
#endif
done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}


DI_ERR_CODE DI_VIDEO_GetThumbnail(HUINT32 ulDeviceId, HUINT16 tgtWidth, HUINT16 tgtHeight, HUINT8 *CapturedBitmap)
{
	NEXUS_SurfaceHandle nSurface=NULL;
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_Error NexusErr = NEXUS_SUCCESS;
	DI_OSD_INSTANCE* pOsdInstance;
#if defined(CONFIG_THUMBNAIL_DECODING)
	DRV_Error drvError;
	int useVideoCapture = 0;
#endif
	HUINT32 ulWindowId = ulDeviceId;

	BSTD_UNUSED(tgtWidth);
	BSTD_UNUSED(tgtHeight);

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s \n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] == NULL)
	{
		if(s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] == TRUE)
		{
			PrintError("[%s] Can't create image capture, because of null display !!!\n",__FUNCTION__);
			rc=DI_ERR_ERROR;
		}
		else
		{
			PrintError("[%s] Can't create image capture, because display is not supported!!!\n",__FUNCTION__);
			rc=DI_ERR_OK;
		}
		goto done;

	}

#if defined(CONFIG_THUMBNAIL_DECODING)
	drvError = DRV_VIDEO_GetThumbnailImage(ulDeviceId, &nSurface, NULL);
	if(drvError != DRV_OK)
	{
		useVideoCapture = 1;
		nSurface= NEXUS_VideoWindow_CaptureVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);
		if (nSurface==NULL)
		{
			PrintDebug("nSurface is NULL\n");
			rc=DI_ERR_ERROR;
			goto done;
		}
	}
	if(nSurface != NULL)
	{
#else
	nSurface= NEXUS_VideoWindow_CaptureVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);
	if (nSurface==NULL)
	{
		PrintDebug("nSurface is NULL\n");
		rc=DI_ERR_ERROR;
		goto done;
	}
#endif

	pOsdInstance=(DI_OSD_INSTANCE*)(*(unsigned long*)CapturedBitmap);

	if (DRV_OSD_CopyImage(nSurface, (NEXUS_SurfaceHandle)pOsdInstance->surface) != DRV_OK)
	{
		PrintError("[%s] DRV_OSD_CopyImage failed\n",__func__);
		rc=DI_ERR_ERROR;
#if defined(CONFIG_THUMBNAIL_DECODING)
		if(useVideoCapture != 1)
		{
			NEXUS_Surface_Destroy(nSurface);
		}
#endif
		goto done;
	}

#if defined(CONFIG_THUMBNAIL_DECODING)
	}
	if(useVideoCapture == 1)
	{
		NexusErr = NEXUS_VideoWindow_ReleaseVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], nSurface);
		if (NexusErr != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_VideoWindow_ReleaseVideoBuffer failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}
	else
	{
		NEXUS_Surface_Destroy(nSurface);
	}
#else
	NexusErr = NEXUS_VideoWindow_ReleaseVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], nSurface);
	if (NexusErr != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_VideoWindow_ReleaseVideoBuffer failed\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}
#endif

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_StartScreenCapture(HUINT32 ulDeviceId, DI_VIDEO_CAPTURE_t eCap, HUINT16 tgtWidth, HUINT16 tgtHeight, HUINT8 *CapturedBitmap)
{
	NEXUS_SurfaceHandle nSurface;
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_Error NexusErr = NEXUS_SUCCESS;
	DI_OSD_INSTANCE* pOsdInstance;
#if defined(CONFIG_USE_THUMBNAIL_DECODING)
	DRV_Error drvError;
#endif
	HUINT32 ulWindowId = ulDeviceId;

	BSTD_UNUSED(tgtWidth);
	BSTD_UNUSED(tgtHeight);

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] == NULL)
	{
		if(s_stVideo[ulDeviceId].bOpenDisplay[HD_DISP] == TRUE)
		{
			PrintError("[%s] Can't create screen capture, because of null display !!!\n",__FUNCTION__);
			rc=DI_ERR_ERROR;
		}
		else
		{
			PrintError("[%s] Can't create screen capture, because display is not supported!!!\n",__FUNCTION__);
			rc=DI_ERR_OK;
		}
		goto done;

	}

	pOsdInstance=(DI_OSD_INSTANCE*)(*(unsigned long*)CapturedBitmap);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s \n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

	if((eCap == DI_VIDEO_CAP_ALL) || (eCap == DI_VIDEO_CAP_VIDEO))
	{
#if defined(CONFIG_USE_THUMBNAIL_DECODING)
		drvError = P_VIDEO_GetThumbnailImage(&nSurface);
		if(drvError == DRV_OK)
		{
#else
		nSurface= NEXUS_VideoWindow_CaptureVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId]);
		if (nSurface==NULL)
		{
			PrintDebug("nSurface is NULL\n");
			rc=DI_ERR_ERROR;
			goto done;
		}
#endif
		if (DRV_OSD_CopyImage(nSurface, (NEXUS_SurfaceHandle)pOsdInstance->surface) != DRV_OK)
		{
			PrintError("[%s] DRV_OSD_CopyImage failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}

#if defined(CONFIG_USE_THUMBNAIL_DECODING)
		}
#else
		NexusErr = NEXUS_VideoWindow_ReleaseVideoBuffer(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], nSurface);
		if (NexusErr != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_VideoWindow_ReleaseVideoBuffer failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
#endif
	}

	if((eCap == DI_VIDEO_CAP_ALL) || (eCap == DI_VIDEO_CAP_OSD))
	{
		if (DRV_OSD_BlendVisibleSurface((NEXUS_SurfaceHandle)pOsdInstance->surface) != DRV_OK)
		{
			PrintError("[%s] DRV_OSD_CopyImage failed\n",__func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_StopCapture(HUINT32 ulDeviceId)
{
	PrintEnter();
	UNREFENCED_PARAM(ulDeviceId);
	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_GetSequenceHeaderInfo(HUINT32 ulDeviceId, DI_VIDEO_StreamInfo_t *stream_info)
{
	DI_ERR_CODE rc=DI_ERR_OK;


	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(stream_info == NULL)
	{
		PrintError("[%s] stream_info is NULL.\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	P_VIDEO_CopySeqHeaderInfo(ulDeviceId,stream_info);

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetEventTimeOut(HUINT32 ulDeviceId, HUINT32 ulTimeOut)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	HUINT32 ulTimeOut_Modified;


	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : timeout:%d\n"PARAM_COLOR_OFF, __FUNCTION__, ulTimeOut);
#endif

	if(ulTimeOut < DI_VIDEO_UNDERRUN_TIMEOUT_DEFAULT)
	{
		PrintDebug("[%s] Event Time is %d\n",__func__,DI_VIDEO_UNDERRUN_TIMEOUT_DEFAULT);
		rc=DI_ERR_OK; /*keep previous value and return OK */
		goto done;
	}

	ulTimeOut_Modified = (ulTimeOut/DI_VIDEO_UNDERRUN_TIMEOUT_RES)*DI_VIDEO_UNDERRUN_TIMEOUT_RES
		+((ulTimeOut%DI_VIDEO_UNDERRUN_TIMEOUT_RES)?100:0);

	s_stVideo[ulDeviceId].ulUnderrunTimeout = ulTimeOut_Modified;
	PrintDebug("[%s] Modified underrun timeout is %ld\n",__func__,s_stVideo[ulDeviceId].ulUnderrunTimeout);

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_GetAfdInfo(HUINT32 ulDeviceId, DI_VIDEO_ACTIVE_FORMAT *peActiveFormat)
{
	DI_ERR_CODE rc=DI_ERR_OK;


	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(peActiveFormat == NULL)
	{
		PrintError("[%s] peActiveFormat is NULL\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	*peActiveFormat=s_stVideoInfo[ulDeviceId].afd;
	PrintDebug("[%s] current afd is %x\n",__func__,s_stVideoInfo[ulDeviceId].afd);

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_VIDEO_Set3DFormat(HUINT32 ulDeviceId, DI_VIDEO_3D_FORMAT e3dFormat)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	DRV_Error nDrvErr = DRV_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].e3dDisplayFormat==e3dFormat)
	{
		PrintError("[%s] : Same display 3d format(%d)!!\n", __func__, e3dFormat);
		rc=DI_ERR_ERROR;
		goto done;
	}

	nDrvErr = P_VIDEO_Set3DFormat(ulDeviceId, e3dFormat);
	if (nDrvErr != DRV_OK)
	{
		PrintError("[%s] : P_VIDEO_Set3DFormat error!\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}
done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_Get3DInfo(HUINT32 ulDeviceId, DI_VIDEO_3D_FORMAT *pe3dFormat)
{
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(pe3dFormat == NULL)
	{
		PrintError("[%s] pe3dFormat is NULL\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	*pe3dFormat=s_stVideo[ulDeviceId].e3dFormat;
	PrintDebug("[%s] current afd is %x\n",__func__,s_stVideo[ulDeviceId].e3dFormat);

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}
#endif
DI_ERR_CODE DI_VIDEO_ControlDPort(HUINT32 ulDeviceId, DI_VIDEO_HD_RESOLUTION eHdResolution, DI_VIDEO_AspectRatio_t eAspectRatio)
{
	DI_ERR_CODE rc=DI_ERR_OK;


	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	/*
	 * Write codes here, using "drv_gpio.c".
	 * Thanks.
	 *
	 * 2009.7.14 dcchung
	 */

#if defined(DI_VIDEO_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : HD Resolution :%d AspectRatio : %d\n"PARAM_COLOR_OFF, __FUNCTION__, eHdResolution,eAspectRatio);
#endif

	/* Line 1 Total Scanning Lines (1080,720,480)*/
	switch(eHdResolution)
	{
		case DI_VIDEO_HD_RESOLUTION_480I:
		case DI_VIDEO_HD_RESOLUTION_480P:
		case DI_VIDEO_HD_RESOLUTION_480I_J:
			DRV_GPIO_Write(GPIO_ID_DCFG_SCANLINE_0,GPIO_HI);
			DRV_GPIO_Write(GPIO_ID_DCFG_SCANLINE_1,GPIO_HI);/* Don't Care*/
			if( eHdResolution == DI_VIDEO_HD_RESOLUTION_480I || eHdResolution == DI_VIDEO_HD_RESOLUTION_480I_J )
			{
				DRV_GPIO_Write(GPIO_ID_DCFG_IORP,GPIO_HI);
			}
			else
			{
				DRV_GPIO_Write(GPIO_ID_DCFG_IORP,GPIO_LOW);
			}
			break;
		case DI_VIDEO_HD_RESOLUTION_720P:
			DRV_GPIO_Write(GPIO_ID_DCFG_SCANLINE_0,GPIO_LOW);
			DRV_GPIO_Write(GPIO_ID_DCFG_SCANLINE_1,GPIO_HI);

			DRV_GPIO_Write(GPIO_ID_DCFG_IORP,GPIO_LOW);
			break;
		case DI_VIDEO_HD_RESOLUTION_1080I:
			DRV_GPIO_Write(GPIO_ID_DCFG_SCANLINE_0,GPIO_LOW);
			DRV_GPIO_Write(GPIO_ID_DCFG_SCANLINE_1,GPIO_LOW);

			DRV_GPIO_Write(GPIO_ID_DCFG_IORP,GPIO_HI);
			break;
		default:
			PrintError("[%s] Resulution(%d) is not supported\n",__func__,eHdResolution);
			rc=DI_ERR_ERROR;
			goto done;
	}

	switch(eAspectRatio)
	{
		case DI_VIDEO_ASPECTRATIO_16_9:
			DRV_GPIO_Write(GPIO_ID_DCFG_ASPECTRATIO_0,GPIO_LOW);
			DRV_GPIO_Write(GPIO_ID_DCFG_ASPECTRATIO_1,GPIO_LOW);
			break;
		case DI_VIDEO_ASPECTRATIO_4_3:
			DRV_GPIO_Write(GPIO_ID_DCFG_ASPECTRATIO_0,GPIO_HI);
			DRV_GPIO_Write(GPIO_ID_DCFG_ASPECTRATIO_1,GPIO_HI);	/* Don't Care*/
			break;
		case DI_VIDEO_ASPECTRATIO_4_3_LETTER:
			DRV_GPIO_Write(GPIO_ID_DCFG_ASPECTRATIO_0,GPIO_LOW);
			DRV_GPIO_Write(GPIO_ID_DCFG_ASPECTRATIO_1,GPIO_HI);
			break;
		default:
			PrintError("[%s] Resulution(%d) is not supported\n",__func__,eHdResolution);
			rc=DI_ERR_ERROR;
			goto done;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_SetSync(HBOOL bOnOff)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	s_stVideo[ulDeviceId].bTsm=bOnOff;
	PrintDebug("[%s] Set video mode is %s\n",__func__,(bOnOff == TRUE) ? "Sync" : " Async");

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}

DI_ERR_CODE DI_VIDEO_GetTrickState(void *phandle, void *pTrickState)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;

	NEXUS_VideoDecoderHandle *decoderHandle;
	NEXUS_VideoDecoderTrickState *decoderTrickState;

	PrintEnter();

	if((phandle == NULL) || (pTrickState == NULL))
	{
		PrintError("DI_VIDEO_GetTrickState parameter NULL!\n");
		goto done;
	}

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	decoderHandle = (NEXUS_VideoDecoderHandle*)phandle;
	decoderTrickState = (NEXUS_VideoDecoderTrickState*)pTrickState;
	NEXUS_VideoDecoder_GetTrickState(*decoderHandle, decoderTrickState);

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
done:

	return rc;
}

DI_ERR_CODE DI_VIDEO_SetTrickState(void *phandle, const void *pTrickState)
{
	DI_ERR_CODE rc=DI_ERR_OK;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;

	NEXUS_VideoDecoderHandle *decoderHandle;
	NEXUS_VideoDecoderTrickState *decoderTrickState;

	PrintEnter();

	if((phandle == NULL) || (pTrickState == NULL))
	{
		PrintError("DI_VIDEO_SetTrickState parameter NULL!\n");
		goto done;
	}

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	decoderHandle = (NEXUS_VideoDecoderHandle*)phandle;
	decoderTrickState = (NEXUS_VideoDecoderTrickState*)pTrickState;
	NEXUS_VideoDecoder_SetTrickState(*decoderHandle, decoderTrickState);

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
done:

	return rc;
}

DI_ERR_CODE DI_VIDEO_SetChannelChangeMode(HUINT32 ulDeviceId, DI_VIDEO_CC_MODE eMode)
{
	NEXUS_VideoDecoderSettings decoderSettings;
	NEXUS_Error nexusError;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);

	switch(eMode)
	{
		case DI_VIDEO_CC_MUTE:
			decoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eMute;
			break;

		case DI_VIDEO_CC_HOLD_UNTIL_TSMLOCK:
			decoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock;
			break;

		case DI_VIDEO_CC_MUTE_UNTIL_FIRST_PICTURE:
			decoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture;
			break;

		case DI_VIDEO_CC_HOLD_UNTIL_FIRST_PICTURE:
			decoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture;
			break;

		default:
			PrintError("[%s] Channel Change mode (%d) is unknown mode -> Set default mode(%d)\n",__func__,eMode, NEXUS_VideoDecoder_ChannelChangeMode_eMute);
			decoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eMute;
			break;
	}

	nexusError = NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] %dth decoder can't set Channel Change Mode [%d]\n",__func__,ulDeviceId,eMode);
		rc=DI_ERR_ERROR;
		goto done;
	}
	PrintDebug("[%s] %dth decoder is set of Channel Change Mode %d\n",__func__,ulDeviceId,eMode);

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);
	return rc;
}


static void P_VIDEO_BuffMonitorTask(void *pvParam)
{
	/*
	 * To prevent "NO_DATA" callback at the beginning, the initial state should be "NO_DATA".
	 * If it is set to "UNKNOWN", it makes a chance to send "NO_DATA" before DI_VIDEO_Start is called.
	 *
	 * 2009.8.10 dcchung
	 */
	DI_VIDEO_RUNNING_STATE CurDecStatus=DI_VIDEO_RUNNING_STATE_NODATA;
	DI_VIDEO_RUNNING_STATE PreDecStatus=DI_VIDEO_RUNNING_STATE_NODATA;
	NEXUS_VideoDecoderStatus decoderStatus;
	NEXUS_VideoDecoderTrickState decoderTrickState;
#if defined(CONFIG_WORKAROUND_VIDEO_MUTE)
	NEXUS_VideoDecoderSettings decoderSettings;
#endif
	unsigned PrevNumDecoded=0;
	unsigned long ulTimeoutCnt=0;
	static DI_VIDEO_NOTIFY_EVT event = DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED;
	ST_VIDEO_MSG VideoMonMsg;
	HUINT32 *pDeviceId = (HUINT32 *)pvParam;
	HUINT32 ulDeviceId=*pDeviceId;
	NEXUS_Error rc;
	DRV_Error nDrvErr=DRV_OK;
	int nVkErr=VK_OK;
#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
	unsigned long ulPrevDecodeError=0;
	unsigned long ulDecodeError=0;
	unsigned long ulPrevDisplayError=0;
	unsigned long ulDisplayError=0;
	NEXUS_VideoDecoderExtendedStatus decoderExtStatus;
	unsigned long ulPrevDisplayDrop=0;
	unsigned long ulDisplayDrop=0;
	unsigned long ulPrevDecoded=0;
	unsigned long ulDecoded=0;
	unsigned long ulPrevDisplayed=0;
	unsigned long ulDisplayed=0;
	unsigned long ulVideoCheckCnt=0;
#if defined(CONFIG_SYNC_RECOVERY)
	unsigned long ulSyncMsgId;
	DRV_SYNC_MSG SyncMsg;
#else
	DI_AUDIO_FORMAT	eAudFmt[3];
#endif
#endif
#if defined(PRINT_VIDEO_PTS_INFO)
	HUINT32 printCount = 0;
#endif

	HAPPY(nVkErr);

	while (1)
	{
		if (s_stVideo[ulDeviceId].bInitialized != TRUE)
		{
			VK_TASK_Sleep(VIDEO_TASK_CHECK_STATE_TIME);
			continue;
		}

		if (VK_MSG_ReceiveTimeout(s_stVideo[ulDeviceId].ulVideoMonMsgQueueId, &VideoMonMsg,sizeof(VideoMonMsg), VIDEO_TASK_CHECK_STATE_TIME)==VK_OK)
		{
			if (VideoMonMsg.ulParam0==DI_VIDEO_RUNNING_STATE_UNKNOWN)
			{
				PreDecStatus=DI_VIDEO_RUNNING_STATE_UNKNOWN;
				CurDecStatus=DI_VIDEO_RUNNING_STATE_UNKNOWN;
				ulTimeoutCnt=0;

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
				ulPrevDecodeError=0;
				ulDecodeError=0;
				ulPrevDisplayError=0;
				ulDisplayError=0;
				ulPrevDisplayDrop=0;
				ulDisplayDrop=0;
				ulDecoded=0;
				ulPrevDecoded=0;
				ulDisplayed=0;
				ulPrevDisplayed=0;
#endif
			}
		}

#if defined(CONFIG_CLOSED_CAPTION)
		s_ulCcDataChecker[ulDeviceId]++;

		if( s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState != DI_VIDEO_CC_DATA_STATE_NODATA
		 && s_ulCcDataChecker[ulDeviceId] > CLOSED_CAPTION_DATA_CHECK_TIME / VIDEO_TASK_CHECK_STATE_TIME )
		{
			/* Add event and state info to callback's param and call the event's callback */
			static DI_VIDEO_CC_DATA_STATE_EVT_t ccDataState;
			ccDataState.eState=DI_VIDEO_CC_DATA_STATE_NODATA;
			P_VIDEO_EVTCall(ulDeviceId, DI_VIDEO_NOTIFY_EVT_CC_DATA_STATE_CHANGED, (void *)&ccDataState);
			s_stVideo[ulDeviceId].stClosedCaptionInfo.eCurCcDataState= DI_VIDEO_CC_DATA_STATE_NODATA;
		}
#endif	

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			PrintDebug("[%s] Stanby mode: skip status checking..\n", __func__);
			continue;
		}
#endif
		rc=NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);
		if (rc) continue;

#if defined(PRINT_VIDEO_PTS_INFO)
		{
			printCount++;
			if(printCount == 1)
			{
				HUINT32 stc;
				DRV_VIDEO_GetCurrentStc(ulDeviceId, &stc);
				DI_UART_Print("\033[1;33m\t\t[%s:%d]\t"
				
							"Video Decoder[%d] : PTS (%#x) STC (%#x) Diff(%d) fifoDepth (%d(%d%%))"

							"\033[00m\n", __func__, __LINE__,
							ulDeviceId,
							decoderStatus.pts,
							stc,
							decoderStatus.ptsStcDifference,
							decoderStatus.fifoDepth,
							decoderStatus.fifoSize?(decoderStatus.fifoDepth*100)/decoderStatus.fifoSize:0);
			}

			printCount = printCount%10;
		}
#endif

		#if defined(CONFIG_TEMP_DTG_HDR2000T)
		//skip decoder status check for old octo1.0 model
		#else
		if(decoderStatus.started != true)
			continue;
		#endif

		NEXUS_VideoDecoder_GetTrickState(s_stVideo[ulDeviceId].hDecoder,&decoderTrickState);

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
		rc=NEXUS_VideoDecoder_GetExtendedStatus(s_stVideo[ulDeviceId].hDecoder,&decoderExtStatus);
		if (rc) continue;

		ulVideoCheckCnt++;		
#define DECODER_STATUS_CHECK_TIMEOUT 5000 /* msec.  It need to adjust using test */
		if( (ulVideoCheckCnt * VIDEO_TASK_CHECK_STATE_TIME) > DECODER_STATUS_CHECK_TIMEOUT )
		{
			ulDecodeError = 0;
			ulDisplayDrop = 0;
			ulDisplayError = 0;
			
			ulVideoCheckCnt = 0;
		}

		ulDisplayDrop += ((int)(decoderStatus.numDisplayDrops - ulPrevDisplayDrop) > 0 ? decoderStatus.numDisplayDrops - ulPrevDisplayDrop : 0);
		ulDecodeError += ((int)(decoderStatus.numDecodeErrors - ulPrevDecodeError) > 0 ? decoderStatus.numDecodeErrors - ulPrevDecodeError : 0);
		ulDisplayError += ((int)(decoderStatus.numDisplayErrors - ulPrevDisplayError) > 0 ? decoderStatus.numDisplayErrors - ulPrevDisplayError : 0);
		ulDecoded += ((int)(decoderStatus.numDecoded - ulPrevDecoded) > 0 ? decoderStatus.numDecoded - ulPrevDecoded : 0);
		ulDisplayed += ((int)(decoderStatus.numDisplayed - ulPrevDisplayed) > 0 ? decoderStatus.numDisplayed - ulPrevDisplayed : 0);

		if(s_stVideo[ulDeviceId].bIsPlayback)
		{
			/* Ignore display error when playback is paused. */
			if(((ulDecoded-ulPrevDecoded) == 0) && ((ulDisplayed-ulPrevDisplayed) == 0))
			{
				ulDisplayError=0;
			}
		}
		else
		{
			/* No more needs to check decoder error count in Live mode */
			ulDecodeError=0;
		}

		/* Trick play -> skip restart.
		Note : normal speed 이상인 경우에 decoderTrickState.rate == NORMAL_PLAY_SPEED을 유지함 -> B사 bug인듯.. 일단 pause, slowmotion일 때 유효 */
		if(decoderTrickState.hostTrickModesEnabled || decoderTrickState.brcmTrickModesEnabled
			|| decoderTrickState.stcTrickEnabled || decoderTrickState.dqtEnabled
			|| decoderTrickState.rate != NORMAL_PLAY_SPEED)
		{
			ulDisplayDrop=0;
			ulDecodeError=0;
			ulDisplayError=0;
		}
		/* Restart 조건으로 ulDisplayError는 특정 조건일 때만 사용함.(정상적인 동작에서도 ulDisplayError는 증가할 수 있기 때문에..) */
		if(!(s_stVideo[ulDeviceId].bIsPlayback && decoderExtStatus.pictureRepeatFlag && decoderTrickState.rate == NORMAL_PLAY_SPEED))
		{
			ulDisplayError=0;
		}

		/* When Decoder Pause or not playback, display drop is occurred. We don't need this status into restart decoder */
		if(s_stVideo[ulDeviceId].bFreeze == TRUE || !s_stVideo[ulDeviceId].bIsPlayback)
		{
			ulDisplayDrop=0;
		}

		if(ulDisplayDrop > 0 || ulDecodeError > 0 || ulDisplayError > 0)
		{
			PrintDebug("[%d]%s ulDisplayDrop = %d, ulDecodeError = %d, ulDisplayError = %d\n",__LINE__,__func__,ulDisplayDrop, ulDecodeError, ulDisplayError);
		}

		if(s_stVideo[ulDeviceId].ulResetTimerId == VK_TIMERID_NULL && s_stVideo[ulDeviceId].bDecReset == FALSE
			&& decoderStatus.started == true
			&& s_stVideo[ulDeviceId].bFreeze == FALSE
			&& (ulDisplayDrop > DEC_DIFF_DISPLAY_DROP_NUM || ulDecodeError > DEC_DIFF_DECODE_ERROR_NUM || ulDisplayError > DEC_DIFF_DISPLAY_ERROR_NUM))
		{
#if defined(CONFIG_WORKAROUND_DEC_RESTART)
#if defined(CONFIG_SYNC_RECOVERY)
				DRV_SYNC_GetMsgId(&ulSyncMsgId);
				SyncMsg.ulParam0 = DRV_SYNC_REQ_RESTART;
				if(ulSyncMsgId != 0)
				{
					VK_MSG_Send(ulSyncMsgId, &SyncMsg, sizeof(SyncMsg));
					PrintError("[%s] Sending Restart Message to Sync Module \n",__func__);
				}
#else
				PrintError("[%s] Decoder Restart \n",__func__);
				nDrvErr=DRV_VIDEO_Stop(ulDeviceId);
				if(nDrvErr == DRV_ERR)
				{
					PrintError("[%s] Stop failed!\n",__func__);
				}

				VK_MEM_Memset(eAudFmt,0,3);
#if defined(CONFIG_AUDIO_DECODER)
				DRV_AUDIO_GetAudioFormat(DI_AUDIO_DECODER_MAIN,&eAudFmt[DI_AUDIO_DECODER_MAIN]);
				if(eAudFmt[DI_AUDIO_DECODER_MAIN] != DI_AUDFORMAT_NONE)
				{
					DRV_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
				}

				DRV_AUDIO_GetAudioFormat(DI_AUDIO_DECODER_SUB,&eAudFmt[DI_AUDIO_DECODER_SUB]);
				if(eAudFmt[DI_AUDIO_DECODER_SUB] != DI_AUDFORMAT_NONE)
				{
					DRV_AUDIO_Stop(DI_AUDIO_DECODER_SUB);
				}

				DRV_AUDIO_GetAudioFormat(DI_AUDIO_DECODER_SPDIF,&eAudFmt[DI_AUDIO_DECODER_SPDIF]);
				if(eAudFmt[DI_AUDIO_DECODER_SPDIF] != DI_AUDFORMAT_NONE)
				{
					DRV_AUDIO_Stop(DI_AUDIO_DECODER_SPDIF);
				}
#if defined(CONFIG_SYNC_RECOVERY)
				DRV_SYNC_SetTsmMode();
#endif

				DRV_VIDEO_Start(ulDeviceId,s_stVideo[ulDeviceId].eVidCodec, TRUE);
				if(eAudFmt[DI_AUDIO_DECODER_MAIN] != DI_AUDFORMAT_NONE)
				{
					PrintDebug("[%s Audio main Decoder Start = %d format\n",__func__,eAudFmt[DI_AUDIO_DECODER_MAIN]);
					DRV_AUDIO_Start(DI_AUDIO_DECODER_MAIN,eAudFmt[DI_AUDIO_DECODER_MAIN]);
				}

				if(eAudFmt[DI_AUDIO_DECODER_SUB] != DI_AUDFORMAT_NONE)
				{
					PrintDebug("[%s Audio sub Decoder Start = %d format\n",__func__,eAudFmt[DI_AUDIO_DECODER_SUB]);
					DRV_AUDIO_Start(DI_AUDIO_DECODER_SUB,eAudFmt[DI_AUDIO_DECODER_SUB]);
				}

				if(eAudFmt[DI_AUDIO_DECODER_SPDIF] != DI_AUDFORMAT_NONE)
				{
					PrintDebug("[%s Audio spdif Decoder Start = %d format\n",__func__,eAudFmt[DI_AUDIO_DECODER_SPDIF]);
					DRV_AUDIO_Start(DI_AUDIO_DECODER_SPDIF,eAudFmt[DI_AUDIO_DECODER_SPDIF]);
				}
#endif
				ulPrevDisplayDrop=0;
				ulPrevDecodeError=0;
				ulPrevDisplayError=0;
				ulPrevDecoded=0;
				ulPrevDisplayed=0;

				ulVideoCheckCnt = 0;
#endif
#else
				PrintError("[%s] Decoder Reset \n",__func__);
				NEXUS_VideoDecoder_Reset(s_stVideo[ulDeviceId].hDecoder);
#endif
				if(s_stVideo[ulDeviceId].ulResetTimerId != VK_TIMERID_NULL)
				{
					VK_TIMER_Cancel(s_stVideo[ulDeviceId].ulResetTimerId);
					s_stVideo[ulDeviceId].ulResetTimerId = VK_TIMERID_NULL;
				}

				nVkErr = VK_TIMER_EventAfter(DEC_RESET_CLEAR_TIME, P_VIDEO_ResetDecoderClear,&ulDeviceId,sizeof(ulDeviceId),(unsigned long *) &s_stVideo[ulDeviceId].ulResetTimerId);
				if(nVkErr != VK_OK)
				{
					PrintError("[%s] VK_TIMER_EventAfter Err(0x%x)!!\n",__func__,nVkErr);
				}

				s_stVideo[ulDeviceId].bDecReset=TRUE;
				ulDisplayDrop=0;
				ulDecodeError=0;
				ulDisplayError=0;
		}
		else
		{
			ulPrevDisplayDrop=decoderStatus.numDisplayDrops;
			ulPrevDecodeError=decoderStatus.numDecodeErrors;
			ulPrevDisplayError=decoderStatus.numDisplayErrors;
			ulPrevDecoded=decoderStatus.numDecoded;
			ulPrevDisplayed=decoderStatus.numDisplayed;
		}
#endif
		if ((PrevNumDecoded != decoderStatus.numDecoded || decoderTrickState.rate == 0 || s_stVideo[ulDeviceId].bIsStill==TRUE )
			&& (decoderStatus.numDecoded > 0)
			&& ((s_stVideo[ulDeviceId].bFirstPtsPassed==TRUE)||(s_stVideo[ulDeviceId].bDecState == FALSE)))
		{
			CurDecStatus = DI_VIDEO_RUNNING_STATE_DECODING;
			ulTimeoutCnt = 0;

			VK_SEM_Get(s_stVideo[ulDeviceId].ulSemId_DI);
			VK_SEM_Get(s_stVideo[ulDeviceId].ulSemId_ShowHide);
#if defined(CONFIG_WORKAROUND_ACTIVE_VIDEO_SHOW)	// kevin
			if (s_stVideo[ulDeviceId].bNeedToShow==TRUE)
			{
#if defined(CONFIG_WORKAROUND_VIDEO_WINDOW)
				nDrvErr=P_VIDEO_Show(ulDeviceId,TRUE);
#endif
#if defined(CONFIG_WORKAROUND_VIDEO_MUTE)
				VK_TASK_Sleep(60);
				NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
				decoderSettings.mute=false;
				NEXUS_VideoDecoder_SetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);
#endif
				if (nDrvErr==DRV_OK)
				{
					s_stVideo[ulDeviceId].bNeedToShow=FALSE;
				}

			}
#endif
			VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_ShowHide);
			VK_SEM_Release(s_stVideo[ulDeviceId].ulSemId_DI);
		}
		else
		{

			if(s_bBufferTimeoutTimer == TRUE)
			{
				ulTimeoutCnt = 0;
				s_bBufferTimeoutTimer = FALSE;
				PrintError("[%s %d] ulTimeoutCnt Clear\n",__func__,__LINE__);
			}

			ulTimeoutCnt++;
			if(ulTimeoutCnt >= (s_stVideo[ulDeviceId].ulUnderrunTimeout / VIDEO_TASK_CHECK_STATE_TIME))
			{
				if(s_stVideo[ulDeviceId].bIsPlayback == true)
				{
					HBOOL bPaused = FALSE;

#if defined(CONFIG_PLAYBACK)
					if(DRV_PVR_PLAY_IsPaused(ulDeviceId, &bPaused) != DRV_OK)
					{
						PrintError("[%s %d] DRV_PVR_PLAY_GetSpeed Fail\n",__func__,__LINE__);
						CurDecStatus = DI_VIDEO_RUNNING_STATE_NODATA;
					}
					else
					{
						if(bPaused == FALSE)
						{
							CurDecStatus = DI_VIDEO_RUNNING_STATE_NODATA;
						}
						else
						{
							/* No Operation */
							/* NODATA event should be ignored while pause-status */
							PrintDebug("[%s %d] NoData is ignored on pause\n",__func__,__LINE__);
						}
					}
#endif
				}
				else
				{
					CurDecStatus = DI_VIDEO_RUNNING_STATE_NODATA;
				}
				ulTimeoutCnt = 0;
			}
		}

		if(PreDecStatus != CurDecStatus)
		{
			static DI_VIDEO_RUNNING_STATE_EVT_t runningState;
			runningState.state=CurDecStatus;
			s_stVideo[ulDeviceId].bDecoding=(CurDecStatus==DI_VIDEO_RUNNING_STATE_DECODING)? TRUE:FALSE;
			PrintDebug("[%s] Current Decoder Status = %d\n",__func__,CurDecStatus);
			P_VIDEO_EVTCall(ulDeviceId, event, (void *)&runningState);
		}

		PreDecStatus = CurDecStatus;
		PrevNumDecoded=decoderStatus.numDecoded;
	}
}

void DRV_VIDEO_PrintStatus(HUINT32 ulDeviceId)
{
	NEXUS_DisplaySettings displaySettings;
	HUINT16 usHdDigitalWidth;
	HUINT16 usHdDigitalHeight;
	NEXUS_VideoWindowHandle window;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoDecoderStreamInformation streamInfo;
	NEXUS_VideoDecoderSettings decoderSettings;
	DI_VIDEO_AOUT_MODE_t OutputStatus;

	int iIndex;

	char strTemp[100];
	char* strAspectRatio[NEXUS_AspectRatio_eMax]={  "NEXUS_AspectRatio_eUnknown",
		"NEXUS_AspectRatio_eSquarePixel",
		"NEXUS_AspectRatio_e4x3",
		"NEXUS_AspectRatio_e16x9",
		"NEXUS_AspectRatio_e221x1",
		"NEXUS_AspectRatio_e15x9",
		"NEXUS_AspectRatio_eSar"};

	char* strFrameRate[NEXUS_VideoFrameRate_eMax]={ "NEXUS_VideoFrameRate_eUnknown",
		"NEXUS_VideoFrameRate_e23_976",
		"NEXUS_VideoFrameRate_e24",
		"NEXUS_VideoFrameRate_e25",
		"NEXUS_VideoFrameRate_e29_97",
		"NEXUS_VideoFrameRate_e30",
		"NEXUS_VideoFrameRate_e50",
		"NEXUS_VideoFrameRate_e59_94",
		"NEXUS_VideoFrameRate_e60"};

	char* strPtsType[NEXUS_PtsType_eInterpolatedFromInvalidPTS+1]={
		"NEXUS_PtsType_eCoded",
		"NEXUS_PtsType_eInterpolatedFromValidPTS",
		"NEXUS_PtsType_eHostProgrammedPTS",
		"NEXUS_PtsType_eInterpolatedFromInvalidPTSi"};

	HAPPY(strAspectRatio);
	HAPPY(strFrameRate);
	HAPPY(strPtsType);

	/* Get Stream info */
	NEXUS_VideoDecoder_GetStreamInformation(s_stVideo[ulDeviceId].hDecoder, &streamInfo);
	/* Get Decoder info */
	NEXUS_VideoDecoder_GetSettings(s_stVideo[ulDeviceId].hDecoder, &decoderSettings);

	// decoder status
	NEXUS_VideoDecoderStatus decoderStatus;
	NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder,&decoderStatus);
	PrintStatus("\n========== Decoder Status ===========\n");
	PrintStatus("started          : %s\n", (decoderStatus.started==1) ? "yes":"no");
#if !defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
	DRV_VIDEO_GetVideoCodecStr(s_stVideo[ulDeviceId].StartSettings.codec, strTemp);
#endif
	PrintStatus("codec            : %s\n", strTemp);
	PrintStatus("source (w,h)     : (%d,%d)\n", decoderStatus.source.width, decoderStatus.source.height);
	PrintStatus("coded (w,h)      : (%d,%d)\n", decoderStatus.coded.width, decoderStatus.coded.height);
	PrintStatus("display (w,h)    : (%d,%d)\n", decoderStatus.display.width, decoderStatus.display.height);
	PrintStatus("aspectRatio      : %s\n", strAspectRatio[decoderStatus.aspectRatio]);
	PrintStatus("frameRate        : %s\n", strFrameRate[decoderStatus.frameRate]);
	PrintStatus("interlaced       : %s\n",( decoderStatus.interlaced==1)? "yes":"no");
	DRV_VIDEO_GetVideoFormatStr(decoderStatus.format, strTemp);
	PrintStatus("format           : %s\n", strTemp);
	PrintStatus("muted            : %s\n", (decoderStatus.muted==1)? "yes":"no");
	PrintStatus("tsm              : %s\n", (decoderStatus.tsm==1)? "yes":"no");
	PrintStatus("ptsoffset        : %dms\n", decoderSettings.ptsOffset/45);
	PrintStatus("pts              : 0x%x\n", decoderStatus.pts);
	PrintStatus("ptsStcdiff              : 0x%x\n", decoderStatus.ptsStcDifference);
	PrintStatus("ptsType          : %s\n", strPtsType[decoderStatus.ptsType]);
	//PrintStatus("timeCode  : %d\n", decoderStatus.timeCode );
	PrintStatus("fifoDepth        : %d\n", decoderStatus.fifoDepth);
	PrintStatus("fifoSize         : %d\n", decoderStatus.fifoSize);
	PrintStatus("queueDepth       : %d\n", decoderStatus.queueDepth);
	PrintStatus("numDecoded       : %d\n", decoderStatus.numDecoded);
	PrintStatus("numDisplayed     : %d\n", decoderStatus.numDisplayed);
	PrintStatus("numDecodeErrors  : %d\n", decoderStatus.numDecodeErrors);
	PrintStatus("numDisplayErrors : %d\n", decoderStatus.numDisplayErrors);
	PrintStatus("numDecodeDrops   : %d\n", decoderStatus.numDecodeDrops);
	PrintStatus("numDisplayDrops  : %d\n", decoderStatus.numDisplayDrops);
	PrintStatus("numDisplayUnderflows : %d\n", decoderStatus.numDisplayUnderflows);
	PrintStatus("ptsErrorCount    : %d\n", decoderStatus.ptsErrorCount);
	PrintStatus("avdStatusBlock    : %d\n", decoderStatus.avdStatusBlock);
	PrintStatus("ulUnderrunTimeout: %lu ms\n", s_stVideo[ulDeviceId].ulUnderrunTimeout);
	PrintStatus("Current AFD      : %x\n", s_stVideoInfo[ulDeviceId].afd);

	NEXUS_VideoDecoderExtendedStatus decoderExtendedStatus;
	NEXUS_VideoDecoder_GetExtendedStatus(s_stVideo[ulDeviceId].hDecoder,&decoderExtendedStatus);
	PrintStatus("\n========== Decoder Extended Status ===========\n");
	PrintStatus("Video Mode	      : %d\n", s_stVideo[ulDeviceId].eDecoderMode);
	PrintStatus("RepeatFlag	      : %s\n", (decoderExtendedStatus.pictureRepeatFlag == true)?"Repeat" : "No Repeat");
#if defined(CONFIG_IFRAME_RANDOM_ACCESS)
	PrintStatus("TreatIFrameAsRap : %s\n", (s_stVideo[ulDeviceId].bTreatIFrameAsRap == true)? "ENABLE" : "DISABLE");
#endif
	PrintStatus("\n========== Display Status ===========\n");
	//window status
	for(iIndex=0 ; iIndex<=SD_DISP; iIndex++)
	{
		window = s_stVideo[ulDeviceId].hWindow[iIndex][ulDeviceId];
		if(window != NULL)
		{
			NEXUS_VideoWindow_GetSettings(window, &windowSettings);
			PrintStatus("%-17s: %s\n", (iIndex==HD_DISP)? "HD Disp":"SD Disp",windowSettings.visible == true ? "SHOW":"HIDE");
		}
	}

	// output status
	DRV_VIDEO_GetCurrentOutputStatus(&OutputStatus);
	PrintStatus("CVBS             : %s\n", OutputStatus.bCVBS==1?"ON":"OFF");
	PrintStatus("SVHS             : %s\n", OutputStatus.bSVHS==1?"ON":"OFF");
	PrintStatus("Component        : %s\n",OutputStatus.bYPbPr==1?"YPbPr":(OutputStatus.bYPbPr_Downscale==1?"YPbPr_Downscale":(OutputStatus.bRGB==1?"RGB":"NONE")));

	VK_memset(&displaySettings, 0x00, sizeof(NEXUS_DisplaySettings));
	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
		DRV_VIDEO_GetVideoFormatStr(displaySettings.format, strTemp);
		PrintStatus("format(HD)           : %s\n", strTemp);
		P_VIDEO_GetDisplaySize(displaySettings.format, &usHdDigitalWidth, &usHdDigitalHeight);
		PrintStatus("Digital (w,h)    : %d,%d\n", usHdDigitalWidth, usHdDigitalHeight);
		PrintStatus("Timebase(HD)          : %d\n", displaySettings.timebase);
	}
	else
		PrintStatus("format(HD)           : Not Support\n");

	if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[SD_DISP], &displaySettings);
		DRV_VIDEO_GetVideoFormatStr(displaySettings.format, strTemp);
		PrintStatus("format(SD)           : %s\n", strTemp);
		P_VIDEO_GetDisplaySize(displaySettings.format, &usHdDigitalWidth, &usHdDigitalHeight);
		PrintStatus("Digital (w,h)    : %d,%d\n", usHdDigitalWidth, usHdDigitalHeight);
		PrintStatus("Timebase(SD)          : %d\n", displaySettings.timebase);
	}
	else
		PrintStatus("format(SD)           : Not Support\n");

	PrintStatus("Desired HD InWindow : (%d,%d) (%d,%d)\n",  s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulX,  s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulY,
			s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulWidth,  s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulHeight);
	PrintStatus("Desired HD OutWindow: (%d,%d) (%d,%d)\n",  s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulX,  s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulY,
			s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulWidth,  s_stVideoInfo[ulDeviceId].stDesiredHdOutRect.ulHeight);

	PrintStatus("Desired SD InWindow : (%d,%d) (%d,%d)\n",  s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulX,  s_stVideoInfo[ulDeviceId].stDesiredSdInRect.ulY,
			s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulWidth,  s_stVideoInfo[ulDeviceId].stDesiredHdInRect.ulHeight);
	PrintStatus("Desired SD OutWindow: (%d,%d) (%d,%d)\n",  s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulX,  s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulY,
			s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulWidth,  s_stVideoInfo[ulDeviceId].stDesiredSdOutRect.ulHeight);

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulDeviceId] != NULL)
	{
		NEXUS_VideoWindowMadSettings madSettings;
		NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulDeviceId], &madSettings);

		PrintStatus("\n========== MAD Status ===========\n");
		PrintStatus("Deinterlace      : %s\n", madSettings.deinterlace==1?"ON":"OFF");

		PrintStatus("\n========== Stream Information Status ===========\n");
		PrintStatus("source (w,h)               : (%d,%d)\n", streamInfo.sourceHorizontalSize, streamInfo.sourceVerticalSize);
		PrintStatus("coded (w,h)                : (%d,%d)\n", streamInfo.codedSourceHorizontalSize, streamInfo.codedSourceVerticalSize);
		PrintStatus("display (w,h)              : (%d,%d)\n", streamInfo.displayHorizontalSize, streamInfo.displayVerticalSize);
		PrintStatus("aspectRatio                : (%d)\n", streamInfo.aspectRatio);
		PrintStatus("sampleAspectRatio (x,y)    : (%d,%d)\n", streamInfo.sampleAspectRatioX, streamInfo.sampleAspectRatioY);
	}
	PrintStatus("\n\n");
}

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_HWTEST)
void DI_VIDEO_RegisterResChangeCallback(pfnDI_VIDEO_Notify pfnNotify)
{
	reschange_callback = pfnNotify;
}
#endif

#if defined(CONFIG_THUMBNAIL_DECODING)
#define THUMBNAIL_W 640
#define THUMBNAIL_H	360
DRV_Error DRV_VIDEO_GetThumbnailImage(HUINT32 ulDeviceId, NEXUS_SurfaceHandle *psurfaceHandle, DRV_PVR_THUMBNAIL_SETUP_t *pthumbSetup)
{
	DRV_Error drvError = DRV_OK;
	NEXUS_Error rc;
	NEXUS_VideoCodec vidCodec;
	NEXUS_StripedSurfaceHandle stripedSurface;
	NEXUS_StripedSurfaceStatus stripedSurfaceStatus;
	NEXUS_Graphics2DHandle blitter;
	NEXUS_StillDecoderStartSettings stillDecoderSettings;
	NEXUS_StillDecoderStatus status;
	NEXUS_PidChannelHandle pidChannel;
	BKNI_EventHandle event;
	NEXUS_SurfaceCreateSettings newCreateSettings;
	NEXUS_SurfaceHandle intermediateSurface;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_Rect rect, rect_src;
	NEXUS_Graphics2DBlitSettings blitSettings;
	unsigned int timesec;

	*psurfaceHandle = NULL;
	DRV_VIDEO_GET_SEMA(ulDeviceId);
	if(pthumbSetup == NULL)
	{
		vidCodec = P_VIDEO_Convert2NexusCodec(s_stVideo[ulDeviceId].eVidCodec);
		timesec = 5;
	}
	else
	{
		vidCodec = P_VIDEO_Convert2NexusCodec(pthumbSetup->VideoCodec);
		timesec = pthumbSetup->ulCaptureTime;
	}

	DRV_OSD_GetGraphics2DHandle(&blitter);

	drvError = DRV_PVR_PB_ThumbnailInit(vidCodec, &pidChannel, pthumbSetup);
	if(drvError != DRV_OK)
	{
		PrintError("[%s] DRV_PVR_PB_ThumbnailInit failed (%d)\n",__func__, drvError);
		DRV_VIDEO_REL_SEMA(ulDeviceId);
		return drvError;
	}

	BKNI_CreateEvent(&event);
	NEXUS_StillDecoder_GetDefaultStartSettings(&stillDecoderSettings);
	stillDecoderSettings.pidChannel = pidChannel;
	stillDecoderSettings.stillPictureReady.callback = P_VIDEO_Still_picture_ready;
	stillDecoderSettings.stillPictureReady.context = event;
	stillDecoderSettings.codec = vidCodec;
	rc = NEXUS_StillDecoder_Start( s_stVideo[0].hStillDecoder, &stillDecoderSettings);
	if (rc)
	{
		BERR_TRACE(rc);
		drvError = DRV_ERR;
		goto done;
	}

	drvError = DRV_PVR_PB_ThumbnailFeed(timesec);
	if(drvError != DRV_OK)
	{
		PrintError("[%s] DRV_PVR_PB_ThumbnailFeed failed (%d)\n",__func__, drvError);
		drvError=DRV_ERR;
		goto done;
	}

	rc = BKNI_WaitForEvent(event, 2000);
	if (rc)
	{
		rc = NEXUS_StillDecoder_GetStatus( s_stVideo[0].hStillDecoder, &status);
		if (!rc && !status.endCodeFound)
		{
			PrintError("Still decode timed out because no end code was found in the ITB. Is this a valid still?\n");
		}
		else if (!rc && !status.stillPictureReady)
		{
			PrintError("Still decode timed out because the decoder did not respond. Is this a valid still?\n");
		}
		else
		{
			PrintError("Still decode timed out for unknown reasons.\n");
		}
	}

	DRV_OSD_AcquireSemaphore();
	rc = NEXUS_StillDecoder_GetStripedSurface( s_stVideo[0].hStillDecoder, &stripedSurface);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_StillDecoder_GetStripedSurface failed (%d)\n",__func__, rc);
		drvError=DRV_ERR;
		DRV_OSD_ReleaseSemaphore();
		goto done;
	}
	rc = NEXUS_StripedSurface_GetStatus(stripedSurface, &stripedSurfaceStatus);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_StripedSurface_GetStatus failed (%d)\n",__func__, rc);
		drvError=DRV_ERR;
		DRV_OSD_ReleaseSemaphore();
		goto done;
	}
	intermediateSurface=NEXUS_Graphics2D_Destripe(blitter, stripedSurface);
	if (intermediateSurface == NULL)
	{
		PrintError("[%s] NEXUS_Graphics2D_Destripe failed\n",__func__);
		drvError=DRV_ERR;
		DRV_OSD_ReleaseSemaphore();
		goto done;
	}


	rect.x = 0;
	rect.y = 0;
	rect.width = THUMBNAIL_W;
	rect.height = THUMBNAIL_H;

	rect_src.x = 0;
	rect_src.y = 0;
	rect_src.width = stripedSurfaceStatus.width-((stripedSurfaceStatus.width/40)*2);
	rect_src.height = stripedSurfaceStatus.height- ((stripedSurfaceStatus.height/40)*2);

	NEXUS_Surface_GetCreateSettings(intermediateSurface, &createSettings);
	newCreateSettings = createSettings;
	newCreateSettings.width = THUMBNAIL_W;
	newCreateSettings.height = THUMBNAIL_H;
	*psurfaceHandle = NEXUS_Surface_Create(&newCreateSettings);

	NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
	blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
	blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* YCrCb has no alpha, so we must set 0xFF */
		blitSettings.constantColor = 0xFF000000; /* alpha is opaque */
	blitSettings.source.surface = intermediateSurface;
	blitSettings.output.surface = *psurfaceHandle;
	blitSettings.source.rect = rect_src;
	blitSettings.output.rect = rect;
	NEXUS_Graphics2D_Blit(blitter, &blitSettings);

	DRV_OSD_Sync();

	NEXUS_Surface_Destroy(intermediateSurface);
	DRV_OSD_ReleaseSemaphore();

done:
	NEXUS_StillDecoder_Stop( s_stVideo[0].hStillDecoder);
	DRV_PVR_PB_ThumbnailUninit();
	BKNI_DestroyEvent(event);
	DRV_VIDEO_REL_SEMA(ulDeviceId);

	return drvError;
}
#endif

DI_ERR_CODE  DI_VIDEO_GetCodecCapability(DI_VIDEO_FORMAT eVideoCodec, DI_VIDEO_CODEC_CAP_t *pstCodecCapInfo)
{
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();

	if(pstCodecCapInfo == NULL)
	{
		PrintError("[%s] pstCodecCapInfo is NULL\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	switch(eVideoCodec)
	{
		case DI_VIDEO_FORMAT_MPEG2:
			pstCodecCapInfo->ulProfileLevelNumberOfEntries = sizeof(s_astMpeg2ProfileLevel)/sizeof(DI_VIDEO_PROFILELEVEL_t);
			pstCodecCapInfo->pstProfileLevel = s_astMpeg2ProfileLevel;
			break;
		case DI_VIDEO_FORMAT_MPEG4Part2:
			pstCodecCapInfo->ulProfileLevelNumberOfEntries = sizeof(s_astMpeg4part2ProfileLevel)/sizeof(DI_VIDEO_PROFILELEVEL_t);
			pstCodecCapInfo->pstProfileLevel = s_astMpeg4part2ProfileLevel;
			break;
		case DI_VIDEO_FORMAT_H264:
			pstCodecCapInfo->ulProfileLevelNumberOfEntries = sizeof(s_astH264ProfileLevel)/sizeof(DI_VIDEO_PROFILELEVEL_t);
			pstCodecCapInfo->pstProfileLevel = s_astH264ProfileLevel;
			break;
		default:
			rc = DI_ERR_ERROR;
			goto done;
			break;
	}
done:
	PrintExit();

	return rc;
}

#if defined(CONFIG_DI20)
DI_ERR_CODE DI_VIDEO_GetMacroVisionVersion(DI_VIDEO_MACROVISION_VERSION_TYPE *peMacrovisonVersion)
{
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();

	if(peMacrovisonVersion == NULL)
	{
		PrintError("[%s] peMacrovisonVersion is NULL\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	*peMacrovisonVersion = DI_VIDEO_MACROVISION_VERSION_NONE;

#if defined(CONFIG_MACROVISION_CERTIFICATION_DVD)
	*peMacrovisonVersion |= DI_VIDEO_MACROVISION_VERSION_7_1_D1;
#endif
#if defined(CONFIG_MACROVISION_CERTIFICATION_DMR)
	*peMacrovisonVersion |= DI_VIDEO_MACROVISION_VERSION_7_1_S1;
#endif
#if defined(CONFIG_MACROVISION_CERTIFICATION_MOP)
	*peMacrovisonVersion |= DI_VIDEO_MACROVISION_VERSION_7_2_H1;
#endif

done:
	PrintExit();

	return rc;
}

DI_ERR_CODE DI_VIDEO_GetOutputCapability(DI_VIDEO_OUTPUT_CAP_t *pstOuputCapInfo)
{
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();

	if(pstOuputCapInfo == NULL)
	{
		PrintError("[%s] pstOuputCapInfo is NULL\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	VK_MEM_Memset(pstOuputCapInfo, 0, sizeof(DI_VIDEO_OUTPUT_CAP_t));

#if ((NEXUS_NUM_COMPONENT_OUTPUTS >=1) && !defined(CONFIG_JAPAN_SPEC))
#if defined(CONFIG_YPBPR_PORT)
	pstOuputCapInfo->stAnalogOutput.bYPbPr = 1;
	pstOuputCapInfo->stAnalogOutput.bRGB = 0;
#else
	pstOuputCapInfo->stAnalogOutput.bYPbPr = 0;
	pstOuputCapInfo->stAnalogOutput.bRGB = 1;
#endif
#else
	pstOuputCapInfo->stAnalogOutput.bYPbPr = 0;
	pstOuputCapInfo->stAnalogOutput.bRGB = 0;
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 1
	pstOuputCapInfo->stAnalogOutput.bCVBS = 1;
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS >= 1
	pstOuputCapInfo->stAnalogOutput.bSVHS = 1;
#endif

#if defined(CONFIG_SDOUT_NTSC)
	pstOuputCapInfo->eVideoSDStandard = DI_VIDEO_SD_STANDARD_NTSC;
#elif defined(CONFIG_SDOUT_NTSC_JAPAN)
	pstOuputCapInfo->eVideoSDStandard = DI_VIDEO_SD_STANDARD_NTSC_J;
#else
	pstOuputCapInfo->eVideoSDStandard = DI_VIDEO_SD_STANDARD_PAL;
#endif
done:
	PrintExit();

	return rc;
}
#endif

DI_ERR_CODE DI_VIDEO_GetCurrentDecoderStatus(HUINT32 ulDeviceId, DI_VIDEO_DECODER_STATUS_t *pstVideoDecoderStatus)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	NEXUS_Error unNexusErr = NEXUS_SUCCESS;
	NEXUS_VideoDecoderStatus decoderStatus;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(pstVideoDecoderStatus == NULL)
	{
		PrintError("[%s] pstVideoDecoderStatus is NULL\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

	unNexusErr = NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder, &decoderStatus);
	if (unNexusErr)
	{
		PrintDebug("[%d]%s err from NEXUS_VideoDecoder_GetStatus",__LINE__, __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}
	pstVideoDecoderStatus->eProfile = P_VIDEO_Convert2DIProfile(decoderStatus.protocolProfile);
	pstVideoDecoderStatus->eLevel = P_VIDEO_Convert2DILevel(decoderStatus.protocolLevel);
	pstVideoDecoderStatus->ulDepth = decoderStatus.fifoDepth;
	pstVideoDecoderStatus->ulTotalSize = decoderStatus.fifoSize;

done:
	DI_VIDEO_REL_SEMA(ulDeviceId);
	PrintExit();

	return rc;
}

DRV_Error DRV_VIDEO_StartMosaic(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec)
{
	NEXUS_Error nexusError = NEXUS_SUCCESS;
	DRV_Error nDrvErr = DRV_OK;
	NEXUS_VideoDecoderStartSettings *startSettings;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoWindowMosaicSettings mosaicSettings;
	NEXUS_DisplayHandle display;
	NEXUS_DisplaySettings displaySettings;
	HUINT32 ulWindowId = ulDeviceId;
	HUINT16 usWidth = 0;
	HUINT16 usHeight = 0;
	ST_VIDEO_MSG VideoMonMsg;
	int i = 0, j = 0;
	HBOOL bStartMosaic = FALSE;

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].bDecState == TRUE)
	{
		PrintError("video decoder is already started!!!\n");
		nDrvErr=DRV_ERR;
		goto done;
	}

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		nexusError = NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("ERR NEXUS_VideoWindow_RemoveInput(HD_DISP) 0x%x!!!\n",nexusError);
			nDrvErr=DRV_ERR;
			goto done;

		}
		else
			bStartMosaic = TRUE;
	}

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
	{
		nexusError = NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);
		if(nexusError != NEXUS_SUCCESS)
		{
			NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);
			PrintError("ERR NEXUS_VideoWindow_RemoveInput(SD_DISP) 0x%x!!!\n",nexusError);
			nDrvErr=DRV_ERR;
			goto done;

		}
		else
			bStartMosaic = TRUE;
	}

	if(bStartMosaic == TRUE)
		NEXUS_VideoInput_Shutdown(s_stVideo[ulDeviceId].hInput);

	for(i = 0; i < DI_NUM_DISPLAYS; i++)
	{
		display = s_stVideo[ulDeviceId].hDisplay[i];
		if(display != NULL)
		{
			NEXUS_Display_GetSettings(display, &displaySettings);
			P_VIDEO_GetDisplaySize(displaySettings.format, &usWidth, &usHeight);

			NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[i][ulWindowId], &windowSettings);
			windowSettings.position.x = 0;
			windowSettings.position.y = 0;
			if(i == HD_DISP)
			{
				windowSettings.position.width = usWidth;
				windowSettings.position.height = usHeight;
			}
			else
			{
				windowSettings.position.width = SD_DISP_WIDTH;
				windowSettings.position.height = SD_DISP_HEIGHT;
			}
			windowSettings.visible = true;
			NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[i][ulWindowId], &windowSettings);

			for(j = 0; j < DI_NUM_MOSAIC_WINDOWS; j++)
			{
				s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i] = NEXUS_VideoWindow_OpenMosaic(s_stVideo[ulDeviceId].hWindow[i][ulWindowId], j);
				if(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i] !=  NULL)
				{
					NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i], &windowSettings);
					if(i == HD_DISP)
					{
						windowSettings.position.x = ((j%8)*((usWidth/8))) & ~0x01;
						windowSettings.position.y = ((j%8)*((usHeight/8))) & ~0x01;
						windowSettings.position.width = ((usWidth)/8) & ~0x01;
						windowSettings.position.height = ((usHeight)/8) & ~0x01;
					}
					else
					{
						windowSettings.position.x = ((j%8)*((SD_DISP_WIDTH/8))) & ~0x01;
						windowSettings.position.y = ((j%8)*((SD_DISP_HEIGHT/8))) & ~0x01;
						windowSettings.position.width = ((SD_DISP_WIDTH)/8) & ~0x01;
						windowSettings.position.height = ((SD_DISP_HEIGHT)/8) & ~0x01;
					}
					windowSettings.visible = false;

					NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i], &windowSettings);

					NEXUS_VideoWindow_GetMosaicSettings(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i], &mosaicSettings);
					mosaicSettings.backendMosaic.enabled = true;
					mosaicSettings.backendMosaic.clipRect.x = ((j%8)*((1920/*usWidth*/)/8)) & ~0x01;
					mosaicSettings.backendMosaic.clipRect.y = ((j%8)*((1080/*usHeight*/)/8)) & ~0x01;
					mosaicSettings.backendMosaic.clipRect.width = ((1920/*usWidth*/)/8) & ~0x01;
					mosaicSettings.backendMosaic.clipRect.height = ((1080/*usHeight*/)/8) & ~0x01;
					mosaicSettings.backendMosaic.clipBase.width = 1920/*usWidth*/;
					mosaicSettings.backendMosaic.clipBase.height = 1080/*usHeight*/;
					NEXUS_VideoWindow_SetMosaicSettings(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i], &mosaicSettings);

					NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[i], s_stVideo[ulDeviceId].hInput);
					s_stVideo[ulDeviceId].stMosaic[j].bOpen = TRUE;
					s_stVideo[ulDeviceId].stMosaic[j].bUsed = FALSE;
				}

			}
		}
	}

	if(bStartMosaic == TRUE)
	{
		startSettings = &s_stVideo[ulDeviceId].StartSettings;
		/* Open the pid channels */
		NEXUS_VideoDecoder_GetDefaultStartSettings(startSettings);

		startSettings->codec = P_VIDEO_Convert2NexusCodec(VideoCodec);
		startSettings->pidChannel = DRV_DEMUX_GetVideoPidChannel(ulDeviceId);
		if(startSettings->pidChannel == NULL)
		{
			PrintError("[%s] VideoPidChannel[%d] is NULL\n",__func__,ulDeviceId);
			nDrvErr = DRV_ERR;
			goto resource;
		}

		startSettings->stcChannel = NULL;

		PrintDebug("[%s] codec=%d  pidChannel=%d\n",__func__,startSettings->codec,(unsigned int)startSettings->pidChannel);
		nexusError = NEXUS_VideoDecoder_Start(s_stVideo[ulDeviceId].hDecoder , startSettings);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s->NEXUS_VideoDecoder_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
			nDrvErr = DRV_ERR;
			goto resource;
		}
		else
		{
			PrintDebug("%s->NEXUS_VideoDecoder_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
			s_stVideo[ulDeviceId].bDecState=TRUE;
		}

		VideoMonMsg.ulParam0 = DI_VIDEO_RUNNING_STATE_UNKNOWN;
		VK_MSG_Send(s_stVideo[ulDeviceId].ulVideoMonMsgQueueId, &VideoMonMsg, sizeof(VideoMonMsg));
		s_stVideo[ulDeviceId].bMosaicState = TRUE;
	}

	PrintExit();
	DRV_VIDEO_REL_SEMA(ulDeviceId);

	return nDrvErr;

resource:
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);
done:
	PrintExit();
	DRV_VIDEO_REL_SEMA(ulDeviceId);

	return nDrvErr;
}

DRV_Error DRV_VIDEO_StopMosaic(HUINT32 ulDeviceId)
{
	DRV_Error	nDrvErr = DRV_OK;
	DI_VIDEO_RECT_t *stDesiredHdOutRect = &s_stVideoInfo[ulDeviceId].stDesiredHdOutRect;
	DI_VIDEO_RECT_t *stDesiredHdInRect = &s_stVideoInfo[ulDeviceId].stDesiredHdInRect;
	DI_VIDEO_RECT_t *stDesiredSdOutRect = &s_stVideoInfo[ulDeviceId].stDesiredSdOutRect;
	DI_VIDEO_RECT_t *stDesiredSdInRect = &s_stVideoInfo[ulDeviceId].stDesiredSdInRect;
	HUINT32 ulWindowId = ulDeviceId;
	int j;

	PrintEnter();

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].bDecState == FALSE)
	{
		PrintError("video decoder is already stopped!!!\n");
		nDrvErr=DRV_ERR;
		goto done;
	}

	NEXUS_VideoDecoder_Stop(s_stVideo[ulDeviceId].hDecoder);

	for(j = 0; j < DI_NUM_MOSAIC_WINDOWS; j++)
	{
		if(s_stVideo[ulDeviceId].stMosaic[j].bOpen == TRUE)
		{
			if(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[HD_DISP] != NULL)
				NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[HD_DISP], s_stVideo[ulDeviceId].hInput);

			if(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[SD_DISP] != NULL)
				NEXUS_VideoWindow_RemoveInput(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[SD_DISP], s_stVideo[ulDeviceId].hInput);

			if(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[HD_DISP] != NULL)
				NEXUS_VideoWindow_Close(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[HD_DISP]);

			if(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[SD_DISP] != NULL)
				NEXUS_VideoWindow_Close(s_stVideo[ulDeviceId].stMosaic[j].hMosaicWindow[SD_DISP]);

			s_stVideo[ulDeviceId].stMosaic[j].bOpen = FALSE;
			s_stVideo[ulDeviceId].stMosaic[j].bUsed = FALSE;
		}
	}

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
		NEXUS_VideoWindow_AddInput(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], s_stVideo[ulDeviceId].hInput);

	/* If video is stopped, then afd info must be initialized.
	 * That means when every channel changed, AFD info must be notified to MW,
	 * even if previous afd and current afd are the same.
	 */
	s_stVideoInfo[ulDeviceId].afd = 0;
	s_stVideoInfo[ulDeviceId].eAspectRatio = DI_VIDEO_ASPECTRATIO_MAX;
	s_stVideo[ulDeviceId].bFirstPtsPassed = FALSE;
	s_stVideo[ulDeviceId].bDecState=FALSE;
	s_stVideo[ulDeviceId].bDecoding=FALSE;
#if defined(CONFIG_MEDIA)
	s_stVideo[ulDeviceId].bIsMedia=FALSE;
#endif
	/* When Video is stopped, local rect info must be clear.
	*/
	VK_MEM_Memset(stDesiredHdOutRect,0xFF,sizeof(DI_VIDEO_RECT_t));
	VK_MEM_Memset(stDesiredHdInRect,0xFF,sizeof(DI_VIDEO_RECT_t));
	VK_MEM_Memset(stDesiredSdOutRect,0xFF,sizeof(DI_VIDEO_RECT_t));
	VK_MEM_Memset(stDesiredSdInRect,0xFF,sizeof(DI_VIDEO_RECT_t));

#if defined(WORKAROUND_DEC_RESET) || defined(CONFIG_WORKAROUND_DEC_RESTART)
	s_stVideo[ulDeviceId].ulResetTimerId=VK_TIMERID_NULL;
	s_stVideo[ulDeviceId].bDecReset=FALSE;
#endif
done:
	s_stVideo[ulDeviceId].bMosaicState = FALSE;

	PrintExit();
	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return nDrvErr;

}

DRV_Error DRV_VIDEO_GetMosaicWindowIndex(HUINT32 ulDeviceId, HUINT32 ulMosaicWindowIndex,HUINT32 *pIndex)
{
	HUINT32 ulIndex =0;

	if(pIndex == NULL)
	{
		PrintError("DRV_VIDEO_GetMosaicWindowIndex pIndex is NULL!!!\n");
		return DRV_ERR;
	}

	DRV_VIDEO_GET_SEMA(ulDeviceId);
	for(ulIndex = 0 ; ulIndex < DI_NUM_MOSAIC_WINDOWS ; ulIndex++)
	{
		if((s_stVideo[ulDeviceId].stMosaic[ulIndex].bOpen == TRUE) && (s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed == TRUE)
			&& (s_stVideo[ulDeviceId].stMosaic[ulIndex].ulIndex == ulMosaicWindowIndex))
		{
			*pIndex = ulIndex;
			DRV_VIDEO_REL_SEMA(ulDeviceId);
			return DRV_OK;
		}
	}

	PrintError("DRV_VIDEO_GetMosaicWindowIndex index is not found ulMosaicWindowIndex = %d !!!\n",ulMosaicWindowIndex);
	for(ulIndex = 0 ; ulIndex < DI_NUM_MOSAIC_WINDOWS ; ulIndex++)
	{
		PrintError("s_stVideo[%d].stMosaic[%d].bOpen = 0x%x \n",ulDeviceId,ulIndex,s_stVideo[ulDeviceId].stMosaic[ulIndex].bOpen);
		PrintError("s_stVideo[%d].stMosaic[%d].bUsed = 0x%x \n",ulDeviceId,ulIndex,s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed);
		PrintError("s_stVideo[%d].stMosaic[%d].ulIndex = 0x%x \n",ulDeviceId,ulIndex,s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed);
	}

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_ERR;
}

DRV_Error DRV_VIDEO_GetMosaicWindowFreeIndex(HUINT32 ulDeviceId, HUINT32 ulMosaicWindowIndex,HUINT32 *pIndex)
{
	HUINT32 ulIndex =0;

	if(pIndex == NULL)
	{
		PrintError("DRV_VIDEO_GetMosaicWindowFreeIndex pIndex is NULL!!!\n");
		return DRV_ERR;
	}

	DRV_VIDEO_GET_SEMA(ulDeviceId);
	for(ulIndex = 0 ; ulIndex < DI_NUM_MOSAIC_WINDOWS ; ulIndex++)
	{
		if((s_stVideo[ulDeviceId].stMosaic[ulIndex].bOpen == TRUE) && (s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed == TRUE)
			&& (s_stVideo[ulDeviceId].stMosaic[ulIndex].ulIndex == ulMosaicWindowIndex))
		{
			*pIndex = ulIndex;
			DRV_VIDEO_REL_SEMA(ulDeviceId);
			return DRV_OK;
		}
	}

	for(ulIndex = 0 ; ulIndex < DI_NUM_MOSAIC_WINDOWS ; ulIndex++)
	{
		if(s_stVideo[ulDeviceId].stMosaic[ulIndex].bOpen == TRUE && s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed == FALSE)
		{
			*pIndex = ulIndex;
			DRV_VIDEO_REL_SEMA(ulDeviceId);
			return DRV_OK;
		}
	}

	PrintError("DRV_VIDEO_GetMosaicWindowFreeIndex index is not found ulMosaicWindowIndex = %d !!!\n",ulMosaicWindowIndex);
	for(ulIndex = 0 ; ulIndex < DI_NUM_MOSAIC_WINDOWS ; ulIndex++)
	{
		PrintError("s_stVideo[%d].stMosaic[%d].bOpen = 0x%x \n",ulDeviceId,ulIndex,s_stVideo[ulDeviceId].stMosaic[ulIndex].bOpen);
		PrintError("s_stVideo[%d].stMosaic[%d].bUsed = 0x%x \n",ulDeviceId,ulIndex,s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed);
		PrintError("s_stVideo[%d].stMosaic[%d].ulIndex = 0x%x \n",ulDeviceId,ulIndex,s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed);
	}

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_ERR;
}

DRV_Error DRV_VIDEO_SetMosaicWindowInfo(HUINT32 ulDeviceId, HUINT32 ulIndex, HUINT32 ulMosaicWindowIndex,DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pOutRect, DI_VIDEO_WIN_TYPE eVidWin)
{
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	s_stVideo[ulDeviceId].stMosaic[ulIndex].bUsed = TRUE;
	s_stVideo[ulDeviceId].stMosaic[ulIndex].ulIndex = ulMosaicWindowIndex;
	VK_memcpy(&s_stVideo[ulDeviceId].stMosaic[ulIndex].stInRect,pInRect,sizeof(DI_VIDEO_RECT_t));
	VK_memcpy(&s_stVideo[ulDeviceId].stMosaic[ulIndex].stOutRect,pOutRect,sizeof(DI_VIDEO_RECT_t));
	s_stVideo[ulDeviceId].stMosaic[ulIndex].eVidWin = eVidWin;

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DI_ERR_CODE DI_VIDEO_StartMosaic(HUINT32 ulDeviceId, DI_VIDEO_FORMAT VideoCodec)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
	DRV_Error	nDrvErr=DRV_OK;
#if (NEXUS_VERSION >= 1502)
	HUINT32 ulWindowId = ulDeviceId;
#endif
	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if (NEXUS_VERSION >= 1502)
	NEXUS_VideoWindowMadSettings madSettings;
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
		if(madSettings.deinterlace == true)
		{
			madSettings.deinterlace=false;
			NEXUS_VideoWindow_SetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
		}
	}
#endif

	nDrvErr=DRV_VIDEO_StartMosaic(ulDeviceId, VideoCodec);
	if(nDrvErr == DRV_ERR)
	{
		PrintError("[%s] Video Mosaic Start Error\n",__func__);
		rc=DI_ERR_ERROR;
		goto done;
	}
	else
	{
		s_stVideo[ulDeviceId].eVidCodec=VideoCodec;
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}

DI_ERR_CODE DI_VIDEO_StopMosaic(HUINT32 ulDeviceId)
{
	DI_ERR_CODE	rc = DI_ERR_OK;
	DRV_Error	nDrvErr=DRV_OK;
#if (NEXUS_VERSION >= 1502)
	HUINT32 ulWindowId = ulDeviceId;
#endif

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	nDrvErr = DRV_VIDEO_StopMosaic(ulDeviceId);
	if(nDrvErr == DRV_ERR)
	{
		PrintError("[%s] Video Mosaic Stop Error\n",__func__);
		rc=DI_ERR_ERROR;
	}
#if (NEXUS_VERSION >= 1502)
	NEXUS_VideoWindowMadSettings madSettings;
	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
		if(madSettings.deinterlace == false)
		{
			madSettings.deinterlace=true;
			NEXUS_VideoWindow_SetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
		}
	}
#endif

	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}


DI_ERR_CODE DI_VIDEO_ShowMosaicWindow(HUINT32 ulDeviceId, HUINT32 ulMosaicWindowIndex, HBOOL bShow)
{
	DI_ERR_CODE	rc = DI_ERR_OK;
	NEXUS_VideoWindowSettings windowSettings;
	DRV_Error	nDrvErr=DRV_OK;
	HUINT32 ulWindowIndex = 0;
	DI_VIDEO_WIN_TYPE eVidWin;
	DI_VIDEO_RECT_t stInRect, stOutRect;
	DI_VIDEO_StreamInfo_t stream_info;
	NEXUS_VideoWindowMosaicSettings mosaicSettings;
	NEXUS_Error nexusError = NEXUS_SUCCESS;
	NEXUS_DisplaySettings displaySettings;
	HUINT16 usHdDigitalWidth = 0;
	HUINT16 usHdDigitalHeight = 0;
	DI_VIDEO_RECT_t stSdWinOutRect;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	nDrvErr = DRV_VIDEO_GetMosaicWindowIndex(ulDeviceId,ulMosaicWindowIndex,&ulWindowIndex);
	if(nDrvErr != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetMosaicWindowIndex ERR!!!\n");
		rc = DI_ERR_NO_RESOURCE;
		goto done;
	}

	eVidWin = s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].eVidWin;
	VK_memcpy(&stInRect,&s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].stInRect,sizeof(DI_VIDEO_RECT_t));
	VK_memcpy(&stOutRect,&s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].stOutRect,sizeof(DI_VIDEO_RECT_t));

	if(s_stVideo[ulDeviceId].hDisplay[HD_DISP] != NULL)
	{
		NEXUS_Display_GetSettings(s_stVideo[ulDeviceId].hDisplay[HD_DISP], &displaySettings);
		P_VIDEO_GetDisplaySize(displaySettings.format, &usHdDigitalWidth, &usHdDigitalHeight);
	}
	else
	{
		usHdDigitalWidth = SD_DISP_WIDTH;
		usHdDigitalHeight = SD_DISP_HEIGHT;
	}

	if (eVidWin & DI_VIDEO_WIN_HD)
	{
		if(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[HD_DISP] !=  NULL)
		{
			P_VIDEO_CopySeqHeaderInfo(ulDeviceId, &stream_info);

			NEXUS_VideoWindow_GetMosaicSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[HD_DISP], &mosaicSettings);

			mosaicSettings.backendMosaic.enabled = true;
			mosaicSettings.backendMosaic.clipBase.x = 0;
			mosaicSettings.backendMosaic.clipBase.y = 0;
			mosaicSettings.backendMosaic.clipBase.width = (HUINT16)stream_info.ulWidth;
			mosaicSettings.backendMosaic.clipBase.height = (HUINT16)stream_info.ulHeight;

			if ((stInRect.ulX == 0) && (stInRect.ulY == 0) && (stInRect.ulWidth == 0) && (stInRect.ulHeight == 0))
			{
				mosaicSettings.backendMosaic.clipRect.x = 0;
				mosaicSettings.backendMosaic.clipRect.y = 0;
				mosaicSettings.backendMosaic.clipRect.width = (HUINT16)stream_info.ulWidth;
				mosaicSettings.backendMosaic.clipRect.height = (HUINT16)stream_info.ulHeight;
			}
			else
			{
#if defined(CONFIG_INRECT_RATIO)
				mosaicSettings.backendMosaic.clipRect.x = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulX, (HUINT16)stream_info.ulWidth);
				mosaicSettings.backendMosaic.clipRect.y = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulY, (HUINT16)stream_info.ulHeight);
				mosaicSettings.backendMosaic.clipRect.width = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulWidth, (HUINT16)stream_info.ulWidth);
				mosaicSettings.backendMosaic.clipRect.height = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulHeight, (HUINT16)stream_info.ulHeight);
#else
				mosaicSettings.backendMosaic.clipRect.x = (HUINT16)stInRect.ulX;
				mosaicSettings.backendMosaic.clipRect.y = (HUINT16)stInRect.ulY;
				mosaicSettings.backendMosaic.clipRect.width = (HUINT16)stInRect.ulWidth;
				mosaicSettings.backendMosaic.clipRect.height = (HUINT16)stInRect.ulHeight;
#endif
				if(mosaicSettings.backendMosaic.clipRect.x%2)
				{
					mosaicSettings.backendMosaic.clipRect.x++;
				}
				if(mosaicSettings.backendMosaic.clipRect.y%2)
				{
					mosaicSettings.backendMosaic.clipRect.y++;
				}
				mosaicSettings.backendMosaic.clipRect.width = mosaicSettings.backendMosaic.clipRect.width & ~0x01;
				mosaicSettings.backendMosaic.clipRect.height = mosaicSettings.backendMosaic.clipRect.height & ~0x01;
			}

			nexusError = NEXUS_VideoWindow_SetMosaicSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[HD_DISP], &mosaicSettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] %dth window can't set In-Out window\n",__func__, ulWindowIndex);
				rc = DI_ERR_ERROR;
				goto done;
			}

			NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[HD_DISP], &windowSettings);

			if (((stOutRect.ulX + stOutRect.ulWidth) > usHdDigitalWidth) || ((stOutRect.ulY + stOutRect.ulHeight) > usHdDigitalHeight))
			{
				/* Out of range */
				PrintError("[%s] %dth mosaic window, OutWindow is out of range, (%d,%d)-(%d,%d)\n",
						__func__, ulWindowIndex, stOutRect.ulX, stOutRect.ulY, stOutRect.ulWidth, stOutRect.ulHeight);
				PrintError("[%s] Digital Width,Height : (%d,%d)\n", __func__, usHdDigitalWidth, usHdDigitalHeight);
				rc = DI_ERR_ERROR;
				goto done;
			}

			if ((stOutRect.ulX == 0) && (stOutRect.ulY == 0) && (stOutRect.ulWidth == 0) && (stOutRect.ulHeight == 0))
			{
				/* Full window */
				windowSettings.position.x= 0;
				windowSettings.position.y = 0;
				windowSettings.position.width = usHdDigitalWidth;
				windowSettings.position.height = usHdDigitalHeight;
			}
			else
			{
				if(stOutRect.ulX%2)
				{
					windowSettings.position.x = stOutRect.ulX+1;
				}
				else
				{
					windowSettings.position.x = stOutRect.ulX;
				}
				if(stOutRect.ulY%2)
				{
					windowSettings.position.y = stOutRect.ulY+1;
				}
				else
				{
					windowSettings.position.y = stOutRect.ulY;
				}
				windowSettings.position.width = (stOutRect.ulWidth) & ~0x01;
				windowSettings.position.height = (stOutRect.ulHeight) & ~0x01;
			}

			windowSettings.visible = bShow;
			nexusError = NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[HD_DISP], &windowSettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] %dth window can't set In-Out window\n",__func__, ulWindowIndex);
				rc = DI_ERR_ERROR;
				goto done;
			}
		}
	}

 	if(eVidWin & DI_VIDEO_WIN_SD)
	{
		if(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[SD_DISP] != NULL)
		{
			P_VIDEO_CopySeqHeaderInfo(ulDeviceId, &stream_info);
			NEXUS_VideoWindow_GetMosaicSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[SD_DISP], &mosaicSettings);

			mosaicSettings.backendMosaic.enabled = true;

			mosaicSettings.backendMosaic.clipBase.x = 0;
			mosaicSettings.backendMosaic.clipBase.y = 0;
			mosaicSettings.backendMosaic.clipBase.width = (HUINT16)stream_info.ulWidth;
			mosaicSettings.backendMosaic.clipBase.height = (HUINT16)stream_info.ulHeight;

			if ((stInRect.ulX == 0) && (stInRect.ulY == 0) && (stInRect.ulWidth == 0) && (stInRect.ulHeight == 0))
			{
				mosaicSettings.backendMosaic.clipRect.x = 0;
				mosaicSettings.backendMosaic.clipRect.y = 0;
				mosaicSettings.backendMosaic.clipRect.width = (HUINT16)stream_info.ulWidth;
				mosaicSettings.backendMosaic.clipRect.height = (HUINT16)stream_info.ulHeight;
			}
			else
			{
#if defined(CONFIG_INRECT_RATIO)
				mosaicSettings.backendMosaic.clipRect.x = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulX, (HUINT16)stream_info.ulWidth);
				mosaicSettings.backendMosaic.clipRect.y = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulY, (HUINT16)stream_info.ulHeight);
				mosaicSettings.backendMosaic.clipRect.width = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulWidth, (HUINT16)stream_info.ulWidth);
				mosaicSettings.backendMosaic.clipRect.height = P_VIDEO_CalcPixelPosition(0, (HUINT16)stInRect.ulHeight, (HUINT16)stream_info.ulHeight);
#else
				mosaicSettings.backendMosaic.clipRect.x = (HUINT16)stInRect.ulX;
				mosaicSettings.backendMosaic.clipRect.y = (HUINT16)stInRect.ulY;
				mosaicSettings.backendMosaic.clipRect.width = (HUINT16)stInRect.ulWidth;
				mosaicSettings.backendMosaic.clipRect.height = (HUINT16)stInRect.ulHeight;
#endif
				if(mosaicSettings.backendMosaic.clipRect.x%2)
				{
					mosaicSettings.backendMosaic.clipRect.x++;
				}
				if(mosaicSettings.backendMosaic.clipRect.y%2)
				{
					mosaicSettings.backendMosaic.clipRect.y++;
				}
				mosaicSettings.backendMosaic.clipRect.width = mosaicSettings.backendMosaic.clipRect.width & ~0x01;
				mosaicSettings.backendMosaic.clipRect.height = mosaicSettings.backendMosaic.clipRect.height & ~0x01;
			}

			nexusError = NEXUS_VideoWindow_SetMosaicSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[SD_DISP], &mosaicSettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] %dth window can't set In-Out window\n",__func__, ulWindowIndex);
				rc = DI_ERR_ERROR;
				goto done;
			}

			stSdWinOutRect.ulX = (stOutRect.ulX*SD_DISP_WIDTH)/usHdDigitalWidth;
			stSdWinOutRect.ulY = (stOutRect.ulY*SD_DISP_HEIGHT)/usHdDigitalHeight;
			stSdWinOutRect.ulWidth = (stOutRect.ulWidth*SD_DISP_WIDTH)/usHdDigitalWidth;
			stSdWinOutRect.ulHeight = (stOutRect.ulHeight*SD_DISP_HEIGHT)/usHdDigitalHeight;
			NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[SD_DISP], &windowSettings);

			if (((stSdWinOutRect.ulX + stSdWinOutRect.ulWidth) > SD_DISP_WIDTH) || ((stSdWinOutRect.ulY + stSdWinOutRect.ulHeight) > SD_DISP_HEIGHT))
			{
				/* Out of range */
				PrintError("[%s] %dth window, output window is out of range, (%d, %d)-(%d, %d)\n",
						__func__, ulWindowIndex, stSdWinOutRect.ulX, stSdWinOutRect.ulY, stSdWinOutRect.ulWidth, stSdWinOutRect.ulHeight);
				rc = DI_ERR_ERROR;
				goto done;
			}

			if ((stSdWinOutRect.ulX == 0) && (stSdWinOutRect.ulY == 0) && (stSdWinOutRect.ulWidth == 0) && (stSdWinOutRect.ulHeight == 0))
			{
				/* Full window */
				windowSettings.position.x= 0;
				windowSettings.position.y = 0;
				windowSettings.position.width = SD_DISP_WIDTH;
				windowSettings.position.height = SD_DISP_HEIGHT;
			}
			else
			{
				if(stSdWinOutRect.ulX%2)
				{
					windowSettings.position.x = stSdWinOutRect.ulX+1;
				}
				else
				{
					windowSettings.position.x = stSdWinOutRect.ulX;
				}
				if(stSdWinOutRect.ulY%2)
				{
					windowSettings.position.y = stSdWinOutRect.ulY+1;
				}
				else
				{
					windowSettings.position.y = stSdWinOutRect.ulY;
				}
				windowSettings.position.width = (stSdWinOutRect.ulWidth) & ~0x01;
				windowSettings.position.height = (stSdWinOutRect.ulHeight) & ~0x01;
			}

			windowSettings.visible = bShow;
			nexusError = NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].stMosaic[ulWindowIndex].hMosaicWindow[SD_DISP], &windowSettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] %dth window can't set In-Out window %d\n",__func__,ulWindowIndex);
				rc = DI_ERR_ERROR;
				goto done;
			}
		}
	}

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}

DI_ERR_CODE DI_VIDEO_SetMosaicWindowSettings(HUINT32 ulDeviceId, HUINT32 ulMosaicWindowIndex, DI_VIDEO_RECT_t *pInRect, DI_VIDEO_RECT_t *pOutRect, DI_VIDEO_WIN_TYPE eVidWin)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	HUINT32 ulWindowIndex = 0;
	DRV_Error	nDrvErr=DRV_OK;

	PrintEnter();
	if(ulMosaicWindowIndex >= DI_NUM_MOSAIC_WINDOWS)
	{
		PrintError("ulMosaicWindowIndex ERR %d!!!\n",ulMosaicWindowIndex);
		rc = DI_ERR_INVALID_PARAM;
		goto done;
	}

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	nDrvErr = DRV_VIDEO_GetMosaicWindowFreeIndex(ulDeviceId,ulMosaicWindowIndex,&ulWindowIndex);
	if(nDrvErr != DRV_OK)
	{
		PrintError("[%s] No resource %d\n",__func__,ulMosaicWindowIndex);
		rc = DI_ERR_NO_RESOURCE;
		goto done;
	}

	DRV_VIDEO_SetMosaicWindowInfo(ulDeviceId,ulWindowIndex,ulMosaicWindowIndex,pInRect,pOutRect,eVidWin);

done:
	PrintExit();
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return rc;
}

DI_ERR_CODE DI_VIDEO_GetVideoPts(HUINT32 *ulPts)
{
	NEXUS_VideoDecoderStatus decoderStatus;
	NEXUS_Error unNexusErr = NEXUS_SUCCESS;
	HUINT32 ulDeviceId = DEF_VID_INSTANCE;

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	unNexusErr = NEXUS_VideoDecoder_GetStatus(s_stVideo[ulDeviceId].hDecoder, &decoderStatus);
	if (unNexusErr)
	{
		PrintDebug("[%d]%s err from NEXUS_VideoDecoder_GetStatus",__LINE__, __func__);
		DI_VIDEO_REL_SEMA(ulDeviceId);
		return DI_ERR_ERROR;
	}

	if(ulPts)
		*ulPts = decoderStatus.pts;

	DI_VIDEO_REL_SEMA(ulDeviceId);

	return DI_ERR_OK;
}
#endif

DRV_Error DRV_VIDEO_GetDisplayHandle(HUINT32 ulDeviceId, NEXUS_DisplayHandle *dispHD, NEXUS_DisplayHandle *dispSD)
{
	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);
	*dispHD = s_stVideo[ulDeviceId].hDisplay[HD_DISP];
	*dispSD = s_stVideo[ulDeviceId].hDisplay[SD_DISP];
	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DRV_Error DRV_VIDEO_GetWindowHandle(HUINT32 ulDeviceId, NEXUS_VideoWindowHandle *winHD, NEXUS_VideoWindowHandle *winSD)
{
	HUINT32 ulWindowId = ulDeviceId;

	DRV_VIDEO_CHECK_INIT(ulDeviceId);
	DRV_VIDEO_GET_SEMA(ulDeviceId);

	*winHD = s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId];
	*winSD = s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId];

	DRV_VIDEO_REL_SEMA(ulDeviceId);
	return DRV_OK;
}

DRV_Error DRV_VIDEO_VBITeletext_Write(DRV_VIDEO_VBI_TT *vbi_tt_data, unsigned long lines, unsigned char polarity)
{
	NEXUS_Error nexusError;
	NEXUS_TeletextLine entry[VBI_TT_ENTRY_SIZE];
	HUINT32 i=0;
	HUINT32 retNum=0;
	HUINT32 ulDeviceId=DEF_VID_INSTANCE;
	DRV_Error rc=DRV_OK;


	UNREFENCED_PARAM(polarity);

	/* TOP Field 7~22 = 16 lines */
	/* BOTTOM Field 320~335 = 16 lines */

	if (s_stVideo[ulDeviceId].bVbiTeletext==TRUE)
	{
		if(vbi_tt_data == NULL)
		{
			PrintError("[%s] Teletext Data is NULL\n",__func__);
			rc=DRV_ERR;
			goto done;
		}

		if(lines > VBI_TT_ENTRY_SIZE)
		{
			PrintError("[%s] Teletext line size is over than maximum size(%d)\n",__func__,VBI_TT_ENTRY_SIZE);
			rc=DRV_ERR;
			goto done;
		}

		for(i=0;i<VBI_TT_ENTRY_SIZE;i++)
		{
			if(vbi_tt_data[i].framingCode != VBI_TT_INVALID_FRAMING_CODE)
			{
				entry[i].framingCode = vbi_tt_data[i].framingCode;
				VK_MEM_Memcpy(entry[i].data,vbi_tt_data[i].data,NEXUS_TELETEXT_LINESIZE);
			}
			else
			{
				entry[i].framingCode = VBI_TT_INVALID_FRAMING_CODE;
			}

			entry[i].lineNumber = i;

			if(polarity == 1)
				entry[i].topField=false; /* Bottom field */
			else
				entry[i].topField=true; /* Top field */
		}
		if(s_stVideo[ulDeviceId].hDisplay[SD_DISP] != NULL)
		{
			nexusError = NEXUS_Display_WriteTeletext(s_stVideo[ulDeviceId].hDisplay[SD_DISP], entry, VBI_TT_ENTRY_SIZE, (size_t *)&retNum);
			if (nexusError != NEXUS_SUCCESS)
			{
				PrintError("[%s] Write SD Teletext error\n",__func__);
				rc=DRV_ERR;
				goto done;
			}
		}
	}

done:
	return rc;
}

NEXUS_VideoFormat P_VIDEO_NvramConvert2NexusVideoFormat(DI_NVRAM_VIDEO_RESOLUTION_E eRes, DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E eRate)
{
	HUINT32 i=0;

	for(i=0;i<sizeof(stNvramVideoFormatTable)/sizeof(stNvramVideoFormatTable[0]);i++)
	{
		if((eRes == stNvramVideoFormatTable[i].eRes) && (eRate == stNvramVideoFormatTable[i].eRate))
			return stNvramVideoFormatTable[i].nexusVideoFormat;
	}
#if defined(CONFIG_HDOUT_1080I_60HZ)
	return NEXUS_VideoFormat_e1080i;
#else
#if defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
	return NEXUS_VideoFormat_e1080p60hz;
#else
	return NEXUS_VideoFormat_e1080i50hz;
#endif
#endif
}

#if NEXUS_NUM_COMPOSITE_OUTPUTS > 0 || NEXUS_NUM_SVIDEO_OUTPUTS > 0 || NEXUS_NUM_COMPONENT_OUTPUTS > 0
static void *P_VIDEO_GetOutputHandle(P_VIDEO_OUTPUT_MODE outputMode)
{
	if(outputMode == P_VIDEO_OUTPUT_MODE_COMPOSITE)
	{
		if ( platformConfig.outputs.composite[0] )
			return (void *)platformConfig.outputs.composite[0];
	}
#if NEXUS_NUM_COMPOSITE_OUTPUTS >= 2
	else if(outputMode == P_VIDEO_OUTPUT_MODE_COMPOSITE2)
	{
		if ( platformConfig.outputs.composite[1] )
			return (void *)platformConfig.outputs.composite[1];
	}
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS >= 1
	else if(outputMode == P_VIDEO_OUTPUT_MODE_SVIDEO)
	{
		if ( platformConfig.outputs.svideo[0] )
			return (void *)platformConfig.outputs.svideo[0];
	}
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	else if(outputMode == P_VIDEO_OUTPUT_MODE_COMPONENT)
	{
		if ( platformConfig.outputs.component[0] )
			return (void *)platformConfig.outputs.component[0];
	}
#endif
	return NULL;
}
#endif

DI_ERR_CODE DI_VIDEO_GetDecoderHandle(HUINT32 ulDeviceId,void *handle)
{

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

#if defined(CONFIG_VIDEO_DECODER)
	DRV_VIDEO_GetVideoHandle(ulDeviceId,(NEXUS_VideoDecoderHandle*)handle);
#endif

	DI_VIDEO_REL_SEMA(ulDeviceId);

	HAPPY(handle);

	return DI_ERR_OK;
}


DI_ERR_CODE DI_VIDEO_GetWindowHandle(HUINT32 ulDeviceId,void *handle_hd,void *handle_sd)
{
	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	DRV_VIDEO_GetWindowHandle(ulDeviceId,(NEXUS_VideoWindowHandle*)handle_hd,(NEXUS_VideoWindowHandle*)handle_sd);

	DI_VIDEO_REL_SEMA(ulDeviceId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_SetCaptureWindow(HUINT32 ulDeviceId,HBOOL bCapture)
{
	NEXUS_VideoWindowSettings windowSettings;
	HUINT32 ulWindowId = ulDeviceId;
	NEXUS_Error nexusError = NEXUS_SUCCESS;

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	/* To prevent video blanking during SetOutWindow */
	NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);
	if(bCapture == TRUE)
	{
		windowSettings.forceCapture = true;
	}
	else
	{
		windowSettings.forceCapture = false;
	}
	nexusError = NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d NEXUS_VideoWindow_SetSettings error\n",__func__,ulWindowId);
		DI_VIDEO_REL_SEMA(ulDeviceId);
		return DI_ERR_ERROR;
	}

	NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &windowSettings);
	if(bCapture == TRUE)
	{
		windowSettings.forceCapture = true;
	}
	else
	{
		windowSettings.forceCapture = false;
	}
	nexusError = NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &windowSettings);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d NEXUS_VideoWindow_SetSettings error\n",__func__,ulWindowId);
		DI_VIDEO_REL_SEMA(ulDeviceId);
		return DI_ERR_ERROR;
	}
	DI_VIDEO_REL_SEMA(ulDeviceId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_SetGameModeWindow(HUINT32 ulDeviceId,HBOOL bGame)
{
	NEXUS_VideoWindowMadSettings madSettings;
	HUINT32 ulWindowId = ulDeviceId;
	NEXUS_Error nexusError = NEXUS_SUCCESS;

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	/* To prevent video blanking during SetOutWindow */
	NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
	if(bGame == TRUE)
	{
		madSettings.gameMode = NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial;
	}
	else
	{
		madSettings.gameMode = NEXUS_VideoWindowGameMode_eOff;
	}
	nexusError = NEXUS_VideoWindow_SetMadSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &madSettings);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d NEXUS_VideoWindow_SetMadSettings error \n",__func__,ulWindowId);
		DI_VIDEO_REL_SEMA(ulDeviceId);
		return DI_ERR_ERROR;
	}

	NEXUS_VideoWindow_GetMadSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &madSettings);
	if(bGame == TRUE)
	{
		madSettings.gameMode = NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial;
	}
	else
	{
		madSettings.gameMode = NEXUS_VideoWindowGameMode_eOff;
	}
	nexusError = NEXUS_VideoWindow_SetMadSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &madSettings);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] %d NEXUS_VideoWindow_SetMadSettings error \n",__func__,ulWindowId);
		DI_VIDEO_REL_SEMA(ulDeviceId);
		return DI_ERR_ERROR;
	}

// set low latency mode  for real-time streaming (ip-camera)
#if (NEXUS_VERSION >= 1204) && defined(CONFIG_VIDEO_DECODER)
	{
		NEXUS_VideoDecoderExtendedSettings extSettings;
		NEXUS_VideoDecoder_GetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);

		if(bGame == TRUE)
		{
#if (NEXUS_VERSION >= 1304)
		  /*
		     * Early picture delivery mode. Enabling this will permit AVD FW to deliver
		     * pictures to the display queue before they have completed decoding.
		     */
		    extSettings.earlyPictureDeliveryMode = true;
		  /*
		     * Zero delay output mode.  Enabling this will cause the decoder to deliver
		     * a picture to the display queue as soon as it's decoded. This eliminates
		     * the pipeline used for picture reordering. This requires the pictures to
		     * be encoded in display order. For example, in an MPEG stream only I's and
		     * P's are allowed; no B's. This feature is currently only implemented for
		     * MPEG video.
		     */
		     extSettings.zeroDelayOutputMode = true;
		  /*
		     * The low latency settings apply mostly to the display queue management.
		     * Using the eFast setting is best for games, and will achieve the given
		     * latency target very quickly.  However, it passes input temporal jitter
		     * directly through to the display.  As such, it is no good for most video
		     * streams.  Therefore, for video, use the eAverage mode, which introduces
		     * a low pass filter over the latency control algorithm.  This will result
		     * in smoother video in the presence of input jitter, but will have periods
		     * of higher latency while the algorithm converges on the target.
		     */
		     extSettings.lowLatencySettings.mode = NEXUS_VideoDecoderLowLatencyMode_eAverage;
		  /*
		     * This is the target latency in milliseconds.  It is rounded up to an
		     * integer multiple of the source picture time internally.  Setting this
		     * to zero does not guarantee zero latency, and may cause oscillation.  It
		     * is best to set this to somewhere between 0 and 1 frame's time for the
		     * lowest latency setting (eFast mode), and 100 ms or so for the best jitter
		     * tolerance (eAverage mode, which translates into smoother video). However,
		     * no one number is good for everyone, as much depends on the streaming
		     * environment.  You may have to play with this number to compromise
		     * between low latency and temporal quality.
		     */
		    extSettings.lowLatencySettings.latency = 400;
#else
			extSettings.lowLatencyTsmMode = TRUE;
#endif
		}
		else
		{
#if (NEXUS_VERSION >= 1304)
			extSettings.zeroDelayOutputMode = false;
			extSettings.lowLatencySettings.mode = NEXUS_VideoDecoderLowLatencyMode_eOff;
#else
			extSettings.lowLatencyTsmMode = FALSE;
#endif
		}
		NEXUS_VideoDecoder_SetExtendedSettings(s_stVideo[ulDeviceId].hDecoder, &extSettings);
	}
#endif

	DI_VIDEO_REL_SEMA(ulDeviceId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_SetWindowVisible(HUINT32 ulDeviceId, HBOOL bVisible)
{
#if (NEXUS_VERSION>600)
	NEXUS_VideoWindowSettings	windowSettings;
	HUINT32						ulWindowId = ulDeviceId;
//	NEXUS_Error					nexusError = NEXUS_SUCCESS;

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	if(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId] != NULL)
	{
		/* To prevent video blanking during SetOutWindow */
		NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);
		windowSettings.visible = bVisible;
		PrintDebug("<%s:%d>; [HD] visible(%d)\n", __FUNCTION__, __LINE__, windowSettings.visible);
		NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[HD_DISP][ulWindowId], &windowSettings);
	}

	if(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId] != NULL)
	{
		/* To prevent video blanking during SetOutWindow */
		NEXUS_VideoWindow_GetSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &windowSettings);
		windowSettings.visible = bVisible;
		PrintDebug("<%s:%d>; [SD] visible(%d)\n", __FUNCTION__, __LINE__, windowSettings.visible);
		NEXUS_VideoWindow_SetSettings(s_stVideo[ulDeviceId].hWindow[SD_DISP][ulWindowId], &windowSettings);
	}

	DI_VIDEO_REL_SEMA(ulDeviceId);
#endif

	return DI_ERR_OK;
}

DI_ERR_CODE DI_VIDEO_DelayOutput(HUINT32 ulDeviceId, HINT32 nDelayMs)
{
	DRV_Error					dResult;
	HINT32 nSetDelayMs;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	DI_VIDEO_CHECK_INIT(ulDeviceId);
	DI_VIDEO_GET_SEMA(ulDeviceId);

	nSetDelayMs = abs(nDelayMs);
	s_stVideo[ulDeviceId].nUserOffset = nSetDelayMs;

	if(s_stVideo[ulDeviceId].bDecState == TRUE)
	{
		dResult = P_VIDEO_DelayDecoder(ulDeviceId, nSetDelayMs);
		if(dResult != DRV_OK)
		{
			PrintError("P_VIDEO_DelayDecoder failed!\n");
			rc = DI_ERR_ERROR;
			goto done;
		}
	}
	
done:
	DI_VIDEO_REL_SEMA(ulDeviceId);
	PrintExit();

	return rc;
}

/* end of file */

