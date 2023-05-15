/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_uhf_input.c $
 * $brcm_Revision: 23 $
 * $brcm_Date: 6/19/12 12:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/uhf_input/7400/src/nexus_uhf_input.c $
 * 
 * 23   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 22   5/14/12 12:37p erickson
 * SW7425-3056: trap bad param in NEXUS_UhfInput_GetEvents
 * 
 * 21   11/22/11 6:17p jtna
 * SW7425-1708: fix s3 resume
 * 
 * 20   11/18/11 4:43p jtna
 * SW7425-1708: do more error checking
 * 
 * 19   11/18/11 11:56a jtna
 * SW7425-1708: add S3 power management for uhf_input
 * 
 * 18   11/3/11 5:41p mward
 * SW7435-7:  Add 7435.
 * 
 * 17   10/19/11 1:11p gmohile
 * SW7420-2103 : Uhf module should ont be locks in active standby
 * 
 * 16   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 * 
 * SW7125-1014/1   7/20/11 4:37p gmohile
 * SW7125-1014 : Rework power management
 * 
 * 15   2/25/11 2:52p gmohile
 * SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
 * 
 * 14   2/18/11 10:41a gmohile
 * SW7420-210 : Fix warnings
 * 
 * 13   2/16/11 6:11p gmohile
 * SW7408-210 : Add uhf standby
 * 
 * 12   12/8/10 2:47p jrubio
 * SW7344-9: add uhf support for 7344/7346
 * 
 * 11   10/7/10 4:15p nickh
 * SW7422-73: Add 7422 support
 * 
 * 10   9/14/10 6:48p hongtaoz
 * SW7425-9: add 7425 support;
 * 
 * 9   3/18/10 3:59p erickson
 * SW7400-2724: set NEXUS_UhfInputEvent.header for
 *  NEXUS_UhfInputMode_eChannel9, set event.repeat correctly
 * 
 * 8   1/26/09 1:54p erickson
 * PR51468: global variable naming convention
 *
 * 7   12/17/08 1:48p katrep
 * PR49875: Add support for 7466
 *
 * 6   8/18/08 10:28a katrep
 * PR45674: Compiler warnings in DEBUG=n builds
 *
 * 5   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 4   3/27/08 2:34p jrubio
 * PR40919: Open UHF PI at Nexus UHF open. use UHF enums
 *
 * 3   3/25/08 11:23a mphillip
 * PR39982: Fix a warning
 *
 * 2   3/24/08 5:26p mphillip
 * PR39982: Implement UHF remote support
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   11/30/07 11:49a jgarrett
 * PR 37423: Removing warnings
 *
 * Nexus_Devel/2   11/21/07 11:12a erickson
 * PR37423: update
 *
 * Nexus_Devel/1   11/20/07 5:56p erickson
 * PR37423: added module for ir_blaster, keypad and uhf_input
 *
 **************************************************************************/
#include "nexus_uhf_input_module.h"
#include "buhf.h"
#include "priv/nexus_core.h"
#include "blst_list.h"

#if (BCHP_CHIP == 7425) || (BCHP_CHIP == 7422) || BCHP_CHIP==7344 || BCHP_CHIP==7346 || (BCHP_CHIP == 7435)
#include "bchp_int_id_uhfr_intr2.h"
#include "bchp_uhfr_intr2.h"
#else
#include "bchp_int_id_uhfr_1.h"
#endif
#if (BCHP_CHIP == 7400)
#include "bchp_uhfr_2.h"
#include "bchp_int_id_uhfr_2.h"
#endif
#if (BCHP_CHIP == 7403)
#include "bchp_clk.h"
#endif

BDBG_MODULE(nexus_uhf_input);

NEXUS_ModuleHandle g_NEXUS_uhfInputModule;

#if NEXUS_NUM_UHF_INPUTS

struct NEXUS_UhfInput {
    NEXUS_OBJECT(NEXUS_UhfInput);
    BUHF_Handle uhf;
    NEXUS_UhfInputSettings settings;
    NEXUS_IsrCallbackHandle dataReady;
    NEXUS_Time lasttime;
    uint32_t   lastcode;


