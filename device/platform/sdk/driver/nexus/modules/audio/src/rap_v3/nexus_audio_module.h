/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_audio_module.h $
 * $brcm_Revision: 32 $
 * $brcm_Date: 7/30/12 4:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/src/rap_v3/nexus_audio_module.h $
 * 
 * 32   7/30/12 4:35p jgarrett
 * SW7435-266: Switching to unified audio module structure
 * 
 * 31   7/27/12 3:21p jgarrett
 * SW7425-3443: Removing DTV-only interfaces
 * 
 * 30   6/19/12 5:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 29   6/14/12 3:17p erickson
 * SW7425-2741: add AudioDecoderPrimer
 * 
 * 28   7/21/11 5:12p erickson
 * SW7125-1014: Rework power management
 * 
 * 27   2/16/11 10:13a gmohile
 * SW7408-210 : Fix kernel mode build
 * 
 * 26   11/22/10 1:35p jhaberf
 * SW35125-28: Added 35125 support for analog audio
 * 
 * 25   11/17/10 3:36p jgarrett
 * SW7408-137: Adding buffer capture extension
 * 
 * 24   9/8/10 12:05p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/1   3/8/10 12:49p vsilyaev
 * SW7468-129: Added private interface between audio and video decoders
 * 
 * 23   7/19/10 4:17p jgarrett
 * SW7550-496: Adding surface module handle to audio initialization for
 *  ZSP graphics.
 * 
 * SW7550-496/2   7/16/10 5:46p nitinb
 * SW7550-496: Added surface module handle to NEXUS_AudioModuleData
 * 
 * SW7550-496/1   7/9/10 6:39p nitinb
 * SW7550-496: Support ZSP graphics extension
 * 
 * 22   6/24/10 4:18p akam
 * SW35230-120: Unlink 35230 RF decoder code from 3549. Move code from
 *  SW35230-120 branch.
 * 
 * 21   5/18/10 6:45p jgarrett
 * SW7335-718: Adding TruSurround to STB chips
 * 
 * 20   5/5/10 3:27p jgarrett
 * SW7405-3993: Adding AudysseyAdv support
 * 
 * 19   5/3/10 5:24p jgarrett
 * SW3548-2899: Adding Audyssey ABX support
 * 
 * 18   3/25/10 3:11p jgarrett
 * SW3548-2827: Adding Mono Downmix Support
 * 
 * 17   3/18/10 5:07p jgarrett
 * SW7405-4030: Adding 3D Surround
 * 
 * 16   6/16/09 12:13p jgarrett
 * PR 55459: Adding nexus_audio.h
 * 
 * 15   4/8/09 4:31p vsilyaev
 * PR 54016: Added support for precompiled header
 * 
 * 14   3/2/09 5:37p jgarrett
 * PR 51468: Renaming globals to match naming conventions
 * 
 * 13   2/12/09 5:36p jgarrett
 * PR 46135: Adding TruVolume
 * 
 * 12   8/21/08 4:20p erickson
 * PR45959: pull trick_ext into main module
 * 
 * 11   8/21/08 3:17p katrep
 * PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
 * 
 * 10   8/8/08 6:59p jgarrett
 * PR 45171: Adding frontend handle for AASD
 * 
 * 9   7/11/08 10:30a jgarrett
 * PR 44645: Adding capture API
 * 
 * 8   5/19/08 5:56p jgarrett
 * PR 42025: Merging to main branch
 * 
 * PR42025/2   5/15/08 8:00p zhiyipei
 * PR42025:Add extension volume API
 * 
 * PR42025/1   5/15/08 6:29p zhiyipei
 * PR42025:Add extension volume API
 * 
 * 7   5/13/08 10:43a jgarrett
 * PR40546: PR40546:Include files modification for compiling errors
 * 
 * 6   5/1/08 10:21a jgarrett
 * PR 42360: Adding analog audio features for 3548/3556
 * 
 * PR40546/1   5/13/08 6:10p zhiyipei
 * PR40546:Include files modification for compiling errors
 * 
 * 5   4/8/08 1:53p erickson
 * PR39453: add 3548 & 3556
 *
 * 4   3/11/08 4:38p jgarrett
 * PR 39983: Adding playback
 *
 * 3   2/5/08 1:49p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 2   1/31/08 9:52a erickson
 * PR36808: added SyncChannel interface
 *
 * 1   1/18/08 2:18p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/10   1/17/08 12:23p jgarrett
 * PR 38753: Adding HDMI input handle
 *
 * Nexus_Devel/9   1/9/08 5:56p jgarrett
 * PR 38535: Adding reverse sample rate conversion
 *
 * Nexus_Devel/8   1/9/08 1:56p jgarrett
 * PR 38535: Renaming bbe and xen
 *
 * Nexus_Devel/7   1/4/08 5:41p jgarrett
 * PR 38535: Adding new interfaces
 *
 * Nexus_Devel/6   12/20/07 2:30p jgarrett
 * PR 38019: Adding RfAudioDecoder
 *
 * Nexus_Devel/5   12/6/07 2:17p jgarrett
 * PR 37971: Adding I2sOutput
 *
 * Nexus_Devel/4   12/5/07 11:28a jgarrett
 * PR 37888: Adding new PVR hooks
 *
 * Nexus_Devel/3   11/27/07 5:26p jgarrett
 * PR 37471: Adding I2sInput and SpdifInput
 *
 * Nexus_Devel/2   11/9/07 9:58a jgarrett
 * PR 36774: Implementing 7405 audio
 *
 * Nexus_Devel/1   10/17/07 11:08a erickson
 * PR36152: add stub 7405
 *
 * Nexus_Devel/2   9/28/07 5:15p jgarrett
 * PR 34594: Adding sync thunks
 *
 * Nexus_Devel/1   9/27/07 7:52p jgarrett
 * PR 34954: Adding module.h file
 *
 **************************************************************************/
