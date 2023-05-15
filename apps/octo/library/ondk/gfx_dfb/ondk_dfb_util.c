/**************************************************************
*	@file		ondk_dfb_util.c
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

ONDK_GFX_t  ONDK_DFBUTIL_GFX_Create(HUINT32 applId, ONDK_Rect_t rt)
{
	return ONDK_DFBUTIL_GFX_CreateEx(applId, rt, "ARGB8888");
}

ONDK_GFX_t  ONDK_DFBUTIL_GFX_CreateEx(HUINT32 applId, ONDK_Rect_t rt, HCHAR *pixelformat)
{
	ONDK_GFX_t 			gfx;
	DFBResult 			err;
	IDirectFBSurface	*dfbSurface;
	IDirectFBWindow	*dfbWindow;

	gfx.window = ONDK_WINDOW_Create(applId, rt.x, rt.y, rt.w, rt.h, pixelformat, TRUE);
	HxLOG_Print(HxANSI_COLOR_YELLOW("(x,y,w,h)=(%d,%d,%d,%d), pixelformat = %s\n"), rt.x, rt.y, rt.w, rt.h, pixelformat);
	ONDK_WINDOW_SetShow(gfx.window, FALSE);
	ONDK_WINDOW_Update(gfx.window, NULL);
	ONDK_WINDOW_Clear(gfx.window);

	//dfbSurface = ONDK_SURFACE_CreatePrimary(rt.w, rt.h, "ARGB8888");
	dfbWindow = gfx.window;
	DFBCHECK(dfbWindow->GetSurface(dfbWindow , &dfbSurface));
	gfx.surface = dfbSurface;
	ONDK_SURFACE_Clear(gfx.surface);

	return gfx;
}



