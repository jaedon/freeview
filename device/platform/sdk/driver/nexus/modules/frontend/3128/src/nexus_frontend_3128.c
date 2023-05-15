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
* $brcm_Workfile: nexus_frontend_3128.c $
* $brcm_Revision: 115 $
* $brcm_Date: 12/6/12 4:02p $
*
* API Description:
*   API name: Frontend 3128
*    APIs to open, close, and setup initial settings for a BCM3128
*    Frontend Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/3128/src/nexus_frontend_3128.c $
* 
* 115   12/6/12 4:02p vishk
* SW3128-240: Bug fix.
* 
* 114   11/12/12 9:54a vishk
* SW7231-1059, SW7231-1057: Enabling setting of scan parameters even for
*  eAuto mode.
* 
* 113   11/9/12 1:55p vishk
* SW3461-206: Add support for 3128 frontend.
* 
* 111   11/1/12 11:17a vishk
* SW3128-240: Power down OOB if transport is not chosen as an option for
*  wakeup
* 
* 110   10/30/12 3:44p vishk
* SW7425-4119: set OOB output mode default to Max, to enable setting it
*  when SetConfigSettings is called the first time.
* 
* 109   10/26/12 3:28p vishk
* SW3461-206: Bug fix.
* 
* 108   10/22/12 4:52p vishk
*  SW7425-4119: Bug fix.
* 
* 107   10/21/12 5:43p vishk
* SW7425-4119: set OOB FEC enable parameter before OOB type opening 3128
* 
* 106   10/21/12 3:43p vishk
* SW3461-270: Add API Support for reading new frontend F/W version ids
*  312X family of frontends.
* 
* 105   10/11/12 3:09p vishk
* SW7425-4007: fix comiler warning.
* 
* 104   10/8/12 2:52p erickson
* SW7435-372: add NEXUS_Frontend_ReadSoftDecisions
* 
* 103   10/3/12 5:41p vishk
* SW7425-4007: CTRL-C causes kernel crash/hang at rmmod with 3128 FE
* 
* 102   10/3/12 5:01p jtna
* SW7425-3782: refactor nexus tsmf impl
* 
* 101   9/28/12 1:56p vishk
* SW7429-313: Settop crash for 97425VMSFF platform after coming out of
*  standby mode.
* 
* 100   9/26/12 2:46p vishk
* SW3128-224: Do not store the previous agcValue to force 3128 to
*  recalibrate the lna everytime it is written.
* 
* 99   9/7/12 2:02p vishk
* SW7425-3765, SW7425-3788: Flatten out directory structures.
* 
* 98   9/6/12 12:05p jtna
* SW7425-3782: wrap NEXUS_Frontend_P_3128_ReapplyTransportSettings
*  correctly
* 
* 97   9/5/12 6:10p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
* 
* 96   8/16/12 4:15p vishk
* SW3128-208: Add power standby feature for OOB as well.
* 
* 95   8/1/12 4:02p vishk
* SW7231-907, SW7425-3519: Add Standby functionality support for 3128.
*  Tuners lost and resource leak when doing channel-change in PIP window.
* 
* 94   7/19/12 4:14p jtna
* SW3472-14: add 3472 MXT support. change MTSIF in API naming to
*  MTSIF_TX, in order to differentiate with MTSIF_RX
* 
* 92   7/6/12 11:54a vishk
* SW3128-191: OOB synchronous lock status reported uncorrectly. Also,
*  remove support for A0.
* 
* 91   6/28/12 5:17p vishk
* SW3128-184: Set isPoweredOn variable according to aob state.
* 
* 90   6/27/12 3:06p jtna
* SW7425-3350: remove NEXUS_FRONTEND_FIXED_MTSIF_CONFIG compile option.
*  it's no longer needed
* 
* 89   6/26/12 3:35p jtna
* SW7425-3062: add feature to propagate host parserband settings to
*  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
* 
* 88   6/8/12 3:28p jtna
* SW7425-3171: add NEXUS_Frontend_ReapplyTransportSettings
* 
* 87   6/6/12 7:01p jtna
* SW7425-2752: fix OOB data routing
* 
* 86   5/17/12 6:03p vishk
* SW3128-149: Bug fix in calculating time intervals.
* 
* 85   5/17/12 1:23p vishk
* SW3128-164: Bug fix. Return error only if the channel number is
*  incorrect.
* 
* 84   5/17/12 1:07p vishk
* SW3128-164: Add Out Of Band Input Mode Selection
* 
* WideBandAtoD/2   5/17/12 1:06p vishk
* SW3128-164: Add Out Of Band Input Mode Selection
* 
* WideBandAtoD/1   5/16/12 2:30p vishk
* SW3128-164 : Add selection to choose the A to D converter for the Out
*  of Band RF Input.
* 
* 83   5/15/12 5:44p jtna
* SW7425-2752: disable all parsers after MXT open
* 
* 82   5/15/12 3:01p vishk
* SW7552-269, SW3128-155, SW3461-198: Add NEXUS_Frontend_WriteRegister()
*  and NEXUS_Frontend_ReadRegister().
* 
* 81   5/13/12 6:30p jtna
* SW7425-2752: rework #if NEXUS_HAS_MXT sections
* 
* 80   5/13/12 5:06p jtna
* SW7425-2752: add NEXUS_FRONTEND_FIXED_MTSIF_CONFIG option to support
*  legacy mode
* 
* 79   5/10/12 7:41p jtna
* SW7425-2752: fix compile for non-MXT platforms. fix unrelated AOB
*  compile issue
* 
* 78   5/10/12 7:20p jtna
* SW7425-2752: keep demod parsers enabled until we go live
* 
* 77   5/10/12 6:53p jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
* 
* 76   5/9/12 12:45p vishk
* SW3128-158: Fix NEXUS_Frontend_P_3128_GetFastStatus to returns correct
*  lockStatus for OOB
* 
* 75   5/7/12 11:37a vishk
* SW3128-149: Add support for Post Reed Solomon bit error rate.
* 
* 74   5/4/12 5:06p vishk
* SW3128-149:  Add acquisition time and total acquisition time.
* 
* 73   4/23/12 2:18p vishk
* SW3128-145: Disabling the debug message.
* 
* 72   4/23/12 11:58a vishk
* SW3128-145: Calling reset status errors out if the downstream core is
*  not powered up.
* 
* 71   4/18/12 11:57a vishk
* SW7552-227: Multiply the AnnexA/B fix after the comparision between the
*  total number of blocks vs the uncorrected blocks.
* 
* 70   4/16/12 3:00p vishk
* SW3128-136: NEXUS_Frontend_P_3128_GetQamStatus returns all zeros in
*  status structure.
* 
* 69   4/13/12 6:38p vishk
* SW3128-141: Reset spectrumDataLength correctly, remove installing and
*  uninstalling of the callbacks, remove powering up and down of ads and
*  baob cores for spectrum data retrieval.
* 
* 68   4/9/12 3:04p jtna
* SW3128-131: wrap with #if NEXUS_FRONTEND_312x_OOB
* 
* 67   4/9/12 12:17p jtna
* SW3128-131: added NEXUS_FrontendOutOfBandOutputMode to
*  NEXUS_3128ConfigSettings
* 
* 66   3/30/12 12:17p jtna
* SW3128-140: add spectrum analyzer feature to TNR PI
* 
* 65   3/15/12 4:52p vishk
* SW3128-133, SW3128-134: Annex not reported by scan modType not set
*  during scan
* 
* 64   3/9/12 12:16p vishk
* SW3128-120, SW3128-128: Add highResEqualizerGain to the status.
* 
* 63   3/7/12 5:12p vishk
* SW3128-128: Update frontendGain and digitalAgcGain.
* 
* 62   3/7/12 1:26p vishk
* SW7552-183: fix compilation error.
* 
* 61   3/7/12 1:06p vishk
* SW7552-183: Update posRsBer and viterbiErrorRate correctly.
* 
* 60   2/29/12 10:48a vishk
* SW3128-117: Event callback need to be registered only once as its a one
*  to many mapping from event to callbacks.
* 
* 59   2/27/12 12:23p vishk
* SW3128-120: Fix channel power calculation.
* 
* 58   2/24/12 2:15p vishk
* SW3128-120, SW3128-110: Add Nyquist filter selection and add frontend
*  gain, equalizer gain and digital agc.
* 
* 57   2/16/12 12:06p vishk
* SW3128-125: Handle lockchange callbacks to app getting collapsed in
*  nexus.
* 
* 56   2/15/12 3:12p vishk
* SW3128-123: Populate Viterbi error rate, corrected bits and uncorrected
*  bits in the Qam status Update fecClean correctly.
* 
* 55   2/15/12 2:37p vishk
* SW3128-123: Populate Viterbi error rate, corrected bits and uncorrected
*  bits in the Qam status
* 
* 54   2/15/12 11:53a vishk
* SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
*  and GetDefaultAcquireParams.
* 
* 53   2/14/12 12:32p vishk
* SW7231-649: 31xx failure during tune_qam.  Call BADS_SetAcquireParams()
*  before calling BADS_Acquire().
* 
* 52   2/6/12 4:21p jtna
* SW7425-2267: fix indentation
* 
* 51   2/1/12 2:10p jtna
* SW3128-84: avoid calling NEXUS_CancelTimer with NULL
* 
* 50   1/26/12 4:20p vishk
* SW3461-135: Add Automatic Tune / Acquire Flags to THD and TC2 PI Bug
*  fix for 3128.
* 
* 49   1/18/12 3:04p jtna
* SW7425-2190: Coverity Defect ID:36489 OVERRUN_STATIC
* 
* 48   1/18/12 12:29p vishk
* SW3128-99: Add nexus support for spectrum data retreival.
* 
* 3128_SPECTRUM_DATA/3   1/18/12 10:44a vishk
* SW3128-99: Add nexus support for spectrum data retreival.
* 
* 3128_SPECTRUM_DATA/2   1/4/12 12:21p vishk
* SW7425-1924: Coverity Defect ID:36187 OVERRUN_STATIC
*  nexus_frontend_3128.c Product=97425
* 
* 3128_SPECTRUM_DATA/1   12/22/11 4:56p vishk
* SW3128-99:Add nexus support for spectrum data retreival.
*
* 47   1/13/12 12:34p jtna
* SW3128-84: wrap NEXUS_Frontend_P_3128_MxtTimeout with #if NEXUS_HAS_MXT
* 
* 46   1/12/12 11:51a jtna
* SW3128-84: merge NEXUS_FrontendTransportSettings callbacks
* 
* 45   1/6/12 5:42p vishk
* SW7425-1786: all chip-specific frontend API's must validate against an
*  always-available chip id before dereferencing chip-specific data
*  structures. Make the function static.
* 
* 44   1/6/12 4:44p vishk
* SW7425-1786: all chip-specific frontend API's must validate against an
*  always-available chip id before dereferencing chip-specific data
*  structures
* 
* 43   12/19/11 3:10p vishk
* SW7425-1991: Corrupted OOB data
* 
* 42   12/16/11 10:04p vishk
* SW3128-92: Add FW auto detection support Add NEXUS_FRONTEND_312x_OOB
*  for conditional compilation of 3128's OOB.
* 
* 41   12/16/11 5:16p vishk
* SW3128-92: Add FW auto detection support. Bug fix.
* 
* 40   12/16/11 2:46p vishk
* SW3128-92: Add FW auto detection support Add probe api to get the chip
*  Id.
* 
* 39   12/15/11 5:57p vishk
* SW3128-92: Add FW auto detection support. Add nexus support.
* 
* 38   12/14/11 2:58p vishk
* SW3128-76, SW7425-1957: NEXUS_Frontend_GetOutOfBandStatus prints "Error
*  reading status." SW3128-76: Add "No Signal" support to
*  BAOB_GetLockStatus API
* 
* 37   12/6/11 10:55a vishk
* SW7231-76: Add support for FBTSFF 97231 + 3128 board. Fix for
*  accumulated vs non-accumulated corrected counts in the PI.
* 
* 36   12/5/11 4:17p vishk
* SW7552-154: Missing ADS Scan Parameter from 7552 Nexus structure.
* 
* 35   12/1/11 5:56p vishk
* SW7425-1722: - NEXUS_Frontend_GetQamStatus prints "Error reading
*  status." and returned NEXUS_TIMEOUT. Code clean up and removal of
*  isAsyncStatusReady.
* 
* 34   11/29/11 5:17p vishk
* SW7425-1722: - NEXUS_Frontend_GetQamStatus prints "Error reading
*  status." and returned NEXUS_TIMEOUT
* 
* 33   11/17/11 4:42p vishk
* SW3461-96: Remove GetLockStatus and replace it with GetFastStatus
* 
* 32   11/11/11 4:29p vishk
* SW3128-72: Add Automatic Tune / Acquire Flags to Tune and Acquire
*  Params in Nexus. Power on the cores conditionally.
* 
* 31   11/11/11 3:16p vishk
* SW3128-72, SW3128-64: Add Automatic Tune / Acquire Flags to Tune and
*  Acquire Params in Nexus  SW3128-64: SW driver for LNA control from
*  BCM7425 host chip
* 
* 30   11/4/11 4:45p vishk
* SW3461-83: Means of returning BCM312x chip revsion and firmware
*  revision via NEXUS i/f
* 
* 29   10/31/11 1:46p vishk
* SW7425-1598: Add support for 3128B0/C0 Adding default chip Id.
* 
* 28   10/21/11 7:27p jtna
* SW7425-1483: call 3128-specific implementation of MXT
* 
* 27   10/14/11 6:08p jtna
* SW7425-1327: if requested, disable MTSIF explicitly and don't rely on
*  BMXT_GetDefaultSettings
* 
* 26   10/14/11 2:53p jtna
* SW7425-1189: added NEXUS_TsmfSettings.frontendTsmfIndex. allow change
*  between auto and semi-auto mode without enable/disable.
* 
* 25   10/12/11 7:31p vishk
* SW3128-16: Add scan support to 3128
* 
* 24   10/11/11 7:00p vishk
* SW3128-46: add support for BCM3124 devices Fix comment.
* 
* 23   10/11/11 6:59p vishk
* SW3128-46: add support for BCM3124 devices
* 
* 22   10/7/11 2:32p vishk
* SW3128-47: Add Support for BCM3123
* 
* 21   10/6/11 5:23p jtna
* SW7425-1327: add NEXUS_Frontend_Get/SetTransportSettings()
* 
* 20   10/6/11 4:50p jtna
* SW7425-1189: change return type to void. make tsmfSettings per frontend
*  handle, not per device.
* 
* 19   10/5/11 12:25p jtna
* SW7425-1189: add transport and frontend TSMF support
* 
* 18   10/4/11 7:41p vishk
* SW3128-52: Once tuned to a certain frequency, can't tune to any other
*  frequencies on 3123.
* 
* 17   9/26/11 6:36p randyjew
* SW7344-140: Add 3123 support for 7418SFF_H board
* 
* 16   8/26/11 8:26a vishk
* SW7425-838: transcode_ts exited with assertion sometimes for Tuner/HDMI
*  source
* 
* 15   8/4/11 11:18a vishk
* SW7231-76: Add support for FBTSFF 97231 + 3128 board. Bug fix.
* 
* 14   8/4/11 11:17a vishk
* SW7231-76: Add support for FBTSFF 97231 + 3128 board. Fix constellation
*  scale factor.
* 
* 13   7/8/11 2:37p vishk
* SW7231-76: Add support for FBTSFF 97231 + 3128 board. Add support for
*  setting agcvalue.
* 
* 12   6/9/11 12:29p nickh
* SW7425-675: Remove warning
* 
* 11   6/8/11 7:44p vishk
* SW7425-675: SW3128-9:  Frontend not working with the latest code.
*  Untune of one channel causes other existing channels to untune as
*  well. Temporary fix till the pi and firware is fixed to untune
*  individual tuner channels.
* 
* 10   6/6/11 8:48p vishk
* SW7425-675: frontend not working with the latest code. Implement Qam
*  and Oob GetStatus using AsyncGetStatus.
* 
* 9   6/3/11 5:23p vishk
* SW7231-180:convert BADS_AutomaticFrequencyOffset and
*  NEXUS_FrontendQamFrequencyOffset enum types to unsigned integer.
* 
* 8   5/11/11 2:41p vishk
* SW3128-10, SW3128-2 : Refactoring of
*  NEXUS_Frontend_3128_Get/SetConfigSettings()  Added support to
*  configure TM_MTSIF_CTRL - Transport Control
* 
* 7   4/20/11 10:59a vishk
* SW3128-7: Channel change to tune to the next set of pids does not work
*  while using brutus. Temporary hack for the lack of in-lock interrupt
*  removed.
* 
* 6   4/18/11 6:18p vishk
* SW3128-7: Channel change to tune to the next set of pids does not work
*  while using brutus. This is a temporary HACK for the lack of lock
*  interrupt.
* 
* 5   4/18/11 4:42p spothana
* SW7425-369 : Enable BCM3128 OOB when NEXUS_FRONTEND_3128_OOB is
*  enabled.
* 
* 4   4/15/11 4:37p vishk
* SW7422-413: Nexus support for the 3128 OOB core for 7422 board
* 
* 3   4/13/11 7:18p vishk
* SW7425-328: Hab timeout seen upon NEXUS_Frontend_Untune()
* 
* 2   3/16/11 11:53a vishk
* SW7231-76 : Add support for FBTSFF 97231 + 3128 board.
* 
* 1   3/4/11 3:32p erickson
* SW7422-171: add 3128
*
***************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_platform_features.h"
#include "nexus_i2c.h"
#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_gpio_priv.h"
#include "btnr.h"
#include "btnr_3128ib.h"
#include "bhab_3128.h"
#include "bads_3128.h"
#include "bhab_3128_fw.h"
#include "bhab.h"
#include "bads.h"
#include "../../c0/bchp_tm.h"
#include "bchp_hsi.h"
#include "bhab_312x_priv.h"

