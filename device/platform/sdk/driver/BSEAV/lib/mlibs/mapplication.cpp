/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mapplication.cpp $
 * $brcm_Revision: 14 $
 * $brcm_Date: 9/25/12 10:31a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mapplication.cpp $
 * 
 * 14   9/25/12 10:31a erickson
 * SW7435-368: fix compliation for 2.6.18
 * 
 * 13   9/24/12 10:19a erickson
 * SW7435-368: remove DSG_SUPPORT, fix wall clock time problem with
 * CLOCK_MONOTONIC
 * 
 * 12   9/24/12 10:12a erickson
 * SW7435-368: remove long-held mutex
 * 
 * 11   1/19/12 4:10p gmohile
 * SW7425-2213 : Add standby support
 * 
 * 10   5/15/08 12:23p jgarrett
 * PR 42114: Reworking idle lock to use KNI and be more efficient
 * 
 * 9   12/14/07 11:15a jrubio
 * PR38220: sched_yield needed between mutex unlock - mutex lock
 * 
 * 8   1/26/07 4:26p erickson
 * PR26661: processEvent must return false when MApplication is
 * terminating. this allows apps to shut down during modal dialog boxes.
 * 
 * 7   12/21/06 5:35p ahulse
 * PR26203: When in modal state, redraw modal box on entering idle
 * 
 * 6   12/12/06 5:08p haisongw
 * PR26369: mtimer doesn't work after system time gets updated
 *
 * 5   8/25/05 2:43p erickson
 * PR16678: changed #if's to standard #ifdef's
 *
 * 4   8/24/05 6:18p arbisman
 * PR16678: Add support for WinCE
 *
 * 3   4/29/05 11:38a erickson
 * PR15017: remove brutus1Sem
 *
 * 2   4/8/05 7:33p erickson
 * PR14701: tickCount should be static. no state kept.
 *
 * 1   2/7/05 11:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/12   9/9/04 1:39p erickson
 * PR12608: improved debug
 *
 * Irvine_BSEAVSW_Devel/11   7/26/04 6:21p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/11   7/26/04 6:20p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/10   7/23/04 2:29p erickson
 * PR11869: allow MApplication to be restarted in same app
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/4   7/13/04 6:15p liangl
 * PR11740: No playback proceeding indication in the playback bar
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/3   7/7/04 3:32p liangl
 * PR11713: Channel change in App B (SD output) in dual decode causes V/A
 * disppear
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/2   6/18/04 7:36p liangl
 * fixed ifndef bug
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/1   6/18/04 6:37p liangl
 * added vxWorks support
 * $brcm_Log: /BSEAV/lib/mlibs/mapplication.cpp $
 * 
 * 14   9/25/12 10:31a erickson
 * SW7435-368: fix compliation for 2.6.18
 * 
 * 13   9/24/12 10:19a erickson
 * SW7435-368: remove DSG_SUPPORT, fix wall clock time problem with
 * CLOCK_MONOTONIC
 * 
 * 12   9/24/12 10:12a erickson
 * SW7435-368: remove long-held mutex
 * 
 * 11   1/19/12 4:10p gmohile
 * SW7425-2213 : Add standby support
 * 
 * 10   5/15/08 12:23p jgarrett
 * PR 42114: Reworking idle lock to use KNI and be more efficient
 * 
 * 9   12/14/07 11:15a jrubio
 * PR38220: sched_yield needed between mutex unlock - mutex lock
 * 
 * 8   1/26/07 4:26p erickson
 * PR26661: processEvent must return false when MApplication is
 * terminating. this allows apps to shut down during modal dialog boxes.
 * 
 * 7   12/21/06 5:35p ahulse
 * PR26203: When in modal state, redraw modal box on entering idle
 * 
 * 6   12/12/06 5:08p haisongw
 * PR26369: mtimer doesn't work after system time gets updated
 *
 * 5   8/25/05 2:43p erickson
 * PR16678: changed #if's to standard #ifdef's
 *
 * 4   8/24/05 6:18p arbisman
 * PR16678: Add support for WinCE
 *
 * 3   4/29/05 11:38a erickson
 * PR15017: remove brutus1Sem
 *
 * 2   4/8/05 7:33p erickson
 * PR14701: tickCount should be static. no state kept.
 *
 * 1   2/7/05 11:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/12   9/9/04 1:39p erickson
 * PR12608: improved debug
 *
 * Irvine_BSEAVSW_Devel/11   7/26/04 6:21p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/11   7/26/04 6:20p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/9   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/8   4/30/04 3:13p erickson
 * PR10883: MApplication now keeps a list of MFrameBuffers and
 * MFrameBuffer keeps a list of top level widgets
 *
 * Irvine_BSEAVSW_Devel/7   3/8/04 11:31a erickson
 * PR9883: added MApplication name for debugging dual output/single app
 * configuration
 *
 * Irvine_BSEAVSW_Devel/6   2/26/04 11:03a erickson
 * PR9900: now using MListItr to allow const access to MList's
 *
 * Irvine_BSEAVSW_Devel/5   2/18/04 2:01p erickson
 * PR9752: Invisible widgets never have focus, and unset _focus in the
 * widgetDestroyed function
 *
 * Irvine_BSEAVSW_Devel/4   2/5/04 11:51a erickson
 * PR9629: Don't allocate a default style as a global variable.
 *
 * Irvine_BSEAVSW_Devel/3   2/4/04 2:11p erickson
 * PR9107: implemented new device io system
 *
 * Irvine_BSEAVSW_Devel/2   2/3/04 10:05a erickson
 * PR9497: initial vxworks support
 *
 * Irvine_BSEAVSW_Devel/1   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/4   8/13/03 10:42a erickson
 * made setMainWidget truly optional
 * added globalKeyProcessor
 *
 * Irvine_BSEAVSW_Devel/3   8/5/03 7:10a erickson
 * handle onIdle reentrancy
 * better printout if MApplication can't init
 *
 * Irvine_BSEAVSW_Devel/2   7/24/03 6:02p erickson
 * new IdleLock functions to allow other threads to interact with
 * microwidgets code
 *
 * Irvine_BSEAVSW_Devel/16   4/11/03 2:16p erickson
 * added terminate(), changed header
 *
 *************************************************************************/
#include "mapplication.h"
#include "mapp_priv.h"
#include "miodevice.h"
#include <errno.h>
#ifdef LINUX
#include <time.h>
#include <pthread.h>
#elif defined __vxworks
#include <time.h>
#include <vxWorks.h>
#include <semLib.h>
#include <pthread.h>
#else
#define pthread_self() (0)
#endif
#include <assert.h>
#include <stdio.h>


#define IDLE_TIMEOUT 20 // in msec
BDBG_MODULE(mapplication);

void MApplication::getDefaultSettings(Settings *settings)
{
    memset(settings, 0, sizeof(*settings));
}

MApplication::MApplication(const MApplication::Settings *settings, const char *name)
{
    _name = name;
    _valid = false;
    _main = NULL;
    _globalKeyProcessor = NULL;
    _modalState = false;
    _focus = NULL;
    _terminating = false;
    _standby = false;
    _style = &_defaultStyle;
    _lastEventWidget = NULL;

    _private = new MApplication::Private;
    BKNI_Memset(_private, 0, sizeof(*_private));

    BKNI_CreateMutex(&_private->idle.mutex);

    _private->win_engine = settings->win_engine;
    _valid = true;
}

MApplication::~MApplication() {
    BKNI_DestroyMutex(_private->idle.mutex);
    delete _private;
}

int MApplication::run() {
    _terminating = false;
    while (!_terminating && processEvent());
    return 0;
}

void MApplication::widgetDestroyed(MWidget *w) {
    if (w == _lastEventWidget)
        _lastEventWidget = NULL;
    if (w == _focus)
        _focus = NULL;
}

bool MApplication::eventPending() {
    bwin_event event;
    return !bwin_peek_event(_private->win_engine, &event);
}

/* converting from bwin_t to MWidget* can be helped with a cache.
It's most likely to be the last one used. */
MWidget *MApplication::findEventWidget(int wid) const
{
    bwin_t window = (bwin_t)wid;
    MWidget *widget;
    assert(sizeof(int) >= sizeof(bwin_t)); /* otherwise we have problems */
    if (_lastEventWidget && _lastEventWidget->id() == (int)window)
        widget = _lastEventWidget;
    else {
        widget = findWidget((int)window);
    }
    return widget;
}

bool MApplication::processEvent(bool modalState) {
    bwin_event event;
    MWidget *widget;
    
    if (_terminating) return false;

    //printf("processEvent (%s) begin\n", (const char *)_name);
    _modalState = modalState;
    if (bwin_get_event(_private->win_engine, &event, IDLE_TIMEOUT)) {
        //printf("  idle (%s)\n", (const char *)_name);
        if(_standby) return true; //Only process io events in standby
        if( _modalState && _modalWidget ) {
        /* Modal box may have been overdrawn, re-draw it */
        _modalWidget->drawNow();
        }   
        _modalWidget = NULL;
        onIdle();
        return true;
    }

    BDBG_MSG(("processEvent (%s) type %d", (const char *)_name, event.type));
    switch (event.type) {
    case bwin_event_type_paint:
        if(_standby) return true; //Only process io events in standby
        widget = findEventWidget((int)event.window);
        _lastEventWidget = widget;
        if (widget) {
        MRect clip(
            event.paint.rect.x,
            event.paint.rect.y,
            event.paint.rect.width,
            event.paint.rect.height);
        BDBG_MSG(("draw %s clip=%d,%d,%d,%d", widget->name(),
              event.paint.rect.x,
              event.paint.rect.y,
              event.paint.rect.width,
              event.paint.rect.height));
        widget->drawBackground(clip);
        widget->draw(clip);
        
        _modalWidget = NULL;
        if( modalState ) {
            MWidget *focusWidget = this->focusedWidget();   
            if( focusWidget ) 
            _modalWidget = focusWidget->topLevelWidget();
        }
        }
        break;

    case bwin_event_type_io:
        for (MIoDevice *io = _iodevices.first();io;io = _iodevices.next())
        if (event.io.handle == (bwin_io_handle)io) {
            io->ready();
            break;
        }
        break;
    }
    return true;
}

void MApplication::terminate()
{
    if (!_terminating) {
        _terminating = true;
    }
}

void MApplication::standby()
{
    if(!_standby)
    _standby = true;    
}

void MApplication::resume()
{
    if(_standby)
    _standby = false;    
}


void MApplication::sendKeyDown(MWidget *widget, int ascii, int modifiers, int scancode)
{
    // send to focused widget, then to every parent
    // if not used, then do global key processing
    bool used = false;
    while (widget) {
        if (widget->keyDown(ascii, modifiers, scancode))
        {
            used = true;
            break;
        }
        widget = widget->parent();
    }

    // global key processing
    if (!used) {
        switch (ascii) {
        case MWKEY_RIGHT:
            tab(MWidget::tabRight);
            break;
        case MWKEY_DOWN:
            tab(MWidget::tabDown);
            break;
        case MWKEY_TAB:
            if (modifiers & MWKMOD_SHIFT)
                tab(MWidget::tabPrev);
            else
                tab(MWidget::tabNext);
            break;
        case MWKEY_LEFT:
            tab(MWidget::tabLeft);
            break;
        case MWKEY_UP:
            tab(MWidget::tabUp);
            break;
        default:
            if (_globalKeyProcessor)
                _globalKeyProcessor->keyDown(ascii, modifiers, scancode);
            break;
        }
    }
}

MWidget *MApplication::focusedWidget() const {
    /* only allow visible widgets to say they have focus */
    if (_focus && _focus->visible())
        return _focus;
    else
        return NULL;
}

void MApplication::setFocus(MWidget *widget) {
    MWidget *temp = _focus;

    /* have to set the new focus before calling focusRepaint on the blurred widget */
    _focus = widget;
    if (temp && temp->visible())
        temp->focusRepaint();
    if (_focus && _focus->visible())
        _focus->focusRepaint();
}

MWidget *MApplication::findWidget(int wid) const {
    MListItr<MFrameBuffer> itrfb(&_fbs);
    for (MFrameBuffer *fb = itrfb.first(); fb; fb = itrfb.next()) {
        MListItr<MWidget> itr(&fb->_topWidgets);
        for (MWidget *w = itr.first(); w; w = itr.next()) {
            if (w->id() == wid)
                return w;
            w = w->findDescendentById(wid);
            if (w)
                return w;
        }
    }
    return NULL;
}

void MApplication::tab(MWidget::TabDirection dir) {
    // first find the focused widget in the list
    MWidget *w = focusedWidget();
    if (w) {
        w->topLevelWidget()->tab(w, dir);
    }
    else {
        // get the first top level widget we can find
        for (MFrameBuffer *fb = _fbs.first(); fb; fb = _fbs.next()) {
            w = fb->_topWidgets[0];
            if (w) {
                setFocus(w);
                break;
            }
        }
    }
}

unsigned long MApplication::tickCount() {
#if defined _WIN32_WCE
    return GetTickCount();
#else
    struct timespec tv;
    int rc;
    /* require CLOCK_MONOTONIC so we survive wall clock time changes. */
    rc = clock_gettime(CLOCK_MONOTONIC, &tv);
    if (rc) {
        BERR_TRACE(rc);
        return 0;
    }
    return tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
#endif
}

void MApplication::acquireIdleLock()
{
    unsigned rptr;
    bool locked = false;
    
    /* Tell the app that we're acquiring the idle lock so that it will yield */
    BKNI_AcquireMutex(_private->idle.mutex);
    rptr = _private->idle.wptr;
    _private->idle.request[rptr] = true;
    if (++_private->idle.wptr == MAPP_MAX_IDLE_REQUEST) {
        _private->idle.wptr = 0;
    }
    BDBG_MSG(("thread %p waiting on idle.request[%d]", pthread_self(), rptr));
    /* if we assert here, expand MAPP_MAX_IDLE_REQUEST */
    BDBG_ASSERT(_private->idle.wptr != _private->idle.rptr);
    BKNI_ReleaseMutex(_private->idle.mutex);
    
    while (1) {
        BKNI_AcquireMutex(_private->idle.mutex);
        locked = !_private->idle.request[rptr];
        BKNI_ReleaseMutex(_private->idle.mutex);
        if (locked) break;
        BKNI_Sleep(1);
    }
}

void MApplication::releaseIdleLock()
{
    BKNI_AcquireMutex(_private->idle.mutex);
    BDBG_MSG(("thread %p completing idle.request[%d]", pthread_self(), _private->idle.rptr));
    if (++_private->idle.rptr == MAPP_MAX_IDLE_REQUEST) {
        _private->idle.rptr = 0;
    }
    BKNI_ReleaseMutex(_private->idle.mutex);
}

/**
* onIdle is reentrant because of mmessagebox, therefore
* we can't rely on _timers current pointer. That's why
* there's an integer index.
**/
void MApplication::onIdle() {
    unsigned count = 10;
    
    /* Check if anyone is waiting for the idle lock, but only 10 per time. */
    BKNI_AcquireMutex(_private->idle.mutex);
    while (count-- && _private->idle.rptr !=  _private->idle.wptr && _private->idle.request[_private->idle.rptr]) {
        unsigned rptr = _private->idle.rptr;
        BDBG_MSG(("enabling idle.request[%d]", rptr));
        _private->idle.request[rptr] = false;
        while (rptr != _private->idle.rptr) {
            BKNI_ReleaseMutex(_private->idle.mutex);
            BKNI_Sleep(1);
            BKNI_AcquireMutex(_private->idle.mutex);
        }
        BDBG_MSG(("resuming from idle.request[%d]", rptr));
    }
    BKNI_ReleaseMutex(_private->idle.mutex);
    
    int curtime = tickCount();
    int i=0;
    for (MTimer *t = _timers.at(i); t; t = _timers.at(++i)) {
        if (t->test(curtime)) {
            if (_timers.at(i) == t)
                _timers.remove();
        }
    }
}

void MApplication::add(MIoDevice *io)
{
    _iodevices.add(io);
}

void MApplication::remove(MIoDevice *io)
{
    _iodevices.remove(io);
}
