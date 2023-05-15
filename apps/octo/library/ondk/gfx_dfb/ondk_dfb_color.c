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

/**
 * default : ARGB8888, DI_OSD_RASTER_OP_SRCOVER,				///< C(d) = C(s)*A(s) + C(d) * (1 - A(s))
 **/
#define ONDK_COL_DIV255(x)			((x + (((x + 128) >> 8) + 128)) >> 8)


#define ONDK_COL_AlphaBlend_RB(bg, fg, fg_a)			(((((fg & 0x00ff00ff) * fg_a) + ((bg & 0x00ff00ff) * (0xff-fg_a))) & 0xff00ff00) >> 8)
#define ONDK_COL_AlphaBlend_G(bg, fg, fg_a)				(((((fg & 0x0000ff00) * fg_a) + ((bg & 0x0000ff00) * (0xff-fg_a))) & 0x00ff0000) >> 8)
#define ONDK_COL_AlphaBlend_A(bg_a, fg_a)				(fg_a+bg_a-ONDK_COL_DIV255(fg_a*bg_a))
#define ONDK_COL_AlphaBlend(bg, fg, bg_a, fg_a)			((ONDK_COL_AlphaBlend_A(bg_a,fg_a) << 24) | ONDK_COL_AlphaBlend_RB(bg, fg, fg_a) | ONDK_COL_AlphaBlend_G(bg, fg, fg_a))



typedef struct _HumaxDFBPixelFormatName {
     DFBSurfacePixelFormat format;
     const char *name;
} HumaxDFBPixelFormatName_t;
#if 0
static HumaxDFBPixelFormatName_t s_Identifier[] = {
	 /* DirectFB */
	{ DSPF_ARGB1555, "ARGB1555" },
	{ DSPF_RGB16, "RGB16" },
	{ DSPF_RGB24, "RGB24" },
	{ DSPF_RGB32, "RGB32" },
	{ DSPF_ARGB, "ARGB" },
	{ DSPF_A8, "A8" },
	{ DSPF_YUY2, "YUY2" },
	{ DSPF_RGB332, "RGB332" },
	{ DSPF_UYVY, "UYVY" },
	{ DSPF_I420, "I420" },
	{ DSPF_YV12, "YV12" },
	{ DSPF_LUT8, "LUT8" },
	{ DSPF_ALUT44, "ALUT44" },
	{ DSPF_AiRGB, "AiRGB" },
	{ DSPF_A1, "A1" },
	{ DSPF_NV12, "NV12" },
	{ DSPF_NV16, "NV16" },
	{ DSPF_ARGB2554, "ARGB2554" },
	{ DSPF_ARGB4444, "ARGB4444" },
	{ DSPF_RGBA4444, "RGBA4444" },
	{ DSPF_NV21, "NV21" },
	{ DSPF_AYUV, "AYUV" },
	{ DSPF_A4, "A4" },
	{ DSPF_ARGB1666, "ARGB1666" },
	{ DSPF_ARGB6666, "ARGB6666" },
	{ DSPF_RGB18, "RGB18" },
	{ DSPF_LUT2, "LUT2" },
	{ DSPF_RGB444, "RGB444" },
	{ DSPF_RGB555, "RGB555" },
	{ DSPF_BGR555, "BGR555" },
	{ DSPF_RGBA5551, "RGBA5551" },
	{ DSPF_YUV444P, "YUV444P" },
	{ DSPF_ARGB8565, "ARGB8565" },
	{ DSPF_AVYU, "AVYU" },
	{ DSPF_VYU, "VYU" },
	{ DSPF_A1_LSB, "A1_LSB" },
	{ DSPF_YV16, "YV16" },
	{ DSPF_LUT4, "LUT4" },
	{ DSPF_ABGR, "ABGR" },
	{ DSPF_ALUT8, "ALUT8" },
	{ DSPF_UNKNOWN, "UNKNOWN" }

	/* HUMAX */
	,{ DSPF_ARGB, "ARGB8888" }

	,{ DSPF_UNKNOWN, NULL }
};

static HxHASH_t *s_hashTable = NULL;

#endif
/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/




ONDK_Color_t  ONDK_COLOR_Blend(ONDK_Color_t fg, ONDK_Color_t bg)
{
	switch(COL_A(fg)){
	case 00:
		return bg;

	case 0xff:
		return fg;

	default:
		break;
	}

	return ONDK_COL_AlphaBlend(bg, fg, COL_A(bg), COL_A(fg));
}


ONDK_Color_t  ONDK_COLOR_BlendWithAlpha(ONDK_Color_t fg, ONDK_Color_t bg, HUINT8 fg_alpha)
{
#if 0
	switch(fg_alpha){
	case 00:
		return bg;

	case 0xff:
		return fg;

	default:
		break;
	}

	return ONDK_COL_AlphaBlend(bg, fg, COL_A(bg), fg_alpha);
#else
	HUINT32		fg_RB, fg_AG, bg_RB, bg_AG;
	HUINT8  	a, r, g, b;
	HUINT32		a0, a1;

	if (fg_alpha == 0)
		return  bg;
	if (fg_alpha == 0xFF)
		return  fg;

	fg_RB = (fg & 0x00FF00FF);
	fg_AG = (fg & 0xFF00FF00)>>8;
	bg_RB = (bg & 0x00FF00FF);
	bg_AG = (bg & 0xFF00FF00)>>8;

	a0 = fg_alpha+1;
	a1 = 255-fg_alpha;
	fg_RB = fg_RB*a0+bg_RB*a1;
	fg_AG = fg_AG*a0+bg_AG*a1;

	a = (fg_AG>>24)&0xFF;
	r = (fg_RB>>24)&0xFF;
	g = (fg_AG>>8)&0xFF;
	b = (fg_RB>>8)&0xFF;

	return  ((a<<24)|(r<<16)|(g<<8)|(b));
#endif
}


HINT32	ONDK_GetPixelFormatByString(HCHAR *pixelFormat)
{
	return 0;
}



ONDK_Result_t	ONDK_DFB_COLOR_Init(HINT32 argc, HCHAR **argv)
{
#if 0
	HINT32 i = 0;

	s_hashTable = HLIB_HASH_SelfNew();
	ONDK_Assert(s_hashTable);

	for(i=0; s_Identifier[i].name != NULL; i++){

	}


#endif
	return ONDK_RET_OK;
}


