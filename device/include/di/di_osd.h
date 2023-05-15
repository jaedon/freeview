/* $Header: $ */
/****************************************************************************
* $Workfile:   di_osd.h  $
* $Modtime:   Jul 16 2007 00:17:52  $
*
* Author:
* Description:
*
*                                 Copyright (c) 2008 HUMAX Co., Ltd.
*                                               All rights reserved.
*****************************************************************************/

#ifndef _DI_OSD_H_
#define _DI_OSD_H_

#include "htype.h"
#include "di_err.h"

/****************************************************************************
* Typedef & const data
*****************************************************************************/

typedef enum tagDI_OSD_RASTER_OP {
	DI_OSD_RASTER_OP_CLR,       				///< C(d) = 0
	DI_OSD_RASTER_OP_SRC,       				///< C(d) = C(s)
	DI_OSD_RASTER_OP_DST,       				///< C(d) = C(d)
	DI_OSD_RASTER_OP_SRCOVER,   				///< C(d) = C(s)*A(s) + C(d) * (1 - A(s))
	DI_OSD_RASTER_OP_DSTOVER,   				///< C(d) = C(s) * (1 - A(d)) + C(d)*A(d)
	DI_OSD_RASTER_OP_SRCIN,     				///< C(d) = C(s) * A(d)
	DI_OSD_RASTER_OP_DSTIN,     				///< C(d) = C(d) * A(s)
	DI_OSD_RASTER_OP_SRCOUT,    				///< C(d) = C(s) * (1 - A(d))
	DI_OSD_RASTER_OP_DSTOUT,    				///< C(d) = C(d) * (1 - A(s))
	DI_OSD_RASTER_OP_SRCATOP,   				///< C(d) = C(s) * A(d) + C(d) * (1 - A(s))
	DI_OSD_RASTER_OP_DSTATOP,   				///< C(d) = C(s) * (1 - A(d)) + C(d) * A(s)
	DI_OSD_RASTER_OP_SRCXOR,    				///< C(d) = C(s) * (1 - A(d)) + C(d) * (1 - A(s))
	DI_OSD_RASTER_OP_SRCMULTIPLYALPHA,     		///< C(d) = C(s) * A(Cont.)
	DI_OSD_RASTER_OP_AVERAGE,   				///< A(d) = A(s) + A(d) * (1 - A(s)), C(d) = C(s) * A(s) + C(d) * (1 - A(s))
	DI_OSD_RASTER_OP_SRCOVER_EXT,  				///< C(d) = C(s) + C(d) * (1 - A(s))
	DI_OSD_RASTER_OP_DSTOVER_EXT,   				///< C(d) = C(s) * (1 - A(d)) + C(d)
	DI_OSD_RASTER_OP_CONSTALPHA, 				 ///< A(d) =  A(Cont.)
	DI_OSD_RASTER_OP_SRCOVER_WITHOUT_PREMULTIPLY_ALPHA, 				// Oc = Sc*1 + Dc*(1-Sa), Oa = Sa*1 + Da*(1-Sa)
    /* ***************************************************************** */
    /* Octo 2.0에서 추가된 함수!                                                                       */
    /* ***************************************************************** */
    // TODO :: Octo 2.0에서 추가된 함수!
    DI_OSD_RASTER_OP_ALPHA_COMBINE_SRC,
    DI_OSD_RASTER_OP_CONSTCOLOR_OVER,
    DI_OSD_RASTER_OP_SRC_PREMULTIPLY_ALPHA,  ///< A(d) = A(Cont.), C(d) = C(s) * A(s)

	DI_OSD_RASTER_OP_CONSTCOLOR_MAX
} DI_OSD_RASTER_OP;

typedef enum tagDI_OSD_COLOR_FORMAT{
	DI_OSD_COLOR_FORMAT_RGB,
	DI_OSD_COLOR_FORMAT_YCBCR
} DI_OSD_COLOR_FORMAT;

// System이 OSD Blend 에서 Scaling을 지원하는가를 의미하는 enum
typedef enum tagDI_OSD_BLEND_CAPABILITY{
	DI_OSD_BLEND_NOSCALING,			// OSD Blend를 지원하지 않는다.
	DI_OSD_BLEND_SCALING			// OSD Blend를 지원한다.
} DI_OSD_BLEND_CAPABILITY;

/***************************************************************************/
typedef unsigned long DI_OSD_HANDLE;

typedef struct tagDI_OSD_RECT{
	int		x;
	int		y;
	int		width;
	int		height;
} DI_OSD_RECT;

