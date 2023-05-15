/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwidgets.c $
 * $brcm_Revision: 16 $
 * $brcm_Date: 10/8/12 11:30a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/src/bwidgets.c $
 * 
 * 16   10/8/12 11:30a tokushig
 * SW7231-749: bwidgets now ignores keys if the focused widget is not
 * visible
 * 
 * 15   6/26/12 4:00p tokushig
 * SW7231-749: add modal message box capability
 * 
 * 14   5/25/12 12:18a tokushig
 * SW7231-749: modify move focus operation to calculate based on center of
 * widget.  this better handles moving focus amongst closely spaced
 * widgets of varying sizes.
 * 
 * 13   4/14/12 9:06p tokushig
 * SW7231-749: fix move focus routine to ignore hidden widgets (including
 * those with ancestors who are hidden)
 * 
 * 12   4/14/12 7:22p tokushig
 * SW7231-749: make bwidget_visible() search ancestors visible states
 * 
 * 11   4/13/12 7:16p tokushig
 * SW7231-749: added text justification settings and bwidget/bwin handle
 * access functions
 * 
 * 10   3/2/12 3:53p tokushig
 * SW7405-5581: propagate key presses to active window and its parents if
 * necessary.  allow controller last chance to act on key presses ignored
 * by screens/bwidgets.  move some callback code to cscreen to hide from
 * individual screen classes.
 * 
 * 9   3/1/12 3:53p tokushig
 * SW7405-5581: added show function for widgets
 * 
 * 8   2/22/12 11:38a tokushig
 * SW7405-5581: add bwidgets_remove_io_handle and bwidgets_trigger_io apis
 * 
 * 7   2/12/12 4:52p tokushig
 * SW7405-5581: add context/param to bwidgets idle callback
 * 
 * 6   1/12/12 6:54p tokushig
 * SW7405-5581: fix bwidget_process_event
 * 
 * 5   11/3/10 11:57a erickson
 * SWDEPRECATED-626: restore bwin_get_event timeout
 * 
 * 4   10/27/10 11:19a erickson
 * SWDEPRECATED-626: update
 * 
 * 3   10/26/10 11:03a erickson
 * SWDEPRECATED-626: extend bwidgets
 * 
 * 2   10/22/10 3:21p erickson
 * SWDEPRECATED-626: revive bwidgets on nexus
 * 
 * 1   2/7/05 9:01p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   7/29/04 3:33p erickson
 * PR12110: added user input support, extended demo
 *
 * Irvine_BSEAVSW_Devel/1   7/29/04 2:07p erickson
 * PR12110: first pass, hello world working
 *
 ****************************************************************/
#include "bwidget_priv.h"
BDBG_MODULE(bwidgets);

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

#define PSETTINGS(WIDGET) ((bwidget_settings *)((WIDGET)->settings))

static bwidget_type is_a[] = {
    /* This needs to have bwidget_type as an index
    for bwidget_p_verify to work. */
    bwidget_type_base, /* base is a base */
    bwidget_type_base, /* label is a base */
    bwidget_type_label /* button is a label */
};

void bwidget_p_verify(bwidget_t widget, bwidget_type type)
{
    /* walk the inheritance tree */
    bwidget_type check = widget->type;
    while (1) {
        if (check == type)
            return;
        if (check == bwidget_type_base)
            break;
        check = is_a[check];
    }
    BDBG_ASSERT(false);
}

bwidget_engine_t
bwidget_engine_open(bwin_engine_t win_engine, const bwidget_engine_settings *settings)
{
    bwidget_engine_t engine = (bwidget_engine_t)BKNI_Malloc(
        sizeof(*engine));
    BKNI_Memset(engine, 0, sizeof(*engine));
    engine->win_engine = win_engine;
    engine->terminated = false;
    engine->settings = *settings;
    BLST_D_INIT(&engine->widgets);
    BLST_D_INIT(&engine->io_handles);
    return engine;
}

void
bwidget_engine_close(bwidget_engine_t engine)
{
    /* TODO: Free io_handles */
    BKNI_Free(engine);
}

int
bwidget_run(bwidget_engine_t engine)
{
    while (!engine->terminated && !bwidget_process_event(engine));
    return !engine->terminated;
}

