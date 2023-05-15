/*
   @file aribcc_draw.c
   @brief MW ARIB CLOSE CAPTION AND SUPER  drawing
   @section copyright	Copyright(c) 2010, Humax Co., Ltd. All rights reserved.
   @section module MW/ARIBCC
   @author dhkim2
   @remark
   		SEN JC3500𵨿  Դ.
		FB  ִ palatte ⼭  ִٰ, surface ׸ ش.
		FONT 32Ʈ Surface Framebuffer Ѵ.
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

// �Ʒ��� extern ������ app_bml.c  �� ����Ǿ� �ִ�.
// �̰��� bml ������ ����� ���̸�,  app_bml.c ��  bml_gfxpeer ���� �������� ����ϱ� ���� ������ �̰����� ���� extern ���� �����Ͽ� ����Ѵ�.
// �ٸ� ������ �������� �ʱ� ���ؼ�  header ���Ͽ� �������� �ʴ´�.
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
* �׷��ȿ� ���� �ʱ�ȭ�� ����.
*
*******************************************************************************/
slim_int dtvGraphic_InitializePeer(void)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_InitializePeer() \n"));
/*		��� �ʱ�ȭ �ϰ�, ������ �� Surface ������ BML ��ü������ ������ ����. -
	ONDK_Init( argc, argv );
	gStillPlaneWnd = ONDK_WINDOW_Create(eAWINDOW_BML, 0, 0, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT, "ARGB8888", FALSE);
	gBMLPlaneWnd = ONDK_WINDOW_Create(eAWINDOW_BML, 0, 0, BML_SURFACE_WIDTH, BML_SURFACE_HEIGHT, "INDEX8", FALSE);
	gBMLSurface = ONDK_SURFACE_GetFromWindow(gBMLPlaneWnd);
*/
	// Font �� ����� ���´�.  ����μ��� MLIB ���� ���� ũ�⺰�� �����ؼ� ����ϴ� �� ���.
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
* �׷��� ���� ó���� ��.
*
*******************************************************************************/
void dtvGraphic_FinalizePeer(void)
{
	WAVEDTVGR_PRT(2, ("dtvGraphic_FinalizePeer() \n"));
/*		������ ���۸� ������ ������,  BML ��ü������ ������ ����. - �ƹ��͵� �� �� ����.
	MLIB_DestroyWindow(gStillPlaneWnd);
	MLIB_DestroyWindow(gBMLWnd);
*/
	return;
}


/*******************************************************************************
* function	dtvGraphic_GetCapsPeer
* input		None
* output	out_caps : �Ӽ��� ������ ����ü
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* �Ӽ� ���� (align)
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
* input		in_mode : Į�� ���
*			in_width : screen�� ��
*			in_height : screen�� ����
*			in_ratio : aspect ratio
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*	<�ػ� ����>
*	- FULL VIDEO : video�� �ػ󵵸� ���� ��, �� �Լ��� ȣ���� �ػ󵵿� ������ �־�� �ȵ�.
*	- PIG VIDEO : BML �ػ󵵰� �����. BML�� ���� PIG ������ dtvVideoStream_UpdatePeer()���� ��.
* 	- NONE VIDEO : video�� �����Ƿ� BML �ػ󵵰� �����.
*
*******************************************************************************/
slim_int dtvGraphic_SetViewModePeer(slim_int in_mode,
                           slim_int in_width,
                           slim_int in_height,
                           slim_int in_ratio)
{
	// ���� �������� ����.  �ʿ��Ѱ�..??
	// TODO:
	return	SLIM_E_DTV_GENERIC;
}


