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
* $brcm_Workfile: nexus_platform_frontend.c $
* $brcm_Revision: 40 $
* $brcm_Date: 12/7/12 6:29p $
*
* API Description:
*   API name: Platform Frontend
*    Platform Frontend Setup
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97346/src/nexus_platform_frontend.c $
* 
* 40   12/7/12 6:29p vishk
* SW3472-36: Disable second channel.
* 
* 39   12/5/12 6:20p vishk
* SW3472-23: CLONE - CLONE - Add SSI and SQI for each "Layer" in HAB
*  ISDBT Status Message - NEXUS
* 
* 38   10/26/12 2:53p vishk
* SW3461-206: Add support for 3472 frontend.
* 
* 37   7/26/12 11:23a jrubio
* SW7346-848: give control to first handle
* 
* 36   7/24/12 2:27p jrubio
* SW7346-848: merge in Vish Changes into mainline
* 
* 35   6/14/12 4:26p jrubio
* SW7346-828: change SFF_V20 to SFF_V10
* 
* 34   5/29/12 10:49a randyjew
* SW7344-9: Move nexus_platform_modules.h to top.
* 
* 33   5/25/12 5:55p randyjew
* SW7344-9: include nexus_platform_modules.h
* 
* 32   5/25/12 11:35a randyjew
* SW7344-9: fix prototype warnings
* 
* 31   2/21/12 2:39p randyjew
* SW7344-9: Disable frontend for 7418 B1
* 
* 30   2/1/12 1:30p jrubio
* SW7346-582: update input bands for HR44
* 
* 29   12/2/11 2:23p jrubio
* SW7346-582: add SFF V20 support
* 
* 28   11/2/11 4:43p randyjew
* SW7344-198: Move input band 2 to 4505
* 
* 27   10/13/11 3:57p randyjew
* SW7344-198:Add 4505 support to 7344SV
* 
* 26   10/11/11 3:21p jrubio
* SW7346-470: change i2c channel
* 
* 25   10/7/11 2:31p vishk
* SW3128-47: Add Support for BCM3123
* 
* 24   10/6/11 4:53p randyjew
* SW7344-198: Add NEXUS_PLATFORM_7344SV for 4506 support
* 
* 23   9/29/11 1:08p randyjew
* SW7344-9: Disable frontend for 7418 B0
* 
* 22   9/26/11 6:37p randyjew
* SW7344-140:Adding 3123 support for 7418SFF_H platform
* 
* 21   9/7/11 2:55p jrubio
* SW7346-470: update input bands for 7346B0
* 
* 20   9/4/11 1:42p vishk
* SW7346-301: Nexus Frontend 7346 needs to support both internal and
*  external (BCM3445) LNAs. Add support for HR44 platform.
* 
* 19   7/27/11 3:36p jrubio
* SW7344-117: fix LNA settings for 7346, fix platform 7346SV
* 
* 18   7/25/11 3:26p jrubio
* SW7346-117: remove duplicate code in frontend code
* 
* 17   7/19/11 10:10a jrubio
* SW7344-112: fix compiler issue
* 
* 16   7/8/11 10:36a jrubio
* SW7344-112: fix close crash for 7344
* 
* 15   6/30/11 4:57p jrubio
* SW7346-165: update streamer band
* 
* 14   6/29/11 10:37a jrubio
* SW7346-165: remove the duplicate code, fix shudown of tuners
* 
* 13   6/28/11 3:25p vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent Add support for SFF board.
* 
* 12   6/27/11 2:01p jrubio
* SW7346-165: turn into 73xx include file for 7344
* 
* 11   6/27/11 10:46a jrubio
* SW7346-165: change 73xx to 73XX in the defined
* 
* 10   6/24/11 11:30a vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent. Compile error.
* 
* 9   6/24/11 11:19a vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent. Bug fix.
* 
* 8   6/24/11 11:12a VISHK
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent. Merge support for 73xx frontends and 7346 40nm
*  frontend.
* 
* 7   6/24/11 10:04a vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent. Compilation error fix.
* 
* 6   6/23/11 7:18p vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent. Bug fix.
* 
* 5   6/23/11 5:50p vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent
* 
* 4   6/22/11 4:02p jrubio
* SW7346-165: remove qpsk tuner
* 
* 3   6/16/11 3:39p randyjew
* SW7344-9: move unused  7344 variable
* 
* 2   5/23/11 3:37p jrubio
* SW7346-213: limit i2c writes to 7346
* 
* 1   5/23/11 3:00p jrubio
* SW7346-213: master the frontend file in 7346
* 
* 12   2/4/11 8:08a jrubio
* SW7344-9: add isl for tone
* 
* 11   1/28/11 3:16p mward
* SW7125-769:  Warning for unused. Also stub
*  NEXUS_Platform_OpenFrontend() when !NEXUS_HAS_FRONTEND.
* 
* 10   1/27/11 3:17p mward
* SW7125-769:  Stub NEXUS_Platform_OpenFrontend().
* 
* 9   1/17/11 12:46p agin
* SW7344-9:  Use PKT0 for LVDS, PKT2 for ASI, and read FPGA chip ID.
* 
* 8   12/28/10 1:51p gmohile
* SW7344-9 : Disable frontend init for 7418 product id
* 
* 7   12/28/10 9:32a agin
* SW7344-9:  Fix LNA for 97346, input band selection.
* 
* 6   12/22/10 2:46p jrubio
* SW7344-9: update streamer input for ASI/LVDS
* 
* 5   12/22/10 11:45a agin
* SW7344-9:  Allow 97346 to initialize properly.
* 
* 4   12/20/10 7:38p agin
* SW7344-9:  Fixed input band mapping for 97344.  Don't initlialize QPSK
*  tuner/demod yet.
* 
* 3   12/15/10 10:06p jrubio
* SW7344-9: update streamer mapping 7346
* 
* 2   12/14/10 11:52a jrubio
* SW7344-9: disable frontend tuners, change input band mapping for 7346
* 
* 1   12/8/10 3:16p jrubio
* SW7344-9: initial frontend file
* 
*
*
***************************************************************************/
#include "nexus_platform_module.h"
#include "nexus_types.h"
#if NEXUS_HAS_I2C && NEXUS_HAS_FRONTEND

