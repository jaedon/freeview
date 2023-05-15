/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_rect.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 9:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_rect.c $
 * 
 * 1   2/7/05 9:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/2   1/19/04 9:02a erickson
 * PR9107: getting ready for BRUTUS_PR9107 branch
 *
 * Irvine_BSEAVSW_Devel/1   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 ****************************************************************/
#include "bwin.h"
#include "bwin_rect.h"
#include "bwin_priv.h"

void printrect(const char *name, const bwin_rect *r)
{
	printf("%s (%d,%d,%d,%d)\n", name, r->x,r->y,r->width,r->height);
}

/* This function allows s1 or s2 to point to the same memory as dest. In place union. */
void
bwin_union_rect(bwin_rect *dest, const bwin_rect *s1, const bwin_rect *s2)
{
	if (s1->width == 0 || s1->height == 0) {
		*dest = *s2;
	}
	else if (s2->width == 0 || s2->height == 0) {
		*dest = *s1;
	}
	else {
		int r,b,x,y;
		x = min(s1->x,s2->x);
		y = min(s1->y,s2->y);
		r = max(s1->x + s1->width, s2->x + s2->width);
		b = max(s1->y + s1->height, s2->y + s2->height);
		dest->x = x;
		dest->y = y;
		dest->width = r - x;
		dest->height = b - y;
	}
}

/* This function allows s1 or s2 to point to the same memory as dest. In place intersection. */
void
bwin_intersect_rect(bwin_rect *dest, const bwin_rect *s1, const bwin_rect *s2)
{
	int r,b,x,y;
	x = max(s1->x, s2->x);
	y = max(s1->y, s2->y);
	r = min(s1->x + (int)s1->width, s2->x + (int)s2->width);
	b = min(s1->y + (int)s1->height, s2->y + (int)s2->height);
	if (r >= x && b >= y) {
		dest->x = x;
		dest->y = y;
		dest->width = r - x;
		dest->height = b - y;
	}
	else {
		dest->x = 0;
		dest->y = 0;
		dest->width = 0;
		dest->height = 0;
	}
#if 0
	printrect("inter", dest);
	printrect("     ", s1);
	printrect("     ", s2);
#endif
}
