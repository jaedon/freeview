/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_lua.h $
* $brcm_Revision: 6 $
* $brcm_Date: 3/27/12 3:30p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_lua.h $
* 
* 6   3/27/12 3:30p tokushig
* SW7405-5581: synchronize lua commands
* 
* 5   2/27/12 4:17p tokushig
* SW7405-5581: break out qam tuner from tuner files
* 
* 
*********************************************************************************/

#ifndef BRUTUS_LUA_H__
#define BRUTUS_LUA_H__

#include "view.h"
#include "bwidgets.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* make sure lua includes are inside the extern "C" */
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef __cplusplus
}
#endif


/* Lua events are created in response to custom Brutus Lua commands.  They are stored temporarily until
   the Brutus main loop has a chance to service them.  Data associated with the Lua event is allocated
   when the command is received, and freed after the associated action is completed.  The CLuaEvent
   defines a Lua event that contains no notification data.  Lua events that require data subclass
   from CLuaEvent.
 */
class CLuaEvent
{
public:
    CLuaEvent(eNotification id, 
              eNotification waitId      = eNotify_Invalid, 
              int           waitTimeout = 1000 /*B_WAIT_FOREVER*/) : 
        _id(id),
        _waitId(waitId),
        _waitTimeout(waitTimeout) {};

    eNotification  getId(void) { return _id; };
    eNotification  getWaitNotification(void) { return _waitId; };
    int            getWaitTimeout(void) { return _waitTimeout; };
    virtual void * getDataPtr(void) { return NULL; };
    
protected:
    eNotification   _id;
    eNotification   _waitId;
    int             _waitTimeout;
};

template<class T>  
class CLuaDataEvent : public CLuaEvent
{
public:
    CLuaDataEvent(eNotification id, T * data, 
                  eNotification waitId         = eNotify_Invalid, 
                  int           waitTimeout    = 1000 /* B_WAIT_FOREVER */) :
        CLuaEvent(id, waitId, waitTimeout),
        _data(data) {};

    ~CLuaDataEvent(void) { if (NULL != _data) delete _data; };

    void * getDataPtr(void) { return (void *)_data; };

protected:
    T * _data;
};


class CLua : public CView
{
public:
    CLua(void);
    ~CLua(void);

    eRet initialize(CModelConfig * pModelConfig);
    eRet uninitialize(void);
    void run(bwidget_engine_t widgetEngine);
    void stop(void);
    CConfiguration * getCfg() { return _pCfg; };

    lua_State * getLuaState(void) { return _pLua; };
    void setStartState(bool started) { _shellStarted = started; };
    bool getStartState(void) { return _shellStarted; };
    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };

    void addEvent(CLuaEvent * pEvent);
    CLuaEvent * getEvent(void);
    CLuaEvent * removeEvent(void);

    void trigger(CLuaEvent * pEvent);

    bool handleInput(char * pLine);
    void processNotification(CNotification & notification);

public:
    bool                 _threadRun;
    uint16_t             _pollInterval;

protected:
    lua_State          * _pLua;
    B_ThreadHandle       _threadShell;
    bool                 _shellStarted;
    MList <CLuaEvent>    _eventList;
    CLuaEvent            _busyLuaEvent;
    B_MutexHandle        _eventMutex;
    B_EventHandle        _busyEvent;
    bwidget_engine_t     _widgetEngine;

    CModelConfig       * _pModelConfig;
    CConfiguration     * _pCfg;
};

#endif /* BRUTUS_LUA_H__ */
