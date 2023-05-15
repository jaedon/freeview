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
* $brcm_Workfile: nexus_frontend_3255.c $
* $brcm_Revision: 139 $
* $brcm_Date: 11/13/12 5:39p $
*
* API Description:
*   API name: Frontend 3255
*    APIs to open, close, and setup initial settings for a BCM3255
*    Frontend Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/3255/src/nexus_frontend_3255.c $
*
* 139   11/13/12 5:39p mward
* SW7435-433: Warnings about no effect from a debug message.
*
* 138   10/23/12 4:18p khat
* SW7425-4037: implement Nexus function that will wrap a magnum PI
*  function, BAUS_TransmitStarvuePkt
*
* 137   10/19/12 5:35p anilmm
* SW7425-3830: Destroy asyncStatusAppCallback upon shutdown
*
* 136   10/17/12 11:19a khat
* SW7425-4063: Removed BKNI_Free call, err_init already handle it.
*
* 135   10/16/12 11:46a jtna
* SW7425-3782: fix compiler error
*
* 134   10/15/12 3:00p anilmm
* SW7425-3830: Hook up Get/RequestAsyncQamStatus to common frontend
*  functions.
*
* 133   10/3/12 5:01p jtna
* SW7425-3782: refactor nexus tsmf impl
*
* 132   9/19/12 2:14p jtna
* SW7425-3782: fix typo
*
* 131   9/6/12 12:05p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
*
* 130   9/6/12 10:54a gmohile
* SW7425-3077 : Power on frontend for active standby
*
* 129   8/2/12 11:46a nickh
* SW7425-3077: Add private standby function that initiates RPC call to
*  put 3383 into appropriate power mode
*
* 128   7/26/12 3:09p jtna
* SW7425-3514: add frontend GetTemperature APIs
*
* 127   7/19/12 4:14p jtna
* SW3472-14: add 3472 MXT support. change MTSIF in API naming to
*  MTSIF_TX, in order to differentiate with MTSIF_RX
*
* 126   7/18/12 7:32p nickh
* SW7425-3247: Store the lockStatus state
*
* 125   7/12/12 2:39p nickh
* SW7425-3232: Add API to allow manual reconfiguration of frontend LNA
*
* 124   6/27/12 3:06p jtna
* SW7425-3350: remove NEXUS_FRONTEND_FIXED_MTSIF_CONFIG compile option.
*  it's no longer needed
*
* 123   6/26/12 3:34p jtna
* SW7425-3062: add feature to propagate host parserband settings to
*  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
*
* 122   6/25/12 4:33p vsilyaev
* SW7425-3247: Removed superfluous 'const'
*
* 121   6/15/12 11:51a nickh
* SW7425-3247: Add support for sending non-Docsis video channel lock
*  status to the Docsis frontend
*
* 120   6/8/12 3:28p jtna
* SW7425-3171: add NEXUS_Frontend_ReapplyTransportSettings
*
* 119   6/4/12 6:16p prasadv
* SW7425-2732: Initialize 3255 device status to operational before
*  creating the heartbeat thread.
*
* 118   5/25/12 1:55p nickh
* SW7425-3072: Add support for retrieving Docsis status information for
*  any available Docsis channel
*
* 118   5/25/12 1:54p nickh
* SW7425-3072: Add support for retrieving Docsis status information for
*  any available Docsis channel
*
* 117   5/15/12 5:44p jtna
* SW7425-2752: disable all parsers after MXT open
*
* 116   5/13/12 6:29p jtna
* SW7425-2752: rework #if NEXUS_HAS_MXT sections
*
* 115   5/13/12 5:06p jtna
* SW7425-2752: add NEXUS_FRONTEND_FIXED_MTSIF_CONFIG option to support
*  legacy mode
*
* 114   5/10/12 7:41p jtna
* SW7425-2752: fix compile for non-MXT platforms
*
* 113   5/10/12 7:20p jtna
* SW7425-2752: keep demod parsers enabled until we go live
*
* 112   5/10/12 6:52p jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
*
* 111   4/11/12 1:51p erickson
* SW7425-2512: add new 3255 LNA values and clarify assignment to 3128
*
* 110   3/30/12 12:34p nickh
* SW7425-2733: Open the correct ADS channels according to the reserved
*  video channels
*
* 109   3/7/12 11:28a jtna
* SW7425-2267: wrap code with #if NEXUS_HAS_MXT
*
* 108   3/6/12 1:30p prasadv
* SW7425-2421: Frontend 3255 RPC notification thread consumes a lot of
*  cpu cycles if export "no_3255=y"
*
* 107   2/28/12 2:13p prasadv
* SW7425-2421: Frontend 3255 RPC notification thread consumes a lot of
*  cpu cycles if export "no_3255=y"
*
* 106   2/24/12 5:02p nickh
* SW7425-2436: Retrieve the last successfully registered Docsis frequency
*
* 105   2/6/12 4:21p jtna
* SW7425-2267: use BMXT_SetParserMapping instead of direct RPC calls
*
* 104   1/31/12 12:59p nickh
* SW7425-2305: Retrieve 3383 Chip ID and Rev and pass in the appropriate
*  parameters to the MXT PI and number of TSMF parsers
*
* 103   12/19/11 5:33p nickh
* SW7425-2023: Add SetParserBandId RPC call
*
* 102   12/19/11 4:47p vishk
* SW3128-92: Add FW auto detection support.
*
* 101   11/22/11 12:49p nickh
* SW7425-1831: Fix memory allocation of frontend structure to account for
*  OOB channel
*
* 100   11/2/11 11:15a spothana
* SW7425-1350: Fix the tsmf settings association with QAM capable DOCSIS
*  channels.
*
* 99   10/21/11 7:27p jtna
* SW7425-1483: call 3383-specific implementation of MXT
*
* 98   10/20/11 4:50p spothana
* SW7425-1350: Correct the comment for legacy mode.
*
* 97   10/20/11 4:20p spothana
* SW7425-1350: In the legacy mode by default assign all the docsis
*  channels as data channels
*
* 96   10/19/11 10:28p spothana
*  SW7425-1350: Initialize the docsis device and then associate the
*  frontend handles with docsis channels based on the platform frontend
*  initialization sequence
*
* 95   10/14/11 2:52p jtna
* SW7425-1189: added NEXUS_TsmfSettings.frontendTsmfIndex. allow change
*  between auto and semi-auto mode without enable/disable.
*
* 94   10/6/11 5:22p jtna
* SW7425-1327: add NEXUS_Frontend_Get/SetTransportSettings()
*
* 93   10/6/11 4:50p jtna
* SW7425-1189: change return type to void. make tsmfSettings per frontend
*  handle, not per device.
*
* 92   10/5/11 4:17p jtna
* SW7425-1189: add transport and frontend TSMF support
*
* 91   10/3/11 3:58p prasadv
* SW7125-772:  Add fdcChannelPower to OOB status. Merge from SW7125-772/1
*  branch
*
* 91   10/3/11 3:57p prasadv
* SW7125-772:  Add fdcChannelPower to OOB status. Merge from SW7125-772/1
*  branch
*
* 91   10/3/11 3:56p prasadv
* SW7125-772:  Add fdcChannelPower to OOB status. Merge from SW7125-772/1
*
* 90   8/31/11 6:16p nickh
* SW7425-1129: Get the number of channels reserved for Docsis and set
*  frontends appropriately
*
* 89   7/7/11 5:26p spothana
* SW7425-763: Set LNA AGC of 3128 from 3383
*
* 88   6/27/11 6:18p nickh
* SW7425-771: Add support for 8 TNR tuners
*
* 87   6/23/11 11:55p nickh
* SW7425-771: Add support for 8 tuners
*
* SW7125-772/1   5/27/11 4:26p mward
* SW7125-772:  Add fdcChannelPower to OOB status.
*
* 86   5/20/11 6:19p mward
* SW7125-905:  I_VCOBUF_LATCH_ON field in DS_TUNER_REF_0_REF_00 is used
*  by tnr/7125 driver to detect need to init shared DS_TUNER_REF.  Clear
*  it on power-down.
*
* 85   5/5/11 5:50p mward
* SW7335-1215: Added NEXUS_CallbackHandler framework
*
* 84   4/21/11 2:48p spothana
* SW7425-369 : Enable DOCSIS channel 0 QAM tuning for BCM97425_SFF
*  platform.
*
* 83   3/21/11 3:48p mward
* SW7125-885:  Cancel any retuneTimer before scheduling a new one.
*
* 82   3/10/11 1:50p mward
* SW7125-734:  Cancel retuneTimer only for ADS channels, not OOB.
*
* 81   2/25/11 2:48p erickson
* SW7425-107: use #if NEXUS_MAX_3255_ADSCHN test instead of special macro
*
* 80   2/21/11 4:56p mward
* SW7125-737:  Uninstall ADS lock change callback before destroying the
*  corresponding Nexus callback.
*
* 79   2/17/11 6:14p haisongw
* SW7125-839: CLONE -Provide an API for the host to read LNA info when
*  the LNA is controlled by BNM
*
* 78   2/17/11 10:13a mward
* SW7125-737:  Allow power down in untune of 31xx, but don't do it
*  automatically in uninit pending "deep power up" fix in DOCSIS and host
*  31xx drivers.
*
* 77   2/16/11 3:20p mward
* SW7125-790:  STB_3.0.1beta4_02162011 doesn't power-on, temporarily
*  disable power-off in untune.
*
* 76   2/10/11 11:39a mward
* SW7125-825:  Null Magnum handles after closing them.
*
* 75   2/2/11 5:07p garetht
* SW7420-1457:sync sms & vms platforms to 97420 latest
*
* 74   1/20/11 1:54p mward
* SW7125-737:  Change order in
*  NEXUS_Frontend_3255_PowerDownFrontendDevices() to prevent GISB
*  timeouts.
*
* 73   1/12/11 1:45p mward
* SW7125-737:  Always include DS0 and DS1 in
*  NEXUS_Frontend_3255_PowerDownFrontendDevices().  Add
*  BDBG_MSG_HEARTBEAT for periodic messages.
*
* 72   12/3/10 4:58p haisongw
* SW7400-2972: NEXUS_Frontend_GetUpstreamStatus doesn't update the status
*  fields other than mode
*
* 71   12/2/10 12:21p mward
* SW7125-728: Add Annex C support.
*
* 70   11/22/10 5:57p mward
* SW7125-737:  Disable BADS power saver command in untune.  Currently in
*  DOCSIS BTNR command powers down both DS and tuner.
*
* 69   11/19/10 9:54a mward
* SW7125-737:  Enable power saver in untune.
*
* 68   11/11/10 10:06a mward
* SW7125-734: Fix OVERRUN_STATIC.
*
* 67   11/9/10 5:16p haisongw
* SW7125-717: Add docsis scan control support (need 3.0.1beta1 docsis
*  image)
*
* 66   11/4/10 7:48p mward
* SW7125-497: Always PowerUpFrontendDS0 and PowerUpFrontendDS1 in
*  NEXUS_Frontend_3255_PowerUpFrontendDevices(), they may have been
*  powered down by untune under host control.
*
* 65   11/2/10 10:18a mward
* SW7125-497: Don't  power down DS0 if either DS0 or DS1 can be host
*  controlled.  DS1 shares reference with DS0.
*
* 64   10/22/10 12:24p ayxiang
* SW7125-675: no need to enable/disable RPC
*
* 63   10/13/10 1:42p mward
* SW7125-645: clarify in/out function parameters (for proxy) with "const"
*  and " out ".
*
* 61   10/1/10 5:10p haisongw
* SW7125-645: add API to retrieve QAM status ( including DPM)
*  asynchronously
*
* 60   9/7/10 10:30a gskerl
* SW7125-237: Cleaned up error handling.
*
* 59   9/1/10 12:10p mward
* SW7125-497: Added NEXUS_Frontend_P_3255_PowerUpFrontendDS1().  Don't
*  power down DS0 or DS1 if they can be host controlled.
*
* 58   9/1/10 11:28a mward
* SW7125-497: Frontend power control supported only on >= C0.
*
* 57   8/19/10 10:18a mward
* SW7125-497: use NEXUS_POWER_MANAGEMENT instead of NEXUS_FRONTEND_PWM
*  only if (BCHP_CHIP==7125).
*
* 56   8/5/10 5:17p mward
* SW7125-497: use NEXUS_POWER_MANAGEMENT instead of NEXUS_FRONTEND_PWM.
*
* 55   8/3/10 2:55p ayxiang
* SW7125-497: DS power up recovery order changed.
*
* 54   7/30/10 2:56p ayxiang
* SW7125-497: for power up recovery of frontend, need more enabling on
*  related registers bits
*
* 53   7/15/10 1:29p erickson
* SW7400-2855: add bounds check for static analysis
*
* 52   7/14/10 6:37p garetht
* SW7420-893: Cut release for 93380VMS Trinity Phase 4.1
*
* 51   7/8/10 5:25p ayxiang
* SW7125-497: power save by power down clock, PLL etc. and resume when
*  going back to normal
*
* 50   7/7/10 6:24p haisongw
* SW7125-181: Schedule a timer for DS auto re-acquire
*
* 49   7/7/10 3:55p mward
* SW7125-181:  Add a callback to implement autoAcquire.
*
* 48   7/2/10 3:26p mward
* SW7125-497:  eliminate warning about unused label.
*
* 47   6/29/10 7:04p ayxiang
* SW7125-497: move system call to application
*
* 46   6/29/10 6:39p mward
* SW7125-497: For proper build without NEXUS_FRONTEND_PWM, leave the API
*  functions defined, but stub them.
*
* 45   6/28/10 5:13p ayxiang
* SW7125-497: add 3255 pwm functions
*
* 44   6/17/10 11:17a haisongw
* SW7400-2827: Improve eCM heartbeat detection scheme
*
* 43   6/15/10 3:08p haisongw
* SW7125-474: use BRPC_Notification_Event_EcmRmagnumReady to reinitialize
*  frontend control faster when it is available
*
* 42   6/11/10 3:29p mward
* SW7125-35:  When timed out waiting for 3255, close RPC socket to avoid
*  the error "BRPC_SOCKET: Fail to bind sock_ntf!".  On 7125 timeout in a
*  reasonable amount of time.
*
* 41   4/15/10 10:31a jtna
* SW7125-312: Coverity Defect ID:20547 USE_AFTER_FREE && ID:20442
*  RESOURCE_LEAK
*
* 40   4/1/10 4:25p garetht
* SW7420-672: SMS/VMS do not use 3380 (3255) for Downstream
*
* 39   3/26/10 5:42p garetht
* SW7420-672: Allow non-vms/sms platforms to boot faster
*
* 38   3/25/10 2:14p garetht
* SW7420-672: check-in 93380vms/sms Phase 4.0 Trinity
*
* 37   3/24/10 2:20p haisongw
* SW7400-2531: Retreive Docsis status for 7125
*
* 36   3/10/10 5:54p haisongw
* SWCMSTB-829: Provide rMagnum API to set/get downstream freq table
*
* 35   3/9/10 2:46p ayxiang
* SW7125-227:default is OOB 1, or explicitly defined 1, otherwise OOB is
*  0
*
* 34   3/9/10 10:31a ayxiang
* SW7125-227: move B_HAS_OOB
*
* 33   1/20/10 4:06p mward
* SW7125-141:  Soft decisions for external 3112s on 97125 are now fixed
*  and scaled properly with STB_3_0_0alpha5 DOCSIS code.
*
* 32   1/20/10 2:44p haisongw
* SW7125-175: dectect OOB support on the fly
*
* 31   1/19/10 3:28p haisongw
* SW7420-450: Functionality required to check spectrum inversion status
*  from 3255R ADS
*
* 30   1/7/10 5:29p haisongw
* SW7125-148: Use GetVersion RPC instead of InitSession for Keepalive
*  timer
*
* 29   12/22/09 3:40p haisongw
* SW7125-148: Add eCM status change callback to application
*
* 28   12/17/09 2:20p mward
* SW7125-141:  Temporarily disable RMagnum call for tuners on 7125 that
*  correspond to external 3112.  DOCSIS code is not handling it yet.
*
* 27   12/16/09 3:44p haisongw
* SWDEPRECATED-3391:keep copy of Nexus3255 setting
*
* 26   12/1/09 2:43p haisongw
* SW7125-131: support hybrid Rmagnum tuners and host controlled tuners.
*
*
* 25   11/30/09 6:41p mward
* SW7125-131: Allow 4 Rmagnum tuners.
*
* 24   11/16/09 11:43a haisongw
* SW7400-2614:Nexus_frontend_3255.c is broken if B_HAS_OOB=0
*
* 23   11/12/09 4:04p jowo
* SW7400-2583: temporarily disable power saving mode for ADS qam
*
* 22   11/10/09 11:00a haisongw
* SW7125-47: 325x heartbeat thread crashes in 7125.
*
* 21   10/28/09 12:06p haisongw
* SW7400-2583:When untune, enable power saving mode for ADS inband tuner
*  and qam
*
* 20   10/6/09 5:48p haisongw
* SW7400-2531: Add 325x Frontend GPIO control support and retreive Docsis
*  status
*
* PROD_OCAP_REFSW_Devel/2   10/6/09 3:43p haisongw
* SW7400-2531: Add 325x Frontend GPIO control support and retreive Docsis
*  status
*
* PROD_OCAP_REFSW_Devel/1   10/6/09 11:46a haisongw
* SW7400-2531: Add 325x Frontend GPIO control support and retreive Docsis
*  status
*
* 18   8/26/09 12:21p haisongw
* SW7400-2479: Add two 3255 specific APIs to retrieve lockstatus directly
*
* 17   8/25/09 6:06p haisongw
* SW7400-2479: Add two 3255 specific APIs to retrieve lockstatus directly
*
* 16   8/25/09 3:45p haisongw
* SW7400-2479: Add two 3255 specific APIs to retrieve lockstatus directly
*
* 15   7/22/09 3:07p jowo
* PR57004: fix a missing break statement in
*  NEXUS_Frontend_P_3255_GetUsStatus()
*
* 14   7/15/09 3:57p haisongw
* PR56829: add an option to use Docsis DS0 tuner as video tuner
*
* 13   7/8/09 3:13p erickson
* PR56607: remove unused NEXUS_Frontend_P_Register function
*
* 12   7/2/09 2:29p haisongw
* PR52808: Add 325x OOB Untune() support
*
* 11   5/21/09 10:23a erickson
* PR55095: added NEXUS_FrontendQamStatus.interleaveDepth
*
* 10   5/4/09 4:26p haisongw
* PR52808: Add 325x Untune() support
*
* 9   4/16/09 2:58p erickson
* PR48963: fix warnings
*
* 8   3/5/09 11:53a prasadv
* PR52808: OOB SNR is in unit of 1/100dB.
*
* 7   3/5/09 10:50a prasadv
* PR52808: Add eCM reset notification event since docsis 2.2.2 release
*
* 6   12/16/08 5:12p anilmm
* PR47724:  Reinitialize AOB/AUS/ADS after 3255 reset
*
* 5   10/15/08 3:57p anilmm
* PR44830:  Clear device handle before setting object ID
*
* 4   10/14/08 6:32p anilmm
* PR47842: BER calculation software implementation
*
* 3   10/8/08 11:54a anilmm
* PR44830:  Add run-time support to bypass 3255 frontend
*
* 2   10/2/08 5:00p anilmm
* PR44830:  Add NEXUS_LockModule to synchronize with rest of Nexus
*
* 1   9/23/08 9:16a jgarrett
* PR 46513: Merging 97459 support to mainline
*
* PR44830/1   7/22/08 6:29p anilmm
* PR44830: Add 3255 frontend support
*
***************************************************************************/