    /* data queued at isr time */
    NEXUS_UhfInputEvent *queue;
    unsigned rptr, wptr;
    bool overflow;
};

struct UhfInputModule {
    NEXUS_UhfInputModuleSettings settings;
    BUHF_Handle uhf;
    BUHF_Settings uhfSettings;
    BINT_CallbackHandle uhfCallback1;
    BINT_CallbackHandle uhfCallback2;
    BLST_D_HEAD(channellist, NEXUS_UhfInput) list;
    bool s3standby;
} g_NEXUS_uhfInput;

static void NEXUS_UhfInput_P_DataReady_isr(void *context, int param);

/****************************************
* Module functions
***************/

void NEXUS_UhfInputModule_GetDefaultSettings(NEXUS_UhfInputModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_UhfInputModule_Init(const NEXUS_UhfInputModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;

    BDBG_ASSERT(!g_NEXUS_uhfInputModule);
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLowActiveStandby;
    g_NEXUS_uhfInputModule = NEXUS_Module_Create("uhf_input", &moduleSettings);
    if (pSettings) {
        g_NEXUS_uhfInput.settings = *pSettings;
    }
    else {
        NEXUS_UhfInputModule_GetDefaultSettings(&g_NEXUS_uhfInput.settings);
    }
    BLST_D_INIT(&g_NEXUS_uhfInput.list);

    return g_NEXUS_uhfInputModule;
}

void NEXUS_UhfInputModule_Uninit()
{
    if (g_NEXUS_uhfInput.uhf) {
        BUHF_Close(g_NEXUS_uhfInput.uhf);
    }
    NEXUS_Module_Destroy(g_NEXUS_uhfInputModule);
    g_NEXUS_uhfInputModule = NULL;
}

NEXUS_Error NEXUS_UhfInputModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BUHF_StandbySettings standbySettings;
    bool wakeup = pSettings->wakeupSettings.uhf;
    BERR_Code rc;        
    
    if (!g_NEXUS_uhfInput.uhf) {
        return 0;
    }
    
    if (enabled) {
        if (pSettings->mode!=NEXUS_StandbyMode_eDeepSleep) { /* not S3 */
            BUHF_GetDefaultStandbySettings(g_NEXUS_uhfInput.uhf, &standbySettings);
            standbySettings.bEnableWakeup = wakeup;
            rc = BUHF_Standby(g_NEXUS_uhfInput.uhf, &standbySettings);
            if (rc) { return BERR_TRACE(rc); }
        } 
        else {
            if (BLST_D_EMPTY(&g_NEXUS_uhfInput.list)) {
                BINT_DisableCallback(g_NEXUS_uhfInput.uhfCallback1);
                BINT_DisableCallback(g_NEXUS_uhfInput.uhfCallback2);
            }
            if (g_NEXUS_uhfInput.uhf) {
                BUHF_Close(g_NEXUS_uhfInput.uhf);
                g_NEXUS_uhfInput.s3standby = true;
            }
        }
    }
    else {
        if (!g_NEXUS_uhfInput.s3standby) { /* not S3 */
            rc = BUHF_Resume(g_NEXUS_uhfInput.uhf);
            if (rc) { return BERR_TRACE(rc); }
        }
        else {
            rc = BUHF_Open(&g_NEXUS_uhfInput.uhf, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, 0, &g_NEXUS_uhfInput.uhfSettings);
            if (rc) { rc=BERR_TRACE(rc); return rc; }

            if (g_NEXUS_uhfInput.uhfCallback1) {
                rc = BINT_EnableCallback(g_NEXUS_uhfInput.uhfCallback1);
                if (rc) {rc=BERR_TRACE(rc); return rc;}
            }
            if (g_NEXUS_uhfInput.uhfCallback2) {
                rc = BINT_EnableCallback(g_NEXUS_uhfInput.uhfCallback2);
                if (rc) {rc=BERR_TRACE(rc); return rc;}
            }
        }
    }
      
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
#endif
  
  return NEXUS_SUCCESS;
}