/*******************************************************************************
* function	dtvGraphic_SetCLUTPeer
* input		in_table : aYCbCr Į�� ����Ʈ
*			in_start : ������ index ���� ��ġ
*			in_end : ������ index ���� ��ġ
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* BML Plane�� YCbCr �����̶�� NetFront�κ��� �Ѿ�� in_table ���� �ٷ� �����ϸ� ������ ���� RGB
* �����̶�� conversion ó���� �ʿ���. (YCbCr2RGB)
*
*******************************************************************************/
slim_int dtvGraphic_SetCLUTPeer(slim_word* in_table,
                       slim_int in_start,
                       slim_int in_end)
{			// ���⼭�� �ȷ�Ʈ�� �迭�� �����ϱ⸸ ��.  �ٷ� ���������� ����.
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
{			//dtvGraphic_SetCLUTPeer() <-- ���⼭ ������ �ȷ�Ʈ�� �ѹ���  ������.
	ONDK_DRAW_SetColorLookupTable(gBMLSurface, (ONDK_Color_t *)s_nPalette, 0, PALETTE_MAX_NUM );
}


/*******************************************************************************
* function	dtvGraphic_DrawTextPeer
* input		in_x : ��ȭ ���� x ��ǥ
*			in_y : ��ȭ ���� y ��ǥ
*			in_text : ��ȭ�� ���ڿ� ������ (EUC-JP)
*			in_len : ���ڿ� ����
*			in_color : ���ڻ� (4���� ��Ʈ)
* 			in_font_family : ��Ʈ ���� (�ձ� ���, �� ���, ���� �ձ� ���)
* 			in_font_size : ��Ʈ ũ��
*			in_letter_spacing : �ڰ�
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* browserApp���� BML stop�� �޽����� ���� NetFront�� �����ϹǷ� sync ������ ���� �� �ִ�.
* browserApp���� BML stop�� s_nGraphicSem�� take �����ν� ��� drawing �Ǵ� ���� ���´�.
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
	while( (*eucjp != '\0') || (in_len > 0) )		// �ѱ��� �ѱ��ڸ� utf8 �� ��ȯ�� ���鼭 draw �� ������.
	{
		unicode = jis2uni( eucjp[0], eucjp[1] );
		used_bytes = uni2utf8( unicode, &utf8_str[0], &utf8_str[1], &utf8_str[2] );
		utf8_str[3] = '\0';
		eucjp += used_bytes;	in_len -= used_bytes;	// ����Ʈ �� ��ŭ ���δ�.

		ONDK_FONT_GetBitmapFont( fntptr, utf8_str, fntptr->w, fntptr->h, bmpbuffer);
		AribCCDraw_DrawBitmapFont(surface, x, y, fntptr->w, fntptr->h, bmpbuffer, attrib, color);
	}
*/

	return SLIM_E_DTV_OK;
}


/*******************************************************************************
* function	dtvGraphic_FillRectPeer
* input		in_rect : ä�� ����
*			in_color : ä�� �� �ε��� ��
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
* input		in_rect : �����ϰ� ä�� ����
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
* input		in_rect : ������Ʈ�� ����
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* ���� ȭ�鿡 �׸� BML Plane �׷��� �����͸� ���� OSD�� ������.
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
* input		in_x : destination x ��ǥ
*			in_y : destination y ��ǥ
* 			in_rect : source ��ǥ
*			in_pixmap : source ������
*			in_pixmap_width : source �������� ��
*			in_pixmap_height : source �������� ����
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
* input		in_status : video ���¸� ����
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
* input		in_plane : ��� �÷���
*			in_rect : �κ��÷�����ǥ
*			in_num : �κ��÷��� ��(in_rect �迭 ��)
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* VIDEO + in_num (0) : ��ȭ�� ��ȭ
* STILL + in_num (0) : ��ȭ�� ����ȭ
* STILL + in_num (1) : PIG
* VIDEO + in_num (n) : ��ȭȭ�鿡 n ������ rectangle(����ȭ��)�� ����.
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
* input	 	in_dest_x : destination x ��ǥ
*			in_dest_y : destination y ��ǥ
*			in_rect : source �ȼ������� ��ǥ
*			in_pixmap : �ȼ�������
* 			in_pixmap_width : �ȼ������� ��
*			in_pixmap_height : �ȼ������� ����
*			in_rowbytes : 1���� ����Ʈ ��
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
* input	 	in_dest_x : destination x ��ǥ
*			in_dest_y : destination y ��ǥ
*			in_rect : source �ȼ������� ��ǥ
*			in_pixmap : �ȼ�������
* 			in_pixmap_width : �ȼ������� ��
*			in_pixmap_height : �ȼ������� ����
*			in_rowbytes : 1���� ����Ʈ ��
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

	in_pixelmap = (HUINT32*)ptr;		// �ӽ÷�. �׽�Ʈ ����. 

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
* input	 	in_rect : �� ä�� ��ǥ
*			in_color : ��
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
* input	 	in_status : ��ȭ ǥ�� ����
*			in_rect : ��ȭ����
*			in_component : component_tag
*			in_mime : ��ȭ Ÿ��
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
* input	 	in_data : ���� ���� ������ ������
*��			in_data_len : ���� ���� ������ ����
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
* input	 	in_data : 8���� ���� ��ȣ ������
*			in_data_len : 8���� ���� ��ȣ ������ ����
*			in_width : 8���� ���ں�ȣ�� ǥ���ϴ� ���� (��)
*			in_height : 8���� ���ں�ȣ�� ǥ���ϴ� ������ ����
* output	None
* return	>= 0 8���� ���� ��ȣ�� ��ũ����.
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
* input	 	in_id : ��ũ����
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
* input	 	in_x : x��ǥ
*			in_y : y��ǥ
*			in_clip_rect : Ŭ���� ����
*			in_id : descriptor
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* drawJIS8() �� ACCESS�κ��� ���� �ҽ� ��.
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




