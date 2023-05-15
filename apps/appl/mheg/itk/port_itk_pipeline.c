/**
	@file     port_itk_pipeline.c
	@brief    ITK Host audio Clip 모듈.

	Description: ITK Host Audio Clip Interface.	\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include <directfb.h>
#include <itk.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_pipeline.h"
#include "port_itk_bitmap.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ITK_DISPLAY_VISIBLE_HIDE	0
#define ITK_DISPLAY_VISIBLE_SHOW	1

#define ITK_IGNORE_TILE_BITMAP_THRESHOLD	414720 // (720*576)

/* macro for a safe call to DirectFB functions */
#ifdef CONFIG_DEBUG
#define DFBCHECK(x...) \
{                                                                \
	DFBResult err = x;                                           \
	if (err != DFB_OK) {                                         \
		HxLOG_Error("ERROR : [DirectFB] %s <%d>:\n\t", __FILE__, __LINE__ ); \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); 		\
		DirectFBErrorFatal( #x, err );                           \
	}                                                            \
}
#else
#define DFBCHECK(x)   \
{						\
           DFBResult err  = x;                                                    \
           if (err != DFB_OK) {                                        \
                HxLOG_Error("dfb error."); \
                DirectFBErrorFatal( #x, err );                         \
           }								\
}
#endif

#define	ENTER_CRITICAL_PL		HxSEMT_Get(s_ulPipelineSemId)
#define	LEAVE_CRITICAL_PL		HxSEMT_Release(s_ulPipelineSemId)
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT32		s_ulPipelineSemId;
static itk_rect_t	s_stDisplayPhysicalRect;
static DFBRegion 	s_stFrameBufferRegion;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define ___STATIC_APIS__________________________________________________________
/**
 * Allocate memory for bitmap
 *
 * @author swkim (2011-08-10)
 *
 * @param thiz [in]
 * @param size [in]
 *
 * @return void*
 */
static void *SnT_bitmapAlloc(struct ITK_bitmap_funcs_t *thiz, size_t size)
{
	PARAM_UNUSED(thiz);
	return HLIB_STD_MemAlloc(size);
}

/**
 * Reallocate memory for bitmap
 *
 * @author swkim (2011-08-10)
 *
 * @param thiz [in]
 * @param size [in]
 *
 * @return void*
 */
static void *SnT_bitmapZalloc(struct ITK_bitmap_funcs_t *thiz, size_t size)
{
	void *p = HLIB_STD_MemAlloc(size);
	PARAM_UNUSED(thiz);
	if (NULL != p)
		HxSTD_memset(p, 0, size);
	return p;
}

/**
 * Free memory for bitmap
 *
 * @author swkim (2011-08-10)
 *
 * @param thiz [in]
 * @param p [in]
 */
static void SnT_bitmapFree(struct ITK_bitmap_funcs_t *thiz, void *p)
{
	PARAM_UNUSED(thiz);
	HLIB_STD_MemFree(p);
}

/**
 * Check whether rectangle is empty
 *
 * @author swkim (2011-08-10)
 *
 * @param r [in]
 *
 * @return itk_int32_t
 */
static itk_int32_t mheg_itk_port_pipelineIsEmptyRect(itk_rect_t const * r)
{
	return r->w <= 0 || r->h <= 0;
}

/**
 * Get intersect rectangle both r1 and r2.
 *
 * @author swkim (2011-08-10)
 *
 * @param r1
 * @param r2
 *
 * @return itk_rect_t
 */
static itk_rect_t mheg_itk_port_pipelineIntersectRect(itk_rect_t const * r1, itk_rect_t const * r2)
{
	itk_rect_t result = { 0, 0, 0, 0 };

	if (mheg_itk_port_pipelineIsEmptyRect(r1) || mheg_itk_port_pipelineIsEmptyRect(r2))
	{
		// nothing to do
	}
	else
	{
		itk_point_t const tl =
						{
							MAX(r1->x, r2->x),
							MAX(r1->y, r2->y)
						};

		itk_point_t const br =
						{
							MIN(r1->x + r1->w, r2->x + r2->w),
							MIN(r1->y + r1->h, r2->y + r2->h)
						};
		itk_rect_t const intersected =
						{
							tl.x, tl.y, br.x - tl.x, br.y - tl.y
						};
		if (!mheg_itk_port_pipelineIsEmptyRect(&intersected))
		{
			result = intersected;
		}
	}

	return result;
}

/**
 * Get the number of rows needed to tile the bitmap into the destination.
 *
 * @author swkim (2011-08-10)
 *
 * @param bitmap
 * @param dest
 * @param tile
 * @param hh
 *
 * @return itk_int32_t
 */
static itk_int32_t mheg_itk_port_pipelineGetTileRows(itk_bitmap_t const * bitmap,
		itk_rect_t const * dest,
		itk_tiling_t tile,
		itk_int32_t const hh/*itk_rect_t const * screen*/)
{
	itk_int32_t rows = 1;

	if (tile == ITK_TILING_TILED)
	{
		// Note: fiddle around to cope with two cases: 1) the tiles fit
		// exactly 2) a partial tile is needed at the end of the row.
		// Take pixel resolution into consideration.
		//itk_int32_t const hh = bitmap->height * screen->h;
		//rows = (dest->h * bitmap->pixel_resolution_h + hh - 1) / hh;
		itk_int32_t const partial_h = hh/*(bitmap->height * screen->h)*/ / bitmap->pixel_resolution_h;
		rows = (dest->h + partial_h - 1) / partial_h;
	}
	return rows;
}

/**
 * Get the number of cols needed to tile the bitmap into the destination.
 *
 * @author swkim (2011-08-10)
 *
 * @param bitmap
 * @param dest
 * @param tile
 * @param ww
 *
 * @return itk_int32_t
 */
static itk_int32_t mheg_itk_port_pipelineGetTileCols(itk_bitmap_t const * bitmap, itk_rect_t const * dest,
														itk_tiling_t tile, itk_int32_t const ww/*itk_rect_t const * screen*/)
{
	itk_int32_t cols = 1;

	if (tile == ITK_TILING_TILED)
	{
		// Note: fiddle around to cope with two cases: 1) the tiles fit
		// exactly 2) a partial tile is needed at the end of the column.
		// Take pixel resolution into consideration.
		//itk_int32_t const ww = bitmap->width * screen->w;
		//cols = (dest->w * bitmap->pixel_resolution_w + ww - 1) / ww;
		itk_int32_t const partial_w = ww/*(bitmap->width * screen->w)*/ / bitmap->pixel_resolution_w;
		cols = (dest->w + partial_w - 1) / partial_w;
	}
	return cols;
}

/**
 * Copy Bitmap with param like pos, cliped area
 *
 * @author swkim (2011-08-10)
 *
 * @param pMhegSurface
 * @param bflags
 * @param pduff
 * @param bitmap
 * @param pos
 * @param clipped
 */
static void mheg_itk_port_pipelineCopyBitmapOnce(IDirectFBSurface *pMhegSurface,
															DFBSurfaceBlittingFlags		bflags,
															DFBSurfacePorterDuffRule	pduff,
															itk_bitmap_t const * bitmap,
															itk_rect_t const * pos,
															itk_rect_t const * clipped)
{
	if (!mheg_itk_port_pipelineIsEmptyRect(clipped))
	{
		ITK_BitmapHandle_t *bm = (ITK_BitmapHandle_t *)bitmap;
		itk_point_t src_tl;
		itk_point_t src_br;
		itk_rect_t src;
		DFBRectangle 		srcRect, destRect;
		void				*buf;
		int 				pitch;
	
		src_tl.x = (clipped->x - pos->x) * bitmap->width / pos->w; 
		src_tl.y = (clipped->y - pos->y) * bitmap->height / pos->h;
		src_br.x = (clipped->x + clipped->w - pos->x) * bitmap->width / pos->w;		
		src_br.y = (clipped->y + clipped->h - pos->y) * bitmap->height / pos->h;
				
		// clip region 이 bitmap size 를 넘어서는 경우 clip 영역을 bitmap size 에 맞게 조절.
		if(src_br.x > bitmap->width)
			src_br.x = bitmap->width;
		if(src_br.y > bitmap->height)
			src_br.y = bitmap->height;

		src.x = src_tl.x;
		src.y = src_tl.y;
		src.w = src_br.x - src_tl.x;
		src.h = src_br.y - src_tl.y;
			
		srcRect.x = src.x;
		srcRect.y = src.y;
		srcRect.w = src.w;
		srcRect.h = src.h;

		if (srcRect.w == 0 || srcRect.h == 0)
		{
			HxLOG_Error("ERROR : srcRect width or height is zero, w(%d), h(%d)\n", srcRect.w, srcRect.h);
			return;
		}

		destRect.x = clipped->x;
		destRect.y = clipped->y;
		destRect.w = clipped->w;
		destRect.h = clipped->h;

		HxLOG_Info(" pos(%ld %ld %ld %ld) clipped(%ld %ld %ld %ld), bitmap(%ld, %ld)\n", pos->x, pos->y, pos->w, pos->h, clipped->x, clipped->y, clipped->w, clipped->h, bitmap->width, bitmap->height);
		HxLOG_Info(" srcRect(%d %d %d %d) destRect(%d %d %d %d)\n", srcRect.x, srcRect.y, srcRect.w, srcRect.h, destRect.x, destRect.y, destRect.w, destRect.h);

#if 0
		DFBCHECK(bm->pBitmapSurface->SetClip(bm->pBitmapSurface, &srcRect));
#endif

		DFBCHECK(bm->pBitmapSurface->Unlock(bm->pBitmapSurface));

		DFBCHECK(pMhegSurface->SetBlittingFlags(pMhegSurface, bflags));
		DFBCHECK(pMhegSurface->SetPorterDuff(pMhegSurface, pduff));
		DFBCHECK(pMhegSurface->StretchBlit(pMhegSurface, bm->pBitmapSurface, &srcRect, &destRect));

		DFBCHECK(bm->pBitmapSurface->Lock(bm->pBitmapSurface, (DFBSurfaceLockFlags)(DSLF_READ|DSLF_WRITE), &buf, &pitch));
	}
}

#define	____START_STATIC____
/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline to release.
 */
void mheg_itk_port_pipelineRelease(struct itk_graphics_pipeline_t * thiz)
{
	MHEG_DFBInfo_t	*dfbInfo = NULL;
	ITK_PipelineHandle_t *pipeline = (ITK_PipelineHandle_t *)thiz;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL) 
	{
		HxLOG_Error("dfbInfo is NULL\n");
		return;
	}
	
	if ((dfbInfo->dfb == NULL) || (pipeline == NULL))
	{
		HxLOG_Error("DirectFB or InputParameter is NULL\n");
		return;
	}

	pipeline->ucVisible = ITK_DISPLAY_VISIBLE_HIDE;
	port_itk_display_ReleaseStill();

	HLIB_STD_MemFree(pipeline);

	HxLOG_Info(" framebuffer screen release called.\n");
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline.
 *
 * @return itk_rect_t The physical size in pixels of the graphics display.
 */
itk_rect_t mheg_itk_port_pipelineGetPhysicalSize(struct itk_graphics_pipeline_t * thiz)
{
	MHEG_DFBInfo_t		*dfbInfo = NULL;
	ITK_PipelineHandle_t 	*pipeline = (ITK_PipelineHandle_t*)thiz;
	itk_rect_t rc = {0, 0, ITK_DISPLAY_PHY_WIN_W, ITK_DISPLAY_PHY_WIN_H};
	int w, h;

	dfbInfo = port_itk_display_getDfbInfo();
	if(dfbInfo==NULL)
	{
		HxLOG_Error("dfbInfo is NULL\n");
		return rc;
	}
	
	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL))
	{
		HxLOG_Error("DirectFB or InputParameter is NULL\n");
		return rc;
	}

	DFBCHECK(pipeline->pSurface->GetSize(pipeline->pSurface, &w, &h));

	rc.x = 0;
	rc.y = 0;
	rc.w = w;
	rc.h = h;

//	HxLOG_Info(" Physical size (%ld %ld %ld %ld)\n", rc.x, rc.y, rc.w, rc.h);

	return rc;
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline to draw on.
 * @param clip The clipping region (in physical space). The effect of
 *
 * @return itk_errcode_t ITKE_OK on success, ITKE_FAIL otherwise.
 */
itk_errcode_t mheg_itk_port_pipelineStartDraw(struct itk_graphics_pipeline_t * thiz, itk_rect_t const * clip)
{
	MHEG_DFBInfo_t		*dfbInfo = NULL;
	ITK_PipelineHandle_t 	*pipeline = (ITK_PipelineHandle_t*)thiz;

	ENTER_CRITICAL_PL;

	dfbInfo = port_itk_display_getDfbInfo();
	if(dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is NULL\n");
		return ITKE_FAIL;
	}
	
	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL) || (clip == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid\n");
		return ITKE_FAIL;
	}

	HxLOG_Info("+++ clip rect(%ld %ld %ld %ld)\n", clip->x, clip->y, clip->w, clip->h);

	// rjlee. check point. show/hide
	if (pipeline->ucVisible != ITK_DISPLAY_VISIBLE_SHOW)
	{
		HxLOG_Info(" display PIPELINE is not show state.\n");
		LEAVE_CRITICAL_PL;
		return ITKE_OK;
	}

	if (!mheg_itk_port_pipelineIsEmptyRect(clip))
	{
		s_stFrameBufferRegion.x1 = clip->x;
		s_stFrameBufferRegion.y1 = clip->y;
		s_stFrameBufferRegion.x2 = clip->x + clip->w;
		s_stFrameBufferRegion.y2 = clip->y + clip->h;

		HxSTD_memcpy(&pipeline->stClip, clip, sizeof(struct itk_rect_t));

		DFBCHECK(pipeline->pSurface->SetClip(pipeline->pSurface, &s_stFrameBufferRegion));
	}

	pipeline->bDrawStarted = TRUE;

	return ITKE_OK;
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline.
 */
