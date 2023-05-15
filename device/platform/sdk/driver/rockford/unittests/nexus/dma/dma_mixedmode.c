/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: dma_mixedmode.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 11/16/12 6:10p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_mixedmode.c $
 * 
 * Hydra_Software_Devel/8   11/16/12 6:10p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/7   3/19/12 3:50p mward
 * SW7435-56:  Add 7435 to BMMD_40NM 1.
 * 
 * Hydra_Software_Devel/6   2/1/12 11:44a katrep
 * SW7429-1:add 7429
 * 
 * Hydra_Software_Devel/5   1/16/12 11:08a mphillip
 * SW7125-1116: Add missing NEXUS_Security_GetDefaultClearKey calls
 * 
 * Hydra_Software_Devel/4   5/27/11 12:23p erickson
 * SWDEPRECATED-3044: fix warning
 * 
 * Hydra_Software_Devel/3   4/28/11 3:28p jtna
 * SW7422-420: sanity checks on mem alloc
 * 
 * Hydra_Software_Devel/2   4/28/11 11:59a jtna
 * SW7422-416: update comments
 * 
 * Hydra_Software_Devel/1   4/25/11 5:30p jtna
 * SW7422-416: add dma_mixedmode test
 * 
 *****************************************************************************/

/* this unittest tests mixing clear, crypto and SG crypto modes */

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DMA_BLOCK 4*1024*1024 /* must be large enough for us to append */
#define DMA_JOBS 1
#define NUM_BLOCKS 2

#if NEXUS_HAS_SECURITY
uint8_t keys[16] = {0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12};
#endif
#if BCHP_CHIP==7231 || BCHP_CHIP==7344 || BCHP_CHIP==7346 || BCHP_CHIP==7358 || \
    BCHP_CHIP==7422 || BCHP_CHIP==7425 || BCHP_CHIP==7552 || BCHP_CHIP==7429 || \
	BCHP_CHIP==7435
#define BMMD_40NM 1
#endif

#define WAIT(first, second) \
    while (1) { \
        rc = NEXUS_DmaJob_GetStatus(first##Job, &first##JobStatus); \
        BDBG_ASSERT(!rc); \
        rc = NEXUS_DmaJob_GetStatus(second##Job, &second##JobStatus); \
        BDBG_ASSERT(!rc); \
        if (first##JobStatus.currentState == NEXUS_DmaJobState_eComplete &&  \
            second##JobStatus.currentState == NEXUS_DmaJobState_eComplete) { \
            BKNI_Printf("Complete\n"); \
            break; \
        } \
        else { \
            BKNI_Printf("Waiting... %d, %d\n", first##JobStatus.currentState, second##JobStatus.currentState); \
            BKNI_Sleep(10); \
        } \
    }

int main(int argc, char **argv)
{
#if NEXUS_HAS_SECURITY
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle clearJob, cryptoJob, sgCryptoJob;
    NEXUS_DmaJobBlockSettings clearBlockSettings, cryptoBlockSettings, sgCryptoBlockSettings[2];
    NEXUS_DmaJobStatus clearJobStatus, cryptoJobStatus, sgCryptoJobStatus;
    void *pMem1 = NULL, *pMem2 = NULL, *pMem3 = NULL, *pMem4 = NULL;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;    
    NEXUS_Error rc;
    BKNI_EventHandle event;
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1); BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2); BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem3); BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem4); BDBG_ASSERT(!rc);
    BKNI_CreateEvent(&event);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!keyHandle) {
        fprintf(stderr, "Allocate keyslot failed\n");
        return 1;
    }
    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    NEXUS_Security_ConfigAlgorithm(keyHandle, &algConfig);

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) {
        fprintf(stderr, "Load encryption key failed\n");
        return 1;
    }

    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.busyWait = 0;
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    clearJob = NEXUS_DmaJob_Create(dma, &jobSettings);
    BDBG_ASSERT(clearJob);
    jobSettings.keySlot = keyHandle;
    cryptoJob = NEXUS_DmaJob_Create(dma, &jobSettings);
    BDBG_ASSERT(cryptoJob);
    jobSettings.numBlocks = 2;
    sgCryptoJob = NEXUS_DmaJob_Create(dma, &jobSettings);
    BDBG_ASSERT(cryptoJob);

    NEXUS_DmaJob_GetDefaultBlockSettings(&clearBlockSettings);
    clearBlockSettings.pSrcAddr = pMem1;
    clearBlockSettings.pDestAddr = pMem2;
    clearBlockSettings.blockSize = DMA_BLOCK;

    NEXUS_DmaJob_GetDefaultBlockSettings(&cryptoBlockSettings);
    cryptoBlockSettings.pSrcAddr = pMem1;
    cryptoBlockSettings.pDestAddr = pMem3;
    cryptoBlockSettings.blockSize = DMA_BLOCK;
    cryptoBlockSettings.resetCrypto = true;

    /* to trigger mmscram, a single block with sgStart and sgEnd both true should suffice. 
       however, use two blocks for testing */
    NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[0]);
    sgCryptoBlockSettings[0].pSrcAddr = pMem1;
    sgCryptoBlockSettings[0].pDestAddr = pMem4;
    sgCryptoBlockSettings[0].blockSize = DMA_BLOCK/2;
    sgCryptoBlockSettings[0].resetCrypto = true;
    sgCryptoBlockSettings[0].scatterGatherCryptoStart = true;
    NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[1]);
    sgCryptoBlockSettings[1].pSrcAddr = (uint8_t*)pMem1+DMA_BLOCK/2;
    sgCryptoBlockSettings[1].pDestAddr = (uint8_t*)pMem4+DMA_BLOCK/2;
    sgCryptoBlockSettings[1].blockSize = DMA_BLOCK/2;
    sgCryptoBlockSettings[1].scatterGatherCryptoEnd = true;
    
    BKNI_Printf("clear -> crypto\n");
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(clear, crypto);

    BKNI_Printf("crypto -> clear\n");
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(crypto, clear);

    BKNI_Printf("clear -> sg crypto\n");
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(clear, sgCrypto);

    /* this triggers a throttle on 65nm platforms */
    BKNI_Printf("sg crypto -> clear\n");
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(sgCrypto, clear);

    BKNI_Printf("crypto -> sg crypto\n");
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(crypto, sgCrypto);

    /* this triggers a throttle on 65nm platforms */
    BKNI_Printf("sg crypto -> crypto\n");
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(sgCrypto, crypto);

    NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[0]);
    sgCryptoBlockSettings[0].pSrcAddr = pMem1;
    sgCryptoBlockSettings[0].pDestAddr = pMem4;
    sgCryptoBlockSettings[0].blockSize = DMA_BLOCK/2;
    sgCryptoBlockSettings[0].resetCrypto = true;
    sgCryptoBlockSettings[0].scatterGatherCryptoStart = true;
    sgCryptoBlockSettings[0].scatterGatherCryptoEnd = true;
    NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[1]);
    sgCryptoBlockSettings[1].pSrcAddr = (uint8_t*)pMem1+DMA_BLOCK/2;
    sgCryptoBlockSettings[1].pDestAddr = (uint8_t*)pMem4+DMA_BLOCK/2;
    sgCryptoBlockSettings[1].blockSize = DMA_BLOCK/2;

    BKNI_Printf("clear -> sg crypto, variant 1\n");
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
#if BMMD_40NM
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
#else
    BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);
