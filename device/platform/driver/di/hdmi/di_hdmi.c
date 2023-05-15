/**
* di_hdmi.c
*/

/**
* @defgroup		DI_HDMI DI_HDMI : DI_HDMI module
* @author			Chung Dong Chul
* @modified by		Sun-woo Lee
* @note			DI_HDMI APIs
* @brief			Define DI_HDMI APIs
* @file 			di_hdmi.c
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

#include "linden_trace.h"

#if defined(CONFIG_HDCP_22_PROVISIONING)
#include "bsagelib_types.h"
#include "bsagelib_tools.h"

#include "sage_srai.h"
#include "sage_provisioning_host.h"
#include "sage_manufacturing_module_ids.h"

#include "drm_metadata_tl.h"
#endif
#include "nexus_platform.h"
#include "nexus_hdmi_output_hdcp.h"
#if defined(CONFIG_HDMI_CEC)
#include "nexus_cec.h"
#endif
#if defined(CONFIG_HDMI_IN)
#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_hdcp.h"
#endif
#include "nexus_display_types.h"
#include "nexus_display.h"
#include "nexus_audio_types.h"

#include "di_err.h"
#include "drv_err.h"
#include "drv_flash.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "drv_hdmi.h"
#include "drv_nvram.h"
#include "di_hdmi.h"
#include "di_nvram.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_pm.h"
#include "di_uart.h"
#include "vkernel.h"
#include "taskdef.h"
#if defined(CONFIG_DI20)
#include "cmdtool.h"
#include "nvram_map.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE TRACE_MODULE_DI_HDMI

#if defined(CONFIG_DEBUG)
//#define DEBUG_HDMI_FULLLOG
//#define DEBUG_HDMI_SEE_PARAMETERS
#endif
#define PARAM_COLOR_ON		"\033[1;31;40m"
#define PARAM_COLOR_OFF	"\033[00m"

#if defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#undef CONFIG_DIRECTFB
#endif

//#define WORKAROUND_RXSENSE_POLLING


// work around pop noise on SHARP TV LC-32GD7 (B2B SW1Team)
// http://svn.humaxdigital.com:3000/issues/59098
#define WORKAROUND_HDMI_POPNOISE_SHARP_LC_32GD7



#if (NEXUS_VERSION >= 1200)
// 7231 V50
// 7425 V60(NGS)
// 7425 V65(HMS1000S)
#define RX_POWERDOWNMODE

#elif(NEXUS_VERSION == 1000)
//7358
#define RX_POWERDOWNMODE

#endif
//#define RX_POWERDOWNMODE


#if (NEXUS_VERSION == 1290) || (NEXUS_VERSION == 1200) 	|| (NEXUS_VERSION == 1000) || (NEXUS_VERSION == 1401)
//1290 : Unified
//1200 : 6.0
//1000 : 4.0
#define HDMI_POWER_CONTROL
#endif

#if (NEXUS_VERSION >= 1290)
#define NEXUS_AVIINFO_FUNCTION
#endif

#define HPD_DELAY 1000


// realod validation hdcp key for factory application
#define RELOAD_VALID_HDCP_KEY

// #define WORKAROUND_HDMI_VENDOR_ID

#define DI_HDMI_STATE_CHANGE_CALLBACK	2
#define DI_HDMI_CEC_PROCESSING_TIME	(5 * 1000)
#define DI_HDMI_HDCP_RETRY_LIMIT 5000
#define DI_HDMI_HDCP_CHECK_POLLING_INTERVAL 6000
#define DI_HDMI_HDCP_CHECK_POLLING_COUNT_MAX 5

/*	NOTICE!!
*	This interval is dependent to the QuantumData firmware version.
*	100ms is the worst case value for the pass of Hdmi Compliance Test.
*	It just may work with 1000ms.
*/
#define DI_HDMI_HDCP_RETRY_COUNT_MAX 50
#if defined(WORKAROUND_RXSENSE_POLLING)
#define DI_HDMI_RXSENSE_POLLING_INTERVAL 100
#endif

#if (NEXUS_NUM_HDMI_OUTPUTS!=0)
	#define DI_HDMI_NUM_OUTPUTS  NEXUS_NUM_HDMI_OUTPUTS
#else
	#define DI_HDMI_NUM_OUTPUTS 0
#endif

#if defined(CONFIG_HDCP_22)
#define SIZEOF_DRM_BINFILE_HEADER (192)
#define SIZEOF_DYNAMIC_OFFSET_HEADER (16)

#define GET_UINT32_FROM_BUF(pBuf) \
    (((HUINT32)(((HUINT8*)(pBuf))[0]) << 24) | \
     ((HUINT32)(((HUINT8*)(pBuf))[1]) << 16) | \
     ((HUINT32)(((HUINT8*)(pBuf))[2]) << 8)  | \
     ((HUINT8 *)(pBuf))[3])
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


typedef enum
{
	DI_HDMI_HDCP_SUCCESS_CALLBACK=0,
	DI_HDMI_HDCP_FAILURE_CALLBACK,
	DI_HDMI_UNDEFINED
}DI_HDMI_HDCP_CONNECTION;

typedef enum
{
	DI_HDMI_HDCP_INIT=0,
	DI_HDMI_HDCP_SUCCESS,
	DI_HDMI_HDCP_BUSY,
	DI_HDMI_HDCP_FAIL
}DI_HDMI_HDCP_STATUS;

typedef enum
{
	DI_HDMI_HOTPLUG_DISCONNECTED=0x0,	/* cable disconnected state */
	DI_HDMI_HOTPLUG_CONNECTED,			/* cable connected and power up state */
	DI_HDMI_HOTPLUG_POWER_DOWN,			/* cable connected and power down state */
	DI_HDMI_HOTPLUG_MAX
} DI_HDMI_HOTPLUG_STATE_e;

#if !defined(RX_POWERDOWNMODE) /* remove */
typedef enum
{
	DI_HDMI_RXSENCE_DISCONNECTED=0x0,	/* The receiver is disconnected state */
	DI_HDMI_RXSENCE_CONNECTED,			/* The receiver is connected and powered up state */
	DI_HDMI_RXSENCE_POWER_DOWN,			/* The receiver is connected and powered down state */
	DI_HDMI_RXSENCE_MAX
} DI_HDMI_RXSENCE_STATE_e;
#endif /* end of !RX_POWERDOWNMODE */

typedef enum
{
	HDMI_MSG_EVT_HOTPLUG_DISCONNECTED,
	HDMI_MSG_EVT_HOTPLUG_CONNECTED,
	HDMI_MSG_EVT_HOTPLUG_POWER_DOWN,
#if !defined(RX_POWERDOWNMODE) /* remove */
	HDMI_MSG_EVT_RXPOWER_DISCONNECTED,
	HDMI_MSG_EVT_RXPOWER_CONNECTED,
	HDMI_MSG_EVT_RXPOWER_POWER_DOWN,
#endif /* end of !RX_POWERDOWNMODE */
	HDMI_MSG_EVT_HDCP_SUCCESS,
	HDMI_MSG_EVT_HDCP_FAIL,
	HDMI_MSG_EVT_CEC,
	HDMI_MSG_CMD_ENABLE_HDCP,
	HDMI_MSG_CMD_DISABLE_HDCP,
	HDMI_MSG_CMD_AV_BLANK,
	HDMI_MSG_CMD_AV_UNBLANK,
	HDMI_MSG_CMD_AV_MUTE,
	HDMI_MSG_CMD_AV_UNMUTE,
	HDMI_MSG_CMD_AUDIO_MUTE,
	HDMI_MSG_CMD_AUDIO_UNMUTE,
	HDMI_MSG_CMD_SEND_CEC_MSG,
	HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT,
	HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT,
	HDMI_MSG_CMD_AUXINFO,
	HDMI_MSG_CMD_COLORSPACE,
	HDMI_MSG_CMD_3D_FORMAT,
	HDMI_MSG_CMD_POWER_ON,
	HDMI_MSG_CMD_POWER_OFF,
	HDMI_MSG_SLF_HDCP_CHECK_POLLING,
	HDMI_MSG_CMD_SET_RESOLUTION,
	HDMI_MSG_MAX
} HDMI_MSG_E;

typedef struct
{
	NEXUS_HdmiOutputHandle hHdmi;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_CecHandle hCec;
#endif
	NEXUS_DisplayHandle hDisplayHd;
	DI_HDMI_CALLBACK pfCallback[DH_EVENT_MAX];
	unsigned long		interruptId;
	DI_HDMI_HOTPLUG_STATE_e	eHotplugState;
#if !defined(RX_POWERDOWNMODE) /* remove */
	DI_HDMI_RXSENCE_STATE_e	eRxsenceState;
#endif /* end of !RX_POWERDOWNMODE */
	DI_HDMI_HDCP_STATUS		eHdcpStatus;
	HUINT32					lastRxPowerTime;
	HBOOL					bInitialized;
	HULONG 					ulSemId;
	HBOOL					bEnableOutput;
	HBOOL					bDevicePowerState;
#if defined(RX_POWERDOWNMODE) || defined(WORKAROUND_RXSENSE_POLLING)
	HBOOL bSetPollingEvent;
#endif

} DI_HDMI_INSTANCE;

typedef struct
{
	HDMI_MSG_E 	eHdmiMsg;
	HUINT32		param1;
	HUINT32		param2;
} HDMI_MSG_T;

typedef struct
{
	HBOOL bHdcpEnable;
	HBOOL bHdmiEnable;
	HBOOL bHdmiAvMute;
	HBOOL bHdmiAudioMute;
	HBOOL bHdmiAvBlank;
	HBOOL bHdmiPowerState;
} HDMI_REQUEST_T;

#if defined(CONFIG_HDCP_22_PROVISIONING)
typedef struct P_HDMI_HDCP22_PROVISION
{
	HUINT8 *pucInput;
	HUINT8 *pucOutput;
	HUINT32 nKeySize;
    SRAI_PlatformHandle platformHandle;
    SRAI_ModuleHandle   moduleHandle;
}P_HDMI_HDCP22_PROVISION_t;
#endif


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_HDMI_INSTANCE s_hdmi_instance;
static HDMI_REQUEST_T s_hdmi_request;

static HUINT8 s_ucaHdcpKey[DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE];
#if defined(CONFIG_HDCP_22)
static HUINT8* s_pucHdcp22Key;
static HUINT32 s_nHdcp22KeySize;
#endif
#if defined(CONFIG_HDCP_22_PROVISIONING)
static P_HDMI_HDCP22_PROVISION_t s_tHDCP22Provision;
#endif
static HULONG ulHdmiTaskId;
static HULONG ulHdmiMsgQueueId;
static HULONG s_ulHdmiSyncCallEventId;

static HULONG	s_hdcpShortRetryTimer;
static HULONG	s_hdcpLongRetryTimer;
static HULONG	s_hdcpCheckPollingTimer;

static HULONG	s_hdcpShortRetryCount;
static HULONG	s_hdcpCheckPollingCount;

#if defined(CONFIG_HDCP) && defined(RELOAD_VALID_HDCP_KEY)
static HBOOL	s_hdcpKeyValid;
#endif

static HUINT32	s_hdmiHdcpRetryInterval = 100;

#if defined(WORKAROUND_RXSENSE_POLLING)
static HULONG	s_RxSensePollingTimer;
#endif

static HULONG	s_HotplugTimer;

#if defined(WORKAROUND_HDMI_VENDOR_ID)
static HUINT8 gVendorID[2];
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
extern DRV_HDMI_Preemp_Setting_t CustomPreempTable[];
#endif

#if defined(CONFIG_DEBUG)
//strings for debugging HDMI TASK
static const char *EventString[] =
{
	"HDMI_MSG_EVT_HOTPLUG_DISCONNECTED",
	"HDMI_MSG_EVT_HOTPLUG_CONNECTED",
	"HDMI_MSG_EVT_HOTPLUG_POWER_DOWN",
#if !defined(RX_POWERDOWNMODE) /* remove */
	"HDMI_MSG_EVT_RXPOWER_DISCONNECTED",
	"HDMI_MSG_EVT_RXPOWER_CONNECTED",
	"HDMI_MSG_EVT_RXPOWER_POWER_DOWN",
#endif /* end of !RX_POWERDOWNMODE */
	"HDMI_MSG_EVT_HDCP_SUCCESS",
	"HDMI_MSG_EVT_HDCP_FAIL",
	"HDMI_MSG_EVT_CEC",
	"HDMI_MSG_CMD_ENABLE_HDCP",
	"HDMI_MSG_CMD_DISABLE_HDCP",
	"HDMI_MSG_CMD_AV_BLANK",
	"HDMI_MSG_CMD_AV_UNBLANK",
	"HDMI_MSG_CMD_AV_MUTE",
	"HDMI_MSG_CMD_AV_UNMUTE",
	"HDMI_MSG_CMD_AUDIO_MUTE",
	"HDMI_MSG_CMD_AUDIO_UNMUTE",
	"HDMI_MSG_CMD_SEND_CEC_MSG",
	"HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT",
	"HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT",
	"HDMI_MSG_CMD_AUXINFO",
	"HDMI_MSG_CMD_COLORSPACE",
	"HDMI_MSG_CMD_3D_FORMAT",
	"HDMI_MSG_CMD_POWER_ON",
	"HDMI_MSG_CMD_POWER_OFF",
	"HDMI_MSG_SLF_HDCP_CHECK_POLLING",
	"HDMI_MSG_CMD_SET_RESOLUTION",
	"HDMI_MSG_MAX"
} ;

#if defined(CONFIG_DI20)
static const char *SupportedResolution[] =
{
	"DI_HDMI_RESOLUTION_Unknown"
	"DI_HDMI_RESOLUTION_640x480P",
	"DI_HDMI_RESOLUTION_720x480P",
	"DI_HDMI_RESOLUTION_720x480P",
	"DI_HDMI_RESOLUTION_1080x720P",
	"DI_HDMI_RESOLUTION_1920x1080I",
	"DI_HDMI_RESOLUTION_720x480I",
	"DI_HDMI_RESOLUTION_720x480I",
	"DI_HDMI_RESOLUTION_720x240P",
	"DI_HDMI_RESOLUTION_720x240P",
	"DI_HDMI_RESOLUTION_2880x480I",
	"DI_HDMI_RESOLUTION_2880x480I",
	"DI_HDMI_RESOLUTION_2880x240P",
	"DI_HDMI_RESOLUTION_2880x240P",
	"DI_HDMI_RESOLUTION_1440x480P",
	"DI_HDMI_RESOLUTION_1440x480P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_720x576P",
	"DI_HDMI_RESOLUTION_720x576P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1920x1080I",
	"DI_HDMI_RESOLUTION_720x576I",
	"DI_HDMI_RESOLUTION_720x576I",
	"DI_HDMI_RESOLUTION_720x288P",
	"DI_HDMI_RESOLUTION_720x288P",
	"DI_HDMI_RESOLUTION_2880x576I",
	"DI_HDMI_RESOLUTION_2880x576I",
	"DI_HDMI_RESOLUTION_2880x288P",
	"DI_HDMI_RESOLUTION_2880x288P",
	"DI_HDMI_RESOLUTION_1440x576P",
	"DI_HDMI_RESOLUTION_1440x576P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_2880x480P",
	"DI_HDMI_RESOLUTION_2880x480P",
	"DI_HDMI_RESOLUTION_2880x576P",
	"DI_HDMI_RESOLUTION_2880x576P",
	"DI_HDMI_RESOLUTION_1920x1080I",
	"DI_HDMI_RESOLUTION_1920x1080I",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_720x576P",
	"DI_HDMI_RESOLUTION_720x576P",
	"DI_HDMI_RESOLUTION_720x576I",
	"DI_HDMI_RESOLUTION_720x576I",
	"DI_HDMI_RESOLUTION_1920x1080I",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_720x480P",
	"DI_HDMI_RESOLUTION_720x480P",
	"DI_HDMI_RESOLUTION_720x480I",
	"DI_HDMI_RESOLUTION_720x480I",
	"DI_HDMI_RESOLUTION_720x576P",
	"DI_HDMI_RESOLUTION_720x576P",
	"DI_HDMI_RESOLUTION_720x576I",
	"DI_HDMI_RESOLUTION_720x576I",
	"DI_HDMI_RESOLUTION_720x480P",
	"DI_HDMI_RESOLUTION_720x480P",
	"DI_HDMI_RESOLUTION_720x480I",
	"DI_HDMI_RESOLUTION_720x480I",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1280x720P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1920x1080P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_1680x720P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_2560x1080P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_4096x2160P",
	"DI_HDMI_RESOLUTION_4096x2160P",
	"DI_HDMI_RESOLUTION_4096x2160P",
	"DI_HDMI_RESOLUTION_4096x2160P",
	"DI_HDMI_RESOLUTION_4096x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_3840x2160P",
	"DI_HDMI_RESOLUTION_Unknown"
};
#elif defined(CONFIG_DI10)
static const char *SupportedResolution[] =
{
	"DI_HDMI_VFORMAT_480I",
	"DI_HDMI_VFORMAT_480I_J",
	"DI_HDMI_VFORMAT_480P",
	"DI_HDMI_VFORMAT_576I",
	"DI_HDMI_VFORMAT_576P",
	"DI_HDMI_VFORMAT_720P",
	"DI_HDMI_VFORMAT_1080I",
	"DI_HDMI_VFORMAT_720P_50HZ",
	"DI_HDMI_VFORMAT_1080I_50HZ",
	"DI_HDMI_VFORMAT_1080P_50HZ",
	"DI_HDMI_VFORMAT_480I_4_3",
	"DI_HDMI_VFORMAT_480P_4_3",
	"DI_HDMI_VFORMAT_576I_4_3",
	"DI_HDMI_VFORMAT_576P_4_3",
	"DI_HDMI_VFORMAT_1080P_24HZ",
	"DI_HDMI_VFORMAT_1080P_30HZ",
	"DI_HDMI_VFORMAT_1080P",
	"DI_HDMI_VFORMAT_2160P_24HZ",
	"DI_HDMI_VFORMAT_2160P_25HZ",
	"DI_HDMI_VFORMAT_2160P_30HZ",
	"DI_HDMI_VFORMAT_2160P_50HZ",
	"DI_HDMI_VFORMAT_2160P_60HZ",
	"DI_HDMI_VFORMAT_1080P_25HZ"
};
#endif //#if defined(CONFIG_DI20)
#endif

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#ifdef HPD_DELAY
void P_HDMI_HotplugTimer(HULONG param0, void* param1);
void P_HDMI_SetHotplugTimer(void);
void P_HDMI_UnsetHotplugTimer(void);
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
DRV_Error P_HDMI_SetPreEmphasisConfiguration(void);
#endif

#if defined(CONFIG_HDCP_22_PROVISIONING)
static int _ParseAndDisplay(HUINT8* pBinFile, HUINT32 binFileLength);
static const char * _MapDrmEnumToString(HUINT32 drm_type);
#endif

#if __________p__________
#endif

static HBOOL P_HDMI_GetHotplugStatus(void)
{
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputStatus	stHdmiOutputStatus;
	HBOOL bConnect=FALSE;

	nexus_rc = NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &stHdmiOutputStatus);
	if(nexus_rc != NEXUS_SUCCESS)
	{
		PrintError("%s():%d failed!\n",__func__,__LINE__);
	}
	if(stHdmiOutputStatus.connected==TRUE)
	{
		bConnect=TRUE;
	}

	return bConnect;
}

#if defined(CONFIG_DI20)
static void P_HDMI_GetVideoIDCode(DI_HDMI_AVAILABLE_FORMATS* pAvailable,HUINT8 VIC, HUINT32 index)
{

	switch (VIC)
	{
	case 1:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_640x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P_4_3;
		break;
	case 2:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P_4_3;
		break;
	case 3:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P;
		break;
	case 4:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P;
		break;
	case 5:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I;
		break;
	case 6:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I_4_3;
		break;
	case 7:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I;
		break;
	case 8:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x240P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P_4_3;
		break;
	case 9:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x240P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P;
		break;
	case 10:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 11:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 12:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x240P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

	//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 13:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x240P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
		break;
	case 14:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1440x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 15:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1440x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P_4_3;
		break;
	case 16:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P;
		break;
	case 17:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P_4_3;
		break;
	case 18:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P;
		break;
	case 19:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 20:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 21:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 22:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
		break;
	case 23:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x288P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 24:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x288P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 25:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 26:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
		break;
	case 27:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x288P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 28:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x288P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P_4_3;
		break;
	case 29:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1440x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P;
		break;
	case 30:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1440x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P;
		break;
	case 31:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_50HZ;
		break;
	case 32:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 33:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I;
		break;
	case 34:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P_4_3;
		break;
	case 35:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P;
		break;
	case 36:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 37:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 38:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2880x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 39:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 40:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 41:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P_4_3;
		break;
	case 42:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P;
		break;
	case 43:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P;
		break;
	case 44:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I;
		break;
	case 45:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I_4_3;
		break;
	case 46:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I;
		break;
	case 47:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P_4_3;
		break;
	case 48:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 49:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 50:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 51:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
		break;
	case 52:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_200HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 53:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_200HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 54:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_200HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 55:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_200HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 56:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_240HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
		break;
	case 57:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_240HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 58:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_240HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 59:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_240HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 60:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 61:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
		break;
	case 62:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 63:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 64:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 65:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 66:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 67:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 68:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 69:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 70:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 71:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 72:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 73:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 74:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 75:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 76:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 77:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 78:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 79:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 80:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 81:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 82:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 83:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 84:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 85:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 86:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 87:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 88:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 89:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 90:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 91:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_100HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 92:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_120HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		//pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 93:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_24HZ;
		break;
	case 94:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_25HZ;
		break;
	case 95:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_30HZ;
		break;
#if (BCHP_CHIP==7445)&&(BCHP_VER > BCHP_VER_C0)
	case 96:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_50HZ;
		break;
	case 97:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_60HZ;
		break;
#endif
	case 98:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_24HZ;
		break;
	case 99:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_25HZ;
		break;
	case 100:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_30HZ;
		break;
#if (BCHP_CHIP==7445)&&(BCHP_VER > BCHP_VER_C0)
	case 101:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_50HZ;
		break;
	case 102:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_60HZ;
		break;
#endif
	case 103:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_24HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_24HZ;
		break;
	case 104:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_25HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_25HZ;
		break;
	case 105:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_30HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_30HZ;
		break;
#if (BCHP_CHIP==7445)&&(BCHP_VER > BCHP_VER_C0)
	case 106:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_50HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_50HZ;
		break;
	case 107:
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_60HZ;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;

		pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_60HZ;
		break;
#endif
	default :
		pAvailable->stVformats[index].ulVFormat		=DI_HDMI_RESOLUTION_Unknown;
		pAvailable->stVformats[index].ulFieldRate		=DI_HDMI_FIELD_RATE_Unknown;
		pAvailable->stVformats[index].ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;
	}

}

