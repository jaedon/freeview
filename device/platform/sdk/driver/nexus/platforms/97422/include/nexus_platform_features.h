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
 * $brcm_Revision: 26 $
 * $brcm_Date: 7/17/12 3:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97422/include/nexus_platform_features.h $
 * 
 * 26   7/17/12 3:24p jessem
 * SW7425-3489: Changed NEXUS_NUM_STC_CHANNELS to 16.
 *
 * 25   4/27/12 12:38p jessem
 * SW7422-470: Updated for Bx.
 *
 * 24   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 *
 * 23   9/26/11 3:21p jessem
 * SW7425-1248: Updated I2C channels to support B0.
 *
 * 22   4/25/11 1:59p nickh
 * SW7422-10: Increase memory for SVC 3D
 *
 * 21   4/16/11 2:09p nickh
 * SW7425-313: Add number of MTSIF interfaces
 *
 * 20   3/31/11 5:12p nickh
 * SW7422-171: Add correct number of frontends and gpio shared interrupt
 *  support for DBS builds
 *
 * 19   3/17/11 1:17p erickson
 * SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
 *  nexus_platform_features.h
 *
 * 18   3/4/11 3:05p jgarrett
 * SW7422-171: Adding support for 3128 daughtercard
 *
 * 17   2/7/11 12:27p nickh
 * SW7422-2: Required for edge cases when decoding HD stream on PIP and
 *  output resolution is 1080p (i.e. PIP swap, PIP PIG mode, HDMI hotplug)
 *
 * 16   12/29/10 2:02p nickh
 * SW7422-2: Rename define for graphics heap to correctly point to MEMC0
 *  rather than MEMC1
 *
 * 15   12/27/10 2:37p erickson
 * SW7422-2: switch default config to use unmapped eDeviceOnly memory for
 *  picture buffers
 *
 * 14   12/27/10 1:42p erickson
 * SW7422-2: add NEXUS_Platform_P_GetDefaultSettings
 *
 * 13   12/22/10 4:06p erickson
 * SW7422-2: remove unused NEXUS_MEMC1_PHYSICAL_ADDRESS
 *
 * 12   12/13/10 2:55p nickh
 * SW7422-10: Update memory requirements for SVC required by XVD and 3D
 *  buffers required for MCVP
 *
 * 11   12/2/10 2:53p nickh
 * SW7422-10: Increase memory to account for still picture allocation in
 *  Brutus
 *
 * 10   12/2/10 1:45p nickh
 * SW7422-10: Increase memory due to SVC/MVC requirements
 *
 * 10   12/2/10 1:44p nickh
 * SW7422-10: Increase memory due to SVC/MVC requirements
 *
 * 10   12/2/10 1:44p nickh
 * SW7422-10: Increase memory due to SVC/MVC requirements
 *
 * 9   11/12/10 10:29a rgreen
 * SW7422-10: Merge changes; restore hdmiInput support
 *
 * SW7422-10/1   11/10/10 2:20p rgreen
 * SW7422-10: Add nexus platform support for hdmiInput
 *
 * 8   11/6/10 10:23a nickh
 * SW7422-10: Temporarily remove hdmi input for the bringup
 *
 * 7   10/21/10 6:08p nickh
 * SW7422-10: Add number of HW still image decoders
 *
 * 6   10/19/10 3:24p nickh
 * SW7422-10: Add VDC heap requirements
 *
 * 5   10/14/10 11:55a nickh
 * SW7422-10: Add AON gpio support
 *
 * 4   10/12/10 7:14p nickh
 * SW7422-10: Add HDMI input support
 *
 * 3   10/1/10 12:19p nickh
 * SW7422-10: Change video decoder heap defines for better clarity
 *
 * 2   9/29/10 5:59p nickh
 * SW7422-10: Fix Virtual Address so that it maps to kuseg region and has
 *  at least 256MB
 *
 * 1   9/24/10 6:26p nickh
 * SW7422-10: Initial nexus platform features file
 *
 * Module Description:
 *
 * Revision History:
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
#define NEXUS_NUM_INPUT_BANDS 10
#define NEXUS_NUM_PARSER_BANDS 16
#if (BCHP_VER >= BCHP_VER_B0)
#define NEXUS_NUM_PID_CHANNELS 512
#define NEXUS_NUM_RAVE_CONTEXTS 48
#else
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_RAVE_CONTEXTS 24
#endif
#define NEXUS_NUM_TIMEBASES 8
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 16
#define NEXUS_NUM_REMUX 2
#define NEXUS_NUM_MTSIF 2
#define NEXUS_NUM_PACKET_SUB 8

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 2
#define NEXUS_NUM_XVD_DEVICES 2
#define NEXUS_NUM_MOSAIC_DECODES 14
#define NEXUS_NUM_STILL_DECODES 1
#define NEXUS_SVC_MVC_SUPPORT 1


/* XVD Heaps
   TODO: Add correct XVD memory sizes when SVC 3D numbers are available
See 7422_Memory_Worksheet.xls to calculate custom numbers */
/* 32(MEMC0)+32(MEMC1) bit non UMA - 2 HD Decodes, 1 HD Still, 1 SD Still 11 CIF , 14 QCIF
   SVD1: Includes support for SVC 2D/3D and MVC
   AVD0: Includes support for AVC Level 4.2 */
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC0_GENERAL_HEAP_SIZE (21*1024*1024)
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC1_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE (55*1024*1024)

