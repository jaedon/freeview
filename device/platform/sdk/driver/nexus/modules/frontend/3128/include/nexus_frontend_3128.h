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
* $brcm_Workfile: nexus_frontend_3128.h $
* $brcm_Revision: 16 $
* $brcm_Date: 11/9/12 1:52p $
*
* API Description:
*   API name: Frontend 3128
*    APIs to open, close, and setup initial settings for a BCM3128
*    Cable Tuner/Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/3128/include/nexus_frontend_3128.h $
* 
* 16   11/9/12 1:52p erickson
* SW3461-206: rework for device api
* 
* 15   11/5/12 9:28a erickson
* SW7425-4119: remove NEXUS_3128Settings.outOfBand.enableFec
* 
* 14   5/17/12 12:39p erickson
* SW3128-164: merge
* 
* WideBandAtoD/1   5/16/12 2:30p vishk
* SW3128-164 : Add selection to choose the A to D converter for the Out
*  of Band RF Input.
* 
* 13   4/12/12 12:04p erickson
* SW7425-2512: convert macro to comment
* 
* 12   4/11/12 1:51p erickson
* SW7425-2512: add new 3255 LNA values and clarify assignment to 3128
* 
* 11   4/9/12 12:17p jtna
* SW3128-131: added NEXUS_FrontendOutOfBandOutputMode to
*  NEXUS_3128ConfigSettings
* 
* 10   2/24/12 2:10p erickson
* SW3128-110, SW3128-120: add nyquist filter selection
* 
* 9   12/16/11 1:49p erickson
* SW3128-92: add NEXUS_Frontend_Probe3128
* 
* 8   11/11/11 3:10p erickson
* SW3128-64: replace NEXUS_3461ConfigSettings.enableDaisyChain with
*  .enableRfLoopThrough
* 
* 7   10/6/11 5:11p mphillip
* SW3128-47: Add support for bcm3123
* 
* 6   5/27/11 3:37p erickson
* SW7422-334: add #include
* 
* 5   5/26/11 3:05p erickson
* SW7422-334: add spi handle and address to NEXUS_3128Settings
* 
* 4   5/24/11 5:11p erickson
* SW7231-32: cleanup comments
* 
* 3   5/11/11 2:34p erickson
* SW3128-10,SW3128-2: Refactoring of
*  NEXUS_Frontend_3128_Get/SetConfigSettings(), Added support to
*  configure TM_MTSIF_CTRL - Transport Control
* 
* 2   3/4/11 4:03p erickson
* SW7422-171: fix void return
*
* 1   3/4/11 3:31p erickson
* SW7422-171: add 3128
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_3128_H__
#define NEXUS_FRONTEND_3128_H__

#include "nexus_frontend.h"
#include "nexus_gpio.h"
#include "nexus_i2c.h"
#include "nexus_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NEXUS_3128_MAX_DOWNSTREAM_CHANNELS
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 8
#endif
#ifndef NEXUS_3128_MAX_OUTOFBAND_CHANNELS
#define NEXUS_3128_MAX_OUTOFBAND_CHANNELS 1
#endif

/***************************************************************************
Summary:
3128 Device Handle
***************************************************************************/
typedef struct NEXUS_3128Device *NEXUS_3128DeviceHandle;

/***************************************************************************
Summary:
Settings for a BCM3128 Device Channel
***************************************************************************/
typedef enum NEXUS_3128ChannelType
{
    NEXUS_3128ChannelType_eInBand,
    NEXUS_3128ChannelType_eOutOfBand,
    NEXUS_3128ChannelType_eUpstream,
    NEXUS_3128ChannelType_eMax
} NEXUS_3128ChannelType;

/***************************************************************************
Summary:
Settings for a BCM3128 Device
***************************************************************************/
typedef struct NEXUS_FrontendDevice3128OpenSettings
{
    /* either GPIO or an L1 is used for notification from the frontend to the host. */
    NEXUS_GpioHandle gpioInterrupt; /* GPIO pin for interrupt. If not NULL, isrNumber is ignored. If NULL, isrNumber is used. */
    unsigned isrNumber;             /* L1 interrupt number. (typically 0..63). See gpioInterrupt for other interrupt option. */

    /* either I2C or SPI is used for the host to control the frontend. */
    NEXUS_I2cHandle i2cDevice;      /* I2C device to use. spiDevice should be NULL to use I2C. */
    uint16_t i2cAddr;               /* master device I2C Address */
    uint8_t i2cSlaveAddr;           /* slave device I2C Address */

    NEXUS_SpiHandle spiDevice;      /* SPI device to use. i2cDevice should be NULL to use SPI. */
    uint16_t spiAddr;               /* master device SPI Address */
    
    bool configureWatchdog;         /* Depending on the platform decide whether to configure watchdog or not. */
    bool inBandOpenDrain;           /* If true, inband is open drain else it is push pull. */
    bool loadAP;                    /* Load Acquisition Processor microcode when starting. */
    bool isMtsif;                   /* Determines if the transport out if configured for legacy mode or MTSIF. 
                                                                   True = MTSIF, False = Legacy mode. */
    struct {
        unsigned ifFrequency;       /* IF frequency used for 3128 OOB module. */
        bool openDrain;             /* If true, out of band is open drain else it is push pull. */
        bool useWidebandAtoD;        /* If true, out of band uses the wideband analog to digital converter. If false, out of band uses its own dedicated analog to digital converter. */
        NEXUS_FrontendOutOfBandNyquistFilter nyquist; /* Nyquist filter used for out of band. */
    }outOfBand;

    struct {
        int bypassable;             /* in units of db. */
        int total;                  /* includes the bypassable and all non-bypassable fixed gains before this device. in units of db. */
    } externalFixedGain;
} NEXUS_FrontendDevice3128OpenSettings;

/***************************************************************************
Summary:
Settings for a BCM3128 Device
***************************************************************************/
typedef struct NEXUS_Frontend3128Settings
{
    NEXUS_FrontendDeviceHandle device;
    NEXUS_3128ChannelType type;     /* Channel type to open from this device */
    unsigned channelNumber;         /* Which channel to open from this device */

    /* the following have been deprecated. see NEXUS_FrontendDevice3128OpenSettings. */
    NEXUS_GpioHandle gpioInterrupt; /* deprecated */
    unsigned isrNumber;             /* deprecated */
    NEXUS_I2cHandle i2cDevice;      /* deprecated */
    uint16_t i2cAddr;               /* deprecated */
    uint8_t i2cSlaveAddr;           /* deprecated */
    NEXUS_SpiHandle spiDevice;      /* deprecated */
    uint16_t spiAddr;               /* deprecated */
    bool configureWatchdog;         /* deprecated */
    bool inBandOpenDrain;           /* deprecated */
    bool loadAP;                    /* deprecated */
    bool isMtsif;                   /* deprecated */
    unsigned ifFrequency;           /* deprecated */
    struct {
        bool openDrain;             /* deprecated */
        bool useWidebandAtoD;        /* deprecated */
        NEXUS_FrontendOutOfBandNyquistFilter nyquist; /* deprecated */
    }outOfBand;
} NEXUS_Frontend3128Settings;

/**
The following defines the packing of NEXUS_3128ConfigSettings.agcValue:

NEXUS_PACKED_AGC_VALUE(LNACHIPID, OUTPUT1_TILT_GAIN, OUTPUT2_TILT_GAIN, GAIN_BOOST_ENABLED, SUPER_BOOST_ENABLED, TILT_ENABLED, AGCVALUE) \
    (((LNACHIPID))<<16 | \
     ((OUTPUT1_TILT_GAIN)&0x3)<<14 | \
     ((OUTPUT2_TILT_GAIN)&0x3)<<12 | \
     ((GAIN_BOOST_ENABLED)?0x1:0x0)<<10 | \
     ((SUPER_BOOST_ENABLED)?0x1:0x0)<<9 | \
     ((TILT_ENABLED)?0x1:0x0)<<8 | \
     ((AGCVALUE)&0x1F) \
    )
    
Field names correspond to NEXUS_3255ChannelAgcConfig naming.
**/
    
/***************************************************************************
Summary:
Config settings for a BCM3128 Device
***************************************************************************/
typedef struct NEXUS_FrontendDevice3128Settings
{
    uint32_t agcValue;                     /* Gain Value: packed value which can be built using the NEXUS_PACKED_AGC_VALUE() macro */
    NEXUS_CallbackDesc updateGainCallback; /* Callback will be called when the gain from the lna needs to be updated */
    bool enableRfLoopThrough;              /* True = Enables RF loop through */
    struct {
        NEXUS_FrontendOutOfBandOutputMode outputMode;
    } outOfBand;
} NEXUS_FrontendDevice3128Settings;

/***************************************************************************
Summary:
Get the default settings for a BCM3128 tuner

Description:

See Also:
NEXUS_Frontend_Open3128
***************************************************************************/
void NEXUS_Frontend_GetDefault3128Settings(
    NEXUS_Frontend3128Settings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
Open a handle to a BCM3128 device.

Description:
Close with NEXUS_Frontend_Close
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open3128( /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_Frontend3128Settings *pSettings
    );

/***************************************************************************
Summary:
Get the default config settings to a BCM3128 device.
***************************************************************************/
void NEXUS_FrontendDevice_GetDefault3128Settings(
    NEXUS_FrontendDevice3128Settings *pConfigSettings   /* [out] */
    );

/***************************************************************************
Summary:
Get the config settings to a BCM3128 device.
***************************************************************************/
NEXUS_Error NEXUS_FrontendDevice_Get3128Settings(
    NEXUS_FrontendDeviceHandle handle,
    NEXUS_FrontendDevice3128Settings *pConfigSettings    /* [out]  */
    );

/***************************************************************************
Summary:
Set the config settings to a BCM3128 device.
***************************************************************************/
NEXUS_Error NEXUS_FrontendDevice_Set3128Settings(
    NEXUS_FrontendDeviceHandle handle,
    const NEXUS_FrontendDevice3128Settings *pConfigSettings
    );

/***************************************************************************
Summary:
***************************************************************************/
void NEXUS_FrontendDevice_GetDefault3128OpenSettings(
    NEXUS_FrontendDevice3128OpenSettings *pSettings
    );

/***************************************************************************
Summary:
***************************************************************************/
NEXUS_FrontendDeviceHandle NEXUS_FrontendDevice_Open3128(
    unsigned index,
    const NEXUS_FrontendDevice3128OpenSettings *pSettings
    );

/* deprecated */
#define NEXUS_Frontend_GetDefault3128ConfigSettings NEXUS_FrontendDevice_GetDefault3128Settings
#define NEXUS_Frontend_3128_GetConfigSettings(handle,pSettings) NEXUS_FrontendDevice_Get3128Settings(NEXUS_Frontend_GetDevice(handle), pSettings)
#define NEXUS_Frontend_3128_SetConfigSettings(handle,pSettings) NEXUS_FrontendDevice_Set3128Settings(NEXUS_Frontend_GetDevice(handle), pSettings)
typedef NEXUS_Frontend3128Settings NEXUS_3128Settings; 
typedef NEXUS_FrontendDevice3128Settings NEXUS_3128ConfigSettings; 

/***************************************************************************
Summary:
    Results of BCM3128 device discovery
 ***************************************************************************/
typedef struct NEXUS_3128ProbeResults
{
    NEXUS_FrontendChipType chip;
} NEXUS_3128ProbeResults;

/***************************************************************************
Summary:
  Probe to see if a BCM3128 device exists with the specified settings

Description:
  Probe to see if a BCM3128 device exists with the specified settings

See Also:
    NEXUS_Frontend_Open3128
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Probe3128(
    const NEXUS_FrontendDevice3128OpenSettings *pSettings,
    NEXUS_3128ProbeResults *pResults    /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_3128_H__ */


