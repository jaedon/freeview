/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaFileCache.h"

#include <nrdbase/ASError.h>
#include <nrdbase/ScopedMutex.h>

#include <nrdase/HttpRequestManager.h>
#include <nrdase/IMediaFile.h>
#include <nrdase/IStreamingHeuristic.h>
#include <nrdase/IStreamSelector.h>
#include <nrdase/NetworkMonitor.h>
#include <nrdase/Manifest.h>
#include <nrdase/MediaFileFactory.h>
#include <nrdase/MediaPresentationRequest.h>

#include "StreamingConfigParameter.h"

using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;

#ifdef BUILD_DEBUG
#define VERBOSE
#endif

#define DEFAULT_CACHE_SIZE 4

//////////
//
// CacheRequest
//
//////////
class MediaFileCache::CacheRequest
{
public:
    shared_ptr<const Manifest> m_manifest;
    shared_ptr<IStreamingHeuristic> m_heuristic;
    shared_ptr<IAseConfigParameter> m_configParameters;

    std::vector<uint32_t> m_trackIndexes;
    int32_t m_currentTrackIndex;
    int32_t m_lastDrmTrack;

    shared_ptr<IMediaFile> m_mediaFile; // media file currently being downloaded

public:
    CacheRequest(const shared_ptr<const Manifest> &manifest,
                 const std::vector<uint32_t> &trackIndexes,
                 const shared_ptr<IStreamingHeuristic> &heuristic,
                 const shared_ptr<IAseConfigParameter> &configParameters)
        : m_manifest(manifest),
          m_heuristic(heuristic),
          m_configParameters(configParameters),
          m_trackIndexes(trackIndexes),
          m_currentTrackIndex(-1),
          m_lastDrmTrack(-1)
    {
    }

    virtual ~CacheRequest()
    {
    }

public:
    std::string contentId()
    {
        return m_manifest->getContentId();
    }

    bool complete()
    {
        return ((size_t)m_currentTrackIndex+1 >= m_trackIndexes.size()) &&
            (m_lastDrmTrack == m_currentTrackIndex);
    }

    ManifestStreamPtr getNextStream(/*out*/uint32_t predictedRebuffers)
    {
        uint32_t trackIndex = 0;
        int32_t streamIndex = -1;
        const std::vector<ManifestTrackPtr> &manifestTracks = m_manifest->getTracks();

        // if we have a track already, see if it has drm streams
        if ((m_currentTrackIndex >= 0) && (m_lastDrmTrack != m_currentTrackIndex))
        {
            trackIndex = m_trackIndexes[m_currentTrackIndex];
            streamIndex = checkDrmStream(trackIndex, /*out*/predictedRebuffers);

            // remember that we checked this track for a drm stream
            m_lastDrmTrack = m_currentTrackIndex;
        }

        // get next stream if necessary
        if (streamIndex == -1)
        {
            // try another track
            ++m_currentTrackIndex;
            if ((size_t)m_currentTrackIndex >= m_trackIndexes.size())
                return ManifestStreamPtr(); // no more tracks

            // get next trackIndex
            trackIndex = m_trackIndexes[m_currentTrackIndex];
            NTRACE(TRACE_MEDIAFILECACHE, "checking next trackIndex %d", trackIndex);

            // make sure this track has streams
            ManifestTrackPtr manifestTrack = manifestTracks[trackIndex];
            if (manifestTrack->getStreams().empty())
                return ManifestStreamPtr(); // no streams

            // use streamSelector to determine what to download
            shared_ptr<IStreamSelector> streamSelector = m_heuristic->getStreamSelector();
            streamSelector->selectStream(trackIndex, manifestTracks,
                                         /*out*/streamIndex, /*out*/predictedRebuffers);


            // if this is an audio track, don't look for a drm track
            if (manifestTrack->getMediaType() == MEDIA_AUDIO)
            {
                NTRACE(TRACE_MEDIAFILECACHE, "audio, no need to look for drm stream");
                m_lastDrmTrack = m_currentTrackIndex;
            }
        }

        // get the ManifestStream
        ManifestTrackPtr manifestTrack = manifestTracks[trackIndex];
        const std::vector<ManifestStreamPtr> &streams = manifestTrack->getStreams();
        ManifestStreamPtr stream = streams[streamIndex];

        NTRACE(TRACE_MEDIAFILECACHE,
               "Request (%s) next stream key %s trackIndex %d streamIndex %d",
               contentId().c_str(), stream->getStreamKey().c_str(),
               trackIndex, streamIndex);

        return stream;
    }

