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
 * $brcm_Workfile: nexus_hdmi_input_module.h $
 * $brcm_Revision: 28 $
 * $brcm_Date: 6/19/12 12:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/3548/src/nexus_hdmi_input_module.h $
 * 
 * 28   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 27   4/18/12 1:17p rgreen
 * SW7425-1988: Schedule hdcpRxChanged task during isr time rather than
 *  setting/waiting for an event during task time
 * 
 * 26   3/28/12 4:27p rgreen
 * SW7425-2515: Remove CEC references in nexus hdmiInput module; see cec
 *  support in cec module
 * 
 * 25   8/9/11 3:20p rgreen
 * SW7425-692: Fix 35230 compilation for non power management build
 * 
 * 24   8/3/11 3:28p gmohile
 * SW7425-692 : Add hdmi input standby
 * 
 * 23   4/18/11 3:45p rgreen
 * SW7422-186: Update hdcpRxChange callback name
 * 
 * 22   4/18/11 1:36p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 * 
 * SW7422-186/4   4/15/11 6:08p rgreen
 * SW7422-186: Remove hdcpKeyset from hdmiInputSettings; Add to  hdmiInput
 *  handle
 * 
 * SW7422-186/3   4/13/11 1:56p rgreen
 * SW7422-186: Replace RequestKsvFifoEvent with generic hdcpEvent
 * 
 * SW7422-186/2   4/7/11 2:38p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * SW7422-186/1   3/31/11 2:47p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * 21   3/3/11 1:26p rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 *  OTP ROM based chips
 * 
 * 20   2/16/11 6:22p rgreen
 * SW7422-129,SW35230-2374: Merge changes
 * 
 * SW7422-129/3   2/10/11 3:49p rgreen
 * SW7422-129: Remove hdcpSettings from hdmiInputSettings; create separate
 *  hdcpSettings
 * 
 * SW7422-129/2   2/9/11 9:42a rgreen
 * SW7422-129,SW35230-2374: Replace KeySet status structure with universal
 *  stHdcpStatus structure to support both OTP RAM and OTP ROM status
 * 
 * SW7422-129/1   2/2/11 6:27p rgreen
 * SW7422-129: Add support for HDCP Key loading to OTP RAM
 * 
 * 19   1/31/11 3:23p rgreen
 * SW35125-12: Fix nexus compilation warning
 * 
 * 18   12/23/10 2:27p rgreen
 * SW35230-2374: Update naming convention for
 *  NEXUS_HdmiInputHdcpKeySetOtpStatus
 * 
 * SW35230-2374/2   12/17/10 5:54p rgreen
 * SW35230-2374: Create separate file for HDCP related functions
 * 
 * SW35230-2374/1   12/3/10 9:09p rgreen
 * SW35230-2374: Add support for OTP HDCP Key verfication
 * 
 * 16   7/13/10 9:03p rgreen
 * SW35230-517: Fix compilation warnings
 * 
 * 15   5/12/10 4:28p rgreen
 * SW3548-2795,SW3548-2642:Merge Changes
 * 
 * SW3548-2642/2   5/3/10 11:51a rgreen
 * SW3548-2795: Add hdmiInputMaster to hdmiInput handle
 *
 * SW3548-2642/1   4/20/10 4:13p rgreen
 * SW3548-2795: support dynamic core and frontend configuration for 3D
 *  mode
 *
 * 14   3/18/10 6:53p jhaberf
 * SW35230-1: Removed hdmi_input CEC files from the 35230 build
 *
 * 13   2/3/10 3:03p erickson
 * SW3548-2727: merge 3DTV support
 *
 * BCM3D/2   1/19/10 4:48p erickson
 * SW3548-2727: add 3DTV support
 *
 * 12   1/29/10 6:55p rgreen
 * SW3556-1022,SW3548-2745: Update nexus shutdown order; remove unused
 *  event and handler
 *
 * 11   1/25/10 4:29p erickson
 * SW3556-1021: must cancel timer in NEXUS_HdmiInput_Close to prevent
 *  NEXUS_HdmiInput_P_ReleaseHotPlug from being called after close. also
 *  added some missing asserts.
 *
 * 10   11/30/09 3:33p rgreen
 * SW35230-15: Add support for 35230
 *
 * 9   10/16/09 4:58p rgreen
 * SW3548-2013:  for debug; report only hdmiInput status changes
 *
 * 8   4/9/09 4:24p rgreen
 * PR52879: Add EDID RAM Support
 *
 * PR52879/1   3/23/09 2:00p rgreen
 * PR52879: Add EDID RAM Support
 *
 * 7   3/6/09 11:08a rgreen
 * PR51307: Add HDMI HDCP Repeater Support
 *
 * 6   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 5   1/26/09 11:30a erickson
 * PR51468: global variable naming convention
 *
 * 4   10/8/08 2:25a erickson
 * PR46176: Implement Power Management; move frontend inside of the
 *  hdmiInput object
 *
 * 3   8/7/08 8:29a erickson
 * PR45358: intial checkin
 *
 * 2   5/12/08 11:07a erickson
 * PR39453: HdmiInputFrontend impl
 *
 * 1   5/6/08 4:45p erickson
 * PR39453: add separate impl for 3548/3556
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   1/9/08 6:19p honglang
 * PR36814: Add header file nexus_hdmi_input_ext.h
 *
 * Nexus_Devel/1   11/8/07 2:40p erickson
 * PR36814: add hdmi_input
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_INPUT_MODULE_H__
#define NEXUS_HDMI_INPUT_MODULE_H__

