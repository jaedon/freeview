/*
   @file aribcc_draw.c
   @brief MW ARIB CLOSE CAPTION AND SUPER  drawing
   @section copyright	Copyright(c) 2010, Humax Co., Ltd. All rights reserved.
   @section module MW/ARIBCC
   @author dhkim2
   @remark
   		SEN JC3500ðµ¨¿  Ô´.
		FB  Ö´ palatte â¼­  Ö´Ù°, surface ×¸ Ø´.
		FONT 32Æ® Surface Framebuffer Ñ´.
*/


#include <stdio.h>
//#include <util_png.h>
#include "directfb.h"
#include "ondk.h"

#include "bml_gfxpeer.h"

#define DFBCHECK(x...)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal( #x, err );                         \
               }
#define	WAVEDTVGR_PRT(x, y)
#define	EMPTY_PRT(x, y)


#define	PALETTE_MAX_NUM	255

static		ONDK_Color_t	s_nPalette[PALETTE_MAX_NUM];
static		ONDKFontHandle	gBmlFont16, gBmlFont20, gBmlFont24, gBmlFont30, gBmlFont36;

// ¾Æ·¡ÀÇ extern º¯¼ö´Â app_bml.c  ¿¡ ¼±¾ðµÇ¾î ÀÖ´Ù.
// ÀÌ°ÍÀº bml ¿¡¼­¸¸ »ç¿ëÇÒ °ÍÀÌ¸ç,  app_bml.c ¿Í  bml_gfxpeer ¿¡¼­ °ø¿ëÀ¸·Î »ç¿ëÇÏ±â À§ÇÑ °ÍÀ¸·Î ÀÌ°÷¿¡¼­ Á÷Á¢ extern À¸·Î ¼±¾ðÇÏ¿© »ç¿ëÇÑ´Ù.
// ´Ù¸¥ °÷À¸·Î °ø°³ÇÏÁö ¾Ê±â À§ÇØ¼­  header ÆÄÀÏ¿¡ Á¤ÀÇÇÏÁö ¾Ê´Â´Ù.
extern	ONDKWindowHandle	gBMLPlaneWnd, gStillPlaneWnd;
extern	ONDKSurfaceHandle	gBMLSurface, gStillSurface;



#define ___PEER_APIs___


/*******************************************************************************
* function	dtvGraphic_InitializePeer
* input		None
* output	None
* return	SLIM_E_DTV_OK Succeeded
* 			SLIM_E_DTV_GENERIC Not succeeded
*			SLIM_E_DTV_NOTSUPPORTED
*
* description
*
* ±×·¡ÇÈ¿¡ °üÇÑ ÃÊ±âÈ­¸¦ ÇàÇÔ.
*
*******************************************************************************/
slim_int dtvGraphic_InitializePeer(void)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_InitializePeer() \n"));
/*		¸ðµâ ÃÊ±âÈ­ ÇÏ°í, À©µµ¿ì ¹× Surface »ý¼ºÀº BML ÀÚÃ¼ÀûÀ¸·Î ½ÇÇàÇÒ °ÍÀÓ. -
	ONDK_Init( argc, argv );
	gStillPlaneWnd = ONDK_WINDOW_Create(eAWINDOW_BML, 0, 0, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT, "ARGB8888", FALSE);
	gBMLPlaneWnd = ONDK_WINDOW_Create(eAWINDOW_BML, 0, 0, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT, "INDEX8", FALSE);
	gBMLSurface = ONDK_SURFACE_GetFromWindow(gBMLPlaneWnd);
*/
	// Font ¸¦ ¸¸µé¾î ³õ´Â´Ù.  ÇöÀç·Î¼­´Â MLIB ¿¡¼­ ±ÛÀÚ Å©±âº°·Î ¼³Á¤ÇØ¼­ »ç¿ëÇÏ´Â °Ô ¾ø¾î¼­.
	gBmlFont16 = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 16, 16);
	gBmlFont20 = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 20, 20);
	gBmlFont24 = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 24, 24);
	gBmlFont30 = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 30, 30);
	gBmlFont36 = ONDK_FONT_CreateGreyFont( JAPANESE_FONT_PATH, 36, 36);

	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_InitializePeer
