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
* $brcm_Revision: 64 $
* $brcm_Date: 5/14/12 3:44p $
*
* API Description:
*   API name: Platform Frontend
*    Platform Frontend Setup
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97125/src/nexus_platform_frontend.c $
* 
* 64   5/14/12 3:44p mward
* SW7231-748: fix #includes
* 
* 63   11/11/11 12:08p mward
* SW7125-1152:  Set defaults for used LNA status values in the failed LNA
*  case.
* 
* 62   11/3/11 12:34p mward
* SW7125-1139:  Improve handling of LNA failure.
* 
* 61   5/27/11 4:06p mward
* SW7125-926:  Use NEXUS_Amplifier for LNA, rather than direct control in
*  tnr/7125.
* 
* SW7125-926/12   5/27/11 3:22p mward
* SW7125-926:  Set LNA boost/tilt parameters in 7125 frontend config.
* 
* SW7125-926/11   5/24/11 4:55p mward
* SW7125-905:  AGC thresholds can't be read from CM when CM is powered
*  off.  Be sure to read them when it's on.
* 
* SW7125-926/10   5/21/11 2:33p mward
* SW7125-926:  Correct parameter to
*  NEXUS_Platform_Frontend_P_UpdateLnaGainTo7125().
* 
* SW7125-926/9   5/20/11 6:15p mward
* SW7125-926:  Force LNA gain read and update at 7125 frontend init,
*  otherwise LNA gain won't be ready for the first tune.
* 
* SW7125-926/8   5/16/11 10:13a mward
* SW7125-905:  Fix non-power-management stubs.
* 
* SW7125-926/7   5/12/11 7:11p mward
* SW7125-905:  Add "skip init" versions of host-controlled 7125
*  (internal) frontends.
* 
* SW7125-926/6   5/12/11 12:32p mward
* SW7125-905:  Adding option to not re-open frontends when switching
*  control.
* 
* SW7125-926/5   5/11/11 6:13p mward
* SW7125-905:  Close 7125 tuner when closing frontend transferring to eCM
*  from host.
* 
* SW7125-926/4   5/10/11 3:56p mward
* SW7125-905:  Don't defer CM power down.  Open host-controlled frontends
*  when transferring control from CM to host.
* 
* SW7125-926/3   5/2/11 11:29a mward
* SW7125-926:  Add run-time environment setting tnr_7125_lna_control.  If
*  defined, will use LNA control in the tnr driver.  This feature is
*  temporary.
* 
* SW7125-926/2   4/29/11 4:39p mward
* SW7125-926:  Encode gain/boost into configSettings.lnaGain.
* 
* SW7125-926/1   4/28/11 5:28p mward
* SW7125-926:  Add update (LNA) gain callback, LNA gain configuration
*  setting for 7125 tuner/DS.
* 
* 60   5/16/11 10:24a mward
* SW7125-905:  Adding option to not re-open frontends when switching
*  control.
* 
* 59   3/30/11 7:00p mward
* SW7125-896:  Probe for LNA at possible I2C addresses if I2C address 0
*  is given in settings.
* 
* 58   3/14/11 4:42p mward
* SW7125-828:  Use new agcGain, read from LNA. (merge)
* 
* SW7125-828/4   3/11/11 6:01p mward
* SW7125-828:  Use new agcGain, read from LNA.
* 
* SW7125-828/3   2/22/11 5:24p mward
* SW7125-845:  Temporarily default to loading AP to 31xx frontends, due
*  to poor performance if AP is not loaded.  Use load_AP=n in environment
*  to default to not loading AP code.
* 
* SW7125-828/2   2/17/11 10:13a mward
* SW7125-737:  Allow power down in untune of 31xx, but don't do it
*  automatically in uninit pending "deep power up" fix in DOCSIS and host
*  31xx drivers.
* 
* SW7125-828/1   2/14/11 12:06p mward
* SW7125-828:  Adding a function to read LNA gain, and update it to 31xx.
* 
* 57   2/9/11 3:46p mward
* SW7125-769:  Better way to check for need to open LNA.
* 
* 56   2/9/11 2:40p mward
* SW7125-769:  Make sure LNA gets opened / initialized if opening only
*  one frontend with NEXUS_Platform_OpenFrontend().
* 
* 55   2/1/11 6:16p mward
* SW7125-769:  Condition code for specific frontends with their macros.
* 
* 54   1/27/11 11:37a mward
* SW7125-769:  Specific frontend IDs for use with
*  NEXUS_Platform_OpenFrontend(). (merge)
* 
* SW7125-769/3   1/24/11 6:21p mward
* SW7125-769:  Fix 3112 bypass function to handle a powered-down 3112.
* 
* SW7125-769/2   1/24/11 5:15p mward
* SW7125-769:  Skip external 3112 open if host_3112 is not defined, but
*  when opening internal, open external DS to set up pass-thru.
* 
* SW7125-769/1   1/24/11 11:33a mward
* SW7125-769:  Add NEXUS_Platform_OpenFrontend().  Use it in
*  NEXUS_Platform_InitFrontend().
* 
* 53   1/21/11 10:46a mward
* SW7125-737:  Fixed build when NEXUS_POWER_MANAGEMENT!=y.
* 
* 52   1/20/11 4:55p mward
* SW7125-737:  Can't read AGC thresholds from shared memory when CM is
*  off.  Don't attempt to open 3255 frontends when host control is
*  indicated in shared memory.
* 
* 51   12/15/10 4:33p mward
* SW7125-737:  Don't automatically select loadAP when CM is off.
* 
* 50   11/30/10 9:38a mward
* SW7125-737:  Added NEXUS_Platform_FrontendType().  App should use it to
*  get frontend type, and must not attempt to access the userParams to
*  read the frontend type.
* 
* 49   11/29/10 10:14a mward
* SW7125-737:  For compatibility with passive standby, always power down
*  CM frontend devices when powering down CM.
* 
* 48   11/22/10 6:18p mward
* SW7125-737:  Open and close LNA even when !NEXUS_POWER_MANAGEMENT, for
*  LNA initialization if no 3255.
* 
* 47   11/22/10 5:38p mward
* SW7125-737:  Simplification for use with DOCSIS that powers on 31xx.
*  Maintain host or BNM-control frontend handles, not both.
* Fixes for initialize/unit issues with standby.
* 
* SW7125-729/SW7125-737/10   11/19/10 11:56a mward
* SW7125-737: NEXUS_Platform_P_FrontendType() using pParam2 of
*  NEXUS_FrontendUserParameters.
* 
* SW7125-729/SW7125-737/9   11/18/10 7:44p mward
* SW7125-737:  Explicit untune on unit frontends to power down.
* 
* SW7125-729/SW7125-737/8   11/18/10 4:04p mward
* SW7125-737:  Clear CM power-down flag when powering down.
* 
* SW7125-729/SW7125-737/7   11/18/10 11:46a mward
* SW7125-737:  Check CM power when bringing up frontends, skip 3255 ones
*  if it is not on.  Fix logic in
*  NEXUS_Platform_Frontend_P_SetupHostControl()
* 
* SW7125-729/SW7125-737/6   11/17/10 6:21p mward
* SW7125-737:  Simplification for use with DOCSIS that powers on 31xx.
*  Maintain host or BNM-control frontend handles, not both.
* 
* SW7125-729/2   11/14/10 11:04a mward
* SW7125-729:  Use NEXUS_AmplifierSettings.enabled rather than separate
*  functions for power saver.
* 
* SW7125-729/1   11/10/10 5:34p mward
* SW7125-729:  NEXUS_Platform_PowerUpLNA() and
*  NEXUS_Platform_PowerDownLNA()
* 
* 45   10/26/10 10:43p agin
* SWNOOS-429:  Make g_onboardQAM visible in general.
* 
* 44   10/22/10 5:11p mward
* SW7125-497:  Use special symbolRate values to turn on and off status
*  timer for control handoff with DOCSIS.  Handle board with only one
*  external 31xx.
* 
* 43   10/22/10 12:27p ayxiang
* SW7125-675: change and add API to provide the frontend handle to be
*  controlled by user based on different frontend PM status.
* 
* 42   9/28/10 3:29p ayxiang
* SW7125-497: when enable host control, will write to shared memory.
*  remove 3112 frontend host control functions.
* 
* 41   9/27/10 8:19p agin
* SWNOOS-425:  Make g_onboardQAM visible to no OS.
* 
* 40   9/8/10 6:29p mward
* SW7125-497: switch back I2C pinmux  in NEXUS_Platform_UninitFrontend().
* 
* 39   9/8/10 4:57p mward
* SW7125-497: NEXUS_Platform_Frontend_P_SetupHostControl() switch back
*  I2C pinmux when switching back to BNM control.
* 
* 38   8/30/10 6:18p mward
* SW7125-35:  Fixed load_AP logic.
* 
* 37   8/26/10 6:15p mward
* SW7125-497: Host control for internal tuner/DS.
* 
* 36   8/23/10 4:27p mward
* SW7125-513:  Adding AGC thresholds to tuner settings.  The BNM's
*  computed ones can be read from shared memory.
* 
* SW7125-513/3   8/17/10 3:04p mward
* SW7125-513:  Adding AGC thresholds to tuner settings.  The BNM's
*  computed ones can be read from shared memory.
* 
* 35   8/12/10 11:55a mward
* SW7125-513:  97019 has max 4 frontends, not 5.
* 
* 34   8/11/10 12:09p mward
* SW7125-513:  Restore 3112 bypass for use with host 7125 frontend
*  control.
* 
* 33   8/6/10 1:47p gskerl
* SW7125-497: Condition close of g_hostStandbyControlFrontend[] with
*  (NEXUS_PLATFORM!=97019).
* 
* 32   8/5/10 6:32p mward
* SW7125-497: Condition close of g_hostStandbyControlFrontend[] with
*  (NEXUS_PLATFORM!=97019).
* 
* 31   8/5/10 5:19p mward
* SW7125-497: use NEXUS_POWER_MANAGEMENT instead of NEXUS_FRONTEND_PWM.
* 
* 30   7/27/10 1:57p mward
* SW7125-513: Merge to main branch:  Simplify names.
* 
* 29   7/26/10 10:24a mward
* SW7125-513: Merge support for 7125 internal tuners.
* 
* SW7125-513/2   7/13/10 7:16p mward
* SW7125-513: Open and close 7125 internal tuners.
* 
* SW7125-513/1   7/12/10 4:43p mward
* SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
*  7125 tuner/ds.
* 
* 28   7/9/10 11:59a mward
* SW7125-35:  Fix logic to pack available frontends at lowest elements of
*  pConfig->frontend[].
* 
* 27   7/6/10 11:15a ayxiang
* SW7125-497: set I2C to host for 3112 to clean the memory allocated when
*  closing.
* 
* 26   7/1/10 7:03p mward
* SW7125-497: Stubbed API functions for kernel/proxy mode build.
* 
* 25   6/30/10 2:45p mward
* SW7125-35:  97019 platform must use NEXUS_MAX_FRONTENDS 4
* and NEXUS_MAX_3255_ADSCHN 4, even though only 2 internal DS are
*  available.
* 
* 24   6/29/10 7:34p ayxiang
* SW7125-497: let external tuner host control api function to return
*  frontend handle for application to use
* 
* 23   6/29/10 1:45p ayxiang
* SW7125-497: change printf to BDBG_MSG
* 
* 22   6/28/10 5:59p ayxiang
* SW7125-497:allocate more frontend handle set array, add functions to
*  enable host control 3112 on 97125
* 
* 21   6/23/10 11:55a mward
* SW7125-35:  Correct number of frontends and order of use for 97019
*  platform.
* 
* 20   6/21/10 4:00p mward
* SW7125-35:  Open 3112 with configureWatchdog = false.
* 
* 19   6/21/10 11:39a mward
* SW7125-35:  Correct interrupt numbers for 3112s.  Add option for
*  whether or not to load AP code.
* 
* 18   6/16/10 1:57p mward
* SW7125-35:  If no_3255=y, don't do host 3112 unless host_3112=y also.
* 
* 17   6/11/10 4:31p mward
* SW7125-35: Eliminate the NEXUS_97125_HOST_3112 build option, always
*  build 97125 with support for host control of BCM3112 QAM DS. Eliminate
*  build option NEXUS_97125_BYPASS_3112.  No longer needed.
* SW7125-466:  Use external 3112 first before internal DS.
* 
* 16   4/23/10 2:56p mward
* SW7125-373:  Use NEXUS_MAX_FRONTENDS for OOB/no OOB decision.
* 
* 15   4/22/10 6:20p mward
* SW7405-3832: switch to NEXUS_PLATFORM macro
* 
* 14   4/22/10 4:27p mward
* SW7125-4: Don't open 3255 tuners at all if no_3255 is set in env.
* 
* 13   11/30/09 6:41p mward
* SW7125-131: Allow 4 Rmagnum tuners.
* 
* 12   11/23/09 4:18p mward
* SW7125-4:  Handle both of the two 3112's under BNM control.
* 
* 11   11/23/09 11:52a mward
* SW7125-35: NEXUS_97125_BYPASS_3112 separate from NEXUS_97125_HOST_3112,
*  to allow compile with latter option.
* 
* 10   11/3/09 3:45p mward
* SW7125-35: Add an option to bypass 3112's by clearing a power-down bit.
* 
* 9   10/19/09 10:55a mward
* SW7125-35:  If using control of BCM3112's by host, don't open 3255 for
*  corresponding QAM.
* 
* 8   10/14/09 2:20p mward
* SW7125-35:  Support control of BCM3112's by host.
* 
* 7   9/29/09 6:09p mward
* SW7125-4: Configure proper input bands during frontend init.
* 
* 6   9/3/09 3:36p mward
* SW7125-4:  Use additional "streamers" to select input bands for QAM DS
*  set up with BBS script.
* 
* 5   8/31/09 7:32p mward
* SW7125-4: Disable SW streamer select for now, use DIP switch.
* 
* 4   8/26/09 7:10p mward
* SW7125-4: Input band corrections, streamer FPGA select.
* 
* 3   8/26/09 12:01p mward
* SW7125-4: Copied 3255/DOCSIS frontend handling from 97420dvr2 platform.
* 
* 2   7/24/09 1:06p mward
* PR 55545: Remove some unused onboard tuner variables.
* 
* 1   7/17/09 2:16p mward
* PR 55545: Copied and adapted from 97420dvr2.
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
***************************************************************************/

