/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mscrollbar.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/27/07 4:15p $
 *
 * Module Description: List of widgets in a scroll view
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mscrollbar.h $
 * 
 * 3   2/27/07 4:15p erickson
 * PR24971: removed ScrollUp and ScrollDown events in favor of simply
 * Scroll. that's how it worked anyway.
 * 
 **********************************************************/
#ifndef MSCROLLBAR_H
#define MSCROLLBAR_H

#include "mwidget.h"
#include "mbutton.h"

BEGIN_EV(MScrollBar)
    EV_METHOD(MScrollBar, Scroll)
END_EV()

class MScrollBar : public MWidget, public MButtonEventHandler {
public:
    enum Orientation {Horizontal, Vertical};

    // NOTE: I'm forcing the geometry to be set later because of overrided setGeometry.
    MScrollBar(MWidget *parent, Orientation o = Vertical, bool buttons = true, bool indicator = true, const char *name = NULL);

    void setButtons(bool enabled);
    void setIndicator(bool enabled);
    void setImage(MImage *image) { _image = image; };
    void setIndicatorImage(MImage *image) { _indicator->setImage(image); };

    void setMinValue(unsigned long minvalue);
    void setMaxValue(unsigned long maxvalue);
    void setValue(unsigned long value);
    void setIndicatorWidth(unsigned long indicWidth);
    void setRange(unsigned long min, unsigned long max);

    unsigned long minValue() const {return _minValue;}
    unsigned long maxValue() const {return _maxValue;}
    unsigned long value() const {return _value;}
    unsigned long indicatorWidth() const {return _indicWidth;}

    unsigned long lineStep() const {return _lineStep;}
    unsigned long pageStep() const {return _pageStep;}
    void setSteps(unsigned long lineStep, unsigned long pageStep) {
        _lineStep = lineStep;
        _pageStep = pageStep;
    }

    void scrollUp() {subtractLine();}
    void scrollDown() {addLine();}
    void subtractLine() {scroll(-_lineStep);}
    void addLine() {scroll(_lineStep);}
    void subtractPage() {scroll(-_pageStep);}
    void addPage() {scroll(_pageStep);}
    void scroll(int amt);

    // events
    void onClick(MButton *self);

    // overrides
    void setGeometry(const MRect &rect);

protected:
    // overrides
    void draw(const MRect &cliprect);

    void layoutIndicator();

    unsigned long _minValue, _maxValue, _value, _lineStep, _pageStep, _indicWidth;
    MPushButton *_up, *_down;
    Orientation _orientation;
    MImage      *_image;

    class MIndicator : public MWidget {
    public:
        MIndicator(MScrollBar *parent);
        void setImage(MImage *image) { _image = image; };
    protected:
        MImage *_image;
        void draw(const MRect &cliprect);
    };
    MIndicator *_indicator;

    SUPPORT_EV(MScrollBar)
    SUPPORT_EV_METHOD(Scroll)

private:
    void init(int min, int max, int linestep, int pagestep, int value, Orientation o,
        bool buttons, bool indicator);
};

#endif //MSCROLLBAR_H
