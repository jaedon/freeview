/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwidget_base.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 3/2/12 3:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/src/bwidget_base.c $
 * 
 * 4   3/2/12 3:53p tokushig
 * SW7405-5581: propagate key presses to active window and its parents if
 * necessary.  allow controller last chance to act on key presses ignored
 * by screens/bwidgets.  move some callback code to cscreen to hide from
 * individual screen classes.
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
 * Irvine_BSEAVSW_Devel/2   7/29/04 3:33p erickson
 * PR12110: added user input support, extended demo
 *
 * Irvine_BSEAVSW_Devel/1   7/29/04 2:06p erickson
 * PR12110: first pass, hello world working
 *
 ****************************************************************/
#include "bwidget_priv.h"

BDBG_MODULE(bwidget_base);

bwidget_t bwidget_p_create(bwidget_engine_t engine, const void *settings, unsigned sizeof_widget, unsigned sizeof_settings, bwidget_type type)
{
    bwidget_t widget;
    BSTD_UNUSED(engine);
    widget = BKNI_Malloc(sizeof_widget);
    widget->settings = BKNI_Malloc(sizeof_settings);
    BKNI_Memcpy(widget->settings, settings, sizeof_settings);
    widget->type = type;
    return widget;
}

#define PSETTINGS(WIDGET) ((bwidget_settings *)((WIDGET)->settings))
void
bwidget_p_set_base_defaults(bwidget_t widget, bwidget_engine_t engine)
{
    bwidget_p_verify(widget, bwidget_type_base);
    
    PSETTINGS(widget)->win = bwin_open(&PSETTINGS(widget)->window);
    
    /* Make sure that the bwin_t is in the same bwin_engine_t as was used
    to create the bwidget_engine_t. */
/*
TODO: add bwin_get_engine(win), then get rid of the engine param
otherwise we can't verify this.
*/
    widget->engine = engine;
    BLST_D_INSERT_HEAD(&engine->widgets, widget, link);
}

void
bwidget_destroy(bwidget_t widget)
{
    BLST_D_REMOVE(&widget->engine->widgets, widget, link);
    BKNI_Free(widget->settings);
    BKNI_Free(widget);
}

static int
bwidget_p_keydown(bwidget_t widget, const bwidget_key key)
{
    BSTD_UNUSED(widget);
    BSTD_UNUSED(key);
    return -1;
}

void bwidget_get_default_settings(bwidget_settings *settings)
{
    BKNI_Memset(settings, 0, sizeof(*settings));
    bwin_settings_init(&settings->window);
    settings->enabled = true;
    settings->draw = NULL; /* nothing to draw */
    settings->window.visible = true;
    settings->key_down = bwidget_p_keydown;
}

bwidget_t
bwidget_create(bwidget_engine_t engine, const bwidget_settings *settings)
{
    bwidget_t widget = bwidget_p_create(engine, settings, sizeof(struct bwidget), sizeof(*settings), bwidget_type_base);
    bwidget_p_set_base_defaults(widget, engine);
    return widget;
}

void
bwidget_get_settings(bwidget_t widget, bwidget_settings *settings)
{
    BKNI_Memcpy(settings, widget->settings, sizeof(*settings));
}

int bwidget_p_apply_settings(bwidget_t widget, const bwidget_settings *settings)
{
    bwin_set(PSETTINGS(widget)->win, &settings->window);
    return 0;
}

int bwidget_set_settings(bwidget_t widget, const bwidget_settings *settings)
{
    BKNI_Memcpy(widget->settings, settings, sizeof(*settings));
    return 0;
}

