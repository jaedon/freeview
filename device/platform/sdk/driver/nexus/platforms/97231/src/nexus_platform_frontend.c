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
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97231/src/nexus_platform_frontend.c $
 * 
 * 46   11/9/12 3:46p vishk
 * SW3461-206: Add support for FBTSFF platform.
 * 
 * 45   10/25/12 6:10p vishk
 * SW3461-206: Remove warning messages.
 * 
 * 44   10/25/12 2:18p vishk
 * SW3461-206: Add description for external fixed gain parameters.
 * 
 * 43   10/25/12 12:42p vishk
 * SW3461-206: Add external fixed gain settings.
 * 
 * 42   10/19/12 4:28p vishk
 * SW3461-270: Add API Support for Reading New Front-End 312X, 314X, 346X
 *  F/W Version IDs
 * 
 * 41   10/19/12 4:19p vishk
 * SW3461-206: Add front end configuration settings for external, fixed-
 *  gain amp (FGA) and optional FGA-bypass
 * 
 * 40   8/31/12 10:01a erickson
 * SW7435-328: convert config.spi[] to NEXUS_Spi_Open
 * 
 * 39   7/26/12 4:01p gmohile
 * SW7425-3519,SW7425-3520,SW7425-3077: Add frontend standby
 * 
 * 38   7/17/12 1:27p erickson
 * SW7231-895: fix warning
 * 
 * 37   7/16/12 3:55p vishk
 * SW7231-895: Add support for 97231B2 EUSFF_V20 board.
 * 
 * 36   6/11/12 1:19p vishk
 * SW3461-215: Enable the suggested fix to all frontends.
 * 
 * 35   6/8/12 7:33p vishk
 * SW3461-215: Add HIF_OSC_STRAP_OVRD_XCORE_BIAS = 0x1F after power up.
 * 
 * 33   5/31/12 5:14p vishk
 * SW7231-828: Merge to mainline.
 * 
 * 32   5/22/12 4:58p randyjew
 * SW7231-317: Fix prototype warnings
 * 
 * 32   5/22/12 4:56p randyjew
 * SW7231-317: Fix prototype warnings
 * 
 * 31   5/14/12 2:37p jtna
 * SW7425-2752: set NEXUS_FrontendUserParams.isMtsif and .param1 to work
 *  correctly with new MTSIF mapping
 * 
 * 30   3/1/12 6:25p vishk
 * SW7231-68: Bug fix to enable opening second demod when using I2C.
 * 
 * 29   3/1/12 5:58p vishk
 * SW7231-68: Renaming the define used to specify SPI usage vs I2C.
 * 
 * 28   2/27/12 6:27p vishk
 * SW7231-68: Disable the second frontend for SPI mode.
 * 
 * 27   2/24/12 6:10p vishk
 * SW7231-68: add SPI support on top of I2C for 3461 interface
 * 
 * 26   2/15/12 8:19p katrep
 * SW7231-317:run time error
 * 
 * 25   2/14/12 12:33p vishk
 * SW7231-649: 31xx failure during tune_qam.  Set MTSIF option correctly
 *  for differnt chip versions.
 * 
 * 24   11/1/11 3:10p vishk
 * SW3461-55: Means of returning BCM3461 chip revsion and firmware
 *  revision via NEXUS i/f
 * 
 * 23   10/12/11 7:35p vishk
 * SW3128-16: Add scan support to 3128
 * 
 * 22   9/9/11 8:16p vishk
 * SW3461-35: RF loopthrough control not exposed via API (Magnum+Nexus)
 * 
 * 21   7/12/11 12:29p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 *  Bug fix.
 * 
 * 20   6/8/11 9:54a katrep
 * SW7231-32:add support 3461 card probing
 * 
 * 19   6/5/11 9:02p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 *  Add support for the second frontend. Implement chip specific function
 *  calls in platform open just once.
 * 
 * 18   6/2/11 9:57p vishk
 * SW3461-8: To open one frontend handle per tuner/demod instance.
 *  Diabling the second tuner for now.
 * 
 * 17   6/2/11 9:33p vishk
 * SW3461-5: Add THD support to 3461
 * 
 * 16   5/27/11 9:43p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 *  Implement brutus support for the downstream core.
 * 
 * 15   5/27/11 2:11p vishk
 * SW7231-32: SW7231-180:Implement the Nexus code to support the 3461
 *  frontend chip.
 * 
 * 14   5/24/11 7:14p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip
 * 
 * 13   5/19/11 9:55p vishk
 * SW7231-76: Add support for FBTSFF 97231 + 3128 board.
 * 
 * 12   5/11/11 3:18p vishk
 * SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control.
 * 
 * 11   5/2/11 2:03p katrep
 * SW7231-32:add more debug
 * 
 * 10   3/21/11 5:02p katrep
 * SW7231-32:fixed the build issue for default platform 97231CBSV board
 * 
 * 9   3/17/11 1:12p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip
 * 
 * 8   3/16/11 3:17p vishk
 * SW7231-76 : Add support for FBTSFF 97231 + 3128 board.
 * 
 * 7   3/16/11 1:31p vishk
 * SW7231-76 : Add support for FBTSFF 97231 + 3128 board.
 * 
 * 6   3/11/11 4:52p katrep
 * SW7231-47:fix support for 97230
 * 
 * 5   2/25/11 1:59p vishk
 * SW7231-30: Add support for 3109,3406 tuners on 97231CSFF board
 * 
 * 4   2/25/11 1:55p vishk
 * SW7231-30: Add support for 3109,3406 tuners on 97231CSFF board
 * 
 * 3   1/28/11 3:15p mward
 * SW7125-769:  Warning for unused. Also stub
 *  NEXUS_Platform_OpenFrontend() when !NEXUS_HAS_FRONTEND.
 * 
 * 2   1/27/11 3:01p mward
 * SW7125-769:  Stub NEXUS_Platform_OpenFrontend().
 * 
 * 1   9/27/10 6:05p nickh
 * SW7422-10: Add initial version of the frontend support
 * 
