/***************************************************************************
 *     (c)2007-2013 Broadcom Corporation
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
 * $brcm_Workfile: nexus_ir_input.c $
 * $brcm_Revision: 41 $
 * $brcm_Date: 4/29/13 5:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/ir_input/src/nexus_ir_input.c $
 * 
 * 41   4/29/13 5:10p bandrews
 * SW7231-1184: merge to main
 * 
 * 40   2/12/13 3:16p gmohile
 * SW7408-362 : add more flexible standby behaviour
 * 
 * 39   1/28/13 12:01p gmohile
 * SW7429-468 : Fix IR data Filter
 * 
 * 38   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 37   6/15/12 3:07p gmohile
 * SW7425-3259 : Disable Sejin remote at init time to prevent false Ir
 *  interrupts
 * 
 * 36   5/7/12 5:19p erickson
 * SW7346-776: added more modes that use SW repeat
 * 
 * 35   2/27/12 3:46p erickson
 * SW7425-2283: add NEXUS_IrInputSettings.pulseWidthFilter
 * 
 * 34   8/12/11 11:09a gmohile
 * SW7125-1014 : Ir input should use the active scheduler
 * 
 * 33   4/25/11 12:12p erickson
 * SW7346-148: assign NEXUS_IrInputEvent.codeHigh unconditionally
 * 
 * 32   4/20/11 6:13p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 31   8/17/10 2:38p erickson
 * SW7400-2843: add NEXUS_IrInput_SetSettings and allow mode and
 *  channel_number to change. rework implementation to facilitate this
 *  change. convert linked lists to arrays.
 *
 * 30   7/16/10 2:11p erickson
 * SW7550-471: merge
 *
 * SW7550-471/2   7/17/10 12:38a nitinb
 * SW7550-471: Replace macro NEXUS_HAS_IR_INPUT_DATA_FILTER by
 *  BKIR_HAS_DATA_FILTER
 *
 * 29   7/14/10 4:09p erickson
 * SW7550-471: merge
 *
 * SW7550-471/1   7/14/10 9:47p nitinb
 * SW7550-471: Added API to enable/disable data filter. Also added support
 *  for 48-bit receiver data buffer for new chips
 *
 * 28   7/6/10 1:08p erickson
 * SWGIGGS2S-984: add NEXUS_IrInputEvent.interval, fix initial irInput-
 *  >lasttime logic
 *
 * 27   5/11/10 5:04p jtna
 * SW7125-307: Coverity - check for null pointer arguments before
 *  dereferencing them
 *
 * 26   3/23/10 5:22p erickson
 * SW7400-2723: NEXUS_IrInput_Open now requires globally unique index.
 *  This is needed for kernel mode proxy and for future proof code.
 *
 * 25   10/12/09 10:15a jtna
 * SW7405-2668: fix typo
 *
 * 24   7/31/09 9:22a erickson
 * PR57240: added NEXUS_IrInputMode_eCirRcmmRcu to the SW-repeat logic
 *  switch
 *
 * 23   7/30/09 4:13p jtna
 * PR56833: refactor custom IR settings
 *
 * 22   7/24/09 2:49p jtna
 * PR56474: add NEXUS_IrInput_GetPreambleStatus
 *
 * 21   5/7/09 11:50a mward
 * PR41482: Missing ! on the NEXUS_IrInputMode_eRemoteB case of
 *  NEXUS_IrInput_Open().  Allow this mode with others on same channel.
 *
 * 20   3/6/09 9:52a erickson
 * PR52852: add coverity hint
 *
 * 19   2/25/09 11:08a erickson
 * PR52491: fix NEXUS_IrInputModule_Uninit logic, added BDBG_OBJECT for
 *  NEXUS_IrChannel, general cleanup
 *
 * 18   2/19/09 2:32p erickson
 * PR52269: merge
 *
 * Trinity_Xmp_Support/4   2/19/09 2:14p prasadv
 * Merging with the latest v17
 *
 * 17   2/5/09 4:33p erickson
 * PR51864: fix NEXUS_IrInputModule_Uninit logic, fix possible NULL
 *  deference in NEXUS_IrInput_Open
 *
 * 16   1/26/09 12:05p erickson
 * PR51468: global variable naming convention
 *
 * 15   1/26/09 11:30a erickson
 * PR51468: global variable naming convention
 *
 * 14   12/11/08 11:56a jrubio
 * PR41482: make sure multiple IR channels work
 *
 * 13   8/18/08 10:28a katrep
 * PR45674: Compiler warnings in DEBUG=n builds
 *
 * 12   8/5/08 10:55a jrubio
 * PR45065: fix typo
 *
 * 11   7/31/08 11:50a erickson
 * PR45065: added NEXUS_IrInputMode_eCirRstep
 *
 * 10   7/21/08 2:54p erickson
 * PR37942: use KIR repeat detection. it's working. also, fix SW repeat
 *  detection logic.
 *
 * 9   4/8/08 9:45a jrubio
 * PR40022: force use of channel 0, channel 1,2 needs to be fixed PR41482
 *
 * 8   4/2/08 3:44p erickson
 * PR23894: switch to BKIR_RegisterCallback
 *
 * 7   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 6   3/26/08 12:02p jrubio
 * PR40022: updated ISR description, check in code for reading of all kir
 *  channels/interrupt devices
 *
 * 5   3/21/08 2:50p erickson
 * PR23894: better explanation of the INT workaround
 *
 * 4   3/17/08 11:28a jrubio
 * PR40022: Walk through the list and figure out what channel has the
 *  correct interrupt Device
 *
 * 3   2/28/08 10:42p vsilyaev
 * PR 40103: Added interfaceHandle and settings for the
 *  NEXUS_IsrCallbackCreate
 *
 * 2   2/6/08 3:39p jgarrett
 * PR 38808: Updating for 2.6.18 toolchain to remove private functions
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   12/5/07 4:23p erickson
 * PR37942: bypass BKIR_IsRepeated for now
 *
 * Nexus_Devel/4   12/5/07 11:16a jgarrett
 * PR 37921: Fixing interrupts and queueing bugs
 *
 * Nexus_Devel/3   11/21/07 1:58p erickson
 * PR37423: define BCHP_INT_ID_kbd1_irqen for 3563
 *
 * Nexus_Devel/2   11/21/07 11:12a erickson
 * PR37423: update
 *
 * Nexus_Devel/1   11/20/07 4:53p erickson
 * PR37423: adding ir_input and led
 *
 **************************************************************************/