#ifndef NEXUS_AUDIO_MODULE_H__
#define NEXUS_AUDIO_MODULE_H__

#include "nexus_base.h"
#include "nexus_audio_thunks.h"
#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_init.h"
#include "priv/nexus_audio_standby_priv.h"
#include "nexus_audio_priv.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "priv/nexus_audio_input_priv.h"
#include "priv/nexus_audio_output_priv.h"
#include "priv/nexus_core_img.h"
#include "priv/nexus_core_img_id.h"
#include "nexus_audio_dac.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_extra.h"
#include "nexus_i2s_output.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_decoder_primer.h"
#include "brap.h"
#include "brap_img.h"
#include "priv/nexus_core.h"
#include "nexus_i2s_input.h"
#include "nexus_audio_capture.h"
#include "nexus_audio.h"
#include "nexus_echo_canceller.h"
#include "nexus_audio_equalizer.h"
#include "nexus_audio_return_channel.h"
#include "nexus_audio_dsp.h"
#include "nexus_spdif_input.h"
#include "nexus_audio_input_capture.h"
#include "nexus_audio_dummy_output.h"
#include "nexus_audio_mux_output.h"

#ifdef NEXUS_HAS_ZSP_GRAPHICS
#include "nexus_graphics2d.h"
#endif
#if NEXUS_AUDIO_BUFFER_CAPTURE_EXT
#include "nexus_audio_buffer_capture.h"
#include "nexus_audio_buffer_capture_priv.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME audio
#define NEXUS_MODULE_SELF g_NEXUS_audioModule

#if NEXUS_NUM_ZSP_VIDEO_DECODERS
#include "nexus_audio_module_video_decoder.h"
#endif

/***************************************************************************
 * Module Instance Data
 ***************************************************************************/
typedef struct NEXUS_AudioModuleData
{
    BRAP_Handle hRap;
    NEXUS_ModuleHandle transport;
    NEXUS_ModuleHandle hdmiInput;
    NEXUS_ModuleHandle hdmiOutput;
    NEXUS_ModuleHandle frontend;
    NEXUS_ModuleHandle surface;
    NEXUS_AudioModuleSettings moduleSettings;
    /* image interface */
    void * img_context;
    BIMG_Interface img_interface;
} NEXUS_AudioModuleData;

/* global module handle & data */
extern NEXUS_ModuleHandle g_NEXUS_audioModule;
extern NEXUS_AudioModuleData g_NEXUS_audioModuleData;

/* Utility functions */
BAVC_AudioSamplingRate NEXUS_AudioModule_P_SampleRate2Avc(unsigned sampleRate);
unsigned NEXUS_AudioModule_P_Avc2SampleRate(BAVC_AudioSamplingRate sampleRate);

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_3dSurround);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Ac3Encode);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioCapture);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDac);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoderPrimer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDummyOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioEncoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioEqualizer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioMuxOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioMixer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioPlayback);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AutoVolumeLevel);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DolbyVolume);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DolbyVolume258);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DtsEncode);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_I2sInput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_I2sMultiOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_I2sOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_RfAudioEncoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SpdifInput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SpdifOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_TruVolume);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_MODULE_H__ */

