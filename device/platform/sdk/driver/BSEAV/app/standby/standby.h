/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: standby.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/27/12 3:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/standby/standby.h $
 * 
 * 1   9/27/12 3:12p gmohile
 * SW7435-327 : Add gpio wakeup
 *
 *****************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_ir_input.h"
#if NEXUS_HAS_UHF_INPUT
#include "nexus_uhf_input.h"
#endif
#if NEXUS_HAS_KEYPAD
#include "nexus_keypad.h"
#endif
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#endif
#include "nexus_parser_band.h"
#include "nexus_transport_wakeup.h"



#define POWER_STANDBY_IR_CODE 0x600A
#define EXIT_IR_CODE 0xD012
#define S1_IR_CODE 0xF001
#define S2_IR_CODE 0xE002
#define S3_IR_CODE 0xD003

#define POWER_STANDBY_UHF_CODE 0x00220373

#if (BCHP_CHIP == 7231)  
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7425 || BCHP_CHIP == 7435)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#elif (BCHP_CHIP == 7420)
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#elif (BCHP_CHIP == 7344 || BCHP_CHIP == 7346) 
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7358) 
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 0
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7552) 
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 0
#define MOCA_SUPPORT 0
#define MEMC1_SUPPORT 0
#elif (BCHP_CHIP == 7429)
#define S3_STANDBY_SUPPORT 1
#define S3_WARM_BOOT 1
#define MOCA_SUPPORT 1
#define MEMC1_SUPPORT 1
#endif

#define SD_DISPLAY 1


typedef struct B_StandbyNexusHandles {
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display0, display1;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_SurfaceHandle framebufferHD, framebufferSD, offscreenHD, offscreenSD;    
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_VideoWindowHandle window0, window1;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_IrInputHandle irHandle;
#if NEXUS_HAS_UHF_INPUT
    NEXUS_UhfInputHandle uhfHandle;
#endif
#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadHandle keypadHandle;
#endif
    NEXUS_FrontendHandle frontend;
#if NEXUS_HAS_CEC
    NEXUS_CecHandle hCec;
#endif
    NEXUS_ParserBand parserBand;
    BKNI_EventHandle event, checkpointEvent, spaceAvailableEvent;
} B_StandbyNexusHandles;


typedef struct B_DeviceState {
    bool playback_started;
    bool decode_started;
    bool graphics_started;
    bool graphics_enabled;
    bool stop_graphics;
    bool qam, sat;
    bool s1_only, s2_only, s3_only, exit_app;
    bool cli;
    bool cecDeviceReady;
} B_DeviceState;


/* this is a simple wrapper around pmlib to make the sample app simpler */
typedef struct pmlib_state_t
{
    bool usb;
    bool enet;
    bool moca;
    bool sata;
    bool tp1;
    bool memc1;
    bool cpu;
    bool ddr;
    int flags;
} pmlib_state_t;


NEXUS_TransportWakeupFilter Filter[18] =
{
    { 0x47, 0xFF, 1 },
    { 0x12, 0xFF, 1 },
    { 0x34, 0xFF, 1 },
    { 0x15, 0xFF, 1 },

    { 0x12, 0xFF, 2 },
    { 0x34, 0xFF, 2 },
    { 0x03, 0xFF, 2 },
    { 0x46, 0xFF, 2 },
    { 0x66, 0xFF, 2 },

    { 0x4F, 0xFF, 3 },
    { 0x31, 0xFF, 3 },
    { 0x00, 0xFF, 3 },

    { 0x88, 0xFF, 2 },
    { 0x77, 0xFF, 2 },
    { 0x66, 0xFF, 2 },
    { 0x55, 0xFF, 2 },
    { 0x44, 0xFF, 2 },
    { 0x33, 0xFF, 2 }
};
