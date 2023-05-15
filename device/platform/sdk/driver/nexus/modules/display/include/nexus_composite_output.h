/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
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
 * $brcm_Workfile: nexus_composite_output.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 2/6/08 12:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_composite_output.h $
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
 * Nexus_Devel/8   11/30/07 10:52a erickson
 * PR35457: api update
 * 
 * Nexus_Devel/7   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 * 
 * Nexus_Devel/6   10/4/07 1:09p vsilyaev
 * PR 34662: Removed VideoOutputDesc
 * 
 * Nexus_Devel/5   9/25/07 1:02p vsilyaev
 * PR 34662: Fixed file names
 * 
 * Nexus_Devel/4   9/17/07 6:48p vsilyaev
 * PR 34662: Coded more implementation
 * 
 * Nexus_Devel/3   9/12/07 1:52p vsilyaev
 * PR 34419: Updated display API
 * 
 * Nexus_Devel/2   9/10/07 4:35p vsilyaev
 * PR 34662: Added nexus_display_types.h
 * 
 * Nexus_Devel/1   9/10/07 2:42p vsilyaev
 * PR 34419: Video display interface
 * 
 * 
 **************************************************************************/
#ifndef NEXUS_COMPOSITE_OUTPUT_H__
#define NEXUS_COMPOSITE_OUTPUT_H__

#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the composite video output interface.
**/
typedef struct NEXUS_CompositeOutput *NEXUS_CompositeOutputHandle;

/**
Summary:
Settings for Composite video output interface
**/
typedef struct NEXUS_CompositeOutputSettings
{
    NEXUS_VideoDac dac;
} NEXUS_CompositeOutputSettings;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
*/
void NEXUS_CompositeOutput_GetDefaultSettings(
    NEXUS_CompositeOutputSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Open a new Composite video output interface
**/
NEXUS_CompositeOutputHandle NEXUS_CompositeOutput_Open( /* attr{destructor=NEXUS_CompositeOutput_Close}  */
    unsigned index,
    const NEXUS_CompositeOutputSettings *pSettings    /* settings */
    );

/**
Summary:
Close the CompositeOutput interface
**/
void NEXUS_CompositeOutput_Close(
    NEXUS_CompositeOutputHandle output
    );

/**
Summary:
Get current settings
**/
void NEXUS_CompositeOutput_GetSettings(
    NEXUS_CompositeOutputHandle output,  
    NEXUS_CompositeOutputSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Apply new settings
**/
NEXUS_Error NEXUS_CompositeOutput_SetSettings(
    NEXUS_CompositeOutputHandle output,  
    const NEXUS_CompositeOutputSettings *pSettings    
    );

/**
Summary:
Returns the abstract NEXUS_VideoOutput connector for this Interface. 
The NEXUS_VideoOutput connector is added to a Display in order to route that Displa's video to the output.

Description:
Used in NEXUS_Display_AddOutput
**/
NEXUS_VideoOutput NEXUS_CompositeOutput_GetConnector(
    NEXUS_CompositeOutputHandle composite
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_COMPOSITE_OUTPUT_H__ */