* input		None
* output	None
* return	None
*
* description
*
* ±×·¡ÇÈ Á¾·á Ã³¸®¸¦ ÇÔ.
*
*******************************************************************************/
void dtvGraphic_FinalizePeer(void)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_FinalizePeer() \n"));
/*		»ý¼ºÇÑ ¹öÆÛ¸¦ Áö¿ö¾ß ÇÏÁö¸¸,  BML ÀÚÃ¼ÀûÀ¸·Î ½ÇÇàÇÒ °ÍÀÓ. - ¾Æ¹«°Íµµ ÇÒ ÀÏ ¾ø´Ù.
	MLIB_DestroyWindow(gStillPlaneWnd);
	MLIB_DestroyWindow(gBMLWnd);
*/
	return;
}


/*******************************************************************************
* function	dtvGraphic_GetCapsPeer
* input		None
* output	out_caps : ¼Ó¼ºÀ» ´ëÀÔÇÒ ±¸Á¶Ã¼
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* ¼Ó¼º ¼³Á¤ (align)
*
*******************************************************************************/
slim_int dtvGraphic_GetCapsPeer(TDTVGraphicCaps *out_caps)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_GetCapsPeer() \n"));

	if(out_caps == NULL)
	{
		EMPTY_PRT(0, ("dtvGrphic_GetCapsPeer() illegal parameter.\n"));
		return	SLIM_E_DTV_GENERIC;
	}
	out_caps->fPixmapAlign = 4;
	out_caps->fBitmapAlign = 4;
	out_caps->fYUVImageAlign = 32;

	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_SetViewModePeer
* input		in_mode : Ä®¶ó ¸ðµå
*			in_width : screenÀÇ Æø
*			in_height : screenÀÇ ³ôÀÌ
*			in_ratio : aspect ratio
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*	<ÇØ»óµµ Àû¿ë>
*	- FULL VIDEO : videoÀÇ ÇØ»óµµ¸¦ Àû¿ë ÇÔ, ÀÌ ÇÔ¼öÀÇ È£ÃâÀÌ ÇØ»óµµ¿¡ ¿µÇâÀ» ÁÖ¾î¼­´Â ¾ÈµÊ.
*	- PIG VIDEO : BML ÇØ»óµµ°¡ Àû¿ëµÊ. BML¿¡ ÀÇÇÑ PIG Àû¿ëÀº dtvVideoStream_UpdatePeer()¿¡¼­ ÇÔ.
* 	- NONE VIDEO : video°¡ ¾øÀ¸¹Ç·Î BML ÇØ»óµµ°¡ Àû¿ëµÊ.
*
*******************************************************************************/
slim_int dtvGraphic_SetViewModePeer(slim_int in_mode,
                           slim_int in_width,
                           slim_int in_height,
                           slim_int in_ratio)
{
	// ¾ÆÁ÷ ±¸ÇöÇÏÁö ¾ÊÀ½.  ÇÊ¿äÇÑ°¡..??
	// TODO:
	return	SLIM_E_DTV_GENERIC;
}


