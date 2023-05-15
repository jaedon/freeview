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
* $brcm_Workfile: nexus_platform_frontend.c $
* $brcm_Revision: 22 $
* $brcm_Date: 10/22/12 6:29p $
*
* API Description:
*   API name: Platform linuxuser
*    linuxuser OS routines
*
*
* Revision History:
*
* $brcm_Log: X:/win_views/7241/nexus/platforms/97429/src/nexus_platform_frontend.c $
* 
* 22   10/22/12 6:29p vishk
* SW3461-206, SW3461-270: Add API Support for Reading New Front-End 312X,
*  314X, 346X F/W Version IDs Add front end configuration settings for
*  external, fixed-gain amp (FGA) and optional FGA-bypass
* 
* 21   9/21/12 11:12a jowo
* SW7429-306: set the user parameters for the opened OOB channel
* 
* 20   9/5/12 4:31p vishk
* SW3462-12: Fix typo.
* 
* 19   7/5/12 3:42p vishk
* SW3462-12: Use 3461 api for 3462.
* 
* 18   7/2/12 10:28a katrep
* SW7429-183:Cleaned up platform define and build issue with no frontend
* 
* 17   6/26/12 3:56p vishk
* SW7429-183: Add support for 7241DCSFBTSFF board
* 
* 16   6/13/12 3:23p katrep
* SW7429-1:compiler warnings
* 
* 15   5/14/12 4:06p jtna
* SW7425-2752: set NEXUS_FrontendUserParams.isMtsif and .param1 to work
*  correctly with new MTSIF mapping
* 
* 14   5/8/12 6:25p vishk
* SW3462-12: Add support for second 3462 frontend.
* 
* 13   5/8/12 2:13p vishk
* SW3462-12: Enable Ofdm status and populate uninit_frontend api.
* 
* 12   5/8/12 9:15a katrep
* SW3462-12:add conditional compile for 3462
* 
* 11   5/7/12 6:09p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
* 
* 10   3/19/12 9:52p nickh
* SW7429-83: Support legacy OOB
* 
* 9   2/17/12 1:07p katrep
* SW7429-1:inputband4 7241 streamer input
* 
* 8   1/18/12 11:37a ayxiang
* SW7429-44: implement uninit_frontend to shutdown cleanly
* 
* 7   1/17/12 4:33p katrep
* SW7429-44:compile time enable option for 3255
* 
* 6   1/16/12 12:52p ayxiang
* SW7429-44: add frontend support for 7429
* 
* 5   12/19/11 5:27p katrep
* SW7429-1:use steramer 0 for 97429 board
* 
* 4   12/19/11 11:14a katrep
* SW7429-1:make streamer input 0 default for 7429
* 
* 3   11/14/11 8:10p katrep
* SW7429-1:streamer assigment for 97428,97429 & 97241
* 
* 2   10/25/11 5:28p katrep
* SW7429-1:updated for 7429 bringup
* 
* 1   8/9/11 8:01p katrep
* SW7429-1:initial checkin
*
*
***************************************************************************/
#include "nexus_types.h"
#include "nexus_platform.h"
#include "priv/nexus_core.h"
#include "nexus_platform_features.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "nexus_input_band.h"
#include "bchp_gio.h"

BDBG_MODULE(nexus_platform_frontend);

#if PLATFORM == 97429

#ifdef NEXUS_FRONTEND_3255

