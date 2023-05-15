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
* $brcm_Workfile: nexus_platform_93335.c $
* $brcm_Revision: 1 $
* $brcm_Date: 9/26/12 1:55p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/93335/src/nexus_platform_93335.c $
* 
* 1   9/26/12 1:55p jzhu
* SW7435-246:support for 93335
* 
* 18   5/31/12 6:56p mward
* SW7435-65:  Re-order and re-arrange heaps in memory for V3D
*  compatibility.
* 
* 17   5/18/12 12:06p mward
* SW7435-165: Run-time selection of transcode use cases
* 
* 16   5/16/12 1:46p erickson
* SW7435-27: adapt heaps if no bmem in lower 256MB
* 
* SW7435-165/2   5/16/12 2:50p mward
* SW7435-165:  Explicitly specify encoder to VCE and encoder to timing
*  generator mappings.
* 
* SW7435-165/1   5/1/12 6:35p mward
* SW7435-165:  Run-time selection of transcode use cases
* 
* 15   4/24/12 6:32p hongtaoz
* SW7435-119: adjusted memc1 display heap size for quad seamless xcodes
*  dynamic resolution;
* 
* 14   4/23/12 6:55p mward
* SW7435-119:  Increase display memory for transcoding to output format
*  of 24Hz.
* 
* 13   4/19/12 6:53p hongtaoz
* SW7435-117: increased 1u4t memc0 display heap allocation;
* 
* 12   4/11/12 2:52p hongtaoz
* SW7435-100: added single/dual xcode usage support;
* 
* 11   4/10/12 11:54a mward
* SW7435-67:  Put NEXUS_OFFSCREEN_SURFACE on same MEMC as HD display
*  framebuffer.
* 
* 10   3/14/12 2:11p mward
* SW7435-26: Additional memory for MJPEG via SID.
* 
* 9   3/6/12 5:20p hongtaoz
* SW7435-50: increase heap[5] size for quad transcoders bringup;
* 
* 8   3/5/12 6:20p mward
* SW7435-7:  Increase NEXUS_MEMC1_PICTURE_BUFFER_HEAP for reference
*  software use case.
* 
* 7   3/5/12 4:38p mward
* SW7435-7: Set RR_EN bits to allow otherwised unused SVC BLD to start in
*  1u4t use case.
* 
* 6   3/2/12 3:49p hongtaoz
* SW7435-7: xcode0's display window heap for non-quad box is on memc1;
* 
* 5   3/2/12 2:50p hongtaoz
* SW7435-7: update default platform heap sizes for quad encodes box
*  bringup;
* 
* 4   3/2/12 2:06p hongtaoz
* SW7435-7: updated quad encodes 1u4t box type's memory heap allocation;
* 
* 3   3/1/12 6:18p mward
* SW7435-7:  Correct video decoder / feeder mapping.
* 
* 2   11/22/11 1:02p mward
* SW7435-7:  Remove 7425-specific code.
* 
* 1   10/25/11 5:15p mward
* SW7435-7: Initial version copied from 97425 version.
* 
* 38   10/5/11 1:03p jessem
* SW7425-1343: Excluded RFM init when compiling in NEXUS_BASE_ONLY mode.
* 
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
#include "bchp_memc_arb_0.h"
#include "bchp_memc_arb_1.h"

BDBG_MODULE(nexus_platform_97435);

static unsigned g_memoryMode;

static unsigned g_num_xcodes = 4;


