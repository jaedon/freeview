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
 * $brcm_Workfile: nexus_video_output.c $
 * $brcm_Revision: 83 $
 * $brcm_Date: 9/17/12 2:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_video_output.c $
 * 
 * 83   9/17/12 2:29p erickson
 * SW7435-344: automatically remove outputs when shutdown
 * 
 * 82   8/1/12 10:22a erickson
 * SW7552-307: back out change
 * 
 * 80   7/24/12 12:36p vsilyaev
 * SW7425-3544: Use static allocation for large data structures
 * 
 * 79   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 78   5/15/12 3:02p erickson
 * SW7346-792: only resolve NEXUS_DisplayAspectRatio_eAuto when converting
 *  to magnum types to remove ambiguity
 * 
 * 77   4/23/12 5:44p erickson
 * SW7346-792: check return codes for enum conversion and don't proceed
 *  with uninitialized value
 * 
 * 76   4/13/12 5:02p erickson
 * SW7346-737: add vsync isr for HDMI Tx CRC
 * 
 * 75   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 74   3/7/12 3:51p erickson
 * SW7425-2344: disable mpaa decimation on disconnect of component and
 *  hdmi
 * 
 * 73   3/6/12 4:49p erickson
 * SW7425-2572: check return code on HDMI format conversion functions and
 *  don't proceed with uninitialized values
 * 
 * 72   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * SW7425-1565/1   10/20/11 4:28p mlei
 * SW7425-1565: [Display-Output]: the setting for mpaaDecimation doesn't
 *  apply change.
 * 
 * 70   7/28/11 3:37p erickson
 * SW7346-319: merge
 * 
 * 69   7/22/11 9:03a erickson
 * SW7425-745: add NEXUS_VideoOutputSettings
 * 
 * 68   7/19/11 11:14a erickson
 * SW7408-291: add NEXUS_VideoOutput_SetVfFilter, requires reverse lookup
 *  of DAC assignments
 * 
 * 67   4/25/11 6:00p akam
 * SWDTV-6275: Included build for 35233.
 * 
 * 66   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 * 
 * 65   12/3/10 9:30a erickson
 * SW7550-577: call BHDM_SetHdmiDataTransferMode based on
 *  NEXUS_DisplayTimingGenerator_eHdmiDvo
 * 
 * 64   12/2/10 8:14p spothana
 * SW7420-1177: Adding nexus hdmi dvo module.
 * 
 * SW7420-1177/2   11/5/10 3:28p spothana
 * SW7420-1177: Display format changes require changes to HDMI rate
 *  manager.
 * 
 * SW7420-1177/1   11/1/10 8:48p spothana
 * SW7420-1177: Adding nexus hdmi dvo module.
 * 
 * 63   11/22/10 4:51p jhaberf
 * SW35125-30: added rudimentary 35125 DTV chip support to nexus display
 * 
 * 62   10/8/10 4:30p erickson
 * SW7405-4782: merge
 * 
 * SW7405-4782/2   10/7/10 12:04p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication - TAKE 2. First implementation attempt has a potential
 *  race condition.
 *
 * SW7405-4782/1   9/10/10 8:42p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication.
 *
 * 61   2/23/10 5:16p petlee
 * SW35230-18: Add check for BCHP_CHIP for 35230
 *
 * 60   1/25/10 4:43p erickson
 * SW7405-3486: implement NEXUS_HdmiOutputSettings.syncOnly
 *
 * 59   12/21/09 4:07p erickson
 * SW3556-935: VDC api change for spread spectrum
 *
 * 58   9/17/09 4:10p erickson
 * SW7405-3033: back out last change. fix datatype instead.
 *
 * 56   9/4/09 5:03p jtna
 * SW7405-2962: notify display on hotplug even if HDMI cable initially
 *  unplugged
 *
 * 55   9/1/09 3:58p gmohile
 * SW7403-869 : Fix mpaa mode for 7403
 *
 * 54   8/28/09 9:36a gmohile
 * SW7403-796 : Add legacy vdc support
 *
 * 53   8/21/09 1:23p gmohile
 * PR 57813: Merge legacy vdc support to mainline
 *
 * 52   7/15/09 1:40p erickson
 * PR56609: merge
 *
 * PR56609/1   7/15/09 11:34a rgreen
 * PR56609: Add HDMI Spread Spectrum support
 *
 * 51   6/12/09 11:13a erickson
 * PR55952: handle default aspect ratio for 1920x1080 display formats in a
 *  consistent way
 *
 * 50   6/8/09 7:06a erickson
 * PR55617: rename NEXUS_P_DisplayAspectRatio_ToMagnum
 *
 * 49   5/20/09 11:29a erickson
 * PR55292: call NEXUS_VideoOutput_P_DestroyLink in
 *  NEXUS_Display_RemoveOutput
 *
 * 48   5/14/09 9:02a erickson
 * PR55101: add NEXUS_ComponentOutputSettings.dacs.RGB.noSync
 *
 * 47   5/4/09 3:40p erickson
 * PR53373: merge HDMI 1.3a support
 *
 * PR53373_DeepColor/1   4/16/09 3:58p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 46   4/7/09 11:42a erickson
 * PR53623: change #define to NEXUS_NUM_656_OUTPUTS
 *
 * 45   4/1/09 12:16p erickson
 * PR53623: merge 656 output support
 *
 * PR53623/2   3/31/09 11:57a mward
 * PR 53623: Dummy to prevent compiler warning.  Add call to
 *  NEXUS_VideoOutput_P_OpenCcir656() in NEXUS_P_VideoOutput_Link().
 *
 * PR53623/1   3/31/09 10:07a mward
 * PR 53623: 656 output support.
 *
 * 44   3/25/09 5:26p erickson
 * PR52818: remove 656 bypass. not supported.
 *
 * 43   3/24/09 2:36p erickson
 * PR52818: added 656 bypass
 *
 * 42   3/16/09 4:37p erickson
 * PR53271: added manual colorSpace option to NEXUS_HdmiOutputSettings
 *
 * 41   3/11/09 9:56a erickson
 * PR52991: fix IFD bypass enum. added comment for future ccir656 support.
 *
 * 40   3/10/09 9:35a erickson
 * PR52991: changed NEXUS_CompositeOutputSettings.analogVideoDecoder to
 *  more generic NEXUS_CompositeOutputSettings.inputBypass
 *
 * 39   3/9/09 11:09a jtna
 * PR52387: added NEXUS_Rfm_P_FormatChange
 *
 * 38   2/20/09 11:43a erickson
 * PR52347: fix settop
 *
 * 37   2/20/09 11:31a erickson
 * PR52347: update connected outputs in SetSettings in a standard way
 *
 * 36   2/20/09 10:55a erickson
 * PR52270: allow composite output for HD display format if
 *  NEXUS_CompositeOutputSettings.analogVideoDecoder is set
 *
 * 35   12/18/08 3:23p erickson
 * PR50501: fix DTV
 *
 * 34   12/17/08 11:40p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * HDMI_TX_Plugfest11_200811/1   11/10/08 7:12p vle
 * PR48842: Fix HDCP in BRUTUS. Make sure to report the status to Brutus
 *  and restart HDCP authentication process on each format change.
 *
 * 33   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * 32   9/16/08 5:14p erickson
 * PR46850: fix warning, improve error check
 *
 * 31   9/12/08 10:43a erickson
 * PR46850: improve DBG feedback if HDMI output is not connected. don't
 *  raise false errors.
 *
 * 30   8/20/08 11:58a erickson
 * PR45794: remove bad assert
 *
 * 29   8/19/08 12:22p erickson
 * PR45794: clarify internal function names. add ASSERT's and comments.
 *
 * 28   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 27   8/1/08 5:38p erickson
 * PR45361: c89 fixes
 *
 * 26   7/7/08 3:19p erickson
 * PR44452: closing video outputs should automatically remove them from
 *  displays
 *
 * 25   6/25/08 11:44a erickson
 * PR39453: moved xvYccEnabled to NEXUS_DisplaySettings
 *
 * 24   6/19/08 2:32p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 23   6/17/08 4:06p vsilyaev
 * PR 40921: Added output portion of scart inputs
 *
 * 22   6/13/08 11:06a erickson
 * PR43651: fix BDBG_MODULE
 *
 * 21   6/7/08 2:42p jgarrett
 * PR 43318: Initial 3549 bringup
 *
 * 20   5/21/08 11:29a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 19   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 18   5/12/08 4:55p erickson
 * PR39453: add NEXUS_HdmiOutputSettings.xvYccEnabled
 *
 * 17   5/12/08 4:09p erickson
 * PR42628: free NEXUS_VideoOutput_P_Link when DisplayModule closes
 *
 * 16   5/1/08 10:14a erickson
 * PR39453: fix check if CompositeOutput is already connected
 *
 * 15   4/30/08 5:12p erickson
 * PR39453: fix check
 *
 * 14   4/30/08 4:34p erickson
 * PR39453: fix static init
 *
 * 13   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 12   4/17/08 11:24a erickson
 * PR40157: use BVDC_DacOutput_eGreen_NoSync if dacHSync is set for VGA
 *  output mode
 *
 * 11   4/11/08 11:05a erickson
 * PR40157: add HSync option to ComponentOutput for VGA output
 *
 * 10   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 9   4/2/08 10:09a erickson
 * PR41222: check rc
 *
 * 8   4/1/08 1:46p jgarrett
 * PR 40606: Caching rate information for HDMI
 *
 * 7   3/20/08 6:15p rjlewis
 * PR40352: type mismatch compile error under VxWorks.
 *
 * 6   2/20/08 1:35p erickson
 * PR39405: added RFM output
 *
 * 5   2/19/08 3:00p erickson
 * PR39737: fix malloc check
 *
 * 4   2/5/08 2:17p jgarrett
 * PR 39017: Forcing connect/disconnect/formatChange to always return
 *  success, even if device disconnected (HDMI)
 *
 * 3   2/1/08 5:25p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 2   1/21/08 3:50p erickson
 * PR38862: added NEXUS_DisplayType_eLvds which is the default. use
 * DisplayType to select DVO or LVDS.
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/8   11/15/07 1:37p erickson
 * PR37136: added Dvo
 *
 * Nexus_Devel/7   11/6/07 1:28p erickson
 * PR36570: remove static
 *
 * Nexus_Devel/6   11/6/07 1:13p erickson
 * PR36570: merge 3563
 *
 * Nexus_Devel/5   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/4   10/4/07 1:09p vsilyaev
 * PR 34662: Removed VideoOutputDesc
 *
 * Nexus_Devel/3   9/25/07 3:13p vsilyaev
 * PR 34662: Fixed video output
 *
 * Nexus_Devel/2   9/25/07 1:02p vsilyaev
 * PR 34662: Fixed file names
 *
 * Nexus_Devel/1   9/17/07 6:58p vsilyaev
 * PR 34662: Implementation of display module
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#if NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_hdmi_output_hdcp.h"
#endif
BDBG_MODULE(nexus_video_output);

#if NEXUS_NUM_RFM_OUTPUTS
#include "priv/nexus_rfm_priv.h"
#endif

static const NEXUS_SvideoOutputSettings NEXUS_SvideoOutputDefaultSettings = {
    NEXUS_VideoDac_eNone,
    NEXUS_VideoDac_eNone
};

static const NEXUS_CompositeOutputSettings NEXUS_CompositeOutputDefaultSettings = {
    NEXUS_VideoDac_eNone
};

static const NEXUS_Ccir656OutputSettings NEXUS_Ccir656OutputDefaultSettings = {
    {0}
};

static BERR_Code NEXUS_ComponentOutput_P_Connect(void *output,  NEXUS_DisplayHandle display);
static BERR_Code NEXUS_CompositeOutput_P_Connect(void *output,  NEXUS_DisplayHandle display);
static BERR_Code NEXUS_SvideoOutput_P_Connect(void *output,  NEXUS_DisplayHandle display);
static BERR_Code NEXUS_Ccir656Output_P_Connect(void *output,  NEXUS_DisplayHandle display);

void
NEXUS_VideoOutputs_P_Init(void)
{
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    unsigned i;
    BSTD_UNUSED(i);
    BSTD_UNUSED(state);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    for(i=0;i<sizeof(state->outputs.component)/sizeof(state->outputs.component[0]);i++) {
        BDBG_OBJECT_INIT(&state->outputs.component[i], NEXUS_ComponentOutput);
        NEXUS_ComponentOutput_GetDefaultSettings(&state->outputs.component[i].cfg);
        state->outputs.component[i].opened = false;
        NEXUS_VIDEO_OUTPUT_INIT(&state->outputs.component[i].output, NEXUS_VideoOutputType_eComponent, &state->outputs.component[i]);
    }
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    for(i=0;i<sizeof(state->outputs.composite)/sizeof(state->outputs.composite[0]);i++) {
        BDBG_OBJECT_INIT(&state->outputs.composite[i], NEXUS_CompositeOutput);
        state->outputs.composite[i].cfg = NEXUS_CompositeOutputDefaultSettings;
        state->outputs.composite[i].opened = false;
        state->outputs.composite[i].dacOutputType = BVDC_DacOutput_eComposite;
        NEXUS_VIDEO_OUTPUT_INIT(&state->outputs.composite[i].output, NEXUS_VideoOutputType_eComposite, &state->outputs.composite[i]);
    }
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS
    for(i=0;i<sizeof(state->outputs.svideo)/sizeof(state->outputs.svideo[0]);i++) {
        BDBG_OBJECT_INIT(&state->outputs.svideo[i], NEXUS_SvideoOutput);
        state->outputs.svideo[i].cfg = NEXUS_SvideoOutputDefaultSettings;
        state->outputs.svideo[i].opened = false;
        NEXUS_VIDEO_OUTPUT_INIT(&state->outputs.svideo[i].output, NEXUS_VideoOutputType_eSvideo, &state->outputs.svideo[i]);
    }
#endif
#if NEXUS_NUM_656_OUTPUTS
    for(i=0;i<sizeof(state->outputs.ccir656)/sizeof(state->outputs.ccir656[0]);i++) {
        BDBG_OBJECT_INIT(&state->outputs.ccir656[i], NEXUS_Ccir656Output);
        state->outputs.ccir656[i].cfg = NEXUS_Ccir656OutputDefaultSettings;
        state->outputs.ccir656[i].opened = false;
        NEXUS_VIDEO_OUTPUT_INIT(&state->outputs.ccir656[i].output, NEXUS_VideoOutputType_eCcir656, &state->outputs.ccir656[i]);
    }
#endif
    return;
}
#if NEXUS_NUM_HDMI_DVO

#if NEXUS_NUM_HDMI_DVO > 1
#error Currently, only one HDMI output is supported
#endif

#include "nexus_hdmi_dvo.h"
#include "priv/nexus_hdmi_dvo_priv.h"

static void NEXUS_VideoOutput_P_HdmiDvoRateChange_isr(NEXUS_DisplayHandle display, void *pParam)
{
    NEXUS_HdmiDvoHandle hdmiDvo = pParam;
    BDBG_ASSERT(NULL != hdmiDvo);
    BDBG_MSG(("> NEXUS_VideoOutput_P_HdmiDvoRateChange_isr"));
    NEXUS_HdmiDvo_VideoRateChange_isr(hdmiDvo, &display->hdmiDvo.rateInfo);
    BDBG_MSG(("< NEXUS_VideoOutput_P_HdmiDvoRateChange_isr"));
    return;
}

static BERR_Code NEXUS_VideoOutput_P_SetHdmiDvoFormat(void *output, NEXUS_DisplayHandle display, NEXUS_VideoFormat format, NEXUS_DisplayAspectRatio aspectRatio)
{
    BERR_Code rc;
    NEXUS_HdmiDvoHandle hdmiDvo = output;
    BFMT_VideoFmt videoFmt;
#if NEXUS_HAS_HDMI_1_3
    NEXUS_HdmiOutputSettings settings;
    BAVC_HDMI_BitsPerPixel colorDepth;
#endif
    BDBG_MSG(("> NEXUS_VideoOutput_P_SetHdmiDvoFormat"));
    BDBG_ASSERT(NULL != hdmiDvo); 

    rc = NEXUS_P_VideoFormat_ToMagnum(format, &videoFmt);
    if (rc) return BERR_TRACE(rc);

    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiDvo);
    NEXUS_HdmiDvo_GetColorimetry_priv(hdmiDvo,&g_NEXUS_DisplayModule_State.hdmiDvo.colorimetry) ;
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiDvo);

    /* Enable HDMI port in the VDC */
    rc = BVDC_Display_SetHdmiConfiguration(display->displayVdc, BVDC_Hdmi_0, g_NEXUS_DisplayModule_State.hdmiDvo.colorimetry);
    if (rc) return BERR_TRACE(rc);

    /* Additional step to configure deep color mode */
#if NEXUS_HAS_HDMI_1_3
    /* Get color depth settings */
    NEXUS_HdmiDvo_GetSettings(hdmiDvo, &settings);
    colorDepth = NEXUS_P_HdmiDvoColorDepth_ToMagnum(settings.colorDepth);
    rc = BVDC_Display_SetHdmiColorDepth(display->displayVdc, colorDepth);
    if (rc) return BERR_TRACE(rc);
#endif

    NEXUS_HdmiDvo_SetDisplayParams_priv(hdmiDvo,videoFmt,g_NEXUS_DisplayModule_State.hdmiDvo.colorimetry,aspectRatio);
    BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    g_NEXUS_DisplayModule_State.hdmiDvo.display = display;
    g_NEXUS_DisplayModule_State.hdmiDvo.hdmiDvo = hdmiDvo;
    g_NEXUS_DisplayModule_State.hdmiDvo.aspectRatio = aspectRatio;
    g_NEXUS_DisplayModule_State.hdmiDvo.format = format;
    BDBG_MSG(("< NEXUS_VideoOutput_P_SetHdmiDvoFormat"));
    return 0;
}

static NEXUS_Error NEXUS_VideoOutput_P_HdmiDvoFormatChange(void *output, NEXUS_DisplayHandle display, NEXUS_VideoFormat format, NEXUS_DisplayAspectRatio aspectRatio)
{
    NEXUS_Error errCode= NEXUS_SUCCESS;
    BDBG_MSG(("> NEXUS_VideoOutput_P_HdmiDvoFormatChange"));
    (void)NEXUS_VideoOutput_P_SetHdmiDvoFormat(output, display, format, aspectRatio);
    BDBG_MSG(("< NEXUS_VideoOutput_P_HdmiDvoFormatChange"));
    return errCode;
}


static NEXUS_Error NEXUS_HdmiDvo_P_Disconnect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_HdmiDvoHandle hdmiDvo=output;
    NEXUS_Error rc=NEXUS_SUCCESS;
    BDBG_MSG(("> NEXUS_HdmiDvo_P_Disconnect"));
    BDBG_ASSERT(NULL != hdmiDvo);
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BVDC_Display_SetHdmiConfiguration(display->displayVdc, BVDC_Hdmi_0,BAVC_MatrixCoefficients_eUnknown);
    BDBG_MSG(("< NEXUS_HdmiDvo_P_Disconnect"));
    return rc;
}

static NEXUS_Error NEXUS_HdmiDvo_P_Connect(void *output,  NEXUS_DisplayHandle display)
{
    NEXUS_Error rc=NEXUS_SUCCESS;
    NEXUS_HdmiDvoHandle hdmiDvo=output;
    BDBG_MSG(("> NEXUS_HdmiDvo_P_Connect"));
    BDBG_ASSERT(NULL != hdmiDvo);
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    (void)NEXUS_VideoOutput_P_SetHdmiDvoFormat(output, display, display->cfg.format, display->cfg.aspectRatio);

    BKNI_EnterCriticalSection();
    BDBG_ASSERT(NULL == display->hdmiDvo.rateChangeCb_isr);
    display->hdmiDvo.rateChangeCb_isr = NEXUS_VideoOutput_P_HdmiDvoRateChange_isr;
    display->hdmiDvo.pCbParam = hdmiDvo;
    if ( display->hdmiDvo.rateInfoValid )
    {
        NEXUS_VideoOutput_P_HdmiDvoRateChange_isr(display, hdmiDvo);
    }
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("< NEXUS_HdmiDvo_P_Connect"));
    return rc;
}

static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenHdmiDvo(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;
    BDBG_MSG(("> NEXUS_VideoOutput_P_OpenHdmiDvo"));
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    BDBG_ASSERT(output->type == NEXUS_VideoOutputType_eHdmiDvo);
    iface.connect = NEXUS_HdmiDvo_P_Connect;
    iface.disconnect = NEXUS_HdmiDvo_P_Disconnect;
    iface.formatChange = NEXUS_VideoOutput_P_HdmiDvoFormatChange;
    BDBG_MSG(("< NEXUS_VideoOutput_P_OpenHdmiDvo"));
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, false);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_eDvo;
    }
    return link;
}

#endif

void
NEXUS_ComponentOutput_GetDefaultSettings(NEXUS_ComponentOutputSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->type = NEXUS_ComponentOutputType_eYPrPb;
}

NEXUS_ComponentOutputHandle
NEXUS_ComponentOutput_Open(unsigned index, const NEXUS_ComponentOutputSettings *cfg)
{
    BERR_Code rc;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_ComponentOutputHandle output;
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    NEXUS_ComponentOutputSettings defaultSettings;

    BSTD_UNUSED(rc);

    if(!cfg) {
        NEXUS_ComponentOutput_GetDefaultSettings(&defaultSettings);
        cfg = &defaultSettings;
    }
    if(index>=sizeof(state->outputs.component)/sizeof(state->outputs.component[0])) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output = &state->outputs.component[index];
    if(output->opened) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output->opened = true;
    output->index = index;
    output->cfg = *cfg;
    NEXUS_OBJECT_SET(NEXUS_ComponentOutput, output);
    return output;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(cfg);
    rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    return NULL;
#endif
}

static void
NEXUS_ComponentOutput_P_Finalizer(NEXUS_ComponentOutputHandle output)
{
    NEXUS_DisplayHandle display;
    NEXUS_OBJECT_ASSERT(NEXUS_ComponentOutput, output);

    /* auto-remove from any Display */
    display = NEXUS_VideoOutput_P_GetDisplay(&output->output);
    if (display) {
        NEXUS_Display_RemoveOutput(display, &output->output);
    }

    BDBG_OBJECT_UNSET(output, NEXUS_ComponentOutput);
    output->opened = false;
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_ComponentOutput, NEXUS_ComponentOutput_Close);

NEXUS_VideoOutput
NEXUS_ComponentOutput_GetConnector(NEXUS_ComponentOutputHandle output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_ComponentOutput);
    return &output->output;
}

void
NEXUS_ComponentOutput_GetSettings(NEXUS_ComponentOutputHandle output,  NEXUS_ComponentOutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_ComponentOutput);
    BDBG_ASSERT(pSettings);
    *pSettings = output->cfg;
    return;
}

NEXUS_Error
NEXUS_ComponentOutput_SetSettings(NEXUS_ComponentOutputHandle output, const NEXUS_ComponentOutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_ComponentOutput);
    output->cfg = *pSettings;
    if (output->output.destination) {
        NEXUS_Error rc;
        NEXUS_VideoOutput_P_Link *link = output->output.destination;
        BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
        rc = NEXUS_ComponentOutput_P_Connect(output, link->display);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return BERR_SUCCESS;
}

void
NEXUS_SvideoOutput_GetDefaultSettings(NEXUS_SvideoOutputSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    *pSettings = NEXUS_SvideoOutputDefaultSettings;
    return;
}



NEXUS_SvideoOutputHandle
NEXUS_SvideoOutput_Open(unsigned index, const NEXUS_SvideoOutputSettings *cfg)
{
    BERR_Code rc;
#if NEXUS_NUM_SVIDEO_OUTPUTS
    NEXUS_SvideoOutputHandle  output;
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    BSTD_UNUSED(rc);

    if(!cfg) {
        cfg  = &NEXUS_SvideoOutputDefaultSettings;
    }
    if(index>=sizeof(state->outputs.svideo)/sizeof(state->outputs.svideo[0])) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output = &state->outputs.svideo[index];
    if(output->opened) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output->opened = true;
    output->cfg = *cfg;
    NEXUS_OBJECT_SET(NEXUS_SvideoOutput, output);
    return output;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(cfg);
    rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    return NULL;
#endif
}

static void
NEXUS_SvideoOutput_P_Finalizer(NEXUS_SvideoOutputHandle output)
{
    NEXUS_DisplayHandle display;
    NEXUS_OBJECT_ASSERT(NEXUS_SvideoOutput, output);

    /* auto-remove from any Display */
    display = NEXUS_VideoOutput_P_GetDisplay(&output->output);
    if (display) {
        NEXUS_Display_RemoveOutput(display, &output->output);
    }

    BDBG_OBJECT_UNSET(output, NEXUS_SvideoOutput);
    output->opened = false;
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_SvideoOutput, NEXUS_SvideoOutput_Close);

NEXUS_VideoOutput
NEXUS_SvideoOutput_GetConnector(NEXUS_SvideoOutputHandle output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_SvideoOutput);
    return &output->output;
}

void
NEXUS_SvideoOutput_GetSettings(NEXUS_SvideoOutputHandle output,  NEXUS_SvideoOutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_SvideoOutput);
    BDBG_ASSERT(pSettings);
    *pSettings = output->cfg;
    return;
}

NEXUS_Error
NEXUS_SvideoOutput_SetSettings(NEXUS_SvideoOutputHandle output, const NEXUS_SvideoOutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_SvideoOutput);
    output->cfg = *pSettings;
    if (output->output.destination) {
        NEXUS_Error rc;
        NEXUS_VideoOutput_P_Link *link = output->output.destination;
        BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
        rc = NEXUS_SvideoOutput_P_Connect(output, link->display);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return BERR_SUCCESS;
}

void
NEXUS_CompositeOutput_GetDefaultSettings(NEXUS_CompositeOutputSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    *pSettings = NEXUS_CompositeOutputDefaultSettings;
    return;
}


NEXUS_CompositeOutputHandle
NEXUS_CompositeOutput_Open(unsigned index, const NEXUS_CompositeOutputSettings *cfg)
{
    BERR_Code rc;
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_CompositeOutputHandle  output;
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    BSTD_UNUSED(rc);

    if(!cfg) {
        cfg  = &NEXUS_CompositeOutputDefaultSettings;
    }
    if(index>=sizeof(state->outputs.composite)/sizeof(state->outputs.composite[0])) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output = &state->outputs.composite[index];
    if(output->opened) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output->opened = true;
    output->cfg = *cfg;
    NEXUS_OBJECT_SET(NEXUS_CompositeOutput, output);
    return output;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(cfg);
    rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    return NULL;
#endif
}

static void
NEXUS_CompositeOutput_P_Finalizer(NEXUS_CompositeOutputHandle output)
{
    NEXUS_DisplayHandle display;
    NEXUS_OBJECT_ASSERT(NEXUS_CompositeOutput, output);

    /* auto-remove from any Display */
    display = NEXUS_VideoOutput_P_GetDisplay(&output->output);
    if (display) {
        NEXUS_Display_RemoveOutput(display, &output->output);
    }

    BDBG_OBJECT_UNSET(output, NEXUS_CompositeOutput);
    output->opened = false;
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_CompositeOutput, NEXUS_CompositeOutput_Close);

NEXUS_VideoOutput
NEXUS_CompositeOutput_GetConnector(NEXUS_CompositeOutputHandle output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_CompositeOutput);
    return &output->output;
}

void
NEXUS_CompositeOutput_GetSettings(NEXUS_CompositeOutputHandle output,  NEXUS_CompositeOutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_CompositeOutput);
    BDBG_ASSERT(pSettings);
    *pSettings = output->cfg;
    return;
}

NEXUS_Error
NEXUS_CompositeOutput_SetSettings(NEXUS_CompositeOutputHandle output, const NEXUS_CompositeOutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_CompositeOutput);
    output->cfg = *pSettings;
    if (output->output.destination) {
        NEXUS_Error rc;
        NEXUS_VideoOutput_P_Link *link = output->output.destination;
        BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
        rc = NEXUS_CompositeOutput_P_Connect(output, link->display);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return BERR_SUCCESS;
}

void
NEXUS_Ccir656Output_GetDefaultSettings(NEXUS_Ccir656OutputSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    *pSettings = NEXUS_Ccir656OutputDefaultSettings;
    return;
}


NEXUS_Ccir656OutputHandle
NEXUS_Ccir656Output_Open(unsigned index, const NEXUS_Ccir656OutputSettings *cfg)
{
    BERR_Code rc;
#if NEXUS_NUM_656_OUTPUTS
    NEXUS_Ccir656OutputHandle  output;
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    BSTD_UNUSED(rc);

    if(!cfg) {
        cfg  = &NEXUS_Ccir656OutputDefaultSettings;
    }
    if(index>=sizeof(state->outputs.ccir656)/sizeof(state->outputs.ccir656[0])) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output = &state->outputs.ccir656[index];
    if(output->opened) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    output->opened = true;
    output->cfg = *cfg;
    NEXUS_OBJECT_SET(NEXUS_Ccir656Output, output);
    return output;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(cfg);
    rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    return NULL;
#endif
}

static void
NEXUS_Ccir656Output_P_Finalizer(NEXUS_Ccir656OutputHandle output)
{
    NEXUS_DisplayHandle display;
    NEXUS_OBJECT_ASSERT(NEXUS_Ccir656Output, output);

    /* auto-remove from any Display */
    display = NEXUS_VideoOutput_P_GetDisplay(&output->output);
    if (display) {
        NEXUS_Display_RemoveOutput(display, &output->output);
    }

    BDBG_OBJECT_UNSET(output, NEXUS_Ccir656Output);
    output->opened = false;
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Ccir656Output, NEXUS_Ccir656Output_Close);

NEXUS_VideoOutput
NEXUS_Ccir656Output_GetConnector(NEXUS_Ccir656OutputHandle output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_Ccir656Output);
    return &output->output;
}

void
NEXUS_Ccir656Output_GetSettings(NEXUS_Ccir656OutputHandle output,  NEXUS_Ccir656OutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_Ccir656Output);
    BDBG_ASSERT(pSettings);
    *pSettings = output->cfg;
    return;
}

NEXUS_Error
NEXUS_Ccir656Output_SetSettings(NEXUS_Ccir656OutputHandle output, const NEXUS_Ccir656OutputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_Ccir656Output);
    output->cfg = *pSettings;
    if (output->output.destination) {
        NEXUS_Error rc;
        NEXUS_VideoOutput_P_Link *link = output->output.destination;
        BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
        rc = NEXUS_Ccir656Output_P_Connect(output, link->display);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return BERR_SUCCESS;
}

BDBG_OBJECT_ID(NEXUS_VideoOutput_P_Link);

BERR_Code
NEXUS_VideoOutput_P_SetDac(NEXUS_VideoOutput_P_Link *link, BVDC_Display_Handle display, NEXUS_VideoDac dac, BVDC_DacOutput type)
{
    uint32_t dacVdc;
    BERR_Code rc;
    switch(dac) {
    default:
    case NEXUS_VideoDac_eNone:
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    case NEXUS_VideoDac_e0:
        dacVdc = BVDC_Dac_0;
        break;
    case NEXUS_VideoDac_e1:
        dacVdc = BVDC_Dac_1;
        break;
    case NEXUS_VideoDac_e2:
        dacVdc = BVDC_Dac_2;
        break;
    case NEXUS_VideoDac_e3:
        dacVdc = BVDC_Dac_3;
        break;
    case NEXUS_VideoDac_e4:
        dacVdc = BVDC_Dac_4;
        break;
    case NEXUS_VideoDac_e5:
        dacVdc = BVDC_Dac_5;
        break;
    case NEXUS_VideoDac_e6:
        dacVdc = BVDC_Dac_6;
        break;
    }
    
    /* store DAC assignment for reverse lookup */
    link->channel[dac - NEXUS_VideoDac_e0] = type;
    
    rc = BVDC_Display_SetDacConfiguration(display, dacVdc, type);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); }
    return rc;
}

static BERR_Code
NEXUS_CompositeOutput_P_Connect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_CompositeOutputHandle composite=output;
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(composite, NEXUS_CompositeOutput);
    link = composite->output.destination;

    rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, composite->cfg.dac, composite->dacOutputType);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); }
    return rc;
}

static BERR_Code
NEXUS_CompositeOutput_P_Disconnect(void *output, NEXUS_DisplayHandle display)
{
    BERR_Code rc;
    NEXUS_CompositeOutputHandle composite = output;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(composite, NEXUS_CompositeOutput);
    link = composite->output.destination;
    
    rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, composite->cfg.dac, BVDC_DacOutput_eUnused);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); }
    return rc;
}

static BERR_Code
NEXUS_SvideoOutput_P_Connect(void *output,  NEXUS_DisplayHandle display)
{
    BERR_Code rc;
    NEXUS_SvideoOutputHandle svideo=output;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(svideo, NEXUS_SvideoOutput);
    link = svideo->output.destination;
    
    rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, svideo->cfg.dacY, BVDC_DacOutput_eSVideo_Luma);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
    rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, svideo->cfg.dacC, BVDC_DacOutput_eSVideo_Chroma);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
err_dac:
    return rc;
}

static BERR_Code
NEXUS_SvideoOutput_P_Disconnect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_SvideoOutputHandle svideo=output;
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(svideo, NEXUS_SvideoOutput);
    link = svideo->output.destination;

    rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, svideo->cfg.dacY, BVDC_DacOutput_eUnused);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
    rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, svideo->cfg.dacC, BVDC_DacOutput_eUnused);
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
err_dac:
    return rc;
}

static BERR_Code
NEXUS_ComponentOutput_P_Connect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_ComponentOutputHandle component=output;
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(component, NEXUS_ComponentOutput);
    link = component->output.destination;

    switch(component->cfg.type) {
    case NEXUS_ComponentOutputType_eYPrPb:
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.YPrPb.dacY, BVDC_DacOutput_eY);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.YPrPb.dacPr, BVDC_DacOutput_ePr);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.YPrPb.dacPb, BVDC_DacOutput_ePb);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        break;
    case NEXUS_ComponentOutputType_eRGB:
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacGreen,
            (component->cfg.dacs.RGB.dacHSync != NEXUS_VideoDac_eNone || component->cfg.dacs.RGB.noSync) ? BVDC_DacOutput_eGreen_NoSync : BVDC_DacOutput_eGreen);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacRed, BVDC_DacOutput_eRed);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacBlue, BVDC_DacOutput_eBlue);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        if (component->cfg.dacs.RGB.dacHSync != NEXUS_VideoDac_eNone && !component->cfg.dacs.RGB.noSync) {
            rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacHSync, BVDC_DacOutput_eHsync);
            if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        }
        break;
    default:
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_dac;
    }
    rc = BVDC_Display_SetMpaaDecimation(display->displayVdc, BVDC_MpaaDeciIf_eComponent, 1 << component->index, component->cfg.mpaaDecimationEnabled);
    if (rc) return BERR_TRACE(rc);

err_dac:
    return rc;
}


static BERR_Code
NEXUS_ComponentOutput_P_Disconnect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_ComponentOutputHandle component=output;
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(component, NEXUS_ComponentOutput);
    link = component->output.destination;

    if (component->cfg.mpaaDecimationEnabled) {
        rc = BVDC_Display_SetMpaaDecimation(display->displayVdc, BVDC_MpaaDeciIf_eComponent, 1 << component->index, false);
        if (rc) {
            rc = BERR_TRACE(rc); /* fall through */
        }
        else {
            /* two ApplyChanges are required: this one and the standard one */
            rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
            if (rc) {rc = BERR_TRACE(rc);} /* fall through */
        }   
    }
    
    switch(component->cfg.type) {
    case NEXUS_ComponentOutputType_eYPrPb:
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.YPrPb.dacY, BVDC_DacOutput_eUnused);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.YPrPb.dacPr, BVDC_DacOutput_eUnused);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.YPrPb.dacPb, BVDC_DacOutput_eUnused);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        break;
    case NEXUS_ComponentOutputType_eRGB:
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacGreen, BVDC_DacOutput_eUnused);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacRed, BVDC_DacOutput_eUnused);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacBlue, BVDC_DacOutput_eUnused);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        if (component->cfg.dacs.RGB.dacHSync != NEXUS_VideoDac_eNone) {
            rc = NEXUS_VideoOutput_P_SetDac(link, display->displayVdc, component->cfg.dacs.RGB.dacHSync, BVDC_DacOutput_eUnused);
            if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto err_dac; }
        }
        break;
    default:
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_dac;
    }

err_dac:
    return rc;
}

static BERR_Code
NEXUS_Ccir656Output_P_Connect(void *output,  NEXUS_DisplayHandle display)
{
    NEXUS_Ccir656OutputHandle ccir656=output;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(ccir656, NEXUS_Ccir656Output);

    rc = BVDC_Display_Set656Configuration(display->displayVdc, BVDC_Itur656Output_0, true); /* enable 656 out */
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); }

    return rc;
}

#if NEXUS_NUM_656_OUTPUTS
static BERR_Code
NEXUS_Ccir656Output_P_Disconnect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_Ccir656OutputHandle ccir656=output;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(ccir656, NEXUS_Ccir656Output);

    rc = BVDC_Display_Set656Configuration(display->displayVdc, BVDC_Itur656Output_0, false); /* disable 656 out */
    if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); }

    return rc;
}
#endif

NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_CreateLink(NEXUS_VideoOutput output, const NEXUS_VideoOutput_P_Iface *iface, bool sdOnly)
{
    NEXUS_VideoOutput_P_Link *link;
    unsigned i;

    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    BDBG_ASSERT(iface);

    link = BKNI_Malloc(sizeof(*link));
    if(!link) {
        BERR_Code rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BSTD_UNUSED(rc);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(link, NEXUS_VideoOutput_P_Link);
    link->iface = *iface;
    link->output = output;
    link->display = NULL;
    link->sdOnly = sdOnly;
    link->displayOutput = 0; /* no unknown value */
    for (i=0;i<NEXUS_VideoDac_eMax;i++) {
        link->channel[i] = BVDC_DacOutput_eUnused;
    }
    output->destination = link;
    return link;
err_alloc:
    return NULL;
}

/* this function is called from NEXUS_Display_Close and removes the link from the list */
void
NEXUS_VideoOutput_P_DestroyLink(NEXUS_VideoOutput_P_Link *link)
{
    BDBG_OBJECT_DESTROY(link, NEXUS_VideoOutput_P_Link);
    BKNI_Free(link);
}

static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenComposite(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;
    NEXUS_CompositeOutputHandle compositeOutput = (NEXUS_CompositeOutputHandle)output->source;

    BDBG_OBJECT_ASSERT(compositeOutput, NEXUS_CompositeOutput);
    iface.connect = NEXUS_CompositeOutput_P_Connect;
    iface.disconnect = NEXUS_CompositeOutput_P_Disconnect;
    iface.formatChange = NULL;
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, true);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_eComposite;
    }
    return link;
}

static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenSvideo(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;

    BDBG_OBJECT_ASSERT((NEXUS_SvideoOutputHandle)output->source, NEXUS_SvideoOutput);
    iface.connect = NEXUS_SvideoOutput_P_Connect;
    iface.disconnect = NEXUS_SvideoOutput_P_Disconnect;
    iface.formatChange = NULL;
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, true);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_eSVideo;
    }
    return link;
}

static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenComponent(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;

    BDBG_OBJECT_ASSERT((NEXUS_ComponentOutputHandle)output->source, NEXUS_ComponentOutput);
    iface.connect = NEXUS_ComponentOutput_P_Connect;
    iface.disconnect = NEXUS_ComponentOutput_P_Disconnect;
    iface.formatChange = NULL;
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, false);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_eComponent;
    }
    return link;
}

#if NEXUS_NUM_656_OUTPUTS
static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenCcir656(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;
    BDBG_OBJECT_ASSERT((NEXUS_Ccir656OutputHandle)output->source, NEXUS_Ccir656Output);
    iface.connect = NEXUS_Ccir656Output_P_Connect;
    iface.disconnect = NEXUS_Ccir656Output_P_Disconnect;
    iface.formatChange = NULL;
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, false);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_e656;
    }
    return link;
}
#endif

#if NEXUS_NUM_HDMI_OUTPUTS

#if NEXUS_NUM_HDMI_OUTPUTS > 1
#error Currently, only one HDMI output is supported
#endif

#include "priv/nexus_hdmi_output_priv.h"

static void
NEXUS_VideoOutput_P_HdmiRateChange_isr(NEXUS_DisplayHandle display, void *pParam)
{
    NEXUS_HdmiOutputHandle hdmi = pParam;
    BDBG_ASSERT(NULL != hdmi);
    NEXUS_HdmiOutput_VideoRateChange_isr(hdmi, &display->hdmi.rateInfo);
}

void NEXUS_VideoOutput_P_SetHdmiSettings(void *context)
{
    BERR_Code rc;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)context;
    NEXUS_HdmiOutputHandle hdmiOutput = display->hdmi.outputNotify;
    NEXUS_HdmiOutputSettings settings;
    BFMT_VideoFmt videoFmt;
    BFMT_AspectRatio aspectRatioVdc;
#if NEXUS_HAS_HDMI_1_3
    BAVC_HDMI_BitsPerPixel colorDepth;
    NEXUS_Error errCode;
#endif
    BVDC_Display_DvoSettings dvoSettings;

    NEXUS_HdmiOutput_GetSettings(hdmiOutput, &settings);

    rc = NEXUS_P_DisplayAspectRatio_ToMagnum(g_NEXUS_DisplayModule_State.hdmiOutput.aspectRatio, g_NEXUS_DisplayModule_State.hdmiOutput.format, &aspectRatioVdc);
    if (rc) {
        aspectRatioVdc = BFMT_AspectRatio_eUnknown; /* don't proceed with uninitialized value. */
    }
    rc = NEXUS_P_VideoFormat_ToMagnum(display->cfg.format, &videoFmt);
    if (rc) {
        videoFmt = BFMT_VideoFmt_eNTSC; /* don't proceed with uninitialized value. */
    }
    
    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    /* NEXUS_HdmiOutput_GetColorimetry_priv returns bool for connected */
    rc = !NEXUS_HdmiOutput_GetColorimetry_priv(hdmiOutput, videoFmt, &g_NEXUS_DisplayModule_State.hdmiOutput.colorimetry);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    if (!rc) {
        rc = BVDC_Display_SetHdmiConfiguration(display->displayVdc, BVDC_Hdmi_0, g_NEXUS_DisplayModule_State.hdmiOutput.colorimetry);
        if (rc) {rc = BERR_TRACE(rc); goto done;}
    }

    rc = BVDC_Display_SetHdmiSyncOnly(display->displayVdc, settings.syncOnly);
    if (rc) {rc = BERR_TRACE(rc); goto done;}

    rc = BVDC_Display_SetMpaaDecimation(display->displayVdc, BVDC_MpaaDeciIf_eHdmi, 1, settings.mpaaDecimationEnabled);
    if (rc) {rc = BERR_TRACE(rc); goto done;}

    BVDC_Display_GetDvoConfiguration(display->displayVdc, &dvoSettings);
    dvoSettings.stSpreadSpectrum.bEnable = settings.spreadSpectrum.enable;
    dvoSettings.stSpreadSpectrum.ulFrequency  = settings.spreadSpectrum.frequency ;
    dvoSettings.stSpreadSpectrum.ulDelta = settings.spreadSpectrum.delta ;
    rc = BVDC_Display_SetDvoConfiguration(display->displayVdc, &dvoSettings);
    if (rc) {rc = BERR_TRACE(rc); goto done;}

    /* set deep color mode in HDMI and VDC*/
#if NEXUS_HAS_HDMI_1_3
    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    errCode = NEXUS_HdmiOutput_SetColorDepth_priv(hdmiOutput, settings.colorDepth);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    if (errCode) {errCode = BERR_TRACE(errCode); goto done;}

    colorDepth = NEXUS_P_HdmiColorDepth_ToMagnum(settings.colorDepth);
    rc = BVDC_Display_SetHdmiColorDepth(display->displayVdc, colorDepth);
    if (rc) {rc = BERR_TRACE(rc); goto done;}
#endif

    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) {rc = BERR_TRACE(rc); goto done;}

    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    (void)NEXUS_HdmiOutput_SetDisplayParams_priv(hdmiOutput, videoFmt, g_NEXUS_DisplayModule_State.hdmiOutput.colorimetry, aspectRatioVdc, 
        display->cfg.timingGenerator == NEXUS_DisplayTimingGenerator_eHdmiDvo,
        NULL /* no change */);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);

done:
    return;
}

static BERR_Code
NEXUS_VideoOutput_P_SetHdmiFormat(void *output, NEXUS_DisplayHandle display, NEXUS_VideoFormat format, NEXUS_DisplayAspectRatio aspectRatio)
{
    BERR_Code rc;
    NEXUS_HdmiOutputHandle hdmiOutput = output;
    BFMT_VideoFmt videoFmt;
    NEXUS_HdmiOutputStatus *hdmiOutputStatus;
    bool aspectRatioChangeOnly;
#if NEXUS_HAS_HDMI_1_3
    NEXUS_HdmiOutputSettings settings;
    BAVC_HDMI_BitsPerPixel colorDepth;
#endif

    BDBG_ASSERT(NULL != hdmiOutput); /* HdmiOutput will validate the handle */
    g_NEXUS_DisplayModule_State.functionData.NEXUS_VideoOutput_P_SetHdmiFormat.cookie = NEXUS_VideoOutput_P_SetHdmiFormat;
    hdmiOutputStatus = &g_NEXUS_DisplayModule_State.functionData.NEXUS_VideoOutput_P_SetHdmiFormat.hdmiOutputStatus;

    rc = NEXUS_HdmiOutput_GetStatus(hdmiOutput, hdmiOutputStatus);
    if (rc) return BERR_TRACE(rc);
    if (!hdmiOutputStatus->connected) {
        NEXUS_CallbackDesc notifyDisplay;
        BDBG_WRN(("HDMI output is not connected. Cannot apply format settings."));

        /* set the callback so that when we do connect, the display is notified */
        display->hdmi.outputNotify = hdmiOutput;
        NEXUS_CallbackHandler_PrepareCallback(display->hdmi.outputNotifyDisplay, notifyDisplay);
        
        NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
        NEXUS_HdmiOutput_SetDisconnectParams_priv(hdmiOutput, display->cfg.timingGenerator == NEXUS_DisplayTimingGenerator_eHdmiDvo, &notifyDisplay);
        NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);

        return -1; /* No need for BERR_TRACE. This is normal. */
    }

    rc = NEXUS_P_VideoFormat_ToMagnum(format, &videoFmt);
    if (rc) return BERR_TRACE(rc);

    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);

    /* If only aspect ratio is changing, set the aspectRatioChangeOnly flag for later use in hdmi_output module*/
    aspectRatioChangeOnly = ((g_NEXUS_DisplayModule_State.hdmiOutput.aspectRatio != aspectRatio) &&
        (g_NEXUS_DisplayModule_State.hdmiOutput.format == format));
	
    rc = NEXUS_HdmiOutput_P_PreFormatChange_priv(hdmiOutput, aspectRatioChangeOnly);
    if (rc) {
        rc = BERR_TRACE(rc);
        /* Keep going, they asked for it... */
    }

    if ( !NEXUS_HdmiOutput_GetColorimetry_priv(hdmiOutput, videoFmt, &g_NEXUS_DisplayModule_State.hdmiOutput.colorimetry) )
    {
        BDBG_WRN(("Video format %d does not seem to be supported by attached HDMI receiver.", format));
        /* Keep going, they asked for it... */
    }
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);

    /* Enable HDMI port in the VDC */
    rc = BVDC_Display_SetHdmiConfiguration(display->displayVdc, BVDC_Hdmi_0, g_NEXUS_DisplayModule_State.hdmiOutput.colorimetry);
    if (rc) return BERR_TRACE(rc);

    /* Additional step to configure deep color mode */
#if NEXUS_HAS_HDMI_1_3
    /* Get color depth settings */
    NEXUS_HdmiOutput_GetSettings(hdmiOutput, &settings);
    colorDepth = NEXUS_P_HdmiColorDepth_ToMagnum(settings.colorDepth);
    rc = BVDC_Display_SetHdmiColorDepth(display->displayVdc, colorDepth);
    if (rc) return BERR_TRACE(rc);
#endif

    /* don't call ApplyChanges here. this will be called by higher level Display code. after the ApplyChanges is done,
    the requiresHdmiOutputBringup flag will cause NEXUS_VideoOutput_P_PostSetHdmiFormat to be called. */

    /* store information that we will need in NEXUS_VideoOutput_P_PostSetHdmiFormat */
    g_NEXUS_DisplayModule_State.hdmiOutput.display = display;
    g_NEXUS_DisplayModule_State.hdmiOutput.hdmiOutput = hdmiOutput;
    g_NEXUS_DisplayModule_State.hdmiOutput.aspectRatio = aspectRatio;
    g_NEXUS_DisplayModule_State.hdmiOutput.format = format;

    BDBG_ASSERT(g_NEXUS_DisplayModule_State.functionData.NEXUS_VideoOutput_P_SetHdmiFormat.cookie == NEXUS_VideoOutput_P_SetHdmiFormat);/* verify that cookie still in place, e.g. no other function were using the same union */

    return BERR_SUCCESS;
}

void NEXUS_VideoOutput_P_PostSetHdmiFormat(void)
{
    BERR_Code rc;
    BFMT_AspectRatio aspectRatioVdc;
    BFMT_VideoFmt videoFmt;
    NEXUS_CallbackDesc notifyDisplay;
    NEXUS_DisplayHandle display = g_NEXUS_DisplayModule_State.hdmiOutput.display;
    NEXUS_HdmiOutputHandle hdmiOutput = g_NEXUS_DisplayModule_State.hdmiOutput.hdmiOutput;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    g_NEXUS_DisplayModule_State.hdmiOutput.display = NULL;
    g_NEXUS_DisplayModule_State.hdmiOutput.hdmiOutput = NULL;

    rc = NEXUS_P_DisplayAspectRatio_ToMagnum(g_NEXUS_DisplayModule_State.hdmiOutput.aspectRatio, g_NEXUS_DisplayModule_State.hdmiOutput.format, &aspectRatioVdc);
    if (rc) {
        aspectRatioVdc = BFMT_AspectRatio_eUnknown; /* don't proceed with uninitialized value. */
    }
    rc = NEXUS_P_VideoFormat_ToMagnum(g_NEXUS_DisplayModule_State.hdmiOutput.format, &videoFmt);
    if (rc) {
        videoFmt = BFMT_VideoFmt_eNTSC; /* don't proceed with uninitialized value. */
    }

    /* now enable/set the HDMI changes */
    display->hdmi.outputNotify = hdmiOutput;
    NEXUS_CallbackHandler_PrepareCallback(display->hdmi.outputNotifyDisplay, notifyDisplay);

#if NEXUS_HAS_HDMI_1_3
    {
    NEXUS_HdmiOutputSettings settings;
    (void) NEXUS_HdmiOutput_GetSettings(hdmiOutput, &settings);
    /* Set deep color mode */
    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    (void) NEXUS_HdmiOutput_SetColorDepth_priv(hdmiOutput, settings.colorDepth);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    }
#endif

    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
    (void)NEXUS_HdmiOutput_SetDisplayParams_priv(hdmiOutput, videoFmt, g_NEXUS_DisplayModule_State.hdmiOutput.colorimetry, aspectRatioVdc, 
        display->cfg.timingGenerator == NEXUS_DisplayTimingGenerator_eHdmiDvo,
        &notifyDisplay);
    (void)NEXUS_HdmiOutput_P_PostFormatChange_priv(hdmiOutput);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.hdmiOutput);
}

static BERR_Code
NEXUS_VideoOutput_P_HdmiFormatChange(void *output, NEXUS_DisplayHandle display, NEXUS_VideoFormat format, NEXUS_DisplayAspectRatio aspectRatio)
{
    (void)NEXUS_VideoOutput_P_SetHdmiFormat(output, display, format, aspectRatio);
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_VideoOutput_P_ConnectHdmi(void *output,  NEXUS_DisplayHandle display)
{
    NEXUS_HdmiOutputHandle hdmi = output;

    BDBG_ASSERT(NULL != hdmi);
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
	
	#if defined(HUMAX_PLATFORM_BASE)
	/* reset varialbles to avod setting aspect only flag after HPD*/
	g_NEXUS_DisplayModule_State.hdmiOutput.aspectRatio = NEXUS_DisplayAspectRatio_eMax;
	g_NEXUS_DisplayModule_State.hdmiOutput.format = NEXUS_VideoFormat_eUnknown;
	#endif
	
    (void)NEXUS_VideoOutput_P_SetHdmiFormat(output, display, display->cfg.format, display->cfg.aspectRatio);

    BKNI_EnterCriticalSection();
    BDBG_ASSERT(NULL == display->hdmi.rateChangeCb_isr);
    display->hdmi.rateChangeCb_isr = NEXUS_VideoOutput_P_HdmiRateChange_isr;
    display->hdmi.vsync_isr = NEXUS_HdmiOutput_P_Vsync_isr;
    display->hdmi.pCbParam = hdmi;
    if ( display->hdmi.rateInfoValid )
    {
        NEXUS_VideoOutput_P_HdmiRateChange_isr(display, hdmi);
    }
    BKNI_LeaveCriticalSection();
    
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_VideoOutput_P_DisconnectHdmi(void *output, NEXUS_DisplayHandle display)
{
    BERR_Code rc;
    NEXUS_HdmiOutputHandle hdmi = output;
    NEXUS_HdmiOutputSettings settings;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;

    BDBG_ASSERT(NULL != hdmi);
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    BKNI_EnterCriticalSection();
    BDBG_ASSERT(NULL != display->hdmi.rateChangeCb_isr);
    display->hdmi.rateChangeCb_isr = NULL;
    display->hdmi.vsync_isr = NULL;
    display->hdmi.pCbParam = NULL;
    BKNI_LeaveCriticalSection();

    (void) NEXUS_HdmiOutput_GetSettings(hdmi, &settings);
    if (settings.mpaaDecimationEnabled) {
        rc = BVDC_Display_SetMpaaDecimation(display->displayVdc, BVDC_MpaaDeciIf_eHdmi, 1, false);
        if (rc) {
            rc = BERR_TRACE(rc); /* fall through */
        }
        else {
            /* two ApplyChanges are required: this one and the standard one */
            rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
            if (rc) {rc = BERR_TRACE(rc);} /* fall through */
        }   
    }

    /* Disable HDMI in VDC */
    rc = BVDC_Display_SetHdmiConfiguration(display->displayVdc, BVDC_Hdmi_0, BAVC_MatrixCoefficients_eUnknown);
    if ( rc )
    {
        return BERR_TRACE(rc);
    }

    /* APPLY VDC Changes */
    if(g_NEXUS_DisplayModule_State.updateMode != NEXUS_DisplayUpdateMode_eAuto) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(video->vdc);
    if ( rc )
    {
        return BERR_TRACE(rc);
    }

    /* Turn off the transmitter itself */
    NEXUS_Module_Lock(video->modules.hdmiOutput);
    rc = NEXUS_HdmiOutput_Disconnect_priv(hdmi);
    NEXUS_Module_Unlock(video->modules.hdmiOutput);
    if ( rc )
    {
        rc = BERR_TRACE(rc);
        return BERR_SUCCESS;
    }

    return rc;
}

static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenHdmi(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;

    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    BDBG_ASSERT(output->type == NEXUS_VideoOutputType_eHdmi);

    iface.connect = NEXUS_VideoOutput_P_ConnectHdmi;
    iface.disconnect = NEXUS_VideoOutput_P_DisconnectHdmi;
    iface.formatChange = NEXUS_VideoOutput_P_HdmiFormatChange;
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, false);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_eDvo;
    }
    return link;
}
#endif

#if NEXUS_NUM_RFM_OUTPUTS
static BERR_Code
NEXUS_Rfm_P_Connect(void *output,  NEXUS_DisplayHandle display)
{
    NEXUS_RfmHandle rfm = output;
    NEXUS_RfmConnectionSettings rfmConnectionSettings;
    unsigned index;
    BERR_Code rc;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;

    NEXUS_Module_Lock(video->modules.rfm);
    NEXUS_Rfm_GetIndex_priv(rfm, &index);
    NEXUS_Module_Unlock(video->modules.rfm);

    rc = BVDC_Display_SetRfmConfiguration(display->displayVdc, BVDC_Rfm_0 + index,  BVDC_RfmOutput_eCVBS, 0);
    if (rc) return BERR_TRACE(rc);

    NEXUS_Module_Lock(video->modules.rfm);
    NEXUS_Rfm_GetConnectionSettings_priv(rfm, &rfmConnectionSettings);
    rfmConnectionSettings.videoEnabled = true;
    rfmConnectionSettings.videoFormat = display->cfg.format;
    rc = NEXUS_Rfm_SetConnectionSettings_priv(rfm, &rfmConnectionSettings);
    NEXUS_Module_Unlock(video->modules.rfm);

    return rc;
}

