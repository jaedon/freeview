/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include 	"ondk_dfb.h"
#include 	"ondk_gfx.h"



/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


/*  Circle and Rounded corner rectangle drawing... */
typedef void	 (*CIRCLE_CALLBACK_t)(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color);


static  CIRCLE_CALLBACK_t	 s_circle_callback;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/



static  void	 circleSubRoutine(ONDKSurfaceHandle surface, int x0, int y0, int radius, ONDK_Color_t color)
{
	int x, y, thres;

	/*  if ((x0 + radius) < surface->clip_sx)
	return;
	if ((x0 - radius) >= surface->clip_ex)
	return;
	if ((y0 + radius) < surface->clip_sy)
	return;
	if ((y0 - radius) >= surface->clip_ey)
	return;
	*/
	if( radius == 0 )
	{
		// 	 osdPut_PixelClip(surface, x0, y0, color);
		return;
	}

	y = radius;
	thres = 3 - (radius + radius);

	for( x = 0; x <= y; x ++ )
	{
		s_circle_callback(surface, x0, y0, x, y, color);

		if( thres < 0 )
			thres += 6 + (x << 2);
		else
		{
			thres += 10 + ((x - y) << 2);
			y --;
		}

	}
}

static  void	 circleFill_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-x, y0+y, x+x+1, 1));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-y, y0+x, y+y+1, 1));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-x, y0-y, x+x+1, 1));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-y, y0-x, y+y+1, 1));
}


static  void	 normalcircle_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult	 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	int			 pitch;
	unsigned long		 *addr;

	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	if (err == DFB_LOCKED)
	{	 dfbSurface->Unlock(dfbSurface);
	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	}

	*(addr + (y0+y) * pitch + (x0+x) ) = color; //  osdPut_PixelClip(pixmap, x0+x, y0+y, color);
	*(addr + (y0+x) * pitch + (x0+y) ) = color; //  osdPut_PixelClip(pixmap, x0+y, y0+x, color);
	*(addr + (y0+y) * pitch + (x0-x) ) = color;	 //  osdPut_PixelClip(pixmap, x0-x, y0+y, color);
	*(addr + (y0+x) * pitch + (x0-y) ) = color;	 //  osdPut_PixelClip(pixmap, x0-y, y0+x, color);
	*(addr + (y0-y) * pitch + (x0+x) ) = color;	 //  osdPut_PixelClip(pixmap, x0+x, y0-y, color);
	*(addr + (y0-x) * pitch + (x0+y) ) = color;	 //  osdPut_PixelClip(pixmap, x0+y, y0-x, color);
	*(addr + (y0-y) * pitch + (x0-x) ) = color;	 //  osdPut_PixelClip(pixmap, x0-x, y0-y, color);
	*(addr + (y0-x) * pitch + (x0-y) ) = color;	 //  osdPut_PixelClip(pixmap, x0-y, y0-x, color);

	dfbSurface->Unlock(dfbSurface);
}


static  void	 roundCornerTopLeft_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult	 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	int			 pitch;
	unsigned long		 *addr;

	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	if (err == DFB_LOCKED)
	{	 dfbSurface->Unlock(dfbSurface);
	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	}
	pitch /= 4;
	*(addr + (y0-y) * pitch + (x0-x) ) = color;	 //  osdPut_PixelClip(pixmap, x0-x, y0-y, color);
	*(addr + (y0-x) * pitch + (x0-y) ) = color;	 //  osdPut_PixelClip(pixmap, x0-y, y0-x, color);

	dfbSurface->Unlock(dfbSurface);
}

static  void	 roundCornerTopRight_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult	 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	int			 pitch;
	unsigned long		 *addr;

	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	if (err == DFB_LOCKED)
	{	 dfbSurface->Unlock(dfbSurface);
	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	}
	pitch /= 4;
	*(addr + (y0-y) * pitch + (x0+x) ) = color;	 //  osdPut_PixelClip(pixmap, x0+x, y0-y, color);
	*(addr + (y0-x) * pitch + (x0+y) ) = color;	 //  osdPut_PixelClip(pixmap, x0+y, y0-x, color);

	dfbSurface->Unlock(dfbSurface);
}

