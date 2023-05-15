/**
* di_hdmi.c
*/

/**
* @defgroup		DI_HDMI DI_HDMI : DI_HDMI module
* @author
* @modified by
* @note			DI_HDMI APIs
* @brief			Define DI_HDMI APIs
* @file 			di_hdmi_source.c
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

#if defined(CONFIG_TRAUMA_ERIN)
#include <hlib.h>
#include <trapi.h>
#include <erin.h>
#endif

#include "bstd.h"
#include "linden_trace.h"

#if defined(CONFIG_HDCP_22_PROVISIONING)
#include "bsagelib_types.h"
#include "bsagelib_tools.h"

#include "sage_srai.h"
#if (NEXUS_VERSION >= 1501)
#include "sage_manufacturing_api.h"
#else
#include "sage_provisioning_host.h"
#endif
#include "sage_manufacturing_module_ids.h"
#if defined(CONFIG_BCM_SAGE_AUTO_FW) && defined(CONFIG_DEBUG)
#include "hdcp22_unbounded_key.c"
#endif
#include "drm_metadata_tl.h"
#endif
#include "nexus_platform.h"
#include "nexus_hdmi_output_hdcp.h"
#include "nexus_display_types.h"
#include "nexus_display.h"
#include "nexus_audio_types.h"
#include "nexus_video_output.h"

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
#include "di_config.h"
#if defined(CONFIG_DI20)
#include "cmdtool.h"
#include "nvram_map.h"
#endif

#include "di_hdmi_priv.h"

//#define HDCP_CERTI_MODE

#if (NEXUS_VERSION >= 1501)
#define SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED (OPERATION_SUCCESSFULLY_OCCURRED)
#else
#define SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED (BINDING_OPERATION_SUCCESSFULLY_OCCURRED)
#endif

#define HDMI_MSG(NAME) [NAME]=#NAME

#if defined(CONFIG_HDCP_22_PROVISIONING)
typedef struct P_HDMI_HDCP22_PROVISION
{
	HUINT8 *pucInput;
	HUINT8 *pucOutput;
	HUINT32 nKeySize;
    SRAI_PlatformHandle platformHandle;
    SRAI_ModuleHandle moduleHandle;
	SRAI_ModuleHandle validationHandle;
}P_HDMI_HDCP22_PROVISION_t;
#endif

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
HdmiInstance_t g_hdmi_instance;
HdmiRequest_t g_hdmi_request;

HULONG ulHdmiMsgQueueId;
HULONG g_ulHdmiSyncCallEventId;

static HUINT8 s_ucaHdcpKey[DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE];

static HULONG s_ulHdmiTaskId;
#if defined(CONFIG_DEBUG)
static HULONG s_ulHdmiAgingTaskId;
#endif
static HULONG s_ulHdmiRequestSemId = 0;

static HULONG	s_ulTimerHandle;
#if defined(CONFIG_HPD_DELAY)
static HULONG	s_ulTimerHotPlugHandle;
#endif
static HULONG	s_ulCheckHdmiPwrTimer =0;

#if defined(CONFIG_HDCP)
static HULONG	s_hdcpRetryCount = 0;
static HBOOL	s_hdcpKeyValid;
#endif

static HBOOL s_bMuteAging = FALSE;
static HBOOL s_bStandbyAging = FALSE;
static HBOOL s_bDisplayAging = FALSE;

#if defined(CONFIG_HDCP_22)
static HUINT8* s_pucHdcp22Key;
static HUINT32 s_nHdcp22KeySize;
#endif
#if defined(CONFIG_HDCP_22_PROVISIONING)
static P_HDMI_HDCP22_PROVISION_t s_tHDCP22Provision;
#endif

#if defined(WORKAROUND_HDMI_VENDOR_ID)
static HUINT8 gVendorID[2];
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
extern DRV_HDMI_Preemp_Setting_t CustomPreempTable[];
#endif

static const char* strHdmiMsg[HDMI_MSG_MAX]={
	HDMI_MSG(HDMI_MSG_EVT_HOTPLUG),
	HDMI_MSG(HDMI_MSG_EVT_HDCP),
	HDMI_MSG(HDMI_MSG_CMD_ENABLE_HDCP) ,
	HDMI_MSG(HDMI_MSG_CMD_DISABLE_HDCP),
	HDMI_MSG(HDMI_MSG_CMD_SEND_CEC_MSG),
	HDMI_MSG(HDMI_MSG_CMD_SET_RESOLUTION),
	HDMI_MSG(HDMI_MSG_CMD_SET_AUX_VIDEO_INFO),
	HDMI_MSG(HDMI_MSG_CMD_SET_COLORSPACE_INFO),
	HDMI_MSG(HDMI_MSG_CMD_SET_COLORSPACE),
	HDMI_MSG(HDMI_MSG_CMD_SET_3D_FORMAT),
	HDMI_MSG(HDMI_MSG_CMD_SET_VENDOR_SPECIFIC_INFO),
	HDMI_MSG(HDMI_MSG_CMD_ADD_HDMI_OUTPUT),
	HDMI_MSG(HDMI_MSG_CMD_REMOVE_HDMI_OUTPUT),
	HDMI_MSG(HDMI_MSG_CMD_CHECK_HDMI_POWER),
	HDMI_MSG(HDMI_MSG_CMD_SET_PREEMPHASIS_CONFIGURATION),
	HDMI_MSG(HDMI_MSG_CMD_SET_SPECIFIC_INFO),
	HDMI_MSG(HDMI_MSG_CMD_SET_AV_MUTE),
	HDMI_MSG(HDMI_MSG_CMD_SET_TMDS_SIGNAL),
	HDMI_MSG(HDMI_MSG_SLF_HDCP_CHECK_POLLING)
};

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_HDCP_22_PROVISIONING)
int SAGE_Provisioning_Init(void);
extern void SAGE_Provisioning_Uninit(void);
extern int SAGE_Provisioning_ValidationKey(void);
extern int SAGE_Provisioning_ProcessBinFile(void);
#endif

#if __________p__________
#endif
#if defined(HDCP_CERTI_MODE)
extern 	DRV_Error P_HDMI_StartHDCP(void);
#endif
void P_HDMI_ConvertNexusAudioCodecToHdmiAudioFormat(NEXUS_AudioCodec eNexusAudioCodec, DI_HDMI_AUDIO_FORMAT *pAudioFormat)
{
	switch(eNexusAudioCodec)
	{
	case NEXUS_AudioCodec_ePcm:
		*pAudioFormat = DI_HDMI_AFORMAT_PCM; break;
	case NEXUS_AudioCodec_eAc3:
		*pAudioFormat = DI_HDMI_AFORMAT_AC3; break;
	case NEXUS_AudioCodec_eAc3Plus:
		*pAudioFormat = DI_HDMI_AFORMAT_AC3PLUS; break;
	case NEXUS_AudioCodec_eMpeg:
		*pAudioFormat = DI_HDMI_AFORMAT_MPEG1 | DI_HDMI_AFORMAT_MPEG2; break;
	case NEXUS_AudioCodec_eMp3:
		*pAudioFormat = DI_HDMI_AFORMAT_MP3; break;
	case NEXUS_AudioCodec_eAac:
		*pAudioFormat = DI_HDMI_AFORMAT_AAC; break;
	case NEXUS_AudioCodec_eDts:
		*pAudioFormat = DI_HDMI_AFORMAT_DTS; break;
	case NEXUS_AudioCodec_eDtsHd:
		*pAudioFormat = DI_HDMI_AFORMAT_DTSHD; break;
	case NEXUS_AudioCodec_eWmaPro:
		*pAudioFormat = DI_HDMI_AFORMAT_WMAPro; break;
	default:
		*pAudioFormat = DI_HDMI_AFORMAT_UNKNOWN; break;
	}
}

void P_HDMI_ConvertVideoIdCodeToHdmiEdidVideoFormat(HUINT32 ulVideoIdCode, DI_HDMI_EDID_VFORMAT *pstEdidVideoFormat)
{
	switch (ulVideoIdCode)
	{
	case 1:
		pstEdidVideoFormat->ulVFormat		= DI_HDMI_RESOLUTION_640x480P;
		pstEdidVideoFormat->ulFieldRate 	= DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	= DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 2:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 3:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 4:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 5:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 6:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 7:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 8:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x240P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 9:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x240P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 10:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 11:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 12:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x240P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 13:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x240P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 14:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1440x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 15:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1440x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 16:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 17:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 18:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 19:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 20:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 21:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 22:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 23:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x288P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 24:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x288P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 25:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 26:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 27:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x288P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 28:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x288P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 29:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1440x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 30:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1440x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 31:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 32:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 33:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 34:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 35:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 36:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 37:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 38:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2880x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 39:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 40:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 41:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 42:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 43:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 44:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 45:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 46:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 47:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 48:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 49:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 50:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 51:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 52:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_200HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 53:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_200HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 54:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_200HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 55:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x576I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_200HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 56:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_240HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 57:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_240HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 58:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_240HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 59:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_720x480I;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_240HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 60:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 61:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 62:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 63:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 64:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 65:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 66:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 67:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 68:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 69:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 70:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 71:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1280x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 72:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 73:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 74:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 75:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 76:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 77:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 78:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1920x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 79:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 80:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 81:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 82:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 83:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 84:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 85:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_1680x720P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 86:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 87:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 88:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 89:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 90:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 91:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_100HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 92:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_2560x1080P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_120HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 93:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 94:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 95:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 96:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 97:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e16_9;
		break;

	case 98:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 99:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 100:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 101:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 102:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_4096x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;

	case 103:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 104:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 105:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 106:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	case 107:
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_3840x2160P;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_e4_3;
		break;

	default :
		pstEdidVideoFormat->ulVFormat		=DI_HDMI_RESOLUTION_Unknown;
		pstEdidVideoFormat->ulFieldRate 	=DI_HDMI_FIELD_RATE_Unknown;
		pstEdidVideoFormat->ulAspectRatio	=DI_HDMI_VIDEO_AR_eNoData;
		break;
	}
}


void P_HDMI_ConvertVideoIdCodeToHdmiVideoFormat(HUINT32 ulVideoIdCode, DI_HDMI_VIDEO_FORMAT *peVideoFormat)
{
	switch (ulVideoIdCode)
	{
	case 2:
		*peVideoFormat = DI_HDMI_VFORMAT_480P_4_3;
		break;
	case 3:
		*peVideoFormat = DI_HDMI_VFORMAT_480P;
		break;
	case 4:
		*peVideoFormat = DI_HDMI_VFORMAT_720P;
		break;
	case 5:
		*peVideoFormat = DI_HDMI_VFORMAT_1080I;
		break;
	case 6:
		*peVideoFormat = DI_HDMI_VFORMAT_480I_4_3;
		break;
	case 7:
		*peVideoFormat = DI_HDMI_VFORMAT_480I;
		break;
	case 16:
		*peVideoFormat = DI_HDMI_VFORMAT_1080P;
		break;
	case 17:
		*peVideoFormat = DI_HDMI_VFORMAT_576P_4_3;
		break;
	case 18:
		*peVideoFormat = DI_HDMI_VFORMAT_576P;
		break;
	case 19:
		*peVideoFormat = DI_HDMI_VFORMAT_720P_50HZ;
		break;
	case 20:
		*peVideoFormat = DI_HDMI_VFORMAT_1080I_50HZ;
		break;
	case 21:
		*peVideoFormat = DI_HDMI_VFORMAT_576I_4_3;
		break;
	case 22:
		*peVideoFormat = DI_HDMI_VFORMAT_576I;
		break;
	case 31:
		*peVideoFormat = DI_HDMI_VFORMAT_1080P_50HZ;
		break;
	case 32:
		*peVideoFormat = DI_HDMI_VFORMAT_1080P_24HZ;
		break;
	case 33:
		*peVideoFormat = DI_HDMI_VFORMAT_1080P_25HZ;
		break;
	case 34:
		*peVideoFormat = DI_HDMI_VFORMAT_1080P_30HZ;
		break;
	case 93:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_24HZ;
		break;
	case 94:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_25HZ;
		break;
	case 95:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_30HZ;
		break;
	case 96:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_50HZ;
		break;
	case 97:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_60HZ;
		break;
	case 98:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_24HZ;
		break;
	case 99:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_25HZ;
		break;
	case 100:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_30HZ;
		break;
	case 101:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_50HZ;
		break;
	case 102:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_60HZ;
		break;
	case 103:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_24HZ;
		break;
	case 104:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_25HZ;
		break;
	case 105:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_30HZ;
		break;
	case 106:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_50HZ;
		break;
	case 107:
		*peVideoFormat = DI_HDMI_VFORMAT_2160P_60HZ;
		break;
	default :
		break;
	}
}

void P_HDMI_ConvertNexusVideoFormatToHdmiEdidVideoFormat(NEXUS_VideoFormat eNexusVideoFormat,
																		DI_HDMI_EDID_VFORMAT *pstEdidVideoFormat)
{
	switch(eNexusVideoFormat)
	{
	case NEXUS_VideoFormat_e480p :		  /* NTSC Progressive (27Mhz) */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_720x480P;
		break;

	case NEXUS_VideoFormat_e576p :        /* HD PAL Progressive 50hz for Australia */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_720x576P;
		break;

	case NEXUS_VideoFormat_e1080i :       /* HD 1080 Interlaced */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080I;
		break;

	case NEXUS_VideoFormat_e1080i50hz :   /* European 50hz HD 1080 */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080I;
		break;

	case NEXUS_VideoFormat_e1080p24hz :   /* HD 1080 Progressive, 24hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080P;
		break;

	case NEXUS_VideoFormat_e1080p25hz :   /* HD 1080 Progressive, 25hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080P;
		break;

	case NEXUS_VideoFormat_e1080p30hz :   /* HD 1080 Progressive, 30hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080P;
		break;

	case NEXUS_VideoFormat_e1080p50hz :   /* HD 1080 Progressive, 50hz. */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080P;
		break;

	case NEXUS_VideoFormat_e1080p60hz :   /* HD 1080 Progressive, 60hz */
			/* NEXUS_VideoFormat_e1080p = NEXUS_VideoFormat_e1080p60hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1920x1080P;
		break;

	case NEXUS_VideoFormat_e720p :        /* HD 720 Progressive */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1280x720P;
		break;

	case NEXUS_VideoFormat_e720p50hz :    /* HD 720p 50hz for Australia */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1280x720P;
		break;

	case NEXUS_VideoFormat_e720p24hz :    /* HD 720p 24hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1280x720P;
		break;

	case NEXUS_VideoFormat_e720p25hz :    /* HD 720p 25hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1280x720P;
		break;

	case NEXUS_VideoFormat_e720p30hz :    /* HD 720p 30hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_1280x720P;
		break;
#if (NEXUS_VERSION >= 1401)
	case NEXUS_VideoFormat_e3840x2160p24hz :  /* UHD 3840x2160 24Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_3840x2160P;
		break;

	case NEXUS_VideoFormat_e3840x2160p25hz :  /* UHD 3840x2160 25Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_3840x2160P;
		break;

	case NEXUS_VideoFormat_e3840x2160p30hz :  /* UHD 3840x2160 30Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_3840x2160P;
		break;
#if (NEXUS_VERSION >= 1402)
	case NEXUS_VideoFormat_e3840x2160p50hz :  /* UHD 3840x2160 50Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_3840x2160P;
		break;

	case NEXUS_VideoFormat_e3840x2160p60hz :  /* UHD 3840x2160 60Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_e16_9;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_3840x2160P;
		break;
#endif
	case NEXUS_VideoFormat_e4096x2160p24hz :  /* UHD 4096x2160 24Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_24HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_4096x2160P;
		break;
#if (NEXUS_VERSION >= 1402)
	case NEXUS_VideoFormat_e4096x2160p25hz :  /* UHD 4096x2160 25Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_25HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_4096x2160P;
		break;

	case NEXUS_VideoFormat_e4096x2160p30hz :  /* UHD 4096x2160 30Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_30HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_4096x2160P;
		break;

	case NEXUS_VideoFormat_e4096x2160p50hz :  /* UHD 4096x2160 50Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_50HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_4096x2160P;
		break;

	case NEXUS_VideoFormat_e4096x2160p60hz :  /* UHD 4096x2160 60Hz */
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_60HZ;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_4096x2160P;
		break;
#endif
#endif
	case NEXUS_VideoFormat_eUnknown :     /* unknown/not supported video format */
	case NEXUS_VideoFormat_eNtsc :        /* 480i, NTSC-M for North America */
	case NEXUS_VideoFormat_eNtsc443 :     /* NTSC encoding with the PAL color carrier frequency. */
	case NEXUS_VideoFormat_eNtscJapan :   /* Japan NTSC, no pedestal level */
	case NEXUS_VideoFormat_ePalM :        /* PAL Brazil */
	case NEXUS_VideoFormat_ePalN :        /* PAL_N */
	case NEXUS_VideoFormat_ePalNc :       /* PAL_N, Argentina */
	case NEXUS_VideoFormat_ePalB :        /* Australia */
	case NEXUS_VideoFormat_ePalB1 :       /* Hungary */
	case NEXUS_VideoFormat_ePalD :        /* China */
	case NEXUS_VideoFormat_ePalD1 :       /* Poland */
	case NEXUS_VideoFormat_ePalDK2 :      /* Eastern Europe */
	case NEXUS_VideoFormat_ePalDK3 :      /* Eastern Europe */
	case NEXUS_VideoFormat_ePalG :        /* Europe. Same as NEXUS_VideoFormat_ePal. */
	case NEXUS_VideoFormat_ePalH :        /* Europe */
	case NEXUS_VideoFormat_ePalK :        /* Europe */
	case NEXUS_VideoFormat_ePalI :        /* U.K. */
	case NEXUS_VideoFormat_ePal60hz :     /* 60Hz PAL */
	case NEXUS_VideoFormat_eSecamL :      /* France */
	case NEXUS_VideoFormat_eSecamB :      /* Middle East */
	case NEXUS_VideoFormat_eSecamG :      /* Middle East */
	case NEXUS_VideoFormat_eSecamD :      /* Eastern Europe */
	case NEXUS_VideoFormat_eSecamK :      /* Eastern Europe */
	case NEXUS_VideoFormat_eSecamH :      /* Line SECAM */
	case NEXUS_VideoFormat_e1250i50hz :   /* HD 1250 Interlaced, 50hz */
	case NEXUS_VideoFormat_e240p60hz :    /* NTSC 240p */
	case NEXUS_VideoFormat_e288p50hz :    /* PAL 288p */
	case NEXUS_VideoFormat_e1440x480p60hz : /* CEA861B */
	case NEXUS_VideoFormat_e1440x576p50hz : /* CEA861B */
	case NEXUS_VideoFormat_eCustomer1440x240p60hz :             /* 240p 60hz 7411 custom format. */
	case NEXUS_VideoFormat_eCustomer1440x288p50hz :             /* 288p 50hz 7411 custom format. */
	case NEXUS_VideoFormat_eCustomer1366x768p60hz :             /* Custom 1366x768 mode */
	case NEXUS_VideoFormat_eCustomer1366x768p50hz :             /* Custom 1366x768 50hz mode */
	case NEXUS_VideoFormat_eVesa640x480p60hz :                  /* DVI Safe mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p60hzRed :              /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p50hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720pReducedBlank :         /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p66hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p56hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p59hzRed :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa848x480p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p66hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p70hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p72hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1064x600p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p70hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p72hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768i87hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x864p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x800p_60Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x960p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x960p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p60hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p69hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p75hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p85hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa832x624p75hz :                  /* MAC-16 */
	case NEXUS_VideoFormat_eVesa1360x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1366x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1400x1050p60hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank :    /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1400x1050p75hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1440x900p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1200p60hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank :    /* DVI VESA mode for computer monitors */

	    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
	case NEXUS_VideoFormat_e720p_3DOU_AS :                      /* HD 3D 720p */
	case NEXUS_VideoFormat_e720p50hz_3DOU_AS :                  /* HD 3D 720p50 */
	case NEXUS_VideoFormat_e720p30hz_3DOU_AS :                  /* HD 3D 720p30 */
	case NEXUS_VideoFormat_e720p24hz_3DOU_AS :                  /* HD 3D 720p24 */
	case NEXUS_VideoFormat_e1080p24hz_3DOU_AS :                 /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	case NEXUS_VideoFormat_e1080p30hz_3DOU_AS :                 /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */

	case NEXUS_VideoFormat_eVesa1680x1050p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red :            /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1200p_75Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red :            /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red :           /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1920x1200p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red :           /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x720p_60Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x720p_75Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x720p_85Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x864p_60Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x864p_85Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x870p_75Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x900p_66Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x900p_76Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1170x584p_50Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p_70Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p_72Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p_76Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p_50Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x960p_75Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1024p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1024p_76Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1728x1080p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p_100Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p_90Hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa848x480p_75Hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa848x480p_85Hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa852x480p_60Hz :                 /* DVI VESA mode for computer monitors */
#if (NEXUS_VERSION>=1290)
	case NEXUS_VideoFormat_e720x482_NTSC :                      /* 720x482i NTSC-M for North America */
	case NEXUS_VideoFormat_e720x482_NTSC_J :                    /* 720x482i Japan */
	case NEXUS_VideoFormat_e720x483p :                          /* 720x483p */
