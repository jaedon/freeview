/***************************************************************************
* Copyright (c) 2002-2008, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: mpainter.cpp $
* $brcm_Revision: 8 $
* $brcm_Date: 3/18/08 3:54p $
 * Created: David Erickson 10/19/01
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/lib/mlibs/mpainter.cpp $
* 
* 8   3/18/08 3:54p erickson
* PR40307: add mapToGlobal for direct access to framebuffer
* 
* 7   8/25/06 1:26p tokushig
* PR22347: in non-skinned brutus, images w/o embedded thumbnails will
* display full image as thumb.  if using a skin, small images w/o
* embedded thumbnails will display full image as thumb but large images
* w/o embedded thumbnails will display a placeholder icon instead
* (images are currently considered large if they have more than 640 *
* 480 = 307200pixels.)  thumbnail please wait popup was replaced by
* sequential drawing of thumbs which provides better feedback to users.
* implemented image caching on thumbs (default: last 30) and for full
* screen image view (default: last 3).
* 
* SanDiego_Brutus_Skin/8   8/23/06 5:00p tokushig
* added return code to drawImageThumb()
* 
* 6   5/31/06 4:18p tokushig
* PR21891: Merging updated picture viewer into mainline.
* 
* SanDiego_Brutus_Skin/7   5/16/06 10:17a tokushig
* Merge from main
* 
* SanDiego_Brutus_Skin/6   5/9/06 12:38p tokushig
* add method to draw pixmaps with a graphical  transition
* 
* SanDiego_Brutus_Skin/5   5/4/06 12:06p tokushig
* add method to draw an image thumbnail using EXIF data if available
* 
* 5   4/17/06 5:08p jgarrett
* PR 20951: Merging skinned UI into mainline
* 
* SanDiego_Brutus_Skin/4   4/4/06 4:57p tokushig
* pixmap scaling is not possible at this time.
* 
* SanDiego_Brutus_Skin/3   4/4/06 11:15a tokushig
* add #if to allow both legacy and magnum version of brutus to display
* pixmap images that do not exactly match the useable graphics
* resolution.  magnum based platforms will scale, legacy based platforms
* will crop
* 
* SanDiego_Brutus_Skin/2   3/22/06 5:07p tokushig
* add draw mode parameter to drawPixmap().  this will allow magnum based
* platforms to scale background pixmaps.
* 
* SanDiego_Brutus_Skin/1   3/7/06 2:29p tokushig
* added handling for eMaximize and eScaleDownToMax for drawing images
* 
* 4   10/3/05 11:18a erickson
* PR17108: fixed linux 2.6 compilation warnings/errors
* 
* 3   8/24/05 11:07a erickson
* PR16819: MPixmap may have id() == 0, so check before using
* 
* 2   8/4/05 4:43p erickson
* PR15139: fixed -pedantic warnings
* 
* 1   2/7/05 11:16p dlwin
* Merge down for release 2005_REFSW_MERGETOMAIN:
* 
* Irvine_BSEAVSW_Devel/13   12/8/04 3:44p erickson
* PR13490: mpixmap draws are really copies, not bwin_image_render calls
* 
* Irvine_BSEAVSW_Devel/12   9/21/04 2:46p erickson
* PR12704: implemented clipping version of drawText
*
* Irvine_BSEAVSW_Devel/11   9/9/04 1:39p erickson
* PR12608: improved debug
*
* Irvine_BSEAVSW_Devel/10   2/25/04 9:12a erickson
* PR9882: must check if image.id() is 0 before using it
*
* Irvine_BSEAVSW_Devel/9   2/23/04 4:23p bandrews
* PR9723: Fixed default brush and pen settings to 32-bit constants
**************************************************************/

#include "mpainter.h"
#include "mframebuffer.h"
#include "bwin.h"
#include <math.h>
#include <string.h>
BDBG_MODULE(mpainter);

#define CONVERT_RECT(BWIN_RECT, MRECT) \
    bwin_rect BWIN_RECT = {MRECT.x(),MRECT.y(),MRECT.width(),MRECT.height()}

MPainter::MPainter(MPaintDevice *paintdevice) {
    _pen = _brush = 0xFF000000;
    _paintdevice = NULL;
    if (paintdevice)
        begin(paintdevice);
}

