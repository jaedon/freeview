/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MEDIA_FILE_CACHE_H__
#define __MEDIA_FILE_CACHE_H__

#include <queue>
#include <list>

#include <nrdbase/Mutex.h>
#include <nrdbase/NFErr.h>

#include <nrdase/IMediaConsumer.h>

namespace netflix {

namespace ase {
class HttpRequestManager;
class IAseConfigParameter;
class IMediaFile;
class IStreamingHeuristic;
class ITransportReporter;
class Manifest;
}

namespace mediacontrol {

class MediaFileCache : public ase::IMediaConsumer
{
public:
    MediaFileCache();
    virtual ~MediaFileCache();

public:
    /**
     * Sets the transport reporter for reporting statistic to
     * @param[in] transportReporter
     */
    void setTransportReporter(
        const shared_ptr<ase::ITransportReporter> &transportReporter);

    /**
     * Sets the size of the cache
     *
     * @param[in] maxItems Max number if items to store in cache
     */
    void setCacheSize(uint32_t maxItems);

    /**
     * Tell the cache to prepare for streaming a manifest
     *
     * @param[in] manifest The manifest to prepare to stream
     * @param[in] trackIndices The audio/video tracks to prepare
     * @param[in] streamingHeuristic Object to use to determine which tracks to prepare
     * @return NFErr_OK on success, error otherwise
     */
    NFErr prepare(const shared_ptr<const ase::Manifest> &manifest,
                  const std::vector<uint32_t> &trackIndexes,
                  shared_ptr<ase::IStreamingHeuristic> streamingHeuristic);

    /**
     * Extract MediaFiles from the cache
     *
     * @param[in] movieId The movieId to get files for
     * @param[out] mediaFiles Vector of MediaFile ptr's
     * @return NFErr_OK on success, error otherwise
     */
    NFErr getMediaFiles(const std::string &movieId,
                        /*out*/std::vector<shared_ptr<ase::IMediaFile> > &mediaFiles);

    /**
     * Start/Stop the cache from downloading
     */
    void start();
    void stop();

    /**
     * Flush all entries out of the cache
     */
    void flush();

    /**
     * Get a list of ContentId's in the cache
     */
    std::vector<std::string> getContentList();

public:
    // virtuals from IMediaConsumer
    virtual void configure( const shared_ptr<ase::IAseConfigParameter> &pConfig );
    virtual void performStreamingTasks() {}
    virtual AseErrorCode getNextRequest(uint32_t downloadTrackId,/*out*/ase::MediaRequestPtr& pRequest);
    virtual void getBufferStats(ase::MediaType mediaType,
                                /*out*/ullong& usedBuffer,
                                /*out*/ullong& availableBuffer);
    virtual bool isBufferAvailable(ase::MediaType mediaType);
    virtual void reportFailure(AseErrorCode errorCode, std::string const& message);
    virtual void reportFailure(AseErrorCode errorCode, int32_t httpErr,
                               std::string const& message);
    virtual void pipeliningDisabled();
    virtual void networkFailing(    uint32_t                dwonloadTrackId,
                                    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived );

private:
    AseErrorCode init(const shared_ptr<const ase::Manifest> &manifest,
                      const shared_ptr<ase::IStreamingHeuristic> &heuristic);
    AseErrorCode createMediaFile();
    void addToCache(const std::string &movieId, shared_ptr<ase::IMediaFile> mediaFile);
    void reduceCache(uint32_t maxItemsm);

private:
    Mutex m_mutex;
    bool m_initializing;
    bool m_paused;
    bool m_consumerSet;
    bool m_trackSet;
    uint32_t m_consumerId;
    uint32_t m_trackId;
    uint32_t m_maxItems;

    Mutex m_configMutex;
    shared_ptr<ase::IAseConfigParameter> m_configParameters;

    // queue of requests to be downloaded
    class CacheRequest;
    typedef std::list<shared_ptr<CacheRequest> > CacheRequests;
    CacheRequests m_requests;

    shared_ptr<ase::IMediaFile> m_mediaFile; // media file currently being downloaded
    std::string m_movieId;
    std::string m_streamKey;

    // map of movieId -> vector of IMediaFiles for that movie
    typedef std::map<std::string, std::vector<shared_ptr<ase::IMediaFile> > > CacheMap;
    CacheMap m_cache;

    // list of movieId's that we have MediaFile's for
    std::deque<std::string> m_cacheList;
};

}};


#endif // __MEDIA_FILE_CACHE_H__