static  void	 roundCornerBottomLeft_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult	 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	int			 pitch;
	unsigned long		 *addr;

	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	if (err == DFB_LOCKED)
	{	 dfbSurface->Unlock(dfbSurface);
	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	}
	pitch /= 4;
	*(addr + (y0+y) * pitch + (x0-x) ) = color;	 //  osdPut_PixelClip(pixmap, x0-x, y0+y, color);
	*(addr + (y0+x) * pitch + (x0-y) ) = color;	 //  osdPut_PixelClip(pixmap, x0-y, y0+x, color);

	dfbSurface->Unlock(dfbSurface);
}

static  void	 roundCornerBottomRight_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult	 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	int			 pitch;
	unsigned long		 *addr;

	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	if (err == DFB_LOCKED)
	{	 dfbSurface->Unlock(dfbSurface);
	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, (void*)&addr, &pitch);
	}
	pitch /= 4;
	*(addr + (y0+y) * pitch + (x0+x) ) = color;	 //  osdPut_PixelClip(pixmap, x0+x, y0+y, color);
	*(addr + (y0+x) * pitch + (x0+y) ) = color;	 //  osdPut_PixelClip(pixmap, x0+y, y0+x, color);

	dfbSurface->Unlock(dfbSurface);
}

static  void	 roundCornerFillTopLeft_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-x, y0-y, x, 1 ));
	}
	if (y > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-y, y0-x, y, 1 ));
	}
}

static  void	 roundCornerFillTopRight_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0, y0-y, x, 1));
	}
	if (y > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0, y0-x, y, 1));
	}
}

static  void	 roundCornerFillBottomLeft_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-x, y0+y, x, 1));
	}
	if (y > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0-y, y0+x, y, 1));
	}
}

static  void	 roundCornerFillBottomRight_callback(ONDKSurfaceHandle surface, int x0, int y0, int x, int y, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0, y0+y, x, 1));
	}
	if (y > 0)
	{
		DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x0, y0+x, y, 1));
	}
}



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/



void	 ONDK_DRAW_FillRoundCorner(ONDKSurfaceHandle surface, int x0, int y0, int radius, int corner, ONDK_Color_t color)
{
	//GWM_assert(corner>=0);
	//GWM_assert(corner<=3);
	switch(corner)
	{
	case TOP_LEFT_CORNER:	 /* Top Left*/
		s_circle_callback = roundCornerFillTopLeft_callback;
		break;
	case TOP_RIGHT_CORNER:  /* Top Right*/
		s_circle_callback = roundCornerFillTopRight_callback;
		break;
	case BOTTOM_LEFT_CORNER:	 /* Bottom Left*/
		s_circle_callback = roundCornerFillBottomLeft_callback;
		break;
	case BOTTOM_RIGHT_CORNER:	 /* Bottom Right*/
		s_circle_callback = roundCornerFillBottomRight_callback;
		break;
	default:
		return;
	}
	circleSubRoutine(surface, x0, y0, radius, color);
}


void	 ONDK_DRAW_StrokeRoundRect(ONDKSurfaceHandle surface, int x1, int y1, int x2, int y2, int r, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x1 > x2)
	{
		SWAP(x1, x2);
	}
	if (y1 > y2)
	{
		SWAP(y1, y2);
	}

	//  if ( is_clipped_area(pixmap, x1, y1, x2, y2) )
	// 	 return;

	if ( (r+r) > (x2-x1) )
		r = (x2-x1)/2;
	if ( (r+r) > (y2-y1) )
		r = (y2-y1)/2;


	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x1+r, y1, x2-x1-r-r, 1));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x1+r, y2, x2-x1-r-r, 1));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x1, y1+r, 1, y2-y1-r-r ));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x2, y1+r, 1, y2-y1-r-r ));

	ONDK_DRAW_StrokeRoundCorner(surface, x1+r, y1+r, r, TOP_LEFT_CORNER, color);
	ONDK_DRAW_StrokeRoundCorner(surface, x2-r, y1+r, r, TOP_RIGHT_CORNER, color);
	ONDK_DRAW_StrokeRoundCorner(surface, x1+r, y2-r, r, BOTTOM_LEFT_CORNER, color);
	ONDK_DRAW_StrokeRoundCorner(surface, x2-r, y2-r, r, BOTTOM_RIGHT_CORNER, color);
}


