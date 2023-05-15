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
* $brcm_Workfile: nexus_platform_97429.c $
* $brcm_Revision: 6 $
* $brcm_Date: 7/25/12 1:59p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97429/src/nexus_platform_97429.c $
* 
* 6   7/25/12 1:59p katrep
* SW7429-150:start graphics heap @512M and set the size upto 256MB
* 
* 5   2/8/12 4:24p katrep
* SW7429-74:update RAAGA_1 RTS for VP6 decode
* 
* 4   2/2/12 10:18a katrep
* SW7429-1:chnnged the platfrom info message to warning
* 
* 3   12/16/11 3:10p katrep
* SW7429-1:enabled streamer iniput 1 for 7429
* 
* 2   9/22/11 5:14p katrep
* SW7429-1:add support for 7429 & bondouts
* 
* 1   8/9/11 8:01p katrep
* SW7429-1:initial checkin
* 
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_memc_arb_0.h"

BDBG_MODULE(nexus_platform_97429);

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
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = -1; /* initialized below */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    
    /* now set the defaults for XVD */
#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(0, &pSettings->videoDecoderModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE;
#endif

#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_DisplaySettings(0, &pSettings->displayModuleSettings);
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size += NEXUS_DISPLAY_UMA32_HEAP_SIZE;
#endif

    /* allocate little extra than we need */
    if (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size != -1) {
        pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size +=1;
        pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size/1000000)*1024*1024;
    }

	/*Give graphics heap 256MB when possible. Make sure doens't cross 256MB boundary, set index to 1 to start at 512MB address*/
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = pMemory->memc[0].region[1].length >= ((unsigned)(256*1024*1024) + (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size)) ? 
													(256*1024*1024):(pMemory->memc[0].region[1].length) - (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size);
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */


#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
    BDBG_WRN(("Picture heap %d (xvd %d, vdc %d), total heap size %d",
              pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size,
              NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE,NEXUS_DISPLAY_UMA32_HEAP_SIZE,
			  pMemory->memc[0].region[1].length));
#endif
	BDBG_WRN(("Graphics heap %d, total heap size %d",
              pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size,pMemory->memc[0].region[1].length));
    
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

    default:
        BDBG_ERR(("Invalid Memory controller configuration = %x", memoryMode));
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

    BSTD_UNUSED(regData);
#if NEXUS_HAS_DVB_CI
    g_pMemoryEbi = NEXUS_Platform_P_MapMemory(NEXUS_DVB_CI_MEMORY_BASE, NEXUS_DVB_CI_MEMORY_LENGTH, false);
    if (!g_pMemoryEbi) { BERR_TRACE(BERR_OS_ERROR); return 1; }
    NEXUS_P_AddMap(NEXUS_DVB_CI_MEMORY_BASE, NULL, g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
#endif
    BDBG_WRN(("Initializing platform... %d\n", PLATFORM ));
    #if (defined BDSP_VP6_SUPPORT || defined BDSP_H264_ENCODE_SUPPORT)
    regData = BREG_Read32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_51);
    regData &= ~(BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_51, RR_EN)|
                 BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_51, BO_VAL));
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_51, RR_EN, 1);        /* Enable round-robin */
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_51, BO_VAL, 0xd8);    /* 1us blockout */
    BREG_Write32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_51, regData);
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
