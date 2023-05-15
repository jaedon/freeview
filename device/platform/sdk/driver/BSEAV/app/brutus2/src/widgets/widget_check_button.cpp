/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_check_button.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 8/8/12 6:00p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_check_button.cpp $
* 
* 3   8/8/12 6:00p tokushig
* SW7231-749: shrink focus indicator size
* 
* 2   5/25/12 12:35a tokushig
* SW7231-749: fixed problem where top/bottom vertical justification were
* switched.
* 
* 1   5/4/12 2:05p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 
*********************************************************************************/

#include "screen.h"
#include "widget_check_button.h"
#include "widget_base.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_check_button);


static void clickCallback(bwidget_t widget)
{
    eRet                 ret     = eRet_Ok;
    CWidgetCheckButton * pWidget = NULL;
    CWidgetBase        * pParent = NULL;

    bwidget_settings bwidgetSettings;
    bwidget_get_settings(widget, &bwidgetSettings);
    pWidget = (CWidgetCheckButton *)bwidgetSettings.data;
    BDBG_ASSERT(NULL != pWidget);

    if (true == pWidget->isVisible())
    {
        /* toggle check */
        pWidget->setCheck(pWidget->isChecked() ? false : true);

        pParent = pWidget->getParent();
        if (NULL != pParent)
        {
            /* widget is visible, so pass on click to the parent widget */
            pParent->onClick(widget);
        }
    }
}

static void check_button_bwidget_draw(bwidget_t widget, const bwin_rect *cliprect)
{
    bwin_t win = bwidget_win(widget);
    bwin_settings win_settings;
    int right, bottom;
    blabel_settings *label_settings = NULL;
    bbutton_settings buttonSettings;
    CWidgetCheckButton * pButton = NULL;
    bool down = false;
    
    bbutton_get(widget, &buttonSettings);
    label_settings = &(buttonSettings.label);

    pButton = (CWidgetCheckButton *)buttonSettings.label.widget.data;
    BDBG_ASSERT(NULL != pButton);

    down = buttonSettings.down;

    /* TODO: for efficieny, we may want bwidgets to have access to bwin private structures. */
    bwin_get(win, &win_settings);
    
    /* only draw if something's within the cliprect */
    if (cliprect && !BWIN_RECT_ISINTERSECTING(&win_settings.rect, cliprect)) {
        return;
    }
    
    right = win_settings.rect.x + win_settings.rect.width;
    bottom = win_settings.rect.y + win_settings.rect.height;

    if (label_settings->image) {
        bwin_image_render(win, label_settings->image, label_settings->render_mode, &win_settings.rect, NULL, cliprect);
    } 
    else {
        bwin_fill_rect(win, &win_settings.rect, label_settings->background_color, cliprect);
        
        if (label_settings->bevel) {
            int i;
            /* top */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, i, win_settings.rect.width-i, i,
                    label_settings->bevel_color[down?2:0], cliprect);
            }
            /* right */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, right-i-1, i, right-i-1, win_settings.rect.height-i,
                    label_settings->bevel_color[down?3:1], cliprect);
            }
            /* bottom */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, bottom-i-1, win_settings.rect.width-i, bottom-i-1,
                    label_settings->bevel_color[down?0:2], cliprect);
            }
            /* left */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, i, i, win_settings.rect.height-i,
                    label_settings->bevel_color[down?1:3], cliprect);
            }
        }
    }

    /* draw check box */

    {
        int i        = 0;
        int box_size = pButton->getBoxSize();
        int margin   = 10;
        bwin_rect brect;
        MRect rect(win_settings.rect.x + win_settings.rect.width - box_size - margin,
                   win_settings.rect.y + (win_settings.rect.height / 2) - (box_size / 2),
                   box_size,
                   box_size);
        rect.setX(rect.x() - label_settings->bevel);

        brect.x      = rect.x();
        brect.y      = rect.y();
        brect.width  = rect.width();
        brect.height = rect.height();
        bwin_fill_rect(win, &brect, label_settings->bevel_color[1], cliprect);

        /* if checked then draw check icon */
        if (true == pButton->isChecked()) {
            brect.x      = rect.x() + 2;
            brect.y      = rect.y() + 2;
            brect.width  = rect.width() - 3;
            brect.height = rect.height() - 3;
            bwin_fill_rect(win, &brect, pButton->getCheckColor(), cliprect);
        }
       
        /* draw check box */
        for (i = 0; i < 2; i++) {
            bwin_draw_line(win, rect.x(), rect.y(), rect.x() + rect.width(), rect.y(),
                pButton->getBoxColor(), cliprect);
            bwin_draw_line(win, rect.x() + rect.width(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height(),
                pButton->getBoxColor(), cliprect);
            bwin_draw_line(win, rect.x(), rect.y() + rect.height(), rect.x() + rect.width() + 1, rect.y() + rect.height(),
                pButton->getBoxColor(), cliprect);
            bwin_draw_line(win, rect.x(), rect.y(), rect.x(), rect.y() + rect.height(),
                pButton->getBoxColor(), cliprect);

            rect.setX(rect.x() - 1);
            rect.setY(rect.y() - 1);
            rect.grow(2);
        }
    
        if (widget == bwidget_get_focus(pButton->getEngine())) {
            /* draw focus */
            for (i=0;i<2;i++) {
                bwin_draw_line(win, rect.x(), rect.y(), rect.x() + rect.width(), rect.y(),
                    label_settings->focus_color, cliprect);
                bwin_draw_line(win, rect.x() + rect.width(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height(),
                    label_settings->focus_color, cliprect);
                bwin_draw_line(win, rect.x(), rect.y() + rect.height(), rect.x() + rect.width() + 1, rect.y() + rect.height(),
                    label_settings->focus_color, cliprect);
                bwin_draw_line(win, rect.x(), rect.y(), rect.x(), rect.y() + rect.height(),
                    label_settings->focus_color, cliprect);

                rect.setX(rect.x() - 1);
                rect.setY(rect.y() - 1);
                rect.grow(2);
            }
        }
    }

    if (label_settings->text) {
        int width, height, base, x, y;
        bwin_measure_text(label_settings->font, label_settings->text, -1,
            &width, &height, &base);
        
        /* calculate vertical justification */
        y = 0;
        if (label_settings->text_justify_vert == bwidget_justify_vert_middle) {
            y = (win_settings.rect.height - height) / 2;
        } 
        else if (label_settings->text_justify_vert == bwidget_justify_vert_bottom) {
            y += label_settings->text_margin;
        }
        else { /* bwidget_justify_vert_top */
            y = win_settings.rect.height - height;
            y -= label_settings->text_margin;
        }

        /* calculate horizontal justification */
        x = 0;
        if (label_settings->text_justify_horiz == bwidget_justify_horiz_center) {
            x = (win_settings.rect.width - width) / 2;
        } 
        else if (label_settings->text_justify_horiz == bwidget_justify_horiz_right) {
            x = win_settings.rect.width - width;
            x -= label_settings->text_margin;
        }
        else { /* bwidget_justify_horiz_left */
            x += label_settings->text_margin;
        }

        bwin_draw_text(win, label_settings->font, x, y,
            label_settings->text, -1, label_settings->text_color,
            cliprect);
    }
}

CWidgetCheckButton::CWidgetCheckButton(bwidget_engine_t   engine, 
                                       CWidgetBase      * pParentWidget,
                                       MRect              geometry,
                                       bwin_font_t        font,
                                       bwin_t             parentWin) : /* optional */
    CWidgetButton(engine, pParentWidget, geometry, font, parentWin),
    _bChecked(false),
    _boxColor(0xFF22527A),
    _boxSize(12),
    _checkColor(0xFF80C42F)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.click = clickCallback;
    buttonSettings.label.widget.draw = check_button_bwidget_draw;
    bbutton_set(getWidget(), &buttonSettings);
}

void CWidgetCheckButton::setCheck(bool bChecked)
{
    _bChecked = bChecked;

    /* dtt todo: only have to repaint checkbox area */
    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetCheckButton::setBoxColor(uint32_t color)
{
    _boxColor = color;

    /* dtt todo: only have to repaint checkbox area */
    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetCheckButton::setCheckColor(uint32_t color)
{
    _checkColor = color;

    /* dtt todo: only have to repaint checkbox area */
    bwin_repaint(bwidget_win(getWidget()), NULL);
}
