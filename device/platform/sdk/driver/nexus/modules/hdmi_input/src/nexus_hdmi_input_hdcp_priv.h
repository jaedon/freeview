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
 * $brcm_Workfile: nexus_hdmi_input_hdcp_priv.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 4/18/11 1:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/7425/src/nexus_hdmi_input_hdcp_priv.h $
 * 
 * 4   4/18/11 1:42p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 * 
 * SW7422-186/3   4/13/11 1:56p rgreen
 * SW7422-186: Replace RequestKsvFifoEvent with generic hdcpEvent
 * 
 * SW7422-186/1   3/31/11 3:15p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * SW7422-186/1   3/31/11 2:47p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * SW7422-186/1   3/31/11 2:38p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * SW7422-186/1   3/28/11 5:38p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * 3   3/3/11 1:04p rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 *  OTP ROM based chips
 * 
 * 2   2/1/11 3:32p rgreen
 * SW7422-129: Add private declaration for HDCP initialization
 * 
 * 1   12/17/10 5:23p rgreen
 * SW35230-2374: Add separate files for HDCP related functions
 * 
 **************************************************************************/

NEXUS_Error NEXUS_HdmiInput_P_InitHdcp(NEXUS_HdmiInputHandle hdmiInput) ;


void NEXUS_HdmiInput_P_HdcpStateChange_isr(void *context, int param2, void *data);

void NEXUS_HdmiInput_P_HdcpProcessChange(void *pContext) ;