    ILocationSelectorPtr getLocationSelector()
    {
        return m_heuristic->getLocationSelector();
    }

private:
    int32_t checkDrmStream(uint32_t trackIndex, /*out*/uint32_t predictedRebuffers)
    {
        NTRACE(TRACE_MEDIAFILECACHE, "checkDrmStream trackIndex %d", trackIndex);

        // make a copy of tracks
        std::vector<ManifestTrackPtr> tracks = m_manifest->getTracks();
        ManifestTrackPtr track = tracks[trackIndex];

        // enable drm streams in track (if not already)
        std::vector<ManifestStreamPtr> streams = track->getStreams();
        std::vector<ManifestStreamPtr>::iterator sit;
        for (sit = streams.begin(); sit != streams.end(); ++sit)
        {
            ManifestStreamPtr stream = *sit;

            // is this drm?
            ContentProfile profile = stream->getContentProfile();
            if (profileDrmType(profile) != NO_DRM)
            {
                // is it already enabled?
                if (stream->enabled())
                {
                    // drm stream already enabled, nothing to do
                    NTRACE(TRACE_MEDIAFILECACHE,
                           "drm stream already enabled for trackIndex %d", trackIndex);
                    return -1;
                }

                // enable it
                stream->setEnabled(true);
            }
            else
            {
                // disable clear stream
                stream->setEnabled(false);
            }
        }

        // drm streams have now been enabled, call heuristic to find drm stream to use
        int32_t streamIndex;
        shared_ptr<IStreamSelector> streamSelector = m_heuristic->getStreamSelector();
        streamSelector->selectStream(trackIndex, tracks,
                                     /*out*/streamIndex, /*out*/predictedRebuffers);

        NTRACE(TRACE_MEDIAFILECACHE, "selected drm trackIndex %d streamIndex %d",
               trackIndex, streamIndex);

        return streamIndex;
    }
};

//////////
//
// checkEnabled
//
//////////
static bool checkEnabled()
{
    shared_ptr<StreamingConfigParameter> scp(new StreamingConfigParameter);
    std::string param = scp->queryParameter("useMediaFileCache");
    if (param.empty())
        return true; // default to on

    if (param == "0")
        return false;

    char c = param[0];
    if ((c == 'f') || (c == 'F'))
        return false;

    return true;
}

//////////
//
// MediaFileCache
//
//////////
MediaFileCache::MediaFileCache()
    : m_mutex(MEDIAFILECACHE_MUTEX, "MediaFileCache"),
      m_initializing( false ),
      m_paused(false),
      m_consumerSet(false),
      m_trackSet(false),
      m_consumerId(-1),
      m_trackId(-1),
      m_maxItems(DEFAULT_CACHE_SIZE),
      m_configMutex(MEDIAFILECACHECONFIG_MUTEX, "MediaFileCacheConfig")
{
}

MediaFileCache::~MediaFileCache()
{
}

//////////
//
// init
//
//////////
AseErrorCode MediaFileCache::init(const shared_ptr<const Manifest> &manifest,
                                  const shared_ptr<IStreamingHeuristic> &heuristic)
{
    // check to make sure we're enabled
    if (!checkEnabled())
    {
        // if we have anything in the cache, flush it
        if (m_cache.size())
            flush();
        return AS_NOT_SUPPORTED;
    }

    AseErrorCode aserr = AS_NO_ERROR;
    bool consumerSet, trackSet;
    uint32_t consumerId, trackId;
    TrackConfiguration config;

    {
        ScopedMutex lock(m_mutex);

        if ( m_initializing ) return AS_NOT_ALlOWED;

        m_initializing = true;

        consumerSet = m_consumerSet;
        trackSet = m_trackSet;
        consumerId = m_consumerId;
        trackId = m_trackId;

        if ( !trackSet )
        {
            config = heuristic->getConnectionConfiguration()
                                    ->getConfiguration( MEDIA_HEADERS );
        }
    }

    do {
        // register this request as a MediaConsumer
        HttpRequestManager::HttpRequestManagerPtr mgr = HttpRequestManager::instance();
        if (!consumerSet)
        {
            aserr =  mgr->addMediaConsumer(shared_from_this(),
                                           /*out*/consumerId);
            if (aserr != AS_NO_ERROR)
            {
                NTRACE(TRACE_MEDIAFILECACHE, "Unable to add MediaConsumer: %d", aserr);
                break;
            }

            consumerSet = true;
        }

        // add a track to download
        if (!trackSet)
        {
            aserr = mgr->addTrack(consumerId, MEDIA_HEADERS, config, /*out*/trackId);
            if (aserr != AS_NO_ERROR)
            {
                NTRACE(TRACE_MEDIAFILECACHE, "Unable to add Track: %d", aserr);
                break;
            }

            trackSet = true;
        }

        // add serving locations to the mgr
        // Note, that we don't have to worry about removing serving locations from the
        // mgr as there are a limited number of serving locations and the location monitor
        // stores them indexed by location key. (old ones will be replaced with new)
        NetworkMonitor::instance()->addServingLocations(manifest->getServingLocations());

    } while( false );


    {
        ScopedMutex lock(m_mutex);

        m_initializing = false;

        if ( consumerSet )
        {
            m_consumerSet = true;
            m_consumerId = consumerId;
        }

        if ( trackSet )
        {
            m_trackSet = true;
            m_trackId = trackId;
        }
    }

    return aserr;
}

//////////
//
// setConfigParameters
//
//////////
void MediaFileCache::configure(const shared_ptr<IAseConfigParameter> &config)
{
    ScopedMutex lock(m_configMutex);
    m_configParameters = config;
}

//////////
//
// setTransportReporter
//
//////////
void MediaFileCache::setTransportReporter(const shared_ptr<ITransportReporter> &reporter)
{
    // set the transport reporter in HttpRequestManager
    HttpRequestManager::instance()->setTransportReporter(reporter);
}

//////////
//
// setCacheSize
//
//////////
void MediaFileCache::setCacheSize(uint32_t maxItems)
{
    NTRACE(TRACE_MEDIAFILECACHE, "setCacheSize %d", maxItems);
    ScopedMutex lock(m_mutex);

    // reduce cache if necessary
    reduceCache(maxItems);

    m_maxItems = maxItems;
}

//////////
//
// prepare
//
//////////
NFErr MediaFileCache::prepare(const shared_ptr<const Manifest> &manifest,
                              const std::vector<uint32_t> &trackIndexes,
                              shared_ptr<IStreamingHeuristic> heuristic)
{
    // ignore if we cache size is set to 0
    if (m_maxItems == 0)
    {
        NTRACE(TRACE_MEDIAFILECACHE, "prepare ignored because cache max is 0");
        return NFErr_NotAllowed;
    }

    // init cache
    AseErrorCode aserr = init(manifest, heuristic);
    if (aserr != AS_NO_ERROR && aserr != AS_NOT_ALlOWED)
    {
        NTRACE(TRACE_MEDIAFILECACHE, "Unable to init MediaFileCache: %d", aserr);
        return NFErr(new ASError(aserr));
    }

    shared_ptr<IAseConfigParameter> configParams;
    {
        ScopedMutex configLock(m_configMutex);
        configParams = m_configParameters;
    }

    // ignore this request if we already have an entry for the movieId
    std::string movieId = manifest->getContentId();
    {
        ScopedMutex lock(m_mutex);
        if (m_cache.find(movieId) != m_cache.end())
        {
            NTRACE(TRACE_MEDIAFILECACHE, "%s already in cache, ignoring prepare",
                   movieId.c_str());
            return NFErr_OK;
        }
    }

    // create a new CacheRequest for this
    NTRACE(TRACE_MEDIAFILECACHE, "prepare %s", movieId.c_str());
    shared_ptr<CacheRequest> request(
        new CacheRequest(manifest, trackIndexes, heuristic, configParams));

    ScopedMutex lock(m_mutex);
    m_requests.push_back(request);

    return NFErr_OK;
}

//////////
//
// getMediaFiles
//
//////////
NFErr MediaFileCache::getMediaFiles(
    const std::string &movieId, /*out*/std::vector<shared_ptr<IMediaFile> > &mediaFiles)
{
    NTRACE(TRACE_MEDIAFILECACHE, "getMediaFiles for %s", movieId.c_str());

    ScopedMutex lock(m_mutex);

    CacheMap::const_iterator cit = m_cache.find(movieId);
    if (cit == m_cache.end())
    {
        NTRACE(TRACE_MEDIAFILECACHE, "getMediaFiles: empty");
        return NFErr_NotFound;
    }

    mediaFiles = cit->second;

    NTRACE(TRACE_MEDIAFILECACHE, "returning %zu files", mediaFiles.size());

    // once the MediaFile's are requested from the cache, stop requesting for it
    if (m_movieId == movieId)
    {
        NTRACE(TRACE_MEDIAFILECACHE, "dropping current cache request for %s",
               movieId.c_str());
        m_movieId.clear();
        m_mediaFile.reset();
    }

    // clear any cache request for this movieId
    CacheRequests::iterator rit;
    for (rit = m_requests.begin(); rit != m_requests.end();)
    {
        if ((*rit)->contentId() == movieId)
        {
            CacheRequests::iterator toremove = rit++;
            m_requests.erase(toremove);
        }
        else
            ++rit;
    }

    // once we've returned these files, discard them from cache
    m_cache.erase(movieId);

    return NFErr_OK;
}

//////////
//
// start/stop
//
//////////
void MediaFileCache::start()
{
    NTRACE(TRACE_MEDIAFILECACHE, "start");

    {
        ScopedMutex lock(m_mutex);

        if (m_initializing || !m_paused) return;

        m_paused = false;
    }

    HttpRequestManager::instance()->resumeTrack(m_trackId);
}

void MediaFileCache::stop()
{
    NTRACE(TRACE_MEDIAFILECACHE, "stop");
    {
        ScopedMutex lock(m_mutex);

        if (m_initializing || m_paused) return;
    }

    m_paused = true;

    HttpRequestManager::instance()->pauseTrack(m_trackId);
}

//////////
//
// flush
//
//////////
void MediaFileCache::flush()
{
    NTRACE(TRACE_MEDIAFILECACHE, "flush");

    ScopedMutex lock(m_mutex);

    m_cache.clear();
    m_cacheList.clear();

    m_requests.clear();
    m_movieId.clear();
    m_mediaFile.reset();
}

//////////
//
// getContentList
//
//////////
std::vector<std::string> MediaFileCache::getContentList()
{
    NTRACE(TRACE_MEDIAFILECACHE, "getContentList: %zu", m_cache.size());

#ifdef VERBOSE
    std::string strlist;
#endif

    ScopedMutex lock(m_mutex);

    std::vector<std::string> list;
    CacheMap::const_iterator it;
    for (it = m_cache.begin(); it != m_cache.end(); ++it)
    {
        list.push_back(it->first);
#ifdef VERBOSE
        strlist += " " + it->first;
#endif
    }

#ifdef VERBOSE
    NTRACE(TRACE_MEDIAFILECACHE, "list: %s", strlist.c_str());
#endif

    return list;
}

//////////
//
// getNextRequest
//
//////////
AseErrorCode MediaFileCache::getNextRequest(uint32_t downloadTrackId, /*out*/MediaRequestPtr &mediaRequest)
{
    (void)downloadTrackId;
    AseErrorCode aserr = AS_NO_ERROR;

    ScopedMutex lock(m_mutex);

    if ( m_initializing ) return AS_PENDING;

    // if we have a media file, see if it's done
    if (m_mediaFile.get() && m_mediaFile->indexComplete())
    {
        NTRACE(TRACE_MEDIAFILECACHE, "completed mediaFile %s for movie %s",
               m_mediaFile->getStreamKey().c_str(), m_movieId.c_str());

        // we have finished a media file
        m_movieId.clear();
        m_mediaFile.reset();

        // any more work for this request?
        shared_ptr<CacheRequest> request = m_requests.front();
        if (request->complete())
        {
            // erase that request
            NTRACE(TRACE_MEDIAFILECACHE, "request done");
            m_requests.pop_front();
        }
    }

    // create next media file if necessary
    if (!m_mediaFile.get())
    {
        // make sure we have a request queued
        if (m_requests.empty())
            return AS_PENDING;

        aserr = createMediaFile();
        if (aserr == AS_NOT_FOUND)
        {
            NTRACE(TRACE_MEDIAFILECACHE,
                   "Warning: unable to create media file from request, ignoring it");
            m_requests.pop_front();
            return AS_PENDING;
        }
        if (aserr != AS_NO_ERROR)
        {
            m_requests.pop_front(); // throw away request if we can't create a media file
            return aserr;
        }
    }

    // forward getNextRequest to the media file
    MediaPresentationRequestPtr pMediaRequest;
    aserr = m_mediaFile->getNextRequest(/*out*/pMediaRequest);
    if (aserr == AS_MEDIA_FILE_REQUEST_PENDING)
        return aserr;   // mediaFile not ready
    if (aserr != AS_NO_ERROR)
    {
        NTRACE(TRACE_MEDIAFILECACHE, "Failed to get request from MediaFile: %d", aserr);
        return aserr;
    }

    // set the remembered fields on the MediaRequest
    // XXX - this should really be set in MediaFile::getNextRequest()
    pMediaRequest->setDownloadTrackId(m_trackId);
    pMediaRequest->setLocationSelector(m_requests.front()->getLocationSelector());

    mediaRequest = pMediaRequest;

    NTRACE(TRACE_MEDIAFILECACHE,
           "getNextRequest returns movie: %s requestTrack: %d key: %s",
           m_movieId.c_str(), m_trackId, m_streamKey.c_str());

    return aserr;
}

//////////
//
// other virtuals
//
//////////
void MediaFileCache::getBufferStats(MediaType /*mediaType*/,
                                    /*out*/ullong& usedBuffer,
                                    /*out*/ullong& availableBuffer)
{
    NTRACE(TRACE_MEDIAFILECACHE, "getBufferStats");
    usedBuffer = 0;
    availableBuffer = 0;
}

bool MediaFileCache::isBufferAvailable(MediaType /*mediaType*/)
{
    return false;
}

void MediaFileCache::reportFailure(AseErrorCode errorCode, std::string const& message)
{
    NRDP_UNUSED(errorCode);
    NRDP_UNUSED(message);
    NTRACE(TRACE_MEDIAFILECACHE, "reportFailure1 %d %s", errorCode, message.c_str());
}

void MediaFileCache::reportFailure(AseErrorCode errorCode, int32_t httpErr,
                                   std::string const& message)
{
    NRDP_UNUSED(errorCode);
    NRDP_UNUSED(httpErr);
    NRDP_UNUSED(message);
    NTRACE(TRACE_MEDIAFILECACHE, "reportFailure2 %d %d %s", errorCode, httpErr,
           message.c_str());
}

void MediaFileCache::pipeliningDisabled()
{
    // Need to implement: update our stored state to say that pipelining is disabled
}

void MediaFileCache::networkFailing(    uint32_t                dwonloadTrackId,
                                        AseErrorCode            latestStatus,
                                        AseUrl::HostPort const& destination,
                                        AseTimeVal const*       pLatestErrorTime,
                                        AseTimeVal const&       latestDataReceived )
{
    NRDP_UNUSED(dwonloadTrackId);
    NRDP_UNUSED(latestStatus);
    NRDP_UNUSED(destination);
    NRDP_UNUSED(pLatestErrorTime);
    NRDP_UNUSED(latestDataReceived);
}

//////////
//
// createMediaFile
//
//////////
AseErrorCode MediaFileCache::createMediaFile()
{
    // get next request
    shared_ptr<CacheRequest> request = m_requests.front();
    if (request->complete())
    {
        NTRACE(TRACE_MEDIAFILECACHE, "createMediaFile() but request complete");
        return AS_PENDING;
    }

    // get the next stream to download
    uint32_t predictedRebuffers = 0;
    ManifestStreamPtr stream = request->getNextStream(/*out*/predictedRebuffers);
    if (!stream.get())
        return AS_NOT_FOUND;
    m_streamKey = stream->getStreamKey();

    // XXX do something with predictedRebuffers

    shared_ptr<IAseConfigParameter> configParams;
    {
        ScopedMutex configLock(m_configMutex);
        configParams = m_configParameters;
    }

    // create a MediaFile on that stream
    AseErrorCode aserr = MediaFileFactory::createMediaFile(configParams, stream,
                                                           /*out*/m_mediaFile);
    if (aserr != AS_NO_ERROR)
    {
        NTRACE(TRACE_MEDIAFILECACHE,
               "MediaFileCache failed to create MediaFile: %d", aserr);
        return aserr;
    }

    // add this file to cache
    m_movieId = request->contentId();
    addToCache(m_movieId, m_mediaFile);

    NTRACE(TRACE_MEDIAFILECACHE, "adding mediaFile for %s", m_movieId.c_str());

    return AS_NO_ERROR;
}

//////////
//
// addToCache
//
//////////
void MediaFileCache::addToCache(const std::string &movieId,
                                shared_ptr<IMediaFile> mediaFile)
{
    ScopedMutex lock(m_mutex);

    // if movieid is already in cache, just add the mediaFile
    CacheMap::iterator it = m_cache.find(movieId);
    if (it != m_cache.end())
    {
        (it->second).push_back(mediaFile);
        return;
    }

    // make room in cache if needed
    if (m_cache.size() + 1 > m_maxItems)
        reduceCache(m_maxItems - 1);

    std::vector<shared_ptr<IMediaFile> > &list = m_cache[movieId];
    list.push_back(mediaFile);

    // add movieId to queue of items
    m_cacheList.push_back(movieId);

    NTRACE(TRACE_MEDIAFILECACHE, "cache size is now %d, %d",
           (int)m_cache.size(), (int)m_cacheList.size());
}

//////////
//
// reduceCache
//
//////////
void MediaFileCache::reduceCache(uint32_t maxItems)
{
    // should be called with lock already, but grab it just in case
    ScopedMutex lock(m_mutex);

    NTRACE(TRACE_MEDIAFILECACHE, "reduceCache to %d (%zu)", maxItems, m_cache.size());

    // remove the oldest item
    while (m_cache.size() > maxItems)
    {
        std::string movieId = m_cacheList.front();
        m_cacheList.pop_front();
        m_cache.erase(movieId);
        NTRACE(TRACE_MEDIAFILECACHE, "erased %s", movieId.c_str());
    }
}
