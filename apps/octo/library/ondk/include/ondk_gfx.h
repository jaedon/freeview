/**************************************************************
 *	@file		mlib.h
 *	Minimized Graphic Library for DirectFB
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Minimized Graphic Library for DirectFB
 **************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#ifndef ____ONDK_GFX_H____
#define ____ONDK_GFX_H____


/******************************************************************************/
#define _________Font_________________________________________________________
/******************************************************************************/
/* size degrees for font */
#define	ONDK_FONT_SIZE_64						64
#define	ONDK_FONT_SIZE_46						46
#define	ONDK_FONT_SIZE_40						40
#define	ONDK_FONT_SIZE_36						36
#define	ONDK_FONT_SIZE_32						32
#define	ONDK_FONT_SIZE_30						30
#define	ONDK_FONT_SIZE_28						28
#define	ONDK_FONT_SIZE_26						26
#define	ONDK_FONT_SIZE_24						24
#define	ONDK_FONT_SIZE_22						22
#define	ONDK_FONT_SIZE_20						20
#define	ONDK_FONT_SIZE_18						18

typedef		enum
{
	TOP_LEFT_CORNER = 0,
	TOP_RIGHT_CORNER,
	BOTTOM_LEFT_CORNER,
	BOTTOM_RIGHT_CORNER
} ROUND_CORNER_TYPE;


/*
 * Flags controlling the text layout.
 * dfb enum same
 */
typedef enum {
	ONDKSTF_LEFT			= 0x00000000,  /* left aligned */
	ONDKSTF_CENTER			= 0x00000001,  /* horizontally centered */
	ONDKSTF_RIGHT			= 0x00000002,  /* right aligned */

	ONDKSTF_TOP				= 0x00000004,  /* y specifies the top
										   instead of the baseline */
	ONDKSTF_BOTTOM			= 0x00000008,  /* y specifies the bottom
										   instead of the baseline */
	ONDKSTF_OUTLINE			= 0x00000010,  /* enables outline rendering if loaded font supports it */

	ONDKSTF_TOPLEFT			= ONDKSTF_TOP | ONDKSTF_LEFT,
	ONDKSTF_TOPCENTER		= ONDKSTF_TOP | ONDKSTF_CENTER,
	ONDKSTF_TOPRIGHT		= ONDKSTF_TOP | ONDKSTF_RIGHT,

	ONDKSTF_BOTTOMLEFT		= ONDKSTF_BOTTOM | ONDKSTF_LEFT,
	ONDKSTF_BOTTOMCENTER	= ONDKSTF_BOTTOM | ONDKSTF_CENTER,
	ONDKSTF_BOTTOMRIGHT		= ONDKSTF_BOTTOM | ONDKSTF_RIGHT,

	 /* humax added */
	ONDKSTF_HUMAX_MASK		= 0xFFFF0000,
	// for multiline
	ONDKSTF_WORDWRAP		= 0x00010000,
	ONDKSTF_EQUALSPACE		= 0x00020000, /* 균등 분할 */
	// center
	ONDKSTF_MIDDLE			= 0x00100000,

	ONDKSTF_TextFlags_MAX
} ONDKSurfaceTextFlags;


typedef void	*(*DEF_CB_GetProcID)	(void);

typedef struct tagONDK_DFB_IMG_Settings_t
{
	DEF_CB_GetProcID	 	cbGetProcId;
	HINT32					nProcMaxCount;

} ONDK_DFB_IMG_Settings_t;

typedef struct tagFontMultiLine_Item_t
{
	HCHAR 	*pszString;
	HINT32 	lineBytes;
	HINT32	lineWidth;
	HINT32	textWidth;
	HINT32	spaceWidth;
	HINT32	spaceCount;
	HBOOL	endLine;
} FontMultiLine_Item_t;

/**************************************************************
	public functions
 **************************************************************/
 /* init */
