/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: remote.cpp $
* $brcm_Revision: 10 $
* $brcm_Date: 9/29/12 9:24p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/remote.cpp $
* 
* 10   9/29/12 9:24p tokushig
* SW7231-749: convert wrn debug statements to msg
* 
* 9   7/9/12 4:15p jrubio
* SW7346-644: Silver smaller remote has different value for Play
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

#include "brutus.h"
#include "brutus_os.h"
#include "remote.h"

BDBG_MODULE(brutus_remote);


static void bwinRemoteCallback(bwidget_engine_t engine, bwin_io_handle handle)
{
    eRet                 ret      = eRet_Ok;
    int                  berror   = 0;
    NEXUS_IrInputEvent   irEvent;
    CRemote            * pRemote  = (CRemote *)handle;
    CRemoteEvent         event;

    BDBG_ASSERT(NULL != pRemote);

    ret = pRemote->getEvent(&event);
    if (eRet_Ok == ret)
    {
        BDBG_MSG(("Remote event code: %#x", event.getCode()));
        berror = bwidget_enter_key(engine, eKey2bwidgets[event.getCode()], true);

        if (-1 == berror)
        {
            BDBG_MSG(("bwidgets did not consume key event so pass on to registered observers"));
            /* bwidgets did not consume key so pass on to registered observers */
            ret = pRemote->notifyObservers(eNotify_KeyDown, &event);
        }
    }
}

static void nexusIrCallback(void * context, int param)
{
    eRet            ret      = eRet_Ok;
    CIrRemote     * irRemote = (CIrRemote *)context;

    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != irRemote);
    BDBG_ASSERT(eBoardResource_irRemote == irRemote->getType());

    /* interrupt the bwin event loop. this will interrupt the sleep and cause the ui to be more responsive. */
    bwidget_trigger_io(irRemote->getWidgetEngine(), (bwin_io_handle)context);
}


CIrRemote::CIrRemote(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CRemote(name, number, eBoardResource_irRemote, pCfg),
    _irInput(NULL)
{
}

eRet CIrRemote::open(bwidget_engine_t widgetEngine)
{
    NEXUS_IrInputSettings settings;
    eRet                  ret       = eRet_Ok;

    BDBG_ASSERT(NULL != widgetEngine);

    _widgetEngine = widgetEngine;

    NEXUS_IrInput_GetDefaultSettings(&settings);
    settings.dataReady.callback = nexusIrCallback;
    settings.dataReady.context  = this;
    _irInput = NEXUS_IrInput_Open(getNumber(), &settings);
    CHECK_PTR_GOTO("nexus ir input open failed!", _irInput, ret, eRet_ExternalError, error);

    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinRemoteCallback);

    return ret;

error:
    return ret;
}


void CIrRemote::close()
{
    bwidget_remove_io_handle(_widgetEngine, (bwin_io_handle)this);

    if (NULL != _irInput)
    {
        NEXUS_IrInput_Close(_irInput);
    }
}

CIrRemote::~CIrRemote()
{
    close();
}

void CIrRemote::dump()
{
    BDBG_WRN(("<%d>%s:%d",
              _type, 
              _name.s(), 
              _number));
}

eRet CIrRemote::getEvent(CRemoteEvent * pEvent)
{
    eRet                 ret       = eRet_NotAvailable;
    NEXUS_Error          nerror    = NEXUS_SUCCESS;
    NEXUS_IrInputEvent   irEvent;
    unsigned             num;
    bool                 overflow;

    BDBG_ASSERT(NULL != pEvent);

    nerror = NEXUS_IrInput_GetEvents(getIrRemote(), &irEvent, 1, &num, &overflow);

    /* do not buffer any remote events */
    NEXUS_IrInput_FlushEvents(getIrRemote()); 

    if (!nerror && num && !irEvent.repeat) 
    {
        BDBG_MSG(("ir event: %#x\n", irEvent.code));
        pEvent->setRepeat(irEvent.repeat);

        /* translate ir key to device independent key */
        switch (irEvent.code) 
        {
        case 0x8035: /* down */
            BDBG_MSG(("down key"));
            pEvent->setCode(eKey_DownArrow);
            break;
        case 0x9034: /* up */
            BDBG_MSG(("up key"));
            pEvent->setCode(eKey_UpArrow);
            break;
        case 0x6037: /* right */
            BDBG_MSG(("right key"));
            pEvent->setCode(eKey_RightArrow);
            break;
        case 0x7036: /* left */
            BDBG_MSG(("left key"));
            pEvent->setCode(eKey_LeftArrow);
            break;
        case 0xe011: /* select */
            BDBG_MSG(("select key"));
            pEvent->setCode(eKey_Select);
            break;
        case 0x500b: /* channel up */
            BDBG_MSG(("channel up key"));
            pEvent->setCode(eKey_ChannelUp);
            break;
        case 0x400c: /* channel down */
            BDBG_MSG(("channel down key"));
            pEvent->setCode(eKey_ChannelDown);
            break;
        case 0x6019:
            BDBG_MSG(("menu key"));
            pEvent->setCode(eKey_Menu);
            break;
        case 0xA033:
            BDBG_MSG(("info key"));
            pEvent->setCode(eKey_Info);
            break;
        case 0x4039:
            BDBG_MSG(("Stop key"));
            pEvent->setCode(eKey_Stop);
            break;
        case 0x401b:
            BDBG_MSG(("Play key"));
            pEvent->setCode(eKey_Play);
            break;
        case 0x5038: /* Smaller remote */
            BDBG_MSG(("Play key"));
            pEvent->setCode(eKey_Play);
            break;
        case 0x1f:
            BDBG_MSG(("Pause key"));
            pEvent->setCode(eKey_Pause);
            break;
        case 0xc031:
            BDBG_MSG(("Record key"));
            pEvent->setCode(eKey_Record);
            break;
        case 0x101e:
            BDBG_MSG(("Rewind key"));
            pEvent->setCode(eKey_Rewind);
            break;
        case 0x201d:
            BDBG_MSG(("Fast Forward key"));
            pEvent->setCode(eKey_FastForward);
            break;
        case 0x0:
            BDBG_MSG(("0 key"));
            pEvent->setCode(eKey_0);
            break;
        case 0xF001:
            BDBG_MSG(("1 key"));
            pEvent->setCode(eKey_1);
            break;
        case 0xE002:
            BDBG_MSG(("2 key"));
            pEvent->setCode(eKey_2);
            break;
        case 0xD003:
            BDBG_MSG(("3 key"));
            pEvent->setCode(eKey_3);
            break;
        case 0xC004:
            BDBG_MSG(("4 key"));
            pEvent->setCode(eKey_4);
            break;
        case 0xB005:
            BDBG_MSG(("5 key"));
            pEvent->setCode(eKey_5);
            break;
        case 0xA006:
            BDBG_MSG(("6 key"));
            pEvent->setCode(eKey_6);
            break;
        case 0x9007:
            BDBG_MSG(("7 key"));
            pEvent->setCode(eKey_7);
            break;
        case 0x8008:
            BDBG_MSG(("8 key"));
            pEvent->setCode(eKey_8);
            break;
        case 0x7009:
            BDBG_MSG(("9 key"));
            pEvent->setCode(eKey_9);
            break;
        default:
            BDBG_MSG(("unhandled remote code 0x%0x", irEvent.code));
            break;
        }

        ret = eRet_Ok;
    }

    return ret;
}
