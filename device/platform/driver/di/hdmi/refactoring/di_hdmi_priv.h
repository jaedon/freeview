#ifndef __DI_HDMI_PRIV_H__
#define __DI_HDMI_PRIV_H__

#include "di_hdmi.h"

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

/* workaround:
        Samsung UHD TV (UN40HU7000F),
        TV goes wrong state of invalid mode or no signal in high bit rate.
*/
#define WORKAROUND_HDMI_INVALIDMODE_SAMSUNG_UN40HU7000F

#if (NEXUS_VERSION >= 1290)
#define NEXUS_AVIINFO_FUNCTION
#endif

//#define HPD_DELAY 1000


// realod validation hdcp key for factory application
#define RELOAD_VALID_HDCP_KEY

// #define WORKAROUND_HDMI_VENDOR_ID

#define HDMI_CEC_PROCESSING_TIME	(5 * 1000)
#if defined(CONFIG_TEMP_HGS1000S)
#define HDCP_SHORT_SLEEP 2000
#else
#define HDCP_SHORT_SLEEP 100
#endif
#define HDCP_POLLING_SLEEP 6000
#define HDCP_RESOLUTION_SLEEP 2000
#define MAX_HDCP_POLLING_COUNT 5
#define MAX_EVENT_WAIT_TIME 5000
#define POWER_RETRY_TIME 100
#define MAX_POWER_RETRY_COUNT 100
#define MAX_EDID_RETRY_COUNT	20

#define EDID_READ_INTERVAL 100

/*	NOTICE!!
*	This interval is dependent to the QuantumData firmware version.
*	100ms is the worst case value for the pass of Hdmi Compliance Test.
*	It just may work with 1000ms.
*/
#define MAX_HDCP_RETRY_COUNT 50

#if defined(CONFIG_HDCP_22)
#define SIZEOF_DRM_BINFILE_HEADER (192)
#define SIZEOF_DYNAMIC_OFFSET_HEADER (16)

#define GET_UINT32_FROM_BUF(pBuf) \
    (((HUINT32)(((HUINT8*)(pBuf))[0]) << 24) | \
     ((HUINT32)(((HUINT8*)(pBuf))[1]) << 16) | \
     ((HUINT32)(((HUINT8*)(pBuf))[2]) << 8)  | \
     ((HUINT8 *)(pBuf))[3])
#endif

#define DI_HDMI_NUM_OUTPUTS  NEXUS_NUM_HDMI_OUTPUTS

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

typedef void (*pfnTimerCallback)(unsigned long, void *);

typedef enum
{
	HdcpResult_Success = 0,
	HdcpResult_Failed,
	HdcpResult_StatusChanged,
	HdcpResult_Max
}HdcpResult_e;

typedef enum
{
	HdmiHotplugState_Connected = 0,
	HdmiHotplugState_Disconnected,
	HdmiHotplugState_Max
}HdmiHotplugState_e;


typedef enum
{
	HDMI_MSG_EVT_HOTPLUG,		
	HDMI_MSG_EVT_HDCP,
	HDMI_MSG_CMD_ENABLE_HDCP,
	HDMI_MSG_CMD_DISABLE_HDCP,
	HDMI_MSG_CMD_SEND_CEC_MSG,
	HDMI_MSG_CMD_SET_RESOLUTION,
	HDMI_MSG_CMD_SET_AUX_VIDEO_INFO,
	HDMI_MSG_CMD_SET_COLORSPACE_INFO,
	HDMI_MSG_CMD_SET_COLORSPACE,
	HDMI_MSG_CMD_SET_3D_FORMAT,
	HDMI_MSG_CMD_SET_VENDOR_SPECIFIC_INFO,
	HDMI_MSG_CMD_ADD_HDMI_OUTPUT,
	HDMI_MSG_CMD_REMOVE_HDMI_OUTPUT,
	HDMI_MSG_CMD_CHECK_HDMI_POWER,
	HDMI_MSG_CMD_SET_PREEMPHASIS_CONFIGURATION,
	HDMI_MSG_CMD_SET_SPECIFIC_INFO,
	HDMI_MSG_CMD_SET_AV_MUTE,
	HDMI_MSG_CMD_SET_TMDS_SIGNAL,
	HDMI_MSG_SLF_HDCP_CHECK_POLLING,
	HDMI_MSG_MAX
} HdmiMsg_e;

