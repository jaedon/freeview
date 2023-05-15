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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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



/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

void			 ONDK_WINDOW_Clear(ONDKWindowHandle mWindow)
{
	DFBResult err;
	IDirectFBWindow	*dfbWindow = (IDirectFBWindow*)mWindow;
	IDirectFBSurface	*dfbSurface;

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
		DFBCHECK(dfbSurface->Clear(dfbSurface, 0, 0, 0, 0));
	}
}

void			 ONDK_WINDOW_Destroy(ONDKWindowHandle mWindow)
{
   DFBResult err;
   IDirectFBWindow           *dfbWindow = (IDirectFBWindow*)mWindow;
   IDirectFBSurface *dfbSurface = NULL;

	ONDK_KEY_ReleaseBuffer(mWindow);

   if (NULL != dfbWindow)
   {
         DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
         DFBCHECK(dfbSurface->ReleaseSource(dfbSurface));
         DFBCHECK(dfbSurface->Release(dfbSurface));
         DFBCHECK(dfbWindow->Release(dfbWindow));
   }

}

/**
*
*	Ô¼ Üº process  window id  Octo	Ï¿ OCTO  directFB  Ì¿Ï¿ ×¸ ×¸ Ï¸
* Ã³ Ò· Ï´ Ô¼ Ì´.
* OCTO  mlib_init	 Ê´Â´. (SAWMAN	 Ê¿   Æ¼...)
*
*/
ONDKWindowHandle ONDK_WINDOW_GetFromWindowID(HUINT32 windowID)
{
	DFBResult				 err;
	IDirectFBWindow			 *pWindow = NULL;
	IDirectFBDisplayLayer	 *pDispLayer = NULL;
	DFBWindowID				 dfbWndId = (DFBWindowID)windowID;
	IDirectFB				*pDirectFB = NULL;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	DFBCHECK(pDirectFB->GetDisplayLayer(pDirectFB, DLID_PRIMARY, &pDispLayer));
	DFBCHECK(pDispLayer->GetWindow(pDispLayer, dfbWndId, &pWindow));

	return (ONDKWindowHandle)pWindow;
}

HINT32   ONDK_WINDOW_GetIDFromWindow(ONDKWindowHandle hWindow)
{
	unsigned long	applId;
	IDirectFBWindow			*pFBWindow	=	(IDirectFBWindow*)hWindow;

	pFBWindow->GetApplicationID(pFBWindow, &applId);

	return applId;
}

HINT32	ONDK_WINDOW_GetWindowIDFromWindow(ONDKWindowHandle hWindow)
{
	DFBWindowID WindowId;
	IDirectFBWindow			*pFBWindow	=	(IDirectFBWindow*)hWindow;

	pFBWindow->GetID(pFBWindow, &WindowId);

	return WindowId;
}

ONDKWindowHandle	 ONDK_WINDOW_Create(HUINT32 applId, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *pixelformat, HBOOL focusable)
{
	DFBResult err;
	DFBWindowDescription	wdesc;
	IDirectFBWindow		 	*dfbWindow;
	IDirectFBSurface		*dfbSurface;

	wdesc.flags = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_OPTIONS | DWDESC_PIXELFORMAT;
	wdesc.caps  = DWCAPS_ALPHACHANNEL;
	wdesc.options = DWOP_ALPHACHANNEL | DWOP_SCALE ;
	wdesc.posx  = x;
	wdesc.posy  = y;
	wdesc.width  = w;
	wdesc.height = h;
	if (!focusable)
	{
		wdesc.caps |= DWCAPS_NOFOCUS;
		wdesc.options |= DWOP_GHOST;
	}
	if (HLIB_STD_StrCaseCmp(pixelformat, "ARGB8888") == 0)
	{
		wdesc.pixelformat = DSPF_ARGB;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "ARGB4444") == 0)
	{
		wdesc.pixelformat = DSPF_ARGB4444;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "INDEX8") == 0)
	{
		wdesc.pixelformat = DSPF_LUT8;
	}
	else
	{
		wdesc.pixelformat = DSPF_ARGB;
	}

	{
		IDirectFBDisplayLayer *pLayer = ONDK_DFB_GetIDirectFBDisplayLayer();
		ONDK_Assert(pLayer);
		DFBCHECK(pLayer->CreateWindow(pLayer, &wdesc, &dfbWindow));
	}

	// resize a window for OSD
	if(w == GWM_SCREEN_WIDTH && h == GWM_SCREEN_HEIGHT)
	{
		if(OSD_SCREEN_WIDTH != GWM_SCREEN_WIDTH)
		{
			DFBCHECK(dfbWindow->Resize(dfbWindow, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT));
		}
	}

	DFBCHECK(dfbWindow->SetApplicationID(dfbWindow, applId));
	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	DFBCHECK(dfbSurface->Clear(dfbSurface, 0, 0, 0, 0));

	if(focusable)
	{
		//IDirectFBEventBuffer *evtBuf = NULL;
		//DFBCHECK(dfbWindow->CreateEventBuffer(dfbWindow, &evtBuf));
		ONDK_KEY_SetBuffer(dfbWindow );
	}


	return (ONDKWindowHandle)dfbWindow;
}




