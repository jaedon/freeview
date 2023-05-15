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
 * $brcm_Workfile: dma_callbacks.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/27/12 3:24p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_callbacks.c $
 * 
 * Hydra_Software_Devel/4   7/27/12 3:24p jtna
 * SW7422-420: change test termination condition so that the test runs for
 * longer
 * 
 * Hydra_Software_Devel/3   10/3/11 11:09a erickson
 * SW7420-1995: cleanup close
 * 
 * Hydra_Software_Devel/2   5/3/11 11:47a jtna
 * SW7422-420: simplify test
 * 
 * Hydra_Software_Devel/1   4/20/11 5:11p jtna
 * SW7422-420: added dma_callbacks.c
 * 
 *****************************************************************************/

/* queue lots of DMA jobs. for each job that was queued, the app must receive one callback */

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DMA_BLOCK 32*1024*1024
#define DMA_JOBS 512

static void CompleteCallback(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    BKNI_SetEvent(pParam);
}

int main(int argc, char **argv)
{
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobHandle job[DMA_JOBS];
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobBlockSettings blockSettings;
    BKNI_EventHandle event[DMA_JOBS];
    void *pMem1 = NULL, *pMem2 = NULL;
    unsigned i, blockSize, numCompleted, numQueued;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    dma = NEXUS_Dma_Open(0, NULL);
    BDBG_ASSERT(dma);

    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2);
    BDBG_ASSERT(!rc);

    /* set memory to some random value */
    BKNI_Memset(pMem1, 0x86, DMA_BLOCK);
    BKNI_Memset(pMem2, 0x24, DMA_BLOCK);
    rc = BKNI_Memcmp(pMem1, pMem2, DMA_BLOCK);
    BDBG_ASSERT(rc!=0);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.completionCallback.callback = CompleteCallback;
    for (i=0; i<DMA_JOBS; i++) {
        rc = BKNI_CreateEvent(&event[i]);
        BDBG_ASSERT(!rc);
        jobSettings.completionCallback.context = event[i];
        job[i] = NEXUS_DmaJob_Create(dma, &jobSettings);
        BDBG_ASSERT(job[i]);
    }

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.cached = true;

    for (blockSize=128; blockSize*DMA_JOBS<DMA_BLOCK; blockSize+=128) {
        for (i=0,numCompleted=0,numQueued=0; i<DMA_JOBS; i++) {
            blockSettings.pSrcAddr = (uint8_t*)pMem1 + blockSize*i;
            blockSettings.pDestAddr = (uint8_t*)pMem2 + blockSize*i;
            blockSettings.blockSize = blockSize;
            rc = NEXUS_DmaJob_ProcessBlocks(job[i], &blockSettings, 1);
            switch (rc) {
                case NEXUS_SUCCESS:
                    numCompleted++;
                    BKNI_SetEvent(event[i]);
                    break;
                case NEXUS_DMA_QUEUED:
                    numQueued++;
                    break;
                default:
                    BDBG_ASSERT(0);
            }
        }

        fprintf(stderr, "block size %d: %u < %u\n", blockSize, blockSize*DMA_JOBS, DMA_BLOCK);
        for (i=0; i<DMA_JOBS; i++) {
            rc = BKNI_WaitForEvent(event[i], BKNI_INFINITE);
            BDBG_ASSERT(!rc);
        }
        rc = BKNI_Memcmp(pMem1, pMem2, blockSize*DMA_JOBS);
        BDBG_ASSERT(rc==0);

        BKNI_Memset(pMem2, rand(), DMA_BLOCK);
    }

    for (i=0; i<DMA_JOBS; i++) {
        BKNI_DestroyEvent(event[i]);
        NEXUS_DmaJob_Destroy(job[i]);
    }
    NEXUS_Dma_Close(dma);
    NEXUS_Memory_Free(pMem1);
    NEXUS_Memory_Free(pMem2);
    NEXUS_Platform_Uninit();

    return 0;
}
