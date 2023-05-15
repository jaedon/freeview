/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   di_osd.c  $
 * $Modtime:   Jul 16 2007 01:07:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/*********************************************************************
 * Header Files
 *********************************************************************/
#include <string.h>
#include "linden_trace.h"

#include "htype.h"
#include "di_err.h"
#include "di_uart.h"
#include "di_video.h"
#include "drv_video.h"
#include "di_osd.h"
#include "drv_osd.h"
#include "vkernel.h"
#include "taskdef.h"
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
#include "drv_sharedmem.h"
#endif

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"

/*********************************************************************
 * Macro Definition
 *********************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_OSD
#define PrintStatus					DI_UART_Print

//#define		CONFIG_SW_OSD_BLEND_COPY
//#define		CONFIG_OSD_DOUBLEBUFFER

#define DI_OSD_FB_MAX_NUM					(1000)
#define DI_OSD_MAX_PALETTE_ENTRIES			(6)
#define DI_OSD_8BIT_PALETTE_ENTRIES (256)

#define GFX_ENGINESYNC_TIMEOUT (1000)	// timeout is 1 sec

#define MAIN_BUF (0)
#if defined(CONFIG_OSD_DOUBLEBUFFER)
#define		FRAMEBUF_MAX		(2)
#define		BACK_BUF			(1)
#define		THRESHOLD_SIZE_DOUBLE_BUFFER		(OSD_WIDTH*OSD_HEIGHT/2)
#else
#define		FRAMEBUF_MAX		(1)
#define		BACK_BUF			(0)
#endif

#define SD_ALIGN_W		9			// 1280/720 = 16/9
#define SD_ALIGN_H		8			// 720/576 = 5/4 & HD Disp : 1080/720 = 3/2
#define HD_ALIGN_W		16			// 720/1280 = 9/16
#define HD_ALIGN_H		10			// 576/720 = 4/5 & HD Disp : 1080/720 = 3/2

#define PARAM_COLOR_ON		"\033[01;34m"
#define PARAM_COLOR_OFF		"\033[00m"

/*********************************************************************
 * Global/Static Variable Definition
 *********************************************************************/
typedef struct
{
	NEXUS_Graphics2DHandle 	nexus_gfx_2DHandle;
	BKNI_EventHandle event;

	DI_OSD_INSTANCE 		HD_OsdInstance[FRAMEBUF_MAX];
	DI_OSD_INSTANCE 		SD_OsdInstance[FRAMEBUF_MAX];

	DI_OSD_RECT				HD_ClipRect;
	DI_OSD_RECT				SD_ClipRect;

	NEXUS_DisplayHandle 	HD_DispHandle;
	NEXUS_DisplayHandle 	SD_DispHandle;

	HBOOL					b_Layer_Show_nHide[DI_OSD_LAYER_MAX];
	unsigned char			ucPlaneAlpha;
}DI_OSD_LIST;

/*********************************************************************
 * Extern Function
 *********************************************************************/
HBOOL	g_BlockPrint[DI_OSD_SURFACE_MAX_NUM];
/*********************************************************************
 * Static Function
 *********************************************************************/
static DI_OSD_LIST 				s_OsdList;

#if defined(CONFIG_OSD_DOUBLEBUFFER)
static HBOOL 					b_HD_SurfaceDirty;
static HBOOL 					b_SD_SurfaceDirty;
#endif


static DI_OSD_INSTANCE 			s_aOsdInstance[DI_OSD_SURFACE_MAX_NUM];
static DI_OSD_INSTANCE 			s_aOsdFBInstance[DI_OSD_FB_MAX_NUM];
static DI_OSD_PALETTE			s_aOsdPalette[DI_OSD_MAX_PALETTE_ENTRIES];
static DI_OSD_INSTANCE 			s_aBlendInstance;

#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
static DI_OSD_INSTANCE 			s_aOrgOsdInstance[DI_OSD_SURFACE_MAX_NUM];
#endif

static unsigned long 			s_ulOsdSemId;

// SD 화면에서 flutter 현상 방지를 위해, SD OSD 에 대해서는 Blurry filter coeffs 로 처리한다. 2009.09.09. kycho
static NEXUS_Graphics2DFilterCoeffs	s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eBlurry;

static DI_ERR_CODE P_OSD_CheckHandle(DI_OSD_HANDLE hOsdHandle);
static DI_ERR_CODE P_OSD_CheckRect(DI_OSD_RECT baseRect, DI_OSD_RECT checkRect);

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
static void *sharedMem=NULL;
#endif

/*********************************************************************
 * Local Function
 *********************************************************************/
#define AssertNEXUS(arg1, arg2) 	if(arg1 != NEXUS_SUCCESS) {PrintError("ERROR code : 0x%x\n",arg1); return arg2;}
#define AssertDI(arg1, arg2) 		if(arg1 != DI_ERR_OK) {PrintError("ERROR code : 0x%x\n",arg1); return arg2;}

#if	defined(CONFIG_SW_OSD_BLEND_COPY)

#define THRESHOLD_SIZE_STOCK_DMA_COPY		(256)

// DD에서 Blend로 사용하는 Blend 공식을 사용한 함수
// BCM7405 Blend Formula (SRCOVER)
// C(d) = C(s)*A(s) + C(d) * (1 - A(s))
// A(d) = A(s) + A(d) * (1 - A(s))
static unsigned long P_OSD_BlendColorSrcCover (unsigned long fg, unsigned long bg)
{
	int		 nColorFA, nColorFR, nColorFG, nColorFB;
	int		 nColorBA, nColorBR, nColorBG, nColorBB;

	nColorFA = (fg & 0xFF000000) >> 24;
	nColorFR = (fg & 0x00FF0000) >> 16;
	nColorFG = (fg & 0x0000FF00) >> 8;
	nColorFB = (fg & 0x000000FF);

	nColorBA = (bg & 0xFF000000) >> 24;
	nColorBR = (bg & 0x00FF0000) >> 16;
	nColorBG = (bg & 0x0000FF00) >> 8;
	nColorBB = (bg & 0x000000FF);

	nColorFR = (nColorFR*nColorFA + nColorBR*(255 - nColorFA))/255;
	if (nColorFR > 255)					{ nColorFR = 255; }
	nColorFG = (nColorFG*nColorFA + nColorBG*(255 - nColorFA))/255;
	if (nColorFG > 255)					{ nColorFG = 255; }
	nColorFB = (nColorFB*nColorFA + nColorBB*(255 - nColorFA))/255;
	if (nColorFB > 255)					{ nColorFB = 255; }
	nColorFA = (nColorFA*255 + nColorBA*(255 - nColorFA))/255;
	if (nColorFA > 255)					{ nColorFA = 255; }

	return	((nColorFA << 24) | (nColorFR << 16) | (nColorFG << 8) | (nColorFB));
}

static int P_OSD_DrawImageOfStockFrame (DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT stSrcRect,
													DI_OSD_HANDLE hDstHandle, DI_OSD_RECT stDstRect,
													DI_OSD_RASTER_OP eRop)
{
	//unsigned char	 ucColorR, ucColorG, ucColorB, ucColorA;
	int				 nLineStart, nLineEnd, nLineCnt, nColCnt;
	int				 nBitmapNextOffset;
	int				 nScreenNextOffset;
	//unsigned long	 ulColor;
	DI_OSD_INSTANCE	*pSrcInstance;
	DI_OSD_INSTANCE	*pDstInstance;
	//HUINT8			*pucImgStartPtr, *pucBmpPtr, *pucBmpPtr_NextLine;
	unsigned long	*pucImgStartPtr, *pucBmpPtr, *pucBmpPtr_NextLine;
	unsigned long	*pulScreenPtr, *pulDrawPtr, *pulDrawPtr_NextLine;

	pSrcInstance = (DI_OSD_INSTANCE *)hSrcHandle;
	pDstInstance = (DI_OSD_INSTANCE *)hDstHandle;

	// Destination : Screen (4byte 단위)
	pulScreenPtr		= (unsigned long *)pDstInstance->pFBAddr;
	pulDrawPtr			= pulScreenPtr + (stDstRect.y * pDstInstance->rect.width);
	nScreenNextOffset	= pDstInstance->rect.width;


	// Source : Stock Image (4byte 단위)
	pucImgStartPtr		= (unsigned long *)pSrcInstance->pFBAddr;
	pucBmpPtr			= pucImgStartPtr + (stSrcRect.y * pSrcInstance->rect.width);
	nBitmapNextOffset	= pSrcInstance->rect.width;

	nLineStart	= stDstRect.y;
	nLineEnd	= stDstRect.y + stDstRect.height;

	for (nLineCnt = nLineStart; nLineCnt < nLineEnd; nLineCnt++)
	{
		pulDrawPtr_NextLine = pulDrawPtr + nScreenNextOffset;
		pucBmpPtr_NextLine = pucBmpPtr + nBitmapNextOffset;

		// X Position 을 맞춘다.
		pulDrawPtr += stDstRect.x;
		pucBmpPtr += stSrcRect.x;

		for (nColCnt = 0; nColCnt < stSrcRect.width; nColCnt ++)
		{
			if ( (eRop==DI_OSD_RASTER_OP_SRCOVER) && ( ((*pucBmpPtr)>>24) != 0xFF) )	// if Alpha is not 0xFF
			{
				// Blend Action~!
				*pulDrawPtr = P_OSD_BlendColorSrcCover (*pucBmpPtr, *pulDrawPtr);
			}
			else
			{
				// Copy & Blend action is same !
				*pulDrawPtr = *pucBmpPtr;
			}

			pulDrawPtr++;
			pucBmpPtr++;
		}

		pulDrawPtr = pulDrawPtr_NextLine;
		pucBmpPtr = pucBmpPtr_NextLine;
	}

	return DI_ERR_OK;
}

#endif

