/***************************************************************************
*     (c)2010-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_base_os_priv.h $
* $brcm_Revision: 5 $
* $brcm_Date: 9/4/12 1:11p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/base/src/nexus_base_os_priv.h $
* 
* 5   9/4/12 1:11p vsilyaev
* SW7420-1004: Removed NEXUS_GetLastError
* 
* 4   8/3/12 3:50p vsilyaev
* SW7420-2330: Added control to module and callback debug features
* 
* 3   7/31/12 1:35p vsilyaev
* SW7420-2328: Fixed spelling of the function name
* 
* 2   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
* 
* 1   9/24/10 10:32a erickson
* SW7420-943: add
*
***************************************************************************/
#ifndef NEXUS_BASE_OS_PRIV_H__
#define NEXUS_BASE_OS_PRIV_H__

#include "blifo.h"
#include "blst_aa_tree.h"

#define NEXUS_P_BASE_MAX_LOCKS  (64)
typedef struct NEXUS_P_LockEntry {
    NEXUS_ModuleHandle module;
    const char *pFileName;
    unsigned lineNumber;
} NEXUS_P_LockEntry;

struct b_objdb_client;
typedef struct NEXUS_P_ThreadInfo {
    BLST_AA_TREE_ENTRY(NEXUS_P_ThreadInfoTree) node;
    void *threadId; /* address used as a key to store/retrieve the thread specific information */
    NEXUS_ThreadHandle nexusThread; /* not NULL for threads created using NEXUS_Thread_Create */
    const char *pThreadName;
    struct b_objdb_client *client;
#if NEXUS_P_DEBUG_MODULE_LOCKS
    BLIFO_HEAD(NEXUS_P_LockStack, NEXUS_P_LockEntry) stack;
    NEXUS_P_LockEntry locks[NEXUS_P_BASE_MAX_LOCKS];
#endif
} NEXUS_P_ThreadInfo;

/* NEXUS_P_ThreadInfo_Init is implemented generically */
void NEXUS_P_ThreadInfo_Init(NEXUS_P_ThreadInfo *info);

/* NEXUS_P_ThreadInfo_Get is implemented per OS */
NEXUS_P_ThreadInfo *NEXUS_P_ThreadInfo_Get(void);

/* following functions could be used by OS layer to help implement NEXUS_P_ThreadInfo_Get */
void NEXUS_Base_P_Thread_AssociateInfo(NEXUS_ThreadHandle thread, void *threadId, NEXUS_P_ThreadInfo *threadInfo); /* associate thread info with the thread */
void NEXUS_Base_P_Thread_DisassociateInfo(NEXUS_ThreadHandle thread, NEXUS_P_ThreadInfo *threadInfo); /* disassociate thread info from the thread */
NEXUS_P_ThreadInfo *NEXUS_Base_P_Thread_GetInfo(void *threadId); /* return thread info associated with the threadId, for nexus threads it would return pointer set by NEXUS_Base_P_Thread_AssociateInfo, and for foreign threads would return pointer from internal store */

/* following two functions could be used by OS layer if it uses native thread local storage */
NEXUS_P_ThreadInfo *NEXUS_Base_P_AllocateThreadInfo(void *threadId);  /* this function would return NEXUS_P_ThreadInfo allocated from the internal store */
void NEXUS_Base_P_TickThreadInfo(NEXUS_P_ThreadInfo *threadInfo); /* this function would mark internally allocated  threadInfo as actively used */

#endif
