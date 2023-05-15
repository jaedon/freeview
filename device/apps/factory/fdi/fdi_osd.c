/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "di_err.h"
#include "di_osd.h"

#include "fvk.h"
#include "fdi_common.h"
#include "fdi_osd.h"


/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define WHITE_COLOR			0xFFFFFFFF
#define YELLOW_COLOR		0xFFFFFF00
#define CYAN_COLOR			0xFF00FFFF
#define GREEN_COLOR			0xFF00FF00
#define MAGENTA_COLOR		0xFFFF00FF
#define RED_COLOR			0xFFFF0000
#define BLUE_COLOR			0xFF0000FF
#define BLACK_COLOR			0xFF000000
#define CLEAR_COLOR			0x00000000

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
DI_OSD_HANDLE g_FactoryOsdHandle = 0;
DI_OSD_HANDLE g_ColorbarOsdHandle = 0;

/*******************************************************************************
* function : FDI_OSD_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_Open(void)
{
	DI_ERR_CODE	nErr;
	DI_OSD_RECT tOSDRect;

	/* for factory application */
	if (!g_FactoryOsdHandle)
	{
		tOSDRect.x = 0;
		tOSDRect.y = 0;
		tOSDRect.width = FDI_OSD_MAX_WIDTH;
		tOSDRect.height = FDI_OSD_MAX_HEIGHT;

		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_0, tOSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &g_FactoryOsdHandle);
		if (nErr != DI_ERR_OK)
		{
			return FDI_ERR;
		}
	}
	DI_OSD_Hide(g_FactoryOsdHandle);

	/* for colorbar */
	if (!g_ColorbarOsdHandle)
	{
		tOSDRect.x = 0;
		tOSDRect.y = 0;
		tOSDRect.width = FDI_OSD_MAX_WIDTH;
		tOSDRect.height = FDI_OSD_MAX_HEIGHT;

		nErr = DI_OSD_CreateSurface(DI_OSD_LAYER_1, tOSDRect, DI_OSD_PIXEL_FORMAT_ARGB_8888, &g_ColorbarOsdHandle);
		if (nErr != DI_ERR_OK)
		{
			g_ColorbarOsdHandle = g_FactoryOsdHandle;
		}
	}
	DI_OSD_Hide(g_ColorbarOsdHandle);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_OSD_FlushScreen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_FlushScreen(int nX, int nY, int nW, int nH)
{
	DI_OSD_RECT tOSDRect;

	tOSDRect.x = nX;
	tOSDRect.y = nY;
	tOSDRect.width = nW;
	tOSDRect.height = nH;
	
	DI_OSD_Flush(g_FactoryOsdHandle, &tOSDRect); 

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_OSD_FlushScreen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_Show()
{
	DI_OSD_Show(g_FactoryOsdHandle);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_OSD_FlushScreen
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_Hide()
{
	DI_OSD_Hide(g_FactoryOsdHandle);
	
	return FDI_NO_ERR;
}

/******************************************************************************
* function : FDI_OSD_DrawFillRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_DrawFillRect(int nX, int nY, int nW, int nH, int nColor)
{
	DI_OSD_RECT tOSDRect;

	tOSDRect.x = nX;
	tOSDRect.y = nY;
	tOSDRect.width = nW;
	tOSDRect.height = nH;
	DI_OSD_FillOSDBlock(g_FactoryOsdHandle, tOSDRect, nColor);

	return FDI_NO_ERR;
}

/******************************************************************************
* function : FDI_OSD_DrawFillRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_GetFrameBuffer(void **ppvFrameBuffer)
{
	DI_OSD_GetFrameBuffer(g_FactoryOsdHandle, (void**)ppvFrameBuffer);

	return FDI_NO_ERR;
}

/******************************************************************************
* function : FDI_OSD_GetOsdRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_GetOsdRect(int nX, int nY, int nW,	int nH, void *pvBuf)
{
	unsigned long *pulFbAddr, *pulStartAddr, *pulOsdBuf;;
	unsigned int unWidth, unHeight;

	if( pvBuf == NULL )
	{
		return FDI_ERR;
	}

	DI_OSD_GetFrameBuffer(g_FactoryOsdHandle, (void**)&pulFbAddr);
	pulOsdBuf = (unsigned long *)pvBuf;
	
	for( unWidth = 0 ; unWidth < nW ; unWidth++ )
	{
		for( unHeight = 0 ; unHeight < nH ; unHeight++ )
		{
			pulStartAddr = pulFbAddr + (nY+unHeight)*720 + (nX+unWidth);
			*pulOsdBuf = *pulStartAddr;
			pulOsdBuf++;
		}
	}

	return FDI_NO_ERR;
}

/******************************************************************************
* function : FDI_OSD_SetOsdRect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FDI_ERR_CODE FDI_OSD_SetOsdRect(int nX, int nY, int nW, int nH, void *pvBuf)
{
	unsigned long *pulFbAddr, *pulStartAddr, *pulOsdBuf;
	unsigned int unWidth, unHeight;

	if( pvBuf == NULL )
	{
		return FDI_ERR;
	}
	
	DI_OSD_GetFrameBuffer(g_FactoryOsdHandle, (void**)&pulFbAddr);
	pulOsdBuf = (unsigned long *)pvBuf;

	for( unWidth = 0 ; unWidth < nW ; unWidth++ )
	{
		for( unHeight = 0 ; unHeight < nH ; unHeight++ )
		{
			pulStartAddr = pulFbAddr + (nY+unHeight)*720 + (nX+unWidth);
			*pulStartAddr = *pulOsdBuf;
			pulOsdBuf++;
		}
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_DisplayColorbar(BOOL bOnOff)
{
	DI_OSD_RECT tOSDRect;

	if( bOnOff == TRUE )
	{
		tOSDRect.x = 0;
		tOSDRect.y = 0;
		tOSDRect.width = FDI_OSD_MAX_WIDTH/8;
		tOSDRect.height = FDI_OSD_MAX_HEIGHT;

		// 1. White Bar
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, WHITE_COLOR);
		// 2. Yellow Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, YELLOW_COLOR);
		// 3. Cyan Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, CYAN_COLOR);
		// 4. Green Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, GREEN_COLOR);
		// 5. Magenta Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, MAGENTA_COLOR);
		// 6. Red Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, RED_COLOR);
		// 7. Blue Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, BLUE_COLOR);
		// 8. Black Bar
		tOSDRect.x += FDI_OSD_MAX_WIDTH/8;
		DI_OSD_FillOSDBlock(g_ColorbarOsdHandle, tOSDRect, BLACK_COLOR);

		tOSDRect.x = 0;
		tOSDRect.y = 0;
		tOSDRect.width = FDI_OSD_MAX_WIDTH;
		tOSDRect.height = FDI_OSD_MAX_HEIGHT;
		DI_OSD_Flush(g_ColorbarOsdHandle, &tOSDRect); 
		DI_OSD_Hide(g_FactoryOsdHandle);
		DI_OSD_Show(g_ColorbarOsdHandle);
	}
	else
	{
		DI_OSD_Hide(g_ColorbarOsdHandle);
		DI_OSD_Show(g_FactoryOsdHandle);
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_OSD_GetPixelInfo(FDI_OSD_PIXEL_FORMAT_e *pePixelFormat, unsigned long *pulPixelSize)
{
	if( pePixelFormat == NULL || pulPixelSize == NULL)
	{
		return FDI_ERR;
	}
	
	*pePixelFormat = FDI_OSD_PIXEL_FORMAT_ARGB_8888;
	*pulPixelSize = 4;
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FDI_OSD_GetColor(FDI_COLOR_e eColor)
{
	unsigned long ulColor;
	
	switch(eColor)
	{
		case FDI_COLOR_BLACK:
			ulColor = 0xFF000000;
			break;
		case FDI_COLOR_GRAY:
			ulColor = 0xFF666666;
			break;
		case FDI_COLOR_DEEPBLUE:
			ulColor = 0xFF000033;
			break;
		case FDI_COLOR_BLUE:
			ulColor = 0xFF0000FF;
			break;
		case FDI_COLOR_SKYBLUE:
			ulColor = 0xFF00CECE;
			break;
		case FDI_COLOR_GREEN:
			ulColor = 0xFF00FF00;
			break;
		case FDI_COLOR_RED:
			ulColor = 0xFFFF0000;
			break;
		case FDI_COLOR_YELLOW:
			ulColor = 0xFFCCCC00;
			break;
		case FDI_COLOR_WHITE:
			ulColor = 0xFFFFFFFE;
			break;
		case FDI_COLOR_NONE:
		default:
			ulColor = 0xFF000000;
			break;
	}
	return ulColor;
}
