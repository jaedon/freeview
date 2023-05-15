/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef DASH_ISO_MEDIA_FILE_H
#define DASH_ISO_MEDIA_FILE_H

#include "IsoMediaFile.h"

namespace netflix {
namespace ase {

class DashIsoMediaFile : public IsoMediaFile
{
public:
    friend struct DashBrandHandler;
    friend class SidxBoxHandler;

    typedef shared_ptr<DashIsoMediaFile> DashIsoMediaFilePtr;

    static AseErrorCode construct(  IAseConfigParameterPtr pConfigParameters,
                                    ConstManifestStreamPtr pManifestSteam,
                                    IMediaFilePtr&         pIsoMediaFile );

    AseErrorCode handleDashBrand( bool& index );

    virtual ~DashIsoMediaFile();

protected:
    DashIsoMediaFile( ConstManifestStreamPtr pManifestSteam );

    AseErrorCode initialize( IAseConfigParameterPtr pConfigParameters );

protected:
    std::auto_ptr<IsoMediaFile::BrandHandler>         mpDashBrandHandler;
    std::auto_ptr<IsoMediaFile::BoxHandlerFactory>    mpSidxBoxHandlerFactory;
};

typedef DashIsoMediaFile::DashIsoMediaFilePtr DashIsoMediaFilePtr;

} // namespace ase
} // namespace netflix

#endif /* DASH_ISO_MEDIA_FILE_H */
