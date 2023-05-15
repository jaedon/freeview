/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "PrintManifest.h"

#include <iomanip>


using namespace netflix;
using namespace netflix::ase;

namespace std {

template<class T>
std::ostream& operator<<( std::ostream& os, std::vector<T> const& vec );

template<class T>
std::ostream& operator<<( std::ostream& os, std::vector<T> const& vec )
{
    for ( int i = 0; i < (int) vec.size(); ++i )
        os << ( i == 0 ? "" : " " ) << vec[ i ];
    return os;
}

std::ostream& operator<<( std::ostream& os, ManifestStream::Protection const& protection );
std::ostream& operator<<( std::ostream& os, MediaType const& type );
std::ostream& operator<<( std::ostream& os, ManifestTrack::Kind const& kind );
std::ostream& operator<<( std::ostream& os, MimeType::Type const& mimeType );
std::ostream& operator<<( std::ostream& os, MimeCodec::AudioType const& type );
std::ostream& operator<<( std::ostream& os, MimeCodec::VideoType const& type );

std::ostream& operator<<( std::ostream& os, StartplayPoint const& point )
{
    os << "(" << point.mBitrateInBps << "kbps,"
        << AseTimeInterval( point.mTimeDuration, 1000 ).getNumberOfTicks() << "ms)";
    return os;
}

//std::ostream& std::operator<<( std::ostream& os, wrap<ManifestStream::Protection> const& protection )
std::ostream& operator<<( std::ostream& os, ManifestStream::Protection const& protection )
{
    os << ( ( protection == ManifestStream::PROTECTION_ISO_CENC ) ? "Common Encryption"
            : ( protection == ManifestStream::PROTECTION_PLAYREADY ) ? "PlayReady"
            : ( protection == ManifestStream::PROTECTION_WIDEVINE ) ? "Widevine"
            : ( protection == ManifestStream::PROTECTION_WIDEVINE_CEF ) ? "WidevineCEF"
            : "Unknown" );
    return os;
}

//std::ostream& std::operator<<( std::ostream& os, wrap<MediaType> const& type )
std::ostream& operator<<( std::ostream& os, MediaType const& type )
{
    os << ( ( type == MEDIA_AUDIO ) ? "Audio"
            : ( type == MEDIA_VIDEO ) ? "Video"
            : ( type == MEDIA_TEXT ) ? "Text"
            : "Unknown" );
    return os;
}

//std::ostream& std::operator<<( std::ostream& os, wrap<ManifestTrack::Kind> const& kind )
std::ostream& operator<<( std::ostream& os, ManifestTrack::Kind const& kind )
{
    os << ( ( kind == ManifestTrack::KIND_CAPTIONS ) ? "Captions"
            : ( kind == ManifestTrack::KIND_SUBTITLES ) ? "Subtitles"
            : ( kind == ManifestTrack::KIND_COMMENTARY ) ? "Commentary"
            : ( kind == ManifestTrack::KIND_DESCRIPTIONS ) ? "Descriptions"
            : "Unknown" );
    return os;
}

//std::ostream& std::operator<<( std::ostream& os, wrap<MimeType::Type> const& mimeType )
std::ostream& operator<<( std::ostream& os, MimeType::Type const& mimeType )
{
    os << ( ( mimeType == MimeType::MIME_TYPE_APPLICATION ) ? "application"
            : ( mimeType == MimeType::MIME_TYPE_AUDIO ) ? "audio"
            : ( mimeType == MimeType::MIME_TYPE_VIDEO ) ? "video"
            : ( mimeType == MimeType::MIME_TYPE_TEXT ) ? "text"
            : ( mimeType == MimeType::MIME_TYPE_IMAGE ) ? "image"
            : "unknown" );
    return os;
}

std::ostream& operator<<( std::ostream& os, MimeType const& mime )
{
    if ( !mime.valid() )
    {
        os << "(invalid)";
    }
    else
    {
        os << mime.getType() << "/" << mime.getSubtype();
    }
    return os;
}

//std::ostream& std::operator<<( std::ostream& os, wrap<MimeCodec::AudioType> const& type )
std::ostream& operator<<( std::ostream& os, MimeCodec::AudioType const& type )
{
    os << ( ( type == MimeCodec::AUDIO_MPEG4 ) ? "mpeg4"
            : ( type == MimeCodec::AUDIO_DDPLUS ) ? "ddplus"
            : "unknown" );
    return os;
}

//std::ostream& std::operator<<( std::ostream& os, wrap<MimeCodec::VideoType> const& type )
std::ostream& operator<<( std::ostream& os, MimeCodec::VideoType const& type )
{
    os << ( ( type == MimeCodec::VIDEO_MPEG4_PART2 ) ? "mpeg4"
            : ( type == MimeCodec::VIDEO_AVC1 ) ? "avc1"
            : ( type == MimeCodec::VIDEO_AVC2 ) ? "avc2"
            : ( type == MimeCodec::VIDEO_SVC1 ) ? "svc1"
            : ( type == MimeCodec::VIDEO_MVC1 ) ? "mvc1"
            : ( type == MimeCodec::VIDEO_MVC2 ) ? "mvc2"
            : "unknown" );
    return os;
}

std::ostream& operator<<( std::ostream& os, MimeCodec::Mpeg4AudioParameters const& params )
{
    os << "." << std::hex << params.objectTypeIndication << "." << params.audioObjectTypeIndication;
    return os << std::dec;
}

std::ostream& operator<<( std::ostream& os, MimeCodec::AvcParameters const& params )
{
    os << "." << std::hex << std::setfill( '0' ) << std::setw( 2 ) << params.profileIdc
        << std::setw( 2 ) << params.constraintSetFlags
        << std::setw( 2 ) << params.avcLevelIdc;
    return os << std::dec << std::setfill( ' ' );
}

std::ostream& operator<<( std::ostream& os, MimeCodec const& codec )
{
    if ( !codec.valid() )
    {
        os << "(invalid)";
    }
    else
    {
        if ( codec.hasAudio() )
        {
            os << "audio(";
            for( int i = 0; i < (int) codec.getAudioTypes().size(); ++i )
            {
                os << (( i == 0 ) ? "" : ", ");
                os << codec.getAudioTypes()[ i ];
                if ( codec.getAudioTypes()[ i ] == MimeCodec::AUDIO_MPEG4 )
                {
                    MimeCodec::Mpeg4AudioParameters params;
                    if ( codec.getMpeg4AudioParameters( i, params ) )
                        os << params;
                }
            }
            os << ")";
        }

        if ( codec.hasVideo() )
        {
            if ( codec.hasAudio() )
                os << ", ";

            os << "video(";
            for( int i = 0; i < (int) codec.getVideoTypes().size(); ++i )
            {
                os << (( i == 0 ) ? "" : ", ");
                MimeCodec::VideoType type = codec.getVideoTypes()[ i ];
                os << type;
                if ( type == MimeCodec::VIDEO_AVC1
                    || type == MimeCodec::VIDEO_AVC2
                    || type == MimeCodec::VIDEO_SVC1
                    || type == MimeCodec::VIDEO_MVC1
                    || type == MimeCodec::VIDEO_MVC2 )
                {
                    MimeCodec::AvcParameters params;
                    if ( codec.getAvcParameters( i, params ) )
                        os << params;
                }
            }
            os << ")";
        }
    }
    return os;
}

std::ostream& operator<<( std::ostream& os, ManifestUrl const& url )
{
    os << url.getUrl() << " (" << url.getLocationKey() << ")";
    return os;
}

std::ostream& operator<<( std::ostream& os, ManifestUrls const& urls )
{
    for( int i = 0; i < (int) urls.size(); ++i )
        os << "                " << urls[i] << std::endl;
    return os;
}

std::ostream& operator<<( std::ostream& os, ManifestStream const& stream )
{
    ContentProfile profile = stream.getContentProfile();
    bool isVideo = (profile < LAST_VIDEO_PROFILE);

    os << "        " << "Stream:" << std::endl;
    os << "            " << "Enabled: " << stream.enabled() << std::endl;
    os << "            " << "Content Profile: " << profileToString(profile) << std::endl;
    os << "            " << "Mime type: " << stream.getMimeType() << std::endl;
    os << "            " << "Mime codec: " << stream.getMimeCodec() << std::endl;

    if (isVideo)
    {
        os << "            " << "Bitrate: " << stream.getNominalBitrate() << std::endl;
        os << "            " << "Video size: " << stream.getVideoSize().first << "x" << stream.getVideoSize().second;
        os << " Pixel aspect ratio: " << stream.getPixelAspectRatio().first << "x" << stream.getPixelAspectRatio().second << std::endl;
        os << "            " << "Video frame rate value: " << stream.getVideoFrameRate().first << std::endl;
        os << "            " << "Video frame rate scale: " << stream.getVideoFrameRate().second << std::endl;

        os << "            " << "Startplay points: " << stream.getStartplayCurve() << std::endl;
        os << "            " << "HD: " << (stream.getHD() ? "true" : "false") << std::endl;
        os << "            " << "SuperHD: " << (stream.getSuperHD() ? "true" : "false") << std::endl;
    }
    else
    {
        os << "            " << "Audio channels: " << stream.getAudioChannels() << std::endl;
        os << "            " << "Audio sampling frequency: " << stream.getAudioSamplingFrequency() << std::endl;
    }

    os << "            " << "Protection: " << stream.getProtections() << std::endl;
    os << "            " << "Urls: "<< std::endl;
    os << stream.getUrls();
    return os;
}

std::ostream& operator<<( std::ostream& os, ManifestTrack const& track )
{
    os << "    " << "Track:" << std::endl;
    os << "        " << "Stereo: " << (track.isStereoVideo() ? "true" : "false") << std::endl;
    os << "        " << "Media Type: " << track.getMediaType()
                    << ", Track kind: " << track.getTrackKind() << std::endl;
    os << "        " << "Language: " << track.getLanguage()
        << ( track.isNativeLanguage() ? " (native)" : " (dubbed)" ) << std::endl;

    for( int i = 0; i < (int) track.getStreams().size(); ++i )
        os << *track.getStreams()[ i ];

    return os;
}

std::ostream& operator<<( std::ostream& os, Manifest const& manifest )
{
    os << "Manifest: " << std::endl;
    os << "    Duration: " << AseTimeInterval( manifest.getDuration(), 1000 ).getNumberOfTicks() << "ms" << std::endl;
    for( int i = 0; i < (int) manifest.getTracks().size(); ++i )
    {
        os << *manifest.getTracks()[ i ];
    }

    return os;
}
}
