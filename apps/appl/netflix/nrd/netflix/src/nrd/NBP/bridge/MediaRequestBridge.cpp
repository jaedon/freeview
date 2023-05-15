/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaRequestBridge.h"

#include <string>
#include <map>

#include <nrdbase/ASError.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/tr1.h>

#include "BufferedMediaRequest.h"
#include "AppendableMediaRequest.h"
#include "LocalConfigParameter.h"
#include "McMediaBufferPool.h"
#include "NrdApplication.h"
#include "NrdpMediaSource.h"
#include "NrdpMediaSourceBuffer.h"
#include "NrdpMedia.h"
#include "StreamingConfigParameter.h"

#include <nrdase/HttpRequestManager.h>
#include <nrdase/IMediaConsumer.h>
#include <nrdase/AseDataBuffer.h>

#include <nrd/Request.h>

#include <nrdnet/AseUrl.h>
#include <nrdnet/HttpResponseHeaders.h>

#include "TransportReporter.h"
#include "StreamingPersistentStore.h"
#include "MediaSourcePlayerFactoryBridge.h"

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;


////////////////////////////////////////////////////////////
// TransportReporter
////////////////////////////////////////////////////////////
class MediaRequestBridge::TransportReporter : public netflix::TransportReporter
{
public:
    TransportReporter(const shared_ptr<MediaRequestBridge> &bridge) :
        mBridge(bridge)
    {
    }

    virtual ~TransportReporter() {}

    virtual void sendEvent( const char *fn, const Variant &args )
    {
        if (shared_ptr<MediaRequestBridge> bridge = mBridge.lock()) {
            bridge->TransportReporterEvent( fn, args );
        }
    }

private:
    weak_ptr<MediaRequestBridge> mBridge;

};

//////////
//
// ProgressEvent
//
//////////
class MediaRequestBridge::ProgressEvent
{
public:
    ProgressEvent() :
        m_time(),
        m_elapsedTimeInMs(0),
        m_bytes(0)
    {
    }

    void update( const AseTimeVal &time, uint32_t elapsedTimeInMs, uint32_t bytes, uint32_t httpResponseTime )
    {
        m_time = time;
        m_elapsedTimeInMs = elapsedTimeInMs;
        m_bytes = bytes;
        if ( httpResponseTime ) m_responseTimes.push_back( httpResponseTime );
    }


    AseTimeVal m_time;
    uint32_t m_elapsedTimeInMs;
    uint32_t m_bytes;
    std::vector<uint32_t> m_responseTimes;
};

////////////////////////////////////////////////////////////
//
// MediaRequestBridge
//
////////////////////////////////////////////////////////////

//////////
//
// methods
//
//////////
#define MediaRequestBridgeMethodList(Method)    \
    Method(downloadTrackPause)                  \
    Method(downloadTrackReconfigure)            \
    Method(downloadTrackResume)                 \
    Method(downloadTracksCreate)                \
    Method(downloadTracksDestroy)               \
    Method(endOfStream)                         \
    Method(mediaBufferPoolCreate)               \
    Method(mediaSourceCreate)                   \
    Method(mediaSourceDestroy)                  \
    Method(parseSegmentIndex)                   \
    Method(requestAbort)                        \
    Method(requestCreate)                       \
    Method(requestRemove)                       \
    Method(requestSwapURL)                      \
    Method(sourceBufferAttachData)              \
    Method(sourceBufferAttachRequest)           \
    Method(sourceBufferSetTimestampOffset)      \
    Method(sourceBuffersCreate)                 \
    Method(sourceBuffersFlush)

MethodTable(MediaRequestBridge, MediaRequestBridgeMethodList);

//////////
//
// MediaRequestBridge::MediaRequestListener
//
//////////
class MediaRequestBridge::MediaRequestListener : public mediacontrol::IJsBridgeMediaRequestListener, public mediacontrol::MediaRequestManagerListener
{
public:
    MediaRequestListener(MediaRequestBridge *bridge) :
            mBridge(bridge)
    {
    }

    virtual ~MediaRequestListener() {}

    virtual void onProgress(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs, uint32_t bytesReceived, uint32_t httpResponseTime)
    {
        if (mBridge)
            mBridge->onProgress(requestId, time, elapsedTimeInMs, bytesReceived, httpResponseTime );
    }

    virtual void onError(uint32_t requestId, const AseTimeVal &time, AseErrorCode errCode, int32_t httpStatusCode )
    {
        if (mBridge)
            mBridge->onError(requestId, time, errCode, httpStatusCode);
    }

    virtual void onComplete(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs)
    {
        if (mBridge)
            mBridge->onComplete(requestId, time, elapsedTimeInMs);
    }

    virtual void onComplete(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs, DataBuffer const& data)
    {
        if (mBridge)
            mBridge->onComplete(requestId, time, elapsedTimeInMs, &data);
    }

    virtual void onLoadStart(uint32_t requestId, const AseTimeVal &time)
    {
        if (mBridge)
            mBridge->onLoadStart(requestId, time);
    }

    virtual void onFirstByteReceived(uint32_t requestId, const AseTimeVal &time, const AseTimeVal &startTime, HttpResponseHeaders headers, bool newConnection)
    {
        if (mBridge)
            mBridge->onFirstByteReceived(requestId, time, startTime, headers, newConnection);
    }

    virtual void onDownloadPaused(const AseTimeVal &time)
    {
        if (mBridge)
            mBridge->downloadPaused(time);
    }

    virtual void onDownloadResumed(const AseTimeVal &time)
    {
        if (mBridge)
            mBridge->downloadResumed(time);
    }

    virtual void onPipeliningDisabled(string const& reason)
    {
        if (mBridge)
            mBridge->onPipeliningDisabled(reason);
    }

    virtual void onRequestScheduled(uint32_t requestId)
    {
        if (mBridge)
            mBridge->onRequestScheduled(requestId);
    }

    virtual void onNetworkFailing(  uint32_t                downloadTrackId,
                                    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived )
    {
        if (mBridge)
            mBridge->onNetworkFailing( downloadTrackId, latestStatus, destination, pLatestErrorTime, latestDataReceived );
    }

private:
    MediaRequestBridge *mBridge;
};

class ArrayBufferAppendable : public MediaSourceAppendable
{
public:
    ArrayBufferAppendable( DataBuffer const& data )
    {
        (void) AseDataBuffer::construct( data, mArrayBufferData );
    }

    ~ArrayBufferAppendable() {}

public:

    virtual bool forwardBlocks()
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE,"Forwarding request for ArrayBufferAppendable, size %zu",
            mArrayBufferData ? mArrayBufferData->getFilledSpaceSize() : 0 );

        forwardBlock( mArrayBufferData );

        mArrayBufferData.reset();

        return true;
    }

protected:
    AseDataBufferPtr    mArrayBufferData;
};


//////////
//
// MediaRequestBridge
//
//////////
MediaRequestBridge::MediaRequestBridge(NrdpMediaSourceManagerPtr mediaSourceManager,
                                       shared_ptr<MediaSourcePlayerFactoryBridge> pf)
    : NfObject("mediarequest", NULL, 0,
               Methods::MediaRequestBridge::methods,
               Methods::MediaRequestBridge::MethodCount),
      m_playerFactory(pf),
      m_httpStartMutex(MEDIAREQUESTBRIDGESTART_MUTEX, "MediaRequestBridgeStart"),
      m_mediaRequestsMutex(MEDIAREQUESTBRIDGE_MUTEX, "MediaRequestBridge"),
      m_mediaBufferPoolMutex(MEDIAREQUESTBRIDGE_POOL_MUTEX, "MediaRequestBridge_Pool"),
      m_httpStarted(false),
      m_downloadPausePending(false),
      m_scheduledMediaRequests( 0 ),
      m_lastProgressEventSentTime(AseTimeVal::now()),
      m_progressReportInterval(500),
      m_mediaSourceManager(mediaSourceManager)
{
    mListener.reset(new MediaRequestListener(this));
}

MediaRequestBridge::~MediaRequestBridge()
{
}

//////////
//
// init
//
//////////
void MediaRequestBridge::init()
{
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "MediaRequestBridge::init()");

    m_streamingConfigParameter.reset(new StreamingConfigParameter);
    m_localConfigParameter.reset(new LocalConfigParameter);

    m_transportReporter.reset(new MediaRequestBridge::TransportReporter(static_pointer_cast<MediaRequestBridge>( shared_from_this())));

    // create the media consumer
    m_mediaRequestManager.reset(new MediaRequestManager(IPersistentStorePtr(new StreamingPersistentStore)));
    m_mediaRequestManager->init(mListener,m_transportReporter);
}

void MediaRequestBridge::deinit()
{
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "MediaRequestBridge::deinit()");
    stopHttpManager();

    // below shared pointer should be reset since it's destroyer is using buffer
    // pool
    m_mediaRequests.clear();
    m_mediaSourceManager.reset();
    m_mediaRequestManager.reset();

    returnBufferPool();
}

//////////
//
// invoke
//
//////////
Variant MediaRequestBridge::invoke(int method, const Variant &args)
{
    Variant result;

    // start http requests if we haven't already
    startHttpManager();

    // make sure we pass back the index of the caller
    const std::string index = args.mapValue<std::string>("idx");
    result["idx"] = index;
    result["success"] = false; // assume failure for now

    switch (method)
    {
#define FORWARD_METHOD(METHODNAME, ...)                \
        case Methods::MediaRequestBridge::METHODNAME:  \
            return METHODNAME(result, method, args);

        // Forward all the methods to the same named function
        MediaRequestBridgeMethodList(FORWARD_METHOD);
    }

    // should never get here, but pass to default handler
    return NfObject::invoke(method, args);
}

unsigned MediaRequestBridge::requestFlags(const Request &request) const
{
    unsigned ret = 0;

    switch (request.method()) {

    case Methods::MediaRequestBridge::mediaSourceCreate:
    case Methods::MediaRequestBridge::mediaSourceDestroy:
    case Methods::MediaRequestBridge::sourceBufferSetTimestampOffset:
    case Methods::MediaRequestBridge::sourceBuffersCreate:
    case Methods::MediaRequestBridge::sourceBuffersFlush:
    case Methods::MediaRequestBridge::requestCreate:
    case Methods::MediaRequestBridge::requestAbort:
    case Methods::MediaRequestBridge::requestRemove:
    case Methods::MediaRequestBridge::requestSwapURL:
    case Methods::MediaRequestBridge::endOfStream:
    case Methods::MediaRequestBridge::parseSegmentIndex:
        ret |= Request::Synchronous;
        break;
    case Methods::MediaRequestBridge::sourceBufferAttachData:
    case Methods::MediaRequestBridge::sourceBufferAttachRequest:
        ret |= Request::Synchronous|Request::NoLogArgs;
        break;
    default:
        break;
    }
    return ret;
}

//////////
//
// macro helpers
//
//////////

