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
 * $brcm_Revision: 66 $
 * $brcm_Date: 10/18/12 2:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97425/include/nexus_platform_features.h $
 *
 * 66   10/18/12 2:52p jessem
 * SW7425-3806: Updated heap settings per new 866MHz RTS.
 *
 * 65   7/17/12 3:24p jessem
 * SW7425-3489: Changed NEXUS_NUM_STC_CHANNELS to 16.
 *
 * 64   6/5/12 2:57p jtna
 * SW7425-2752: rework satellite frontend MTSIF connection
 *
 * 63   4/17/12 4:33p hongtaoz
 * SW7425-2896: Bx has 16 playpumps;
 *
 * 62   4/17/12 12:15p jessem
 * SW7425-2891: Updated display buffer counts.
 *
 * 61   4/6/12 1:14p jessem
 * SW7425-2845: Set NEXUS_MAX_FRONTENDS to 7 in order to use only 4 311x
 *  tuners.
 *
 * 60   3/6/12 7:04p hongtaoz
 * SW7425-2576: fixed number of stc channels;
 *
 * 59   2/25/12 4:35p hongtaoz
 * SW7231-68: added SPI macro for 7425 to fix compile error;
 *
 * 58   2/13/12 2:00p jessem
 * SW7425-2379: Updated SGPIO and AON SGPIO pin counts.
 *
 * 57   2/13/12 1:52p gmullen
 * SW7425-2376: Added support for RAVE contexts 24-47
 *
 * 56   2/9/12 2:26p jgarrett
 * SW7425-2074: Adding NEXUS_NUM_SPDIF_INPUTS
 *
 * 55   2/9/12 2:24p jgarrett
 * SW7425-2074: Merge to main branch
 *
 * SW7425-2074/2   2/3/12 5:19p jgarrett
 * SW7425-2074: Rebasing to latest code
 *
 * SW7425-2074/1   1/18/12 6:06p jgarrett
 * SW7425-2074: Initial version of InputCapture API
 *
 * 54   2/1/12 7:06p nickh
 * SW7425-2318: Add support for 8 tuners from 3383 + 8 tuners from 3128 +
 *  1 OOB
 *
 * 53   12/19/11 11:16a erickson
 * SW7405-5325: add NEXUS_NUM_PACKET_SUB
 *
 * 52   12/15/11 6:05p vishk
 * SW3128-92: Add FW auto detection support. Add nexus support. Fix
 *  compile error.
 *
 * 51   12/15/11 5:58p vishk
 * SW3128-92: Add FW auto detection support. Add nexus support.
 *
 * 50   12/15/11 4:26p vsilyaev
 * SW7425-1140: Merge NEXUS_Cec module support into main-line
 *
 * SW7425-1140/1   12/9/11 5:11p vle
 * SW7425-1140: add NEXUS Cec module
 *
 * 49   12/15/11 4:13p gmullen
 * HW7425-1351: Fixed incorrect number of PID channels for Bx parts
 *
 * 48   11/8/11 2:35p hongtaoz
 * SW7425-1235: extend number of audio decoders to 6 for 7425 platform;
 *
 * 47   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 *
 * 46   10/18/11 3:05p hongtaoz
 * SW7425-1402: changed memc1 gfx heap index to make gfx heap base address
 *  256MB aligned;
 *
 * 45   10/13/11 11:06p hongtaoz
 * SW7425-1349: added audio dummy outputs;
 *
 * 44   10/11/11 6:52p vishk
 * SW3128-46: add support for BCM3124 devices
 *
 * 43   10/10/11 11:56a jessem
 * SWNOOS-489: Added DBS frontend support.
 *
 * 42   9/30/11 3:42p hongtaoz
 * SW7425-1320, SW7425-1322: added display ID macroes for dual transcodes;
 *
 * 41   9/19/11 5:46p hongtaoz
 * SW7425-1322: added nexus platform support for dual transcode;
 *
 * 40   9/15/11 10:56a jessem
 * SW7425-1248: Updated I2C channel for VMS frontend tuner.
 *
 * 39   9/14/11 2:31p jessem
 * SW7425-1248: Corrected I2C channels for Ax.
 *
 * 38   9/13/11 7:00p jessem
 * SW7425-1248: Updated pinmux and I2C usage for B0.
 *
 * 37   8/9/11 10:34a jessem
 * SW7425-1069: Added AVS (PM) monitoring feature.
 *
 * 36   7/11/11 6:12p vanessah
 * SW7425-689: sync with application and PI MADR compression
 *
 * 35   6/23/11 7:16p hongtaoz
 * SW7425-466: roll up XVD general heap size to accomandate some internal
 *  fw allocation;
 *
 * 34   6/23/11 6:12p hongtaoz
 * SW7425-466: remove fw size from video decoder general heap;
 *
 * 33   6/23/11 3:15p hongtaoz
 * SW7425-466: updated 7425 default video decoder heap allocation sizes
 *  for 3x decodes support;
 *
 * 32   6/3/11 5:13p vanessah
 * SW7425-465: add one more HD buffer for MADR and change the default MADR
 *  compression value to 20 to be consistent with PI
 *
 * 31   5/12/11 3:54p hongtaoz
 * SW7425-466: rework avd/mfd/decoder/xdm mapping; add 3x decodes; added
 *  1xHD buffer for MCVP QM store;
 *
 * SW7425-466/1   5/11/11 12:59p hongtaoz
 * SW7425-466: rework avd/mfd/decoder channel mapping;
 *
 * 30   4/25/11 2:00p nickh
 * SW7425-9: Increase memory for SVC 3D
 *
 * 29   4/19/11 4:28p spothana
 * SW7425-369 : Correct the number of DOCSIS InBand channels
 *
 * 28   4/18/11 4:47p spothana
 * SW7425-369 : Remove BCM3128 Card support.
 *
 * 27   4/16/11 1:46p hongtaoz
 * SW7425-313: added MTSIF support;r
 *
 * 26   4/15/11 7:56p vanessah
 * SW7425-317: MADR RTS need compression to be turn on due to BW limit
 *
 * 25   4/8/11 11:52a vishk
 * SW7425-313 : Add support for VMSSFF board with 3128 frontend.
 *
 * 24   4/7/11 2:59p hongtaoz
 * SW7425-165, SW7425-52: allocate more heap for MADR (4-field pixel + 2-
 *  field QM);
 *
 * 23   4/4/11 6:01p hongtaoz
 * SW7425-1: update encoder secure buffer allocation to match PI;
 *
 * 22   3/17/11 1:17p erickson
 * SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
 *  nexus_platform_features.h
 *
 * 21   3/9/11 10:37a VISHK
 * SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
 *  board) plug on 7422 board  Adding support for 97425 platform.
 *
 * 20   3/3/11 4:02p jessem
 * SW7425-132: Changed NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1 to 0.
 *
 * 19   3/1/11 6:49p vishk
 * SW7422-272: Add nexus gpio support for frontend 4506 tuners
 *
 * 18   2/28/11 7:36p hongtaoz
 * SW7425-113, SW7425-121: updated BVN clients memc0/1 assignments;
 *
 * 17   2/28/11 4:22p vanessah
 * SW7425-132: brutus pip swap run out of memory
 *
 * 16   2/13/11 12:15a hongtaoz
 * SW7425-96: separate ViCE2 fw heap from picture heap;
 *
 * 15   2/4/11 4:32p hongtaoz
 * SW7425-9: reduced ViCE2 heap sizes according to the new FW;
 *
 * 14   1/13/11 5:17p hongtaoz
 * SW7425-9: partition MEMC0 high mem region for video encoder secure heap
 *  and decoder general heap; expanded AVD1 picture heap size to bring up
 *  PIP;
 *
 * 13   12/30/10 11:23p hongtaoz
 * SW7425-9: updated vce heap size for bin buffer workaround;
 *
 * 12   12/30/10 2:06p erickson
 * SW7422-2: update mmap
 *
 * 11   12/30/10 11:01a jgarrett
 * SW7425-41: Moving definition of NEXUS_HAS_AUDIO_MUX_OUTPUT to
 *  platform_features
 *
 * 10   12/27/10 4:23p vsilyaev
 * SW7425-40: Updated video encoder memory size to match expectations of
 *  FW/PI
 *
 * 9   12/27/10 11:58a erickson
 * SW7422-2: introduce macros for default heap indices
 *
 * 8   12/22/10 4:06p erickson
 * SW7422-2: remove unused NEXUS_MEMC1_PHYSICAL_ADDRESS
 *
 * 7   12/15/10 4:07p hongtaoz
 * SW7425-9: added chip specific video encoder display index macro;
 *
 * 6   12/13/10 1:03p hongtaoz
 * SW7425-9: updated display heap allocation number; added ViCE2 heap
 *  allocation number;
 *
 * 5   12/9/10 7:06p hongtaoz
 * SW7425-9: updated the heap allocation numbers;
 *
 * 4   11/24/10 4:40p hongtaoz
 * SW7425-9: updated 7425 features and heap allocation;
 *
 * 3   11/3/10 3:44p hongtaoz
 * SW7425-9: add picture decoder feature for 7425;
 *
 * 2   10/15/10 6:40p hongtaoz
 * SW7425-9: changed MEMC1 physical base address; changed some of platform
 *  features macroes for 7425 to reflect hw;
 *
 * 1   9/13/10 4:37p hongtaoz
 * SW7425-9: add initial version for 7425 support;
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
#if defined(HUMAX_PLATFORM_BASE)
#if defined(CONFIG_BCM_MTSIF)
#define NEXUS_NUM_PARSER_BANDS 8
#else
#define NEXUS_NUM_PARSER_BANDS 11
#endif
#else
#define NEXUS_NUM_PARSER_BANDS 16
#endif

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
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_NUM_PLAYPUMPS 13 /*thers's a problem with ppump 13 & 14. limit the total pump number till things clear.*/
#else
#define NEXUS_NUM_PLAYPUMPS 16
#endif
#define NEXUS_NUM_REMUX 2
#define NEXUS_NUM_MTSIF 2
#define NEXUS_NUM_PACKET_SUB 8

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 3
#define NEXUS_NUM_XVD_DEVICES 2
#if defined(CONFIG_TEMP_HMSLITE)
#define NEXUS_NUM_MOSAIC_DECODES 0
#else
#define NEXUS_NUM_MOSAIC_DECODES 14
#endif
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
#define NEXUS_VIDEO_DECODER_SVD0_32MEMC1_PICTURE_HEAP_SIZE (57.89*1024*1024) /* with efficient memory app, it could be reduced to 55 */

#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_GENERAL_HEAP_SIZE (32.23*1024*1024) /* support 2x HD decodes */
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_SECURE_HEAP_SIZE  (0)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if defined(CONFIG_BCM_TWO_TRANSCODER)
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE (88.7*1024*1024)
#else
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE (45.8*1024*1024)
#endif
#else
#define NEXUS_VIDEO_DECODER_AVD1_32MEMC0_PICTURE_HEAP_SIZE (45.7*1024*1024)
#endif

/* Audio Features */
#define NEXUS_NUM_AUDIO_DSP 1
#define NEXUS_NUM_AUDIO_DECODERS 6
#define NEXUS_NUM_AUDIO_DACS 2
#if defined(CONFIG_TEMP_HMSLITE)
#define NEXUS_NUM_SPDIF_INPUTS 0
#else
#define NEXUS_NUM_SPDIF_INPUTS 1
#endif
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_DUMMY_OUTPUTS 4
#define NEXUS_NUM_AUDIO_MIXERS 8
#define NEXUS_NUM_AUDIO_INPUT_CAPTURES 3

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
#define NEXUS_NUM_VCE_DEVICES    1
#if (BCHP_VER >= BCHP_VER_B0)
#define NEXUS_NUM_VCE_CHANNELS  2 /* dual per device */
#else
#define NEXUS_NUM_VCE_CHANNELS  1
#endif
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
#define NEXUS_NUM_DISPLAYS 4
#define NEXUS_NUM_VIDEO_WINDOWS   2
#define NEXUS_NUM_DNR 2
#define NEXUS_NUM_DCR 2
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

/* 7425B0:
       display3 -> STG0 -> encoder0; 1st encoder connected with display 3
       display2 -> STG1 -> encoder1; 2nd encoder mutual exclusive with PIP window and local SD display2
       display1 -> local SD display
       display0 -> local HD display
 */
#define NEXUS_ENCODER0_DISPLAY_IDX 3
#define NEXUS_ENCODER1_DISPLAY_IDX 2

/* backwards compatible */
#define NEXUS_ENCODER_DISPLAY_IDX NEXUS_ENCODER0_DISPLAY_IDX

/* 	- Display buffers required for main memory heap on MEMC1
 *  - Except the main path, all other paths
 *    are allocated from the MEMC1 */
#define NEXUS_32MEMC0_32MEMC1_STR  "32+32 bit NonUMA,Display HD+HD,1080p"
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC0              0
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC0          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC0        (8+4) /* 1080p3D C0V0 with ANR, 4 buffers for 3D */
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC0    (4)   /* C1V0, for 1080p24/25/30/ */
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC0              2    /* MCVP QM store */
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC0          0

/*  - Buffers for the main path are
 *    allocated from VDC heap on MEMC0 when the window is created */
#define NEXUS_DISPLAY_NUM_SD_BUFFERS_MEMC1              4    /* 1080p source SD PIP C1V1 capture buffer */
#define NEXUS_DISPLAY_NUM_SD_PIP_BUFFERS_MEMC1          0
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1         8    /* HD xcode C3V0 with MADR */
#else
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS_MEMC1         6    /* HD xcode C3V0 with MADR */
#endif
#define NEXUS_DISPLAY_NUM_FULL_HD_PIP_BUFFERS_MEMC1    (8)   /* 1080p3D PIP C0V1 */
#define NEXUS_DISPLAY_NUM_HD_BUFFERS_MEMC1  		    0
#define NEXUS_DISPLAY_NUM_HD_PIP_BUFFERS_MEMC1 	        0

/* Still Picture Decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

/* Cable Frontend */
/* The maximum number of input bands on 7425 are 9. Also 3128 has 8 DS + 1 OB */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_CABLE_MODEM)
#define NEXUS_MAX_FRONTENDS 9
#else
#define NEXUS_MAX_FRONTENDS 4
#endif
#else
#if defined(NEXUS_USE_7425_SV_BOARD)
#define NEXUS_MAX_FRONTENDS 7
#elif defined(NEXUS_PLATFORM_7425_DBS)
#define NEXUS_MAX_FRONTENDS 8
#elif defined(NEXUS_PLATFORM_4528_DBS)
#define NEXUS_MAX_FRONTENDS 16
#else

#if defined(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT)
#define NEXUS_MAX_FRONTENDS 17
#else
#define NEXUS_MAX_FRONTENDS 9
#endif

#if NEXUS_USE_FRONTEND_DAUGHTER_CARD
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 1
#endif
#endif
#endif

#define NEXUS_MAX_3255_ADSCHN 8
#define NEXUS_3255_OOB_TUNER_IFFREQ (1250000)/* 1.25 MHz */

#define NEXUS_SHARED_FRONTEND_INTERRUPT 1

/* SPI */
#define NEXUS_NUM_SPI_CHANNELS 2

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 4
#define NEXUS_NUM_GPIO_PINS 111

/* AON GPIO */
#define NEXUS_NUM_AON_SGPIO_PINS 6
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
#if defined(CONFIG_TEMP_HMSLITE)
#define NEXUS_NUM_SMARTCARD_CHANNELS 0
#else
#define NEXUS_NUM_SMARTCARD_CHANNELS 2
#endif

#endif

/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_BASE (0x19800000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

/* Memory features */
#define NEXUS_NUM_MEMC 2

/* default heap indices */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_GRAPHICS3D)
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_DRIVER_HEAP         1
#define NEXUS_MEMC0_GRAPHICS_HEAP       2 /* make gfx heap aligned by 256MB for better of v3d purpose */
#define NEXUS_MEMC1_GRAPHICS_HEAP       3 /* make gfx heap aligned by 256MB for better of v3d purpose */
#define NEXUS_MEMC1_MAIN_HEAP           4
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 5
#define NEXUS_MEMC1_PICTURE_BUFFER_HEAP 6
#else
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_GRAPHICS_HEAP       1 /* make gfx heap aligned by 256MB for better of v3d purpose */
#define NEXUS_MEMC1_GRAPHICS_HEAP       2 /* make gfx heap aligned by 256MB for better of v3d purpose */
#define NEXUS_MEMC1_MAIN_HEAP           3
#define NEXUS_MEMC0_PICTURE_BUFFER_HEAP 4
#define NEXUS_MEMC1_PICTURE_BUFFER_HEAP 5
#define NEXUS_MEMC0_DRIVER_HEAP         6
#endif

#if defined(HUMAX_PLATFORM_BASE)
#undef NEXUS_NUM_COMPONENT_OUTPUTS
#undef NEXUS_NUM_COMPOSITE_OUTPUTS
#define NEXUS_NUM_COMPONENT_OUTPUTS CONFIG_BCM_COMPONENT_COUNT
#define NEXUS_NUM_COMPOSITE_OUTPUTS CONFIG_BCM_COMPOSITE_COUNT
#endif

#define NEXUS_AVS_MONITOR           1


#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */
