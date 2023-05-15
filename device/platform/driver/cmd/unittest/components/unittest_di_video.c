/**
 * unittest_di_video.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"
#include "di_uart.h"
#include "di_err.h"
#include "di_video.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_VIDEO
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

const HINT8 *ut_makeDiAspectRatioToStr(DI_VIDEO_AspectRatio_t eAspectRatio)
{
	switch(eAspectRatio)
	{
		ENUM_TO_STR(DI_VIDEO_ASPECTRATIO_16_9);
		ENUM_TO_STR(DI_VIDEO_ASPECTRATIO_4_3);
		ENUM_TO_STR(DI_VIDEO_ASPECTRATIO_4_3_LETTER);
		ENUM_TO_STR(DI_VIDEO_ASPECTRATIO_14_9);
		ENUM_TO_STR(DI_VIDEO_ASPECTRATIO_1_1);
		ENUM_TO_STR(DI_VIDEO_ASPECTRATIO_SAR);
		default: break;
	}
	return ("Unknown");
}

const HINT8 *ut_makeDiFrameRateToStr(DI_VIDEO_FrameRate_t eFrameRate)
{
	switch(eFrameRate)
	{
		ENUM_TO_STR(DI_VIDEO_FRAMERATE_24);
		ENUM_TO_STR(DI_VIDEO_FRAMERATE_25);
		ENUM_TO_STR(DI_VIDEO_FRAMERATE_30);
		ENUM_TO_STR(DI_VIDEO_FRAMERATE_50);
		ENUM_TO_STR(DI_VIDEO_FRAMERATE_60);
		default: break;
	}
	return ("Unknown");
}

const HINT8 *ut_makeDiVideoFormatToStr(DI_VIDEO_FORMAT eFrameRate)
{
	switch(eFrameRate)
	{
		ENUM_TO_STR(DI_VIDEO_FORMAT_MPEG1);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MPEG2);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MPEG4Part2);
		ENUM_TO_STR(DI_VIDEO_FORMAT_H261);
		ENUM_TO_STR(DI_VIDEO_FORMAT_H263);
		ENUM_TO_STR(DI_VIDEO_FORMAT_H264);
		ENUM_TO_STR(DI_VIDEO_FORMAT_VC1SimpleMain);
		ENUM_TO_STR(DI_VIDEO_FORMAT_VC1);
		ENUM_TO_STR(DI_VIDEO_FORMAT_AVS);
		ENUM_TO_STR(DI_VIDEO_FORMAT_DIVX_311);
		ENUM_TO_STR(DI_VIDEO_FORMAT_STILL_MPEG2);
		ENUM_TO_STR(DI_VIDEO_FORMAT_STILL_H264);
		ENUM_TO_STR(DI_VIDEO_FORMAT_STILL_VC1);
		ENUM_TO_STR(DI_VIDEO_FORMAT_H265);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_MPEG1);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_MPEG2);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_MPEG4Part2);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_H261);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_H263);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_H264);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_VC1);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_AVS);
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_DIVX_311);	
		ENUM_TO_STR(DI_VIDEO_FORMAT_MEDIA_H265);		
		default: break;
	}
	return ("Unknown");	
}

static UT_RESULT ut_getCapability(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDevice;
	HUINT32 i;
	DI_VIDEO_CAP_t *stVideoCap;

	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);

	DI_UART_Print("\n\n  > ulNumOfDevice : %d\n", ulNumOfDevice);

	for(i = 0; i < ulNumOfDevice; i++)
	{
		DI_UART_Print("\n\n  > stVideoCap[%d].ulGroupId : %d\n", i, stVideoCap[i].ulGroupId);
		DI_UART_Print("  > stVideoCap[%d].ulDecoderId : %d\n", i, stVideoCap[i].ulDecoderId);
		DI_UART_Print("  > stVideoCap[%d].eDecoderType : %d\n", i, stVideoCap[i].eDecoderType);
		DI_UART_Print("  > stVideoCap[%d].eSupportedFormat : 0x%x\n", i, stVideoCap[i].eSupportedFormat);
		DI_UART_Print("  > stVideoCap[%d].ulPrimerCount : %d\n", i, stVideoCap[i].ulPrimerCount);
	}

	ret = UT_OK;
done:
	return ret;
}

static UT_RESULT ut_getSequenceHeaderInfo(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	DI_VIDEO_StreamInfo_t stStreamInfo;

	result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
	DONE_ERR(DI_ERR_OK != result);

	DI_UART_Print("\n\n  > stStreamInfo.ulWidth : %d\n", stStreamInfo.ulWidth);
	DI_UART_Print("  > stStreamInfo.ulHeight : %d\n", stStreamInfo.ulHeight);
	DI_UART_Print("  > stStreamInfo.ulDisplayWidth : %d\n", stStreamInfo.ulDisplayWidth);
	DI_UART_Print("  > stStreamInfo.ulDisplayHeightId : %d\n", stStreamInfo.ulDisplayHeight);
	DI_UART_Print("  > stStreamInfo.eAspectRatio : %s\n", ut_makeDiAspectRatioToStr(stStreamInfo.eAspectRatio));
	DI_UART_Print("  > stStreamInfo.eFrameRate : %s\n", ut_makeDiFrameRateToStr(stStreamInfo.eFrameRate));
	DI_UART_Print("  > stStreamInfo.eVideoFormat : %s\n", ut_makeDiVideoFormatToStr(stStreamInfo.eVideoFormat));
	DI_UART_Print("  > stStreamInfo.bProgressive : %d\n", stStreamInfo.bProgressive);
	DI_UART_Print("  > stStreamInfo.ucChromaFormat : %d\n", stStreamInfo.ucChromaFormat);
	DI_UART_Print("  > stStreamInfo.ucVideoFormat : %d\n", stStreamInfo.ucVideoFormat);
	DI_UART_Print("  > stStreamInfo.ucColorPrimarie : %d\n", stStreamInfo.ucColorPrimarie);
	DI_UART_Print("  > stStreamInfo.ucAlpha : %d\n", stStreamInfo.ucAlpha);
	
	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_stop(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDevice;
	DI_VIDEO_CAP_t *stVideoCap;
	DI_VIDEO_RUNNING_STATE videoState;
	DI_VIDEO_StreamInfo_t stStreamInfo;

	
	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);
	
	result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
	DONE_ERR(DI_ERR_OK != result);

	//Abnormal case
	result = DI_VIDEO_Stop(-1);
	DONE_ERR(DI_ERR_OK == result);

	//Abnormal case
	result = DI_VIDEO_Stop(ulNumOfDevice + 1);
	DONE_ERR(DI_ERR_OK == result);

	result = DI_VIDEO_GetState(0 ,&videoState);
	DONE_ERR(DI_ERR_OK != result);

	if(videoState != DI_VIDEO_RUNNING_STATE_DECODING)
	{
		result = DI_VIDEO_Start(0, stStreamInfo.eVideoFormat);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}

	result = DI_VIDEO_Stop(0);
	DONE_ERR(DI_ERR_OK != result);

	VK_TASK_Sleep(1000);

	ret = UT_OK;
done:
	return ret;
}

static UT_RESULT ut_start(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	DI_VIDEO_StreamInfo_t stStreamInfo;
	DI_VIDEO_RUNNING_STATE videoState;
	HUINT32 ulNumOfDevice;
	DI_VIDEO_CAP_t *stVideoCap;
	
		
	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);

	result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
	DONE_ERR(DI_ERR_OK != result);

	result = DI_VIDEO_GetState(0 ,&videoState);
	DONE_ERR(DI_ERR_OK != result);

	//Abnormal case
	result = DI_VIDEO_Start(-1, stStreamInfo.eVideoFormat);
	DONE_ERR(DI_ERR_OK == result);

	//Abnormal case
	result = DI_VIDEO_Start(ulNumOfDevice + 1, stStreamInfo.eVideoFormat);
	DONE_ERR(DI_ERR_OK == result);

	if(videoState == DI_VIDEO_RUNNING_STATE_DECODING)
	{
		result = DI_VIDEO_Stop(0);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}	
	
	result = DI_VIDEO_Start(0, stStreamInfo.eVideoFormat);
	DONE_ERR(DI_ERR_OK != result);
	
	VK_TASK_Sleep(1000);

	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_show(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDevice;
	DI_VIDEO_CAP_t *stVideoCap;
	DI_VIDEO_RUNNING_STATE videoState;
	DI_VIDEO_StreamInfo_t stStreamInfo;
	HUINT32 i;
	HBOOL flag;
		
	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);

	flag = TRUE;

	//Abnormal case
	result = DI_VIDEO_Show(-1, flag);
	DONE_ERR(DI_ERR_OK == result);

	//Abnormal case
	result = DI_VIDEO_Show(ulNumOfDevice+1, flag);
	DONE_ERR(DI_ERR_OK == result);

	// Get state of video decoder
	result = DI_VIDEO_GetState(0 ,&videoState);
	DONE_ERR(DI_ERR_OK != result);

	// Get stream info
	result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
	DONE_ERR(DI_ERR_OK != result);

	if(videoState != DI_VIDEO_RUNNING_STATE_DECODING)
	{
		result = DI_VIDEO_Start(0, stStreamInfo.eVideoFormat);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}
		
	
	for(i = 0; i<20; i++)
	{
		flag ^= TRUE; //TRUE -> FALSE ; FALSE -> TRUE
		result = DI_VIDEO_Show(0, flag);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}

	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_pause(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDevice;
	DI_VIDEO_CAP_t *stVideoCap;
	DI_VIDEO_RUNNING_STATE videoState;
	DI_VIDEO_StreamInfo_t stStreamInfo;
	HUINT32 i;
	HBOOL flag;
		
	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);

	flag = FALSE;

	//Abnormal case
	result = DI_VIDEO_Pause(-1, flag);
	DONE_ERR(DI_ERR_OK == result);

	//Abnormal case
	result = DI_VIDEO_Pause(ulNumOfDevice+1, flag);
	DONE_ERR(DI_ERR_OK == result);

	// Get state of video decoder
	result = DI_VIDEO_GetState(0 ,&videoState);
	DONE_ERR(DI_ERR_OK != result);

	// Get stream info
	result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
	DONE_ERR(DI_ERR_OK != result);

	if(videoState != DI_VIDEO_RUNNING_STATE_DECODING)
	{
		result = DI_VIDEO_Start(0, stStreamInfo.eVideoFormat);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}
	
	for(i = 0; i<20; i++)
	{
		flag ^= TRUE;//TRUE -> FALSE ; FALSE -> TRUE
		result = DI_VIDEO_Pause(0, flag);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}

	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_setGround(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDevice;
	DI_VIDEO_CAP_t *stVideoCap;
	DI_VIDEO_RUNNING_STATE videoState;
	DI_VIDEO_StreamInfo_t stStreamInfo;
	HUINT32 i;
	HBOOL flag;
	DI_VIDEO_COLOR_t color;
		
	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);

	flag = FALSE;
	
	// Get state of video decoder
	result = DI_VIDEO_GetState(0 ,&videoState);
	DONE_ERR(DI_ERR_OK != result);

	if(videoState != DI_VIDEO_RUNNING_STATE_DECODING)
	{
		// Get stream info
		result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
		DONE_ERR(DI_ERR_OK != result);
	
		result = DI_VIDEO_Start(0, stStreamInfo.eVideoFormat);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}
	
	for(i = 0; i<10; i++)
	{
		flag ^= TRUE;//TRUE -> FALSE ; FALSE -> TRUE
		result = DI_VIDEO_SetBlank(0, flag);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}

	color.a = 0;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	
	for(i = 0; i<10; i++)
	{
		result = DI_VIDEO_SetPlaneAlpha(0, color.a);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
		color.a += 50;
	}

	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_setInOutWindow(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDevice;
	DI_VIDEO_CAP_t *stVideoCap;
	DI_VIDEO_RUNNING_STATE videoState;
	DI_VIDEO_StreamInfo_t stStreamInfo;
	HUINT32 i;
	HBOOL flag;
	DI_VIDEO_RECT_t InRect;
	DI_VIDEO_RECT_t OutRect;
	DI_VIDEO_WIN_TYPE eType = DI_VIDEO_WIN_HD;
	DI_VIDEO_COLOR_t color;

		
	result = DI_VIDEO_GetCapability(&ulNumOfDevice, &stVideoCap);
	DONE_ERR(DI_ERR_OK != result);

	flag = FALSE;
	
	// Get state of video decoder
	result = DI_VIDEO_GetState(0 ,&videoState);
	DONE_ERR(DI_ERR_OK != result);

	if(videoState != DI_VIDEO_RUNNING_STATE_DECODING)
	{
		// Get stream info
		result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
		DONE_ERR(DI_ERR_OK != result);
	
		result = DI_VIDEO_Start(0, stStreamInfo.eVideoFormat);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}

	
	InRect.ulX=0;
	InRect.ulY=0;
	InRect.ulWidth=500;
	InRect.ulHeight=500;
	OutRect.ulX=500;
	OutRect.ulY=500;
	OutRect.ulWidth=500;
	OutRect.ulHeight=500;

	// Get stream info
	result = DI_VIDEO_GetSequenceHeaderInfo(0, &stStreamInfo);
	DONE_ERR(DI_ERR_OK != result);

	result = DI_VIDEO_SetInOutWindow(0, &InRect, &OutRect, &flag, eType);
	DONE_ERR(DI_ERR_OK != result);

	if(InRect.ulWidth == stStreamInfo.ulWidth || InRect.ulHeight == stStreamInfo.ulHeight)
		DONE_ERR(flag != TRUE);

	DI_UART_Print("bTouched = %d \n", flag);
	VK_TASK_Sleep(3000);
	

	InRect.ulX=0;
	InRect.ulY=0;
	InRect.ulWidth=stStreamInfo.ulWidth;
	InRect.ulHeight=stStreamInfo.ulHeight;

	result = DI_VIDEO_SetInOutWindow(0, &InRect, &OutRect, &flag, eType);
	DONE_ERR(DI_ERR_OK != result);
	DI_UART_Print("bTouched = %d \n", flag);
	if(InRect.ulWidth == stStreamInfo.ulWidth || InRect.ulHeight == stStreamInfo.ulHeight)
		DONE_ERR(flag != TRUE);
	
	VK_TASK_Sleep(3000);

	color.a = 0;
	color.r = 0;
	color.g = 50;
	color.b = 100;
	
	for(i = 0; i<10; i++)
	{
		result = DI_VIDEO_SetBackground(0, color);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
		color.a += 50;
		color.r += 10;
		color.g += 10;
		color.b += 10;
	}
	
	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_setDisplayStandard(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 i;
	
	DI_VIDEO_HD_RESOLUTION reso;

	result = DI_VIDEO_SetDisplayStandard(-1);
	//Converted to 1080i@50
	DONE_ERR(DI_ERR_OK != result);
	VK_TASK_Sleep(1000);

	result = DI_VIDEO_SetDisplayStandard(DI_VIDEO_HD_RESOLUTION_MAX+1);
	//Converted to 1080i@50
	DONE_ERR(DI_ERR_OK != result);
	VK_TASK_Sleep(1000);


	for(i=0; i<=DI_VIDEO_HD_RESOLUTION_MAX; i++)
	{
		result = DI_VIDEO_SetDisplayStandard(i);
		DONE_ERR(DI_ERR_OK != result);
		VK_TASK_Sleep(1000);
	}
	
	ret = UT_OK;
done:
	return ret;

}


static UT_MENU_ITEM_T	menu_video[] =
{
	MENU_START,
	MENU_FUNC(ut_getCapability, 1),
	MENU_FUNC(ut_getSequenceHeaderInfo, 1),
	MENU_FUNC(ut_start, 2),
	MENU_FUNC(ut_show, 20),
	MENU_FUNC(ut_pause, 20),
	MENU_FUNC(ut_setGround, 20),
	MENU_FUNC(ut_setDisplayStandard, 20),
	MENU_FUNC(ut_setInOutWindow, 20),
	MENU_FUNC(ut_stop, 2),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_video[] =
{
	MENU_SUB(menu_video),
};

void UT_VIDEO_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_video)/sizeof(menu_main_video[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_video[i]);
	}
	array_size = sizeof(menu_video)/sizeof(menu_video[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_video[i]);
	}
}

