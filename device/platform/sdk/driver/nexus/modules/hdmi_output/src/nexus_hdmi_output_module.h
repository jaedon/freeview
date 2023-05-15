/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_output_module.h $
 * $brcm_Revision: 33 $
 * $brcm_Date: 6/20/12 2:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/7425/src/nexus_hdmi_output_module.h $
 * 
 * 33   6/20/12 2:35p rgreen
 * SW7358-263, SW7425-3206: Use polling to detect RxSense state instead of
 *  interrupts
 * 
 * 32   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 31   5/31/12 2:24p rgreen
 * SW7425-3149: Enable TMDS lines in one location
 * 
 * 30   4/18/12 11:28a rgreen
 * SW7552-163: Merge changes for variable timer to detect RxSense
 * 
 * SW7552-163/2   4/16/12 2:48p mward
 * SW7552-163:  Merge from main
 * 
 * SW7552-163/1   4/9/12 7:14p rgreen
 * SW7552-163: Implement variable timer for checking Receiver Sense
 *  (RxSense) power after a hot plug connect event
 * 
 * 29   4/13/12 4:58p erickson
 * SW7346-737: add NEXUS_HdmiOutput_GetCrcData
 *
 * 28   3/26/12 3:20p vle
 * SW7425-2515: Merged to mainline
 *
 * SW7425-2515/1   3/23/12 6:19p vle
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 *  related code out of hdmi_output module
 *
 * 27   2/23/12 11:34a rgreen
 * SW7125-1146,SW7408-317: Merge changes
 *
 * SW7408-317/2   2/22/12 1:14p rgreen
 * SW7125-1146,SW7408-317:use _isr designation for isr variables
 *
 * SW7408-317/1   2/21/12 6:33p rgreen
 * SW7125-1146,SW7408-317: Treat RxSense and HP events separately.  Update
 *  processing of both events
 *
 * 26   1/12/12 11:24a gmohile
 * SW7425-1708 : S3 warm boot support
 *
 * 25   12/19/11 12:35p vle
 * SW7425-1140: Fix build issue without CEC support. CEC functionalities
 *  in NEXUS_HDMI_OUTPUT modules is enabled with build option
 *  BHDM_CEC_SUPPORT=y
 *
 * 24   12/16/11 6:01p vle
 * SW7425-1140: Disable CEC functionalities in NEXUS_HDMI_OUTPUT modules.
 *
 * 23   11/22/11 6:11p vle
 * SW7425-1140: Update nexus hdmi_output cec to use new BCEC PI instead of
 *  BHDM_CEC
 *
 * SW7425-1140/2   11/22/11 2:00p vle
 * SW7425-1140: fix build errors when build without CEC support.
 *
 * SW7425-1140/1   11/16/11 12:19p vle
 * SW7425-1140: Update to use new CEC PI instead of BHDM_CEC
 *
 * 22   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 *
 * 21   2/16/11 10:14a gmohile
 * SW7408-210 : Fix kernel mode build
 *
 * 20   1/14/11 6:27p vle
 * SW7405-5062: Merge to mainline
 *
 * SW7405-5062/1   12/16/10 3:09p vle
 * SW7405-5062: Make sure all hotplug event (disconnect or connect) get
 *  served correctly.
 *
 * 19   11/5/10 5:40p jgarrett
 * SW7125-698: Applying HDMI channel status information when it updates
 *
 * 18   10/8/10 11:58a vle
 * SW7405-4782: Merge to main branch
 *
 * SW7405-4782/1   10/7/10 12:04p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication - TAKE 2. First implementation attempt has a potential
 *  race condition.
 *
 * 17   3/3/10 12:42p vle
 * SW7405-3987: merge to main branch
 *
 * SW7405-3987/1   3/2/10 5:39p vle
 * SW7405-3987: Report specific HDCP error status to high level
 *  application.
 *
 * 16   1/7/10 10:06p rgreen
 * SW7405-3740: Use isr function to make sure all HP events are processed
 *
 * 15   9/24/09 4:48p jgarrett
 * SW7405-2818: Allowing HDMI audio without video
 *
 * 14   9/16/09 6:06p vle
 * SW7405-2885: Fix 7420 build issue. Make sure HDMI HDCP debug functions
 *  are wrap around with NEXUS_HAS_SECURITY
 *
 * 13   9/15/09 4:48p gmohile
 * SW7405-2885 : new debug functions
 *
 * 12   8/25/09 3:28p vle
 * SW7405-2670:
 * Add support to enable/disable TMDS lines
 * Do not poll for receiver sense while encryption is still enabled.
 * Add implementation of interrupt based receiver sense
 *
 * 11   6/24/09 3:12p mphillip
 * PR54926: Add /proc/hdmi_output
 *
 * 10   1/26/09 1:55p erickson
 * PR51468: global variable naming convention
 *
 * 9   12/17/08 11:38p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * 8   12/15/08 10:10a erickson
 * PR49583: merge
 *
 * PR49583/1   12/12/08 5:42p vle
 * PR49583: Fix compiler warning
 *
 * 7   8/1/08 4:26p erickson
 * PR39979: add CEC support
 *
 * 6   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 5   3/31/08 1:53p erickson
 * PR41075: add BDBG_OBJECT
 *
 * 4   3/17/08 12:39p erickson
 * PR40103: replace NEXUS_QueueCallback with NEXUS_TaskCallback
 *
 * 3   2/26/08 5:42p jgarrett
 * PR 39017: Enabling HDCP
 *
 * 2   2/21/08 5:59p jgarrett
 * PR 39017: Adding HDCP
 *
 * 1   2/1/08 5:25p jgarrett
 * PR 39017: Adding hdmi_output
 *
 **************************************************************************/

#include "nexus_base.h"
#include "nexus_hdmi_output_thunks.h"
#include "nexus_hdmi_output.h"
#include "nexus_hdmi_output_hdcp.h"
#include "nexus_hdmi_output_init.h"
#include "priv/nexus_hdmi_output_standby_priv.h"
#include "nexus_hdmi_output_extra.h"
#include "bhdm.h"
#include "bhdm_edid.h"
#include "nexus_platform_features.h"
#include "priv/nexus_core_video.h"
#include "priv/nexus_core_audio.h"

#if NEXUS_HAS_SECURITY
#include "bhdm_hdcp.h"
#include "bhdcplib.h"
#include "bhdcplib_keyloader.h"
#endif

#ifndef NEXUS_HDMI_OUTPUT_MODULE_H__
#define NEXUS_HDMI_OUTPUT_MODULE_H__

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME hdmi_output
#define NEXUS_MODULE_SELF g_NEXUS_hdmiOutputModule

/* Global Types */
typedef enum NEXUS_HdmiOutputState
{
    NEXUS_HdmiOutputState_eDisconnected,
    NEXUS_HdmiOutputState_eConnected,
    NEXUS_HdmiOutputState_ePoweredDown,
    NEXUS_HdmiOutputState_eMax
} NEXUS_HdmiOutputState;

typedef enum NEXUS_HdmiOutputLogicalAddrSearch
{
    NEXUS_HdmiOutputLogicalAddrSearch_eInit,
    NEXUS_HdmiOutputLogicalAddrSearch_eNext,
    NEXUS_HdmiOutputLogicalAddrSearch_eReady
} NEXUS_HdmiOutputLogicalAddrSearch;

