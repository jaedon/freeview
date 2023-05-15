/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_label.cpp $
* $brcm_Revision: 10 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_label.cpp $
* 
* 10   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 9   8/8/12 4:57p tokushig
* SW7231-749: add font get/set methods
* 
* 8   8/2/12 11:02a tokushig
* SW7231-749: Add missing destructor to label class
* 
* 7   7/9/12 4:16p tokushig
* SW7231-749: change label key handling to first try handling within the
* label itself and then propagate up the heirarchy
* 
* 6   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 5   4/16/12 5:52p tokushig
* SW7231-749: add progess label widget
* 
* 4   4/14/12 7:26p tokushig
* SW7231-749: change the way click/keydown callbacks execute and modify
* way listview widget redirects keydown events to itself (so it can do
* scrolling internally)
* 
* 2   4/13/12 7:25p tokushig
* SW7231-749: add widget features
* 
* 1   4/11/12 12:13p tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#include "widget_label.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_label);

static int keyDownCallback(bwidget_t widget, bwidget_key key)
{
    eRet          ret     = eRet_NotAvailable; /* assume key is not consumed */
    CWidgetBase * pWidget = NULL;
    CWidgetBase * pParent = NULL;

    bwidget_settings bwidgetSettings;
    bwidget_get_settings(widget, &bwidgetSettings);
    pWidget = (CWidgetBase *)bwidgetSettings.data;
    BDBG_ASSERT(NULL != pWidget);
    
    /* call widget's key press handler - return code determines
       whether key is consumed or will be propogated further. */
    ret = pWidget->onKeyDown(widget, key);

    return ((eRet_Ok == ret) ? 0 : -1);
}

CWidgetLabel::CWidgetLabel(bwidget_engine_t   engine, 
                           CWidgetBase      * pParentWidget, 
                           MRect              geometry,
                           bwin_font_t        font,
                           bwin_t             parentWin) : /* optional */
    CWidgetBase(engine, pParentWidget),
    _image(NULL)
{
    blabel_settings labelSettings;
    BDBG_ASSERT((NULL != pParentWidget) || (NULL != parentWin));

    memset(_pText, 0, LABEL_MAX_TEXT);

    blabel_get_default_settings(&labelSettings);
    labelSettings.widget.window.parent       = (NULL == parentWin) ? pParentWidget->getWin() : parentWin,
    labelSettings.widget.window.x            = geometry.x();
    labelSettings.widget.window.y            = geometry.y();
    labelSettings.widget.window.rect.width   = geometry.width();
    labelSettings.widget.window.rect.height  = geometry.height();
    labelSettings.font                       = font;
    labelSettings.bevel                      = 3;
    labelSettings.bevel_color[bevel_eTop]    = 0xFFAAAAAA;
    labelSettings.bevel_color[bevel_eRight]  = 0xFFAAAAAA;
    labelSettings.bevel_color[bevel_eBottom] = 0xFFAAAAAA;
    labelSettings.bevel_color[bevel_eLeft]   = 0xFFAAAAAA;
    labelSettings.text                       = _pText;
    labelSettings.text_color                 = 0xFF676767;
    labelSettings.text_justify_horiz         = bwidget_justify_horiz_left;
    labelSettings.text_margin                = 2;
    labelSettings.background_color           = 0xFFF0F0F0;
    labelSettings.widget.key_down            = keyDownCallback;
    labelSettings.widget.data                = this; /* save ptr to this object */
    _widget                                  = blabel_create(getEngine(), &labelSettings);
}

CWidgetLabel::~CWidgetLabel()
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

MRect CWidgetLabel::getGeometry()
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    return MRect(labelSettings.widget.window.x,
                 labelSettings.widget.window.y,
                 labelSettings.widget.window.rect.width,
                 labelSettings.widget.window.rect.height);
}

void CWidgetLabel::setGeometry(MRect geometry)
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    labelSettings.widget.window.x            = geometry.x();
    labelSettings.widget.window.y            = geometry.y();
    labelSettings.widget.window.rect.width   = geometry.width();
    labelSettings.widget.window.rect.height  = geometry.height();
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

uint32_t CWidgetLabel::getBackgroundColor()
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    return labelSettings.background_color;
}

void CWidgetLabel::setBackgroundColor(uint32_t color)
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    labelSettings.background_color = color;
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

uint32_t CWidgetLabel::getTextColor()
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    return labelSettings.text_color;
}

void CWidgetLabel::setTextColor(uint32_t color)
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    labelSettings.text_color = color;
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetLabel::setText(const char * pText, bwidget_justify_horiz justifyHoriz, bwidget_justify_vert justifyVert)
{
    blabel_settings labelSettings;
    bool            change         = false;

    if (NULL == pText)
    {
        _pText[0] = '\0';
    }
    else
    {
        strncpy(_pText, pText, LABEL_MAX_TEXT);
    }

    blabel_get(getWidget(), &labelSettings);
    if (bwidget_justify_horiz_max != justifyHoriz)
    {
        labelSettings.text_justify_horiz = justifyHoriz;
        change = true;
    }
    if (bwidget_justify_vert_max != justifyVert)
    {
        labelSettings.text_justify_vert = justifyVert;
        change = true;
    }

    if (true == change)
    {
        blabel_set(getWidget(), &labelSettings);
    }

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

int CWidgetLabel::getBevel()
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    return labelSettings.bevel;
}

void CWidgetLabel::setBevel(int bevel)
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    labelSettings.bevel = bevel;
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetLabel::setTextMargin(int margin)
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    labelSettings.text_margin = margin;
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

bwin_font_t CWidgetLabel::getFont()
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    return buttonSettings.label.font;
}

void CWidgetLabel::setFont(bwin_font_t font)
{
    blabel_settings labelSettings;

    blabel_get(getWidget(), &labelSettings);
    labelSettings.font = font;
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

eRet CWidgetLabel::loadImage(const char * filename, bwin_image_render_mode renderMode)
{
    eRet            ret           = eRet_Ok;
    blabel_settings labelSettings;

    BDBG_ASSERT(NULL != filename);

    if (NULL != _image)
    {
        bwin_image_close(_image);
    }

    _image = bwin_image_load(getWinEngine(), filename);
    CHECK_PTR_GOTO("unable to load bwin image", _image, ret, eRet_ExternalError, error);

    blabel_get(getWidget(), &labelSettings);
    labelSettings.image = _image;
    labelSettings.render_mode = renderMode;
    blabel_set(getWidget(), &labelSettings);

    bwin_repaint(bwidget_win(getWidget()), NULL);

error:
    return ret;
}