extern	ONDK_Result_t		ONDK_Init(HINT32 argc, HCHAR **argv);
extern	ONDK_Result_t  		ONDK_SYS_Init(HINT32 argc, HCHAR **argv);
extern 	ONDK_Result_t  		ONDK_STDLIB_Init(HINT32 argc, HCHAR **argv);
extern	ONDK_Result_t 		ONDK_TIMER_Init(HINT32 argc, HCHAR **argv);
extern  ONDK_Result_t	 	ONDK_KEY_Init(HINT32 argc, HCHAR **argv);
extern	ONDK_Result_t  		ONDK_DFB_Init(HINT32 argc, HCHAR **argv);
extern	ONDK_Result_t  		ONDK_IMG_Init(HINT32 argc, HCHAR **argv);
extern	ONDK_Result_t  		ONDK_FONT_Init(HINT32 argc, HCHAR **argv);
extern 	ONDK_Result_t		ONDK_DFB_COLOR_Init(HINT32 argc, HCHAR **argv);
extern	ONDK_Result_t  		ONDK_DFB_VIDEO_Init(HINT32 argc, HCHAR **argv);

/* destroy */
extern	ONDK_Result_t		ONDK_Destroy(void);
extern	ONDK_Result_t		ONDK_DFB_Destroy(void);




 /*
  * window
  */
extern	ONDKWindowHandle 	ONDK_WINDOW_GetFromWindowID(HUINT32 windowID);
extern HINT32              ONDK_WINDOW_GetIDFromWindow(ONDKWindowHandle hWindow);
extern HINT32	ONDK_WINDOW_GetWindowIDFromWindow(ONDKWindowHandle hWindow);
extern	ONDKWindowHandle	ONDK_WINDOW_Create(HUINT32 applId, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *pixelformat, HBOOL focusable);
extern ONDKWindowHandle	    ONDK_WINDOW_CreateInputOnly(HUINT32 applId);

extern	void				ONDK_WINDOW_SetShow(ONDKWindowHandle mWindow, HBOOL bShow);
extern  HBOOL				ONDK_WINDOW_IsShow(ONDKWindowHandle mWindow);
extern  void				ONDK_WINDOW_SetOpacity(ONDKWindowHandle mWindow, HUINT32 opacity);
extern	void				ONDK_WINDOW_Clear(ONDKWindowHandle mWindow);
extern 	void			 	ONDK_WINDOW_Destroy(ONDKWindowHandle mWindow);
extern	void				ONDK_WINDOW_Update(ONDKWindowHandle mWindow, ONDK_Rect_t *rc);
extern	void			 	ONDK_WINDOW_UpdateFlipNone(ONDKWindowHandle mWindow, ONDK_Rect_t *rc);
extern	void			 	ONDK_WINDOW_UpdateFlipOnSync(ONDKWindowHandle mWindow, ONDK_Rect_t *rc);
extern	ONDK_Result_t		ONDK_WINDOW_Init(HINT32 argc, HCHAR **argv);
extern	void				ONDK_WINDOW_Blit(ONDKWindowHandle _window, ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, HINT32 x, HINT32 y);
extern	void				ONDK_WINDOW_BlendBlit(ONDKWindowHandle _window, ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, HINT32 x, HINT32 y);
extern	void				ONDK_WINDOW_StretchBlendBlit(ONDKWindowHandle _window,  ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect);
extern	void				ONDK_WINDOW_StretchBlit(ONDKWindowHandle _window,  ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect);
extern	void				ONDK_WINDOW_SetResolution(ONDKWindowHandle window, HINT32 w, HINT32 h);
extern	void		 		ONDK_WINDOW_SetOutputAspectRatio(MOSD_ASPECTRATIO aspect);
extern	void				DBG_ONDK_GWM_GFX_UpdateScreenShowLine(void);




/*
 * video
 */
extern	ONDKVideoHandle 	ONDK_VIDEO_Create( const char *a_pFile );
extern	ONDK_Result_t 		ONDK_VIDEO_PlayOnece( 	ONDKVideoHandle vhandle,
													ONDKSurfaceHandle targetSurface,
													ONDK_Rect_t *rt,
													void (* updateScreen)(const void *),
													void *userdata);
