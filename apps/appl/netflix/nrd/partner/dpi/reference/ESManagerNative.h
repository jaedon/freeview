/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ESMANAGER_H
#define ESMANAGER_H

/** @file ESManagerNative.h - Reference implementation of the
 * IElementaryStreamManager interface.
 *
 * ESManagerNative is the reference application's implementation of the
 * IElementaryStreamManager interface (see IElementaryStreamPlayer.h). A
 * device partner may use or modify this header and accompanying .cpp
 * as needed.
 *
 * In the reference application, DeviceLibNative (the reference application's
 * implementation of IDeviceLib) creates one instance of the ESManagerNative when
 * the DeviceLibNative is constructed. The ESManagerNative instance persists
 * until the DeviceLibNative is destroyed when the application exits. As
 * specified in IElementaryStreamManager interface, ESManagerNative provides
 * methods to create playback groups and provides DRM functionality like
 * generating challenges and storing keys.  In the reference implementation, the
 * manager also provides a RendererManager that reference implementation uses to
 * provide video and audio players access to the Linux-specific video and audio
 * rendering plugins.
 *
 * The DPI specifies that playback groups are only created by the
 * IElementaryStreamManager object and that elementary stream players are in
 * turn only created by IPlaybackGroup so that the device can use the manager to
 * track all the playback resources in use.  The reference implementation gives
 * each playback group access to the manager object for use when audio and video
 * renderers need to be allocated.
 **/

#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/DrmManager.h>

#include "DecoderManager.h"
#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>
#include <set>

namespace netflix {
namespace device {
namespace esplayer {

class RendererManager;

class ESManagerNative : public IElementaryStreamManager
{
public:
    ESManagerNative();
    virtual ~ESManagerNative();

    /**
     * IElementaryStreamManager methods
     */
    virtual NFErr createPlaybackGroup(IPlaybackGroup*& playbackGroup); // to be deprecated
    virtual NFErr createPlaybackGroup(IPlaybackGroup*& playbackGroup, uint32_t pipelineId = 0);
    virtual void destroyPlaybackGroup(IPlaybackGroup* streamPlayer);

    /*
     * Native methods
     */
    shared_ptr<RendererManager> getRendererManager();
    shared_ptr<DecoderManager> getDecoderManager();
    netflix::DrmManager* getMultiSessionDrmManager();

private:
    std::set<PlaybackGroupNative*> mPlaybackGroups;
    shared_ptr<RendererManager>    mRendererManager;
    shared_ptr<DecoderManager>     mDecoderManager;
    netflix::DrmManager*           mMultiSessionDrmManager;
    std::map<uint32_t, bool>       mIsMediaPipelineInUse;
};

} // namespace esplayer
} // namespace netflix
} // namespace device

#endif
