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
* $brcm_Revision: 18 $
* $brcm_Date: 2/24/12 4:16p $
*
* API Description:
*   API name: Platform Features
*    This file defines a set of controllable fields for a 7400 platform.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97468/include/nexus_platform_features.h $
* 
* 18   2/24/12 4:16p randyjew
* SW7420-2256: Define NEXUS_NUM_CEC
* 
* 17   12/17/10 1:42p randyjew
*  SW7208-18: Adjust NEXUS_NUM_RECPUMPS to NEXUS_NUM_RAVE_CONTEXTS
* 
* 16   12/3/10 10:18a randyjew
* SW7208-18:Adjust NEXUS_NUM_RECPUMPS
* 
* 15   6/15/10 5:23p randyjew
* SW7208-18: NEXUS_NUM_AUDIO_DECODERS to 3
* 
* 14   6/11/10 12:42p randyjew
* SW7468-247: Add Svideo support for 7468 B0 boards
* 
* 13   6/9/10 11:05a randyjew
* SW7468-247: Moving back NEXUS_NUM_VIDEO_WINDOWS to 1
* 
* 12   5/25/10 11:48a randyjew
* SW7468-6:Update Nexus_Num_Video_Windows to 2
* 
* 11   5/3/10 5:23p randyjew
* SW7468-6:Adjust Max Pid Channels
* 
* 10   2/8/10 3:11p randyjew
* SW7468-115: Add AVS support for 97468
* 
* 9   1/18/10 4:35p randyjew
* SW7468-6: Adjusted HD Buffers settings.
* 
* 8   12/17/09 7:33p randyjew
* SW7468-6:Change to 0 component outputs for 7468
* 
* 7   12/17/09 5:51p randyjew
* 7468-6:update Features
* 
* 6   12/15/09 2:54p randyjew
* SW7468-6:Update Features
* 
* 5   12/9/09 3:58p randyjew
* SW7468-6:Update VideoDacs and FrontEnd
* 
* 4   12/1/09 5:03p randyjew
* SW7468-6: Modify features
* 
* 3   12/1/09 4:51p randyjew
* SW7468-6: Modify features
* 
* 2   10/7/09 12:15p lwhite
* SW7468-6: Modify features
* 
* 
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

#ifdef NEXUS_BASE_ONLY
#define NEXUS_NUM_L1_REGISTERS 3
#else

/* Transport Features */
#define NEXUS_NUM_INPUT_BANDS 2
#define NEXUS_NUM_PARSER_BANDS 2
#define NEXUS_NUM_PID_CHANNELS 128
#define NEXUS_NUM_TIMEBASES 2
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 128
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_RECPUMPS  NEXUS_NUM_RAVE_CONTEXTS /*some RAVE contexts are used for AV decode*/
#define NEXUS_NUM_PLAYPUMPS 2
#define NEXUS_NUM_REMUX 0

/* Video Features,do not change these numbers
   this covers the chip max capabilities  */
#define NEXUS_NUM_VIDEO_DECODERS 1    
#define NEXUS_NUM_MOSAIC_DECODES 0
#define NEXUS_NUM_STILL_DECODES 1


/* XVD Heaps
See 7405_Memory_Worksheet.xls to calculate custom numbers */
/* 32 bit UMA 1 HD Decode 1 HD Still,6 CIF , 12 QCIF */
#define NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE (36*1024*1024)
#define NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE (0)
#define NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE  (0)

/* Audio Features */
#define NEXUS_NUM_AUDIO_DECODERS 3 
#define NEXUS_NUM_AUDIO_DACS 1	/* Should be 2 - Temporary fix to compile */
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 4

/* #define NEXUS_NUM_I2S_INPUTS 1 */
#ifndef NEXUS_NUM_I2S_OUTPUTS
#define NEXUS_NUM_I2S_OUTPUTS 1
#endif
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#define NEXUS_NUM_AUDIO_CAPTURES 1

/* Display Features */
/* TODO: Add VDC memory allocations */
#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 0
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_CEC 1
#define NEXUS_NUM_HDMI_INPUTS 0
#define NEXUS_NUM_COMPONENT_OUTPUTS 0
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
#define NEXUS_NUM_VIDEO_DACS 4


/* Display Settings VDC Memory allocations, temporarily from 7405_Memory_Worksheet.xls */
#define NEXUS_DISPLAY_NUM_SD_BUFFERS          12
#define NEXUS_DISPLAY_NUM_HD_BUFFERS          3
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS      0


/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 0

/* Cable Frontend */
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 1
#define NEXUS_MAX_FRONTENDS 1

/* GPIO */
#define NEXUS_NUM_SGPIO_PINS 4
#define NEXUS_NUM_GPIO_PINS 53

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 2

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 1

/* UARTS */
#define NEXUS_NUM_UARTS 2

/* UHF INPUTS */
#define NEXUS_NUM_UHF_INPUTS 0

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 1

#endif /* #if NEXUS_BASE_ONLY */

#define NEXUS_NUM_MEMC 1

#define NEXUS_AVS_MONITOR 1

#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

