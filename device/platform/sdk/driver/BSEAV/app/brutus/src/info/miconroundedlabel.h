/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: miconroundedlabel.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/31/06 4:20p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/miconroundedlabel.h $
 * 
 * 4   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   2/14/06 12:50p tokushig
 * removed local storage of textfocuscolor...using version in mwidget
 * instead
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:40p tokushig
 * added ability to use an image for the focus highlight.  this may need
 * to be pushed down lower into mwidget later.
 * 
 * 2   4/18/05 10:56a erickson
 * PR14593: prerender images for faster draw time on slow platforms
 * 
 * 1   2/7/05 8:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 ****************************************************************************/
#ifndef MICONROUNDEDLABEL_H__
#define MICONROUNDEDLABEL_H__

#include "mlabel.h"

/**
* Flat label with rounded edges and optional icon.
* If no icon, it behaves like a regular label.
* The MWidget background applies to the corners beyond the rounding.
**/
class MIconRoundedLabel : public MLabel {
public:
    MIconRoundedLabel(MWidget *parent, const MRect &rect, const char *text = NULL,
        const MImage *icon = NULL);
    ~MIconRoundedLabel();

    /* The background of the rounded panel. If -1, no panel. */
    void setPanelBackgroundColor(int bgcolor) {_bgcolor = bgcolor;repaint();}
    void focusRepaint() {repaint();}
    
    void setImageFocus(MImage * img) { _imageFocus = img; };

    /* Convert from MImage to MPixmap. This causes the image to be rendered once
    and stored in memory. The caller may deallocate the icon after this is called. */
    void prerender();
    
protected:
    const MImage *_icon;
    MPixmap *_pixmap;
    int _bgcolor;
    MImage *_imageFocus;
    void draw(const MRect &cliprect);
};

#endif //MICONROUNDEDLABEL_H__
