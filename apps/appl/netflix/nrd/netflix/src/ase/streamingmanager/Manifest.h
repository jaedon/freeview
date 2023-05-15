/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MANIFEST_H
#define MANIFEST_H

/** @file Manifest.h */
#include <algorithm>
#include <set>
#include <vector>

#include <nrdnet/AseUrl.h>

#include <nrdbase/ObjectCount.h>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdase/AseTimeStamp.h>
#include <nrdase/AseTimeRange.h>
#include <nrdase/ByteRange.h>
#include <nrdase/StartplayPoint.h>
#include <nrdase/MimeType.h>
#include <nrdase/MimeCodec.h>

NRDP_DECLARE_OBJECTCOUNT(ManifestUrl);
NRDP_DECLARE_OBJECTCOUNT(ManifestTrack);
NRDP_DECLARE_OBJECTCOUNT(ManifestServingLocation);
NRDP_DECLARE_OBJECTCOUNT(ManifestServer);
NRDP_DECLARE_OBJECTCOUNT(Manifest);

namespace netflix {
namespace ase {

class Manifest;
class ManifestServer;
class ManifestServingLocation;
class ManifestTrack;
class ManifestStream;
class ManifestUrl;

typedef shared_ptr<Manifest> ManifestPtr;
typedef shared_ptr<Manifest const> ConstManifestPtr;
typedef shared_ptr<ManifestServer> ManifestServerPtr;
typedef shared_ptr<ManifestServer const> ConstManifestServerPtr;
typedef shared_ptr<ManifestServingLocation> ManifestServingLocationPtr;
typedef shared_ptr<ManifestServingLocation const> ConstManifestServingLocationPtr;
typedef shared_ptr<ManifestTrack> ManifestTrackPtr;
typedef shared_ptr<ManifestTrack const> ConstManifestTrackPtr;
typedef shared_ptr<ManifestStream> ManifestStreamPtr;
typedef shared_ptr<ManifestStream const> ConstManifestStreamPtr;
typedef shared_ptr<ManifestUrl> ManifestUrlPtr;
typedef shared_ptr<ManifestUrl const> ConstManifestUrlPtr;

/**
 * domain and location are interchangable in this release.
 */

/**
 * @class ManifestUrl
 * @brief Represents a URL that appears in a manifest
 */
class ManifestUrl
{
public:
    ~ManifestUrl();
    /** Create a new ManifestUrl object
     *  @param url [in] The URL
     *  @param locationKey [in] Domain Key: URLs with the same
     *                        Location Key may be assumed to be hosted
     *                        on infrastructure with shared
     *                        performance-related properties.
     *                        For example on the same CDN.
     */
    static int construct( ManifestStreamPtr   pParentStream,
                          AseUrl const& url,
                          std::string const&  locationKey,
                          uint32_t            cdnId,
                          ManifestUrlPtr&     pManifestUrl );

    /** Get the URL */
    AseUrl const& getUrl() const { return mUrl; }

    /** Get the location key */
    std::string const& getLocationKey() const { return mLocationKey; }

    uint32_t getCdnId() const { return mCdnId; }

    /** Returns a ptr to the parent ManifestStream */
    ManifestStreamPtr getParent() const { return mParentStreamPtr.lock(); }

    struct UrlCompareLess
    {
        // compare two urls following the order of level, location rank and server rank
        bool operator()( ManifestUrl const& lhs, ManifestUrl const& rhs );
    };

protected:
    ManifestUrl( ManifestStreamPtr pParentStream,
                 AseUrl const& url,
                 std::string const& locationKey,
                 uint32_t cdnId );

    /** The parent stream */
    weak_ptr<ManifestStream>  mParentStreamPtr;

    const AseUrl mUrl;
    const std::string mLocationKey;
    uint32_t mCdnId;
};

typedef std::vector<ConstManifestUrlPtr> ManifestUrls;

class ManifestStream
{
public:
    ~ManifestStream();
    enum Protection
    {
        PROTECTION_UNKNOWN = 0,
        PROTECTION_ISO_CENC,        // ISO Common Encryption Scheme
        PROTECTION_PLAYREADY,
        PROTECTION_WIDEVINE,
        PROTECTION_WIDEVINE_CEF
    };

    struct NominalBitrateCompare
    {
        bool operator()( ManifestStream const& lhs, ManifestStream const& rhs );
    };

    /** Construct a manifest stream with no attributes
     *  @param pParentTrack [in] The parent track
     *  @param pManifestStream [out] The new manifest stream object
     *  @return Zero on success or negative error code
     */
    static int construct(   ManifestTrackPtr       pParentTrack,
                            ManifestStreamPtr&     pManifestStream );


