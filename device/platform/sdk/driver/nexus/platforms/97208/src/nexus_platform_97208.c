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
* $brcm_Workfile: nexus_platform_97208.c $
* $brcm_Revision: 9 $
* $brcm_Date: 8/31/12 10:14a $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97208/src/nexus_platform_97208.c $
* 
* 9   8/31/12 10:14a erickson
* SW7435-341: remove NEXUS_GetEnv("main_memory_size") from platform code
* 
* 8   5/25/11 10:00a randyjew
* SW7422-445: Fix warning in ReadMemcConfig
* 
* 7   5/23/11 10:56a randyjew
* SW7422-445: Eliminate Memc indexes greater than 0.
* 
* 6   5/20/11 4:43p randyjew
* SW7422-445: Add NEXUS_Platform_P_GetDefaultSettings, set -1 for heap
*  sizes, use all available memory.
* 
* 5   1/11/11 1:38p randyjew
* SW7420-1078: Platforms Not requiring Heap control returns default heap
*  instead of NULL.
* 
* 4   1/3/11 11:52a randyjew
* SW7422-2:fix warning
* 
* 3   12/23/10 4:13p randyjew
* SW7422-2: fix warnings
* 
* 2   12/23/10 10:05a randyjew
* SW7422-2: remove  numMemc.
* 
* 1   12/22/10 4:54p randyjew
* SW7422-2:rework nexus_platform_core, add board-specific file and API,
*  mmap after determining heaps
* 
* 1   12/20/10 4:26p erickson
* SW7422-2: rework nexus_platform_core, add board-specific file and API,
*  mmap after determining heaps
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_clk.h"
#include "priv/nexus_core.h"
#include "bchp_memc_misc_0.h"
#include "bchp_memc_ddr_0.h"

BDBG_MODULE(nexus_platform_97208);

#if NEXUS_HAS_DVB_CI
static void *g_pMemoryEbi;
#endif

#ifdef NO_OS
#include "nexus_core_module.h"
#endif

#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    unsigned numDecodes=0;
    unsigned i;

    for(i=0;i<NEXUS_NUM_XVD_DEVICES;i++) {
        if(pSettings->heapSize[i].general || pSettings->heapSize[i].secure || pSettings->heapSize[i].picture )
        {
            BDBG_MSG(("Using User Defined Video Decoder[%u] Heaps", i));
            continue;
        }

          pSettings->heapSize[i].general = NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE;
            pSettings->heapSize[i].secure = NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE;
            pSettings->heapSize[i].picture = NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE;
            
          
    }
	numDecodes=NEXUS_NUM_VIDEO_DECODERS;
    if(!pSettings->numDecodes || !pSettings->numStillDecodes || !pSettings->numMosaicDecodes )
    {
        pSettings->numDecodes = numDecodes;
        /* TODO based on the memory modes */
        pSettings->numStillDecodes=NEXUS_NUM_STILL_DECODES;
        pSettings->numMosaicDecodes=NEXUS_NUM_MOSAIC_DECODES;
    }

    return;
}
#endif

#if NEXUS_HAS_DISPLAY
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_DisplaySettings(NEXUS_DisplayModuleSettings *pSettings)
{
    if(pSettings->sdBuffers.count || pSettings->hdBuffers.count || pSettings->fullHdBuffers.count)
    {
        BDBG_WRN(("Using User Defined Display Buffers"));
        return;
    }

    
	 pSettings->displayHeapSettings[0].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS;
        pSettings->displayHeapSettings[0].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS;
        pSettings->displayHeapSettings[0].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS;
        g_NEXUS_platformHandles.config.numWindowsPerDisplay = 1; /* no PIP */
        g_NEXUS_platformHandles.config.supportedDecoder[0] = false; /* no PIP */
    

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
    pSettings->heap[0].size = -1;
    pSettings->heap[0].memoryType = NEXUS_MemoryType_eFull;
#if 0 
    /*
     * Picture memory for XVD0
     */
    pSettings->heap[1].memcIndex = 1;
    pSettings->heap[1].subIndex = 0;
    pSettings->heap[1].size = -1;
    pSettings->heap[1].memoryType = NEXUS_MemoryType_eDeviceOnly;

    /*
     * Picture memory for XVD1
     */
    pSettings->heap[2].memcIndex = 2;
    pSettings->heap[2].subIndex = 0;
    pSettings->heap[2].size = -1;
    pSettings->heap[2].memoryType = NEXUS_MemoryType_eDeviceOnly;
#endif
    /* adjust module settings based on MEMC config */
#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
#endif
#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_DisplaySettings(&pSettings->displayModuleSettings);
#endif
   BDBG_MSG(("\n NEXUS_Platform_P_GetDefaultSettings <--"));
}




NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(BREG_Handle hReg, NEXUS_PlatformMemory *pMemory, NEXUS_PlatformSettings *pSettings)
{
    uint32_t regValue;
    unsigned memSizes[NEXUS_NUM_MEMC];
    uint32_t memc0_config=1;
    uint32_t ddr0Device=0;

    BSTD_UNUSED(pSettings);
    BKNI_Memset(pMemory, 0, sizeof(*pMemory));
    BKNI_Memset(memSizes, 0, sizeof(*memSizes));

#if(BCHP_VER >= BCHP_VER_B0)
    /* Read DDR parameters */
    /* DRAM Width: 1(32 bit), 2 (16 bit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_MISC_0_MEMC_STRAP_DDR_CONFIG);
    memc0_config = BCHP_GET_FIELD_DATA(regValue, MEMC_MISC_0_MEMC_STRAP_DDR_CONFIG, DDR_CONFIG);


    /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
    ddr0Device = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

#else
    memc0_config = 2;
    ddr0Device = 2;
#endif

     switch (memc0_config)
    {
    case 1:  /* 32b */
        {
           
            switch(ddr0Device)
            {
                case 0: /* 256 Mbits Device Tech*/
                    memSizes[0] = 64*1024*1024;
                    break;
                case 1: /* 512 Mbits Device Tech*/
                    memSizes[0] = 128*1024*1024;
                    break;
                case 2: /* 1024 Mbits Device Tech*/
                    memSizes[0] = 256*1024*1024;
                    break;
                case 3: /* 2048 Mbits Device Tech*/
                    memSizes[0] = 512*1024*1024;
                    break;
                case 4: /* 4096 Mbits Device Tech*/
                    memSizes[0] = 1024*1024*1024;
                    break;
            default:
                    BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
                    break;
            }
        }
        break;

    case 2: 
        {
            /* 16b */
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
                    memSizes[0] = 1024*1024*1024;
                    break;
            default:
                    BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
                    break;
            }

        }
        break;

    default:
        BDBG_ERR(("Invalid Memory controller ddr configuration = %x", memc0_config));
        break;
    }

#ifdef NEXUS_SECURITY_REGVER
    /* Reserve some memory for video FW */
#define VIDEO_FW_SIZE (1024*1024+64*1024)
    memSizes[0] -= VIDEO_FW_SIZE*2;
#endif

    BDBG_MSG(("Memory indicates: MEMC0: %d MB", memSizes[0] >> 20));
    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];

    return 0;
}


NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex)
{
    NEXUS_HeapHandle heapHandle=NULL; 
    BSTD_UNUSED(displayIndex);
    heapHandle = g_pCoreHandles->nexusHeap[0]; /* default heap for surface creation */
    return heapHandle;
}

NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
#if NEXUS_HAS_DVB_CI
    g_pMemoryEbi = NEXUS_Platform_P_MapMemory(NEXUS_DVB_CI_MEMORY_BASE, NEXUS_DVB_CI_MEMORY_LENGTH, false);
    if (!g_pMemoryEbi) { errCode = BERR_TRACE(BERR_OS_ERROR); goto err_ebi; }
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