/*******************************************************************************
* function	dtvGraphic_SetCLUTPeer
* input		in_table : aYCbCr Ä®¶ó »¡·¹Æ®
*			in_start : ¼³Á¤ÇÒ index °³½Ã À§Ä¡
*			in_end : ¼³Á¤ÇÒ index Á¾·á À§Ä¡
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* BML PlaneÀÌ YCbCr Æ÷¸äÀÌ¶ó¸é NetFront·ÎºÎÅÍ ³Ñ¾î¿Â in_table °ªÀ» ¹Ù·Î Àû¿ëÇÏ¸é µÇÁö¸¸ ¸¸¾à RGB
* Æ÷¸äÀÌ¶ó¸é conversion Ã³¸®°¡ ÇÊ¿äÇÔ. (YCbCr2RGB)
*
*******************************************************************************/
slim_int dtvGraphic_SetCLUTPeer(slim_word* in_table,
                       slim_int in_start,
                       slim_int in_end)
{			// ¿©±â¼­´Â ÆÈ·¹Æ®¸¦ ¹è¿­¿¡ ÀúÀåÇÏ±â¸¸ ÇÔ.  ¹Ù·Î Àû¿ëÇÏÁö´Â ¾ÊÀ½.
	HINT32	i, num = 0;

	WAVEDTVGR_PRT(2, ("dtvGraphic_SetCLUTPeer() start=%d, end=%d\n", in_start, in_end));

	if(in_table == NULL || in_start < 0 || in_end > 223)
	{
		EMPTY_PRT(0, ("dtvGraphic_SetCLUTPeer() illegal parameter.\n"));
		return	SLIM_E_DTV_ILLEGALPARAMETER;
	}

	for (i = in_start; i<= in_end; i++)
	{
		s_nPalette[i] = (ONDK_Color_t) in_table[num];
		num++;
	}

	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_ExposeCLUTPeer
* input		None
* output	None
* return	None
*
* description
*
*******************************************************************************/
void	dtvGraphic_ExposeCLUTPeer(void)
{			//dtvGraphic_SetCLUTPeer() <-- ¿©±â¼­ ¼³Á¤ÇÑ ÆÈ·¹Æ®¸¦ ÇÑ¹ø¿¡  Àû¿ëÇÔ.
	ONDK_DRAW_SetColorLookupTable(gBMLSurface, (ONDK_Color_t *)s_nPalette, 0, PALETTE_MAX_NUM );
}


/*******************************************************************************
* function	dtvGraphic_DrawTextPeer
* input		in_x : ¹¦È­ °³½Ã x ÁÂÇ¥
*			in_y : ¹¦È­ °³½Ã y ÁÂÇ¥
*			in_text : ¹¦È­ÇÒ ¹®ÀÚ¿­ Æ÷ÀÎÅÍ (EUC-JP)
*			in_len : ¹®ÀÚ¿­ ±æÀÌ
*			in_color : ¹®ÀÚ»ö (4°èÁ¶ ÆùÆ®)
* 			in_font_family : ÆùÆ® Á¾·ù (µÕ±Ù °íµñ, °¢ °íµñ, ±½Àº µÕ±Ù °íµñ)
* 			in_font_size : ÆùÆ® Å©±â
*			in_letter_spacing : ÀÚ°£
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* browserApp¿¡¼­ BML stopÀ» ¸Þ½ÃÁö¸¦ ÅëÇØ NetFront¿¡ ÅëÁöÇÏ¹Ç·Î sync ¹®Á¦°¡ ÀÖÀ» ¼ö ÀÖ´Ù.
* browserApp¿¡¼­ BML stop½Ã s_nGraphicSem¸¦ take ÇÔÀ¸·Î½á °è¼Ó drawing µÇ´Â °ÍÀ» ¸·´Â´Ù.
*
*******************************************************************************/
slim_int	dtvGraphic_DrawTextPeer(slim_int in_x,
                        slim_int in_y,
                        slim_char* in_text,
                        slim_int in_len,
                        ONDK_Color_t* in_colors,
                        slim_int in_font_family,
                        slim_int in_font_size,
                        slim_int in_font_style,
                        ONDK_Rect_t* in_clip_rect,
                        slim_int in_letter_spacing)
{
	ONDKFontHandle 	fonthandle;
	HUINT8	utf8_str[12], *eucjp;
	HUINT16	unicode;
	int		used_bytes;
	ONDK_Rect_t	_rc;
	HUINT8	bmpbuffer[72*72];
	HUINT8	color[4];

	WAVEDTVGR_PRT(2, ("dtvGraphic_DrawTextPeer()\n"));

	if(((in_text == NULL) || (in_colors == NULL) || (in_clip_rect == NULL)) ||
		((in_clip_rect->x == 0) && (in_clip_rect->y == 0) && (in_clip_rect->h == 0) && (in_clip_rect->w == 0)))
	{
		EMPTY_PRT(0, ("dtvGraphic_DrawTextPeer() illegal parameter.\n"));
		return	SLIM_E_DTV_ILLEGALPARAMETER;
	}

	EMPTY_PRT(4, ("(%3d,%3d),(%3d,%3d,%3d,%3d),color=0x%x,family=%d,style=%d,len=%d,size=%3d\n",
		in_x, in_y, in_clip_rect->x, in_clip_rect->y,in_clip_rect->x+in_clip_rect->w,
		in_clip_rect->y+in_clip_rect->h, *in_colors, in_font_family, in_font_style, in_len, in_font_size));

	switch(in_font_size)
	{
		case 16:
			fonthandle = &gBmlFont16;
			break;
		case 24:
			fonthandle = &gBmlFont24;
			break;
		case 30:
			fonthandle = &gBmlFont30;
			break;
		case 36:
			fonthandle = &gBmlFont36;
			break;
		case 20:
		default:
			fonthandle = &gBmlFont20;
			break;
	}

	eucjp = in_text;
	memset(utf8_str, 0, 12);
/*
	while( (*eucjp != '\0') || (in_len > 0) )		// ÇÑ±ÛÀÚ ÇÑ±ÛÀÚ¸¦ utf8 ·Î º¯È¯ÇØ °¡¸é¼­ draw ÇØ ³ª°£´Ù.
	{
		unicode = jis2uni( eucjp[0], eucjp[1] );
		used_bytes = uni2utf8( unicode, &utf8_str[0], &utf8_str[1], &utf8_str[2] );
		utf8_str[3] = '\0';
		eucjp += used_bytes;	in_len -= used_bytes;	// ¹ÙÀÌÆ® ¼ö ¸¸Å­ ÁÙÀÎ´Ù.

		ONDK_FONT_GetBitmapFont( fntptr, utf8_str, fntptr->w, fntptr->h, bmpbuffer);
		AribCCDraw_DrawBitmapFont(surface, x, y, fntptr->w, fntptr->h, bmpbuffer, attrib, color);
	}
*/

	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_FillRectPeer
* input		in_rect : Ã¤¿ï ¿µ¿ª
*			in_color : Ã¤¿ï »ö ÀÎµ¦½º °ª
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
*******************************************************************************/
slim_int	dtvGraphic_FillRectPeer(ONDK_Rect_t* in_rect,
                        				HUINT8	in_color)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_FillRectPeer()\n"));

	ONDK_DRAW_FillRectLUT(gBMLSurface, *in_rect, in_color);
	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_FillTransparentRectPeer
* input		in_rect : Åõ¸íÇÏ°Ô Ã¤¿ï ¿µ¿ª
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvGraphic_FillTransparentRectPeer(ONDK_Rect_t* in_rect)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_FillTransparentRectPeer()\n"));

	ONDK_DRAW_FillRectLUT(gBMLSurface, *in_rect, 0x08 );
	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_ExposeRectPeer
* input		in_rect : ¾÷µ¥ÀÌÆ®ÇÒ ¿µ¿ª
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* °¡»ó È­¸é¿¡ ±×¸° BML Plane ±×·¡ÇÈ µ¥ÀÌÅÍ¸¦ ½ÇÁ¦ OSD¿¡ Àû¿ëÇÔ.
*
*******************************************************************************/
slim_int	dtvGraphic_ExposeRectPeer(ONDK_Rect_t* in_rect)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_ExposeRectPeer()\n"));

	ONDK_WINDOW_Update(gBMLPlaneWnd, in_rect);
	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_BitBltPeer
* input		in_x : destination x ÁÂÇ¥
*			in_y : destination y ÁÂÇ¥
* 			in_rect : source ÁÂÇ¥
*			in_pixmap : source µ¥ÀÌÅÍ
*			in_pixmap_width : source µ¥ÀÌÅÍÀÇ Æø
*			in_pixmap_height : source µ¥ÀÌÅÍÀÇ ³ôÀÌ
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvGraphic_BitBltPeer(slim_int in_x,
                      slim_int in_y,
                      ONDK_Rect_t* in_rect,
                      slim_byte* in_pixmap,
                      slim_int in_pixmap_width,
                      slim_int in_pixmap_height)
{
	DFBResult			err;
	IDirectFBSurface	*dfbSurface;
	HUINT8			*ptr, *tptr_src, *tptr_dst;
	int				pitch, i, j, w, h;

	WAVEDTVGR_PRT(2, ("dtvGraphic_BitBltPeer( )\n"));

	w = in_rect->w;
	h = in_rect->h;
	dfbSurface = (IDirectFBSurface*)gBMLSurface;
	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_READ|DSLF_WRITE, (void**)&ptr, &pitch));
				//HUINT32*	ONDK_SURFACE_Lock(ONDKSurfaceHandle _surface)

	for (i=0; i<h; i++)
	{	tptr_dst = ptr+(i+in_y)*pitch+in_x;
		tptr_src = in_pixmap+(i+in_rect->y)*in_pixmap_width+in_rect->x;
		for (j=0; j<w; j++)
		{	*tptr_dst = *tptr_src;
			tptr_dst++;
			tptr_src++;
		}
	}
	DFBCHECK(dfbSurface->Unlock (dfbSurface));
				//void	ONDK_SURFACE_Unlock(ONDKSurfaceHandle _surface)

	ONDK_WINDOW_Update(gBMLPlaneWnd, in_rect);
	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_NotifyVideoStatusPeer
* input		in_status : video »óÅÂ¸¦ ÅëÁö
* output	None
* return	None
*
* description
*
*******************************************************************************/
void	dtvGraphic_NotifyVideoStatusPeer(slim_int in_status)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_NotifyVideoStatusPeer() status=%d\n", in_status));
#if 0
	switch(in_status)
	{
		case SLIM_DTV_VIDEOSTATUS_VIDEONONE:
			break;

		case SLIM_DTV_VIDEOSTATUS_VIDEOONLY:
		case SLIM_DTV_VIDEOSTATUS_INVISIBLE:
			/* invisible attribute is set - Original video resolution & Restore video origin */
			GWM_SendMessageTo(GWM_GetApplication(browserApp), BROW_EVT_NOTIFY_VIDEOSTATUS, VIDEO_INVISIBLE, 0, 0);
			break;

		case SLIM_DTV_VIDEOSTATUS_VIDEOCOMPOSED:
			GWM_SendMessageTo(GWM_GetApplication(browserApp), BROW_EVT_NOTIFY_VIDEOSTATUS, VIDEO_COMPOSED, 0, 0);
			break;

		default:
			break;
	}
#endif
}