    /** Returns a ptr to the parent ManifestStream */
    ManifestTrackPtr getParent() const;

    /** Enable/disable a stream. A disabled stream should be ignored for adaptive
     * streaming purposes. It is there only as a placeholder
     */
    bool enabled() const;
    void setEnabled(bool enabled);

    /**
     * Gets the streamKey to identify this stream
     * @return streamKey
     */
    std::string const& getStreamKey() const;

    /**
     * Sets the streamKey to identify this stream
     * @param[in] key The key to identify this stream
     */
    void setStreamKey(std::string const& key);

    /** Get the stream or header URLs
     *
     *  @return The stream URLs for non-chunked streams or the header URLs for chunked
     *          streams
     */
    ManifestUrls const& getUrls() const;

    /* Set the manifest urls */
    void setUrls( ManifestUrls const& urls );

    /** Get MIME type */
    MimeType const& getMimeType() const;

    /** Set the MIME type */
    void setMimeType( const MimeType& sMimeType );

    /** Get the RFC4281 codecs parameter */
    MimeCodec const& getMimeCodec() const;

    /** Set the RFC4281 codecs parameter */
    void setMimeCodec( const MimeCodec& codecs );

    /** Get the content profile */
    netflix::ContentProfile getContentProfile() const;

    /** Set the Content Profile */
    void setContentProfile(const netflix::ContentProfile& contentProfile);

    /** Get the encoded video width and height in pixels */
    std::pair<int,int> getVideoSize() const;

    /** Set the encoded video width and height */
    void setVideoSize( int nWidth, int nHeight );

    /** Gets/Sets the HD property */
    void setHD(bool hd);
    bool getHD() const;

    /** Gets/Sets the SUPERHD property */
    void setSuperHD(bool superhd);
    bool getSuperHD() const;

    /** Get the pixel aspect ratio */
    std::pair<int,int> getPixelAspectRatio() const;

    /** Set the pixel aspect ratio */
    void setPixelAspectRatio( int nParx, int nPary );

    /** Get the video frame rate */
    std::pair<int,int> getVideoFrameRate() const;

    /** Set the video frame rate */
    void setVideoFrameRate( int value, int scale );

    /** Get the number of audio channels */
    std::vector<int> const& getAudioChannels() const;

    /** Set the number of audio channels */
    void setAudioChannels( std::vector<int> const& nChannels );

    /** Get the audio sample frequency in Hz */
    int getAudioSamplingFrequency() const;

    /** Set the audio sample frequency in Hz */
    void setAudioSampleFrequency( int nFrequency );

    /** Test for a descriptor tag */
    bool hasDescriptor( std::string const& descriptor ) const;

    /** Add a descriptor tags */
    void addDescriptor( std::string const& descriptor );

    /** Remove a descriptor tag */
    void removeDescriptor( std::string const& descriptor );

    /** Get the startplay curve */
    std::vector<StartplayPoint> const& getStartplayCurve() const;

    /** Set the startplay curve */
    void setStartplayCurve( std::vector< StartplayPoint > const& );

    /** Get the nominal bitrate (in Kbps) */
    int getNominalBitrate() const;

    /** Set the nominal bitrate */
    void setNominalBitrate(int bitrateKbps);

    /** Get the supported protection types */
    std::vector<Protection> const& getProtections() const;

    /** Set the protection types */
    void setProtections( std::vector<Protection> const& types );

    /** Gets/Sets the stream hashes (for validation of timed text streams */
    std::map<std::string, std::string> const& getTimedTextHashes() const;
    void setTimedTextHashes( std::map<std::string, std::string> const& hashes );

    /** Sort the urls according to the given comparison function */
    template<class Compare>
        void sortUrls( Compare comp );

protected:
    ManifestStream( ManifestTrackPtr pParentTrack );

private:
    ManifestStream( const ManifestStream& );
    ManifestStream& operator=( const ManifestStream& );

protected:
    /** The parent track */
    weak_ptr<ManifestTrack>     mParentTrackPtr;

    bool                        mEnabled;

    std::string                 mStreamKey;

    /** The stream URLs */
    ManifestUrls                mUrls;

    MimeType                    mMimeType;
    MimeCodec                   mMimeCodec;
    netflix::ContentProfile              mContentProfile;
    int                         mVideoWidth;
    int                         mVideoHeight;
    int                         mParx;
    int                         mPary;
    std::pair<int, int>         mFrameRate;
    std::vector<int>            mAudioChannels;
    int                         mAudioSamplingFrequency;
    std::set<std::string>       mDescriptors;
    bool                        mHD;
    bool                        mSuperHD;

