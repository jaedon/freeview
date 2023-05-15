/***************************************************************************
 *     Copyright (c) 2001-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mmessagebox.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 2/2/07 10:09a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mmessagebox.cpp $
 * 
 * 4   2/2/07 10:09a erickson
 * PR26661: added MApplication::clearModalState. this should be called
 * after modal loop is done.
 * 
 * 3   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/23/06 4:44p tokushig
 * correct max value calculation in setProgress()
 * 
 * SanDiego_Brutus_Skin/1   5/22/06 10:15a tokushig
 * added optional progress bar
 * 
 * 1   2/7/05 11:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   1/14/05 11:34a erickson
 * PR11077: tweaked MMessageBox constructor to use MFrameBuffer
 * 
 * Irvine_BSEAVSW_Devel/8   4/22/04 10:15p erickson
 * PR9107: need to raise all MMessageBox's by default
 *
 ****************************************************************************/
#include "mmessagebox.h"
#include "mapplication.h"
#include <bstd.h>
#include <bkni.h>

#define BUTTON_WIDTH 90
#define BUTTON_HEIGHT 30
#define SPACING 20
#define BORDER 15
#define TEXTBORDER 5

MMessageBox::MMessageBox(MFrameBuffer *fb, int width, int height, const char *name) :
    MWidget(fb, name),_timer(fb->app())
{
    setGeometry(MRect((fb->screenWidth()-width)/2,(fb->screenHeight()-height)/2,
        width,height));
    init();
}

MMessageBox::MMessageBox(MFrameBuffer *fb, const MRect &rect, const char *name) :
    MWidget(fb, rect, name), _timer(fb->app())
{
    init();
}

void MMessageBox::init() {
    setBackgroundColor(style()->color(MStyle::MESSAGEBOX_BACKGROUND));
    raise();
    _result = 0;
    _label = new MLabel(this, "mmessagebox_label");
    _label->setGeometry(MRect(BORDER,BORDER,width()-BORDER*2,height()-BUTTON_HEIGHT-BORDER*2));
    _label->setWrapMode(MLabel::WordWrap);
    _label->setAlignment(MPainter::alCenter);
    _label->setVAlignment(MPainter::valCenter);
    _timer.addHandler(this);
}

void MMessageBox::set(const char *message, int buttons, int focusbutton, int inputwidth, int progresswidth) {
    clearButtons();
    _buttons = buttons;
    _result = 0;
    _input = NULL;
    _progress = NULL;
    _label->setText(message);
    _focusWidget = NULL;

    if (inputwidth != -1) {
        _input = new MLineEdit(this, MRect(
            (width()-inputwidth)/2,
            height()-BORDER*2-BUTTON_HEIGHT*2,
            inputwidth,
            BUTTON_HEIGHT));
        _focusWidget = _input;
        _label->setGeometry(MRect(
            TEXTBORDER,
            TEXTBORDER,
            width()-TEXTBORDER*2,
            height()-BORDER*2-TEXTBORDER*2-BUTTON_HEIGHT*2));
    }
    else
    if (progresswidth != -1)
    {
        _progress = new MScrollBar(this, MScrollBar::Horizontal, false);
        _progress->setGeometry(MRect(
            (width()-progresswidth)/2,
            height()-BORDER-BUTTON_HEIGHT,
            progresswidth,
            BUTTON_HEIGHT));
    }

    if (buttons) {
        MWidget *temp;
        if (focusbutton == -1)
            focusbutton = buttons;
        if (buttons & bOK) {
            temp = addButton("OK", bOK);
            if (focusbutton & bOK)
                _focusWidget = temp;
        }
        if (buttons & bCancel) {
            temp = addButton("Cancel", bCancel);
            if (focusbutton & bCancel)
                _focusWidget = temp;
        }
        if (buttons & bYes) {
            temp = addButton("Yes", bYes);
            if (focusbutton & bYes)
                _focusWidget = temp;
        }
        if (buttons & bNo) {
            temp = addButton("No", bNo);
            if (focusbutton & bNo)
                _focusWidget = temp;
        }
        if (buttons & bAll) {
            temp = addButton("All", bAll);
            if (focusbutton & bAll)
                _focusWidget = temp;
        }
    }

    if ((!buttons) && (inputwidth == -1) && (progresswidth == -1)) {
        // resize label to fill the entire area
        _label->setGeometry(MRect(TEXTBORDER,TEXTBORDER,width()-TEXTBORDER*2,
            height()-TEXTBORDER*2));
    }
}