static DI_ERR_CODE P_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputStatus	stHdmiOutputStatus;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_CecStatus 	tCecStatus;
#endif
	DI_HDMI_AUDIO_FORMAT	eAFormat=DI_HDMI_AFORMAT_PCM;
	HUINT8	ucBitCount=0;
	NEXUS_AudioCodec	eNexusAudioCodec=NEXUS_AudioCodec_eUnknown;
	HUINT8 i, j, /* indexes */
		DataOffset,
		DataBlockIndex,
		DataBlockTag,
		DataBlockLength,
		NumVideoDescriptors,
		EdidVideoIDCode ;
	HUINT8 extensions=0;
	NEXUS_HdmiOutputEdidBlock stEdidBlock;

	PrintEnter();

	if((s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_DISCONNECTED)&&(P_HDMI_GetHotplugStatus()==FALSE))/* nexus state re-check*/
	{
		PrintError("HDMI is NOT connected\n");
		pAvailable->sPhysicalAddr.NodeA = 0xF;
		pAvailable->sPhysicalAddr.NodeB = 0xF;
		pAvailable->sPhysicalAddr.NodeC = 0xF;
		pAvailable->sPhysicalAddr.NodeD = 0xF;
		pAvailable->ulLogicalAddr=0xF;
		rc=DI_ERR_ERROR;
		goto done;
	}

	VK_memset(pAvailable, 0x00, sizeof(DI_HDMI_AVAILABLE_FORMATS));

	nexus_rc=NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &stHdmiOutputStatus);
	if (nexus_rc!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		rc=DI_ERR_ERROR;
		goto done;
	}
	/* 1. get supported audio format & samplingrate list */

	while(eAFormat < 0x8000)
	{
		switch(eAFormat)
		{
		case DI_HDMI_AFORMAT_PCM:
			eNexusAudioCodec=NEXUS_AudioCodec_ePcm;
			break;
		case DI_HDMI_AFORMAT_AC3:
			eNexusAudioCodec=NEXUS_AudioCodec_eAc3;
			break;
		case DI_HDMI_AFORMAT_AC3PLUS:
			eNexusAudioCodec=NEXUS_AudioCodec_eAc3Plus;
			break;
		case DI_HDMI_AFORMAT_MPEG1:
		case DI_HDMI_AFORMAT_MPEG2:
			eNexusAudioCodec=NEXUS_AudioCodec_eMpeg;
			break;
		case DI_HDMI_AFORMAT_MP3:
			eNexusAudioCodec=NEXUS_AudioCodec_eMp3;
			break;
		case DI_HDMI_AFORMAT_AAC:
			eNexusAudioCodec=NEXUS_AudioCodec_eAac;
			break;
		case DI_HDMI_AFORMAT_DTS:
			eNexusAudioCodec=NEXUS_AudioCodec_eDts;
			break;
		case DI_HDMI_AFORMAT_DTSHD:
			eNexusAudioCodec=NEXUS_AudioCodec_eDtsHd;
			break;
		case DI_HDMI_AFORMAT_WMAPro:
			eNexusAudioCodec=NEXUS_AudioCodec_eWmaPro;
			break;
		case DI_HDMI_AFORMAT_OneBit:
		case DI_HDMI_AFORMAT_ATRAC:
		case DI_HDMI_AFORMAT_MATMLP:
		case DI_HDMI_AFORMAT_DST:
		default:
			eNexusAudioCodec=NEXUS_AudioCodec_eUnknown;
			break;
		}

		if (stHdmiOutputStatus.audioInfo[eNexusAudioCodec].bSupported==true)
		{
			pAvailable->ulAudioFormats |= eAFormat;
			pAvailable->tAudioInfo[ucBitCount].ulAudioFormat=eAFormat;
			pAvailable->tAudioInfo[ucBitCount].unSupportedFs=stHdmiOutputStatus.audioInfo[eNexusAudioCodec].uSamplingRate;
			ucBitCount++;
		}
		eAFormat=eAFormat << 1;
	}


	/* 2. get physical & logical address */

	pAvailable->sPhysicalAddr.NodeA = stHdmiOutputStatus.physicalAddressA;
	pAvailable->sPhysicalAddr.NodeB = stHdmiOutputStatus.physicalAddressB;
	pAvailable->sPhysicalAddr.NodeC = stHdmiOutputStatus.physicalAddressC;
	pAvailable->sPhysicalAddr.NodeD = stHdmiOutputStatus.physicalAddressD;

	/* 3. get supported video format */
	/* check ALL extensions for Version 3 Timing Extensions */
	VK_memset(&stEdidBlock, 0x00, sizeof(NEXUS_HdmiOutputEdidBlock));
	NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, 0, &stEdidBlock) ;
	extensions=stEdidBlock.data[126];
	if (!extensions)
	{
		PrintDebug(("No EDID Extensions Found... No HDMI Support")) ;
		rc=DI_ERR_ERROR ;
		goto done ;
	}

	if (extensions > 127)
	{
		PrintError("Invalid EDID block... extensions(%d)", extensions);
		rc = DI_ERR_ERROR ;
		goto done ;
	}

	pAvailable->ulNumOfVFormat = 0;

	for(i=1 ; i <= extensions; i++)
	{
		NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, i, &stEdidBlock) ;

		/* check for Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_TAG]!=DRV_HDMI_EDID_EXT_TIMING_DATA)
		{
			continue;
		}

		/* check for Version 3 Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_VERSION]!=DRV_HDMI_EDID_TIMING_VERSION_3)
		{
			continue;
		}

		/* check if data blocks exist before the 18 Byte Detailed Timing data */
		DataOffset=stEdidBlock.data[DRV_HDMI_EDID_EXT_DATA_OFFSET] ;
		if ((DataOffset==0) || (DataOffset==4)) /* no Reserved Data is Available */
		{
			PrintDebug("-----V3 Timing Extension contains no CEA Data Blocks") ;
			continue ;          /* continue to the next Timing Extension */
		}

		/* set the index to the start of Data Blocks */
		DataBlockIndex=DRV_HDMI_EDID_EXT_DATA_BLOCK_COLLECTION ;

		/* scan through the data blocks and retrieve the necessary information */
		while (DataBlockIndex < DataOffset)
		{
			DataBlockTag=stEdidBlock.data[DataBlockIndex] >> 5 ;
			DataBlockLength=stEdidBlock.data[DataBlockIndex] & 0x1F ;
			if (DataBlockTag==DRV_HDMI_EDID_CeaDataBlockTag_eVideoDB)   /* Video DB */
			{
				HUINT32 ulPrevMax;
				/* check each video descriptor for requested video support */
				NumVideoDescriptors=DataBlockLength ;

				ulPrevMax = pAvailable->ulNumOfVFormat;
				pAvailable->ulNumOfVFormat += NumVideoDescriptors;


				PrintDebug("Supported Reolutions : \n");

				/* for each CEA Video ID Code Found */
				for (j=0 ; j < NumVideoDescriptors ; j++)
				{
					if(ulPrevMax+j > 107)
					{
						PrintError("Exceed Max Video Format\n");
						break;
					}
					/* get the supported Video Code ID; check if a native format */
					EdidVideoIDCode=stEdidBlock.data[DataBlockIndex + j + 1] ;
					EdidVideoIDCode=EdidVideoIDCode  & 0x7F;

					if(EdidVideoIDCode <= 107)
					{
#if defined(CONFIG_DEBUG)
						PrintDebug("\t\t(%d)%s\n", EdidVideoIDCode, SupportedResolution[EdidVideoIDCode]);
#endif
					}
					P_HDMI_GetVideoIDCode(pAvailable,EdidVideoIDCode,ulPrevMax+j);

				} /* for each Supported CEA Video ID Code */
			} /* for each CEA Video ID Code found */
#if defined(CONFIG_HDOUT_2160P)
			else if (DataBlockTag == DRV_HDMI_EDID_CeaDataBlockTag_eExtendedDB)
			{
				/* Ycbcr 4:2:0 VDB(Video Data Block tag = 0x07)*/
				HUINT8 ExtTagCode;
				HUINT32 ulPrevMax;

				ExtTagCode = stEdidBlock.data[DataBlockIndex+1];
				if(ExtTagCode == 0xE)
				{
					NumVideoDescriptors=DataBlockLength ;

					ulPrevMax = pAvailable->ulNumOfVFormat;
					pAvailable->ulNumOfVFormat += NumVideoDescriptors;

					PrintDebug("Supported Reolutions Ycbcr 4:2:0 VDB : \n");
					/* for each CEA Video ID Code Found */
					for (j=0 ; j < NumVideoDescriptors-1 ; j++)
					{
						if(ulPrevMax+j > 107)
						{
							PrintError("Exceed Max Video Format\n");
							break;
						}
						/* get the supported Video Code ID; check if a native format */
						EdidVideoIDCode=stEdidBlock.data[DataBlockIndex + j + 2] ;
						EdidVideoIDCode=EdidVideoIDCode  & 0x7F;

						if(EdidVideoIDCode <= 107)
						{
#if defined(CONFIG_DEBUG)
							PrintDebug("\t\t Ycbcr 4:2:0 VDB(%d)%s\n", EdidVideoIDCode, SupportedResolution[EdidVideoIDCode]);
#endif
						}
						P_HDMI_GetVideoIDCode(pAvailable,EdidVideoIDCode,ulPrevMax+j);

					} /* for each Supported CEA Video ID Code */
				}
			}
#endif
			DataBlockIndex += DataBlockLength + 1;
		} /* while DataBlockIndex < DataOffset */
	} /* for each extension */
	/* 4. get current logical address */
	pAvailable->ulLogicalAddr=0x0f;
	/* 5. get supported 3D format in the TV */
	for (i=0; i<NEXUS_VideoFormat_eMax; i++)
	{
		if (stHdmiOutputStatus.hdmi3DFormatsSupported[i])
		{
			if (stHdmiOutputStatus.hdmi3DFormatsSupported[i] & NEXUS_HdmiOutput_3DStructure_TopAndBottom)
				pAvailable->ul3dFormats |= DI_HDMI_3D_FORMAT_TOPANDBOTTOM;
			if (stHdmiOutputStatus.hdmi3DFormatsSupported[i] & NEXUS_HdmiOutput_3DStructure_SideBySideHalfHorizontal)
				pAvailable->ul3dFormats |= DI_HDMI_3D_FORMAT_SIDEBYSIDE;
			if (stHdmiOutputStatus.hdmi3DFormatsSupported[i] & NEXUS_HdmiOutput_3DStructure_FramePacking)
				pAvailable->ul3dFormats |= DI_HDMI_3D_FORMAT_FRAMEPACKING;
		}
	}
#if defined(CONFIG_HDMI_CEC)
	nexus_rc=NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &tCecStatus);
	if (nexus_rc!=NEXUS_SUCCESS)
	{
		// Do not take it as an error, because it is often the case that a TV doesn't support CEC.
		// 2009.11.26 dcchung
		goto done;
	}
	else
	{
		pAvailable->ulLogicalAddr=tCecStatus.logicalAddress;
	}
#endif

done:
	PrintExit();
	return rc;
}

#else
/* DI1.0 */
static DI_ERR_CODE P_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputStatus	stHdmiOutputStatus;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_CecStatus 	tCecStatus;
#endif
	DI_HDMI_AUDIO_FORMAT	eAFormat=DI_HDMI_AFORMAT_PCM;
	HUINT8	ucBitCount=0;
	NEXUS_AudioCodec	eNexusAudioCodec=NEXUS_AudioCodec_eUnknown;
	HUINT8 i, j, /* indexes */
		DataOffset,
		DataBlockIndex,
		DataBlockTag,
		DataBlockLength,
		NumVideoDescriptors,
		EdidVideoIDCode ;
	HUINT8 extensions=0;
	NEXUS_HdmiOutputEdidBlock stEdidBlock;

	PrintEnter();

	if(s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_DISCONNECTED)
	{
		PrintError("HDMI is NOT connected\n");
		pAvailable->sPhysicalAddr.NodeA = 0xF;
		pAvailable->sPhysicalAddr.NodeB = 0xF;
		pAvailable->sPhysicalAddr.NodeC = 0xF;
		pAvailable->sPhysicalAddr.NodeD = 0xF;
		pAvailable->ulLogicalAddr=0xF;

		rc=DI_ERR_ERROR;
		goto done;
	}

	nexus_rc=NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &stHdmiOutputStatus);
	if (nexus_rc!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		rc=DI_ERR_ERROR;
		goto done;
	}
	VK_memset(pAvailable, 0x00, sizeof(DI_HDMI_AVAILABLE_FORMATS));

	/* 1. get supported audio format & samplingrate list */

	while(eAFormat < 0x8000)
	{
		switch(eAFormat)
		{
		case DI_HDMI_AFORMAT_PCM:
			eNexusAudioCodec=NEXUS_AudioCodec_ePcm;
			break;
		case DI_HDMI_AFORMAT_AC3:
			eNexusAudioCodec=NEXUS_AudioCodec_eAc3;
			break;
		case DI_HDMI_AFORMAT_AC3PLUS:
			eNexusAudioCodec=NEXUS_AudioCodec_eAc3Plus;
			break;
		case DI_HDMI_AFORMAT_MPEG1:
		case DI_HDMI_AFORMAT_MPEG2:
			eNexusAudioCodec=NEXUS_AudioCodec_eMpeg;
			break;
		case DI_HDMI_AFORMAT_MP3:
			eNexusAudioCodec=NEXUS_AudioCodec_eMp3;
			break;
		case DI_HDMI_AFORMAT_AAC:
			eNexusAudioCodec=NEXUS_AudioCodec_eAac;
			break;
		case DI_HDMI_AFORMAT_DTS:
			eNexusAudioCodec=NEXUS_AudioCodec_eDts;
			break;
		case DI_HDMI_AFORMAT_DTSHD:
			eNexusAudioCodec=NEXUS_AudioCodec_eDtsHd;
			break;
		case DI_HDMI_AFORMAT_WMAPro:
			eNexusAudioCodec=NEXUS_AudioCodec_eWmaPro;
			break;
		case DI_HDMI_AFORMAT_OneBit:
		case DI_HDMI_AFORMAT_ATRAC:
		case DI_HDMI_AFORMAT_MATMLP:
		case DI_HDMI_AFORMAT_DST:
		default:
			eNexusAudioCodec=NEXUS_AudioCodec_eUnknown;
			break;
		}

		if (stHdmiOutputStatus.audioInfo[eNexusAudioCodec].bSupported==true)
		{
			pAvailable->ulAudioFormats |= eAFormat;
			pAvailable->tAudioInfo[ucBitCount].ulAudioFormat=eAFormat;
			pAvailable->tAudioInfo[ucBitCount].unSupportedFs=stHdmiOutputStatus.audioInfo[eNexusAudioCodec].uSamplingRate;
			ucBitCount++;
		}
		eAFormat=eAFormat << 1;
	}


	/* 2. get physical address */
	pAvailable->sPhysicalAddr.NodeA = stHdmiOutputStatus.physicalAddressA;
	pAvailable->sPhysicalAddr.NodeB = stHdmiOutputStatus.physicalAddressB;
	pAvailable->sPhysicalAddr.NodeC = stHdmiOutputStatus.physicalAddressC;
	pAvailable->sPhysicalAddr.NodeD = stHdmiOutputStatus.physicalAddressD;

	/* 3. get supported video format */
	/* check ALL extensions for Version 3 Timing Extensions */
	VK_memset(&stEdidBlock, 0x00, sizeof(NEXUS_HdmiOutputEdidBlock));
	nexus_rc=NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, 0, &stEdidBlock) ;
	if (nexus_rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetEdidBlock failed!\n");
	}

	extensions=stEdidBlock.data[126];
	if (!extensions)
	{
		PrintDebug(("No EDID Extensions Found... No HDMI Support")) ;
		rc=DI_ERR_ERROR ;
		goto done ;
	}

	if (extensions > 127)
	{
		PrintError("Invalid EDID block... extensions(%d)", extensions);
		rc = DI_ERR_ERROR ;
		goto done ;
	}

	for(i=1 ; i <= extensions; i++)
	{
		NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, i, &stEdidBlock) ;

		/* check for Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_TAG]!=DRV_HDMI_EDID_EXT_TIMING_DATA)
		{
			continue;
		}

		/* check for Version 3 Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_VERSION]!=DRV_HDMI_EDID_TIMING_VERSION_3)
		{
			continue;
		}

		/* check if data blocks exist before the 18 Byte Detailed Timing data */
		DataOffset=stEdidBlock.data[DRV_HDMI_EDID_EXT_DATA_OFFSET] ;
		if ((DataOffset==0) || (DataOffset==4)) /* no Reserved Data is Available */
		{
			PrintDebug("-----V3 Timing Extension contains no CEA Data Blocks") ;
			continue ;          /* continue to the next Timing Extension */
		}

		/* set the index to the start of Data Blocks */
		DataBlockIndex=DRV_HDMI_EDID_EXT_DATA_BLOCK_COLLECTION ;

		/* scan through the data blocks and retrieve the necessary information */
		while (DataBlockIndex < DataOffset)
		{
			DataBlockTag=stEdidBlock.data[DataBlockIndex] >> 5 ;
			DataBlockLength=stEdidBlock.data[DataBlockIndex] & 0x1F ;
			if (DataBlockTag==DRV_HDMI_EDID_CeaDataBlockTag_eVideoDB)   /* Video DB */
			{
				/* check each video descriptor for requested video support */
				NumVideoDescriptors=DataBlockLength ;

				/* for each CEA Video ID Code Found */
				for (j=0 ; j < NumVideoDescriptors ; j++)
				{
					/* get the supported Video Code ID; check if a native format */
					EdidVideoIDCode=stEdidBlock.data[DataBlockIndex + j + 1] ;
					EdidVideoIDCode=EdidVideoIDCode  & 0x7F;
					switch (EdidVideoIDCode)
					{
					case 2:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P_4_3;
						break;
					case 3:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480P;
						break;
					case 4:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P;
						break;
					case 5:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I;
						break;
					case 6:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I_4_3;
						break;
					case 7:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_480I;
						break;
					case 16:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P;
						break;
					case 17:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P_4_3;
						break;
					case 18:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576P;
						break;
					case 19:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_720P_50HZ;
						break;
					case 20:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080I_50HZ;
						break;
					case 21:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I_4_3;
						break;
					case 22:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_576I;
						break;

					case 31:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_50HZ;
						break;
					case 32:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_24HZ;
						break;
					case 33:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_25HZ;
						break;
					case 34:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_1080P_30HZ;
						break;
					case 93:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_24HZ;
						break;
					case 94:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_25HZ;
						break;
					case 95:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_30HZ;
						break;
					case 96:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_50HZ;
						break;
					case 97:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_60HZ;
						break;
					case 98:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_24HZ;
						break;
					case 99:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_25HZ;
						break;
					case 100:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_30HZ;
						break;
					case 101:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_50HZ;
						break;
					case 102:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_60HZ;
						break;
					case 103:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_24HZ;
						break;
					case 104:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_25HZ;
						break;
					case 105:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_30HZ;
						break;
					case 106:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_50HZ;
						break;
					case 107:
						pAvailable->ulVideoFormats|=DI_HDMI_VFORMAT_2160P_60HZ;
						break;
					default :
						break;
					}
				} /* for each Supported CEA Video ID Code */
			} /* for each CEA Video ID Code found */

			DataBlockIndex += DataBlockLength + 1;
		} /* while DataBlockIndex < DataOffset */
	} /* for each extension */
	/* 4. get current logical address */
	pAvailable->ulLogicalAddr=0x0f;
#if defined(CONFIG_HDMI_CEC)
	nexus_rc=NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &tCecStatus);
	if (nexus_rc!=NEXUS_SUCCESS)
	{
		// Do not take it as an error, because it is often the case that a TV doesn't support CEC.
		// 2009.11.26 dcchung
		goto done;
	}
	else
	{
		pAvailable->ulLogicalAddr=tCecStatus.logicalAddress;
	}
#endif

done:
	PrintExit();
	return rc;
}
#endif

#ifdef HPD_DELAY
void P_HDMI_HotplugTimer(HULONG param0, void* param1)
{
	HDMI_MSG_T HdmiMsg;

	PrintEnter();

	HAPPY(param0);
	HAPPY(param1);

	if(s_hdmi_instance.bDevicePowerState == FALSE)
	{
		PrintError("HDMI Power Down State. Cancel HPD operation\n");
	  	//P_HDMI_SetHotplugTimer();
	}
   else
   {
		HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HOTPLUG_CONNECTED;
		VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
   }
	s_HotplugTimer = 0;

	PrintExit();
	return;
}

void P_HDMI_SetHotplugTimer(void)
{
	PrintEnter();

	if(s_HotplugTimer != 0)
	{
		PrintError("Prior Hotplug Connect event exists yet\n");
		P_HDMI_UnsetHotplugTimer();
	}

	VK_TIMER_EventAfter(HPD_DELAY, P_HDMI_HotplugTimer, NULL, 0, (unsigned long *)&s_HotplugTimer);

	PrintExit();
}

void P_HDMI_UnsetHotplugTimer(void)
{
	if(s_HotplugTimer != 0)
	{
		VK_TIMER_Cancel(s_HotplugTimer);
		s_HotplugTimer = 0;
		PrintDebug("Prior Hotplug Connect Canceled\n");
	}

	return;
}
#endif

void P_HDMI_HdcpShortRetryTimer(HULONG param0, void* param1)
{
	HDMI_MSG_T HdmiMsg;
	HAPPY(param0);
	HAPPY(param1);

	PrintEnter();

	if (s_hdcpShortRetryTimer != 0)
	{
		s_hdcpShortRetryTimer = 0;
	}
	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDCP;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

	PrintExit();

	return;
}

void P_HDMI_SetHdcpShortRetryTimer(void)
{
	PrintEnter();

	if (s_hdcpShortRetryTimer != 0)
		return;

	VK_TIMER_EventAfter(s_hdmiHdcpRetryInterval, P_HDMI_HdcpShortRetryTimer, NULL, 0, (unsigned long *)&s_hdcpShortRetryTimer);

	PrintExit();

	return;
}

void P_HDMI_HdcpLongRetryTimer(HULONG param0, void* param1)
{
	HDMI_MSG_T HdmiMsg;
	HAPPY(param0);
	HAPPY(param1);

	PrintEnter();

	if (s_hdcpLongRetryTimer != 0)
	{
		s_hdcpLongRetryTimer = 0;
	}

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDCP;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

	PrintExit();

	return;
}

void P_HDMI_CancelHdcpLongRetryTimer(void)
{
	PrintEnter();

	if (s_hdcpLongRetryTimer != 0)
	VK_TIMER_Cancel(s_hdcpLongRetryTimer);

	s_hdcpLongRetryTimer = 0;

	PrintExit();

	return;
}

void P_HDMI_SetHdcpLongRetryTimer(void)
{
	PrintEnter();

	if (s_hdcpLongRetryTimer != 0)
		return;
	VK_TIMER_EventAfter(DI_HDMI_HDCP_RETRY_LIMIT, P_HDMI_HdcpLongRetryTimer, NULL, 0, (unsigned long *)&s_hdcpLongRetryTimer);

	PrintExit();

	return;
}

void P_HDMI_HdcpCheckPollingTimer(HULONG param0, void* param1)
{
	HDMI_MSG_T HdmiMsg;
	HAPPY(param0);
	HAPPY(param1);

	PrintEnter();

	if (s_hdcpCheckPollingTimer != 0)
	{
		s_hdcpCheckPollingTimer = 0;
	}

	HdmiMsg.eHdmiMsg = HDMI_MSG_SLF_HDCP_CHECK_POLLING;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

	PrintExit();

	return;
}

void P_HDMI_SetHdcpCheckPollingTimer(void)
{
	PrintEnter();

	if (s_hdcpCheckPollingTimer != 0)
	{
		VK_TIMER_Cancel(s_hdcpCheckPollingTimer);
	}

	VK_TIMER_EventAfter(DI_HDMI_HDCP_CHECK_POLLING_INTERVAL, P_HDMI_HdcpCheckPollingTimer, NULL, 0, (unsigned long *)&s_hdcpCheckPollingTimer);

	PrintExit();

	return;
}

void P_HDMI_UnsetHdcpCheckPollingTimer(void)
{
	PrintEnter();

	if (s_hdcpCheckPollingTimer != 0)
	{
		VK_TIMER_Cancel(s_hdcpCheckPollingTimer);
		s_hdcpCheckPollingTimer = 0;
	}

	PrintExit();

	return;
}

#if defined(WORKAROUND_RXSENSE_POLLING)
static void P_HDMI_rxsence_callback(void *pParam, int iParam)
{
	HDMI_MSG_T HdmiMsg;
	NEXUS_Error	nResult;
	NEXUS_HdmiOutputStatus nStatus;
	NEXUS_HdmiOutputHandle hdmi=pParam;
	DI_HDMI_RXSENCE_STATE_e eRxsenceState=DI_HDMI_RXSENCE_DISCONNECTED;
	//PrintEnter();

	/*************************
	*	1. Get the current status.
	*************************/
	nResult=NEXUS_HdmiOutput_GetStatus(hdmi, &nStatus);
	if (nResult!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		goto done;
	}

#if (NEXUS_VERSION < 1200)
	eRxsenceState = (DI_HDMI_RXSENCE_STATE_e)nStatus.rxsence;
#else
	eRxsenceState = (DI_HDMI_RXSENCE_STATE_e)(nStatus.rxPowered ? DI_HDMI_RXSENCE_CONNECTED : DI_HDMI_RXSENCE_DISCONNECTED);
#endif

	/*************************
	*	2. Send Msg to the TASK.
	*************************/
	HAPPY(iParam);

	if (s_hdmi_instance.eRxsenceState != eRxsenceState)
	{
		PrintDebug("[P_HDMI_rxsence_callback] s_hdmi_instance.eRxsenceState(%d) -> eRxsenceState(%d)\n", s_hdmi_instance.eRxsenceState, eRxsenceState);
		s_hdmi_instance.eRxsenceState = eRxsenceState;
		if (eRxsenceState == DI_HDMI_RXSENCE_DISCONNECTED)
		{
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_RXPOWER_DISCONNECTED;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}
		else if (eRxsenceState==DI_HDMI_RXSENCE_CONNECTED)
		{
			/* I don't think this event needs for us because this event is alredy invoked by HOTPLUG event */
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_RXPOWER_CONNECTED;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}
		else if (eRxsenceState == DI_HDMI_RXSENCE_POWER_DOWN)
		{
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_RXPOWER_POWER_DOWN;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}
		else
		{
			/*	Nothing to do.
			*/
		}
	}

done:
	HAPPY(pParam);
	//PrintExit();
	return;
}
#endif /* end of !RX_POWERDOWNMODE */

#if defined(WORKAROUND_RXSENSE_POLLING)
void P_HDMI_RxSensePollingTimer(HULONG param0, void* param1)
{
	HAPPY(param0);
	HAPPY(param1);
#if defined(RX_POWERDOWNMODE)
	if (s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED || s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_POWER_DOWN)
#else
	if (s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED)
#endif /* end of RX_POWERDOWNMODE */
	{
		P_HDMI_rxsence_callback(s_hdmi_instance.hHdmi, 0);
	}
}
#endif

static void P_HDMI_SetDitheringData(void)
{
	/* Sony TV (KDL-32EX403) zapping pop noise workaround
	Broadcom James recommended to apply almost the whole TVs*/

	DRV_Error	dResult;
	NEXUS_Error nResult;
	char monitorName[14];
	NEXUS_HdmiOutputSettings hdmiSettings;

	PrintEnter();

	VK_MEM_Memset(monitorName, 0x0, sizeof(monitorName));
	dResult = DRV_HDMI_GetMonitorName(monitorName);
	if(dResult == DRV_OK)
	{
		PrintDebug("[%s] Monitor name is %s.\n", __FUNCTION__, monitorName);

		NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
#if defined(WORKAROUND_HDMI_POPNOISE_SHARP_LC_32GD7)
		if(VK_strstr(monitorName, "SHARP") != NULL)
		{
			hdmiSettings.audioDitherEnabled = FALSE;
		}
		else
#endif
		{
			hdmiSettings.audioDitherEnabled = TRUE;
		}

		nResult = NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] %d NEXUS_HdmiOutput_SetSettings failed!\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		PrintError("[%s] %d DRV_HDMI_GetMonitorName failed!\n", __FUNCTION__, __LINE__);
	}

	PrintExit();

	return;
}

static void P_HDMI_SetPauseBurstData(void)
{
	DRV_Error	dResult;
	NEXUS_Error nResult;
	char monitorName[14];
	NEXUS_HdmiOutputSettings hdmiSettings;

	PrintEnter();

	/* Philip TV pop noise & drop noise workaround */
	VK_MEM_Memset(monitorName, 0x0, sizeof(monitorName));
	dResult = DRV_HDMI_GetMonitorName(monitorName);
	if(dResult == DRV_OK)
	{
		PrintDebug("[%s] Monitor name is %s.\n", __FUNCTION__, monitorName);
		NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		if(VK_strstr(monitorName, "Philips") != NULL)
		{
			/* Pause burst data */
#if (NEXUS_VERSION == 1200)
/* Pause burst data 2st patch is not released for only 7425 V6.0
    If Patch for 7425 V6.0 is released, This context should be removed */
			hdmiSettings.enablePauseBurst = TRUE;
#else
#if (NEXUS_VERSION <= 900)
#else
			hdmiSettings.audioBurstType = NEXUS_SpdifOutputBurstType_ePause;
#endif
#endif
		}
		else
		{
			/* Zero data */
#if (NEXUS_VERSION == 1200)
/* Pause burst data 2st patch is not released for only 7425 V6.0
	If Patch for 7425 V6.0 is released, This context should be removed */
			hdmiSettings.enablePauseBurst = FALSE;
#else
#if (NEXUS_VERSION <= 900)
#else
			hdmiSettings.audioBurstType = NEXUS_SpdifOutputBurstType_eNone;
#endif
#endif
		}
		nResult = NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("[%s] %d NEXUS_HdmiOutput_SetSettings failed!\n", __FUNCTION__, __LINE__);
		}
#if (NEXUS_VERSION == 1200)
/* Pause burst data 2st patch is not released for only 7425 V6.0
	If Patch for 7425 V6.0 is released, This context should be removed */
		PrintDebug("[%s] Pause Burst data set to %d !\n", __FUNCTION__, hdmiSettings.enablePauseBurst);
#else
#if (NEXUS_VERSION <= 900)
#else
		PrintDebug("[%s] Pause Burst data set to %d !\n", __FUNCTION__, hdmiSettings.audioBurstType);
#endif
#endif
	}
	else
	{
		PrintError("[%s] %d DRV_HDMI_GetMonitorName failed!\n", __FUNCTION__, __LINE__);
	}

	PrintExit();

	return;
}

static void P_HDMI_hotplug_callback(void *pParam, int iParam)
{//Sunwoo
	NEXUS_Error	nResult;
	HDMI_MSG_T	HdmiMsg;
	NEXUS_HdmiOutputStatus nStatus;
	NEXUS_HdmiOutputHandle hdmi= pParam;
	DI_HDMI_HOTPLUG_STATE_e eHotplugState=DI_HDMI_HOTPLUG_DISCONNECTED;
#if defined(WORKAROUND_HDMI_VENDOR_ID)
	NEXUS_HdmiOutputBasicEdidData BasicEdidData;
#endif

	PrintEnter();

	/*************************
	*	1. Get the current status.
	*************************/
	nResult=NEXUS_HdmiOutput_GetStatus(hdmi, &nStatus);
	if (nResult!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		goto done;
	}

	if(!s_hdmi_instance.bDevicePowerState)
	{
		PrintError("HPD in standby state igronred.\n");
		goto done;
	}

#if (NEXUS_VERSION < 1200)
#if defined(RX_POWERDOWNMODE)
	PrintDebug("[P_HDMI_hotplug_callback] connected (%d), rxsense(%d)\n", nStatus.connected, nStatus.rxsence);
#endif
#endif

#if (NEXUS_VERSION < 1200)
#if defined(RX_POWERDOWNMODE)
	if(nStatus.connected && nStatus.rxsence)
	{
		eHotplugState=DI_HDMI_HOTPLUG_CONNECTED;
	}
	else if(nStatus.connected && !nStatus.rxsence)
	{
		eHotplugState=DI_HDMI_HOTPLUG_POWER_DOWN;
	}
	else
	{
		eHotplugState=DI_HDMI_HOTPLUG_DISCONNECTED;
	}
#else
	eHotplugState=(DI_HDMI_HOTPLUG_STATE_e) (nStatus.connected ? DI_HDMI_HOTPLUG_CONNECTED : DI_HDMI_HOTPLUG_DISCONNECTED);
#endif /* end of RX_POWERDOWNMODE */
#else
	if(nStatus.connected && nStatus.rxPowered)
	{
		eHotplugState=DI_HDMI_HOTPLUG_CONNECTED;
	}
	else if(nStatus.connected && !nStatus.rxPowered)
	{
		eHotplugState=DI_HDMI_HOTPLUG_POWER_DOWN;
	}
	else
	{
		eHotplugState=DI_HDMI_HOTPLUG_DISCONNECTED;
	}
#endif

#if defined(WORKAROUND_HDMI_VENDOR_ID)
	/*************************
	*	2. Get the VendorID.
	*************************/
	if(eHotplugState == DI_HDMI_HOTPLUG_CONNECTED)
	{
		nResult=NEXUS_HdmiOutput_GetBasicEdidData(s_hdmi_instance.hHdmi, &BasicEdidData);
		if (nResult!=NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_GetBasicEdidData failed!\n");
		}
		VK_MEM_Memcpy(gVendorID, BasicEdidData.vendorID, 2);
	}
#endif

	/*************************
	*	2.4. cancel pending HPD timer if exists.
	*************************/
	#ifdef HPD_DELAY
	if((eHotplugState == DI_HDMI_HOTPLUG_DISCONNECTED) || (eHotplugState == DI_HDMI_HOTPLUG_POWER_DOWN))
	{

		P_HDMI_UnsetHotplugTimer();
	}
	#endif
	/*************************
	*	3. Send Msg to the TASK.
	*************************/
	HAPPY(iParam);

	PrintDebug("[P_HDMI_hotplug_callback] s_hdmi_instance.eHotplugState(%d) -> eHotplugState(%d)\n", s_hdmi_instance.eHotplugState, eHotplugState);
	if (s_hdmi_instance.eHotplugState != eHotplugState)
	{
		if (eHotplugState == DI_HDMI_HOTPLUG_DISCONNECTED)
		{
			#ifdef HPD_DELAY
			P_HDMI_UnsetHotplugTimer();
			#endif

#if !defined(RX_POWERDOWNMODE) /* remove */
#if defined(WORKAROUND_RXSENSE_POLLING)
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_RXPOWER_DISCONNECTED;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
#endif
#endif /* end of !RX_POWERDOWNMODE */
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_HOTPLUG_DISCONNECTED;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}
		else if (eHotplugState==DI_HDMI_HOTPLUG_CONNECTED)
		{
			P_HDMI_SetDitheringData();
			P_HDMI_SetPauseBurstData();

			#ifdef HPD_DELAY
			P_HDMI_SetHotplugTimer();
			#else
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_HOTPLUG_CONNECTED;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
			#endif
		}
		else if (eHotplugState == DI_HDMI_HOTPLUG_POWER_DOWN)
		{
			#ifdef HPD_DELAY
			P_HDMI_UnsetHotplugTimer();
			#endif

			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_HOTPLUG_POWER_DOWN;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}
		else
		{
			/*	Nothing to do.
			*/
		}
	}
#if (NEXUS_VERSION > 1000)
	else
	{
		//recover from hdcp fail
		//20130521. 현 시점에 여기 들어오는 일은 없을 것으로 추정
		if (eHotplugState==DI_HDMI_HOTPLUG_CONNECTED && s_hdmi_instance.eHdcpStatus == DI_HDMI_HDCP_FAIL)
		{

			PrintError("[P_HDMI_hotplug_callback] HPD CONNECTED CALLBACK TWICE\n");

			HdmiMsg.eHdmiMsg=HDMI_MSG_CMD_DISABLE_HDCP;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));

			HdmiMsg.eHdmiMsg=HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));

			s_hdmi_instance.eHotplugState = DI_HDMI_HOTPLUG_DISCONNECTED;

			#ifdef HPD_DELAY
			P_HDMI_SetHotplugTimer();
			#else
			s_hdmi_instance.eHotplugState = DI_HDMI_HOTPLUG_DISCONNECTED;
			HdmiMsg.eHdmiMsg=HDMI_MSG_EVT_HOTPLUG_CONNECTED;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
			#endif
		}
	}
#endif
#if defined(RX_POWERDOWNMODE) || defined(WORKAROUND_RXSENSE_POLLING)
	s_hdmi_instance.bSetPollingEvent = FALSE;
#endif

done:
	PrintExit();
	return;
}

#if defined(CONFIG_HDMI_CEC)
static void P_HDMI_CEC_DeviceReady_callback(void *context, int param)
{
	HAPPY(context);
	HAPPY(param);

	PrintEnter();

	VK_EVENT_Send(s_ulHdmiSyncCallEventId, 0);

#if defined(CONFIG_DEBUG)
	{
		NEXUS_CecStatus status;
		NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &status);

		PrintDebug("BCM%d Logical Address <%d> Acquired\n", BCHP_CHIP, status.logicalAddress);
		PrintDebug("BCM%d Physical Address: %X.%X.%X.%X\n", BCHP_CHIP,
														(status.physicalAddress[0] & 0xF0) >> 4,
														(status.physicalAddress[0] & 0x0F),
														(status.physicalAddress[1] & 0xF0) >> 4,
														(status.physicalAddress[1] & 0x0F));

		if ((status.physicalAddress[0] & 0xFF) && (status.physicalAddress[1] = 0xFF))
		{
			PrintDebug("CEC Devie Ready!\n");
		}
	}
#endif
	PrintExit();

	return;
}

static void P_HDMI_CEC_MsgReceived_callback(void *context, int param)
{
	NEXUS_CecStatus status;
	NEXUS_CecReceivedMessage receivedMessage;
	NEXUS_Error eNexusError=NEXUS_SUCCESS;
#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	int i, j;
	char msgBuffer[16];
#endif
	DI_HDMI_CEC_MSG_t 	tDiCecMsg;

	PrintEnter();

	HAPPY(context);
	HAPPY(param);
	VK_EVENT_Send(s_ulHdmiSyncCallEventId, 0);
	eNexusError=NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &status);
#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	PrintDebug("Msg Received Status for Phys/Logical Addrs: %X.%X.%X.%X / %d\n",
		(status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
		(status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
		status.logicalAddress);
	PrintDebug("Received Msg Acknowledged: %s\n", status.transmitMessageAcknowledged ? "Yes" : "No");
	PrintDebug("Received Msg Pending: %s\n", status.messageTransmitPending ? "Yes" : "No");
#endif
	PrintDebug("Message Received: %s\n", status.messageReceived ? "Yes" : "No");
	PrintDebug("CEC Device Type: %d\n", status.deviceType);

	if (status.messageReceived==FALSE)
	{
		goto done;
	}

	eNexusError=NEXUS_Cec_ReceiveMessage(s_hdmi_instance.hCec, &receivedMessage);
	if (eNexusError!=NEXUS_SUCCESS)
	{
		PrintError("@@ERROR@@ %s, eNexusError: 0x%0x\n", __func__, eNexusError);
		goto done;
	}

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	{
		/* For debugging purposes */
		for (i=0, j=0; i<= receivedMessage.data.length; i++)
		{
			j += VK_snprintf(msgBuffer + j, sizeof(msgBuffer)-j, "%02X ", receivedMessage.data.buffer[i]);

		}

		PrintDebug("CEC Message Length %d Received: %s\n", receivedMessage.data.length, msgBuffer);
		PrintDebug("Msg Recd Status from Phys/Logical Addrs: %X.%X.%X.%X / %d\n",
			(status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
			(status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
			status.logicalAddress);
	}
#endif

	if (s_hdmi_instance.pfCallback[DH_EVENT_CEC_MSG_RECEIVED]!=NULL)
	{
		VK_memset(&tDiCecMsg, 0x00, sizeof(DI_HDMI_CEC_MSG_t));
		tDiCecMsg.ucInitAddr=receivedMessage.data.initiatorAddr;
		tDiCecMsg.ucDestAddr=receivedMessage.data.destinationAddr;
		tDiCecMsg.ucMegLength=receivedMessage.data.length;

		VK_memcpy(tDiCecMsg.ucMessage, receivedMessage.data.buffer, receivedMessage.data.length);


		s_hdmi_instance.pfCallback[DH_EVENT_CEC_MSG_RECEIVED]((void *)&tDiCecMsg);
	}

done:
	PrintExit();

	return;
}

static void P_HDMI_CEC_MsgTransmitted_callback(void *context, int param)
{
	HAPPY(context);
	HAPPY(param);

	PrintEnter();

	VK_EVENT_Send(s_ulHdmiSyncCallEventId, 0);
#if defined(CONFIG_DEBUG)
	{
		NEXUS_CecStatus status;

		NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &status);

		PrintDebug("Msg Xmit Status for Phys/Logical Addrs: %X.%X.%X.%X / %d\n",
			(status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
			(status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
			status.logicalAddress);
		PrintDebug("Xmit Msg Acknowledged: %s\n", status.transmitMessageAcknowledged ? "Yes" : "No");
		PrintDebug("Xmit Msg Pending: %s\n", status.messageTransmitPending ? "Yes" : "No");
	}
#endif
	PrintExit();

	return;
}
#endif

#if defined(CONFIG_HDCP)
static void P_HDMI_hdcp_callback(void *pContext, int param)
{//Sunwoo
	DI_HDMI_HDCP_CONNECTION eCurStatus = (HUINT32)param;
	HDMI_MSG_T HdmiMsg;
	NEXUS_Error nResult;

	NEXUS_HdmiOutputHdcpStatus hdcpStatus;
	static HBOOL bWaitngRepeaterReady = FALSE;

	HAPPY(pContext);

	PrintEnter();
	PrintDebug("Hdcp result  %d\n", eCurStatus);

	if(eCurStatus == DI_HDMI_HDCP_SUCCESS_CALLBACK)
	{
		HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP_SUCCESS;
		VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
	}
	else if(eCurStatus == DI_HDMI_HDCP_FAILURE_CALLBACK)
	{
		HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP_FAIL;
		VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
	}
#if 1
	if(eCurStatus == DI_HDMI_UNDEFINED)
	{
		//For HDCP test 1B-03. Irregular procedure: (Second part of authentication) Timeout of KSV list READY
		nResult = NEXUS_HdmiOutput_GetHdcpStatus(s_hdmi_instance.hHdmi, &hdcpStatus);

		if (nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed\n");
			s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_FAIL;
		}

		if(s_hdmi_instance.eHdcpStatus != DI_HDMI_HDCP_SUCCESS)
		{
			if(bWaitngRepeaterReady == FALSE && hdcpStatus.hdcpState == NEXUS_HdmiOutputHdcpState_eWaitForRepeaterReady && hdcpStatus.ksvFifoReady == 0)
			{
				bWaitngRepeaterReady = TRUE;
				P_HDMI_CancelHdcpLongRetryTimer();
				P_HDMI_SetHdcpLongRetryTimer();

			}
		}

		#if 0
		PrintDebug("HDCP State : %d\n", s_hdmi_instance.eHdcpStatus);
		PrintDebug("STATE(%d) Error(%d) ready(%d) Enrypt(%d) Repeater(%d) **FIFOREADY*(%d) 400(%d) 1_1(%d) fast(%d) \n",
		hdcpStatus.hdcpState,
		hdcpStatus.hdcpError,
		hdcpStatus.linkReadyForEncryption,
		hdcpStatus.transmittingEncrypted,
		hdcpStatus.isHdcpRepeater,
		hdcpStatus.ksvFifoReady, hdcpStatus.i2c400Support, hdcpStatus.hdcp1_1Features, hdcpStatus.fastReauthentication);
		#endif
	}
	else if(bWaitngRepeaterReady != FALSE)
	{
		bWaitngRepeaterReady = FALSE;
	}
#endif
	PrintExit();

	return;
}
#endif

static DRV_Error P_HDMI_GetFirstVideoId(HUINT8* pFirstVideoId)
{
	DRV_Error rc=DRV_OK;
	HUINT8 i,// j, /* indexes */
		DataOffset,
		DataBlockIndex,
		DataBlockTag,
		DataBlockLength,
		NumVideoDescriptors,
		EdidVideoIDCode ;
	HUINT8 extensions=0;
	NEXUS_HdmiOutputEdidBlock stEdidBlock;

	PrintEnter();

	/* first get the number of extensions in the EDID */
	/* audio support is always in first V3 Timing Extension; never in block 0 */
	if (NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, 0, &stEdidBlock)==NEXUS_SUCCESS)
	{
		extensions=stEdidBlock.data[DRV_HDMI_EDID_EXTENSION];
	}

	if (!extensions)
	{
		PrintDebug("No EDID Extensions fFound... No HDMI Support") ;
		rc=DRV_ERR;
		goto done ;
	}

	/* check ALL extensions for Version 3 Timing Extensions */
	for (i=1 ; i <= extensions; i++)
	{
		NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, i, &stEdidBlock) ;

		/* check for Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_TAG]!=DRV_HDMI_EDID_EXT_TIMING_DATA)
			continue ;

		/* check for Version 3 Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_VERSION]!=DRV_HDMI_EDID_TIMING_VERSION_3)
			continue ;

		PrintDebug("V3 Timing Extension found in EDID Extension #%d", i) ;

		/* check if data blocks exist before the 18 Byte Detailed Timing data */
		DataOffset=stEdidBlock.data[DRV_HDMI_EDID_EXT_DATA_OFFSET] ;
		if ((DataOffset==0)
			||  (DataOffset==4)) /* no Reserved Data is Available */
		{
			PrintDebug("-----V3 Timing Extension contains no CEA Data Blocks") ;
			continue ;          /* continue to the next Timing Extension */
		}
		PrintDebug("-----CEA Data Blocks found in V3 Timing Extension") ;

		/* set the index to the start of Data Blocks */
		DataBlockIndex=DRV_HDMI_EDID_EXT_DATA_BLOCK_COLLECTION ;

		/* scan through the data blocks and retrieve the necessary information */
		while (DataBlockIndex < DataOffset)
		{
			/* get the Data Block type */
			DataBlockTag=stEdidBlock.data[DataBlockIndex] >> 5 ;

			/* get the Data Block length */
			DataBlockLength=stEdidBlock.data[DataBlockIndex] & 0x1F ;
			/*
			PrintDebug("\n[%02X] CEA-861 %s (0x%02x) found; %d bytes",
			stEdidBlock.data[DataBlockIndex],
			CeaTagName[DataBlockTag], DataBlockTag, DataBlockLength) ;
			*/
			switch (DataBlockTag) {
				/* skip Block Tags that are of no interest to this function */
			case DRV_HDMI_EDID_CeaDataBlockTag_eVSDB :      /* Vendor Specific DB */
			case DRV_HDMI_EDID_CeaDataBlockTag_eAudioDB :   /* Audio DB */
			case DRV_HDMI_EDID_CeaDataBlockTag_eSpeakerDB : /* Speaker Allocation DB */
			case DRV_HDMI_EDID_CeaDataBlockTag_eReserved0 :
			case DRV_HDMI_EDID_CeaDataBlockTag_eReserved5 :
			case DRV_HDMI_EDID_CeaDataBlockTag_eReserved6 :
			case DRV_HDMI_EDID_CeaDataBlockTag_eExtendedDB:
				break ;

			case DRV_HDMI_EDID_CeaDataBlockTag_eVideoDB :   /* Video DB */
				/* check each video descriptor for requested video support */
				NumVideoDescriptors=DataBlockLength ;

				/* for each CEA Video ID Code Found */
				//for (j=0 ; j < NumVideoDescriptors ; j++)
				//{
				/* get the supported Video Code ID; check if a native format */
				EdidVideoIDCode=stEdidBlock.data[DataBlockIndex + 1];//j + 1] ;
				EdidVideoIDCode=EdidVideoIDCode  & 0x7F ;
				*pFirstVideoId=EdidVideoIDCode;
				goto done ;
				//} /* for each Supported CEA Video ID Code */
				break;

				/* return error on unknown Tags */
			default :
				PrintDebug("\nCEA-861 Data Block Tag Code <%d> is not supported", DataBlockTag) ;
				rc=DRV_ERR;
				goto done ;
			} /* for each CEA Video ID Code found */

			DataBlockIndex += DataBlockLength + 1;
		} /* while DataBlockIndex < DataOffset */

	} /* for each extension */

done:
	PrintExit();
	return 	rc ;

}

DRV_Error P_HDMI_EnableHDCP(HBOOL bEnable)
{
#if defined(CONFIG_HDCP)
	NEXUS_HdmiOutputHdcpSettings *pHdcpSettings = NULL;
	NEXUS_Error nResult;
	DRV_Error dResult;
	HINT32 uiIndexX, uiIndexY, uiCount=0;
#endif

	HAPPY(bEnable);

	PrintEnter();

#if defined(CONFIG_HDCP)
	if(bEnable == TRUE)
	{
		/*	NOTICE!!
		*	pHdcpSettings structure variable is too huge to be located in stack.
		*	If it is defined as a local variable, it makes a PREVENT warning.
		*/

		pHdcpSettings = DD_MEM_Alloc(sizeof(NEXUS_HdmiOutputHdcpSettings));
		if(pHdcpSettings == NULL)
		{
			PrintError("Out of memory!\n");
			PrintExit();
			return DRV_ERR;
		}

		NEXUS_HdmiOutput_GetHdcpSettings(s_hdmi_instance.hHdmi, pHdcpSettings);

		/*	1. Register Callback Functons
		*/
		pHdcpSettings->successCallback.callback=P_HDMI_hdcp_callback;
		pHdcpSettings->successCallback.context=s_hdmi_instance.hHdmi;
		pHdcpSettings->successCallback.param=DI_HDMI_HDCP_SUCCESS_CALLBACK;
		pHdcpSettings->failureCallback.callback=P_HDMI_hdcp_callback;
		pHdcpSettings->failureCallback.context=s_hdmi_instance.hHdmi;
		pHdcpSettings->failureCallback.param=DI_HDMI_HDCP_FAILURE_CALLBACK;
		pHdcpSettings->stateChangedCallback.callback=P_HDMI_hdcp_callback;
		pHdcpSettings->stateChangedCallback.context=s_hdmi_instance.hHdmi;
		pHdcpSettings->stateChangedCallback.param=DI_HDMI_STATE_CHANGE_CALLBACK;
		pHdcpSettings->transmitEncrypted=true;

		/*	2. Key Validation Check
		*/
#if defined(RELOAD_VALID_HDCP_KEY)
		for(uiIndexX=0; uiIndexX<DRV_HDMI_HDCPKEY_VERTOR_SIZE; uiIndexX++)
		{
			for(uiIndexY=0; uiIndexY<8; uiIndexY++)
			{
				if (s_ucaHdcpKey[uiIndexX] & (0x01<<uiIndexY))
				{
					uiCount++;
				}
			}
		}
		if (uiCount!=20)
		{
			PrintError("Invalid Key!! Reading again the HDCP Key from nvram!\n");
			dResult = DRV_NVRAM_GetField(DI_NVRAM_FIELD_HDCP_KEY, 0, s_ucaHdcpKey, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_FLASH_GetField failed!\n");
				DD_MEM_Free(pHdcpSettings);
				PrintExit();
				return DRV_ERR;
			}
		}
		else if (s_hdcpKeyValid == FALSE)
		{
			s_hdcpKeyValid = TRUE;
		}
#endif
		/*	3. Key Setting
		*/
#if defined(CONFIG_HDCP_22)
		if((s_pucHdcp22Key != NULL) && !(s_nHdcp22KeySize < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER)))
		{
			nResult = NEXUS_HdmiOutput_SetHdcp2xBinKeys(s_hdmi_instance.hHdmi, s_pucHdcp22Key, s_nHdcp22KeySize);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("Error : NEXUS_HdmiOutput_SetHdcp2xBinKeys() - HDCP2.x will not work.\n");
				DD_MEM_Free(pHdcpSettings);
				PrintExit();
				return DRV_ERR;
			}
		}
#endif
		VK_MEM_Memcpy((void *)pHdcpSettings->encryptedKeySet,
			&s_ucaHdcpKey[DRV_HDMI_HDCPKEY_VERTOR_SIZE+DRV_HDMI_HDCPKEY_DUMMY_SIZE],
			DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE-(DRV_HDMI_HDCPKEY_VERTOR_SIZE+DRV_HDMI_HDCPKEY_DUMMY_SIZE));
		VK_MEM_Memcpy((void *)pHdcpSettings->aksv.data, &s_ucaHdcpKey[0],NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH);

		nResult = NEXUS_HdmiOutput_SetHdcpSettings(s_hdmi_instance.hHdmi, pHdcpSettings);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_SetHdcpSettings failed!\n");
			DD_MEM_Free(pHdcpSettings);
			PrintExit();
			return DRV_ERR;
		}

		/*	4. Start Authentication processes
		*/
		//block out revoked key process. .
		#if 0
		nResult =NEXUS_HdmiOutput_SetHdcpRevokedKsvs(s_hdmi_instance.hHdmi, (NEXUS_HdmiOutputHdcpKsv*)s_ucaHdcpKey, 40);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_SetHdcpRevokedKsvs failed!\n");
			DD_MEM_Free(pHdcpSettings);
			PrintExit();
			return DRV_ERR;
		}
		#endif

		nResult = NEXUS_HdmiOutput_StartHdcpAuthentication(s_hdmi_instance.hHdmi);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_StartHdcpAuthentication failed!\n");
			DD_MEM_Free(pHdcpSettings);
			PrintExit();
			return DRV_ERR;
		}
		DD_MEM_Free(pHdcpSettings);
	}
	else if(bEnable == FALSE)
	{
		nResult = NEXUS_HdmiOutput_DisableHdcpAuthentication(s_hdmi_instance.hHdmi);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_DisableHdcpAuthentication failed!\n");
			PrintExit();
			return DRV_ERR;
		}
	}
#endif

	PrintExit();
	return DRV_OK;
}

void P_HDMI_FireCallbackFunc(DI_HDMI_EVENT eEvent)
{
	PrintEnter();

	VK_INTR_Event(s_hdmi_instance.interruptId, &eEvent);

	PrintExit();

	return;
}

void P_HDMI_MakeEvent(HUINT32 uiNum, DI_HDMI_HDCP_CONNECTION eEvent)
{
	HUINT32 i=0;
	HDMI_MSG_T HdmiMsg;

	PrintEnter();

	for(i=0; i<uiNum; i++)
	{
		if(eEvent == DI_HDMI_HDCP_SUCCESS_CALLBACK)
		{
			HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP_SUCCESS;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}
		else if(eEvent == DI_HDMI_HDCP_FAILURE_CALLBACK)
		{
			HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP_FAIL;
			VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		}

		HdmiMsg.eHdmiMsg = eEvent;
		VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
	}

	PrintExit();

	return;
}

DRV_Error P_HDMI_SetAVMute(HBOOL bMute)
{//Sunwoo
	NEXUS_Error nResult;

	PrintEnter();

	nResult = NEXUS_HdmiOutput_SetAVMute(s_hdmi_instance.hHdmi, bMute);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetAVMute failed!\n");
		PrintExit();
		return DRV_ERR;
	}
	PrintExit();
	return DRV_OK;
}

DRV_Error P_HDMI_SetAvBlank(HBOOL bBlank)
{
	NEXUS_Error nResult;

	PrintEnter();

	nResult = NEXUS_HdmiOutput_SetAVBlank(s_hdmi_instance.hHdmi, bBlank);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetAVBlank failed!\n");
		PrintExit();
		return DRV_ERR;
	}
	PrintExit();
	return DRV_OK;
}

DRV_Error P_HDMI_SetTmdsSignal(HBOOL bOnOff)
{//Sunwoo
    NEXUS_Error nResult;

#if (NEXUS_VERSION >= 1403)
	NEXUS_HdmiOutputTmdsSignalSettings tmds;

	PrintEnter();

    tmds.clock = bOnOff;
    tmds.data = bOnOff;
	nResult = NEXUS_HdmiOutput_SetTmdsSignal(s_hdmi_instance.hHdmi, &tmds);
	if(nResult != NEXUS_SUCCESS)
	{    
		PrintError("NEXUS_HdmiOutput_SetTmdsSignal failed!\n");
		PrintExit();
		return DRV_ERR;
	} 
#else 
	PrintEnter();

	nResult = NEXUS_HdmiOutput_SetTmdsSignal(s_hdmi_instance.hHdmi, bOnOff);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetTmdsSignal failed!\n");
		PrintExit();
		return DRV_ERR;
	}
#endif

	PrintExit();
	return DRV_OK;
}

#ifdef NEXUS_AVIINFO_FUNCTION
//use Broadcom function from nexus 12.4
//Broadcom :  NEXUS_HdmiOutput_SetAviInfoFrame
//Humax :      NEXUS_HdmiOutput_SetAVInfoFrame
DRV_Error P_HDMI_SetAviInfoAspect(DI_HDMI_VIDEO_AR eAR, DI_HDMI_VIDEO_AFD eAFD)
{
	NEXUS_HdmiAviInfoFrame AviInfoFrame;
	NEXUS_Error		nResult = NEXUS_SUCCESS;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	nResult = NEXUS_HdmiOutput_GetAviInfoFrame(s_hdmi_instance.hHdmi, &AviInfoFrame);

	if (nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetAviInfoFrame Error \n");
		dResult = DRV_ERR;
	}
	else
	{
		AviInfoFrame.bOverrideDefaults = TRUE;
		AviInfoFrame.ePictureAspectRatio = eAR;
    	AviInfoFrame.eActiveFormatAspectRatio = eAFD;

		NEXUS_HdmiOutput_SetAviInfoFrame(s_hdmi_instance.hHdmi, &AviInfoFrame);

		if (nResult != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_SetAviInfoFrame Error \n");
			dResult = DRV_ERR;
		}
	}

	PrintExit();
	return dResult;
}
#else
//use Humax function before 12.4.
DRV_Error P_HDMI_SetAviInfoAspect(DI_HDMI_VIDEO_AR eAR, DI_HDMI_VIDEO_AFD eAFD)
{
	NEXUS_Error		nResult = NEXUS_SUCCESS;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	nResult = NEXUS_HdmiOutput_SetAVInfoFrame(s_hdmi_instance.hHdmi, eAR, eAFD);

	if (nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetAVInfoFrame Error \n");
		dResult = DRV_ERR;
	}

	PrintExit();
	return dResult;
}
#endif

void P_HDMI_SetColorSpaceInfo(DI_HDMI_AUX_VIDEO_INFO stAVInfo)
{
	HDMI_MSG_T HdmiMsg;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_COLORSPACE;
	HdmiMsg.param1 = stAVInfo.eColorSpace;
	VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
	PrintExit();
	return;
}

DRV_Error P_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT e3dFormat)
{
	NEXUS_Error eNexusError = NEXUS_SUCCESS;
	NEXUS_HdmiOutputVendorSpecificInfoFrame vsi;

	PrintEnter();

	/* set the VSI */
	NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(s_hdmi_instance.hHdmi, &vsi);
	switch(e3dFormat)
	{
		case DI_HDMI_3D_FORMAT_NONE:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
			break;
		case DI_HDMI_3D_FORMAT_SIDEBYSIDE:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
			vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideHalf;
			break;
		case DI_HDMI_3D_FORMAT_TOPANDBOTTOM:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
			vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eTopAndBottom;
			break;
		case DI_HDMI_3D_FORMAT_FRAMEPACKING:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
			vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eFramePacking;
			break;
		default:
			vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
			break;
	}
	eNexusError = NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(s_hdmi_instance.hHdmi, &vsi);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_HdmiOutput_SetVendorSpecificInfoFrame error\n", __func__);
		PrintExit();
		return DRV_ERR;
	}
	PrintExit();
	return DRV_OK;
}

DRV_Error P_HDMI_SetAudioMute(HBOOL bMute)
{
#if 0
	NEXUS_Error nResult;

	PrintEnter();

	nResult = NEXUS_HdmiOutput_SetAudioMute(s_hdmi_instance.hHdmi, bMute);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetAudioMute failed! nResult=%x\n", nResult);
		PrintExit();
		return DRV_ERR;
	}
	PrintExit();
#else
	BSTD_UNUSED(bMute);
#endif
	return DRV_OK;
}

DRV_Error P_HDMI_SetPowerState(HBOOL bPowerOn)
{
	PrintEnter();

#ifdef HDMI_POWER_CONTROL
	if(s_hdmi_instance.bDevicePowerState != bPowerOn)
	{

		NEXUS_HdmiOutput_SetPowerState( s_hdmi_instance.hHdmi, bPowerOn);
		s_hdmi_instance.bDevicePowerState = bPowerOn;

		s_hdmi_instance.eHotplugState=DI_HDMI_HOTPLUG_DISCONNECTED;
#if defined(CONFIG_TEMP_WA7500) || defined(CONFIG_TEMP_WA8500) || defined(CONFIG_TEMP_WA7000RN)
		if(bPowerOn != TRUE)
		{
			P_HDMI_SetTmdsSignal(bPowerOn);
		}
#else
		P_HDMI_SetTmdsSignal(bPowerOn);
#endif

		if(bPowerOn)P_HDMI_hotplug_callback(s_hdmi_instance.hHdmi, (int)s_hdmi_instance.hDisplayHd);

	}
	else
	{
		PrintError("Setting HDMI Power skipped. Aleady Power %s State.\n", bPowerOn? "On":"Off");
	}
#else
	HAPPY(bPowerOn);
	PrintError("Power state control function not installed yet.");
#endif

	PrintExit();

	return DRV_OK;
}



DRV_Error P_HDMI_EnableHdmiMode(HBOOL enable)
{
	NEXUS_Error nexus_rc;
	
#if defined(CONFIG_AUDIO_DECODER) 
	DRV_Error	dResult;
	HBOOL		needToStopAudio = FALSE;
#if defined(WORKAROUND_HDMI_POPNOISE_SHARP_LC_32GD7)
	char monitorName[14];
	NEXUS_HdmiOutputSettings hdmiSettings;
#endif
#endif

	PrintEnter();

	if (enable == TRUE)
	{
#if defined(CONFIG_AUDIO_DECODER)
#if defined(WORKAROUND_HDMI_POPNOISE_SHARP_LC_32GD7)
		/* Sharp TV pop noise workaround */
		VK_MEM_Memset(monitorName, 0x0, sizeof(monitorName));
		dResult = DRV_HDMI_GetMonitorName(monitorName);
		if(dResult == DRV_OK)
		{
			PrintDebug("[%s] Monitor name is %s.\n", __FUNCTION__, monitorName);
			NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
			if(!VK_strstr(monitorName, "SHARP"))
			{
				dResult = DRV_AUDIO_PlaybackDummyToHDMI(TRUE);
			}
		}
#else
		dResult = DRV_AUDIO_PlaybackDummyToHDMI(TRUE);
#endif
		if (dResult == DRV_OK)
		{
			needToStopAudio = TRUE;
		}
#endif

		nexus_rc=NEXUS_Display_AddOutput(s_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(s_hdmi_instance.hHdmi));
		if (nexus_rc != NEXUS_SUCCESS)
		{
			PrintError("[P_HDMI_EnableHdmiMode] Composite port can't be added!! \n");
		}

#if defined(CONFIG_AUDIO_DECODER)
		if (needToStopAudio == TRUE)
		{
			DRV_AUDIO_PlaybackDummyToHDMI(FALSE);
		}
#endif
	}
	else
	{
		nexus_rc = NEXUS_Display_RemoveOutput(s_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(s_hdmi_instance.hHdmi));
		if(nexus_rc != NEXUS_SUCCESS)
		{
			PrintError("[P_HDMI_EnableHdmiMode] NEXUS_Display_RemoveOutput failed!\n");
		}
	}

	PrintExit();
	return DRV_OK;
}

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
DRV_Error P_HDMI_SetPreEmphasisConfiguration(void)
{
	HBOOL bCustomSetting = FALSE;
	HUINT16 wCustomPreemp = 0;
	HUINT8 cIndex = 0;

	NEXUS_DisplaySettings displaySettings;;
	NEXUS_HdmiOutputPreEmphasisConfiguration PreEmphasisConfiguration;
	NEXUS_HdmiOutputSettings hdmiSettings;

	NEXUS_Error rc = NEXUS_SUCCESS;

	PrintEnter();

	if(s_hdmi_instance.bInitialized != TRUE)
	{
		PrintError("[%s] Hdmi is not Initialized, rc = %d",__func__);
		PrintExit();
		return DRV_ERR;

	}

	//W/A. need to wait till resolution change finished
	VK_TASK_Sleep(50);

	NEXUS_Display_GetSettings(s_hdmi_instance.hDisplayHd, &displaySettings);
	NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);

	cIndex = 0;

	while(CustomPreempTable[cIndex].video_format!= NEXUS_VideoFormat_eMax)
	{
		if(CustomPreempTable[cIndex].video_format== displaySettings.format)
		{
			if(CustomPreempTable[cIndex].color_depth == hdmiSettings.colorDepth)
			{
				wCustomPreemp = CustomPreempTable[cIndex].custum_preemp;
				bCustomSetting = TRUE;
			}
		}

		cIndex++;
	};

	if(bCustomSetting != TRUE)
	{
		PrintDebug("Custom HDMI Preemphasis Not Defined\n");
		PrintExit();
		return DRV_OK;
	}

	rc = NEXUS_HdmiOutput_GetPreEmphasisConfiguration(s_hdmi_instance.hHdmi, &PreEmphasisConfiguration);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetPreEmphasisConfiguration() Failed, rc = %d", rc);
		PrintExit();
		return DRV_ERR;
	}

	PrintError("preEmphasis value = %x\n",wCustomPreemp);

	PreEmphasisConfiguration.preEmphasis_CK = wCustomPreemp;
	PreEmphasisConfiguration.preEmphasis_Ch0 = wCustomPreemp;
	PreEmphasisConfiguration.preEmphasis_Ch1 = wCustomPreemp;
	PreEmphasisConfiguration.preEmphasis_Ch2 = wCustomPreemp;

	rc = NEXUS_HdmiOutput_SetPreEmphasisConfiguration(s_hdmi_instance.hHdmi, &PreEmphasisConfiguration);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetPreEmphasisConfiguration() Failed, rc = %d", rc);
		PrintExit();
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}
#endif

#if __________task__________
#endif

#define MAX_EDID_ATTEPMT	20
#define EDID_READ_INTERVAL 100