/****************************************
* API functions
***************/

/* get KIR isr from INT and dispatch data to appropriate NEXUS_IrInputHandle */
static void NEXUS_UhfInput_P_DataReady_isr(void *context, int param)
{
    BERR_Code rc;
    NEXUS_UhfInputEvent event;
    NEXUS_UhfInputHandle uhfInput = (NEXUS_UhfInputHandle) context;
    BUHF_Data data;

    NEXUS_Time time;

    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BDBG_MSG(("Data received at UHFR: %s ", __FUNCTION__));

    BKNI_Memset(&event, 0, sizeof(event));

    rc = BUHF_Read(g_NEXUS_uhfInput.uhf, &data);
    if (rc != BERR_SUCCESS) return;
    if (uhfInput->settings.channel == NEXUS_UhfInputMode_eChannel9) {
        BDBG_MSG(("Data received at UHFR: 0x10%08x Cust2 type Packet \n\n", data.value));
        event.code = data.value;
        event.header = 0x10;
    } else {
        BDBG_MSG(("Data received at UHFR: 0x%08x, Preamble type %d (orig data=0x%08x)\n\n", event.code, data.prType, data.value));
        if (data.prType == BUHF_PrType_e1)
            event.code = data.value >> 6;
        else if (data.prType == BUHF_PrType_e2)
            event.code = data.value >> 27;
    }

    NEXUS_Time_Get(&time);
    event.repeat = (uhfInput->lastcode == event.code) && (unsigned)NEXUS_Time_Diff(&time, &uhfInput->lasttime) < uhfInput->settings.repeatFilterTime;

    uhfInput->lasttime = time;
    uhfInput->lastcode = event.code;

    if (event.repeat) event.code |= 0x80000000;

    /* add to queue and fire event */
    uhfInput->queue[uhfInput->wptr++] = event;
    if (uhfInput->wptr == uhfInput->settings.eventQueueSize)
        uhfInput->wptr = 0;
    BDBG_MSG(("add %d, %d, %x%s [queue size %d]", uhfInput->rptr, uhfInput->wptr, event.code, event.repeat?"(r)":"", uhfInput->settings.eventQueueSize));
    if (uhfInput->wptr == uhfInput->rptr) {
        BDBG_WRN(("Overflow"));
        uhfInput->overflow = true;
    }
    NEXUS_IsrCallback_Fire_isr(uhfInput->dataReady);
}

void NEXUS_UhfInput_GetDefaultSettings(NEXUS_UhfInputSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->repeatFilterTime = 150;
    pSettings->eventQueueSize = 10;
    pSettings->channel = NEXUS_UhfInputMode_eChannel1;
}

NEXUS_UhfInputHandle NEXUS_UhfInput_Open(unsigned index, const NEXUS_UhfInputSettings *pSettings)
{
    NEXUS_UhfInputHandle uhfInput;
    BUHF_Settings uhfSettings;
    BERR_Code rc = BERR_SUCCESS;

    BINT_Id uhfIrq1 = 0, uhfIrq2 = 0;

    NEXUS_UhfInputSettings defaultSettings;

    if (!pSettings) {
        NEXUS_UhfInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    /* TODO: Add support for second receiver. add it in standby as well */
    if (index > 0) {
        BDBG_WRN(("Only the first receiver is currently supported"));
        return NULL;
    }

    uhfInput = BKNI_Malloc(sizeof(*uhfInput));
    if (!uhfInput) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_UhfInput, uhfInput);
    uhfInput->queue = BKNI_Malloc(pSettings->eventQueueSize*sizeof(NEXUS_UhfInputEvent));
    if (!uhfInput->queue) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    uhfInput->settings = *pSettings;

     /* You Must Open UHF Remote PI because Settings cannot be changed once its open */
    BUHF_GetDefaultSettings(&uhfSettings);
    uhfSettings.channel = pSettings->channel;
    uhfSettings.mode = BUHF_Mode_eAdvanced; /* Set in UHF Advanced mode. TODO: IR Mode */
    rc = BUHF_Open(&g_NEXUS_uhfInput.uhf, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, 0, &uhfSettings);
    g_NEXUS_uhfInput.uhfSettings = uhfSettings;

#ifdef BCHP_INT_ID_ALT_PACKET_END
    if (pSettings->channel == NEXUS_UhfInputMode_eChannel9) {
        uhfIrq1 = BCHP_INT_ID_ALT_PACKET_END;
        uhfIrq2 = BCHP_INT_ID_ALT_SFR;
    } else {
        uhfIrq1 = BCHP_INT_ID_CORR_DECODE_PR1_END;
        uhfIrq2 = BCHP_INT_ID_CORR_DECODE_PR2_END;
    }
#else
#if NEXUS_NUM_UHF_INPUTS==1
    if (pSettings->channel == NEXUS_UhfInputMode_eChannel9) {
        uhfIrq1 = BCHP_INT_ID_IMSK_ALT_PACKET_END;
        uhfIrq2 = BCHP_INT_ID_IMSK_ALT_SFR;
    } else {
        uhfIrq1 = BCHP_INT_ID_IMSK_CORR_DECODE_PR1_END;
        uhfIrq2 = BCHP_INT_ID_IMSK_CORR_DECODE_PR2_END;
    }
#else
    if (pSettings->channel == NEXUS_UhfInputMode_eChannel9) {
        uhfIrq1 = BCHP_INT_ID_UHFR_1_IMSK_ALT_PACKET_END;
        uhfIrq2 = BCHP_INT_ID_UHFR_1_IMSK_ALT_SFR;
    } else {
        uhfIrq1 = BCHP_INT_ID_UHFR_1_IMSK_CORR_DECODE_PR1_END;
        uhfIrq2 = BCHP_INT_ID_UHFR_1_IMSK_CORR_DECODE_PR2_END;
    }
#endif
#endif

    /* UHF does not reply an ISR callback, which can lose events if we simply WaitForEvent on the UHF event.
     * Instead, we must tap into INT to avoid missing input. */
    if (uhfIrq1 && uhfIrq2) {
        if (!g_NEXUS_uhfInput.uhfCallback1) {
            rc = BINT_CreateCallback(&g_NEXUS_uhfInput.uhfCallback1, g_pCoreHandles->bint, uhfIrq1,
                NEXUS_UhfInput_P_DataReady_isr, (void *)uhfInput, 0);
            if (rc) {rc=BERR_TRACE(rc);goto error;}
            rc = BINT_EnableCallback(g_NEXUS_uhfInput.uhfCallback1);
            if (rc) {rc=BERR_TRACE(rc);goto error;}
        }
        if (!g_NEXUS_uhfInput.uhfCallback2) {
            rc = BINT_CreateCallback(&g_NEXUS_uhfInput.uhfCallback2, g_pCoreHandles->bint, uhfIrq2,
                NEXUS_UhfInput_P_DataReady_isr, (void *)uhfInput, 0);
            if (rc) {rc=BERR_TRACE(rc);goto error;}
            rc = BINT_EnableCallback(g_NEXUS_uhfInput.uhfCallback2);
            if (rc) {rc=BERR_TRACE(rc);goto error;}
        }
    }

    /* allow dataReady callback to be invoke from dataready isr */
    uhfInput->dataReady = NEXUS_IsrCallback_Create(uhfInput, NULL);
    if(!uhfInput->dataReady) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    NEXUS_IsrCallback_Set(uhfInput->dataReady, &pSettings->dataReady);

    return uhfInput;
error:
    NEXUS_UhfInput_Close(uhfInput);
    return NULL;
}