void
bwidget_p_idle(bwidget_engine_t engine)
{
    /* TODO: process timers */
    
    /* Give the application idle time */
    if (engine->settings.idle)
        (*engine->settings.idle)(engine, engine->settings.context, engine->settings.param);
}

bwidget_t
bwidget_p_find_widget(bwidget_engine_t engine, bwin_t win)
{
    bwidget_t widget;

    /* TODO: optimize this search with a hash, or something */
    for (widget = BLST_D_FIRST(&engine->widgets); widget; widget = BLST_D_NEXT(widget, link)) {
        if (GET_WIN(widget) == win) {
            return widget;
        }
    }
    /* TODO: this is normal for the top-level window which covers the
    framebuffer */
    return NULL;
}

void
bwidget_add_io_handle(bwidget_engine_t engine, bwin_io_handle handle,
    bwidget_io_callback io_callback)
{
    bwidget_io *io = (bwidget_io*)BKNI_Malloc(sizeof(bwidget_io));
    io->handle = handle;
    io->callback = io_callback;
    BLST_D_INSERT_HEAD(&engine->io_handles, io, link);

    /* add io handle to bwin so bwidget_process_event()'s call to bwin_get_event() will work properly */
    bwin_add_io_handle(engine->win_engine, handle);
}

void
bwidget_remove_io_handle(bwidget_engine_t engine, bwin_io_handle handle)
{
    bwidget_io *io = NULL;

    bwin_remove_io_handle(engine->win_engine, handle);

    for (io = BLST_D_FIRST(&engine->io_handles);
         NULL != io;
         io = BLST_D_NEXT(io, link))
    {
        if (io->handle == handle)
        {
            BLST_D_REMOVE(&engine->io_handles, io, link);
            break;
        }
    }
}

void 
bwidget_trigger_io(bwidget_engine_t engine, bwin_io_handle handle)
{
    bwin_trigger_io(engine->win_engine, handle);
}

int
bwidget_process_event(bwidget_engine_t engine)
{
    bwin_event event;

    /* TODO: timeout should be based on min(next timer, 20), not just 20 */
    if (bwin_get_event(engine->win_engine, &event, 20)) {
        bwidget_p_idle(engine);
        return 0;
    }

    BDBG_MSG(("event %d", event.type));
    switch (event.type) {
        case bwin_event_type_paint:
            {
            bwidget_settings *settings;
            bwidget_t widget = bwidget_p_find_widget(engine, event.window);
            if (!widget)
                break;
            settings = (bwidget_settings *)widget->settings;
            BDBG_ASSERT(settings);
            if (settings->draw) {
                /* widget->drawBackground(clip); */
                BDBG_MSG(("draw"));
                (*settings->draw)(widget, &event.paint.rect);
            }
            }
            break;

        case bwin_event_type_io:
            {
            /* look up io handle and fire callback */
            bwidget_io *io;
            for (io = BLST_D_FIRST(&engine->io_handles); io; io = BLST_D_NEXT(io, link)) {
                if (io->handle == event.io.handle) {
                    (*io->callback)(engine, io->handle);
                    break;
                }
            }
            }
            break;
    }
    return 0;
}

void
bwidget_terminate(bwidget_engine_t engine)
{
    engine->terminated = true;
}

int
bwidget_enter_key(bwidget_engine_t engine, bwidget_key key, bool down)
{
    bwidget_t widget = engine->focus;
    if (!widget)
        return -1;

    if (!bwidget_visible(widget))
        return -1;

    if (key == bwidget_key_select && widget->type == bwidget_type_button) {
        bbutton_click(widget);
        return 0;
    }
    
    if (down) {
        /* see if the widget wants to process this. if not, send to
        parent. if no window wants it, then see if the window_engine wants it
        (i.e. to move focus) */
        while (NULL != widget) {
            if (0 == PSETTINGS(widget)->key_down(widget, key)) {
                return 0;
            }

            widget = bwidget_get_parent(widget);
        }

        switch (key) {
        case bwidget_key_up:
            bwidget_move_focus(engine, -1, 0); return 0;
        case bwidget_key_down:
            bwidget_move_focus(engine, 1, 0); return 0;
        case bwidget_key_left:
            bwidget_move_focus(engine, 0, -1); return 0;
        case bwidget_key_right:
            bwidget_move_focus(engine, 0, 1); return 0;
        default:
            break;
        }
    }

    return -1;
}

