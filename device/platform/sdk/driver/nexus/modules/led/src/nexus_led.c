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
 * $brcm_Workfile: nexus_led.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 9/28/12 8:45a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/led/src/nexus_led.c $
 * 
 * 12   9/28/12 8:45a erickson
 * SW7435-344: convert spi/gpio handle to index in module settings. only
 *  open spi/gpio when led/keypad is actually opened.
 *
 * 11   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 *
 * 10   5/11/10 5:04p jtna
 * SW7125-307: Coverity - check for null pointer arguments before
 *  dereferencing them
 *
 * 9   11/18/09 2:07p gmohile
 * SW7408-1 : Use NEXUS_HAS_SPI for spi frontend
 *
 * 8   10/14/09 2:32p jtna
 * SW7405-3181: add NEXUS_Led_WriteSegments
 *
 * 7   7/23/09 12:00p erickson
 * PR57047: add NEXUS_Led_SetDot
 *
 * 6   4/9/09 11:40a jrubio
 * PR52188: add support for 2nd Receiver
 *
 * 5   1/26/09 1:54p erickson
 * PR51468: global variable naming convention
 *
 * 4   8/18/08 10:28a katrep
 * PR45674: Compiler warnings in DEBUG=n builds
 *
 * 3   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 2   3/31/08 12:32p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   11/21/07 11:12a erickson
 * PR37423: update
 *
 * Nexus_Devel/1   11/20/07 4:53p erickson
 * PR37423: adding ir_input and led
 *
 **************************************************************************/
#include "nexus_led_module.h"
#include "bled.h"
#include "priv/nexus_core.h"
#if NEXUS_HAS_SPI
#include "nexus_spi.h"
#endif

BDBG_MODULE(nexus_led);

NEXUS_ModuleHandle g_NEXUS_ledModule;
struct {
    NEXUS_LedModuleSettings settings;
} g_NEXUS_led;

/****************************************
* Module functions
***************/
void NEXUS_LedModule_GetDefaultSettings(NEXUS_LedModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}
NEXUS_ModuleHandle NEXUS_LedModule_Init(const NEXUS_LedModuleSettings *pSettings)
{
    BDBG_ASSERT(!g_NEXUS_ledModule);
    g_NEXUS_ledModule = NEXUS_Module_Create("led", NULL);
    if (pSettings) {
        g_NEXUS_led.settings = *pSettings;
    }
    else {
        NEXUS_LedModule_GetDefaultSettings(&g_NEXUS_led.settings);
    }
    return g_NEXUS_ledModule;
}
void NEXUS_LedModule_Uninit()
{
    NEXUS_Module_Destroy(g_NEXUS_ledModule);
    g_NEXUS_ledModule = NULL;
}

/****************************************
* API functions
***************/

#define NEXUS_NUM_LED_DIGITS 4

struct NEXUS_Led {
    NEXUS_OBJECT(NEXUS_Led);
    BLED_Handle led;
    NEXUS_LedSettings settings;
    uint8_t ledState;
    bool dots[NEXUS_NUM_LED_DIGITS];
    uint8_t chars[NEXUS_NUM_LED_DIGITS];
#if NEXUS_HAS_SPI
    NEXUS_SpiHandle spi;
#endif
};

void NEXUS_Led_GetDefaultSettings(NEXUS_LedSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->brightness = 100;
}

NEXUS_LedHandle NEXUS_Led_Open(unsigned index, const NEXUS_LedSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_LedHandle led;
    BLED_Settings ledSettings;
    NEXUS_LedSettings defaultSettings;

    if (!pSettings) {
        NEXUS_Led_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* only one LED controller */
    if (index > 0) {
        BDBG_ERR(("invalid led[%d]", index));
        return NULL;
    }

    led = BKNI_Malloc(sizeof(*led));
    if (!led) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_Led, led);

#if NEXUS_HAS_SPI
    if (g_NEXUS_led.settings.spi.valid) {
        NEXUS_SpiSettings spiSettings;
        NEXUS_Spi_GetDefaultSettings(&spiSettings);
        spiSettings.clockActiveLow = true;
        led->spi = NEXUS_Spi_Open(g_NEXUS_led.settings.spi.index, &spiSettings);
        if (!led->spi) {
            rc = BERR_TRACE(NEXUS_UNKNOWN);
            goto error;
        }
        return led;
    }
#endif

    BLED_GetDefaultSettings(&ledSettings, g_pCoreHandles->chp);
    ledSettings.percentBrightness = pSettings->brightness;
    rc = BLED_Open(&led->led, g_pCoreHandles->chp, g_pCoreHandles->reg, &ledSettings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    return led;
error:
    NEXUS_Led_Close(led);
    return NULL;
}

static void NEXUS_Led_P_Finalizer(NEXUS_LedHandle led)
{
    NEXUS_OBJECT_ASSERT(NEXUS_Led, led);

#if NEXUS_HAS_SPI
    if (led->spi) {
        NEXUS_Spi_Close(led->spi);
    }
#endif

    if (led->led) {
        BLED_Close(led->led);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_Led, led);
    BKNI_Free(led);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Led, NEXUS_Led_Close);

void NEXUS_Led_GetSettings(NEXUS_LedHandle led, NEXUS_LedSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(led, NEXUS_Led);
    *pSettings = led->settings;
}

NEXUS_Error NEXUS_Led_SetSettings(NEXUS_LedHandle led, const NEXUS_LedSettings *pSettings)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(led, NEXUS_Led);

#if NEXUS_HAS_SPI
    if (!led->spi)
#endif
    {
        rc = BLED_AdjustBrightness(led->led, pSettings->brightness);
        if (rc) return BERR_TRACE(rc);
    }
    /* TODO: implement scrolling option with timer */

    led->settings = *pSettings;
    return 0;
}