void mheg_itk_port_pipelineEndDraw(struct itk_graphics_pipeline_t * thiz)
{
	ITK_PipelineHandle_t	*pipeline = (ITK_PipelineHandle_t*)thiz;
	DFBRegion			flipRegion;
	MHEG_DFBInfo_t		*dfbInfo;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		goto ITK_END_DRAW;
	}

	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL))
	{
		HxLOG_Error("DirectFB or input paramter is invalid\n");
		goto ITK_END_DRAW;
	}

	if (pipeline->bDrawStarted)
	{
		HxLOG_Info(" +++ update (%d, %d, %d, %d)\n",
				s_stFrameBufferRegion.x1,s_stFrameBufferRegion.y1, s_stFrameBufferRegion.x2 - s_stFrameBufferRegion.x1, s_stFrameBufferRegion.y2 - s_stFrameBufferRegion.y1);

		// TODO: UI manager로 update 전달해야함.
		/* mheg player에서 Flip 하도록 함 - DFB Slave로 동작하게 하기 위함 */

		flipRegion.x1 = s_stFrameBufferRegion.x1;
		flipRegion.x2 = s_stFrameBufferRegion.x2;
		flipRegion.y1 = s_stFrameBufferRegion.y1;
		flipRegion.y2 = s_stFrameBufferRegion.y2;

		port_itk_display_flipWindow(pipeline->pSurface, &flipRegion, DSFLIP_BLIT);
	}
	else
	{
		HxLOG_Info(" +++ Ignored for draw was not started! (%d, %d, %d, %d)\n",
				s_stFrameBufferRegion.x1,s_stFrameBufferRegion.y1, s_stFrameBufferRegion.x2 - s_stFrameBufferRegion.x1, s_stFrameBufferRegion.y2 - s_stFrameBufferRegion.y1);
	}

ITK_END_DRAW:
	LEAVE_CRITICAL_PL;
	if(pipeline)
	{
		pipeline->bDrawStarted = FALSE;
	}
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline.
 * @param area_to_clear
 */
void mheg_itk_port_pipelineClearArea(struct itk_graphics_pipeline_t * thiz, itk_rect_t const * area_to_clear)
{
	IDirectFBSurface		*pMhegSurface;
	ITK_PipelineHandle_t	*pipeline = (ITK_PipelineHandle_t *)thiz;
	itk_rect_t				dst;
	MHEG_DFBInfo_t			*dfbInfo;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return;
	}

	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL))
	{
		HxLOG_Error("DirectFB or input paramter is invalid\n");
		return;
	}

	dst = mheg_itk_port_pipelineIntersectRect(area_to_clear, &pipeline->stClip);

	HxLOG_Info("++ area( x:%ld, y:%ld, w:%ld, h:%ld -> x:%ld, y:%ld, w:%ld, h:%ld) ---- END!\n", 
		area_to_clear->x, area_to_clear->y, area_to_clear->w, area_to_clear->h, 
		dst.x, dst.y, dst.w, dst.h);

	if (dst.w == 0 || dst.h == 0)
	{
		HxLOG_Error("ERROR : dst width or height is zero, w(%d), h(%d)\n", dst.w, dst.h);
		return;
	}

	pMhegSurface = pipeline->pSurface;
	DFBCHECK(pMhegSurface->SetColor(pMhegSurface, 0, 0, 0, 0));	// transparent
	DFBCHECK(pMhegSurface->SetDrawingFlags(pMhegSurface, DSDRAW_NOFX));
	DFBCHECK(pMhegSurface->SetPorterDuff(pMhegSurface, DSPD_SRC));
	DFBCHECK(pMhegSurface->FillRectangle(pMhegSurface, (int)dst.x, (int)dst.y, (int)dst.w, (int)dst.h));
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline.
 * @param fill_colour
 * @param area_to_fill
 */
