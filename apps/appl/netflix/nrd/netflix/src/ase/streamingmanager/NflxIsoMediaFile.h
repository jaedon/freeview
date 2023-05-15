/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NFLX_ISO_MEDIA_FILE_H
#define NFLX_ISO_MEDIA_FILE_H

#include "IsoMediaFile.h"
#include "DashIsoMediaFile.h"
#include <nrdase/DebugLogging.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( NflxIsoMediaFile );

class NflxIsoMediaFile : public DashIsoMediaFile
{
public:
    typedef shared_ptr<NflxIsoMediaFile> NflxIsoMediaFilePtr;
    friend struct NflxBrandHandler;
    friend struct NflxHeaderBoxHandler;
    friend class NflxIndexBoxHandler;

    // NflxIsoMediaFile

    /* Construct a new NflxIsoMediaFile object
     * @param[in] pManifestStream The stream details for the object
     * @param[out] pIsoMediaFile Pointer to the newly constructed object
     * @return 0 on success
     */
    static AseErrorCode construct(  IAseConfigParameterPtr pConfigParameters,
                                    ConstManifestStreamPtr pManifestSteam,
                                    IMediaFilePtr& pIsoMediaFile );

public:
    virtual ~NflxIsoMediaFile();

protected:
    NflxIsoMediaFile( ConstManifestStreamPtr pManifestSteam );

    AseErrorCode initialize( IAseConfigParameterPtr pConfigParameters );

    AseErrorCode handleNflxBrand( bool& index );

    AseErrorCode handleNflxHeaderBox( BoxReader const& reader );

    AseErrorCode handleNflxIndexBox( BoxReader const& reader );

protected:
    IsoMediaFile::BrandHandler*         mpNflxBrandHandler;
    IsoMediaFile::BoxHandlerFactory*    mpNflxBoxHandlerFactory;
    ullong                            mIndexOffset;
    uint32_t                            mIndexSize;
    bool                                mbNeedNflxIndex;
};

typedef NflxIsoMediaFile::NflxIsoMediaFilePtr NflxIsoMediaFilePtr;

}} // namespace netflix::ase

#endif
