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
 * $brcm_Workfile: nexus_gpio.c $
 * $brcm_Revision: 27 $
 * $brcm_Date: 9/27/12 1:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/gpio/src/nexus_gpio.c $
 * 
 * 27   9/27/12 1:13p erickson
 * SW7435-362: lock module during init and uninit
 * 
 * 26   1/27/12 4:54p gmohile
 * SW7425-1708 : Add gpio standby
 * 
 * 25   7/14/11 2:11p jtna
 * SW7405-5414: added NEXUS_GpioSettings.maskEdgeInterrupts
 * 
 * 24   5/25/11 3:50p shyi
 * SWDTV-7058: Added support for TVM GPIO interrupt
 * 
 * 23   2/22/11 4:52p randyjew
 * SW7344-25: Creating seperate callback handle for AONGpios.
 * 
 * 22   2/16/11 4:57p randyjew
 * SW7344-25:Fixed compile warning
 * 
 * 21   2/16/11 3:17p randyjew
 * SW7344-25:Add Gpio register abstraction to support AON Gpio's and other
 *  Gpio register ranges.
 * 
 * 20   12/6/10 12:08p erickson
 * SW7408-146: add NEXUS_Gpio_GetPinMux
 * 
 * 19   11/17/10 1:27p VISHK
 * SW7422-71: Add support for GPIO interrupts to 3112 frontend tuners.
 * 
 * 18   10/11/10 3:52p erickson
 * SW7422-75: deprecate NEXUS_GpioType. convert from sparse array to
 *  linked list. nexus_gpio_table.c is now responsible for bounds check.
 *  this allows us to avoid updating generic code for chip-specific GPIO
 *  blocks.
 * 
 * 17   10/11/10 1:06p nickh
 * SW7422-73: Add 7422 support
 * 
 * 16   9/14/10 6:47p hongtaoz
 * SW7425-9: compile for 7425;
 * 
 * 15   9/8/10 10:52a erickson
 * SW7550-560: fix interrupt safety of NEXUS_Gpio_SetSettings
 *
 * 14   4/21/10 4:17p erickson
 * SW7405-4228: fix logic in NEXUS_Gpio_OpenAux
 *
 * 13   4/19/10 4:29p erickson
 * SW7405-4228: use NEXUS_Gpio_Open macro and
 *  NEXUS_Gpio_OpenAux(typeAndPin) implementation to fix linux kernel mode
 *  proxy callbacks for GPIO
 *
 * 12   4/5/10 4:26p shyi
 * SW35230-124: NEXUS GPIO for 35230 (merging to main branch)
 *
 * 11   1/6/10 12:15p jhaberf
 * SW35230-1: Added 35230 DTV chip support
 *
 * 10   7/10/09 4:08p jhaberf
 * PR53796: Adding 35130 #define in order to get gpio module to build for
 *  the 35130 software development environment
 *
 * 9   3/12/09 2:13p jgarrett
 * PR 50409: Disabling interrupt before setting mask/edge/... registers to
 *  avoid spurious interrupts
 *
 * 8   1/26/09 12:05p erickson
 * PR51468: global variable naming convention
 *
 * 7   1/26/09 11:29a erickson
 * PR51468: global variable naming convention
 *
 * 6   8/18/08 10:28a katrep
 * PR45674: Compiler warnings in DEBUG=n builds
 *
 * 5   2/28/08 10:42p vsilyaev
 * PR 40103: Added interfaceHandle and settings for the
 *  NEXUS_IsrCallbackCreate
 *
 * 4   2/26/08 11:40a jgarrett
 * PR 39016: Fixing open sequencing if interrupt is enabled
 *
 * 3   2/25/08 8:10p jgarrett
 * PR 39610: Masking level interrupts after interrupt is received
 *
 * 2   1/25/08 2:30p erickson
 * PR39016: fix interrupt enable
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/6   11/30/07 11:08a jgarrett
 * PR 37801: Adjusting register offsets
 *
 * Nexus_Devel/5   11/26/07 10:57a erickson
 * PR37423: support other name of gpio interrupt
 *
 * Nexus_Devel/4   11/26/07 9:45a erickson
 * PR37423: gpio update
 *
 * Nexus_Devel/3   11/21/07 11:12a erickson
 * PR37423: update
 *
 * Nexus_Devel/2   11/20/07 2:22p erickson
 * PR37423: simplify module init
 *
 * Nexus_Devel/1   11/20/07 1:28p erickson
 * PR37423: added uart, gpio, spi modules
 *
 **************************************************************************/
