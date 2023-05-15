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
 * $brcm_Revision: 32 $
 * $brcm_Date: 11/20/12 4:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97435/include/nexus_platform_features.h $
 *
 * 32   11/20/12 4:13p mward
 * SW7435-500:  Changed NEXUS_NUM_STC_CHANNELS to 16.
 *
 * 31   6/28/12 4:13p mward
 * SW7401-4521: Eliminate NEXUS_TIMEBASE_HAS_32BIT_PRESCALE  use
 *  NEXUS_TransportCapabilities for prescale limits instead.
 *
 * 30   6/27/12 12:26p mward
 * SW7401-3748: Added runtime param check for PCR_INC and PRESCALE.  Added
 *  a flag NEXUS_TIMEBASE_HAS_32BIT_PRESCALE which can be defined for
 *  chips in which BXPT_HAS_32BIT_PRESCALE.
 *
 * 29   5/31/12 6:56p mward
 * SW7435-65:  Re-order and re-arrange heaps in memory for V3D
 *  compatibility.
 *
 * 28   5/18/12 12:06p mward
 * SW7435-165: Run-time selection of transcode use cases
 *
 * 27   5/8/12 5:33p erickson
 * SW7435-27: add webcpu support
 *
 * SW7435-165/1   5/1/12 6:35p mward
 * SW7435-165:  Run-time selection of transcode use cases
 *
 * 26   4/24/12 6:31p hongtaoz
 * SW7435-119: adjusted memc1 display heap size for quad seamless xcodes
 *  dynamic resolution;
 *
 * 25   4/23/12 6:52p mward
 * SW7435-119:  Increase FULL_HD_BUFFERS_MEMC0 for transcoding to output
 *  format of 24Hz.
 *
 * 24   4/19/12 6:53p hongtaoz
 * SW7435-117: increased 1u4t memc0 display heap allocation;
 *
 * 23   4/11/12 2:52p hongtaoz
 * SW7435-100: added single/dual xcode usage support;
 *
 * 22   4/10/12 11:54a mward
 * SW7435-67:  Put NEXUS_OFFSCREEN_SURFACE on same MEMC as HD display
 *  framebuffer.
 *
 * 21   4/5/12 12:06p mward
 * SW7435-7:  Correct NEXUS_NUM_2D_ENGINES.
 *
 * 20   3/28/12 11:44a mward
 * SW7425-2128:  Add NEXUS_NUM_CEC for 97435 platform.
 *
 * 19   3/23/12 4:47p mward
 * SW7435-7:  Enable AVS.
 *
 * 18   3/10/12 2:56p mward
 * SW7435-42:  Set NEXUS_MAX_FRONTENDS 7 for SV board, in order to open 4
 *  on-board 311x frontends.
 *
 * 17   3/8/12 1:56p mward
 * SW7435-42:  Set NEXUS_MAX_FRONTENDS 4 for SV board.  Latest rev
 *  connects pins to UARTs for debug, not frontends 4,5.
 *
 * 16   3/6/12 6:56p hongtaoz
 * SW7435-7: 7435 has 8 STC channels; disable 2nd audio dsp and 3rd/4th
 *  encoder displays for non-quad box types;
 *
 * 15   3/6/12 5:14p hongtaoz
 * SW7435-7: condition 2 VCE devices with quad box type;
 *
 * 14   3/5/12 5:11p mward
 * SW7435-7:  Increase NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE
 *  for reference software use case.
 *
 * 13   3/4/12 4:14p mward
 * SW7435-7:  use NEXUS_MEMC0_MAIN_HEAP for NEXUS_MEMC0_GRAPHICS_HEAP
 *
 * 12   3/3/12 7:20a mward
 * SW7435-7:  Define NEXUS_MEMC0_GRAPHICS_HEAP.
 *
 * 11   3/2/12 2:49p hongtaoz
 * SW7435-7: updated SVD0 picture heap size(no SVC support) for quad
 *  encode box;
 *
 * 10   3/2/12 2:07p hongtaoz
 * SW7435-7: updated quad memc alloc for 1u4t usage; 7435 has 6 displays;
 *
 * 9   3/2/12 9:31a mward
 * SW7435-7:  NEXUS_NUM_VCE_CHANNELS  is per device, not total.
 *
 * 8   3/1/12 6:13p mward
 * SW7435-7:  Set NEXUS_NUM_VIDEO_DECODERS 4.
 *
 * 7   2/13/12 2:30p gmullen
 * SW7425-2376: Updated number of PID channels
 *
 * 6   2/9/12 3:03p jgarrett
 * SW7425-2074: Adding audio input capture
 *
 * 5   2/9/12 2:28p jgarrett
 * SW7425-2074: Adding NEXUS_NUM_SPDIF_INPUTS
 *
 * 4   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 *
 * 3   11/1/11 11:08a mward
 * SW7435-7:  Always define NEXUS_NUM_L1_REGISTERS.
 *
 * 2   10/31/11 4:20p mward
 * SW7435-7:  Update feature counts.
 *
 * 1   10/25/11 4:43p mward
 * SW7435-7: Add 7435.
 *
 * 46   10/18/11 3:05p hongtaoz
 * SW7425-1402: changed memc1 gfx heap index to make gfx heap base address
 *  256MB aligned;
 *
 *
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 4