#endif
	    /* dynamics: custom format */
	case NEXUS_VideoFormat_eCustom0 :         /* Customized video format 0. This enums allows the user to select custom display settings programmed in
	                                           magnum/commonutils/fmt/bfmt_custom.c. This cannot be changed at run time.
	                                           Typically, the default bfmt_custom.c sets eCustom0 to be a 60Hz format and eCustom1 to be a 50Hz format.
	                                           Custom settings can be obtained upon request. */
	case NEXUS_VideoFormat_eCustom1 :         /* Customized video format 1. See NEXUS_VideoFormat_eCustom0 for details. */
	case NEXUS_VideoFormat_eCustom2 :         /* eCustom2 is defined at run time by calling NEXUS_Display_SetCustomFormatSettings.
	                                           This runtime modification cannot be done for eCustom0 and eCustom1.
	                                           Typically, apps set eCustom2 to be a 48Hz format, which also supports 24Hz content. */

	    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
	case NEXUS_VideoFormat_eCustom1920x2160i_48Hz : /* 3DTV panel format */

	    /* Legacy 3D timing and display formats */
	case NEXUS_VideoFormat_eCustom_3D_720p :        /* HD 3D 720p with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_720p_50hz :   /* HD 3D 720p50hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_720p_30hz :   /* HD 3D 720p30hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_720p_24hz :   /* HD 3D 720p24hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_1080p_24hz :  /* HD 3D 1080p24hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_1080p_30hz :  /* HD 3D 1080p30hz with custom timing */

	case NEXUS_VideoFormat_eMax :             /* Total number of video formats */
	default :
		pstEdidVideoFormat->ulAspectRatio = DI_HDMI_VIDEO_AR_eNoData;
		pstEdidVideoFormat->ulFieldRate = DI_HDMI_FIELD_RATE_Unknown;
		pstEdidVideoFormat->ulVFormat = DI_HDMI_RESOLUTION_Unknown;
		break;
	}
}

void P_HDMI_ConvertNexusVideoFormatToHdmiVideoFormat(NEXUS_VideoFormat eNexusVideoFormat,
																		DI_HDMI_VIDEO_FORMAT *peHdmiVideoFormat)
{
	switch(eNexusVideoFormat)
	{
	case NEXUS_VideoFormat_e480p :		  /* NTSC Progressive (27Mhz) */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_480P;
		break;

	case NEXUS_VideoFormat_e576p :        /* HD PAL Progressive 50hz for Australia */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_576P;
		break;

	case NEXUS_VideoFormat_e1080i :       /* HD 1080 Interlaced */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080I;
		break;

	case NEXUS_VideoFormat_e1080i50hz :   /* European 50hz HD 1080 */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080I_50HZ;
		break;

	case NEXUS_VideoFormat_e1080p24hz :   /* HD 1080 Progressive, 24hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080P_24HZ;
		break;

	case NEXUS_VideoFormat_e1080p25hz :   /* HD 1080 Progressive, 25hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080P_25HZ;
		break;

	case NEXUS_VideoFormat_e1080p30hz :   /* HD 1080 Progressive, 30hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080P_30HZ;
		break;

	case NEXUS_VideoFormat_e1080p50hz :   /* HD 1080 Progressive, 50hz. */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080P_50HZ;
		break;

	case NEXUS_VideoFormat_e1080p60hz :   /* HD 1080 Progressive, 60hz */
			/* NEXUS_VideoFormat_e1080p = NEXUS_VideoFormat_e1080p60hz */
			*peHdmiVideoFormat = DI_HDMI_VFORMAT_1080P;
		break;

	case NEXUS_VideoFormat_e720p30hz :    /* HD 720p 30hz */
	case NEXUS_VideoFormat_e720p :        /* HD 720 Progressive */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_720P;
		break;

	case NEXUS_VideoFormat_e720p24hz :	  /* HD 720p 24hz */
	case NEXUS_VideoFormat_e720p25hz :	  /* HD 720p 25hz */
	case NEXUS_VideoFormat_e720p50hz :    /* HD 720p 50hz for Australia */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_720P_50HZ;
		break;
#if (NEXUS_VERSION >= 1401)
	case NEXUS_VideoFormat_e3840x2160p24hz :  /* UHD 3840x2160 24Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_24HZ;
		break;

	case NEXUS_VideoFormat_e3840x2160p25hz :  /* UHD 3840x2160 25Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_25HZ;
		break;

	case NEXUS_VideoFormat_e3840x2160p30hz :  /* UHD 3840x2160 30Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_30HZ;
		break;
#if (NEXUS_VERSION >= 1402)
	case NEXUS_VideoFormat_e3840x2160p50hz :  /* UHD 3840x2160 50Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_50HZ;
		break;

	case NEXUS_VideoFormat_e3840x2160p60hz :  /* UHD 3840x2160 60Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_60HZ;
		break;
#endif
	case NEXUS_VideoFormat_e4096x2160p24hz :  /* UHD 4096x2160 24Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_24HZ;
		break;
#if (NEXUS_VERSION >= 1402)
	case NEXUS_VideoFormat_e4096x2160p25hz :  /* UHD 4096x2160 25Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_25HZ;
		break;

	case NEXUS_VideoFormat_e4096x2160p30hz :  /* UHD 4096x2160 30Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_30HZ;
		break;

	case NEXUS_VideoFormat_e4096x2160p50hz :  /* UHD 4096x2160 50Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_50HZ;
		break;

	case NEXUS_VideoFormat_e4096x2160p60hz :  /* UHD 4096x2160 60Hz */
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_2160P_60HZ;
		break;
#endif
#endif
	case NEXUS_VideoFormat_eUnknown :     /* unknown/not supported video format */
	case NEXUS_VideoFormat_eNtsc :        /* 480i, NTSC-M for North America */
	case NEXUS_VideoFormat_eNtsc443 :     /* NTSC encoding with the PAL color carrier frequency. */
	case NEXUS_VideoFormat_eNtscJapan :   /* Japan NTSC, no pedestal level */
	case NEXUS_VideoFormat_ePalM :        /* PAL Brazil */
	case NEXUS_VideoFormat_ePalN :        /* PAL_N */
	case NEXUS_VideoFormat_ePalNc :       /* PAL_N, Argentina */
	case NEXUS_VideoFormat_ePalB :        /* Australia */
	case NEXUS_VideoFormat_ePalB1 :       /* Hungary */
	case NEXUS_VideoFormat_ePalD :        /* China */
	case NEXUS_VideoFormat_ePalD1 :       /* Poland */
	case NEXUS_VideoFormat_ePalDK2 :      /* Eastern Europe */
	case NEXUS_VideoFormat_ePalDK3 :      /* Eastern Europe */
	case NEXUS_VideoFormat_ePalG :        /* Europe. Same as NEXUS_VideoFormat_ePal. */
	case NEXUS_VideoFormat_ePalH :        /* Europe */
	case NEXUS_VideoFormat_ePalK :        /* Europe */
	case NEXUS_VideoFormat_ePalI :        /* U.K. */
	case NEXUS_VideoFormat_ePal60hz :     /* 60Hz PAL */
	case NEXUS_VideoFormat_eSecamL :      /* France */
	case NEXUS_VideoFormat_eSecamB :      /* Middle East */
	case NEXUS_VideoFormat_eSecamG :      /* Middle East */
	case NEXUS_VideoFormat_eSecamD :      /* Eastern Europe */
	case NEXUS_VideoFormat_eSecamK :      /* Eastern Europe */
	case NEXUS_VideoFormat_eSecamH :      /* Line SECAM */
	case NEXUS_VideoFormat_e1250i50hz :   /* HD 1250 Interlaced, 50hz */
	case NEXUS_VideoFormat_e240p60hz :    /* NTSC 240p */
	case NEXUS_VideoFormat_e288p50hz :    /* PAL 288p */
	case NEXUS_VideoFormat_e1440x480p60hz : /* CEA861B */
	case NEXUS_VideoFormat_e1440x576p50hz : /* CEA861B */
	case NEXUS_VideoFormat_eCustomer1440x240p60hz :             /* 240p 60hz 7411 custom format. */
	case NEXUS_VideoFormat_eCustomer1440x288p50hz :             /* 288p 50hz 7411 custom format. */
	case NEXUS_VideoFormat_eCustomer1366x768p60hz :             /* Custom 1366x768 mode */
	case NEXUS_VideoFormat_eCustomer1366x768p50hz :             /* Custom 1366x768 50hz mode */
	case NEXUS_VideoFormat_eVesa640x480p60hz :                  /* DVI Safe mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p60hzRed :              /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p50hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720pReducedBlank :         /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x350p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x400p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p66hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa640x480p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa720x400p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p56hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p59hzRed :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p70hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p72hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p75hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p85hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa848x480p60hz :                  /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p66hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p70hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p72hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1064x600p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p70hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p72hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x720p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1024x768i87hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x864p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p75hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x800p_60Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x960p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x960p85hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p60hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p69hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p75hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p85hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa832x624p75hz :                  /* MAC-16 */
	case NEXUS_VideoFormat_eVesa1360x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1366x768p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1400x1050p60hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank :    /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1400x1050p75hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1440x900p60hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1200p60hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank :    /* DVI VESA mode for computer monitors */

	    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
	case NEXUS_VideoFormat_e720p_3DOU_AS :                      /* HD 3D 720p */
	case NEXUS_VideoFormat_e720p50hz_3DOU_AS :                  /* HD 3D 720p50 */
	case NEXUS_VideoFormat_e720p30hz_3DOU_AS :                  /* HD 3D 720p30 */
	case NEXUS_VideoFormat_e720p24hz_3DOU_AS :                  /* HD 3D 720p24 */
	case NEXUS_VideoFormat_e1080p24hz_3DOU_AS :                 /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	case NEXUS_VideoFormat_e1080p30hz_3DOU_AS :                 /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */

	case NEXUS_VideoFormat_eVesa1680x1050p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red :            /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1200p_75Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red :            /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red :           /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1920x1200p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red :           /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x720p_60Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x720p_75Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x720p_85Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x864p_60Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x864p_85Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x870p_75Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x900p_66Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1152x900p_76Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1170x584p_50Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p_70Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p_72Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x1024p_76Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x768p_50Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1280x960p_75Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1024p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1600x1024p_76Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa1728x1080p_60Hz :               /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p_100Hz :                /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa800x600p_90Hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa848x480p_75Hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa848x480p_85Hz :                 /* DVI VESA mode for computer monitors */
	case NEXUS_VideoFormat_eVesa852x480p_60Hz :                 /* DVI VESA mode for computer monitors */
#if (NEXUS_VEESION >= 1290)
	case NEXUS_VideoFormat_e720x482_NTSC :                      /* 720x482i NTSC-M for North America */
	case NEXUS_VideoFormat_e720x482_NTSC_J :                    /* 720x482i Japan */
	case NEXUS_VideoFormat_e720x483p :                          /* 720x483p */
#endif
	    /* dynamics: custom format */
	case NEXUS_VideoFormat_eCustom0 :         /* Customized video format 0. This enums allows the user to select custom display settings programmed in
	                                           magnum/commonutils/fmt/bfmt_custom.c. This cannot be changed at run time.
	                                           Typically, the default bfmt_custom.c sets eCustom0 to be a 60Hz format and eCustom1 to be a 50Hz format.
	                                           Custom settings can be obtained upon request. */
	case NEXUS_VideoFormat_eCustom1 :         /* Customized video format 1. See NEXUS_VideoFormat_eCustom0 for details. */
	case NEXUS_VideoFormat_eCustom2 :         /* eCustom2 is defined at run time by calling NEXUS_Display_SetCustomFormatSettings.
	                                           This runtime modification cannot be done for eCustom0 and eCustom1.
	                                           Typically, apps set eCustom2 to be a 48Hz format, which also supports 24Hz content. */

	    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
	case NEXUS_VideoFormat_eCustom1920x2160i_48Hz : /* 3DTV panel format */

	    /* Legacy 3D timing and display formats */
	case NEXUS_VideoFormat_eCustom_3D_720p :        /* HD 3D 720p with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_720p_50hz :   /* HD 3D 720p50hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_720p_30hz :   /* HD 3D 720p30hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_720p_24hz :   /* HD 3D 720p24hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_1080p_24hz :  /* HD 3D 1080p24hz with custom timing */
	case NEXUS_VideoFormat_eCustom_3D_1080p_30hz :  /* HD 3D 1080p30hz with custom timing */

	case NEXUS_VideoFormat_eMax :             /* Total number of video formats */
	default :
		*peHdmiVideoFormat = DI_HDMI_VFORMAT_DUMMY;
		break;
	}
}

const char * P_HDMI_GetHdmiAudioFormatStr(DI_HDMI_AUDIO_FORMAT eAudioFormat)
{
	switch(eAudioFormat)
	{
	case DI_HDMI_AFORMAT_PCM:
		return "DI_HDMI_AFORMAT_PCM";
	case DI_HDMI_AFORMAT_AC3:
		return "DI_HDMI_AFORMAT_AC3";
	case DI_HDMI_AFORMAT_AC3PLUS:
		return "DI_HDMI_AFORMAT_AC3PLUS";
	case DI_HDMI_AFORMAT_MPEG1:
		return "DI_HDMI_AFORMAT_MPEG1";
	case DI_HDMI_AFORMAT_MPEG2:
		return "DI_HDMI_AFORMAT_MPEG2";
	case DI_HDMI_AFORMAT_MP3:
		return "DI_HDMI_AFORMAT_MP3";
	case DI_HDMI_AFORMAT_AAC:
		return "DI_HDMI_AFORMAT_AAC";
	case DI_HDMI_AFORMAT_DTS:
		return "DI_HDMI_AFORMAT_DTS";
	case DI_HDMI_AFORMAT_DTSHD:
		return "DI_HDMI_AFORMAT_DTSHD";
	case DI_HDMI_AFORMAT_WMAPro:
		return "DI_HDMI_AFORMAT_WMAPro";
	default:
		return "Unknown Audio Format";
	}
}

const char * P_HDMI_GetHdmiVideoFormatStr(DI_HDMI_VIDEO_FORMAT eVideoFormat)
{
	switch (eVideoFormat)
	{
	case DI_HDMI_VFORMAT_480I :
		return "DI_HDMI_VFORMAT_480I";
	case DI_HDMI_VFORMAT_480I_J :
		return "DI_HDMI_VFORMAT_480I_J";
	case DI_HDMI_VFORMAT_480P :
		return "DI_HDMI_VFORMAT_480P";
	case DI_HDMI_VFORMAT_576I:
		return "DI_HDMI_VFORMAT_576I";
	case DI_HDMI_VFORMAT_576P :
		return "DI_HDMI_VFORMAT_576P";
	case DI_HDMI_VFORMAT_720P :
		return "DI_HDMI_VFORMAT_720P";
	case DI_HDMI_VFORMAT_1080I :
		return "DI_HDMI_VFORMAT_1080I";
	case DI_HDMI_VFORMAT_720P_50HZ :
		return "DI_HDMI_VFORMAT_720P_50HZ";
	case DI_HDMI_VFORMAT_1080I_50HZ :
		return "DI_HDMI_VFORMAT_1080I_50HZ";
	case DI_HDMI_VFORMAT_1080P_50HZ :
		return "DI_HDMI_VFORMAT_1080P_50HZ";
	case DI_HDMI_VFORMAT_480I_4_3 :
		return "DI_HDMI_VFORMAT_480I_4_3";
	case DI_HDMI_VFORMAT_480P_4_3 :
		return "DI_HDMI_VFORMAT_480P_4_3";
	case DI_HDMI_VFORMAT_576I_4_3 :
		return "DI_HDMI_VFORMAT_576I_4_3";
	case DI_HDMI_VFORMAT_576P_4_3 :
		return "DI_HDMI_VFORMAT_576P_4_3";
	case DI_HDMI_VFORMAT_1080P_24HZ :
		return "DI_HDMI_VFORMAT_1080P_24HZ";
	case DI_HDMI_VFORMAT_1080P_25HZ :
		return "DI_HDMI_VFORMAT_1080P_25HZ";
	case DI_HDMI_VFORMAT_1080P_30HZ :
		return "DI_HDMI_VFORMAT_1080P_30HZ";
	case DI_HDMI_VFORMAT_1080P :
		return "DI_HDMI_VFORMAT_1080P";
 	case DI_HDMI_VFORMAT_2160P_24HZ :
		return "DI_HDMI_VFORMAT_2160P_24HZ";
	case DI_HDMI_VFORMAT_2160P_25HZ :
		return "DI_HDMI_VFORMAT_2160P_25HZ";
	case DI_HDMI_VFORMAT_2160P_30HZ :
		return "DI_HDMI_VFORMAT_2160P_30HZ";
	case DI_HDMI_VFORMAT_2160P_50HZ :
		return "DI_HDMI_VFORMAT_2160P_50HZ";
	case DI_HDMI_VFORMAT_2160P_60HZ :
		return "DI_HDMI_VFORMAT_2160P_60HZ";
	case DI_HDMI_VFORMAT_DUMMY :
		return "DI_HDMI_VFORMAT_DUMMY";
	default:
		return "Unknown Video Format";
	}
}


const char * P_HDMI_GetHdmiResolutionStr(DI_HDMI_VIDEO_RESOLUTION eResolution)
{
	switch (eResolution)
	{
	case DI_HDMI_RESOLUTION_Unknown :
		return "DI_HDMI_RESOLUTION_Unknown";
	case DI_HDMI_RESOLUTION_720x240P :
		return "DI_HDMI_RESOLUTION_720x240P";
	case DI_HDMI_RESOLUTION_2880x240P :
		return "DI_HDMI_RESOLUTION_2880x240P";
	case DI_HDMI_RESOLUTION_640x480P :
		return "DI_HDMI_RESOLUTION_640x480P";
	case DI_HDMI_RESOLUTION_720x480I :
		return "DI_HDMI_RESOLUTION_720x480I";
	case DI_HDMI_RESOLUTION_720x480P :
		return "DI_HDMI_RESOLUTION_720x480P";
	case DI_HDMI_RESOLUTION_1440x480P :
		return "DI_HDMI_RESOLUTION_1440x480P";
	case DI_HDMI_RESOLUTION_2880x480I :
		return "DI_HDMI_RESOLUTION_2880x480I";
	case DI_HDMI_RESOLUTION_2880x480P :
		return "DI_HDMI_RESOLUTION_2880x480P";
	case DI_HDMI_RESOLUTION_720x288P :
		return "DI_HDMI_RESOLUTION_720x288P";
	case DI_HDMI_RESOLUTION_2880x288P :
		return "DI_HDMI_RESOLUTION_2880x288P";
	case DI_HDMI_RESOLUTION_720x576I :
		return "DI_HDMI_RESOLUTION_720x576I";
	case DI_HDMI_RESOLUTION_720x576P :
		return "DI_HDMI_RESOLUTION_720x576P";
	case DI_HDMI_RESOLUTION_1440x576P :
		return "DI_HDMI_RESOLUTION_1440x576P";
	case DI_HDMI_RESOLUTION_2880x576I :
		return "DI_HDMI_RESOLUTION_2880x576I";
	case DI_HDMI_RESOLUTION_2880x576P :
		return "DI_HDMI_RESOLUTION_2880x576P";
	case DI_HDMI_RESOLUTION_1080x720P :
		return "DI_HDMI_RESOLUTION_1080x720P";
	case DI_HDMI_RESOLUTION_1280x720P :
		return "DI_HDMI_RESOLUTION_1280x720P";
	case DI_HDMI_RESOLUTION_1920x1080I :
		return "DI_HDMI_RESOLUTION_1920x1080I";
	case DI_HDMI_RESOLUTION_1920x1080P :
		return "DI_HDMI_RESOLUTION_1920x1080P";
	case DI_HDMI_RESOLUTION_1680x720P :
		return "DI_HDMI_RESOLUTION_1680x720P";
	case DI_HDMI_RESOLUTION_2560x1080P :
		return "DI_HDMI_RESOLUTION_2560x1080P";
	case DI_HDMI_RESOLUTION_3840x2160P :
		return "DI_HDMI_RESOLUTION_3840x2160P";
	case DI_HDMI_RESOLUTION_4096x2160P :
		return "DI_HDMI_RESOLUTION_4096x2160P";
	case DI_HDMI_RESOLUTION_DUMMY :
		return "DI_HDMI_RESOLUTION_DUMMY";
	default:
		return "Unknown Video Resolution";
	}
}


const char * P_HDMI_GetHdmiFieldRateStr(DI_HDMI_VIDEO_FIELD_RATE eFieldRate)
{
	switch (eFieldRate)
	{
	case DI_HDMI_FIELD_RATE_Unknown :
		return "DI_HDMI_FIELD_RATE_Unknown";
	case DI_HDMI_FIELD_RATE_24HZ :
		return "DI_HDMI_FIELD_RATE_24HZ";
	case DI_HDMI_FIELD_RATE_25HZ :
		return "DI_HDMI_FIELD_RATE_25HZ";
	case DI_HDMI_FIELD_RATE_30HZ :
		return "DI_HDMI_FIELD_RATE_30HZ";
	case DI_HDMI_FIELD_RATE_50HZ :
		return "DI_HDMI_FIELD_RATE_50HZ";
	case DI_HDMI_FIELD_RATE_60HZ :
		return "DI_HDMI_FIELD_RATE_60HZ";
	case DI_HDMI_FIELD_RATE_100HZ :
		return "DI_HDMI_FIELD_RATE_100HZ";
	case DI_HDMI_FIELD_RATE_120HZ :
		return "DI_HDMI_FIELD_RATE_120HZ";
	case DI_HDMI_FIELD_RATE_200HZ :
		return "DI_HDMI_FIELD_RATE_200HZ";
	case DI_HDMI_FIELD_RATE_240HZ :
		return "DI_HDMI_FIELD_RATE_240HZ";
	case DI_HDMI_FIELD_RATE_DUMMY :
		return "DI_HDMI_FIELD_RATE_DUMMY";
	default:
		return "Unknown Field Rate";
	}
}


