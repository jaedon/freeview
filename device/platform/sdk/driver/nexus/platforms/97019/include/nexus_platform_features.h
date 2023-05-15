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
* $brcm_Revision: 24 $
* $brcm_Date: 2/24/12 11:46a $
*
* API Description:
*   API name: Platform Features
*    This file defines a set of controllable fields for a 7400 platform.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97019/include/nexus_platform_features.h $
* 
* 24   2/24/12 11:46a mward
* SW7420-2256:  Define NEXUS_NUM_CEC.
* 
* 23   8/19/11 9:57a mward
* SW7125-4: Remove misleading comment about NEXUS_NUM_AUDIO_DACS.
* 
* 22   6/15/11 10:22a mward
* SW7125-981:  RAP can support 4 audio playbacks.
* 
* 21   5/12/11 7:10p mward
* SW7125-905:  Add "skip init" versions of host-controlled 7125
*  (internal) frontends.
* 
* 20   4/6/11 2:18p mward
* SW7125-899:  Define indices for dual BMEM heaps.
* 
* 19   3/11/11 3:35p mward
* SW7125-831:  Define NEXUS_NUM_AUDIO_DECODERS 3 for Audio Descriptor.
*  (merge unnecessary branch.)
* 
* SW7125-828/1   2/14/11 4:40p mward
* SW7125-831:  Define NEXUS_NUM_AUDIO_DECODERS 3 for Audio Descriptor.
* 
* 18   2/1/11 6:20p mward
* SW7125-769:  Fix specific frontend defines.
* 
* 17   1/27/11 11:35a mward
* SW7125-769:  Specific frontend IDs for use with
*  NEXUS_Platform_OpenFrontend(). (merge)
* 
* SW7125-769/1   1/24/11 11:32a mward
* SW7125-769:  Specific frontend IDs for use with
*  NEXUS_Platform_OpenFrontend().
* 
* 16   10/22/10 10:22a mward
* SW7125-656:  VDC workaround allows MAD compression.
* 
* 15   10/21/10 5:49p mward
* SW7125-656:  Temporarily disable MAD compression.  Use CFE 3.15, CFE
*  3.14 RTS requires MAD compression.
* 
* 14   9/17/10 12:08p mward
* SW7125-424:  Add NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL to
*  nexus_platform_features.h to use deinterlacer compression on platforms
*  that need it, such as 97125.
* 
* 13   8/31/10 9:50a mward
* SW7125-497: Setting NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY 1 to
*  allow host use of DS1 during standby.
* 
* 12   6/30/10 2:42p mward
* SW7125-35:  97019 platform must use NEXUS_MAX_FRONTENDS 4
* and NEXUS_MAX_3255_ADSCHN 4, even though only 2 internal DS are
*  available.
* 
* 11   6/23/10 11:55a mward
* SW7125-35:  Correct number of frontends and order of use for 97019
*  platform.
* 
* 10   6/9/10 3:41p mward
* SW7125-460:  DVB_CI values.
* 
* 9   4/23/10 2:39p mward
* SW7125-373:  7019 has 4 number of ADS channels like 7125.
* 
* 8   3/17/10 2:30p mward
* SW7125-234: Add PEP related defines after Picture control refactoring.
* 
* 5   11/3/09 3:11p mward
* SW7125-27, SW7125-52:  Only one video window per display if only one
*  video decoder.
* 
* 3   8/26/09 4:27p mward
* SW7125-4: Update.
* 
* 2   8/26/09 2:14p mward
* SW7420-228: move NEXUS_NUM_MEMC to nexus_platform_features.h
* 
* 2   8/19/09 4:01p mward
* PR55545: Support for 7125.  Correct some items copied from 7400.
* 
* 1   7/24/09 6:40p mward
* PR 55545: Initial version.
* 
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

#ifdef NEXUS_BASE_ONLY
#define NEXUS_NUM_L1_REGISTERS 3
#else

/* Transport Features */
#define NEXUS_NUM_INPUT_BANDS 6
#define NEXUS_NUM_PARSER_BANDS 6
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_TIMEBASES 2
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 2
#define NEXUS_NUM_REMUX 2

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 1    
#define NEXUS_NUM_MOSAIC_DECODES 12
#define NEXUS_NUM_STILL_DECODES 1


/* XVD Heaps
See 7405_Memory_Worksheet.xls to calculate custom numbers */
/* 32 bit UMA 1 HD Decode 1 HD Still,6 CIF , 12 QCIF */
#define NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE (36*1024*1024)
#define NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE (0)
#define NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE  (0)

/* Audio Features */
#define NEXUS_NUM_AUDIO_DECODERS 3
#define NEXUS_NUM_AUDIO_DACS 1
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 4

/* #define NEXUS_NUM_I2S_INPUTS 1 */
#ifndef NEXUS_NUM_I2S_OUTPUTS
#define NEXUS_NUM_I2S_OUTPUTS 2
#endif
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 4
#define NEXUS_NUM_AUDIO_CAPTURES 1

/* Display Features */
/* TODO: Add VDC memory allocations */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 0
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_CEC 1
#define NEXUS_NUM_HDMI_INPUTS 0
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 1
#define NEXUS_NUM_RFM_OUTPUTS 0

/* Max number of displays and windows supported
by this platform. Actual numbers may vary depeding
upon the chip usage. See below */
#define NEXUS_NUM_DISPLAYS 2
#define NEXUS_NUM_VIDEO_WINDOWS   1
#define NEXUS_NUM_DNR 1
#define NEXUS_NUM_DCR 1
#define NEXUS_NUM_VIDEO_DACS 6
#define NEXUS_NUM_LAB 2
#define NEXUS_NUM_CAB 2
#define NEXUS_NUM_PEP 1


/* Display Settings VDC Memory allocations, temporarily from 7405_Memory_Worksheet.xls */
#define NEXUS_DISPLAY_NUM_SD_BUFFERS          10
#define NEXUS_DISPLAY_NUM_HD_BUFFERS          0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS      6

/* This sets the madSettings.stVideoTestFeature1.ulBitsPerPixel to use reduced bits/pixel in the deinterlacer .
9.5 bit/pixel has been found to have negligible impact on quality, while freeing significant bandwidth
for other uses.  The unit of ulBitsPerPixel is 0.5 bits.  CFE 3.14 requires this compression, CFE 3.15 does not.
*/
#define NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL 19

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1

/* Cable Frontend */
#define NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY 0
#define NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY 1
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 0
#define NEXUS_MAX_FRONTENDS 4
#define NEXUS_MAX_3255_ADSCHN 4

#define NEXUS_FRONTEND_ID_HOST_CONTROLLED 16
#define NEXUS_FRONTEND_ID_HOST_CONTROLLED_SKIP_INIT 8
/* Specfic frontend IDs for use with NEXUS_Platform_OpenFrontend() */
/* saved as id in NEXUS_FrontendUserParameters */
#define NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1    1
#define NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0    3

/* HOST_CONTROLLED and CM_CONTROLLED frontends cannot be open simultaneously */
#define NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1  (NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1+NEXUS_FRONTEND_ID_HOST_CONTROLLED)
#define NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0  (NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0+NEXUS_FRONTEND_ID_HOST_CONTROLLED)
#define NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS1_SKIP_INIT  (NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS1+\
															  NEXUS_FRONTEND_ID_HOST_CONTROLLED+\
															  NEXUS_FRONTEND_ID_HOST_CONTROLLED_SKIP_INIT)
#define NEXUS_FRONTEND_ID_HOST_CONTROLLED_INTERNAL_7125_DS0_SKIP_INIT  (NEXUS_FRONTEND_ID_CM_CONTROLLED_INTERNAL_7125_DS0+\
															  NEXUS_FRONTEND_ID_HOST_CONTROLLED+\
															  NEXUS_FRONTEND_ID_HOST_CONTROLLED_SKIP_INIT)

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 8
#define NEXUS_NUM_GPIO_PINS 106

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 4

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

#endif /* #if NEXUS_BASE_ONLY */

/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_BASE (0x19800000)
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)

#define NEXUS_NUM_MEMC 1

/* default heap indices */
#define NEXUS_MEMC0_MAIN_HEAP           0
#define NEXUS_MEMC0_GRAPHICS_HEAP       1

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