#define READ_VAR(type, name)                    \
    args.mapValue<type>(name, &ok);             \
    do {                                        \
        if (!ok) {                              \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while (0)

#define READ_VARIANT(name)                      \
    args.value(name, &ok);                      \
    do {                                        \
        if (!ok) {                              \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while (0)

#define READ_ARRAY(var, name)                   \
    Variant var = args[name];                   \
    do {                                        \
        if (!var.isArray()) {                   \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while(0)

#define READ_DATA(var, name)                                            \
    DataBuffer var;                                                     \
    do {                                                                \
        const Variant::StringMapIterator it = args.find(name);          \
        if (it == args.stringMapEnd()) {                                \
            invalidArgumentError(method, name);                         \
            sendEvent("result", result);                                \
            return false;                                               \
        }  else if (it->second.type() == Variant::Type_DataBuffer) {    \
            var = it->second.dataBuffer();                              \
        } else if (it->second.type() == Variant::Type_String) {         \
            var = DataBuffer::fromBase64(it->second.string());          \
        } else {                                                        \
            invalidArgumentError(method, name);                         \
            sendEvent("result", result);                                \
            return false;                                               \
        }                                                               \
    } while (0)


//////////
//
// mediaSourceCreate
//
//////////
Variant MediaRequestBridge::mediaSourceCreate(Variant &result, int method,
                                           const Variant &args)
{
    uint32_t sourceId;

    // create the mediaSource
    NFErr retCode = createBufferPool();
    if (retCode != NFErr_OK)
    {
        addErrorCode(result, retCode);
        return result;
    }

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "mediaSourceCreate");
    {
        ScopedMutex lock(m_mediaBufferPoolMutex);
        sourceId = m_mediaSourceManager->createMediaSource(m_streamingConfigParameter);
        m_lastSourceId = sourceId;
    }

    // get the source we just created
    NrdpMediaSourcePtr mediaSource = m_mediaSourceManager->getMediaSource(sourceId);

    // register the media source with the MediaRequestManager, so that it can receive
    // media requests
    m_mediaRequestManager->addMediaSource( mediaSource );

    // give the media source the manager so that it can complete buffer flushes
    mediaSource->setMediaRequestManager( m_mediaRequestManager );

    // give the media source to the player
    bool ok;
    const std::string mspPath = READ_VAR(std::string, "player");
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "using player at %s", mspPath.c_str());
    m_mediaSourcePlayer = m_playerFactory->findPlayer(mspPath);
    if (m_mediaSourcePlayer)
        m_mediaSourcePlayer->setMediaSource(mediaSource);

    // return sourceid
    result["success"] = true;
    result["sourceid"] = sourceId;

    return result;
}

//////////
//
// mediaSourceDestroy
//
//////////
Variant MediaRequestBridge::mediaSourceDestroy(Variant &result, int method,
                                            const Variant &args)
{
    // read the sourceId
    bool ok;

    uint32_t sourceId = READ_VAR(uint32_t, "sourceid");

    // destroy the mediaSource
#ifdef NRDP_HAS_TRACING
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "mediaSourceDestroy = %d", sourceId);
#endif

    NrdpMediaSourcePtr const mediaSource = m_mediaSourceManager->getMediaSource(sourceId);
    if (!mediaSource.get())
    {
        addErrorCode(result, NFErr_BadParameter);
    }
    else
    {
        m_mediaRequestManager->removeMediaSource( mediaSource );

        m_mediaSourceManager->destroyMediaSource(sourceId);

        //m_mediaSource->flushSourceBuffer();

        if (m_mediaSourcePlayer)
            m_mediaSourcePlayer->setMediaSource(NrdpMediaSourcePtr());

        result["success"] = true;
    }

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "mediaSourceDestroy: %u:%u:%u",
            m_mediaRequests.empty(), m_mediaRequestManager->hasMediaDownloadTracks(),
            m_mediaSourceManager->numberOfMediaSources());
    if ( m_mediaRequests.empty() && !m_mediaRequestManager->hasMediaDownloadTracks()
            && !m_mediaSourceManager->numberOfMediaSources() )
    {
        // Only destory the buffer pool when there are no active media sources and download tracks
        returnBufferPool();
        m_mediaRequestManager->resetConfig();
    }

    return result;
}


//////////
//
// mediaBufferPoolCreate
//
//////////
bool MediaRequestBridge::mediaBufferPoolCreate(Variant &result, int /*method*/,
                                                  const Variant &/*args*/)
{
    // create the media buffer pool
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "mediaBufferPoolCreate");
    NFErr retCode = createBufferPool();
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "mediaBufferPoolCreated");
    addErrorCode(result, retCode);
    sendEvent("result", result);
    return true;
}


//////////
//
// sourceBufferAttachData
//
//////////
Variant MediaRequestBridge::sourceBufferAttachData(Variant &result, int method,
                                                  const Variant &args)
{
    bool ok;

    // read args
    uint32_t sourceId = readSourceId(args);
    uint32_t bufferId = READ_VAR(uint32_t, "bufferid");
    READ_DATA(data, "data");

    NTRACE(TRACE_MEDIAREQUESTBRIDGE,
           "sourceBufferAttachHeader sourceId %d bufferId %d",
           sourceId, bufferId);

    // get the mediaSource
    NrdpMediaSourcePtr mediaSource = m_mediaSourceManager->getMediaSource(sourceId);
    if (!mediaSource.get())
    {
        // invalid media source
        addErrorCode(result, NFErr_BadParameter);
    }
    else
    {
        MediaSourceAppendablePtr pAppendable(new ArrayBufferAppendable(data));

        m_mediaRequestManager->appendBuffer(mediaSource, bufferId, pAppendable);

        addErrorCode(result, NFErr_OK);
    }

    return result;
}

//////////
//
// sourceBufferAttachRequest
//
//////////
Variant MediaRequestBridge::sourceBufferAttachRequest(Variant &result, int method,
                                                   const Variant &args)
{
    bool ok;

    // read args
    uint32_t sourceId = readSourceId(args);
    uint32_t bufferId = READ_VAR(uint32_t, "bufferid");
    uint32_t requestId = READ_VAR(uint32_t, "requestid");

    NTRACE(TRACE_MEDIAREQUESTBRIDGE,
           "sourceBufferAttachRequest sourceId %d bufferId %d requestId %d",
           sourceId, bufferId, requestId);
    NFErr retCode = NFErr_BadParameter;
    JsBridgeMediaRequestPtr pMediaRequest;
    {
        ScopedMutex lock(m_mediaRequestsMutex);
        MediaRequestRecord* const pMediaRequestRecord = findRequest( requestId );
        if ( pMediaRequestRecord )
        {
            pMediaRequestRecord->m_appended = true;
            pMediaRequest = pMediaRequestRecord->m_request;

            // XXX We'll forget about this request here if it is complete
            //     Need to change this if we want to support multi-append
            if ( pMediaRequestRecord->m_complete && pMediaRequest->isAppendable() )
            {
                eraseRequest( requestId );
            }
        }
    }

    if (pMediaRequest && pMediaRequest->isAppendable())
    {
        // get mediaSource
        NrdpMediaSourcePtr mediaSource = m_mediaSourceManager->getMediaSource(sourceId);
        if (mediaSource.get())
        {
            m_mediaRequestManager->appendBuffer(
                mediaSource, bufferId,
                static_pointer_cast<AppendableMediaRequest>(pMediaRequest));
            retCode = NFErr_OK;
        }
    }

    addErrorCode(result, retCode);

    return result;
}

//////////
//
// setSourceBufferTimestampOffset
//
//////////
Variant MediaRequestBridge::sourceBufferSetTimestampOffset(Variant &result, int method,
                                                        const Variant &args)
{
    bool ok;

    // read args
    uint32_t const sourceId = readSourceId(args);
    uint32_t const bufferId = READ_VAR(uint32_t, "bufferid");
    int32_t  const timeOffset = READ_VAR(int32_t, "timeoffset");
    uint32_t const timescale = args.mapValue<uint32_t>( "timescale", 0, 1000 );
    NTRACE(TRACE_MEDIAREQUESTBRIDGE,
           "sourceBufferSetTimestampOffset: src id: %u, buffer id: %u, time offset:%d:%u",
           sourceId, bufferId, timeOffset, timescale);

    // get mediaSource
    NrdpMediaSourcePtr mediaSource = m_mediaSourceManager->getMediaSource(sourceId);
    if (!mediaSource.get())
    {
        addErrorCode(result, NFErr_BadParameter);
    }
    else
    {
        m_mediaRequestManager->setSourceBufferTimestampOffset(mediaSource, bufferId,
                                                              timeOffset, timescale);

        addErrorCode(result, NFErr_OK);
    }

    return result;
}