    /** The startplay curve */
    std::vector<StartplayPoint> mStartPlayCurve;
    int                         mNominalBitrate;

    /** Protection types */
    std::vector<Protection>     mProtections;

    /** Timed Text hashes */
    std::map<std::string, std::string> mTimedTextHashes;
};

class ManifestTrack
{
public:
    ~ManifestTrack();
    enum Kind
    {
        KIND_UNKNOWN = 0,
        KIND_PRIMARY,
        KIND_CAPTIONS,
        KIND_SUBTITLES,
        KIND_COMMENTARY,
        KIND_DESCRIPTIONS,
        KIND_FORCED_NARRATIVE
    };

    /* Error codes returned by methods of this class */
    // None

    /** Construct a manifest track with no streams
     *  @param pParentManifest [in] The parent manifest
     *  @param pManifestTrack [out] The new manifest track object
     *  @return Zero on success or negative error code
     */
    static int construct(   ManifestPtr pParentManifest,
                            ManifestTrackPtr& pManifestTrack );

    /**
     * Gets the trackKey to identify this stream
     * @return trackKey
     */
    std::string const& getTrackKey() const;

    /**
     * Sets the trackKey to identify this stream
     * @param[in] key The key to identify this stream
     */
    void setTrackKey(std::string const& key);

    /** Get the streams */
    std::vector<ManifestStreamPtr> const& getStreams() const;

    /** Add a stream */
    void addStream( ManifestStreamPtr pManifestStream );

    /** Remove a stream */
    void removeStream( int index );

    /** Get the media type */
    MediaType getMediaType() const;

    /** Set the media type */
    void setMediaType( MediaType type );

    /** Get the language */
    std::string getLanguage() const;

    /** Set the language */
    void setLanguage( const std::string& sLang );

    /** Get the native language flag (true if language is the native langauge of the content */
    bool isNativeLanguage() const;

    /** Set the native language flag */
    void setNativeLanguage( bool isNative );

    /** Get the track kind */
    Kind getTrackKind() const;

    /** Set the track kind */
    void setTrackKind( Kind kind );

    /** Get if the track supports stereo video */
    bool isStereoVideo() const;

    /** Sets whether the track supports stereo video */
    void setStereoVideo(bool stereoVideo);

    /** Get the timestamp offset */
    AseTimeStamp getTimestampOffset() const;

    /** Set the timestamp offset for the manifest track*/
    void setTimestampOffset(AseTimeStamp const& offset);

    /** Gets the raw track type */
    std::string getRawType() const;

    /** Sets the raw track type */
    void setRawType(const std::string &rawType);

    /** Sets the max video frame rate */
    void setMaxVideoFrameRate(int value, int scale);

    /** Gets the max video frame rate */
    std::pair<int, int> getMaxVideoFrameRate() const;

    /** Gets/sets the trackKey of track that is paired with this track */
    std::string getPairedTrack() const;
    void setPairedTrack(const std::string &pairedTrack);

    /** Gets/sets tracks that are not permitted to be used with this track */
    std::vector<std::string> const& getDisallowedTracks() const;
    void setDisallowedTracks(std::vector<std::string> const& disallowedTracks);

    /** Sort the streams according to the given comparison function */
    template<class Compare>
        void sortStreams( Compare comp );

    ManifestPtr getParent() const;

protected:
    ManifestTrack( ManifestPtr pManifest );

private:
    ManifestTrack( ManifestTrack const& );
    ManifestTrack& operator=( ManifestTrack const& );

protected:
    weak_ptr<Manifest>        mManifestPtr;

    std::string                     mTrackKey;

    std::string                     mRawType;

    std::string                     mPairedTrack;

    std::vector<std::string>        mDisallowedTracks;

    MediaType                       mMediaType;

    std::vector<ManifestStreamPtr>  mStreams;

    std::string                     mLanguage;

    bool                            mNativeLanguage;

    Kind                            mKind;

    bool                            mStereoVideo;

    std::pair<int, int>             mMaxVideoFrameRate;

    AseTimeStamp                    mTimestampOffset;
};

/**
 * @class ManifestServingLocation
 */
class ManifestServingLocation
{
public:
    ~ManifestServingLocation();
    /** Construct a ManifestServingLocation object. */
    static int construct(std::string const& locationKey,
                         uint32_t rank,
                         uint32_t level,
                         uint32_t weight,
                         ManifestServingLocationPtr& pManifestServingLocation );

    const std::string& getLocationKey() const { return mLocationKey; }
    uint32_t getRank() const { return mRank; }
    uint32_t getLevel() const { return mLevel; }
    uint32_t getWeight() const { return mWeight; }

    bool operator<(ManifestServingLocation const& rhs);

protected:
    ManifestServingLocation(std::string const& locationKey,
                            uint32_t rank,
                            uint32_t level,
                            uint32_t weight);

protected:
    std::string mLocationKey;
    uint32_t mRank; // a soft priority rank within the same level
    uint32_t mLevel; // a level reflecting cost of the location
    uint32_t mWeight;
};

/**
 * @class ManifestServer
 */
class ManifestServer
{
public:
    ~ManifestServer();
    /** Construct a ManifestServer object. */
    static int construct(const std::string &cdnName,
                         const std::string &cdnType,
                         uint32_t cdnId,
                         const std::string &locationKey,
                         uint32_t rank,
                         bool lowGradeFlag,
                         /*out*/ManifestServerPtr &pManifestServer);

    const std::string &getName() const { return mName; }
    const std::string &getCdnType() const { return mCdnType; }
    uint32_t getCdnId() const { return mCdnId; }
    const std::string &getLocationKey() const { return mLocationKey; }
    uint32_t getRank() const { return mRank; }
    bool getLowGradeFlag() const { return mLowGradeFlag; }

protected:
    ManifestServer(const std::string &cdnName,
                   const std::string &cdnType,
                   uint32_t cdnId,
                   const std::string &locationKey,
                   uint32_t rank,
                   bool lowGradeFlag);

protected:
    std::string mName;
    std::string mCdnType;
    uint32_t mCdnId;
    std::string mLocationKey;
    uint32_t mRank;
    bool mLowGradeFlag;
};

/**
 * @class Manifest
 * @brief Class representing a generic adaptive streaming manifest
 *
 * This is a messenger class containing a format independent representation of
 * and adaptive streaming manifest
 */
class Manifest
{
public:
    ~Manifest();
    /* Error codes returned by methods of this class */
    static const int ERR_INVALID_DURATION = -1;

    /** Construct a manifest period with no tracks
     *  @param dur [in] The presentation duration
     *  @param pManifest [out] The new manifest object
     *  @return Zero on success or negative error code
     */
    static int construct( AseTimeInterval dur,
                          ManifestPtr& pManifest );

    /** Get the duration */
    AseTimeInterval getDuration() const;

    /** Set the duration */
    void setDuration( AseTimeInterval dur );

    /** Get the time range */
    AseTimeRange getTimeRange() const;

    /** Set the time range */
    void setTimeRange( AseTimeRange const& timeRange );

    /** Get the tracks */
    std::vector<ManifestTrackPtr> const& getTracks() const;

    /** Add a track */
    void addTrack( ManifestTrackPtr pManifestTrack );

    /** Remove a track */
    void removeTrack(int index);

    /** Get the serving locations. */
    std::vector<ManifestServingLocationPtr> const& getServingLocations() const;

    /** Set the serving locations. */
    void setServingLocations( std::vector<ManifestServingLocationPtr> const& servingLocations );

    /** get the serving location given the location key */
    ManifestServingLocationPtr findServingLocation(std::string const& locationKey) const;
    /** Get the servers indexed by cdnId */
    std::map<uint32_t, ManifestServerPtr> const& getServers() const;

    /** Sets the servers */
    void setServers(std::map<uint32_t, ManifestServerPtr> const& servers);

    /** set the content id */
    void setContentId( std::string const& contentId );

    /** get the content id */
    std::string getContentId() const;

    /** set the period index
     * (REMOVEME: The reason why we need a period index is to be able to deliver the
     * correct stream header to a given stream of a specific period)
     */
    void setManifestIndex( uint32_t manifestIndex );

    /** get the period index */
    uint32_t getManifestIndex() const;

    /** set whether this period is the last one */
    void setFinalPeriod( bool finalPeriod );

    /** get whether this period is final */
    bool getFinalPeriod() const;

    /** set the maximum video size */
    void setMaxVideoSize( uint32_t width, uint32_t height);

    /** get the maximum video size */
    void getMaxVideoSize( uint32_t& width, uint32_t& height ) const;

    /** Get the location keys */
    std::vector<std::string> getLocationKeys() const;

    /** Whether this Manifest requires license */
    bool requireLicense() const;

    /** set whether this Manifest requires license */
    void setRequireLicense(bool flag);

    /** Sort the streams of each track according to the given comparison function */
    template<class Compare>
        void sortStreams( Compare comp );

