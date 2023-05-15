/**
 * unittest_di_osd.c
*/

/**
 * @author		Kevin Woo
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

#include "linden_trace.h"
#include "unittest.h"


#include "htype.h"
#include "di_err.h"
#include "di_osd.h"
#include "vkernel.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_OSD

#define OSD_WIDTH			1280
#define OSD_HEIGHT			720

#define OSD_SD_WIDTH		720
#define OSD_SD_HEIGHT		576

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_OSD_HANDLE		s_TEST_OSD_HANDLE = 0;
static HUINT32				*s_TEST_OSD_BUF = 0;

static DI_OSD_HANDLE		s_TEST_OSD_HANDLE_SD = 0;
static HUINT32				*s_TEST_OSD_BUF_SD = 0;

/* pixel align -> A[31:24]-R[23:16]-G[15:8]-B[7:0] */
static HUINT32 ul_white_pixel 		= 0xFFFFFFFF;
static HUINT32 ul_yellow_pixel 		= 0xFFFFFF00;
static HUINT32 ul_cyan_pixel 		= 0xFF00FFFF;
static HUINT32 ul_green_pixel 		= 0xFF00FF00;
static HUINT32 ul_magenta_pixel 	= 0xFFFF00FF;
static HUINT32 ul_red_pixel 		= 0xFFFF0000;
static HUINT32 ul_blue_pixel 		= 0xFF0000FF;
static HUINT32 ul_black_pixel 		= 0xFF000000;

static HUINT32 ul_clear_pixel 		= 0x00000000;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static void INT_UT_OSD_Copy_Ver_ColorBar(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect)
{
	DI_OSD_RECT st_OSDRect;

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

	return;
}

static void INT_UT_OSD_Copy_Hor_ColorBar(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect, unsigned char ucPixelAlpha)
{
	DI_OSD_RECT 	st_OSDRect;
	HUINT32 		ulFillPixel;
	HUINT32 		ulFillAlpha;

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

	return;
}


static UT_RESULT ut_di_osd_init(void)
{
	DI_ERR_CODE		result;
	DI_OSD_RECT 	st_OSDRect;
	int 			ret = UT_FAIL;

	// Get or Create Main OSD Surface (1280x720 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_0, &s_TEST_OSD_HANDLE); // skkim3 : need to implemet DI func
	if (s_TEST_OSD_HANDLE == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_WIDTH;
		st_OSDRect.height = OSD_HEIGHT;

		result = DI_OSD_CreateSurface(DI_OSD_LAYER_0, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE);
		DONE_ERR(result != DI_ERR_OK);
	}
	result = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE,    (void *)&s_TEST_OSD_BUF);
	DONE_ERR(result != DI_ERR_OK);

	// Get or Create Sub(SD) OSD Surface (720x576 size)
	DRV_OSD_GetOsdHandle(DI_OSD_LAYER_1, &s_TEST_OSD_HANDLE_SD); // skkim3 : need to implemet DI func
	if (s_TEST_OSD_HANDLE_SD == 0)
	{
		st_OSDRect.x = 0;
		st_OSDRect.y = 0;
		st_OSDRect.width = OSD_SD_WIDTH;
		st_OSDRect.height = OSD_SD_HEIGHT;

		result = DI_OSD_CreateSurface(DI_OSD_LAYER_0, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_SD);
		DONE_ERR(result != DI_ERR_OK);
	}

	result = DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_SD, (void *)&s_TEST_OSD_BUF_SD);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;


}

static UT_RESULT ut_di_osd_fill(void)
{

	DI_ERR_CODE 		result;
	DI_OSD_SURFACE_INFO st_TestSurfInfo;
	DI_OSD_RECT 		st_rect;
	int 				ret = UT_FAIL;

	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DONE_ERR(result != DI_ERR_OK);

	st_rect.x = 0;
	st_rect.y = 0;
	st_rect.width = st_TestSurfInfo.rect.width;
	st_rect.height = st_TestSurfInfo.rect.height;

	result = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_rect, ul_white_pixel);
	DONE_ERR(result != DI_ERR_OK);

	result = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_rect);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;

}

