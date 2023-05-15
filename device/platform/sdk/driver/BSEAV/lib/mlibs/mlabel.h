/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlabel.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 12/19/06 8:51a $
 *
 * Module Description: List of widgets in a scroll view
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlabel.h $
 * 
 * 3   12/19/06 8:51a erickson
 * PR26523: improved debug/headers
 * 
 **************************************************************/

#ifndef MLABEL_H
#define MLABEL_H

#include "mwidget.h"
#include "mpainter.h"

class MLabel : public MWidget {
public:
    MLabel(MFrameBuffer *fb, const MRect &rect, const char *text = NULL, const char *name = NULL);

    MLabel(MWidget *parent, const char *name = NULL);
    MLabel(MWidget *parent, const MRect &rect, const char *text = NULL, const char *name = NULL);
    MLabel(MWidget *parent, const MRect &rect, const MImage *image, const char *name = NULL);
    MLabel(MWidget *parent, const MImage *image, int bevelWidth = 0, const char *name = NULL);
    MLabel(MWidget *parent, const MRect &rect,
        const MImage *leftimage, const MImage *centerimage, const MImage *rightimage,
        const char *text = NULL, const char *name = NULL);

    const MImage *image() const {return _image;}
    void setImage(const MImage *image,
        MPainter::DrawImageMode drawImageMode = MPainter::eSingle);

    void setBevel(int width, MPainter::BevelStyle style = MPainter::bsRaised);
    // The border is the space between the inside of the bevel and any text
    void setBorder(int width) {_border = width;}

    void setAlignment(MPainter::Alignment al) {_al = al;}
    void setVAlignment(MPainter::VAlignment val) {_val = val;}

    enum WrapMode {WordWrap, NewLineWrap, NoWrap};
    void setWrapMode(WrapMode mode) {_wrapMode = mode;}

    // overrides
    void focusRepaint() {}

protected:
    virtual void draw(const MRect &cliprect);

    MPainter::Alignment _al;
    MPainter::VAlignment _val;
    MPainter::DrawImageMode _drawImageMode;
    WrapMode _wrapMode;
    const MImage *_image, *_leftimage, *_rightimage;
    int _bevelWidth, _border;
    MPainter::BevelStyle _bevelStyle;

private:
    void init();

    struct TextLine {
        const char *start;
        int len, y, h;
        TextLine(const char *s, int l, int ay, int ah) {
            start = s;
            len = l;
            y = ay;
            h = ah;
        }
    };
    void measureText(MPainter &ptr, MList<TextLine> &list);
    void parseNewLineText(MPainter &ptr, MList<TextLine> &list);
    void drawMeasuredText(MPainter &ptr, MList<TextLine> &list);
};

#endif //MLABEL_H