    /** Audio/TimedText track preferences */
    typedef std::pair<std::string, std::string> TrackIDPair;
    void setAudioTimedTextTrackPreferences(
        const std::vector<TrackIDPair> &preferences);
    std::vector<TrackIDPair> const& getAudioTimedTextTrackPreferences() const;

protected:
    Manifest( AseTimeInterval dur );

private:
    Manifest( const Manifest& );
    Manifest& operator=( const Manifest& );

protected:
    AseTimeInterval                             mDuration;
    AseTimeRange                                mTimeRange;
    std::string                                 mContentId;
    uint32_t                                    mManifestIndex;
    bool                                        mFinalPeriod;
    bool                                        mRequireLicense;
    uint32_t                                    mMaxWidth;
    uint32_t                                    mMaxHeight;
    std::vector<ManifestTrackPtr>               mTracks;
    std::vector<ManifestServingLocationPtr>     mServingLocations;
    std::map<uint32_t, ManifestServerPtr>       mServers;
    std::vector<TrackIDPair>                    mAudioTimedTextTrackPreferences;

    std::set<std::string>                       mLocationKeys;
};


/**
 * @class ManifestUrl
 */
inline int ManifestUrl::construct( ManifestStreamPtr  pParentStream,
                                   AseUrl const&      url,
                                   std::string const& locationKey,
                                   uint32_t cdnId,
                                   ManifestUrlPtr&    pManifestUrl )
{
    pManifestUrl.reset( new ManifestUrl( pParentStream, url, locationKey, cdnId ) );
    return AS_NO_ERROR;
}

inline ManifestUrl::ManifestUrl( ManifestStreamPtr pParentStream,
                                 AseUrl const& url,
                                 std::string const& locationKey,
                                 uint32_t cdnId )
    : mParentStreamPtr( pParentStream )
      , mUrl( url )
      , mLocationKey( locationKey )
      , mCdnId( cdnId )
{
    NRDP_OBJECTCOUNT_REF(ManifestUrl, this);
}

inline ManifestUrl::~ManifestUrl()
{
    NRDP_OBJECTCOUNT_DEREF(ManifestUrl, this);
}

/**
 * @class ManifestStream
 */
inline ManifestTrackPtr ManifestStream::getParent() const
{
    return mParentTrackPtr.lock();
}

inline std::string const& ManifestStream::getStreamKey() const
{
    return mStreamKey;
}

inline void ManifestStream::setStreamKey(std::string const& key)
{
    mStreamKey = key;
}

inline bool ManifestStream::enabled() const
{
    return mEnabled;
}

inline void ManifestStream::setEnabled(bool enable)
{
    mEnabled = enable;
}

inline MimeType const& ManifestStream::getMimeType() const
{
    return mMimeType;
}

inline void ManifestStream::setMimeType( MimeType const& mime )
{
    mMimeType = mime;
    if ( !mMimeCodec.valid() ) mMimeCodec = MimeCodec(mime);
}

inline MimeCodec const& ManifestStream::getMimeCodec() const
{
    return mMimeCodec;
}

inline void ManifestStream::setMimeCodec( MimeCodec const& codecs )
{
    mMimeCodec = codecs;
}

inline netflix::ContentProfile ManifestStream::getContentProfile() const
{
    return mContentProfile;
}

inline void ManifestStream::setContentProfile( netflix::ContentProfile const& contentProfile )
{
    mContentProfile = contentProfile;
}

inline std::pair<int,int> ManifestStream::getVideoSize() const
{
    return std::make_pair( mVideoWidth, mVideoHeight );
}

inline void ManifestStream::setVideoSize( int nWidth, int nHeight )
{
    mVideoWidth = nWidth;
    mVideoHeight = nHeight;
}

inline void ManifestStream::setHD(bool hd)
{
    mHD = hd;
}

inline bool ManifestStream::getHD() const
{
    return mHD;
}

inline void ManifestStream::setSuperHD(bool superhd)
{
    mSuperHD = superhd;
}

inline bool ManifestStream::getSuperHD() const
{
    return mSuperHD;
}

inline std::pair<int,int> ManifestStream::getPixelAspectRatio() const
{
    return std::make_pair( mParx, mPary );
}

inline void ManifestStream::setPixelAspectRatio( int nParx, int nPary )
{
    mParx = nParx;
    mPary = nPary;
}

inline std::pair<int,int> ManifestStream::getVideoFrameRate() const
{
    return mFrameRate;
}

inline void ManifestStream::setVideoFrameRate( int value, int scale )
{
    mFrameRate = std::make_pair( value, scale );
}

inline std::vector<int> const& ManifestStream::getAudioChannels() const
{
    return mAudioChannels;
}

inline void ManifestStream::setAudioChannels( std::vector<int> const& nChannels )
{
    mAudioChannels = nChannels;
}

inline int ManifestStream::getAudioSamplingFrequency() const
{
    return mAudioSamplingFrequency;
}

inline void ManifestStream::setAudioSampleFrequency( int nFrequency )
{
    mAudioSamplingFrequency = nFrequency;
}

inline bool ManifestStream::hasDescriptor( std::string const& descriptor ) const
{
    return ( mDescriptors.find( descriptor ) != mDescriptors.end() );
}

inline void ManifestStream::addDescriptor( std::string const& descriptor )
{
    mDescriptors.insert( descriptor );
}

inline void ManifestStream::removeDescriptor( std::string const& descriptor )
{
    mDescriptors.erase( descriptor );
}

inline int ManifestStream::construct(
    ManifestTrackPtr        pParentTrack,
    ManifestStreamPtr&      pManifestStream )
{
    pManifestStream.reset( new ManifestStream( pParentTrack ) );
    return 0;
}

inline ManifestUrls const& ManifestStream::getUrls() const
{
    return mUrls;
}

inline void ManifestStream::setUrls( ManifestUrls const& urls )
{
    mUrls = urls;
}

inline std::vector<StartplayPoint> const& ManifestStream::getStartplayCurve() const
{
    return mStartPlayCurve;
}

inline void ManifestStream::setStartplayCurve( std::vector< StartplayPoint > const& curve )
{
    mStartPlayCurve = curve;
}

inline int ManifestStream::getNominalBitrate() const
{
    return mNominalBitrate;
}

inline void ManifestStream::setNominalBitrate(int bitrateKbps)
{
    mNominalBitrate = bitrateKbps;
}

inline std::vector<ManifestStream::Protection> const& ManifestStream::getProtections() const
{
    return mProtections;
}

inline void ManifestStream::setProtections( std::vector<ManifestStream::Protection> const& types )
{
    mProtections = types;
}

inline std::map<std::string, std::string> const&  ManifestStream::getTimedTextHashes() const
{
    return mTimedTextHashes;
}

inline void ManifestStream::setTimedTextHashes( std::map<std::string, std::string> const& hashes)
{
    mTimedTextHashes = hashes;
}

inline int ManifestTrack::construct(    ManifestPtr pParentManifest,
                                        ManifestTrackPtr& pManifestTrack )
{
    pManifestTrack.reset( new ManifestTrack( pParentManifest ) );
    return AS_NO_ERROR;
}

inline std::string const& ManifestTrack::getTrackKey() const
{
    return mTrackKey;
}

inline void ManifestTrack::setTrackKey(std::string const& key)
{
    mTrackKey = key;
}

inline std::vector<ManifestStreamPtr> const& ManifestTrack::getStreams() const
{
    return mStreams;
}

inline void ManifestTrack::addStream( ManifestStreamPtr pManifestStream )
{
    mStreams.push_back( pManifestStream );
}

inline void ManifestTrack::removeStream( int index )
{
    mStreams.erase( mStreams.begin() + index );
}

/** Get the media type */
inline MediaType ManifestTrack::getMediaType() const
{
    return mMediaType;
}

/** Set the media type */
inline void ManifestTrack::setMediaType( MediaType type )
{
    mMediaType = type;
}

inline std::string ManifestTrack::getLanguage() const
{
    return mLanguage;
}

inline void ManifestTrack::setLanguage( const std::string& sLang )
{
    mLanguage = sLang;
}

inline bool ManifestTrack::isNativeLanguage() const
{
    return mNativeLanguage;
}

inline void ManifestTrack::setNativeLanguage( bool isNative )
{
    mNativeLanguage = isNative;
}

inline ManifestTrack::Kind ManifestTrack::getTrackKind() const
{
    return mKind;
}

inline void ManifestTrack::setTrackKind( ManifestTrack::Kind kind )
{
    mKind = kind;
}

inline bool ManifestTrack::isStereoVideo() const
{
    return mStereoVideo;
}

inline void ManifestTrack::setStereoVideo(bool stereoVideo)
{
    mStereoVideo = stereoVideo;
}

inline AseTimeStamp ManifestTrack::getTimestampOffset() const
{
    return mTimestampOffset;
}

inline void ManifestTrack::setTimestampOffset(AseTimeStamp const& offset)
{
    mTimestampOffset = offset;
}

inline std::string ManifestTrack::getRawType() const
{
    return mRawType;
}

inline void ManifestTrack::setRawType(std::string const& rawType)
{
    mRawType = rawType;
}

inline void ManifestTrack::setMaxVideoFrameRate(int value, int scale)
{
    mMaxVideoFrameRate = std::make_pair(value, scale);
}

inline std::pair<int, int> ManifestTrack::getMaxVideoFrameRate() const
{
    return mMaxVideoFrameRate;
}

inline std::string ManifestTrack::getPairedTrack() const
{
    return mPairedTrack;
}

inline void ManifestTrack::setPairedTrack(std::string const& pairedTrack)
{
    mPairedTrack = pairedTrack;
}

inline std::vector<std::string> const& ManifestTrack::getDisallowedTracks() const
{
    return mDisallowedTracks;
}

inline void ManifestTrack::setDisallowedTracks(std::vector<std::string> const& disallowed)
{
    mDisallowedTracks = disallowed;
}

inline ManifestTrack::ManifestTrack( ManifestPtr pManifest )
    : mManifestPtr( pManifest )
      , mMediaType( MEDIA_UNKNOWN )
      , mNativeLanguage( true )
      , mKind( KIND_UNKNOWN )
      , mStereoVideo( false )
      , mMaxVideoFrameRate( 0, 1 )
      , mTimestampOffset(AseTimeStamp::ZERO)
{
    NRDP_OBJECTCOUNT_REF(ManifestTrack, this);
}

inline ManifestTrack::~ManifestTrack()
{
    NRDP_OBJECTCOUNT_DEREF(ManifestTrack, this);
}

inline ManifestPtr ManifestTrack::getParent() const
{
    return mManifestPtr.lock();
}

/** TODO: move this function to Utility */
template<class Compare, class Pointer>
    struct DereferenceCompare
{
    bool operator()( Pointer const& pLhs, Pointer const& pRhs )
    {
        return ( mComparator( *pLhs, *pRhs ) );
    }

