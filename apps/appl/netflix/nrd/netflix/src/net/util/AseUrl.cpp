/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
/*
 * This software module was originally developed by Qualcomm, Inc. in
 * the course of development of the ISO/IEC MPEG-B DASH standard for
 * reference purposes and its performance may not have been optimized.
 * This software module is an implementation of one or more tools as
 * specified by the ISO/IEC MPEG-B DASH standard.
 *
 * ISO/IEC gives users free license to this software module or
 * modifications thereof for use in products claiming conformance to
 * audiovisual and image-coding related ITU Recommendations and/or ISO/IEC
 * International Standards.
 *
 * ISO/IEC gives users the same free license to this software module or
 * modifications thereof for research purposes and further ISO/IEC
 * standardisation.
 *
 * Those intending to use this software module in products are advised that
 * its use may infringe existing patents. ISO/IEC have no liability for use
 * of this software module or modifications thereof.  Copyright is not
 * released for products that do not conform to audiovisual and
 * image-coding related ITU Recommendations and/or ISO/IEC International
 * Standards.
 *
 * Qualcomm, Inc. retains full right to modify and use the code for its own
 * purpose, assign or donate the code to a third party and to inhibit third
 * parties from using the code for products that do not conform to
 * audiovisual and image-coding related ITU Recommendations and/or ISO/IEC
 * International Standards.
 *
 * This copyright notice must be included in all copies or derivative
 * works.  Copyright (c) ISO/IEC 2010.
 */

/*
 * This module has been modified from its original version by Netflix
 */

#include <nrdbase/StringUtils.h>
#include "AseUrl.h"

#include <algorithm>
#include <cctype>

#include <assert.h>

using namespace std;