MPainter::MPainter(MPaintDevice *paintdevice, const MRect &cliprect) {
    _pen = _brush = 0xFF000000;
    _paintdevice = NULL;
    if (paintdevice)
        begin(paintdevice);
    /* TODO: addClipRegion isn't going to matter if !paintdevice */
    addClipRegion(cliprect);
}

MPainter::~MPainter() {
    end();
}

void MPainter::begin(MPaintDevice *paintdevice) {
    if (!_paintdevice) {
        _paintdevice = paintdevice;
        setClipRegion(paintdevice->clientRect());
    }
}

void MPainter::end() {
    if (_paintdevice) {
        _paintdevice = NULL;
    }
}

void MPainter::setBrush(int color) {
    _brush = color;
}

void MPainter::setFont(const MFont &font) {
    _font = font;
}

void MPainter::setPen(int color) {
    _pen = color;
}

int MPainter::pen() const {
    return _pen;
}

void MPainter::fillRect(const MRect &rect) {
    if (!id()) return;
    CONVERT_RECT(r, rect);
    CONVERT_RECT(clip, _clip);
    BDBG_MSG(("fillRect %d,%d,%d,%d", r.x,r.y,r.width,r.height));
    bwin_fill_rect((bwin_t)id(), &r, _brush, &clip);
}

void MPainter::fillRoundedRect(const MRect &rect, int rounding)
{
    int w = rect.width()-rounding*2;
    int h = rect.height()-rounding*2;
    if (w <= 0 || h <=0) return; // invalid

    // center
    fillRect(MRect(rect.x()+rounding,rect.y()+rounding,w,h));

    // sidebars
    fillRect(MRect(rect.x()+rounding,rect.y(),w,rounding));
    fillRect(MRect(rect.x()+rounding,rect.bottom()-rounding,w,rounding));
    fillRect(MRect(rect.x(),rounding,rounding,h));
    fillRect(MRect(rect.right()-rounding,rounding,rounding,h));

    // rounded edges
    fillEllipse(rect.x()+rounding, rect.y()+rounding, rounding, rounding);
    fillEllipse(rect.right()-rounding-2, rect.y()+rounding, rounding, rounding);
    fillEllipse(rect.x()+rounding, rect.bottom()-rounding-2, rounding, rounding);
    fillEllipse(rect.right()-rounding-2, rect.bottom()-rounding-2, rounding, rounding);
}

void MPainter::fillRect(const MRect &rect, int fromColor, int toColor, Gradient gradient)
{
    int steps = 0;
    switch (gradient) {
    case eGradientVertical: steps = rect.height(); break;
    case eGradientHorizontal: steps = rect.width(); break;
    }
    if (!steps) return;
    float fade = 1.0, fade_inc = 1.0/steps;

    for (int i=0;i<steps;i++) {
        int color = fadeColor(fade, fromColor, toColor);
        setPen(color);
        switch (gradient) {
        case eGradientVertical:
            drawLine(rect.x(),rect.y()+i,rect.right(),rect.y()+i);
            break;
        case eGradientHorizontal:
            drawLine(rect.x()+i,rect.y(),rect.x()+i,rect.bottom());
            break;
        }
        fade -= fade_inc;
    }
}

void MPainter::drawRect(const MRect &rect) {
    drawLine(rect.x(), rect.y(), rect.right(), rect.y());
    drawLine(rect.x(), rect.y(), rect.x(), rect.bottom());
    drawLine(rect.right()-1, rect.y(), rect.right()-1, rect.bottom());
    drawLine(rect.x(), rect.bottom()-1, rect.right(), rect.bottom()-1);
}

void MPainter::drawLine(int x1, int y1, int x2, int y2) {
    if (!id()) return;
    CONVERT_RECT(clip, _clip);
    bwin_draw_line((bwin_t)id(), x1, y1, x2, y2, _pen, &clip);
}

void MPainter::drawEllipse(int x, int y, int rx, int ry) {
    if (!id()) return;
    CONVERT_RECT(clip, _clip);
    if (ry == -1)
        ry = rx;
    bwin_draw_ellipse((bwin_t)id(), x, y, rx, ry, _pen, &clip);
}

void MPainter::fillEllipse(int x, int y, int rx, int ry) {
    if (!id()) return;
    CONVERT_RECT(clip, _clip);
    if (ry == -1)
        ry = rx;
    bwin_fill_ellipse((bwin_t)id(), x, y, rx, ry, _brush, &clip);
}

void MPainter::measureText(const char *text, int len, int *width, int *height) {
    if (text && *text && _font.id()) {
        int base;
        bwin_measure_text((bwin_font_t)_font.id(), text, len, width, height, &base);
    }
    else {
        *width = *height = 0;
    }
}

void MPainter::drawText(int x, int y, const char *text, int len) {
    if (!id() || !text || !*text || !_font.id()) return;
    CONVERT_RECT(clip, _clip);
    BDBG_MSG(("drawText %d,%d: %s, %06x", x, y, text, _pen));
    bwin_draw_text((bwin_t)id(), (bwin_font_t)_font.id(),
        x, y, text, len, _pen, &clip);
}

void MPainter::drawText(const MRect &rect, const char *text, int len, Alignment al,
    VAlignment val)
{
    if (!id() || !text || !*text || !_font.id()) return;
    int w,h;
    if (al != alLeft || val != valTop)
        measureText(text, len, &w, &h);
    int x=0,y=0;
    switch (al) {
    case alLeft: x = rect.x(); break;
    case alRight: x = rect.right() - w; break;
    case alCenter: x = rect.x() + ((int)rect.width()-w)/2; break;
    }
    switch (val) {
    case valTop: y = rect.y(); break;
    case valBottom: y = rect.bottom() - h; break;
    case valCenter: y = rect.y() + ((int)rect.height()-h)/2; break;
    }

    /* calc the desired cliprect */
    CONVERT_RECT(clip, _clip);
    CONVERT_RECT(d, rect);
    bwin_intersect_rect(&clip, &d, &clip);

    BDBG_MSG(("drawText %d,%d: %s, %06x, clip=%d,%d,%d,%d", x, y, text, _pen,
        clip.x, clip.y, clip.width, clip.height));
    bwin_draw_text((bwin_t)id(), (bwin_font_t)_font.id(),
        x, y, text, len, _pen, &clip);

}

MRect MPainter::fitRect(int w, int h, MRect bound,
    DrawImageMode mode, Alignment al, VAlignment val)
{
    int x = 0;
    int y = 0;
    if (mode == eScaleDownToMax) {
        if (w > (int)bound.width() || h > (int)bound.height())
            mode = eMaximize;
        else
            mode = eSingle;
    }

    if (mode == eStretch || mode == eTile) {
        return bound;
    }
    else if (mode == eSingle) {
        switch (al) {
        case alCenter: x = ((signed)bound.width()-w)/2; break;
        case alRight: x = (signed)bound.width()-w; break;
        }
        switch (val) {
        case valCenter: y = ((signed)bound.height()-h)/2; break;
        case valBottom: y = (signed)bound.height()-h; break;
        }
        if (x < 0) x = 0;
        if (y < 0) y = 0;
    }
    else if (mode == eMaximize) {
        float dw = (float)bound.width() / w;
        float dh = (float)bound.height() / h;
        // scale by the lesser
        if (dw > dh) {
            w = (int)(w * dh);
            h = (int)(h * dh);
            switch (al) {
            case alCenter: x = ((signed)bound.width() - w)/2; break;
            case alRight: x = (signed)bound.width()-w; break;
            }
        }
        else {
            w = (int)(w * dw);
            h = (int)(h * dw);
            switch (val) {
            case valCenter: y = ((signed)bound.height() - h)/2; break;
            case valBottom: y = (signed)bound.height()-h; break;
            }

        }
    }
    return MRect(bound.x()+x,bound.y()+y,w,h);
}

void MPainter::drawImage(const MImage &image, int x, int y,
    int width, int height, DrawImageMode mode,
    Alignment al, VAlignment val)
{
    bwin_image_render_mode rmode;

    if (!id() || !image.id())
        return;
    if (width == -1)
        width = image.width();
    if (height == -1)
        height = image.height();

    switch (mode)
    {
    case eTile:

        rmode = bwin_image_render_mode_tile;
        break;

    case eStretch:

        rmode = bwin_image_render_mode_stretch;
        break;

    // don't change aspect ratio, make as large as possible
    case eMaximize: 

        rmode = bwin_image_render_mode_maximize;
        break;

    // don't change aspect ratio, scale down to max, but don't scale up
    case eScaleDownToMax: 

        rmode = bwin_image_render_mode_maximize_down;
        break;

    default:

        rmode = bwin_image_render_mode_single;
        break;
    }

    MRect result = fitRect(image.width(), image.height(), MRect(x,y,width,height), mode, al, val);

    //TODO: tiled
    CONVERT_RECT(dest, result);
    CONVERT_RECT(clip, _clip);
    bwin_image_render((bwin_t)id(), (bwin_image_t)image.id(), rmode, &dest, NULL, &clip);
}

void MPainter::drawImage(const MRect &rect, const MImage &image) {
    if (!id() || !image.id())
        return;
    CONVERT_RECT(dest, rect);
    CONVERT_RECT(clip, _clip);
    bwin_image_render((bwin_t)id(), (bwin_image_t)image.id(), bwin_image_render_mode_single, &dest, NULL, &clip);
}

int MPainter::drawImageThumb(const MImage &image, int x, int y,
                              int width, int height, DrawImageMode mode,
                              Alignment al, VAlignment val)
{
    bwin_image_render_mode rmode;

    if (!id() || !image.id())
        return 0;
    if (width == -1)
        width = image.widthThumb();
    if (height == -1)
        height = image.heightThumb();

    switch (mode)
    {
    case eTile:

        rmode = bwin_image_render_mode_tile;
        break;

    case eStretch:

        rmode = bwin_image_render_mode_stretch;
        break;

    // don't change aspect ratio, make as large as possible
    case eMaximize: 

        rmode = bwin_image_render_mode_maximize;
        break;

    // don't change aspect ratio, scale down to max, but don't scale up
    case eScaleDownToMax: 

        rmode = bwin_image_render_mode_maximize_down;
        break;

    default:

        rmode = bwin_image_render_mode_single;
        break;
    }

    MRect result = fitRect(image.widthThumb(), image.heightThumb(), 
                           MRect(x,y,width,height), mode, al, val);

    //TODO: tiled
    CONVERT_RECT(dest, result);
    CONVERT_RECT(clip, _clip);
    return (bwin_image_render_thumb((bwin_t)id(), (bwin_image_t)image.id(), rmode, &dest, &clip));
}

int MPainter::drawImageThumb(const MRect &rect, const MImage &image) {
    if (!id() || !image.id())
        return 0;
    CONVERT_RECT(dest, rect);
    CONVERT_RECT(clip, _clip);

    return (bwin_image_render_thumb((bwin_t)id(), (bwin_image_t)image.id(), bwin_image_render_mode_single, &dest, &clip));
}

void MPainter::drawPixmap(const MPoint &point, const MPixmap &pixmap, const MRect &rect) {
    bwin_rect dest = {point.x(), point.y(), rect.width(), rect.height()};

    if (!id() || !pixmap.id())
        return;
    CONVERT_RECT(src, MRect(0, 0, pixmap.width(), pixmap.height()));
    CONVERT_RECT(clip, _clip);
    bwin_copy_rect((bwin_t)id(), &dest, (bwin_t)pixmap.id(), &src, &clip);
}

void MPainter::drawPixmap(const MPixmap &image, int x, int y,
    int width, int height, DrawImageMode mode, Alignment al, VAlignment val)
{
    if (width == -1) width = image.width();
    if (height == -1) height = image.height();

    mode = MPainter::eSingle;

    MRect result = fitRect(image.width(), image.height(), MRect(x,y,width,height), mode, al, val);

    //TODO: tiled
    drawPixmap(MPoint(result.x(), result.y()), image, result);
}

#define TRANS_FIELDS 90 /* 1 second */
void MPainter::drawPixmapWithTransition(const MPoint &point, const MPixmap &pixmapOld, const MPixmap &pixmapNew, const MRect &rect, TransitionMode transition) 
{
    bwin_rect dest = {point.x(), point.y(), rect.width(), rect.height()};

    if (!id() || !pixmapOld.id() || !pixmapNew.id())
        return;

    CONVERT_RECT(srcNew, MRect(0, 0, pixmapNew.width(), pixmapNew.height()));
    CONVERT_RECT(srcOld, MRect(0, 0, pixmapOld.width(), pixmapOld.height()));
    CONVERT_RECT(clip, _clip);

    switch (transition)
    {
    case eDissolve:
        for (int j = 1; j <= TRANS_FIELDS; j++)
        {
            unsigned long alpha = ((255 * j) / TRANS_FIELDS);
            bwin_blit((bwin_t)id(), &dest, 
                      BWIN_BLEND_WITH_PIXEL1 | BWIN_SET_DEST_ALPHA_WITH_SRC1_ALPHA, 
                      (bwin_t)pixmapNew.id(), &dest, 
                      (bwin_t)pixmapOld.id(), &dest, 
                      alpha, alpha);
            sync();
        }
        break;

    case eWipeLeft:
        for (int j = 1; j <= TRANS_FIELDS; j++)
        {
            bwin_rect rect = dest;

            rect.width = ((dest.width * (TRANS_FIELDS - j)) / TRANS_FIELDS) & ~1;
            if (rect.width > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapOld.id(), 
                               &rect, &clip);
            }

            rect.x = rect.width;
            rect.width = dest.width - rect.x;
            if ( rect.width > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                               &rect, &clip);
            }

            sync();
        }
        break;

    case eWipeRight:
        for (int j = TRANS_FIELDS; j >= 1; j--)
        {
            bwin_rect rect = dest;

            rect.width = ((dest.width * (TRANS_FIELDS - j)) / TRANS_FIELDS) & ~1;
            if (rect.width > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                               &rect, &clip);
            }

            rect.x = rect.width;
            rect.width = dest.width - rect.x;
            if ( rect.width > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapOld.id(), 
                               &rect, &clip);
            }

            sync();
        }
        break;

    case eWipeBox:
        bwin_copy_rect((bwin_t)id(), &dest, (bwin_t)pixmapOld.id(), 
                       &dest, &clip);
        for ( int j=10; j<=TRANS_FIELDS; j++ )
        {
            unsigned long alpha = ((255 * j) / TRANS_FIELDS);

            bwin_rect rect;
            rect.width  = ((dest.width * j) / TRANS_FIELDS) & ~1;
            rect.x      = ((dest.width - rect.width) / 2) & ~1;
            rect.height = ((dest.height * j) / TRANS_FIELDS) & ~1;
            rect.y      = ((dest.height - rect.height) / 2) & ~1;
            bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                           &rect, &clip);
            //bwin_blit((bwin_t)id(), &rect, 
                      //BWIN_BLEND_WITH_PIXEL1 | BWIN_SET_DEST_ALPHA_WITH_SRC1_ALPHA, 
                      //(bwin_t)pixmapNew.id(), &dest, 
                      //(bwin_t)pixmapOld.id(), &rect, 
                      //alpha, alpha);
            sync();
        }
        break;

    case eSplitShutter:
        {
            /* We're going to work in 24 steps, (1080/24)=45) */
            const int steps = 24;
            for (int j = 1; j <= TRANS_FIELDS; j += 3)
            {
                int k;
                bwin_rect rect    = dest;
                bwin_rect srcrect = dest;
                bwin_copy_rect((bwin_t)id(), &dest, (bwin_t)pixmapNew.id(), 
                               &dest, &clip);

                for (k = 0; k < steps; k += 2)
                {
                    /* Even pass, move to right side */
                    bwin_rect rect    = dest;
                    bwin_rect srcrect = dest;

                    srcrect.width  = ((dest.width * (TRANS_FIELDS - j)) / TRANS_FIELDS) & ~1;
                    rect.width     = srcrect.width;
                    rect.x         = dest.width - rect.width;
                    srcrect.height = dest.height / steps;
                    rect.height    = srcrect.height;
                    srcrect.y      = (dest.height * k) / steps;
                    rect.y         = srcrect.y;

                    if ( srcrect.width > 0 )
                        bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapOld.id(), 
                                       &srcrect, &clip);
                }

                for (k = 1; k < steps; k += 2)
                {
                    /* Odd pass, move to left side */
                    bwin_rect rect    = dest;
                    bwin_rect srcrect = dest;

                    srcrect.width  = ((dest.width * (TRANS_FIELDS - j)) / TRANS_FIELDS) & ~1;
                    rect.width     = srcrect.width;
                    srcrect.x      = dest.width - rect.width;
                    srcrect.height = dest.height / steps;
                    rect.height    = srcrect.height;
                    srcrect.y      = (dest.height * k) / steps;
                    rect.y         = srcrect.y;

                    if ( srcrect.width > 0 )
                        bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapOld.id(), 
                                       &srcrect, &clip);
                }

                sync();
            }
        }
        break;

    case eSmearUp:
        for (int j = 1; j <= TRANS_FIELDS; j++)
        {
            bwin_rect rect = dest;
            bwin_rect line = dest;
            
            line.y      = line.height * (TRANS_FIELDS - j) / TRANS_FIELDS;
            line.height = 1;

            rect.height = ((dest.height * (TRANS_FIELDS - j)) / TRANS_FIELDS) & ~1;
            if (rect.height > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                               &line, &clip);
            }

            rect.y = rect.height;
            rect.height = dest.height - rect.y;
            if ( rect.height > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                               &rect, &clip);
            }

            sync();
        }
        break;

    case eSmearDown:
        for (int j = TRANS_FIELDS; j >= 0; j--)
        {
            bwin_rect rect = dest;
            bwin_rect line = dest;

            rect.height = ((dest.height * (TRANS_FIELDS - j)) / TRANS_FIELDS) & ~1;
            if (rect.height > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                               &rect, &clip);
            }

            line.y      = line.height * (TRANS_FIELDS - j) / TRANS_FIELDS;
            line.height = 1;

            rect.y = rect.height;
            rect.height = dest.height - rect.y;
            if ( rect.height > 0 )
            {
                bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                               &line, &clip);
            }

            sync();
        }
        break;

    case eZoomIn:
        bwin_copy_rect((bwin_t)id(), &dest, (bwin_t)pixmapOld.id(), 
                       &dest, &clip);
        for ( int j = 10; j <= TRANS_FIELDS; j++ )
        {
            unsigned long alpha = ((255 * j) / TRANS_FIELDS);

            bwin_rect rect;
            rect.width  = ((dest.width * j) / TRANS_FIELDS) & ~1;
            rect.x      = ((dest.width - rect.width) / 2) & ~1;
            rect.height = ((dest.height * j) / TRANS_FIELDS) & ~1;
            rect.y      = ((dest.height - rect.height) / 2) & ~1;

            bwin_blit((bwin_t)id(), &rect, 
                      BWIN_BLEND_WITH_PIXEL1 | BWIN_SET_DEST_ALPHA_WITH_SRC1_ALPHA, 
                      (bwin_t)pixmapNew.id(), &dest, 
                      (bwin_t)pixmapOld.id(), &rect, 
                      alpha, alpha);
            //replace bwin_blit() with this to remove fade in/out
            //bwin_copy_rect((bwin_t)id(), &rect, (bwin_t)pixmapNew.id(), 
                           //&dest, &clip);
            sync();
        }
        break;

    case eBoxLineWipe:
        {
            #define BLOCKS_WIDE 12  /* evenly divisible into 720 */
            #define BLOCKS_HIGH 4  /* evenly divisible into 480 */
            #define TRAIL_LEN 12
            bwin_copy_rect((bwin_t)id(), &dest, (bwin_t)pixmapOld.id(), 
                           &dest, &clip);
            int blockWidth  = dest.width / BLOCKS_WIDE;
            int blockHeight = dest.height / BLOCKS_HIGH;
            bwin_rect rect[TRAIL_LEN];
            memset(rect, 0, TRAIL_LEN * sizeof(bwin_rect));

            for (int y = 0; y < BLOCKS_HIGH; y++)
            {
                for (int x = 0; x < BLOCKS_WIDE; x++)
                {
                    unsigned int alpha;
                    rect[0].x      = dest.x + (x * blockWidth);
                    rect[0].y      = dest.y + (y * blockHeight);
                    rect[0].width  = blockWidth;
                    rect[0].height = blockHeight;
                    
                    //draw new rect
                    alpha = (255 / TRAIL_LEN);
                    bwin_blit((bwin_t)id(), &rect[0], 
                              BWIN_BLEND_WITH_PIXEL1 | BWIN_SET_DEST_ALPHA_WITH_SRC1_ALPHA, 
                              (bwin_t)pixmapNew.id(), &rect[0], 
                              (bwin_t)pixmapOld.id(), &rect[0], 
                              alpha, alpha);

                    //draw trail rects with increasing alpha values
                    for (int j = 1; j < TRAIL_LEN; j++)
                    {
                        alpha = ((255 * (j + 1)) / TRAIL_LEN);
                        if (rect[j].width)
                        {
                            bwin_blit((bwin_t)id(), &rect[j], 
                                      BWIN_BLEND_WITH_PIXEL1 | BWIN_SET_DEST_ALPHA_WITH_SRC1_ALPHA, 
                                      (bwin_t)pixmapNew.id(), &rect[j], 
                                      (bwin_t)pixmapOld.id(), &rect[j], 
                                      alpha, alpha);
                        }
                    }

                    //shift rects
                    for (int i = TRAIL_LEN - 1; i >= 1; i--)
                    {
                        rect[i] = rect[i - 1];
                    }

                    sync();
                }
            }

            while (rect[TRAIL_LEN - 1].width)
            {
                rect[0].width = 0;

                //draw trail rects with increasing alpha values
                for (int j = 1; j < TRAIL_LEN; j++)
                {
                    unsigned int alpha = ((255 * (j + 1)) / TRAIL_LEN);
                    if (rect[j].width)
                    {
                        bwin_blit((bwin_t)id(), &rect[j], 
                                  BWIN_BLEND_WITH_PIXEL1 | BWIN_SET_DEST_ALPHA_WITH_SRC1_ALPHA, 
                                  (bwin_t)pixmapNew.id(), &rect[j], 
                                  (bwin_t)pixmapOld.id(), &rect[j], 
                                  alpha, alpha);
                    }
                }

                //shift rects
                for (int i = TRAIL_LEN - 1; i >= 1; i--)
                {
                    rect[i] = rect[i - 1];
                }

                sync();
            }
        }
        break;

    case eSwap:
    default:
        drawPixmap(point, pixmapNew, rect);
        break;
    }
}

void MPainter::drawBevel(const MRect &rect, int width,
    BevelStyle bstyle, int cornerRounding)
{
    int topColor = -1, leftColor = -1, rightColor = -1, bottomColor = -1;
    bool updown = false;
    switch (bstyle) {
    case bsRaised:
        bottomColor = style()->color(MStyle::BEVEL_OUTER);
        rightColor = style()->color(MStyle::BEVEL_OUTER);
        leftColor = style()->color(MStyle::BEVEL_INNER);
        topColor = style()->color(MStyle::BEVEL_INNER);
        break;
    case bsSunken:
        // default colors
        break;
    case bsUpDown:
        updown = true;
        break;
    }

    drawBevel(rect, width, topColor, leftColor, rightColor, bottomColor, cornerRounding, updown);
}

void MPainter::drawBevel(const MRect &rect, int w,
    int topColor, int leftColor, int rightColor, int bottomColor,
    int cornerRounding, bool updown)
{
    //printf("drawBevel %d,%d,%d,%d\n", rect.x(),rect.y(),rect.width(),rect.height());
    if (topColor == -1)
        topColor = style()->color(MStyle::BEVEL_OUTER);
    if (leftColor == -1)
        leftColor = style()->color(MStyle::BEVEL_OUTER);
    if (rightColor == -1)
        rightColor = style()->color(MStyle::BEVEL_INNER);
    if (bottomColor == -1)
        bottomColor = style()->color(MStyle::BEVEL_INNER);

    if (cornerRounding) {
        setPen(topColor);
        point(rect.x()+w,rect.y()+w);
        //setPen(bottomColor);
        //point(rect.x()+w,rect.y()+rect.height()-w-1);
        setPen(rightColor);
        point(rect.x()+rect.width()-w-1,rect.y()+w);
        setPen(bottomColor);
        point(rect.x()+rect.width()-w-1,rect.y()+rect.height()-w-1);
    }

    for (int i=0;i<w;i++) {
        if (updown && i == w/2) {
            // swap colors
            int temp = topColor;
            topColor = bottomColor;
            bottomColor = temp;
            temp = leftColor;
            leftColor = rightColor;
            rightColor = temp;
        }

        int r = rect.x() + rect.width();
        int b = rect.y() + rect.height();
        int cr = cornerRounding * cornerRounding;

        setPen(topColor);
        drawLine(
            rect.x()+i+cr,
            rect.y()+i,
            r-i-cr,
            rect.y()+i);

        setPen(leftColor);
        drawLine(
            rect.x()+i,
            rect.y()+i+cr,
            rect.x()+i,
            b-i-cr);

        setPen(rightColor);
        drawLine(
            r-i-1,
            rect.y()+i+cr,
            r-i-1,
            b-i-cr);

        setPen(bottomColor);
        drawLine(
            rect.x()+i+cr,
            b-i-1,
            r-i-cr,
            b-i-1);

        if (cornerRounding)
            cornerRounding--;
    }
}