static HBOOL P_OSD_EnablePrint(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_OSD_LAYER	i;

	if( pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
		return TRUE;

	for (i=DI_OSD_LAYER_0; i<DI_OSD_SURFACE_MAX_NUM; i++)
	{
		if (g_BlockPrint[i])
		{
			if( pOsdInstance->status == DI_OSD_SURFACE_ENABLED
				&& pOsdInstance->layer == i)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*********************************************************************
 *
 * Function 	P_OSD_CheckHandle
 * Description	This function sets the alpha value of the specified OSD plane.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
static DI_ERR_CODE P_OSD_CheckHandle(DI_OSD_HANDLE hOsdHandle)
{
	if (hOsdHandle)	// null handle check
	{
		DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

		if( pOsdInstance->surface)
		{
			return DI_ERR_OK;
		}
	}

	return DI_ERR_ERROR;
}

/*********************************************************************
 *
 * Function 	P_OSD_CheckRect
 * Description	This function sets the alpha value of the specified OSD plane.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
static DI_ERR_CODE P_OSD_CheckRect(DI_OSD_RECT baseRect, DI_OSD_RECT checkRect)
{
	if( checkRect.x < baseRect.x ||
		checkRect.y < baseRect.y ||
		checkRect.x + checkRect.width > baseRect.x + baseRect.width ||
		checkRect.y + checkRect.height > baseRect.y + baseRect.height)
	{
		PrintError("@Error@ - base(%d,%d,%d,%d), check(%d,%d,%d,%d)\n",
			baseRect.x, baseRect.y, baseRect.width, baseRect.height,
			checkRect.x, checkRect.y, checkRect.width, checkRect.height);

		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

static void P_OSD_AlignRect(DI_OSD_RECT baseRect, DI_OSD_RECT checkRect, DI_OSD_RECT *alignRect)
{
	int		Align_W, Align_H;

	alignRect->x = checkRect.x;
	alignRect->y = checkRect.y;
	alignRect->width = checkRect.width;
	alignRect->height = checkRect.height;

	/* 우선 left/right/top/bottom 을 1 pixel 만큼 크게 한다. */
	if (alignRect->x>0)
	{
		alignRect->x -= 1;
		alignRect->width += 1;
	}
	if (alignRect->y>0)
	{
		alignRect->y -= 1;
		alignRect->height += 1;
	}

	if (alignRect->x + alignRect->width < baseRect.x + baseRect.width)
		alignRect->width += 1;
	if (alignRect->y + alignRect->height < baseRect.y + baseRect.height)
		alignRect->height += 1;

	Align_W = (baseRect.height>OSD_SD_HEIGHT)?(HD_ALIGN_W):(SD_ALIGN_W);
	Align_H = (baseRect.height>OSD_SD_HEIGHT)?(HD_ALIGN_H):(SD_ALIGN_H);

	/* X & WIDTH Align */
	if (alignRect->x > Align_W)
	{
		alignRect->width += alignRect->x%Align_W;
		alignRect->x -= alignRect->x%Align_W;
	}
	else
	{
		alignRect->width += alignRect->x;
		alignRect->x= 0;
	}

	if (alignRect->x + alignRect->width < baseRect.x + baseRect.width-Align_W)
	{
		alignRect->width += (Align_W - alignRect->width%Align_W);
	}
	else
	{
		alignRect->width = baseRect.width;
		alignRect->x = baseRect.x;
	}

	/* Y & HEIGHT Align */
	if (alignRect->y > Align_H)
	{
		alignRect->height += alignRect->y%Align_H;
		alignRect->y -= alignRect->y%Align_H;
	}
	else
	{
		alignRect->height += alignRect->y;
		alignRect->y = 0;
	}

	if (alignRect->y + alignRect->height < baseRect.y + baseRect.height-Align_H)
	{
		alignRect->height += (Align_H - alignRect->height%Align_H);
	}
	else
	{
		alignRect->height = baseRect.height;
		alignRect->y = baseRect.y;
	}

	return;
}


static DI_ERR_CODE P_OSD_Scale_OsdRect(DI_OSD_RECT *src_rect, DI_OSD_RECT *dest_rect,
									int src_max_width, int src_max_height,
									int dest_max_width, int dest_max_height)
{
	if((src_rect==NULL) || (dest_rect==NULL))
	{
		PrintError("@Error@ - rect is NULL!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if ((src_max_width == 0) || (src_max_height == 0))
	{
		PrintError("@Error@ - Invalid size!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	dest_rect->x = (src_rect->x* dest_max_width) / src_max_width;
	dest_rect->y = (src_rect->y * dest_max_height) / src_max_height;
	dest_rect->width = (src_rect->width* dest_max_width) / src_max_width;
	dest_rect->height = (src_rect->height * dest_max_height) / src_max_height;

	return DI_ERR_OK;
}

static void P_OSD_gfxEventComplete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void P_OSD_gfxEngineSync(void)
{
	NEXUS_Error 			rc;
	BERR_Code	eStatus = BERR_SUCCESS;

	rc = NEXUS_Graphics2D_Checkpoint(s_OsdList.nexus_gfx_2DHandle, NULL);
	if(rc==NEXUS_GRAPHICS2D_QUEUED)
	{
		eStatus=BKNI_WaitForEvent(s_OsdList.event, BKNI_INFINITE);
		if(eStatus == BERR_TIMEOUT)
		{
			PrintError("[%s] apply times out\n",__func__);
		}
	}
	else if(rc==NEXUS_GRAPHICS2D_BUSY)
	{
		PrintError("[%s] NEXUS_GRAPHICS2D_BUSY\n",__func__);
	}

	return;
}

#if defined(CONFIG_OSD_DOUBLEBUFFER)
static void P_OSD_Flip_HD(void *context, int unused)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	BSTD_UNUSED(context);
	BSTD_UNUSED(unused);

	//CHECK_BOOT_TIME("HD: Start CB");

	if (b_HD_SurfaceDirty)
	{
		//if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
		{
			/* flush cached memory */
		    NEXUS_Surface_Flush(s_OsdList.HD_OsdInstance[BACK_BUF].surface);

			NEXUS_Graphics2D_GetDefaultBlitSettings( &blitSettings );

			// HD Surface Backup
		    blitSettings.source.surface = s_OsdList.HD_OsdInstance[BACK_BUF].surface;
			blitSettings.source.rect.x = (int16_t)s_OsdList.HD_ClipRect.x;
		    blitSettings.source.rect.y = (int16_t)s_OsdList.HD_ClipRect.y;
		    blitSettings.source.rect.width = (uint16_t)s_OsdList.HD_ClipRect.width;
		    blitSettings.source.rect.height = (uint16_t)s_OsdList.HD_ClipRect.height;

		    blitSettings.output.surface = s_OsdList.HD_OsdInstance[MAIN_BUF].surface;
			blitSettings.output.rect.x = (int16_t)s_OsdList.HD_ClipRect.x;
		    blitSettings.output.rect.y = (int16_t)s_OsdList.HD_ClipRect.y;
		    blitSettings.output.rect.width = (uint16_t)s_OsdList.HD_ClipRect.width;
		    blitSettings.output.rect.height = (uint16_t)s_OsdList.HD_ClipRect.height;

		    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
		    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

		    rc = NEXUS_Graphics2D_Blit( s_OsdList.nexus_gfx_2DHandle, &blitSettings );
			if (rc)
			{
				PrintError("NEXUS_Graphics2D_Blit error(%d) !!\n", rc);
				return;
			}

			//P_OSD_gfxEngineSync();

			b_HD_SurfaceDirty = FALSE;

			//VK_SEM_Release(s_ulOsdSemId);
		}
	}

	//CHECK_BOOT_TIME("HD: End   CB");

    return;
}

static void P_OSD_Flip_SD(void *context, int unused)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	BSTD_UNUSED(context);
	BSTD_UNUSED(unused);

	//CHECK_BOOT_TIME("SD: Start CB");

	if (b_SD_SurfaceDirty)
	{
		//if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
		{
			/* flush cached memory */
		    NEXUS_Surface_Flush(s_OsdList.SD_OsdInstance[BACK_BUF].surface);

			NEXUS_Graphics2D_GetDefaultBlitSettings( &blitSettings );

			// HD Surface Backup
		    blitSettings.source.surface = s_OsdList.SD_OsdInstance[BACK_BUF].surface;
			blitSettings.source.rect.x = (int16_t)s_OsdList.SD_ClipRect.x;
		    blitSettings.source.rect.y = (int16_t)s_OsdList.SD_ClipRect.y;
		    blitSettings.source.rect.width = (uint16_t)s_OsdList.SD_ClipRect.width;
		    blitSettings.source.rect.height = (uint16_t)s_OsdList.SD_ClipRect.height;

		    blitSettings.output.surface = s_OsdList.SD_OsdInstance[MAIN_BUF].surface;
			blitSettings.output.rect.x = (int16_t)s_OsdList.SD_ClipRect.x;
		    blitSettings.output.rect.y = (int16_t)s_OsdList.SD_ClipRect.y;
		    blitSettings.output.rect.width = (uint16_t)s_OsdList.SD_ClipRect.width;
		    blitSettings.output.rect.height = (uint16_t)s_OsdList.SD_ClipRect.height;

		    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
		    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

		    rc = NEXUS_Graphics2D_Blit( s_OsdList.nexus_gfx_2DHandle, &blitSettings );
			if (rc)
			{
				PrintError("NEXUS_Graphics2D_Blit error(%d) !!\n", rc);
				return;
			}

			//P_OSD_gfxEngineSync();

			b_SD_SurfaceDirty = FALSE;

			//VK_SEM_Release(s_ulOsdSemId);
		}
	}

	//CHECK_BOOT_TIME("SD: End   CB");

    return;
}
#endif

/*********************************************************************
 *
 * Function 	P_OSD_Fill
 * Description	This function operate Fill-action with color(ARGB_8888)
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
static DI_ERR_CODE P_OSD_Fill(DI_OSD_INSTANCE	*pOsdInstance, DI_OSD_RECT rect, unsigned long color)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DFillSettings 	fillSettings;

	if(pOsdInstance->surface != NULL)
	{

		NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
		fillSettings.surface = pOsdInstance->surface;
		fillSettings.rect.x = (int16_t)rect.x;
		fillSettings.rect.y = (int16_t)rect.y;
		fillSettings.rect.width = (uint16_t)rect.width;
		fillSettings.rect.height = (uint16_t)rect.height;
		fillSettings.colorOp = NEXUS_FillOp_eCopy;
		fillSettings.alphaOp = NEXUS_FillOp_eCopy;

		fillSettings.color = (uint32_t)color;

		rc = NEXUS_Graphics2D_Fill(s_OsdList.nexus_gfx_2DHandle, &fillSettings);
		AssertNEXUS(rc, DI_ERR_ERROR);

#if 1 /* di 2.0 올리면서 sync 함수 호출 안함 */
		P_OSD_gfxEngineSync();
#endif
	}

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	P_OSD_BlendFill
 * Description	This function operate Fill-action with color(ARGB_8888)
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
static DI_ERR_CODE P_OSD_BlendFill(DI_OSD_INSTANCE	*pOsdInstance, DI_OSD_RECT rect, unsigned long color)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DFillSettings 	fillSettings;


	if(pOsdInstance->surface != NULL)
	{
		NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
		fillSettings.surface = pOsdInstance->surface;
		fillSettings.rect.x = (int16_t)rect.x;
		fillSettings.rect.y = (int16_t)rect.y;
		fillSettings.rect.width = (uint16_t)rect.width;
		fillSettings.rect.height = (uint16_t)rect.height;
		fillSettings.colorOp = NEXUS_FillOp_eBlend;
		fillSettings.alphaOp = NEXUS_FillOp_eCopy;

		fillSettings.color = (uint32_t)color;

		rc = NEXUS_Graphics2D_Fill(s_OsdList.nexus_gfx_2DHandle, &fillSettings);
		AssertNEXUS(rc, DI_ERR_ERROR);

#if 1 /* di 2.0 올리면서 sync 함수 호출 안함 */
		P_OSD_gfxEngineSync();
#endif
	}

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	P_OSD_Blit
 * Description	This function operate Blit-action (copy/blend)
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
static DI_ERR_CODE P_OSD_Blit(DI_OSD_INSTANCE	*pSrcInstance, DI_OSD_RECT srcRect,
							DI_OSD_INSTANCE	*pDstInstance, DI_OSD_RECT dstRect,
							DI_OSD_INSTANCE	*pOutInstance, DI_OSD_RECT outRect,
							DI_OSD_RASTER_OP rop, HUINT32 ulConstantColor, HBOOL bUseFilterSetting)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	NEXUS_Graphics2D_GetDefaultBlitSettings( &blitSettings );

	if (bUseFilterSetting)
	{
		// SD 화면에서 flutter 현상 방지를 위해, SD OSD 에 대해서는 Blurry filter coeffs 로 처리한다. 2009.09.09. kycho
		blitSettings.horizontalFilter = s_FilterCoeffs;
		blitSettings.verticalFilter = s_FilterCoeffs;
	}

	/*
	BlitSetting 에서
	dest & output 의 rect 은 반드시 같은 size 여야 한다. (limit !!!)
	그러므로, scale 이 필요한 surface 는
	반드시 source 에 setting 해주고, blend 조건을 바꾸는 작업이 필요하다.
	Blend 를 Source + Dest -> Dest 형태로 운영하기는 BlendOSDBlock 함수는 상관없음.
	*/

    blitSettings.source.surface = pSrcInstance->surface;
	blitSettings.source.rect.x = (int16_t)srcRect.x;
    blitSettings.source.rect.y = (int16_t)srcRect.y;
    blitSettings.source.rect.width = (uint16_t)srcRect.width;
    blitSettings.source.rect.height = (uint16_t)srcRect.height;

	if ((rop != DI_OSD_RASTER_OP_SRC || rop != DI_OSD_RASTER_OP_SRCMULTIPLYALPHA) && pDstInstance != NULL)
	{
		// Copy action 의 경우, Dst 는 output 이므로 BlitSettings 에서 dest 는 NULL 이다.
	    blitSettings.dest.surface = pDstInstance->surface;
		blitSettings.dest.rect.x = (int16_t)dstRect.x;
	    blitSettings.dest.rect.y = (int16_t)dstRect.y;
	    blitSettings.dest.rect.width = (uint16_t)dstRect.width;
	    blitSettings.dest.rect.height = (uint16_t)dstRect.height;
	}

	blitSettings.output.surface = pOutInstance->surface;
	blitSettings.output.rect.x = (int16_t)outRect.x;
    blitSettings.output.rect.y = (int16_t)outRect.y;
    blitSettings.output.rect.width = (uint16_t)outRect.width;
    blitSettings.output.rect.height = (uint16_t)outRect.height;

	blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;

	// useless equation parameter
	blitSettings.colorBlend.subtract_e = false;				// +
	blitSettings.colorBlend.e = NEXUS_BlendFactor_eZero;	// 0

	blitSettings.alphaBlend.subtract_e = false;				// +
	blitSettings.alphaBlend.e = NEXUS_BlendFactor_eZero;	// 0

	switch(rop)
	{
		case DI_OSD_RASTER_OP_SRC:			// Copy
			blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
	    	blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
			break;
		case DI_OSD_RASTER_OP_SRCATOP:
			// Oc = Sc*Da + Dc*(1-Sa)
			// Oa = Da
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;			// Sc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eDestinationAlpha;	// Da
			blitSettings.colorBlend.subtract_cd = false;						// +
			blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor;	// Dc
			blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;	// (1-Sa)

			blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyDest;
			break;
		case DI_OSD_RASTER_OP_DSTATOP:
			// Oc = Dc*Sa + Sc*(1-Da)
			// Oa = Sa
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eDestinationColor;		// Dc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceAlpha;				// Sa
			blitSettings.colorBlend.subtract_cd = false;							// +
			blitSettings.colorBlend.c = NEXUS_BlendFactor_eSourceColor;				// Sc
			blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseDestinationAlpha;	// (1-Da)

			blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
			break;
		case DI_OSD_RASTER_OP_SRCOVER:
			// Oc = Sc*Sa + Dc*(1-Sa)
			// Oa = Sa*1 + Da*(1-Sa)
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;			// Sc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceAlpha;			// Sa
			blitSettings.colorBlend.subtract_cd = false;						// +
			blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor;	// Dc
			blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;	// (1-Sa)

			blitSettings.alphaBlend.a = NEXUS_BlendFactor_eSourceAlpha;			// Sa
			blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;					// 1
			blitSettings.alphaBlend.subtract_cd = false;						// +
			blitSettings.alphaBlend.c = NEXUS_BlendFactor_eDestinationAlpha;	// Da
			blitSettings.alphaBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;	// (1-Sa)
			break;
		case DI_OSD_RASTER_OP_SRCOVER_WITHOUT_PREMULTIPLY_ALPHA:
			// Oc = Sc*1 + Dc*(1-Sa)
			// Oa = Sa*1 + Da*(1-Sa)
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;			// Sc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eOne;					// 1
			blitSettings.colorBlend.subtract_cd = false; 						// +
			blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor; 	// Dc
			blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;  // (1-Sa)

			blitSettings.alphaBlend.a = NEXUS_BlendFactor_eSourceAlpha;			// Sa
			blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;		            // 1
			blitSettings.alphaBlend.subtract_cd = false; 					    // +
			blitSettings.alphaBlend.c = NEXUS_BlendFactor_eDestinationAlpha;    // Da
			blitSettings.alphaBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;  // (1-Sa)
			break;
		case DI_OSD_RASTER_OP_DSTOVER:
			// Oc = Dc*Da + Sc*(1-Da)
			// Oa = Da*1 + Sa*(1-Da)
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eDestinationColor;		// Dc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eDestinationAlpha;		// Da
			blitSettings.colorBlend.subtract_cd = false;							// +
			blitSettings.colorBlend.c = NEXUS_BlendFactor_eSourceColor;				// Sc
			blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseDestinationAlpha;	// (1-Da)

			blitSettings.alphaBlend.a = NEXUS_BlendFactor_eDestinationAlpha;		// Da
			blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;						// 1
			blitSettings.alphaBlend.subtract_cd = false;							// +
			blitSettings.alphaBlend.c = NEXUS_BlendFactor_eSourceAlpha;				// Sa
			blitSettings.alphaBlend.d = NEXUS_BlendFactor_eInverseDestinationAlpha;	// (1-Da)
    		break;
		case DI_OSD_RASTER_OP_SRCMULTIPLYALPHA:
			// Oc = Sc
			// Oa = Sa*Ca
			blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;

			blitSettings.constantColor = ulConstantColor;
			blitSettings.alphaBlend.a = NEXUS_BlendFactor_eSourceAlpha;		// Sa
			blitSettings.alphaBlend.b = NEXUS_BlendFactor_eConstantAlpha;	// Ca
			break;
		case DI_OSD_RASTER_OP_SRCIN:
			// Oc = Sc*Da
			// Oa = Da
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;			// Sc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eDestinationAlpha;	// Da

			blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyDest;
			break;
		case DI_OSD_RASTER_OP_DSTIN:
			// Oc = Dc*Sa
			// Oa = Sa
			blitSettings.colorBlend.a = NEXUS_BlendFactor_eDestinationColor;	// Dc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceAlpha;			// Sa

			blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
			break;
		case DI_OSD_RASTER_OP_CONSTALPHA:
			// Oc = Sc
			// Oa = Ca
			blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
			blitSettings.constantColor = ulConstantColor;
			blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
			break;
		case DI_OSD_RASTER_OP_SRC_PREMULTIPLY_ALPHA:
			blitSettings.constantColor		= ulConstantColor;
			blitSettings.colorBlend.a 		= NEXUS_BlendFactor_eSourceColor;			// Sc
			blitSettings.colorBlend.b 		= NEXUS_BlendFactor_eSourceAlpha;			// Sa
			blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
			break;
		case DI_OSD_RASTER_OP_ALPHA_COMBINE_SRC:
			blitSettings.constantColor		 = ulConstantColor;
			blitSettings.alphaBlend.a 		 = NEXUS_BlendFactor_eSourceAlpha;
			blitSettings.alphaBlend.b 		 = NEXUS_BlendFactor_eConstantAlpha;
			blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
			break;
		case DI_OSD_RASTER_OP_CONSTCOLOR_OVER:
			// Oc = ConstColor*ConstAlpha + Dc*(1-Sa)
			// Oa = ConstColor*1 + Da*(1-Sa)
			blitSettings.constantColor		 = ulConstantColor;

			blitSettings.colorBlend.a = NEXUS_BlendFactor_eConstantColor; 		// Sc
			blitSettings.colorBlend.b = NEXUS_BlendFactor_eConstantAlpha; 		// Sa
			blitSettings.colorBlend.subtract_cd = false;						// +
			blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor;	// Dc
			blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseConstantAlpha;	// (1-Sa)

			blitSettings.alphaBlend.a = NEXUS_BlendFactor_eConstantAlpha; 		// Sa
			blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne; 				// 1
			blitSettings.alphaBlend.subtract_cd = false;						// +
			blitSettings.alphaBlend.c = NEXUS_BlendFactor_eDestinationAlpha;			// Da
			blitSettings.alphaBlend.d = NEXUS_BlendFactor_eInverseConstantAlpha;	// (1-Sa)
			break;

		default:
			PrintError("Not supported Operation (0x%x)\n", rop);
			return DI_ERR_INVALID_PARAM;
			break;
	}

    rc = NEXUS_Graphics2D_Blit( s_OsdList.nexus_gfx_2DHandle, &blitSettings );
	AssertNEXUS(rc, DI_ERR_ERROR);

#if 1 /* di 2.0 올리면서 sync 함수 호출 안함 */
	P_OSD_gfxEngineSync();
#endif

	return DI_ERR_OK;
}


/*********************************************************************
 *
 * Function 	P_OSD_Flush
 * Description	This function display osd (graphic) to real plane.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	P_OSD_Flush(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pRect)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE			nRet = DI_ERR_OK;
	DI_OSD_RECT			AdjRect, FlushRect, DispRect, SrcRect;
	int					TargetIndex;
	HBOOL				b_Is_Blend = FALSE;
	DI_OSD_INSTANCE		SrcInstance;
	unsigned int		i;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pRect == NULL)
	{
		PrintError("pRect is NULL!!\n");
		pRect = &pOsdInstance->rect;
	}

	if( P_OSD_CheckRect(pOsdInstance->rect, *pRect) != DI_ERR_OK)
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		return DI_ERR_INVALID_PARAM;
	}

	P_OSD_AlignRect(pOsdInstance->rect, *pRect, &AdjRect);

	//if ( P_OSD_EnablePrint(hOsdHandle) )
	PrintDebug(PARAM_COLOR_ON"Flush -> Layer[%d], hOsdHandle(0x%x) BaseRect(%d,%d,%d,%d)\n pRect (%d,%d,%d,%d) AdjRect(%d,%d,%d,%d) ",
		pOsdInstance->layer, hOsdHandle,
		pOsdInstance->rect.x, pOsdInstance->rect.y, pOsdInstance->rect.width, pOsdInstance->rect.height,
		pRect->x, pRect->y, pRect->width, pRect->height, AdjRect.x, AdjRect.y, AdjRect.width, AdjRect.height);

#if defined(CONFIG_OSD_DOUBLEBUFFER)
	if (AdjRect.width * AdjRect.height > THRESHOLD_SIZE_DOUBLE_BUFFER )/* large size */
	{
		TargetIndex = BACK_BUF;
	}
	else
#endif
	{
		TargetIndex = MAIN_BUF;
	}

	/*
	Get Flush Rect from AdjRect :
	flush parameter 로 내려온 surface 가 기준좌표인 (0,0,1280,720)-Layer_0 이 아닐경우
	좌표계산의 혼란이 생기므로, flushRect 은 (0,0,1280,720) 좌표로 전환해서 관리한다.
	*/
	nRet = P_OSD_Scale_OsdRect(&AdjRect, &FlushRect,
				pOsdInstance->rect.width, pOsdInstance->rect.height,
				s_aOsdInstance[DI_OSD_LAYER_0].rect.width, s_aOsdInstance[DI_OSD_LAYER_0].rect.height);
	AssertDI(nRet, DI_ERR_ERROR);

	/*
	Blend Order

	Display Top
	-----
	Layer_0(Main) Surface
	Layer_1 Surface
	Layer_2 Surface
	Video
	BackGround
	-----
	Display Bottom
	*/

	/* flush cached memory */
	NEXUS_Surface_Flush(s_aOsdInstance[pOsdInstance->layer].surface);

	/* Make Composited Surface */
	for(i=DI_OSD_LAYER_1; i<DI_OSD_LAYER_MAX; i++)
	{
		if (s_OsdList.b_Layer_Show_nHide[i])
		{
			/* Get SrcRect from AdjRect */
			nRet = P_OSD_Scale_OsdRect(&AdjRect, &SrcRect,
						pOsdInstance->rect.width, pOsdInstance->rect.height,
						s_aOsdInstance[i].rect.width, s_aOsdInstance[i].rect.height);
			AssertDI(nRet, DI_ERR_ERROR);

			if (b_Is_Blend)	// 만약 한 번이라도 Blend 되어 있다면, blit action 의 dest, out 은 BlendSurface 이다.
			{
				/* flush cached memory */
				//NEXUS_Surface_Flush(s_aOsdInstance[i].surface);
				//NEXUS_Surface_Flush(s_aBlendInstance.surface);

				// Layer_X_Surf + Blend_Surf => Blend_Surf
				nRet = P_OSD_Blit(&s_aOsdInstance[i], SrcRect, &s_aBlendInstance, FlushRect, &s_aBlendInstance, FlushRect, DI_OSD_RASTER_OP_DSTOVER, 0, FALSE);
				AssertDI(nRet, DI_ERR_ERROR);
			}
			else			// 첫 번째 Blend action 이라면, Main(LAYER_0) OSD 의 상태에 따라, BlendSurface 를 만들어 주어야 한다.
			{
				if (s_OsdList.b_Layer_Show_nHide[DI_OSD_LAYER_0])	// Main(Layer_0) OSD Show
				{
					/* flush cached memory */
					//NEXUS_Surface_Flush(s_aOsdInstance[i].surface);
					//NEXUS_Surface_Flush(s_aOsdInstance[DI_OSD_LAYER_0].surface);

					// Layer_X_Surf + Layer_0_Surf => Blend_Surf :: Destination Over Source
					nRet = P_OSD_Blit(&s_aOsdInstance[i], SrcRect, &s_aOsdInstance[DI_OSD_LAYER_0], FlushRect, &s_aBlendInstance, FlushRect, DI_OSD_RASTER_OP_DSTOVER, 0, FALSE);
					AssertDI(nRet, DI_ERR_ERROR);
				}
				else	// Main(Layer_0) OSD Hide
				{
					/* flush cached memory */
					//NEXUS_Surface_Flush(s_aOsdInstance[i].surface);

					// Layer_X_Surf => Blend_Surf
					nRet = P_OSD_Blit(&s_aOsdInstance[i], SrcRect, NULL, FlushRect, &s_aBlendInstance, FlushRect, DI_OSD_RASTER_OP_SRC, 0, FALSE);
					AssertDI(nRet, DI_ERR_ERROR);
				}
			}

			// if Layer_X exist (show), MUST set b_Is_Blend.
			b_Is_Blend = TRUE;
		}
	}


	if ( (b_Is_Blend==FALSE) && (s_OsdList.b_Layer_Show_nHide[DI_OSD_LAYER_0]==FALSE) )	// ALL Layer is HIDE~!!
	{
		/************
		Clear HD
		*************/
		if(s_OsdList.HD_DispHandle != NULL)
		{
			nRet = P_OSD_Fill(&s_OsdList.HD_OsdInstance[TargetIndex], s_OsdList.HD_ClipRect, 0);
			AssertDI(nRet, DI_ERR_ERROR);
		}

		/************
		Clear SD
		*************/
		if(s_OsdList.SD_DispHandle != NULL)
		{
			nRet = P_OSD_Fill(&s_OsdList.SD_OsdInstance[TargetIndex], s_OsdList.SD_ClipRect, 0);
			AssertDI(nRet, DI_ERR_ERROR);
		}
	}
	else	// Composited (Blend) Surface or LAYER_0 Surface is Dipsplay Surface~~!!
	{
		// if b_Is_Blend is not set, Only show layer is LAYER_0 (Main) :: Both Surface is (0, 0, 1280, 720) size~!!!
		SrcInstance = (b_Is_Blend)?s_aBlendInstance:s_aOsdInstance[DI_OSD_LAYER_0];

		/************
		Display on HD
		*************/
		if(s_OsdList.HD_DispHandle != NULL)
		{
			/* Get HD DispRect from FlushRect */
			nRet = P_OSD_Scale_OsdRect(&FlushRect, &DispRect,
						s_aOsdInstance[DI_OSD_LAYER_0].rect.width, s_aOsdInstance[DI_OSD_LAYER_0].rect.height,
						s_OsdList.HD_ClipRect.width, s_OsdList.HD_ClipRect.height);
			AssertDI(nRet, DI_ERR_ERROR);

			//if ( P_OSD_EnablePrint(hOsdHandle) )
			PrintDebug(" HD DispRect(%d,%d,%d,%d) ", DispRect.x, DispRect.y, DispRect.width, DispRect.height);

			nRet = P_OSD_Blit(&SrcInstance, FlushRect, NULL, DispRect, &s_OsdList.HD_OsdInstance[TargetIndex], DispRect, DI_OSD_RASTER_OP_SRC, 0, FALSE);
			AssertDI(nRet, DI_ERR_ERROR);
		}


		/************
		Display on SD
		*************/
		/* Get SD DispRect from FlushRect */
		if(s_OsdList.SD_DispHandle != NULL)
		{
			nRet = P_OSD_Scale_OsdRect(&FlushRect, &DispRect,
						s_aOsdInstance[DI_OSD_LAYER_0].rect.width, s_aOsdInstance[DI_OSD_LAYER_0].rect.height,
						s_OsdList.SD_ClipRect.width, s_OsdList.SD_ClipRect.height);
			AssertDI(nRet, DI_ERR_ERROR);

			//if ( P_OSD_EnablePrint(hOsdHandle) )
			PrintDebug("SD DispRect(%d,%d,%d,%d)\n"PARAM_COLOR_OFF, DispRect.x, DispRect.y, DispRect.width, DispRect.height);

			// SD 화면에서 flutter 현상 (얇은 가로 줄의 화면 떨림) 방지를 위해, SD OSD 에 대해서는 Blurry filter coeffs 로 처리한다. 2009.09.09. kycho
			// SD 화면에서 선명도를 높이는 방향으로 진행하기로 결정되어 filter setting 되돌림. 2009.10.29. kycho
			nRet = P_OSD_Blit(&SrcInstance, FlushRect, NULL, DispRect, &s_OsdList.SD_OsdInstance[TargetIndex], DispRect, DI_OSD_RASTER_OP_SRC, 0, FALSE);
			AssertDI(nRet, DI_ERR_ERROR);
		}
	}

#if defined(CONFIG_OSD_DOUBLEBUFFER)
	if (TargetIndex )	/* if need surface flip :: large size */
	{
		NEXUS_Error rc;

		//CHECK_BOOT_TIME("Set GRC    FB");

		b_HD_SurfaceDirty = TRUE;
		b_SD_SurfaceDirty = TRUE;

		// For frameBufferCallback(P_OSD_Flip func) Fire
		if(s_OsdList.HD_DispHandle != NULL)
		{
			rc = NEXUS_Display_SetGraphicsFramebuffer(s_OsdList.HD_DispHandle, s_OsdList.HD_OsdInstance[MAIN_BUF].surface);
			AssertNEXUS(rc, DI_ERR_ERROR);
		}
		if(s_OsdList.SD_DispHandle != NULL)
		{
			rc = NEXUS_Display_SetGraphicsFramebuffer(s_OsdList.SD_DispHandle, s_OsdList.SD_OsdInstance[MAIN_BUF].surface);
			AssertNEXUS(rc, DI_ERR_ERROR);
		}
	}
#endif

	PrintExit();

	return DI_ERR_OK;
}

#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)

static HBOOL P_OSD_DiffCheck(DI_OSD_RECT baseRect, DI_OSD_RECT checkRect)
{
	if( checkRect.x != baseRect.x || checkRect.y != baseRect.y ||
		checkRect.width != baseRect.width || checkRect.height != baseRect.height)
	{
		return TRUE;
	}
	return FALSE;
}

static DI_ERR_CODE P_OSD_CopyOrgToDisp(DI_OSD_LAYER	nLayer, DI_OSD_RECT srcRect, DI_OSD_RECT dstRect)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	NEXUS_Graphics2D_GetDefaultBlitSettings( &blitSettings );

    blitSettings.source.surface = s_aOrgOsdInstance[nLayer].surface;
	blitSettings.source.rect.x = (int16_t)srcRect.x;
    blitSettings.source.rect.y = (int16_t)srcRect.y;
    blitSettings.source.rect.width = (uint16_t)srcRect.width;
    blitSettings.source.rect.height = (uint16_t)srcRect.height;

	blitSettings.output.surface = s_aOsdInstance[nLayer].surface;
	blitSettings.output.rect.x = (int16_t)dstRect.x;
    blitSettings.output.rect.y = (int16_t)dstRect.y;
    blitSettings.output.rect.width = (uint16_t)dstRect.width;
    blitSettings.output.rect.height = (uint16_t)dstRect.height;

	blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

    rc = NEXUS_Graphics2D_Blit( s_OsdList.nexus_gfx_2DHandle, &blitSettings );
	AssertNEXUS(rc, DI_ERR_ERROR);

#if 1 /* di 2.0 올리면서 sync 함수 호출 안함 */
	P_OSD_gfxEngineSync();
#endif

	return DI_ERR_OK;
}

static DI_ERR_CODE P_OSD_MakeDispSurface(DI_OSD_HANDLE hOsdHandle, HBOOL bChangeInRect, HBOOL bChangeOutRect)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE			nRet = DI_ERR_OK;

	BSTD_UNUSED(bChangeInRect);

	PrintEnter();

	PrintDebug("[%s]: Layer[%d], bChangeOutRect(0x%x), bChangeInRect(0x%x)\n", __FUNCTION__, pOsdInstance->layer, bChangeOutRect, bChangeInRect);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		if (pOsdInstance->bUseBG)
		{
			if (pOsdInstance->nBG_Status == DI_OSD_BG_STATUS_FULL)
			{
				// Clear BG Plane
				nRet = P_OSD_Fill(&s_aOsdInstance[pOsdInstance->layer], pOsdInstance->rect, 0x0);
				if (nRet != DI_ERR_OK)
				{
					PrintError("ERROR in P_OSD_Fill(), nRet(0x%x)\n", nRet);
					goto done;
				}
			}
			else
			{
				if (bChangeOutRect)
				{
					nRet = P_OSD_Fill(&s_aOsdInstance[pOsdInstance->layer], pOsdInstance->rect, 0x0);
					if (nRet != DI_ERR_OK)
					{
						PrintError("ERROR in P_OSD_Fill(), nRet(0x%x)\n", nRet);
						goto done;
					}
				}
#if defined(CONFIG_SCALABLE_OSD_SURFACE)
				// Copy to BG Plane with In/Out Rect
				nRet = P_OSD_CopyOrgToDisp(pOsdInstance->layer, pOsdInstance->In_rect, pOsdInstance->Out_rect);
#else
				nRet = P_OSD_CopyOrgToDisp(pOsdInstance->layer, pOsdInstance->rect, pOsdInstance->rect);
#endif
				if (nRet != DI_ERR_OK)
				{
					PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nRet);
					goto done;
				}

				if (pOsdInstance->nBG_Status == DI_OSD_BG_STATUS_NONE_FULL)
				{
					// Clear BG Rect for BackGround Effect
					nRet = P_OSD_Fill(&s_aOsdInstance[pOsdInstance->layer], pOsdInstance->BG_rect, 0x0);
					if (nRet != DI_ERR_OK)
					{
						PrintError("ERROR in P_OSD_Fill(), nRet(0x%x)\n", nRet);
						goto done;
					}
				}
			}

			// Full Flush
			nRet = P_OSD_Flush(hOsdHandle, &pOsdInstance->rect);
			if (nRet != DI_ERR_OK)
			{
				PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nRet);
				goto done;
			}
		}
		else