extern	ONDK_Result_t 		ONDK_VIDEO_PlayLoop( 	ONDKVideoHandle vhandle,
													ONDKSurfaceHandle targetSurface,
													ONDK_Rect_t *rt,
													void (* updateScreen)(const void *),
													void *userdata);
extern	ONDK_Result_t 		ONDK_VIDEO_PlayRewind( 	ONDKVideoHandle vhandle,
													ONDKSurfaceHandle targetSurface,
													ONDK_Rect_t *rt,
													void (* updateScreen)(const void *),
													void *userdata);
extern	ONDK_Result_t 		ONDK_VIDEO_SeekTo(ONDKVideoHandle vhandle, HINT32 seconds);
extern	ONDK_Result_t 		ONDK_VIDEO_GetPos(ONDKVideoHandle vhandle, HINT32 *ret_seconds);
extern	ONDK_Result_t 		ONDK_VIDEO_GetLength(ONDKVideoHandle vhandle, HINT32 *ret_seconds);
extern	ONDK_Result_t		ONDK_VIDEO_IsBusy(ONDKVideoHandle vhandle);
extern	ONDK_Result_t 		ONDK_VIDEO_Stop( ONDKVideoHandle vhandle);
extern	ONDK_Result_t 		ONDK_VIDEO_Release( ONDKVideoHandle vhandle);
extern ONDK_Result_t 		ONDK_VIDEO_SetSpeed(ONDKVideoHandle vhandle, HFLOAT64 speed);
extern ONDK_Result_t 		ONDK_VIDEO_GetSpeed(ONDKVideoHandle vhandle, HFLOAT64 *speed);

/*
 * surface
 */
extern	ONDKSurfaceHandle	ONDK_SURFACE_GetFromWindow(ONDKWindowHandle mWindow);
extern	ONDKSurfaceHandle	ONDK_SURFACE_Create(HINT32 width, HINT32 height, HINT8 *pixelformat);
extern	ONDKSurfaceHandle	ONDK_SURFACE_CreateWithPixelFormatIndex(HINT32 width, HINT32 height, HINT32 pixelformat);
extern	void 				ONDK_SURFACE_Blitflag_SetColorCopy(ONDKSurfaceHandle surface, HUINT8 a, HUINT8 r, HUINT8 g, HUINT8 b);
extern	void 				ONDK_SURFACE_Blitflag_SetCopy(ONDKSurfaceHandle surface);
extern	void 				ONDK_SURFACE_Blitflag_SetBlendWithVideo(ONDKSurfaceHandle surface);
extern	void 				ONDK_SURFACE_Blitflag_SetBlend(ONDKSurfaceHandle surface);
extern	void 				ONDK_SURFACE_Blitflag_SetColorBlend(ONDKSurfaceHandle surface, HUINT8 a, HUINT8 r, HUINT8 g, HUINT8 b);
extern	void 				ONDK_SURFACE_Blitflag_SetAlphaBlend( ONDKImageHandle surface, HUINT8 a);
extern	ONDK_Result_t		ONDK_SURFACE_Init(HINT32 argc, HCHAR **argv);
extern	void 				ONDK_SURFACE_Blit(ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, int x, int y);
extern	void 				ONDK_SURFACE_BlitWithoutBlend(ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, int x, int y);
extern	void	 			ONDK_SURFACE_StretchBlit( ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect);
extern	void	 			ONDK_SURFACE_StretchBlitWithoutBlend( ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect);
extern	void				ONDK_SURFACE_Destroy(ONDKSurfaceHandle surface);
extern	void 				ONDK_SURFACE_GetClip(ONDKSurfaceHandle surface, ONDK_Region_t *region);
extern	void 				ONDK_SURFACE_GetClipElement(ONDKSurfaceHandle surface, HINT32 *x1, HINT32 *y1, HINT32 *x2, HINT32 *y2);
extern	void 				ONDK_SURFACE_SetClip(ONDKSurfaceHandle surface, ONDK_Region_t region);
extern  void				ONDK_SURFACE_Clear(ONDKSurfaceHandle surface);
extern 	void				ONDK_SURFACE_ClearRect(ONDKSurfaceHandle surface, ONDK_Region_t region);
extern	void 				ONDK_SURFACE_GetSize(ONDKSurfaceHandle surface, HINT32 *width, HINT32 *height);
extern	HUINT32*			ONDK_SURFACE_Lock(ONDKSurfaceHandle _surface);
extern	void				ONDK_SURFACE_Unlock(ONDKSurfaceHandle _surface);
extern	void 				ONDK_SURFACE_FlipBlit(ONDKSurfaceHandle surface, ONDK_Rect_t *rc);
extern	void 				ONDK_SURFACE_FlipWithForSync(ONDKSurfaceHandle surface, ONDK_Rect_t *rc);



