/**
 * test_main.c
*/

/**
 * @defgroup		TEST_MAIN TEST_MAIN : TEST Main
 * @author		Jin-hong Kim
 * @note			test main
 * @brief			test main
 * @file 			test_main.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linden_trace.h"

#include "htype.h"
#include "di_err.h"
#include "di_osd.h"
#include "vkernel.h"

#if	0
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_OSD_DEBUG
#define TEST_OSD_ENTER_LEAVE_DEBUG

#ifdef TEST_OSD_DEBUG
#define PrintDebug			printf
#define PrintError			printf
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			printf
#endif

#ifdef TEST_OSD_ENTER_LEAVE_DEBUG
#define PrintEnter				printf("(+)%s\n", __FUNCTION__)
#define PrintExit				printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

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

#if	0
/* pixel align -> A[31:24]-R[23:16]-G[15:8]-B[7:0] */
static HUINT32 white_pixel[] 	= {0xFFFFFFFF};
static HUINT32 yellow_pixel[] 	= {0xFFFFFF00};
static HUINT32 cyan_pixel[] 	= {0xFF00FFFF};
static HUINT32 green_pixel[] 	= {0xFF00FF00};
static HUINT32 magenta_pixel[] 	= {0xFFFF00FF};
static HUINT32 red_pixel[] 		= {0xFFFF0000};
static HUINT32 blue_pixel[] 	= {0xFF0000FF};
static HUINT32 black_pixel[] 	= {0xFF000000};
#else
static HUINT32 ul_white_pixel 		= 0xFFFFFFFF;
static HUINT32 ul_yellow_pixel 		= 0xFFFFFF00;
static HUINT32 ul_cyan_pixel 		= 0xFF00FFFF;
static HUINT32 ul_green_pixel 		= 0xFF00FF00;
static HUINT32 ul_magenta_pixel 	= 0xFFFF00FF;
static HUINT32 ul_red_pixel 		= 0xFFFF0000;
static HUINT32 ul_blue_pixel 		= 0xFF0000FF;
static HUINT32 ul_black_pixel 		= 0xFF000000;
#endif

static HUINT32 ul_clear_pixel 		= 0x00000000;
#endif
/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static void TEST_OSD_SelectTestCase(HUINT32 ulTestNumber);
static void TEST_OSD_Menu(void);

#if	0
int TEST_OSD_Init(void);
int TEST_OSD_FlushColorBar(void);
int TEST_OSD_ClearScreen(void);
int TEST_OSD_Copy(void);
int TEST_OSD_Blend(void);


static void INT_TEST_OSD_Copy_Ver_ColorBar(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect)
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

static void INT_TEST_OSD_Copy_Hor_ColorBar(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect)
{
	DI_OSD_RECT st_OSDRect;

	PrintEnter;

	PrintDebug("%s(%d) Surface rect is (x=%d, y=%d, w=%d, h=%d)\n", __func__, __LINE__, rect.x, rect.y, rect.width, rect.height);

	st_OSDRect.x = rect.x;
	st_OSDRect.y = rect.y;
	st_OSDRect.width = rect.width;
	st_OSDRect.height = rect.height/8;

	// 1. White Bar
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_white_pixel);

	// 2. Yellow Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_yellow_pixel);

	// 3. Cyan Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_cyan_pixel);

	// 4. Green Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_green_pixel);

	// 5. Magenta Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_magenta_pixel);

	// 6. Red Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_red_pixel);

	// 7. Blue Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_blue_pixel);

	// 8. Black Bar
	st_OSDRect.y += rect.height/8;
	DI_OSD_FillOSDBlock(hOsdHandle, st_OSDRect, ul_black_pixel);

	PrintExit;

	return;
}
#endif

void TEST_DI_OSD(void)
{
	DI_ERR_CODE ulRet = 0;
	HUINT32 ulTestNumber = 0;

    do {
        TEST_OSD_Menu(); /* print menu */

        ulRet = TEST_GetNumber(&ulTestNumber);
		if (ulRet == DI_ERR_ERROR || ulTestNumber == 0)
		{
			/* */
			break;
		}
		else if (ulRet == DI_ERR_OK)
		{
            TEST_OSD_SelectTestCase(ulTestNumber);
        }
        else
        {
            printf( "Unknown key Code(%d)!!\n", ulTestNumber);
        }
    } while (1);

}



static void TEST_OSD_Menu(void)
{

	printf( "*============================================================*\n"); /* 70 */
	printf( "*    DD TEST TOOL/CHANNEL                                    *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    00. Quit                                                *\n"); /* 70 */
	printf( "*    01. TEST_OSD_Init                                       *\n"); /* 70 */
	printf( "*    02. Display Color Bar                                   *\n"); /* 70 */
	printf( "*    03. Clear Screen & Surface                              *\n"); /* 70 */
	printf( "*    04. TEST_OSD_Copy                                       *\n"); /* 70 */
	printf( "*    05. TEST_OSD_Blend                                      *\n"); /* 70 */
	printf( "*    06. DI_OSD_DeleteRegion                                 *\n"); /* 70 */
	printf( "*    07. DI_OSD_GetFrameBuffer                               *\n"); /* 70 */
	printf( "*    08. DI_OSD_GetFrameBuffer_ForSDSize                     *\n"); /* 70 */
	printf( "*    09. DI_OSD_ChangePalette                                *\n"); /* 70 */
	printf( "*    10. DI_OSD_ChangeColor                                  *\n"); /* 70 */
	printf( "*    11. DI_OSD_BlendOSDBlock                                *\n"); /* 70 */
	printf( "*    12. DI_OSD_CopyOSDBlock                                 *\n"); /* 70 */
	printf( "*    13. DI_OSDFlushScreen                                   *\n"); /* 70 */
	printf( "*    14. DI_OSD_CreatePalette                                *\n"); /* 70 */
	printf( "*    15. DI_OSD_DeletePalette                                *\n"); /* 70 */
	printf( "*------------------------------------------------------------*\n"); /* 70 */
	printf( "*    Select Number & Press OK                                *\n"); /* 70 */
	printf( "*============================================================*\n"); /* 70 */


}



static void TEST_OSD_SelectTestCase(HUINT32 ulTestNumber)
{
    switch( ulTestNumber ) {

	case 1 :
		//TEST_OSD_Init();
        break;

	case 2 :
		//TEST_OSD_FlushColorBar();
        break;

	case 3 :
		//TEST_OSD_ClearScreen();
        break;

	case 4 :
		//TEST_OSD_Copy();
        break;

	case 5 :
		//TEST_OSD_Blend();
        break;

	case 6 :
        break;

	case 7 :
        break;

	case 8 :
        break;

	case 9 :
        break;

	case 10 :
        break;

	case 11 :
        break;

	case 12 :
        break;

	case 13 :
        break;

	case 14 :
        break;

	case 15 :
        break;

	default :
        break;
    }

}

#if	0
int TEST_OSD_Init(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_RECT st_OSDRect;

	PrintEnter;

	nErr = DI_OSD_Init();
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_Init() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	nErr = DI_OSD_SetPlaneAlpha(DI_OSD_LAYER_0, 0xFF);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_SetPlaneAlpha() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	nErr = DI_OSD_Show(s_TEST_OSD_HANDLE);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_SetPlaneAlpha() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	st_OSDRect.x = 0;
	st_OSDRect.y = 0;
	st_OSDRect.width = OSD_WIDTH;
	st_OSDRect.height = OSD_HEIGHT;

	nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_0, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_CreateSurface() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE,    (void *)&s_TEST_OSD_BUF);

	st_OSDRect.x = 0;
	st_OSDRect.y = 0;
	st_OSDRect.width = OSD_SD_WIDTH;
	st_OSDRect.height = OSD_SD_HEIGHT;

	nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_0, st_OSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &s_TEST_OSD_HANDLE_SD);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_CreateSurface() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	DI_OSD_GetFrameBuffer(s_TEST_OSD_HANDLE_SD, (void *)&s_TEST_OSD_BUF_SD);

	PrintExit;

	return DI_ERR_OK;
}

