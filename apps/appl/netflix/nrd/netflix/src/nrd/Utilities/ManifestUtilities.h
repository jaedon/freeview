/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MANIFEST_UTILITIES_H__
#define __MANIFEST_UTILITIES_H__

#include <nrdbase/NFErr.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/Manifest.h>

namespace netflix {

class Variant;


namespace util {

class ManifestUtilities
{
public:
    // create a manifest from a Variant
    static NFErr createManifestFromVariant(/*out*/shared_ptr<ase::Manifest> &manifest,
                                           /*out*/bool &fastplayPossible,
                                           const Variant &var, bool drmOnly);

    // StreamKey
    static std::string encodeStreamKey(const std::string &downloadableId,
                                       ContentProfile profile);
    static bool parseStreamKey(/*out*/std::string &downloadableId,
                               /*out*/ContentProfile &profile,
                               const std::string &streamKey);
};

}}

#endif // __MANIFEST_UTILITIES_H__