#include "nexus_frontend_module.h"
#include "nexus_platform_features.h"
#include "bads.h"
#include "bads_3255.h"
#include "btnr.h"
#include "btnr_3255ib.h"
#include "brpc.h"
#include "brpc_3255.h"
#include "brpc_socket.h"
#include "brpc_plat.h"
#if NEXUS_HAS_MXT
#include "bmxt.h"
#include "bmxt_3383.h"
#endif

#if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
#include "baob.h"
#include "baus.h"
#include "btnr_3255ob.h"
#endif

BDBG_MODULE(nexus_frontend_3255);

BDBG_OBJECT_ID(NEXUS_3255Device);

#define sizeInLong(x)   (sizeof(x)/sizeof(uint32_t))

void b_convert_ipaddr(char *c, int b)
{
    c[0] = (b>>24)&0xff;
    c[1] = (b>>16)&0xff;
    c[2] = (b>>8)&0xff;
    c[3] =  b&0xff;
}

/***************************************************************************
Summary
DOCSIS QAM video status data type
***************************************************************************/
typedef struct NEXUS_3255QamChannelStatus
{
    NEXUS_TaskCallbackHandle statusCallback;
    BADS_Status status;
    unsigned fecCorrected;      /* accumulated FEC corrected count*/
    unsigned fecUnCorrected;    /* accumulated FEC uncorrected count*/
    unsigned totalSymbol;       /* accumulated Total symbol count*/
    unsigned unlockCount;       /* unlock count */
    unsigned acquireCount;      /* QAM acquiring total count */
    unsigned failureCount;      /* QAM acquiring failure count */
    unsigned failureFreq;       /* failure QAM frequency */
    NEXUS_Time lockTime;        /* QAM locked time*/
    bool    locked;
} NEXUS_3255QamChannelStatus;

/***************************************************************************
Summary
DOCSIS device handle
***************************************************************************/
typedef struct NEXUS_3255Device
{
    BDBG_OBJECT(NEXUS_3255Device)
    BADS_Handle ads;
#if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
    BAUS_Handle aus;
    BTNR_Handle oobTnr;
    BAOB_Handle aob;
#endif
    BRPC_Handle rpc_handle;
    NEXUS_ThreadHandle rpc_notification;
    unsigned rpc_notification_count;
    NEXUS_ThreadHandle heartbeat;
    BKNI_EventHandle heartbeatEvent;
    bool heartbeat_enabled;
    NEXUS_FrontendHandle frontendHandle[NEXUS_MAX_FRONTENDS];
    bool rpc_notification_enabled;
    bool isOOBsupported;
    unsigned numChannels;
    unsigned numDocsisChannels;
    unsigned numOfQamFrontends;
    unsigned numOfFrontends;
    unsigned oobFrontendIndex;
    NEXUS_3255DeviceSettings deviceSettings;
    NEXUS_3255ChannelCapabilities  *channelCapabilities;
    NEXUS_3255DeviceStatus deviceStatus;
    NEXUS_TaskCallbackHandle eCMStatusCallback;
    unsigned numOfTsmfParsers;
    unsigned numOfUsedTsmfParsers;
    uint32_t NonCmControlledVideoChLockStatus;
    NEXUS_StandbyMode currentPowerState;
#if NEXUS_HAS_MXT
    BMXT_Handle mxt;
    NEXUS_FrontendDeviceMtsifConfig mtsifConfig;
#endif
} NEXUS_3255Device;

/**********************************************************************************
 Summary:
 DOCSIS channel handle
 *********************************************************************************/
typedef struct NEXUS_3255Channel
{
    unsigned chn_num; /* channel number */
    unsigned frontendIndex;
    unsigned tsmfParserIndex;
    BADS_ChannelHandle ads_chn;
    BTNR_Handle tnr;
    NEXUS_CallbackHandler lockDriverCBHandler;
    NEXUS_TaskCallbackHandle lockAppCallback;
    NEXUS_TaskCallbackHandle lockDriverCallback;
    NEXUS_TaskCallbackHandle asyncStatusAppCallback;
    NEXUS_TimerHandle retuneTimer;
    NEXUS_3255QamChannelStatus ads_status;
    NEXUS_FrontendQamSettings last_ads;
    NEXUS_FrontendOutOfBandSettings last_aob;
    NEXUS_FrontendUpstreamSettings  last_aus;
    bool tune_started;
    NEXUS_3255ChannelSettings channelSettings;
    NEXUS_TsmfSettings tsmfSettings;
    NEXUS_3255DeviceHandle deviceHandle; /* 3255 device*/
} NEXUS_3255Channel;

/*************************************************************************************
Summary:
DOCSIS Channel Handle
***************************************************************************************/
typedef struct NEXUS_3255Channel *NEXUS_3255ChannelHandle;

/*********************************************************************************************
Summary:
 Private APIs that are hooked to the front end API for OOB upstream and downstream channels
**********************************************************************************************/
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
static void NEXUS_Frontend_P_3255OobChannelLockChange(void *pParam)
{
    NEXUS_TaskCallbackHandle callback = (NEXUS_TaskCallbackHandle)pParam;

    BDBG_MSG(("3255 OOB lock event change"));

    if ( callback )
    {
        NEXUS_TaskCallback_Fire(callback);
    }
}

static void NEXUS_Frontend_P_3255OobChannelUnTune(void *handle)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    channelHandle->tune_started = false;

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    NEXUS_Frontend_P_UnsetMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    NEXUS_Frontend_P_UnsetTransportConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
#endif
#endif
}

static NEXUS_Error NEXUS_Frontend_P_3255OobChannelTune(
    void *handle,
    const NEXUS_FrontendOutOfBandSettings *pSettings)
{
    BERR_Code rc;
    BAOB_ModulationType modType;
    BAOB_SpectrumMode spectrum;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT(NULL != pSettings);
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    rc = NEXUS_Frontend_P_SetMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTsmfConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
#endif
#endif

    switch (pSettings->mode)
    {
    case NEXUS_FrontendOutOfBandMode_eAnnexAQpsk:
        modType = BAOB_ModulationType_eAnnexAQpsk;
        break;
    case NEXUS_FrontendOutOfBandMode_eDvs178Qpsk:
        modType = BAOB_ModulationType_eDvs178Qpsk;
        break;
    case NEXUS_FrontendOutOfBandMode_ePod_AnnexAQpsk:
        modType = BAOB_ModulationType_ePod_AnnexAQpsk;
        break;
    case NEXUS_FrontendOutOfBandMode_ePod_Dvs178Qpsk:
        modType = BAOB_ModulationType_ePod_Dvs178Qpsk;
        break;
    default:
        return BERR_INVALID_PARAMETER;
    }

    switch (pSettings->spectrum)
    {
    case NEXUS_FrontendOutOfBandSpectrum_eAuto:
        spectrum = BAOB_SpectrumMode_eAuto;
        break;
    case NEXUS_FrontendOutOfBandSpectrum_eInverted:
        spectrum = BAOB_SpectrumMode_eInverted;
        break;
    case NEXUS_FrontendOutOfBandSpectrum_eNonInverted:
        spectrum = BAOB_SpectrumMode_eNoInverted;
        break;
    default:
        return BERR_INVALID_PARAMETER;
    }

    NEXUS_TaskCallback_Set(channelHandle->lockAppCallback, &(pSettings->lockCallback));

    if ( channelHandle->tnr)
    {
        rc = BTNR_SetTunerRfFreq(channelHandle->tnr, pSettings->frequency, BTNR_TunerMode_eDigital);
        if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}
    }

    rc = BAOB_Set_Spectrum(deviceHandle->aob, spectrum);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

    if (BAOB_Acquire(deviceHandle->aob, modType, pSettings->symbolRate)) { return BERR_TRACE(rc);}

    channelHandle->tune_started = true;
    channelHandle->last_aob = *pSettings;
    return BERR_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_Get3255OobChannelStatus(
    void *handle,
    NEXUS_FrontendOutOfBandStatus *pStatus)
{
    BERR_Code  rc;
    struct BAOB_Status st;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    rc = BAOB_GetStatus(deviceHandle->aob,  &st);
    if (rc!=BERR_SUCCESS){ return BERR_TRACE(rc);}

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
    pStatus->fdcChannelPower = st.fdcChannelPower;
    pStatus->berErrorCount = st.berErrorCount;

    pStatus->settings = channelHandle->last_aob;
    BDBG_MSG((" OOB STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t st.agcExtLevel %d "
              "\t snr_estimate %d \n\t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d"
              "\t fdcChannelPower %d",st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel,
              st.snrEstimate,st.correctedCount, st.uncorrectedCount, st.berErrorCount, st.fdcChannelPower));

    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_3255UsChannelTune(
    void *handle,
    const NEXUS_FrontendUpstreamSettings *pSettings)
{
    BERR_Code rc;
    BAUS_OperationMode mode;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;


    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT(NULL != pSettings);
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    rc = NEXUS_Frontend_P_SetMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTsmfConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
#endif
#endif

    switch (pSettings->mode)
    {
    case NEXUS_FrontendUpstreamMode_eAnnexA:
        mode = BAUS_OperationMode_eAnnexA;
        break;
    case NEXUS_FrontendUpstreamMode_eDvs178:
        mode = BAUS_OperationMode_eDvs178;
        break;
    case NEXUS_FrontendUpstreamMode_ePodAnnexA :
        mode = BAUS_OperationMode_ePodAnnexA;
        break;
    case NEXUS_FrontendUpstreamMode_ePodDvs178:
        mode = BAUS_OperationMode_ePodDvs178;
        break;
    case NEXUS_FrontendUpstreamMode_eDocsis:
        mode = BAUS_OperationMode_eDocsis;
        break;
    default:
        return BERR_NOT_SUPPORTED;
    }

    rc = BAUS_SetOperationMode(deviceHandle->aus, mode);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

    if (mode != BAUS_OperationMode_eDocsis) {
        rc = BAUS_SetRfFreq(deviceHandle->aus, pSettings->frequency);
        if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

        rc = BAUS_SetSymbolRate(deviceHandle->aus, pSettings->symbolRate);
        if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

        rc = BAUS_SetPowerLevel(deviceHandle->aus, pSettings->powerLevel);
        if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}
    }

    channelHandle->last_aus = *pSettings;
    return BERR_SUCCESS;

}
static NEXUS_Error NEXUS_Frontend_P_Get3255UsChannelStatus(
    void *handle,
    NEXUS_FrontendUpstreamStatus *pStatus)
{
    BERR_Code  rc;
    struct BAUS_Status st;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BAUS_GetStatus(deviceHandle->aus,  &st);
    if (rc!=BERR_SUCCESS){ return BERR_TRACE(rc);}

    switch (st.operationMode)
    {
        case BAUS_OperationMode_ePodAnnexA:
            pStatus->mode  = NEXUS_FrontendUpstreamMode_ePodAnnexA;
            break;
        case BAUS_OperationMode_ePodDvs178:
            pStatus->mode  = NEXUS_FrontendUpstreamMode_ePodDvs178;
            break;
        case BAUS_OperationMode_eAnnexA :
            pStatus->mode  = NEXUS_FrontendUpstreamMode_eAnnexA;
            break;
        case BAUS_OperationMode_eDvs178:
            pStatus->mode = NEXUS_FrontendUpstreamMode_eDvs178;
            break;
        case BAUS_OperationMode_eDocsis:
            pStatus->mode = NEXUS_FrontendUpstreamMode_eDocsis;
            break;
        default:
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    pStatus->frequency = st.rfFreq;
    pStatus->symbolRate = st.symbolRate;
    pStatus->powerLevel = st.powerLevel;
    pStatus->sysXtalFreq = st.sysXtalFreq;

    return NEXUS_SUCCESS;
}
#endif

/*********************************************************************************************
Summary:
 Private APIs that are hooked to the front end API for QAM video channels
**********************************************************************************************/
static void NEXUS_Frontend_P_3255QamChannelLockChange(void *pParam)
{
    NEXUS_TaskCallbackHandle callback = (NEXUS_TaskCallbackHandle)pParam;

    BDBG_MSG(("3255 lock event change"));

    if ( callback )
    {
        NEXUS_TaskCallback_Fire(callback);
    }
}

static void NEXUS_Frontend_P_3255_Reacquire(void *context)
{

    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)context;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BADS_ModulationType qam_mode;
    BERR_Code rc;
    BADS_InbandParam params;
    NEXUS_FrontendQamSettings *pSettings = &channelHandle->last_ads;
    BADS_LockStatus lockStatus = BADS_LockStatus_eUnlocked;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT(NULL != pSettings);

    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return;

    rc = BADS_GetLockStatus(channelHandle->ads_chn, &lockStatus);
    if (rc!=BERR_SUCCESS) { BERR_TRACE(rc); return; }
    if (lockStatus == BADS_LockStatus_eUnlocked)
    {
        BDBG_MSG(("%s() unlocked, reacquiring.",__FUNCTION__));
        BKNI_Memset(&params, 0, sizeof(params));

        if ( pSettings->annex == NEXUS_FrontendQamAnnex_eA )
        {
            switch ( pSettings->mode )
            {
            case NEXUS_FrontendQamMode_e16:
                qam_mode = BADS_ModulationType_eAnnexAQam16;
                break;
            case NEXUS_FrontendQamMode_e32:
                qam_mode = BADS_ModulationType_eAnnexAQam32;
                break;
            case NEXUS_FrontendQamMode_e64:
                qam_mode = BADS_ModulationType_eAnnexAQam64;
                break;
            case NEXUS_FrontendQamMode_e128:
                qam_mode = BADS_ModulationType_eAnnexAQam128;
                break;
            case NEXUS_FrontendQamMode_e256:
                qam_mode = BADS_ModulationType_eAnnexAQam256;
                break;
            default:
                return;
            }
        }
        else if ( pSettings->annex == NEXUS_FrontendQamAnnex_eB )
        {
            switch ( pSettings->mode )
            {
            case NEXUS_FrontendQamMode_e64:
                qam_mode = BADS_ModulationType_eAnnexBQam64;
                break;
            case NEXUS_FrontendQamMode_e256:
                qam_mode = BADS_ModulationType_eAnnexBQam256;
                break;
            default:
                return;
            }
        }
        else
        {
            BERR_TRACE(BERR_INVALID_PARAMETER);return;
        }

        if ( pSettings->symbolRate )
        {
            params.symbolRate = pSettings->symbolRate;
        }
        else
        {
            params.symbolRate = NEXUS_Frontend_P_GetDefaultQamSymbolRate(pSettings->mode, pSettings->annex);
        }

        params.modType = qam_mode;
        rc = BADS_Acquire(channelHandle->ads_chn, &params);
        if (rc!=BERR_SUCCESS) { BERR_TRACE(rc); return; }
        channelHandle->tune_started = true;
    }
    channelHandle->retuneTimer = NULL;

}

static void NEXUS_Frontend_P_Check3255ChannelReacquireStatus(void *context)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)context;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BERR_Code rc;
    NEXUS_FrontendQamSettings *pSettings = &channelHandle->last_ads;
    BADS_LockStatus lockStatus = BADS_LockStatus_eUnlocked;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT(NULL != pSettings);

    BDBG_MSG(("%s() calling firing lockAppCallback.",__FUNCTION__));
    NEXUS_TaskCallback_Fire(channelHandle->lockAppCallback);

    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return;

    rc = BADS_GetLockStatus(channelHandle->ads_chn, &lockStatus);
    if (rc!=BERR_SUCCESS) { BERR_TRACE(rc); return; }

    if ((lockStatus == BADS_LockStatus_eUnlocked) && pSettings->autoAcquire)
    {
        if( channelHandle->retuneTimer )
        {
            NEXUS_CancelTimer(channelHandle->retuneTimer);
        }
        BDBG_MSG(("%s() unlocked, schedule a timer to reacquire.",__FUNCTION__));
        channelHandle->retuneTimer= NEXUS_ScheduleTimer(2000, NEXUS_Frontend_P_3255_Reacquire, context);
    }
}

static NEXUS_Error NEXUS_Frontend_P_3255QamChannelTune(
    void *handle,
    const NEXUS_FrontendQamSettings *pSettings)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BADS_ModulationType qam_mode;
    BERR_Code rc;
    BADS_InbandParam params;
#if 0
    BTNR_PowerSaverSettings pwrSetting;
#endif
    unsigned chn_num = channelHandle->chn_num;
    NEXUS_CallbackDesc callbackDesc;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_MSG(("TuneQAM docsis channel %u",chn_num));
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    rc = NEXUS_Frontend_P_SetMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTsmfConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
#endif
#endif

    BKNI_Memset(&params, 0, sizeof(params));

    if ( pSettings->annex == NEXUS_FrontendQamAnnex_eA )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e16:
            qam_mode = BADS_ModulationType_eAnnexAQam16;
            break;
        case NEXUS_FrontendQamMode_e32:
            qam_mode = BADS_ModulationType_eAnnexAQam32;
            break;
        case NEXUS_FrontendQamMode_e64:
            qam_mode = BADS_ModulationType_eAnnexAQam64;
            break;
        case NEXUS_FrontendQamMode_e128:
            qam_mode = BADS_ModulationType_eAnnexAQam128;
            break;
        case NEXUS_FrontendQamMode_e256:
            qam_mode = BADS_ModulationType_eAnnexAQam256;
            break;
        default:
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else if ( pSettings->annex == NEXUS_FrontendQamAnnex_eB )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e64:
            qam_mode = BADS_ModulationType_eAnnexBQam64;
            break;
        case NEXUS_FrontendQamMode_e256:
            qam_mode = BADS_ModulationType_eAnnexBQam256;
            break;
        default:
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else if ( pSettings->annex == NEXUS_FrontendQamAnnex_eC )
    {
        switch ( pSettings->mode )
        {
        case NEXUS_FrontendQamMode_e64:
            qam_mode = BADS_ModulationType_eAnnexCQam64;
            break;
        case NEXUS_FrontendQamMode_e256:
            qam_mode = BADS_ModulationType_eAnnexCQam256;
            break;
        default:
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( pSettings->symbolRate )
    {
        params.symbolRate = pSettings->symbolRate;
    }
    else
    {
        params.symbolRate = NEXUS_Frontend_P_GetDefaultQamSymbolRate(pSettings->mode, pSettings->annex);
    }

    callbackDesc.param = chn_num;
    NEXUS_CallbackHandler_PrepareCallback(channelHandle->lockDriverCBHandler, callbackDesc);
    NEXUS_TaskCallback_Set(channelHandle->lockDriverCallback, &callbackDesc);
    NEXUS_TaskCallback_Set(channelHandle->lockAppCallback, &(pSettings->lockCallback));
    NEXUS_TaskCallback_Set(channelHandle->asyncStatusAppCallback, &(pSettings->asyncStatusReadyCallback));

    if (channelHandle->tnr)
    {
#if 0
        pwrSetting.enable = false;
        rc = BTNR_SetPowerSaver(channelHandle->tnr, &pwrSetting);
        if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}
#endif
        rc = BTNR_SetTunerRfFreq(channelHandle->tnr, pSettings->frequency, BTNR_TunerMode_eDigital);
        if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}
    }

    params.modType = qam_mode;
    rc = BADS_Acquire(channelHandle->ads_chn, &params);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

    channelHandle->tune_started = true;
    if (channelHandle->retuneTimer)
        NEXUS_CancelTimer(channelHandle->retuneTimer);
    channelHandle->retuneTimer = NULL;
    channelHandle->last_ads = *pSettings;

    return BERR_SUCCESS;
}

static void NEXUS_Frontend_P_3255QamChannelUnTune(void *handle)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
#if NEXUS_HAS_MXT
#if defined(HUMAX_PLATFORM_BASE)
#else
    NEXUS_Frontend_P_UnsetMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    NEXUS_Frontend_P_UnsetTransportConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
#endif
#endif

    channelHandle->tune_started = false;
    if (channelHandle->retuneTimer)
        NEXUS_CancelTimer(channelHandle->retuneTimer);
    channelHandle->retuneTimer = NULL;
    return;
}
#if 0
NEXUS_Error NEXUS_Frontend_3255_GetQamLockStatus(
    NEXUS_FrontendHandle frontHandle,
    bool *locked)
{
    BERR_Code rc;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)frontHandle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)channelHandle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    *locked = false;
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BADS_GetLockStatus(channelHandle->ads_chn, locked);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

    return BERR_SUCCESS;
}
#endif

static NEXUS_Error NEXUS_Frontend_P_Get3255QamChannelStatus(
    void *handle,
    NEXUS_FrontendQamStatus *pStatus)
{
    BERR_Code rc;
    BADS_Status st;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BADS_Version version;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);


    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BADS_GetStatus(channelHandle->ads_chn,  &st);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}
    rc = BADS_GetVersion(deviceHandle->ads, &version);
    if (rc) return BERR_TRACE(rc);

    pStatus->fecLock = st.isFecLock;
    pStatus->receiverLock = st.isQamLock;
    pStatus->symbolRate = st.rxSymbolRate;
    pStatus->ifAgcLevel = st.agcIntLevel;
    pStatus->rfAgcLevel = st.agcExtLevel;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;

    if (st.isQamLock == false)
    {
        st.dsChannelPower = 0;
        st.mainTap = 0;
        st.equalizerGain = 0;
        st.interleaveDepth = 0;
    }
    pStatus->dsChannelPower = (version.minVer <= 1) ? st.dsChannelPower : 0;
    pStatus->mainTap = st.mainTap;
    pStatus->equalizerGain = st.equalizerGain;
    pStatus->interleaveDepth = st.interleaveDepth;

    if (st.isFecLock == false ||st.isQamLock == false)
    {
        st.snrEstimate = 0;
        st.correctedCount = 0;
        st.uncorrectedCount = 0;
        st.berRawCount = 0;
        st.goodRsBlockCount = 0;
        st.postRsBER = 0;
        st.elapsedTimeSec = 0;
    }

    pStatus->snrEstimate = st.snrEstimate*100/256;
#if defined(HUMAX_PLATFORM_BASE)  && defined(CONFIG_CABLE_MODEM)
    pStatus->fecCorrected = st.accCorrectedCount;
    pStatus->fecUncorrected = st.accUncorrectedCount;
#else
    pStatus->fecCorrected = st.correctedCount;
    pStatus->fecUncorrected = st.uncorrectedCount;
#endif
    pStatus->berEstimate = st.berRawCount;
    pStatus->goodRsBlockCount = st.goodRsBlockCount;
    pStatus->postRsBer = st.postRsBER;
    pStatus->postRsBerElapsedTime = st.elapsedTimeSec;
    pStatus->spectrumInverted = st.isSpectrumInverted;
    pStatus->settings = channelHandle->last_ads;

    BDBG_MSG((" STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t st.agcExtLevel %d "
              "\t snr_estimate %d \t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d",
              st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel, st.snrEstimate,
              st.correctedCount, st.uncorrectedCount, st.berRawCount));

    return BERR_SUCCESS;
}


static NEXUS_Error NEXUS_Frontend_P_Get3255ChannelSoftDecisions(
    void *handle,
    NEXUS_FrontendSoftDecision *pDecisions,
    size_t length)
{
    int i;
    BERR_Code rc;
    int16_t *data_i, *data_q;
    int16_t return_length;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    if ((data_i = BKNI_Malloc(length*sizeof(int16_t))) == NULL) {
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }
    if ((data_q = BKNI_Malloc(length*sizeof(int16_t))) == NULL) {
        BKNI_Free(data_i);
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }

    rc = BADS_GetSoftDecision(channelHandle->ads_chn, (int16_t)length,  data_i, data_q, &return_length);
    if (rc!=BERR_SUCCESS) {
        BKNI_Free(data_i);
        BKNI_Free(data_q);
        return_length = length;
        return BERR_TRACE(rc);
    }

    for(i=0;i<return_length;i++) {
        pDecisions[i].i =  data_i[i]*2;
        pDecisions[i].q = data_q[i]*2;
    }

    BKNI_Free(data_i);
    BKNI_Free(data_q);

    return BERR_SUCCESS;

}

