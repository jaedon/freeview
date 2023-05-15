/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_input.c $
* $brcm_Revision: SW7346-1228/2 $
* $brcm_Date: 3/28/13 11:46a $
*
* API Description:
*   API name: AudioInput
*   Generic API for audio filter graph management
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_input.c $
* 
* SW7346-1228/2   3/28/13 11:46a mpeteri
* SW7346-1228: Check for case where mixer is started, but inputs are not
* 
* SW7346-1228/1   3/26/13 6:28p ialauder
* SW7346-1228: Return false in NEXUS_AudioInput_P_MixerSettingsEqual so
*  we create a mixer and src for each output.
* 
* 48   3/11/13 4:02p jgarrett
* SW7425-4487: Mixer input volume adjustments should not change output
*  volume
* 
* 47   2/18/13 8:40p bandrews
* SW7435-658: fix unsynchronized connected audio input problem via a task
*  callback
* 
* 46   2/18/13 11:19a jgarrett
* SW7435-641: Adding changes for sync channel to other audio module
*  branches
* 
* 45   10/24/12 3:00p jgarrett
* SW7425-3831: Fixing compiler warning
* 
* 44   10/9/12 11:05p jgarrett
* SW7425-3831: Merge to main branch
* 
* SW7425-3831/2   10/9/12 10:45p jgarrett
* SW7425-3831: Merge with latest main banch changes
* 
* SW7425-3831/1   9/14/12 6:12p jgarrett
* SW7425-3831: Adding programmable mixer output rate
* 
* 43   9/18/12 1:42p jgarrett
* SW7425-3944: Forcibly stopping mixers on forced cleanup
* 
* 42   7/27/12 2:56p jgarrett
* SW7425-3443: Removing DTV-only APIs and directories
* 
* 41   7/6/12 3:53p jgarrett
* SW7425-455: Allowing nodes without physical outputs
* 
* 40   7/2/12 12:41p jgarrett
* SW7425-3352: Force shutdown now stops a running DSP mixer
* 
* 39   6/28/12 6:13p jgarrett
* SW7231-877: Applying input volume if set before mixer is allocated
* 
* 38   6/5/12 5:22p jgarrett
* SW7425-3071: Adding NEXUS_AudioInput_HasConnectedOutputs
* 
* 37   6/4/12 2:11p jgarrett
* SW7425-3071: Adding routines to verify connections
* 
* 36   4/12/12 2:52p jgarrett
* SW7425-2075: Adding HBR support
* 
* 35   4/9/12 4:24p gskerl
* SW7422-354: Corrected typo: pOutputNode -> pMasterNode
* 
* 34   2/22/12 3:31p jgarrett
* SW7405-5003: Removing BKNI_Memcmp
* 
* 33   2/13/12 1:02p jgarrett
* SW7425-2377: Fixing return after setting mixer settings
* 
* 32   2/13/12 10:46a jgarrett
* SW7420-1988: Downgrading WRN to MSG for dangling outputs on shutdown
* 
* 31   2/13/12 10:40a jgarrett
* SW7425-2377: Adding error checking if mixer creation fails
* 
* 30   2/9/12 2:24p jgarrett
* SW7425-2074: Merge to main branch
* 
* SW7425-2074/1   2/7/12 7:14p jgarrett
* SW7425-2074: Adding Input Capture, enabling connector format changes
* 
* 29   1/17/12 2:53p gskerl
* SW7429-18: Changed NEXUS_AudioInput_P_GetInputPortStatus() to use new
*  fields in the BAPE_MaiInputFormatDetectionStatus struct.
* 
* 28   10/14/11 3:36p jgarrett
* SW7425-1388: Coverity CID 33925,33926,33927,33928
* 
* 27   10/11/11 4:46p jgarrett
* SW7425-1349: Adding AudioDummyOutput
* 
* 26   9/16/11 11:44a jgarrett
* SWDTV-8673: Adding input running check for AnalogAudioDecoder
* 
* 25   9/15/11 3:26p jgarrett
* SWDTV-8673: Not attempting to change mixers or equalizer stages if a
*  node is already started
* 
* 24   9/8/11 8:45a jgarrett
* SWDTV-6627: Adding nexus EQ support
* 
* 23   8/26/11 12:03p jgarrett
* SW7425-742: Merge to main branch
* 
* SW7425-724/1   8/24/11 11:55a jgarrett
* SW7425-724: Adding RF Audio Encoder
* 
* 22   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   8/11/11 6:26p jgarrett
* SWDTV-6306: Adding SPDIF Input
* 
* 21   8/9/11 9:35a jgarrett
* SWDTV-6761: Stubbing out studio sound for STB chips
* 
* 20   8/8/11 5:38p jgarrett
* SWDTV-6761: Adding StudioSound
* 
* 19   6/28/11 10:33a jgarrett
* SW7422-406: Adding input volume settings for DSP mixers
* 
* 18   6/16/11 6:02p jgarrett
* SW7425-437: Breaking downstream connections properly if source node is
*  shutdown while connected
* 
* 17   6/5/11 6:57p jgarrett
* SW7425-406: Initial FW mixer implemenation on APE
* 
* 16   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 15   5/16/11 5:19p jgarrett
* SWDTV-6762: Adding Audyssey ADV/ABX
* 
* 14   5/16/11 4:57p jgarrett
* SWDTV-6763: Adding 3D Surround
* 
* 13   5/13/11 6:32p jgarrett
* SW7425-410: Adding AVL
* 
* 12   5/13/11 3:46p jgarrett
* SW7425-552: Additional sample rate master fix for post-processing
* 
* 11   5/12/11 9:50a jgarrett
* SW7422-410: Adding custom voice auto shutdown
* 
* 10   5/9/11 10:41a jgarrett
* SW7425-552: Fixing sample rate master when post-processing is used.
* 
* 9   4/26/11 4:23p jgarrett
* SW7425-437: Not allowing dead nodes in the filter graph
* 
* 8   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 7   3/24/11 9:47a jgarrett
* SW7422-352: Removing warnings on systems without HDMI input
* 
* 6   3/23/11 6:28p jgarrett
* SW7422-352: adding HDMI input support to nexus
* 
* 5   3/22/11 2:35p jgarrett
* SW7422-356: Fixing crash on mux stop
* 
* 4   3/21/11 7:05p jgarrett
* SW7422-356: Adding MuxOutput support in APE/Nexus
* 
* 3   2/28/11 2:58p jgarrett
* SW7422-146: Added input capture to I2sInput
* 
* 2   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/

#include "nexus_audio_module.h"
#include "blst_queue.h"
#include "blst_slist.h"
#include "priv/nexus_audio_decoder_priv.h"

BDBG_MODULE(nexus_audio_input);

#if NEXUS_NUM_HDMI_INPUTS > 0
#if NEXUS_NUM_HDMI_INPUTS > 1
#error TODO: Support more than one HDMI Input
#endif
static BAPE_MaiInputHandle g_maiInput;
#endif

typedef enum NEXUS_AudioOutputTiming
{
    NEXUS_AudioOutputTiming_eFlexible,
    NEXUS_AudioOutputTiming_eDac,
    NEXUS_AudioOutputTiming_ePll,
    NEXUS_AudioOutputTiming_eMax
} NEXUS_AudioOutputTiming;

typedef struct NEXUS_AudioInputMixerNode
{
    BAPE_MixerHandle inputMixer, outputMixer, eqMixer;
    BAPE_MixerSettings settings;
    NEXUS_AudioEqualizerHandle nexusEq;
    BAPE_EqualizerHandle apeEq;
    unsigned usageCount;
    unsigned mixerIndex;
    NEXUS_AudioOutputTiming timing;
    unsigned dacTimingCount;
    unsigned pllTimingCount;
    unsigned flexTimingCount;
    BLST_Q_ENTRY(NEXUS_AudioInputMixerNode) mixerNode;
} NEXUS_AudioInputMixerNode;

typedef struct NEXUS_AudioOutputNode
{
    NEXUS_AudioOutputObject *pOutputConnector;
    NEXUS_AudioInputMixerNode *pMixerNode;          /* Copy of mixer this was added to */
    BLST_Q_ENTRY(NEXUS_AudioOutputNode) outputNode;
} NEXUS_AudioOutputNode;

typedef struct NEXUS_AudioDownstreamNode
{
    NEXUS_AudioInputObject *pDownstreamObject;
    BLST_Q_ENTRY(NEXUS_AudioDownstreamNode) downstreamNode;
} NEXUS_AudioDownstreamNode;

typedef struct NEXUS_AudioUpstreamNode
{
    void *pConnectionData;
    size_t connectionDataSize;
    NEXUS_AudioInputObject *pUpstreamObject;
    BLST_Q_ENTRY(NEXUS_AudioUpstreamNode) upstreamNode;
} NEXUS_AudioUpstreamNode;

BDBG_OBJECT_ID(NEXUS_AudioInputData);

typedef struct NEXUS_AudioInputData
{
    BDBG_OBJECT(NEXUS_AudioInputData)
    NEXUS_AudioInputObject *pConnector;
    BLST_Q_HEAD(UpstreamList, NEXUS_AudioUpstreamNode) upstreamList;       /* List of inputs to this node.  NEXUS_AudioUpstreamNodes will be in this list */
    BLST_Q_HEAD(DownstreamList, NEXUS_AudioDownstreamNode) downstreamList; /* List of downstream objects connected to this node.
                                                                              AudioDownstreamNodes will be in this list */
    BLST_Q_HEAD(OutputList, NEXUS_AudioOutputNode) outputList;             /* One input can contain one or more outputs */
    BLST_Q_HEAD(MixerList, NEXUS_AudioInputMixerNode) mixerList;

    BAPE_MixerInputVolume inputVolume;
} NEXUS_AudioInputData;

static void NEXUS_AudioInput_P_ForceStopUpstream(NEXUS_AudioInput input);
static void NEXUS_AudioInput_P_ForceStopDownstream(NEXUS_AudioInput input);
static bool NEXUS_AudioInput_P_IsRunningUpstream(NEXUS_AudioInput input);
static bool NEXUS_AudioInput_P_IsRunningDownstream(NEXUS_AudioInput input);
static void NEXUS_AudioInput_P_UnlinkOutputPort(NEXUS_AudioInput input, NEXUS_AudioOutputNode *pOutputNode);
static NEXUS_Error NEXUS_AudioInput_P_CheckOutputMixer(NEXUS_AudioInput input, NEXUS_AudioOutputNode *pOutputNode);
static NEXUS_Error NEXUS_AudioInput_P_CheckOutputMixers(NEXUS_AudioInput input);
static NEXUS_AudioOutputTiming NEXUS_AudioInput_P_GetOutputTiming(NEXUS_AudioOutput output);
static bool NEXUS_AudioInput_P_MixerSettingsEqual(const BAPE_MixerSettings *pSettings1, const BAPE_MixerSettings *pSettings2);

