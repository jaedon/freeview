/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MEDIAREQUESTBRIDGE_H__
#define __MEDIAREQUESTBRIDGE_H__

#include <nrdase/IMediaBufferPool.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/HttpPipeliningSupportDetector.h>

#include <nrdnet/HttpResponseHeaders.h>

#include "MediaRequestManager.h"
#include "MediaSourceAppendable.h"
#include "NfObject.h"
#include "IBufferManager.h"

#include <map>

namespace netflix {

namespace mediacontrol {

class IMediaSourcePlayer;

class LocalConfigParameter;
typedef shared_ptr<LocalConfigParameter> LocalConfigParameterPtr;

class NrdpMediaSourceManager;
typedef shared_ptr<NrdpMediaSourceManager> NrdpMediaSourceManagerPtr;

class NrdpMediaSource;
typedef shared_ptr<NrdpMediaSource> NrdpMediaSourcePtr;

class NrdpVideo;
typedef shared_ptr<NrdpVideo> NrdpVideoPtr;

class StreamingConfigParameter;
typedef shared_ptr<StreamingConfigParameter> StreamingConfigParameterPtr;

class JsBridgeMediaRequest;
typedef shared_ptr<JsBridgeMediaRequest> JsBridgeMediaRequestPtr;

}

class MediaSourcePlayerFactoryBridge;

class MediaRequestBridge : public NfObject
{
public:
    enum ResponseType
    {
        DATABUFFER = 0,
        STREAM
    };

public:
    MediaRequestBridge(mediacontrol::NrdpMediaSourceManagerPtr mediaSourceManager,
                       shared_ptr<MediaSourcePlayerFactoryBridge> playerFactory);
    virtual ~MediaRequestBridge();

    void init();
    void deinit();

    // NfObject interface
    virtual Variant invoke(int method, const Variant &var);
    virtual unsigned requestFlags(const Request &request) const;

private:
    // invoke methods
    Variant mediaSourceCreate(Variant &result, int meth, const Variant &a);
    Variant mediaSourceDestroy(Variant &result, int meth, const Variant &a);

    bool mediaBufferPoolCreate(Variant &result, int meth, const Variant &a);

    Variant sourceBufferAttachData(Variant &r, int meth, const Variant &a);
    Variant sourceBufferAttachRequest(Variant &r, int meth, const Variant &a);
    Variant sourceBufferSetTimestampOffset(Variant &r, int meth, const Variant &a);
    Variant sourceBuffersCreate(Variant &result, int meth, const Variant &a);
    Variant sourceBuffersFlush(Variant &result, int meth, const Variant &a);

    bool downloadTracksCreate(Variant &result, int meth, const Variant &a);
    bool downloadTrackPause(Variant &result, int meth, const Variant &a);
    bool downloadTrackResume(Variant &result, int meth, const Variant &a);
    bool downloadTrackReconfigure(Variant &result, int meth, const Variant &a);
    bool downloadTracksDestroy(Variant &result, int meth, const Variant &a);

    Variant requestCreate(Variant &result, int meth, const Variant &a);
    Variant requestAbort(Variant &result, int meth, const Variant &a);
    Variant requestRemove(Variant &result, int meth, const Variant &a);
    Variant requestSwapURL(Variant &result, int meth, const Variant &a);
    Variant endOfStream(Variant &result, int meth, const Variant &a);

    Variant parseSegmentIndex(Variant& result, int meth, const Variant&a);

    void flushMediaRequests(uint32_t trackId);
    AseErrorCode createMediaRequest(uint32_t requestId,
                                    ResponseType responseType,
                                    uint32_t trackId,
                                    std::string const& url,
                                    llong start,
                                    llong end,
                                    mediacontrol::JsBridgeMediaRequestPtr& pMediaRequest);
    AseErrorCode abortMediaRequest(uint32_t requestId);

    AseErrorCode swapUrl(uint32_t requestId, std::string const& url);

    void startHttpManager();
    void stopHttpManager();
    NFErr createBufferPool();
    // return the buffer pool
    void returnBufferPool();

