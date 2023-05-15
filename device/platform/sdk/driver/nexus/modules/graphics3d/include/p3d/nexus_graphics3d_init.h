/***************************************************************************
 *     (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_graphics3d_init.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/17/12 8:02a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/include/p3d/nexus_graphics3d_init.h $
 * 
 * 1   7/17/12 8:02a erickson
 * SW7435-235: refactor directories to remove symbolic links
 * 
 * 2   3/9/12 1:37p erickson
 * SW7425-1969: nexus_base.h is not in public api
 * 
 * 1   2/8/11 10:52a erickson
 * SWVC4-118: merge
 * 
 * hauxwell_35230_v3d/1   12/6/10 11:51a gsweet
 * SWVC4-101: Allow abstract platform support for V3D driver.
 * 
 * 1   7/10/08 4:00p vsilyaev
 * PR 40869: Graphics 3D API
 * 
 * 
 **************************************************************************/
#ifndef NEXUS_GRAPHICS3D_INIT_H__
#define NEXUS_GRAPHICS3D_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to configure the Graphics3d module.

Description:

See Also:
NEXUS_Graphics3dModule_GetDefaultSettings
NEXUS_Graphics3dModule_Init
**/
typedef struct NEXUS_Graphics3dModuleSettings 
{
    int dummy; /* There are no members at this time. */
} NEXUS_Graphics3dModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_Graphics3dModule_Init
**/
void NEXUS_Graphics3dModule_GetDefaultSettings(
    NEXUS_Graphics3dModuleSettings *pSettings /* [out] */
    );
    
/**
Summary:
Initialize the Graphics3d module.

Description:
This is called by the NEXUS Platform when the system is initialized.

See Also:
NEXUS_Graphics3dModule_Uninit
NEXUS_Graphics3d_Open - open Interface for Graphics3d
**/    
NEXUS_ModuleHandle NEXUS_Graphics3dModule_Init(
    const NEXUS_Graphics3dModuleSettings *pSettings
    );
    
/**
Summary:
Uninitialize the Graphics3d module.

Description:
This is called by the NEXUS Platform when the system is uninitialized.

See Also:
NEXUS_Graphics3dModule_Init
**/     
void NEXUS_Graphics3dModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
