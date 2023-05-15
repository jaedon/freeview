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
* $brcm_Workfile: nexus_wmdrmpd_module.h $
* $brcm_Revision: 2 $
* $brcm_Date: 11/6/12 1:18p $
*
* API Description:
*   API name: WMDRMPD Module
*
* Revision History:
*
* $brcm_Log: /nexus/modules/wmdrmpd/src/nexus_wmdrmpd_module.h $
* 
* 2   11/6/12 1:18p hugost
* SW7435-473: Merged David's fix into main.
* 
* 1   7/27/12 2:26p jgarrett
* SW7425-3281: Merge to main branch
* 
* SW7425-3281/3   6/22/12 7:27a jgarrett
* SW7425-3281: Switching IO to use heapIndex for kernel-mode
*  compatibility
* 
* SW7425-3281/2   6/21/12 5:05p jgarrett
* SW7425-3281: Initial IO implementation
* 
* SW7425-3281/1   6/20/12 2:35p piyushg
* SW7425-3281: Initial checkin of Nexus directory structure and header
*  files.
*
***************************************************************************/

#ifndef NEXUS_WMDRMPD_MODULE_H_
#define NEXUS_WMDRMPD_MODULE_H_

#include "nexus_base.h"
#include "nexus_wmdrmpd_thunks.h"
#include "nexus_wmdrmpd.h"
#include "nexus_wmdrmpd_init.h"
#include "nexus_wmdrmpd_core.h"
#include "nexus_wmdrmpd_io.h"

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME wmdrmpd
#define NEXUS_MODULE_SELF g_NEXUS_wmDrmPdModule

extern NEXUS_ModuleHandle g_NEXUS_wmDrmPdModule;

typedef struct NEXUS_WmDrmPdModuleData
{
    NEXUS_WmDrmPdModuleSettings settings;
} NEXUS_WmDrmPdModuleData;

extern NEXUS_WmDrmPdModuleData g_NEXUS_wmDrmPdModuleData;

void NEXUS_WmDrmPd_P_ProcessIoRequest(
    const NEXUS_WmDrmPdIoRequest *pRequest, 
    NEXUS_WmDrmPdIoResponse *pResponse /* [out] */
    );

NEXUS_WmDrmPdIoHandle nexus_wmdrmpd_p_create_io(void);
void nexus_wmdrmpd_p_destroy_io(void);

#endif /* #ifndef NEXUS_WMDRMPD_MODULE_H_ */



