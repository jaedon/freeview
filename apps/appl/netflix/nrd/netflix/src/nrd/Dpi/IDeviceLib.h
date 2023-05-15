/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** @file IDeviceLib.h Interface that must be implemented by NRDP partners
 * which allows the NRDP SDK to retrieve other interfaces implemented by the
 * partner. Examples include the ISystem, IElementaryStreamManager, and
 * IBufferManager.
 *
 * The netflix application creates the IDeviceLib interface via an extern "C"
 * call createDeviceLib() declared below. NRDP partners must provide an
 * implementation for createDeviceLib.
 */

#ifndef NRDP_DEVICELIB_H
#define NRDP_DEVICELIB_H

#include <nrd/config.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Configuration.h>

namespace netflix {

class IWebCrypto;

namespace mediacontrol {
class IAdaptiveStreamingPlayer;
}

namespace device {

class IAudioMixer;
class IBufferManager;
class IDrm;
class IDrmSystem;
class IPlaybackDevice;
class ISystem;
class IDeviceLib;
class ITextToSpeech;


namespace esplayer {
class IElementaryStreamManager;
}


/**
 * The extern "C" function createDeviceLib() must be implemented by the device
 * partner to allow the netflix application to instantiate the device-specific
 * IDeviceLib implementation. createDeviceLib() is not required when the
 * partner's DPI implementation is run within the MediaControlSim application.
 *
 * @return an IDeviceLib pointer to an instance of the device's implementation
 * of IDeviceLib.
 */
extern "C" IDeviceLib *createDeviceLib();

/**
 * The extern "C" function createBufferManager() need only be
 * implemented to allow the partner's DPI implementation to run within
 * test applications.
 *
 * @return an IBufferManager interface to the partner's implementation of
 * IBufferManager.
 */
extern "C" IBufferManager *createBufferManager(int audioPoolSize, int videoPoolSize);

/**
 * The extern "C" function createSystem() need only be implemented to
 * allow a partner's ISystem DPI implementation to run within test
 * applications.
 *
 * @return an ISystem interface to the partner's implementation of ISystem.
 */
extern "C" ISystem *createSystem();

#ifdef NRDP_HAS_ESPLAYER
/**
 * The extern "C" function createElementaryStreamManager() need only be
 * implemented to allow the partner's IElementaryStreamPlayer DPI implementation
 * to run within the test applications.
 *
 * @return an esplayer::IElementaryStreamManager interface to the partner's
 * implementation of IElementaryStreamManager (defined in
 * IElementaryStreamPlayer.h)
 */
extern "C" esplayer::IElementaryStreamManager *createElementaryStreamManager();

#else

/**
 *
 * !!!! "IPlaybackDevice" IS DEPRECATED !!!!  Instead, all partners should use
 * IElementaryStreamManager. Netflix will not support anything related to
 * IPlaybackDevice.
 *
 * The extern "C" function createPlaybackDevice() need only be implemented to
 * allow a partner's IPlaybackDevice (non-IElementaryStreamPlayer) DPI
 * implementation to run within test applications.
 *
 * @return an IPlabackDevice interface to the partner's implementation of
 * IPlaybackDevice.
 */
extern "C" netflix::device::IPlaybackDevice *createPlaybackDevice();

/**
 * The extern "C" function createDrm() need only be implemented to
 * allow a partner's IPlaybackDevice (non-IElementaryStreamPlayer) DPI
 * implementation to run within the test applications.
 *
 * @return an IDrm interface to the partner's implementation of IDrm.
 */
extern "C" netflix::device::IDrm *createDrm();

#endif

/**
 * @class IDeviceLib IDeviceLib.h
 * @brief Device library interface.
 */
class IDeviceLib
{
public:

    /**
     * Returns an ISystem interface to the device-specific system
     * implementation.
     *
     * @return the ISystem interface.
     */
    virtual shared_ptr<ISystem> getSystem() = 0;

    // XXX WebCrypto
    // XXX for now, always include WebCrypto
    virtual shared_ptr<IWebCrypto> getWebCrypto() = 0;

#ifdef NRDP_HAS_ESPLAYER
    /**
     * Returns an IElementaryStreamManager interface to the device-specific
     * elementary stream manager implementation. This method is only part of the
     * IDeviceLib interface when the device implements IElementaryStreamPlayer.
     * Devices implementing IPlaybackDevice do not implement this.
     *
     * @return the IElementaryStreamManager interface.
     */
    virtual shared_ptr<esplayer::IElementaryStreamManager> getESManager() = 0;
#else
    /**
     * Returns an IPlaybackDevice interface to the device-specific playback
     * device implementation. This method is only part of the IDeviceLib
     * interface when the device implements IPlaybackDevice.  Devices
     * implementing IElementaryStreamPlayer do not implement this.
     *
     * @return the IPlaybackDevice interface.
     */
    virtual shared_ptr<IPlaybackDevice> getPlaybackDevice() = 0;
#endif

    /**
     * Returns an IBufferManager interface to the device-specific buffer manager
     * implementation.
     *
     * @return the IBufferManager interface.
     */
    virtual shared_ptr<IBufferManager> getBufferManager() = 0;

    /**
     * Returns an IDrmSystem interface to the device-specific DRM implementation.
     *
     */
    virtual shared_ptr<IDrmSystem> getDrmSystem() = 0;

    /**
     * Returns an IAudioMixer interface to the device-specific audio mixer
     * implementation. This is not required and the app will function without
     * one.
     *
     * @return the IAudioMixer interface
     */
    virtual shared_ptr<IAudioMixer> getAudioMixer()
    {
        return shared_ptr<IAudioMixer>();
    }

    virtual shared_ptr<ITextToSpeech> getTextToSpeech()
    {
        return shared_ptr<ITextToSpeech>();
    }

    virtual ~IDeviceLib() {};
};

}} // namespace netflix::device

#endif // NRDP_DEVICELIB_H