/**
 * @brief typedef of struct HdmiInstance_t
 *
 * DI HDMI Instance.
 */

typedef struct
{
	NEXUS_HdmiOutputHandle hHdmi;	/* nexus hdmi handle */
	DI_HDMI_EDID_INFO stBasicEdidData;	/* basic edid data */
	DI_HDMI_EDID_RAW stEdidBlock;	/* raw edid data */
#if defined(CONFIG_HDMI_CEC)
	NEXUS_CecHandle hCec;	/* nexus cec handle */
#endif
	NEXUS_DisplayHandle hDisplayHd;	/* nexus display handle */
	DI_HDMI_CALLBACK pfCallback[DH_EVENT_MAX];	/* DI callback function pointer */
	unsigned long		interruptId;	/* interrupt id, used in VK INTR */
	HBOOL					bInitialized;	/* is initialized or not */
	HULONG 					ulSemId;	/* semaphore */
	DI_HDMI_AUTOLIPSYNC_INFO stAutoLipsyncInfo;	/* auto lipsync info */
	HBOOL		bStartedHdcp;
} HdmiInstance_t;

typedef struct
{
	HdmiMsg_e 	eHdmiMsg;
	HUINT32		param1;
	HUINT32		param2;
} HdmiMsg_t;

typedef struct
{
	HBOOL bHdcpEnable;
	HBOOL bHdmiEnable;
	HBOOL bHdmiAvMute;
	HBOOL bHdmiAudioMute;
	HBOOL bHdmiAvBlank;
	HBOOL bHdmiPowerState;
} HdmiRequest_t;

#define CHECK_INIT(x) \
	do { \
		if (g_hdmi_instance.bInitialized == FALSE) { \
			PrintError("HDMI instance is not initialized.\n") \
			return DI_ERR_ERROR; \
		} \
	} while (0)	

#define ASSERT(x) \
	do { \
		if (x == NULL) { \
			PrintError("Pointer is NULL.\n") \
			return DI_ERR_ERROR; \
		} \
	} while (0) 

#define LOCK_HDMI(x) \
	do { \
		int VK_Err = VK_OK; \
		VK_Err = VK_SEM_Get(g_hdmi_instance.ulSemId); \
		if(VK_Err != VK_OK) { \
			PrintError("Sema Get Error\n"); \
		} \
	} while (0)


#define UNLOCK_HDMI(x) \
	do { \
		int VK_Err = VK_OK; \
		VK_Err = VK_SEM_Release(g_hdmi_instance.ulSemId); \
		if(VK_Err != VK_OK) { \
			PrintError("Sema Get Error\n"); \
		} \
	} while (0)

#define LOCK_REQUEST(x) \
	do { \
		int VK_Err = VK_OK; \
		VK_Err = VK_SEM_Get(s_ulHdmiRequestSemId); \
		if(VK_Err != VK_OK) { \
			PrintError("Sema Get Error\n"); \
		} \
	} while (0)
		
		
#define UNLOCK_REQUEST(x) \
	do { \
		int VK_Err = VK_OK; \
		VK_Err = VK_SEM_Release(s_ulHdmiRequestSemId); \
		if(VK_Err != VK_OK) { \
			PrintError("Sema Get Error\n"); \
		} \
	} while (0)


#define SEND_MESSAGE(x) \
	do { \
		int vResult = VK_OK; \
		vResult = VK_MSG_Send(ulHdmiMsgQueueId, (x), sizeof(HdmiMsg_t)); \
		if (vResult != VK_OK) \
		{ \
			PrintError("VK_MSG_SendTimeout failed!\n"); \
		} \
	} while (0)