typedef struct tagDI_OSD_CAPABILITY{
	unsigned char	ucNumLayer;
	unsigned char	ucNumSurface;
	DI_OSD_BLEND_CAPABILITY		eBlendCapable;
} DI_OSD_CAPABILITY;

typedef struct tagDI_OSD_COLOR_RGB{
	unsigned char	ucAlpha;
	unsigned char	ucRed;
	unsigned char	ucGreen;
	unsigned char	ucBlue;
} DI_OSD_COLOR_RGB;

typedef struct tagDI_OSD_COLOR_YCRCB {
	unsigned char	ucAlpha;
	unsigned char	ucY;
	unsigned char	ucCb;
	unsigned char	ucCr;
} DI_OSD_COLOR_YCRCB;

typedef enum tagDI_OSD_LAYER{
	DI_OSD_LAYER_0,
	DI_OSD_LAYER_1,
	DI_OSD_LAYER_2,
	DI_OSD_LAYER_3,
	DI_OSD_LAYER_4,
	DI_OSD_LAYER_5
} DI_OSD_LAYER;

typedef enum tagDI_OSD_PIXEL_FORMAT{
	DI_OSD_PIXEL_FORMAT_ARGB_8888,
	DI_OSD_PIXEL_FORMAT_RGBA_8888,
	DI_OSD_PIXEL_FORMAT_ARGB_4444,
	DI_OSD_PIXEL_FORMAT_RGBA_4444,
	DI_OSD_PIXEL_FORMAT_ARGB_1555,
	DI_OSD_PIXEL_FORMAT_RGB_565,
	DI_OSD_PIXEL_FORMAT_CLUT_8,
	DI_OSD_PIXEL_FORMAT_A_8,
	DI_OSD_PIXEL_FORMAT_YCBCR_444,
	DI_OSD_PIXEL_FORMAT_YCBCR_422

} DI_OSD_PIXEL_FORMAT;

typedef struct tagDI_OSD_PALETTE {
	DI_OSD_COLOR_FORMAT		nColorFormat;
	DI_OSD_COLOR_RGB		*pRGB;
	DI_OSD_COLOR_YCRCB		*pYCrCb;
} DI_OSD_PALETTE;

typedef struct tagDI_OSD_SURFACE_INFO {
	DI_OSD_LAYER		layer;
	DI_OSD_RECT			rect;
	DI_OSD_PIXEL_FORMAT	format;
	unsigned long		ulPlaneByteSize;
} DI_OSD_SURFACE_INFO;



/****************************************************************************
* Global function prototypes
*****************************************************************************/
DI_ERR_CODE	DI_OSD_GetCapability(DI_OSD_CAPABILITY *pOsdCapability);
DI_ERR_CODE	DI_OSD_GetSurfaceInfo(DI_OSD_HANDLE hOsdHandle, DI_OSD_SURFACE_INFO *pInfo);
DI_ERR_CODE	DI_OSD_GetFrameBufferInfo(DI_OSD_HANDLE hOsdHandle, DI_OSD_SURFACE_INFO *pInfo);
DI_ERR_CODE	DI_OSD_CreateSurface(DI_OSD_LAYER nOsdLayer,
 									DI_OSD_RECT rect,
 									DI_OSD_PIXEL_FORMAT nColorFormat,
 									DI_OSD_HANDLE *pOsdHandle);
DI_ERR_CODE	DI_OSD_CreateFrameBuffer( unsigned long width, unsigned long height,
										DI_OSD_PIXEL_FORMAT format,
										DI_OSD_HANDLE *pOsdHandle);
DI_ERR_CODE	DI_OSD_DeleteSurface(DI_OSD_HANDLE hOsdHandle);
DI_ERR_CODE	DI_OSD_DeleteFrameBuffer(DI_OSD_HANDLE hOsdHandle);
DI_ERR_CODE	DI_OSD_Show(DI_OSD_HANDLE hOsdHandle);
DI_ERR_CODE	DI_OSD_Hide(DI_OSD_HANDLE hOsdHandle);
DI_ERR_CODE	DI_OSD_Flush(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pRect);
DI_ERR_CODE	DI_OSD_GetFrameBuffer(DI_OSD_HANDLE hOsdHandle, void **pFrameBuffer);
DI_ERR_CODE	DI_OSD_CreatePalette(DI_OSD_COLOR_FORMAT nColorFmt, DI_OSD_PALETTE **pPaletteHandle);
DI_ERR_CODE	DI_OSD_DeletePalette(DI_OSD_PALETTE *pPaletteHandle);
DI_ERR_CODE	DI_OSD_SetPalette(DI_OSD_HANDLE hOsdHandle, DI_OSD_PALETTE *pPaletteHandle);
DI_ERR_CODE	DI_OSD_ChangeColor(DI_OSD_HANDLE hOsdHandle, unsigned char index, unsigned long color);
DI_ERR_CODE	DI_OSD_CopyOSDBlock(DI_OSD_HANDLE hSrcHandle,
										DI_OSD_RECT srcRect,
										DI_OSD_HANDLE hDstHandle,
										DI_OSD_RECT dstRect);
