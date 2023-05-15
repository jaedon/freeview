/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StreamingManager.h"
#include "HttpRequestManager.h"
#include "MediaPresentation2.h"
#include "MediaFileFactory.h"
#include "NetworkMonitor.h"

#include <nrdbase/Log.h>

using namespace netflix::ase;
using namespace std;
using namespace netflix;

/**
 * TODO
 * 1. Add Mutex protection here
 */

static HttpHeaders sExtraHttpHeaders;

int32_t StreamingManager::configure(    IPersistentStorePtr     pPersistentStore,
                                        IAseConfigParameterPtr  pConfigParameter )
{
    static bool bHttpRequestManagerStarted = false;

    int32_t retCode = HttpRequestManager::instance()->configure( pConfigParameter, pPersistentStore );
    if ( retCode != AS_NO_ERROR )
        return retCode;

    NetworkMonitor::instance()->configure( pConfigParameter, pPersistentStore );

    if ( !bHttpRequestManagerStarted )
    {
        retCode = HttpRequestManager::instance()->start();
        if ( retCode != AS_NO_ERROR )
            return retCode;

        DEBUGLOG( MediaPresentationLog, "Http Request Manager started" );

        bHttpRequestManagerStarted = true;
    }

    return AS_NO_ERROR;
}

int32_t StreamingManager::deinitialize()
{
    DEBUGLOG( MediaPresentationLog, "StreamingManager::deinitialize" );

    (void) HttpRequestManager::instance()->stop();

    (void) NetworkMonitor::instance()->close();

    return AS_NO_ERROR;
}

void StreamingManager::addExtraHttpHeader( const std::string& key, const std::string& value )
{
    sExtraHttpHeaders[ key ] = value;

    HttpRequestManager::instance()->setExtraRequestHeaders( sExtraHttpHeaders );
}

int StreamingManager::construct(ManifestPtr pManifest,
                          IAseConfigParameterPtr const& pConfigParameter,
                          IAseConfigParameterPtr pLocalConfigParameter,
                          IStreamingHeuristicPtr const& pStreamingHeuristic,
                          IMediaSinkPtr const& pMediaSink,
                          IMediaBufferPoolPtr const& pMediaBufferPool,
                          IStreamingReporterPtr const& pStreamingReporter,
                          ITransportReporterPtr const& pTransportReporter,
                          IPersistentStorePtr const& pPersistentStore,
                          bool fastStart,
                          StreamingManagerPtr& pStreamingManager )
{
    configureLoggers( *pConfigParameter );

    pStreamingManager.reset(new StreamingManager( pManifest,
                                                  pMediaSink, pMediaBufferPool,
                                                  pStreamingReporter, pTransportReporter,
                                                  pStreamingHeuristic,
                                                  pConfigParameter, pLocalConfigParameter,
                                                  fastStart,
                                                  pPersistentStore) );
    pStreamingManager->createMediaPresentation();
    return AS_NO_ERROR;
}

bool StreamingManager::canPlayType( MimeType const& mime )
{
    return MediaFileFactory::canPlayType( mime );
}

