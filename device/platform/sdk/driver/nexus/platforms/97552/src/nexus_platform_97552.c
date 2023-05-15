/***************************************************************************
*     (c)2010-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_97552.c $
* $brcm_Revision: 7 $
* $brcm_Date: 7/26/12 6:00p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97552/src/nexus_platform_97552.c $
* 
* 7   7/26/12 6:00p xhuang
* SW7552-299: rework heap allocation to match nexus update for 256MB
*  kernel memory case
* 
* 6   7/5/12 5:43p xhuang
* SW7552-299: rework heap allocation to support 256MB kernel memory in
*  512MB total memory
* 
* 5   3/30/12 6:36p xhuang
* SW7552-95: support memory beyond 256M
* 
* 4   3/23/12 3:59p xhuang
* SW7552-218: return off screen heap for DirectFB
* 
* 3   8/8/11 1:24p xhuang
* SW7552-77: Add DVB-CI support
* 
* 2   8/4/11 1:56p xhuang
* SW7552-25: fix bug for 512M memory detect
* 
* 1   1/25/11 6:13p xhuang
* SW7552-25: Initial version
* 
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr_0.h"

BDBG_MODULE(nexus_platform_97552);

#if NEXUS_HAS_DVB_CI
static void *g_pMemoryEbi;
#endif

#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    /* For 7552, only 16bit mode usage */
    if(pSettings->heapSize[0].general || pSettings->heapSize[0].secure || pSettings->heapSize[0].picture )
    {
        BDBG_MSG(("Using User Defined Video Decoder Heaps"));
    }
    else
    {
        pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_UMA16_GENERAL_HEAP_SIZE;
        pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_UMA16_SECURE_HEAP_SIZE;
        pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE;
        pSettings->avdHeapIndex[0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
        pSettings->mfdMapping[0] = 0; 
        pSettings->avdMapping[0] = 0;
    }
}
#endif

#if NEXUS_HAS_DISPLAY
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_DisplaySettings(NEXUS_DisplayModuleSettings *pSettings)
{
    if(pSettings->sdBuffers.count || pSettings->hdBuffers.count || pSettings->fullHdBuffers.count)
    {
        BDBG_MSG(("Using User Defined Display Buffers"));
        return;
    }

    pSettings->primaryDisplayHeapIndex = NEXUS_MEMC0_MAIN_HEAP;

    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA16;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].fullHdBuffers.pipCount = NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_UMA16;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].hdBuffers.pipCount = NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_UMA16;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].sdBuffers.pipCount = NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_UMA16;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].hdBuffers.format = NEXUS_VideoFormat_e1080i;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].sdBuffers.format = NEXUS_VideoFormat_ePalG;
    pSettings->displayHeapSettings[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    
    pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
}
#endif

void NEXUS_Platform_P_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    NEXUS_PlatformMemory *pMemory = &g_platformMemory;
    bool region0Available = (pMemory->osRegion[0].base >= 512*1024*1024) ? false : true;

	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = region0Available ? 0 : 1;
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;	/* use all the available memory not reserved by kernel */
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

	pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
#if (NEXUS_UPPER_MEMORY_SUPPORT == 1)
	/* bmem=192M@64M bmem=256M@512M for the boards with 512 Mbytes of memory */
	pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
#else
    /* bmem=192M@64M */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 0;
#endif
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 0; /* initialized below */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

  /* Below the memory is being set for XVD and VDC according to the Memory Spreadsheet. Please calculate these numbers for your platform.  
     Customer Platforms will have different Memory needs. */
#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += (NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE);
#endif

#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_DisplaySettings(&pSettings->displayModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_DISPLAY_UMA16_HEAP_SIZE;
#endif

    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += 1661952;  
    /* must 4k align */
    if(pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size%4096)
    {
        pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += (4096-pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size % 4096);
    }

#if (NEXUS_UPPER_MEMORY_SUPPORT == 1)
    if(region0Available)
    {
        pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
        pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
        pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = -1;  /* remaining memory */
        pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */
    }
#endif

#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
    BDBG_WRN(("Picture heap %d (xvd %d, vdc %d)",
              pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size,
              NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE,
              NEXUS_DISPLAY_UMA16_HEAP_SIZE));
