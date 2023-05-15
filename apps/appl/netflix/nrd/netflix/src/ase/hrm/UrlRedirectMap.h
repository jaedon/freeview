/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef URL_REDIRECT_MAP_H
#define URL_REDIRECT_MAP_H

#include <nrdase/AseCommonDataTypes.h>

#include <nrdnet/AseUrl.h>

#include <map>

namespace netflix {
namespace ase {
/** @class UrlRedirectMap
 *
 *  This class maintains a map of URL redirects and URL failures. Since we have several
 *  download processes operating in parallel (although all on the same thread) we use this
 *  class as a central point for recording and looking up redirects for a session.
 *
 *  We also detect redirect loops here, where the servers are redirecting us through
 *  a repeating sequence of URLs.
 */
class UrlRedirectMap
{
public:

    typedef shared_ptr<UrlRedirectMap> UrlRedirectMapPtr;

    UrlRedirectMap( uint32_t maxRedirects = 25 );

    ~UrlRedirectMap();

    /** Add an URL redirect entry.
     *
     *  @param originalUrl [In] The original URL.
     *  @param destinationUrl [In] The URL redirect destination.
     *  @return True if the entry could be added, false if too many redirects
     */
    bool addRedirect( AseUrl const& original, AseUrl const& destination );

    /** Remove an URL redirect entry.
     *
     *  @param originalUrl [In] The original URL.
     */
    void removeRedirect( AseUrl const& original );

    /** Check if an URL is redirected.
     *
     *  @param originalUrl [In] The original URL.
     */
    bool isRedirected( AseUrl const& original ) const;

    /** Get the  URL redirect destination for a given original. if there is no URL redirect,
     *  the original URL is returned.
     *
     *  @param originalUrl [In] The original URL.
     *  @return The redirected-to location, if redirected, or the original URL is not.
     */
    AseUrl getRedirect( AseUrl const& original ) const;

    /** Add a URL failure entry
     *
     *  @param url [in] The failed URL
     *  @param status [in] The failure status
     */
    void addFailure( AseUrl const& url, uint32_t status );

    /** Remove a URL failure entry
     *  @param url [in] The now-working-again URL
     */
    void removeFailure( AseUrl const& url );

    /** Test is a URL is failed
     *  @param url [in] The URL to check
     *  @return True if the URL has failed
     */
    bool isFailed( AseUrl const& url ) const;

    int32_t getFailureStatus( AseUrl const& url, uint32_t& status ) const;

protected:
    /*
     *  Map from Original URL -> ( redirect count, destination URL or empty URL )
     *
     * The redirect count is the number of redirects to get to the Original URL and
     * is used to check when we exceed the redirect count.
     *
     */
    typedef std::map<AseUrl,std::pair<uint32_t,AseUrl> > UrlRedirectMapType;
    typedef std::map<AseUrl,uint32_t> UrlFailureMapType;

    uint32_t const      mMaximumRedirects;

    UrlRedirectMapType  mUrlRedirectMap;

    UrlFailureMapType   mUrlFailureMap;

};

typedef UrlRedirectMap::UrlRedirectMapPtr UrlRedirectMapPtr;

inline UrlRedirectMap::UrlRedirectMap( uint32_t maxRedirects )
    : mMaximumRedirects( maxRedirects )
{
}

inline UrlRedirectMap::~UrlRedirectMap()
{
}

} // namespace ase
} // namespace netflix

#endif