#include "nexus_gpio_module.h"
#include "priv/nexus_core.h"
#include "priv/nexus_gpio_priv.h"
#include "priv/nexus_gpio_standby_priv.h"
#include "bint.h"
#include "bchp_int_id_irq0.h"
#include "bchp_gio.h"

#ifdef BCHP_INT_ID_aon_gio_irqen
#error "BCHP_INT_ID_aon_gio_irqen should not be defined previously"
#endif

#if NEXUS_NUM_AON_GPIO_PINS > 0
#include "bchp_int_id_irq0_aon.h"
#include "bchp_gio_aon.h"
#define BCHP_INT_ID_aon_gio_irqen BCHP_INT_ID_IRQ0_AON_gio_irqen
#endif
#if NEXUS_NUM_TGPIO_PINS > 0
#include "bchp_int_id_tvm.h"
#include "bchp_tvm.h"
#define BCHP_INT_ID_aon_gio_irqen BCHP_INT_ID_TVM_GPIO_INT
#endif

/**
The Nexus Gpio module does not use the Magnum GIO porting interface.
Interrupts are requested directly from INT. GPIO registers are accessed directly.
**/

BDBG_MODULE(nexus_gpio);

NEXUS_ModuleHandle g_NEXUS_gpioModule;
struct {
    NEXUS_GpioModuleSettings settings;
    BINT_CallbackHandle intCallback;
    #ifdef BCHP_INT_ID_aon_gio_irqen
    BINT_CallbackHandle intAonCallback;
    #endif
    BLST_S_HEAD(NEXUS_GpioList, NEXUS_Gpio) list;
    bool s3standby;
} g_NEXUS_gpio;
static void NEXUS_Gpio_P_isr(void *context, int param);

BDBG_OBJECT_ID(NEXUS_Gpio);



/****************************************
* Module functions
***************/

void NEXUS_GpioModule_GetDefaultSettings(NEXUS_GpioModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_GpioModule_Init(const NEXUS_GpioModuleSettings *pSettings)
{
    BERR_Code rc;

    BDBG_ASSERT(!g_NEXUS_gpioModule);
    g_NEXUS_gpioModule = NEXUS_Module_Create("gpio", NULL);
    if (!g_NEXUS_gpioModule) {
        return NULL;
    }
    NEXUS_LockModule();
    BKNI_Memset(&g_NEXUS_gpio, 0, sizeof(g_NEXUS_gpio));
    if (pSettings) {
        g_NEXUS_gpio.settings = *pSettings;
    }
    else {
        NEXUS_GpioModule_GetDefaultSettings(&g_NEXUS_gpio.settings);
    }
    BLST_S_INIT(&g_NEXUS_gpio.list);

    /* Prior to installing L2 callback, we must ensure that all GPIO interrupts are masked. */
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_MASK_LO, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_STAT_LO, 0xFFFFFFFF);
    #ifdef BCHP_GIO_MASK_HI
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_MASK_HI, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_STAT_HI, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_GIO_MASK_EXT
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_STAT_EXT, 0xFFFFFFFF);
    #endif
   #ifdef BCHP_GIO_AON_MASK_LO
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_AON_MASK_LO, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_AON_STAT_LO, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_GIO_AON_MASK_EXT
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_AON_MASK_EXT, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_AON_STAT_EXT, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_GIO_MASK_EXT_HI
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT_HI, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_STAT_EXT_HI, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_GIO_MASK_EXT2
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT2, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_GIO_STAT_EXT2, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_TVM_GPIO_MASK_0
    BREG_Write32(g_pCoreHandles->reg, BCHP_TVM_GPIO_MASK_0, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_TVM_GPIO_STAT_0, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_TVM_GPIO_MASK_1
    BREG_Write32(g_pCoreHandles->reg, BCHP_TVM_GPIO_MASK_1, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_TVM_GPIO_STAT_1, 0xFFFFFFFF);
    #endif
    #ifdef BCHP_TVM_GPIO_MASK_2
    BREG_Write32(g_pCoreHandles->reg, BCHP_TVM_GPIO_MASK_2, 0);
    BREG_Write32(g_pCoreHandles->reg, BCHP_TVM_GPIO_STAT_2, 0xFFFFFFFF);
    #endif

#ifndef BCHP_INT_ID_gio_irqen
#ifdef BCHP_INT_ID_IRQ0_gio_irqen
#define BCHP_INT_ID_gio_irqen BCHP_INT_ID_IRQ0_gio_irqen
#else
#define BCHP_INT_ID_gio_irqen BCHP_INT_ID_gio
#endif
#endif
    rc = BINT_CreateCallback(&g_NEXUS_gpio.intCallback, g_pCoreHandles->bint, BCHP_INT_ID_gio_irqen, NEXUS_Gpio_P_isr, NULL, 0);
    if (rc) {rc=BERR_TRACE(rc); return NULL;}
    rc = BINT_EnableCallback(g_NEXUS_gpio.intCallback);
    if (rc) {rc=BERR_TRACE(rc); return NULL;}
    #ifdef BCHP_INT_ID_aon_gio_irqen
    rc = BINT_CreateCallback(&g_NEXUS_gpio.intAonCallback, g_pCoreHandles->bint, BCHP_INT_ID_aon_gio_irqen, NEXUS_Gpio_P_isr, NULL, 0);
    if (rc) {rc=BERR_TRACE(rc); return NULL;}
    rc = BINT_EnableCallback(g_NEXUS_gpio.intAonCallback);
    if (rc) {rc=BERR_TRACE(rc); return NULL;}
    #endif

    NEXUS_UnlockModule();
    return g_NEXUS_gpioModule;
}

void NEXUS_GpioModule_Uninit()
{
    NEXUS_LockModule();
    BINT_DisableCallback(g_NEXUS_gpio.intCallback);
    BINT_DestroyCallback(g_NEXUS_gpio.intCallback);
    #ifdef BCHP_INT_ID_aon_gio_irqen
    BINT_DisableCallback(g_NEXUS_gpio.intAonCallback);
    BINT_DestroyCallback(g_NEXUS_gpio.intAonCallback);
    #endif
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_gpioModule);
    g_NEXUS_gpioModule = NULL;
}

/****************************************
* API functions
***************/

void NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType type, NEXUS_GpioSettings *pSettings)
{
    BSTD_UNUSED(type);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_GpioHandle NEXUS_Gpio_OpenAux(unsigned typeAndPin, const NEXUS_GpioSettings *pSettings)
{
    NEXUS_GpioHandle gpio;
    BERR_Code rc;
    NEXUS_GpioSettings defaultSettings;
#ifndef NEXUS_GPIO_REGISTER_ABSTRACTION
    uint32_t address;
#endif
#define NEXUS_GPIO_TYPE(typeAndPin) (typeAndPin >> 16)
#define NEXUS_GPIO_PIN(typeAndPin) (typeAndPin & 0xFFFF)
    NEXUS_GpioType type = NEXUS_GPIO_TYPE(typeAndPin);
    unsigned pin = NEXUS_GPIO_PIN(typeAndPin);

    /* There is no bounds check for type or pin in this generic file.
    When the general eMax was extended from 2 to 3, no existing copies of nexus_gpio_table.c were updated to handle it.
    The worst case is that type>1 is handled as if type==0.
    If more rigorous checking is needed, it must be done in each chip's nexus_gpio_table.c. */

    /* chip-specific sanity check */
    rc = NEXUS_Gpio_P_CheckPinmux(type, pin);
    if ( rc!=NEXUS_SUCCESS ) { rc = BERR_TRACE(rc); goto err_pinmux;}

    gpio = BKNI_Malloc(sizeof(*gpio));
    if(!gpio) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BKNI_Memset(gpio, 0, sizeof(*gpio));
    BDBG_OBJECT_SET(gpio, NEXUS_Gpio);

    if (!pSettings) {
        NEXUS_Gpio_GetDefaultSettings(type, &defaultSettings);
        pSettings = &defaultSettings;
    }

    gpio->pin = pin;
    gpio->type = type;
#if NEXUS_GPIO_REGISTER_ABSTRACTION
    rc = NEXUS_Gpio_P_GetPinData(gpio);
    if ( rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_pindata;}
#else
    rc = NEXUS_Gpio_P_GetPinData(type, pin, &address, &gpio->shift);
    if ( rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_pindata;}

	/* populate the addresses */
    {
	unsigned offset = address - BCHP_GIO_ODEN_LO;
	gpio->addr.iodir=BCHP_GIO_IODIR_LO+offset;
	gpio->addr.data=BCHP_GIO_DATA_LO+offset;
	gpio->addr.oden=BCHP_GIO_ODEN_LO+offset;
	gpio->addr.mask=BCHP_GIO_MASK_LO+offset;
	gpio->addr.ec= BCHP_GIO_EC_LO+offset;
	gpio->addr.ei= BCHP_GIO_EI_LO+offset;
	gpio->addr.level=BCHP_GIO_LEVEL_LO+offset;
	gpio->addr.stat=BCHP_GIO_STAT_LO+offset;	
	}
#endif
    gpio->isrCallback = NEXUS_IsrCallback_Create(gpio,NULL);
    if(!gpio->isrCallback) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_callback;}

    BKNI_EnterCriticalSection();
    BLST_S_INSERT_HEAD(&g_NEXUS_gpio.list, gpio, link);
    BKNI_LeaveCriticalSection();

    rc = NEXUS_Gpio_SetSettings(gpio, pSettings);
    if (rc) {rc=BERR_TRACE(rc); goto err_gpio;}

    return gpio;

err_pindata:
err_callback:
err_gpio:
    NEXUS_Gpio_Close(gpio);
err_alloc:
err_pinmux:
    return NULL;
}

