/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mbutton.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/31/06 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mbutton.cpp $
 * 
 * 4   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/5   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/4   5/4/06 2:23p tokushig
 * fix dropshadow color selection when dropshadow matches text color
 * 
 * 3   4/17/06 5:04p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/3   2/28/06 1:21p tokushig
 * allow buttons with only a focus image
 * 
 * SanDiego_Brutus_Skin/2   2/23/06 10:04a tokushig
 * added optional icon to mpushbutton
 * 
 * SanDiego_Brutus_Skin/1   2/14/06 12:48p tokushig
 * draws opaque rect behind button image if a background color is
 * specified - this will prevent any background transparency from
 * affecting the button's transparency
 * 
 * 2   8/24/05 6:18p arbisman
 * PR16678: Add support for WinCE
 * 
 * 1   2/7/05 11:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   12/20/04 2:40p erickson
 * PR13311: converted checkbox box to check
 * 
 * Irvine_BSEAVSW_Devel/3   2/26/04 11:03a erickson
 * PR9900: now using MListItr to allow const access to MList's
 *
 ****************************************************************************/
#include "mbutton.h"
#include "mpainter.h"

MButton::MButton(MWidget *parent, const MRect &rect, const char *text, const char *name) :
    MWidget(parent, rect, text, name)
{
    _down = false;
    _toggleable = false;
    _al = MPainter::alCenter;
    _val = MPainter::valCenter;
    setFocusable(true);
}

void MButton::click() {
    if (enabled()) {
        if (_toggleable)
            _down = !_down;
        // fire click before repaint because we might have a delay before
        // the button can be repainted and we don't want it to be invisible
        FIRE(Click);
        //NOTE: this isn't quite right, but it turns out that focus
        // completely surronds the area affected by a click.
        // When this isn't true, this has to change.
        focusRepaint();
    }
}

void MButton::setChecked(bool checked) {
    if (_down != checked) {
        _down = checked;
        focusRepaint();
    }
}

bool MButton::keyDown(int ascii, int modifiers, int scancode) {
    switch (ascii) {
    case 13:
    case ' ':
        click();
        return true;
    default:
        return false;
    }
}

FIRE_EV_METHOD(MButton, Click)

//////////////////////////////////////////////

MPushButton::MPushButton(MWidget *parent, const char *text, const char *name) :
    MButton(parent, MRect(0,0,70,30), text, name)
{
    init();
}

MPushButton::MPushButton(MWidget *parent, const MRect &rect, const char *text, const char *name) :
    MButton(parent, rect, text, name)
{
    init();
}

MPushButton::MPushButton(MWidget *parent, int x, int y,
    const MImage *normal, const MImage *down, const MImage *focused,
    const char *text, const char *name) :
    MButton(parent, MRect(x,y,normal?normal->width():10,normal?normal->height():10), text, name)
{
    init();
    _normal = normal;
    _down = down;
    _focused = focused;
    setTransparent(true);
}

void MPushButton::init() {
    setTransparent(false);
    _bevel = 5;
    _normal = _down = _focused = _icon = NULL;
}

void MPushButton::setImages(MImage *normal, MImage *down, MImage *focused) 
{ 
    _normal = normal; 
    _down = down; 
    _focused = focused; 
    setTransparent(true);
}

