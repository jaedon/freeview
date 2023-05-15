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
 * $brcm_Workfile: nexus_hdmi_input_init.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/14/10 3:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/3548/include/nexus_hdmi_input_init.h $
 * 
 * 3   7/14/10 3:20p erickson
 * SW7405-4621: change to nexus_types.h
 * 
 * 2   1/21/10 11:27a erickson
 * SW3548-2727: move HD-DVI settings from NEXUS_HdmiInputModuleSettings to
 *  NEXUS_VideoHdmiInputSettings. this allows us to change settings at
 *  runtime per HD-DVI input.
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
 * Nexus_Devel/5   11/30/07 11:12a erickson
 * PR35457: api update
 *
 * Nexus_Devel/4   11/28/07 10:37a erickson
 * PR35457: doc update
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_INPUT_INIT_H__
#define NEXUS_HDMI_INPUT_INIT_H__

/*=========================================
The HdmiInput module provides an Interface for controlling the HDMI receiver.

See NEXUS_HdmiInput_Open and related functions.
===========================================*/

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
HdmiInput module settings
**/
typedef struct NEXUS_HdmiInputModuleSettings
{
    struct
    {
        NEXUS_ModuleHandle security; /* NEXUS_SecurityModule_Open. Needed for HDCP. */
    } modules;
} NEXUS_HdmiInputModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_HdmiInputModule_GetDefaultSettings(
    NEXUS_HdmiInputModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the HdmiInput module.
**/
NEXUS_ModuleHandle NEXUS_HdmiInputModule_Init(
    const NEXUS_HdmiInputModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the HdmiInput module.
**/
void NEXUS_HdmiInputModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
