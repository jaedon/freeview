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
* $brcm_Log: /nexus/platforms/97425/src/nexus_platform_frontend.c $
*
* 82   12/14/12 11:23a ayxiang
* SW7425-4339: fix the 3128 mtsif setting when under 3383+3128 mix mode
*
* 81   12/6/12 5:56p mphillip
* SW7429-404: Add support for the 7429 + 4538 SATIP board
*
* 80   11/19/12 5:23p mphillip
* SW7425-4035: Allow early exist from i2c address probe, and provide a
*  compile-time option to skip the search altogether
*
* 79   11/9/12 4:44p mphillip
* SW7425-4035: Clean up 4538 initialization
*
* 78   11/9/12 1:56p vishk
* SW3461-206: Add front end configuration settings for external, fixed-
*  gain amp (FGA) and optional FGA-bypass for 3128 frontend.
*
* 77   11/5/12 3:43p vishk
* SW7425-4119: Remove enableFec from NEXUS_3128Settings and make it a run
*  time setting using outputMode parameter in NEXUS_3128ConfigSettings
*  struct.
*
* 76   11/1/12 5:46p mphillip
* SW7425-2030: Update 4528 i2c configuration to be generic for all Bx and
*  higher revisions
*
* 75   10/24/12 3:50p mphillip
* SW7425-4035: Merge 4538 support branch to main
*
* SW7425-4035/1   10/4/12 5:19p mphillip
* SW7425-4035: Initial 4538 support
*
* 74   8/31/12 10:00a erickson
* SW7435-328: remove unused config.spi[] code
*
* 73   7/26/12 5:33p mward
* SW7425-2690:  Allow a binary built with NEXUS_USE_7425_SV_BOARD=y to
*  also run on 97425 (or 97435) C board, don't assert when 3405 does not
*  ack, just run without frontends.
*
* 72   7/2/12 2:40p rjlewis
* SW7425-3410: Fixed function definition.
*
* 71   6/19/12 12:44p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
*
* 70   6/11/12 11:20p jessem
* SW7425-3225: Update 97422 VMS support.
*
* 69   6/1/12 11:47a mward
* SW7425-3053: eliminate warning for unused variable idx.
*
* 68   5/31/12 10:30a nickh
* SW7425-2752: Fix compile issue
*
* 67   5/29/12 6:26p jtna
* SW7425-3053: try a better fix. previous fix did not silence coverity
*  and broke compile
*
* 66   5/18/12 11:59a vishk
* SW7358-217: Move from edge triggered to level triggered interrupt for
*  DOCSIS_PLUS_BCM3128 platform as well.
*
* 65   5/15/12 3:46p mward
* SW7425-2752:  Compiler warning for unused variable.
*
* 64   5/15/12 3:26p jessem
* SW7425-3053: Fixed coverity issue 37726.
*
* 63   5/14/12 4:15p jtna
* SW7425-2752: set NEXUS_FrontendUserParams.isMtsif and .param1 to work
*  correctly with new MTSIF mapping
*
* 62   5/9/12 4:52p erickson
* SW7231-748: fix warning
*
* 61   5/9/12 12:35p ayxiang
* SW7435-157: correct the parameter assigned to frontend which will be
*  used to set parser's input.
*
* 60   5/7/12 5:10p jtna
* SW7425-2752: fix incorrect 3128 OOB channel number set with
*  NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT=y mode. major clean up
*  of channel number and frontend number index book-keeping.
*
* 59   5/7/12 12:40p jtna
* SW7425-2752: add comments for #if-#else spaghetti
*
* 58   4/27/12 3:02p mward
* SW7435-153: warning: unused variable 'pConfig' when B_REFSW_DEBUG=n.
*
* 57   4/13/12 6:04p vishk
* SW7358-217: Move from edge triggered to level triggered interrupt.
*
* 56   3/30/12 7:24p mphillip
* SW7425-2030: Enable all 16 tuners (4528_0 are frontends 0..7, 4528_1
*  are frontends 8..15)
*
* 55   3/20/12 4:49p erickson
* SW7425-2677: change non-ERR to WRN
*
* 54   3/16/12 3:16p mphillip
* SW7425-2030: Determine ext_irq from bchp headers instead of hard-coded
*  numbers
*
* 53   3/15/12 12:21p mphillip
* SW7425-2030: Invert tuners on B1 so input bands match
*
* 52   3/8/12 10:22a mphillip
* SW7425-2030: Update IRQs for B1 and 4528
*
* 51   3/7/12 6:02p nickh
* SW7425-2081: Add get/set LTSID functions and check for conflicting
*  frontends on the same LTSID number
*
* 50   2/24/12 2:15p vishk
* SW3128-120, SW3128-110: Add Nyquist filter selection and add frontend
*  gain, equalizer gain and digital agc.
*
* 49   2/24/12 12:23p nickh
* SW7425-2235: Close device handle only if opened
*
* 48   2/23/12 6:11p gmohile
* SW7425-2473 : Set handle to NULL after uninit
*
* 47   2/9/12 11:50a nickh
* SW7425-2307: Add ChipId for 3255 OOB frontend
*
* 46   12/20/11 3:58p mphillip
* SW7425-2030: Update i2c configuration for 7425satipsw b1
*
* 45   12/19/11 5:40p nickh
* SW7425-2023: Add frontend Parser Band remapping
*
* 44   12/18/11 7:08p prasadv
* SW3128-56: Clean up. Remove 3124 support. Auto detection has been added
*  to take care of 312x.
*
* 43   12/16/11 2:46p vishk
* SW3128-92: Add FW auto detection support Add probe api to get the chip
*  Id.
*
* 42   12/1/11 4:13p mphillip
* SW7425-1304: Change 4528 to HAB
*
* 41   11/15/11 6:13p vishk
* SW7425-1598: Add support for 3128B0/C0 Remove compiler warning for SV
*  board.
*
* 40   10/31/11 1:53p vishk
* SW7425-1598: Add support for 3128B0/C0
*
* 39   10/28/11 5:28p vishk
* SW7425-1598: Add support for 3128B0/C0
*
* 38   10/28/11 1:51p jessem
* SW7425-1598: Fixed compile warnings for SV board.
*
* 37   10/27/11 3:09p vishk
* SW7425-1598: Add support for 3128B0/C0
*
* 36   10/26/11 2:27p jessem
* SW7425-1350: Fixed compile warnings.
*
* 35   10/20/11 5:01p spothana
* SW7425-1350: Update the debug messages.
*
* 34   10/20/11 3:14p spothana
* SW7425-1350 : Fix the compilation errors.
*
* 33   10/19/11 10:27p spothana
*  SW7425-1350: Initialize the docsis device and then associate the
*  frontend handles with docsis channels based on the platform frontend
*  initialization sequence
*
* 32   10/18/11 12:08p prasadv
* SW3128-56: Use the correct compile time flag for 3124
*
* 31   10/17/11 7:31p prasadv
* SW3128-56: Add 3124 Support
*
* 30   10/13/11 3:47p hongtaoz
* SW3128-46: fixed compile error for 97425 SV board;
*
* 29   10/11/11 6:53p vishk
* SW3128-46: add support for BCM3124 devices
*
* 28   10/10/11 11:49a nickh
* SW7425-1248: Add frontend Card support
*
* 27   10/7/11 4:15p agin
* SWNOOS-489:  Fixed error message.
*
* 26   9/15/11 10:57a jessem
* SW7425-1248: Updated I2C channel for VMS frontend tuner.
*
* 25   9/14/11 2:31p jessem
* SW7425-1248: Corrected I2C channels for Ax.
*
* 24   9/13/11 7:00p jessem
* SW7425-1248: Updated pinmux and I2C usage for B0.
*
* 23   8/31/11 6:21p nickh
* SW7425-1129: Increase default number of 3383 + 3128 frontends to 4 each
*
* 22   7/22/11 2:45p jessem
* SW7425-964: Added code to reset ISL642 for 97425 DBS.
*
* 21   7/7/11 7:53a spothana
* SW7425-763 : Assign chip id in the front end user parameters for
*  differentiation between different QAM chips at the app level.
*
* 20   7/6/11 5:55p spothana
* SW7425-622: Use the NEXUS_MAX_3255_ADSCHN as the index for OOB by
*  default.
*
* 19   6/13/11 12:17p spothana
* SW7425-622: Fix the DOCSIS OOB index.
*
* 18   6/13/11 12:05p spothana
* SW7425-622: Adding simul usage of DOCSIS and BCM3128 tuners.
*
* 17   5/11/11 3:19p vishk
* SW3128-2: Added support to configure TM_MTSIF_CTRL - Transport Control.
*
* 16   4/19/11 4:29p spothana
* SW7425-369: Correct the number of DOCSIS InBand channels.
*
* 15   4/19/11 11:26a vishk
* SW7425-369: fixed compile error; Fix indentations.
*
* 14   4/19/11 12:41a hongtaoz
* SW7425-369: fixed compile error;
*
* 13   4/18/11 4:50p spothana
* SW7425-369: Add NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT and
*  NEXUS_PLATFORM_DOCSIS_IB_SUPPORT
*
* 12   4/15/11 4:45p vishk
* SW7422-413: Nexus support for the 3128 OOB core for 7422 board
*
* 11   4/14/11 4:02p hongtaoz
* SW7425-344, SW7125-769: clean uninit platform frontend;
*
* 10   4/12/11 4:46p spothana
* SW7425-331: BCM97425 VMS tuning not working.
*
* 9   4/8/11 11:53a vishk
* SW7425-313 : Add support for VMSSFF board with 3128 frontend.
*
* 8   3/28/11 1:19p nickh
* SW7422-412: Add 7422 DBS support
*
* 7   3/11/11 12:51p hongtaoz
* SW7422-171: remove compile warning;
*
* 6   3/9/11 3:02p hongtaoz
* SW7422-171: compile 3128 only if board supports it;
*
* 5   3/8/11 4:02p VISHK
* SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
*  board) plug on 7422 board  Add support for 3128 daughter card for
*  97425 platform.
*
* 4   3/1/11 6:47p vishk
* SW7422-272: Add nexus gpio support for frontend 4506 tuners
*
* 3   1/28/11 3:19p mward
* SW7125-769:  Warning for unused. Also stub
*  NEXUS_Platform_OpenFrontend() when !NEXUS_HAS_FRONTEND.
*
* 2   1/27/11 6:58p mward
* SW7125-769:  Stub NEXUS_Platform_OpenFrontend().
*
* 1   1/17/11 5:46p hongtaoz
* SW7425-9: updated 7425 pin mux setting for tuner inputs; differentiate
*  SV vs VMS board;
*
* 6   1/5/11 6:43p spothana
* SW7422-96: Differentiate between 7422 SV and VMS boards using
*  NEXUS_USE_7422_SV_BOARD.
*
* 5   1/4/11 11:09p hongtaoz
* SW7425-9: add frontend support for 7425;
*
* 4   12/7/10 6:16p spothana
* SW7422-96: Remove references to 3255 for now
*
* 3   12/2/10 2:05p spothana
* SW7422-96: Add cable tuner support
*
* 2   11/17/10 7:13p vsilyaev
* SW7405-4996: Use strcmp function for string comparison
*
* SW7405-4996/1   11/17/10 3:09p calvinho
* SW7405-4996: Update string compare function calls
*
* 1   9/27/10 6:05p nickh
* SW7422-10: Add initial version of the frontend support
*
***************************************************************************/

