/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/MimeCodec.h>

#include <sstream>

using namespace netflix::ase;
using namespace std;

static inline string strip( string const& str );

string strip( string const& str )
{
    string::size_type a = str.find_first_not_of( " \t" );
    string::size_type b = str.find_last_not_of( " \t" );
    return ( b == string::npos ) ? string() : str.substr( a, b - a + 1 );
}

static void split( string const& str, string::size_type pos, char delim, vector<string>& result )
{
    result.clear();
    string::size_type pos2;
    do {
        pos2 = str.find( delim, pos );
        result.push_back( strip( str.substr( pos, ( pos2 == string::npos ? string::npos : pos2 - pos ) ) ) );
        pos = pos2 + 1;
    } while ( pos2 != string::npos );
}

MimeCodec::MimeCodec( MimeType const& mime )
    : mValid( false )
{
    bool fancy = false;
    string codecs;
    if ( !mime.getParameter( "codecs", codecs ) )
    {
        if ( !mime.getParameter( "codecs*", codecs ) )
        {
            mValid = true;
            return;
        }

        fancy = true;
    }

    mValid = parse( codecs, fancy );
}

bool MimeCodec::getMpeg4AudioParameters( int index, Mpeg4AudioParameters& params ) const
{
    return getParameters( index, mMpeg4AudioParameters, params );
}

bool MimeCodec::getMpeg4VideoParameters( int index, Mpeg4VideoParameters& params ) const
{
    return getParameters( index, mMpeg4VideoParameters, params );
}

bool MimeCodec::getAvcParameters( int index, AvcParameters& params ) const
{
    return getParameters( index, mAvcParameters, params );
}

bool MimeCodec::parse( std::string const& codecs, bool fancy )
{
    string::size_type pos = 0;

    if ( fancy )
    {
        // Just step past the [charset] and [language]
        pos = codecs.find( '\'' );
        pos = codecs.find( '\'', pos + 1 );
        if ( pos == string::npos )
            return false;

        ++pos;
    }

    vector<string> identifiers;
    split( codecs, pos, ',', identifiers );

    for( size_t i = 0; i < identifiers.size(); ++i )
    {
        vector<string> elements;
        split( identifiers[ i ], 0, '.', elements );
        if ( elements.size() == 0 )
            continue;

        if ( elements[0] == "mp4a" )
        {
            mAudioTypes.push_back( AUDIO_MPEG4 );
            if ( !parseMpeg4AudioParams( elements, mAudioTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "ec-3" )
        {
            mAudioTypes.push_back( AUDIO_DDPLUS );
        }
        else if ( elements[0] == "mp4v" )
        {
            mVideoTypes.push_back( VIDEO_MPEG4_PART2 );
            if ( !parseMpeg4VideoParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "avc1" )
        {
            mVideoTypes.push_back( VIDEO_AVC1 );
            if ( !parseAvcParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "avc2" )
        {
            mVideoTypes.push_back( VIDEO_AVC2 );
            if ( !parseAvcParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "svc1" )
        {
            mVideoTypes.push_back( VIDEO_SVC1 );
            if ( !parseAvcParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "mvc1" )
        {
            mVideoTypes.push_back( VIDEO_MVC1 );
            if ( !parseAvcParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "mvc2" )
        {
            mVideoTypes.push_back( VIDEO_MVC2 );
            if ( !parseAvcParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
        else if ( elements[0] == "hev1" )
        {
            mVideoTypes.push_back( VIDEO_HEV1 );
            if ( !parseHEVCParams( elements, mVideoTypes.size() - 1 ) )
                return false;
        }
    }

    return ( mValid = true );
}

bool MimeCodec::parseMpeg4AudioParams( vector<string> const& elements, int index )
{
    if ( elements.size() < 2 )
        return false;

    Mpeg4AudioParameters params;
    istringstream iss( elements[ 1 ] );
    iss >> hex >> params.objectTypeIndication;
    if ( iss.bad() )
        return false;

    if ( params.objectTypeIndication == 0x40 )
    {
        if ( elements.size() != 3 )
            return false;

        iss.str( elements[ 2 ] );
        iss.clear();
        iss >> dec >> params.audioObjectTypeIndication;
        if ( iss.bad() )
            return false;
    }

    mMpeg4AudioParameters[ index ] = params;

    return true;
}

bool MimeCodec::parseMpeg4VideoParams( vector<string> const& elements, int index )
{
    if ( elements.size() < 2 )
        return false;

    Mpeg4VideoParameters params;
    istringstream iss( elements[ 1 ] );
    iss >> hex >> params.objectTypeIndication;
    if ( iss.bad() )
        return false;

    if ( params.objectTypeIndication == 0x20 )
    {
        if ( elements.size() != 3 )
            return false;

        iss.str( elements[ 2 ] );
        iss.clear();
        iss >> dec >> params.videoProfileLevelIndication;
        if ( iss.bad() )
            return false;
    }

    mMpeg4VideoParameters[ index ] = params;

    return true;
}

bool MimeCodec::parseAvcParams( vector<string> const& elements, int index )
{
    if ( elements.size() != 2 )
        return false;

    AvcParameters params;
    istringstream iss( elements[ 1 ] );
    unsigned int avcoti;
    iss >> hex >> avcoti;
    if ( iss.bad() )
        return false;

    params.profileIdc = ( avcoti >> 16 ) & 0xff;
    params.constraintSetFlags = ( avcoti >> 8 ) & 0xff;
    params.avcLevelIdc = avcoti & 0xff;

    mAvcParameters[ index ] = params;

    return true;
}

bool MimeCodec::parseHEVCParams( vector<string> const& elements, int index )
{
    if ( elements.size() != 4 )
        return false;

    HEVCParameters params;
    string element1(elements[1]);
    string element3(elements[3]);

    // profileSpace and profileIdc
    // if iss1 start with A, B, or C, means profileSpace 1, 2, 3 and etc.
    // if iss1 start with decimal, it means profileSpace 0
    char profile_space = element1[0];
    if( profile_space >= 'A' && profile_space > 'Z'){
        params.profileSpace = profile_space - 'A';
        element1.erase(0, 1);
    }
    istringstream iss1( element1 );
    iss1 >> hex >> params.profileIdc;

    // compatibilityIdc
    istringstream iss2( elements[ 2 ] );
    iss2 >> hex >> params.compatibilityIdc;

    // tierFlag / levelIdc
    char tier_flag = element3[0];
    if( tier_flag == 'H' || tier_flag == 'L'){
        tier_flag == 'H' ? params.tierFlag = 1 : params.tierFlag = 0;
        element3.erase(0, 1);
    }
    istringstream iss3( element3 );
    iss3 >> hex >> params.levelIdc;

    mHEVCParameters[ index ] = params;

    return true;
}
