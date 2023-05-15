/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaFileFactory.h"
#include "DashIsoMediaFile.h"
#include "NflxIsoMediaFile.h"
#include <nrdbase/AseErrorCodes.h>

using namespace std;
using namespace netflix::ase;

MediaFileFactory MediaFileFactory::sInstance;

AseErrorCode MediaFileFactory::createMediaFileImpl(
                            IAseConfigParameterPtr pConfigParameters,
                            ConstManifestStreamPtr pManifestSteam,
                            IMediaFilePtr& pMediaFile )
{
    MimeType const& mime = pManifestSteam->getMimeType();
    MimeTypeTable::iterator it = mMimeTypes.begin();
    for( ; it != mMimeTypes.end(); ++it )
    {
        if ( it->first.contains( mime ) )
        {
            return (*it->second)( pConfigParameters, pManifestSteam, pMediaFile );
        }
    }

    return AS_UNSUPPORTED_MIME_TYPE;
}

bool MediaFileFactory::canPlayTypeImpl( MimeType const& mime )
{
    MimeTypeTable::iterator it = mMimeTypes.begin();
    for( ; it != mMimeTypes.end(); ++it )
    {
        if ( it->first.contains( mime ) )
            return true;
    }

    return false;
}

MediaFileFactory::MediaFileFactory()
{
    registerMimeTypeImpl( MimeType( "video/x-nflx-mp4" ), &NflxIsoMediaFile::construct );
    //registerMimeTypeImpl( MimeType( "video/mp4" ), &DashIsoMediaFile::construct );
    //registerMimeTypeImpl( MimeType( "video/3gpp" ), &DashIsoMediaFile::construct );
    //registerMimeTypeImpl( MimeType( "video/3gpp2"), &DashIsoMediaFile::construct );
    registerMimeTypeImpl( MimeType( "audio/mp4" ), &NflxIsoMediaFile::construct );
    //registerMimeTypeImpl( MimeType( "audio/3gpp" ), &DashIsoMediaFile::construct );
    //registerMimeTypeImpl( MimeType( "audio/3gpp2" ), &DashIsoMediaFile::construct );
}
