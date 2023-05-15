#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_uart.h"
#include "di_osd.h"
#include "drv_osd.h"
#include "di_err.h"
#include "di_video.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define TEST_OSD_DEBUG
//#define TEST_OSD_ENTER_LEAVE_DEBUG
//#define TEST_EXT_DEBUG
//#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug			DI_UART_Print
#define PrintError			DI_UART_Print
#elif defined(TEST_OSD_DEBUG)
#define PrintDebug			DI_UART_Print
#define PrintError			DI_UART_Print
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			DI_UART_Print
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit				DI_UART_Print("(-)%s\n", __FUNCTION__)
#elif defined(TEST_OSD_ENTER_LEAVE_DEBUG)
#define PrintEnter				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif



#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define TEMP_FB_WIDTH		1
#define TEMP_FB_HEIGHT		130

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3
#define HWTEST_PARAM4	szParam4
#define HWTEST_PARAM5	szParam5
#define HWTEST_PARAM6	szParam6
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM4=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM5=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM6=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

/*********************************************************************
 * Extern Function
 *********************************************************************/
extern HBOOL	g_BlockPrint[DI_OSD_SURFACE_MAX_NUM];

extern unsigned char AudioOnlyStillImage[];
extern unsigned char ScrambledStillImage[];
extern unsigned long ulAudioOnlyStillImage_size;
extern unsigned long ulScrambledStillImage_size;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_OSD_HANDLE		s_TEST_OSD_HANDLE = 0;
static HUINT32				*s_TEST_OSD_BUF = 0;

static DI_OSD_HANDLE		s_TEST_OSD_HANDLE_SD = 0;
static HUINT32				*s_TEST_OSD_BUF_SD = 0;

static DI_OSD_HANDLE		s_TEST_OSD_HANDLE_LAYER2 = 0;
static HUINT32				*s_TEST_OSD_BUF_LAYER2 = 0;

#if	defined(CONFIG_OSD_LAYER_MAX_4) || defined(CONFIG_OSD_LAYER_MAX_5)
static DI_OSD_HANDLE		s_TEST_OSD_HANDLE_LAYER3 = 0;
static HUINT32				*s_TEST_OSD_BUF_LAYER3 = 0;
#endif

#if	defined(CONFIG_OSD_LAYER_MAX_5)
static DI_OSD_HANDLE		s_TEST_OSD_HANDLE_LAYER4 = 0;
static HUINT32				*s_TEST_OSD_BUF_LAYER4 = 0;
#endif



static HUINT32 ul_white_pixel 		= 0xFFFFFFFF;
static HUINT32 ul_yellow_pixel 		= 0xFFFFFF00;
static HUINT32 ul_cyan_pixel 		= 0xFF00FFFF;
static HUINT32 ul_green_pixel 		= 0xFF00FF00;
static HUINT32 ul_magenta_pixel 	= 0xFFFF00FF;
static HUINT32 ul_red_pixel 		= 0xFFFF0000;
static HUINT32 ul_blue_pixel 		= 0xFF0000FF;
static HUINT32 ul_black_pixel 		= 0xFF000000;

static HUINT32 ul_clear_pixel 		= 0x00000000;



static void INT_HwTest_OSD_Copy_Ver_ColorBar(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect)
{
	DI_OSD_RECT st_OSDRect;

	PrintEnter;

	PrintDebug("%s(%d) Surface rect is (x=%d, y=%d, w=%d, h=%d)\n", __func__, __LINE__, rect.x, rect.y, rect.width, rect.height);

	st_OSDRect.x = rect.x;
	st_OSDRect.y = rect.y;
	st_OSDRect.width = rect.width/8;
	st_OSDRect.height = rect.height;

	// 1. White Bar
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_white_pixel);

	// 2. Yellow Bar
	st_OSDRect.x += rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_yellow_pixel);

	// 3. Cyan Bar
	st_OSDRect.x += rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_cyan_pixel);

	// 4. Green Bar
	st_OSDRect.x += rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_green_pixel);

	// 5. Magenta Bar
	st_OSDRect.x +=rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_magenta_pixel);

	// 6. Red Bar
	st_OSDRect.x += rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_red_pixel);

	// 7. Blue Bar
	st_OSDRect.x += rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_blue_pixel);

	// 8. Black Bar
	st_OSDRect.x += rect.width/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_black_pixel);

	PrintExit;

	return;
}

static void INT_HwTest_OSD_Copy_Hor_ColorBar(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect, unsigned char ucPixelAlpha)
{
	DI_OSD_RECT st_OSDRect;
	unsigned long ulFillPixel;
	unsigned long ulFillAlpha;

	PrintEnter;

	PrintDebug("%s(%d) Surface rect is (x=%d, y=%d, w=%d, h=%d)\n", __func__, __LINE__, rect.x, rect.y, rect.width, rect.height);

	st_OSDRect.x = rect.x;
	st_OSDRect.y = rect.y;
	st_OSDRect.width = rect.width;
	st_OSDRect.height = rect.height/8;

	// 1. White Bar
	ulFillPixel = ul_white_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 2. Yellow Bar
	ulFillPixel = ul_yellow_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 3. Cyan Bar
	ulFillPixel = ul_cyan_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 4. Green Bar
	ulFillPixel = ul_green_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 5. Magenta Bar
	ulFillPixel = ul_magenta_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 6. Red Bar
	ulFillPixel = ul_red_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 7. Blue Bar
	ulFillPixel = ul_blue_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	// 8. Black Bar
	ulFillPixel = ul_black_pixel & 0x00FFFFFF;
	ulFillAlpha = ucPixelAlpha<<24;
	ulFillPixel |= ulFillAlpha;
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ulFillPixel);

	PrintExit;

	return;
}


DI_OSD_LAYER INT_HwTest_OSD_GetLayerFromInput(int lUserInput)
{
	DI_OSD_LAYER	nInputLayer = DI_OSD_LAYER_0;

	PrintEnter;

	if (lUserInput == 1)
		nInputLayer = DI_OSD_LAYER_1;
	else if (lUserInput == 2)
		nInputLayer = DI_OSD_LAYER_2;
#if	defined(CONFIG_OSD_LAYER_MAX_4) || defined(CONFIG_OSD_LAYER_MAX_5)
	else if (lUserInput == 3)
		nInputLayer = DI_OSD_LAYER_3;
#if	defined(CONFIG_OSD_LAYER_MAX_5)
	else if (lUserInput == 4)
		nInputLayer = DI_OSD_LAYER_4;
#endif
#elif defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	//added by dsshin(01347)
	else if (lUserInput == 3)
		nInputLayer = DI_OSD_LAYER_3;
#endif
	else
		nInputLayer = DI_OSD_LAYER_0;

	PrintExit;

	return nInputLayer;
}

int HwTest_OSD_Init(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_RECT st_OSDRect;

	PrintEnter;

	// Get or Create Main OSD Surface (1280x720 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_0, &s_TEST_OSD_HANDLE);
	if (s_TEST_OSD_HANDLE == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_WIDTH;
		st_OSDRect.height = OSD_HEIGHT;

		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_0, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE);
		AssertDI(nErr, CMD_ERR);
	}

	nErr = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE,    (void *)&s_TEST_OSD_BUF);
	AssertDI(nErr, CMD_ERR);

	// Get or Create Sub(SD) OSD Surface (720x576 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_1, &s_TEST_OSD_HANDLE_SD);
	if (s_TEST_OSD_HANDLE_SD == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_SD_WIDTH;
		st_OSDRect.height = OSD_SD_HEIGHT;

		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_1, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_SD);
		AssertDI(nErr, CMD_ERR);
	}

	nErr = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_SD, (void *)&s_TEST_OSD_BUF_SD);
	AssertDI(nErr, CMD_ERR);

	PrintExit;

	return DI_ERR_OK;
}


