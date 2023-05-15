
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "../VideoESPlayerNative.h"
#include "../MediaUtils.h"
#include "../ESPlayerConstants.h"
#include "../VideoRenderer.h"
#include "../ReferenceClockNative.h"

#include "VideoDecoderOMXIL.h"
#include <OMX_Broadcom.h>

#include <nrdbase/Log.h>
#include <cstring>

#include <nrdbase/ScopedMutex.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"
#include "../DeviceLibNative.h"

#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
#if 1
#   define OMX_OPENGL
# endif
#ifdef OMX_OPENGL
extern void *createEGLVideoImage(unsigned width, unsigned height);
# endif
#endif

using namespace netflix::device;
using namespace netflix::device::esplayer;
using namespace netflix;

VideoDecoderOMXIL::VideoDecoderOMXIL(const VideoAttributes&, ReferenceClockNative& referenceClock,
                                     const VideoRenderer::VideoRendererInfo& rendererInfo, bool&)
    : VideoDecoder(rendererInfo),
      ComponentOMXIL(sConfiguration->omxilVideoDecoder, OMX_IndexParamVideoInit),
      mFrames(0), mDroppedFrames(0), mDropFrames(false), mFrameWidth(0), mFrameHeight(0), mReferenceClock(referenceClock)
{
#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
    if(!sConfiguration->omxilVideoScheduler.empty()) {
        mScheduler.init(sConfiguration->omxilVideoScheduler, OMX_IndexParamVideoInit);
        if(shared_ptr<ComponentOMXIL> clock = getClock())
            mScheduler.receive(clock.get(), clock->inputPort()+1, mScheduler.outputPort()+1);
    }

    setState(OMX_StateIdle);
    {
        OMX_VIDEO_PARAM_PORTFORMATTYPE format;
        OMX_INIT_STRUCTURE(format);
        format.nPortIndex = inputPort();
        format.eCompressionFormat = OMX_VIDEO_CodingAVC;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamVideoPortFormat, &format));
    }
    {
        OMX_PARAM_PORTDEFINITIONTYPE portParam;
        OMX_INIT_STRUCTURE(portParam);
        portParam.nPortIndex = inputPort();
        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portParam));
        portParam.nPortIndex = inputPort();
        portParam.nBufferCountActual = 20;
        OMX_VERIFY(OMX_SetParameter(handle(), OMX_IndexParamPortDefinition, &portParam));
    }
    initInputBuffers();
    if(setState(OMX_StateExecuting))
        waitForCommandComplete(OMX_CommandStateSet, OMX_StateExecuting);

    if(mScheduler.isValid()) {
        mScheduler.receive(this);
        mScheduler.setState(OMX_StateExecuting);
    }

#if defined(OMX_OPENGL)
    if(!sConfiguration->omxilVideoGLRenderer.empty()) {
        static OMX_CALLBACKTYPE glCallbacks;
        memset(&glCallbacks, 0, sizeof(&glCallbacks));
        glCallbacks.EventHandler = VideoDecoderOMXIL::omxil_glRendererEventHandler;
        glCallbacks.EmptyBufferDone = ComponentOMXIL::omxil_EmptyBufferDone;
        glCallbacks.FillBufferDone = ComponentOMXIL::omxil_FillBufferDone;
        mRenderer.init(sConfiguration->omxilVideoGLRenderer, OMX_IndexParamVideoInit, &glCallbacks);
        mRenderer.receive(mScheduler.isValid() ? &mScheduler : this);
    } else
#endif
    {
        mRenderer.init(sConfiguration->omxilVideoRenderer, OMX_IndexParamVideoInit);
        mRenderer.setState(OMX_StateIdle);
        {
            OMX_CONFIG_DISPLAYREGIONTYPE region;
            OMX_INIT_STRUCTURE(region);
            region.nPortIndex = mRenderer.inputPort();
            region.set = OMX_DISPLAY_SET_LAYER;
            region.layer = 0;
            OMX_VERIFY(OMX_SetParameter(mRenderer.handle(), OMX_IndexConfigDisplayRegion, &region));
        }
        mRenderer.receive(mScheduler.isValid() ? &mScheduler : this);
        mRenderer.setState(OMX_StateExecuting);
    }
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

VideoDecoderOMXIL::~VideoDecoderOMXIL()
{
}