#include "nexus_types.h"
#include "nexus_platform.h"
#include "priv/nexus_core.h"
#include "nexus_frontend.h"
#include "nexus_platform_features.h"
#include "nexus_platform_priv.h"
#include "nexus_base.h"
#include "nexus_input_band.h"
#include "bchp_gio.h"

#if NEXUS_PLATFORM_4528_DBS
#include "nexus_spi.h"
#include "nexus_frontend_4528.h"
#endif
#if NEXUS_PLATFORM_4538_DBS
#include "nexus_spi.h"
#include "nexus_frontend_4538.h"
#endif
#if NEXUS_PLATFORM_7425_DBS || NEXUS_PLATFORM_7422_DBS
#include "nexus_frontend_4506.h"
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_PLATFORM_DB4506)
#include "nexus_frontend_4506.h"
#endif
#include "humax_i2c_bus_index.h"
#endif

BDBG_MODULE(nexus_platform_frontend);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_PLATFORM_DB4506)
static NEXUS_FrontendHandle g_frontend4506[NEXUS_MAX_FRONTENDS] = {NULL};
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    unsigned i;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if(g_frontend4506[i]) {
            NEXUS_Frontend_Close(g_frontend4506[i]);
            g_frontend4506[i] = NULL;
        }
    }
}
#elif NEXUS_FRONTEND_45216 /* TODO_ILEE */

#define USE_SPI_FRONTEND


/* Assume a 45216 daughtercard if 45216 is built on 7366 */
#define NEXUS_PLATFORM_7366_EXTERNAL_45216 1
#define NEXUS_PLATFORM_7366_EXTERNAL_45216_MTSIF0 1
/*#define NEXUS_PLATFORM_7366_EXTERNAL_45216_MTSIF1 1*/

#if NEXUS_PLATFORM_7366_EXTERNAL_45216
#include "nexus_frontend_45216.h"
#include "bchp_sun_top_ctrl.h"


#define NEXUS_PLATFORM_45208_GPIO_INTERRUPT 77
#define NEXUS_PLATFORM_45208_GPIO_RESET 75
#endif
#define NEXUS_PLATFORM_45208_I2C 3

#define NEXUS_PLATFORM_45208_FRONTEND_STRING "45216"
#define NEXUS_PLATFORM_45208_FRONTEND_I2C_ADDRESS 0x68
#define NEXUS_PLATFORM_45208_MTSIF_OFFSET 2


#if NEXUS_PLATFORM_45208_GPIO_RESET
static NEXUS_GpioHandle gpioHandle = NULL;
#endif
static NEXUS_GpioHandle gpioHandleInt = NULL;

#ifdef USE_SPI_FRONTEND

#include "nexus_spi.h"
static NEXUS_SpiHandle g_3461spi[NEXUS_NUM_SPI_CHANNELS];
#endif

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_FrontendDeviceHandle device;
    NEXUS_FrontendUserParameters userParams;
    unsigned i=0;

#ifdef USE_SPI_FRONTEND
    NEXUS_SpiSettings spiSettings;
#endif

#if NEXUS_PLATFORM_7366_EXTERNAL_45216
    {
        NEXUS_FrontendDevice45216OpenSettings deviceSettings;
        NEXUS_GpioSettings gpioSettings;

        NEXUS_FrontendDevice_GetDefault45216OpenSettings(&deviceSettings);

#if NEXUS_PLATFORM_45208_GPIO_RESET
        BDBG_MSG(("Setting GPIO %d high", NEXUS_PLATFORM_45208_GPIO_RESET));
        /* GPIO 3 is connected to the 4548 reset, so set it high */
        NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettings);
        gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
        gpioSettings.value = NEXUS_GpioValue_eHigh;
        gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
        gpioSettings.interrupt.callback = NULL;
        gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, NEXUS_PLATFORM_45208_GPIO_RESET, &gpioSettings);
        BDBG_ASSERT(NULL != gpioHandle);

        BKNI_Sleep(500);
#endif



#ifdef USE_SPI_FRONTEND
    NEXUS_Spi_GetDefaultSettings(&spiSettings);

	spiSettings.clockActiveLow = false;/* true; */
	spiSettings.baud = 6750000;

	BDBG_WRN(("########### Added spiSettings [3]"));
    g_3461spi[0] = NEXUS_Spi_Open(0, &spiSettings);
    if (!g_3461spi[0]) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    deviceSettings.i2cDevice = NULL;
    deviceSettings.spiDevice = g_3461spi[0];
    deviceSettings.spiAddr = 0x20;
#if 0 /* TODO_ILEE */
	deviceSettings.diseqc.i2cAddr = 0x08;
	deviceSettings.diseqc.i2cDevice = pConfig->i2c[3];

		/* Configure DiSEqC. */
		BDBG_ERR(("############ deviceSettings.diseqc.i2cAddr [0x%x]", deviceSettings.diseqc.i2cAddr));
		BDBG_ERR(("############ deviceSettings.diseqc.i2cDevice [0x%x]", deviceSettings.diseqc.i2cDevice));
#endif


#else
    if (!pConfig->i2c[3]) {
            BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
            /* rc = BERR_NOT_INITIALIZED; goto done; */
    }
        deviceSettings.i2cDevice = pConfig->i2c[NEXUS_PLATFORM_45208_I2C];
        deviceSettings.i2cAddr = NEXUS_PLATFORM_45208_FRONTEND_I2C_ADDRESS;
#endif

        BDBG_MSG(("Setting up interrupt on GPIO %d",NEXUS_PLATFORM_45208_GPIO_INTERRUPT));
        NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettings);
        gpioSettings.mode = NEXUS_GpioMode_eInput;
        gpioSettings.interruptMode = NEXUS_GpioInterrupt_eLow;
        gpioHandleInt = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, NEXUS_PLATFORM_45208_GPIO_INTERRUPT, &gpioSettings);
        BDBG_ASSERT(NULL != gpioHandleInt);
        deviceSettings.gpioInterrupt = gpioHandleInt;

		deviceSettings.mtsif[0].clockRate = 75000000; /* Initialize at 75 MHz. */
		deviceSettings.mtsif[1].clockRate = 75000000; /* Initialize at 75 MHz. */

        BDBG_MSG(("Opening %s device",NEXUS_PLATFORM_45208_FRONTEND_STRING));
        device = NEXUS_FrontendDevice_Open45216(0, &deviceSettings);

        if (device) {
            NEXUS_FrontendDeviceCapabilities capabilities;
            NEXUS_FrontendDevice_GetCapabilities(device, &capabilities);
            BDBG_MSG(("Opening %d %s frontends",capabilities.numTuners,NEXUS_PLATFORM_45208_FRONTEND_STRING));
            for (i=0; i < capabilities.numTuners ; i++)
            {
                NEXUS_45216FrontendSettings channelSettings;
                channelSettings.device = device;
                channelSettings.channelNumber = i;
                pConfig->frontend[i] = NEXUS_Frontend_Open45216(&channelSettings);
                if ( NULL == (pConfig->frontend[i]) )
                {
                    BDBG_ERR(("Unable to open %s demod %d (as frontend[%d])",NEXUS_PLATFORM_45208_FRONTEND_STRING,i,i));
                    continue;
                }
                BDBG_MSG(("%sfe: %d(%d):%p",NEXUS_PLATFORM_45208_FRONTEND_STRING,i,i,pConfig->frontend[i]));
                NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
                userParams.isMtsif = true;
                userParams.param1 = channelSettings.channelNumber + NEXUS_PLATFORM_45208_MTSIF_OFFSET;
                userParams.pParam2 = NULL;
                BDBG_MSG(("%sfe: %d(%d):%p: (%s,%i)",NEXUS_PLATFORM_45208_FRONTEND_STRING,i,i,pConfig->frontend[i],userParams.isMtsif ? "mtsif" : "not mtsif",userParams.param1));
                NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
            }
        } else {
            BDBG_ERR(("Unable to open %s device",NEXUS_PLATFORM_45208_FRONTEND_STRING));
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
    }