void NEXUS_Gpio_Close(NEXUS_GpioHandle gpio)
{
    BDBG_OBJECT_ASSERT(gpio, NEXUS_Gpio);
    if (gpio->isrCallback) {
        NEXUS_IsrCallback_Destroy(gpio->isrCallback);
    }
    BKNI_EnterCriticalSection();
    BLST_S_REMOVE(&g_NEXUS_gpio.list, gpio, NEXUS_Gpio, link);
    BKNI_LeaveCriticalSection();
    BDBG_OBJECT_DESTROY(gpio, NEXUS_Gpio);
    BKNI_Free(gpio);
}

void NEXUS_Gpio_GetSettings(NEXUS_GpioHandle gpio, NEXUS_GpioSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(gpio, NEXUS_Gpio);
    *pSettings = gpio->settings;
}

static void NEXUS_Gpio_P_SetBit_isr(uint32_t addr, unsigned shift, bool set)
{
    uint32_t val = BREG_Read32(g_pCoreHandles->reg, addr);
    if (set) {
        val |= (1 << shift);
    }
    else{
        val &= ~(1 << shift);
    }
    BREG_Write32(g_pCoreHandles->reg, addr, val);
}

static unsigned NEXUS_Gpio_P_GetBit(uint32_t addr, unsigned shift)
{
    /* No critical section because its an atomic read. */
    return (BREG_Read32(g_pCoreHandles->reg, addr) >> shift) & 0x1;
}

void NEXUS_Gpio_SetInterruptEnabled_isr(NEXUS_GpioHandle gpio, bool enabled)
{
      NEXUS_Gpio_P_SetBit_isr(gpio->addr.mask, gpio->shift, enabled); /* The MASK register is a misnomer. MASK = 1 is unmasked. */
}

NEXUS_Error NEXUS_Gpio_SetSettings(NEXUS_GpioHandle gpio, const NEXUS_GpioSettings *pSettings)
{
    unsigned edge_conf = 0, edge_insensitive = 0, edge_level = 0, enabled = 1;

    BDBG_OBJECT_ASSERT(gpio, NEXUS_Gpio);

    switch (pSettings->interruptMode){
    case NEXUS_GpioInterrupt_eRisingEdge:
        edge_conf = 1;
        edge_insensitive = 0;
        edge_level = 0;
        break;
    case NEXUS_GpioInterrupt_eFallingEdge:
        edge_conf = 0;
        edge_insensitive = 0;
        edge_level = 0;
        break;
    case NEXUS_GpioInterrupt_eEdge:
        edge_conf = 0;
        edge_insensitive = 1;
        edge_level = 0;
        break;
    case NEXUS_GpioInterrupt_eHigh:
        edge_conf = 1;
        edge_insensitive = 0;
        edge_level = 1;
        break;
    case NEXUS_GpioInterrupt_eLow:
        edge_conf = 0;
        edge_insensitive = 0;
        edge_level = 1;
        break;
    default: /* NEXUS_GpioInterrupt_eDisabled */
        enabled = 0;
        break;
    }

    NEXUS_IsrCallback_Set(gpio->isrCallback, &pSettings->interrupt);

    BKNI_EnterCriticalSection();

    NEXUS_Gpio_P_SetBit_isr(gpio->addr.iodir, gpio->shift, pSettings->mode == NEXUS_GpioMode_eInput);
    
    if (pSettings->mode != NEXUS_GpioMode_eInput) {
        NEXUS_Gpio_P_SetBit_isr(gpio->addr.data, gpio->shift, pSettings->value);
    }
    
    NEXUS_Gpio_P_SetBit_isr(gpio->addr.oden, gpio->shift, pSettings->mode == NEXUS_GpioMode_eOutputOpenDrain);

    if (!enabled) {
        /* Mask before resetting interrupt condition bits to avoid a re-trigger */
        NEXUS_Gpio_P_SetBit_isr(gpio->addr.mask, gpio->shift, 0); /* The MASK register is a misnomer. MASK = 1 is unmasked. */
    }

    NEXUS_Gpio_P_SetBit_isr(gpio->addr.ec, gpio->shift, edge_conf);
    NEXUS_Gpio_P_SetBit_isr(gpio->addr.ei, gpio->shift, edge_insensitive);
    NEXUS_Gpio_P_SetBit_isr(gpio->addr.level, gpio->shift, edge_level);

    gpio->settings = *pSettings;
    
    if (enabled) {
        NEXUS_Gpio_P_SetBit_isr(gpio->addr.mask, gpio->shift, enabled); /* The MASK register is a misnomer. MASK = 1 is unmasked. */
    }
    BKNI_LeaveCriticalSection();

    return 0;
}

NEXUS_Error NEXUS_Gpio_GetStatus(NEXUS_GpioHandle gpio, NEXUS_GpioStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(gpio, NEXUS_Gpio);
    BKNI_EnterCriticalSection();
    pStatus->value = NEXUS_Gpio_P_GetBit(gpio->addr.data, gpio->shift);
    pStatus->interruptPending = NEXUS_Gpio_P_GetBit(gpio->addr.stat, gpio->shift);
    BKNI_LeaveCriticalSection();
    return 0;
}

NEXUS_Error NEXUS_Gpio_ClearInterrupt(NEXUS_GpioHandle gpio)
{
    BDBG_OBJECT_ASSERT(gpio, NEXUS_Gpio);
    BKNI_EnterCriticalSection();
    BREG_Write32(g_pCoreHandles->reg, gpio->addr.stat, 1 << gpio->shift);
    if ( gpio->settings.interruptMode != NEXUS_GpioInterrupt_eDisabled )
    {
        /* Re-enable interrupts.  May be a masked level interrupt */
        NEXUS_Gpio_P_SetBit_isr(gpio->addr.mask, gpio->shift, 1); /* The MASK register is a misnomer. MASK = 1 is unmasked. */
    }
    BKNI_LeaveCriticalSection();
    return 0;
}

void NEXUS_Gpio_SetInterruptCallback_priv(NEXUS_GpioHandle gpio, NEXUS_GpioIsrCallback callback_isr, void *context, int param)
{
    BKNI_EnterCriticalSection();
    gpio->directIsrCallback.callback_isr = callback_isr;
    gpio->directIsrCallback.context = context;
    gpio->directIsrCallback.param = param;
    BKNI_LeaveCriticalSection();
}

static bool NEXUS_Gpio_P_Dispatch_isr(NEXUS_GpioHandle gpio)
{
    if (NEXUS_Gpio_P_GetBit(gpio->addr.stat, gpio->shift) &
        NEXUS_Gpio_P_GetBit(gpio->addr.mask, gpio->shift))
    {
        /* clear status immediately */
        BREG_Write32(g_pCoreHandles->reg,gpio->addr.stat, 1 << gpio->shift);
        if ( gpio->settings.maskEdgeInterrupts || 
             gpio->settings.interruptMode == NEXUS_GpioInterrupt_eLow ||
             gpio->settings.interruptMode == NEXUS_GpioInterrupt_eHigh )
        {
            /* Mask a level interrupt */
            NEXUS_Gpio_P_SetBit_isr(gpio->addr.mask, gpio->shift, 0);
            /* The MASK register is a misnomer. MASK = 1 is unmasked. */
        }
        if(gpio->isrCallback) {
            NEXUS_IsrCallback_Fire_isr(gpio->isrCallback);
        }
        
        if(gpio->directIsrCallback.callback_isr){
            gpio->directIsrCallback.callback_isr(gpio->directIsrCallback.context, gpio->directIsrCallback.param);
        }
            
        return true;
    }

    return false;
}

static void NEXUS_Gpio_P_isr(void *context, int param)
{
    NEXUS_GpioHandle gpio;
    BDBG_MSG(("NEXUS_Gpio_P_isr"));

    BSTD_UNUSED(context);
    BSTD_UNUSED(param);

#if 0 /* Useful for debugging flood scenarios */
    BDBG_MSG(("GIO_MASK_LO 0x%08x GIO_STAT_LO 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_LO), BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_STAT_LO)));
    #ifdef BCHP_GIO_MASK_HI
    BDBG_MSG(("GIO_MASK_HI 0x%08x GIO_STAT_HI 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_HI), BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_STAT_HI)));
    #endif
    #ifdef BCHP_GIO_MASK_EXT
    BDBG_MSG(("GIO_MASK_EXT 0x%08x GIO_STAT_EXT 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT), BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_STAT_EXT)));
    #endif
    #ifdef BCHP_GIO_MASK_EXT_HI
    BDBG_MSG(("GIO_MASK_EXT_HI 0x%08x GIO_STAT_EXT_HI 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT_HI), BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT_HI)));
    #endif
    #ifdef BCHP_GIO_MASK_EXT2
    BDBG_MSG(("GIO_MASK_EXT2 0x%08x GIO_STAT_EXT2 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT2), BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_MASK_EXT2)));
    #endif
    #ifdef BCHP_TVM_GPIO_MASK_0
    BDBG_MSG(("TVM_GPIO_MASK_0 0x%08x TVM_GPIO_STAT_0 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_TVM_GPIO_MASK_0), BREG_Read32(g_pCoreHandles->reg, BCHP_TVM_GPIO_STAT_0)));
    #endif
    #ifdef BCHP_TVM_GPIO_MASK_1
    BDBG_MSG(("TVM_GPIO_MASK_1 0x%08x TVM_GPIO_STAT_1 0x%08x", BREG_Read32(g_pCoreHandles->reg, BCHP_TVM_GPIO_MASK_1), BREG_Read32(g_pCoreHandles->reg, BCHP_TVM_GPIO_STAT_1)));
    #endif
#endif

    for (gpio=BLST_S_FIRST(&g_NEXUS_gpio.list); gpio; gpio = BLST_S_NEXT(gpio, link)) {
        if (NEXUS_Gpio_P_Dispatch_isr(gpio)) {
            BDBG_MSG(("Dispatched GPIO interrupt %u, type %u", gpio->pin, gpio->type));
        }
    }
}

NEXUS_Error NEXUS_Gpio_GetPinMux( unsigned typeAndPin, uint32_t *pAddr, uint32_t *pMask, unsigned *pShift )
{
    NEXUS_GpioType type = NEXUS_GPIO_TYPE(typeAndPin);
    unsigned pin = NEXUS_GPIO_PIN(typeAndPin);
    return NEXUS_Gpio_P_GetPinMux(type, pin, pAddr, pMask, pShift );
}

NEXUS_Error NEXUS_GpioModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BERR_Code rc = NEXUS_SUCCESS;

    if (enabled) {
	if(pSettings->mode==NEXUS_StandbyMode_eDeepSleep) {
	    rc = BINT_DisableCallback(g_NEXUS_gpio.intCallback);
	    g_NEXUS_gpio.s3standby = true;
	}
    } else {
	if(g_NEXUS_gpio.s3standby) {
	    rc = BINT_EnableCallback(g_NEXUS_gpio.intCallback);
	    g_NEXUS_gpio.s3standby = false;
	}
    }
    return rc;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif           
}
