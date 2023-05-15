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
* $brcm_Workfile: nexus_audio_decoder.h $
* $brcm_Revision: 69 $
* $brcm_Date: 10/9/12 12:13a $
*
* API Description:
*   API name: AudioDecoder
*    API for audio decoder management.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_audio_decoder.h $
* 
* 69   10/9/12 12:13a jgarrett
* SW7346-914: Adding audio ancillary data support
* 
* 68   8/21/12 5:22p erickson
* SW7231-958: separate structs/enums and functions for perl parser
* 
* 67   7/26/12 11:08a jgarrett
* SW7425-3603: Correcting DRC default level handling
* 
* 66   6/26/12 2:14p jgarrett
* SWDEPRECATED-3659: Adding NEXUS_AudioDecoder_GetCapabilities
* 
* 65   6/1/12 2:31p jgarrett
* SW7425-2889: Adding littleEndian flag for DTS audio to support DTS-CD.
* 
* 64   3/13/12 6:18p jgarrett
* SW7435-24: Adding programmable DSP index
* 
* 63   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 62   12/20/11 4:13p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* 61   12/2/11 4:01p jgarrett
* SW7420-2074: Adding MPEG conformance mode to AAC settings
* 
* 60   10/11/11 8:24a erickson
* SW7420-1992: move types to nexus_audio_decoder_types.h
* 
* 59   9/20/11 5:09p jgarrett
* SW7425-1045: Mapping additional settings and status between nexus and
*  the audio FW
* 
* 58   6/21/11 5:03p jgarrett
* SW7425-654: Adding Audio NRT APIs
* 
* 57   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 56   4/22/11 10:43a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 55   2/15/11 2:22p erickson
* SW7420-1135: refactor into nexus_audio_decoder_types.h
*
* 54   1/21/11 5:21p jhaberf
* SW35230-2819: Added AC3/EAC3 bit stream ID to the audio decoder status
*
* 53   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
*
* 52   11/18/10 11:06a jhaberf
* SW35230-91: Added ADPCM audio decoder support
*
* 51   11/3/10 6:20p jgarrett
* SW7125-700: Adding new codec status
*
* 50   9/7/10 6:20p jgarrett
* SW7125-623: Adding programmable TSM GA and Discard thresholds.
*
* 49   9/1/10 5:39p jgarrett
* SW7405-4796: Adding streamStatusAvailable callback
*
* 48   8/18/10 10:06a jgarrett
* SW7420-790: Merging MS11 updates to main branch
*
* SW7420-790/2   8/16/10 5:07p jgarrett
* SW7420-790: Merge to latest baseline
*
* 47   8/5/10 2:39p erickson
* SWDEPRECATED-2425: update multichannel comments
*
* 46   7/28/10 11:20a jgarrett
* SW7405-4212: Adding MS10 substream ID for ac3/ac3+
*
* SW7420-790/1   6/22/10 6:28p jgarrett
* SW7420-790: Adding new MS10/MS11 AC3/AC3Plus configuration data
*
* 45   6/10/10 6:54p jgarrett
* SW7405-4212: Adding fields required for MS10 certification
*
* 44   5/3/10 5:22p jgarrett
* SW7405-4212: Removing DolbyPulse as an explicit codec type
*
* SW7405-4212/2   4/28/10 2:52p jgarrett
* SW7405-4212: Adding support for DSP mixing
*
* SW7405-4212/1   4/26/10 5:23p jgarrett
* SW7405-4212: Adding MS10 changes for pulse/transcode
*
* 43   4/12/10 1:12p jgarrett
* SW7405-3972: Adding new versioning scheme for DTS and other algorithms
*
* 42   3/18/10 10:55a jgarrett
* SW7405-4030: Adding Dolby Pulse Support
*
* 41   2/8/10 3:17p jgarrett
* SW3556-1017: Merge to main branch
*
* SW3556-1017/2   2/2/10 2:02p jgarrett
* SW3556-983: Adding dialog normalization control for AC3/AC3+
*
* SW3556-1017/1   2/2/10 1:41p jgarrett
* SW3556-1017: Adding descriptor ramp setting
*
* 40   12/31/09 11:13a jgarrett
* SW7405-3158: Renaming DTS config variables
*
* 39   12/2/09 4:43p vsilyaev
* SW7405-3158: Added DTS codec specific configuration
*
* 38   12/1/09 6:08p vsilyaev
* SW7405-3515: Added left/right AAC downmix
*
* 37   11/25/09 2:19p vsilyaev
* SW7405-2740: Added WMA Pro codec configuration
*
* 36   11/24/09 2:24p vsilyaev
* SW7405-3419: Added flag to control decoding of 96KHz audio streams
*
* 35   11/2/09 11:16a erickson
* SW7405-3315: remove playbackOverride. use NEXUS_StcChannelMode instead.
*
* 34   10/15/09 11:52a jgarrett
* SW7405-2352: Adding CDB overflow and underflow status values
*
* 33   10/15/09 11:39a jgarrett
* SW7405-2352: Adding CDB overflow and underflow callbacks
*
* 32   9/11/09 3:14p jgarrett
* SW7405-3006: Adding ptsError callback and additional status
*
* 31   6/30/09 10:50a jgarrett
* PR 52051: Adding dialog normalization for AC3/AC3+
*
* 30   6/3/09 10:39a erickson
* PR55648: change type and clarify meaning of
*  NEXUS_AudioDecoderStatus.timebase
*
* PR52051/1   6/23/09 6:44p mward
* PR 52051:  Add dialog normalization enable setting.
*
* 29   5/14/09 3:54p jgarrett
* PR 54786: Adding lock status
*
* 28   4/6/09 5:25p jgarrett
* PR 53393: Adding memory configuration options
*
* 26   4/3/09 6:48p jgarrett
* PR 52821: Switching GFX enums to new values
*
* 25   3/3/09 12:23p jgarrett
* PR 52687: Adding AAC downmix and codec settings and wideGaThreshold
*  options
*
* 24   2/9/09 5:56p jgarrett
* PR 51309: Changing the AAC Acmod dual mono name
*
* 23   2/9/09 5:49p jgarrett
* PR 51309: Changing AAC Acmod to include dual mono value
*
* 22   1/29/09 4:24p jgarrett
* PR 51692: Adding ptsStcDifference
*
* 21   12/11/08 4:16p jgarrett
* PR 49886: Adding word length and max word length for SPDIF/HDMI sources
*
* 20   12/4/08 10:42a jgarrett
* PR 49975: Adding NEXUS_AudioDecoderStatus.tsm
*
* 19   9/26/08 6:47p jgarrett
* PR 47226: Converting LFE enable to auto/manual option
*
* 18   9/22/08 6:12p jgarrett
* PR 47189: Adding independent delay flag
*
* 17   9/11/08 6:02p jgarrett
* PR 46557: Revamping codec-specifics
*
* 16   8/22/08 4:49p jgarrett
* PR 44766: PR 44767: Reworking downmix and dual-mono logic
*
* 15   7/23/08 3:01p jgarrett
* PR 39452: Merging to main branch
*
* 14   7/21/08 1:59p erickson
* PR40222: change ptsOffset to signed int
*
* 13   7/17/08 5:24p jgarrett
* PR 42642: Adding fifoThreshold
*
* 12   7/17/08 3:29p jgarrett
* PR 44509: Adding AudioDecoderOpenSettings for fifoSize
*
* 11   7/14/08 12:14p erickson
* PR42739: added playbackOverride option for IPTV
*
* 10   6/19/08 3:16p jgarrett
* PR 42243: Moving input volume control to specific inputs and out of
*  mixer
*
* Nexus_Audio_Descriptors/1   5/15/08 5:35p jgarrett
* PR 42360: Adding AudioDescriptor interface
*
* 9   5/13/08 7:16p jgarrett
* PR 42221: Adding frame queue
*
* 8   4/22/08 3:47p jgarrett
* PR 40118: Removing duplicate stereo mode setting
*
* 7   3/11/08 2:12p erickson
* PR40222: add ptsOffset
*
* 6   2/7/08 12:11p vsilyaev
* PR 38692: Added attributes for shutdown functions
*
* 5   2/6/08 4:28p vsilyaev
* PR 38682: Added attributes for the linux kernel/user proxy mode
*
* 4   1/23/08 8:37p vobadm
* PR35457: update docs
*
* 3   1/23/08 5:16p erickson
* PR35457: update docs
*
* 2   1/23/08 2:39p erickson
* PR35457: update docs
*
* 1   1/18/08 2:15p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/16   1/9/08 5:56p jgarrett
* PR 38535: Adding additional dolby settings and status values
*
* Nexus_Devel/15   11/30/07 1:18p erickson
* PR34925: rename to nexus_stc_channel.h
*
* Nexus_Devel/14   11/29/07 2:32p erickson
* PR35457: doc update
*
* Nexus_Devel/13   11/29/07 10:32a jgarrett
* PR 37471: Adding non-transport sources
*
* Nexus_Devel/12   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
*
* Nexus_Devel/11   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/10   10/24/07 3:00p vsilyaev
* PR 36404: Trick mode extension
*
* Nexus_Devel/9   10/9/07 3:49p erickson
* PR35824: change Params to StartSettings
*
* Nexus_Devel/8   10/4/07 2:25p erickson
* PR34594: removed audio descriptors
*
* Nexus_Devel/7   10/4/07 12:33p erickson
* PR34594: change to NEXUS_AudioDecoder_GetDefaultSettings
*
* Nexus_Devel/6   9/27/07 7:51p jgarrett
* PR 34594: Moving downmix type to audio_types.h
*
* Nexus_Devel/5   9/17/07 1:32p jgarrett
* PR 34954: Implementing naming convention changes
*
* Nexus_Devel/4   9/11/07 10:03a erickson
* PR34254: update
*
* Nexus_Devel/4   9/11/07 10:01a erickson
* PR34254: update
*
* Nexus_Devel/3   9/10/07 11:30a jgarrett
* PR 34254: Changing to NEXUS_Error
*
* Nexus_Devel/2   9/6/07 2:57p jgarrett
* PR 34254: Fixing Timebase and StcChannel
*
* Nexus_Devel/4   8/28/07 1:30p erickson
* PR34254: added stcChannel, rework api
*
* Nexus_Devel/3   8/24/07 11:57a jgarrett
* PR 34254: Implementing Source -> Input name changes
*
***************************************************************************/
#ifndef NEXUS_AUDIO_DECODER_H__
#define NEXUS_AUDIO_DECODER_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_decoder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AudioDecoder

