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
* $brcm_Revision: 49 $
* $brcm_Date: 9/20/12 12:34p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97231/include/nexus_platform_features.h $
* 
* 49   9/20/12 12:34p gmohile
* SW7429-240 : encoder display idx is defined only if dsp encoder is
*  enabled
* 
* 48   9/18/12 2:14p gmohile
*  SW7429-240 : Add NEXUS_ENCODER_DISPLAY_IDX
* 
* 47   7/17/12 1:56p randyjew
* SW7231-882: Make graphics heap 256MB when possible.
* 
* 46   7/16/12 3:54p vishk
* SW7231-895: Add support for 97231B2 EUSFF_V20 board.
* 
* 45   7/11/12 6:23p randyjew
* SW7231-891: Disable NEXUS_ENCODER_DISPLAY_IDX causes brutus issues,
*  Todo.
* 
* 44   7/10/12 3:24p randyjew
* SW7231-891: add NEXUS_ENCODER_DISPLAY_IDX
* 
* 43   5/31/12 5:07p vishk
* SW7231-828: Merge to mainline
* 
* AlphaBoxee/1   5/31/12 4:11p vishk
* SW7231-828: Please provide ATSC(MX603+BCM3517) tuner driver.
* 
* 42   5/3/12 11:58a katrep
* SW7231-317:add NEXUS_NUM_AUDIO_INPUT_CAPTURES
* 
* 41   3/1/12 5:59p vishk
* SW7231-68: Setting the default to I2C.
* 
* 40   3/1/12 5:58p vishk
* SW7231-68: Renaming the define used to specify SPI usage vs I2C.
* 
* 39   2/24/12 7:02p vishk
* SW7231-68: Settting the default to I2C.
* 
* 38   2/24/12 5:57p vishk
* SW7231-68: add SPI support on top of I2C for 3461 interface
* 
* 37   2/23/12 4:38p katrep
* SW7231-317:add hdmi cec
* 
* 36   2/9/12 5:48p ssood
* SW7425-2331: increase NEXUS_NUM_AUDIO_DECODERS to 3 to support
*  additional decoder for audio encode path
* 
* 35   1/16/12 6:06p gmohile
* SW7425-1473 : Merge raaga encoder support
* 
* 34   1/3/12 4:40p katrep
* SW7231-317:enabled avs
* 
* 33   12/9/11 2:35p jrubio
* SW7231-527: fix A0 support
* 
* 32   12/9/11 11:05a jrubio
* SW7231-527: fix A0 support
* 
* 31   11/23/11 11:15a gmohile
* SW7425-1473 : Enable audio mux output
* 
* 30   11/16/11 5:28p vishk
* SW3461-78: Add support for 3128 C0 on DCSFBTSFF board(7231B0+3128C0)
* 
* 29   11/3/11 6:53p jgarrett
* SW7422-400: Enabling audio debug logs on 40nm APE chips
* 
* 28   10/12/11 7:33p vishk
* SW3128-16: Add scan support to 3128
* 
* 27   10/10/11 5:44p katrep
* SW7231-317:fixed incorrect macro
* 
* 26   9/22/11 4:06p katrep
* SW7231-317:updated #of  image decoders
* 
* 25   9/9/11 4:14p katrep
* SW7231-320:add support for still image decoder
* 
* 24   8/31/11 11:16a katrep
* SW7231-317:add support for 7231 B0
* 
* 23   8/9/11 6:35p katrep
* SW7231-334:integrated svb-ci code from Ashwin
* 
* 22   7/11/11 10:08p vanessah
* SW7425-689: sync nexus and PI madr dcxs value
* 
* 21   6/5/11 9:05p vishk
* SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
* 
* 20   5/17/11 11:58a katrep
* SW7231-143: need more memory for picture heap, review spreadsheet
* 
* 19   5/16/11 1:22p katrep
* SW7231-143:removed hardcoded heap creation
* 
* 18   5/9/11 5:26p katrep
* SW7231-137:add support for aon, aons goio
* 
* 17   4/12/11 11:54a VISHK
* SW7231-76 : Add support for FBTSFF 97231 + 3128 board.  Add support for
*  all three board configurations.
* 
* 16   3/17/11 1:17p erickson
* SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
*  nexus_platform_features.h
* 
* 15   3/16/11 11:53a vishk
* SW7231-76 : Add support for FBTSFF 97231 + 3128 board.
* 
* 14   2/25/11 1:54p vishk
* SW7231-30: Add support for 3109,3406 tuners on 97231CSFF board
* 
* 13   1/24/11 5:49p katrep
* SW7231-4:1080i mad issue on brutus
* 
* 12   1/3/11 2:24p katrep
* SW7231-4:XVD uses heap 0 and heap 1
* 
* 11   1/3/11 10:38a katrep
* SW7231-4:Increased XVD memory to support dual SVC decode
* 
* 10   12/30/10 5:56p erickson
* SW7422-2: update mmap
*
* 9   12/30/10 1:31p erickson
* SW7422-2: update mmap
*
* 8   12/27/10 8:17p katrep
* SW7231-4:Updated VDC,XVD heap numbers
*
* 7   12/27/10 4:15p katrep
* SW7231-4:updated VDC buffers
*
* 6   12/25/10 6:00p katrep
* SW7231-4:updated num of audio dacs
*
* 5   12/22/10 4:47p erickson
* SW7422-2: remove unused NEXUS_MEMC1_PHYSICAL_ADDRESS
*
* 4   12/22/10 4:07p katrep
* SW7231-26:add support for new heap configuration, add support for 7230
*
* 3   12/8/10 6:51p katrep
* SW7231-4:remove uhf input
*
* 2   12/2/10 10:10a katrep
* SW7231-4:intial version
*
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

