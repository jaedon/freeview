/**
 * unittest_di_audio.c
*/

/**
 * @author		Larry
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"
#include "di_uart.h"
#include "di_err.h"
#include "di_audio.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_AUDIO
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
HUINT32 test_audio_passed;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static UT_RESULT ut_audio_Pause(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 deviceId;
	HUINT32 i;
	DI_AUDIO_RUNNING_STATE state;
	DI_AUDIO_FORMAT format;
	DI_AUDIO_STREAM_INFO_t audioStreamInfo;

	deviceId = DI_AUDIO_DECODER_MAIN;
	result = DI_AUDIO_GetState(deviceId, &state);
	DONE_ERR(DI_ERR_OK != result);

	if(state == DI_AUDIO_STATE_UNKNOWN)
	{
		format = DI_AUDFORMAT_MPEG;
		result = DI_AUDIO_Start(deviceId, format);
		DONE_ERR(DI_ERR_OK != result);
	}

	for(i=0; i<20; i++)
	{
		VK_TASK_Sleep(100);
		result = DI_AUDIO_Pause(deviceId, TRUE);
		DONE_ERR(DI_ERR_OK != result);

		VK_TASK_Sleep(100);
		result = DI_AUDIO_Pause(deviceId, FALSE);
		DONE_ERR(DI_ERR_OK != result);
	}

	VK_TASK_Sleep(100);
	ret = UT_OK;
done:
	return ret;
}


static UT_RESULT ut_audio_StartStopAudio(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 deviceId;
	DI_AUDIO_RUNNING_STATE state;
	DI_AUDIO_FORMAT format;
	DI_AUDIO_STREAM_INFO_t audioStreamInfo;

	deviceId = DI_AUDIO_DECODER_MAIN;
	result = DI_AUDIO_GetState(deviceId, &state);
	DONE_ERR(DI_ERR_OK != result);

	if(state != DI_AUDIO_STATE_UNKNOWN)
	{
		result = DI_AUDIO_GetStreamInfo(deviceId, &audioStreamInfo);
		DONE_ERR(DI_ERR_OK != result);
		format = audioStreamInfo.eFormat;

		result = DI_AUDIO_Stop(deviceId);
		DONE_ERR(DI_ERR_OK != result);

		result = DI_AUDIO_GetStreamInfo(deviceId, &audioStreamInfo);
		DONE_ERR(DI_ERR_OK == result);
	}else
	{
		format = DI_AUDFORMAT_MPEG;
	}

	DI_UART_Print("Stream format is  0x%08X \n", format);

	VK_TASK_Sleep(3000);
	result = DI_AUDIO_Start(deviceId, format);
	DONE_ERR(DI_ERR_OK != result);


	VK_TASK_Sleep(100);
	// Already Started => ERROR
	result = DI_AUDIO_Start(deviceId, format);
	DONE_ERR(DI_ERR_OK == result);
	
	VK_TASK_Sleep(100);

	ret = UT_OK;
done:
	return ret;
}


static UT_RESULT ut_audio_getCapability(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 numOfDevice;
	DI_AUDIO_CAP_t *pCapInfo;

	
	result = DI_AUDIO_GetCapability(&numOfDevice , &pCapInfo);
	DONE_ERR(DI_ERR_OK != result);

	if((0 > numOfDevice) || (NULL == pCapInfo))
	{
		ret = UT_FAIL;
		goto done;
	}

	DI_UART_Print("Number of audio device = %d \n", numOfDevice);
	DI_UART_Print("============================================\n");
	DI_UART_Print("=== Main Audio Decoder Capability Information === \n");
	DI_UART_Print("=== Group ID = %d \n", pCapInfo->group_id);
	DI_UART_Print("=== Decoder ID = %d \n", pCapInfo->decoder_id);
	DI_UART_Print("=== Decoder Type = %d \n", pCapInfo->decoder_type);
	DI_UART_Print("=== Support format = 0x%08X\n", pCapInfo->support_format);
	DI_UART_Print("=== Volume max = %d \n", pCapInfo->volume_max);
	DI_UART_Print("=== Primer Count = %d \n", pCapInfo->ulPrimerCount);
	DI_UART_Print("============================================\n");

	ret = UT_OK;
done:
	return ret;
}

static UT_RESULT ut_audio_SetVolume_Mute(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 deviceId;
	DI_AUDIO_RUNNING_STATE state;
	DI_AUDIO_FORMAT format;
	DI_AUDIO_STREAM_INFO_t audioStreamInfo;

	deviceId = DI_AUDIO_DECODER_MAIN;
	format = DI_AUDFORMAT_MPEG;
	DI_AUDIO_Start(deviceId, format);
	
	
	VK_TASK_Sleep(100);
	result = DI_AUDIO_SetVolume(DI_AUDIO_OUTPUT_HDMI, -10);
	DONE_ERR(DI_ERR_OK != result);

	VK_TASK_Sleep(100);
	result = DI_AUDIO_SetVolume(DI_AUDIO_OUTPUT_HDMI, 500);
	DONE_ERR(DI_ERR_OK != result);

	VK_TASK_Sleep(100);
	result = DI_AUDIO_SetVolume(DI_AUDIO_OUTPUT_HDMI, 70);
	DONE_ERR(DI_ERR_OK != result);

	VK_TASK_Sleep(100);
	result = DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_HDMI, TRUE);
	DONE_ERR(DI_ERR_OK != result);

	VK_TASK_Sleep(100);
	result = DI_AUDIO_SetMute(DI_AUDIO_OUTPUT_HDMI, FALSE);
	DONE_ERR(DI_ERR_OK != result);

	ret = UT_OK;
done:
	return ret;
}

static void func_audio_callback(void *pvParam)
{
    DI_AUDIO_CALLBACK_DATA_t *pstCallback = NULL;
    pstCallback = (DI_AUDIO_CALLBACK_DATA_t*)pvParam;
    if(pstCallback->nEvent == DI_AUDIO_RUNNING_STATE_CHANGED)
		test_audio_passed = TRUE;
}

static UT_RESULT ut_audio_RegisterCallback(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 deviceId;
	DI_AUDIO_RUNNING_STATE state;
	DI_AUDIO_FORMAT format;
	DI_AUDIO_STREAM_INFO_t audioStreamInfo;
	HUINT32 ulAudioCaller = 0;
	test_audio_passed = FALSE;

	deviceId = DI_AUDIO_DECODER_MAIN;
	result = DI_AUDIO_GetState(deviceId, &state);
	DONE_ERR(DI_ERR_OK != result);

	if(state != DI_AUDIO_STATE_UNKNOWN)
	{
		result = DI_AUDIO_GetStreamInfo(deviceId, &audioStreamInfo);
		DONE_ERR(DI_ERR_OK != result);
		format = audioStreamInfo.eFormat;

		result = DI_AUDIO_Stop(deviceId);
		DONE_ERR(DI_ERR_OK != result);
	}else
	{
		format = DI_AUDFORMAT_MPEG;
	}

	DI_UART_Print("Stream format is  0x%08X \n", format);
	DI_AUDIO_RegisterEventCallback(DI_AUDIO_DECODER_MAIN, DI_AUDIO_RUNNING_STATE_CHANGED, func_audio_callback, &ulAudioCaller);
	
	VK_TASK_Sleep(1000);
	result = DI_AUDIO_Start(deviceId, format);
	DONE_ERR(DI_ERR_OK != result);
	VK_TASK_Sleep(1000);

	DONE_ERR(TRUE != test_audio_passed);
	
	ret = UT_OK;
done:
	return ret;
}



static UT_MENU_ITEM_T	menu_audio[] =
{
	MENU_START,
	MENU_FUNC(ut_audio_getCapability, 1),
	MENU_FUNC(ut_audio_StartStopAudio, 2),
	MENU_FUNC(ut_audio_SetVolume_Mute, 2),
	MENU_FUNC(ut_audio_Pause, 5),
	MENU_FUNC(ut_audio_RegisterCallback, 5),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_audio[] =
{
	MENU_SUB(menu_audio),
};

void UT_AUDIO_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_audio)/sizeof(menu_main_audio[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_audio[i]);
	}
	array_size = sizeof(menu_audio)/sizeof(menu_audio[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_audio[i]);
	}
}


