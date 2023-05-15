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
* $brcm_Revision: 2 $
* $brcm_Date: 11/8/12 11:39a $
*
* API Description:
*   API name: Frontend 3255
*    APIs to open, close, and setup initial settings for a BCM3255
*    Frontend Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/7125/src/nexus_frontend_3255.c $
* 
* 2   11/8/12 11:39a mward
* SW7125-1321: Warnings about no effect.
* 
* 1   11/8/12 10:28a mward
* SW7125-1321: 7125 uses platform-specifc versions of 3255 files.
* 
* SW7125-772/8   2/8/12 6:03p mward
* SW7125-1231:  Update modulation type in GetQamStatus api by the value
*  returned by eCM so it can be read from DOCSIS tuner.
* 
* SW7125-772/7   11/17/11 12:07p mward
* SW3461-79: Add No Signal to GetLockStatus()
* 
* SW7125-772/6   8/5/11 4:09p mward
* SW7125-1042:  Eliminate compiler warning.
* 
* SW7125-772/5   8/4/11 2:10p mward
* SW7125-1042:  NEXUS_Frontend_3255_PowerUpFrontendDevices() don't power
*  up unless currently powered down.
* 
* SW7125-772/4   7/7/11 5:10p mward
* SW7125-891:  Add Viterbi status values as done for 31xx.
* 
* SW7125-772/3   7/7/11 3:29p mward
* SW7125-1017:  Add and reset accumulating counters as expected by Nexus
*  API.  Compile option provided for previous behavior.
* 
* SW7125-772/2   7/6/11 6:54p mward
* SW7125-1016: Return NEXUS_Error values.
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
*
***************************************************************************/

#include "nexus_frontend_module.h"
#include "nexus_platform_features.h"
#include "bads.h"
#if NEXUS_MAX_3255_ADSCHN
#include "bads_3255.h"
#endif
#include "btnr.h"
#include "btnr_3255ib.h"
#include "brpc.h"
#include "brpc_3255.h"
#include "brpc_socket.h"
#include "brpc_plat.h"

#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)
#include "bchp_common.h"
#include "bchp_us_top.h"
#include "bchp_ds_qdsafe_0_0.h"
#include "bchp_ds_tuner_anactl_0.h"
#include "bchp_ds_tuner_ref_0.h"
#include "bchp_ds_qdsafe_0_1.h"
#include "bchp_ds_tuner_anactl_1.h"
#include "bchp_ds_tuner_ref_1.h"
#include "bchp_int.h"
#include "bchp_oob.h"
#include "bchp_vcxo_ctl_misc.h"
#include "bchp_clkgen.h"
#include "bchp_us_pdac0.h"
#include "bchp_per_timer.h"
#include "bchp_unimac_interface_0.h"
#include "bchp_sub_ubus_sgisb.h"
#endif


/*by default, it has OOB, but with platform which using 3255 code w/o oob, it has to define it to 0 in nexus_platform_features.h*/
#if !defined(NEXUS_HAS_OOB) ||NEXUS_HAS_OOB
#define B_HAS_OOB   1
#else
#define B_HAS_OOB   0
#endif

#if B_HAS_OOB
#include "baob.h"
#include "baus.h"
#include "btnr_3255ob.h"
#endif

#if BCHP_CHIP==7125
#define B_CTRL_DOCSIS_SCAN
#endif

BDBG_MODULE(nexus_frontend_3255);

BDBG_OBJECT_ID(NEXUS_3255);

#define sizeInLong(x)	(sizeof(x)/sizeof(uint32_t))

#define NEXUS_3255_OOB_CHN      NEXUS_MAX_3255_ADSCHN
#define NEXUS_3255_MAX_CHN      (NEXUS_MAX_3255_ADSCHN+1)


/***************************************************************************
 * Internal Module Structure
 ***************************************************************************/
typedef struct NEXUS_3255_DS_Status
{
    NEXUS_TaskCallbackHandle statusCallback;
    BADS_Status	status;
    unsigned fecCorrected;		/* accumulated FEC corrected count*/
    unsigned fecUnCorrected;	/* accumulated FEC uncorrected count*/
    unsigned totalSymbol;		/* accumulated Total symbol count*/
    unsigned unlockCount;    	/* unlock count */
    unsigned acquireCount;    	/* QAM acquiring total count */
    unsigned failureCount;    	/* QAM acquiring failure count */
    unsigned failureFreq;    	/* failure QAM frequency */
    NEXUS_Time lockTime;		/* QAM locked time*/
    bool 	locked;
	unsigned long ulGoodRsBlockCount; /* accumulated RS Block Count */
} NEXUS_3255_DS_Status;

typedef struct NEXUS_3255
{
    BDBG_OBJECT(NEXUS_3255)
    BTNR_Handle tnr[NEXUS_3255_MAX_CHN];
    BADS_Handle ads;
#if NEXUS_MAX_3255_ADSCHN
    BADS_ChannelHandle ads_chn[NEXUS_MAX_3255_ADSCHN];
#endif
#if B_HAS_OOB
    BAUS_Handle aus;
    BAOB_Handle aob;
	unsigned long oobFecCorrected; 		/* accumulated FEC corrected Count */
	unsigned long oobFecUnCorrected; 	/* accumulated FEC uncorrected Count */
	unsigned long oobBerErrorCount; 	/* accumulated BER Count */
#endif
    BRPC_Handle rpc_handle;
    NEXUS_ThreadHandle rpc_notification;
    unsigned rpc_notification_count;
    NEXUS_ThreadHandle heartbeat;
    BKNI_EventHandle heartbeatEvent;
    bool heartbeat_enabled;
    NEXUS_FrontendHandle frontendHandle[NEXUS_3255_MAX_CHN];
    NEXUS_TaskCallbackHandle lockAppCallback[NEXUS_3255_MAX_CHN];
#if NEXUS_MAX_3255_ADSCHN
    NEXUS_FrontendQamSettings qam[NEXUS_MAX_3255_ADSCHN];
	NEXUS_TaskCallbackHandle lockDriverCallback[NEXUS_MAX_3255_ADSCHN];
	NEXUS_CallbackHandler lockDriverCBHandler[NEXUS_MAX_3255_ADSCHN];
    NEXUS_TimerHandle retuneTimer[NEXUS_MAX_3255_ADSCHN];
    NEXUS_3255_DS_Status ads_status[NEXUS_MAX_3255_ADSCHN];
#endif
    NEXUS_FrontendOutOfBandSettings oob;
    NEXUS_FrontendUpstreamSettings us;
    bool rpc_notification_enabled;
    bool tune_started[NEXUS_3255_MAX_CHN];
    bool isOOBsupported;
    unsigned numChannels;
    NEXUS_FrontendOutOfBandSettings last_aob;
#if NEXUS_MAX_3255_ADSCHN
    NEXUS_FrontendQamSettings       last_ads[NEXUS_MAX_3255_ADSCHN];
#endif
    NEXUS_FrontendUpstreamSettings  last_aus;
    NEXUS_3255Settings deviceSettings;
    NEXUS_DocsisStatus status;
    NEXUS_TaskCallbackHandle eCMStatusCallback;
} NEXUS_3255;

typedef struct NEXUS_3255Channel
{
    unsigned chn_num; /* channel number */
    NEXUS_3255 *pDevice; /* 3255 device*/
} NEXUS_3255Channel;

static NEXUS_3255 *p_3255device = NULL;

/***************************************************************************
 * Module callback functions for tuning
 ***************************************************************************/
static void NEXUS_Frontend_P_3255_process_notification(uint32_t device_id, uint32_t event, void *cntx);
static void NEXUS_Frontend_P_3255_Close(NEXUS_FrontendHandle handle);
#if NEXUS_MAX_3255_ADSCHN
static void NEXUS_Frontend_P_3255_ADS_lockchange(void *cntx);
static NEXUS_Error NEXUS_Frontend_P_3255_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings);
static void NEXUS_Frontend_P_3255_UnTuneQam(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3255_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3255_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length);
static void NEXUS_Frontend_P_3255_ResetQamStatus(void *handle);
#endif
#if B_HAS_OOB
static void NEXUS_Frontend_P_3255_AOB_lockchange(void *cntx);
static NEXUS_Error NEXUS_Frontend_P_3255_TuneOOB(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings);
static void NEXUS_Frontend_P_3255_UnTuneOOB(void *handle);
static NEXUS_Error NEXUS_Frontend_P_3255_GetOOBStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_3255_TuneUs(void *handle, const NEXUS_FrontendUpstreamSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_3255_GetUsStatus(void *handle, NEXUS_FrontendUpstreamStatus *pStatus);
#endif
static void NEXUS_Frontend_P_3255_ReacquireCheck(void *context);

#if NEXUS_MAX_3255_ADSCHN
/***************************************************************************
Summary:
    Lock callback handler for a 3255 Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_3255_ADS_lockchange(void *pParam)
{
    NEXUS_TaskCallbackHandle callback = (NEXUS_TaskCallbackHandle)pParam;

    BDBG_MSG(("3255 lock event change"));

    if ( callback )
    {
        NEXUS_TaskCallback_Fire(callback);
    }
}
#endif

#if B_HAS_OOB
/***************************************************************************
Summary:
    Lock callback handler for a 3255 OutofBand device
 ***************************************************************************/
static void NEXUS_Frontend_P_3255_AOB_lockchange(void *pParam)
{
    NEXUS_TaskCallbackHandle callback = (NEXUS_TaskCallbackHandle)pParam;

    BDBG_MSG(("3255 OOB lock event change"));

    if ( callback )
    {
        NEXUS_TaskCallback_Fire(callback);
    }
}
#endif

/***************************************************************************
Summary:
    RPC Notification process thread
 ***************************************************************************/
static void b_check_rpc(void *arg)
{
    uint32_t device_id, event;
    struct NEXUS_3255 *pDevice = (NEXUS_3255 *)arg;
    /*TODO:: use infinite select() instead of polling*/
    while(pDevice->rpc_notification_enabled) {
        BRPC_CheckNotification(pDevice->rpc_handle,  &device_id, &event, 100);
        if (BRPC_GET_NOTIFICATION_EVENT(event)) {
            BDBG_MSG(("check_rpc(): notified by server (device_id = %08x) event is %x\n", device_id, event));
            NEXUS_LockModule();
            NEXUS_Frontend_P_3255_process_notification(device_id, event, arg);
            NEXUS_UnlockModule();
        }
    }
}

/***************************************************************************
Summary:
    Get the default settings for a BCM3255 tuner
See Also:
    NEXUS_Frontend_Open3255
 ***************************************************************************/
void NEXUS_Frontend_GetDefault3255Settings(
    NEXUS_3255Settings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->type = NEXUS_3255ChannelType_eInBand;
    pSettings->channelNumber = 0;
    pSettings->autoAcquire = false;
    pSettings->fastAcquire = false;
    pSettings->enableFEC = false; /* CableCARD will do FEC for OOB*/
}

/***************************************************************************
Summary:
    Set the settings for a BCM3255 tuner
See Also:
    NEXUS_Frontend_3255_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_3255_SetSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_3255Settings *pSettings
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    NEXUS_Error ret = NEXUS_SUCCESS;
#ifdef B_CTRL_DOCSIS_SCAN
    BERR_Code rc;
    bool docsisDisabled;
#endif

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
#ifdef B_CTRL_DOCSIS_SCAN
    docsisDisabled = pDevice->deviceSettings.disableDocsis;
#endif

    pDevice->deviceSettings = *pSettings;
    if (pDevice->eCMStatusCallback)
        NEXUS_TaskCallback_Set(pDevice->eCMStatusCallback, &(pSettings->docsisStateChange));

#ifdef B_CTRL_DOCSIS_SCAN
    if ( docsisDisabled != pSettings->disableDocsis)
    {
        if (pDevice->status == NEXUS_DocsisStatus_eOperational && pDevice->tnr[0])
        {
            if (docsisDisabled)
            {
                rc = BTNR_SetTunerRfFreq(pDevice->tnr[0], 0, BTNR_TunerMode_eDocsis);
                if (rc!=BERR_SUCCESS) { ret = NEXUS_NOT_SUPPORTED;}
            }
            else
            {
                /* hardcode RF frequency as 597Mhz here for the time being*/
                rc = BTNR_SetTunerRfFreq(pDevice->tnr[0], 597000000, BTNR_TunerMode_eDigital);
                if (rc!=BERR_SUCCESS) { ret = NEXUS_NOT_SUPPORTED;}
            }
            if (ret != NEXUS_SUCCESS)
                pDevice->deviceSettings.disableDocsis = docsisDisabled;
        }
        else
            pDevice->deviceSettings.disableDocsis = docsisDisabled;
    }
#endif

    return ret;
}

/***************************************************************************
Summary:
    Get the current settings for a BCM3255 tuner
See Also:
    NEXUS_Frontend_3255_SetSettings
 ***************************************************************************/
void NEXUS_Frontend_3255_GetSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_3255Settings *pSettings
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

    BDBG_ASSERT(NULL != pSettings);
    *pSettings = pDevice->deviceSettings;
}

