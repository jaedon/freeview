/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 9/21/12 8:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel.cpp $ 
* 
* 3   9/21/12 8:07p tokushig
* SW7231-749: add label size adjustability to addLabelPopupButton()
* 
* 2   8/8/12 6:05p tokushig
* SW7231-749: add support for named widgets in listview widgets.  this is
* used to support menu widgets that allow the user to select the visible
* widgets.
* 
* 1   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
*********************************************************************************/
#include "bstd.h"
#include "brutus.h"
#include "panel.h"

BDBG_MODULE(brutus_panel);

CPanel::CPanel(bwidget_engine_t   engine, 
                       CWidgetBase      * pParentWidget, 
                       MRect              geometry,
                       bwin_font_t        font,
                       bwin_t             parentWin) :
    CWidgetLabel(engine, pParentWidget, geometry, font, parentWin),
    _pModelConfig(NULL),
    _pCfg(NULL),
    _focusWidget(NULL)
{
}

void CPanel::show(bool bShow)
{
    if (true == bShow)
    {
        /* showing this panel so restore last focus */
        if (NULL != _focusWidget)
        {
            bwidget_set_focus(_focusWidget);
        }
    }
    else
    {
        if (true == isVisible())
        {
            /* save the current focus before hiding */
            _focusWidget = bwidget_get_focus(getEngine());
        }
    }

    CWidgetLabel::show(bShow);
}

/* creates a button with two labels over it.  allows you to have 2 columns of text on the button. */
eRet CPanel::addDualLabelButton(CWidgetMenu   * pMenu, 
                                const char    *  strName,
                                CWidgetButton ** pButton, 
                                CWidgetLabel  ** pLabel, 
                                CWidgetLabel  ** pValue,
                                bwin_font_t      font)
{
    eRet                       ret        = eRet_Ok;
    CWidgetListViewContainer * pContainer = NULL;

    BDBG_ASSERT(NULL != strName);

    pContainer = new CWidgetListViewContainer(getEngine(), this, MRect(0, 0, 0, 18), font);
    CHECK_PTR_GOTO("unable to allocate button widget", pContainer, ret, eRet_OutOfMemory, error);
    (*pButton) = pContainer;
    pMenu->addButton(*pButton, strName);
    {
        MRect rect   = (*pButton)->getGeometry();
        int   margin = 5;

        rect.setX(margin);
        rect.setY(0);
        /* set width of first text field to be half of the button width */
        rect.setWidth((rect.width() - margin * 2) / 2);

        *pLabel = new CWidgetLabel(getEngine(), *pButton, rect, font); 
        CHECK_PTR_GOTO("unable to allocate label widget", *pLabel, ret, eRet_OutOfMemory, error);
        (*pLabel)->setBevel(0);

        rect.setX(rect.x() + rect.width());

        (*pValue) = new CWidgetLabel(getEngine(), *pButton, rect, font); 
        CHECK_PTR_GOTO("unable to allocate label widget", *pValue, ret, eRet_OutOfMemory, error);
        (*pValue)->setBevel(0);
    }

goto done;
error:
    DEL(pButton);
    DEL(pLabel);
    DEL(pValue);
done:
    return ret;
}


/* creates a button with an edit surrounded by two labels on either side.
   allows you to have 2 columns of text and one edit on a single parent button. */