* API Description:
*   API name: Platform Frontend
*    Platform Frontend Setup
*
* Revision History:
*
***************************************************************************/
#if NEXUS_PLATFORM == 97231

#include "nexus_types.h"
#include "nexus_platform.h"
#include "priv/nexus_core.h"
#include "nexus_frontend.h"
#include "nexus_platform_features.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "nexus_input_band.h"
#include "bchp_gio.h"
#if NEXUS_PLATFORM_7231_3520
#include "nexus_frontend_3520.h"
#elif NEXUS_PLATFORM_7231_FBTSFF
#include "nexus_frontend_3128.h"
#elif NEXUS_PLATFORM_7231_CSFF
#include "nexus_frontend_31xx.h"
#else /* NEXUS_PLATFORM_7231_EUSFF && CVBS boards */
#include "nexus_frontend_3461.h"
#endif

BDBG_MODULE(nexus_platform_frontend);

NEXUS_GpioHandle tunerGpio[NEXUS_MAX_FRONTENDS];

#if NEXUS_PLATFORM_7231_3520
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_3520Settings settings3520;
    NEXUS_FrontendUserParameters userParams;

    /* Open on-board 3517A */
    NEXUS_Frontend_GetDefault3520Settings(&settings3520);
    settings3520.i2cDevice = pConfig->i2c[2];
    if (!settings3520.i2cDevice) {
        BDBG_ERR(("I2C[2]/BSC_M2 channel unavailable for 3520"));
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;
    }
    settings3520.i2cAddr = 0x0c;
    settings3520.isrNumber = 10;

    pConfig->frontend[0] = NEXUS_Frontend_Open3520(&settings3520);
    if (NULL == pConfig->frontend[0])
    {
        BDBG_ERR(("Unable to open onboard 3520 demodulator "));
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[0], &userParams);
    userParams.param1 = NEXUS_InputBand_e0;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[0], &userParams);

    /* Open on-board 3517B */
    settings3520.i2cAddr = 0x0d;
    settings3520.isrNumber = 11;

    pConfig->frontend[1] = NEXUS_Frontend_Open3520(&settings3520);
    if (NULL == pConfig->frontend[1])
    {
        BDBG_ERR(("Unable to open onboard 3520 demodulator "));
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;        
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[1], &userParams);
    userParams.param1 = NEXUS_InputBand_e1;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[1], &userParams);
done:
    return rc;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i]) {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }

    return;
}

#elif NEXUS_PLATFORM_7231_EUSFF || NEXUS_PLATFORM_7231_EUSFF_V20
static NEXUS_SpiHandle g_3461spi;
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_3461Settings st3461Settings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDeviceHandle parentDevice;
    NEXUS_FrontendDevice3461OpenSettings deviceOpenSettings;
    NEXUS_FrontendDeviceLinkSettings linkSettings;
    NEXUS_FrontendDevice3461Settings deviceSettings;
    NEXUS_FrontendType type;

    BDBG_WRN(("Waiting for 3461 Downstream frontend(7231_EUSFF) to initialize"));

#if NEXUS_PLATFORM_97231_3461_USES_SPI
    {
        NEXUS_SpiSettings spiSettings;
        NEXUS_Spi_GetDefaultSettings(&spiSettings);
        spiSettings.clockActiveLow = true;
        g_3461spi = NEXUS_Spi_Open(0, &spiSettings);
        if (!g_3461spi) {
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
    }
    deviceOpenSettings.spiDevice = g_3461spi;
    deviceOpenSettings.spiAddr = 0x40;
#else
    if (!pConfig->i2c[1]) {
            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
            return BERR_NOT_INITIALIZED;
    }
    deviceOpenSettings.i2cDevice = pConfig->i2c[1];    /* Onboard tuner/demod use BSC_M1.*/
    deviceOpenSettings.i2cAddr = 0x6c;
#endif
#if NEXUS_PLATFORM_7231_EUSFF
    deviceOpenSettings.isrNumber = 10;
#elif NEXUS_PLATFORM_7231_EUSFF_V20
    deviceOpenSettings.isrNumber = 8;
#endif
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.externalFixedGain.total = 7;       /* These are platform specific values given by the board designer. */
    deviceOpenSettings.externalFixedGain.bypassable = 14; /* These are platform specific values given by the board designer. */

    parentDevice = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(parentDevice == NULL){rc = BERR_TRACE(rc); goto done;}
    
    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eInternalLna;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = true;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(parentDevice, &deviceSettings);

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    st3461Settings.device = parentDevice;
    st3461Settings.type = NEXUS_3461ChannelType_eDvbt; /*REDUNDANT for now as there is only one instance of any demod running. */
    st3461Settings.channelNumber = 0;                    /*REDUNDANT for now. */

    pConfig->frontend[0] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[0])
    {
        BDBG_ERR(("Unable to open onboard 3461 dvbt2 tuner/demodulator "));
    }

    NEXUS_Frontend_GetType(pConfig->frontend[0], &type);
    BDBG_MSG(("familyId = 0x%x", type.chip.familyId));
    BDBG_MSG(("chipId = 0x%x", type.chip.id));
    BDBG_MSG(("version.major = 0x%x", type.chip.version.major ));
    BDBG_MSG(("version.major = 0x%x", type.chip.version.minor ));
    BDBG_MSG(("version.buildType = 0x%x", type.chip.version.buildType ));
    BDBG_MSG(("version.buildId = 0x%x", type.chip.version.buildId ));
    BDBG_MSG(("bondoutOptions[0] = 0x%x", type.chip.bondoutOptions[0] ));
    BDBG_MSG(("bondoutOptions[1] = 0x%x", type.chip.bondoutOptions[1] ));
    
    BDBG_MSG(("firmwareVersion.major = 0x%x", type.firmwareVersion.major ));
    BDBG_MSG(("firmwareVersion.major = 0x%x", type.firmwareVersion.minor ));
    BDBG_MSG(("firmwareVersion.buildType = 0x%x", type.firmwareVersion.buildType ));
    BDBG_MSG(("firmwareVersion.buildId = 0x%x", type.firmwareVersion.buildId ));

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[0], &userParams);
    userParams.param1 = NEXUS_InputBand_e0;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[0], &userParams);
 
#if NEXUS_PLATFORM_97231_3461_USES_SPI
#else
    BDBG_WRN(("Waiting for the second 3461 Downstream frontend(7231_EUSFF) to initialize"));
    st3461Settings.device = NULL;

#if NEXUS_PLATFORM_7231_EUSFF_V20
    deviceOpenSettings.isrNumber = 9;
