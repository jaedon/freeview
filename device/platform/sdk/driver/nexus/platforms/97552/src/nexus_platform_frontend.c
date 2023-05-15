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
 * $brcm_Revision: 18 $
 * $brcm_Date: 6/19/12 12:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97552/src/nexus_platform_frontend.c $
 * 
 * 18   6/19/12 12:31p vishk
 * SW7552-112: set default loop through to false.
 * 
 * 17   5/14/12 3:26p jtna
 * SW7425-2752: set NEXUS_FrontendUserParams.isMtsif and .param1 to work
 *  correctly with new MTSIF mapping
 * 
 * 16   4/26/12 5:32p xhuang
 * SW7552-261: change aon_gpio_04 for external LNA
 * 
 * 15   3/14/12 6:38p xhuang
 * SW7552-220: enable OOB for 7592
 * 
 * 14   1/30/12 6:43p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support
 * 
 * 13   1/17/12 10:20a xhuang
 * SW7552-191: merge to main
 * 
 * SW7552-191/1   1/16/12 10:18a xhuang
 * SW7552-191: only enable OOB for 7574B0
 * 
 * 12   1/13/12 10:36a vishk
 * SW7552-147: 7552B0: Add Nexus OOB support change WRN to MSG.
 * 
 * 11   1/13/12 10:32a vishk
 * SW7552-147: 7552B0: Add Nexus OOB support Fix Typo.
 * 
 * 10   1/12/12 2:50p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support Disable OOB for A0
 * 
 * 9   1/11/12 5:52p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support
 * 
 * NEXUS_AOB_7552/3   1/11/12 5:08p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support
 * 
 * NEXUS_AOB_7552/2   1/3/12 4:48p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support
 * 
 * NEXUS_AOB_7552/1   1/2/12 6:35p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support
 * 
 * 8   12/28/11 4:29p vishk
 * SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
 * 
 * 7   12/28/11 4:09p vishk
 * SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board.
 * 
 * 6   12/20/11 2:46p vishk
 * SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
 * 
 * 5   12/15/11 11:39a xhuang
 * SW7552-141: Change IB5 to streamer input in B0
 * 
 * SW7552-141/1   12/6/11 1:48p jianweiz
 * SW7552-141: Change IB5 to streamer input in B0
 * 
 * 4   8/29/11 6:51p vishk
 * SW7552-112:CLONE -RF loopthrough control not exposed via API
 *  (Magnum+Nexus)
 * 
 * 3   7/18/11 7:32p xhuang
 * SW7552-59: open QAM/OFDM at run time
 * 
 * 2   6/29/11 10:57a vishk
 * SW7552-56: BBS tune problem with DVB-C/QAM 7552 bondout only support
 *  DVB-T. Enable QAM and disable DVB-T for other bondouts that support
 *  QAM.
 * 
 * 1   6/8/11 2:52p vishk
 * SW7552-35: Add 7552 Nexus frontend support. External Tuner support has
 *  been removed as there will be no use case for it. Also, Scan features
 *  have been disabled for now.
 * 
 * SW7552-34/5   6/1/11 11:18a jtna
 * SW7552-35: set correct inputbands for frontend
 * 
 * SW7552-34/4   5/31/11 3:03p jtna
 * SW7552-35: update
 * 
 * SW7552-34/3   5/27/11 8:25p jtna
 * SW7552-35: 7552 frontend work
 * 
 * SW7552-34/2   5/26/11 11:43a xhuang
 * SW7552-34: correct input band setting
 * 
 * SW7552-34/2   5/26/11 11:43a xhuang
 * SW7552-34: correct input band setting
 * 
 * SW7552-34/1   5/19/11 11:10p xhuang
 * SW7552-34: Temp disable frontend and RFM for bringup
 * 
 * 8   1/28/11 3:20p mward
 * SW7125-769:  Warning for unused. Also stub
 *  NEXUS_Platform_OpenFrontend() when !NEXUS_HAS_FRONTEND.
 * 
 * 7   1/27/11 7:03p mward
 * SW7125-769:  Stub NEXUS_Platform_OpenFrontend().
 * 
 * 6   3/1/10 6:44p VISHK
 * SW7550-149: Provide API for DVBC/QAM RF bypass.  Fixed a compiler
 *  warning.
 * 
 * 5   1/21/10 3:31p VISHK
 * SW7550-206: Please review and merge the changes in attached nexus
 *  frontend files
 * 
 * 3   11/18/09 9:43a jgarrett
 * SW7550-48: Adding nexus hooks for tuner and platform frontend
 *  initialization
 * 
 * 2   11/17/09 4:56a nitinb
 * SW7550-29: Use IB1 for streamer input in
 *  NEXUS_Platform_GetStreamerInputBand
 * 
 * 1   9/18/09 11:56a nitinb
 * SW7550-29: Getting initial nexus build for 7550
 * 
 ***************************************************************************/
#include "nexus_types.h"
#include "priv/nexus_core.h"
#include "nexus_platform_features.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "nexus_input_band.h"
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#include "nexus_frontend_7552.h"
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
#include "nexus_frontend_3128.h"
#endif
#endif

BDBG_MODULE(nexus_platform_frontend);

#if NEXUS_HAS_FRONTEND

NEXUS_Error NEXUS_Platform_InitFrontend()
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_PlatformStatus platformStatus;    
    int index=0;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_Error rc;
    NEXUS_7552ConfigSettings configSettings;
    NEXUS_7552FrontendSettings settings;        
    NEXUS_GpioSettings gpioSettings;    
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
    unsigned i=0;
    NEXUS_3128Settings st3128Settings;
    NEXUS_3128ProbeResults results;
#endif        
    rc = NEXUS_Platform_GetStatus(&platformStatus);
    if(rc != NEXUS_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto error;
    }


    BDBG_MSG(("Chip ID = 0x%x", platformStatus.chipId));
    if((platformStatus.chipId == 0x7581) || (platformStatus.chipId == 0x7582) ||
        (platformStatus.chipId == 0x7591) || (platformStatus.chipId == 0x7592) ||
        (platformStatus.chipId == 0x7574))
    {
        NEXUS_Frontend_GetDefault7552Settings(&settings);
        settings.type = NEXUS_7552ChannelType_eQam;
        pConfig->frontend[index] = NEXUS_Frontend_Open7552(&settings);
        if ( NULL == pConfig->frontend[index] )
        {
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        BDBG_MSG(("pConfig->frontend[%d] = 0x%x", index, pConfig->frontend[index]));
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[index], &userParams);
        userParams.param1 = NEXUS_InputBand_e1;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[index], &userParams);
    
        index++;
    }

    if((platformStatus.chipId == 0x7531) || (platformStatus.chipId == 0x7532) ||
        (platformStatus.chipId == 0x7541) || (platformStatus.chipId == 0x7542) ||
        (platformStatus.chipId == 0x7551) || (platformStatus.chipId == 0x7552) ||        
        (platformStatus.chipId == 0x7591) || (platformStatus.chipId == 0x7592))
    {
        NEXUS_Frontend_GetDefault7552Settings(&settings);
        settings.type = NEXUS_7552ChannelType_eOfdm;
        /* default setting support ISDB-T, disable for DVB-T only chip */
        if((platformStatus.chipId == 0x7551) || (platformStatus.chipId == 0x7552))
        {
            settings.supportIsdbt = false;
        }
        pConfig->frontend[index] = NEXUS_Frontend_Open7552(&settings);
        if ( NULL == pConfig->frontend[index] )
        {
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto error;
        }
        BDBG_MSG(("pConfig->frontend[%d] = 0x%x", index, pConfig->frontend[index]));

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[index], &userParams);
        userParams.param1 = NEXUS_InputBand_e0;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[index], &userParams);

        index++;
    }

   
    NEXUS_Frontend_GetDefault7552ConfigSettings(&configSettings);
    configSettings.rfInput = NEXUS_7552TunerRfInput_eInternalLna;
    configSettings.enableRfLoopThrough = false;
    NEXUS_Frontend_7552_SetConfigSettings(pConfig->frontend[index-1], &configSettings);

#ifndef	NEXUS_FRONTEND_7552_A0
    if((platformStatus.chipId == 0x7574) || (platformStatus.chipId == 0x7592))
    {
        NEXUS_Frontend_GetDefault7552Settings(&settings);
        settings.type = NEXUS_7552ChannelType_eOutOfBand;
        pConfig->frontend[index] = NEXUS_Frontend_Open7552(&settings);
        if ( NULL == pConfig->frontend[index] )
        {
           (void)BERR_TRACE(BERR_NOT_SUPPORTED);
           goto error;
        }
        
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[index], &userParams);
        userParams.param1 = NEXUS_InputBand_e9;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[index], &userParams);

        index++;
    }
#endif
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
#if (BCHP_VER >= BCHP_VER_B0)
    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);

    st3128Settings.i2cDevice = pConfig->i2c[1];
    st3128Settings.i2cAddr = 0x6c;
    st3128Settings.isrNumber = 10; /* Make sure that this number is correct. */
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

    for (i=0; i < NEXUS_3128_MAX_DOWNSTREAM_CHANNELS;i++)
    {
        BDBG_MSG(("Waiting for onboard 3128 tuner/demodulator channel %d to initialize", i));
        st3128Settings.channelNumber = i;
        pConfig->frontend[index] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[index])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator channel %d", i));
            continue;
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[index], &userParams);
        userParams.isMtsif = st3128Settings.isMtsif;
        userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        userParams.chipId = 0x3128;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[index], &userParams);
        index++;
    }
#endif
#endif

    /* set aon_gpio_04 to gpio output for external LNA */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &gpioSettings); 
    gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
    gpioSettings.value = NEXUS_GpioValue_eHigh;
    gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
    NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 4, &gpioSettings);

    return BERR_SUCCESS;

error:
    while ( index-- > 0 )
    {
        NEXUS_Frontend_Close(pConfig->frontend[index]);
        pConfig->frontend[index] = NULL;
    }
    return BERR_TRACE(BERR_NOT_INITIALIZED);
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    int i;

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        if ( NULL != pConfig->frontend[i] )
        {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }
}
#else
NEXUS_Error NEXUS_Platform_InitFrontend()
{
    NEXUS_Error errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_Error errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
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
#if (BCHP_VER >= BCHP_VER_B0)   
    *pInputBand = NEXUS_InputBand_e5;
#else
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

