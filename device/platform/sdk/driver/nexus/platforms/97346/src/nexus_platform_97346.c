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
* $brcm_Workfile: nexus_platform_97346.c $
* $brcm_Revision: 18 $
* $brcm_Date: 7/12/12 3:55p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97346/src/nexus_platform_97346.c $
* 
* 18   7/12/12 3:55p randyjew
* SW7344-347: Make sure Graphics heap doesn't cross 256 boundary.
* 
* 17   3/27/12 2:54p randyjew
* SW7418-86: Add 7418SFF_RVU support
* 
* 16   3/15/12 12:49p randyjew
* SW7346-727: Revert back with extra padding until Memory Worksheet issue
*  resolved
* 
* 15   3/13/12 10:50a jrubio
* SW7346-727: modify nexus platform tuner
* 
* 14   1/12/12 10:17a jrubio
* SW7346-400: modify secure settings
* 
* 13   11/30/11 9:55a jrubio
* SW7346-470: fix memc0 picture heap
* 
* 12   11/26/11 1:22p agin
* SWNOOS-500:  Diags uses NEXUS_Platform_P_ReadMemcConfig before nexus is
*  initialized.
* 
* 11   11/22/11 4:43p jgarrett
* SW7425-1823: Setting RTS values for video operations on Raaga
* 
* 10   9/21/11 5:03p randyjew
* SW7418-39: Add external rfm for 7418sff_h
* 
* 9   9/15/11 11:32a jrubio
* SW7346-470: update memory configuration
* 
* 8   8/26/11 4:25p randyjew
* SW7418-25: Clean Up confusion on proper heap sizes.
* 
* 7   8/26/11 1:41p randyjew
* SW7418-25: Fix memSize  to 512MB in 16bit memory mode.
* 
* 6   8/25/11 2:00p jrubio
* SW7346-445: also add 1GB to 2g ddr3 parts
* 
* 5   8/12/11 2:35p jrubio
* SW7346-445: fix deadcode in switch statement
* 
* 4   7/15/11 10:35a jrubio
* SW7346-306: update code for GFX heap
* 
* 3   6/29/11 4:21p randyjew
* SW7418-13:Add NEXUS_HAS_DISPLAY to avoid compile error when building
*  with NEXUS_BASE_ONLY
* 
* 2   5/23/11 3:45p randyjew
* SW7231-128: Added BCHP_Handle parameter to BTST_VideoShmoo() to support
*  pwr managment. 7346/7344/7418
* 
* 1   5/23/11 2:54p jrubio
* SW7346-213: create 7346 main file
* 
* 8   5/23/11 2:51p jrubio
* SW7346-213: fix memory allocations, do not hardcode mem
* 
* 6   5/17/11 5:38p randyjew
* SW7425-466: overriding default MFD/AVD mappings for 7344/7418
* 
* 5   3/23/11 10:24a jrubio
* SW7344-35: exclude 7346
* 
* 4   3/22/11 4:56p randyjew
* SW7344-35: Add call to BTST_VideoShmoo to address VFD burst read issue
*  with 7344 a0
* 
* 3   2/11/11 2:50p jrubio
* SW7346-24: fix HDMI / composite issue
* 
* 2   1/13/11 1:20p garylin
* SW7344-18: fix compile error for DVB-CI
* 
* 1   1/4/11 4:49p jrubio
* SW7344-20: add HD+HD
* 
* 4   12/30/10 3:40p erickson
* SW7422-2: use per-window heaps in VDC
*
* 3   12/30/10 1:45p erickson
* SW7422-2: update mmap
*
* 2   12/22/10 3:12p erickson
* SW7422-2: adjustments
*
* 1   12/22/10 11:02a jrubio
* SW7422-2: add nexus platform core rework
*
*
* 1   12/20/10 5:29p erickson
* SW7422-2: rework nexus_platform_core
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "priv/nexus_core.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr_0.h"
#if (defined BCHP_REV_A0 && BCHP_CHIP!=7346)
#include "btst_video_shmoo.h"
#endif
#if BCHP_CHIP==7344
BDBG_MODULE(nexus_platform_97344);
#else
BDBG_MODULE(nexus_platform_97346);
#endif

#if BCHP_VER >= BCHP_VER_B0 && (defined BDSP_VP6_SUPPORT || defined BDSP_H264_ENCODE_SUPPORT)
#include "bchp_memc_arb_0.h"
#endif

#if NEXUS_HAS_DVB_CI
static void *g_pMemoryEbi;
#endif

