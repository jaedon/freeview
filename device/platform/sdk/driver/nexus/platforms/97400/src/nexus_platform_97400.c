/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_platform_97400.c $
* $brcm_Revision: 6 $
* $brcm_Date: 5/10/12 11:27a $
*
* API Description:
*   API name: Platform Core
*    7400 core module initialization
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/nexus_platform_97400.c $
* 
* 6   5/10/12 11:27a mward
* SW7425-2607:  Set VideoDecoder heap sizes in
*  NEXUS_Platform_P_VideoDecoderSettings().
* 
* 5   5/20/11 11:27a mward
* SW7422-445:  Set heap[0] to -1 for "all available memory".
* 
* 4   3/11/11 2:17p mward
* SW7422-2:  Fixed MEMC1 size for 7400.  32 or 64 MB.
* 
* 3   2/18/11 2:28p erickson
* SW7420-1078: add NEXUS_OFFSCREEN_SURFACE for
*  NEXUS_Platform_GetFramebufferHeap
*
* 2   1/7/11 5:37p mward
* SW7422-2:  Added NEXUS_Platform_P_GetDefaultSettings().
*
* 1   12/22/10 5:10p mward
* SW7422-2: add nexus platform core rework
*
* 1   12/22/10 10:47a mward
* SW7422-2: rework nexus_platform_core
*
***************************************************************************/
#include "nexus_types.h"
#include "nexus_base.h"
#include "priv/nexus_core.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"

#include "bchp_sun_top_ctrl.h"

#if NEXUS_CONFIG_IMAGE
#include "nexus_img_kernel.h"
#endif

#ifdef NO_OS
#include "nexus_core_module.h"
#endif

BDBG_MODULE(nexus_platform_97400);

#if NEXUS_HAS_DVB_CI
static void *g_pMemoryEbi;
#endif

#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    unsigned i;
    pSettings->avdHeapIndex[0] = 1;
    pSettings->mfdMapping[0] = 0;

    pSettings->avdHeapIndex[1] = 2;
    pSettings->mfdMapping[1] = 1;

    for(i=0;i<NEXUS_NUM_XVD_DEVICES;i++) {
        pSettings->heapSize[i].general = NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE;
        pSettings->heapSize[i].secure = NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE;
        pSettings->heapSize[i].picture = NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE;
    }

    return;
}
#endif

/*
 * Platform specific API to assign implicit heap indices for various types of buffers
 */
void NEXUS_Platform_P_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    BDBG_MSG(("\n NEXUS_Platform_P_GetDefaultSettings -->"));
    /*
     * Main heap for various data structures in different nexus and magnum modules.
     */
    pSettings->heap[0].memcIndex = 0;
    pSettings->heap[0].subIndex = 0;
    /* pSettings->heap[0].size = 192 * 1024 * 1024; */
    pSettings->heap[0].size = -1;
    pSettings->heap[0].memoryType = NEXUS_MemoryType_eFull;

    /*
     * Picture memory for XVD0
     */
    pSettings->heap[1].memcIndex = 1;
    pSettings->heap[1].subIndex = 0;
    /* pSettings->heap[1].size = 64*1024*1024; */
    pSettings->heap[1].size = -1;
    pSettings->heap[1].memoryType = NEXUS_MemoryType_eDeviceOnly;

    /*
     * Picture memory for XVD1
     */
    pSettings->heap[2].memcIndex = 2;
    pSettings->heap[2].subIndex = 0;
    pSettings->heap[2].size = -1;
    pSettings->heap[2].memoryType = NEXUS_MemoryType_eDeviceOnly;

#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
#endif
   BDBG_MSG(("\n NEXUS_Platform_P_GetDefaultSettings <--"));
}


NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(BREG_Handle hReg, NEXUS_PlatformMemory *pMemory, NEXUS_PlatformSettings *pSettings)
{
    uint32_t regValue, board_strap;
    unsigned memSizes[3];
    BSTD_UNUSED(pSettings);

    BKNI_Memset(pMemory, 0, sizeof(*pMemory));

    regValue = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_STRAP_VALUE_0);

    board_strap = BCHP_GET_FIELD_DATA(regValue, SUN_TOP_CTRL_STRAP_VALUE_0, strap_ddr_configuration);

    memSizes[0] = (4 * 16 << (board_strap & 3)) << 20;
    memSizes[1] = memSizes[2] = (32 << ((board_strap & 4)!=0)) << 20;

    BDBG_MSG(("Memory indicates: MEMC0: %d MB MEMC1: %d MB MEMC2: %d MB",
               memSizes[0] >> 20,memSizes[1] >> 20,memSizes[2] >> 20));

    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];
    pMemory->memc[1].length = memSizes[1];
    pMemory->memc[2].length = memSizes[2];

    return NEXUS_SUCCESS;
}

NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex)
{
    BSTD_UNUSED(displayIndex);
    return g_pCoreHandles->nexusHeap[0]; /* just return heap[0] for all requests */
}

NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
#if NEXUS_HAS_DVB_CI
    g_pMemoryEbi = NEXUS_Platform_P_MapMemory(NEXUS_DVB_CI_MEMORY_BASE, NEXUS_DVB_CI_MEMORY_LENGTH, false);
    if (!g_pMemoryEbi) { errCode = BERR_TRACE(BERR_OS_ERROR); goto err_ebi; }
    NEXUS_P_AddMap(NEXUS_DVB_CI_MEMORY_BASE, NULL, g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
err_ebi:
#endif
    return errCode;
}

void NEXUS_Platform_P_UninitBoard(void)
{
#if NEXUS_HAS_DVB_CI
    if (g_pMemoryEbi) {
        NEXUS_Platform_P_UnmapMemory(g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
    }
#endif
}