#endif
}

/* read memoryMode & memSizes[].
this function does not know memc offsets.
this function is needed for memc's that linux doesn't report (e.g. non-UMA 7405 MEMC1)
*/
NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(BREG_Handle hReg, NEXUS_PlatformMemory *pMemory, NEXUS_PlatformSettings *pSettings)
{
    uint32_t regValue;
    uint32_t memc0_config=1;
    uint32_t ddr0Device;
    unsigned memSizes[NEXUS_NUM_MEMC];
    unsigned memoryMode;

    BSTD_UNUSED(pSettings);

    BKNI_Memset(pMemory, 0, sizeof(*pMemory));
    BKNI_Memset(memSizes, 0, sizeof(*memSizes));

    /* TODO: 7552 may only have 16bit mode, but remain the code in case we have 8bit usage 
     * Read DDR parameters
     * DRAM Width: 0 (32 bit), 1 (16 bit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
    memc0_config = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG, DRAM_WIDTH);

    /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit), 5 (8Gbit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
    ddr0Device = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

    /* MEMC0 configured in 32b mode */
    if (memc0_config == 0)
    {
        memoryMode = 0; /* 32b configuration */
     }
    /* Single decode - MEMC0 configured in 16b mode */
    else
    {
        memoryMode = 1; /* 16b configuration */
    }

    switch (memoryMode)
    {
    case 0: /* 32bit */
        break;
    case 1: /* 16bit */
        switch(ddr0Device)
        {
        case 0: /* 256 Mbits Device Tech*/
            memSizes[0] = 32*1024*1024;
            break;
        case 1: /* 512 Mbits Device Tech*/
            memSizes[0] = 64*1024*1024;
            break;
        case 2: /* 1024 Mbits Device Tech*/
            memSizes[0] = 128*1024*1024;
            break;
        case 3: /* 2048 Mbits Device Tech*/
            memSizes[0] = 256*1024*1024;
            break;
        case 4: /* 4096 Mbits Device Tech*/
            memSizes[0] = 512*1024*1024;
            break;
        case 5: /* 8192 Mbits Device Tech*/
            memSizes[0] = 1024*1024*1024;
            break;
        default:
            BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        break;
    default:
        BDBG_ERR(("Invalid Memory controller configuration = %x", memoryMode));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    BDBG_MSG(("Memory indicates: MEMC0: %d MB", memSizes[0] >> 20));

    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];

    return 0;
}

/***************************************************************************
Summary:
    Based on the RTS settings for each platform, framebuffer for each display
    could be placed on any heaps. This API shall return the heap handle
    for each frame buffer.
See Also:
    NEXUS_Platform_P_GetFramebufferHeap
 ***************************************************************************/
NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex)
{
    NEXUS_HeapHandle heapHandle=NULL;
    switch (displayIndex) {
    case 0: /* HD Display */
    case 1: /* SD1 Display */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
        break;
    case NEXUS_OFFSCREEN_SURFACE:
#if (NEXUS_UPPER_MEMORY_SUPPORT == 1)   
        if(g_platformMemory.memc[0].region[0].length)
        {
            heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP];
        }
        else
        {
            heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
        }
#else
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
#endif
        break;
    default:
        BDBG_MSG(("Invalid display index %d",displayIndex));
        break;
    }
    return  heapHandle;
}

NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
#if NEXUS_HAS_DVB_CI
    g_pMemoryEbi = NEXUS_Platform_P_MapMemory(NEXUS_DVB_CI_MEMORY_BASE, NEXUS_DVB_CI_MEMORY_LENGTH, false);
    if (!g_pMemoryEbi) { BERR_TRACE(BERR_OS_ERROR); return NEXUS_OS_ERROR; }
    NEXUS_P_AddMap(NEXUS_DVB_CI_MEMORY_BASE, NULL, g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
#endif
    return 0;
}

void NEXUS_Platform_P_UninitBoard(void)
{
#if NEXUS_HAS_DVB_CI
    if (g_pMemoryEbi) {
        NEXUS_Platform_P_UnmapMemory(g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
    }
#endif
}