#include "nexus_platform_priv.h"
#include "nexus_types.h"
#include "priv/nexus_core.h"
#include "nexus_i2c.h"
#include "nexus_frontend.h"
#include "nexus_platform_features.h"
#include "nexus_platform.h"
#include "nexus_platform_frontend_power_management.h"
#include "nexus_base.h"
#include "nexus_frontend_3255.h"
#include "nexus_input_band.h"
#include "nexus_amplifier.h"
#include "nexus_amplifier_3412.h"

#include "bchp_hif_cpu_intr1.h"

/* i2c channel assignments */
#define I2C_DEVICE_FPGA_CH 2

#if NEXUS_POWER_MANAGEMENT && (NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY + NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY)
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG)
#define HOST_CONTROL_NOFRONTEND 0x50574d00             /*"PWM" 0 */
#define HOST_CONTROL_DS0 0x50574d01        /*internal DS0           it's typically always controlled by BNM*/
#define HOST_CONTROL_DS1 0x50574d02        /*internal DS1*/
#define HOST_CONTROL_DS2 0x50574d04        /*external 3112 A*/
#define HOST_CONTROL_DS3 0x50574d08        /*external 3112 B*/
#else
#define HOST_CONTROL_NOFRONTEND 0x004d5750             /*"PWM" 0 */
#define HOST_CONTROL_DS0 0x014d5750        /*internal DS0           it's typically always controlled by BNM*/
#define HOST_CONTROL_DS1 0x024d5750        /*internal DS1*/
#define HOST_CONTROL_DS2 0x044d5750        /*external 3112 A*/
#define HOST_CONTROL_DS3 0x084d5750        /*external 3112 B*/
#endif
#define HOST_CONTROL_MASK (~HOST_CONTROL_NOFRONTEND & (HOST_CONTROL_DS0 | HOST_CONTROL_DS1 | HOST_CONTROL_DS2 | HOST_CONTROL_DS3))
static NEXUS_Error NEXUS_Platform_Frontend_P_SetupHostControl(bool byHost,bool openAllFrontends);
#endif

BDBG_MODULE(nexus_platform_frontend);

unsigned NEXUS_Platform_P_FrontendType(NEXUS_FrontendHandle h)
{
    unsigned frontendType = 0;  /* unknown */
    if (h)
    {
        NEXUS_FrontendUserParameters userParams;
        NEXUS_Frontend_GetUserParameters(h, &userParams);
        switch (userParams.chipId)
        {
            case 0x3112: 
                frontendType = 3112;
                break;
            case 0x3255:
                frontendType = 3255;
                break;
            case 0x7125: 
                frontendType = 7125;
                break;
            default: 
                break;
        }
    }
    return frontendType;
}

NEXUS_Error NEXUS_Platform_FrontendType(NEXUS_FrontendHandle h, uint32_t *pFrontendType)
{
    *pFrontendType = NEXUS_Platform_P_FrontendType(h);
    if (*pFrontendType == 0){
        return NEXUS_NOT_SUPPORTED;
    }
    else {
        return NEXUS_SUCCESS;
    }
}

static NEXUS_AmplifierHandle g_LNA = NULL;

#if (NEXUS_PLATFORM != 97019)
#include "nexus_frontend_31xx.h"

#include "bchp_31xx.h"
#include "nexus_i2c.h"
#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_core.h"
#include "bchp_sun_top_ctrl.h"

#define DS_TUNER_ANACTL_WDATA_05_ADDR 0x00032014


#define RETRY_LIMIT 4
void NEXUS_Platform_P_Bypass3112(unsigned ds)
{
	NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
	NEXUS_I2cHandle i2cDevice = pConfig->i2c[3];	/* Onboard tuner/demod use BSC 3.*/
	BREG_I2C_Handle i2cHandle;
	uint8_t buf[4];
	int i;
    uint16_t i2cAddr = (ds & 0x1) ? 0x69 : 0x67;
    unsigned id = (ds == 2) ? NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2 : 
                              NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3;
    NEXUS_FrontendUserParameters userParams;

    for (i=0;i<NEXUS_MAX_FRONTENDS;i++)
    {
        if (pConfig->frontend[i])
        {   /* all frontends must be either host or CM controlled */
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            if (userParams.id==id)
            {
                BDBG_MSG(("%s(%u) skipping open",__FUNCTION__,ds));
                return;
            }
        }
    }

    i2cHandle = NEXUS_I2c_GetRegHandle(i2cDevice, NULL);

    buf[0]= 0x00;   /* SH_IND_GLOBAL_RESET1 */
    buf[1]= 0xff;   /* reserved=0xf AP_WDT_RST=1 UPG_WDT_RST=1 UPG_RST=1 PERIPH_RST=1 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x01;   /* SH_IND_GLOBAL_RESET0 */
    buf[1]= 0xff;   /* OTP_RST=1 TM_RST=1 reserved=1 DS_RST=1 AP_RST=1 reserved=1 reserved=1 CG_RST=1 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x02;   /* SH_IND_OSC_CTRL1 */
    buf[1]= 0xa0;   /* IAMP_ADJ=250uA ICBUF_ADG=250uA IBUFD_ADJ=200uA LIMIT_AMP=inactive XTAL_ADJ_CM 1.45V */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x03;   /* SH_IND_OSC_CTRL0 ' enable diffosc clocks */
    buf[1]= 0x40;   /* BYP_DBLR=0 DISABLE_CML6=1 DISABLE_CML5=0 DISABLE_CML4=0 DISABLE_CML3=0 DISABLE_CML2=0 ENABLE_FMON=0 PWRDN_DBLR=0 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x06;   /* SH_IND_CLK_CNTRL */
    buf[1]= 0x00;   /* AP_CLK_SRC=DIFFOSC_108 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x05;   /* SH_IND_POWER_UP ' power up d2cdiff_54, d2cdiff_108 and tuner */
    buf[1]= 0x07;   /* PWRUP_TUNER=1 PWRUP_108=1 PWRUP_54=1 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x09;   /* SH_IND_D2CDIFF_CNTRL NO_AC_108=0 NO_AC_54=0*/
    buf[1]= 0x00;   /* NO_AC_108=0 NO_AC_54=0 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x07;   /* SH_IND_CLK_ENABLE1 ' enable internal register clocks */
    buf[1]= 0xff;   /* reserved=0x3f EN_OTP_9=1 EN_UPG_27=1 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x08;   /* SH_IND_CLK_ENABLE0 ' enable internal register clocks */
    buf[1]= 0xff;   /* EN_UPG_54=1 EN_PLL_54=1 reserved=0x3 EN_DS_54=1 reserved=0x1 EN_AP_54=1 EN_AP_108=1 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x00;   /* SH_IND_GLOBAL_RESET1 */
    buf[1]= 0x00;   /* AP_WDT_RST=0 UPG_WDT_RST=0 UPG_RST=0 PERIPH_RST=0 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    buf[0]= 0x01;   /* SH_IND_GLOBAL_RESET0 */
    buf[1]= 0x00;   /* OTP_RST=0 TM_RST=0 DS_RST=0 AP_RST=0 CG_RST=0 */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2); 

    for (i=0;i<RETRY_LIMIT;i++) {	/* wait for ready */
        BREG_I2C_Read(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_STATUS), buf, 1);
        if (0==buf[0]) {
            break;
        }
    }

    buf[0]= (DS_TUNER_ANACTL_WDATA_05_ADDR>>24) & 0xff;
    buf[1]= (DS_TUNER_ANACTL_WDATA_05_ADDR>>16) & 0xff;
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_A_31_24), buf, 2); 

    buf[0]= (DS_TUNER_ANACTL_WDATA_05_ADDR>>8) & 0xff;
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_A_15_8), buf, 1); 

    buf[0]= (DS_TUNER_ANACTL_WDATA_05_ADDR>>0) & 0xff;
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_CMD), buf, 1); 

    for (i=0;i<RETRY_LIMIT;i++)	{
        BREG_I2C_Read(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_STATUS), buf, 1);
        if (0==buf[0]) {
            break;
        }
    }

    BREG_I2C_Read(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_D_31_24), buf, 4);
    BDBG_MSG(("DS_TUNER_ANACTL_WDATA_05 = 0x%02x%02x%02x%02x",buf[0],buf[1],buf[2],buf[3]));
    buf[3] &= ~0x8; /* Clear PWRDN_BG_1P2 and write back the register */
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_D_31_24), buf, 4);
    buf[0]= ((DS_TUNER_ANACTL_WDATA_05_ADDR>>0) & 0xff) | 1;
    BREG_I2C_Write(i2cHandle, i2cAddr, DEVICE(SH_SFR_IO_MBOX_CMD), buf, 1); 
}