//////////
//
// sourceBuffersCreate
//
//////////
Variant MediaRequestBridge::sourceBuffersCreate(Variant &result, int method,
                                            const Variant &args)
{
    bool ok;

    // read args
    uint32_t sourceId = READ_VAR(uint32_t, "sourceid");
    READ_ARRAY(mediaTypes, "mediatypes");

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "sourceBuffersCreate %d %s",
           sourceId, mediaTypes.toJSON().c_str());

    // get the mediaSource
    NrdpMediaSourcePtr mediaSource = m_mediaSourceManager->getMediaSource(sourceId);
    if (!mediaSource.get())
    {
        addErrorCode(result, NFErr_BadParameter);
        return result;
    }

    // create each source buffer
    Variant bufferIds;
    Variant::ArrayIterator it;
    for (it = mediaTypes.arrayBegin(); it != mediaTypes.arrayEnd(); ++it)
    {
        MediaType mediaType = (MediaType)it->value<uint32_t>();
        uint32_t bufferId = mediaSource->createSourceBuffer(mediaType);
        bufferIds.push_back(bufferId);
    }
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "sourceBufferCreate = %s",
           bufferIds.toJSON().c_str());

    // return bufferid's created
    result["success"] = true;
    result["bufferids"] = bufferIds;

    return result;
}

//////////
//
// sourceBuffersFlush
//
//////////
Variant MediaRequestBridge::sourceBuffersFlush(Variant &result, int method,
                                            const Variant &args)
{
    // read args
    READ_ARRAY(bufferIds, "bufferids");

    // TODO: remove this API

    Variant results;
    result["success"] = true;
    result["results"] = results;

    return result;
}

//////////
//
// downloadTracksCreate
//
//////////
bool MediaRequestBridge::downloadTracksCreate(Variant &result, int method,
                                                 const Variant &args)
{
    // read args
    READ_ARRAY(configs, "configs");

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "downloadTracksCreate %s",
           configs.toJSON().c_str());

    NFErr retCode = createBufferPool();
    if (retCode != NFErr_OK)
    {
        addErrorCode(result, retCode);
        sendEvent("result", result);
        return false;
    }
    m_mediaRequestManager->printStats();

    // create download tracks
    Variant trackIds;
    Variant::ArrayIterator it;
    for (it = configs.arrayBegin(); it != configs.arrayEnd(); ++it)
    {
        MediaType const mediaType = (MediaType)it->mapValue<uint32_t>( "type" );
        bool openRange = it->mapValue<bool>( "openRange", 0, true );
        bool pipeline = it->mapValue<bool>( "pipeline", 0, false );
        uint32_t connections = it->mapValue<uint32_t>( "connections", 0, 1 );
        uint32_t const socketBufferSize = it->mapValue<uint32_t>(
            "socketBufferSize", 0, 0);

        TrackConfiguration config( openRange, pipeline, connections, socketBufferSize );

        NFErr err;
        uint32_t trackId = 0;

        if ( mediaType > 3 )
        {
            err = NFErr_BadParameter;
        }
        else
        {
            err = m_mediaRequestManager->createDownloadTrack(mediaType, config, trackId);
        }

        if (err != NFErr_OK)
        {
            NTRACE(TRACE_MEDIAREQUESTBRIDGE, "failed to add track type %d: %s",
                   mediaType, err.toString().c_str());
            result["success"] = false;
            result["errorcode"] = err.toVariant();
            sendEvent("result", result);
            return false;
        }
        NTRACE(TRACE_MEDIAREQUESTBRIDGE,
               "added track: %u, %u:%u:%u:%u for media type: %u",
               trackId,
               config.mOpenRange, config.mPipeline,
               config.mConnections, config.mSocketBufferSize,
               mediaType);
        trackIds.push_back(trackId);

        m_trackIdToMediaTypeMap[trackId] = mediaType;
    }
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "downloadTracksCreate = %s",
           trackIds.toJSON().c_str());

    // return the trackId
    result["success"] = true;
    result["trackids"] = trackIds;
    sendEvent("result", result);
    return true;
}

//////////
//
// downloadTrackPause
//
//////////
bool MediaRequestBridge::downloadTrackPause(Variant &result, int method,
                                               const Variant &args)
{
    bool ok;

    // read args
    uint32_t trackId = READ_VAR(uint32_t, "trackid");
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "downloadTrackPause %d", trackId);

    // pause track
    NFErr err = m_mediaRequestManager->pauseDownloadTrack( trackId );
    if (err != NFErr_OK)
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE,
               "failed to pause track: %s", err.toString().c_str());
        result["success"] = false;
        result["errorcode"] = err.toVariant();
    }
    else
    {
        result["success"] = true;
    }

    sendEvent("result", result);
    return true;
}

//////////
//
// downloadTrackResume
//
//////////
bool MediaRequestBridge::downloadTrackResume(Variant &result, int method,
                                                const Variant &args)
{
    bool ok;

    // read args
    uint32_t trackId = READ_VAR(uint32_t, "trackid");
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "downloadTrackResume %d", trackId);

    // resume track
    NFErr err = m_mediaRequestManager->resumeDownloadTrack( trackId );
    if (err != NFErr_OK)
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE,
               "failed to resume track: %s", err.toString().c_str());
        result["success"] = false;
        result["errorcode"] = err.toVariant();
    }
    else
    {
        result["success"] = true;
    }

    sendEvent("result", result);
    return true;
}

