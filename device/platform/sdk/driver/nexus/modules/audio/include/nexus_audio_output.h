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
* $brcm_Workfile: nexus_audio_output.h $
* $brcm_Revision: 11 $
* $brcm_Date: 5/23/12 6:13p $
*
* API Description:
*   API name: AudioOutput
*    Generic APIs for audio outputs from an audio mixer.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio_output.h $
* 
* 11   5/23/12 6:13p jgarrett
* SW7425-3070: Adding NEXUS_AudioOutputPll Settings to control VCXO
*  source
* 
* 10   7/7/11 5:29p jgarrett
* SW7552-72: Adding nco option
* 
* 9   2/19/09 10:36a jgarrett
* PR 51964: Adding options for default sampling rate and PLL
* 
* 8   9/25/08 5:07p jgarrett
* PR 46055: Revising equalizer API
* 
* 7   5/28/08 7:03p jgarrett
* PR 42027: Adding custom equalizer mode
* 
* 6   3/11/08 3:08p erickson
* PR40222: added independent audio output delay for both the user and
*  SyncChannel
* 
* 5   2/7/08 11:57a vsilyaev
* PR 38682: Changed return type of XXX_Shutdown routines to void
* 
* 4   1/31/08 9:47a erickson
* PR36808: remove unnecessary includes
* 
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
* 
* 2   1/23/08 2:39p erickson
* PR35457: update docs
* 
* 1   1/18/08 2:15p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/12   1/9/08 2:52p erickson
* PR38535: added comments
* 
* Nexus_Devel/11   1/8/08 7:00p jgarrett
* PR 38535: Changing volume control, adding equalizer and channel order
* control
* 
* Nexus_Devel/10   12/28/07 2:11p erickson
* PR35457: added units
* 
* Nexus_Devel/9   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
* Nexus_Devel/8   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
*
* Nexus_Devel/7   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/6   10/4/07 2:25p erickson
* PR34594: removed audio descriptors
*
* Nexus_Devel/5   9/27/07 7:51p jgarrett
* PR 34954: Adding return type for close
*
* Nexus_Devel/4   9/24/07 5:04p jgarrett
* PR 34954: Adding dummy structure member
*
* Nexus_Devel/3   9/17/07 1:32p jgarrett
* PR 34954: Implementing naming convention changes
*
* Nexus_Devel/2   9/10/07 11:30a jgarrett
* PR 34254: Adding multiple inclusion protection
*
* Nexus_Devel/2   8/24/07 11:57a jgarrett
* PR 34254: Implementing Source -> Input name changes
*
***************************************************************************/
#ifndef NEXUS_AUDIO_OUTPUT_H__
#define NEXUS_AUDIO_OUTPUT_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AudioOutput

Header file: nexus_audio_output.h

Module: Audio

Description: Abstract connector for routing audio to an output

**************************************/

/***************************************************************************
Summary:
Generic audio output settings
***************************************************************************/
typedef struct NEXUS_AudioOutputSettings
{
    NEXUS_Timebase timebase;

	NEXUS_AudioChannelMode channelMode;

    NEXUS_AudioVolumeType volumeType; /* specifies the units for leftVolume and rightVolume. */
    int32_t leftVolume; /* the units depend on the value of volumeType. See docs for NEXUS_AudioVolumeType. */
    int32_t rightVolume; /* the units depend on the value of volumeType. See docs for NEXUS_AudioVolumeType. */

    bool muted;
    
    unsigned additionalDelay; /* Delay to add to this path. If you are using SyncChannel, this additionalDelay will 
    							 be added to whatever delay SyncChannel assigns. In milliseconds. */

    NEXUS_AudioOutputPll pll; /* PLL Selection.  May not apply to all outputs, see NEXUS_AudioOutputPll for details. */

    NEXUS_AudioOutputNco nco; /* NCO Selection.  May not apply to all outputs, see NEXUS_AudioOutputNco for details. */

    unsigned defaultSampleRate; /* This is the default sample rate for an output in Hz (default=48000). 
                                   Outputs connected to an AudioDecoder will automatically switch to the 
                                   decoded sample rate.  Outputs connected to other inputs such as AudioPlayback
                                   or I2sInput will sample rate convert the input data to this rate.  Adjusting
                                   this rate to match the source rate can avoid a sample rate conversion. 
                                   Supported values are 48000, 44100, and 32000. */
} NEXUS_AudioOutputSettings;

/***************************************************************************
Summary:
Get settings of an audio output
***************************************************************************/
void NEXUS_AudioOutput_GetSettings(
    NEXUS_AudioOutput output,
    NEXUS_AudioOutputSettings *pSettings    /* [out] Current Settings */
    );

/***************************************************************************
Summary:
Set settings of an audio output
***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_SetSettings(
    NEXUS_AudioOutput output,
    const NEXUS_AudioOutputSettings *pSettings
    );

/***************************************************************************
Summary:
Status of the audio output
***************************************************************************/
typedef struct NEXUS_AudioOutputStatus 
{
    unsigned delay; /* Delay on this path, in milliseconds. This reports the total delay including SyncChannel's delay and NEXUS_AudioOutputSettings.additionalDelay. */
} NEXUS_AudioOutputStatus;

/***************************************************************************
Summary:
Get status of an audio output
***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_GetStatus(
    NEXUS_AudioOutput output,
    NEXUS_AudioOutputStatus *pStatus    /* [out] */
    );

/***************************************************************************
Summary:
Add an input to this output
***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_AddInput(
    NEXUS_AudioOutput output,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Add an input to this output
***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_RemoveInput(
    NEXUS_AudioOutput output,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Add an input to this output
***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_RemoveAllInputs(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
Shutdown this output handle

Description:
    This routine should be called before the specific output object
    (e.g. AudioDac) is closed.
***************************************************************************/
void NEXUS_AudioOutput_Shutdown(
    NEXUS_AudioOutput output
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_OUTPUT_H__ */

