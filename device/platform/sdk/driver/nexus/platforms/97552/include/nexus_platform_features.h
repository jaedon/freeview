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
 * $brcm_Workfile: nexus_platform_features.h $
 * $brcm_Revision: 20 $
 * $brcm_Date: 3/30/12 6:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97552/include/nexus_platform_features.h $
 * 
 * 20   3/30/12 6:32p xhuang
 * SW7552-95: support memory beyond 256M
 * 
 * 19   3/27/12 7:51p xhuang
 * SW7552-95: support 512M memory
 * 
 * 18   2/27/12 1:27a xhuang
 * SW7552-220: fix compiling issue for SPI
 * 
 * 17   1/17/12 1:53p xhuang
 * SW7552-189: fix memory segment fault issue
 * 
 * 16   1/17/12 9:01a xhuang
 * SW7552-191: Enable AVS by default
 * 
 * 15   1/17/12 8:52a xhuang
 * SW7552-193: Add HDMI CEC support for 7552
 * 
 * 14   1/2/12 6:38p vishk
 * SW7552-147: 7552B0: Add Nexus OOB support
 * 
 * 13   12/27/11 1:49p vishk
 * SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board.
 * 
 * 12   12/20/11 2:46p vishk
 * SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
 * 
 * 11   12/15/11 11:41a xhuang
 * SW7552-141: change 7552B0 input band maximum
 * 
 * SW7552-141/1   12/6/11 1:49p jianweiz
 * SW7552-141: change 7552B0 input band maximum
 * 
 * 10   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 * 
 * 9   9/1/11 6:44p xhuang
 * SW7552-114: change MAD pixel setting to fix RTS issue
 * 
 * 8   8/24/11 1:24p xhuang
 * SW7552-108: reduce XVD/VDC  buffer according to 7552 usage
 * 
 * 7   8/23/11 6:23p xhuang
 * SW7552-108: enable audio capture feature
 * 
 * 6   8/8/11 4:35p xhuang
 * SW7552-77: Add DVB-CI support
 * 
 * 5   8/8/11 1:24p xhuang
 * SW7552-77: Add DVB-CI support
 * 
 * 4   6/1/11 8:38p xhuang
 * SW7552-34: merge bringup change to main line
 * 
 * SW7552-34/3   6/1/11 5:58p jtna
 * SW7552-26: enable 7552 RFM
 * 
 * SW7552-34/2   5/27/11 8:37p jtna
 * SW7552-35: merge frontend work
 * 
 * SW7552-34/1   5/19/11 11:06p xhuang
 * SW7552-34: Temp disable frontend and RFM for bringup
 * 
 * 3   4/29/11 3:19p xhuang
 * SW7552-25: Turn on MAD compression
 * 
 * 2   4/28/11 6:45p xhuang
 * SW7552-25: Increase buffer for VDC
 * 
 * 1   1/25/11 5:52p xhuang
 * SW7552-25: Add 7552 support
 * 
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

#if NEXUS_BASE_ONLY

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

#else

/* Transport Features */
#if (BCHP_VER >= BCHP_VER_B0)
#define NEXUS_NUM_INPUT_BANDS 10
#else
#define NEXUS_NUM_INPUT_BANDS 5
#endif
#define NEXUS_NUM_PARSER_BANDS 10
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_TIMEBASES 4
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 4
#define NEXUS_NUM_REMUX 2
#define NEXUS_NUM_CEC 1
#if (BCHP_VER >= BCHP_VER_B0)
#define NEXUS_NUM_MTSIF 2
#endif
/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 1
#define NEXUS_NUM_XVD_DEVICES 1
#define NEXUS_NUM_MOSAIC_DECODES 12
#define NEXUS_NUM_STILL_DECODES 1

/* XVD Heaps
See the latest XVD_Memory_Worksheet.xls to calculate custom numbers */
/* 16 bit 1 HD decode , 1 SD still, Secure Heap */
#define NEXUS_VIDEO_DECODER_UMA16_GENERAL_HEAP_SIZE (10201728)
#define NEXUS_VIDEO_DECODER_UMA16_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE (22937600+3276800)
#define NEXUS_NUM_VIDEO_DECODERS_UMA16 NEXUS_NUM_VIDEO_DECODERS