ONDKWindowHandle	 ONDK_WINDOW_CreateInputOnly(HUINT32 applId)
{
	DFBResult err;
	DFBWindowDescription	wdesc;
	IDirectFBWindow		 	*dfbWindow;
    IDirectFBDisplayLayer *pLayer = ONDK_DFB_GetIDirectFBDisplayLayer();
    ONDK_Assert(pLayer);

	wdesc.flags = DWDESC_CAPS;
	wdesc.caps  = DWCAPS_INPUTONLY;

	DFBCHECK(pLayer->CreateWindow(pLayer, &wdesc, &dfbWindow));
	DFBCHECK(dfbWindow->SetApplicationID(dfbWindow, applId));

	ONDK_KEY_SetBuffer(dfbWindow );
	return (ONDKWindowHandle)dfbWindow;
}


void			 ONDK_WINDOW_SetShow(ONDKWindowHandle mWindow, HBOOL bShow)
{
	DFBResult err;
	IDirectFBWindow	 *dfbWindow = (IDirectFBWindow*)mWindow;

	if (NULL != dfbWindow)
	{
		if (bShow)
		{
			DFBCHECK(dfbWindow->SetOpacity(dfbWindow, 0xff));
		}
		else
		{
			DFBCHECK(dfbWindow->SetOpacity(dfbWindow, 0x00));
		}
	}
}

HBOOL	 ONDK_WINDOW_IsShow(ONDKWindowHandle mWindow)
{
	DFBResult err = DFB_OK;
	IDirectFBWindow	 *dfbWindow = (IDirectFBWindow*)mWindow;
	HUINT8  color =0 ;

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->GetOpacity(dfbWindow,&color));
	}
	return ( color> 0) ? TRUE : FALSE;
}

void			 ONDK_WINDOW_SetOpacity(ONDKWindowHandle mWindow, HUINT32 opacity)
{
	DFBResult err = DFB_OK;
	IDirectFBWindow	 *dfbWindow = (IDirectFBWindow*)mWindow;

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->SetOpacity(dfbWindow, opacity));
	}
}

void			 ONDK_WINDOW_UpdateFlipNone(ONDKWindowHandle mWindow, ONDK_Rect_t *rc)
{
	DFBResult err;
	DFBRegion region;
	IDirectFBWindow		 *dfbWindow = (IDirectFBWindow*)mWindow;
	IDirectFBSurface		 *dfbSurface;

	if (dfbWindow == NULL)
		return;

	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	if (rc == NULL)
	{
		DFBCHECK(dfbSurface->Flip(dfbSurface, NULL, DSFLIP_NONE));
	}
	else
	{
		region.x1 = rc->x;
		region.y1 = rc->y;
		region.x2 = rc->x + rc->w;
		region.y2 = rc->y + rc->h;

		DFBCHECK(dfbSurface->Flip(dfbSurface, &region, DSFLIP_NONE));
	}
}

void			 ONDK_WINDOW_UpdateFlipOnSync(ONDKWindowHandle mWindow, ONDK_Rect_t *rc)
{
	DFBResult err;
	DFBRegion region;
	IDirectFBWindow		 *dfbWindow = (IDirectFBWindow*)mWindow;
	IDirectFBSurface		 *dfbSurface;

	if (dfbWindow == NULL)
		return;

	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	if (rc == NULL)
	{
		DFBCHECK(dfbSurface->Flip(dfbSurface, NULL, DSFLIP_WAITFORSYNC));
	}
	else
	{
		region.x1 = rc->x;
		region.y1 = rc->y;
		region.x2 = rc->x + rc->w;
		region.y2 = rc->y + rc->h;

		DFBCHECK(dfbSurface->Flip(dfbSurface, &region, DSFLIP_WAITFORSYNC));
	}
}


void			 ONDK_WINDOW_Update(ONDKWindowHandle mWindow, ONDK_Rect_t *rc)
{
	DFBResult err;
	DFBRegion region;
	IDirectFBWindow		 *dfbWindow = (IDirectFBWindow*)mWindow;
	IDirectFBSurface		 *dfbSurface;

	if (dfbWindow == NULL)
		return;

	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	if (rc == NULL)
	{
		DFBCHECK(dfbSurface->Flip(dfbSurface, NULL, (DSFLIP_BLIT)));
	}
	else
	{
		region.x1 = rc->x;
		region.y1 = rc->y;
		region.x2 = rc->x + rc->w;
		region.y2 = rc->y + rc->h;

		DFBCHECK(dfbSurface->Flip(dfbSurface, &region, (DSFLIP_BLIT)));
	}
}

