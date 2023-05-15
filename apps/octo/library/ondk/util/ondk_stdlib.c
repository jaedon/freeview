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
#include   <ondk_gfx.h>






/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/




ONDK_Color_t  ONDK_Color(HUINT8 a, HUINT8 r, HUINT8 g, HUINT8 b)
{
//#define ONDK_Color(a,r,g,b)		((a&0xff<<24) | (r&0xff<<16) | (g&0xff<<8) | (b&0xff))
//#define ONDK_Color(a,r,g,b)		((a&0xff) | (r&0xff<<8) | (g&0xff<<16) | (b&0xff<<24))

	return (ONDK_Color_t)((a << 24) | (r << 16) | (g << 8) | (b));
}

ONDK_Size_t	 ONDK_Size(HINT32 w, HINT32 h)
{
	ONDK_Size_t	 s;

	s.w = w;
	s.h = h;

	return s;
}

ONDK_Point_t ONDK_Point(HINT32 x, HINT32 y)
{
	ONDK_Point_t	 p;

	p.x = x;
	p.y = y;

	return p;
}


ONDK_Rect_t	 ONDK_Rect(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	ONDK_Rect_t	 r;

	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	return r;
}

ONDK_Region_t	 ONDK_Region(HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2)
{
	ONDK_Region_t r;

	r.x1 = x1;
	r.y1 = y1;
	r.x2 = x2;
	r.y2 = y2;

	return r;
}

ONDK_Rect_t	 ONDK_RegionToRect(ONDK_Region_t rn)
{
	ONDK_Rect_t rt;

	rt.x = rn.x1;
	rt.y = rn.y1;
	rt.w = rn.x2 - rn.x1 + 1;
	rt.h = rn.y2 - rn.y1 + 1;

	return rt;
}

ONDK_Region_t ONDK_RectToRegion(ONDK_Rect_t rt)
{
	ONDK_Region_t rn;

	rn.x1 = rt.x;
	rn.y1 = rt.y;
	rn.x2 = rt.x + rt.w - 1;
	rn.y2 = rt.y + rt.h - 1;

	return rn;
}


HBOOL ONDK_IsInnerRegion(ONDK_Region_t* r1, ONDK_Region_t* r2)
{
	if (r2->x2 >= r1->x1 && r2->x2 <= r1->x2 &&
		r2->y2 >= r1->y1 && r2->y2 <= r1->y2)
	{
		return TRUE;
	}
	return FALSE;
}

HBOOL ONDK_IsInnerRect(ONDK_Rect_t* r1, ONDK_Rect_t* r2)
{
	if (   r1->x <= r2->x
		&& r1->y <= r2->y
		&& (/*ex1*/r1->x + r1->w - 1) >= (/*ex2*/r2->x + r2->w - 1)
		&& (/*ey1*/r1->y + r1->h - 1) >= (/*ey2*/r2->y + r2->h - 1))
	{
		return TRUE;
	}
	return FALSE;
}

HBOOL ONDK_CmpRect(ONDK_Rect_t* r1, ONDK_Rect_t* r2)
{
	if (   r1->x == r2->x
		&& r1->y == r2->y
		&& (/*ex1*/r1->x + r1->w - 1) == (/*ex2*/r2->x + r2->w - 1)
		&& (/*ey1*/r1->y + r1->h - 1) == (/*ey2*/r2->y + r2->h - 1))
	{
		return TRUE;
	}
	return FALSE;
}



ONDK_Rect_t	 ONDK_UnionRect(const ONDK_Rect_t *r1, const ONDK_Rect_t *r2)
{
	ONDK_Rect_t	 rc;

	if (ONDK_IsEmptyRect(r1))
	{
		rc = *r2;
		return rc;
	}

	if (ONDK_IsEmptyRect(r2))
	{
		rc = *r1;
		return rc;
	}

	rc.x	 = HxMACRO_MIN(r1->x, r2->x);
	rc.y	 = HxMACRO_MIN(r1->y, r2->y);
	rc.w	 = HxMACRO_MAX(/*ex1*/r1->x + r1->w - 1, /*ex2*/r2->x + r2->w - 1) - rc.x + 1;
	rc.h	 = HxMACRO_MAX(/*ey1*/r1->y + r1->h - 1, /*ey2*/r2->y + r2->h - 1) - rc.y + 1;

	return rc;
}