static void P_HDMI_Task(void *pvParam)
{
	HDMI_MSG_T HdmiMsg;
	HUINT32			countRetry;
	int				vkMsg;
	NEXUS_Error		nResult;
	DRV_Error		dResult;
	NEXUS_HdmiOutputHdcpStatus hdcpStatus;
	NEXUS_HdmiOutputSettings hdmiSettings;
	NEXUS_HdmiOutputStatus hdmiOutputStatus;

	HAPPY(pvParam);

	/*	NOTICE!!
	*	Letting MW know that the event occured makes DI_HDMI_EnableOutput function called.
	*	So If DI_HDMI_EnableOutput is called already, Should I give any notice to MW about that event?
	*/

	while (1)
	{
		vkMsg = VK_MSG_Receive(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
		if (vkMsg != VK_OK)
		{
			continue;
		}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(DRV_PM_GetStandbyStatus() == TRUE)
		{
			PrintDebug("[%s] Stanby mode: skip status checking..\n", __func__);
			continue;
		}
#endif

#if defined(CONFIG_DEBUG)
		PrintDebug(PARAM_COLOR_ON"P_HDMI_TASK : %s\n"PARAM_COLOR_OFF, EventString[HdmiMsg.eHdmiMsg]);
#endif

#if 0
		PrintDebug(PARAM_COLOR_ON"\ts_hdmi_instance.eHotplugState : %d\n"PARAM_COLOR_OFF, s_hdmi_instance.eHotplugState);
		PrintDebug(PARAM_COLOR_ON"\ts_hdmi_instance.eHdcpStatus : %d\n"PARAM_COLOR_OFF, s_hdmi_instance.eHdcpStatus);
		PrintDebug(PARAM_COLOR_ON"\ts_hdmi_instance.bEnableOutput : %d\n"PARAM_COLOR_OFF, s_hdmi_instance.bEnableOutput);

		PrintDebug(PARAM_COLOR_ON"\ts_hdmi_request.bHdcpEnable : %d\n"PARAM_COLOR_OFF, s_hdmi_request.bHdcpEnable);
		PrintDebug(PARAM_COLOR_ON"\ts_hdmi_request.bHdmiEnable : %d\n"PARAM_COLOR_OFF, s_hdmi_request.bHdmiEnable);
		PrintDebug(PARAM_COLOR_ON"\ts_hdmi_request.bHdmiPowerState : %d\n"PARAM_COLOR_OFF, s_hdmi_request.bHdmiPowerState);
#endif

		VK_SEM_Get(s_hdmi_instance.ulSemId);
		switch (HdmiMsg.eHdmiMsg)
		{
		case HDMI_MSG_EVT_HOTPLUG_DISCONNECTED :
			if (s_hdmi_instance.eHotplugState != DI_HDMI_HOTPLUG_DISCONNECTED)
			{
				//s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_INIT;
				s_hdmi_instance.eHotplugState = DI_HDMI_HOTPLUG_DISCONNECTED;
#if !defined(RX_POWERDOWNMODE) /* remove */
				s_hdmi_instance.eRxsenceState = DI_HDMI_RXSENCE_DISCONNECTED;
#endif /* end of !RX_POWERDOWNMODE */

				P_HDMI_FireCallbackFunc(DH_EVENT_HOTPLUG_DISCONNECT);

				HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
				VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
#if defined(RX_POWERDOWNMODE) ||  !defined(WORKAROUND_RXSENSE_POLLING)
				if (s_hdmi_instance.eHdcpStatus != DI_HDMI_HDCP_INIT)
				{
					HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDCP;
					VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
				}
#endif
			}
			else
			{
				PrintError("HPD aleady in disconnected state\n");
			}
			s_hdmi_instance.lastRxPowerTime = 0;

			break;
		case HDMI_MSG_EVT_HOTPLUG_CONNECTED:
			if (s_hdmi_instance.eHotplugState != DI_HDMI_HOTPLUG_CONNECTED)
			{
				s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_INIT;
				s_hdmi_instance.eHotplugState = DI_HDMI_HOTPLUG_CONNECTED;
#if !defined(RX_POWERDOWNMODE)&& !defined(WORKAROUND_RXSENSE_POLLING) /* remove//  need to check the condition */
				s_hdmi_instance.eRxsenceState = DI_HDMI_RXSENCE_CONNECTED;
#endif
				if (s_hdmi_instance.pfCallback[DH_EVENT_HOTPLUG_CONNECT]!=NULL)
				{
					DI_HDMI_AVAILABLE_FORMATS stAvailable;
					for (countRetry=0; countRetry<MAX_EDID_ATTEPMT; countRetry++)
					{
						if (P_HDMI_GetCapability(&stAvailable)==DI_ERR_OK)
						{
							PrintDebug("Hdmi Status: HDMI_MSG_EVT_HOTPLUG_CONNECTED\n");
							P_HDMI_FireCallbackFunc(DH_EVENT_HOTPLUG_CONNECT);
							break;
						}

						if (s_hdmi_instance.eHotplugState!=DI_HDMI_HOTPLUG_CONNECTED)
							break;

						VK_TASK_Sleep(EDID_READ_INTERVAL);
					}
					if (countRetry == MAX_EDID_ATTEPMT)
					{
						PrintDebug("Hdmi Status: Dummy HDMI_MSG_EVT_HOTPLUG_CONNECTED\n");
						P_HDMI_FireCallbackFunc(DH_EVENT_HOTPLUG_CONNECT);
					}
				}
				if (s_hdmi_request.bHdmiEnable == TRUE)
				{
					HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT;
					VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
				}
#if defined(RX_POWERDOWNMODE)||  !defined(WORKAROUND_RXSENSE_POLLING)
				s_hdcpShortRetryCount = 0;
				P_HDMI_UnsetHdcpCheckPollingTimer();
				if (s_hdmi_request.bHdcpEnable == TRUE)
				{
					s_hdmiHdcpRetryInterval = 300;
					P_HDMI_SetHdcpShortRetryTimer();
				}
#endif
			}
			else
			{
				PrintError("HPD aleady in connected state\n");
			}

			s_hdmi_instance.lastRxPowerTime = 0;

			break;
		case HDMI_MSG_EVT_HOTPLUG_POWER_DOWN :
			if (s_hdmi_instance.eHotplugState != DI_HDMI_HOTPLUG_POWER_DOWN)
			{
				DI_UART_Print(PARAM_COLOR_ON"[P_HDMI_Task] HDMI_MSG_EVT_HOTPLUG_POWER_DOWN\n"PARAM_COLOR_OFF);
				//s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_INIT;
				s_hdmi_instance.eHotplugState = DI_HDMI_HOTPLUG_POWER_DOWN;
#if !defined(RX_POWERDOWNMODE) &&  !defined(WORKAROUND_RXSENSE_POLLING)/* remove//  need to check the condition*/
				s_hdmi_instance.eRxsenceState = DI_HDMI_RXSENCE_POWER_DOWN;
#endif
				PrintDebug("Hdmi Status: HDMI_MSG_EVT_HOTPLUG_POWER_DOWN\n");

				P_HDMI_FireCallbackFunc(DH_EVENT_HOTPLUG_POWER_DOWN);

#if defined(RX_POWERDOWNMODE) || !defined(WORKAROUND_RXSENSE_POLLING)
				if (s_hdmi_instance.bEnableOutput != FALSE)
				{
					HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
					VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
				}
				if (s_hdmi_instance.eHdcpStatus != DI_HDMI_HDCP_INIT)
				{
					HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDCP;
					VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
				}
#endif
			}
			else
			{
				PrintError("HPD aleady in power down state\n");
			}
			s_hdmi_instance.lastRxPowerTime = 0;

			break;
#if !defined(RX_POWERDOWNMODE) /* remove */
		case HDMI_MSG_EVT_RXPOWER_POWER_DOWN :
			s_hdmi_instance.eRxsenceState = DI_HDMI_RXSENCE_POWER_DOWN;
			s_hdmi_instance.lastRxPowerTime = 0;

			break;
		case HDMI_MSG_EVT_RXPOWER_DISCONNECTED :
			s_hdmi_instance.eRxsenceState = DI_HDMI_RXSENCE_DISCONNECTED;
			s_hdmi_instance.lastRxPowerTime = 0;
			if (s_hdmi_instance.bEnableOutput != FALSE)
			{
				HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
				VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
			}
			if (s_hdmi_instance.eHdcpStatus == DI_HDMI_HDCP_SUCCESS)
			{
				HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDCP;
				VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
			}
			break;
		case HDMI_MSG_EVT_RXPOWER_CONNECTED :
			s_hdmi_instance.eRxsenceState = DI_HDMI_RXSENCE_CONNECTED;
			s_hdmi_instance.lastRxPowerTime = VK_TIMER_GetSystemTick();
			s_hdcpShortRetryCount = 0;
			P_HDMI_UnsetHdcpCheckPollingTimer();

			if (s_hdmi_request.bHdmiEnable != FALSE)
			{
				if (s_hdmi_instance.bEnableOutput == FALSE)
				{
					HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT;
					VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
				}
			}

			if (s_hdmi_request.bHdcpEnable == TRUE)
			{
				s_hdmiHdcpRetryInterval = 300;
				P_HDMI_SetHdcpShortRetryTimer();
			}

			break;
#endif /* end of !RX_POWERDOWNMODE */
		case HDMI_MSG_EVT_HDCP_SUCCESS :
			s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_SUCCESS;
			P_HDMI_FireCallbackFunc(DH_EVENT_HDCP_SUCCESS);

			P_HDMI_SetHdcpCheckPollingTimer();
			//PrintError(PARAM_COLOR_ON"HDCP check by polling start!\n"PARAM_COLOR_OFF);
			break;
		case HDMI_MSG_EVT_HDCP_FAIL :
			s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_FAIL;
			if (s_hdmi_request.bHdcpEnable == TRUE)
			{
				if (s_hdcpShortRetryCount++ < DI_HDMI_HDCP_RETRY_COUNT_MAX)
				{
					if (s_hdcpShortRetryCount == 1)
					{
							P_HDMI_SetHdcpLongRetryTimer();
							PrintError(PARAM_COLOR_ON"HDCP Retry in long timer!\n"PARAM_COLOR_OFF);
					}
					else
					{
						s_hdmiHdcpRetryInterval = 100;
						P_HDMI_SetHdcpShortRetryTimer();
						PrintError(PARAM_COLOR_ON"HDCP Retry in short timer(%d)!\n"PARAM_COLOR_OFF,s_hdcpShortRetryCount );
					}
				}
				else
				{
					s_hdcpShortRetryCount = 0;
					P_HDMI_SetHdcpLongRetryTimer();
					P_HDMI_FireCallbackFunc(DH_EVENT_HDCP_FAIL);
					PrintError(PARAM_COLOR_ON"HDCP Fail callback!\n"PARAM_COLOR_OFF);
				}
			}

			break;
		case HDMI_MSG_CMD_ENABLE_HDCP :
			if (s_hdmi_request.bHdcpEnable == TRUE)
			{
#if defined(RX_POWERDOWNMODE)
				if ((s_hdmi_instance.eHotplugState != DI_HDMI_HOTPLUG_CONNECTED)&&(P_HDMI_GetHotplugStatus()==FALSE))/* nexus state re-check*/
#else
				if (s_hdmi_instance.eRxsenceState != DI_HDMI_RXSENCE_CONNECTED)
#endif /* end of RX_POWERDOWNMODE */
				{
					PrintError(PARAM_COLOR_ON"[P_HDMI_Task] P_HDMI_EnableHDCP aborted(Rx not connected)!\n"PARAM_COLOR_OFF);
					break;
				}

				if(s_hdmi_instance.bEnableOutput == TRUE)
				{
					dResult = P_HDMI_EnableHDCP(TRUE);
				}
				else
				{
					PrintError(PARAM_COLOR_ON"HDMI output is not enabled yet!\n"PARAM_COLOR_OFF);
					dResult = DRV_ERR;
				}

				if (dResult != DRV_OK)
				{
					s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_FAIL;
					PrintError(PARAM_COLOR_ON"P_HDMI_EnableHDCP failed!\n"PARAM_COLOR_OFF);
					if (s_hdcpShortRetryCount++ < DI_HDMI_HDCP_RETRY_COUNT_MAX)
					{
						if (s_hdcpShortRetryCount == 1)
						{
							P_HDMI_SetHdcpLongRetryTimer();
							PrintError(PARAM_COLOR_ON"HDCP Retry in long timer!\n"PARAM_COLOR_OFF);
						}
						else
						{
							s_hdmiHdcpRetryInterval = 100;
							P_HDMI_SetHdcpShortRetryTimer();
							PrintError(PARAM_COLOR_ON"HDCP Retry in short timer(%d)!\n"PARAM_COLOR_OFF,s_hdcpShortRetryCount );
						}
					}
					else
					{
						s_hdcpShortRetryCount = 0;
						P_HDMI_SetHdcpLongRetryTimer();
						P_HDMI_FireCallbackFunc(DH_EVENT_HDCP_FAIL);
						PrintError(PARAM_COLOR_ON"HDCP Fail callback!\n"PARAM_COLOR_OFF);
					}
					s_hdcpCheckPollingCount = 0;
				}
				#if 0
				else
				{
					P_HDMI_SetHdcpCheckPollingTimer();
					PrintError(PARAM_COLOR_ON"HDCP check by polling start!\n"PARAM_COLOR_OFF);
				}
				#endif
			}

			break;
		case HDMI_MSG_CMD_DISABLE_HDCP :
			if(s_hdmi_instance.eHdcpStatus != DI_HDMI_HDCP_INIT)
			{
				dResult = P_HDMI_EnableHDCP(FALSE);
				if(dResult != DRV_OK)
				{
					PrintError(PARAM_COLOR_ON"[P_HDMI_Task] P_HDMI_EnableHDCP(FALSE) failed!\n"PARAM_COLOR_OFF);
				}
				s_hdmi_instance.eHdcpStatus = DI_HDMI_HDCP_INIT;
			}
			else
			{
				PrintError(PARAM_COLOR_ON"HDCP is not enabled or aleady disalbled!\n"PARAM_COLOR_OFF);
			}
			break;
		case HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT :
			if (s_hdmi_request.bHdmiEnable == TRUE)
			{
#if defined(RX_POWERDOWNMODE)
				if (s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED)
#else
				if (s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED && s_hdmi_instance.eRxsenceState == DI_HDMI_RXSENCE_CONNECTED)
#endif /* end of RX_POWERDOWNMODE */
				{
					if (s_hdmi_instance.bEnableOutput == FALSE)
					{
#if !defined(CONFIG_TEMP_HMX4KJP)
						P_HDMI_EnableHdmiMode(TRUE);
#endif
						#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
						dResult = P_HDMI_SetPreEmphasisConfiguration();
						if (dResult != DRV_OK)
						{
							PrintError("P_HDMI_SetPreEmphasisConfiguration failed!\n");
						}
						#endif
						s_hdmi_instance.bEnableOutput = TRUE;
					}
				}
			}

			break;
		case HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT :
			if (s_hdmi_instance.bEnableOutput == TRUE)
			{
#if !defined(CONFIG_TEMP_HMX4KJP)
				P_HDMI_EnableHdmiMode(FALSE);
#endif
				s_hdmi_instance.bEnableOutput = FALSE;
			}
			break;
		case HDMI_MSG_CMD_SET_RESOLUTION :

			nResult = NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &hdmiOutputStatus);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_GetStatus failed!\n");
				hdmiOutputStatus.connected = FALSE;
			}

			if(hdmiOutputStatus.connected == FALSE)
			{
				PrintError("HDMI is not ready.\n");
				dResult = DRV_ERR;
				VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);
				break;
			}

			dResult = P_HDMI_EnableHDCP(FALSE);
			if(dResult != DRV_OK)
			{
				PrintError("P_HDMI_DisableHDCP() failed!\n");
			}

			dResult = DRV_VIDEO_SetDisplayStandard((DI_VIDEO_HD_RESOLUTION)HdmiMsg.param1);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_VIDEO_SetDisplayStandard(%d) failed!\n", (HUINT32)HdmiMsg.param1);
			}

			if(s_hdmi_request.bHdcpEnable)
			{
				HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDCP;
				VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
			}

			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);

			break;
		case HDMI_MSG_CMD_AV_MUTE :
			dResult = P_HDMI_SetAVMute(TRUE);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAVMute failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);

			break;
		case HDMI_MSG_CMD_AV_UNMUTE :
			dResult = P_HDMI_SetAVMute(FALSE);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAVMute(FALSE) failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);

			break;
		case HDMI_MSG_CMD_AUDIO_MUTE :
			dResult = P_HDMI_SetAudioMute(TRUE);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAudioMute failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);

			break;
		case HDMI_MSG_CMD_AUDIO_UNMUTE :
			dResult = P_HDMI_SetAudioMute(FALSE);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAudoMute(FALSE) failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);
			break;
		case HDMI_MSG_CMD_AV_BLANK :
			dResult = P_HDMI_SetAvBlank(TRUE);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAvBlank failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);
			break;
		case HDMI_MSG_CMD_AV_UNBLANK :
			dResult = P_HDMI_SetAvBlank(FALSE);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAvBlank(FALSE) failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);
			break;
		case HDMI_MSG_CMD_AUXINFO :
			dResult = P_HDMI_SetAviInfoAspect(HdmiMsg.param1, HdmiMsg.param2);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAviInfoAspect failed!\n");
			}
			break;
		case HDMI_MSG_CMD_COLORSPACE :
			NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
			if (HdmiMsg.param1 == DI_HDMI_VIDEO_ColorSpace_eAuto)
			{
				hdmiSettings.autoColorSpace = TRUE;	/* if autoColorSpace is true, then whatever color space reported by the HDMI receiver is used (default is true) */
			}
			else if (HdmiMsg.param1 == DI_HDMI_VIDEO_ColorSpace_eRgb || HdmiMsg.param1 == DI_HDMI_VIDEO_ColorSpace_eYCbCr422 || HdmiMsg.param1 == DI_HDMI_VIDEO_ColorSpace_eYCbCr444)
			{
				hdmiSettings.autoColorSpace = FALSE; /* if autoColorSpace is false, then this value is used instead of the value reported by the HDMI receiver */
				hdmiSettings.colorSpace = HdmiMsg.param1;
			}
			nResult = NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
			if (nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_SetSettings failed!\n");
			}

			HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
			VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

			HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT;
			VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

			break;

		case HDMI_MSG_CMD_3D_FORMAT :
			dResult = P_HDMI_Set3DFormat(HdmiMsg.param1);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_Set3DFormat failed!\n");
			}
			VK_EVENT_Send(s_ulHdmiSyncCallEventId, dResult);

			break;
		case HDMI_MSG_CMD_POWER_ON:
			P_HDMI_SetPowerState(TRUE);
			break;
		case HDMI_MSG_CMD_POWER_OFF:
			P_HDMI_UnsetHdcpCheckPollingTimer();
			P_HDMI_SetPowerState(FALSE);
			break;


		case HDMI_MSG_SLF_HDCP_CHECK_POLLING :
			if(s_hdmi_instance.eHotplugState != DI_HDMI_HOTPLUG_CONNECTED)
				break;
#if !defined(RX_POWERDOWNMODE) /* remove */
			if(s_hdmi_instance.eRxsenceState != DI_HDMI_RXSENCE_CONNECTED)
				break;
#endif /* end of !RX_POWERDOWNMODE */
			if(s_hdmi_request.bHdcpEnable != TRUE)
				break;

			nResult = NEXUS_HdmiOutput_GetHdcpStatus(s_hdmi_instance.hHdmi, &hdcpStatus);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed!\n");
			}

			if (hdcpStatus.transmittingEncrypted == true)
			{
				if (s_hdcpCheckPollingCount++ < DI_HDMI_HDCP_CHECK_POLLING_COUNT_MAX)
				{
					P_HDMI_SetHdcpCheckPollingTimer();
				}
				else
				{
					s_hdcpCheckPollingCount = 0;
				}
				break;
			}

#if defined(WORKAROUND_HDMI_HDCP_FAIL_SAMSUNG_XXXXXXXX)
			HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
			VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

			HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT;
			VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
#endif
			HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ENABLE_HDCP;
			VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

			break;
		default :
			PrintError("[P_HDMI_Task] Unknown Msaage : %d\n", HdmiMsg.eHdmiMsg);
			break;
		}
		VK_SEM_Release(s_hdmi_instance.ulSemId);
	}
}

void P_HDMI_Intr(DI_HDMI_EVENT *pEvent)
{
	DI_HDMI_EVENT localEvent;

	PrintEnter();

	if (pEvent == NULL || DH_EVENT_MAX <= *pEvent)
	{
		PrintError("Invalid Parameter. \n" );
		PrintExit();
		return;
	}

	if (s_hdmi_instance.pfCallback[*pEvent]!=NULL)
	{
		PrintDebug("HDMI CALLBACK %d Fired!\n", *pEvent );
		localEvent=*pEvent;
		(*s_hdmi_instance.pfCallback[*pEvent])(&localEvent);
	}
	else
	{
		PrintError("HDMI CALLBACK %d not registered!\n", *pEvent );
	}

	PrintExit();

}


#if __________drv___________
#endif

/////////////////////////////////////////////////////////////////////////////////////////

DRV_Error DRV_HDMI_Init(void)
{
	int iIndex;
	DI_ERR_CODE	rc=DI_ERR_OK;
	NEXUS_Error nexus_rc;
	unsigned char ucSemName[20];
	NEXUS_DisplayHandle hDisplaySd;
	NEXUS_HdmiOutputSettings hdmiSettings;
	NEXUS_PlatformConfiguration platformConfig;

#if defined(CONFIG_HDMI_CEC)
	NEXUS_HdmiOutputStatus hdmiStatus;
	NEXUS_CecSettings cecSettings;
#endif

//	TraceSetModule( TRACE_MODULE_DI_HDMI);
//	TraceSetLevel( TRACE_MODULE_DI_HDMI, TRACE_LEVEL_ALL );

	PrintEnter();

#if defined(DEBUG_HDMI_FULLLOG)
	TraceSetModule( TRACE_MODULE_DI_HDMI);
	TraceSetLevel( TRACE_MODULE_DI_HDMI,	TRACE_LEVEL_ALL);
#endif

	NEXUS_Platform_GetConfiguration(&platformConfig);
	s_hdmi_instance.hHdmi=platformConfig.outputs.hdmi[0];
#if defined(CONFIG_HDMI_CEC)
	s_hdmi_instance.hCec=platformConfig.outputs.cec[0];
#endif

	rc=VK_INTR_Register((void (*)(void *))P_HDMI_Intr, "hdmi_hotplug", sizeof(DI_HDMI_EVENT), &s_hdmi_instance.interruptId, VK_INTR_TASK0);
	if (rc!=VK_OK)
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return DI_ERR_ERROR;
	}

	for (iIndex=0 ; iIndex<DH_EVENT_MAX ; iIndex++)
	{
		s_hdmi_instance.pfCallback[iIndex]=NULL;
	}

	VK_memset(ucSemName, 0x00, 20);
	VK_snprintf((char*)ucSemName, 16, "di_hdmi_di");
	rc=VK_SEM_Create(&s_hdmi_instance.ulSemId, (const char*)ucSemName, DEFAULT_SUSPEND_TYPE);
	if (rc!=VK_OK)
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return DI_ERR_ERROR;
	}

	rc=DRV_VIDEO_GetDisplayHandle(0, &s_hdmi_instance.hDisplayHd, &hDisplaySd);
	if (rc)
	{
		PrintError("[%s] DRV_VIDEO_GetDisplayHandle error!!\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	/* Install hotplug callback -- video only for now */
	NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);

#if defined(CONFIG_DIRECTFB)
	if(hdmiSettings.hotplugCallback.callback != NULL || hdmiSettings.cecCallback.callback != NULL)
	{
		hdmiSettings.hotplugCallback.callback = NULL;
		hdmiSettings.hotplugCallback.context = NULL;

		hdmiSettings.cecCallback.callback = NULL;
		hdmiSettings.cecCallback.context = NULL;
		nexus_rc=NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		if (nexus_rc)
		{
			PrintError("[%s] NEXUS_HdmiOutput_SetSettings error!!\n", __func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
	}
#endif

#if defined(CONFIG_HDMI_NO_EDID)
    hdmiSettings.hotplugCallback.callback=NULL;
#else
	hdmiSettings.hotplugCallback.callback=P_HDMI_hotplug_callback;
#endif
	hdmiSettings.hotplugCallback.context=s_hdmi_instance.hHdmi;
	hdmiSettings.hotplugCallback.param=(int)s_hdmi_instance.hDisplayHd;

	hdmiSettings.preFormatChangeAvMuteDelay = 50;
	hdmiSettings.postFormatChangeAvMuteDelay = 150;

	nexus_rc=NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_HdmiOutput_SetSettings error!!\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

#if defined(CONFIG_HDMI_CEC)
	nexus_rc=NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &hdmiStatus);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_Hdmioutput_GetStatus error!!\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	NEXUS_Cec_GetSettings(s_hdmi_instance.hCec, &cecSettings);

	cecSettings.messageReceivedCallback.callback = P_HDMI_CEC_MsgReceived_callback ;
	cecSettings.messageReceivedCallback.context = NULL;

	cecSettings.messageTransmittedCallback.callback = P_HDMI_CEC_MsgTransmitted_callback;
	cecSettings.messageTransmittedCallback.context = NULL;

	cecSettings.logicalAddressAcquiredCallback.callback = P_HDMI_CEC_DeviceReady_callback ;
	cecSettings.logicalAddressAcquiredCallback.context = NULL;

    PrintDebug("CEC Msg Recd Status from Phys Addrs: %X.%X.%X.%X ",
        (cecSettings.physicalAddress[0] & 0xF0) >> 4, (cecSettings.physicalAddress[0] & 0x0F),
        (cecSettings.physicalAddress[1] & 0xF0) >> 4, (cecSettings.physicalAddress[1] & 0x0F)) ;

	cecSettings.physicalAddress[0]= (hdmiStatus.physicalAddressA << 4)
	    | hdmiStatus.physicalAddressB;
	cecSettings.physicalAddress[1]= (hdmiStatus.physicalAddressC << 4)
	    | hdmiStatus.physicalAddressD;

	/* Enable CEC core */
	cecSettings.enabled = true;

	nexus_rc=NEXUS_Cec_SetSettings(s_hdmi_instance.hCec, &cecSettings);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_Cec_SetSettings error!!\n", __func__);
		//rc=DI_ERR_ERROR;
		//goto done;
	}
#endif

	rc=DRV_NVRAM_GetField(DI_NVRAM_FIELD_HDCP_KEY, 0, s_ucaHdcpKey, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);
	if (rc)
	{
		PrintError("[%s] DRV_NVRAM_GetFild error!!\n", __func__);
	}

#if defined(CONFIG_HDCP_22)
	rc = DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, HDCP22_KEY, &s_nHdcp22KeySize);
	if(rc)
	{
		PrintError("DRV_NVRAM_GetLength(HDCP22_KEY) error(0x%x)!!\n", rc);
		PrintError("Please install HDCP22 KEY using provisioning-tool!\n");
		rc = DRV_OK;
	}
	else
	{
		if(!(s_nHdcp22KeySize < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER)))
		{
			s_pucHdcp22Key = DD_MEM_Alloc(s_nHdcp22KeySize);
			if(s_pucHdcp22Key != NULL)
			{
				rc = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, HDCP22_KEY, 0, s_pucHdcp22Key, s_nHdcp22KeySize);
				if(rc)
				{
					PrintError("Error : DRV_NVRAM_Read(HDCP22_KEY)\n");
					DD_MEM_Free(s_pucHdcp22Key);
					s_pucHdcp22Key = NULL;
				}
				else
				{
					PrintDebug("OK : DRV_NVRAM_Read(HDCP22_KEY) size(%d)\n", s_nHdcp22KeySize);
				}
			}
			else
			{
				PrintError("Error : DD_MEM_Alloc(), size(%d)\n", s_nHdcp22KeySize);
			}
		}
		else
		{
			PrintError("HDCP22_KEY length is %d. Please check your HDCP22 Key!\n", s_nHdcp22KeySize);
		}
	}
#endif

	s_hdmi_instance.eHotplugState=DI_HDMI_HOTPLUG_DISCONNECTED;
#if !defined(RX_POWERDOWNMODE) /* remove */
	s_hdmi_instance.eRxsenceState=DI_HDMI_RXSENCE_MAX;
#endif /* end of !RX_POWERDOWNMODE */
	s_hdmi_instance.bInitialized=TRUE;

	if(VK_EVENT_Create(&s_ulHdmiSyncCallEventId, "HdmiSyncEvent") != VK_OK)
	{
		PrintError("[%s] Creating Hdmi VK_EVENT_Create failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	if(VK_MSG_Create(10, sizeof(HDMI_MSG_T),"HdmiMsgQueue",&ulHdmiMsgQueueId,VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		PrintError("[%s] Creating Hdmi Msg Queue failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	if(VK_TASK_Create(P_HDMI_Task, HDMI_HDCP_TASK_PRIORITY, HDMI_HDCP_TASK_STACK_SIZE, "HdmiTask", (void *)0, &ulHdmiTaskId, FALSE)!=VK_OK)
	{
		PrintError("[%s] Creating HdmiTask failed!\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}
	VK_TASK_Start(ulHdmiTaskId);

	s_HotplugTimer = 0;
	s_hdmi_instance.bDevicePowerState = TRUE;

#if defined(CONFIG_HDMI_NO_EDID)
#else
	P_HDMI_hotplug_callback(s_hdmi_instance.hHdmi, (int)s_hdmi_instance.hDisplayHd);
#endif

#if defined(CONFIG_TEMP_HMX4KJP)
	P_HDMI_EnableHdmiMode(FALSE);
#endif
	P_HDMI_EnableHdmiMode(TRUE);
	s_hdmi_instance.bEnableOutput = TRUE;
	s_hdmi_request.bHdmiEnable = TRUE;

#if defined(WORKAROUND_RXSENSE_POLLING)
	VK_TIMER_EventEvery(DI_HDMI_RXSENSE_POLLING_INTERVAL, P_HDMI_RxSensePollingTimer, NULL, 0, (unsigned long *)&s_RxSensePollingTimer);
#endif

done:
	PrintExit();
	return rc;
}

void DRV_HDMI_UnInit(void)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_Error nexus_rc;
	NEXUS_CecSettings cecSettings;
#endif

	PrintEnter();

#if defined(CONFIG_HDMI_CEC)
	NEXUS_Cec_GetSettings(s_hdmi_instance.hCec, &cecSettings);
	cecSettings.enabled = false;
	nexus_rc=NEXUS_Cec_SetSettings(s_hdmi_instance.hCec, &cecSettings);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_Cec_SetSettings error!!\n", __func__);
	}
#endif

	rc=VK_TIMER_Cancel(s_hdcpShortRetryTimer);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TIMER_Cancel (s_hdcpShortRetryTimer) failed!\n", __FUNCTION__);
	}
	s_hdcpShortRetryTimer=0;

	rc=VK_TIMER_Cancel(s_hdcpLongRetryTimer);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TIMER_Cancel (s_hdcpLongRetryTimer) failed!\n", __FUNCTION__);
	}
	s_hdcpLongRetryTimer=0;

	rc=VK_TIMER_Cancel(s_hdcpCheckPollingTimer);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TIMER_Cancel (s_hdcpCheckPollingTimer) failed!\n", __FUNCTION__);
	}
	s_hdcpCheckPollingTimer=0;

#if defined(WORKAROUND_RXSENSE_POLLING)
	rc=VK_TIMER_Cancel(s_RxSensePollingTimer);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TIMER_Cancel (s_RxSensePollingTimer) failed!\n", __FUNCTION__);
	}
	s_RxSensePollingTimer=0;