#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(unsigned memoryMode, NEXUS_VideoDecoderModuleSettings *pSettings)
{
	switch (memoryMode) {
	case 0:	/* 32b */
		/* By convention the first XVD device is the SVD0 decoder which has its picture */
		pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_UMA32_GENERAL_HEAP_SIZE;
		pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_UMA32_SECURE_HEAP_SIZE;
		pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE;
		break;

	case 1:	/* 16b */
		/* By convention the first XVD device is the SVD0 decoder which has its picture */
		pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_UMA16_GENERAL_HEAP_SIZE;
		pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_UMA16_SECURE_HEAP_SIZE;
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
			case 0:	/* 32 bit */
				pSettings->displayHeapSettings[i].fullHdBuffers.count =     NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA32;
				pSettings->displayHeapSettings[i].hdBuffers.count =         NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA32;
				pSettings->displayHeapSettings[i].sdBuffers.count =         NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA32;
				pSettings->displayHeapSettings[i].fullHdBuffers.pipCount =  0;
				pSettings->displayHeapSettings[i].hdBuffers.pipCount =      0;
				pSettings->displayHeapSettings[i].sdBuffers.pipCount =      0;
				break;
			case 1:	/* 16 bit */
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


#if NEXUS_PLATFORM_7418SFF_RVU
	/* 7418SFF_RVU board only has a total of 256M on the board use:  bmem=192M@64M as boot options */
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;	/* use all the available memory not reserved by kernel */
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(0, &pSettings->videoDecoderModuleSettings);
#endif

#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_DisplaySettings(0, &pSettings->displayModuleSettings);
#endif
#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
    BDBG_WRN(("Heap Size UMA16(xvd %d, vdc %d)",
              NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE,
              NEXUS_DISPLAY_UMA16_HEAP_SIZE));
#endif
#else

	/* kernel suggested boot options bmem=192M@64M bmem=512M@512M for boards with >750M memory 
	   bmem=192M@64M bmem=256M@512M for the boards with 512 Mbytes of memory */

	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;	/* use all the available memory not reserved by kernel */
	pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;


	pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
	pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 0; /* initialized below */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 0;  /*initalized below. Make sure memory doesn't cross 256MB boundary */
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

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

	/*Don't let graphics heap cross 256 boundary, must be at least 64MB, memcIndex 0, subIndex1 starts at 512MB*/
	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = (256*1024*1024) - (pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size);


#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
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
            memSizes[0] = 1024*1024*1024;
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
    }

	#ifndef UCOS_DIAGS
    BDBG_MSG(("Memory indicates: MEMC0: %d MB, %d bit", memSizes[0] >> 20, memcWidth));
	#endif

    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];
#if NEXUS_HAS_DISPLAY && NEXUS_HAS_VIDEO_DECODER
    g_NEXUS_platformHandles.config.numWindowsPerDisplay = NEXUS_NUM_VIDEO_DECODERS;
    g_NEXUS_platformHandles.config.supportedDecoder[0] = true;
    g_NEXUS_platformHandles.config.supportedDecoder[1] = true;
    g_NEXUS_platformHandles.config.supportedDisplay[0] = true;
    g_NEXUS_platformHandles.config.supportedDisplay[1] = true;
#endif
    return 0;
}

NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex)
{
    NEXUS_HeapHandle heapHandle=NULL;

    switch (displayIndex) {
        case 0: /* HD Display */
        case 1: /* SD1 Display */
            heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_MAIN_HEAP];
            break;
        case NEXUS_OFFSCREEN_SURFACE:
            heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP];
            break;
        default:
            BDBG_WRN(("Invalid display index %d",displayIndex));
            break;
    }
    return  heapHandle;
}

NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
    uint32_t regData;
#if NEXUS_HAS_DVB_CI
    g_pMemoryEbi = NEXUS_Platform_P_MapMemory(NEXUS_DVB_CI_MEMORY_BASE, NEXUS_DVB_CI_MEMORY_LENGTH, false);
    if (!g_pMemoryEbi) { BERR_TRACE(BERR_OS_ERROR); return 1;}
    NEXUS_P_AddMap(NEXUS_DVB_CI_MEMORY_BASE, NULL, g_pMemoryEbi, NEXUS_DVB_CI_MEMORY_LENGTH);
#endif
#if (defined BCHP_REV_A0 && BCHP_CHIP!=7346)
	/* Run video shmoo to program the correct phase for 277MHz DDR */
	if (BTST_VideoShmoo( g_pCoreHandles->heap[0], g_pCoreHandles->chp,g_pCoreHandles->reg ) == BERR_TIMEOUT)
	{
		BDBG_ERR(("VFD read access timed out. CFE may not have programmed VFD client blockouts for 277 MHz operation."));
	}
#endif
#if BCHP_VER >= BCHP_VER_B0 && (defined BDSP_VP6_SUPPORT || defined BDSP_H264_ENCODE_SUPPORT)
    regData = BREG_Read32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_51);
    regData &= ~(BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_51, RR_EN)|
                 BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_51, BO_VAL));
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_51, RR_EN, 1);        /* Enable round-robin */
    regData |= BCHP_FIELD_DATA(MEMC_ARB_0_CLIENT_INFO_51, BO_VAL, 0xd8);    /* 1us blockout */
    BREG_Write32(g_pCoreHandles->reg, BCHP_MEMC_ARB_0_CLIENT_INFO_51, regData);
#else
    BSTD_UNUSED(regData);
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

#if NEXUS_PLATFORM_7418SFF_H
extern NEXUS_Error NEXUS_Platform_P_InitExternalRfm(const NEXUS_PlatformConfiguration *pConfig)
{
    NEXUS_GpioHandle channelGpio;
    NEXUS_GpioSettings gpioSettings;
    NEXUS_GpioStatus gpioStatus;
    bool channel4 = false;
    static uint8_t channel3Data[4] = {0x80, 0x00, 0x1e, 0xa3};
    static uint8_t channel4Data[4] = {0x80, 0x00, 0x21, 0xa3};
    uint8_t *pData;

    /* Make sure that platform initialized the i2c controllers */
    if ( pConfig->i2c[NEXUS_I2C_CHANNEL_EXT_RFM])
    {
    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eAonStandard, &gpioSettings);
    gpioSettings.mode = NEXUS_GpioMode_eInput;
    channelGpio = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 5, &gpioSettings);
    if ( channelGpio )
    {
        NEXUS_Gpio_GetStatus(channelGpio, &gpioStatus);
        channel4 = gpioStatus.value == NEXUS_GpioValue_eLow ? false : true;
        NEXUS_Gpio_Close(channelGpio);
    }

    BDBG_WRN(("Initializing external RFM for channel %u", channel4?4:3));
    pData = channel4?channel4Data:channel3Data;

    NEXUS_I2c_WriteNoAddr(pConfig->i2c[NEXUS_I2C_CHANNEL_EXT_RFM], 0x65, pData, 4);
    }

    return NEXUS_SUCCESS;
}
#endif

