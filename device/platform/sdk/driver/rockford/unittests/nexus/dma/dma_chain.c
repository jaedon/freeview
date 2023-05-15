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
 * $brcm_Workfile: dma_chain.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/15/11 10:46a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_chain.c $
 * 
 * Hydra_Software_Devel/4   4/15/11 10:46a jtna
 * SW7422-420: add NEXUS_Platform_Uninit at the end
 * 
 * Hydra_Software_Devel/3   3/10/11 2:50p erickson
 * SW7420-1010: add some BDBG_ASSERT and BKNI_Fail calls so that EC can
 * catch failures
 *
 * Hydra_Software_Devel/2   11/4/08 5:44p vsilyaev
 * PR 42158: Added fast mode to cooperate with automated testing
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
/* #define DMA_BLOCK   2*1024*1024 */
#define DMA_BLOCK  160*1024

static void CompleteCallback(void *pParam, int iParam)
{
    iParam=iParam;
    pParam=pParam;
    /* fprintf(stderr, "CompleteCallback:%#lx fired\n", (unsigned long)pParam); */
}

#define CHAIN   (8)


int main(int argc, char **argv)
{
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJobs[CHAIN];
    NEXUS_DmaJobBlockSettings blockSettings;
    NEXUS_DmaJobStatus jobStatus;
    void *pMem[CHAIN+1];
    unsigned block,block_size;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned chain,i,count;

    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    dma = NEXUS_Dma_Open(0, NULL);

    for(chain=0;chain<CHAIN+1;chain++) {
        NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem[chain]);
    }

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.completionCallback.callback = CompleteCallback;
    for(chain=0;chain<CHAIN;chain++) {
        jobSettings.completionCallback.context = &dmaJobs[chain];
        dmaJobs[chain] = NEXUS_DmaJob_Create(dma, &jobSettings);
    }

    for(block_size=DMA_BLOCK;block_size > 16;
#if DMA_FAST_TEST
            block_size /= 16
#else
            block_size -= block_size/16
#endif
            ) {
        fprintf(stderr, "---DMA block %u bytes----\n", block_size);
        for(block=block_size,count=0;count<16;count++,block -= block/64) {
            for(chain=0;chain<CHAIN+1;chain++) {
                memset(pMem[chain], chain, block);
            }
            NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
            for(chain=0;chain<CHAIN;chain++) {
                blockSettings.blockSize = block;
                blockSettings.pSrcAddr = pMem[chain];
                blockSettings.pDestAddr = pMem[chain+1];
                rc = NEXUS_DmaJob_ProcessBlocks(dmaJobs[chain], &blockSettings, 1);
                BDBG_ASSERT(!rc || rc == NEXUS_DMA_QUEUED);
            }
            if(rc==NEXUS_DMA_QUEUED) {
                fprintf(stderr, "QUEUED\n");
            }

            for(i=0;;i++) {
                rc = NEXUS_DmaJob_GetStatus(dmaJobs[CHAIN-1], &jobStatus);
                BDBG_ASSERT(!rc);
                if(jobStatus.currentState == NEXUS_DmaJobState_eComplete ) {
                    break;
                }
                if(i==0) {
                    fprintf(stderr, "Waiting...");
                }
                fprintf(stderr, " %u ", i);
                usleep(1000);
            }
            if(i>0) {
                fprintf(stderr, "\n");
            }

            fprintf(stderr, "%u: ", block);
            for(chain=0;chain<CHAIN;chain++) {
                if ( memcmp(pMem[chain+1], pMem[0], block) ) {
                    fprintf(stderr, "Failed %u\n", chain);
                    BKNI_Fail();
                } else {
                    fprintf(stderr, " %u ", chain);
                }
            }
            fprintf(stderr, "\n");

            usleep(40*1000);
        }
    }

    for(chain=0;chain<CHAIN;chain++) {
        NEXUS_DmaJob_Destroy(dmaJobs[chain]);
    }

    for(chain=0;chain<CHAIN+1;chain++) {
        NEXUS_Memory_Free(pMem[chain]);
    }

    NEXUS_Dma_Close(dma);
    NEXUS_Platform_Uninit();

    return 0;
}