static void NEXUS_UhfInput_P_Finalizer(NEXUS_UhfInputHandle uhfInput)
{
    NEXUS_OBJECT_ASSERT(NEXUS_UhfInput, uhfInput);
    if (BLST_D_EMPTY(&g_NEXUS_uhfInput.list)) {
        BINT_DisableCallback(g_NEXUS_uhfInput.uhfCallback1);
        BINT_DisableCallback(g_NEXUS_uhfInput.uhfCallback2);
    }

    if (g_NEXUS_uhfInput.uhfCallback1) {
        BINT_DestroyCallback(g_NEXUS_uhfInput.uhfCallback1);
        g_NEXUS_uhfInput.uhfCallback1 = NULL;
    }
    if (g_NEXUS_uhfInput.uhfCallback2) {
        BINT_DestroyCallback(g_NEXUS_uhfInput.uhfCallback2);
        g_NEXUS_uhfInput.uhfCallback2 = NULL;
    }

    if (uhfInput->queue) {
        BKNI_Free(uhfInput->queue);
        uhfInput->queue = NULL;
    }
    if (uhfInput->dataReady) {
        NEXUS_IsrCallback_Destroy(uhfInput->dataReady);
        uhfInput->dataReady = NULL;
    }

    NEXUS_OBJECT_DESTROY(NEXUS_UhfInput, uhfInput);
    BKNI_Free(uhfInput);

    /* Close UHF Handle */
    if (g_NEXUS_uhfInput.uhf) {
        BUHF_Close(g_NEXUS_uhfInput.uhf);
    }

    g_NEXUS_uhfInput.uhf = NULL;

}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_UhfInput, NEXUS_UhfInput_Close);

NEXUS_Error NEXUS_UhfInput_GetEvents(NEXUS_UhfInputHandle uhfInput, NEXUS_UhfInputEvent *pEvents, size_t numEvents, size_t *pNumEventsRead, bool *pOverflow)
{
    size_t numEventsRead=0;
    BDBG_OBJECT_ASSERT(uhfInput, NEXUS_UhfInput);
    if (!pEvents || !pNumEventsRead || !pOverflow) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    *pOverflow = uhfInput->overflow;
    uhfInput->overflow = false;
    /* no critical section needed for simple producer/consumer algo */
    while (numEvents && uhfInput->rptr != uhfInput->wptr) {
        pEvents[numEventsRead] = uhfInput->queue[uhfInput->rptr];
        if (++uhfInput->rptr == uhfInput->settings.eventQueueSize) {
            uhfInput->rptr = 0;
        }
        numEventsRead++;
        numEvents--;
    }
    *pNumEventsRead = numEventsRead;
    BDBG_MSG(("GetEvents returning %d events", numEventsRead));
    return 0;
}

void NEXUS_UhfInput_FlushEvents(NEXUS_UhfInputHandle uhfInput)
{
    BDBG_OBJECT_ASSERT(uhfInput, NEXUS_UhfInput);
    uhfInput->rptr = uhfInput->wptr;
}

#else


void NEXUS_UhfInput_GetDefaultSettings(
    NEXUS_UhfInputSettings *pSettings    /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Uhf Input not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;

}

NEXUS_UhfInputHandle NEXUS_UhfInput_Open(  /* attr{destructor=NEXUS_UhfInput_Close}  */
    unsigned index,
    const NEXUS_UhfInputSettings *pSettings  /* May be passed as NULL for defaults */
    )
{
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    BDBG_WRN(("Uhf Input not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_UhfInput_Close(
    NEXUS_UhfInputHandle handle
    )
{
    BSTD_UNUSED(handle);
    BDBG_WRN(("Uhf Input not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}


NEXUS_Error NEXUS_UhfInput_GetEvents(
    NEXUS_UhfInputHandle handle,
    NEXUS_UhfInputEvent *pEvents,   /* Pointer to an array of events */
    size_t numEvents,               /* Size of the event array */
    size_t *pNumEventsRead,         /* [out] Number of events actually read */
    bool *pOverflow                 /* [out] Has an overflow occurred? */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pEvents);
    BSTD_UNUSED(numEvents);
    BSTD_UNUSED(pNumEventsRead);
    BSTD_UNUSED(pOverflow);
    BDBG_WRN(("Uhf Input not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}


void NEXUS_UhfInput_FlushEvents(
    NEXUS_UhfInputHandle handle
    )
{
    BSTD_UNUSED(handle);
    BDBG_WRN(("Uhf Input not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}
#endif /* NEXUS_NUM_UHF_INPUTS */