static void NEXUS_Platform_Frontend_P_UpdateLnaGainTo31xx(void *context, int param)
{
    NEXUS_FrontendHandle handle = (NEXUS_FrontendHandle)context;
    NEXUS_Error errCode;
    NEXUS_31xxConfigSettings configSettings;
    NEXUS_AmplifierStatus lnaStatus;

    BDBG_ASSERT(NULL != handle);
    BSTD_UNUSED(param);

    errCode = NEXUS_Frontend_31xx_GetConfigSettings(handle,&configSettings);
    if ( errCode ) {
        errCode = BERR_TRACE(errCode);
    }
    if (g_LNA) {
        errCode = NEXUS_Amplifier_GetStatus(g_LNA,&lnaStatus);
        if ( errCode ) {
            errCode = BERR_TRACE(errCode);
        }
    }
    else {
        lnaStatus.agcGain = 0x1e;
    }
    configSettings.agcValue = ((lnaStatus.agcGain & 0x1f) | 0x100);
    BDBG_MSG(("%s(%u) setting agcValue = 0x%04x",__FUNCTION__,param,configSettings.agcValue));
    errCode = NEXUS_Frontend_31xx_SetConfigSettings(handle,&configSettings);
    if ( errCode ) {
        errCode = BERR_TRACE(errCode);
    }
}

NEXUS_Error NEXUS_Platform_P_Init3112Frontend(NEXUS_FrontendHandle *pFrontend, unsigned frontend, bool loadAP)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_31xxSettings st31xxSettings;
    NEXUS_31xxConfigSettings configSettings;
    BERR_Code errCode=BERR_SUCCESS;

    NEXUS_Frontend_GetDefault31xxSettings(&st31xxSettings);

    st31xxSettings.i2cDevice = pConfig->i2c[3];     		/* Onboard tuner/demod use BSC 3.*/
	/* EXT_IRQ_0/EXT_IRQ_1 for 31xx. */
    st31xxSettings.isrNumber = (frontend & 0x1) ? (BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_13_CPU_INTR_SHIFT + 32) : 
												  (BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_12_CPU_INTR_SHIFT + 32); 	
    st31xxSettings.i2cAddr = (frontend & 0x1) ? 0x69 : 0x67;
    st31xxSettings.channelNumber = 0;
    st31xxSettings.type = NEXUS_31xxChannelType_eInBand;
    st31xxSettings.ifFrequency = 44000000;          /* Only applicable for 3117 and 3114 OOB module. */
    st31xxSettings.configureWatchdog = false;
	st31xxSettings.loadAP = loadAP;

    *pFrontend = NEXUS_Frontend_Open31xx(&st31xxSettings);

    if(*pFrontend) {
        BDBG_WRN(("Found a 3112 downstream."));
        errCode = NEXUS_Frontend_31xx_GetConfigSettings(*pFrontend,&configSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        else
        {
            configSettings.updateGainCallback.callback = NEXUS_Platform_Frontend_P_UpdateLnaGainTo31xx;
            configSettings.updateGainCallback.context = *pFrontend;
            configSettings.updateGainCallback.param = frontend;
            errCode = NEXUS_Frontend_31xx_SetConfigSettings(*pFrontend,&configSettings);
        }
        NEXUS_Platform_Frontend_P_UpdateLnaGainTo31xx(*pFrontend, frontend); /* set initial LNA gain */
    }
    else {
        BDBG_WRN(("No 3112 frontend found."));
    }


    return errCode;
}
#else
NEXUS_Error NEXUS_Platform_P_Init3112Frontend(NEXUS_FrontendHandle *pFrontend, unsigned frontend, bool loadAP)
{
	BSTD_UNUSED(frontend);
	BSTD_UNUSED(loadAP);
	*pFrontend = NULL;
	return 0;
}
#endif

#ifdef NEXUS_FRONTEND_7125
#include "nexus_frontend_7125.h"
#include "nexus_tuner_7125.h"
#include "nexus_platform_priv.h"
#include "bchp_sub_bnmctl.h"
#include "bchp_sub_ubus_sgisb.h"
#include "breg_endian.h"

static int32_t g_Min_AGC_Threshold = 0;	/* Lower AGC Threshold */
static int32_t g_Max_AGC_Threshold = 0;	/* Upper range of AGC */

static NEXUS_Error NEXUS_Platform_P_GetAgcThresholds(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;

    /* Map physical memory into uncached local address space */
    uint32_t bnmDestBase;
    uint32_t bnmDestMask;
    uint32_t memSize;
	uint32_t offset;
    uint32_t *memBase;

    bool CM_on = NEXUS_Platform_P_CMPowerIsOn();

    if (CM_on) /* if not, you won't be able to read the Base and Mask registers */
    {
        g_Max_AGC_Threshold = 0;
        g_Min_AGC_Threshold = 0;
    
        bnmDestBase = BREG_Read32(hReg, BCHP_SUB_UBUS_SGISB_BNM_DDR_DEST_BASE);   /*(unsigned int *)0xb4880950;*/
        bnmDestMask = BREG_Read32(hReg, BCHP_SUB_UBUS_SGISB_BNM_DDR_WMASK);   /*(unsigned int *)0xb4880958;*/
    
        memSize = ~(bnmDestMask | 0xe0000000) + 1;
        offset = (bnmDestBase & 0xfffffff8);
        
        if ((memSize>15*1024*1024) && (memSize<65*1024*1024))
        {
            memBase = (uint32_t *)NEXUS_Platform_P_MapMemory(offset, memSize, false);
            if (memBase)
            {
                g_Max_AGC_Threshold = memBase[(memSize/(sizeof(uint32_t)))-4];
                g_Min_AGC_Threshold = memBase[(memSize/(sizeof(uint32_t)))-3];
                BREG_BE32(g_Max_AGC_Threshold);
                BREG_BE32(g_Min_AGC_Threshold);
                NEXUS_Platform_P_UnmapMemory(memBase, memSize);
            }
        }
    }
    BDBG_MSG(("%s(): read g_Min_AGC_Threshold = %08x, g_Max_AGC_Threshold = %08x",__FUNCTION__, g_Min_AGC_Threshold,g_Max_AGC_Threshold));
    return NEXUS_SUCCESS;
}

static void NEXUS_Platform_Frontend_P_UpdateLnaGainTo7125(void *context, int param)
{
    NEXUS_FrontendHandle handle = (NEXUS_FrontendHandle)context;
    NEXUS_Error errCode;
    NEXUS_7125ConfigSettings configSettings;
    NEXUS_AmplifierStatus lnaStatus;

    BDBG_ASSERT(NULL != handle);
    BSTD_UNUSED(param);

    errCode = NEXUS_Frontend_7125_GetConfigSettings(handle,&configSettings);
    if ( errCode ) {
        errCode = BERR_TRACE(errCode);
    }
    if (g_LNA) {
        errCode = NEXUS_Amplifier_GetStatus(g_LNA,&lnaStatus);
        if ( errCode ) {
            errCode = BERR_TRACE(errCode);
        }
    }
    else {
        lnaStatus.agcGain = 0x1e;
		lnaStatus.gainBoostEnabled = false;
		lnaStatus.superBoostEnabled = false;
		lnaStatus.tiltEnabled = false;
    }
	configSettings.agcValue = lnaStatus.agcGain;
	configSettings.gainBoostEnabled = lnaStatus.gainBoostEnabled;
	configSettings.superBoostEnabled = lnaStatus.superBoostEnabled;
	configSettings.tiltEnabled = lnaStatus.tiltEnabled;
    errCode = NEXUS_Frontend_7125_SetConfigSettings(handle,&configSettings);
    if ( errCode ) {
        errCode = BERR_TRACE(errCode);
    }
}


static NEXUS_TunerHandle g_tuner[2];
NEXUS_Error NEXUS_Platform_P_Init7125Frontend(NEXUS_FrontendHandle *pFrontend, unsigned channelNumber, bool skipInit)
{
    NEXUS_7125FrontendSettings st7125Settings;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
	NEXUS_7125TunerSettings tunerSettings;
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_7125ConfigSettings configSettings;

	if ( channelNumber > 1 )
	{
		errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto error;
	}

	NEXUS_Tuner_GetDefault7125Settings(&tunerSettings);
    if (NEXUS_GetEnv("tnr_7125_lna_control") == NULL)
	{
		tunerSettings.qamLnaI2c = NULL; /* 7125 tnr driver does not control LNA */
	}
	else
	{
		tunerSettings.qamLnaI2c = pConfig->i2c[3]; /* 7125 tnr driver controls LNA on this I2C bus */
	}
	tunerSettings.unitNumber = channelNumber;
	tunerSettings.skipInit = skipInit;

	if ( (g_Max_AGC_Threshold>g_Min_AGC_Threshold) && (g_Min_AGC_Threshold > (tunerSettings.Min_AGC_Threshold/2)) && 
		 (g_Min_AGC_Threshold < (tunerSettings.Min_AGC_Threshold*2)) && (g_Max_AGC_Threshold > (tunerSettings.Max_AGC_Threshold/2)) && 
		 (g_Max_AGC_Threshold < (tunerSettings.Max_AGC_Threshold*2)) )
	{
		tunerSettings.Min_AGC_Threshold = g_Min_AGC_Threshold;
		tunerSettings.Max_AGC_Threshold = g_Max_AGC_Threshold;
	}

	if (NEXUS_GetEnv("AGC_max")) {
		tunerSettings.Max_AGC_Threshold = NEXUS_atoi(NEXUS_GetEnv("AGC_max"));
		BDBG_MSG(("AGC_max %d",tunerSettings.Max_AGC_Threshold));
	}
	if (NEXUS_GetEnv("AGC_min")) {
		tunerSettings.Min_AGC_Threshold = NEXUS_atoi(NEXUS_GetEnv("AGC_min"));
		BDBG_MSG(("AGC_min %d",tunerSettings.Min_AGC_Threshold));
	}
    BDBG_MSG(( "Max_AGC_Threshold=0x%08x",tunerSettings.Max_AGC_Threshold ));
    BDBG_MSG(( "Min_AGC_Threshold=0x%08x",tunerSettings.Min_AGC_Threshold ));

    g_tuner[channelNumber] = NEXUS_Tuner_Open7125(&tunerSettings);
    if ( NULL == g_tuner[channelNumber] )
    {
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto error;
    }

    NEXUS_Frontend_GetDefault7125Settings(&st7125Settings);

    st7125Settings.channelNumber = channelNumber;
	st7125Settings.devices.tuner = g_tuner[channelNumber];

    *pFrontend = NEXUS_Frontend_Open7125(&st7125Settings);

	if(*pFrontend) {
		BDBG_WRN(("Found a 7125 downstream."));
		errCode = NEXUS_Frontend_7125_GetConfigSettings(*pFrontend,&configSettings);
		if ( errCode )
		{
			return BERR_TRACE(errCode);
		}
		else
		{
			configSettings.updateGainCallback.callback = NEXUS_Platform_Frontend_P_UpdateLnaGainTo7125;
			configSettings.updateGainCallback.context = *pFrontend;
			configSettings.updateGainCallback.param = channelNumber;
			errCode = NEXUS_Frontend_7125_SetConfigSettings(*pFrontend,&configSettings);
		}
		NEXUS_Platform_Frontend_P_UpdateLnaGainTo7125(*pFrontend, channelNumber); /* set initial LNA gain */
	}
    else {
        BDBG_WRN(("No 7125 frontend found."));
    }
error:
    return 0;
}
#endif

