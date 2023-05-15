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
* $brcm_Workfile: nexus_platform_core.c $
* $brcm_Revision: 57 $
* $brcm_Date: 10/16/12 10:28a $
*
* Revision History:
*
 * $brcm_Log: /nexus/platforms/common/src/nexus_platform_core.c $
 * 
 * 57   10/16/12 10:28a erickson
 * SW7425-1792: add core to module list, remove unused code
 * 
 * 56   7/5/12 7:43a erickson
 * SW7425-3389: fix memset sizeof
 * 
 * 55   6/19/12 1:24p erickson
 * SW7435-238: concat all regions on a MEMC for ARC protection. allows
 *  linux in upper memory to be protected.
 * 
 * 54   6/19/12 10:56a erickson
 * SW7435-238: core module requires full MEMC base address and offset, not
 *  bmem-limited, for MRC to protect upper and lower memory
 * 
 * 53   3/28/12 11:09a erickson
 * SW7435-65: implement NEXUS_PlatformSettings.heap[].guardBanding
 * 
 * 52   3/14/12 3:12p erickson
 * SW7425-2607: use NEXUS_MAX_MEMC
 * 
 * 51   2/13/12 3:44p erickson
 * SW7231-657: check NEXUS_Heap_GetStatus return code
 * 
 * 50   11/29/11 11:38a erickson
 * SW7420-2129: remove unused function
 * 
 * 49   11/16/11 12:55p erickson
 * SW7422-445: read host memory in NEXUS_Platform_GetDefaultSettings so
 *  that platform code can adapt to various OS memory configurations
 * 
 * 48   11/15/11 3:19p erickson
 * SW7425-1747: remove old code
 * 
 * 47   9/9/11 2:10p agin
 * SWNOOS-454:  Don't include nexus_core_module.h.
 * 
 * 46   8/9/11 4:17p mward
 * SW7125-1073:  Condition at compile time using NEXUS_MAX_MEMC to
 *  eliminate unreachable code.
 * 
 * 45   7/11/11 11:44a erickson
 * SWDTV-7208: implement NEXUS_Platform_P_CalcSubMemc for DTV using linux
 *  bmem
 * 
 * 44   7/1/11 11:20a erickson
 * SW7346-187: consolidate code for MIPS kernel mode test. make
 *  nexus_map.c generic.
 * 
 * 43   5/19/11 3:49p erickson
 * SW7422-445: call NEXUS_Platform_P_ReadMemcConfig from
 *  NEXUS_Platform_GetDefaultSettings so that heap defaults can be based
 *  on board strapping option
 * 
 * 42   5/17/11 2:15p jhaberf
 * SWDTV-6928: Merge of 35233 platform code to the main
 * 
 * 41   5/12/11 6:56p agin
 * SWNOOS-457:  Use DIAGS_MEM_DMA_TEST insted of DIAGS.
 * 
 * 40   5/4/11 1:51p erickson
 * SW7346-187: add NEXUS_BASE_OS_linuxkernel test
 * 
 * 39   5/3/11 5:08p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 * 
 * 38   4/6/11 2:15p mward
 * SW7125-899:  Allow memc region[1] at 512MB for 65NM chips with other
 *  than BMIPS5000.
 * 
 * 37   3/15/11 9:06p agin
 * SWNOOS-447:  Use flag to determine whether to run dma memory test.
 * 
 * 36   3/4/11 11:27a erickson
 * SW7400-3006: add NEXUS_Platform_CreateHeap
 *
 * 35   3/2/11 11:14a erickson
 * SW7422-310: use same fake address for cached & uncached to converse
 *  fake address space
 *
 * 34   3/2/11 10:46a erickson
 * SW7422-310: dynamic fake address mapping scheme using nexus_map.c
 *
 * 33   1/19/11 5:08p erickson
 * SW7422-220: allow heap[] array to be sparse
 *
 * 32   1/13/11 4:02p mward
 * SW7422-2:  Fixes for 7400, which has 3 MEMCs.
 *
 * 31   1/5/11 11:11a agin
 * SWNOOS-437:  Support memory test for no-os.
 *
 * 30   1/4/11 12:15p erickson
 * SW7422-2: don't unmap fake mappings
 *
 * 29   12/30/10 2:41p erickson
 * SW7422-2: fix 7425
 *
 * 28   12/30/10 1:19p erickson
 * SW7422-2: update macros
 *
 * 27   12/28/10 2:43p erickson
 * SW7422-2: define BMIPS5500 in nexus_platform_priv.h based on BCHP_CHIP
 *  list
 *
 * 26   12/28/10 9:53a gmohile
 * SW7422-2 : 7408 support for nexus_platform_core rework
 *
 * 25   12/27/10 2:36p erickson
 * SW7422-2: add ERR
 *
 * 24   12/27/10 10:39a erickson
 * SW7422-2: use local var
 *
 * 23   12/27/10 9:48a erickson
 * SW7422-2: add chips to list, correct limits
 *
 * 22   12/22/10 5:01p erickson
 * SW7422-2: fix NEXUS_Platform_P_CalcSubMemc
 *
 * 21   12/22/10 2:28p erickson
 * SW7422-2: add support for multiple physical address regions within a
 *  single MEMC
 *
 * 20   12/21/10 2:19p erickson
 * SW7422-2: do calc so that >1 MEMC1 heap is allowed
 *
 * 19   12/20/10 6:38p erickson
 * SW7422-2: rework for MEMC1 fake address technique
 *
 * 18   12/20/10 4:26p erickson
 * SW7422-2: rework nexus_platform_core, add board-specific file and API,
 *  mmap after determining heaps
