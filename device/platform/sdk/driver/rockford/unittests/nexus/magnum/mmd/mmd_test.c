/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: mmd_test.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/30/11 5:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/mmd/mmd_test.c $
 * 
 * Hydra_Software_Devel/1   8/30/11 5:46p jtna
 * SW7405-5473: added mmd magnum/nexus hybrid unittest
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "./priv/nexus_core.h" /* needed for g_pCoreHandles */
#include "bmem.h"
#include "bmmd.h"
#include <stdio.h>

BDBG_MODULE(mmd_test);
#define BLOCKSIZE 4096

static void callback_isr(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    BKNI_SetEvent_isr(pParam);
}

int main(int argc, char **argv)
{
    BMEM_Heap_Handle hMem;
    BMMD_Settings settings;
    BMMD_Handle mmd;
    BMMD_ContextHandle clearContext, cryptoContext;
    BMMD_ContextSettings clearContextSettings, cryptoContextSettings;
    BMMD_ContextBlockSettings clearBlockSettings, cryptoBlockSettings[2];
    void *pMem1 = NULL, *pMem2 = NULL, *pMem3 = NULL;
    uint32_t offset1, offset2, offset3;
    BKNI_EventHandle clearEvent, cryptoEvent;
    BERR_Code rc, rc2;

    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_Init(NULL);
    hMem = g_pCoreHandles->heap[0];

    rc = BDBG_SetModuleLevel("bmmd", BDBG_eMsg);
    BDBG_ASSERT(!rc);
    
    /* allocate some memory from heap */
    pMem1 = BMEM_Heap_Alloc(hMem, BLOCKSIZE);
    pMem2 = BMEM_Heap_Alloc(hMem, BLOCKSIZE);
    pMem3 = BMEM_Heap_Alloc(hMem, BLOCKSIZE);
    BMEM_Heap_ConvertAddressToOffset(hMem, pMem1, &offset1);
    BMEM_Heap_ConvertAddressToOffset(hMem, pMem2, &offset2);
    BMEM_Heap_ConvertAddressToOffset(hMem, pMem3, &offset3);
    BKNI_Printf("offsets %x %x %x\n", offset1, offset2, offset3);

    BKNI_Memset(pMem1, 1, BLOCKSIZE);
    BKNI_Memset(pMem2, 2, BLOCKSIZE);
    BKNI_Memset(pMem3, 3, BLOCKSIZE);
    
    /* create events */
    BKNI_CreateEvent(&clearEvent);
    BKNI_CreateEvent(&cryptoEvent);
    
    /* open MMD */
    BMMD_GetDefaultSettings(&settings);
#if 0
    settings.engineType = BMMD_EngineType_eSharf;
    settings.engineIndex = 1;
#endif
    BMMD_Open(g_pCoreHandles->chp, g_pCoreHandles->reg, hMem, g_pCoreHandles->bint, &settings, &mmd);
    
    /* setup clear context */   
    BMMD_Context_GetDefaultSettings(&clearContextSettings);
    clearContextSettings.maxNumBlocks = 1;
    clearContextSettings.scramMode = BMMD_ScramMode_eNone;
    clearContextSettings.callback_isr = callback_isr;
    clearContextSettings.pParm1 = clearEvent;
    clearContext = BMMD_Context_Create(mmd, &clearContextSettings);
    
    /* setup crypto context. encryption using scatter-gather */
    BMMD_Context_GetDefaultSettings(&cryptoContextSettings);
    cryptoContextSettings.maxNumBlocks = 2;
    cryptoContextSettings.scramMode = BMMD_ScramMode_eBlock;
    cryptoContextSettings.keyslot = 0;
    cryptoContextSettings.callback_isr = callback_isr;
    cryptoContextSettings.pParm1 = cryptoEvent;
    cryptoContext = BMMD_Context_Create(mmd, &cryptoContextSettings);
    
    /* set block settings for clear context */
    BMMD_Context_GetDefaultBlockSettings(&clearBlockSettings);
    clearBlockSettings.src = offset1;
    clearBlockSettings.dst = offset2;
    clearBlockSettings.size = BLOCKSIZE;
    
    /* set block settings for crypto context */
    BMMD_Context_GetDefaultBlockSettings(&cryptoBlockSettings[0]);
    cryptoBlockSettings[0].src = offset2;
    cryptoBlockSettings[0].dst = offset3;
    cryptoBlockSettings[0].size = 16; /* first 16 bytes */
    cryptoBlockSettings[0].resetCrypto = true;
    cryptoBlockSettings[0].sgScramStart = true;
    cryptoBlockSettings[0].sgScramEnd = false;
    
    BMMD_Context_GetDefaultBlockSettings(&cryptoBlockSettings[1]);
    cryptoBlockSettings[1].src = offset2+16;
    cryptoBlockSettings[1].dst = offset3+16;
    cryptoBlockSettings[1].size = BLOCKSIZE-16; /* remaining bytes */
    cryptoBlockSettings[1].resetCrypto = false;
    cryptoBlockSettings[1].sgScramStart = false;
    cryptoBlockSettings[1].sgScramEnd = true;
    
    /* enqueue both contexts */
    rc = BMMD_Context_Enqueue(clearContext, &clearBlockSettings, 1);
    rc2 = BMMD_Context_Enqueue(cryptoContext, cryptoBlockSettings, 2);
    
    /* wait for both transfers to finish */
    if (rc==BMMD_QUEUED) {
        BKNI_WaitForEvent(clearEvent, BKNI_INFINITE);
    }
    if (rc2==BMMD_QUEUED) {
        BKNI_WaitForEvent(cryptoEvent, BKNI_INFINITE);
    }
    
    /* shutdown */
    BMMD_Context_Destroy(clearContext);
    BMMD_Context_Destroy(cryptoContext);
    BMMD_Close(mmd);

    BKNI_DestroyEvent(clearEvent);
    BKNI_DestroyEvent(cryptoEvent);

    BMEM_Heap_Free(hMem, pMem1);
    BMEM_Heap_Free(hMem, pMem2);
    BMEM_Heap_Free(hMem, pMem3);

    NEXUS_Platform_Uninit();
  
    return 0;
}