#endif	// #if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		{
#if defined(CONFIG_SCALABLE_OSD_SURFACE)
			if (bChangeOutRect)
			{
				nRet = P_OSD_Fill(&s_aOsdInstance[pOsdInstance->layer], pOsdInstance->rect, 0x0);
				if (nRet != DI_ERR_OK)
				{
					PrintError("ERROR in P_OSD_Fill(), nRet(0x%x)\n", nRet);
					goto done;
				}
			}
			// Copy to BG Plane with In/Out Rect
			nRet = P_OSD_CopyOrgToDisp(pOsdInstance->layer, pOsdInstance->In_rect, pOsdInstance->Out_rect);
			if (nRet != DI_ERR_OK)
			{
				PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nRet);
				goto done;
			}

			if (bChangeOutRect)
			{
				// Full Flush
				nRet = P_OSD_Flush(hOsdHandle, &pOsdInstance->rect);
			}
			else
			{
				// Partial Flush for speed
				nRet = P_OSD_Flush(hOsdHandle, &pOsdInstance->Out_rect);
			}
			if (nRet != DI_ERR_OK)
			{
				PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nRet);
				goto done;
			}
#endif
		}
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nRet= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return nRet;
}

#endif

/*********************************************************************
 * Global Function
 *********************************************************************/
/*********************************************************************
 *
 * Function 	DI_OSD_Init
 * Description	This function initializes the OSD library. This should be called only once
 *				during system initialization.
 * Inputs		None
 * Outputs		None
 * Returns		DI_ERR_CODE_OK	if successful
 *				DI_ERR_CODE
 *
 *********************************************************************/