void	 ONDK_DRAW_StrokeRect(ONDKSurfaceHandle surface, int x1, int y1, int x2, int y2, int r, ONDK_Color_t color)
{
	DFBResult			err;
	ONDK_Rect_t			stRect;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x1 > x2)
	{
		SWAP(x1, x2);
	}
	if (y1 > y2)
	{
		SWAP(y1, y2);
	}

	if ( (r+r) > (x2-x1) )
		r = (x2-x1)/2;
	if ( (r+r) > (y2-y1) )
		r = (y2-y1)/2;

	// Top
	stRect = ONDK_Rect(x1, y1, (x2-x1), r);
	ONDK_DRAW_FillRect(surface, stRect, color);

	// Left
	stRect = ONDK_Rect(x1, (y1+r), r, (y2-y1-(r*2)));
	ONDK_DRAW_FillRect(surface, stRect, color);

	// Right
	stRect = ONDK_Rect((x2-r), (y1+r), r, (y2-y1-(r*2)));
	ONDK_DRAW_FillRect(surface, stRect, color);

	// Bottom
	stRect = ONDK_Rect(x1, (y2-r), (x2-x1), r);
	ONDK_DRAW_FillRect(surface, stRect, color);
}


void	 ONDK_DRAW_FillRoundRect(ONDKSurfaceHandle surface, int x1, int y1, int x2, int y2, int r, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	int w, h;

	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));

	if (x1 > x2)
	{
		SWAP(x1, x2);
	}
	if (y1 > y2)
	{
		SWAP(y1, y2);
	}

	//  if ( is_clipped_area(pixmap, x1, y1, x2, y2) )
	// 	 return;

	if ( (r+r) > (x2-x1) )
		r = (x2-x1)/2;
	if ( (r+r) > (y2-y1) )
		r = (y2-y1)/2;
	w = x2-x1+1;
	h = y2-y1+1;

	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x1, y1+r, w, h-r-r ));
	w -= r+r;
	ONDK_DRAW_FillRoundCorner(surface, x1+r, y1+r, r, TOP_LEFT_CORNER, color);
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x1+r, y1, w, r ));
	ONDK_DRAW_FillRoundCorner(surface, x2-r, y1+r, r, TOP_RIGHT_CORNER, color);

	ONDK_DRAW_FillRoundCorner(surface, x1+r, y2-r, r, BOTTOM_LEFT_CORNER, color);
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x1+r, y2-r+1, w, r ));
	ONDK_DRAW_FillRoundCorner(surface, x2-r, y2-r, r, BOTTOM_RIGHT_CORNER, color);
}

void	 ONDK_DRAW_FillCopyRectElement(ONDKSurfaceHandle _surface, HINT32 x, HINT32 y, HINT32 w, HINT32 h, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetColorCopy(_surface, COL_A(_color), COL_R(_color), COL_G(_color), COL_B(_color));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x, y, w, h));
}

void	 ONDK_DRAW_FillRectElement(ONDKSurfaceHandle _surface, HINT32 x, HINT32 y, HINT32 w, HINT32 h, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetColorBlend(_surface, COL_A(_color), COL_R(_color), COL_G(_color), COL_B(_color));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, x, y, w, h));
}

void	 ONDK_DRAW_Rect(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetCopy(_surface);
	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(_color), COL_G(_color), COL_B(_color), COL_A(_color)));
	DFBCHECK(dfbSurface->DrawRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
}

extern	void	 		ONDK_DRAW_RectElement(ONDKSurfaceHandle _surface,  HINT32 x, HINT32 y, HINT32 w, HINT32 h, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetCopy(_surface);
	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(_color), COL_G(_color), COL_B(_color), COL_A(_color)));
	DFBCHECK(dfbSurface->DrawRectangle(dfbSurface, x, y, w, h));
}

void	 ONDK_DRAW_FillRect(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetColorBlend(_surface, COL_A(_color), COL_R(_color), COL_G(_color), COL_B(_color));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
}

void	 ONDK_DRAW_FillRectWithoutColorBlend(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
}

void	 ONDK_DRAW_FillCopyRect(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetCopy(_surface);
	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(_color), COL_G(_color), COL_B(_color), COL_A(_color)));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
}

void	 ONDK_DRAW_FillRectToVideoSurface(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color)
{
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	ONDK_SURFACE_Blitflag_SetBlendWithVideo(_surface);
	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(_color), COL_G(_color), COL_B(_color), COL_A(_color)));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
}

void	 ONDK_DRAW_FillRect16(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, ONDK_Color_t _color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	_color  = ((_color & 0xF000) * 0x11000)
		| ((_color & 0x0F00) * 0x01100)
		| ((_color & 0x00F0) * 0x00110)
		| ((_color & 0x000F) * 0x00011);
	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(_color), COL_G(_color), COL_B(_color), COL_A(_color)));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
}

/* DirectFB  8bpp Screen Drawing. - Broadcom Driver's Direct FB could not BLIT for 8bpp so,
I'm using direct access(software blit) for blit 8bpp function temporary. - by ccash@humax */
void	 ONDK_DRAW_FillRectLUT(ONDKSurfaceHandle _surface, ONDK_Rect_t _rc, HUINT8 _color)
{
#if	1		// There is some BUGs in BRCM driver. if (Slave mode(?)) then Blit will not be expected.
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	HUINT8		 *ptr;
	int			 pitch, i, j;

	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_WRITE, (void**)&ptr, &pitch));
	for (i=_rc.y; i<(_rc.y+_rc.h); i++)
	{
		for (j=_rc.x; j<(_rc.x+_rc.w); j++)
		{
			*(ptr+i*pitch+j) = _color;
		}
	}
	DFBCHECK(dfbSurface->Unlock (dfbSurface));
#else
	DFBResult				 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	DFBCHECK(dfbSurface->SetColorIndex(dfbSurface, _color ));
	DFBCHECK(dfbSurface->FillRectangle(dfbSurface, _rc.x, _rc.y, _rc.w, _rc.h));
#endif
}


void		ONDK_DRAW_FillGradationRect(ONDKSurfaceHandle surface, ONDK_Rect_t rc, HBOOL isHorizontal, ONDK_Color_t c1, ONDK_Color_t c2 )
{
	int	 i, w, h;
	ONDK_Color_t  blend;
	DFBResult	 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	w = rc.w;
	h = rc.h;
	if (isHorizontal)
	{
		for ( i=0; i<h; i++ )
		{
			blend=ONDK_COLOR_BlendWithAlpha(c2, c1, (HUINT8)(i*256/h) );
			DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(blend), COL_G(blend), COL_B(blend), COL_A(blend)));
			DFBCHECK(dfbSurface->FillRectangle(dfbSurface, rc.x, rc.y+i, rc.w, 1));
		}
	}
	else
	{
		for ( i=0; i<w; i++ )
		{
			blend=ONDK_COLOR_BlendWithAlpha(c2, c1, (HUINT8)(i*256/w) );
			DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(blend), COL_G(blend), COL_B(blend), COL_A(blend)));
			DFBCHECK(dfbSurface->FillRectangle(dfbSurface, rc.x+i, rc.y, 1, rc.h));
		}
	}
}

void ONDK_DRAW_Line(ONDKSurfaceHandle surface, int x1, int y1, int x2, int y2, ONDK_Color_t color)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	DFBCHECK(dfbSurface->SetColor(dfbSurface,COL_R(color), COL_G(color), COL_B(color), COL_A(color)));
	DFBCHECK(dfbSurface->DrawLine (dfbSurface, x1, y1, x2, y2));
}


void ONDK_DRAW_Bitmap(ONDKSurfaceHandle surface, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pucImgOrg)
{
	DFBResult		err;
	IDirectFBSurface	*dfbSurface = (IDirectFBSurface*)surface;
	HUINT8			*tptr;
	HUINT8			*pucImg = pucImgOrg;
	void			*ptr = NULL;
	int				pitch, i, j;

	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_READ|DSLF_WRITE, &ptr, &pitch));

	for (i=0; i<h; i++)
	{
		tptr = ((HUINT8 *)ptr + ((y+i ) * pitch) + (x*4));
		for (j=0; j<w; j++)
		{
			*tptr = *pucImg; tptr++; pucImg++;
			*tptr = *pucImg; tptr++; pucImg++;
			*tptr = *pucImg; tptr++; pucImg++;
			*tptr = *pucImg; tptr++; pucImg++;
		}
	}

	DFBCHECK(dfbSurface->Unlock (dfbSurface));
}



