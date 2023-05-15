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
 * $brcm_Workfile: dma_bulk.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 11/16/12 6:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_bulk.c $
 * 
 * Hydra_Software_Devel/8   11/16/12 6:10p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/7   1/16/12 11:08a mphillip
 * SW7125-1116: Add missing NEXUS_Security_GetDefaultClearKey calls
 * 
 * Hydra_Software_Devel/6   4/15/11 3:14p jtna
 * SW7422-420: replace deprecated NEXUS_DmaJob_Start() with
 * NEXUS_DmaJob_ProcessBlocks()
 * 
 * Hydra_Software_Devel/5   3/10/11 2:50p erickson
 * SW7420-1010: add some BDBG_ASSERT and BKNI_Fail calls so that EC can
 * catch failures
 *
 * Hydra_Software_Devel/4   11/4/08 5:44p vsilyaev
 * PR 42158: Added fast mode to cooperate with automated testing
 *
 * Hydra_Software_Devel/3   6/25/08 5:50p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * Hydra_Software_Devel/2   6/24/08 11:32p vsilyaev
 * PR 42392: Updated to use new security API
 *
 * Hydra_Software_Devel/1   6/18/08 3:18p vsilyaev
 * PR 42392: DMA unit test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DMA_FAST_TEST   1

#if DMA_FAST_TEST
#define DMA_BLOCK   1*1024*1024
#else
#define DMA_BLOCK   88*1024*1024
#endif


#define DMA_VERIFY_TRANSFER 1
#define DMA_CYPHER NEXUS_HAS_SECURITY

#if DMA_CYPHER
#include "nexus_security.h"
#endif

int main(int argc, char **argv)
{
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
    NEXUS_DmaJobStatus jobStatus;
    void *pMem1=NULL, *pMem2=NULL;
    unsigned i;
    unsigned block;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned alg;
#if DMA_CYPHER
    NEXUS_KeySlotHandle encKeyHandle;
    NEXUS_SecurityKeySlotSettings keySettings;
#endif


    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

#if DMA_CYPHER
    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    encKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!encKeyHandle) {
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }
#endif

    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2);
    memset(pMem1, 0x01, DMA_BLOCK);
    memset(pMem2, 0x02, DMA_BLOCK);

    for(alg=0;;alg++) {
        NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
        jobSettings.numBlocks = 1;
        jobSettings.busyWait = 100;
        if(alg==0) { }
#if DMA_CYPHER
        else {
            static const uint8_t keys[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
            NEXUS_SecurityAlgorithmSettings algConfig;
            NEXUS_SecurityClearKey key;

            NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
            NEXUS_Security_GetDefaultClearKey(&key);
            key.keySize = 16;
            switch(alg) {
            case 1:
                fprintf(stderr, "3DES\n");
                algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
                break;
            case 2:
                fprintf(stderr, "DES\n");
                algConfig.algorithm = NEXUS_SecurityAlgorithm_eDes;
                break;
            case 3:
                fprintf(stderr, "AES\n");
                algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
                break;
            case 4:
                fprintf(stderr, "AES192\n");
                algConfig.algorithm = NEXUS_SecurityAlgorithm_eAes192;
                key.keySize = 24;
                break;
            default:
                goto done;
            }
            algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
            algConfig.terminationMode = NEXUS_SecurityTerminationMode_eCipherStealing;
            algConfig.operation = NEXUS_SecurityOperation_eEncrypt;
            NEXUS_Security_ConfigAlgorithm(encKeyHandle, &algConfig);
            /* Load AV keys */
            key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
            BKNI_Memcpy(key.keyData, keys, sizeof(keys));
            if (NEXUS_Security_LoadClearKey(encKeyHandle, &key) != 0) {
                printf("\nLoad encryption key failed \n");
                return 1;
            }
            jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
            jobSettings.keySlot = encKeyHandle;
        }
#else /* DMA_CYPHER */
        else {goto done;}
#endif

        for(block=DMA_BLOCK;
#if DMA_FAST_TEST
                block > 256;
                block /=16
#else
                block > 16;
                block /=4
#endif
                ) {
#if DMA_VERIFY_TRANSFER
            memset(pMem1, 0x01, block);
            memset(pMem2, 0x02, block);
#endif
            fprintf(stderr, "---DMA block %u bytes----\n", block);

            dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

            NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
#if DMA_VERIFY_TRANSFER
            blockSettings.cached = true;
#else
            blockSettings.cached = false;
#endif
            if(alg!=0) {
                blockSettings.resetCrypto = true;
                blockSettings.scatterGatherCryptoStart = true;
                blockSettings.scatterGatherCryptoEnd = true;
            }

            for(i=0;(i*block)<DMA_BLOCK;i++) {
                blockSettings.pSrcAddr = pMem1;
                blockSettings.pDestAddr = pMem2;
                blockSettings.blockSize = block;
                rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
                BDBG_ASSERT(!rc || rc == NEXUS_DMA_QUEUED);
                if (rc == NEXUS_DMA_QUEUED) {
                    for(;;) {
                        rc = NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
                        BDBG_ASSERT(!rc);
                        if(jobStatus.currentState == NEXUS_DmaJobState_eComplete ) {
                            break;
                        }
                    }
                }

                /* if(i==0) fprintf(stderr, "%#x\n", *(uint8_t *)pMem2); */
#if DMA_VERIFY_TRANSFER
                if(alg==0) {
                    fprintf(stderr, "Comparing...   ");
                    if ( memcmp(pMem1, pMem2, block) ) {
                        fprintf(stderr, "Failed\n");
                        BKNI_Fail();
                    } else {
                        fprintf(stderr, "Success [%u]\n", i);
                    }
                    usleep(40*1000);
                }
#endif
            }
            fprintf(stderr, "---DMA block %u bytes %u runs ----\n", block, i);
            NEXUS_DmaJob_Destroy(dmaJob);
        }
    }
done:
    NEXUS_Memory_Free(pMem2);
    NEXUS_Memory_Free(pMem1);

    NEXUS_Dma_Close(dma);

    return 0;
}