#define POST_EVENT(x) \
	do { \
		int vResult = VK_OK; \
		vResult = VK_EVENT_Send(g_ulHdmiSyncCallEventId, (x)); \
		if (vResult != VK_OK) \
		{ \
			PrintError("VK_EVENT_Send failed!\n"); \
		} \
	} while (0)


#define WAIT_EVENT(x) \
	do { \
		unsigned long vResult = 0; \
		vResult = VK_EVENT_ReceiveTimeout(g_ulHdmiSyncCallEventId, (unsigned long *)&(x), MAX_EVENT_WAIT_TIME); \
		if (vResult != VK_OK) \
		{ \
			PrintError("VK_EVENT_ReceiveTimeout failed!\n"); \
		} \
	} while (0)

#define CHECK_CONNECTION(OUTPUT_SETTING, STATUS) \
	do { \
		NEXUS_Error rc; \
		rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &(OUTPUT_SETTING)); \
		if(rc != NEXUS_SUCCESS) \
		{ \
			PrintError("NEXUS_HdmiOutput_GetStatus() Failed\n"); \
			PrintExit(); \
			return 0; \
		} \
		\
		(STATUS) = ((OUTPUT_SETTING).connected && (OUTPUT_SETTING).rxPowered) ? TRUE : FALSE; \
		\
		PrintDebug("\t\tconnected = (%d), rxPowered = (%d), hotplug state = (%d)\n", \
		(OUTPUT_SETTING).connected, (OUTPUT_SETTING).rxPowered, (STATUS)); \
	} while (0)
	
#define DI_HDMI_CHECK_CONNECTED(OUTPUT_SETTING, STATUS) \
	do { \
		NEXUS_Error rc; \
		rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &(OUTPUT_SETTING)); \
		if(rc != NEXUS_SUCCESS) \
		{ \
			PrintError("NEXUS_HdmiOutput_GetStatus() Failed\n"); \
			PrintExit(); \
			return; \
		} \
		\
		PrintDebug("stHdmiOutputStatus.connected = (%d)\n", (OUTPUT_SETTING).connected); \
		PrintDebug("stHdmiOutputStatus.rxPowered = (%d)\n", (OUTPUT_SETTING).rxPowered); \
		\
		(STATUS) = ((OUTPUT_SETTING).connected) ? TRUE : FALSE; \
		\
		PrintDebug("hotplug status(%d)\n", STATUS); \
	} while (0)
			
#define DI_HDMI_CHECK_RXPOWERED(OUTPUT_SETTING, STATUS) \
	do { \
		NEXUS_Error rc; \
		rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &(OUTPUT_SETTING)); \
		if(rc != NEXUS_SUCCESS) \
		{ \
			PrintError("NEXUS_HdmiOutput_GetStatus() Failed\n"); \
			PrintExit(); \
			return; \
		} \
		\
		PrintDebug("stHdmiOutputStatus.connected = (%d)\n", (OUTPUT_SETTING).connected); \
		PrintDebug("stHdmiOutputStatus.rxPowered = (%d)\n", (OUTPUT_SETTING).rxPowered); \
		\
		(STATUS) = ((OUTPUT_SETTING).rxPowered) ? TRUE : FALSE; \
		\
		PrintDebug("hotplug status(%d)\n", STATUS); \
	} while (0)

#define GET_NEXUS_STATUS(OUTPUT_SETTING) \
	do { \
		NEXUS_Error rc; \
		rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &(OUTPUT_SETTING)); \
		if(rc != NEXUS_SUCCESS) \
		{ \
			PrintError("NEXUS_HdmiOutput_GetStatus() Failed\n"); \
			PrintExit(); \
			return; \
		} \
	} while (0)

/* Function prototypes */
DRV_Error P_HDMI_SetPreEmphasisConfiguration(void);

void P_HDMI_ConvertNexusAudioCodecToHdmiAudioFormat(NEXUS_AudioCodec eNexusAudioCodec, DI_HDMI_AUDIO_FORMAT *pAudioFormat);
void P_HDMI_ConvertVideoIdCodeToHdmiEdidVideoFormat(HUINT32 ulVideoIdCode, DI_HDMI_EDID_VFORMAT *pstEdidVideoFormat);
void P_HDMI_ConvertNexusVideoFormatToHdmiEdidVideoFormat(NEXUS_VideoFormat eNexusVideoFormat, 
																		DI_HDMI_EDID_VFORMAT *pstEdidVideoFormat);
DRV_Error P_HDMI_GetHotplugStatus(HBOOL *bConnected);
DRV_Error P_HDMI_ForceUpdate(void);
DRV_Error P_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable);
void P_HDMI_HdcpShortRetryTimer(HULONG param0, void* param1);
void P_HDMI_SetHdcpShortRetryTimer(void);
void P_HDMI_HdcpLongRetryTimer(HULONG param0, void* param1);
void P_HDMI_SetHdcpLongRetryTimer(void);
void P_HDMI_HdcpSelfRetry(HULONG param0, void* param1);
void P_HDMI_SetHdcpSelfRetryTimer(void);
void P_HDMI_UnsetHdcpSelfRetryTimer(void);
void P_HDMI_SetDitheringData(void);
void P_HDMI_SetPauseBurstData(void);
void P_HDMI_SetDefaultColor(void);
void P_HDMI_HotplugCallback(void *pParam, int iParam);
void P_HDMI_HdcpCallback(void *pContext, int param);
DRV_Error P_HDMI_GetFirstVideoId(HUINT8* pFirstVideoId);
DRV_Error P_HDMI_EnableHDCP(void);
DRV_Error P_HDMI_DisableHDCP(void);
DRV_Error P_HDMI_OutputHandler(HdmiMsg_e eHdmiMsg);
DRV_Error P_HDMI_HdcpEventHandler(HdcpResult_e eHdcpResult);
DRV_Error P_HDMI_HotplugEventHandler(HdmiHotplugState_e eHotplugStatus);
void P_HDMI_FireCallbackFunc(DI_HDMI_EVENT eEvent);
DRV_Error P_HDMI_SetAuxVideoInfo(DI_HDMI_AUX_VIDEO_INFO *pstAuxVideoInfo);
DRV_Error P_HDMI_SetAviInfoAspect(DI_HDMI_VIDEO_AR eAR, DI_HDMI_VIDEO_AFD eAFD);
DRV_Error P_HDMI_SetColorSpaceInfo(DI_HDMI_AUX_VIDEO_INFO *pstAVInfo);
DRV_Error P_HDMI_SetColorSpace(NEXUS_VideoFormat eVideoFormat);
DRV_Error P_HDMI_SetVendorSpecificInfo(DI_HDMI_VENDOR_SPECIFIC_INFO *pstVendorSpecificInfo);
DRV_Error P_HDMI_AddHdmiOutput(void);
DRV_Error P_HDMI_RemoveHdmiOutput(void);
DRV_Error P_HDMI_SetTmds(HBOOL bEnable);
void P_HDMI_PowerCheckTimer(HULONG ulTimerId, void *pvParam);
void P_HDMI_Task(void *pvParam);
void P_HDMI_Intr(DI_HDMI_EVENT *pEvent);
void P_HDMI_SetTimerFunction(HUINT32 ulTime, pfnTimerCallback timerFunc);

#if defined(CONFIG_HDCP_22_PROVISIONING)
static int _ParseAndDisplay(HUINT8* pBinFile, HUINT32 binFileLength);
static const char * _MapDrmEnumToString(HUINT32 drm_type);
#endif

#endif	/* __DI_HDMI_PRIV_H__ */
