/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: dvb_ci.c $
 * $brcm_Revision: Hydra_Software_Devel/29 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/dvb_ci/dvb_ci.c $
 * 
 * Hydra_Software_Devel/29   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "nexus_platform.h"
#include "nexus_gpio.h"
#include <stdio.h>

#ifdef NEXUS_HAS_DVB_CI
#include "nexus_dvb_ci.h"

#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125)
    /* Limit buffer size to 1024 bytes to minimize the lattency on the FlexBus */
	#define DVB_CI_MAX_HOST_BUFFER_SIZE 1024
#else
    /* No limit (actually limited to 64 kB) */
	#define DVB_CI_MAX_HOST_BUFFER_SIZE 0
#endif

static void init_settings(NEXUS_DvbCiOpenSettings *pSettings);

static void card_insertion(void *pParam, int param)
{
    NEXUS_DvbCiStatus status;

    param=param;

    fprintf(stderr, "Card insertion status changed\n");

    NEXUS_DvbCi_GetStatus(pParam, &status);
    /* If the card has been inserted and there is no valid CIS, we need to reset */
    if ( status.cardPresent )
    {
        /* If the CIS is valid, we can proceed with the personality change and size negotiation */
        if ( status.validCis )
        {
            uint16_t bufferSize=0;
            /* CIS is valid */
            fprintf(stderr, "CIS is valid\n");
            fprintf(stderr, "Switching to DVB mode\n");
            NEXUS_DvbCi_ChangePersonality(pParam);
            fprintf(stderr, "Negotiating buffer sizes\n");
            NEXUS_DvbCi_NegotiateBuffers(pParam, DVB_CI_MAX_HOST_BUFFER_SIZE, &bufferSize);
            fprintf(stderr, "Negotiated buffer size is %u bytes\n", bufferSize);
        }
        else
        {
            /* CIS is invalid.  Reset the card to read it */
            fprintf(stderr, "Card inserted\n");
            fprintf(stderr, "Resetting Card...\n");
            NEXUS_DvbCi_Reset(pParam);
        }
    }
    else
    {
        fprintf(stderr, "Card Removed\n");
    }
}

static void card_error(void *pParam, int param)
{
    NEXUS_DvbCiStatus status;

    param=param;

    fprintf(stderr, "Card error detected - checking status\n");
    NEXUS_DvbCi_GetStatus(pParam, &status);
    fprintf(stderr, "Error Code is %d\n", status.lastError);
    fprintf(stderr, "Resetting card via PCMCIA reset\n");
    NEXUS_DvbCi_Reset(pParam);
}

int main(int argc, char **argv)
{
    NEXUS_DvbCiHandle handle;
    NEXUS_DvbCiOpenSettings openSettings;
    NEXUS_DvbCiSettings settings;

    argc=argc;
    argv=argv;

    NEXUS_Platform_Init(NULL);

    NEXUS_DvbCi_GetDefaultOpenSettings(&openSettings);
    openSettings.deviceOffset = NEXUS_DVB_CI_MEMORY_BASE;
#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125) || (BCHP_CHIP == 35233)
    openSettings.deviceOffsetIO = NEXUS_DVB_CI_IO_MEMORY_BASE;
#endif
    openSettings.ebiChipSelect = NEXUS_DVB_CI_CHIP_SELECT;

    init_settings(&openSettings);

    handle = NEXUS_DvbCi_Open(&openSettings);
    if ( NULL == handle )
    {
        fprintf(stderr, "Unable to open DVB-CI device\n");
        return -1;
    }

    fprintf(stderr, "Installing callbacks\n");
    NEXUS_DvbCi_GetSettings(handle, &settings);
    settings.cardDetectCallback.callback = card_insertion;
    settings.cardDetectCallback.context = handle;
    settings.errorCallback.callback = card_error;
    settings.errorCallback.context = handle;
    NEXUS_DvbCi_SetSettings(handle, &settings);

    fprintf(stderr, "Forcing card check\n");
    card_insertion(handle, 0);

    fprintf(stderr, "Press enter to exit\n");
    getchar();

    return 0;
}

#if BCHP_CHIP == 3556
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    NEXUS_GpioHandle u2902Pin;
    NEXUS_GpioSettings gpioSettings;

    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 29, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 38, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 19, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 39, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 57, NULL);
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 1, NULL);
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 42, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 43, NULL);
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;

    /* enable chip U2902 */
    u2902Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 40, NULL);
    NEXUS_Gpio_GetSettings(u2902Pin, &gpioSettings);
    gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
    gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
    gpioSettings.value = NEXUS_GpioValue_eLow;
    NEXUS_Gpio_SetSettings(u2902Pin, &gpioSettings);
}
#elif BCHP_CHIP == 35230
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 6, NULL);
    pSettings->cd2Pin = NULL;   /* This chip is fixed 5V and uses an external OR gate for CD1/CD2 */
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 7, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 86, NULL);          /* MCIF_SDI pinmux */
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 87, NULL);     /* MCIF_SDO pinmux */
    pSettings->vcctristateValue = 1;
    pSettings->vcc3vValue = 1;
    pSettings->vcc5vValue = 0;
}
#elif BCHP_CHIP == 35125
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 2, NULL);
    pSettings->cd2Pin = NULL;   /* This chip is fixed 5V and uses an external OR gate for CD1/CD2 */
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 8, NULL);          /* USB_PWRFLT_1 pinmux */
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 3, NULL);          
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 9, NULL);     /* USB_PWRFLT_2 pinmux */
    pSettings->vcctristateValue = 1;
    pSettings->vcc3vValue = 1;
    pSettings->vcc5vValue = 0;
}
#elif BCHP_CHIP == 35233
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 28, NULL); 
    pSettings->cd2Pin = NULL;   /* This chip is fixed 5V and uses an external OR gate for CD1/CD2 */
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 99, NULL);          
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eTvMicro, 36, NULL);          
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 56, NULL);     
    pSettings->vcctristateValue = 1;
    pSettings->vcc3vValue = 1;
    pSettings->vcc5vValue = 0;
}
#elif BCHP_CHIP == 3563
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 3563 platform\n");
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 21, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 22, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 63, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 28, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 62, NULL);
    pSettings->vccEnablePin1 = NULL;
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 1;
    pSettings->vcc5vValue = 2;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 3, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 4, NULL);
    pSettings->vpp1EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 13, NULL);
    pSettings->vpp1EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 18, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp1tristateValue = 0;
    pSettings->vpp03vValue = 1;
    pSettings->vpp13vValue = 1;
    pSettings->vpp05vValue = 2;
    pSettings->vpp15vValue = 2;
}
#elif BCHP_CHIP == 7405
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    NEXUS_PlatformConfiguration platformConfig;

    fprintf(stderr, "Opening GPIO's for 7405 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 85, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 24, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 108, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 84, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 110, NULL);    /* The 97405 only supports one VCC voltage based on RD2901 */
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 1;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 5, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 4, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vpp1EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 3, NULL);
    pSettings->vpp1EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 2, NULL);
    pSettings->vpp1tristateValue = 0;
    pSettings->vpp13vValue = 2;
    pSettings->vpp15vValue = 1;
    /* VS pins are not useful on the 97405 because of the fixed VCC voltage above */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    pSettings->fpgaI2c = platformConfig.i2c[2];
    pSettings->fpgaI2cAddr = 0x25;
}
#elif BCHP_CHIP == 7325
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7325 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 22, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 1, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 24, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 0, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 25, NULL);
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 26, NULL);
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 20, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 21, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 23, NULL);
    /* pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 18, NULL);  */
}
#elif BCHP_CHIP == 7335
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7335 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 33, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 32, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 36, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 23, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 24, NULL); /* 23 is used by readyPin */
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 82, NULL); /* gpio_24=0 (5v), gpio_24=1 (0v) */
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 40, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 41, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 37, NULL); /* card in, still low */
    /* pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 82, NULL); */
}
#elif BCHP_CHIP == 7340
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7340 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 00, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 01, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 03, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 68, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 22, NULL);
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 23, NULL);
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 20, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 21, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 02, NULL); /* card in, still low */
    /* pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 68, NULL); */
}
#elif BCHP_CHIP == 7344
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7344 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 20, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 21, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 18, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 59, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 41, NULL);
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 42, NULL);
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 77, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 78, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 22, NULL); 
    /* pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 68, NULL); */
    /* pSettings->ebiEnablePin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 15, NULL); */
    /* pSettings->ebiEnableValue = 0; */
}
#elif BCHP_CHIP == 7346
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7346 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 45, NULL);
    pSettings->cd2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 56, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 15, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 70, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 12, NULL);
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 13, NULL);
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 10, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 11, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 14, NULL); 
    /* pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 68, NULL); */
}
#elif (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552)
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7358 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 100, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 78, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 96, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 76, NULL);
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 77, NULL);
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 74, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 75, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
/*
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 98, NULL); 
    pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 92, NULL);
*/
}
#elif BCHP_CHIP == 7420
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7420 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 39, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 103, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 50, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 61, NULL); 
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 62, NULL); 
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 55, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 56, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eSpecial, 5, NULL); 
    pSettings->ebiEnablePin = NEXUS_Gpio_Open(NEXUS_GpioType_eSpecial, 4, NULL); 
    pSettings->ebiEnableValue = 0;
}
#elif (BCHP_CHIP == 7231) || (BCHP_CHIP == 7241) || (BCHP_CHIP == 7429)
static void init_settings(NEXUS_DvbCiOpenSettings *pSettings)
{
    fprintf(stderr, "Opening GPIO's for 7231 platform\n");
    pSettings->littleEndian = false;
    pSettings->cd1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 77, NULL);
    pSettings->resetPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 78, NULL);
    pSettings->readyPin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 99, NULL);
    pSettings->vccEnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 76, NULL); 
    pSettings->vccEnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 122, NULL); 
    pSettings->vcctristateValue = 0;
    pSettings->vcc3vValue = 2;
    pSettings->vcc5vValue = 1;
    pSettings->vpp0EnablePin0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 74, NULL);
    pSettings->vpp0EnablePin1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 75, NULL);
    pSettings->vpp0tristateValue = 0;
    pSettings->vpp03vValue = 2;
    pSettings->vpp05vValue = 1;
    /* pSettings->ebiEnablePin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 124, NULL);
    pSettings->ebiEnableValue = 0; */

    pSettings->vs1Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 131, NULL); 
    /* pSettings->vs2Pin = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 97, NULL); */

}
#else
#error Unsupported chip
#endif

#else

int main(int argc, char **argv)
{
    argc=argc;
    argv=argv;
    fprintf(stderr, "No DVB-CI Support\n");
    return 0;
}

#endif