#include "priv/nexus_core.h"
#include "nexus_i2c.h"
#include "bfpga.h"
#include "bfpga_name.h"
#include "nexus_frontend.h"
#include "nexus_platform_features.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "bchp_sun_top_ctrl.h"
#include "nexus_frontend_7346.h"
#if NEXUS_FRONTEND_4506
#include "nexus_frontend_4506.h"
#endif
#if NEXUS_PLATFORM_7418SFF_H
#include "nexus_frontend_3128.h"
#endif
#if NEXUS_PLATFORM_97346_I2SFF
#include "nexus_frontend_3461.h"
#endif
BDBG_MODULE(nexus_platform_frontend);

/* i2c channel assignments */
#define I2C_DEVICE_FPGA_CH 2
#if (BCHP_CHIP==7344)
#define I2C_DEVICE_VOLTAGE_REG_CH 1
#else /* 7346 */
#define I2C_DEVICE_VOLTAGE_REG_CH 3
#endif

#define ISL9492_CH0_I2C_ADDR 0x08
#define ISL9492_CH1_I2C_ADDR 0x09
#define FPGA_CHIP_ADDR 0xE

#if NEXUS_PLATFORM_97346_HR44
    static NEXUS_GpioHandle gpioHandle = NULL;
#endif
#if defined(NEXUS_PLATFORM_7418SFF_H)
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i;
    NEXUS_3128Settings st3128Settings;
    NEXUS_FrontendUserParameters userParams;

    /* Open downstream tuners */
    for (i=0; i < NEXUS_3128_MAX_DOWNSTREAM_CHANNELS; i++)
    {
        BDBG_WRN(("Waiting for frontend 3123 channel %d to initialize", i));
        
        NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);
        st3128Settings.isrNumber = 18;    
        st3128Settings.i2cDevice = pConfig->i2c[2];    /* Onboard tuner/demod use BSC_M2.*/    
        st3128Settings.i2cAddr = 0x6f;
        st3128Settings.type = NEXUS_3128ChannelType_eInBand;
        st3128Settings.channelNumber = i;
        st3128Settings.ifFrequency = 0;     
        st3128Settings.inBandOpenDrain=true;
        st3128Settings.loadAP = true;
        st3128Settings.configureWatchdog = false;
        st3128Settings.isMtsif = false;

        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_MSG(("Unable to open onboard 3123 tuner/demodulator %d", i));
            continue;
        }
        BDBG_MSG(("pConfig->frontend[%d]",i, pConfig->frontend[i]));
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

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i])
             NEXUS_Frontend_Close(pConfig->frontend[i]);
    }

    return;
}

