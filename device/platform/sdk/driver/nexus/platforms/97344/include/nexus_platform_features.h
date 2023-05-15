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
 * $brcm_Revision: 55 $
 * $brcm_Date: 9/20/12 10:44a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97344/include/nexus_platform_features.h $
 *
 * 55   9/20/12 10:44a gmohile
 * SW7429-240 : encoder display idx is defined only if dsp encoder is
 *  enabled
 *
 * 54   9/18/12 2:03p gmohile
 * SW7429-240 : Add NEXUS_ENCODER_DISPLAY_IDX
 *
 * 53   8/9/12 2:08p jrubio
 * SW7425-3701: reduce number of playbacks
 *
 * 52   7/24/12 4:35p jrubio
 * SW7346-848: merge in Vish Changes into mainline
 *
 * 51   2/29/12 10:23a gmullen
 * SW7346-718: Increased NEXUS_NUM_TIMEBASES
 *
 * 50   2/13/12 3:14p gmullen
 * SW7425-2376: Updated number of transport PID channels and RAVE contexts
 *
 * 49   2/6/12 9:09p gmohile
 * SW7425-1473 : Merge 7346 raaga video encoder support
 *
 * 48   1/12/12 3:32p randyjew
 * SW7425-1140: Add CEC support for 7344/7346
 *
 * 47   12/7/11 10:21a gmullen
 * SW7346-577: NEXUS_NUM_PLAYPUMPS was incorrect for 7346Bx
 *
 * 46   11/29/11 3:32p jrubio
 * SW7346-564: add B0 2 SGPIO AON pins
 *
 * 45   11/23/11 11:30a gmohile
 * SW7425-1473 : Enable audio mux output
 *
 * 44   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 *
 * 43   10/31/11 1:51p randyjew
 * SW7344-204: Add Still image Decode support
 *
 * 42   10/13/11 3:57p randyjew
 * SW7344-198: Add External 4505 Support for SV boards
 *
 * 41   10/7/11 2:31p vishk
 * SW3128-47: Add Support for BCM3123
 *
 * 40   10/6/11 12:50p randyjew
 * SW7346-117: Add AVS support
 *
 * 39   10/5/11 6:16p jrubio
 * SW7346-470: increase input bands to 10 for B0
 *
 * 38   9/26/11 5:25p randyjew
 * SW7344-140:Adding 7418SFF_H support
 *
 * 37   9/21/11 5:04p randyjew
 * SW7418-39: Add External RFM support for 7418SFF_H
 *
 * 36   9/7/11 2:55p jrubio
 * SW7346-470: update input bands for 7346B0
 *
 * 35   9/2/11 5:49p vishk
 * SW7346-301: Nexus Frontend 7346 needs to support both internal and
 *  external (BCM3445) LNAs
 *
 * 34   7/28/11 2:19p jrubio
 * SW7346-205: update buffers
 *
 * 33   7/26/11 10:12a jrubio
 * SW7346-117: update 7344 internal tuners to 1
 *
 * 32   7/11/11 10:08p vanessah
 * SW7425-689: sync nexus and PI madr dcxs value
 *
 * 31   6/28/11 4:16p jrubio
 * SW7346-165: undo last checkin, right now SFF and HR44 PLATFORM_VER's
 *  can default to 2 Tuners.
 *
 * 30   6/28/11 3:23p vishk
 * SW7346-165: NEXUS Frontend changes for multi-tuner board are not
 *  platform independent Add support for SFF board.
 *
 * 29   6/23/11 5:57p vishk
 * SW7346-165: NEXUS Frontend changes for multi-tuner board are not
 *  platform independent
 *
 * 28   5/23/11 3:08p jrubio
 * SW7346-213: update naming
 *
 * 27   4/26/11 3:39p randyjew
 * SW7344-9: temporarily remove 2nd tuner for 7344.
 *
 * 26   4/26/11 2:46p jrubio
 * SW7346-158: add 7346 SPI frontpanel
 *
 * 25   3/30/11 11:27a jrubio
 * SW7346-116: update number of audio decodes to 3
 *
 * 24   3/18/11 2:07p jrubio
 * SW7344-9: turn on Mad with compression
 *
 * 23   3/17/11 1:17p erickson
 * SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
 *  nexus_platform_features.h
 *
 * 22   2/16/11 4:39p randyjew
 * SW7344-25: Add Spi Frontpanel support for 7344
 *
 * 21   1/5/11 6:52p agin
 * SW7344-20: Define I2C channel for MOCA.
 *
 * 20   1/4/11 4:51p jrubio
 * SW7344-20: add HD HD Pip
 *
 * 19   1/2/11 1:36p jrubio
 * SW7344-9: remove compile errors
 *
 * 18   12/30/10 5:33p jrubio
 * SW7344-9: increase memory for HD HD PIP
 *
 * 17   12/30/10 1:31p erickson
 * SW7422-2: update mmap
 *
 * 16   12/23/10 10:48a jrubio
 * SW7422-2: revert back to version 14. Remove NUM_MEMC_REGIONS
 *
 * 15   12/23/10 10:27a jrubio
 * SW7422-2: add NUM_MEMC_REGIONS define
 *
 * 14   12/22/10 4:06p erickson
 * SW7422-2: remove unused NEXUS_MEMC1_PHYSICAL_ADDRESS
 *
 * 13   12/22/10 11:03a jrubio
 * SW7422-2: add MEMC
 *
 * 12   12/17/10 10:57a garylin
 * SW7344-16: added supported for DVB-CI
 *
 * 11   12/15/10 11:40a jrubio
 * SW7344-9: update features for 7346
 *
 * 10   12/14/10 11:51a jrubio
 * SW7344-9: modify platform features for bringup
 *
 * 9   12/13/10 10:36p jrubio
 * SW7344-9: reduces features for now
 *
 * 8   12/13/10 11:57a jrubio
 * SW7344-9: update 7346 GPIO pins
 *
 * 7   12/9/10 3:34p jrubio
 * SW7344-9: adjust features settings
 *
 * 6   12/8/10 9:50a jrubio
 * SW7344-9: modify 7344 settings
 *
 * 5   12/7/10 11:48a jrubio
 * SW7344-9: fix typo
 *
 * 4   12/7/10 11:48a jrubio
 * SW7344-9: add frontends
 *
 * 3   12/7/10 11:39a jrubio
 * SW7344-9: modify existing features
 *
 * 2   12/2/10 2:19p jrubio
 * SW7344-9: update features
 *
 * 1   11/29/10 4:20p jrubio
 * SW7344-9: initial Version
 *
 * 9   11/12/10 10:29a rgreen
 * SW7422-10: Merge changes; restore hdmiInput support
 *
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
#if BCHP_CHIP==7344
#define NEXUS_NUM_INPUT_BANDS 5
#define NEXUS_NUM_PLAYPUMPS 4
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_TIMEBASES 4
#elif BCHP_CHIP==7346
#if defined(HUMAX_PLATFORM_BASE) && BCHP_VER>=BCHP_VER_B0
#define NEXUS_NUM_INPUT_BANDS 11
#else
#define NEXUS_NUM_INPUT_BANDS 10
#endif
#define NEXUS_NUM_PLAYPUMPS 12
#define NEXUS_NUM_PID_CHANNELS 512
#define NEXUS_NUM_RAVE_CONTEXTS 48
#define NEXUS_NUM_TIMEBASES 8
#else
#error "Unknown Chip"
#endif
#define NEXUS_NUM_PARSER_BANDS 10
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_REMUX 2

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 2
#define NEXUS_NUM_XVD_DEVICES 1
#define NEXUS_NUM_MOSAIC_DECODES 14
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

