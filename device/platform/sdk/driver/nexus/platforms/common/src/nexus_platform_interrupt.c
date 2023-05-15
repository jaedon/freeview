/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_interrupt.c $
* $brcm_Revision: 14 $
* $brcm_Date: 9/10/12 10:02a $
*
* API Description:
*   API name: Platform linuxuser
*    linuxuser OS routines
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/nexus_platform_interrupt.c $
* 
* 14   9/10/12 10:02a katrep
* SW7445-1:fixed the issue with 7550 build
* 
* 13   9/5/12 5:33p katrep
* SW7445-1:updated external interrupts
* 
* 12   11/18/11 1:02p erickson
* SW7420-2144: ConnectInterrupt will fail with NEXUS_NOT_AVAILABLE if
*  nexus does not manage the L1
* 
* 11   11/15/11 4:04p erickson
* SW7425-1747: remove old code
* 
* 10   10/28/11 11:25a erickson
* SW7420-1148: fail init if interrupt cannot connect or enable.
*  multiprocess client may be keeping previous driver open and interrupts
*  connected.
* 
* 9   8/9/11 10:51a mward
* SW7125-1070: OVERRUN_STATIC g_enable_interrupt[] in
*  NEXUS_Platform_P_InitInterrupts().
* 
* 8   5/17/11 2:15p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* SWDTV-6928/1   5/4/11 6:25p mstefan
* SWDTV-6928: merge to main
* 
* 7   9/28/10 11:11a agin
* SWNOOS-425:  Fixed GHS and SDE compiler error.
* 
* 6   9/14/10 6:45p hongtaoz
* SW7425-9: adding 7425 support;
* 
* 5   8/10/10 5:16p erickson
* SW7420-880: NEXUS_Platform_P_UninitInterrupts should only disconnect
*  interrupts that were connected. use g_enable_interrupt[] to avoid
*  duplicating logic.
*
* 4   8/2/10 1:46p erickson
* SW7420-880: call NEXUS_Platform_P_DisconnectInterrupt for each L1 in
*  NEXUS_Platform_P_UninitInterrupts
*
* 3   5/12/09 4:45p erickson
* PR52109: set callback
*
* 2   5/12/09 4:34p erickson
* PR52109: added external interrupt interface
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/4   11/29/07 3:04p jgarrett
* PR 37632: Removing prototype for bint routine
*
* Nexus_Devel/3   11/29/07 2:44p jgarrett
* PR 37632: Adding support for L1 interrupts above 64
*
* Nexus_Devel/2   9/27/07 10:11p jgarrett
* PR 35002: Adding interrupt reset
*
* Nexus_Devel/1   9/21/07 6:26p jgarrett
* PR 35002: Successful 97400 build
*
***************************************************************************/

#include "nexus_types.h"
#include "nexus_base.h"
#include "nexus_platform_extint.h"
#include "nexus_platform_features.h"
#include "nexus_platform_priv.h"
#include "priv/nexus_core.h"
#include "bkni.h"
#include "bchp_hif_cpu_intr1.h"


#if (BCHP_CHIP==35125) || (BCHP_CHIP==35230) || (BCHP_CHIP==35233)
/* TODO - This register is reserved on the 35130. This define has been added
   as an expedient in order to get the code to build. This needs to be
   revisted and to handle the 'real' interrupt correctly
*/
#define BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_0_CPU_INTR_SHIFT 21
#endif/*BCHP_CHIP==35230*/

#if 0
/* #define B_IRQ_ID_W1_N(id,n) (21 + n) */
#define B_IRQ_ID_W1_N(id,n) \
    (BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_##id##_##n##_CPU_INTR_SHIFT + 32)
#endif

BDBG_MODULE(nexus_platform_interrupt);

struct NEXUS_ExternalInterrupt
{
    NEXUS_ExternalInterruptSettings settings;
    NEXUS_IsrCallbackHandle callback;
    bool enabled;
    unsigned irq; /* 0-based bit position (0..63) in L1 register set */
};

#ifndef NEXUS_EXTIRQ_MAX
#define NEXUS_EXTIRQ_MAX 5
#endif

struct NEXUS_ExternalInterrupt g_NEXUS_ExternalInterrupt[NEXUS_EXTIRQ_MAX];

static bool g_enable_interrupt[NEXUS_NUM_L1_REGISTERS*32];

NEXUS_Error NEXUS_Platform_P_InitInterrupts(void)
{
    int i;
    BINT_Handle intHandle = g_pCoreHandles->bint;
    NEXUS_Error rc = 0;

    NEXUS_Platform_P_ResetInterrupts();
    BKNI_Memset(g_enable_interrupt, 0, sizeof(g_enable_interrupt));

#if NEXUS_NUM_L1_REGISTERS > 2
    {
        uint32_t l1Mask[BINT_MAX_INTC_SIZE];
        BKNI_Memset(l1Mask, 0, sizeof(l1Mask));
        BINT_GetL1BitMask(intHandle, l1Mask);
        for( i=0; i<32; i++ )
        {
            int j;
            uint32_t bit = 1ul<<i;
            for ( j = 0; j < NEXUS_NUM_L1_REGISTERS; j++ )
            {
                if ( l1Mask[j] & bit )
                {
                    int intNum = i + (32*j);
                    g_enable_interrupt[intNum] = true;
                }
            }
        }
    }
#else
    {
        uint32_t l1masklo, l1maskhi;
        BINT_GetL1BitMask(intHandle, &l1masklo, &l1maskhi);
        for( i=0; i<64; i++ )
        {
            if( i >=32 )
            {
                if( l1maskhi & 1ul<<(i-32) )
                {
                    g_enable_interrupt[i] = true;
                }
            }
            else
            {
                if( l1masklo & 1ul<<i )
                {
                    g_enable_interrupt[i] = true;
                }
            }
        }
    }
#endif

    for (i=0;i<NEXUS_NUM_L1_REGISTERS*32 && !rc;i++) {
        if (g_enable_interrupt[i]) {
            BDBG_MSG(("Enabling L1 interrupt %d", i));
            rc = NEXUS_Platform_P_ConnectInterrupt(i, (NEXUS_Core_InterruptFunction)BINT_Isr, intHandle, i);
            if (!rc) {
                rc = NEXUS_Platform_P_EnableInterrupt(i);
                if (rc) break;
            }
            else if (rc == NEXUS_NOT_AVAILABLE) {
                rc = 0; /* if L1 not managed by linux, we get this error */
            }
        }
    }
    BKNI_Memset(&g_NEXUS_ExternalInterrupt, 0, sizeof(g_NEXUS_ExternalInterrupt));

    return rc;
}

void NEXUS_Platform_P_UninitInterrupts(void)
{
    unsigned i;
    for (i=0;i<NEXUS_NUM_L1_REGISTERS*32;i++) {
        if (g_enable_interrupt[i]) {
            NEXUS_Platform_P_DisconnectInterrupt(i);
            g_enable_interrupt[i] = false;
        }
    }
}


void NEXUS_Platform_GetDefaultExternalInterruptSettings( NEXUS_ExternalInterruptSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

static void NEXUS_Platform_P_ExternalInterrupt_isr(void *context, int param)
{
    BSTD_UNUSED(param);
    NEXUS_IsrCallback_Fire_isr(((struct NEXUS_ExternalInterrupt*)context)->callback);
}

NEXUS_Error NEXUS_Platform_EnableExternalInterrupt( unsigned interruptId, const NEXUS_ExternalInterruptSettings *pSettings )
{
    NEXUS_Error rc;
    struct NEXUS_ExternalInterrupt *handle;

    if (interruptId >= NEXUS_EXTIRQ_MAX){
        BDBG_ERR(("invalid external interrupt %d", interruptId));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    handle = &g_NEXUS_ExternalInterrupt[interruptId];

    if (handle->enabled) {
        BDBG_WRN(("External interrupt %d already enabled", interruptId));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    switch (interruptId) {
#ifdef BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_0_CPU_INTR_SHIFT        
        case 0: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_0_CPU_INTR_SHIFT + 32; break;
        case 1: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_1_CPU_INTR_SHIFT + 32; break;
        case 2: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_2_CPU_INTR_SHIFT + 32; break;
        case 3: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_3_CPU_INTR_SHIFT + 32; break;
    #if NEXUS_EXTIRQ_MAX > 4
        case 4: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT + 32; break;
    #endif
    #if NEXUS_EXTIRQ_MAX > 5
        case 5: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_5_CPU_INTR_SHIFT + 32; break;
    #endif
    #if NEXUS_EXTIRQ_MAX > 6
        case 6: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_6_CPU_INTR_SHIFT + 32; break;
    #endif
    #if NEXUS_EXTIRQ_MAX > 7
        case 7: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_7_CPU_INTR_SHIFT + 32; break;
    #endif
    #if NEXUS_EXTIRQ_MAX > 8
        case 8: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_8_CPU_INTR_SHIFT + 32; break;
    #endif
#else 
#ifdef BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_00_CPU_INTR_SHIFT
        case 0: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_00_CPU_INTR_SHIFT + 96; break;
        case 1: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_01_CPU_INTR_SHIFT + 96; break;
        case 2: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_02_CPU_INTR_SHIFT + 96; break;
	case 3: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_03_CPU_INTR_SHIFT + 96; break;
	case 4: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_04_CPU_INTR_SHIFT + 96; break;
	case 5: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_EXT_IRQ_05_CPU_INTR_SHIFT + 96; break;
#else
        case 0: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_00_CPU_INTR_SHIFT + 32; break;
        case 1: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_01_CPU_INTR_SHIFT + 32; break;
        case 2: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_02_CPU_INTR_SHIFT + 32; break;
        case 3: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_03_CPU_INTR_SHIFT + 32; break;
    #if NEXUS_EXTIRQ_MAX > 4
        case 4: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_04_CPU_INTR_SHIFT + 32; break;
    #endif
    #if NEXUS_EXTIRQ_MAX > 5
        case 5: handle->irq = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_05_CPU_INTR_SHIFT + 32; break;
    #endif
#endif
#endif

    default: return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    handle->settings = *pSettings;
    handle->callback = NEXUS_IsrCallback_Create(NULL, NULL);
    NEXUS_IsrCallback_Set(handle->callback, &pSettings->callback);
    rc = NEXUS_Platform_P_ConnectInterrupt(handle->irq, NEXUS_Platform_P_ExternalInterrupt_isr, handle, 0);
    if (!rc) {
        rc = NEXUS_Platform_P_EnableInterrupt(handle->irq);
    }
    if (!rc) {
        handle->enabled = true;
    }

    return rc;
}

void NEXUS_Platform_DisableExternalInterrupt( unsigned interruptId )
{
    struct NEXUS_ExternalInterrupt *handle;

    if (interruptId >= NEXUS_EXTIRQ_MAX){
        BDBG_ERR(("invalid external interrupt %d", interruptId));
        return;
    }

    handle = &g_NEXUS_ExternalInterrupt[interruptId];

    if (!handle->enabled) {
        BDBG_WRN(("External interrupt %d not enabled", interruptId));
        return;
    }

    NEXUS_Platform_P_DisableInterrupt(handle->irq);
    NEXUS_Platform_P_DisconnectInterrupt(handle->irq);
    NEXUS_IsrCallback_Destroy(handle->callback);
    handle->enabled = false;
}


