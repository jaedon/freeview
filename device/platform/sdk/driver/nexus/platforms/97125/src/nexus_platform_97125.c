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
* $brcm_Workfile: nexus_platform_97125.c $
* $brcm_Revision: 11 $
* $brcm_Date: 7/27/12 4:44p $
*
* API Description:
*   API name: Platform Core
*    7400 core module initialization
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97125/src/nexus_platform_97125.c $
* 
* 11   7/27/12 4:44p mward
* SW7125-1300:  Allocate main heap first, in case no DOCSIS heap is
*  provided.
* 
* 10   7/23/12 1:39p mward
* SW7125-1300:  Detect DOCSIS heap in any bmem= order.
* 
* 9   7/9/12 3:34p mward
* SW7125-1300:  "we support >1 bmem region in a nexus memc region, but
*  they must be adjacent" requires bmem reservation for DOCSIS to be the
*  2nd, not the last.  Call NEXUS_Platform_P_GetHostMemory() in
*  NEXUS_Platform_P_GetDefaultSettings() to get the sizes of the BMEM and
*  DOCSIS regions from the OS, as adjacent bmem regions have already been
*  concatenated by nexus_platform_settings.
* 
* 8   11/17/11 6:26p mward
* SW7125-1121: Simplified NEXUS_Platform_P_GetDefaultSettings() 7125
*  platforms will (continue to) need bmem= statements to explicitly
*  specify the  DOCSIS reservation and BMEM in the first 256MB.
* 
* 7   11/16/11 6:49p mward
* SW7125-1121:  Use NEXUS_MEMORY_TYPE_RESERVED for a DOCSIS heap.
* 
* 6   7/28/11 4:17p mward
* SW7125-899:  Primary display buffers must come from a fully mapped heap
*  (main heap) only offscreen surfaces come from the graphics heap.
* 
* 5   4/6/11 7:09p mward
* SW7125-899:  NEXUS_Platform_P_GetFramebufferHeap() return graphics heap
*  if one has been created.
* 
* 4   4/6/11 2:20p mward
* SW7125-899:  Added NEXUS_Platform_P_GetDefaultSettings() to allow use
*  of multiple BMEM heaps.
* 
* 3   2/18/11 2:28p erickson
* SW7420-1078: add NEXUS_OFFSCREEN_SURFACE for
*  NEXUS_Platform_GetFramebufferHeap
*
* 2   12/22/10 3:12p erickson
* SW7422-2: adjustments
*
* 1   12/22/10 10:47a mward
* SW7422-2: rework nexus_platform_core
*
* 18   11/22/10 11:11a mward
* SW7420-1078:  Platforms not requiring heap control can return NULL in
*  the NEXUS_Platform_P_GetFramebufferHeap and the default heap for
*  display would be used for frame buffers.
*
* 17   10/30/10 11:59a mward
* SW7422-85: Add support for sharing MEMC1 with Linux
*
* 16   10/7/10 1:03p vsilyaev
* SW7405-4924: Communicate addressable size for each DDRAM controller
*
* 15   8/26/10 6:10p mward
* SW7125-460:  Changes needed for DVB-CI.
*
* 14   8/4/10 2:05p erickson
* SW7400-2865: add BREG_SetAtomicUpdate32Callback
*
* 13   7/15/10 4:26p mward
* SW7125-513:  Allow access to DS registers.
*
* 12   6/28/10 5:54p ayxiang
* SW7125-497: make register range bigger for PWM
*
* 11   6/3/10 5:18p erickson
* SW7405-3277: remove ext_sys_reset logic. it can cause NAND flash
*  corruptions depending on board wiring. if this needs to be added back
*  in, please consult with the linux team to see what effect it might
*  have on your system.
*
* 10   2/2/10 5:00p agin
* SWNOOS-385:  Adapt to new parameters for memory test.
*
* 9   12/14/09 7:00p agin
* SWNOOS-385:  Call memory test with correct parameters.
*
* 8   10/22/09 12:16p erickson
* SW7405-3277: update comments on ext_sys_reset. this code should not be
*  copied to customer systems without analysis.
*
* 7   10/21/09 4:23p mward
* SW7125-41: NEXUS_Platform_P_GetMainMemory() expects strapMemSize in
*  bytes, not MB.
*
* 6   9/24/09 10:52a mward
* SW7125-4: BCHP_IN656 registers renamed to BCHP_IN656_0 for consistency
*  with other chips.
*
* 5   9/4/09 2:19p mward
* SW7125-19: 7125 platforms get flash errors if external reset is
*  asserted in NEXUS_Platform_P_InitCore().  Disable the reset to allow
*  execution from flash.
*
* 4   8/27/09 8:11p mward
* SW7125-4: Default to 256MB for 97125.
*
* 3   8/25/09 1:46p erickson
* SW7420-228: move NEXUS_NUM_MEMC to nexus_platform_features.h
*
* 2   8/10/09 9:39p mward
* PR 55545: initialize memSize0
*
* 1   7/17/09 2:16p mward
* PR 55545: Copied and adapted from 97420dvr2.
*
* 9   7/14/09 1:10p agin
* PR56698: For BCM97456 no-os only, don't drive external system reset to
*  clear pending interrupts.
*
* 8   5/17/09 8:05a agin
* PR54069: Adapt DMA memory test for NEXUS.
*
* 7   4/13/09 6:12p mward
* PR53868: set the
*  NEXUS_VideoDecoderModuleSettings.avdHeapIndex[]appropriately for 7400.
*  Use NEXUS_Platform_P_SetCoreModuleSettings() to set MEMC config.
*
* 6   2/3/09 6:24p mward
* PR49968: Missing NEXUS_CoreModule_Uninit().
*
* 5   11/7/08 5:16p mphillip
* PR48697: Add support for kernel-mode NEXUS_CONFIG_IMAGE=y
*
* 4   10/17/08 12:13p mward
* PR47739: Use atomic update as recommended for
*  BCHP_SUN_TOP_CTRL_SW_RESET.
*
* 3   9/10/08 5:59p jgarrett
* PR46457: Updating InitCore prototype
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/3   9/28/07 4:25p jgarrett
* PR 35002: Fixing compiler warning
*
* Nexus_Devel/2   9/25/07 11:30a jgarrett
* PR 35002: Fixing memory sizing
*
* Nexus_Devel/1   9/24/07 1:00p jgarrett
* PR 35002: Splitting platform and core for better 97401/97400 sharing
*
***************************************************************************/
#include "nexus_types.h"
#include "nexus_base.h"
#include "priv/nexus_core.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"

