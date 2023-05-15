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
* $brcm_Date: 2/24/12 11:45a $
*
* API Description:
*   API name: Platform Features
*    This file defines a set of controllable fields for a 7400 platform.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/include/nexus_platform_features.h $
* 
* 26   2/24/12 11:45a mward
* SW7420-2256:  Define NEXUS_NUM_CEC.
* 
* 25   5/17/11 1:06p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* SWDTV-6928/1   5/4/11 6:30p mstefan
* SWDTV-6928: merge to main
* 
* 24   1/12/11 7:10p mward
* SW7422-2:  Checked in updated RDB header for 7400d0, BCHP_REGISTER_END
*  workaround is no longer needed.
* 
* 23   1/5/11 6:27p mward
* SW7422-2:  for 7400d0, BCHP_REGISTER_END is not defined as it should be
*  in BCHP.
* 
* 22   12/3/10 10:53a mward
* SW7125-716: NEXUS_NUM_RECPUMPS = NEXUS_NUM_RAVE_CONTEXTS
* 
* 21   7/7/10 11:48a mward
* SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
*  singledecode on 97400.
* 
* 20   9/8/09 5:49p mward
* SW7400-2505: Add TSM log support on Nexus as it existed for Native
*  settop.  Build with BRAP_SUPPORT_TSM_LOG=y.
* 
* 19   8/25/09 1:46p erickson
* SW7420-228: move NEXUS_NUM_MEMC to nexus_platform_features.h
* 
* 18   8/3/09 6:23p mward
* PR 53128: Increase Full HD buffers to accomodate 1080p display formats.
* 
* 17   6/8/09 6:14p mward
* PR55830:  Initial implementation of PCM audio capture for 7400.
* 
* 16   4/7/09 11:42a erickson
* PR53623: change #define to NEXUS_NUM_656_OUTPUTS
* 
* 15   4/1/09 12:15p erickson
* PR53623: merge 656 output support
* 
* PR53623/1   3/31/09 10:08a mward
* PR 53623: 656 output support.
* 
* 14   2/23/09 5:47p mward
* PR47739: Support 4501 and 4506 tuner cards.
* 
* 13   2/17/09 5:22p mward
* PR52200: PCM playback for 7400.
* 
* 12   2/10/09 5:42p mward
* PR52005: Enable mosaic with NEXUS_NUM_MOSAIC_DECODES 12.
* 
* 11   11/25/08 5:07p mward
* PR47739: NEXUS_NUM_AUDIO_DACS 3 duplicates dac1 on display1 and
*  display2 as in native Settop API.
* 
* 10   11/25/08 11:28a mward
* PR47739: NEXUS_NUM_COMPOSITE_OUTPUTS 2
* 
* 9   11/10/08 9:59a mward
* PR47739: Consolidate nexus_platform_7400.h here, as done for other
*  platforms.
* 
* 8   10/31/08 10:22a mward
* PR47739: Audio Features.
* 
* 7   9/2/08 7:48p katrep
* PR46457: Configure nexus features at the run time based on board strap
*  options
* 
* 6   7/18/08 10:50a erickson
* PR44919: add video decoder heap size
* 
* 5   5/19/08 1:06p vishk
* PR 40034: Develop Nexus Smartcard Interface/Module.
* PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
* 
* 4   3/27/08 2:37p jrubio
* PR40919: add UHF define
* 
* 3   2/21/08 9:27a erickson
* PR39405: added RFM
* 
* 2   2/1/08 5:25p jgarrett
* PR 39017: Adding HdmiOutput
* 
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/6   12/6/07 2:17p jgarrett
* PR 37971: Cleaning up audio definitions
* 
* Nexus_Devel/5   11/26/07 9:46a erickson
* PR37423: added GPIO #defines
* 
* Nexus_Devel/4   11/5/07 2:12p erickson
* PR36725: add NEXUS_PlatformConfiguration, share more code with 97401
* 
* Nexus_Devel/3   10/17/07 2:45p vsilyaev
* PR 34662: Added number of video windows
* 
* Nexus_Devel/2   9/28/07 4:25p jgarrett
* PR 35002: Adding new features
* 
* Nexus_Devel/1   9/24/07 3:43p jgarrett
* PR 35002: Adding platform_features
* 
***************************************************************************/
#ifndef NEXUS_PLATFORM_FEATURES_H__
#define NEXUS_PLATFORM_FEATURES_H__

