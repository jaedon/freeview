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
 * $brcm_Workfile: nexus_security_init.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/include/nexus_security_init.h $
 * 
 * 6   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 5   12/22/10 5:48p mphillip
 * SW7422-104: Merge 40nm support to /main
 * 
 * SW7422-104/1   12/16/10 6:14p mphillip
 * SW7422-104: Initial support for 7422
 * 
 * 4   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 * 
 * 3   1/18/10 7:25p mphillip
 * SW7550-199: Add customer modes to security module initialization (for
 *  select chips only)
 * 
 * 2   4/27/09 1:30p erickson
 * PR53745: added NEXUS_SecurityModuleSettings.numKeySlotsForType[]
 * 
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/3   11/30/07 11:13a erickson
 * PR35457: api update
 * 
 * Nexus_Devel/2   11/28/07 2:11p erickson
 * PR35457: doc update
 * 
 * Nexus_Devel/1   11/15/07 3:54p erickson
 * PR37137: added Security module
 * 
 **************************************************************************/
#ifndef NEXUS_SECURITY_INIT_H__
#define NEXUS_SECURITY_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_SECURITY_MAX_KEYSLOT_TYPES 8

/**
Summary:
Settings used to configure customer mode for the Security module.

Description:
This enum describes modes available for the security system.  Not all modes are supported on all chips, nor do all chips require this to be set.

See Also:
NEXUS_SecurityModule_GetDefaultSettings
NEXUS_SecurityModule_Init
**/
typedef enum NEXUS_SecurityCustomerMode {
    NEXUS_SecurityCustomerMode_eGeneric,
    NEXUS_SecurityCustomerMode_eDvs042,
    NEXUS_SecurityCustomerMode_eDesCts,
    NEXUS_SecurityCustomerMode_eDvbCsa
} NEXUS_SecurityCustomerMode; 

/**
Summary:
Settings used to configure the Security module.

Description:

See Also:
NEXUS_SecurityModule_GetDefaultSettings
NEXUS_SecurityModule_Init
**/
typedef struct NEXUS_SecurityModuleSettings 
{
    NEXUS_SecurityCustomerMode customerMode;
    unsigned int numKeySlotsForType[NEXUS_SECURITY_MAX_KEYSLOT_TYPES];
    bool enableMulti2Key;
} NEXUS_SecurityModuleSettings;

/**
Summary:
Get default settings for the Security module initialization.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.

See Also:
NEXUS_SecurityModule_Init
**/
void NEXUS_SecurityModule_GetDefaultSettings(
    NEXUS_SecurityModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the Security module.

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.securitySettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

The keyslot table can only be initialized once per power cycle of the chip, and on some chips the values of a previous initialization can be read back.  NEXUS_SecurityModule_GetCurrentSettings can read that information.
**/    
NEXUS_ModuleHandle NEXUS_SecurityModule_Init(
    const NEXUS_SecurityModuleSettings *pSettings
    );

/**
Summary:
Retrieve the current Security module settings.

Description:
This function retrieves the current Security module settings.

If the application has modified pSettings->numKeySlotsForType, or wishes to read the number of keyslots allocated per type, this returns the information.

See Also:
NEXUS_SecurityModule_Init
**/     
void NEXUS_SecurityModule_GetCurrentSettings(
    NEXUS_ModuleHandle module,
    NEXUS_SecurityModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the Security module.

Description:
Called by NEXUS_Platform_Uninit
**/     
void NEXUS_SecurityModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