//////////
//
// downloadTrackReconfigure
//
//////////
bool MediaRequestBridge::downloadTrackReconfigure(Variant &result, int method,
                                                     const Variant &args)
{
    bool ok;

    // read args
    uint32_t trackId = READ_VAR(uint32_t, "trackid");
    Variant vconfig = READ_VARIANT("config");

    bool openRange = vconfig.mapValue<bool>( "openRange", 0, true );
    bool pipeline = vconfig.mapValue<bool>( "pipeline", 0, false );
    uint32_t connections = vconfig.mapValue<uint32_t>( "connections", 0, 1 );
    uint32_t const socketBufferSize = vconfig.mapValue<uint32_t>(
        "socketBufferSize", 0, 0);

    TrackConfiguration const config( openRange, pipeline, connections, socketBufferSize );

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "downloadTrackReconfigure %d", trackId);

    // reconfigure track
    NFErr err = m_mediaRequestManager->reconfigureDownloadTrack( trackId, config );
    if (err != NFErr_OK)
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE,
               "failed to reconfigure track: %s", err.toString().c_str());
        result["success"] = false;
        result["errorcode"] = err.toVariant();
    }
    else
    {
        result["success"] = true;
    }

    sendEvent("result", result);
    return true;
}

//////////
//
// downloadTracksDestroy
//
//////////
bool MediaRequestBridge::downloadTracksDestroy(Variant &result, int method,
                                                  const Variant &args)
{
    // read args
    READ_ARRAY(trackIds, "trackids");

    NTRACE(TRACE_MEDIAREQUESTBRIDGE,
           "downloadTracksDestroy %s", trackIds.toJSON().c_str());

    // remove tracks
    Variant results;
    bool success = true;

    Variant::ArrayIterator it;
    for (it = trackIds.arrayBegin(); it != trackIds.arrayEnd(); ++it)
    {
        Variant r;
        uint32_t trackId = it->value<uint32_t>();

        (void) m_mediaRequestManager->removeDownloadTrack(trackId);

        m_trackIdToMediaTypeMap.erase( trackId );

        // We may have removed the track in NrdpVideo
        flushMediaRequests(trackId);

        NTRACE(TRACE_MEDIAREQUESTBRIDGE,
                "flushed media requests for track id: %u", trackId);

        r["success"] = true;
        results.push_back(r);
    }
    result["success"] = success;
    result["results"] = results;
    sendEvent("result", result);

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "MediaRequestBridge Objects: %zu:%u:%zu",
           m_mediaRequests.size(), m_scheduledMediaRequests,
           m_progressEvents.size());

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "downloadTracksDestroy: %u:%u:%u",
            m_mediaRequests.empty(), m_mediaRequestManager->hasMediaDownloadTracks(),
            m_mediaSourceManager->numberOfMediaSources());
    if ( m_mediaRequests.empty() && !m_mediaRequestManager->hasMediaDownloadTracks()
            && !m_mediaSourceManager->numberOfMediaSources() )
    {
        // Only destory the buffer pool when there are no active media sources and download tracks
        returnBufferPool();
        m_mediaRequestManager->resetConfig();
    }
    return true;
}

//////////
//
// requestCreate
//
//////////
Variant MediaRequestBridge::requestCreate(Variant &result, int method, const Variant &args)
{
    bool ok;

    // read args
    uint32_t requestId = READ_VAR(uint32_t, "requestid");
    uint32_t trackId = READ_VAR(uint32_t, "trackid");
    std::string url = READ_VAR(std::string, "url");
    llong byteStart = READ_VAR(llong, "start");
    llong byteEnd = READ_VAR(llong, "end");
    ResponseType responseType = (ResponseType)READ_VAR(uint32_t, "responsetype");
    NTRACE(TRACE_MEDIAREQUESTBRIDGE,
           "requestCreate track %d request id: %u, %lld-%lld, %d, %s",
           trackId, requestId, byteStart, byteEnd, responseType, url.c_str());

    //TrackConfiguration trackConfig = m_trackConfigurations[trackId];

    JsBridgeMediaRequestPtr pMediaRequest;
    AseErrorCode retCode = createMediaRequest(requestId, responseType, trackId, url,
                                              byteStart, byteEnd, /*out*/pMediaRequest);

    if ( retCode != AS_NO_ERROR )
    {
        Log::warn(TRACE_MEDIAREQUESTBRIDGE, "failed to create request: %d", retCode);
        result["success"] = false;
        result["errorcode"] = retCode;
    }
    else
    {
        // forward to MediaRequestManager
        m_mediaRequestManager->addMediaRequest(pMediaRequest);
        {
            ScopedMutex lock(m_mediaRequestsMutex);
            m_mediaRequests[requestId] = MediaRequestRecord( pMediaRequest );
        }

        // return request id
        result["success"] = true;
        result["requestid"] = requestId;
    }

    return result;
}

//////////
//
// requestAbort
//
//////////
Variant MediaRequestBridge::requestAbort(Variant &result, int method, const Variant &args)
{
    bool ok;

    // read args
    uint32_t requestId = READ_VAR(uint32_t, "requestid");
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "abortRequest %d", requestId);

    AseErrorCode retCode = abortMediaRequest(requestId);
    if ( retCode != AS_NO_ERROR )
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE, "failed to abort request: %d", retCode);
        result["success"] = false;
        result["error"] = retCode;
    }
    else
    {
        result["success"] = true;
    }

    return result;
}

//////////
//
// requestRemove
//
//////////
Variant MediaRequestBridge::requestRemove(Variant &result, int method, const Variant &args)
{
    bool ok;

    // read args
    uint32_t requestId = READ_VAR(uint32_t, "requestid");
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "removeRequest %d", requestId);

    AseErrorCode retCode = abortMediaRequest(requestId);
    if ( retCode != AS_NO_ERROR )
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE, "failed to remove request: %d", retCode);
        result["success"] = false;
        result["error"] = retCode;
    }
    else
    {
        result["success"] = true;
    }

    return result;
}

//////////
//
// requestSwapURL
//
//////////
Variant MediaRequestBridge::requestSwapURL(Variant &result, int method, const Variant &args)
{
    bool ok;

    // read args
    uint32_t requestId = READ_VAR(uint32_t, "requestid");
    std::string url = READ_VAR(std::string, "url");

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "requestSwapURL %d = %s",
           requestId, url.c_str());

    AseErrorCode retCode = swapUrl(requestId, url);
    if ( retCode != AS_NO_ERROR )
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE, "failed to swap url: %d", retCode);
        result["success"] = false;
        result["error"] = retCode;
    }
    else
    {
        result["success"] = true;
    }

    return result;
}

