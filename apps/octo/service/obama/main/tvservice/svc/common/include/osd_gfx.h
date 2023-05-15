
/****************************************************************************
*
* File Name	: mw_osd.h
*
* Description	:
*
*
 * Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2008/11/05	File Created.				JK Baek
* 2009/03/12	File reCreated.				wjmoh
*
****************************************************************************/

#ifndef	__OSD_GFX_H__
#define	__OSD_GFX_H__

#include <osd.h>
//차후 삭제 또는 변경한다. by dkim2
//#include <util_png.h>

#if defined(CONFIG_MWC_OSD_16BIT_TRUECOLOR)

/* default 16 bit(ARGB4444) color macroes */

#define ALPHA(X)					(((X) & 0xf000) >> 12)
#define RED(X)						(((X) & 0x0f00) >> 8)
#define GREEN(X)					(((X) & 0x00f0) >> 4)
#define BLUE(X)						(((X) & 0x000f))

#define SCREEN_COLOR_FROM_ARGB32(pscr, a, r, g, b) 	OSD_GFX_GetColorFromARGB32(pscr, a, r, g, b)
#define SCREEN_COLOR_FROM_RGB32(pscr, r, g, b) 		OSD_GFX_GetColorFromRGB32(pscr, r, g, b)

#define GET_ARGB(a, r, g, b) 	((((a) & 0xf) << 12) | (((r) & 0xf) << 8) | (((g) & 0xf) << 4) | ((b) & 0xf))
#define GET_RGB(r, g, b) 		(0xf000 | (((r) & 0xf) << 8) | (((g) & 0xf) << 4) | ((b) & 0xf))

#define OSD_COL_MAX_VALUE			0xFFFF
#define OSD_COL_ELEMENT_MAXVALUE	0xF
#define OSD_COL_ELEMENT_HALFVALUE	0x4


#else /* CONFIG_MWC_OSD_16BIT_TRUECOLOR */


#define ALPHA(X)					((X & 0xff000000)>>24)
#define RED(X)						((X & 0x00ff0000) >> 16)
#define GREEN(X)					((X & 0x0000ff00) >> 8)
#define BLUE(X)						((X & 0x000000ff))

#define GET_ARGB(a, r, g, b) 		((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define GET_RGB(r, g, b) 			(0xff000000 | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))

#define OSD_COL_MAX_VALUE			0xFFFFFFFF
#define OSD_COL_ELEMENT_MAXVALUE	0xFF
#define OSD_COL_ELEMENT_HALFVALUE	0xF

#endif /* CONFIG_MWC_OSD_16BIT_TRUECOLOR */

#ifdef CONFIG_MWC_OSD_16BIT_TRUECOLOR
#define DEFAULT_SCREEN_PIXEL_FORMAT 	(ePAL_PixelFormat_ARGB4444)
#define TRUECOLOR_BPP			16
#else
#define DEFAULT_SCREEN_PIXEL_FORMAT 	(ePAL_PixelFormat_ARGB8888)
#define	TRUECOLOR_BPP			32
#endif

#define ALPHA32(X)					(((X & 0xff000000)	>> 24)	& 0xff)
#define RED32(X)					(((X & 0x00ff0000) 	>> 16)	& 0xff)
#define GREEN32(X)					(((X & 0x0000ff00) 	>> 8 )	& 0xff)
#define BLUE32(X)					(((X & 0x000000ff)	)		& 0xff)

#define ALPHA16(X)					(((X & 0xf000)	>> 12)	& 0xf)
#define RED16(X)					(((X & 0x0f00) 	>> 8)	& 0xf)
#define GREEN16(X)					(((X & 0x00f0) 	>> 4)	& 0xf)
#define BLUE16(X)					(((X & 0x000f)	)		& 0xf)

#define GET_ARGB32(a, r, g, b) 		((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define GET_ARGB16(a, r, g, b) 		((((a) & 0xf) << 12) | (((r) & 0xf) << 8) | (((g) & 0xf) << 4) | ((b) & 0xf))


//#define DIV_COLOR(x)				((x + (((x + OSD_COL_ELEMENT_HALFVALUE+1) >> OSD_COL_ELEMENT_HALFVALUE) + (OSD_COL_ELEMENT_HALFVALUE+1))) >> OSD_COL_ELEMENT_HALFVALUE)


#define	MIN_COORDINATE(a,b)	((a)<(b)?(a):(b))
#define	MAX_COORDINATE(a,b)	((a)>(b)?(a):(b))

