/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwidget_priv.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/26/12 4:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/src/bwidget_priv.h $
 * 
 * 4   6/26/12 4:00p tokushig
 * SW7231-749: add modal message box capability
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
#include "bwidgets.h"
#include <bkni.h>
#include "blst_list.h"

/* Internal enum use for simple RTTI when typecasting from generic
bwidget_t to specific widget. */
typedef enum {
    bwidget_type_base,
    bwidget_type_label,
    bwidget_type_button
} bwidget_type;

typedef struct bwidget_io {
    bwin_io_handle handle;
    bwidget_io_callback callback;
    BLST_D_ENTRY(bwidget_io) link; /* see bwidget_engine.io_handles */
} bwidget_io;

/* Implementation behind the handles. */
struct bwidget_engine {
    bwin_engine_t win_engine;
    bwidget_t focus;
    bwidget_t modal;
    bwidget_t modal_last_focus;
    bool terminated;
    bwidget_engine_settings settings;
    BLST_D_HEAD(widhead, bwidget) widgets; /* have to keep a list of widgets
        so we can look up bwidgets from bwin events */
    BLST_D_HEAD(iohead, bwidget_io) io_handles;
};

struct bwidget {
    bwidget_type type; /* Basic RTTI for verifying handles */
    void *settings; /* This points to the public settings structure for
        each widget. By making it a pointer, we avoid having unsused allocation
        because of nested implementation structures and nested settings structures. */
    bwidget_engine_t engine;
    BLST_D_ENTRY(bwidget) link; /* see bwidget_engine.widgets */
};

/* Each subclass must have its parent implementation as the
first member in order to typecast */

struct bwidget_label {
    struct bwidget widget;
};

struct bwidget_button {
    struct bwidget_label label;
};

#define GET_WIN(WIDGET) \
    ((WIDGET) ? ((bwidget_settings*)(WIDGET)->settings)->win : NULL)

bwidget_t bwidget_p_create(bwidget_engine_t engine, const void *settings, unsigned sizeof_widget, unsigned sizeof_settings, bwidget_type type);
void bwidget_p_verify(bwidget_t widget, bwidget_type type);
void bwidget_p_draw_label_helper(bwidget_t widget, bool down, const bwin_rect *cliprect);
void bwidget_p_set_base_defaults(bwidget_t widget, bwidget_engine_t engine);
void bwidget_p_set_label_defaults(bwidget_t widget, bwidget_engine_t engine);

int bwidget_p_apply_settings(bwidget_t widget, const bwidget_settings *settings);
int blabel_p_apply_settings(bwidget_t widget, const blabel_settings *settings);
int bbutton_p_apply_settings(bwidget_t widget, const bbutton_settings *settings);
