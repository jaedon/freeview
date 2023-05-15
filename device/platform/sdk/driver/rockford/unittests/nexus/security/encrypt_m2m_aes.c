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
 * $brcm_Workfile: encrypt_m2m_aes.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/16/12 6:12p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/security/encrypt_m2m_aes.c $
 * 
 * Hydra_Software_Devel/4   11/16/12 6:12p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/3   3/20/12 3:24p mphillip
 * SW7405-4668: Add NEXUS_Security_GetDefaultClearKey calls before using
 * the clear key
 * 
 * Hydra_Software_Devel/2   3/20/12 2:43p mphillip
 * SW7405-4668: Return non-zero on failure
 * 
 * Hydra_Software_Devel/1   10/12/10 6:42p mphillip
 * SW7405-4668: Add m2m encryption/decryption unit tests
 * 
 * 
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DMA_BLOCK   1*1024*1024

/* AES KAT (Known Answer Test) built on the sample from FIPS 197 */
uint8_t aesEcbIn[16] = {0x50, 0x68, 0x12, 0xA4, 0x5F, 0x08, 0xC8, 0x89, 0xB9, 0x7F, 0x59, 0x80, 0x03, 0x8B, 0x83, 0x59};
uint8_t aesEcbKnownAnswer[16] = {0xD8, 0xF5, 0x32, 0x53, 0x82, 0x89, 0xEF, 0x7D, 0x06, 0xB5, 0x06, 0xA4, 0xFD, 0x5B, 0xE9, 0xC9};
uint8_t aesEcbKey[16] = {0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12};

#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif
int main(int argc, char **argv)
{
#if NEXUS_HAS_SECURITY
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
    NEXUS_DmaJobStatus jobStatus;
    void *pMem1=NULL, *pMem2=NULL;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    NEXUS_KeySlotHandle encKeyHandle;
    NEXUS_KeySlotHandle decKeyHandle;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;

    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!encKeyHandle || !decKeyHandle) {
        printf("\nAllocate keyslot failed \n");
        return 1;
    }

    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2);
    memset(pMem1, 0x01, DMA_BLOCK);
    memset(pMem2, 0x02, DMA_BLOCK);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.busyWait = 100;

    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keySize = 16;
    algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    NEXUS_Security_ConfigAlgorithm(encKeyHandle, &algConfig);
    /* Load AV keys */
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    BKNI_Memcpy(key.keyData, aesEcbKey, sizeof(aesEcbKey));
    if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }
    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
    NEXUS_Security_ConfigAlgorithm(decKeyHandle, &algConfig);
    if (NEXUS_Security_LoadClearKey(decKeyHandle, &key) != 0) {
        printf("\nLoad decryption key failed \n");
        return 1;
    }
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    jobSettings.keySlot = encKeyHandle;

    memset(pMem1, 0x01, DMA_BLOCK);
    memcpy(pMem1, aesEcbIn, sizeof(aesEcbIn));
    memset(pMem2, 0x02, DMA_BLOCK);

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.cached = true;
    blockSettings.resetCrypto = true;              
    blockSettings.scatterGatherCryptoStart = true; 
    blockSettings.scatterGatherCryptoEnd = true;   

    blockSettings.pSrcAddr = pMem1;
    blockSettings.pDestAddr = pMem2;
    blockSettings.blockSize = DMA_BLOCK;
    rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
    if(rc!=NEXUS_SUCCESS) { 
        for(;;) {
            NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
            if(jobStatus.currentState == NEXUS_DmaJobState_eComplete ) {
                break;
            }
        } 
    }
    if (memcmp(pMem2,aesEcbKnownAnswer,sizeof(aesEcbKnownAnswer))) {
        fprintf(stderr, "Encryption failed to match expected answer\n");
        return 1;
    }
    NEXUS_DmaJob_Destroy(dmaJob);

    jobSettings.keySlot = decKeyHandle;

    memset(pMem1, 0x01, DMA_BLOCK);
    memcpy(pMem1, aesEcbKnownAnswer, sizeof(aesEcbKnownAnswer));
    memset(pMem2, 0x02, DMA_BLOCK);

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.cached = true;
    blockSettings.resetCrypto = true;              
    blockSettings.scatterGatherCryptoStart = true; 
    blockSettings.scatterGatherCryptoEnd = true;   

    blockSettings.pSrcAddr = pMem1;
    blockSettings.pDestAddr = pMem2;
    blockSettings.blockSize = DMA_BLOCK;
    rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
    if(rc!=NEXUS_SUCCESS) { 
        for(;;) {
            NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
            if(jobStatus.currentState == NEXUS_DmaJobState_eComplete ) {
                break;
            }
        } 
    }
    if (memcmp(pMem2,aesEcbIn,sizeof(aesEcbIn))) {
        fprintf(stderr, "Decryption failed to match expected answer\n");
        return 1;
    }

    NEXUS_DmaJob_Destroy(dmaJob);

    NEXUS_Memory_Free(pMem2);
    NEXUS_Memory_Free(pMem1);

    NEXUS_Dma_Close(dma);
#else
	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);
#endif /* NEXUS_HAS_SECURITY */

    return 0;
}
