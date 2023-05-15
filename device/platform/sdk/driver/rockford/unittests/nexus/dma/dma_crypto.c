/******************************************************************************
 *    (c)2009 Broadcom Corporation
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
 * $brcm_Workfile: dma_crypto.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/16/12 6:10p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_crypto.c $
 * 
 * Hydra_Software_Devel/4   11/16/12 6:10p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/3   1/13/12 6:18p jtna
 * SW7125-1116: change to NEXUS_SecurityAlgorithmVariant_eEcb
 * 
 * Hydra_Software_Devel/2   1/13/12 5:09p cdisc
 * SW7125-1116: using Odd keyslot to reflect changes in nexus_security
 * 
 * Hydra_Software_Devel/1   5/13/11 12:25p jtna
 * SW7400-3026: add dma_crypto unittest
 * 
 *****************************************************************************/

/* simple dma encrypt -> decrypt example that also exercises changing DmaJobSettings.keyslot */

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif
#include <stdio.h>
#include <unistd.h>

#define DMA_BLOCK 32
#define LOOPS 4

int main(void)
{
#if NEXUS_HAS_SECURITY
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobStatus jobStatus;
    NEXUS_DmaJobBlockSettings blockSettings;  
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle encryptKey, decryptKey;
    NEXUS_SecurityAlgorithmSettings algSettings;
    NEXUS_SecurityClearKey key;
    NEXUS_Error rc;
    void *pSrc, *pDst1, *pDst2;
    unsigned int i, j;
    uint8_t keys[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    /* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    /* Open DMA handle */
    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pSrc);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pDst1);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pDst2);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encryptKey = NEXUS_Security_AllocateKeySlot(&keySettings);
    BDBG_ASSERT(encryptKey);

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    decryptKey = NEXUS_Security_AllocateKeySlot(&keySettings);
    BDBG_ASSERT(decryptKey);

    /* Set up encryption key */
    NEXUS_Security_GetDefaultAlgorithmSettings(&algSettings);
    algSettings.algorithm = NEXUS_SecurityAlgorithm_eAes;
    algSettings.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algSettings.operation = NEXUS_SecurityOperation_eEncrypt;
    algSettings.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    rc = NEXUS_Security_ConfigAlgorithm(encryptKey, &algSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    rc = NEXUS_Security_LoadClearKey(encryptKey, &key);
    BDBG_ASSERT(!rc);

    /* Set up decryption key */
    algSettings.operation = NEXUS_SecurityOperation_eDecrypt;
    rc = NEXUS_Security_ConfigAlgorithm(decryptKey, &algSettings);
    BDBG_ASSERT(!rc);
    
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    rc = NEXUS_Security_LoadClearKey(decryptKey, &key);
    BDBG_ASSERT(!rc);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);
    BDBG_ASSERT(dmaJob);

    for (i=0; i<LOOPS; i++) {
        BKNI_Printf("loop %d\n", i);
        NEXUS_DmaJob_GetSettings(dmaJob, &jobSettings);
        jobSettings.dataFormat = (i%2)?NEXUS_DmaDataFormat_eBlock:NEXUS_DmaDataFormat_eMpeg;
        rc = NEXUS_DmaJob_SetSettings(dmaJob, &jobSettings);
        BDBG_ASSERT(!rc);

        /* encrypt */
        BKNI_Memset(pSrc, 0, DMA_BLOCK);

        NEXUS_DmaJob_GetSettings(dmaJob, &jobSettings);
        jobSettings.keySlot = encryptKey;
        rc = NEXUS_DmaJob_SetSettings(dmaJob, &jobSettings);
        BDBG_ASSERT(!rc);

        NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
        blockSettings.pSrcAddr = pSrc;
        blockSettings.pDestAddr = pDst1; 
        blockSettings.blockSize = DMA_BLOCK;
        blockSettings.resetCrypto = true;
        blockSettings.scatterGatherCryptoStart = true;
        blockSettings.scatterGatherCryptoEnd = true;
        blockSettings.cached = true;
        NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

        while (1) {
            NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
            if (jobStatus.currentState == NEXUS_DmaJobState_eComplete) {
                break;
            }
            BKNI_Delay(1);
        }

        /* decrypt */
        BKNI_Memset(pDst2, 0xAA, DMA_BLOCK); /* fill it with garbage */

        NEXUS_DmaJob_GetSettings(dmaJob, &jobSettings);
        jobSettings.keySlot = decryptKey;
        rc = NEXUS_DmaJob_SetSettings(dmaJob, &jobSettings);
        BDBG_ASSERT(!rc);

        NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
        blockSettings.pSrcAddr = pDst1;
        blockSettings.pDestAddr = pDst2; 
        blockSettings.blockSize = DMA_BLOCK;
        blockSettings.resetCrypto = true;
        blockSettings.scatterGatherCryptoStart = true;
        blockSettings.scatterGatherCryptoEnd = true;
        blockSettings.cached = true;
        NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

        while (1) {
            NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
            if (jobStatus.currentState == NEXUS_DmaJobState_eComplete) {
                break;
            }
            BKNI_Delay(1);
        }

#if 1
        BKNI_Printf("SRC:  ");
        for (j=0; j<DMA_BLOCK; j++) {
            BKNI_Printf("%02x ", *((uint8_t*)pSrc+j));
        }
        BKNI_Printf("\n");

        BKNI_Printf("DST1: ");
        for (j=0; j<DMA_BLOCK; j++) {
            BKNI_Printf("%02x ", *((uint8_t*)pDst1+j));
        }
        BKNI_Printf("\n");

        BKNI_Printf("DST2: ");
        for (j=0; j<DMA_BLOCK; j++) {
            BKNI_Printf("%02x ", *((uint8_t*)pDst2+j));
        }
        BKNI_Printf("\n");
#endif

        /* compare pSrc against pDst2 */
        for (j=0;j<DMA_BLOCK; j++) {
            if (*((uint8_t*)pSrc+j) != *((uint8_t*)pDst2+j)) {
                BKNI_Printf("Comparison failed at location %d\n", j);
                break;
            }
        }

        if (j==DMA_BLOCK) {
            BKNI_Printf("Comparison pass\n");
        }
    }

    printf("done\n");

    NEXUS_Security_FreeKeySlot(encryptKey);
    NEXUS_Security_FreeKeySlot(decryptKey);
    NEXUS_DmaJob_Destroy(dmaJob);
    NEXUS_Dma_Close(dma);
    NEXUS_Memory_Free(pSrc);
    NEXUS_Memory_Free(pDst1);
    NEXUS_Memory_Free(pDst2);
    NEXUS_Platform_Uninit();
#endif /* NEXUS_HAS_SECURITY */
    return 0;
}
