/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_modalmsgbox.cpp $
* $brcm_Revision: 7 $
* $brcm_Date: 11/1/12 6:52p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_modalmsgbox.cpp $
* 
* 7   11/1/12 6:52p tokushig
* SW7231-749: unlink all windows directly decendended from the bwin
* framebuffer.  for some reason these windows are not unlinked
* automatically and will result in an assert when the bwin framebuffer
* is closed.
* 
* 7   11/1/12 6:50p tokushig
* SW7231-749: unlink all windows directly decendended from the bwin
* framebuffer.  for some reason these windows are not unlinked
* automatically and will result in an assert when the bwin framebuffer
* is closed.
* 
* 6   10/19/12 12:04p tokushig
* SW7231-749: add ability to show/hide buttons
* 
* 5   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 4   10/7/12 3:32p tokushig
* SW7231-749: add ability to set default msg box button
* 
* 3   8/2/12 11:17a tokushig
* SW7231-749: change destructor
* 
* 2   7/9/12 4:24p tokushig
* SW7231-749: remove msgbox parent widget parameter.  widget will find
* framebuffer bwin internally for msgbox.  this will allow the msgbox to
* display outside the confines of it's parent widget's geometry.
* 
* 1   6/26/12 4:00p tokushig
* SW7231-749: add modal message box capability
* 
* 
*********************************************************************************/

#include "widget_modalmsgbox.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_modalmsgbox);

static int msg_box_bwidget_key_down(bwidget_t widget, const bwidget_key key)
{
    eRet                 ret            = eRet_NotAvailable; /* assume key is not consumed */
    CWidgetModalMsgBox * pMsgBox;
    blabel_settings      labelSettings;

    blabel_get(widget, &labelSettings);
    pMsgBox = (CWidgetModalMsgBox *)labelSettings.widget.data;
    BDBG_ASSERT(NULL != pMsgBox);

    ret = pMsgBox->onKeyDown(widget, key);

    return ((eRet_Ok == ret) ? 0 : -1);
}

CWidgetModalMsgBox::CWidgetModalMsgBox(bwidget_engine_t   engine, 
                                       CWidgetBase      * pParentWidget, 
                                       MRect              geometry,
                                       bwin_font_t        font) :
    CWidgetLabel(engine, pParentWidget, geometry, font, NULL),
    _done(false),
    _pLabel(NULL),
    _pOk(NULL),
    _pCancel(NULL),
    _result()
{
    eRet            ret             = eRet_Ok;
    MRect           rectLabel       = geometry;
    MRect           rectButton      = geometry;
    blabel_settings labelSettings;

    /* modal msg box so capture key down events (for consuming) */
    blabel_get(getWidget(), &labelSettings);
    labelSettings.widget.key_down   = msg_box_bwidget_key_down;
    labelSettings.widget.data       = this;
    blabel_set(getWidget(), &labelSettings);

    setBackgroundColor(0xFF222222);
    setBevel(0);
    setZOrder(getZOrder() + 1);

    rectLabel.setX(5);
    rectLabel.setY(5);
    rectLabel.setWidth(geometry.width() - 10);
    rectLabel.setBottom(geometry.height() - 35);
    _pLabel = new CWidgetLabel(engine, this, rectLabel, font);
    /*_pLabel->setTextColor(0xFF222222);*/

    rectButton.setWidth(75);
    rectButton.setHeight(25);
    rectButton.setX(geometry.width() / 3 - (rectButton.width() / 2));
    rectButton.setY(geometry.height() - 30);
    _pOk = new CWidgetButton(engine, this, rectButton, font);
    ret = _pOk->loadImage("images/menu_bar_gradient.jpg", bwin_image_render_mode_tile);
    CHECK_ERROR_GOTO("unable to set button widget image", ret, error);
    _pOk->setTextColor(0xFFF0F0F0);
    _pOk->setText("Ok");

    rectButton.setWidth(75);
    rectButton.setHeight(25);
    rectButton.setX(geometry.width() / 3 * 2 - (rectButton.width() / 2));
    rectButton.setY(geometry.height() - 30);
    _pCancel = new CWidgetButton(engine, this, rectButton, font);
    ret = _pCancel->loadImage("images/menu_bar_gradient.jpg", bwin_image_render_mode_tile);
    CHECK_ERROR_GOTO("unable to set button widget image", ret, error);
    _pCancel->setTextColor(0xFFF0F0F0);
    _pCancel->setText("Cancel");

    /* reparent bwin to framebuffer so we can draw outside the bounds of our bwidget parent */
    {
        bwin_t          msgBoxWin       = getWin();
        bwin_settings   msgBoxSettings;

        /* traverse up bwin hierarchy to get to framebuffer - save in msgBoxWin */
        bwin_get(msgBoxWin, &msgBoxSettings);
        while (NULL != msgBoxSettings.parent)
        {
            msgBoxWin = msgBoxSettings.parent;
            bwin_get(msgBoxWin, &msgBoxSettings);
        }
        setParentWin(msgBoxWin);
    }
error:
    return;
}