/* Audio Features */
#define NEXUS_NUM_AUDIO_DSP 1
#define NEXUS_NUM_AUDIO_DECODERS 3
#define NEXUS_NUM_AUDIO_DACS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 4
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 2
#define NEXUS_NUM_VIDEO_ENCODERS 1
#endif

#if defined(HUMAX_PLATFORM_BASE)
#if !defined(NEXUS_NUM_DSP_VIDEO_ENCODERS)
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 2
#endif
#endif

#define NEXUS_NUM_I2S_INPUTS 1
#define NEXUS_NUM_I2S_OUTPUTS 1
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_CAPTURES 1

#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_AUDIO_PLAYBACKS 3
#else
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#endif

#define NEXUS_HAS_AUDIO_MUX_OUTPUT 1

/* Display Features */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 1
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_HDMI_INPUTS 0
#define NEXUS_NUM_CEC 1
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 0
#define NEXUS_NUM_RFM_OUTPUTS 0

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
/* Keep this to non hdsd for bringup */
#define NEXUS_NUM_DISPLAYS 2
#define NEXUS_NUM_VIDEO_WINDOWS   2
#define NEXUS_NUM_DNR 1
#define NEXUS_NUM_DCR 1
#define NEXUS_NUM_VIDEO_DACS 4
#define NEXUS_NUM_LAB 1
#define NEXUS_NUM_CAB 1
#define NEXUS_NUM_PEP 1
#define NEXUS_HAS_TNT 1
#ifdef NEXUS_PLATFORM_97346_I2SFF
#define NEXUS_NUM_MTSIF 2
#endif