Header file: nexus_audio_decoder.h

Module: Audio

Description: Decode compressed audio from a NEXUS_PidChannel

**************************************/

/***************************************************************************
Summary:
Audio decoder type
***************************************************************************/
typedef enum NEXUS_AudioDecoderType
{
    NEXUS_AudioDecoderType_eDecode,
    NEXUS_AudioDecoderType_eAudioDescriptor,
    NEXUS_AudioDecoderType_ePassthrough,
    NEXUS_AudioDecoderType_eMax
} NEXUS_AudioDecoderType;

/***************************************************************************
Summary:
Audio decoder open settings
***************************************************************************/
typedef struct NEXUS_AudioDecoderOpenSettings
{
    NEXUS_AudioDecoderType type;

    unsigned fifoSize;                  /* Audio FIFO size in bytes */
    unsigned ancillaryDataFifoSize;     /* Ancillary Data FIFO size in bytes */

    bool independentDelay;              /* Set to true to enable independent output delay */

    NEXUS_AudioMultichannelFormat multichannelFormat;   /*
        Allocate resources at Open-time so that NEXUS_AudioOutput_AddInput with NEXUS_AudioDecoderConnectorType_eMultichannel can do multichannel output.
        If you set multichannelFormat = NEXUS_AudioMultichannelFormat_eNone, then NEXUS_AudioDecoderConnectorType_eMultichannel will simply result in stereo.
        If you set multichannelFormat to something other than eNone, then NEXUS_AudioDecoderConnectorType_eMultichannel will result in the specified multichannel format.
        This setting has no effect on connector types other than NEXUS_AudioDecoderConnectorType_eMultichannel.
        Default is NEXUS_AudioMultichannelFormat_eNone to save resources. */

    unsigned dspIndex;  /* Index of the DSP you would like to use.  Default = 0. */
} NEXUS_AudioDecoderOpenSettings;