#include "nexus_frontend_3255.h"
static NEXUS_3255DeviceHandle st3255DeviceHandle;

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0;

    /*defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT)*/
    unsigned st3255Channel;
    NEXUS_3255DeviceSettings st3255DeviceSettings;
    NEXUS_3255ChannelSettings st3255ChannelSettings;
    NEXUS_3255DeviceCapabilities st3255DeviceCapabilities;
    NEXUS_3255ChannelCapabilities st3255ChannelCapabilities;

    NEXUS_FrontendUserParameters userParams;

    NEXUS_Frontend_GetDefault3255DeviceSettings(&st3255DeviceSettings);
    st3255DeviceSettings.rpcTimeout = 50;
    st3255DeviceSettings.mtsif = true;
    st3255DeviceHandle = NEXUS_Frontend_Open3255Device(0,&st3255DeviceSettings);
    NEXUS_Frontend_Get3255DeviceCapabilities(st3255DeviceHandle,&st3255DeviceCapabilities);
    BDBG_ERR(("DOCSIS Capabilities : Total Channels %u QAM Channels %u Docsis Channels %u oob %s",
              st3255DeviceCapabilities.totalChannels,st3255DeviceCapabilities.numOfQamChannels,
              st3255DeviceCapabilities.numOfDocsisChannels,st3255DeviceCapabilities.isOobChannelSupported?"true":"false" ));
    /*
     * Open the DOCSIS Inband channels
     */
    for (i=0,st3255Channel=0;
         (st3255Channel < st3255DeviceCapabilities.totalChannels) && (i < NEXUS_MAX_FRONTENDS-1);
          st3255Channel++)
    {
        NEXUS_Frontend_Get3255ChannelCapabilities(st3255DeviceHandle,st3255Channel,&st3255ChannelCapabilities);
        if(st3255ChannelCapabilities.channelType == NEXUS_3255ChannelType_eInBand)
        {
            BDBG_MSG((" frontend index %u Docsis QAM channel %u",i,st3255Channel));
            NEXUS_Frontend_GetDefault3255ChannelSettings(&st3255ChannelSettings);
            st3255ChannelSettings.channelNumber = st3255Channel;
            pConfig->frontend[i] = NEXUS_Frontend_Open3255Channel(st3255DeviceHandle,&st3255ChannelSettings);
            if ( NULL == (pConfig->frontend[i]) )
            {
                BDBG_ERR(("Unable to open onboard 3255 tuner/demodulator %d",i));
                continue;
            }
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            userParams.isMtsif = st3255DeviceSettings.mtsif;
            userParams.param1 = st3255DeviceSettings.mtsif ? st3255ChannelSettings.channelNumber : NEXUS_InputBand_e0 + st3255Channel;
            userParams.chipId = 0x3255; /* 3255 API for BCM3383*/
            userParams.pParam2 = NULL;
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
            #if 0
            NEXUS_Frontend_GetTransportSettings(pConfig->frontend[i],&transportSettings);
            transportSettings.bandmap.virtualParserBandNum = NEXUS_InputBand_e0 + i;
            transportSettings.bandmap.virtualParserIsPlayback = false;
            NEXUS_Frontend_SetTransportSettings(pConfig->frontend[i],&transportSettings);
            #endif
            i++;
        }
        else
        {
            BDBG_MSG(("Docsis Non-QAM channel %u",st3255Channel));
        }
    }


    /*
        * If OOB channel is present in the Docsis device, check for the channel number
        */
    if(st3255DeviceCapabilities.isOobChannelSupported)
    {
        for(st3255Channel=0;st3255Channel<st3255DeviceCapabilities.totalChannels;st3255Channel++)
        {
            NEXUS_Frontend_Get3255ChannelCapabilities(st3255DeviceHandle,st3255Channel,&st3255ChannelCapabilities);
            if(st3255ChannelCapabilities.channelType == NEXUS_3255ChannelType_eOutOfBand)
            {
                BDBG_MSG(("Found Docsis OOB channel index %u",st3255Channel));
                break;
            }
        }
    }
    if (NEXUS_StrCmp(NEXUS_GetEnv("disable_oob_frontend"), "y") != 0)
    {
        BDBG_MSG(("Opening onboard 3255 OOB %u",i));
        NEXUS_Frontend_GetDefault3255ChannelSettings(&st3255ChannelSettings);
        st3255ChannelSettings.channelNumber = st3255Channel;
        pConfig->frontend[i] = NEXUS_Frontend_Open3255Channel(st3255DeviceHandle,&st3255ChannelSettings);
        if ( NULL == (pConfig->frontend[i]) )
        {
            BDBG_ERR(("Unable to open 3255 tuner/demodulator OOB %d",i));
        }
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3255DeviceSettings.mtsif ? st3255ChannelSettings.channelNumber : NEXUS_InputBand_e0+i;
        userParams.isMtsif = st3255DeviceSettings.mtsif;
        userParams.chipId = 0x3255; /* 3255 API for BCM3383*/
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }
    else
    {
        BDBG_MSG(("env - disable_oob_frontend set"));
    }


    return BERR_SUCCESS;
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
    }
    }
#if 1/*defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT)*/
    NEXUS_Frontend_Close3255Device(st3255DeviceHandle);
