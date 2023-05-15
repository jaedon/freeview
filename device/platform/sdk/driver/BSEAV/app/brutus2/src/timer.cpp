/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: timer.cpp $
* $brcm_Revision: 4 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/timer.cpp $
* 
* 4   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 3   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 2   2/23/12 3:31p tokushig
* SW7405-5581: add comments/headers
* 
* 
*********************************************************************************/

#include "brutus.h"
#include "brutus_os.h"
#include "timer.h"

BDBG_MODULE(brutus_timer);

/* 
    Callback from scheduler when timer expires.  Triggering the bwidget
    main loop will ensure that the timer observer is not called until
    it can safely run.
*/ 
void timerCallback(void * context)
{
    CTimer * pTimer = (CTimer *)context;
    BDBG_ASSERT(NULL != context);

    pTimer->clearTimerId();
    bwidget_trigger_io(pTimer->getWidgetEngine(), (bwin_io_handle)pTimer);
}

/* 
    Callback from the bwidgets main loop - io trigger is complete and we can
    safely notify the observer of the timer expiration.
*/ 
void bwinTimerCallback(bwidget_engine_t engine, bwin_io_handle handle)
{
    CTimer * pTimer = (CTimer *)handle;
    BDBG_ASSERT(NULL != pTimer);

    pTimer->notifyObservers(eNotify_Timeout, pTimer);
}

/* 
Constructor
   widgetEngine - the bwidget engine that will be synchronized with when the timer
                  expires.
   pObserver - the observer who will be notified when the timer expires.
   timeout - the timeout period in millisecs.  This value can be overridden when the
             timer is started
*/
CTimer::CTimer(bwidget_engine_t widgetEngine, CObserver * pObserver, int timeout) :
    _widgetEngine(widgetEngine),
    _pObserver(pObserver),
    _timeout(timeout),
    _timerId(NULL)
{
    BDBG_ASSERT(_widgetEngine);
    BDBG_ASSERT(_pObserver);
    
    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinTimerCallback);
    registerObserver(_pObserver, eNotify_Timeout);
}

CTimer::CTimer(CObserver * pObserver) :
    _widgetEngine(NULL),
    _pObserver(pObserver),
    _timeout(0),
    _timerId(NULL)
{
    BDBG_ASSERT(_pObserver);
    
    registerObserver(_pObserver, eNotify_Timeout);
}

CTimer::~CTimer()
{
    stop();
    clearAllObservers();

    if (NULL != _widgetEngine)
    {
        bwidget_remove_io_handle(_widgetEngine, (bwin_io_handle)this);
    }
}

void CTimer::setWidgetEngine(bwidget_engine_t widgetEngine) 
{ 
    if (NULL != _widgetEngine)
    {
        bwidget_remove_io_handle(_widgetEngine, (bwin_io_handle)this);
    }

    _widgetEngine = widgetEngine; 

    if (NULL != _widgetEngine)
    {
        bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinTimerCallback);
    }
}

/*
Starts the timer. 
    timeout - optional parameter that if given will specify the timeout period in millisecs. 
*/
void CTimer::start(int timeout)
{
    if (NULL != _timerId)
    {
        BDBG_MSG(("Restarting timer: %d ms", _timeout));
        stop();
    }

    if (0 < timeout)
    {
        _timeout = timeout;
    }

    _timerId = B_Scheduler_StartTimer(gScheduler, gLock, _timeout, timerCallback, this);
}

void CTimer::stop()
{
    if (NULL != _timerId)
    {
        B_Scheduler_CancelTimer(gScheduler, _timerId);
        _timerId = NULL;
    }
}
