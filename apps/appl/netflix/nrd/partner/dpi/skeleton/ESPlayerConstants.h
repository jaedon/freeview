/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ESPLAYERCONSTANTS_H
#define ESPLAYERCONSTANTS_H

/* @file ESPlayerConstants.h - Contains constants specific to the reference
 * implementation of the IElementaryStreamPlayer interface.  A device
 * implementation may modify this header and accompanying .cpp files as
 * needed.
 */

#include <nrdbase/Thread.h>
#include <nrdbase/Time.h>
#include <nrddpi/Common.h>
#include <nrdbase/Exportable.h>

namespace netflix  {
namespace device   {
namespace esplayer {

class NRDP_EXPORTABLE ESPlayerConstants
{
public:
    // ES Player Settings
    static const Time WAIT_FOR_FLUSH_OFF;
    static const Time WAIT_FOR_FLUSH_END;
};

// We reserve the top 16 bits in ref-app specific error codes for
// flags that indicate error categories areas.
enum ESNativeError
{
    skeletonDeviceSpecific_NoError = 0,

    // related to drm
    skeletonDeviceSpecific_UnknownPrectionSystemId,
    skeletonDeviceSpecific_NoDecryptionContext,
    skeletonDeviceSpecific_WrongSubsampleMapping,
    skeletonDeviceSpecific_NoDrmSystemFound,
    skeletonDeviceSpecific_NoDrmSessionFound,
    skeletonDeviceSpecific_InvalidDrmSecureStopId,

    // related to decoding
    skeletonDeviceSpecific_WrongBitDepth,
    skeletonDeviceSpecific_MissingSampleAttributes,
    skeletonDeviceSpecific_UpdateAttributesFailed,

    // related to player
    skeletonDeviceSpecific_NoInitialStreamAttributes,
    skeletonDeviceSpecific_NoInitialAudioStreamAttributes,
    skeletonDeviceSpecific_NoInitialVideoStreamAttributes,
    skeletonDeviceSpecific_VideoPlayerCreationFailed,
    skeletonDeviceSpecific_VideoPlayerInitializationFailed,
    skeletonDeviceSpecific_AudioPlayerCreationFailed,
    skeletonDeviceSpecific_AudioPlayerInitializationFailed,
    skeletonDeviceSpecific_Wrong3DMode,
    skeletonDeviceSpecific_Wrong3DViewNumber,
    skeletonDeviceSpecific_createPlaybackGroupFailed
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif
