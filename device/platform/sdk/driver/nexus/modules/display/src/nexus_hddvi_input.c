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
 * $brcm_Workfile: nexus_hddvi_input.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 10/12/12 3:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_hddvi_input.c $
 * 
 * 7   10/12/12 3:15p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle
 * 
 * 6   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 5   2/13/12 3:43p erickson
 * SW7231-656: fix coverity warning
 * 
 * 4   11/17/11 4:57p erickson
 * SW7231-387: autoFormat should default to true
 * 
 * 3   12/2/10 8:14p spothana
 * SW7420-1177: Adding format and autoformat variables to HDDVI.
 * 
 * SW7420-1177/2   11/5/10 4:04p spothana
 * SW7420-1177: Set the DVI port to external input.
 * 
 * SW7420-1177/1   11/5/10 3:28p spothana
 * SW7420-1177: Updating nexus_hddvi module
 * 
 * 2   7/31/09 3:27p nickh
 * PR56969: Set default inputDataMode
 * 
 * 1   7/23/09 10:39a erickson
 * PR56969: add hddvi input
 *
 **************************************************************************/
#include "nexus_display_module.h"
#include "nexus_power_management.h"
#include "bfmt.h"
BDBG_MODULE(nexus_hddvi_input);

struct NEXUS_HdDviInput {
    NEXUS_OBJECT(NEXUS_HdDviInput);
    NEXUS_HdDviInputSettings settings;
    NEXUS_VideoInputObject input;
    BAVC_SourceId sourceId;
    BAVC_VDC_HdDvi_Picture stHdDviPic;
    unsigned index;
};
static const BFMT_VideoFmt g_autoDetectFormats[] = {
    BFMT_VideoFmt_eNTSC ,                   
    BFMT_VideoFmt_e1080i,                   
    BFMT_VideoFmt_e1080p,                   
    BFMT_VideoFmt_e720p,                       
    BFMT_VideoFmt_e480p,                       
    BFMT_VideoFmt_e1080p_30Hz                 
};

#if NEXUS_NUM_HDDVI_INPUTS
static void NEXUS_VideoInput_P_HdDviInputPictureCallback_isr(void *pvParm1, int iParm2,
      BAVC_Polarity ePolarity, BAVC_SourceState eSourceState, void **ppvPicture)
{
    NEXUS_HdDviInputHandle hdDviInput =(NEXUS_HdDviInputHandle)pvParm1;
    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eSourceState);
    *ppvPicture = &hdDviInput->stHdDviPic;
    BDBG_MSG(("NEXUS_VideoInput_P_HdDviInputPictureCallback_isr"));
    return;
}
#endif

void NEXUS_HdDviInput_GetDefaultSettings( NEXUS_HdDviInputSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->autoFormat = true;
    pSettings->inputDataMode = 24; /* 24 bit */
    pSettings->format = NEXUS_VideoFormat_eNtsc;
}

NEXUS_HdDviInputHandle NEXUS_HdDviInput_Open( unsigned index, const NEXUS_HdDviInputSettings *pSettings )
{
#if NEXUS_NUM_HDDVI_INPUTS
    NEXUS_HdDviInputHandle hdDviInput;
    NEXUS_HdDviInputSettings defaultSettings;

    if (index >= NEXUS_NUM_HDDVI_INPUTS) {
        BDBG_ERR(("Cannot open HdDviInput %d. NEXUS_NUM_HDDVI_INPUTS is %d.", index, NEXUS_NUM_HDDVI_INPUTS));
        return NULL;
    }

    if (!pSettings) {
        NEXUS_HdDviInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    hdDviInput = BKNI_Malloc(sizeof(*hdDviInput));
    if (!hdDviInput) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_HdDviInput, hdDviInput);
    NEXUS_VIDEO_INPUT_INIT(&hdDviInput->input, NEXUS_VideoInputType_eHdDvi, hdDviInput);
    hdDviInput->index = index;
    hdDviInput->settings = *pSettings;

    hdDviInput->stHdDviPic.eCscMode                 = BAVC_CscMode_e709RgbFullRange;
    hdDviInput->stHdDviPic.eColorSpace              = BAVC_Colorspace_eRGB;
    hdDviInput->stHdDviPic.eMatrixCoefficients      = BAVC_MatrixCoefficients_eItu_R_BT_709;
    hdDviInput->stHdDviPic.eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709;
    
    switch (index) {
    case 0: hdDviInput->sourceId = BAVC_SourceId_eHdDvi0; break;
#if NEXUS_NUM_HDDVI_INPUTS > 1
    case 1: hdDviInput->sourceId = BAVC_SourceId_eHdDvi1; break;
#endif
    /* add each one explicitly. don't assume BAVC_SourceId enums are contiguous */
    default: BERR_TRACE(NEXUS_INVALID_PARAMETER); goto error;
    }

    NEXUS_OBJECT_REGISTER(NEXUS_VideoInput, &hdDviInput->input, Open);
    return hdDviInput;

error:
    NEXUS_HdDviInput_Close(hdDviInput);
    return NULL;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
#endif
}


static void NEXUS_HdDviInput_P_Finalizer( NEXUS_HdDviInputHandle hdDviInput )
{
    NEXUS_OBJECT_ASSERT(NEXUS_HdDviInput, hdDviInput);

    /* This input is in the Display module, so we can auto-disconnect & shutdown */
    if (hdDviInput->input.destination) {
        NEXUS_VideoWindow_RemoveInput(hdDviInput->input.destination, &hdDviInput->input);
        BDBG_ASSERT(!hdDviInput->input.destination);
    }
    if (hdDviInput->input.ref_cnt) {
        NEXUS_VideoInput_Shutdown(&hdDviInput->input);
        BDBG_ASSERT(!hdDviInput->input.ref_cnt);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_HdDviInput, hdDviInput);
    BKNI_Free(hdDviInput);
}

static void NEXUS_HdDviInput_P_Release( NEXUS_HdDviInputHandle hdDviInput )
{
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    NEXUS_OBJECT_UNREGISTER(NEXUS_VideoInput, &hdDviInput->input, Close);
    return;
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_HdDviInput, NEXUS_HdDviInput_Close);

void NEXUS_HdDviInput_GetSettings( NEXUS_HdDviInputHandle hdDviInput, NEXUS_HdDviInputSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    *pSettings = hdDviInput->settings;
}

