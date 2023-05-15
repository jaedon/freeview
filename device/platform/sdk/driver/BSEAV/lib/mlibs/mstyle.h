/***************************************************************************
 *     Copyright (c) 2002-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mstyle.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/15/05 5:22p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mstyle.h $
 * 
 * 2   2/15/05 5:22p erickson
 * PR13387: added palette support by converting the color array into
 * indexes and outputing a palette for external use
 * 
 * 1   2/7/05 11:19p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   3/4/04 11:31a erickson
 * PR9023: support all lineedit colors needed
 * 
 * Irvine_BSEAVSW_Devel/2   12/10/03 1:33p erickson
 * PR8813: added a couple new colors and the std header
 *
 *********************************************/
#ifndef MSTYLE_H
#define MSTYLE_H

class MStyle {
public:
	MStyle();

	enum ColorIndex {
		BUTTON_FACE,
		BUTTON_FACE_FOCUSED,
		BUTTON_LEFTBEVEL,
		BUTTON_TOPBEVEL,
		BUTTON_RIGHTBEVEL,
		BUTTON_BOTTOMBEVEL,
		TEXT,
		TEXT_FOCUSED,
		CHECKBOX_BACKGROUND,
		BEVEL_INNER,
		BEVEL_OUTER,
		CHECKBOX_CHECK,
		FOCUS,
		MESSAGEBOX_BACKGROUND,
		MESSAGEBOX_BORDER,
		MESSAGEBOX_TEXT,
		LABEL_DROPSHADOW,
		LINEEDIT_FOCUSBORDER,
		LINEEDIT_TEXT,
		LINEEDIT_BACKGROUND,
		BACKGROUND,
		TRANSPARENT,
		TOTAL_COLORINDEX
	};

	void set(ColorIndex index, int color, bool enabled = true) {
		(enabled?_colors[index]:_disabledColors[index]) = color;
	}
	int color(ColorIndex index, bool enabled = true) const {
		return enabled?_colors[index]:_disabledColors[index];
	}
	
	/* If using palettized framebuffer, this will build a palette
	using the current colors, then convert all colors to their indexes.
	This provides seamless color support.
	This is a one-way operation. */
	void convertToPalette(unsigned long **palette, int *num);
	void freePalette(unsigned long *palette);

	enum PropIndex {
		CHECKBOX_FOCUS_WIDTH,
		CHECKBOX_WIDTH,
		RADIOBUTTON_FOCUS_WIDTH,
		RADIOBUTTON_WIDTH,
		BUTTON_ROUNDING,
		TOTAL_PROPINDEX
	};

	void set(PropIndex index, int value) {_props[index] = value;}
	int prop(PropIndex index) const {return _props[index];}


protected:
	int _colors[TOTAL_COLORINDEX];
	int _disabledColors[TOTAL_COLORINDEX];
	int _props[TOTAL_PROPINDEX];
};

#endif // MSTYLE_H
