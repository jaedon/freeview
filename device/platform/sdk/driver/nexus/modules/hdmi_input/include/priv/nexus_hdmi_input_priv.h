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
 * $brcm_Workfile: nexus_hdmi_input_priv.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/19/12 12:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/3563/include/priv/nexus_hdmi_input_priv.h $
 * 
 * 7   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 6   4/18/11 12:22p erickson
 * SW7422-186: merge
 * 
 * SW7422-186/1   4/7/11 5:59p rgreen
 * SW7422-186:  Add HDCP Repeater Support
 * 
 * 5   3/6/09 4:57p erickson
 * PR51307: Add support for HDMI HDCP Repeater
 *
 * 4   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 3   10/5/08 11:43p erickson
 * PR46176: Display must inform HdmiInput when its connected/disconnected
 *
 * 2   3/26/08 10:54a erickson
 * PR40742: added more HdmiInput which is passed in from VDC source
 *  changed callback
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   1/17/08 12:23p jgarrett
 * PR 36814: Adding GetIndex
 *
 * Nexus_Devel/2   11/9/07 11:50a erickson
 * PR36814: updates
 *
 * Nexus_Devel/1   11/8/07 2:40p erickson
 * PR36814: add hdmi_input
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_INPUT_PRIV_H__
#define NEXUS_HDMI_INPUT_PRIV_H__


#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_init.h"
#include "bavc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Called by Display
**/
void NEXUS_HdmiInput_PictureCallback_isr(
    NEXUS_HdmiInputHandle hdmiInput,
    BAVC_VDC_HdDvi_Picture **ppPicture /* in/out param */
    );

void NEXUS_HdmiInput_GetSourceId_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    BAVC_SourceId *id /* [out] */
    );

void NEXUS_HdmiInput_SetFrameRate_isr(
    NEXUS_HdmiInputHandle hdmiInput,
    BAVC_FrameRateCode frameRate
    );

/* Display module informs HdmiInput that its connection to a VideoWindow has changed. */
void NEXUS_HdmiInput_VideoConnected_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    bool connected
    );

/* Display module needs to read HdmiInput module settings to apply to VDC */
void NEXUS_HdmiInput_GetModuleSettings_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputModuleSettings *pSettings
    );

/**
Called by Audio
**/
void NEXUS_HdmiInput_GetIndex_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    unsigned *pIndex /* [out] */
    );

/**
Pass HdmiInput status which can only be obtained by the Display module
**/
void NEXUS_HdmiInput_SetStatus_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputStatus *pStatus
    );

/**
Pass HdmiInput status which can only be obtained by the Display module
**/
typedef void (*NEXUS_HdmiInputFormatChangeCb)(void *pParam);
void NEXUS_HdmiInput_SetFormatChangeCb_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputFormatChangeCb pFunction_isr,
    void *pFuncParam
    );

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_HdmiInput);

#ifdef __cplusplus
}
#endif

#endif
