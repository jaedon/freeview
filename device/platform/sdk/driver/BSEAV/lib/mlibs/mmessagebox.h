/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mmessagebox.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/31/06 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mmessagebox.h $
 * 
 * 2   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/1   5/22/06 10:15a tokushig
 * added optional progress bar
 * 
 * 1   2/7/05 11:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   1/14/05 11:34a erickson
 * PR11077: tweaked MMessageBox constructor to use MFrameBuffer
 * 
 * Irvine_BSEAVSW_Devel/3   4/22/04 10:15p erickson
 * PR9107: need to raise all MMessageBox's by default
 *
 ****************************************************************************/
#ifndef MMESSAGEBOX_H
#define MMESSAGEBOX_H

#include "mwidget.h"
#include "mbutton.h"
#include "mstring.h"
#include "mlabel.h"
#include "mlineedit.h"
#include "mtimer.h"
#include "mscrollbar.h"

class MMessageBox : public MWidget, public MButtonEventHandler, public MTimerEventHandler {
public:
    static const int bOK =      0x01;
    static const int bCancel =  0x02;
    static const int bYes =     0x04;
    static const int bNo =      0x08;
    static const int bAll =     0x10;

    /**
    Create a message box at any location
    **/
    MMessageBox(MFrameBuffer *fb, const MRect &rect, const char *name = NULL);
    
    /**
    Create a message box centered to the framebuffer
    **/
    MMessageBox(MFrameBuffer *fb, int width, int height, const char *name = NULL);

    void set(const char *message, int buttons, int focusbutton = -1, int inputwidth = -1, int progresswidth = -1);
    void setProgress(int percent);
    void clearButtons();
    MButton *addButton(const char *text, int id);
    int result() const {return _result;}
    void show() {MWidget::show();}
    void setWrapMode(MLabel::WrapMode mode) {_label->setWrapMode(mode);}
    void setEchoMode(MLineEdit::EchoMode mode) {if (_input) _input->setEchoMode(mode);}
    const char *text() {return _input?_input->text():NULL;}
    MScrollBar * progressBar() { return _progress; };

    int process(int msecTimeout = -1);

    static int info(MFrameBuffer *fb, const char *message, int msecTimeout = -1);
    static int show(MFrameBuffer *fb, const char *message, int buttons = bYes|bNo,
        int focusbutton = bNo, int msecTimeout = -1);
    static int input(MFrameBuffer *fb, const char *message, int inputwidth, int buttons,
        MString &response, int msecTimeout = -1,
        MLineEdit::EchoMode echoMode = MLineEdit::Normal);

    void onTimeout(MTimer *sender);
    void onClick(MButton *self);

protected:
    // overrides
    void draw(const MRect &cliprect);
    bool keyDown(int ascii, int modifiers, int scancode);

    void init();

    int _result;
    int _buttons;
    MList<MButton> _buttonList;
    MLabel *_label;
    MString _text;
    MLineEdit *_input;
    MScrollBar *_progress;
    MTimer _timer;
    MWidget *_focusWidget;
};

#endif //MMESSAGEBOX_H
