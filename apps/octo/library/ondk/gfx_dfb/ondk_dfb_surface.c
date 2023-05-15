/**************************************************************
 *	@file		ondk.h
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


typedef struct tagDFBSurface_SystemCapa
{
	DFBSurfaceBlittingFlags	blittingFlags;
	DFBSurfaceDrawingFlags 	drawFlags;
} DFBSurface_SystemCapa_t;

STATIC DFBSurface_SystemCapa_t		s_stDFBSurSysCapa;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

void	 ONDK_SURFACE_Clear(ONDKSurfaceHandle surface)
{
	DFBResult err;
	IDirectFBSurface		 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK (dfbSurface->Clear(dfbSurface, 0x00, 0x00, 0x00, 0x00));
}

void	ONDK_SURFACE_ClearRect(ONDKSurfaceHandle surface, ONDK_Region_t region)
{
	ONDK_SURFACE_SetClip(surface, region);
	ONDK_SURFACE_Clear(surface);
}

void	 ONDK_SURFACE_GetSize(ONDKSurfaceHandle surface, HINT32 *width, HINT32 *height)
{
	DFBResult err;
	IDirectFBSurface		 *dfbSurface = (IDirectFBSurface*)surface;

	if(width == NULL || height == NULL)
	{
		HxLOG_Critical("NULL POINT!\n");
		return;
	}

	DFBCHECK (dfbSurface->GetSize (dfbSurface, width, height));
}


ONDKSurfaceHandle  ONDK_SURFACE_GetFromWindow(ONDKWindowHandle mWindow)
{
	DFBResult err;
	IDirectFBWindow	 *dfbWindow = (IDirectFBWindow*)mWindow;
	IDirectFBSurface	 *dfbSurface;

	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));

	return (ONDKSurfaceHandle)dfbSurface;
}

ONDKSurfaceHandle  ONDK_SURFACE_CreateWithPixelFormatIndex(HINT32 width, HINT32 height, HINT32 pixelformat)
{
	DFBResult	 err;

	DFBSurfaceDescription	dsc;
	IDirectFBSurface		*surface = NULL;
	IDirectFB				*pDirectFB = NULL;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS;
	dsc.caps = DSCAPS_PREMULTIPLIED;
	dsc.width = width;
	dsc.height = height;
	dsc.pixelformat = pixelformat;

	DFBCHECK(pDirectFB->CreateSurface(pDirectFB, &dsc, &surface));

	HxLOG_Debug("[%s:%d] (+) surface(0x%x) name(%s)\r\n", __FUNCTION__, __LINE__, surface, ONDK_GWM_APP_GetAppName(ONDK_GWM_APP_GetThis()));

	return (ONDKSurfaceHandle)surface;
}


ONDKSurfaceHandle  ONDK_SURFACE_Create(HINT32 width, HINT32 height, HINT8 *pixelformat)
{
	DFBResult	 err;

	DFBSurfaceDescription	dsc;
	IDirectFBSurface		*surface = NULL;
	IDirectFB				*pDirectFB = NULL;
	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS ;
	dsc.caps = DSCAPS_PREMULTIPLIED;
	dsc.width = width;
	dsc.height = height;
	dsc.pixelformat = DSPF_ARGB;
//	dsc.caps =


	if (HLIB_STD_StrCaseCmp(pixelformat, "ARGB8888") == 0){
		dsc.pixelformat = DSPF_ARGB;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "ARGB4444") == 0){
		dsc.pixelformat = DSPF_ARGB4444;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "RGB32") == 0){
		dsc.pixelformat = DSPF_RGB32;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "INDEX8") == 0){
		dsc.pixelformat = DSPF_LUT8;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "RGB24") == 0){
		dsc.pixelformat = DSPF_RGB24;
	}

	DFBCHECK(pDirectFB->CreateSurface(pDirectFB, &dsc, &surface));

	HxLOG_Debug("[%s:%d] (+) surface(0x%x) name(%s)\r\n", __FUNCTION__, __LINE__, surface, ONDK_GWM_APP_GetAppName(ONDK_GWM_APP_GetThis()));

	return (ONDKSurfaceHandle)surface;
	//  HxLOG_Error("Not support Pixel Format : [%s]\n", pixelformat);
	//  return NULL;
}


ONDKSurfaceHandle  ONDK_SURFACE_CreatePrimary(HINT32 width, HINT32 height, HINT8 *pixelformat)
{
	DFBResult	 err;

	DFBSurfaceDescription	dsc;
	IDirectFBSurface		*surface = NULL;
	IDirectFB				*pDirectFB = NULL;
	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS;
	dsc.caps =  DSCAPS_FLIPPING | DSCAPS_PRIMARY | DSCAPS_PREMULTIPLIED;
	dsc.width = width;
	dsc.height = height;
	dsc.pixelformat = DSPF_ARGB;
//	dsc.caps =

	if (HLIB_STD_StrCaseCmp(pixelformat, "ARGB8888") == 0){
		dsc.pixelformat = DSPF_ARGB;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "ARGB4444") == 0){
		dsc.pixelformat = DSPF_ARGB4444;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "RGB32") == 0){
		dsc.pixelformat = DSPF_RGB32;
	}
	else if (HLIB_STD_StrCaseCmp(pixelformat, "INDEX8") == 0){
		dsc.pixelformat = DSPF_LUT8;
	}

	DFBCHECK(pDirectFB->CreateSurface(pDirectFB, &dsc, &surface));

	HxLOG_Debug("[%s:%d] (+) surface(0x%x) name(%s)\r\n", __FUNCTION__, __LINE__, surface, ONDK_GWM_APP_GetAppName(ONDK_GWM_APP_GetThis()));

	return (ONDKSurfaceHandle)surface;
	//  HxLOG_Error("Not support Pixel Format : [%s]\n", pixelformat);
	//  return NULL;
}

void ONDK_SURFACE_Blitflag_SetColorCopy(ONDKSurfaceHandle surface, HUINT8 a, HUINT8 r, HUINT8 g, HUINT8 b)
{
    DFBResult			 	err;
	IDirectFBSurface	 	*dfbSurface = (IDirectFBSurface*)surface;

	DFBSurfaceBlittingFlags blitFlags = DSBLIT_NOFX;
	DFBSurfaceDrawingFlags 	drawFlags = DSDRAW_NOFX;

	if(0 == a)
	{
		return;
	}

	if(0xFF != a)
	{
		blitFlags |= DSBLIT_BLEND_COLORALPHA;
	}
	blitFlags |= DSBLIT_COLORIZE;

	DFBCHECK(dfbSurface->SetBlittingFlags(dfbSurface, blitFlags));
	DFBCHECK(dfbSurface->SetPorterDuff( dfbSurface, DSPD_SRC ));
	DFBCHECK(dfbSurface->SetSrcBlendFunction(dfbSurface, DSBF_ONE));
	DFBCHECK(dfbSurface->SetDstBlendFunction(dfbSurface, DSBF_ZERO));
	DFBCHECK(dfbSurface->SetDrawingFlags(dfbSurface, drawFlags));
	DFBCHECK(dfbSurface->SetColor(dfbSurface, r, g, b, a));

}

STATIC void _ondk_surface_Blitflag_SetPorterDuff_SRCOVER(ONDKSurfaceHandle surface, DFBSurfaceBlittingFlags blitFlags, DFBSurfaceDrawingFlags 	drawFlags)
{
    DFBResult			 	err;
	IDirectFBSurface	 	*dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetBlittingFlags(dfbSurface, blitFlags));
	DFBCHECK(dfbSurface->SetDrawingFlags(dfbSurface, drawFlags));
	DFBCHECK(dfbSurface->SetPorterDuff( dfbSurface, DSPD_SRC_OVER));
}

STATIC void _ondk_surface_Blitflag_SetPorterDuff_SRC(ONDKSurfaceHandle surface, DFBSurfaceBlittingFlags blitFlags, DFBSurfaceDrawingFlags 	drawFlags)
{
    DFBResult			 	err;
	IDirectFBSurface	 	*dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetBlittingFlags(dfbSurface, blitFlags));
	DFBCHECK(dfbSurface->SetDrawingFlags(dfbSurface, drawFlags));
	DFBCHECK(dfbSurface->SetPorterDuff( dfbSurface, DSPD_SRC));
}

STATIC DFBSurfaceDrawingFlags _ondk_surface_Blitflag_GetDrawFlagWidthCapa(DFBSurfaceDrawingFlags drawFlag)
{
	DFBSurfaceDrawingFlags checkedFlags = drawFlag;


	return checkedFlags;
}


void ONDK_SURFACE_Blitflag_SetCopy(ONDKSurfaceHandle surface)
{
	DFBSurfaceBlittingFlags blitFlags = DSBLIT_NOFX;
	DFBSurfaceDrawingFlags	drawFlags = DSDRAW_NOFX;

	blitFlags |= DSBLIT_BLEND_ALPHACHANNEL;
	drawFlags |= DSDRAW_BLEND;

	drawFlags = _ondk_surface_Blitflag_GetDrawFlagWidthCapa(drawFlags);
	_ondk_surface_Blitflag_SetPorterDuff_SRC(surface, blitFlags, drawFlags);
}

void ONDK_SURFACE_Blitflag_SetBlendWithVideo(ONDKSurfaceHandle surface)
{
	DFBSurfaceBlittingFlags blitFlags = DSBLIT_NOFX;
	DFBSurfaceDrawingFlags	drawFlags = DSDRAW_NOFX;

	blitFlags |= DSBLIT_BLEND_ALPHACHANNEL;
	drawFlags |= DSDRAW_BLEND;

	drawFlags = _ondk_surface_Blitflag_GetDrawFlagWidthCapa(drawFlags);
	_ondk_surface_Blitflag_SetPorterDuff_SRC(surface, blitFlags, drawFlags);
}


void ONDK_SURFACE_Blitflag_SetAlphaBlend( ONDKImageHandle surface, HUINT8 a)
{
    DFBResult			 	err;
	IDirectFBSurface	 	*dfbSurface = (IDirectFBSurface*)surface;

	DFBSurfaceBlittingFlags blitFlags = DSBLIT_NOFX;
	DFBSurfaceDrawingFlags 	drawFlags = DSDRAW_NOFX;

	if(0 == a)
	{
		return;
	}

	if(0xFF != a)
	{
		blitFlags |= DSBLIT_BLEND_COLORALPHA;
	}
	blitFlags |= DSBLIT_BLEND_ALPHACHANNEL;
	drawFlags |= DSDRAW_BLEND;

//	blitFlags |= DSBLIT_SRC_PREMULTIPLY;
//	blitFlags |= DSBLIT_DST_PREMULTIPLY;
	drawFlags |= DSDRAW_SRC_PREMULTIPLY;
//	drawFlags |= DSDRAW_DST_PREMULTIPLY;

	drawFlags = _ondk_surface_Blitflag_GetDrawFlagWidthCapa(drawFlags);
	_ondk_surface_Blitflag_SetPorterDuff_SRCOVER(surface, blitFlags, drawFlags);
	DFBCHECK(dfbSurface->SetColor(dfbSurface, 0, 0, 0, a));
}

void ONDK_SURFACE_Blitflag_SetColorBlend(ONDKSurfaceHandle surface, HUINT8 a, HUINT8 r, HUINT8 g, HUINT8 b)
{
    DFBResult			 	err;
	IDirectFBSurface	 	*dfbSurface = (IDirectFBSurface*)surface;

	DFBSurfaceBlittingFlags blitFlags = DSBLIT_NOFX;
	DFBSurfaceDrawingFlags 	drawFlags = DSDRAW_NOFX;

	if(0 == a)
	{
		return;
	}

	if(0xFF != a)
	{
		blitFlags |= DSBLIT_BLEND_COLORALPHA;
	}
	blitFlags |= DSBLIT_COLORIZE;
	blitFlags |= DSBLIT_BLEND_ALPHACHANNEL;
	drawFlags |= DSDRAW_BLEND;

//	blitFlags |= DSBLIT_SRC_PREMULTIPLY;
//	blitFlags |= DSBLIT_DST_PREMULTIPLY;
	drawFlags |= DSDRAW_SRC_PREMULTIPLY;
//	drawFlags |= DSDRAW_DST_PREMULTIPLY;

	drawFlags = _ondk_surface_Blitflag_GetDrawFlagWidthCapa(drawFlags);
	_ondk_surface_Blitflag_SetPorterDuff_SRCOVER(surface, blitFlags, drawFlags);
	DFBCHECK(dfbSurface->SetColor(dfbSurface, r, g, b, a));
}

void ONDK_SURFACE_Blitflag_SetBlend(ONDKSurfaceHandle surface)
{
	DFBSurfaceBlittingFlags blitFlags = DSBLIT_NOFX;
	DFBSurfaceDrawingFlags 	drawFlags = DSDRAW_NOFX;

#if 1//defined(CONFIG_APUI_NATIVE_APP)
	blitFlags |= DSBLIT_BLEND_ALPHACHANNEL;
	drawFlags |= DSDRAW_BLEND;
	drawFlags |= DSDRAW_SRC_PREMULTIPLY;
#else
	blitFlags |= DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY;
	drawFlags |= DSDRAW_BLEND;
	drawFlags |= DSDRAW_SRC_PREMULTIPLY | DSDRAW_SRC_PREMULTIPLY;
#endif

	drawFlags = _ondk_surface_Blitflag_GetDrawFlagWidthCapa(drawFlags);
	_ondk_surface_Blitflag_SetPorterDuff_SRCOVER(surface, blitFlags, drawFlags);
}


void	 ONDK_SURFACE_Destroy(ONDKSurfaceHandle surface)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	HxLOG_Debug("[%s:%d] (-) surface(0x%x) name(%s)\r\n", __FUNCTION__, __LINE__, surface, ONDK_GWM_APP_GetAppName(ONDK_GWM_APP_GetThis()));

	DFBCHECK(dfbSurface->Release(dfbSurface));
}

void ONDK_SURFACE_GetClip(ONDKSurfaceHandle surface, ONDK_Region_t *region)
{
	DFBResult			 err;
	DFBRegion			 dfbRegion;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	if(region== NULL )
	{
		HxLOG_Critical("NULL POINT!!\n");
		return;
	}

	DFBCHECK(dfbSurface->GetClip(dfbSurface, &dfbRegion));

	region->x1 = dfbRegion.x1;
	region->y1 = dfbRegion.y1;
	region->x2 = dfbRegion.x2;
	region->y2 = dfbRegion.y2;
}

void ONDK_SURFACE_GetClipElement(ONDKSurfaceHandle surface, HINT32 *x1, HINT32 *y1, HINT32 *x2, HINT32 *y2)
{
	DFBResult			 err;
	DFBRegion			 dfbRegion;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->GetClip(dfbSurface, &dfbRegion));

	*x1 = dfbRegion.x1;
	*y1 = dfbRegion.y1;
	*x2 = dfbRegion.x2;
	*y2 = dfbRegion.y2;
}


void ONDK_SURFACE_SetClip(ONDKSurfaceHandle surface, ONDK_Region_t region)
{
	DFBResult			err;
	const DFBRegion	 	dfbRegion = {region.x1, region.y1, region.x2, region.y2};
	IDirectFBSurface	*dfbSurface = (IDirectFBSurface*)surface;

	DFBCHECK(dfbSurface->SetClip(dfbSurface, &dfbRegion));
}


HUINT32*	 ONDK_SURFACE_Lock(ONDKSurfaceHandle _surface)
{
	DFBResult	 err;
	int	 pitch;
	void	 *addr;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, &addr, &pitch);
	if (err == DFB_LOCKED)
	{
		dfbSurface->Unlock(dfbSurface);
		err = dfbSurface->Lock(dfbSurface, DSLF_READ | DSLF_WRITE, &addr, &pitch);
	}

	if (err != DFB_OK)
	{
		HxLOG_Critical("Error : Fail to LOCK Surface....\n");
		return (HUINT32*)NULL;
	}

	return (HUINT32*)addr;
}

void	 ONDK_SURFACE_Unlock(ONDKSurfaceHandle _surface)
{
	DFBResult			err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;

	DFBCHECK(dfbSurface->Unlock(dfbSurface));
}


void ONDK_SURFACE_FlipBlit(ONDKSurfaceHandle surface, ONDK_Rect_t *rc)
{
	DFBResult err;
	DFBRegion region;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

	if (rc == NULL)
	{
		DFBCHECK(dfbSurface->Flip(dfbSurface, NULL, DSFLIP_BLIT));
	}
	else
	{
		region.x1 = rc->x;
		region.y1 = rc->y;
		region.x2 = rc->x + rc->w;
		region.y2 = rc->y + rc->h;

		DFBCHECK(dfbSurface->Flip(dfbSurface, &region, DSFLIP_BLIT));
	}
}

void ONDK_SURFACE_FlipWithForSync(ONDKSurfaceHandle surface, ONDK_Rect_t *rc)
{
	DFBResult err;
	DFBRegion region;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;

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

/**********************************************************************************************************/
#define _______BLIT_FUNTION_______________________
/**********************************************************************************************************/
ONDK_Result_t	ONDK_SURFACE_Init(HINT32 argc, HCHAR **argv)
{
	HxSTD_memset(&s_stDFBSurSysCapa, 0, sizeof(DFBSurface_SystemCapa_t));



	return ONDK_RET_OK;
}


void ONDK_SURFACE_Blit(ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, int x, int y)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;
	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};

	ONDK_SURFACE_Blitflag_SetBlend(destsurface);
	DFBCHECK(dfbSurface->Blit(dfbSurface, dfbSrcSurface, &srcDfbRect, x, y));
}

void ONDK_SURFACE_BlitWithoutBlend(ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, int x, int y)
{
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;
	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};

	DFBCHECK(dfbSurface->Blit(dfbSurface, dfbSrcSurface, &srcDfbRect, x, y));
}

void	 ONDK_SURFACE_StretchBlit( ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;

	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};
	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};

	ONDK_SURFACE_Blitflag_SetBlend(destsurface);
	DFBCHECK(dfbSurface->StretchBlit(dfbSurface, dfbSrcSurface, &srcDfbRect, &dstDfbRect));
}

void	 ONDK_SURFACE_StretchBlitWithoutBlend( ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;

	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, srcRect.w, srcRect.h};
	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};

	DFBCHECK(dfbSurface->StretchBlit(dfbSurface, dfbSrcSurface, &srcDfbRect, &dstDfbRect));
}