#include "nexus_ir_input_module.h"
#include "bkir.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_ir_input);

NEXUS_ModuleHandle g_NEXUS_irInputModule;

#define BKIR_KirInterruptDevice_eMax (BKIR_KirInterruptDevice_eCir+1)

struct NEXUS_IrInput {
    NEXUS_OBJECT(NEXUS_IrInput);
    unsigned index;
    NEXUS_IrInputSettings settings;
    NEXUS_IsrCallbackHandle dataReady;
    uint32_t lastcode;
    NEXUS_Time lasttime;
    bool lasttime_set;
    /* data queued at isr time */
    NEXUS_IrInputEvent *queue;
    unsigned rptr, wptr;
    bool overflow;

    struct NEXUS_IrChannel *irChannel;
    BKIR_KirInterruptDevice interruptDevice; /* irChannel->irInput[interruptDevice] == this */
};

BDBG_OBJECT_ID(NEXUS_IrChannel);

struct NEXUS_IrChannel {
    BDBG_OBJECT(NEXUS_IrChannel)
    BKIR_ChannelHandle kirChannel;
    unsigned refcnt;
    NEXUS_IrInputHandle irInput[BKIR_KirInterruptDevice_eMax];
} NEXUS_IrChannel;

struct IrInputModule {
    NEXUS_IrInputModuleSettings settings;
    BKIR_Handle kir;
    struct NEXUS_IrChannel channel[BKIR_N_CHANNELS];
} g_NEXUS_irInput;

static BERR_Code NEXUS_IrInput_P_DataReady_isr(BKIR_ChannelHandle hChn, void *context);

/****************************************
* Module functions
***************/

