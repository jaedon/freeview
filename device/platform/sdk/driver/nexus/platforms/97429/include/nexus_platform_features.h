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
* $brcm_Revision: 31 $
* $brcm_Date: 9/20/12 10:49a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97429/include/nexus_platform_features.h $
* 
* 31   9/20/12 10:49a gmohile
* SW7429-240 : encoder display idx is defined only if dsp encoder is
*  enabled
* 
* 30   9/18/12 2:04p gmohile
* SW7429-240 : Add NEXUS_ENCODER_DISPLAY_IDX
* 
* 29   7/2/12 10:28a katrep
* SW7429-183:Cleaned up platform define and build issue with no frontend
* 
* 28   6/26/12 3:56p vishk
* SW7429-183: Add support for 7241DCSFBTSFF board
* 
* 27   6/13/12 2:03p katrep
* SW7429-180:move the avs outside of of baseonly
* 
* 26   6/5/12 1:08p rjlewis
* SW7429-180: enable AVS by default.
* 
* 25   6/4/12 1:42p gmohile
* SW7231-792 : Enable 7429 audio mux output
* 
* 24   5/30/12 2:02p gskerl
* SW7429-29: Defined NEXUS_NUM_AUDIO_RETURN_CHANNEL to 1
* 
* 24   5/30/12 2:01p gskerl
* SW7429-29: Defined NEXUS_NUM_AUDIO_RETURN_CHANNEL to 1
* 
* 23   5/8/12 3:25p vishk
* SW3462-12: Enable support for second frontend
* 
* 22   5/7/12 6:13p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
* 
* 21   5/3/12 3:02p katrep
* SW7429-150:swap graphics and picture heap index to start graphics heap
*  at 256 boundary
* 
* 20   5/3/12 1:51p katrep
* SW7429-1:changed input capture to 2
* 
* 19   5/3/12 12:06p katrep
* SW7429-1:add NEXUS_NUM_AUDIO_INPUT_CAPTURES
* 
* 18   5/3/12 12:02p katrep
* SW7429-1:add NEXUS_NUM_AUDIO_INPUT_CAPTURES
* 
* 17   4/25/12 10:06a katrep
* SW7429-1:add cec & image image decoder
* 
* 16   2/9/12 3:03p jgarrett
* SW7425-2074: Adding audio input capture
* 
* 15   2/9/12 2:28p jgarrett
* SW7425-2074: Adding NEXUS_NUM_SPDIF_INPUTS
* 
* 14   2/3/12 3:20p katrep
* SW7429-1:fixed the BASE_ONLY build issue
* 
* 13   1/31/12 6:31p katrep
* SW7429-56:updated for Trinity frontend
* 
* 12   1/17/12 4:33p katrep
* SW7429-44:compile time enable option for 3255
* 
* 11   1/11/12 11:26a katrep
* SW7429-1:7429 has five i2c channnels and one hw still image decoder
* 
* 10   1/2/12 4:14p katrep
* SW7429-1:reverted back last change issue fixed in transport module
* 
* 9   12/29/11 1:49p katrep
* SW7429-1:tmp fixed the build issue for  7428
* 
* 8   11/30/11 4:51p mward
* SW7429-1:  Restore DACs and SPDIF outputs.
* 
* 7   11/30/11 4:15p mward
* SW7429-1:  Define NEXUS_MAX_FRONTENDS 0 for now.
* 
* 6   11/28/11 5:18p mward
* SW7429-1:  NEXUS_NUM_TIMEBASES 5 to use DPCR4
* 
* 5   11/3/11 6:53p jgarrett
* SW7422-400: Enabling audio debug logs on 40nm APE chips
* 
* 4   10/26/11 11:06a katrep
* SW7429-1:add rfm
* 
* 3   10/25/11 5:28p katrep
* SW7429-1:updated for 7429 bringup
* 
* 2   9/22/11 4:13p katrep
* SW7429-1:initial checkin
* 
* 1   8/9/11 8:01p katrep
* SW7429-1:initial checkin
* 
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

#if NEXUS_BASE_ONLY

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

#else

/* Transport Features */
#if PLATFORM == 97428
#define NEXUS_NUM_INPUT_BANDS 1
#else
#define NEXUS_NUM_INPUT_BANDS 5
#endif
#define NEXUS_NUM_MTSIF 2
#define NEXUS_NUM_PARSER_BANDS 10
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_TIMEBASES 5
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 6
#if PLATFORM == 97428
#define NEXUS_NUM_REMUX 0
#else 
#define NEXUS_NUM_REMUX 2
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
#endif

#if defined(HUMAX_PLATFORM_BASE)
#if !defined(NEXUS_NUM_DSP_VIDEO_ENCODERS)
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 2
#endif
#endif