#if NEXUS_BASE_ONLY

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

#else

/* Transport Features */
#define NEXUS_NUM_INPUT_BANDS 8
#define NEXUS_NUM_PARSER_BANDS 10
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_TIMEBASES 4
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 6
#define NEXUS_NUM_REMUX 2
#if BCHP_VER >= BCHP_VER_B0
#define NEXUS_NUM_MTSIF 2
#endif


/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 2 /* Technically, two channels on a single decoder, but this indicates PIP support */
#define NEXUS_NUM_XVD_DEVICES 1
#define NEXUS_NUM_MOSAIC_DECODES 12
#define NEXUS_NUM_STILL_DECODES 1
#define NEXUS_SVC_MVC_SUPPORT 1

/* XVD Heaps
See the latest XVD_Memory_Worksheet.xls to calculate custom numbers */
/* 32,16  bit UMA 2 HD Decodes 1080p decodes, single SVC or MVC Decode */

/* This memory allocation is reference software usage modes, changes these vaules
   to meet your boards memory requirements 

   Features selected Dual SVc HD, SVC & MVC,SVC progressive + 3D, Efficient memory , 
   IP,MPEG, VC1, AVS Selected  Mode*/
#define NEXUS_VIDEO_DECODER_UMA32_GENERAL_HEAP_SIZE (42*1024*1024)
#define NEXUS_VIDEO_DECODER_UMA32_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE (106*1024*1024)
#define NEXUS_NUM_VIDEO_DECODERS_UMA32 2

/* Same as 32 UMA case */
#define NEXUS_VIDEO_DECODER_UMA16_GENERAL_HEAP_SIZE (42*1024*1024)
#define NEXUS_VIDEO_DECODER_UMA16_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE (106*1024*1024)
#define NEXUS_NUM_VIDEO_DECODERS_UMA16 2


#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 2
#define NEXUS_NUM_VIDEO_ENCODERS 1
#define NEXUS_ENCODER_DISPLAY_IDX 1
#endif

#if defined(HUMAX_PLATFORM_BASE)
#if !defined(NEXUS_NUM_DSP_VIDEO_ENCODERS)
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 2
#endif
#endif


/* Audio Features */
#define NEXUS_NUM_AUDIO_DSP 1
#define NEXUS_NUM_AUDIO_DECODERS 3
#define NEXUS_NUM_AUDIO_DACS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 4