*
***************************************************************************/
#include "nexus_types.h"
#include "nexus_base.h"
#include "priv/nexus_core.h"
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#ifdef NEXUS_FPGA_SUPPORT
#include "nexus_platform_fpga.h"
#endif
#include "breg_mem.h"
#if NEXUS_CONFIG_IMAGE
#include "nexus_img_kernel.h"
#endif
#include "bchp_common.h"
#include "nexus_map.h"

#ifdef DIAGS_MEM_DMA_TEST
extern int run_dma_memory_test;
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_CI)
#include "bchp_hif_top_ctrl.h"
#endif

#if defined(HUMAX_PLATFORM_BASE)
#include "bchp_memc_arb_0.h"
#if (NEXUS_NUM_MEMC == 2)
#include "bchp_memc_arb_1.h"
#endif
#include "humax_rts.h"
#endif 

BDBG_MODULE(nexus_platform_core);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
/*
 * It is often the case that HUMAX DI controls the BCM's registers directly.
 * To do this, DI needs the register handle that is opened here.
 * Let's define the hanle as a global variable.
 * dcchung 2009.3.21
 */
BREG_Handle g_hReg;
#else
static BREG_Handle g_hReg;
#endif
static void *g_pRegAddress;
static NEXUS_Core_Settings g_coreSettings;
NEXUS_PlatformMemory g_platformMemory;
static bool g_mipsKernelMode;

/* map all heaps */
static NEXUS_Error NEXUS_Platform_P_MapRegion(unsigned index, NEXUS_Core_MemoryRegion *region);
static void NEXUS_Platform_P_UnmapRegion(NEXUS_Core_MemoryRegion *region);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#define 	PHYS_EBI_CS2_BASE  			NEXUS_DVB_CI_MEMORY_BASE	/*0x1A000000*/
#define		PHYS_EBI_CS2_MEMRW_SIZE		NEXUS_DVB_CI_MEMORY_LENGTH	/*0x100000*/
/*#define		PHYS_EBI_CS2_IORW_SIZE		0x2000*/

BREG_Handle g_CI_MMAP_Handle;
void *g_pCIAddress;

NEXUS_Error Init_EBI_CS2_ForCI(void)
{
	g_pCIAddress = NEXUS_Platform_P_MapMemory(PHYS_EBI_CS2_BASE,
	                                           PHYS_EBI_CS2_MEMRW_SIZE, false);

	if ( NULL == g_pCIAddress )
	{
	    return BERR_TRACE(BERR_OS_ERROR);
	}

	/* Open register interface with mapped address */
	BREG_Open(&g_CI_MMAP_Handle, g_pCIAddress, PHYS_EBI_CS2_MEMRW_SIZE );
	return BERR_SUCCESS;
}
#endif

#if defined(HUMAX_PLATFORM_BASE)
void NEXUS_Platform_Registers_Read_Access(unsigned long Offset, unsigned long *Value)
{
	extern BREG_Handle g_hReg;

	/*Access register in 32bit mode*/
	*Value = BREG_Read32(g_hReg, Offset);
}