#endif

	/* disable audio/video output */
	P_HDMI_EnableHdmiMode(FALSE);

	rc=VK_TASK_Stop(ulHdmiTaskId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TASK_Stop failed!\n", __FUNCTION__);
	}

	rc=VK_TASK_Destroy(ulHdmiTaskId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TASK_Destroy failed!\n", __FUNCTION__);
	}

	rc=VK_MSG_Destroy(ulHdmiMsgQueueId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_MSG_Destory failed!\n", __FUNCTION__);
	}

	rc=VK_EVENT_Destroy(s_ulHdmiSyncCallEventId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_EVENT_Destory failed!\n", __FUNCTION__);
	}

	PrintExit();

	return;
}

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
DRV_Error DRV_HDMI_SetPreEmphasisConfiguration(void)
{
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	dResult = P_HDMI_SetPreEmphasisConfiguration();
	if (dResult != DRV_OK)
	{
		PrintError("P_HDMI_SetPreEmphasisConfiguration failed!\n");
	}

	PrintExit();

	return dResult;
}
#endif

DRV_Error DRV_HDMI_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDResolution)
{
	int vResult;
	DRV_Error dResult = DRV_ERR;
	HDMI_MSG_T HdmiMsg;
	unsigned long evtResult;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_RESOLUTION;
	HdmiMsg.param1 = eHDResolution;
	vResult = VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
	if (vResult != VK_OK)
	{
		PrintError("VK_MSG_SendTimeout failed!\n");
	}
	vResult = VK_EVENT_ReceiveTimeout(s_ulHdmiSyncCallEventId, &evtResult, 500);
	if (vResult != VK_OK)
	{
		PrintError("VK_EVENT_ReceiveTimeout failed!\n");
	}
	dResult = (DRV_Error)evtResult;

	PrintExit();
	return dResult;
}

DRV_Error DRV_HDMI_SetAVMute(HBOOL bMute)
{
	HINT32 vResult;
	DRV_Error dResult = DRV_ERR;
	HDMI_MSG_T HdmiMsg;
	unsigned long evtResult;

	PrintEnter();
	PrintDebug("DRV_HDMI_SetAVMute bMute: %s\n", bMute?"T":"F");

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if (s_hdmi_request.bHdmiAvMute == bMute)
	{
		PrintError("[DRV_HDMI_SetAVMute] AV Mute(%d) was requested already !!\n", bMute);
		dResult = DRV_OK;
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		goto ret;
	}

	s_hdmi_request.bHdmiAvMute = bMute;
	VK_SEM_Release(s_hdmi_instance.ulSemId);
	HdmiMsg.eHdmiMsg = (bMute == TRUE) ? HDMI_MSG_CMD_AV_MUTE : HDMI_MSG_CMD_AV_UNMUTE;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
	VK_EVENT_ReceiveTimeout(s_ulHdmiSyncCallEventId, &evtResult, 500);
	dResult = (DRV_Error)evtResult;

ret:
	PrintExit();
	return dResult;
}

DRV_Error DRV_HDMI_SetAvBlank(HBOOL bBlank)
{
	HINT32 vResult;
	DRV_Error dResult = DRV_ERR;
	HDMI_MSG_T HdmiMsg;
	unsigned long evtResult;

	PrintEnter();
	PrintDebug("DRV_HDMI_SetAvBlank bBlank: %s\n", bBlank?"T":"F");

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if (s_hdmi_request.bHdmiAvBlank == bBlank)
	{
		PrintError("[DRV_HDMI_SetAvBlank] AV Blank(%d) was requested already !!\n", bBlank);
		dResult = DRV_OK;
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		goto ret;
	}

	s_hdmi_request.bHdmiAvBlank = bBlank;
	VK_SEM_Release(s_hdmi_instance.ulSemId);

	HdmiMsg.eHdmiMsg = (bBlank == TRUE) ? HDMI_MSG_CMD_AV_BLANK : HDMI_MSG_CMD_AV_UNBLANK;

	vResult = VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
	if (vResult != VK_OK)
	{
		PrintError("VK_MSG_SendTimeout failed!\n");
	}
	vResult = VK_EVENT_ReceiveTimeout(s_ulHdmiSyncCallEventId, &evtResult, 500);
	if (vResult != VK_OK)
	{
		PrintError("VK_EVENT_ReceiveTimeout failed!\n");
	}
	dResult = (DRV_Error)evtResult;

ret:
	PrintExit();
	return dResult;
}

DRV_Error DRV_HDMI_DumpRawEdid(void)
{
	DRV_Error rc=DRV_OK;
	NEXUS_HdmiOutputEdidBlock stEdidBlock;
	unsigned char NumExtensions;
	unsigned char i;

	PrintEnter();

	if (s_hdmi_instance.bInitialized==FALSE || VK_SEM_Get(s_hdmi_instance.ulSemId)!=VK_OK)
		return DRV_ERR;

	if (NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, 0, &stEdidBlock)==NEXUS_SUCCESS)
	{
		DI_UART_Dump((unsigned char*)stEdidBlock.data, DRV_HDMI_EDID_BLOCKSIZE, 16);

		NumExtensions=stEdidBlock.data[DRV_HDMI_EDID_EXTENSION];
		for (i=1 ; i<=NumExtensions ; i++)
		{
			/* read the next 128 Byte EDID block */
			if (NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, i, &stEdidBlock)==NEXUS_SUCCESS)
			{
				DI_UART_Dump(stEdidBlock.data, DRV_HDMI_EDID_BLOCKSIZE, 16);
			}
		}
	}
	else
	{
		PrintDebug("[%s] Unable to read EDID block0\n", __func__);
	}

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DRV_Error DRV_HDMI_PrintEdidInfo(void)
{
	DRV_Error rc=DRV_OK;

#if (NEXUS_VERSION >= 1402)
	HUINT32 i;
	NEXUS_HdmiOutputEdidData Edid;

	VK_MEM_Memset(&Edid, 0x0, sizeof(NEXUS_HdmiOutputEdidData));

	if(s_hdmi_instance.bInitialized==FALSE)
	{
		return DRV_ERR;
	}

	if(VK_SEM_Get(s_hdmi_instance.ulSemId)!=VK_OK)
	{
		return DRV_ERR;
	}

	if (NEXUS_HdmiOutput_GetEdidData(s_hdmi_instance.hHdmi, &Edid)==NEXUS_SUCCESS)
	{
		DI_UART_Print("Edid.valid = %d\n", Edid.valid);
		DI_UART_Print("\n");

		for(i=0; i<sizeof(Edid.basicData.vendorID); i++)
		{
			DI_UART_Print("Edid.basicData.vendorID[%d] = %02X\n", i, Edid.basicData.vendorID[i]);
		}
		for(i=0; i<sizeof(Edid.basicData.productID); i++)
		{
			DI_UART_Print("Edid.basicData.productID[%d] = %02X\n", i, Edid.basicData.productID[i]);
		}
		for(i=0; i<sizeof(Edid.basicData.serialNum); i++)
		{
			DI_UART_Print("Edid.basicData.serialNum[%d] = %02X\n", i, Edid.basicData.serialNum[i]);
		}
		DI_UART_Print("Edid.basicData.manufWeek = %02X\n", Edid.basicData.manufWeek);
		DI_UART_Print("Edid.basicData.manufYear = %04X\n", Edid.basicData.manufYear);
		DI_UART_Print("Edid.basicData.edidVersion = %02X\n", Edid.basicData.edidVersion);
		DI_UART_Print("Edid.basicData.edidRevision = %02X\n", Edid.basicData.edidRevision);
		DI_UART_Print("Edid.basicData.maxHorizSize = %02X\n", Edid.basicData.maxHorizSize);
		DI_UART_Print("Edid.basicData.maxVertSize = %02X\n", Edid.basicData.maxVertSize);
		DI_UART_Print("Edid.basicData.extensions = %02X\n", Edid.basicData.extensions);
		DI_UART_Print("Edid.basicData.features = %02X\n", Edid.basicData.features);
		for(i=0; i<sizeof(Edid.basicData.monitorName); i++)
		{
			DI_UART_Print("Edid.basicData.monitorName[%d] = %02X\n", i, Edid.basicData.monitorName[i]);
		}
		DI_UART_Print("Edid.basicData.monitorRange.minVertical = %04X\n", Edid.basicData.monitorRange.minVertical);
		DI_UART_Print("Edid.basicData.monitorRange.maxVertical = %04X\n", Edid.basicData.monitorRange.maxVertical);
		DI_UART_Print("Edid.basicData.monitorRange.minHorizontal = %04X\n", Edid.basicData.monitorRange.minHorizontal);
		DI_UART_Print("Edid.basicData.monitorRange.maxHorizontal = %04X\n", Edid.basicData.monitorRange.maxHorizontal);
		DI_UART_Print("Edid.basicData.monitorRange.secondaryTiming = %04X\n", Edid.basicData.monitorRange.secondaryTiming);
		for(i=0; i<sizeof(Edid.basicData.monitorRange.secondaryTimingParameters); i++)
		{
			DI_UART_Print("Edid.basicData.monitorRange.secondaryTimingParameters[%d] = %02X\n", i, Edid.basicData.monitorRange.secondaryTimingParameters[i]);
		}
		DI_UART_Print("Edid.basicData.preferredVideoFormat = %X\n", Edid.basicData.preferredVideoFormat);
		DI_UART_Print("\n");

		DI_UART_Print("Edid.hdmiVsdb.valid = %d\n", Edid.hdmiVsdb.valid);
		DI_UART_Print("Edid.hdmiVsdb.version = %02X\n", Edid.hdmiVsdb.version);
		for(i=0; i<sizeof(Edid.hdmiVsdb.ieeeRegId); i++)
		{
			DI_UART_Print("Edid.basicData.ieeeRegId[%d] = %02X\n", i, Edid.hdmiVsdb.ieeeRegId[i]);
		}
		DI_UART_Print("Edid.hdmiVsdb.tagCode = %02X\n", Edid.hdmiVsdb.tagCode);
		DI_UART_Print("Edid.hdmiVsdb.physicalAddressA = %02X\n", Edid.hdmiVsdb.physicalAddressA);
		DI_UART_Print("Edid.hdmiVsdb.physicalAddressB = %02X\n", Edid.hdmiVsdb.physicalAddressB);
		DI_UART_Print("Edid.hdmiVsdb.physicalAddressC = %02X\n", Edid.hdmiVsdb.physicalAddressC);
		DI_UART_Print("Edid.hdmiVsdb.physicalAddressD = %02X\n", Edid.hdmiVsdb.physicalAddressD);
		DI_UART_Print("Edid.hdmiVsdb.acpIsrcN = %d\n", Edid.hdmiVsdb.acpIsrcN);
		DI_UART_Print("Edid.hdmiVsdb.deepColor30bit = %d\n", Edid.hdmiVsdb.deepColor30bit);
		DI_UART_Print("Edid.hdmiVsdb.deepColor36bit = %d\n", Edid.hdmiVsdb.deepColor36bit);
		DI_UART_Print("Edid.hdmiVsdb.deepColor48bit = %d\n", Edid.hdmiVsdb.deepColor48bit);
		DI_UART_Print("Edid.hdmiVsdb.deepColorY444 = %d\n", Edid.hdmiVsdb.deepColorY444);
		DI_UART_Print("Edid.hdmiVsdb.dviDual = %d\n", Edid.hdmiVsdb.dviDual);
		DI_UART_Print("Edid.hdmiVsdb.maxTmdsClockRate = %04X\n", Edid.hdmiVsdb.maxTmdsClockRate);
		DI_UART_Print("Edid.hdmiVsdb.latencyFieldsPresent = %d\n", Edid.hdmiVsdb.latencyFieldsPresent);
		DI_UART_Print("Edid.hdmiVsdb.interlacedLatencyFieldsPresent = %d\n", Edid.hdmiVsdb.interlacedLatencyFieldsPresent);
		DI_UART_Print("Edid.hdmiVsdb.hdmiVideoPresent = %d\n", Edid.hdmiVsdb.hdmiVideoPresent);
		DI_UART_Print("Edid.hdmiVsdb.contentTypeGraphicsText = %d\n", Edid.hdmiVsdb.contentTypeGraphicsText);
		DI_UART_Print("Edid.hdmiVsdb.contentTypePhoto = %d\n", Edid.hdmiVsdb.contentTypePhoto);
		DI_UART_Print("Edid.hdmiVsdb.contentTypeCinema = %d\n", Edid.hdmiVsdb.contentTypeCinema);
		DI_UART_Print("Edid.hdmiVsdb.contentTypeGame = %d\n", Edid.hdmiVsdb.contentTypeGame);
		DI_UART_Print("Edid.hdmiVsdb.videoLatency = %02X\n", Edid.hdmiVsdb.videoLatency);
		DI_UART_Print("Edid.hdmiVsdb.audioLatency = %02X\n", Edid.hdmiVsdb.audioLatency);
		DI_UART_Print("Edid.hdmiVsdb.interlacedVideoLatency = %02X\n", Edid.hdmiVsdb.interlacedVideoLatency);
		DI_UART_Print("Edid.hdmiVsdb.interlacedAudioLatency = %02X\n", Edid.hdmiVsdb.interlacedAudioLatency);
		DI_UART_Print("Edid.hdmiVsdb.hdmi3DPresent = %d\n", Edid.hdmiVsdb.hdmi3DPresent);
		DI_UART_Print("Edid.hdmiVsdb.hdmi3DMultiPresent = %02X\n", Edid.hdmiVsdb.hdmi3DMultiPresent);
		DI_UART_Print("Edid.hdmiVsdb.hdmiImageSize = %02X\n", Edid.hdmiVsdb.hdmiImageSize);
		DI_UART_Print("Edid.hdmiVsdb.hdmiVICLen = %02X\n", Edid.hdmiVsdb.hdmiVICLen);
		DI_UART_Print("Edid.hdmiVsdb.hdmi3DLen = %02X\n", Edid.hdmiVsdb.hdmi3DLen);
		DI_UART_Print("Edid.hdmiVsdb.underscan = %d\n", Edid.hdmiVsdb.underscan);
		DI_UART_Print("Edid.hdmiVsdb.audio = %d\n", Edid.hdmiVsdb.audio);
		DI_UART_Print("Edid.hdmiVsdb.yCbCr444 = %d\n", Edid.hdmiVsdb.yCbCr444);
		DI_UART_Print("Edid.hdmiVsdb.yCbCr422 = %d\n", Edid.hdmiVsdb.yCbCr422);
		DI_UART_Print("Edid.hdmiVsdb.nativeFormatsInDescriptors = %02X\n", Edid.hdmiVsdb.nativeFormatsInDescriptors);
		DI_UART_Print("\n");

		DI_UART_Print("Edid.hdmiForumVsdb.valid = %d\n", Edid.hdmiForumVsdb.valid);
		DI_UART_Print("Edid.hdmiForumVsdb.tagCode = %02X\n", Edid.hdmiForumVsdb.tagCode);
		DI_UART_Print("Edid.hdmiForumVsdb.version = %02X\n", Edid.hdmiForumVsdb.version);
		DI_UART_Print("Edid.hdmiForumVsdb.maxTMDSCharacterRate = %04X\n", Edid.hdmiForumVsdb.maxTMDSCharacterRate);
		DI_UART_Print("Edid.hdmiForumVsdb._3dOsdDisparity = %d\n", Edid.hdmiForumVsdb._3dOsdDisparity);
		DI_UART_Print("Edid.hdmiForumVsdb.dualView = %d\n", Edid.hdmiForumVsdb.dualView);
		DI_UART_Print("Edid.hdmiForumVsdb.independentView = %d\n", Edid.hdmiForumVsdb.independentView);
		DI_UART_Print("Edid.hdmiForumVsdb.sub340McscScramble = %d\n", Edid.hdmiForumVsdb.sub340McscScramble);
		DI_UART_Print("Edid.hdmiForumVsdb.readRequest = %d\n", Edid.hdmiForumVsdb.readRequest);
		DI_UART_Print("Edid.hdmiForumVsdb.scdc = %d\n", Edid.hdmiForumVsdb.scdc);
		DI_UART_Print("Edid.hdmiForumVsdb.deepColor420_30bit = %d\n", Edid.hdmiForumVsdb.deepColor420_30bit);
		DI_UART_Print("Edid.hdmiForumVsdb.deepColor420_36bit = %d\n", Edid.hdmiForumVsdb.deepColor420_36bit);
		DI_UART_Print("Edid.hdmiForumVsdb.deepColor420_48bit = %d\n", Edid.hdmiForumVsdb.deepColor420_48bit);
		DI_UART_Print("\n");

		for(i=0; i<NEXUS_VideoFormat_eMax; i++)
		{
			DI_UART_Print("Edid.videoFormatSupported[%d] = %d\n", i, Edid.videoFormatSupported[i]);
		}
		DI_UART_Print("\n");
	}
	else
	{
		PrintError("[%s] NEXUS_HdmiOutput_GetEdidData error!!!\n", __func__);
	}

	VK_SEM_Release(s_hdmi_instance.ulSemId);
#else
	DI_UART_Print("DRV_HDMI_PrintEdidInfo() is not supported!!!\n", __FUNCTION__);
#endif

	return rc;
}

DRV_Error DRV_HDMI_GetStatus(char* pStr)
{
	HAPPY(pStr);
	return DRV_OK;
}

DRV_Error DRV_HDMI_SetActiveFormat(void)
{
	DRV_Error rc=DRV_OK;

	PrintEnter();

	if (s_hdmi_instance.bInitialized==FALSE || VK_SEM_Get(s_hdmi_instance.ulSemId)!=VK_OK)
		return DRV_ERR;

	// Perhaps, NEXUS wants NEXUS_Display_AddOutput for the function.
	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DRV_Error DRV_HDMI_GetHdcpKey(HUINT8** ppHdcpKey)
{
	DRV_Error rc=DRV_OK;

	PrintEnter();

	if (s_hdmi_instance.bInitialized==FALSE || VK_SEM_Get(s_hdmi_instance.ulSemId)!=VK_OK)
		return DRV_ERR;

	*ppHdcpKey=s_ucaHdcpKey;

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

void DRV_HDMI_GetHdcpStateStr(HUINT32 ulIndex, char* pStr)
{
	char* strHdcpState[NEXUS_HdmiOutputHdcpState_eMax]=
	{
		"NEXUS_HdmiOutputHdcpState_eUnpowered",
		"NEXUS_HdmiOutputHdcpState_eUnauthenticated",
		"NEXUS_HdmiOutputHdcpState_eInitializedAuthentication",
		"NEXUS_HdmiOutputHdcpState_eWaitForReceiverAuthentication",
		"NEXUS_HdmiOutputHdcpState_eReceiverR0Ready",
		"NEXUS_HdmiOutputHdcpState_eR0LinkFailure",
		"NEXUS_HdmiOutputHdcpState_eReceiverAuthenticated",
		"NEXUS_HdmiOutputHdcpState_eWaitForRepeaterReady",
		"NEXUS_HdmiOutputHdcpState_eCheckForRepeaterReady",
		"NEXUS_HdmiOutputHdcpState_eRepeaterReady",
		"NEXUS_HdmiOutputHdcpState_eLinkAuthenticated",
		"NEXUS_HdmiOutputHdcpState_eEncryptionEnabled",
		"NEXUS_HdmiOutputHdcpState_eRepeaterAuthenticationFailure",
		"NEXUS_HdmiOutputHdcpState_eRiLinkIntegrityFailure",
		"NEXUS_HdmiOutputHdcpState_ePjLinkIntegrityFailure"
	};

	/*
	* "pStr" should be an array that is big enough.
	* dcchung 2009.5.12
	*/
	if (ulIndex<NEXUS_HdmiOutputHdcpState_eMax)
		VK_strncpy(pStr, strHdcpState[ulIndex], 60);
	else
		VK_strncpy(pStr, "Undefined VideoFomat", 30);

	return;
}

void DRV_HDMI_PrintAviPacket(void)
{
#ifdef NEXUS_AVIINFO_FUNCTION
	NEXUS_HdmiAviInfoFrame AviInfoFrame;
	NEXUS_Error		nResult = NEXUS_SUCCESS;

	nResult = NEXUS_HdmiOutput_GetAviInfoFrame(s_hdmi_instance.hHdmi, &AviInfoFrame);
	if(nResult != NEXUS_SUCCESS)
	{
		DI_UART_Print("NEXUS_HdmiOutput_GetAviInfoFrame failed!\n");
		return;
	}
	DI_UART_Print("============================== AVI-INFO   =========================\n");
	DI_UART_Print("AVI-INFO  videoIdCode: [%d]\n", AviInfoFrame.videoIdCode );
	DI_UART_Print("AVI-INFO  eColorSpace: [%d]\n", AviInfoFrame.eColorSpace );
	DI_UART_Print("AVI-INFO  eActiveInfo: [%d]\n", AviInfoFrame.eActiveInfo );
	DI_UART_Print("AVI-INFO  eColorimetry: [%d]\n", AviInfoFrame.eColorimetry );
	DI_UART_Print("AVI-INFO  ePictureAspectRatio: [%d]\n", AviInfoFrame.ePictureAspectRatio );
	DI_UART_Print("AVI-INFO  eActiveFormatAspectRatio: [%d]\n", AviInfoFrame.eActiveFormatAspectRatio );
	DI_UART_Print("AVI-INFO  eITContent: [%d]\n", AviInfoFrame.eITContent );
	DI_UART_Print("AVI-INFO  eExtendedColorimetry: [%d]\n", AviInfoFrame.eExtendedColorimetry );
	DI_UART_Print("AVI-INFO  eRGBQuantizationRange: [%d]\n", AviInfoFrame.eRGBQuantizationRange );
	DI_UART_Print("AVI-INFO  eYccQuantizationRange: [%d]\n", AviInfoFrame.eYccQuantizationRange );
	DI_UART_Print("AVI-INFO  eContentType: [%d]\n", AviInfoFrame.eContentType );
	DI_UART_Print("=====================================================================\n");
#endif

	return;
}

DRV_Error DRV_HDMI_PrintInfo(void)
{//Sunwoo
#if defined(CONFIG_DEBUG)
	NEXUS_Error nResult;
	NEXUS_HdmiOutputStatus hdmiStatus;
	NEXUS_HdmiOutputHdcpStatus hdcpStatus;
	char* contents;
	char *pszEnable = "enable";
	char *pszDisable = "disable";
	char *pszConnect = "connect";
	char *pszUnConnect = "unconnect";
#if defined(RX_POWERDOWNMODE)
	 char *pszPowerDown = "power-down";
#endif /* end of RX_POWERDOWNMODE */
	const char *pszMute = "Mute";
	const char *pszUnmute = "Unmute";

	unsigned int i =0;

	PrintEnter();

	DI_UART_Print("============================== MW requested =========================\n");
	DI_UART_Print("\n");
	DI_UART_Print("HDMI Enable : [%s]\n", s_hdmi_request.bHdmiEnable ? pszEnable : pszDisable );
	DI_UART_Print("HDCP Enable : [%s]\n", s_hdmi_request.bHdcpEnable ? pszEnable : pszDisable );
	DI_UART_Print("AV MUTE     : [%s]\n", s_hdmi_request.bHdmiAvMute ? pszMute : pszUnmute );
	DI_UART_Print("AUDIO Mute  : [%s]\n", s_hdmi_request.bHdmiAudioMute ? pszMute : pszUnmute );
	DI_UART_Print("AV BLANK    : [%s]\n", s_hdmi_request.bHdmiAvBlank ? pszEnable : pszDisable );
	DI_UART_Print("HDMI POWER  : [%s]\n", s_hdmi_request.bHdmiPowerState? pszEnable : pszDisable );
	DI_UART_Print("=====================================================================\n");

	DI_UART_Print("============================== DI status    =========================\n");
	DI_UART_Print("ePowerStatus   : [%s]\n", s_hdmi_instance.bDevicePowerState? pszEnable : pszDisable );
#if defined(RX_POWERDOWNMODE)
	if(s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED)
		contents = pszConnect;
	else if(s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_POWER_DOWN)
		contents = pszPowerDown;
	else
		contents = pszUnConnect;
	DI_UART_Print("eHotplugState  : [%s]\n", contents);
#else
	DI_UART_Print("eHotplugState  : [%s]\n", s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED ? pszConnect : pszUnConnect );
#endif /* end of RX_POWERDOWNMODE */
	DI_UART_Print("bEnableOutput    : [%s]\n", s_hdmi_instance.bEnableOutput ? pszEnable : pszDisable );
	DI_UART_Print("eHdcpStatus     : [%s]\n", s_hdmi_instance.eHdcpStatus ? pszEnable : pszDisable );
#if !defined(RX_POWERDOWNMODE) /* remove */
	DI_UART_Print("eRxsenceState : [%s]\n", s_hdmi_instance.eRxsenceState == DI_HDMI_RXSENCE_CONNECTED ? pszConnect : pszUnConnect );
#endif /* end of !RX_POWERDOWNMODE */
	DI_UART_Print("=====================================================================\n");

	DI_UART_Print("============================== NEXUS status =========================\n");

	nResult = NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &hdmiStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed!\n");
	}
	nResult = NEXUS_HdmiOutput_GetHdcpStatus(s_hdmi_instance.hHdmi, &hdcpStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed!\n");
	}

	contents = hdmiStatus.connected?"Connected":"Disconnected";
	DI_UART_Print("HotPlug: [%s]\n", contents);
#if defined(RX_POWERDOWNMODE)
#if (NEXUS_VERSION < 1200)
	contents = hdmiStatus.rxsence?"Connected":"Disconnected";
#else
	contents = hdmiStatus.rxPowered?"Connected":"Disconnected";
#endif
	DI_UART_Print("RxSense: [%s]\n", contents);
#endif /* end of RX_POWERDOWNMODE */
	DI_UART_Print("\n");

#if defined(CONFIG_HDCP_22)
	if(hdcpStatus.hdcp2_2Features==true)
	{
		DI_UART_Print("Hdcp Version : 2.2\n");
	}
	else
	{
		DI_UART_Print("Hdcp Version : 1.x\n");
	}
#endif
	DI_UART_Print("Hdcp Authentication: [%s] \n", hdcpStatus.transmittingEncrypted?"T":"F");
	DI_UART_Print("Is Repeater?: [%s]\n", hdcpStatus.isHdcpRepeater?"T":"F");
	DI_UART_Print("Monitor Name : %s\n", hdmiStatus.monitorName);
	DI_UART_Print("=====================================================================\n");
#if defined(CONFIG_DI20)
	{
	    char *pTempString = NULL;
		pTempString = DD_MEM_Alloc(200);
		if(pTempString == NULL)
		{
			PrintError("Can't Allocate Buffer\n");
		}
		else
		{
			int* (*pTempFunc)(char *pArgString) = (void *)CMD_GetInfoUI;

			VK_snprintf(&pTempString[0], 200, "\n\tConnected : %s\n\tHDCP Authentication : %s\n\tMonitor Name : %s\n", hdmiStatus.connected?"YES":"NO",
					hdcpStatus.transmittingEncrypted?"SUCCESS":"FAIL",
					hdmiStatus.monitorName);
			
			if(pTempFunc != NULL) 
			{
				pTempFunc(pTempString);
			}		

			DD_MEM_Free(pTempString);
		}
	}
#endif
	{
		DI_HDMI_AVAILABLE_FORMATS Available;

		P_HDMI_GetCapability(&Available);

#if defined(RX_POWERDOWNMODE)
		DI_UART_Print("HDMI Capability\n");
#else
		DI_UART_Print("HDMI Capability[%s]\n", contents);
#endif /* end of RX_POWERDOWNMODE */
		DI_UART_Print("ulVideoFormats = %x\n", Available.ulVideoFormats);

#if defined(CONFIG_DI10)
		for(i=0;i<16;i++)
		{
			if(Available.ulVideoFormats & (0x0001<<i))
				DI_UART_Print("%s\n",SupportedResolution[i]);
		}
#endif
		DI_UART_Print("sPhysicalAddr.NodeA = %x\n", Available.sPhysicalAddr.NodeA);
		DI_UART_Print("sPhysicalAddr.NodeB = %x\n", Available.sPhysicalAddr.NodeB);
		DI_UART_Print("sPhysicalAddr.NodeC = %x\n", Available.sPhysicalAddr.NodeC);
		DI_UART_Print("sPhysicalAddr.NodeD = %x\n", Available.sPhysicalAddr.NodeD);
		DI_UART_Print("ulLogicalAddr = %x\n", Available.ulLogicalAddr);
		DI_UART_Print("ulAudioFormats = %x\n", Available.ulAudioFormats);

		if(s_hdmi_instance.eHotplugState == DI_HDMI_HOTPLUG_CONNECTED)
		{
		    DI_UART_Print("ulNumOfVFormat = %d\n", Available.ulNumOfVFormat);
			DI_UART_Print("\tfomat | rate | acpect\n");
			for(i=0;i<Available.ulNumOfVFormat;i++)
			{
				DI_UART_Print("\t%05x | %04x | %04x\n", Available.stVformats[i].ulVFormat, Available.stVformats[i].ulFieldRate, Available.stVformats[i].ulAspectRatio);
			}

			DI_UART_Print("Supported Audio Format (%04x)\n", Available.ulAudioFormats);
			for(i=0;i<16;i++)
			{
				switch(Available.tAudioInfo[i].ulAudioFormat)
				{
					case DI_HDMI_AFORMAT_PCM:
						DI_UART_Print("\t\tNEXUS_AudioCodec_ePcm\n");
						break;
					case DI_HDMI_AFORMAT_AC3:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eAc3\n");
						break;
					case DI_HDMI_AFORMAT_AC3PLUS:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eAc3Plus\n");
						break;
					case DI_HDMI_AFORMAT_MPEG1:
					case DI_HDMI_AFORMAT_MPEG2:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eMpeg\n");
						break;
					case DI_HDMI_AFORMAT_MP3:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eMp3\n");
						break;
					case DI_HDMI_AFORMAT_AAC:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eAac\n");
						break;
					case DI_HDMI_AFORMAT_DTS:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eDts\n");
						break;
					case DI_HDMI_AFORMAT_DTSHD:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eDtsHd\n");
						break;
					case DI_HDMI_AFORMAT_WMAPro:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eWmaPro\n");
						break;
					case DI_HDMI_AFORMAT_OneBit:
					case DI_HDMI_AFORMAT_ATRAC:
					case DI_HDMI_AFORMAT_MATMLP:
					case DI_HDMI_AFORMAT_DST:
						DI_UART_Print("\t\tNEXUS_AudioCodec_eUnknown\n");
						break;
					default:
						break;
				}
				DI_UART_Print("########## %d \n",Available.tAudioInfo[i].unSupportedFs);
			}
		}

		DRV_HDMI_PrintAviPacket();
	}

	PrintExit();
#endif
	return DRV_OK;
}

DRV_Error DRV_HDMI_SetAudioMute(HBOOL bMute)
{
	HINT32 vResult;
	DRV_Error dResult = DRV_ERR;
	HDMI_MSG_T HdmiMsg;
	unsigned long evtResult;

	PrintEnter();
	PrintDebug("DRV_HDMI_SetAudioMute bMute: %s\n", bMute?"T":"F");

	vResult=VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	if (s_hdmi_request.bHdmiAudioMute == bMute)
	{
		PrintError("[DRV_HDMI_SetAudioMute] Audio Mute(%d) was requested already !!\n", bMute);
		dResult = DRV_OK;
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		goto ret;
	}

	s_hdmi_request.bHdmiAudioMute = bMute;
	VK_SEM_Release(s_hdmi_instance.ulSemId);
	HdmiMsg.eHdmiMsg = (bMute == TRUE) ? HDMI_MSG_CMD_AUDIO_MUTE : HDMI_MSG_CMD_AUDIO_UNMUTE;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
	VK_EVENT_ReceiveTimeout(s_ulHdmiSyncCallEventId, &evtResult, 500);
	dResult = (DRV_Error)evtResult;

ret:
	PrintExit();
	return dResult;
}

DRV_Error DRV_HDMI_AudioEnable(HBOOL audioEnable)
{
	PrintEnter();
	BSTD_UNUSED(audioEnable);
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_HDMI_VideoEnable(HBOOL videoEnable)
{
	NEXUS_Error nResult;

	PrintEnter();

	if (videoEnable == TRUE)
	{
		nResult = NEXUS_Display_AddOutput(s_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(s_hdmi_instance.hHdmi));
	}
	else
	{
		nResult = NEXUS_Display_RemoveOutput(s_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(s_hdmi_instance.hHdmi));
	}
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[DRV_HDMI_VideoEnable] nexus failed! (%d)\n", nResult);
	}
	s_hdmi_instance.bEnableOutput = FALSE;
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT e3dFormat)
{
	int vResult;
	DRV_Error dResult = DRV_ERR;
	HDMI_MSG_T HdmiMsg;
	unsigned long evtResult;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_3D_FORMAT;
	HdmiMsg.param1 = e3dFormat;
	vResult = VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
	if (vResult != VK_OK)
	{
		PrintError("VK_MSG_SendTimeout failed!\n");
	}
	vResult = VK_EVENT_ReceiveTimeout(s_ulHdmiSyncCallEventId, &evtResult, 500);
	if (vResult != VK_OK)
	{
		PrintError("VK_EVENT_ReceiveTimeout failed!\n");
	}
	dResult = (DRV_Error)evtResult;

	PrintExit();
	return dResult;
}

DRV_Error DRV_HDMI_GetMonitorName(char *pucVendorId)
{
	NEXUS_Error nResult;
	NEXUS_HdmiOutputStatus hdmiStatus;

	PrintEnter();

	nResult = NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &hdmiStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] nexus failed! (%d)\n",__FUNCTION__, nResult);
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		return DRV_ERR;
	}

	VK_strncpy(pucVendorId, hdmiStatus.monitorName, VK_strlen(hdmiStatus.monitorName));

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_GetHdmiOutputHandle(NEXUS_HdmiOutputHandle *hHdmi)
{
	PrintEnter();

	*hHdmi = s_hdmi_instance.hHdmi;

	PrintExit();

	return DRV_OK;
}


void DRV_HDMI_SetAviInfoAspect(DI_HDMI_AUX_VIDEO_INFO stAVInfo)
{
	HDMI_MSG_T HdmiMsg;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_AUXINFO;
	HdmiMsg.param1 = stAVInfo.eAR;
	HdmiMsg.param2 = stAVInfo.eAFD;
	VK_MSG_Send(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T));
	PrintExit();
	return;
}


#if (defined(CONFIG_TEMP_HMX4KJP) || defined(CONFIG_TEMP_UHD7445)) && (BCHP_VER >= BCHP_VER_D0)
void DRV_HDMI_SetColorSpace(NEXUS_VideoFormat format)
{
	NEXUS_HdmiOutputSettings hdmiSettings;

#if ((NEXUS_VERSION == 1401) && (NEXUS_VERSION_MINOR >= 1)) || defined(CONFIG_WORKAROUND_BRCM_HDMI_20_10BITS)
	if (format == NEXUS_VideoFormat_e3840x2160p60hz || format == NEXUS_VideoFormat_e3840x2160p50hz)
	{
		NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
		NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
	}
	else
	{
		NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
		NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
	}
#elif ((NEXUS_VERSION == 1402) && (NEXUS_VERSION_MINOR >= 1))
	if (format == NEXUS_VideoFormat_e3840x2160p60hz || format == NEXUS_VideoFormat_e3840x2160p50hz)
	{
		NEXUS_HdmiOutputEdidData Edid;
		NEXUS_HdmiOutput_GetEdidData(s_hdmi_instance.hHdmi,&Edid);

		NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);

		/* Default colorSpace & colorDepth */
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
		hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e8bit;

		if(Edid.hdmiForumVsdb.valid)
		{
			if(Edid.hdmiForumVsdb.deepColor420_30bit)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
				hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e10bit;
			}

			if(Edid.hdmiForumVsdb.deepColor420_36bit)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
				hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e12bit;
			}

			if(Edid.hdmiForumVsdb.deepColor420_48bit)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
				hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e16bit;
			}
		}

		NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
	}
	else
	{
		NEXUS_HdmiOutput_GetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
		hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e8bit;
		NEXUS_HdmiOutput_SetSettings(s_hdmi_instance.hHdmi, &hdmiSettings);
	}
#endif
}
#endif

#if __________di____________
#endif

DI_ERR_CODE DI_HDMI_SetPowerState(HBOOL bPowerOn)
{

	DI_ERR_CODE result = DI_ERR_OK;

#ifdef HDMI_POWER_CONTROL

	HINT32 vResult;
	HDMI_MSG_T HdmiMsg;

	PrintEnter();

	PrintDebug("DI_HDMI_SetPowerState %d\n", bPowerOn);

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		goto ret;
	}

	s_hdmi_request.bHdmiPowerState = bPowerOn;


	HdmiMsg.eHdmiMsg = (bPowerOn == TRUE) ? HDMI_MSG_CMD_POWER_ON : HDMI_MSG_CMD_POWER_OFF;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);


ret:
	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();
#else
	HAPPY(bPowerOn);
	PrintDebug("DI_HDMI_SetPowerState Not Supported\n");
	result = DI_ERR_NOT_SUPPORTED;
#endif

	return result;
}

DI_ERR_CODE DI_HDMI_EnableHDCP(HBOOL bEnable)
{
#if defined(CONFIG_HDCP)	
	HINT32 vResult;
	HDMI_MSG_T HdmiMsg;
#endif
	
	HAPPY(bEnable);

	PrintEnter();

#if defined(CONFIG_HDCP)
	PrintDebug("DI_HDMI_EnableHDCP bEnable: %s\n", bEnable?"T":"F");

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		goto ret;
	}

	if (s_hdmi_request.bHdcpEnable == bEnable)
	{
		PrintError("[DI_HDMI_EnableHDCP] HDCP enable(%d) was requested already !!\n", bEnable);
		goto ret;
	}

	s_hdmi_request.bHdcpEnable = bEnable;
	HdmiMsg.eHdmiMsg = (bEnable == TRUE) ? HDMI_MSG_CMD_ENABLE_HDCP : HDMI_MSG_CMD_DISABLE_HDCP;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

ret:
	VK_SEM_Release(s_hdmi_instance.ulSemId);
