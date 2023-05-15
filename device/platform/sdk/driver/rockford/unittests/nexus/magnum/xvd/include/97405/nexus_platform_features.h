/***************************************************************************
*     (c)2004-2009 Broadcom Corporation
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
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 5/14/09 11:04a $
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/magnum/xvd/include/97405/nexus_platform_features.h $
* 
* Hydra_Software_Devel/1   5/14/09 11:04a jtna
* PR54515: added xvd_app
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

/* Transport Features */
#define NEXUS_NUM_INPUT_BANDS 6
#define NEXUS_NUM_PARSER_BANDS 7
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_TIMEBASES 4
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 32
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 5
#define NEXUS_NUM_REMUX 2

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 2    /* Technically, two channels on a single decoder, but this indicates PIP support */
#define NEXUS_NUM_MOSAIC_DECODES 12
#define NEXUS_NUM_STILL_DECODES 1


/* XVD Heaps
See 7405_Memory_Worksheet.xls to calculate custom numbers */
/*64 bit UMA  1 HD Decode,1 SD Decode, 1 HD Still,1 SD Still,6 CIF , 12 QCIF,Secure Heap */
#define NEXUS_VIDEO_DECODER_UMA64_GENERAL_HEAP_SIZE (46*1024*1024)
#define NEXUS_VIDEO_DECODER_UMA64_SECURE_HEAP_SIZE4  (0)
#define NEXUS_VIDEO_DECODER_UMA64_PICTURE_HEAP_SIZE  (0)
#define NEXUS_NUM_VIDEO_DECODERS_UMA64 NEXUS_NUM_VIDEO_DECODERS

/* 32 bit UMA 1 HD Decode 1 HD Still,6 CIF , 12 QCIF,Secure Heap */
#define NEXUS_VIDEO_DECODER_UMA32_GENERAL_HEAP_SIZE (36*1024*1024)
#define NEXUS_VIDEO_DECODER_UMA32_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_UMA32_PICTURE_HEAP_SIZE (0)
#define NEXUS_NUM_VIDEO_DECODERS_UMA32 1

/* 16 bit 1 SD decode , 1 SD still, Secure Heap */
#define NEXUS_VIDEO_DECODER_UMA16_GENERAL_HEAP_SIZE (12*1024*1024)
#define NEXUS_VIDEO_DECODER_UMA16_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_UMA16_PICTURE_HEAP_SIZE (0)
#define NEXUS_NUM_VIDEO_DECODERS_UMA16 1

/* 32+16 bit non UMA 1 HD Decode 1 SD decode 1 HD Still,1 SD Still 6 CIF , 12 QCIF,Secure Heap */
#define NEXUS_VIDEO_DECODER_NONUMA3216_GENERAL_HEAP_SIZE (13*1024*1024)
#define NEXUS_VIDEO_DECODER_NONUMA3216_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_NONUMA3216_PICTURE_HEAP_SIZE (33*1024*1024)
#define NEXUS_NUM_VIDEO_DECODERS_NONUMA3216 NEXUS_NUM_VIDEO_DECODERS

/* 16+16 bit non UMA1 HD decode , 1 HD still, Secure Heap */
#define NEXUS_VIDEO_DECODER_NONUMA1616_GENERAL_HEAP_SIZE (10*1024*1024)
#define NEXUS_VIDEO_DECODER_NONUMA1616_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_NONUMA1616_PICTURE_HEAP_SIZE (26*1024*1024)
#define NEXUS_NUM_VIDEO_DECODERS_NONUMA1616 1

#if 0 /* removed to bring up nexus with reduced set of modules */
/* Audio Features */
#define NEXUS_NUM_AUDIO_DECODERS 3  /* On the 7405, this is possible but may exhaust DSP bandwidth with transcoding */
#define NEXUS_NUM_AUDIO_DACS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 8

#define NEXUS_NUM_I2S_INPUTS 1

#ifndef NEXUS_NUM_I2S_OUTPUTS
#define NEXUS_NUM_I2S_OUTPUTS 1
#endif
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#define NEXUS_NUM_AUDIO_CAPTURES 1
#endif

/* Display Features */
/* TODO: Add VDC memory allocations */
#ifndef NEXUS_NUM_656_INPUTS
#define NEXUS_NUM_656_INPUTS 0
#endif
#define NEXUS_NUM_656_OUTPUTS 0
#if 0 /* removed to bring up nexus with reduced set of modules */
#define NEXUS_NUM_HDMI_OUTPUTS 1
#endif
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 1
#if 0 /* removed to bring up nexus with reduced set of modules */
#define NEXUS_NUM_RFM_OUTPUTS 1
#endif

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
#define NEXUS_NUM_DISPLAYS 2
#define NEXUS_NUM_VIDEO_WINDOWS   2
#define NEXUS_NUM_DNR 1
#define NEXUS_NUM_DCR 1
#define NEXUS_NUM_VIDEO_DACS 6


/* Display Settings VDC Memory allocations,see 7405_Memory_Worksheet.xls */
#define NEXUS_UMA64_STR  "64 bit UMA,Display HD+SD,656 input,MAD,PIP(HD+SD),lipsync,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA64          10
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA64          0
#define NEXUS_DISPLAY_NUM_FULLHD_BUFFERS_UMA64      6
#define NEXUS_NUM_VIDEO_WINDOWS_UMA64   NEXUS_NUM_VIDEO_WINDOWS
#define NEXUS_NUM_DISPLAYS_UMA64        NEXUS_NUM_DISPLAYS

#define NEXUS_UMA32_STR "32 bit UMA,Display HD+SD,656 input,MAD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA32          0
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA32          0
#define NEXUS_DISPLAY_NUM_FULLHD_BUFFERS_UMA32      6
#define NEXUS_NUM_VIDEO_WINDOWS_UMA32   1
#define NEXUS_NUM_DISPLAYS_UMA32        NEXUS_NUM_DISPLAYS


#define NEXUS_UMA16_STR "16 bit UMA,Display SD,656 input,MAD,SD only decode"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_UMA16          8
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_UMA16          0
#define NEXUS_DISPLAY_NUM_FULLHD_BUFFERS_UMA16      0
#define NEXUS_NUM_VIDEO_WINDOWS_UMA16   1
#define NEXUS_NUM_DISPLAYS_UMA16        1


#define NEXUS_NONUMA3216_STR  "32+16 bit NonUMA,Display HD+SD,656 input,MAD,PIP(HD+SD),lipsync,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_NONUMA3216      10
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_NONUMA3216      0
#define NEXUS_DISPLAY_NUM_FULLHD_BUFFERS_NONUMA3216  6
#define NEXUS_NUM_VIDEO_WINDOWS_NONUMA3216   NEXUS_NUM_VIDEO_WINDOWS
#define NEXUS_NUM_DISPLAYS_NONUMA3216        NEXUS_NUM_DISPLAYS

#define NEXUS_NONUMA1616_STR "16+16 bit non UMA,Display HD+SD,656 input,MAD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_NONUMA1616      0
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_NONUMA1616      0
#define NEXUS_DISPLAY_NUM_FULLHD_BUFFERS_NONUMA1616  5
#define NEXUS_NUM_VIDEO_WINDOWS_NONUMA1616   1
#define NEXUS_NUM_DISPLAYS_NONUMA1616    NEXUS_NUM_DISPLAYS


/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1

#if 0 /* removed to bring up nexus with reduced set of modules */
#if NEXUS_BOARD_7405_MSG
    /* 7405-MSG borad has three 4506s */
    #define NEXUS_NUM_FRONTEND_CARD_SLOTS 3
    #define NEXUS_MAX_FRONTENDS (2*NEXUS_NUM_FRONTEND_CARD_SLOTS)
#elif NEXUS_BOARD_7405_IFE
     #define NEXUS_MAX_FRONTENDS 4
#else
    /* The 97405 has two dual-channel slots and a single onboard 3510 */
    #define NEXUS_NUM_FRONTEND_CARD_SLOTS 2
    #define NEXUS_MAX_FRONTENDS 1+(2*NEXUS_NUM_FRONTEND_CARD_SLOTS)
#endif
#endif

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 8
#define NEXUS_NUM_GPIO_PINS 113

#if 0 /* removed to bring up nexus with reduced set of modules */
/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 4
#endif

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 1

/* UARTS */
#define NEXUS_NUM_UARTS 3

/* UHF INPUTS */
#if 0 /* removed to bring up nexus with reduced set of modules */
#define NEXUS_NUM_UHF_INPUTS 1
#endif

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 2


/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_BASE (0x19800000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