#endif
    return BERR_SUCCESS;
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
        if (pConfig->frontend[i]) {
            tempHandle = NEXUS_Frontend_GetDevice(pConfig->frontend[i]);
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
            NEXUS_FrontendDevice_Close(deviceHandles[i]);
            deviceHandles[i] = NULL;
        }
    }
#if NEXUS_PLATFORM_7366_EXTERNAL_45216
    if(gpioHandleInt)
    {
        NEXUS_Gpio_Close(gpioHandleInt);
        gpioHandleInt = NULL;
    }
#if NEXUS_PLATFORM_45208_GPIO_RESET
    if(gpioHandle)
    {
        NEXUS_Gpio_Close(gpioHandle);
        gpioHandle = NULL;
    }
#endif

#ifdef USE_SPI_FRONTEND
    for (i=0; i<NEXUS_NUM_SPI_CHANNELS; i++)
    {
        if (g_3461spi[i])
        {
            BDBG_MSG(("deviceHandles[%d] = 0x%x", i, deviceHandles[i]));
            NEXUS_Spi_Close(g_3461spi[i]);
            g_3461spi[i] = NULL;
        }
    }
#endif

#endif

    return;
}
#else	/* defined(CONFIG_PLATFORM_DB4506) */

#if NEXUS_PLATFORM_7422_CABLE || NEXUS_PLATFORM_7425_CABLE

#if defined(NEXUS_USE_7425_SV_BOARD) || defined(NEXUS_USE_7422_SV_BOARD)
#include "nexus_frontend_31xx.h"
#else
    #if NEXUS_USE_7425_VMS_SFF || NEXUS_USE_7422_VMS_SFF
        #include "nexus_platform_cable_frontend.h"
        #if defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)
        #include "nexus_frontend_3255.h"
        #include "nexus_frontend_3128.h"
        #elif defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT)
        #include "nexus_frontend_3255.h"
        #else
        #include "nexus_frontend_3128.h"
        #endif
        #if !defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT) && defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
        #include "nexus_frontend_3255.h"
        #endif
    #else
    #include "nexus_frontend_31xx.h"
    #endif
#endif
#endif


#if NEXUS_PLATFORM_7422_CABLE || NEXUS_PLATFORM_7425_CABLE
    #if NEXUS_USE_7425_VMS_SFF || NEXUS_USE_7422_VMS_SFF
        static NEXUS_GpioHandle gpioHandle = NULL;
        static unsigned ltsidCount[NEXUS_MAX_FRONTENDS];
    #if defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
        static NEXUS_3255DeviceHandle st3255DeviceHandle;
    #endif
    #else
        #if NEXUS_NUM_FRONTEND_CARD_SLOTS
            static NEXUS_GpioHandle gpioHandle = NULL;
        #endif
        static NEXUS_GpioHandle tunerGpio[NEXUS_MAX_FRONTENDS];
    #endif
#endif

#if !defined(NEXUS_USE_7425_VMS_SFF) && !defined(NEXUS_USE_7422_VMS_SFF) && !defined(NEXUS_PLATFORM_4528_DBS) && !defined(NEXUS_PLATFORM_4538_DBS)
NEXUS_Error bcm3405_init( void )
{
    NEXUS_Error rc;
    uint8_t data[]={0x3,0x0,    0x4,0x1a,   0x5,0x0,    0x6,0x4,
                    0x7,0x0,    0x8,0x13,   0x9,0x0,    0xa,0x40,
                    0xb,0x40,   0xc,0xd,    0xe,0xff,   0xf,0xff,
                    0x10,0xff,  0x11,0x77,  0x12,0x63,  0x13,0xa0,
                    0x14,0x9d,  0x15,0xaa,  0x16,0x45,  0x17,0x0,
                    0x18,0x88,  0x19,0xf0,  0x1a,0xf0,  0x1b,0xf0,
                    0x1c,0xf0,  0x1d,0xf0,  0x1e,0xf0,  0x1f,0xf0,
                    0x20,0xf0,  0x21,0xf0,  0x24,0xf0};
    uint8_t i;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;

    for(i=0; i < sizeof(data); i+=2)
    {
        rc = NEXUS_I2c_Write(pConfig->i2c[NEXUS_I2C_CHANNEL_LNA], 0x64, data[i], &data[i+1], 1);
    }
    return rc;
}
#endif

#if NEXUS_PLATFORM_7425_DBS || NEXUS_PLATFORM_7422_DBS
#include "nexus_frontend_4506.h"
#endif

#if NEXUS_PLATFORM_4528_DBS
#include "bchp_hif_cpu_intr1.h"
#define ISL9492_CH0_I2C_ADDR 0x08
#define ISL9492_CH1_I2C_ADDR 0x09
static   NEXUS_GpioHandle gpioHandle = NULL;

#if (BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)
/* 7425 SATIPSW B1 values */
#define I2C_DEVICE_VOLTAGE_REG_CH 1
#define I2C_4528_INDEX_1 3
#define I2C_4528_INDEX_2 4
#else
/* 7425 SATIPSW A1 values */
#define I2C_DEVICE_VOLTAGE_REG_CH 4
#define I2C_4528_INDEX_1 1
#define I2C_4528_INDEX_2 2
#endif

#define NUM_4528_CHANNELS_PER 8
#define I2C_4528_ADDRESS_1 0x68
#define I2C_4528_ADDRESS_2 0x68
#define EXT_4528_IRQ_1 BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_00_CPU_INTR_SHIFT
#define EXT_4528_IRQ_2 BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_EXT_IRQ_01_CPU_INTR_SHIFT

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0, j=0;
    NEXUS_GpioSettings gpioSettings;
    NEXUS_SpiSettings spiSettings;
    NEXUS_SpiHandle mixerPllSpi;
    NEXUS_Error rc;
    uint32_t data;
    NEXUS_4528Settings st4528Settings;
    NEXUS_FrontendUserParameters userParams;

    if ((!pConfig->i2c[I2C_4528_INDEX_1]) && (!pConfig->i2c[I2C_4528_INDEX_2]) ){
        BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        return BERR_NOT_INITIALIZED;
    }

    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettings);
    gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
    gpioSettings.value = NEXUS_GpioValue_eHigh;
    gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
    gpioSettings.interrupt.callback = NULL;
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 75, &gpioSettings);
    BDBG_ASSERT(NULL != gpioHandle);
    NEXUS_Gpio_Close(gpioHandle);
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 76, &gpioSettings);
    BDBG_ASSERT(NULL != gpioHandle);

    /* ADF4350 PLL frequency synthesizers use Spi Channel 0 */
    NEXUS_Spi_GetDefaultSettings(&spiSettings);
    spiSettings.clockActiveLow = false;
    spiSettings.txLeadingCapFalling = false;
    mixerPllSpi = NEXUS_Spi_Open(0, &spiSettings);
    if (!mixerPllSpi)
    {
        BDBG_ERR(("Unable to open SPI For mixer PLL"));
    }
    /* register 5 */
    data = 0x05005800;
    rc = NEXUS_Spi_Write(mixerPllSpi, (uint8_t *)&data, sizeof(data));
    if (rc)
        BDBG_ERR(("NEXUS_Spi_Write reg 5 error\n"));

    /* register 4 */
    data = 0x6c619100;
    rc = NEXUS_Spi_Write(mixerPllSpi, (uint8_t *)&data, sizeof(data));
    if (rc)
        BDBG_ERR(("NEXUS_Spi_Write reg 4 error\n"));

    /* register 3 */
    data = 0xb3040000;
    rc = NEXUS_Spi_Write(mixerPllSpi, (uint8_t *)&data, sizeof(data));
    if (rc)
        BDBG_ERR(("NEXUS_Spi_Write reg 3 error\n"));

    /* register 2 */
    data = 0xc28e0100;
    rc = NEXUS_Spi_Write(mixerPllSpi, (uint8_t *)&data, sizeof(data));
    if (rc)
        BDBG_ERR(("NEXUS_Spi_Write reg 2 error\n"));

    /* register 1 */
    data = 0x39ad0008;
    rc = NEXUS_Spi_Write(mixerPllSpi, (uint8_t *)&data, sizeof(data));
    if (rc)
        BDBG_ERR(("NEXUS_Spi_Write reg 1 error\n"));

    /* register 0 */
    data = 0x2826dc02;
    rc = NEXUS_Spi_Write(mixerPllSpi, (uint8_t *)&data, sizeof(data));
    if (rc)
        BDBG_ERR(("NEXUS_Spi_Write reg 0 error\n"));

    NEXUS_Frontend_GetDefault4528Settings(&st4528Settings);

    st4528Settings.i2cDevice = pConfig->i2c[I2C_4528_INDEX_1];
    st4528Settings.i2cAddr = I2C_4528_ADDRESS_1;
    st4528Settings.isrNumber = EXT_4528_IRQ_1;

    for (i=0; i <  NUM_4528_CHANNELS_PER; i++)
    {
        st4528Settings.channelNumber = i;
        pConfig->frontend[i] = NEXUS_Frontend_Open4528(&st4528Settings);
        if ( NULL == (pConfig->frontend[i]) )
        {
            BDBG_ERR(("Unable to open onboard 4528 tuner/demodulator %d",i));
        }
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.isMtsif = true;
        userParams.param1 = userParams.isMtsif ? st4528Settings.channelNumber : NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }

    st4528Settings.i2cDevice = pConfig->i2c[I2C_4528_INDEX_2];
    st4528Settings.i2cAddr = I2C_4528_ADDRESS_2;
    st4528Settings.isrNumber = EXT_4528_IRQ_2;

    for (j=0; j < NUM_4528_CHANNELS_PER; j++, i++)
    {
        st4528Settings.channelNumber = j;
        pConfig->frontend[i] = NEXUS_Frontend_Open4528(&st4528Settings);
        if ( NULL == (pConfig->frontend[i]) )
        {
            BDBG_ERR(("Unable to open onboard 4528 tuner/demodulator %d",j));
        }
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.isMtsif = true;
        userParams.param1 = userParams.isMtsif ? st4528Settings.channelNumber : NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }

    /* Tone generated internally according to EXTMpin */
    data = 0x20;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00, (const uint8_t *)&data, 1);
    data = 0x44;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00, (const uint8_t *)&data, 1);

    /* Dynamic current limit */
    data = 0x40;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00,(const uint8_t *)&data, 1);
    /* Internal linear regulator is turned on and boost circuit is on */
    data = 0x78;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH0_I2C_ADDR, 0x00,(const uint8_t *)&data, 1);

    /* Tone generated internally according to EXTMpin */
    data = 0x20;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00, (const uint8_t *)&data, 1);
    data = 0x44;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00, (const uint8_t *)&data, 1);

    /* Dynamic current limit */
    data = 0x40;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00,(const uint8_t *)&data, 1);
    /* Internal linear regulator is turned on and boost circuit is on */
    data = 0x78;
    NEXUS_I2c_Write(g_NEXUS_platformHandles.config.i2c[I2C_DEVICE_VOLTAGE_REG_CH], ISL9492_CH1_I2C_ADDR, 0x00,(const uint8_t *)&data, 1);

    return 0;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i]) {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }

    if(gpioHandle)
    {
        NEXUS_Gpio_Close(gpioHandle);
        gpioHandle = NULL;
    }
    return;
}
#elif NEXUS_PLATFORM_4538_DBS
#include "bchp_hif_cpu_intr1.h"
#define ISL9492_CH0_I2C_ADDR 0x08
#define ISL9492_CH1_I2C_ADDR 0x09
static   NEXUS_GpioHandle gpioHandle = NULL;
/* Uncomment the following define in order to disable the i2c address search */
/*#define NEXUS_PLATFORM_BYPASS_I2C_ADDRESS_SEARCH 1*/

#define I2C_DEVICE_VOLTAGE_REG_CH 1
#define I2C_4538_INDEX_1 4
#define I2C_4538_INDEX_2 4

#define NUM_4538_CHANNELS_PER 8
#define I2C_4538_ADDRESS_1 0x6a
#define EXT_4538_IRQ_1 16

#include "breg_i2c.h"
#include "priv/nexus_i2c_priv.h"

static bool NEXUS_Platform_P_Is4538(NEXUS_I2cHandle i2cDevice, uint16_t i2cAddr)
{
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[5];
    uint16_t chipId=0;
    uint8_t subAddr;

    i2cHandle = NEXUS_I2c_GetRegHandle(i2cDevice, NULL);
    BDBG_MSG(("i2c handle: %p, i2caddr: 0x%x",i2cHandle,i2cAddr));
    buf[0]= 0x0;
    subAddr = 0x1;
    BREG_I2C_WriteNoAddr(i2cHandle, i2cAddr, (uint8_t *)&subAddr, 1);
    BREG_I2C_ReadNoAddr(i2cHandle, i2cAddr, buf, 1);
    chipId = buf[0];

    subAddr = 0x2;
    BREG_I2C_WriteNoAddr(i2cHandle, i2cAddr, (uint8_t *)&subAddr, 1);
    BREG_I2C_ReadNoAddr(i2cHandle, i2cAddr, buf, 1);
    chipId = (chipId <<8) | buf[0];

    BDBG_MSG(("chip ID = 0x%04x", chipId));

    return chipId == 0x4538;
}


NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0;
    NEXUS_4538Settings st4538Settings;
    NEXUS_FrontendUserParameters userParams;
    uint16_t i2cAddr = I2C_4538_ADDRESS_1;

    if ((!pConfig->i2c[I2C_4538_INDEX_1]) && (!pConfig->i2c[I2C_4538_INDEX_2]) ){
        BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        return BERR_NOT_INITIALIZED;
    }

    BDBG_MSG(("Checking i2c: 0x%02x",I2C_4538_ADDRESS_1));
    if (!NEXUS_Platform_P_Is4538(pConfig->i2c[I2C_4538_INDEX_1],I2C_4538_ADDRESS_1)) {
#if NEXUS_PLATFORM_BYPASS_I2C_ADDRESS_SEARCH
        BDBG_ERR(("Unable to locate 4538 at 0x%02x",I2C_4538_ADDRESS_1));
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
#else
        int ix;
        for (ix=0x68; ix<=0x6f; ix++) {
            BDBG_MSG(("Checking i2c: 0x%02x",ix));
            if (ix != I2C_4538_ADDRESS_1 && NEXUS_Platform_P_Is4538(pConfig->i2c[I2C_4538_INDEX_1],ix)) {
                i2cAddr = ix;
                BDBG_MSG(("Found 4538 at 0x%02x",ix));
                break;
            }
        }
        if (i2cAddr == I2C_4538_ADDRESS_1) {
            BDBG_ERR(("Unable to locate 4538"));
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
#endif
    }

    NEXUS_Frontend_GetDefault4538Settings(&st4538Settings);

    st4538Settings.i2cDevice = pConfig->i2c[I2C_4538_INDEX_1];
    st4538Settings.i2cAddr = i2cAddr;
    st4538Settings.isrNumber = EXT_4538_IRQ_1;
    st4538Settings.gpioInterrupt = NULL;

    for (i=0; i <  NUM_4538_CHANNELS_PER; i++)
    {
        st4538Settings.channelNumber = i;
        pConfig->frontend[i] = NEXUS_Frontend_Open4538(&st4538Settings);
        if ( NULL == (pConfig->frontend[i]) )
        {
            BDBG_ERR(("Unable to open onboard 4538 tuner/demodulator %d",i));
        }
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.isMtsif = true;
        userParams.param1 = userParams.isMtsif ? st4538Settings.channelNumber : NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }

    return NEXUS_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i=0;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if (pConfig->frontend[i]) {
            NEXUS_Frontend_Close(pConfig->frontend[i]);
            pConfig->frontend[i] = NULL;
        }
    }

    if(gpioHandle)
    {
        NEXUS_Gpio_Close(gpioHandle);
        gpioHandle = NULL;
    }
    return;
}


#elif NEXUS_PLATFORM_7422_CABLE || NEXUS_PLATFORM_7425_CABLE
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
 static NEXUS_FrontendCardHandle g_frontendCards[NEXUS_NUM_FRONTEND_CARD_SLOTS];
#endif
NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    unsigned i = 0;
    NEXUS_Error rc = NEXUS_SUCCESS;
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    unsigned idx = 0;
#endif
#if defined(NEXUS_USE_7425_SV_BOARD) || defined(NEXUS_USE_7422_SV_BOARD)
    NEXUS_31xxSettings st31xxSettings;
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    int card=0;
    unsigned numChannels;
    NEXUS_FrontendCardSettings cardSettings;
#endif

#else
    #if NEXUS_USE_7425_VMS_SFF || NEXUS_USE_7422_VMS_SFF
    NEXUS_FrontendLTSIDParameters ltsidParams;
    unsigned j=0;
        #if defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)
        unsigned u3255ch;
        unsigned u3128ch;
        NEXUS_Frontend3128Settings st3128Settings;
        NEXUS_3255DeviceSettings st3255DeviceSettings;
        NEXUS_3255ChannelSettings st3255ChannelSettings;
        NEXUS_3255DeviceCapabilities st3255DeviceCapabilities;
        NEXUS_3255ChannelCapabilities st3255ChannelCapabilities;
        NEXUS_3128ProbeResults results;
        NEXUS_FrontendDevice3128OpenSettings st3128DeviceOpenSettings;
        #elif defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT)
        unsigned u3255ch;
        NEXUS_3255DeviceSettings st3255DeviceSettings;
        NEXUS_3255ChannelSettings st3255ChannelSettings;
        NEXUS_3255DeviceCapabilities st3255DeviceCapabilities;
        NEXUS_3255ChannelCapabilities st3255ChannelCapabilities;
        #else
        unsigned u3128ch;
        NEXUS_3128ProbeResults results;
        NEXUS_Frontend3128Settings st3128Settings;
        NEXUS_FrontendDevice3128OpenSettings st3128DeviceOpenSettings;
        NEXUS_FrontendDevice3128Settings st3128DeviceSettings;
        #endif
        #if !(defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)) && defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
        unsigned u3255ch;
        NEXUS_3255DeviceSettings st3255DeviceSettings;
        NEXUS_3255ChannelSettings st3255ChannelSettings;
        NEXUS_3255DeviceCapabilities st3255DeviceCapabilities;
        NEXUS_3255ChannelCapabilities st3255ChannelCapabilities;
        #endif
    #else
    NEXUS_31xxSettings st31xxSettings;
    #endif
#endif /* defined(NEXUS_USE_7425_SV_BOARD) || defined(NEXUS_USE_7422_SV_BOARD) */

    NEXUS_FrontendUserParameters userParams;
#if !defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)
    NEXUS_GpioSettings tunerGpioSettings;
