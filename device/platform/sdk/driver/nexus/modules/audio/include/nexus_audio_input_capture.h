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
* $brcm_Workfile: nexus_audio_input_capture.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/6/12 2:25p $
*
* API Description:
*   API name: AudioInputCapture
*    Object used to capture data from an external input into the audio subsystem
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_audio_input_capture.h $
* 
* 3   9/6/12 2:25p jgarrett
* SW7425-3667: Adding cplusplus wrapper
* 
* 2   2/27/12 6:09p jgarrett
* SW7425-2074: Adding null_allowed attribute
* 
* 1   2/9/12 2:24p jgarrett
* SW7425-2074: Merge to main branch
* 
* SW7425-2074/3   2/9/12 1:25p jgarrett
* SW7425-2074: Implementing code review feedback
* 
* SW7425-2074/2   1/18/12 6:06p jgarrett
* SW7425-2074: Initial version of InputCapture API
* 
* SW7425-2074/1   1/18/12 10:21a jgarrett
* SW7425-2156: Adding power management support for DSP video decoder
* 
***************************************************************************/
#ifndef NEXUS_AUDIO_INPUT_CAPTURE_H_
#define NEXUS_AUDIO_INPUT_CAPTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AudioInputCapture

Header file: nexus_audio_input_capture.h

Module: Audio

Description: Capture data from an external input into the audio subsystem

**************************************/

/***************************************************************************
Summary:
Handle for audio input capture
***************************************************************************/
typedef struct NEXUS_AudioInputCapture *NEXUS_AudioInputCaptureHandle;

/***************************************************************************
Summary:
Open-time settings for AudioInputCapture
***************************************************************************/
typedef struct NEXUS_AudioInputCaptureOpenSettings
{
    int tbd;    /* Currently a placeholder */
} NEXUS_AudioInputCaptureOpenSettings;

/***************************************************************************
Summary:
Get default Open-time settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetDefaultOpenSettings(
    NEXUS_AudioInputCaptureOpenSettings *pSettings  /* [out] */
    );

/***************************************************************************
Summary:
Open an AudioInputCapture handle
***************************************************************************/
NEXUS_AudioInputCaptureHandle NEXUS_AudioInputCapture_Open(
    unsigned index,
    const NEXUS_AudioInputCaptureOpenSettings *pSettings /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an AudioInputCapture handle
***************************************************************************/
void NEXUS_AudioInputCapture_Close(
    NEXUS_AudioInputCaptureHandle handle
    );

/***************************************************************************
Summary:
Start-time settings for AudioInputCapture
***************************************************************************/
typedef struct NEXUS_AudioInputCaptureStartSettings
{
    NEXUS_AudioInput input;
} NEXUS_AudioInputCaptureStartSettings;

/***************************************************************************
Summary:
Get Default Start-time settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetDefaultStartSettings(
    NEXUS_AudioInputCaptureStartSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Start capturing input data
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_Start(
    NEXUS_AudioInputCaptureHandle handle,
    const NEXUS_AudioInputCaptureStartSettings *pSettings
    );

/***************************************************************************
Summary:
Stop capturing input data
***************************************************************************/
void NEXUS_AudioInputCapture_Stop(
    NEXUS_AudioInputCaptureHandle handle
    );

/***************************************************************************
Summary:
Settings for AudioInputCapture
***************************************************************************/
typedef struct NEXUS_AudioInputCaptureSettings
{
    NEXUS_CallbackDesc sourceChanged;       /* This callback fires whenever specifics such as sample rate, layer, or bitrate change.
                                               It also fires when capturing HDMI/SPDIF input if the input data format changes.  */

    int32_t volumeMatrix[NEXUS_AudioChannel_eMax][NEXUS_AudioChannel_eMax]; /* Volume matrix.  This allows customization of channel volume
                                                                               output.  Default is [Left][Left] = Normal, [Right][Right] = Normal,
                                                                               [Center][Center] = Normal, etc.  All other volumes are zero by default.
                                                                               This will affect stereo and multichannel PCM outputs only,  not compressed. */
    bool muted;     /* This will affect stereo and multichannel PCM outputs only, not compressed. */
} NEXUS_AudioInputCaptureSettings;

/***************************************************************************
Summary:
Get current settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetSettings(
    NEXUS_AudioInputCaptureHandle handle,
    NEXUS_AudioInputCaptureSettings *pSettings  /* [out] */
    );

/***************************************************************************
Summary:
Set settings for AudioInputCapture
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_SetSettings(
    NEXUS_AudioInputCaptureHandle handle,
    const NEXUS_AudioInputCaptureSettings *pSettings
    );

/***************************************************************************
Summary:
Status for AudioInputCapture
***************************************************************************/
typedef struct NEXUS_AudioInputCaptureStatus
{
    bool started;               /* True if capture has been started */
    bool inputSignalPresent;    /* True if the input signal is present (applicable to HDMI/SPDIF inputs only) */
    bool inputSignalValid;      /* True if the input signal is valid and can be captured (applicable to HDMI/SPDIF inputs only) */

    unsigned sampleRate;        /* In Hz */
    NEXUS_AudioCodec codec;     /* Audio stream format (applicable to HDMI/SPDIF inputs only) */
    unsigned numPcmChannels;    /* Number of PCM channels in the stream (applicable to HDMI input only) */
} NEXUS_AudioInputCaptureStatus;

/***************************************************************************
Summary:
Get current status for AudioInputCapture
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_GetStatus(
    NEXUS_AudioInputCaptureHandle handle,
    NEXUS_AudioInputCaptureStatus *pStatus  /* [out] */
    );

/***************************************************************************
Summary:
Get audio connector to connect InputCapture to other objects
***************************************************************************/
NEXUS_AudioInput NEXUS_AudioInputCapture_GetConnector(
    NEXUS_AudioInputCaptureHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_INPUT_CAPTURE_H_ */