int HwTest_OSD_Init_Over_Layer2(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_RECT st_OSDRect;

	PrintEnter;

	// Get or Create Layer_2 OSD Surface (1280x720 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_2, &s_TEST_OSD_HANDLE_LAYER2);
	if (s_TEST_OSD_HANDLE_LAYER2 == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_WIDTH;
		st_OSDRect.height = OSD_HEIGHT;

		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_2, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER2);
		AssertDI(nErr, CMD_ERR);

		PrintDebug("[%s:%d] Layer_2 Test Surface W=%d, H=%d is Created\n", __func__, __LINE__, st_OSDRect.width, st_OSDRect.height);
	}

	nErr = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_LAYER2,    (void *)&s_TEST_OSD_BUF_LAYER2);
	AssertDI(nErr, CMD_ERR);


#if	defined(CONFIG_OSD_LAYER_MAX_4)
	// Get or Create Layer_3 OSD Surface (1280x720 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_3, &s_TEST_OSD_HANDLE_LAYER3);
	if (s_TEST_OSD_HANDLE_LAYER3 == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_WIDTH;
		st_OSDRect.height = OSD_HEIGHT;

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		nErr = DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER_3, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER3, TRUE);
#elif defined(CONFIG_SCALABLE_OSD_SURFACE)
		nErr = DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER_3, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER3, FALSE);
#else
		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_3, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER3);
#endif
		AssertDI(nErr, CMD_ERR);

		PrintDebug("[%s:%d] Layer_3 Test Surface W=%d, H=%d is Created\n", __func__, __LINE__, st_OSDRect.width, st_OSDRect.height);
	}

	nErr = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_LAYER3,    (void *)&s_TEST_OSD_BUF_LAYER3);
	AssertDI(nErr, CMD_ERR);

#elif	defined(CONFIG_OSD_LAYER_MAX_5)
	// Get or Create Layer_3 OSD Surface (1280x720 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_3, &s_TEST_OSD_HANDLE_LAYER3);
	if (s_TEST_OSD_HANDLE_LAYER3 == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_WIDTH;
		st_OSDRect.height = OSD_HEIGHT;

#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
		nErr = DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER_3, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER3, FALSE);
#else
		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_3, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER3);
#endif
		AssertDI(nErr, CMD_ERR);

		PrintDebug("[%s:%d] Layer_3 Test Surface W=%d, H=%d is Created\n", __func__, __LINE__, st_OSDRect.width, st_OSDRect.height);
	}

	nErr = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_LAYER3,    (void *)&s_TEST_OSD_BUF_LAYER3);
	AssertDI(nErr, CMD_ERR);

	// Get or Create Layer_4 OSD Surface (1280x720 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_4, &s_TEST_OSD_HANDLE_LAYER4);
	if (s_TEST_OSD_HANDLE_LAYER4 == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_WIDTH;
		st_OSDRect.height = OSD_HEIGHT;

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		nErr = DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER_4, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER4, TRUE);
#elif defined(CONFIG_SCALABLE_OSD_SURFACE)
		nErr = DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER_4, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER4, FALSE);
#else
		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_4, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_LAYER4);
#endif
		AssertDI(nErr, CMD_ERR);

		PrintDebug("[%s:%d] Layer_4 Test Surface W=%d, H=%d is Created\n", __func__, __LINE__, st_OSDRect.width, st_OSDRect.height);
	}

	nErr = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_LAYER4, (void *)&s_TEST_OSD_BUF_LAYER4);
	AssertDI(nErr, CMD_ERR);
#endif

	PrintExit;

	return DI_ERR_OK;
}

int HwTest_OSD_FlushColorBar(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo;

	PrintEnter;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);
	}

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);

	INT_HwTest_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);

#if defined(CONFIG_OS_UCOS) /* TEMP_LOADER30 */
	nErr = DI_OSD_Show(s_TEST_OSD_HANDLE);
	AssertDI(nErr, CMD_ERR);

	nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	AssertDI(nErr, CMD_ERR);
#else
	nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	AssertDI(nErr, CMD_ERR);

	nErr = DI_OSD_Show(s_TEST_OSD_HANDLE);
	AssertDI(nErr, CMD_ERR);
#endif
	PrintExit;

	return DI_ERR_OK;
}

int HwTest_OSD_ColorBar(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo;

	PrintEnter;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);
	}

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);

	INT_HwTest_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
	return DI_ERR_OK;
}


int HwTest_OSD_ClearScreen(DI_OSD_LAYER nOsdLayer)
{
	DI_ERR_CODE	nErr;
	DI_OSD_HANDLE	Surf_Handle;
	DI_OSD_INSTANCE	*pSurfInstance;
	int	i;

	PrintEnter;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);
	}

	if (nOsdLayer >= DI_OSD_LAYER_MAX)
	{
		CMD_Printf("[%s]Not Avaiable Surface(LAYER_%d) So ALL Surface Clear~~!!!\r\n", __func__, nOsdLayer);

		for (i=DI_OSD_LAYER_0; i<DI_OSD_LAYER_MAX; i++)
		{
			if(DRV_OSD_GetOsdHandle((DI_OSD_LAYER)i, &Surf_Handle) != DRV_OK)
			{
				CMD_Printf("[%s]Not Avaiable Surface(LAYER_%d)\r\n", __func__, nOsdLayer);
				return CMD_ERR;
			}

			pSurfInstance = (DI_OSD_INSTANCE *)Surf_Handle;

			// Fill surface with full rect.
			nErr = DI_OSD_FillOSDBlock(Surf_Handle, pSurfInstance->rect, ul_clear_pixel);
			AssertDI(nErr, CMD_ERR);

			nErr = DI_OSD_Flush(Surf_Handle, &pSurfInstance->rect);
			AssertDI(nErr, CMD_ERR);
		}
	}
	else
	{
		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("[%s]Not Avaiable Surface(LAYER_%d)\r\n", __func__, nOsdLayer);
			return CMD_ERR;
		}

		pSurfInstance = (DI_OSD_INSTANCE *)Surf_Handle;

		// Fill surface with full rect.
		nErr = DI_OSD_FillOSDBlock(Surf_Handle, pSurfInstance->rect, ul_clear_pixel);
		AssertDI(nErr, CMD_ERR);

		nErr = DI_OSD_Flush(Surf_Handle, &pSurfInstance->rect);
		AssertDI(nErr, CMD_ERR);
	}

	PrintExit;

	return DI_ERR_OK;
}

int HwTest_OSD_Copy(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;

	PrintEnter;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);
	}

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);
	PrintDebug("%s(%d) Test Surface SD W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo_SD.rect.width, st_TestSurfInfo_SD.rect.height);

	INT_HwTest_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, 0xFF);

	// Copy From s_TEST_OSD_HANDLE_SD To s_TEST_OSD_HANDLE with full rect.
	nErr = DI_OSD_CopyOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_CopyOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	PrintExit;

	return DI_ERR_OK;
}

int HwTest_OSD_Blend(unsigned char ucalpha)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;

	PrintEnter;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);
	}

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);
	PrintDebug("%s(%d) Test Surface SD W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo_SD.rect.width, st_TestSurfInfo_SD.rect.height);

	INT_HwTest_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
	INT_HwTest_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, ucalpha);

	// Blend : s_TEST_OSD_HANDLE_SD(Hor. ColorBar) + s_TEST_OSD_HANDLE(Ver. ColorBar) = s_TEST_OSD_HANDLE
	nErr = DI_OSD_BlendOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, 	/* source */
								s_TEST_OSD_HANDLE, st_TestSurfInfo.rect,		/* dest */
								DI_OSD_RASTER_OP_SRCOVER);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_BlendOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	PrintExit;

	return DI_ERR_OK;
}

int HwTest_OSD_Test(int	TestNumber)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;
	int	i;
	DI_OSD_RECT	st_rect, st_ScaleRect;

	PrintEnter;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);
	}

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);


	if (TestNumber == 1)
	{
		CMD_Printf("Command : osd test 1 -> OSD Fill Test \r\n");

		st_rect.x = 0;
		st_rect.y = 0;
		st_rect.width = st_TestSurfInfo.rect.width;
		st_rect.height = 1;

		for (i=0; i<st_TestSurfInfo.rect.height; i++)
		{
			st_rect.y = i;
			if (i%10 == 0)
			{
				nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_rect, ul_white_pixel);
				if(nErr != DI_ERR_OK)
				{
					PrintError("%s(%d) DI_OSD_FillOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
					return DI_ERR_ERROR;
				}

				nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_rect);
				if(nErr != DI_ERR_OK)
				{
					PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
					return DI_ERR_ERROR;
				}
			}

		}
	}
	else if (TestNumber == 2)
	{
		CMD_Printf("Command : osd test 2 -> OSD Fill & Copy Test \r\n");

		st_rect.x = 0;
		st_rect.y = 0;
		st_rect.width = 1;
		st_rect.height = st_TestSurfInfo_SD.rect.height;

		for (i=0; i<st_TestSurfInfo_SD.rect.width; i++)
		{
			st_rect.x = i;
			if (i%10 == 0)
			{
				nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE_SD, st_rect, ul_yellow_pixel);
				if(nErr != DI_ERR_OK)
				{
					PrintError("%s(%d) DI_OSD_FillOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
					return DI_ERR_ERROR;
				}

				st_ScaleRect.x = (st_rect.x* OSD_WIDTH+OSD_SD_WIDTH*0.9) / OSD_SD_WIDTH;
				st_ScaleRect.y = (st_rect.y * OSD_HEIGHT+OSD_SD_HEIGHT*0.9) / OSD_SD_HEIGHT;
				st_ScaleRect.width = (st_rect.width* OSD_WIDTH+OSD_SD_WIDTH*0.9) / OSD_SD_WIDTH;
				st_ScaleRect.height = (st_rect.height * OSD_HEIGHT+OSD_SD_HEIGHT*0.9) / OSD_SD_HEIGHT;

				/* adjust rect */
				if (st_ScaleRect.x+st_ScaleRect.width > OSD_WIDTH)
					st_ScaleRect.x = OSD_WIDTH-st_ScaleRect.width;

				if (st_ScaleRect.y+st_ScaleRect.height > OSD_HEIGHT)
					st_ScaleRect.y = OSD_HEIGHT-st_ScaleRect.height;

				// Copy From s_TEST_OSD_HANDLE_SD To s_TEST_OSD_HANDLE with full rect.
				nErr = DI_OSD_CopyOSDBlock(s_TEST_OSD_HANDLE_SD, st_rect, s_TEST_OSD_HANDLE, st_ScaleRect);
				if(nErr != DI_ERR_OK)
				{
					PrintError("%s(%d) DI_OSD_CopyOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
					return DI_ERR_ERROR;
				}

				nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
				if(nErr != DI_ERR_OK)
				{
					PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
					return DI_ERR_ERROR;
				}
			}

		}
	}
	else if (TestNumber == 3)
	{
		CMD_Printf("Command : osd test 3 -> OSD Fill & Blend Test \r\n");

		st_rect.x = st_TestSurfInfo.rect.width/4;
		st_rect.y = 0;
		st_rect.width = st_TestSurfInfo.rect.width/2;
		st_rect.height = 1;

		for (i=st_TestSurfInfo.rect.height/4; i<st_TestSurfInfo.rect.height*3/4; i++)
		{
			st_rect.y = i;

			nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_rect, ul_red_pixel);
			if(nErr != DI_ERR_OK)
			{
				PrintError("%s(%d) DI_OSD_FillOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
				return DI_ERR_ERROR;
			}

			nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_rect);
			if(nErr != DI_ERR_OK)
			{
				PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
				return DI_ERR_ERROR;
			}
		}

		st_rect.x = 0;
		st_rect.y = st_TestSurfInfo_SD.rect.height/2;
		st_rect.width = 1;
		st_rect.height = st_TestSurfInfo_SD.rect.height/2;

		for (i=st_TestSurfInfo_SD.rect.width/2; i<st_TestSurfInfo_SD.rect.width; i++)
		{
			st_rect.x = i;

			nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE_SD, st_rect, ul_blue_pixel);
			if(nErr != DI_ERR_OK)
			{
				PrintError("%s(%d) DI_OSD_FillOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
				return DI_ERR_ERROR;
			}

			st_ScaleRect.x = (st_rect.x* OSD_WIDTH+OSD_SD_WIDTH*0.9) / OSD_SD_WIDTH;
			st_ScaleRect.y = (st_rect.y * OSD_HEIGHT+OSD_SD_HEIGHT*0.9) / OSD_SD_HEIGHT;
			st_ScaleRect.width = (st_rect.width* OSD_WIDTH+OSD_SD_WIDTH*0.9) / OSD_SD_WIDTH;
			st_ScaleRect.height = (st_rect.height * OSD_HEIGHT+OSD_SD_HEIGHT*0.9) / OSD_SD_HEIGHT;

			/* adjust rect */
			if (st_ScaleRect.x+st_ScaleRect.width > OSD_WIDTH)
				st_ScaleRect.x = OSD_WIDTH-st_ScaleRect.width;

			if (st_ScaleRect.y+st_ScaleRect.height > OSD_HEIGHT)
				st_ScaleRect.y = OSD_HEIGHT-st_ScaleRect.height;

			// Copy From s_TEST_OSD_HANDLE_SD To s_TEST_OSD_HANDLE with full rect.
			// Blend : s_TEST_OSD_HANDLE_SD(Hor. ColorBar) + s_TEST_OSD_HANDLE(Ver. ColorBar) = s_TEST_OSD_HANDLE
			nErr = DI_OSD_BlendOSDBlock(s_TEST_OSD_HANDLE_SD, st_rect, 		/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			if(nErr != DI_ERR_OK)
			{
				PrintError("%s(%d) DI_OSD_BlendOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
				return DI_ERR_ERROR;
			}

			nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
			if(nErr != DI_ERR_OK)
			{
				PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
				return DI_ERR_ERROR;
			}
		}
	}
	else if (TestNumber == 4)
	{
		CMD_Printf("Command : osd test 4 -> OSD Copy & Blend Test : Yellow gradiation bar 1 width blend loop & flush\r\n");

		DI_OSD_HANDLE FB_Handle;
		HUINT32 *pFB_Addr;
		int nLineCnt, nColCnt;

		nErr = DI_OSD_CreateFrameBuffer(TEMP_FB_WIDTH, TEMP_FB_HEIGHT, DI_OSD_PIXEL_FORMAT_ARGB_8888, &FB_Handle);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_GetFrameBuffer(FB_Handle, (void *)&pFB_Addr);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nLineCnt = 0; nLineCnt < TEMP_FB_HEIGHT; nLineCnt++)
		{
			for (nColCnt = 0; nColCnt < TEMP_FB_WIDTH; nColCnt ++)
			{
				*(pFB_Addr+nLineCnt*TEMP_FB_WIDTH+nColCnt) = 0xFFFFFF00 | (nLineCnt<<1);
			}
		}

		st_rect.x = 0;
		st_rect.y = 0;
		st_rect.width = TEMP_FB_WIDTH;
		st_rect.height = TEMP_FB_HEIGHT;

		for (nColCnt = 200; nColCnt < 1200; nColCnt ++)
		{
			st_ScaleRect.x = nColCnt;
			st_ScaleRect.y = 0;
			st_ScaleRect.width = TEMP_FB_WIDTH;
			st_ScaleRect.height = TEMP_FB_HEIGHT;

			nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			AssertDI(nErr, DI_ERR_ERROR);
		}
		st_ScaleRect.x = 200;
		st_ScaleRect.width = 1000;
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nColCnt = 200; nColCnt < 1200; nColCnt ++)
		{
			st_ScaleRect.x = nColCnt;
			st_ScaleRect.y = 150;
			st_ScaleRect.width = TEMP_FB_WIDTH;
			st_ScaleRect.height = TEMP_FB_HEIGHT;

			nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			AssertDI(nErr, DI_ERR_ERROR);
		}
		st_ScaleRect.x = 200;
		st_ScaleRect.width = 1000;
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);


		for (nColCnt = 200; nColCnt < 1200; nColCnt ++)
		{
			st_ScaleRect.x = nColCnt;
			st_ScaleRect.y = 300;
			st_ScaleRect.width = TEMP_FB_WIDTH;
			st_ScaleRect.height = TEMP_FB_HEIGHT;

			nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			AssertDI(nErr, DI_ERR_ERROR);
		}
		st_ScaleRect.x = 200;
		st_ScaleRect.width = 1000;
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nColCnt = 200; nColCnt < 1200; nColCnt ++)
		{
			st_ScaleRect.x = nColCnt;
			st_ScaleRect.y = 440;
			st_ScaleRect.width = TEMP_FB_WIDTH;
			st_ScaleRect.height = TEMP_FB_HEIGHT;

			nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			AssertDI(nErr, DI_ERR_ERROR);
		}
		st_ScaleRect.x = 200;
		st_ScaleRect.width = 1000;
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nColCnt = 200; nColCnt < 1200; nColCnt ++)
		{
			st_ScaleRect.x = nColCnt;
			st_ScaleRect.y = 580;
			st_ScaleRect.width = TEMP_FB_WIDTH;
			st_ScaleRect.height = TEMP_FB_HEIGHT;

			nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			AssertDI(nErr, DI_ERR_ERROR);
		}
		st_ScaleRect.x = 200;
		st_ScaleRect.width = 1000;
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_DeleteFrameBuffer(FB_Handle);
        if ( nErr != DI_ERR_OK )
        {
            return DI_ERR_ERROR;
        }

	}
	else if (TestNumber == 5)
	{
		CMD_Printf("Command : osd test 5 -> OSD Copy & Blend Test  : Yellow gradiation bar 1 width scale blend & flush\r\n");

		DI_OSD_HANDLE FB_Handle;
		HUINT32 *pFB_Addr;
		int nLineCnt, nColCnt;

		nErr = DI_OSD_CreateFrameBuffer(TEMP_FB_WIDTH, TEMP_FB_HEIGHT, DI_OSD_PIXEL_FORMAT_ARGB_8888, &FB_Handle);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_GetFrameBuffer(FB_Handle, (void *)&pFB_Addr);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nLineCnt = 0; nLineCnt < TEMP_FB_HEIGHT; nLineCnt++)
		{
			for (nColCnt = 0; nColCnt < TEMP_FB_WIDTH; nColCnt ++)
			{
				*(pFB_Addr+nLineCnt*TEMP_FB_WIDTH+nColCnt) = 0xFFFFFF00 | (nLineCnt<<1);
			}
		}

		st_rect.x = 0;
		st_rect.y = 0;
		st_rect.width = TEMP_FB_WIDTH;
		st_rect.height = TEMP_FB_HEIGHT;

		st_ScaleRect.x = 200;
		st_ScaleRect.y = 0;
		st_ScaleRect.width = 1000;
		st_ScaleRect.height = TEMP_FB_HEIGHT;
		nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		st_ScaleRect.x = 200;
		st_ScaleRect.y = 150;
		st_ScaleRect.width = 1000;
		st_ScaleRect.height = TEMP_FB_HEIGHT;
		nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
		AssertDI(nErr, DI_ERR_ERROR);
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);


		st_ScaleRect.x = 200;
		st_ScaleRect.y = 300;
		st_ScaleRect.width = 1000;
		st_ScaleRect.height = TEMP_FB_HEIGHT;
		nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
		AssertDI(nErr, DI_ERR_ERROR);
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		st_ScaleRect.x = 200;
		st_ScaleRect.y = 440;
		st_ScaleRect.width = 1000;
		st_ScaleRect.height = TEMP_FB_HEIGHT;
		nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
		AssertDI(nErr, DI_ERR_ERROR);
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		st_ScaleRect.x = 200;
		st_ScaleRect.y = 580;
		st_ScaleRect.width = 1000;
		st_ScaleRect.height = TEMP_FB_HEIGHT;
		nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
		AssertDI(nErr, DI_ERR_ERROR);
		nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_DeleteFrameBuffer(FB_Handle);
		if ( nErr != DI_ERR_OK )
        {
            return nErr;
        }
	}
	else if (TestNumber == 6)
	{
		CMD_Printf("Command : osd test 6 -> OSD Copy & Blend LOOP Test  : Red gradiation bar 1 width blend loop & flush\r\n");

		DI_OSD_HANDLE FB_Handle;
		HUINT32 *pFB_Addr;
		int nLineCnt, nColCnt;
		int TestCount = 0;

		nErr = DI_OSD_CreateFrameBuffer(TEMP_FB_WIDTH, TEMP_FB_HEIGHT, DI_OSD_PIXEL_FORMAT_ARGB_8888, &FB_Handle);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_GetFrameBuffer(FB_Handle, (void *)&pFB_Addr);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nLineCnt = 0; nLineCnt < TEMP_FB_HEIGHT; nLineCnt++)
		{
			for (nColCnt = 0; nColCnt < TEMP_FB_WIDTH; nColCnt ++)
			{
				*(pFB_Addr+nLineCnt*TEMP_FB_WIDTH+nColCnt) = 0xFFFF0000 | (nLineCnt<<1);
			}
		}

		st_rect.x = 0;
		st_rect.y = 0;
		st_rect.width = TEMP_FB_WIDTH;
		st_rect.height = TEMP_FB_HEIGHT;

		for (TestCount=0; TestCount<100; TestCount++)
		{
			for (nColCnt = 100; nColCnt < 1200; nColCnt ++)
			{
				st_ScaleRect.x = nColCnt;
				st_ScaleRect.y = (TestCount%4)*180;
				st_ScaleRect.width = TEMP_FB_WIDTH;
				st_ScaleRect.height = TEMP_FB_HEIGHT;

				nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
											s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
											DI_OSD_RASTER_OP_SRCATOP);
				AssertDI(nErr, DI_ERR_ERROR);
			}
			st_ScaleRect.x = 100;
			st_ScaleRect.width = 1100;
			nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
			AssertDI(nErr, DI_ERR_ERROR);

			if (TestCount>0)
			{
				st_ScaleRect.y = ((TestCount-1)%4)*180;
				nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_ScaleRect, ul_clear_pixel);
				AssertDI(nErr, DI_ERR_ERROR);

				nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
				AssertDI(nErr, DI_ERR_ERROR);
			}
		}

		nErr = DI_OSD_DeleteFrameBuffer(FB_Handle);
		AssertDI(nErr, DI_ERR_ERROR);
	}
	else if (TestNumber == 7)
	{
		CMD_Printf("Command : osd test 7 -> OSD Copy & Blend LOOP Test  : Red gradiation bar 1 width scale blend & flush\r\n");

		DI_OSD_HANDLE FB_Handle;
		HUINT32 *pFB_Addr;
		int nLineCnt, nColCnt;
		int TestCount = 0;

		nErr = DI_OSD_CreateFrameBuffer(TEMP_FB_WIDTH, TEMP_FB_HEIGHT, DI_OSD_PIXEL_FORMAT_ARGB_8888, &FB_Handle);
		AssertDI(nErr, DI_ERR_ERROR);

		nErr = DI_OSD_GetFrameBuffer(FB_Handle, (void *)&pFB_Addr);
		AssertDI(nErr, DI_ERR_ERROR);

		for (nLineCnt = 0; nLineCnt < TEMP_FB_HEIGHT; nLineCnt++)
		{
			for (nColCnt = 0; nColCnt < TEMP_FB_WIDTH; nColCnt ++)
			{
				*(pFB_Addr+nLineCnt*TEMP_FB_WIDTH+nColCnt) = 0xFFFF0000 | (nLineCnt<<1);
			}
		}

		st_rect.x = 0;
		st_rect.y = 0;
		st_rect.width = TEMP_FB_WIDTH;
		st_rect.height = TEMP_FB_HEIGHT;

		for (TestCount=0; TestCount<100; TestCount++)
		{
			st_ScaleRect.x = 100;
			st_ScaleRect.y = (TestCount%4)*180;
			st_ScaleRect.width = 1100;
			st_ScaleRect.height = TEMP_FB_HEIGHT;
			nErr = DI_OSD_BlendOSDBlock(FB_Handle, st_rect, 				/* source */
										s_TEST_OSD_HANDLE, st_ScaleRect,	/* dest */
										DI_OSD_RASTER_OP_SRCATOP);
			AssertDI(nErr, DI_ERR_ERROR);

			nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
			AssertDI(nErr, DI_ERR_ERROR);

			if (TestCount>0)
			{
				st_ScaleRect.y = ((TestCount-1)%4)*180;
				nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_ScaleRect, ul_clear_pixel);
				AssertDI(nErr, DI_ERR_ERROR);

				nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_ScaleRect);
				AssertDI(nErr, DI_ERR_ERROR);
			}
		}

		nErr = DI_OSD_DeleteFrameBuffer(FB_Handle);
		if ( nErr != DI_ERR_OK )
        {
            return nErr;
        }
	}


	PrintExit;

	return DI_ERR_OK;
}

int CMD_HwTest_Parser_OSD(void *szArgs)
{
	GET_ARGS;
	int	ulLayer = 0;
	DI_OSD_LAYER	nOsdLayer;
	DI_OSD_HANDLE	Surf_Handle;
	DI_ERR_CODE	nErr;
	int 	out_x=0, out_y=0, out_w=0, out_h=0;
	DI_OSD_RECT		stInRect, stOutRect;
	int 	in_x=0, in_y=0, in_w=0, in_h=0;
#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
	int 	bg_x, bg_y, bg_w, bg_h;
	DI_OSD_RECT		stBGRect;
#endif
    out_x = out_x;
    out_y = out_y;
    out_w = out_w;
    out_h = out_h;
    stInRect.x = 0;
    stInRect.y = 0;
    stInRect.width = 0;
    stInRect.height = 0;
    stOutRect.x = 0;
    stOutRect.y = 0;
    stOutRect.width = 0;
    stOutRect.height = 0;
    in_x = in_x;
    in_y = in_y;
    in_w = in_w;
    in_h = in_h;

	if (s_TEST_OSD_HANDLE==0)
	{
		nErr = HwTest_OSD_Init();
		AssertDI(nErr, CMD_ERR);

#if	defined(CONFIG_OSD_LAYER_MAX_4) || defined(CONFIG_OSD_LAYER_MAX_5)
		nErr = HwTest_OSD_Init_Over_Layer2();
		AssertDI(nErr, CMD_ERR);
#endif
	}

	if CMD_IS("info")
	{
		CMD_Printf("Command : osd info -> Infomation of OSD\r\n");

		DRV_OSD_PrintInfo();

		iResult = CMD_OK;
	}
	else if CMD_IS("dbg")
	{
		if PARAM_IS("on")
		{
			if (READABLE_IN_DEC(HWTEST_PARAM1, ulLayer))
			{
				if (ulLayer < 0 || ulLayer >= DI_OSD_SURFACE_MAX_NUM )
					ulLayer = DI_OSD_LAYER_0;

				CMD_Printf("Command : osd dbg on %d -> ON debug print of Layer_%d\r\n", ulLayer, ulLayer);

				g_BlockPrint[ulLayer] = FALSE;
			}
			else
			{
				CMD_Printf("Command : osd dbg on -> ON debug print of All Layer\r\n");

				for (ulLayer=DI_OSD_LAYER_0; ulLayer<DI_OSD_SURFACE_MAX_NUM; ulLayer++)
				{
					g_BlockPrint[ulLayer] = FALSE;
				}
			}
		}
		else if PARAM_IS("off")
		{
			if (READABLE_IN_DEC(HWTEST_PARAM1, ulLayer))
			{
				if (ulLayer < 0 || ulLayer >= DI_OSD_SURFACE_MAX_NUM )
					ulLayer = DI_OSD_LAYER_0;

				CMD_Printf("Command : osd dbg off %d -> OFF debug print of Layer_%d\r\n", ulLayer, ulLayer);

				g_BlockPrint[ulLayer] = TRUE;
			}
			else
			{
				CMD_Printf("Command : osd dbg off -> OFF debug print of All Layer\r\n");

				for (ulLayer=DI_OSD_LAYER_0; ulLayer<DI_OSD_SURFACE_MAX_NUM; ulLayer++)
				{
					g_BlockPrint[ulLayer] = TRUE;
				}
			}
		}
		else
		{
			CMD_Printf("Command : osd dbg -> ON debug print of All Layer\r\n");

			for (ulLayer=DI_OSD_LAYER_0; ulLayer<DI_OSD_SURFACE_MAX_NUM; ulLayer++)
			{
				g_BlockPrint[ulLayer] = FALSE;
			}
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("bar")
	{
		CMD_Printf("Command : osd bar -> Display Test Vertical ColorBar\r\n");

		iResult = HwTest_OSD_FlushColorBar();
		AssertDI(iResult, CMD_ERR);
	}
	else if CMD_IS("clear")
	{


		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

			CMD_Printf("Command : osd clear %d-> Clear Layer_%d Surface \r\n", ulLayer, nOsdLayer);

			iResult = HwTest_OSD_ClearScreen(nOsdLayer);
			AssertDI(iResult, CMD_ERR);

			if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
			{
				CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
				return CMD_ERR;
			}

		}
		else
		{
			CMD_Printf("Command : osd clear ALL -> Clear ALL Layer(0/1/2) Surface \r\n");

			iResult = HwTest_OSD_ClearScreen(DI_OSD_LAYER_MAX);
			AssertDI(iResult, CMD_ERR);
		}
	}
	else if CMD_IS("refresh")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

			iResult=DRV_OSD_Refresh(nOsdLayer);
				AssertDI(iResult, CMD_ERR);
		}
		else
		{
			iResult = DRV_OSD_Refresh(DI_OSD_LAYER_0);
			AssertDI(iResult, CMD_ERR);
		}
	}

	else if CMD_IS("copy")
	{
		CMD_Printf("Command : osd copy -> OSD Copy function Test \r\n");

		iResult = HwTest_OSD_Copy();
		AssertDI(iResult, CMD_ERR);
	}
	else if CMD_IS("blend")
	{
		int				lAlpha;
		unsigned char 	ucAlpha;

		if (READABLE_IN_DEC(HWTEST_PARAM, lAlpha))
		{
			ucAlpha = (unsigned char)lAlpha;

			CMD_Printf("Command : osd blend %d -> OSD Blend function Test \r\n", ucAlpha);

			iResult = HwTest_OSD_Blend(ucAlpha);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd blend [0~255]  \r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("show")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#else
			CMD_Printf("user input ok : osd show %d\n", ulLayer);
#endif
		}
		else
		{
			ulLayer = 0;
		}

		nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
			return CMD_ERR;
		}
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#else
		CMD_Printf("Command : osd show %d -> Show OSD Layer[%d] \r\n", ulLayer, nOsdLayer);
#endif
		iResult = DI_OSD_Show(Surf_Handle);
		AssertDI(iResult, CMD_ERR);
	}
	else if CMD_IS("hide")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			CMD_Printf("user input ok : osd hide %d\n", ulLayer);
		}
		else
		{
			ulLayer = 0;
		}

		nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
			return CMD_ERR;
		}

		CMD_Printf("Command : osd hide %d -> Hide OSD Layer[%d] \r\n", ulLayer, nOsdLayer);

		iResult = DI_OSD_Hide(Surf_Handle);
		AssertDI(iResult, CMD_ERR);
	}
#if defined(CONFIG_VIDEO_DECODER)
	else if CMD_IS("st0")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			CMD_Printf("user input ok : osd st0 %d\n", ulLayer);
		}
		else
		{
			ulLayer = 0;
		}

		nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
			return CMD_ERR;
		}

		iResult = DI_VIDEO_PlayMemoryStream(0, DI_VIDEO_STREAM_TYPE_ES, DI_VIDEO_FORMAT_STILL_MPEG2, (HUINT8*)AudioOnlyStillImage, ulAudioOnlyStillImage_size, (void *)&Surf_Handle);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Command : osd st0 %d -> Display AudioOnlyStillImage to OSD Layer[%d] \r\n", ulLayer, nOsdLayer);

#if defined(CONFIG_SCALABLE_OSD_SURFACE)
		iResult = DI_OSD_SetInOutRect(Surf_Handle, NULL, NULL);
#elif defined(CONFIG_BACKGROUND_OSD_SURFACE)
		iResult = DI_OSD_SetBackGroundRect(Surf_Handle, NULL);
#else
		iResult = DRV_OSD_Refresh(nOsdLayer);
#endif
		AssertDI(iResult, CMD_ERR);

		iResult = DI_OSD_Show(Surf_Handle);
		AssertDI(iResult, CMD_ERR);
	}
	else if CMD_IS("st1")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			CMD_Printf("user input ok : osd st1 %d\n", ulLayer);
		}
		else
		{
			ulLayer = 0;
		}

		nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
			return CMD_ERR;
		}

		iResult = DI_VIDEO_PlayMemoryStream(0, DI_VIDEO_STREAM_TYPE_ES, DI_VIDEO_FORMAT_STILL_MPEG2, (HUINT8*)ScrambledStillImage, ulScrambledStillImage_size, (void *)&Surf_Handle);
		AssertDI(iResult, CMD_ERR);

		CMD_Printf("Command : osd st1 %d -> Display ScrambledStillImage to OSD Layer[%d] \r\n", ulLayer, nOsdLayer);

#if defined(CONFIG_SCALABLE_OSD_SURFACE)
		iResult = DI_OSD_SetInOutRect(Surf_Handle, NULL, NULL);
#elif defined(CONFIG_BACKGROUND_OSD_SURFACE)
		iResult = DI_OSD_SetBackGroundRect(Surf_Handle, NULL);
#else
		iResult = DRV_OSD_Refresh(nOsdLayer);
#endif
		AssertDI(iResult, CMD_ERR);

		iResult = DI_OSD_Show(Surf_Handle);
		AssertDI(iResult, CMD_ERR);
	}
#endif
#if defined(CONFIG_SCALABLE_OSD_SURFACE)
	else if CMD_IS("src")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			CMD_Printf("user input ok : osd setin about layer_%d\n", ulLayer);
		}
		else
		{
			ulLayer = 0;
		}

		nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
			return CMD_ERR;
		}

		iResult = DI_OSD_GetInOutRect(Surf_Handle, &stInRect, &stOutRect);
		AssertDI(iResult, CMD_ERR);

		if ( (READABLE_IN_DEC(HWTEST_PARAM1, in_x)) && (READABLE_IN_DEC(HWTEST_PARAM2, in_y))
					&& (READABLE_IN_DEC(HWTEST_PARAM3, in_w)) && (READABLE_IN_DEC(HWTEST_PARAM4, in_h)))
		{
			CMD_Printf("Command : osd src %d %d %d %d %d-> Set In Window of OSD Layer[%d]\n", ulLayer, in_x, in_y, in_w, in_h, nOsdLayer);

			stInRect.x = in_x;
			stInRect.y = in_y;
			stInRect.width = in_w;
			stInRect.height = in_h;
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd src [layer#]  [x] [y] [w] [h]\n");
			return CMD_ERR;
		}

		iResult = DI_OSD_SetInOutRect(Surf_Handle, &stInRect, &stOutRect);
		AssertDI(iResult, CMD_ERR);
	}
	else if CMD_IS("win")
	{
		if (READABLE_IN_DEC(HWTEST_PARAM, ulLayer))
		{
			CMD_Printf("user input ok : osd setout about layer_%d\n", ulLayer);
		}
		else
		{
			ulLayer = 0;
		}

		nOsdLayer = INT_HwTest_OSD_GetLayerFromInput(ulLayer);

		if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", nOsdLayer);
			return CMD_ERR;
		}

		iResult = DI_OSD_GetInOutRect(Surf_Handle, &stInRect, &stOutRect);
		AssertDI(iResult, CMD_ERR);

		if ( (READABLE_IN_DEC(HWTEST_PARAM1, out_x)) && (READABLE_IN_DEC(HWTEST_PARAM2, out_y))
					&& (READABLE_IN_DEC(HWTEST_PARAM3, out_w)) && (READABLE_IN_DEC(HWTEST_PARAM4, out_h)))
		{
			CMD_Printf("Command : osd win %d %d %d %d %d-> Set Out Window of OSD Layer[%d]\n", ulLayer, out_x, out_y, out_w, out_h, nOsdLayer);

			stOutRect.x = out_x;
			stOutRect.y = out_y;
			stOutRect.width = out_w;
			stOutRect.height = out_h;
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd win [layer#]  [x] [y] [w] [h]\n");
			return CMD_ERR;
		}

		iResult = DI_OSD_SetInOutRect(Surf_Handle, &stInRect, &stOutRect);
		AssertDI(iResult, CMD_ERR);
	}