void b_get_global_rect(bwin_t win, bwin_rect *rect)
{
    bwin_settings settings;
    bwin_get(win, &settings);
    if (!settings.parent) {
        rect->x = settings.x;
        rect->y = settings.y;
        rect->width = settings.rect.width;
        rect->height = settings.rect.height;
    }
    else {
        b_get_global_rect(settings.parent, rect);
        rect->x += settings.x;
        rect->y += settings.y;
        rect->width = settings.rect.width;
        rect->height = settings.rect.height;
    }
}

#define abs(x) ((x)<0?-(x):(x))

int
bwidget_move_focus(bwidget_engine_t engine, int vertical, int horizontal)
{
    bwidget_t widget;
    bwin_rect start;
    int sx, sy, sr, sb;
    int lastdist = 0x7FFFFFFF;
    bwidget_t lastwidget = NULL;
    
    BDBG_MSG(("bwidget_move_focus %p: %d %d", engine->focus, vertical, horizontal));
    if (!engine->focus) {
        /* no focus, so just find one */
        for (widget = BLST_D_FIRST(&engine->widgets); widget; widget = BLST_D_NEXT(widget, link)) {
            if (PSETTINGS(widget)->focusable) {
                return bwidget_set_focus(widget);
            }
        }
    }
    
    b_get_global_rect(PSETTINGS(engine->focus)->win, &start);
    sx = start.x;
    sy = start.y;
    sr = start.x+start.width;
    sb = start.y+start.height;
        
    for (widget = BLST_D_FIRST(&engine->widgets); widget; widget = BLST_D_NEXT(widget, link)) {
        bwin_rect test;
        int wx, wy, wr, wb, dx, dy, dist;
        
        if (widget == engine->focus || !PSETTINGS(widget)->focusable || !PSETTINGS(widget)->enabled ||!bwidget_visible(widget) /*PSETTINGS(widget)->window.visible*/) {
            continue;
        }
        
        b_get_global_rect(PSETTINGS(widget)->win, &test);
        
        wx = test.x;
        wy = test.y;
        wr = test.x+test.width;
        wb = test.y+test.height;
        BDBG_MSG_TRACE(("bwidget_move_focus %d,%d,%d,%d => %d,%d,%d,%d", sx,sy,sr,sb, wx,wy,wr,wb));

        /* default deltas */
        dx = sx-wx;
        dy = sy-wy;        
        
        if (vertical==0 && horizontal<0) { /* left */
            dx = sx - wr;
            if (dx <= -(int)start.width) continue;
        }
        else if (vertical==0 && horizontal>0) { /* right */
            dx = wx - sr;
            if (dx <= -(int)start.width) continue;
        }
        else if (vertical<0 && horizontal==0) { /* up */
            dy = sy - wb;
            if (dy <= -(int)start.height) continue;
        }
        else if (vertical>0 && horizontal==0) { /* down */
            dy = wy - sb;
            if (dy <= -(int)start.height) continue;
        }   
        
        /* calc dist based on center of start and widget */
        {
            int wMidX, wMidY;
            int sMidX, sMidY;
            wMidX = (test.x + test.width) / 2;
            wMidY = (test.y + test.height) / 2;
            sMidX = (start.x + start.width) / 2;
            sMidY = (start.y + start.height) / 2;

            dist = abs(sMidX - wMidX) + abs(sMidY - wMidY);
        }

        if (dist < lastdist) {
            lastdist = dist;
            lastwidget = widget;
        }
    }
    
    if (lastwidget) {
        return bwidget_set_focus(lastwidget);
    }
    
    return -1;
}