#if NEXUS_HAS_MXT
#include "bmxt.h"
#include "bmxt_3128.h"
#endif

#if NEXUS_FRONTEND_312x_OOB
#include "baob.h"
#include "btnr_3128ob.h"
#endif

BDBG_MODULE(nexus_frontend_3128);

BDBG_OBJECT_ID(NEXUS_3128);

/* Currently there are eight ADS channels and one AOB channel on 3128 */
#define NEXUS_MAX_3128_FRONTENDS (NEXUS_3128_MAX_DOWNSTREAM_CHANNELS + NEXUS_3128_MAX_OUTOFBAND_CHANNELS)

/* The OOB channel number on 3128 */
#define NEXUS_3128_OOB_CHN      NEXUS_3128_MAX_DOWNSTREAM_CHANNELS

typedef struct NEXUS_PreviousStatus
{
    unsigned fecCorrected;
    unsigned fecUncorrected;
    unsigned fecClean;
    unsigned viterbiUncorrectedBits;
    unsigned viterbiTotalBits;
    NEXUS_Time time;
}NEXUS_PreviousStatus;

/***************************************************************************
 * Internal Module Structure
 ***************************************************************************/
typedef struct NEXUS_3128
{
    BDBG_OBJECT(NEXUS_3128)
    BLST_S_ENTRY(NEXUS_3128) node;
    uint16_t  chipFamilyId;
    uint16_t  chipId;
    BHAB_Handle hab;
    unsigned    isrNumber;
    NEXUS_GpioHandle gpioInterrupt;
    uint16_t    i2cAddr;
    NEXUS_I2cHandle i2cHandle;
    unsigned    numfrontends;
    unsigned    numChannels;
    BADS_Handle ads;
    BTNR_Handle tnr[NEXUS_MAX_3128_FRONTENDS];
    NEXUS_FrontendHandle    frontendHandle[NEXUS_MAX_3128_FRONTENDS];
    BADS_ChannelHandle  ads_chn[NEXUS_3128_MAX_DOWNSTREAM_CHANNELS];
    NEXUS_FrontendQamSettings   qam[NEXUS_3128_MAX_DOWNSTREAM_CHANNELS];
    NEXUS_FrontendOutOfBandSettings oob;
    NEXUS_FrontendQamSettings   last_ads[NEXUS_3128_MAX_DOWNSTREAM_CHANNELS];
    BKNI_EventHandle isrEvent;
    NEXUS_EventCallbackHandle isrEventCallback;
    NEXUS_IsrCallbackHandle lockAppCallback[NEXUS_MAX_3128_FRONTENDS];
    NEXUS_IsrCallbackHandle updateGainAppCallback;
    NEXUS_IsrCallbackHandle asyncStatusAppCallback[NEXUS_MAX_3128_FRONTENDS];
    NEXUS_TaskCallbackHandle spectrumDataAppCallback[NEXUS_MAX_3128_FRONTENDS];
    bool                    isInternalAsyncStatusCall[NEXUS_MAX_3128_FRONTENDS];
    uint16_t revId;
    NEXUS_3128ConfigSettings deviceSettings;
    bool isMtsif;
    bool isPoweredOn[NEXUS_MAX_3128_FRONTENDS];
    uint32_t *spectrumDataPointer;
    unsigned spectrumDataLength;
    BKNI_EventHandle spectrumEvent;
    NEXUS_EventCallbackHandle spectrumEventCallback;
    bool acquireInProgress[NEXUS_MAX_3128_FRONTENDS];
    unsigned count[NEXUS_MAX_3128_FRONTENDS];
    NEXUS_PreviousStatus previousStatus[NEXUS_MAX_3128_FRONTENDS];
    NEXUS_FrontendDevice *pGenericDeviceHandle;
#if NEXUS_FRONTEND_312x_OOB
    BAOB_Handle aob;
    NEXUS_FrontendOutOfBandSettings last_aob;
#endif
#if NEXUS_HAS_MXT
    BMXT_Handle mxt;
    NEXUS_FrontendDeviceMtsifConfig mtsifConfig;
#endif

} NEXUS_3128;

static BLST_S_HEAD(devList, NEXUS_3128) g_3128DevList = BLST_S_INITIALIZER(g_3128DevList);


typedef struct NEXUS_3128Channel
{
    unsigned chn_num; /* channel number */
    NEXUS_3128 *pDevice; /* 3128 device*/
} NEXUS_3128Channel;