typedef struct NEXUS_HdmiOutput
{
    NEXUS_OBJECT(NEXUS_HdmiOutput);
    bool opened;
    bool videoConnected;
    bool tmdsEnabled ;

#if defined(HUMAX_PLATFORM_BASE)
/* added  to distinguish (active) standby mode */
	bool devicePowered;
#endif

    /* isr context */
    bool forceDisconnect_isr ;
    NEXUS_HdmiOutputState lastHotplugState_isr ;

    NEXUS_HdmiOutputState lastState;
    NEXUS_HdmiOutputState lastCallbackState;
    NEXUS_VideoOutputObject videoConnector;
    NEXUS_AudioOutputObject audioConnector;
    BHDM_Handle hdmHandle;
    NEXUS_HdmiOutputOpenSettings openSettings;
    NEXUS_HdmiOutputSettings settings;
    NEXUS_EventCallbackHandle hotplugEventCallback;
    NEXUS_EventCallbackHandle rxSenseEventCallback;
    NEXUS_TimerHandle powerTimer;
    bool enableHdcpPostFormatChange;
    bool formatChangeMute;
    bool avMuteSetting;
    bool hdcpStarted;
	
    NEXUS_TimerHandle connectTimer;
    uint8_t checkRxSenseCount ;
    uint8_t lastReceiverSense ;

    NEXUS_TimerHandle postFormatChangeTimer;
    bool aspectRatioChangeOnly;

    BAVC_AudioSamplingRate sampleRate;
    BAVC_AudioBits audioBits;
    BAVC_AudioFormat audioFormat;
    unsigned audioNumChannels;    /* PCM only */

    NEXUS_HdmiOutputLogicalAddrSearch searchState;
    unsigned logAddrSearchIndex;

    NEXUS_TaskCallbackHandle hdcpFailureCallback;
    NEXUS_TaskCallbackHandle hdcpStateChangedCallback;
    NEXUS_TaskCallbackHandle hdcpSuccessCallback;
    NEXUS_TaskCallbackHandle hotplugCallback;
    NEXUS_TaskCallbackHandle notifyDisplay;
    BKNI_EventHandle notifyAudioEvent;

#if NEXUS_HAS_SECURITY
    BHDCPlib_Handle hdcpHandle;
    NEXUS_EventCallbackHandle hdcpHotplugCallback;
    NEXUS_EventCallbackHandle riCallback;
    NEXUS_EventCallbackHandle pjCallback;
    NEXUS_TimerHandle hdcpTimer;
    NEXUS_HdmiOutputHdcpSettings hdcpSettings;
    NEXUS_HdmiOutputHdcpKsv *pRevokedKsvs;
    uint16_t numRevokedKsvs;
    BHDCPlib_State hdcpState;
    BHDCPlib_HdcpError hdcpError;
#endif
    bool resumeFromS3;
    BHDM_Settings hdmSettings;

    struct {
        NEXUS_HdmiOutputCrcData *queue;
        unsigned size; /* num entries, not num bytes */
        unsigned rptr, wptr;
    } crc;
} NEXUS_HdmiOutput;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_HdmiOutput);

/* Global module handle & data */
extern NEXUS_ModuleHandle g_NEXUS_hdmiOutputModule;
extern NEXUS_HdmiOutputModuleSettings g_NEXUS_hdmiOutputModuleSettings;

/* Internal Private Routines */
NEXUS_Error NEXUS_HdmiOutput_P_Shutdown(void);

NEXUS_HdmiOutputState NEXUS_HdmiOutput_P_GetState(NEXUS_HdmiOutputHandle output);

NEXUS_Error NEXUS_HdmiOutput_P_InitHdcp(NEXUS_HdmiOutputHandle output);
NEXUS_Error NEXUS_HdmiOutput_P_UninitHdcp(NEXUS_HdmiOutputHandle output);
void NEXUS_HdmiOutput_P_HdcpNotifyHotplug(NEXUS_HdmiOutputHandle output);

void NEXUS_HdmiOutputModule_Print(void);

/* Proxy conversion */
#define NEXUS_P_HDMI_OUTPUT_HDCP_KSV_SIZE(num) ((num)*sizeof(NEXUS_HdmiOutputHdcpKsv))

#endif /* #ifndef NEXUS_HDMI_OUTPUT_MODULE_H__ */