void
bwidget_set_modal(bwidget_engine_t engine, bwidget_t widget)
{
    if (NULL == widget) 
    {
        /* set new focus and remove modality */
        if (NULL != engine->modal_last_focus)
        {
            bwidget_set_focus(engine->modal_last_focus);
        }
        else
        {
            /* we don't know last focused widget to just pick one */
            engine->focus = NULL;
            bwidget_move_focus(engine, 0, -1);
        }

        engine->modal            = NULL;
        engine->modal_last_focus = NULL;
    }
    else
    {
        /* save current focus widget and set given widget modal */
        bwidget_p_verify(widget, bwidget_type_base);
        engine->modal_last_focus = engine->focus;
        engine->modal = widget;
        bwidget_set_focus(widget);
    }
}

bool
bwidget_is_modal(bwidget_engine_t engine, bwidget_t widget)
{
    BDBG_ASSERT(engine);
    BDBG_ASSERT(widget);
    return (engine->modal == widget);
}

int
bwidget_set_focus(bwidget_t widget)
{
    bwidget_p_verify(widget, bwidget_type_base);
    if (!PSETTINGS(widget)->focusable)
        return -1;

    if (widget->engine->modal)
    {
        /* a widget is currently modal so restrict focus changes to
           that widget and it's ancestors */
        if (false == bwidget_is_parent(widget, widget->engine->modal))
        {
            return -1;
        }
    }

    /* blur if already focused */
    if (widget->engine->focus) {
        bwidget_t w = widget->engine->focus;
        bwidget_settings *settings = (bwidget_settings *)w->settings;
        if (settings->blur)
            (*settings->blur)(w);
        bwin_repaint(GET_WIN(w), NULL);
    }

    /* apply focus */
    widget->engine->focus = widget;
    bwin_repaint(GET_WIN(widget), NULL);
    if (PSETTINGS(widget)->focus)
        (*PSETTINGS(widget)->focus)(widget);
    return 0;
}

bwidget_t 
bwidget_get_focus(bwidget_engine_t engine)
{
    return engine->focus;
}

void
bwidget_show(bwidget_t widget, bool show)
{
    bwin_settings bwinSettings;

    bwidget_p_verify(widget, bwidget_type_base);
    bwin_get(GET_WIN(widget), &bwinSettings);
    bwinSettings.visible = show;
    bwin_set(GET_WIN(widget), &bwinSettings);

    bwin_repaint(GET_WIN(widget), NULL);
}

bool
bwidget_is_parent(bwidget_t widget, bwidget_t parent)
{
    bwin_settings bwinSettings;
    bwin_t        bwinParent;
    bool          bAncestor     = false;

    bwidget_p_verify(widget, bwidget_type_base);
    bwidget_p_verify(parent, bwidget_type_base);

    bwin_get(GET_WIN(widget), &bwinSettings);
    bwinParent = GET_WIN(parent);

    while (NULL != bwinSettings.parent)
    {
        if (bwinSettings.parent == bwinParent)
        {
            /* found matching parent */
            bAncestor = true;
            break;
        }

        bwin_get(bwinSettings.parent, &bwinSettings);
    }

    return bAncestor;
}

bool
bwidget_visible(bwidget_t widget)
{
    bwin_settings bwinSettings;
    bool          visible = false;

    bwidget_p_verify(widget, bwidget_type_base);
    bwin_get(GET_WIN(widget), &bwinSettings);
    visible = bwinSettings.visible;

    if (true == visible)
    {
        /* widget is visible, so check to see if any ancestors are hidden */
        while (NULL != bwinSettings.parent)
        {
            bwin_get(bwinSettings.parent, &bwinSettings);
            visible = bwinSettings.visible;
            if (false == visible)
            {
                /* found hidden ancestor */
                break;
            }
        }
    }

    return visible;
}

bwin_t bwidget_win(bwidget_t widget)
{
    return ((bwidget_settings *)widget->settings)->win;
}

bwin_engine_t bwidget_win_engine(bwidget_engine_t widget_engine)
{
    return ((struct bwidget_engine *)widget_engine)->win_engine;
}

bwidget_t bwidget_get_parent(bwidget_t widget)
{
    bwidget_t parent = NULL;
    bwin_settings settings;
    BDBG_ASSERT(widget);

    bwin_get(PSETTINGS(widget)->win, &settings);
    if (settings.parent)
    {
        parent = bwidget_p_find_widget(widget->engine, settings.parent);
    }

    return parent;
}