#endif
    WAIT(clear, sgCrypto);

    BKNI_Printf("sg crypto -> clear, variant 1\n");
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
#if BMMD_40NM
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
#else
    BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);
#endif
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(sgCrypto, clear);
  
    BKNI_Printf("crypto -> sg crypto, variant 1\n");
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
#if BMMD_40NM
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
#else
    BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);
#endif
    WAIT(crypto, sgCrypto);

    BKNI_Printf("sg crypto -> crypto, variant 1\n");
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
#if BMMD_40NM
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
#else
    BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);
#endif
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(sgCrypto, crypto);

    NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[0]);
    sgCryptoBlockSettings[0].pSrcAddr = (uint8_t*)pMem1+DMA_BLOCK/2;
    sgCryptoBlockSettings[0].pDestAddr = (uint8_t*)pMem4+DMA_BLOCK/2;
    sgCryptoBlockSettings[0].blockSize = DMA_BLOCK/2;
    NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[1]);
    sgCryptoBlockSettings[1].pSrcAddr = pMem1;
    sgCryptoBlockSettings[1].pDestAddr = pMem4;
    sgCryptoBlockSettings[1].blockSize = DMA_BLOCK/2;
    sgCryptoBlockSettings[1].resetCrypto = true;
    sgCryptoBlockSettings[1].scatterGatherCryptoStart = true;
    sgCryptoBlockSettings[1].scatterGatherCryptoEnd = true;

    BKNI_Printf("clear -> sg crypto, variant 2\n");
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(clear, sgCrypto);

    /* this triggers a throttle on 65nm platforms */
    BKNI_Printf("sg crypto -> clear, variant 2\n");
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(clearJob, &clearBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(sgCrypto, clear);
  
    BKNI_Printf("crypto -> sg crypto, variant 2\n");
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(crypto, sgCrypto);

    /* this triggers a throttle on 65nm platforms */
    BKNI_Printf("sg crypto -> crypto, variant 2\n");
    rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob, sgCryptoBlockSettings, 2);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob, &cryptoBlockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_DMA_QUEUED);
    WAIT(sgCrypto, crypto);
    
    BKNI_Printf("done\n");

    NEXUS_Memory_Free(pMem1);
    NEXUS_Memory_Free(pMem2);
    NEXUS_Memory_Free(pMem3);
    NEXUS_Memory_Free(pMem4);
    
    NEXUS_DmaJob_Destroy(clearJob);
    NEXUS_DmaJob_Destroy(cryptoJob);
    NEXUS_DmaJob_Destroy(sgCryptoJob);
    NEXUS_Dma_Close(dma);
#else
	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);
#endif /* NEXUS_HAS_SECURITY */
    return 0;
}
