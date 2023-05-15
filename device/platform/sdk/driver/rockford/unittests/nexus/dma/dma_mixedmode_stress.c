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
 * $brcm_Workfile: dma_mixedmode_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 11/16/12 6:11p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_mixedmode_stress.c $
 * 
 * Hydra_Software_Devel/7   11/16/12 6:11p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/6   10/29/12 10:25a erickson
 * SW7422-435: shorten output
 * 
 * Hydra_Software_Devel/5   1/16/12 11:08a mphillip
 * SW7125-1116: Add missing NEXUS_Security_GetDefaultClearKey calls
 * 
 * Hydra_Software_Devel/4   6/30/11 7:33p jtna
 * SW7422-435: update
 * 
 * Hydra_Software_Devel/3   6/22/11 5:42p jtna
 * SW7422-435: add more tests and options
 * 
 * Hydra_Software_Devel/2   6/13/11 3:19p jtna
 * SW7422-435: create jobs in better sequence
 * 
 * Hydra_Software_Devel/1   5/13/11 5:57p jtna
 * SW7422-435: add dma_mixedmode_stress test
 * 
 *****************************************************************************/

/* this unittest does a "mixedmode" stress test by mixing different jobs of varying numBlocks */

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

BDBG_MODULE(dma_mixedmode_stress);

#define DMA_BLOCK 4*1024*1024
#define HEAP_SIZE 32*1024*1024
#define MAX_JOBS 32
#define MAX_CHAIN_LENGTH 32

/* 1024*6 loops take 110 seconds on 97405. LONG_TEST would be about 24 hours */
#define LONG_TEST 0
#define LOOPS (1024*6)
#if LONG_TEST
#undef LOOPS
#define LOOPS (1024*6*800)
#endif

uint8_t keys[16] = {0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12};

int main(int argc, char **argv)
{
#if NEXUS_HAS_SECURITY
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle clearJob[MAX_JOBS], cryptoJob[MAX_JOBS], sgCryptoJob[MAX_JOBS];
    NEXUS_DmaJobBlockSettings clearBlockSettings[MAX_CHAIN_LENGTH], cryptoBlockSettings[MAX_CHAIN_LENGTH], sgCryptoBlockSettings[MAX_CHAIN_LENGTH];
    NEXUS_DmaJobStatus jobStatus;
    unsigned clearJobIndex, cryptoJobIndex, sgCryptoJobIndex;
    void *pMem1, *pMem2, *pMem3, *pMem4;
    unsigned i, j, wait;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;    
    NEXUS_Error rc;
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    /* create one big heap */
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1); BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2); BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem3); BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem4); BDBG_ASSERT(!rc);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    BDBG_ASSERT(keyHandle);

    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    NEXUS_Security_ConfigAlgorithm(keyHandle, &algConfig);
    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    rc = NEXUS_Security_LoadClearKey(keyHandle, &key);
    BDBG_ASSERT(!rc);

    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = MAX_CHAIN_LENGTH;
    jobSettings.busyWait = 0;
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    for (i=0; i<MAX_JOBS; i++) {
        clearJob[i] = NEXUS_DmaJob_Create(dma, &jobSettings);
        BDBG_ASSERT(clearJob[i]);
    }
    jobSettings.keySlot = keyHandle;
    for (i=0; i<MAX_JOBS; i++) {
        cryptoJob[i] = NEXUS_DmaJob_Create(dma, &jobSettings);
        BDBG_ASSERT(cryptoJob[i]);
    }
    for (i=0; i<MAX_JOBS; i++) {
        sgCryptoJob[i] = NEXUS_DmaJob_Create(dma, &jobSettings);
        BDBG_ASSERT(sgCryptoJob[i]);
    }

    for (i=0; i<MAX_CHAIN_LENGTH; i++) {
        NEXUS_DmaJob_GetDefaultBlockSettings(&clearBlockSettings[i]);
        clearBlockSettings[i].pSrcAddr = (uint8_t*)pMem1+DMA_BLOCK/MAX_CHAIN_LENGTH*i;
        clearBlockSettings[i].pDestAddr = (uint8_t*)pMem2+DMA_BLOCK/MAX_CHAIN_LENGTH*i;
        clearBlockSettings[i].blockSize = DMA_BLOCK/MAX_CHAIN_LENGTH;
    }

    for (i=0; i<MAX_CHAIN_LENGTH; i++) {
        NEXUS_DmaJob_GetDefaultBlockSettings(&cryptoBlockSettings[i]);
        cryptoBlockSettings[i].pSrcAddr = (uint8_t*)pMem1+DMA_BLOCK/MAX_CHAIN_LENGTH*i;
        cryptoBlockSettings[i].pDestAddr = (uint8_t*)pMem3+DMA_BLOCK/MAX_CHAIN_LENGTH*i;
        cryptoBlockSettings[i].blockSize = DMA_BLOCK/MAX_CHAIN_LENGTH;
    }
    cryptoBlockSettings[0].resetCrypto = true;

    for (i=0; i<MAX_CHAIN_LENGTH; i++) {
        NEXUS_DmaJob_GetDefaultBlockSettings(&sgCryptoBlockSettings[i]);
        sgCryptoBlockSettings[i].pSrcAddr = (uint8_t*)pMem1+DMA_BLOCK/MAX_CHAIN_LENGTH*i;
        sgCryptoBlockSettings[i].pDestAddr = (uint8_t*)pMem4+DMA_BLOCK/MAX_CHAIN_LENGTH*i;
        sgCryptoBlockSettings[i].blockSize = DMA_BLOCK/MAX_CHAIN_LENGTH;
    }
    sgCryptoBlockSettings[0].resetCrypto = true;
    sgCryptoBlockSettings[0].scatterGatherCryptoStart = true;

    clearJobIndex = 0;
    cryptoJobIndex = 0;
    sgCryptoJobIndex = 0;
    for (i=0; i<LOOPS; i++) {
        unsigned select = rand()%3;
        unsigned chain = 1 + (rand()%(MAX_CHAIN_LENGTH-1));

        if (i && (i % 250 == 0)) BDBG_WRN(("loops: %d", i));
        switch (select) {
            case 0:
                BDBG_MSG(("clear   [%2d] %2d", clearJobIndex, chain));
                rc = NEXUS_DmaJob_ProcessBlocks(clearJob[clearJobIndex++], clearBlockSettings, chain);
                break;
            case 1:
                BDBG_MSG(("crypto  [%2d] %2d", cryptoJobIndex, chain));
                rc = NEXUS_DmaJob_ProcessBlocks(cryptoJob[cryptoJobIndex++], cryptoBlockSettings, chain);
                break;
            case 2:
                BDBG_MSG(("sgCrypto[%2d] %2d", sgCryptoJobIndex, chain));
                sgCryptoBlockSettings[chain-1].scatterGatherCryptoEnd = true;
                rc = NEXUS_DmaJob_ProcessBlocks(sgCryptoJob[sgCryptoJobIndex++], sgCryptoBlockSettings, chain);
                sgCryptoBlockSettings[chain-1].scatterGatherCryptoEnd = false;
                break;
        }
        BDBG_ASSERT(rc==NEXUS_SUCCESS||rc==NEXUS_DMA_QUEUED);

        if (clearJobIndex >= MAX_JOBS || cryptoJobIndex >= MAX_JOBS || sgCryptoJobIndex >= MAX_JOBS) {
            BDBG_MSG(("wait..."));
            for (wait=0,j=0; j<MAX_JOBS; j++) {
                NEXUS_DmaJob_GetStatus(clearJob[j], &jobStatus);
                if (jobStatus.currentState != NEXUS_DmaJobState_eComplete) {
                    BDBG_MSG(("waiting clear %d", j));
                    BKNI_Sleep(10);
                    j--;
                    if (wait++==500) { BKNI_Fail(); }
                }
            }
            for (wait=0,j=0; j<MAX_JOBS; j++) {
                NEXUS_DmaJob_GetStatus(cryptoJob[j], &jobStatus);
                if (jobStatus.currentState != NEXUS_DmaJobState_eComplete) {
                    BDBG_MSG(("waiting crypto %d", j));
                    BKNI_Sleep(10);
                    j--;
                    if (wait++==500) { BKNI_Fail(); }
                }
            }
            for (wait=0,j=0; j<MAX_JOBS; j++) {
                NEXUS_DmaJob_GetStatus(sgCryptoJob[j], &jobStatus);
                if (jobStatus.currentState != NEXUS_DmaJobState_eComplete) {
                    BDBG_MSG(("waiting sgCrypto %d", j));
                    BKNI_Sleep(10);
                    j--;
                    if (wait++==500) { BKNI_Fail(); }
                }
            }
            clearJobIndex = 0;
            cryptoJobIndex = 0;
            sgCryptoJobIndex = 0;
        }
    }
    BKNI_Sleep(1000);
    
    NEXUS_Memory_Free(pMem1);
    NEXUS_Memory_Free(pMem2);
    NEXUS_Memory_Free(pMem3);
    NEXUS_Memory_Free(pMem4);

    for (i=0; i<MAX_JOBS; i++) {
        NEXUS_DmaJob_Destroy(clearJob[i]);
        NEXUS_DmaJob_Destroy(cryptoJob[i]);
        NEXUS_DmaJob_Destroy(sgCryptoJob[i]);
    }
    NEXUS_Dma_Close(dma);
    NEXUS_Security_FreeKeySlot(keyHandle);

    NEXUS_Platform_Uninit();
#else
	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);
#endif /* NEXUS_HAS_SECURITY */

    return 0;
}
