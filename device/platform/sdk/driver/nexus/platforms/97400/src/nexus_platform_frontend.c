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
* $brcm_Revision: 24 $
* $brcm_Date: 6/8/12 12:12p $
*
* API Description:
*   API name: Platform Frontend
*    Platform Frontend Setup
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/nexus_platform_frontend.c $
* 
* 24   6/8/12 12:12p mward
* SW7400-3078:  Compiler warning.
* 
* 23   11/15/11 3:19p erickson
* SW7425-1747: remove old code
* 
* 22   6/9/11 11:27a shyi
* SWDTV-7365: Added Sharp VA1E1CD2406 tuner support to Nexus
* 
* 21   5/25/11 12:08p shyi
* SWDTV-7214: Added checking whether frontend was opened to platform
*  unitialization time
* 
* 20   5/19/11 2:46p jhaberf
* SWDTV-6928: Merge in of 35233 frontend changes
* 
* 19   5/17/11 2:15p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* SWDTV-6928/1   5/4/11 6:25p mstefan
* SWDTV-6928: merge to main
* 
* 18   1/28/11 3:17p mward
* SW7125-769:  Warning for unused. Also stub
*  NEXUS_Platform_OpenFrontend() when !NEXUS_HAS_FRONTEND.
* 
* 17   1/27/11 3:24p mward
* SW7125-769:  Stub NEXUS_Platform_OpenFrontend().
* 
* 16   3/22/10 5:11p mward
* SW7400-2726:  Set global handles to NULL after closing to indicate
*  closed to prevent errors re-initializing.
* 
* 15   2/26/10 2:40p mward
* SW7400-2673: Fix build without NEXUS_FRONTEND_4500.
* 
* 14   2/25/10 6:22p mward
* SW7400-2673: Implement support for BCM4500.
* 
* SW7400-2673/1   2/24/10 3:34p mward
* SW7400-2673: probe for on-board 4500.
* 
* 13   8/20/09 1:58p erickson
* PR56791: set NEXUS_FrontendCardSettings.bitWideSync = true for 97400
*  demods
*
* 12   8/10/09 11:30a gmohile
* PR 56512 : Remove references to 7401/03
*
* 11   7/31/09 9:37a gmohile
* PR 56512 : Merger 7403 support to main line
*
* 10   7/7/09 12:12p mward
* PR56558: Compiler error: conflicting types for 'i'.
*
* 9   7/2/09 10:42a erickson
* PR56558: make NEXUS_Platform_InitFrontend a public function, callable
*  after NEXUS_Platform_Init if NEXUS_PlatformSettings.openFrontend =
*  false.
*
* 8   5/8/09 10:27a mward
* PR47739:  Restoring omitted registration of onboard frontend.
*
* 7   5/7/09 4:57p mward
* PR47739:  Close frontend cards.  Separately close on-board 3517 and its
*  tuner.
*
* 6   2/23/09 5:47p mward
* PR47739: Support 4501 and 4506 tuner cards.
*
* 5   1/26/09 3:28p mphillip
* PR51468: Update 7400 nexus_platform_frontend for the naming convention
*
* 4   10/9/08 10:15a erickson
* PR47328: moved NEXUS_Platform_GetStreamerInputBand to frontend.c
*
* 3   2/7/08 6:44p jgarrett
* PR 34907: Correcting function prototypes
*
* 2   2/1/08 5:00p vsilyaev
* PR 38682: Fixed prototype for functions without argument
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/3   11/8/07 9:17a erickson
* PR36802: use standard NEXUS_HAS_MODULE defines, impl Platform_Uninit
*
* Nexus_Devel/2   11/5/07 2:12p erickson
* PR36725: add NEXUS_PlatformConfiguration, share more code with 97401
*
* Nexus_Devel/1   10/10/07 11:29a jgarrett
* PR 35551: Initial version
*
***************************************************************************/

#include "nexus_types.h"
#include "priv/nexus_core.h"
#include "nexus_i2c.h"
#include "bfpga.h"
#include "bfpga_name.h"
#include "nexus_frontend.h"
#include "nexus_frontend_3520.h"
#include "nexus_tuner_dtt760x.h"
#ifdef NEXUS_FRONTEND_4500
#include "nexus_frontend_4500.h"
#endif
#include "nexus_platform_features.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "bchp_hif_cpu_intr1.h"

BDBG_MODULE(nexus_platform_frontend);

