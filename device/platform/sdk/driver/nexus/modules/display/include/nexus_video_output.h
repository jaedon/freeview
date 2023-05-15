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
 * $brcm_Workfile: nexus_video_output.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/22/11 9:03a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_video_output.h $
 * 
 * 5   7/22/11 9:03a erickson
 * SW7425-745: add NEXUS_VideoOutputSettings
 * 
 * 4   7/19/11 11:14a erickson
 * SW7408-291: add NEXUS_VideoOutput_SetVfFilter, requires reverse lookup
 *  of DAC assignments
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
 * Nexus_Devel/6   11/30/07 10:53a erickson
 * PR35457: api update
 * 
 **************************************************************************/
#ifndef NEXUS_VIDEO_OUTPUT_H__
#define NEXUS_VIDEO_OUTPUT_H__

#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
The application must call NEXUS_VideoOutput_Shutdown after the NEXUS_VideoOutput 
has been disconnected from all displays.

Description:
This is needed to free internally cached data.
**/
void NEXUS_VideoOutput_Shutdown(
    NEXUS_VideoOutput output
    );

#define NEXUS_MAX_VF_FILTER_ENTRIES 16

/**
Summary:
Video formatter filter

Description:
There is one VF per channel of each output.
**/
typedef struct NEXUS_VideoOutputVfFilter
{
    uint32_t filterRegs[NEXUS_MAX_VF_FILTER_ENTRIES];
    unsigned numEntries;
} NEXUS_VideoOutputVfFilter;

/**
Summary:
Get current VF filter settings

Description:
You must specify the correct number of VF filter entries for your chip. This can be determined from the chip specification.
**/
void NEXUS_VideoOutput_GetVfFilter(
    NEXUS_VideoOutput output,
    NEXUS_VideoDac dac,
    NEXUS_VideoOutputVfFilter *pFilter /* [out] */
    );

/**
Summary:
Override default VF filter settings

Description:
You must specify the correct number of VF filter entries for your chip. This can be determined from the chip specification.
**/
NEXUS_Error NEXUS_VideoOutput_SetVfFilter(
    NEXUS_VideoOutput output,
    NEXUS_VideoDac dac,
    const NEXUS_VideoOutputVfFilter *pFilter
    );
    
/**
Summary:
Video output settings
**/
typedef struct NEXUS_VideoOutputSettings
{
    bool mute; /* send black by muting the CSC, but do not disable the DAC's. outputs will not lose sync. */
} NEXUS_VideoOutputSettings;

/**
Summary:
Get current video output

Description:
Output must be connected to a display to have an effect.
**/
void NEXUS_VideoOutput_GetSettings(
    NEXUS_VideoOutput output,
    NEXUS_VideoOutputSettings *pSettings
    );
    
/**
Summary:
Set new video output

Description:
Output must be connected to a display to have an effect.
**/
NEXUS_Error NEXUS_VideoOutput_SetSettings(
    NEXUS_VideoOutput output,
    const NEXUS_VideoOutputSettings *pSettings
    );
    
#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_OUTPUT_H__ */