StreamingManager::StreamingManager(ManifestPtr pManifest,
                          IMediaSinkPtr const& pMediaSink,
                          IMediaBufferPoolPtr const& pMediaBufferPool,
                          IStreamingReporterPtr const& pStreamingReporter,
                          ITransportReporterPtr const& pTransportReporter,
                          IStreamingHeuristicPtr const& pStreamingHeuristic,
                          IAseConfigParameterPtr const& pConfigParameter,
                          IAseConfigParameterPtr pLocalConfigParameter,
                          bool fastStart,
                          IPersistentStorePtr const& pPersistentStore) :
                    mMediaPresentationPtr( MediaPresentationPtr() ),
                    mCurrentManifestPtr(pManifest),
                    mMediaSinkPtr(pMediaSink),
                    mMediaBufferPoolPtr(pMediaBufferPool),
                    mStreamingReporterPtr(pStreamingReporter),
                    mTransportReporterPtr(pTransportReporter),
                    mStreamingHeuristicsPtr(pStreamingHeuristic),
                    mConfigParamterPtr(pConfigParameter),
                    mLocalConfigParametePtr(pLocalConfigParameter),
                    mPersistentStorePtr(pPersistentStore),
                    mVideoTrackId(0),
                    mAudioTrackId(0),
                    mHeadersTrackId(0),
                    mState(STOPPED),
                    mFastStart(fastStart),
                    mManifestSwitched(false),
                    mFirstAudioSwitch(true),
                    mVideoTrackDownloaderPaused(false)
{
    DEBUGLOG(MediaPresentationLog, "fast start: %u", mFastStart);
    if ( mFastStart )
    {
        pManifest->setFinalPeriod(false);
    }
    else
    {
        pManifest->setFinalPeriod(true);
    }

    mManifestPtrs.push_back(pManifest);

    mStreamingHeuristicsPtr->
        getConnectionConfiguration()->
            configure( mConfigParamterPtr );
}


AseErrorCode StreamingManager::open( std::vector<uint32_t> const& trackIndices,
                                     vector<shared_ptr<IMediaFile> > const& mediaFiles)
{
    AseErrorCode retCode = AS_NO_ERROR;

    do {

        mTrackIndexes = trackIndices;
        retCode = mMediaPresentationPtr->open( trackIndices, mFastStart, mediaFiles );
        if ( retCode != AS_NO_ERROR )
            break;

        DEBUGLOG( MediaPresentationLog, "Media Presentation open returned" );

        // Pause the Media Presentation while we set up the tracks
        mMediaPresentationPtr->pause();

        retCode = startHttpRequestManager();
        if( retCode != AS_NO_ERROR )
            break;

        mVideoManifestTrack = getManifestTrack( MEDIA_VIDEO );
        if ( !mVideoManifestTrack )
        {
            DEBUGLOG(MediaPresentationLog, "could not find video track...");
            retCode = AS_NOT_FOUND;
            break;
        }

        retCode = addDownloadTrack( MEDIA_VIDEO, mVideoTrackId );
        if ( retCode != AS_NO_ERROR )
        {
            DEBUGLOG(MediaPresentationLog, "failed to addDownloadTrack in open (1), closing...");
            break;
        }

        mAudioManifestTrack = getManifestTrack( MEDIA_AUDIO );
        if ( !mAudioManifestTrack )
        {
            DEBUGLOG(MediaPresentationLog, "could not find audio track in open()...");
        }
        else
        {
            retCode = addDownloadTrack( MEDIA_AUDIO, mAudioTrackId );
            if ( retCode != AS_NO_ERROR )
            {
                DEBUGLOG(MediaPresentationLog, "failed to addDownloadTrack in open (2), closing...");
                break;
            }
        }

        mMediaPresentationPtr->updateDownloaderTrackId(mVideoTrackId, mAudioTrackId, mHeadersTrackId);

        DEBUGLOG( MediaPresentationLog, "Provided track ids to Media Presentation (%u,%u,%u)",
                                            mAudioTrackId,
                                            mVideoTrackId,
                                            mHeadersTrackId);

        mMediaPresentationPtr->resume();

    } while( false );

    if ( retCode != AS_NO_ERROR )
    {
        close();
    }

    checkError( retCode );
    return retCode;
}

AseErrorCode StreamingManager::startHttpRequestManager()
{
    int32_t retCode = HttpRequestManager::instance()->
                addMediaConsumer(   mMediaPresentationPtr,
                                    mMediaConsumerId );
    if ( retCode != AS_NO_ERROR )
        return retCode;

    (void) HttpRequestManager::instance()->setTransportReporter( mTransportReporterPtr );
    (void) networkChange( mNetworkInterfaceInfo );
    (void) externalIpAddressChange( mClientExternalIpAddress );

    retCode = addDownloadTrack( MEDIA_HEADERS, mHeadersTrackId );
    if ( retCode != AS_NO_ERROR )
    {
        DEBUGLOG(MediaPresentationLog, "failed to addDownloadTrack in startHttpRequestManager, closing...");
        return retCode;
    }

    return retCode;
}

