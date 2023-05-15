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
 * $brcm_Workfile: nexus_hdmi_input_ext.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 7/6/11 12:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/3548/include/nexus_hdmi_input_ext.h $
 * 
 * 6   7/6/11 12:18p erickson
 * SWDTV-6771: merge
 * 
 * SWDTV-6771/2   6/23/11 2:50p rgreen
 * SWDTV-6771: Incorporate review comments for  ISRC packets
 * 
 * SWDTV-6771/1   6/22/11 2:44p rgreen
 * SWDTV-6771: Add support for HDMI ISRC packets
 * 
 * 5   6/12/09 1:25p erickson
 * PR36814: add NEXUS_HdmiInput_GetVendorSpecificInfoFrameData
 *
 * 4   10/9/08 5:05p erickson
 * PR43214: merge
 *
 * 3   9/22/08 4:17p erickson
 * PR45761: merge
 *
 * PR45761/1   9/17/08 5:27p honglang
 * PR45761: provide GCP and audio clock regeneration packet to the high
 *  level application
 *
 * 2   5/19/08 1:38p erickson
 * PR39453: HdmiInput updates
 *
 * 1   5/6/08 4:45p erickson
 * PR39453: add separate impl for 3548/3556
 *
 * 3   1/23/08 8:37p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * PR36159/1   1/9/08 8:38p honglang
 * PR36814: Add a new header file for passthrough functions : get packets
 * raw function group.
 *
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_INPUT_EXT_H__
#define NEXUS_HDMI_INPUT_EXT_H__

#include "nexus_types.h"
#include "nexus_hdmi_input_info.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
Get a copy of the original Packet Bytes received by the HDMI receiver
**/
NEXUS_Error NEXUS_HdmiInput_GetRawPacketData(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputPacketType packetType,
    NEXUS_HdmiPacket *pPacket   /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetAviInfoFrameData(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiAviInfoFrame *pAviInfoFrame /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetAudioInfoFrameData(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiAudioInfoFrame *pAudioInfoFrame /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetSpdInfoFrameData(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiSpdInfoFrame *pSpdInfoFrame /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetAudioContentProtection(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiAudioContentProtection *pAcp /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetAudioClockRegeneration(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiAudioClockRegeneration *pAudioClockRegeneration /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetGeneralControlPacket(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiGeneralControlPacket *pGeneralControlPacket /* [out] */
    );


/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_GetVendorSpecificInfoFrameData(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiVendorSpecificInfoFrame *pVendorSpecificInfoFrame /* [out] */
    );


/**
Summary: 
Function to retrieve International Standard Recording Code and/or UPC/EAN Data 
describing the origin or owner details for each track of content on the medium
**/
NEXUS_Error NEXUS_HdmiInput_GetISRCData( 
    NEXUS_HdmiInputHandle hdmiInput, 
    NEXUS_HdmiISRC *pISRCData /* [out] */
    ) ;

/**
Summary:
Debug function to get data to form Eye Diagram

Description:
Use this function to create an Eye Diagram.
**/
NEXUS_Error NEXUS_HdmiInput_DebugGetEyeDiagramData(
    NEXUS_HdmiInputHandle hdmiInput,
    uint8_t *pAdcData /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif

