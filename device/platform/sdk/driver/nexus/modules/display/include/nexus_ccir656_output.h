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
 * $brcm_Workfile: nexus_ccir656_output.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/25/11 3:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_ccir656_output.h $
 * 
 * 2   7/25/11 3:00p jhaberf
 * SWDTV-8043: added null_allowed attribute for some APIs
 * 
 * PR53623/2   3/31/09 11:57a mward
 * PR 53623: Dummy to prevent compiler warning.
 * 
 * PR53623/1   3/31/09 10:26a mward
 * PR 53623: 656 output support.
 *
 **************************************************************************/
#ifndef NEXUS_CCIR656_OUTPUT_H__
#define NEXUS_CCIR656_OUTPUT_H__

#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the CCIR-656 video output interface.
**/
typedef struct NEXUS_Ccir656Output *NEXUS_Ccir656OutputHandle;

/**
Summary:
Settings for CCIR-656 video output interface
**/
typedef struct NEXUS_Ccir656OutputSettings
{
    struct {
        /* TODO: see nexus_ccir656_input.h for 656 VBI formats. we may need to move that enum to core if they are needed. */
        int dummy;
    } vbi;
} NEXUS_Ccir656OutputSettings;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
*/
void NEXUS_Ccir656Output_GetDefaultSettings(
    NEXUS_Ccir656OutputSettings *pSettings /* [out] */
    );

/**
Summary:
Open a new CCIR-656 video output interface
**/
NEXUS_Ccir656OutputHandle NEXUS_Ccir656Output_Open( /* attr{destructor=NEXUS_Ccir656Output_Close}  */
    unsigned index,
    const NEXUS_Ccir656OutputSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Close the CCIR-656 video output interface
**/
void NEXUS_Ccir656Output_Close(
    NEXUS_Ccir656OutputHandle output
    );

/**
Summary:
Get current settings
**/
void NEXUS_Ccir656Output_GetSettings(
    NEXUS_Ccir656OutputHandle output,
    NEXUS_Ccir656OutputSettings *pSettings    /* [out] */
    );

/**
Summary:
Apply new settings
**/
NEXUS_Error NEXUS_Ccir656Output_SetSettings(
    NEXUS_Ccir656OutputHandle output,
    const NEXUS_Ccir656OutputSettings *pSettings
    );

/**
Summary:
Returns the abstract NEXUS_VideoOutput connector for this Interface.
The NEXUS_VideoOutput connector is added to a Display in order to route that Displa's video to the output.

Description:
Used in NEXUS_Display_AddOutput
**/
NEXUS_VideoOutput NEXUS_Ccir656Output_GetConnector(
    NEXUS_Ccir656OutputHandle output
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_CCIR656_OUTPUT_H__ */