void MPainter::drawRoundedBevel(const MRect &rect, int width, bool bevelout,
    int fromcolor, int tocolor)
{
#if 1
    float logbase = log(width);
    if (logbase)
        for (int i=0;i<width;i++) {
            int step = bevelout?i+1:(width-i);
            int color = fadeColor(log(step)/logbase,fromcolor,tocolor);
            //printf("%d: color %x fade %0.2f\n", step, color, log(step)/logbase);
            setPen(color);
            drawRect(MRect(rect.x()+i,rect.y()+i,
                rect.width()-i*2,rect.height()-i*2));
        }
#else
    if (!width) return;
    if (bevelout) {
        int temp = tocolor;
        tocolor = fromcolor;
        fromcolor = temp;
    }
    float fade = 1.0, fade_inc = 1.0/width;

    for (int i=0;i<width;i++) {
        int color = fadeColor(fade, fromcolor, tocolor);
        //printf("color %x\n", color);
        setPen(color);
        drawRect(MRect(rect.x()+i,rect.y()+i,
            rect.width()-i*2,rect.height()-i*2));
        fade -= fade_inc;
    }
#endif
}

int MPainter::fadeColor(float fade, int fromcolor, int tocolor) {
    int fa = fromcolor>>24;
    int fr = (fromcolor>>16)&0xFF;
    int fg = (fromcolor>>8)&0xFF;
    int fb = fromcolor&0xFF;
    int ta = tocolor>>24;
    int tr = (tocolor>>16)&0xFF;
    int tg = (tocolor>>8)&0xFF;
    int tb = tocolor&0xFF;
    fade = 1.0-fade;
    fa = fa+(int)((ta-fa)*fade);
    fr = fr+(int)((tr-fr)*fade);
    fg = fg+(int)((tg-fg)*fade);
    fb = fb+(int)((tb-fb)*fade);
    return fa << 24 | fr << 16 | fg << 8 | fb;
}

