/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ComponentOMXIL.h"

#include "../DeviceLibNative.h"


static int sInitOMXIL=0;

using namespace netflix::device;
using namespace netflix::device::esplayer;
using namespace netflix;

shared_ptr<ComponentOMXIL> ComponentOMXIL::sClock;

std::string ComponentOMXIL::OMXEvent::describe() const
{
    std::ostringstream result;
    switch(event) {
    case OMX_EventCmdComplete:
        switch(data1) {
        case OMX_CommandStateSet:
            switch ((int)data2) {
            case OMX_StateInvalid:
                result << "StateSet:OMX_StateInvalid";
                break;
            case OMX_StateLoaded:
                result << "StateSet:OMX_StateLoaded";
                break;
            case OMX_StateIdle:
                result << "StateSet:OMX_StateIdle";
                break;
            case OMX_StateExecuting:
                result << "StateSet:OMX_StateExecuting";
                break;
            case OMX_StatePause:
                result << "StateSet:OMX_StatePause";
                break;
            case OMX_StateWaitForResources:
                result << "StateSet:OMX_StateWaitForResources";
                break;
            default:
                result << "StateSet:OMX_Statexxxxx, state(" << (int)data2 << ")";
                break;
            }
            break;
        case OMX_CommandFlush:
            result << "OMX_CommandFlushed, port(" << (int)data2 << ")";
            break;
        case OMX_CommandPortDisable:
            result << "OMX_CommandPortDisabled, nData1(0x" << std::hex << data1 << std::dec << "), port(" << (int)data2 << ")";
            break;
        case OMX_CommandPortEnable:
            result << "OMX_CommandPortEnabled, nData1(0x" << std::hex << data1 << std::dec << "), port(" << (int)data2 << ")";
            break;
        case OMX_CommandMarkBuffer:
            result << "OMX_CommandMarkBuffer, nData1(0x" << std::hex << data1 << std::dec << "), port(" << (int)data2 << ")";
            break;
        }
        break;
    case OMX_EventBufferFlag:
        result << "OMX_EventBufferFlag(input)";
        break;
    case OMX_EventPortSettingsChanged:
        result << "OMX_EventPortSettingsChanged(output)";
        break;
    case OMX_EventMark:
        result << "OMX_EventMarked";
        break;
    case OMX_EventResourcesAcquired:
        result << "MX_EventResourcesAcquired";
        break;
    case OMX_EventError:
        switch((OMX_S32)data1) {
        case OMX_ErrorSameState:
            break;
        case OMX_ErrorInsufficientResources:
            result << "OMX_ErrorInsufficientResources, insufficient resources";
            break;
        case OMX_ErrorFormatNotDetected:
            result << "OMX_ErrorFormatNotDetected, cannot parse input stream";
            break;
        case OMX_ErrorPortUnpopulated:
            result << "OMX_ErrorPortUnpopulated port " << data2 << ", cannot parse input stream";
            break;
        case OMX_ErrorStreamCorrupt:
            result << "OMX_ErrorStreamCorrupt, Bitstream corrupt";
            break;
        default:
            result << "OMX_EventError detected, nData1(0x" << std::hex << data1 << std::dec << "), port(" << (int)data2 << ")";
            break;
        }
        break;
    default:
        result << "Unknown, nData1(0x" << std::hex << data1 << std::dec << "), port(" << (int)data2 << ")";
        break;
    }
    return result.str();
}

bool ComponentOMXIL::init(const std::string &name, OMX_INDEXTYPE index, OMX_CALLBACKTYPE *callbacks)
{
    assert(!name.empty());
    globalInit();
    cleanup();
    mName = name;

    if(!callbacks) {
        static OMX_CALLBACKTYPE builtinCallbacks;
        memset(&builtinCallbacks, 0, sizeof(builtinCallbacks));
        builtinCallbacks.EventHandler = ComponentOMXIL::omxil_EventHandler;
        builtinCallbacks.EmptyBufferDone = ComponentOMXIL::omxil_EmptyBufferDone;
        builtinCallbacks.FillBufferDone = ComponentOMXIL::omxil_FillBufferDone;
        callbacks = &builtinCallbacks;
    }
    OMX_VERIFY(OMX_GetHandle(&mHandle, (char*)name.c_str(), this, callbacks));

    OMX_PORT_PARAM_TYPE port;
    OMX_INIT_STRUCTURE(port);
    OMX_VERIFY(OMX_GetParameter(handle(), index, &port));
    mInput  = port.nStartPortNumber;
    mOutput = mInput + 1;
    if(mOutput > port.nStartPortNumber+port.nPorts-1)
        mOutput = port.nStartPortNumber+port.nPorts-1;

    char compName[128];
    OMX_UUIDTYPE uid;
    OMX_VERSIONTYPE compVersion, specVersion;
    OMX_VERIFY(OMX_GetComponentVersion(handle(), compName, &compVersion, &specVersion, &uid));
    Log::error(TRACE_LOG, "[%s:%d]:%s:%p: Created: [%s](0x%x/0x%x[%s]) [%d->%d]", __func__, __LINE__,
                              this->name().c_str(), handle(), compName, compVersion, specVersion, uid, mInput, mOutput);
    return disablePorts();
}

void ComponentOMXIL::cleanup()
{
    if(mHandle) {
        for(std::list<Connection>::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            disablePort(it->port);
            it->component->disablePort(it->componentPort);
            OMX_VERIFY(OMX_SetupTunnel(handle(), it->port, 0, 0));
            OMX_VERIFY(OMX_SetupTunnel(it->component->handle(), it->componentPort, 0, 0));
        }
        mConnections.clear();
        cleanupBuffers();
        OMX_VERIFY(OMX_FreeHandle(mHandle));
        mInput = mOutput = 0;
        mHandle = 0;
        globalCleanup();
    }
}

void ComponentOMXIL::cleanupOutputBuffer(int port)
{
    if(port == -1)
        port = mInput;
    if(mOutputBuffer) {
        OMX_VERIFY(OMX_FreeBuffer(handle(), mOutput, mOutputBuffer));
        disablePort(mOutput);
        mOutputBuffer = 0;
    }
}

void ComponentOMXIL::cleanupInputBuffers(int port)
{
    if(port == -1)
        port = mInput;
    if(!mInputBuffers.empty()) {
        ScopedMutex _lock(mBufferLock);
#if defined(OMX_VERBOSE)
        Log::error(TRACE_LOG, "[%s:%d]:%s:%p: DestroyingInput(%d): %d", __func__, __LINE__, name().c_str(), handle(), port, mInputBuffers.size());
#endif
        for(int i = 0; !mInputBuffers.empty(); ++i) {
            OMX_BUFFERHEADERTYPE *buffer = mInputBuffers.front();
            mInputBuffers.pop();
            const OMX_ERRORTYPE destroyed = OMX_FreeBuffer(handle(), port, buffer);
            if(destroyed == OMX_ErrorNone) {
#if defined(OMX_VERBOSE)
                Log::error(TRACE_LOG, "+ Destroyed(%d/%p)", i, buffer);
#endif
            } else {
                Log::error(TRACE_LOG, "+ !!!!!!! Failure(%d/0x%x)", i, destroyed);
            }
        }
        mPendingInputBuffers = mInputBuffers;
        disablePort(port);
    }
}


int ComponentOMXIL::initInputBuffers(int port)
{
    if(port == -1)
        port = mInput;
    if(handle()) {
        cleanupInputBuffers();

        OMX_PARAM_PORTDEFINITIONTYPE portFormat;
        OMX_INIT_STRUCTURE(portFormat);
        portFormat.nPortIndex = port;

        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portFormat));
        if(state() != OMX_StateIdle) {
             if(state() != OMX_StateLoaded)
                 setState(OMX_StateLoaded);
            if(setState(OMX_StateIdle))
                waitForCommandComplete(OMX_CommandStateSet, OMX_StateIdle);
        }
        enablePort(port);
#if defined(OMX_VERBOSE)
        Log::error(TRACE_LOG, "[%s:%d]:%s:%p: CreatingInput(%d): %d*%d", __func__, __LINE__, name().c_str(), handle(), port,
                    portFormat.nBufferSize, portFormat.nBufferCountActual);
#endif
        for(unsigned int i = 0; i < portFormat.nBufferCountActual; ++i) {
            OMX_BUFFERHEADERTYPE *buffer = NULL;
            const OMX_ERRORTYPE created = OMX_AllocateBuffer(handle(), &buffer, port, 0, portFormat.nBufferSize);
            if(created == OMX_ErrorNone) {
#if defined(OMX_VERBOSE)
                Log::error(TRACE_LOG, "+ Created(%d/%p)", i, buffer);
#endif
                mInputBuffers.push(buffer);
                mPendingInputBuffers.push(buffer);
            } else {
                Log::error(TRACE_LOG, "+ !!!!!!! Failure(%d/0x%x)", i, created);
            }
        }
        if(!mInputBuffers.empty())
            return mInputBuffers.size();
    }
    return 0;
}

void ComponentOMXIL::cleanupBuffers()
{
    cleanupOutputBuffer();
    cleanupInputBuffers();
}

void ComponentOMXIL::globalInit()
{
    if(!sInitOMXIL++) {
        Log::error(TRACE_LOG, "[%s:%d]: OpenMax-IL Start!", __func__, __LINE__);
        OMX_VERIFY(OMX_Init());
#if 1
        char component[1024];
        for(int i = 0; OMX_ComponentNameEnum(component, 1024, i) != OMX_ErrorNoMore; ++i)
            Log::error(TRACE_LOG, "COMPONENT[%d]: %s", i, component);
#endif
    }
}

void ComponentOMXIL::globalCleanup()
{
    --sInitOMXIL;
    if(!sInitOMXIL) {
        OMX_VERIFY(OMX_Deinit());
        Log::error(TRACE_LOG, "[%s:%d]: OpenMax-IL Shutdown!", __func__, __LINE__);
    } else if(sInitOMXIL == 1) {
        Log::error(TRACE_LOG, "[%s:%d]: Clock cleanup!", __func__, __LINE__);
        sClock.reset();
    }
}

bool ComponentOMXIL::receive(const Connection &connection)
{
    Log::error(TRACE_LOG, "[%s:%d]: Receive: %s:%p[%d]->%s:%p[%d]", __func__, __LINE__,
                              connection.component->name().c_str(), connection.component->handle(), connection.componentPort,
                              name().c_str(), handle(), connection.port);
    for(std::list<Connection>::const_iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
        if(it->port == connection.port)
            Log::error(TRACE_LOG, " + REUSED PORT");
    }
    mConnections.push_back(connection);
    if(connection.component->state() == OMX_StateLoaded)
        connection.component->setState(OMX_StateIdle);
    disablePort(connection.port);
    connection.component->disablePort(connection.componentPort);

    { //select port stream
        OMX_PARAM_U32TYPE param;
        OMX_INIT_STRUCTURE(param);
        param.nPortIndex = connection.componentPort;
        if(OMX_GetParameter(connection.component->handle(), OMX_IndexParamNumAvailableStreams, &param) == OMX_ErrorNone) {
            if(!param.nU32) // no streams available
                return false;
            param.nU32 = 0;
            OMX_VERIFY(OMX_SetParameter(connection.component->handle(), OMX_IndexParamActiveStream, &param));
        }
    }

    OMX_VERIFY(OMX_SetupTunnel(connection.component->handle(), connection.componentPort, handle(), connection.port));
    connection.component->enablePort(connection.componentPort);
    enablePort(connection.port);
    if(state() == OMX_StateLoaded) {
        //waitForEvent(OMX_EventPortSettingsChanged, port);
        if(setState(OMX_StateIdle)) {
            // waitForCommandComplete(OMX_CommandStateSet, OMX_StateIdle);
        }
        // waitForCommandComplete(OMX_CommandPortEnable, port);
        // component->waitForCommandComplete(OMX_CommandPortEnable, componentPort);
    }
    return true;
}

bool ComponentOMXIL::setState(OMX_STATETYPE state)
{
    if(handle()) {
        OMX_STATETYPE state_actual = OMX_StateMax;
        OMX_VERIFY(OMX_GetState(handle(), &state_actual));
        if(state != state_actual) {
            const OMX_ERRORTYPE stateSet = OMX_SendCommand(handle(), OMX_CommandStateSet, state, 0);
            if(stateSet == OMX_ErrorNone)
                return true;
            Log::error(TRACE_LOG, "[%s:%d]:%s:%p: Failure settting state(%d/0x%x)", __func__, __LINE__, name().c_str(), handle(), state, stateSet);
#if defined(OMX_VERBOSE)
        } else {
            Log::error(TRACE_LOG, "[%s:%d]:%s:%p: Already in state(%d)", __func__, __LINE__, name().c_str(), handle(), state);
#endif
        }

    }
    return false;
}

OMX_STATETYPE ComponentOMXIL::state() const
{
    OMX_STATETYPE state_actual = OMX_StateMax;
    if(handle())
        OMX_VERIFY(OMX_GetState(handle(), &state_actual));
    return state_actual;
}

bool ComponentOMXIL::disablePorts()
{
    if(handle()) {
        OMX_PORT_PARAM_TYPE ports;
        OMX_INIT_STRUCTURE(ports);

        OMX_ERRORTYPE omx_err = OMX_ErrorNone;
        const OMX_INDEXTYPE idxTypes[] = {
            OMX_IndexParamAudioInit,
            OMX_IndexParamImageInit,
            OMX_IndexParamVideoInit,
            OMX_IndexParamOtherInit
        };
        for(int i=0; i < 4; i++) {
            omx_err = OMX_GetParameter(handle(), idxTypes[i], &ports);
            if(omx_err == OMX_ErrorNone) {
                uint32_t j;
                for(j=0; j<ports.nPorts; j++) {
                    OMX_PARAM_PORTDEFINITIONTYPE portFormat;
                    OMX_INIT_STRUCTURE(portFormat);
                    portFormat.nPortIndex = ports.nStartPortNumber+j;

                    omx_err = OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portFormat);
                    if(omx_err != OMX_ErrorNone && portFormat.bEnabled == OMX_FALSE)
                        continue;

                    OMX_VERIFY(OMX_SendCommand(handle(), OMX_CommandPortDisable, ports.nStartPortNumber+j, NULL));
                }
            }
        }
        return true;
    }
    return false;
}

bool ComponentOMXIL::enablePort(unsigned int port)
{
    if(handle()) {
        OMX_PARAM_PORTDEFINITIONTYPE portFormat;
        OMX_INIT_STRUCTURE(portFormat);
        portFormat.nPortIndex = port;
        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portFormat));
        if(portFormat.bEnabled == OMX_FALSE) {
            OMX_VERIFY(OMX_SendCommand(handle(), OMX_CommandPortEnable, port, 0));
            return true;
        }
    }
    return false;
}

bool ComponentOMXIL::disablePort(unsigned int port)
{
    if(handle()) {
        OMX_PARAM_PORTDEFINITIONTYPE portFormat;
        OMX_INIT_STRUCTURE(portFormat);
        portFormat.nPortIndex = port;
        OMX_VERIFY(OMX_GetParameter(handle(), OMX_IndexParamPortDefinition, &portFormat));
        if(portFormat.bEnabled == OMX_TRUE) {
            OMX_VERIFY(OMX_SendCommand(handle(), OMX_CommandPortDisable, port, 0));
            return true;
        }
    }
    return false;
}