eRet CPanel::addDualLabelEditButton(CWidgetMenu   *  pMenu, 
                                    const char    *  strName,
                                    CWidgetButton ** pButton, 
                                    CWidgetLabel  ** pLabelLeft, 
                                    CWidgetEdit   ** pEdit,
                                    CWidgetLabel  ** pLabelRight,
                                    bwin_font_t      font)
{
    eRet                       ret        = eRet_Ok;
    CWidgetListViewContainer * pContainer = NULL;

    BDBG_ASSERT(NULL != strName);

    pContainer = new CWidgetListViewContainer(getEngine(), this, MRect(0, 0, 0, 20), font);
    CHECK_PTR_GOTO("unable to allocate button widget", pContainer, ret, eRet_OutOfMemory, error);
    (*pButton) = pContainer;
    pMenu->addButton(*pButton, strName);
    {
        MRect rect   = (*pButton)->getGeometry();
        int   margin = 0;

        rect.setX(margin);
        rect.setY(0);
        /* set width of first text field to be a third of the button width */
        rect.setWidth(80);

        *pLabelLeft = new CWidgetLabel(getEngine(), *pButton, rect, font); 
        CHECK_PTR_GOTO("unable to allocate label widget", *pLabelLeft, ret, eRet_OutOfMemory, error);
        (*pLabelLeft)->setBevel(0);
        (*pLabelLeft)->setTextMargin(5);

        rect.setX(rect.x() + rect.width());
        rect.setWidth(90);

        /* specify 'this' as parent so we get the key/click callbacks but add optional bwin parent
           parameter to maintain proper drawing heirarchy*/
        *pEdit = new CWidgetEdit(getEngine(), this, rect, font, (*pButton)->getWin()); 
        CHECK_PTR_GOTO("unable to allocate edit widget", *pEdit, ret, eRet_OutOfMemory, error);
        (*pEdit)->setBevel(0);
        (*pEdit)->setPosition(0);
        pContainer->setFocusWidget(*pEdit);

        rect.setX(rect.x() + rect.width());
        rect.setWidth(35);

        *pLabelRight = new CWidgetLabel(getEngine(), *pButton, rect, font); 
        CHECK_PTR_GOTO("unable to allocate label widget", *pLabelRight, ret, eRet_OutOfMemory, error);
        (*pLabelRight)->setBevel(0);
        (*pLabelRight)->setTextMargin(5);
    }

goto done;
error:
    DEL(pButton);
    DEL(pLabelLeft);
    DEL(pEdit);
    DEL(pLabelRight);
done:
    return ret;
}


/* creates a button with a label and a popup list widget 
   allows you to have a column of text and popup list on a single parent button. */
eRet CPanel::addLabelPopupButton(CWidgetMenu      *  pMenu,
                                 const char       *  strName,
                                 CWidgetButton    ** pButton,
                                 CWidgetLabel     ** pLabel, 
                                 CWidgetPopupList ** pPopup,
                                 bwin_font_t         font,
                                 uint8_t             labelPercentage)
{
    eRet                       ret        = eRet_Ok;
    CWidgetListViewContainer * pContainer = NULL;

    BDBG_ASSERT(NULL != strName);

    pContainer = new CWidgetListViewContainer(getEngine(), this, MRect(0, 0, 0, 20), font);
    CHECK_PTR_GOTO("unable to allocate button widget", pContainer, ret, eRet_OutOfMemory, error);
    (*pButton) = pContainer;
    pMenu->addButton(*pButton, strName);
    {
        MRect rect     = (*pButton)->getGeometry();
        MRect rectMenu = pMenu->getGeometry();
        int   margin   = 0;

        rect.setX(margin);
        rect.setY(0);
        /* set width of first text field based on given percentage */
        rect.setWidth((rect.width() - margin * 2) * labelPercentage / 100);

        *pLabel = new CWidgetLabel(getEngine(), *pButton, rect, font); 
        CHECK_PTR_GOTO("unable to allocate label widget", *pLabel, ret, eRet_OutOfMemory, error);
        (*pLabel)->setBevel(0);
        (*pLabel)->setTextMargin(5);

        rect.setX(rect.x() + rect.width());
        rect.setWidth(rectMenu.width() - rect.x() - 20);

        /* the popup list parent will be this so we can handle button clicks here, but set the bwin
           parent as pButton so that it will display in the proper place */
        *pPopup = new CWidgetPopupList(getEngine(), this, rect, font, (*pButton)->getWin());
        CHECK_PTR_GOTO("unable to allocate popup list widget", *pPopup, ret, eRet_OutOfMemory, error);
        (*pPopup)->setBevel(0);
        pContainer->setFocusWidget(*pPopup);
    }

goto done;
error:
    DEL(pButton);
    DEL(pLabel);
    DEL(pPopup);
done:
    return ret;
}
