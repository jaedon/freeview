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
* $brcm_Workfile: nexus_frontend_31xx.c $
* $brcm_Revision: 85 $
* $brcm_Date: 9/7/12 1:57p $
*
* API Description:
*   API name: Frontend 31xx
*    APIs to open, close, and setup initial settings for a BCM31xx
*    Frontend Device.
*
* Revision History:
*
* $brcm_Log: X:/win_views/97231/nexus/modules/frontend/3117/src/nexus_frontend_31xx.c $
* 
* 85   9/7/12 1:57p vishk
* SW7425-3765, SW7425-3788: Remove symbolic links in pi
* 
* 84   8/27/12 3:34p vishk
* SW7425-3825: Coverity Defect ID:21038 OVERRUN_STATIC
*  Function=NEXUS_Frontend_P_31xx_Standby() Product=97425
* 
* 83   8/22/12 3:10p vishk
* SW7425-3519: Add standby support for 31xx frontend.
* 
* 82   7/2/12 4:58p vishk
* SW7425-3394: Coverity Defect ID:10869 OVERRUN_STATIC
*  Function=NEXUS_Frontend_P_31xx_RequestQamAsyncStatus() Product=97425
* 
* 81   6/21/12 4:19p vishk
* SW7425-2983: Make the image name same across all versions.
* 
* 80   6/20/12 5:40p vishk
* SW7231-589: Set the function pointer for async status callback.
* 
* 79   6/20/12 4:32p vishk
* SW7405-5670: Add not supported warning for nexus scan_qam api for 3117
*  family.
* 
* 78   6/12/12 1:47p vishk
* SW7405-5669: Clean up retrun codes.
* 
* 77   2/28/12 11:54a vishk
* SW7231-691: Correct implementation of reading chip id and rev id.
* 
* 76   1/13/12 4:18p vishk
* SW7231-589: Extend NEXUS_Frontend_GetFastStatus() for 3109 frontends
* 
* 75   1/9/12 12:31p vishk
* SW7425-1786: all chip-specific frontend API's must validate against an
*  always-available chip id before dereferencing chip-specific data
*  structures
* 
* 74   1/4/12 11:28a vishk
* SW7425-1996:  Coverity Defect ID:36303 OVERRUN_STATIC
*  nexus_frontend_31xx.c Product=97425
* 
* 73   12/9/11 12:03p mward
* SW7425-1851: ISO C90 forbids mixed declarations and code
* 
* 72   12/5/11 9:53p vishk
* SW7425-1851: Fix coverity defects in nexus frontend
* 
* 71   11/3/11 4:00p vishk
* SW3461-80: Means of returning BCM31xx chip revsion and firmware
*  revision via NEXUS i/f
* 
* 70   7/6/11 3:35p vishk
* SW7125-983: SW7125-1017: [Kbro 7019] Tuner 3109 AGC Return Value Not
*  Accurate Accumulated counters for 7125 & 3255 frontends
* 
* 69   7/6/11 3:18p vishk
* SW7125-983: SW7125-1017: [Kbro 7019] Tuner 3109 AGC Return Value Not
*  Accurate Accumulated counters for 7125 & 3255 frontends
* 
* 68   6/8/11 3:38p vishk
* SW7231-180: Remove duplicate member variables in bads.h and convert
*  BADS_AutomaticFrequencyOffset and NEXUS_FrontendQamFrequencyOffset
*  enum types to unsigned integer.
* 
* 67   6/3/11 9:39a vishk
* SW7400-1771: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
*  Connect berRawCount status member.
* 
* 66   5/31/11 5:42p vishk
* SW7400-1771: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
*  Connect GoodRsBlockCount status member.
* 
* 65   4/15/11 4:34p vishk
* SW7422-413: Nexus support for the 3128 OOB core for 7422 board
* 
* 64   3/10/11 6:27p vishk
*  SW7231-67: Unable to tune QAM-256
* 
* 63   3/7/11 3:01p mward
* SW7125-737:  Call BHAB_InitAp() with NULL AP image to indicate reset,
*  but not reload, the AP..
* 
* 62   3/4/11 3:36p erickson
* SW7422-171: change NEXUS_Frontend_31xx_SetConfigSettings pSettings to
*  const
*
* 61   12/2/10 7:49p VISHK
* SW7405-5037: To make the i2c address programmable for the slave
*  device(LNA) of 31xx frontend chip.
*
* 60   12/2/10 7:47p VISHK
* SW7405-5037: To make the i2c address programmable for the slave
*  device(LNA) of 31xx frontend chip.
*
* 59   12/2/10 7:28p VISHK
* SW7405-5037: To make the i2c address programmable for the slave
*  device(LNA) of 31xx frontend chip.
*
* 58   11/17/10 7:19p VISHK
* SW7422-71: Add support for GPIO interrupts to 3112 frontend tuners
*
* 57   11/3/10 2:37p VISHK
* SW7125-673: Coverity Defect ID:28783 OVERRUN_STATIC
*  nexus_frontend_31xx.c Product=97125
*
* 56   11/2/10 6:09p VISHK
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 55   10/30/10 12:41p mward
* SW7420-1020: Compiler warnings.
*
* 54   10/30/10 12:34p mward
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 53   10/28/10 4:25p VISHK
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 52   10/22/10 4:09p VISHK
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 51   10/21/10 6:45p vishk
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 51   10/21/10 6:45p vishk
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 50   10/21/10 4:28p mward
* SW7125-674:  Corrected goto err_gain_cbk_create
*
* 49   10/7/10 10:58a mward
* SW7125-35: Add loadAP for 7125.  Coverity fixes.
*
* 48   10/5/10 7:35p VISHK
* SW7405-4915: Add nexus api to control power saver mode in downstream
*  demod and tuner, out of banddemod and tuner and upstream. Also add
*  nexus api to control daisy chaining of the frontends.
*
* 47   9/27/10 11:51a VISHK
* SW7420-891: Add support to program TNR AGC gain at Nexus layer
*
* 46   9/21/10 6:08p VISHK
* SW7405-4889: Expose Ads and Aob open drain options in nexus
*
* 45   9/8/10 11:01a VISHK
* SW7420-1049: Add isr callbacks for BHAB_Interrupt_eUpdateGain/eNoSignal
*
* 44   9/7/10 10:16p VISHK
* SW7420-1049: Add isr callbacks for BHAB_Interrupt_eUpdateGain/eNoSignal
*
* 43   9/1/10 3:16p VISHK
* SW7400-2900: Support from Nexus (3255 Frontend OOB) to retrieve the
*  Power and BER from the OOB tuner to support Opencable MIB requirements
*
* 42   8/24/10 11:32a VISHK
* SW7405-4808 : Connect reset status api for ADS and AOB to nexus api.
*
* 42   8/24/10 11:32a VISHK
* SW7405-4808 : Connect reset status api for ADS and AOB to nexus api.
*
* 41   8/10/10 1:51p VISHK
* SW7420-891: Add support to program TNR AGC gain at Nexus layer
*
* 40   8/5/10 4:37p VISHK
* SW7405-4722: Add support for "Frequency offset" and "Null packets" for
*  ADS and expose "auto acquire" in nexus for AOB
*
* 39   7/14/10 3:21p VISHK
* SW7420-891: Add support to program TNR AGC gain at Nexus layer
*
* 38   6/10/10 6:37p mward
* SW7125-35:  Correct scaling of SNR estimate.  Added delay for 97125
*  platform.
*
* 37   5/7/10 3:33p VISHK
* SW7420-735: 3112's with the same i2c address but different i2c channels
*  will alias
*
* 36   3/8/10 8:43p VISHK
* SW7405-3995: Support spectrum mode and spectrum inversion control from
*  nexus.
*
* 35   2/3/10 1:35p garetht
* SW7420-569: Tuning the 3112 causes an exception with kernel mode driver
*  on SMS/VMS platform
*
* 34   1/15/10 1:58p VISHK
* SW7550-135: Support 7550 QAM/DVB-C support in Nexus
*
* 33   12/15/09 11:53a mphillip
* SW7405-3596: Expose QAM auto acquire setting as a user-configurable
*  parameter
*
* 32   10/5/09 10:52a vishk
* SW7420-362, SW7405-2851: nnedto support physical interrupt line sharing
*  amongst multiple frontends.
* 31xx: Add support to configure the Watchdog feature of 31xx.
*
* 32   10/5/09 10:50a vishk
* SW7420-362, SW7405-2851:  need to support physical interrupt line
*  sharing amongst multiple frontends.
* 31xx: Add support to configure the Watchdog feature of 31xx
*
* 31   9/22/09 2:41p vishk
* SW7405-2852: 31xx: ADS add support configure of Parallel or Serial
*  Downstream interface.
*
* For A0, the DS-data does not work so always the GPIO[0] is used to
*  carry the transport data.
*
* 30   9/16/09 2:23p vishk
* SW7420-350: 3112 frontend support
*
* 28   8/26/09 11:10a vishk
* SW7405-2846: Bcm31xx: Nexus API, add support for "test" modulations
*
* 27   8/18/09 2:36p vishk
* PR 57767: add support for constellation for 31xx frontends.
*
* 26   8/17/09 4:23p vishk
* PR 57767: add support for constellation for 31xx frontends.
*
* 25   8/13/09 4:25p vishk
* PR 57518: Bcm31xx: Nexus API,  add support for "test" modulations
*
* 24   8/13/09 4:10p vishk
* PR 57518: Bcm31xx: Nexus API,  add support for "test" modulations
*
* 23   8/11/09 4:49p vishk
* PR 57480: Bcm31xx: Add support Bcm31xx B0 chips
*
* 22   8/3/09 4:55p vishk
* PR 56572:  Upstream support for 3117
*
* 21   7/31/09 6:06p vishk
* PR 56572: Upstream support for 3117
*
* 20   7/8/09 3:13p erickson
* PR56607: remove unused NEXUS_Frontend_P_Register function
*
* 19   6/26/09 1:42p vishk
* PR 56235: Add support to set IfFreq in AOB PI for 3117
* PR 56288: Add support to set OOB AGC pin mode for 3117
*
* 18   6/16/09 4:43p vishk
* PR 55960: Need to query spectrumInverted setting for BCM31xx
*
* 17   6/1/09 3:34p vishk
* PR 53208: 3117 AP does not initialize on firs attemp consistently
*
* 15   4/17/09 5:02p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 14   4/8/09 9:18a vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 13   4/7/09 7:02p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 12   4/7/09 6:28p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 11   3/11/09 7:13p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 10   3/11/09 7:09p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 9   3/11/09 6:45p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 8   3/10/09 1:46p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 7   3/9/09 5:00p vishk
* PR46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 6   3/5/09 9:21a vishk
* PR46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 5   3/4/09 8:52p vishk
* PR46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 4   3/3/09 4:01p vishk
* PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
*
* 3   2/12/09 2:38p vishk
* PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
*
* 2   1/29/09 7:54p vishk
* PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
*
* 1   1/16/09 8:03p vishk
* PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
*
***************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_platform_features.h"
#include "nexus_i2c.h"
#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_gpio_priv.h"
#include "btnr.h"
#ifdef NEXUS_FRONTEND_3117
#include "btnr_3117ib.h"
#include "btnr_3117ob.h"
#include "bhab_3117.h"
#include "bads_3117.h"
#define BCHIP_HAS_AUS 1
#endif
#ifdef NEXUS_FRONTEND_3114
#include "btnr_3114ib.h"
#include "btnr_3114ob.h"
#include "bhab_3114.h"
#include "bads_3114.h"
#endif
#ifdef NEXUS_FRONTEND_3112
#include "btnr_3112ib.h"
#include "bhab_3112.h"
#include "bads_3112.h"
#endif
#ifdef NEXUS_FRONTEND_3109
#include "btnr_3109ib.h"
#include "bhab_3109.h"
#include "bads_3109.h"
#endif
#ifdef NEXUS_FRONTEND_31xx_OOB
#define BCHIP_HAS_AOB 1
#endif