/* This sets the madSettings.stVideoTestFeature1.ulBitsPerPixel to use reduced
 * bits/pixel in the deinterlacer .9.5 bit/pixel has been found to have
 * negligible impact on quality, while freeing significant bandwidth for other
 * uses.  The unit of ulBitsPerPixel is 0.5 bits.  CFE 3.14 requires this
 * compression, CFE 3.15 does not. */
#define NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL 18

#if NEXUS_NUM_DSP_VIDEO_ENCODERS
#define NEXUS_ENCODER_DISPLAY_IDX 1
#endif

#define NEUXS_DISPLAY_SIZE_FULL_HD_BUFFER 4147200
#define NEUXS_DISPLAY_SIZE_HD_BUFFER 2073600
#define NEXUS_DISPLAY_SIZE_SD_BUFFER 414720

#define NEXUS_UMA32_STR "32 bit UMA,Display HD+SD,656 input,MAD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA32              5
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_UMA32          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_UMA32         12
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_UMA32     14
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA32              1
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

/* AVS support */
#define NEXUS_AVS_MONITOR 1

#if BCHP_VER >= BCHP_VER_B0
/* Still Picture Decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1
#endif

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

/* Cable Frontend */
#if BCHP_CHIP==7344
#if NEXUS_PLATFORM_7418SFF_H
#define NEXUS_MAX_FRONTENDS 3
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 3
#else
#if NEXUS_PLATFORM_7344SV
#define NEXUS_MAX_FRONTENDS 2
#else
#define NEXUS_MAX_FRONTENDS 1
#endif
#endif
/* Internal Frontends define to 1 */
#define NEXUS_7346_MAX_FRONTEND_CHANNELS 1
#elif BCHP_CHIP==7346
#if defined(NEXUS_PLATFORM_97346_SV)
#if NEXUS_USE_FRONTEND_DAUGHTER_CARD
#define NEXUS_MAX_FRONTENDS 9
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 1
#else
#define NEXUS_MAX_FRONTENDS 4
#endif
#elif defined(NEXUS_PLATFORM_97346_HR44)
#define NEXUS_MAX_FRONTENDS 6
#elif NEXUS_PLATFORM_97346_I2SFF
#define NEXUS_MAX_FRONTENDS 4
#else
#define NEXUS_MAX_FRONTENDS 2
#endif
#endif


/* #define NEXUS_MAX_3255_ADSCHN 0
#define NEXUS_3255_OOB_TUNER_IFFREQ (1250000) 1.25 MHz */

/* GPIO */
#if BCHP_CHIP==7344
#define NEXUS_NUM_SGPIO_PINS 6
#define NEXUS_NUM_GPIO_PINS 85
#elif BCHP_CHIP==7346
#define NEXUS_NUM_SGPIO_PINS 6
#define NEXUS_NUM_GPIO_PINS 72
#else
#error "unsupported Chip"
#endif

/* AON GPIO */
#if BCHP_CHIP==7344
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_AON_SGPIO_PINS 4
#else
#define NEXUS_NUM_AON_SGPIO_PINS 2
#endif
#define NEXUS_NUM_AON_GPIO_PINS 19
#define NEXUS_GpioType_eAonStandard 4
#define NEXUS_GpioType_eAonSpecial 5
#elif BCHP_CHIP==7346
#if BCHP_VER== BCHP_VER_A0
#define NEXUS_NUM_AON_SGPIO_PINS 2
#else
#define NEXUS_NUM_AON_SGPIO_PINS 4
#endif
#define NEXUS_NUM_AON_GPIO_PINS 27
#define NEXUS_GpioType_eAonStandard 4
#define NEXUS_GpioType_eAonSpecial 5
#else
#error "unsupported Chip"
#endif

#if BCHP_CHIP==7344
/*Actually an AON GPIO for Keybad */
#define NEXUS_KEYPAD_AON_GPIO 10
#else
#define NEXUS_KEYPAD_AON_GPIO 25
#endif

/* I2C Channels */
/* AON GPIO */
#define NEXUS_NUM_I2C_CHANNELS  4

#if BCHP_CHIP==7344
#define NEXUS_MOCA_I2C_CHANNEL 2
#if NEXUS_PLATFORM_7418SFF_H
#define NEXUS_I2C_CHANNEL_EXT_RFM         1  /* External RFM */
#endif
#else
#define NEXUS_MOCA_I2C_CHANNEL 1
#endif

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 1

/* UARTS */
#define NEXUS_NUM_UARTS 3

/* UHF INPUTS */
#define NEXUS_NUM_UHF_INPUTS 1

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 2

#endif
#if 1
/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_BASE (0x19800000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)
#endif
/* Memory features */
#define NEXUS_NUM_MEMC 1

/* default heap indices */
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 1
#define NEXUS_MEMC0_GRAPHICS_HEAP       2

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

