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
 * $brcm_Workfile: mbutton.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/7/12 10:00a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mbutton.h $
 * 
 * 3   9/7/12 10:00a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef MBUTTON_H
#define MBUTTON_H

#include "mwidget.h"
#include "mpainter.h"

BEGIN_EV(MButton)
    EV_METHOD(MButton, Click)
END_EV()

class MButton : public MWidget {
public:
    MButton(MWidget *parent, const MRect &rect, const char *text, const char *name = NULL);
    virtual void click();
    void setChecked(bool checked);

    bool down() const {return _down;}
    bool checked() const {return down();}
    bool isChecked() const {return down();}
    void setAlignment(MPainter::Alignment al) {_al = al;}
    void setVAlignment(MPainter::VAlignment val) {_val = val;}

protected:
    bool _toggleable;
    bool _down;
    MPainter::Alignment _al;
    MPainter::VAlignment _val;
    
    virtual bool keyDown(int ascii, int modifiers, int scancode);
    SUPPORT_EV(MButton)
    SUPPORT_EV_METHOD(Click)
};

class MPushButton : public MButton {
public:
    MPushButton(MWidget *parent, const char *text = NULL, const char *name = NULL);
    MPushButton(MWidget *parent, const MRect &rect, const char *text, const char *name = NULL);
    MPushButton(MWidget *parent, int x, int y,
        const MImage *normal, const MImage *down = NULL, const MImage *focused = NULL,
        const char *text = NULL, const char *name = NULL);

    void setBevelWidth(int b) {_bevel = b;}
    void setToggleable(bool t) {_toggleable = t;}

    void setImages(MImage *normal = NULL, MImage *down = NULL, MImage *focused = NULL); 
    void setIcon(MImage *icon = NULL) { _icon = icon; };

protected:
    int _bevel;
    const MImage *_normal, *_down, *_focused, *_icon;
    virtual void draw(const MRect &cliprect);
private:
    void init();
};

class MCheckBox : public MButton {
public:
    MCheckBox(MWidget *parent, const MRect &rect = MRect(0,0,50,20), const char *text = NULL, const char *name = NULL);
    virtual void focusRepaint() {
        repaint(MRect(0,0,style()->prop(MStyle::CHECKBOX_FOCUS_WIDTH),height()));}
protected:
    virtual void draw(const MRect &cliprect);
};

class MRadioButton : public MButton {
public:
    /**
    * Group ids are compared among siblings.
    */
    MRadioButton(MWidget *parent, const MRect &rect = MRect(0,0,50,20), const char *text = NULL, int groupId = 0, const char *name = NULL);
    const char *className() const {return "MRadioButton";}
    int groupId() const {return _groupId;}

    void click();
    virtual void focusRepaint() {repaint(MRect(0,0,style()->prop(MStyle::RADIOBUTTON_FOCUS_WIDTH),height()));}
protected:
    virtual void draw(const MRect &cliprect);
    int _groupId;
};

#endif //MBUTTON_H