/*******************************************************************************
* function	dtvGraphic_BeginUpdatePeer
* input		None
* output	None
* return	None
*
* description
*
*******************************************************************************/
void	dtvGraphic_BeginUpdatePeer(void)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_BeginUpdatePeer()\n"));
}


/*******************************************************************************
* function	dtvGraphic_EndUpdatePeer
* input		None
* output	None
* return	None
*
* description
*
*******************************************************************************/
void	dtvGraphic_EndUpdatePeer(void)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_EndUpdatePeer() \n"));
}


/*******************************************************************************
* function	dtvSwitchingPlane_SetPeer
* input		in_plane : ¹è°æ ÇÃ·¹ÀÎ
*			in_rect : ºÎºÐÇÃ·¹ÀÎÁÂÇ¥
*			in_num : ºÎºÐÇÃ·¹ÀÎ ¼ö(in_rect ¹è¿­ ¼ö)
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* VIDEO + in_num (0) : ÀüÈ­¸é µ¿È­
* STILL + in_num (0) : ÀüÈ­¸é Á¤ÁöÈ­
* STILL + in_num (1) : PIG
* VIDEO + in_num (n) : µ¿È­È­¸é¿¡ n °³¼öÀÇ rectangle(Á¤ÁöÈ­¸é)ÀÌ »ý±è.
*
*******************************************************************************/
slim_int	dtvSwitchingPlane_SetPeer(slim_int in_plane,
				                          ONDK_Rect_t* in_rect,
				                          slim_int in_num)
{
	return	SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvStillPlane_BitBltPeer
* input	 	in_dest_x : destination x ÁÂÇ¥
*			in_dest_y : destination y ÁÂÇ¥
*			in_rect : source ÇÈ¼¿µ¥ÀÌÅÍ ÁÂÇ¥
*			in_pixmap : ÇÈ¼¿µ¥ÀÌÅÍ
* 			in_pixmap_width : ÇÈ¼¿µ¥ÀÌÅÍ Æø
*			in_pixmap_height : ÇÈ¼¿µ¥ÀÌÅÍ ³ôÀÌ
*			in_rowbytes : 1ÇàÀÇ ¹ÙÀÌÆ® ¼ö
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvStillPlane_BitBltPeer(slim_int in_dest_x, slim_int in_dest_y,
                         ONDK_Rect_t* in_rect,
                         slim_byte *in_pixelmap,	/* RGB */
                         slim_int in_pixelmap_width,
                         slim_int in_pixelmap_height,
                         slim_int in_rowBytes)
{
	DFBResult			err;
	IDirectFBSurface	*dfbSurface;
	HUINT32			*ptr, *tptr_src, *tptr_dst;
	int				pitch, i, j, w, h;

	WAVEDTVGR_PRT(2, ("dtvStillPlane_BitBltPeer( )\n"));

	w = in_rect->w;
	h = in_rect->h;
	dfbSurface = (IDirectFBSurface*)gStillSurface;
	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_READ|DSLF_WRITE, (void**)&ptr, &pitch));
				//HUINT32*	ONDK_SURFACE_Lock(ONDKSurfaceHandle _surface)
	pitch /= 4;		// Copying HUINT32.

	for (i=0; i<h; i++)
	{
		tptr_dst = ptr+(i+in_dest_y)*pitch+in_dest_x;
		tptr_src = in_pixelmap+(i+in_rect->y)*in_rowBytes+in_rect->x;
		for (j=0; j<w; j++)
		{
			*tptr_dst = *tptr_src;
			tptr_dst++;
			tptr_src++;
		}
	}
	DFBCHECK(dfbSurface->Unlock (dfbSurface));
				//void	ONDK_SURFACE_Unlock(ONDKSurfaceHandle _surface)

	ONDK_WINDOW_Update(gStillPlaneWnd, in_rect);
	return SLIM_E_DTV_OK;
}