bool ComponentOMXIL::flush(int port)
{
    if(handle()) {
        OMX_VERIFY(OMX_SendCommand(handle(), OMX_CommandFlush, port, 0));
        waitForCommandComplete(OMX_CommandFlush, port);
        return true;
    }
    return false;
}

OMX_BUFFERHEADERTYPE *ComponentOMXIL::inputBuffer(Time wait)
{
    ScopedMutex _lock(mBufferLock);
    while(true) {
        if(!mPendingInputBuffers.empty()) {
            OMX_BUFFERHEADERTYPE *buffer = mPendingInputBuffers.front();
            mPendingInputBuffers.pop();
            return buffer;
        }
        if(mInputBufferCondition.wait(&mBufferLock, &wait) == NFErr_TimedOut)
            break;
    }
    return 0;
}

bool ComponentOMXIL::sendData(const unsigned char *data, uint32_t dataSize, OMX_U32 flags, OMX_TICKS timeStamp, const char *desc)
{
    NRDP_UNUSED(desc);
    do {
        for(int i = 0; i < 3; ++i) {
            if(OMX_BUFFERHEADERTYPE *buffer = inputBuffer(Time(1000))) {
#if defined(OMX_VERBOSE)
                Log::error(TRACE_LOG, "%s:%p: + %s %p [%d/%d]", name().c_str(), handle(), desc, buffer, buffer->nAllocLen, dataSize);
#endif
                buffer->nFlags = flags;
                buffer->nOffset = 0;
                buffer->nTimeStamp = timeStamp;
                buffer->nFilledLen = dataSize > buffer->nAllocLen ? buffer->nAllocLen : dataSize;
                if(data) {
                    memcpy(buffer->pBuffer, data, buffer->nFilledLen);
                    data += buffer->nFilledLen;
                    dataSize -= buffer->nFilledLen;
                    if(!dataSize)
                        buffer->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
                }
                emptyBuffer(buffer);
            } else if(i == 2) {
                Log::error(TRACE_LOG, "[%s:%d]:%s:%p: !!!!!!!! %d ", __func__, __LINE__, name().c_str(), handle(), dataSize);
                return false;
            } else {
                Log::error(TRACE_LOG, "[%s:%d]:%s:%p: !!!!!!!! %d Try again %d ", __func__, __LINE__, name().c_str(), handle(), dataSize, i);
                //flushInput();
            }
        }
    } while(dataSize);
    return true;
}

bool ComponentOMXIL::emptyBuffer(OMX_BUFFERHEADERTYPE *buffer)
{
    if(handle()) {
        assert(!mInputBuffers.empty());
        const OMX_ERRORTYPE emptied = OMX_EmptyThisBuffer(handle(), buffer);
        assert(buffer->nAllocLen >= buffer->nFilledLen);
        if(emptied == OMX_ErrorNone)
            return true;
        Log::error(TRACE_LOG, "[%s:%d]:%s:%p: !!!!! FAILURE 0x%x", __func__, __LINE__, name().c_str(), handle(), emptied);
    }
    return false;
}

bool ComponentOMXIL::fillBuffer(OMX_BUFFERHEADERTYPE *buffer)
{
    if(handle()) {
        {
            ScopedMutex _lock(mBufferLock);
            mOutputBuffer = buffer;
        }
        const OMX_ERRORTYPE filled = OMX_FillThisBuffer(handle(), buffer);
        if(filled == OMX_ErrorNone)
            return true;
        Log::error(TRACE_LOG, "[%s:%d]:%s:%p: !!!!! FAILURE 0x%x", __func__, __LINE__, name().c_str(), handle(), filled);
    }
    return false;
}