const char * P_HDMI_GetHdmiAspectRatioStr(DI_HDMI_VIDEO_AR eAspectRatio)
{
	switch (eAspectRatio)
	{
	case DI_HDMI_VIDEO_AR_eNoData :
		return "DI_HDMI_VIDEO_AR_eNoData";
	case DI_HDMI_VIDEO_AR_e4_3 :
		return "DI_HDMI_VIDEO_AR_e4_3";
	case DI_HDMI_VIDEO_AR_e16_9 :
		return "DI_HDMI_VIDEO_AR_e16_9";
	default:
		return "Unknown Aspect Ratio";
	}
}

/* add 14.01 example code func */
DRV_Error P_HDMI_ForceUpdate(void)
{
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputSettings hdmiSettings	 ;

	PrintEnter();

	/* force HDMI updates after a hotplug */
	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings) ;
	nexus_rc = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings) ;
	if (nexus_rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		PrintExit();
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error P_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable)
{
#if defined(CONFIG_HDMI_CEC)
	DRV_Error eDrvErr = DRV_OK;
#endif
	NEXUS_Error	nexus_rc = NEXUS_SUCCESS;
	NEXUS_HdmiOutputStatus	stHdmiOutputStatus;
	NEXUS_HdmiOutputBasicEdidData stHdmiBasicEdidData;

	DI_HDMI_AUDIO_FORMAT	eAFormat = DI_HDMI_AFORMAT_PCM;
	DI_HDMI_VIDEO_FORMAT	eVideoFormat = DI_HDMI_VFORMAT_DUMMY;
	DI_HDMI_EDID_VFORMAT	stEdidVideoFormat;
	HUINT32 offset = 0;
	HUINT32 i, j, /* indices */
		DataOffset,
		DataBlockIndex,
		DataBlockTag,
		DataBlockLength,
		NumVideoDescriptors,
		EdidVideoIDCode;
	HUINT8 extensions=0;
	NEXUS_HdmiOutputEdidBlock stEdidBlock;
	HBOOL bConnected = FALSE;
	char videoFormatStr[100] = {0, };

	PrintEnter();

	CHECK_CONNECTION(stHdmiOutputStatus, bConnected);

	if(!(bConnected))
	{
		PrintError("Device connection error, connected = %d, rxPowered = %d\n",
					stHdmiOutputStatus.connected, stHdmiOutputStatus.rxPowered);
		PrintExit();

		pAvailable->sPhysicalAddr.NodeA = 0xF;
		pAvailable->sPhysicalAddr.NodeB = 0xF;
		pAvailable->sPhysicalAddr.NodeC = 0xF;
		pAvailable->sPhysicalAddr.NodeD = 0xF;

		pAvailable->ulLogicalAddr = 0xF;
		return DRV_ERR;
	}

	/* Update basic edid data */
	nexus_rc = NEXUS_HdmiOutput_GetBasicEdidData(g_hdmi_instance.hHdmi, &stHdmiBasicEdidData);
	if (nexus_rc != NEXUS_SUCCESS)
	{
		
#if defined(CONFIG_TRAUMA_ERIN)
		TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_TVEDID,"NEXUS_HdmiOutput_GetBasicEdidData is failed\n");
#endif
		PrintError("NEXUS_HdmiOutput_GetEdidBlock failed!\n");
		PrintExit();
		return DRV_ERR;
	}

	VK_memset(&(g_hdmi_instance.stBasicEdidData), 0x00, sizeof(DI_HDMI_EDID_INFO));

	VK_memcpy(g_hdmi_instance.stBasicEdidData.vendorID, stHdmiBasicEdidData.vendorID, sizeof(stHdmiBasicEdidData.vendorID));
	VK_memcpy(g_hdmi_instance.stBasicEdidData.productID, stHdmiBasicEdidData.productID, sizeof(stHdmiBasicEdidData.productID));
	VK_memcpy(g_hdmi_instance.stBasicEdidData.serialNum, stHdmiBasicEdidData.serialNum, sizeof(stHdmiBasicEdidData.serialNum));

	g_hdmi_instance.stBasicEdidData.manufWeek = stHdmiBasicEdidData.manufWeek;
	g_hdmi_instance.stBasicEdidData.manufYear = stHdmiBasicEdidData.manufYear;
	g_hdmi_instance.stBasicEdidData.edidVersion = stHdmiBasicEdidData.edidVersion;
	g_hdmi_instance.stBasicEdidData.edidRevision = stHdmiBasicEdidData.edidRevision;
	g_hdmi_instance.stBasicEdidData.maxHorizSize = stHdmiBasicEdidData.maxHorizSize;
	g_hdmi_instance.stBasicEdidData.maxVertSize = stHdmiBasicEdidData.maxVertSize;
	g_hdmi_instance.stBasicEdidData.extensions = stHdmiBasicEdidData.extensions;

	g_hdmi_instance.stBasicEdidData.firstVideoId = 0;

	VK_memcpy(g_hdmi_instance.stBasicEdidData.monitorName, stHdmiOutputStatus.monitorName, sizeof(stHdmiOutputStatus.monitorName));

	/* Set auto lipsync info */
	g_hdmi_instance.stAutoLipsyncInfo.ucVideo = stHdmiOutputStatus.autoLipsyncInfo.videoLatency;
	g_hdmi_instance.stAutoLipsyncInfo.ucAudio = stHdmiOutputStatus.autoLipsyncInfo.audioLatency;
	g_hdmi_instance.stAutoLipsyncInfo.ucInterlacedVideo = stHdmiOutputStatus.autoLipsyncInfo.interlacedVideoLatency;
	g_hdmi_instance.stAutoLipsyncInfo.ucInterlacedAudio = stHdmiOutputStatus.autoLipsyncInfo.interlacedAudioLatency;

	/* Set auto lipsync state */
	if((g_hdmi_instance.stAutoLipsyncInfo.ucVideo != 0) ||
		(g_hdmi_instance.stAutoLipsyncInfo.ucAudio != 0) ||
		(g_hdmi_instance.stAutoLipsyncInfo.ucInterlacedVideo != 0) ||
		(g_hdmi_instance.stAutoLipsyncInfo.ucInterlacedAudio != 0))
	{
		g_hdmi_instance.stAutoLipsyncInfo.eState = DI_HDMI_AUTOLIPSYNC_ON;
	}
	else
	{
		g_hdmi_instance.stAutoLipsyncInfo.eState = DI_HDMI_AUTOLIPSYNC_OFF;
	}

	/* get supported audio format & sampling rate list */
	VK_memset(pAvailable, 0x00, sizeof(DI_HDMI_AVAILABLE_FORMATS));

	PrintDebug("Supported Audio Format : \n");

	for(i=0, j=0; i<NEXUS_AudioCodec_eMax; i++)
	{
		if (stHdmiOutputStatus.audioCodecSupported[i] == true)
		{
			P_HDMI_ConvertNexusAudioCodecToHdmiAudioFormat(i, &eAFormat);

			pAvailable->ulAudioFormats |= eAFormat;
			pAvailable->tAudioInfo[j].ulAudioFormat = eAFormat;
			pAvailable->tAudioInfo[j].unSupportedFs = stHdmiOutputStatus.audioInfo[i].uSamplingRate;

			PrintDebug("\t%s\n", P_HDMI_GetHdmiAudioFormatStr(eAFormat));
			PrintDebug("\tunSupportedFs = %d\n", pAvailable->tAudioInfo[j].unSupportedFs);

			j++;
		}
	}

	/* get physical address */
	pAvailable->sPhysicalAddr.NodeA = stHdmiOutputStatus.physicalAddressA;
	pAvailable->sPhysicalAddr.NodeB = stHdmiOutputStatus.physicalAddressB;
	pAvailable->sPhysicalAddr.NodeC = stHdmiOutputStatus.physicalAddressC;
	pAvailable->sPhysicalAddr.NodeD = stHdmiOutputStatus.physicalAddressD;

	/* check ALL extensions for Version 3 Timing Extensions */
	VK_memset(&stEdidBlock, 0x00, sizeof(NEXUS_HdmiOutputEdidBlock));

	nexus_rc = NEXUS_HdmiOutput_GetEdidBlock(g_hdmi_instance.hHdmi, 0, &stEdidBlock) ;
	if (nexus_rc != NEXUS_SUCCESS)
	{		
#if defined(CONFIG_TRAUMA_ERIN)
		TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_TVEDID,"NEXUS_HdmiOutput_GetEdidBlock is failed\n");
#endif
		PrintError("NEXUS_HdmiOutput_GetEdidBlock failed!\n");
		PrintExit();
		return DRV_ERR;
	}

	/* get raw edid data */
	VK_memset(g_hdmi_instance.stEdidBlock.data, 0x00, sizeof(DI_HDMI_EDID_RAW));

	VK_memcpy(&(g_hdmi_instance.stEdidBlock.data[offset]), &stEdidBlock, sizeof(NEXUS_HdmiOutputEdidBlock));
	offset += sizeof(NEXUS_HdmiOutputEdidBlock);

	/* if sink has extension block, get together */
	extensions = stEdidBlock.data[126];
	if (!extensions)
	{
		PrintDebug(("No EDID Extensions Found... No HDMI Support")) ;
		return DRV_ERR;
	}

	if (extensions > HDMI_EDID_MAX_EXTENSIONS)
	{
		PrintError("Invalid EDID block... extensions(%d)", extensions);
		return DRV_ERR;
	}

	for(i=1; i<=extensions; i++)
	{
		nexus_rc = NEXUS_HdmiOutput_GetEdidBlock(g_hdmi_instance.hHdmi, i, &stEdidBlock) ;
		if (nexus_rc != NEXUS_SUCCESS)
		{
#if defined(CONFIG_TRAUMA_ERIN)			
			TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_TVEDID,"NEXUS_HdmiOutput_GetEdidBlock is failed\n");
#endif
			PrintError("NEXUS_HdmiOutput_GetEdidBlock failed!\n");
			PrintExit();
			return DRV_ERR;
		}

		VK_memcpy(&g_hdmi_instance.stEdidBlock.data[offset], &stEdidBlock, sizeof(NEXUS_HdmiOutputEdidBlock));
		offset += sizeof(NEXUS_HdmiOutputEdidBlock);

		/* check for Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_TAG] != DRV_HDMI_EDID_EXT_TIMING_DATA)
		{
			continue;
		}

		/* check for Version 3 Timing Data Extension */
		if (stEdidBlock.data[DRV_HDMI_EDID_EXT_VERSION] != DRV_HDMI_EDID_TIMING_VERSION_3)
		{
			continue;
		}

		/* check if data blocks exist before the 18 Byte Detailed Timing data */
		DataOffset = stEdidBlock.data[DRV_HDMI_EDID_EXT_DATA_OFFSET] ;
		if ((DataOffset==0) || (DataOffset==4)) /* no Reserved Data is Available */
		{
			PrintDebug("-----V3 Timing Extension contains no CEA Data Blocks") ;
			continue ;			/* continue to the next Timing Extension */
		}

		/* set the index to the start of Data Blocks */
		DataBlockIndex = DRV_HDMI_EDID_EXT_DATA_BLOCK_COLLECTION ;

		pAvailable->ulNumOfVFormat = 0;

		/* scan through the data blocks and retrieve the necessary information */
		while (DataBlockIndex < DataOffset)
		{
			DataBlockTag = stEdidBlock.data[DataBlockIndex] >> 5 ;
			DataBlockLength = stEdidBlock.data[DataBlockIndex] & 0x1F ;

			DataBlockIndex++;

			if (DataBlockTag == DRV_HDMI_EDID_CeaDataBlockTag_eVideoDB)   /* Video DB */
			{
				/* check each video descriptor for requested video support */
				NumVideoDescriptors = DataBlockLength ;

				PrintDebug("Video Data Block found.\n");
				PrintDebug("Supported Video Format : \n");

				/* for each CEA Video ID Code Found */
				for (j=0 ; j<NumVideoDescriptors ; j++)
				{
					if(j > CEA_861_MAX_NUM_VIDEO_ID_CODE)
					{
						PrintError("Exceed Max Video Format\n");
						break;
					}

					/* get the supported Video Code ID; check if a native format */
					EdidVideoIDCode = stEdidBlock.data[DataBlockIndex + j] ;
					EdidVideoIDCode = EdidVideoIDCode  & 0x7F;

					P_HDMI_ConvertVideoIdCodeToHdmiEdidVideoFormat(EdidVideoIDCode, &stEdidVideoFormat);

					pAvailable->stVformats[pAvailable->ulNumOfVFormat].ulVFormat	= stEdidVideoFormat.ulVFormat;
					pAvailable->stVformats[pAvailable->ulNumOfVFormat].ulFieldRate	= stEdidVideoFormat.ulFieldRate;
					pAvailable->stVformats[pAvailable->ulNumOfVFormat].ulAspectRatio = stEdidVideoFormat.ulAspectRatio;

					P_HDMI_ConvertVideoIdCodeToHdmiVideoFormat(EdidVideoIDCode, &eVideoFormat);

					pAvailable->ulVideoFormats |= eVideoFormat;

					PrintDebug("\t%s | %s | %s | %s\n",
							P_HDMI_GetHdmiResolutionStr(stEdidVideoFormat.ulVFormat),
							P_HDMI_GetHdmiFieldRateStr(stEdidVideoFormat.ulFieldRate),
							P_HDMI_GetHdmiAspectRatioStr(stEdidVideoFormat.ulAspectRatio),
							P_HDMI_GetHdmiVideoFormatStr(eVideoFormat));

					pAvailable->ulNumOfVFormat++;

				} /* for each Supported CEA Video ID Code */
			} /* for each CEA Video ID Code found */
#if defined(CONFIG_HDOUT_2160P)
			else if (DataBlockTag == DRV_HDMI_EDID_CeaDataBlockTag_eExtendedDB)
			{
				/* Ycbcr 4:2:0 VDB(Video Data Block tag = 0x07)*/
				HUINT8 ExtTagCode;

				PrintDebug("Extended Data Block found.\n");

				ExtTagCode = stEdidBlock.data[DataBlockIndex];
				if(ExtTagCode == 0xE)
				{
					/* DataBlockLength - ExtTagCodeLength(1 Byte) */
					DataBlockLength--;
					DataBlockIndex++;

					NumVideoDescriptors = DataBlockLength;

					PrintDebug("YCbCr 4:2:0 Video Data Block found.\n");
					PrintDebug("Supported Reolutions Ycbcr 4:2:0 VDB : \n");
					/* for each CEA Video ID Code Found */
					for (j=0 ; j < NumVideoDescriptors; j++)
					{
						if(j > CEA_861_MAX_NUM_VIDEO_ID_CODE)
						{
							PrintError("Exceed Max Video Format\n");
							break;
						}
						/* get the supported Video Code ID; check if a native format */
						EdidVideoIDCode = stEdidBlock.data[DataBlockIndex + j] ;
						EdidVideoIDCode = EdidVideoIDCode  & 0x7F;

						P_HDMI_ConvertVideoIdCodeToHdmiEdidVideoFormat(EdidVideoIDCode, &stEdidVideoFormat);

						pAvailable->stVformats[pAvailable->ulNumOfVFormat].ulVFormat	= stEdidVideoFormat.ulVFormat;
						pAvailable->stVformats[pAvailable->ulNumOfVFormat].ulFieldRate	= stEdidVideoFormat.ulFieldRate;
						pAvailable->stVformats[pAvailable->ulNumOfVFormat].ulAspectRatio = stEdidVideoFormat.ulAspectRatio;

						P_HDMI_ConvertVideoIdCodeToHdmiVideoFormat(EdidVideoIDCode, &eVideoFormat);

						pAvailable->ulVideoFormats |= eVideoFormat;

						PrintDebug("\t%s | %s | %s | %s\n",
								P_HDMI_GetHdmiResolutionStr(stEdidVideoFormat.ulVFormat),
								P_HDMI_GetHdmiFieldRateStr(stEdidVideoFormat.ulFieldRate),
								P_HDMI_GetHdmiAspectRatioStr(stEdidVideoFormat.ulAspectRatio),
								P_HDMI_GetHdmiVideoFormatStr(eVideoFormat));

						pAvailable->ulNumOfVFormat++;

					} /* for each Supported CEA Video ID Code */
				}
			}
#endif
			DataBlockIndex += DataBlockLength;
		} /* while DataBlockIndex < DataOffset */
	} /* for each extension */

	/* set preferred video format according to NEXUS preferred video format */
	P_HDMI_ConvertNexusVideoFormatToHdmiEdidVideoFormat(stHdmiOutputStatus.preferredVideoFormat, &stEdidVideoFormat);

	P_HDMI_ConvertNexusVideoFormatToHdmiVideoFormat(stHdmiOutputStatus.preferredVideoFormat, &eVideoFormat);

	VK_memcpy(&(g_hdmi_instance.stBasicEdidData.preferredVideoFormat), &stEdidVideoFormat, sizeof(DI_HDMI_EDID_VFORMAT));

	VK_memcpy(&(g_hdmi_instance.stBasicEdidData.firstVideoId), &eVideoFormat, sizeof(HUINT32));

#if defined(CONFIG_VIDEO_DECODER)
	DRV_VIDEO_GetVideoFormatStr(stHdmiOutputStatus.preferredVideoFormat, videoFormatStr);
#endif

	PrintDebug("Preferred video format  : %s\n", videoFormatStr);

	/* get current logical address */
	pAvailable->ulLogicalAddr = 0x0f;

	/* get supported 3D format in the TV */
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
	eDrvErr = DRV_HDMI_CEC_GetLogicalAddress(g_hdmi_instance.hCec, &(pAvailable->ulLogicalAddr));
	if(eDrvErr != DRV_OK)
	{
		PrintError("DRV_HDMI_CEC_GetLogicalAddress() returned Error!!\n");
		return DRV_ERR;
	}
#endif

	PrintExit();

	return DRV_OK;
}

#if defined(CONFIG_HDCP)
/* TCB means Timer Callback */
void P_HDMI_TCB_EnableHdcp(unsigned long ulTimerHandle, void * peTimer)
{
	HBOOL bRequestedHdcpState = FALSE;
	DRV_Error dRes = DRV_OK;
	HAPPY(ulTimerHandle);
	HAPPY(peTimer);

	PrintEnter();

	PrintDebug("Timer Clear, Handle = 0x%X\n", s_ulTimerHandle);

	/* set timer handle to ZERO */
	s_ulTimerHandle = 0;

	LOCK_REQUEST();

	bRequestedHdcpState = g_hdmi_request.bHdcpEnable;

	UNLOCK_REQUEST();

	dRes = DRV_HDMI_EnableHDCP(bRequestedHdcpState);
	if(dRes != DRV_OK)
	{
		PrintError("DRV_HDMI_EnableHDCP() Failed!!\n");
		PrintExit();
		return;
	}

	return;
}
#endif

void P_HDMI_UnsetTimerFunction(void)
{
	HUINT32 res = 0;

	if(s_ulTimerHandle != 0)
	{
		PrintDebug("Timer already set, Cancel previous timer, Handle = 0x%X\n", s_ulTimerHandle);
		res = VK_TIMER_Cancel(s_ulTimerHandle);
		if(res != VK_OK)
		{
			PrintError("VK_TIMER_Cancel() Returned ERROR!!\n");
			return;
		}

		s_ulTimerHandle = 0;
	}
}

void P_HDMI_SetTimerFunction(HUINT32 ulTime, pfnTimerCallback timerFunc)
{
	HUINT32 res = 0;

	PrintEnter();

	if(s_ulTimerHandle != 0)
	{
		PrintDebug("Timer already set, Cancel previous timer, Handle = 0x%X\n", s_ulTimerHandle);
		res = VK_TIMER_Cancel(s_ulTimerHandle);
		if(res != VK_OK)
		{
			PrintError("VK_TIMER_Cancel() Returned ERROR!!\n");
			return;
		}
	}

	res = VK_TIMER_EventAfter(ulTime, timerFunc, NULL, 0, (unsigned long*)&s_ulTimerHandle);
	if (res != VK_OK)
	{
		PrintError("VK_TIMER_EventAfter() Returned ERROR!!\n");
		return;
	}

	PrintDebug("Timer set, Handle = 0x%X\n", s_ulTimerHandle);

	PrintExit();

	return;
}
#if defined(CONFIG_HPD_DELAY)
/* TCB means Timer Callback */
void P_HDMI_TCB_HotPlugConnected(unsigned long ulTimerHandle, void * peTimer)
{
	HdmiMsg_t	HdmiMsg;
	HAPPY(ulTimerHandle);
	HAPPY(peTimer);

	PrintEnter();

	PrintDebug("Timer Clear, Handle = 0x%X\n", s_ulTimerHotPlugHandle);

	/* set timer handle to ZERO */
	s_ulTimerHotPlugHandle = 0;

	HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HOTPLUG;
	HdmiMsg.param1 = HdmiHotplugState_Connected;

	SEND_MESSAGE(&HdmiMsg);

	return;
}

