/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: miconroundedlabel.cpp $
 * $brcm_Revision: 5 $
 * $brcm_Date: 5/31/06 4:20p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/miconroundedlabel.cpp $
 * 
 * 5   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 4   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   2/14/06 12:50p tokushig
 * removed local storage of textfocuscolor...using version in mwidget
 * instead
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:39p tokushig
 * added ability to use an image for the focus highlight.  this may need
 * to be pushed down lower into mwidget later.
 * 
 * 3   10/3/05 11:19a erickson
 * PR17108: fixed linux 2.6 compilation warnings
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
#include "miconroundedlabel.h"
#include "bstd.h"
BDBG_MODULE(miconroundedlabel);

#define BTN_ICON_WIDTH 36

MIconRoundedLabel::MIconRoundedLabel(MWidget *parent, const MRect &rect, const char *text,
    const MImage *icon) : MLabel(parent, rect, text)
{
    _icon = icon;
    _pixmap = NULL;
    _bgcolor = -1;
    _imageFocus = NULL;
}

MIconRoundedLabel::~MIconRoundedLabel()
{
    if (_pixmap) delete _pixmap;
}

void MIconRoundedLabel::draw(const MRect &cliprect)
{
    MPainter ptr(this, cliprect);
    MRect rect;
    MRect rectImageFocus = MRect(0,0,width(),height());

    if (!_icon && !_pixmap)
        rect = MRect(0,0,width(),height());
    else
        rect = MRect(BTN_ICON_WIDTH,0,width()-BTN_ICON_WIDTH,height());

    if (_bgcolor != -1) {
        ptr.setPen(_bgcolor);
        ptr.setBrush(_bgcolor);

        if (_imageFocus)
        {
            ptr.fillRect(rect);
        }
        else
        {
            ptr.fillRoundedRect(rect, 8);
        }
    }

    if (hasFocus()) {
        if (_imageFocus)
        {
            ptr.drawImage(rect, *_imageFocus);
        }
        else
        {
            ptr.drawFocus(rect, -1, 2);
        }
    }

    if (_icon)
        ptr.drawImage(*_icon);
    else if (_pixmap)
        ptr.drawPixmap(*_pixmap);

    if (_text) {
        ptr.setFont(currentFont());
        if (hasFocus())
        {
            ptr.setPen(textColorFocus());
        }
        else
        {
            ptr.setPen(textColor());
        }
        ptr.setBrush(-1);
        rect.setX(rect.x() + 5);
        ptr.drawText(rect, _text, _textlen, _al, _val);
    }
}

void MIconRoundedLabel::prerender()
{
    int w = _icon->width();
    int h = _icon->height();
    
    /* draw the image into a throw away buffer */
    _pixmap = new MPixmap(fb(), w, h);

    unsigned long bgcolor = findBackgroundColor();  
    if ((int)bgcolor == -1)
        bgcolor = 0xFF000000;
    
    /* If the image as alpha channel, then it's going to blend with whatever
    is in the pixmap, so set this. */
    MPainter p2(_pixmap);
    p2.setBrush(bgcolor);
    p2.fillRect(MRect(0, 0, w, h));
    p2.drawImage(*_icon);
    
    _icon = NULL; /* we can't delete _icon because we didn't create it */
}
