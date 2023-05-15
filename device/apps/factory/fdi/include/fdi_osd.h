/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_OSD_H_
#define _FDI_OSD_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
// To do : add to support ntsc
#define FDI_OSD_MAX_WIDTH						720
#define FDI_OSD_MAX_HEIGHT						576

typedef enum
{
	FDI_OSD_PIXEL_FORMAT_ARGB_8888,
	FDI_OSD_PIXEL_FORMAT_RGBA_8888,
	FDI_OSD_PIXEL_FORMAT_ARGB_4444,
	FDI_OSD_PIXEL_FORMAT_RGBA_4444,
	FDI_OSD_PIXEL_FORMAT_ARGB_1555,
	FDI_OSD_PIXEL_FORMAT_CLUT_8,
	FDI_OSD_PIXEL_FORMAT_A_8,
	FDI_OSD_PIXEL_FORMAT_YCBCR_444,
	FDI_OSD_PIXEL_FORMAT_YCBCR_422
} FDI_OSD_PIXEL_FORMAT_e;

typedef enum
{
	FDI_COLOR_BLACK = 0,
	FDI_COLOR_GRAY,
	FDI_COLOR_DEEPBLUE,
	FDI_COLOR_BLUE,
	FDI_COLOR_SKYBLUE,
	FDI_COLOR_GREEN,
	FDI_COLOR_RED,
	FDI_COLOR_YELLOW,
	FDI_COLOR_WHITE,
	FDI_COLOR_NONE
} FDI_COLOR_e;

#define RGB_COLOR_BLACK							FDI_OSD_GetColor(FDI_COLOR_BLACK)
#define RGB_COLOR_GRAY							FDI_OSD_GetColor(FDI_COLOR_GRAY)
#define RGB_COLOR_DEEPBLUE						FDI_OSD_GetColor(FDI_COLOR_DEEPBLUE)
#define RGB_COLOR_BLUE							FDI_OSD_GetColor(FDI_COLOR_BLUE)
#define RGB_COLOR_SKYBLUE						FDI_OSD_GetColor(FDI_COLOR_SKYBLUE)
#define RGB_COLOR_GREEN							FDI_OSD_GetColor(FDI_COLOR_GREEN)
#define RGB_COLOR_RED							FDI_OSD_GetColor(FDI_COLOR_RED)
#define RGB_COLOR_YELLOW						FDI_OSD_GetColor(FDI_COLOR_YELLOW)
#define RGB_COLOR_WHITE							FDI_OSD_GetColor(FDI_COLOR_WHITE)
#define RGB_COLOR_NONE							(-1)

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : FDI_OSD_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_Open(void);

/*******************************************************************************
* function : FDI_OSD_FlushScreen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_FlushScreen(int nX, int nY, int nW, int nH);

/*******************************************************************************
* function : FDI_OSD_FlushScreen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_Show();

/*******************************************************************************
* function : FDI_OSD_FlushScreen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_Hide();

/******************************************************************************
* function : FDI_OSD_DrawFillRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_DrawFillRect(int nX, int nY, int nW, int nH, int nColor);

/******************************************************************************
* function : FDI_OSD_DrawFillRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_GetFrameBuffer(void **ppvFrameBuffer);

/******************************************************************************
* function : FDI_OSD_GetOsdRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_GetOsdRect(int nX, int nY, int nW,	int nH, void *pvBuf);

/******************************************************************************
* function : FDI_OSD_SetOsdRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_SetOsdRect(int nX, int nY, int nW, int nH, void *pvBuf);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_DisplayColorbar(BOOL bOnOff);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_GetPixelInfo(FDI_OSD_PIXEL_FORMAT_e *pePixelFormat, unsigned long *pulPixelSize);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FDI_OSD_GetColor(FDI_COLOR_e eColor);

#endif