RefAppDeviceSpecificError VideoDecoderOMXIL::decode(const ISampleAttributes *pSampleAttr,
                                                    const unsigned char* auData, uint32_t auDataSize,
                                                    VideoRenderer::VideoFrameBuffer &frameBuffer,
                                                    bool &gotPicture, bool &resend, Format3D &format3D)
{
    format3D = NOT_3D;
    gotPicture = false;
    resend = false;

    int dropFrame = 0;
    OMX_U32 auDataFlags = 0;
    llong PTS = INVALID_TIMESTAMP;
    const int64_t currPresentationTime = mReferenceClock.getCurrPresentationTime();
    if(pSampleAttr) {
        PTS = pSampleAttr->getPTS();
        if(pSampleAttr->getMediaAttributes()) {
            mFrameWidth = pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageWidth;
            mFrameHeight = pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageHeight;
        }
        if(pSampleAttr->isKeyFrame()) {
            const MediaAttributes *attr = pSampleAttr->getMediaAttributes();
            if(!attr || !attr->mVideoAttributes)
                return refAppDeviceSpecific_MissingSampleAttributes;
            auDataFlags |= OMX_BUFFERFLAG_SYNCFRAME;
            for(uint32_t i = 0; i < attr->mVideoAttributes->mCodecSpecificDataLen.size(); i++){
                if(!sendData(attr->mVideoAttributes->mCodecSpecificData[i], attr->mVideoAttributes->mCodecSpecificDataLen[i],
                             OMX_BUFFERFLAG_CODECCONFIG, convertTime(Time(0)), "codecSpecific"))
                   return refAppDeviceSpecific_WrongBitDepth;
            }
            if(!pSampleAttr->isDependedOn() && checkIfShouldDropFrame(PTS, currPresentationTime))
                dropFrame = __LINE__;
        }
        if(checkIfShouldDropUntilNextGOP(pSampleAttr, currPresentationTime))
            dropFrame = __LINE__;
    }
    if(!(mFrames++)) {
        auDataFlags |= OMX_BUFFERFLAG_STARTTIME;
        Log::error(TRACE_LOG, "[%s:%d]: Start: %lld (%lld)", __FILE__, __LINE__, Time::mono().ms(), PTS);
        ComponentOMXIL::startClockTime(PTS);
    }

#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]:%s:%p: %d [%dx%d]", __func__, __LINE__, name().c_str(), handle(), auDataSize,
                              (pSampleAttr && pSampleAttr->getMediaAttributes()) ? pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageWidth : -1,
                              (pSampleAttr && pSampleAttr->getMediaAttributes()) ? pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageHeight : -1);
#endif
    RefAppDeviceSpecificError result = refAppDeviceSpecific_NoError;

#if 1
    const bool debugFrame = dropFrame;
#else
    const bool debugFrame = true;
#endif
    if(debugFrame) {
        static llong sLastPTS = 0;
        static Time sLastClockTime;
        const Time clockTime = getClockTime();
        Log::warn(TRACE_LOG, " !!!!! %s(%d:%d) [%lld:%lld] %lldms%s(%lldms::[%lld:%lld])", dropFrame ? "DROP" : "DATA",
                                  mFrames, mDropFrames, currPresentationTime, PTS, currPresentationTime-PTS,
                                  (pSampleAttr && pSampleAttr->isKeyFrame()) ? "*" : "",
                                 PTS-clockTime.ms(), clockTime.ms()-sLastClockTime.ms(), PTS-sLastPTS);
        sLastClockTime = clockTime;
        sLastPTS = PTS;
    }

    if(dropFrame) {
        ++mDroppedFrames;
    } else {
        if(!sendData(auData, auDataSize, auDataFlags, convertTime(PTS), "auData"))
            result = refAppDeviceSpecific_WrongBitDepth;
        gotPicture = true;

#if 0
        const Time clockPTS = getClockTime();
        enqueueTimestamp(clockPTS.ms());
        frameBuffer.setTimestamp(dequeueTimestamp());
#else
        enqueueTimestamp(PTS);
        frameBuffer.setTimestamp(dequeueTimestamp());
#endif
        if(mDroppedFrames) {
            Log::error(TRACE_LOG, "[%s:%d]: RESUME(%d) %lldms", __func__, __LINE__, mDroppedFrames, currPresentationTime-PTS);
            mDroppedFrames = 0;
        }
    }
    return result;
}

void VideoDecoderOMXIL::reset()
{
    Log::error(TRACE_LOG, "OMX::VideoDecoder RESET");
    sendData(0, 0, OMX_BUFFERFLAG_TIME_UNKNOWN | OMX_BUFFERFLAG_EOS, convertTime(Time(0)), "videodecoder::reset");
    flushInput();
    flushOutput();
#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
    if(mScheduler.isValid()) {
        mScheduler.flushOutput();
        mScheduler.flushInput();
    } else {
        mRenderer.flushOutput();
        mRenderer.flushInput();
    }
#endif
    resetTimestampQueue();
    mDropFrames = false;
    mFrames = mDroppedFrames = 0;
}

OMX_ERRORTYPE VideoDecoderOMXIL::omxil_glRendererEventHandler(OMX_HANDLETYPE nType, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
#if defined(OMX_OPENGL)
    if(eEvent == OMX_EventPortSettingsChanged) {
        ComponentOMXIL *component = (ComponentOMXIL*)pAppData;
        OMX_PARAM_PORTDEFINITIONTYPE portParam;
        OMX_INIT_STRUCTURE(portParam);
        portParam.nPortIndex = component->outputPort();
        OMX_VERIFY(OMX_GetConfig(component->handle(), OMX_IndexParamPortDefinition, &portParam));
        assert(portParam.eDomain == OMX_PortDomainVideo);
        static unsigned sGLWidth = 0, sGLHeight = 0;
        if(portParam.format.video.nFrameWidth != sGLWidth || portParam.format.video.nFrameHeight != sGLHeight) {
            sGLWidth = portParam.format.video.nFrameWidth;
            sGLHeight = portParam.format.video.nFrameHeight;
            Log::error(TRACE_LOG, "[%s:%d]: EGLRenderer Switch: %dx%d", __func__, __LINE__, sGLWidth, sGLHeight);
            if(void *glImage = createEGLVideoImage(sGLWidth, sGLHeight)) {
                Log::error(TRACE_LOG, " + %p", glImage);
                OMX_VERIFY(OMX_SendCommand(component->handle(), OMX_CommandPortDisable, component->outputPort(), 0));
                component->cleanupOutputBuffer();
                component->waitForCommandComplete(OMX_CommandPortDisable, component->outputPort());
                component->setState(OMX_StateIdle);
                OMX_VERIFY(OMX_SendCommand(component->handle(), OMX_CommandPortEnable, component->outputPort(), 0));
                OMX_BUFFERHEADERTYPE *buffer = NULL;
                OMX_VERIFY(OMX_UseEGLImage(component->handle(), &buffer, component->outputPort(), NULL, glImage));
                component->setState(OMX_StateExecuting);
                component->waitForCommandComplete(OMX_CommandPortEnable, component->outputPort());
                component->fillBuffer(buffer);
            }
        }
    }
#endif
    return ComponentOMXIL::omxil_EventHandler(nType, pAppData, eEvent, nData1, nData2, pEventData);
}

OMX_ERRORTYPE VideoDecoderOMXIL::eventHandler(OMXEvent event)
{
    if(event.event == OMX_EventPortSettingsChanged) {
        ScopedMutex _lock(bufferLock());
        OMX_PARAM_PORTDEFINITIONTYPE portParam;
        OMX_INIT_STRUCTURE(portParam);
        portParam.nPortIndex = outputPort();
        OMX_VERIFY(OMX_GetConfig(handle(), OMX_IndexParamPortDefinition, &portParam));
        assert(portParam.eDomain == OMX_PortDomainVideo);
        Log::error(TRACE_LOG, "[%s:%d]: Decoder Switch: %dx%d (%dx%d)", __func__, __LINE__, mFrameWidth, mFrameHeight,
                                  portParam.format.video.nFrameWidth, portParam.format.video.nFrameHeight);
        disablePort(outputPort());
        mRenderer.disablePort(mRenderer.inputPort());
        portParam.nPortIndex = mRenderer.inputPort();
        OMX_VERIFY(OMX_SetConfig(mRenderer.handle(), OMX_IndexParamPortDefinition, &portParam));
        portParam.nPortIndex = mRenderer.outputPort();
        OMX_VERIFY(OMX_SetConfig(mRenderer.handle(), OMX_IndexParamPortDefinition, &portParam));
        if(mScheduler.isValid()) {
            portParam.nPortIndex = mScheduler.inputPort();
            OMX_VERIFY(OMX_SetConfig(mScheduler.handle(), OMX_IndexParamPortDefinition, &portParam));
            portParam.nPortIndex = mScheduler.outputPort();
            OMX_VERIFY(OMX_SetConfig(mScheduler.handle(), OMX_IndexParamPortDefinition, &portParam));
        }
        enablePort(outputPort());
        mRenderer.enablePort(mRenderer.inputPort());
    }
    return ComponentOMXIL::eventHandler(event);
}

bool VideoDecoderOMXIL::checkIfShouldDropUntilNextGOP(const ISampleAttributes *pSampleAttr, const int64_t &currPresentationTime) const
{
    if(pSampleAttr->isKeyFrame())
        mDropFrames = false;
    if(currPresentationTime != INVALID_TIMESTAMP && currPresentationTime > pSampleAttr->getPTS() + ESPlayerConstants::VIDEO_DROP_THRESHOLD_WHOLE_GOP)
        mDropFrames = true;
    return mDropFrames;
}

bool VideoDecoderOMXIL::checkIfShouldDropFrame(const int64_t &PTS, const int64_t &currPresentationTime ) const
{
    if(PTS != INVALID_TIMESTAMP && currPresentationTime != INVALID_TIMESTAMP &&
       currPresentationTime > PTS + ESPlayerConstants::VIDEO_DROP_THRESHOLD_DEC &&
       mDroppedFrames < ESPlayerConstants::MAX_CONSECUTIVE_VIDEO_DROP_DEC)
        return true;
    return false;
}