/*******************************************************************************
* function	dtvStillPlane_BitQuaterBltPeer
* input	 	in_dest_x : destination x ÁÂÇ¥
*			in_dest_y : destination y ÁÂÇ¥
*			in_rect : source ÇÈ¼¿µ¥ÀÌÅÍ ÁÂÇ¥
*			in_pixmap : ÇÈ¼¿µ¥ÀÌÅÍ
* 			in_pixmap_width : ÇÈ¼¿µ¥ÀÌÅÍ Æø
*			in_pixmap_height : ÇÈ¼¿µ¥ÀÌÅÍ ³ôÀÌ
*			in_rowbytes : 1ÇàÀÇ ¹ÙÀÌÆ® ¼ö
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvStillPlane_BitQuaterBltPeer(slim_int in_dest_x, slim_int in_dest_y,
						 ONDK_Rect_t* in_rect,
						 slim_byte *in_pixelmap,	/* RGB */
						 slim_int in_pixelmap_width,
						 slim_int in_pixelmap_height,
						 slim_int in_rowBytes)
{
	DFBResult			err;
	IDirectFBSurface	*dfbSurface;
	HUINT32 		*ptr, *tptr_src, *tptr_dst;
	int 				pitch, i, j, w, h;
	ONDK_Rect_t		update_area;

	WAVEDTVGR_PRT(2, ("dtvStillPlane_BitQuaterBltPeer( )\n"));

	w = in_rect->w /2 ;
	h = in_rect->h /2;
	dfbSurface = (IDirectFBSurface*)gStillSurface;
	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_READ|DSLF_WRITE, (void**)&ptr, &pitch));
				//HUINT32*	ONDK_SURFACE_Lock(ONDKSurfaceHandle _surface)
	pitch /= 4; 	// Copying HUINT32.

	in_pixelmap = (HUINT32*)ptr;		// ÀÓ½Ã·Î. Å×½ºÆ® ¸ñÀû. 

	for (i=0; i<h; i++)
	{
		tptr_dst = ptr+(i+in_dest_y)*pitch+in_dest_x;
		tptr_src = (HUINT32*)(in_pixelmap+(i+in_rect->y)*2 * in_rowBytes + in_rect->x*2 );
		for (j=0; j<w; j++)
		{
			*tptr_dst = *tptr_src;
			tptr_dst++;
			tptr_src +=2;
		}
	}
	DFBCHECK(dfbSurface->Unlock (dfbSurface));
				//void	ONDK_SURFACE_Unlock(ONDKSurfaceHandle _surface)

	update_area.x = in_dest_x;	
	update_area.y = in_dest_y;	
	update_area.w = w;	
	update_area.h = h;
	ONDK_WINDOW_Update(gStillPlaneWnd, &update_area);
	return SLIM_E_DTV_OK;
}



