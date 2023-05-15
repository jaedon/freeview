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
 * $brcm_Workfile: nexus_led_init.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/28/12 8:45a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/led/include/nexus_led_init.h $
 * 
 * 5   9/28/12 8:45a erickson
 * SW7435-344: convert spi/gpio handle to index in module settings. only
 *  open spi/gpio when led/keypad is actually opened.
 * 
 * 4   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 * 
 * 3   11/18/09 4:04p erickson
 * SW7408-1: merge. allow systems without SPI.
 * 
 * SW7408-1/1   11/18/09 2:54p gmohile
 * SW7408-1 : Use NEXUS_HAS_SPI for spi frontpanel
 * 
 * 2   4/9/09 11:39a jrubio
 * PR52188: add support for 2nd Receiver
 * 
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/1   11/20/07 4:53p erickson
 * PR37423: adding ir_input and led
 * 
 **************************************************************************/
#ifndef NEXUS_LED_INIT_H__
#define NEXUS_LED_INIT_H__

#include "nexus_types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_LedModuleSettings {
    struct {
        bool valid;
        unsigned index;
    } spi;    
} NEXUS_LedModuleSettings;

void NEXUS_LedModule_GetDefaultSettings(
    NEXUS_LedModuleSettings *pSettings /* [out] */
    );
    
NEXUS_ModuleHandle NEXUS_LedModule_Init(
    const NEXUS_LedModuleSettings *pSettings
    );
    
void NEXUS_LedModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