//////////
//
// endOfStream
//
//////////
Variant MediaRequestBridge::endOfStream(Variant &result, int method, const Variant &args)
{
    bool ok;
    uint32_t bufferId = READ_VAR(uint32_t, "bufferid");

    // read sourceId
    uint32_t sourceId = args.mapValue<uint32_t>("sourceid", &ok);
    if (!ok)
    {
        // HACK - if sourceId not found, use the lastSourceId created
        Log::warn(TRACE_MEDIAREQUESTBRIDGE,
                  "endOfStream not passed sourceId, using last one %u",
                  m_lastSourceId);
        sourceId = m_lastSourceId;
    }

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "endOfStream: sourceId %u bufferId: %u",
           sourceId, bufferId);

    // get the mediaSource
    NrdpMediaSourcePtr mediaSource = m_mediaSourceManager->getMediaSource(sourceId);
    if (!mediaSource.get())
    {
        addErrorCode(result, NFErr_BadParameter);
    }
    else
    {
        mediaSource->endOfStream(bufferId);

        addErrorCode(result, NFErr_OK);
    }
    return result;
}

Variant MediaRequestBridge::parseSegmentIndex(Variant& result, int method, const Variant& args)
{
    bool ok;

    READ_DATA( in, "data" );
    uint32_t const anchor = READ_VAR( uint32_t, "anchor" );

    uint32_t const* p = (uint32_t const*) ( in.constData() );

    uint32_t const version_and_flags = ntohl( *p++ );
    ++p;                                                // reference_id
    uint32_t const timescale = ntohl( *p++ );
    ullong presentation_time, offset;

    if ( version_and_flags & 0xff000000 )
    {
        presentation_time = (((ullong) ntohl( *p )) << 32 ) + ntohl( *( p + 1 ) );
        p += 2;
        offset = (((ullong) ntohl( *p )) << 32 ) + ntohl( *( p + 1 ) );
        p += 2;
    }
    else
    {
        presentation_time = ntohl( *p++ );
        offset = ntohl( *p++ );
    }

    uint32_t const reference_count = ntohl( *p++ ) & 0xffff;

    offset += anchor;

    NTRACE(TRACE_MEDIAREQUESTBRIDGE,"parseSegmentIndex: %x, timescale: %u, pts: %llu, anchor: %u, offset: %llu, count: %u",
        version_and_flags,
        timescale,
        presentation_time,
        anchor,
        offset,
        reference_count );

    // Sidx format
    // [size] 4 bytes (1 bit type, 31 bits size )
    // [duration] 4 bytes
    // [sap information] 4 bytes
    //
    // Fragment Format:
    // [size] 4 bytes
    // [startPts] 4 bytes
    // [offset] 6 bytes
    // [duration] 2 bytes

    DataBuffer out( reference_count * 16 );
    out.setUsed( reference_count * 16 );
    uint32_t *q = (uint32_t*) ( out.data() );

    uint32_t presentation_time_ms = (uint32_t) ( presentation_time * 1000 / timescale );
    uint32_t total_duration_ms = 0;

    for( uint32_t i = 0; i < reference_count; ++i )
    {
        uint32_t const size = ntohl(*p++) & 0x7fffffff;
        uint32_t const duration = ntohl( *p++ );
        ++p;

        *q++ = htonl( size );
        *q++ = htonl( presentation_time_ms );

        ullong const next_presentation_time = presentation_time + duration;
        uint32_t const next_presentation_time_ms = (uint32_t) ( next_presentation_time * 1000 / timescale );
        uint32_t const duration_ms = next_presentation_time_ms - presentation_time_ms;

        *q++ = htonl( (uint32_t) ( ( offset >> 16 ) & 0xffffffff ) );
        *q++ = htonl( (uint32_t) ( ( offset & 0xffff ) << 16 ) + duration_ms );

        offset += size;
        presentation_time = next_presentation_time;
        presentation_time_ms = next_presentation_time_ms;
        total_duration_ms += duration_ms;
    }

    result["success"] = true;
    result["data"] = out;
    result["avdur"] = total_duration_ms / reference_count;

    return result;
}

//////////
//
// helper functions
//
//////////
AseErrorCode MediaRequestBridge::createMediaRequest(
    uint32_t requestId, ResponseType responseType, uint32_t trackId, string const& url,
    llong start, llong end, JsBridgeMediaRequestPtr& pMediaRequest)
{
    NRDP_UNUSED( responseType );

    assert(end >= start);
    // TODO: verify this is a valid track
    //
    AseUrl aseUrl(url);
    ByteRange byteRange(start, end);

    MediaType mediaType = m_trackIdToMediaTypeMap[trackId];
    AseErrorCode aseErr = AS_NO_ERROR;

    switch(responseType)
    {
        case DATABUFFER:
            pMediaRequest.reset( new BufferedMediaRequest( aseUrl, byteRange ) );
            break;

        case STREAM:
        {
            ScopedMutex lock(m_mediaBufferPoolMutex);
            if ( m_mediaBufferPool )
            {
                pMediaRequest.reset( new AppendableMediaRequest(    aseUrl,
                                                                    byteRange,
                                                                    m_mediaBufferPool,
                                                                    mediaType ) );
            }
            else
            {
                aseErr = AS_NOT_ALlOWED;
            }
            break;
        }

        default:
            aseErr = AS_NOT_ALlOWED;
            break;
    }



    if ( aseErr == AS_NO_ERROR )
    {
        pMediaRequest->setDownloadTrackId(trackId);
        pMediaRequest->setRequestId(requestId);
        pMediaRequest->setListener(mListener);
    }

    return aseErr;
}

AseErrorCode
MediaRequestBridge::abortMediaRequest(uint32_t requestId)
{
    AseErrorCode retCode = AS_NO_ERROR;
    bool pending = false;
    JsBridgeMediaRequestPtr pMediaRequest;
    {
        ScopedMutex lock(m_mediaRequestsMutex);
        MediaRequestRecord* const pMediaRequestRecord = findRequest( requestId );
        if ( pMediaRequestRecord )
        {
            if ( !pMediaRequestRecord->m_appended )
            {
                pMediaRequest = pMediaRequestRecord->m_request;

                pending = true;

                eraseRequest( requestId );
            }
            else
            {
                retCode = AS_NOT_ALlOWED;
            }

        }
        else
        {
            retCode = AS_NOT_FOUND;
        }
    }

    if ( pMediaRequest && pending )
    {
        NFErr abortRetCode = m_mediaRequestManager->abortPendingMediaRequest(
            pMediaRequest);
        if ( abortRetCode == NFErr_OK )
        {
            pMediaRequest.reset();
        }
    }

    if ( pMediaRequest )
        m_mediaRequestManager->abandonMediaRequest( pMediaRequest );

    return retCode;
}