DRV_Error	DRV_OSD_Init(void)
{
	int 				loop;
	//NEXUS_Error			rc = NEXUS_SUCCESS;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_GraphicsSettings graphicsSettings;
	NEXUS_Graphics2DSettings gfxSettings;
	NEXUS_SurfaceMemory surfmem;
	DRV_Error			nRet = DRV_OK;
	NEXUS_DisplayHandle HD_DispHandle, SD_DispHandle;
	//NEXUS_DisplaySettings 	displaySettings;
	int	i;
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	DRV_DISP_SHAREDMEM_t *sharedHandles;
#endif
	PrintEnter();

	VK_memset(&s_OsdList, 0x00, sizeof(DI_OSD_LIST));

	/* HD OSD Init */
	/* HD GraphicDisplay FrameBuffer Create */
	NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
	createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	sharedMem=DRV_SHAREDMEM_Setup(sizeof(DRV_DISP_SHAREDMEM_t),DRV_DISP_SHAREDMEM_ID,false);
	sharedHandles=(DRV_DISP_SHAREDMEM_t *)sharedMem;
	HD_DispHandle=sharedHandles->disp_handle0;
	SD_DispHandle=sharedHandles->disp_handle1;
   	s_OsdList.SD_DispHandle=SD_DispHandle;
   	s_OsdList.HD_DispHandle=HD_DispHandle;
#else
	nRet = DRV_VIDEO_GetDisplayHandle(0, &HD_DispHandle, &SD_DispHandle);
	if (nRet != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetDisplayHandle error!!\n");
		return DRV_ERR;
	}
#endif

	/*
    ** The following size accomodates all display formats - where the horiz
    ** size is larger (e.g.1080i) the horizontal filter is automatically invoked
    */
	if(HD_DispHandle != NULL)
	{
#if defined(CONFIG_HDOUT_2160P) && defined(CONFIG_OSD_2160P)	//issue : no display on 1080i. We have to change dynamic OSD resolution management 1080i, p and 2160p.
		createSettings.width = 3840;
		createSettings.height = 2160;
#else
		createSettings.width = OSD_WIDTH;	//1280. where the horiz. size is larger (e.g.1080i) the horizontal filter is automatically invoked
		createSettings.height = 1080;
#endif

		for(i=0; i<FRAMEBUF_MAX; i++)
		{
			s_OsdList.HD_OsdInstance[i].surface = NEXUS_Surface_Create(&createSettings);
			if( s_OsdList.HD_OsdInstance[i].surface == NULL)
			{
				PrintError("NEXUS_Surface_Create HD(%d) error!!\n", i);
				return DRV_ERR_OUTOFMEMORY;
			}

			s_OsdList.HD_OsdInstance[i].status = DI_OSD_SURFACE_ENABLED;
			s_OsdList.HD_OsdInstance[i].format = DI_OSD_PIXEL_FORMAT_ARGB_8888;
			s_OsdList.HD_OsdInstance[i].rect.x =0;
			s_OsdList.HD_OsdInstance[i].rect.y =0;
			s_OsdList.HD_OsdInstance[i].rect.width = createSettings.width;
			s_OsdList.HD_OsdInstance[i].rect.height = createSettings.height;

			NEXUS_Surface_GetMemory(s_OsdList.HD_OsdInstance[i].surface, &surfmem);
			/* fill with transparent */
		    VK_memset(surfmem.buffer, 0x00, createSettings.height * surfmem.pitch);

			/* added after nexus 12.xx */
			NEXUS_Surface_Flush(s_OsdList.HD_OsdInstance[i].surface);

			s_OsdList.HD_OsdInstance[i].pFBAddr = surfmem.buffer;
		}

		// Init OSD Alpha & Show/Hide State
		NEXUS_Display_GetGraphicsSettings(HD_DispHandle, &graphicsSettings);

		s_OsdList.ucPlaneAlpha = graphicsSettings.alpha;
	}
	else
	{
		PrintError("NEXUS not support HD Graphics(Display) !!!\n");
	}

	if(SD_DispHandle != NULL)
	{
		/* SD OSD Init */
		/* SD GraphicDisplay FrameBuffer Create */
		NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
		createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
		createSettings.width = OSD_WIDTH/2;	//640. where the horiz. size is larger (e.g.1080i) the horizontal filter is automatically invoked
		createSettings.height = OSD_SD_HEIGHT;
		createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);

		for(i=0; i<FRAMEBUF_MAX; i++)
		{
			s_OsdList.SD_OsdInstance[i].surface = NEXUS_Surface_Create(&createSettings);
			if( s_OsdList.SD_OsdInstance[i].surface == NULL)
			{
				PrintError("NEXUS_Surface_Create SD(%d) error!!\n", i);
				return DRV_ERR_OUTOFMEMORY;
			}

			s_OsdList.SD_OsdInstance[i].status = DI_OSD_SURFACE_ENABLED;
			s_OsdList.SD_OsdInstance[i].format = DI_OSD_PIXEL_FORMAT_ARGB_8888;
			s_OsdList.SD_OsdInstance[i].rect.x =0;
			s_OsdList.SD_OsdInstance[i].rect.y =0;
			s_OsdList.SD_OsdInstance[i].rect.width = createSettings.width;
			s_OsdList.SD_OsdInstance[i].rect.height = createSettings.height;

			NEXUS_Surface_GetMemory(s_OsdList.SD_OsdInstance[i].surface, &surfmem);
			/* fill with transparent */
			VK_memset(surfmem.buffer, 0x00, createSettings.height * surfmem.pitch);

			/* added after nexus 12.xx */
			NEXUS_Surface_Flush(s_OsdList.SD_OsdInstance[i].surface);

			s_OsdList.SD_OsdInstance[i].pFBAddr = surfmem.buffer;
		}
	}
	else
	{
		PrintError("NEXUS not support SD Graphics(Display) !!!\n");
	}

	nRet = VK_SEM_Create(&s_ulOsdSemId, "di_osd", DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("Error(0x%x) in VK_SEM_Create()\n\r", nRet);
		return DRV_ERR;
	}

	s_OsdList.nexus_gfx_2DHandle = NEXUS_Graphics2D_Open(0, NULL);
	if( s_OsdList.nexus_gfx_2DHandle == NULL)
	{
		PrintError("NEXUS_Graphics2D_Open error!!\n");
		return DRV_ERR;
	}

	BKNI_CreateEvent(&s_OsdList.event);
	NEXUS_Graphics2D_GetSettings(s_OsdList.nexus_gfx_2DHandle, &gfxSettings);
	gfxSettings.checkpointCallback.callback = P_OSD_gfxEventComplete;
	gfxSettings.checkpointCallback.context = s_OsdList.event;
	NEXUS_Graphics2D_SetSettings(s_OsdList.nexus_gfx_2DHandle, &gfxSettings);

	for( loop = 0; loop < DI_OSD_SURFACE_MAX_NUM; loop++)
	{
		VK_memset(&s_aOsdInstance[loop], 0x00, sizeof(DI_OSD_INSTANCE));
#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
		VK_memset(&s_aOrgOsdInstance[loop], 0x00, sizeof(DI_OSD_INSTANCE));
#endif
	}

	VK_memset(&s_aBlendInstance, 0x00, sizeof(DI_OSD_INSTANCE));

	for( loop = 0; loop < DI_OSD_FB_MAX_NUM; loop++)
	{
		VK_memset(&s_aOsdFBInstance[loop], 0x00, sizeof(DI_OSD_INSTANCE));
	}

	for( loop = 0; loop < DI_OSD_MAX_PALETTE_ENTRIES; loop++)
	{
		VK_memset(&s_aOsdPalette[loop], 0x00, sizeof(DI_OSD_PALETTE));
	}

	/*
	display format 변경시 마다 Surface 를 다시 만들지 말고,
	HD Surface 는 1080 size 를 감안하여, (1280, 1080) 로 고정하여 생성하고,
	SD Surface 는 576i(PAL), 480i(NTSC) 감안하여, (640, 576) 으로 고정하여 생성하고,
	clip 을 조정하여 format 변경에 대처하자.
	*/
	if( DRV_OSD_SetDisplayFormatToHD() != DRV_OK)
	{
		PrintError("DRV_OSD_SetDisplayFormat error!!\n");
		return DRV_ERR;
	}

	if( DRV_OSD_SetDisplayFormatToSD() != DRV_OK)
	{
		PrintError("DRV_OSD_SetDisplayFormat error!!\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_GetCapability
 * Description	This function gets the capabitity of OSD block like a number of planes,
 *				screens, supported color format, and so on.
 * Inputs		NONE
 * Outputs		???
 * Returns		DDI_ERR_CODE_OK	if successful
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetCapability(DI_OSD_CAPABILITY *pOsdCapability)
{
	PrintEnter();

	pOsdCapability->ucNumLayer = DI_OSD_LAYER_MAX;
	pOsdCapability->ucNumSurface = DI_OSD_SURFACE_MAX_NUM;
	pOsdCapability->eBlendCapable = DI_OSD_BLEND_SCALING;

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_GetSurfaceInfo
 * Description	This function gets a surface information like pixel format, rectangle, layer.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetSurfaceInfo(DI_OSD_HANDLE hOsdHandle, DI_OSD_SURFACE_INFO *pInfo)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	NEXUS_SurfaceHandle hSurfaceHandle;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceMemory mem;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	hSurfaceHandle = pOsdInstance->surface;
	NEXUS_Surface_GetCreateSettings(hSurfaceHandle , &createSettings);
	NEXUS_Surface_GetMemory(hSurfaceHandle, &mem);

	pInfo->layer 		= pOsdInstance->layer;
	pInfo->format 		= pOsdInstance->format;
	pInfo->rect.x 		= pOsdInstance->rect.x;
	pInfo->rect.y 		= pOsdInstance->rect.y;
	pInfo->rect.width 	= pOsdInstance->rect.width;
	pInfo->rect.height 	= pOsdInstance->rect.height;
	pInfo->ulPlaneByteSize	= createSettings.height * mem.pitch;

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_GetFrameBufferInfo
 * Description	This function Return informations of the Frame buffer.
 * Inputs		Handle of frame buffer.
 * Outputs      Rect/memory/format/layer information
 * Returns		DI_ERR_CODE_OK	if successful
 *
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetFrameBufferInfo(DI_OSD_HANDLE hOsdHandle, DI_OSD_SURFACE_INFO *pInfo)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	NEXUS_SurfaceHandle hSurfaceHandle;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceMemory mem;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	hSurfaceHandle = pOsdInstance->surface;
	NEXUS_Surface_GetCreateSettings(hSurfaceHandle , &createSettings);
	NEXUS_Surface_GetMemory(hSurfaceHandle, &mem);

	pInfo->layer 			= pOsdInstance->layer;
	pInfo->format 		= pOsdInstance->format;
	pInfo->rect.x 		= pOsdInstance->rect.x;
	pInfo->rect.y 		= pOsdInstance->rect.y;
	pInfo->rect.width 	= pOsdInstance->rect.width;
	pInfo->rect.height 	= pOsdInstance->rect.height;
	pInfo->ulPlaneByteSize	= createSettings.height * mem.pitch;

	PrintExit();

	return DI_ERR_OK;
}
/*********************************************************************
 *
 * Function 	DI_OSD_CreateSurface
 * Description	This function makes a new screen on an specified plane with the specified
 *				position and size.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_CreateSurface(DI_OSD_LAYER nOsdLayer,
		DI_OSD_RECT rect,
		DI_OSD_PIXEL_FORMAT format,
		DI_OSD_HANDLE *pOsdHandle)
{
	int 				i;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceHandle surface;
	NEXUS_SurfaceMemory frameBufferMem;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		if( nOsdLayer >= DI_OSD_LAYER_MAX)
		{
			PrintError("Invalid Layer_%d :: LAYER_MAX is %d\n", nOsdLayer, DI_OSD_LAYER_MAX);
			nErr= DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( s_aOsdInstance[nOsdLayer].status != DI_OSD_DISABLED)
		{
			PrintError("LAYER_%d Surface is already created ~~!!!\n", nOsdLayer);
			nErr= DI_ERR_ERROR;
			goto done;
		}

		i = (int)nOsdLayer;

		NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
#if defined(CONFIG_DEVICE_MODE_KERNEL) || defined (CONFIG_OS_UCOS)
		createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
		switch( format)
		{
			case DI_OSD_PIXEL_FORMAT_ARGB_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
				break;
			case DI_OSD_PIXEL_FORMAT_RGBA_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_4444:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA4_R4_G4_B4;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_1555:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA1_R5_G5_B5;
				break;
			case DI_OSD_PIXEL_FORMAT_CLUT_8:
				createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
				break;
			default:
				PrintError("Not supported color format(%d)\n", format);
				nErr= DI_ERR_INVALID_PARAM;
				goto done;
				break;
		}

		createSettings.width = rect.width;
	    createSettings.height = rect.height;

	    surface = NEXUS_Surface_Create(&createSettings);
		if( surface == NULL)
		{
			PrintError("NEXUS_Surface_Create error!!\n");
			DRV_OSD_PrintInfo();
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		NEXUS_Surface_GetMemory(surface, &frameBufferMem);

		s_aOsdInstance[i].status = DI_OSD_SURFACE_ENABLED;
		s_aOsdInstance[i].rect.x = rect.x;
		s_aOsdInstance[i].rect.y = rect.y;
		s_aOsdInstance[i].rect.width = rect.width;
		s_aOsdInstance[i].rect.height = rect.height;
		s_aOsdInstance[i].surface = surface;
		s_aOsdInstance[i].format = format;
		s_aOsdInstance[i].pFBAddr = frameBufferMem.buffer;
		s_aOsdInstance[i].layer = nOsdLayer;

		*pOsdHandle = (DI_OSD_HANDLE)&s_aOsdInstance[i];

		PrintDebug("Surf[%d]=Handle:0x%x, w(%d),h(%d), format(%d)\n", i, *pOsdHandle, rect.width, rect.height, format);

		// MULTI LAYER 지원을 위해, LAYER_0 의 Main OSD Surface (1280x720) 과 같은 size 의 Flush 용 Blend Surface 가 필요하다.
		if (nOsdLayer == DI_OSD_LAYER_0 && s_aBlendInstance.status == DI_OSD_DISABLED)
		{
			surface = NEXUS_Surface_Create(&createSettings);
			if( surface == NULL)
			{
				PrintError("NEXUS_Surface_Create error!!\n");
				DRV_OSD_PrintInfo();
				goto done;
			}

			NEXUS_Surface_GetMemory(surface, &frameBufferMem);

			s_aBlendInstance.status = DI_OSD_SURFACE_ENABLED;
			s_aBlendInstance.rect.x = rect.x;
			s_aBlendInstance.rect.y = rect.y;
			s_aBlendInstance.rect.width = rect.width;
			s_aBlendInstance.rect.height = rect.height;
			s_aBlendInstance.surface = surface;
			s_aBlendInstance.format = format;
			s_aBlendInstance.pFBAddr = frameBufferMem.buffer;
			s_aBlendInstance.layer = nOsdLayer;
		}

		PrintDebug("Blend Surf[%d]=Handle:0x%x, w(%d),h(%d), format(%d)\n", i, &s_aBlendInstance, rect.width, rect.height, format);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_CreateFrameBuffer
 * Description	This function makes a new screen on an specified plane with the specified
 *				position and size.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_CreateFrameBuffer( unsigned long width, unsigned long height,
										DI_OSD_PIXEL_FORMAT format,
										DI_OSD_HANDLE *pOsdHandle)
{
	int 				i;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceHandle surface;
	NEXUS_SurfaceMemory frameBufferMem;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		for( i = 0; i < DI_OSD_FB_MAX_NUM; i++)
		{
			if( s_aOsdFBInstance[ i].status == DI_OSD_DISABLED)
			{
				break;
			}
		}
		if( i == DI_OSD_FB_MAX_NUM)
		{
			PrintError("Not enough FB resource!!\n");
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		/* Make Window */

		NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
#if defined(CONFIG_DEVICE_MODE_KERNEL) || defined (CONFIG_OS_UCOS)
		createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
		switch( format)
		{
			case DI_OSD_PIXEL_FORMAT_ARGB_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
				break;
			case DI_OSD_PIXEL_FORMAT_RGBA_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_4444:		// For MHEG Bitmap Pixel Format
				createSettings.pixelFormat = NEXUS_PixelFormat_eA4_R4_G4_B4;
				break;
			case DI_OSD_PIXEL_FORMAT_RGBA_4444:		// For MHEG Bitmap Pixel Format
				createSettings.pixelFormat = NEXUS_PixelFormat_eR4_G4_B4_A4;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_1555:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA1_R5_G5_B5;
				break;
			case DI_OSD_PIXEL_FORMAT_CLUT_8:
				createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
				break;
			case DI_OSD_PIXEL_FORMAT_A_8:			// For MHEG Mask Pixel Format
				createSettings.pixelFormat = NEXUS_PixelFormat_eA8;
				break;
			default:
				PrintError("Not supported color format(%d)\n", format);
				nErr= DI_ERR_INVALID_PARAM;
				goto done;
				break;
		}

		createSettings.width = width;
		createSettings.height = height;

		surface = NEXUS_Surface_Create(&createSettings);
		if( surface == NULL)
		{
			PrintError("NEXUS_Surface_Create error!! (i:%d, width=%d, height=%d)\n", i, width, height);
			DRV_OSD_PrintInfo();
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		NEXUS_Surface_GetMemory(surface, &frameBufferMem);

		s_aOsdFBInstance[i].status = DI_OSD_FB_ENABLED;
		s_aOsdFBInstance[i].rect.x = 0;
		s_aOsdFBInstance[i].rect.y = 0;
		s_aOsdFBInstance[i].rect.width = width;
		s_aOsdFBInstance[i].rect.height = height;
		s_aOsdFBInstance[i].surface = surface;
		s_aOsdFBInstance[i].format = format;
		s_aOsdFBInstance[i].pFBAddr = frameBufferMem.buffer;
		s_aOsdFBInstance[i].layer = DI_OSD_LAYER_0;

		*pOsdHandle = (DI_OSD_HANDLE)&s_aOsdFBInstance[i];

		PrintDebug("FB[%d]=Handle:0x%x, w(%d),h(%d), format(%d)\n", i, *pOsdHandle, width, height, format);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return nErr;
}

/*********************************************************************
 *
 * Function 	DI_OSD_DeleteSurface
 * Description	This function destroys a screen created by DI_OSD_CreateScreen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_DeleteSurface(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
		{
			PrintError("P_OSD_CheckHandle error!!\n");
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
		{
			PrintError("Status(%d) error!!\n", pOsdInstance->status);
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		NEXUS_Surface_Destroy(pOsdInstance->surface);

		// Deleted Surface (Layer) 는 Hide 처리.
		s_OsdList.b_Layer_Show_nHide[pOsdInstance->layer] = FALSE;

		pOsdInstance->status = DI_OSD_DISABLED;
		pOsdInstance->rect.x = 0;
		pOsdInstance->rect.y = 0;
		pOsdInstance->rect.width = 0;
		pOsdInstance->rect.height = 0;
		pOsdInstance->surface = NULL;
		pOsdInstance->format = DI_OSD_PIXEL_FORMAT_ARGB_8888;
		pOsdInstance->layer = DI_OSD_LAYER_MAX;
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return nErr;
}

/*********************************************************************
 *
 * Function 	DI_OSD_DeleteFrameBuffer
 * Description	This function destroys a screen created by DI_OSD_CreateScreen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_DeleteFrameBuffer(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
		{
			PrintError("P_OSD_CheckHandle error!!\n");
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( pOsdInstance->status != DI_OSD_FB_ENABLED)
		{
			PrintError("Status(%d) error!!\n", pOsdInstance->status);
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		NEXUS_Surface_Destroy(pOsdInstance->surface);

		pOsdInstance->status = DI_OSD_DISABLED;
		pOsdInstance->rect.x = 0;
		pOsdInstance->rect.y = 0;
		pOsdInstance->rect.width = 0;
		pOsdInstance->rect.height = 0;
		pOsdInstance->surface = NULL;
		pOsdInstance->format = DI_OSD_PIXEL_FORMAT_ARGB_8888;
		pOsdInstance->layer = DI_OSD_LAYER_MAX;
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_Show
 * Description	This function causes a screen to be displayed if the associated plane is
 *				currently being displayed.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
 DI_ERR_CODE	DI_OSD_Show(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if (s_OsdList.b_Layer_Show_nHide[pOsdInstance->layer] != TRUE)
	{
		PrintDebug("SHOW:: hOsdHandle(0x%x), layer(%d)\n", hOsdHandle, pOsdInstance->layer);

		s_OsdList.b_Layer_Show_nHide[pOsdInstance->layer] = TRUE;

		// OSD Plane Refresh
		DRV_OSD_Refresh(pOsdInstance->layer);
	}
	else
	{
		PrintDebug("Already SHOW state:: hOsdHandle(0x%x), layer(%d)\n", hOsdHandle, pOsdInstance->layer);
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_OSD_Hide(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if (s_OsdList.b_Layer_Show_nHide[pOsdInstance->layer])
	{
		PrintDebug("HIDE:: hOsdHandle(0x%x), layer(%d)\n", hOsdHandle, pOsdInstance->layer);

		// OSD Plane Refresh
		s_OsdList.b_Layer_Show_nHide[pOsdInstance->layer] = FALSE;

		DRV_OSD_Refresh(pOsdInstance->layer);
	}
	else
	{
		PrintDebug("Already HIDE state:: hOsdHandle(0x%x), layer(%d)\n", hOsdHandle, pOsdInstance->layer);
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_Flush(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pRect)
{
	DI_ERR_CODE			nRet = DI_ERR_OK;
	DI_OSD_INSTANCE			*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if (s_OsdList.b_Layer_Show_nHide[pOsdInstance->layer] == FALSE)
	{
		return DI_ERR_ERROR;
	}

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		nRet = P_OSD_Flush(hOsdHandle, pRect);
		if (nRet != DI_ERR_OK)
		{
			PrintError("Error in P_OSD_Flush()\n");
		}

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nRet= DI_ERR_ERROR;
	}

	PrintExit();

	return nRet;

}


/*********************************************************************
 *
 * Function 	DI_OSD_GetFrameBuffer
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetFrameBuffer(DI_OSD_HANDLE hOsdHandle, void **pFrameBuffer)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	*pFrameBuffer = pOsdInstance->pFBAddr;

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_CreatePalette
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_CreatePalette(DI_OSD_COLOR_FORMAT nColorFmt, DI_OSD_PALETTE **pPaletteHandle)
{
	int	palette_id;
	int	loop;

	PrintEnter();

	switch( nColorFmt)
	{
		case DI_OSD_COLOR_FORMAT_RGB:
			for( loop = 0; loop < DI_OSD_MAX_PALETTE_ENTRIES; loop++)
			{
				if(s_aOsdPalette[loop].pRGB == (DI_OSD_COLOR_RGB *)NULL)
				{
					palette_id = loop;
					break;
				}
			}
			if(loop >= DI_OSD_MAX_PALETTE_ENTRIES)
			{
				PrintError("No more palette entries.\n");
				return DI_ERR_NO_RESOURCE;
			}

			s_aOsdPalette[ palette_id].pRGB = (DI_OSD_COLOR_RGB *)DD_MEM_Alloc(sizeof(DI_OSD_COLOR_RGB) *DI_OSD_8BIT_PALETTE_ENTRIES);
			if(s_aOsdPalette[ palette_id].pRGB == NULL)
			{
				PrintError("No more memory.\n");
				return DI_ERR_NO_ALLOC;
			}

			VK_memset(s_aOsdPalette[ palette_id].pRGB, 0, sizeof(DI_OSD_COLOR_RGB) *DI_OSD_8BIT_PALETTE_ENTRIES);
			s_aOsdPalette[ palette_id].nColorFormat = nColorFmt;
			*pPaletteHandle = &s_aOsdPalette[palette_id];
			break;

		default:
			return DI_ERR_INVALID_PARAM;
			break;
	}

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_DeletePalette
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_DeletePalette(DI_OSD_PALETTE *pPaletteHandle)
{
	PrintEnter();

	if( pPaletteHandle->pRGB)
	{
		DD_MEM_Free(pPaletteHandle->pRGB);
	}
	pPaletteHandle->pRGB = (DI_OSD_COLOR_RGB *)NULL;
	pPaletteHandle->nColorFormat = 0;

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_ChangePalette
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_SetPalette(DI_OSD_HANDLE hOsdHandle, DI_OSD_PALETTE *pPaletteHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	NEXUS_SurfaceMemory mem;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->format != DI_OSD_PIXEL_FORMAT_CLUT_8)
	{
		PrintError("Pixel format error!! (pixel format: %d)\n", pOsdInstance->format);
		return DI_ERR_INVALID_PARAM;
	}

	NEXUS_Surface_GetMemory(pOsdInstance->surface, &mem);

	VK_memcpy((void *)mem.palette, (const void *)pPaletteHandle->pRGB, mem.numPaletteEntries*sizeof(NEXUS_Pixel));

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_ChangeColor
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_ChangeColor(DI_OSD_HANDLE hOsdHandle, unsigned char ucIndex, unsigned long ulColor)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	NEXUS_SurfaceMemory mem;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->format != DI_OSD_PIXEL_FORMAT_CLUT_8)
	{
		PrintError("Pixel format error!! (pixel format: %d)\n", pOsdInstance->format);
		return DI_ERR_INVALID_PARAM;
	}

	NEXUS_Surface_GetMemory(pOsdInstance->surface, &mem);

	if( ucIndex >= mem.numPaletteEntries)
	{
		PrintError("ucIndex too large error!! (ucIndex: %d, mem.numPaletteEntries: %d)\n", ucIndex, mem.numPaletteEntries);
		return DI_ERR_INVALID_PARAM;
	}

	mem.palette[ucIndex] = ulColor;

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 * Function 	DI_OSD_SetOutWindow
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_ERR_CODE	DI_OSD_SetOutWindow(DI_OSD_LAYER nOsdLayer, DI_OSD_RECT *pRect)
{
	BSTD_UNUSED(nOsdLayer);
	BSTD_UNUSED(pRect);

	PrintError("Not Supported Funtion. Why did you call %s in KRAKEN??? \n", __func__);

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_BlendOSDBlock
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_BlendOSDBlock(DI_OSD_HANDLE hSrcHandle,
		DI_OSD_RECT srcRect,
		DI_OSD_HANDLE hDstHandle,
		DI_OSD_RECT dstRect,
/*		Source + Dest -> Dest 형태로 운영하기로 함.
		DI_OSD_HANDLE hOutHandle,
		DI_OSD_RECT outRect,
*/
		DI_OSD_RASTER_OP rop)
{
	DI_OSD_INSTANCE	*pSrcInstance = (DI_OSD_INSTANCE *)hSrcHandle;
	DI_OSD_INSTANCE	*pDstInstance = (DI_OSD_INSTANCE *)hDstHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if( P_OSD_CheckHandle( hSrcHandle) != DI_ERR_OK || P_OSD_CheckHandle( hDstHandle) != DI_ERR_OK )
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( P_OSD_CheckRect( pSrcInstance->rect, srcRect) != DI_ERR_OK || P_OSD_CheckRect( pDstInstance->rect, dstRect) != DI_ERR_OK)
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		PrintError("src(0x%x) (%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
				hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);
		return DI_ERR_INVALID_PARAM;
	}

#if	defined(CONFIG_SW_OSD_BLEND_COPY)
	if ((dstRect.width * dstRect.height) < THRESHOLD_SIZE_STOCK_DMA_COPY && (dstRect.width==srcRect.width) && (dstRect.height==srcRect.height))
	{
		return P_OSD_DrawImageOfStockFrame (hSrcHandle, srcRect, hDstHandle, dstRect, rop);
	}
#endif

	if ( P_OSD_EnablePrint(hSrcHandle) && P_OSD_EnablePrint(hDstHandle) )
		PrintDebug("Blend -> src(0x%x) (%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
				hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		/* flush cached memory */
    	NEXUS_Surface_Flush(pSrcInstance->surface);
    	NEXUS_Surface_Flush(pDstInstance->surface);

		nErr = P_OSD_Blit(pSrcInstance, srcRect, pDstInstance, dstRect, pDstInstance, dstRect, rop, 0, FALSE);
		if (nErr != DI_ERR_OK)
		{
			PrintError("ERROR in P_OSD_Blit(), nErr(0x%x), rop(0x%x)\n", nErr, rop);
		}

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_CopyOSDBlock
 * Description	This function copies the rectangle from a  source screen to the specified
 *		 		position in a destination screen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_CopyOSDBlock(DI_OSD_HANDLE hSrcHandle,
		DI_OSD_RECT srcRect,
		DI_OSD_HANDLE hDstHandle,
		DI_OSD_RECT dstRect)
{
	DI_OSD_INSTANCE	*pSrcInstance = (DI_OSD_INSTANCE *)hSrcHandle;
	DI_OSD_INSTANCE	*pDstInstance = (DI_OSD_INSTANCE *)hDstHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if( P_OSD_CheckHandle( hSrcHandle) != DI_ERR_OK || P_OSD_CheckHandle( hDstHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( P_OSD_CheckRect( pSrcInstance->rect, srcRect) != DI_ERR_OK || P_OSD_CheckRect( pDstInstance->rect, dstRect) != DI_ERR_OK)
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		PrintError("src(0x%x)(%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
				hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);
		return DI_ERR_INVALID_PARAM;
	}

#if	defined(CONFIG_SW_OSD_BLEND_COPY)
	if ((dstRect.width * dstRect.height) < THRESHOLD_SIZE_STOCK_DMA_COPY && (dstRect.width==srcRect.width) && (dstRect.height==srcRect.height))
	{
		return P_OSD_DrawImageOfStockFrame (hSrcHandle, srcRect, hDstHandle, dstRect, DI_OSD_RASTER_OP_SRC);
	}
#endif

	if ( P_OSD_EnablePrint(hSrcHandle) && P_OSD_EnablePrint(hDstHandle) )
		PrintDebug("Copy -> src(0x%x)(%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
				hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		/* flush cached memory */
    	NEXUS_Surface_Flush(pSrcInstance->surface);

		nErr = P_OSD_Blit(pSrcInstance, srcRect, NULL, dstRect, pDstInstance, dstRect, DI_OSD_RASTER_OP_SRC, 0, FALSE);
		if (nErr != DI_ERR_OK)
		{
			PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nErr);
		}

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

	PrintExit();

	return nErr;
}

/*********************************************************************
 *
 * Function 	DI_OSD_FillOSDBlock
 * Description	This function copies the rectangle from a  source screen to the specified
 *		 		position in a destination screen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_FillOSDBlock(DI_OSD_HANDLE hOsdHandle,
		DI_OSD_RECT rect,
		unsigned long color)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE			nErr = DI_ERR_OK;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}
	if( P_OSD_CheckRect( pOsdInstance->rect, rect))
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		PrintError("hOsdHandle:0x%x, rect(%d,%d,%d,%d), color:0x%08x\n",
				hOsdHandle, rect.x, rect.y, rect.width, rect.height, color);
		return DI_ERR_INVALID_PARAM;
	}

	if ( P_OSD_EnablePrint(hOsdHandle) )
		PrintDebug("Fill -> hOsdHandle:0x%x, rect(%d,%d,%d,%d), color:0x%08x\n",
			hOsdHandle, rect.x, rect.y, rect.width, rect.height, color);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		/* flush cached memory */
	    NEXUS_Surface_Flush(pOsdInstance->surface);

		nErr = P_OSD_Fill(pOsdInstance, rect, color);
		if (nErr != DI_ERR_OK)
		{
			PrintError("ERROR in P_OSD_Fill(), nErr(0x%x)\n", nErr);
		}

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

	PrintExit();

	return nErr;
}

/*********************************************************************
 *
 * Function 	DI_OSD_BlendFillOSDBlock
 * Description	This function copies the rectangle from a  source screen to the specified
 *		 		position in a destination screen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_BlendFillOSDBlock(DI_OSD_HANDLE hOsdHandle,
		DI_OSD_RECT rect,
		unsigned long color)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE			nErr = DI_ERR_OK;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}
	if( P_OSD_CheckRect( pOsdInstance->rect, rect))
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		PrintError("hOsdHandle:0x%x, rect(%d,%d,%d,%d), color:0x%08x\n",
				hOsdHandle, rect.x, rect.y, rect.width, rect.height, color);
		return DI_ERR_INVALID_PARAM;
	}

	if ( P_OSD_EnablePrint(hOsdHandle) )
		PrintDebug("Fill -> hOsdHandle:0x%x, rect(%d,%d,%d,%d), color:0x%08x\n",
			hOsdHandle, rect.x, rect.y, rect.width, rect.height, color);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		/* flush cached memory */
	    NEXUS_Surface_Flush(pOsdInstance->surface);

		nErr = P_OSD_BlendFill(pOsdInstance, rect, color);
		if (nErr != DI_ERR_OK)
		{
			PrintError("ERROR in P_OSD_Fill(), nErr(0x%x)\n", nErr);
		}

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

	PrintExit();

	return nErr;
}

/*********************************************************************
 *
 * Function 	DI_OSD_SetAlphaOSDBlock
 * Description	This function copies the rectangle from a  source screen to the specified
 *		 		position in a destination screen with alpha parameter.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_SetAlphaOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect,
									DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect,
									DI_OSD_RASTER_OP rop, unsigned char ucAlpha)
{
	DI_OSD_INSTANCE	*pSrcInstance = (DI_OSD_INSTANCE *)hSrcHandle;
	DI_OSD_INSTANCE	*pDstInstance = (DI_OSD_INSTANCE *)hDstHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if( P_OSD_CheckHandle( hSrcHandle) != DI_ERR_OK || P_OSD_CheckHandle( hDstHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( P_OSD_CheckRect( pSrcInstance->rect, srcRect) != DI_ERR_OK || P_OSD_CheckRect( pDstInstance->rect, dstRect) != DI_ERR_OK)
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		PrintError("src(0x%x)(%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
				hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);
		return DI_ERR_INVALID_PARAM;
	}

	if ( P_OSD_EnablePrint(hSrcHandle) && P_OSD_EnablePrint(hDstHandle) )
		PrintDebug("SetAlpha -> src(0x%x)(%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d), ucAlpha(0x%x)\n",
				hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height, ucAlpha);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		/* flush cached memory */
    	NEXUS_Surface_Flush(pSrcInstance->surface);

		nErr = P_OSD_Blit(pSrcInstance, srcRect, NULL, dstRect, pDstInstance, dstRect, rop, (HUINT32)(ucAlpha<<24), FALSE);
		if (nErr != DI_ERR_OK)
		{
			PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nErr);
		}

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

	PrintExit();

	return nErr;
}

