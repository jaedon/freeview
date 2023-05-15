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
 * $brcm_Workfile: nexus_uart_init.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/14/10 10:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/uart/7400/include/nexus_uart_init.h $
 * 
 * 2   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
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
 * Nexus_Devel/1   11/20/07 1:28p erickson
 * PR37423: added uart, gpio, spi modules
 * 
 **************************************************************************/
#ifndef NEXUS_UART_INIT_H__
#define NEXUS_UART_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to configure the Uart module.

Description:

See Also:
NEXUS_UartModule_GetDefaultSettings
NEXUS_UartModule_Init
**/
typedef struct NEXUS_UartModuleSettings 
{
    int dummy; /* There are no members at this time. */
} NEXUS_UartModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_UartModule_Init
**/
void NEXUS_UartModule_GetDefaultSettings(
    NEXUS_UartModuleSettings *pSettings /* [out] */
    );
    
/**
Summary:
Initialize the Uart module.

Description:
This is called by the NEXUS Platform when the system is initialized.

See Also:
NEXUS_UartModule_Uninit
NEXUS_Uart_Open - open Interface for Uart
**/    
NEXUS_ModuleHandle NEXUS_UartModule_Init(
    const NEXUS_UartModuleSettings *pSettings
    );
    
/**
Summary:
Uninitialize the Uart module.

Description:
This is called by the NEXUS Platform when the system is uninitialized.

See Also:
NEXUS_UartModule_Init
**/     
void NEXUS_UartModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
