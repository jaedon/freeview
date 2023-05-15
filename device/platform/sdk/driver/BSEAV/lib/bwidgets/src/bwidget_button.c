/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwidget_button.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 10/27/10 11:18a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/src/bwidget_button.c $
 * 
 * 4   10/27/10 11:18a erickson
 * SWDEPRECATED-626: update
 * 
 * 3   10/26/10 11:03a erickson
 * SWDEPRECATED-626: extend bwidgets
 * 
 * 2   10/25/10 10:06a erickson
 * SWDEPRECATED-626: refactor, add image support
 * 
 * 1   2/7/05 9:00p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/3/05 1:03p erickson
 * PR12110: simplified api and adapted to build system changes
 * 
 * Irvine_BSEAVSW_Devel/1   7/29/04 2:07p erickson
 * PR12110: first pass, hello world working
 *
 ****************************************************************/
#include "bwidget_priv.h"

#define PSETTINGS(widget) ((bbutton_settings*)widget->settings)

static void
bwidget_p_draw_button(bwidget_t widget, const bwin_rect *cliprect)
{
    bwidget_p_verify(widget, bwidget_type_button);
    bwidget_p_draw_label_helper(widget, PSETTINGS(widget)->down, cliprect);
}

void
bwidget_p_set_button_defaults(bwidget_t widget, bwidget_engine_t engine)
{
    bwidget_p_verify(widget, bwidget_type_button);
    bwidget_p_set_label_defaults(widget, engine);
}

void bbutton_get_default_settings(bbutton_settings *settings)
{
    BKNI_Memset(settings, 0, sizeof(*settings));
    blabel_get_default_settings(&settings->label);
    
    /* like a label, but we use the down state */
    settings->label.widget.draw = bwidget_p_draw_button;
    settings->label.widget.focusable = true;
}

bwidget_t bbutton_create(bwidget_engine_t engine, const bbutton_settings *settings)
{
    bwidget_t button = bwidget_p_create(engine, settings, sizeof(struct bwidget_button), sizeof(*settings), bwidget_type_button);
    bwidget_p_set_button_defaults(button, engine);
    return button;
}

void
bbutton_get(bwidget_t widget, bbutton_settings *settings)
{
    BKNI_Memcpy(settings, widget->settings, sizeof(*settings));
}

int bbutton_p_apply_settings(bwidget_t widget, const bbutton_settings *settings)
{
    return blabel_p_apply_settings(widget, &settings->label);
}
    
int bbutton_set(bwidget_t widget, const bbutton_settings *settings)
{
    bbutton_p_apply_settings(widget, settings);
    BKNI_Memcpy(widget->settings, settings, sizeof(*settings));
    return 0;
}

void bbutton_click(bwidget_t widget)
{
    if (PSETTINGS(widget)->click) {
        (PSETTINGS(widget)->click)(widget);
    }
}