#ifdef VENDOR_REQUEST
/***************************************************************************
Summary:
    Process vendor specific request to Docsis
See Also:

 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_3255_VendorRequest(
    NEXUS_FrontendHandle handle,
    BRPC_Param_VEN_Request *pRequest,
    BRPC_Param_VEN_Reply *pReply
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BERR_Code retCode, retVal;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    pRequest->devId = BRPC_DevId_3255_VEN;
    retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_VEN_Request, (const uint32_t *)pRequest, sizeInLong(BRPC_Param_VEN_Request), (uint32_t *)pReply, sizeInLong(BRPC_Param_VEN_Reply), &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_VendorRequest Fail"));
        return NEXUS_NOT_SUPPORTED;
    }
    return NEXUS_SUCCESS;
}
#endif

/***************************************************************************
Summary:
    Initialize all ADS/OOB channels.
 ***************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_Init3255(NEXUS_3255 *pDevice)
{
    BERR_Code rc;
#if NEXUS_MAX_3255_ADSCHN
    unsigned int i, num_ch;
    BADS_Settings ads_cfg;
    BADS_ChannelSettings chn_cfg;
    BTNR_3255Ib_Settings tnr3255_cfg;
#endif
#if B_HAS_OOB
    BTNR_3255Ob_Settings tnrOb3255_cfg;
    BAOB_Settings aob_cfg;
    BAUS_Settings aus_cfg;
#endif

#if NEXUS_MAX_3255_ADSCHN
    rc = BADS_3255_GetDefaultSettings( &ads_cfg, NULL);
    if ( rc != BERR_SUCCESS ) goto err_init;

    ads_cfg.hGeneric = pDevice->rpc_handle;
    rc = BADS_Open(&pDevice->ads, NULL, NULL, NULL, &ads_cfg);
    if ( rc != BERR_SUCCESS ) goto err_init;

    rc = BADS_Init(pDevice->ads);
    if ( rc != BERR_SUCCESS ) goto err_init;

    /* get total ADS channel number*/
    rc = BADS_GetTotalChannels(pDevice->ads, &num_ch);
    if (rc!=BERR_SUCCESS) goto err_init;
    pDevice->numChannels = num_ch;

    for (i=0;i<num_ch;i++) {
        rc = BTNR_3255Ib_GetDefaultSettings(&tnr3255_cfg, NULL);
        if (rc != BERR_SUCCESS) goto err_init;
        tnr3255_cfg.hGeneric = pDevice->rpc_handle;
        tnr3255_cfg.ifFreq = BTNR_3255Ib_SETTINGS_IFFREQ;
        tnr3255_cfg.devId += i;
        rc =  BTNR_3255Ib_Open(&pDevice->tnr[i], NULL, NULL, NULL, &tnr3255_cfg);
        if (rc != BERR_SUCCESS) goto err_init;
    }

    /* Configure ADS channels */
    for (i=0;i<num_ch;i++) {
        rc = BADS_GetChannelDefaultSettings( pDevice->ads, i, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto err_init;
        chn_cfg.autoAcquire = pDevice->deviceSettings.autoAcquire;
        chn_cfg.fastAcquire = pDevice->deviceSettings.fastAcquire;
        rc = BADS_OpenChannel( pDevice->ads, &pDevice->ads_chn[i], i, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto err_init;
    }
#endif /* #if NEXUS_MAX_3255_ADSCHN */


#if B_HAS_OOB
    rc = BTNR_3255Ob_GetDefaultSettings(&tnrOb3255_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto oob_done;
    tnrOb3255_cfg.hGeneric = pDevice->rpc_handle;

/* BTNR_3255OB_SETTINGS_IFFREQ is deprecated (incorrectly assigns IF_FREQ based on host chip)
   use NEXUS_3255_OOB_TUNER_IFFREQ instead (to be defined in nexus_platform_features.h) */
#ifndef NEXUS_3255_OOB_TUNER_IFFREQ
#define NEXUS_3255_OOB_TUNER_IFFREQ BTNR_3255OB_SETTINGS_IFFREQ
#endif

    tnrOb3255_cfg.ifFreq = NEXUS_3255_OOB_TUNER_IFFREQ;
    rc = BTNR_3255Ob_Open(&pDevice->tnr[NEXUS_3255_OOB_CHN], NULL, &tnrOb3255_cfg);
    if (rc!=BERR_SUCCESS) goto oob_done;

    rc = BAOB_GetDefaultSettings( &aob_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto oob_done;
    aob_cfg.hGeneric = pDevice->rpc_handle;
    aob_cfg.enableFEC = pDevice->deviceSettings.enableFEC;
    rc = BAOB_Open(&pDevice->aob, NULL, NULL, NULL, &aob_cfg);
    if (rc!=BERR_SUCCESS) goto oob_done;

    rc = BAUS_GetDefaultSettings(&aus_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto oob_done;
    aus_cfg.xtalFreq = BAUS_SETTINGS_XTALFREQ;
    aus_cfg.hGeneric = pDevice->rpc_handle;;
    rc = BAUS_Open( &pDevice->aus, NULL, NULL, &aus_cfg);
    if (rc!=BERR_SUCCESS) goto oob_done;

    pDevice->isOOBsupported = true;
oob_done:

#else
    pDevice->isOOBsupported = false;
#endif

    pDevice->status = NEXUS_DocsisStatus_eOperational;
    return NEXUS_SUCCESS;

#if NEXUS_MAX_3255_ADSCHN
err_init:
    for ( i = 0; i < NEXUS_MAX_3255_ADSCHN && NULL != pDevice->tnr[i]; i++)
        BTNR_Close(pDevice->tnr[i]);
    for ( i = 0; i < NEXUS_MAX_3255_ADSCHN && NULL != pDevice->ads_chn[i]; i++)
        BADS_CloseChannel(pDevice->ads_chn[i]);
    if (pDevice->ads) BADS_Close(pDevice->ads);
#endif
#if B_HAS_OOB
    if (pDevice->tnr[NEXUS_3255_OOB_CHN]) BTNR_Close(pDevice->tnr[NEXUS_3255_OOB_CHN]);
    if (pDevice->aus) BAUS_Close(pDevice->aus);
    if (pDevice->aob) BAOB_Close(pDevice->aob);
#endif
    pDevice->status = NEXUS_DocsisStatus_eUninitialized;

    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_InitRPC(NEXUS_3255 *pDevice)
{
    BRPC_OpenSocketImplSettings socketSettings;
    BRPC_Param_InitSession Param;
    BERR_Code errCode, retCode, retVal;
    unsigned count;

    BRPC_GetDefaultOpenSocketImplSettings(&socketSettings);
#if NEXUS_MAX_3255_ADSCHN
    socketSettings.timeout = 50; /* streamline for other platforms */
#else
    socketSettings.timeout = 2000; /* need 2 secs for the VMS/SMS */
#endif
    if (NEXUS_GetEnv("no_3255") != NULL)
    {
        socketSettings.rpc_disabled = true;
        BDBG_WRN(("**********325X is NOT used!**********"));
    }
    errCode = BRPC_Open_SocketImpl(&pDevice->rpc_handle, &socketSettings);
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
            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_InitSession, (const uint32_t *)&Param, sizeof(Param)/4, NULL, 0, &retVal);
            if (retCode == BERR_SUCCESS/*&& retVal == BERR_SUCCESS*/) break;
            BKNI_Sleep(10);
            #if (BCHP_CHIP==7125)
            if (count++ > 20)
            #else
            if (count++ >= 1000)
            #endif
            {
                BDBG_ERR((" timeout in waiting for 3255"));
                BRPC_Close_SocketImpl(pDevice->rpc_handle);
                return NEXUS_UNKNOWN;
            };
        }
    }
    BDBG_WRN((" RPC is initialized between 3255 and 740x"));
    BRPC_Close_SocketImpl(pDevice->rpc_handle);

    /* reopen RPC socket to apply normal timeout value*/
    socketSettings.timeout = 3000; /*3000ms for reliable connection*/
    errCode = BRPC_Open_SocketImpl(&pDevice->rpc_handle, &socketSettings);
    if ( errCode != BERR_SUCCESS ) return NEXUS_UNKNOWN;
    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Shut down all 3255 channels.
 ***************************************************************************/
static void NEXUS_Frontend_3255_ChannelClose(NEXUS_3255 * pDevice)
{
#if NEXUS_MAX_3255_ADSCHN
    unsigned i;

    /* all channels are closed, close everthing*/
    for ( i = 0; i < NEXUS_MAX_3255_ADSCHN && NULL != pDevice->tnr[i]; i++) {
        BTNR_Close(pDevice->tnr[i]);
        pDevice->tnr[i] = NULL;
    }

    for ( i = 0; i < NEXUS_MAX_3255_ADSCHN && NULL != pDevice->ads_chn[i]; i++) {
        BADS_CloseChannel(pDevice->ads_chn[i]);
        pDevice->ads_chn[i] = NULL;
    }

    if (pDevice->ads) {
        BADS_Close(pDevice->ads);
        pDevice->ads = NULL;
    }
#endif

#if B_HAS_OOB
    if (pDevice->tnr[NEXUS_3255_OOB_CHN]) BTNR_Close(pDevice->tnr[NEXUS_3255_OOB_CHN]);
    if (pDevice->aus) {
        BAUS_Close(pDevice->aus);
        pDevice->aus = NULL;
    }
    if (pDevice->aob) {
        BAOB_Close(pDevice->aob);
        pDevice->aob = NULL;
    }
#endif
    pDevice->status = NEXUS_DocsisStatus_eUninitialized;
    pDevice->isOOBsupported = false;

}
/***************************************************************************
Summary:
    Check to see whether 3255 has reset.
 ***************************************************************************/
static void NEXUS_Frontend_3255_Heartbeat(void * arg)
{
    struct NEXUS_3255 *pDevice = (NEXUS_3255 *)arg;
    BERR_Code retVal, retCode;
    NEXUS_Error errCode;
    BRPC_Param_ADS_GetVersion outVerParam;
    BRPC_Param_XXX_Get Param;
    bool need_restart;
#if NEXUS_MAX_3255_ADSCHN
    int i;
#endif
    int count;

    Param.devId = BRPC_DevId_3255;

    pDevice->rpc_notification_count = 0; /* clear notification count*/

#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)
startover:
#endif
    while(pDevice->heartbeat_enabled == true)
    {
		#if 0
        BDBG_MSG((" check eCM's heartbeat every 2 seconds"));
		#endif
        need_restart = true;

        /* to avoid false alarm that eCM is dead or reset:
           (1) use double RPC calls to verify eCM status
           (2) eCM notification also indicates eCM is alive
        */
        if (pDevice->status == NEXUS_DocsisStatus_eOperational)
        {
            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ADS_GetVersion, (const uint32_t *)&Param, sizeof(Param)/4, (uint32_t *)&outVerParam, sizeInLong(outVerParam), &retVal);
            need_restart = (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS) && (!pDevice->rpc_notification_count);
            if (need_restart == true && pDevice->status == NEXUS_DocsisStatus_eOperational) /* do one more test */
            {
                BKNI_Sleep(100);
                retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ADS_GetVersion, (const uint32_t *)&Param, sizeof(Param)/4, (uint32_t *)&outVerParam, sizeInLong(outVerParam), &retVal);
                need_restart = (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS)&&(!pDevice->rpc_notification_count);
                if (need_restart) pDevice->status = NEXUS_DocsisStatus_eReset;
            }
        }
        while (need_restart == true && pDevice->heartbeat_enabled == true)
        {
            BDBG_ERR((" Delay 10 seconds and start polling eCM "));
            retCode = BKNI_WaitForEvent(pDevice->heartbeatEvent, 10000);
            if (pDevice->heartbeat_enabled == false) break;
            /*  delay max 10 seconds or wait for EcmRmagnumReady event then start polling eCM */
            BKNI_ResetEvent(pDevice->heartbeatEvent);
            count = 0;
            if (NEXUS_GetEnv("no_3255") == NULL)
            {
                while (pDevice->heartbeat_enabled == true) {
                    BDBG_MSG((" waiting for eCM booting up!"));
                    retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_InitSession, (const uint32_t *)&Param, sizeof(Param)/4, NULL, 0, &retVal);
                    need_restart = (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS);
                    if (need_restart == false ||  ++count >= 5) break;
                    BKNI_Sleep(100);
                }
            }
            if (need_restart == true)
            {
                BDBG_ERR((" eCM is dead! "));
                NEXUS_LockModule();
                pDevice->status = NEXUS_DocsisStatus_eFailed;
                /* ask application to manually reset eCM*/
                NEXUS_TaskCallback_Fire(pDevice->eCMStatusCallback);
                NEXUS_UnlockModule();
            }
            else
            {
                BDBG_ERR((" eCM's heartbeat is back. Re-initialize ADS/AOB/AUS "));
                NEXUS_LockModule();
                NEXUS_Frontend_3255_ChannelClose(pDevice);
                /* coverity[freed_arg] */
                errCode = NEXUS_Frontend_P_Init3255(pDevice);
                if (errCode) {
                    BDBG_ERR((" Error in Re-initialize ADS/AOB/AUS, what to do? "));
                    NEXUS_UnlockModule();
                    continue;
                } else {
#if NEXUS_MAX_3255_ADSCHN
                    for ( i = 0; i < NEXUS_MAX_3255_ADSCHN && NULL != pDevice->ads_chn[i]; i++)
                    {
                        BADS_InstallCallback(pDevice->ads_chn[i], BADS_Callback_eLockChange,
                            (BADS_CallbackFunc)NEXUS_Frontend_P_3255_ADS_lockchange, (void*)pDevice->lockDriverCallback[i]);
                    }
#endif
#if B_HAS_OOB
                    if (pDevice->aob)
                    {
                        BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eLockChange,
                            (BAOB_CallbackFunc)NEXUS_Frontend_P_3255_AOB_lockchange, (void*)pDevice->lockAppCallback[NEXUS_3255_OOB_CHN]);
                    }
#endif
                }
                NEXUS_TaskCallback_Fire(pDevice->eCMStatusCallback);
                NEXUS_UnlockModule();
            }
        }
		#if 0
        BDBG_MSG(" Done checking eCM heartbeat "));
		#endif
next_try:
        NEXUS_LockModule();
        pDevice->rpc_notification_count = 0; /* clear notification count*/
        NEXUS_UnlockModule();
        retCode = BKNI_WaitForEvent(pDevice->heartbeatEvent, 2000);
        if (pDevice->heartbeat_enabled == false) break;
        if (pDevice->rpc_notification_count) {
            BDBG_MSG((" Got %d RPC notification, no need to poll",pDevice->rpc_notification_count ));
            goto next_try;
        }
    }

#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)
    while(pDevice->status == NEXUS_DocsisStatus_ePowerSave)
    {
        /*BDBG_MSG((" check eCM's heartbeat every 200ms, if notifcation ecm_ready is not recevied\n"));*/
        if(pDevice->heartbeat_enabled == true ){
            bool ready, rpc_disabled;
            retCode = 0;
            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ADS_GetVersion, (const uint32_t *)&Param, sizeof(Param)/4, (uint32_t *)&outVerParam, sizeInLong(outVerParam), &retVal);
            ready = (retCode == BERR_SUCCESS && retVal == BERR_SUCCESS);
            rpc_disabled = pDevice->rpc_handle->rpc_disabled;
            BDBG_MSG(("ready %d, rpc enabled %d, retCode %d, retVal %d\n", ready, rpc_disabled, retCode, retVal));
            if(ready){
                pDevice->status = NEXUS_DocsisStatus_eOperational;
                BDBG_MSG(("in case ecm_ready not recevied, but eCM wakeup, set to operational\n"));
                goto startover;
            }
        }
        BKNI_Sleep(200);
    }
#endif

}


/***************************************************************************
Summary:
    Open a handle to a BCM3255 device.
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open3255(
    const NEXUS_3255Settings *pSettings
    )
{
    BERR_Code errCode, rc;
    NEXUS_FrontendHandle frontendHandle;
    NEXUS_3255 *pDevice = p_3255device;
    unsigned int chn_num;
    bool cleanupDeviceOnError = false;        /* Assume that device already exists */
    NEXUS_ThreadSettings thread_settings;
    NEXUS_3255Channel *channelHandle;

    BDBG_ASSERT(NULL != pSettings);
    chn_num = pSettings->channelNumber;

#if NEXUS_MAX_3255_ADSCHN
    if ( ((chn_num >= NEXUS_MAX_3255_ADSCHN)&& pSettings->type == NEXUS_3255ChannelType_eInBand)
        || (chn_num >= NEXUS_3255_MAX_CHN ))
    {
        BDBG_ERR((" channel number exceeds available one"));
        goto err_malloc;
    }
#endif

    if ( NULL == pDevice)
    {
        cleanupDeviceOnError = true;    /* Since we're creating the device, we'll need to delete it if we have an error. */

        pDevice = BKNI_Malloc(sizeof(NEXUS_3255));
        if ( NULL == pDevice )
        {
          errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc;
        }
        BKNI_Memset(pDevice, 0, sizeof(NEXUS_3255));
        BDBG_OBJECT_SET(pDevice, NEXUS_3255);

        if (NEXUS_Frontend_P_InitRPC(pDevice) ) goto err_rpc;

        pDevice->deviceSettings = *pSettings;
        /* Initialize ADS/OOB channels */
        errCode = NEXUS_Frontend_P_Init3255(pDevice);
        if ( errCode != BERR_SUCCESS ) goto err_init;

        /* Create Docsis status callback */
        pDevice->eCMStatusCallback = NEXUS_TaskCallback_Create(pDevice, NULL);
        if ( NULL == pDevice->eCMStatusCallback )
        {
            goto err_init;
        }

        /* start RPC notification thread*/
        pDevice->rpc_notification_enabled = true;
        NEXUS_Thread_GetDefaultSettings(&thread_settings);
        /* TODO:: set correct thread priority*/

        pDevice->rpc_notification = NEXUS_Thread_Create("rpc_notification", b_check_rpc, (void*)pDevice, &thread_settings);
        if (pDevice->rpc_notification == NULL)
        {
            BDBG_ERR((" can't create RPC notification thread"));
            goto err_init;
        }

        /* start 3255 heartbeat monitoring thread*/
        pDevice->heartbeat_enabled= true;
        NEXUS_Thread_GetDefaultSettings(&thread_settings);
        /* TODO:: set correct thread priority*/
        BKNI_CreateEvent(&pDevice->heartbeatEvent);
        BKNI_ResetEvent(pDevice->heartbeatEvent);

        pDevice->heartbeat = NEXUS_Thread_Create("3255_Heartbeat", NEXUS_Frontend_3255_Heartbeat, (void*)pDevice, &thread_settings);
        if (pDevice->heartbeat == NULL)
        {
            BDBG_ERR((" can't create 3255_Heartbeat thread"));
            goto err_init;
        }

        p_3255device = pDevice;
    }
    else
    {
        BDBG_MSG((" found device"));
    }

    /* check if requested channel is available*/
    if ( pSettings->type == NEXUS_3255ChannelType_eInBand
        && chn_num >= pDevice->numChannels)
    {
        BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_init;      /* Go clean up if necessary */
    }

    /* check if OOB/upstream channel is available*/
    if ( (pSettings->type == NEXUS_3255ChannelType_eOutOfBand
          || pSettings->type == NEXUS_3255ChannelType_eUpstream)
          && pDevice->isOOBsupported == false)
    {
        BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_init;      /* Go clean up if necessary */
    }

    /* check if frontend Handle is already opened*/
    if ( pDevice->frontendHandle[chn_num] != NULL )
    {
        BDBG_WRN((" Frontend channel is already opened"));
        return pDevice->frontendHandle[chn_num];
    }

    channelHandle = (NEXUS_3255Channel*)BKNI_Malloc(sizeof(NEXUS_3255Channel));
    if ( NULL == channelHandle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_init;
    }

    /* Create a Nexus frontend handle */
    pDevice->frontendHandle[chn_num] = frontendHandle = NEXUS_Frontend_P_Create(channelHandle);
    if ( NULL == frontendHandle )
    {
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        BKNI_Free(channelHandle);
        goto err_init;
    }

    /* Establish device capabilities */
    if ( pSettings->type == NEXUS_3255ChannelType_eInBand)
    {
#if NEXUS_MAX_3255_ADSCHN
        frontendHandle->capabilities.qam = true;
        frontendHandle->capabilities.outOfBand = false;
        frontendHandle->capabilities.upstream = false;
        BKNI_Memset(frontendHandle->capabilities.qamModes, true, sizeof(frontendHandle->capabilities.qamModes));
        /* bind functions*/
        frontendHandle->tuneQam = NEXUS_Frontend_P_3255_TuneQam;
        frontendHandle->getQamStatus = NEXUS_Frontend_P_3255_GetQamStatus;
        frontendHandle->getSoftDecisions = NEXUS_Frontend_P_3255_GetSoftDecisions;
        frontendHandle->untune = NEXUS_Frontend_P_3255_UnTuneQam;
		frontendHandle->resetStatus = NEXUS_Frontend_P_3255_ResetQamStatus;
#endif
    }
#if B_HAS_OOB
    else
    {
        frontendHandle->capabilities.qam = false;
        frontendHandle->capabilities.outOfBand = true;
        frontendHandle->capabilities.upstream = true;
        BKNI_Memset(frontendHandle->capabilities.outOfBandModes, true, sizeof(frontendHandle->capabilities.outOfBandModes));
        BKNI_Memset(frontendHandle->capabilities.upstreamModes, true, sizeof(frontendHandle->capabilities.upstreamModes));
        /* bind functions*/
        frontendHandle->tuneOutOfBand = NEXUS_Frontend_P_3255_TuneOOB;
        frontendHandle->getOutOfBandStatus = NEXUS_Frontend_P_3255_GetOOBStatus;
        frontendHandle->untune = NEXUS_Frontend_P_3255_UnTuneOOB;
        frontendHandle->tuneUpstream = NEXUS_Frontend_P_3255_TuneUs;
        frontendHandle->getUpstreamStatus = NEXUS_Frontend_P_3255_GetUsStatus;
    }
#endif
    frontendHandle->close = NEXUS_Frontend_P_3255_Close;

    /* Create app callback */
    pDevice->lockAppCallback[chn_num] = NEXUS_TaskCallback_Create(frontendHandle, NULL);
    if ( NULL == pDevice->lockAppCallback[chn_num] )
    {
        goto err_app_callback;
    }
#if NEXUS_MAX_3255_ADSCHN
        /* install callback to  notify of lock/unlock change */
    if ( pSettings->type == NEXUS_3255ChannelType_eInBand)
    {
		/* Create driver callback for re-acquire check */
		pDevice->lockDriverCallback[chn_num] = NEXUS_TaskCallback_Create(frontendHandle, NULL);
		if ( NULL == pDevice->lockDriverCallback[chn_num] )
		{
			goto err_driver_callback;
		}

        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eLockChange,
								  (BADS_CallbackFunc)NEXUS_Frontend_P_3255_ADS_lockchange, (void*)pDevice->lockDriverCallback[chn_num]);
        if (rc!=BERR_SUCCESS) goto err_driver_callback;
		NEXUS_CallbackHandler_Init(pDevice->lockDriverCBHandler[chn_num], NEXUS_Frontend_P_3255_ReacquireCheck, (void *)channelHandle);
    }
#endif
#if B_HAS_OOB
    else
    {
        rc = BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eLockChange, 
								  (BAOB_CallbackFunc)NEXUS_Frontend_P_3255_AOB_lockchange, (void*)pDevice->lockAppCallback[chn_num]);
        if (rc!=BERR_SUCCESS) goto err_app_callback;
    }
#endif

    /* save channel number in channelHandle*/
    channelHandle->chn_num = chn_num;
    channelHandle->pDevice = pDevice;
    /* Success */
    return frontendHandle;

err_driver_callback:
    if (pDevice->lockDriverCallback[chn_num]) {
        NEXUS_TaskCallback_Destroy(pDevice->lockDriverCallback[chn_num]);
    }
err_app_callback:
    if (pDevice->lockAppCallback[chn_num]) {
        NEXUS_TaskCallback_Destroy(pDevice->lockAppCallback[chn_num]);
    }
    NEXUS_Frontend_P_Destroy(frontendHandle);

err_init:
    if (cleanupDeviceOnError) {
        if (pDevice->heartbeat) {
            NEXUS_UnlockModule();       /* Need to unlock because heartbeat thread might be blocked on our lock. */
                pDevice->heartbeat_enabled = false;
                BKNI_SetEvent(pDevice->heartbeatEvent);
                BKNI_Sleep(100);
                NEXUS_Thread_Destroy(pDevice->heartbeat);
            NEXUS_LockModule();         /* The heartbeat thread should be gone by now.  Get the module lock again. */
        }

        if (pDevice->rpc_notification) {
            NEXUS_UnlockModule();       /* Need to unlock because rpc_notification thread might be blocked on our lock. */
                pDevice->rpc_notification_enabled = false;
                BKNI_Sleep(600);        /* time for task to finish */
                NEXUS_Thread_Destroy(pDevice->rpc_notification);
            NEXUS_LockModule();         /* The rpc_notification thread should be gone by now.  Get the module lock again. */
        }

        if (pDevice->heartbeatEvent)
            BKNI_DestroyEvent(pDevice->heartbeatEvent);

        if (pDevice->eCMStatusCallback)
            NEXUS_TaskCallback_Destroy(pDevice->eCMStatusCallback);

        BRPC_Close_SocketImpl(pDevice->rpc_handle);
    }
err_rpc:
    if (cleanupDeviceOnError) {
        BKNI_Free(pDevice);
        p_3255device = NULL;
    }
err_malloc:
    return NULL;
}

/***************************************************************************
Summary:
    Close a handle to a BCM3255 device.
See Also:
    NEXUS_Frontend_Open3255
 ***************************************************************************/
static void NEXUS_Frontend_P_3255_Close(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_3255 *pDevice;
    NEXUS_3255Channel *pChannel;
    unsigned i;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pChannel =(NEXUS_3255Channel*) handle->pDeviceHandle;
    pDevice = (NEXUS_3255 *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

    if (pChannel->chn_num >= NEXUS_3255_MAX_CHN)
    {
        BDBG_ERR((" wrong Frontend Handle"));
        return;
    }

#if NEXUS_MAX_3255_ADSCHN
    /* uninstall lock change callback */
    if (( pChannel->chn_num < NEXUS_MAX_3255_ADSCHN) && pDevice->ads_chn[pChannel->chn_num] )
    {
        BERR_Code rc;
        rc = BADS_InstallCallback(pDevice->ads_chn[pChannel->chn_num], BADS_Callback_eLockChange, NULL, NULL);
        if (rc) {
            rc = BERR_TRACE(rc);
        }
    }
#endif
#if B_HAS_OOB
    else if (pDevice->aob)
    {
        BERR_Code rc;
        rc = BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eLockChange, NULL, NULL);
        if (rc) {
            rc = BERR_TRACE(rc);
        }
    }
#endif

#if NEXUS_MAX_3255_ADSCHN
    if (pChannel->chn_num < NEXUS_MAX_3255_ADSCHN) 
	{
		if (pDevice->retuneTimer[pChannel->chn_num])
		{
			/* in case channel tuned with autoAcquire wasn't untuned prior to close. */
			NEXUS_CancelTimer(pDevice->retuneTimer[pChannel->chn_num]);
			pDevice->retuneTimer[pChannel->chn_num] = NULL;
		}
		NEXUS_CallbackHandler_Shutdown(pDevice->lockDriverCBHandler[pChannel->chn_num]);
		if ( NULL != pDevice->lockDriverCallback[pChannel->chn_num])
			NEXUS_TaskCallback_Destroy(pDevice->lockDriverCallback[pChannel->chn_num]);
		pDevice->lockDriverCallback[pChannel->chn_num] = NULL;
	}
#endif

    if ( NULL != pDevice->lockAppCallback[pChannel->chn_num])
        NEXUS_TaskCallback_Destroy(pDevice->lockAppCallback[pChannel->chn_num]);
    pDevice->lockAppCallback[pChannel->chn_num] = NULL;

#if NEXUS_MAX_3255_ADSCHN
    if ( pChannel->chn_num < NEXUS_MAX_3255_ADSCHN
        && NULL != pDevice->ads_status[pChannel->chn_num].statusCallback)
    {
        NEXUS_TaskCallback_Destroy(pDevice->ads_status[pChannel->chn_num].statusCallback);
        pDevice->ads_status[pChannel->chn_num].statusCallback = NULL;
    }
#endif

    NEXUS_Frontend_P_Destroy(handle);
    pDevice->frontendHandle[pChannel->chn_num] = NULL;
    BKNI_Free(pChannel);

    /* see if there are still open channels*/
    for ( i=0; i< NEXUS_3255_MAX_CHN; i++)
    {
        if ( pDevice->frontendHandle[i]) return;
    }

    NEXUS_Frontend_3255_ChannelClose(pDevice);

    if (pDevice->heartbeat)
    {
        NEXUS_UnlockModule();       /* Need to unlock because heartbeat thread might be blocked on our lock. */
            pDevice->heartbeat_enabled = false;
            BKNI_SetEvent(pDevice->heartbeatEvent);
            BKNI_Sleep(100);
            NEXUS_Thread_Destroy(pDevice->heartbeat);
        NEXUS_LockModule();         /* The heartbeat thread should be gone by now.  Get the module lock again. */
        BKNI_DestroyEvent(pDevice->heartbeatEvent);
    }


    NEXUS_UnlockModule();           /* Need to unlock because rpc_notification thread might be blocked on our lock. */
        pDevice->rpc_notification_enabled = false;
        BKNI_Sleep(600); /* time for task to finish */
        NEXUS_Thread_Destroy(pDevice->rpc_notification);
    NEXUS_LockModule();             /* The rpc_notification thread should be gone by now.  Get the module lock again. */

    NEXUS_TaskCallback_Destroy(pDevice->eCMStatusCallback);

    BRPC_Close_SocketImpl(pDevice->rpc_handle);

    BDBG_OBJECT_DESTROY(pDevice, NEXUS_3255);
    BKNI_Free(pDevice);
    p_3255device = NULL;
}

