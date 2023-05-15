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
* $brcm_Workfile: nexus_platform_97422.c $
* $brcm_Revision: 12 $
* $brcm_Date: 4/27/12 12:38p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97422/src/nexus_platform_97422.c $
* 
* 12   4/27/12 12:38p jessem
* SW7422-470: Updated for Bx.
*
* 11   5/23/11 11:51a jessem
* SW7231-128: Added BCHP_Handle parameter to BTST_VideoShmoo() to support
*  power management.
*
* 10   3/4/11 2:31p jessem
* SW7422-295: Added check for timeout error in call to BTST_VideoShmoo().
*
* 9   2/28/11 1:16p jessem
* SW7422-295: Added call to BTST_VideoShmoo to address VFD burst read
*  issue with 7422 a0.
*
* 8   12/30/10 12:19p erickson
* SW7422-2: adjust 97422 heaps, add comments
*
* 7   12/29/10 2:51p nickh
* SW7422-2: Allow main MEMC0 heap to be specified from kernel boot up
*  option
*
* 6   12/29/10 1:58p nickh
* SW7422-2: Increase main memory size and reduce graphics heap
*
* 5   12/27/10 2:37p erickson
* SW7422-2: switch default config to use unmapped eDeviceOnly memory for
*  picture buffers
*
* 4   12/27/10 1:42p erickson
* SW7422-2: add NEXUS_Platform_P_GetDefaultSettings
*
* 3   12/22/10 3:12p erickson
* SW7422-2: adjustments
*
* 2   12/21/10 2:12p hongtaoz
* SW7422-2: fixed MEMC1 base address;
*
* 1   12/21/10 9:51a erickson
* SW7422-2: rework nexus_platform_core
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "nexus_platform_features.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr23_shim_addr_cntl_1.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_memc_ddr_1.h"
#include "bchp_clkgen.h"

#if defined BDSP_VP6_SUPPORT || defined BDSP_H264_ENCODE_SUPPORT
#include "bchp_memc_arb_0.h"
#endif

#ifdef BCHP_REV_A0
#include "btst_video_shmoo.h"
#endif
BDBG_MODULE(nexus_platform_97422);

static unsigned g_memoryMode;

#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    /* By convention the first XVD device is the SVD0 decoder which has its picture
     * buffers on MEMC1 */
    pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_SVD0_32MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_SVD0_32MEMC1_SECURE_HEAP_SIZE;
    pSettings->heapSize[0].picture = NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE;
    pSettings->avdHeapIndex[0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->mfdMapping[0] = 0; /* SVD is mapped to MPEG feeder 0 */

    /* The second XVD device is the AVD1 decoder has its picture
     * buffers on MEMC1 */
    pSettings->heapSize[1].general = NEXUS_VIDEO_DECODER_AVD1_32MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[1].secure =  NEXUS_VIDEO_DECODER_AVD1_32MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[1].picture = NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE;
    pSettings->avdHeapIndex[1] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
    pSettings->mfdMapping[1] = 1; /* AVD is mapped to MPEG feeder 1 */
}
#endif