AseErrorCode StreamingManager::addManifest(ManifestPtr pManifest)
{
    DEBUGLOG(MediaPresentationLog, "addManifest");
    pManifest->setFinalPeriod(true);
    pManifest->setRequireLicense(true);
    mManifestPtrs.push_back(pManifest);
    mStreamingHeuristicsPtr->getLocationSelector()->
        addManifest(pManifest);
    return mMediaPresentationPtr->addManifest(pManifest);
}

void StreamingManager::notifyLicenseAcquired( uint32_t manifestIndex )
{
    mMediaPresentationPtr->notifyLicenseAcquired(manifestIndex);
}

void StreamingManager::notifyManifestSelected( uint32_t manifestIndex )
{
#ifdef BUILD_DEBUG
    bool validManifestIndex = false;
#endif
    for ( vector<ManifestPtr>::iterator iter = mManifestPtrs.begin();
            iter != mManifestPtrs.end();
            ++iter )
    {
        ManifestPtr pManifest = *iter;
        if ( pManifest->getManifestIndex() == manifestIndex )
        {
            mCurrentManifestPtr = pManifest;
#ifdef BUILD_DEBUG
            validManifestIndex = true;
#endif
            break;
        }
    }
#ifdef BUILD_DEBUG
    assert(validManifestIndex == true);
#endif
    mManifestSwitched = true;
}


/**
 * The start sequence should be
 * MediaPresentation start --> HttpRequestManager add tracks
 */
AseErrorCode StreamingManager::start( AseTimeStamp const& seekTime )
{
    AseErrorCode retCode = AS_NO_ERROR;
    if ( mState == STARTED )
        return retCode;

    do {
        retCode = addAndRemoveTracks();
        if ( retCode != AS_NO_ERROR )
        {
            DEBUGLOG(MediaPresentationLog, "failed to addAndRemoveTracks, closing...");
            break;
        }

        mState = STARTED;

        retCode = mMediaPresentationPtr->start(seekTime);
        if ( retCode != AS_NO_ERROR )
        {
            DEBUGLOG(MediaPresentationLog, "Media presentation failed to start");
            break;
        }

        DEBUGLOG(MediaPresentationLog, "Media presentation started");
        DEBUGLOG(MediaPresentationLog, "StreamingManager started");

    } while( false );

    if ( retCode != AS_NO_ERROR )
        close();

    checkError( retCode );
    return retCode;
}

AseErrorCode StreamingManager::addDownloadTrack( MediaType type, uint32_t& trackId )
{
    //Add a track to HTTP request manager, set open range flag based on
    //the media type.
    TrackConfiguration const& config = mStreamingHeuristicsPtr->
                                            getConnectionConfiguration()->
                                                getConfiguration( type );

    AseErrorCode retCode = HttpRequestManager::instance()->addTrack( mMediaConsumerId, type, config, trackId );
    mTrackConfigurations[trackId] = config;

    DEBUGLOG(MediaPresentationLog,"StreamingManager::addDownloadTrack type %u track %u",
                type, trackId );

    PERFTRACE( "create a track downloader: track id: %u, media type: %u, time: %llu\n",
            trackId, type, Time::mono().val());

    return retCode;
}

/**
 * The stop sequence should be
 * HttpRequestManager stop --> MediaPresentation stop
 */
void StreamingManager::stop()
{
    DEBUGLOG(MediaPresentationLog, "StreamingManager::stopping (state=%d)", mState);

    if ( mState == STARTED )
    {
        mState = STOPPING;

        HttpRequestManager::instance()->removeTrack( mAudioTrackId );
        HttpRequestManager::instance()->removeTrack( mVideoTrackId );
        HttpRequestManager::instance()->removeTrack( mHeadersTrackId );
        mTrackConfigurations.erase(mAudioTrackId);
        mTrackConfigurations.erase(mVideoTrackId);
        mTrackConfigurations.erase(mHeadersTrackId);

        mAudioTrackId = 0;
        mVideoTrackId = 0;
        mHeadersTrackId = 0;

        mVideoTrackDownloaderPaused = false;

        mVideoManifestTrack.reset();
        mAudioManifestTrack.reset();

        if (mMediaPresentationPtr.get())
        {
            mMediaPresentationPtr->stop();
            DEBUGLOG(MediaPresentationLog, "stopped media presentation");
        }

        if ( mManifestSwitched )
        {
            mFastStart = false;
            DEBUGLOG(MediaPresentationLog, "disable fast start flag");
        }

        mState = STOPPED;
        DEBUGLOG(MediaPresentationLog, "StreamingManager::stopped");
    }
}

void StreamingManager::skip()
{
    if (mMediaPresentationPtr.get())
    {
        mMediaPresentationPtr->skip();
        DEBUGLOG(MediaPresentationLog, "media presentation skips");
    }
}

void StreamingManager::close()
{
    // make sure we are stopped
    stop();

    HttpRequestManager::instance()->removeMediaConsumer( mMediaConsumerId );

    if (mMediaPresentationPtr.get())
    {
        mMediaPresentationPtr->close();
        mMediaPresentationPtr = MediaPresentationPtr();

        Variant state;
        getNetworkMonitorState( &state );

        mTransportReporterPtr->reportNetworkMonitorState( state );
    }

    DEBUGLOG(MediaPresentationLog, "StreamingManager closed");
}

void StreamingManager::setStreamingSpeed(uint32_t speed)
{
    if (mMediaPresentationPtr.get())
    {
        mMediaPresentationPtr->setStreamingSpeed(speed);
    }
}

AseErrorCode StreamingManager::switchTracks( std::vector<uint32_t> const& trackIndexes )
{
    AseErrorCode retCode = AS_NO_ERROR;
    DEBUGLOG( MediaPresentationLog, "StreamingManager::switchTracks");

    if ( trackIndexes == mTrackIndexes )
    {
        DEBUGLOG( MediaPresentationLog, "...same tracks as current");
        return AS_NO_ERROR;
    }

    mTrackIndexes = trackIndexes;
    if ( mState == STARTED )
    {
        mMediaPresentationPtr->pause();
        retCode = switchAudioTrackDownloaders();
        checkError(retCode);
        DEBUGLOG( MediaPresentationLog, "On-the-fly audio track switch: prepare to switch, "
                    "time=%llu ms", AseTimeVal::now().ms());
        if (retCode == AS_NO_ERROR)
        {
            // It's tricky here. We need to update the downloader track id after MediaPresentation switches the track.
            // Otherwise the old audio track in MediaPresentation will send out request to the new downloader track for audio,
            // and when the downloader track receives the data, the old audio track will be gone.
            mMediaPresentationPtr->updateDownloaderTrackId(mVideoTrackId, mAudioTrackId, mHeadersTrackId);
            DEBUGLOG( MediaPresentationLog, "Provided track ids to Media Presentation (%u,%u,%u)",
                                                mAudioTrackId,
                                                mVideoTrackId,
                                                mHeadersTrackId);
            retCode = HttpRequestManager::instance()->pauseTrack(mVideoTrackId);
            mVideoTrackDownloaderPaused = true;
            checkError(retCode);
            DEBUGLOG( MediaPresentationLog, "On-the-fly audio track switch: pause video track, "
                    "time=%llu ms", AseTimeVal::now().ms());
        }

    }

    if ( retCode == AS_NO_ERROR )
        retCode = mMediaPresentationPtr->switchTracks(trackIndexes);

    checkError(retCode);
    return retCode;
}

AseErrorCode StreamingManager::rebuffer()
{
   return mMediaPresentationPtr->rebuffer();
}