/**************************************************************************************
  Private APIs for managing a DOCSIS device
 **************************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_Init3255DeviceRPC(NEXUS_3255DeviceHandle deviceHandle)
{
    BRPC_OpenSocketImplSettings socketSettings;
    BRPC_Param_InitSession Param;
    BERR_Code errCode, retCode, retVal;
    unsigned count;

    BRPC_GetDefaultOpenSocketImplSettings(&socketSettings);
    socketSettings.timeout = deviceHandle->deviceSettings.rpcTimeout;

    if (NEXUS_GetEnv("no_3255") != NULL)
    {
        socketSettings.rpc_disabled = true;
        BDBG_WRN(("**********DOCSIS is NOT used!**********"));
    }
    errCode = BRPC_Open_SocketImpl(&deviceHandle->rpc_handle, &socketSettings);
    if ( errCode != BERR_SUCCESS ) return NEXUS_UNKNOWN;
    /* wait for 3255 to bootup and setup Rmagnum interface*/
    Param.devId = BRPC_DevId_3255;
    count = 0;
    if (NEXUS_GetEnv("no_3255") == NULL)
    {
       while (1) {
            #if BDBG_DEBUG_BUILD
            if (count%20==0)BDBG_WRN((" waiting for 3255 booting up!"));
            #endif
            retCode = BRPC_CallProc(deviceHandle->rpc_handle,
                                    BRPC_ProcId_InitSession,
                                    (const uint32_t *)&Param,
                                    sizeof(Param)/4,
                                    NULL, 0, &retVal);
            if (retCode == BERR_SUCCESS/*&& retVal == BERR_SUCCESS*/) break;
            BKNI_Sleep(10);
            if (count++ >= 1000)
            {
                BDBG_ERR((" timeout in waiting for 3255"));
                BRPC_Close_SocketImpl(deviceHandle->rpc_handle);
                return NEXUS_UNKNOWN;
            };
        }
    }
    BDBG_WRN((" RPC is initialized between 3255 and 740x"));
    BRPC_Close_SocketImpl(deviceHandle->rpc_handle);

    /* reopen RPC socket to apply normal timeout value*/
    socketSettings.timeout = 3000; /*3000ms for reliable connection*/
    errCode = BRPC_Open_SocketImpl(&deviceHandle->rpc_handle, &socketSettings);
    if ( errCode != BERR_SUCCESS ) return NEXUS_UNKNOWN;
    BDBG_WRN(("Device Handle %x, RPC handle %x", deviceHandle, deviceHandle->rpc_handle));
    return NEXUS_SUCCESS;
}

static NEXUS_3255ChannelHandle NEXUS_Frontend_P_Get3255ChannelHandle(
    NEXUS_3255DeviceHandle deviceHandle,
    unsigned chn_num)
{
    unsigned i=0;
    bool foundChannelHandle=false;
    NEXUS_3255ChannelHandle channelHandle;

    BDBG_MSG(("deviceHandle->numOfQamFrontends %u deviceHandle %x",i,deviceHandle));
    for(i=0;i< deviceHandle->numOfQamFrontends && deviceHandle->frontendHandle[i];i++)
    {
        channelHandle = deviceHandle->frontendHandle[i]->pDeviceHandle;
        if(channelHandle->chn_num == chn_num)
        {
            BDBG_MSG(("found channel handle %u",chn_num));
            foundChannelHandle = true;
            break;
        }
    }

    if(foundChannelHandle)
    {
        return channelHandle;
    }
    else
    {
        return NULL;
    }
}

static void NEXUS_Frontend_P_Process3255DeviceRpcNotification(
    uint32_t device_id,
    uint32_t event,
    void * arg)
{
    unsigned id;
    unsigned rpc_event;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)arg;
    NEXUS_3255ChannelHandle channelHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_MSG(("NEXUS_Frontend_P_3255DeviceProcessNotification deviceId %x event %u deviceHandle %x",device_id,event,deviceHandle));
    deviceHandle->rpc_notification_count++;

    switch (device_id)
    {
        case BRPC_DevId_3255_DS0:
        case BRPC_DevId_3255_DS1:
        case BRPC_DevId_3255_DS2:
        case BRPC_DevId_3255_DS3:
        case BRPC_DevId_3255_DS4:
        case BRPC_DevId_3255_DS5:
        case BRPC_DevId_3255_DS6:
        case BRPC_DevId_3255_DS7:
            id = device_id - BRPC_DevId_3255_DS0;
            rpc_event = BRPC_GET_NOTIFICATION_EVENT(event);
            BDBG_MSG(("channel id %u",id));
            channelHandle = NEXUS_Frontend_P_Get3255ChannelHandle(deviceHandle,id);
            if (channelHandle)
            {
                if (rpc_event == BRPC_Notification_Event_DsChannelPower)
                {
                    BDBG_MSG((" got DS power notification"));
                    channelHandle->ads_status.status.dsChannelPower = BRPC_GET_DS_POWER(event);
                    if (channelHandle->ads_status.statusCallback)
                        NEXUS_TaskCallback_Fire(channelHandle->ads_status.statusCallback);
                    if (channelHandle->asyncStatusAppCallback)
                        NEXUS_TaskCallback_Fire(channelHandle->asyncStatusAppCallback);
                }
                else
                {
                    if (channelHandle->tune_started == true) /* only handle notification when tuner is active*/
                    {
                        BADS_ProcessNotification(channelHandle->ads_chn, event);
                    }
                }
            }
            break;
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    case BRPC_DevId_3255_OB0:
            if(deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex])
            {
                channelHandle = deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex]->pDeviceHandle;
                if (deviceHandle->aob != NULL && channelHandle->tune_started)
                    BAOB_ProcessNotification(deviceHandle->aob, event);
            }
            break;
#endif
    case BRPC_DevId_3255:
            rpc_event = BRPC_GET_NOTIFICATION_EVENT(event);
            if (rpc_event == BRPC_Notification_Event_EcmReset) {
                BDBG_WRN((" got eCM reset notification"));
                deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eReset;
                NEXUS_TaskCallback_Fire(deviceHandle->eCMStatusCallback);
            }
            else if (rpc_event == BRPC_Notification_Event_EcmOperational) {
                  /* The ECM opertaional notification is sent after Docsis is registered*/
                  /* not very useful for Rmagnum bring up*/
                BDBG_MSG((" got eCM registeration notification"));
            }
            else if (rpc_event == BRPC_Notification_Event_EcmRmagnumReady)
            {
                BDBG_WRN((" got eCM Rmagnum ready  notification"));
                BKNI_SetEvent(deviceHandle->heartbeatEvent);
            }
            break;
        case BRPC_DevId_3255_US0:
        case BRPC_DevId_3255_TNR0:
        case BRPC_DevId_3255_TNR1:
        case BRPC_DevId_3255_TNR2:
        case BRPC_DevId_3255_TNR3:
        case BRPC_DevId_3255_TNR4:
        case BRPC_DevId_3255_TNR5:
        case BRPC_DevId_3255_TNR6:
        case BRPC_DevId_3255_TNR7:
        default:
            BDBG_WRN((" unknown notification from 3255 device %d", device_id));
    }
}

static void NEXUS_Frontend_3255DeviceRpcNotificationThread(void *arg)
{
    uint32_t device_id, event;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)arg;
    /*TODO:: use infinite select() instead of polling*/
    while(deviceHandle->rpc_notification_enabled) {
        BRPC_CheckNotification(deviceHandle->rpc_handle,  &device_id, &event, 100);
        if (BRPC_GET_NOTIFICATION_EVENT(event)) {
            BDBG_MSG(("check_rpc(): notified by server (device_id = %08x) event is %x\n", device_id, event));
            NEXUS_LockModule();
            NEXUS_Frontend_P_Process3255DeviceRpcNotification(device_id, event, arg);
            NEXUS_UnlockModule();
        }
    }
}


static NEXUS_Error NEXUS_Frontend_P_Open3255Channels(NEXUS_3255DeviceHandle deviceHandle)
{
    BERR_Code rc;
    NEXUS_3255ChannelHandle channelHandle;
    unsigned int i;
    BADS_Settings ads_cfg;
    BADS_ChannelSettings chn_cfg;
    BTNR_3255Ib_Settings tnr3255_cfg;
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    BTNR_3255Ob_Settings tnrOb3255_cfg;
    BAOB_Settings aob_cfg;
    BAUS_Settings aus_cfg;
#endif

    rc = BADS_3255_GetDefaultSettings( &ads_cfg, NULL);
    if ( rc != BERR_SUCCESS ) goto err_init;

    ads_cfg.hGeneric = deviceHandle->rpc_handle;
    rc = BADS_Open(&deviceHandle->ads, NULL, NULL, NULL, &ads_cfg);
    if ( rc != BERR_SUCCESS ) goto err_init;

    rc = BADS_Init(deviceHandle->ads);
    if ( rc != BERR_SUCCESS ) goto err_init;

    for (i=0;i<deviceHandle->numOfQamFrontends;i++)
    {
        NEXUS_3255ChannelHandle channelHandle = deviceHandle->frontendHandle[i]->pDeviceHandle;
        rc = BTNR_3255Ib_GetDefaultSettings(&tnr3255_cfg, NULL);
        if (rc != BERR_SUCCESS) goto err_init;
        tnr3255_cfg.hGeneric = deviceHandle->rpc_handle;
        tnr3255_cfg.ifFreq = BTNR_3255Ib_SETTINGS_IFFREQ;
        tnr3255_cfg.devId += channelHandle->chn_num;
        rc =  BTNR_3255Ib_Open(&channelHandle->tnr, NULL, NULL, NULL, &tnr3255_cfg);
        if (rc != BERR_SUCCESS) goto err_init;
    }

    /* Configure ADS channels */
    for (i=0;i<deviceHandle->numOfQamFrontends;i++)
    {
        channelHandle = deviceHandle->frontendHandle[i]->pDeviceHandle;
        rc = BADS_GetChannelDefaultSettings( deviceHandle->ads, i, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto err_init;
        chn_cfg.autoAcquire = deviceHandle->deviceSettings.autoAcquire;
        chn_cfg.fastAcquire = deviceHandle->deviceSettings.fastAcquire;
        rc = BADS_OpenChannel( deviceHandle->ads, &channelHandle->ads_chn, channelHandle->chn_num, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto err_init;
    }

#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
                channelHandle = deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex]->pDeviceHandle;
    rc = BTNR_3255Ob_GetDefaultSettings(&tnrOb3255_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto err_init;
    tnrOb3255_cfg.hGeneric = deviceHandle->rpc_handle;

/* BTNR_3255OB_SETTINGS_IFFREQ is deprecated (incorrectly assigns IF_FREQ based on host chip)
   use NEXUS_3255_OOB_TUNER_IFFREQ instead (to be defined in nexus_platform_features.h) */
#ifndef NEXUS_3255_OOB_TUNER_IFFREQ
#define NEXUS_3255_OOB_TUNER_IFFREQ BTNR_3255OB_SETTINGS_IFFREQ
#endif

    tnrOb3255_cfg.ifFreq = NEXUS_3255_OOB_TUNER_IFFREQ;
    rc = BTNR_3255Ob_Open(&channelHandle->tnr, NULL, &tnrOb3255_cfg);
    if (rc!=BERR_SUCCESS) goto err_init;

    rc = BAOB_GetDefaultSettings( &aob_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto err_init;
    aob_cfg.hGeneric = deviceHandle->rpc_handle;
    aob_cfg.enableFEC = deviceHandle->deviceSettings.enableFEC;
    rc = BAOB_Open(&deviceHandle->aob, NULL, NULL, NULL, &aob_cfg);
    if (rc!=BERR_SUCCESS) goto err_init;

    rc = BAUS_GetDefaultSettings(&aus_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto err_init;
    aus_cfg.xtalFreq = BAUS_SETTINGS_XTALFREQ;
    aus_cfg.hGeneric = deviceHandle->rpc_handle;;
    rc = BAUS_Open( &deviceHandle->aus, NULL, NULL, &aus_cfg);
    if (rc!=BERR_SUCCESS) goto err_init;

    deviceHandle->isOOBsupported = true;
#else
    deviceHandle->isOOBsupported = false;
#endif
                deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eOperational;
    return BERR_SUCCESS;

err_init:
    for ( i = 0; i < deviceHandle->numOfQamFrontends && NULL != deviceHandle->frontendHandle[i]; i++)
    {
        channelHandle= deviceHandle->frontendHandle[i]->pDeviceHandle;
        BTNR_Close(channelHandle->tnr);
    }
    for ( i = 0; i < deviceHandle->numOfQamFrontends && NULL != deviceHandle->frontendHandle[i]; i++)
    {
        channelHandle= deviceHandle->frontendHandle[i]->pDeviceHandle;
        BADS_CloseChannel(channelHandle->ads_chn);
    }
    if (deviceHandle->ads)
    {
        BADS_Close(deviceHandle->ads);
    }
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    channelHandle = deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex]->pDeviceHandle;
    if (channelHandle->tnr)
      BTNR_Close(channelHandle->tnr);
    if (deviceHandle->aus)
    {
        BAUS_Close(deviceHandle->aus);
    }
    if (deviceHandle->aob)
    {
        BAOB_Close(deviceHandle->aob);
    }
#endif
    deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eUninitialized;
    return rc;
}

static void NEXUS_Frontend_P_Close3255Channels(NEXUS_3255DeviceHandle deviceHandle)
{
    unsigned i;
    NEXUS_3255ChannelHandle channelHandle;
    BDBG_MSG(("NEXUS_Frontend_P_Close3255Channels >>"));
    /* all channels are closed, close everthing*/
    for ( i = 0; i < deviceHandle->numOfQamFrontends && deviceHandle->frontendHandle[i]; i++)
    {
        channelHandle = deviceHandle->frontendHandle[i]->pDeviceHandle;
        if(channelHandle->tnr)
        {
            BTNR_Close(channelHandle->tnr);
        }
        channelHandle->tnr = NULL;
    }

    for ( i = 0; i < deviceHandle->numOfQamFrontends && deviceHandle->frontendHandle[i]; i++)
    {
        channelHandle = deviceHandle->frontendHandle[i]->pDeviceHandle;
        if(channelHandle->ads_chn)
        {
            BADS_CloseChannel(channelHandle->ads_chn);
        }
        channelHandle->ads_chn = NULL;
    }

    if (deviceHandle->ads)
    {
        BADS_Close(deviceHandle->ads);
        deviceHandle->ads = NULL;
    }
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    if(deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex])
    {
        channelHandle = deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex]->pDeviceHandle;
        if (channelHandle->tnr)
        {
            BTNR_Close(channelHandle->tnr);
            channelHandle->tnr = NULL;
        }
        if (deviceHandle->aus)
        {
            BAUS_Close(deviceHandle->aus);
            deviceHandle->aus = NULL;
        }
        if (deviceHandle->aob)
        {
            BAOB_Close(deviceHandle->aob);
            deviceHandle->aob = NULL;
        }
    }
#endif

    deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eUninitialized;
    BDBG_MSG(("NEXUS_Frontend_P_Close3255Channels <<"));
    return;
}

static void NEXUS_Frontend_P_3255DeviceHeartbeatThread(void * arg)
{
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)arg;
    BERR_Code retVal, retCode;
    NEXUS_Error errCode;
    BRPC_Param_ADS_GetVersion outVerParam;
    BRPC_Param_XXX_Get Param;
    bool need_restart;
    unsigned i;
    int count;

    Param.devId = BRPC_DevId_3255;

    deviceHandle->rpc_notification_count = 0; /* clear notification count*/

    while(deviceHandle->heartbeat_enabled == true)
    {
        /* BDBG_MSG((" check eCM's heartbeat every 2 seconds")); */
        need_restart = true;

        /* to avoid false alarm that eCM is dead or reset:
           (1) use double RPC calls to verify eCM status
           (2) eCM notification also indicates eCM is alive
        */
        if (deviceHandle->deviceStatus == NEXUS_3255DeviceStatus_eOperational)
        {
            retCode = BRPC_CallProc(deviceHandle->rpc_handle,
                                    BRPC_ProcId_ADS_GetVersion,
                                    (const uint32_t *)&Param,
                                     sizeof(Param)/4,
                                    (uint32_t *)&outVerParam,
                                    sizeInLong(outVerParam), &retVal);
            need_restart = (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS) && (!deviceHandle->rpc_notification_count);
            if (need_restart == true && deviceHandle->deviceStatus == NEXUS_3255DeviceStatus_eOperational) /* do one more test */
            {
                BKNI_Sleep(100);
                retCode = BRPC_CallProc(deviceHandle->rpc_handle,
                                        BRPC_ProcId_ADS_GetVersion,
                                        (const uint32_t *)&Param,
                                        sizeof(Param)/4,
                                        (uint32_t *)&outVerParam,
                                        sizeInLong(outVerParam),
                                        &retVal);
                need_restart = (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS)&&(!deviceHandle->rpc_notification_count);
                if (need_restart) deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eReset;
            }
        }
        while (need_restart == true && deviceHandle->heartbeat_enabled == true)
        {
            BDBG_ERR((" Delay 10 seconds and start polling eCM "));
            retCode = BKNI_WaitForEvent(deviceHandle->heartbeatEvent, 10000);
            if (deviceHandle->heartbeat_enabled == false) break;
            /*  delay max 10 seconds or wait for EcmRmagnumReady event then start polling eCM */
            BKNI_ResetEvent(deviceHandle->heartbeatEvent);
            count = 0;
            if (NEXUS_GetEnv("no_3255") == NULL)
            {
                while (deviceHandle->heartbeat_enabled == true) {
                    BDBG_MSG((" waiting for eCM booting up!"));
                    retCode = BRPC_CallProc(deviceHandle->rpc_handle,
                                            BRPC_ProcId_InitSession,
                                            (const uint32_t *)&Param,
                                            sizeof(Param)/4,
                                            NULL, 0, &retVal);
                    need_restart = (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS);
                    if (need_restart == false ||  ++count >= 5) break;
                    BKNI_Sleep(100);
                }
            }
            if (need_restart == true)
            {
                BDBG_ERR((" eCM is dead! "));
                NEXUS_LockModule();
                deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eFailed;
                /* ask application to manually reset eCM*/
                NEXUS_TaskCallback_Fire(deviceHandle->eCMStatusCallback);
                NEXUS_UnlockModule();
            }
            else
            {
                BDBG_ERR((" eCM's heartbeat is back. Re-initialize ADS/AOB/AUS "));
                NEXUS_LockModule();
                NEXUS_Frontend_P_Close3255Channels(deviceHandle);
                /* coverity[freed_arg] */
                errCode = NEXUS_Frontend_P_Open3255Channels(deviceHandle);
                if (errCode)
                {
                    BDBG_ERR((" Error in Re-initialize ADS/AOB/AUS, what to do? "));
                    NEXUS_UnlockModule();
                    continue;
                }
                else
                {
                    for ( i = 0; i < deviceHandle->numOfQamFrontends && NULL!=deviceHandle->frontendHandle[i]; i++)
                    {
                        NEXUS_3255ChannelHandle channelHandle=deviceHandle->frontendHandle[i]->pDeviceHandle;
                        BADS_InstallCallback(channelHandle->ads_chn, BADS_Callback_eLockChange,
                            (BADS_CallbackFunc)NEXUS_Frontend_P_3255QamChannelLockChange, (void*)channelHandle->lockDriverCallback);
                    }
                    #if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
                    if (deviceHandle->aob)
                    {
                        NEXUS_3255ChannelHandle channelHandle=deviceHandle->frontendHandle[deviceHandle->oobFrontendIndex]->pDeviceHandle;
                        BAOB_InstallCallback(deviceHandle->aob, BAOB_Callback_eLockChange,
                            (BAOB_CallbackFunc)NEXUS_Frontend_P_3255OobChannelLockChange, (void*)channelHandle->lockAppCallback);
                    }
                    #endif
                }
                NEXUS_TaskCallback_Fire(deviceHandle->eCMStatusCallback);
                NEXUS_UnlockModule();
            }
        }
        /* BDBG_MSG((" Done checking eCM heartbeat ")); */
next_try:
        NEXUS_LockModule();
        deviceHandle->rpc_notification_count = 0; /* clear notification count*/
        NEXUS_UnlockModule();
        retCode = BKNI_WaitForEvent(deviceHandle->heartbeatEvent, 2000);
        if (deviceHandle->heartbeat_enabled == false) break;
        if (deviceHandle->rpc_notification_count) {
            BDBG_MSG((" Got %d RPC notification, no need to poll",deviceHandle->rpc_notification_count ));
            goto next_try;
        }
    }

}

/**********************************************************************************
 Summary:
 Private API for closing a frontend handle. It's hooked up to the main
 close funtion pointer in the frontend handle during frontend open.
 **********************************************************************************/
static void NEXUS_Frontend_P_Close3255Channel(NEXUS_FrontendHandle handle)
{
    NEXUS_3255DeviceHandle deviceHandle;
    NEXUS_3255ChannelHandle channelHandle;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    channelHandle =(NEXUS_3255ChannelHandle) handle->pDeviceHandle;
    deviceHandle = channelHandle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);

    /* uninstall lock change callback */
    if (channelHandle->ads_chn)
    {
        BERR_Code rc;
        rc = BADS_InstallCallback(channelHandle->ads_chn, BADS_Callback_eLockChange, NULL, NULL);
        if (rc) {rc = BERR_TRACE(rc);}
    }
    else
    {
        #if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
        if (deviceHandle->aob)
        {
        BERR_Code rc;
            rc = BAOB_InstallCallback(deviceHandle->aob, BAOB_Callback_eLockChange, NULL, NULL);
            if (rc) { rc = BERR_TRACE(rc);}
        }
        #endif
    }

    if (channelHandle->retuneTimer)
    {
            /* in case channel tuned with autoAcquire wasn't untuned prior to close. */
         NEXUS_CancelTimer(channelHandle->retuneTimer);
         channelHandle->retuneTimer = NULL;
    }

    if(channelHandle->lockDriverCallback)
    {
        NEXUS_CallbackHandler_Shutdown(channelHandle->lockDriverCBHandler);
    }
    if ( NULL != channelHandle->lockDriverCallback)
    {
        NEXUS_TaskCallback_Destroy(channelHandle->lockDriverCallback);
        channelHandle->lockDriverCallback = NULL;
    }
    if ( NULL != channelHandle->lockAppCallback)
    {
        NEXUS_TaskCallback_Destroy(channelHandle->lockAppCallback);
        channelHandle->lockAppCallback = NULL;
    }
    if ( NULL != channelHandle->asyncStatusAppCallback)
    {
        NEXUS_TaskCallback_Destroy(channelHandle->asyncStatusAppCallback);
        channelHandle->asyncStatusAppCallback = NULL;
    }

    if(NULL != channelHandle->ads_status.statusCallback)
    {
        NEXUS_TaskCallback_Destroy(channelHandle->ads_status.statusCallback);
        channelHandle->ads_status.statusCallback = NULL;
    }

    NEXUS_Frontend_P_Destroy(handle);
    deviceHandle->frontendHandle[channelHandle->frontendIndex] = NULL;
    if(channelHandle->tnr)
    {
        BTNR_Close(channelHandle->tnr);
        channelHandle->tnr = NULL;
    }

    if(channelHandle->ads_chn)
    {
        BADS_CloseChannel(channelHandle->ads_chn);
        channelHandle->ads_chn = NULL;
    }
    BKNI_Free(channelHandle);
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3255GetTemperature(void *handle, NEXUS_FrontendTemperature *pTemperature)
{
    NEXUS_Error ret = NEXUS_SUCCESS;

    BERR_Code retCode = BERR_SUCCESS;
    BERR_Code retVal;
    BRPC_Param_ECM_ReadDieTemperature outParam;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BDBG_ASSERT(deviceHandle);

    retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_ECM_ReadDieTemperature,
        NULL, 0, (uint32_t *)&outParam, sizeInLong(outParam), &retVal);

    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" Unable to get the 3383 Die Temperature via RPC to 3383"));
        return NEXUS_INVALID_PARAMETER;
    }

    pTemperature->temperature  = outParam.TempInDot00DegC;

    return ret;
}

static NEXUS_Error NEXUS_Frontend_P_3255Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)channelHandle->deviceHandle;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_ECM_PowerSaver Param;
    BERR_Code retVal;
    BSTD_UNUSED(enabled);

    BDBG_WRN(("Channel Handle %x ,Device Handle %x, RPC handle %x", channelHandle, deviceHandle, deviceHandle->rpc_handle));

    BDBG_ASSERT( deviceHandle->rpc_handle);

    if (deviceHandle->currentPowerState == pSettings->mode)
    {
        BDBG_ERR(("3255 Device already in standby mode: %d \n", pSettings->mode));
    	return NEXUS_SUCCESS;
    }

    Param.devId = BRPC_DevId_3255;

    if ((pSettings->mode == NEXUS_StandbyMode_ePassive) || (pSettings->mode == NEXUS_StandbyMode_eDeepSleep))
    	Param.mode = BRPC_ECM_PowerMode_Standby1;
    else if ((pSettings->mode == NEXUS_StandbyMode_eActive) || (pSettings->mode == NEXUS_StandbyMode_eOn))
    	Param.mode = BRPC_ECM_PowerMode_On;
    else
    {
        BDBG_ERR((" Unsupported standby mode"));
        return NEXUS_INVALID_PARAMETER;
    }

    retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_ECM_PowerSaver,
			(const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
    	BDBG_ERR((" Unable to transistion to Standby mode "));
        return NEXUS_INVALID_PARAMETER;
    }

    deviceHandle->currentPowerState = pSettings->mode;

    return NEXUS_SUCCESS;
}

/**********************************************************************************
 Summary:
 Private API for getting QAM status aynchronously. It's hooked up to the main
 getQamAsyncStatus funtion pointer in the frontend handle during frontend open.
 **********************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_3255_GetAsyncQamStatus(
    void *handle,
    NEXUS_FrontendQamStatus *pStatus)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BADS_Status *pSt;


    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    pSt = &channelHandle->ads_status.status;
    pStatus->fecLock = pSt->isFecLock;
    pStatus->receiverLock = pSt->isQamLock;
    pStatus->symbolRate = pSt->rxSymbolRate;
    pStatus->ifAgcLevel = pSt->agcIntLevel;
    pStatus->rfAgcLevel = pSt->agcExtLevel;
    pStatus->carrierFreqOffset = pSt->carrierFreqOffset;
    pStatus->carrierPhaseOffset = pSt->carrierPhaseOffset;

    if (pSt->isQamLock == false)
    {
        pSt->dsChannelPower = 0;
        pSt->mainTap = 0;
        pSt->equalizerGain = 0;
        pSt->interleaveDepth = 0;
    }
    pStatus->dsChannelPower = pSt->dsChannelPower;
    pStatus->mainTap = pSt->mainTap;
    pStatus->equalizerGain = pSt->equalizerGain;
    pStatus->interleaveDepth = pSt->interleaveDepth;

    if (pSt->isFecLock == false ||pSt->isQamLock == false)
    {
        pSt->snrEstimate = 0;
        pSt->correctedCount = 0;
        pSt->uncorrectedCount = 0;
        pSt->berRawCount = 0;
        pSt->goodRsBlockCount = 0;
        pSt->postRsBER = 0;
        pSt->elapsedTimeSec = 0;
    }

    pStatus->snrEstimate = pSt->snrEstimate*100/256;
    pStatus->fecCorrected = pSt->correctedCount;
    pStatus->fecUncorrected = pSt->uncorrectedCount;
    pStatus->berEstimate = pSt->berRawCount;
    pStatus->goodRsBlockCount = pSt->goodRsBlockCount;
    pStatus->postRsBer = pSt->postRsBER;
    pStatus->postRsBerElapsedTime = pSt->elapsedTimeSec;
    pStatus->spectrumInverted = pSt->isSpectrumInverted;
    pStatus->settings = channelHandle->last_ads;

    BDBG_MSG((" STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t agcExtLevel %d \t snr_estimate %d "
              "\t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d",pStatus->fecLock, pStatus->receiverLock,
               pStatus->ifAgcLevel, pStatus->rfAgcLevel, pStatus->snrEstimate,pStatus->fecCorrected,
               pStatus->fecUncorrected, pStatus->berEstimate));
    BDBG_MSG((" STATUS DS: ds power %d in 1/10 dBmV", pStatus->dsChannelPower));
    return NEXUS_SUCCESS;

}

/**********************************************************************************
 Summary:
 Private API for requesting QAM status aynchronously. It's hooked up to the main
 requestQamAsyncStatus funtion pointer in the frontend handle during frontend open.
 **********************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_3255_RequestAsyncQamStatus(
    void *handle)
{
    BERR_Code rc;
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    unsigned chn_num = channelHandle->chn_num;
    BADS_Version version;
    NEXUS_3255QamChannelStatus *qamChannelStatus;
    BRPC_Param_ADS_GetDsChannelPower param;
    BERR_Code retCode = BERR_SUCCESS;
    BERR_Code retVal;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);

    BDBG_MSG((" start NEXUS_Frontend_3255_GetQamStatus"));
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    qamChannelStatus = &channelHandle->ads_status;

    rc = BADS_GetStatus(channelHandle->ads_chn,  &qamChannelStatus->status);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc);}

    BDBG_MSG((" STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t st.agcExtLevel %d \t "
              "snr_estimate %d \t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d",
              qamChannelStatus->status.isFecLock,qamChannelStatus->status.isQamLock,
              qamChannelStatus->status.agcIntLevel,qamChannelStatus->status.agcExtLevel,
              qamChannelStatus->status.snrEstimate,qamChannelStatus->status.correctedCount,
              qamChannelStatus->status.uncorrectedCount,qamChannelStatus->status.berRawCount));
    BDBG_MSG((" end NEXUS_Frontend_3255_GetQamStatus"));

    rc = BADS_GetVersion(deviceHandle->ads, &version);
    if (rc) return BERR_TRACE(rc);

    BDBG_ASSERT( deviceHandle->rpc_handle);
    param.devId = BRPC_DevId_3255_DS0 + chn_num;
    retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_ADS_GetDsChannelPower,
                            (const uint32_t *)&param, sizeInLong(param), NULL, 0, &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_GetDSPower Fail"));
        return NEXUS_NOT_SUPPORTED;
    }
    return NEXUS_SUCCESS;
}

/*****************************************************************************************
 Summary:
 Private APIs for mapping the parser bands of a DOCSIS channel.
 (Applicable for DOCSIS devices that are connected to host transport using MTSIF interface)
 ****************************************************************************************/