void MPainter::drawFocus(const MRect &rect, int innercolor,
    int cornerRounding)
{
    int color = style()->color(MStyle::FOCUS);
#if 0
    setPen(color);
    for (int i=0;i<4;i++) {
        if (innercolor != -1 && i == 3)
            setPen(innercolor);
        drawRect(MRect(rect.x()+i,rect.y()+i,rect.width()-2*i,rect.height()-2*i));
    }
#endif
    drawBevel(rect, 4, color, color, color, color, cornerRounding);
    if (innercolor != -1) {
        MRect shrink = rect;
        shrink.grow(-6);
        shrink.moveBy(3,3);
        setPen(innercolor);
        drawRect(shrink);
    }
}

void MPainter::drawPoly(Point *points, int count) {
    if (!id()) return;
//TODO: GrPoly(id(), _gc, count, (GR_POINT*)points);
}

void MPainter::fillPoly(Point *points, int count) {
    if (!id()) return;
//TODO:     GrFillPoly(id(), _gc, count, (GR_POINT*)points);
}

void MPainter::point(int x, int y) {
    if (!id()) return;
    CONVERT_RECT(clip, _clip);
    bwin_draw_point((bwin_t)id(), x, y, _pen, &clip);
}

void MPainter::setClipRegion(const MRect &rect) {
    _clip = rect;
}

void MPainter::addClipRegion(const MRect &rect) {
    _clip = _clip.intersection(rect);
}

void MPainter::clearClipRegion() {
    _clip.set(0,0,0,0);
}

MRect MPainter::clipRegionRect() {
    return _clip;
}

void MPainter::sync() {
    MFrameBuffer::Settings settings;
    _paintdevice->fb()->getSettings(&settings);
    bwin_sync(settings.framebuffer);
}

extern "C" {
    void bwin_p_convert_to_global(bwin_t window, int x, int y, int *globalx, int *globaly);
}

MPoint MPainter::mapToGlobal(const MPoint &point) const
{
    int x, y;
    bwin_p_convert_to_global((bwin_t)id(), point.x(), point.y(), &x, &y);
    return MPoint(x,y);
}