void ComponentOMXIL::emptiedBuffer(OMX_BUFFERHEADERTYPE *buffer)
{
    ScopedMutex _lock(mBufferLock);
    mPendingInputBuffers.push(buffer);
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]:%s:%p: Emptied(%p) [%d]", __func__, __LINE__, name().c_str(), handle(), buffer, mPendingInputBuffers.size());
#endif
    mInputBufferCondition.broadcast();
}

OMX_ERRORTYPE ComponentOMXIL::omxil_EventHandler(OMX_HANDLETYPE, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
    ComponentOMXIL *component = (ComponentOMXIL*)pAppData;
    const OMXEvent event(eEvent, nData1, nData2, pEventData);
#if defined(OMX_VERBOSE) || 1
    Log::error(TRACE_LOG, "[%s:%d]:%s:%p: %s", __func__, __LINE__, component->name().c_str(), component->handle(), event.describe().c_str());
#endif
    return component->eventHandler(event);
}

OMX_ERRORTYPE ComponentOMXIL::eventHandler(OMXEvent event)
{
    {
        ScopedMutex _lock(mEventLock);
        mEvents.push_back(event);
        mEventCondition.broadcast();
    }
    return OMX_ErrorNone;
}

bool ComponentOMXIL::waitForEvent(OMX_EVENTTYPE event, OMX_U32 nData1, Time wait)
{
    bool result = false;
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]: Waiting for: %d(%d) [%lld]", __func__, __LINE__, event, nData1, wait.ms());
#endif
    {
        ScopedMutex _lock(mEventLock);
        while(true) {
            if(!mEvents.empty()) {
                for(std::list<OMXEvent>::iterator it = mEvents.begin(); it != mEvents.end(); ++it) {
#if defined(OMX_VERBOSE)
                    Log::error(TRACE_LOG, "[%s:%d]: Looking at: %s", __func__, __LINE__, it->describe().c_str());
#endif
                    if(it->event == event && it->data1 == nData1) {
                        mEvents.erase(it);
                        result = true;
                        break;
                    }
                }
                if(result)
                    break;
            }
            if(mEventCondition.wait(&mEventLock, wait) == NFErr_TimedOut)
                break;
        }
    }
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]: ~Waiting for: %d(%d) [%lld]", __func__, __LINE__, event, nData1, wait.ms());
#endif
    return result;
}

bool ComponentOMXIL::waitForEvent(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, Time wait)
{
    bool result = false;
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]: Waiting for: %d(%d::%d) [%lld]", __func__, __LINE__, event, nData1, nData2, wait.ms());
#endif
    {
        ScopedMutex _lock(mEventLock);
        while(true) {
            if(!mEvents.empty()) {
                for(std::list<OMXEvent>::iterator it = mEvents.begin(); it != mEvents.end(); ++it) {
#if defined(OMX_VERBOSE)
                    Log::error(TRACE_LOG, "[%s:%d]: Looking at: %s", __func__, __LINE__, it->describe().c_str());
#endif
                    if(it->event == event && it->data1 == nData1 && it->data2 == nData2) {
                        mEvents.erase(it);
                        result = true;
                        break;
                    }
                }
                if(result)
                    break;
            }
            if(mEventCondition.wait(&mEventLock, wait) == NFErr_TimedOut)
                break;
        }
    }
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]: ~Waiting for: %d(%d::%d) [%lld]", __func__, __LINE__, event, nData1, nData2, wait.ms());
#endif
    return result;
}

bool ComponentOMXIL::waitForCommandComplete(OMX_COMMANDTYPE command, OMX_U32 nData, Time wait)
{
    bool result = false;
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]: Waiting for: %d(%d) [%lld]", __func__, __LINE__, command, nData, wait.ms());
#endif
    {
        ScopedMutex _lock(mEventLock);
        while(true) {
            if(!mEvents.empty()) {
                for(std::list<OMXEvent>::iterator it = mEvents.begin(); it != mEvents.end(); ++it) {
#if defined(OMX_VERBOSE)
                    Log::error(TRACE_LOG, "[%s:%d]: Looking at: %s [%d:%d:%d]", __func__, __LINE__, it->describe().c_str(),
                        it->event, it->data1, it->data2);
#endif
                    if(it->event == OMX_EventCmdComplete && it->data1 == command && it->data2 == nData) {
                        mEvents.erase(it);
                        result = true;
                        break;
                    }
                }
                if(result)
                    break;
            }
            if(mEventCondition.wait(&mEventLock, wait) == NFErr_TimedOut)
                break;
        }
    }
