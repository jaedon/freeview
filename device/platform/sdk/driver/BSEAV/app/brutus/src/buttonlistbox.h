/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: buttonlistbox.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 3/26/10 5:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/buttonlistbox.h $
 * 
 * 3   3/26/10 5:22p erickson
 * SW7405-3625: refactor constructor params for ButtonListBox to allow for
 * more generic use
 *
 * 2   10/8/08 11:53a tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/2   10/8/08 10:49a tokushig
 * PR43426: move global _buttonActionList to a class attribute for
 * homescreen and videoscreen.  this will allow the code to work properly
 * when 2 instances of brutus exist as well as avoid a global constructor
 * issue in vxworks.
 *
 * 1   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   8/21/08 5:21p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 *
 *
 **************************************************************************/
#ifndef BUTTON_LIST_BOX__
#define BUTTON_LIST_BOX__

#include "bstd.h"
#include "control.h"
#include "mlistbox.h"

/********************************************
*
* ButtonListBox - a custom list box entry for the Homescreen
*
********************************************/
class ButtonListBox : public MColumnListBox {
public:
    ButtonListBox(MWidget *parent,
        const MRect &rect, int scrollBarWidth, int buttonVSpacing, unsigned arrowFillColor, const char *name = NULL) :
        MColumnListBox(parent, rect, scrollBarWidth, name)
    {
        _showButtons = true;
        _buttonIconList.clear();
        _arrowFillColor = arrowFillColor;
        _buttonVSpacing = buttonVSpacing;
        scrollBar()->hide();
    }

    void insert(int index, const char *text, MImage *icon);
    void showButtons(bool show) { _showButtons = show; }
    bool buttonsVisible() { return _showButtons; }
    void setButtonVSpacing(int spacing) { _buttonVSpacing = spacing; }
    int buttonVSpacing() { return _buttonVSpacing; }

protected:

    class ButtonListItem : public MColumnListItem {
    public:
        ButtonListItem(ButtonListBox *listbox, const char *name = NULL) :
            MColumnListItem(listbox, name)
        {
            _buttonListBox = listbox;
        }
    protected:
        void drawColumn(
            MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
            MPainter::Alignment al, MPainter::VAlignment val);
        ButtonListBox *_buttonListBox;
    };

    MListItem *createItem();

    unsigned _arrowFillColor;
    MList <MImage> _buttonIconList;
    int _buttonVSpacing;
    bool _showButtons;

    void layout();
    void draw(const MRect &cliprect);
    virtual void drawArrows(MPainter &ptr, const MRect &rect);
    virtual void drawIcon(MPainter &ptr, const MRect &rect, const int index);
};

#endif