#endif

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetState(HDMI_RECEIVER_INFO_OPTION ReceiverInfoOption, HDMI_RECEIVER_INFO* pReceiverInfo)
{
	HINT32 vResult;
	NEXUS_Error nexus_rc;

	PrintEnter();

	if (s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (pReceiverInfo == NULL)
	{
		PrintError("pReceiverInfo is NULL\n");

		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s\n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

	if ((ReceiverInfoOption&IS_CONNECTED)==IS_CONNECTED)
	{
		// This code is about "A TV is connected to a HDMI port or not?"
		NEXUS_HdmiOutputStatus HdmiOutputStatus;
		nexus_rc=NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &HdmiOutputStatus);
		if (nexus_rc==NEXUS_SUCCESS)
		{

#if (NEXUS_VERSION < 1200)
#if defined(RX_POWERDOWNMODE)
			if(HdmiOutputStatus.connected && HdmiOutputStatus.rxsence)
			{
				pReceiverInfo->bIsConnected=TRUE;
			}
			else
			{
				pReceiverInfo->bIsConnected=FALSE;
			}
#else
			pReceiverInfo->bIsConnected = (HdmiOutputStatus.connected ? TRUE : FALSE);
#endif /* end of RX_POWERDOWNMODE */
#else
			if(HdmiOutputStatus.connected && HdmiOutputStatus.rxPowered)
			{
				pReceiverInfo->bIsConnected=TRUE;
			}
			else
			{
				pReceiverInfo->bIsConnected=FALSE;
			}
#endif

		}
		else
		{
			PrintError("NEXUS_HdmiOutput_GetStatus error\n");
			pReceiverInfo->bIsConnected=FALSE;
		}
#if defined(DEBUG_HDMI_SEE_PARAMETERS)
		DI_UART_Print(PARAM_COLOR_ON"%s connected %d\n"PARAM_COLOR_OFF, __FUNCTION__, pReceiverInfo->bIsConnected);
#endif
	}

	if ((ReceiverInfoOption&IS_HDMI_DEVICE)==IS_HDMI_DEVICE)
	{
		// This code is about "A TV can run in HDMI mode or not?"
		NEXUS_HdmiOutputStatus HdmiOutputStatus;
		nexus_rc=NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &HdmiOutputStatus);
		if (nexus_rc==NEXUS_SUCCESS)
		{
			pReceiverInfo->bIsHdmiDevice=HdmiOutputStatus.hdmiDevice;
		}
		else
		{
			PrintError("NEXUS_HdmiOutput_GetStatus error\n");
			pReceiverInfo->bIsHdmiDevice=FALSE;
		}
	}

	if ((ReceiverInfoOption&IS_AUTHENCATED)==IS_AUTHENCATED)
	{
		// This code is about "Current link is authenticated or not?"
		NEXUS_HdmiOutputHdcpStatus hdcpStatus;

		nexus_rc=NEXUS_HdmiOutput_GetHdcpStatus(s_hdmi_instance.hHdmi, &hdcpStatus);
		if (nexus_rc==NEXUS_SUCCESS)
		{
			switch (hdcpStatus.hdcpState)
			{
			case NEXUS_HdmiOutputHdcpState_eInitializedAuthentication :
			case NEXUS_HdmiOutputHdcpState_eLinkAuthenticated :
			case NEXUS_HdmiOutputHdcpState_eEncryptionEnabled:
				// Use the value of hdcpStatus.hdcpState that is taken when P_HDMI_hdcp_callback gets a SUCCEESS message.
				// dcchung
				pReceiverInfo->bIsAuthencated=TRUE;
				break;
			default :
				pReceiverInfo->bIsAuthencated=FALSE;
				break;
			}
		}
		else
		{
			PrintError("NEXUS_HdmiOutput_GetHdcpStatus error\n");
			pReceiverInfo->bIsAuthencated=FALSE;
		}
	}

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_RegisterEventNotify(DI_HDMI_EVENT eHdmiEvent, DI_HDMI_CALLBACK pfnHdmiEventCallback)
{
	HINT32 vResult;
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	if (s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (pfnHdmiEventCallback == NULL)
	{
		PrintError("pfnHdmiEventCallback is NULL\n");

		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : event :%d\n"PARAM_COLOR_OFF, __FUNCTION__, eHdmiEvent);
#endif

	if (eHdmiEvent<DH_EVENT_MAX)
		s_hdmi_instance.pfCallback[eHdmiEvent]= pfnHdmiEventCallback;
	else
	{
		PrintDebug("[%s] Unknown event\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

done:
	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable)
{
	HINT32 vResult;
	DI_ERR_CODE	rc=DI_ERR_OK;

	PrintEnter();

	if (s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (pAvailable == NULL)
	{
		PrintError("pAvailable is NULL\n");

		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s\n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

	rc=P_HDMI_GetCapability(pAvailable);

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_EnableOutput(HBOOL bEnable)
{
	HINT32 vResult;
	HDMI_MSG_T HdmiMsg;

	PrintEnter();

	PrintDebug("DI_HDMI_EnableOutput bEnable: %s\n", bEnable?"T":"F");

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		PrintExit();
		return DI_ERR_ERROR;
	}

	if (s_hdmi_request.bHdmiEnable == bEnable)
	{
		PrintError("[DI_HDMI_EnableOutput] HDMI Output enable(%d) was requested already !!\n", bEnable);
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		PrintExit();
		return DI_ERR_OK;
	}
	s_hdmi_request.bHdmiEnable = bEnable;
	if (s_hdmi_request.bHdcpEnable == TRUE)
	{
		//s_hdmi_request.bHdcpEnable = FALSE;
		HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDCP;
		VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);
	}
	HdmiMsg.eHdmiMsg = (bEnable == TRUE) ? HDMI_MSG_CMD_ENABLE_HDMI_OUTPUT : HDMI_MSG_CMD_DISABLE_HDMI_OUTPUT;
	VK_MSG_SendTimeout(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HDMI_MSG_T), 0);

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SetAVMute(HBOOL bEnable)
{
	DRV_Error	eDrvError=DRV_OK;

	PrintEnter();

	eDrvError = DRV_HDMI_SetAVMute(bEnable);
	if (eDrvError != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SetAVBlank(HBOOL bBlank)
{
	DRV_Error	eDrvError=DRV_OK;

	PrintEnter();

	//return DI_ERR_OK; // not to allow controlling the av blank on/off by application

	eDrvError = DRV_HDMI_SetAvBlank(bBlank);
	if (eDrvError != DRV_OK)
	{
		PrintError("[DI_HDMI_SetAVBlank] error in DRV_HDMI_SetAvBlankv (blank-%d), error(%d)\n", bBlank, eDrvError);
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SendCecMsg(DI_HDMI_CEC_MSG_t* pstCecMsg)
{
#if defined(CONFIG_HDMI_CEC)
	DI_ERR_CODE	eDiError=DI_ERR_OK;
	NEXUS_Error	eNexusError=NEXUS_SUCCESS;
	NEXUS_CecMessage transmitMessage;
	unsigned long		evtResult;
	int					vkResult;

	PrintEnter();

	if (s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (pstCecMsg == NULL)
	{
		PrintError("pstCecMsg is NULL\n");

		return DI_ERR_ERROR;
	}

	transmitMessage.initiatorAddr=(HUINT8)(pstCecMsg->ucInitAddr);
	transmitMessage.destinationAddr=(HUINT8)(pstCecMsg->ucDestAddr);
	transmitMessage.length = (HUINT8)(pstCecMsg->ucMegLength);
	VK_memcpy(transmitMessage.buffer, pstCecMsg->ucMessage, pstCecMsg->ucMegLength);

	eNexusError=NEXUS_Cec_TransmitMessage(s_hdmi_instance.hCec, &transmitMessage);
	if (eNexusError!=NEXUS_SUCCESS)
	{
		VK_printf("[ERROR] Send Cec Message, eNexusError: 0x%x !!\n", eNexusError);
		eDiError=DI_ERR_SEND;
		goto done;
	}

	vkResult = VK_EVENT_ReceiveTimeout(s_ulHdmiSyncCallEventId, &evtResult, 500);
	if (vkResult!=VK_OK)
	{
		PrintError("[VK_EVENT_ReceiveTimeout] Send Cec Message, vkResult: 0x%x !!\n", vkResult);
		eDiError=DI_ERR_TIMEOUT;
		goto done;
	}

done:
	PrintExit();
	return eDiError;

#else
	HAPPY(pstCecMsg);
	return DI_ERR_OK;
#endif
}

DI_ERR_CODE DI_HDMI_GetLogicalAddress(HUINT8* ucLogicalAddr)
{
	HINT32 vResult;
	DI_ERR_CODE rc=DI_ERR_OK;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_Error	eNexusError=NEXUS_SUCCESS;
	NEXUS_CecStatus 	tCecStatus;
#endif

	PrintEnter();

	if (s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (ucLogicalAddr == NULL)
	{
		PrintError("ucLogicalAddr is NULL\n");

		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

	*ucLogicalAddr = 0xF;

#if defined(CONFIG_HDMI_CEC)
	eNexusError=NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &tCecStatus);
	if (eNexusError!=NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_HdmiOutput_GetCecConfiguration Error: 0x%x !!\n", __func__, eNexusError);
		rc=DI_ERR_ERROR;
		goto done;
	}
	else
	{
		*ucLogicalAddr=tCecStatus.logicalAddress;
	}
done:
#endif

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_GetEdidInfo(DI_HDMI_EDID_INFO* pEdidInfo)
{
	HINT32 vResult;
	DI_ERR_CODE rc=DI_ERR_OK;
	DRV_Error drv_rc=DRV_OK;
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputBasicEdidData BasicEdidData;
	NEXUS_HdmiOutputStatus	stHdmiOutputStatus;

	PrintEnter();

	if (s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (pEdidInfo == NULL)
	{
		PrintError("pEdidInfo is NULL\n");

		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s\n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

	if (NEXUS_HdmiOutput_GetBasicEdidData(s_hdmi_instance.hHdmi, &BasicEdidData)!=NEXUS_SUCCESS)
	{
		PrintDebug("[%s] Unable to get BasicEdidData\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	if (pEdidInfo!=NULL)
	{
		HUINT8 ucFirstVideoId=0;

		VK_memcpy(pEdidInfo->vendorID, BasicEdidData.vendorID, 2);
		VK_memcpy(pEdidInfo->productID, BasicEdidData.productID, 2);
		VK_memcpy(pEdidInfo->serialNum, BasicEdidData.serialNum, 4);
		nexus_rc=NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &stHdmiOutputStatus);
		if (nexus_rc!=NEXUS_SUCCESS)
		{
			PrintDebug("[%s] NEXUS_HdmiOutput_GetStatus error\n", __func__);
			rc=DI_ERR_ERROR;
			goto done;
		}
		VK_memcpy(pEdidInfo->monitorName, stHdmiOutputStatus.monitorName, 14);
		pEdidInfo->manufWeek=BasicEdidData.manufWeek;
		pEdidInfo->manufYear=BasicEdidData.manufYear;
		pEdidInfo->edidVersion=BasicEdidData.edidVersion;
		pEdidInfo->edidRevision=BasicEdidData.edidRevision;
		pEdidInfo->maxHorizSize = BasicEdidData.maxHorizSize;
		pEdidInfo->maxVertSize = BasicEdidData.maxVertSize;
		pEdidInfo->extensions = BasicEdidData.extensions;

		drv_rc=P_HDMI_GetFirstVideoId(&ucFirstVideoId);
		if (drv_rc==DRV_OK)
		{
			switch (ucFirstVideoId)
			{
			case 6:
			case 7:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_480I;
				break;
			case 1:
			case 2:
			case 3:
			case 14:
			case 15:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_480P;
				break;
			case 21:
			case 22:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_576I;
				break;
			case 17:
			case 18:
			case 29:
			case 30:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_576P;
				break;
			case 4:
			case 19:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_720P_50HZ;
				break;
			case 5:
			case 20:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_1080I_50HZ;
				break;
			case 31:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_1080P_50HZ;
				break;
			case 16:
				pEdidInfo->firstVideoId=DI_HDMI_VFORMAT_1080P;
				break;
			case 32:
			case 33:
			case 34:
			default :
				pEdidInfo->firstVideoId=0;
				break;
			}
		}
		else
		{
			rc=DI_ERR_ERROR;
		}
	}

done:
	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_SetAuxVideoInfo(DI_HDMI_AUX_VIDEO_INFO stAVInfo)
{
	HINT32 vResult;
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

	DRV_HDMI_SetAviInfoAspect(stAVInfo);

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_SetColorSpace_Info(DI_HDMI_AUX_VIDEO_INFO stAVInfo)
{
	HINT32 vResult;
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

	P_HDMI_SetColorSpaceInfo(stAVInfo);

	VK_SEM_Release(s_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT e3dFormat)
{
	DRV_Error eDrvError = DRV_OK;

	PrintEnter();

	eDrvError = DRV_HDMI_Set3DFormat(e3dFormat);
	if (eDrvError != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetCECInfo(DI_HDMI_CEC_STATE* pState)
{
	HINT32 vResult;
	DI_ERR_CODE rc = DI_ERR_OK;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_Error nexus_rc;
	NEXUS_CecStatus	pStatus;
#endif

	PrintEnter();

	if(s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");
		return DI_ERR_ERROR;
	}

	if(pState == NULL)
	{
		PrintError("pState is NULL\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}
#if defined(CONFIG_HDMI_CEC)
	nexus_rc = NEXUS_Cec_GetStatus(s_hdmi_instance.hCec, &pStatus);
	if(nexus_rc!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Cec_GetStatus failed\n");
		rc=DI_ERR_ERROR;
		goto done;
	}

	if(pStatus.logicalAddress == 0xFF)
	{
		*pState=DI_HDMI_CEC_NO_DEVICE;
	}
	else
	{
		*pState=DI_HDMI_CEC_DEVICE;
	}

	done:
#endif
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		PrintExit();

		return rc;

}

DI_ERR_CODE DI_HDMI_GetAutoLipSyncInfo(DI_HDMI_AUTOLIPSYNC_INFO* pstInfo)
{
	HINT32 vResult;
	DI_ERR_CODE rc = DI_ERR_OK;
	NEXUS_Error nexus_rc;
	NEXUS_HdmiOutputStatus	stHdmiOutputStatus;

	PrintEnter();

	if(s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");
		return DI_ERR_ERROR;
	}

	if(pstInfo == NULL)
	{
		PrintError("pstInfo is NULL\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}
	nexus_rc = NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &stHdmiOutputStatus);
	if(nexus_rc!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		rc=DI_ERR_ERROR;
		goto done;
	}
	pstInfo->ucVideo = stHdmiOutputStatus.autoLipsyncInfo.videoLatency;
	pstInfo->ucAudio = stHdmiOutputStatus.autoLipsyncInfo.audioLatency;
	pstInfo->ucInterlacedVideo = stHdmiOutputStatus.autoLipsyncInfo.interlacedVideoLatency;
	pstInfo->ucInterlacedAudio = stHdmiOutputStatus.autoLipsyncInfo.interlacedAudioLatency;

	if((pstInfo->ucVideo!=0)||(pstInfo->ucAudio!=0)) /* non-interlaced */
	{
		pstInfo->eState=DI_HDMI_AUTOLIPSYNC_ON;
	}
	else if((pstInfo->ucInterlacedVideo!=0)||(pstInfo->ucInterlacedAudio!=0)) /* interlaced */
	{
		pstInfo->eState=DI_HDMI_AUTOLIPSYNC_ON;
	}
	else
	{
		pstInfo->eState=DI_HDMI_AUTOLIPSYNC_OFF;
	}

	done:
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		PrintExit();

		return rc;

}

DI_ERR_CODE DI_HDMI_GetRawEdid(DI_HDMI_EDID_RAW* pEdid)
{
	HINT32 vResult;
	NEXUS_HdmiOutputEdidBlock stEdidBlock;
	HUINT32 NumExtensions, i;
	DI_ERR_CODE rc = DI_ERR_OK;

	PrintEnter();

	if(s_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");
		return DI_ERR_ERROR;
	}

	if(pEdid == NULL)
	{
		PrintError("pEdid is NULL\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(s_hdmi_instance.ulSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	/* 128Bytes Basic block */
	if(NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, 0, &stEdidBlock)==NEXUS_SUCCESS)
	{
		VK_MEM_Memcpy(pEdid->data, stEdidBlock.data, DRV_HDMI_EDID_BLOCKSIZE);
		NumExtensions=stEdidBlock.data[DRV_HDMI_EDID_EXTENSION];
		for(i=1 ; i<=NumExtensions ; i++)
		{
			/* next 128Bytes Extension block */
			if(NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, i, &stEdidBlock)==NEXUS_SUCCESS)
			{
				VK_MEM_Memcpy(pEdid->data+128, stEdidBlock.data, DRV_HDMI_EDID_BLOCKSIZE);
 			}
		}
	}
	else
	{
		rc = DI_ERR_ERROR;
		PrintDebug("[%s] Unable to read E-EDID block\n", __func__);
		goto done;
	}

	done:
		VK_SEM_Release(s_hdmi_instance.ulSemId);
		PrintExit();

		return rc;
}

DI_ERR_CODE DI_HDMI_GetPortOutNum(HUINT8* ucNum)
{
	*ucNum = DI_HDMI_NUM_OUTPUTS;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetHdcpVersion(DI_HDMI_HDCP_VERSION *eHdcpVersion)
{
	DI_ERR_CODE rc = DI_ERR_OK;
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputHdcpStatus hdcpStatus;

	PrintEnter();

	if(eHdcpVersion == NULL)
	{
		PrintError("[%s] eHdcpVersion is NULL\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

#if (NEXUS_VERSION >= 1403)

	nexus_rc = NEXUS_HdmiOutput_GetHdcpStatus(s_hdmi_instance.hHdmi, &hdcpStatus);
	if(nexus_rc == NEXUS_SUCCESS)
	{
		if(hdcpStatus.hdcp2_2Features==true)
		{
			*eHdcpVersion = DI_HDMI_HDCP_VERSION_2_2;
			PrintDebug("Hdcp Version = %s\n", "2.X");
		}
		else
		{
			*eHdcpVersion = DI_HDMI_HDCP_VERSION_1_4;
			PrintDebug("Hdcp Version = %s\n", "1.X");
		}
	}

#else

	nexus_rc = NEXUS_HdmiOutput_GetHdcpStatus(s_hdmi_instance.hHdmi, &hdcpStatus);
	if(nexus_rc == NEXUS_SUCCESS)
	{
		*eHdcpVersion = DI_HDMI_HDCP_VERSION_1_4;
		PrintDebug("Hdcp Version = %s\n", "1.X");
	}

#endif

done:
	PrintExit();

	return rc;
}

#if defined(CONFIG_HDMI_IN)
static HBOOL s_bInitInputToOutput = FALSE;
static NEXUS_HdmiInputHandle s_hdmiInput = NULL;

BDBG_MODULE(hdmi_input_to_hdmi_output);
static uint8_t SampleEDID[] =
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x74, 0x22, 0x05, 0x01, 0x11, 0x20,
	0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
	0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
	0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x37, 0x34, 0x32, 0x32, 0x2F, 0x37, 0x34, 0x32, 0x35, 0x0A, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x89,

	0x02, 0x03, 0x3C, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07,
	0x06, 0x01, 0x29, 0x09, 0x07, 0x01, 0x11, 0x07, 0x00, 0x15, 0x07, 0x00, 0x01, 0x1D, 0x00, 0x72,
	0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A,
	0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18,
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D
};

void source_changed(void *context, int param)
{
	HAPPY(context);
	HAPPY(param);
}

void avmute_changed(void *context, int param)
{
	NEXUS_HdmiInputHandle hdmiInput;
	NEXUS_HdmiInputStatus hdmiInputStatus;
	HAPPY(param);

	hdmiInput = (NEXUS_HdmiInputHandle) context;
	NEXUS_HdmiInput_GetStatus(hdmiInput, &hdmiInputStatus);

	if(!hdmiInputStatus.validHdmiStatus)
	{
		PrintError("avmute_changed callback : Unable to get hdmiInput status\n");
	}
	else
	{
		PrintError("avmute_changed callback : %s\n", hdmiInputStatus.avMute ? "Set_AvMute" : "clear_AvMute");
	}
}

HINT32 DI_HDMI_InitInputToOutput(void)
{
	NEXUS_TimebaseSettings timebaseSettings;
	NEXUS_HdmiInputSettings hdmiInputSettings;
	NEXUS_HdmiOutputStatus hdmiOutputStatus;
	NEXUS_Error rc;
	unsigned hdmiInputIndex = 0;
	NEXUS_HdmiOutputBasicEdidData hdmiOutputBasicEdidData;
	uint8_t *attachedRxEdid;
	NEXUS_HdmiOutputEdidBlock edidBlock;
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
	DRV_Error nDrvErr = DRV_OK;
#endif

	PrintEnter();

	NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
		timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
	NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

	NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
		hdmiInputSettings.timebase = NEXUS_Timebase_e0;
		hdmiInputSettings.sourceChanged.callback = source_changed;
		hdmiInputSettings.frontend.hpdDisconnected = false;
		hdmiInputSettings.useInternalEdid = true;

	rc = NEXUS_HdmiOutput_GetStatus(s_hdmi_instance.hHdmi, &hdmiOutputStatus);
	if(rc)
		BERR_TRACE(rc);

	if(hdmiOutputStatus.connected)
	{
		unsigned i, j;
		NEXUS_HdmiOutput_GetBasicEdidData(s_hdmi_instance.hHdmi, &hdmiOutputBasicEdidData);
		attachedRxEdid = BKNI_Malloc((hdmiOutputBasicEdidData.extensions + 1) * sizeof(edidBlock.data));
		for(i = 0; i <= hdmiOutputBasicEdidData.extensions; i++)
		{
			rc = NEXUS_HdmiOutput_GetEdidBlock(s_hdmi_instance.hHdmi, i, &edidBlock);
			if(rc)
				BDBG_ERR(("Error retrieve EDID from attached receiver"));

			for(j = 0; j < sizeof(edidBlock.data); j++)
			{
				attachedRxEdid[i * sizeof(edidBlock.data) + j] = edidBlock.data[j];
			}
		}

		s_hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, &hdmiInputSettings, attachedRxEdid, (uint16_t) (sizeof(NEXUS_HdmiOutputEdidBlock) * (hdmiOutputBasicEdidData.extensions + 1)));

		BKNI_Free(attachedRxEdid);
	}
	else
	{
		s_hdmiInput = NEXUS_HdmiInput_OpenWithEdid(hdmiInputIndex, &hdmiInputSettings, &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
	}

	if(!s_hdmiInput)
	{
		PrintError("Can't get hdmi input\n");

		PrintExit();

		return -1;
	}

	NEXUS_HdmiInput_GetSettings(s_hdmiInput, &hdmiInputSettings);
	hdmiInputSettings.avMuteChanged.callback = avmute_changed;
	hdmiInputSettings.avMuteChanged.context = s_hdmiInput;
	NEXUS_HdmiInput_SetSettings(s_hdmiInput, &hdmiInputSettings);

#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
	nDrvErr = DRV_AUDIO_InitAudioInputCapture(0);
	if(nDrvErr != DRV_OK)
	{
		PrintError("error(%d) in DRV_AUDIO_InitAudioInputCapture() \n", nDrvErr);
		return DI_ERR_ERROR;
	}
#endif

	PrintExit();

	return 0;
}

void DI_HDMI_UnInitInputToOutput(void)
{
	PrintDebug("DI_HDMI_UnInitInputToOutput\n");
}

DRV_Error DRV_VIDEO_SetHdmiVideoInputToOutput(HUINT32 ulDeviceId, NEXUS_HdmiInputHandle hdmiInput);
DRV_Error DRV_VIDEO_UnsetHdmiVideoInputToOutput(HUINT32 ulDeviceId, NEXUS_HdmiInputHandle hdmiInput);
DRV_Error DRV_AUDIO_SetHdmiAudioInputToOutput(HUINT32 ulDevId, NEXUS_HdmiInputHandle hdmiInput);
DRV_Error DRV_AUDIO_UnsetHdmiAudioInputToOutput(HUINT32 ulDevId);

void DI_HDMI_SetInputToOutput(void)
{
	PrintEnter();

	if(!s_bInitInputToOutput)
	{
		if(DI_HDMI_InitInputToOutput() < 0)
		{
			PrintError("DI_HDMI_InitInputToOutput returns error\n");
			PrintExit();
			return;
		}

		s_bInitInputToOutput = true;
	}

	DRV_VIDEO_SetHdmiVideoInputToOutput(0, s_hdmiInput);
	DRV_AUDIO_SetHdmiAudioInputToOutput(0, s_hdmiInput);

	PrintExit();

	return;
}

void DI_HDMI_UnsetInputToOutput(void)
{
	PrintEnter();

	DRV_VIDEO_UnsetHdmiVideoInputToOutput(0, s_hdmiInput);
	DRV_AUDIO_UnsetHdmiAudioInputToOutput(0);

	PrintExit();

	return;
}
#endif

/*Please refer to file(BSEAV/lib/security/sage/manufacturing/provision) */
DI_ERR_CODE DI_HDMI_HDCP22KeyProvisioning(HUINT8 *pucInput, HUINT32 nKeySize, HUINT8 *pucOutput)
{
#if defined(CONFIG_HDCP_22_PROVISIONING)
    int rc = 0;

	if(pucInput == NULL || pucOutput == NULL || !nKeySize)
	{
		PrintError("[%s] Error : Invalid parameter - pucInput(0x%x), pucOutput(0x%x)\r\n", __func__, pucInput, pucOutput);
		return DI_ERR_INVALID_PARAM;
	}

    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***\tBroadcom Corporation DRM bin file provisioning utility (Copyright 2014)\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");

	s_tHDCP22Provision.pucInput = pucInput;
	s_tHDCP22Provision.pucOutput = pucOutput;
	s_tHDCP22Provision.nKeySize = nKeySize;

    /* Initialize manufacturing platform and provsioning module */
    rc = SAGE_Provisioning_Init();
    if (rc != 0) {
        goto handle_error;
    }

    rc = SAGE_Provisioning_ProcessBinFile();
    if (rc != 0) {
        goto handle_error;
    }

handle_error:
	PrintError("[%s] Shutting down validation module and platform\r\n", __func__);
    SAGE_Provisioning_Uninit();

    if (rc) {
		PrintError("[%s] Failure in SAGE Provisioning tool\r\n", __func__);
		return DI_ERR_ERROR;
    }
#else
	HAPPY(pucInput);
	HAPPY(nKeySize);
	HAPPY(pucOutput);
#endif
    return DI_ERR_OK;
}

#if defined(CONFIG_HDCP_22_PROVISIONING)
int SAGE_Provisioning_Init(void)
{
    BERR_Code sage_rc;
    BSAGElib_State state;
    int rc = 0;

    BSAGElib_InOutContainer *container = SRAI_Container_Allocate();
    if (container == NULL) {
        rc = 1;
        goto handle_error;
    }

    /* Open the Manufacturing platform */
    sage_rc = SRAI_Platform_Open(BSAGE_PLATFORM_ID_MANUFACTURING,
                                 &state,
                                 &s_tHDCP22Provision.platformHandle);
    if (sage_rc != BERR_SUCCESS) {
        rc = 1;
        goto handle_error;
    }

    /* Check init state */
    if (state != BSAGElib_State_eInit)
    {
        /* Not yet initialized: send init command */
        sage_rc = SRAI_Platform_Init(s_tHDCP22Provision.platformHandle, NULL);
        if (sage_rc != BERR_SUCCESS)
        {
            rc = 1;
            goto handle_error;
        }
    }

    /* Initialize Provisioning module */
    sage_rc = SRAI_Module_Init(s_tHDCP22Provision.platformHandle,
                               PROVISIONING_MODULE,
                               container, &s_tHDCP22Provision.moduleHandle);
    if ((sage_rc != BERR_SUCCESS) || (container->basicOut[0] != 0))
    {
        rc = 1;
        goto handle_error;
    }

handle_error:
    if (rc)
    {
        /* error: cleanup */
        SAGE_Provisioning_Uninit();
    }

    if(container != NULL)
    {
		PrintError("\t*** Freeing SRAI container\r\n");
        SRAI_Container_Free(container);
        container = NULL;
    }

    return rc;
}

void SAGE_Provisioning_Uninit(void)
{
    if (s_tHDCP22Provision.moduleHandle)
    {
		PrintError("\t*** Uninitializing module '%p'\r\n", s_tHDCP22Provision.moduleHandle);
        SRAI_Module_Uninit(s_tHDCP22Provision.moduleHandle);
        s_tHDCP22Provision.moduleHandle = NULL;
    }

    if (s_tHDCP22Provision.platformHandle)
    {
		PrintError("\t*** Uninitializing platform '%p'\r\n", s_tHDCP22Provision.platformHandle);
        SRAI_Platform_Close(s_tHDCP22Provision.platformHandle);
        s_tHDCP22Provision.platformHandle = NULL;
    }

    return;
}

int SAGE_Provisioning_ProcessBinFile(void)
{
    BERR_Code sage_rc;
    int rc = 0;

    BSAGElib_InOutContainer *container = NULL;

	container = SRAI_Container_Allocate();
    if (container == NULL)
    {
        rc = 1;
        goto handle_error;
    }

    /* Allocate memory in global region */
    container->blocks[0].data.ptr = SRAI_Memory_Allocate(s_tHDCP22Provision.nKeySize, SRAI_MemoryType_Shared);
    if(container->blocks[0].data.ptr == NULL)
    {
		PrintError("\t### Cannot allocate '%d' bytes of memory ^^^^^^^^^^^^^^^^^^^\r\n", s_tHDCP22Provision.nKeySize);
        rc = 1;
        goto handle_error;
    }

    container->blocks[0].len = s_tHDCP22Provision.nKeySize;

	VK_MEM_Memcpy(container->blocks[0].data.ptr, s_tHDCP22Provision.pucInput, s_tHDCP22Provision.nKeySize);

    /* todo : pass buff size to this function */
    rc = _ParseAndDisplay(container->blocks[0].data.ptr, container->blocks[0].len);
    if(rc != 0)
    {
        goto handle_error;
    }

    sage_rc = SRAI_Module_ProcessCommand(s_tHDCP22Provision.moduleHandle, PROVISIONING_COMMAND_ProcessBinFile, container);
    if ((sage_rc != BERR_SUCCESS) || (container->basicOut[0] != 0))
    {
		PrintError("\t### Error processing bin file ^^^^^^^^^^^^^^^^^^\r\n");
        rc = 1;
        goto handle_error;
    }

	PrintError("\t*** Provisioning command successfully returned from SAGE, analyzing result....\r\n");

handle_error:

    if(container != NULL)
    {
        /* overwrite bin file */
        if (container->basicOut[1] == BINDING_OPERATION_SUCCESSFULLY_OCCURRED)
        {
			PrintError("\t*** SAGE side binding operation successfully completed\r\n");
			VK_MEM_Memcpy(s_tHDCP22Provision.pucOutput, container->blocks[0].data.ptr, s_tHDCP22Provision.nKeySize);
        }
        else
        {
            /* Display error code received from SAGE side */
			PrintError("\tThe following SAGE error occurred during the DRM binfile binding process (0x%08x):\r\n", container->basicOut[1]);
			PrintError("\t%s\r\n", BSAGElib_Tools_ReturnCodeToString(container->basicOut[1]));
			PrintError("\t%s\r\n", BSAGElib_Tools_ReturnCodeToString(container->basicOut[1]));
			PrintError("\t%s\r\n", BSAGElib_Tools_ReturnCodeToString(container->basicOut[1]));
        	rc = 1;
        }

		PrintError("\t*** Freeing shared memory\n");
        if(container->blocks[0].data.ptr != NULL){
            SRAI_Memory_Free(container->blocks[0].data.ptr);
            container->blocks[0].data.ptr = NULL;
        }

		PrintError("\t*** Freeing SRAI container\n");
        SRAI_Container_Free(container);
        container = NULL;
    }

    return rc;
}

static int _ParseAndDisplay(HUINT8 *pBinFile, HUINT32 binFileLength)
{
    HUINT32 number_of_drms = 0;
    HUINT32 current_drm_type = 0;
    HUINT32 i;
    const char * pString = NULL;

    if(binFileLength < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER))
    {
		PrintError("\tbinFileLength = %u is less than the size of a valid bin file(%u)\r\n", binFileLength, (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER));
        return -1;
    }

    /* jump to the end of the header and get the number of DRMs */
    number_of_drms = GET_UINT32_FROM_BUF(&pBinFile[SIZEOF_DRM_BINFILE_HEADER]);

    /* sanity check */
    if(number_of_drms > MAX_NUMBER_BINFILE_DRM_TYPES)
    {
		PrintError("\tNumber of DRMs detected in header (%u) exceeds number of supported types (%u)\r\n", number_of_drms, MAX_NUMBER_BINFILE_DRM_TYPES);
        return -1;
    }

	PrintError("\tProvisioning the current DRM bin file for the following '%u' DRMs detected:\r\n", number_of_drms);
    for(i = 0; i < number_of_drms; i++)
    {
        /* todo : check out of bound access */
        current_drm_type = GET_UINT32_FROM_BUF(&pBinFile[SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER + 16*i]);
        pString = _MapDrmEnumToString(current_drm_type);
        if(pString == NULL)
        {
			PrintError("\tInvalid DRM type detected (0x%08x), exiting application\r\n", current_drm_type);
            return -1;
        }

		PrintError("\t\t>>>> %s (0x%08x)\r\n", pString, current_drm_type);
    }

    return 0;
}

static const char * _MapDrmEnumToString(uint32_t drm_type)
{
    switch(drm_type)
    {
    case BinFileDrmType_eNetflix:
        return "NETFLIX";
    case BinFileDrmType_eWidevine:
           return "WIDEVINE";
    case BinFileDrmType_eDtcpIp:
           return "DTCP-IP";
    case BinFileDrmType_ePlayready:
           return "PLAYREADY";
    case BinFileDrmType_eSecureSwRsa:
           return "SECURE_SW_RSA";
    case BinFileDrmType_eCustomPrivate:
           return "CUSTOM_PRIVATE";
    case BinFileDrmType_eAdobeAxcess:
           return "ADOBE";
    case BinFileDrmType_eHdcp22Rx:
           return "HDCP 2.2 RX";
    case BinFileDrmType_eHdcp22Tx:
           return "HDCP 2.2 TX";
    case BinFileDrmType_eMax:
           return NULL;
    default:
        return NULL;
    }
    return NULL;
}

#endif
