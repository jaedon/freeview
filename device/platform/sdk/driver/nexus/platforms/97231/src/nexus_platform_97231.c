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
* $brcm_Workfile: nexus_platform_97231.c $
* $brcm_Revision: 26 $
* $brcm_Date: 10/25/12 9:53a $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97231/src/nexus_platform_97231.c $
* 
* 26   10/25/12 9:53a randyjew
* SW7231-882: Fix  graphics heap calculations when 2nd bmem region less
*  than 256 MB
* 
* 25   10/16/12 11:03a gmohile
* SW7231-1037 : Fix client 51 RTS for raaga video encoder
* 
* 24   7/17/12 2:40p randyjew
* SW7231-882: fix warning
* 
* 23   7/17/12 1:56p randyjew
* SW7231-882: Make graphics heap 256 MB when possible
* 
* 22   7/12/12 3:40p randyjew
* SW7231-882: Make sure Graphics heap doesn't cross 256 boundary.
* 
* 21   1/16/12 6:06p gmohile
* SW7425-1473 : Merge raaga encoder support
* 
* 19   12/5/11 9:41a jrubio
* SW7231-507: fix A0 Support
* 
* 18   11/30/11 9:58a jrubio
* SW7231-451: adjust memory config
* 
* 17   11/23/11 10:25a gmohile
* SW7231-485 : Update avd pll settings
* 
* 16   11/22/11 3:47p jgarrett
* SW7425-1823: Setting RTS values for video operations on Raaga
* 
* 15   8/29/11 11:12a katrep
* SW7231-143:fixed coverity issue
* 
* 14   8/9/11 6:35p katrep
* SW7231-334:integrated svb-ci code from Ashwin
* 
* 13   6/28/11 5:38p katrep
* SW7231-199:updated for flash support
* 
* 12   5/27/11 10:44a katrep
* SW7231-143:fixed some minor errors
* 
* 11   5/18/11 9:20a katrep
* SW7231-143:fixed issue with the memc0-#1 heap
* 
* 10   5/16/11 1:22p katrep
* SW7231-143:removed hardcoded heap creation
* 
* 9   4/14/11 6:03p katrep
* SW7231-4:updated default memory configuration for 97230
* 
* 8   4/12/11 2:03p katrep
* SW7231-4:97230 has only 512M memory, limit the heap creation
* 
* 7   3/9/11 3:36p katrep
* SW7231-49:add support for second display HDSD_SINGLE=1
* 
* 6   1/10/11 4:53p katrep
* SW7231-4:add 16 UMA required for 7230
*
* 5   1/3/11 11:25a erickson
* SW7422-2: fix indenting
*
* 4   12/30/10 5:56p erickson
* SW7422-2: update mmap
*
* 3   12/30/10 1:31p erickson
* SW7422-2: update mmap
*
* 2   12/22/10 4:49p erickson
* SW7422-2: adjustments
*
* 1   12/22/10 4:07p katrep
* SW7231-26:add support for new heap configuration, add support for 7230
*
*
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_clkgen.h"

#if BCHP_VER >= BCHP_VER_B0 && (defined BDSP_VP6_SUPPORT || defined BDSP_H264_ENCODE_SUPPORT)
#include "bchp_memc_arb_0.h"
#endif

BDBG_MODULE(nexus_platform_97231);

#if NEXUS_HAS_DVB_CI
static void *g_pMemoryEbi;
#endif
#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(unsigned memoryMode, NEXUS_VideoDecoderModuleSettings *pSettings)
{
    switch (memoryMode) {
    case 0: /* 32b */
        /* By convention the first XVD device is the SVD0 decoder which has its picture */
        pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_UMA32_GENERAL_HEAP_SIZE;
        pSettings->heapSize[0].secure =  0;
        pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE;
        break;

    case 1: /* 16b */
        /* By convention the first XVD device is the SVD0 decoder which has its picture */
        pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_UMA16_GENERAL_HEAP_SIZE;
        pSettings->heapSize[0].secure =  0;
        pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE;
        break;
    }
    pSettings->avdHeapIndex[0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->mfdMapping[0]= 0; /* SVD is mapped to MPEG feeder 0 */
    pSettings->avdMapping[0]=0;
    pSettings->avdMapping[1]=0;
}
#endif

#if NEXUS_HAS_DISPLAY
static void NEXUS_Platform_P_DisplaySettings(unsigned memoryMode, NEXUS_DisplayModuleSettings *pSettings)
{
    unsigned i;
    pSettings->primaryDisplayHeapIndex = NEXUS_MEMC0_MAIN_HEAP;

    for (i=0;i<NEXUS_MAX_HEAPS;i++ ) {

        switch (i) 
        {
        case NEXUS_MEMC0_PICTURE_BUFFER_HEAP:
            switch (memoryMode) {
            case 0: /* 32 bit */
                pSettings->displayHeapSettings[i].fullHdBuffers.count =     NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA32;
                pSettings->displayHeapSettings[i].hdBuffers.count =         NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA32;
                pSettings->displayHeapSettings[i].sdBuffers.count =         NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA32;
                pSettings->displayHeapSettings[i].fullHdBuffers.pipCount =  0;
                pSettings->displayHeapSettings[i].hdBuffers.pipCount =      0;
                pSettings->displayHeapSettings[i].sdBuffers.pipCount =      0;
                break;
            case 1: /* 16 bit */
                pSettings->displayHeapSettings[i].fullHdBuffers.count =     NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA16;
                pSettings->displayHeapSettings[i].hdBuffers.count =         NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16;
                pSettings->displayHeapSettings[i].sdBuffers.count =         NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16;
                pSettings->displayHeapSettings[i].fullHdBuffers.pipCount =  0;
                pSettings->displayHeapSettings[i].hdBuffers.pipCount =      0;
                pSettings->displayHeapSettings[i].sdBuffers.pipCount =      0;
                break;
            }
            break;
        default:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = 0;
            pSettings->displayHeapSettings[i].hdBuffers.count = 0;
            pSettings->displayHeapSettings[i].sdBuffers.count = 0;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = 0;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = 0;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = 0;
            break;
        }
        pSettings->displayHeapSettings[i].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
        pSettings->displayHeapSettings[i].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].hdBuffers.format = NEXUS_VideoFormat_e1080i;
        pSettings->displayHeapSettings[i].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
        pSettings->displayHeapSettings[i].sdBuffers.format = NEXUS_VideoFormat_ePalG;
        pSettings->displayHeapSettings[i].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    }

    pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[0][1] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][1] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
}
#endif

