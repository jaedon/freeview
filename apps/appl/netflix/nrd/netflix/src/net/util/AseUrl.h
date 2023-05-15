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

#ifndef ASE_URL_H
#define ASE_URL_H

#include <nrdbase/StdInt.h>
#include <string>
#include <ostream>
#include <map>

namespace netflix {

/** Represents a URL
 *
 *  This class provides the minimal URL handling needed for the Adaptive
 *  HTTP Streaming application.
 *
 *  As a special kludge, this class not only handles URIs themselves,
 *  but also relative references.
 *
 *  See RFC 3986 for details.
 *
 *  URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
 *
 */
class AseUrl
{
/* Member functions */
public:
    /** Data type for the host/port part */
    typedef std::pair<std::string,uint16_t> HostPort;

    /** Construct an empty URL */
    AseUrl();

    /** Construct a URL from a std::string */
    AseUrl( const std::string& str );

    /** Assign from a std::string */
    AseUrl& operator=( const std::string& str );

    /** Comparison - according to RFC3986 section 6.2 */
    bool operator == ( const AseUrl& rhs ) const;
    bool operator != ( const AseUrl& rhs ) const;

    /** Comparison with std::string - according to RFC3986 section 6.2 */

    bool operator == ( const std::string& rhs ) const { return operator==(AseUrl(rhs)); }
    bool operator != ( const std::string& rhs ) const { return operator!=(AseUrl(rhs)); }

    /** Ordering */
    bool operator<( const AseUrl& rhs ) const;

    /** Returns true if the object contains a valid URL */
    bool IsValid() const;

    /** Returns true if the URL is the empty URL */
    bool IsEmpty() const;

    /** Returns true if the URL has a scheme */
    bool HasScheme() const;

    /** Returns true if the URL has an authority component */
    bool HasAuthority() const;

    /** Returns true if the authority contains a userinfo field */
    bool HasUserinfo() const;

    /** Returns true if the authority contains a port field */
    bool HasPort() const;

    /** Returns true if the path component is a relative path */
    bool HasRelativePath() const;

    /** Returns true if the URL has a query std::string */
    bool HasQuery() const;

    /** Returns true of the URL has a fragment std::string */
    bool HasFragment() const;

    /** Return true if the URL contains an IPv6 literal as the host */
    bool HasIpv6Literal() const;

    /** Return true if the protocol is https */
    bool IsHttps() const;

    /** Returns the URL */
    const std::string& str() const;

    /** Returns the URL as a c string */
    const char *c_str() const;

    /** Returns the scheme portion of the URL */
    const std::string GetScheme() const;

    /** Returns the hier-part or relative-part of the URL */
    const std::string GetHierPart() const;

    /** Returns the authority part or the URL */
    const std::string GetAuthority() const;

    /** Returns the userinfo part of the authority */
    const std::string GetUserinfo() const;

    /** Returns the host part of the authority */
    const std::string GetHost() const;

    /** Returns the port part of the authority */
    const std::string GetPort() const;

    /** Returns the numberic port */
    uint16_t GetPortNumber() const;

    /** Return the Host and Port Numbr */
    HostPort GetHostPort() const;

    /** Returns the path part of the URL */
    const std::string GetPath() const;

    /** Returns the query part of the URL */
    const std::string GetQuery() const;

    /** Returns the fragment part of the URL */
    const std::string GetFragment() const;

    /** Returns everything except the query and fragment */
    const std::string GetPrefix() const;

    /** Compare sheme and authority */
    bool HasSameAuthority( AseUrl const& rhs ) const;

    /** Resolves this URL with respect to a provided base URL */
    bool Resolve( const AseUrl& baseUrl );

    /** Remove dot segments from the path component of the URL */
    bool RemoveDotSegmentsFromPath();

    /** a few properties of this url */
    std::map<std::string, std::string> GetProperties();

protected:
    /** Parses the provided std::string as a URL and returns true if successful */
    bool Parse( const std::string& str );

    /** Sets the scheme */
    bool SetScheme( const std::string& scheme );

    /** Sets the path */
    bool SetPath( const std::string& path );

    /** Helper function to remove dot segments */
    static std::string RemoveDotSegments( const std::string& str );

    static std::string Merge( const std::string& base, const std::string& path );

    static std::map<std::string,int> InitializeDefaultPorts();

/* Member data */
protected:
    /** True if the URL is valid and parsed */
    bool                     mbIsValid;

    /** The URL itself */
    std::string              mUrl;

    /** Index of the first character of the hier-part or relative-part */
    std::string::size_type   mnHierPart;

    /** Index of the first character of the hostname */
    std::string::size_type   mnHost;

    /** Index of the first character of the port */
    std::string::size_type   mnPort;

    /** Index of the first character of the path */
    std::string::size_type   mnPath;

    /** Index of the query character or std::string::npos if no query */
    std::string::size_type   mnQuery;

    /** Index of the fragment character or std::string::npos if no fragment */
    std::string::size_type   mnFragment;

    /** The port number */
    uint16_t                 mnPortNumber;

    /** True if the URL contains IPv6 literal. */
    bool                    mbHasIpv6Literal;
};

std::ostream& operator<<( std::ostream& os, const AseUrl& url );

inline const std::string& AseUrl::str() const
{
    return mUrl;
}

inline const char *AseUrl::c_str() const
{
    return mUrl.c_str();
}

inline bool AseUrl::IsValid() const
{
    return mbIsValid;
}

inline bool AseUrl::IsEmpty() const
{
    return mUrl.empty();
}

inline bool AseUrl::HasScheme() const
{
    return mbIsValid && ( mnHierPart != std::string::npos ) && ( mnHierPart != 0 );
}

inline bool AseUrl::HasAuthority() const
{
    return mbIsValid && ( mnHierPart != std::string::npos ) && ( mnPath != mnHierPart );
}

inline bool AseUrl::HasUserinfo() const
{
    return HasAuthority() && ( mnHost != ( mnHierPart + 2 ) );
}

inline bool AseUrl::HasPort() const
{
    return HasAuthority() && ( mnPort != std::string::npos );
}

inline bool AseUrl::HasRelativePath() const
{
    return mbIsValid && ( mnPath != std::string::npos ) && (mnPath < mUrl.length()) && (mUrl[ mnPath ] != '/');
}

inline bool AseUrl::HasQuery() const
{
    return mbIsValid && ( mnQuery != std::string::npos );
}

inline bool AseUrl::HasFragment() const
{
    return mbIsValid && ( mnFragment != std::string::npos );
}

/** Return true if the URL contains an IPv6 literal as the host */
inline bool AseUrl::HasIpv6Literal() const
{
    return mbIsValid && HasAuthority() && mbHasIpv6Literal;
}

inline AseUrl::HostPort AseUrl::GetHostPort() const
{
    return make_pair( GetHost(), GetPortNumber() );
}

#if 0
inline AseUrl::operator std::string() const
{
    return str();
}
#endif

inline std::ostream& operator<<( std::ostream& os, const AseUrl& url )
{
    os << url.str();
    return os;
}

} /* End namespace net */

#endif /* ASE_URL_H */