    // JsBridgeMediaRequest interfaces
    void onProgress(uint32_t requestId, const AseTimeVal &time,
                    uint32_t elapsedTimeInMs, uint32_t bytesReceived,uint32_t httpResponseTime);
    void onError(uint32_t requestId, const AseTimeVal &time, AseErrorCode errCode,int32_t httpStatusCode);

    void onComplete(uint32_t requestId, const AseTimeVal &time,
                            uint32_t elapsedTimeInMs, DataBuffer const* pData = 0 );

    void onLoadStart(uint32_t requestId, const AseTimeVal &time);
    void onFirstByteReceived(uint32_t requestId, const AseTimeVal &time,const AseTimeVal &startTime, HttpResponseHeaders headers, bool newConnection);

    void downloadPaused(const AseTimeVal &time);
    void downloadResumed(const AseTimeVal &time);
    void onPipeliningDisabled(std::string const& reason);
    void onRequestScheduled(uint32_t requestId);
    void onNetworkFailing(  uint32_t                downloadTrackId,
                            AseErrorCode            latestStatus,
                            AseUrl::HostPort const& destination,
                            AseTimeVal const*       pLatestErrorTime,
                            AseTimeVal const&       latestDataReceived );

    void sendDownloadPausedEvent(const AseTimeVal &time);

    void TransportReporterEvent(const char *function, Variant const& args);

protected:

    struct MediaRequestRecord
    {
        mediacontrol::JsBridgeMediaRequestPtr   m_request;
        bool                                    m_scheduled;
        bool                                    m_appended;
        bool                                    m_complete;

        MediaRequestRecord( mediacontrol::JsBridgeMediaRequestPtr pRequest = mediacontrol::JsBridgeMediaRequestPtr() )
            : m_request( pRequest )
            , m_scheduled( false )
            , m_appended( false )
            , m_complete( false )
        {}
    };

    MediaRequestRecord* findRequest( uint32_t requestid );

    mediacontrol::JsBridgeMediaRequestPtr findMediaRequest( uint32_t requestid );

    void eraseRequest( uint32_t requestId );

    void addErrorCode(Variant &result, NFErr const& errorCode);

    AseErrorCode disablePipeline();

protected:

    // helper class for JsBridgeMediaRequestListener interfaces
    class MediaRequestListener;
    shared_ptr<MediaRequestListener> mListener;
    friend class MediaRequestListener;

    // factory to access MediaSourcePlayers
    shared_ptr<MediaSourcePlayerFactoryBridge> m_playerFactory;

    Mutex m_httpStartMutex;
    Mutex m_mediaRequestsMutex;
    Mutex m_mediaBufferPoolMutex;
    bool m_httpStarted;

    bool m_downloadPausePending;

    mediacontrol::MediaRequestManagerPtr            m_mediaRequestManager;
    shared_ptr<mediacontrol::IMediaSourcePlayer>    m_mediaSourcePlayer;
    ase::IMediaBufferPoolPtr                        m_mediaBufferPool;
    shared_ptr<device::IBufferManager>              m_bufferManager;

    mediacontrol::StreamingConfigParameterPtr       m_streamingConfigParameter;
    mediacontrol::LocalConfigParameterPtr           m_localConfigParameter;

    std::map<uint32_t, ase::MediaType>              m_trackIdToMediaTypeMap;
    std::map<uint32_t, MediaRequestRecord>          m_mediaRequests;
    uint32_t                                        m_scheduledMediaRequests;

    class ProgressEvent;
    std::map<uint32_t, ProgressEvent>               m_progressEvents;
    AseTimeVal                                      m_lastProgressEventSentTime;
    uint32_t                                        m_progressReportInterval;

    mediacontrol::NrdpMediaSourceManagerPtr         m_mediaSourceManager;

    class TransportReporter;
    shared_ptr<TransportReporter>                   m_transportReporter;

    static const uint32_t defaultBufferPoolUnitSize = 4096;

    // HACK for legacy JS support
    // can be removed once the next nrdjs is pushed
    uint32_t readSourceId(const Variant &args);
    uint32_t m_lastSourceId;
};

} // namespace netflix

#endif // __MEDIAREQUESTBRIDGE_H__