/*******************************************************************************
* function	dtvStillPlane_FillRectPeer
* input	 	in_rect : »ö Ã¤¿ï ÁÂÇ¥
*			in_color : »ö
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvStillPlane_FillRectPeer(ONDK_Rect_t* in_rect,
                         	  slim_word in_color)	/*ARGB*/
{
	WAVEDTVGR_PRT(2, ("dtvStillPlane_FillRectPeer( )\n"));

	ONDK_DRAW_FillRect(gStillSurface, *in_rect, (ONDK_Color_t)in_color);
	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvVideoStream_UpdatePeer
* input	 	in_status : µ¿È­ Ç¥½Ã ¿©ºÎ
*			in_rect : µ¿È­¿µ¿ª
*			in_component : component_tag
*			in_mime : µ¿È­ Å¸ÀÔ
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvVideoStream_UpdatePeer(slim_int in_status,
											ONDK_Rect_t* in_rect,
                        					slim_int in_component,
                        					slim_int in_mime)
{
    return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvDRCS_LoadPeer
* input	 	in_data : ¿ÜÀÚ Á¤ÀÇ µ¥ÀÌÅÍ Æ÷ÀÎÅÍ
*¤²			in_data_len : ¿ÜÀÚ Á¤ÀÇ µ¥ÀÌÅÍ ±æÀÌ
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvDRCS_LoadPeer(slim_byte *in_data,
                 					slim_word in_data_len)
{
    return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvDRCS_UnloadPeer
* input	 	None
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtvDRCS_UnloadPeer(void)
{
    return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtv8Text_CreatePeer
* input	 	in_data : 8´ÜÀ§ ¹®ÀÚ ºÎÈ£ µ¥ÀÌÅÍ
*			in_data_len : 8´ÜÀ§ ¹®ÀÚ ºÎÈ£ µ¥ÀÌÅÍ ±æÀÌ
*			in_width : 8´ÜÀ§ ¹®ÀÚºÎÈ£¸¦ Ç¥½ÃÇÏ´Â ¿µ¿ª (Æø)
*			in_height : 8´ÜÀ§ ¹®ÀÚºÎÈ£¸¦ Ç¥½ÃÇÏ´Â ¿µ¿ªÀÇ ³ôÀÌ
* output	None
* return	>= 0 8´ÜÀ§ ¹®ÀÚ ºÎÈ£ÀÇ µð½ºÅ©¸³ÅÍ.
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtv8Text_CreatePeer(slim_byte *in_data, slim_int in_data_len,
                    slim_int in_width, slim_int in_height)
{
    return 0;
}


/*******************************************************************************
* function	dtv8Text_DisposePeer
* input	 	in_id : µð½ºÅ©¸³ÅÍ
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
slim_int	dtv8Text_DisposePeer(slim_int in_id)
{
    return  0;
}


/*******************************************************************************
* function	dtv8Text_DrawPeer
* input	 	in_x : xÁÂÇ¥
*			in_y : yÁÂÇ¥
*			in_clip_rect : Å¬¸®ÇÎ ¿µ¿ª
*			in_id : descriptor
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* drawJIS8() Àº ACCESS·ÎºÎÅÍ ¹ÞÀº ¼Ò½º ÀÓ.
*
*******************************************************************************/
slim_int	dtv8Text_DrawPeer(slim_int in_x, slim_int in_y,
                  ONDK_Rect_t* in_clip_rect,
                  slim_int in_id)
{
    return  0;
}

#define ___DEBUG_APIs___




/* ################################################### */

void	test_drawfont(void)
{
#if 0
	HUINT8	text[30] = "ABCD_abcd_012345";
	ONDK_Rect_t rect;
	ONDK_Color_t color;

{
	EG_S8 text_bold[30] = "ABCD_abcd(bold)";
	EG_S8 text_normal[30] = "ABCD_abcd(normal)";

/* Bold : Normal font test */
	color = 0xFF070707;

	rect.fX = 150;
	rect.fY = 150;
	rect.fWidth = 324;
	rect.fHeight = 24;

	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, text_bold, strlen(text_bold), &color, 1, 24, 2, &rect, 0);

	rect.fX = 400;
	rect.fY = 150;
	rect.fWidth = 324;
	rect.fHeight = 24;

	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, text_normal, strlen(text_normal), &color, 1, 24, 0, &rect, 0);

	rect.fX = 150;
	rect.fY = 180;
	rect.fWidth = 324;
	rect.fHeight = 30;

	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, text_bold, strlen(text_bold), &color, 1, 30, 2, &rect, 0);

	rect.fX = 400;
	rect.fY = 180;
	rect.fWidth = 324;
	rect.fHeight = 30;

	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, text_normal, strlen(text_normal), &color, 1, 30, 0, &rect, 0);

	rect.fX = 150;
	rect.fY = 220;
	rect.fWidth = 324;
	rect.fHeight = 36;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, text_bold, strlen(text_bold), &color, 1, 36, 2, &rect, 0);

	rect.fX = 400;
	rect.fY = 220;
	rect.fWidth = 324;
	rect.fHeight = 36;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, text_normal, strlen(text_normal), &color, 1, 36, 0, &rect, 0);

