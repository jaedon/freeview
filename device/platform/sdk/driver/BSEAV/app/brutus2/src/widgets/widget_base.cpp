/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_base.cpp $
* $brcm_Revision: 7 $
* $brcm_Date: 10/7/12 3:25p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_base.cpp $
* 
* 7   10/7/12 3:25p tokushig
* SW7231-749: add method to get parent bwin win
* 
* 6   7/9/12 4:13p tokushig
* SW7231-749: fixed setParent and setZOrder settings to apply properly
* 
* 5   5/25/12 12:26a tokushig
* SW7231-749: added additional support for bwidget features for all
* widget classes
* 
* 4   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 3   4/14/12 5:24p tokushig
* SW7231-749: refactor widget classes to hide keypress/click callback
* mechanism and simplify screen creation.  also added new menu widget.
* 
* 2   4/13/12 7:25p tokushig
* SW7231-749: add widget features
* 
* 1   4/11/12 12:12p tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#include "widget_base.h"

CWidgetBase::CWidgetBase(bwidget_engine_t engine, CWidgetBase * pParent) :
    _engine(engine),
    _pParent(pParent),
    _widget(NULL),
    _value(-1)
{
}

void CWidgetBase::show(bool bShow)
{
    bwidget_show(getWidget(), bShow);
}

bool CWidgetBase::isVisible()
{
    return bwidget_visible(getWidget());
}

/* returns true if focusable, false otherwise */
bool CWidgetBase::setFocus()
{
    return (bwidget_set_focus(getWidget()) != -1) ? true : false;
}

void CWidgetBase::setFocusable(bool bFocusable)
{
    bwidget_settings settings;
    bwidget_get_settings(getWidget(), &settings);
    settings.focusable = bFocusable;
    bwidget_set_settings(getWidget(), &settings);
}

bwin_t CWidgetBase::getParentWin()
{
    bwidget_settings bwidgetSettings;

    bwidget_get_settings(getWidget(), &bwidgetSettings);
    return bwidgetSettings.window.parent;
}

void CWidgetBase::setParentWin(bwin_t parentWin)
{
    bwidget_settings bwidgetSettings;

    bwidget_get_settings(getWidget(), &bwidgetSettings);
    bwidgetSettings.window.parent = parentWin;
    bwidget_set_settings(getWidget(), &bwidgetSettings);

    bwin_set(getWin(), &bwidgetSettings.window);
}

int CWidgetBase::getZOrder()
{
    bwidget_settings settings;
    bwidget_get_settings(getWidget(), &settings);

    return settings.window.zorder;
}

void CWidgetBase::setZOrder(int zorder)
{
    bwidget_settings settings;
    bwidget_get_settings(getWidget(), &settings);
    settings.window.zorder = zorder;
    bwidget_set_settings(getWidget(), &settings);

    bwin_set(getWin(), &settings.window);
}

/* returns eRet_Ok to consume keypress, otherwise key is propogated up heirarchy */
eRet CWidgetBase::onKeyDown(bwidget_t widget, bwidget_key key)
{
    return eRet_NotSupported;
}

void CWidgetBase::onClick(bwidget_t widget)
{
    return;
}