#define	COLOR_SYS_TRANSPARENT(psr)	OSD_GFX_GetColorFromOSDColor32(psr,0x00000000)
#define	COLOR_SYS_BLACK(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFF000000)
#define	COLOR_SYS_WHITE(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFFFFFFFF)
#define COLOR_SYS_WHITE2(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFFDCDCDC)		// LCD용 white 너무 하얗게 할 수 없는 부분이 있다.
#define	COLOR_SYS_RED(psr)			OSD_GFX_GetColorFromOSDColor32(psr,0xFFFF0000)
#define	COLOR_SYS_BLUE(psr)			OSD_GFX_GetColorFromOSDColor32(psr,0xFF0000FF)
#define	COLOR_SYS_DARKBLUE(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFF3434FF)
#define	COLOR_SYS_GREEN(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFF00FF00)
#define	COLOR_SYS_YELLOW(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFFFFFF00)
#define	COLOR_SYS_GRAY(psr)			OSD_GFX_GetColorFromOSDColor32(psr,0xFF808080)
#define	COLOR_SYS_DARKGRAY(psr)		OSD_GFX_GetColorFromOSDColor32(psr,0xFF171717)

/* ========================================================= */
/*		Global Variables .. 								 */
/* ========================================================= */
//extern	GWM_SystemColor_t	gSystemColor;

/** @brief application OSD */
extern	OSD_Screen_t		g_osdScreen;

/** @brief screen presenting subtitle/TTX */
extern	OSD_Screen_t		g_dataScreen;

/** @brief screen presenting data broadcasting */
extern	OSD_Screen_t		g_auxScreen;


#ifdef CONFIG_SW_MULTIPLANE
/** @brief multi-purposal screen */
extern	OSD_Screen_t		g_SWMergeScreen;
#endif


/* ========================================================= */
/*		Functions .. 										 */
/* ========================================================= */

extern OSD_Screen_t		*OSD_GFX_GetOsdScreen(void);
extern OSD_Screen_t		*OSD_GFX_GetDataScreen(void);
extern OSD_Screen_t		*OSD_GFX_GetAuxScreen(void);

extern OSD_Result_t		OSD_GFX_GetScreenSize(const OSD_Screen_t *pBaseScreen, OSD_Size_t *pScrSize);
extern OSD_Result_t		OSD_GFX_GetClipSize(const OSD_Screen_t *pBaseScreen, OSD_Size_t *pScrSize);
extern OSD_Result_t		OSD_GFX_GetBitPerPixel (OSD_Screen_t *pScr, HUINT32 *pulBpp);