static UT_RESULT ut_di_osd_fill_colorbar_ver(void)
{

	DI_ERR_CODE 		result;
	DI_OSD_SURFACE_INFO st_TestSurfInfo;
	DI_OSD_RECT 		st_rect;
	int 				ret = UT_FAIL;

	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DONE_ERR(result != DI_ERR_OK);

	st_rect.x = 0;
	st_rect.y = 0;
	st_rect.width = st_TestSurfInfo.rect.width;
	st_rect.height = st_TestSurfInfo.rect.height;

	INT_UT_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);

	result = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_rect);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;


}

static UT_RESULT ut_di_osd_fill_colorbar_hor(void)
{
	DI_ERR_CODE 		result;
	DI_OSD_SURFACE_INFO st_TestSurfInfo;
	DI_OSD_RECT 		st_rect;
	int 				ret = UT_FAIL;

	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DONE_ERR(result != DI_ERR_OK);

	st_rect.x = 0;
	st_rect.y = 0;
	st_rect.width = st_TestSurfInfo.rect.width;
	st_rect.height = st_TestSurfInfo.rect.height;

	INT_UT_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect, 0xFF);

	result = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_rect);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;

}

UT_RESULT ut_di_osd_copy(void)
{
	DI_ERR_CODE			result;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;
	int 				ret = UT_FAIL;

	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DONE_ERR(result != DI_ERR_OK);
	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);
	DONE_ERR(result != DI_ERR_OK);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);
	PrintDebug("%s(%d) Test Surface SD W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo_SD.rect.width, st_TestSurfInfo_SD.rect.height);

	INT_UT_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, 0xFF);

	// Copy From s_TEST_OSD_HANDLE_SD To s_TEST_OSD_HANDLE with full rect.
	result = DI_OSD_CopyOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
	DONE_ERR(result != DI_ERR_OK);

	result = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;

}

UT_RESULT ut_di_osd_blend(void)
{
	DI_ERR_CODE			result;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;
	int 				ret = UT_FAIL;

	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DONE_ERR(result != DI_ERR_OK);
	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);
	DONE_ERR(result != DI_ERR_OK);

	INT_UT_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
	INT_UT_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, 0x88);

	// Blend : s_TEST_OSD_HANDLE_SD(Hor. ColorBar) + s_TEST_OSD_HANDLE(Ver. ColorBar) = s_TEST_OSD_HANDLE
	result = DI_OSD_BlendOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, 	/* source */
								s_TEST_OSD_HANDLE, st_TestSurfInfo.rect,		/* dest */
								DI_OSD_RASTER_OP_SRCATOP);
	DONE_ERR(result != DI_ERR_OK);

	result = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;

}

static UT_RESULT ut_di_osd_clear_screen(void)
{
	DI_ERR_CODE			result;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;
	int 				ret = UT_FAIL;

	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DONE_ERR(result != DI_ERR_OK);
	result = DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);
	DONE_ERR(result != DI_ERR_OK);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);
	PrintDebug("%s(%d) Test Surface SD W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo_SD.rect.width, st_TestSurfInfo_SD.rect.height);

	// Clear framebuffer
	result = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect, ul_clear_pixel);
	DONE_ERR(result != DI_ERR_OK);

	result = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, ul_clear_pixel);
	DONE_ERR(result != DI_ERR_OK);

	result = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret;

}

static UT_MENU_ITEM_T menu_osd[] =
{
	MENU_START,
	MENU_FUNC(ut_di_osd_init,1),
	MENU_FUNC(ut_di_osd_fill,1),
	MENU_FUNC(ut_di_osd_fill_colorbar_ver,1),
	MENU_FUNC(ut_di_osd_fill_colorbar_hor,1),
	MENU_FUNC(ut_di_osd_copy,1),
	MENU_FUNC(ut_di_osd_blend,1),
	MENU_FUNC(ut_di_osd_clear_screen,1),
	MENU_END,
};

static UT_MENU_ITEM_T menu_main_osd[] =
{
	MENU_SUB(menu_osd),
};

void UT_OSD_Init(void)
{
	HINT32 	i;
	HINT32	array_size;

	array_size = sizeof(menu_main_osd)/sizeof(menu_main_osd[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_osd[i]);
	}

	array_size = sizeof(menu_osd)/sizeof(menu_osd[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_osd[i]);
	}
}