#include "bchp_memc_ddr_0.h"

#if NEXUS_CONFIG_IMAGE
#include "nexus_img_kernel.h"
#endif

#ifdef NO_OS
#include "nexus_core_module.h"
#endif

BDBG_MODULE(nexus_platform_97125);

#if NEXUS_HAS_DVB_CI
static void *g_pMemoryEbi;
#endif

#ifndef NEXUS_DOCSIS_HEAP
#ifdef NEXUS_MEMC0_GRAPHICS_HEAP
#define NEXUS_DOCSIS_HEAP (NEXUS_MEMC0_GRAPHICS_HEAP+1)
#else
#define NEXUS_DOCSIS_HEAP (NEXUS_MEMC0_MAIN_HEAP+1)
#endif
#endif

/*
 * Platform specific API to assign implicit heap indices for various types of buffers
 */
void NEXUS_Platform_P_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    static NEXUS_PlatformMemory platformMemory;
    NEXUS_PlatformMemory *pMemory = &platformMemory;
    int i;

    NEXUS_Platform_P_GetHostMemory(pMemory); /* get bmem regions from OS, un-concatenated. */

    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = 0;
    pSettings->heap[NEXUS_DOCSIS_HEAP].size = 0;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 0;

    
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (0==pMemory->osRegion[i].length) {
            break;
        }
        if (pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size == 0) {
            /*
             * NEXUS_MEMC0_MAIN_HEAP for various data structures in different nexus and magnum modules.
             * Offscreen graphics surfaces can also be created on this heap.
             */
            pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
            pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = pMemory->osRegion[i].base >= 512*1024*1024 ? 1:0;
            pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = pMemory->osRegion[i].length;
            pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;
            BDBG_MSG(("Main heap OS region %d, %u@%x",i,pMemory->osRegion[i].length,pMemory->osRegion[i].base));
        }
        else if (pMemory->osRegion[i].base + pMemory->osRegion[i].length == 256*1024*1024) {
        /* if 2 or more bmem entries, the second is DOCSIS */
            pSettings->heap[NEXUS_DOCSIS_HEAP].size = pMemory->osRegion[i].length;
            BDBG_MSG(("DOCSIS heap OS region %d, %u@%x",i,pMemory->osRegion[i].length,pMemory->osRegion[i].base));
            pSettings->heap[NEXUS_DOCSIS_HEAP].memcIndex = 0;
            pSettings->heap[NEXUS_DOCSIS_HEAP].subIndex = 0; /* always in lower 256MB */
            pSettings->heap[NEXUS_DOCSIS_HEAP].memoryType = NEXUS_MEMORY_TYPE_RESERVED; /* no mapping, no allocations. external use. */
        }
        else {
#ifdef NEXUS_MEMC0_GRAPHICS_HEAP
            /* Linux boot command had 3 bmem statements
             * NEXUS_MEMC0_GRAPHICS_HEAP would be for creating off screen surfaces 
             * by the application with cached mapping only.
             */
            pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
            pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = pMemory->osRegion[i].base >= 512*1024*1024 ? 1:0;
            pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = pMemory->osRegion[i].length;
            pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */
            BDBG_MSG(("Graphics heap OS region %d, %u@%x",i,pMemory->osRegion[i].length,pMemory->osRegion[i].base));
#endif
        }
    }
    if (pSettings->heap[NEXUS_DOCSIS_HEAP].size == 0) {
        BDBG_WRN(("No DOCSIS heap region specified.  Use a bmem= statement on the kernel boot command line to reserve DOCSIS memory at the top of first 256MB"));
    }
}

NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(BREG_Handle hReg, NEXUS_PlatformMemory *pMemory, NEXUS_PlatformSettings *pSettings)
{
    uint32_t regValue, ddr0Device;
    unsigned memSizes[1];
    NEXUS_Error errCode = NEXUS_SUCCESS;
    BSTD_UNUSED(pSettings);

    BKNI_Memset(pMemory, 0, sizeof(*pMemory));

    /* DRAM width for 7125 is always 32 bit. 2*16-bit DDR parts. Need not read
    BCHP_MEMC_DDR23_APHY_AC_0_CONFIG.DRAM_WIDTH */

    /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
    ddr0Device = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

    switch(ddr0Device)
    {
        case 0: /* 256 Mbits Device Tech*/
            memSizes[0] = 2*32*1024*1024;
            break;
        case 1: /* 512 Mbits Device Tech*/
            memSizes[0] = 2*64*1024*1024;
            break;
        case 2: /* 1024 Mbits Device Tech*/
            memSizes[0] = 2*128*1024*1024;
            break;
        case 3: /* 2048 Mbits Device Tech*/
            memSizes[0] = 2*256*1024*1024;
            break;
        case 4: /* 4096 Mbits Device Tech */
            memSizes[0] = 2*512*1024*1024;
            break;
    default:
            BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
            errCode = NEXUS_UNKNOWN;
            goto error;
    }

    BDBG_MSG(("Memory indicates: MEMC0: %d MB", memSizes[0] >> 20));

    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];
error:
    return errCode;
}

NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex)
{
    if ((g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP]) && (NEXUS_OFFSCREEN_SURFACE==displayIndex)) {
        return g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP]; 
    }
    else {
        return g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
    }
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