void P_HDMI_TCB_HotPlugDisconnected(unsigned long ulTimerHandle, void * peTimer)
{
	HdmiMsg_t	HdmiMsg;
	HAPPY(ulTimerHandle);
	HAPPY(peTimer);

	PrintEnter();

	PrintDebug("Timer Clear, Handle = 0x%X\n", s_ulTimerHotPlugHandle);

	/* set timer handle to ZERO */
	s_ulTimerHotPlugHandle = 0;

	HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HOTPLUG;
	HdmiMsg.param1 = HdmiHotplugState_Disconnected;

	SEND_MESSAGE(&HdmiMsg);

	return;
}


void P_HDMI_UnsetTimerHotPlugFunction(void)
{
	HUINT32 res = 0;

	if(s_ulTimerHotPlugHandle != 0)
	{
		PrintDebug("Timer already set, Cancel previous timer, Handle = 0x%X\n", s_ulTimerHotPlugHandle);
		res = VK_TIMER_Cancel(s_ulTimerHotPlugHandle);
		if(res != VK_OK)
		{
			PrintError("VK_TIMER_Cancel() Returned ERROR!!\n");
			return;
		}

		s_ulTimerHotPlugHandle = 0;
	}
}

void P_HDMI_SetTimerHotPlugFunction(HUINT32 ulTime, pfnTimerCallback timerFunc)
{
	HUINT32 res = 0;

	PrintEnter();

	if(s_ulTimerHotPlugHandle != 0)
	{
		PrintDebug("Timer already set, Cancel previous timer, Handle = 0x%X\n", s_ulTimerHotPlugHandle);
		res = VK_TIMER_Cancel(s_ulTimerHotPlugHandle);
		if(res != VK_OK)
		{
			PrintError("VK_TIMER_Cancel() Returned ERROR!!\n");
			return;
		}
	}

	res = VK_TIMER_EventAfter(ulTime, timerFunc, NULL, 0, (unsigned long*)&s_ulTimerHotPlugHandle);
	if (res != VK_OK)
	{
		PrintError("VK_TIMER_EventAfter() Returned ERROR!!\n");
		return;
	}

	PrintDebug("Timer set, Handle = 0x%X\n", s_ulTimerHotPlugHandle);

	PrintExit();

	return;
}
#endif

DRV_Error P_HDMI_SetAVMute(HBOOL bMute)
{
	DRV_Error dResult = DRV_OK;
	NEXUS_Error rc;

	PrintEnter();

	rc = NEXUS_HdmiOutput_SetAVMute(g_hdmi_instance.hHdmi, bMute) ;
	if (rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetAVMute failed\n");
		PrintExit();
		dResult = DRV_ERR;
		return dResult;
	}

	PrintExit();

	return dResult;
}

DRV_Error P_HDMI_SetTmds(HBOOL bEnable)
{
#if (NEXUS_VERSION >= 1403)
	DRV_Error dResult = DRV_OK;
	NEXUS_Error nexus_rc;
	NEXUS_HdmiOutputTmdsSignalSettings	tmds;

	PrintEnter();

	tmds.clock = bEnable;
	tmds.data = bEnable;

	nexus_rc = NEXUS_HdmiOutput_SetTmdsSignal(g_hdmi_instance.hHdmi, &tmds);
	if(nexus_rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetTmdsSignal failed!\n");
		dResult = DRV_ERR;
		PrintExit();
		return dResult;
	}

	PrintExit();
	return dResult;
#else
	DRV_Error dResult = DRV_OK;
	NEXUS_Error nexus_rc;

	PrintEnter();

	nexus_rc = NEXUS_HdmiOutput_SetTmdsSignal(g_hdmi_instance.hHdmi, bEnable);
	if(nexus_rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetTmdsSignal failed!\n");
		dResult = DRV_ERR;
		PrintExit();
		return dResult;
	}

	PrintExit();
	return dResult;
#endif
}
void P_HDMI_SetDitheringData(void)
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
	if(dResult != DRV_OK)
	{
		PrintError("DRV_HDMI_GetMonitorName failed!\n");
		PrintExit();
		return;
	}

	PrintDebug("Monitor name is %s.\n", monitorName);

	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

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

	nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetSettings failed! rc = %d\n", nResult);
		PrintExit();
		return;
	}

	PrintExit();

	return;
}

void P_HDMI_SetPauseBurstData(void)
{
	DRV_Error	dResult;
	NEXUS_Error nResult;
	char monitorName[14];
	NEXUS_HdmiOutputSettings hdmiSettings;

	PrintEnter();

	/* Philips TV pop noise & drop noise workaround */
	VK_MEM_Memset(monitorName, 0x0, sizeof(monitorName));

	dResult = DRV_HDMI_GetMonitorName(monitorName);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_HDMI_GetMonitorName failed!\n");
		PrintExit();
		return;
	}

	PrintDebug("[%s] Monitor name is %s.\n", __FUNCTION__, monitorName);

	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

	if(VK_strstr(monitorName, "Philips") != NULL)
	{
		/* Pause burst data */
		hdmiSettings.audioBurstType = NEXUS_SpdifOutputBurstType_ePause;
	}
	else
	{
		/* Zero data */
		hdmiSettings.audioBurstType = NEXUS_SpdifOutputBurstType_eNone;
	}

	nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetSettings failed!\n");
		PrintExit();
		return;
	}
	PrintDebug("[%s] Pause Burst data set to %d !\n", __FUNCTION__, hdmiSettings.audioBurstType);

	PrintExit();

	return;
}

void P_HDMI_SetDefaultColor(void)
{
	NEXUS_DisplayHandle dispHandle_HD, dispHandle_SD;
	NEXUS_DisplaySettings displaySettings;
	HUINT32 ulDeviceId=0;
	DRV_Error nDrvErr = DRV_OK;
#if defined(CONFIG_HDMI_NO_EDID)
#elif (NEXUS_VERSION >= 1403)
	NEXUS_HdmiOutputSettings hdmiSettings;
	NEXUS_Error nResult = NEXUS_SUCCESS;
#endif

	PrintEnter();

	nDrvErr = DRV_VIDEO_GetDisplayHandle(ulDeviceId, &dispHandle_HD, & dispHandle_SD);
	DONE_ERR((nDrvErr!=DRV_OK));
	NEXUS_Display_GetSettings(dispHandle_HD, &displaySettings);

#if defined(CONFIG_HDMI_NO_EDID)
#else
#if (NEXUS_VERSION >= 1502)
	NEXUS_HdmiOutputBasicEdidData hdmiBasicEdidData;
	nResult = NEXUS_HdmiOutput_GetBasicEdidData(g_hdmi_instance.hHdmi, &hdmiBasicEdidData);
	DONE_ERR((nResult!=NEXUS_SUCCESS));

#if defined(WORKAROUND_HDMI_INVALIDMODE_SAMSUNG_UN40HU7000F)
	if ((displaySettings.format == NEXUS_VideoFormat_e3840x2160p50hz || displaySettings.format == NEXUS_VideoFormat_e3840x2160p60hz) &&
		(hdmiBasicEdidData.vendorID[0]==0x4C && hdmiBasicEdidData.vendorID[1]==0x2D) &&
		(hdmiBasicEdidData.productID[0]==0xAE && hdmiBasicEdidData.productID[1]==0x0B) &&
		(hdmiBasicEdidData.serialNum[0]==0x01 && hdmiBasicEdidData.serialNum[1]==0x00 &&
		 hdmiBasicEdidData.serialNum[2]==0x00 && hdmiBasicEdidData.serialNum[3]==0x00))
	{
		/* Samsung UHD, UN40HU7000F(40'). Fix color space to 4:2:0 and depth to 10.
		   in higher clock rate, tv has some problem(invalid mode or no signal).*/
		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
		hdmiSettings.colorDepth = 10;
		nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		DONE_ERR((nResult!=NEXUS_SUCCESS));
	}
	else
#endif
	{
		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eAuto;
		hdmiSettings.colorDepth = 0;
		nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		DONE_ERR((nResult!=NEXUS_SUCCESS));
	}
#elif (NEXUS_VERSION >= 1403)
	if (displaySettings.format== NEXUS_VideoFormat_e3840x2160p60hz || displaySettings.format == NEXUS_VideoFormat_e3840x2160p50hz)
	{
		NEXUS_HdmiOutputEdidData Edid;
		nResult = NEXUS_HdmiOutput_GetEdidData(g_hdmi_instance.hHdmi,&Edid);
		DONE_ERR((nResult!=NEXUS_SUCCESS));

		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

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

		nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		DONE_ERR((nResult!=NEXUS_SUCCESS));
	}
	else
	{
		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
		hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e8bit;
		nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		DONE_ERR((nResult!=NEXUS_SUCCESS));
	}
#endif
#endif

done:
	PrintExit();
	return ;
}

/* refer to nexus example */
void P_HDMI_HotplugCallback(void *pParam, int iParam)
{
	NEXUS_Error	nResult = NEXUS_SUCCESS;
#if defined(CONFIG_HPD_DELAY)
#else
	HdmiMsg_t	HdmiMsg;
	HdmiHotplugState_e eHotplugState = HdmiHotplugState_Disconnected;
#endif
	NEXUS_HdmiOutputStatus stHdmiOutputStatus;
	HBOOL bIsConnected = FALSE;
	HBOOL bRequestedHdmiPowerState = FALSE;
#if defined(WORKAROUND_HDMI_VENDOR_ID)
	NEXUS_HdmiOutputBasicEdidData BasicEdidData;
#endif

#if defined(HDCP_CERTI_MODE)
	{
		NEXUS_HdmiOutputStatus status;
		NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &status);
		if (status.rxPowered)
		{
			P_HDMI_StartHDCP();
		}
	}
	return;
#endif

	PrintEnter();

	LOCK_REQUEST();

	bRequestedHdmiPowerState = g_hdmi_request.bHdmiPowerState;

	UNLOCK_REQUEST();

	if (bRequestedHdmiPowerState == FALSE)
	{
		PrintError("HPD in standby state igronred.\n");
		PrintExit();
		return;
	}

	/*************************
	*	1. Get the current status.
	*************************/

	nResult = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &stHdmiOutputStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus() Failed\n");
		PrintExit();
		return ;
	}

	bIsConnected = (stHdmiOutputStatus.connected && stHdmiOutputStatus.rxPowered) ? TRUE : FALSE;

#if defined(CONFIG_HDOUT_2160P)
	if (bIsConnected)
	{
		P_HDMI_SetDefaultColor();
	}
#endif

#if defined(WORKAROUND_HDMI_VENDOR_ID)
	/*************************
	*	2. Get the VendorID.
	*************************/
	if(bIsConnected == TRUE)
	{
		nResult = NEXUS_HdmiOutput_GetBasicEdidData(g_hdmi_instance.hHdmi, &BasicEdidData);
		if (nResult!=NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_GetBasicEdidData failed!\n");
			PrintExit();
			return;
		}
		VK_MEM_Memcpy(gVendorID, BasicEdidData.vendorID, 2);
	}
#endif

	/*************************
	*	3. Send Msg to the TASK.
	*************************/
	HAPPY(pParam);
	HAPPY(iParam);
#if defined(CONFIG_HPD_DELAY)
	if(bIsConnected == TRUE)
	{
		P_HDMI_SetTimerHotPlugFunction(500,P_HDMI_TCB_HotPlugConnected);
	}
	else
	{
		P_HDMI_SetTimerHotPlugFunction(500,P_HDMI_TCB_HotPlugDisconnected);
	}
#else
	eHotplugState = bIsConnected ? HdmiHotplugState_Connected : HdmiHotplugState_Disconnected;

	PrintDebug("eHotplugState(%d)\n", eHotplugState);

	HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HOTPLUG;
	HdmiMsg.param1 = eHotplugState;

	SEND_MESSAGE(&HdmiMsg);
#endif

	PrintExit();
	return;
}

#if defined(CONFIG_HDCP)
void P_HDMI_HdcpSuccessCallback(void *pContext, int param)
{
	HdmiMsg_t HdmiMsg;
	HAPPY(pContext);
	HAPPY(param);

#if defined(HDCP_CERTI_MODE)
	return;
#endif

	HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP;
	HdmiMsg.param1 = HdcpResult_Success;

	SEND_MESSAGE(&HdmiMsg);

	PrintExit();

	return;
}

void P_HDMI_HdcpFailureCallback(void *pContext, int param)
{
	HdmiMsg_t HdmiMsg;
	NEXUS_Error nResult;

	NEXUS_HdmiOutputHdcpStatus hdcpStatus;

	HAPPY(pContext);
	HAPPY(param);

	PrintEnter();

#if defined(HDCP_CERTI_MODE)
	NEXUS_HdmiOutputStatus status;
	NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &status);
	if (status.rxPowered)
	{
		P_HDMI_StartHDCP();
	}
	return;
#endif

	nResult = NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi_instance.hHdmi, &hdcpStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed!\n");
	}

	PrintEnter();

#if defined(CONFIG_TRAUMA_ERIN)
	if(hdcpStatus.hdcpError != NEXUS_HdmiOutputHdcpError_eSuccess)
	{
		TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_HDCP_NOT_SUPPORT,"GetHdcpStatus = [%d]\n",hdcpStatus.hdcpError);	
	}
#endif

	switch (hdcpStatus.hdcpError)
	{
		case NEXUS_HdmiOutputHdcpError_eSuccess :
/*			PrintError("HDCP Authentication Successful\n");*/
			break;

		case NEXUS_HdmiOutputHdcpError_eRxBksvError :
			PrintError("HDCP Rx BKsv Error\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRxBksvRevoked :
			PrintError("HDCP Rx BKsv/Keyset Revoked\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRxBksvI2cReadError :
		case NEXUS_HdmiOutputHdcpError_eTxAksvI2cWriteError :
			PrintError("HDCP I2C Read Error\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eTxAksvError :
			PrintError("HDCP Tx Aksv Error\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eReceiverAuthenticationError :
			PrintError("HDCP Receiver Authentication Failure\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRepeaterAuthenticationError :
		case NEXUS_HdmiOutputHdcpError_eRepeaterLinkFailure :    /* Repeater Error; unused */
			PrintError("HDCP Repeater Authentication Failure\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRxDevicesExceeded :
			PrintError("HDCP Repeater MAX Downstram Devices Exceeded\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRepeaterDepthExceeded :
			PrintError("HDCP Repeater MAX Downstram Levels Exceeded\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRepeaterFifoNotReady :
			PrintError("Timeout waiting for Repeater\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eRepeaterDeviceCount0 : /* unused */
			break;

		case NEXUS_HdmiOutputHdcpError_eLinkRiFailure :
			PrintError("HDCP Ri Integrity Check Failure\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eLinkPjFailure :
			PrintError("HDCP Pj Integrity Check Failure\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eFifoUnderflow :
		case NEXUS_HdmiOutputHdcpError_eFifoOverflow :
			PrintError("Video configuration issue\n");
			break;

		case NEXUS_HdmiOutputHdcpError_eMultipleAnRequest : /* Should not reach here; but flag if occurs */
			PrintError("Multiple Authentication Request... \n");
			break;
		default :
			PrintError("Unknown HDCP Authentication Error %d\n", hdcpStatus.hdcpError);
			break;
	}


	HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP;
	HdmiMsg.param1 = HdcpResult_Failed;

	SEND_MESSAGE(&HdmiMsg);

	PrintExit();

	return;
}

DRV_Error P_HDMI_StartHDCP(void)
{
	NEXUS_HdmiOutputHdcpSettings *pHdcpSettings = NULL;
	NEXUS_Error nResult;
	DRV_Error dResult;
	HINT32 uiIndexX, uiIndexY, uiCount=0;

	PrintEnter();

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

	NEXUS_HdmiOutput_GetHdcpSettings(g_hdmi_instance.hHdmi, pHdcpSettings);

	/*
	 * 1. Register Callback Functons
	 */
	pHdcpSettings->successCallback.callback = P_HDMI_HdcpSuccessCallback;
	pHdcpSettings->successCallback.context = g_hdmi_instance.hHdmi;

	pHdcpSettings->failureCallback.callback = P_HDMI_HdcpFailureCallback;
	pHdcpSettings->failureCallback.context = g_hdmi_instance.hHdmi;

	pHdcpSettings->transmitEncrypted = true;

	/*
	 * 2. Key Validation Check
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

	if (uiCount != 20)
	{
		PrintError("Invalid Key!! Reading again the HDCP Key from nvram!\n");
		dResult = DRV_NVRAM_GetField(DI_NVRAM_FIELD_HDCP_KEY, 0, s_ucaHdcpKey, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);
		if(dResult != DRV_OK)
		{
			PrintError("DRV_NVRAM_GetField(HDCP_KEY) failed!\n");
			DD_MEM_Free(pHdcpSettings);
			PrintExit();
			return DRV_ERR;
		}
	}

	s_hdcpKeyValid = TRUE;
#endif

	/*
	 * 3. Key Setting
	 */

#if defined(CONFIG_HDCP_22)
	if (s_nHdcp22KeySize < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER))
	{
		dResult = DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, HDCP22_KEY, &s_nHdcp22KeySize);
		if ((dResult != DRV_OK) || (s_nHdcp22KeySize < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER)))
		{
			PrintError("Error : No Hdcp Key\n");
			DD_MEM_Free(pHdcpSettings);
			PrintExit();
			return DRV_ERR;
		}

		if (s_pucHdcp22Key == NULL)
		{
			s_pucHdcp22Key = DD_MEM_Alloc(s_nHdcp22KeySize);
		}

		if (s_pucHdcp22Key != NULL)
		{
			dResult = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, HDCP22_KEY, 0, s_pucHdcp22Key, s_nHdcp22KeySize);
			if (dResult != DRV_OK)
			{
				PrintError("Error : DRV_NVRAM_Read(HDCP22_KEY)\n");
				DD_MEM_Free(s_pucHdcp22Key);
				s_pucHdcp22Key = NULL;
			}
		}
	}

	if((s_pucHdcp22Key != NULL) && !(s_nHdcp22KeySize < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER)))
	{
		nResult = NEXUS_HdmiOutput_SetHdcp2xBinKeys(g_hdmi_instance.hHdmi, s_pucHdcp22Key, s_nHdcp22KeySize);
		if(nResult != NEXUS_SUCCESS)
		{
			PrintError("Error : NEXUS_HdmiOutput_SetHdcp2xBinKeys() - HDCP2.x will not work.\n");
			DD_MEM_Free(pHdcpSettings);
			PrintExit();
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("Error : No Hdcp Key\n");
		DD_MEM_Free(pHdcpSettings);
		PrintExit();
		return DRV_ERR;
	}
#endif
	VK_MEM_Memcpy((void *)pHdcpSettings->encryptedKeySet,
		&s_ucaHdcpKey[DRV_HDMI_HDCPKEY_VERTOR_SIZE+DRV_HDMI_HDCPKEY_DUMMY_SIZE],
		DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE-(DRV_HDMI_HDCPKEY_VERTOR_SIZE+DRV_HDMI_HDCPKEY_DUMMY_SIZE));
	VK_MEM_Memcpy((void *)pHdcpSettings->aksv.data, &s_ucaHdcpKey[0],NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH);

	nResult = NEXUS_HdmiOutput_SetHdcpSettings(g_hdmi_instance.hHdmi, pHdcpSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetHdcpSettings failed!\n");
		DD_MEM_Free(pHdcpSettings);
		PrintExit();
		return DRV_ERR;
	}

	/*
	 * 4. Start Authentication processes
	 */

#if 0
	//block out revoked key process. .
	nResult = NEXUS_HdmiOutput_SetHdcpRevokedKsvs(g_hdmi_instance.hHdmi, (NEXUS_HdmiOutputHdcpKsv*)s_ucaHdcpKey, 40);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetHdcpRevokedKsvs failed!\n");
		DD_MEM_Free(pHdcpSettings);
		PrintExit();
		return DRV_ERR;
	}
#endif

	nResult = NEXUS_HdmiOutput_StartHdcpAuthentication(g_hdmi_instance.hHdmi);
	if(nResult != NEXUS_SUCCESS)
	{
#if defined(CONFIG_TRAUMA_ERIN)		
		TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_HDCP,"NEXUS_HdmiOutput_StartHdcpAuthentication is failed\n");
#endif
		PrintError("NEXUS_HdmiOutput_StartHdcpAuthentication failed!\n");
		DD_MEM_Free(pHdcpSettings);
		PrintExit();
		return DRV_ERR;
	}
	DD_MEM_Free(pHdcpSettings);

	PrintExit();

	return DRV_OK;
}

 DRV_Error P_HDMI_EnableHDCP(void)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	NEXUS_HdmiOutputStatus stHdmiOutputStatus;
	NEXUS_HdmiOutputHdcpStatus hdcpStatus;

	HBOOL bRequestedHdmiPowerState = FALSE;
	HBOOL bRequestedHdcpState = FALSE;
	HBOOL bConnected = FALSE;

 	DRV_Error dResult = DRV_OK;

	PrintEnter();

	dResult = DRV_OK;

 	LOCK_REQUEST();

	bRequestedHdmiPowerState = g_hdmi_request.bHdmiPowerState;
	bRequestedHdcpState = g_hdmi_request.bHdcpEnable;

	UNLOCK_REQUEST();

	/* If middleware doesn't want to enable HDCP, do not enable */
	if (bRequestedHdmiPowerState == FALSE)
		return DRV_OK;

	if (bRequestedHdcpState == FALSE)
		return DRV_OK;

	/* Check attached device's status first */
	CHECK_CONNECTION(stHdmiOutputStatus, bConnected);

	do{
		if(!(bConnected))
		{
			PrintDebug("device is not connected or not powered\n");
			dResult = DRV_ERR;
			break;
		}

		/* check hdcp status */
		rc = NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi_instance.hHdmi, &hdcpStatus);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed!! rc = %d\n", rc);
			dResult = DRV_ERR;
			break;
		}
#if !defined(CONFIG_PRODUCT_IMAGE_FACTORY)
		/* if hdcp succeeded, just set timer to check later and do not start hdcp authentication process */
		if (hdcpStatus.transmittingEncrypted == true)
		{
			PrintDebug("\t\ttransmittingEncrypted is true.\n");
			dResult = DRV_OK;
			break;
		}
#endif
		/* Start HDCP */
		dResult = P_HDMI_StartHDCP();
		if (dResult != DRV_OK)
		{
			PrintError("P_HDMI_StartHDCP() failed!\n");
			dResult = DRV_ERR;
			break;
		}
		g_hdmi_instance.bStartedHdcp = TRUE;
	} while(0);


	PrintExit();

	return dResult;
}

 DRV_Error P_HDMI_DisableHDCP(void)
{
	NEXUS_Error rc = NEXUS_SUCCESS;

	NEXUS_HdmiOutputHdcpStatus hdcpStatus;

	PrintEnter();

    //P_HDMI_UnsetTimerFunction();

    if(g_hdmi_instance.bStartedHdcp == FALSE)
    {
    	PrintExit();
    	return DRV_OK;
    }

    g_hdmi_instance.bStartedHdcp = FALSE;

    rc = NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi_instance.hHdmi, &hdcpStatus);
    if(rc != NEXUS_SUCCESS)
    {
    	PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed!! rc = %d\n", rc);
    	PrintExit();
    	return DRV_ERR;
    }

    /* if hdcp succeeded, just set timer to check later and do not start hdcp authentication process */
    if (hdcpStatus.transmittingEncrypted != true)
    {
    	PrintDebug("\t\ttransmittingEncrypted is true.\n");
    	PrintExit();
    	return DRV_OK;
    }

    rc = NEXUS_HdmiOutput_DisableHdcpAuthentication(g_hdmi_instance.hHdmi);
    if(rc != NEXUS_SUCCESS)
    {
    	PrintError("NEXUS_HdmiOutput_DisableHdcpAuthentication failed!!, rc = %d\n", rc);
    	PrintExit();
    	return DRV_ERR;
    }

	PrintExit();

	return DRV_OK;
}

 DRV_Error P_HDMI_HdcpEventHandler(HdcpResult_e eHdcpResult)
{
	HBOOL bRequestedHdcpState = FALSE;
	HBOOL bConnected = FALSE;
	NEXUS_HdmiOutputStatus stHdmiOutputStatus;

	PrintEnter();

	LOCK_REQUEST();

	bRequestedHdcpState = g_hdmi_request.bHdcpEnable;

	UNLOCK_REQUEST();

	PrintDebug("g_hdmi_request.bHdcpEnable is %s\n", (bRequestedHdcpState == TRUE) ? "TRUE" : "FALSE");

	CHECK_CONNECTION(stHdmiOutputStatus, bConnected);

	/* Ignore all event if hdmi power is off */
	if((bConnected == FALSE)||(bRequestedHdcpState == FALSE))
	{
		PrintDebug("Ignore all event\n");
		return DRV_OK;
	}

	switch(eHdcpResult)
	{
	case HdcpResult_Success:

		PrintError("---------------FIRE-[DH_EVENT_HDCP_SUCCESS]---------------------\n");
		P_HDMI_FireCallbackFunc(DH_EVENT_HDCP_SUCCESS);

		P_HDMI_SetTimerFunction(HDCP_POLLING_SLEEP, P_HDMI_TCB_EnableHdcp);

		/* clear retry count */
		s_hdcpRetryCount = 0;

		break;

	case HdcpResult_Failed :
		/* If middleware doesn't want to enable HDCP, do not retry anymore */
		if (bRequestedHdcpState != TRUE)
			break;

		/* Short retry will do MAX_HDCP_RETRY_COUNT times with interval 100ms */
		if (s_hdcpRetryCount >= MAX_HDCP_RETRY_COUNT)
		{
			P_HDMI_SetTimerFunction(HDCP_POLLING_SLEEP, P_HDMI_TCB_EnableHdcp);
			PrintError("HDCP Retry in long timer(%d)!\n", s_hdcpRetryCount);

			PrintError("---------------FIRE-[DH_EVENT_HDCP_FAIL]------------------------\n");
			P_HDMI_FireCallbackFunc(DH_EVENT_HDCP_FAIL);
			s_hdcpRetryCount = 0;
		}
		else
		{
			P_HDMI_SetTimerFunction(HDCP_SHORT_SLEEP, P_HDMI_TCB_EnableHdcp);
			PrintError("HDCP Retry in short timer(%d)!\n", s_hdcpRetryCount);
		}

		s_hdcpRetryCount++;

		break;


	default :
		PrintError("should not be here!\n");
		break;
	}

	PrintExit();

	return DRV_OK;
}
#endif

