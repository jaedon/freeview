/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwidget_label.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 8/8/12 4:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/src/bwidget_label.c $
 * 
 * 8   8/8/12 4:58p tokushig
 * SW7231-749: shrink size of focus
 * 
 * 7   5/25/12 12:19a tokushig
 * SW7231-749: fixed problem where top/bottom vertical justification were
 * switched.
 * 
 * 6   4/13/12 7:17p tokushig
 * SW7231-749: added label text horiz/vert justify options
 * 
 * 5   4/11/12 12:08p tokushig
 * SW7231-749: add justify and margin settings for label/button text
 * 
 * 4   11/3/10 12:39p erickson
 * SWDEPRECATED-626: improve redraw speed by comparing with cliprect
 * before drawing
 * 
 * 3   10/27/10 11:18a erickson
 * SWDEPRECATED-626: update
 * 
 * 2   10/25/10 10:06a erickson
 * SWDEPRECATED-626: refactor, add image support
 * 
 * 1   2/7/05 9:00p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   2/3/05 1:03p erickson
 * PR12110: simplified api and adapted to build system changes
 * 
 * Irvine_BSEAVSW_Devel/2   7/29/04 3:33p erickson
 * PR12110: added user input support, extended demo
 * 
 * Irvine_BSEAVSW_Devel/1   7/29/04 2:07p erickson
 * PR12110: first pass, hello world working
 *
 ****************************************************************/
#include "bwidget_priv.h"
BDBG_MODULE(bwidget_label);

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

#define PSETTINGS(widget) ((blabel_settings*)widget->settings)

static void
bwidget_p_draw_label(bwidget_t widget, const bwin_rect *cliprect)
{
    /* code reuse for button */
    bwidget_p_draw_label_helper(widget, false, cliprect);
}

void
bwidget_p_draw_label_helper(bwidget_t widget, bool down, const bwin_rect *cliprect)
{
    bwin_t win = GET_WIN(widget);
    bwin_settings win_settings;
    int right, bottom;
    blabel_settings *label_settings = PSETTINGS(widget);
    
    bwidget_p_verify(widget, bwidget_type_label);
    
    /* TODO: for efficieny, we may want bwidgets to have access to bwin private structures. */
    bwin_get(win, &win_settings);
    
    /* only draw if something's within the cliprect */
    if (cliprect && !BWIN_RECT_ISINTERSECTING(&win_settings.rect, cliprect)) {
        return;
    }
    
    right = win_settings.rect.x + win_settings.rect.width;
    bottom = win_settings.rect.y + win_settings.rect.height;
    BDBG_MSG_TRACE(("draw_label: %p %d,%d,%d,%d (%d,%d,%d,%d)", widget, label_settings->widget.window.x, label_settings->widget.window.y, win_settings.rect.width, win_settings.rect.height,
        cliprect->x, cliprect->y, cliprect->width, cliprect->height));

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

    if (widget->engine->focus == widget) {
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
        }
        else { /* bwidget_justify_horiz_left */
            x += label_settings->text_margin;
        }

        bwin_draw_text(win, label_settings->font, x, y,
            label_settings->text, -1, label_settings->text_color,
            cliprect);
    }
}

void
bwidget_p_set_label_defaults(bwidget_t widget, bwidget_engine_t engine)
{
    bwidget_p_verify(widget, bwidget_type_label);
    bwidget_p_set_base_defaults(widget, engine);
}

void blabel_get_default_settings(blabel_settings *settings)
{
    BKNI_Memset(settings, 0, sizeof(*settings));
    bwidget_get_default_settings(&settings->widget);
    settings->bevel = 5;
    settings->widget.draw = bwidget_p_draw_label;
    
    settings->image = NULL;
    settings->render_mode = bwin_image_render_mode_stretch;
    
    settings->background_color = 0xff282277;
    settings->text_color = 0xff999999;
    settings->focus_color = 0xffAAAA00;
    settings->bevel_color[0] = 0xff7070c0;
    settings->bevel_color[1] = 0xff11112d;
    settings->bevel_color[2] = 0xff303040;
    settings->bevel_color[3] = 0xff8080d0;
}

bwidget_t
blabel_create(bwidget_engine_t engine, const blabel_settings *settings)
{
    bwidget_t label = bwidget_p_create(engine, settings, sizeof(struct bwidget_label), sizeof(*settings), bwidget_type_label);
    bwidget_p_set_label_defaults(label, engine);
    return label;
}

void
blabel_get(bwidget_t widget, blabel_settings *settings)
{
    BKNI_Memcpy(settings, widget->settings, sizeof(*settings));
}

int blabel_p_apply_settings(bwidget_t widget, const blabel_settings *settings)
{
    return bwidget_p_apply_settings(widget, &settings->widget);
}

int
blabel_set(bwidget_t widget, const blabel_settings *settings)
{
    blabel_p_apply_settings(widget, settings);
    BKNI_Memcpy(widget->settings, settings, sizeof(*settings));
    return 0;
}

