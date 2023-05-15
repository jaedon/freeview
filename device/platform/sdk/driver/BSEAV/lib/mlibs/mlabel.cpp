/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlabel.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 12/19/06 8:51a $
 *
 * Module Description: List of widgets in a scroll view
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlabel.cpp $
 * 
 * 6   12/19/06 8:51a erickson
 * PR26523: improved debug/headers
 * 
 **************************************************************/

#include "mlabel.h"

BDBG_MODULE(mlabel);

MLabel::MLabel(MFrameBuffer *fb, const MRect &rect, const char *text, const char *name) :
    MWidget(fb, rect, text, name)
{
    init();
}

MLabel::MLabel(MWidget *parent, const char *name) :
    MWidget(parent, name)
{
    init();
}

MLabel::MLabel(MWidget *parent, const MRect &rect, const char *text, const char *name) :
    MWidget(parent, rect, text, name)
{
    init();
}

MLabel::MLabel(MWidget *parent, const MRect &rect, const MImage *image, const char *name) :
    MWidget(parent, rect, NULL, name)
{
    init();
    _image = image;
}

MLabel::MLabel(MWidget *parent, const MImage *image, int bevelWidth, const char *name) :
    MWidget(parent, MRect(0,0,image->width()+bevelWidth*2,image->height()+bevelWidth*2), NULL, name)
{
    init();
    _bevelWidth = bevelWidth;
    _image = image;
}

MLabel::MLabel(MWidget *parent, const MRect &rect,
    const MImage *leftimage, const MImage *centerimage, const MImage *rightimage,
    const char *text, const char *name) :
    MWidget(parent, rect, text, name)
{
    init();
    _leftimage = leftimage;
    _image = centerimage;
    _rightimage = rightimage;
    _al = MPainter::alCenter;
    _val = MPainter::valCenter;
}


void MLabel::init() {
    _al = MPainter::alLeft;
    _val = MPainter::valTop;
    _drawImageMode = MPainter::eSingle;
    _border = _bevelWidth = 0;
    _bevelStyle = MPainter::bsSunken;
    _wrapMode = NoWrap;
    _image = _leftimage = _rightimage = NULL;
}

void MLabel::setBevel(int width, MPainter::BevelStyle style) {
    _bevelWidth = width;
    _bevelStyle = style;
}

void MLabel::draw(const MRect &cliprect) {
    BDBG_MSG(("MLabel::draw %s %s", name(), text()));
    MPainter p(this, cliprect);

    if (_leftimage) {
        p.drawImage(*_leftimage, 0, 0);
        p.drawImage(*_image, _leftimage->width(), 0,
            width() -_leftimage->width() - _rightimage->width(), -1, MPainter::eStretch);
        p.drawImage(*_rightimage, width()-_rightimage->width(), 0);
    }
    else if (_image) {
        p.drawImage(*_image, _bevelWidth, _bevelWidth,
            width(), height(), _drawImageMode, _al, _val);
    }

    // text can be draw over images
    if (text()) {
        p.setFont(currentFont());
        p.setPen(textColor());
        p.setBrush(-1);

        switch (_wrapMode) {
        case WordWrap:
        {
            MAutoList<TextLine> list;
            measureText(p, list);
            drawMeasuredText(p, list);
        }
        break;

        case NewLineWrap:
        {
            MAutoList<TextLine> list;
            parseNewLineText(p, list);
            drawMeasuredText(p, list);
        }
        break;

        default:
        {
            if (_dropshadow) {
                int c = p.pen();
                if (c == style()->color(MStyle::LABEL_DROPSHADOW))
                {
                    //drop shadow same color as text so use opposite color
                    p.setPen(~c & 0x00FFFFFF | 0xFF000000);
                }
                else
                {
                p.setPen(style()->color(MStyle::LABEL_DROPSHADOW));
                }
                p.drawText(MRect(_dropshadow,_dropshadow,width(),height()), _text, _textlen, _al, _val);
                p.setPen(c);
            }
            p.drawText(MRect(_bevelWidth+_border,_bevelWidth+_border,
                width()-(_bevelWidth+_border)*2,height()-(_bevelWidth+_border)*2),
                _text, _textlen, _al, _val);
        }
        }
    }

    if (_bevelWidth) {
        p.drawBevel(clientRect(), _bevelWidth, _bevelStyle, 0);
    }
}

void MLabel::measureText(MPainter &p, MList<TextLine> &list) {
    const char *t, *start, *next, *fit = NULL;
    t = start = _text;
    int w,h;
    int y = _bevelWidth + _border;
    while (t) {
        next = strpbrk(t, " \t\n\r");
        p.measureText(start, next?next-start:-1, &w, &h);
        if (w > (int)width()) {
            if (fit) {
                //p.drawText(start, fit-start, MRect(0,y,width(),h), _al, _val);
                list.add(new TextLine(start, fit-start, y, h));
            }
            else {
                //have to draw partial because a single word is too big
                next = start;
                do {
                    next++;
                    p.measureText(start, next-start, &w, &h);
                } while (w < (int)width());
                t = next;
                fit = next-1;
                //p.drawText(start, fit-start, MRect(0,y,width(),h), _al, _val);
                list.add(new TextLine(start, fit-start, y, h));
            }
            y += h;
            start = t;
            fit = NULL;
        }
        else {
            if (!next)
                break;
            t = next+1;
            fit = next;
        }
    }
    // draw the remainder
    if (start && *start) {
        //p.drawText(start, -1, MRect(0,y,width(),h), _al, _val);
        list.add(new TextLine(start, -1, y, h));
    }
}

void MLabel::parseNewLineText(MPainter &p, MList<TextLine> &list) {
    const char *t = _text, *next;
    int w,h;
    int y = _bevelWidth + _border;
    while (t) {
        next = strchr(t, '\n');
        p.measureText(t, next?next-t:-1, &w, &h);
        list.add(new TextLine(t, next?next-t:-1, y, h));
        if (next)
            next++;
        t = next;
        y += h;
    }
}

void MLabel::drawMeasuredText(MPainter &ptr, MList<TextLine> &list) {
    // computer offset for vertical alignment
    int totalheight = 0;
    for (TextLine *tl = list.first(); tl; tl = list.next())
        totalheight += tl->h;
    int heightoffset = 0;
    switch (_val) {
    case MPainter::valTop: heightoffset = 0; break;
    case MPainter::valCenter: heightoffset = (height()-totalheight)/2; break;
    case MPainter::valBottom: heightoffset = height()-totalheight; break;
    }

    // draw text
    for (TextLine *tl = list.first(); tl; tl = list.next()) {
        ptr.drawText(MRect(_bevelWidth+_border,tl->y + heightoffset,width(),tl->h),
            tl->start, tl->len,
            _al, MPainter::valTop);
    }
    // nice for debugging
    //ptr.drawRect(clientRect());
}

void MLabel::setImage(const MImage *image, MPainter::DrawImageMode drawImageMode) {
    _image = image;
    _drawImageMode = drawImageMode;
    repaint();
}