DRV_Error P_HDMI_HotplugEventHandler(HdmiHotplugState_e eHotplugStatus)
{
	HdmiMsg_t HdmiMsg;
	DI_HDMI_AVAILABLE_FORMATS stAvailable;
	HBOOL bRequestedHdmiPowerState = FALSE;
	int i=0;
	unsigned char bEdidDone = FALSE;

	PrintEnter();

	LOCK_REQUEST();

	bRequestedHdmiPowerState = g_hdmi_request.bHdmiPowerState;

	UNLOCK_REQUEST();

	PrintError("g_hdmi_request.bHdmiPowerState is %s\n", (bRequestedHdmiPowerState == TRUE) ? "TRUE" : "FALSE");

	/* Ignore all event if hdmi power is off */
	if(bRequestedHdmiPowerState == FALSE)
	{
		PrintError("g_hdmi_request.bHdmiPowerState = %d\n", bRequestedHdmiPowerState);
		return DRV_OK;
	}

	PrintError("eHotplugStatus : %d\n", (HUINT32)eHotplugStatus);

	switch(eHotplugStatus)
	{
	case HdmiHotplugState_Disconnected :

		P_HDMI_UnsetTimerFunction();
		HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_DISABLE_HDCP;

		SEND_MESSAGE(&HdmiMsg);

		P_HDMI_FireCallbackFunc(DH_EVENT_HOTPLUG_DISCONNECT);

		break;

	case HdmiHotplugState_Connected:
		/*
		 * Callback to MW
		 * HDCP On
		 */
		for (i=0; i<MAX_EDID_RETRY_COUNT; i++)
		{
			if (P_HDMI_GetCapability(&stAvailable) == DRV_OK)
			{
				bEdidDone = TRUE;
				break;
			}
			VK_TASK_Sleep(EDID_READ_INTERVAL);
		}

		if (bEdidDone)
		{
			P_HDMI_SetDitheringData();
			P_HDMI_SetPauseBurstData();
		}
		else
		{
			P_HDMI_ForceUpdate();
		}

		if (g_hdmi_instance.pfCallback[DH_EVENT_HOTPLUG_CONNECT])
		{
			P_HDMI_FireCallbackFunc(DH_EVENT_HOTPLUG_CONNECT);		
		}

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
		P_HDMI_SetPreEmphasisConfiguration();
#endif

#if defined(CONFIG_HDCP)
		P_HDMI_SetTimerFunction(HDCP_POLLING_SLEEP, P_HDMI_TCB_EnableHdcp);
#endif
		break;

	default :
		PrintError("should not be here!!\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

void P_HDMI_FireCallbackFunc(DI_HDMI_EVENT eEvent)
{
	PrintEnter();

	VK_INTR_Event(g_hdmi_instance.interruptId, &eEvent);
	switch(eEvent) {
	case DH_EVENT_HOTPLUG_CONNECT:
		PrintError("---------------FIRE-[DH_EVENT_HOTPLUG_CONNECT]------------------\n");
		break;
	case DH_EVENT_HOTPLUG_DISCONNECT:
		PrintError("-------------FIRE-[DH_EVENT_HOTPLUG_DISCONNECT]-----------------\n");
		break;
	default:
		break;
	}

	PrintExit();
	return;
}

void P_HDMI_MakeEvent(HUINT32 uiNum, HdcpResult_e eEvent)
{
	HUINT32 i=0;
	HdmiMsg_t HdmiMsg;

	PrintEnter();

	for(i=0; i<uiNum; i++)
	{
		HdmiMsg.eHdmiMsg = HDMI_MSG_EVT_HDCP;
		HdmiMsg.param1 = eEvent;

		SEND_MESSAGE(&HdmiMsg);
	}

	PrintExit();

	return;
}

DRV_Error P_HDMI_SetColorSpaceInfo(DI_HDMI_AUX_VIDEO_INFO *pstAVInfo)
{
	NEXUS_HdmiOutputSettings hdmiSettings;
	NEXUS_Error nResult = NEXUS_SUCCESS;

	PrintEnter();

	PrintError("pstAVInfo->eAFD : %d\n", pstAVInfo->eAFD);
	PrintError("pstAVInfo->eAR : %d\n", pstAVInfo->eAR);
	PrintError("pstAVInfo->eColorSpace : %d\n", pstAVInfo->eColorSpace);

	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

	switch(pstAVInfo->eColorSpace)
	{
		case DI_HDMI_VIDEO_ColorSpace_eAuto:
			/* if autoColorSpace is true, then whatever color space reported by the HDMI receiver is used (default is true) */
			hdmiSettings.autoColorSpace = TRUE;
			break;

		case DI_HDMI_VIDEO_ColorSpace_eRgb:
		case DI_HDMI_VIDEO_ColorSpace_eYCbCr422:
		case DI_HDMI_VIDEO_ColorSpace_eYCbCr444:

#if (NEXUS_VERSION > 1290) //temporary config to fix compile error under 12.4
			/* if autoColorSpace is false, then this value is used instead of the value reported by the HDMI receiver */
			hdmiSettings.autoColorSpace = FALSE;
			if(pstAVInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eRgb)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eRgb;
			}
	#if (NEXUS_VERSION >= 1402)
			else if(pstAVInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eYCbCr420)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
			}
	#endif
			else if(pstAVInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eYCbCr422)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr422;
			}
			else if(pstAVInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eYCbCr444)
			{
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
			}
			else
			{
	#if (NEXUS_VERSION >= 1402)
				hdmiSettings.colorSpace = NEXUS_ColorSpace_eAuto;
	#else
				hdmiSettings.colorSpace = pstAVInfo->eColorSpace;
	#endif
			}
#else
			hdmiSettings.autoColorSpace = FALSE;
			hdmiSettings.colorSpace = pstAVInfo->eColorSpace;
#endif
			break;
		default:
			break;
	}

	nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
	if (nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetSettings failed!\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error P_HDMI_SetColorSpace(NEXUS_VideoFormat eVideoFormat)
{
#if (NEXUS_VERSION >= 1402)

	NEXUS_HdmiOutputSettings hdmiSettings;
	NEXUS_Error	rc = NEXUS_SUCCESS;

	PrintEnter();

	PrintError("eVideoFormat : 0x%X\n", eVideoFormat);

	if (eVideoFormat == NEXUS_VideoFormat_e3840x2160p60hz || eVideoFormat == NEXUS_VideoFormat_e3840x2160p50hz)
	{
		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eAuto;
		hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_eMax;
		rc = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_HdmiOutput_SetSettings error\n", __func__);
			PrintExit();
			return DRV_ERR;
		}
	}
	else
	{
		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
		hdmiSettings.colorDepth = NEXUS_HdmiColorDepth_e8bit;
		rc = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_HdmiOutput_SetSettings error\n", __func__);
			PrintExit();
			return DRV_ERR;
		}
	}
	PrintExit();
#else
	HAPPY(eVideoFormat);
#endif



	return DRV_OK;
}

DRV_Error P_HDMI_SetSpecificInfo(DI_HDMI_SPECIFIC_INFO_t *pstSpecificInfo)
{

	NEXUS_Error eNexusError = NEXUS_SUCCESS;
	NEXUS_HdmiOutputVendorSpecificInfoFrame vsi;
	NEXUS_HdmiAviInfoFrame AviInfoFrame;
	NEXUS_HdmiOutputSettings hdmiSettings;

	PrintEnter();

	PrintDebug("pstSpecificInfo->eFrame : %d\n", pstSpecificInfo->eFrame);
	PrintDebug("pstSpecificInfo->eAFD : %d\n", pstSpecificInfo->eAFD);
	PrintDebug("pstSpecificInfo->eAR : %d\n", pstSpecificInfo->eAR);
	PrintDebug("pstSpecificInfo->eColorSpace : %d\n", pstSpecificInfo->eColorSpace);

	switch(pstSpecificInfo->eFrame)
	{
		case DI_HDMI_3D_INFO :

		/* set the VSI */
		NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(g_hdmi_instance.hHdmi, &vsi);

		switch(pstSpecificInfo->e3DFormat)
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

		eNexusError = NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(g_hdmi_instance.hHdmi, &vsi);

		if(eNexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] NEXUS_HdmiOutput_SetVendorSpecificInfoFrame error\n", __func__);
			PrintExit();
			return DRV_ERR;
		}

			break;

		case DI_HDMI_COLOR_SPACE_INFO :

			NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

			switch(pstSpecificInfo->eColorSpace)
			{
				case DI_HDMI_VIDEO_ColorSpace_eAuto:
					/* if autoColorSpace is true, then whatever color space reported by the HDMI receiver is used (default is true) */
					hdmiSettings.autoColorSpace = TRUE;
					break;

				case DI_HDMI_VIDEO_ColorSpace_eRgb:
#if (NEXUS_VERSION >= 1402)
				case DI_HDMI_VIDEO_ColorSpace_eYCbCr420:
#endif
				case DI_HDMI_VIDEO_ColorSpace_eYCbCr422:
				case DI_HDMI_VIDEO_ColorSpace_eYCbCr444:

				#if (NEXUS_VERSION >= 1404) //temporary config to fix compile error under 12.4
					/* if autoColorSpace is false, then this value is used instead of the value reported by the HDMI receiver */
					hdmiSettings.autoColorSpace = FALSE;

					if(pstSpecificInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eRgb)
					{
						hdmiSettings.colorSpace = NEXUS_ColorSpace_eRgb;
					}
					else if(pstSpecificInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eYCbCr420)
					{
						hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
					}
					else if(pstSpecificInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eYCbCr422)
					{
						hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr422;
					}
					else if(pstSpecificInfo->eColorSpace == DI_HDMI_VIDEO_ColorSpace_eYCbCr444)
					{
						hdmiSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
					}
					else
					{
						hdmiSettings.colorSpace = NEXUS_ColorSpace_eAuto;
					}
				#else
					hdmiSettings.autoColorSpace = FALSE;
					hdmiSettings.colorSpace = pstSpecificInfo->eColorSpace;
				#endif
					break;
				default :
					break;
			}

			eNexusError = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
			if (eNexusError != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_SetSettings failed!\n");
				return DRV_ERR;
			}

			break;

		case DI_HDMI_AVI_INFO :

				eNexusError = NEXUS_HdmiOutput_GetAviInfoFrame(g_hdmi_instance.hHdmi, &AviInfoFrame);
				if (eNexusError != NEXUS_SUCCESS)
				{
					PrintError("NEXUS_HdmiOutput_GetAviInfoFrame Error \n");
					return DRV_ERR;
				}

				AviInfoFrame.ePictureAspectRatio = pstSpecificInfo->eAR;
				AviInfoFrame.eActiveFormatAspectRatio = pstSpecificInfo->eAFD;
				eNexusError = NEXUS_HdmiOutput_SetAviInfoFrame(g_hdmi_instance.hHdmi, &AviInfoFrame);
				if (eNexusError != NEXUS_SUCCESS)
				{
						PrintError("NEXUS_HdmiOutput_SetAviInfoFrame Error \n");
						return DRV_ERR;
				}

			break ;

	}

	PrintExit();
	return DRV_OK;
}

DRV_Error P_HDMI_AddHdmiOutput(void)
{
	NEXUS_Error nexus_rc;
	DRV_Error	dResult = DRV_OK;
#if defined(CONFIG_AUDIO_DECODER)
	HBOOL		needToStopAudio = FALSE;
#if defined(WORKAROUND_HDMI_POPNOISE_SHARP_LC_32GD7)
	char monitorName[14];
	NEXUS_HdmiOutputSettings hdmiSettings;
#endif
#endif

	PrintEnter();

#if (NEXUS_VERSION < 1290)
	nexus_rc = NEXUS_HdmiOutput_SetPowerState( g_hdmi_instance.hHdmi, TRUE);
	DONE_ERR((nexus_rc!=NEXUS_SUCCESS));

	nexus_rc = NEXUS_HdmiOutput_SetTmdsSignal(g_hdmi_instance.hHdmi, TRUE);
	DONE_ERR((nexus_rc!=NEXUS_SUCCESS));
#endif

#if defined(CONFIG_AUDIO_DECODER)
	/* Add audio decoder first */
#if defined(WORKAROUND_HDMI_POPNOISE_SHARP_LC_32GD7)
	/* Sharp TV pop noise workaround */
	VK_MEM_Memset(monitorName, 0x0, sizeof(monitorName));
	dResult = DRV_HDMI_GetMonitorName(monitorName);
	DONE_ERR((dResult!=DRV_OK));

	PrintDebug("Monitor name is %s.\n", monitorName);

	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
	if(!VK_strstr(monitorName, "SHARP"))
	{
		dResult = DRV_AUDIO_PlaybackDummyToHDMI(TRUE);
	}
#else
	dResult = DRV_AUDIO_PlaybackDummyToHDMI(TRUE);
#endif
	if (dResult == DRV_OK)
	{
		needToStopAudio = TRUE;
	}
#endif	/* CONFIG_AUDIO_DECODER */

	/* Add display */
	nexus_rc = NEXUS_Display_AddOutput(g_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(g_hdmi_instance.hHdmi));
	switch (nexus_rc)
	{
	case NEXUS_SUCCESS:
		break;
#if (NEXUS_VERSION >= 1290)
	case NEXUS_DISPLAY_ADD_OUTPUT_ALREADY_CONNECTED:
		PrintError("Display Output Already Connected!!\n");
		break;
#endif
	default:
		DONE_ERR((0));
	}

#if defined(HDCP_CERTI_MODE)
#else
#if defined(CONFIG_HDCP)
	/* Start HDCP after sleep */
	P_HDMI_SetTimerFunction(HDCP_POLLING_SLEEP, P_HDMI_TCB_EnableHdcp);
#endif//CONFIG_HDCP
#endif//HDCP_CERTI_MODE
#if defined(CONFIG_AUDIO_DECODER)
	if (needToStopAudio == TRUE)
	{
		dResult = DRV_AUDIO_PlaybackDummyToHDMI(FALSE);
		DONE_ERR((dResult!=DRV_OK));
	}
#endif

	dResult = DRV_OK;
done:
	PrintExit();
	return dResult;
}

DRV_Error P_HDMI_RemoveHdmiOutput(void)
{
	NEXUS_Error nexus_rc;
	DRV_Error dResult = DRV_ERR;

	PrintEnter();

#if defined(CONFIG_HDCP)
	P_HDMI_UnsetTimerFunction();

	dResult = P_HDMI_DisableHDCP();
	if(dResult!= DRV_OK)
	{
		PrintError("[%s]P_HDMI_DisableHDCP error 0x%x \n",__func__,dResult);
	}
#endif

#if (NEXUS_VERSION < 1290)
	nexus_rc = NEXUS_HdmiOutput_SetPowerState( g_hdmi_instance.hHdmi, FALSE);
	DONE_ERR((nexus_rc!=NEXUS_SUCCESS));

	nexus_rc = NEXUS_HdmiOutput_SetTmdsSignal(g_hdmi_instance.hHdmi, FALSE);
	DONE_ERR((nexus_rc!=NEXUS_SUCCESS));
#endif

	nexus_rc = NEXUS_Display_RemoveOutput(g_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(g_hdmi_instance.hHdmi));
	DONE_ERR((nexus_rc!=NEXUS_SUCCESS));

	dResult = DRV_OK;
done:
	PrintExit();
	return dResult;
}

DRV_Error P_HDMI_AddRemove(HBOOL enable)
{
	NEXUS_Error nexus_rc;
	DRV_Error dResult = DRV_ERR;

	PrintEnter();

	if(enable)
	{
		nexus_rc = NEXUS_Display_AddOutput(g_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(g_hdmi_instance.hHdmi));
		if(nexus_rc != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Display_RemoveOutput() failed!\n");
			DONE_ERR((nexus_rc!=NEXUS_SUCCESS));
		}
	}
	else
	{
		nexus_rc = NEXUS_Display_RemoveOutput(g_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(g_hdmi_instance.hHdmi));
		if(nexus_rc != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Display_RemoveOutput() failed!\n");
			DONE_ERR((nexus_rc!=NEXUS_SUCCESS));
		}
	}

done:
	PrintExit();
	return dResult;

}