void	 ONDK_DRAW_StrokeCircle(ONDKSurfaceHandle surface, int x0, int y0, int radius, ONDK_Color_t color)
{
	s_circle_callback = normalcircle_callback;
	circleSubRoutine(surface, x0, y0, radius, color);
}

void	 ONDK_DRAW_FillCircle(ONDKSurfaceHandle surface, int x0, int y0, int radius, ONDK_Color_t color)
{
	s_circle_callback = circleFill_callback;
	circleSubRoutine(surface, x0, y0, radius, color);
}


void	 ONDK_DRAW_StrokeRoundCorner(ONDKSurfaceHandle surface, int x0, int y0, int radius, int corner, ONDK_Color_t color)
{
	//GWM_assert(corner>=0);
	//GWM_assert(corner<=3);
	switch(corner)
	{
	case TOP_LEFT_CORNER:	 /* Top Left*/
		s_circle_callback = roundCornerTopLeft_callback;
		circleSubRoutine(surface, x0, y0, radius, color);
		break;
	case TOP_RIGHT_CORNER:  /* Top Right*/
		s_circle_callback = roundCornerTopRight_callback;
		circleSubRoutine(surface, x0, y0, radius, color);
		break;
	case BOTTOM_LEFT_CORNER:	 /* Bottom Left*/
		s_circle_callback = roundCornerBottomLeft_callback;
		circleSubRoutine(surface, x0, y0, radius, color);
		break;
	case BOTTOM_RIGHT_CORNER:	 /* Bottom Right*/
		s_circle_callback = roundCornerBottomRight_callback;
		circleSubRoutine(surface, x0, y0, radius, color);
		break;
	default:
		break;
	}
}




void	 ONDK_DRAW_GetColorLookupTable(ONDKSurfaceHandle _surface, ONDK_Color_t *lookup_table, int start_index, int end_index )
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	IDirectFBPalette	 *palette;
	DFBColor			 colour[256];
	int	 i, pal_size;

	ONDK_Assert(start_index >= 0);
	ONDK_Assert(end_index <= 255);

	DFBCHECK(dfbSurface->GetPalette(dfbSurface, &palette));
	DFBCHECK(palette->GetSize( palette, &pal_size));
	DFBCHECK(palette->GetEntries( palette, colour, pal_size, 0));

	for (i=start_index; i<=end_index; i++)
	{
		lookup_table[i] = (ONDK_Color_t)(colour[i].a<<24) | (colour[i].r<<16) | (colour[i].g<<8) | (colour[i].b) ;
	}

}

void	 ONDK_DRAW_SetColorLookupTable(ONDKSurfaceHandle _surface, ONDK_Color_t *lookup_table, int start_index, int end_index )
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	IDirectFBPalette	 *palette;
	DFBColor			 colour[256];
	int	 i, pal_size;

	ONDK_Assert(start_index >= 0);
	ONDK_Assert(end_index <= 255);

	DFBCHECK(dfbSurface->GetPalette(dfbSurface, &palette));
	DFBCHECK(palette->GetSize( palette, &pal_size));
	DFBCHECK(palette->GetEntries( palette, colour, pal_size, 0));

	for (i=start_index; i<=end_index; i++)
	{
		colour[i].a = COL_A(lookup_table[i] );
		colour[i].r = COL_R(lookup_table[i] );
		colour[i].g = COL_G(lookup_table[i] );
		colour[i].b = COL_B(lookup_table[i] );
	}
	DFBCHECK(palette->SetEntries( palette, colour, pal_size, 0 ));
	DFBCHECK(dfbSurface->SetPalette(dfbSurface, palette));

}



#define	DLG_FRAME_OUTLINE_COLOR1	0xFF3C3F44
#define	DLG_FRAME_OUTLINE_COLOR2	0xFF2B2E33
#define	DLG_FRAME_FACE_COLOR		0xFFE8E8E8

#define	DLG_CAPTION_BAR_COLOR		0xFF7F7F7F
#define	DLG_CAPTION_FONT_COLOR	0xFFF8F8F8
#define	DLG_MESSAGE_FONT_COLOR	0xFF040404

#define	DLG_BTN_OUTLINE_COLOR		0xFF010101
#define	DLG_BTN_BRIGHT_COLOR1		0xFF5D5D5D
#define	DLG_BTN_BRIGHT_COLOR2		0xFF373536
#define	DLG_BTN_DARK_COLOR1		0xFF1E1C1D
#define	DLG_BTN_DARK_COLOR2		0xFF040404
#define	DLG_FOCUSED_OUTLINE_COLOR		0xFF00417D
#define	DLG_FOCUSED_BRIGHT_COLOR1		0xFF94CEFF
#define	DLG_FOCUSED_BRIGHT_COLOR2		0xFF2684c2
#define	DLG_FOCUSED_DARK_COLOR1		0xFF016CB4
#define	DLG_FOCUSED_DARK_COLOR2		0xFF026DB5
#define	DLG_BTN_CAPTION_COLOR		0xFFF8F8F8

void	ONDK_DRAW_DrawButton( ONDKSurfaceHandle _surface, ONDK_Rect_t rect, ONDKFontHandle fontHandle, HUINT8 *title, int hilight)
{
	ONDK_Color_t	out_col, bright_col1,bright_col2, dark_col1,dark_col2, font_col;
	ONDK_Rect_t	_rc;

	if (hilight==FALSE)
	{	out_col = DLG_BTN_OUTLINE_COLOR;
		bright_col1 = DLG_BTN_BRIGHT_COLOR1;
		bright_col2 = DLG_BTN_BRIGHT_COLOR2;
		dark_col1 = DLG_BTN_DARK_COLOR1;
		dark_col2 = DLG_BTN_DARK_COLOR2;
		font_col = DLG_BTN_CAPTION_COLOR;
	}
	else
	{	out_col = DLG_FOCUSED_OUTLINE_COLOR;
		bright_col1 = DLG_FOCUSED_BRIGHT_COLOR1;
		bright_col2 = DLG_FOCUSED_BRIGHT_COLOR2;
		dark_col1 = DLG_FOCUSED_DARK_COLOR1;
		dark_col2 = DLG_FOCUSED_DARK_COLOR2;
		font_col = DLG_BTN_CAPTION_COLOR;
	}
	// 버튼 테두리 그리기
	_rc.x = rect.x+1;		_rc.y = rect.y;		_rc.w = rect.w-2;		_rc.h = 2;
	ONDK_DRAW_FillRect(_surface,_rc, out_col);
	_rc.y = rect.y+rect.h-2;
	ONDK_DRAW_FillRect(_surface,_rc, out_col);
	_rc.x = rect.x;		_rc.y = rect.y+1;		_rc.w = 2;		_rc.h = rect.h-2;
	ONDK_DRAW_FillRect(_surface,_rc, out_col);
	_rc.x = rect.x+rect.w-2;
	ONDK_DRAW_FillRect(_surface,_rc, out_col);

	_rc.x = rect.x+2;		_rc.y = rect.y+2; 	_rc.w = rect.w-4;		_rc.h = (rect.h-4)/2;
	ONDK_DRAW_FillGradationRect(_surface, _rc, TRUE, bright_col1, bright_col2 );
	_rc.x = rect.x+2;		_rc.y = rect.y+(rect.h/2); 	_rc.w = rect.w-4;		_rc.h = (rect.h-4)/2;
	ONDK_DRAW_FillGradationRect(_surface, _rc, TRUE, dark_col1, dark_col2 );
	{
		int 		line_count, str_w, str_h, font_height;
		font_height = ONDK_FONT_GetHeight(fontHandle);
		line_count = ONDK_FONT_GetWidthMultiLine( fontHandle, title, rect.w, 0, &str_w, &str_h );
		ONDK_FONT_DrawStringForMultiLine( _surface, fontHandle, rect.x+(rect.w-str_w)/2, rect.y+(rect.h-font_height)/2, str_w, font_height, title, 0, font_col );
	}

}


