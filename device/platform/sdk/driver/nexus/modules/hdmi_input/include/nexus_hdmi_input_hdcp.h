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
 * $brcm_Workfile: nexus_hdmi_input_hdcp.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 4/18/11 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/3548/include/nexus_hdmi_input_hdcp.h $
 * 
 * 3   4/18/11 12:22p erickson
 * SW7422-186: merge
 * 
 * SW7422-186/3   4/15/11 2:56p rgreen
 * SW7422-186: API Review updates
 * 
 * SW7422-186/2   4/11/11 2:55p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * SW7422-186/1   3/28/11 5:37p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * 2   2/16/11 5:21p erickson
 * SW7422-129: merge
 *
 * SW7422-129/2   2/10/11 3:48p rgreen
 * SW7422-129: Remove HdcpInit and HdcpUnit calls; use Hdcp Get/Set
 *  Settings instead
 *
 * SW7422-129/1   2/2/11 6:26p rgreen
 * SW7422-129: Add HdcpSettings to HdmiInputSettings
 *
 * 1   12/23/10 10:15a erickson
 * SW35230-2374: add hdcp interface
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_INPUT_HDCP_H__
#define NEXUS_HDMI_INPUT_HDCP_H__

#include "nexus_hdmi_input.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
**/
void NEXUS_HdmiInput_HdcpGetDefaultSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpSettings *pSettings /* [out] */
    )  ;

/**
Summary:
**/
void NEXUS_HdmiInput_HdcpGetSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_HdmiInput_HdcpSetSettings(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputHdcpSettings *pSettings
    );


/**
Summary:
Default Key Set is the Test Key Set from HDCP Specification and is not 
compatible with production devices
**/
void NEXUS_HdmiInput_HdcpGetDefaultKeyset(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHdcpKeyset *pKeyset /* [out] */
    );

/**
Summary:
Initialize the HDMI Input  HDMI HDCP Key Set

Keyset initialization is normally needed once per cold boot
for platforms loading encrypted keys into on-chip RAM 
**/
NEXUS_Error NEXUS_HdmiInput_HdcpSetKeyset(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiInputHdcpKeyset *pKeyset /* [out] */
    ) ;

/**
Summary:
Get the status of the HDMI HDCP: Key Set, HDCP state etc.
**/
NEXUS_Error NEXUS_HdmiInput_HdcpGetStatus(
   NEXUS_HdmiInputHandle hdmiInput,
   NEXUS_HdmiInputHdcpStatus *pHdcpStatus /* [out] */
   );


/**
Summary:
For use with HDMI HDCP Repeaters
Load the Downstream KSVs read from the downstream devices connected
to a HDMI transmitter used in the same chip/box.  The KSVs are loaded
into the HDMI Input KSV FIFO that will be read by the upstream
transmitter connected to the hdmiInput
**/
NEXUS_Error NEXUS_HdmiInput_HdcpLoadKsvFifo(
    NEXUS_HdmiInputHandle hdmiInput,
    const NEXUS_HdmiHdcpDownStreamInfo *pDownstreamInfo,
    const NEXUS_HdmiHdcpKsv *pDownstreamKsvs, /* attr{nelem=numDevices} */
	unsigned numDevices
	);

#ifdef __cplusplus
}
#endif

#endif

