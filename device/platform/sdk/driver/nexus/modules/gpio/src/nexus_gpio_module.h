/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_gpio_module.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 2/16/11 2:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/gpio/7400/src/nexus_gpio_module.h $
 * 
 * 5   2/16/11 2:55p randyjew
 * SW7344-25: Add Gpio register abstraction to support AON Gpio's and
 *  other Gpio register ranges.
 * 
 * 4   12/6/10 10:53a erickson
 * SW7408-146: add NEXUS_Gpio_GetPinMux
 * 
 * 3   4/8/09 4:31p vsilyaev
 * PR 54016: Added support for precompiled header
 * 
 * 2   1/26/09 11:29a erickson
 * PR51468: global variable naming convention
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   11/30/07 11:08a jgarrett
 * PR 37801: Adjusting register offsets
 *
 * Nexus_Devel/2   11/26/07 9:45a erickson
 * PR37423: gpio update
 *
 * Nexus_Devel/1   11/20/07 1:28p erickson
 * PR37423: added uart, gpio, spi modules
 *
 **************************************************************************/
#ifndef NEXUS_GPIO_MODULE_H__
#define NEXUS_GPIO_MODULE_H__

#include "nexus_base.h"
#include "nexus_gpio_thunks.h"
#include "nexus_platform_features.h"
#include "nexus_gpio.h"
#include "nexus_gpio_init.h"
#include "priv/nexus_core.h"
#include "priv/nexus_gpio_priv.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME gpio
#define NEXUS_MODULE_SELF g_NEXUS_gpioModule
#if BCHP_CHIP == 7344
#define NEXUS_GPIO_REGISTER_ABSTRACTION 1
#endif
struct NEXUS_Gpio {
    BDBG_OBJECT(NEXUS_Gpio)
    BLST_S_ENTRY(NEXUS_Gpio) link;
    NEXUS_GpioType type;
    unsigned pin;
    NEXUS_GpioSettings settings;
    /*uint32_t offset; *//* register set offset from LO */
    uint32_t shift;
    NEXUS_IsrCallbackHandle isrCallback;
    struct {
        NEXUS_GpioIsrCallback callback_isr;
        void *context;
        int param;
    }directIsrCallback;
    struct {
        uint32_t iodir, data, oden, mask, ec, ei, level,stat;
    } addr; /* register addresses for GPIO abstraction */    
};

/* global handle. there is no global data. */
extern NEXUS_ModuleHandle g_NEXUS_gpioModule;

/* These functions must be implemented per-chip */
NEXUS_Error NEXUS_Gpio_P_CheckPinmux(NEXUS_GpioType type, unsigned pin);
#if NEXUS_GPIO_REGISTER_ABSTRACTION
NEXUS_Error NEXUS_Gpio_P_GetPinData(NEXUS_GpioHandle gpio);
#else
NEXUS_Error NEXUS_Gpio_P_GetPinData(NEXUS_GpioType type, unsigned pin, uint32_t *pAddress, uint32_t *pShift);
#endif
NEXUS_Error NEXUS_Gpio_P_GetPinMux(NEXUS_GpioType type, unsigned pin, uint32_t *pAddr, uint32_t *pMask, unsigned *pShift);

#ifdef __cplusplus
}
#endif

#endif
