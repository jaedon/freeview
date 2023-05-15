/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: dma_api_usage.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/12/11 2:41p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_api_usage.c $
 * 
 * Hydra_Software_Devel/5   5/12/11 2:41p jtna
 * SW7550-739: add sharf testing
 * 
 * Hydra_Software_Devel/4   4/25/11 5:29p jtna
 * SW7422-416: add check for invalid SG crypto start/end indicators
 * 
 * Hydra_Software_Devel/3   4/20/11 5:12p jtna
 * SW7422-420: cosmetic update
 * 
 * Hydra_Software_Devel/2   4/20/11 3:53p jtna
 * SW7422-420: added more tests
 * 
 * Hydra_Software_Devel/1   4/14/11 6:43p jtna
 * SW7422-420: add dma_api_usage test
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DMA_BLOCK 4*1024*1024
#define NUM_DMA_HANDLES 4
#define NUM_JOBS_PER_HANDLE 2

#if 0
static void CompleteCallback(void *pParam, int iParam)
{
    iParam=iParam;
    fprintf(stderr, "CompleteCallback:%#lx fired\n", (unsigned long)pParam);
    BKNI_SetEvent(pParam);
}
#endif

int main(int argc, char **argv)
{
    NEXUS_DmaHandle dma[NUM_DMA_HANDLES];
    NEXUS_DmaSettings dmaSettings;
    NEXUS_DmaJobHandle job[NUM_DMA_HANDLES*NUM_JOBS_PER_HANDLE];
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobBlockSettings blockSettings;
    NEXUS_DmaJobStatus jobStatus;
    void *pMem1=NULL, *pMem2=NULL;
    unsigned i;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

#if NEXUS_NUM_DMA_CHANNELS
    /* virtualization */
    {
        for (i=0; i<NUM_DMA_HANDLES; i++) {
            dma[i] = NEXUS_Dma_Open(0, NULL);
            BDBG_ASSERT(dma[i]);
        }
        NEXUS_Platform_Uninit(); /* this will close all opened NEXUS_Dma handles */
    }
#endif
    /* now re-init */
    NEXUS_Platform_Init(&platformSettings);

    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2);
    BDBG_ASSERT(!rc);

    /* open with different settings */
    {
        NEXUS_Dma_GetDefaultSettings(&dmaSettings);
        for (i=0; i<NUM_DMA_HANDLES; i++) {
            dmaSettings.swapMode = (NEXUS_DmaSwapMode)i%NEXUS_DmaSwapMode_eMax;
#if 0
            dmaSettings.coreType = NEXUS_DmaCoreType_eSharf;
#endif
            dma[i] = NEXUS_Dma_Open(0, &dmaSettings);
            BDBG_ASSERT(dma[i]);
        }
    }

    /* test swapMode and busyWait */
    {
        *((uint32_t*)pMem1) = 0x00112233;
        NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
        jobSettings.busyWaitThreshold = 4096;
        jobSettings.busyWait = 200;
        for (i=0; i<NUM_DMA_HANDLES*NUM_JOBS_PER_HANDLE; i++) {
            unsigned parentIndex = i/NUM_JOBS_PER_HANDLE;
            job[i] = NEXUS_DmaJob_Create(dma[parentIndex], &jobSettings);
            BDBG_ASSERT(job[i]);
        }
    
        NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
        for (i=0; i<NUM_DMA_HANDLES*NUM_JOBS_PER_HANDLE; i++) {
            unsigned parentIndex = i/NUM_JOBS_PER_HANDLE;
            blockSettings.pSrcAddr = pMem1;
            blockSettings.pDestAddr = pMem2;
            blockSettings.blockSize = 4;
            blockSettings.cached = true;
            rc = NEXUS_DmaJob_ProcessBlocks(job[i], &blockSettings, 1);
            BDBG_ASSERT(rc==NEXUS_SUCCESS || rc==NEXUS_DMA_QUEUED); /* it should have finished within the function */

            NEXUS_Dma_GetSettings(dma[parentIndex], &dmaSettings);
            switch (dmaSettings.swapMode) {
                case NEXUS_DmaSwapMode_eNone:
                    BDBG_ASSERT(*((uint32_t*)pMem2)==0x00112233);
                    break;
                case NEXUS_DmaSwapMode_eWord:
                    BDBG_ASSERT(*((uint32_t*)pMem2)==0x22330011);
                    break;
                case NEXUS_DmaSwapMode_eByte:
                    BDBG_ASSERT(*((uint32_t*)pMem2)==0x33221100);
                    break;
                default:
                    BDBG_ASSERT(0);
            }
        }
    }
    
    /* now test changing to the same settings */
    {
        for (i=0; i<NUM_DMA_HANDLES; i++) {
            NEXUS_Dma_GetSettings(dma[i], &dmaSettings);
            BDBG_ASSERT(dmaSettings.swapMode==(NEXUS_DmaSwapMode)i%NEXUS_DmaSwapMode_eMax);
            dmaSettings.swapMode = NEXUS_DmaSwapMode_eNone;
            NEXUS_Dma_SetSettings(dma[i], &dmaSettings); /* this should change the settings for all jobs */

            NEXUS_Dma_GetSettings(dma[i], &dmaSettings);
            BDBG_ASSERT(dmaSettings.swapMode == NEXUS_DmaSwapMode_eNone);        
        }

        NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
        for (i=0; i<NUM_DMA_HANDLES*NUM_JOBS_PER_HANDLE; i++) {
            unsigned parentIndex = i/NUM_JOBS_PER_HANDLE;
            *((uint32_t*)pMem2) = 0;
            blockSettings.pSrcAddr = pMem1;
            blockSettings.pDestAddr = pMem2;
            blockSettings.blockSize = 4;
            blockSettings.cached = true;
            rc = NEXUS_DmaJob_ProcessBlocks(job[i], &blockSettings, 1);
            BDBG_ASSERT(rc==NEXUS_SUCCESS); /* it should have finished within the function */

            NEXUS_Dma_GetSettings(dma[parentIndex], &dmaSettings);
            BDBG_ASSERT(*((uint32_t*)pMem2)==0x00112233);
        }
    }

    /* we don't need a large number of jobs any more. close them */
    for (i=0; i<NUM_DMA_HANDLES*NUM_JOBS_PER_HANDLE; i++) {
        NEXUS_DmaJob_Destroy(job[i]);
    }

    /* pass in incorrect sgStart/sgEnd settings. it should error out */
    {
        NEXUS_DmaJobBlockSettings aBlockSettings[4];
        NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
        jobSettings.numBlocks = 4;
        job[0] = NEXUS_DmaJob_Create(dma[0], &jobSettings);
        BDBG_ASSERT(job[0]);
        NEXUS_DmaJob_GetDefaultBlockSettings(&aBlockSettings[0]);
        aBlockSettings[0].pSrcAddr = pMem1;
        aBlockSettings[0].pDestAddr = pMem2;
        aBlockSettings[0].blockSize = DMA_BLOCK;
        aBlockSettings[0].resetCrypto = true;
        NEXUS_DmaJob_GetDefaultBlockSettings(&aBlockSettings[1]);
        aBlockSettings[1].pSrcAddr = pMem1;
        aBlockSettings[1].pDestAddr = pMem2;
        aBlockSettings[1].blockSize = DMA_BLOCK;
        aBlockSettings[1].resetCrypto = true;
        NEXUS_DmaJob_GetDefaultBlockSettings(&aBlockSettings[2]);
        aBlockSettings[2].pSrcAddr = pMem1;
        aBlockSettings[2].pDestAddr = pMem2;
        aBlockSettings[2].blockSize = DMA_BLOCK;
        aBlockSettings[2].resetCrypto = true;
        NEXUS_DmaJob_GetDefaultBlockSettings(&aBlockSettings[3]);
        aBlockSettings[3].pSrcAddr = pMem1;
        aBlockSettings[3].pDestAddr = pMem2;
        aBlockSettings[3].blockSize = DMA_BLOCK;
        aBlockSettings[3].resetCrypto = true;

        /* end without start */
        aBlockSettings[0].scatterGatherCryptoStart = false;
        aBlockSettings[0].scatterGatherCryptoEnd = true;
        aBlockSettings[1].scatterGatherCryptoStart = false;
        aBlockSettings[1].scatterGatherCryptoEnd = false;
        rc = NEXUS_DmaJob_ProcessBlocks(job[0], aBlockSettings, 2);
        BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);

        /* start without end */
        aBlockSettings[0].scatterGatherCryptoStart = true;
        aBlockSettings[0].scatterGatherCryptoEnd = false;
        aBlockSettings[1].scatterGatherCryptoStart = false;
        aBlockSettings[1].scatterGatherCryptoEnd = false;
        rc = NEXUS_DmaJob_ProcessBlocks(job[0], aBlockSettings, 2);        
        BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);

        /* two starts */
        aBlockSettings[0].scatterGatherCryptoStart = true;
        aBlockSettings[0].scatterGatherCryptoEnd = false;
        aBlockSettings[1].scatterGatherCryptoStart = true;
        aBlockSettings[1].scatterGatherCryptoEnd = false;
        aBlockSettings[2].scatterGatherCryptoStart = false;
        aBlockSettings[2].scatterGatherCryptoEnd = true;
        rc = NEXUS_DmaJob_ProcessBlocks(job[0], aBlockSettings, 3);
        BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);

        /* two ends */
        aBlockSettings[0].scatterGatherCryptoStart = true;
        aBlockSettings[0].scatterGatherCryptoEnd = false;
        aBlockSettings[1].scatterGatherCryptoStart = false;
        aBlockSettings[1].scatterGatherCryptoEnd = true;
        aBlockSettings[2].scatterGatherCryptoStart = false;
        aBlockSettings[2].scatterGatherCryptoEnd = true;
        rc = NEXUS_DmaJob_ProcessBlocks(job[0], aBlockSettings, 3);
        BDBG_ASSERT(rc==NEXUS_INVALID_PARAMETER);

        NEXUS_DmaJob_Destroy(job[0]);
    }

    /* exercise passing an array of blockSettings */
    {
        #define NUM_BLOCKS 4
        NEXUS_DmaJobBlockSettings aBlockSettings[NUM_BLOCKS];
        BDBG_CASSERT(NUM_BLOCKS%2==0);
        NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
        jobSettings.numBlocks = NUM_BLOCKS;
        job[0] = NEXUS_DmaJob_Create(dma[0], &jobSettings);
        BDBG_ASSERT(job[0]);
        /* set the memory to some random value */
        BKNI_Memset(pMem1, 0x4, DMA_BLOCK);
        BKNI_Memset(pMem2, 0xff, DMA_BLOCK);
        rc = BKNI_Memcmp(pMem1, pMem2, DMA_BLOCK);
        BDBG_ASSERT(rc!=0);
        
        for (i=0; i<NUM_BLOCKS; i++) {
            NEXUS_DmaJob_GetDefaultBlockSettings(&aBlockSettings[i]);
            aBlockSettings[i].pSrcAddr = (uint8_t*)pMem1+(DMA_BLOCK/NUM_BLOCKS)*i;
            aBlockSettings[i].pDestAddr = (uint8_t*)pMem2+(DMA_BLOCK/NUM_BLOCKS)*i;
            aBlockSettings[i].blockSize = DMA_BLOCK/NUM_BLOCKS;
            aBlockSettings[i].cached = true;
        }
        rc = NEXUS_DmaJob_ProcessBlocks(job[0], aBlockSettings, NUM_BLOCKS);
        BDBG_ASSERT(rc==NEXUS_SUCCESS || rc==NEXUS_DMA_QUEUED);
        while (1) {
            NEXUS_DmaJob_GetStatus(job[0], &jobStatus);
            if (jobStatus.currentState == NEXUS_DmaJobState_eComplete) {
                break;
            }
            BKNI_Sleep(100);
            fprintf(stderr, "Waiting\n");
        }
        rc = BKNI_Memcmp(pMem1, pMem2, DMA_BLOCK);
        BDBG_ASSERT(rc==0);
    }

    /* test destroying a job while another job is running. the undestroyed job should finish. 
       this exercises power management as well, since magnum deasserts DMA RUN is when all MMD contexts
       are destroyed and Nexus powers down the DMA clock */
    {
        #undef NUM_BLOCKS
        #define NUM_BLOCKS 2
        NEXUS_DmaJobBlockSettings aBlockSettings[NUM_BLOCKS];
        BDBG_CASSERT(DMA_BLOCK >= 4*1024*1024); /* the block size must be large enough so that we can destroy the job in the middle of it */
        NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
        jobSettings.numBlocks = NUM_BLOCKS;
        job[1] = NEXUS_DmaJob_Create(dma[0], &jobSettings);
        BDBG_ASSERT(job[1]);

        /* set the memory to some random value */
        BKNI_Memset(pMem1, 0x4, DMA_BLOCK);
        BKNI_Memset(pMem2, 0xff, DMA_BLOCK);
        rc = BKNI_Memcmp(pMem1, pMem2, DMA_BLOCK);
        BDBG_ASSERT(rc!=0);

        for (i=0; i<NUM_BLOCKS; i++) {
            NEXUS_DmaJob_GetDefaultBlockSettings(&aBlockSettings[i]);
            aBlockSettings[i].pSrcAddr = (uint8_t*)pMem1+(DMA_BLOCK/NUM_BLOCKS)*i;
            aBlockSettings[i].pDestAddr = (uint8_t*)pMem2+(DMA_BLOCK/NUM_BLOCKS)*i;
            aBlockSettings[i].blockSize = DMA_BLOCK/NUM_BLOCKS;
            aBlockSettings[i].cached = true;
            NEXUS_DmaJob_ProcessBlocks(job[i], aBlockSettings, NUM_BLOCKS);
        }
        NEXUS_DmaJob_Destroy(job[0]);
        while (1) {
            NEXUS_DmaJob_GetStatus(job[1], &jobStatus);
            if (jobStatus.currentState == NEXUS_DmaJobState_eComplete) {
                break;
            }
            BKNI_Sleep(100);
            fprintf(stderr, "Waiting\n");
        }

        /* we don't care about the first half, but the second half should be equal */
        rc = BKNI_Memcmp((uint8_t*)pMem1+DMA_BLOCK/NUM_BLOCKS, (uint8_t*)pMem2+DMA_BLOCK/NUM_BLOCKS, DMA_BLOCK/NUM_BLOCKS);
        BDBG_ASSERT(rc==0);

        /* now destroy the other job as well. this should trigger a dynamic power management power down */
        NEXUS_DmaJob_Destroy(job[1]);

        /* create another job which should trigger a power up */
        BKNI_Memset(pMem1, 0xfe, DMA_BLOCK);
        rc = BKNI_Memcmp(pMem1, pMem2, DMA_BLOCK);
        BDBG_ASSERT(rc!=0);
        job[0] = NEXUS_DmaJob_Create(dma[0], &jobSettings);

        NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
        blockSettings.pSrcAddr = pMem1;
        blockSettings.pDestAddr = pMem2;
        blockSettings.blockSize = DMA_BLOCK;
        blockSettings.cached = true;
        NEXUS_DmaJob_ProcessBlocks(job[0], &blockSettings, 1);

        while (1) {
            NEXUS_DmaJob_GetStatus(job[0], &jobStatus);
            if (jobStatus.currentState == NEXUS_DmaJobState_eComplete) {
                break;
            }
            BKNI_Sleep(100);
            fprintf(stderr, "Waiting\n");
        }
        rc = BKNI_Memcmp(pMem1, pMem2, DMA_BLOCK);
        BDBG_ASSERT(rc==0);

        /* now just close */
        for (i=0; i<NUM_DMA_HANDLES; i++) {
            NEXUS_Dma_Close(dma[i]);
        } 
        BKNI_Sleep(5000); /* wait to see if anything wants to go wrong */
    }

    /* uninit */
    NEXUS_Memory_Free(pMem1);
    NEXUS_Memory_Free(pMem2);
    NEXUS_Platform_Uninit();

    return 0;
}