/* TBD: Audio Features */
#define NEXUS_NUM_AUDIO_DSP 1
#define NEXUS_NUM_AUDIO_DECODERS 3
#define NEXUS_NUM_AUDIO_DACS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 8

#define NEXUS_NUM_I2S_INPUTS 0
#define NEXUS_NUM_I2S_OUTPUTS 1
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_CAPTURES 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 3

/* Display Features */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 0
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_HDMI_INPUTS 0
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 0
#define NEXUS_NUM_RFM_OUTPUTS 1

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
#define NEXUS_NUM_DISPLAYS 2
#define NEXUS_NUM_VIDEO_WINDOWS   1
#define NEXUS_NUM_DNR 1
#define NEXUS_NUM_DCR 1
#define NEXUS_NUM_VIDEO_DACS 4
#define NEXUS_NUM_LAB 2
#define NEXUS_NUM_CAB 2
#define NEXUS_NUM_PEP 1
#define NEXUS_HAS_TNT 1

/* This sets the madSettings.stVideoTestFeature1.ulBitsPerPixel to use reduced
 * bits/pixel in the deinterlacer .9.5 bit/pixel has been found to have
 * negligible impact on quality, while freeing significant bandwidth for other
 * uses.  The unit of ulBitsPerPixel is 0.5 bits.  CFE 3.14 requires this
 * compression, CFE 3.15 does not. */
#define NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL 18

#define NEXUS_DISPLAY_SIZE_FULL_HD_BUFFER 4147200
#define NEXUS_DISPLAY_SIZE_FULL_HD_PIP_BUFFER (4147200/4)
#define NEXUS_DISPLAY_SIZE_HD_BUFFER 2073600
#define NEXUS_DISPLAY_SIZE_HD_PIP_BUFFER (2073600/4)
#define NEXUS_DISPLAY_SIZE_SD_BUFFER 414720
#define NEXUS_DISPLAY_SIZE_SD_PIP_BUFFER (414720/4)

#define NEXUS_UMA16_STR "16 bit UMA,Single HD decode/display, HD MAD"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16              4
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_UMA16          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA16         6
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_UMA16     2
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16              0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_UMA16          0

#define NEXUS_DISPLAY_UMA16_HEAP_SIZE  ((NEXUS_DISPLAY_SIZE_FULL_HD_BUFFER*NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA16)+ \
                                        (NEXUS_DISPLAY_SIZE_FULL_HD_PIP_BUFFER*NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_UMA16)+ \
                                        (NEXUS_DISPLAY_SIZE_HD_BUFFER*NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16)+ \
                                        (NEXUS_DISPLAY_SIZE_HD_PIP_BUFFER*NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_UMA16)+ \
                                        (NEXUS_DISPLAY_SIZE_SD_BUFFER*NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16)+ \
                                        (NEXUS_DISPLAY_SIZE_SD_PIP_BUFFER*NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_UMA16))
/* Still Picture Decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 0
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

/* Cable Frontend */
/* TODO */
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 0
#define NEXUS_MAX_FRONTENDS 11
#define NEXUS_FRONTEND_SCAN_SUPPORTED 1
#define NEXUS_MAX_3255_ADSCHN 0
#define NEXUS_3255_OOB_TUNER_IFFREQ (1250000)/* 1.25 MHz */

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 4
#define NEXUS_NUM_GPIO_PINS 125

/* AON GPIO */
#define NEXUS_NUM_AON_SGPIO_PINS 2
#define NEXUS_NUM_AON_GPIO_PINS 21
#define NEXUS_GpioType_eAonStandard 4 
#define NEXUS_GpioType_eAonSpecial 5 

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 4

/* SPI Channels */
#define NEXUS_NUM_SPI_CHANNELS 2

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 1

/* UARTS */
#define NEXUS_NUM_UARTS 3

/* UHF INPUTS */
#define NEXUS_NUM_UHF_INPUTS 0

/* eneble AVS */
#define NEXUS_AVS_MONITOR 1

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 2

#endif

/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 3
#define NEXUS_DVB_CI_MEMORY_BASE (0x12000000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

/* Memory features */
#define NEXUS_NUM_MEMC 1

#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 1
#define NEXUS_MEMC0_GRAPHICS_HEAP       2

/* Use memory beyond 256M */
#define NEXUS_UPPER_MEMORY_SUPPORT      0

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