AseErrorCode StreamingManager::addAndRemoveTracks(  MediaType           type,
                                                    ManifestTrackPtr&   currentManifestTrack,
                                                    uint32_t&           currentTrackId )
{
    // The purpose of this function is to ensure that the
    // tracks for media type in mTrackIndexes matches the current track
    // and if not create a new track downloader

    AseErrorCode retCode = AS_NO_ERROR;

    // Get the manifest track for media type type, using mTrackIndexes
    ManifestTrackPtr manifestTrack = getManifestTrack( type );

    if ( !compareTracks( manifestTrack, currentManifestTrack ) )
    {
        if( currentManifestTrack )
        {
            retCode = HttpRequestManager::instance()->removeTrack( currentTrackId );
            if ( retCode != AS_NO_ERROR )
                return retCode;
            mTrackConfigurations.erase(currentTrackId);

            currentManifestTrack.reset();
        }

        if( manifestTrack )
        {
            retCode = addDownloadTrack( type, currentTrackId );
            if ( retCode != AS_NO_ERROR )
                return retCode;
        }

        currentManifestTrack = manifestTrack;
    }

    return retCode;
}


AseErrorCode StreamingManager::addAndRemoveTracks()
{
    // The purpose of this function is to ensure that the HttpRequestManager
    // audio and video tracks match the tracks referred to in mTrackIndexes
    if ( HttpRequestManager::instance() == NULL )
        return AS_NOT_ALlOWED;

    mMediaPresentationPtr->pause();

    AseErrorCode retCode;

    if ( mHeadersTrackId == 0 )
    {
        retCode = addDownloadTrack( MEDIA_HEADERS, mHeadersTrackId );
        if ( retCode != AS_NO_ERROR )
        {
            DEBUGLOG(MediaPresentationLog, "failed to add HEADERS track in addAndRemoveTracks...");
            return retCode;
        }
    }

    retCode = addAndRemoveTracks( MEDIA_AUDIO, mAudioManifestTrack, mAudioTrackId );
    if ( retCode != AS_NO_ERROR )
        return retCode;

    retCode = addAndRemoveTracks( MEDIA_VIDEO, mVideoManifestTrack, mVideoTrackId );
    if ( retCode != AS_NO_ERROR )
        return retCode;

    mMediaPresentationPtr->updateDownloaderTrackId( mVideoTrackId, mAudioTrackId, mHeadersTrackId);
    DEBUGLOG( MediaPresentationLog, "Provided track ids to Media Presentation (%u,%u,%u)",
                                    mAudioTrackId,
                                    mVideoTrackId,
                                    mHeadersTrackId);

    mMediaPresentationPtr->resume();

    return retCode;
}

AseErrorCode StreamingManager::switchAudioTrackDownloaders()
{
    AseErrorCode retCode = AS_NO_ERROR;
    DEBUGLOG(MediaPresentationLog, "switchAudioTrackDownloaders: fastStart: %u, firstAudioSwitch: %u",
                        mFastStart, mFirstAudioSwitch);


    retCode = addAndRemoveTracks( MEDIA_AUDIO, mAudioManifestTrack, mAudioTrackId );
    if ( retCode != AS_NO_ERROR )
        return retCode;

    mFirstAudioSwitch = false;

    DEBUGLOG(MediaPresentationLog, "Switched audio track. New track id %d", mAudioTrackId );

    return retCode;
}


void StreamingManager::checkError(AseErrorCode errorCode)
{
    if ( (errorCode != AS_NO_ERROR) && (errorCode != AS_NOT_ALlOWED) )
    {
        if ( errorCode == AS_MEDIA_DURATION_ERROR )
        {
            DEBUGLOG(MediaPresentationLog, "StreamingManager report AS_MEDIA_DURATION_ERROR");
            mStreamingReporterPtr->streamingFailure(
                    mCurrentManifestPtr->getManifestIndex(),
                    -1, -1, AS_MEDIA_DURATION_ERROR, 0, "");
        }
        else
        {
            DEBUGLOG(MediaPresentationLog, "StreamingManager report AS_INIT_FAILURE");
            mStreamingReporterPtr->streamingFailure(
                    mCurrentManifestPtr->getManifestIndex(),
                    -1, -1, AS_INIT_FAILURE, 0, "");
        }
    }
}