ONDK_Region_t	 ONDK_UnionRegion(const ONDK_Region_t *r1, const ONDK_Region_t *r2)
{
	ONDK_Region_t	 rc;

	if (ONDK_IsEmptyRegion(r1))
	{
		rc = *r2;
		return rc;
	}

	if (ONDK_IsEmptyRegion(r2))
	{
		rc = *r1;
		return rc;
	}

	rc.x1	 = HxMACRO_MIN(r1->x1, r2->x1);
	rc.y1	 = HxMACRO_MIN(r1->y1, r2->y1);
	rc.x2	 = HxMACRO_MAX(r1->x2, r2->x2);
	rc.y2	 = HxMACRO_MAX(r1->y2, r2->y2);

	return rc;
}


ONDK_Rect_t	ONDK_IntersectRect (const ONDK_Rect_t *r1, const ONDK_Rect_t *r2)
{
	ONDK_Rect_t	rc;

	rc.x	= HxMACRO_MAX(r1->x, r2->x);
	rc.y	= HxMACRO_MAX(r1->y, r2->y);
	rc.w	= HxMACRO_MIN(/*ex1*/r1->x + r1->w - 1, /*ex2*/r2->x + r2->w - 1) - rc.x + 1;
	rc.h	= HxMACRO_MIN(/*ey1*/r1->y + r1->h - 1, /*ey2*/r2->y + r2->h - 1) - rc.y + 1;

	return rc;
}

ONDK_Region_t	ONDK_IntersectRegion (const ONDK_Region_t *r1, const ONDK_Region_t *r2)
{
	ONDK_Region_t	rc;

	rc.x1	= HxMACRO_MAX(r1->x1, r2->x1);
	rc.y1	= HxMACRO_MAX(r1->y1, r2->y1);
	rc.x2	= HxMACRO_MIN(r1->x2, r2->x2);
	rc.y2	= HxMACRO_MIN(r1->y2, r2->y2);

	return rc;
}


HBOOL		ONDK_IsOverlapped (const ONDK_Rect_t *r1, const ONDK_Rect_t *r2)
{
	HINT32	ex1	= r1->x + r1->w - 1;
	HINT32	ey1 = r1->y + r1->h - 1;
	HINT32	ex2 = r2->x + r2->w - 1;
	HINT32	ey2 = r2->y + r2->h - 1;

	if (r1->x > ex2 || ex1 < r2->x || r1->y > ey2 || ey1 < r2->y)
	{
		return FALSE;
	}
	return TRUE;
}


HBOOL		ONDK_IsOverlappedRegion (const ONDK_Region_t *r1, const ONDK_Region_t *r2)
{
	if (r1->x1 > r2->x2 ||
		r1->y1 > r2->y2 ||
		r1->x2 < r2->x1 ||
		r1->y2 < r2->y1)
	{
		return FALSE;
	}
	return TRUE;
}


HBOOL	 ONDK_IsEmptyRect(const ONDK_Rect_t *r)
{
	if ((r->w <= 0) || (r->h <= 0))
		return TRUE;

	return FALSE;
}


HBOOL	ONDK_IsEmptyRegion(const ONDK_Region_t *r)
{
	if ((r->x2 == 0) || (r->y2 == 0) || (r->x1 >= r->x2) || (r->y1 >= r->y2))
		return TRUE;

	return FALSE;

}

void	 ONDK_CopyRect(ONDK_Rect_t *dst, ONDK_Rect_t *src)
{
	HxSTD_MemCopy(dst, src, sizeof(ONDK_Rect_t));
}


ONDK_Result_t  ONDK_STDLIB_Init(HINT32 argc, HCHAR **argv){

	return ONDK_RET_OK;
}