void mheg_itk_port_pipelineFillArea(struct itk_graphics_pipeline_t * thiz,
												 itk_colour_t const * fill_colour,
												 itk_rect_t const * area_to_fill)
{
	// The fill_bitmap is a 1x1 bitmap which we colour in and extend to fill
	// the clipped destination.
	IDirectFBSurface		*pMhegSurface;
	ITK_PipelineHandle_t	*pipeline = (ITK_PipelineHandle_t *)thiz;
	int						alpha;
	itk_rect_t 				dst;
	MHEG_DFBInfo_t			*dfbInfo;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return;
	}

	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL) || (fill_colour == NULL) || (area_to_fill == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return;
	}


	dst = mheg_itk_port_pipelineIntersectRect(area_to_fill, &pipeline->stClip);

	pMhegSurface = pipeline->pSurface;
	alpha = 255 - fill_colour->trans;

	HxLOG_Info(" +++ color(r:%d, g:%d, b:%d, a:%d -> a:%d) rect(%ld %ld %ld %ld -> %ld %ld %ld %ld)\n",
				fill_colour->red, fill_colour->green, fill_colour->blue, fill_colour->trans,alpha,
				area_to_fill->x, area_to_fill->y, area_to_fill->w, area_to_fill->h, dst.x, dst.y, dst.w, dst.h);

	if (dst.w == 0 || dst.h == 0)
	{
		HxLOG_Error("ERROR : dst width or height is zero, w(%d), h(%d)\n", dst.w, dst.h);
		return;
	}
	
	DFBCHECK(pMhegSurface->SetColor(pMhegSurface, fill_colour->red, fill_colour->green, fill_colour->blue, alpha));
	DFBCHECK(pMhegSurface->SetDrawingFlags(pMhegSurface, (DFBSurfaceDrawingFlags)(DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY)));
	DFBCHECK(pMhegSurface->SetPorterDuff(pMhegSurface, DSPD_SRC_OVER));
	DFBCHECK(pMhegSurface->FillRectangle(pMhegSurface, (int)dst.x, (int)dst.y, (int)dst.w, (int)dst.h));
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param width The number of pixels in each row of the mask
 * @param height The height of the mask in pixels
 * @param depth  The minimum depth of the mask, bits per pixel, 1 or 8.
 *
 * @return itk_mask_t* The mask, on success, NULL otherwise.
 */
itk_mask_t *mheg_itk_port_pipelineCreateMask(struct itk_graphics_pipeline_t * thiz, itk_int32_t width, itk_int32_t height, itk_int32_t depth)
{
	ITK_MaskHandle_t *pmask = NULL;
	MHEG_DFBInfo_t	*dfbInfo;

	PARAM_UNUSED(thiz);

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return (itk_mask_t*)NULL;
	}

	if ((dfbInfo->dfb == NULL) || (thiz == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is null\n");
		return (itk_mask_t*)NULL;
	}

	//HxLOG_Info(" +++ width(%ld) height(%ld) depth(%ld)\n", width, height, depth);

	if (depth == 8)
	{
		pmask = (ITK_MaskHandle_t *)HLIB_STD_MemAlloc(sizeof(struct ITK_MaskHandle_t));
		if (pmask)
		{
			DFBSurfaceDescription sdsc;
			void *buf;
			int pitch;

			if (width == 0)		width = 1;
			if (height == 0)	height = 1;

			sdsc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_CAPS|DSDESC_WIDTH|DSDESC_HEIGHT|DSDESC_PIXELFORMAT); // TODO : DSHF_FONT
			sdsc.caps = DSCAPS_VIDEOONLY;
			sdsc.width = width;
			sdsc.height = height;
			sdsc.pixelformat = DSPF_A8;	// regardless of requested depth

			DFBCHECK(dfbInfo->dfb->CreateSurface(dfbInfo->dfb, &sdsc, &pmask->pMaskSurface));

			if (pmask->pMaskSurface == NULL)
			{
				HxLOG_Error("pMaskSurface is NULL\n");
				HLIB_STD_MemFree(pmask);
				return (itk_mask_t *)NULL;
			}

			DFBCHECK(pmask->pMaskSurface->Lock(pmask->pMaskSurface, (DFBSurfaceLockFlags)(DSLF_READ|DSLF_WRITE), &buf, &pitch));

			pmask->mask.width = width;
			pmask->mask.height = height;
			pmask->mask.pitch = pitch;
			pmask->mask.depth = depth;
			pmask->mask.mask = (itk_uint8_t *)buf;
		}
	}

	return (itk_mask_t *)pmask;
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param mask
 */
