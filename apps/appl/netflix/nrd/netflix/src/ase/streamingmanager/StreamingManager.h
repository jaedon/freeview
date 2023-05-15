/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef STREAMING_MANAGER_H
#define STREAMING_MANAGER_H

#include <nrdase/Manifest.h>
#include <nrdase/IMediaSink.h>
#include <nrdase/AseTimeStamp.h>
#include <nrdase/IMediaBufferPool.h>
#include <nrdase/IStreamingHeuristic.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdase/IStreamingReporter.h>
#include <nrdase/ITransportReporter.h>
#include <nrdase/IPersistentStore.h>
#include <nrdase/BitrateRange.h>
#include <nrdase/IMediaFile.h>

#include <nrdbase/NetworkInterfaceInfo.h>

namespace netflix {
namespace ase {

class MediaPresentation;
typedef shared_ptr<MediaPresentation> MediaPresentationPtr;

class HttpRequestManager;
typedef HttpRequestManager* HttpRequestManagerPtr;

class INetworkMonitorListener;
typedef shared_ptr<INetworkMonitorListener> INetworkMonitorListenerPtr;

/**
 * @class StreamingManager
 * @brief
 */
class StreamingManager : public enable_shared_from_this<StreamingManager>
{
public:
    typedef shared_ptr<StreamingManager> StreamingManagerPtr;

    /** Configure static data / threads */
    static int32_t configure(   IPersistentStorePtr     pPersistentStore,
                                IAseConfigParameterPtr  pConfigParameter );

    /** Deinitialize static data / threads */
    static int32_t deinitialize();

    /** Add an extra HTTP request header. */
    static void addExtraHttpHeader( const std::string& key, const std::string& value );

    /** construct the streaming manager
     * @param[in] pManifest The media manifest
     * @param[in] pStreamingHeuristic the streaming heuristic
     * @param[in] pMediaSink The media sink accepting a/v data
     * @param[in] pMediaBufferPool The buffer allocator for media, i.e. audio and video
     * @param[in] pStreamingReporter reporter for streaming related events and metrics
     * @param[in] pTransportReporter reporter for transport metrics
     * @param[in] fastStart: fast start mode
     * @param[out] pStreamingManager The new streaming manager object
     * @return 0 success, negative error codes to be defined
     */
    static int construct( ManifestPtr pManifest,
                          IAseConfigParameterPtr const& pConfigParameter,
                          IAseConfigParameterPtr pLocalConfigParameter,
                          IStreamingHeuristicPtr const& pStreamingHeuristic,
                          IMediaSinkPtr const& pMediaSink,
                          IMediaBufferPoolPtr const& pMediaBufferPool,
                          IStreamingReporterPtr const& pStreamingReporter,
                          ITransportReporterPtr const& pTransportReporter,
                          IPersistentStorePtr const& pPersistentStore,
                          bool fastStart,
                          StreamingManagerPtr& pStreamingManager );

    ~StreamingManager()
    {
        close();
    }

    /** Check if StreamingManager supports a given MIME type
     *  @param[in] mime The MIME type
     *  @return true if supported, false if not
     */
    static bool canPlayType( MimeType const& mime );

    // NFCOMMENT: open, start, are asynchronous

    /** Open the Streaming Manager with a default set of active tracks
     *  @param trackIndices [in] The indices of the initially active tracks
     */
    AseErrorCode open( std::vector<uint32_t> const& trackIndices,
                       std::vector<shared_ptr<IMediaFile> > const& mediaFiles);

    /**
     * Add a new period to the playback session
     * @param[in] pManifest the newly added manifest period to the playback
     * @return
     */
    AseErrorCode addManifest(ManifestPtr pManifest);

    /** Start Streaming
     *  @param seekTime [in] The seek time
     */
    AseErrorCode start( AseTimeStamp const& seekTime );

    void stop();

    void skip();

    void close();

    void setStreamingSpeed(uint32_t speed);

    /**
     * Notify streaming manager the license for the given period is acquired
     */
    void notifyLicenseAcquired( uint32_t manifestIndex );

    /**
     * Notify streaming manager a given period is selected
     */
    void notifyManifestSelected( uint32_t manifestIndex );

    /**
     * switch to the newly selected tracks
     * FIXME: explicitly disable this feature when the player is in PERIOD mode?
     * @param[in] trackIndexes the indexes of the newly selected tracks
     * @return
     */
    AseErrorCode switchTracks( std::vector<uint32_t> const& trackIndexes );

    void resumeVideoTrackDownloader();

    AseErrorCode rebuffer();

    /** set the available video bitrate ranges */
    void setVideoBitrateRanges(std::vector< shared_ptr<BitrateRange> > const& videoBitrateRanges);

    /**
     * dynamically adjust maximum/minimum streaming buffer length
     *
     * @param[in] powerSaving: whether the player in power saving mode
     * @param[in] maxBufferLen: maximum buffer length in ms
     * @param[in] minBufferLen: minimum buffer length in ms (ignored if powerSaving=false)
     */
    void setStreamingBuffer(bool powerSaving,
                            uint32_t maxBufferLen,
                            uint32_t minBufferLen);
    /**
     * return the effective max streaming buffer length
     */
    uint32_t getMaxStreamingBuffer() const;

    void networkChange( NetworkInterfaceInfo const& netinfo );

    void externalIpAddressChange( std::string const& ipaddr );

    void getNetworkMonitorState( Variant* pState );

    void setNetworkMonitorState( Variant const& state );

    void setNetworkMonitorListener( INetworkMonitorListenerPtr pNetworkMonitorListener );

protected:
    StreamingManager( ManifestPtr pManifest,
                      IMediaSinkPtr const& pMediaSink,
                      IMediaBufferPoolPtr const& pMediaBufferPool,
                      IStreamingReporterPtr const& pStreamingReporter,
                      ITransportReporterPtr const& pTransportReporter,
                      IStreamingHeuristicPtr const& pStreamingHeuristic,
                      IAseConfigParameterPtr const& pConfigParameter,
                      IAseConfigParameterPtr pLocalConfigParameter,
                      bool fastStart,
                      IPersistentStorePtr const& pPersistentStore);

    AseErrorCode startHttpRequestManager();

    AseErrorCode addDownloadTrack( MediaType type, uint32_t& trackId );
    AseErrorCode addAndRemoveTracks(    MediaType           type,
                                        ManifestTrackPtr&   currentManifestTrack,
                                        uint32_t&           currentTrackId );
    AseErrorCode addAndRemoveTracks();
    AseErrorCode switchAudioTrackDownloaders();

    void checkError(AseErrorCode errorCode);

    bool compareTracks( ManifestTrackPtr pManifestTrack,
                        ManifestTrackPtr pManifestTrack2 );

    void createMediaPresentation();

    /** Get the manifest track for a media type, using mTrackIndexes */
    ManifestTrackPtr getManifestTrack( MediaType type ) const;

private:
    StreamingManager( const StreamingManager& ) : enable_shared_from_this<StreamingManager>() {}
    StreamingManager& operator=( const StreamingManager& ) {return *this;}

protected:
    enum StreamingManagerState
    {
        STARTED,
        STOPPING,
        STOPPED
    };

    MediaPresentationPtr    mMediaPresentationPtr;
    HttpRequestManagerPtr   mHttpRequestManagerPtr;

    std::vector<ManifestPtr> mManifestPtrs;
    ManifestPtr mCurrentManifestPtr;
    IMediaSinkPtr mMediaSinkPtr;
    IMediaBufferPoolPtr mMediaBufferPoolPtr;
    IStreamingReporterPtr mStreamingReporterPtr;
    ITransportReporterPtr mTransportReporterPtr;
    IStreamingHeuristicPtr mStreamingHeuristicsPtr;
    IAseConfigParameterPtr mConfigParamterPtr;
    IAseConfigParameterPtr mLocalConfigParametePtr;
    IPersistentStorePtr mPersistentStorePtr;

    // Media Consumer Id registered with the HttpRequestManager
    uint32_t              mMediaConsumerId;

    // current track indexes for the current manifest
    std::vector<uint32_t> mTrackIndexes;

    std::map<uint32_t, TrackConfiguration> mTrackConfigurations;

    // Current manifest tracks
    ManifestTrackPtr      mVideoManifestTrack;
    ManifestTrackPtr      mAudioManifestTrack;

    // Id for next track downloader
    uint32_t              mNextTrackId;

    // Ids for current track downloaders
    uint32_t              mVideoTrackId;
    uint32_t              mAudioTrackId;
    uint32_t              mHeadersTrackId;

    StreamingManagerState mState;
    bool                  mFastStart;
    bool                  mManifestSwitched;
    bool                  mFirstAudioSwitch;

    volatile bool         mVideoTrackDownloaderPaused;

    //Network interface information
    NetworkInterfaceInfo  mNetworkInterfaceInfo;
    std::string                 mClientExternalIpAddress;
};

typedef StreamingManager::StreamingManagerPtr StreamingManagerPtr;

}}

#endif