/*
 * math
 */
extern	ONDK_Color_t			ONDK_Color(HUINT8 a, HUINT8 r, HUINT8 g, HUINT8 b);
extern	ONDK_Rect_t			ONDK_Rect(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern	ONDK_Size_t	 		ONDK_Size(HINT32 w, HINT32 h);
extern	ONDK_Point_t 		ONDK_Point(HINT32 x, HINT32 y);
extern	ONDK_Region_t		ONDK_Region(HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2);
extern	ONDK_Region_t 		ONDK_RectToRegion(ONDK_Rect_t rt);
extern	ONDK_Rect_t	 		ONDK_RegionToRect(ONDK_Region_t rn);
extern	ONDK_Region_t		ONDK_UnionRegion(const ONDK_Region_t *r1, const ONDK_Region_t *r2);
extern	HBOOL				ONDK_IsEmptyRegion(const ONDK_Region_t *r);
extern	ONDK_Region_t		ONDK_IntersectRegion (const ONDK_Region_t *r1, const ONDK_Region_t *r2);



extern	HBOOL				ONDK_IsOverlapped (const ONDK_Rect_t *r1, const ONDK_Rect_t *r2);
extern	HBOOL				ONDK_IsOverlappedRegion (const ONDK_Region_t *r1, const ONDK_Region_t *r2);

extern	ONDK_Rect_t			ONDK_IntersectRect (const ONDK_Rect_t *r1, const ONDK_Rect_t *r2);
extern	ONDK_Rect_t	 		ONDK_UnionRect(const ONDK_Rect_t *r1, const ONDK_Rect_t *r2);
extern	HBOOL 				ONDK_CmpRect(ONDK_Rect_t* r1, ONDK_Rect_t* r2);
extern	HBOOL 				ONDK_IsInnerRect(ONDK_Rect_t* r1, ONDK_Rect_t* r2);
extern	HBOOL 				ONDK_IsInnerRegion(ONDK_Region_t* r1, ONDK_Region_t* r2);

extern	HBOOL				ONDK_IsEmptyRect(const ONDK_Rect_t *r);
extern	void				ONDK_CopyRect(ONDK_Rect_t *dst, ONDK_Rect_t *src);

/*
 * color
 */

extern ONDK_Color_t			ONDK_COLOR_BlendWithAlpha(ONDK_Color_t fg, ONDK_Color_t bg, HUINT8 alpha);



/*
 * key
 */

extern	HUINT32	            	ONDK_KEY_RegisterKeyCallback(ONDKWindowHandle mWindow, ONDK_KEY_KeyNotifyCallback_t pfnKeyNotifyCallback);
extern	ONDKHumaxKeyHandle 		ONDK_KEY_GetBuffer(ONDKWindowHandle hWindow);
extern 	void 					ONDK_KEY_SetBuffer(ONDKWindowHandle mWindow);
extern	void 					ONDK_KEY_ReleaseBuffer(ONDKWindowHandle mWindow);
extern  void					ONDK_KEY_Active(HBOOL bActive);
extern	HUINT32 ONDK_KEY_GetTaskId(void);
/*
 * draw
 */

extern	void	 		ONDK_DRAW_Rect(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color);
extern	void	 		ONDK_DRAW_RectElement(ONDKSurfaceHandle _surface,  HINT32 x, HINT32 y, HINT32 w, HINT32 h, ONDK_Color_t _color);

extern	void			ONDK_DRAW_FillRect(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color);
extern	void	 		ONDK_DRAW_FillRectWithoutColorBlend(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc);
extern	void	 		ONDK_DRAW_FillCopyRect(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color);
extern	void	 		ONDK_DRAW_FillRectElement(ONDKSurfaceHandle _surface, HINT32 x, HINT32 y, HINT32 w, HINT32 h, ONDK_Color_t _color);
extern	void	 		ONDK_DRAW_FillRectToVideoSurface(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color);


extern	void			ONDK_DRAW_FillRect16(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color);
extern	void			ONDK_DRAW_FillRectLUT(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, HUINT8 _color);
extern	void			ONDK_DRAW_FillGradationRect(ONDKSurfaceHandle surface, ONDK_Rect_t rc, HBOOL isHorizontal, ONDK_Color_t c1, ONDK_Color_t c2 );
extern	void 			ONDK_DRAW_Line(ONDKSurfaceHandle surface, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, ONDK_Color_t color);
extern	void            ONDK_DRAW_Bitmap(ONDKSurfaceHandle surface, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pucImgOrg);
extern	void			ONDK_DRAW_StrokeCircle(ONDKSurfaceHandle surface, HINT32 x0, HINT32 y0, HINT32 radius, ONDK_Color_t color);
extern	void			ONDK_DRAW_FillCircle(ONDKSurfaceHandle surface, HINT32 x0, HINT32 y0, HINT32 radius, ONDK_Color_t color);
extern	void			ONDK_DRAW_StrokeRoundCorner(ONDKSurfaceHandle surface, HINT32 x0, HINT32 y0, HINT32 radius, HINT32 corner, ONDK_Color_t color);
extern	void			ONDK_DRAW_FillRoundCorner(ONDKSurfaceHandle surface, HINT32 x0, HINT32 y0, HINT32 radius, HINT32 corner, ONDK_Color_t color);
extern	void			ONDK_DRAW_StrokeRoundRect(ONDKSurfaceHandle surface, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, HINT32 r, ONDK_Color_t color);
extern	void			ONDK_DRAW_StrokeRect(ONDKSurfaceHandle surface, int x1, int y1, int x2, int y2, int r, ONDK_Color_t color);
extern	void			ONDK_DRAW_FillRoundRect(ONDKSurfaceHandle surface, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2, HINT32 r, ONDK_Color_t color);
extern	void			ONDK_DRAW_GetColorLookupTable(ONDKSurfaceHandle _surface, ONDK_Color_t *lookup_table, HINT32 start_index, HINT32 end_index );
extern	void			ONDK_DRAW_SetColorLookupTable(ONDKSurfaceHandle _surface, ONDK_Color_t *lookup_table, HINT32 start_index, HINT32 end_index );

extern	void 			ONDK_DRAW_MediaCaption_Draw(ONDKSurfaceHandle surface, ONDKFontHandle font, HCHAR *pstr, HUINT32 x, HUINT32 y, ONDK_Color_t color);

extern	void		ONDK_DRAW_DrawButton( ONDKSurfaceHandle _surface, ONDK_Rect_t rect, ONDKFontHandle fontHandle, HUINT8 *title, int hilight);
extern	void		ONDK_DRAW_DrawDialogFrame( ONDKSurfaceHandle _surface, ONDK_Rect_t rect, ONDKFontHandle fontHandle, HUINT8 *title);
extern	void		ONDK_DRAW_SystemNoticeDialog( ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT8 *text_message);


extern  void			ONDK_FONT_DrawStringMediaSubtitle_ToWindow(ONDKWindowHandle _window, HCHAR *pstr , HUINT32 nFontSize, HUINT32 nPosition);
extern  void			ONDK_FONT_DrawStringMediaSubtitle_ToSurface(ONDKSurfaceHandle _surface, HCHAR *pstr , HUINT32 nFontSize, HUINT32 nPosition_X,HUINT32 nPosition_Y ,ONDK_Color_t color,HUINT8 *path);
extern HINT32 			ONDK_FONT_DrawStringForMultiLine(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr, HUINT32 vspace, ONDK_Color_t color);

/*
 * Image
 */
extern ONDK_Result_t 		ONDK_IMG_Settings(ONDK_DFB_IMG_Settings_t *pSettings);
extern ONDKImageHandle 		ONDK_IMG_Create(HINT32 width, HINT32 height, HINT8 *pixelformat,const HUINT8* path);
extern void 				ONDK_IMG_Blit( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, HINT32 x, HINT32 y);
extern void 				ONDK_IMG_BlitAlpha( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, HINT32 x, HINT32 y, HUINT8 alpha);
extern void 				ONDK_IMG_StretchBlit( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t dstRect);
extern void 				ONDK_IMG_StretchBlitAlpha( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t dstRect, HUINT8 alpha);
extern ONDK_Size_t 			ONDK_IMG_NonCreateOnlyGetSize(const HUINT8* path);
extern void 				ONDK_IMG_StretchBlitPartRectAlpha( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect, HUINT8 alpha);
extern void 				ONDK_IMG_StretchBlitPartRect( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect);

extern ONDK_Size_t 			ONDK_IMG_GetSize( ONDKImageHandle surface);
extern void	 				ONDK_IMG_Destroy(ONDKImageHandle surface);
extern ONDKImageHandle		ONDK_IMG_CreateSelf(const HUINT8* path);
extern void 				DBG_ONDK_IMG_Dump(void *procId);
extern void	 				ONDK_IMG_DestroyByProcId(HUINT32 procId);
/*
 * FONT
 */
extern	ONDKFontHandle		ONDK_FONT_Create(const HINT8 *path, HINT32 w, HINT32 h);
extern	ONDKFontHandle		ONDK_FONT_CreateRotationFont(ONDK_FontId_t id, HINT32 w, HINT32 angle);
extern	ONDKFontHandle 		ONDK_FONT_CreateSystemFont(ONDK_FontId_t id,  HUINT32 size);
extern	void				ONDK_FONT_Release(ONDKFontHandle   fontHandle);

extern	void				ONDK_FONT_DrawString(ONDKSurfaceHandle _surface, ONDKFontHandle   fontHandle,HCHAR *pstr ,HUINT32 x ,HUINT32 y ,ONDK_Color_t color, ONDKSurfaceTextFlags textflag);
extern	void 				ONDK_FONT_DrawStringAbbr(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr ,HUINT32 x ,HUINT32 y, HUINT32 w, ONDK_Color_t color);
extern	void 				ONDK_FONT_DrawStringAlign(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr , HUINT32 x ,HUINT32 y, HUINT32 w, ONDK_Color_t color, ONDKSurfaceTextFlags textflag);
extern	void 				ONDK_FONT_DrawStringAlignAbbr(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr , HUINT32 x ,HUINT32 y, HUINT32 w, ONDK_Color_t color, ONDKSurfaceTextFlags textflag);
extern	void 				ONDK_FONT_DrawStringAlignRetPos(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr , HUINT32 x ,HUINT32 y, HUINT32 w, ONDK_Color_t color, ONDKSurfaceTextFlags textflag, HINT32 *ret_pWidth);
extern	void 				ONDK_FONT_DrawStringAlignAbbrRetPos(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr , HUINT32 x ,HUINT32 y, HUINT32 w, ONDK_Color_t color, ONDKSurfaceTextFlags textflag, HINT32 *ret_pWidth);
extern 	HBOOL 				ONDK_FONT_DrawStringPosition(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr ,HUINT32 drawStartPos, HUINT32 drawEndPos, HUINT32 x ,HUINT32 y, HUINT32 w,ONDK_Color_t color,ONDKSurfaceTextFlags textflag);


extern HINT32				ONDK_FONT_DrawOneCharacter(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HCHAR *utf8pstr, HINT32 x, HINT32 y, ONDK_Color_t color, HINT32 *out_width);
extern HINT32				ONDK_FONT_GetOneCharacterWidth(ONDKFontHandle fontHandle, HUINT8 *utf8pstr, HUINT32 *ret_pUsedBytes);
extern HINT32 				ONDK_FONT_DrawMultiLine(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr, HUINT32 vspace, ONDK_Color_t color);
extern HINT32 				ONDK_FONT_DrawMultiLineAlign(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr, HUINT32 vspace, ONDK_Color_t color, ONDKSurfaceTextFlags textflag);
extern HINT32 				ONDK_FONT_DrawMultiLineAlignRetPos(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr, HUINT32 vspace, ONDK_Color_t color, ONDKSurfaceTextFlags textflag, ONDK_Point_t *ret_pLastPoint);
extern HINT32	 			ONDK_FONT_GetLineCount(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT8 *pstr, HUINT32 vspace, ONDKSurfaceTextFlags textflag, HUINT32 *ret_out_height);
extern HINT32				ONDK_FONT_GetLineCountWithParseMultiLine(ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
										HUINT32 vspace,	HINT32 lMaxLine, ONDKSurfaceTextFlags textflag, HUINT32 *ret_out_height, FontMultiLine_Item_t *ret_arrItem);

extern HINT32	            ONDK_FONT_GetWidthMultiLine( ONDKFontHandle font, unsigned char *pstr, HINT32 max_w, HINT32 vspace, HINT32 *out_area_width, HINT32 *out_area_height);

extern void 				ONDK_FONT_StringDrawLUT(ONDKSurfaceHandle surface, ONDKFontHandle font, HCHAR *utf8pstr, HUINT32 x , HUINT32 y , HUINT8 color );

extern ONDKFontHandle		ONDK_FONT_CreateGreyFont( const HINT8 *path, HINT32 width, HINT32 height);
extern void					ONDK_FONT_ReleaseGreyFont(ONDKFontHandle fontHandle);
extern HINT32				ONDK_FONT_GetBitmapFont(ONDKFontHandle fontHandle, HCHAR *utf8pstr, HUINT32 w , HUINT32 h, HUINT8 *outBuffer);
extern HINT32				ONDK_FONT_DrawBitmapFont256Grey(ONDKSurfaceHandle surface, HUINT32 x, HUINT32 y, HUINT8 *fontBmp, HUINT32 font_w , HUINT32 font_h, ONDK_Color_t *color);
extern HINT32				ONDK_FONT_DrawBitmapFont4GreyLookupTable(ONDKSurfaceHandle surface, HUINT32 x, HUINT32 y, HUINT8 *fontBmp, HUINT32 font_w , HUINT32 font_h, HUINT8 *color_4grey);


extern HINT32 				ONDK_FONT_GetSize(ONDKFontHandle fontHandle);

extern HINT32 				ONDK_FONT_GetWidth(ONDKFontHandle fontHandle);
extern HINT32 				ONDK_FONT_GetHeight(ONDKFontHandle fontHandle);
extern HINT32 				ONDK_FONT_GetStringWidth(ONDKFontHandle fontHandle,HCHAR *pstr);

extern void 				DBG_ONDK_FONT_Dump(void *arg);

/*
 * gfx util
 */
extern 	ONDK_GFX_t			ONDK_DFBUTIL_GFX_Create(HUINT32 applId, ONDK_Rect_t rt);
extern	ONDK_GFX_t  		ONDK_DFBUTIL_GFX_CreateEx(HUINT32 applId, ONDK_Rect_t rt, HCHAR *pixelformat);

#if 0
/*
 * might remake...
 *
 */
    typedef enum {
    /* single byte container */
        eHxCP_ISO_ASCII=0,      /**< 7bit code, but storage is 8bit */
        eHxCP_ISO_SINGLEBYTE,   /**< 8bit code, plane text, almost same with 6937 */
        eHxCP_ISO_6937,
        eHxCP_ISO_8859, /**< not plane text, take character table code  */
        eHxCP_ISO_8859_1,
        eHxCP_ISO_8859_2,
        eHxCP_ISO_8859_3,
        eHxCP_ISO_8859_4,
        eHxCP_ISO_8859_5,
        eHxCP_ISO_8859_6,
        eHxCP_ISO_8859_7,
        eHxCP_ISO_8859_8,
        eHxCP_ISO_8859_9,
        eHxCP_ISO_8859_10,
        eHxCP_ISO_8859_11,
        eHxCP_ISO_8859_12,
        eHxCP_ISO_8859_13,
        eHxCP_ISO_8859_14,
        eHxCP_ISO_8859_15,
        eHxCP_ISO_300468,
    //  eHxCP_ISO_EUROPE300468,
        eHxCP_ISO_LG,   /**< ㅜㅜ, 보통은 ascii, icon은 UTF-16 */
        eHxCP_UNICODE_UTF8,
        eHxCP_UNICODE_DVB_UTF8,

    /* Area codepage */
        eHxCP_ISO_JAPAN_EUC,
        eHxCP_ISO_JAPAN_JIS,
        eHxCP_ISO_JAPAN_SJIS,

    /* JUST define, humax dont use */
        eHxCP_ISO_SIMPLEFIED_CHINESE,
        eHxCP_ISO_TRADITIONAL_CHINESE,
    /* 2 byte container */
        eHxCP_UNICODE_UCS2,
        eHxCP_UNICODE_DVB_UCS2,
    /* 4 byte container */
        eHxCP_ISO_UCS4,
    /* extra */
        eHxCP_ISO_EMFLASH,
        eHxCP_ISO_EMFLASH_UNI,

    /* Win CP */
        eHxCP_WIN_1251, //Russia 향에서 CP1251 (for Cyrillic) 에 대응하기 위해 추가

    /* ~ */
        eHCodepage_MAX
    } eHxCP_CodePage_e;
#endif



extern void 			 		ONDK_GWM_GFX_ClearScreenByRegion(ONDKSurfaceHandle pixmap, ONDK_Region_t *area);
extern ONDK_Result_t			ONDK_GWM_GFX_UpdateScreen(ONDK_WINDOW_Position_e ePosition,ONDK_Region_t Area);
extern ONDK_Color_t 			ONDK_GWM_GFX_GetSysDimmedAlpha(ONDKWindowHandle pstScreen, ONDK_Color_t srcColor, HUINT8 darkness);
extern ONDK_Color_t 			ONDK_GWM_GFX_GetSysDimmedColor(ONDKSurfaceHandle pstScreen, ONDK_Color_t srcColor, HINT32 level);
extern ONDK_Result_t			ONDK_GWM_GFX_SyncScreen(ONDKSurfaceHandle pstOsdScr, ONDK_Rect_t *rc);
extern void						ONDK_GWM_GFX_DrawFrame(ONDKWindowHandle pixmap, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HINT32 bright, HINT32 face, HINT32 dark);
extern HINT32					ONDK_GWM_GFX_GetStringWidth(ONDKFontHandle fontHandle, const HCHAR *string);
extern HINT32					ONDK_GWM_GFX_DrawMultiLineString(ONDKWindowHandle  pixmap, ONDKFontHandle fontHandle, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *string, HUINT32 vspace, ONDK_Color_t color);
extern HINT32					ONDK_GWM_GFX_CountMultiLine(ONDKFontHandle fontHandle, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *string, HINT32 vspace);
extern HINT32					ONDK_GWM_GFX_DrawBitmap(ONDKWindowHandle pixmap, HINT32 x, HINT32 y, HUINT8 *bmp_ptr, OSD_BmpBlendMode_t mode);

extern void						OSD_GFX_DrawRoundCornerFill(ONDKSurfaceHandle  pixmap, HINT32 x0, HINT32 y0, HINT32 radius, HINT32 corner, ONDK_Color_t color);
extern	void					OSD_GFX_DrawStringWithAbbr(ONDKWindowHandle  pixmap, ONDKFontHandle fontHandle, HINT32 x, HINT32 y, HINT32 w, HUINT8 *string, ONDK_Color_t color);

#endif