#if NEXUS_HAS_VIDEO_DECODER
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoDecoderSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    /* By convention the first XVD device is the SVD0 decoder which has its picture
     * buffers on MEMC1 */
    pSettings->heapSize[0].general = NEXUS_VIDEO_DECODER_SVD0_32MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[0].secure =  NEXUS_VIDEO_DECODER_SVD0_32MEMC1_SECURE_HEAP_SIZE;
    pSettings->heapSize[0].picture = (g_num_xcodes == 4) ? NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE_4T : 
                                                           NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE_2T;
    pSettings->avdHeapIndex[0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
    pSettings->mfdMapping[0] = 0; /* 1st decoder is mapped to MPEG feeder 0 */
    pSettings->avdMapping[0] = 0; /* 1st decoder is mapped to SVD0 */
	pSettings->mfdMapping[3] = 1; /* 4th decoder is mapped to MPEG feeder 1 */
	pSettings->avdMapping[3] = 0; /* 4th decoder is mapped to SVD0 */

    /* The second XVD device is the AVD1 decoder has its picture
     * buffers on MEMC0 */
    pSettings->heapSize[1].general = NEXUS_VIDEO_DECODER_AVD1_32MEMC0_GENERAL_HEAP_SIZE;
    pSettings->heapSize[1].secure =  NEXUS_VIDEO_DECODER_AVD1_32MEMC0_SECURE_HEAP_SIZE;
    pSettings->heapSize[1].picture = NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE;
    pSettings->avdHeapIndex[1] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;
	pSettings->mfdMapping[1] = 2; /* 2nd decoder is mapped to MPEG feeder 2 */
	pSettings->avdMapping[1] = 1; /* 2nd decoder is mapped to AVD1 */
	pSettings->mfdMapping[2] = 3; /* 3rd decoder is mapped to MPEG feeder 3 */
	pSettings->avdMapping[2] = 1; /* 3rd decoder is mapped to AVD1 */

    /* set decoder general heap to heap 5 */
    pSettings->hostAccessibleHeapIndex = NEXUS_MEMC0_DRIVER_HEAP;
}
#endif

#if NEXUS_HAS_VIDEO_ENCODER
/* If the user supplied video encoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_VideoEncoderSettings(NEXUS_VideoEncoderModuleSettings *pSettings)
{
    /* According to vce memory spreadsheet, encoder has its picture buffers on MEMC1 */
    if (g_num_xcodes == 4) {
        pSettings->heapSize[1].general = 0;/* encoder default: no use for current hw */
        pSettings->heapSize[1].secure =  NEXUS_VIDEO_ENCODER_ViCE_32MEMC0_SECURE_HEAP_SIZE;
        pSettings->heapSize[1].picture = NEXUS_VIDEO_ENCODER_ViCE_32MEMC1_PICTURE_HEAP_SIZE;
    
        pSettings->heapIndex[1].firmware[0] = NEXUS_MEMC1_MAIN_HEAP;
        pSettings->heapIndex[1].firmware[1] = NEXUS_MEMC1_MAIN_HEAP;
        pSettings->heapIndex[1].system  = NEXUS_MEMC1_MAIN_HEAP; /* FW debug log buffer */
        pSettings->heapIndex[1].general = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* same as picture heap for UMA */
        pSettings->heapIndex[1].secure  = NEXUS_MEMC0_DRIVER_HEAP; /* mapped for CDB capture; must be memc0 */
        pSettings->heapIndex[1].picture = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* unmapped */
        pSettings->heapSize[0].general = 0;/* encoder default: no use for current hw */
        pSettings->heapSize[0].secure =  NEXUS_VIDEO_ENCODER_ViCE_32MEMC0_SECURE_HEAP_SIZE;
        pSettings->heapSize[0].picture = NEXUS_VIDEO_ENCODER_ViCE_32MEMC1_PICTURE_HEAP_SIZE;
    
        pSettings->heapIndex[0].firmware[0] = NEXUS_MEMC0_MAIN_HEAP;
        pSettings->heapIndex[0].firmware[1] = NEXUS_MEMC0_MAIN_HEAP;
        pSettings->heapIndex[0].system  = NEXUS_MEMC0_MAIN_HEAP; /* FW debug log buffer */
        pSettings->heapIndex[0].general = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* same as picture heap for UMA */
        pSettings->heapIndex[0].secure  = NEXUS_MEMC0_DRIVER_HEAP; /* mapped for CDB capture; must be memc0 */
        pSettings->heapIndex[0].picture = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* unmapped */
        pSettings->vceMapping[0].device = 0;
        pSettings->vceMapping[0].channel = 0;
        pSettings->vceMapping[1].device = 0;
        pSettings->vceMapping[1].channel = 1;
        pSettings->vceMapping[2].device = 1;
        pSettings->vceMapping[2].channel = 0;
        pSettings->vceMapping[3].device = 1;
        pSettings->vceMapping[3].channel = 1;
    }
    else {
        pSettings->heapSize[0].general = 0;/* encoder default: no use for current hw */
        pSettings->heapSize[0].secure =  NEXUS_VIDEO_ENCODER_ViCE_32MEMC0_SECURE_HEAP_SIZE;
        pSettings->heapSize[0].picture = NEXUS_VIDEO_ENCODER_ViCE_32MEMC1_PICTURE_HEAP_SIZE;
    
        pSettings->heapIndex[0].firmware[0] = NEXUS_MEMC1_MAIN_HEAP;
        pSettings->heapIndex[0].firmware[1] = NEXUS_MEMC1_MAIN_HEAP;
        pSettings->heapIndex[0].system  = NEXUS_MEMC1_MAIN_HEAP; /* FW debug log buffer */
        pSettings->heapIndex[0].general = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* same as picture heap for UMA */
        pSettings->heapIndex[0].secure  = NEXUS_MEMC0_DRIVER_HEAP; /* mapped for CDB capture; must be memc0 */
        pSettings->heapIndex[0].picture = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* unmapped */
        pSettings->vceMapping[0].device = 1;
        pSettings->vceMapping[0].channel = 0;
        pSettings->vceMapping[1].device = 1;
        pSettings->vceMapping[1].channel = (g_num_xcodes == 2) ? 1 : -1;
        pSettings->vceMapping[2].device = -1;
        pSettings->vceMapping[2].channel = -1;
        pSettings->vceMapping[3].device = -1;
        pSettings->vceMapping[3].channel = -1;
    }
    return;
}
#endif

