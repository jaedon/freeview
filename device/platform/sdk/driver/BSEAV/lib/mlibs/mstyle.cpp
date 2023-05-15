/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mstyle.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/31/06 4:16p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mstyle.cpp $
 * 
 * 4   5/31/06 4:16p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   4/17/06 5:05p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   2/28/06 1:22p tokushig
 * change dropshadow color to black
 * 
 * SanDiego_Brutus_Skin/1   1/11/06 11:15a tokushig
 * using text_focused color for list box so adjusted color value to
 * maintain the current brutus appearance.
 * 
 * 2   2/15/05 5:22p erickson
 * PR13387: added palette support by converting the color array into
 * indexes and outputing a palette for external use
 * 
 ********************************************************/
#include "mstyle.h"
#include "bstd.h"
#include "bkni.h"

MStyle::MStyle() {
    _colors[BUTTON_FACE] = 0xff282277;
    _colors[BUTTON_LEFTBEVEL] = 0xff8080d0;
    _colors[BUTTON_TOPBEVEL] = 0xff7070c0;
    _colors[BUTTON_RIGHTBEVEL] = 0xff11112d;
    _colors[BUTTON_BOTTOMBEVEL] = 0xff303040;
    _colors[TEXT] = 0xff999999;
    _colors[CHECKBOX_BACKGROUND] = 0xff999999;
    _colors[BEVEL_OUTER] = 0xff333333;
    _colors[BEVEL_INNER] = 0xff666666;
    _colors[CHECKBOX_CHECK] = 0xff222222;
    _colors[FOCUS] = 0xffAAAA00;
    _colors[MESSAGEBOX_BACKGROUND] = 0xff373777;
    _colors[MESSAGEBOX_BORDER] = 0xff232363;
    _colors[MESSAGEBOX_TEXT] = 0xff131353;
    _colors[LABEL_DROPSHADOW] = 0xff000000;
    _colors[LINEEDIT_FOCUSBORDER] = 0xff333333;
    _colors[LINEEDIT_TEXT] = 0xff000000;
    _colors[LINEEDIT_BACKGROUND] = 0xff999999;
    _colors[BUTTON_FACE_FOCUSED] = 0xff7070a0;
    _colors[TEXT_FOCUSED] = 0xff999999; //0xff505020;
    _colors[BACKGROUND] = 0xff373755;
    _colors[TRANSPARENT] = 0xff00ff00;

    /* same by default */
    BKNI_Memcpy(&_disabledColors, &_colors, sizeof(_disabledColors));
    _disabledColors[BUTTON_FACE] = 0xff181227;
    _disabledColors[BUTTON_LEFTBEVEL] = 0xff404060;
    _disabledColors[BUTTON_TOPBEVEL] = 0xff303050;
    _disabledColors[BUTTON_RIGHTBEVEL] = 0xff11112d;
    _disabledColors[BUTTON_BOTTOMBEVEL] = 0xff202030;
    _disabledColors[TEXT] = 0xff666666;
    _disabledColors[CHECKBOX_BACKGROUND] = 0xff444444;
    _disabledColors[BEVEL_OUTER] = 0xff111111;
    _disabledColors[BEVEL_INNER] = 0xff444444;
    _disabledColors[CHECKBOX_CHECK] = 0xff333333;
    _disabledColors[FOCUS] = 0xff888888; /* disabled focus? */
    _disabledColors[MESSAGEBOX_BACKGROUND] = 0xff373777;
    _disabledColors[MESSAGEBOX_BORDER] = 0xff232363;
    _disabledColors[MESSAGEBOX_TEXT] = 0xff131353;
    _disabledColors[LABEL_DROPSHADOW] = 0xff000000;
    _disabledColors[LINEEDIT_FOCUSBORDER] = 0xff333333;
    _disabledColors[LINEEDIT_TEXT] = 0xff111111;
    _disabledColors[LINEEDIT_BACKGROUND] = 0xff666666;

    _props[CHECKBOX_FOCUS_WIDTH] = 30;
    _props[CHECKBOX_WIDTH] = 20;
    _props[RADIOBUTTON_FOCUS_WIDTH] = 30;
    _props[RADIOBUTTON_WIDTH] = 20;
    _props[BUTTON_ROUNDING] = 2;
}

void MStyle::convertToPalette(unsigned long **palette, int *num)
{
    unsigned long *pixarray = (unsigned long *)BKNI_Malloc(
        TOTAL_COLORINDEX * 2 * sizeof(unsigned long));
    for (int i=0;i<TOTAL_COLORINDEX;i++) {
        pixarray[i] = _colors[i];
    }
    for (int i=0;i<TOTAL_COLORINDEX;i++) {
        pixarray[i+TOTAL_COLORINDEX] = _disabledColors[i];
    }
    
    /* return it */
    *palette = pixarray;
    *num = TOTAL_COLORINDEX * 2;
    
    /* now convert our arrays to the palette indexes */
    for (int i=0;i<TOTAL_COLORINDEX*2;i++) {
        _colors[i] = i;
    }
    for (int i=0;i<TOTAL_COLORINDEX;i++) {
        _disabledColors[i] = i+TOTAL_COLORINDEX;
    }
}

void MStyle::freePalette(unsigned long *palette)
{
    BKNI_Free(palette);
}