/* primitives*/
extern OSD_Result_t		OSD_GFX_SetPoint(OSD_Point_t *pPoint, HINT32 x, HINT32 y);
extern OSD_Result_t		OSD_GFX_SetSize(OSD_Size_t *pSize, HINT32 width, HINT32 height);
extern OSD_Result_t		OSD_GFX_CopySize(OSD_Size_t *pDstSize, const OSD_Size_t *pSrcSize);
extern OSD_Result_t		OSD_GFX_CheckAreaEmpty(const OSD_Area_t *pArea);
extern HBOOL			OSD_GFX_IsAreaEffective(const OSD_Area_t *pArea);
extern OSD_Result_t		OSD_GFX_SetAreaByPointSize(OSD_Area_t *pArea, const OSD_Point_t *pPoint, const OSD_Size_t *pSize);
extern OSD_Result_t		OSD_GFX_SetRectByPointSize(OSD_Rect_t *pRect, const OSD_Point_t *pPoint, const OSD_Size_t *pSize);
extern OSD_Result_t		OSD_GFX_SetAreaByRect(OSD_Area_t *pArea, const OSD_Rect_t *pRect);
extern OSD_Result_t		OSD_GFX_SetRectByArea(OSD_Rect_t *pRect, const OSD_Area_t *pArea);
extern OSD_Result_t		OSD_GFX_CopyArea(OSD_Area_t *pDestArea, const OSD_Area_t *pSrcArea);
extern OSD_Result_t		OSD_GFX_MergeArea(const OSD_Area_t *pArea1, const OSD_Area_t *pArea2, OSD_Area_t *pDestArea);
extern HBOOL			OSD_GFX_IsAreaOverRapped(const OSD_Area_t *pArea1, const OSD_Area_t *pArea2);
extern OSD_Result_t		OSD_GFX_IntersectArea(const OSD_Area_t *pArea1, const OSD_Area_t *pArea2, OSD_Area_t *pIntersected_area);
extern OSD_Result_t		OSD_GFX_SetArea(OSD_Area_t *pArea, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2);
extern OSD_Result_t		OSD_GFX_SetRect(OSD_Rect_t *pRect, HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern OSD_Result_t		OSD_GFX_CheckRectEmpty(const OSD_Rect_t *pRect);
extern HBOOL			OSD_GFX_IsRectEffective(OSD_Rect_t *pRect);
extern OSD_Result_t		OSD_GFX_CopyRect(OSD_Rect_t *pDest, OSD_Rect_t *pSrc);
extern OSD_Result_t		OSD_GFX_OffsetRect(OSD_Rect_t *pRect, HINT32 xOffset, HINT32 yOffset);
extern OSD_Result_t		OSD_GFX_InflateRect(OSD_Rect_t *rc, HINT32 w, HINT32 h);
extern OSD_Result_t		OSD_GFX_DeflateRect(OSD_Rect_t *rc, HINT32 w, HINT32 h);

/**
  @remark	Initializes application OSDs. \n
  			Basically the application OSD consists of some sort of screen, \n
  			so it means this initializes a final composited OSD plane.	\n
  @remark	API Type : sync \n
  @return		ERR_OK \n
                     ERR_FAIL : error on initialization\n
*/
extern	OSD_Result_t			OSD_GFX_InitializeOsd(void);


/**
  @remark	TBD: de-initializes OSDs. \n
  @remark	API Type : sync \n
  @return		OK or FAIL \n
*/
extern	OSD_Result_t			OSD_GFX_DestructOsd(void);
extern	OSD_Result_t	OSD_GFX_MakeScreen(HINT32 nScreenIdx, OSD_Screen_t *scr, HINT32 width
										, HINT32 height, HUINT32 uiBpp, HBOOL bUseBG);

/* UI bitmap image */
extern OSD_Result_t			OSD_GFX_MakeBitmap(HUINT8 *pDumpedImg, HUINT32 *phBuffer, HUINT32 imgWidth, HUINT32 imgHeight);

/**
  @remark	set the screen active. \n
  @remark	API Type : sync \n
  @return		OK or FAIL \n
*/
extern	OSD_Result_t			OSD_GFX_SetActiveScreen(OSD_Screen_t *scr);

/**
  @remark	update specified region of the screen. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_UpdateScreen(OSD_Screen_t *scr, OSD_Area_t *area);


/**
  @remark	set clipping region of the screen. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_SetClip(OSD_Screen_t *scr, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2);


/**
  @remark	get the clipped region of the screen. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_GetClip(OSD_Screen_t *scr, HINT32 *x1, HINT32 *y1, HINT32 *x2, HINT32 *y2);


/**
  @remark	check if the specified region is clipped or not. \n
  @remark	API Type : sync \n
  @return		>0 if that clipped \n
*/
extern	HINT32			OSD_GFX_IsClippedArea(OSD_Screen_t *scr, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2);


/**
  @remark	erase entire region of the screen. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_ClearScreen(OSD_Screen_t *pixmap, OSD_Rect_t *screenRc);


/**
  @remark	erase specified area of the screen. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern 	void 			OSD_GFX_ClearScreenByArea(OSD_Screen_t *pixmap, OSD_Area_t *area);


/**
  @remark	change color of given position. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_PutPixel(OSD_Screen_t *pixmap, HINT32 x, HINT32 y, OSD_Color_t color);


/**
  @remark	change color, considers clipped or not. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_PutPixelClip(OSD_Screen_t  *pixmap, HINT32 x, HINT32 y, OSD_Color_t color);


/**
  @remark	draw horizontal line. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawHLineNoClip(OSD_Screen_t  *pixmap, HINT32 x, HINT32 y, HINT32 w, OSD_Color_t color);


/**
  @remark	draw horizontal line, considers clipping region. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawHLineClip(OSD_Screen_t  *pixmap, HINT32 x, HINT32 y, HINT32 w, OSD_Color_t color);


/**
  @remark	draw vertical line. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawVLineNoClip(OSD_Screen_t  *pixmap, HINT32 x, HINT32 y, HINT32 h, OSD_Color_t color);


/**
  @remark	draw vertical line, considers clipping region. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawVLineClip(OSD_Screen_t  *pixmap, HINT32 x, HINT32 y, HINT32 h, OSD_Color_t color);


/**
  @remark	draw a rectangle frame. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawRect(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, OSD_Color_t color);


/**
  @remark	draw and fill a rectangle. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawRectFill(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, OSD_Color_t color);

/**
  @remark	draw a line. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void 			OSD_GFX_DrawLine(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, OSD_Color_t color);


/**
  @remark	draw a circle frame. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawCircle(OSD_Screen_t  *pixmap, HINT32 x0, HINT32 y0, HINT32 radius, OSD_Color_t color);


/**
  @remark	draw and fill a circle \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawCircleFill(OSD_Screen_t  *pixmap, HINT32 x0, HINT32 y0, HINT32 radius, OSD_Color_t color);


/**
  @remark	draw a round corner arc. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawRoundCorner(OSD_Screen_t  *pixmap, HINT32 x0, HINT32 y0, HINT32 radius, HINT32 corner, OSD_Color_t color);


/**
  @remark	draw and fill a round corner arc. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawRoundCornerFill(OSD_Screen_t  *pixmap, HINT32 x0, HINT32 y0, HINT32 radius, HINT32 corner, OSD_Color_t color);


/**
  @remark	draw a round corner rectangle frame. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawRoundRect(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, HINT32 r, OSD_Color_t color);


/**
  @remark	draw and fill a round corner rectangle. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawRoundRectFill(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, HINT32 r, OSD_Color_t color);


/**
  @remark	draw and fill a triangle. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_DrawTriangleFill(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, HINT32 x3, HINT32 y3, OSD_Color_t color);


/**
  @remark	enlarge the source screen to the destination with arithmatic scale. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_StretchArea(OSD_Screen_t  *source, OSD_Screen_t  *destination, HINT32 sx1, HINT32 sy1, HINT32 sx2, HINT32 sy2, HINT32 dx1, HINT32 dy1, HINT32 dx2, HINT32 dy2);


/**
  @remark	transfer bit blocks of the source to the destination. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void				OSD_GFX_BlitScreen(OSD_Screen_t *source, OSD_Screen_t *destination, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HINT32 dx, HINT32 dy);


/**
  @remark	copy or blned bit blocks of the source to the destination. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern 	void				OSD_GFX_Blit(OSD_Screen_t *source, OSD_Rect_t *srcRect,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect,
									OSD_BlitMode_t mode);

/**
  @remark	replace source alpha with given alpha and blit \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_BlitAlpha(OSD_Screen_t *source, OSD_Rect_t *srcRect,
									 OSD_Screen_t *destination, OSD_Rect_t *dstRect, HUINT8 ucAlpha);


/**
  @remark	multiply source alpha with given alpha and blit. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_BlitMultiplyAlpha(OSD_Screen_t *source, OSD_Rect_t *srcRect,
									 OSD_Screen_t *destination, OSD_Rect_t *dstRect, HUINT8 ucAlpha);

/**
  @remark	blned bit buffer of the source to the destination. it ignores the dstRect->w and dstRect->h \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern 	void				OSD_GFX_FB_Blend(OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);


/**
  @remark	copy bit buffer of the source to the destinatio. it ignores the dstRect->w and dstRect->h \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern 	void				OSD_GFX_FB_Copy(OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);

/**
  @remark	API for SubFrame. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	OSD_Result_t		OSD_GFX_MakeFrameBuffer(OSD_Screen_t *scr, HINT32 width, HINT32 height);
extern 	OSD_Result_t	 	OSD_GFX_DestoryFrameBuffer(OSD_Screen_t *scr);
extern 	OSD_Result_t		OSD_GFX_UpdateFrameBufOnScreen(OSD_Screen_t *pSrcScreen, OSD_Rect_t *pSrcRect,	OSD_Screen_t *pDstScreen, OSD_Rect_t *pDstRect);



/**
  @remark	blend a frame, consists of 3 decoded stock images vertically aligned. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern 	OSD_Result_t	OSD_GFX_BlendStockFrame(OSD_Screen_t *screen, HUINT32 x1, HUINT32 y1, HUINT32 x2, HUINT32 y2,
						OSD_StockImage_t *pLeftImg, OSD_StockImage_t *pCenterImg, OSD_StockImage_t *pRightImg,
						HUINT8 transparency);


/**
  @remark	copy a frame, consists of 3 decoded stock images vertically aligned. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern 	OSD_Result_t	OSD_GFX_CopyStockFrame(OSD_Screen_t *screen, HUINT32 x1, HUINT32 y1, HUINT32 x2, HUINT32 y2,
						OSD_StockImage_t *pLeftImg, OSD_StockImage_t *pCenterImg, OSD_StockImage_t *pRightImg,
						HUINT8 transparency);


/**
  @remark	blend a frame, consists of 4 decoded stock images vertically aligned. \n
  			center2 image presents some blocks of the part of whole frame.
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern 	OSD_Result_t	OSD_GFX_BlendStockFrame2(OSD_Screen_t *screen,
						HUINT32 x1, HUINT32 y1, HUINT32 x2, HUINT32 y2,
						OSD_StockImage_t *pLeftImg, OSD_StockImage_t *pCenterImg,
						OSD_StockImage_t *pRightImg, OSD_StockImage_t *pCenter2Img,
						HUINT8 transparency);


/**
  @remark	copy a frame, consists of 4 decoded stock images vertically aligned. \n
  			center2 image presents some blocks of the part of whole frame.
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	OSD_Result_t	OSD_GFX_CopyStockFrame2(OSD_Screen_t *screen,
						HUINT32 x1, HUINT32 y1, HUINT32 x2, HUINT32 y2,
						OSD_StockImage_t *pLeftImg, OSD_StockImage_t *pCenterImg,
						OSD_StockImage_t *pRightImg, OSD_StockImage_t *pCenter2Img,
						HUINT8 transparency);

/**
  @remark

  @remark
  @return
*/
extern	OSD_Result_t	OSD_GFX_BlendStockVFrame(OSD_Screen_t *screen, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2,
						OSD_StockImage_t *pTopImg, OSD_StockImage_t *pCenterImg, OSD_StockImage_t *pBottomImg,
						HUINT8 transparency);

/**
  @remark

  @remark
  @return
*/
extern	OSD_Result_t	OSD_GFX_CopyStockVFrame(OSD_Screen_t *screen, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2,
						OSD_StockImage_t *pTopImg, OSD_StockImage_t *pCenterImg, OSD_StockImage_t *pBottomImg,
						HUINT8 transparency);

/**
  @remark	calculate dimmed color value of the source. \n
  @remark	param darkness is 0 to 255, if 255, this returns source color	\n
  @remark	API Type : sync \n
  @return		color \n
*/
extern 	OSD_Color_t		OSD_GFX_GetDimmedColor(OSD_Color_t srcColor, HUINT8 darkness);

/**
  @brief	change or set palette
  @param	s_pCaptionPlane pointer of OSD_Screen
  @param	i index of clut
  @param 	a value of alpha
  @param 	r value of red
  @param 	g value of green
  @param 	b value of blue
*/
OSD_Result_t	OSD_GFX_ChangePalette(OSD_Screen_t *pScr, HUINT32 i, HUINT32 a, HUINT32 r
										, HUINT32 g, HUINT32 b);

/**
  @remark	calculate brightened color value of the source. \n
  @remark	param brightness is 0 to 255, if 0, this returns source color	\n
  @remark	API Type : sync \n
  @return		color \n
*/
extern 	OSD_Color_t		OSD_GFX_GetBrightenColor(OSD_Color_t srcColor, HUINT8 darkness);


extern	void 		OSD_GFX_DrawString(OSD_Screen_t  *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HUINT8 *string, OSD_Color_t color);
extern	HINT32		OSD_GFX_GetStringWidth(OSD_Font_t *font, const HUINT8 *string);
extern	void		OSD_GFX_DrawStringWithAbbr(OSD_Screen_t  *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 w, HUINT8 *string, OSD_Color_t color);
extern	void		OSD_GFX_DrawUCS2StringWithAbbr(OSD_Screen_t  *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 w, const CH_UCS2 *string, OSD_Color_t color);
extern	HINT32		OSD_GFX_DrawMultiLineString(OSD_Screen_t  *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *string, HINT32 vspace, OSD_Color_t color);
extern 	HINT32		OSD_GFX_CountMultiLineStringAlignUTF8( OSD_Font_t *pFont, HINT32 x, HINT32 y, HINT32 w, HINT32 h,
											  HUINT8 *szSrcString, HINT32 nVSpace, HUINT32 *pnLineCount );
extern	HINT32		OSD_GFX_DrawMultiLineStringAlignUTF8WithBG(OSD_Screen_t  *pOsdScr, OSD_Font_t *pFont, HINT32 x, HINT32 y, HINT32 w, HINT32 h,
											  HUINT8 *szSrcString, HINT32 nVSpace, OSD_Color_t cFontColor, HINT32 nAlign, OSD_Color_t cOsdBgColor);
extern	HINT32		OSD_GFX_CountMultiLine(OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *string, HINT32 vspace);
extern	HINT32		OSD_GFX_DrawMultiLineStringAlign(OSD_Screen_t  *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *string, HINT32 vspace, OSD_Color_t color, HINT32 align );
extern	HINT32		OSD_GFX_DrawMultiLineStringAlignWithEncoding(OSD_Screen_t  *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *string, HINT32 vspace, OSD_Color_t color, HINT32 align );
extern	HINT32		OSD_GFX_GetStringWidthMultiLine(OSD_Font_t *font, HUINT8 *string);

/**
  @remark	break the input string by the width, returns the remaining. \n
  @remark	API Type : sync \n
  @return		color \n
*/
extern HUINT8 *		OSD_GFX_GetStringLine(OSD_Font_t *font, HINT32 w, HUINT8 *string, HINT32 *outCharNum);
extern HERROR		OSD_GFX_DrawSingleByteStrFit(OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HUINT32 color, HUINT8 *pSingleByteStr, HUINT32 ulStrLen, HUINT32 ulRectWidth);
extern HERROR		OSD_GFX_DrawUCS2StrFit(OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HUINT32 color, const CH_UCS2 *pUCS2Str, HUINT32 ulStrLen, HUINT32 ulRectWidth);

/**
  @description	: parameter 들에 맞게 text draw 하는 함수
  @type			: Inline
  @return		: 실제 draw 할 text length
*/
extern HINT32			OSD_GFX_DrawTextByLimit(OSD_Screen_t 		*screen,		/**< [in] */
										HINT32 				nObjectId,		/**< [in] gwm object id */
										OSD_FontId_t 		eFontId,		/**< [in] */
										OSD_Font_Align_t 	eAlign,			/**< [in] */
										HUINT32 			ulFontSize,		/**< [in] FONT_SIZE_XXL, XL, L, M, S, XS, XXS */
										HINT32 				x,				/**< [in] */
										HINT32 				y,				/**< [in] */
										HINT32 				w,				/**< [in] */
										HUINT8 				*szText,		/**< [in] draw할 text */
										OSD_Color_t 		ulTextColor);	/**< [in] color 값, COL(C_T_Subtitle01) 와 같은 형태로 넘겨 줄 수 있음 */


/**
  @remark	set the screen frontmost. \n
  @remark	API Type : sync \n
  @return		OK or FAIL \n
*/
extern OSD_Result_t	OSD_GFX_SetVisualScreen(OSD_Screen_t *pstOsdScr,  HBOOL bIsVisible);


#define ____FRAMBUFFER_DRAW_FUNC____
/**
  @remark	blned bit buffer of the source to the destination. it draws exact the range of dstRect W/H \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_FB_BlendRect(OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);


/**
  @remark	copy bit buffer of the source to the destinatio. it draws exact the range of dstRect W/H \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_FB_CopyRect(OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);


/**
  @remark	blned bit buffer of the source to the destination. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_FB_BlendStretch (OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);


/**
  @remark	copy bit buffer of the source to the destination. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_FB_CopyStretch (OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);



/**
  @remark	blned bit buffer of the source to the destination. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern	void				OSD_GFX_FB_BlendScale (OSD_Screen_Buffer_t srcBuff, HINT32 srcWidth, HINT32 srcHeight,
									OSD_Screen_t *destination, OSD_Rect_t *dstRect);


extern OSD_Result_t			OSD_GFX_CopyFrameRound(	OSD_Screen_t *screen,
															HUINT32 x1,
															HUINT32 y1,
															HUINT32 x2,
															HUINT32 y2,
															OSD_StockImage_t *pLeftTopImg,
															OSD_StockImage_t *pRightTopImg,
															OSD_StockImage_t *pLeftBottomImg,
															OSD_StockImage_t *pRightBottomImg,
															HINT32 lAlpha);

extern OSD_Result_t			OSD_GFX_CopyFrameRoundWithFillRect(	OSD_Screen_t *screen,
															HUINT32 x1,
															HUINT32 y1,
															HUINT32 x2,
															HUINT32 y2,
															OSD_StockImage_t *pLeftTopImg,
															OSD_StockImage_t *pRightTopImg,
															OSD_StockImage_t *pLeftBottomImg,
															OSD_StockImage_t *pRightBottomImg,
															OSD_Color_t fillcolor,
															HINT32 lAlpha);


extern OSD_Result_t				OSD_GFX_CopyVFrame(	OSD_Screen_t *screen,
													HUINT32 x1,
													HUINT32 y1,
													HUINT32 x2,
													HUINT32 y2,
													OSD_StockImage_t *pTopImg,
													OSD_StockImage_t *pMiddleImg,
													OSD_StockImage_t *pBottomImg,
													HINT32 lAlpha);

extern	void				OSD_GFX_GetImageTransparency(OSD_Image_Handle_t hImgHandle,HINT32 *plTransparency);

extern	void				OSD_GFX_CopyImage(	OSD_Image_Handle_t hImgHandle,
													OSD_Screen_t *pOsdSreen,
													OSD_Rect_t *pDstRect,
													HINT32 lAlpha);

extern	void				OSD_GFX_StretchImage (	OSD_Image_Handle_t hImgHandle,
														OSD_Screen_t *pOsdSreen,
														OSD_Rect_t *pDstRect,
														HINT32 lAlpha);

extern HERROR			OSD_GFX_DrawStockImage(	OSD_Screen_t *pPixmap,
														HINT32 lStartX,
														HINT32 lStartY,
														OSD_Image_Handle_t hImage,
														HINT32 lAlpha);

extern OSD_Result_t		OSD_GFX_CopyFrame(	OSD_Screen_t *screen,
								  HUINT32 x1,
								  HUINT32 y1,
								  HUINT32 x2,
								  HUINT32 y2,
								  OSD_StockImage_t *pLeftImg,
								  OSD_StockImage_t *pCenterImg,
								  OSD_StockImage_t *pRightImg,
								  HINT32 lAlpha);

extern  void	OSD_GFX_BlitFillScreen(OSD_Screen_t	*pOsdScr, HINT32 nX, HINT32 nY, HINT32 nW
										, HINT32 nH, OSD_Color_t cColor);

/**
  @remark	Show the screen. \n
  @remark	API Type : sync \n
  @return		OK or FAIL \n
*/
extern OSD_Result_t	OSD_GFX_ShowScreen(OSD_Screen_t *scr);


/**
  @remark	Hide screen. \n
  @remark	API Type : sync \n
  @return		OK or FAIL \n
*/
extern OSD_Result_t	OSD_GFX_HideScreen(OSD_Screen_t *scr);
/* for fill color blended with background */
extern	void	OSD_GFX_DrawRectBlendFill(OSD_Screen_t  *pixmap, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, OSD_Color_t color);


/**
  @remark	copy bit buffer of the source to the destination. \n
  @remark	API Type : sync \n
  @return	n/a \n
*/
extern void				OSD_GFX_FB_CopyScale (OSD_Screen_Buffer_t srcBuff, OSD_Rect_t *srcRect, OSD_Screen_t *destination, OSD_Rect_t *dstRect);

extern HBOOL 			OSD_GFX_InitializeArea(void);
extern void 			OSD_GFX_DestroyArea(void);

extern HINT32		OSD_GFX_GetStartPositionX_DrawString( OSD_Font_t *pFont, HINT32 x, HINT32 nOrgRectWidth,  HUINT16 eEncoding, HUINT8 *pString, OSD_Font_Align_t strAlign, HBOOL bAbbreviation);

extern OSD_Color_t 		OSD_GFX_GetARGB32FromColor(OSD_Screen_t *pstOsdScreen, HUINT8 ucAlpha, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue);
extern OSD_Color_t 		OSD_GFX_GetRGB32FromColor(OSD_Screen_t *pstOsdScreen, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue);
extern OSD_Color_t 		OSD_GFX_GetOSDColor32FromColor(OSD_Screen_t *pstOsdScreen, OSD_Color_t ulColor);

extern OSD_Color_t 		OSD_GFX_GetColorFromOSDColor32(OSD_Screen_t *pstOsdScreen, OSD_Color_t ulColor);
extern OSD_Color_t 		OSD_GFX_GetColorFromRGB32(OSD_Screen_t *pstOsdScreen, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue);
extern OSD_Color_t 		OSD_GFX_GetColorFromARGB32(OSD_Screen_t *pstOsdScreen, HUINT8 ucAlpha, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue);
extern OSD_Color_t		OSD_GFX_AddAlphaOSDColor(OSD_Screen_t *pstOsdScreen, HUINT8 ucAlpha, OSD_Color_t ulColor);

/**
  @remark	get the color of given pixel. \n
  @remark	API Type : sync \n
  @return		color \n
*/
extern OSD_Color_t		OSD_GFX_GetPixel(OSD_Screen_t *pixmap, HINT32 x, HINT32 y);
extern HERROR			OSD_GFX_GetClippedArea(	OSD_Screen_t *pPixmap, HINT32 lStartX, HINT32 lStartY, HINT32 lWidth, HINT32 lHeight, OSD_Rect_t *pClippedRect);
extern HERROR			OSD_GFX_DrawStockImageWithRange (OSD_Screen_t *pPixmap, HINT32 lStartX, HINT32 lStartY, HINT32 lWidth, HINT32 lHeight, void *pvImageInfo);
extern OSD_Color_t 		OSD_GFX_GetSysDimmedColor(OSD_Screen_t *pstScreen, OSD_Color_t srcColor, HUINT8 darkness);
extern OSD_Color_t 		OSD_GFX_GetAlphaBlendColor(OSD_Screen_t *pstScreen, OSD_Color_t cSrcOsdColor, HUINT8 ucAlpha, HINT32 x, HINT32 y);
/**
  @remark	draw a frame having a default look. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern	void			OSD_GFX_DrawFrame(OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HINT32 bright, HINT32 face, HINT32 dark);

/**
  @remark	draw a 32-bit(ARGB) bitmap, generated with the imgtool. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern 	HINT32	OSD_GFX_DrawBitmap(OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HUINT8 *bmp_ptr, OSD_BmpBlendMode_t mode);
extern	HINT32	OSD_GFX_DrawBitmapWithRange (OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *bmp_ptr, OSD_BmpBlendMode_t mode);
extern	HINT32	OSD_GFX_DrawThumbnailBitmap(OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT32 *bmp_ptr);
extern	HINT32	OSD_GFX_DrawBitmapWithScale (OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *bmp_ptr, OSD_BmpBlendMode_t mode);

/**
  @remark	draw a rectangluar frame consists of 3 images, LCR form. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern 	HINT32	OSD_GFX_DrawImageFrame(OSD_Screen_t *pixmap, HUINT32 x1, HUINT32 y1, HUINT32 x2, HUINT32 y2,
						HUINT8 *leftBmpPtr, HUINT8 *centerBmpPtr, HUINT8 *rightBmpPtr,
						OSD_BmpBlendMode_t mode, HUINT8 transLevel);
/**
  @remark	draw a dimmed 32-bit(ARGB) bitmap, generated with the imgtool. \n
  @remark	API Type : sync \n
  @return		n/a \n
*/
extern 	HINT32	OSD_GFX_DrawDimmedBitmap(OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HUINT8 *bmp_ptr, OSD_BmpBlendMode_t mode, HUINT8 darkness);
extern 	HINT32	OSD_GFX_GetBitmapWidth(HUINT8 *pBmp);
extern	HINT32 	OSD_GFX_GetBitmapHeight(HUINT8 *pBmp);


extern HERROR	OSD_GFX_IsEnoughSpaceForText(OSD_FontId_t eFontId, OSD_Font_Align_t eAlign, HUINT32 ulFontSize, HINT32 w, HINT8 *szText, HBOOL *pbIsEnoughSpace);
extern HERROR	OSD_GFX_IsEnoughSpaceForText_WithFont(OSD_FontId_t eFontId, OSD_Font_Align_t eAlign, OSD_Font_t *pFont, HINT32 w, HINT8 *szText, HBOOL *pbIsEnoughSpace);

extern OSD_Screen_t		*OSD_GFX_GetScreen(OSD_ScreenType_t eOsdScrType);
extern OSD_Result_t		OSD_GFX_ChangeScreenSize(OSD_Screen_t *scr, OSD_Rect_t *pRect);
extern	OSD_Result_t  OSD_GFX_MakeFrameBuffer8bpp(OSD_Screen_t *scr, int width, int height);

/* UI stock image management */
#if defined(CONFIG_PACKAGE_LIBPNG)
extern OSD_Result_t		OSD_GFX_MakeStockImage(HUINT8 *pImgFile, HUINT32 *phBuffer, HUINT32 *outWidth, HUINT32 *outHeight);
extern OSD_Result_t		OSD_GFX_MakeStockImageWithNoPreAlpha (HUINT8 *pImgFile, HUINT32 *phBuffer, HUINT32 *outWidth, HUINT32 *outHeight);
extern OSD_Result_t		OSD_GFX_DeleteStockImage(HUINT32 hBuffer);
#endif /* #if defined(CONFIG_PACKAGE_LIBPNG) */

#if defined(CONFIG_PACKAGE_PCX)
extern HERROR 			OSD_GFX_CreateImageFromResource(OSD_Image_Resource_Info_t* pImageResourceInfo, OSD_Image_Handle_t* phImage);
extern HERROR 			OSD_GFX_GetImageSize(OSD_Image_Handle_t hImage, OSD_Size_t* pSize);
extern HERROR 			OSD_GFX_DestroyImage(OSD_Image_Handle_t hImage);
#endif /* #if defined(CONFIG_PACKAGE_PCX) */

#if	defined(CONFIG_MWC_OSD_RESIZABLE_DATA_PLANE)
extern	OSD_Result_t	OSD_GFX_ChangeDataScreenResolution(HINT32 w, HINT32 h);
extern	OSD_Result_t	OSD_GFX_SetDefaultDataScreenResolution(void);
#endif

#if defined(CONFIG_MWC_OSD_BGPLANE1)
extern OSD_Screen_t		*OSD_GFX_GetBG1Screen(void);
#endif
#if defined(CONFIG_MWC_OSD_BGPLANE1) && defined(CONFIG_MWC_OSD_BGPLANE2)
extern OSD_Screen_t		*OSD_GFX_GetBG2Screen(void);
#endif

extern OSD_Result_t		OSD_GFX_SetVideoRectOnBGPlane(OSD_Rect_t *pRect);
extern OSD_Result_t		OSD_GFX_SetOSDPlaneInOutRect(OSD_Screen_t *pScr, OSD_Rect_t *pInRect, OSD_Rect_t *pOutRect);
extern OSD_Result_t		OSD_GFX_GetOSDPlaneInOutRect(OSD_Screen_t *pScr, OSD_Rect_t *pInRect, OSD_Rect_t *pOutRect);


#if defined(CONFIG_PACKAGE_PCX)
extern void		OSD_GFX_StretchImagePcx (	OSD_Image_Handle_t hImgHandle,
									 OSD_Screen_t *pOsdSreen,
									 OSD_Rect_t *pDstRect,
									 HINT32 lAlpha);
#endif


#if defined(CONFIG_PACKAGE_LIBPNG)
//OSD_Result_t	OSD_GFX_DrawDecodedPng( OSD_Screen_t *screen, int x, int y, PNGPixMap *bmp);
#endif

OSD_Color_t* OSD_GFX_GetDefaultPalette(void);

#endif	/*__OSD_GFX_H__ */


