/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioDecoderOMXIL.h"
#include "../ESPlayerNative.h"
#include "../MediaUtils.h"
#include <cstring>

#include <OMX_Broadcom.h>

#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Endian.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"
#include "../DeviceLibNative.h"

using namespace std;
using namespace netflix::device::esplayer;
using namespace netflix;

AudioDecoderOMXIL::AudioDecoderOMXIL(uint32_t numOutputChannels)
    : AudioDecoder(numOutputChannels),
      ComponentOMXIL(sConfiguration->omxilAudioDecoder, OMX_IndexParamAudioInit), mFrames(0)
{
#if defined(BUILD_OPENMAXIL_AUDIO_RENDERER)
    setState(OMX_StateIdle);


#if 0
    const OMX_AUDIO_CODINGTYPE encoding = OMX_AUDIO_CodingAAC;
#elif 1
    const OMX_AUDIO_CODINGTYPE encoding = OMX_AUDIO_CodingPCM;
#else
    const OMX_AUDIO_CODINGTYPE encoding = OMX_AUDIO_CodingAutoDetect;
#endif
    {
        OMX_PARAM_PORTDEFINITIONTYPE portParam;
        OMX_INIT_STRUCTURE(portParam);
        portParam.nPortIndex = inputPort();
        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portParam));
        portParam.nPortIndex = inputPort();
        portParam.nBufferCountActual = 4;
        portParam.format.audio.eEncoding = encoding;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamPortDefinition, &portParam));
    }
    {
        OMX_AUDIO_PARAM_PORTFORMATTYPE format;
        OMX_INIT_STRUCTURE(format);
        format.nPortIndex = inputPort();
        format.eEncoding = encoding;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamAudioPortFormat, &format));
    }
    if(encoding == OMX_AUDIO_CodingAAC) {
        OMX_AUDIO_PARAM_AACPROFILETYPE aacFormat;
        OMX_INIT_STRUCTURE(aacFormat);
        aacFormat.nPortIndex = inputPort();
        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamAudioAac, &aacFormat));
        aacFormat.nChannels = numOutputChannels;
        aacFormat.eAACProfile = OMX_AUDIO_AACObjectHE;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamAudioAac, &aacFormat));
    }

    initInputBuffers();
    if(setState(OMX_StateExecuting))
        waitForCommandComplete(OMX_CommandStateSet, OMX_StateExecuting);

    mRenderer.init(sConfiguration->omxilAudioRenderer, OMX_IndexParamAudioInit);
    mRenderer.setState(OMX_StateIdle);
#if 0
    if(shared_ptr<ComponentOMXIL> clock = getClock())
        clock->receive(this);
    else
        mRenderer.receive(this);
#else
    mRenderer.receive(this);
#endif
    mRenderer.setState(OMX_StateExecuting);
#else
    setState(OMX_StateExecuting);
    enablePort(outputPort());
    OMX_PARAM_PORTDEFINITIONTYPE portFormat;
    OMX_INIT_STRUCTURE(portFormat);
    portFormat.nPortIndex = outputPort();
    OMX_BUFFERHEADERTYPE *buffer = NULL;
    const OMX_ERRORTYPE created = OMX_AllocateBuffer(handle(), &buffer, outputPort(), 0, portFormat.nBufferSize);
    if(created == OMX_ErrorNone) {
#if defined(OMX_VERBOSE)
        Log::error(TRACE_LOG, "[%s:%d]:%s:%p: CreatedOutput(%d): %d [%p]", __func__, __LINE__, name().c_str(), handle(), portFormat.nBufferSize,
                                  portFormat.nBufferCountActual, buffer);
#endif
        fillBuffer(buffer);
    } else {
        Log::error(TRACE_LOG, "[%s:%d]:%s:%p: !! FAILURE", __func__, __LINE__, name().c_str(), handle());
    }
#endif
}

AudioDecoderOMXIL::~AudioDecoderOMXIL()
{
}

bool AudioDecoderOMXIL::decode(const ISampleAttributes *pSampleAttr,
                                const unsigned char *auData, uint32_t auDataSize,
                                AudioESPlayerNative::DecodedAudioBuffer& decodedAudioBuffer)
{
    OMX_U32 auDataFlags = 0;
    llong PTS = INVALID_TIMESTAMP;
    if(!(mFrames++))
        auDataFlags |= OMX_BUFFERFLAG_STARTTIME;

    if(pSampleAttr) {
        PTS = pSampleAttr->getPTS();
        if(pSampleAttr->isKeyFrame()) {
            const MediaAttributes *attr = pSampleAttr->getMediaAttributes();
            if(!attr || !attr->mAudioAttributes)
                return refAppDeviceSpecific_MissingSampleAttributes;
            auDataFlags |= OMX_BUFFERFLAG_SYNCFRAME;
            if(!sendData(attr->mAudioAttributes->mCodecSpecificData, attr->mAudioAttributes->mCodecSpecificDataLen,
                         OMX_BUFFERFLAG_CODECCONFIG, convertTime(Time(0)), "codecSpecific"))
                return false;
        }
    }
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]:%s:%p: %d", __func__, __LINE__, name().c_str(), handle(), auDataSize);
#endif
    if(!sendData(auData, auDataSize, auDataFlags, convertTime(PTS), "auData") && 0)
        return false;
    decodedAudioBuffer.setDataBytes(auDataSize);
    decodedAudioBuffer.setTimestamp(pSampleAttr->getPTS());
    return true;
}

void AudioDecoderOMXIL::reset()
{
    Log::error(TRACE_LOG, "OMX::AudioDecoder RESET");
    sendData(0, 0, OMX_BUFFERFLAG_TIME_UNKNOWN | OMX_BUFFERFLAG_EOS, convertTime(Time(0)), "reset");
    flushInput();
    flushOutput();
#if defined(BUILD_OPENMAXIL_AUDIO_RENDERER)
    mRenderer.flushOutput();
    mRenderer.flushInput();
#endif
    mFrames = 0;
}

bool AudioDecoderOMXIL::updateAttributes(const AudioAttributes *attributes)
{
    NRDP_UNUSED(attributes);
    return true;
}