NEXUS_Error NEXUS_Frontend_P_3255_ReapplyTransportSettings(void *handle)
{
#if NEXUS_HAS_MXT
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    NEXUS_Error rc;

    rc = NEXUS_Frontend_P_SetMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
    rc = NEXUS_Frontend_P_SetTransportConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
#else
    rc = NEXUS_Frontend_P_SetTsmfConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
#endif
#else
    BSTD_UNUSED(handle);
#endif
    return NEXUS_SUCCESS;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
extern void NEXUS_Frontend_P_DumpMtsifConfig(NEXUS_FrontendHandle frontend, const NEXUS_FrontendDeviceMtsifConfig *pConfig);

NEXUS_Error NEXUS_Frontend_P_3255_GetPathinfo(void *handle)
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    NEXUS_Frontend_P_DumpMtsifConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    return NEXUS_SUCCESS;
}
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
NEXUS_Error NEXUS_Frontend_P_3255_ReapplyTSMFSettings(void *handle)
{
#if NEXUS_HAS_MXT
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    NEXUS_Error rc;

    rc = NEXUS_Frontend_P_SetTsmfConfig(deviceHandle->frontendHandle[channelHandle->frontendIndex], &deviceHandle->mtsifConfig);
    if (rc) { return BERR_TRACE(rc); }
#else
    BSTD_UNUSED(handle);
#endif
    return NEXUS_SUCCESS;
}
#endif

void NEXUS_Frontend_GetDefault3255DeviceSettings(
    NEXUS_3255DeviceSettings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->autoAcquire = false;
    pSettings->fastAcquire = false;
    pSettings->enableFEC = false; /* CableCARD will do FEC for OOB*/
    pSettings->rpcTimeout = 50;
    return;
}


NEXUS_3255DeviceHandle NEXUS_Frontend_Open3255Device(
    unsigned index,
    const NEXUS_3255DeviceSettings *pSettings
    )
{
    BERR_Code errCode;
    unsigned i, num_ch, bonded_ch;
    BADS_Settings ads_cfg;
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    BTNR_3255Ob_Settings tnrOb3255_cfg;
    BAOB_Settings aob_cfg;
    BAUS_Settings aus_cfg;
#endif
    NEXUS_ThreadSettings thread_settings;
    NEXUS_3255DeviceHandle deviceHandle=NULL;
    BDBG_MSG(("NEXUS_Frontend_Open3255Device >>>"));
    /*
     * Assuming that there is only one docsis device
     */
    BSTD_UNUSED(index);
    deviceHandle = BKNI_Malloc(sizeof(NEXUS_3255Device));
    if ( NULL == deviceHandle )
    {
          errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc;
        }
    BKNI_Memset(deviceHandle, 0, sizeof(NEXUS_3255Device));
    BDBG_OBJECT_SET(deviceHandle, NEXUS_3255Device);

    deviceHandle->deviceSettings = *pSettings;

    if (NEXUS_Frontend_P_Init3255DeviceRPC(deviceHandle) ) goto err_rpc;
    BDBG_WRN(("Device Handle %x, RPC handle %x", deviceHandle, deviceHandle->rpc_handle));
    errCode = BADS_3255_GetDefaultSettings( &ads_cfg, NULL);
    if ( errCode != BERR_SUCCESS ) goto err_init;

    ads_cfg.hGeneric = deviceHandle->rpc_handle;
    errCode = BADS_Open(&deviceHandle->ads, NULL, NULL, NULL, &ads_cfg);
    if ( errCode != BERR_SUCCESS ) goto err_init;

    errCode = BADS_Init(deviceHandle->ads);
    if ( errCode != BERR_SUCCESS ) goto err_init;

    /* get total ADS channel number*/
    errCode = BADS_GetTotalChannels(deviceHandle->ads, &num_ch);
    if (errCode!=BERR_SUCCESS) goto err_init;
    deviceHandle->numChannels = num_ch;
    BDBG_MSG(("docsis numChannels %u",num_ch));

#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    deviceHandle->channelCapabilities = BKNI_Malloc(sizeof(NEXUS_3255ChannelCapabilities)*(num_ch+1));
#else
    deviceHandle->channelCapabilities = BKNI_Malloc(sizeof(NEXUS_3255ChannelCapabilities)*num_ch);
#endif
    if(!deviceHandle->channelCapabilities) goto err_init;

    /* get number of bonded channels */
    errCode = BADS_GetBondingCapability(deviceHandle->ads, &bonded_ch);
    if (errCode!=BERR_SUCCESS) goto err_init;
    if(bonded_ch==num_ch)
    {
        BDBG_WRN(("DOCSIS operating in legacy mode. So using all the DOCSIS channels for data"));
    }
    else
    {
        BDBG_MSG(("docsis bonded channels 0-%u",bonded_ch-1));
    }
    deviceHandle->numDocsisChannels = bonded_ch;
    for(i=0;i<num_ch;i++)
    {
        if(i<bonded_ch)
        {
            deviceHandle->channelCapabilities[i].channelType = NEXUS_3255ChannelType_eDocsis;
        }
        else
        {
            deviceHandle->channelCapabilities[i].channelType = NEXUS_3255ChannelType_eInBand;
        }
    }

    /* Initialize non-Docsis video channel lock status */
    deviceHandle->NonCmControlledVideoChLockStatus = 0;

    /* Set the Initial Power state of the Docsis frontend device to "On" */
    deviceHandle->currentPowerState = NEXUS_StandbyMode_eOn;

#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    errCode = BTNR_3255Ob_GetDefaultSettings(&tnrOb3255_cfg, NULL);
    if (errCode!=BERR_SUCCESS) goto oob_done;
    tnrOb3255_cfg.hGeneric = deviceHandle->rpc_handle;

    /* BTNR_3255OB_SETTINGS_IFFREQ is deprecated (incorrectly assigns IF_FREQ based on host chip)
      use NEXUS_3255_OOB_TUNER_IFFREQ instead (to be defined in nexus_platform_features.h) */
#ifndef NEXUS_3255_OOB_TUNER_IFFREQ
    #define NEXUS_3255_OOB_TUNER_IFFREQ BTNR_3255OB_SETTINGS_IFFREQ
#endif
    tnrOb3255_cfg.ifFreq = NEXUS_3255_OOB_TUNER_IFFREQ;
    errCode = BTNR_3255Ob_Open(&deviceHandle->oobTnr, NULL, &tnrOb3255_cfg);
    if (errCode!=BERR_SUCCESS) goto oob_done;

    errCode = BAOB_GetDefaultSettings( &aob_cfg, NULL);
    if (errCode!=BERR_SUCCESS) goto oob_done;
    aob_cfg.hGeneric = deviceHandle->rpc_handle;
    aob_cfg.enableFEC = deviceHandle->deviceSettings.enableFEC;
    errCode = BAOB_Open(&deviceHandle->aob, NULL, NULL, NULL, &aob_cfg);
    if (errCode!=BERR_SUCCESS)
     {
        BDBG_ASSERT(0);
        goto oob_done;
    }

    errCode = BAUS_GetDefaultSettings(&aus_cfg, NULL);
    if (errCode!=BERR_SUCCESS) goto oob_done;
    aus_cfg.xtalFreq = BAUS_SETTINGS_XTALFREQ;
    aus_cfg.hGeneric = deviceHandle->rpc_handle;;
    errCode = BAUS_Open( &deviceHandle->aus, NULL, NULL, &aus_cfg);
    if (errCode!=BERR_SUCCESS) goto oob_done;

    deviceHandle->isOOBsupported = true;
    deviceHandle->channelCapabilities[num_ch].channelType = NEXUS_3255ChannelType_eOutOfBand;
oob_done:
#else
    deviceHandle->isOOBsupported = false;
#endif

#if NEXUS_HAS_MXT
    {
        /* open MXT */
        BMXT_Settings mxtSettings;
        BADS_Version version;
        uint16_t chipId;
        uint8_t chipRev;

        BMXT_3383_GetDefaultSettings(&mxtSettings);
        for (i=0; i<BMXT_NUM_MTSIF; i++) {
            mxtSettings.MtsifTxCfg[i].Enable = pSettings->mtsif;
        }
        mxtSettings.hRpc = deviceHandle->rpc_handle;

        /* Retrieve Chip Id and Rev */
        errCode = BADS_GetVersion(deviceHandle->ads, &version);
        if (errCode) goto err_rpc;

        chipId = (uint16_t)(version.majVer >> 16);

        if (chipId == 0x3383)
            mxtSettings.chip = BMXT_Chip_e3383;
        else
            goto err_rpc;

        chipRev = (uint8_t)(version.majVer & 0x0000000f);

        switch (chipRev)
        {
            case 0:
                mxtSettings.chipRev = BMXT_ChipRev_eA0;
                deviceHandle->numOfTsmfParsers = 2;
                break;
            case 1:
                mxtSettings.chipRev = BMXT_ChipRev_eA1;
                deviceHandle->numOfTsmfParsers = 2;
                break;
            case 2:
                mxtSettings.chipRev = BMXT_ChipRev_eB0;
                deviceHandle->numOfTsmfParsers = 8;
                break;
            default:
                goto err_rpc;
        }

        errCode = BMXT_Open(&deviceHandle->mxt, NULL, NULL, &mxtSettings);
        if ( errCode != BERR_SUCCESS ) goto err_init;

{
        BMXT_ParserConfig parserConfig;
        BKNI_Memset(&deviceHandle->mtsifConfig, 0, sizeof(NEXUS_FrontendDeviceMtsifConfig));
        deviceHandle->mtsifConfig.mxt = deviceHandle->mxt;
        deviceHandle->mtsifConfig.numDemodIb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eInputBand);
        deviceHandle->mtsifConfig.numDemodPb = BMXT_GetNumResources(mxtSettings.chip, mxtSettings.chipRev, BMXT_ResourceType_eParser);

        for (i=0; i<deviceHandle->mtsifConfig.numDemodPb; i++) {
            errCode = BMXT_GetParserConfig(deviceHandle->mxt, i, &parserConfig);
            if (errCode) { errCode = BERR_TRACE(errCode); goto err_init; }

            /* turn off demod parsers */
            parserConfig.Enable = false;

            /* cache other settings */
            deviceHandle->mtsifConfig.demodPbSettings[i].errorInputIgnore = parserConfig.ErrorInputIgnore;
            deviceHandle->mtsifConfig.demodPbSettings[i].timestampMode = (unsigned)parserConfig.TsMode;
            deviceHandle->mtsifConfig.demodPbSettings[i].dssMode = parserConfig.DssMode;

            errCode = BMXT_SetParserConfig(deviceHandle->mxt, i, &parserConfig);
            if (errCode) { errCode = BERR_TRACE(errCode); goto err_init; }
        }
}

    }
#endif

        /* Create Docsis status callback */
    deviceHandle->eCMStatusCallback = NEXUS_TaskCallback_Create(deviceHandle, NULL);
    if ( NULL == deviceHandle->eCMStatusCallback )
    {
            goto err_init;
        }
        /* start RPC notification thread*/
	deviceHandle->rpc_notification_enabled = true;
    if (NEXUS_GetEnv("no_3255") != NULL){
		BDBG_WRN(("Disabling RPC notification since 3255 is not used"));
			deviceHandle->rpc_notification_enabled = false;
	}

    NEXUS_Thread_GetDefaultSettings(&thread_settings);
        /* TODO:: set correct thread priority*/
    deviceHandle->rpc_notification = NEXUS_Thread_Create("rpc_notification",
                                                         NEXUS_Frontend_3255DeviceRpcNotificationThread,
                                                         (void*)deviceHandle,
                                                         &thread_settings);
    if (deviceHandle->rpc_notification == NULL)
    {
            BDBG_ERR((" can't create RPC notification thread"));
            goto err_init;
        }
        /* start 3255 heartbeat monitoring thread*/
    deviceHandle->heartbeat_enabled= true;
        NEXUS_Thread_GetDefaultSettings(&thread_settings);
        /* TODO:: set correct thread priority*/
    BKNI_CreateEvent(&deviceHandle->heartbeatEvent);
    BKNI_ResetEvent(deviceHandle->heartbeatEvent);
    deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eOperational;
    deviceHandle->heartbeat = NEXUS_Thread_Create("3255_Heartbeat",
                                                  NEXUS_Frontend_P_3255DeviceHeartbeatThread,
                                                  (void*)deviceHandle,
                                                  &thread_settings);

    if (deviceHandle->heartbeat == NULL)
    {
            BDBG_ERR((" can't create 3255_Heartbeat thread"));
            goto err_init;
        }
    BDBG_WRN(("Device Handle %x, RPC handle %x", deviceHandle, deviceHandle->rpc_handle));
    BDBG_MSG(("NEXUS_Frontend_Open3255Device %x>>>",deviceHandle));
    return deviceHandle;

err_init:
    if (deviceHandle->ads)
    {
        BADS_Close(deviceHandle->ads);
    }
    if(deviceHandle->channelCapabilities)
    {
        BKNI_Free(deviceHandle->channelCapabilities);
    }
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    if (deviceHandle->aus)
    {
        BAUS_Close(deviceHandle->aus);
    }
    if (deviceHandle->aob)
    {
        BAOB_Close(deviceHandle->aob);
    }
     if (deviceHandle->oobTnr)
     {
         BTNR_Close(deviceHandle->oobTnr);
     }
#endif
    deviceHandle->deviceStatus = NEXUS_3255DeviceStatus_eUninitialized;
    if (deviceHandle->heartbeat)
    {
        NEXUS_UnlockModule();       /* Need to unlock because heartbeat thread might be blocked on our lock. */
        deviceHandle->heartbeat_enabled = false;
        BKNI_SetEvent(deviceHandle->heartbeatEvent);
        BKNI_Sleep(100);
        NEXUS_Thread_Destroy(deviceHandle->heartbeat);
        NEXUS_LockModule();         /* The heartbeat thread should be gone by now.  Get the module lock again. */
    }

    if (deviceHandle->rpc_notification)
    {
        NEXUS_UnlockModule();       /* Need to unlock because rpc_notification thread might be blocked on our lock. */
        deviceHandle->rpc_notification_enabled = false;
        BKNI_Sleep(600);        /* time for task to finish */
        NEXUS_Thread_Destroy(deviceHandle->rpc_notification);
        NEXUS_LockModule();         /* The rpc_notification thread should be gone by now.  Get the module lock again. */
    }

    if (deviceHandle->heartbeatEvent)
        BKNI_DestroyEvent(deviceHandle->heartbeatEvent);

    if (deviceHandle->eCMStatusCallback)
            NEXUS_TaskCallback_Destroy(deviceHandle->eCMStatusCallback);

        BRPC_Close_SocketImpl(deviceHandle->rpc_handle);
err_rpc:
    BKNI_Free(deviceHandle);
err_malloc:
    return NULL;
}

void NEXUS_Frontend_Close3255Device(NEXUS_3255DeviceHandle deviceHandle)
{

    BDBG_ASSERT(deviceHandle);
    if(deviceHandle->channelCapabilities)
    {
        BKNI_Free(deviceHandle->channelCapabilities);
    }
    if (deviceHandle->ads)
    {
        BADS_Close(deviceHandle->ads);
    }
#if defined(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT)
    if (deviceHandle->aus)
    {
        BAUS_Close(deviceHandle->aus);
    }
    if (deviceHandle->aob)
    {
        BAOB_Close(deviceHandle->aob);
    }
#endif

#if NEXUS_HAS_MXT
    if (deviceHandle->mxt) {
        BMXT_Close(deviceHandle->mxt);
    }
#endif
    if (deviceHandle->heartbeat)
    {
        NEXUS_UnlockModule();       /* Need to unlock because heartbeat thread might be blocked on our lock. */
        deviceHandle->heartbeat_enabled = false;
        BKNI_SetEvent(deviceHandle->heartbeatEvent);
        BKNI_Sleep(100);
        NEXUS_Thread_Destroy(deviceHandle->heartbeat);
        NEXUS_LockModule();         /* The heartbeat thread should be gone by now.  Get the module lock again. */
        BKNI_DestroyEvent(deviceHandle->heartbeatEvent);
    }
    NEXUS_UnlockModule();           /* Need to unlock because rpc_notification thread might be blocked on our lock. */
    deviceHandle->rpc_notification_enabled = false;
    BKNI_Sleep(600); /* time for task to finish */
    NEXUS_Thread_Destroy(deviceHandle->rpc_notification);
    NEXUS_LockModule();             /* The rpc_notification thread should be gone by now.  Get the module lock again. */
    NEXUS_TaskCallback_Destroy(deviceHandle->eCMStatusCallback);
    BRPC_Close_SocketImpl(deviceHandle->rpc_handle);
    BDBG_OBJECT_DESTROY(deviceHandle, NEXUS_3255Device);
    BKNI_Free(deviceHandle);
    return;
}

NEXUS_Error NEXUS_Frontend_Get3255DeviceCapabilities(
    NEXUS_3255DeviceHandle handle,
    NEXUS_3255DeviceCapabilities *pCapabilities
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BDBG_ASSERT((handle));
    pCapabilities->numOfDocsisChannels = handle->numDocsisChannels;
    pCapabilities->numOfQamChannels = handle->numChannels-handle->numDocsisChannels;
    pCapabilities->totalChannels = handle->numChannels;
    pCapabilities->isOobChannelSupported = handle->isOOBsupported;
    if(handle->isOOBsupported)
    {
        pCapabilities->totalChannels+=1;
    }
    pCapabilities->numOfTsmfParsers = handle->numOfTsmfParsers;
    return rc;
}

NEXUS_Error NEXUS_Frontend_Get3255DeviceGpioPinSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_3255DeviceGpioPin pin,
    NEXUS_3255DeviceGpioPinSettings *pSettings
    )
{
    /*TODO*/
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pin);
    BSTD_UNUSED(pSettings);
    return NEXUS_NOT_SUPPORTED;
}


NEXUS_Error NEXUS_Frontend_Set3255DeviceGpioPinSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_3255DeviceGpioPin pin,
    const NEXUS_3255DeviceGpioPinSettings *pSettings
    )
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_POD_CardApplyPower Param;
    BERR_Code retVal;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT( deviceHandle->rpc_handle);
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    switch (pin) {
        case NEXUS_3255DeviceGpioPin_eOob:
            Param.devId = BRPC_DevId_3255;
            Param.powerMode = (pSettings->mode == NEXUS_GpioMode_eOutputPushPull) ?
                               ENABLE_POD_OUTPINS : DISABLE_POD_OUTPINS;
            BDBG_MSG((" NEXUS_3255GpioPin_eOob %s", (Param.powerMode == ENABLE_POD_OUTPINS) ? "enable" : "disable" ));
            retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_POD_CardApplyPower,
                                    (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal);
            if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
            {
                BDBG_ERR((" NEXUS_3255GpioPin_eOob Fail"));
                return NEXUS_INVALID_PARAMETER;
            }
            break;
        default:
            BDBG_WRN((" unknown NEXUS_3255GpioPin"));
            return NEXUS_NOT_SUPPORTED;
    }
    return NEXUS_SUCCESS;

}

#ifdef VENDOR_REQUEST
/***************************************************************************
Summary:
    Process vendor specific request to Docsis
See Also:

 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Get3255DeviceVendorSpecificRequest(
    NEXUS_FrontendHandle handle,
    BRPC_Param_VEN_Request *pRequest,
    BRPC_Param_VEN_Reply *pReply
    )
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)channelHandle->deviceHandle;
    BERR_Code retCode, retVal;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT( deviceHandle->rpc_handle);
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    pRequest->devId = BRPC_DevId_3255_VEN;
    retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_VEN_Request, (const uint32_t *)pRequest,
                            sizeInLong(BRPC_Param_VEN_Request), (uint32_t *)pReply, sizeInLong(BRPC_Param_VEN_Reply), &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_VendorRequest Fail"));
        return NEXUS_NOT_SUPPORTED;
    }
    return NEXUS_SUCCESS;
}
#endif

void NEXUS_Frontend_GetDefault3255ChannelSettings(
    NEXUS_3255ChannelSettings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->channelNumber = 0;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
    pSettings->enableTsmfSupport = true;
#else
    pSettings->enableTsmfSupport = false;
#endif
    return;
}

static NEXUS_Error NEXUS_Frontend_P_3255TransmitDebugPacket(void* handle, NEXUS_FrontendDebugPacketType type,const uint8_t *pBuffer, size_t size)
{
	NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle;
	NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
	NEXUS_Error rc = NEXUS_SUCCESS;
	BERR_Code retCode = BERR_SUCCESS;

	/* The Starvue transmit packet size is fixed at 54 bytes */
	if ((handle == NULL) || (pBuffer == NULL) || (size != 54) || (type != NEXUS_FrontendDebugPacketType_eOob))
	{
		BDBG_ERR(("Parameters is not valid\n"));
		return NEXUS_INVALID_PARAMETER;
	}
	BDBG_ASSERT(channelHandle);
	BDBG_ASSERT(deviceHandle);
	BDBG_ASSERT(deviceHandle->aus);

	retCode = BAUS_TransmitStarvuePkt(deviceHandle->aus, (uint8_t *)pBuffer, size);
	if (retCode != BERR_SUCCESS){
		BDBG_ERR(("NOT SUCCESS\n"));
		rc = (retCode = BERR_OUT_OF_DEVICE_MEMORY)? NEXUS_OUT_OF_DEVICE_MEMORY : NEXUS_INVALID_PARAMETER;
	}
	return rc;
}

NEXUS_FrontendHandle NEXUS_Frontend_Open3255Channel(
    NEXUS_3255DeviceHandle deviceHandle,
    const NEXUS_3255ChannelSettings *pSettings
    )
{
    BERR_Code errCode, rc;
    NEXUS_FrontendHandle frontendHandle;
    unsigned int chn_num;
    NEXUS_3255ChannelHandle channelHandle;
    BTNR_3255Ib_Settings tnr3255_cfg;
    BADS_ChannelSettings chn_cfg;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(pSettings->channelNumber <= deviceHandle->numChannels);

    chn_num = pSettings->channelNumber;
    channelHandle = (NEXUS_3255ChannelHandle)BKNI_Malloc(sizeof(NEXUS_3255Channel));

    if ( NULL == channelHandle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_init;
    }
    BKNI_Memset((void *)channelHandle,0,sizeof(NEXUS_3255Channel));
    BKNI_Memcpy((void *)&channelHandle->channelSettings,(void*)pSettings,sizeof(*pSettings));
    channelHandle->deviceHandle = deviceHandle;
    BDBG_WRN(("Handle %x, Device Handle %x num %x", channelHandle, channelHandle->deviceHandle, chn_num));
    deviceHandle->frontendHandle[deviceHandle->numOfFrontends] = frontendHandle = NEXUS_Frontend_P_Create(channelHandle);
    /* Establish device capabilities */
    if ( deviceHandle->channelCapabilities[pSettings->channelNumber].channelType == NEXUS_3255ChannelType_eInBand)
    {
        rc = BTNR_3255Ib_GetDefaultSettings(&tnr3255_cfg, NULL);
        if (rc != BERR_SUCCESS) goto err_init;
        tnr3255_cfg.hGeneric = deviceHandle->rpc_handle;
        tnr3255_cfg.ifFreq = BTNR_3255Ib_SETTINGS_IFFREQ;
        tnr3255_cfg.devId += chn_num;
        rc =  BTNR_3255Ib_Open(&channelHandle->tnr, NULL, NULL, NULL, &tnr3255_cfg);
        if (rc != BERR_SUCCESS) goto err_init;

        rc = BADS_GetChannelDefaultSettings( deviceHandle->ads, chn_num, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto err_init;
        chn_cfg.autoAcquire = deviceHandle->deviceSettings.autoAcquire;
        chn_cfg.fastAcquire = deviceHandle->deviceSettings.fastAcquire;
        rc = BADS_OpenChannel( deviceHandle->ads,&channelHandle->ads_chn, chn_num, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto err_init;
    if ( NULL == frontendHandle )
    {
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_init;
    }
        frontendHandle->capabilities.docsis = false;
        frontendHandle->capabilities.qam = true;
        frontendHandle->capabilities.outOfBand = true;
        frontendHandle->capabilities.upstream = true;
        BKNI_Memset(frontendHandle->capabilities.qamModes, true, sizeof(frontendHandle->capabilities.qamModes));
        /* bind functions*/
        frontendHandle->tuneQam = NEXUS_Frontend_P_3255QamChannelTune;
        frontendHandle->getQamStatus = NEXUS_Frontend_P_Get3255QamChannelStatus;
        frontendHandle->getSoftDecisions = NEXUS_Frontend_P_Get3255ChannelSoftDecisions;
        frontendHandle->untune = NEXUS_Frontend_P_3255QamChannelUnTune;
        frontendHandle->getTemperature = NEXUS_Frontend_P_3255GetTemperature;
        frontendHandle->standby = NEXUS_Frontend_P_3255Standby;
        frontendHandle->requestQamAsyncStatus = NEXUS_Frontend_P_3255_RequestAsyncQamStatus;
        frontendHandle->getQamAsyncStatus = NEXUS_Frontend_P_3255_GetAsyncQamStatus;
		frontendHandle->transmitDebugPacket= NEXUS_Frontend_P_3255TransmitDebugPacket;
        BDBG_MSG(("deviceHandle->numOfQamFrontends %u",deviceHandle->numOfQamFrontends));
        deviceHandle->numOfQamFrontends++;
    }
    #if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
    else
    {
        channelHandle->tnr = deviceHandle->oobTnr;
        frontendHandle->capabilities.docsis = false;
        frontendHandle->capabilities.qam = false;
        frontendHandle->capabilities.outOfBand = true;
        frontendHandle->capabilities.upstream = true;
        BKNI_Memset(frontendHandle->capabilities.outOfBandModes, true, sizeof(frontendHandle->capabilities.outOfBandModes));
        BKNI_Memset(frontendHandle->capabilities.upstreamModes, true, sizeof(frontendHandle->capabilities.upstreamModes));
        /* bind functions*/
        frontendHandle->tuneOutOfBand = NEXUS_Frontend_P_3255OobChannelTune;
        frontendHandle->getOutOfBandStatus = NEXUS_Frontend_P_Get3255OobChannelStatus;
        frontendHandle->untune = NEXUS_Frontend_P_3255OobChannelUnTune;
        frontendHandle->tuneUpstream = NEXUS_Frontend_P_3255UsChannelTune;
        frontendHandle->getUpstreamStatus = NEXUS_Frontend_P_Get3255UsChannelStatus;
        frontendHandle->getTemperature = NEXUS_Frontend_P_3255GetTemperature;
        frontendHandle->standby = NEXUS_Frontend_P_3255Standby;
        deviceHandle->oobFrontendIndex = deviceHandle->numOfFrontends;
		frontendHandle->transmitDebugPacket= NEXUS_Frontend_P_3255TransmitDebugPacket;
    }
    #endif

    if(deviceHandle->deviceSettings.mtsif)
    {
        if(pSettings->enableTsmfSupport && deviceHandle->numOfUsedTsmfParsers < deviceHandle->numOfTsmfParsers)
        {
            channelHandle->tsmfParserIndex = deviceHandle->numOfUsedTsmfParsers;
            deviceHandle->numOfUsedTsmfParsers++;
        }
        else
        {
            if(deviceHandle->numOfTsmfParsers &&
               deviceHandle->numOfUsedTsmfParsers >= deviceHandle->numOfTsmfParsers)
            {
                BDBG_WRN(("User requested TSMF parsing can't be enabled because all the available parsers are already acquired"));
                channelHandle->channelSettings.enableTsmfSupport = false;
            }
            else
            {
                BDBG_MSG(("TSMF parsing disabled for this channel"));
            }
        }
        frontendHandle->reapplyTransportSettings = NEXUS_Frontend_P_3255_ReapplyTransportSettings;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
        frontendHandle->reapplyTSMFSettings = NEXUS_Frontend_P_3255_ReapplyTSMFSettings;
        frontendHandle->getpathinfo = NEXUS_Frontend_P_3255_GetPathinfo;
#endif
    }

    frontendHandle->close = NEXUS_Frontend_P_Close3255Channel;
    /* Create app callback */
    channelHandle->lockAppCallback = NEXUS_TaskCallback_Create(frontendHandle, NULL);
    if ( NULL == channelHandle->lockAppCallback)
    {
        goto err_app_callback;
    }
    /* Create async status app callback */
    channelHandle->asyncStatusAppCallback = NEXUS_TaskCallback_Create(frontendHandle, NULL);
    if ( NULL == channelHandle->asyncStatusAppCallback)
    {
        goto err_app_callback;
    }
        /* install callback to  notify of lock/unlock change */
    if ( deviceHandle->channelCapabilities[pSettings->channelNumber].channelType == NEXUS_3255ChannelType_eInBand)
    {
        /* Create driver callback for re-acquire check */
        channelHandle->lockDriverCallback = NEXUS_TaskCallback_Create(frontendHandle, NULL);
        if ( NULL == channelHandle->lockDriverCallback)
        {
            goto err_driver_callback;
        }

        rc = BADS_InstallCallback(channelHandle->ads_chn, BADS_Callback_eLockChange,
                                  (BADS_CallbackFunc)NEXUS_Frontend_P_3255QamChannelLockChange, (void*)channelHandle->lockDriverCallback);
        if (rc!=BERR_SUCCESS) goto err_driver_callback;
        NEXUS_CallbackHandler_Init(channelHandle->lockDriverCBHandler, NEXUS_Frontend_P_Check3255ChannelReacquireStatus, (void *)channelHandle);
    }
    #if NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT
    else
    {
        rc = BAOB_InstallCallback(deviceHandle->aob, BAOB_Callback_eLockChange,
                                  (BAOB_CallbackFunc)NEXUS_Frontend_P_3255OobChannelLockChange, (void*)channelHandle->lockAppCallback);
        if (rc!=BERR_SUCCESS) goto err_app_callback;
    }
    #endif
    /* save channel number in channelHandle*/
    channelHandle->frontendIndex = deviceHandle->numOfFrontends;
    deviceHandle->numOfFrontends++;
    channelHandle->chn_num = chn_num;
    channelHandle->deviceHandle = deviceHandle;

    /* Success */
    return frontendHandle;

err_driver_callback:
    if (channelHandle->lockDriverCallback) {
        NEXUS_TaskCallback_Destroy(channelHandle->lockDriverCallback);
    }
err_app_callback:
    if (channelHandle->lockAppCallback) {
        NEXUS_TaskCallback_Destroy(channelHandle->lockAppCallback);
    }
    if (channelHandle->asyncStatusAppCallback) {
        NEXUS_TaskCallback_Destroy(channelHandle->asyncStatusAppCallback);
    }
err_init:
    if(deviceHandle->frontendHandle[deviceHandle->numOfFrontends])
    {
        NEXUS_Frontend_P_Destroy(deviceHandle->frontendHandle[deviceHandle->numOfFrontends]);
        deviceHandle->frontendHandle[deviceHandle->numOfFrontends] = NULL;
    }
    if(channelHandle->tnr)
    {
        BTNR_Close(channelHandle->tnr);
    }
    if(channelHandle->ads_chn)
    {
        BADS_CloseChannel(channelHandle->ads_chn);
    }
    if(channelHandle)
    {
        BKNI_Free(channelHandle);
    }
    return NULL;
}

NEXUS_Error NEXUS_Frontend_Set3255ChannelSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_3255ChannelSettings *pSettings
    )
{
    NEXUS_3255ChannelHandle channelHandle = handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)channelHandle->deviceHandle;
    NEXUS_Error ret = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BKNI_Memcpy((void *)&channelHandle->channelSettings,(void *)pSettings,sizeof(*pSettings));
    if (deviceHandle->eCMStatusCallback)
        NEXUS_TaskCallback_Set(deviceHandle->eCMStatusCallback, &(pSettings->docsisStateChange));

    if(deviceHandle->deviceSettings.mtsif)
    {
        if(pSettings->enableTsmfSupport &&
           deviceHandle->numOfUsedTsmfParsers < deviceHandle->numOfTsmfParsers &&
           channelHandle->frontendIndex!=deviceHandle->oobFrontendIndex)
        {
            channelHandle->tsmfParserIndex = deviceHandle->numOfUsedTsmfParsers;
            deviceHandle->numOfUsedTsmfParsers++;
            BDBG_MSG(("DOCSIS ch %u TsmfParser Index %u frontendIndex %u",channelHandle->chn_num,deviceHandle->numOfUsedTsmfParsers,channelHandle->frontendIndex));
        }
        else
        {
            if(deviceHandle->numOfTsmfParsers &&
               deviceHandle->numOfUsedTsmfParsers >= deviceHandle->numOfTsmfParsers)
            {
                BDBG_WRN(("User requested TSMF parsing can't be enabled because all the available parsers are already acquired"));
                channelHandle->channelSettings.enableTsmfSupport = false;
            }
            else
            {
                BDBG_MSG(("TSMF parsing disabled for this channel"));
            }
        }
    }
    return ret;
}

