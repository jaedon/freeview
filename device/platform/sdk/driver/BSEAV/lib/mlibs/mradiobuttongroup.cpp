/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mradiobuttongroup.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/31/06 4:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mradiobuttongroup.cpp $
 * 
 * 4   5/31/06 4:17p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   5/11/06 2:55p erickson
 * PR20951: don't hardcode textColor in radio button
 * 
 ********************************************************************/
#include "mradiobuttongroup.h"

MRadioButtonGroup::MRadioButtonGroup(MWidget *parent, const MRect &rect, const char *name) :
    MWidget(parent, rect, NULL, name)
{
    _itemHeight = -1;
}

void MRadioButtonGroup::click(int index) {
    MRadioButton *rb = (MRadioButton *)child(index);
    if (rb)
        rb->click();
}

bool MRadioButtonGroup::itemEnabled(int index) {
    MRadioButton *rb = (MRadioButton *)child(index);
    return rb && rb->enabled();
}

int MRadioButtonGroup::checked() {
    int i=0;
    for (MRadioButton *rb = (MRadioButton *)firstChild(); rb; rb = (MRadioButton *)nextChild(),i++) {
        if (rb->checked())
            return i;
    }
    return -1;
}

void MRadioButtonGroup::add(const char *text) {
    MRadioButton *rb = new MRadioButton(this, MRect(0,0,1,1), text);
    rb->addHandler(this);
    if (visible())
        rb->show();
    layout();
}

void MRadioButtonGroup::setGeometry(const MRect &rect) {
    MWidget::setGeometry(rect);
    layout();
}

void MRadioButtonGroup::layout() {
    if (!totalChildren())
        return;
    int inc = _itemHeight == -1 ? height()/totalChildren() : _itemHeight;
    int y = 0;
    for (MRadioButton *rb = (MRadioButton *)firstChild(); rb; rb = (MRadioButton *)nextChild()) {
        rb->setGeometry(MRect(0,y,width(),inc));
        y += inc;
    }
}

bool MRadioButtonGroup::focus() {
    if (totalChildren())
        return firstChild()->focus();
    else
        return MWidget::focus();
}

void MRadioButtonGroup::setEnabled(bool e) {
    if (e != enabled()) {
        MWidget::setEnabled(e);
        for (MRadioButton *rb = (MRadioButton *)firstChild(); rb; rb = (MRadioButton *)nextChild()) {
            rb->setEnabled(e);
        }
    }
}

void MRadioButtonGroup::setEnabled(int index, bool enabled) {
    MRadioButton *rb = (MRadioButton*)_children.at(index);
    if (rb && rb->enabled() != enabled) {
        rb->setEnabled(enabled);
        if (!enabled && rb->checked())
            clickDefault();
    }
}

FIRE_EV_METHOD(MRadioButtonGroup, Click)

void MRadioButtonGroup::onClick(MButton *event) {
    FIRE(Click);
}

void MRadioButtonGroup::clickDefault() {
    for (MRadioButton *rb = (MRadioButton *)firstChild(); rb; rb = (MRadioButton *)nextChild()) {
        if (rb->enabled()) {
            rb->click();
            break;
        }
    }
}

void MRadioButtonGroup::setItemHeight(int itemHeight) {
    _itemHeight = itemHeight;
    layout();
}