#endif  
    deviceOpenSettings.i2cAddr = 0x6d;

    st3461Settings.device = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(st3461Settings.device == NULL){rc = BERR_TRACE(rc); goto done;}

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eOff;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eExternalLna;
    deviceSettings.enableRfLoopThrough = false;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(st3461Settings.device, &deviceSettings);

    st3461Settings.type = NEXUS_3461ChannelType_eInBand;
    pConfig->frontend[1] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[1])
    {
        BDBG_ERR(("Unable to open onboard 3461 dvbt2 tuner/demodulator "));
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[1], &userParams);
    userParams.param1 = NEXUS_InputBand_e1;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[1], &userParams);

    NEXUS_FrontendDevice_GetDefaultLinkSettings(&linkSettings);

    rc = NEXUS_FrontendDevice_Link(parentDevice, st3461Settings.device, &linkSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}
#endif

done:
    return rc;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i]) {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }
    
    if (g_3461spi) {
        NEXUS_Spi_Close(g_3461spi);
        g_3461spi = NULL;
    }

    return;
}

#elif NEXUS_PLATFORM_7231_CSFF

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;
    NEXUS_31xxSettings st31xxSettings;
    NEXUS_FrontendUserParameters userParams;
    
    for (i=0; i < NEXUS_MAX_FRONTENDS; i++)
    {
        BDBG_WRN(("Waiting for frontend(7231_CSFF) %d to initialize", i));

        NEXUS_Frontend_GetDefault31xxSettings(&st31xxSettings);
        st31xxSettings.isrNumber = 9 + i;
        st31xxSettings.i2cDevice = pConfig->i2c[1];   /* Onboard tuner/demod use BSC_M1.*/
        st31xxSettings.i2cAddr = 0x66 + i;
        st31xxSettings.type = NEXUS_31xxChannelType_eInBand;
        st31xxSettings.channelNumber = 0;
        st31xxSettings.ifFrequency = 0;
        st31xxSettings.inBandOpenDrain=true;
        st31xxSettings.loadAP = true;
        st31xxSettings.configureWatchdog = false;

        pConfig->frontend[i] = NEXUS_Frontend_Open31xx(&st31xxSettings);    
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3109 tuner/demodulator %d", i));
            continue;
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }

    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;


    unsigned frontendCnt = NEXUS_MAX_FRONTENDS;

    if (NEXUS_GetEnv("disable_oob_frontend") && !BKNI_Memcmp(NEXUS_GetEnv("disable_oob_frontend"), "y", 1))
        frontendCnt = NEXUS_MAX_FRONTENDS-1;
    else
        frontendCnt = NEXUS_MAX_FRONTENDS;
    
    for (i=0; i<frontendCnt; i++)
    {
        if (pConfig->frontend[i])
             NEXUS_Frontend_Close(pConfig->frontend[i]);

    pConfig->frontend[i] = NULL;
    }

    return;
}

#elif NEXUS_PLATFORM_7231_FBTSFF

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;
    NEXUS_Frontend3128Settings st3128Settings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_3128ProbeResults results;
    NEXUS_FrontendDevice3128OpenSettings st3128DeviceOpenSettings;


    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);

    NEXUS_FrontendDevice_GetDefault3128OpenSettings(&st3128DeviceOpenSettings);
    st3128DeviceOpenSettings.isrNumber = 9;    
    st3128DeviceOpenSettings.i2cDevice = pConfig->i2c[1];    /* Onboard tuner/demod use BSC_M1.*/    
    st3128DeviceOpenSettings.i2cAddr = 0x6c ;
    st3128DeviceOpenSettings.outOfBand.ifFrequency = 0;     
    st3128DeviceOpenSettings.inBandOpenDrain=true;
    st3128DeviceOpenSettings.loadAP = true;
    st3128DeviceOpenSettings.configureWatchdog = false;
#if (BCHP_VER >= BCHP_VER_B0)   
    st3128DeviceOpenSettings.isMtsif = true;
#else
    st3128DeviceOpenSettings.isMtsif = false;
#endif
    st3128Settings.device = NEXUS_FrontendDevice_Open3128(0, &st3128DeviceOpenSettings);

	NEXUS_Frontend_Probe3128(&st3128DeviceOpenSettings, &results);
	BDBG_MSG(("chip.familyId = 0x%x", results.chip.familyId));
	BDBG_MSG(("chip.id = 0x%x", results.chip.id));
	BDBG_MSG(("version.major = 0x%x", results.chip.version.major ));
	BDBG_MSG(("version.minor = 0x%x", results.chip.version.minor ));

    st3128Settings.type = NEXUS_3128ChannelType_eInBand;
    /* Open downstream tuners */
    for (i=0; i < (results.chip.id & 0xF); i++)
    {
        BDBG_WRN(("Waiting for frontend(7231_FBTSFF) %d to initialize", i));
        
        st3128Settings.channelNumber = i;

        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator %d", i));
            continue;
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.isMtsif = st3128DeviceOpenSettings.isMtsif;
        userParams.param1 = st3128DeviceOpenSettings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }

    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i])
             NEXUS_Frontend_Close(pConfig->frontend[i]);
    
    pConfig->frontend[i] = NULL;
    }

    return;
}

#elif NEXUS_FRONTEND_3461
#include "nexus_frontend_3461.h"

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_3461Settings st3462Settings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDeviceHandle parentDevice;
    NEXUS_FrontendDevice3461OpenSettings deviceOpenSettings;
    NEXUS_FrontendDeviceLinkSettings linkSettings;
    NEXUS_FrontendDevice3461Settings deviceSettings;
#if 0
    NEXUS_GpioSettings stGpioCh0Setting, stGpioCh1Setting;
    NEXUS_GpioHandle hGpioResetCh0, hGpioResetCh1;

	BDBG_WRN(("!!! Reset channel GOIO before NEXUS_Platform_InitFrontend"));

	/* BCM3461 Reset GPIO 91 Setting */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &stGpioCh0Setting);
    stGpioCh0Setting.mode = NEXUS_GpioMode_eOutputPushPull;
	stGpioCh0Setting.value = NEXUS_GpioValue_eHigh;/* NEXUS_GpioValue_eLow; */
    stGpioCh0Setting.interruptMode = NEXUS_GpioInterrupt_eDisabled;

	/* BCM3461 Reset GPIO 105 Setting */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &stGpioCh1Setting);
    stGpioCh1Setting.mode = NEXUS_GpioMode_eOutputPushPull;
	stGpioCh1Setting.value = NEXUS_GpioValue_eHigh;/* NEXUS_GpioValue_eLow; */
    stGpioCh1Setting.interruptMode = NEXUS_GpioInterrupt_eDisabled;

	/* Master channel reset GPIO 91 */
	hGpioResetCh0 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 91, &stGpioCh0Setting);
    if (NULL == hGpioResetCh0)
    {
        BDBG_ERR(("Unable to open Reset GPIO for Master Channel."));
    }

	/* Slave channel reset GPIO 105 */
	hGpioResetCh1 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 105, &stGpioCh1Setting);
    if (NULL == hGpioResetCh0)
    {
        BDBG_ERR(("Unable to open Reset GPIO for Master Channel."));
    }

	NEXUS_Gpio_GetSettings(hGpioResetCh0,&stGpioCh0Setting);
	stGpioCh0Setting.value = NEXUS_GpioValue_eLow;
	NEXUS_Gpio_SetSettings(hGpioResetCh0,&stGpioCh0Setting);

	NEXUS_Gpio_GetSettings(hGpioResetCh1,&stGpioCh1Setting);
	stGpioCh1Setting.value = NEXUS_GpioValue_eLow;
	NEXUS_Gpio_SetSettings(hGpioResetCh1,&stGpioCh1Setting);

	BKNI_Sleep(30);/*at least 25ms */

	stGpioCh0Setting.value = NEXUS_GpioValue_eHigh;
	NEXUS_Gpio_SetSettings(hGpioResetCh0,&stGpioCh0Setting);

	stGpioCh1Setting.value = NEXUS_GpioValue_eHigh;
	NEXUS_Gpio_SetSettings(hGpioResetCh1,&stGpioCh1Setting);
#endif
    BDBG_WRN(("Waiting for the first 3462 Downstream frontend(7231_3461) to initialize"));

    NEXUS_Frontend_GetDefault3461Settings(&st3462Settings);
    if (!pConfig->i2c[1]) {
            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
            return BERR_NOT_INITIALIZED;
    }
    deviceOpenSettings.i2cDevice = pConfig->i2c[3];    /* Onboard tuner/demod use BSC_M3.*/
    deviceOpenSettings.i2cAddr = 0x6c;
#if defined (CONFIG_DEVICE_MODE_USER)
	deviceOpenSettings.isrNumber = 10;
#else /* CONFIG_DEVICE_MODE_KERNEL */
	deviceOpenSettings.isrNumber = 11;
#endif
    deviceOpenSettings.loadAP = true;

    parentDevice = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eInternalLna;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = true;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(parentDevice, &deviceSettings);

    NEXUS_Frontend_GetDefault3461Settings(&st3462Settings);
    st3462Settings.device = parentDevice;
    st3462Settings.type = NEXUS_3461ChannelType_eDvbt; /*REDUNDANT for now as there is only one instance of any demod running. */
    st3462Settings.channelNumber = 0;                   /*REDUNDANT for now. */

    pConfig->frontend[0] = NEXUS_Frontend_Open3461(&st3462Settings);
    if (NULL == pConfig->frontend[0])
    {
        BDBG_ERR(("Unable to open onboard 3462 tuner/demodulator "));
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[0], &userParams);
    userParams.param1 = NEXUS_InputBand_e0;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[0], &userParams);

    BDBG_WRN(("Waiting for the second 3462 Downstream frontend(7231_3461) to initialize"));

    deviceOpenSettings.i2cDevice = pConfig->i2c[2];    /* Onboard tuner/demod use BSC_M2.*/
    deviceOpenSettings.i2cAddr = 0x6e;
#if defined (CONFIG_DEVICE_MODE_USER)
	deviceOpenSettings.isrNumber = 10;
#else /* CONFIG_DEVICE_MODE_KERNEL */
	deviceOpenSettings.isrNumber = 11;
#endif

    st3462Settings.device = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eOff;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eExternalLna;
    deviceSettings.enableRfLoopThrough = false;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(st3462Settings.device, &deviceSettings);

    pConfig->frontend[1] = NEXUS_Frontend_Open3461(&st3462Settings);
    if (NULL == pConfig->frontend[1])
    {
        BDBG_ERR(("Unable to open onboard 3462 tuner/demodulator "));
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[1], &userParams);
    userParams.param1 = NEXUS_InputBand_e1;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[1], &userParams);

    BDBG_ERR(("child handle = 0x%x, parent handle = 0x%x", st3462Settings.device, parentDevice));

    NEXUS_FrontendDevice_GetDefaultLinkSettings(&linkSettings);

    rc = NEXUS_FrontendDevice_Link(parentDevice, st3462Settings.device, &linkSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}
done:
    return rc;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i])
        {
            BDBG_MSG(("NEXUS_Platform_UninitFrontend frontend %u",i));
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }
    return;
}
/* NEXUS_FRONTEND_3461 */

#else /* SILICON VERIFICATION BOARD */

#if NEXUS_NUM_FRONTEND_CARD_SLOTS
static NEXUS_FrontendCardHandle g_frontendCards[NEXUS_NUM_FRONTEND_CARD_SLOTS];
#endif

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    int frontend=0;
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    int card=0;
    unsigned numChannels, i;
    NEXUS_FrontendCardSettings cardSettings;
    NEXUS_FrontendUserParameters userParams;
#endif
    BDBG_WRN(("Waiting for frontend(7231_CBSV) to initialize"));

    if (!pConfig->i2c[1]) {
        BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        return BERR_NOT_INITIALIZED;
    }
    /* Probe Daughercards First */
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    /* Find first empty frontend in config */
    for (frontend=0 ; pConfig->frontend[frontend] && frontend < NEXUS_MAX_FRONTENDS; frontend++ );
    if ( frontend >= NEXUS_MAX_FRONTENDS ) {
        BDBG_ERR(("No front ends a available"));
        return BERR_SUCCESS;
    }

    /* Probe first slot */
    NEXUS_FrontendCard_GetDefaultSettings(&cardSettings);
    cardSettings.i2cDevice = pConfig->i2c[1];               /* First slot uses I2C 1 */
    cardSettings.isrNumber = 10;                            /* Second slot uses EXT IRQ 13 (L1 #50) */
    cardSettings.numChannels = 2;                           /* First slot has 2 channels */
    /* cardSettings does not provide fields like i2cAddr or i2cAddr.
     * Those are obtained while probing e.g. from NEXUS_Frontend_GetDefault3461Settings().
     */

    BDBG_WRN(("Probing slot 0"));
    g_frontendCards[card] = NEXUS_FrontendCard_Open(&cardSettings);
    if ( g_frontendCards[card] ) {
        BDBG_WRN(("Found tuner card in slot 0"));
        NEXUS_FrontendCard_GetNumChannels(g_frontendCards[card], &numChannels);
        for ( i=0; i < numChannels && frontend < NEXUS_MAX_FRONTENDS; frontend++, i++ ) {
            pConfig->frontend[frontend] = NEXUS_FrontendCard_GetChannel(g_frontendCards[card], i);
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[frontend], &userParams);
            userParams.param1 = (i==0)?NEXUS_InputBand_e0:NEXUS_InputBand_e1;
            userParams.pParam2 = NULL;
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[frontend], &userParams);
        }
        card++;
    }
    if ( card >= NEXUS_NUM_FRONTEND_CARD_SLOTS || frontend >= NEXUS_MAX_FRONTENDS ) {
        return BERR_SUCCESS;
    }
    /* MF implement the other slots (2,3) */
    /* MF implement fall back to the on board FE? (3128) */
#endif /* NEXUS_NUM_FRONTEND_CARD_SLOTS */
    return BERR_SUCCESS;
}
    
void NEXUS_Platform_UninitFrontend(void)
{
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    int i;
    for ( i = NEXUS_NUM_FRONTEND_CARD_SLOTS-1; i >= 0; i-- ) {
        if ( g_frontendCards[i] ) {
            NEXUS_FrontendCard_Close(g_frontendCards[i]);
        }
    }
#endif
    return;
}

#endif

BTRC_MODULE(ChnChange_TuneStreamer, ENABLE);

NEXUS_Error
NEXUS_Platform_GetStreamerInputBand(unsigned index, NEXUS_InputBand *pInputBand)
{
    BDBG_ASSERT(pInputBand);
    if (index > 0) {
        BDBG_ERR(("Only 1 streamer input available"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BTRC_TRACE(ChnChange_TuneStreamer, START);
    *pInputBand = NEXUS_InputBand_e5;
    BTRC_TRACE(ChnChange_TuneStreamer, STOP);
    return NEXUS_SUCCESS;
}

NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see 
                   nexus_platform_features.h for possible platform-specific macros.
                */
    )
{
    NEXUS_Error errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    BSTD_UNUSED(errCode);
    BSTD_UNUSED(id);
    return NULL;
}

#elif NEXUS_PLATFORM == 97230

#include "nexus_types.h"
#include "nexus_platform.h"
#include "priv/nexus_core.h"
#include "priv/nexus_frontend_standby_priv.h"
#include "nexus_frontend.h"
#include "nexus_platform_features.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "nexus_input_band.h"
#include "bchp_gio.h"

BDBG_MODULE(nexus_platform_frontend);


NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return 0;
}

void NEXUS_Platform_UninitFrontend(void)
{
}


BTRC_MODULE(ChnChange_TuneStreamer, ENABLE);

NEXUS_Error
NEXUS_Platform_GetStreamerInputBand(unsigned index, NEXUS_InputBand *pInputBand)
{
    BDBG_ASSERT(pInputBand);
    if (index > 0) {
        BDBG_ERR(("Only 1 streamer input available"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BTRC_TRACE(ChnChange_TuneStreamer, START);
    *pInputBand = NEXUS_InputBand_e5;
    BTRC_TRACE(ChnChange_TuneStreamer, STOP);
    return NEXUS_SUCCESS;
}

NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see 
                   nexus_platform_features.h for possible platform-specific macros.
                */
    )
{
    NEXUS_Error errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    BSTD_UNUSED(errCode);
    BSTD_UNUSED(id);
    return NULL;
}
#endif