void NEXUS_IrInputModule_GetDefaultSettings(NEXUS_IrInputModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_IrInputModule_Init(const NEXUS_IrInputModuleSettings *pSettings)
{
    BKIR_Settings kirSettings;
    BERR_Code rc;
    unsigned ch;
    NEXUS_ModuleSettings moduleSettings;

    BDBG_ASSERT(!g_NEXUS_irInputModule);
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLowActiveStandby;
    g_NEXUS_irInputModule = NEXUS_Module_Create("ir_input", &moduleSettings);
    if (!g_NEXUS_irInputModule) {
        return NULL;
    }
    BKNI_Memset(&g_NEXUS_irInput, 0, sizeof(g_NEXUS_irInput));
    if (pSettings) {
        g_NEXUS_irInput.settings = *pSettings;
    }
    else {
        NEXUS_IrInputModule_GetDefaultSettings(&g_NEXUS_irInput.settings);
    }

    BKIR_GetDefaultSettings(&kirSettings, g_pCoreHandles->chp);
    rc = BKIR_Open(&g_NEXUS_irInput.kir, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, &kirSettings);
    if (rc) {rc=BERR_TRACE(rc);goto error;}

    BDBG_MSG(("creating %d KIR channels", BKIR_N_CHANNELS));
    for (ch=0;ch<BKIR_N_CHANNELS;ch++) {
        BKIR_ChannelSettings channelSettings;
        struct NEXUS_IrChannel *irChannel = &g_NEXUS_irInput.channel[ch];
        BDBG_OBJECT_SET(irChannel, NEXUS_IrChannel);
		
#if defined(HUMAX_PLATFORM_BASE)
#if (BCHP_CHIP == 7346)
        /* 
        When BCM7346 supports IR single decode at URSR 12.4, BCM7346 cause too many IR 1 interrupts as HW bug. 
        So don't open IR 1 channel of BCM7346 before official fixed patch.
        If BCM7346 product supports IR dual protocol decode, Use IR channel 2 as sub IR protocol. 
        And DRV_REMOCON_Init() should be changed in order that sub IR input use IR Channel 2.
        */ 
        if(ch == 1)
        {
        	continue;
        }
#endif
#endif
        BKIR_GetChannelDefaultSettings(g_NEXUS_irInput.kir, ch, &channelSettings);
        channelSettings.intMode = true; /* we don't really care about the event from KIR because there's no ISR-time buffering.
            We must still register with INT ourselves.  But, if you don't set this flag, interrupts are not enabled properly. */
        rc = BKIR_OpenChannel(g_NEXUS_irInput.kir, &irChannel->kirChannel, ch, &channelSettings);
        if (rc) {rc=BERR_TRACE(rc); goto error;}

        BDBG_MSG(("%p KIR Channel %d, IrChannel %p", irChannel->kirChannel, ch, irChannel));

	/*Disable Sejin device as it causes false interrupts */
	BKIR_DisableIrDevice(irChannel->kirChannel, (BKIR_KirDevice)NEXUS_IrInputMode_eSejin38KhzKbd);
    }

    return g_NEXUS_irInputModule;
error:
    NEXUS_IrInputModule_Uninit();
    return NULL;
}

void NEXUS_IrInputModule_Uninit(void)
{
    unsigned ch;
    for (ch=0;ch<BKIR_N_CHANNELS;ch++) {
        struct NEXUS_IrChannel *irChannel = &g_NEXUS_irInput.channel[ch];
        unsigned i;

        BDBG_OBJECT_ASSERT(irChannel, NEXUS_IrChannel);
        for (i=0;i<BKIR_KirInterruptDevice_eMax;i++) {
            if (irChannel->irInput[i]) {
                NEXUS_IrInput_Close(irChannel->irInput[i]);
            }
        }
        BDBG_ASSERT(irChannel->refcnt == 0);
        if (irChannel->kirChannel) {
            BKIR_CloseChannel(irChannel->kirChannel);
        }
        BDBG_OBJECT_DESTROY(irChannel, NEXUS_IrChannel);
    }

    if (g_NEXUS_irInput.kir) {
        BKIR_Close(g_NEXUS_irInput.kir);
    }

    NEXUS_Module_Destroy(g_NEXUS_irInputModule);
    g_NEXUS_irInputModule = NULL;
}

/****************************************
* API functions
***************/

/* get KIR isr from INT and dispatch data to appropriate NEXUS_IrInputHandle */
static BERR_Code NEXUS_IrInput_P_DataReady_isr(BKIR_ChannelHandle hChn, void *context)
{
    BERR_Code rc;
    NEXUS_IrInputEvent event;
    struct NEXUS_IrChannel *irChannel = context;
    BKIR_KirInterruptDevice interruptDevice;
    unsigned char kirCode[8];
    NEXUS_Time time;

    BDBG_OBJECT_ASSERT(irChannel, NEXUS_IrChannel);
    BDBG_ASSERT(irChannel->kirChannel == hChn);

    BKNI_Memset(kirCode, 0, sizeof(kirCode));
    rc = BKIR_Read_Isr(hChn, &interruptDevice, kirCode);
    if (rc) {rc=BERR_TRACE(rc); return 0;}

    BKNI_Memset(&event, 0, sizeof(event));
    event.code = (uint32_t)kirCode[0] |
        ((uint32_t)kirCode[1] << 8) |
        ((uint32_t)kirCode[2] << 16) |
        ((uint32_t)kirCode[3] << 24);
    event.codeHigh = (uint32_t)kirCode[4] |
        ((uint32_t)kirCode[5] << 8);
    BDBG_MSG(("NEXUS_IrInput_P_DataReady_isr dev %d, code %#x %#x", interruptDevice, event.code, event.codeHigh));

    if (interruptDevice < BKIR_KirInterruptDevice_eMax && irChannel->irInput[interruptDevice]) {
        NEXUS_IrInputHandle irInput = irChannel->irInput[interruptDevice];
        unsigned time_diff;

        BDBG_OBJECT_ASSERT(irInput, NEXUS_IrInput);

        NEXUS_Time_Get(&time);
        if (irInput->lasttime_set) {
            time_diff = NEXUS_Time_Diff(&time, &irInput->lasttime);
        }
        else {
            time_diff = 0;
        }
        event.interval = time_diff;

        switch (irInput->settings.mode) {
        default:
            /* TODO: should be BKIR_IsRepeated_isr. also, BKIR_IsRepeated_isr should return non-BERR_TRACE error if HW repeat not supported. 
            then nexus switch statement could be removed. */
            rc = BKIR_IsRepeated(hChn, &event.repeat);
            if (rc) {rc=BERR_TRACE(rc);}
            break;
        case NEXUS_IrInputMode_eRemoteB:
        case NEXUS_IrInputMode_eTwirpKbd:
        case NEXUS_IrInputMode_eSejin38KhzKbd:
        case NEXUS_IrInputMode_eSejin56KhzKbd:
        case NEXUS_IrInputMode_eCirXmp:
        case NEXUS_IrInputMode_eCirRcmmRcu:
            /* no hardware to detect this, so measure time */
            event.repeat = (irInput->lastcode == event.code) && time_diff < irInput->settings.repeatFilterTime;
            break;
        }
        irInput->lasttime = time;
        irInput->lasttime_set = true;
        irInput->lastcode = event.code;

        /* add to queue and fire event */
        irInput->queue[irInput->wptr++] = event;
        if (irInput->wptr == irInput->settings.eventQueueSize)
            irInput->wptr = 0;
        BDBG_MSG(("add rptr=%d, wptr=%d, code=%x repeat=%c, interval=%d [queue size %d]", irInput->rptr, irInput->wptr,
            event.code, event.repeat?'y':'n', event.interval, irInput->settings.eventQueueSize));
        if (irInput->wptr == irInput->rptr)
        {
            BDBG_WRN(("Overflow"));
            irInput->overflow = true;
        }
        NEXUS_IsrCallback_Fire_isr(irInput->dataReady);
    }

    return 0;
}

void NEXUS_IrInput_GetDefaultSettings(NEXUS_IrInputSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->repeatFilterTime = 150;
    pSettings->eventQueueSize = 20;
    pSettings->mode = NEXUS_IrInputMode_eRemoteA;
}

/*
 * pSettings->channel_number corresponds to the KIR Channel to be Opened
*/
NEXUS_IrInputHandle NEXUS_IrInput_Open(unsigned index, const NEXUS_IrInputSettings *pSettings)
{
    NEXUS_IrInputHandle irInput = NULL;
    BERR_Code rc;
    NEXUS_IrInputSettings defaultSettings;
    unsigned ch;

    /* verify index is unique among all NEXUS_IrInputHandle's, regardless of channel.
    This is required for external identification of the interface handle in the kernel proxy.
    It may also be required for future internal state. It is also good SW design to require a unique index for each interface. */
    for (ch=0;ch<BKIR_N_CHANNELS;ch++) {
        unsigned i;
        struct NEXUS_IrChannel *irChannel = &g_NEXUS_irInput.channel[ch];
        BDBG_OBJECT_ASSERT(irChannel, NEXUS_IrChannel);
        for (i=0;i<BKIR_KirInterruptDevice_eMax;i++) {
            if (irChannel->irInput[i] && irChannel->irInput[i]->index == index) {
                BDBG_ERR(("NEXUS_IrInput %d already opened", index));
                return NULL;
            }
        }
    }

    if (!pSettings) {
        NEXUS_IrInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    irInput = BKNI_Malloc(sizeof(*irInput));
    if (!irInput) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_IrInput, irInput);
    irInput->index = index;

    /* allow dataReady callback to be invoke from dataready isr */
    irInput->dataReady = NEXUS_IsrCallback_Create(irInput, NULL);
    if (!irInput->dataReady) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}

    rc = NEXUS_IrInput_SetSettings(irInput, pSettings);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto error;
    }

    return irInput;

error:
    if (irInput) {
        NEXUS_IrInput_Close(irInput);
    }
    return NULL;
}

/* NEXUS_IrInput_P_Disconnect can be called from SetSettings or Close */
static void NEXUS_IrInput_P_Disconnect(NEXUS_IrInputHandle irInput)
{
    irInput->irChannel->irInput[irInput->interruptDevice] = NULL;
    irInput->interruptDevice = BKIR_KirInterruptDevice_eNone;
    BKIR_DisableIrDevice(irInput->irChannel->kirChannel, irInput->settings.mode);

    BDBG_ASSERT(irInput->irChannel->refcnt);
    if (--irInput->irChannel->refcnt == 0) {
        BKIR_UnregisterCallback(irInput->irChannel->kirChannel);
    }
    irInput->irChannel = NULL;
}

static void NEXUS_IrInput_P_Finalizer(NEXUS_IrInputHandle irInput)
{
    NEXUS_OBJECT_ASSERT(NEXUS_IrInput, irInput);
    if (irInput->irChannel) {
        NEXUS_IrInput_P_Disconnect(irInput);
    }
    if (irInput->queue) {
        BKNI_Free(irInput->queue);
    }
    if (irInput->dataReady) {
        NEXUS_IsrCallback_Destroy(irInput->dataReady);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_IrInput, irInput);
    BKNI_Free(irInput);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_IrInput, NEXUS_IrInput_Close);

