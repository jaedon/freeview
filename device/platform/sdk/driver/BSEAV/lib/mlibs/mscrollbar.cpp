/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mscrollbar.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/27/07 4:15p $
 *
 * Module Description: List of widgets in a scroll view
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mscrollbar.cpp $
 * 
 * 3   2/27/07 4:15p erickson
 * PR24971: removed ScrollUp and ScrollDown events in favor of simply
 * Scroll. that's how it worked anyway.
 * 
 **********************************************************/
#include "mscrollbar.h"
#include "mpainter.h"

BDBG_MODULE(mscrollbar);

MScrollBar::MScrollBar(MWidget *parent, Orientation o, bool buttons, bool indicator, const char *name) :
    MWidget(parent, name)
{
    init(0,100,10,20,0, o, buttons, indicator);
}

void MScrollBar::init(int min, int max, int linestep, int pagestep, int value, Orientation o,
    bool buttons, bool indicator)
{
    _minValue = min;
    _maxValue = max;
    _lineStep = linestep;
    _pageStep = pagestep;
    _value = value;
    _indicWidth = _lineStep;
    _orientation = o;

    _image = NULL;
    _up = _down = NULL;
    _indicator = NULL;
    setButtons(buttons);
    setIndicator(indicator);
}

void MScrollBar::setRange(unsigned long min, unsigned long max) {
    if (min != _minValue || max != _maxValue) {
        _minValue = min;
        _maxValue = max;
        layoutIndicator();
    }
}

void MScrollBar::setMinValue(unsigned long minvalue) {
    if (minvalue != _minValue) {
        _minValue = minvalue;
        layoutIndicator();
    }
}

void MScrollBar::setMaxValue(unsigned long maxvalue) {
    if (maxvalue != _maxValue) {
        _maxValue = maxvalue;
        layoutIndicator();
    }
}

void MScrollBar::setValue(unsigned long value) {
    if (value != _value) {
        _value = value;
        layoutIndicator();
    }
}

void MScrollBar::setIndicatorWidth(unsigned long indicWidth) {
    if (_indicWidth != indicWidth) {
        _indicWidth = indicWidth;
        layoutIndicator();
    }
}

void MScrollBar::setGeometry(const MRect &rect) {
    MWidget::setGeometry(rect);
    if (_up) {
        if (_orientation == Vertical) {
            _up->setGeometry(MRect(0,0,width(),width()));
            _down->setGeometry(MRect(0,height()-width(),width(),width()));
        }
        else {
            _up->setGeometry(MRect(0,0,height(),height()));
            _down->setGeometry(MRect(width()-height(),0,height(),height()));
        }
    }
    if (_indicator)
        layoutIndicator();
}

void MScrollBar::layoutIndicator() {
    // validate the numbers
    if (_minValue > _maxValue)
        _minValue = _maxValue = 0;
    if (_indicWidth > _maxValue-_minValue)
        _indicWidth = _maxValue-_minValue;
    if (_value + _indicWidth > _maxValue)
        _value = _maxValue-_indicWidth;
    else if (_value < _minValue)
        _value = _minValue;

    if (!_indicator)
        return;

    int size = maxValue() - minValue();
    if (_orientation == Vertical) {
        int maxheight = height();
        int newy = 0, newh = 0;
        if (_up) {
            maxheight -= _up->height();
            newy = _up->height();
            maxheight -= _down->height();
        }
        if (size) {
            newy += maxheight * (value()-minValue()) / size;
            newh = maxheight * indicatorWidth() / size;
        }
        if (width() > 0 && newh > 0) {
            //printf("v indic geo %d,%d,%d,%d\n", 2,newy+2,width()-4,newh-4);
            _indicator->setGeometry(MRect(2,newy+2,width()-4,newh-4));
        }
    }
    else {
        int maxwidth = width();
        int newx = 0, neww = 0;
        if (_up) {
            maxwidth -= _up->width() + _down->width();
            newx = _up->width();
        }
        if (size) {
            newx += maxwidth * (value()-minValue()) / size;
            neww = maxwidth * indicatorWidth() / size;
        }
        if (height() > 0 && neww > 0) {
            //printf("h indic geo %d,%d,%d,%d\n", newx+2,2,neww-4,height()-4);
            _indicator->setGeometry(MRect(newx+2,2,neww-4,height()-4));
        }
    }
}

void MScrollBar::draw(const MRect &cliprect) {
    MPainter ptr(this, cliprect);
    MRect r = ptr.clipRegionRect();
    if (_image)
    {
        ptr.drawImage(*_image, 0, 0, -1, -1, MPainter::eTile);
    }
    else
    {
        ptr.drawBevel(clientRect(), 2);
    }
}

void MScrollBar::scroll(int amt) {
    if (amt < 0 && _value == _minValue ||
        amt > 0 && _value == _maxValue)
        return;

    _value += amt;
    layoutIndicator();
    FIRE(Scroll);
}

void MScrollBar::onClick(MButton *self) {
    if (self == _up) {
        scrollUp();
    }
    else if (self == _down) {
        scrollDown();
    }
}

FIRE_EV_METHOD(MScrollBar, Scroll)

void MScrollBar::setButtons(bool enabled) {
    if (enabled) {
        if (!_up) {
            _up = new MPushButton(this,
            _orientation==Vertical?"^":"<", "scrollup");
            _up->addHandler(this);
            _down = new MPushButton(this,
            _orientation==Vertical?"v":">", "scrolldown");
            _down->addHandler(this);
            layoutIndicator();
        }
    }
    else {
        if (_up) {
            //_up->hide();
            delete _up;
            _down->hide();
            delete _down;
            _up = _down = NULL;
            layoutIndicator();
        }
    }
}

void MScrollBar::setIndicator(bool enabled) {
    if (enabled) {
        if (!_indicator) {
            _indicator = new MIndicator(this);
            //_indicator->setBackgroundColor(0xdddddd);
            layoutIndicator();
        }
    }
    else {
        if (_indicator) {
            delete _indicator;
            _indicator = NULL;
        }
    }
}

void MScrollBar::MIndicator::draw(const MRect &cliprect) {
    MPainter ptr(this, cliprect);
    if (_image)
    {
        ptr.drawImage(*_image, 0, 0, -1, -1, MPainter::eTile);
    }
    else
    {
        ptr.drawBevel(clientRect(), 2,
            style()->color(MStyle::BEVEL_OUTER),
            style()->color(MStyle::BEVEL_OUTER),
            style()->color(MStyle::BEVEL_INNER),
            style()->color(MStyle::BEVEL_INNER));
    }
}

MScrollBar::MIndicator::MIndicator(MScrollBar *parent) : MWidget(parent, "mindicator") {
    setBackgroundColor(style()->color(MStyle::BUTTON_FACE));
    _image = NULL;
}