void P_HDMI_PowerCheckTimer(HULONG ulTimerId, void *pvParam)
{
	HdmiMsg_t HdmiMsg;
	HBOOL bConnected = FALSE;
	static HUINT32 	s_unCount =0;
	NEXUS_HdmiOutputStatus stHdmiOutputStatus;

	PrintEnter();

	CHECK_CONNECTION(stHdmiOutputStatus, bConnected); 
	
	if(bConnected == FALSE)
	{
		PrintError("[%s](%ld)bIsConnected :%d s_unCount[%d]\n",__FUNCTION__,s_ulCheckHdmiPwrTimer,bConnected,s_unCount );
				
		if(s_unCount > MAX_POWER_RETRY_COUNT)
		{
			PrintDebug(" Max Count, Timer Cancel...\n\n\n\n");
		}
		else
		{
			s_unCount++;			
			PrintExit();
			return;
		}
	}

	if(s_unCount)
	{
		HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_REMOVE_HDMI_OUTPUT;
		SEND_MESSAGE(&HdmiMsg);
		
		HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_ADD_HDMI_OUTPUT;
		SEND_MESSAGE(&HdmiMsg);		
	}

	if(s_ulCheckHdmiPwrTimer != 0)
	{
		VK_TIMER_Cancel(s_ulCheckHdmiPwrTimer);
		s_ulCheckHdmiPwrTimer = 0;
	}

	s_unCount = 0;
	
	PrintExit();
}

#if defined(CONFIG_HDCP_22_PROVISIONING) && defined(CONFIG_BCM_SAGE_AUTO_FW) && defined(CONFIG_DEBUG)
DRV_Error P_HDMI_GenHDCP22AndReboot(void)
{
	FILE *sage_flag_fd = NULL;
	HUINT8 *output = NULL;
	DI_ERR_CODE rc = DI_ERR_ERROR;
	HINT32 ret = 0;
#if defined(CONFIG_EMMC_FLASH)
	HUINT8 s_szCommandString[128] = {0, };
#endif
	sage_flag_fd = fopen(DRV_HDMI_SAGE_FW_FLAG_FILE, "r");
	if (sage_flag_fd != NULL)
	{
		PrintError("\n\n This system will generate HDCP22 KEY and reboot!!! \n\n");

		output = DD_MEM_Alloc(sizeof(s_ucaUnboundedHdcp22Key));
		if (output == NULL)
		{
			PrintError("[%s:%d] mem allocation failed!!!!\n", __FUNCTION__, __LINE__);
			fclose(sage_flag_fd);
			return DI_ERR_ERROR;
		}

		rc = DI_HDMI_HDCP22KeyProvisioning(s_ucaUnboundedHdcp22Key, sizeof(s_ucaUnboundedHdcp22Key), output);
		if (rc != DI_ERR_OK)
		{
			PrintError("[%s:%d] hdcp2.2 key Provisioning fail!!!! \n", __FUNCTION__, __LINE__);
			fclose(sage_flag_fd);
			return DI_ERR_ERROR;
		}

#if defined(CONFIG_EMMC_FLASH)
		VK_strncpy(s_szCommandString, "echo 0 > /sys/block/mmcblk0/mmcblk0boot1/force_ro", sizeof(s_szCommandString));
		VK_SYSTEM_Command(s_szCommandString);
#endif
		rc = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, HDCP22_KEY, 0, output, sizeof(s_ucaUnboundedHdcp22Key));
		if (rc != DI_ERR_OK)
		{
			PrintError("[%s:%d] fail to write hdcp2.2 key to nvram!!!! \n", __FUNCTION__, __LINE__);
			fclose(sage_flag_fd);
			return DI_ERR_ERROR;
		}
#if defined(CONFIG_EMMC_FLASH)
		VK_strncpy(s_szCommandString, "echo 1 > /sys/block/mmcblk0/mmcblk0boot1/force_ro", sizeof(s_szCommandString));
		VK_SYSTEM_Command(s_szCommandString);
#endif

		ret = remove(DRV_HDMI_SAGE_FW_FLAG_FILE);
		if(ret < 0)
		{
			PrintError("[%s:%d] fail to remove a file!!!! \n", __FUNCTION__, __LINE__);
			fclose(sage_flag_fd);
			return DI_ERR_ERROR;
		}

		fclose(sage_flag_fd);
		DI_POWER_SetMainChipReset();
	}

	return DI_ERR_ERROR;
}
#endif
#if __________task__________
#endif

void P_HDMI_Task(void *pvParam)
{
	HdmiMsg_t HdmiMsg;
	int	vkMsg;
	DRV_Error dResult;
	NEXUS_HdmiOutputStatus hdmiOutputStatus;
	NEXUS_Error nexus_rc;
#if defined(CONFIG_HDCP)
	NEXUS_HdmiOutputHdcpSettings hdcpSettings;
#endif

	HAPPY(pvParam);
	HAPPY(strHdmiMsg);

	/*	NOTICE!!
	*	Letting MW know that the event occured makes DI_HDMI_EnableOutput function called.
	*	So If DI_HDMI_EnableOutput is called already, Should I give any notice to MW about that event?
	*/
	while (1)
	{
		vkMsg = VK_MSG_Receive(ulHdmiMsgQueueId, &HdmiMsg, sizeof(HdmiMsg_t));
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
		PrintDebug(PARAM_COLOR_ON"P_HDMI_TASK : %s\n"PARAM_COLOR_OFF, strHdmiMsg[HdmiMsg.eHdmiMsg]);
#endif

#if 0
		LOCK_REQUEST();

		PrintDebug(PARAM_COLOR_ON"\tg_hdmi_request.bHdcpEnable : %d\n"PARAM_COLOR_OFF, g_hdmi_request.bHdcpEnable);
		PrintDebug(PARAM_COLOR_ON"\tg_hdmi_request.bHdmiEnable : %d\n"PARAM_COLOR_OFF, g_hdmi_request.bHdmiEnable);
		PrintDebug(PARAM_COLOR_ON"\tg_hdmi_request.bHdmiPowerState : %d\n"PARAM_COLOR_OFF, g_hdmi_request.bHdmiPowerState);

		UNLOCK_REQUEST();
#endif

		switch (HdmiMsg.eHdmiMsg)
		{
		case HDMI_MSG_EVT_HOTPLUG :
			dResult = P_HDMI_HotplugEventHandler((HdmiHotplugState_e)HdmiMsg.param1);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_HotplugHandler failed!\n");
			}
			break;

#if defined(CONFIG_HDCP)
		case HDMI_MSG_EVT_HDCP :
			dResult = P_HDMI_HdcpEventHandler((HdcpResult_e)HdmiMsg.param1);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_HdcpHandler failed!\n");
			}
			break;
#endif

		case HDMI_MSG_CMD_SET_COLORSPACE_INFO :
			dResult = P_HDMI_SetColorSpaceInfo((DI_HDMI_AUX_VIDEO_INFO *)HdmiMsg.param1);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetColorSpaceInfo failed!\n");
			}
			break;

		case HDMI_MSG_CMD_SET_COLORSPACE :
			dResult = P_HDMI_SetColorSpace((NEXUS_VideoFormat)HdmiMsg.param1);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetColorSpace failed!\n");
			}
			break;

		case HDMI_MSG_CMD_SET_SPECIFIC_INFO :
			dResult = P_HDMI_SetSpecificInfo((DI_HDMI_SPECIFIC_INFO_t *)HdmiMsg.param1);
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetSpecificInfo failed!\n");
			}
			break;

		case HDMI_MSG_CMD_ADD_HDMI_OUTPUT:
			dResult = P_HDMI_AddHdmiOutput();
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_AddHdmiOutput() failed!\n");
			}
			break;

		case HDMI_MSG_CMD_REMOVE_HDMI_OUTPUT:
			
			dResult = P_HDMI_RemoveHdmiOutput();
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_RemoveHdmiOutput() failed!\n");
			}
			break;

		case HDMI_MSG_CMD_CHECK_HDMI_POWER:

			(void)VK_TIMER_EventEvery(POWER_RETRY_TIME, P_HDMI_PowerCheckTimer ,NULL,0,&s_ulCheckHdmiPwrTimer);
			
			break;

		case HDMI_MSG_CMD_SET_PREEMPHASIS_CONFIGURATION:
			dResult = P_HDMI_SetPreEmphasisConfiguration();
			if (dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetPreEmphasisConfiguration failed!\n");
			}
			break;

#if defined(CONFIG_HDCP)
		case HDMI_MSG_CMD_ENABLE_HDCP :
			dResult = P_HDMI_EnableHDCP();
			if(dResult != DRV_OK)
			{
				PrintDebug("P_HDMI_EnableHDCP() failed!\n");
				P_HDMI_SetTimerFunction(HDCP_SHORT_SLEEP, P_HDMI_TCB_EnableHdcp);
				P_HDMI_HdcpEventHandler(HdcpResult_Failed);

			}
			else
			{
				P_HDMI_SetTimerFunction(HDCP_POLLING_SLEEP, P_HDMI_TCB_EnableHdcp);
			}
			break;

		case HDMI_MSG_CMD_DISABLE_HDCP :
			dResult = P_HDMI_DisableHDCP();
			if(dResult != DRV_OK)
			{
				PrintError("P_HDMI_DisableHDCP() failed!\n");
			}
			break;
#endif

		case HDMI_MSG_CMD_SET_AV_MUTE :
			dResult = P_HDMI_SetAVMute((HBOOL)HdmiMsg.param1);
			if(dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetAVMute(%d) failed!\n", (HUINT32)HdmiMsg.param1);
			}
			break;

		case HDMI_MSG_CMD_SET_TMDS_SIGNAL :
			dResult = P_HDMI_SetTmds((HBOOL)HdmiMsg.param1);
			if(dResult != DRV_OK)
			{
				PrintError("P_HDMI_SetTmds(%d) failed!\n", (HUINT32)HdmiMsg.param1);
			}
			break;

		case HDMI_MSG_CMD_SET_RESOLUTION :
			nexus_rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &hdmiOutputStatus);
			if(nexus_rc != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_GetStatus failed!\n");
				hdmiOutputStatus.connected = FALSE;
			}

			if(hdmiOutputStatus.connected == FALSE)
			{
				PrintError("HDMI is not ready.\n");
				break;
			}

#if defined(CONFIG_HDCP)
			/* Nexus 15.2 occurs hdcp fail event when disabling hdcp.
			   If that, driver code will retry to enable hdcp in callback function and
			   some tv occurs unexpected behavior. So, ignore callback function.
			   TV model : LG 49UF850V */
			NEXUS_HdmiOutput_GetHdcpSettings(g_hdmi_instance.hHdmi, &hdcpSettings);
			hdcpSettings.successCallback.callback = NULL;
			hdcpSettings.failureCallback.callback = NULL;
			nexus_rc = NEXUS_HdmiOutput_SetHdcpSettings(g_hdmi_instance.hHdmi, &hdcpSettings);
			if(nexus_rc != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_SetHdcpSettings failed!\n");
				break;
			}

			dResult = P_HDMI_DisableHDCP();
			if(dResult != DRV_OK)
			{
				PrintError("P_HDMI_DisableHDCP() failed!\n");
			}
#endif

			dResult = DRV_VIDEO_SetDisplayStandard((DI_VIDEO_HD_RESOLUTION)HdmiMsg.param1);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_VIDEO_SetDisplayStandard(%d) failed!\n", (HUINT32)HdmiMsg.param1);
			}

#if defined(CONFIG_HDCP)
			NEXUS_HdmiOutput_GetHdcpSettings(g_hdmi_instance.hHdmi, &hdcpSettings);
			hdcpSettings.successCallback.callback = P_HDMI_HdcpSuccessCallback;
			hdcpSettings.failureCallback.callback = P_HDMI_HdcpFailureCallback;
			nexus_rc = NEXUS_HdmiOutput_SetHdcpSettings(g_hdmi_instance.hHdmi, &hdcpSettings);
			if(nexus_rc != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_HdmiOutput_SetHdcpSettings failed!\n");
				break;
			}

			if (g_hdmi_request.bHdcpEnable)
			{
				/* compatibility issue with 'LG 49UF850V'.
				   enable HDCP again after HDCP_RESOLUTION_SLEEP */
				P_HDMI_SetTimerFunction(HDCP_RESOLUTION_SLEEP, P_HDMI_TCB_EnableHdcp);
			}
#endif

			break;

		default :
			PrintError("[P_HDMI_Task] Unknown Msaage : %d\n", HdmiMsg.eHdmiMsg);
			break;
		}
	}
}


void P_HDMI_AgingTask(void *pvParam)
{
	DRV_Error res = DRV_OK;
	HBOOL bMute = FALSE;
	HBOOL bConnect = FALSE;
	HBOOL bOnOff = FALSE;
	HAPPY(pvParam);

	while(1)
	{
		VK_TASK_Sleep(10000);

		if(s_bMuteAging)
		{
			bMute = bMute ? FALSE : TRUE;

			res = DRV_HDMI_SetAVMute(bMute);
			if(res != DRV_OK)
			{
				PrintError("DRV_HDMI_SetAVMute() returned ERROR!\n");
			}
		}

		if(s_bDisplayAging)
		{
			bConnect = bConnect ? FALSE : TRUE;

			res = DRV_HDMI_SetDisplayOutput(bConnect);
			if(res != DRV_OK)
			{
				PrintError("DRV_HDMI_SetDisplayOutput() returned ERROR!\n");
			}
		}

		if(s_bStandbyAging)
		{
			bOnOff = bOnOff ? FALSE : TRUE;

			res = DI_HDMI_SetPowerState(bOnOff);
			if(res != DRV_OK)
			{
				PrintError("DRV_HDMI_SetDisplayOutput() returned ERROR!\n");
			}

		}
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

	if (g_hdmi_instance.pfCallback[*pEvent] == NULL)
	{
		PrintError("HDMI CALLBACK %d not registered!\n", *pEvent );
		PrintExit();
		return;
	}

	PrintDebug("HDMI CALLBACK %d Fired!\n", *pEvent );

	localEvent = *pEvent;

	/* FIRE */
	(*g_hdmi_instance.pfCallback[*pEvent])(&localEvent);

	PrintExit();

	return;
}


#if __________drv___________
#endif

/////////////////////////////////////////////////////////////////////////////////////////

DRV_Error DRV_HDMI_Init(void)
{
	int i = 0;
	DI_ERR_CODE	rc = DI_ERR_OK;
	DRV_Error eDrvErr = DRV_OK;
	int res = 0;
	NEXUS_Error nexus_rc = NEXUS_SUCCESS;
	unsigned char ucSemName[20];
	NEXUS_DisplayHandle hDisplaySd;
	NEXUS_HdmiOutputSettings hdmiSettings;

	/* Get HDMI Handle */
	NEXUS_PlatformConfiguration platformConfig;

	PrintEnter();

	PrintError("New HDMI Driver\n");

	NEXUS_Platform_GetConfiguration(&platformConfig);

	g_hdmi_instance.hHdmi = platformConfig.outputs.hdmi[0];
#if defined(CONFIG_HDMI_CEC)
	g_hdmi_instance.hCec = platformConfig.outputs.cec[0];
#endif

	/* Register Callback */
	rc = VK_INTR_Register((void (*)(void *))P_HDMI_Intr, "hdmi_hotplug", sizeof(DI_HDMI_EVENT), &g_hdmi_instance.interruptId, VK_INTR_TASK0);
	if (rc!=VK_OK)
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return DI_ERR_ERROR;
	}

	/* Make callback function NULL */
	for (i=0; i<DH_EVENT_MAX; i++)
	{
		g_hdmi_instance.pfCallback[i] = NULL;
	}

	/* Semaphore */
	VK_memset(ucSemName, 0x00, 20);
	VK_snprintf((char*)ucSemName, 16, "di_hdmi_di");
	rc = VK_SEM_Create(&g_hdmi_instance.ulSemId, (const char*)ucSemName, DEFAULT_SUSPEND_TYPE);
	if (rc!=VK_OK)
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return DI_ERR_ERROR;
	}

	VK_memset(ucSemName, 0x00, 20);
	VK_snprintf((char*)ucSemName, 16, "g_hdmi_request");
	rc = VK_SEM_Create(&s_ulHdmiRequestSemId, (const char*)ucSemName, DEFAULT_SUSPEND_TYPE);
	if (rc!=VK_OK)
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return DI_ERR_ERROR;
	}

	/* Get display handle */
	rc = DRV_VIDEO_GetDisplayHandle(0, &g_hdmi_instance.hDisplayHd, &hDisplaySd);
	if (rc)
	{
		PrintError("[%s] DRV_VIDEO_GetDisplayHandle error!!\n", __func__);
		return DI_ERR_ERROR;
	}

	/* Install hotplug callback -- video only for now */
	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

#if defined(CONFIG_DIRECTFB)
	/*
	  * If directfb is used, directfb register it's callback function prior to DI_HDMI.
	  * So, to use our callback function, cancel directfb's setting here.
	  */

	PrintError("[%s] Check callback!!\n", __func__);
	if(hdmiSettings.hotplugCallback.callback != NULL || hdmiSettings.cecCallback.callback != NULL)
	{
		PrintError("[%s] Unregister callback!!\n", __func__);

		hdmiSettings.hotplugCallback.callback = NULL;
		hdmiSettings.hotplugCallback.context = NULL;

		hdmiSettings.cecCallback.callback = NULL;
		hdmiSettings.cecCallback.context = NULL;

		nexus_rc = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
		if (nexus_rc)
		{
			PrintError("[%s] NEXUS_HdmiOutput_SetSettings error!!\n", __func__);
			return DI_ERR_ERROR;
		}
	}
#endif

#if defined(CONFIG_HDMI_NO_EDID)
	hdmiSettings.hotplugCallback.callback = NULL;
#else
	hdmiSettings.hotplugCallback.callback = P_HDMI_HotplugCallback;
#endif
	hdmiSettings.hotplugCallback.context = g_hdmi_instance.hHdmi;
	hdmiSettings.hotplugCallback.param = (int)g_hdmi_instance.hDisplayHd;

	hdmiSettings.preFormatChangeAvMuteDelay = 50;
	hdmiSettings.postFormatChangeAvMuteDelay = 150;

	nexus_rc = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_HdmiOutput_SetSettings error!!\n", __func__);
		return DI_ERR_ERROR;
	}

	#if defined(CONFIG_HDMI_CEC)
	DI_HDMI_CEC_Init();
	#endif

	/* Get HDCP Key */
	eDrvErr = DRV_NVRAM_GetField(DI_NVRAM_FIELD_HDCP_KEY, 0, s_ucaHdcpKey, DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE);
	if (eDrvErr != DRV_OK)
	{
#if defined(CONFIG_TRAUMA_ERIN)		
		TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_HDCP_NOT_FOUND,"Getting DI_NVRAM_FIELD_HDCP_KEY is failed \n");
#endif
		PrintError("[%s] DRV_NVRAM_GetFild(HDCP_KEY) error!!\n", __func__);
	}

#if defined(CONFIG_HDCP_22_PROVISIONING) && defined(CONFIG_BCM_SAGE_AUTO_FW) && defined(CONFIG_DEBUG) && !(CONFIG_PRODUCT_IMAGE_FACTORY)
	rc = P_HDMI_GenHDCP22AndReboot();
#endif

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
#if defined(CONFIG_TRAUMA_ERIN)					
					TxERIN_Error(ERIN_ERRCODE_DD_DISPLAY_HDCP_NOT_FOUND,"Getting HDCP22_KEY is failed \n");
