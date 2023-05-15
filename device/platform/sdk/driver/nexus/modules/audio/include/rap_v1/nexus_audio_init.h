/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_audio_init.h $
 * $brcm_Revision: 15 $
 * $brcm_Date: 5/6/11 5:43p $
 *
 * Module Description:
 *      Audio Initialization Routines
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio_init.h $
 * 
 * 15   5/6/11 5:43p jgarrett
 * SW7420-1856: Making post mixing buffers adjustable for MS11
 * 
 * 14   7/19/10 4:17p jgarrett
 * SW7550-496: Adding surface module handle to audio initialization for
 *  ZSP graphics.
 * 
 * SW7550-496/1   7/16/10 5:43p nitinb
 * SW7550-496: Added suface module handle to NEXUS_AudioModuleSettings
 * 
 * 13   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 *
 * 12   5/14/10 2:41p jgarrett
 * SW7335-709: Adding option to set PLL before audio module initializes
 * 
 * 11   8/28/09 4:00p jgarrett
 * SW7405-2928: Adding multichannel capture support
 * 
 * 10   7/14/09 1:46p jgarrett
 * PR 54367: adding capture buffer size option
 * 
 * 9   4/6/09 5:25p jgarrett
 * PR 53393: Adding memory configuration options
 * 
 * 8   4/2/09 2:24p jgarrett
 * PR 52729: Adding HBR option for SPDIF and allowing the platform to
 *  configure whether HBR is enabled
 * 
 * 7   11/12/08 3:48p jgarrett
 * PR 48920: Adding a global independent delay flag that affects all
 *  channels
 * 
 * 6   9/25/08 5:07p jgarrett
 * PR 46055: Refactoring equalizer API
 * 
 * 5   8/8/08 6:59p jgarrett
 * PR 45171: Adding Frontend handle for AASD
 * 
 * 4   7/23/08 3:01p jgarrett
 * PR 40367: Removing runtime codec booleans
 * 
 * 3   2/20/08 1:36p erickson
 * PR39405: added RFM output
 * 
 * 2   2/5/08 1:49p jgarrett
 * PR 39017: Adding HdmiOutput
 * 
 * 1   1/18/08 2:18p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/5   1/17/08 12:23p jgarrett
 * PR 38753: Adding HDMI Input handle
 * 
 * Nexus_Devel/4   1/8/08 7:01p jgarrett
 * PR 38535: Adding equalizer settings, touching up post-processing names
 * 
 * Nexus_Devel/3   11/29/07 2:24p erickson
 * PR35457: doc update for 0.5 release
 * 
 * Nexus_Devel/2   11/9/07 9:58a jgarrett
 * PR 36774: Adding encode and post-processing options to init
 * 
 * Nexus_Devel/1   9/27/07 7:52p jgarrett
 * PR 34954: Adding audio module init
 * 
 **************************************************************************/
#ifndef NEXUS_AUDIO_INIT_H__
#define NEXUS_AUDIO_INIT_H__

#include "nexus_types.h"
#include "nexus_audio_output.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_AudioModuleSettings
{
    struct 
    {
        NEXUS_ModuleHandle transport;
        NEXUS_ModuleHandle hdmiInput;   /* Only required for platforms that support HDMI input */
        NEXUS_ModuleHandle hdmiOutput;  /* Only required for platforms that support HDMI output */
        NEXUS_ModuleHandle rfm;         /* Only required for platforms that support RFM */
        NEXUS_ModuleHandle frontend;    /* Only required for platforms that support RfAudioDecoder */
        NEXUS_ModuleHandle surface; 
    } modules;

    bool watchdogEnabled;
    bool independentDelay;              /* If true, independent delay is enabled on all channels */
    bool hbrEnabled;                    /* If true, high bitrate audio passthrough is enabled for direct AC3+ passthrough.  
                                           This does increase memory requirements. */
    int maximumProcessingBranches;      /* Set this to the maximum number of active post processing branches active on a single channel.  Default=0. */
    unsigned audioCaptureFifoSize;      /* Audio capture fifo size in bytes (per-channel).  On some systems, this must be set only at initialization time. */
    unsigned audioCaptureChannels;      /* Default = 2 for stereo.  To capture 5.1 content use 6. */

    unsigned numPostMixingInputBuffers; /* Number of input buffers required by processing done post-mixing.  Two buffers are required per PCM channel-pair for each 
                                           FMM stream, And one buffer is required for a compressed channel.  Input can be either playback or hardware mixed data. */
    unsigned numPostMixingOutputBuffers;/* Number of output buffers required between DSP processing done after mixing and the FMM hardware.  One buffer is required 
                                           per channel of output from any processing done after mixing.  For Stereo, use 2 buffers.  For Multichannel 5.1 use 6 buffers.
                                           Compressed data requires one bufffer. */

    NEXUS_AudioOutputPll defaultPll;    /* Default PLL for outputs.  Default=0.  */
} NEXUS_AudioModuleSettings;

void NEXUS_AudioModule_GetDefaultSettings(
    NEXUS_AudioModuleSettings *pSettings    /* [out] */
    );

NEXUS_ModuleHandle NEXUS_AudioModule_Init(
    const NEXUS_AudioModuleSettings *pSettings  /* NULL will use default settings */
    );

void NEXUS_AudioModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_INIT_H__ */

