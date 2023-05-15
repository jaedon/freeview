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
* $brcm_Workfile: nexus_frontend_7346.h $
* $brcm_Revision: 10 $
* $brcm_Date: 8/29/12 1:45p $
*
* API Description:
*   API name: Frontend 7346
*    APIs to open, close, and setup initial settings for a BCM7346
*    Dual-Channel Integrated Satellite Tuner/Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/7346/include/nexus_frontend_7346.h $
* 
* 10   8/29/12 1:45p mphillip
* SW7346-932: Merge LNA gain stage disabling changes to main
* 
* CSP555064/2   8/2/12 12:21p cnovak
* CSP555064: Code review. Change element names.
* 
* CSP555064/1   7/27/12 3:32p cnovak
* CSP555064: Bypass 7346 LNA gain. Allow override control of daisy gain.
* 
* 9   3/21/12 4:08p mphillip
* SW7344-263: Convert uint8_t to unsigned for extensibility
* 
* 8   3/12/12 9:56a mphillip
* SW7344-263: Add baseband agc value
* 
* 7   2/2/12 2:39p mphillip
* SW7346-639: Header clarification
* 
* 6   11/18/11 5:32p mphillip
* SW7346-554: Update LNA in/out settings comments for clarity.
* 
* 5   9/14/11 4:34p mphillip
* SW7344-198: Temporary workaround
* 
* 4   9/2/11 5:29p mphillip
* SW7346-301: Add 3445 LNA support for 7346
* 
* 3   7/25/11 5:26p erickson
* SW7346-117: allow NEXUS_7346_MAX_FRONTEND_CHANNELS redefines
* 
* 2   7/25/11 4:49p erickson
* SW7346-117: have 7344 same ast/g3 path as 7346
* 
* 1   6/23/11 5:36p erickson
* SW7346-165: add 7346
* 
***************************************************************************/
#ifndef NEXUS_FRONTEND_7346_H__
#define NEXUS_FRONTEND_7346_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_frontend.h"
#include "nexus_i2c.h"
#include "nexus_frontend_3445.h"

#ifndef NEXUS_7346_MAX_FRONTEND_CHANNELS
#define NEXUS_7346_MAX_FRONTEND_CHANNELS 2
#endif

/******************************************************************************
Summary:
   Specifies the configuration of an output channel on the attached LNA

Description:
   Specifies the configuration of an output channel on the attached LNA
******************************************************************************/
typedef enum NEXUS_7346LnaInput
{
    NEXUS_7346LnaInput_eNone = 0,    /* output is powered off */
    NEXUS_7346LnaInput_eIn0,         /* output comes from IN0. Mapped to BAST_TunerLnaOutputConfig_eIn0 */
    NEXUS_7346LnaInput_eIn1,         /* output comes from IN1. Mapped to BAST_TunerLnaOutputConfig_eIn1 */
    NEXUS_7346LnaInput_eDaisy,       /* output comes from daisy */
    NEXUS_7346LnaInput_eMax
} NEXUS_7346LnaInput;

/******************************************************************************
Summary:
    LNA output configuration.

Description:
    This structure specifies the connection of LNA inputs to outputs.

For example, this configuration:
      +----------+
IN0 --+--\       +-- OUT0
      |   \      +-- OUT1
IN1 --+    \-----+-- DAISY
      +----------+

would be represented by:
        NEXUS_7346LnaSettings lnaSettings;
        lnaSettings.out0 = NEXUS_7346LnaInput_eNone;
        lnaSettings.out1 = NEXUS_7346LnaInput_eNone;
        lnaSettings.daisy = NEXUS_7346LnaInput_eIn0;

    If the input for a given output is set to NEXUS_7346LnaInput_eNone, then this LNA
will not be used.

    NEXUS_7346LnaSettings controls the internal LNA on a 7346.
******************************************************************************/
typedef struct NEXUS_7346LnaSettings
{
   NEXUS_7346LnaInput out0;  /* Configure the input linked to OUT0. Set this to eNone to not use this LNA. */
   NEXUS_7346LnaInput out1;  /* Configure the input linked to OUT1. Set this to eNone to not use this LNA. */
   NEXUS_7346LnaInput daisy; /* Configure the input linked to DAISY. Set this to eNone to not use this LNA. */
} NEXUS_7346LnaSettings;

/******************************************************************************
Summary:
   Specifies an output channel on the attached LNA
******************************************************************************/
typedef enum NEXUS_7346LnaOutput
{
    NEXUS_7346LnaOutput_eNone = 0, /* No output */
    NEXUS_7346LnaOutput_eOut0,     /* LNA primary output */
    NEXUS_7346LnaOutput_eOut1,     /* LNA secondary output */
    NEXUS_7346LnaOutput_eDaisy,    /* LNA daisy output */
    NEXUS_7346LnaOutput_eMax
} NEXUS_7346LnaOutput;

/***************************************************************************
Summary:
    Settings for a BCM7346 Device
 ***************************************************************************/
typedef struct NEXUS_7346FrontendSettings
{
    /* The channel of the device */
    unsigned channelNumber;              /* Which channel from the device will be opened */
    
    bool isInternalLna;                  /* If true, 7346 uses the internal LNA too. */
    NEXUS_7346LnaSettings lnaSettings;   /* Internal LNA settings. */

    struct {
        bool enabled;                    /* If true, demon is connected and uses the external 3445 LNA */
        NEXUS_3445LnaSettings settings;  /* 3445 settings */
        unsigned i2cChannelNumber;       /* i2c bus for 3445 */
        NEXUS_3445LnaOutput lnaOutput;   /* What LNA output feeds this channel. Set to eNone for external LNA. */
    } external3445Lna;

    bool lnbPowerUpPinSelect; /* if true, the LNBPU signal will be routed to the TXEN pin */
} NEXUS_7346FrontendSettings;

/***************************************************************************
Summary:
    Get the default settings for a BCM7346 frontend

Description:
See Also:
    NEXUS_Frontend_Open7346
 ***************************************************************************/
void NEXUS_Frontend_GetDefault7346Settings(
    NEXUS_7346FrontendSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a handle to a BCM7346 device.

Description:
See Also:
    NEXUS_Frontend_Close7346
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open7346(  /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_7346FrontendSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the settings of an LNA attached to the 7346 frontend
 ***************************************************************************/
void NEXUS_Frontend_Get7346LnaSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_7346LnaSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Set the settings of an LNA attached to the 7346 frontend

Description:
Do not call this for an external LNA.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Set7346LnaSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_7346LnaSettings *pSettings
    );

/***************************************************************************
Summary:
    Get an I2C Handle for the 7346 frontend I2C controller.

Description:
    The BCM7346 chips have an on-board I2C master interface per frontend
    channel.  This function allows you to get a handle to this interface
    for use with other I2C devices such as tuners or DiSEQc controllers.
    This interface implements the Read, ReadNoAddr and Write, and WriteNoAddr
    routines.

See Also:
    NEXUS_I2c_Open
    NEXUS_I2c_Read
    NEXUS_I2c_Write
    NEXUS_I2c_ReadNoAddr
    NEXUS_I2c_WriteNoAddr
 ***************************************************************************/
NEXUS_I2cHandle NEXUS_Frontend_Get7346MasterI2c(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Status for a BCM7346 LNA Device
 ***************************************************************************/
typedef struct NEXUS_7346LnaStatus
{
    NEXUS_7346LnaInput  lnaInput;
    unsigned            status;
    unsigned            agc;         /* LNA AGC value, typically used in input power calculations */
    unsigned            baseBandAgc; /* Baseband AGC value, typically used in input power calculations */
} NEXUS_7346LnaStatus;

/***************************************************************************
Summary:
Get the status of an LNA attached to the 7346 frontend

Description:
Do not call this for an external LNA.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Get7346LnaStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_7346LnaStatus *pStatus    /* [out] */
    );

/***************************************************************************
Summary:
Additional 7346 settings used for tuning

Description:
These settings are 7346 specific. Instead of adding them to the generic NEXUS_FrontendSatelliteSettings,
they are added in this chip-specific settings structure.
***************************************************************************/
typedef struct NEXUS_Frontend7346TuneSettings
{
    bool disableFecReacquire;
    bool bypassLnaGain; /* Bypass the internal LNA Gain stage */
    struct  {
        bool override;  /* Override the value of Daisy Gain */
        unsigned value; /* Daisy Gain value to be used */
    } daisyGain; 
} NEXUS_Frontend7346TuneSettings;

/***************************************************************************
Summary:
Get current 7346 settings using for tuning
***************************************************************************/
void NEXUS_Frontend_Get7346TuneSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_Frontend7346TuneSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Set new 7346 settings used for tuning, usually set before calling NEXUS_Frontend_TuneSatellite
***************************************************************************/
NEXUS_Error NEXUS_Frontend_Set7346TuneSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_Frontend7346TuneSettings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_7346_H__ */