DI_ERR_CODE    DI_OSD_FilterOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect,
                                    DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect,
                                    DI_OSD_RASTER_OP rop, unsigned int ulConstantColor, int nFilter)
{
	DI_OSD_INSTANCE    *pSrcInstance = (DI_OSD_INSTANCE *)hSrcHandle;
	DI_OSD_INSTANCE    *pDstInstance = (DI_OSD_INSTANCE *)hDstHandle;
	DI_ERR_CODE    nErr = DI_ERR_OK;

	PrintEnter();

	if( P_OSD_CheckHandle( hSrcHandle) != DI_ERR_OK || P_OSD_CheckHandle( hDstHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( P_OSD_CheckRect( pSrcInstance->rect, srcRect) != DI_ERR_OK || P_OSD_CheckRect( pDstInstance->rect, dstRect) != DI_ERR_OK)
	{
		PrintError("Invalid pRect:: pRect(check) is larger than created(base) rect !!\n");
		PrintError("src(0x%x)(%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
			hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
			hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);
		return DI_ERR_INVALID_PARAM;
	}

	if ( P_OSD_EnablePrint(hSrcHandle) && P_OSD_EnablePrint(hDstHandle) )
		PrintDebug("SetAlpha -> src(0x%x)(%d,%d,%d,%d), dest(0x%x)(%d,%d,%d,%d)\n",
			hSrcHandle, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
			hDstHandle, dstRect.x, dstRect.y, dstRect.width, dstRect.height);

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		/* flush cached memory */
		NEXUS_Surface_Flush(pSrcInstance->surface);

		if(nFilter >= 0)
		{
			DRV_OSD_SetFilterCoeffs(nFilter);
		}
		nErr = P_OSD_Blit(pSrcInstance, srcRect, pDstInstance, dstRect, pDstInstance, dstRect, rop, ulConstantColor, TRUE);
		if (nErr != DI_ERR_OK)
		{
			PrintError("ERROR in P_OSD_Blit(), nErr(0x%x)\n", nErr);
		}
		if(nFilter >= 0)
		{
			DRV_OSD_SetFilterCoeffs(NEXUS_Graphics2DFilterCoeffs_eBlurry);
		}
		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

	PrintExit();

	return nErr;
}


#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
/*********************************************************************
 *
 * Function 	DI_OSD_CreateSurfaceWithOrg
 * Description	This function makes a new screen on an specified plane with the specified
 *				position and size.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER nOsdLayer,
		DI_OSD_RECT rect,
		DI_OSD_PIXEL_FORMAT format,
		DI_OSD_HANDLE *pOsdHandle,
		HBOOL bUseBGPlane)
{
	int 				i;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceHandle surface;
	NEXUS_SurfaceMemory frameBufferMem;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		if( nOsdLayer >= DI_OSD_LAYER_MAX)
		{
			PrintError("Invalid Layer_%d :: LAYER_MAX is %d\n", nOsdLayer, DI_OSD_LAYER_MAX);
			nErr= DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( s_aOsdInstance[nOsdLayer].status != DI_OSD_DISABLED)
		{
			PrintError("LAYER_%d Surface is already created ~~!!!\n", nOsdLayer);
			nErr= DI_ERR_ERROR;
			goto done;
		}

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		if ( bUseBGPlane && (nOsdLayer != DI_OSD_BG_LAYER) )
		{
			PrintError("Invalid Layer_%d :: BackGround Plane must be Layer_%d because of LAYER_MAX(%d)\n", nOsdLayer, DI_OSD_BG_LAYER, DI_OSD_LAYER_MAX);
			nErr= DI_ERR_INVALID_PARAM;
			goto done;
		}
#endif

		i = (int)nOsdLayer;

		NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
#if defined(CONFIG_DEVICE_MODE_KERNEL) || defined (CONFIG_OS_UCOS)
		createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
		switch( format)
		{
			case DI_OSD_PIXEL_FORMAT_ARGB_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
				break;
			case DI_OSD_PIXEL_FORMAT_RGBA_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_1555:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA1_R5_G5_B5;
				break;
			case DI_OSD_PIXEL_FORMAT_CLUT_8:
				createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
				break;
			default:
				PrintError("Not supported color format(%d)\n", format);
				nErr= DI_ERR_INVALID_PARAM;
				goto done;
				break;
		}

		createSettings.width = rect.width;
	    createSettings.height = rect.height;

		/* Main surface Create */
	    surface = NEXUS_Surface_Create(&createSettings);
		if( surface == NULL)
		{
			PrintError("NEXUS_Surface_Create error!!\n");
			DRV_OSD_PrintInfo();
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		NEXUS_Surface_GetMemory(surface, &frameBufferMem);

		/* fill with transparent */
	    VK_memset(frameBufferMem.buffer, 0x00, createSettings.height * frameBufferMem.pitch);

		s_aOsdInstance[i].status = DI_OSD_SURFACE_ENABLED;
		s_aOsdInstance[i].rect.x = rect.x;
		s_aOsdInstance[i].rect.y = rect.y;
		s_aOsdInstance[i].rect.width = rect.width;
		s_aOsdInstance[i].rect.height = rect.height;
		s_aOsdInstance[i].surface = surface;
		s_aOsdInstance[i].format = format;
		s_aOsdInstance[i].pFBAddr = frameBufferMem.buffer;
		s_aOsdInstance[i].layer = nOsdLayer;

		/* Clone surface Create */
		surface = NEXUS_Surface_Create(&createSettings);
		if( surface == NULL)
		{
			PrintError("NEXUS_Surface_Create error!!\n");
			DRV_OSD_PrintInfo();
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		NEXUS_Surface_GetMemory(surface, &frameBufferMem);

		/* fill with transparent */
	    VK_memset(frameBufferMem.buffer, 0x00, createSettings.height * frameBufferMem.pitch);

		s_aOrgOsdInstance[i].status = DI_OSD_SURFACE_ENABLED;
		s_aOrgOsdInstance[i].rect.x = rect.x;
		s_aOrgOsdInstance[i].rect.y = rect.y;
		s_aOrgOsdInstance[i].rect.width = rect.width;
		s_aOrgOsdInstance[i].rect.height = rect.height;
		s_aOrgOsdInstance[i].surface = surface;
		s_aOrgOsdInstance[i].format = format;
		s_aOrgOsdInstance[i].pFBAddr = frameBufferMem.buffer;
		s_aOrgOsdInstance[i].layer = nOsdLayer;

		/* Org Exist flag SET */
		s_aOsdInstance[i].bOrgExist = TRUE;
		s_aOrgOsdInstance[i].bOrgExist = TRUE;

		/* Org Dirty flag SET */
		s_aOrgOsdInstance[i].bOrgDirty = TRUE;

#if defined(CONFIG_SCALABLE_OSD_SURFACE)
		s_aOrgOsdInstance[i].In_rect.x = rect.x;
		s_aOrgOsdInstance[i].In_rect.y = rect.y;
		s_aOrgOsdInstance[i].In_rect.width = rect.width;
		s_aOrgOsdInstance[i].In_rect.height = rect.height;

		s_aOrgOsdInstance[i].Out_rect.x = rect.x;
		s_aOrgOsdInstance[i].Out_rect.y = rect.y;
		s_aOrgOsdInstance[i].Out_rect.width = rect.width;
		s_aOrgOsdInstance[i].Out_rect.height = rect.height;
#endif

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
		s_aOrgOsdInstance[i].bUseBG = bUseBGPlane;

		s_aOrgOsdInstance[i].BG_rect.x = rect.x;
		s_aOrgOsdInstance[i].BG_rect.y = rect.y;
		s_aOrgOsdInstance[i].BG_rect.width = rect.width;
		s_aOrgOsdInstance[i].BG_rect.height = rect.height;

		s_aOrgOsdInstance[i].nBG_Status = DI_OSD_BG_STATUS_FULL;
#endif

		/* pOsdHandle is "s_aOrgOsdInstance" not s_aOsdInstance */
		*pOsdHandle = (DI_OSD_HANDLE)&s_aOrgOsdInstance[i];

		PrintDebug("Surf[%d]=Handle:0x%x, w(%d),h(%d), format(%d)\n", i, *pOsdHandle, rect.width, rect.height, format);

		// MULTI LAYER 지원을 위해, LAYER_0 의 Main OSD Surface (1280x720) 과 같은 size 의 Flush 용 Blend Surface 가 필요하다.
		if (nOsdLayer == DI_OSD_LAYER_0)
		{
			surface = NEXUS_Surface_Create(&createSettings);
			if( surface == NULL)
			{
				PrintError("NEXUS_Surface_Create error!!\n");
				DRV_OSD_PrintInfo();
				goto done;
			}

			NEXUS_Surface_GetMemory(surface, &frameBufferMem);

			s_aBlendInstance.status = DI_OSD_SURFACE_ENABLED;
			s_aBlendInstance.rect.x = rect.x;
			s_aBlendInstance.rect.y = rect.y;
			s_aBlendInstance.rect.width = rect.width;
			s_aBlendInstance.rect.height = rect.height;
			s_aBlendInstance.surface = surface;
			s_aBlendInstance.format = format;
			s_aBlendInstance.pFBAddr = frameBufferMem.buffer;
			s_aBlendInstance.layer = nOsdLayer;
		}

		PrintDebug("Blend Surf[%d]=Handle:0x%x, w(%d),h(%d), format(%d)\n", i, &s_aBlendInstance, rect.width, rect.height, format);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	DI_OSD_DeleteSurfaceWithOrg
 * Description	This function destroys a screen created by DI_OSD_CreateScreen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_DeleteSurfaceWithOrg(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;
	DI_OSD_LAYER	nLayer;

	PrintEnter();

	nLayer = pOsdInstance->layer;

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
		{
			PrintError("P_OSD_CheckHandle error!!\n");
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
		{
			PrintError("Status(%d) error!!\n", pOsdInstance->status);
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( pOsdInstance->bOrgExist != TRUE)
		{
			PrintError("This Surface is not surface with clone (pOsdInstance->bOrgExist=0x%x). Use DI_OSD_DeleteSurface~!!!\n", pOsdInstance->bOrgExist);
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		NEXUS_Surface_Destroy(s_aOsdInstance[nLayer].surface);
		NEXUS_Surface_Destroy(s_aOrgOsdInstance[nLayer].surface);

		VK_memset(&s_aOsdInstance[nLayer], 0x00, sizeof(DI_OSD_INSTANCE));
		VK_memset(&s_aOrgOsdInstance[nLayer], 0x00, sizeof(DI_OSD_INSTANCE));

		// Deleted Surface (Layer) 는 Hide 처리.
		s_OsdList.b_Layer_Show_nHide[nLayer] = FALSE;
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return nErr;
}
#endif

#if defined(CONFIG_SCALABLE_OSD_SURFACE)
/*********************************************************************
 *
 * Function 	DI_OSD_SetInOutRect
 * Description	This function  make scalable osd plane coordination.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_SetInOutRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pInRect, DI_OSD_RECT *pOutRect)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE			nRet = DI_ERR_OK;
	HBOOL				bChangeInRect = FALSE;
	HBOOL				bChangeOutRect = FALSE;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->bOrgExist != TRUE)
	{
		PrintError("ERROR :: Don't Exist Org surface in this layer : hOsdHandle(0x%x), layer(%d), pOsdInstance->bOrgExist(%d)\n",
			hOsdHandle, pOsdInstance->layer, pOsdInstance->bOrgExist);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pInRect == NULL)
	{
		PrintDebug("pInRect is NULL!! instance In_rect is same~!\n");
		bChangeInRect = FALSE;
	}
	else
	{
		if ( P_OSD_DiffCheck(pOsdInstance->In_rect, *pInRect) )
		{
			bChangeInRect = TRUE;

			if( P_OSD_CheckRect( pOsdInstance->rect, *pInRect) != DI_ERR_OK)
			{
				PrintError("Invalid pInRect :: pInRect(check) is larger than created(base) rect !!\n");
				PrintError("rect(%d,%d,%d,%d), pInRect(%d,%d,%d,%d)\n",
						pOsdInstance->rect.x, pOsdInstance->rect.y, pOsdInstance->rect.width, pOsdInstance->rect.height,
						pInRect->x, pInRect->y, pInRect->width, pInRect->height);
				return DI_ERR_INVALID_PARAM;
			}

			/* In Rectangle value update~! */
			VK_memcpy(&pOsdInstance->In_rect, pInRect, sizeof(DI_OSD_RECT));
		}
		else
		{
			bChangeInRect = FALSE;
		}
	}

	if( pOutRect == NULL)
	{
		PrintDebug("pOutRect is NULL!! instance Out_rect is same \n");
		bChangeOutRect = FALSE;
	}
	else
	{
		if ( P_OSD_DiffCheck(pOsdInstance->Out_rect, *pOutRect) )
		{
			bChangeOutRect = TRUE;

			if( P_OSD_CheckRect( pOsdInstance->rect, *pOutRect) != DI_ERR_OK)
			{
				PrintError("Invalid pOutRect :: pOutRect(check) is larger than created(base) rect !!\n");
				PrintError("rect(%d,%d,%d,%d), pOutRect(%d,%d,%d,%d)\n",
						pOsdInstance->rect.x, pOsdInstance->rect.y, pOsdInstance->rect.width, pOsdInstance->rect.height,
						pOutRect->x, pOutRect->y, pOutRect->width, pOutRect->height);
				return DI_ERR_INVALID_PARAM;
			}

			/* Out Rectangle value update~! */
			VK_memcpy(&pOsdInstance->Out_rect, pOutRect, sizeof(DI_OSD_RECT));
		}
		else
		{
			bChangeOutRect = FALSE;
		}
	}

// 처리속도가 문제가 되어, 같은 좌표 call 에 대해 passthrough 해야 한다면, 아래 #if 0 열어서 사용하자.
#if	0
	if (bChangeInRect==FALSE && bChangeOutRect==FALSE && pOsdInstance->bOrgDirty == FALSE)
	{
		PrintError("No Changed In/Out Rect Value : No Action return~!\n");
		return DI_ERR_OK;
	}
#endif

	nRet = P_OSD_MakeDispSurface(hOsdHandle, bChangeInRect, bChangeOutRect);
	if (nRet != DI_ERR_OK)
	{
		PrintError("ERROR in P_OSD_MakeDispSurface(), nRet(0x%x)\n", nRet);
	}

	pOsdInstance->bOrgDirty = FALSE;

	PrintExit();

	return nRet;
}


/*********************************************************************
 *
 * Function 	DI_OSD_GetInOutRect
 * Description	This function get scalable osd plane coordination value.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetInOutRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pInRect, DI_OSD_RECT *pOutRect)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->bOrgExist != TRUE)
	{
		PrintError("ERROR :: Don't Exist Org surface in this layer : hOsdHandle(0x%x), layer(%d), pOsdInstance->bOrgExist(%d)\n",
			hOsdHandle, pOsdInstance->layer, pOsdInstance->bOrgExist);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pInRect == NULL)
	{
		PrintDebug("pInRect is NULL!! \n");
		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		/* In Rectangle value Copy */
		VK_memcpy(pInRect, &pOsdInstance->In_rect, sizeof(DI_OSD_RECT));
	}

	if( pOutRect == NULL)
	{
		PrintDebug("pOutRect is NULL!! \n");
		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		/* Out Rectangle value Copy */
		VK_memcpy(pOutRect, &pOsdInstance->Out_rect, sizeof(DI_OSD_RECT));
	}

	PrintExit();

	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
/*********************************************************************
 *
 * Function 	DI_OSD_SetInOutRect
 * Description	This function  make scalable osd plane coordination.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_SetBackGroundRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pBGRect)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE			nRet = DI_ERR_OK;
	HBOOL				bChangeBGRect = FALSE;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->bOrgExist != TRUE)
	{
		PrintError("ERROR :: Don't Exist Org surface in this layer : hOsdHandle(0x%x), layer(%d), pOsdInstance->bOrgExist(%d)\n",
			hOsdHandle, pOsdInstance->layer, pOsdInstance->bOrgExist);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if ( pOsdInstance->bUseBG != TRUE )
	{
		PrintError("This plane(surface) is not Background plane !!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pBGRect == NULL)
	{
		PrintDebug("pInRect is NULL!! instance In_rect is same~!\n");
		bChangeBGRect = FALSE;
	}
	else
	{
		if ( P_OSD_DiffCheck(pOsdInstance->BG_rect, *pBGRect) )
		{
			bChangeBGRect = TRUE;

			if( P_OSD_CheckRect( pOsdInstance->rect, *pBGRect) != DI_ERR_OK)
			{
				PrintError("Invalid pInRect :: pBGRect(check) is larger than created(base) rect !!\n");
				PrintError("rect(%d,%d,%d,%d), pInRect(%d,%d,%d,%d)\n",
						pOsdInstance->rect.x, pOsdInstance->rect.y, pOsdInstance->rect.width, pOsdInstance->rect.height,
						pBGRect->x, pBGRect->y, pBGRect->width, pBGRect->height);
				return DI_ERR_INVALID_PARAM;
			}

			/* In Rectangle value update~! */
			VK_memcpy(&pOsdInstance->BG_rect, pBGRect, sizeof(DI_OSD_RECT));
		}
		else
		{
			bChangeBGRect = FALSE;
		}
	}

// 처리속도가 문제가 되어, 같은 좌표 call 에 대해 passthrough 해야 한다면, 아래 #if 0 열어서 사용하자.
#if	0
	if (bChangeBGRect == FALSE && pOsdInstance->bOrgDirty == FALSE)
	{
		PrintError("No Changed BackGround Rect Value : No Action return~!\n");
		return DI_ERR_OK;
	}
#endif

	if ( (pOsdInstance->BG_rect.x == 0) && (pOsdInstance->BG_rect.y == 0) &&
		(pOsdInstance->BG_rect.width == 0) && (pOsdInstance->BG_rect.height == 0) )
	{
		pOsdInstance->nBG_Status = DI_OSD_BG_STATUS_NONE;
	}
	else if ( (pOsdInstance->BG_rect.x == pOsdInstance->rect.x) && (pOsdInstance->BG_rect.y == pOsdInstance->rect.y) &&
		(pOsdInstance->BG_rect.width == pOsdInstance->rect.width) && (pOsdInstance->BG_rect.height == pOsdInstance->rect.height) )
	{
		pOsdInstance->nBG_Status = DI_OSD_BG_STATUS_FULL;
	}
	else
	{
		pOsdInstance->nBG_Status = DI_OSD_BG_STATUS_NONE_FULL;
	}

	nRet = P_OSD_MakeDispSurface(hOsdHandle, FALSE, FALSE);
	if (nRet != DI_ERR_OK)
	{
		PrintError("ERROR in P_OSD_MakeDispSurface(), nRet(0x%x)\n", nRet);
	}

	pOsdInstance->bOrgDirty = FALSE;

	PrintExit();

	return nRet;
}

/*********************************************************************
 *
 * Function 	DI_OSD_GetBackGroundRect
 * Description	This function  make scalable osd plane coordination.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetBackGroundRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pBGRect)
{
	DI_OSD_INSTANCE		*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->layer >= DI_OSD_LAYER_MAX || pOsdInstance->status != DI_OSD_SURFACE_ENABLED)
	{
		PrintError("ERROR :: Invalid surface : hOsdHandle(0x%x), layer(%d), status(%d)\n", hOsdHandle, pOsdInstance->layer, pOsdInstance->status);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if( pOsdInstance->bOrgExist != TRUE)
	{
		PrintError("ERROR :: Don't Exist Org surface in this layer : hOsdHandle(0x%x), layer(%d), pOsdInstance->bOrgExist(%d)\n",
			hOsdHandle, pOsdInstance->layer, pOsdInstance->bOrgExist);
		DRV_OSD_PrintInfo();
		return DI_ERR_INVALID_PARAM;
	}

	if ( pOsdInstance->bUseBG != TRUE )
	{
		PrintError("This plane(surface) is not Background plane !!\n");
		return DI_ERR_INVALID_PARAM;
	}

	if( pBGRect == NULL)
	{
		PrintDebug("pInRect is NULL!!\n");
		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		/* Background Rectangle value Copy */
		VK_memcpy(pBGRect, &pOsdInstance->BG_rect, sizeof(DI_OSD_RECT));
	}

	PrintExit();

	return DI_ERR_OK;
}
#endif

/*********************************************************************
 *
 * Function 	DRV_OSD_ResetDisplayFormat
 * Description	This function reset osd (graphic) clip size when display format change.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DRV_Error DRV_OSD_ResetDisplayFormatToHD(void)
{
	NEXUS_Error				rc = NEXUS_SUCCESS;
    NEXUS_GraphicsSettings 	graphicsSettings;

	/* graphicsSettings.position will default to the display size */

	/*
	display format 변경에 대응하기위해 (display  format 변경시 clip 이 display size 보다 크면 error),
	display format 변경 전에 graphic engine disable 하자.
	display format 변경 후에 변경된 display format 으로 clip 재 조정하자. (call by DRV_OSD_SetDisplayFormat()
	*/
	/* NEXUS_Rect clip;
		The area within the surface that should be shown, relative to 0,0.
        If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
        If you specify a cliprect which is smaller, only that portition will be shown
        and it will be scaled to fit the rectangle of size position.width and position.height.
        At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
        The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth.
	*/

	// For HD Display
	NEXUS_Display_GetGraphicsSettings(s_OsdList.HD_DispHandle, &graphicsSettings);

	graphicsSettings.enabled = false;

    rc = NEXUS_Display_SetGraphicsSettings(s_OsdList.HD_DispHandle, &graphicsSettings);
	AssertNEXUS(rc, DRV_ERR);

	return DRV_OK;
}

DRV_Error DRV_OSD_ResetDisplayFormatToSD(void)
{
	NEXUS_Error				rc = NEXUS_SUCCESS;
    NEXUS_GraphicsSettings 	graphicsSettings;

	/* graphicsSettings.position will default to the display size */

	/*
	display format 변경에 대응하기위해 (display  format 변경시 clip 이 display size 보다 크면 error),
	display format 변경 전에 graphic engine disable 하자.
	display format 변경 후에 변경된 display format 으로 clip 재 조정하자. (call by DRV_OSD_SetDisplayFormat()
	*/
	/* NEXUS_Rect clip;
		The area within the surface that should be shown, relative to 0,0.
        If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
        If you specify a cliprect which is smaller, only that portition will be shown
        and it will be scaled to fit the rectangle of size position.width and position.height.
        At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
        The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth.
	*/

	// For SD Display
	if(s_OsdList.SD_DispHandle != NULL)
	{
		NEXUS_Display_GetGraphicsSettings(s_OsdList.SD_DispHandle, &graphicsSettings);
		graphicsSettings.enabled = false;
		rc = NEXUS_Display_SetGraphicsSettings(s_OsdList.SD_DispHandle, &graphicsSettings);
		AssertNEXUS(rc, DRV_ERR);
	}

	return DRV_OK;
}



/*********************************************************************
 *
 * Function 	DRV_OSD_SetDisplayFormat
 * Description	This function set display format to osd (graphic) when display format change.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DRV_Error DRV_OSD_SetDisplayFormatToHD(void)
{
	NEXUS_Error				rc = NEXUS_SUCCESS;
	NEXUS_DisplaySettings 	displaySettings;
    NEXUS_GraphicsSettings 	graphicsSettings;
	DRV_Error				nRet = DRV_OK;
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
#else
	nRet = DRV_VIDEO_GetDisplayHandle(0, &(s_OsdList.HD_DispHandle), &(s_OsdList.SD_DispHandle));
	if (nRet != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetDisplayHandle error!!\n");
		return DRV_ERR;
	}
#endif
	if(s_OsdList.HD_DispHandle != NULL)
	{
		// HD Display
		NEXUS_Display_GetSettings(s_OsdList.HD_DispHandle, &displaySettings);
		switch ( displaySettings.format )
		{
			case NEXUS_VideoFormat_e1080p24hz:
			case NEXUS_VideoFormat_e1080i50hz:
			case NEXUS_VideoFormat_e1080i:
			case NEXUS_VideoFormat_e1080p50hz:
			case NEXUS_VideoFormat_e1080p30hz:
			case NEXUS_VideoFormat_e1080p:
				s_OsdList.HD_ClipRect.width = OSD_WIDTH;	// 1280
				s_OsdList.HD_ClipRect.height = 1080;
				break;
			case NEXUS_VideoFormat_e720p50hz:
			case NEXUS_VideoFormat_e720p:
				s_OsdList.HD_ClipRect.width = OSD_WIDTH;	// 1280
				s_OsdList.HD_ClipRect.height = 720;
				break;
			case NEXUS_VideoFormat_ePal:
			case NEXUS_VideoFormat_ePalI:
			case NEXUS_VideoFormat_e576p:
				s_OsdList.HD_ClipRect.width = OSD_WIDTH/2;	// 640
				s_OsdList.HD_ClipRect.height = 576;
				break;
			case NEXUS_VideoFormat_eNtsc:
			case NEXUS_VideoFormat_eNtsc443:
			case NEXUS_VideoFormat_eNtscJapan:
			case NEXUS_VideoFormat_e480p:
				s_OsdList.HD_ClipRect.width = OSD_WIDTH/2;	// 640
				s_OsdList.HD_ClipRect.height = 480;
				break;
#if defined(CONFIG_HDOUT_2160P)
			case NEXUS_VideoFormat_e3840x2160p24hz:
			case NEXUS_VideoFormat_e3840x2160p25hz:
			case NEXUS_VideoFormat_e3840x2160p30hz:
			case NEXUS_VideoFormat_e3840x2160p50hz:
			case NEXUS_VideoFormat_e3840x2160p60hz:
#if defined(CONFIG_OSD_2160P)
				s_OsdList.HD_ClipRect.width = 3840;
				s_OsdList.HD_ClipRect.height = 2160;
#else
				s_OsdList.HD_ClipRect.width = OSD_WIDTH;	// 1280
				s_OsdList.HD_ClipRect.height = 1080;
#endif
				break;
#endif
			default:
				PrintError("Not supported HD disp format(0x%x)\n", displaySettings.format);
				return DRV_ERR_INVALID_PARAMETER;
		}

		PrintDebug("HD Disp format(0x%x), ClipRect(w, h)=(%d,%d)\n",
				displaySettings.format, s_OsdList.HD_ClipRect.width, s_OsdList.HD_ClipRect.height);

		/* graphicsSettings.position will default to the display size */

		/*
		display format 변경시 마다 HD_Surface 를 다시 만들지 말고,
		HD Surface 는 1080 size 를 감안하여, (1280, 1080) 로 고정하여 생성하고,
		clip 을 조정하여 format 변경에 대처하자.
		*/
		/* NEXUS_Rect clip;
			The area within the surface that should be shown, relative to 0,0.
		If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
		If you specify a cliprect which is smaller, only that portition will be shown
		and it will be scaled to fit the rectangle of size position.width and position.height.
		At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
		The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth.
		*/

		// HD
		NEXUS_Display_GetGraphicsSettings(s_OsdList.HD_DispHandle, &graphicsSettings);

		graphicsSettings.clip.width = s_OsdList.HD_ClipRect.width;
		graphicsSettings.clip.height = s_OsdList.HD_ClipRect.height;

		graphicsSettings.enabled = true;

		graphicsSettings.alpha = s_OsdList.ucPlaneAlpha;	// original alpha 로 되돌려 놓음.

#if defined(CONFIG_OSD_DOUBLEBUFFER)
		graphicsSettings.frameBufferCallback.callback = P_OSD_Flip_HD;
		graphicsSettings.frameBufferCallback.context = NULL;

		b_HD_SurfaceDirty = FALSE;
#endif

	    rc = NEXUS_Display_SetGraphicsSettings(s_OsdList.HD_DispHandle, &graphicsSettings);
		if (rc!=NEXUS_SUCCESS)
		{
			PrintError("Error in NEXUS_Display_SetGraphicsSettings\n");
		}

		rc = NEXUS_Display_SetGraphicsFramebuffer(s_OsdList.HD_DispHandle, s_OsdList.HD_OsdInstance[MAIN_BUF].surface);
		if (rc!=NEXUS_SUCCESS)
		{
			PrintError("Error in NEXUS_Display_SetGraphicsFramebuffer\n");
		}

		// OSD Plane Refresh
		if (s_aOsdInstance[DI_OSD_LAYER_0].status == DI_OSD_SURFACE_ENABLED)
		{
			DRV_OSD_Refresh(DI_OSD_LAYER_0);
		}
	}

	return DRV_OK;
}

DRV_Error DRV_OSD_SetDisplayFormatToSD(void)
{
	NEXUS_Error				rc = NEXUS_SUCCESS;
	NEXUS_DisplaySettings 	displaySettings;
    NEXUS_GraphicsSettings 	graphicsSettings;
	DRV_Error				nRet = DRV_OK;
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
#else
	nRet = DRV_VIDEO_GetDisplayHandle(0, &(s_OsdList.HD_DispHandle), &(s_OsdList.SD_DispHandle));
	if (nRet != DRV_OK)
	{
		PrintError("DRV_VIDEO_GetDisplayHandle error!!\n");
		return DRV_ERR;
	}
#endif
	// SD Display
	if(s_OsdList.SD_DispHandle != NULL)
	{
		NEXUS_Display_GetSettings(s_OsdList.SD_DispHandle, &displaySettings);
		switch ( displaySettings.format )
		{
			case NEXUS_VideoFormat_ePal:
			case NEXUS_VideoFormat_ePalI:
				s_OsdList.SD_ClipRect.width = OSD_WIDTH/2;	// 640
				s_OsdList.SD_ClipRect.height = OSD_SD_HEIGHT;
				break;
			case NEXUS_VideoFormat_eNtsc:
			case NEXUS_VideoFormat_eNtsc443:
			case NEXUS_VideoFormat_eNtscJapan:
				s_OsdList.SD_ClipRect.width = OSD_WIDTH/2;	// 640
				s_OsdList.SD_ClipRect.height = 480;
				break;
			default:
				PrintError("Not supported SD disp format(0x%x)\n", displaySettings.format);
				return DRV_ERR;
		}

		PrintDebug("SD Disp format(0x%x), ClipRect(w, h)=(%d,%d)\n",
				displaySettings.format, s_OsdList.SD_ClipRect.width, s_OsdList.SD_ClipRect.height);

		/* graphicsSettings.position will default to the display size */

		/*
		display format 변경시 마다 HD_Surface 를 다시 만들지 말고,
		HD Surface 는 1080 size 를 감안하여, (1280, 1080) 로 고정하여 생성하고,
		clip 을 조정하여 format 변경에 대처하자.
		*/
		/* NEXUS_Rect clip;
			The area within the surface that should be shown, relative to 0,0.
		If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
		If you specify a cliprect which is smaller, only that portition will be shown
		and it will be scaled to fit the rectangle of size position.width and position.height.
		At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
		The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth.
		*/

		// SD
		NEXUS_Display_GetGraphicsSettings(s_OsdList.SD_DispHandle, &graphicsSettings);

		graphicsSettings.clip.width = s_OsdList.SD_ClipRect.width;
		graphicsSettings.clip.height = s_OsdList.SD_ClipRect.height;

		graphicsSettings.enabled = true;

		graphicsSettings.alpha = s_OsdList.ucPlaneAlpha;	// original alpha 로 되돌려 놓음.

#if defined(CONFIG_OSD_DOUBLEBUFFER)
		graphicsSettings.frameBufferCallback.callback = P_OSD_Flip_SD;
		graphicsSettings.frameBufferCallback.context = NULL;

		b_SD_SurfaceDirty = FALSE;
#endif

		rc = NEXUS_Display_SetGraphicsSettings(s_OsdList.SD_DispHandle, &graphicsSettings);
		AssertNEXUS(rc, DRV_ERR);

		rc = NEXUS_Display_SetGraphicsFramebuffer(s_OsdList.SD_DispHandle, s_OsdList.SD_OsdInstance[MAIN_BUF].surface);
		AssertNEXUS(rc, DRV_ERR);

		// OSD Plane Refresh
		if (s_aOsdInstance[DI_OSD_LAYER_0].status == DI_OSD_SURFACE_ENABLED)
		{
			DRV_OSD_Refresh(DI_OSD_LAYER_0);
		}
	}

	return DRV_OK;
}


DRV_Error DRV_OSD_GetGraphics2DHandle(NEXUS_Graphics2DHandle* pGraphics2DHandle)
{
	PrintEnter();

	if (s_OsdList.nexus_gfx_2DHandle == 0)
	{
		PrintError("s_OsdList.nexus_gfx_2DHandle is NULL~~!!!\n");
		return DRV_ERR;
	}

	*pGraphics2DHandle=s_OsdList.nexus_gfx_2DHandle;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_CopyImage(NEXUS_SurfaceHandle hSrcNexusSurfHandle, NEXUS_SurfaceHandle hDstNexusSurfHandle)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	PrintEnter();

	if( (hSrcNexusSurfHandle == NULL) || (hDstNexusSurfHandle == NULL) )
	{
		PrintError("Surface Handle is NULL~!\n");
		return DRV_ERR;
	}

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
		blitSettings.source.surface = hSrcNexusSurfHandle;
		blitSettings.output.surface = hDstNexusSurfHandle;
		blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
		blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* YCrCb has no alpha, so we must set 0xFF */
		blitSettings.constantColor = 0xFF000000; /* alpha is opaque */

		rc = NEXUS_Graphics2D_Blit( s_OsdList.nexus_gfx_2DHandle, &blitSettings );
		AssertNEXUS(rc, DRV_ERR);

#if 1 /* di 2.0 올리면서 sync 함수 호출 안함 */
		P_OSD_gfxEngineSync();
#endif

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_BlendVisibleSurface(NEXUS_SurfaceHandle hDstNexusSurfHandle)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	PrintEnter();

	if(hDstNexusSurfHandle == NULL)
	{
		PrintError("Surface Handle is NULL~!\n");
		return DRV_ERR;
	}

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{

		NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

		blitSettings.source.surface = s_OsdList.HD_OsdInstance[MAIN_BUF].surface;
		blitSettings.dest.surface = hDstNexusSurfHandle;
		blitSettings.output.surface = hDstNexusSurfHandle;

		blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
	    	blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;

		blitSettings.colorBlend.subtract_e = false;				// +
		blitSettings.colorBlend.e = NEXUS_BlendFactor_eZero;	// 0

		blitSettings.alphaBlend.subtract_e = false;				// +
		blitSettings.alphaBlend.e = NEXUS_BlendFactor_eZero;	// 0

		blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;			// Sc
		blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceAlpha;			// Sa
		blitSettings.colorBlend.subtract_cd = false;						// +
		blitSettings.colorBlend.c = NEXUS_BlendFactor_eDestinationColor;	// Dc
		blitSettings.colorBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;	// (1-Sa)

		blitSettings.alphaBlend.a = NEXUS_BlendFactor_eSourceAlpha;			// Sa
		blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;					// 1
		blitSettings.alphaBlend.subtract_cd = false;						// +
		blitSettings.alphaBlend.c = NEXUS_BlendFactor_eDestinationAlpha;	// Da
		blitSettings.alphaBlend.d = NEXUS_BlendFactor_eInverseSourceAlpha;	// (1-Sa)
		rc = NEXUS_Graphics2D_Blit( s_OsdList.nexus_gfx_2DHandle, &blitSettings );
		AssertNEXUS(rc, DRV_ERR);

#if 1 /* di 2.0 올리면서 sync 함수 호출 안함 */
		P_OSD_gfxEngineSync();
#endif

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_Refresh(DI_OSD_LAYER nOsdLayer)
{
	DI_ERR_CODE			nRet = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		nRet = P_OSD_Flush((DI_OSD_HANDLE)&s_aOsdInstance[nOsdLayer], &s_aOsdInstance[nOsdLayer].rect);
		if (nRet != DI_ERR_OK)
		{
			PrintError("Error(0x%x) In P_OSD_Flush, Layer(%d)\n", nRet, nOsdLayer);
			VK_SEM_Release(s_ulOsdSemId);
			return DRV_ERR;
		}
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		return DRV_ERR;
	}

	VK_SEM_Release(s_ulOsdSemId);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_GetOsdHandle(DI_OSD_LAYER nOsdLayer, DI_OSD_HANDLE *pOsdHandle)
{
	PrintEnter();

	if (s_aOsdInstance[nOsdLayer].surface && s_aOsdInstance[nOsdLayer].status == DI_OSD_SURFACE_ENABLED)
	{
#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
		if (s_aOrgOsdInstance[nOsdLayer].bOrgExist)
		{
			*pOsdHandle = (DI_OSD_HANDLE)&s_aOrgOsdInstance[nOsdLayer];
		}
		else
		{
			*pOsdHandle = (DI_OSD_HANDLE)&s_aOsdInstance[nOsdLayer];
		}
#else
		*pOsdHandle = (DI_OSD_HANDLE)&s_aOsdInstance[nOsdLayer];
#endif
	}
	else
	{
		PrintError("Layer(%d) Created Surface is not existed.\n", nOsdLayer);
		*pOsdHandle = 0;
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_AcquireSemaphore(void)
{
	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
		return DRV_OK;
	else
		return DRV_ERR;
}

DRV_Error DRV_OSD_ReleaseSemaphore(void)
{
	if(VK_SEM_Release(s_ulOsdSemId) == VK_OK)
		return DRV_OK;
	else
		return DRV_ERR;
}


#define		KILOBYTE	(1024)
#define		MEGABYTE	(1024*1024)

void DRV_OSD_PrintInfo(void)
{
	int i;
	int Total_Mem = 0;
	int Total_FB_Mem = 0;
	int ulMB, ulKB;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceMemory mem;

	/* OSD Plane Info */
	PrintStatus("\n\tOSD Infomation\n");

	for(i=0; i<DI_OSD_LAYER_MAX; i++)
		PrintStatus("Layer[%d] State (%s)\n", i, s_OsdList.b_Layer_Show_nHide[i]?"SHOW":"HIDE");

	PrintStatus("Alpha(%d)\n", s_OsdList.ucPlaneAlpha);
	PrintStatus("HD_ClipRect : (%d, %d, %d, %d)\n",
		s_OsdList.HD_ClipRect.x, s_OsdList.HD_ClipRect.y, s_OsdList.HD_ClipRect.width, s_OsdList.HD_ClipRect.height);
	PrintStatus("SD_ClipRect : (%d, %d, %d, %d)\n",
		s_OsdList.SD_ClipRect.x, s_OsdList.SD_ClipRect.y, s_OsdList.SD_ClipRect.width, s_OsdList.SD_ClipRect.height);

	Total_Mem = FRAMEBUF_MAX*((OSD_WIDTH*1080*4) + (OSD_WIDTH/2*OSD_SD_HEIGHT*4));

	ulMB = Total_Mem/MEGABYTE;
	ulKB = (Total_Mem - ulMB*MEGABYTE)/KILOBYTE;

	PrintStatus("FRAMEBUF_MAX(%d):: HD (%d, 1080), SD (%d, %d) \t--> Mem(%d= %d,%03d KB)\n",
		FRAMEBUF_MAX, OSD_WIDTH, OSD_WIDTH/2, OSD_SD_HEIGHT, Total_Mem, ulMB, ulKB);

	/* Created Surface Info */
	PrintStatus("\n\tCreated Surface Info\n");
	for(i=0; i<DI_OSD_SURFACE_MAX_NUM; i++)
	{
		if (s_aOsdInstance[i].status == DI_OSD_SURFACE_ENABLED)
		{
			NEXUS_Surface_GetCreateSettings(s_aOsdInstance[i].surface, &createSettings);
			NEXUS_Surface_GetMemory(s_aOsdInstance[i].surface, &mem);

			ulMB = createSettings.height*mem.pitch/MEGABYTE;
			ulKB = (createSettings.height*mem.pitch - ulMB*MEGABYTE)/KILOBYTE;

			PrintStatus("Layer[%d]:: Surf[%d]=Handle:0x%x, w(%d),h(%d), format(%d) \t--> Mem(%d = %d,%03d KB)\n",
				s_aOsdInstance[i].layer, i, (unsigned int)&s_aOsdInstance[i], createSettings.width, createSettings.height, s_aOsdInstance[i].format,
				createSettings.height*mem.pitch, ulMB, ulKB);

			Total_Mem += createSettings.height*mem.pitch;

			if (i == DI_OSD_LAYER_0)
			{
				NEXUS_Surface_GetCreateSettings(s_aBlendInstance.surface, &createSettings);
				NEXUS_Surface_GetMemory(s_aBlendInstance.surface, &mem);

				ulMB = createSettings.height*mem.pitch/MEGABYTE;
				ulKB = (createSettings.height*mem.pitch - ulMB*MEGABYTE)/KILOBYTE;

				PrintStatus("Layer_X::BlendSurf=Handle:0x%x, w(%d),h(%d), format(%d) \t--> Mem(%d = %d,%03d KB)\n",
					(unsigned int)&s_aBlendInstance, createSettings.width, createSettings.height, s_aBlendInstance.format,
					createSettings.height*mem.pitch, ulMB, ulKB);

				Total_Mem += createSettings.height*mem.pitch;
			}
#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
			if (s_aOrgOsdInstance[i].bOrgExist)
			{
				NEXUS_Surface_GetCreateSettings(s_aOrgOsdInstance[i].surface, &createSettings);
				NEXUS_Surface_GetMemory(s_aOrgOsdInstance[i].surface, &mem);

				ulMB = createSettings.height*mem.pitch/MEGABYTE;
				ulKB = (createSettings.height*mem.pitch - ulMB*MEGABYTE)/KILOBYTE;

				PrintStatus("Layer[%d]:: Org_Surf[%d]=Handle:0x%x, w(%d),h(%d), format(%d) \t--> Mem(%d = %d,%03d KB)\n",
					s_aOrgOsdInstance[i].layer, i, (unsigned int)&s_aOrgOsdInstance[i], createSettings.width, createSettings.height, s_aOrgOsdInstance[i].format,
					createSettings.height*mem.pitch, ulMB, ulKB);

				Total_Mem += createSettings.height*mem.pitch;

#if defined(CONFIG_SCALABLE_OSD_SURFACE)
				PrintStatus("\t In_rect(%d,%d,%d,%d), Out_rect(%d,%d,%d,%d)\n",
					s_aOrgOsdInstance[i].In_rect.x, s_aOrgOsdInstance[i].In_rect.y, s_aOrgOsdInstance[i].In_rect.width, s_aOrgOsdInstance[i].In_rect.height,
					s_aOrgOsdInstance[i].Out_rect.x, s_aOrgOsdInstance[i].Out_rect.y, s_aOrgOsdInstance[i].Out_rect.width, s_aOrgOsdInstance[i].Out_rect.height);
#endif
#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
				if (s_aOrgOsdInstance[i].bUseBG)
				{
					PrintStatus("\t BG_rect(%d,%d,%d,%d), BG_Status:%d (0=None, 1=FULL, 2=Normal)\n",
						s_aOrgOsdInstance[i].BG_rect.x, s_aOrgOsdInstance[i].BG_rect.y, s_aOrgOsdInstance[i].BG_rect.width, s_aOrgOsdInstance[i].BG_rect.height,
						s_aOrgOsdInstance[i].nBG_Status);
				}
#endif
			}
#endif
		}
	}

	ulMB = Total_Mem/MEGABYTE;
	ulKB = (Total_Mem - ulMB*MEGABYTE)/KILOBYTE;
	PrintStatus("Created Surface Total Memory (~ %d= %d,%03d KB)\n", Total_Mem, ulMB, ulKB);

	/* Created Framebuffer Info */
	PrintStatus("\n\tCreated Framebuffer Info\n\r");
	for(i=0; i<DI_OSD_FB_MAX_NUM; i++)
	{
		if (s_aOsdFBInstance[i].status == DI_OSD_FB_ENABLED)
		{
			NEXUS_Surface_GetCreateSettings(s_aOsdFBInstance[i].surface, &createSettings);
			NEXUS_Surface_GetMemory(s_aOsdFBInstance[i].surface, &mem);

			ulMB = createSettings.height*mem.pitch/MEGABYTE;
			ulKB = (createSettings.height*mem.pitch - ulMB*MEGABYTE)/KILOBYTE;

			PrintStatus("FB[%d]=Handle:0x%x, w(%d),h(%d), format(%d) \t--> Mem(%d= %d,%03d KB)\n",
				i, (unsigned int)&s_aOsdFBInstance[i], createSettings.width, createSettings.height, s_aOsdFBInstance[i].format,
				createSettings.height*mem.pitch, ulMB, ulKB);

			Total_FB_Mem += createSettings.height*mem.pitch;
		}
	}

	ulMB = Total_FB_Mem/MEGABYTE;
	ulKB = (Total_FB_Mem - ulMB*MEGABYTE)/KILOBYTE;
	PrintStatus("Created FrameBuffer Total Memory (~ %d= %d,%03d KB)\n\n\r", Total_FB_Mem, ulMB, ulKB);

	Total_Mem += Total_FB_Mem;

	ulMB = Total_Mem/MEGABYTE;
	ulKB = (Total_Mem - ulMB*MEGABYTE)/KILOBYTE;

	PrintStatus("Created Surface + FrameBuffer Total Memory (~ %d= %d,%03d KB)\n\n\r", Total_Mem, ulMB, ulKB);

	return;
}

DRV_Error DRV_OSD_GetFB(int nFB_Num, DI_OSD_HANDLE *pOsdHandle)
{
	PrintEnter();

	if( s_aOsdFBInstance[nFB_Num].status != DI_OSD_FB_ENABLED)
	{
		PrintError("No FrameBuffer resource!! nFB_Num=%d\n", nFB_Num);
		DRV_OSD_PrintInfo();
		return DRV_ERR;
	}

	*pOsdHandle = (DI_OSD_HANDLE)&s_aOsdFBInstance[nFB_Num];

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_SetFilterCoeffs(int lFilterSelect)
{
	PrintEnter();

	switch(lFilterSelect)
	{
		case 0:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_ePointSample;
			PrintStatus("Input is 0 : NEXUS_Graphics2DFilterCoeffs_ePointSample\n");
			break;
		case 1:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eBilinear;
			PrintStatus("Input is 1 : NEXUS_Graphics2DFilterCoeffs_eBilinear\n");
			break;
		case 2:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
			PrintStatus("Input is 2 : NEXUS_Graphics2DFilterCoeffs_eAnisotropic (default setting)\n");
			break;
		case 3:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eSharp;
			PrintStatus("Input is 3 : NEXUS_Graphics2DFilterCoeffs_eSharp\n");
			break;
		case 4:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eSharper;
			PrintStatus("Input is 4 : NEXUS_Graphics2DFilterCoeffs_eSharper\n");
			break;
		case 5:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eBlurry;
			PrintStatus("Input is 5 : NEXUS_Graphics2DFilterCoeffs_eBlurry\n");
			break;
		case 6:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eAntiFlutter;
			PrintStatus("Input is 6 : NEXUS_Graphics2DFilterCoeffs_eAntiFlutter\n");
			break;
		case 7:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eAntiFlutterBlurry;
			PrintStatus("Input is 7 : NEXUS_Graphics2DFilterCoeffs_eAntiFlutterBlurry\n");
			break;
		case 8:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eAntiFlutterSharp;
			PrintStatus("Input is 8 : NEXUS_Graphics2DFilterCoeffs_eAntiFlutterSharp\n");
			break;
		default:
			s_FilterCoeffs = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
			PrintStatus("Input is not available : NEXUS_Graphics2DFilterCoeffs_eAnisotropic (default setting)\n");
			break;
	}

	PrintExit();

	return DRV_OK;
}

void DRV_OSD_Sync(void)
{
	P_OSD_gfxEngineSync();
}

