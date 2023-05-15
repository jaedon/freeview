/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/MimeType.h>

#include <sstream>

using namespace netflix::ase;
using namespace std;


static string strip( string const& str )
{
    string::size_type a = str.find_first_not_of( " \t" );
    string::size_type b = str.find_last_not_of( " \t" );
    return ( b == string::npos ) ? string() : str.substr( a, b - a + 1 );
}

static void unescape( string& str )
{
    string result;
    string::size_type pos = 0, pos2;
    do
    {
        pos2 = str.find( '\\', pos );
        result += str.substr( pos, ( pos2 == string::npos ? string::npos : pos2 - pos ) );
        pos = pos2 + 1;

    } while( pos2 != string::npos );
    str = result;
}

MimeType::MimeTypesMap* MimeType::initializeTypesMap()
{
    MimeTypesMap& map = *( new MimeTypesMap );
    map[ "application" ] = MIME_TYPE_APPLICATION;
    map[ "video" ] = MIME_TYPE_VIDEO;
    map[ "audio" ] = MIME_TYPE_AUDIO;
    map[ "image" ] = MIME_TYPE_IMAGE;
    map[ "text" ] = MIME_TYPE_TEXT;
    return &map;
}

MediaType MimeType::getMediaType() const
{
    return ( mType == MIME_TYPE_UNKNOWN ) ? MEDIA_UNKNOWN
            : ( mType == MIME_TYPE_AUDIO ) ? MEDIA_AUDIO
            : ( mType == MIME_TYPE_VIDEO ) ? MEDIA_VIDEO
            : ( mType == MIME_TYPE_IMAGE ) ? MEDIA_VIDEO
            : MEDIA_TEXT;
}

bool MimeType::getParameter( string const& name, string& value ) const
{
    map<string,string>::const_iterator it = mParameters.find( name );
    if ( it == mParameters.end() )
        return false;

    value = it->second;
    return true;
}

bool MimeType::contains( MimeType const& other )
{
    if ( getType() != other.getType() )
        return false;

    if ( getSubtype() != other.getSubtype() )
        return false;

    return true;
}

bool MimeType::parse( string const& mime )
{
    static std::auto_ptr<const MimeTypesMap> sMimeTypesMapPtr( initializeTypesMap() );
    string::size_type pos = mime.find( '/' );
    if ( pos == string::npos )
        return (mValid = false);

    string type = strip( mime.substr( 0, pos ) );
    MimeTypesMap::const_iterator it = sMimeTypesMapPtr->find( type );
    if ( it == sMimeTypesMapPtr->end() )
        return (mValid = false);

    mType = it->second;

    string::size_type pos2 = mime.find_first_of( " \t;", pos );
    string::size_type n =
        ( pos2 == string::npos ) ? string::npos : pos2 - ( pos + 1 );

    mSubtype = strip( mime.substr( pos + 1, n ) );

    if ( pos2 != string::npos )
    {
        pos = mime.find( ';', pos2 );
        while( pos != string::npos )
        {
            pos2 = mime.find( '=', pos );
            string name = strip( mime.substr( pos + 1, pos2 - pos - 1 ) );

            string value;
            ++pos2;
            pos = mime.find_first_not_of( " \t", pos2 );
            if ( pos == string::npos || mime[ pos ] == ';' )
            {
                mParameters.insert( make_pair( name, value ) );
                continue;
            }

            // If it's a quoted string, then find the end
            if ( mime[ pos ] == '"' )
            {
                pos2 = pos + 1;
                while( true )
                {
                    pos2 = mime.find( '"', pos2 );
                    if ( pos2 == string::npos )
                        return (mValid = false);

                    if ( mime[ pos2 - 1 ] != '\\' )
                        break;
                    ++pos2;
                }

                value = mime.substr( pos + 1, ( pos2 - pos - 1 ) );
                pos = mime.find( ';', pos2 );
            }
            else
            {
                // Find the next parameter
                pos2 = mime.find( ';', pos );
                value = strip( mime.substr( pos, ( pos2 == string::npos ) ? string::npos : pos2 - pos ) );
                pos = pos2;
            }

            unescape( value );

            mParameters.insert( make_pair( name, value ) );
        }
    }

    return (mValid = true);
}