#include "nexus_hdmi_input_thunks.h"
#include "nexus_platform_features.h"
#include "nexus_base.h"
#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_init.h"
#include "nexus_hdmi_input_ext.h"
#include "nexus_hdmi_input_hdcp.h"
#include "bhdr.h"
#include "bhdr_fe.h"
#include "bhdr_hdcp.h"
#if NEXUS_POWER_MANAGEMENT
#include "bhdr_pwr.h"
#endif

#include "priv/nexus_core.h"
#include "priv/nexus_core_video.h"
#include "priv/nexus_hdmi_input_priv.h"
#if NEXUS_HAS_AUDIO
#include "priv/nexus_core_audio.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME hdmiInput
#define NEXUS_MODULE_SELF g_NEXUS_hdmiInputModule

/* global handle. there is no global data. */
extern NEXUS_ModuleHandle g_NEXUS_hdmiInputModule;

BDBG_OBJECT_ID_DECLARE(NEXUS_HdmiInput);

struct NEXUS_HdmiInput {
    NEXUS_OBJECT(NEXUS_HdmiInput);
    BHDR_FE_ChannelHandle frontend; /* current connection */

    NEXUS_IsrCallbackHandle hotPlugCallback;
    NEXUS_HdmiInputHandle masterHdmiInput;

    bool videoConnected;
    NEXUS_HdmiInputSettings settings;
    NEXUS_HdmiInputHdcpSettings hdcpSettings ;

    BAVC_Colorspace manualColorSpace; /* cached mapping of color space (to avoid isr-time conversion) */
    BAVC_Colorspace reportedColorSpace; /* color space from the receiver */
    NEXUS_VideoInputObject videoInput;
#if NEXUS_HAS_AUDIO
    NEXUS_AudioInputObject audioInput;
#endif
    uint8_t index;
    BHDR_Handle hdr;

    NEXUS_HdmiInputHdcpStatus stHdcpStatus ;
    NEXUS_HdmiInputHdcpKeyset	hdcpKeyset ;

#if BHDR_CONFIG_EDID_RAM
    BHDR_EDID_Info stEdidInfo ;
#endif

    BKNI_EventHandle frameRateEvent, formatEvent;
    NEXUS_EventCallbackHandle frameRateHandler, formatHandler;

    NEXUS_IsrCallbackHandle avMuteCallback;
    NEXUS_IsrCallbackHandle aviInfoFrameChanged;
    NEXUS_IsrCallbackHandle audioInfoFrameChanged;
    NEXUS_IsrCallbackHandle spdInfoFrameChanged;
    NEXUS_IsrCallbackHandle vendorSpecificInfoFrameChanged;
    NEXUS_IsrCallbackHandle audioContentProtectionChanged;
    NEXUS_IsrCallbackHandle gamutMetadataPacketChanged;
    NEXUS_IsrCallbackHandle hdcpKeysetStatusUpdate ;

    NEXUS_IsrCallbackHandle hdcpRxChanged ;
    NEXUS_TaskCallbackHandle sourceChangedCallback;

    NEXUS_TimerHandle releaseHotPlugTimer;
	
    /* status */
    bool bInitialized;
    bool bSentResetHdDviBegin;
    bool avMute;
    uint8_t uiFormatChangeMuteCount ;
    BAVC_VDC_HdDvi_Picture stFieldData;
    NEXUS_VideoFrameRate frameRate;
    NEXUS_HdmiInputStatus vdcStatus;
#if BDBG_DEBUG_BUILD
    NEXUS_HdmiInputStatus previousVdcStatus;
#endif
    NEXUS_HdmiInputFormatChangeCb pPcFormatCallback_isr;
    void *pPcFormatCallbackParam;
};

BDBG_OBJECT_ID_DECLARE(NEXUS_HdmiInputFrontend);

struct NEXUS_HdmiInputFrontend
{
    BDBG_OBJECT(NEXUS_HdmiInputFrontend)
    unsigned index;
    BHDR_FE_ChannelHandle channel;
    NEXUS_IsrCallbackHandle hotPlugCallback;
    NEXUS_HdmiInputHandle attachment;
};

#if (HDMI_RX_GEN == 35230)
void NEXUS_HdmiInputTvm_P_HotPlugDisable(void) ;
void NEXUS_HdmiInputTvm_P_HotPlugEnable(void) ;
void NEXUS_HdmiInputTvm_P_EdidRam_LoadData(const uint8_t *dataBytes, uint16_t numEdidBytes) ;
void NEXUS_HdmiInputTvm_P_EdidRam_Enable(void) ;
#endif

/* Proxy conversion */
#define NEXUS_P_HDMI_INPUT_HDCP_KSV_SIZE(num) ((num)*sizeof(NEXUS_HdmiOutputHdcpKsv))

#ifdef __cplusplus
}
#endif

#endif