void NEXUS_Platform_Registers_Write_Access(unsigned long Offset, unsigned long Value)
{
	extern BREG_Handle g_hReg;

	/*Access register in 32bit mode*/
	BREG_Write32(g_hReg, Offset, Value);
}
#else
void NEXUS_Platform_Registers_Read_Access(unsigned long Offset, unsigned long *Value)
{
	BSTD_UNUSED(Offset);
	BSTD_UNUSED(Value);
}

void NEXUS_Platform_Registers_Write_Access(unsigned long Offset, unsigned long Value)
{
	BSTD_UNUSED(Offset);
	BSTD_UNUSED(Value);
}
#endif

#if defined(HUMAX_PLATFORM_BASE)
static void NEXUS_Platform_P_RtsOntheFly(BREG_Handle hReg)
{
	int i = 0;
	uint32_t reg;
	RTS_CLIENT * tRTSClientEntry = NULL;

	tRTSClientEntry = gpRTS_Optimized;
	if(tRTSClientEntry != NULL)
	{
		BDBG_MSG(("Update RTS MEMC0\n"));
		while (1) {
			if (tRTSClientEntry[i].uId== 0xFFFFFFFF) {
			    /* We're done */
			    break;
			}
		    reg = BREG_Read32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_0 + (tRTSClientEntry[i].uId* 4));
		    if (reg != tRTSClientEntry[i].uValue) {
		        BDBG_MSG(("Client %2d was 0x%08lx is 0x%08lx <==", tRTSClientEntry[i].uId, reg, tRTSClientEntry[i].uValue));
		        BREG_Write32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_0 + (tRTSClientEntry[i].uId * 4), tRTSClientEntry[i].uValue);
		    }
		    i++;
		}
	}
#if (NEXUS_NUM_MEMC == 2)
	tRTSClientEntry = gpRTS_Optimized_MEMC1;
	if(tRTSClientEntry != NULL)
	{
		BDBG_MSG(("Update RTS MEMC1\n"));
		while (1) {
			if (tRTSClientEntry[i].uId== 0xFFFFFFFF) {
			    /* We're done */
			    break;
			}
		    reg = BREG_Read32(hReg, BCHP_MEMC_ARB_1_CLIENT_INFO_0 + (tRTSClientEntry[i].uId* 4));
		    if (reg != tRTSClientEntry[i].uValue) {
		        BDBG_MSG(("Client %2d was 0x%08lx is 0x%08lx <==", tRTSClientEntry[i].uId, reg, tRTSClientEntry[i].uValue));
		        BREG_Write32(hReg, BCHP_MEMC_ARB_1_CLIENT_INFO_0 + (tRTSClientEntry[i].uId * 4), tRTSClientEntry[i].uValue);
		    }
		    i++;
		}
	}
#endif
}
#endif

BREG_Handle NEXUS_Platform_P_MapRegisters(void)
{
    BREG_Handle reg;
    
    if (g_pRegAddress) {
        BERR_TRACE(NEXUS_UNKNOWN);
        return NULL;
    }
    
    /* mmap registers in uncached address space */
    g_pRegAddress = NEXUS_Platform_P_MapMemory(BCHP_PHYSICAL_OFFSET, BCHP_REGISTER_END, NEXUS_MemoryMapType_eUncached);
    if ( NULL == g_pRegAddress ) {
        BERR_TRACE(BERR_OS_ERROR);
        return NULL;
    }

    /* Open register interface with mapped address */
    BREG_Open(&reg, g_pRegAddress, BCHP_REGISTER_END);
    return reg;
}

void NEXUS_Platform_P_UnmapRegisters(BREG_Handle reg)
{
    BREG_Close(reg);
    NEXUS_Platform_P_UnmapMemory(g_pRegAddress, BCHP_REGISTER_END);
    g_pRegAddress = NULL;
}

NEXUS_Error NEXUS_Platform_P_InitCore( NEXUS_PlatformSettings *pSettings )
{
    NEXUS_Error errCode = 0;
    unsigned int i = 0;
    NEXUS_PlatformMemory *pMemory = &g_platformMemory;
    struct nexus_map_settings map_settings;

/* TODO: for nfe image, g_mipsKernelMode could be passed in as runtime param */
#if NEXUS_MODE_driver || NEXUS_BASE_OS_linuxkernel
    g_mipsKernelMode = true;
#else
    g_mipsKernelMode = false;
#endif

    g_hReg = NEXUS_Platform_P_MapRegisters();
    if (!g_hReg) {
        goto err_mapreg;
    }

#if defined(HUMAX_PLATFORM_BASE)
	NEXUS_Platform_P_RtsOntheFly(g_hReg);
#endif

    /* determine heaps based on MEMC config and user config */
    NEXUS_CoreModule_GetDefaultSettings(&g_coreSettings);
    errCode = NEXUS_Platform_P_SetCoreModuleSettings(pSettings, pMemory, &g_coreSettings);
    if ( errCode ) { errCode=BERR_TRACE(errCode); goto err_memc; }

    nexus_p_get_default_map_settings(&map_settings);
    /* the fake address range is (2GB - 4K). subtracting 4K allows the code to avoid 0x0000_0000.
    for kernel mode, we can't have a valid fake address of 0x0000_0000. too much code depends on ptr == NULL meaning "no pointer".
    for user mode, we can't have base+size == 0x0000_0000. too much code will fail on the wrap around.
    only 1 byte is needed to avoid this situation, but using 4K avoid possible alignment bugs. */
    if (g_mipsKernelMode) {
        map_settings.offset = 0x00000000 + 4096;
        map_settings.size = 0x80000000 - 4096;
    }
    else {
        map_settings.offset = 0x80000000;
        map_settings.size = 0x80000000 - 4096;
    }
    map_settings.mmap = NEXUS_Platform_P_MapMemory;
    map_settings.munmap = NEXUS_Platform_P_UnmapMemory;
    errCode = nexus_p_init_map(&map_settings);
    if ( errCode ) {
        errCode=BERR_TRACE(errCode);
        goto err_memc;
    }

    /* map all heaps based on user settings */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        g_coreSettings.heapRegion[i].guardBanding = pSettings->heap[i].guardBanding;
        errCode = NEXUS_Platform_P_MapRegion(i, &g_coreSettings.heapRegion[i]);
        if (errCode) {errCode = BERR_TRACE(errCode); goto err_map;}
    }

    for (i = 0; i < NEXUS_MAX_MEMC; i++) {
        unsigned j;
        g_coreSettings.memcRegion[i].offset = pMemory->memc[i].region[0].original_base;
        g_coreSettings.memcRegion[i].length = 0;
        for (j=0;j<NEXUS_NUM_MEMC_REGIONS;j++) {
            /* this assumes MEMC0/MEMC1 regions on are interleaved, which is true for all existing silicon */
            if (pMemory->memc[i].region[j].original_length) {
                unsigned length = pMemory->memc[i].region[j].original_base + pMemory->memc[i].region[j].original_length - pMemory->memc[i].region[0].original_base;
                if (length > g_coreSettings.memcRegion[i].length) {
                    g_coreSettings.memcRegion[i].length = length;
                }
            }
        }
    }
    g_coreSettings.regHandle = g_hReg;
    g_coreSettings.interruptInterface.pDisconnectInterrupt = NEXUS_Platform_P_DisconnectInterrupt;
    g_coreSettings.interruptInterface.pConnectInterrupt = NEXUS_Platform_P_ConnectInterrupt;
    g_coreSettings.interruptInterface.pEnableInterrupt_isr = NEXUS_Platform_P_EnableInterrupt_isr;
    g_coreSettings.interruptInterface.pDisableInterrupt_isr = NEXUS_Platform_P_DisableInterrupt_isr;
#if NEXUS_CONFIG_IMAGE
    g_coreSettings.imgInterface.create = Nexus_IMG_Driver_Create;
    g_coreSettings.imgInterface.destroy = Nexus_IMG_Driver_Destroy;
    g_coreSettings.imgInterface.open= Nexus_IMG_Driver_Open;
    g_coreSettings.imgInterface.close= Nexus_IMG_Driver_Close;
    g_coreSettings.imgInterface.next= Nexus_IMG_Driver_Next;
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    errCode =  Init_EBI_CS2_ForCI();
    if (errCode != BERR_SUCCESS )
    {
        return BERR_TRACE(BERR_OS_ERROR);
    }
#endif

    /* Initialize core module */
    g_NEXUS_platformHandles.core = NEXUS_CoreModule_Init(&g_coreSettings);
    if ( !g_NEXUS_platformHandles.core ) {
        errCode=BERR_TRACE(NEXUS_UNKNOWN);
        goto err_core;
    }

    /* Set up OS/Magnum atomic access. We can't set this up until Core is ready. */
    BREG_SetAtomicUpdate32Callback(g_hReg, NEXUS_Platform_P_AtomicUpdateCallback_isr, NULL);

    NEXUS_Platform_P_ConfigureGisbTimeout();

#ifdef DIAGS_MEM_DMA_TEST
    if (run_dma_memory_test)
        NO_OS_MemoryTest(g_pCoreHandles->chp,
            g_pCoreHandles->reg,
            g_pCoreHandles->bint,
            g_pCoreHandles->heap[0],
            g_pCoreHandles->heap[1],
            g_pCoreHandles->heap[2],
            g_coreSettings.memcRegion[0].length,
            NULL,
            NULL);
#endif

    return BERR_SUCCESS;

/* Error cases */
err_core:
err_map:
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_Platform_P_UnmapRegion(&g_coreSettings.heapRegion[i]);
    }
    nexus_p_uninit_map();
err_memc:
    NEXUS_Platform_P_UnmapRegisters(g_hReg);
    g_hReg = NULL;
err_mapreg:

    return errCode;
}

void NEXUS_Platform_P_UninitCore(void)
{
    unsigned i;
    NEXUS_CoreModule_Uninit();
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_Platform_P_UnmapRegion(&g_coreSettings.heapRegion[i]);
    }
    nexus_p_uninit_map();
    NEXUS_Platform_P_UnmapRegisters(g_hReg);
    g_hReg = NULL;
}

static NEXUS_Error NEXUS_Platform_P_MapRegion(unsigned index, NEXUS_Core_MemoryRegion *region)
{
    NEXUS_MemoryMapType uncachedMapType, cachedMapType;

    BSTD_UNUSED(index); /* index included for debug */
    if (!region->length) return 0;

    BDBG_MSG(("NEXUS_Platform_P_MapRegion heap[%d] %#x:%d, memoryType=%x", index, region->offset, region->length, region->memoryType));
    /* this function runs in the driver. if in user mode, the driver's mapping is used for the application too. */

    if (!region->pvAddr) {
        uncachedMapType = (region->memoryType & NEXUS_MEMORY_TYPE_DRIVER_UNCACHED) ? NEXUS_MemoryMapType_eUncached : NEXUS_MemoryMapType_eFake;
        region->pvAddr = nexus_p_map_memory(region->offset, region->length, uncachedMapType);
        if (!region->pvAddr) return BERR_TRACE(NEXUS_UNKNOWN);
    }
    else {
        /* this should not happen */
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    if (!region->pvAddrCached) {
        if (g_mipsKernelMode) {
        /* do driver mapping, don't do application mapping */
        cachedMapType = (region->memoryType & NEXUS_MEMORY_TYPE_DRIVER_CACHED) ? NEXUS_MemoryMapType_eCached : NEXUS_MemoryMapType_eFake;
        }
        else {
        /* in user mode, do driver or application mapping */
        cachedMapType = (region->memoryType & NEXUS_MEMORY_TYPE_APPLICATION_CACHED || region->memoryType & NEXUS_MEMORY_TYPE_DRIVER_CACHED) ? NEXUS_MemoryMapType_eCached : NEXUS_MemoryMapType_eFake;
        }
        if (cachedMapType == uncachedMapType && uncachedMapType == NEXUS_MemoryMapType_eFake) {
            /* reuse the fake mapping so we don't exhaust fake addressing */
            region->pvAddrCached = region->pvAddr;
        }
        else {
            region->pvAddrCached = nexus_p_map_memory(region->offset, region->length, cachedMapType);
            if (!region->pvAddrCached) return BERR_TRACE(NEXUS_UNKNOWN);
        }
    }
    else {
        /* region->pvAddrCached may have already been mapped by app. see NEXUS_Platform_CreateHeap. */
    }

    /* on sucess, neither address can be zero */
    BDBG_ASSERT(region->pvAddr);
    BDBG_ASSERT(region->pvAddrCached);
    return 0;
}

static void NEXUS_Platform_P_UnmapRegion(NEXUS_Core_MemoryRegion *region)
{
    if (region->pvAddr) {
        nexus_p_unmap_memory(region->pvAddr, region->length);
    }
    if (region->pvAddrCached && region->pvAddrCached != region->pvAddr) {
        nexus_p_unmap_memory(region->pvAddrCached, region->length);
    }
    region->pvAddr = NULL;
    region->pvAddrCached = NULL;
}

/* given pMemory->memc[].length, compute pMemory->memc[].region[].base and .length
the physical address layout is fixed per chip type, so this should be in common core code, not per-platform code. */
NEXUS_Error NEXUS_Platform_P_CalcSubMemc(NEXUS_PlatformMemory *pMemory)
{
    unsigned memcIndex;
    for (memcIndex = 0; memcIndex < NEXUS_MAX_MEMC; memcIndex++) {
        unsigned length = pMemory->memc[memcIndex].length;
        unsigned i;
        
#if BMIPS5000_40NM || BMIPS4380_40NM
        if (memcIndex == 0) {
            if (length > 0x80000000) { /* 2GB limit in physical addressing scheme */
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
            else if (length > 0x10000000) {
                pMemory->memc[memcIndex].region[0].base = 0;
                pMemory->memc[memcIndex].region[0].length = 0x10000000;
                pMemory->memc[memcIndex].region[1].base = 0x20000000;
                pMemory->memc[memcIndex].region[1].length = length - 0x10000000;
            }
            else {
                pMemory->memc[memcIndex].region[0].base = 0;
                pMemory->memc[memcIndex].region[0].length = length;
            }
        }
        else {
            pMemory->memc[memcIndex].region[0].base = 0x90000000;
            pMemory->memc[memcIndex].region[0].length = length;
        }
#elif BMIPS5000_65NM
        if (memcIndex == 0) {
            if (length >= 0x50000000) { /* 1.25 GB limit in physical addressing scheme */
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
            else if (length > 0x10000000) {
                pMemory->memc[memcIndex].region[0].base = 0;
                pMemory->memc[memcIndex].region[0].length = 0x10000000;
                pMemory->memc[memcIndex].region[1].base = 0x20000000;
                pMemory->memc[memcIndex].region[1].length = length - 0x10000000;
            }
            else {
                pMemory->memc[memcIndex].region[0].base = 0;
                pMemory->memc[memcIndex].region[0].length = length;
            }
        }
        else {
            pMemory->memc[memcIndex].region[0].base = 0x60000000;
            pMemory->memc[memcIndex].region[0].length = length;
        }
#else
        if (memcIndex == 0) {
            if (length >= 0x50000000) { /* 1.25 GB limit in physical addressing scheme */
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
            else if (length > 0x10000000) {
                pMemory->memc[memcIndex].region[0].base = 0;
                pMemory->memc[memcIndex].region[0].length = 0x10000000;
                pMemory->memc[memcIndex].region[1].base = 0x20000000;
                pMemory->memc[memcIndex].region[1].length = length - 0x10000000;
            }
            else {
                pMemory->memc[memcIndex].region[0].base = 0;
                pMemory->memc[memcIndex].region[0].length = length;
            }
        }
        #if (NEXUS_MAX_MEMC > 1)
        else if (memcIndex == 1) {
            pMemory->memc[memcIndex].region[0].base = 0x60000000;
            pMemory->memc[memcIndex].region[0].length = length;
        }
        #if (NEXUS_MAX_MEMC > 2)
        else {
            pMemory->memc[memcIndex].region[0].base = 0x70000000;
            pMemory->memc[memcIndex].region[0].length = length;
        }
        #endif
        #endif
#endif

        /* preserve original values so MRC can guard memory above/below nexus allocations */
        for (i=0;i<NEXUS_NUM_MEMC_REGIONS;i++) {
            pMemory->memc[memcIndex].region[i].original_base = pMemory->memc[memcIndex].region[i].base;
            pMemory->memc[memcIndex].region[i].original_length = pMemory->memc[memcIndex].region[i].length;
        }
    }
    return 0;
}

NEXUS_Error NEXUS_Platform_P_ConvertFromPhysicalAddress( const NEXUS_PlatformMemory *pMemory, unsigned offset, unsigned *pMemc, unsigned *pMemcOffset )
{
    unsigned i,j;
    for (i=0;i<NEXUS_MAX_MEMC;i++) {
        unsigned regionOffset = 0;
        for (j=0;j<NEXUS_NUM_MEMC_REGIONS;j++) {
            if (pMemory->memc[i].region[j].length &&
                offset >= pMemory->memc[i].region[j].base &&
                offset < pMemory->memc[i].region[j].base + pMemory->memc[i].region[j].length)
            {
                *pMemc = i;
                /* the MEMC offset includes this address's offset from its region's base, plus
                this region's offset from its MEMC base */
                *pMemcOffset = offset - pMemory->memc[i].region[j].base + regionOffset;
                return 0;
            }
            regionOffset += pMemory->memc[i].region[j].length;
        }
    }
    BDBG_ERR(("Unable to convert offset %#x", offset));
    return BERR_TRACE(NEXUS_INVALID_PARAMETER);
}

void NEXUS_Platform_GetDefaultCreateHeapSettings( NEXUS_PlatformCreateHeapSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/* g_runtimeHeaps[].heap mirrors g_pCoreHandles->nexusHeap[], but only for runtime-created heaps */
static struct {
    NEXUS_HeapHandle heap;
    NEXUS_Core_MemoryRegion region;
    NEXUS_PlatformCreateHeapSettings settings;
} g_runtimeHeaps[NEXUS_MAX_HEAPS];

NEXUS_HeapHandle NEXUS_Platform_CreateHeap( const NEXUS_PlatformCreateHeapSettings *pSettings )
{
    NEXUS_HeapHandle heap;
    unsigned i;
    unsigned unused = NEXUS_MAX_HEAPS;
    NEXUS_Core_MemoryRegion *region;
    NEXUS_Error rc;

    /* param validation */
    if (pSettings->offset + pSettings->size <= pSettings->offset) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    /* verify that new heap has no offset or userAddress overlap with existing heaps */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        heap = g_pCoreHandles->nexusHeap[i];
        if (heap) {
            NEXUS_MemoryStatus status;
            rc = NEXUS_Heap_GetStatus(heap, &status);
            if (rc) {
                rc = BERR_TRACE(rc);
                return NULL;
            }
            if (pSettings->offset >= status.offset && pSettings->offset + pSettings->size <= status.offset + status.size) {
                BDBG_ERR(("NEXUS_Platform_CreateHeap: runtime heap's offset cannot overlap existing heap"));
                return NULL;
            }
            if (pSettings->userAddress >= status.addr && (uint8_t*)pSettings->userAddress + pSettings->size <= (uint8_t*)status.addr + status.size) {
                BDBG_ERR(("NEXUS_Platform_CreateHeap: runtime heap's address cannot overlap existing heap"));
                return NULL;
            }
        }
        else if (unused == NEXUS_MAX_HEAPS) {
            unused = i;
        }
    }
    if (unused == NEXUS_MAX_HEAPS) {
        BDBG_ERR(("must increase NEXUS_MAX_HEAPS"));
        return NULL;
    }
    i = unused;
    BDBG_ASSERT(!g_runtimeHeaps[i].heap);
    g_runtimeHeaps[i].settings = *pSettings;

    /* we can create the heap now */
    region = &g_runtimeHeaps[i].region;
    BKNI_Memset(region, 0, sizeof(*region));
    region->memcIndex = 0;
    region->offset = pSettings->offset;
    region->length = pSettings->size;
    region->memoryType = pSettings->memoryType;
    region->alignment = pSettings->alignment;
    region->pvAddrCached = pSettings->userAddress;
    region->locked = pSettings->locked;

    rc = NEXUS_Platform_P_MapRegion(i, region);
    if (rc) {
        rc = BERR_TRACE(rc);
        return NULL;
    }

    heap = NEXUS_Heap_Create(i, g_pCoreHandles->mem, region);
    if (!heap) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    g_runtimeHeaps[i].heap = heap;

    return heap;
}

void NEXUS_Platform_DestroyHeap( NEXUS_HeapHandle heap )
{
    unsigned i;
    /* only allow heaps created with NEXUS_Platform_CreateHeap to be destroyed here */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_runtimeHeaps[i].heap == heap) {
            BDBG_ASSERT(heap == g_pCoreHandles->nexusHeap[i]);
            NEXUS_Heap_Destroy(heap);
            if (g_runtimeHeaps[i].settings.userAddress) {
                /* don't unmap what the user mapped */
                g_runtimeHeaps[i].region.pvAddrCached = NULL;
            }
            NEXUS_Platform_P_UnmapRegion(&g_runtimeHeaps[i].region);
            g_runtimeHeaps[i].heap = NULL;
            return;
        }
    }
    BDBG_ERR(("NEXUS_Platform_DestroyHeap: %p was not created with NEXUS_Platform_CreateHeap", heap));
}
