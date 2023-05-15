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
* $brcm_Workfile: nexus_core_audio.h $
* $brcm_Revision: 35 $
* $brcm_Date: 2/9/12 2:24p $
*
* API Description:
*   API name: CoreAudio
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/include/priv/nexus_core_audio.h $
* 
* 35   2/9/12 2:24p jgarrett
* SW7425-2074: Merge to main branch
* 
* SW7425-2074/2   2/3/12 5:19p jgarrett
* SW7425-2074: Rebasing to latest code
* 
* SW7425-2074/1   1/18/12 6:06p jgarrett
* SW7425-2074: Initial version of InputCapture API
* 
* 34   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 33   10/11/11 4:46p jgarrett
* SW7425-1349: Adding AudioDummyOutput
* 
* 32   8/24/11 11:55a jgarrett
* SW7425-724: Adding RF Audio Encoder
* 
* 31   7/8/11 6:40p jgarrett
* SWDTV-6760: Adding I2sMultiOutput
* 
* 30   6/5/11 6:57p jgarrett
* SW7425-406: Initial FW mixer implemenation on APE
* 
* 29   4/27/11 6:51p jgarrett
* SW7420-1826: Adding AudioOutputGroup
* 
* 28   3/11/11 6:02p jgarrett
* SW7422-146: Decoder supports external inputs
* 
* 27   1/18/11 4:19p jgarrett
* SW7422-146: Adding names to input/output objects
* 
* 26   1/14/11 11:38a jgarrett
* SW7425-41: Adding dummy sink output ports
* 
* 25   12/29/10 6:29p jgarrett
* SW7425-41: Adding audio encoder and mux outputs
* 
* 24   11/18/10 11:06a jhaberf
* SW35230-90: Added new AudioInputType enum for customer specific audio
*  processing
* 
* 23   11/10/10 7:07p jhaberf
* SW3548-2923: Added Support for SRS Studio.
* 
* 22   10/27/10 4:16p erickson
* SW7420-1215: add BDBG_OBJECT checking to NEXUS_AudioInput and
*  NEXUS_AudioOutput
* 
* 21   9/15/10 3:26p jhaberf
* SW35230-1214: Audio Return Channel (ARC) support on Nexus SPDIF
* 
* SW35230-1214/2   9/15/10 2:53p akam
* SW35230-1214: Added ARC support in Nexus audio.
* 
* SW35230-1214/1   9/15/10 2:30p akam
* SW35230-1214: Added ARC support in Nexus audio.
* 
* 20   8/18/10 10:06a jgarrett
* SW7420-790: Merging MS11 updates to main branch
* 
* SW7420-790/3   8/16/10 5:07p jgarrett
* SW7420-790: Merge to latest baseline
* 
* 19   6/17/10 1:39p jhaberf
* SW35230-211: Update Nexus audio for 35230 RAP changes. Add the new
*  secondary I2S port.
* 
* SW7420-790/2   6/11/10 5:00p jgarrett
* SW7420-790: Adding MS11 Support
* 
* SW7420-790/1   6/8/10 5:49p jgarrett
* SW7420-790: Adding DolbyVolume258
* 
* 18   5/5/10 3:27p jgarrett
* SW7405-3993: Adding AudysseyAdv support
* 
* 17   5/3/10 5:24p jgarrett
* SW3548-2899: Adding Audyssey ABX support
* 
* 16   4/29/10 1:56p jhaberf
* SW35230-91: Added SBC encode for the 35230
* 
* 15   3/25/10 3:11p jgarrett
* SW3548-2827: Adding Mono Downmix Support
* 
* 14   3/18/10 5:07p jgarrett
* SW7405-4030: Adding 3D Surround
* 
* 13   12/8/09 11:07a jgarrett
* SW7408-17: Adding 7408 support
* 
* 12   9/9/09 1:08p erickson
* SW7405-2977: remove dependency on magnum RAP PI
*
* 11   3/20/09 6:56p jgarrett
* PR 53293: Adding DolbyVolume
*
* 10   2/13/09 9:26a jgarrett
* PR 46135: Adding TruVolume
*
* 9   10/4/08 6:21a jgarrett
* PR 45709: Adding AC3 encode
*
* 8   9/10/08 1:15p jgarrett
* PR 46131: Adding DtsEncode
*
* 7   9/9/08 5:01p jgarrett
* PR 46637: Adding TruSurroundHd
*
* 6   5/1/08 10:21a jgarrett
* PR 42360: Adding analog audio decoder
*
* 5   4/15/08 12:36p jgarrett
* PR 41621: Adding module handle for callbacks.
*
* 4   4/7/08 12:45p jgarrett
* PR 40116: Merging to main branch
*
* PR40116/1   4/2/08 6:33p bingz
* PR40116: Need to connect channel order selection to all outputs
*
* 3   2/20/08 1:32p erickson
* PR39405: added Rfm
*
* 2   2/5/08 1:49p jgarrett
* PR 39017: Converting AudioOutputList to use NEXUS_AudioOutput instead
*  of ports.
*
* 1   1/18/08 2:22p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/5   1/4/08 6:01p jgarrett
* PR 38535: Adding new types
*
* Nexus_Devel/4   12/20/07 2:30p jgarrett
* PR 38178: Adding RfDecoder
*
* Nexus_Devel/3   11/12/07 6:49p jgarrett
* PR 36953: Adding AUDIO_INPUT_INIT and AUDIO_OUTPUT_INIT
*
* Nexus_Devel/2   11/9/07 9:59a jgarrett
* PR 36774: Adding prepareToStart callback
*
* Nexus_Devel/1   11/6/07 4:23p jgarrett
* PR 34954: Migrating to audio connector model
*
***************************************************************************/
#ifndef NEXUS_CORE_AUDIO_H__
#define NEXUS_CORE_AUDIO_H__