#include "nexus_types.h" /* required for BCHP_VER */

/* Indicate the generation of DTV platform */
#if (BCHP_CHIP==35125) || (BCHP_CHIP==35230)
#define NEXUS_DTV_PLATFORM 3
#elif (BCHP_CHIP==35233)
#define NEXUS_DTV_PLATFORM 4
#endif


#if NEXUS_DTV_PLATFORM >= 3
/* Transport Features */
#define NEXUS_NUM_INPUT_BANDS 7
#define NEXUS_NUM_PARSER_BANDS 3 /*was 7 for 3548*/
#define NEXUS_NUM_PID_CHANNELS 128
#define NEXUS_NUM_TIMEBASES 2
#define NEXUS_NUM_STC_CHANNELS 1
#define NEXUS_NUM_MESSAGE_FILTERS 64
#define NEXUS_NUM_RECPUMPS 2
#if (BCHP_CHIP==35125)
#define NEXUS_NUM_PLAYPUMPS 2
#else
#define NEXUS_NUM_PLAYPUMPS 5
#endif
#define NEXUS_NUM_REMUX 1
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24

/* Video Features -- per decoder */
#define NEXUS_NUM_VIDEO_DECODERS 1
#define NEXUS_NUM_MOSAIC_DECODES 0  /* Will enable this at a later point */
#define NEXUS_NUM_STILL_DECODES 1
#define NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE ((3625984)+518656)
#define NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE ((5210112)+307200) * (NEXUS_NUM_VIDEO_DECODERS + NEXUS_NUM_STILL_DECODES)
#define NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE ((3440640*7)+3440640)

/* Audio Features */
#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125) || (BCHP_CHIP == 35233)
#define NEXUS_NUM_AUDIO_RETURN_CHANNEL 1
#endif
#define NEXUS_AUDIO_BUILD_35230_TO_DO
#define NEXUS_NUM_AUDIO_DECODERS 3
#define NEXUS_NUM_AUDIO_DACS 3
#define NEXUS_NUM_AUDIO_MIXERS 8
#define NEXUS_NUM_I2S_OUTPUTS 5
#define NEXUS_NUM_I2S_INPUTS 1
#define NEXUS_NUM_SPDIF_INPUTS 2
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_RF_AUDIO_DECODERS 1
#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 2
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#define NEXUS_NUM_ANALOG_AUDIO_DECODERS 1
#define NEXUS_NUM_ANALOG_AUDIO_INPUTS 8  /* 1-7: for ADC input channel 1-7; 0: mute */
#define NEXUS_NUM_AUDIO_CAPTURES 1
#define NEXUS_AUDIO_CAPTURE_CHANNEL_POST_PROCESSING_ENABLED 1

/* Display Features */
#define NEXUS_NUM_656_INPUTS 1
#if 0
#define NEXUS_NUM_656_OUTPUTS 0
#define NEXUS_NUM_HDMI_OUTPUTS 0
#endif/*0*/
#define NEXUS_NUM_HDMI_INPUTS 1
#if 0
#define NEXUS_NUM_DNR 2
#define NEXUS_NUM_DCR 2
#define NEXUS_NUM_LAB 2
#define NEXUS_NUM_CAB 2
#define NEXUS_NUM_PEP 1

#define NEXUS_NUM_VIDEO_DACS 3
#endif/*#if 0*/
/* Not all 35230 chip revisions have hardware cursor support.
 * The implementation file, nexus_display_cursor.c will check the chip revision
 * whether to support it or not. */