//#define	DLG_TYPE_BUTTONTYPE	0x0000000F
#define	DLG_TYPE_NO_BUTTON		0x00000000
#define	DLG_TYPE_CONFIRM		0x00000001
#define	DLG_TYPE_YES_NO			0x00000002
#define	DLG_TYPE_YES_NO_CANCEL	0x00000003

#define	DLG_FOCUS_ON_CONFIRM	1
#define	DLG_FOCUS_ON_YES		2
#define	DLG_FOCUS_ON_NO		3
#define	DLG_FOCUS_ON_CANCEL	4


void	ONDK_DRAW_DrawDialogFrame( ONDKSurfaceHandle _surface, ONDK_Rect_t rect, ONDKFontHandle fontHandle, HUINT8 *title)
{
	ONDK_Rect_t	_rc;
	// Frame 을 그린다.
	_rc.x = rect.x+1;		_rc.y = rect.y+1;		_rc.w = rect.w-2;		_rc.h = 4;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR2);
	_rc.y = rect.y+rect.h-5;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR2);
	_rc.y = rect.y+1;		_rc.w = 4;		_rc.h = rect.h-5;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR2);
	_rc.x = rect.x+rect.w-5;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR2);

	_rc.x = rect.x;		_rc.y = rect.y;		_rc.w = rect.w;		_rc.h = 1;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR1);
	_rc.y = rect.y+rect.h-1;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR1);
	_rc.y = rect.y;		_rc.w = 1;		_rc.h = rect.h-1;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR1);
	_rc.x = rect.x+rect.w-1;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_OUTLINE_COLOR1);

	_rc.x = rect.x+5;		_rc.y = rect.y+5;		_rc.w = rect.w-10;		_rc.h = rect.h-10;
	ONDK_DRAW_FillRect(_surface,_rc, DLG_FRAME_FACE_COLOR);

	if ( title != NULL )
	{	int 		line_count, str_w, str_h, font_height;
		_rc.x = rect.x+5;		_rc.y = rect.y+5;		_rc.w = rect.w-10;		_rc.h = 42;	//rect.h-10;
		ONDK_DRAW_FillRect(_surface,_rc, DLG_CAPTION_BAR_COLOR);
		font_height = ONDK_FONT_GetHeight(fontHandle);
		line_count = ONDK_FONT_GetWidthMultiLine( fontHandle, title, rect.w, 0, &str_w, &str_h );
		ONDK_FONT_DrawStringForMultiLine( _surface, fontHandle, rect.x+(rect.w-str_w)/2, rect.y+10, str_w, font_height, title, 0, DLG_CAPTION_FONT_COLOR );
	}

}


void	ONDK_DRAW_SystemNoticeDialog( ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT8 *text_message)
{
	ONDK_Rect_t _rc;
	int		surface_width, surface_height;
	int 		line_count, str_w, str_h, font_height;

	ONDK_Assert(text_message)	;
	ONDK_SURFACE_GetSize(_surface, &surface_width, &surface_height );

	font_height = ONDK_FONT_GetHeight(fontHandle);
	line_count = ONDK_FONT_GetWidthMultiLine( fontHandle, text_message, surface_width/2+surface_width/4, 0, &str_w, &str_h );	// 메세지 표시의 MAX 표시는 전체 스크린의 절반 크기가 MAX.

	_rc.w = str_w + 60;		_rc.h = (font_height+10) * line_count + 60;
	_rc.x = (surface_width-_rc.w)/2;	_rc.y = (surface_height-_rc.h)/2;
	ONDK_DRAW_DrawDialogFrame( _surface, _rc, fontHandle, NULL );	// title 없는 다이얼로그 바탕그림.

	_rc.x += 30;	_rc.w = str_w;		_rc.y +=30;
	ONDK_FONT_DrawStringForMultiLine( _surface, fontHandle, _rc.x, _rc.y, _rc.w, _rc.h, text_message, 10, DLG_MESSAGE_FONT_COLOR );

}


