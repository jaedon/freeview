/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mgeom.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:11p $
 *
 * Module Description: General purpose geometry classes
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mgeom.cpp $
 * 
 * 1   2/7/05 11:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   1/29/04 2:25p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 ***************************************************************************/

#include "mgeom.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

bool MRect::intersects(const MRect &rect) const {
	return
		rect.right() >= x() &&
		rect.bottom() >= y() &&
		rect.x() <= right() &&
		rect.y() <= bottom();
}

MRect MRect::intersection(const MRect &rect) const {
	int xx = max(x(), rect.x());
	int yy = max(y(), rect.y());
	int r = min(right(), rect.right());
	int b = min(bottom(), rect.bottom());
	if (r <= xx || b <= yy)
		return MRect();
	else
		return MRect(xx,yy,r - xx,b - yy);
}

MRect MRect::combine(const MRect &rect) const {
	int xx = min(x(), rect.x());
	int yy = min(y(), rect.y());
	return MRect(xx,yy,
		max(right(), rect.right()) - xx,
		max(bottom(), rect.bottom()) - yy);
}

MRect MRect::minus(const MRect &rect) const {
	MRect r = intersection(rect);
	if (r.isNull())
		return *this;

	// test where rect is "bigger" than this
	int tx = (rect.x()<=x())?1:0;
	int ty = (rect.y()<=y())?1:0;
	int tr = (rect.right()>=right())?1:0;
	int tb = (rect.bottom()>=bottom())?1:0;
	int sum = tx+ty+tr+tb;

	if (sum == 3) {
		int xx = tr?x():rect.right();
		int yy = tb?y():rect.bottom();
		return MRect(
			xx,
			yy,
			(tx?right():rect.x()) - xx,
			(ty?bottom():rect.y()) - yy);
	}
	else if (sum == 4) {
		return MRect();
	}
	else
		return *this;
}

void MRect::grow(int amount) {
	_width += amount;
	_height += amount;
}

bool MRect::contains(const MRect &rect) const {
	return
		x() <= rect.x() &&
		y() <= rect.y() &&
		right() >= rect.right() &&
		bottom() >= rect.bottom();

}