/***************************************************************************
 * Module callback functions for tuning
 ***************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_3128_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings);
static void NEXUS_Frontend_P_3128_UnTuneQam(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3128_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3128_GetQamAsyncStatus(void *handle, NEXUS_FrontendQamStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3128_RequestQamAsyncStatus(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3128_ReadSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead);
static NEXUS_Error NEXUS_Frontend_P_3128_ReapplyTransportSettings(void *handle);
static void NEXUS_Frontend_P_3128_ResetQamStatus(void *handle);
static uint16_t NEXUS_Frontend_P_Is3128Family(const NEXUS_FrontendDevice3128OpenSettings *pSettings);
static uint16_t NEXUS_Frontend_P_Get3128Rev(const NEXUS_FrontendDevice3128OpenSettings *pSettings);
static void NEXUS_Frontend_P_UnInit3128(NEXUS_3128 *pDevice);
static void NEXUS_Frontend_P_3128_Close( NEXUS_FrontendHandle handle);
static uint16_t NEXUS_Frontend_P_get3128ChipId(const NEXUS_FrontendDevice3128OpenSettings *pSettings, unsigned familyId);
static NEXUS_Error NEXUS_Frontend_P_3128_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3128_GetQamScanStatus(void *handle, NEXUS_FrontendQamScanStatus *pScanStatus);
static void NEXUS_Frontend_P_3128_GetType(void *handle, NEXUS_FrontendType *type);
static NEXUS_Error NEXUS_Frontend_P_3128_RequestSpectrumAnalyzerData(void *handle, const NEXUS_FrontendSpectrumSettings *pSettings );
static NEXUS_Error NEXUS_Frontend_P_3128_WriteRegister(void *handle, unsigned address, uint32_t value);
static NEXUS_Error NEXUS_Frontend_P_3128_ReadRegister(void *handle, unsigned address, uint32_t *value   );
static NEXUS_Error NEXUS_Frontend_P_3128_Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings);

NEXUS_Error NEXUS_Frontend_P_3128_GetInternalGain(void *handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings);
NEXUS_Error NEXUS_Frontend_P_3128_GetExternalGain(void *handle, NEXUS_ExternalGainSettings *pSettings);
NEXUS_Error NEXUS_Frontend_P_3128_SetExternalGain(void *handle, const NEXUS_ExternalGainSettings *pSettings);
#if NEXUS_AMPLIFIER_SUPPORT
NEXUS_Error NEXUS_Frontend_P_3128_GetAmplifierStatus(void *handle, NEXUS_AmplifierStatus *pStatus);
NEXUS_Error NEXUS_Frontend_P_3128_SetAmplifierStatus(void *handle, const NEXUS_AmplifierStatus *pStatus);
#endif

#if NEXUS_FRONTEND_312x_OOB 
static void NEXUS_Frontend_P_3128_UnTuneOob(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3128_TuneOob(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_3128_GetOobStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3128_GetOobAsyncStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3128_RequestOobAsyncStatus(void *handle);
static void NEXUS_Frontend_P_3128_ResetOobStatus(void *handle);
#endif
/***************************************************************************
Summary:
    Lock callback handler for a 3128 Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_spetctrumDataReadyCallback(void *pParam)
{
    BDBG_ASSERT(NULL != pParam);
    BKNI_SetEvent((BKNI_EventHandle)pParam);
}

static void NEXUS_Frontend_P_3128_spetctrumEventCallback(void *pParam)
{

    NEXUS_Error rc = NEXUS_SUCCESS;
    BTNR_SpectrumData  spectrumData;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != pParam);
    pChannel = (NEXUS_3128Channel *)pParam;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    spectrumData.data = pDevice->spectrumDataPointer;

    rc = BTNR_GetSpectrumAnalyzerData(pDevice->tnr[pChannel->chn_num], &spectrumData);
    if (rc){rc = BERR_TRACE(rc);  goto done;}

    pDevice->spectrumDataLength += spectrumData.datalength;
    pDevice->spectrumDataPointer += spectrumData.datalength;

    if(!spectrumData.moreData) {
        if ( pDevice->spectrumDataAppCallback[pChannel->chn_num] )
        {
            pDevice->spectrumDataLength = 0;        
            NEXUS_TaskCallback_Fire(pDevice->spectrumDataAppCallback[pChannel->chn_num]);
        }
    }

done:
    return;
}


static void NEXUS_Frontend_P_3128_callback_isr(void *pParam)
{   
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;    
    BDBG_ASSERT(NULL != pParam);
    pChannel = (NEXUS_3128Channel *)pParam;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if(pDevice->acquireInProgress[pChannel->chn_num]){
        pDevice->count[pChannel->chn_num]++;
    }
    if(pDevice->count[pChannel->chn_num] == 2){
        pDevice->acquireInProgress[pChannel->chn_num] = false;
        pDevice->count[pChannel->chn_num] = 0;
    }

    if ( pDevice->lockAppCallback[pChannel->chn_num] )
    {
        NEXUS_IsrCallback_Fire_isr(pDevice->lockAppCallback[pChannel->chn_num]);
    }
}
static void NEXUS_Frontend_P_3128_updateGainCallback_isr(void *pParam)
{
    NEXUS_IsrCallbackHandle callback;
    BDBG_ASSERT(NULL != pParam);
    callback = (NEXUS_IsrCallbackHandle)pParam;

     if ( callback )
    {
        NEXUS_IsrCallback_Fire_isr(callback);
    }
}

/***************************************************************************
Summary:
    Lock callback handler for a 3128 Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_AsyncStatusCallback_isr(void *pParam)
{   
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;    
    BDBG_ASSERT(NULL != pParam);
    pChannel = (NEXUS_3128Channel *)pParam;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    
    if(!pDevice->isInternalAsyncStatusCall[pChannel->chn_num]){
        if (pDevice->asyncStatusAppCallback[pChannel->chn_num])
        {
            NEXUS_IsrCallback_Fire_isr(pDevice->asyncStatusAppCallback[pChannel->chn_num]);
        }
    }
}
/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3128 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_L1_isr(void *pParam1, int pParam2)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Handle hab;
    BDBG_ASSERT(NULL != pParam1);
    hab = (BHAB_Handle)pParam1;
    BSTD_UNUSED(pParam2);

    rc = BHAB_HandleInterrupt_isr(hab);
    if(rc){rc = BERR_TRACE(rc);}
}
/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3128 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_GpioIsrControl_isr(bool enable, void *pParam)
{
    NEXUS_GpioHandle gpioHandle;
    BDBG_ASSERT(NULL != pParam);
    gpioHandle = (NEXUS_GpioHandle)pParam;

    if(enable){
        NEXUS_Gpio_SetInterruptEnabled_isr(gpioHandle, true);
    }
    else {
        NEXUS_Gpio_SetInterruptEnabled_isr(gpioHandle, false);
    }
}

/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3128 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_IsrControl_isr(bool enable, void *pParam)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    int isrnumber;
    BDBG_ASSERT(NULL != pParam);
    isrnumber = (int)pParam;
    
    if ( enable )
    {
        rc = NEXUS_Core_EnableInterrupt_isr(isrnumber);     
        if(rc) BERR_TRACE(rc); 
    }
    else
    {
        NEXUS_Core_DisableInterrupt_isr(isrnumber);
    }
}

/***************************************************************************
Summary:
    ISR Event Handler for a 3128 device
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_IsrEvent(void *pParam)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Handle hab;
    BDBG_ASSERT(NULL != pParam);
    hab = (BHAB_Handle)pParam;

    rc = BHAB_ProcessInterruptEvent(hab);
    if(rc) BERR_TRACE(rc);
}
/***************************************************************************
Summary:
    Initialize Hab for a 3128 device
***************************************************************************/
static void NEXUS_Frontend_P_UnInit_3128_Hab(NEXUS_3128 *pDevice)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_GpioSettings gpioSettings;

    if(pDevice->isrNumber) {
        NEXUS_Core_DisconnectInterrupt(pDevice->isrNumber);
    }
    else if(pDevice->gpioInterrupt){
        NEXUS_Gpio_SetInterruptCallback_priv(pDevice->gpioInterrupt, NULL, NULL, 0);
        NEXUS_Gpio_GetSettings(pDevice->gpioInterrupt,  &gpioSettings);
        gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
        gpioSettings.interrupt.callback = NULL;
        rc = NEXUS_Gpio_SetSettings(pDevice->gpioInterrupt, &gpioSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    if(pDevice->isrEvent) pDevice->isrEvent = NULL;
    if(pDevice->isrEventCallback)NEXUS_UnregisterEvent(pDevice->isrEventCallback);
    pDevice->isrEventCallback = NULL;
    if(pDevice->hab) { 
        rc = BHAB_Close(pDevice->hab);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    pDevice->hab = NULL;
done:
    return; 
}

/***************************************************************************
Summary:
    Initialize Hab for a 3128 device
***************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_Init_3128_Hab(NEXUS_3128 *pDevice, const NEXUS_FrontendDevice3128OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Settings stHabSettings;
    BREG_I2C_Handle i2cHandle;
    
    rc = BHAB_3128_GetDefaultSettings(&stHabSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    stHabSettings.chipAddr = pSettings->i2cAddr;
    if(pSettings->isrNumber) {
        stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_3128_IsrControl_isr;
        stHabSettings.interruptEnableFuncParam = (void*)pSettings->isrNumber;
    }
    else if(pSettings->gpioInterrupt){
        stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_3128_GpioIsrControl_isr;
        stHabSettings.interruptEnableFuncParam = (void*)pSettings->gpioInterrupt;
    }

    stHabSettings.slaveChipAddr = pSettings->i2cSlaveAddr;
    stHabSettings.isMtsif = pSettings->isMtsif;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NEXUS_MODULE_SELF);
    if(i2cHandle == NULL ){rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto done;}

    rc = BHAB_Open( &pDevice->hab, (void *)i2cHandle, &stHabSettings);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    /* Success opeining Hab.  Connect Interrupt */
    if(pSettings->isrNumber) {
        rc = NEXUS_Core_ConnectInterrupt(pSettings->isrNumber, NEXUS_Frontend_P_3128_L1_isr, (void *)pDevice->hab, 0);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else if(pSettings->gpioInterrupt){
        NEXUS_Gpio_SetInterruptCallback_priv(pSettings->gpioInterrupt, NEXUS_Frontend_P_3128_L1_isr, (void *)pDevice->hab, 0);
    }

    if (pSettings->loadAP)
    {
        BDBG_WRN(("BHAB_InitAp(3128 image)"));
        rc = BHAB_InitAp(pDevice->hab, bcm3128_leap_image);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    /* Get events and register callbacks */
    rc = BHAB_GetInterruptEventHandle(pDevice->hab, &pDevice->isrEvent);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pDevice->isrEventCallback = NEXUS_RegisterEvent(pDevice->isrEvent, NEXUS_Frontend_P_3128_IsrEvent, pDevice->hab);
    if ( NULL == pDevice->isrEventCallback ){rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto done; }

    return BERR_SUCCESS;
done:
    NEXUS_Frontend_P_UnInit_3128_Hab(pDevice);
    return rc;
}

/***************************************************************************
Summary:
    Get the default settings for a BCM3128 tuner
See Also:
    NEXUS_Frontend_Open3128
 ***************************************************************************/
void NEXUS_Frontend_GetDefault3128Settings(
    NEXUS_3128Settings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->type = NEXUS_3128ChannelType_eInBand;
    pSettings->channelNumber = 0;
    pSettings->inBandOpenDrain = true;
    pSettings->loadAP = true;
    pSettings->i2cSlaveAddr = 0x60;
    pSettings->isMtsif = false;
    pSettings->outOfBand.openDrain = true;
}
/***************************************************************************
Summary:
    Initialize all ADS/OOB channels.
 ***************************************************************************/
static void NEXUS_Frontend_P_UnInit3128(NEXUS_3128 *pDevice)
{
    NEXUS_Error rc = BERR_SUCCESS;
    unsigned int i;

#if NEXUS_HAS_MXT
    if (pDevice->mxt) {
        BMXT_Close(pDevice->mxt);
    }
#endif

    for ( i = 0; i < NEXUS_MAX_3128_FRONTENDS && NULL != pDevice->tnr[i]; i++) {
        if(pDevice->tnr[i]){
            rc = BTNR_Close(pDevice->tnr[i]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            pDevice->tnr[i] = NULL;
        }
    }
    for ( i = 0; i < NEXUS_3128_MAX_DOWNSTREAM_CHANNELS && NULL != pDevice->ads_chn[i]; i++) {
        if(pDevice->ads_chn[i]){ 
            rc = BADS_CloseChannel(pDevice->ads_chn[i]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            (pDevice->ads_chn[i]) = NULL;
        }
    }
    if (pDevice->ads) {
        rc = BADS_Close(pDevice->ads);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->ads = NULL;
    }
#if NEXUS_FRONTEND_312x_OOB     
    if (pDevice->aob){
        rc = BAOB_Close(pDevice->aob);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->aob = NULL;
    }
#endif  
done:
    return;
}
/***************************************************************************
Summary:
    Initialize all ADS/OOB channels.
 ***************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_Init3128(NEXUS_3128 *pDevice, const NEXUS_FrontendDevice3128OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned int i;
    BTNR_3128Ib_Settings tnrIb3128_cfg;
    BADS_Settings ads_cfg;
    BADS_ChannelSettings chn_cfg;
#if NEXUS_FRONTEND_312x_OOB     
    BAOB_Settings aob_cfg;
    BTNR_3128Ob_Settings tnrOb3128_cfg;
#endif
#if NEXUS_HAS_MXT
    BMXT_Settings mxtSettings;
#endif

    rc = BADS_3128_GetDefaultSettings( &ads_cfg, NULL);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    ads_cfg.transportConfig = BADS_TransportData_eSerial;
    ads_cfg.hGeneric = pDevice->hab;
    ads_cfg.isOpenDrain = pSettings->inBandOpenDrain;
    rc = BADS_Open(&pDevice->ads, NULL, NULL, NULL, &ads_cfg);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BADS_Init(pDevice->ads);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    /* get total ADS channel number*/
    rc = BADS_GetTotalChannels(pDevice->ads, &pDevice->numChannels);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    for (i=0;i<pDevice->numChannels;i++) {
        rc = BTNR_3128Ib_GetDefaultSettings(&tnrIb3128_cfg);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        tnrIb3128_cfg.channelNo = i;
        rc =  BTNR_3128Ib_Open(&pDevice->tnr[i],&tnrIb3128_cfg, pDevice->hab);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    /* Configure ADS channels */
    for (i=0;i<pDevice->numChannels;i++) {
        rc = BADS_GetChannelDefaultSettings( pDevice->ads, i, &chn_cfg);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        rc = BADS_OpenChannel( pDevice->ads, &pDevice->ads_chn[i], i, &chn_cfg);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

#if NEXUS_FRONTEND_312x_OOB
    if((pDevice->chipId == 0x3128) || (pDevice->chipId == 0x3127)){
        rc = BTNR_3128Ob_GetDefaultSettings(&tnrOb3128_cfg);
        tnrOb3128_cfg.ifFreq = pSettings->outOfBand.ifFrequency;
        if(pSettings->outOfBand.useWidebandAtoD)
            tnrOb3128_cfg.inputMode = BTNR_3128Ob_OutOfBandInputMode_eWideBandAdc;
        else
            tnrOb3128_cfg.inputMode = BTNR_3128Ob_OutOfBandInputMode_eOutOfBandAdc;
        rc = BTNR_3128Ob_Open(&pDevice->tnr[NEXUS_3128_OOB_CHN], &tnrOb3128_cfg, pDevice->hab);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    
        rc = BAOB_GetDefaultSettings( &aob_cfg, NULL);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        /* There is only one AOB device in 3128/3114. so the device id is left to default settings. */
        aob_cfg.hGeneric = pDevice->hab;
        aob_cfg.serialData = true;
        aob_cfg.isOpenDrain = pSettings->outOfBand.openDrain;
        aob_cfg.nyquist = pSettings->outOfBand.nyquist;
        rc = BAOB_Open(&pDevice->aob, NULL, NULL, NULL, &aob_cfg);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
#endif
#if NEXUS_HAS_MXT
    /* open MXT */
    BMXT_3128_GetDefaultSettings(&mxtSettings);
    for (i=0; i<BMXT_NUM_MTSIF; i++) {
        mxtSettings.MtsifTxCfg[i].Enable = pDevice->isMtsif; /* note, .isMtsif passed to BHAB_Open is independent of this */
    }
    mxtSettings.hHab = pDevice->hab;
    /* TODO: set mxtSettings.chipRev correctly. 3128 HW is the same for A0/B0/C0 though, so we're not missing out */
    rc = BMXT_Open(&pDevice->mxt, NULL, NULL, &mxtSettings);
    if (rc!=BERR_SUCCESS) goto done;

    {
        BMXT_ParserConfig parserConfig;
        BKNI_Memset(&pDevice->mtsifConfig, 0, sizeof(NEXUS_FrontendDeviceMtsifConfig));
        pDevice->mtsifConfig.pMxtHandle = pDevice->mxt;
        pDevice->mtsifConfig.numDemodIb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eInputBand);
        pDevice->mtsifConfig.numDemodPb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eParser);

        for (i=0; i<pDevice->mtsifConfig.numDemodPb; i++) {
            rc = BMXT_GetParserConfig(pDevice->mxt, i, &parserConfig);
            if (rc) { rc = BERR_TRACE(rc); goto done; }

            /* turn off demod parsers */
            parserConfig.Enable = false;

            /* cache other settings */
            pDevice->mtsifConfig.demodPbSettings[i].errorInputIgnore = parserConfig.ErrorInputIgnore;
            pDevice->mtsifConfig.demodPbSettings[i].timestampMode = (unsigned)parserConfig.TsMode;
            pDevice->mtsifConfig.demodPbSettings[i].dssMode = parserConfig.DssMode;

            rc = BMXT_SetParserConfig(pDevice->mxt, i, &parserConfig);
            if (rc) { rc = BERR_TRACE(rc); goto done; }
        }
    }    
#endif    
    return BERR_SUCCESS;
done:
    NEXUS_Frontend_P_UnInit3128(pDevice);
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_WriteRegister(void *handle, unsigned address, uint32_t value)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    rc = BHAB_WriteRegister(pDevice->hab, address, &value);
    if(rc){rc = BERR_TRACE(rc);}

    return rc;
}
static NEXUS_Error NEXUS_Frontend_P_3128_ReadRegister(void *handle, unsigned address, uint32_t *value   )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    rc = BHAB_ReadRegister(pDevice->hab, address, value);
    if(rc){rc = BERR_TRACE(rc);}

    return rc;
}

static uint16_t NEXUS_Frontend_P_Get3128Rev(const NEXUS_FrontendDevice3128OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[2];
    uint16_t revId=0xef;
    uint8_t subAddr;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);

    buf[0]= 0x0;
    subAddr = 0x3;
    rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    revId = buf[0];

    subAddr = 0x4;
    rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    revId = (revId <<8) | buf[0];

    BDBG_MSG(("revId = 0x%x", revId));
    return revId;
done:   
    return 0;   
}

static uint16_t NEXUS_Frontend_P_Is3128Family(const NEXUS_FrontendDevice3128OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[4];
    uint16_t chipFamilyId=0;
    uint8_t subAddr;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);
    buf[0]= 0x0;
    subAddr = 0x1;
    rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    chipFamilyId = buf[0];
    
    subAddr = 0x2;
    rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    chipFamilyId = (chipFamilyId <<8) | buf[0];

    BDBG_MSG(("ChipFamilyId = 0x%x", chipFamilyId));
    return chipFamilyId;
done:   
    return 0;
}

static uint16_t NEXUS_Frontend_P_get3128ChipId(const NEXUS_FrontendDevice3128OpenSettings *pSettings, unsigned familyId)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint32_t sb1, addr, subAddr;
    uint16_t chipId=0;
    uint8_t data=0, i=0,  buf[4];
    BREG_I2C_Handle i2cHandle;
    
    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);

    addr = BCHP_TM_CHIP_ID;   
    sb1 = ((addr & 0x000000FF) << 24 |  
           (addr & 0x0000FF00) << 8 |   
           (addr & 0x00FF0000) >> 8 |   
           (addr & 0xFF000000) >> 24 );
   
   /* set READ_RBUS for read mode */
   data = 0x1;   
   rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, CSR_CONFIG, &data, 1);
   if(rc){rc = BERR_TRACE(rc); goto done;}
   rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, CSR_RBUS_ADDR0, (uint8_t *)&sb1, 4);
   if(rc){rc = BERR_TRACE(rc); goto done;}
   
    /* poll the busy bit to make sure the transaction is completed */   
   for(i=0; i < 5; i++){   
        subAddr = CSR_STATUS;
        rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, &data, 1);      
        if(rc){rc = BERR_TRACE(rc); goto done;}
        if ((data & (1 << CSR_STATUS_BUSY_SHIFT)) == 0)   
            break;   
   }
   
   if(i==5)   
       BDBG_WRN(("Read transaction not finished\n"));
   
    /* read the data */   
    subAddr = CSR_RBUS_DATA0;
    rc = BREG_I2C_WriteNoAddr(i2cHandle, pSettings->i2cAddr, (uint8_t *)&subAddr, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    BKNI_Memset(buf, 0, 4);
    rc = BREG_I2C_ReadNoAddr(i2cHandle, pSettings->i2cAddr, buf, 4);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    chipId = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
   
   /* set READ_RBUS to the reset value for write mode */   
   data = CSR_CONFIG_READ_RBUS_WRITE;   
   rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, CSR_CONFIG, &data, 1);
   if(rc){rc = BERR_TRACE(rc); goto done;}
   
   BDBG_MSG(("chipId = 0x%x", chipId));

    if((chipId == 0x00) && (familyId == 0x3128))
        chipId = 0x3128;
    return chipId;