#include "bhab.h"
#include "bchp_31xx.h"
#include "bads.h"
#include "bhab_31xx_fw.h"
#ifdef BCHIP_HAS_AOB
#include "baob.h"
#endif
#ifdef BCHIP_HAS_AUS
#include "baus.h"
#endif

BDBG_MODULE(nexus_frontend_31xx);

BDBG_OBJECT_ID(NEXUS_31xx);

#define BCHIP_3117 0x3117
#define BCHIP_3114 0x3114
#define BCHIP_3112 0x3112
#define BCHIP_3109 0x3109

#define NEXUS_MAX_31xx_FRONTENDS 3

/* Currently there is only one ADS channel on 3117/3114/3112/3109 */
#define NEXUS_MAX_31xx_ADSCHN 1

/* The OOB channel number on 3117 */
#define NEXUS_31xx_OOB_CHN      NEXUS_MAX_31xx_ADSCHN
#define NEXUS_31xx_AUS_CHN      (NEXUS_31xx_OOB_CHN + 1)

/* The Upstream frontend will not have a tuner hence 1 is subtracted. */
#define NEXUS_MAX_31xx_TUNERS  (NEXUS_MAX_31xx_FRONTENDS - 1)

/***************************************************************************
 * Internal Module Structure
 ***************************************************************************/
typedef struct NEXUS_31xx
{
    BDBG_OBJECT(NEXUS_31xx)
    BLST_S_ENTRY(NEXUS_31xx) node;
    uint16_t  chipId;
    BHAB_Handle hab;
    unsigned    isrNumber;
    NEXUS_GpioHandle gpioInterrupt;
    uint16_t    i2cAddr;
    NEXUS_I2cHandle i2cHandle;
    unsigned    numfrontends;
    BTNR_Handle tnr[NEXUS_MAX_31xx_TUNERS];
    NEXUS_FrontendHandle    frontendHandle[NEXUS_MAX_31xx_FRONTENDS];
    BADS_Handle ads;
    BADS_ChannelHandle  ads_chn[NEXUS_MAX_31xx_ADSCHN];
    NEXUS_FrontendQamSettings   qam[NEXUS_MAX_31xx_ADSCHN];
    NEXUS_FrontendQamSettings   last_ads[NEXUS_MAX_31xx_ADSCHN];
#ifdef BCHIP_HAS_AOB
    BAOB_Handle aob;
    NEXUS_FrontendOutOfBandSettings oob;
    NEXUS_FrontendOutOfBandSettings last_aob;
#endif
#ifdef BCHIP_HAS_AUS
    BAUS_Handle aus;
    NEXUS_FrontendUpstreamSettings ups;
    NEXUS_FrontendUpstreamSettings last_ups;
#endif
    BKNI_EventHandle isrEvent;
    NEXUS_EventCallbackHandle isrEventCallback;
    NEXUS_IsrCallbackHandle lockAppCallback[NEXUS_MAX_31xx_FRONTENDS];
    NEXUS_IsrCallbackHandle updateGainAppCallback[NEXUS_MAX_31xx_ADSCHN];
    NEXUS_IsrCallbackHandle asyncStatusAppCallback[NEXUS_MAX_31xx_FRONTENDS];
    unsigned ifFreq; /* Only applicable for 3117 and 3114 OOB module. */
    uint16_t revId;
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    bool isMaster;
#endif
    bool oobOpenDrain;
    bool adsOpenDrain;
    unsigned agcValue;                        /* Gain Value*/
    NEXUS_CallbackDesc updateGainCallbackDesc;    /* Callback will be called when the gain from the lna needs to be updated. */
    NEXUS_CallbackDesc qamAsyncStatusReadyDesc; /* Callback will be called when the async qam status is ready. */
    NEXUS_CallbackDesc oobAsyncStatusReadyDesc; /* Callback will be called when the async qam status is ready. */
    bool enableDaisyChain;
} NEXUS_31xx;

static BLST_S_HEAD(devList, NEXUS_31xx) g_deviceList = BLST_S_INITIALIZER(g_deviceList);


typedef struct NEXUS_31xxChannel
{
    unsigned chn_num; /* channel number */
    NEXUS_31xx *pDevice; /* 31xx device*/
} NEXUS_31xxChannel;

/***************************************************************************
 * Module callback functions for tuning
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_Close(NEXUS_FrontendHandle handle);
static NEXUS_Error NEXUS_Frontend_P_31xx_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings);
static void NEXUS_Frontend_P_31xx_UnTuneQam(void *handle);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetQamAsyncStatus(void *handle, NEXUS_FrontendQamStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_31xx_RequestQamAsyncStatus(void *handle);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length);
static void NEXUS_Frontend_P_31xx_ResetAdsStatus(void *handle);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetQamScanStatus(void *handle, NEXUS_FrontendQamScanStatus *pScanStatus);
#ifdef BCHIP_HAS_AOB
static void NEXUS_Frontend_P_31xx_UnTuneOob(void *handle);
static NEXUS_Error NEXUS_Frontend_P_31xx_TuneOob(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetOobStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetOobAsyncStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_31xx_RequestOobAsyncStatus(void *handle);
static void NEXUS_Frontend_P_31xx_ResetOobStatus(void *handle);
#endif
#ifdef BCHIP_HAS_AUS
static void NEXUS_Frontend_P_31xx_UnTuneUpstream(void *handle);
static NEXUS_Error NEXUS_Frontend_P_31xx_TuneUpstream(void *handle, const NEXUS_FrontendUpstreamSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetUpstreamStatus(void *handle, NEXUS_FrontendUpstreamStatus *pStatus);
#endif
static uint16_t NEXUS_Frontend_P_Get31xxRev(const NEXUS_31xxSettings *pSettings);
static void NEXUS_Frontend_P_31xx_GetType(void *handle, NEXUS_FrontendType *type);
static NEXUS_Error NEXUS_Frontend_P_31xx_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus);
static NEXUS_Error NEXUS_Frontend_P_31xx_Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings);

/***************************************************************************
Summary:
    Lock callback handler for a 31xx Inband device
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_callback_isr(void *pParam)
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
    Enable/Disable interrupts for a 3117 device
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_L1_isr(void *pParam1, int pParam2)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHAB_Handle hab;
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    NEXUS_31xx *pDevice, *pIntDevice;
#endif
    BDBG_ASSERT(0 != pParam2);
    hab = (BHAB_Handle)pParam2;
    BDBG_ASSERT(NULL != pParam1);
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    pIntDevice = (NEXUS_31xx *)pParam1;
#else
    BSTD_UNUSED(pParam1);
#endif

    rc = BHAB_HandleInterrupt_isr(hab);
    if(rc){rc = BERR_TRACE(rc);}    

#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    for ( pDevice = BLST_S_FIRST(&g_deviceList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
    {
        BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

        if ((pDevice->hab != NULL) && (pDevice->isrNumber== pIntDevice->isrNumber) && (pDevice->gpioInterrupt == pIntDevice->gpioInterrupt) && (pDevice->hab != hab))
        {
            rc = BHAB_HandleInterrupt_isr(pDevice->hab);
            if(rc){rc = BERR_TRACE(rc);}
        }
    }
#endif

}
/***************************************************************************
Summary:
    Enable/Disable interrupts for a 3117 device
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_GpioIsrControl_isr(bool enable, void *pParam)
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
    Enable/Disable interrupts for a 3117 device
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_IsrControl_isr(bool enable, void *pParam)
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
    ISR Event Handler for a 3117 device
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_IsrEvent(void *pParam)
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
    Initialize Hab for a 3117 device
***************************************************************************/
void NEXUS_Frontend_P_UnInit_31xx_Hab(NEXUS_31xx *pDevice)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_GpioSettings gpioSettings;

    if(pDevice->isrNumber) {
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        if(pDevice->isMaster)
        {
            NEXUS_Core_DisconnectInterrupt(pDevice->isrNumber);
        }
#else
        NEXUS_Core_DisconnectInterrupt(pDevice->isrNumber);
#endif
    }
    else if(pDevice->gpioInterrupt){
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        if(pDevice->isMaster)
        {
            NEXUS_Gpio_SetInterruptCallback_priv(pDevice->gpioInterrupt, NULL, NULL, 0);
            NEXUS_Gpio_GetSettings(pDevice->gpioInterrupt,  &gpioSettings);
            gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
            gpioSettings.interrupt.callback = NULL;
            rc = NEXUS_Gpio_SetSettings(pDevice->gpioInterrupt, &gpioSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
#else
        NEXUS_Gpio_SetInterruptCallback_priv(pDevice->gpioInterrupt, NULL, NULL, 0);
        NEXUS_Gpio_GetSettings(pDevice->gpioInterrupt,  &gpioSettings);
        gpioSettings.interruptMode = NEXUS_GpioInterrupt_eDisabled;
        gpioSettings.interrupt.callback = NULL;
        rc = NEXUS_Gpio_SetSettings(pDevice->gpioInterrupt, &gpioSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}

#endif
    }

    if(pDevice->isrEvent) pDevice->isrEvent = NULL;
    if(pDevice->isrEventCallback)NEXUS_UnregisterEvent(pDevice->isrEventCallback);
    pDevice->isrEventCallback = NULL;
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    BKNI_EnterCriticalSection();
    BLST_S_REMOVE(&g_deviceList, pDevice, NEXUS_31xx, node);
    BKNI_LeaveCriticalSection();
#endif
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
    Initialize Hab for a 3117 device
***************************************************************************/
NEXUS_Error NEXUS_Frontend_P_Init_31xx_Hab(NEXUS_31xx *pDevice, const NEXUS_31xxSettings *pSettings)
{
    NEXUS_Error rc = BERR_SUCCESS;
    BHAB_Settings stHabSettings;
    BREG_I2C_Handle i2cHandle;
    BHAB_NmiSettings nmiSettings;
    BHAB_WatchDogTimerSettings wdtSettings;

    switch (pDevice->chipId)
    {
#ifdef NEXUS_FRONTEND_3117
    case BCHIP_3117:
        rc = BHAB_3117_GetDefaultSettings(&stHabSettings);
        break;
#endif
#ifdef NEXUS_FRONTEND_3114
    case BCHIP_3114:
        rc = BHAB_3114_GetDefaultSettings(&stHabSettings);
        break;
#endif
#ifdef NEXUS_FRONTEND_3112
    case BCHIP_3112:
        rc = BHAB_3112_GetDefaultSettings(&stHabSettings);
        break;
#endif
#ifdef NEXUS_FRONTEND_3109
    case BCHIP_3109:
        rc = BHAB_3109_GetDefaultSettings(&stHabSettings);
        break;
#endif
    default:
        rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
    }
    if ( rc != BERR_SUCCESS ) goto done;


    stHabSettings.chipAddr = pSettings->i2cAddr;
    if(pSettings->isrNumber) {
        stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_31xx_IsrControl_isr;
        stHabSettings.interruptEnableFuncParam = (void*)pSettings->isrNumber;
    }
    else if(pSettings->gpioInterrupt){
        stHabSettings.interruptEnableFunc = NEXUS_Frontend_P_31xx_GpioIsrControl_isr;
        stHabSettings.interruptEnableFuncParam = (void*)pSettings->gpioInterrupt;
    }

    stHabSettings.slaveChipAddr = pSettings->i2cSlaveAddr;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NEXUS_MODULE_SELF);
    BDBG_ASSERT(NULL != i2cHandle);

    rc = BHAB_Open( &pDevice->hab, (void *)i2cHandle, &stHabSettings);
    if ( rc != BERR_SUCCESS ) goto done;

#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
    /* disconnect the previous for shared interrupt, last one is master */
    if(pSettings->isrNumber) {
        NEXUS_Core_DisconnectInterrupt(pSettings->isrNumber);
    }
    else if(pSettings->gpioInterrupt) {
        NEXUS_Gpio_SetInterruptCallback_priv(pSettings->gpioInterrupt, NULL, NULL, 0);
    }
#endif

    /* Success opeining Hab.  Connect Interrupt */
    if(pSettings->isrNumber) {
        rc = NEXUS_Core_ConnectInterrupt(pSettings->isrNumber,
                                              NEXUS_Frontend_P_31xx_L1_isr,
                                              (void *)pDevice,
                                              (int)pDevice->hab);
        if ( rc != BERR_SUCCESS ) goto done;
    }
    else if(pSettings->gpioInterrupt){
        NEXUS_Gpio_SetInterruptCallback_priv(pSettings->gpioInterrupt, NEXUS_Frontend_P_31xx_L1_isr, (void *)pDevice, (int)pDevice->hab);
    }


    if (pSettings->loadAP)
    {
        if(pDevice->revId == 0x0) {
#ifdef NEXUS_FRONTEND_31xx_B0           
            BDBG_ERR(("Nexus is compiled to support 31xx B0 revision. But, the revision read is not B0.)"));
            rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
#endif
        }
        else if (pDevice->revId == 0x100){
#ifndef NEXUS_FRONTEND_31xx_B0
            BDBG_ERR(("Nexus is compiled to support either 31xx A0 or A1 revision. But, the revision read is neither.)"));
            rc = BERR_TRACE(BERR_NOT_INITIALIZED); goto done;
#endif          
        }
        
        BDBG_WRN(("BHAB_InitAp(rev a image)"));
        rc = BHAB_InitAp(pDevice->hab, bcm31xx_ap_image);
        if ( rc != BERR_SUCCESS ) goto done;
    }
    else
    {
        rc = BHAB_InitAp(pDevice->hab, NULL);
        if ( rc != BERR_SUCCESS ) goto done;
    }

    /* Get events and register callbacks */
    rc = BHAB_GetInterruptEventHandle(pDevice->hab, &pDevice->isrEvent);
    if ( rc != BERR_SUCCESS ) goto done;

    pDevice->isrEventCallback = NEXUS_RegisterEvent(pDevice->isrEvent, NEXUS_Frontend_P_31xx_IsrEvent, pDevice->hab);
    if ( NULL == pDevice->isrEventCallback )
    {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto done;
    }

    if(pSettings->configureWatchdog) {
        BKNI_Memset(&wdtSettings, 0, sizeof(wdtSettings));

        wdtSettings.clearWatchDog.enable = 0;
        wdtSettings.clearWatchDog.polarity = 1;
        wdtSettings.clearWatchDog.select = 10;
        wdtSettings.nmiMode = 0; /* nmi triggered midway */
        wdtSettings.oneShot = 0;
        wdtSettings.start = 0;
        wdtSettings.timeout = 0x202fbf00;   /*20 secs*/

        rc = BHAB_SetWatchDogTimer(pDevice->hab, &wdtSettings);
        if ( rc != BERR_SUCCESS ) goto done;

        BKNI_Memset(&nmiSettings, 0, sizeof(nmiSettings));

        nmiSettings.nmi.enable = 0;
        nmiSettings.nmi.polarity = 1; /*redundant*/
        nmiSettings.nmi.select = 0;

        rc = BHAB_SetNmiConfig(pDevice->hab, &nmiSettings);
        if ( rc != BERR_SUCCESS ) goto done;
    }

    return BERR_SUCCESS;

done:
    NEXUS_Frontend_P_UnInit_31xx_Hab(pDevice);
    return rc;
}

/***************************************************************************
Summary:
    Get the default settings for a BCM3117 tuner
See Also:
    NEXUS_Frontend_Open3117
 ***************************************************************************/
void NEXUS_Frontend_GetDefault31xxSettings(
    NEXUS_31xxSettings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->type = NEXUS_31xxChannelType_eInBand;
    pSettings->channelNumber = 0;
    pSettings->inBandOpenDrain = true;
    pSettings->outOfBandOpenDrain = true;
    pSettings->loadAP = true;
    pSettings->i2cSlaveAddr = 0x60;
}
/***************************************************************************
Summary:
    Initialize all ADS/OOB channels.
 ***************************************************************************/
void NEXUS_Frontend_P_UnInit31xx(NEXUS_31xx *pDevice)
{
    NEXUS_Error rc = BERR_SUCCESS;
    unsigned int i;

    for ( i = 0; i < NEXUS_MAX_31xx_TUNERS && NULL != pDevice->tnr[i]; i++) {
        if(pDevice->tnr[i]){
            rc = BTNR_Close(pDevice->tnr[i]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
            pDevice->tnr[i] = NULL;
        }
    }
    for ( i = 0; i < NEXUS_MAX_31xx_ADSCHN && NULL != pDevice->ads_chn[i]; i++) {
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
#if BCHIP_HAS_AOB
    if (pDevice->aob){
        rc = BAOB_Close(pDevice->aob);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->aob = NULL;
    }
#endif
#ifdef BCHIP_HAS_AUS
    if (pDevice->aob){
        rc = BAUS_Close(pDevice->aus);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pDevice->aus = NULL;
    }
#endif
done:
    return;
}
/***************************************************************************
Summary:
    Initialize all ADS/OOB channels.
 ***************************************************************************/
static NEXUS_Error NEXUS_Frontend_P_Init31xx(NEXUS_31xx *pDevice)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned int i, num_ch;
#ifdef NEXUS_FRONTEND_3117
    BTNR_3117Ob_Settings tnrOb3117_cfg;
    BTNR_3117Ib_Settings tnrIb3117_cfg;
#endif
#ifdef NEXUS_FRONTEND_3114
    BTNR_3114Ib_Settings tnrIb3114_cfg;
    BTNR_3114Ob_Settings tnrOb3114_cfg;
#endif
#ifdef NEXUS_FRONTEND_3112
    BTNR_3112Ib_Settings tnrIb3112_cfg;
#endif
#ifdef NEXUS_FRONTEND_3109
    BTNR_3109Ib_Settings tnrIb3109_cfg;
#endif
#ifdef BCHIP_HAS_AUS
    BAUS_Settings aus_cfg;
#endif
#ifdef BCHIP_HAS_AOB
    BAOB_Settings aob_cfg;
#endif
    BADS_Settings ads_cfg;
    BADS_ChannelSettings chn_cfg;

    switch (pDevice->chipId)
    {
#ifdef NEXUS_FRONTEND_3117
    case BCHIP_3117:
        rc = BADS_3117_GetDefaultSettings( &ads_cfg, NULL);
        break;
#endif
#ifdef NEXUS_FRONTEND_3114
    case BCHIP_3114:
        rc = BADS_3114_GetDefaultSettings( &ads_cfg, NULL);
        break;
#endif
#ifdef NEXUS_FRONTEND_3112
    case BCHIP_3112:
        rc = BADS_3112_GetDefaultSettings( &ads_cfg, NULL);
        break;
#endif
#ifdef NEXUS_FRONTEND_3109
    case BCHIP_3109:
        rc = BADS_3109_GetDefaultSettings( &ads_cfg, NULL);
        break;
#endif
    default:
        rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
    }
    if ( rc != BERR_SUCCESS ) goto done;

    if(pDevice->revId == 0x0) {
        ads_cfg.transportConfig = BADS_TransportData_eGpioSerial;
    }
    else if (pDevice->revId == 0x100){
        ads_cfg.transportConfig = BADS_TransportData_eSerial;
    }
    ads_cfg.hGeneric = pDevice->hab;
    ads_cfg.isOpenDrain = pDevice->adsOpenDrain;
    rc = BADS_Open(&pDevice->ads, NULL, NULL, NULL, &ads_cfg);
    if ( rc != BERR_SUCCESS ) goto done;

    rc = BADS_Init(pDevice->ads);
    if ( rc != BERR_SUCCESS ) goto done;

    /* get total ADS channel number*/
    rc = BADS_GetTotalChannels(pDevice->ads, &num_ch);
    if (rc!=BERR_SUCCESS) goto done;

    for (i=0;i<num_ch;i++) {
        switch (pDevice->chipId)
        {
#ifdef NEXUS_FRONTEND_3117
        case BCHIP_3117:
            rc = BTNR_3117Ib_GetDefaultSettings(&tnrIb3117_cfg);
            if (rc != BERR_SUCCESS) goto done;
            rc =  BTNR_3117Ib_Open(&pDevice->tnr[i],&tnrIb3117_cfg, pDevice->hab);
            break;
#endif
#ifdef NEXUS_FRONTEND_3114
        case BCHIP_3114:
            rc = BTNR_3114Ib_GetDefaultSettings(&tnrIb3114_cfg);
            if (rc != BERR_SUCCESS) goto done;
            rc =  BTNR_3114Ib_Open(&pDevice->tnr[i],&tnrIb3114_cfg, pDevice->hab);
            break;
#endif
#ifdef NEXUS_FRONTEND_3112
        case BCHIP_3112:
            rc = BTNR_3112Ib_GetDefaultSettings(&tnrIb3112_cfg);
            if (rc != BERR_SUCCESS) goto done;
            rc =  BTNR_3112Ib_Open(&pDevice->tnr[i],&tnrIb3112_cfg, pDevice->hab);
            break;
#endif
#ifdef NEXUS_FRONTEND_3109
        case BCHIP_3109:
            rc = BTNR_3109Ib_GetDefaultSettings(&tnrIb3109_cfg);
            if (rc != BERR_SUCCESS) goto done;
            rc =  BTNR_3109Ib_Open(&pDevice->tnr[i],&tnrIb3109_cfg, pDevice->hab);
            break;
#endif
        default:
            rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
        }
        if(rc){rc = BERR_TRACE(rc); goto done;} 
    }

    /* Configure ADS channels */
    for (i=0;i<num_ch;i++) {
        rc = BADS_GetChannelDefaultSettings( pDevice->ads, i, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto done;
        rc = BADS_OpenChannel( pDevice->ads, &pDevice->ads_chn[i], i, &chn_cfg);
        if (rc!=BERR_SUCCESS) goto done;
    }

    if((pDevice->chipId == BCHIP_3109) || (pDevice->chipId == BCHIP_3112))
        return BERR_SUCCESS;

    switch (pDevice->chipId)
    {
#ifdef NEXUS_FRONTEND_3117
    case BCHIP_3117:
        BTNR_3117Ob_GetDefaultSettings(&tnrOb3117_cfg);

        tnrOb3117_cfg.ifFreq = pDevice->ifFreq;
        rc = BTNR_3117Ob_Open(&pDevice->tnr[NEXUS_31xx_OOB_CHN], &tnrOb3117_cfg, pDevice->hab);
        if (rc!=BERR_SUCCESS) goto done;
        break;
#endif
#ifdef NEXUS_FRONTEND_3114
    case BCHIP_3114:
        BTNR_3114Ob_GetDefaultSettings(&tnrOb3114_cfg);

        tnrOb3114_cfg.ifFreq = pDevice->ifFreq;
        rc = BTNR_3114Ob_Open(&pDevice->tnr[NEXUS_31xx_OOB_CHN], &tnrOb3114_cfg, pDevice->hab);
        if (rc!=BERR_SUCCESS) goto done;
        break;
#endif
    default:
        rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto done;
    }

#ifdef BCHIP_HAS_AOB
    rc = BAOB_GetDefaultSettings( &aob_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto done;
    /* There is only one AOB device in 3117/3114. so the device id is left to default settings. */
    aob_cfg.hGeneric = pDevice->hab;
    aob_cfg.ifFreq = pDevice->ifFreq;
    aob_cfg.serialData = true;
    aob_cfg.isOpenDrain = pDevice->oobOpenDrain;
    rc = BAOB_Open(&pDevice->aob, NULL, NULL, NULL, &aob_cfg);
    if (rc!=BERR_SUCCESS) goto done;
#endif

#ifdef BCHIP_HAS_AUS
    rc = BAUS_GetDefaultSettings(&aus_cfg, NULL);
    if (rc!=BERR_SUCCESS) goto done;
    /* There is only one AOB device in 3117/3114. so the device id is left to default settings. */
    aus_cfg.hGeneric = pDevice->hab;
    rc = BAUS_Open(&pDevice->aus, NULL, NULL, &aus_cfg);
    if (rc!=BERR_SUCCESS) goto done;
#endif

    return BERR_SUCCESS;
done:
    NEXUS_Frontend_P_UnInit31xx(pDevice);
    return rc;
}

static uint16_t NEXUS_Frontend_P_Get31xxRev(const NEXUS_31xxSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[2];
    uint16_t revId=0;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);

    buf[0]= 0x15;
    rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_Read(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_DAT), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    revId = buf[0];

    buf[0]= 0x16;
    rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_Read(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_DAT), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    revId = (revId<<8) | buf[0];

    return revId;
done:
    return 0;
}

static uint16_t NEXUS_Frontend_P_Is31xx(const NEXUS_31xxSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BREG_I2C_Handle i2cHandle;
    uint8_t buf[2];
    uint16_t chipId=0;

    i2cHandle = NEXUS_I2c_GetRegHandle(pSettings->i2cDevice, NULL);

    if (pSettings->loadAP)
    {
        buf[0]= 0x00;
        buf[1]= 0xff;
        rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        buf[0]= 0x01;
        buf[1]= 0xff;
        rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        buf[0]= 0x00;
        buf[1]= 0x00;
        rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2);
        if(rc){rc = BERR_TRACE(rc); goto done;}

        buf[0]= 0x01;
        buf[1]= 0x00;
        rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 2);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    buf[0]= 0x13;
    rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_Read(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_DAT), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    chipId = buf[0];

    buf[0]= 0x14;
    rc = BREG_I2C_Write(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_ADR), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    rc = BREG_I2C_Read(i2cHandle, pSettings->i2cAddr, DEVICE(SH_SFR_H_LOCAL_DAT), buf, 1);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    chipId = (chipId<<8) | buf[0];

    return chipId;
done:
    return 0;
}

/***************************************************************************
Summary:
    Open a handle to a BCM3117 device.
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open31xx(
    const NEXUS_31xxSettings *pSettings
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_FrontendHandle frontendHandle = NULL;
    NEXUS_31xx *pDevice = NULL;
    unsigned int chn_num=0;
    NEXUS_IsrCallbackHandle callback;
    NEXUS_IsrCallbackHandle updateGainCallback  = NULL;
    NEXUS_IsrCallbackHandle qamAsyncStatusReadyCallback = NULL;
#ifdef BCHIP_HAS_AOB
    NEXUS_IsrCallbackHandle oobAsyncStatusReadyCallback = NULL;
#endif
    NEXUS_31xxChannel *pChannel;
    uint16_t chipId=0, revId=0;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->i2cDevice);

    if ( ((pSettings->channelNumber >= NEXUS_MAX_31xx_ADSCHN)&& pSettings->type == NEXUS_31xxChannelType_eInBand)
        || (pSettings->channelNumber >= NEXUS_MAX_31xx_FRONTENDS ))
    {
        BDBG_ERR((" channel number exceeds available one"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_create;
    }

    switch(pSettings->type)
    {
    case NEXUS_31xxChannelType_eInBand:
        chn_num = pSettings->channelNumber;
        break;
    case NEXUS_31xxChannelType_eOutOfBand:
        chn_num = NEXUS_31xx_OOB_CHN;
        break;
    case NEXUS_31xxChannelType_eUpstream:
        chn_num = NEXUS_31xx_AUS_CHN;
        break;
    default:
        BDBG_ERR((" channel type not supported"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_create;
    }

    for ( pDevice = BLST_S_FIRST(&g_deviceList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
    {
        BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
        if ((pSettings->i2cAddr == pDevice->i2cAddr) && (pSettings->i2cDevice== pDevice->i2cHandle))
        {
            break;
        }
    }

    if ( NULL == pDevice)
    {
#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        for ( pDevice = BLST_S_FIRST(&g_deviceList); NULL != pDevice; pDevice = BLST_S_NEXT(pDevice, node) )
        {
            BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
            if ((pSettings->gpioInterrupt == pDevice->gpioInterrupt) && (pSettings->isrNumber == pDevice->isrNumber))
            {
                pDevice->isMaster = false;
            }
        }
#endif
        chipId = NEXUS_Frontend_P_Is31xx(pSettings);
        revId = NEXUS_Frontend_P_Get31xxRev(pSettings);

        pDevice = BKNI_Malloc(sizeof(NEXUS_31xx));
        if(NULL == pDevice){rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create;}
        BKNI_Memset(pDevice, 0, sizeof(NEXUS_31xx));
        BDBG_OBJECT_SET(pDevice, NEXUS_31xx);

        pDevice->chipId = chipId;
        pDevice->revId = revId;
        pDevice->i2cAddr = pSettings->i2cAddr;
        pDevice->isrNumber = pSettings->isrNumber;
        pDevice->gpioInterrupt = pSettings->gpioInterrupt;
        pDevice->i2cHandle = pSettings->i2cDevice;
        pDevice->ifFreq = pSettings->ifFrequency;
        pDevice->adsOpenDrain = pSettings->inBandOpenDrain;
        pDevice->oobOpenDrain = pSettings->outOfBandOpenDrain;
 #ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        pDevice->isMaster = true;
#endif
        rc = NEXUS_Frontend_P_Init_31xx_Hab(pDevice, pSettings);
        if(rc){rc = BERR_TRACE(rc); goto err_init_hab;}

        /* Initialize ADS channels, OOB and AUS */
        rc = NEXUS_Frontend_P_Init31xx(pDevice);
        if(rc){rc = BERR_TRACE(rc); goto err_init;} 

#ifdef NEXUS_SHARED_FRONTEND_INTERRUPT
        BKNI_EnterCriticalSection();
        BLST_S_INSERT_HEAD(&g_deviceList, pDevice, node);
        BKNI_LeaveCriticalSection();
#else
        BLST_S_INSERT_HEAD(&g_deviceList, pDevice, node);
#endif
    }
    else
    {
        BDBG_MSG((" found device"));
    }

    if ((pSettings->type == NEXUS_31xxChannelType_eOutOfBand ) && ((pDevice->chipId == BCHIP_3109) || (pDevice->chipId == BCHIP_3112)))
    {
        return NULL;
    }

    /* chekc if fronthandle is already opened*/
    if ( pDevice->frontendHandle[chn_num] != NULL )
    {
        return pDevice->frontendHandle[chn_num];
    }

    pChannel = (NEXUS_31xxChannel*)BKNI_Malloc(sizeof(NEXUS_31xxChannel));
    if ( NULL == pChannel ) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_ch_alloc;}

    /* Create a Nexus frontend handle */
    frontendHandle = NEXUS_Frontend_P_Create(pChannel);
    if ( NULL == frontendHandle ) {rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}

    /* Establish device capabilities */
    if ( pSettings->type == NEXUS_31xxChannelType_eInBand)
    {
        frontendHandle->capabilities.qam = true;
        frontendHandle->capabilities.outOfBand = false;
        frontendHandle->capabilities.upstream = false;
        BKNI_Memset(frontendHandle->capabilities.qamModes, true, sizeof(frontendHandle->capabilities.qamModes));
        /* bind functions*/
        frontendHandle->tuneQam = NEXUS_Frontend_P_31xx_TuneQam;
        frontendHandle->getQamStatus = NEXUS_Frontend_P_31xx_GetQamStatus;
        frontendHandle->getSoftDecisions = NEXUS_Frontend_P_31xx_GetSoftDecisions;
        frontendHandle->resetStatus = NEXUS_Frontend_P_31xx_ResetAdsStatus;
        frontendHandle->untune = NEXUS_Frontend_P_31xx_UnTuneQam;
        frontendHandle->requestQamAsyncStatus = NEXUS_Frontend_P_31xx_RequestQamAsyncStatus;
        frontendHandle->getQamAsyncStatus = NEXUS_Frontend_P_31xx_GetQamAsyncStatus;
        frontendHandle->getQamScanStatus = NEXUS_Frontend_P_31xx_GetQamScanStatus;
    }
#ifdef BCHIP_HAS_AOB
    else if ( pSettings->type == NEXUS_31xxChannelType_eOutOfBand)
    {
        frontendHandle->capabilities.qam = false;
        frontendHandle->capabilities.outOfBand = true;
        frontendHandle->capabilities.upstream = false;
        BKNI_Memset(frontendHandle->capabilities.outOfBandModes, true, sizeof(frontendHandle->capabilities.outOfBandModes));
        /* bind functions*/
        frontendHandle->tuneOutOfBand = NEXUS_Frontend_P_31xx_TuneOob;
        frontendHandle->getOutOfBandStatus = NEXUS_Frontend_P_31xx_GetOobStatus;
        frontendHandle->getSoftDecisions = NEXUS_Frontend_P_31xx_GetSoftDecisions;
        frontendHandle->resetStatus = NEXUS_Frontend_P_31xx_ResetOobStatus;
        frontendHandle->untune = NEXUS_Frontend_P_31xx_UnTuneOob;
        frontendHandle->requestOutOfBandAsyncStatus = NEXUS_Frontend_P_31xx_RequestOobAsyncStatus;
        frontendHandle->getOutOfBandAsyncStatus = NEXUS_Frontend_P_31xx_GetOobAsyncStatus;
    }
#endif
#ifdef BCHIP_HAS_AUS
    else if ( pSettings->type == NEXUS_31xxChannelType_eUpstream)
    {
        frontendHandle->capabilities.qam = false;
        frontendHandle->capabilities.outOfBand = false;
        frontendHandle->capabilities.upstream = true;
        BKNI_Memset(frontendHandle->capabilities.upstreamModes, true, sizeof(frontendHandle->capabilities.upstreamModes));
        /* bind functions*/
        frontendHandle->tuneUpstream = NEXUS_Frontend_P_31xx_TuneUpstream;
        frontendHandle->getUpstreamStatus = NEXUS_Frontend_P_31xx_GetUpstreamStatus;
        frontendHandle->untune = NEXUS_Frontend_P_31xx_UnTuneUpstream;
    }
#endif

    frontendHandle->close = NEXUS_Frontend_P_31xx_Close;
    frontendHandle->getType = NEXUS_Frontend_P_31xx_GetType;
    frontendHandle->getFastStatus = NEXUS_Frontend_P_31xx_GetFastStatus;
    frontendHandle->standby = NEXUS_Frontend_P_31xx_Standby;

    /* Create app callback */
    callback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
    if ( NULL == callback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_cbk_create;}
    /* install callback to  notify of lock/unlock change */
    if ( pSettings->type == NEXUS_31xxChannelType_eInBand)
    {
        updateGainCallback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
        if ( NULL == updateGainCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_gain_cbk_create;}
        
        qamAsyncStatusReadyCallback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
        if ( NULL == qamAsyncStatusReadyCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_qam_async_create;}
        
        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eLockChange, (BADS_CallbackFunc)NEXUS_Frontend_P_31xx_callback_isr, (void*)callback);
        if(rc){rc = BERR_TRACE(rc); goto err_cbk_inst;} 
        
        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eUpdateGain, (BADS_CallbackFunc)NEXUS_Frontend_P_31xx_callback_isr, (void*)updateGainCallback);
        if(rc){rc = BERR_TRACE(rc); goto err_cbk_inst;} 

        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eAsyncStatusReady, (BADS_CallbackFunc)NEXUS_Frontend_P_31xx_callback_isr, (void*)qamAsyncStatusReadyCallback);
        if(rc){rc = BERR_TRACE(rc); goto err_cbk_inst;} 

        /* For now this is hooked up to the lock/unlock callback. */
        rc = BADS_InstallCallback(pDevice->ads_chn[chn_num], BADS_Callback_eNoSignal, (BADS_CallbackFunc)NEXUS_Frontend_P_31xx_callback_isr, (void*)callback);
        if(rc){rc = BERR_TRACE(rc); goto err_cbk_inst;} 

        pDevice->updateGainAppCallback[chn_num] = updateGainCallback ;
        pDevice->asyncStatusAppCallback[chn_num] = qamAsyncStatusReadyCallback ;
    }
#ifdef BCHIP_HAS_AOB
    else if(pSettings->type == NEXUS_31xxChannelType_eOutOfBand)
    {
        oobAsyncStatusReadyCallback = NEXUS_IsrCallback_Create(frontendHandle, NULL);
        if ( NULL == oobAsyncStatusReadyCallback ) { rc = BERR_TRACE(NEXUS_NOT_INITIALIZED); goto err_oob_async_create;}
        
        rc = BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eLockChange, (BAOB_CallbackFunc)NEXUS_Frontend_P_31xx_callback_isr, (void*)callback);
        if(rc){rc = BERR_TRACE(rc); goto err_cbk_inst;}
        
        rc = BAOB_InstallCallback(pDevice->aob, BAOB_Callback_eAsyncStatusReady, (BAOB_CallbackFunc)NEXUS_Frontend_P_31xx_callback_isr, (void*)oobAsyncStatusReadyCallback);
        if(rc){rc = BERR_TRACE(rc); goto err_cbk_inst;}
        
        pDevice->asyncStatusAppCallback[chn_num] = oobAsyncStatusReadyCallback ;
    }
#endif
    /* See if upstream needs a callback. */
    pDevice->enableDaisyChain = true;
    pDevice->lockAppCallback[chn_num] = callback ;
    pDevice->frontendHandle[chn_num] = frontendHandle;
    /* save channel number in channelHandle*/
    pChannel->chn_num = chn_num;
    pChannel->pDevice = pDevice;    
    frontendHandle->chip.id = pDevice->chipId;
    pDevice->numfrontends++;
    return frontendHandle;

#ifdef BCHIP_HAS_AOB
err_oob_async_create:
    if ( oobAsyncStatusReadyCallback ) NEXUS_IsrCallback_Destroy(oobAsyncStatusReadyCallback);
#endif
err_qam_async_create:
    if ( qamAsyncStatusReadyCallback ) NEXUS_IsrCallback_Destroy(qamAsyncStatusReadyCallback);
err_cbk_inst:
    if ( updateGainCallback ) NEXUS_IsrCallback_Destroy(updateGainCallback);
err_gain_cbk_create:
    if ( callback ) NEXUS_IsrCallback_Destroy(callback);
err_cbk_create:
    if ( frontendHandle ) BKNI_Free(frontendHandle);
err_alloc:
    if ( pChannel ) BKNI_Free(pChannel);
    if(pDevice->numfrontends != 0)
        return NULL;
err_ch_alloc:
    NEXUS_Frontend_P_UnInit31xx(pDevice);
err_init:
    NEXUS_Frontend_P_UnInit_31xx_Hab(pDevice);
err_init_hab:
    if ( pDevice ) BKNI_Free(pDevice);
    pDevice = NULL;
err_create:
    return NULL;
}

/***************************************************************************
Summary:
    Close a handle to a BCM3117 device.
See Also:
    NEXUS_Frontend_Open3117
 ***************************************************************************/
static void NEXUS_Frontend_P_31xx_Close(
    NEXUS_FrontendHandle handle
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pChannel =(NEXUS_31xxChannel*) handle->pDeviceHandle;
    BDBG_ASSERT(NULL != pChannel);
    pDevice = (NEXUS_31xx *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_FRONTENDS)
    {
        BDBG_ERR((" Unsupported Frontend Handle"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if ( NULL != pDevice->lockAppCallback[pChannel->chn_num])
        NEXUS_IsrCallback_Destroy(pDevice->lockAppCallback[pChannel->chn_num]);
    if ( NULL != pDevice->asyncStatusAppCallback[pChannel->chn_num])
        NEXUS_IsrCallback_Destroy(pDevice->asyncStatusAppCallback[pChannel->chn_num]);

    if ((pChannel->chn_num < NEXUS_MAX_31xx_ADSCHN) && ( NULL != pDevice->updateGainAppCallback[pChannel->chn_num]))
        NEXUS_IsrCallback_Destroy(pDevice->updateGainAppCallback[pChannel->chn_num]);

    NEXUS_Frontend_P_Destroy(handle);

    pDevice->frontendHandle[pChannel->chn_num] = NULL;
    BKNI_Free(pChannel);
    pDevice->numfrontends--;

    /* see if there are still open channels*/
    if (pDevice->numfrontends) return;

    NEXUS_Frontend_P_UnInit31xx(pDevice);
    NEXUS_Frontend_P_UnInit_31xx_Hab(pDevice);

#ifndef NEXUS_SHARED_FRONTEND_INTERRUPT
    BLST_S_REMOVE(&g_deviceList, pDevice, NEXUS_31xx, node);
#endif
    BDBG_OBJECT_DESTROY(pDevice, NEXUS_31xx);
    BKNI_Free(pDevice);
done:
        return;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_TuneQam(void *handle, const NEXUS_FrontendQamSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BADS_InbandParam ibParam;
    BTNR_PowerSaverSettings pwrSettings;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

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
            return BERR_TRACE(BERR_NOT_SUPPORTED);
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
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        ibParam.enableNullPackets = false;
    }
    else
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    ibParam.symbolRate = pSettings->symbolRate;
    ibParam.autoAcquire = pSettings->autoAcquire;
    ibParam.enableDpm = pSettings->enablePowerMeasurement;
    ibParam.spectrum = pSettings->spectrumMode;
    ibParam.invertSpectrum = pSettings->spectralInversion;
    ibParam.frequencyOffset = pSettings->frequencyOffset;

    NEXUS_IsrCallback_Set(pDevice->lockAppCallback[pChannel->chn_num], &(pSettings->lockCallback));    
    NEXUS_IsrCallback_Set(pDevice->asyncStatusAppCallback[pChannel->chn_num], &(pSettings->asyncStatusReadyCallback));

    if (pDevice->tnr[pChannel->chn_num])
    {
        pwrSettings.enable = false;
        rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &pwrSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;} 
        
        rc = BADS_DisablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
        if(rc){rc = BERR_TRACE(rc); goto done;} 

        rc = BTNR_SetTunerRfFreq(pDevice->tnr[pChannel->chn_num], pSettings->frequency, BTNR_TunerMode_eDigital);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    BKNI_Sleep(90);

    rc = BADS_Acquire(pDevice->ads_chn[pChannel->chn_num], &ibParam );
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pDevice->last_ads[pChannel->chn_num] = *pSettings;

    return BERR_SUCCESS;
done:
    NEXUS_Frontend_P_31xx_UnTuneQam(handle);
    return rc;
}

static void NEXUS_Frontend_P_31xx_UnTuneQam(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    
    rc = BADS_EnablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
    if(rc){rc = BERR_TRACE(rc); goto done;}
    
done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_Standby(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BTNR_PowerSaverSettings pwrSettings;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BSTD_UNUSED(pSettings);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_FRONTENDS)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    if(enabled){    
        if(pChannel->chn_num < NEXUS_MAX_31xx_ADSCHN){
            rc = BADS_EnablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
        else if(pChannel->chn_num == NEXUS_31xx_OOB_CHN){
#ifdef BCHIP_HAS_AOB
            rc = BAOB_EnablePowerSaver(pDevice->aob);
            if(rc){rc = BERR_TRACE(rc); goto done;}
#endif      
        }
        else if(pChannel->chn_num == NEXUS_31xx_AUS_CHN){
#ifdef BCHIP_HAS_AUS
            rc = BAUS_DisableTransmitter(pDevice->aus);
            if(rc){BERR_TRACE(rc);}
#endif      
        }       
        if(pChannel->chn_num < NEXUS_MAX_31xx_TUNERS){
            pwrSettings.enable = true;
            rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &pwrSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
    }
    else{
        if(pChannel->chn_num < NEXUS_MAX_31xx_TUNERS){
            pwrSettings.enable = false;
            rc = BTNR_SetPowerSaver(pDevice->tnr[pChannel->chn_num], &pwrSettings);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }

        if(pChannel->chn_num < NEXUS_MAX_31xx_ADSCHN){
            rc = BADS_DisablePowerSaver(pDevice->ads_chn[pChannel->chn_num]);
            if(rc){rc = BERR_TRACE(rc); goto done;}
        }
        else if(pChannel->chn_num == NEXUS_31xx_OOB_CHN){
#ifdef BCHIP_HAS_AOB
            rc = BAOB_DisablePowerSaver(pDevice->aob);
            if(rc){rc = BERR_TRACE(rc); goto done;}
#endif      
        }
        else if(pChannel->chn_num == NEXUS_31xx_AUS_CHN){
#ifdef BCHIP_HAS_AUS
            rc = BAUS_EnableTransmitter(pDevice->aus);
            if(rc){BERR_TRACE(rc);}
#endif      
        }   
    }    
    
done:
    return rc;
}

static NEXUS_FrontendLockStatus  NEXUS_Frontend_P_GetLockStatus(unsigned lockStatus)
{
    switch ( lockStatus )
    {
    /*BADS_LockStatus_eUnlocked == BAOB_LockStatus_eUnlocked.  */
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
static NEXUS_Error NEXUS_Frontend_P_31xx_GetQamScanStatus(void *handle, NEXUS_FrontendQamScanStatus *pScanStatus)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pScanStatus);
    
    BDBG_WRN(("NEXUS_Frontend_GetQamScanStatus() is not supported for 31xx frontend."));

    return NEXUS_NOT_SUPPORTED;

}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus)
{
    NEXUS_Error  rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    unsigned lock;

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = (NEXUS_31xx *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if(pChannel->chn_num < NEXUS_MAX_31xx_ADSCHN){
        rc = BADS_GetLockStatus(pDevice->ads_chn[pChannel->chn_num],  (BADS_LockStatus *)&lock);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pStatus->lockStatus = NEXUS_Frontend_P_GetLockStatus(lock);
    }
#ifdef BCHIP_HAS_AOB
    else if(pChannel->chn_num == NEXUS_31xx_OOB_CHN){
        rc = BAOB_GetLockStatus(pDevice->aob,  (bool *)&lock);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        pStatus->lockStatus = NEXUS_Frontend_P_GetLockStatus(lock);
    }
#endif
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetQamStatus(void *handle, NEXUS_FrontendQamStatus *pStatus)
{    
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BADS_Status st;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BADS_GetStatus(pDevice->ads_chn[pChannel->chn_num],  &st);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pStatus->fecLock = st.isFecLock;
    pStatus->receiverLock = st.isQamLock;
    pStatus->rfAgcLevel = st.agcExtLevel;
    pStatus->ifAgcLevel = st.agcIntLevel;
    pStatus->snrEstimate = st.snrEstimate*100/256;
    pStatus->fecCorrected = st.accCorrectedCount;
    pStatus->fecUncorrected = st.accUncorrectedCount;
    pStatus->fecClean = st.cleanCount;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->symbolRate = st.rxSymbolRate;
    pStatus->berEstimate = st.berRawCount;
    pStatus->dsChannelPower = st.dsChannelPower;
    pStatus->mainTap = st.mainTap;
    pStatus->equalizerGain = st.equalizerGain;
    pStatus->postRsBer = 0;/* Not supported */
    pStatus->postRsBerElapsedTime = 0;/* Not supported */
    pStatus->settings = pDevice->last_ads[pChannel->chn_num];
    pStatus->spectrumInverted = st.isSpectrumInverted;

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
    
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetQamAsyncStatus(void *handle, NEXUS_FrontendQamStatus *pStatus)
{
    
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BADS_Status st;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }
    
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BADS_GetAsyncStatus(pDevice->ads_chn[pChannel->chn_num],  &st);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pStatus->fecLock = st.isFecLock;
    pStatus->receiverLock = st.isQamLock;
    pStatus->rfAgcLevel = st.agcExtLevel;
    pStatus->ifAgcLevel = st.agcIntLevel;
    pStatus->snrEstimate = st.snrEstimate*100/256;
    pStatus->fecCorrected = st.accCorrectedCount;
    pStatus->fecUncorrected = st.accUncorrectedCount;
    pStatus->fecClean = st.cleanCount;
    pStatus->carrierPhaseOffset = st.carrierPhaseOffset;
    pStatus->carrierFreqOffset = st.carrierFreqOffset;
    pStatus->symbolRate = st.rxSymbolRate;
    pStatus->berEstimate = st.berRawCount;
    pStatus->dsChannelPower = st.dsChannelPower;
    pStatus->mainTap = st.mainTap;
    pStatus->equalizerGain = st.equalizerGain;
    pStatus->postRsBer = 0;/* Not supported */
    pStatus->postRsBerElapsedTime = 0;/* Not supported */
    pStatus->settings = pDevice->last_ads[pChannel->chn_num];
    pStatus->spectrumInverted = st.isSpectrumInverted;
    pStatus->goodRsBlockCount = st.goodRsBlockCount;
    pStatus->berRawCount = st.berRawCount;
    
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

done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_RequestQamAsyncStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if(pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    rc = BADS_RequestAsyncStatus(pDevice->ads_chn[pChannel->chn_num]);
    if(rc){rc = BERR_TRACE(rc); goto done;}

done:
    return rc;

}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length)
{
    #define TOTAL_ADS_SOFTDECISIONS 30

    int j;
    size_t i;
    int16_t return_length;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    int16_t d_i[TOTAL_ADS_SOFTDECISIONS], d_q[TOTAL_ADS_SOFTDECISIONS];

    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    for( i=0; i<length; i += TOTAL_ADS_SOFTDECISIONS )
    {

        rc = BADS_GetSoftDecision(pDevice->ads_chn[pChannel->chn_num], (int16_t)TOTAL_ADS_SOFTDECISIONS,  d_i, d_q, &return_length);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        for ( j=0; j<TOTAL_ADS_SOFTDECISIONS && i+j<length; j++ )
        {
            pDecisions[i+j].i = d_i[j] * 32;
            pDecisions[i+j].q = d_q[j] * 32;
        }
    }
done:
    return rc;

}

void NEXUS_Frontend_GetDefault31xxConfigSettings(
    NEXUS_31xxConfigSettings *pConfigSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pConfigSettings);
    BKNI_Memset(pConfigSettings, 0, sizeof(*pConfigSettings));
    pConfigSettings->enableDaisyChain = true;
    pConfigSettings->agcValue = 0x1f; /* Max gain*/
}

static NEXUS_31xxChannel *NEXUS_Get31xxChannel(NEXUS_FrontendHandle handle)
{
    NEXUS_31xxChannel *pChannel;
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
     
    if((handle->chip.id == BCHIP_3117) || (handle->chip.id == BCHIP_3114) ||(handle->chip.id == BCHIP_3112) ||(handle->chip.id == BCHIP_3109)) {
        pChannel = (NEXUS_31xxChannel *)handle->pDeviceHandle;
        return pChannel;
    }
    return NULL;
}

NEXUS_Error NEXUS_Frontend_31xx_GetConfigSettings(
    NEXUS_FrontendHandle handle,                 /* [In]  */
    NEXUS_31xxConfigSettings *pConfigSettings    /* [out]  */
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xxChannel *pChannel;
    NEXUS_31xx *pDevice;

    pChannel = NEXUS_Get31xxChannel(handle);    
    if (!pChannel) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done; }

    pDevice = (NEXUS_31xx *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    
    pConfigSettings->updateGainCallback = pDevice->updateGainCallbackDesc;
    pConfigSettings->qamAsyncStatusReadyCallback = pDevice->qamAsyncStatusReadyDesc;
    pConfigSettings->oobAsyncStatusReadyCallback = pDevice->oobAsyncStatusReadyDesc;
    pConfigSettings->agcValue = pDevice->agcValue;
    pConfigSettings->enableDaisyChain = pDevice->enableDaisyChain;
done:
    return rc;
}

NEXUS_Error NEXUS_Frontend_31xx_SetConfigSettings(
    NEXUS_FrontendHandle handle,                 /* [In] */
    const NEXUS_31xxConfigSettings *pConfigSettings    /* [In] */
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned  gainValue;
    NEXUS_31xxChannel *pChannel;
    NEXUS_31xx *pDevice;

    pChannel = NEXUS_Get31xxChannel(handle);    
    if (!pChannel) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done; }

    pDevice = (NEXUS_31xx *)pChannel->pDevice;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    
    
    if(pChannel->chn_num < NEXUS_MAX_31xx_ADSCHN) {
        NEXUS_IsrCallback_Set(pDevice->updateGainAppCallback[pChannel->chn_num], &(pConfigSettings->updateGainCallback));
        pDevice->updateGainCallbackDesc = pConfigSettings->updateGainCallback;

        NEXUS_IsrCallback_Set(pDevice->asyncStatusAppCallback[pChannel->chn_num], &(pConfigSettings->qamAsyncStatusReadyCallback));
        pDevice->qamAsyncStatusReadyDesc = pConfigSettings->qamAsyncStatusReadyCallback;
    }
    else if(pChannel->chn_num == NEXUS_31xx_OOB_CHN) {
        NEXUS_IsrCallback_Set(pDevice->asyncStatusAppCallback[pChannel->chn_num], &(pConfigSettings->oobAsyncStatusReadyCallback));
        pDevice->oobAsyncStatusReadyDesc = pConfigSettings->oobAsyncStatusReadyCallback;
    }

    if (((pDevice->agcValue != pConfigSettings->agcValue)&&(pDevice->tnr[pChannel->chn_num])) && (pChannel->chn_num <= NEXUS_31xx_OOB_CHN))
    {
        rc = BTNR_P_GetTunerAgcRegVal(pDevice->tnr[pChannel->chn_num], BCM31xx_DS_TUNER_REF_DB0_WDATA_01, &gainValue);
        if(rc){rc = BERR_TRACE(rc); goto done;}
        gainValue &= 0x00003fff;
        gainValue |= (pConfigSettings->agcValue << 14);
        rc = BTNR_SetTunerAgcRegVal(pDevice->tnr[pChannel->chn_num], BCM31xx_DS_TUNER_REF_DB0_WDATA_01, &gainValue);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    pDevice->agcValue = pConfigSettings->agcValue;

    if(pDevice->enableDaisyChain != pConfigSettings->enableDaisyChain) {
        rc = BADS_SetDaisyChain(pDevice->ads, pConfigSettings->enableDaisyChain);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }
    pDevice->enableDaisyChain = pConfigSettings->enableDaisyChain;

done:
    return rc;
}

static void NEXUS_Frontend_P_31xx_ResetAdsStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num >= NEXUS_MAX_31xx_ADSCHN)
    {
        BDBG_ERR((" Unsupported channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    rc = BADS_ResetStatus(pDevice->ads_chn[pChannel->chn_num]);
    if (rc){BERR_TRACE(rc);}

done:
    return;
}


/***************************************************************************
Summary:
    Retrieve the chip family id, chip id, chip version and firmware version.
***************************************************************************/
static void NEXUS_Frontend_P_31xx_GetType(void *handle, NEXUS_FrontendType *type)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    uint32_t       familyId, chipId;
    uint16_t       chipVer;
    uint8_t        majApVer, minApVer;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num < NEXUS_MAX_31xx_FRONTENDS);

    type->chip.familyId = (uint32_t)pDevice->chipId;    
    type->chip.id = (uint32_t)pDevice->chipId;
    type->chip.version.major = (pDevice->revId >> 8) + 1;
    type->chip.version.minor = pDevice->revId & 0xff;
    
    rc = BHAB_GetApVersion(pDevice->hab, &familyId, &chipId, &chipVer, &majApVer, &minApVer);

    if(rc || (type->chip.id != chipId)){
        BDBG_ERR(("Type mismatch while retreiving chip id and family id."));
        BERR_TRACE(BERR_UNKNOWN); goto done;
    }
    type->firmwareVersion.major = majApVer;
    type->firmwareVersion.minor = minApVer;
done:
    return;
}


#ifdef BCHIP_HAS_AOB
static NEXUS_Error NEXUS_Frontend_P_31xx_TuneOob(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BAOB_AcquireParam obParams;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);

    if (pChannel->chn_num != NEXUS_31xx_OOB_CHN)
    {
        BDBG_ERR((" Unsupported out of band channel."));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto done;
    }

    NEXUS_IsrCallback_Set(pDevice->lockAppCallback[NEXUS_31xx_OOB_CHN], &(pSettings->lockCallback));

    if (pDevice->tnr[NEXUS_31xx_OOB_CHN])
    {
        rc = BTNR_SetTunerRfFreq(pDevice->tnr[NEXUS_31xx_OOB_CHN], pSettings->frequency, BTNR_TunerMode_eDigital);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    obParams.autoAcquire = pSettings->autoAcquire;
    obParams.modType = (BAOB_ModulationType)pSettings->mode ;
    obParams.symbolRate = pSettings->symbolRate;

    rc = BAOB_Acquire(pDevice->aob, &obParams);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pDevice->last_aob = *pSettings;
done:
    return rc;
}

static void NEXUS_Frontend_P_31xx_UnTuneOob(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BTNR_PowerSaverSettings pwrSettings;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_OOB_CHN);

    rc = BAOB_EnablePowerSaver(pDevice->aob);
    if(rc){rc = BERR_TRACE(rc); goto done;}
done:
    return;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetOobStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BAOB_Status st;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_OOB_CHN);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BAOB_GetStatus(pDevice->aob,  &st);
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
    pStatus->ifFreq = st.ifFreq;
    pStatus->loFreq = st.loFreq;
    pStatus->sysXtalFreq = st.sysXtalFreq;
    pStatus->berErrorCount = st.berErrorCount;

    BDBG_MSG((" OOB STATUS : fec_lock = %d,  qam_lock = %d, snr_estimate = %d, fec_corr_cnt = %d\n",
            st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel, st.snrEstimate,
            st.correctedCount, st.uncorrectedCount, st.berErrorCount));
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetOobAsyncStatus(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BAOB_Status st;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_OOB_CHN);

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
    pStatus->ifFreq = st.ifFreq;
    pStatus->loFreq = st.loFreq;
    pStatus->sysXtalFreq = st.sysXtalFreq;
    pStatus->berErrorCount = st.berErrorCount;

    BDBG_MSG((" OOB ASYNC STATUS : fec_lock = %d,  qam_lock = %d, snr_estimate = %d, fec_corr_cnt = %d\n",
            st.isFecLock, st.isQamLock, st.agcIntLevel, st.agcExtLevel, st.snrEstimate,
            st.correctedCount, st.uncorrectedCount, st.berErrorCount));
done:
    return rc;
}

static NEXUS_Error NEXUS_Frontend_P_31xx_RequestOobAsyncStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_OOB_CHN);

    rc = BAOB_RequestAsyncStatus(pDevice->aob);
    if(rc){rc = BERR_TRACE(rc);}

    return rc;
}