#endif
#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
	else if CMD_IS("bg")
	{


		if(DRV_OSD_GetOsdHandle(DI_OSD_BG_LAYER, &Surf_Handle) != DRV_OK)
		{
			CMD_Printf("Not Avaiable Surface(LAYER_%d)\r\n", DI_OSD_BG_LAYER);
			return CMD_ERR;
		}

		iResult = DI_OSD_GetBackGroundRect(Surf_Handle, &stBGRect);
		AssertDI(iResult, CMD_ERR);

		if ( (READABLE_IN_DEC(HWTEST_PARAM, bg_x)) && (READABLE_IN_DEC(HWTEST_PARAM1, bg_y))
					&& (READABLE_IN_DEC(HWTEST_PARAM2, bg_w)) && (READABLE_IN_DEC(HWTEST_PARAM3, bg_h)))
		{
			CMD_Printf("Command : osd bg %d %d %d %d-> Set Background rect of OSD Layer[%d]\n", bg_x, bg_y, bg_w, bg_h, DI_OSD_BG_LAYER);

			stBGRect.x = bg_x;
			stBGRect.y = bg_y;
			stBGRect.width = bg_w;
			stBGRect.height = bg_h;
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd bg [x] [y] [w] [h]\n");
			return CMD_ERR;
		}

		iResult = DI_OSD_SetBackGroundRect(Surf_Handle, &stBGRect);
		AssertDI(iResult, CMD_ERR);
	}