#endif
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

	/* Event */
	res = VK_EVENT_Create(&g_ulHdmiSyncCallEventId, "HdmiSyncEvent");
	if(res != VK_OK)
	{
		PrintError("[%s] Creating Hdmi VK_EVENT_Create failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	/* Message queue */
	res = VK_MSG_Create(10, sizeof(HdmiMsg_t),"HdmiMsgQueue",&ulHdmiMsgQueueId,VK_SUSPENDTYPE_FIFO);
	if(res != VK_OK)
	{
		PrintError("[%s] Creating Hdmi Msg Queue failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	/* HDMI Task */
	res = VK_TASK_Create(P_HDMI_Task, HDMI_HDCP_TASK_PRIORITY, HDMI_HDCP_TASK_STACK_SIZE, "HdmiTask", (void *)0, &s_ulHdmiTaskId, FALSE);
	if(res != VK_OK)
	{
		PrintError("[%s] Creating HdmiTask failed!\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}

	res = VK_TASK_Start(s_ulHdmiTaskId);
	if(res != VK_OK)
	{
		PrintError("[%s] Starting HdmiTask failed!\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_DEBUG)
	res = VK_TASK_Create(P_HDMI_AgingTask, HDMI_HDCP_TASK_PRIORITY, HDMI_HDCP_TASK_STACK_SIZE, "HdmiAgingTask", (void *)0, &s_ulHdmiAgingTaskId, FALSE);
	if(res != VK_OK)
	{
		PrintError("[%s] Creating HdmiAgingTask failed!\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}

	res = VK_TASK_Start(s_ulHdmiAgingTaskId);
	if(res != VK_OK)
	{
		PrintError("[%s] Starting HdmiAgingTask failed!\n",__FUNCTION__);
		return DI_ERR_ERROR;
	}
#endif

	g_hdmi_instance.bInitialized = TRUE;
	g_hdmi_instance.bStartedHdcp = FALSE;

#if defined(CONFIG_HDMI_NO_EDID)
#else
	P_HDMI_HotplugCallback(g_hdmi_instance.hHdmi, (int)g_hdmi_instance.hDisplayHd);
#endif

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
	eDrvErr = DRV_HDMI_SetPreEmphasisConfiguration();
	if (eDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_HDMI_SetPreEmphasisConfiguration error!!\n", __func__);
	}
#endif

	PrintExit();
	return DI_ERR_OK;
}

void DRV_HDMI_UnInit(void)
{
	DI_ERR_CODE	rc=DI_ERR_OK;
	NEXUS_Error nexus_rc;
#if defined(CONFIG_HDMI_CEC)
	NEXUS_CecSettings cecSettings;
#endif

	PrintEnter();

#if defined(CONFIG_HDMI_CEC)
	NEXUS_Cec_GetSettings(g_hdmi_instance.hCec, &cecSettings);
	cecSettings.enabled = false;
	nexus_rc = NEXUS_Cec_SetSettings(g_hdmi_instance.hCec, &cecSettings);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_Cec_SetSettings error!!\n", __func__);
	}
#endif

	rc = VK_TIMER_Cancel(s_ulTimerHandle);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TIMER_Cancel (s_ulTimerHandle) failed!\n", __FUNCTION__);
	}
	s_ulTimerHandle=0;

	/* disable audio/video output */
	P_HDMI_RemoveHdmiOutput();

	nexus_rc = NEXUS_Display_RemoveOutput(g_hdmi_instance.hDisplayHd, NEXUS_HdmiOutput_GetVideoConnector(g_hdmi_instance.hHdmi));
	if(nexus_rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Display_RemoveOutput() failed!\n");
		return ;
	}

	NEXUS_VideoOutput_Shutdown(NEXUS_HdmiOutput_GetVideoConnector(g_hdmi_instance.hHdmi));

	rc = VK_TASK_Stop(s_ulHdmiTaskId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TASK_Stop failed!\n", __FUNCTION__);
	}

	rc = VK_TASK_Destroy(s_ulHdmiTaskId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_TASK_Destroy failed!\n", __FUNCTION__);
	}

	rc = VK_MSG_Destroy(ulHdmiMsgQueueId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_MSG_Destory failed!\n", __FUNCTION__);
	}

	rc = VK_EVENT_Destroy(g_ulHdmiSyncCallEventId);
	if (rc!=DI_ERR_OK)
	{
		PrintError("[%s] VK_EVENT_Destory failed!\n", __FUNCTION__);
	}

	PrintExit();

	return;
}

DRV_Error DRV_HDMI_DumpRawEdid(void)
{
	PrintEnter();

	CHECK_INIT();

	DI_UART_Dump((unsigned char*)&(g_hdmi_instance.stEdidBlock), sizeof(DI_HDMI_EDID_RAW), 16);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_CheckSupportedResolution(NEXUS_VideoFormat eVideoFormat, HBOOL *pbSupported)
{
	NEXUS_Error	nexus_rc;
	NEXUS_HdmiOutputStatus hdmiStatus;

	PrintEnter();

	LOCK_HDMI();

	nexus_rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &hdmiStatus) ;
	if (nexus_rc != NEXUS_SUCCESS)
	{
		UNLOCK_HDMI();
		PrintError("NEXUS_HdmiOutput_GetStatus failed\n");
		PrintExit();
		return DRV_ERR;
	}

	if(hdmiStatus.videoFormatSupported[eVideoFormat] != TRUE)
	{
		PrintError("Not Supported Tv Resolution[%d] \n",eVideoFormat);
		*pbSupported = FALSE;
	}
	else
	{
		PrintError("Supported Tv Resolution[%d] \n",eVideoFormat);
		*pbSupported = TRUE;
	}

	UNLOCK_HDMI();

	PrintExit();

	return DRV_OK;
}

void DRV_HDMI_ConverHdVideoFormatToNvramVideoFormat(DI_VIDEO_HD_RESOLUTION eHdFormat,DI_NVRAM_VIDEO_RESOLUTION_E *peNvramHdFormat)
{

	PrintEnter();

	switch (eHdFormat)
	{
		case DI_VIDEO_HD_RESOLUTION_480I:
		case DI_VIDEO_HD_RESOLUTION_480I_J:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_720x480I; break;
		case DI_VIDEO_HD_RESOLUTION_480P:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_720x480P; break;
		case DI_VIDEO_HD_RESOLUTION_576I:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_720x576I; break;
		case DI_VIDEO_HD_RESOLUTION_576P:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_720x576P ; break;
		case DI_VIDEO_HD_RESOLUTION_720P:
		case DI_VIDEO_HD_RESOLUTION_720P_50HZ:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_1280x720P; break;
		case DI_VIDEO_HD_RESOLUTION_1080I:
		case DI_VIDEO_HD_RESOLUTION_1080I_50HZ:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_1920x1080I; break;
		case DI_VIDEO_HD_RESOLUTION_1080P:
		case DI_VIDEO_HD_RESOLUTION_1080P_24HZ:
		case DI_VIDEO_HD_RESOLUTION_1080P_25HZ:
		case DI_VIDEO_HD_RESOLUTION_1080P_30HZ:
		case DI_VIDEO_HD_RESOLUTION_1080P_50HZ:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_1920x1080P; break;
		case DI_VIDEO_HD_RESOLUTION_2160P:
		case DI_VIDEO_HD_RESOLUTION_2160P_24HZ:
		case DI_VIDEO_HD_RESOLUTION_2160P_25HZ:
		case DI_VIDEO_HD_RESOLUTION_2160P_30HZ:
		case DI_VIDEO_HD_RESOLUTION_2160P_50HZ:
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_3840x2160P; break;
		default:
			PrintError("[%s]Hd video format covert is not supported \n",__func__);
			PrintError("[%s]default setting -> 1920x1080I \n",__func__);
			*peNvramHdFormat = DI_NVRAM_RESOLUTION_1920x1080I;
			break;

	}

	PrintExit();

	return ;
}

#if defined(CONFIG_HDOUT_2160P)
DRV_Error DRV_HDMI_SetColorInfo(DI_VIDEO_HD_RESOLUTION eFormat)
{
	NEXUS_Error nResult;
	NEXUS_HdmiOutputSettings nSettings;
	NEXUS_HdmiOutputEdidVideoFormatSupport stRequestedVideoFormatSupport  ;
	NEXUS_HdmiOutputEdidData edid ;

	HAPPY(eFormat);
	HAPPY(nResult);
	HAPPY(nSettings);
	HAPPY(stRequestedVideoFormatSupport);
	HAPPY(edid);

	PrintEnter();
	LOCK_HDMI();

#if (NEXUS_VERSION >= 1502)
	P_HDMI_SetDefaultColor();
	goto done;
#else
	if((eFormat == DI_VIDEO_HD_RESOLUTION_2160P_50HZ) || (eFormat == DI_VIDEO_HD_RESOLUTION_2160P))
	{
	    /* Get the supported features of the requested format (colorspace, 3d support, etc.) */
	    nResult = NEXUS_HdmiOutput_GetVideoFormatSupport(g_hdmi_instance.hHdmi,
	        DRV_VIDEO_GetFormatType(0,eFormat), &stRequestedVideoFormatSupport) ;
		DONE_ERR((nResult != NEXUS_SUCCESS));

		PrintDebug("Video Format Type = %d\n",DRV_VIDEO_GetFormatType(0,eFormat));
		PrintDebug("Support yCbCr420 = %d\n",stRequestedVideoFormatSupport.yCbCr420);
		PrintDebug("Support yCbCr422 = %d\n",stRequestedVideoFormatSupport.yCbCr422);
		PrintDebug("Support yCbCr444rgb444 = %d\n",stRequestedVideoFormatSupport.yCbCr444rgb444);

	    /* Get the Capabilities of the attached Rx */
	    nResult = NEXUS_HdmiOutput_GetEdidData(g_hdmi_instance.hHdmi, &edid);
		DONE_ERR((nResult != NEXUS_SUCCESS));

		PrintDebug("Support deepColor30bit = %d\n",edid.hdmiVsdb.deepColor30bit);
		PrintDebug("Support deepColor36bit = %d\n",edid.hdmiVsdb.deepColor36bit);
		PrintDebug("Support deepColor48bit = %d\n",edid.hdmiVsdb.deepColor48bit);

		NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &nSettings);

		nSettings.autoColorSpace = FALSE;
		nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
		nSettings.colorDepth = 8;

	/* 	Priority Setting Sequence */

		if(stRequestedVideoFormatSupport.yCbCr422&&edid.hdmiVsdb.deepColor36bit)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr422;
			nSettings.colorDepth = 12;
		}
		else if(stRequestedVideoFormatSupport.yCbCr422&&edid.hdmiVsdb.deepColor30bit)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr422;
			nSettings.colorDepth = 10;
		}
		else if(stRequestedVideoFormatSupport.yCbCr420&&edid.hdmiVsdb.deepColor36bit)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
			nSettings.colorDepth = 12;
		}
		else if(stRequestedVideoFormatSupport.yCbCr420&&edid.hdmiVsdb.deepColor30bit)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
			nSettings.colorDepth = 10;
		}
		else if(stRequestedVideoFormatSupport.yCbCr444rgb444)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
			nSettings.colorDepth = 8;
		}
		else if(stRequestedVideoFormatSupport.yCbCr422)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr422;
			nSettings.colorDepth = 8;
		}
		else if(stRequestedVideoFormatSupport.yCbCr420)
		{
			nSettings.colorSpace = NEXUS_ColorSpace_eYCbCr420;
			nSettings.colorDepth = 8;
		}
		else
		{
			DI_UART_Print("[%s] default Color space/depth setting \n",__func__);
		}

		DI_UART_Print("[%s]colorSpace = %d\n",__func__,nSettings.colorSpace);
		DI_UART_Print("[%s]colorDepth = %d\n",__func__,nSettings.colorDepth);

		nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &nSettings);
		DONE_ERR((nResult != NEXUS_SUCCESS));
	}
#endif

done:
	UNLOCK_HDMI();
	PrintExit();
	return DRV_OK;
}
#endif

DRV_Error DRV_HDMI_PrintEdidInfo(void)
{
#if (NEXUS_VERSION >= 1402)
	unsigned int i;
	NEXUS_HdmiOutputEdidData Edid;
	NEXUS_Error rc = NEXUS_SUCCESS;

	VK_MEM_Memset(&Edid, 0x0, sizeof(NEXUS_HdmiOutputEdidData));

	CHECK_INIT();

	LOCK_HDMI();

	rc = NEXUS_HdmiOutput_GetEdidData(g_hdmi_instance.hHdmi, &Edid);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetEdidData error!!!\n");
		UNLOCK_HDMI();
		return DRV_ERR;
	}

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

	UNLOCK_HDMI();
#else
	DI_UART_Print("DRV_HDMI_PrintEdidInfo() is not supported!!!\n", __FUNCTION__);
#endif

	return DRV_OK;
}

#if defined(CONFIG_HDCP)
DRV_Error DRV_HDMI_EnableHDCP(HBOOL bEnable)
{
 	HdmiMsg_t HdmiMsg;
	DRV_Error dResult = DRV_OK;

  	HAPPY(bEnable);

	PrintEnter();

 	PrintDebug("DI_HDMI_EnableHDCP bEnable: %s\n", bEnable?"T":"F");

	HdmiMsg.eHdmiMsg = (bEnable == TRUE) ? HDMI_MSG_CMD_ENABLE_HDCP : HDMI_MSG_CMD_DISABLE_HDCP;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return DRV_OK;
}
#endif

DRV_Error DRV_HDMI_GetHdcpKey(HUINT8** ppHdcpKey)
{
	DRV_Error rc = DRV_OK;

	PrintEnter();

	CHECK_INIT();

	LOCK_HDMI();

	*ppHdcpKey=s_ucaHdcpKey;

	UNLOCK_HDMI();

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

	nResult = NEXUS_HdmiOutput_GetAviInfoFrame(g_hdmi_instance.hHdmi, &AviInfoFrame);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetAviInfoFrame failed!\n");
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
{
	DRV_Error rc = DRV_OK;
	NEXUS_Error nResult;
	NEXUS_HdmiOutputStatus hdmiStatus;
	NEXUS_HdmiOutputHdcpStatus hdcpStatus;
	char *pszEnable = "enable";
	char *pszDisable = "disable";
	char videoFormatStr[100] = {0, };
	const char *pszMute = "Mute";
	const char *pszUnmute = "Unmute";
	DI_HDMI_AVAILABLE_FORMATS Available;
	NEXUS_HdmiOutputSettings hdmiSettings;
	char* strColorSpace[NEXUS_ColorSpace_eMax+1]=
		{
 #if (NEXUS_VERSION >= 1402)
			"NEXUS_ColorSpace_eAuto",
 #endif
	    	"NEXUS_ColorSpace_eRgb",
		    "NEXUS_ColorSpace_eYCbCr422",   /* 2 */
    		"NEXUS_ColorSpace_eYCbCr444",
 #if (NEXUS_VERSION >= 1402)
    		"NEXUS_ColorSpace_eYCbCr420",   /* 4 */
 #endif
    		"NEXUS_ColorSpace_eMax"
    	};

	unsigned int i = 0;

#if !defined(CONFIG_DEBUG)
	HAPPY(strColorSpace);
	HAPPY(pszUnmute);
	HAPPY(pszMute);
	HAPPY(pszDisable);
	HAPPY(pszEnable);
#endif

	PrintEnter();

	DI_UART_Print("============================== MW requested =========================\n");
	DI_UART_Print("\n");

	LOCK_REQUEST();

	DI_UART_Print("HDMI Enable : [%s]\n", g_hdmi_request.bHdmiEnable ? pszEnable : pszDisable );
	DI_UART_Print("HDCP Enable : [%s]\n", g_hdmi_request.bHdcpEnable ? pszEnable : pszDisable );
	DI_UART_Print("AV MUTE     : [%s]\n", g_hdmi_request.bHdmiAvMute ? pszMute : pszUnmute );
	DI_UART_Print("AUDIO Mute  : [%s]\n", g_hdmi_request.bHdmiAudioMute ? pszMute : pszUnmute );
	DI_UART_Print("AV BLANK    : [%s]\n", g_hdmi_request.bHdmiAvBlank ? pszEnable : pszDisable );
	DI_UART_Print("HDMI POWER  : [%s]\n", g_hdmi_request.bHdmiPowerState? pszEnable : pszDisable );

	UNLOCK_REQUEST();

	DI_UART_Print("=====================================================================\n");

	DI_UART_Print("============================== NEXUS status =========================\n");

	nResult = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &hdmiStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetStatus failed!\n");
	}

	nResult = NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi_instance.hHdmi, &hdcpStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_GetHdcpStatus failed!\n");
	}

	DI_UART_Print("connected : [%s]\n", hdmiStatus.connected ? "Yes" : "No");
	DI_UART_Print("rxPowered : [%s]\n", hdmiStatus.rxPowered ? "Yes" : "No");
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
	DI_UART_Print("Hdcp Authentication: [%s]\n", hdcpStatus.transmittingEncrypted?"T":"F");
	DI_UART_Print("Is Repeater?: [%s]\n", hdcpStatus.isHdcpRepeater?"T":"F");
	DI_UART_Print("Monitor Name : %s\n", hdmiStatus.monitorName);
	DI_UART_Print("\n");

	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);
	DI_UART_Print("ColorSpace : %s (%d)\nColorDepth : %dbit \n\n",strColorSpace[hdmiSettings.colorSpace],hdmiSettings.colorSpace, hdmiSettings.colorDepth);

	DI_UART_Print("NEXUS supported video format\n");

	for(i=0; i<NEXUS_VideoFormat_eMax; i++)
	{
		if(hdmiStatus.videoFormatSupported[i] == true)
		{
			videoFormatStr[0] = 0;
#if defined(CONFIG_VIDEO_DECODER)
			DRV_VIDEO_GetVideoFormatStr(i, videoFormatStr);
#endif
			DI_UART_Print("\t\t%s\n", videoFormatStr);
		}

	}

	DI_UART_Print("=====================================================================\n");

#if defined(CONFIG_DI20)
	{
	    char *pTempString = NULL;
		pfn_cmdhwtest_InfoUI infoUIFn = (pfn_cmdhwtest_InfoUI)NULL;

		pTempString = DD_MEM_Alloc(200);
		if(pTempString == NULL)
		{
			PrintError("Can't Allocate Buffer\n");
		}
		else
		{
			VK_snprintf(&pTempString[0], 200, "\n\tConnected : %s\n\tHDCP Authentication : %s\n\tMonitor Name : %s\n", hdmiStatus.connected?"YES":"NO",
					hdcpStatus.transmittingEncrypted?"SUCCESS":"FAIL",
					hdmiStatus.monitorName);

			if(CMD_GetInfoUI() != NULL)
			{
				infoUIFn = CMD_GetInfoUI();
				if(NULL != infoUIFn)
				{
					infoUIFn(pTempString);
				}
			}
			DD_MEM_Free(pTempString);
		}
	}
#endif
	DRV_HDMI_PrintAviPacket();

	rc = P_HDMI_GetCapability(&Available);
	if(rc != DRV_OK)
	{
		PrintError("P_HDMI_GetCapability() returned error!!\n");
		return DRV_ERR;
	}

	DI_UART_Print("HDMI Capability\n");

	DI_UART_Print("\n");
	DI_UART_Print("preferredVideoFormat.Aspect Ratio = %x\n", g_hdmi_instance.stBasicEdidData.preferredVideoFormat.ulAspectRatio);
	DI_UART_Print("preferredVideoFormat.Resolution = %x\n", g_hdmi_instance.stBasicEdidData.preferredVideoFormat.ulVFormat);
	DI_UART_Print("preferredVideoFormat.Field Rate = %x\n", g_hdmi_instance.stBasicEdidData.preferredVideoFormat.ulFieldRate);

	DI_UART_Print("\n");
	DI_UART_Print("DI supported video format(EDID video format)\n");

	for(i=0; i<Available.ulNumOfVFormat; i++)
	{
		DI_UART_Print("\t\t%s | %s | %s\n",
			P_HDMI_GetHdmiResolutionStr(Available.stVformats[i].ulVFormat),
			P_HDMI_GetHdmiFieldRateStr(Available.stVformats[i].ulFieldRate),
			P_HDMI_GetHdmiAspectRatioStr(Available.stVformats[i].ulAspectRatio));
	}

	DI_UART_Print("DI supported video format(video format)\n");

	for(i = DI_HDMI_VFORMAT_480I; i<DI_HDMI_VFORMAT_DUMMY; i = i << 1)
	{
		if((Available.ulVideoFormats & i) == i)
		{
			DI_UART_Print("\t\t%s\n", P_HDMI_GetHdmiVideoFormatStr(i));
		}
	}

	DI_UART_Print("\n");
	DI_UART_Print("ulVideoFormats = %x\n", Available.ulVideoFormats);
	DI_UART_Print("ulAudioFormats = %x\n", Available.ulAudioFormats);


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
		//DI_UART_Print("########## %d \n",Available.tAudioInfo[i].unSupportedFs);
	}

	DI_UART_Print("\n");
	DI_UART_Print("sPhysicalAddr.NodeA = %x\n", Available.sPhysicalAddr.NodeA);
	DI_UART_Print("sPhysicalAddr.NodeB = %x\n", Available.sPhysicalAddr.NodeB);
	DI_UART_Print("sPhysicalAddr.NodeC = %x\n", Available.sPhysicalAddr.NodeC);
	DI_UART_Print("sPhysicalAddr.NodeD = %x\n", Available.sPhysicalAddr.NodeD);
	DI_UART_Print("ulLogicalAddr = %x\n", Available.ulLogicalAddr);

	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_HDMI_GetMonitorName(char *pucVendorId)
{
	PrintEnter();

	ASSERT(pucVendorId);

	VK_memcpy(pucVendorId, g_hdmi_instance.stBasicEdidData.monitorName, sizeof(g_hdmi_instance.stBasicEdidData.monitorName));

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_GetHdmiOutputHandle(NEXUS_HdmiOutputHandle *hHdmi)
{
	PrintEnter();

	*hHdmi = g_hdmi_instance.hHdmi;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_SetColorDepth(DRV_HDMI_ColorDepth colorDepth)
{
#if (NEXUS_VERSION >= 1403)
	NEXUS_Error nResult;
	NEXUS_HdmiOutputSettings nSettings;

	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &nSettings);

	if(colorDepth == DRV_HDMI_ColorDepth_e8bit)
	{
		nSettings.colorDepth = NEXUS_HdmiColorDepth_e8bit;
	}
	else if(colorDepth == DRV_HDMI_ColorDepth_e10bit)
	{
		nSettings.colorDepth = NEXUS_HdmiColorDepth_e10bit;
	}
	else if(colorDepth == DRV_HDMI_ColorDepth_e12bit)
	{
		nSettings.colorDepth = NEXUS_HdmiColorDepth_e12bit;
	}
	else if(colorDepth == DRV_HDMI_ColorDepth_e16bit)
	{
		nSettings.colorDepth = NEXUS_HdmiColorDepth_e16bit;
	}
	else
	{
		nSettings.colorDepth = NEXUS_HdmiColorDepth_eMax;
	}

	nResult = NEXUS_HdmiOutput_SetSettings(g_hdmi_instance.hHdmi, &nSettings);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetSettings failed!\n");
		return DRV_ERR;
	}
#else
	HAPPY(colorDepth);
	PrintError("DRV_HDMI_SetColorDepth is not supported \n");
#endif

	return DRV_OK;
}

