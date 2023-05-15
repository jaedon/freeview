/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_button.cpp $
* $brcm_Revision: 9 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_button.cpp $
* 
* 9   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 8   8/8/12 4:57p tokushig
* SW7231-749: add font get/set methods
* 
* 7   8/2/12 11:01a tokushig
* SW7231-749: Add missing destructor to button class
* 
* 6   6/26/12 4:00p tokushig
* SW7231-749: add modal message box capability
* 
* 5   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 4   4/14/12 7:26p tokushig
* SW7231-749: change the way click/keydown callbacks execute and modify
* way listview widget redirects keydown events to itself (so it can do
* scrolling internally)
* 
* 3   4/14/12 5:24p tokushig
* SW7231-749: refactor widget classes to hide keypress/click callback
* mechanism and simplify screen creation.  also added new menu widget.
* 
* 2   4/13/12 7:25p tokushig
* SW7231-749: add widget features
* 
* 1   4/11/12 12:13p tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#include "screen.h"
#include "widget_button.h"
#include "widget_base.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_button);


static void clickCallback(bwidget_t widget)
{
    eRet          ret     = eRet_Ok;
    CWidgetBase * pWidget = NULL;
    CWidgetBase * pParent = NULL;

    bwidget_settings bwidgetSettings;
    bwidget_get_settings(widget, &bwidgetSettings);
    pWidget = (CWidgetBase *)bwidgetSettings.data;
    BDBG_ASSERT(NULL != pWidget);

    if (true == pWidget->isVisible())
    {
        pParent = pWidget->getParent();
        if (NULL != pParent)
        {
            /* widget is visible, so pass on click to the parent widget */
            pParent->onClick(widget);
        }
    }
}


CWidgetButton::CWidgetButton(bwidget_engine_t   engine, 
                             CWidgetBase      * pParentWidget,
                             MRect              geometry,
                             bwin_font_t        font,
                             bwin_t             parentWin) : /* optional */
    CWidgetBase(engine, pParentWidget),
    _image(NULL)
{
    bbutton_settings buttonSettings;

    BDBG_ASSERT(NULL != pParentWidget);

    memset(_pText, 0, BUTTON_MAX_TEXT);

    bbutton_get_default_settings(&buttonSettings);
    buttonSettings.label.widget.window.parent       = (NULL == parentWin) ? pParentWidget->getWin() : parentWin;
    buttonSettings.label.widget.window.x            = geometry.x();
    buttonSettings.label.widget.window.y            = geometry.y();
    buttonSettings.label.widget.window.rect.width   = geometry.width();
    buttonSettings.label.widget.window.rect.height  = geometry.height();
    buttonSettings.label.font                       = font;
    buttonSettings.label.bevel                      = 3;
    buttonSettings.label.bevel_color[bevel_eTop]    = 0xFFAAAAAA;
    buttonSettings.label.bevel_color[bevel_eRight]  = 0xFFDCDCDC;
    buttonSettings.label.bevel_color[bevel_eBottom] = 0xFFAAAAAA;
    buttonSettings.label.bevel_color[bevel_eLeft]   = 0xFFDCDCDC;
    buttonSettings.label.text                       = _pText;
    buttonSettings.label.text_color                 = 0xFF676767; /*0xFF82C345;*/
    buttonSettings.label.text_justify_horiz         = bwidget_justify_horiz_center;
    buttonSettings.label.text_justify_vert          = bwidget_justify_vert_middle;
    buttonSettings.label.text_margin                = 2;
    buttonSettings.label.background_color           = 0xFFF0F0F0; /*0xFF222222;*/
    buttonSettings.click                            = clickCallback;
    buttonSettings.label.widget.data                = this; /* save ptr to this object */
    _widget                                         = bbutton_create(getEngine(), &buttonSettings);
}

CWidgetButton::~CWidgetButton()
{
    if (NULL != _widget)
    {
        bwidget_destroy(_widget);
        _widget = NULL;
    }

    if (NULL != _image)
    {
        bwin_image_close(_image);
        _image = NULL;
    }
}

MRect CWidgetButton::getGeometry()
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    return MRect(buttonSettings.label.widget.window.x,
                 buttonSettings.label.widget.window.y,
                 buttonSettings.label.widget.window.rect.width,
                 buttonSettings.label.widget.window.rect.height);
}

void CWidgetButton::setGeometry(MRect geometry)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.widget.window.x            = geometry.x();
    buttonSettings.label.widget.window.y            = geometry.y();
    buttonSettings.label.widget.window.rect.width   = geometry.width();
    buttonSettings.label.widget.window.rect.height  = geometry.height();
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

uint32_t CWidgetButton::getBackgroundColor()
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    return buttonSettings.label.background_color;
}

void CWidgetButton::setBackgroundColor(uint32_t color)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.background_color = color;
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

uint32_t CWidgetButton::getTextColor()
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    return buttonSettings.label.text_color;
}

void CWidgetButton::setTextColor(uint32_t color)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.text_color = color;
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetButton::setText(const char * pText, 
                            bwidget_justify_horiz justifyHoriz,
                            bwidget_justify_vert justifyVert)
{
    bbutton_settings buttonSettings;
    bool             change          = false;

    if (NULL == pText)
    {
        _pText[0] = '\0';
    }
    else
    {
        strncpy(_pText, pText, BUTTON_MAX_TEXT);
    }

    bbutton_get(getWidget(), &buttonSettings);
    if (bwidget_justify_horiz_max != justifyHoriz)
    {
        buttonSettings.label.text_justify_horiz = justifyHoriz;
        change = true;
    }
    if (bwidget_justify_vert_max != justifyVert)
    {
        buttonSettings.label.text_justify_vert = justifyVert;
        change = true;
    }

    if (true == change)
    {
        bbutton_set(getWidget(), &buttonSettings);
    }

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

bwin_font_t CWidgetButton::getFont()
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    return buttonSettings.label.font;
}

void CWidgetButton::setFont(bwin_font_t font)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.font = font;
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetButton::setBevel(int bevel)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.bevel = bevel;
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetButton::setTextMargin(int margin)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.text_margin = margin;
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}


eRet CWidgetButton::loadImage(const char * filename, bwin_image_render_mode renderMode)
{
    eRet            ret           = eRet_Ok;
    bbutton_settings buttonSettings;

    BDBG_ASSERT(NULL != filename);

    if (NULL != _image)
    {
        bwin_image_close(_image);
    }

    _image = bwin_image_load(getWinEngine(), filename);
    CHECK_PTR_GOTO("unable to load bwin image", _image, ret, eRet_ExternalError, error);

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.image       = _image;
    buttonSettings.label.render_mode = renderMode;
    bbutton_set(getWidget(), &buttonSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);

error:
    return ret;
}