void
MediaRequestBridge::flushMediaRequests(uint32_t trackId)
{
    std::vector<JsBridgeMediaRequestPtr> requests;

    {
        ScopedMutex lock(m_mediaRequestsMutex);
        map<uint32_t, MediaRequestRecord>::iterator iter = m_mediaRequests.begin();
        while(iter != m_mediaRequests.end())
        {
            JsBridgeMediaRequestPtr const pMediaRequest = iter->second.m_request;
            if ( pMediaRequest->getDownloadTrackId() == trackId )
            {
                if ( iter->second.m_scheduled )
                    --m_scheduledMediaRequests;

                map<uint32_t, MediaRequestRecord>::iterator const remove = iter++;

                m_mediaRequests.erase(remove);

                requests.push_back( pMediaRequest );
            }
            else
            {
                ++iter;
            }
        }
    }

    m_mediaRequestManager->flushMediaRequests(requests);
    m_mediaRequestManager->flushMediaRequests(trackId);
    return;
}

AseErrorCode
MediaRequestBridge::swapUrl(uint32_t requestId, string const& url)
{
    JsBridgeMediaRequestPtr pMediaRequest;
    {
        ScopedMutex lock(m_mediaRequestsMutex);
        pMediaRequest = findMediaRequest( requestId );
    }

    if ( !pMediaRequest )
        return AS_NOT_FOUND;

    m_mediaRequestManager->swapUrl(pMediaRequest, AseUrl(url));

    return AS_NO_ERROR;
}

void MediaRequestBridge::addErrorCode(Variant &result, NFErr const& errorCode)
{
    if ( errorCode.ok() )
    {
        result["success"] = true;
    }
    else
    {
        result["success"] = false;
        result["error"] = errorCode.toVariant();
    }
}

//////////
//
// startHttpManager
//
//////////
void MediaRequestBridge::startHttpManager()
{
    ScopedMutex lock(m_httpStartMutex);

    if (m_httpStarted)
        return;

    HttpRequestManager::instance()->start();
    m_httpStarted = true;
}

void MediaRequestBridge::stopHttpManager()
{
    ScopedMutex lock(m_httpStartMutex);

    if (!m_httpStarted)
        return;

    HttpRequestManager::instance()->stop();
    m_httpStarted = false;
}

//////////
//
// createBufferPool
//
//////////
NFErr MediaRequestBridge::createBufferPool()
{
    ScopedMutex lock(m_mediaBufferPoolMutex);

    if (m_mediaBufferPool)
        return NFErr_OK;

    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "createBufferPool");

    m_bufferManager = nrdApp()->getDeviceLib()->getBufferManager();

    McMediaBufferPool::getBufferPool(m_bufferManager, m_mediaBufferPool);

    uint32_t videoMemorySize = m_streamingConfigParameter->getParameterAs<uint32_t>
                                    ("videoMemorySize", -1);
    uint32_t videoVirtualMemorySize = m_streamingConfigParameter->getParameterAs<uint32_t>
                                    ("videoVirtualMemorySize", -1);
    uint32_t audioMemorySize = m_streamingConfigParameter->getParameterAs<uint32_t>
                                    ("audioMemorySize", -1);
    m_mediaBufferPool->setBufferPoolSizes(videoMemorySize, videoVirtualMemorySize,
                                          audioMemorySize);

    AseErrorCode aseErr = m_mediaBufferPool->open(defaultBufferPoolUnitSize, 0);
    if (aseErr!= AS_NO_ERROR)
    {
        Log::warn(TRACE_MEDIAREQUESTBRIDGE, "failed to open buffer pool: %u", aseErr);
        NFErr err(new ASError(aseErr));
        if (aseErr == AS_NOT_ENOUGH_MEMORY)
            return err.push(NFErr_MC_BufferAllocationFailure);
        else
            return err.push(NFErr_MC_InitStreamFailure);
    }

    Log::warn(TRACE_MEDIAREQUESTBRIDGE, "created the buffer pool");
    m_mediaRequestManager->setConfig(m_streamingConfigParameter, m_mediaBufferPool);

    return NFErr_OK;
}

void MediaRequestBridge::returnBufferPool()
{
    ScopedMutex lock(m_mediaBufferPoolMutex);
    NTRACE(TRACE_MEDIAREQUESTBRIDGE, "buffer pool: %lu", m_mediaBufferPool.use_count());
    m_mediaBufferPool.reset();
    m_bufferManager.reset();
    Log::warn(TRACE_MEDIAREQUESTBRIDGE, "returned the buffer pool");
}

void MediaRequestBridge::onProgress(uint32_t requestId, const AseTimeVal &time,
                                    uint32_t elapsedTimeInMs, uint32_t bytesReceived,
                                    uint32_t httpResponseTime )
{
    m_progressEvents[requestId].update(time, elapsedTimeInMs, bytesReceived,
                                       httpResponseTime );

    AseTimeVal now = AseTimeVal::now();
    if (m_lastProgressEventSentTime + AseTimeVal::fromMS(m_progressReportInterval) < now)
    {
        Variant evt;
        evt["type"] = "onprogress";
        evt["requestId"] = requestId;
        Variant progress;
        map< uint32_t, ProgressEvent>::iterator iter;
        for (iter = m_progressEvents.begin();
             iter != m_progressEvents.end();
             ++iter )
        {
            Variant m;
            m["requestId"] = iter->first;
            m["timestamp"] = (iter->second).m_time.ms();
            m["timeInMs"] = (iter->second).m_elapsedTimeInMs;
            m["bytes"] = (iter->second).m_bytes;
            if (!(iter->second).m_responseTimes.empty())
                m["rt"] = (iter->second).m_responseTimes;
            progress.push_back(m);
        }
        evt["progress"] = progress;
        sendEvent("onprogress", evt);
        m_progressEvents.clear();
        m_lastProgressEventSentTime = now;
    }
}

void MediaRequestBridge::onError(uint32_t requestId, const AseTimeVal &time,
                                 AseErrorCode errorCode, int32_t httpStatusCode)
{
    Variant evt;
    evt["type"] = "onerror";
    evt["timestamp"] = time.ms();
    evt["requestId"] = requestId;
    evt["errorcode"] = errorCode;
    evt["httpcode"] = httpStatusCode;
    sendEvent("onerror", evt);
}

