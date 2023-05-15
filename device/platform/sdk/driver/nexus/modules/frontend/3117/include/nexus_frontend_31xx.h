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
* $brcm_Workfile: nexus_frontend_31xx.h $
* $brcm_Revision: 15 $
* $brcm_Date: 3/4/11 3:34p $
*
* API Description:
*   API name: Frontend 31xx
*    APIs to open, close, and setup initial settings for a BCM31xx
*    Cable Tuner/Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/3117/include/nexus_frontend_31xx.h $
* 
* 15   3/4/11 3:34p erickson
* SW7422-171: change NEXUS_Frontend_31xx_SetConfigSettings pSettings to
*  const
*
* 14   12/9/10 11:47a erickson
* SW7405-5037: add NEXUS_31xxSettings.i2cSlaveAddr
*
* 13   11/19/10 11:10a erickson
* SW7422-71: added NEXUS_31xxSettings.gpioInterrupt
*
* 12   10/28/10 4:06p mphillip
* SW7420-1020: Add asynchronous status support to oob frontends
*
* 11   10/21/10 5:51p mphillip
* SW7420-1020: Add asynchronous status API
*
* 10   10/7/10 10:42a mphillip
* SW7125-35: Add loadAP for 7125
*
* SW7125-35/3   10/6/10 12:07p mward
* SW7125-35: Merge changes from main.
*
* SW7125-35/2   8/24/10 1:43p mward
* SW7125-35: Merge changes from main.
*
* SW7125-35/1   6/21/10 12:17p mward
* SW7125-35:  Add a setting to specify whether or not to load AP code.
*
* 9   10/5/10 7:22p mphillip
* SW7405-4915: Add configuration settings for 31xx
*
* 8   9/21/10 6:02p mphillip
* SW7405-4889: Expose Ads and Aob open drain options in nexus
*
* 7   9/7/10 8:21p mphillip
* SW7420-1049: Add isr callbacks for BHAB_Interrupt_eUpdateGain/eNoSignal
*
* 6   7/14/10 3:04p mphillip
* SW7420-891: Add support to program TNR AGC gain
*
* 5   10/5/09 10:57a erickson
* SW7405-2851: add configureWatchdog
*
* 4   6/26/09 1:59p erickson
* PR56235: add NEXUS_31xxSettings.ifFrequency
*
* 3   3/11/09 6:33p vsilyaev
* PR 46431: share the same API file for BCM3117, BCM3114, and BCM3112
*
* 2   3/6/09 9:27a jgarrett
* PR 46431: Adding NMI interrupt
*
* 1   1/19/09 12:12p erickson
* PR46431: add nexus_frontend_31xx.h
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_31xx_H__
#define NEXUS_FRONTEND_31xx_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_frontend.h"
#include "nexus_gpio.h"


/***************************************************************************
Summary:
3117 Device Handle
***************************************************************************/
typedef struct NEXUS_31xxDevice *NEXUS_31xxDeviceHandle;

/***************************************************************************
Summary:
Settings for a BCM31xx Device Channel
***************************************************************************/
typedef enum NEXUS_31xxChannelType
{
    NEXUS_31xxChannelType_eInBand,
    NEXUS_31xxChannelType_eOutOfBand,
    NEXUS_31xxChannelType_eUpstream,
    NEXUS_31xxChannelType_eMax
} NEXUS_31xxChannelType;

/***************************************************************************
Summary:
Settings for a BCM31xx Device
***************************************************************************/
typedef struct NEXUS_31xxSettings
{
    NEXUS_I2cHandle i2cDevice;      /* I2C device to use */
    NEXUS_GpioHandle gpioInterrupt; /* GPIO pin for interrupt. If not NULL, isrNumber is ignored. If NULL, isrNumber is used. */
    unsigned isrNumber;             /* L1 interrupt number. (typically 0..63). See gpioInterrupt for other interrupt option. */
    uint16_t i2cAddr;               /* 31xx's master device I2C Address */
    uint8_t i2cSlaveAddr;           /* 31xx's slave device I2C Address */

    NEXUS_31xxChannelType type;     /* Channel type to open from this device */
    unsigned channelNumber;         /* Which channel to open from this device */
    unsigned ifFrequency;           /* IF frequency used for 31xx OOB module. */
    bool configureWatchdog;         /* Depending on the platform decide whether to configure watchdog or not. */
    bool outOfBandOpenDrain;        /* If true, out of band is open drain else it is push pull. */
    bool inBandOpenDrain;           /* If true, inband is open drain else it is push pull. */
    bool loadAP;                    /* Load Acquisition Processor microcode when starting. */
} NEXUS_31xxSettings;

/***************************************************************************
Summary:
Lna Settings for a BCM31xx Device Channel
***************************************************************************/

typedef struct NEXUS_31xxConfigSettings
{
    unsigned agcValue;                        /* Gain Value*/
    NEXUS_CallbackDesc updateGainCallback;    /* Callback will be called when the gain from the lna needs to be updated. */
    bool enableDaisyChain;
    NEXUS_CallbackDesc qamAsyncStatusReadyCallback;   /* Callback will be called when the async qam status is ready. */
    NEXUS_CallbackDesc oobAsyncStatusReadyCallback;   /* Callback will be called when the async out of band status is ready. */
} NEXUS_31xxConfigSettings;

/***************************************************************************
Summary:
Get the default settings for a BCM31xx tuner

Description:

See Also:
NEXUS_Frontend_Open31xx
***************************************************************************/
void NEXUS_Frontend_GetDefault31xxSettings(
    NEXUS_31xxSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
Open a handle to a BCM31xx device.

Description:

See Also:
NEXUS_Frontend_Close
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open31xx( /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_31xxSettings *pSettings
    );

/***************************************************************************
Summary:
Get the default config settings to a BCM31xx device.


Description:

See Also:
NEXUS_Frontend_Open31xx
***************************************************************************/
void NEXUS_Frontend_GetDefault31xxConfigSettings(
    NEXUS_31xxConfigSettings *pConfigSettings   /* [out] */
    );

/***************************************************************************
Summary:
Get the config settings to a BCM31xx device.

Description:

See Also:NEXUS_Frontend_31xx_SetLnaGain
***************************************************************************/
NEXUS_Error NEXUS_Frontend_31xx_GetConfigSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_31xxConfigSettings *pConfigSettings    /* [out]  */
    );

/***************************************************************************
Summary:
Set the config settings to a BCM31xx device.

Description:

See Also:NEXUS_Frontend_31xx_GetLnaGain
***************************************************************************/

NEXUS_Error NEXUS_Frontend_31xx_SetConfigSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_31xxConfigSettings *pConfigSettings
    );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_31xx_H__ */