done:   
    return 0;
}

/***************************************************************************
Summary:
    Get the default settings for a BCM3128 device.
See Also:
    NEXUS_FrontendDevice_Open3128
 ***************************************************************************/
void NEXUS_FrontendDevice_GetDefault3128OpenSettings(NEXUS_FrontendDevice3128OpenSettings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->inBandOpenDrain = true;
    pSettings->loadAP = true;
    pSettings->i2cSlaveAddr = 0x60;
    pSettings->isMtsif = false;
    pSettings->outOfBand.openDrain = true;

}

NEXUS_FrontendDeviceHandle NEXUS_FrontendDevice_Open3128(unsigned index, const NEXUS_FrontendDevice3128OpenSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice = NULL;
    NEXUS_FrontendDevice *pFrontendDevice = NULL;
    NEXUS_3128ProbeResults results;
    unsigned chipFamilyId=0, chipId = 0, revId=0;
    BSTD_UNUSED(index);

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->i2cDevice);

    rc = NEXUS_Frontend_Probe3128(pSettings, &results);
    if(rc){rc = BERR_TRACE(rc); goto err_create;} 

    for ( pDevice = BLST_S_FIRST(&g_3128DevList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
    {
       BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
       if ((pSettings->i2cAddr == pDevice->i2cAddr) && (pSettings->i2cDevice== pDevice->i2cHandle))
       {
           break;
       }
    }

    if ( NULL == pDevice)
    {
        chipFamilyId = NEXUS_Frontend_P_Is3128Family(pSettings);
        chipId = NEXUS_Frontend_P_get3128ChipId(pSettings, chipFamilyId);
        revId = NEXUS_Frontend_P_Get3128Rev(pSettings);

        pFrontendDevice = BKNI_Malloc(sizeof(*pFrontendDevice));        
        if (NULL == pFrontendDevice) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create;}

        /* Memsetting the whole structure should cover initializing the child list. */
        BKNI_Memset(pFrontendDevice, 0, sizeof(*pFrontendDevice));

        pDevice = BKNI_Malloc(sizeof(NEXUS_3128));     
        if (NULL == pDevice) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create;}
        
        BKNI_Memset(pDevice, 0, sizeof(NEXUS_3128));
        BDBG_OBJECT_SET(pDevice, NEXUS_3128);

        pDevice->chipFamilyId = chipFamilyId;
        pDevice->chipId = chipId;
        pDevice->revId = revId;
        pDevice->i2cAddr = pSettings->i2cAddr;
        pDevice->isrNumber = pSettings->isrNumber;
        pDevice->gpioInterrupt = pSettings->gpioInterrupt;
        pDevice->i2cHandle = pSettings->i2cDevice;
        pDevice->isMtsif = pSettings->isMtsif;
        pDevice->pGenericDeviceHandle = pFrontendDevice;
#if NEXUS_FRONTEND_312x_OOB
        pDevice->deviceSettings.outOfBand.outputMode = NEXUS_FrontendOutOfBandOutputMode_eMax; /* Setting default. */
#endif
        rc = NEXUS_Frontend_P_Init_3128_Hab(pDevice, pSettings);
        if(rc){rc = BERR_TRACE(rc); goto err_init_hab;}

        /* Initialize ADS channels and OOB */
        rc = NEXUS_Frontend_P_Init3128(pDevice, pSettings);
        if(rc){rc = BERR_TRACE(rc); goto err_init;} 

        pDevice->updateGainAppCallback = NEXUS_IsrCallback_Create(NULL, NULL);
        if ( NULL == pDevice->updateGainAppCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_ch_alloc;}

        rc = BKNI_CreateEvent(&pDevice->spectrumEvent);
        if(rc){rc = BERR_TRACE(rc); goto err_callback_destroy;} 

        BLST_S_INSERT_HEAD(&g_3128DevList, pDevice, node);
    }
    else
    {
        BDBG_MSG(("Found 312x device"));
        return pDevice->pGenericDeviceHandle;
    }

    pFrontendDevice->pDevice = pDevice;
    pFrontendDevice->familyId = results.chip.familyId;
    pFrontendDevice->bypassableFixedGain = pSettings->externalFixedGain.bypassable * 100;
    pFrontendDevice->totalFixedBoardGain = pSettings->externalFixedGain.total * 100;
    pFrontendDevice->application = NEXUS_FrontendDeviceApplication_eCable;
    pFrontendDevice->getInternalGain = NEXUS_Frontend_P_3128_GetInternalGain;
    pFrontendDevice->getExternalGain = NEXUS_Frontend_P_3128_GetExternalGain;
    pFrontendDevice->setExternalGain = NEXUS_Frontend_P_3128_SetExternalGain;
#if NEXUS_AMPLIFIER_SUPPORT 
    pFrontendDevice->getAmplifierStatus = NEXUS_Frontend_P_3128_GetAmplifierStatus;
    pFrontendDevice->setAmplifierStatus = NEXUS_Frontend_P_3128_SetAmplifierStatus;
#endif
    return pFrontendDevice;

err_callback_destroy:
    if ( pDevice->updateGainAppCallback ) NEXUS_IsrCallback_Destroy(pDevice->updateGainAppCallback);
err_ch_alloc:
    NEXUS_Frontend_P_UnInit3128(pDevice);
err_init:   
    NEXUS_Frontend_P_UnInit_3128_Hab(pDevice);
err_init_hab:
    if ( pDevice ) BKNI_Free(pDevice);
    pDevice = NULL;
err_create: 
    if(pFrontendDevice) BKNI_Free(pFrontendDevice);
    return NULL;

}