#if NEXUS_NUM_FRONTEND_CARD_SLOTS
static NEXUS_FrontendCardHandle g_frontendCards[NEXUS_NUM_FRONTEND_CARD_SLOTS];
#endif

static bool g_inputBandBusy[NEXUS_NUM_INPUT_BANDS];

static BFPGA_OutputSelect g_fpgaOutputs[NEXUS_NUM_INPUT_BANDS] =
{
    BFPGA_OutputSelect_ePkt0, /* Packet 0 */
    BFPGA_OutputSelect_ePkt1, /* Packet 1 */
    BFPGA_OutputSelect_ePkt2, /* Packet 2 */
    BFPGA_OutputSelect_ePkt3, /* Packet 3 */
    BFPGA_OutputSelect_ePkt4  /* Packet 4 */
#if NEXUS_NUM_INPUT_BANDS > 5
    ,BFPGA_OutputSelect_ePkt4  /* Packet 5 doesn't exist yet */
#endif
};

static NEXUS_FrontendHandle g_onboard3517 = NULL;
static NEXUS_FrontendHandle g_onboard4500 = NULL;
static NEXUS_TunerHandle g_onboardDtt760x = NULL;

int NEXUS_Platform_P_AllocInputBand(NEXUS_FrontendHandle frontend, BFPGA_TsSelect tsSelect)
{
    BERR_Code errCode;
    int i;
    if (NULL == g_NEXUS_platformHandles.fpgaHandle)
    {
        /* Can't allocate an input band. */
        return NEXUS_NUM_INPUT_BANDS;
    }
    /* Allocate an input band for this frontend */
    for ( i = 0; i < NEXUS_NUM_INPUT_BANDS; i++ )
    {
        if ( false == g_inputBandBusy[i] )
        {
            NEXUS_FrontendUserParameters userParams;
            g_inputBandBusy[i] = true;
            /* Route FPGA */
            errCode = BFPGA_SetTsOutput(g_NEXUS_platformHandles.fpgaHandle, g_fpgaOutputs[i], tsSelect);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
            NEXUS_Frontend_GetUserParameters(frontend, &userParams);
            userParams.param1 = i;  /* Input Band */
            userParams.pParam2 = (void *)tsSelect; /* FPGA Selector */
            NEXUS_Frontend_SetUserParameters(frontend, &userParams);
            break;
        }
    }
    return i;
}

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_FrontendHandle frontend = NULL;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;


    NEXUS_3520Settings settings3520;
    unsigned totalFrontends = 0;
    int index=0;
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    int card=0;
    unsigned numChannels, i;
    NEXUS_FrontendCardSettings cardSettings;
#endif

    BKNI_Memset(&g_inputBandBusy, 0, sizeof(g_inputBandBusy));

    g_inputBandBusy[4] = true;    /* By default, we reserve 4 as our streamer input */
    g_inputBandBusy[5] = true;    /* The FPGA code doesn't support PKT5 yet... */

    if (!pConfig->i2c[3]) {
        BDBG_ERR(("i2c[3] required"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    /* Probe Daughercards First */
    #if NEXUS_NUM_FRONTEND_CARD_SLOTS

    /* Find first empty frontend in pConfig */
    for ( ; pConfig->frontend[index] && index < NEXUS_MAX_FRONTENDS; index++ );
    if ( index >= NEXUS_MAX_FRONTENDS )
    {
        return BERR_SUCCESS;
    }

    /* Probe first slot */
    NEXUS_FrontendCard_GetDefaultSettings(&cardSettings);
    cardSettings.i2cDevice = pConfig->i2c[3];               /* Slot 0a */
    cardSettings.isrNumber = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_11_CPU_INTR_SHIFT+32;
    cardSettings.numChannels = 2;                           /* Has 2 channels */
    cardSettings.bitWideSync = true;
    BDBG_MSG(("Probing slot 0"));
    g_frontendCards[card] = NEXUS_FrontendCard_Open(&cardSettings);
    if ( g_frontendCards[card] )
    {
        NEXUS_FrontendCard_GetNumChannels(g_frontendCards[card], &numChannels);
        BDBG_WRN(("Found tuner card in slot 0 with %u channels",numChannels));
        for ( i=0; i < numChannels && index < NEXUS_MAX_FRONTENDS; index++, i++ )
        {
            frontend = NEXUS_FrontendCard_GetChannel(g_frontendCards[card], i);
            if ( NEXUS_NUM_INPUT_BANDS == NEXUS_Platform_P_AllocInputBand(frontend,(i==0)?BFPGA_TsSelect_eSlot0_Ts2:BFPGA_TsSelect_eSlot0_Ts3) )
            {
                BDBG_ERR(("No more input bands for tuner in slot 0"));
                NEXUS_Frontend_Close(frontend);
            }
            if ( NULL == frontend )
            {
                BDBG_ERR(("Unable to init tuner in slot 0"));
                /* but keep going */
            }
            else
            {
                /* register with frontend list */
                pConfig->frontend[totalFrontends++] = frontend;
            }
        }
        card++;
    }
    if ( index >= NEXUS_MAX_FRONTENDS )
    {
        return BERR_SUCCESS;
    }
    if ( card >= NEXUS_NUM_FRONTEND_CARD_SLOTS )
    {
        goto onboard;
    }

    NEXUS_FrontendCard_GetDefaultSettings(&cardSettings);
    cardSettings.i2cDevice = pConfig->i2c[2];               /* Slot 1a */
    cardSettings.isrNumber = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_13_CPU_INTR_SHIFT+32;
    cardSettings.numChannels = 2;                           /* Has 2 channels */
    cardSettings.bitWideSync = true;
    BDBG_MSG(("Probing slot 1"));
    g_frontendCards[card] = NEXUS_FrontendCard_Open(&cardSettings);
    if ( g_frontendCards[card] )
    {
        NEXUS_FrontendCard_GetNumChannels(g_frontendCards[card], &numChannels);
        BDBG_WRN(("Found tuner card in slot 1 with %u channels",numChannels));
        for ( i=0; i < numChannels && index < NEXUS_MAX_FRONTENDS; index++, i++ )
        {
            frontend = NEXUS_FrontendCard_GetChannel(g_frontendCards[card], i);
            if ( NEXUS_NUM_INPUT_BANDS == NEXUS_Platform_P_AllocInputBand(frontend,(i==0)?BFPGA_TsSelect_eSlot1_Ts4:BFPGA_TsSelect_eSlot1_Ts5) )
            {
                BDBG_ERR(("No more input bands for tuner in slot 1"));
                NEXUS_Frontend_Close(frontend);
            }
            if ( NULL == frontend )
            {
                BDBG_ERR(("Unable to init tuner in slot 1"));
                /* but keep going */
            }
            else
            {
                /* register with frontend list */
                pConfig->frontend[totalFrontends++] = frontend;
            }
        }
        card++;
    }
    if ( index >= NEXUS_MAX_FRONTENDS )
    {
        return BERR_SUCCESS;
    }
    if ( card >= NEXUS_NUM_FRONTEND_CARD_SLOTS )
    {
        goto onboard;
    }
    NEXUS_FrontendCard_GetDefaultSettings(&cardSettings);
    cardSettings.i2cDevice = pConfig->i2c[1];               /* Slot 2 */
    cardSettings.isrNumber = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT+32;
    cardSettings.numChannels = 1;                           /* Has 1 channel */
    cardSettings.bitWideSync = true;
    BDBG_MSG(("Probing slot 2"));
    g_frontendCards[card] = NEXUS_FrontendCard_Open(&cardSettings);
    if ( g_frontendCards[card] )
    {
        NEXUS_FrontendCard_GetNumChannels(g_frontendCards[card], &numChannels);
        BDBG_WRN(("Found tuner card in slot 2 with %u channels",numChannels));
        for ( i=0; i < numChannels && index < NEXUS_MAX_FRONTENDS; index++, i++ )
        {
            frontend = NEXUS_FrontendCard_GetChannel(g_frontendCards[card], i);
            if ( NEXUS_NUM_INPUT_BANDS == NEXUS_Platform_P_AllocInputBand(frontend,BFPGA_TsSelect_eSlot2_Ts6) )
            {
                BDBG_ERR(("No more input bands for tuner in slot 2"));
                NEXUS_Frontend_Close(frontend);
            }
            if ( NULL == frontend )
            {
                BDBG_ERR(("Unable to init tuner in slot 2"));
                /* but keep going */
            }
            else
            {
                /* register with frontend list */
                pConfig->frontend[totalFrontends++] = frontend;
            }
        }
        card++;
    }
    #endif

onboard:
    /* Open on-board 3517 */
    NEXUS_Frontend_GetDefault3520Settings(&settings3520);
    settings3520.i2cDevice = pConfig->i2c[1];
    if (!settings3520.i2cDevice) {
        BDBG_ERR(("I2C[1] channel unavailable for 3520"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    settings3520.i2cAddr = 0x0d;
    settings3520.isrNumber = 37;

    g_onboard3517 = frontend = NEXUS_Frontend_Open3520(&settings3520);
    if ( NULL == frontend )
    {
        BDBG_ERR(("Unable to init on-board 3517"));
        /* but keep going */
    }
    else
    {
        /* Open supporting device -- there is a dtt760x connected to the 3517 I2C */
        NEXUS_3520Devices devices;
        NEXUS_Dtt760xSettings dttSettings;
        int j;

        NEXUS_Frontend_Get3520Devices(frontend, &devices);
        NEXUS_Tuner_GetDefaultDtt760xSettings(&dttSettings);
        dttSettings.i2cDevice = NEXUS_Frontend_Get3520MasterI2c(frontend);
        g_onboardDtt760x = devices.tuner = NEXUS_Tuner_OpenDtt760x(&dttSettings);
        BDBG_ASSERT(NULL != devices.tuner);
        NEXUS_Frontend_Set3520Devices(frontend, &devices);
        j = NEXUS_Platform_P_AllocInputBand(frontend,BFPGA_TsSelect_eMb3517);
        if ( NEXUS_NUM_INPUT_BANDS == j )
        {
            BDBG_ERR(("No more slots for onboard 3517"));
            NEXUS_Frontend_Close(frontend);
            g_onboard3517 = NULL;
            NEXUS_Tuner_Close(devices.tuner);
            g_onboardDtt760x = NULL;
        }
        else
        {
            /* register with frontend list */
            pConfig->frontend[totalFrontends++] = frontend;
        }
    }
    /* End on-board 3517 */

    #ifdef NEXUS_FRONTEND_4500
    {
        NEXUS_4500Settings settings4500;
        /* Open on-board 4500 */
        NEXUS_Frontend_GetDefault4500Settings(&settings4500);
        settings4500.i2cDevice = pConfig->i2c[1];
        if (!settings4500.i2cDevice) {
            BDBG_ERR(("I2C[1] channel unavailable for 4500"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        settings4500.i2cAddr = 0x10;
        settings4500.isrNumber = BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_6_CPU_INTR_SHIFT+32;
        settings4500.diseqcI2cAddr = 0x09;  /* ISL6425 I2C address on BCM97400 */
        settings4500.diseqcChip = 6425;
    
        g_onboard4500 = frontend = NEXUS_Frontend_Open4500(&settings4500);
        if ( NULL == frontend )
        {
            BDBG_ERR(("Unable to init on-board 4500"));
            /* but keep going */
        }
        else
        {
            int j;
    
            j = NEXUS_Platform_P_AllocInputBand(frontend,BFPGA_TsSelect_eMb4500);
            if ( NEXUS_NUM_INPUT_BANDS == j )
            {
                BDBG_ERR(("No more slots for onboard 4500"));
                NEXUS_Frontend_Close(frontend);
                g_onboard4500 = NULL;
            }
            else
            {
                /* register with frontend list */
                pConfig->frontend[totalFrontends++] = frontend;
            }
        }
        /* End on-board 4500 */
    }
    #endif
    BDBG_WRN(("Total frontends %u",totalFrontends));

    #if BDBG_DEBUG_BUILD
    if (g_NEXUS_platformHandles.fpgaHandle)
    {
        BFPGA_DumpConfiguration(g_NEXUS_platformHandles.fpgaHandle);
    }
    #endif

    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    #if NEXUS_NUM_FRONTEND_CARD_SLOTS
    int i;
    for ( i = NEXUS_NUM_FRONTEND_CARD_SLOTS-1; i >= 0; i-- )
    {
        if ( g_frontendCards[i] )
        {
            NEXUS_FrontendCard_Close(g_frontendCards[i]);
        }
    }
    #endif
    if ( g_onboard3517 )
    {
        NEXUS_Frontend_Close(g_onboard3517);
        g_onboard3517 = NULL;
    }
    if ( g_onboardDtt760x )
    {
        NEXUS_Tuner_Close(g_onboardDtt760x);
        g_onboardDtt760x = NULL;
    }
    if ( g_onboard4500 )
    {
        NEXUS_Frontend_Close(g_onboard4500);
        g_onboard4500 = NULL;
    }
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
    *pInputBand = NEXUS_InputBand_e4;
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