void NEXUS_IrInput_GetSettings( NEXUS_IrInputHandle irInput, NEXUS_IrInputSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(irInput, NEXUS_IrInput);
    *pSettings = irInput->settings;
}

NEXUS_Error NEXUS_IrInput_SetSettings( NEXUS_IrInputHandle irInput, const NEXUS_IrInputSettings *pSettings )
{
    struct NEXUS_IrChannel *new_channel;
    BERR_Code rc;
    BKIR_KirInterruptDevice interruptDevice;

    BDBG_OBJECT_ASSERT(irInput, NEXUS_IrInput);

    /* param validation */
    if (pSettings->channel_number >= BKIR_N_CHANNELS) {
        BDBG_ERR(("Invalid channel_number %d, num_channels=%d", pSettings->channel_number, BKIR_N_CHANNELS));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if ( !pSettings->eventQueueSize ) {
        BDBG_ERR(("Invalid queue size %d", pSettings->eventQueueSize));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if (pSettings->eventQueueSize != irInput->settings.eventQueueSize) {
        if (irInput->queue) {
            BKNI_Free(irInput->queue);
        }
        irInput->queue = BKNI_Malloc(pSettings->eventQueueSize*sizeof(NEXUS_IrInputEvent));
        if (!irInput->queue) {
            return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        }
    }
    BDBG_ASSERT(irInput->queue);

    switch (pSettings->mode) {
    case NEXUS_IrInputMode_eTwirpKbd:
        interruptDevice = BKIR_KirInterruptDevice_eTwirpKbd; break;
    case NEXUS_IrInputMode_eSejin38KhzKbd:
    case NEXUS_IrInputMode_eSejin56KhzKbd:
        interruptDevice = BKIR_KirInterruptDevice_eSejinKbd; break;
    case NEXUS_IrInputMode_eRemoteA:
        interruptDevice = BKIR_KirInterruptDevice_eRemoteA; break;
    case NEXUS_IrInputMode_eRemoteB:
        interruptDevice = BKIR_KirInterruptDevice_eRemoteB; break;
    default:
        interruptDevice = BKIR_KirInterruptDevice_eCir; break;
    }

    /* only one IrInput can use an interrupt device on the same channel. verify this before disconnecting. */
    new_channel = &g_NEXUS_irInput.channel[pSettings->channel_number];
    BDBG_OBJECT_ASSERT(new_channel, NEXUS_IrChannel);
    if (new_channel->irInput[interruptDevice] && new_channel->irInput[interruptDevice] != irInput) {
        BDBG_ERR(("cannot use mode %d on channel_number %d because of interrupt device %d conflict",
            pSettings->mode, pSettings->channel_number, interruptDevice));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* now that we've validated the new state, we can proceed with the disconnect/connect without risk of failure */

    /* disconnect from channel if changing channel_number or interruptDevice */
    if (irInput->irChannel && (pSettings->channel_number != irInput->settings.channel_number || interruptDevice != irInput->interruptDevice)) {
        NEXUS_IrInput_P_Disconnect(irInput);
    }

    /* connect to a channel */
    if (!irInput->irChannel) {
        irInput->irChannel = new_channel;
        irInput->interruptDevice = interruptDevice;
        irInput->irChannel->irInput[irInput->interruptDevice] = irInput;
        if (irInput->irChannel->refcnt++ == 0) {
            BKIR_RegisterCallback(irInput->irChannel->kirChannel, NEXUS_IrInput_P_DataReady_isr, irInput->irChannel);
        }
        BDBG_MSG(("irInput(%p:%d) kirChannel=%p irChannel=%p", irInput, irInput->index, irInput->irChannel->kirChannel, irInput->irChannel));
    }
    BDBG_OBJECT_ASSERT(irInput->irChannel, NEXUS_IrChannel);

    /* compile-time assert that the Nexus and Magnum enums are in sync. If they are not, please sync them up or create a conversion function. */
    BDBG_CASSERT(BKIR_KirDevice_eNumKirDevice == (BKIR_KirDevice)NEXUS_IrInputMode_eMax);

    if (pSettings->useCustomSettings) {
        /* User is only allowed to use 1 Custom CIR on a platform, no matter how many HW IR Receivers there are */
        CIR_Param cirParam;

        /* compile-time assert that the Nexus and Magnum structs are at least equal in size */
        BDBG_CASSERT(sizeof(pSettings->customSettings) == sizeof(cirParam));

        /* set the custom device type that the custom settings will apply to */
        BKIR_SetCustomDeviceType(irInput->irChannel->kirChannel, (BKIR_KirDevice)pSettings->mode);

        BKNI_Memcpy(&cirParam, &pSettings->customSettings, sizeof(NEXUS_IrInputCustomSettings));
        BKIR_SetCustomCir(irInput->irChannel->kirChannel, &cirParam);

        /* enable device with custom settings */
        rc = BKIR_EnableIrDevice(irInput->irChannel->kirChannel, BKIR_KirDevice_eCirCustom);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        rc = BKIR_EnableIrDevice(irInput->irChannel->kirChannel, (BKIR_KirDevice)pSettings->mode);
        if (rc) return BERR_TRACE(rc);
    }
    
    if (pSettings->pulseWidthFilter != irInput->settings.pulseWidthFilter) {
        if (pSettings->pulseWidthFilter) {
            /* converse of (128*filter_width+2)/27) */
            unsigned filter_width = (pSettings->pulseWidthFilter*27-2)/128;
            /* print programmed value to reduce confusion between nexus units and magnum/HW units */
            BDBG_LOG(("input %d: pulseWidthFilter of %d microseconds => filter_width %d", irInput->index, pSettings->pulseWidthFilter, filter_width));
            rc = BKIR_EnableFilter1(irInput->irChannel->kirChannel, filter_width);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            (void)BKIR_DisableFilter1(irInput->irChannel->kirChannel);
        }
    }

    NEXUS_IsrCallback_Set(irInput->dataReady, &pSettings->dataReady);

    irInput->settings = *pSettings;

    return 0;
}

NEXUS_Error NEXUS_IrInput_GetEvents(NEXUS_IrInputHandle irInput, NEXUS_IrInputEvent *pEvents, size_t numEvents, size_t *pNumEventsRead, bool *pOverflow)
{
    size_t numEventsRead=0;

    BDBG_OBJECT_ASSERT(irInput, NEXUS_IrInput);
    if (numEvents && !pEvents) BERR_TRACE(NEXUS_INVALID_PARAMETER);
    *pOverflow = irInput->overflow;
    irInput->overflow = false;
    /* no critical section needed for simple producer/consumer algo */
    while (numEvents && irInput->rptr != irInput->wptr) {
        pEvents[numEventsRead] = irInput->queue[irInput->rptr];
        if (++irInput->rptr == irInput->settings.eventQueueSize) {
            irInput->rptr = 0;
        }
        numEventsRead++;
        numEvents--;
    }
    *pNumEventsRead = numEventsRead;
    BDBG_MSG(("GetEvents returning %d events", numEventsRead));
    return 0;
}

void NEXUS_IrInput_FlushEvents(NEXUS_IrInputHandle irInput)
{
    BDBG_OBJECT_ASSERT(irInput, NEXUS_IrInput);
    irInput->rptr = irInput->wptr;
}

void NEXUS_IrInput_GetDefaultCustomSettings(NEXUS_IrInputCustomSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_IrInput_GetCustomSettingsForMode(NEXUS_IrInputMode mode, NEXUS_IrInputCustomSettings *pSettings)
{
    BERR_Code rc;
    CIR_Param cirParam;

    if (mode==NEXUS_IrInputMode_eCustom) {
        /* required check so that we don't dereference a null handle in BKIR */
        goto error;
    }

    rc = BKIR_GetDefaultCirParam(NULL, (BKIR_KirDevice)mode, &cirParam);
    if (rc==BERR_SUCCESS) {
        BKNI_Memcpy(pSettings, &cirParam, sizeof(NEXUS_IrInputCustomSettings));
        return NEXUS_SUCCESS;
    }

error:
    return BERR_TRACE(NEXUS_INVALID_PARAMETER);
}

NEXUS_Error NEXUS_IrInput_GetPreambleStatus(NEXUS_IrInputHandle irInput, NEXUS_IrInputPreambleStatus *pStatus)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(irInput, NEXUS_IrInput);

    rc = BKIR_IsPreambleA(irInput->irChannel->kirChannel, &pStatus->preambleA);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    rc = BKIR_IsPreambleB(irInput->irChannel->kirChannel, &pStatus->preambleB);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    return NEXUS_SUCCESS;

error:
    return NEXUS_UNKNOWN;
}

void NEXUS_IrInput_GetDefaultDataFilter( NEXUS_IrInputDataFilter *pPattern )
{
    BKNI_Memset(pPattern, 0, sizeof(*pPattern));
    pPattern->patternWord0 = ~0;
    pPattern->patternWord0 = ~0;
    pPattern->mask0 = ~0;
    pPattern->mask1 = ~0;
}

NEXUS_Error NEXUS_IrInput_EnableDataFilter(NEXUS_IrInputHandle handle, const NEXUS_IrInputDataFilter *pPattern)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_IrInput);

    rc = BKIR_EnableDataFilter(handle->irChannel->kirChannel,
            pPattern->filterWord[0].enabled ? pPattern->filterWord[0].patternWord : ((uint64_t)pPattern->patternWord1 << 32) | pPattern->patternWord0,
            pPattern->filterWord[1].enabled ? pPattern->filterWord[1].patternWord : NEXUS_IR_INPUT_FILTER_DISABLED,
            pPattern->filterWord[0].enabled ? pPattern->filterWord[0].mask : ((uint64_t)pPattern->mask1 << 32) | pPattern->mask0,
            pPattern->filterWord[1].enabled ? pPattern->filterWord[1].mask : NEXUS_IR_INPUT_FILTER_DISABLED);

    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

void NEXUS_IrInput_DisableDataFilter(NEXUS_IrInputHandle handle)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_IrInput);

    rc = BKIR_DisableDataFilter(handle->irChannel->kirChannel);
    if (rc) BERR_TRACE(rc);
}
