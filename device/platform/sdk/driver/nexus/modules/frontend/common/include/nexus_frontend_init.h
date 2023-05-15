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
* $brcm_Workfile: nexus_frontend_init.h $
* $brcm_Revision: 5 $
* $brcm_Date: 9/5/12 6:08p $
*
* API Description:
*   API name: Frontend Init
*    Frontend module initialization APIs
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/include/nexus_frontend_init.h $
* 
* 5   9/5/12 6:08p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
* 
* 4   7/14/10 10:33a erickson
* SW7405-4621: replace nexus_base.h with nexus_types.h
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
* Nexus_Devel/1   10/5/07 5:51p jgarrett
* PR 35551: Adding initial version
* 
***************************************************************************/
#ifndef NEXUS_FRONTEND_INIT_H__
#define NEXUS_FRONTEND_INIT_H__

#include "nexus_types.h"
#include "nexus_i2c.h"


#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Frontend Module Settings
***************************************************************************/
typedef struct NEXUS_FrontendModuleSettings
{
    NEXUS_ModuleHandle i2cModule;
    NEXUS_ModuleHandle transport;
} NEXUS_FrontendModuleSettings;

/***************************************************************************
Summary:
    Get Default settings for the frontend module
 ***************************************************************************/
void NEXUS_FrontendModule_GetDefaultSettings(
    NEXUS_FrontendModuleSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Initialize the frontend module
 ***************************************************************************/
NEXUS_ModuleHandle NEXUS_FrontendModule_Init(
    const NEXUS_FrontendModuleSettings *pSettings
    );

/***************************************************************************
Summary:
    Un-Initialize the frontend module
 ***************************************************************************/
void NEXUS_FrontendModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