#if NEXUS_NUM_HDDVI_INPUTS
static NEXUS_Error NEXUS_HdDviInput_P_ApplyVdcSettings(NEXUS_VideoInput_P_Link *link, const NEXUS_HdDviInputSettings *pSettings)
{
    BERR_Code rc;
    BVDC_HdDvi_Settings vdcSettings;
    BFMT_VideoFmt formatVdc;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    BDBG_MSG((" > NEXUS_HdDviInput_P_ApplyVdcSettings"));

    rc = BVDC_Source_SetAutoFormat(link->sourceVdc, pSettings->autoFormat, (void *)g_autoDetectFormats, sizeof(g_autoDetectFormats)/sizeof(*g_autoDetectFormats));
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    if(!pSettings->autoFormat && pSettings->format != NEXUS_VideoFormat_eUnknown) {
        rc = NEXUS_P_VideoFormat_ToMagnum(pSettings->format, &formatVdc);
        if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BVDC_Source_SetVideoFormat(link->sourceVdc, formatVdc);
        if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    }

    (void)BVDC_Source_GetHdDviConfiguration(link->sourceVdc, &vdcSettings);
    vdcSettings.bEnableDe = pSettings->enableDe;
    vdcSettings.stFmtTolerence.ulWidth = pSettings->formatDetectionTolerance.width;
    vdcSettings.stFmtTolerence.ulHeight = pSettings->formatDetectionTolerance.height;

    switch (pSettings->inputDataMode) {
    case 24: vdcSettings.eInputDataMode = BVDC_HdDvi_InputDataMode_e24Bit; break;
    case 30: vdcSettings.eInputDataMode = BVDC_HdDvi_InputDataMode_e30Bit; break;
    case 36: vdcSettings.eInputDataMode = BVDC_HdDvi_InputDataMode_e36Bit; break;
    default: return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    BVDC_Source_SetInputPort(link->sourceVdc, 0);
    rc = BVDC_Source_SetHdDviConfiguration(link->sourceVdc, &vdcSettings);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_ApplyChanges(video->vdc);
    if ( rc )
    {
        return BERR_TRACE(rc);
    }
    BDBG_MSG((" < NEXUS_HdDviInput_P_ApplyVdcSettings"));
    return 0;
}
#endif

NEXUS_Error NEXUS_HdDviInput_SetSettings( NEXUS_HdDviInputHandle hdDviInput, const NEXUS_HdDviInputSettings *pSettings )
{
#if NEXUS_NUM_HDDVI_INPUTS
    NEXUS_VideoInput_P_Link *link;

    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    BDBG_MSG((" > NEXUS_HdDviInput_SetSettings"));
    hdDviInput->settings = *pSettings;

    link = hdDviInput->input.source;
    if (!link) return 0; /* not connected */
    BDBG_MSG((" < NEXUS_HdDviInput_SetSettings"));
    return NEXUS_HdDviInput_P_ApplyVdcSettings(link, &hdDviInput->settings);
#else
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    hdDviInput->settings = *pSettings;
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_VideoInput NEXUS_HdDviInput_GetConnector( NEXUS_HdDviInputHandle hdDviInput )
{
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    return &hdDviInput->input;
}

NEXUS_Error NEXUS_HdDviInput_GetStatus( NEXUS_HdDviInputHandle hdDviInput, NEXUS_HdDviInputStatus *pStatus )
{
    BFMT_AspectRatio vdcAspectRatio;
    BFMT_VideoFmt vdcVideoFormat;
    NEXUS_VideoInput_P_Link *link;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    link = hdDviInput->input.source;
    if (!link) return 0; /* not connected */

    BDBG_ASSERT(link->sourceVdc);

    rc = BVDC_Source_GetAspectRatio(link->sourceVdc, &vdcAspectRatio);
    if (rc) return BERR_TRACE(rc);
    pStatus->aspectRatio = NEXUS_P_AspectRatio_FromMagnum(vdcAspectRatio);

    rc = BVDC_Source_GetVideoFormat(link->sourceVdc, &vdcVideoFormat);
    if (rc) return BERR_TRACE(rc);
    pStatus->videoFormat = NEXUS_P_VideoFormat_FromMagnum(vdcVideoFormat);

    return 0;
}

#if NEXUS_NUM_HDDVI_INPUTS
static NEXUS_Error
NEXUS_VideoInput_P_ConnectHdDviInput(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_HdDviInputHandle hdDviInput;

    BDBG_ASSERT(link->input);
    BDBG_ASSERT(link->input->type == NEXUS_VideoInputType_eHdDvi);
    hdDviInput = link->input->source;
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    BDBG_MSG((" < NEXUS_VideoInput_P_ConnectHdDviInput"));
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHdDviInput, true);
    BVDC_Source_InstallPictureCallback(link->sourceVdc,NEXUS_VideoInput_P_HdDviInputPictureCallback_isr, 
                                       (void *)link->input->source, (BAVC_SourceId_eHdDvi0 == hdDviInput->sourceId) ? 0 : 1);
    BDBG_MSG((" > NEXUS_VideoInput_P_ConnectHdDviInput"));
    return NEXUS_HdDviInput_P_ApplyVdcSettings(link, &hdDviInput->settings);
}

static void
NEXUS_VideoInput_P_DisconnectHdDviInput(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_HdDviInputHandle hdDviInput;

    BDBG_ASSERT(link->input);
    BDBG_ASSERT(link->input->type == NEXUS_VideoInputType_eHdDvi);
    hdDviInput = link->input->source;
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);
    BDBG_MSG((" > NEXUS_VideoInput_P_DisconnectHdDviInput"));
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eHdDviInput, false);
    return;
}

NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_OpenHdDviInput(NEXUS_VideoInput input)
{
    NEXUS_VideoInput_P_Iface iface;
    NEXUS_VideoInput_P_Link *link;
    NEXUS_VideoInput_P_LinkData data;
    NEXUS_HdDviInputHandle hdDviInput;
    BDBG_MSG((" > NEXUS_VideoInput_P_OpenHdDviInput"));
    BDBG_ASSERT(input->type == NEXUS_VideoInputType_eHdDvi);
    hdDviInput = input->source;
    BDBG_OBJECT_ASSERT(hdDviInput, NEXUS_HdDviInput);

    iface.connect = NEXUS_VideoInput_P_ConnectHdDviInput;
    iface.disconnect = NEXUS_VideoInput_P_DisconnectHdDviInput;
    NEXUS_VideoInput_P_LinkData_Init(&data, hdDviInput->sourceId);
    link = NEXUS_VideoInput_P_CreateLink(input, &data, &iface);
    if(!link) {
        return NULL;
    }
    BDBG_MSG((" < NEXUS_VideoInput_P_OpenHdDviInput"));
    return link;
}
#endif

/*
Other possible VDC HdDvi functions to be added:
BVDC_Source_SetAutoFormat
BVDC_Source_SetVideoFormat
BVDC_Window_SetUserPanScan
BVDC_Source_OverrideAspectRatio
*/