#endif
	else if CMD_IS("test")
	{
		CMD_Printf("Command : osd test -> Test OSD Copy & Blend function \r\n");

		int 	TestNum;

		if ( READABLE_IN_DEC(HWTEST_PARAM, TestNum) )
		{
			CMD_Printf("Command : osd test %d -> Test OSD Fill(1) & Copy(2) & Blend(Other) function \r\n", TestNum);

			CHECK_BOOT_TIME("Before HwTest_OSD_Test");

			iResult = HwTest_OSD_Test(TestNum);
			AssertDI(iResult, CMD_ERR);

			CHECK_BOOT_TIME("After HwTest_OSD_Test");
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd test [%d]  \r\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("filter")
	{
		int	lSelectNum = 0;

		if (READABLE_IN_DEC(HWTEST_PARAM, lSelectNum))
		{
			CMD_Printf("Command : osd filter %d -> set OSD filter [%d] \n", lSelectNum, lSelectNum);

			DRV_OSD_SetFilterCoeffs(lSelectNum);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd filter [filter#(0~8)] \n");
			return CMD_ERR;
		}

		iResult = DRV_OSD_Refresh(DI_OSD_LAYER_0);
		AssertDI(iResult, CMD_ERR);
	}
	else if CMD_IS("fb")
	{
		int	fb_num;

		if (READABLE_IN_DEC(HWTEST_PARAM, fb_num))
		{
			DI_OSD_HANDLE FB_Handle;

			if(DRV_OSD_GetFB(fb_num, &FB_Handle) != DRV_OK)
			{
				CMD_Printf("Not Avaiable FrameBuffer(%d)\r\n", fb_num);
				return CMD_ERR;
			}

			DI_OSD_INSTANCE	*pFBInstance = (DI_OSD_INSTANCE *)FB_Handle;

			if PARAM1_IS("print")
			{
				int	w_Idx, h_Idx, nStart;
				HUINT32 *pFBAddr = (HUINT32 *)pFBInstance->pFBAddr;

				CMD_Printf("FrameBuffer(%d) :: Width(%d), Height(%d)\r\n", fb_num, pFBInstance->rect.width, pFBInstance->rect.height);

				if (READABLE_IN_DEC(HWTEST_PARAM2, nStart))
				{
					if (nStart > pFBInstance->rect.width - 10)
						nStart = pFBInstance->rect.width - 10;
				}
				else
				{
					nStart = 0;
				}

				CMD_Printf("Print Pixel From %d To %d of FB Width(%d)\r\n", nStart, nStart+9, pFBInstance->rect.width);

				for (h_Idx=0; h_Idx<pFBInstance->rect.height; h_Idx++)
				{
					for (w_Idx=0; w_Idx<pFBInstance->rect.width; w_Idx++)
					{
						if (w_Idx >= nStart && w_Idx < nStart+10 )
							CMD_Printf("%08X ", *(pFBAddr + h_Idx*pFBInstance->rect.width + w_Idx));
					}
					CMD_Printf("\n");
				}

				iResult = CMD_OK;
			}
			else if PARAM1_IS("disp")
			{
				int	disp_x, disp_y;
				DI_OSD_SURFACE_INFO st_TestSurfInfo;

				if (s_TEST_OSD_HANDLE==0)
				{
					nErr = HwTest_OSD_Init();
					AssertDI(nErr, CMD_ERR);
				}

				DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);

				if ( (READABLE_IN_DEC(HWTEST_PARAM2, disp_x)) && (READABLE_IN_DEC(HWTEST_PARAM3, disp_y)) )
				{
					if (disp_x + pFBInstance->rect.width > st_TestSurfInfo.rect.width)
						disp_x = st_TestSurfInfo.rect.width - pFBInstance->rect.width;
					if (disp_y + pFBInstance->rect.height > st_TestSurfInfo.rect.height)
						disp_y = st_TestSurfInfo.rect.height - pFBInstance->rect.height;

					CMD_Printf("Command : osd fb %d disp %d %d:: fb w(%d) h(%d)\r\n",
						fb_num, disp_x, disp_y, pFBInstance->rect.width, pFBInstance->rect.height);

					DI_OSD_RECT		disp_rect;

					disp_rect.x = disp_x;
					disp_rect.y = disp_y;
					disp_rect.width = pFBInstance->rect.width;
					disp_rect.height = pFBInstance->rect.height;

					// Copy From FB_Handle To s_TEST_OSD_HANDLE with input position.
					iResult = DI_OSD_CopyOSDBlock(FB_Handle, pFBInstance->rect, s_TEST_OSD_HANDLE, disp_rect);
					AssertDI(iResult, CMD_ERR);
				}
				else
				{
					CMD_Printf("Command : osd fb %d disp FULL :: fb w(%d) h(%d)\n",
						fb_num, pFBInstance->rect.width, pFBInstance->rect.height);

					// Copy From FB_Handle To s_TEST_OSD_HANDLE with full rect.
					iResult = DI_OSD_CopyOSDBlock(FB_Handle, pFBInstance->rect, s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
					AssertDI(iResult, CMD_ERR);
				}

				iResult = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
				AssertDI(iResult, CMD_ERR);
			}
			else if PARAM1_IS("alpha")
			{
				int	lAlpha;
				unsigned char ucAlpha;

				if (READABLE_IN_DEC(HWTEST_PARAM2, lAlpha))
				{
					ucAlpha = (unsigned char)lAlpha;
					CMD_Printf("Command : osd fb %d alpha %d :: fb[%d] multiplied alpha(%d)\n",
						fb_num, ucAlpha, fb_num, ucAlpha);

					// Multiplay alpha to FB with ucAlpha value.
					iResult = DI_OSD_SetAlphaOSDBlock(FB_Handle, pFBInstance->rect, FB_Handle, pFBInstance->rect,DI_OSD_RASTER_OP_CONSTALPHA, ucAlpha);
					AssertDI(iResult, CMD_ERR);
				}
				else
				{
					CMD_Printf("Wrong Command~! right command : osd fb [fb#] alpha [0~255]\n");
					return CMD_ERR;
				}
			}
			else
			{
				CMD_Printf("Wrong Command~! right command : osd fb [fb#] print / disp [x] [y] / alpha [0~255]\n");
				return CMD_ERR;
			}
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd fb [fb#] print / disp [x] [y] / alpha [0~255]\n");
			return CMD_ERR;
		}
	}
	else if CMD_IS("surf")
	{
		int	surf_num;

		if (READABLE_IN_DEC(HWTEST_PARAM, surf_num))
		{
			int	fill_x, fill_y, fill_w, fill_h;
			unsigned int	ulColor;

			if (surf_num == 1)
			{
				nOsdLayer = DI_OSD_LAYER_1;
				ulColor = 0xBB00FF00;	// default green
			}
			else if (surf_num == 2)
			{
				ulColor = 0xCC0000FF;	// default blue
				nOsdLayer = DI_OSD_LAYER_2;
			}
			else
			{
				nOsdLayer = DI_OSD_LAYER_0;
				ulColor = 0x55FF0000;	// default red
			}

			if(DRV_OSD_GetOsdHandle(nOsdLayer, &Surf_Handle) != DRV_OK)
			{
				CMD_Printf("Not Avaiable Surface(LAYER_%d)\n", nOsdLayer);
				return CMD_ERR;
			}

			DI_OSD_INSTANCE	*pSurfInstance = (DI_OSD_INSTANCE *)Surf_Handle;

			if PARAM1_IS("print")
			{
				int	w_Idx, h_Idx, nStart;
				HUINT32 *pFBAddr = (HUINT32 *)pSurfInstance->pFBAddr;

				CMD_Printf("LAYER_%d Surface :: Width(%d), Height(%d)\n", nOsdLayer, pSurfInstance->rect.width, pSurfInstance->rect.height);

				if (READABLE_IN_DEC(HWTEST_PARAM2, nStart))
				{
					if (nStart > pSurfInstance->rect.width - 10)
						nStart = pSurfInstance->rect.width - 10;
				}
				else
				{
					nStart = 0;
				}

				CMD_Printf("Print Pixel From %d To %d of Surface Width(%d)\n", nStart, nStart+9, pSurfInstance->rect.width);

				for (h_Idx=0; h_Idx<pSurfInstance->rect.height; h_Idx++)
				{
					for (w_Idx=0; w_Idx<pSurfInstance->rect.width; w_Idx++)
					{
						if (w_Idx >= nStart && w_Idx < nStart+10 )
							CMD_Printf("%08X ", *(pFBAddr + h_Idx*pSurfInstance->rect.width + w_Idx));
					}
					CMD_Printf("\n");
				}

				iResult = CMD_OK;
			}
			else if PARAM1_IS("fill")
			{
				if ( READABLE_IN_DEC(HWTEST_PARAM2, fill_x) && READABLE_IN_DEC(HWTEST_PARAM3, fill_y)
					&& READABLE_IN_DEC(HWTEST_PARAM4, fill_w) && READABLE_IN_DEC(HWTEST_PARAM5, fill_h)
					&& READABLE_IN_HEX(HWTEST_PARAM6, ulColor) )
				{
					if (fill_x + fill_w > pSurfInstance->rect.width)
						fill_w = pSurfInstance->rect.width - fill_x;
					if (fill_y + fill_h > pSurfInstance->rect.height)
						fill_h = pSurfInstance->rect.height - fill_y;

					CMD_Printf("Command : osd surf %d fill (%d %d %d %d) (ulColor:0x%X):: Surface w(%d) h(%d)\n",
						surf_num, fill_x, fill_y, fill_w, fill_h, ulColor, pSurfInstance->rect.width, pSurfInstance->rect.height);

					DI_OSD_RECT		fill_rect;

					fill_rect.x = fill_x;
					fill_rect.y = fill_y;
					fill_rect.width = fill_w;
					fill_rect.height = fill_h;

					// Fill surface in input position.
					iResult = DI_OSD_FillOSDBlock(Surf_Handle, fill_rect, ulColor);
					AssertDI(iResult, CMD_ERR);

					iResult = DI_OSD_Flush(Surf_Handle, &fill_rect);
					AssertDI(iResult, CMD_ERR);
				}
				else
				{
					CMD_Printf("Command : osd surf %d fill FULL ulColor(0x%X) :: surface w(%d) h(%d)\n",
						surf_num, ulColor, pSurfInstance->rect.width, pSurfInstance->rect.height);

					// Fill surface with full rect.
					iResult = DI_OSD_FillOSDBlock(Surf_Handle, pSurfInstance->rect, ulColor);
					AssertDI(iResult, CMD_ERR);

					iResult = DI_OSD_Flush(Surf_Handle, &pSurfInstance->rect);
					AssertDI(iResult, CMD_ERR);
				}
			}
			else if PARAM1_IS("flush")
			{
				if ( READABLE_IN_DEC(HWTEST_PARAM2, fill_x) && READABLE_IN_DEC(HWTEST_PARAM3, fill_y)
					&& READABLE_IN_DEC(HWTEST_PARAM4, fill_w) && READABLE_IN_DEC(HWTEST_PARAM5, fill_h))
				{
					if (fill_x + fill_w > pSurfInstance->rect.width)
						fill_w = pSurfInstance->rect.width - fill_x;
					if (fill_y + fill_h > pSurfInstance->rect.height)
						fill_h = pSurfInstance->rect.height - fill_y;

					CMD_Printf("Command : osd surf %d flush (%d %d %d %d) :: Surface w(%d) h(%d)\n",
						surf_num, fill_x, fill_y, fill_w, fill_h, pSurfInstance->rect.width, pSurfInstance->rect.height);

					DI_OSD_RECT		fill_rect;

					fill_rect.x = fill_x;
					fill_rect.y = fill_y;
					fill_rect.width = fill_w;
					fill_rect.height = fill_h;

					iResult = DI_OSD_Flush(Surf_Handle, &fill_rect);
					AssertDI(iResult, CMD_ERR);
				}
				else
				{
					CMD_Printf("Command : osd surf %d flush FULL :: surface w(%d) h(%d)\n",
						surf_num, pSurfInstance->rect.width, pSurfInstance->rect.height);

					iResult = DI_OSD_Flush(Surf_Handle, &pSurfInstance->rect);
					AssertDI(iResult, CMD_ERR);
				}
			}
			else
			{
				CMD_Printf("Wrong Command~! right command : osd surf [layer#] print / fill [x y w h color<hex>] / flush [x y w h]\n");
				return CMD_ERR;
			}
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : osd surf [layer#] print / fill [x y w h color<hex>] / flush [x y w h]\n");
			return CMD_ERR;
		}
	}
	else
	{
		CMD_Printf("Command : osd ?? -> osd info/dbg/bar/clear/copy/blend/show/hide/st0/st1/test/fb/surf\n");
#if defined(CONFIG_SCALABLE_OSD_SURFACE)
		CMD_Printf("Add Command for scalable plane: osd src/win [layer#]  [x] [y] [w] [h]\n");
#endif
#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		CMD_Printf("Add Command for background plane: osd bg [x] [y] [w] [h]\n");
#endif
	}

	return iResult;
}

#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef HWTEST_PARAM3
#undef HWTEST_PARAM4
#undef HWTEST_PARAM5
#undef HWTEST_PARAM6
#undef GET_ARGS
#undef CMD_IS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC

