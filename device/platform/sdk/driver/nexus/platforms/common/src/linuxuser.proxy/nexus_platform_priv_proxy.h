/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_priv_proxy.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/4/12 4:03p $
*
* API Description:
*   API name: Platform (private)
*    Private APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser.proxy/nexus_platform_priv_proxy.h $
* 
* 3   9/4/12 4:03p erickson
* SW7420-1123: refactor common, local code for linuxuser.client and
*  linuxuser.proxy
* 
* 2   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
* 
* 1   2/7/08 11:18a vsilyaev
* PR 38682: Common interface for proxy platform
* 
* 
***************************************************************************/
#ifndef NEXUS_PLATFORM_PRIV_PROXY_H__
#define NEXUS_PLATFORM_PRIV_PROXY_H__

#include "nexus_platform.h"
#include "nexus_core_init.h"

/* nexus_platform_local.c */
NEXUS_Error NEXUS_Platform_P_Magnum_Init(void);
void NEXUS_Platform_P_Magnum_Uninit(void);
NEXUS_Error NEXUS_Platform_P_ModulesInit(const NEXUS_PlatformSettings *pSettings);
void NEXUS_Platform_P_ModulesUninit(void);

#endif /* #ifndef NEXUS_PLATFORM_PRIV_PROXY_H__ */

