/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_frontend_4506.h $
* $brcm_Revision: 11 $
* $brcm_Date: 9/2/11 5:31p $
*
* API Description:
*   API name: Frontend 4506
*    APIs to open, close, and setup initial settings for a BCM4506
*    Dual-Channel Satellite Tuner/Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/4506/include/nexus_frontend_4506.h $
* 
* 11   9/2/11 5:31p mphillip
* SW7346-192: Enable/disable external LNA
* 
* 10   6/23/11 5:33p erickson
* SW7346-165: add macro
* 
* 9   3/3/11 5:02p mphillip
* SW7422-272: Add nexus gpio support for 4506 tuners
* 
* 8   11/16/10 2:24p erickson
* SW7405-4968: back out open-time networkSpec. use acquire-time.
* 
* 6   4/16/10 10:24a erickson
* SW3556-1092: added frontend master I2C (MI2c) interface for 4501/4506
*
* 5   7/31/09 4:04p erickson
* PR57239: added 4506-specific debug functions
*
* 4   7/14/09 11:16a erickson
* PR56791: added NEXUS_4506Settings.bitWideSync
*
* 3   3/13/08 7:41p jgarrett
* PR 39985: Adding 4506 support
*
* 2   2/7/08 5:36p jgarrett
* PR 39407: Converting to common close functions
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   10/4/07 2:41p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_4506_H__
#define NEXUS_FRONTEND_4506_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_frontend.h"
#include "nexus_i2c.h"
#include "nexus_gpio.h"

#define NEXUS_4506_MAX_FRONTEND_CHANNELS 2

/***************************************************************************
Summary:
    Settings for a BCM4506 Device
 ***************************************************************************/
typedef struct NEXUS_4506Settings
{
    /* The following parameters identify the device to be used. */
    NEXUS_I2cHandle      i2cDevice;         /* I2C device to use */
    uint16_t             i2cAddr;           /* Device I2C Address */
    unsigned             isrNumber;         /* L1 interrupt number (typically 0..63)*/

    /* The following parameters identify the channel of the device */
    unsigned             channelNumber;     /* Which channel from the device will be opened */

    bool                 bitWideSync;
    NEXUS_GpioHandle     gpioInterrupt;     /* GPIO pin for interrupt. If not NULL, isrNumber is ignored. If NULL, isrNumber is used. */
    bool                 is3445ExternalLna; /* If true, 4506 is connected and uses the external 3445 LNA. */
} NEXUS_4506Settings;

/***************************************************************************
Summary:
    Get the default settings for a BCM4506 frontend

Description:
See Also:
    NEXUS_Frontend_Open4506
 ***************************************************************************/
void NEXUS_Frontend_GetDefault4506Settings(
    NEXUS_4506Settings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a handle to a BCM4506 device.

Description:
See Also:
    NEXUS_Frontend_Close4506
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open4506(  /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_4506Settings *pSettings
    );

/***************************************************************************
Summary:
    Results of BCM4506 device discovery
 ***************************************************************************/
typedef struct NEXUS_4506ProbeResults
{
    uint8_t revision;
} NEXUS_4506ProbeResults;

/***************************************************************************
Summary:
    Probe to see if a BCM4506 device exists with the specified settings

Description:
See Also:
    NEXUS_Frontend_Open4506
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Probe4506(
    const NEXUS_4506Settings *pSettings,
    NEXUS_4506ProbeResults *pResults        /* [out] */
    );

/***************************************************************************
Summary:
    Get an I2C Handle for the 4506's master I2C controller.

Description:
    The BCM4506 has an on-board I2C controller that can be used to control
    tuner/amplifier devices as opposed to using a host I2C Channel.
    Please be aware, this controller has limited functionality and only
    supports the ReadNoAddr and WriteNoAddr routines.

See Also:
    NEXUS_I2c_Open
    NEXUS_I2c_ReadNoAddr
    NEXUS_I2c_WriteNoAddr
 ***************************************************************************/
NEXUS_I2cHandle NEXUS_Frontend_Get4506MasterI2c(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Write4506HostRegister(
    NEXUS_FrontendHandle handle,
    uint8_t address,
    uint8_t data
    );

/***************************************************************************
Summary:
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Read4506HostRegister(
    NEXUS_FrontendHandle handle,
    uint8_t address,
    uint8_t *pData /* [out] */
    );

/***************************************************************************
Summary:
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Read4506Memory(
    NEXUS_FrontendHandle handle,
    uint16_t addr,       /* starting address */
    uint8_t *buffer,     /* attr{nelem=bufferSize} [out] buffer into which data is read */
    uint16_t bufferSize  /* size of buffer and number of bytes to read */
    );

/***************************************************************************
Summary:
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Write4506Memory(
    NEXUS_FrontendHandle handle,
    uint16_t addr,       /* starting address */
    const uint8_t *buffer, /* attr{nelem=bufferSize} buffer from which data is written */
    uint16_t bufferSize  /* size of buffer and number of bytes to write */
    );

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_PLATFORM_DB4506)
typedef enum NEXUS_3445LnaInput
{
    NEXUS_3445LnaInput_eNone = 0, /* output is powered off */
    NEXUS_3445LnaInput_eDaisy,    /* output comes from daisy */
    NEXUS_3445LnaInput_eIn1Vga,   /* output comes from IN1(VGA) */
    NEXUS_3445LnaInput_eIn1Db,    /* output comes from IN1(DB) */
    NEXUS_3445LnaInput_eIn2Vga,   /* output comes from IN2(VGA) */
    NEXUS_3445LnaInput_eMax
} NEXUS_3445LnaInput;

typedef struct NEXUS_3445LnaSettings
{
   NEXUS_3445LnaInput out1;  /* Configure the input linked to OUT1. Set to eNone to not use this LNA. */
   NEXUS_3445LnaInput out2;  /* Configure the input linked to OUT2. Set to eNone to not use this LNA. */
   NEXUS_3445LnaInput daisy; /* Configure the input linked to DAISY. Set to eNone to not use this LNA. */
} NEXUS_3445LnaSettings;

NEXUS_Error NEXUS_Frontend_Get3445LnaAgc(
	NEXUS_FrontendHandle handle,
	uint8_t *pucAgc0,
	uint8_t *pucAgc1
);

NEXUS_Error NEXUS_Frontend_Set3445LnaSettings(
    NEXUS_FrontendHandle handle,
	const NEXUS_3445LnaSettings *pSettings
);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_4506_H__ */