#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_GENERAL_HEAP_SIZE (16*1024*1024)
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_SECURE_HEAP_SIZE  (0)
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE (43*1024*1024)

/* Audio Features */
#define NEXUS_NUM_AUDIO_DSP 1
#define NEXUS_NUM_AUDIO_DECODERS 6
#define NEXUS_NUM_AUDIO_DACS 2
#define NEXUS_NUM_SPDIF_INPUTS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 4
#define NEXUS_NUM_AUDIO_MIXERS 8
#define NEXUS_NUM_AUDIO_INPUT_CAPTURES 3

#define NEXUS_NUM_I2S_INPUTS 1
#define NEXUS_NUM_I2S_OUTPUTS 2
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_CAPTURES 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 3

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

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
#define NEXUS_NUM_DISPLAYS 3
#define NEXUS_NUM_VIDEO_WINDOWS   2
#define NEXUS_NUM_DNR 1
#define NEXUS_NUM_DCR 1
#define NEXUS_NUM_VIDEO_DACS 4
#define NEXUS_NUM_LAB 2
#define NEXUS_NUM_CAB 2
#define NEXUS_NUM_PEP 1
#define NEXUS_HAS_TNT 1


/*  - Display buffers required for main memory heap on MEMC1
 *  - The main path, main PIP, secondary path, and secondary PIP path
 *    are allocated from the main VDC heap on MEMC1 */
#define NEXUS_32MEMC0_32MEMC1_STR  "32+32 bit NonUMA,Display HD+HD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1              0
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1          5
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1         8
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1     13
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1              12
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1          0

#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0              0
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0         0
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0     0
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0              0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0          0

/* Still Picture Decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

#if defined(NEXUS_PLATFORM_7425_DBS)
/* DBS Frontend */
#define NEXUS_MAX_FRONTENDS 8
#else
/* Cable Frontend */
#if defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)
#define NEXUS_MAX_FRONTENDS 17
#else
#define NEXUS_MAX_FRONTENDS 9
#endif

#if NEXUS_USE_FRONTEND_DAUGHTER_CARD
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 1
#endif
#endif

#define NEXUS_MAX_3255_ADSCHN 0
#define NEXUS_3255_OOB_TUNER_IFFREQ (1250000)/* 1.25 MHz */
#define NEXUS_SHARED_FRONTEND_INTERRUPT 1

/* SPI */
#define NEXUS_NUM_SPI_CHANNELS 2

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 6
#define NEXUS_NUM_GPIO_PINS 111

/* AON GPIO */
#define NEXUS_NUM_AON_SGPIO_PINS 4
#define NEXUS_NUM_AON_GPIO_PINS 18
#define NEXUS_GpioType_eAonStandard 4
#define NEXUS_GpioType_eAonSpecial 5

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 5

/* I2C channel usage assignments. Refer to BSC table in the board schematics. */
#if (BCHP_VER >= BCHP_VER_B0)
#define NEXUS_I2C_CHANNEL_HDMI_TX         0
#define NEXUS_I2C_CHANNEL_HDMI_RX         1  /* Unused */
/*#define NEXUS_I2C_CHANNEL_MOCA          2     BCM3450. Used by the kernel. */
#define NEXUS_I2C_CHANNEL_LNA             3  /* BCM3405 */
#define NEXUS_I2C_CHANNEL_EXT_RFM         3  /* External RFM */
#define NEXUS_I2C_CHANNEL_TUNERS_4_5      3  /* BCM3112_4/_5(for SV board) */
#define NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3  4  /* BCM3112_0/_1/_2/_3(for SV board) */
#define NEXUS_I2C_CHANNEL_DSTRM_TUNER     4  /* BCM3128(for VMS board) */
#else
#define NEXUS_I2C_CHANNEL_HDMI_TX         3
#define NEXUS_I2C_CHANNEL_HDMI_RX            /* Unused */
/* #define NEXUS_I2C_CHANNEL_MOCA               BCM3450. Used by the kernel. */
#define NEXUS_I2C_CHANNEL_LNA             1  /* BCM3405 */
#define NEXUS_I2C_CHANNEL_EXT_RFM         1  /* External RFM */
#define NEXUS_I2C_CHANNEL_DSTRM_TUNER     2  /* BCM3128(for VMS board) */
#define NEXUS_I2C_CHANNEL_TUNERS_4_5      1  /* BCM3112_4/_5(for SV board) */
#define NEXUS_I2C_CHANNEL_TUNERS_0_1_2_3  2  /* BCM3112_0/_1/_2/_3(for SV board) */
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

/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_BASE (0x19800000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

/* Memory features */
#define NEXUS_NUM_MEMC 2

/* default heap indices */
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC1_MAIN_HEAP           1
#define NEXUS_MEMC0_GRAPHICS_HEAP       2
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 3
#define NEXUS_MEMC1_PICTURE_BUFFER_HEAP 4

#define NEXUS_AVS_MONITOR           1


#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

