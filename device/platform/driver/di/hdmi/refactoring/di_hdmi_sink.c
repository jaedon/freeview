/**
* di_hdmi_sink.c
*/

/**
* @defgroup		DI_HDMI DI_HDMI : DI_HDMI module
* @author
* @modified by
* @note			HDMI API for sink device
* @brief
* @file 			di_hdmi_sink.c
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

#include "nexus_platform.h"
#include "nexus_hdmi_output_hdcp.h"
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
#endif
#include "di_hdmi_priv.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
extern HdmiInstance_t g_hdmi_instance;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

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

	rc = NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &hdmiOutputStatus);
	if(rc)
		BERR_TRACE(rc);

	if(hdmiOutputStatus.connected)
	{
		unsigned i, j;
		NEXUS_HdmiOutput_GetBasicEdidData(g_hdmi_instance.hHdmi, &hdmiOutputBasicEdidData);
		attachedRxEdid = BKNI_Malloc((hdmiOutputBasicEdidData.extensions + 1) * sizeof(edidBlock.data));
		for(i = 0; i <= hdmiOutputBasicEdidData.extensions; i++)
		{
			rc = NEXUS_HdmiOutput_GetEdidBlock(g_hdmi_instance.hHdmi, i, &edidBlock);
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
	DI_HDMI_SetInputToOutputExt(NULL);
	return;
}

void DI_HDMI_SetInputToOutputExt(DI_HDMI_PORT_INFO_t *ulHdmiPortInfo)
{
	HUINT32 inputPort = -1;
	HUINT32 outputPort = -1;
	DRV_Error nDrvErr = DRV_ERR;

	PrintEnter();
#if defined (CONFIG_HDMI_RECEIVER_SII9575)
	if(ulHdmiPortInfo != NULL)
	{
		inputPort = ulHdmiPortInfo->ulInputPort;
		outputPort = ulHdmiPortInfo->ulOutputPort;
		nDrvErr = DRV_Si_Sii9575_ChangePortMap(inputPort,outputPort);
		if(nDrvErr != DRV_OK)
		{
			PrintError("DRV_Si_Sii9575_ChangePortMap returns error\n");
			goto END;
		}
	}
#endif
	if(!s_bInitInputToOutput)
	{
		if(DI_HDMI_InitInputToOutput() < 0)
		{
			PrintError("DI_HDMI_InitInputToOutput returns error\n");
			goto END;
		}

		s_bInitInputToOutput = true;
	}

	DRV_VIDEO_SetHdmiVideoInputToOutput(0, s_hdmiInput);
	DRV_AUDIO_SetHdmiAudioInputToOutput(0, s_hdmiInput);

END :
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

int DRV_HDMI_Sync_Init(void)
{
	DRV_Error nDrvErr = DRV_OK;
	PrintEnter();

#if defined (CONFIG_HDMI_RECEIVER_SII9575)
		nDrvErr = DRV_Si_Sii9575_Init();
		if(nDrvErr != DRV_OK)
		{
			PrintError("%s->DRV_SIL9575_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nDrvErr, __LINE__, __FILE__);
		}
#endif
	PrintExit();
	return nDrvErr;
}
#endif