/***************************************************************************
Summary:
    Open a handle to a BCM3128 device.
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open3128(const NEXUS_3128Settings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_FrontendHandle frontendHandle = NULL;
    NEXUS_3128 *pDevice = NULL;
    unsigned int chn_num=0;
    NEXUS_IsrCallbackHandle callback;
    NEXUS_IsrCallbackHandle qamAsyncStatusReadyCallback = NULL;
    NEXUS_TaskCallbackHandle spectrumDataCallback  = NULL;
    NEXUS_3128Channel *pChannel;
    NEXUS_FrontendDevice3128OpenSettings openSettings;
    NEXUS_FrontendDevice *pFrontendDevice = NULL;
#if NEXUS_FRONTEND_312x_OOB
    NEXUS_IsrCallbackHandle oobAsyncStatusReadyCallback = NULL;
#endif

    BDBG_ASSERT(NULL != pSettings);

    if ( ((pSettings->channelNumber >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)&& pSettings->type == NEXUS_3128ChannelType_eInBand)
        || (pSettings->channelNumber >= NEXUS_MAX_3128_FRONTENDS ))
    {
        BDBG_ERR((" channel number exceeds available one"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_create;
    }

    if(pSettings->device == NULL) {     
        BDBG_ASSERT(NULL != pSettings->i2cDevice);
        NEXUS_FrontendDevice_GetDefault3128OpenSettings(&openSettings);
        openSettings.configureWatchdog = pSettings->configureWatchdog;
        openSettings.gpioInterrupt = pSettings->gpioInterrupt;
        openSettings.i2cAddr = pSettings->i2cAddr;
        openSettings.i2cDevice  = pSettings->i2cDevice;
        openSettings.i2cSlaveAddr  = pSettings->i2cSlaveAddr;
        openSettings.inBandOpenDrain  = pSettings->inBandOpenDrain;
        openSettings.isMtsif  = pSettings->isMtsif;
        openSettings.isrNumber  = pSettings->isrNumber;
        openSettings.loadAP  = pSettings->loadAP;
        openSettings.spiAddr  = pSettings->spiAddr;
        openSettings.spiDevice  = pSettings->spiDevice;
#if NEXUS_FRONTEND_312x_OOB
        openSettings.outOfBand.nyquist  = pSettings->outOfBand.nyquist;
        openSettings.outOfBand.ifFrequency  = pSettings->ifFrequency;
        openSettings.outOfBand.useWidebandAtoD  = pSettings->outOfBand.useWidebandAtoD;
        openSettings.outOfBand.openDrain  = pSettings->outOfBand.openDrain;
#endif      
        pFrontendDevice = NEXUS_FrontendDevice_Open3128(0, &openSettings);
        pDevice = (NEXUS_3128 *)pFrontendDevice->pDevice;
    }
    else {
        pDevice = (NEXUS_3128 *)pSettings->device->pDevice;
        pFrontendDevice =  pSettings->device;
    }


    switch(pSettings->type)
    {
    case NEXUS_3128ChannelType_eInBand:
        chn_num = pSettings->channelNumber;
        break;
        case NEXUS_3128ChannelType_eOutOfBand:
        chn_num = NEXUS_3128_OOB_CHN;
        break;
    default:
        BDBG_ERR((" channel type not supported"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_create;
    }

    /* chekc if fronthandle is already opened*/
    if ( pDevice->frontendHandle[chn_num] != NULL )
    {
        return pDevice->frontendHandle[chn_num];
    }

    pChannel = (NEXUS_3128Channel*)BKNI_Malloc(sizeof(NEXUS_3128Channel));
    if ( NULL == pChannel ) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create;}

    /* Create a Nexus frontend handle */
    frontendHandle = NEXUS_Frontend_P_Create(pChannel);
    if ( NULL == frontendHandle ) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}

    /* Establish device capabilities */
    if ( pSettings->type == NEXUS_3128ChannelType_eInBand) {
        frontendHandle->capabilities.qam = true;
        frontendHandle->capabilities.outOfBand = false;
        frontendHandle->capabilities.upstream = false;
        BKNI_Memset(frontendHandle->capabilities.qamModes, true, sizeof(frontendHandle->capabilities.qamModes));
        /* bind functions*/
        frontendHandle->tuneQam = NEXUS_Frontend_P_3128_TuneQam;
        frontendHandle->getQamStatus = NEXUS_Frontend_P_3128_GetQamStatus;
        frontendHandle->readSoftDecisions = NEXUS_Frontend_P_3128_ReadSoftDecisions;
        frontendHandle->resetStatus = NEXUS_Frontend_P_3128_ResetQamStatus;
        frontendHandle->untune = NEXUS_Frontend_P_3128_UnTuneQam;
        frontendHandle->requestQamAsyncStatus = NEXUS_Frontend_P_3128_RequestQamAsyncStatus;
        frontendHandle->getQamAsyncStatus = NEXUS_Frontend_P_3128_GetQamAsyncStatus;
        frontendHandle->getQamScanStatus = NEXUS_Frontend_P_3128_GetQamScanStatus;
    }
#if NEXUS_FRONTEND_312x_OOB     
    else if ( pSettings->type == NEXUS_3128ChannelType_eOutOfBand) {
        frontendHandle->capabilities.qam = false;
        frontendHandle->capabilities.outOfBand = true;
        frontendHandle->capabilities.upstream = false;
        BKNI_Memset(frontendHandle->capabilities.outOfBandModes, true, sizeof(frontendHandle->capabilities.outOfBandModes));
        /* bind functions*/
        frontendHandle->tuneOutOfBand = NEXUS_Frontend_P_3128_TuneOob;
        frontendHandle->getOutOfBandStatus = NEXUS_Frontend_P_3128_GetOobStatus;
        frontendHandle->readSoftDecisions = NEXUS_Frontend_P_3128_ReadSoftDecisions;
        frontendHandle->resetStatus = NEXUS_Frontend_P_3128_ResetOobStatus;
        frontendHandle->untune = NEXUS_Frontend_P_3128_UnTuneOob;
        frontendHandle->requestOutOfBandAsyncStatus = NEXUS_Frontend_P_3128_RequestOobAsyncStatus;
        frontendHandle->getOutOfBandAsyncStatus = NEXUS_Frontend_P_3128_GetOobAsyncStatus;
    }
#endif

    frontendHandle->reapplyTransportSettings = NEXUS_Frontend_P_3128_ReapplyTransportSettings;
    frontendHandle->close = NEXUS_Frontend_P_3128_Close;
    frontendHandle->getFastStatus = NEXUS_Frontend_P_3128_GetFastStatus;
    frontendHandle->getType = NEXUS_Frontend_P_3128_GetType;
    frontendHandle->requestSpectrumData = NEXUS_Frontend_P_3128_RequestSpectrumAnalyzerData;
    frontendHandle->writeRegister = NEXUS_Frontend_P_3128_WriteRegister;
    frontendHandle->readRegister = NEXUS_Frontend_P_3128_ReadRegister;
    frontendHandle->standby = NEXUS_Frontend_P_3128_Standby;

    frontendHandle->pGenericDeviceHandle = pFrontendDevice;

    /* Create app callback */
    callback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
    if ( NULL == callback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_cbk_create;}
    /* install callback to  notify of lock/unlock change */
    if ( pSettings->type == NEXUS_3128ChannelType_eInBand)
    {
        qamAsyncStatusReadyCallback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
        if ( NULL == qamAsyncStatusReadyCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_callback_destroy;}

        spectrumDataCallback = NEXUS_TaskCallback_Create(frontendHandle, NULL);
        if ( NULL == spectrumDataCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_callback_destroy;}

        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eLockChange, (BADS_CallbackFunc)NEXUS_Frontend_P_3128_callback_isr, (void*)pChannel);
        if(rc){rc = BERR_TRACE(rc); goto err_callback_destroy;} 

        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eUpdateGain, (BADS_CallbackFunc)NEXUS_Frontend_P_3128_updateGainCallback_isr, (void*)pDevice->updateGainAppCallback);
        if(rc){rc = BERR_TRACE(rc); goto err_callback_destroy;} 

        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eAsyncStatusReady, (BADS_CallbackFunc)NEXUS_Frontend_P_3128_AsyncStatusCallback_isr, (void*)pChannel);
        if(rc){rc = BERR_TRACE(rc); goto err_callback_destroy;} 

        /* For now this is hooked up to the lock/unlock callback. */
        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eNoSignal, (BADS_CallbackFunc)NEXUS_Frontend_P_3128_callback_isr, (void*)pChannel);
        if(rc){rc = BERR_TRACE(rc); goto err_callback_destroy;}
        
        rc = BTNR_InstallCallback(pDevice->tnr[chn_num], BTNR_Callback_eSpectrumDataReady, (BTNR_CallbackFunc)NEXUS_Frontend_P_3128_spetctrumDataReadyCallback, (void*)pDevice->spectrumEvent);
        if(rc){rc = BERR_TRACE(rc); goto err_callback_destroy;}
        
        pDevice->spectrumDataAppCallback[chn_num] = spectrumDataCallback;
        pDevice->asyncStatusAppCallback[chn_num] = qamAsyncStatusReadyCallback;
    }
    else if(pSettings->type == NEXUS_3128ChannelType_eOutOfBand)
    {
#if NEXUS_FRONTEND_312x_OOB
        if((pDevice->chipId == 0x3128) || (pDevice->chipId == 0x3127)){
            oobAsyncStatusReadyCallback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
            if ( NULL == oobAsyncStatusReadyCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_oob_async_create;}

            rc = BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eLockChange, (BAOB_CallbackFunc)NEXUS_Frontend_P_3128_callback_isr, (void*)pChannel);
            if(rc){rc = BERR_TRACE(rc); goto err_oob_async_create;}

            rc = BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eAsyncStatusReady, (BAOB_CallbackFunc)NEXUS_Frontend_P_3128_AsyncStatusCallback_isr, (void*)pChannel);
            if(rc){rc = BERR_TRACE(rc); goto err_oob_async_create;}

            pDevice->asyncStatusAppCallback[chn_num] = oobAsyncStatusReadyCallback ;
        }
        else{
            BDBG_ERR(("Out of band is not supported on the 0x%x chip", pDevice->chipId));
        }
#endif      
    }
    
    /* See if upstream needs a callback. */
    pDevice->lockAppCallback[chn_num] = callback;
    pDevice->frontendHandle[chn_num] = frontendHandle;
    /* save channel number in pChannel*/
    pChannel->chn_num = chn_num;
    pChannel->pDevice = pDevice;
    frontendHandle->chip.familyId = pDevice->chipFamilyId;
    frontendHandle->chip.id = pDevice->chipId;
    pDevice->numfrontends++;
    return frontendHandle;

#if NEXUS_FRONTEND_312x_OOB
err_oob_async_create:
    if ( oobAsyncStatusReadyCallback ) NEXUS_IsrCallback_Destroy(oobAsyncStatusReadyCallback);
#endif
err_callback_destroy:
    if ( spectrumDataCallback ) NEXUS_TaskCallback_Destroy(spectrumDataCallback);
    if ( qamAsyncStatusReadyCallback ) NEXUS_IsrCallback_Destroy(qamAsyncStatusReadyCallback);
    if ( callback ) NEXUS_IsrCallback_Destroy(callback);
err_cbk_create:
    if ( frontendHandle ) BKNI_Free(frontendHandle);
err_alloc:
    if ( pChannel ) BKNI_Free(pChannel);
    if(pDevice->numfrontends != 0)
        return NULL;
err_create:
    return NULL;
}

/***************************************************************************
Summary:
    Close a handle to a BCM3128 device.
See Also:
    NEXUS_Frontend_Open3128
 ***************************************************************************/