#include "bstd.h"

/* Do not include Magnum audio header files here. */

typedef enum NEXUS_AudioInputType
{
    NEXUS_AudioInputType_eDecoder,
    NEXUS_AudioInputType_ePlayback,
    NEXUS_AudioInputType_eI2s,
    NEXUS_AudioInputType_eHdmi,
    NEXUS_AudioInputType_eSpdif,
    NEXUS_AudioInputType_eRf,
    NEXUS_AudioInputType_eRfDecoder,
    NEXUS_AudioInputType_eMixer,
    NEXUS_AudioInputType_eDspMixer, /* Used only on APE platforms */
    NEXUS_AudioInputType_eEncoder,
    NEXUS_AudioInputType_eRfEncoder,
    NEXUS_AudioInputType_eBbe,
    NEXUS_AudioInputType_eXen,
    NEXUS_AudioInputType_eDolbyDigitalBass,
    NEXUS_AudioInputType_eDtsNeo,
    NEXUS_AudioInputType_eAutoVolumeLevel,
    NEXUS_AudioInputType_eProLogic2,
    NEXUS_AudioInputType_eTruSurroundXt,
    NEXUS_AudioInputType_eTruSurroundHd,
    NEXUS_AudioInputType_eTruVolume,
    NEXUS_AudioInputType_eCustomSurround,
    NEXUS_AudioInputType_eCustomBass,
    NEXUS_AudioInputType_eCustomVoice,
    NEXUS_AudioInputType_eAnalogDecoder,
    NEXUS_AudioInputType_eAnalogInput,
    NEXUS_AudioInputType_eDtsEncode,
    NEXUS_AudioInputType_eAc3Encode,
    NEXUS_AudioInputType_eDolbyVolume,
    NEXUS_AudioInputType_eDolbyVolume258,
    NEXUS_AudioInputType_e3dSurround,
    NEXUS_AudioInputType_eMonoDownmix,
    NEXUS_AudioInputType_eSbcEncode,
    NEXUS_AudioInputType_eAudysseyAbx,
    NEXUS_AudioInputType_eAudysseyAdv,
    NEXUS_AudioInputType_eDolbyDigitalReencode,
    NEXUS_AudioInputType_eStudioSound,
    NEXUS_AudioInputType_eCustomAvl,
    NEXUS_AudioInputType_eCustomDbe,
    NEXUS_AudioInputType_eCustomAcc,
    NEXUS_AudioInputType_eEchoCanceller,
    NEXUS_AudioInputType_eInputCapture,
    NEXUS_AudioInputType_eMax
} NEXUS_AudioInputType;

typedef enum NEXUS_AudioInputFormat
{
    NEXUS_AudioInputFormat_eNone,
    NEXUS_AudioInputFormat_ePcmStereo,
    NEXUS_AudioInputFormat_ePcm5_1,
    NEXUS_AudioInputFormat_ePcm7_1,
    NEXUS_AudioInputFormat_ePcmMono,
    NEXUS_AudioInputFormat_eCompressed,
    NEXUS_AudioInputFormat_eMax
} NEXUS_AudioInputFormat;

typedef enum NEXUS_AudioOutputType
{
    NEXUS_AudioOutputType_eDac = 0,
    NEXUS_AudioOutputType_eI2s,
    NEXUS_AudioOutputType_eI2sMulti,
    NEXUS_AudioOutputType_eSpdif,
    NEXUS_AudioOutputType_eHdmi,
    NEXUS_AudioOutputType_eRfm,
    NEXUS_AudioOutputType_eCapture,
    NEXUS_AudioOutputType_eArc,
    NEXUS_AudioOutputType_eMux,
    NEXUS_AudioOutputType_eGroup,
    NEXUS_AudioOutputType_eDummy,
    NEXUS_AudioOutputType_eMax
} NEXUS_AudioOutputType;

typedef struct NEXUS_AudioInputFormatData
{
    BAVC_AudioSamplingRate samplingRate;
} NEXUS_AudioInputFormatData;

typedef NEXUS_Error (*NEXUS_AudioInputConnectCb)(void *, struct NEXUS_AudioInputObject *);
typedef NEXUS_Error (*NEXUS_AudioInputDisconnectCb)(void *, struct NEXUS_AudioInputObject *);

/**
Typecast to BRAP_OutputPort in audio module.
**/
typedef enum NEXUS_AudioOutputPort
{
    NEXUS_AudioOutputPort_eSpdif = 0,
    NEXUS_AudioOutputPort_eDac0,
    NEXUS_AudioOutputPort_eI2s0,
    NEXUS_AudioOutputPort_eI2s1,
    NEXUS_AudioOutputPort_eI2s2,
    NEXUS_AudioOutputPort_eDac1,
    NEXUS_AudioOutputPort_eMai,
    NEXUS_AudioOutputPort_eFlex,
    NEXUS_AudioOutputPort_eRfMod,
#if BRAP_VER >= 4
    NEXUS_AudioOutputPort_eArc,
#endif
    NEXUS_AudioOutputPort_eI2s3,
    NEXUS_AudioOutputPort_eI2s4,
    NEXUS_AudioOutputPort_eSpdif1,
    NEXUS_AudioOutputPort_eI2s5,
    NEXUS_AudioOutputPort_eI2s6,
    NEXUS_AudioOutputPort_eI2s7,
    NEXUS_AudioOutputPort_eI2s8,
    NEXUS_AudioOutputPort_eDac2,
#if BRAP_VER >= 4
    NEXUS_AudioOutputPort_eI2s9,
#endif
    NEXUS_AudioOutputPort_eMaiMulti0,
    NEXUS_AudioOutputPort_eMaiMulti1,
    NEXUS_AudioOutputPort_eMaiMulti2,
    NEXUS_AudioOutputPort_eMaiMulti3,
#if BRAP_VER >= 4
    NEXUS_AudioOutputPort_eDummySink0,
    NEXUS_AudioOutputPort_eDummySink1,
    NEXUS_AudioOutputPort_eDummySink2,
    NEXUS_AudioOutputPort_eDummySink3,
#endif
    NEXUS_AudioOutputPort_eMax
} NEXUS_AudioOutputPort;

BDBG_OBJECT_ID_DECLARE(NEXUS_AudioInput);

typedef struct NEXUS_AudioInputObject
{
    BDBG_OBJECT(NEXUS_AudioInput)
    NEXUS_AudioInputFormat format;      /* Data Format */
    NEXUS_AudioInputType objectType;    /* Object type */
    void *pObjectHandle;                /* Handle of actual "input" object */
    void *pMixerData;                   /* Blind mixer data container */
    NEXUS_ModuleHandle module;                      /* Module handle, used to serialize callbacks if necessary */
    NEXUS_AudioInputConnectCb connectCb;            /* Connect Callback - Called prior to starting in a new connection */
    NEXUS_AudioInputDisconnectCb disconnectCb;      /* Disconnect Callback - Called when connections change */
    uint32_t port;                      /* Connector port.  Typecast to BAPE_Connector accordingly. */
    uint32_t inputPort;                 /* Input port.  Typecast to BAPE_InputPort accordingly. */
    const char *pName;
} NEXUS_AudioInputObject;

#define NEXUS_AUDIO_INPUT_INIT(input, inputType, handle) \
do \
{ \
    BKNI_Memset((input), 0, sizeof(NEXUS_AudioInputObject)); \
    BDBG_OBJECT_SET((input), NEXUS_AudioInput); \
    (input)->objectType = (inputType); \
    (input)->pObjectHandle = (handle); \
} while(0)

BDBG_OBJECT_ID_DECLARE(NEXUS_AudioOutput);

typedef struct NEXUS_AudioOutputObject
{
    BDBG_OBJECT(NEXUS_AudioOutput)
    NEXUS_AudioOutputType objectType;   /* Object type */
    void *pObjectHandle;                /* Handle of actual "input" object */
    void *pMixerData;                   /* Blind mixer data container */
    uint32_t port;                      /* Actual raptor output port, typecast to BRAP_OutputPort or BAPE_MixerOutput */
    const char *pName;                  /* Name of object */
    NEXUS_Error (*setChannelMode)(void *pHandle, NEXUS_AudioChannelMode channelMode);
} NEXUS_AudioOutputObject;

#define NEXUS_AUDIO_OUTPUT_INIT(output, outputType, handle) \
do \
{ \
    BKNI_Memset((output), 0, sizeof(NEXUS_AudioOutputObject)); \
    BDBG_OBJECT_SET((output), NEXUS_AudioOutput); \
    (output)->objectType = (outputType); \
    (output)->pObjectHandle = (handle); \
} while(0)

#define NEXUS_AUDIO_MAX_OUTPUTS (NEXUS_AudioOutputPort_eMax)
typedef struct NEXUS_AudioOutputList
{
    NEXUS_AudioOutput outputs[NEXUS_AUDIO_MAX_OUTPUTS];
} NEXUS_AudioOutputList;

#endif /* #ifndef NEXUS_CORE_AUDIO_H__ */

