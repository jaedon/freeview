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
 * $brcm_Revision: 15 $
 * $brcm_Date: 7/30/12 4:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/src/rap_v1/nexus_audio_module.h $
 * 
 * 15   7/30/12 4:35p jgarrett
 * SW7435-266: Switching to unified audio module structure
 * 
 * 14   7/27/12 3:21p jgarrett
 * SW7425-3443: Removing DTV-only interfaces
 * 
 * 13   6/21/12 6:29p mward
 * SW7425-2741: Restore NEXUS_AudioDecoder to object class declarations.
 * 
 * 12   6/20/12 1:15p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 11   6/14/12 5:50p mward
 * SW7425-2741: add AudioDecoderPrimer
 * 
 * 10   7/26/11 12:14p mward
 * SW7125-1014 : Update audio standby function
 * 
 * 9   2/28/11 1:58p mward
 * SW7408-210 : Fix kernel mode build
 * 
 * 8   5/26/10 11:47a mward
 * SW7335-718: Stubbing TruSurround on 7400.
 * 
 * 7   7/2/09 12:47p mward
 * PR 55459: Stub ramp step APIs, not available in 7400 RAP.
 * 
 * 6   4/9/09 11:51a mward
 * PR 52340: Adding TruVolume for STB platforms also adding Dolby Volume.
 * 
 * 5   3/13/09 12:20p mward
 * PR 51468: Renaming globals to match naming conventions
 * 
 * 4   2/17/09 5:22p mward
 * PR52200: NEXUS_AudioModule_P_SampleRate2Avc() needed for PCM playback.
 * 
 * 3   7/11/08 10:30a jgarrett
 * PR 42924: PR 44645: Adding capture and decoder_extra
 * 
 * 2   3/11/08 5:25p jgarrett
 * PR 39983: Adding playback stubs
 * 
 * 1   1/18/08 2:18p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/6   12/6/07 2:17p jgarrett
 * PR 37971: Adding i2s_output
 * 
 * Nexus_Devel/5   12/4/07 2:12p vsilyaev
 * PR 36404: Use structure to conrol audio trickmodes
 * 
 * Nexus_Devel/4   11/27/07 5:25p jgarrett
 * PR 37471: Adding I2sInput and SpdifInput
 * 
 * Nexus_Devel/3   10/24/07 3:00p vsilyaev
 * PR 36404: Trick mode extension
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

#include "nexus_audio_thunks.h"
#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_base.h"
#include "nexus_audio_init.h"
#include "priv/nexus_audio_standby_priv.h"
#include "nexus_audio_priv.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_audio_dac.h"
#include "nexus_spdif_output.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_extra.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_decoder_primer.h"
#include "nexus_audio_capture.h"
#include "nexus_i2s_input.h"
#include "nexus_i2s_output.h"
#include "brap.h"
#include "brap_img.h"
#include "priv/nexus_core.h"
#include "nexus_audio.h"
#include "nexus_tru_volume.h"
#include "nexus_dolby_volume.h"
#include "nexus_echo_canceller.h"
#include "nexus_audio_equalizer.h"
#include "nexus_audio_return_channel.h"
#include "nexus_audio_dsp.h"
#include "nexus_spdif_input.h"
#include "nexus_audio_input_capture.h"
#include "nexus_audio_dummy_output.h"
#include "nexus_audio_mux_output.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME audio
#define NEXUS_MODULE_SELF g_NEXUS_audioModule

/***************************************************************************
 * Module Instance Data
 ***************************************************************************/
typedef struct NEXUS_AudioModuleData
{
    BRAP_Handle hRap;
    NEXUS_ModuleHandle transport;
    NEXUS_AudioModuleSettings moduleSettings;
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