void NEXUS_Platform_P_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    /* kernel suggested boot options bmem=192M@64M bmem=512M@512M for boards with >750M memory 
       bmem=192M@64M bmem=256M@512M for the boards with 512 Mbytes of memory */
    const NEXUS_PlatformMemory *pMemory = &g_platformMemory; /* g_platformMemory is completely initialized already */

    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;   /* use all the available memory not reserved by kernel */
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;


    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 0; /* initialized below */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* now set the defaults for XVD */
#if NEXUS_HAS_VIDEO_DECODER
    #if NEXUS_PLATFORM == 97230
    NEXUS_Platform_P_VideoDecoderSettings(1, &pSettings->videoDecoderModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE;
    #else
    NEXUS_Platform_P_VideoDecoderSettings(0, &pSettings->videoDecoderModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE;
    #endif
#endif

#if NEXUS_HAS_DISPLAY
    #if NEXUS_PLATFORM == 97230
    NEXUS_Platform_P_DisplaySettings(1, &pSettings->displayModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_DISPLAY_UMA16_HEAP_SIZE;
    #else
    NEXUS_Platform_P_DisplaySettings(0, &pSettings->displayModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_DISPLAY_UMA32_HEAP_SIZE;
    #endif
#endif

    /* allocate little extra than we need */
    if (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size != -1) {
        pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size +=1;
        pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size/1000000)*1024*1024;
    }

	
    BDBG_WRN(("pMemory->osRegion[0].length %d MB, pMemory->osRegion[1].length %d MB	  ",pMemory->osRegion[0].length/1024 /1024,pMemory->osRegion[1].length /1024 /1024));
	BDBG_WRN(("pMemory->osRegion[0].length %d MB, pMemory->osRegion[1].length %d MB	  ",pMemory->osRegion[0].length/1024 /1024,pMemory->osRegion[1].length /1024 /1024));
	
	 /*heap checks*/
	if((unsigned)pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size > pMemory->osRegion[1].length)
		BDBG_ERR(("Picture heap bigger than BMEM region[1] (%d MB > %d MB)",pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size /1024 /1024, pMemory->osRegion[1].length /1024 /1024  ));

	/*Give graphics heap 256MB when possible. Make sure doens't cross 256MB boundary, set index to 1 to start at 512MB address 768*/
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
	
	if(pMemory->osRegion[1].length >=((unsigned)(256*1024*1024) + (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size)))
    	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = (256*1024*1024);
	else if(pMemory->osRegion[1].length > (unsigned)(pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size))
		pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = pMemory->osRegion[1].length - (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size) ;
	else  
		pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 0;
	
	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

	 BDBG_WRN(("Graphics heap %d ",
              pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size));

#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
#if NEXUS_PLATFORM == 97230
    BDBG_WRN(("Picture heap %d (xvd %d, vdc %d)",
              pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size,
              NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE,
              NEXUS_DISPLAY_UMA16_HEAP_SIZE)); 
#else
    BDBG_WRN(("Picture heap %d (xvd %d, vdc %d)",
              pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size,
              NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE,
              NEXUS_DISPLAY_UMA32_HEAP_SIZE));
#endif
#endif
    
}


NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(BREG_Handle hReg, NEXUS_PlatformMemory *pMemory, NEXUS_PlatformSettings *pSettings)
{
    uint32_t regValue;
    unsigned memoryMode;
    unsigned memSizes[NEXUS_NUM_MEMC];
    uint32_t memc0_config=1;
    uint32_t ddr0Device=0;
    unsigned memcWidth;

    BSTD_UNUSED(pSettings);
    BKNI_Memset(pMemory, 0, sizeof(*pMemory));
    BKNI_Memset(memSizes, 0, sizeof(*memSizes));

    /* Read DDR parameters
     * DRAM Width: 0 (32 bit), 1 (16 bit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
    memc0_config = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG, DRAM_WIDTH);

    /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit)
     * Should be 4 for 7422 configuration */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
    ddr0Device = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

    /* 7422 SVC decode requires 32b (memc0 )
       Other usage cases may permit a 16b (memc0) configuration */
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
    case 0: /* 32b */
        memcWidth = 32;
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

        break;

    case 1: /* 16b */
        memcWidth = 16;
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
        break;
    }

    BDBG_MSG(("Memory indicates: MEMC0: %d MB, %d bit", memSizes[0] >> 20, memcWidth));
    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];
#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
    g_NEXUS_platformHandles.config.numWindowsPerDisplay =  NEXUS_NUM_VIDEO_DECODERS;
    g_NEXUS_platformHandles.config.supportedDecoder[0] = true;
    g_NEXUS_platformHandles.config.supportedDecoder[1] = true;
    g_NEXUS_platformHandles.config.supportedDisplay[0] = true;
    g_NEXUS_platformHandles.config.supportedDisplay[1] = true;
#endif

    return 0;
}

NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex)
{
#if 1
    BSTD_UNUSED(displayIndex);
    if (g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP]) {
        return g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP]; 
    }
    else {
        return g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
    }
#else
    NEXUS_HeapHandle heapHandle=NULL;
    switch (displayIndex) {
    case 0: /* HD Display */
    case 1: /* SD1 Display */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
        break;
    default:
        BDBG_MSG(("Invalid display index %d",displayIndex));
       break;
    }
    return  heapHandle;
#endif
}

NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
    uint32_t regData;
#if NEXUS_HAS_DVB_CI
    BDBG_ERR(("NEXUS_Platform_P_InitBoard++++++++++\n"));
    g_pMemoryEbi = NEXUS_Platform_P_MapMemory(NEXUS_DVB_CI_MEMORY_BASE, NEXUS_DVB_CI_MEMORY_LENGTH, false);
    if (!g_pMemoryEbi) { BERR_TRACE(BERR_OS_ERROR); return 1; }
    NEXUS_P_AddMap(NEXUS_DVB_CI_MEMORY_BASE, NULL, g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
#endif
    
    regData = BREG_Read32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_DIV);
    regData &= ~(BCHP_MASK(CLKGEN_PLL_AVD_PLL_DIV, PDIV));
    regData &= ~(BCHP_MASK(CLKGEN_PLL_AVD_PLL_DIV, NDIV_INT));
    regData |= BCHP_FIELD_DATA(CLKGEN_PLL_AVD_PLL_DIV, PDIV, 0x3);
    regData |= BCHP_FIELD_DATA(CLKGEN_PLL_AVD_PLL_DIV, NDIV_INT, 0x98);
    BREG_Write32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_DIV, regData);
    
    BREG_AtomicUpdate32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_0,
			BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_0_MDIV_CH0_MASK, 0x7 << BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_0_MDIV_CH0_SHIFT);
    BREG_AtomicUpdate32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_1,
			BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_1_MDIV_CH1_MASK, 0x9 << BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_1_MDIV_CH1_SHIFT);
    BREG_AtomicUpdate32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_2,
			BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_MASK, 0x5 << BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_2_MDIV_CH2_SHIFT);
    BREG_AtomicUpdate32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_3,
			BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_MASK, 0x9 << BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_3_MDIV_CH3_SHIFT);
#if BCHP_VER >= BCHP_VER_B0
    BREG_AtomicUpdate32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_4,
			BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_4_MDIV_CH4_MASK, 0x8 << BCHP_CLKGEN_PLL_AVD_PLL_CHANNEL_CTRL_CH_4_MDIV_CH4_SHIFT);
#endif    

#if BCHP_VER >= BCHP_VER_B0 && (defined BDSP_VP6_SUPPORT || defined BDSP_H264_ENCODE_SUPPORT)
    regData = BREG_Read32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_51);
    regData &= ~(BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_51, RR_EN)|
                 BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_51, BO_VAL));
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_51, RR_EN, 0);        /* Enable round-robin */
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_51, BO_VAL, 0x11e);    /* 1us blockout */
    BREG_Write32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_51, regData);

    regData = BREG_Read32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_49);
    regData &= ~(BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_49, BO_VAL));    
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_49, BO_VAL, 0x230);    /* 2us blockout */
    BREG_Write32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_49, regData);
#else
    BSTD_UNUSED(regData);
#endif
    return NEXUS_SUCCESS;
}

void NEXUS_Platform_P_UninitBoard(void)
{
#if NEXUS_HAS_DVB_CI
    if (g_pMemoryEbi) {
        NEXUS_Platform_P_UnmapMemory(g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
    }
#endif
}