void StreamingManager::setVideoBitrateRanges(vector< shared_ptr<BitrateRange> > const& videoBitrateRanges)
{
    mMediaPresentationPtr->setVideoBitrateRanges(videoBitrateRanges);
}

void StreamingManager::setStreamingBuffer(bool powerSaving,
                                          uint32_t maxBufferLen,
                                          uint32_t minBufferLen)
{
    mMediaPresentationPtr->setStreamingBuffer(powerSaving, maxBufferLen, minBufferLen);
}

uint32_t StreamingManager::getMaxStreamingBuffer() const
{
    return mMediaPresentationPtr->getMaxStreamingBuffer();
}

bool StreamingManager::compareTracks( ManifestTrackPtr pManifestTrack, ManifestTrackPtr pManifestTrack2 )
{
    if ( !pManifestTrack || !pManifestTrack2 )
        return false;

    MediaType const type = pManifestTrack->getMediaType();
    return ( ( pManifestTrack2->getMediaType() == type )
        && ( ( type == MEDIA_VIDEO && pManifestTrack->isStereoVideo() == pManifestTrack2->isStereoVideo() )
            || ( type == MEDIA_AUDIO && pManifestTrack->getTrackKey() == pManifestTrack2->getTrackKey() ) ) );
}

ManifestTrackPtr StreamingManager::getManifestTrack( MediaType type ) const
{
    vector<ManifestTrackPtr> const& manifestTracks = mCurrentManifestPtr->getTracks();
    vector<uint32_t>::const_iterator it = mTrackIndexes.begin();
    for ( ; it != mTrackIndexes.end(); ++it )
    {
        ManifestTrackPtr pManifestTrack = manifestTracks[*it];
        if (pManifestTrack->getMediaType() == type)
        {
            return pManifestTrack;
        }
    }

    return ManifestTrackPtr();
}

void StreamingManager::createMediaPresentation()
{
    MediaPresentation::construct( shared_from_this(),
                                  mCurrentManifestPtr,
                                  mMediaSinkPtr,
                                  mMediaBufferPoolPtr, mStreamingHeuristicsPtr,
                                  mConfigParamterPtr, mLocalConfigParametePtr,
                                  mStreamingReporterPtr,
                                  mPersistentStorePtr,
                                  mMediaPresentationPtr);
}

void StreamingManager::resumeVideoTrackDownloader()
{
    if ( mVideoTrackDownloaderPaused )
    {
        HttpRequestManager::instance()->resumeTrackAsync(mVideoTrackId);
        mVideoTrackDownloaderPaused = false;
    }
}

void StreamingManager::networkChange( NetworkInterfaceInfo const& netinfo )
{
    mNetworkInterfaceInfo = netinfo;
    (void) NetworkMonitor::instance()->networkChange( mNetworkInterfaceInfo );
}

void StreamingManager::externalIpAddressChange( std::string const& ipaddr )
{
    mClientExternalIpAddress = ipaddr;
    (void) NetworkMonitor::instance()->externalIpAddressChange( mClientExternalIpAddress );
}

void StreamingManager::getNetworkMonitorState( Variant* pState )
{
    (void) NetworkMonitor::instance()->getState( pState );
}

void StreamingManager::setNetworkMonitorState( Variant const& state )
{
    (void) NetworkMonitor::instance()->setState( state );
}

void StreamingManager::setNetworkMonitorListener( INetworkMonitorListenerPtr pNetworkMonitorListener )
{
    (void) NetworkMonitor::instance()->setNetworkMonitorListener( pNetworkMonitorListener );
}