#if 0
	rect.fX = 150;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 3;
	rect.fY = 300;
	rect.fWidth = 2;
	rect.fHeight = 2;
	dtvGraphic_FillRectPeer(&rect, color);
#else
	rect.fX = 150;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);

	rect.fX = rect.fX + 2;
	rect.fY = 300;
	rect.fWidth = 1;
	rect.fHeight = 1;
	dtvGraphic_FillRectPeer(&rect, color);
#endif
}

	color = 0xFF070707;

	/* KAKU GOTHIC : MARU GOTHIC font test */
	rect.fX = 400;
	rect.fY = 200;
	rect.fWidth = 400;
	rect.fHeight = 40;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, (slim_char*)text, strlen(text), &color, 1, 36, 0, &rect, 0);

	rect.fX = 400;
	rect.fY = 250;
	rect.fWidth = 400;
	rect.fHeight = 40;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, (slim_char*)text, strlen(text), &color, 2, 36, 0, &rect, 0);

	rect.fX = 400;
	rect.fY = 300;
	rect.fWidth = 400;
	rect.fHeight = 40;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, (slim_char*)text, strlen(text), &color, 2, 36, 2, &rect, 0);

	rect.fX = 400;
	rect.fY = 350;
	rect.fWidth = 400;
	rect.fHeight = 40;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, (slim_char*)text, strlen(text), &color, 3, 36, 0, &rect, 0);

	rect.fX = 400;
	rect.fY = 400;
	rect.fWidth = 400;
	rect.fHeight = 40;
	dtvGraphic_DrawTextPeer(rect.fX, rect.fY, (slim_char*)text, strlen(text), &color, 3, 36, 2, &rect, 0);

	rect.fX = 0;
	rect.fY = 0;
	rect.fWidth = 960;
	rect.fHeight = 540;

	dtvGraphic_ExposeRectPeer(&rect);

#endif

}