static BERR_Code
NEXUS_Rfm_P_Disconnect(void *output, NEXUS_DisplayHandle display)
{
    NEXUS_RfmHandle rfm = output;
    NEXUS_RfmConnectionSettings rfmConnectionSettings;
    unsigned index;
    BERR_Code rc;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;

    NEXUS_Module_Lock(video->modules.rfm);
    NEXUS_Rfm_GetIndex_priv(rfm, &index);
    NEXUS_Module_Unlock(video->modules.rfm);

    rc = BVDC_Display_SetRfmConfiguration(display->displayVdc, BVDC_Rfm_0 + index,  BVDC_RfmOutput_eUnused, 0);
    if (rc) return BERR_TRACE(rc);

    NEXUS_Module_Lock(video->modules.rfm);
    NEXUS_Rfm_GetConnectionSettings_priv(rfm, &rfmConnectionSettings);
    rfmConnectionSettings.videoEnabled = false;
    rc = NEXUS_Rfm_SetConnectionSettings_priv(rfm, &rfmConnectionSettings);
    NEXUS_Module_Unlock(video->modules.rfm);

    return rc;
}

static BERR_Code
NEXUS_Rfm_P_FormatChange(void *output, NEXUS_DisplayHandle display, NEXUS_VideoFormat format, NEXUS_DisplayAspectRatio aspectRatio)
{
    NEXUS_RfmHandle rfm = output;
    NEXUS_RfmConnectionSettings rfmConnectionSettings;
    BERR_Code rc;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;
    BSTD_UNUSED(display);
    BSTD_UNUSED(aspectRatio);

    NEXUS_Module_Lock(video->modules.rfm);
    NEXUS_Rfm_GetConnectionSettings_priv(rfm, &rfmConnectionSettings);
    rfmConnectionSettings.videoEnabled = true;
    rfmConnectionSettings.videoFormat = format;
    rc = NEXUS_Rfm_SetConnectionSettings_priv(rfm, &rfmConnectionSettings);
    NEXUS_Module_Unlock(video->modules.rfm);

    return rc;
}

static NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_OpenRfm(NEXUS_VideoOutput output)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_VideoOutput_P_Iface iface;
    iface.connect = NEXUS_Rfm_P_Connect;
    iface.disconnect = NEXUS_Rfm_P_Disconnect;
    iface.formatChange = NEXUS_Rfm_P_FormatChange;
    link = NEXUS_VideoOutput_P_CreateLink(output, &iface, false);
    if (link) {
        link->displayOutput = BVDC_DisplayOutput_eComposite;
    }
    return link;
}
#endif

NEXUS_VideoOutput_P_Link *
NEXUS_P_VideoOutput_Link(NEXUS_VideoOutput output)
{
    BERR_Code rc;
    BDBG_ASSERT(output->destination==NULL);
    switch(output->type) {
#if NEXUS_NUM_HDMI_DVO
    case NEXUS_VideoOutputType_eHdmiDvo:
        return NEXUS_VideoOutput_P_OpenHdmiDvo(output);
#endif
    case NEXUS_VideoOutputType_eComposite:
        return NEXUS_VideoOutput_P_OpenComposite(output);
    case NEXUS_VideoOutputType_eComponent:
        return NEXUS_VideoOutput_P_OpenComponent(output);
    case NEXUS_VideoOutputType_eSvideo:
        return NEXUS_VideoOutput_P_OpenSvideo(output);
#if NEXUS_NUM_HDMI_OUTPUTS
    case NEXUS_VideoOutputType_eHdmi:
        return NEXUS_VideoOutput_P_OpenHdmi(output);
#endif
#if NEXUS_NUM_RFM_OUTPUTS
    case NEXUS_VideoOutputType_eRfm:
        return NEXUS_VideoOutput_P_OpenRfm(output);
#endif
#if NEXUS_NUM_SCART_INPUTS
    case NEXUS_VideoOutputType_eScartInput:
        return NEXUS_VideoOutput_P_OpenScartInput(output);
#endif
#if NEXUS_NUM_656_OUTPUTS
    case NEXUS_VideoOutputType_eCcir656:
        return NEXUS_VideoOutput_P_OpenCcir656(output);
#endif
    default:
        rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }
}

void
NEXUS_VideoOutput_Shutdown(NEXUS_VideoOutput output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    if (output->destination) {
        NEXUS_DisplayHandle display = NEXUS_VideoOutput_P_GetDisplay(output);
        if (display) {
            NEXUS_Display_RemoveOutput(display, output);
        }
    }
    return;
}

NEXUS_DisplayHandle NEXUS_VideoOutput_P_GetDisplay(NEXUS_VideoOutput output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);

    if (output->destination) {
        NEXUS_VideoOutput_P_Link *link = output->destination;
        BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
        return link->display;
    }
    return NULL;
}

void NEXUS_VideoOutput_GetVfFilter( NEXUS_VideoOutput output, NEXUS_VideoDac dac, NEXUS_VideoOutputVfFilter *pFilter )
{
    BERR_Code rc;
    bool override;
    NEXUS_VideoOutput_P_Link *link;
    
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    
    BKNI_Memset(pFilter, 0, sizeof(*pFilter));
    if (!output->destination) {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        goto done;
    }
    link = output->destination;
    BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
    
    if (dac >= NEXUS_VideoDac_eMax || dac == NEXUS_VideoDac_eNone) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto done;
    }
    
    rc = BVDC_Display_GetVfFilter(link->display->displayVdc, link->displayOutput, link->channel[dac-NEXUS_VideoDac_e0], &override, pFilter->filterRegs, NEXUS_MAX_VF_FILTER_ENTRIES);
    if (rc) {
        rc = BERR_TRACE(rc);
        pFilter->numEntries = 0;
    }
    /* TODO: VDC does not return actual number populated, so we just report back the max. */
    pFilter->numEntries = NEXUS_MAX_VF_FILTER_ENTRIES;
done:
    return;
}

NEXUS_Error NEXUS_VideoOutput_SetVfFilter( NEXUS_VideoOutput output, NEXUS_VideoDac dac, const NEXUS_VideoOutputVfFilter *pFilter )
{
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *link;
    
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    if (!output->destination) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }    
    link = output->destination;
    BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
    
    if (dac >= NEXUS_VideoDac_eMax || dac == NEXUS_VideoDac_eNone) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    if (pFilter) {
        rc = BVDC_Display_SetVfFilter(link->display->displayVdc, link->displayOutput, link->channel[dac-NEXUS_VideoDac_e0], true, (uint32_t*)pFilter->filterRegs, pFilter->numEntries);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        rc = BVDC_Display_SetVfFilter(link->display->displayVdc, link->displayOutput, link->channel[dac-NEXUS_VideoDac_e0], false, NULL, 0);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

void NEXUS_VideoOutput_GetSettings( NEXUS_VideoOutput output, NEXUS_VideoOutputSettings *pSettings )
{
    NEXUS_VideoOutput_P_Link *link;
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    if (!output->destination) {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
        return;
    }    
    link = output->destination;
    BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
    *pSettings = link->settings;
}
    
NEXUS_Error NEXUS_VideoOutput_SetSettings( NEXUS_VideoOutput output, const NEXUS_VideoOutputSettings *pSettings )
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    if (!output->destination) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }    
    link = output->destination;
    BDBG_OBJECT_ASSERT(link->display, NEXUS_Display);
    
    rc = BVDC_Display_SetMuteMode(link->display->displayVdc, link->displayOutput, pSettings->mute?BVDC_MuteMode_eConst:BVDC_MuteMode_eDisable);
    if (rc) return BERR_TRACE(rc);
    
    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    
    link->settings = *pSettings;
    return 0;
}
