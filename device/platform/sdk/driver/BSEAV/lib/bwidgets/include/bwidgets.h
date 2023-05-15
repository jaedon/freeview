/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwidgets.h $
 * $brcm_Revision: 17 $
 * $brcm_Date: 6/26/12 4:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/include/bwidgets.h $
 * 
 * 17   6/26/12 4:00p tokushig
 * SW7231-749: add modal message box capability
 * 
 * 16   4/20/12 3:34p tokushig
 * SW7231-749: fix remote number button mapping issue and adjust size of
 * scan menu
 * 
 * 15   4/20/12 2:45p tokushig
 * SW7231-749: add support for scan menu
 * 
 * 14   4/17/12 6:44p tokushig
 * SW7231-749: add exit key handling - closes main menu
 * 
 * 13   4/17/12 4:36p tokushig
 * SW7231-749: menu button on remote will show/hide menu.  on screen
 * channel number will timeout in 3 secs if the menu is not shown.  if
 * the menu is shown, the channel number will always be displayed.
 * 
 * 12   4/14/12 5:17p tokushig
 * SW7231-749: add optional user data pointer to each bwidget
 * 
 * 11   4/13/12 7:16p tokushig
 * SW7231-749: added text justification settings and bwidget/bwin handle
 * access functions
 * 
 * 10   4/11/12 12:08p tokushig
 * SW7231-749: add justify and margin settings for label/button text
 * 
 * 9   3/2/12 3:53p tokushig
 * SW7405-5581: propagate key presses to active window and its parents if
 * necessary.  allow controller last chance to act on key presses ignored
 * by screens/bwidgets.  move some callback code to cscreen to hide from
 * individual screen classes.
 * 
 * 8   3/1/12 3:54p tokushig
 * SW7405-5581: added show function for widgets
 * 
 * 7   2/22/12 11:38a tokushig
 * SW7405-5581: add bwidgets_remove_io_handle and bwidgets_trigger_io apis
 * 
 * 6   2/12/12 4:52p tokushig
 * SW7405-5581: add context/param to bwidgets idle callback
 * 
 * 5   1/12/12 6:58p tokushig
 * SW7405-5581: add key codes and fix function prototypes for get/set
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
 * 1   2/7/05 8:59p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   2/3/05 1:03p erickson
 * PR12110: simplified api and adapted to build system changes
 * 
 * Irvine_BSEAVSW_Devel/2   7/29/04 3:33p erickson
 * PR12110: added user input support, extended demo
 * 
 * Irvine_BSEAVSW_Devel/1   7/29/04 2:06p erickson
 * PR12110: first pass, hello world working
 *
 ****************************************************************/
#ifndef BWIDGETS_H__
#define BWIDGETS_H__

#include "bwin.h"
#include "bwin_font.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=****************************
Basic design principles:

- bwin is exposed in the bwidgets api. To resize a widget, just resize the bwin_t.
- Quasi-inheritance is accomplished using nested structures, just like an
    actual C++ implementation. The base structure must always be first so that
    typecasting is possible.
- Events are implemented using a standard bwidget_callback function pointer
    and a bwidget_t handle parameter.
- There is a default draw operation which can be overridden with the
    bwidget_draw function pointer.
- Instead of providing every possible GUI option to these widgets, we'll keep
    them very small and simple, and allow user customization of the draw operations.
- There are no globals. bwidget_engine_t allows multiple instances of the widget
    set.
- All colors are ARGB8888 values, which is the bwin format.

*******************************/

typedef struct bwidget_engine *bwidget_engine_t;
typedef struct bwidget *bwidget_t;

/**
Callbacks
**/

/* Event from a widget */
typedef void (*bwidget_callback)(bwidget_t widget);

/* Event from the engine (e.g. idle time) */
typedef void (*bwidget_engine_callback)(bwidget_engine_t widget_engine, void * context, int param);

/* Event from an io_handle which interrupted bwin (e.g. user input )*/
typedef void (*bwidget_io_callback)(bwidget_engine_t engine, bwin_io_handle handle);

/* Drawing operation for application overrides */
typedef void (*bwidget_draw)(bwidget_t widget, const bwin_rect *cliprect);

/**********************************
* bwidget_engine functions
*/

typedef struct {
    bwidget_engine_callback   idle; /* idle time callback */
    void                    * context;
    int                       param;
} bwidget_engine_settings;

bwidget_engine_t
bwidget_engine_open(bwin_engine_t win_engine, 
    const bwidget_engine_settings *settings);

void
bwidget_engine_close(bwidget_engine_t engine);

bwidget_t
bwidget_get_focus(bwidget_engine_t engine);

/* Run the main message loop */
int
bwidget_run(bwidget_engine_t engine);

/* Process a single message from the message loop */
int
bwidget_process_event(bwidget_engine_t engine);

/* Terminate the message loop */
void
bwidget_terminate(bwidget_engine_t engine);

void
bwidget_add_io_handle(bwidget_engine_t engine, bwin_io_handle handle,
    bwidget_io_callback io_callback);

void
bwidget_remove_io_handle(bwidget_engine_t engine, bwin_io_handle handle);

void
bwidget_trigger_io(bwidget_engine_t engine, bwin_io_handle handle);

/***********************************
* user input
**/