/***************************************************************************
Summary:
Get default open settings for an audio decoder
***************************************************************************/
void NEXUS_AudioDecoder_GetDefaultOpenSettings(
    NEXUS_AudioDecoderOpenSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open an audio decoder of the specified type
***************************************************************************/
NEXUS_AudioDecoderHandle NEXUS_AudioDecoder_Open( /* attr{destructor=NEXUS_AudioDecoder_Close}  */
    unsigned index,
    const NEXUS_AudioDecoderOpenSettings *pSettings   /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an audio decoder of the specified type
***************************************************************************/
void NEXUS_AudioDecoder_Close(
    NEXUS_AudioDecoderHandle handle
    );

/***************************************************************************
Summary:
Get Settings for an audio decoder
***************************************************************************/
void NEXUS_AudioDecoder_GetSettings(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioDecoderSettings *pSettings   /* [out] Settings */
    );

/***************************************************************************
Summary:
Set Settings for an audio decoder
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_SetSettings(
    NEXUS_AudioDecoderHandle handle,
    const NEXUS_AudioDecoderSettings *pSettings /* Settings */
    );

/***************************************************************************
Summary:
Initialize an audio decoder program structure
***************************************************************************/
void NEXUS_AudioDecoder_GetDefaultStartSettings(
    NEXUS_AudioDecoderStartSettings *pSettings /* [out] Program Defaults */
    );

/***************************************************************************
Summary:
Start deocding the specified program
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Start(
    NEXUS_AudioDecoderHandle handle,
    const NEXUS_AudioDecoderStartSettings *pSettings    /* What to start decoding */
    );

