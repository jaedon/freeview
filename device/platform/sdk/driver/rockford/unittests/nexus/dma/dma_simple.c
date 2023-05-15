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
 * $brcm_Workfile: dma_simple.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/10/12 3:32p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/dma/dma_simple.c $
 * 
 * Hydra_Software_Devel/1   2/10/12 3:32p jtna
 * SW7420-420: add a really simple dma test
 * 
 *****************************************************************************/
#include "nexus_platform.h"
#if NEXUS_HAS_DMA
#include "nexus_dma.h"
#endif
#include "nexus_memory.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DMA_BLOCK   8*1024*1024

#if NEXUS_HAS_DMA
static void callback(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    fprintf(stderr, "callback: %#lx\n", (unsigned long)pParam);
    BKNI_SetEvent(pParam);
}
#endif

int main(void)
{
#if NEXUS_HAS_DMA
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
    void *pMem1=NULL, *pMem2=NULL;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    BKNI_EventHandle event;

    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem1);
    NEXUS_Memory_Allocate(DMA_BLOCK, NULL, &pMem2);
    BKNI_CreateEvent(&event);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.busyWait = 500000;
    jobSettings.busyWaitThreshold = DMA_BLOCK;
    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr = pMem1;
    blockSettings.pDestAddr = pMem2;
    blockSettings.blockSize = DMA_BLOCK;
    rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);

    NEXUS_Memory_Free(pMem2);
    NEXUS_Memory_Free(pMem1);
    BKNI_DestroyEvent(event);

    NEXUS_DmaJob_Destroy(dmaJob);
    NEXUS_Dma_Close(dma);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