void MMessageBox::setProgress(int percent)
{
    if ((percent < 0) || (percent > 100))
    {
        return;
    }

    _progress->setMaxValue((uint32_t)100 - percent);
    _progress->drawNow();
}

void MMessageBox::clearButtons() {
    for (MButton *btn = _buttonList.first(); btn;) {
        delete btn;
        _buttonList.remove();
        btn = _buttonList.current();
    }
    _buttons = 0;
}

MButton *MMessageBox::addButton(const char *text, int id) {
    MButton *btn = new MPushButton(this, text, text);
    btn->addHandler(this);
    btn->setTag(id);
    _buttonList.add(btn);

    // resize the buttons
    int margin = (width() - ((BUTTON_WIDTH+SPACING)*_buttonList.total() - SPACING))/2;
    int i=0;
    for (MButton *b = _buttonList.first(); b; b = _buttonList.next(), i++) {
        b->setGeometry(MRect(
            margin+(BUTTON_WIDTH+SPACING)*i,
            height()-BORDER-BUTTON_HEIGHT,
            BUTTON_WIDTH, BUTTON_HEIGHT));
    }
    return btn;
}

void MMessageBox::onClick(MButton *self) {
    for (MButton *btn = _buttonList.first(); btn; btn = _buttonList.next()) {
        if (btn == self) {
            _result = self->tag();
            break;
        }
    }
}

int MMessageBox::show(MFrameBuffer *fb, const char *message, int buttons, int focusbutton, int msecTimeout) {
    MMessageBox mbox(fb, 400, 150);
    mbox.set(message, buttons, focusbutton);
    if (!buttons) {
        mbox.show();
        mbox.drawNow();
        BKNI_Sleep(msecTimeout);
        return bOK;
    }
    else
        return mbox.process(msecTimeout);
}

int MMessageBox::input(MFrameBuffer *fb, const char *message, int inputwidth, int buttons,
    MString &response, int msecTimeout, MLineEdit::EchoMode echoMode)
{
    MMessageBox mbox(fb, 400, 150);
    mbox.set(message, buttons, 0, inputwidth);
    mbox.setEchoMode(echoMode);
    int rc = mbox.process(msecTimeout);
    response = mbox.text();
    return rc;
}

int MMessageBox::process(int msecTimeout) {
    MWidget *wasFocus = app()->focusedWidget();
    show();
    if (_focusWidget)
        _focusWidget->focus();

    // start timer if needed
    if (msecTimeout != -1)
        _timer.start(msecTimeout);

    // process all messages while we wait for the result
    while (!result() && app()->processEvent(true));
    app()->clearModalState();

    _timer.stop();

    // hide and restore focus
    hide();
    if (wasFocus)
        wasFocus->focus();
    return result();
}

int MMessageBox::info(MFrameBuffer *fb, const char *message, int msecTimeout) {
    return show(fb, message, bOK, bOK, msecTimeout);
}

void MMessageBox::draw(const MRect &cliprect) {
    MPainter ptr(this, cliprect);
    ptr.drawRoundedBevel(MRect(2,2,width()-4,height()-4),
        10, true, backgroundColor(),
        style()->color(MStyle::MESSAGEBOX_BORDER));
    ptr.setPen(style()->color(MStyle::MESSAGEBOX_TEXT));
    ptr.drawRect(clientRect());
    ptr.drawRect(MRect(1,1,width()-2,height()-2));
}

bool MMessageBox::keyDown(int ascii, int modifiers, int scancode) {
    switch (ascii) {
    case MWKEY_ENTER:
        // Because buttons handle the ENTER key, this key will only reach
        // MMessageBox when focus is on the input box,
        // therefore we know this should generate an OK.
        if (_buttons & bOK)
            _result = bOK;
        else
            _result = bYes;
        return true;
    case MWKEY_ESCAPE:
        if (_buttons & bCancel)
            _result = bCancel;
        else
            _result = bNo;
        return true;
    default:
        return MWidget::keyDown(ascii, modifiers, scancode);
    }
}

void MMessageBox::onTimeout(MTimer *sender) {
    if (_buttons & bCancel)
        _result = bCancel;
    else
        _result = bNo;
}