#endif

#if !defined(NEXUS_USE_7425_VMS_SFF) && !defined(NEXUS_USE_7422_VMS_SFF)
    rc = bcm3405_init();
    if (rc != NEXUS_SUCCESS) {
        return BERR_SUCCESS;
    }
#endif
    if (!pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3] && !pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_4_5]) {
        BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
        return BERR_NOT_INITIALIZED;
    }

#if defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF)
    for (j=0; j<NEXUS_MAX_FRONTENDS; j++) {
        ltsidCount[j] = 0;
    }
#if defined NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT

    NEXUS_Frontend_GetDefault3255DeviceSettings(&st3255DeviceSettings);
    st3255DeviceSettings.rpcTimeout = 50;
    st3255DeviceSettings.mtsif = true;
    st3255DeviceHandle = NEXUS_Frontend_Open3255Device(0,&st3255DeviceSettings);
    NEXUS_Frontend_Get3255DeviceCapabilities(st3255DeviceHandle,&st3255DeviceCapabilities);
    BDBG_MSG(("DOCSIS Capabilities : Total Channels %u QAM Channels %u Docsis Channels %u oob %s",
              st3255DeviceCapabilities.totalChannels,st3255DeviceCapabilities.numOfQamChannels,
              st3255DeviceCapabilities.numOfDocsisChannels,st3255DeviceCapabilities.isOobChannelSupported?"true":"false" ));
    /*
     * Open the DOCSIS Inband channels
     */
    for (i=0, u3255ch=0;
         (u3255ch<st3255DeviceCapabilities.totalChannels) && (i<NEXUS_MAX_FRONTENDS-1);
         u3255ch++)
    {
        NEXUS_Frontend_Get3255ChannelCapabilities(st3255DeviceHandle, u3255ch, &st3255ChannelCapabilities);
        if(st3255ChannelCapabilities.channelType == NEXUS_3255ChannelType_eInBand)
        {
            BDBG_MSG((" frontend index %u Docsis QAM channel %u", i, u3255ch));
            NEXUS_Frontend_GetDefault3255ChannelSettings(&st3255ChannelSettings);
            st3255ChannelSettings.channelNumber = u3255ch;
            pConfig->frontend[i] = NEXUS_Frontend_Open3255Channel(st3255DeviceHandle,&st3255ChannelSettings);
            if ( NULL == (pConfig->frontend[i]) )
            {
                BDBG_ERR(("Unable to open onboard 3255 tuner/demodulator %d", i));
                continue;
            }
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            userParams.param1 = st3255DeviceSettings.mtsif ? st3255ChannelSettings.channelNumber : NEXUS_InputBand_e0+u3255ch;
            userParams.isMtsif = st3255DeviceSettings.mtsif;
            userParams.chipId = 0x3255; /* 3255 API for BCM3383*/
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

            NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
            ltsidParams.ltsidNum = userParams.param1;
            ltsidParams.chipId = 0x3255; /* 3255 API for BCM3383*/
            ltsidParams.mtsifNum = 1;
            ltsidParams.mtsifEnabled = true;
            NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
            ltsidCount[ltsidParams.ltsidNum] += 1;

            i++;
        }
        else
        {
            BDBG_MSG(("Docsis Non-QAM channel %u", u3255ch));
        }
    }
    /* GPIO 28 is used instead of EXT_IRQ. */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);

    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eLow;

#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,82, &tunerGpioSettings);
#else
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,28, &tunerGpioSettings);
#endif
    if (NULL == gpioHandle)
    {
      BDBG_ERR(("Unable to open GPIO for tuner %d", i));
      return BERR_NOT_INITIALIZED;
    }

    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);

    NEXUS_FrontendDevice_GetDefault3128OpenSettings(&st3128DeviceOpenSettings);
    st3128DeviceOpenSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_DSTRM_TUNER];
    st3128DeviceOpenSettings.i2cAddr = 0x6c;
    st3128DeviceOpenSettings.isrNumber = 0;
    st3128DeviceOpenSettings.gpioInterrupt = gpioHandle;
    st3128DeviceOpenSettings.inBandOpenDrain=true;
    st3128DeviceOpenSettings.loadAP = true;
    st3128DeviceOpenSettings.isMtsif = true;
    st3128Settings.device = NEXUS_FrontendDevice_Open3128(0, &st3128DeviceOpenSettings);
    if(st3128Settings.device == NULL){
        BDBG_ERR(("Unable to open 3128 device."));
        rc = BERR_TRACE(rc); return BERR_NOT_INITIALIZED;
    }

    st3128Settings.loadAP = true;
    st3128Settings.type = NEXUS_3128ChannelType_eInBand;
    st3128Settings.isMtsif = true;

    /*
    * Open the BCM3128 InBand channels
    */
    rc = NEXUS_Frontend_Probe3128(&st3128DeviceOpenSettings, &results);
    if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);

    for(u3128ch=0; ((i<NEXUS_MAX_FRONTENDS-1) && (u3128ch<(results.chip.id & 0xF))); u3128ch++)
    {
        BDBG_MSG((" frontend index %u BCM3128 QAM channel %u", i, u3128ch));
        st3128Settings.channelNumber = u3128ch;
        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator channel %d", i));
            continue;
        }
        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0+u3128ch;
        userParams.isMtsif = st3128Settings.isMtsif;
        userParams.chipId = 0x3128;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

        NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidParams.ltsidNum = userParams.param1;
        ltsidParams.chipId = 0x3128;
        ltsidParams.mtsifNum = 0;
        ltsidParams.mtsifEnabled = true;
        NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidCount[ltsidParams.ltsidNum] += 1;

        i++;
    }

#elif defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT)

    NEXUS_Frontend_GetDefault3255DeviceSettings(&st3255DeviceSettings);
    st3255DeviceSettings.rpcTimeout = 50;
    st3255DeviceSettings.mtsif = true;
    st3255DeviceHandle = NEXUS_Frontend_Open3255Device(0,&st3255DeviceSettings);
    NEXUS_Frontend_Get3255DeviceCapabilities(st3255DeviceHandle,&st3255DeviceCapabilities);
    BDBG_MSG(("DOCSIS Capabilities : Total Channels %u QAM Channels %u Docsis Channels %u oob %s",
              st3255DeviceCapabilities.totalChannels,st3255DeviceCapabilities.numOfQamChannels,
              st3255DeviceCapabilities.numOfDocsisChannels,st3255DeviceCapabilities.isOobChannelSupported?"true":"false" ));
    /*
     * Open the DOCSIS Inband channels
     */
    for (i=0, u3255ch=0;
         (u3255ch < st3255DeviceCapabilities.totalChannels) && (i < NEXUS_MAX_FRONTENDS-1);
          u3255ch++)
    {
        NEXUS_Frontend_Get3255ChannelCapabilities(st3255DeviceHandle, u3255ch, &st3255ChannelCapabilities);
        if (st3255ChannelCapabilities.channelType == NEXUS_3255ChannelType_eInBand)
        {
            BDBG_MSG((" frontend index %u Docsis QAM channel %u", i, u3255ch));
            NEXUS_Frontend_GetDefault3255ChannelSettings(&st3255ChannelSettings);
            st3255ChannelSettings.channelNumber = u3255ch;
            pConfig->frontend[i] = NEXUS_Frontend_Open3255Channel(st3255DeviceHandle, &st3255ChannelSettings);
            if ( NULL == (pConfig->frontend[i]) )
            {
                BDBG_ERR(("Unable to open onboard 3255 tuner/demodulator %d",i));
                continue;
            }
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            userParams.param1 = st3255DeviceSettings.mtsif ? st3255ChannelSettings.channelNumber : NEXUS_InputBand_e0+i;
            userParams.isMtsif = st3255DeviceSettings.mtsif;
            userParams.chipId = 0x3255; /* 3255 API for BCM3383*/
            NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

            NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
            ltsidParams.ltsidNum = userParams.param1;
            ltsidParams.chipId = 0x3255; /* 3255 API for BCM3383*/
            ltsidParams.mtsifNum = 1;
            ltsidParams.mtsifEnabled = true;
            NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
            ltsidCount[ltsidParams.ltsidNum] += 1;

            i++;
        }
        else
        {
            BDBG_MSG(("Docsis Non-QAM channel %u", u3255ch));
        }
    }
#else
    /* GPIO 28 is used instead of EXT_IRQ. */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);

    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eLow;

#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,82, &tunerGpioSettings);
#else
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,28, &tunerGpioSettings);
#endif
    if (NULL == gpioHandle)
    {
      BDBG_ERR(("Unable to open GPIO for tuner %d", i));
      return BERR_NOT_INITIALIZED;
    }

    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);

    NEXUS_FrontendDevice_GetDefault3128OpenSettings(&st3128DeviceOpenSettings);
    st3128DeviceOpenSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_DSTRM_TUNER];
    st3128DeviceOpenSettings.i2cAddr = 0x6c;
    st3128DeviceOpenSettings.isrNumber = 0;
    st3128DeviceOpenSettings.gpioInterrupt = gpioHandle;
    st3128DeviceOpenSettings.inBandOpenDrain=true;
    st3128DeviceOpenSettings.loadAP = true;
    st3128DeviceOpenSettings.isMtsif = true;

    st3128DeviceOpenSettings.outOfBand.ifFrequency = 0;
    st3128Settings.outOfBand.openDrain = true;
    st3128DeviceSettings.outOfBand.outputMode = NEXUS_FrontendOutOfBandOutputMode_eFec;

	st3128Settings.device = NEXUS_FrontendDevice_Open3128(0, &st3128DeviceOpenSettings);
    if(st3128Settings.device == NULL){
        BDBG_ERR(("Unable to open 3128 device."));
        rc = BERR_TRACE(rc); return BERR_NOT_INITIALIZED;
    }

    st3128Settings.type = NEXUS_3128ChannelType_eInBand;
    st3128Settings.isMtsif = true;
    /* NEXUS_MAX_FRONTENDS=9; BCM3128 has 8 InBand Channels and 1 OOB channel
    * Open the BCM3128 InBand channels
    */
    rc = NEXUS_Frontend_Probe3128(&st3128DeviceOpenSettings, &results);
    if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);

    for (i=0, u3128ch=0; u3128ch<(results.chip.id & 0xF); u3128ch++)
    {
        BDBG_WRN(("Waiting for onboard 3128 tuner/demodulator channel %d to initialize", u3128ch));
        st3128Settings.channelNumber = u3128ch;
        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator channel %d", i));
            continue;
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0+i;
        userParams.isMtsif = st3128Settings.isMtsif;
        userParams.chipId = 0x3128;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

        NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidParams.ltsidNum = userParams.param1;
        ltsidParams.chipId = 0x3128;
        ltsidParams.mtsifNum = 0;
        ltsidParams.mtsifEnabled = true;
        NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidCount[ltsidParams.ltsidNum] += 1;
        i++;
    }
#endif /* defined NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT */

#if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT

    #if !(defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)) && defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    NEXUS_Frontend_GetDefault3255DeviceSettings(&st3255DeviceSettings);
    st3255DeviceSettings.rpcTimeout = 50;
    st3255DeviceHandle = NEXUS_Frontend_Open3255Device(0,&st3255DeviceSettings);
    NEXUS_Frontend_Get3255DeviceCapabilities(st3255DeviceHandle,&st3255DeviceCapabilities);
    #endif
    /*
     * If OOB channel is present in the Docsis device, check for the channel number
     */
    u3255ch=0;
    if(st3255DeviceCapabilities.isOobChannelSupported)
    {
        for(u3255ch=0; u3255ch<st3255DeviceCapabilities.totalChannels; u3255ch++)
        {
            NEXUS_Frontend_Get3255ChannelCapabilities(st3255DeviceHandle, u3255ch, &st3255ChannelCapabilities);
            if(st3255ChannelCapabilities.channelType == NEXUS_3255ChannelType_eOutOfBand)
            {
                BDBG_MSG(("Found Docsis OOB channel index %u", u3255ch));
                break;
            }
        }
    }
    if (NEXUS_StrCmp(NEXUS_GetEnv("disable_oob_frontend"), "y") != 0)
    {
        BDBG_MSG(("Opening onboard 3255 OOB %u", i));
        NEXUS_Frontend_GetDefault3255ChannelSettings(&st3255ChannelSettings);
        st3255ChannelSettings.channelNumber = u3255ch;
        pConfig->frontend[i] = NEXUS_Frontend_Open3255Channel(st3255DeviceHandle, &st3255ChannelSettings);
        if ( NULL == (pConfig->frontend[i]) )
        {
            BDBG_ERR(("Unable to open 3255 tuner/demodulator OOB %d", i));
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3255DeviceSettings.mtsif ? st3255ChannelSettings.channelNumber : NEXUS_InputBand_e0+i;
        userParams.isMtsif = st3255DeviceSettings.mtsif;
        userParams.chipId = 0x3255;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

        NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidParams.ltsidNum = userParams.param1;
        ltsidParams.chipId = 0x3255;
        NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidCount[ltsidParams.ltsidNum] += 1;
    }
    else
    {
        BDBG_MSG(("env - disable_oob_frontend set"));
    }
#else /* NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT */
    /*Open the BCM3128 OOB channel */
    st3128Settings.type = NEXUS_3128ChannelType_eOutOfBand;
    BDBG_WRN(("Waiting for onboard 3128 Oob channel %d to initialize", u3128ch));
    st3128Settings.channelNumber = u3128ch;
    pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
    if (NULL == pConfig->frontend[i])
    {
        BDBG_ERR(("Unable to open onboard 3128 Oob channel %d", i));
    }

    NEXUS_Frontend_GetDefault3128ConfigSettings(&st3128DeviceSettings);

    st3128DeviceSettings.outOfBand.outputMode = NEXUS_FrontendOutOfBandOutputMode_eFec;

    rc = NEXUS_Frontend_3128_SetConfigSettings(pConfig->frontend[i], &st3128DeviceSettings);
    if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);

    NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
    userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0+i;
    userParams.isMtsif = st3128Settings.isMtsif;
    NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

    NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
    ltsidParams.ltsidNum = userParams.param1;
    ltsidParams.chipId = 0x3128;
    NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
    ltsidCount[ltsidParams.ltsidNum] += 1;
#endif /* NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT */

    /* TODO: this is completely bogus and relied on SetLTSID() overriding userParams, which it is not allowed to do.
       if you mess with userParams.param1, you will get no data routed */
    #if 0
    /* Check for multiple configurations on the same LTSID number */
    for (j=0; j<NEXUS_MAX_FRONTENDS; j++) {
        if (ltsidCount[j] > 1) {
            BDBG_ERR(("****************************************************"));
            BDBG_ERR(("ERROR: You have configured multiple frontends on LTSID number: %d", j));
            BDBG_ERR(("****************************************************"));
        }
    }
    #endif

#else /* defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF) */

     /* Probe Daughercards First */
#if NEXUS_NUM_FRONTEND_CARD_SLOTS
     if (!pConfig->i2c[NEXUS_I2C_CHANNEL_DSTRM_TUNER]) {
         BDBG_ERR(("Frontend cannot be initialized without first initializing I2C."));
         return BERR_NOT_INITIALIZED;
     }

     /* Find first empty frontend in config */
     for ( ; pConfig->frontend[idx] && idx < NEXUS_MAX_FRONTENDS; idx++ );
     if ( idx >= NEXUS_MAX_FRONTENDS )
     {
         return BERR_SUCCESS;
     }

     /* Probe first slot */
     NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);
     tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
     tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eFallingEdge;
     gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,82, &tunerGpioSettings);
     if (NULL == gpioHandle)
     {
         BDBG_ERR(("Unable to open GPIO for tuner %d", i));
     }

     rc = NEXUS_Frontend_Probe3128(&st3128DeviceOpenSettings, &results);
     if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);


     NEXUS_FrontendCard_GetDefaultSettings(&cardSettings);
     cardSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_DSTRM_TUNER];
     cardSettings.isrNumber = 0;                             /* This slot  uses GPIO interrupt 50 */
     cardSettings.numChannels = (results.chip.id & 0xF); /* 3128 card has 8 downstream channels and 3124 has 4*/
     cardSettings.numOutOfBandChannels = 0;   /* for now till the oob support is provided in the PI*/
                                                             /* First slot has 1 out of band channel */
     cardSettings.gpioInterrupt = gpioHandle;                /* This slot uses GPIO for interrupt */

     g_frontendCards[card] = NEXUS_FrontendCard_Open(&cardSettings);
     if ( g_frontendCards[card] ) {
         BDBG_WRN(("Found tuner card in slot 0"));
         NEXUS_FrontendCard_GetNumChannels(g_frontendCards[card], &numChannels);
         for ( i=0; i < numChannels && idx < NEXUS_MAX_FRONTENDS; idx++, i++ )
         {
             pConfig->frontend[idx] = NEXUS_FrontendCard_GetChannel(g_frontendCards[card], i);
             NEXUS_Frontend_GetUserParameters(pConfig->frontend[idx], &userParams);
             userParams.param1 = NEXUS_InputBand_e0;
             userParams.pParam2 = NULL;
             NEXUS_Frontend_SetUserParameters(pConfig->frontend[idx], &userParams);
         }
         card++;
     }
     else {
         BDBG_WRN(("Unable to find tuner card in slot 0"));
     }
#else /* NEXUS_NUM_FRONTEND_CARD_SLOTS */

    /* Old VMS and SV platforms have totally six tuners
    * Open the 31xx tuners' Inband channels */
    for (i=0; i<NEXUS_MAX_FRONTENDS-1-2; i++)
    {
        BDBG_WRN(("Waiting for onboard frontend %d to initialize", i));
        NEXUS_Frontend_GetDefault31xxSettings(&st31xxSettings);

        /* GPIO's are used instead of EXT_IRQ's
        * GPIO's have UPG_MAIN as their L1
        * GPIO's 19, 20, 26, 27, 62, and 63 are used
        * TODO:  Program GPIO settings and ensure Nexus frontend has
        *        support for GPIO interrupts
        */
        NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);
        tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
        tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eFallingEdge;
        st31xxSettings.isrNumber = 0;

        switch(i) {
        case 0:
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,19, &tunerGpioSettings);
            st31xxSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3];
            st31xxSettings.i2cAddr = 0x66;
            break;
        case 1:
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,20, &tunerGpioSettings);
            st31xxSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3];
            st31xxSettings.i2cAddr = 0x67;
            break;
        case 2:
#if NEXUS_USE_7422_SV_BOARD || NEXUS_USE_7425_SV_BOARD
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,26, &tunerGpioSettings);
#else
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,24, &tunerGpioSettings);
#endif
            st31xxSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3];
            st31xxSettings.i2cAddr = 0x68;
            break;
        case 3:
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,27, &tunerGpioSettings);
            st31xxSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3];
            st31xxSettings.i2cAddr = 0x69;
            break;
        /* coverity[dead_error_condition] */
        case 4:
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,62, &tunerGpioSettings);
            st31xxSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_4_5];
            st31xxSettings.i2cAddr = 0x66;
            break;
        /* coverity[dead_error_condition] */
        case 5:
            tunerGpio[i] = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,63, &tunerGpioSettings);
            st31xxSettings.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_TUNERS_4_5];
            st31xxSettings.i2cAddr = 0x67;
            break;
        }
        if (NULL == tunerGpio[i])
        {
            BDBG_ERR(("Unable to open GPIO for tuner %d", i));
            continue;
        }
        st31xxSettings.gpioInterrupt = tunerGpio[i];
        st31xxSettings.inBandOpenDrain=true;
        st31xxSettings.loadAP = true;
        st31xxSettings.type = NEXUS_31xxChannelType_eInBand;
        st31xxSettings.channelNumber = 0;
        st31xxSettings.ifFrequency = 0;

        pConfig->frontend[i] = NEXUS_Frontend_Open31xx(&st31xxSettings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3112 tuner/demodulator %d", i));
            continue;
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = NEXUS_InputBand_e0 + i;
        userParams.pParam2 = NULL;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);
    }
#endif /* NEXUS_NUM_FRONTEND_CARD_SLOTS */
#endif /* defined(NEXUS_USE_7425_VMS_SFF) || defined(NEXUS_USE_7422_VMS_SFF)*/
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
#if defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT) || defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    if(st3255DeviceHandle)
    {
        NEXUS_Frontend_Close3255Device(st3255DeviceHandle);
    }
#endif
#if defined NEXUS_USE_7425_VMS_SFF || defined NEXUS_USE_7422_VMS_SFF
    if(gpioHandle)
    {
        NEXUS_Gpio_Close(gpioHandle);
        gpioHandle = NULL;
    }
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
    return;
}
#elif NEXUS_PLATFORM_7425_DBS || NEXUS_PLATFORM_7422_DBS
    static NEXUS_FrontendHandle g_onboard4506[NEXUS_MAX_FRONTENDS] = {NULL};

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_FrontendUserParameters userParams;
    unsigned i=0;
    NEXUS_4506Settings settings4506;
    NEXUS_FrontendHandle frontend = NULL;
    unsigned frontendNum = 0;
    NEXUS_GpioSettings tunerGpioSettings;
    NEXUS_GpioHandle tunerGpio;

    /* Open on-board 4506 */
    NEXUS_Frontend_GetDefault4506Settings(&settings4506);
    settings4506.i2cDevice = pConfig->i2c[NEXUS_I2C_CHANNEL_LNA];
    if (!settings4506.i2cDevice) {
        BDBG_ERR(("Unable to initialize I2C \n"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);
    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eFallingEdge;
    tunerGpio = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 50, &tunerGpioSettings);
    if (NULL == tunerGpio)
    {
        BDBG_ERR(("Unable to open GPIO for tuner."));
    }

    /* Initialize the 4 onboard BCM4506 tuners on 97425DBS */
    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        /* GPIO's are used instead of EXT_IRQ's
        * GPIO's have UPG_MAIN as their L1
        * GPIO 50 is used
        * TODO:  Program GPIO settings and ensure Nexus frontend has
        *        support for GPIO interrupts */
        BDBG_WRN(("Waiting for frontend %d to initialize", i));
        settings4506.gpioInterrupt = tunerGpio;
        settings4506.isrNumber = 0;

        switch(i)
        {
        /* Tuner 0 */
        case 0:
        case 1:
            settings4506.i2cAddr = 0x69;
            break;

        /* Tuner 1 */
        case 2:
        case 3:
            settings4506.i2cAddr = 0x68;
            break;

        /* Tuner 2 */
        case 4:
        case 5:
            settings4506.i2cAddr = 0x67;
            break;

        /* Tuner 3 */
        case 6:
        case 7:
            settings4506.i2cAddr = 0x66;
            break;

        default:
            BDBG_ERR(("Invalid Frontend = %d", i));
            break;
        }

        /* Channel 0 of Tuner */
        if ( (i%2) == 0)
            settings4506.channelNumber = 0;
        /* Channel 1 of Tuner */
        else
            settings4506.channelNumber = 1;

        g_onboard4506[frontendNum] = frontend = NEXUS_Frontend_Open4506(&settings4506);

        if ( NULL == frontend )
        {
            BDBG_ERR(("Unable to init on-board 4506 \n"));
        }

        /* Set the appropriate input bands */
        NEXUS_Frontend_GetUserParameters(frontend, &userParams);
        userParams.pParam2 = NULL;

        switch(i)
        {
        /* Tuner 0 */
        case 0:
            userParams.param1 = NEXUS_InputBand_e0;
            break;
        case 1:
            userParams.param1 = NEXUS_InputBand_e1;
            break;

        /* Tuner 1 */
        case 2:
            userParams.param1 = NEXUS_InputBand_e2;
            break;
        case 3:
            userParams.param1 = NEXUS_InputBand_e3;
#if NEXUS_PLATFORM_7425_DBS
            /* Reset ISL6423 wired to 4506B ch1 */
            {
                uint8_t i2cAddrISL = 0x09;
                uint8_t data = 0x40;
                NEXUS_I2c_WriteNoAddr(settings4506.i2cDevice, i2cAddrISL, &data, 1);
                data = 0x70;
                NEXUS_I2c_WriteNoAddr(settings4506.i2cDevice, i2cAddrISL, &data, 1);
            }
#endif
            break;

        /* Tuner 2 */
        case 4:
            userParams.param1 = NEXUS_InputBand_e4;
            break;
        case 5:
            userParams.param1 = NEXUS_InputBand_e5;
            break;

        /* Tuner 3 */
        case 6:
            userParams.param1 = NEXUS_InputBand_e6;
            break;
        case 7:
            userParams.param1 = NEXUS_InputBand_e7;
            break;

        default:
            BDBG_ERR(("Invalid Frontend = %d", i));
            break;
        }

        NEXUS_Frontend_SetUserParameters(frontend, &userParams);
        pConfig->frontend[frontendNum] = frontend;
        frontend = NULL;
        frontendNum++;
    }

    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    unsigned i;

    for (i=0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if(g_onboard4506[i]) {
            NEXUS_Frontend_Close(g_onboard4506[i]);
            g_onboard4506[i] = NULL;
        }
    }
}

#else
#if NEXUS_FRONTEND_3128
#include "nexus_frontend_3128.h"

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_FrontendUserParameters userParams;
    unsigned i=0;
    NEXUS_FrontendHandle frontend = NULL;
    NEXUS_GpioSettings tunerGpioSettings;
    static   NEXUS_GpioHandle gpioHandle = NULL;
    NEXUS_3128ProbeResults results;
    NEXUS_FrontendDevice3128OpenSettings st3128DeviceOpenSettings;
    NEXUS_Frontend3128Settings st3128Settings;
    NEXUS_FrontendDevice3128Settings st3128DeviceSettings;
    unsigned u3128ch;
   /* NEXUS_FrontendLTSIDParameters ltsidParams; */
    NEXUS_Error rc = NEXUS_SUCCESS;
  /*  static unsigned ltsidCount[NEXUS_MAX_FRONTENDS]; */

    /* GPIO 28 is used instead of EXT_IRQ. */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);

    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eLow;

#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,82, &tunerGpioSettings);
#else
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,28, &tunerGpioSettings);
#endif
    if (NULL == gpioHandle)
    {
      BDBG_ERR(("Unable to open GPIO for tuner %d", i));
      return BERR_NOT_INITIALIZED;
    }

    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);

    NEXUS_FrontendDevice_GetDefault3128OpenSettings(&st3128DeviceOpenSettings);
    st3128DeviceOpenSettings.i2cDevice = pConfig->i2c[4];
    st3128DeviceOpenSettings.i2cAddr = 0x6c;
    st3128DeviceOpenSettings.isrNumber = 0;
    st3128DeviceOpenSettings.gpioInterrupt = gpioHandle;
    st3128DeviceOpenSettings.inBandOpenDrain=true;
    st3128DeviceOpenSettings.loadAP = true;
    st3128DeviceOpenSettings.isMtsif = true;

    st3128DeviceOpenSettings.outOfBand.ifFrequency = 0;
    st3128Settings.outOfBand.openDrain = true;
    st3128DeviceSettings.outOfBand.outputMode = NEXUS_FrontendOutOfBandOutputMode_eFec;

	st3128Settings.device = NEXUS_FrontendDevice_Open3128(0, &st3128DeviceOpenSettings);
    if(st3128Settings.device == NULL){
        BDBG_ERR(("Unable to open 3128 device."));
        rc = BERR_TRACE(rc); return BERR_NOT_INITIALIZED;
    }

    st3128Settings.type = NEXUS_3128ChannelType_eInBand;
    st3128Settings.isMtsif = true;
    /* NEXUS_MAX_FRONTENDS=9; BCM3128 has 8 InBand Channels and 1 OOB channel
    * Open the BCM3128 InBand channels
    */
    rc = NEXUS_Frontend_Probe3128(&st3128DeviceOpenSettings, &results);
    if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);

    for (i=0, u3128ch=0; u3128ch<(results.chip.id & 0xF); u3128ch++,i++)
    {
        BDBG_WRN(("Waiting for onboard 3128 tuner/demodulator channel %d to initialize", u3128ch));
        st3128Settings.channelNumber = u3128ch;
        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator channel %d", i));
            continue;
        }

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0+i;
        userParams.isMtsif = st3128Settings.isMtsif;
        userParams.chipId = 0x3128;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