static NEXUS_AudioOutputTiming NEXUS_AudioInput_P_GetOutputTiming(NEXUS_AudioOutput output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    switch ( output->objectType )
    {
    case NEXUS_AudioOutputType_eArc:
    case NEXUS_AudioOutputType_eSpdif:
    case NEXUS_AudioOutputType_eI2s:
        return NEXUS_AudioOutputTiming_ePll;
    case NEXUS_AudioOutputType_eHdmi:
    case NEXUS_AudioOutputType_eCapture:
    case NEXUS_AudioOutputType_eDummy:
        return NEXUS_AudioOutputTiming_eFlexible;
    case NEXUS_AudioOutputType_eDac:
    case NEXUS_AudioOutputType_eRfm:
        return NEXUS_AudioOutputTiming_eDac;
    default:
        BDBG_ERR(("Output timing not known for output type %u", output->objectType));
        BDBG_ASSERT(0);
        return NEXUS_AudioOutputTiming_eMax;
    }
}

static bool NEXUS_AudioInput_P_MixerSettingsEqual(const BAPE_MixerSettings *pSettings1, const BAPE_MixerSettings *pSettings2)
{
    BDBG_ASSERT(NULL != pSettings1);
    BDBG_ASSERT(NULL != pSettings2);

#if 1
    /*  When Spdif and HDMI are sourcing the same compressed path and forking is performed at the SRC output, FW MS
        can lock up due to an underflow at the input of the mixer feeding Spdif (SW7346-1228).  Returning false here to force
        each output to have its own mixer and SRC. */
    return false;
#else
    if ( pSettings1->type != pSettings2->type )
    {
        return false;
    }
    if ( pSettings1->mixerSampleRate != pSettings2->mixerSampleRate )
    {
        return false;
    }
    if ( pSettings1->defaultSampleRate != pSettings2->defaultSampleRate )
    {
        return false;
    }
    if ( pSettings1->outputPll != pSettings2->outputPll )
    {
        return false;
    }
    if ( pSettings1->outputNco != pSettings2->outputNco )
    {
        return false;
    }
    if ( pSettings1->outputTimebase != pSettings2->outputTimebase )
    {
        return false;
    }
    if ( pSettings1->outputDelay != pSettings2->outputDelay )
    {
        return false;
    }

    return true;
#endif
}

bool NEXUS_AudioInput_P_IsRunning(NEXUS_AudioInput input)
{
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    if ( NEXUS_AudioInput_P_IsRunningUpstream(input) ||
         NEXUS_AudioInput_P_IsRunningDownstream(input) )
    {
        return true;
    }
    return false;
}

static bool NEXUS_AudioInput_P_IsRunningUpstream(NEXUS_AudioInput input)
{
    NEXUS_AudioUpstreamNode *pNode;
    NEXUS_AudioInputData *pData;
    bool running = false;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_IsRunning(%p)", input));

    pData = input->pMixerData;

    /* Can't be running if not connected */
    if ( NULL == pData )
    {
        BDBG_MSG(("No data - not running"));
        return false;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    switch ( input->objectType )
    {
#if NEXUS_NUM_AUDIO_DECODERS
    case NEXUS_AudioInputType_eDecoder:
        BDBG_MSG(("Decoder type - checking run status"));
        running = NEXUS_AudioDecoder_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_AUDIO_PLAYBACKS
    case NEXUS_AudioInputType_ePlayback:
        BDBG_MSG(("Playback type - checking run status"));
        running = NEXUS_AudioPlayback_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_I2S_INPUTS
    case NEXUS_AudioInputType_eI2s:
        BDBG_MSG(("I2sInput type - checking run status"));
        running = NEXUS_I2sInput_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_RF_AUDIO_DECODERS
    case NEXUS_AudioInputType_eRfDecoder:
        BDBG_MSG(("RF Audio Decoder type - checking run status"));
        running = NEXUS_RfAudioDecoder_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_ANALOG_AUDIO_DECODERS
    case NEXUS_AudioInputType_eAnalogDecoder:
    	BDBG_MSG(("Analog Decoder type - checking run status"));
    	running = NEXUS_AnalogAudioDecoder_P_IsRunning(input->pObjectHandle);
    	break;		
#endif
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES
    case NEXUS_AudioInputType_eInputCapture:
        BDBG_MSG(("InputCapture type - checking run status"));
        running = NEXUS_AudioInputCapture_P_IsRunning(input->pObjectHandle);
        break;
#endif
    default:
        /* Recurse if not a decoder type, stop if any input is running - that means we are also */
        BDBG_MSG(("other type - recursively checking run status"));
        pNode = BLST_Q_FIRST(&pData->upstreamList);
        while ( NULL != pNode && false == running )
        {
            running = running || NEXUS_AudioInput_P_IsRunningUpstream(pNode->pUpstreamObject);
            pNode = BLST_Q_NEXT(pNode, upstreamNode);
        }
        break;
    }

    BDBG_MSG(("Returning running status %d", running));
    return running;
}

static bool NEXUS_AudioInput_P_IsRunningDownstream(NEXUS_AudioInput input)
{
    NEXUS_AudioDownstreamNode *pNode;
    NEXUS_AudioInputData *pData;
    bool running = false;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_IsRunning(%p)", input));

    pData = input->pMixerData;

    /* Can't be running if not connected */
    if ( NULL == pData )
    {
        BDBG_MSG(("No data - not running"));
        return false;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    switch ( input->objectType )
    {
#if NEXUS_NUM_AUDIO_DECODERS
    case NEXUS_AudioInputType_eDecoder:
        BDBG_MSG(("Decoder type - checking run status"));
        running = NEXUS_AudioDecoder_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_AUDIO_PLAYBACKS
    case NEXUS_AudioInputType_ePlayback:
        BDBG_MSG(("Playback type - checking run status"));
        running = NEXUS_AudioPlayback_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_I2S_INPUTS
    case NEXUS_AudioInputType_eI2s:
        BDBG_MSG(("I2sInput type - checking run status"));
        running = NEXUS_I2sInput_P_IsRunning(input->pObjectHandle);
        break;
#endif
#if NEXUS_NUM_RF_AUDIO_DECODERS
    case NEXUS_AudioInputType_eRfDecoder:
        BDBG_MSG(("RF Audio Decoder type - checking run status"));
        running = NEXUS_RfAudioDecoder_P_IsRunning(input->pObjectHandle);
        break;
#endif
    default:
        break;
    }

    if ( !running )
    {
        /* Recurse  */
        BDBG_MSG(("recursively checking run status"));
        pNode = BLST_Q_FIRST(&pData->downstreamList);
        while ( NULL != pNode && false == running )
        {
            running = running || NEXUS_AudioInput_P_IsRunningDownstream(pNode->pDownstreamObject);
            pNode = BLST_Q_NEXT(pNode, downstreamNode);
        }
    }

    BDBG_MSG(("Returning running status %d", running));
    return running;
}

NEXUS_AudioInputFormat NEXUS_AudioInput_P_GetFormat(NEXUS_AudioInput input)
{
    NEXUS_AudioUpstreamNode *pNode;
    NEXUS_AudioInputFormat format;
    NEXUS_AudioInputData *pData;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_GetFormat(%p)", input));

    /* If input format is set at this node, return it.  Otherwise, recurse up, taking the maximum number of channels. */
    if ( input->format == NEXUS_AudioInputFormat_eNone )
    {
        pData = input->pMixerData;

        /* Can't do anything if not connected */
        if ( NULL == pData )
        {
            return NEXUS_AudioInputFormat_eNone;
        }
        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

        format = NEXUS_AudioInputFormat_eNone;

        pNode = BLST_Q_FIRST(&pData->upstreamList);
        while ( NULL != pNode )
        {
            NEXUS_AudioInputFormat newFormat;

            newFormat = NEXUS_AudioInput_P_GetFormat(pNode->pUpstreamObject);

            if ( newFormat > format )
            {
                format = newFormat;
            }

            pNode = BLST_Q_NEXT(pNode, upstreamNode);
        }
    }
    else
    {
        format = input->format;
    }

    return format;
}

static NEXUS_AudioInputData *NEXUS_AudioInput_P_CreateData(NEXUS_AudioInput input)
{
    NEXUS_AudioInputData *pData;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("Creating connection data for input %p", input));

    pData = BKNI_Malloc(sizeof(NEXUS_AudioInputData));
    if ( NULL != pData )
    {
        int i, j;
        BDBG_OBJECT_SET(pData, NEXUS_AudioInputData);
        input->pMixerData = pData;
        pData->pConnector = input;
        BLST_Q_INIT(&pData->upstreamList);
        BLST_Q_INIT(&pData->downstreamList);
        BLST_Q_INIT(&pData->outputList);
        BLST_Q_INIT(&pData->mixerList);

        for ( i = 0; i < BAPE_Channel_eMax; i++ )
        {
            for ( j = 0; j < BAPE_Channel_eMax; j++ )
            {
                pData->inputVolume.coefficients[i][j] = (i==j)?BAPE_VOLUME_NORMAL:BAPE_VOLUME_MIN;
            }
        }
        pData->inputVolume.muted = false;
    }

    return pData;
}

NEXUS_Error NEXUS_AudioInput_P_AddInput(NEXUS_AudioInput destination, NEXUS_AudioInput source)
{
    NEXUS_AudioInputData *pDestinationData, *pSourceData;
    NEXUS_AudioDownstreamNode *pDownstream;
    NEXUS_AudioUpstreamNode *pUpstream;
    NEXUS_Error errCode;
    BAPE_MixerAddInputSettings addInputSettings;

    BDBG_OBJECT_ASSERT(destination, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(source, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_AddInput(%p,%p)", destination, source));

    BAPE_Mixer_GetDefaultAddInputSettings(&addInputSettings);

    /* On 7440/3563/7405+ architectures, inputs must be stopped for this to proceed. */
    if ( NEXUS_AudioInput_P_IsRunning(destination) )
    {
        BDBG_ERR(("Can not add inputs to a node while it's running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( NEXUS_AudioInput_P_IsRunning(source) )
    {
        BDBG_ERR(("Can not add running inputs to a node."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( (source->objectType == NEXUS_AudioInputType_eHdmi ||
          source->objectType == NEXUS_AudioInputType_eSpdif) &&
         destination->objectType != NEXUS_AudioInputType_eDecoder )
    {
        BDBG_ERR(("Can only connect SPDIF or HDMI Input to a decoder"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Other sanity checks are done by caller, they differ per-type */
    pDestinationData = destination->pMixerData;
    if ( NULL == pDestinationData )
    {
        /* Must allocate data on the first connection */
        pDestinationData = NEXUS_AudioInput_P_CreateData(destination);
    }
    BDBG_OBJECT_ASSERT(pDestinationData, NEXUS_AudioInputData);

    pSourceData = source->pMixerData;
    if ( NULL == pSourceData )
    {
        /* Must allocate data on the first connection */
        pSourceData = NEXUS_AudioInput_P_CreateData(source);
    }
    BDBG_OBJECT_ASSERT(pSourceData, NEXUS_AudioInputData);

    pDownstream = BKNI_Malloc(sizeof(NEXUS_AudioDownstreamNode));
    if ( NULL == pDownstream )
    {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }

    pUpstream = BKNI_Malloc(sizeof(NEXUS_AudioUpstreamNode));
    if ( NULL == pUpstream )
    {
        BKNI_Free(pDownstream);
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }

    pDownstream->pDownstreamObject = destination;
    pUpstream->pUpstreamObject = source;
    pUpstream->pConnectionData = NULL;
    pUpstream->connectionDataSize = 0;

    /* Link source -> APE mixer if required */
    if ( destination->objectType == NEXUS_AudioInputType_eMixer )
    {
        NEXUS_AudioInputMixerNode *pMixerNode;
        NEXUS_AudioMixerSettings mixerSettings;

        NEXUS_AudioMixer_GetSettings(destination->pObjectHandle, &mixerSettings);

        for ( pMixerNode = BLST_Q_FIRST(&pDestinationData->mixerList);
              pMixerNode != NULL;
              pMixerNode = BLST_Q_NEXT(pMixerNode, mixerNode) )
        {
            if ( NULL == mixerSettings.master )
            {
                addInputSettings.sampleRateMaster = (source->objectType == NEXUS_AudioInputType_ePlayback)?false:true;
            }
            else
            {
                addInputSettings.sampleRateMaster = (source == mixerSettings.master) ? true : false;
            }
            errCode = BAPE_Mixer_AddInput(pMixerNode->inputMixer, (BAPE_Connector)source->port, &addInputSettings);
            if ( errCode )
            {
                BKNI_Free(pDownstream);
                BKNI_Free(pUpstream);
                return BERR_TRACE(errCode);
            }
        }
    }

    /* Create connection from source -> destination */
    BLST_Q_INSERT_TAIL(&pSourceData->downstreamList, pDownstream, downstreamNode);
    /* Create reverse-linkage for destination -> source */
    BLST_Q_INSERT_TAIL(&pDestinationData->upstreamList, pUpstream, upstreamNode);

    /* Done */
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioInput_P_RemoveAllInputs(NEXUS_AudioInput destination)
{
    NEXUS_Error errCode;
    NEXUS_AudioUpstreamNode *pNode;
    NEXUS_AudioInputData *pDestinationData;

    BDBG_MSG(("NEXUS_AudioInput_P_RemoveAllInputs(%p)", destination));

    pDestinationData = destination->pMixerData;

    if ( NULL != pDestinationData )
    {
        BDBG_OBJECT_ASSERT(pDestinationData, NEXUS_AudioInputData);
        while ( NULL != (pNode=BLST_Q_FIRST(&pDestinationData->upstreamList)) )
        {
            errCode = NEXUS_AudioInput_P_RemoveInput(destination, pNode->pUpstreamObject);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioInput_P_RemoveInput(NEXUS_AudioInput destination, NEXUS_AudioInput source)
{
    NEXUS_AudioUpstreamNode *pUpstreamNode;
    NEXUS_AudioDownstreamNode *pDownstreamNode;
    NEXUS_AudioInputData *pDestinationData;
    NEXUS_AudioInputData *pSourceData;

    BDBG_MSG(("NEXUS_AudioInput_P_RemoveInput(%p,%p)", destination, source));

    pDestinationData = destination->pMixerData;
    BDBG_OBJECT_ASSERT(pDestinationData, NEXUS_AudioInputData);    /* It's impossible for this to be NULL in a valid config. */

    /* On 7440/3563/7405+ architectures, inputs must be stopped for this to proceed. */
    if ( NEXUS_AudioInput_P_IsRunning(destination) )
    {
        BDBG_ERR(("Can not remove inputs from a node while it's running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("Unlink upstream link"));

    if ( destination->objectType == NEXUS_AudioInputType_eMixer )
    {
        NEXUS_AudioInputMixerNode *pMixerNode;
        for ( pMixerNode = BLST_Q_FIRST(&pDestinationData->mixerList);
              pMixerNode != NULL;
              pMixerNode = BLST_Q_NEXT(pMixerNode, mixerNode) )
        {
            BAPE_Mixer_RemoveInput(pMixerNode->inputMixer, (BAPE_Connector)source->port);
        }
    }

    /* TODO: Unlink post-processing */

    /* Unlink upstream link */
    pUpstreamNode = BLST_Q_FIRST(&pDestinationData->upstreamList);
    while ( NULL != pUpstreamNode && pUpstreamNode->pUpstreamObject != source )
    {
        pUpstreamNode = BLST_Q_NEXT(pUpstreamNode, upstreamNode);
    }
    /* We had better find a node or the list is broken */
    if ( NULL == pUpstreamNode)
    {
        BDBG_ASSERT(NULL != pUpstreamNode);
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    BDBG_ASSERT(pUpstreamNode->pUpstreamObject == source);
    BLST_Q_REMOVE(&pDestinationData->upstreamList, pUpstreamNode, upstreamNode);
    BDBG_MSG(("Free connection data"));
    /* Free up connection-specific data if set */
    if ( NULL != pUpstreamNode->pConnectionData )
    {
        BDBG_MSG(("Connection data exists -- freeing"));
        BKNI_Free(pUpstreamNode->pConnectionData);
        pUpstreamNode->pConnectionData = NULL;
        pUpstreamNode->connectionDataSize = 0;
    }
    BDBG_MSG(("Freeing upstream node"));
    BKNI_Free(pUpstreamNode);

    pSourceData = source->pMixerData;
    BDBG_OBJECT_ASSERT(pSourceData, NEXUS_AudioInputData);

    BDBG_MSG(("Free downstream data"));
    pDownstreamNode = BLST_Q_FIRST(&pSourceData->downstreamList);
    while ( NULL != pDownstreamNode && pDownstreamNode->pDownstreamObject != destination )
    {
        pDownstreamNode = BLST_Q_NEXT(pDownstreamNode, downstreamNode);
    }
    /* We had better find a node or the list is broken */
    if ( NULL == pDownstreamNode )
    {
        BDBG_ASSERT(NULL != pDownstreamNode);
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    BDBG_ASSERT(pDownstreamNode->pDownstreamObject == destination);
    BLST_Q_REMOVE(&pSourceData->downstreamList, pDownstreamNode, downstreamNode);
    BKNI_Free(pDownstreamNode);

    return BERR_SUCCESS;
}

bool NEXUS_AudioInput_P_IsConnected(NEXUS_AudioInput destination, NEXUS_AudioInput source)
{
    NEXUS_AudioUpstreamNode *pUpstreamNode;
    NEXUS_AudioInputData *pDestinationData;
    NEXUS_AudioInputData *pSourceData;

    BDBG_OBJECT_ASSERT(destination, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(source, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_IsConnected(%p,%p)", destination, source));

    pDestinationData = destination->pMixerData;
    pSourceData = source->pMixerData;

    /* No possible connection if data is absent for either object */
    if ( NULL == destination->pMixerData || NULL == source->pMixerData )
    {
        return false;
    }
    BDBG_OBJECT_ASSERT(pDestinationData, NEXUS_AudioInputData);
    BDBG_OBJECT_ASSERT(pSourceData, NEXUS_AudioInputData);

    /* Check for destination->source link */
    pUpstreamNode = BLST_Q_FIRST(&pDestinationData->upstreamList);
    while ( NULL != pUpstreamNode && pUpstreamNode->pUpstreamObject != source )
    {
        pUpstreamNode = BLST_Q_NEXT(pUpstreamNode, upstreamNode);
    }

    return (NULL == pUpstreamNode) ? false : true;
}

static NEXUS_Error NEXUS_AudioInput_P_IterateOutputs(NEXUS_AudioInput input, NEXUS_AudioOutputList *pOutputList, int *pIndex, bool scanMixers)
{
    int i;
    NEXUS_AudioOutputNode *pOutputNode;
    NEXUS_AudioDownstreamNode *pDownstreamNode;
    NEXUS_AudioInputData *pData;
    NEXUS_Error errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pOutputList);
    BDBG_ASSERT(NULL != pIndex);

    BDBG_MSG(("NEXUS_AudioInput_P_IterateOutputs(%p)", input));

    /* First, iterate through any outputs directly attached to this object */
    i = *pIndex;
    pData = input->pMixerData;
    BDBG_MSG(("Scanning node %p for outputs", input));
    if ( NULL == pData || i >= NEXUS_AUDIO_MAX_OUTPUTS )
    {
        if ( NULL == pData )
        {
            BDBG_MSG(("No connection data at node %p", input));
        }
        else
        {
            BDBG_MSG(("Output list full at node %p", input));
        }
        goto done;
    }
    pOutputNode = BLST_Q_FIRST(&pData->outputList);
    while ( NULL != pOutputNode && i < NEXUS_AUDIO_MAX_OUTPUTS )
    {
        BDBG_ASSERT(NULL != pOutputNode->pOutputConnector);
        pOutputList->outputs[i++] = pOutputNode->pOutputConnector;
        BDBG_MSG(("Found output %p at node %p", pOutputNode->pOutputConnector, input));
        pOutputNode = BLST_Q_NEXT(pOutputNode, outputNode);
    }

    /* Now recurse through all downstream connections */
    pDownstreamNode = BLST_Q_FIRST(&pData->downstreamList);
    while ( NULL != pDownstreamNode && i < NEXUS_AUDIO_MAX_OUTPUTS )
    {
        if ( (pDownstreamNode->pDownstreamObject->objectType != NEXUS_AudioInputType_eMixer) ||
             true == scanMixers )
        {
            BDBG_MSG(("Recursing into downstream node %p", pDownstreamNode->pDownstreamObject));
            errCode = NEXUS_AudioInput_P_IterateOutputs(pDownstreamNode->pDownstreamObject, pOutputList, &i, scanMixers);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto done;
            }
        }
        else
        {
            BDBG_MSG(("Skipping downstream mixer %p", pDownstreamNode->pDownstreamObject));
        }
        pDownstreamNode = BLST_Q_NEXT(pDownstreamNode, downstreamNode);
    }

done:
    *pIndex = i;
    return errCode;
}

NEXUS_Error NEXUS_AudioInput_P_GetOutputs(NEXUS_AudioInput input, NEXUS_AudioOutputList *pOutputList, bool directOnly)
{
    int i=0;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pOutputList);

    BDBG_MSG(("NEXUS_AudioInput_P_GetOutputs(%p)", input));

    /* Invalidate list on first call */
    BKNI_Memset(pOutputList, 0, sizeof(NEXUS_AudioOutputList));

    return NEXUS_AudioInput_P_IterateOutputs(input, pOutputList, &i, !directOnly);
}

NEXUS_Error NEXUS_AudioInput_P_SetConnectionData(NEXUS_AudioInput destination, NEXUS_AudioInput source, const void *pData, size_t dataSize)
{
    NEXUS_AudioInputData *pInputData;
    NEXUS_AudioUpstreamNode *pUpstreamNode;

    BDBG_OBJECT_ASSERT(destination, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(source, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pData);
    BDBG_ASSERT(dataSize > 0);

    pInputData = destination->pMixerData;
    BDBG_OBJECT_ASSERT(pInputData, NEXUS_AudioInputData);

    pUpstreamNode = BLST_Q_FIRST(&pInputData->upstreamList);
    while ( NULL != pUpstreamNode && pUpstreamNode->pUpstreamObject != source )
    {
        pUpstreamNode = BLST_Q_NEXT(pUpstreamNode, upstreamNode);
    }
    if ( NULL == pUpstreamNode )
    {
        BDBG_ASSERT(NULL != pUpstreamNode);
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    if ( NULL != pUpstreamNode->pConnectionData && dataSize != pUpstreamNode->connectionDataSize )
    {
        /* Odd request.  Size has changed?  Handle it anyway */
        BKNI_Free(pUpstreamNode->pConnectionData);
        pUpstreamNode->connectionDataSize = 0;
        pUpstreamNode->pConnectionData = NULL;
    }

    if ( NULL == pUpstreamNode->pConnectionData )
    {
        /* Malloc the data */
        pUpstreamNode->pConnectionData = BKNI_Malloc(dataSize);
        if ( NULL == pUpstreamNode->pConnectionData )
        {
            return BERR_OUT_OF_SYSTEM_MEMORY;
        }
        pUpstreamNode->connectionDataSize = dataSize;
    }

    BDBG_ASSERT(NULL != pUpstreamNode->pConnectionData);

    /* Copy data */
    BKNI_Memcpy(pUpstreamNode->pConnectionData, pData, dataSize);

    return BERR_SUCCESS;
}

const void *NEXUS_AudioInput_P_GetConnectionData(NEXUS_AudioInput destination, NEXUS_AudioInput source)
{
    NEXUS_AudioInputData *pInputData;
    NEXUS_AudioUpstreamNode *pUpstreamNode;
    NEXUS_Error rc=0;

    BDBG_OBJECT_ASSERT(destination, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(source, NEXUS_AudioInput);

    pInputData = destination->pMixerData;
    BDBG_OBJECT_ASSERT(pInputData, NEXUS_AudioInputData);

    pUpstreamNode = BLST_Q_FIRST(&pInputData->upstreamList);
    while ( NULL != pUpstreamNode && pUpstreamNode->pUpstreamObject != source )
    {
        pUpstreamNode = BLST_Q_NEXT(pUpstreamNode, upstreamNode);
    }
    if ( NULL == pUpstreamNode )
    {
        BDBG_ASSERT(NULL != pUpstreamNode);
        rc=BERR_TRACE(NEXUS_UNKNOWN);
        return NULL;
    }

    return pUpstreamNode->pConnectionData;  /* Return connection data pointer - may be NULL if never set */
}

NEXUS_Error NEXUS_AudioInput_P_ConnectOutput(NEXUS_AudioInput input, NEXUS_AudioOutput output)
{
    NEXUS_AudioInputData *pInputData;
    NEXUS_AudioOutputNode *pNode;
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);

    pInputData = input->pMixerData;

    /* SPDIF and HDMI are special cases.  HDMI accepts no outputs, SPDIF
       can be connected either to a decoder or a SPDIF output as bypass */
    if ( input->objectType == NEXUS_AudioInputType_eHdmi || input->objectType == NEXUS_AudioInputType_eSpdif )
    {
        BDBG_ERR(("Can not directly connect outputs to SPDIF or HDMI Input.  Please connect to a decoder for use."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("Connecting output %p to input %p", output, input));

    if ( NULL == pInputData )
    {
        pInputData = NEXUS_AudioInput_P_CreateData(input);
        if ( NULL == pInputData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }
    BDBG_OBJECT_ASSERT(pInputData, NEXUS_AudioInputData);

#if NEXUS_HAS_AUDIO_MUX_OUTPUT
    if ( output->objectType == NEXUS_AudioOutputType_eMux )
    {
        errCode = NEXUS_AudioMuxOutput_P_AddInput(output->pObjectHandle, input);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
#endif

    pNode = BKNI_Malloc(sizeof(NEXUS_AudioOutputNode));
    if ( NULL == pNode )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }

    pNode->pOutputConnector = output;
    pNode->pMixerNode = NULL;
    BLST_Q_INSERT_TAIL(&pInputData->outputList, pNode, outputNode);

    return BERR_SUCCESS;

err_malloc:
#if NEXUS_HAS_AUDIO_MUX_OUTPUT
    if ( output->objectType == NEXUS_AudioOutputType_eMux )
    {
        NEXUS_AudioMuxOutput_P_RemoveInput(output->pObjectHandle, input);
    }    
#endif
    return errCode;
}

NEXUS_Error NEXUS_AudioInput_P_DisconnectOutput(NEXUS_AudioInput input, NEXUS_AudioOutput output)
{
    NEXUS_AudioInputData *pInputData;
    NEXUS_AudioOutputNode *pNode;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);

    pInputData = input->pMixerData;
    BDBG_OBJECT_ASSERT(pInputData, NEXUS_AudioInputData);    /* Illegal for this to be NULL with connections */

    pNode = BLST_Q_FIRST(&pInputData->outputList);
    while ( NULL != pNode && pNode->pOutputConnector != output )
    {
        pNode = BLST_Q_NEXT(pNode, outputNode);
    }
    if ( NULL == pNode)
    {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    /* Clear DAC slave selection if required. */
    if ( NEXUS_AudioOutput_P_IsDacSlave(pNode->pOutputConnector) ||
         NEXUS_AudioOutput_P_IsSpdifSlave(pNode->pOutputConnector) )
    {
        NEXUS_AudioOutput_P_SetSlaveSource(pNode->pOutputConnector, NULL);
    }

    /*
       If a DAC is removed, the slave source will be re-evaluated at next start call
       (During CheckOutputMixer).  No need to check that here.
    */

#if NEXUS_HAS_AUDIO_MUX_OUTPUT
    if ( output->objectType == NEXUS_AudioOutputType_eMux )
    {
        NEXUS_AudioMuxOutput_P_RemoveInput(output->pObjectHandle, input);
    }    
    else
#endif
    {
        NEXUS_AudioInput_P_UnlinkOutputPort(input, pNode);
    }

    BLST_Q_REMOVE(&pInputData->outputList, pNode, outputNode);

    BKNI_Memset(pNode, 0, sizeof(*pNode));
    BKNI_Free(pNode);

    return BERR_SUCCESS;
}

void NEXUS_AudioInput_Shutdown(
    NEXUS_AudioInput input
    )
{
    NEXUS_AudioInputData *pInputData;
    NEXUS_AudioOutputNode *pNode;
    NEXUS_AudioDownstreamNode *pDownstreamNode;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("Shutting down connector %p", input));

    pInputData = input->pMixerData;

    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_WRN(("Forcibly stopping inputs to input %p on shutdown", input));
    }
        NEXUS_AudioInput_P_ForceStop(input);

    if ( NULL != pInputData )
    {
        BDBG_OBJECT_ASSERT(pInputData, NEXUS_AudioInputData);
        /* Break object connections */
        switch ( input->objectType )
        {
        default:
            break;
#if NEXUS_CVOICE
        case NEXUS_AudioInputType_eCustomVoice:
            NEXUS_CustomVoice_RemoveAllInputs(input->pObjectHandle);
            break;
#endif
        case NEXUS_AudioInputType_eEncoder:
            NEXUS_AudioEncoder_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eRfEncoder:
            NEXUS_RfAudioEncoder_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eTruVolume:
            NEXUS_TruVolume_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eDolbyDigitalReencode:
            NEXUS_DolbyDigitalReencode_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eDolbyVolume:
            NEXUS_DolbyVolume_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eDolbyVolume258:
            NEXUS_DolbyVolume258_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eMixer:
        case NEXUS_AudioInputType_eDspMixer:
            NEXUS_AudioMixer_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_eAutoVolumeLevel:
            NEXUS_AutoVolumeLevel_RemoveAllInputs(input->pObjectHandle);
            break;
        case NEXUS_AudioInputType_e3dSurround:
            NEXUS_3dSurround_RemoveAllInputs(input->pObjectHandle);
            break;
        /* Remaining types are not yet supported */
        }
        /* Remove all inputs */
        rc = NEXUS_AudioInput_P_RemoveAllInputs(input);
        if (rc) {rc = BERR_TRACE(rc);}
        /* Break all downstream connections */
        while ( (pDownstreamNode=BLST_Q_FIRST(&pInputData->downstreamList)) )
        {
            BDBG_WRN(("Forcibly breaking connection between input %p and %p on shutdown", pDownstreamNode->pDownstreamObject, input));
            switch ( pDownstreamNode->pDownstreamObject->objectType )
            {
            default:
                rc = BERR_TRACE(BERR_NOT_SUPPORTED);
                break;
    #if NEXUS_CVOICE
            case NEXUS_AudioInputType_eCustomVoice:
                rc = NEXUS_CustomVoice_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
    #endif
            case NEXUS_AudioInputType_eEncoder:
                rc = NEXUS_AudioEncoder_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eRfEncoder:
                NEXUS_RfAudioEncoder_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eTruVolume:
                rc = NEXUS_TruVolume_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eDolbyDigitalReencode:
                rc = NEXUS_DolbyDigitalReencode_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eDolbyVolume:
                rc = NEXUS_DolbyVolume_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eDolbyVolume258:
                rc = NEXUS_DolbyVolume258_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eMixer:
            case NEXUS_AudioInputType_eDspMixer:
                rc = NEXUS_AudioMixer_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_eAutoVolumeLevel:
                rc = NEXUS_AutoVolumeLevel_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            case NEXUS_AudioInputType_e3dSurround:
                rc = NEXUS_3dSurround_RemoveInput(pDownstreamNode->pDownstreamObject->pObjectHandle, input);
                break;
            /* Remaining types are not yet supported */
            }
            if (rc) {
                rc = BERR_TRACE(rc);
                break;
            }
        }
        /* Remove all outputs */
        while ( NULL != (pNode = BLST_Q_FIRST(&pInputData->outputList)) )
        {
            BDBG_MSG(("Forcibly removing output %p on shutdown (port=%d)", pNode->pOutputConnector, pNode->pOutputConnector->port));
            rc = NEXUS_AudioOutput_RemoveInput(pNode->pOutputConnector, input);
            if (rc) {
                rc = BERR_TRACE(rc);
                break;
            }
        }

        /* Free connection data */
        BDBG_OBJECT_DESTROY(pInputData, NEXUS_AudioInputData);
        BKNI_Free(pInputData);
        input->pMixerData = NULL;
    }

    return ;
}

/***************************************************************************
Summary:
    Prepare the input chain to start.  May build and/or validate connections.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_PrepareToStart(
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioInputData *pData;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_PrepareToStart(%p)", input));

    pData = input->pMixerData;
    if ( NULL == pData )
    {
        /* If no connections exist, there is nothing to do */
        return BERR_SUCCESS;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    BDBG_ASSERT(input->objectType != NEXUS_AudioInputType_eHdmi);
    BDBG_ASSERT(input->objectType != NEXUS_AudioInputType_eSpdif);
    BDBG_ASSERT(input->objectType != NEXUS_AudioInputType_eMixer);

    BDBG_MSG(("Checking output mixers"));
    errCode = NEXUS_AudioInput_P_CheckOutputMixers(input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

void NEXUS_AudioInput_GetSyncSettings_priv( NEXUS_AudioInput audioInput, NEXUS_AudioInputSyncSettings *pSyncSettings )
{
    NEXUS_ASSERT_MODULE();
    switch (audioInput->objectType)
    {
    case NEXUS_AudioInputType_eDecoder:
        NEXUS_AudioDecoder_GetSyncSettings_priv((NEXUS_AudioDecoderHandle)audioInput->pObjectHandle, pSyncSettings);
        break;
    default:
        BDBG_WRN(("This audio input does not support SyncChannel"));
        break;
    }
}

NEXUS_Error NEXUS_AudioInput_SetSyncSettings_priv( NEXUS_AudioInput audioInput, const NEXUS_AudioInputSyncSettings *pSyncSettings )
{
    NEXUS_Error rc = 0;
    NEXUS_ASSERT_MODULE();
    switch (audioInput->objectType)
    {
    case NEXUS_AudioInputType_eDecoder:
        rc = NEXUS_AudioDecoder_SetSyncSettings_priv((NEXUS_AudioDecoderHandle)audioInput->pObjectHandle, pSyncSettings);
        break;
    default:
        BDBG_WRN(("This audio input does not support SyncChannel"));
        break;
    }
    return rc;
}

NEXUS_Error NEXUS_AudioInput_GetSyncStatus_isr( NEXUS_AudioInput audioInput, NEXUS_AudioInputSyncStatus *pSyncStatus )
{
    NEXUS_Error rc = 0;
    switch (audioInput->objectType)
    {
    case NEXUS_AudioInputType_eDecoder:
        rc = NEXUS_AudioDecoder_GetSyncStatus_isr((NEXUS_AudioDecoderHandle)audioInput->pObjectHandle, pSyncStatus);
        break;
    default:
        BDBG_WRN(("This audio input does not support SyncChannel"));
        break;
    }
    return rc;
}

/***************************************************************************
Summary:
    Returns the first object downstream from the current object that matches
    the specified type.  This is a depth-first search, not breadth-first.
 ***************************************************************************/
NEXUS_AudioInput NEXUS_AudioInput_P_FindByType(
    NEXUS_AudioInput input,
    NEXUS_AudioInputType type
    )
{
    NEXUS_AudioInput obj=NULL;
    NEXUS_AudioInputData *pData;
    NEXUS_AudioDownstreamNode *pNode;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    if ( NULL == input->pMixerData )
    {
        return NULL;
    }

    pData = input->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    /* Recurse through all children */
    for ( pNode = BLST_Q_FIRST(&pData->downstreamList);
        NULL != pNode;
        pNode = BLST_Q_NEXT(pNode, downstreamNode) )
    {
        if ( pNode->pDownstreamObject != NULL )
        {
            if ( pNode->pDownstreamObject->objectType == type )
            {
                return pNode->pDownstreamObject;
            }
            else
            {
                obj = NEXUS_AudioInput_P_FindByType(pNode->pDownstreamObject, type);
                if ( obj )
                {
                    return obj;
                }
            }
        }
    }

    /* Not found if we get here */
    return NULL;
}

static void NEXUS_AudioInput_P_UnlinkOutputPort(NEXUS_AudioInput input, NEXUS_AudioOutputNode *pOutputNode)
{
    NEXUS_AudioInputData *pData;
    NEXUS_AudioInputMixerNode *pMixerNode;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pOutputNode);
    pData = input->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);
    
    pMixerNode = pOutputNode->pMixerNode;
    if ( pMixerNode )
    {
        BDBG_ASSERT(pMixerNode->usageCount > 0);
        pMixerNode->usageCount--;
        BAPE_Mixer_RemoveOutput(pMixerNode->outputMixer, (BAPE_OutputPort)pOutputNode->pOutputConnector->port);
        if ( pMixerNode->usageCount == 0 )
        {
            /* Destroy the mixer */
            if ( pMixerNode->eqMixer )
            {
                BAPE_Mixer_RemoveAllInputs(pMixerNode->eqMixer);
                BAPE_Mixer_Destroy(pMixerNode->eqMixer);
            }
            if ( pMixerNode->apeEq )
            {
                BAPE_Equalizer_RemoveAllInputs(pMixerNode->apeEq);
                BAPE_Equalizer_Destroy(pMixerNode->apeEq);
            }
            BAPE_Mixer_Destroy(pMixerNode->inputMixer);
            BLST_Q_REMOVE(&pData->mixerList, pMixerNode, mixerNode);
            BKNI_Memset(pMixerNode, 0, sizeof(NEXUS_AudioInputMixerNode));
            BKNI_Free(pMixerNode);
        }
        else
        {
            switch ( NEXUS_AudioInput_P_GetOutputTiming(pOutputNode->pOutputConnector) )
            {
            case NEXUS_AudioOutputTiming_eDac:
                BDBG_ASSERT(pMixerNode->dacTimingCount > 0);
                pMixerNode->dacTimingCount--;
                break;
            case NEXUS_AudioOutputTiming_ePll:
                BDBG_ASSERT(pMixerNode->pllTimingCount > 0);
                pMixerNode->pllTimingCount--;
                break;
            case NEXUS_AudioOutputTiming_eFlexible:
                BDBG_ASSERT(pMixerNode->flexTimingCount > 0);
                pMixerNode->flexTimingCount--;
                break;
            default:
                /* Should never get here */
                BDBG_ASSERT(0);
                break;
            }

            if ( (pMixerNode->dacTimingCount == 0 && pMixerNode->timing == NEXUS_AudioOutputTiming_eDac) ||
                 (pMixerNode->pllTimingCount == 0 && pMixerNode->timing == NEXUS_AudioOutputTiming_ePll)  )
            {
                pMixerNode->timing = NEXUS_AudioOutputTiming_eFlexible;
            }
        }
        pOutputNode->pMixerNode = NULL;
    }
}

NEXUS_Error NEXUS_AudioInput_P_OutputSettingsChanged(
    NEXUS_AudioInput input, 
    NEXUS_AudioOutput output
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioInputData *pData;
    NEXUS_AudioOutputNode *pOutputNode;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    pData = input->pMixerData;
    if ( NULL == pData )
    {
        return NEXUS_SUCCESS;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    /* If we are running, stop here */
    if ( NEXUS_AudioInput_P_IsRunningUpstream(input) )
    {
        return NEXUS_SUCCESS;
    }

    /* Check my local outputs first */
    for ( pOutputNode = BLST_Q_FIRST(&pData->outputList);
          pOutputNode != NULL;
          pOutputNode = BLST_Q_NEXT(pOutputNode, outputNode) )
    {
        if ( pOutputNode->pOutputConnector == output )
        {
            errCode = NEXUS_AudioInput_P_CheckOutputMixer(input, pOutputNode);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
            return BERR_SUCCESS;
        }
    }

    BDBG_ERR(("output %p is not connected to input %p", output, input));
    BDBG_ASSERT(0);
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}

static NEXUS_Error NEXUS_AudioInput_P_CheckOutputMixer(
    NEXUS_AudioInput input, 
    NEXUS_AudioOutputNode *pOutputNode
    )
{
    NEXUS_AudioInputMixerNode *pMixerNode;
    BAPE_MixerSettings mixerSettings;
    NEXUS_AudioInputData *pData;
    NEXUS_Error errCode=0;
    NEXUS_AudioOutputTiming timing;
    BAPE_MixerAddInputSettings addInputSettings;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pOutputNode);
    pData = input->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    if ( pOutputNode->pOutputConnector->objectType == NEXUS_AudioOutputType_eMux )
    {
        /* Mux outputs do nothing here - they don't attach to a mixer */
        return BERR_SUCCESS;
    }

    timing = NEXUS_AudioInput_P_GetOutputTiming(pOutputNode->pOutputConnector);

    if ( NEXUS_AudioOutput_P_IsDacSlave(pOutputNode->pOutputConnector) ||
         NEXUS_AudioOutput_P_IsSpdifSlave(pOutputNode->pOutputConnector) )
    {
        NEXUS_AudioOutputNode *pMasterNode;
        NEXUS_AudioOutputType masterType = (NEXUS_AudioOutput_P_IsDacSlave(pOutputNode->pOutputConnector))?
            NEXUS_AudioOutputType_eDac:NEXUS_AudioOutputType_eSpdif;
        /* Setup master/slave linkage to first available master */
        for ( pMasterNode = BLST_Q_FIRST(&pData->outputList);
              pMasterNode != NULL;
              pMasterNode = BLST_Q_NEXT(pMasterNode, outputNode) )
        {
            BDBG_ASSERT(NULL != pMasterNode->pOutputConnector);
            if ( pMasterNode->pOutputConnector->objectType == masterType )
            {
                break;
            }
        }
        if ( NULL == pMasterNode )
        {
            BDBG_WRN(("No master output available to drive output %p (type %u)", pOutputNode->pOutputConnector, pOutputNode->pOutputConnector->objectType));
        }
        return NEXUS_AudioOutput_P_SetSlaveSource(pOutputNode->pOutputConnector, pMasterNode?pMasterNode->pOutputConnector:NULL);
    }

    BDBG_MSG(("Getting output mixer settings"));
    NEXUS_AudioOutput_P_GetMixerSettings(pOutputNode->pOutputConnector, &mixerSettings);
    BAPE_Mixer_GetDefaultAddInputSettings(&addInputSettings);

    if ( pOutputNode->pMixerNode )
    {
        /* See if our settings have changed relative to the current node */
        if ( !NEXUS_AudioInput_P_MixerSettingsEqual(&mixerSettings, &pOutputNode->pMixerNode->settings) ||
             NEXUS_AudioOutput_P_GetEqualizer(pOutputNode->pOutputConnector) != pOutputNode->pMixerNode->nexusEq )
        {
            BDBG_MSG(("Unlinking mixer output"));
            NEXUS_AudioInput_P_UnlinkOutputPort(input, pOutputNode);
        }
    }

    if ( NULL == pOutputNode->pMixerNode )
    {
        /* Scan through all mixers to see if we match existing settings */
        for ( pMixerNode = BLST_Q_FIRST(&pData->mixerList);
              pMixerNode != NULL;
              pMixerNode = BLST_Q_NEXT(pMixerNode, mixerNode) )
        {
            BDBG_MSG(("Comparing against mixer node %p", pMixerNode));
            if ( NEXUS_AudioInput_P_MixerSettingsEqual(&mixerSettings, &pMixerNode->settings) )
            {
                if ( (pMixerNode->timing == NEXUS_AudioOutputTiming_eDac && timing == NEXUS_AudioOutputTiming_ePll) ||
                     (pMixerNode->timing == NEXUS_AudioOutputTiming_ePll && timing == NEXUS_AudioOutputTiming_eDac))
                {
                    BDBG_MSG(("Can not add PLL and DAC outputs to the same mixer."));
                    continue;
                }
                if ( pMixerNode->nexusEq != NEXUS_AudioOutput_P_GetEqualizer(pOutputNode->pOutputConnector) )
                {
                    /* TODO: Handle creating EQ/second mixer, linking, and adding output to appropriate mixer */
                    BDBG_MSG(("Can not have different equalization with the same mixer."));
                    continue;
                }
                BDBG_MSG(("Comparing against mixer node %p - MATCH", pMixerNode));
                errCode = BAPE_Mixer_AddOutput(pMixerNode->outputMixer, (BAPE_OutputPort)pOutputNode->pOutputConnector->port);
                if ( errCode )
                {
                    return BERR_TRACE(errCode);
                }
                NEXUS_AudioOutput_P_SetOutputFormat(pOutputNode->pOutputConnector, NEXUS_AudioInput_P_GetFormat(input));
                pOutputNode->pMixerNode = pMixerNode;
                pMixerNode->usageCount++;
                switch ( timing )
                {
                case NEXUS_AudioOutputTiming_eFlexible:
                    pMixerNode->flexTimingCount++;
                    break;
                case NEXUS_AudioOutputTiming_ePll:
                    pMixerNode->pllTimingCount++;
                    if ( pMixerNode->timing == NEXUS_AudioOutputTiming_eFlexible )
                    {
                        BDBG_MSG(("Added PLL output to flex mixer.  Switching mixer timing to PLL."));
                        pMixerNode->timing = NEXUS_AudioOutputTiming_ePll;
                    }
                    break;
                case NEXUS_AudioOutputTiming_eDac:
                    pMixerNode->dacTimingCount++;
                    if ( pMixerNode->timing == NEXUS_AudioOutputTiming_eFlexible )
                    {
                        BDBG_MSG(("Added DAC output to flex mixer.  Switching mixer timing to DAC."));
                        pMixerNode->timing = NEXUS_AudioOutputTiming_eDac;
                    }
                default:
                    break;
                }
                return BERR_SUCCESS;
            }
        }

        /* If we get here, we need to add a new mixer */
        pMixerNode = BKNI_Malloc(sizeof(NEXUS_AudioInputMixerNode));
        if ( NULL == pMixerNode )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc;
        }
        BKNI_Memset(pMixerNode, 0, sizeof(NEXUS_AudioInputMixerNode));
        pMixerNode->usageCount = 1;
        switch ( timing )
        {
        case NEXUS_AudioOutputTiming_eFlexible:
            pMixerNode->flexTimingCount=1;
            break;
        case NEXUS_AudioOutputTiming_ePll:
            pMixerNode->pllTimingCount=1;
            break;
        case NEXUS_AudioOutputTiming_eDac:
            pMixerNode->dacTimingCount=1;
        default:
            break;
        }
        pMixerNode->timing = timing;
        pMixerNode->nexusEq = NEXUS_AudioOutput_P_GetEqualizer(pOutputNode->pOutputConnector);
        BDBG_MSG(("Allocating mixer"));
        pMixerNode->settings = mixerSettings;
        if ( input->objectType == NEXUS_AudioInputType_eMixer )
        {
            NEXUS_AudioMixerSettings nexusMixerSettings;
            NEXUS_AudioMixer_GetSettings(input->pObjectHandle, &nexusMixerSettings);
            /* This is intentionally set after saving the mixer settings.  All outputs from the same mixer will receive the
               same sample rate, but we don't consider this in the NEXUS_AudioOutput_P_GetMixerSettings function */
            mixerSettings.mixerSampleRate = nexusMixerSettings.outputSampleRate;
        }
        errCode = BAPE_Mixer_Create(NEXUS_AUDIO_DEVICE_HANDLE, &mixerSettings, &pMixerNode->inputMixer);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_mixer_open;
        }
        if ( input->objectType == NEXUS_AudioInputType_eMixer )
        {
            NEXUS_AudioUpstreamNode *pUpstreamNode;
            NEXUS_AudioMixerSettings mixerSettings;
            NEXUS_AudioMixer_GetSettings(input->pObjectHandle, &mixerSettings);
            /* Mixers add all upstream objects as inputs */
            for ( pUpstreamNode = BLST_Q_FIRST(&pData->upstreamList);
                  pUpstreamNode != NULL;
                  pUpstreamNode = BLST_Q_NEXT(pUpstreamNode, upstreamNode) )
            {
                /* Check for the mixer's master option */                
                if ( NULL == mixerSettings.master )
                {
                    addInputSettings.sampleRateMaster = (pUpstreamNode->pUpstreamObject->objectType == NEXUS_AudioInputType_ePlayback)?false:true;
                }
                else
                {
                    addInputSettings.sampleRateMaster = (pUpstreamNode->pUpstreamObject == mixerSettings.master) ? true : false;
                }
                errCode = BAPE_Mixer_AddInput(pMixerNode->inputMixer, (BAPE_Connector)pUpstreamNode->pUpstreamObject->port, &addInputSettings);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_add_input;
                }
                errCode = BAPE_Mixer_SetInputVolume(pMixerNode->inputMixer, (BAPE_Connector)pUpstreamNode->pUpstreamObject->port, &pData->inputVolume);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                }
            }
        }
        else
        {
            /* Non-mixers add themselves as inputs */
            addInputSettings.sampleRateMaster = (input->objectType == NEXUS_AudioInputType_ePlayback)?false:true;
            errCode = BAPE_Mixer_AddInput(pMixerNode->inputMixer, (BAPE_Connector)input->port, &addInputSettings);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_add_input;
            }
            errCode = BAPE_Mixer_SetInputVolume(pMixerNode->inputMixer, (BAPE_Connector)input->port, &pData->inputVolume);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }

        /* Determine output mixer based on whether we are using an equalizer or not */
        if ( pMixerNode->nexusEq )
        {
            BAPE_EqualizerSettings eqSettings;
            BAPE_Connector apeConnector;
            BDBG_MSG(("Equalizer active.  Outputs connect to a mixer->eq->mixer cascade."));
            BAPE_Equalizer_GetDefaultSettings(&eqSettings);
            BDBG_MSG(("Creating EQ"));
            errCode = BAPE_Equalizer_Create(NEXUS_AUDIO_DEVICE_HANDLE, &eqSettings, &pMixerNode->apeEq);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_eq_create;
            }
            BDBG_MSG(("Linking EQ to input mixer"));
            BAPE_Mixer_GetConnector(pMixerNode->inputMixer, &apeConnector);
            errCode = BAPE_Equalizer_AddInput(pMixerNode->apeEq, apeConnector);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_eq_add_input;
            }
            BDBG_MSG(("Creating cascaded mixer for EQ"));
            errCode = BAPE_Mixer_Create(NEXUS_AUDIO_DEVICE_HANDLE, &pMixerNode->settings, &pMixerNode->eqMixer);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output_mixer_create;
            }
            BAPE_Equalizer_GetConnector(pMixerNode->apeEq, &apeConnector);
            BAPE_Mixer_GetDefaultAddInputSettings(&addInputSettings);
            addInputSettings.sampleRateMaster = true;   /* Output mixer should always follow EQ Sample Rate */
            errCode = BAPE_Mixer_AddInput(pMixerNode->eqMixer, apeConnector, &addInputSettings);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output_mixer_add_input;
            }
            pMixerNode->outputMixer = pMixerNode->eqMixer;
        }
        else
        {
            BDBG_MSG(("No Equalizer.  Outputs connect to a single mixer."));
            pMixerNode->outputMixer = pMixerNode->inputMixer;
        }

        errCode = BAPE_Mixer_AddOutput(pMixerNode->outputMixer, (BAPE_OutputPort)pOutputNode->pOutputConnector->port);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_add_output;
        }

        /* Add to the list */
        pOutputNode->pMixerNode = pMixerNode;
        BLST_Q_INSERT_TAIL(&pData->mixerList, pMixerNode, mixerNode);
    }

    NEXUS_AudioOutput_P_SetOutputFormat(pOutputNode->pOutputConnector, NEXUS_AudioInput_P_GetFormat(input));

    /* Refresh equalizer stages if required.  There may be a more optimal way to do this, but this will work. */
    pMixerNode = pOutputNode->pMixerNode;
    BDBG_ASSERT(NULL != pMixerNode);
    if ( pMixerNode->nexusEq )
    {
        BAPE_EqualizerStageHandle *pStages;
        unsigned numStages;
        NEXUS_AudioEqualizer_P_GetStages(pMixerNode->nexusEq, &pStages, &numStages);
        errCode = BAPE_Equalizer_SetStages(pMixerNode->apeEq, pStages, numStages);
        if ( errCode )
        {
            NEXUS_AudioInput_P_UnlinkOutputPort(input, pOutputNode);
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;

err_add_output:
    if ( pMixerNode->eqMixer )
    {
        BAPE_Mixer_RemoveAllInputs(pMixerNode->eqMixer);
    }
err_output_mixer_add_input:
    if ( pMixerNode->eqMixer )
    {
        BAPE_Mixer_Destroy(pMixerNode->eqMixer);
    }
err_output_mixer_create:
    if ( pMixerNode->apeEq )
    {
        BAPE_Equalizer_RemoveAllInputs(pMixerNode->apeEq);
    }    
err_eq_add_input:
    if ( pMixerNode->apeEq )
    {
        BAPE_Equalizer_Destroy(pMixerNode->apeEq);
    }
err_eq_create:
    BAPE_Mixer_RemoveAllInputs(pMixerNode->inputMixer);
err_add_input:
    BAPE_Mixer_Destroy(pMixerNode->inputMixer);
err_mixer_open:
    BKNI_Free(pMixerNode);
err_malloc:
    return errCode;
}

static NEXUS_Error NEXUS_AudioInput_P_CheckOutputMixers(NEXUS_AudioInput input)
{
    NEXUS_Error errCode;
    NEXUS_AudioInputData *pData;
    NEXUS_AudioDownstreamNode *pDownstreamNode;
    NEXUS_AudioOutputNode *pOutputNode;
    bool hasOutputs=false;
    bool hasChildren=false;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    pData = input->pMixerData;
    if ( NULL == pData )
    {
        return BERR_SUCCESS;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);
    
    /* Check if mixer itself is running (mixer can be started even if inputs are not) */
    if ( input->objectType == NEXUS_AudioInputType_eDspMixer || input->objectType == NEXUS_AudioInputType_eMixer )
    {
        if ( NEXUS_AudioMixer_P_IsStarted(input->pObjectHandle) )
        {
            BDBG_MSG(("Mixer %p already started", input->pObjectHandle));
            return BERR_SUCCESS;
        }
    }

    /* Check if input is already running if this node might have more than one input */
    if ( BLST_Q_FIRST(&pData->upstreamList) != BLST_Q_LAST(&pData->upstreamList) )
    {
        if ( NEXUS_AudioInput_P_IsRunningUpstream(input) )
        {
            BDBG_MSG(("Already running"));
            return BERR_SUCCESS;
        }
    }

    /* Check my local outputs first */
    for ( pOutputNode = BLST_Q_FIRST(&pData->outputList);
          pOutputNode != NULL;
          pOutputNode = BLST_Q_NEXT(pOutputNode, outputNode) )
    {
        BDBG_MSG(("Checking Output Mixer for output %p (output type %d)", pOutputNode->pOutputConnector, pOutputNode->pOutputConnector->objectType));
        errCode = NEXUS_AudioInput_P_CheckOutputMixer(input, pOutputNode);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        hasOutputs = true;
    }

    for ( pDownstreamNode = BLST_Q_FIRST(&pData->downstreamList);
          pDownstreamNode != NULL;
          pDownstreamNode = BLST_Q_NEXT(pDownstreamNode, downstreamNode) )
    {
        /* Recurse through children */
        BDBG_MSG(("Recursively Checking Output Mixers for input %p (input type %d)", pDownstreamNode->pDownstreamObject, pDownstreamNode->pDownstreamObject->objectType));
        errCode = NEXUS_AudioInput_P_CheckOutputMixers(pDownstreamNode->pDownstreamObject);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        hasChildren = true;
    }

    switch ( input->objectType )
    {
    case NEXUS_AudioInputType_eDecoder:
    case NEXUS_AudioInputType_eDolbyDigitalReencode:
        /* This is okay for these types - they have multiple paths */
        break;
    default:
        #if 0 /* No longer required, APE can detect this */        
        if ( hasOutputs == false && hasChildren == false )
        {
            BDBG_ERR(("AudioInput %#x (type=%u) is not connected to any other nodes or outputs.  This is currently not supported.", input, input->objectType));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        #endif
        break;
    }

    return BERR_SUCCESS;
}

void NEXUS_AudioInput_P_GetVolume(
    NEXUS_AudioInput input, 
    BAPE_MixerInputVolume *pInputVolume    /* [out] */
    )
{
    NEXUS_AudioInputData *pData;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pInputVolume);
    pData = input->pMixerData;

    if ( NULL == pData )
    {
        pData = NEXUS_AudioInput_P_CreateData(input);
        if ( NULL == pData )
        {
            (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            BKNI_Memset(pInputVolume, 0, sizeof(*pInputVolume));
            return;
        }
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);
    *pInputVolume = pData->inputVolume;
}

static NEXUS_Error NEXUS_AudioInput_P_SetVolumeDownstream(
    NEXUS_AudioInput previous, 
    NEXUS_AudioInput input,
    const BAPE_MixerInputVolume *pInputVolume
    )
{
    NEXUS_AudioDownstreamNode *pDownstreamNode;
    NEXUS_AudioInputMixerNode *pMixerNode;
    NEXUS_AudioInputData *pData;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pInputVolume);

    pData = input->pMixerData;
    if ( NULL == pData )
    {
        pData = NEXUS_AudioInput_P_CreateData(input);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }
    
    switch ( input->objectType )
    {
    case NEXUS_AudioInputType_eMixer:
    case NEXUS_AudioInputType_eDspMixer:
    	/* For mixers, volume affects the input to the mixer.  Output is not affected. */
    	break;
   	default:
    	/* Store Volume */
	    pData->inputVolume = *pInputVolume;
	    break;
	}

    /* Don't apply volume past DSP mixers */
    if ( input->objectType != NEXUS_AudioInputType_eDspMixer )
    {
        /* Iterate through my mixers and apply the volume setting */
        for ( pMixerNode = BLST_Q_FIRST(&pData->mixerList);
              pMixerNode != NULL;
              pMixerNode = BLST_Q_NEXT(pMixerNode, mixerNode) )
        {
            if ( input->objectType == NEXUS_AudioInputType_eMixer )
            {
                errCode = BAPE_Mixer_SetInputVolume(pMixerNode->inputMixer, (BAPE_Connector)previous->port, pInputVolume);
            }
            else
            {
                errCode = BAPE_Mixer_SetInputVolume(pMixerNode->inputMixer, (BAPE_Connector)input->port, pInputVolume);
            }
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    /* Stop recursing after a mixer is found */
    switch ( input->objectType )
    {
    case NEXUS_AudioInputType_eMixer:
    case NEXUS_AudioInputType_eDspMixer:
        /* Propagate volume to mixer if needed */
        errCode = NEXUS_AudioMixer_P_SetInputVolume(input->pObjectHandle, previous, pInputVolume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        break;
    default:
        /* Iterate through downstream nodes and apply mixer volume */
        for ( pDownstreamNode = BLST_Q_FIRST(&pData->downstreamList);
              pDownstreamNode != NULL;
              pDownstreamNode = BLST_Q_NEXT(pDownstreamNode, downstreamNode) )
        {
            errCode = NEXUS_AudioInput_P_SetVolumeDownstream(input, pDownstreamNode->pDownstreamObject, pInputVolume);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
        break;
    }

    /* Done */
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioInput_P_SetVolume(
    NEXUS_AudioInput input, 
    const BAPE_MixerInputVolume *pInputVolume
    )
{
    return NEXUS_AudioInput_P_SetVolumeDownstream(NULL, input, pInputVolume);
}

/***************************************************************************
Summary: 
    Get an external input port handle
 ***************************************************************************/
BAPE_InputPort NEXUS_AudioInput_P_GetInputPort(
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    switch ( input->objectType )
    {
#if NEXUS_NUM_HDMI_INPUTS
    case NEXUS_AudioInputType_eHdmi:
        {
            BAPE_InputPort port;
            BAPE_MaiInput_GetInputPort(g_maiInput, &port);
            return port;
        }
#endif
    default:
        return (BAPE_InputPort)input->inputPort;
    }
}

/***************************************************************************
Summary:
Determine if this input supports dynamic format changes
 ***************************************************************************/
bool NEXUS_AudioInput_P_SupportsFormatChanges(
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    switch ( input->objectType )
    {
    case NEXUS_AudioInputType_eHdmi:
    case NEXUS_AudioInputType_eSpdif:
        return true;
    default:
        return false;
    }
}

/***************************************************************************
Summary:
Enable/Disable interrupt for dynamic format changes
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_SetFormatChangeInterrupt(
    NEXUS_AudioInput input,
    void (*pCallback_isr)(void *, int),
    void *pParam1,
    int param2
    )
{
    if ( false == NEXUS_AudioInput_P_SupportsFormatChanges(input) )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    switch ( input->objectType )
    {
#if NEXUS_NUM_HDMI_INPUTS
    case NEXUS_AudioInputType_eHdmi:
        {
            BERR_Code errCode;
            BAPE_MaiInputFormatDetectionSettings detectionSettings;
            BAPE_MaiInput_GetFormatDetectionSettings(g_maiInput, &detectionSettings);
            detectionSettings.enabled = (pCallback_isr != NULL) ? true : false;
            detectionSettings.formatChangeInterrupt.pCallback_isr = pCallback_isr;
            detectionSettings.formatChangeInterrupt.pParam1 = pParam1;
            detectionSettings.formatChangeInterrupt.param2 = param2;
            errCode = BAPE_MaiInput_SetFormatDetectionSettings(g_maiInput, &detectionSettings);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
        return BERR_SUCCESS;
#endif
#if NEXUS_NUM_SPDIF_INPUTS
    case NEXUS_AudioInputType_eSpdif:
        return NEXUS_SpdifInput_P_SetFormatChangeInterrupt(input->pObjectHandle, pCallback_isr, pParam1, param2);
#endif
    default:
        BSTD_UNUSED(pCallback_isr);
        BSTD_UNUSED(pParam1);
        BSTD_UNUSED(param2);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_AudioInput_P_Init(void)
{
#if NEXUS_NUM_HDMI_INPUTS
    {
        BAPE_MaiInputSettings settings;
        BERR_Code errCode;
        BAPE_MaiInput_GetDefaultSettings(&settings);
        errCode = BAPE_MaiInput_Open(NEXUS_AUDIO_DEVICE_HANDLE, 0, &settings, &g_maiInput);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
#endif
    return BERR_SUCCESS;
}

void NEXUS_AudioInput_P_Uninit(void)
{
#if NEXUS_NUM_HDMI_INPUTS
    BAPE_MaiInput_Close(g_maiInput);
    g_maiInput = NULL;
#endif
}

NEXUS_Error NEXUS_AudioInput_P_GetInputPortStatus(
    NEXUS_AudioInput input,
    NEXUS_AudioInputPortStatus *pStatus     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_ASSERT(NULL != pStatus);

    switch ( input->objectType )
    {
#if NEXUS_NUM_HDMI_INPUTS
    case NEXUS_AudioInputType_eHdmi:
        {
            BAPE_MaiInputFormatDetectionStatus detectionStatus;
            BAPE_MaiInput_GetFormatDetectionStatus(g_maiInput, &detectionStatus);
            pStatus->signalPresent = detectionStatus.signalPresent;
            pStatus->compressed = detectionStatus.compressed;
            pStatus->hbr = detectionStatus.hbr;
            pStatus->codec = NEXUS_Audio_P_MagnumToCodec(detectionStatus.codec);
            pStatus->sampleRate = detectionStatus.sampleRate;
            pStatus->numPcmChannels = detectionStatus.numPcmChannels;
        }
        return BERR_SUCCESS;
#endif
#if NEXUS_NUM_SPDIF_INPUTS
    case NEXUS_AudioInputType_eSpdif:
        return NEXUS_SpdifInput_P_GetInputPortStatus(input->pObjectHandle, pStatus);
#endif        
    default:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_AudioInput_P_ForceStop(NEXUS_AudioInput input)
{
    NEXUS_AudioDownstreamNode *pNode;
    NEXUS_AudioInputData *pData;

    pData = input->pMixerData;

    /* Can't be running if not connected */
    if ( NULL == pData )
    {
        return;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    NEXUS_AudioInput_P_ForceStopUpstream(input);

    /* Recurse Downstream */
    for ( pNode = BLST_Q_FIRST(&pData->downstreamList);
          NULL != pNode;
          pNode = BLST_Q_NEXT(pNode, downstreamNode) )
    {
        NEXUS_AudioInput_P_ForceStopDownstream(pNode->pDownstreamObject);
    }
}

static void NEXUS_AudioInput_P_ForceStopUpstream(NEXUS_AudioInput input)
{
    /* Stop any upstream components first */
    NEXUS_AudioUpstreamNode *pUpNode;
    NEXUS_AudioInputData *pData;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_ForceStopUpstream(%p)", input));

    pData = input->pMixerData;

    /* Can't be running if not connected */
    if ( NULL == pData )
    {
        return;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    switch ( input->objectType )
    {
#if NEXUS_NUM_AUDIO_DECODERS
    case NEXUS_AudioInputType_eDecoder:
        if ( NEXUS_AudioDecoder_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AudioDecoder_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_AUDIO_PLAYBACKS
    case NEXUS_AudioInputType_ePlayback:
        if ( NEXUS_AudioPlayback_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AudioPlayback_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_I2S_INPUTS
    case NEXUS_AudioInputType_eI2s:
        if ( NEXUS_I2sInput_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_I2sInput_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_RF_AUDIO_DECODERS
    case NEXUS_AudioInputType_eRfDecoder:
        if ( NEXUS_RfAudioDecoder_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_RfAudioDecoder_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_ANALOG_AUDIO_DECODERS
    case NEXUS_AudioInputType_eAnalogDecoder:
        if ( NEXUS_AnalogAudioDecoder_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AnalogAudioDecoder_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES > 0
    case NEXUS_AudioInputType_eInputCapture:
        if ( NEXUS_AudioInputCapture_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AudioInputCapture_Stop(input->pObjectHandle);
        }
        break;
#endif
    case NEXUS_AudioInputType_eDspMixer:
        NEXUS_AudioMixer_Stop(input->pObjectHandle);
        break;        
    default:
        break;
    }
    /* Recurse Upstream */
    for ( pUpNode = BLST_Q_FIRST(&pData->upstreamList);
          NULL != pUpNode;
          pUpNode = BLST_Q_NEXT(pUpNode, upstreamNode) )
    {
        NEXUS_AudioInput_P_ForceStopUpstream(pUpNode->pUpstreamObject);
    }
}

static void NEXUS_AudioInput_P_ForceStopDownstream(NEXUS_AudioInput input)
{
    /* Stop any upstream components first */
    NEXUS_AudioUpstreamNode *pUpNode;
    NEXUS_AudioDownstreamNode *pDownNode;
    NEXUS_AudioInputData *pData;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_ForceStopUpstream(%p)", input));

    pData = input->pMixerData;

    /* Can't be running if not connected */
    if ( NULL == pData )
    {
        return;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    switch ( input->objectType )
    {
#if NEXUS_NUM_AUDIO_DECODERS
    case NEXUS_AudioInputType_eDecoder:
        if ( NEXUS_AudioDecoder_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AudioDecoder_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_AUDIO_PLAYBACKS
    case NEXUS_AudioInputType_ePlayback:
        if ( NEXUS_AudioPlayback_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AudioPlayback_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_I2S_INPUTS
    case NEXUS_AudioInputType_eI2s:
        if ( NEXUS_I2sInput_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_I2sInput_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_RF_AUDIO_DECODERS
    case NEXUS_AudioInputType_eRfDecoder:
        if ( NEXUS_RfAudioDecoder_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_RfAudioDecoder_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_ANALOG_AUDIO_DECODERS
    case NEXUS_AudioInputType_eAnalogDecoder:
        if ( NEXUS_AnalogAudioDecoder_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AnalogAudioDecoder_Stop(input->pObjectHandle);
        }
        break;
#endif
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES > 0
    case NEXUS_AudioInputType_eInputCapture:
        if ( NEXUS_AudioInputCapture_P_IsRunning(input->pObjectHandle) )
        {
            NEXUS_AudioInputCapture_Stop(input->pObjectHandle);
        }
        break;
#endif
    case NEXUS_AudioInputType_eDspMixer:
        NEXUS_AudioMixer_Stop(input->pObjectHandle);
        /* Fall through */
    case NEXUS_AudioInputType_eMixer:
        /* Mixers are a special case.  They can have multiple inputs, all of which must be stopped also. */
        /* Recurse Upstream */
        for ( pUpNode = BLST_Q_FIRST(&pData->upstreamList);
              NULL != pUpNode;
              pUpNode = BLST_Q_NEXT(pUpNode, upstreamNode) )
        {
            NEXUS_AudioInput_P_ForceStopUpstream(pUpNode->pUpstreamObject);
        }
        break;
        /* Fall Through */
    default:
        break;
    }
    /* Recurse Downstream */
    for ( pDownNode = BLST_Q_FIRST(&pData->downstreamList);
          NULL != pDownNode;
          pDownNode = BLST_Q_NEXT(pDownNode, downstreamNode) )
    {
        NEXUS_AudioInput_P_ForceStopDownstream(pDownNode->pDownstreamObject);
    }
}

static bool NEXUS_AudioInput_P_IsConnectedDownstream(NEXUS_AudioInput source, NEXUS_AudioInput sink)
{
    NEXUS_AudioDownstreamNode *pNode;
    NEXUS_AudioInputData *pData;
    bool connected = false;

    BDBG_OBJECT_ASSERT(source, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(sink, NEXUS_AudioInput);

    BDBG_MSG(("NEXUS_AudioInput_P_IsConnectedDownstream(%p,%p)", source,sink));

    pData = source->pMixerData;

    /* Can't be running if not connected */
    if ( NULL == pData )
    {
        return false;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioInputData);

    /* This call has reversed args, sink<-source */
    connected = NEXUS_AudioInput_P_IsConnected(sink, source);

    /* Recurse  */
    BDBG_MSG(("recursively checking connected status"));
    pNode = BLST_Q_FIRST(&pData->downstreamList);
    while ( NULL != pNode && false == connected )
    {
        connected = connected || NEXUS_AudioInput_P_IsConnectedDownstream(pNode->pDownstreamObject, sink);
        pNode = BLST_Q_NEXT(pNode, downstreamNode);
    }

    BDBG_MSG(("Returning connected status %d", connected));

    return connected;
}

void NEXUS_AudioInput_IsConnectedToInput(
    NEXUS_AudioInput input1,
    NEXUS_AudioInput input2,
    bool *pConnected            /* [out] True if the nodes are connected in a filter graph */
    )
{
    bool connected;

    BDBG_OBJECT_ASSERT(input1, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(input2, NEXUS_AudioInput);
    
    connected = NEXUS_AudioInput_P_IsConnectedDownstream(input1, input2);
    if ( false == connected )
    {
        connected = NEXUS_AudioInput_P_IsConnectedDownstream(input2, input1);
    }
    *pConnected = connected;
}

void NEXUS_AudioInput_IsConnectedToOutput(
    NEXUS_AudioInput input,
    NEXUS_AudioOutput output,
    bool *pConnected            /* [out] True if the nodes are connected in a filter graph */
    )
{
    NEXUS_AudioOutputList outputList;
    NEXUS_Error errCode;
    unsigned i;

    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    BDBG_ASSERT(NULL != pConnected);

    *pConnected = false;

    errCode = NEXUS_AudioInput_P_GetOutputs(input, &outputList, false);
    if ( BERR_SUCCESS == errCode )
    {
        for ( i = 0; i < NEXUS_AUDIO_MAX_OUTPUTS; i++ )
        {
            if ( outputList.outputs[i] == output )
            {
                *pConnected = true;
                return;
            }
        }
    }
}

void NEXUS_AudioInput_HasConnectedOutputs(
    NEXUS_AudioInput input,
    bool *pConnected            /* [out] True if one or more output is connected to this node */
    )
{
    NEXUS_AudioOutputList outputList;
    NEXUS_Error errCode;
    BDBG_ASSERT(NULL != pConnected);

    NEXUS_ASSERT_MODULE();

    *pConnected = false;
    errCode = NEXUS_AudioInput_P_GetOutputs(input, &outputList, false);
    if ( BERR_SUCCESS == errCode )
    {
        if ( outputList.outputs[0] != NULL )
        {
            *pConnected = true;
        }
    }
}