void mheg_itk_port_pipelineReleaseMask(struct itk_graphics_pipeline_t * thiz, itk_mask_t *mask)
{
	ITK_MaskHandle_t * pmask = (ITK_MaskHandle_t *)mask;
	MHEG_DFBInfo_t	*dfbInfo;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return ;
	}

	if ((dfbInfo->dfb == NULL) || (thiz == NULL) || (mask == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return;
	}

	HxLOG_Info(" +++ mask(%p)->width(%ld) .height(%ld) .pitch(%ld) .depth(%ld)\n", mask, mask->width, mask->height, mask->pitch, mask->depth);

	if (pmask)
	{
		DFBCHECK(pmask->pMaskSurface->Unlock(pmask->pMaskSurface));
		DFBCHECK(pmask->pMaskSurface->Release(pmask->pMaskSurface));
		HLIB_STD_MemFree(pmask); pmask = NULL;
	}
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz The pipeline.
 * @param mask The mask.
 * @param pen The pen colour to use for drawing the mask.
 * @param top_left_corner_of_mask
 */
void mheg_itk_port_pipelineFillMask(struct itk_graphics_pipeline_t * thiz, itk_mask_t const * mask, itk_colour_t const * pen, itk_point_t const * top_left_corner_of_mask)
{
	ITK_PipelineHandle_t	*pipeline = (ITK_PipelineHandle_t*)thiz;
	ITK_MaskHandle_t		*pmask = (ITK_MaskHandle_t*)mask;
	int 					alpha;
	itk_rect_t 				dst;
	IDirectFBSurface		*pMhegSurface = NULL;
	MHEG_DFBInfo_t			*dfbInfo;

	void *buf;
	int pitch;

	dfbInfo = port_itk_display_getDfbInfo();
	if(dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo NULL\n");
		return;
	}
	
	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL) || (pmask == NULL) || (pen == NULL) || (top_left_corner_of_mask == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return;
	}

	dst.x = top_left_corner_of_mask->x;
	dst.y = top_left_corner_of_mask->y;
	dst.w = mask->width;
	dst.h = mask->height;
	dst = mheg_itk_port_pipelineIntersectRect(&dst, &pipeline->stClip);

	if (!mheg_itk_port_pipelineIsEmptyRect(&dst))
	{
		DFBRectangle src =
				{
					(int)dst.x - top_left_corner_of_mask->x,
					(int)dst.y - top_left_corner_of_mask->y,
					(int)dst.w,
					(int)dst.h
				};

		pMhegSurface = pipeline->pSurface;

		alpha = 255-pen->trans;

//		HxLOG_Info(" +++ color(r:%d, g:%d, b:%d, a:%d -> r:%d, g:%d, b:%d, a:%d) , rect(%ld %ld %ld %ld -> %ld %ld %ld %ld) \n",
//					pen->red, pen->green, pen->blue, pen->trans,pen->red, pen->green, pen->blue, alpha,
//					top_left_corner_of_mask->x, top_left_corner_of_mask->y, mask->width, mask->height,
//					dst.x, dst.y, dst.w, dst.h);

		if (src.w == 0 || src.h == 0)
		{
			HxLOG_Error("ERROR : src width or height is zero, w(%d), h(%d)\n", dst.w, dst.h);
			return;
		}

		DFBCHECK(pmask->pMaskSurface->Unlock(pmask->pMaskSurface));

		DFBCHECK(pMhegSurface->SetColor(pMhegSurface, pen->red, pen->green, pen->blue, alpha));
		DFBCHECK(pMhegSurface->SetBlittingFlags(pMhegSurface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL|DSBLIT_COLORIZE|DSBLIT_BLEND_COLORALPHA|DSBLIT_SRC_PREMULTIPLY)));
		DFBCHECK(pMhegSurface->SetPorterDuff(pMhegSurface, DSPD_SRC_OVER));
		DFBCHECK(pMhegSurface->Blit(pMhegSurface, pmask->pMaskSurface, &src, dst.x, dst.y));

		DFBCHECK(pmask->pMaskSurface->Lock(pmask->pMaskSurface, (DFBSurfaceLockFlags)(DSLF_READ|DSLF_WRITE), &buf, &pitch));
	}
}

/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param thiz
 * @param w
 * @param h
 * @param format
 *
 * @return itk_bitmap_t*
 */
itk_bitmap_t *mheg_itk_port_pipelineCreateBitmapInstance(struct ITK_bitmap_funcs_t *thiz, itk_uint32_t w, itk_uint32_t h, itk_pixel_format_t format)
{
	ITK_BitmapHandle_t	*bm = NULL;
	void					*buf = NULL;
	int					pitch;
	DFBSurfaceDescription 	sdsc;

	MHEG_DFBInfo_t		*dfbInfo= NULL;
	IDirectFB				*pDfb= NULL;
	IDirectFBSurface		*pSurface= NULL;

	PARAM_UNUSED(thiz);

	dfbInfo = port_itk_display_getDfbInfo();
	if(dfbInfo == NULL)
	{
		HxLOG_Error(" dfbInfo is NULL\n");
		return (itk_bitmap_t *)NULL;
	}
	
	pDfb = dfbInfo->dfb;
	pSurface = dfbInfo->dfbSurface;
	if (pDfb == NULL || pSurface == NULL)
	{
		HxLOG_Error(" DirectFB is NULL\n");
		return (itk_bitmap_t *)NULL;
	}

	if (w <= 0 || h <= 0)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return (itk_bitmap_t *)NULL;
	}

	/** allocate a bitmap **/
	bm = (ITK_BitmapHandle_t *)HLIB_STD_MemAlloc(sizeof(*bm));
	if (!bm)
	{
		HxLOG_Error(" out of memory : bitmap is NULL !\n");
		return (itk_bitmap_t *)NULL;
	}
	
	HxSTD_memset(bm, 0, sizeof(*bm));

	sdsc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_CAPS|DSDESC_WIDTH|DSDESC_HEIGHT|DSDESC_PIXELFORMAT);
	sdsc.caps = (DFBSurfaceCapabilities) (DSCAPS_PREMULTIPLIED | DSCAPS_VIDEOONLY);
	sdsc.width = w;
	sdsc.height = h;
	sdsc.pixelformat = (DFBSurfacePixelFormat)DSPF_ARGB;

	/** create a surface for the bitmap **/
	DFBCHECK(pDfb->CreateSurface(pDfb, &sdsc, &bm->pBitmapSurface));
	if (bm->pBitmapSurface == NULL)
	{
	    HxLOG_Error("pBitmapSurface is NULL\n");
	    goto _itk_display_err;
	}

	//DFBCHECK(bm->pBitmapSurface->SetColor(bm->pBitmapSurface, 0x00, 0x00, 0x00, 0x00));
	//DFBCHECK(bm->pBitmapSurface->FillRectangle (bm->pBitmapSurface, 0, 0, w, h));
	DFBCHECK(bm->pBitmapSurface->Lock(bm->pBitmapSurface, (DFBSurfaceLockFlags)(DSLF_READ|DSLF_WRITE), &buf, &pitch));

	/** pixel dimensions **/
	bm->bitmap.width	= w;
	bm->bitmap.height	= h;

	/** bytes in each row **/
	bm->bitmap.pitch = pitch;

	/** format of pixels **/
	bm->bitmap.format = format;

	/** bitmap pixel data **/
	bm->bitmap.bitmap = (itk_uint8_t *)buf;

	bm->bitmap.pixel_resolution_w = 720;
	bm->bitmap.pixel_resolution_h = 576;

	HxLOG_Info(" +++ bitmap(%p, %p) w/h(%ld, %ld) format(%d) res(%ld, %ld)\n",bm, bm->pBitmapSurface, w, h, format, bm->bitmap.pixel_resolution_w, bm->bitmap.pixel_resolution_h);

	return (itk_bitmap_t *)bm;