void NEXUS_Platform_P_InitQAM(bool allowHost)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    bool CM_on = NEXUS_Platform_P_CMPowerIsOn();
    unsigned i;

    if ((NEXUS_GetEnv("no_3255") == NULL) && CM_on)
	{
        uint32_t hostControlledFrontend = 0;
		/* initialize 3255 frontends */
        #if NEXUS_POWER_MANAGEMENT
        if (NEXUS_SUCCESS != NEXUS_Platform_ReadFrontendControlSharedMemory(&hostControlledFrontend))
        {
            hostControlledFrontend = HOST_CONTROL_NOFRONTEND | HOST_CONTROL_DS2 | HOST_CONTROL_DS3 |
                                     HOST_CONTROL_DS1 | HOST_CONTROL_DS0;
        }
        if (hostControlledFrontend == 0xffffffff)
        {
            hostControlledFrontend = 0; /* unitialized */
        }
        #else
        #define HOST_CONTROL_NOFRONTEND 0xffffffff
        #endif
        if (!(hostControlledFrontend & ~HOST_CONTROL_NOFRONTEND))
        {
#if (NEXUS_PLATFORM==97019)
            const unsigned order[] = {NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1,
                NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0};
#else
            const unsigned order[] = {NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2,
                NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1,
                NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3,
                NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0,
                NEXUS_FRONTEND_ID_CM_CONTROLLED_OOB};
#endif

            #if NEXUS_POWER_MANAGEMENT
            NEXUS_Frontend_3255_PowerUpFrontendDevices(NULL);
            #endif
            
    		for ( i=0;i<(sizeof(order)/sizeof(unsigned));i++ )
    		{
    			NEXUS_Platform_OpenFrontend(order[i]);
    		}
        }
	}

    if ((allowHost) && (NULL==(pConfig->frontend[0])))
    {
        const unsigned int_order[] = {NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1,
            NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0};
        const unsigned int_number = (sizeof(int_order)/sizeof(unsigned));

        const unsigned *order = int_order;
        unsigned n=int_number;

#if (NEXUS_PLATFORM!=97019)
        const unsigned all_order[] = {NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2,
            NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1,
            NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3,
            NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0};
        const unsigned all_number = (sizeof(all_order)/sizeof(unsigned));

        if ((NULL!=NEXUS_GetEnv("no_3255")) && (NULL==NEXUS_GetEnv("host_3112")))
        {   /* For speed, skip 3112 unless requested when no_3255 is specified */
            BDBG_MSG(("Opening Internal frontends for host control."));
        }
        else
        {
            BDBG_MSG(("Opening All frontends for host control."));
            order = all_order;
            n=all_number;
        }
#endif
        for ( i=0;i<n;i++ )
        {
            NEXUS_Platform_OpenFrontend(order[i]);
        }
    }
}

NEXUS_Error NEXUS_Platform_InitFrontend(void)
{
    NEXUS_Platform_P_InitQAM(true);

    return BERR_SUCCESS;
}

void NEXUS_Platform_UninitFrontend(void)
{
    unsigned i;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    bool host = NEXUS_Platform_P_IsHostFrontendPinmux();
    for (i =0; i<NEXUS_MAX_FRONTENDS; i++)
    {
        if ( pConfig->frontend[i] ){
            NEXUS_FrontendUserParameters userParams;
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);

            if (3255==NEXUS_Platform_P_FrontendType(pConfig->frontend[i])) {
                NEXUS_Platform_P_HostFrontendPinmux(false);
            }
            else {
                NEXUS_Platform_P_HostFrontendPinmux(true);
            }
            switch (userParams.id)
            {   /* don't automatically power down 31xx, let user app do it explicitly. */
                #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2
                case NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2:
                    break;
                #endif
                #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3
                case NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3:
                    break;
                #endif
                #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2
                case NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2:
                    break;
                #endif
                #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3
                case NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3:
                    break;
                #endif
                default:
                    BDBG_MSG(("%s(): untuning QAM %d",__FUNCTION__,i));
                    NEXUS_Frontend_Untune(pConfig->frontend[i]);
                    break;
            }
			BDBG_MSG(("%s(): closing QAM %d",__FUNCTION__,i));
			NEXUS_Frontend_Close(pConfig->frontend[i]);
			pConfig->frontend[i] = NULL;
		}
    }
    NEXUS_Platform_P_HostFrontendPinmux(host);

#ifdef NEXUS_FRONTEND_7125
	for (i=0;i<2;i++)
	{
		if ( g_tuner[i] )
		{
			NEXUS_Tuner_Close(g_tuner[i]);
			g_tuner[i] = NULL;
		}
	}
#endif

    if (g_LNA)
    {
        bool host = NEXUS_Platform_P_IsHostFrontendPinmux();
        BDBG_MSG(("closing LNA"));
        NEXUS_Platform_P_HostFrontendPinmux(true);
        NEXUS_Amplifier_Close(g_LNA);
        g_LNA = NULL;
        NEXUS_Platform_P_HostFrontendPinmux(host);
    }
}