void		 ONDK_WINDOW_SetResolution(ONDKWindowHandle window, HINT32 w, HINT32 h)
{
	DFBResult err;
	IDirectFBWindow *dfbWindow = (IDirectFBWindow *)window;
	IDirectFBSurface	 *dfbSurface;
	int	 windowsurface_w, windowsurface_h;

	if (dfbWindow == NULL)
		return;

	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	DFBCHECK (dfbSurface->GetSize (dfbSurface, &windowsurface_w, &windowsurface_h));

	DFBCHECK(dfbWindow->Resize(dfbWindow, (GWM_SCREEN_WIDTH*windowsurface_w/w), (GWM_SCREEN_HEIGHT*windowsurface_h/h) ));

}

void		 ONDK_WINDOW_SetWindowPos(ONDKWindowHandle window, HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	DFBResult err;
	IDirectFBWindow *dfbWindow = (IDirectFBWindow *)window;

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->Resize(dfbWindow, w, h ));
		DFBCHECK(dfbWindow->MoveTo(dfbWindow, x, y ));
	}
}


void		 ONDK_WINDOW_SetOutputAspectRatio(MOSD_ASPECTRATIO aspect)
{
	DFBResult	err;
	IDirectFB		*_dfb;
	IDirectFBDisplayLayer   *pPrimary = NULL;

	_dfb = ONDK_DFB_GetIDirectFB();
	DFBCHECK(_dfb->GetDisplayLayer(_dfb, DLID_PRIMARY, &pPrimary));

	switch(aspect)
	{
		case MOSD_LETTERBOX:
			DFBCHECK(pPrimary->SetScreenLocation (pPrimary, 0.0, 0.125, 1.0, 0.75 ));
			break;
		case MOSD_PILLARBOX:
			DFBCHECK(pPrimary->SetScreenLocation (pPrimary, 0.125, 0.0, 0.75, 1.0 ));
			break;
		case MOSD_FULLSCREEN:
		default:
			DFBCHECK(pPrimary->SetScreenLocation (pPrimary, 0.0, 0.0, 1.0, 1.0 ));
			break;
	}

}

/**********************************************************************************************************/
#define _______BLIT_FUNTION_______________________
/**********************************************************************************************************/
ONDK_Result_t	ONDK_WINDOW_Init(HINT32 argc, HCHAR **argv)
{

	return ONDK_RET_OK;
}

void			ONDK_WINDOW_Blit(ONDKWindowHandle _window, ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, HINT32 x, HINT32 y)
{
   	DFBResult err;
	IDirectFBWindow	*dfbWindow = (IDirectFBWindow*)_window;
	IDirectFBSurface	*dfbSurface = NULL;
	IDirectFBSurface	*srcSurface = (IDirectFBSurface*)_surface;
	DFBRectangle		sRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
		ONDK_SURFACE_Blitflag_SetCopy(dfbSurface);
		DFBCHECK(dfbSurface->Blit(dfbSurface, srcSurface, &sRect, x, y));
	}
}

void			ONDK_WINDOW_BlendBlit(ONDKWindowHandle _window, ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, HINT32 x, HINT32 y)
{
    	DFBResult err;
	IDirectFBWindow	*dfbWindow = (IDirectFBWindow*)_window;
	IDirectFBSurface	*dfbSurface = NULL;
	IDirectFBSurface	*srcSurface = (IDirectFBSurface*)_surface;
	DFBRectangle		sRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
		ONDK_SURFACE_Blitflag_SetBlend(dfbSurface);
		DFBCHECK(dfbSurface->Blit(dfbSurface, srcSurface, &sRect, x, y));
	}
}

void			ONDK_WINDOW_StretchBlit(ONDKWindowHandle _window,  ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect)
{
    	DFBResult err;
	IDirectFBWindow	*dfbWindow = (IDirectFBWindow*)_window;
	IDirectFBSurface	*dfbSurface = NULL;
	IDirectFBSurface	*srcSurface = (IDirectFBSurface*)_surface;

	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};
	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
		ONDK_SURFACE_Blitflag_SetCopy(dfbSurface);
		DFBCHECK(dfbSurface->StretchBlit(dfbSurface, srcSurface, &srcDfbRect, &dstDfbRect));
	}
}

void			ONDK_WINDOW_StretchBlendBlit(ONDKWindowHandle _window,  ONDKSurfaceHandle _surface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect)
{
    	DFBResult err;
	IDirectFBWindow	*dfbWindow = (IDirectFBWindow*)_window;
	IDirectFBSurface	*dfbSurface = NULL;
	IDirectFBSurface	*srcSurface = (IDirectFBSurface*)_surface;

	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};
	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};

	if (NULL != dfbWindow)
	{
		DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
		ONDK_SURFACE_Blitflag_SetBlend(dfbSurface);
		DFBCHECK(dfbSurface->StretchBlit(dfbSurface, srcSurface, &srcDfbRect, &dstDfbRect));
	}
}