void NEXUS_Frontend_Get3255ChannelSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_3255ChannelSettings *pSettings
    )
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)channelHandle->deviceHandle;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);

    BDBG_ASSERT(NULL != pSettings);
    *pSettings = channelHandle->channelSettings;
}

NEXUS_Error NEXUS_Frontend_Get3255ChannelCapabilities(
    NEXUS_3255DeviceHandle handle,
    unsigned channel,
    NEXUS_3255ChannelCapabilities *pCapabilities
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BDBG_ASSERT(handle);
    BKNI_Memcpy((void *)pCapabilities,(void*)&handle->channelCapabilities[channel],
                sizeof(NEXUS_3255ChannelCapabilities));
    return rc;
}

NEXUS_Error NEXUS_Frontend_GetDocsisChannelStatus(
    NEXUS_FrontendHandle handle,
    unsigned docsisChannel,
    NEXUS_DocsisChannelStatus *pStatus
    )
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_XXX_Get Param;
    BRPC_Param_ECM_GetStatus outParam;
    BERR_Code retVal;
    BADS_Version version;
#if 0
    BRPC_Param_ADS_GetDsChannelPower param;
#endif

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT( deviceHandle->rpc_handle);

    if (docsisChannel >= deviceHandle->numDocsisChannels)
    {
	BDBG_ERR(("Invalid Docsis Index: %u \n", docsisChannel));
        return NEXUS_INVALID_PARAMETER;
    }

    BKNI_Memset(pStatus, 0, sizeof(NEXUS_DocsisChannelStatus));
    pStatus->deviceStatus = deviceHandle->deviceStatus;
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_SUCCESS);

    retVal = BADS_GetVersion(deviceHandle->ads, &version);
    if (retVal) return NEXUS_NOT_SUPPORTED;

    Param.devId = BRPC_DevId_3255_DS0 + docsisChannel;
    BDBG_MSG((" NEXUS_Frontend_3255_GetDocsisStatus"));
    retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_ECM_GetStatus, (const uint32_t *)&Param,
                            sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_GetDocsisStatus Fail"));
        return NEXUS_NOT_SUPPORTED;
    }

    pStatus->downstreamCenterFreq = outParam.downstreamCenterFreq;
    pStatus->downstreamPowerLevel = (version.minVer <=1) ? outParam.downstreamPowerLevel : 0;
    pStatus->downstreamCarrierLock = outParam.downstreamCarrierLock;
    pStatus->channelScdmaStatus = (NEXUS_3255ChannelScdmaType)outParam.channelScdmaStatus;
    pStatus->upstreamModType = (NEXUS_3255ChannelUsModType)outParam.upstreamModuType;
    pStatus->upstreamXmtCenterFreq = outParam.upstreamXmtCenterFreq;
    pStatus->upstreamPowerLevel = outParam.upstreamPowerLevel;
    pStatus->upStreamSymbolrate = outParam.upStreamSymbolrate;
    pStatus->lastKnownGoodFreq = outParam.lastKnownGoodFreq;

    pStatus->macAddress[0] = (outParam.ecmMacAddressHi>>24)&0xff;
    pStatus->macAddress[1] = (outParam.ecmMacAddressHi>>16)&0xff;
    pStatus->macAddress[2] = (outParam.ecmMacAddressHi>>8)&0xff;
    pStatus->macAddress[3] = (outParam.ecmMacAddressHi)&0xff;
    pStatus->macAddress[4] = (outParam.ecmMacAddressLo>>8)&0xff;
    pStatus->macAddress[5] = (outParam.ecmMacAddressLo)&0xff;

    if (outParam.isEcmIpMode)
    {
        b_convert_ipaddr(pStatus->ipv4Address,outParam.ecmIpAddress);
        b_convert_ipaddr(&pStatus->ipv6Address[0],outParam.ecmIpv6Address0);
        b_convert_ipaddr(&pStatus->ipv6Address[4],outParam.ecmIpv6Address1);
        b_convert_ipaddr(&pStatus->ipv6Address[8],outParam.ecmIpv6Address2);
        b_convert_ipaddr(&pStatus->ipv6Address[12],outParam.ecmIpv6Address3);
    }
    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Frontend_Get3255ChannelAgcConfig(
    NEXUS_FrontendHandle handle,
    NEXUS_3255ChannelAgcConfig *pStatus
    )
{
    NEXUS_3255ChannelHandle channelHandle = (NEXUS_3255ChannelHandle)handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = channelHandle->deviceHandle;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_XXX_Get Param;
    BRPC_Param_TNR_GetAgcVal outParam;
    BERR_Code retVal;

    BDBG_OBJECT_ASSERT(deviceHandle, NEXUS_3255Device);
    BDBG_ASSERT( deviceHandle->rpc_handle);
    if (deviceHandle->deviceStatus != NEXUS_3255DeviceStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    Param.devId = BRPC_DevId_3255_TNR0 + channelHandle->chn_num;
    retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_TNR_GetAgcVal, (const uint32_t *)&Param,
                            sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_GetTnrAgc Fail"));
        return NEXUS_NOT_SUPPORTED;
    }


    /********************************************************************************************************
    The format of 32 bit agcGain passed by 3383

    B3-B2: 16 bit chipid. Ex: 0x3412
    B1: b7b6=Output1Tilt, b5b4=Output2Tilt, b3=0, b2=SuperBoost, b1= Boost, b0=Tilt {0=OFF, 1=ON}
    B0:  Lna gain value from 0-0x1F (RDB indicates a 6 bit value but a valid gain value is only 5 bits)
    +---------------+----------------+-----------------+--------------+
    |            LnaChipId           |  T1/T2/0/S/B/T  |      AGC     |
    +---------------+----------------+-----------------+--------------+
            B3             B2                B1               B0


    Example:  Host receives LNA reading such of 0x3412561f
    Break it down:
    LnaChipId = 3412
    Output1 Stage2 Tilt: 1
    Output2 Stage2 Tilt: 1
    SuperBoost = ON
    Boost = ON
    Stage1 Tilt = OFF
    AGC = 0x1F  (max value)
    **********************************************************************************************************/
    pStatus->agcValue = (0x1f & outParam.AgcVal);
    pStatus->lnaChipId = (outParam.AgcVal >> 16);
    pStatus->output1TiltGain = ((outParam.AgcVal >> 14) & 0x3);
    pStatus->output2TiltGain = ((outParam.AgcVal >> 12) & 0x3);
    pStatus->superBoostEnabled = (outParam.AgcVal & 0x00000400)?true:false;
    pStatus->gainBoostEnabled = (outParam.AgcVal & 0x00000200)?true:false;
    pStatus->tiltEnabled = (outParam.AgcVal & 0x00000100)?true:false;
    BDBG_MSG((" %s  AGC is 0x%x LNA Chip ID 0x%x Tilt %d Boost %d SuperBoost %d Output1 Stage2 Tilt %d Output2 Stage2 Tilt %d",
              __FUNCTION__, pStatus->agcValue, pStatus->lnaChipId, pStatus->tiltEnabled, pStatus->gainBoostEnabled,
              pStatus->superBoostEnabled, pStatus->output1TiltGain, pStatus->output2TiltGain));
    return NEXUS_SUCCESS;

}


NEXUS_3255DeviceHandle NEXUS_Frontend_Get3255DeviceHandle(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_3255ChannelHandle channelHandle = handle->pDeviceHandle;
    NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)channelHandle->deviceHandle;

    return deviceHandle;
}

void NEXUS_Frontend_Get3255LockStatusForHostChannels(
    NEXUS_3255DeviceHandle handle,
    uint32_t *pLockStatus
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_3255Device);

    *pLockStatus = handle->NonCmControlledVideoChLockStatus;
}


NEXUS_Error NEXUS_Frontend_Set3255LockStatusForHostChannels(
    NEXUS_3255DeviceHandle handle,
    uint32_t lockStatus
    )
{
        NEXUS_Error ret = NEXUS_SUCCESS;
        BERR_Code retCode = BERR_SUCCESS;
        BERR_Code retVal;
        BRPC_Param_ECM_HostChannelsLockStatus Param;
        NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)handle;

        Param.NonCmControlledVideoChLockStatus = lockStatus;

        retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_ECM_HostChannelsLockStatus,
                                                        (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal);

        if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
        {
                BDBG_ERR((" Unable to send the HostChannelsLockStatus through RPC to 3383 "));
                return NEXUS_INVALID_PARAMETER;
        }

	deviceHandle->NonCmControlledVideoChLockStatus = lockStatus;

    return ret;
}

NEXUS_Error NEXUS_Frontend_Config3255Lna(
    NEXUS_3255DeviceHandle handle
    )
{
        NEXUS_Error ret = NEXUS_SUCCESS;
        BERR_Code retCode = BERR_SUCCESS;
        BERR_Code retVal;
	BRPC_Param_ECM_DoLnaReConfig Param;
        NEXUS_3255DeviceHandle deviceHandle = (NEXUS_3255DeviceHandle)handle;

	Param.devId = BRPC_DevId_3255;

        retCode = BRPC_CallProc(deviceHandle->rpc_handle, BRPC_ProcId_ECM_DoLnaReConfig,
                                                        (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal);

        if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
        {
                BDBG_ERR((" Unable to send the ECM_DoLnaReConfig through RPC to 3383 "));
                return NEXUS_INVALID_PARAMETER;
        }
    return ret;
}
