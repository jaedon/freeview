/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: mpainter.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mpainter.h $
 * 
 * 6   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef MPAINTER_H
#define MPAINTER_H

#include "microwidgets.h"
#include "mpaintdevice.h"
#include "mpixmap.h"
#include "mfont.h"
#include "mfontmetrics.h"
#include "mgeom.h"

class MPainter {
public:
    MPainter(MPaintDevice *widget = NULL);
    MPainter(MPaintDevice *widget, const MRect &cliprect);
    virtual ~MPainter();

    void begin(MPaintDevice *widget);
    void end();
    
    /* This must be called if you draw apart from the message loop. If you are painting
    inside of a draw() function, then sync() will be called for you. */
    void sync();

    enum Alignment {alLeft, alCenter, alRight};
    enum VAlignment {valTop, valCenter, valBottom};

    void setClipRegion(const MRect &rect);
    void addClipRegion(const MRect &rect);
    void clearClipRegion();
    MRect clipRegionRect();

    void setBrush(int color);
    void setPen(int color);
    int pen() const;
    void setFont(const MFont &font);
    MFont font() const {return _font;}
    MFontMetrics fontMetrics() const {return MFontMetrics(_font);}

    void drawRect(const MRect &rect);
    void fillRect(const MRect &rect);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawEllipse(int x, int y, int rx, int ry = -1);
    void fillEllipse(int x, int y, int rx, int ry = -1);
    void fillRoundedRect(const MRect &rect, int roundingRadius);

    /* roundingRadius must be <= edgeWidth */
    //void drawRoundedRect(const MRect &rect, int roundingRadius, int edgeWidth);

    enum Gradient {
        eGradientVertical,
        eGradientHorizontal
    };
    void fillRect(const MRect &rect, int fromColor, int toColor, Gradient gradient);

    //NOTE: Point is exactly the same size structure as Microwindow's GR_POINT
    struct Point {
        int x,y;
        //Point(int ax,int ay) {x=ax;y=ay;}
    };
    void drawPoly(Point *points, int count);
    void fillPoly(Point *points, int count);

    void measureText(const char *text, int len, int *width, int *height);
    void drawText(int x, int y, const char *text, int len = -1);
    void drawText(const MRect &rect, const char *text, int len = -1, Alignment al = alLeft,
        VAlignment val = valTop);

    enum DrawImageMode {
        eTile, // not supported yet
        eStretch, // change size & aspect ratio to fill entire area
        eSingle, // don't change size, but align if smaller than area
        eMaximize, // don't change aspect ratio, make as large as possible
        eScaleDownToMax, // don't change aspect ratio, scale down to max, but don't scale up
        eMaxDrawImageModes};

    enum TransitionMode {
        eSwap,                  // no transition
        eDissolve,
        eWipeRight,
        eWipeLeft,
        eWipeBox,
        eSmearUp,              //requires hardware acceleration
        eSmearDown,            //requires hardware acceleration
        eSplitShutter,
        eBoxLineWipe,
        eZoomIn,               //requires hardware acceleration
        eLastTransition};      //this entry must be last!

    static MRect fitRect(int w, int h, MRect bound,
        DrawImageMode mode, Alignment al, VAlignment val);

/* TODO: clarify the image drawing api. what coordinates apply to what. */

    /**
    * In drawImage, the rect refers to the location where you want the image
    * drawn. You must draw the whole image. But, unlike pixmap, you can scale.
    */
    void drawImage(const MRect &rect, const MImage &image);
    void drawImage(const MImage &image, int x = 0, int y = 0,
        int width = -1, int height = -1, DrawImageMode mode = eSingle,
        Alignment al = alLeft, VAlignment val = valTop);

    /**
     *  drawImageThumb will attempt to use an embedded EXIF thumbnail but if
     * it does not exist, will revert to using the full sized image.  
     **/
    int drawImageThumb(const MRect &rect, const MImage &image);
    int drawImageThumb(const MImage &image, int x, int y,
                        int width, int height, DrawImageMode mode,
                        Alignment al, VAlignment val);

    /**
    * In drawPixmap, the rect refers to the coordinate system of the pixmap so you can
    * draw a part of the pixmap. However, unlike image, you cannot scale.
    */
    void drawPixmap(const MPoint &point, const MPixmap &image, const MRect &rect);
    /**
    * This applies alignment within the specified width and height
    **/
    void drawPixmap(const MPixmap &image, int x = 0, int y = 0,
        int width = -1, int height = -1, DrawImageMode mode = eSingle,
        Alignment al = alLeft, VAlignment val = valTop);

    /**
    * In drawPixmapWithTransistion, the rect refers to the coordinate system 
    * of the pixmap so you can draw a part of the pixmap. However, unlike 
    * image, you cannot scale.  The transition determines how pixmapNew is
    * displayed given pixmapOld as a starting point.
    */
    void drawPixmapWithTransition(const MPoint &point, const MPixmap &pixmapOld, 
                                  const MPixmap &pixmapNew, const MRect &rect, 
                                  TransitionMode transition);

    enum BevelStyle {bsRaised, bsSunken, bsUpDown};

    void drawBevel(const MRect &rect, int width,
        BevelStyle style, int cornerRounding = 0);
    void drawBevel(const MRect &rect, int width,
        int topColor = -1, int leftColor = -1,
        int rightColor = -1, int bottomColor = -1,
        int cornerRounding = 0, bool updown = false);
    void drawRoundedBevel(const MRect &rect, int width, bool bevelout,
        int fromcolor, int tocolor);
    static int fadeColor(float fade, int fromcolor, int tocolor = 0xff000000);
    void drawFocus(const MRect &rect, int innercolor = -1,
        int cornerRounding = 0);

    void point(int x, int y);
    
    MPoint mapToGlobal(const MPoint &point) const;

protected:
    MPaintDevice *_paintdevice;
    int id() const {return _paintdevice->id();}
    MStyle *style() const {return _paintdevice->style();}
    int _pen, _brush;
    MFont _font;
    MRect _clip;
};

#endif //MPAINTER_H