#if !NEXUS_BASE_ONLY

/* Transport Features */
#define NEXUS_NUM_INPUT_BANDS 11
#if defined(HUMAX_PLATFORM_BASE)
#if defined(CONFIG_BCM_MTSIF)
#define NEXUS_NUM_PARSER_BANDS 8
#else
#define NEXUS_NUM_PARSER_BANDS 11
#endif
#else
#define NEXUS_NUM_PARSER_BANDS 16
#endif
#define NEXUS_NUM_PID_CHANNELS 512
#define NEXUS_NUM_TIMEBASES 10
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 48
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_PLAYPUMPS 13 /*thers's a problem with ppump 13 & 14. limit the total pump number till things clear.*/
#else
#define NEXUS_NUM_PLAYPUMPS 22
#endif
#define NEXUS_NUM_REMUX 2
#define NEXUS_NUM_MTSIF 2

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 4
#define NEXUS_NUM_XVD_DEVICES 2
#define NEXUS_NUM_MOSAIC_DECODES 14
#define NEXUS_NUM_STILL_DECODES 1
#define NEXUS_SVC_MVC_SUPPORT 1


/* XVD Heaps
   TODO: Add correct XVD memory sizes when SVC 3D numbers are available
See 7422_Memory_Worksheet.xls to calculate custom numbers */
/* 32(MEMC0)+32(MEMC1) bit non UMA - 3 HD Decodes, 1 HD Still, 1 SD Still 11 CIF , 14 QCIF
   SVD0: Includes support for up to single 1080p60 SVC 2D/3D and MVC/AVC decode;
   AVD1: Includes support for up to dual HD AVC Level 4.2 decodes (one for PIP, the other for transcoder) */
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC0_GENERAL_HEAP_SIZE (21.38*1024*1024) /* FW code heap is separate */
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC1_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE_4T (53.4*1024*1024) /* no SVC support for quad box.  Around 50 should be adequate, extra for still video decoder */
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE_2T (57.89*1024*1024) /* with efficient memory app, it could be reduced to 55 */
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_GENERAL_HEAP_SIZE (32.23*1024*1024) /* support 2x HD decodes */
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE (45.7*1024*1024)

/* Audio Features */
#define NEXUS_NUM_AUDIO_DSP 2
#define NEXUS_NUM_AUDIO_DECODERS 6
#define NEXUS_NUM_AUDIO_INPUT_CAPTURES 1 /* Number of external inputs active at a time */
#define NEXUS_NUM_AUDIO_DACS 2
#define NEXUS_NUM_SPDIF_INPUTS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 4
#define NEXUS_NUM_AUDIO_MIXERS 8

#define NEXUS_NUM_I2S_INPUTS 1
#define NEXUS_NUM_I2S_OUTPUTS 2
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_CAPTURES 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 3

#define NEXUS_HAS_AUDIO_MUX_OUTPUT 1

/* Display Features */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 2
#define NEXUS_NUM_HDMI_INPUTS 1
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_CEC 1
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 0
#define NEXUS_NUM_RFM_OUTPUTS 0

/* Video Encoder Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VCE_DEVICES    2
#define NEXUS_NUM_VCE_CHANNELS  2 /* per device */
#define NEXUS_NUM_VIDEO_ENCODERS   (NEXUS_NUM_VCE_CHANNELS * NEXUS_NUM_VCE_DEVICES)

/* ViCE2 Heaps
   1MB FW code+data + 1MB debug buffer are not included below;
   See 7425_Memory_Worksheet.xls and RTS document to calculate custom numbers */
