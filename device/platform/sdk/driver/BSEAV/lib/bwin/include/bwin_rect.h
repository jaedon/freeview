/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_rect.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 9:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/include/bwin_rect.h $
 * 
 * 1   2/7/05 9:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   9/21/04 2:46p erickson
 * PR12704: added std __cplusplus wrapper for use with C++
 * 
 * Irvine_BSEAVSW_Devel/3   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/2   1/19/04 8:59a erickson
 * PR9107: switching to BRUTUS_PR9107 branch
 * 
 * Irvine_BSEAVSW_Devel/1   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 ****************************************************************/
#ifndef BWIN_RECT_H__
#define BWIN_RECT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
A rectangle.
Description:
If both width and height are 0, the rectangle is considered to be NULL.
If either width or height are 0, bwin_rect describes a line.
**/
typedef struct {
	int x; /* can be < 0 */
	int y; /* can be < 0 */
	unsigned width;
	unsigned height;
} bwin_rect;

/**
Summary:
Test if two rectangles are equal.
**/
#define BWIN_RECT_ISEQUAL(PRECT1, PRECT2)( \
	(PRECT1)->x == (PRECT2)->x && \
	(PRECT1)->y == (PRECT2)->y && \
	(PRECT1)->width == (PRECT2)->width && \
	(PRECT1)->height == (PRECT2)->height)

#define BWIN_RECT_ISINSIDE(PRECT_OUTSIDE, PRECT_INSIDE) (\
	(PRECT_OUTSIDE)->x <= (PRECT_INSIDE)->x && \
	(PRECT_OUTSIDE)->y <= (PRECT_INSIDE)->y && \
	(PRECT_OUTSIDE)->x+(int)(PRECT_OUTSIDE)->width >= (PRECT_INSIDE)->x+(int)(PRECT_INSIDE)->width && \
	(PRECT_OUTSIDE)->y+(int)(PRECT_OUTSIDE)->height >= (PRECT_INSIDE)->y+(int)(PRECT_INSIDE)->height)

#define BWIN_RECT_ISINTERSECTING(PRECT1, PRECT2) (\
	(PRECT1)->x < (PRECT2)->x+(int)(PRECT2)->width && \
	(PRECT1)->y < (PRECT2)->y+(int)(PRECT2)->height && \
	(PRECT1)->x+(int)(PRECT1)->width > (PRECT2)->x && \
	(PRECT1)->y+(int)(PRECT1)->height > (PRECT2)->y)

/**
Summary:
Assign the members of the rectangle.
**/
#define BWIN_SET_RECT(PRECT,X,Y,W,H) \
	do {(PRECT)->x = X; (PRECT)->y = Y; (PRECT)->width = W; (PRECT)->height = H;} while(0)

/**
Summary:
Test is a point is located inside a rectangle.
**/
#define BWIN_POINT_IN_RECT(X,Y,PRECT) (\
	(X)>=(PRECT)->x && \
	(Y)>=(PRECT)->y && \
	(X)<(PRECT)->x + (int)(PRECT)->width && \
	(Y)<=(PRECT)->y + (int)(PRECT)->height)

/**
Summary:
Find the intersection of two rectangles.
Description:
If the rectangles do not intersect, dest will be all 0's.

Beware when calculating the intersection of a rectangle and a line (a line has either
width or height of zero, but not both). This algorithm will not elimnate the rightmost
or bottommost line. See the implementatin of bwin_draw_line for an example of how
to deal with this.
**/
void
bwin_intersect_rect(bwin_rect *dest, const bwin_rect *s1, const bwin_rect *s2);

/**
Summary:
Find the union of two rectangles.
**/
void
bwin_union_rect(bwin_rect *dest, const bwin_rect *s1, const bwin_rect *s2);

#ifdef __cplusplus
}
#endif

#endif /* BWIN_RECT_H__ */
