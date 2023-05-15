/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MIME_CODEC_H
#define MIME_CODEC_H

#include <string>
#include <vector>
#include <map>

#include <nrdase/MimeType.h>

namespace netflix {
namespace ase
{

/** Class to parse RFC4281 codec parameter */
class MimeCodec
{
public:
    enum AudioType
    {
        AUDIO_NONE = 0,
        AUDIO_MPEG4,
        AUDIO_DDPLUS

    };

    enum VideoType
    {
        VIDEO_NONE = 0,
        VIDEO_MPEG4_PART2,
        VIDEO_AVC1,
        VIDEO_AVC2,
        VIDEO_SVC1,
        VIDEO_MVC1,
        VIDEO_MVC2,
        VIDEO_HEV1
    };

    struct Mpeg4Parameters
    {
        int objectTypeIndication;
    };

    struct Mpeg4AudioParameters : public Mpeg4Parameters
    {
        int audioObjectTypeIndication;
    };

    struct Mpeg4VideoParameters : public Mpeg4Parameters
    {
        int videoProfileLevelIndication;
    };

    struct AvcParameters
    {
        int profileIdc;

        int constraintSetFlags;

        int avcLevelIdc;
    };

    struct HEVCParameters
    {
        int profileSpace;

        int profileIdc;

        int compatibilityIdc;

        int tierFlag;

        int levelIdc;
    };


public:
    MimeCodec();

    MimeCodec( std::string const& codec, bool fancy = false );

    MimeCodec( MimeType const& mime );

    MimeCodec& operator=( std::string const& codec );

    bool valid() const;

    bool hasAudio() const;

    bool hasVideo() const;

    std::vector<AudioType> const& getAudioTypes() const;

    std::vector<VideoType> const& getVideoTypes() const;

    bool getMpeg4AudioParameters( int index, Mpeg4AudioParameters& params ) const;

    bool getMpeg4VideoParameters( int index, Mpeg4VideoParameters& params ) const;

    bool getAvcParameters( int index, AvcParameters& params ) const;

protected:
    bool parse( std::string const& codec, bool fancy = false );

    bool parseMpeg4AudioParams( std::vector<std::string> const& elements, int index );

    bool parseMpeg4VideoParams( std::vector<std::string> const& elements, int index );

    bool parseAvcParams( std::vector<std::string> const& elements, int index );

    bool parseHEVCParams(std::vector<std::string> const& elements, int index );

    template<typename T, typename S>
    static bool getParameters( int index, T const& container, S& result );

protected:
    bool                        mValid;

    std::vector<AudioType>      mAudioTypes;

    std::vector<VideoType>      mVideoTypes;

    std::map<int,Mpeg4AudioParameters>  mMpeg4AudioParameters;

    std::map<int,Mpeg4VideoParameters>  mMpeg4VideoParameters;

    std::map<int,AvcParameters>         mAvcParameters;

    std::map<int, HEVCParameters>    mHEVCParameters;
};

inline MimeCodec::MimeCodec()
    : mValid( false )
{
}

inline MimeCodec::MimeCodec( std::string const& codec, bool fancy )
    : mValid( false )
{
    mValid = parse( codec, fancy );
}

inline MimeCodec& MimeCodec::operator=( std::string const& codec )
{
    mValid = parse( codec );
    return *this;
}

inline bool MimeCodec::valid() const
{
    return mValid;
}

inline bool MimeCodec::hasAudio() const
{
    return valid() && getAudioTypes().size() != 0;
}

inline bool MimeCodec::hasVideo() const
{
    return valid() && getVideoTypes().size() != 0;
}

inline std::vector<MimeCodec::AudioType> const& MimeCodec::getAudioTypes() const
{
    return mAudioTypes;
}

inline std::vector<MimeCodec::VideoType> const& MimeCodec::getVideoTypes() const
{
    return mVideoTypes;
}

template<typename T, typename S>
bool MimeCodec::getParameters( int index, T const& container, S& result )
{
    typename T::const_iterator it = container.find( index );
    if ( it == container.end() )
        return false;

    result = (it->second);
    return true;
}

}} // namespace ase

#endif /* MIME_CODEC_H */