int TEST_OSD_FlushColorBar(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo;

	PrintEnter;

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);

#if	0
	int i, j;

	for (i = 0; i < OSD_HEIGHT; i++)
	{
		for (j=0; j<OSD_WIDTH; j++)
		{
			if ( j<= (OSD_WIDTH/8) )										// White Bar
			{
				VK_memcpy(s_TEST_OSD_BUF+ j + i*OSD_WIDTH, white_pixel, 4);
			}
			else if ( j> (OSD_WIDTH/8) && j<= (2*OSD_WIDTH/8) )				//Yellow Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, yellow_pixel, 4);
			}
			else if ( j> (2*OSD_WIDTH/8) && j<= (3*OSD_WIDTH/8) )			//Cyan Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, cyan_pixel, 4);
			}
			else if ( j> (3*OSD_WIDTH/8) && j<= (4*OSD_WIDTH/8) )			//Green Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, green_pixel, 4);
			}
			else if ( j> (4*OSD_WIDTH/8) && j<= (5*OSD_WIDTH/8) )			//Magenta Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, magenta_pixel, 4);
			}
			else if ( j> (5*OSD_WIDTH/8) && j<= (6*OSD_WIDTH/8) )			//Red Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, red_pixel, 4);
			}
			else if ( j> (6*OSD_WIDTH/8) && j<= (7*OSD_WIDTH/8) )			//Blue Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, blue_pixel, 4);
			}
			else															//Black Bar
			{
				VK_memcpy(s_TEST_OSD_BUF + j + i*OSD_WIDTH, black_pixel, 4);
			}
		}
	}
#else

	INT_TEST_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);

#endif

	nErr = DI_OSD_Flush(s_TEST_OSD_HANDLE, &st_TestSurfInfo.rect);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_Flush() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	PrintExit;

	return DI_ERR_OK;
}

int TEST_OSD_ClearScreen(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;

	PrintEnter;

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);

	// Clear framebuffer
	nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect, ul_clear_pixel);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_FillOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
		return DI_ERR_ERROR;
	}

	nErr = DI_OSD_FillOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, ul_clear_pixel);
	if(nErr != DI_ERR_OK)
	{
		PrintError("%s(%d) DI_OSD_FillOSDBlock() err:0x%x\n\r", __func__, __LINE__, nErr);
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

int TEST_OSD_Copy(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;

	PrintEnter;

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);
	PrintDebug("%s(%d) Test Surface SD W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo_SD.rect.width, st_TestSurfInfo_SD.rect.height);

	INT_TEST_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect);

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

int TEST_OSD_Blend(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_SURFACE_INFO st_TestSurfInfo, st_TestSurfInfo_SD;

	PrintEnter;

	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE, &st_TestSurfInfo);
	DI_OSD_GetSurfaceInfo(s_TEST_OSD_HANDLE_SD, &st_TestSurfInfo_SD);

	PrintDebug("%s(%d) Test Surface W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo.rect.width, st_TestSurfInfo.rect.height);
	PrintDebug("%s(%d) Test Surface SD W=%d, H=%d\n", __func__, __LINE__, st_TestSurfInfo_SD.rect.width, st_TestSurfInfo_SD.rect.height);

	INT_TEST_OSD_Copy_Ver_ColorBar(s_TEST_OSD_HANDLE, st_TestSurfInfo.rect);
	INT_TEST_OSD_Copy_Hor_ColorBar(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect);

	// Blend : s_TEST_OSD_HANDLE_SD(Hor. ColorBar) + s_TEST_OSD_HANDLE(Ver. ColorBar) = s_TEST_OSD_HANDLE
	nErr = DI_OSD_BlendOSDBlock(s_TEST_OSD_HANDLE_SD, st_TestSurfInfo_SD.rect, 	/* source */
								s_TEST_OSD_HANDLE, st_TestSurfInfo.rect,		/* dest */
								DI_OSD_RASTER_OP_SRCATOP);
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
#endif