static void NEXUS_Frontend_P_3128_Close(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pChannel =(NEXUS_3128Channel*) handle->pDeviceHandle;
    BDBG_ASSERT(NULL != pChannel);
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if (pChannel->chn_num >= NEXUS_MAX_3128_FRONTENDS)
    {
        BDBG_ERR((" Unsupported Frontend Handle"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if ( NULL != pDevice->lockAppCallback[pChannel->chn_num])
        NEXUS_IsrCallback_Destroy(pDevice->lockAppCallback[pChannel->chn_num]);
    if ( NULL != pDevice->asyncStatusAppCallback[pChannel->chn_num])
        NEXUS_IsrCallback_Destroy(pDevice->asyncStatusAppCallback[pChannel->chn_num]);

    if (pChannel->chn_num < NEXUS_3128_MAX_DOWNSTREAM_CHANNELS) {
        if(pDevice->spectrumDataAppCallback[pChannel->chn_num]) {
            NEXUS_TaskCallback_Destroy(pDevice->spectrumDataAppCallback[pChannel->chn_num]);
        }
    }
    NEXUS_Frontend_P_Destroy(handle);

    pDevice->frontendHandle[pChannel->chn_num] = NULL;
    BKNI_Free(pChannel);
    pDevice->numfrontends--;
    
    /* see if there are still open channels*/
    if (pDevice->numfrontends) return;

    if(pDevice->updateGainAppCallback) {
        NEXUS_IsrCallback_Destroy(pDevice->updateGainAppCallback);
    }

    if(pDevice->spectrumEventCallback)NEXUS_UnregisterEvent(pDevice->spectrumEventCallback);
    pDevice->spectrumEventCallback = NULL;

    if (pDevice->spectrumEvent) {
        BKNI_DestroyEvent(pDevice->spectrumEvent);
        pDevice->spectrumEvent = NULL;
    }

    NEXUS_Frontend_P_UnInit3128(pDevice);
    NEXUS_Frontend_P_UnInit_3128_Hab(pDevice);
    BLST_S_REMOVE(&g_3128DevList, pDevice, NEXUS_3128, node);
    BDBG_OBJECT_DESTROY(pDevice, NEXUS_3128);
    BKNI_Free(pDevice);
done:
    return;
}

#if NEXUS_AMPLIFIER_SUPPORT
NEXUS_Error NEXUS_Frontend_P_3128_GetAmplifierStatus(void *handle, NEXUS_AmplifierStatus *pStatus)
{     
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3128 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));


    if(pDevice->pGenericDeviceHandle->parent != NULL){
        rc = NEXUS_Frontend_P_GetAmplifierStatus(pDevice->pGenericDeviceHandle->parent, pStatus);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else if(pDevice->pGenericDeviceHandle->amplifier != NULL){
        rc = NEXUS_Amplifier_GetStatus(pDevice->pGenericDeviceHandle->amplifier, pStatus);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    else {
        BDBG_ERR(("Amplifier not linked to the parent device."));
    }
    
done:
    return rc;
}

NEXUS_Error NEXUS_Frontend_P_3128_SetAmplifierStatus(void *handle, const NEXUS_AmplifierStatus *pStatus)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3128 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    BSTD_UNUSED(pStatus);
    /* Set LNA/Amplifier parameters. */

    return rc;

}
#endif

NEXUS_Error NEXUS_Frontend_P_3128_GetInternalGain(void *handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings)
{   
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    BHAB_InternalGainInputParams inputParams;
    BHAB_InternalGainSettings internalGain;
    NEXUS_GainParameters gainParams;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3128 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);


    pSettings->frequency = params->frequency;
    inputParams.frequency = params->frequency;

    rc = BHAB_GetInternalGain(pDevice->hab, &inputParams, &internalGain);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    if(params->accumulateTillRootDevice){
        if(pDevice->pGenericDeviceHandle->parent){            
            gainParams.rfInput = pDevice->pGenericDeviceHandle->linkSettings.rfInput;
            gainParams.accumulateTillRootDevice = params->accumulateTillRootDevice;
            gainParams.frequency = params->frequency;
            rc = NEXUS_Frontend_P_GetInternalGain(pDevice->pGenericDeviceHandle->parent, &gainParams, pSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
    }

    if(params->rfInput == NEXUS_FrontendDeviceRfInput_eDaisy){
        pSettings->totalVariableGain += internalGain.internalGainDaisy;
    }
    else if(params->rfInput == NEXUS_FrontendDeviceRfInput_eLoopThrough){
        pSettings->totalVariableGain += internalGain.internalGainLoopThrough;
    }

    pSettings->daisyGain += internalGain.internalGainDaisy;
    pSettings->loopThroughGain += internalGain.internalGainLoopThrough;
done:
    return rc;
}
NEXUS_Error NEXUS_Frontend_P_3128_GetExternalGain(void *handle, NEXUS_ExternalGainSettings *pSettings)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    BHAB_ExternalGainSettings gain;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3128 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BSTD_UNUSED(pSettings);


    rc = BHAB_GetExternalGain(pDevice->hab, &gain);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pSettings->bypassableGain = gain.externalGainBypassable;
    pSettings->totalGain = gain.externalGainTotal;

done:
    return rc;

}
NEXUS_Error NEXUS_Frontend_P_3128_SetExternalGain(void *handle, const NEXUS_ExternalGainSettings *pSettings)
{   
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    BHAB_ExternalGainSettings externalGain;
    BDBG_ASSERT(NULL != handle);
    pDevice = (NEXUS_3128 *)handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);


    externalGain.externalGainTotal = pSettings->totalGain;
    externalGain.externalGainBypassable = pSettings->bypassableGain;

    rc = BHAB_SetExternalGain(pDevice->hab, &externalGain);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
done:   
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BADS_InbandParam ibParam;
    unsigned chn_num;
    BTNR_PowerSaverSettings pwrSettings;    
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BADS_ChannelScanSettings scanParam;
    unsigned temp_frequency;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = pChannel->pDevice;
    chn_num = pChannel->chn_num;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if (chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    rc = NEXUS_Frontend_P_SetMtsifConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
    rc = NEXUS_Frontend_P_SetTransportConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
    rc = NEXUS_Frontend_P_SetTsmfConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
#endif
#endif

    temp_frequency = pDevice->last_ads[chn_num].frequency;
    pDevice->last_ads[chn_num].frequency = pSettings->frequency;
    
    if((!BKNI_Memcmp(pSettings, &pDevice->last_ads[chn_num], sizeof(NEXUS_FrontendQamSettings))) && (pSettings->acquisitionMode == NEXUS_FrontendQamAcquisitionMode_eScan)){
        if (pDevice->tnr[chn_num])
        {
            if(!pDevice->isPoweredOn[chn_num]){
                pwrSettings.enable = false;
                rc = BTNR_SetPowerSaver(pDevice->tnr[chn_num], &pwrSettings);
                if(rc){rc = BERR_TRACE(rc); goto retrack;} 

                rc = BADS_DisablePowerSaver(pDevice->ads_chn[chn_num]);
                if(rc){rc = BERR_TRACE(rc); goto retrack;}      

                pDevice->isPoweredOn[chn_num] = true;
            }
            
            pDevice->count[chn_num] = 0;
            pDevice->acquireInProgress[chn_num] = true;
            rc = BTNR_SetTunerRfFreq(pDevice->tnr[chn_num], pSettings->frequency, BTNR_TunerMode_eDigital);
            if(rc){rc = BERR_TRACE(rc); goto retrack;} 

            pDevice->last_ads[chn_num] = *pSettings;

            return rc;
        }
    }

    rc = BADS_GetDefaultAcquireParams(pDevice->ads_chn[chn_num], &ibParam);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
    if ( pSettings->annex == NEXUS_FrontendQamAnnex_eA )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e16:
            ibParam.modType = BADS_ModulationType_eAnnexAQam16;
            break;
        case NEXUS_FrontendQamMode_e32:
            ibParam.modType = BADS_ModulationType_eAnnexAQam32;
            break;
        case NEXUS_FrontendQamMode_e64:
            ibParam.modType = BADS_ModulationType_eAnnexAQam64;
            break;
        case NEXUS_FrontendQamMode_e128:
            ibParam.modType = BADS_ModulationType_eAnnexAQam128;
            break;
        case NEXUS_FrontendQamMode_e256:
            ibParam.modType = BADS_ModulationType_eAnnexAQam256;
            break;
        default:
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        }     
       
        ibParam.enableNullPackets = pSettings->enableNullPackets;
    }
    else if ( pSettings->annex == NEXUS_FrontendQamAnnex_eB )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e64:
            ibParam.modType = BADS_ModulationType_eAnnexBQam64;
            break;
        case NEXUS_FrontendQamMode_e256:
            ibParam.modType = BADS_ModulationType_eAnnexBQam256;
            break;
        default:
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        }
        
        ibParam.enableNullPackets = false;
    }
    else
    {   
        rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
    }

    NEXUS_IsrCallback_Set(pDevice->lockAppCallback[chn_num], &(pSettings->lockCallback));
    NEXUS_IsrCallback_Set(pDevice->asyncStatusAppCallback[chn_num], &(pSettings->asyncStatusReadyCallback));
    
    if (pDevice->tnr[chn_num])
    {
        if(!pDevice->isPoweredOn[chn_num]){
            pwrSettings.enable = false;
            rc = BTNR_SetPowerSaver(pDevice->tnr[chn_num], &pwrSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;} 

            rc = BADS_DisablePowerSaver(pDevice->ads_chn[chn_num]);
            if(rc){rc = BERR_TRACE(rc); goto done;}      

            pDevice->isPoweredOn[chn_num] = true;
        }
        pDevice->count[chn_num] = 0;
        pDevice->acquireInProgress[chn_num] = true;
        rc = BTNR_SetTunerRfFreq(pDevice->tnr[chn_num], pSettings->frequency, BTNR_TunerMode_eDigital);
        if(rc){rc = BERR_TRACE(rc); goto done;} 
    }

    /* Scan Parameters */ 
    if((pSettings->acquisitionMode == NEXUS_FrontendQamAcquisitionMode_eScan) || (pSettings->acquisitionMode == NEXUS_FrontendQamAcquisitionMode_eAuto)){
        BKNI_Memset(&scanParam, 0, sizeof(scanParam));
        scanParam.QM = true;
        scanParam.TO = true; 
        if( pSettings->spectrumMode == NEXUS_FrontendQamSpectrumMode_eAuto) scanParam.AI = true;        
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e16]) scanParam.A16 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e32]) scanParam.A32= true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e64]) scanParam.A64 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e128]) scanParam.A128 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e256]) scanParam.A256 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e512]) scanParam.A512 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eA][NEXUS_FrontendQamMode_e1024]) scanParam.A1024 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eB][NEXUS_FrontendQamMode_e64]) scanParam.B64 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eB][NEXUS_FrontendQamMode_e256]) scanParam.B256 = true;
        if(pSettings->scan.mode[NEXUS_FrontendQamAnnex_eB][NEXUS_FrontendQamMode_e1024]) scanParam.B1024 = true;
        if(pSettings->scan.frequencyOffset){
            scanParam.CO = true; 
            scanParam.carrierSearch = pSettings->scan.frequencyOffset/256; 
        }        
        scanParam.upperBaudSearch = pSettings->scan.upperBaudSearch;
        scanParam.lowerBaudSearch = pSettings->scan.lowerBaudSearch;
        
        rc = BADS_SetScanParam(pDevice->ads_chn[chn_num], &scanParam );
        if(rc){rc = BERR_TRACE(rc); goto done;}
    
        ibParam.tuneAcquire = true;
    }
    else {
        ibParam.tuneAcquire = false;
    }

    ibParam.symbolRate = pSettings->symbolRate;
    ibParam.autoAcquire = pSettings->autoAcquire;
    ibParam.enableDpm = pSettings->enablePowerMeasurement;
    ibParam.spectrum = pSettings->spectrumMode;
    ibParam.invertSpectrum = pSettings->spectralInversion;
    ibParam.frequencyOffset =  pSettings->frequencyOffset;
    ibParam.acquisitionType = pSettings->acquisitionMode;

    rc = BADS_SetAcquireParams(pDevice->ads_chn[chn_num], &ibParam );
    if(rc){rc = BERR_TRACE(rc); goto done;}  

    rc = BADS_Acquire(pDevice->ads_chn[chn_num], &ibParam );
    if(rc){rc = BERR_TRACE(rc); goto done;} 

    pDevice->last_ads[chn_num] = *pSettings;

    return BERR_SUCCESS;
    
retrack:
    pDevice->last_ads[chn_num].frequency = temp_frequency;
done:   
    NEXUS_Frontend_P_3128_UnTuneQam(handle);
    return rc;
}

static void NEXUS_Frontend_P_3128_UnTuneQam(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if (pChannel->chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    NEXUS_Frontend_P_UnsetMtsifConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    NEXUS_Frontend_P_UnsetTransportConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
#endif
#endif

    /* This is currently done as a hack to avoid the demod trying to acquire continuously in autoacquire mode.
            This hack will be taken out once a proper tuner and demod untune feature is implemented. 
            But, as the demod is digital, powering it up and down should add very little time delay. */

    rc = BADS_EnablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
    if(rc){rc = BERR_TRACE(rc); goto done;}         

    pDevice->isPoweredOn[pChannel->chn_num] = false;

done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3128_Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BTNR_PowerSaverSettings pwrSettings;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    BSTD_UNUSED(pSettings);

    if (pChannel->chn_num >= NEXUS_MAX_3128_FRONTENDS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if(enabled){
        if(pChannel->chn_num == NEXUS_3128_OOB_CHN){
            if((!pSettings->wakeupSettings.transport)){
#if NEXUS_FRONTEND_312x_OOB         
                rc = BAOB_EnablePowerSaver(pDevice->aob);
                if(rc){rc = BERR_TRACE(rc); goto done;}
#endif    
            }
        }
        else{
            rc = BADS_EnablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
            if(rc){rc = BERR_TRACE(rc); goto done;}         
        }

        pwrSettings.enable = true;
        rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &pwrSettings);
        if (rc){rc = BERR_TRACE(rc);}
        
        pDevice->isPoweredOn[pChannel->chn_num] = false;
    }
    else{
        pwrSettings.enable = false;
        rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &pwrSettings);
        if (rc){rc = BERR_TRACE(rc);}
        
        if (pChannel->chn_num == NEXUS_3128_OOB_CHN){
#if NEXUS_FRONTEND_312x_OOB         
            rc = BAOB_DisablePowerSaver(pDevice->aob);
            if(rc){rc = BERR_TRACE(rc); goto done;}
#endif          
        }
        else{
            rc = BADS_DisablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
            if(rc){rc = BERR_TRACE(rc); goto done;}         
        }
        
        pDevice->isPoweredOn[pChannel->chn_num] = true;
    }

done:
    return rc;
}

static NEXUS_FrontendLockStatus  NEXUS_Frontend_P_GetAdsLockStatus(BADS_LockStatus lockStatus)
{
    switch ( lockStatus )
    {
    case BADS_LockStatus_eUnlocked:
        return NEXUS_FrontendLockStatus_eUnlocked;
    case BADS_LockStatus_eLocked:
        return NEXUS_FrontendLockStatus_eLocked;
    case BADS_LockStatus_eNoSignal:
        return NEXUS_FrontendLockStatus_eNoSignal;
    default:
        BDBG_WRN(("Unrecognized lock status (%d) ", lockStatus));
        BERR_TRACE(BERR_NOT_SUPPORTED);
        return NEXUS_FrontendLockStatus_eUnknown;
    }   
}

#if NEXUS_FRONTEND_312x_OOB
static NEXUS_FrontendLockStatus  NEXUS_Frontend_P_GetAobLockStatus(BAOB_LockStatus lockStatus)
{
    switch ( lockStatus )
    {
    case BAOB_LockStatus_eUnlocked:
        return NEXUS_FrontendLockStatus_eUnlocked;
    case BAOB_LockStatus_eLocked:
        return NEXUS_FrontendLockStatus_eLocked;
    case BAOB_LockStatus_eNoSignal:
        return NEXUS_FrontendLockStatus_eNoSignal;
    default:
        BDBG_WRN(("Unrecognized lock status (%d) ", lockStatus));
        BERR_TRACE(BERR_NOT_SUPPORTED);
        return NEXUS_FrontendLockStatus_eUnknown;
    }   
}
#endif

static NEXUS_Error NEXUS_Frontend_P_3128_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BADS_LockStatus eLock = BADS_LockStatus_eUnlocked;
#if NEXUS_FRONTEND_312x_OOB 
    BAOB_LockStatus bLock = BAOB_LockStatus_eUnlocked;
