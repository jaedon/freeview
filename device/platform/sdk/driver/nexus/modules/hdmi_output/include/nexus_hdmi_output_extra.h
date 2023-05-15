/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_output_extra.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/18/10 4:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/7400/include/nexus_hdmi_output_extra.h $
 * 
 * 3   6/18/10 4:33p erickson
 * SW3548-2952: Add hdmi debug command to start (hdm_packet_start) and
 *  stop (hdm_packet_stop) the transmission of selected hdmi packets
 *
 * 2   11/2/09 3:11p erickson
 * SW7405-2885: add debug functions
 *
 * 1   9/14/09 2:32p erickson
 * SW7405-2885: new debug functions
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_OUTPUT_EXTRA_H__
#define NEXUS_HDMI_OUTPUT_EXTRA_H__

#include "nexus_hdmi_output.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Display attached receiver's EDID information
**/
void NEXUS_HdmiOutput_DisplayRxEdid(
    NEXUS_HdmiOutputHandle handle
    );

/**
Summary:
Display attached receiver's EDID information
**/
void NEXUS_HdmiOutput_DisplayRxInfo(
    NEXUS_HdmiOutputHandle handle
    );

/**
Summary:
Get BCM_VideoFmt from the nth Detail Timing block
**/
void NEXUS_HdmiOutput_GetVideoFormatFromDetailTiming(
    NEXUS_HdmiOutputHandle handle,
    uint8_t detailTimingNumber
    );

/**
Summary:
Modify the colorimetry indicate in the AVI Infoframe
**/
void NEXUS_HdmiOutput_SetAviInfoFrameColorimetry(
    NEXUS_HdmiOutputHandle handle,
    uint8_t colorimetry
    );

/**
Summary:
Modify the picture aspect ratio indicated in the AVI Infoframe
**/
void NEXUS_HdmiOutput_SetAviInfoFrameAspectRatio(
    NEXUS_HdmiOutputHandle handle,
    uint8_t aspectRatio
    );

/**
Summary:
Enable/Disable Pj Checking
**/
void NEXUS_HdmiOutput_EnablePjChecking(
    NEXUS_HdmiOutputHandle handle,
    bool enable
    );

/**
Summary:
Force video pixel to all 0s or 1s
**/
void NEXUS_HdmiOutput_ForceVideoPixel(
    NEXUS_HdmiOutputHandle handle,
    uint8_t pixelValue
    );


/**
Summary:
Enable packet transmission
**/
void NEXUS_HdmiOutput_EnablePacketTransmission(
    NEXUS_HdmiOutputHandle handle,
    uint8_t packetChoice
    );

/**
Summary:
Disable packet transmission
**/
void NEXUS_HdmiOutput_DisablePacketTransmission(
    NEXUS_HdmiOutputHandle handle,
    uint8_t packetChoice
    );


#ifdef __cplusplus
}
#endif

#endif
