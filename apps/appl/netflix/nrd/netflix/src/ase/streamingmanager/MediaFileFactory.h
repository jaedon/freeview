/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_FILE_FACTORY_H
#define MEDIA_FILE_FACTORY_H

#include <nrdase/MimeType.h>
#include <nrdase/Manifest.h>
#include <nrdase/IMediaFile.h>
#include <nrdase/IAseConfigParameter.h>

namespace netflix {
namespace ase {

/** Singleton object managing the different kinds of media file */
class MediaFileFactory
{
public:
    typedef AseErrorCode (*ConstructFnPtr)(  IAseConfigParameterPtr pConfigParameters,
                                        ConstManifestStreamPtr pManifestSteam,
                                        IMediaFilePtr& pMediaFile );

public:

    static AseErrorCode createMediaFile( IAseConfigParameterPtr pConfigParameters,
                                    ConstManifestStreamPtr pManifestSteam,
                                    IMediaFilePtr& pMediaFile );

    static bool canPlayType( MimeType const& mime );

    static void registerMimeType( MimeType const& mime, ConstructFnPtr );

    MediaFileFactory();

protected:
    AseErrorCode createMediaFileImpl(    IAseConfigParameterPtr pConfigParameters,
                                    ConstManifestStreamPtr pManifestSteam,
                                    IMediaFilePtr& pMediaFile );

    bool canPlayTypeImpl( MimeType const& mime );

    void registerMimeTypeImpl( MimeType const& mime, ConstructFnPtr pFunction );

protected:
    static MediaFileFactory sInstance;

    typedef std::vector<std::pair<MimeType,ConstructFnPtr> > MimeTypeTable;

    MimeTypeTable mMimeTypes;
};


inline AseErrorCode MediaFileFactory::createMediaFile(
                                    IAseConfigParameterPtr pConfigParameters,
                                    ConstManifestStreamPtr pManifestSteam,
                                    IMediaFilePtr& pMediaFile )
{
    return sInstance.createMediaFileImpl( pConfigParameters, pManifestSteam, pMediaFile );
}

inline void MediaFileFactory::registerMimeType( MimeType const& mime, ConstructFnPtr pFunction )
{
    return sInstance.registerMimeTypeImpl( mime, pFunction );
}

inline bool MediaFileFactory::canPlayType( MimeType const& mime )
{
    return sInstance.canPlayTypeImpl( mime );
}

inline void MediaFileFactory::registerMimeTypeImpl( MimeType const& mime, ConstructFnPtr pFunction )
{
    mMimeTypes.push_back( std::make_pair( mime, pFunction ) );
}

}}
#endif