#if NEXUS_HAS_DISPLAY
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_DisplaySettings(NEXUS_DisplayModuleSettings *pSettings)
{
    unsigned i;

    pSettings->primaryDisplayHeapIndex = NEXUS_MEMC1_MAIN_HEAP;

    for(i=0;i<NEXUS_MAX_HEAPS;i++ ) {
        switch(i)
        {
        case NEXUS_MEMC0_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0;
            break;
        case NEXUS_MEMC1_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1;
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


   /*
    * Based on RTS settings for BCM97422
    */
    pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[0][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[1][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[2][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->videoWindowHeapIndex[2][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
}
#endif

void NEXUS_Platform_P_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    /* bmem=192M@64M bmem=512M@512M */

    /* heap[0] is nexus default heap */
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    /* heap[1] used for PIP framebuffer */
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].size = 32 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    /* offscreen (non-framebuffer) gfx surfaces */
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = 256 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = -1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].subIndex = 0;
    /* NOTE: we cannot use size = -1 on MEMC1 because the fake address scheme would run up to 0xFFFFFFFF, and that
    exposes logic errors in nexus/magnum. */
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = 768 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
#endif
#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_DisplaySettings(&pSettings->displayModuleSettings);
#endif
}

/* read memoryMode & memSizes[].
this function does not know memc offsets.
this function is needed for memc's that linux doesn't report (e.g. non-UMA 7405 MEMC1)
*/
NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(BREG_Handle hReg, NEXUS_PlatformMemory *pMemory, NEXUS_PlatformSettings *pSettings)
{
    uint32_t regValue;
    uint32_t memc0_config=1, memc1_config=1;
    uint32_t ddr0Device=0, ddr1Device =0;
    unsigned memSizes[NEXUS_NUM_MEMC];
    unsigned memoryMode;

    BSTD_UNUSED(pSettings);

    BKNI_Memset(pMemory, 0, sizeof(*pMemory));
    BKNI_Memset(memSizes, 0, sizeof(*memSizes));

    /* Read DDR parameters
     * DRAM Width: 0 (32 bit), 1 (16 bit) */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
    memc0_config = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG, DRAM_WIDTH);

    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_1_CONFIG);
    memc1_config = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR23_SHIM_ADDR_CNTL_1_CONFIG, DRAM_WIDTH);

    /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit)
     * Should be 4 for 7422 configuration */
    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
    ddr0Device = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

    regValue = BREG_Read32(hReg, BCHP_MEMC_DDR_1_CNTRLR_CONFIG);
    ddr1Device = BCHP_GET_FIELD_DATA(regValue, MEMC_DDR_1_CNTRLR_CONFIG, DEVICE_TECH);

    /* 7422 SVC decode requires 32b+32b (memc0 + memc1)
       Other usage cases may permit a 32b+16b (memc0 + memc1) configuration */
    /* MEMC0 + MEMC1 configured in 32b mode */
    if (memc0_config == 0 && memc1_config == 0)
    {
        memoryMode = 0; /* 32b+32b configuration */
     }
    /* Single decode - MEMC1 configured in 16b mode */
    else
    {
        memoryMode = 1; /* 32b+16b configuration */
    }
    g_memoryMode = memoryMode;

    BDBG_CASSERT(NEXUS_NUM_MEMC >= 2);
    switch (memoryMode)
    {
    case 0: /* 32b+32b */
        {
            /* 32b */
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

            /* 32b */
            switch(ddr1Device)
            {
                case 0: /* 256 Mbits Device Tech*/
                    memSizes[1] = 64*1024*1024;
                    break;
                case 1: /* 512 Mbits Device Tech*/
                    memSizes[1] = 128*1024*1024;
                    break;
                case 2: /* 1024 Mbits Device Tech*/
                    memSizes[1] = 256*1024*1024;
                    break;
                case 3: /* 2048 Mbits Device Tech*/
                    memSizes[1] = 512*1024*1024;
                    break;
                case 4: /* 4096 Mbits Device Tech*/
                    memSizes[1] = 1024*1024*1024;
                    break;
            default:
                    BDBG_ERR(("Unknown Value in DDR1 Device Config Register"));
                    break;
            }
        }
        break;

    case 1: /* 32b+16b */
        {
            /* 32b */
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
                    return BERR_TRACE(NEXUS_UNKNOWN);
            }

            /* 16b */
            switch(ddr1Device)
            {
                case 0: /* 256 Mbits Device Tech*/
                    memSizes[1] = 32*1024*1024;
                    break;
                case 1: /* 512 Mbits Device Tech*/
                    memSizes[1] = 64*1024*1024;
                    break;
                case 2: /* 1024 Mbits Device Tech*/
                    memSizes[1] = 128*1024*1024;
                    break;
                case 3: /* 2048 Mbits Device Tech*/
                    memSizes[1] = 256*1024*1024;
                    break;
                case 4: /* 4096 Mbits Device Tech*/
                    memSizes[1] = 1024*1024*1024;
                    break;
            default:
                    BDBG_ERR(("Unknown Value in DDR1 Device Config Register"));
                    return BERR_TRACE(NEXUS_UNKNOWN);
            }
        }
        break;

    default:
        BDBG_ERR(("Invalid Memory controller configuration = %x", memoryMode));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    BDBG_MSG(("Memory indicates: MEMC0: %d MB, MEMC1: %d MB", memSizes[0] >> 20, memSizes[1] >> 20));

    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];
    pMemory->memc[1].length = memSizes[1];

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
    case 2: /* SD2 Display aka 3rd Display */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC1_MAIN_HEAP];
        break;
    default:
        BDBG_MSG(("Invalid display index %d",displayIndex));
    }
    return  heapHandle;
}


NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
	uint32_t regData;

    /* do work after Core module is up. */
#ifdef BCHP_REV_A0
	/* Run video shmoo to program the correct phase for 277MHz DDR */
	if (BTST_VideoShmoo( g_pCoreHandles->heap[0], g_pCoreHandles->chp, g_pCoreHandles->reg ) == BERR_TIMEOUT)
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
#endif
    return 0;
}

void NEXUS_Platform_P_UninitBoard(void)
{
    return;
}

#ifndef NEXUS_BASE_ONLY
#if NEXUS_USE_7425_SV_BOARD
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
    channelGpio = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 8, &gpioSettings);
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
#endif