#define NEXUS_NUM_I2S_INPUTS 1
#define NEXUS_NUM_I2S_OUTPUTS 1
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_CAPTURES 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#define NEXUS_NUM_AUDIO_INPUT_CAPTURES 2

#define NEXUS_HAS_AUDIO_MUX_OUTPUT 1


/* Display Features */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 2
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_CEC 1
#define NEXUS_NUM_HDMI_INPUTS 0
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 0
#define NEXUS_NUM_RFM_OUTPUTS 0
#if BCHP_VER >= BCHP_VER_B0
#define NEXUS_NUM_HDDVI_INPUTS 1
#define NEXUS_NUM_PICTURE_DECODERS 1
#else
#define NEXUS_NUM_HDDVI_INPUTS 0
#define NEXUS_NUM_PICTURE_DECODERS 0
#endif

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
#define NEXUS_NUM_DISPLAYS 2
#define NEXUS_NUM_VIDEO_WINDOWS   2
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


#define NEUXS_DISPLAY_SIZE_FULL_HD_BUFFER 4147200
#define NEUXS_DISPLAY_SIZE_HD_BUFFER 2073600
#define NEXUS_DISPLAY_SIZE_SD_BUFFER 414720

#define NEXUS_UMA32_STR "32 bit UMA,Display HD+SD,656 input,MAD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA32              6
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_UMA32          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA32         8
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_UMA32     0
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA32              0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_UMA32          0

#define NEXUS_DISPLAY_UMA32_HEAP_SIZE  ((NEUXS_DISPLAY_SIZE_FULL_HD_BUFFER * NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA32)+\
                                        (NEUXS_DISPLAY_SIZE_HD_BUFFER * NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA32)+ \
                                        (NEXUS_DISPLAY_SIZE_SD_BUFFER * NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA32))

#define NEXUS_UMA16_STR "16 bit UMA,Display SD,656 input,MAD,SD only decode"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16              6
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_UMA16          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA16         8
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_UMA16     0
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16              0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_UMA16          0

#define NEXUS_DISPLAY_UMA16_HEAP_SIZE  ((NEUXS_DISPLAY_SIZE_FULL_HD_BUFFER * NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA16)+\
                                        (NEUXS_DISPLAY_SIZE_HD_BUFFER * NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16)+ \
                                       (NEXUS_DISPLAY_SIZE_SD_BUFFER * NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16))


/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

/* Cable Frontend */
#if defined(NEXUS_PLATFORM_7231_CSFF)
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 0
#define NEXUS_MAX_FRONTENDS 2
#elif defined(NEXUS_PLATFORM_7231_EUSFF) || defined(NEXUS_PLATFORM_7231_EUSFF_V20)
#define NEXUS_PLATFORM_97231_3461_USES_SPI 0
#define NEXUS_SHARED_FRONTEND_INTERRUPT 1
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 0
#define NEXUS_MAX_FRONTENDS 8
#elif defined(NEXUS_PLATFORM_7231_3520)
#define NEXUS_MAX_FRONTENDS 2
#elif defined(NEXUS_PLATFORM_7231_FBTSFF)
#if NEXUS_FRONTEND_3128
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 8
#elif NEXUS_FRONTEND_3124
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 4
#elif NEXUS_FRONTEND_3123
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 3
#endif
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 0
#define NEXUS_MAX_FRONTENDS 8
#elif defined(NEXUS_FRONTEND_3461)
        #define NEXUS_MAX_FRONTENDS 2
        #define NEXUS_SHARED_FRONTEND_INTERRUPT 1
#else
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 4
#define NEXUS_MAX_FRONTENDS 8
#endif
#define NEXUS_MAX_3255_ADSCHN 0
#define NEXUS_3255_OOB_TUNER_IFFREQ (1250000)/* 1.25 MHz */

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 6
#define NEXUS_NUM_GPIO_PINS 150

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

/* default heap indices */
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 2
#define NEXUS_MEMC0_GRAPHICS_HEAP       1 /* make gfx heap aligned by 256MB for better of v3d purpose */

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