_itk_display_err:

	if (bm->pBitmapSurface) 
		DFBCHECK(bm->pBitmapSurface->Release(bm->pBitmapSurface));

	HLIB_STD_MemFree(bm); bm = NULL;

	HxLOG_Error(" Can't Create An bitmap !\n");

	return (itk_bitmap_t *)NULL;
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param width The number of pixels in each row of the bitmap
 * @param height The height of the bitmap in pixels
 *
 * @return itk_bitmap_t* The bitmap, on success, NULL otherwise.
 */
itk_bitmap_t *mheg_itk_port_pipelineCreateBitmap(struct itk_graphics_pipeline_t * thiz,
                       itk_int32_t width,
                       itk_int32_t height)
{
	ITK_PipelineHandle_t	*pipeline = (ITK_PipelineHandle_t *)thiz;
	itk_bitmap_t			*pBitmap = NULL;
	MHEG_DFBInfo_t		*dfbInfo = NULL;

	dfbInfo = port_itk_display_getDfbInfo();
	if(dfbInfo == NULL)
	{
		HxLOG_Error(" dfbInfo is NULL\n");
		return (itk_bitmap_t *)NULL;
	}
	
	if ((dfbInfo->dfb == NULL) || (pipeline == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return (itk_bitmap_t *)NULL;
	}

	pBitmap = mheg_itk_port_pipelineCreateBitmapInstance((struct ITK_bitmap_funcs_t *)thiz, width, height, pipeline->PixelFormat);

	HxLOG_Info(" +++ bitmap(%p) width(%ld) heighlt(%ld)\n", pBitmap, width, height);

	return pBitmap;
}

/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param thiz
 * @param bitmap
 */
void mheg_itk_port_pipelineReleaseBitmapInstance(struct ITK_bitmap_funcs_t *thiz, itk_bitmap_t * bitmap)
{
	ITK_BitmapHandle_t *bm = (ITK_BitmapHandle_t *)bitmap;
	MHEG_DFBInfo_t		*dfbInfo;

	dfbInfo = port_itk_display_getDfbInfo();
	if ((dfbInfo->dfb == NULL) || (bm == NULL) || (bm->pBitmapSurface == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return;
	}

	HxLOG_Info(" +++ bitmap ptr(%p, %p)\n", bm, bm->pBitmapSurface);

	DFBCHECK(bm->pBitmapSurface->Unlock(bm->pBitmapSurface));
	DFBCHECK(bm->pBitmapSurface->Release(bm->pBitmapSurface));

	HLIB_STD_MemFree(bm); bm = NULL;
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param bitmap
 */
void mheg_itk_port_pipelineReleaseBitmap(struct itk_graphics_pipeline_t * thiz, itk_bitmap_t * bitmap)
{
	MHEG_DFBInfo_t	*dfbInfo;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return ;
	}

	if ((dfbInfo->dfb == NULL) || (bitmap == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return;
	}

	HxLOG_Info(" +++ bitmap ptr(%p)\n", bitmap);

	mheg_itk_port_pipelineReleaseBitmapInstance((struct ITK_bitmap_funcs_t *)thiz, bitmap);
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param[in] format The source data encoding format
 * @param[in] data The source data
 * @param[in] dataSize The number of bytes of source data
 *
 * @return itk_bitmap_t* An opaque handle to the picture, NULL if the data could not be used
 */
itk_bitmap_t *mheg_itk_port_pipelineCreateBitmapFromData(struct itk_graphics_pipeline_t * thiz, itk_picture_format_t format,
																	  itk_uint8_t const * data, size_t dataSize)
{
	ITK_PipelineHandle_t	*pipeline = (ITK_PipelineHandle_t *)thiz;
	itk_bitmap_t			*bitmap = NULL;
	ITK_bitmap_funcs_t		funcs;
	MHEG_DFBInfo_t			*dfbInfo = NULL;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return (itk_bitmap_t *)NULL;
	}
	
	if ((dfbInfo->dfb == NULL) || (pipeline == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return (itk_bitmap_t *)NULL;
	}

	funcs.alloc			= SnT_bitmapAlloc;
	funcs.zalloc		= SnT_bitmapZalloc;
	funcs.free			= SnT_bitmapFree;
	funcs.createBitmap	= mheg_itk_port_pipelineCreateBitmapInstance;
	funcs.freeBitmap	= mheg_itk_port_pipelineReleaseBitmapInstance;
	bitmap				= port_itk_bitmap_getPictureData(data, dataSize, format, pipeline->PixelFormat, &funcs);

	HxLOG_Info(" +++ bitmap(%p) format(%d) data(%p) dataSize(%d)\n", bitmap, format, data, dataSize);

	return bitmap;
}

/**
 * refer to itk_display.h - itk_graphics_pipeline_t
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param bitmap The bitmap to copy. The bitmap will have been created by
 * @param destination The physical region in which to display the bitmap.
 * @param tile Whether or not to tile the bitmap
 */
void mheg_itk_port_pipelineCopyBitmap(struct itk_graphics_pipeline_t * thiz, itk_bitmap_t const * bitmap, itk_rect_t const * destination, itk_tiling_t tile)
{
	itk_rect_t clip_dest;
	itk_int32_t hh, ww, rows, cols, row, col;

	ITK_PipelineHandle_t 		*pipeline = (ITK_PipelineHandle_t *)thiz;
	DFBSurfaceBlittingFlags		bflags = DSBLIT_BLEND_ALPHACHANNEL;
	DFBSurfacePorterDuffRule	pduff = DSPD_SRC_OVER;
	IDirectFBSurface			*pMhegSurface = NULL;
	MHEG_DFBInfo_t			*dfbInfo = NULL;

	dfbInfo = port_itk_display_getDfbInfo();
	if (dfbInfo == NULL)
	{
		HxLOG_Error("dfbInfo is Null\n");
		return ;
	}

	if ((dfbInfo->dfb == NULL) || (pipeline == NULL) || (pipeline->pSurface == NULL) || (bitmap == NULL))
	{
		HxLOG_Error("DirectFB or input parameter is invalid NULL\n");
		return ;
	}

	/*
		destination -> source bitmap영역중 copyBitmap으로 넘겨주는 rectangle
		clip_dest -> startDraw에서 지정된 rectangle과 destination 좌표를 고려해서 실제 좌표계에 그려질 clipping 영역
	*/
	clip_dest = mheg_itk_port_pipelineIntersectRect(&pipeline->stClip, destination);
	
	hh = bitmap->height * s_stDisplayPhysicalRect.h;
	ww = bitmap->width * s_stDisplayPhysicalRect.w;
	rows = mheg_itk_port_pipelineGetTileRows(bitmap, destination, tile, hh);
	cols = mheg_itk_port_pipelineGetTileCols(bitmap, destination, tile, ww);

	HxLOG_Info("\t++++ START! rows/col(%ld, %ld), tile(%d), ww/hh(%ld, %ld), res(%ld, %ld), dest(%ld, %ld, %ld, %ld) \n", rows, cols, tile, ww, hh,
								bitmap->pixel_resolution_w, bitmap->pixel_resolution_h, destination->x, destination->y, destination->w, destination->h);

	HxLOG_Info("clip(%d, %d, %d, %d) clip_dest(%d, %d, %d, %d)\n", 
		pipeline->stClip.x, pipeline->stClip.y, pipeline->stClip.w, pipeline->stClip.h, 
		clip_dest.x, clip_dest.y, clip_dest.w, clip_dest.h);

	/* HD021 issue를 위한 work-around */
	if ((bitmap->height == 1) && (bitmap->width == 1) && (tile == ITK_TILING_TILED) &&
		(bitmap->pixel_resolution_h * bitmap->pixel_resolution_w >= ITK_IGNORE_TILE_BITMAP_THRESHOLD) &&
		((destination->w - destination->x) * (destination->h - destination->y) >= ITK_IGNORE_TILE_BITMAP_THRESHOLD))
	{
		HxLOG_Error("input parameter is invalid \n");
		return;
	}

	pMhegSurface = pipeline->pSurface;

	for (row = 0; row != rows; ++row)
	{
		for (col = 0; col != cols; ++col)
		{			
			itk_point_t const tl =
			{
				destination->x + (itk_int32_t)((double)(col * ww) / (double)bitmap->pixel_resolution_w + 0.5),
				destination->y + (itk_int32_t)((double)(row * hh) / (double)bitmap->pixel_resolution_h+ 0.5)
			};


			/*
				pos -> tile을 고려한 destination rectangle
				revised -> pos rectangle과 clip되어야할 rectangle사이의 clipping영역
			*/

			/*
				DTG HD024, HD026 pass 를 위한 w/a 코드
			*/
			
#if 1
			if(((ww/bitmap->pixel_resolution_w) == clip_dest.w)&&((hh/ bitmap->pixel_resolution_h) ==clip_dest.h))
			{
				itk_rect_t const pos = {
					tl.x,
					tl.y,
					(ww)/bitmap->pixel_resolution_w,
					(hh)/ bitmap->pixel_resolution_h
				};
				
				itk_rect_t const clipped = mheg_itk_port_pipelineIntersectRect(&pos, &clip_dest);

				HxLOG_Info("pos(%d, %d, %d, %d),  clip_dest(%d, %d, %d, %d)\n", 
					pos.x, pos.y, pos.w, pos.h,
					clip_dest.x, clip_dest.y, clip_dest.w, clip_dest.h);

				mheg_itk_port_pipelineCopyBitmapOnce(pMhegSurface, bflags, pduff, bitmap, &pos, &clipped);
			}
			else
			{
				itk_rect_t const pos = {
					tl.x,
					tl.y,
					(ww + bitmap->pixel_resolution_w/2)/bitmap->pixel_resolution_w,
					(hh + bitmap->pixel_resolution_h/2)/ bitmap->pixel_resolution_h
				};
				
				itk_rect_t const clipped = mheg_itk_port_pipelineIntersectRect(&pos, &clip_dest);

				HxLOG_Info("pos(%d, %d, %d, %d),  clip_dest(%d, %d, %d, %d)\n", 
					pos.x, pos.y, pos.w, pos.h,
					clip_dest.x, clip_dest.y, clip_dest.w, clip_dest.h);

				mheg_itk_port_pipelineCopyBitmapOnce(pMhegSurface, bflags, pduff, bitmap, &pos, &clipped);
			}
#else
			itk_point_t const br =
			{
				destination->x + (itk_int32_t)((double)((col + 1) * ww) / (double)bitmap->pixel_resolution_w + 0.5),
				destination->y + (itk_int32_t)((double)((row + 1) * hh) / (double)bitmap->pixel_resolution_h + 0.5)
			};

			itk_rect_t const pos = { tl.x, tl.y, br.x - tl.x, br.y - tl.y };
			itk_rect_t const clipped = mheg_itk_port_pipelineIntersectRect(&pos, &clip_dest);
			mheg_itk_port_pipelineCopyBitmapOnce(pMhegSurface, bflags, pduff, bitmap, &pos, &clipped);
			HxLOG_Info("pos(%d, %d, %d, %d),  clip_dest(%d, %d, %d, %d)\n", 
				pos.x, pos.y, pos.w, pos.h,
				clip_dest.x, clip_dest.y, clip_dest.w, clip_dest.h);

#endif
		}
	}
}



#define ___PUBLIC_APIS__________________________________________________________
void	port_itk_pipeline_init(void)
{
	HxSEMT_Create(&s_ulPipelineSemId,   "sHostGPL", HxSEMT_PRIORITY);

	s_stDisplayPhysicalRect.x = 0;
	s_stDisplayPhysicalRect.y = 0;
	s_stDisplayPhysicalRect.w = ITK_DISPLAY_PHY_WIN_W;
	s_stDisplayPhysicalRect.h = ITK_DISPLAY_PHY_WIN_H;

	s_stFrameBufferRegion.x1 = 0; 
	s_stFrameBufferRegion.y1 = 0;
	s_stFrameBufferRegion.x2 = ITK_DISPLAY_PHY_WIN_W;
	s_stFrameBufferRegion.y2 = ITK_DISPLAY_PHY_WIN_H;
}

/**
 * Gets a graphics pipeline from the display.
 *
 * @param[in] thiz The display
 *
 * @return A handle to the pipeline on success, NULL otherwise.
 *
 * Note that if the display cannot provide a graphics pipeline, clients
 * will ask instead for a display framebuffer using
 * struct itk_display_t::getFrameBuffer(). The pipeline is preferred since it can
 * take advantage of hardware capabilities.
 */
itk_graphics_pipeline_t *port_itk_pipeline_Get(struct itk_display_t *thiz)
{
	ITK_PipelineHandle_t	*pipeline = NULL;
	MHEG_DFBInfo_t			*dfbInfo;

	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return (itk_graphics_pipeline_t *)NULL;
	}

	pipeline = (ITK_PipelineHandle_t *)HLIB_STD_MemAlloc(sizeof(ITK_PipelineHandle_t));

	/**
	 * failed to allocate pipeline.
	 */
	if (pipeline == NULL)
	{
		HxLOG_Error(" pipeline is NULL\n");
		return (itk_graphics_pipeline_t *)NULL;
	}
	else
	{
		HxSTD_memset(pipeline, 0, sizeof(ITK_PipelineHandle_t));
	}

	dfbInfo = port_itk_display_getDfbInfo();
	/**
	 * 1. link av stream handling functions.
	 */
	pipeline->vtbl.release 				= mheg_itk_port_pipelineRelease;
	pipeline->vtbl.getPhysicalSize		= mheg_itk_port_pipelineGetPhysicalSize;
	pipeline->vtbl.startDraw 			= mheg_itk_port_pipelineStartDraw;
	pipeline->vtbl.endDraw				= mheg_itk_port_pipelineEndDraw;
	pipeline->vtbl.clearArea			= mheg_itk_port_pipelineClearArea;
	pipeline->vtbl.fillArea				= mheg_itk_port_pipelineFillArea;
	pipeline->vtbl.createMask			= mheg_itk_port_pipelineCreateMask;
	pipeline->vtbl.releaseMask			= mheg_itk_port_pipelineReleaseMask;
	pipeline->vtbl.fillMask				= mheg_itk_port_pipelineFillMask;
	pipeline->vtbl.createBitmap			= mheg_itk_port_pipelineCreateBitmap;
	pipeline->vtbl.createBitmapFromData	= mheg_itk_port_pipelineCreateBitmapFromData;
	pipeline->vtbl.releaseBitmap		= mheg_itk_port_pipelineReleaseBitmap;
	pipeline->vtbl.copyBitmap			= mheg_itk_port_pipelineCopyBitmap;

	pipeline->stClip.x = 0;
	pipeline->stClip.y = 0;
	pipeline->stClip.w = ITK_DISPLAY_OSD_HD_W;
	pipeline->stClip.h = ITK_DISPLAY_OSD_HD_H;

	pipeline->pSurface = dfbInfo->dfbSurface;
	pipeline->ucVisible = ITK_DISPLAY_VISIBLE_SHOW;
	pipeline->PixelFormat 	= ITK_FB_ARGB8888;

	// pipeline->ulPitch	= ?? // line 당 실제 byte 수

	HxLOG_Info(" pipeline created.(%p)\n", pipeline);

	return (itk_graphics_pipeline_t *)pipeline;
}