static NEXUS_Error NEXUS_Led_P_WriteString(NEXUS_LedHandle led)
{
    unsigned i;

    for (i=0;i<NEXUS_NUM_LED_DIGITS;i++) {
        uint8_t code;
        BERR_Code rc;

        switch(led->chars[i]) {
        case 'A': case 'a': code = LED_A; break;
        case 'B': case 'b': code = LED_B; break;
        case 'C': case 'c': code = LED_C; break;
        case 'D': case 'd': code = LED_D; break;
        case 'E': case 'e': code = LED_E; break;
        case 'F': case 'f': code = LED_F; break;
        case 'G': case 'g': code = LED_G; break;
        case 'H': case 'h': code = LED_H; break;
        case 'I': case 'i': code = LED_I; break;
        case 'J': case 'j': code = LED_J; break;
        case 'K': case 'k': code = LED_K; break;
        case 'L': case 'l': code = LED_L; break;
        case 'M': case 'm': code = LED_M; break;
        case 'N': case 'n': code = LED_N; break;
        case 'O': case 'o': code = LED_O; break;
        case 'P': case 'p': code = LED_P; break;
        case 'Q': case 'q': code = LED_C; break;
        case 'R': case 'r': code = LED_R; break;
        case 'S': case 's': code = LED_S; break;
        case 'T': case 't': code = LED_T; break;
        case 'U': case 'u': code = LED_U; break;
        case 'V': case 'v': code = LED_V; break;
        case 'W': case 'w': code = LED_W; break;
        case 'X': case 'x': code = LED_X; break;
        case 'Y': case 'y': code = LED_Y; break;
        case 'Z': case 'z': code = LED_Z; break;
        case '-': code = LED_DASH; break;
        case '0': code = LED_NUM0; break;
        case '1': code = LED_NUM1; break;
        case '2': code = LED_NUM2; break;
        case '3': code = LED_NUM3; break;
        case '4': code = LED_NUM4; break;
        case '5': code = LED_NUM5; break;
        case '6': code = LED_NUM6; break;
        case '7': code = LED_NUM7; break;
        case '8': code = LED_NUM8; break;
        case '9': code = LED_NUM9; break;
        default: code = 0xFF;
        }

        if (led->dots[i]) {
            code &= ~0x80; /* 0 = on */
        }
        else {
            code |= 0x80; /* 1 = off */
        }

        BDBG_MSG(("write %d: %c%s, code=%x", i+1, led->chars[i], led->dots[i]?" (dot)":"", code));
        rc = BLED_Write(led->led, i+1, code);
        if (rc) { return BERR_TRACE(rc); }
    }
    return 0;
}

NEXUS_Error NEXUS_Led_WriteString(NEXUS_LedHandle led, const char *pString, unsigned length)
{
    unsigned i;

    BDBG_OBJECT_ASSERT(led, NEXUS_Led);

    /* SPI LED does not support this function */
#if NEXUS_HAS_SPI
    if (led->spi) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#endif

    if (!length && pString) {
        while (pString[length]) {
            length++;
        }
    }
    if (length > 128) {
        BDBG_WRN(("NEXUS_Led_WriteString pString length exceeds kernel mode proxy attr fixed_length comment."));
    }
    if (length > NEXUS_NUM_LED_DIGITS) {
        length = NEXUS_NUM_LED_DIGITS;
    }
    /* write the whole chars[] array */
    for (i=0;i<NEXUS_NUM_LED_DIGITS;i++) {
        if (i < length) {
            led->chars[i] = pString[i];
        }
        else {
            led->chars[i] = 0;
        }
    }

    return NEXUS_Led_P_WriteString(led);
}

NEXUS_Error NEXUS_Led_WriteSegments(NEXUS_LedHandle led, NEXUS_LedDigit digit, uint8_t value)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(led, NEXUS_Led);

    /* SPI LED does not support this function */
#if NEXUS_HAS_SPI
    if (led->spi) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#endif

    if (digit >= NEXUS_LedDigit_eMax) {
        return NEXUS_INVALID_PARAMETER;
    }

    rc = BLED_Write(led->led, digit, ~value);
    if (rc) { return BERR_TRACE(rc); }

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Led_SetDot( NEXUS_LedHandle led, unsigned dotIndex, bool enabled )
{
    BDBG_OBJECT_ASSERT(led, NEXUS_Led);

    /* SPI LED does not support this function */
#if NEXUS_HAS_SPI
    if (led->spi) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#endif

    if (dotIndex >= NEXUS_NUM_LED_DIGITS) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    led->dots[dotIndex] = enabled;
    return NEXUS_Led_P_WriteString(led);
}

NEXUS_Error NEXUS_Led_SetLedState(NEXUS_LedHandle led, unsigned ledNumber, bool active)
{
    BDBG_OBJECT_ASSERT(led, NEXUS_Led);

#if NEXUS_HAS_SPI
    if (led->spi) {
        NEXUS_Error rc;
        uint8_t chipAddr;
        uint8_t data[2];

        if (active)
            led->ledState |= (1 << ledNumber);
        else
            led->ledState &= ~(1 << ledNumber);

        /* use chip address 0 */
        chipAddr = 0;
        data[0] = (chipAddr << 1) | 0x01;
        data[1] = led->ledState;

        rc = NEXUS_Spi_Write(led->spi, data, 2);
        if (rc) return BERR_TRACE(rc);

        return 0;
    }
#endif

    return BLED_SetDiscreteLED(led->led, active, ledNumber);
}