    DereferenceCompare( Compare comp ) : mComparator( comp ) { }

    Compare mComparator;
};

template<class Compare>
    void ManifestTrack::sortStreams( Compare comp )
{
    std::sort( mStreams.begin(), mStreams.end(),
               DereferenceCompare<Compare,ManifestStreamPtr>( comp ) );
}

template<class Compare>
    void ManifestStream::sortUrls( Compare comp )
{
    std::sort( mUrls.begin(), mUrls.end(),
               DereferenceCompare<Compare, ConstManifestUrlPtr>(comp) );
}

struct DereferenceLess
{
    template<class Pointer>
        bool operator()(Pointer const& pLhs, Pointer const& pRhs)
        {
            return *pLhs < *pRhs;
        }
};


/**
 * @class ManifestServingLocation
 */
/** Construct a ManifestServingLocation object. */
inline int ManifestServingLocation::construct(
    std::string const& locationKey,
    uint32_t rank,
    uint32_t level,
    uint32_t weight,
    ManifestServingLocationPtr& pManifestServingLocation )
{
    pManifestServingLocation.reset( new ManifestServingLocation(
                                        locationKey,
                                        rank,
                                        level,
                                        weight) );

    return AS_NO_ERROR;
}

inline ManifestServingLocation::ManifestServingLocation(
    std::string const& locationKey,
    uint32_t rank,
    uint32_t level,
    uint32_t weight ) : mLocationKey( locationKey ),
                          mRank( rank ),
                          mLevel( level ),
                          mWeight( weight )
{
    NRDP_OBJECTCOUNT_REF(ManifestServingLocation, this);
}

inline ManifestServingLocation::~ManifestServingLocation()
{
    NRDP_OBJECTCOUNT_DEREF(ManifestServingLocation, this);
}

inline bool ManifestServingLocation::operator<(ManifestServingLocation const& rhs)
{
    if ( mLocationKey == rhs.mLocationKey )
        return false;
    else
        return ( mRank < rhs.mRank );
}

/**
 * @class ManifestServer
 */
inline int ManifestServer::construct(const std::string &name,
                                     const std::string &cdnType,
                                     uint32_t cdnId,
                                     const std::string &locationKey,
                                     uint32_t rank,
                                     bool lowGradeFlag,
                                     /*out*/ManifestServerPtr &pManifestServer)
{
    pManifestServer.reset(new ManifestServer(name, cdnType, cdnId, locationKey, rank, lowGradeFlag));
    return AS_NO_ERROR;
}

inline ManifestServer::ManifestServer(const std::string &name,
                                      const std::string &cdnType,
                                      uint32_t cdnId,
                                      const std::string &locationKey,
                                      uint32_t rank,
                                      bool lowGradeFlag) :
    mName(name),
    mCdnType(cdnType),
    mCdnId(cdnId),
    mLocationKey(locationKey),
    mRank(rank),
    mLowGradeFlag(lowGradeFlag)

{
    NRDP_OBJECTCOUNT_REF(ManifestServer, this);
}

inline ManifestServer::~ManifestServer()
{
    NRDP_OBJECTCOUNT_DEREF(ManifestServer, this);
}

/**
 * @class Manifest
 */
inline int Manifest::construct( AseTimeInterval dur, ManifestPtr& pManifest )
{
    pManifest.reset( new Manifest( dur ) );
    return AS_NO_ERROR;
}

inline AseTimeInterval Manifest::getDuration() const
{
    return mDuration;
}

inline void Manifest::setDuration( AseTimeInterval dur )
{
    mDuration = dur;
}

inline AseTimeRange Manifest::getTimeRange() const
{
    return mTimeRange;
}

inline void Manifest::setContentId( std::string const& contentId )
{
    mContentId = contentId;
}

inline std::string Manifest::getContentId() const
{
    return mContentId;
}

inline void Manifest::setManifestIndex( uint32_t manifestIndex )
{
    mManifestIndex = manifestIndex;
}

inline uint32_t Manifest::getManifestIndex() const
{
    return mManifestIndex;
}

inline void Manifest::setFinalPeriod( bool finalPeriod )
{
    mFinalPeriod = finalPeriod;
}

inline bool Manifest::getFinalPeriod() const
{
    return mFinalPeriod;
}

inline void Manifest::setMaxVideoSize( uint32_t width, uint32_t height)
{
    mMaxWidth = width;
    mMaxHeight = height;
}

inline void Manifest::getMaxVideoSize( uint32_t& width, uint32_t& height ) const
{
    width = mMaxWidth;
    height = mMaxHeight;
}

inline bool Manifest::requireLicense() const
{
    return mRequireLicense;
}

inline void Manifest::setRequireLicense(bool flag)
{
    mRequireLicense = flag;
}

inline void Manifest::setTimeRange(AseTimeRange const& timeRange)
{
    mTimeRange = timeRange;
}

inline std::vector<ManifestTrackPtr> const& Manifest::getTracks() const
{
    return mTracks;
}

inline void Manifest::addTrack( ManifestTrackPtr pManifestTrack )
{
    mTracks.push_back( pManifestTrack );
}

inline void Manifest::removeTrack(int index)
{
    mTracks.erase(mTracks.begin() + index);
}

/** Get the serving locations. */
inline std::vector<ManifestServingLocationPtr> const& Manifest::getServingLocations() const
{
    return mServingLocations;
}

/** Set the serving locations. */
inline void Manifest::setServingLocations(
    std::vector<ManifestServingLocationPtr> const& servingLocations )
{
    mServingLocations = servingLocations;
}

/** Get the servers */
inline std::map<uint32_t, ManifestServerPtr> const& Manifest::getServers() const
{
    return mServers;
}

inline ManifestServingLocationPtr
Manifest::findServingLocation(std::string const& locationKey) const
{
    ManifestServingLocationPtr pServingLocation = ManifestServingLocationPtr();
    for (std::vector<ManifestServingLocationPtr>::const_iterator locIter = mServingLocations.begin();
            locIter != mServingLocations.end();
            ++locIter)
    {
        if ( (*locIter)->getLocationKey() == locationKey )
        {
            pServingLocation = *locIter;
            break;
        }
    }
    return pServingLocation;
}

inline void Manifest::setServers(std::map<uint32_t, ManifestServerPtr> const& servers)
{
    mServers = servers;
}

inline Manifest::Manifest( AseTimeInterval dur )
    : mDuration( dur ),
      mMaxWidth(0),
      mMaxHeight(0)
{
    NRDP_OBJECTCOUNT_REF(Manifest, this);
}

inline Manifest::~Manifest()
{
    NRDP_OBJECTCOUNT_DEREF(Manifest, this);
}

template<class Compare>
    void Manifest::sortStreams( Compare comp )
{
    std::vector<ManifestTrackPtr>::iterator it = mTracks.begin();
    std::vector<ManifestTrackPtr>::iterator end = mTracks.end();
    for( ; it != end; ++it )
        (*it)->sortStreams( comp );
}

inline void Manifest::setAudioTimedTextTrackPreferences(
    const std::vector<TrackIDPair> &combos)
{
    mAudioTimedTextTrackPreferences = combos;
}

inline std::vector<Manifest::TrackIDPair> const&
    Manifest::getAudioTimedTextTrackPreferences() const
{
    return mAudioTimedTextTrackPreferences;
}

}} /* end namespace ase */

#endif /* MANIFEST_H */