static void NEXUS_Frontend_P_31xx_ResetOobStatus(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_OOB_CHN);

    rc = BAOB_ResetStatus(pDevice->aob);
    if(rc) BERR_TRACE(rc);

    return;
}
#endif

#ifdef BCHIP_HAS_AUS
static NEXUS_Error NEXUS_Frontend_P_31xx_TuneUpstream(void *handle, const NEXUS_FrontendUpstreamSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BAUS_OperationMode operationMode;
    BAUS_TestModeSettings testModeSettings;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_AUS_CHN);

    switch ( pSettings->mode )
    {
    case NEXUS_FrontendUpstreamMode_eAnnexA:
        operationMode = BAUS_OperationMode_eAnnexA;
        break;
    case NEXUS_FrontendUpstreamMode_eDvs178:
        operationMode = BAUS_OperationMode_eDvs178;
        break;
    case NEXUS_FrontendUpstreamMode_eDocsis:
        operationMode = BAUS_OperationMode_eDocsis;
        break;
    case NEXUS_FrontendUpstreamMode_ePodAnnexA:
        operationMode = BAUS_OperationMode_ePodAnnexA;
        break;
    case NEXUS_FrontendUpstreamMode_ePodDvs178:
        operationMode = BAUS_OperationMode_ePodDvs178;
        break;
    case NEXUS_FrontendUpstreamMode_ePn23:
        operationMode = BAUS_OperationMode_eTestPn23;
        break;
    case NEXUS_FrontendUpstreamMode_eSingleCw:
        operationMode = BAUS_OperationMode_eTestSingleCw;
        break;
    case NEXUS_FrontendUpstreamMode_eDoubleCw:
        operationMode = BAUS_OperationMode_eTestDoubleCw;
        break;
    default:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BKNI_Memset(&testModeSettings, 0, sizeof(BAUS_TestModeSettings));

    testModeSettings.nyquistAlphaValue = pSettings->testModeSettings.nyquistAlphaValue;
    testModeSettings.spectrumInverted = pSettings->testModeSettings.spectrumInverted;
    testModeSettings.qamMode = pSettings->testModeSettings.qamMode;
    testModeSettings.bypassFilters = pSettings->testModeSettings.bypassFilters;
    testModeSettings.rfFreq2 = pSettings->testModeSettings.rfFreq2;
    testModeSettings.operationMode = operationMode;

    rc = BAUS_EnableTransmitter( pDevice->aus );
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BAUS_SetOperationMode(pDevice->aus, operationMode);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    if(testModeSettings.operationMode > NEXUS_FrontendUpstreamMode_ePodStarvue ) {
        rc = BAUS_SetTestModeParams(pDevice->aus, &testModeSettings);
        if(rc){rc = BERR_TRACE(rc); goto done;}
    }

    rc = BAUS_SetRfFreq(pDevice->aus, pSettings->frequency);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BAUS_SetPowerLevel(pDevice->aus, pSettings->powerLevel);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    rc = BAUS_SetSymbolRate(pDevice->aus, pSettings->symbolRate);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pDevice->last_ups = *pSettings;
done:
    return rc;
}

static void NEXUS_Frontend_P_31xx_UnTuneUpstream(void *handle)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_AUS_CHN);

    rc = BAUS_DisableTransmitter(pDevice->aus);
    if(rc){BERR_TRACE(rc);}
}

static NEXUS_Error NEXUS_Frontend_P_31xx_GetUpstreamStatus(void *handle, NEXUS_FrontendUpstreamStatus *pStatus)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct BAUS_Status st;
    NEXUS_31xx *pDevice;
    NEXUS_31xxChannel *pChannel;
    BDBG_ASSERT(NULL != handle);
    pChannel = (NEXUS_31xxChannel *)handle;
    pDevice = pChannel->pDevice;
    BDBG_ASSERT(NULL != pDevice);
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_31xx);
    BDBG_ASSERT(pChannel->chn_num == NEXUS_31xx_AUS_CHN);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    rc = BAUS_GetStatus(pDevice->aus,  &st);
    if(rc){rc = BERR_TRACE(rc); goto done;}

    pStatus->frequency = st.rfFreq;
    pStatus->powerLevel = st.powerLevel;
    pStatus->symbolRate = st.symbolRate;
    pStatus->sysXtalFreq = st.sysXtalFreq;

    switch ( st.operationMode )
    {
        case BAUS_OperationMode_eAnnexA:
            pStatus->mode = NEXUS_FrontendUpstreamMode_eAnnexA;
            break;
        case BAUS_OperationMode_eDvs178:
            pStatus->mode = NEXUS_FrontendUpstreamMode_eDvs178;
            break;
        case BAUS_OperationMode_eDocsis:
            pStatus->mode = NEXUS_FrontendUpstreamMode_eDocsis;
            break;
        case BAUS_OperationMode_ePodAnnexA:
            pStatus->mode = NEXUS_FrontendUpstreamMode_ePodAnnexA;
            break;
        case BAUS_OperationMode_ePodDvs178:
            pStatus->mode = NEXUS_FrontendUpstreamMode_ePodDvs178;
            break;
        default:
            return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
done:
    return rc;
}
#endif