/* 32(MEMC0)+32(MEMC1) bit non UMA - 1 HD encode
   ViCE2: Includes support for AVC, VC1, MPEG4, MPEG2 */
#define NEXUS_VIDEO_ENCODER_ViCE_32MEMC0_SECURE_HEAP_SIZE  (0x00400000 * NEXUS_NUM_VCE_CHANNELS) /* ViCE2's SG output on MEMC0; ITB/CDB is extra */
#define NEXUS_VIDEO_ENCODER_ViCE_32MEMC1_PICTURE_HEAP_SIZE (0x02800000 * NEXUS_NUM_VCE_CHANNELS) /* picture stores on MEMC1 */

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
#define NEXUS_NUM_DISPLAYS 6 /* C0/C1 for local HD/SD simul, C5/4/3/2 for quad transcodes */
#define NEXUS_NUM_VIDEO_WINDOWS   2 /* per display */
#define NEXUS_NUM_DNR 4 /* for 4xMFDs */
#define NEXUS_NUM_DCR 4
#define NEXUS_NUM_VIDEO_DACS 4
#define NEXUS_NUM_LAB 1
#define NEXUS_NUM_CAB 1
#define NEXUS_NUM_PEP 1
#define NEXUS_HAS_TNT 1

/* This sets the madSettings.stVideoTestFeature1.ulBitsPerPixel to use reduced
 * bits/pixel in the deinterlacer .9.5 bit/pixel has been found to have
 * negligible impact on quality, while freeing significant bandwidth for other
 * uses.  The unit of ulBitsPerPixel is 0.5 bits.  CFE 3.14 requires this
 * compression, CFE 3.15 does not. */
#define NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL 18

/* 7435:
if !NEXUS_SWAP_VCE_INSTANCE:
       display5 -> STG0 -> encoder0; 1st encoder connected with display 5
       display4 -> STG1 -> encoder1; 2nd encoder connected with display 4
       display3 -> STG2 -> encoder2; 3rd encoder mutual exclusive with PIP and local display 3
       display2 -> STG3 -> encoder3; 4th encoder mutual exclusive with PIP window and local SD display2
else (NEXUS_SWAP_VCE_INSTANCE)
       display5 -> STG2 -> encoder0 (vice1_ch0); 1st encoder connected with display 5
       display4 -> STG3 -> encoder1 (vice1_ch1); 2nd encoder connected with display 4
       display3 -> STG0 -> encoder2 (vice0_ch0); 3rd encoder mutual exclusive with PIP and local display 3
       display2 -> STG1 -> encoder3 (vice0_ch1); 4th encoder mutual exclusive with PIP window and local SD display2
endif
       display1 -> local SD display
       display0 -> local HD display
 */
#define NEXUS_ENCODER0_DISPLAY_IDX 5
#define NEXUS_ENCODER1_DISPLAY_IDX 4
#define NEXUS_ENCODER2_DISPLAY_IDX 3
#define NEXUS_ENCODER3_DISPLAY_IDX 2

/* backwards compatible */
#define NEXUS_ENCODER_DISPLAY_IDX NEXUS_ENCODER0_DISPLAY_IDX

/* Display buffers required different memc assignments
 * See 7435 memory spreadsheet.
 */
/* For the 4 transcode use case */
/* C0V0 and C5V0 are from MEMC0, rest on MEMC1 */
#define NEXUS_32MEMC0_32MEMC1_STR_4T  "32+32 bit NonUMA,1u4t"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0_4T         		0
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0_4T    		0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0_4T    		4 /* C5V0 1080p24->720p24 bias CapScl */
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0_4T  	4 /* C1V0 1080p24->SD */
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0_4T   			0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0_4T 			0

/*  - Buffers for the main path are
 *    allocated from VDC heap on MEMC0 when the window is created */
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1_4T      		0
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1_4T  		0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1_4T 		13 /* 4+3+3+3 1080p24 with bias CapScl*/
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1_4T 	2  /* MCVP QM stores */
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1_4T  			1
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1_4T		0


/* For the 2 and 1 transcode use cases */
#define NEXUS_32MEMC0_32MEMC1_STR_2T  "32+32 bit NonUMA,Display HD+HD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0_2T      		0
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0_2T  		0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0_2T 		(8+2) /* 1080p3D C0V0 with ANR */
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0_2T 	(4)   /* C1V0 */
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0_2T  			1 /* MCVP QM store */
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0_2T			0