#else

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
#if BCHP_CHIP == 7346   
    uint8_t  data_c, data_d, data_e, regaddr=0;
#endif
    NEXUS_7346FrontendSettings settings;
    NEXUS_FrontendUserParameters userParams;
    unsigned i=0;
    uint8_t data;
    uint32_t prod_id;
#if NEXUS_PLATFORM_97346_I2SFF
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned j;
    NEXUS_3461Settings st3472Settings;
    NEXUS_3461ProbeResults results;
    NEXUS_FrontendDevice3461OpenSettings deviceOpenSettings;
    NEXUS_FrontendDevice3461Settings deviceSettings;
#endif  
#if NEXUS_PLATFORM_97346_SV || NEXUS_PLATFORM_97346_HR44 || NEXUS_PLATFORM_7344SV
    unsigned j;
    NEXUS_4506Settings settings4506;
#if NEXUS_PLATFORM_97346_HR44
    NEXUS_GpioSettings gpioSettings;
#endif
#endif
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    
    NEXUS_Platform_ReadRegister(BCHP_SUN_TOP_CTRL_PRODUCT_ID, &prod_id);
    if(prod_id == 0x74180000 || prod_id == 0x74180010 || prod_id == 0x74180011){
    return BERR_SUCCESS;
    }

    NEXUS_Frontend_GetDefault7346Settings(&settings);

    /* Setup LNA configurations per-board. */
#if (BCHP_CHIP == 7346)
#if NEXUS_PLATFORM_97346_SFF || NEXUS_PLATFORM_97346_I2SFF
#ifdef SFF_V10
    settings.lnaSettings.out0 = NEXUS_7346LnaInput_eIn1;
    settings.lnaSettings.out1 = NEXUS_7346LnaInput_eIn1;
    settings.lnaSettings.daisy = NEXUS_7346LnaInput_eNone;
#else
    settings.lnaSettings.out0 = NEXUS_7346LnaInput_eIn0;
    settings.lnaSettings.out1 = NEXUS_7346LnaInput_eIn0;
    settings.lnaSettings.daisy = NEXUS_7346LnaInput_eNone;
#endif
#elif NEXUS_PLATFORM_97346_HR44
    settings.isInternalLna = true;
    settings.lnaSettings.out0 = NEXUS_7346LnaInput_eIn0;
    settings.lnaSettings.out1 = NEXUS_7346LnaInput_eIn0;
    settings.lnaSettings.daisy = NEXUS_7346LnaInput_eIn0;
    settings.external3445Lna.enabled = true;
    settings.external3445Lna.i2cChannelNumber = 0; /* SDS0 */
    settings.external3445Lna.lnaOutput = NEXUS_3445LnaOutput_eOut1;
    settings.external3445Lna.settings.daisy = NEXUS_3445LnaInput_eIn2Vga;
    settings.external3445Lna.settings.out1  = NEXUS_3445LnaInput_eIn2Vga;
    settings.external3445Lna.settings.out2  = NEXUS_3445LnaInput_eIn2Vga;
#else
    settings.lnaSettings.out0 = NEXUS_7346LnaInput_eIn0;
    settings.lnaSettings.out1 = NEXUS_7346LnaInput_eIn1;
    settings.lnaSettings.daisy = NEXUS_7346LnaInput_eIn0;
#endif  
    settings.isInternalLna = true;
#else /* 7344 SFF and SV board */
    settings.lnaSettings.out0 = NEXUS_7346LnaInput_eIn0;
    settings.lnaSettings.out1 = NEXUS_7346LnaInput_eNone;
    settings.lnaSettings.daisy = NEXUS_7346LnaInput_eIn0;
    settings.isInternalLna = true;
#endif

    /* Switch the frontends. Diseqc is on tuner 1 */
    /* Open 734x Demodulator Channels */
    for ( i = 0; i < NEXUS_7346_MAX_FRONTEND_CHANNELS; i++ )
    {
        BDBG_WRN(("734x Trying to open frontend channel %d", i));
        settings.channelNumber = i;       

        pConfig->frontend[i] = NEXUS_Frontend_Open7346(&settings);
        if ( pConfig->frontend[i] )
        {
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            switch (i)
            {
#if BCHP_CHIP==7346
            case 0: userParams.param1 = NEXUS_InputBand_e8; break;
            case 1: userParams.param1 = NEXUS_InputBand_e9; break;
            default: BDBG_MSG(("unsupported channel!\n"));
#else /* 97344 */
                case 0: userParams.param1 = NEXUS_InputBand_e3; break;
                
                default: BDBG_MSG(("unsupported channel!\n"));
#endif
                }
            userParams.pParam2 = NULL;
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
        }
        else
        {
            BDBG_MSG(("NEXUS_Frontend_Open7346 Failed!\n"));
        }
        BDBG_MSG(("pConfig->frontend[%d] = 0x%x", i, pConfig->frontend[i]));
    }

#if NEXUS_PLATFORM_97346_I2SFF
        if (!pConfig->i2c[2]) {
            BDBG_MSG(("Frontend daughter cannot be initialized without first initializing I2C BSC_M2."));
            return BERR_NOT_INITIALIZED;
        }

        NEXUS_Frontend_GetDefault3461Settings(&st3472Settings);

        NEXUS_FrontendDevice_GetDefault3461OpenSettings(&deviceOpenSettings);
        deviceOpenSettings.i2cDevice = pConfig->i2c[2];
        deviceOpenSettings.i2cAddr = 0x6c;
        deviceOpenSettings.isrNumber = 11;
        deviceOpenSettings.inBandOpenDrain=true;
        deviceOpenSettings.loadAP = true;
        deviceOpenSettings.isMtsif = true;

        rc = NEXUS_Frontend_Probe3461(&deviceOpenSettings, &results);
        if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);
        BDBG_MSG(("chip.familyId = 0x%x", results.chip.familyId));
        BDBG_MSG(("chip.id = 0x%x", results.chip.id));
        BDBG_MSG(("version.major = 0x%x", results.chip.version.major ));
        BDBG_MSG(("version.major = 0x%x", results.chip.version.minor ));        

        st3472Settings.device = NEXUS_FrontendDevice_Open3461(0, &deviceOpenSettings);
        if(st3472Settings.device == NULL){rc = BERR_TRACE(BERR_NOT_INITIALIZED); return rc;}

        NEXUS_FrontendDevice_GetDefault3461Settings(&deviceSettings);
        deviceSettings.rfDaisyChain = NEXUS_3461RfDaisyChain_eOff;
        deviceSettings.rfInput = NEXUS_3461TunerRfInput_eInternalLna;
        deviceSettings.enableRfLoopThrough = false;
        deviceSettings.terrestrial = true;
        NEXUS_FrontendDevice_Set3461Settings(st3472Settings.device, &deviceSettings);

        st3472Settings.type = NEXUS_3461ChannelType_eIsdbt;

        /* Currently only one frontend channel will be opened. The second will be supported soon. */
        for (j=0; j<1; j++, i++)
        {
            BDBG_MSG(("Waiting for onboard 3472 tuner/demodulator channel %d to initialize", j));
            st3472Settings.channelNumber = j;
            pConfig->frontend[i] = NEXUS_Frontend_Open3461(&st3472Settings);
            if (NULL == pConfig->frontend[i])
            {
                BDBG_MSG(("Unable to open onboard 3472 tuner/demodulator channel %d", j));
                continue;
            }

            if(!st3472Settings.device)st3472Settings.device = NEXUS_Frontend_GetDevice(pConfig->frontend[i]);
    
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            userParams.param1 = deviceOpenSettings.isMtsif ? st3472Settings.channelNumber : NEXUS_InputBand_e0+i;
            userParams.isMtsif = deviceOpenSettings.isMtsif;
            userParams.chipId = 0x3472;
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
        }
#endif

    /* Swap frontend handles now*/
#if NEXUS_PLATFORM_97346_SFF || NEXUS_PLATFORM_97346_I2SFF
{   
    NEXUS_FrontendHandle tempfrontend1,tempfrontend2;
    tempfrontend1 =       pConfig->frontend[0];
    tempfrontend2 =       pConfig->frontend[1];
    pConfig->frontend[0]= tempfrontend2 ;
    pConfig->frontend[1]= tempfrontend1; 

}
#endif


#if NEXUS_PLATFORM_97346_SV  || NEXUS_PLATFORM_97346_HR44 || NEXUS_PLATFORM_7344SV
#if NEXUS_PLATFORM_97346_HR44
       NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettings);
       gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
       gpioSettings.value = NEXUS_GpioValue_eHigh;
       gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
       gpioSettings.interrupt.callback = NULL;
       gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 49, &gpioSettings);
       BDBG_ASSERT(NULL != gpioHandle);
       NEXUS_Gpio_Close(gpioHandle);
       gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 65, &gpioSettings);
       BDBG_ASSERT(NULL != gpioHandle);
      
       /* delay for reset extension */
       BKNI_Sleep(200);
#endif

    /* Open 4505 has only one Demodulator Channel */
#if NEXUS_PLATFORM_97346_HR44
    for ( j = 0; j < NEXUS_4506_MAX_FRONTEND_CHANNELS ; j++, i++ )
    {
        BDBG_WRN(("4506 Trying to open frontend channel %d", i));
        /* Open on-board 4506 */
        NEXUS_Frontend_GetDefault4506Settings(&settings4506);
        settings4506.i2cDevice = pConfig->i2c[2];   
        if (!settings4506.i2cDevice) {
            BDBG_MSG(("Unable to initialize I2C \n"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        settings4506.isrNumber = 17;
        settings4506.i2cAddr = 0x69;
        settings4506.is3445ExternalLna = false;
        settings4506.channelNumber = j;
        pConfig->frontend[i] = NEXUS_Frontend_Open4506(&settings4506);
        if ( pConfig->frontend[i] )
        {
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            switch (j)
            {
                case 0: userParams.param1 = NEXUS_InputBand_e0; break;
                case 1: userParams.param1 = NEXUS_InputBand_e1; break;
                default: BDBG_MSG(("unsupported channel!\n"));
                }
            userParams.pParam2 = NULL;
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
        }
        else
        {
            BDBG_MSG(("NEXUS_Frontend_Open4506 Failed!\n"));
        }
    }
#endif

    /* Open 4505 has only one Demodulator Channel */
#if NEXUS_PLATFORM_97346_HR44
    for ( j = 0; j < NEXUS_4506_MAX_FRONTEND_CHANNELS ; j++, i++ )
#else
    for ( j = 0; j < NEXUS_4506_MAX_FRONTEND_CHANNELS - 1 ; j++, i++ )
#endif
    {
        BDBG_WRN(("4506 Trying to open frontend channel %d", i));
        /* Open on-board 4506 */
        NEXUS_Frontend_GetDefault4506Settings(&settings4506);
#if NEXUS_PLATFORM_97346_HR44       
        settings4506.i2cDevice = pConfig->i2c[3];   
#elif (BCHP_CHIP==7346 && BCHP_VER==BCHP_VER_A0) || (NEXUS_PLATFORM_7344SV)
        settings4506.i2cDevice = pConfig->i2c[1];
#else        
        settings4506.i2cDevice = pConfig->i2c[2];
#endif
        if (!settings4506.i2cDevice) {
            BDBG_MSG(("Unable to initialize I2C \n"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        settings4506.isrNumber = 11;
        settings4506.i2cAddr = 0x69;
        /* As of now, only for 4506 daughter card, this need to be set true. Because the 4506 on the daughter card directly controls the external 3445 LNA. */
        settings4506.is3445ExternalLna = false;
        settings4506.channelNumber = j;
        pConfig->frontend[i] = NEXUS_Frontend_Open4506(&settings4506);
        if ( pConfig->frontend[i] )
        {
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            switch (j)
            {
#if  NEXUS_PLATFORM_7344SV
            case 0: userParams.param1 = NEXUS_InputBand_e2; break;
#endif
#if NEXUS_PLATFORM_97346_SV || NEXUS_PLATFORM_97346_HR44
            case 0: userParams.param1 = NEXUS_InputBand_e5; break;
#endif

#if NEXUS_PLATFORM_97346_HR44               
                case 1: userParams.param1 = NEXUS_InputBand_e3; break;
#endif
                default: BDBG_MSG(("unsupported channel!\n"));
                }
            userParams.pParam2 = NULL;
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
        }
        else
        {
            BDBG_MSG(("NEXUS_Frontend_Open external 4506 Failed!\n"));
        }
    }
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)

#else

    /* Tone generated internally according to EXTMpin */
    data = 0x20;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00, (const uint8_t *)&data, 1);
    /* Dynamic current limit */
    data = 0x44;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00,(const uint8_t *)&data, 1);
    /* Internal linear regulator is turned on and boost circuit is on */
    data = 0x78;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00,(const uint8_t *)&data, 1);
#if BCHP_CHIP==7346
    data = 0x20;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00,(const uint8_t *) &data, 1);
    data = 0x44;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00, (const uint8_t *) &data, 1);
    data = 0x78;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00, (const uint8_t *) &data, 1);
#endif

/* SET THE FPGA for STREAMER by DEFAULT */
    /* 7346 Board only */
#if (BCHP_CHIP == 7346) && !defined(NEXUS_PLATFORM_97346_SFF)
    BDBG_MSG(("fpga i2c %d %p", I2C_DEVICE_FPGA_CH , g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_FPGA_CH]));
    (void)NEXUS_I2c_Read(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_FPGA_CH], FPGA_CHIP_ADDR, 0xc, &data_c, 1);
    (void)NEXUS_I2c_Read(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_FPGA_CH], FPGA_CHIP_ADDR, 0xd, &data_d, 1);
    (void)NEXUS_I2c_Read(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_FPGA_CH], FPGA_CHIP_ADDR, 0xe, &data_e, 1);
    
    BDBG_MSG(("FPGA version:  0x%02x\n", data_e));

    if ((data_c != 0x46) || (data_d != 0x73)) BDBG_MSG(("Reading of Transport FPGA yields CHIPID=0x%02x%02x\n", data_d, data_c));

    data = 0x80;
    (void)NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_FPGA_CH], FPGA_CHIP_ADDR, 4, &data, 1);

    if (NEXUS_GetEnv("NEXUS_ASI")) {
        regaddr = 2; /* value maybe 0?*/
        data = 0x10; /* Use streamer 1 for ASI, pkt interface 2 */
    } else {
        regaddr = 2;
        data = 0x00; /* Use streamer 0 for LVDS, pkt interface 0 */
    }
    (void)NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_FPGA_CH], FPGA_CHIP_ADDR, regaddr, &data, 1);
#endif    

#endif /* #if defined(HUMAX_PLATFORM_BASE) */


    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    int i;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;

    for ( i = NEXUS_MAX_FRONTENDS-1; i >= 0; i-- )
    {
        /* Close extensions first */
        if (pConfig->frontend[i])
        {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
        }
#if 0 /* Once SWM is in*/
        /* Close underlying demod */
        if ( pConfig->frontend[i] )
        {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
        }
#endif
    }
#if NEXUS_PLATFORM_97346_HR44   
    if(gpioHandle)
    {
        NEXUS_Gpio_Close(gpioHandle);
        gpioHandle = NULL;
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
    BTRC_TRACE(ChnChange_TuneStreamer, START);

#if BCHP_CHIP==7344

    switch (index)
    {
        case 0:
            *pInputBand = NEXUS_InputBand_e0;
            break;
        case 1:
            *pInputBand = NEXUS_InputBand_e1;
            break;
        default:
            BDBG_MSG(("index %d not supported\n", index));
            return NEXUS_NOT_SUPPORTED; 
    }

#else 
    BSTD_UNUSED(index);   
    *pInputBand = NEXUS_InputBand_e2;
    
#endif

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

#else
#endif /* NEXUS_HAS_I2C && NEXUS_HAS_FRONTEND */