#if defined(OMX_VERBOSE)
    Log::error(TRACE_LOG, "[%s:%d]: ~Waiting for: %d(%d) [%lld]", __func__, __LINE__, command, nData, wait.ms());
#endif
    return result;
}

void ComponentOMXIL::filledBuffer(OMX_BUFFERHEADERTYPE *buffer)
{
    if(handle()) {
        ScopedMutex _lock(mBufferLock);
        if(buffer != mOutputBuffer) {
            OMX_VERIFY(OMX_FreeBuffer(handle(), mOutput, buffer));
            buffer = 0;
        }
    }
    if(buffer)
        fillBuffer(buffer);
}

OMX_TICKS ComponentOMXIL::convertTime(const Time &time)
{
#ifndef OMX_SKIP64BIT
    return ticks.ms() * 1000;
#else
    OMX_TICKS result;
    const uint64_t micro_s = time.ms() * 1000;
    result.nHighPart = (micro_s >> 32) & 0xFFFFFFFF;
    result.nLowPart = (micro_s)        & 0xFFFFFFFF;
    return result;
#endif
}

Time ComponentOMXIL::convertTime(const OMX_TICKS &ticks)
{
#ifndef OMX_SKIP64BIT
    return Time(ticks / 1000);
#else
    return Time(((int64_t(ticks.nHighPart) << 32) | ticks.nLowPart) / 1000);
#endif

}

Time ComponentOMXIL::getClockTime()
{
    if(sClock) {
        OMX_TIME_CONFIG_TIMESTAMPTYPE pts;
        OMX_INIT_STRUCTURE(pts);
        pts.nPortIndex = sClock->outputPort();
        OMX_VERIFY(OMX_GetConfig(sClock->handle(), OMX_IndexConfigTimeCurrentMediaTime, &pts));
        return convertTime(pts.nTimestamp);
    }
    return Time(0);
}

shared_ptr<ComponentOMXIL> ComponentOMXIL::getClock()
{
    if(!sClock && !sConfiguration->omxilClock.empty()) {
        sClock.reset(new ComponentOMXIL(sConfiguration->omxilClock, OMX_IndexParamOtherInit));
        {
            OMX_TIME_CONFIG_CLOCKSTATETYPE clockState;
            OMX_INIT_STRUCTURE(clockState);
#if 0
            clockState.eState = OMX_TIME_ClockStateRunning;
            clockState.nStartTime = convertTime(Time::mono());
#else
            clockState.eState = OMX_TIME_ClockStateWaitingForStartTime;
            clockState.nWaitMask = 1;
#endif
            OMX_VERIFY(OMX_SetParameter(sClock->handle(), OMX_IndexConfigTimeClockState, &clockState));
        }
        //sClock->setState(OMX_StateExecuting);
    }
    return sClock;
}

void ComponentOMXIL::startClockTime(const Time &time)
{
    if(shared_ptr<ComponentOMXIL> clock = getClock()) {
        clock->setState(OMX_StateIdle);
        OMX_TIME_CONFIG_CLOCKSTATETYPE clockState;
        OMX_INIT_STRUCTURE(clockState);
        clockState.eState = OMX_TIME_ClockStateRunning;
        clockState.nStartTime = convertTime(time);
        clockState.nWaitMask = 1;
        OMX_VERIFY(OMX_SetParameter(clock->handle(), OMX_IndexConfigTimeClockState, &clockState));
        clock->setState(OMX_StateExecuting);
    }
}