/*  - Buffers for the main path are
 *    allocated from VDC heap on MEMC0 when the window is created */
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1_2T      		4 /* 1080p source SD PIP C1V1 capture buffer */
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1_2T  		0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1_2T 		6 /* HD xcode C3V0 with MADR */
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1_2T 	(8) /* 1080p3D PIP C0V1 */
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1_2T  			0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1_2T		0

/* Still Picture Decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1

/* Graphics Features */
#if NEXUS_WEBCPU
/* 1 per instance of nexus_graphics2d */
#define NEXUS_NUM_2D_ENGINES 1
#else
#define NEXUS_NUM_2D_ENGINES 2
#endif
#define NEXUS_NUM_3D_ENGINES 1
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

/* Cable Frontend */
/* The maximum number of input bands on 7425 are 9. Also 3128 has 8 DS + 1 OB */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_PLATFORM_DB4506)
#define NEXUS_MAX_FRONTENDS CONFIG_NUM_OF_SAT_CHANNEL
#endif
#define NEXUS_MAX_FRONTENDS 9
#if NEXUS_FRONTEND_3128
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 8
#elif NEXUS_FRONTEND_3124
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 4
#elif NEXUS_FRONTEND_3123s
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 3
#endif
#else
#if defined(NEXUS_USE_7425_SV_BOARD)
#define NEXUS_MAX_FRONTENDS 7
#elif defined(NEXUS_PLATFORM_7425_DBS)
#define NEXUS_MAX_FRONTENDS 8
#else
#define NEXUS_MAX_FRONTENDS 9
#if NEXUS_FRONTEND_3128
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 8
#elif NEXUS_FRONTEND_3124
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 4
#elif NEXUS_FRONTEND_3123
#define NEXUS_3128_MAX_DOWNSTREAM_CHANNELS 3
#endif
#if NEXUS_USE_FRONTEND_DAUGHTER_CARD
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 1
#endif
#endif
#endif

#define NEXUS_MAX_3255_ADSCHN 8
#define NEXUS_3255_OOB_TUNER_IFFREQ (1250000)/* 1.25 MHz */
#define NEXUS_SHARED_FRONTEND_INTERRUPT 1

/* GPIO */
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_SGPIO_PINS 4
#else
#define NEXUS_NUM_SGPIO_PINS 6
#endif
#define NEXUS_NUM_GPIO_PINS 111

/* AON GPIO */
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_AON_SGPIO_PINS 6
#else
#define NEXUS_NUM_AON_SGPIO_PINS 4
#endif
#define NEXUS_NUM_AON_GPIO_PINS 18
#define NEXUS_GpioType_eAonStandard 4
#define NEXUS_GpioType_eAonSpecial 5

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 5

/* I2C channel usage assignments. Refer to BSC table in the board schematics. */
#define NEXUS_I2C_CHANNEL_HDMI_TX         0
#define NEXUS_I2C_CHANNEL_HDMI_RX         1  /* Unused */
/*#define NEXUS_I2C_CHANNEL_MOCA          2     BCM3450. Used by the kernel. */
#define NEXUS_I2C_CHANNEL_LNA             3  /* BCM3405 */
#define NEXUS_I2C_CHANNEL_EXT_RFM         3  /* External RFM */
#define NEXUS_I2C_CHANNEL_TUNERS_4_5      3  /* BCM3112_4/_5(for SV board) */
#define NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3  4  /* BCM3112_0/_1/_2/_3(for SV board) */
#define NEXUS_I2C_CHANNEL_DSTRM_TUNER     4  /* BCM3128(for VMS board) */

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 2

/* UARTS */
#define NEXUS_NUM_UARTS 3

/* UHF INPUTS */
#define NEXUS_NUM_UHF_INPUTS 1

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 2

#endif

/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_BASE (0x19800000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

/* Memory features */
#define NEXUS_NUM_MEMC 2

/* default heap indices */
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC1_GRAPHICS_HEAP       1 /* make gfx heap aligned by 256MB for better of v3d purpose */
#define NEXUS_MEMC0_GRAPHICS_HEAP       2
#define NEXUS_MEMC1_MAIN_HEAP           3
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 4
#define NEXUS_MEMC1_PICTURE_BUFFER_HEAP 5
#define NEXUS_MEMC0_DRIVER_HEAP         6


#define NEXUS_AVS_MONITOR           1


#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */
