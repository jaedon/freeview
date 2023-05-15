/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_input_connection.c $
 * $brcm_Revision: 24 $
 * $brcm_Date: 11/15/11 3:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_hdmi_input_connection.c $
 * 
 * 24   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 23   9/15/10 11:45a petlee
 * SW35230-1156: Fix logic on how hdmi gets connected to hddvi
 * 
 * 22   3/23/10 3:25p petlee
 * SW35230-106: Merging branch SW35230-106 to mainline
 * 
 * SW35230-106a/3   3/9/10 2:39p petlee
 * SW35230-106: Updating link object creation and hddvi routing
 *  implementation
 * 
 * SW35230-106a/2   3/9/10 2:01p petlee
 * SW35230-106: Updating link object creation and hddvi routing
 *  implementation
 * 
 * SW35230-106a/1   2/25/10 3:02p petlee
 * SW35230-106: Updating the way the link object is created for 35230
 * 
 * 21   1/21/10 11:27a erickson
 * SW3548-2727: move HD-DVI settings from NEXUS_HdmiInputModuleSettings to
 *  NEXUS_VideoHdmiInputSettings. this allows us to change settings at
 *  runtime per HD-DVI input.
 *
 * 20   9/21/09 2:52p erickson
 * SW3548-2475: use NEXUS_VideoInputStatus.videoPresent as the filter for
 *  noSignal. status.originalFormat will never go to eUnknown.
 *
 * 19   9/16/09 3:41p erickson
 * SW3548-2475: prevent false positives if BVDC_Source_GetInputStatus
 *  returns noSignal as true, but VDC source callback has a format. in
 *  this case we should force noSignal = false. if we really have lost
 *  signal, a VDC source callback will be forthcoming.
 *
 * 18   9/11/09 8:44a erickson
 * SW3548-2452: use NEXUS_VideoFormat from VDC source callback, not VDC
 *  get function
 *
 * 17   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 16   11/26/08 2:32p erickson
 * PR49228: merge
 *
 * PR49228/1   11/25/08 7:00p honglang
 * PR49226, PR49228: get the real VDC format detection info. The format
 *  info of the locked timing can be got by
 *  NEXUS_VideoFormat_GetInfo(NEXUS_VideoInputStatus.format,
 *  &videoFmtInfo)
 *
 * 15   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * 14   11/3/08 2:27p erickson
 * PR47030: add HdmiInput heap
 *
 * 13   10/22/08 11:40a erickson
 * PR48216: allow display to compile with no HdmiInput module
 *
 * 12   10/5/08 11:43p erickson
 * PR46176: Display must inform HdmiInput when its connected/disconnected
 *
 * 11   8/20/08 12:10p erickson
 * PR45616: prefer data from VDC Source callback over
 *  BVDC_Source_GetInputStatus
 *
 * 10   7/16/08 3:43p erickson
 * PR44871: remove premature BVDC_ApplyChanges
 *
 * 9   4/23/08 2:55p erickson
 * PR39009: merge
 *
 * PR39009/2   4/23/08 2:17p honglang
 * PR39009: HDMI disconnect should call BVDC_Source_InstallCallback
 *  disable HDDVI general callback because there's no way to re-install
 *  the callback due to HDMI input isn't removed and re-created.
 *
 * PR39009/1   4/17/08 12:33a honglang
 * PR39009: add more status to API
 *
 * 7   4/11/08 11:06a vsilyaev
 * PR 40410: Fixed naming typo
 *
 * 6   4/1/08 4:38p erickson
 * PR41149: memset structure, even if we don't intend to set every member
 *
 * 5   3/27/08 4:51p vsilyaev
 * PR 40818: Improved internal API to allow source private heaps
 *
 * 4   3/26/08 11:44p vsilyaev
 * PR 40410: Added more video settings for HDMI Input interface
 *
 * 3   3/26/08 12:53p erickson
 * PR40742: fix warning
 *
 * 2   3/26/08 10:54a erickson
 * PR40742: added more HdmiInput which is passed in from VDC source
 *  changed callback
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   12/18/07 1:34p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/1   11/9/07 11:16a erickson
 * PR36814: added more VDC HdDvi settings, moved to separate source file
 *
 **************************************************************************/
#include "nexus_display_module.h"

#if NEXUS_NUM_HDMI_INPUTS
#include "priv/nexus_hdmi_input_priv.h"

BDBG_MODULE(nexus_hdmi_input_connection);

#define pVideo (&g_NEXUS_DisplayModule_State)

static void NEXUS_VideoInput_P_HdmiPictureCallback_isr(void *pvParm1, int iParm2,
      BAVC_Polarity ePolarity, BAVC_SourceState eSourceState, void **ppvPicture)
{
    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eSourceState);
    NEXUS_HdmiInput_PictureCallback_isr((NEXUS_HdmiInputHandle)pvParm1, (BAVC_VDC_HdDvi_Picture **)ppvPicture);
}

static const BFMT_VideoFmt g_autoDetectFormats[] = {
    BFMT_VideoFmt_eNTSC,
    BFMT_VideoFmt_e480p,
    BFMT_VideoFmt_e720p,
    BFMT_VideoFmt_e1080i
};

static NEXUS_Error
NEXUS_VideoHdmiInput_P_ApplySettings(NEXUS_VideoInput_P_Link *inputLink, const NEXUS_VideoHdmiInputSettings *pSettings)
{
    BERR_Code rc;
    BFMT_VideoFmt formatVdc;
    BVDC_HdDvi_Settings HdDviSettings;

    BDBG_OBJECT_ASSERT(inputLink, NEXUS_VideoInput_P_Link);
/*    BDBG_ASSERT(inputLink->link && inputLink->link->type != NEXUS_VideoInputType_eHdmi); */
    BDBG_ASSERT(inputLink->sourceVdc);

    rc = BVDC_Source_SetHVStart(inputLink->sourceVdc, !pSettings->autoPosition, pSettings->position.x, pSettings->position.y);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    rc = BVDC_Source_SetAutoFormat(inputLink->sourceVdc, pSettings->autoFormat, (void *)g_autoDetectFormats, sizeof(g_autoDetectFormats)/sizeof(*g_autoDetectFormats));
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    if(!pSettings->autoFormat && pSettings->format != NEXUS_VideoFormat_eUnknown) {
        rc = NEXUS_P_VideoFormat_ToMagnum(pSettings->format, &formatVdc);
        if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BVDC_Source_SetVideoFormat(inputLink->sourceVdc, formatVdc);
        if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    }

    rc = BVDC_Source_GetHdDviConfiguration(inputLink->sourceVdc, &HdDviSettings);
    if (rc) return BERR_TRACE(rc);

    switch(pSettings->dataMode) {
    case NEXUS_HdDviDataMode_e36bit:
        HdDviSettings.eInputDataMode = BVDC_HdDvi_InputDataMode_e36Bit;
        break;
    case NEXUS_HdDviDataMode_e24bit:
        HdDviSettings.eInputDataMode = BVDC_HdDvi_InputDataMode_e24Bit;
        break;
    case NEXUS_HdDviDataMode_e30bit:
        HdDviSettings.eInputDataMode = BVDC_HdDvi_InputDataMode_e30Bit;
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    HdDviSettings.bEnableDe = pSettings->deEnabled;
    HdDviSettings.stFmtTolerence.ulWidth = pSettings->formatTolerance.width;
    HdDviSettings.stFmtTolerence.ulHeight = pSettings->formatTolerance.height;

    rc = BVDC_Source_SetHdDviConfiguration(inputLink->sourceVdc, &HdDviSettings);
    if (rc) return BERR_TRACE(rc);

    if (pSettings->externalInput) {
        rc = BVDC_Source_SetInputPort(inputLink->sourceVdc, BVDC_HdDviInput_Ext);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        rc = BVDC_Source_SetInputPort(inputLink->sourceVdc, BVDC_HdDviInput_Hdr);
        if (rc) return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
}

static BERR_Code
NEXUS_VideoInput_P_ConnectHdmiInput(NEXUS_VideoInput_P_Link *link)
{
    BERR_Code rc;

    /* NOTE: VideoInput might need to use BVDC_Source_InstallPictureCallback in the future. If so, it should
    call into this file. See NEXUS_VideoInput_P_HdmiSourceCallback_isr for an example. */
    rc = BVDC_Source_InstallPictureCallback(link->sourceVdc,
        NEXUS_VideoInput_P_HdmiPictureCallback_isr, (void *)link->input->source, 0);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_VideoHdmiInput_P_ApplySettings(link, &link->info.hdmi);
    if (rc) return BERR_TRACE(rc);

    /* cannot call BVDC_ApplyChanges here. more VDC settings need to be applied. */

    /* Tell HdmiInput we are connected */
    NEXUS_Module_Lock(pVideo->modules.hdmiInput);
    NEXUS_HdmiInput_VideoConnected_priv((NEXUS_HdmiInputHandle)link->input->source, true);
    NEXUS_HdmiInput_SetFormatChangeCb_priv((NEXUS_HdmiInputHandle)link->input->source, NEXUS_VideoInput_P_CheckFormatChange_isr, link);
    NEXUS_Module_Unlock(pVideo->modules.hdmiInput);

    return 0;
}

static void
NEXUS_VideoInput_P_DisconnectHdmiInput(NEXUS_VideoInput_P_Link *link)
{
    BERR_Code rc;

    rc = BVDC_Source_InstallPictureCallback(link->sourceVdc, NULL, NULL, 0);
    if (rc) rc = BERR_TRACE(rc);
    if(pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc) rc = BERR_TRACE(rc);

    /* Tell HdmiInput we are not connected */
    NEXUS_Module_Lock(pVideo->modules.hdmiInput);
    NEXUS_HdmiInput_VideoConnected_priv((NEXUS_HdmiInputHandle)link->input->source, false);
    NEXUS_HdmiInput_SetFormatChangeCb_priv((NEXUS_HdmiInputHandle)link->input->source, NULL, NULL);
    NEXUS_Module_Unlock(pVideo->modules.hdmiInput);
}

static void
NEXUS_VideoHdmiInput_P_GetDefaultSettings( NEXUS_VideoHdmiInputSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->autoFormat = true;
    pSettings->format = NEXUS_VideoFormat_eUnknown;
    pSettings->autoPosition = true;
    pSettings->dataMode = NEXUS_HdDviDataMode_e30bit;
    pSettings->deEnabled = false;
    pSettings->externalInput = false;
    pSettings->formatTolerance.width = 5;
    pSettings->formatTolerance.height = 3;
    return;
}

/* called from nexus_video_input.c */
NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_OpenHdmi(NEXUS_VideoInput input)
{
    BAVC_SourceId sourceId;
    NEXUS_VideoInput_P_Iface iface;
    NEXUS_VideoInput_P_Link *link;
    NEXUS_VideoInput_P_LinkData data;
    NEXUS_HdmiInputSettings hdmiInputSettings;

    BDBG_ASSERT(input->source);
    NEXUS_Module_Lock(pVideo->modules.hdmiInput);
    NEXUS_HdmiInput_GetSourceId_priv(input->source, &sourceId);
    NEXUS_Module_Unlock(pVideo->modules.hdmiInput);
    NEXUS_HdmiInput_GetSettings(input->source, &hdmiInputSettings);

    iface.connect = NEXUS_VideoInput_P_ConnectHdmiInput;
    iface.disconnect = NEXUS_VideoInput_P_DisconnectHdmiInput;
    NEXUS_VideoInput_P_LinkData_Init(&data, sourceId);
    data.heap = hdmiInputSettings.heap;
    link = NEXUS_VideoInput_P_CreateLink(input, &data, &iface);
    if(!link) {
        return NULL;
    }
    NEXUS_VideoHdmiInput_P_GetDefaultSettings(&link->info.hdmi);
    return link;
}

void
NEXUS_VideoInput_P_CloseHdmi(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_VideoInput_P_DestroyLink(link);
}

void
NEXUS_VideoInput_P_SetHdmiInputStatus(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_HdmiInputStatus status;
    BVDC_Source_InputStatus vdcInputStatus;
    BERR_Code rc;

    BKNI_Memset(&status, 0, sizeof(status));

    /* we are not setting every field in NEXUS_HdmiInputStatus. only set the fields which HdmiInput cannot get itself. this
    code must stay in sync with NEXUS_HdmiInput_GetStatus. */

    /* prefer vdcSourceCallbackData data */
    BKNI_EnterCriticalSection();
    if (!link->vdcSourceCallbackData.bActive || link->vdcSourceCallbackData.pFmtInfo == NULL) {
        status.interlaced = true; /* default to interlaced if unknown */
        status.vPolarity = NEXUS_VideoPolarity_ePositive;
        status.hPolarity = NEXUS_VideoPolarity_ePositive;
    }
    else {
        status.interlaced = link->vdcSourceCallbackData.pFmtInfo->bInterlaced;
    }
    /* copy data from NEXUS_VideoInputStatus because we prefer data from VDC source callback */
    status.originalFormat = link->status.format;
    BKNI_LeaveCriticalSection();

    /* but supplement with BVDC_Source_GetInputStatus if needed */
    rc = BVDC_Source_GetInputStatus(link->sourceVdc, &vdcInputStatus);
    if (!rc) {
        if (link->status.videoPresent) {
            /* because vdcInputStatus.bNoSignal provides instantaneous HW state, it is possible that bNoSignal is true even if we're locked (i.e. we have a video format).
            this creates unnecessary problems in SW state. instead, filter it here. if we're not locked, noSignal can provide
            valuable information. */
            status.noSignal = false;
        }
        else {
            status.noSignal = vdcInputStatus.bNoSignal;
        }

        status.vBlank = vdcInputStatus.ulVBlank;
        status.hBlank = vdcInputStatus.ulHBlank;
        status.vertFreq =  vdcInputStatus.ulVertFreq;
        status.avWidth =  vdcInputStatus.ulAvWidth;
        status.avHeight = vdcInputStatus.ulAvHeight;
        status.vPolarity = vdcInputStatus.ulVPolarity;
        status.hPolarity = vdcInputStatus.ulHPolarity;
    }

    NEXUS_Module_Lock(pVideo->modules.hdmiInput);
    NEXUS_HdmiInput_SetStatus_priv((NEXUS_HdmiInputHandle)link->input->source, &status);
    NEXUS_Module_Unlock(pVideo->modules.hdmiInput);
}

void
NEXUS_VideoInput_P_HdmiSourceCallback_isr(NEXUS_VideoInput_P_Link *link,const  BVDC_Source_CallbackData *pData)
{
    if (pData->stDirty.bFrameRate) {
        NEXUS_HdmiInput_SetFrameRate_isr(link->input->source, pData->eFrameRateCode);
    }
}

#endif

void
NEXUS_VideoHdmiInput_GetSettings(NEXUS_VideoInput input, NEXUS_VideoHdmiInputSettings *pSettings)
{
#if NEXUS_NUM_HDMI_INPUTS
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    BDBG_ASSERT(pSettings);
    if(input->type == NEXUS_VideoInputType_eHdmi) {
        NEXUS_VideoInput_P_Link *inputLink  = NEXUS_VideoInput_P_Get(input);
        if(inputLink) {
            *pSettings = inputLink->info.hdmi;
            return;
        }
    } else {
        BDBG_ERR(("NEXUS_VideoHdmiInput_GetSettings: %#lx unsupported type %u", (unsigned long)input, (unsigned)input->type));
    }
    NEXUS_VideoHdmiInput_P_GetDefaultSettings(pSettings);
#else
    BSTD_UNUSED(input);
    BSTD_UNUSED(pSettings);
#endif
}


NEXUS_Error
NEXUS_VideoHdmiInput_SetSettings(NEXUS_VideoInput input, const NEXUS_VideoHdmiInputSettings *pSettings)
{
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_Error rc;
    NEXUS_VideoInput_P_Link *inputLink;
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    BDBG_ASSERT(pSettings);
    if(input->type != NEXUS_VideoInputType_eHdmi) { return BERR_TRACE(BERR_INVALID_PARAMETER); }

    inputLink  = NEXUS_VideoInput_P_Get(input);
    if(!inputLink) { return BERR_TRACE(BERR_NOT_SUPPORTED); }
    if(inputLink->sourceVdc) {
        rc = NEXUS_VideoHdmiInput_P_ApplySettings(inputLink, pSettings);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_apply;}
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    }
    inputLink->info.hdmi = *pSettings;
    return NEXUS_SUCCESS;
err_apply:
    return rc;
#else
    BSTD_UNUSED(input);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