DRV_Error DRV_HDMI_SetColorSpaceInfo(DI_HDMI_AUX_VIDEO_INFO stAVI)
{
	HdmiMsg_t HdmiMsg;
	DI_HDMI_AUX_VIDEO_INFO stAuxVideoInfo;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	VK_memset(&stAuxVideoInfo, 0x00, sizeof(DI_HDMI_AUX_VIDEO_INFO));
	VK_memcpy(&stAuxVideoInfo, &stAVI, sizeof(DI_HDMI_AUX_VIDEO_INFO));

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_COLORSPACE_INFO;
	HdmiMsg.param1 = (unsigned long)&stAuxVideoInfo;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;
}

DRV_Error DRV_HDMI_SetInfoFrame(DI_HDMI_SPECIFIC_INFO_t *stInfo)
{
	HdmiMsg_t HdmiMsg;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_SPECIFIC_INFO;
	HdmiMsg.param1 = (unsigned long)stInfo;

	LOCK_HDMI();

	dResult = P_HDMI_SetSpecificInfo(stInfo);
	if (dResult != DRV_OK)
	{
		PrintError("P_HDMI_SetSpecificInfo failed!\n");
	}

	UNLOCK_HDMI();

	PrintExit();

	return dResult;

}

DRV_Error DRV_HDMI_SetDisplayOutput(HBOOL bOnOff)
{
	HdmiMsg_t HdmiMsg;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	LOCK_HDMI();

	if( s_ulCheckHdmiPwrTimer != 0)
	{
		VK_TIMER_Cancel(s_ulCheckHdmiPwrTimer);
		s_ulCheckHdmiPwrTimer = 0;
	}

	HdmiMsg.eHdmiMsg = (bOnOff == TRUE) ? HDMI_MSG_CMD_ADD_HDMI_OUTPUT : HDMI_MSG_CMD_REMOVE_HDMI_OUTPUT;

	SEND_MESSAGE(&HdmiMsg);

	if(bOnOff == TRUE)
		HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_CHECK_HDMI_POWER;

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;
}

DRV_Error DRV_HDMI_SetPreEmphasisConfiguration(void)
{
	HdmiMsg_t HdmiMsg;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_PREEMPHASIS_CONFIGURATION;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;
}

DRV_Error DRV_HDMI_SetAVMute(HBOOL bMute)
{
	DRV_Error dResult = DRV_OK;
	HdmiMsg_t HdmiMsg;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_AV_MUTE;

	HdmiMsg.param1 = bMute ? TRUE : FALSE;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;
}

DRV_Error DRV_HDMI_SetTmds(HBOOL bEnable)
{
	DRV_Error dResult = DRV_OK;
	HdmiMsg_t HdmiMsg;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_TMDS_SIGNAL;

	HdmiMsg.param1 = bEnable ? TRUE : FALSE;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;
}

DRV_Error DRV_HDMI_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION eHDResolution)
{
	DRV_Error dResult = DRV_OK;
	HdmiMsg_t HdmiMsg;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_RESOLUTION;
	HdmiMsg.param1 = (HUINT32)eHDResolution;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;

}

DRV_Error DRV_HDMI_SetMuteAging(HBOOL bEnable)
{
	PrintEnter();

	s_bMuteAging = bEnable;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_SetDisplayAging(HBOOL bEnable)
{
	PrintEnter();

	s_bDisplayAging = bEnable;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_SetStandbyAging(HBOOL bEnable)
{
	PrintEnter();

	s_bStandbyAging = bEnable;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_HDMI_SetColorSpace(NEXUS_VideoFormat eVideoFormat)
{
	HdmiMsg_t HdmiMsg;
	DRV_Error dResult = DRV_OK;

	PrintEnter();

	HdmiMsg.eHdmiMsg = HDMI_MSG_CMD_SET_COLORSPACE;
	HdmiMsg.param1 = eVideoFormat;

	LOCK_HDMI();

	SEND_MESSAGE(&HdmiMsg);

	UNLOCK_HDMI();

	PrintExit();

	return dResult;
}

DRV_Error P_HDMI_SetPreEmphasisConfiguration(void)
{
#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
	HBOOL bCustomSetting = FALSE;
	HUINT16 wCustomPreemp = 0;
	HUINT8 cIndex = 0;

	NEXUS_DisplaySettings displaySettings;
	NEXUS_HdmiOutputPreEmphasisConfiguration PreEmphasisConfiguration;
	NEXUS_HdmiOutputSettings hdmiSettings;

	NEXUS_Error rc = NEXUS_SUCCESS;
#endif

	PrintEnter();

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)

	if(g_hdmi_instance.bInitialized != TRUE)
	{
		PrintError("[%s] Hdmi is not Initialized, rc = %d",__func__);
		PrintExit();
		return DRV_ERR;

	}

	NEXUS_Display_GetSettings(g_hdmi_instance.hDisplayHd, &displaySettings);
	NEXUS_HdmiOutput_GetSettings(g_hdmi_instance.hHdmi, &hdmiSettings);

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

	rc = NEXUS_HdmiOutput_GetPreEmphasisConfiguration(g_hdmi_instance.hHdmi, &PreEmphasisConfiguration);
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

	rc = NEXUS_HdmiOutput_SetPreEmphasisConfiguration(g_hdmi_instance.hHdmi, &PreEmphasisConfiguration);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_HdmiOutput_SetPreEmphasisConfiguration() Failed, rc = %d", rc);
		PrintExit();
		return DRV_ERR;
	}
#endif

	PrintExit();

	return DRV_OK;
}

#if __________di____________
#endif

DI_ERR_CODE DI_HDMI_SetPowerState(HBOOL bPowerOn)
{
	DRV_Error dResult = DRV_OK;
	HBOOL bConnect = bPowerOn;
	HBOOL bRequestedHdmiPowerState = FALSE;
	HBOOL bRequestedHdcpState = FALSE;

	PrintEnter();

	LOCK_REQUEST();

	bRequestedHdmiPowerState = g_hdmi_request.bHdmiPowerState;
	bRequestedHdcpState = g_hdmi_request.bHdcpEnable;

	if (bRequestedHdmiPowerState == bPowerOn)
	{
		UNLOCK_REQUEST();
		PrintError("SetPowerState(%d) was requested already !!\n", bPowerOn);
		PrintExit();
		return DI_ERR_OK;
	}

	g_hdmi_request.bHdmiPowerState = bPowerOn;

	UNLOCK_REQUEST();

	dResult = DRV_HDMI_SetDisplayOutput(bConnect);
	if(dResult != DRV_OK)
	{
		PrintError("DRV_HDMI_SetDisplayOutput() Failed!!\n");
		PrintExit();
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_EnableHDCP(HBOOL bEnable)
{
	PrintEnter();

#if defined(CONFIG_HDCP)
#else
	HAPPY(bEnable);
#endif

#if defined(CONFIG_HDCP)
	LOCK_REQUEST();

	if (g_hdmi_request.bHdcpEnable == bEnable)
	{
		UNLOCK_REQUEST();
		PrintError("HDCP enable(%d) was requested already !!\n", bEnable);
		PrintExit();
		return DI_ERR_OK;
	}

	g_hdmi_request.bHdcpEnable = bEnable;

	UNLOCK_REQUEST();

#if defined(HDCP_CERTI_MODE)
	P_HDMI_StartHDCP();
	PrintExit();

	return DI_ERR_OK;
#endif

	P_HDMI_SetTimerFunction(HDCP_POLLING_SLEEP, P_HDMI_TCB_EnableHdcp);
#endif

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetState(HDMI_RECEIVER_INFO_OPTION ReceiverInfoOption, HDMI_RECEIVER_INFO* pReceiverInfo)
{
	NEXUS_Error rc;
	NEXUS_HdmiOutputStatus HdmiOutputStatus;
	NEXUS_HdmiOutputHdcpStatus hdcpStatus;

	PrintEnter();

	CHECK_INIT();

	ASSERT(pReceiverInfo);

	LOCK_HDMI();

	pReceiverInfo->bIsHdmiDevice = FALSE;
	pReceiverInfo->bIsConnected = FALSE;
	pReceiverInfo->bIsAuthencated = FALSE;

	rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &HdmiOutputStatus);
	if(rc != NEXUS_SUCCESS)
	{
		UNLOCK_HDMI();
		PrintError("NEXUS_HdmiOutput_GetStatus() returned error!, rc = %d\n", rc);
		PrintExit();
		return DI_ERR_ERROR;
	}

	if ((ReceiverInfoOption & IS_CONNECTED) == IS_CONNECTED)
	{
		// This code is about "A TV is connected to a HDMI port or not?"
#if (NEXUS_VERSION < 1200)
		pReceiverInfo->bIsConnected = (HdmiOutputStatus.connected ? TRUE : FALSE);
#else
		if(HdmiOutputStatus.connected && HdmiOutputStatus.rxPowered)
		{
			pReceiverInfo->bIsConnected = TRUE;
		}
#endif
		PrintError("[NEXUS].connected = %d, [NEXUS].rxPowered = %d, [DI].bIsConnected = %d\n",
						HdmiOutputStatus.connected,
						HdmiOutputStatus.rxPowered,
						pReceiverInfo->bIsConnected);
	}

	if ((ReceiverInfoOption & IS_HDMI_DEVICE) == IS_HDMI_DEVICE)
	{
		pReceiverInfo->bIsHdmiDevice = HdmiOutputStatus.hdmiDevice;
		PrintError("[DI].bIsHdmiDevice = %d\n", pReceiverInfo->bIsHdmiDevice);
	}

	if ((ReceiverInfoOption & IS_AUTHENCATED) == IS_AUTHENCATED)
	{
		// This code is about "Current link is authenticated or not?"

		rc = NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi_instance.hHdmi, &hdcpStatus);
		if (rc != NEXUS_SUCCESS)
		{
			UNLOCK_HDMI();
			PrintError("NEXUS_HdmiOutput_GetHdcpStatus error\n");
			PrintExit();
			return DI_ERR_OK;
		}

		switch (hdcpStatus.hdcpState)
		{
		case NEXUS_HdmiOutputHdcpState_eInitializedAuthentication :
		case NEXUS_HdmiOutputHdcpState_eLinkAuthenticated :
		case NEXUS_HdmiOutputHdcpState_eEncryptionEnabled:
			// Use the value of hdcpStatus.hdcpState that is taken when P_HDMI_HdcpCallback gets a SUCCEESS message.
			pReceiverInfo->bIsAuthencated = TRUE;
			break;
		default :
			pReceiverInfo->bIsAuthencated = FALSE;
			break;
		}

		PrintError("[DI].bIsAuthenticated = %d\n", pReceiverInfo->bIsAuthencated);
	}

#if defined(CONFIG_HDCP_22)
	if ((ReceiverInfoOption & IS_HDCP22_SINK) == IS_HDCP22_SINK)
	{
		rc = NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi_instance.hHdmi, &hdcpStatus);
		if (rc != NEXUS_SUCCESS)
		{
			UNLOCK_HDMI();
			PrintError("NEXUS_HdmiOutput_GetHdcpStatus error\n");
			PrintExit();
			return DI_ERR_ERROR;
		}
		if(hdcpStatus.hdcp2_2Features == true)
		{
			pReceiverInfo->bIsHDCP22Sink = TRUE;
		}
		else
		{
			pReceiverInfo->bIsHDCP22Sink = FALSE;
		}
	}
#endif
	UNLOCK_HDMI();

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_RegisterEventNotify(DI_HDMI_EVENT eHdmiEvent, DI_HDMI_CALLBACK pfnHdmiEventCallback)
{
	DI_ERR_CODE rc=DI_ERR_OK;

	PrintEnter();

	CHECK_INIT();

	ASSERT(pfnHdmiEventCallback);

	LOCK_HDMI();

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s : event :%d\n"PARAM_COLOR_OFF, __FUNCTION__, eHdmiEvent);
#endif

	if (eHdmiEvent<DH_EVENT_MAX)
		g_hdmi_instance.pfCallback[eHdmiEvent]= pfnHdmiEventCallback;
	else
	{
		PrintDebug("[%s] Unknown event\n", __func__);
		rc = DI_ERR_ERROR;
		goto done;
	}

done:
	UNLOCK_HDMI();

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable)
{
	DI_ERR_CODE	rc=DI_ERR_OK;

	PrintEnter();

	CHECK_INIT();

	ASSERT(pAvailable);

	LOCK_HDMI();

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	DI_UART_Print(PARAM_COLOR_ON"%s\n"PARAM_COLOR_OFF, __FUNCTION__);
#endif

	rc = P_HDMI_GetCapability(pAvailable);

	UNLOCK_HDMI();

	PrintExit();

	return rc;
}

DI_ERR_CODE DI_HDMI_EnableOutput(HBOOL bEnable)
{
	PrintEnter();

	/* Do nothing but save mw setting */
	PrintError("Do not use this function. This is doing nothing.\n");

	LOCK_REQUEST();

	if (g_hdmi_request.bHdmiEnable == bEnable)
	{
		UNLOCK_REQUEST();
		PrintError("HDMI Output enable(%d) was requested already !!\n", bEnable);
		PrintExit();
		return DI_ERR_OK;
	}

	g_hdmi_request.bHdmiEnable = bEnable;

	UNLOCK_REQUEST();

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SetAVMute(HBOOL bEnable)
{
	HAPPY(bEnable);

	PrintEnter();

#if 0
	/* Do not use this yet */
	res = DRV_HDMI_SetAVMute(bEnable);
	if(res != DRV_OK)
	{
		PrintError("DRV_HDMI_SetAVMute() returned ERROR!\n");
		PrintExit();
		return DI_ERR_ERROR;
	}
#else
	PrintError("Do not call this function\n");
#endif

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SetAVBlank(HBOOL bBlank)
{
	HAPPY(bBlank);

	PrintEnter();

	PrintError("Do not call this function\n");

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetEdidInfo(DI_HDMI_EDID_INFO* pEdidInfo)
{
	PrintEnter();

	CHECK_INIT();

	ASSERT(pEdidInfo);

	VK_memcpy(pEdidInfo, &(g_hdmi_instance.stBasicEdidData), sizeof(DI_HDMI_EDID_INFO));

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SetAuxVideoInfo(DI_HDMI_AUX_VIDEO_INFO stAVInfo)
{
	DRV_Error eDrvErr = DRV_OK;
	DI_HDMI_SPECIFIC_INFO_t stFrameInfo;

	PrintEnter();

	CHECK_INIT();

	VK_memset(&stFrameInfo, 0x00, sizeof(DI_HDMI_SPECIFIC_INFO_t));

	stFrameInfo.eFrame = DI_HDMI_AVI_INFO;
	stFrameInfo.eAFD = stAVInfo.eAFD;
	stFrameInfo.eAR= stAVInfo.eAR;

	eDrvErr = DRV_HDMI_SetInfoFrame(&stFrameInfo);
	if (eDrvErr != DRV_OK)
	{
		PrintError("DRV_HDMI_SetInfoFrame() returned Error!!");
		PrintExit();
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_SetColorSpace_Info(DI_HDMI_AUX_VIDEO_INFO stAVInfo)
{
	DRV_Error eDrvErr = DRV_OK;
	DI_HDMI_SPECIFIC_INFO_t stFrameInfo;

	PrintEnter();

	CHECK_INIT();

	VK_memset(&stFrameInfo, 0x00, sizeof(DI_HDMI_SPECIFIC_INFO_t));

	stFrameInfo.eFrame = DI_HDMI_COLOR_SPACE_INFO;
	stFrameInfo.eColorSpace = stAVInfo.eColorSpace;

	eDrvErr = DRV_HDMI_SetInfoFrame(&stFrameInfo);
	if (eDrvErr != DRV_OK)
	{
		PrintError("DRV_HDMI_SetInfoFrame() returned Error!!");
		PrintExit();
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

/*
	NOT USED NOW
*/
DI_ERR_CODE DI_HDMI_SetColorSpace(NEXUS_VideoFormat eVideoFormat)
{
	DRV_Error eDrvError = DRV_OK;

	PrintEnter();

	CHECK_INIT();

	eDrvError = DRV_HDMI_SetColorSpace(eVideoFormat);
	if (eDrvError != DRV_OK)
	{
		PrintError("DRV_HDMI_SetAuxVideoInfo() returned Error!!");
		PrintExit();
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT e3dFormat)
{
	DRV_Error eDrvError = DRV_OK;
	DI_HDMI_SPECIFIC_INFO_t stFrameInfo;

	PrintEnter();

	CHECK_INIT();

	VK_memset(&stFrameInfo, 0x00, sizeof(DI_HDMI_SPECIFIC_INFO_t));

	stFrameInfo.eFrame = DI_HDMI_3D_INFO;
	stFrameInfo.e3DFormat = e3dFormat;

	eDrvError = DRV_HDMI_SetInfoFrame(&stFrameInfo);
	if (eDrvError != DRV_OK)
	{
		PrintError("DRV_HDMI_SetInfoFrame() returned Error!!");
		PrintExit();
		return DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetAutoLipSyncInfo(DI_HDMI_AUTOLIPSYNC_INFO* pstInfo)
{
	PrintEnter();

	CHECK_INIT();

	ASSERT(pstInfo);

	VK_memcpy(pstInfo, &(g_hdmi_instance.stAutoLipsyncInfo), sizeof(DI_HDMI_AUTOLIPSYNC_INFO));

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetRawEdid(DI_HDMI_EDID_RAW* pEdid)
{
	PrintEnter();

	CHECK_INIT();

	ASSERT(pEdid);

	VK_memcpy(pEdid, &(g_hdmi_instance.stEdidBlock), sizeof(DI_HDMI_EDID_RAW));

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetPortOutNum(HUINT8* ucNum)
{
	*ucNum = DI_HDMI_NUM_OUTPUTS;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_GetHdcpVersion(DI_HDMI_HDCP_VERSION *eHdcpVersion)
{
	PrintEnter();

	CHECK_INIT();

	ASSERT(eHdcpVersion);

#if defined(CONFIG_HDCP_22)
	*eHdcpVersion = DI_HDMI_HDCP_VERSION_2_2;
#else
	*eHdcpVersion = DI_HDMI_HDCP_VERSION_1_4;
#endif

	PrintExit();

	return DI_ERR_OK;
}

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
	BSTD_UNUSED(pucInput);
	BSTD_UNUSED(nKeySize);
	BSTD_UNUSED(pucOutput);
#endif
    return DI_ERR_OK;
}

DI_ERR_CODE DI_HDMI_HDCP22KeyValidation(HUINT8 *pucInput, HUINT32 nKeySize)
{
#if defined(CONFIG_HDCP_22_PROVISIONING)
    int rc = 0;

	if(pucInput == NULL || !nKeySize)
	{
		PrintError("[%s] Error : Invalid parameter - pucInput(0x%x)\r\n", __func__, pucInput);
		return DI_ERR_INVALID_PARAM;
	}

    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***\tBroadcom Corporation DRM bin file Validation utility (Copyright 2014)\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");

	s_tHDCP22Provision.pucInput = pucInput;
	s_tHDCP22Provision.nKeySize = nKeySize;

    /* Initialize manufacturing platform and provsioning module */
    rc = SAGE_Provisioning_Init();
    if (rc != 0)
	{
        goto handle_error;
    }

    rc = SAGE_Provisioning_ValidationKey();
    if (rc != 0)
	{
        goto handle_error;
    }

handle_error:
	PrintError("[%s] Shutting down validation module and platform\r\n", __func__);
    SAGE_Provisioning_Uninit();

    if (rc)
	{
		PrintError("[%s] Failure in SAGE Provisioning tool\r\n", __func__);
		return DI_ERR_ERROR;
    }
#else
	BSTD_UNUSED(pucInput);
	BSTD_UNUSED(nKeySize);
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

	/* Initialize Validation module */
	sage_rc = SRAI_Module_Init(s_tHDCP22Provision.platformHandle,
							   VALIDATION_MODULE,
							   container, &s_tHDCP22Provision.validationHandle);
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

    if (s_tHDCP22Provision.validationHandle)
    {
		PrintError("\t*** Uninitializing module '%p'\r\n", s_tHDCP22Provision.validationHandle);
        SRAI_Module_Uninit(s_tHDCP22Provision.validationHandle);
        s_tHDCP22Provision.validationHandle = NULL;
    }

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

int SAGE_Provisioning_ValidationKey(void)
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

    sage_rc = SRAI_Module_ProcessCommand(s_tHDCP22Provision.validationHandle, VALIDATION_COMMAND_ValidateHdcp22, container);
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
    	if((container->basicOut[1] == SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED))
		{
			rc = 0;
		}
		else
		{
			rc = 1;
		}

        if(container->blocks[0].data.ptr != NULL)
		{
            SRAI_Memory_Free(container->blocks[0].data.ptr);
            container->blocks[0].data.ptr = NULL;
        }

        SRAI_Container_Free(container);
        container = NULL;
    }

    return rc;
}


int SAGE_Provisioning_ProcessBinFile(void)
{
    BERR_Code sage_rc;
    int rc = 0;
#if (NEXUS_VERSION >= 1501)
    SAGE_Manufacturing_OTP_Index otp_index = SAGE_Manufacturing_OTP_Index_A; //default otp index
#endif

    BSAGElib_InOutContainer *container = NULL;

	container = SRAI_Container_Allocate();
    if (container == NULL)
    {
        rc = 1;
        goto handle_error;
    }

#if (NEXUS_VERSION >= 1501)
    container->basicIn[0] = otp_index + 1;
#endif
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
        if (container->basicOut[1] == SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED)
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