#endif
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    pStatus->lockStatus = NEXUS_FrontendLockStatus_eUnknown;

    if(pChannel->chn_num < NEXUS_3128_MAX_DOWNSTREAM_CHANNELS){
        rc = BADS_GetLockStatus(pDevice->ads_chn[pChannel->chn_num],  &eLock);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pStatus->lockStatus = NEXUS_Frontend_P_GetAdsLockStatus(eLock);
    }
#if NEXUS_FRONTEND_312x_OOB     
    else if(pChannel->chn_num == NEXUS_3128_OOB_CHN){
        rc = BAOB_GetLockStatus(pDevice->aob,  &bLock);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pStatus->lockStatus = NEXUS_Frontend_P_GetAobLockStatus(bLock);
    }
#endif
    pStatus->acquireInProgress = pDevice->acquireInProgress[pChannel->chn_num];
    return BERR_SUCCESS;
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_GetQamScanStatus(void *handle, NEXUS_FrontendQamScanStatus *pScanStatus)
{
    NEXUS_Error  rc;
    struct BADS_ScanStatus st;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    BKNI_Memset(pScanStatus, 0, sizeof(*pScanStatus));

    if (pChannel->chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    rc = BADS_GetScanStatus(pDevice->ads_chn[pChannel->chn_num],  &st);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
    pScanStatus->spectrumInverted = st.isSpectrumInverted;
    pScanStatus->symbolRate = st.symbolRate;
    pScanStatus->frequencyOffset = st.carrierFreqOffset;
    pScanStatus->interleaver = st.interleaver;
    pScanStatus->acquisitionStatus = st.acquisitionStatus;
    if(st.modType < BADS_ModulationType_eAnnexBQam16) {
        pScanStatus->annex = NEXUS_FrontendQamAnnex_eA;
        pScanStatus->mode = st.modType;
    }
    else if(st.modType < BADS_ModulationType_eAnnexCQam16) {
        pScanStatus->annex = NEXUS_FrontendQamAnnex_eB;
        pScanStatus->mode = st.modType - BADS_ModulationType_eAnnexBQam16;
    }
    else 
        BDBG_ERR(("Unsupported Annex."));
    return BERR_SUCCESS;
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned j=0;
    uint32_t buf=0;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if (pChannel->chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = true;

    rc = NEXUS_Frontend_P_3128_RequestQamAsyncStatus(pChannel);
    if(rc){rc = BERR_TRACE(rc); goto done;} 

    for(j=0; j < 100; j++) {

        BKNI_Sleep(20);

        rc = BHAB_ReadRegister(pDevice->hab, BCHP_LEAP_CTRL_SW_SPARE0 , &buf);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        if (buf & (BHAB_ADS_CHN0_STATUS_RDY << pChannel->chn_num)) {
            rc = NEXUS_Frontend_P_3128_GetQamAsyncStatus(pChannel, pStatus);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            break;
        }
    }
    
    pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = false;
    return BERR_SUCCESS;
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_GetQamAsyncStatus(void *handle, NEXUS_FrontendQamStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BADS_Status st;
    uint64_t totalbits=0, uncorrectedBits=0;
    unsigned cleanBlock = 0, correctedBlock = 0, unCorrectedBlock = 0, totalBlock = 0;
    NEXUS_Time currentTime;
    NEXUS_PreviousStatus *prevStatus;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if (pChannel->chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BADS_GetAsyncStatus(pDevice->ads_chn[pChannel->chn_num],  &st);
    if(rc){rc = BERR_TRACE(rc); goto done;} 

    prevStatus = &pDevice->previousStatus[pChannel->chn_num];
    NEXUS_Time_Get(&currentTime);
    pStatus->fecLock = st.isFecLock;
    pStatus->receiverLock = st.isQamLock;
    pStatus->rfAgcLevel = st.agcExtLevel;
    pStatus->ifAgcLevel = st.agcIntLevel;
    pStatus->snrEstimate = st.snrEstimate*100/256;
    pStatus->fecCorrected = st.accCorrectedCount;
    pStatus->fecUncorrected = st.accUncorrectedCount;
    pStatus->fecClean = st.accCleanCount;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->symbolRate = st.rxSymbolRate;
    pStatus->berRawCount = st.berRawCount;
    pStatus->dsChannelPower = st.dsChannelPower;
    pStatus->mainTap = st.mainTap;
    pStatus->settings = pDevice->last_ads[pChannel->chn_num];
    pStatus->spectrumInverted = st.isSpectrumInverted;
    pStatus->highResEqualizerGain = st.equalizerGain;
    pStatus->digitalAgcGain = st.digitalAgcGain;
    pStatus->frontendGain= st.feGain;

    pStatus->postRsBerElapsedTime = NEXUS_Time_Diff(&currentTime, &prevStatus->time);

    if(pStatus->fecUncorrected  > prevStatus->fecUncorrected)
        unCorrectedBlock = pStatus->fecUncorrected - prevStatus->fecUncorrected;
    if(pStatus->fecClean > prevStatus->fecClean)
        cleanBlock = pStatus->fecClean - prevStatus->fecClean;   
    if(pStatus->fecCorrected > prevStatus->fecCorrected)
        correctedBlock = pStatus->fecCorrected - prevStatus->fecCorrected;
    
    totalBlock = (uint64_t)(unCorrectedBlock + cleanBlock + correctedBlock);

    if(totalBlock > unCorrectedBlock){
        unCorrectedBlock = (uint64_t)unCorrectedBlock * 11224 / 1000;
        if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eA)
            pStatus->postRsBer = ((uint64_t)unCorrectedBlock * 2097152 * 1024 )/((uint64_t)totalBlock*8*187);
        else if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eB)
            pStatus->postRsBer = ((uint64_t)unCorrectedBlock * 2097152 * 1024)/((uint64_t)totalBlock*7*122);
    }

    pStatus->viterbiUncorrectedBits = st.correctedBits + (uint32_t)((uint64_t)pStatus->fecUncorrected * 11224)/1000;
    if(pStatus->viterbiUncorrectedBits > prevStatus->viterbiUncorrectedBits)
        uncorrectedBits = pStatus->viterbiUncorrectedBits - prevStatus->viterbiUncorrectedBits;

    if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eA){
        pStatus->viterbiTotalBits = (uint32_t)(((uint64_t)pStatus->fecCorrected + (uint64_t)pStatus->fecUncorrected + (uint64_t)pStatus->fecClean) * 204 * 8);  
    }
    else if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eB){
        pStatus->viterbiTotalBits = (uint32_t)(((uint64_t)pStatus->fecCorrected + (uint64_t)pStatus->fecUncorrected + (uint64_t)pStatus->fecClean) * 127 * 7);  
    }

    if(pStatus->viterbiTotalBits > prevStatus->viterbiTotalBits)
        totalbits =  pStatus->viterbiTotalBits - prevStatus->viterbiTotalBits;

    if (totalbits > uncorrectedBits) {
        pStatus->viterbiErrorRate = (uint32_t)((uint64_t)uncorrectedBits * 2097152 * 1024 / totalbits);
    }

    prevStatus->fecUncorrected = pStatus->fecUncorrected;
    prevStatus->fecClean = pStatus->fecClean;
    prevStatus->fecCorrected = pStatus->fecCorrected;
    prevStatus->viterbiUncorrectedBits = pStatus->viterbiUncorrectedBits;
    prevStatus->viterbiTotalBits = pStatus->viterbiTotalBits;
    prevStatus->time = currentTime;

    return BERR_SUCCESS;
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_RequestQamAsyncStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if (pChannel->chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    rc = BADS_RequestAsyncStatus(pDevice->ads_chn[pChannel->chn_num]);
    if(rc){rc = BERR_TRACE(rc); goto done;} 

    return BERR_SUCCESS;
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_ReadSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead)
{
    #define TOTAL_ADS_SOFTDECISIONS 30

    NEXUS_Error rc = NEXUS_SUCCESS;
    int16_t d_i[TOTAL_ADS_SOFTDECISIONS], d_q[TOTAL_ADS_SOFTDECISIONS];
    int16_t return_length;
    NEXUS_3128 *pDevice;
    unsigned i;    
    NEXUS_3128Channel *pChannel;
    
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    BKNI_Memset(pDecisions, 0, (sizeof(NEXUS_FrontendSoftDecision) * length));

    if (pChannel->chn_num >= NEXUS_3128_MAX_DOWNSTREAM_CHANNELS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    /* only make one call to ADS. if app needs more, they can loop. */
    rc = BADS_GetSoftDecision(pDevice->ads_chn[pChannel->chn_num], (int16_t)TOTAL_ADS_SOFTDECISIONS, d_i, d_q, &return_length);
    if(rc){rc = BERR_TRACE(rc); goto done;} 
    
    for (i=0; (int)i<return_length && i<length; i++)
    {
        pDecisions[i].i = d_i[i] * 2;
        pDecisions[i].q = d_q[i] * 2;
    }
    *pNumRead = i;
    
    return BERR_SUCCESS;
    
done:
    return rc;
}

NEXUS_Error NEXUS_Frontend_P_3128_ReapplyTransportSettings(void *handle)
{
#if NEXUS_HAS_MXT
    NEXUS_3128Channel *pChannel = (NEXUS_3128Channel *)handle;
    NEXUS_3128 *pDevice;
    unsigned chn_num;
    NEXUS_Error rc;
    
    BDBG_ASSERT(pChannel);
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_ASSERT(pDevice);
    chn_num = pChannel->chn_num;
    rc = NEXUS_Frontend_P_SetMtsifConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(pDevice->frontendHandle[chn_num], &pDevice->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTsmfConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
#else
    BSTD_UNUSED(handle);
#endif

    return NEXUS_SUCCESS;
}

void NEXUS_FrontendDevice_GetDefault3128Settings(NEXUS_FrontendDevice3128Settings *pSettings)
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->agcValue = 0x1f; /* Max gain*/
}

NEXUS_Error NEXUS_FrontendDevice_Get3128Settings(NEXUS_FrontendDeviceHandle handle, NEXUS_FrontendDevice3128Settings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128 *pDevice = NULL;
    BDBG_ASSERT(handle != NULL);    

    pDevice = handle->pDevice;
    if (!pDevice) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done; }
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    *pSettings = pDevice->deviceSettings;
done:   
    return rc;

}

NEXUS_Error NEXUS_FrontendDevice_Set3128Settings(NEXUS_FrontendDeviceHandle handle, const NEXUS_FrontendDevice3128Settings *pSettings )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint32_t buf;
    NEXUS_3128 *pDevice = NULL;
    BHAB_ConfigSettings habConfigSettings;
#if NEXUS_FRONTEND_312x_OOB 
    BAOB_ConfigSettings aobConfigSettings;
#endif

    BDBG_ASSERT(handle != NULL);
    pDevice = handle->pDevice;
    if (!pDevice) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done; }
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    NEXUS_IsrCallback_Set(pDevice->updateGainAppCallback, &(pSettings->updateGainCallback));
    pDevice->deviceSettings.updateGainCallback = pSettings->updateGainCallback;

    rc = BHAB_ReadRegister(pDevice->hab, BCHP_TM_SFT0, &buf);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    if(buf & 0x80000000) {
        /* If the CPPM bit is set by docsis and is being consumed by 3128(once consumed, 3128 sets this bit to 0), then update 3128 CPPM */
        buf = pSettings->agcValue;
        buf |= 0x80000000;
    }
    pDevice->deviceSettings.agcValue = pSettings->agcValue;

    rc = BHAB_WriteRegister(pDevice->hab, BCHP_TM_SFT0, &buf);  
    if(rc){rc = BERR_TRACE(rc); goto done;}

    if(pSettings->enableRfLoopThrough != pDevice->deviceSettings.enableRfLoopThrough) {
        rc = BHAB_GetConfigSettings(pDevice->hab, &habConfigSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        
        habConfigSettings.enableLoopThrough = pSettings->enableRfLoopThrough;

        rc = BHAB_SetConfigSettings(pDevice->hab, &habConfigSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        pDevice->deviceSettings.enableRfLoopThrough = pSettings->enableRfLoopThrough;
    }

#if NEXUS_FRONTEND_312x_OOB
    if(pSettings->outOfBand.outputMode != pDevice->deviceSettings.outOfBand.outputMode) {       
        if(pDevice->isPoweredOn[NEXUS_3128_OOB_CHN]) {
            rc = BAOB_GetConfigSettings(pDevice->aob, &aobConfigSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}

            aobConfigSettings.outputMode = pSettings->outOfBand.outputMode;
            rc = BAOB_SetConfigSettings(pDevice->aob, &aobConfigSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
        pDevice->deviceSettings.outOfBand.outputMode = pSettings->outOfBand.outputMode;            
    }
#endif
done:
    return rc;
}

static void NEXUS_Frontend_P_3128_ResetQamStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);

    if(pDevice->isPoweredOn[pChannel->chn_num]) {
        rc = BADS_ResetStatus(pDevice->ads_chn[pChannel->chn_num]);
        if (rc){BERR_TRACE(rc);}
    }
    else{
        BDBG_MSG(("The downstream core is Powered Off"));
    }
}

/***************************************************************************
Summary:
  Probe to see if a BCM3128 device exists with the specified settings

Description:
  Probe to see if a BCM3128 device exists with the specified settings

See Also:
    NEXUS_Frontend_Open3128
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Probe3128(const NEXUS_FrontendDevice3128OpenSettings *pSettings, NEXUS_3128ProbeResults *pResults)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint16_t chipVer=0xffff;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->i2cDevice);
    BDBG_ASSERT(NULL != pResults);

    pResults->chip.familyId = 0x0;

    pResults->chip.familyId = (uint32_t)NEXUS_Frontend_P_Is3128Family(pSettings);
    if ( pResults->chip.familyId != 0x3128 )
    {
        BDBG_ERR(("pResults->chip.familyId = 0x%x", pResults->chip.familyId));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    pResults->chip.id = NEXUS_Frontend_P_get3128ChipId(pSettings, pResults->chip.familyId);
    chipVer = NEXUS_Frontend_P_Get3128Rev(pSettings);
    
    pResults->chip.version.major = (chipVer >> 8) + 1;
    pResults->chip.version.minor = chipVer & 0xff;
done:
    return rc;
}

/***************************************************************************
Summary:
    Retrieve the chip family id, chip id, chip version and firmware version.
***************************************************************************/
static void NEXUS_Frontend_P_3128_GetType(void *handle, NEXUS_FrontendType *type)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BFEC_SystemVersionInfo  versionInfo;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    
    rc = BHAB_GetVersionInfo(pDevice->hab, &versionInfo);
    if(rc) BERR_TRACE(rc);

    type->chip.familyId = (uint32_t)pDevice->chipId;    
    type->chip.id = (uint32_t)pDevice->chipId;
    type->chip.version.major = (pDevice->revId >> 8) + 1;
    type->chip.version.minor = pDevice->revId & 0xff;
    type->chip.bondoutOptions[0] = versionInfo.bondoutOptions[0];
    type->chip.bondoutOptions[1] = versionInfo.bondoutOptions[1];
    type->chip.version.buildType = 0;
    type->chip.version.buildId = 0;

    type->firmwareVersion.major = versionInfo.firmware.majorVersion;
    type->firmwareVersion.minor = versionInfo.firmware.minorVersion;
    type->firmwareVersion.buildType = versionInfo.firmware.buildType;
    type->firmwareVersion.buildId = versionInfo.firmware.buildId;

    if((type->chip.familyId != versionInfo.familyId) || (type->chip.id != versionInfo.chipId)){
        BDBG_ERR(("Type mismatch while retreiving chip id and family id."));
        BERR_TRACE(BERR_UNKNOWN); goto done;
    }
    if(pDevice->revId != versionInfo.chipVersion){
        BDBG_ERR(("Type mismatch while retreiving chip version."));
        BERR_TRACE(BERR_UNKNOWN); goto done;
    }
done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3128_RequestSpectrumAnalyzerData(void *handle, const NEXUS_FrontendSpectrumSettings *pSettings )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BTNR_SpectrumSettings settings;
    BTNR_PowerSaverSettings pwrSettings;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BDBG_ASSERT(NULL != pSettings);

    switch(pSettings->fftSize)
    {
    case 64:
        settings.fftSize = 0x6;
        break;
    case 128:
        settings.fftSize = 0x7;
        break;
    case 256:
        settings.fftSize = 0x8;
        break;
    case 512:
        settings.fftSize = 0x9;
        break;
    case 1024:
        settings.fftSize = 0xa;
        break;
    case 2048:
        settings.fftSize = 0xb;
        break;
    case 4096:
        settings.fftSize = 0xc;
        break;
    default:
        BDBG_ERR(("FFT size not supported. Current supported FFT sizes are 64, 128, 256, 512, 1024 and 2048"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    settings.numSamples = pSettings->numSamples;
    settings.startFreq = pSettings->startFrequency;
    settings.stopFreq = pSettings->stopFrequency;
#if 0 /* binAverage is not used any more */
    settings.binAverage = pSettings->binAverage;
#endif 

    pDevice->spectrumDataPointer = (uint32_t *)pSettings->data;

    NEXUS_TaskCallback_Set(pDevice->spectrumDataAppCallback[pChannel->chn_num], &(pSettings->dataReadyCallback));
   
    if(pDevice->spectrumEventCallback == NULL) {
        pDevice->spectrumEventCallback = NEXUS_RegisterEvent(pDevice->spectrumEvent, NEXUS_Frontend_P_3128_spetctrumEventCallback, pChannel);
        if ( NULL == pDevice->spectrumEventCallback ){rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto done; }
    }

    if (pDevice->tnr[pChannel->chn_num])
    {
        pwrSettings.enable = false;
        rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &pwrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;} 
    }

    rc = BTNR_RequestSpectrumAnalyzerData(pDevice->tnr[pChannel->chn_num], &settings);
    if (rc){BERR_TRACE(rc);}
done:
    return rc;
}

#if NEXUS_FRONTEND_312x_OOB
static NEXUS_Error NEXUS_Frontend_P_3128_TuneOob(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BAOB_AcquireParam obParams;
    BTNR_PowerSaverSettings pwrSettings;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_ASSERT(pChannel->chn_num == NEXUS_3128_OOB_CHN);

    if(pDevice->deviceSettings.outOfBand.outputMode == NEXUS_FrontendOutOfBandOutputMode_eMax){
        BDBG_ERR(("Out of band output mode set to %d is not supported. Use NEXUS_FrontendDevice_Set3128Settings() to set the right settings."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    rc = NEXUS_Frontend_P_SetMtsifConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
    rc = NEXUS_Frontend_P_SetTransportConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
    rc = NEXUS_Frontend_P_SetTsmfConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
    if (rc) { rc = BERR_TRACE(rc); goto done; }
#endif
#endif

    NEXUS_IsrCallback_Set(pDevice->lockAppCallback[NEXUS_3128_OOB_CHN], &(pSettings->lockCallback));
    NEXUS_IsrCallback_Set(pDevice->asyncStatusAppCallback[NEXUS_3128_OOB_CHN], &(pSettings->asyncStatusReadyCallback));

    if (pDevice->tnr[NEXUS_3128_OOB_CHN])
    {
        if(!pDevice->isPoweredOn[NEXUS_3128_OOB_CHN]){
            pwrSettings.enable = false;
            rc = BTNR_SetPowerSaver(pDevice->tnr[NEXUS_3128_OOB_CHN], &pwrSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            rc = BAOB_DisablePowerSaver(pDevice->aob);
            if(rc){rc = BERR_TRACE(rc); goto done;}

            pDevice->isPoweredOn[NEXUS_3128_OOB_CHN] = true;
        }
        rc = BTNR_SetTunerRfFreq(pDevice->tnr[NEXUS_3128_OOB_CHN], pSettings->frequency, BTNR_TunerMode_eDigital);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    rc = BAOB_GetDefaultAcquireParams(&obParams);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
    obParams.autoAcquire = pSettings->autoAcquire;
    obParams.modType = (BAOB_ModulationType)pSettings->mode ;
    obParams.symbolRate = pSettings->symbolRate;
    obParams.berSrc = pSettings->bertSource;
    obParams.spectrum = pSettings->spectrum;

    rc = BAOB_SetAcquireParams(pDevice->aob, &obParams);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BAOB_Acquire(pDevice->aob, &obParams);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pDevice->last_aob = *pSettings;
    return BERR_SUCCESS;
done:
    return rc;
}

static void NEXUS_Frontend_P_3128_UnTuneOob(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_3128_OOB_CHN);

    if (pChannel->chn_num == NEXUS_3128_OOB_CHN){
#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
        NEXUS_Frontend_P_UnsetMtsifConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
        NEXUS_Frontend_P_UnsetTransportConfig(pDevice->frontendHandle[pChannel->chn_num], &pDevice->mtsifConfig);
#endif
#endif

        rc = BAOB_EnablePowerSaver(pDevice->aob);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        pDevice->isPoweredOn[NEXUS_3128_OOB_CHN] = false;
    }
    else {
        BERR_TRACE(BERR_INVALID_PARAMETER);
    }
done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3128_GetOobStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned j=0;
    uint32_t buf=0;
    NEXUS_3128 *pDevice;
    NEXUS_3128Channel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_3128_OOB_CHN);
    
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = true;

    rc = NEXUS_Frontend_P_3128_RequestOobAsyncStatus(pChannel);
    if(rc){rc = BERR_TRACE(rc); goto done;} 

    for(j=0; j < 20; j++) {

        BKNI_Sleep(20);

        rc = BHAB_ReadRegister(pDevice->hab, BCHP_LEAP_CTRL_SW_SPARE0 , &buf);
        if(rc){rc = BERR_TRACE(rc); goto done;}
            
        if(buf & BHAB_OOB_SPARE_STATUS_RDY) {
            BDBG_ERR(("Reading oob async status."));
            rc = NEXUS_Frontend_P_3128_GetOobAsyncStatus(pChannel, pStatus);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            
            BDBG_ERR(("oob async status: isFecLocked = %d", pStatus->isFecLocked));
            break;
        }
    }

    pDevice->isInternalAsyncStatusCall[pChannel->chn_num] = false;

done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_GetOobAsyncStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BAOB_Status st;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    uint64_t elapsedSymbols=0;
    unsigned unCorrectedBlock = 0;
    NEXUS_Time currentTime;
    NEXUS_PreviousStatus *prevStatus;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_3128_OOB_CHN);
    
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BAOB_GetAsyncStatus(pDevice->aob,  &st);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pStatus->isFecLocked = st.isFecLock;
    pStatus->isQamLocked = st.isQamLock;
    pStatus->symbolRate = st.symbolRate;
    pStatus->snrEstimate = st.snrEstimate*100/256;
    pStatus->agcIntLevel = st.agcIntLevel;
    pStatus->agcExtLevel = st.agcExtLevel;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;
    pStatus->correctedCount = st.correctedCount;
    pStatus->uncorrectedCount = st.uncorrectedCount;
    pStatus->mode = (NEXUS_FrontendOutOfBandMode)st.modType;
    pStatus->symbolRate = st.symbolRate;
    pStatus->ifFreq = 0;/* Not required on 3128*/
    pStatus->loFreq = 0;/* Not required on 3128*/
    pStatus->sysXtalFreq = st.sysXtalFreq;
    pStatus->berErrorCount = st.berErrorCount;
    pStatus->fdcChannelPower = st.aobChannelPower/10;
    pStatus->frontendGain = st.feGain;
    pStatus->digitalAgcGain = st.digitalAgcGain;
    pStatus->highResEqualizerGain = st.equalizerGain;
    pStatus->acquisitionTime = st.acquisitionTime;
    pStatus->totalAcquisitionTime = st.totalAcquisitionTime;
    pStatus->postRsBer = 0;
    
    prevStatus = &pDevice->previousStatus[pChannel->chn_num];
    NEXUS_Time_Get(&currentTime);
    if(pStatus->uncorrectedCount  > prevStatus->fecUncorrected)
       unCorrectedBlock = pStatus->uncorrectedCount - prevStatus->fecUncorrected;
    
    pStatus->postRsBerElapsedTime = NEXUS_Time_Diff(&currentTime, &prevStatus->time);

    elapsedSymbols = pStatus->postRsBerElapsedTime * pStatus->symbolRate;
    if(elapsedSymbols){
        if(pStatus->settings.mode == NEXUS_FrontendOutOfBandMode_eAnnexAQpsk){
            pStatus->postRsBer = ((uint64_t)unCorrectedBlock * 2097152 * 1024 * 55)/(elapsedSymbols * 2 * 53);
        }
        else if(pStatus->settings.mode == NEXUS_FrontendOutOfBandMode_eDvs178Qpsk){
            /*pStatus->postRsBer = ((uint64_t)unCorrectedBlock * 2097152 * 1024 * 96 * 11)/(elapsedSymbols * 94 * 2 * 100000);*/
            pStatus->postRsBer = ((uint64_t)unCorrectedBlock * 2097152 * 1024 * 3 * 11)/(elapsedSymbols * 47 * 12500);
        }
    }
  
    prevStatus->fecUncorrected = pStatus->uncorrectedCount;
    prevStatus->time = currentTime;

    BDBG_MSG((" OOB ASYNC STATUS : fec_lock = %d,  qam_lock = %d, snr_estimate = %d, fec_corr_cnt = %d\n",
            st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel, st.snrEstimate,
            st.correctedCount, st.uncorrectedCount, st.berErrorCount));

    return BERR_SUCCESS;
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_3128_RequestOobAsyncStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_3128_OOB_CHN);
    
    rc = BAOB_RequestAsyncStatus(pDevice->aob);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    return BERR_SUCCESS;
done:
    return rc;
}

static void NEXUS_Frontend_P_3128_ResetOobStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_3128Channel *pChannel;
    NEXUS_3128 *pDevice;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_3128Channel *)handle;
    pDevice = (NEXUS_3128 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3128);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_3128_OOB_CHN);

    rc = BAOB_ResetStatus(pDevice->aob);
    if(rc){rc = BERR_TRACE(rc);}
    return;
}
#endif