#if NEXUS_HAS_DISPLAY
/* If the user supplied video decoder settings not available,
   then use default  Video Buffers based on the chip usage modes */
static void NEXUS_Platform_P_DisplaySettings(NEXUS_DisplayModuleSettings *pSettings)
{
    unsigned i;

    pSettings->primaryDisplayHeapIndex = (g_num_xcodes == 4) ? NEXUS_MEMC1_PICTURE_BUFFER_HEAP : 
                                                               NEXUS_MEMC0_PICTURE_BUFFER_HEAP;

    for(i=0;i<NEXUS_MAX_HEAPS;i++ ) {
        switch(i)
        {
        case NEXUS_MEMC0_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0_4T : 
                                                                                          NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0_2T;
            pSettings->displayHeapSettings[i].hdBuffers.count = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0_4T : 
                                                                                      NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0_2T;
            pSettings->displayHeapSettings[i].sdBuffers.count = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0_4T : 
                                                                                      NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0_2T;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0_4T : 
                                                                                             NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0_2T;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0_4T : 
                                                                                         NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0_2T;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0_4T : 
                                                                                         NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0_2T;
            break;
        case NEXUS_MEMC1_PICTURE_BUFFER_HEAP:
            pSettings->displayHeapSettings[i].fullHdBuffers.count = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1_4T : 
                                                                                          NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1_2T;
            pSettings->displayHeapSettings[i].hdBuffers.count = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1_4T : 
                                                                                      NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1_2T;
            pSettings->displayHeapSettings[i].sdBuffers.count = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1_4T : 
                                                                                      NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1_2T;
            pSettings->displayHeapSettings[i].fullHdBuffers.pipCount = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1_4T : 
                                                                                             NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1_2T;
            pSettings->displayHeapSettings[i].hdBuffers.pipCount = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1_4T : 
                                                                                         NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1_2T;
            pSettings->displayHeapSettings[i].sdBuffers.pipCount = (g_num_xcodes == 4) ? NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1_4T : 
                                                                                         NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1_2T;
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
    * Based on RTS settings
    */
    if (g_num_xcodes == 4) {
        pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* C0V0: local HD display main window */
        pSettings->videoWindowHeapIndex[0][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
        pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* C1V0: local SD main */
        pSettings->videoWindowHeapIndex[1][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
        pSettings->videoWindowHeapIndex[2][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 3 */
        pSettings->videoWindowHeapIndex[2][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
        pSettings->videoWindowHeapIndex[3][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 2 */
        pSettings->videoWindowHeapIndex[4][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 1 */
        pSettings->videoWindowHeapIndex[5][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* xcode 0 */
        pSettings->encoderTgMapping[0] = 0;
        pSettings->encoderTgMapping[1] = 1;
        pSettings->encoderTgMapping[2] = 2;
        pSettings->encoderTgMapping[3] = 3;
    }
    else {
        pSettings->videoWindowHeapIndex[0][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* C0V0: local HD display main window */
        pSettings->videoWindowHeapIndex[0][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* C0V1: local HD PIP */
        pSettings->videoWindowHeapIndex[1][0] = NEXUS_MEMC0_PICTURE_BUFFER_HEAP;/* C1V0: local SD main */
        pSettings->videoWindowHeapIndex[1][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* C1V1: local SD PIP */
        pSettings->videoWindowHeapIndex[2][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
        pSettings->videoWindowHeapIndex[2][1] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
        pSettings->videoWindowHeapIndex[3][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;
        pSettings->videoWindowHeapIndex[4][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 1 */
        pSettings->videoWindowHeapIndex[5][0] = NEXUS_MEMC1_PICTURE_BUFFER_HEAP;/* xcode 0 */
        pSettings->encoderTgMapping[0] = 2;
        pSettings->encoderTgMapping[1] = 3;
        pSettings->encoderTgMapping[2] = 0;
        pSettings->encoderTgMapping[3] = 1;
    }
}
#endif

void NEXUS_Platform_P_GetDefaultSettings(NEXUS_PlatformSettings *pSettings)
{
    const NEXUS_PlatformMemory *pMemory = &g_platformMemory; /* g_platformMemory is completely initialized already */

    /* bmem=192M@64M bmem=192M@512M */

    /* heap[0] is nexus default heap, also used for SD2 FB */
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memcIndex = 0;
    /* if there is no bmem in lower 256MB, heap[0] must move to upper memory. we must also shrink NEXUS_MEMC0_GRAPHICS_HEAP */
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].subIndex = pMemory->osRegion[0].base >= 512*1024*1024 ? 1:0;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].size = -1;
    pSettings->heap[NEXUS_MEMC0_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    /* heap[1] Offscreen gfx surfaces & Main FB. 256MB aligned for better V3D purpose. */
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].size = 256 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

	/* heap[2] Offscreen gfx surfaces & Main FB. 256MB aligned for better V3D purpose. */
	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memcIndex = 0;
	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].subIndex = 1;
	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].size = pMemory->osRegion[0].base >= 512*1024*1024 ? 128 * 1024 * 1024 : 256 * 1024 * 1024;
	pSettings->heap[NEXUS_MEMC0_GRAPHICS_HEAP].memoryType = NEXUS_MemoryType_eApplication; /* cached only */

    if (g_num_xcodes == 4) {
    /* heap[3] used for VCE 2nd core fw and debug buffer */
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].size = 30 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    /* heap[4] MEMC0 device heap for picture buffers (AVD1+VDC+VCE0).
     * TODO: optimize it to reduce device memory allocation!
     */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 146 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* heap[5] MEMC1 device heap for picture buffers: SVD0+VDC+VCE1.
     * TODO: optimize it to reduce device memory allocation!
     */
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].subIndex = 0;
    /* NOTE: we cannot use size = -1 on MEMC1 because the fake address scheme would run up to 0xFFFFFFFF, and that
    exposes logic errors in nexus/magnum. */
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = 190 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* heap[6] used for XVD fw/general heap and VCE secure heap.
     * TODO: separate XVD fw heap from non-CPU accessible heap to optimize mappable size;
     * TODO: security support for VCE secure heap;
     */
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].size = 98 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].memoryType = NEXUS_MemoryType_eFull;
    }
    else {
    /* heap[3] used for VCE 2nd core fw and debug buffer */
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].subIndex = 0;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].size = 30  * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_MAIN_HEAP].memoryType = NEXUS_MemoryType_eFull;

    /* heap[4] MEMC0 device heap for picture buffers (AVD1+VDC).
     * TODO: optimize it to reduce device memory allocation!
     */
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].size = 172 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC0_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* heap[5] MEMC1 device heap for picture buffers: video decoder, encoder and BVN.
     * TODO: optimize it to reduce device memory allocation!
     */
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memcIndex = 1;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].subIndex = 0;
    /* NOTE: we cannot use size = -1 on MEMC1 because the fake address scheme would run up to 0xFFFFFFFF, and that
    exposes logic errors in nexus/magnum. */
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].size = 176 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC1_PICTURE_BUFFER_HEAP].memoryType = NEXUS_MemoryType_eDeviceOnly; /* unmapped */

    /* heap[6] used for XVD fw/general heap and VCE secure heap.
     * TODO: separate XVD fw heap from non-CPU accessible heap to optimize mappable size;
     * TODO: security support for VCE secure heap;
     */
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].memcIndex = 0;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].subIndex = 1;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].size = 80 * 1024 * 1024;
    pSettings->heap[NEXUS_MEMC0_DRIVER_HEAP].memoryType = NEXUS_MemoryType_eFull;
    }