void MPushButton::draw(const MRect &cliprect) {
    //printf("MPushButton::draw %s\n", name());
    MPainter p(this, cliprect);

    if (hasFocus() && _focused)
    {
        //if background color has been set, draw an opaque rect
        //behind image so image transparency will not be affected
        //by the underlying background image transparency
        if (backgroundColor() != -1) {
            int32_t nColorNonTransparent = 
                backgroundColor() & 0x00ffffff | 0xff000000;
            p.setPen(nColorNonTransparent);
            p.setBrush(nColorNonTransparent);
            p.fillRect(cliprect);
        }

        //handle focused case separately since we want the option
        //to use just a focus image and no _normal or _down images
        p.drawImage(clientRect(), *_focused);
    }
    else
    if (_normal) {
        //if background color has been set, draw an opaque rect
        //behind image so image transparency will not be affected
        //by the underlying background image transparency
        if (backgroundColor() != -1) {
            int32_t nColorNonTransparent = 
                backgroundColor() & 0x00ffffff | 0xff000000;
            p.setPen(nColorNonTransparent);
            p.setBrush(nColorNonTransparent);
            p.fillRect(cliprect);
        }

        if (down() && _down)
        {
            p.drawImage(clientRect(), *_down);
        }
        else {
            p.drawImage(clientRect(), *_normal);
            if (hasFocus())
                p.drawFocus(clientRect(), -1, style()->prop(MStyle::BUTTON_ROUNDING));
        }
    }
    else 
    if (!_normal && !_down && !_focused) {
        // draw face
        p.setPen(style()->color(MStyle::BUTTON_FACE, enabled()));
        p.setBrush(style()->color(MStyle::BUTTON_FACE, enabled()));
        p.fillRect(MRect(_bevel,_bevel,width()-2*_bevel,height()-2*_bevel));

        // draw bevel
        if (down())
            // reverse the bevel
            p.drawBevel(clientRect(), _bevel,
                style()->color(MStyle::BUTTON_BOTTOMBEVEL, enabled()),
                style()->color(MStyle::BUTTON_RIGHTBEVEL, enabled()),
                style()->color(MStyle::BUTTON_LEFTBEVEL, enabled()),
                style()->color(MStyle::BUTTON_TOPBEVEL, enabled()),
                style()->prop(MStyle::BUTTON_ROUNDING));
        else
            p.drawBevel(clientRect(), _bevel,
                style()->color(MStyle::BUTTON_TOPBEVEL, enabled()),
                style()->color(MStyle::BUTTON_LEFTBEVEL, enabled()),
                style()->color(MStyle::BUTTON_RIGHTBEVEL, enabled()),
                style()->color(MStyle::BUTTON_BOTTOMBEVEL, enabled()),
                style()->prop(MStyle::BUTTON_ROUNDING));
        if (hasFocus())
            p.drawFocus(clientRect(), -1, style()->prop(MStyle::BUTTON_ROUNDING));
    }

    uint32_t nIconWidth = 0;
    uint32_t nIconBorder = 0;

    //draw icon and adjust client rect for text
    if (_icon)
    {
        nIconWidth  = _icon->width();
        nIconBorder = 10;
        MRect rect  = MRect(nIconBorder, 
                            height()/2 - nIconWidth/2, 
                            nIconWidth, 
                            height());
        p.drawImage(rect, *_icon);
    }

    p.setFont(currentFont());
    p.setPen(hasFocus() ? textColorFocus() : textColor());

#if 0
    int tw, th;
    p.measureText(_text, _textlen, &tw, &th);
    int x = (width()-tw)/2;
    int y = (height()-th)/2;
#endif
    int x = 0;
    int y = 0;
    if (down()) {
        x += 2;
        y += 2;
    }

    if (_dropshadow) {
        int c = p.pen();
        if (c == style()->color(MStyle::LABEL_DROPSHADOW))
        {
            //text is same color as dropshadow so use opposite color
            p.setPen(~c & 0x00FFFFFF | 0xFF000000);
        }
        else
        {
            p.setPen(style()->color(MStyle::LABEL_DROPSHADOW));
        }
        
        p.drawText(MRect(x + _dropshadow + nIconWidth + (nIconBorder * 2), 
                         y + _dropshadow, 
                         width() - nIconWidth, 
                         height()), 
                   _text, _textlen, _al, _val);
        p.setPen(c);
    }

    p.drawText(MRect(x + nIconWidth + (nIconBorder * 2), y, 
                     width() - nIconWidth, height()), 
               _text, _textlen, _al, _val);
}

///////////////////////////////////////////////

