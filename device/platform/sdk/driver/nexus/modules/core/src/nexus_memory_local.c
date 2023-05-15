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
* $brcm_Workfile: nexus_memory_local.c $
* $brcm_Revision: 5 $
* $brcm_Date: 7/9/12 4:43p $
*
* API Description:
*   API name: Memory
*    Specific APIs related to device memory allocation
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/src/nexus_memory_local.c $
* 
* 5   7/9/12 4:43p vsilyaev
* SW7425-3418: xxx_local.c files can't include xxx_module.h file
* 
* 4   7/9/12 11:30a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 3   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 2   1/19/12 10:31a erickson
* SW7425-2218: add NEXUS_AVS_EXTENSION. fix core thunk.
* 
* 1   2/3/11 6:33p vsilyaev
* SW7420-1441: Use dedicated file for 'local' memory API
*
***************************************************************************/
/* can't include nexus_core_modules.h */
#include "nexus_base.h"
#include "nexus_memory.h"
#include "nexus_core_utils.h"

BDBG_MODULE(nexus_memory);

void NEXUS_Memory_FlushCache( void *pMemory, size_t numBytes )
{
    BDBG_ASSERT(NULL != pMemory);
    NEXUS_FlushCache(pMemory, numBytes);
}

NEXUS_Error NEXUS_Memory_Allocate( size_t numBytes, const NEXUS_MemoryAllocationSettings *pSettings, void **ppMemory )
{
    NEXUS_MemoryBlockHandle block;
    NEXUS_MemoryAllocationSettings defaultSettings;

    if ( NULL == pSettings )
    {
        NEXUS_Memory_GetDefaultAllocationSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    block = NEXUS_MemoryBlock_Allocate(pSettings->heap, numBytes, pSettings->alignment, NULL);
    if(block==NULL) {return BERR_TRACE(NEXUS_NOT_AVAILABLE);}
    return NEXUS_MemoryBlock_Lock(block, ppMemory);
}

void NEXUS_Memory_Free( void *pMemory )
{
    NEXUS_MemoryBlockHandle block;
    block = NEXUS_MemoryBlock_FromAddress(pMemory);
    if(block) {
        NEXUS_MemoryBlock_Unlock(block);
        NEXUS_MemoryBlock_Free(block);
    }
    return;
}

void NEXUS_StopCallbacks(void *interfaceHandle)
{
    NEXUS_Platform_P_StopCallbacks(interfaceHandle);
    return;
}

void NEXUS_StartCallbacks(void *interfaceHandle)
{
    NEXUS_Platform_P_StartCallbacks(interfaceHandle);
    return;
}