#endif
#if 1 /*defined NEXUS_USE_7425_VMS_SFF*/
/*
    if(gpioHandle)
    {
        NEXUS_Gpio_Close(gpioHandle);
        gpioHandle = NULL;
    }
*/
#else
    for (i=0; i<NEXUS_MAX_FRONTENDS-3; i++)
    {
        if(tunerGpio[i])
        {
            NEXUS_Gpio_Close(tunerGpio[i]);
            tunerGpio[i] = NULL;
        }
    }
#endif
    return;}

#else /* NEXUS_FRONTEND_3255 */
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    return;
}
#endif /* NEXUS_FRONTEND_3255 */

#elif PLATFORM == 97241 /* PLATFORM */

/* This is only being used this way for now, as there are no other reference boards supporting 7241 + 3128 combination other than the daughter cards. */
#if NEXUS_FRONTEND_3128
#include "nexus_frontend_3128.h"

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_FrontendUserParameters userParams;
    unsigned i = 0;
    NEXUS_3128ProbeResults results;
    NEXUS_3128Settings st3128Settings;
    
    if (!pConfig->i2c[3]) {
        BDBG_ERR(("Frontend daughter cannot be initialized without first initializing I2C BSC_M3."));
        return BERR_NOT_INITIALIZED;
    }

    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);
    st3128Settings.i2cDevice = pConfig->i2c[3];
    st3128Settings.i2cAddr = 0x6c;
#if NEXUS_PLATFORM_7241_DCSFBTSFF
    st3128Settings.isrNumber = 10;
#else
    st3128Settings.isrNumber = 11;
#endif
    st3128Settings.inBandOpenDrain=true;
    st3128Settings.loadAP = true;
    st3128Settings.type = NEXUS_3128ChannelType_eInBand;
    st3128Settings.ifFrequency = 0;
    st3128Settings.isMtsif = true;
    /* NEXUS_MAX_FRONTENDS=9; BCM3128 has 8 InBand Channels and 1 OOB channel
    * Open the BCM3128 InBand channels
    */
    rc = NEXUS_Frontend_Probe3128(&st3128Settings, &results);
    if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);
    BDBG_ERR(("chip.familyId = 0x%x", results.chip.familyId));
    BDBG_ERR(("chip.id = 0x%x", results.chip.id));
    BDBG_ERR(("version.major = 0x%x", results.chip.version.major ));
    BDBG_ERR(("version.major = 0x%x", results.chip.version.minor ));
#if 1
    for (i=0; i<(results.chip.id & 0xF); i++)
    {
        BDBG_MSG(("Waiting for onboard 3128 tuner/demodulator channel %d to initialize", i));
        st3128Settings.channelNumber = i;
        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator channel %d", i));
            continue;
        }
        BDBG_ERR(("pConfig->frontend[%d] = 0x%x", i, pConfig->frontend[i]));

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0+i;
        userParams.isMtsif = st3128Settings.isMtsif;
        userParams.chipId = 0x3128;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }
#endif
    return BERR_SUCCESS;
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

#elif NEXUS_FRONTEND_3461
#include "nexus_frontend_3461.h"
#ifdef USE_SPI_FRONTEND
static NEXUS_SpiHandle g_3461spi[NEXUS_NUM_SPI_CHANNELS];
#endif
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
#ifndef USE_SPI_FRONTEND
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_3461Settings st3461Settings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDeviceHandle parentDevice=NULL;
    NEXUS_FrontendDevice3461OpenSettings deviceOpenSettings;
    NEXUS_FrontendDevice3461Settings deviceSettings;
    NEXUS_FrontendDeviceLinkSettings linkSettings;
    NEXUS_FrontendType type;

#ifdef USE_SPI_FRONTEND
    NEXUS_SpiSettings spiSettings;
#endif
    BDBG_WRN(("Waiting for the Master 3461 Downstream frontend(7241_T2SFF) to initialize"));

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    NEXUS_FrontendDevice_GetDefault3461OpenSettings(&deviceOpenSettings);

#ifdef USE_SPI_FRONTEND
    NEXUS_Spi_GetDefaultSettings(&spiSettings);
    spiSettings.clockActiveLow = true;
    g_3461spi[0] = NEXUS_Spi_Open(0, &spiSettings);
    if (!g_3461spi[0]) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    deviceOpenSettings.i2cDevice = NULL;
    deviceOpenSettings.spiDevice = g_3461spi[0];
    deviceOpenSettings.spiAddr = 0x40;
#else
    if (!pConfig->i2c[0]) {
            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        rc = BERR_NOT_INITIALIZED; goto done;
    }
    deviceOpenSettings.i2cDevice = pConfig->i2c[0];    /* Onboard tuner/demod use BSC_M3.*/
    deviceOpenSettings.i2cAddr = 0x6c;
#endif

    deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = true;

    parentDevice = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(parentDevice == NULL){
        BDBG_ERR(("Unable to open first onboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }
    
    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eInternalLna;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = true;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(parentDevice, &deviceSettings);

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    st3461Settings.device = parentDevice;
    st3461Settings.type = NEXUS_3461ChannelType_eDvbt; /*REDUNDANT for now as there is only one instance of any demod running. */
    st3461Settings.channelNumber = 0;                   /*REDUNDANT for now. */

    pConfig->frontend[0] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[0])
    {
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[0], &userParams);
    userParams.param1 = NEXUS_InputBand_e0;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[0], &userParams);

    NEXUS_Frontend_GetType(pConfig->frontend[0], &type);     /*BKNI_Sleep(800);*/ 

#ifdef USE_SPI_FRONTEND
    BKNI_Sleep(1000);
#endif

    BDBG_WRN(("Waiting for the Slave 3461 Downstream frontend(7241_T2SFF) to initialize"));

#ifdef USE_SPI_FRONTEND
	g_3461spi[1] = NEXUS_Spi_Open(1, &spiSettings);
	if (!g_3461spi[1]) {
		return BERR_TRACE(NEXUS_NOT_AVAILABLE);
	}
	deviceOpenSettings.i2cDevice = NULL;
	deviceOpenSettings.spiDevice = g_3461spi[1];
	deviceOpenSettings.spiAddr = 0x40;

#else
    if (!pConfig->i2c[4]) {
            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
            rc = BERR_NOT_INITIALIZED; goto done;
    }
    deviceOpenSettings.i2cDevice = pConfig->i2c[4];   /* Onboard tuner/demod use BSC_M4.*/
    deviceOpenSettings.i2cAddr = 0x6e;
#endif
    deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = false;

    st3461Settings.device = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(st3461Settings.device == NULL){
        BDBG_ERR(("Unable to open second nboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eOff;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = false;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(st3461Settings.device, &deviceSettings);

    pConfig->frontend[1] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[1])
    {
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[1], &userParams);
    userParams.param1 = NEXUS_InputBand_e1;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[1], &userParams);

    NEXUS_FrontendDevice_GetDefaultLinkSettings(&linkSettings);

    rc = NEXUS_FrontendDevice_Link(parentDevice, st3461Settings.device, &linkSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    return NEXUS_SUCCESS;
done:
    if(pConfig->frontend[0]){
        NEXUS_Frontend_Close(pConfig->frontend[0]);
        pConfig->frontend[0] = NULL;
    }
    if(pConfig->frontend[1]){
        NEXUS_Frontend_Close(pConfig->frontend[1]);
        pConfig->frontend[1] = NULL;
    }
    if(parentDevice){
        NEXUS_FrontendDevice_Close(parentDevice);
        parentDevice = NULL;
    }
    if(st3461Settings.device){
        NEXUS_FrontendDevice_Close(st3461Settings.device);
        st3461Settings.device = NULL;
    }
    return rc;
    #else
    return NEXUS_SUCCESS;
    #endif
}

void NEXUS_Platform_UninitFrontend(void)
{    
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0, j=0;
    NEXUS_FrontendDeviceHandle tempHandle, deviceHandles[NEXUS_MAX_FRONTENDS];
    bool handleFound = false;

    BKNI_Memset(deviceHandles, 0, sizeof(deviceHandles));

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        handleFound = false;
        if (pConfig->frontend[i])
        {
            tempHandle = NEXUS_Frontend_GetDevice(pConfig->frontend[i]);
            BDBG_MSG(("tempHandle = 0x%x", tempHandle));

            if(tempHandle != NULL){
                for( j = 0; j<i; j++){
                    if(tempHandle == deviceHandles[j])
                    handleFound = true;
                }
                if(!handleFound)
                    deviceHandles[j] = tempHandle;
            }
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
{
        if (deviceHandles[i])
    {
            BDBG_MSG(("deviceHandles[%d] = 0x%x", i, deviceHandles[i]));
            NEXUS_FrontendDevice_Close(deviceHandles[i]);
            deviceHandles[i] = NULL;
    }
    }

#ifdef USE_SPI_FRONTEND
    for (i=0; i<NEXUS_NUM_SPI_CHANNELS; i++)
{
        if (g_3461spi[i])
    {
            BDBG_MSG(("deviceHandles[%d] = 0x%x", i, g_3461spi[i]));
            NEXUS_Spi_Close(g_3461spi[i]);
            g_3461spi[i] = NULL;
        }
    }
#endif
    return;
}


#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */
NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see
                   nexus_platform_features.h for possible platform-specific macros.
                */
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_3461Settings st3461Settings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDeviceHandle parentDevice=NULL;
    NEXUS_FrontendDevice3461OpenSettings deviceOpenSettings;
    NEXUS_FrontendDevice3461Settings deviceSettings;
    NEXUS_FrontendDeviceLinkSettings linkSettings;
    NEXUS_FrontendType type;
    
#ifdef USE_SPI_FRONTEND
    NEXUS_SpiSettings spiSettings;
#endif
    BDBG_WRN(("Waiting for the Master(I2C) 3461 Downstream frontend(7241_T2SFF) to initialize"));

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    NEXUS_FrontendDevice_GetDefault3461OpenSettings(&deviceOpenSettings);
 #if 1   
    /* Master I2C */
    if (!pConfig->i2c[0]) {
                BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        rc = BERR_NOT_INITIALIZED; goto done; 
    }
    deviceOpenSettings.i2cDevice = pConfig->i2c[0];    /* Onboard tuner/demod use BSC_M3.*/
    deviceOpenSettings.i2cAddr = 0x6c;

    deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = true;

    parentDevice = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(parentDevice == NULL){
        BDBG_ERR(("Unable to open first onboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done; 
    }

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eInternalLna;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = true;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(parentDevice, &deviceSettings);

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    st3461Settings.device = parentDevice;
    st3461Settings.type = NEXUS_3461ChannelType_eDvbt; /*REDUNDANT for now as there is only one instance of any demod running. */
    st3461Settings.channelNumber = 0;                   /*REDUNDANT for now. */

    pConfig->frontend[0] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[0])
    {
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done; 
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[0], &userParams);
    userParams.param1 = NEXUS_InputBand_e0;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[0], &userParams);

    NEXUS_Frontend_GetType(pConfig->frontend[0], &type);  /*BKNI_Sleep(800);*/
#endif

    /* Slave1 SPI */
     BDBG_WRN(("Waiting for the Slave1 3461 Downstream frontend(7241_T2SFF) to initialize"));

    /* Slave1 SPI */
    NEXUS_Spi_GetDefaultSettings(&spiSettings);
    spiSettings.clockActiveLow = true;
    g_3461spi[0] = NEXUS_Spi_Open(0, &spiSettings);
    if (!g_3461spi[0]) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    deviceOpenSettings.i2cDevice = NULL;
    deviceOpenSettings.spiDevice = g_3461spi[0];
    deviceOpenSettings.spiAddr = 0x40;
    deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = false;

    parentDevice = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(parentDevice == NULL){
        BDBG_ERR(("Unable to open first onboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eInternalLna;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = false;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(parentDevice, &deviceSettings);

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    st3461Settings.device = parentDevice;
    st3461Settings.type = NEXUS_3461ChannelType_eDvbt; /*REDUNDANT for now as there is only one instance of any demod running. */
    st3461Settings.channelNumber = 0;                   /*REDUNDANT for now. */

    pConfig->frontend[1] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[1])
    {
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[1], &userParams);
    userParams.param1 = NEXUS_InputBand_e1;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[1], &userParams);

    NEXUS_Frontend_GetType(pConfig->frontend[1], &type);  /*BKNI_Sleep(800);*/
    
/* Have to maintain more than 1000ms time delay for avoiding fail issue of AC On/Off aging test.  TO DO SYKIM   */ 
/* add 500ms for clearing wake-up issue about S3 aging test in 3000t model..  TO DO SYKIM */
#ifdef USE_SPI_FRONTEND
    BKNI_Sleep(1500); 
#endif

    /* Slave2 SPI */
    BDBG_WRN(("Waiting for the Slave2 3461 Downstream frontend(7241_T2SFF) to initialize"));

    g_3461spi[1] = NEXUS_Spi_Open(1, &spiSettings);
    if (!g_3461spi[1]) {
    return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    deviceOpenSettings.i2cDevice = NULL; 
    deviceOpenSettings.spiDevice = g_3461spi[1];
    deviceOpenSettings.spiAddr = 0x40;

    deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = false;

    st3461Settings.device = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(st3461Settings.device == NULL){
        BDBG_ERR(("Unable to open second nboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
    deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eOff;
    deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
    deviceSettings.enableRfLoopThrough = false;
    deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(st3461Settings.device, &deviceSettings);

    pConfig->frontend[2] = NEXUS_Frontend_Open3461(&st3461Settings);
    if (NULL == pConfig->frontend[2])
    {
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[2], &userParams);
    userParams.param1 = NEXUS_InputBand_e2;
    userParams.pParam2 = NULL;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[2], &userParams);

    NEXUS_FrontendDevice_GetDefaultLinkSettings(&linkSettings);

    rc = NEXUS_FrontendDevice_Link(parentDevice, st3461Settings.device, &linkSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    return NEXUS_SUCCESS;
done:
    if(pConfig->frontend[0]){
        NEXUS_Frontend_Close(pConfig->frontend[0]);
        pConfig->frontend[0] = NULL;
    }
    if(pConfig->frontend[1]){
        NEXUS_Frontend_Close(pConfig->frontend[1]);
        pConfig->frontend[1] = NULL;
    }
    if(pConfig->frontend[2]){
        NEXUS_Frontend_Close(pConfig->frontend[2]);
        pConfig->frontend[2] = NULL;
    }
    if(parentDevice){
        NEXUS_FrontendDevice_Close(parentDevice);
        parentDevice = NULL;
    }
    if(st3461Settings.device){
        NEXUS_FrontendDevice_Close(st3461Settings.device);
        st3461Settings.device = NULL;
    }
    return rc;
}
#else
NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see
                   nexus_platform_features.h for possible platform-specific macros.
                */
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_3461Settings st3461Settings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendDeviceHandle parentDevice=NULL;
    NEXUS_FrontendDevice3461OpenSettings deviceOpenSettings;
    NEXUS_FrontendDevice3461Settings deviceSettings;
    NEXUS_FrontendDeviceLinkSettings linkSettings;
    NEXUS_FrontendType type;
    
#ifdef USE_SPI_FRONTEND
    NEXUS_SpiSettings spiSettings;
#endif
    BDBG_WRN(("Waiting for the Master 3461 Downstream frontend(7241_T2SFF) to initialize"));

	NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    NEXUS_FrontendDevice_GetDefault3461OpenSettings(&deviceOpenSettings);

#ifdef USE_SPI_FRONTEND
    NEXUS_Spi_GetDefaultSettings(&spiSettings);
    spiSettings.clockActiveLow = true;
    g_3461spi[0] = NEXUS_Spi_Open(0, &spiSettings);
    if (!g_3461spi[0]) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    deviceOpenSettings.i2cDevice = NULL;
    deviceOpenSettings.spiDevice = g_3461spi[0];
    deviceOpenSettings.spiAddr = 0x40;
#else
    if (!pConfig->i2c[0]) {
	            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        rc = BERR_NOT_INITIALIZED; goto done;
	    }
    deviceOpenSettings.i2cDevice = pConfig->i2c[0];    /* Onboard tuner/demod use BSC_M3.*/
    deviceOpenSettings.i2cAddr = 0x6c;
#endif

    deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = true;

	parentDevice = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(parentDevice == NULL){
        BDBG_ERR(("Unable to open first onboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

	NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
	deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eInternalLna;
	deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
	deviceSettings.enableRfLoopThrough = true;
	deviceSettings.terrestrial = true;
	NEXUS_FrontendDevice_Set3461Settings(parentDevice, &deviceSettings);

    NEXUS_Frontend_GetDefault3461Settings(&st3461Settings);
    st3461Settings.device = parentDevice;
    st3461Settings.type = NEXUS_3461ChannelType_eDvbt; /*REDUNDANT for now as there is only one instance of any demod running. */
    st3461Settings.channelNumber = 0;                   /*REDUNDANT for now. */

    pConfig->frontend[0] = NEXUS_Frontend_Open3461(&st3461Settings);
	if (NULL == pConfig->frontend[0])
	{
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
	}

	NEXUS_Frontend_GetUserParameters(pConfig->frontend[0], &userParams);
	userParams.param1 = NEXUS_InputBand_e0;
	userParams.pParam2 = NULL;
	NEXUS_Frontend_SetUserParameters(pConfig->frontend[0], &userParams);

    NEXUS_Frontend_GetType(pConfig->frontend[0], &type);  /*BKNI_Sleep(800);*/
    
/* Have to maintain more than 1000ms time delay for avoiding fail issue of AC On/Off aging test.  TO DO SYKIM   */ 
/* add 500ms for clearing wake-up issue about S3 aging test in 3000t model..  TO DO SYKIM */
#ifdef USE_SPI_FRONTEND
	BKNI_Sleep(1500); 
#endif
    
    BDBG_WRN(("Waiting for the Slave 3461 Downstream frontend(7241_T2SFF) to initialize"));

#ifdef USE_SPI_FRONTEND
	g_3461spi[1] = NEXUS_Spi_Open(1, &spiSettings);
	if (!g_3461spi[1]) {
		return BERR_TRACE(NEXUS_NOT_AVAILABLE);
	}
	deviceOpenSettings.i2cDevice = NULL;
	deviceOpenSettings.spiDevice = g_3461spi[1];
	deviceOpenSettings.spiAddr = 0x40;
#else
    if (!pConfig->i2c[4]) {
            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
            rc = BERR_NOT_INITIALIZED; goto done;
    }
	deviceOpenSettings.i2cDevice = pConfig->i2c[4];    /* Onboard tuner/demod use BSC_M4.*/
	deviceOpenSettings.i2cAddr = 0x6e;
#endif
	deviceOpenSettings.isrNumber = 11;
    deviceOpenSettings.loadAP = true;
    deviceOpenSettings.crystalSettings.enableDaisyChain = false;

    st3461Settings.device = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
    if(st3461Settings.device == NULL){
        BDBG_ERR(("Unable to open second nboard 3461 tuner/demodulator device"));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
    }

	NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
	deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eOff;
	deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
	deviceSettings.enableRfLoopThrough = false;
	deviceSettings.terrestrial = true;
    NEXUS_FrontendDevice_Set3461Settings(st3461Settings.device, &deviceSettings);

    pConfig->frontend[1] = NEXUS_Frontend_Open3461(&st3461Settings);
	if (NULL == pConfig->frontend[1])
	{
        BDBG_ERR(("Unable to open onboard 3461 tuner/demodulator channel."));
        rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
	}

	NEXUS_Frontend_GetUserParameters(pConfig->frontend[1], &userParams);
	userParams.param1 = NEXUS_InputBand_e1;
	userParams.pParam2 = NULL;
	NEXUS_Frontend_SetUserParameters(pConfig->frontend[1], &userParams);

    NEXUS_FrontendDevice_GetDefaultLinkSettings(&linkSettings);

    rc = NEXUS_FrontendDevice_Link(parentDevice, st3461Settings.device, &linkSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    return NEXUS_SUCCESS;
done:
    if(pConfig->frontend[0]){
        NEXUS_Frontend_Close(pConfig->frontend[0]);
        pConfig->frontend[0] = NULL;
    }
    if(pConfig->frontend[1]){
        NEXUS_Frontend_Close(pConfig->frontend[1]);
        pConfig->frontend[1] = NULL;
    }
    if(parentDevice){
        NEXUS_FrontendDevice_Close(parentDevice);
        parentDevice = NULL;
    }
    if(st3461Settings.device){
        NEXUS_FrontendDevice_Close(st3461Settings.device);
        st3461Settings.device = NULL;
    }
    return rc;
}

#endif

#else /* NEXUS_FRONTEND_3461 */
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    return;
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
#endif /* NEXUS_FRONTEND_3461 */

#else /* PLATFORM */
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    return;
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
#if PLATFORM == 97428
    *pInputBand = NEXUS_InputBand_e0;
#elif PLATFORM == 97429
    *pInputBand = NEXUS_InputBand_e0;  /* 9729 CV support two streamer inputs, select 0 */
#elif PLATFORM == 97241
    *pInputBand = NEXUS_InputBand_e4;
#endif
    BTRC_TRACE(ChnChange_TuneStreamer, STOP);
    return NEXUS_SUCCESS;
}