namespace netflix {

const char sSchemeChars[] = "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789+-.";

AseUrl::AseUrl()
    : mbIsValid( true )
    , mnHierPart( string::npos )
    , mnHost( string::npos )
    , mnPort( string::npos )
    , mnPath( string::npos )
    , mnQuery( string::npos )
    , mnFragment( string::npos )
    , mnPortNumber( 0 )
    , mbHasIpv6Literal( false )
{
}

AseUrl::AseUrl( const string& str )
    : mbIsValid( false )
    , mnHierPart( string::npos )
    , mnHost( string::npos )
    , mnPort( string::npos )
    , mnPath( string::npos )
    , mnQuery( string::npos )
    , mnFragment( string::npos )
    , mnPortNumber( 0 )
    , mbHasIpv6Literal( false )
{
    (void) Parse( str );
}

AseUrl& AseUrl::operator=( const string& str )
{
    (void) Parse( str );
    return *this;
}

bool AseUrl::operator == ( const AseUrl& rhs ) const
{
    return !HasRelativePath() && !rhs.HasRelativePath()
            && GetScheme() == rhs.GetScheme()
            && GetHost() == rhs.GetHost()
            && GetPortNumber() == rhs.GetPortNumber()
            && GetPath() == rhs.GetPath()
            && GetQuery() == rhs.GetQuery();
}

bool AseUrl::operator != ( const AseUrl& rhs ) const
{
    return !(*this == rhs.mUrl);
}

bool AseUrl::operator<( AseUrl const& rhs ) const
{
    if ( !IsValid() || !rhs.IsValid() )
        return false;

    string::size_type len1 = ( mnPort != string::npos ? mnPort - 1 : mnPath );
    string::size_type len2 = ( rhs.mnPort != string::npos ? rhs.mnPort - 1 : rhs.mnPath );
    int cmp = mUrl.compare( 0, len1, rhs.mUrl, 0, len2 );
    if ( cmp != 0 )
        return ( cmp < 0 );

    if ( GetPortNumber() != rhs.GetPortNumber() )
        return GetPortNumber() < rhs.GetPortNumber();

    if ( mnPath == string::npos || rhs.mnPath == string::npos )
        return ( mnPath == string::npos && rhs.mnPath != string::npos );

    len1 = ( ( mnFragment != string::npos ) ? mnFragment : mUrl.size() ) - mnPath;
    len2 = ( ( rhs.mnFragment != string::npos ) ? rhs.mnFragment : rhs.mUrl.size() ) - rhs.mnPath;

    return mUrl.compare( mnPath, len1, rhs.mUrl, rhs.mnPath, len2 ) < 0;
}

/** Return true if the protocol is https */
bool AseUrl::IsHttps() const
{
    return HasScheme()
           && (strCaseCmp( GetScheme(), "https" ) == 0);
}

const string AseUrl::GetScheme() const
{
    return HasScheme() ? mUrl.substr( 0, mnHierPart - 1 ) : string();
}

const string AseUrl::GetHierPart() const
{
    if ( !mbIsValid || ( mnHierPart == string::npos ) )
        return string();

    return mUrl.substr( mnHierPart, mnQuery != string::npos ? mnQuery - mnHierPart
                                    : mnFragment != string::npos ? mnFragment - mnHierPart
                                    : string::npos );
}

const string AseUrl::GetAuthority() const
{
    if ( !HasAuthority() )
        return string();
    assert( mnPath != string::npos );
    return mUrl.substr( mnHierPart + 2, mnPath != string::npos ? mnPath - mnHierPart - 2
                                      : mnQuery != string::npos ? mnQuery - mnHierPart - 2
                                      : mnFragment != string::npos ? mnFragment - mnHierPart - 2
                                      : string::npos );
}

const string AseUrl::GetUserinfo() const
{
    return HasUserinfo() ? mUrl.substr( mnHierPart + 2, mnHost - mnHierPart - 3 ) : string();
}

const string AseUrl::GetHost() const
{
    string retVal = "";

    if (HasAuthority())
    {
        if (mUrl[mnHost] == '[')
        {
            retVal = mUrl.substr( mnHost + 1, mnPort != string::npos ? mnPort - mnHost - 3 : mnPath - mnHost - 2 );
        }
        else
        {
            retVal = mUrl.substr( mnHost, mnPort != string::npos ? mnPort - mnHost - 1 : mnPath - mnHost );
        }
    }

    return retVal;
}

const string AseUrl::GetPort() const
{
    return HasPort() ? mUrl.substr( mnPort, mnPath - mnPort ) : string();
}

uint16_t AseUrl::GetPortNumber() const
{
    return mnPortNumber;
}

map<string,int> AseUrl::InitializeDefaultPorts()
{
    map<string,int> defaultPorts;
    defaultPorts[ "http" ] = 80;
    defaultPorts[ "https" ] = 443;
    return defaultPorts;
}

const string AseUrl::GetPath() const
{
    if ( !mbIsValid || ( mnPath == string::npos ) )
        return string();

    return mUrl.substr( mnPath, mnQuery != string::npos ? mnQuery - mnPath
                                    : mnFragment != string::npos ? mnFragment - mnPath
                                    : string::npos );
}

const string AseUrl::GetQuery() const
{
    return HasQuery() ? mUrl.substr( mnQuery + 1,
                            mnFragment == string::npos ?
                              string::npos : mnFragment - mnQuery - 1 )
                      : string();
}

const string AseUrl::GetFragment() const
{
    return HasFragment() ? mUrl.substr( mnFragment + 1 ) : string();
}

const string AseUrl::GetPrefix() const
{
    return mbIsValid ? mUrl.substr( 0, mnQuery ) : string();
}

bool AseUrl::HasSameAuthority( AseUrl const& rhs ) const
{
    return HasScheme() && rhs.HasScheme()
            && GetScheme() == rhs.GetScheme()
            && HasAuthority() && rhs.HasAuthority()
            && GetHost() == rhs.GetHost()
            && GetPortNumber() == rhs.GetPortNumber();
}

bool AseUrl::SetScheme( const string& scheme )
{
    if ( !mbIsValid || scheme.find_first_not_of( sSchemeChars ) != string::npos )
        return false;

    mUrl.replace( 0, mnHierPart, scheme + ':' );
    int diff = scheme.size() + 1 - mnHierPart;

    mnHierPart += diff;
    if ( mnHost != string::npos ) mnHost += diff;
    if ( mnPort != string::npos ) mnPort += diff;
    if ( mnPath != string::npos ) mnPath += diff;
    if ( mnQuery != string::npos ) mnQuery += diff;
    if ( mnFragment != string::npos ) mnFragment += diff;

    return true;
}

bool AseUrl::SetPath( const string& path )
{
    if ( !mbIsValid )
        return false;

    string::size_type len = ( mnQuery != string::npos ) ? mnQuery - mnPath
                                : ( mnFragment != string::npos ) ? mnFragment - mnPath
                                : string::npos;

    mUrl.replace( mnPath, len, path );

    if ( len != string::npos )
    {
        int diff = path.size() - len;
        if ( mnQuery != string::npos ) mnQuery += diff;
        if ( mnFragment != string::npos ) mnFragment += diff;
    }

    return true;
}

bool AseUrl::Resolve( const AseUrl& baseUrl )
{
    // Following RFC3986 Section 5.2
    if ( !baseUrl.HasScheme() )
        return false;

    if ( ( !HasScheme() || GetScheme() == baseUrl.GetScheme() )
            && !HasAuthority() )
    {
        string target = baseUrl.GetScheme() + "://" + baseUrl.GetAuthority();

        if ( GetPath().size() == 0 )
        {
            target = baseUrl.GetPath();
            if ( HasQuery() || baseUrl.HasQuery() )
            {
                target += '?';
                target += ( HasQuery() ? GetQuery() : baseUrl.GetQuery() );
            }
        }
        else
        {
            if ( mUrl[ mnPath ] == '/' )
            {
                target += RemoveDotSegments( GetPath() );
            }
            else
            {
                target += Merge( baseUrl.GetPath(), GetPath() );
            }
            if ( HasQuery() )
            {
                target += '?';
                target += GetQuery();
            }
        }

        if ( HasFragment() )
        {
            target += '#';
            target += GetFragment();
        }

        Parse( target );
    }
    else if ( HasAuthority() )
    {
        SetScheme( baseUrl.GetScheme() );
    }

    return (mbIsValid = RemoveDotSegmentsFromPath());
}

bool AseUrl::RemoveDotSegmentsFromPath()
{
    return SetPath( RemoveDotSegments( GetPath() ) );
}

string AseUrl::RemoveDotSegments( const string& str )
{
    string in( str ), out;

    while( in.length() > 0 )
    {
        if ( in == "." || in == ".." )
            break;
        else if ( in.compare( 0, 3, "../" ) == 0 )
            in.erase( 0, 3 );
        else if ( in.compare( 0, 2, "./" ) == 0 )
            in.erase( 0, 2 );
        else if ( in.compare( 0, 2, "/." ) == 0
                        && ( in.length() == 2 || in[2] == '/' ) )
            in.erase( 1, 2 );
        else if ( in.compare( 0, 3, "/.." ) == 0
                        && ( in.length() == 3 || in[3] == '/' ) )
        {
            in.erase( 1, 3 );
            string::size_type idx = out.rfind( '/' );
            if ( idx == string::npos )
                out.clear();
            else
                out.erase( idx );
        }
        else
        {
            string::size_type idx = in.find( '/', 1 );
            out += in.substr( 0, idx );
            in.erase( 0, idx );
        }
    }

    return out;
}

string AseUrl::Merge( const string& base, const string& path )
{
    string out;
    if ( base.empty() )
    {
        out = '/';
    }
    else
    {
        string::size_type idx = base.rfind( '/' );
        if ( idx != string::npos )
            out = base.substr( 0, idx + 1 );
    }

    out += path;

    return out;
}

bool AseUrl::Parse( const string& str )
{
    static map<string,int> sDefaultPorts = InitializeDefaultPorts();

    mUrl.clear();

    mbHasIpv6Literal = false;

    string::size_type slash = str.find( '/' );
    string::size_type scheme = str.find( ':' );

    mnHierPart = ( scheme != string::npos ) && ( scheme < slash )
                                                            ? scheme + 1 : 0;

    mnQuery = str.find( '?', mnHierPart );
    mnFragment = str.find( '#', mnQuery == string::npos ? mnHierPart : mnQuery );

    if ( str.size() >= ( mnHierPart + 2 ) && str[ mnHierPart ] == '/' && str[ mnHierPart + 1 ] == '/' )
    {
        // authority path-abempty
        slash = str.find( '/', mnHierPart + 2 );
        mnPath = slash < mnQuery ? slash : mnQuery;
        if( mnPath == string::npos )
            mnPath = str.length();

        string::size_type at = str.find( '@', mnHierPart + 2 );
        mnHost = ( at != string::npos && at < slash )
                                                ? at + 1 : mnHierPart + 2;

        if ( str[ mnHost ] == '[' )
        {
            string::size_type close = str.find( ']', mnHost );
            if ( close == string::npos )
                return (mbIsValid = false);

            mbHasIpv6Literal = true;

            mnPort = str[ close + 1 ] == ':' ? close + 2 : string::npos;
        }
        else
        {
            string::size_type port = str.find( ':', mnHost );
            mnPort = port != string::npos && port < mnPath ? port + 1 : string::npos;
        }
    }
    else
    {
        // path-absolute, path-rootless, path-noscheme or path-empty
        mnPath = mnHierPart;
    }

    mUrl = str;
    mbIsValid = true;

    // Normalize the scheme and host
    if ( HasScheme() )
#if defined(__psp2__)
        std::transform( mUrl.begin(), mUrl.begin() + mnHierPart - 1, mUrl.begin(), tolower );
#else
        std::transform( mUrl.begin(), mUrl.begin() + mnHierPart - 1, mUrl.begin(), std::ptr_fun<int,int>(tolower) );
#endif

    if ( HasAuthority() )
    {
        string::iterator begin = mUrl.begin() + mnHost;
        string::iterator end = mUrl.begin() + ( mnPort != string::npos ? mnPort : mnPath );
#if defined(__psp2__)
        std::transform( begin, end, begin, tolower );
#else
		std::transform( begin, end, begin, std::ptr_fun<int,int>(tolower) );
#endif
    }

    // Get the port
    if ( !HasPort() )
    {
        map<string,int>::iterator it = sDefaultPorts.find( GetScheme() );
        mnPortNumber = ( it != sDefaultPorts.end() ) ? it->second : 0;
    }
    else
    {
        mnPortNumber = (uint16_t) strtoul( GetPort().c_str(), NULL, 10 );
    }

    return mbIsValid;
}

} // end namespace net