MCheckBox::MCheckBox(MWidget *parent, const MRect &rect, const char *text, const char *name) :
    MButton(parent, rect, text, name)
{
    _toggleable = true;
    _al = MPainter::alLeft;
}

void MCheckBox::draw(const MRect &cliprect) {
    int fw = style()->prop(MStyle::CHECKBOX_FOCUS_WIDTH);
    int w = style()->prop(MStyle::CHECKBOX_WIDTH);

    MPainter ptr(this, cliprect);
    MRect r(0, ((int)height()-fw)/2,fw,fw);
    if (hasFocus())
        ptr.drawFocus(r, -1, style()->prop(MStyle::BUTTON_ROUNDING));
    r = MRect((fw-w)/2, (height()-w)/2, w, w);
    ptr.drawBevel(r, 2);

    ptr.setBrush(textColor());
    r.grow(-4);
    r.moveBy(2,2);
    ptr.fillRect(r);

    if (checked()) {
#define START_CHECK_X 8
        int x = START_CHECK_X;
        int y = r.y()+7;
        ptr.setPen(style()->color(MStyle::CHECKBOX_CHECK, enabled()));
        for (;x<START_CHECK_X+4;x++) {
            ptr.drawLine(x,y,x+3,y);
            y++;
            ptr.drawLine(x,y,x+4,y);
            y++;
        }
        for (;x<START_CHECK_X+11;x++) {
            y--;
            ptr.drawLine(x,y,x+3,y);
            y--;
            ptr.drawLine(x,y,x+4,y);
        }
    }

    ptr.setFont(currentFont());
    ptr.setPen(textColor());
    ptr.drawText(MRect(fw+5,0,width()-fw-5,height()), text(), _textlen,
        _al, _val);
}

MRadioButton::MRadioButton(MWidget *parent, const MRect &rect, const char *text, int groupId, const char *name) :
    MButton(parent, rect, text, name)
{
    _groupId = groupId;
    _toggleable = true;
    _al = MPainter::alLeft;
}

void MRadioButton::click() {
    if (checked() || !enabled()) return;

    // uncheck any other in the group
    if (parent()) {
        for (MWidget *sib = parent()->firstChild(); sib; sib = parent()->nextChild()) {
            if (sib != this && !strcmp(sib->className(), "MRadioButton")) {
                MRadioButton *rb = (MRadioButton*)sib;
                if (rb->groupId() == groupId() && rb->checked())
                {
                    // don't fire a message
                    rb->_down = false;
                    rb->focusRepaint();
                }
            }
        }
    }
    MButton::click();
}

void MRadioButton::draw(const MRect &cliprect) {
    int fw = style()->prop(MStyle::RADIOBUTTON_FOCUS_WIDTH);
    int w = style()->prop(MStyle::RADIOBUTTON_WIDTH);

    MPainter ptr(this, cliprect);
    if (hasFocus()) {
        MRect r(0, ((int)height()-fw)/2,fw,fw);
        ptr.drawFocus(r, -1, style()->prop(MStyle::BUTTON_ROUNDING));
    }
    int x = fw/2;
    int y = height()/2;
    int r = w/2;
    ptr.setPen(style()->color(MStyle::BEVEL_OUTER));
    ptr.drawEllipse(x,y,r,r);
    r--;
    ptr.setPen(style()->color(MStyle::BEVEL_INNER));
    ptr.drawEllipse(x,y,r,r);
    r--;
    ptr.drawEllipse(x,y,r,r);
    r--;
    ptr.setBrush(textColor());
    ptr.fillEllipse(x,y,r,r);

    if (checked()) {
        r = w/2 - 5;
        ptr.setBrush(style()->color(MStyle::CHECKBOX_CHECK, enabled()));
        ptr.fillEllipse(x,y,r,r);
    }

    ptr.setFont(currentFont());
    ptr.setPen(textColor());
    x = fw+5;
    ptr.drawText(MRect(x,0,width()-x,height()), text(), _textlen,
            _al, _val);
}

