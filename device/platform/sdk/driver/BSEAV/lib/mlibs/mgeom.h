/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mgeom.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:11p $
 *
 * Module Description: General purpose geometry classes
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mgeom.h $
 * 
 * 1   2/7/05 11:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   1/29/04 2:25p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * \main\SanJose_Linux_Devel\7   3/8/02 1:51p erickson
 * Added == and != operators for MRect
 *
 ***************************************************************************/

#ifndef MGEOM_H
#define MGEOM_H

class MRect {
public:
	MRect() {set(0,0,0,0);}
	MRect(int x, int y, unsigned w, unsigned h) {set(x,y,w,h);}

	void set(int x, int y, unsigned w, unsigned h) {
		_x = x;
		_y = y;
		_width = w;
		_height = h;
	}
	int x() const {return _x;}
	int y() const {return _y;}
	unsigned width() const {return _width;}
	unsigned height() const {return _height;}
	// derived
	int right() const {return x()+(int)width();}
	int bottom() const {return y()+(int)height();}
	int midX() const {return x()+(int)width()/2;}
	int midY() const {return y()+(int)height()/2;}

	void setX(int x) {_x = x;}
	void setY(int y) {_y = y;}
	void setRight(int r) {_width = r-_x;}
	void setBottom(int b) {_height = b-_y;}
	void setWidth(unsigned w) {_width = w;}
	void setHeight(unsigned h) {_height = h;}

	void setSize(unsigned w, unsigned h) {_width = w; _height = h;}
	void moveTo(int x, int y) {_x = x; _y = y;}
	void moveBy(int dx, int dy) {_x += dx; _y += dy;}

	bool isNull() {return _width <= 0 || _height <= 0;}

	/**
	* Returns true if the rectangles intersect
	*/
	bool intersects(const MRect &rect) const;

	/**
	* Returns the rectangle of intersection
	*/
	MRect intersection(const MRect &rect) const;

	bool contains(const MRect &rect) const;

	/**
	* Returns the small rectangle that contains both rectangles.
	*/
	MRect combine(const MRect &rect) const;

	/**
	* Returns the smallest rectangle that contains what's left
	* after substracting rect.
	*/
	MRect minus(const MRect &rect) const;

	/**
	* Increase both width and height. If amount<0, then it shrinks the MRect.
	*/
	void grow(int amount);

protected:
	int _x, _y;
	unsigned _width, _height;
};

inline bool operator == (const MRect &r1, const MRect &r2) {
	return
		r1.x() == r2.x() &&
		r1.y() == r2.y() &&
		r1.width() == r2.width() &&
		r1.height() == r2.height();
}

inline bool operator != (const MRect &r1, const MRect &r2) {
	return !(r1 == r2);
}

class MPoint {
public:
	MPoint() {_x = _y = 0;}
	MPoint(int x, int y) {_x = x; _y = y;}
	int x() const {return _x;}
	int y() const {return _y;}
	void setX(int x) {_x = x;}
	void setY(int y) {_y = y;}

protected:
	int _x, _y;
};

inline MPoint operator +(const MPoint &p1, const MPoint &p2) {
	return MPoint(p1.x()+p2.x(),p1.y()+p2.y());
}
inline MPoint operator -(const MPoint &p1, const MPoint &p2) {
	return MPoint(p1.x()-p2.x(),p1.y()-p2.y());
}

class MSize {
public:
	MSize() {_width = _height = 0;}
	MSize(unsigned w, unsigned h) {_width = w; _height = h;}
	unsigned width() const {return _width;}
	unsigned height() const {return _height;}
	void setWidth(unsigned w) {_width = w;}
	void setHeight(unsigned h) {_height = h;}

protected:
	unsigned _width, _height;
};

#endif //MGEOM_H