#if 0
        NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidParams.ltsidNum = userParams.param1;
        ltsidParams.chipId = 0x3128;
        ltsidParams.mtsifNum = 0;
        ltsidParams.mtsifEnabled = true;
        NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidCount[ltsidParams.ltsidNum] += 1;
        i++;
#endif
    }
    return 0;
}

void NEXUS_Platform_UninitFrontend(void)
{
}

#else /* #if NEXUS_FRONTEND_3128 */

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    return 0;
}

void NEXUS_Platform_UninitFrontend(void)
{
}
#endif /* #if NEXUS_FRONTEND_3128 */
#endif
#endif	/* defined(CONFIG_PLATFORM_DB4506) */
#endif	/* defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD) */

BTRC_MODULE(ChnChange_TuneStreamer, ENABLE);

NEXUS_Error NEXUS_Platform_GetStreamerInputBand(unsigned index, NEXUS_InputBand *pInputBand)
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

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_PLATFORM_DB4506)
static NEXUS_GpioHandle tunerGpio01;
static NEXUS_GpioHandle tunerGpio23;

NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
	unsigned channelid
)
{
	NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
	NEXUS_4506Settings settings4506;
	NEXUS_FrontendHandle frontend = NULL;
	NEXUS_GpioSettings tunerGpioSettings;

	/* Open on-board 4506 */
	NEXUS_Frontend_GetDefault4506Settings(&settings4506);

	/* External LNA BCM3445 Setting */
	settings4506.is3445ExternalLna = 1;

	BDBG_WRN(("Waiting for frontend %d to initialize", channelid));

	if ((channelid == 0)||(channelid == 1))
	{
		settings4506.i2cDevice = pConfig->i2c[I2C_CHANNEL_CHANNEL];
		if (!settings4506.i2cDevice) {
			BDBG_ERR(("Unable to initialize master BCM4506 I2C \n"));
			return NULL;
		}
	}
	else if ((channelid == 2)||(channelid == 3))
	{
		settings4506.i2cDevice = pConfig->i2c[I2C_CHANNEL_CHANNEL2];
		if (!settings4506.i2cDevice) {
			BDBG_ERR(("Unable to initialize slave BCM4506 I2C \n"));
			return NULL;
		}
	}
	else
	{
		BDBG_ERR(("Invalid channelid \n"));
		return NULL;
	}

	if (channelid == 0)
	{
		NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);
	    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
	    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eFallingEdge;
	    tunerGpio01 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 62, &tunerGpioSettings);
	    if (NULL == tunerGpio01)
	    {
	        BDBG_ERR(("Unable to open GPIO for tuner01."));
			return NULL;
	    }
	}
	else if (channelid == 2)
	{
		NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);
	    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
	    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eFallingEdge;
	    tunerGpio23 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 66, &tunerGpioSettings);
	    if (NULL == tunerGpio23)
	    {
	        BDBG_ERR(("Unable to open GPIO for tuner23."));
			return NULL;
	    }
	}
	else
	{
		/* Do nothing */
	}

	settings4506.isrNumber = 0;

	switch(channelid)
	{
		/* Tuner 0 */
		case 0:
		case 1:
			settings4506.gpioInterrupt = tunerGpio01;
			settings4506.i2cAddr = 0x69;
			break;

		/* Tuner 1 */
		case 2:
		case 3:
			settings4506.gpioInterrupt = tunerGpio23;
			settings4506.i2cAddr = 0x68;
			break;

		default:
			BDBG_ERR(("Invalid Frontend = %d", channelid));
			break;
	}

	/* Channel 0 of Tuner */
	if ( (channelid%2) == 0)
		settings4506.channelNumber = 0;
	/* Channel 1 of Tuner */
	else
		settings4506.channelNumber = 1;

	g_frontend4506[channelid] = frontend = NEXUS_Frontend_Open4506(&settings4506);

	if ( NULL == frontend )
	{
		if ((channelid==2)||(channelid==3))
		{
			BDBG_ERR(("Frontend has only 1 I2C Bus. Re-try to open the slave BCM4506"));
			settings4506.i2cDevice = pConfig->i2c[I2C_CHANNEL_CHANNEL];
			if (!settings4506.i2cDevice)
			{
				BDBG_ERR(("Unable to initialize slave BCM4506 I2C \n"));
				return NULL;
			}

			g_frontend4506[channelid] = frontend = NEXUS_Frontend_Open4506(&settings4506);
			if ( NULL == frontend )
			{
				BDBG_ERR(("Unable to init on-board 4506 \n"));
			}
		}
	}

	pConfig->frontend[channelid] = frontend;

	return frontend;
}
#else	/* defined(CONFIG_PLATFORM_DB4506) */
#if NEXUS_FRONTEND_3128
NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
See nexus_platform_frontend.c for ID assignment and/or see
nexus_platform_features.h for possible platform-specific macros.
*/
)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_FrontendUserParameters userParams;
    unsigned i=0;
    NEXUS_FrontendHandle frontend = NULL;
    NEXUS_GpioSettings tunerGpioSettings;
    static   NEXUS_GpioHandle gpioHandle = NULL;
    NEXUS_3128ProbeResults results;
    NEXUS_FrontendDevice3128OpenSettings st3128DeviceOpenSettings;
    NEXUS_Frontend3128Settings st3128Settings;
    NEXUS_FrontendDevice3128Settings st3128DeviceSettings;
    unsigned u3128ch;
   /* NEXUS_FrontendLTSIDParameters ltsidParams; */
    NEXUS_Error rc = NEXUS_SUCCESS;
  /*  static unsigned ltsidCount[NEXUS_MAX_FRONTENDS]; */

    /* GPIO 28 is used instead of EXT_IRQ. */
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &tunerGpioSettings);

    tunerGpioSettings.mode = NEXUS_GpioMode_eInput;
    tunerGpioSettings.interruptMode = NEXUS_GpioInterrupt_eLow;

#if NEXUS_NUM_FRONTEND_CARD_SLOTS
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,82, &tunerGpioSettings);
#else
    gpioHandle = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard,28, &tunerGpioSettings);
#endif
    if (NULL == gpioHandle)
    {
      BDBG_ERR(("Unable to open GPIO for tuner %d", i));
      return BERR_NOT_INITIALIZED;
    }

    NEXUS_Frontend_GetDefault3128Settings(&st3128Settings);

    NEXUS_FrontendDevice_GetDefault3128OpenSettings(&st3128DeviceOpenSettings);
    st3128DeviceOpenSettings.i2cDevice = pConfig->i2c[4];
    st3128DeviceOpenSettings.i2cAddr = 0x6c;
    st3128DeviceOpenSettings.isrNumber = 0;
    st3128DeviceOpenSettings.gpioInterrupt = gpioHandle;
    st3128DeviceOpenSettings.inBandOpenDrain=true;
    st3128DeviceOpenSettings.loadAP = true;
    st3128DeviceOpenSettings.isMtsif = true;

    st3128DeviceOpenSettings.outOfBand.ifFrequency = 0;
    st3128Settings.outOfBand.openDrain = true;
    st3128DeviceSettings.outOfBand.outputMode = NEXUS_FrontendOutOfBandOutputMode_eFec;

    st3128Settings.device = NEXUS_FrontendDevice_Open3128(0, &st3128DeviceOpenSettings);
    if(st3128Settings.device == NULL){
        BDBG_ERR(("Unable to open 3128 device."));
        rc = BERR_TRACE(rc); return BERR_NOT_INITIALIZED;
    }

    st3128Settings.type = NEXUS_3128ChannelType_eInBand;
    st3128Settings.isMtsif = true;
    /* NEXUS_MAX_FRONTENDS=9; BCM3128 has 8 InBand Channels and 1 OOB channel
    * Open the BCM3128 InBand channels
    */
    rc = NEXUS_Frontend_Probe3128(&st3128DeviceOpenSettings, &results);
    if(rc) return BERR_TRACE(BERR_NOT_INITIALIZED);

    for (i=0, u3128ch=0; u3128ch<(results.chip.id & 0xF); u3128ch++,i++)
    {
        BDBG_ERR(("Waiting for onboard 3128 tuner/demodulator channel %d to initialize", u3128ch));
        st3128Settings.channelNumber = u3128ch;
        pConfig->frontend[i] = NEXUS_Frontend_Open3128(&st3128Settings);
        if (NULL == pConfig->frontend[i])
        {
            BDBG_ERR(("Unable to open onboard 3128 tuner/demodulator channel %d", i));
            continue;
        }

        BDBG_ERR(("channel [%d][0x%x] ",i, pConfig->frontend[i]));

        NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
        userParams.param1 = st3128Settings.isMtsif ? st3128Settings.channelNumber : NEXUS_InputBand_e0+i;
        userParams.isMtsif = st3128Settings.isMtsif;
        userParams.chipId = 0x3128;
        NEXUS_Frontend_SetUserParameters(pConfig->frontend[i], &userParams);

#if 0
        NEXUS_Frontend_GetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidParams.ltsidNum = userParams.param1;
        ltsidParams.chipId = 0x3128;
        ltsidParams.mtsifNum = 0;
        ltsidParams.mtsifEnabled = true;
        NEXUS_Frontend_SetLTSID(pConfig->frontend[i], &ltsidParams);
        ltsidCount[ltsidParams.ltsidNum] += 1;
        i++;
#endif
    }
    return 0;
}
#else /* NEXUS_FRONTEND_3128*/
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
#endif  /* NEXUS_FRONTEND_3128*/
#endif	/* defined(CONFIG_PLATFORM_DB4506) */
#endif	/* defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD) */