/***************************************************************************
Summary:
Stop deocding the current program
***************************************************************************/
void NEXUS_AudioDecoder_Stop(
    NEXUS_AudioDecoderHandle handle
    );

/***************************************************************************
Summary:
Discards all data accumulated in the decoder buffer
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Flush(
    NEXUS_AudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Get codec-specific decoder settings
***************************************************************************/
void NEXUS_AudioDecoder_GetCodecSettings(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioCodec codec,
    NEXUS_AudioDecoderCodecSettings *pSettings  /* [out] settings for specified codec */
    );

/***************************************************************************
Summary:
    Set codec-specific decoder settings
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_SetCodecSettings(
    NEXUS_AudioDecoderHandle handle,
    const NEXUS_AudioDecoderCodecSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the current audio decoder status
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_GetStatus(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioDecoderStatus *pStatus   /* [out] current status */
    );

/***************************************************************************
Summary:
    Get raw channel status information from the decoder

Description:
    When the decoder is connected to a digital input, this routine can
    return the raw channel status bit information from the input device.
    Currently, this applies to HDMI or SPDIF inputs only.  This routine
    will return an error if not connected to a digital input.

See Also:
    NEXUS_SpdifOutput_SetRawChannelStatus
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_GetRawChannelStatus(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioRawChannelStatus *pStatus   /* [out] current status */
    );

/***************************************************************************
Summary:
Audio decoder connection type 
 
Description: 
These are legacy macros for backward compatibility.  Please refer to 
NEXUS_AudioConnectorType defined in nexus_audio_types.h. 
 
See Also: 
NEXUS_AudioConnectorType 
***************************************************************************/
#define NEXUS_AudioDecoderConnectorType                 NEXUS_AudioConnectorType
#define NEXUS_AudioDecoderConnectorType_eStereo         NEXUS_AudioConnectorType_eStereo       /* two channel PCM audio */
#define NEXUS_AudioDecoderConnectorType_eMultichannel   NEXUS_AudioConnectorType_eMultichannel /* multichannel PCM audio 
                                                            (e.g. 5.1 or 7.1). see NEXUS_AudioDecoderOpenSettings.multichannelFormat for
                                                            required open-time settings to enable multichannel output. */
#define NEXUS_AudioDecoderConnectorType_eCompressed     NEXUS_AudioConnectorType_eCompressed   /* compressed audio (may be stereo or multichannel) */
#define NEXUS_AudioDecoderConnectorType_eMono           NEXUS_AudioConnectorType_eMono         /* single-channel PCM audio */
#define NEXUS_AudioDecoderConnectorType_eMax            NEXUS_AudioConnectorType_eMax

/***************************************************************************
Summary:
    Get an audio connector for use in the audio mixer
***************************************************************************/
NEXUS_AudioInput NEXUS_AudioDecoder_GetConnector( 
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioConnectorType type
    );

/***************************************************************************
Summary:
Discover if a codec is supported.

Description:
Codecs may not be supported because of lack of hardware support, compile-time options, or
run-time options like module memory settings.
***************************************************************************/
void NEXUS_AudioDecoder_IsCodecSupported(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioCodec codec,
    bool *pSupported
    );


/***************************************************************************
Summary:
Get ancillary data from a digital stream.

Description:
Ancillary data for audio streams can contain additional per-frame metadata, 
including information about the encoder used and track information.  This 
interface allows the application to capture and process that data. The format 
of the data is stream-dependent.

The format of the data stream is NEXUS_AudioAncillaryDataHeader, followed by 
a whole ancillary data packet.  Ringbuffer wraps are handled internally, so 
NEXUS_AudioDecoder_GetAncillaryDataBuffer will always return whole packets 
including headers.

You must specify the NEXUS_AudioDecoderSettings.ancillaryDataEnabled to enable 
data flow to this interface.

NEXUS_AudioDecoder_GetUserDataBuffer can be called multiple times in a row and is non-destructive.

See Also:
NEXUS_AudioDecoder_AncillaryDataReadComplete
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_GetAncillaryDataBuffer(
    NEXUS_AudioDecoderHandle handle,
    void **pBuffer,   /* [out] attr{memory=cached} pointer to ancillary data */
    size_t *pSize     /* [out] number of bytes of userdata */
    );

/***************************************************************************
Summary:
Tell the audio decoder how much data was consumed from the last NEXUS_AudioDecoder_GetAncillaryDataBuffer call.

Description:
NEXUS_AudioDecoder_AncillaryDataReadComplete can only be called once 
after NEXUS_AudioDecoder_GetAncillaryDataBuffer. After calling 
NEXUS_AudioDecoder_AncillaryDataReadComplete, you must call 
NEXUS_AudioDecoder_GetAncillaryDataBuffer again before processing any 
more data.

See Also:
NEXUS_AudioDecoder_GetAncillaryDataBuffer
***************************************************************************/
void NEXUS_AudioDecoder_AncillaryDataReadComplete(
    NEXUS_AudioDecoderHandle handle,
    size_t size   /* number of bytes of userdata consumed by the application */
    );

/***************************************************************************
Summary:
Discard all ancillary data which is currently queued.
***************************************************************************/
void NEXUS_AudioDecoder_FlushAncillaryData(
    NEXUS_AudioDecoderHandle handle
    );


#ifdef __cplusplus
}
#endif

#endif