DI_ERR_CODE	DI_OSD_BlendOSDBlock(DI_OSD_HANDLE hSrcHandle,
								DI_OSD_RECT srcRect,
								DI_OSD_HANDLE hDstHandle,
								DI_OSD_RECT dstRect,
								DI_OSD_RASTER_OP rop);

DI_ERR_CODE	DI_OSD_BlendBlock(DI_OSD_HANDLE hSrcHandle,
								DI_OSD_RECT srcRect,
								DI_OSD_HANDLE hDstHandle,
								DI_OSD_RECT dstRect,
								DI_OSD_RASTER_OP rop);
DI_ERR_CODE	DI_OSD_FillOSDBlock(DI_OSD_HANDLE hOsdHandle,
										DI_OSD_RECT rect,
										unsigned long color);
DI_ERR_CODE	DI_OSD_BlendFillOSDBlock(DI_OSD_HANDLE hOsdHandle,
										DI_OSD_RECT rect,
										unsigned long color);
DI_ERR_CODE	DI_OSD_SetAlphaOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect,
									DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect,
									DI_OSD_RASTER_OP rop, unsigned char ucAlpha);
DI_ERR_CODE DI_OSD_FilterOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect,
                                    DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect,
                                    DI_OSD_RASTER_OP rop, unsigned int ulConstantColor, int nFilter);
DI_ERR_CODE	DI_OSD_SetOutWindow(DI_OSD_LAYER nOsdLayer, DI_OSD_RECT *pRect);

#if defined(USE_SCALABLE_OSD_SURFACE) || defined(USE_BACKGROUND_OSD_SURFACE)
DI_ERR_CODE	DI_OSD_CreateSurfaceWithOrg(DI_OSD_LAYER nOsdLayer,
												DI_OSD_RECT rect,
												DI_OSD_PIXEL_FORMAT format,
												DI_OSD_HANDLE *pOsdHandle,
												HBOOL bUseBGPlane);
DI_ERR_CODE	DI_OSD_DeleteSurfaceWithOrg(DI_OSD_HANDLE hOsdHandle);
#endif
#if defined(USE_SCALABLE_OSD_SURFACE)
DI_ERR_CODE	DI_OSD_SetInOutRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pInRect, DI_OSD_RECT *pOutRect);
DI_ERR_CODE	DI_OSD_GetInOutRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pInRect, DI_OSD_RECT *pOutRect);
#endif
#if defined(USE_BACKGROUND_OSD_SURFACE)
DI_ERR_CODE	DI_OSD_SetBackGroundRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pBGRect);
DI_ERR_CODE	DI_OSD_GetBackGroundRect(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pBGRect);
#endif

void DI_OSD_SetDFB(void*);


/**************************** IDTV ONLY ***********************************/
typedef enum tagDI_OSD_VCOM_PATTERN{
	DI_OSD_VCOM_PATTERN_DEFAULT,
	DI_OSD_VCOM_PATTERN_GREEN,
	DI_OSD_VCOM_PATTERN_GREEN_BLUE,
	DI_OSD_VCOM_PATTERN_MAX
} DI_OSD_VCOM_PATTERN;

DI_ERR_CODE	DI_OSD_CreateVcomSurface(void);
DI_ERR_CODE	DI_OSD_CreateVcomSurfaceExt(DI_OSD_VCOM_PATTERN pattern);
DI_ERR_CODE	DI_OSD_DestroyVcomSurface(void);
DI_ERR_CODE	DI_OSD_GetVcomFrameBuffer(void **pFrameBuffer);
DI_ERR_CODE	DI_OSD_CopyVcomOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect, DI_OSD_RECT dstRect);
/**************************** IDTV ONLY ***********************************/
#endif	// _DI_OSD_H_
