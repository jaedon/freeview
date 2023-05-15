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
 * $brcm_Workfile: nexus_smartcard_init.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/smartcard/include/nexus_smartcard_init.h $
 * 
 * 5   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 4   5/22/09 4:50p erickson
 * PR54381: add settings
 *
 * 3   4/21/09 11:32a erickson
 * PR54381: added NEXUS_SmartcardModuleSettings.clockFrequency
 *
 * 2   4/20/09 2:57p erickson
 * PR54381: added NEXUS_SmartcardClockSource
 *
 * 1   5/19/08 12:03p vishk
 * PR 40034: Develop Nexus Smartcard Interface/Module.
 *
 *
 **************************************************************************/
#ifndef NEXUS_SMARTCARD_INIT_H__
#define NEXUS_SMARTCARD_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
Smartcard clock source
*/
typedef enum NEXUS_SmartcardClockSource
{
    NEXUS_SmartcardClockSource_eUnknown = 0,    /* Unknown value */
    NEXUS_SmartcardClockSource_eInternalClock,  /* Smartcard uses internal clock */
    NEXUS_SmartcardClockSource_eExternalClock,  /* Smartcard uses external clock */
    NEXUS_SmartcardClockSource_eMax
} NEXUS_SmartcardClockSource;

/**
Summary:
Settings used to configure the Smartcard module.

Description:

See Also:
NEXUS_SmartcardModule_GetDefaultSettings
NEXUS_SmartcardModule_Init
**/
typedef struct NEXUS_SmartcardModuleSettings
{
    NEXUS_SmartcardClockSource clockSource;
    unsigned long clockFrequency;
    NEXUS_ModuleHandle audio; /* needed for smartcard implementations that use an audio PLL */
    bool externalOscillatior; /* Set to true if the external oscillator is used. */
    bool routedInternal; /* Set to true if the clock is routed internally within the chip. */
} NEXUS_SmartcardModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_SmartcardModule_Init
**/
void NEXUS_SmartcardModule_GetDefaultSettings(
    NEXUS_SmartcardModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the Smartcard module.

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.smartCardSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_SmartcardModule_Init(
    const NEXUS_SmartcardModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the Smartcard module.

Description:
Called by NEXUS_Platform_Uninit
**/
void NEXUS_SmartcardModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