#if NEXUS_POWER_MANAGEMENT && (NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY + NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY)
static NEXUS_Error NEXUS_Platform_Frontend_P_SetupHostControl(bool byHost,bool openAllFrontends) 
{
        uint32_t nHostControlledFrontend = HOST_CONTROL_NOFRONTEND;
        bool host = NEXUS_Platform_P_IsHostFrontendPinmux();
        NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;

        if (byHost && !host)
        {
			unsigned i;

			BDBG_MSG(("%s(byHost==true) Transferring from eCM to host.",__FUNCTION__));
			for (i =0; i<NEXUS_MAX_FRONTENDS; i++)
			{
				if ( pConfig->frontend[i] ){
					BDBG_MSG(("closing (eCM-controlled) QAM %d",i));
					NEXUS_Frontend_Close(pConfig->frontend[i]);
					pConfig->frontend[i] = NULL;
				}
			}
            /* Indicate host-controlled frontends to BNM during its boot-up. */
            nHostControlledFrontend = HOST_CONTROL_NOFRONTEND;
			#if (NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY>0)
            nHostControlledFrontend |= HOST_CONTROL_DS2;
			#endif
			#if (NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY>1)
            nHostControlledFrontend |= HOST_CONTROL_DS3;
            #endif
			#if (NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY>0)
            nHostControlledFrontend |= HOST_CONTROL_DS1;
			#endif
			#if (NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY>1)
            nHostControlledFrontend |= HOST_CONTROL_DS0;
            #endif
            BDBG_MSG(("Set host controlled frontend 0x%x to shared memory\n", nHostControlledFrontend));
			/* This shared memory value tells DOCSIS to skip initialization of particular
			frontends as it boots, to avoid disrupting ongoing use. */
            NEXUS_Platform_WriteFrontendControlSharedMemory(nHostControlledFrontend);
			if (openAllFrontends) 
			{
				NEXUS_Platform_P_InitQAM(true); /* Open the host-controlled frontends into the Nexus platform */
			}
        }
        else if (!byHost && host)
        {   
            unsigned i;

            BDBG_MSG(("%s(byHost==false) Transferring from host to eCM.",__FUNCTION__));
            NEXUS_Platform_P_HostFrontendPinmux(false);  /*set pinmux for bnm to use BNM_M_ (I2C) bus for 3112 */
            for (i =0; i<NEXUS_MAX_FRONTENDS; i++)
            {
                if ( pConfig->frontend[i] ){
                    BDBG_MSG(("closing (host-controlled) QAM %d",i));
                    /* note that we deliberately close the host-controlled frontend with pinmux NOT set to host control,
                    to prevent reset of AP when load AP option may have been used.  */
                    NEXUS_Frontend_Close(pConfig->frontend[i]);
                    pConfig->frontend[i] = NULL;
                }
            }
#ifdef NEXUS_FRONTEND_7125
			for (i=0;i<2;i++)
			{
				if ( g_tuner[i] )
				{
					BDBG_MSG(("closing (host-controlled) 7125 tuner %d",i));
					NEXUS_Tuner_Close(g_tuner[i]);
					g_tuner[i] = NULL;
				}
			}
#endif

            nHostControlledFrontend = HOST_CONTROL_NOFRONTEND;
            BDBG_MSG(("Set host controlled frontend 0x%x to shared memory\n", nHostControlledFrontend));
            NEXUS_Platform_WriteFrontendControlSharedMemory(nHostControlledFrontend);

			if (openAllFrontends) 
			{
				NEXUS_Platform_P_InitQAM(true); /* Open the eCM-controlled frontends into the Nexus platform */
			}
        }

        return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_Frontend_EnableHostControl(bool openAllFrontends) 
{
    NEXUS_Platform_Frontend_P_SetupHostControl(true,openAllFrontends);
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_Frontend_DisableHostControl(bool openAllFrontends) 
{
    NEXUS_Platform_Frontend_P_SetupHostControl(false,openAllFrontends);
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_Frontend_GetFrontendInstanceControlled(NEXUS_FrontendHandle * pControlledFrontend, int index)
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;

    *pControlledFrontend = pConfig->frontend[index];
    BDBG_MSG(("%s(0x%x,%d)",__FUNCTION__, *pControlledFrontend, index));
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_ShutdownCM(void)
{
	BDBG_MSG(("Shutting down CM"));
	NEXUS_Platform_P_ShutdownCM();
    return BERR_SUCCESS;
}

#else
NEXUS_Error NEXUS_Platform_Frontend_EnableHostControl(bool openAllFrontends) 
{
	BSTD_UNUSED(openAllFrontends);
    return BERR_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Platform_Frontend_DisableHostControl(bool openAllFrontends) 
{
	BSTD_UNUSED(openAllFrontends);
    return BERR_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Platform_Frontend_GetFrontendInstanceControlled(NEXUS_FrontendHandle * pControlledFrontend, int index)
{
    BSTD_UNUSED(pControlledFrontend);
    BSTD_UNUSED(index);
    
    return BERR_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Platform_ShutdownCM(void)
{
    return NEXUS_NOT_SUPPORTED;
}
#endif

#if NEXUS_POWER_MANAGEMENT
NEXUS_Error NEXUS_Platform_PowerUpLNA(void)
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
    bool host = NEXUS_Platform_P_IsHostFrontendPinmux();
    NEXUS_AmplifierSettings settings;

    if (g_LNA) {
        NEXUS_Platform_P_HostFrontendPinmux(true);
        NEXUS_Amplifier_GetSettings(g_LNA,&settings);
    
        settings.enabled = true;
        errCode = NEXUS_Amplifier_SetSettings(g_LNA,&settings);
        if (errCode) 
        {
            errCode = BERR_TRACE(errCode);
        }
    
        NEXUS_Platform_P_HostFrontendPinmux(host);
    }
    return errCode;
}

NEXUS_Error NEXUS_Platform_PowerDownLNA(void)
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
	if (g_LNA) {
		bool host = NEXUS_Platform_P_IsHostFrontendPinmux();
		NEXUS_AmplifierSettings settings;
	
		NEXUS_Platform_P_HostFrontendPinmux(true);
		NEXUS_Amplifier_GetSettings(g_LNA,&settings);
	
		settings.enabled = false;
		errCode = NEXUS_Amplifier_SetSettings(g_LNA,&settings);
		if (errCode) 
		{
			errCode = BERR_TRACE(errCode);
		}
	
		NEXUS_Platform_P_HostFrontendPinmux(host);
	}
    return errCode;
}
#else
NEXUS_Error NEXUS_Platform_PowerUpLNA(void)
{
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Platform_PowerDownLNA(void)
{
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}
#endif




BTRC_MODULE(ChnChange_TuneStreamer, ENABLE);

NEXUS_Error
NEXUS_Platform_GetStreamerInputBand(unsigned index, NEXUS_InputBand *pInputBand)
{
    BDBG_ASSERT(pInputBand);
	#if (NEXUS_PLATFORM==97019)	
	BDBG_ERR(("no inputs available"));
	BSTD_UNUSED(index);
	return BERR_TRACE(BERR_INVALID_PARAMETER);
	#else
    if (index > 2) {
        BDBG_ERR(("Only 3 streamer inputs available"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BTRC_TRACE(ChnChange_TuneStreamer, START);
	switch (index)
	{
		default:
		case 0: 
			*pInputBand = NEXUS_InputBand_e1; /* FPGA on PKT1 pins */
			break;
		case 1: 
			*pInputBand = NEXUS_InputBand_e3; /* FPGA on PKT1 pins */
			BDBG_WRN(("STREAMER 1 IB3 for DS1"));
			break;
		case 2: 
#if 0
			*pInputBand = NEXUS_InputBand_e2; /* FPGA on PKT1 pins */
			BDBG_WRN(("STREAMER 2 IB2 for DS0"));
#else
			*pInputBand = NEXUS_InputBand_e0; /* FPGA on PKT1 pins */
			BDBG_WRN(("STREAMER 2 IB0 for 3112"));
#endif
			break;
	}
    BTRC_TRACE(ChnChange_TuneStreamer, STOP);
	#endif
    return NEXUS_SUCCESS;
}

NEXUS_FrontendHandle NEXUS_Platform_Frontend_P_Open3255(unsigned channelNumber, NEXUS_3255ChannelType type)
{
    NEXUS_FrontendHandle frontend = NULL;
    bool CM_on = NEXUS_Platform_P_CMPowerIsOn();
    if ((NEXUS_GetEnv("no_3255") == NULL) && CM_on)
	{
        NEXUS_3255Settings st3255Settings;
		NEXUS_Platform_P_HostFrontendPinmux(false); /* set pinmux for BNM to use BSC_M3 (I2C) bus for 3112 */
        NEXUS_Frontend_GetDefault3255Settings(&st3255Settings);
        st3255Settings.channelNumber = channelNumber;
        st3255Settings.type = type;
        BDBG_MSG(("Opening onboard 3255 %d", channelNumber));
        frontend = NEXUS_Frontend_Open3255(&st3255Settings);
		NEXUS_Platform_P_GetAgcThresholds();
	}
    return frontend;
}

NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see 
                   nexus_platform_features.h for possible platform-specific macros.
                */
    )
{
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_InputBandSettings inputBandSettings;
    NEXUS_FrontendHandle frontend = NULL;
    unsigned i;
    bool load_AP = false;
    const char *load_AP_env=NEXUS_GetEnv("load_AP");
    if (load_AP_env) {
        load_AP = (load_AP_env[0]!='n'&&load_AP_env[0]!='N');
    }

    for (i=0;i<NEXUS_MAX_FRONTENDS;i++)
    {
        if (pConfig->frontend[i])
        {   /* all frontends must be either host or CM controlled */
            NEXUS_Frontend_GetUserParameters(pConfig->frontend[i], &userParams);
            if ( ((id < NEXUS_FRONTEND_ID_HOST_CONTROLLED) && (userParams.id >= NEXUS_FRONTEND_ID_HOST_CONTROLLED)) ||
                 ((id >= NEXUS_FRONTEND_ID_HOST_CONTROLLED) && (userParams.id < NEXUS_FRONTEND_ID_HOST_CONTROLLED)) )
            {
                BDBG_ERR(("%s(%u) Can't open %s controlled frontend, %s controlled already opened.",
                          __FUNCTION__,id,(id < NEXUS_FRONTEND_ID_HOST_CONTROLLED)?"CM":"host",
                         (id < NEXUS_FRONTEND_ID_HOST_CONTROLLED)?"host":"CM"));
                return NULL;
            }
            if (userParams.id==id)
            {
                BDBG_MSG(("%s(%u) returning already open pConfig->frontend[%u]",__FUNCTION__,id,i));
                return pConfig->frontend[i];
            }
        }
    }

    if (NULL==g_LNA) /* (first time) */
    {   /* open LNA for power control, or to initialize it for host frontend control */
        NEXUS_3412Settings lnaSettings;
        bool host = (id >= NEXUS_FRONTEND_ID_HOST_CONTROLLED);

        BDBG_MSG(("Opening LNA (%s)",host ? "host":"BNM"));
        NEXUS_Platform_P_HostFrontendPinmux(true);

        NEXUS_Amplifier_GetDefault3412Settings(&lnaSettings);
        lnaSettings.i2cDevice = g_NEXUS_platformHandles.config.i2c[3];
        lnaSettings.i2cAddr = 0x0; /* probe for it */
        lnaSettings.skipInit = (!host);
        BDBG_MSG(("%sinitializing LNA",lnaSettings.skipInit ? "NOT ":""));
        g_LNA = NEXUS_Amplifier_Open3412(&lnaSettings);
        if (!g_LNA) {
            BDBG_ERR(("Failed to open LNA."));
        }
        NEXUS_Platform_P_HostFrontendPinmux(host);
    }

    switch (id)
    {
        #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0
        case NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0:
            frontend = NEXUS_Platform_Frontend_P_Open3255(0, NEXUS_3255ChannelType_eInBand);
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1
        case NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1:
            frontend = NEXUS_Platform_Frontend_P_Open3255(1, NEXUS_3255ChannelType_eInBand);
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0
        case NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0:
		case NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0_SKIP_INIT:
            BDBG_MSG(("Opening internal 7125 DS0%s",(id==NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0_SKIP_INIT)?" skip init":""));
            NEXUS_Platform_P_HostFrontendPinmux(true);
            NEXUS_Platform_P_Init7125Frontend(&frontend,0,(id==NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0_SKIP_INIT));
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2
            NEXUS_Platform_P_Bypass3112(2);
            #endif
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3
            NEXUS_Platform_P_Bypass3112(3);
            #endif
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1
        case NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1:
		case NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1_SKIP_INIT:
			BDBG_MSG(("Opening internal 7125 DS1%s",(id==NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1_SKIP_INIT)?" skip init":""));
            NEXUS_Platform_P_HostFrontendPinmux(true);
            NEXUS_Platform_P_Init7125Frontend(&frontend,1,(id==NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1_SKIP_INIT));
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2
            NEXUS_Platform_P_Bypass3112(2);
            #endif
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3
            NEXUS_Platform_P_Bypass3112(3);
            #endif
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2
        case NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2:
            frontend = NEXUS_Platform_Frontend_P_Open3255(2, NEXUS_3255ChannelType_eInBand);
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3
        case NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3:
            frontend = NEXUS_Platform_Frontend_P_Open3255(3, NEXUS_3255ChannelType_eInBand);
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_OOB
        case NEXUS_FRONTEND_ID_CM_CONTROLLED_OOB:
            frontend = NEXUS_Platform_Frontend_P_Open3255(4, NEXUS_3255ChannelType_eOutOfBand);
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2
        case NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2:
            BDBG_MSG(("Opening onboard 3112 A"));
            NEXUS_Platform_P_HostFrontendPinmux(true);
            NEXUS_Platform_P_Init3112Frontend(&frontend,2,load_AP);
            if (!frontend)
            {
                BDBG_MSG(("Failed to open onboard 3112 A without loading AP, re-try, loading it."));
                NEXUS_Platform_P_Init3112Frontend(&frontend,2,true);
            }
            break;
        #endif
        #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3
        case NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3:
            BDBG_MSG(("Opening onboard 3112 B"));
            NEXUS_Platform_P_HostFrontendPinmux(true);
            NEXUS_Platform_P_Init3112Frontend(&frontend,3,load_AP);
            if (!frontend)
            {
                BDBG_MSG(("Failed to open onboard 3112 B without loading AP, re-try, loading it."));
                NEXUS_Platform_P_Init3112Frontend(&frontend,3,true);
            }
            break;
        #endif
        default:
            break;
    }

    if ( frontend )
    {
        unsigned i;
        NEXUS_Frontend_GetUserParameters(frontend, &userParams);
        switch (id)
        {
            #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0
            case NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0:
                userParams.param1 = NEXUS_InputBand_e2;	/* DS0 reserved for DOCSIS */
                userParams.chipId = 0x3255;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = false;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0 
            case NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0:
                userParams.param1 = NEXUS_InputBand_e2;	/* DS0 reserved for DOCSIS */
                userParams.chipId = 0x7125;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = false;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1
            case NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1:
                userParams.param1 = NEXUS_InputBand_e3;	/* DS1 */
                userParams.chipId = 0x3255;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = false;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1
            case NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1:
                userParams.param1 = NEXUS_InputBand_e3;	/* DS1 */
                userParams.chipId = 0x7125;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = false;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2
            case NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS2:
                userParams.param1 = NEXUS_InputBand_e0; /* BCM3112 on PKT0 */
                userParams.chipId = 0x3255;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = true;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2
            case NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS2:
                userParams.param1 = NEXUS_InputBand_e0; /* BCM3112 on PKT0 */
                userParams.chipId = 0x3112;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = true;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3
            case NEXUS_FRONTEND_ID_CM_CONTROLLED_EXTERNAL_31XX_DS3:
                userParams.param1 = NEXUS_InputBand_e1; /* BCM3112 on PKT1 */
                userParams.chipId = 0x3255;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = true;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            #ifdef NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3
            case NEXUS_FRONTEND_ID_HOST_CONTROLLED_EXTERNAL_31XX_DS3:
                userParams.param1 = NEXUS_InputBand_e1; /* BCM3112 on PKT1 */
                userParams.chipId = 0x3112;
                NEXUS_InputBand_GetSettings(userParams.param1, &inputBandSettings);
                inputBandSettings.clockActiveHigh = true;
                inputBandSettings.parallelInput = false;
                NEXUS_InputBand_SetSettings(userParams.param1, &inputBandSettings);
                break;
            #endif
            default:
                break;
        }
        userParams.id = id;
        userParams.chipRevision = 0; /* platform not using this for now */

        NEXUS_Frontend_SetUserParameters(frontend, &userParams);

        /* Store in config structure */
        for ( i=0; i< NEXUS_MAX_FRONTENDS; i++)
        {
            if (NULL==pConfig->frontend[i])
            {
                BDBG_MSG(("Added pConfig->frontend[%u]",i));
                pConfig->frontend[i] = frontend;
                break;
            }
        }
    }

    return frontend;
}


