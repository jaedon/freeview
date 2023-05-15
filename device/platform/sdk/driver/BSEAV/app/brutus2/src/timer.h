/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: timer.h $
* $brcm_Revision: 3 $
* $brcm_Date: 2/28/12 3:01p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/timer.h $
* 
* 3   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 2   2/23/12 3:30p tokushig
* SW7405-5581: add comments/headers
* 
* 
*********************************************************************************/

#ifndef __TIMER_H
#define __TIMER_H

#include "bwidgets.h"
#include "mvc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
   CTimer is a model class used to provide timers that are synchronized with
   with brutus2 bwidgets main loop.  It relies on the b_os_lib's scheduler
   to set/clear/trigger timers.  This class can only notify CObserver classes
   which restricts it to controller and view classes - this ensures compliance 
   with the underlying MVC architecture.
*/
class CTimer : public CMvcModel
{
public:
    CTimer(bwidget_engine_t widgetEngine, CObserver * pObserver, int timeout);
    CTimer(CObserver * pObserver);
    ~CTimer(void);

    void start(int timeout = -1);
    void stop(void);

    bool isStarted(void) { return (NULL != _timerId); };
    void clearTimerId(void) { _timerId = NULL; };
    int getTimeout(void) { return _timeout; };
    void setTimeout(int timeout) { _timeout = timeout; };
    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };
    void setWidgetEngine(bwidget_engine_t widgetEngine);

protected:
    CObserver          * _pObserver;
    B_SchedulerTimerId   _timerId;
    int                  _timeout;
    bwidget_engine_t     _widgetEngine;
};

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H */
