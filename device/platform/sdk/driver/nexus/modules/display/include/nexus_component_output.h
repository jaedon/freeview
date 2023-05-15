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
 * $brcm_Workfile: nexus_component_output.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 7/25/11 3:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_component_output.h $
 * 
 * 9   7/25/11 3:00p jhaberf
 * SWDTV-8043: added null_allowed attribute for some APIs
 * 
 * 8   12/23/10 11:28a erickson
 * SW7405-5070: added comment for SCART component output
 *
 * 7   5/14/09 9:02a erickson
 * PR55101: add NEXUS_ComponentOutputSettings.dacs.RGB.noSync
 *
 * 6   4/11/08 11:05a erickson
 * PR40157: add HSync option to ComponentOutput for VGA output
 *
 * 5   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 4   2/6/08 12:42p vsilyaev
 * PR 38682: Added attributes for the linux/kernel proxy mode
 *
 * 3   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 2:39p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#ifndef NEXUS_COMPONENT_OUTPUT_H__
#define NEXUS_COMPONENT_OUTPUT_H__

#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the component video output interface.
**/
typedef struct NEXUS_ComponentOutput *NEXUS_ComponentOutputHandle;

/**
Summary:
Component colorspace options

Description:
For SCART output, set the following:
    NEXUS_ComponentOutputSettings settings;
    settings.type = NEXUS_ComponentOutputType_eRGB;
    settings.dacs.RGB.noSync = true;
**/
typedef enum NEXUS_ComponentOutputType
{
    NEXUS_ComponentOutputType_eRGB,
    NEXUS_ComponentOutputType_eYPrPb
} NEXUS_ComponentOutputType;

/**
Summary:
Settings for Component video output interface

Description:
ComponentOutput also supports VGA output with an optional HSync DAC setting.
**/
typedef struct NEXUS_ComponentOutputSettings
{
    NEXUS_ComponentOutputType type;
    union {
        struct {
            NEXUS_VideoDac dacY;
            NEXUS_VideoDac dacPr;
            NEXUS_VideoDac dacPb;
        } YPrPb;
        struct {
            NEXUS_VideoDac dacGreen;
            NEXUS_VideoDac dacRed;
            NEXUS_VideoDac dacBlue;
            NEXUS_VideoDac dacHSync; /* Optional. If you want to drive a VGA output with an hsync dac, set this to that dac. VSync will be automatically driven by HW if HSync is set.
                                        For a RGB component output, set this to NEXUS_VideoDac_eNone and Hsync will be driven on green.
                                        Set this to NEXUS_VideoDac_eNone and noSync to true if no Hsync on any dac. */
            bool noSync; /* If true, then there will be no Hsync on either green or an Hsync dac. */
        } RGB;
    } dacs;

    bool mpaaDecimationEnabled; /* When MPAA decimation is enabled, a significant number of pixels in the output video
                                   signal are replaced with values interpolated from neighboring pixels.
                                   Therefore, video quality is lowered. */
} NEXUS_ComponentOutputSettings;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
*/
void NEXUS_ComponentOutput_GetDefaultSettings(
    NEXUS_ComponentOutputSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Open a new Component video output interface
**/
NEXUS_ComponentOutputHandle NEXUS_ComponentOutput_Open( /* attr{destructor=NEXUS_ComponentOutput_Close}  */
    unsigned index,
    const NEXUS_ComponentOutputSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Close the ComponentOutput interface
**/
void NEXUS_ComponentOutput_Close(
    NEXUS_ComponentOutputHandle output
    );

/**
Summary:
Get current settings
**/
void NEXUS_ComponentOutput_GetSettings(
    NEXUS_ComponentOutputHandle output,
    NEXUS_ComponentOutputSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Apply new settings
**/
NEXUS_Error NEXUS_ComponentOutput_SetSettings(
    NEXUS_ComponentOutputHandle output,
    const NEXUS_ComponentOutputSettings *pSettings
    );

/**
Summary:
Returns the abstract NEXUS_VideoOutput connector for this Interface.
The NEXUS_VideoOutput connector is added to a Display in order to route that Displa's video to the output.

Description:
Used in NEXUS_Display_AddOutput
**/
NEXUS_VideoOutput NEXUS_ComponentOutput_GetConnector(
    NEXUS_ComponentOutputHandle output
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_COMPONENT_OUTPUT_H__ */