static void NEXUS_Frontend_P_3255_process_notification(uint32_t device_id, uint32_t event, void * arg)
{
#if NEXUS_MAX_3255_ADSCHN
    int id;
#endif
    int rpc_event;
    struct NEXUS_3255 *pDevice = (NEXUS_3255 *)arg;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

    pDevice->rpc_notification_count++;

    switch (device_id) {
#if NEXUS_MAX_3255_ADSCHN
        case BRPC_DevId_3255_DS0:
        case BRPC_DevId_3255_DS1:
        case BRPC_DevId_3255_DS2:
        case BRPC_DevId_3255_DS3:
            id = device_id - BRPC_DevId_3255_DS0;
            rpc_event = BRPC_GET_NOTIFICATION_EVENT(event);
            if (id < NEXUS_MAX_3255_ADSCHN && pDevice->ads_chn[id] != NULL) {
                if (rpc_event == BRPC_Notification_Event_DsChannelPower) {
                    BDBG_MSG((" got DS power notification"));
                    pDevice->ads_status[id].status.dsChannelPower = BRPC_GET_DS_POWER(event);
                    if (pDevice->ads_status[id].statusCallback)
                        NEXUS_TaskCallback_Fire(pDevice->ads_status[id].statusCallback);
                    }
                else if (pDevice->tune_started[id] == true) /* only handle notification when tuner is active*/
                {
                    BADS_ProcessNotification(pDevice->ads_chn[id], event);
                }
            }
            break;
#endif
#if B_HAS_OOB
        case BRPC_DevId_3255_OB0:
            if (pDevice->tune_started[NEXUS_3255_OOB_CHN]== true) /* only handle notification when tuner is active*/
            {
                if (pDevice->aob != NULL)
                    BAOB_ProcessNotification(pDevice->aob, event);
            }
            break;
#endif
    case BRPC_DevId_3255:
            rpc_event = BRPC_GET_NOTIFICATION_EVENT(event);
            if (rpc_event == BRPC_Notification_Event_EcmReset) {
                BDBG_WRN((" got eCM reset notification"));
                pDevice->status = NEXUS_DocsisStatus_eReset;
                NEXUS_TaskCallback_Fire(pDevice->eCMStatusCallback);
            }
            else if (rpc_event == BRPC_Notification_Event_EcmOperational) {
                  /* The ECM opertaional notification is sent after Docsis is registered*/
                  /* not very useful for Rmagnum bring up*/
                BDBG_MSG((" got eCM registeration notification"));
            }
            else if (rpc_event == BRPC_Notification_Event_EcmRmagnumReady) {
                BDBG_WRN((" got eCM Rmagnum ready  notification"));
                BKNI_SetEvent(pDevice->heartbeatEvent);
#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)
                /*BDBG_ASSERT(pDevice->status == NEXUS_DocsisStatus_ePowerSave);*/
                /*SO FAR I FOUND POLLING ECM ALIVE IS QUICKER THAN GETTING THIS NOTIFCATION*/
                pDevice->status = NEXUS_DocsisStatus_eOperational;
                BDBG_MSG(("\n\n\n process notification set to normal docsis status\n\n\n"));
#endif
            }
            break;
        case BRPC_DevId_3255_US0:
        case BRPC_DevId_3255_TNR0:
        case BRPC_DevId_3255_TNR1:
        case BRPC_DevId_3255_TNR2:
        case BRPC_DevId_3255_TNR3:
        default:
            BDBG_WRN((" unknown notification from 3255 device %d", device_id));
    }
}

#if NEXUS_MAX_3255_ADSCHN
static void NEXUS_Frontend_P_3255_UnTuneQam(void *handle)
{
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;
#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)
    BERR_Code rc;
    BTNR_PowerSaverSettings pwrSetting;
#endif
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    pDevice->tune_started[chn_num] = false;
    if (pDevice->retuneTimer[chn_num])
        NEXUS_CancelTimer(pDevice->retuneTimer[chn_num]);
    pDevice->retuneTimer[chn_num] = NULL;

    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return;

    if (NEXUS_GetEnv("ds0_video") == NULL)
    {
        if (chn_num == 0) return;
    }
#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)
/* STB_3.0.1beta4_02162011 doesn't power-on external QAM.  You should not untune 31xx
unless you will tune it under host control to power it back on. */
    if (pDevice->tnr[chn_num])
    {
        pwrSetting.enable = true;
        rc = BTNR_SetPowerSaver(pDevice->tnr[chn_num], &pwrSetting);
        if (rc!=BERR_SUCCESS) { BDBG_ERR((" error in BTNR_SetPowerSaver"));}
    }
/*  ADS power-save currently handled by BTNR call.
    rc = BADS_EnablePowerSaver(pDevice->ads_chn[chn_num]);
    if (rc!=BERR_SUCCESS) { BDBG_ERR((" error in BADS_EnablePowerSaver"));}
*/
#endif
    return;
}

static void NEXUS_Frontend_P_3255_Reacquire(void *context)
{

    NEXUS_3255Channel *channelHandle = (NEXUS_3255Channel *)context;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BADS_ModulationType qam_mode;
    BERR_Code rc;
    BADS_InbandParam params;
    unsigned chn_num = channelHandle->chn_num;
    NEXUS_FrontendQamSettings *pSettings = &pDevice->last_ads[chn_num];
    BADS_LockStatus lockStatus;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(NULL != pSettings);

    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return;

    rc = BADS_GetLockStatus(pDevice->ads_chn[chn_num], &lockStatus);
    if (rc!=BERR_SUCCESS) { BERR_TRACE(rc); return; }
    if (lockStatus != BADS_LockStatus_eLocked)
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
        rc = BADS_Acquire(pDevice->ads_chn[chn_num], &params);
        if (rc!=BERR_SUCCESS) { BERR_TRACE(rc); return; }
        pDevice->tune_started[chn_num] = true;
    }
    pDevice->retuneTimer[chn_num] = NULL;

}

static void NEXUS_Frontend_P_3255_ReacquireCheck(void *context)
{
    NEXUS_3255Channel *channelHandle = (NEXUS_3255Channel *)context;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BERR_Code rc;
    unsigned chn_num = channelHandle->chn_num;
    NEXUS_FrontendQamSettings *pSettings = &pDevice->last_ads[chn_num];
    BADS_LockStatus lockStatus;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(NULL != pSettings);

	BDBG_MSG(("%s() calling firing lockAppCallback.",__FUNCTION__));
	NEXUS_TaskCallback_Fire(pDevice->lockAppCallback[chn_num]);

    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return;

    rc = BADS_GetLockStatus(pDevice->ads_chn[chn_num], &lockStatus);
    if (rc!=BERR_SUCCESS) { BERR_TRACE(rc); return; }

    if ((lockStatus != BADS_LockStatus_eLocked) && pSettings->autoAcquire)
    {
        if( pDevice->retuneTimer[chn_num] )
        {
            NEXUS_CancelTimer(pDevice->retuneTimer[chn_num]);
        }    
        BDBG_MSG(("%s() unlocked, schedule a timer to reacquire.",__FUNCTION__));
        pDevice->retuneTimer[chn_num] = NEXUS_ScheduleTimer(2000, NEXUS_Frontend_P_3255_Reacquire, context);
    }
}

static NEXUS_Error NEXUS_Frontend_P_3255_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings)
{
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BADS_ModulationType qam_mode;
    BERR_Code rc;
    BADS_InbandParam params;
#if 0
    BTNR_PowerSaverSettings pwrSetting;
#endif
    unsigned chn_num = channelHandle->chn_num;
    NEXUS_CallbackDesc callbackDesc;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(NULL != pSettings);
	BDBG_ASSERT(chn_num < NEXUS_MAX_3255_ADSCHN);

    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

#if !(defined(B_CTRL_DOCSIS_SCAN) || BCHP_CHIP==7425)
    if (NEXUS_GetEnv("ds0_video") == NULL)
    {
        if (chn_num == 0)
        {
            BDBG_WRN((" Reserved Docsis tuner, no tuning action is performed"));
            return NEXUS_SUCCESS;
        }
    }
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
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
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
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
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
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
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
	NEXUS_CallbackHandler_PrepareCallback(pDevice->lockDriverCBHandler[chn_num], callbackDesc);
	NEXUS_TaskCallback_Set(pDevice->lockDriverCallback[chn_num], &callbackDesc);
	NEXUS_TaskCallback_Set(pDevice->lockAppCallback[chn_num], &(pSettings->lockCallback));

    if (pDevice->tnr[chn_num])
    {
#if 0
        pwrSetting.enable = false;
        rc = BTNR_SetPowerSaver(pDevice->tnr[chn_num], &pwrSetting);
        if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}
#endif
        rc = BTNR_SetTunerRfFreq(pDevice->tnr[chn_num], pSettings->frequency, BTNR_TunerMode_eDigital);
        if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}
    }
	pDevice->ads_status[chn_num].fecCorrected = 0;
	pDevice->ads_status[chn_num].fecUnCorrected = 0;
	pDevice->ads_status[chn_num].ulGoodRsBlockCount = 0;

    params.modType = qam_mode;
    rc = BADS_Acquire(pDevice->ads_chn[chn_num], &params);
    if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}

    pDevice->tune_started[chn_num] = true;
    if (pDevice->retuneTimer[chn_num])
        NEXUS_CancelTimer(pDevice->retuneTimer[chn_num]);
    pDevice->retuneTimer[chn_num] = NULL;
    pDevice->last_ads[chn_num] = *pSettings;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_3255_GetQamLockStatus(NEXUS_FrontendHandle frontHandle, bool *locked)
{
    BERR_Code rc;
    NEXUS_3255Channel *channelHandle = frontHandle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;
	BADS_LockStatus lockStatus;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

    *locked = false;
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BADS_GetLockStatus(pDevice->ads_chn[chn_num], &lockStatus);
	*locked = (lockStatus == BADS_LockStatus_eLocked);
    if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_Frontend_P_3255_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus)
{
    BERR_Code rc;
    BADS_Status st;
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;
    BADS_Version version;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);


    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BADS_GetStatus(pDevice->ads_chn[chn_num],  &st);
    if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}
    rc = BADS_GetVersion(pDevice->ads, &version);
    if (rc) return (NEXUS_Error)BERR_TRACE(rc);

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

#if NEXUS_FRONTEND_ACCUMULATE_STATISTICS
	pDevice->ads_status[chn_num].fecCorrected = st.accCorrectedCount;
	pDevice->ads_status[chn_num].fecUnCorrected = st.accUncorrectedCount;
	pDevice->ads_status[chn_num].ulGoodRsBlockCount += st.goodRsBlockCount;
	pStatus->fecCorrected = pDevice->ads_status[chn_num].fecCorrected;
	pStatus->fecUncorrected = pDevice->ads_status[chn_num].fecUnCorrected;
	pStatus->goodRsBlockCount = pDevice->ads_status[chn_num].ulGoodRsBlockCount;
#else	
	pStatus->fecCorrected = st.correctedCount;
	pStatus->fecUncorrected = st.uncorrectedCount;
	pStatus->goodRsBlockCount = st.goodRsBlockCount;
#endif

    pStatus->snrEstimate = st.snrEstimate*100/256;
    pStatus->berEstimate = st.berRawCount;
    pStatus->postRsBer = st.postRsBER;
    pStatus->postRsBerElapsedTime = st.elapsedTimeSec;
    pStatus->spectrumInverted = st.isSpectrumInverted;
    pStatus->settings = pDevice->last_ads[chn_num];

	switch ( st.modType )
	{
	case BADS_ModulationType_eAnnexAQam16:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e16;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam32:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e32;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam64:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e64;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam128:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e128;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam256:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e256;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam512:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e512;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam1024:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e1024;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam2048:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e2048;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam4096:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e4096;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case BADS_ModulationType_eAnnexBQam16:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e16;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam32:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e32;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam64:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e64;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam128:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e128;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam256:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e256;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam512:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e512;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam1024:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e1024;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam2048:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e2048;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam4096:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e4096;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case BADS_ModulationType_eAnnexCQam16:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e16;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam32:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e32;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam64:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e64;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam128:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e128;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam256:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e256;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam512:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e512;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam1024:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e1024;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam2048:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e2048;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam4096:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e4096;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	default:
		/* reported value unrecognized. leave it as set */
		BDBG_MSG(("unrecognized modType"));
		break;
	}

    BDBG_MSG((" STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t st.agcExtLevel %d \t snr_estimate %d \t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d",
            st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel, st.snrEstimate,
            st.correctedCount, st.uncorrectedCount, st.berRawCount));

	pStatus->viterbiUncorrectedBits = st.correctedBits + (uint32_t)((uint64_t)pStatus->fecUncorrected * 11224)/1000;

	if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eA){
		pStatus->viterbiTotalBits = (uint32_t)(((uint64_t)pStatus->fecCorrected + (uint64_t)pStatus->fecUncorrected + (uint64_t)pStatus->fecClean) * 204 * 8);  
	}
	else if(pStatus->settings.annex == NEXUS_FrontendQamAnnex_eB){
		pStatus->viterbiTotalBits = (uint32_t)(((uint64_t)pStatus->fecCorrected + (uint64_t)pStatus->fecUncorrected + (uint64_t)pStatus->fecClean) * 127 * 7);  
	}
	if (pStatus->viterbiTotalBits) {
		pStatus->viterbiErrorRate = (uint32_t)((uint64_t)pStatus->viterbiUncorrectedBits * 2097152 * 1024 / (unsigned)pStatus->viterbiTotalBits);
	}

	return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_Frontend_P_3255_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length)
{
    int i;
    BERR_Code rc;
    int16_t *data_i, *data_q;
    int16_t return_length;
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;


    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    if ((data_i = BKNI_Malloc(length*sizeof(int16_t))) == NULL) {
        return NEXUS_OUT_OF_SYSTEM_MEMORY;
    }
    if ((data_q = BKNI_Malloc(length*sizeof(int16_t))) == NULL) {
        BKNI_Free(data_i);
        return NEXUS_OUT_OF_SYSTEM_MEMORY;
    }

    rc = BADS_GetSoftDecision(pDevice->ads_chn[chn_num], (int16_t)length,  data_i, data_q, &return_length);
    if (rc!=BERR_SUCCESS) {
        BKNI_Free(data_i);
        BKNI_Free(data_q);
        return_length = length;
        return (NEXUS_Error)BERR_TRACE(rc);
    }

    for(i=0;i<return_length;i++) {
        pDecisions[i].i =  data_i[i]*2;
        pDecisions[i].q = data_q[i]*2;
    }

    BKNI_Free(data_i);
    BKNI_Free(data_q);

    return NEXUS_SUCCESS;

}


NEXUS_Error NEXUS_Frontend_3255_GetAsyncQamStatus(NEXUS_FrontendHandle frontHandle, NEXUS_FrontendQamStatus *pStatus)
{
    NEXUS_3255Channel *channelHandle = frontHandle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;
    BADS_Status *pSt;


    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    pSt = &pDevice->ads_status[chn_num].status;
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
	
#ifdef NEXUS_FRONTEND_ACCUMULATE_STATISTICS
	pDevice->ads_status[chn_num].fecCorrected = pSt->accCorrectedCount;
	pDevice->ads_status[chn_num].fecUnCorrected = pSt->accUncorrectedCount;
	pDevice->ads_status[chn_num].ulGoodRsBlockCount += pSt->goodRsBlockCount;
	pStatus->fecCorrected = pDevice->ads_status[chn_num].fecCorrected;
	pStatus->fecUncorrected = pDevice->ads_status[chn_num].fecUnCorrected;
	pStatus->goodRsBlockCount = pDevice->ads_status[chn_num].ulGoodRsBlockCount;
#else	
    pStatus->fecCorrected = pSt->correctedCount;
    pStatus->fecUncorrected = pSt->uncorrectedCount;
    pStatus->goodRsBlockCount = pSt->goodRsBlockCount;
#endif

    pStatus->snrEstimate = pSt->snrEstimate*100/256;
    pStatus->berEstimate = pSt->berRawCount;
    pStatus->postRsBer = pSt->postRsBER;
    pStatus->postRsBerElapsedTime = pSt->elapsedTimeSec;
    pStatus->spectrumInverted = pSt->isSpectrumInverted;
    pStatus->settings = pDevice->last_ads[chn_num];

	switch ( pSt->modType )
	{
	case BADS_ModulationType_eAnnexAQam16:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e16;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam32:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e32;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam64:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e64;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam128:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e128;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam256:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e256;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam512:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e512;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam1024:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e1024;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam2048:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e2048;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case  BADS_ModulationType_eAnnexAQam4096:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e4096;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eA;
		break;
	case BADS_ModulationType_eAnnexBQam16:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e16;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam32:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e32;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam64:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e64;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam128:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e128;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam256:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e256;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam512:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e512;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam1024:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e1024;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam2048:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e2048;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case  BADS_ModulationType_eAnnexBQam4096:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e4096;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eB;
		break;
	case BADS_ModulationType_eAnnexCQam16:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e16;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam32:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e32;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam64:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e64;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam128:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e128;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam256:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e256;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam512:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e512;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam1024:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e1024;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam2048:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e2048;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	case  BADS_ModulationType_eAnnexCQam4096:
		pStatus->settings.acquisitionMode = NEXUS_FrontendQamMode_e4096;
		pStatus->settings.annex = NEXUS_FrontendQamAnnex_eC;
		break;
	default:
		/* reported value unrecognized. leave it as set */
		BDBG_MSG(("unrecognized modType"));
		break;
	}

    BDBG_MSG((" STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t agcExtLevel %d \t snr_estimate %d \t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d",
         pStatus->fecLock, pStatus->receiverLock, pStatus->ifAgcLevel, pStatus->rfAgcLevel, pStatus->snrEstimate,
         pStatus->fecCorrected, pStatus->fecUncorrected, pStatus->berEstimate));
    BDBG_MSG((" STATUS DS: ds power %d in 1/10 dBmV", pStatus->dsChannelPower));
    return NEXUS_SUCCESS;

}


NEXUS_Error NEXUS_Frontend_3255_RequestAsyncQamStatus(NEXUS_FrontendHandle frontHandle, const NEXUS_CallbackDesc *cb)
{
    BERR_Code rc;
    NEXUS_3255Channel *channelHandle = frontHandle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    unsigned chn_num = channelHandle->chn_num;
    BADS_Version version;
    NEXUS_3255_DS_Status *ds_status;
    BRPC_Param_ADS_GetDsChannelPower param;
    BERR_Code retCode = BERR_SUCCESS;
    BERR_Code retVal;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

    BDBG_MSG((" start NEXUS_Frontend_3255_GetQamStatus"));
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    if (cb == NULL) return (NEXUS_INVALID_PARAMETER);

    ds_status = &pDevice->ads_status[chn_num];
    if (ds_status->statusCallback == NULL)
    {
        ds_status->statusCallback = NEXUS_TaskCallback_Create(pDevice, NULL);
    if ( NULL == ds_status->statusCallback )
    {
        BDBG_ERR((" can't create QAM status Callback"));
        return (NEXUS_NOT_SUPPORTED);
    }
    NEXUS_TaskCallback_Set(ds_status->statusCallback, cb);
    }

    rc = BADS_GetStatus(pDevice->ads_chn[chn_num],  &ds_status->status);
    if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}

    BDBG_MSG((" STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t st.agcExtLevel %d \t snr_estimate %d \t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d",
            ds_status->status.isFecLock, ds_status->status.isQamLock, ds_status->status.agcIntLevel, ds_status->status.agcExtLevel, ds_status->status.snrEstimate,
            ds_status->status.correctedCount, ds_status->status.uncorrectedCount, ds_status->status.berRawCount));
    BDBG_MSG((" end NEXUS_Frontend_3255_GetQamStatus"));


    rc = BADS_GetVersion(pDevice->ads, &version);
    if (rc) return (NEXUS_Error)BERR_TRACE(rc);

    if (version.minVer <= 1)
    {
        NEXUS_TaskCallback_Fire(ds_status->statusCallback);
        return NEXUS_SUCCESS;
    }

    BDBG_ASSERT( pDevice->rpc_handle);
    param.devId = BRPC_DevId_3255_DS0 + chn_num;
    retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ADS_GetDsChannelPower, (const uint32_t *)&param, sizeInLong(param), NULL, 0, &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_GetDSPower Fail"));
        return NEXUS_NOT_SUPPORTED;
    }
    return NEXUS_SUCCESS;
}
#else


NEXUS_Error NEXUS_Frontend_3255_GetQamLockStatus(NEXUS_FrontendHandle frontHandle, bool *locked)
{
    BSTD_UNUSED(frontHandle);
    *locked = false;
    return NEXUS_SUCCESS;
}
#endif


#if 0
NEXUS_Error NEXUS_Frontend_3255_GetDSPower(
    NEXUS_FrontendHandle handle,
    int *powerlevel
    )
{
	BERR_Code rc;
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
	NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
	unsigned chn_num = channelHandle->chn_num;
	BADS_Version version;
	BRPC_Param_ADS_GetDsChannelPower param;
	BERR_Code retCode = BERR_SUCCESS;
    BERR_Code retVal;
 	BADS_Status st;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

	*powerlevel = pDevice->ads_status[chn_num].channelPower = NEXUS_FRONTEND_INVALID_DS_POWER;

	if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
	rc = BADS_GetVersion(pDevice->ads, &version);
	if (rc) return (NEXUS_NOT_SUPPORTED);

    if (version.minVer <= 1)
	{
		BADS_GetStatus(pDevice->ads_chn[chn_num], &st);
        *powerlevel = st.dsChannelPower;
    }
	else
	{
        BDBG_ASSERT( pDevice->rpc_handle);
		if (pDevice->ads_status[chn_num].channelPowerEvent == NULL)
		{
			retCode = BKNI_CreateEvent(&pDevice->ads_status[chn_num].channelPowerEvent);
			if (pDevice->ads_status[chn_num].channelPowerEvent == NULL
				|| retCode != BERR_SUCCESS)
				return (NEXUS_NOT_SUPPORTED);
		}

		param.devId = BRPC_DevId_3255_DS0 + chn_num;
        retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ADS_GetDsChannelPower, (const uint32_t *)&param, sizeInLong(param), NULL, 0, &retVal);
        if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
        {
            BDBG_ERR((" NEXUS_Frontend_3255_GetDSPower Fail"));
            return NEXUS_NOT_SUPPORTED;
        }
		BKNI_ResetEvent(pDevice->ads_status[chn_num].channelPowerEvent);
		NEXUS_UnlockModule();
		retCode = BKNI_WaitForEvent(pDevice->ads_status[chn_num].channelPowerEvent, 1000);
 	 	NEXUS_LockModule();
		if (pDevice->ads_status[chn_num].channelPower == NEXUS_FRONTEND_INVALID_DS_POWER
			|| retCode == BERR_TIMEOUT)
			return NEXUS_NOT_SUPPORTED;
		else
			*powerlevel = pDevice->ads_status[chn_num].channelPower;
	}
    return NEXUS_SUCCESS;
}
#endif


NEXUS_Error NEXUS_Frontend_3255_GetGpioPinSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_3255GpioPin pin,
    NEXUS_3255GpioPinSettings *pSettings
    )
{
    /*TODO*/
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pin);
    BSTD_UNUSED(pSettings);
    return NEXUS_NOT_SUPPORTED;
}


NEXUS_Error NEXUS_Frontend_3255_SetGpioPinSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_3255GpioPin pin,
    const NEXUS_3255GpioPinSettings *pSettings
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_POD_CardApplyPower Param;
    BERR_Code retVal;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    switch (pin) {
        case NEXUS_3255GpioPin_eOob:
            Param.devId = BRPC_DevId_3255;
            Param.powerMode = (pSettings->mode == NEXUS_GpioMode_eOutputPushPull) ?
                               ENABLE_POD_OUTPINS : DISABLE_POD_OUTPINS;
            BDBG_MSG((" NEXUS_3255GpioPin_eOob %s", (Param.powerMode == ENABLE_POD_OUTPINS) ? "enable" : "disable" ));
            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_POD_CardApplyPower, (const uint32_t *)&Param, sizeInLong(Param), NULL, 0, &retVal);
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

void b_convert_ipaddr(char *c, int b)
{
    c[0] = (b>>24)&0xff;
    c[1] = (b>>16)&0xff;
    c[2] = (b>>8)&0xff;
    c[3] =  b&0xff;
}

NEXUS_Error NEXUS_Frontend_3255_GetDocsisStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDocsisStatus *pStatus
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_XXX_Get Param;
    BRPC_Param_ECM_GetStatus outParam;
    BERR_Code retVal;
    BADS_Version version;
#if 0
    BRPC_Param_ADS_GetDsChannelPower param;
#endif

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);

    BKNI_Memset(pStatus, 0, sizeof(NEXUS_FrontendDocsisStatus));
    pStatus->status = pDevice->status;
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_SUCCESS);

    retVal = BADS_GetVersion(pDevice->ads, &version);
    if (retVal) return NEXUS_NOT_SUPPORTED;

    Param.devId = BRPC_DevId_3255_DS0;
    BDBG_MSG((" NEXUS_Frontend_3255_GetDocsisStatus"));
    retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ECM_GetStatus, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_GetDocsisStatus Fail"));
        return NEXUS_NOT_SUPPORTED;
    }

    pStatus->downstreamCenterFreq = outParam.downstreamCenterFreq;
    pStatus->downstreamPowerLevel = (version.minVer <=1) ? outParam.downstreamPowerLevel : 0;
    pStatus->downstreamCarrierLock = outParam.downstreamCarrierLock;
    pStatus->channelScdmaStatus = (NEXUS_DocsisScdmaType)outParam.channelScdmaStatus;
    pStatus->upstreamModType = (NEXUS_DocsisUsModType)outParam.upstreamModuType;
    pStatus->upstreamXmtCenterFreq = outParam.upstreamXmtCenterFreq;
    pStatus->upstreamPowerLevel = outParam.upstreamPowerLevel;
    pStatus->upStreamSymbolrate = outParam.upStreamSymbolrate;

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
#if 0 /* TODO:: get DS0 power level*/
	if (version.minVer > 1)
	{
        BDBG_ASSERT( pDevice->rpc_handle);
	    if (pDevice->ads_status[0].channelPowerEvent == NULL)
		{
			retCode = BKNI_CreateEvent(&pDevice->ads_status[0].channelPowerEvent);
			if (pDevice->ads_status[0].channelPowerEvent == NULL
				|| retCode != BERR_SUCCESS) goto done;
		}
		param.devId = BRPC_DevId_3255_DS0;
        retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ADS_GetDsChannelPower, (const uint32_t *)&param, sizeInLong(param), NULL, 0, &retVal);
        if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
        {
            BDBG_ERR((" NEXUS_Frontend_3255_GetDSPower Fail"));
            goto done;
        }
		pDevice->ads_status[0].channelPower = NEXUS_FRONTEND_INVALID_DS_POWER;
		BKNI_ResetEvent(pDevice->ads_status[0].channelPowerEvent);
		NEXUS_UnlockModule();
		retCode = BKNI_WaitForEvent(pDevice->ads_status[0].channelPowerEvent, 1000);
 	 	NEXUS_LockModule();
        pStatus->downstreamPowerLevel = pDevice->ads_status[0].channelPower;
        BDBG_MSG((" Docsis DS power %d 1/10 dBmV", pStatus->downstreamPowerLevel));
	}
	done:
#endif

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_3255_GetTnrAgc(
    NEXUS_FrontendHandle handle,
    uint32_t *agcVal
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
    BERR_Code retCode = BERR_SUCCESS;
    BRPC_Param_XXX_Get Param;
    BRPC_Param_TNR_GetAgcVal outParam;
    BERR_Code retVal;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    Param.devId = BRPC_DevId_3255_TNR0 + channelHandle->chn_num;
    retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_TNR_GetAgcVal, (const uint32_t *)&Param, sizeInLong(Param), (uint32_t *)&outParam, sizeInLong(outParam), &retVal);
    if (retCode != BERR_SUCCESS || retVal != BERR_SUCCESS )
    {
        BDBG_ERR((" NEXUS_Frontend_3255_GetTnrAgc Fail"));
        return NEXUS_NOT_SUPPORTED;
    }
	*agcVal = outParam.AgcVal;
    BDBG_MSG((" %s	AGC is 0x%x", __FUNCTION__,*agcVal));
    return NEXUS_SUCCESS;

}

#if NEXUS_POWER_MANAGEMENT && (BCHP_CHIP==7125)

static NEXUS_Error NEXUS_Frontend_P_3255_PowerDownFrontendDS0(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_MSG((" NEXUS_Frontend_3255_PowerDownFrontendDS0 start\n"));
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		/*DS0.power_down_DS0_AFE*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE0);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE0, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE0, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE0, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE0, PWRUP_CMLBUF,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE0, PWRUP,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE0, EN_CLK,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE0, reg);

		reg = BREG_Read32(hReg, BCHP_DS_QDSAFE_0_0_CTRL1);
		reg &= ~(
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PD_ADC) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PWDN_BG) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, CLK_RESET)
				);
		reg |=
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PD_ADC,  1 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PWDN_BG,  1 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, CLK_RESET,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_QDSAFE_0_0_CTRL1, reg);


		/*DS_0.power_down_DS0_Tuner*/
		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_03);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_03, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_04, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_05);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_05, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_06);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_PWRUP_LF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_QP2_PD) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_MANIBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_QP_PD) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_VC_PRB_EN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_VZ_PRB_EN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_PWRUP_LF,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_QP2_PD, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_MANIBAL,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_QP_PD,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_VC_PRB_EN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_VZ_PRB_EN,  0)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_06, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_07);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IP_PD_LO) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IP_PD_LO, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN,  1)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_07, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_14);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_14, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_02, I_OUTCML_PWRDN_M)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_02, I_OUTCML_PWRDN_M,  0x7f )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_00);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_VCOBUF_LATCH_ON)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP, 0) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP,  0) | 
				/* this field will indicate to tnr/7125 (host) driver whether shared init has been done */
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_VCOBUF_LATCH_ON,  0) 
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_00, reg);


		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_QP_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_QP_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_04, reg);

		/*DS_0.power_down_DS0_receiver*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS0);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS0, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, EN_CLK,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS0, reg);

		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DS0_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DS0_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		BDBG_MSG((" NEXUS_Frontend_3255_PowerDownFrontendDS0 end\n"));
	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerDownFrontendDS1(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		BDBG_MSG((" NEXUS_Frontend_3255_PowerDownFrontendDS1 start\n"));
		BSTD_UNUSED(handle);
		/*power_down_DS1_AFE*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE1);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE1, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE1, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE1, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE1, PWRUP_CMLBUF,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE1, PWRUP,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE1, EN_CLK,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE1, reg);

		reg = BREG_Read32(hReg, BCHP_DS_QDSAFE_0_1_CTRL1);
		/* mask/shift defs missing from ds1 register RDB headers, but same as DS0 */
		reg &= ~(
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PD_ADC) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PWDN_BG) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, CLK_RESET)
				);
		reg |=
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PD_ADC,  1 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PWDN_BG,  1 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, CLK_RESET,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_QDSAFE_0_1_CTRL1, reg);


		/*power_down_DS1_Tuner*/
		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_03);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_03, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_04, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_05);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_05, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_07);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN,  1)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_07, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_14);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_14, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_00);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP, 0) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP,  0)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_00, reg);


		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_QP_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_QP_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_04, reg);

		/*power_down_DS1_receiver*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS1);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS1, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, EN_CLK,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS1, reg);

		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DS1_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DS1_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		BDBG_MSG((" NEXUS_Frontend_3255_PowerDownFrontendDS1 end\n"));
	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerDownFrontendOOB(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownFrontendOOB start\n"));
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		/*power_down_OOB_AFEr*/
		reg = BREG_Read32(hReg, BCHP_OOB_AFE_RX_CTRL_2);
		reg &= ~(
				BCHP_MASK(OOB_AFE_RX_CTRL_2, AFERX_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(OOB_AFE_RX_CTRL_2, AFERX_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_OOB_AFE_RX_CTRL_2, reg);

		reg = BREG_Read32(hReg, BCHP_OOB_AFE_PLL_CTRL_2);
		reg &= ~(
				BCHP_MASK(OOB_AFE_PLL_CTRL_2, PLL_REFCOMP_PWRDN) |
				BCHP_MASK(OOB_AFE_PLL_CTRL_2, PLL_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(OOB_AFE_PLL_CTRL_2, PLL_REFCOMP_PWRDN,  1 ) |
				BCHP_FIELD_DATA(OOB_AFE_PLL_CTRL_2, PLL_PWRDN,  1 )
		;
		BREG_Write32(hReg, BCHP_OOB_AFE_PLL_CTRL_2, reg);

		/*power_down_OOB_Digital*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, OB_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, OB_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_OB);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_OB, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_OB, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_OB, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_OB, PWRUP_CMLBUF,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_OB, PWRUP, 0) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_OB, EN_CLK,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_OB, reg);

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownFrontendOOB end\n"));
	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerDownDOCSISDS(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownDOCSISDS start\n"));
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		/*power_down_DOCSIS_DS_MAC*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, D3DSMAC_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, D3DSMAC_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4, CLOCK_ENA,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4, reg);


		/*power_down_DS_FPM*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DSMAC_FPM_TOP_CLKEN) |
				BCHP_MASK(INT_CLKCONTROL, FPM_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DSMAC_FPM_TOP_CLKEN,  0 ) |
				BCHP_FIELD_DATA(INT_CLKCONTROL, FPM_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		/*power_down_DS_PCM*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, PCM_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, PCM_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);


		/*power_down_DS_DTP*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DTP_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DTP_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownDOCSISDS end\n"));

	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerDownDOCSISUS(
    NEXUS_FrontendHandle handle
    )
{
	BSTD_UNUSED(handle);
#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownDOCSISUS start\n"));

		/*power_down_US_Digital*/
	#if 0
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_US);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_US, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_US, EN_CLK,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_US, reg);
	#else
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_US, 0);
	#endif

		reg = BREG_Read32(hReg, BCHP_CLKGEN_USMAC_TC_CLK_PM_CTRL);
		reg &= ~(
				BCHP_MASK(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_216) |
				BCHP_MASK(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_108)
				);
		reg |=
				BCHP_FIELD_DATA(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_216,  1 )	|
				BCHP_FIELD_DATA(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_108,  1 )
		;
		BREG_Write32(hReg, BCHP_CLKGEN_USMAC_TC_CLK_PM_CTRL, reg);


		/*power_down_US_PHY*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PWRDN);
		reg &= ~(
				BCHP_MASK(US_TOP_PWRDN, BGCORE) |
				BCHP_MASK(US_TOP_PWRDN, VREG) |
				BCHP_MASK(US_TOP_PWRDN, PHASE_ITP) |
				BCHP_MASK(US_TOP_PWRDN, PLL)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PWRDN, BGCORE,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PWRDN, VREG, 0) |
				BCHP_FIELD_DATA(US_TOP_PWRDN, PHASE_ITP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PWRDN, PLL,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PWRDN, reg);

		reg = BREG_Read32(hReg, BCHP_US_PDAC0_PWRDN);
		reg &= ~(
				BCHP_MASK(US_PDAC0_PWRDN, DAC3) |
				BCHP_MASK(US_PDAC0_PWRDN, DAC2) |
				BCHP_MASK(US_PDAC0_PWRDN, DAC1) |
				BCHP_MASK(US_PDAC0_PWRDN, DAC0)
				);
		reg |=
				BCHP_FIELD_DATA(US_PDAC0_PWRDN, DAC3,  1 ) |
				BCHP_FIELD_DATA(US_PDAC0_PWRDN, DAC2,  1 ) |
				BCHP_FIELD_DATA(US_PDAC0_PWRDN, DAC1,  1 ) |
				BCHP_FIELD_DATA(US_PDAC0_PWRDN, DAC0,  1 )
		;
		BREG_Write32(hReg, BCHP_US_PDAC0_PWRDN, reg);

		/*power_down_DOCSIS_US_MAC_CLK*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, USMAC_TC_TOP_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, USMAC_TC_TOP_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3, CLOCK_ENA,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3, reg);

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownDOCSISUS end\n"));

	}
#endif

    return NEXUS_SUCCESS;

}


static NEXUS_Error NEXUS_Frontend_P_3255_PowerDownUSPLL(
    NEXUS_FrontendHandle handle
    )
{
	BSTD_UNUSED(handle);
#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;
		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownUSPLL start\n"));

		/*power_down_US_PLL*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PWRDN);
		reg &= ~(
				BCHP_MASK(US_TOP_PWRDN, VREG) |
				BCHP_MASK(US_TOP_PWRDN, PLL)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PWRDN, VREG,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PWRDN, PLL,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PWRDN, reg);

		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_RST);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_RST, PHASE) |
				BCHP_MASK(US_TOP_PLL_RST, DIGITAL) |
				BCHP_MASK(US_TOP_PLL_RST, ANALOG)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_RST, PHASE,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_RST, DIGITAL,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_RST, ANALOG,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_RST, reg);


		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerDownUSPLL end\n"));
	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_DisableMIPSClk(
    NEXUS_FrontendHandle handle
    )
{
	BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		BDBG_MSG((" NEXUS_Frontend_P_3255_DisableMIPSClk start\n"));

		/*Disable_MIPS_CLK*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, MIPS_CLK_EN) |
				BCHP_MASK(INT_CLKCONTROL, MIPS_UBUS_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, MIPS_CLK_EN,  0 ) |
				BCHP_FIELD_DATA(INT_CLKCONTROL, MIPS_UBUS_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1, CLOCK_ENA,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1, reg);


		BDBG_MSG((" NEXUS_Frontend_P_3255_DisableMIPSClk end\n"));

	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerdownDOCSISPLL(
    NEXUS_FrontendHandle handle
    )
{
	BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		/*standby_passive_DOCSIS_PLL_power_down*/

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerdownDOCSISPLL start\n"));

		/*power down Docsis UBUS and scb_bridge related to DRAM access*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2, CLOCK_ENA,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2, reg);


		/*power down Docsis RBUS*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5, CLOCK_ENA,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5, reg);


		/*Power Down Docsis PLL*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_CTRL, DRESET) |
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_CTRL, ARESET) |
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_CTRL, POWERDOWN)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, DRESET,  1 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, ARESET,  1 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, POWERDOWN,  1 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL, reg);



		BDBG_MSG((" NEXUS_Frontend_P_3255_DisableMIPSClk end\n"));

	}
#endif

    return NEXUS_SUCCESS;

}


static NEXUS_Error NEXUS_Frontend_P_3255_DisableWatchdogTimer(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);
#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		BDBG_MSG((" NEXUS_Frontend_P_3255_DisableWatchdogTimer start\n"));

		BREG_Write32(hReg, BCHP_PER_TIMER_WatchDogDefCount, 0);
		BREG_Write32(hReg, BCHP_PER_TIMER_WatchDogCtl, 0xee00);
		BREG_Write32(hReg, BCHP_PER_TIMER_WatchDogCtl, 0x00ee);

		BDBG_MSG((" NEXUS_Frontend_P_3255_DisableWatchdogTimer end\n"));
	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerdownEPHY(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerdownEPHY start\n"));

		/*power_down_EPHY*/
		BREG_Write32(hReg, BCHP_UNIMAC_INTERFACE_0_MDIO_CMD, 0x243F008B);
		BREG_Write32(hReg, BCHP_UNIMAC_INTERFACE_0_MDIO_CMD, 0x243001C0);
		BREG_Write32(hReg, BCHP_UNIMAC_INTERFACE_0_MDIO_CMD, 0x24347000);
		BREG_Write32(hReg, BCHP_UNIMAC_INTERFACE_0_MDIO_CMD, 0x243F000F);
		BREG_Write32(hReg, BCHP_UNIMAC_INTERFACE_0_MDIO_CMD, 0x243020D0);
		BREG_Write32(hReg, BCHP_UNIMAC_INTERFACE_0_MDIO_CMD, 0x243F000B);

		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, ENET25_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, ENET25_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, CLOCK_ENA) |
				BCHP_MASK(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, DIS_CH) |
				BCHP_MASK(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, EN_CMLBUF)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, CLOCK_ENA,  0 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, DIS_CH,  1 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, EN_CMLBUF,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, reg);

		/*power_down_ENET_UNIMAC*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, PER_UNIMAC_ARB_CLK_EN) |
				BCHP_MASK(INT_CLKCONTROL, UNIMAC0_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, PER_UNIMAC_ARB_CLK_EN,  0 ) |
				BCHP_FIELD_DATA(INT_CLKCONTROL, UNIMAC0_CLK_EN,  0 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		BDBG_MSG((" NEXUS_Frontend_P_3255_PowerdownEPHY end\n"));

	}
#endif

	return NEXUS_SUCCESS;

}



NEXUS_Error NEXUS_Frontend_3255_PowerDownFrontendDevices(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_Frontend_P_3255_DisableWatchdogTimer(handle);
    NEXUS_Frontend_P_3255_PowerDownFrontendDS1(handle);
    NEXUS_Frontend_P_3255_PowerDownFrontendDS0(handle);
    NEXUS_Frontend_P_3255_PowerDownFrontendOOB(handle);
    NEXUS_Frontend_P_3255_PowerDownDOCSISDS(handle);
    NEXUS_Frontend_P_3255_PowerDownDOCSISUS(handle);
    NEXUS_Frontend_P_3255_PowerDownUSPLL(handle);
    NEXUS_Frontend_P_3255_PowerdownEPHY(handle);
    NEXUS_Frontend_P_3255_DisableMIPSClk(handle);
    NEXUS_Frontend_P_3255_PowerdownDOCSISPLL(handle);

    return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_Frontend_P_3255_PowerUpFrontendDS0(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		BDBG_MSG((" NEXUS_Frontend_3255_PowerUpFrontendDS0 start\n"));

		/*DS_0.power_up_DS0_receiver*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS0);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS0, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS0, reg);

		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DS0_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DS0_CLK_EN,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);


		/*DS0.power_up_DS0_AFE*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE0);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE0, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE0, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE0, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE0, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE0, PWRUP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE0, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE0, reg);

		reg = BREG_Read32(hReg, BCHP_DS_QDSAFE_0_0_CTRL1);
		reg &= ~(
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PD_ADC) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PWDN_BG) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, CLK_RESET)
				);
		reg |=
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PD_ADC,  0 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PWDN_BG,  0 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, CLK_RESET,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_QDSAFE_0_0_CTRL1, reg);

		/*DS_0.power_up_DS0_Tuner*/
		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_03);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_03, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN,  0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_04, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_05);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_05, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_06);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_PWRUP_LF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_QP2_PD) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_MANIBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_QP_PD) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_VC_PRB_EN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_VZ_PRB_EN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_PWRUP_LF,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_QP2_PD, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_MANIBAL,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_QP_PD,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_VC_PRB_EN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_VZ_PRB_EN,  1)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_06, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_07);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IP_PD_LO) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IP_PD_LO, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN,  0)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_07, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_14);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_0_WDATA_14, reg);


		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_02, I_OUTCML_PWRDN_M)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_02, I_OUTCML_PWRDN_M,  0x7c )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_00);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP, 1) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP,  1)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_00, reg);


		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_QP_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_QP_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_0_REF_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_0_REF_04, reg);



		BDBG_MSG((" NEXUS_Frontend_3255_PowerUpFrontendDS0 end\n"));

	}
