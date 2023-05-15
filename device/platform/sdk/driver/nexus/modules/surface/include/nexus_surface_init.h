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
 * $brcm_Workfile: nexus_surface_init.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface/include/nexus_surface_init.h $
 * 
 * 6   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 5   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 *
 * 4   4/21/10 9:27a erickson
 * SW7420-703: add NEXUS_SurfaceModuleSettings.heapIndex for chips where
 *  graphics cannot be allocated in MEMC0
 *
 * 3   1/23/08 8:37p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:19p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   11/30/07 11:12a erickson
 * PR35457: api update
 *
 * Nexus_Devel/4   11/28/07 2:11p erickson
 * PR35457: doc update
 *
 * Nexus_Devel/3   9/28/07 10:01a erickson
 * PR34419: 0.1 prerel review
 *
 * Nexus_Devel/2   9/26/07 1:41p vsilyaev
 * PR 34662: Added API to return magnum surface handle
 *
 * Nexus_Devel/1   9/21/07 1:20p erickson
 * PR35198: update
 *
 **************************************************************************/
#ifndef NEXUS_SURFACE_INIT_H__
#define NEXUS_SURFACE_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to configure the Surface module.
**/
typedef struct NEXUS_SurfaceModuleSettings
{
    unsigned heapIndex; /* default heap index for allocating surface memory if NEXUS_SurfaceCreateSettings.heap is NULL. */
} NEXUS_SurfaceModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_SurfaceModule_Init
**/
void NEXUS_SurfaceModule_GetDefaultSettings(
    NEXUS_SurfaceModuleSettings *pSettings
    );

/**
Summary:
Initialize the Surface module.

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.surfacePlatformSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_SurfaceModule_Init(
    const NEXUS_SurfaceModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the Surface module.

Description:
Called by NEXUS_Platform_Uninit
**/
void NEXUS_SurfaceModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
