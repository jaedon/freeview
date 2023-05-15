/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "UrlRedirectMap.h"
#include <nrdbase/AseErrorCodes.h>

using namespace netflix;
using namespace ase;
using namespace std;

/** Add a URL redirect entry. */
bool UrlRedirectMap::addRedirect(   const AseUrl& originalUrl,
                                    const AseUrl& destinationUrl )
{
    // Create or find an entry for the original URL to give us the redirect depth
    uint32_t orginalDepth = 0;
    UrlRedirectMapType::iterator urlRedirectItr = mUrlRedirectMap.find( originalUrl );
    if ( urlRedirectItr == mUrlRedirectMap.end() )
    {
        mUrlRedirectMap[ originalUrl ] = make_pair( 0, destinationUrl );
    }
    else
    {
        orginalDepth = urlRedirectItr->second.first;
        urlRedirectItr->second.second = destinationUrl;
    }

    if ( orginalDepth >= mMaximumRedirects ) return false;

    // Set the depth and clear the URL for the destinationUrl
    // This clears any previously received redirect for the desintationUrl,
    // ensuring that we never create loops in our data structure
    mUrlRedirectMap[ destinationUrl ] = make_pair( orginalDepth + 1, AseUrl() );

    return true;
}

/** Remove a URL redirect entry and all subsequent redirects. */
void UrlRedirectMap::removeRedirect( const AseUrl& originalUrl )
{
    UrlRedirectMapType::iterator urlRedirectItr = mUrlRedirectMap.find( originalUrl );
    bool first = true;

    while ( urlRedirectItr != mUrlRedirectMap.end() )
    {
        AseUrl const nexturl = urlRedirectItr->second.second;

        if ( first )
        {
            urlRedirectItr->second.second = AseUrl();
            first = false;
        }
        else
        {
            mUrlRedirectMap.erase( urlRedirectItr );
        }

        if ( nexturl.IsEmpty() ) break;

        urlRedirectItr = mUrlRedirectMap.find( nexturl );
    }
}

/** Check if a URL is redirected. */
bool UrlRedirectMap::isRedirected( const AseUrl& url ) const
{
    UrlRedirectMapType::const_iterator const urlRedirectItr = mUrlRedirectMap.find( url );
    return ( urlRedirectItr != mUrlRedirectMap.end() && !urlRedirectItr->second.second.IsEmpty() );
}

/** Get the  URL redirect destination for a given original. */
AseUrl UrlRedirectMap::getRedirect( const AseUrl& originalUrl ) const
{
    AseUrl retUrl = originalUrl;

    UrlRedirectMapType::const_iterator urlRedirectItr;

    do
    {
        urlRedirectItr = mUrlRedirectMap.find( retUrl );
        if ( urlRedirectItr == mUrlRedirectMap.end() || urlRedirectItr->second.second.IsEmpty() )
            break;

        retUrl = urlRedirectItr->second.second;
    } while( true );

    return retUrl;
}



/** Add a URL failure entry */
void UrlRedirectMap::addFailure(    const AseUrl& url,
                                    uint32_t status )
{
    mUrlFailureMap[ url ] = status;
}

/** Remove a URL failure entry */
void UrlRedirectMap::removeFailure( const AseUrl& url )
{
    UrlFailureMapType::iterator const it = mUrlFailureMap.find( url );

    if (it != mUrlFailureMap.end())
    {
        mUrlFailureMap.erase( it );
    }
}

bool UrlRedirectMap::isFailed( AseUrl const& url ) const
{
    return ( mUrlFailureMap.find( url ) != mUrlFailureMap.end() );
}

/** Check is a URL is failed */
int32_t UrlRedirectMap::getFailureStatus(
                                    const AseUrl& url,
                                    uint32_t& status ) const
{
    int32_t retVal = AS_NOT_FOUND;

    UrlFailureMapType::const_iterator const it = mUrlFailureMap.find( url );

    if (it != mUrlFailureMap.end())
    {
        status = it->second;

        retVal = AS_NO_ERROR;
    }

    return retVal;
}