#endif

    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_PowerUpFrontendDS1(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);

#if (BCHP_VER>=BCHP_VER_C0)
	{
		BREG_Handle hReg = g_pCoreHandles->reg;
		uint32_t reg;

		BDBG_MSG((" NEXUS_Frontend_3255_PowerUpFrontendDS0 start\n"));

		/*DS_1.power_up_DS1_receiver*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS1);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS1, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS1, reg);

		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DS1_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DS1_CLK_EN,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);


		/*DS1.power_up_DS1_AFE*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE1);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE1, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE1, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS_AFE1, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE1, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE1, PWRUP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS_AFE1, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS_AFE1, reg);

		reg = BREG_Read32(hReg, BCHP_DS_QDSAFE_0_1_CTRL1);
		reg &= ~(
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PD_ADC) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, PWDN_BG) |
				BCHP_MASK(DS_QDSAFE_0_0_CTRL1, CLK_RESET)
				);
		reg |=
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PD_ADC,  0 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, PWDN_BG,  0 ) |
				BCHP_FIELD_DATA(DS_QDSAFE_0_0_CTRL1, CLK_RESET,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_QDSAFE_0_1_CTRL1, reg);

		/*DS_1.power_up_DS1_Tuner*/
		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_01, DPM_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, PGALPF_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_02, MIX_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_03);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, PGA_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFF, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFI,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_03, IFLPF_PWROFFQ,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_03, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, OUTVGA_PWRDN,  0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, DCO_PRE_IQ_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, I_DCO_PRE_IQ_PWRDN_VBAL,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, REG1P2V_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_IDCO_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_04, VGA_QDCO_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_04, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_05);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, PWRDN_BG_1P2,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_VCO_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_05, IP_PWRUP_WD,  1 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_05, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_06);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_PWRUP_LF) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_QP2_PD) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_MANIBAL) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_QP_PD) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_VC_PRB_EN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_06, IP_VZ_PRB_EN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_PWRUP_LF,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_QP2_PD, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_MANIBAL,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_QP_PD,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_VC_PRB_EN, 1) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_06, IP_VZ_PRB_EN,  1)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_06, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_07);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IP_PD_LO) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN) |
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, DACIMFREG_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IP_PD_LO, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, IMF_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, I_DAC_PWRUP,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, CML1200_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_07, D2D1200_PWRDN,  0)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_07, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_14);
		reg &= ~(
				BCHP_MASK(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_ANACTL_0_WDATA_14, MXR_DCODAC_PWRDN_1P2,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_ANACTL_1_WDATA_14, reg);


		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_02);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_02, I_OUTCML_PWRDN_M)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_02, I_OUTCML_PWRDN_M,  0x7c )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_02, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_00);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_MUX_PWRUP,  1 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_UGB_PWRUP, 1) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_REFBUF_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FB_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV4_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_DIV_FBREG_PWRDN, 0) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_00, I_VCO_PWRUP,  1)
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_00, reg);


		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_01);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_QP_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_REGQPPFD_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_QP_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_01, I_OUTDIV_REG_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_01, reg);

		reg = BREG_Read32(hReg, BCHP_DS_TUNER_REF_1_REF_04);
		reg &= ~(
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN) |
				BCHP_MASK(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTDIV_IGEN_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_IGEN_PWRDN,  0 ) |
				BCHP_FIELD_DATA(DS_TUNER_REF_0_REF_04, I_OUTCML_DIODE_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_DS_TUNER_REF_1_REF_04, reg);



		BDBG_MSG((" NEXUS_Frontend_3255_PowerUpFrontendDS1 end\n"));

	}
#endif

    return NEXUS_SUCCESS;

}

NEXUS_Error NEXUS_Frontend_3255_PowerUpFrontendDevices(
    NEXUS_FrontendHandle handle
    )
{
#if (BCHP_VER>=BCHP_VER_C0)
	BREG_Handle hReg = g_pCoreHandles->reg;
	uint32_t reg;

	BDBG_MSG((" NEXUS_Frontend_3255_PowerUpFrontendDevices start\n"));
	/*Power Down Docsis PLL*/
	reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL);
	if (reg & BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, POWERDOWN,  1 ))
	{
		BDBG_WRN((" NEXUS_Frontend_3255_PowerUpFrontendDevices(): powering on.\n"));
		/*bring up bus clocks to VCXO*/
		reg = BREG_Read32(hReg, BCHP_CLKGEN_USMAC_TC_CLK_PM_CTRL);
		reg &= ~(
				BCHP_MASK(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_216) |
				BCHP_MASK(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_108)
				);
		reg |=
				BCHP_FIELD_DATA(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_216,  0 )	|
				BCHP_FIELD_DATA(CLKGEN_USMAC_TC_CLK_PM_CTRL, DIS_CLK_108,  0 )
		;
		BREG_Write32(hReg, BCHP_CLKGEN_USMAC_TC_CLK_PM_CTRL, reg);

		/* resume DOCSIS PLL*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_CTRL, ARESET) |
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_CTRL, POWERDOWN)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, ARESET,  0 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, POWERDOWN, 0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL, reg);

		BKNI_Sleep(2);

		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_CTRL, DRESET)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_CTRL, DRESET,  0 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_CTRL, reg);


		/*Resume BNM MIPS clock*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1, CLOCK_ENA,  1)
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_1, reg);

		/*Resume UBUS and SCB_BRIDGE clock*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2, CLOCK_ENA,  1)
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_2, reg);

		/*Resume DOCSIS US MAC clock*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3, CLOCK_ENA,  1 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_3, reg);

		/*Resume DOCSIS DS MAC clock*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4, CLOCK_ENA,  1 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_4, reg);

		/*Resume RBUS clock*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5, CLOCK_ENA)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5, CLOCK_ENA,  1 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_DOCSIS_PM_DIS_CHL_5, reg);

		/*Bringign up the US PLL*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PWRDN);
		reg &= ~(
				BCHP_MASK(US_TOP_PWRDN, VREG) |
				BCHP_MASK(US_TOP_PWRDN, PLL)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PWRDN, VREG,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PWRDN, PLL,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PWRDN, reg);

		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_RST);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_RST, PHASE) |
				BCHP_MASK(US_TOP_PLL_RST, DIGITAL) |
				BCHP_MASK(US_TOP_PLL_RST, ANALOG)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_RST, PHASE,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_RST, DIGITAL,  0 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_RST, ANALOG,  0 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_RST, reg);

		/*bring up below in order to access OOB*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, D3DSMAC_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, D3DSMAC_CLK_EN,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);


		/*power_up_OOB_Digital*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, OB_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, OB_CLK_EN,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_OB);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_OB, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_OB, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_OB, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_OB, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_OB, PWRUP, 1) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_OB, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_OB, reg);

		/*power_up_OOB_AFEr*/
		reg = BREG_Read32(hReg, BCHP_OOB_AFE_RX_CTRL_2);
		reg &= ~(
				BCHP_MASK(OOB_AFE_RX_CTRL_2, AFERX_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(OOB_AFE_RX_CTRL_2, AFERX_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_OOB_AFE_RX_CTRL_2, reg);

		reg = BREG_Read32(hReg, BCHP_OOB_AFE_PLL_CTRL_2);
		reg &= ~(
				BCHP_MASK(OOB_AFE_PLL_CTRL_2, PLL_REFCOMP_PWRDN) |
				BCHP_MASK(OOB_AFE_PLL_CTRL_2, PLL_PWRDN)
				);
		reg |=
				BCHP_FIELD_DATA(OOB_AFE_PLL_CTRL_2, PLL_REFCOMP_PWRDN,  0 ) |
				BCHP_FIELD_DATA(OOB_AFE_PLL_CTRL_2, PLL_PWRDN,  0 )
		;
		BREG_Write32(hReg, BCHP_OOB_AFE_PLL_CTRL_2, reg);

		/*recover MIPS clocks controlled by PERIPH block*/
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, MIPS_CLK_EN ) |
				BCHP_MASK(INT_CLKCONTROL, MIPS_UBUS_CLK_EN )
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, MIPS_CLK_EN ,  1 ) |
				BCHP_FIELD_DATA(INT_CLKCONTROL, MIPS_UBUS_CLK_EN ,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);

		/*Resume BNM EPHY clocks:*/
		reg = BREG_Read32(hReg, BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2);
		reg &= ~(
				BCHP_MASK(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, CLOCK_ENA) |
				BCHP_MASK(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, DIS_CH) |
				BCHP_MASK(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, EN_CMLBUF)
				);
		reg |=
				BCHP_FIELD_DATA(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, CLOCK_ENA,  1 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, DIS_CH,  0 ) |
				BCHP_FIELD_DATA(VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, EN_CMLBUF,  1 )
		;
		BREG_Write32(hReg, BCHP_VCXO_CTL_MISC_RAP_AVD_PLL_CHL_2, reg);
		NEXUS_Frontend_P_3255_PowerUpFrontendDS0(handle);
		NEXUS_Frontend_P_3255_PowerUpFrontendDS1(handle);
	
		BDBG_MSG((" NEXUS_Frontend_3255_PowerUpFrontendDevices end\n"));
	}
	else
	{
		BDBG_WRN(("NEXUS_Frontend_3255_PowerUpFrontendDevices() skipped, already powered on.\n"));
	}
#endif
	BSTD_UNUSED(handle);

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Frontend_3255_TransferFrontendControlToHost(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_MSG(("NEXUS_Frontend_3255_TransferFrontendControlToHost docsis device status %d\n", pDevice->status));



    if(pDevice->status == NEXUS_DocsisStatus_eOperational){
        BERR_Code retVal, retCode;


        {
            BRPC_Param_ECM_Transit_Frontend_Control_to_Host Param;

            Param.devId = BRPC_DevId_3255;

            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ECM_Transit_Frontend_Control_to_Host, (const uint32_t *)&Param, sizeof(Param)/4, NULL, 0, &retVal);

            /*NEXUS_Frontend_3255_EnableRPC(handle, false);*/

            BDBG_MSG(("frontend pwm send BRPC_ProcId_ECM_Transit_Frontend_Control_to_Host message ... retCode %d, retVal %d\n", retCode, retVal));
            if(retCode != BERR_SUCCESS || retVal != BERR_SUCCESS)
            {
                BDBG_WRN(("frontend pwm send BRPC_ProcId_ECM_Transit_Frontend_Control_to_Host message, RPC call failed\n"));
                return NEXUS_UNKNOWN;
            }
        }


        {
            BRPC_Param_ECM_PowerSaver Param;
            Param.devId = BRPC_DevId_3255;
            Param.mode = BRPC_ECM_PowerMode_Off;    /* let BNM have chance to do something before entering power mode off */

            BDBG_MSG(("frontend pwm send BRPC_ProcId_ECM_PowerSaver message ...\n"));

            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ECM_PowerSaver, (const uint32_t *)&Param, sizeof(Param)/4, NULL, 0, &retVal);

            if(retCode != BERR_SUCCESS || retVal != BERR_SUCCESS)
            {
                BDBG_WRN(("frontend pwm send BRPC_ProcId_ECM_PowerSaver message, RPC call failed\n"));
                return NEXUS_UNKNOWN;
            }
        }



        pDevice->heartbeat_enabled = false;
        pDevice->status = NEXUS_DocsisStatus_ePowerSave;

    }
    else
        return NEXUS_NOT_SUPPORTED;

    return NEXUS_SUCCESS;
}



NEXUS_Error NEXUS_Frontend_3255_TransferFrontendControlToCM(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_MSG(("NEXUS_Frontend_3255_TransferFrontendControlToCM docsis device status %d\n", pDevice->status));

    if(1){
        BERR_Code retVal, retCode;

        {
            BRPC_Param_ECM_Transit_Frontend_Control_to_Bnm Param;

            Param.devId = BRPC_DevId_3255;


            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ECM_Transit_Frontend_Control_to_Bnm, (const uint32_t *)&Param, sizeof(Param)/4, NULL, 0, &retVal);
            BDBG_MSG(("frontend pwm send BRPC_ProcId_ECM_Transit_Frontend_Control_to_Bnm message ... retCode %d, retVal %d\n", retCode, retVal));
            if(retCode != BERR_SUCCESS || retVal != BERR_SUCCESS)
            {
                BDBG_WRN(("frontend pwm send BRPC_ProcId_ECM_Transit_Frontend_Control_to_BNM message, RPC call failed\n"));
                return NEXUS_UNKNOWN;
            }
        }

        /*BNM wants me to re-init this again*/
        {
            NEXUS_Error errCode;
            int i = 0;

            BDBG_ERR((" eCM' wake up. Re-initialize ADS/AOB/AUS "));
            NEXUS_Frontend_3255_ChannelClose(pDevice);
            errCode = NEXUS_Frontend_P_Init3255(pDevice);
            if (errCode) {
                BDBG_ERR((" Error in Re-initialize ADS/AOB/AUS, after BNM wakeup from powersave mode\n "));
                return NEXUS_UNKNOWN;
            } else {
#if NEXUS_MAX_3255_ADSCHN
                for ( i = 0; i < NEXUS_MAX_3255_ADSCHN && NULL != pDevice->ads_chn[i]; i++)
                {
                    BADS_InstallCallback(pDevice->ads_chn[i], BADS_Callback_eLockChange,
                        (BADS_CallbackFunc)NEXUS_Frontend_P_3255_ADS_lockchange, (void*)pDevice->lockAppCallback[i]);
                }
#endif
#if B_HAS_OOB
                if (pDevice->aob)
                {
                    BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eLockChange,
                        (BAOB_CallbackFunc)NEXUS_Frontend_P_3255_AOB_lockchange, (void*)pDevice->lockAppCallback[NEXUS_3255_OOB_CHN]);
                }
#endif
            }
            NEXUS_TaskCallback_Fire(pDevice->eCMStatusCallback);
        }
#if 0
        if(0){
            BRPC_Param_ECM_PowerSaver Param;
            Param.devId = BRPC_DevId_3255;
            Param.mode = BRPC_ECM_PowerMode_On;    /* at this time BNM is already on, so may this is not necessary */

            retCode = BRPC_CallProc(pDevice->rpc_handle, BRPC_ProcId_ECM_PowerSaver, (const uint32_t *)&Param, sizeof(Param)/4, NULL, 0, &retVal);
            if(retCode != BERR_SUCCESS || retVal != BERR_SUCCESS)
            {
                BDBG_WRN(("frontend pwm send BRPC_ProcId_ECM_PowerSaver message, RPC call failed\n"));
                return NEXUS_UNKNOWN;
            }
        }
#endif
    }else
        return NEXUS_NOT_SUPPORTED;

    return NEXUS_SUCCESS;

}

NEXUS_Error NEXUS_Frontend_3255_IsCMOperational(
    NEXUS_FrontendHandle handle,
    bool * pStatus
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);

    BDBG_MSG(("NEXUS_Frontend_3255_IsCMOperational docsis device status %d, heartbeat enabled %d\n", pDevice->status, pDevice->heartbeat_enabled));
    if (pDevice->status == NEXUS_DocsisStatus_eOperational) * pStatus = true;
    else * pStatus = false;

    return (NEXUS_SUCCESS);

}

NEXUS_Error NEXUS_Frontend_3255_EnableRPC(
    NEXUS_FrontendHandle handle,
    bool toEnable
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);

    BRPC_EnableRPC(pDevice->rpc_handle, toEnable);

    return (NEXUS_SUCCESS);

}

NEXUS_Error NEXUS_Frontend_3255_EnableHeartBeat(
    NEXUS_FrontendHandle handle,
    bool toEnable
    )
{
    NEXUS_3255Channel *channelHandle = handle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT( pDevice->rpc_handle);

    pDevice->heartbeat_enabled = toEnable;

    return (NEXUS_SUCCESS);

}

#else
NEXUS_Error NEXUS_Frontend_3255_PowerDownFrontendDevices(
    NEXUS_FrontendHandle handle
    )
{
       BSTD_UNUSED(handle);
	return NEXUS_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Frontend_3255_PowerUpFrontendDevices(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NEXUS_NOT_SUPPORTED;
}


NEXUS_Error NEXUS_Frontend_3255_TransferFrontendControlToHost(
    NEXUS_FrontendHandle handle
    )
{
        BSTD_UNUSED(handle);
        return NEXUS_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Frontend_3255_TransferFrontendControlToCM(
    NEXUS_FrontendHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NEXUS_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Frontend_3255_IsCMOperational(
    NEXUS_FrontendHandle handle,
    bool * pStatus
    )
{
    BSTD_UNUSED(handle);
    *pStatus = true;
    return NEXUS_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Frontend_3255_EnableRPC(
    NEXUS_FrontendHandle handle,
    bool toEnable
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(toEnable);
    return NEXUS_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_Frontend_3255_EnableHeartBeat(
    NEXUS_FrontendHandle handle,
    bool toEnable
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(toEnable);
    return NEXUS_NOT_SUPPORTED;
}

#endif

#if B_HAS_OOB
static void NEXUS_Frontend_P_3255_UnTuneOOB(void *handle)
{
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(channelHandle->chn_num == NEXUS_3255_OOB_CHN);

    pDevice->tune_started[NEXUS_3255_OOB_CHN] = false;
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return;

#if 0 /*TODO:: not supported in 325x side yet*/
    rc = BAOB_EnablePowerSaver(pDevice->aob);
    if (rc!=BERR_SUCCESS) { BDBG_ERR((" error in BAOB_EnablePowerSaver"));}
#endif
}

static NEXUS_Error NEXUS_Frontend_P_3255_TuneOOB(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings)
{
    BERR_Code rc;
    BAOB_ModulationType modType;
    BAOB_SpectrumMode spectrum;
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(NULL != pSettings);
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    BDBG_ASSERT(channelHandle->chn_num == NEXUS_3255_OOB_CHN);

    switch (pSettings->mode) {
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
        return NEXUS_INVALID_PARAMETER;
    }

    switch (pSettings->spectrum) {
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
        return NEXUS_INVALID_PARAMETER;
    }

    NEXUS_TaskCallback_Set(pDevice->lockAppCallback[NEXUS_3255_OOB_CHN], &(pSettings->lockCallback));

    if ( pDevice->tnr[NEXUS_3255_OOB_CHN])
    {
        rc = BTNR_SetTunerRfFreq(pDevice->tnr[NEXUS_3255_OOB_CHN], pSettings->frequency, BTNR_TunerMode_eDigital);
        if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}
    }

    rc = BAOB_Set_Spectrum(pDevice->aob, spectrum);
    if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}
#ifdef NEXUS_FRONTEND_ACCUMULATE_STATISTICS
	pDevice->oobFecCorrected = 0;
	pDevice->oobFecUnCorrected = 0;
	pDevice->oobBerErrorCount = 0;
#endif

    if (BAOB_Acquire(pDevice->aob, modType, pSettings->symbolRate)) { return (NEXUS_Error)BERR_TRACE(rc);}

    pDevice->tune_started[NEXUS_3255_OOB_CHN] = true;
    pDevice->last_aob = *pSettings;
    return NEXUS_SUCCESS;

}

static NEXUS_Error NEXUS_Frontend_P_3255_GetOOBStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus)
{
    BERR_Code  rc;
    struct BAOB_Status st;
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(channelHandle->chn_num == NEXUS_3255_OOB_CHN);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    rc = BAOB_GetStatus(pDevice->aob,  &st);
    if (rc!=BERR_SUCCESS){ return (NEXUS_Error)BERR_TRACE(rc);}
#ifdef NEXUS_FRONTEND_ACCUMULATE_STATISTICS
	pDevice->oobFecCorrected += st.correctedCount;
	pDevice->oobFecUnCorrected += st.uncorrectedCount;
	pDevice->oobBerErrorCount += st.berErrorCount;
    pStatus->correctedCount = pDevice->oobFecCorrected;
    pStatus->uncorrectedCount = pDevice->oobFecUnCorrected;
	pStatus->berErrorCount = pDevice->oobBerErrorCount;
#else	
    pStatus->correctedCount = st.correctedCount;
    pStatus->uncorrectedCount = st.uncorrectedCount;
	pStatus->berErrorCount = st.berErrorCount;
#endif

    pStatus->isFecLocked = st.isFecLock;
    pStatus->isQamLocked = st.isQamLock;
    pStatus->symbolRate = st.symbolRate;
    pStatus->snrEstimate = st.snrEstimate*100/256;
    pStatus->agcIntLevel = st.agcIntLevel;
    pStatus->agcExtLevel = st.agcExtLevel;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;
	pStatus->fdcChannelPower = st.fdcChannelPower;

    pStatus->settings = pDevice->last_aob;
    BDBG_MSG((" OOB STATUS : fec_lock %d \t qam_lock %d \t  agcIntLevel %d \t st.agcExtLevel %d \t snr_estimate %d \n\t fec_corr_cnt %d \t fec_uncorr_cnt %d\t ber_estimate %d\t fdcChannelPower %d",
            st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel, st.snrEstimate,
            st.correctedCount, st.uncorrectedCount, st.berErrorCount, st.fdcChannelPower));

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_3255_GetOutOfBandLockStatus(NEXUS_FrontendHandle frontHandle, bool *locked)
{
    BERR_Code  rc;
    NEXUS_3255Channel *channelHandle = frontHandle->pDeviceHandle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(channelHandle->chn_num == NEXUS_3255_OOB_CHN);

    *locked = false;
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BAOB_GetLockStatus(pDevice->aob,  locked);
    if (rc!=BERR_SUCCESS){ return (NEXUS_Error)BERR_TRACE(rc);}
    return NEXUS_SUCCESS;
}
static NEXUS_Error NEXUS_Frontend_P_3255_TuneUs(void *handle, const NEXUS_FrontendUpstreamSettings *pSettings)
{
    BERR_Code rc;
    BAUS_OperationMode mode;
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;


    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(channelHandle->chn_num == NEXUS_3255_OOB_CHN);
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);

    switch (pSettings->mode) {
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
        return NEXUS_NOT_SUPPORTED;
    }

    rc = BAUS_SetOperationMode(pDevice->aus, mode);
    if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}

    if (mode != BAUS_OperationMode_eDocsis) {
        rc = BAUS_SetRfFreq(pDevice->aus, pSettings->frequency);
        if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}

        rc = BAUS_SetSymbolRate(pDevice->aus, pSettings->symbolRate);
        if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}

        rc = BAUS_SetPowerLevel(pDevice->aus, pSettings->powerLevel);
        if (rc!=BERR_SUCCESS) { return (NEXUS_Error)BERR_TRACE(rc);}
    }

    pDevice->last_aus = *pSettings;
    return NEXUS_SUCCESS;

}
static NEXUS_Error NEXUS_Frontend_P_3255_GetUsStatus(void *handle, NEXUS_FrontendUpstreamStatus *pStatus)
{
    BERR_Code  rc;
    struct BAUS_Status st;
    NEXUS_3255Channel *channelHandle = handle;
    NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);
    BDBG_ASSERT(channelHandle->chn_num == NEXUS_3255_OOB_CHN);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    if (pDevice->status != NEXUS_DocsisStatus_eOperational) return (NEXUS_NOT_SUPPORTED);
    rc = BAUS_GetStatus(pDevice->aus,  &st);
    if (rc!=BERR_SUCCESS){ return (NEXUS_Error)BERR_TRACE(rc);}

    switch (st.operationMode) {
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
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    pStatus->frequency = st.rfFreq;
    pStatus->symbolRate = st.symbolRate;
    pStatus->powerLevel = st.powerLevel;
    pStatus->sysXtalFreq = st.sysXtalFreq;

    return NEXUS_SUCCESS;
}

#endif


#if NEXUS_MAX_3255_ADSCHN
/***************************************************************************
Summary:
    Reset the status of a 3255 QAM tuner
***************************************************************************/
static void NEXUS_Frontend_P_3255_ResetQamStatus(
    void *handle
    )
{
	BERR_Code rc;
	NEXUS_3255Channel *channelHandle = handle;
	NEXUS_3255 *pDevice = (NEXUS_3255 *)channelHandle->pDevice;
	unsigned chn_num = channelHandle->chn_num;

	BDBG_OBJECT_ASSERT(pDevice, NEXUS_3255);

	rc = BADS_ResetStatus(pDevice->ads_chn[chn_num]);

	pDevice->ads_status[chn_num].fecCorrected = 0;
	pDevice->ads_status[chn_num].fecUnCorrected = 0;
	pDevice->ads_status[chn_num].ulGoodRsBlockCount = 0;

	return;
}
#endif