#define NEXUS_NUM_HARDWARE_CURSORS 1

/* One Scart input for 935230 */
#define NEXUS_NUM_SCART_INPUTS 1

/* ADC Features */
#define NEXUS_ANALOG_VIDEO_INPUT_ADC_MAX 4

/* display 0 is LVDS, display 1 is SD */
#define NEXUS_NUM_DISPLAYS 2
#define NEXUS_NUM_PANEL_OUTPUTS 1
#define NEXUS_NUM_COMPOSITE_OUTPUTS 1

/* maximum number of windows in each display */
#define NEXUS_NUM_VIDEO_WINDOWS   2

/* Video inputs */
#define NEXUS_NUM_COMPONENT_INPUTS 3
#if (BCHP_CHIP==35233)
/* 35233 does not have and svideo inputs */
#else
#define NEXUS_NUM_SVIDEO_INPUTS 1
#endif
#define NEXUS_NUM_COMPOSITE_INPUTS 4
#define NEXUS_NUM_PC_INPUTS 1
#if (BCHP_CHIP == 35125)
#define NEXUS_NUM_ANALOG_VIDEO_DECODERS 1
#else
#define NEXUS_NUM_ANALOG_VIDEO_DECODERS 2
#endif

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1

#if NEXUS_HAS_FRONTEND
/* Maximum number of frontends - one internal */
#define NEXUS_MAX_FRONTENDS 1
#define NEXUS_FRONTEND_SCAN_SUPPORTED 1
#endif


/* external interrupts */
#define NEXUS_EXTIRQ_MAX 7

#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35233)
#define NEXUS_NUM_GPIO_PINS 143
#define NEXUS_NUM_SGPIO_PINS 0
#define NEXUS_NUM_TGPIO_PINS 45
#elif (BCHP_CHIP == 35125)
#define NEXUS_NUM_GPIO_PINS 51
#define NEXUS_NUM_SGPIO_PINS 0
#define NEXUS_NUM_TGPIO_PINS 47
#else
#error "GPIO porting is required"
#endif


/* DVB-CI Details */
#define NEXUS_DVB_CI_CHIP_SELECT 2
#define NEXUS_DVB_CI_MEMORY_LENGTH (1024*1024)
#define NEXUS_DVB_CI_IO_MEMORY_LENGTH  (1024*1024)
#define NEXUS_DVB_CI_MEMORY_BASE       (0x12e00000)
#define NEXUS_DVB_CI_IO_MEMORY_BASE    (NEXUS_DVB_CI_MEMORY_BASE + NEXUS_DVB_CI_MEMORY_LENGTH)

/* I2C Features */
#define NEXUS_NUM_I2C_CHANNELS 3

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 3

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 1

/* PWM Channels */
#define NEXUS_NUM_PWM_CHANNELS 10

#if 0
/* UARTS */
#define NEXUS_NUM_UARTS 3

/* Smartcard */
#define NEXUS_NUM_SMARTCARD_CHANNELS 1
#endif/*0 [JEH]*/

/* Still picture decoders */
#define NEXUS_NUM_PICTURE_DECODERS 1

/* Enable GFD support for vertical upscaling */
#define NEXUS_HAS_GFD_VERTICAL_UPSCALE 1

#define NEXUS_NUM_MEMC 1

#else /* NEXUS_DTV_PLATFORM */

/* Transport Features */
#define NEXUS_NUM_RAVE_CHANNELS 1
#define NEXUS_NUM_RAVE_CONTEXTS 24
#define NEXUS_NUM_REMUX 1

#define NEXUS_NUM_INPUT_BANDS 6
#define NEXUS_NUM_PARSER_BANDS 6
#define NEXUS_NUM_PID_CHANNELS 256
#define NEXUS_NUM_TIMEBASES 4
#define NEXUS_NUM_STC_CHANNELS 16
#define NEXUS_NUM_MESSAGE_FILTERS 64
#define NEXUS_NUM_RECPUMPS NEXUS_NUM_RAVE_CONTEXTS
#define NEXUS_NUM_PLAYPUMPS 5

