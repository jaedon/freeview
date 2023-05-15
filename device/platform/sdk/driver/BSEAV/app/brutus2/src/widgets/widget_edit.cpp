/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_edit.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 8/8/12 6:00p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_edit.cpp $
* 
* 3   8/8/12 6:00p tokushig
* SW7231-749: shrink focus indicator size
* 
* 2   5/25/12 12:35a tokushig
* SW7231-749: fixed problem where top/bottom vertical justification were
* switched.
* 
* 1   4/20/12 3:58p tokushig
* SW7231-749: added edit widgets
* 
* 
*********************************************************************************/

#include "screen.h"
#include "widget_edit.h"
#include "widget_base.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_edit);


static int edit_bwidget_key_down(bwidget_t widget, const bwidget_key key)
{
    eRet              ret            = eRet_NotAvailable; /* assume key is not consumed */
    CWidgetEdit     * pEdit;
    blabel_settings   labelSettings;

    blabel_get(widget, &labelSettings);
    pEdit = (CWidgetEdit *)labelSettings.widget.data;
    BDBG_ASSERT(NULL != pEdit);

    ret = pEdit->onKeyDown(widget, key);

    return ((eRet_Ok == ret) ? 0 : -1);
}

/* override default button draw routine to add cursor */
static void edit_bwidget_draw(bwidget_t widget, const bwin_rect *cliprect)
{
    bwin_t win = bwidget_win(widget);
    bwin_settings win_settings;
    int right, bottom;
    blabel_settings * label_settings;
    blabel_settings labelSettings;
    CWidgetEdit * pEdit;
    bool down = false;

    blabel_get(widget, &labelSettings);
    label_settings = &labelSettings;

    pEdit = (CWidgetEdit *)label_settings->widget.data;
    BDBG_ASSERT(NULL != pEdit);
    
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

    if (widget == bwidget_get_focus(pEdit->getEngine())) {
        /* draw focus */
        int i;
        for (i=0;i<2;i++) {
            bwin_draw_line(win, i, i, win_settings.rect.width-i, i,
                label_settings->focus_color, cliprect);
            bwin_draw_line(win, right-i-1, i, right-i-1, win_settings.rect.height-i,
                label_settings->focus_color, cliprect);
            bwin_draw_line(win, i, bottom-i-1, win_settings.rect.width-i, bottom-i-1,
                label_settings->focus_color, cliprect);
            bwin_draw_line(win, i, i, i, win_settings.rect.height-i,
                label_settings->focus_color, cliprect);
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
            x -= label_settings->bevel;
        }
        else { /* bwidget_justify_horiz_left */
            x += label_settings->text_margin;
            x += label_settings->bevel;
        }

        MString drawText = label_settings->text;
        int drawWidth  = 0; 
        int drawHeight = 0; 
        int drawBase   = 0; 

        /* draw text before cursor */
        if (0 < pEdit->getPosition() && drawText.length())
        {
            bwin_measure_text(label_settings->font, label_settings->text, pEdit->getPosition(),
                &drawWidth, &drawHeight, &drawBase);
            bwin_draw_text(win, label_settings->font, x, y,
                label_settings->text, pEdit->getPosition(), label_settings->text_color,
                cliprect);
        }

        /* draw cursor */
        if (widget == bwidget_get_focus(pEdit->getEngine()))
        {
            int margin      = label_settings->bevel;
            int widthCursor = 2;
            bwin_rect rectCursor = { x + drawWidth - widthCursor / 2, 
                                     margin, 
                                     widthCursor, 
                                     win_settings.rect.height - margin * 2 };
            bwin_fill_rect(win, &rectCursor, pEdit->getTextColor(), cliprect);
        }

        /* draw text after cursor */
        if (drawText.length() > pEdit->getPosition() && drawText.length())
        {
            bwin_draw_text(win, label_settings->font, x + drawWidth, y,
                ((const char *)label_settings->text) + pEdit->getPosition(), 
                drawText.length() - pEdit->getPosition(), 
                label_settings->text_color, cliprect);
        }
    }
}

CWidgetEdit::CWidgetEdit(bwidget_engine_t   engine, 
                         CWidgetBase      * pParentWidget,
                         MRect              geometry,
                         bwin_font_t        font,
                         bwin_t             parentWin) : /* optional */
    CWidgetButton(engine, pParentWidget, geometry, font, parentWin),
    _position(0)
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.label.widget.draw        = edit_bwidget_draw;
    buttonSettings.label.widget.key_down    = edit_bwidget_key_down;
    buttonSettings.label.text_justify_horiz = bwidget_justify_horiz_right;
    buttonSettings.label.text_margin        = 5;
    bbutton_set(getWidget(), &buttonSettings);
}

MString CWidgetEdit::getText()
{
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    return MString(buttonSettings.label.text);
}

eRet CWidgetEdit::onKeyDown(bwidget_t widget, bwidget_key key)
{
    eRet            ret            = eRet_NotAvailable; /* assume key is not consumed */
    blabel_settings labelSettings;

    blabel_get(widget, &labelSettings);

    switch (key)
    {
    case bwidget_key_left:
        if (0 < getPosition())
        {
            if (labelSettings.text_justify_horiz == bwidget_justify_horiz_right)
            {
                setPosition(getPosition() - 1);
            }
            else
            {
                removeChar(&_position, false);
            }
            ret = eRet_Ok; /* consume key */
        }
        break;

    case bwidget_key_right:
        if (getPosition() < MString(_pText).length())
        {
            if (labelSettings.text_justify_horiz == bwidget_justify_horiz_right)
            {
                removeChar(&_position, true);
            }
            else
            {
                setPosition(getPosition() + 1);
            }
            ret = eRet_Ok; /* consume key */
        }
        break;

    default:
        if (32 <= key && 255 >= key)
        {
            BDBG_WRN(("TTTTTTTTTT widget edit ascii key received! 0x%X", key));
            addChar(key, &_position);
            ret = eRet_Ok; /* consume key */
        }
        break;
    }

    return ret;
}

void CWidgetEdit::setPosition(uint16_t position)
{
    _position = position;
    bwin_repaint(bwidget_win(getWidget()), NULL);
}

void CWidgetEdit::addChar(char ch, uint16_t * pPosition)
{
    int length = MString(_pText).length();

    if (length < EDIT_MAX_TEXT)
    {
        /* shift letters right to make room for new char */
        for (int i = length; i >= *pPosition; i--)
        {
            _pText[i + 1] = _pText[i];
        }
        _pText[*pPosition] = ch;

        (*pPosition)++;
        
        bwin_repaint(bwidget_win(getWidget()), NULL);
    }
}

void CWidgetEdit::removeChar(uint16_t * pPosition, bool right)
{
    int length = MString(_pText).length();

    if (true == right)
    {
        if (*pPosition < length)
        {
            /* shift letters from position to length forward */
            for (int i = *pPosition; i < length; i++)
            {
                _pText[i] = _pText[i + 1];
            }
            bwin_repaint(bwidget_win(getWidget()), NULL);
        }
    }
    else
    {
        if (*pPosition > 0)
        {
            /* shift letters from position to length backwards */
            for (int i = *pPosition; i < length; i++)
            {
                _pText[i - 1] = _pText[i];
            }

            (*pPosition)--;

            bwin_repaint(bwidget_win(getWidget()), NULL);
        }
    }
}