/* These codes are recognized as input.
Also, ascii codes from 0x20 up to 0x7F are recognized as text input.
*/
typedef enum {
    bwidget_key_invalid,
    bwidget_key_select,
    bwidget_key_up,
    bwidget_key_down,
    bwidget_key_left,
    bwidget_key_right,
    bwidget_key_backspace, /* delete to the left */
    bwidget_key_delete, /* delete from from the right */
    bwidget_key_chup,
    bwidget_key_chdown,
    bwidget_key_play,
    bwidget_key_stop,
    bwidget_key_pause,
    bwidget_key_rewind,
    bwidget_key_fastforward,
    bwidget_key_record,
    bwidget_key_menu,
    bwidget_key_info,
    bwidget_key_exit,
    /* asciii codes */
    bwidget_key_0          = 0x30,
    bwidget_key_1,
    bwidget_key_2,
    bwidget_key_3,
    bwidget_key_4,
    bwidget_key_5,
    bwidget_key_6,
    bwidget_key_7,
    bwidget_key_8,
    bwidget_key_9,
    bwidget_key_max
} bwidget_key;

/* Event from key press */
typedef int (*bwidget_keydown)(bwidget_t widget, const bwidget_key key);

/* Returns 0 if the key was consumed */
int
bwidget_enter_key(bwidget_engine_t engine,
    bwidget_key key, /* the key that is being pressed */
    bool down /* true for keydown, false for keyup */
    );

/**********************************
* bwidget base functions
*/

typedef struct {
    bwin_settings window;
    bwin_t win; /* read-only */
    
    bool focusable;
    bool enabled;

    bwidget_callback focus; /* gain focus */
    bwidget_callback blur; /* lose focus */

    bwidget_draw draw; /* draw has a default implementation which can be overridden. */
    bwidget_keydown key_down; /* key processing callback */
    void * data; /* optional user data */
} bwidget_settings;

void bwidget_get_default_settings(bwidget_settings *settings);
bwidget_t bwidget_create(bwidget_engine_t engine, const bwidget_settings *settings);
void bwidget_get_settings(bwidget_t widget, bwidget_settings *settings);
int bwidget_set_settings(bwidget_t widget, const bwidget_settings *settings);
void bwidget_destroy(bwidget_t widget);

bwidget_t bwidget_get_parent(bwidget_t widget);
bwin_t bwidget_win(bwidget_t widget);
bwin_engine_t bwidget_win_engine(bwidget_engine_t widget_engine);

/**********************************
* bwidget functions
*/

/* returns 0 if focus was set */
int
bwidget_move_focus(bwidget_engine_t engine,
    int vertical, /* -1, 0, or 1 */
    int horizontal /* -1, 0, or 1 */
    );

void
bwidget_set_modal(bwidget_engine_t engine, bwidget_t widget);

bool
bwidget_is_modal(bwidget_engine_t engine, bwidget_t widget);

/* returns 0 if focus was set */
int
bwidget_set_focus(bwidget_t widget);

bwidget_t 
bwidget_get_focus(bwidget_engine_t engine);

bool
bwidget_is_parent(bwidget_t widget, bwidget_t parent);

void
bwidget_show(bwidget_t widget, bool show);

bool
bwidget_visible(bwidget_t widget);


/**********************************
* bwidget text justification
*/

typedef enum {
    bwidget_justify_vert_top,
    bwidget_justify_vert_middle,
    bwidget_justify_vert_bottom,
    bwidget_justify_vert_max
} bwidget_justify_vert;

typedef enum {
    bwidget_justify_horiz_left,
    bwidget_justify_horiz_center,
    bwidget_justify_horiz_right,
    bwidget_justify_horiz_max
} bwidget_justify_horiz;

/**********************************
* bwidget label
*/

typedef struct {
    bwidget_settings widget;
    const char *text; /* only the pointer is stored. the application must guarantee that
        the memory is preserved during the life of the widget */
    bwin_font_t font;
    uint32_t text_color;
    bwidget_justify_vert text_justify_vert;
    bwidget_justify_horiz text_justify_horiz;
    uint32_t text_margin;

    /* image */
    bwin_image_t image;
    bwin_image_render_mode render_mode;
    
    /* drawing (if image == NULL) */    
    uint32_t background_color;
    uint32_t focus_color;
    uint32_t bevel_color[4]; /* top, right, bottom, left */
    int bevel;
} blabel_settings;

void blabel_get_default_settings(blabel_settings *settings);
bwidget_t blabel_create(bwidget_engine_t engine, const blabel_settings *settings);
void blabel_get(bwidget_t widget, blabel_settings *settings);
int blabel_set(bwidget_t widget, const blabel_settings *settings);

/**********************************
* bwidget button
*/

typedef struct {
    blabel_settings label;
    bool down;
    bwidget_callback click;
} bbutton_settings;

void bbutton_get_default_settings(bbutton_settings *settings);
bwidget_t bbutton_create(bwidget_engine_t engine, const bbutton_settings *settings);
void bbutton_get(bwidget_t widget, bbutton_settings *settings);
int bbutton_set(bwidget_t widget, const bbutton_settings *settings);
void bbutton_click(bwidget_t widget);

#ifdef __cplusplus
}
#endif

#endif /* BWIDGETS_H__*/