void MediaRequestBridge::onComplete(uint32_t requestId, const AseTimeVal &time,
                                    uint32_t elapsedTimeInMs, DataBuffer const* pData)
{
    {
        ScopedMutex lock(m_mediaRequestsMutex);
        MediaRequestRecord* pMediaRequestRecord = findRequest( requestId );
        if ( pMediaRequestRecord )
        {
            // XXX Forget about non-appendable or already appended requests
            //     We will need to change this if we want to allow multi-append
            if ( pMediaRequestRecord->m_appended ||
                 !pMediaRequestRecord->m_request->isAppendable() )
            {
                eraseRequest( requestId );
            }
            else
            {
                pMediaRequestRecord->m_complete = true;
                pMediaRequestRecord->m_scheduled = false;
                --m_scheduledMediaRequests;
            }
        }
    }

    Variant evt;
    evt["type"] = "oncomplete";
    evt["timestamp"] = time.ms();
    evt["requestId"] = requestId;
    evt["timeInMs"] = elapsedTimeInMs;
    if ( pData )
        evt["data"] = *pData;

    sendEvent("oncomplete", evt);

    if ( m_downloadPausePending && m_scheduledMediaRequests == 0 )
    {
        sendDownloadPausedEvent(AseTimeVal::now());
    }
}

void MediaRequestBridge::onLoadStart(uint32_t requestId, const AseTimeVal &time)
{
    Variant evt;
    evt["type"] = "onloadstart";
    evt["timestamp"] = time.ms();
    evt["requestId"] = requestId;
    sendEvent("onloadstart", evt);
}

void MediaRequestBridge::onFirstByteReceived(   uint32_t requestId,
                                                const AseTimeVal &time,
                                                const AseTimeVal &startTime,
                                                HttpResponseHeaders headers,
                                                bool newConnection )
{
    Variant evt;
    evt["type"] = "onfirstbyte";
    evt["timestamp"] = time.ms();
    evt["requestId"] = requestId;
    evt["headers"] = headers;
    if ( startTime != AseTimeVal::ZERO )
    {
        evt["start"] = startTime.ms();
        evt["connect"] = newConnection;
    }

    sendEvent("onfirstbyte", evt);
}

void MediaRequestBridge::downloadPaused(const AseTimeVal &time)
{
    m_downloadPausePending = true;
    if ( m_scheduledMediaRequests == 0 )
        sendDownloadPausedEvent(time);
}

void MediaRequestBridge::sendDownloadPausedEvent(const AseTimeVal &time)
{
    Variant evt;
    evt["type"] = "downloadpaused";
    evt["timestamp"] = time.ms();
    sendEvent("downloadpaused", evt);
    m_downloadPausePending = false;
}

void MediaRequestBridge::downloadResumed(const AseTimeVal &time)
{
    m_downloadPausePending = false;
    Variant evt;
    evt["type"] = "downloadresumed";
    evt["timestamp"] = time.ms();
    sendEvent("downloadresumed", evt);
}

void MediaRequestBridge::onPipeliningDisabled(string const& reason)
{
    Variant evt;
    evt["type"] = "pipelinedetection";
    evt["pipeline"] = false;
    evt["reason"] = reason.c_str();
    sendEvent("pipelinedetection", evt);
}

void MediaRequestBridge::onRequestScheduled(uint32_t requestId)
{
    {
        ScopedMutex lock(m_mediaRequestsMutex);
        MediaRequestRecord* const pMediaRequestRecord = findRequest( requestId );
        if ( pMediaRequestRecord )
        {
            pMediaRequestRecord->m_scheduled = true;
            ++m_scheduledMediaRequests;
        }
    }

    ScopedMutex lock(m_mediaBufferPoolMutex);
    if ( m_mediaBufferPool )
    {
        NTRACE(TRACE_MEDIAREQUESTBRIDGE, "available buffer: %u:%u",
               m_mediaBufferPool->getUnitSize() *
                 m_mediaBufferPool->getFreeUnitCount(MEDIA_AUDIO),
               m_mediaBufferPool->getUnitSize() *
                 m_mediaBufferPool->getFreeUnitCount(MEDIA_VIDEO));
    }
}

void MediaRequestBridge::onNetworkFailing(  uint32_t                downloadTrackId,
                                            AseErrorCode            latestStatus,
                                            AseUrl::HostPort const& destination,
                                            AseTimeVal const*       pLatestErrorTime,
                                            AseTimeVal const&       latestDataReceived )
{
    Variant evt;
    evt["type"] = "networkfailing";
    evt["track"] = downloadTrackId;
    evt["status"] = latestStatus;
    evt["host"] = destination.first.c_str();
    evt["port"] = destination.second;
    if ( pLatestErrorTime )
        evt["errortime"] = pLatestErrorTime->ms();
    evt["datatime"] = latestDataReceived.ms();
    sendEvent( "networkfailing", evt );
}

void MediaRequestBridge::TransportReporterEvent(const char *function, const Variant &args)
{
    Variant vmap;
    vmap["type"] = "TransportReporter";
    vmap["funcname"] = function;
    vmap["args"] = args;
    sendEvent("TransportReporter", vmap);
}

MediaRequestBridge::MediaRequestRecord* MediaRequestBridge::findRequest(
    uint32_t requestid )
{
    std::map<uint32_t, MediaRequestRecord>::iterator it =
        m_mediaRequests.find( requestid );
    return ( it != m_mediaRequests.end() ) ? &(it->second) : 0;
}

JsBridgeMediaRequestPtr MediaRequestBridge::findMediaRequest( uint32_t requestid )
{
    MediaRequestRecord* const pRequestRecord = findRequest( requestid );
    return pRequestRecord ? pRequestRecord->m_request : JsBridgeMediaRequestPtr();
}

void MediaRequestBridge::eraseRequest( uint32_t requestid )
{
    std::map<uint32_t, MediaRequestRecord>::iterator it = m_mediaRequests.find(
        requestid );
    if ( it != m_mediaRequests.end() )
    {
        if ( it->second.m_scheduled )
        {
            --m_scheduledMediaRequests;
        }

        m_mediaRequests.erase( it );
    }
}

uint32_t MediaRequestBridge::readSourceId(const Variant &args)
{
    bool ok;
    // read sourceId
    uint32_t sourceId = args.mapValue<uint32_t>("sourceid", &ok);
    if (!ok)
    {
        // HACK - if sourceId not found, use the lastSourceId created
        Log::warn(TRACE_MEDIAREQUESTBRIDGE,
                  "Not passed sourceId, using last one %u", m_lastSourceId);
        sourceId = m_lastSourceId;
    }
    return sourceId;
}