/* Audio Features */
#define NEXUS_NUM_AUDIO_DSP 1
#define NEXUS_NUM_AUDIO_DECODERS 3 /* decode, passthrough, sub */
#define NEXUS_NUM_AUDIO_INPUT_CAPTURES 2 /* Number of external inputs active at a time */
#define NEXUS_NUM_AUDIO_DACS 1
#define NEXUS_NUM_SPDIF_INPUTS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 4
#if PLATFORM == 97428
#define NEXUS_NUM_I2S_INPUTS 0   
#else
#define NEXUS_NUM_I2S_INPUTS 1   
#endif
#define NEXUS_NUM_I2S_OUTPUTS 1
#define NEXUS_NUM_AUDIO_CAPTURES 1
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_AUDIO_PLAYBACKS 5
#else
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#endif
#define NEXUS_NUM_AUDIO_RETURN_CHANNEL   1

#define NEXUS_HAS_AUDIO_MUX_OUTPUT 1

/* Display Features */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_CEC 1
#if PLATFORM == 97428
#define NEXUS_NUM_656_OUTPUTS 0 
#define NEXUS_NUM_HDDVI_INPUTS 0      
#else
#define NEXUS_NUM_656_OUTPUTS 1 
#define NEXUS_NUM_HDDVI_INPUTS 1      
#endif
#define NEXUS_NUM_HDMI_OUTPUTS 1
#if PLATFORM == 97241
#define NEXUS_NUM_HDMI_INPUTS 0       
#else
#define NEXUS_NUM_HDMI_INPUTS 1       /* not available on 7241 */
#endif
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 0
#if PLATFORM == 97241
#define NEXUS_NUM_RFM_OUTPUTS 0
#else
#define NEXUS_NUM_RFM_OUTPUTS 1
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

#if NEXUS_NUM_DSP_VIDEO_ENCODERS
#define NEXUS_ENCODER_DISPLAY_IDX 1
#endif

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


/* Still Picture Decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 4
#define NEXUS_NUM_GPIO_PINS 150

/* AON GPIO */
#define NEXUS_NUM_AON_SGPIO_PINS 6
#define NEXUS_NUM_AON_GPIO_PINS 21
#define NEXUS_GpioType_eAonStandard 4
#define NEXUS_GpioType_eAonSpecial 5

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 5

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 1

/* UARTS */
#define NEXUS_NUM_UARTS 3

/* UHF INPUTS */
#define NEXUS_NUM_UHF_INPUTS 0

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 2

/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 3
#define NEXUS_DVB_CI_MEMORY_BASE (0x12000000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

/* front end */
#if PLATFORM == 97429 /* valid for both CV and DVR boards */
#define NEXUS_MAX_FRONTENDS 9 /* 3383 can support 8 drownstream */
#define NEXUS_MAX_3255_ADSCHN 8
#elif PLATFORM == 97241
    #if NEXUS_PLATFORM_7241_C2SFF
        #define NEXUS_MAX_FRONTENDS 4
        #define NEXUS_SHARED_FRONTEND_INTERRUPT 1
        #ifdef USE_SPI_FRONTEND
            /* SPI Channels */
	        #define NEXUS_NUM_SPI_CHANNELS 2
        #endif
    #elif NEXUS_USE_FRONTEND_DAUGHTER_CARD
        #define NEXUS_MAX_FRONTENDS 9
        #define NEXUS_NUM_FRONTEND_CARD_SLOTS 1
    #elif NEXUS_PLATFORM_7241_DCSFBTSFF
        #define NEXUS_MAX_FRONTENDS 8
    #else
/*		#define NEXUS_MAX_FRONTENDS 0  TODO_ILEE */
#if defined(CONFIG_BRCM_MOD_USE_SPI_FRONTEND) && defined(CONFIG_BRCM_MOD_USE_I2C_FRONTEND)  /*TODO_SYKIM */
        #define NEXUS_MAX_FRONTENDS 3     
#else
        #define NEXUS_MAX_FRONTENDS 2
#endif
        #define NEXUS_SHARED_FRONTEND_INTERRUPT 1
 		#ifdef USE_SPI_FRONTEND		/* ADD SY */
            /* SPI Channels */
	        #define NEXUS_NUM_SPI_CHANNELS 2
        #endif
    #endif
#else
    #define NEXUS_MAX_FRONTENDS 0
#endif

#define NEXUS_AVS_MONITOR           1

#endif /* NEXUS_BASE_ONLY, do not add anyhting below this line */

/* Memory features */
#define NEXUS_NUM_MEMC 1
/* default heap indices */
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 2
#define NEXUS_MEMC0_GRAPHICS_HEAP       1

#if defined(HUMAX_PLATFORM_BASE)
#undef NEXUS_NUM_COMPONENT_OUTPUTS
#undef NEXUS_NUM_COMPOSITE_OUTPUTS
#define NEXUS_NUM_COMPONENT_OUTPUTS CONFIG_BCM_COMPONENT_COUNT
#define NEXUS_NUM_COMPOSITE_OUTPUTS CONFIG_BCM_COMPOSITE_COUNT
#endif

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