/* Audio Features */
#define NEXUS_NUM_AUDIO_DECODERS 3
#define NEXUS_NUM_AUDIO_DACS 3      /* This is a lie, there are 2, but it allows one DAC per display, 
                                       dac0 for display[0], dac1 for display[1] and display[2], as
                                       per native Settop API.
                                       Settop won't use DAC on both display[1] and display[2], you 
                                       shouldn't either. */
#define NEXUS_NUM_SPDIF_OUTPUTS 1
#define NEXUS_NUM_AUDIO_MIXERS 9
#define NEXUS_NUM_I2S_OUTPUTS 3

#define NEXUS_NUM_AUDIO_CAPTURE_CHANNELS 0
#define NEXUS_NUM_AUDIO_CAPTURES 1
#define NEXUS_NUM_AUDIO_PLAYBACKS 2
#define NEXUS_HAS_AUDIO_TSM_LOG_SUPPORT 1

/* Video Features */
#define NEXUS_NUM_VIDEO_DECODERS 2
/* Video Features -- per decoder */
#if NEXUS_VIDEO_DECODER_SINGLEDECODE
#define NEXUS_NUM_MOSAIC_DECODES 0
#else
#define NEXUS_NUM_MOSAIC_DECODES 12
#endif
#define NEXUS_NUM_STILL_DECODES 1 
#define NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE 17406362
#define NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE 0
#define NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE 31813796


/* Display Features */
/* TODO: Add VDC memory allocations */
#define NEXUS_NUM_DNR 2
#define NEXUS_NUM_DCR 2
#define NEXUS_NUM_LAB 2
#define NEXUS_NUM_CAB 2
#define NEXUS_NUM_PEP 1
#define NEXUS_NUM_VIDEO_DACS 7

#define NEXUS_NUM_656_INPUTS 0
#define NEXUS_NUM_656_OUTPUTS 1
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_CEC 1
#define NEXUS_NUM_DISPLAYS 3
#define NEXUS_NUM_VIDEO_WINDOWS   2

#define NEXUS_DISPLAY_NUM_SD_BUFFERS         10
#define NEXUS_DISPLAY_NUM_HD_BUFFERS         0
#define NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS    6      /* allocate full HD bufffers instead of HD buffers, required for 1080p o/p */  

/* Video outputs */
#define NEXUS_NUM_COMPOSITE_OUTPUTS 2
#define NEXUS_NUM_COMPONENT_OUTPUTS 1
#define NEXUS_NUM_SVIDEO_OUTPUTS 1
#define NEXUS_NUM_HDMI_OUTPUTS 1
#define NEXUS_NUM_RFM_OUTPUTS 1

/* Graphics Features */
#define NEXUS_NUM_2D_ENGINES 1
#define NEXUS_NUM_3D_ENGINES 1

/* I2C Channels */
#define NEXUS_NUM_I2C_CHANNELS 5

/* Interrupts */
#define NEXUS_NUM_L1_REGISTERS 2

/* DMA Channels */
#define NEXUS_NUM_DMA_CHANNELS 2

/* UARTS */
#define NEXUS_NUM_UARTS 3

/* The maximum number of frontends is usually equal to the number of slots on the reference board. */
#define NEXUS_MAX_FRONTENDS 7
#define NEXUS_NUM_FRONTEND_CARD_SLOTS 3

#define NEXUS_NUM_SGPIO_PINS 10
#define NEXUS_NUM_GPIO_PINS 113

/* UHF INPUTS */
#define NEXUS_NUM_UHF_INPUTS 2

/* SMARTCARD CHANNELS */
#define NEXUS_NUM_SMARTCARD_CHANNELS 3

#define NEXUS_NUM_MEMC 3

#endif



#endif /* #ifndef NEXUS_PLATFORM_FEATURES_H__ */

