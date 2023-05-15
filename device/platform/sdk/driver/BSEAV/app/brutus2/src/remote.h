/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: remote.h $
* $brcm_Revision: 9 $
* $brcm_Date: 4/20/12 3:34p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/remote.h $
* 
* 9   4/20/12 3:34p tokushig
* SW7231-749: fix remote number button mapping issue and adjust size of
* scan menu
* 
* 8   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 7   4/19/12 11:12p jrubio
* SW7346-644: add pvr keys for GI remote
* 
* 6   4/17/12 6:45p tokushig
* SW7231-749: add exit key handling - closes main menu
* 
* 5   4/17/12 4:36p tokushig
* SW7231-749: menu button on remote will show/hide menu.  on screen
* channel number will timeout in 3 secs if the menu is not shown.  if
* the menu is shown, the channel number will always be displayed.
* 
* 4   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef REMOTE_H__
#define REMOTE_H__

#include "nexus_ir_input.h"
#include "bwidgets.h"
#include "brutus_cfg.h"
#include "resource.h"

#ifdef __cplusplus
extern "C" {
#endif

/* dtt todo - add ascii codes 0x20 up to 0x7f for text input */
/* changes to eKey enum must also be reflected in eKey2bwidgets */
typedef enum eKey {
    eKey_Invalid,
    eKey_Select,
    eKey_UpArrow,
    eKey_DownArrow,
    eKey_LeftArrow,
    eKey_RightArrow,
    eKey_Backspace,
    eKey_Delete,
    eKey_ChannelUp,
    eKey_ChannelDown,
    eKey_Play,
    eKey_Stop,
    eKey_Pause,
    eKey_Rewind,
    eKey_FastForward,
    eKey_Record,
    eKey_Menu,
    eKey_Info,
    eKey_Exit,
    /* ascii codes */
    eKey_0,
    eKey_1,
    eKey_2,
    eKey_3,
    eKey_4,
    eKey_5,
    eKey_6,
    eKey_7,
    eKey_8,
    eKey_9,
    eKey_Max
} eKey;

/* mapping to bwidget equivalents */
/* dtt todo - add ascii codes 0x20 up to 0x7f for text input */
static const bwidget_key eKey2bwidgets[eKey_Max] = {
    bwidget_key_invalid,
    bwidget_key_select,
    bwidget_key_up,
    bwidget_key_down,
    bwidget_key_left,
    bwidget_key_right,
    bwidget_key_backspace,
    bwidget_key_delete,
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
    /* ascii codes */
    bwidget_key_0,
    bwidget_key_1,
    bwidget_key_2,
    bwidget_key_3,
    bwidget_key_4,
    bwidget_key_5,
    bwidget_key_6,
    bwidget_key_7,
    bwidget_key_8,
    bwidget_key_9
};


class CRemoteEvent
{
public:
    CRemoteEvent(void) : _code(0), _repeat(false) {};

    uint32_t getCode(void) { return _code; };
    void setCode(uint32_t code) { _code = code; };
    bool isRepeat(void) { return _repeat; };
    void setRepeat(bool repeat) { _repeat = repeat; };

protected:
    uint32_t _code;
    bool     _repeat;
};


class CRemote : public CResource
{
public:
    CRemote(const char * name, const uint16_t number, const eBoardResource type, CConfiguration * pCfg) :
        CResource(name, number, type, pCfg) {};
    
    ~CRemote(void) {};

    virtual eRet open(bwidget_engine_t widgetEngine) = 0;
    virtual void close(void) = 0;
    virtual void dump(void) = 0;
    virtual eRet getEvent(CRemoteEvent * pEvent) = 0;

    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };

protected:
    bwidget_engine_t               _widgetEngine;
};


class CIrRemote : public CRemote
{
public:
    CIrRemote(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CIrRemote(void);

    virtual eRet open(bwidget_engine_t widgetEngine);
    virtual void close(void);
    virtual void dump();
    virtual eRet getEvent(CRemoteEvent * pEvent);

    NEXUS_IrInputHandle getIrRemote(void) { return _irInput; };

protected:
    NEXUS_IrInputHandle         _irInput;
};


#ifdef __cplusplus
}
#endif

#endif /* REMOTE_H__ */