#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_Platform_P_VideoDecoderSettings(&pSettings->videoDecoderModuleSettings);
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    NEXUS_Platform_P_VideoEncoderSettings(&pSettings->videoEncoderSettings);
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
    uint32_t regValue,blockout;
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

    /* 7425 SVC decode requires 32b+32b (memc0 + memc1)
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
    }

    BDBG_MSG(("Memory indicates: MEMC0: %d MB, MEMC1: %d MB", memSizes[0] >> 20, memSizes[1] >> 20));

    /* populate memc[] */
    pMemory->memc[0].length = memSizes[0];
    pMemory->memc[1].length = memSizes[1];

    /* Read RTS settings to find the number of hardware (ViCE) video encode channels */
    regValue = BREG_Read32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_18);
    blockout = BCHP_GET_FIELD_DATA(regValue, MEMC_ARB_0_CLIENT_INFO_18, BO_VAL);
    if (blockout==0x7fff) {
        g_num_xcodes = 4;
        /* Workaround allows otherwised unused SVC BLD to start. Newer CFE shouldn't need it. */
	regValue = BREG_Read32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_18);
	regValue |= (BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_18, RR_EN));
	BREG_Write32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_18, regValue);
	regValue = BREG_Read32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_19);
	regValue |= (BCHP_MASK(MEMC_ARB_0_CLIENT_INFO_19, RR_EN));
	BREG_Write32(hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_19, regValue);
    }
    else {
        regValue = BREG_Read32(hReg, BCHP_MEMC_ARB_1_CLIENT_INFO_74);
        blockout = BCHP_GET_FIELD_DATA(regValue, MEMC_ARB_1_CLIENT_INFO_74, BO_VAL);
        if (blockout>0x800) {
            g_num_xcodes = 2;
        }
        else {
            g_num_xcodes = 1;
        }
    }
    BDBG_WRN(("CFE RTS is configured for %u transcodes",g_num_xcodes));

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
    if (g_num_xcodes == 4) {
        switch (displayIndex) {
    case 0: /* HD Display */
    case 2: /* xcode3 Display aka 3rd Display*/
    case 3: /* xcode2 Display aka 4th Display */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP];
        break;
    case 1: /* SD1 Display */
    case 4: /* xcode1 Display */
    case 5: /* xcode0 Display */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC1_GRAPHICS_HEAP];
        break;
	case NEXUS_OFFSCREEN_SURFACE:
		heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP];
		break;
        default:
            BDBG_ERR(("Invalid display index %d",displayIndex));
        }
    }
    else {
        switch (displayIndex) {
    case 0: /* HD Display */
    case 1: /* SD1 Display */
    case 2: /* SD2 Display aka 3rd Display*/
    case 4: /* xcode1 Display */
	case 5: /* xcode0 Display */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC1_GRAPHICS_HEAP];
        break;
    case 3: /* SD3 Display aka 4th Display on MEMC1 */
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC0_GRAPHICS_HEAP];
        break;
    case NEXUS_OFFSCREEN_SURFACE:
        heapHandle = g_pCoreHandles->nexusHeap[NEXUS_MEMC1_GRAPHICS_HEAP];
        break;
        default:
            BDBG_ERR(("Invalid display index %d",displayIndex));
        }
    }
    return heapHandle;
}


NEXUS_Error NEXUS_Platform_P_InitBoard(void)
{
	uint32_t regData;

    /* do work after Core module is up. */
	BSTD_UNUSED(regData);

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