CWidgetModalMsgBox::~CWidgetModalMsgBox()
{
    /* reparent before destroying because all windows directly
       descended from the framebuffer win don't seem to unlink automatically.
       changing the parent win to NULL will force the unlink to occur before
       destruction. */
    setParentWin(NULL);

    DEL(_pLabel);
    DEL(_pOk);
    DEL(_pCancel);
}         

/* given NULL button names will result in that button being hidden */
MString CWidgetModalMsgBox::showModal(const char * msg,
                                      uint8_t      focusButton,
                                      const char * name1, 
                                      const char * name2, 
                                      const char * name3)
{
    uint8_t numVisibleButtons = 0;
    MRect   geometry          = getGeometry();
    MRect   rectButton;

    BDBG_ASSERT(msg);
    BDBG_ASSERT(NULL != name1 || NULL != name2 || NULL != name3);

    _pLabel->setText(msg);

    if (NULL != name1)
    {
    _pOk->setText(name1);
        _pOk->show(true);
        numVisibleButtons++;
    }
    else
    {
        _pOk->show(false);
    }

    if (NULL != name2)
    {
    _pCancel->setText(name2);
        _pCancel->show(true);
        numVisibleButtons++;
    }
    else
    {
        _pCancel->show(false);
    }

    /* DTT TODO: handle button3 if non NULL */

    /* adjust layout depending on the number of visible buttons */
    {
        uint8_t buttonPosition = 1;

        if (NULL != name1)
        {
            rectButton = _pOk->getGeometry();
            rectButton.setX(geometry.width() / (numVisibleButtons + 1) * buttonPosition - (rectButton.width() / 2));
            _pOk->setGeometry(rectButton);
            buttonPosition++;
        }

        if (NULL != name2)
        {
            rectButton = _pCancel->getGeometry();
            rectButton.setX(geometry.width() / (numVisibleButtons + 1) * buttonPosition - (rectButton.width() / 2));
            _pCancel->setGeometry(rectButton);
            buttonPosition++;
        }

        /* DTT TODO: handle button3 if non NULL */
    }

    show(true);
    bwidget_set_modal(getEngine(), getWidget());

    switch (focusButton)
    {
    case 0:
        _pOk->setFocus();
        break;
    case 1:
    default:
        _pCancel->setFocus();
        break;
    }

    _done = false;
    while ((false == _done) && (!bwidget_process_event(getEngine())));

    bwidget_set_modal(getEngine(), NULL);;
    show(false);

    return _result;
}

void CWidgetModalMsgBox::onClick(bwidget_t widget)
{
    if (_pOk->getWidget() == widget)
    {
        _done   = true;
        _result = _pOk->getText();
    }
    else
    if (_pCancel->getWidget() == widget)
    {
        _done   = true;
        _result = _pCancel->getText();
    }
}

eRet CWidgetModalMsgBox::onKeyDown(bwidget_t widget, bwidget_key key)
{
    eRet ret = eRet_Ok; /* assume consume keys */

    if (true == bwidget_is_modal(getEngine(), getWidget()))
    {
        if ((bwidget_key_left == key) || (bwidget_key_right == key))
        {
            ret = eRet_NotAvailable; /* do NOT consume these keys */
        }
    }

    return ret;
}

void CWidgetModalMsgBox::setText(const char * pText, bwidget_justify_horiz justifyHoriz, bwidget_justify_vert justifyVert)
{
    _pLabel->setText(pText, justifyHoriz, justifyVert);
}

