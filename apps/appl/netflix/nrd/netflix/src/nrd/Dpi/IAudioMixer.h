/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** @file IAudioMixer.h
 *  @brief Netflix implements this DPI for you, but you need to implement IAudioMixerOutput.h.
 *
 * IAudioMixer is a simple interface for playing back audio clips and to enable
 * UI sound effects. The mixer is also responsible for decoding the audio file and
 * specifying which formats it supports.
 */

#ifndef NRDP_AUDIOMIXER_H
#define NRDP_AUDIOMIXER_H

#include <string>
#include <vector>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/NFErr.h>
#include <nrdbase/DataBuffer.h>

namespace netflix {
namespace device {

/**
 * @class IAudioMixer IAudioMixer.h
 * @brief Interface for playing back audio clips as UI sound effects
 */
class IAudioMixer
{
public:
    class AudioListener
    {
    public:
        virtual ~AudioListener() {}

        /** Called when playback of a clip is complete, regardless of whether
         * it terminated normally or was stopped prematurely */
        virtual void playDone(const std::string &URL) = 0;

        /* Called when the capabilties change */
        virtual void capabilityChanged() = 0;
    };

    struct Capability
    {
        Capability() :
            uiSoundBufferSize(0),
            uiSoundBufferTransferSize(0),
            uiSoundBufferSizeEstimate(0)
        {
        }
        double uiSoundBufferSize; /** in milliseconds */
        double uiSoundBufferTransferSize; /** in milliseconds */
        double uiSoundBufferSizeEstimate; /** in milliseconds */
    };

    enum Ease
    {
        EaseLinear = 0,
        EaseInCubic,
        EaseOutCubic,
        EaseCount
    };

    /** Private data about the processed audio clip */
    typedef void* AudioClip;

    /** Private data about a current playback so it can be aborted */
    typedef void* PlayId;

    /** Free all playback resources */
    virtual ~IAudioMixer() {}

    /**
     * Return the audio mixer's capabilities
     */
    virtual Capability getCapability() const = 0;

    /**
     * Return a list of all of the audio codec types the mixer can play.
     */
    virtual std::vector<AudioCodecType> supportedCodecs() = 0;

    /**
     * Given an audio file, do whatever pre-processing of the audio file is
     * appropriate. The format of the file is not known, but is hinted at by the
     * extension of the URL.
     *
     * @param[in] URL The URL that was used to fetch the audio file
     * @param[in] data The raw bytes of the audio file. The memory will be freed as
     *                 soon as prepareClip returns, so if you need it, copy it.
     * @param[out] clip Private data used to store information about the audio file
     * @param[out] clipSize the size the audio clip takes in memory
     *
     * @return NFErr_OK if the audio file can be played
     */
    virtual NFErr prepareClip(const std::string &URL, const DataBuffer &data,
                              AudioClip &clip, unsigned int &clipSize) = 0;

    /**
     * When the UI no longer needs the audio, it will unload it to free up memory.
     *
     * @param[in] clip The clip to be freed.
     * @param[in] URL The URL that was used to fetch the audio file
     */
    virtual void freeClip(AudioClip clip, const std::string &URL) = 0;

    /**
     * Play the specified clip at the given volume.
     *
     * There may be multiple clips played at the same time, but only one
     * instance of any given clip. In other words, given clips A and B, A
     * and B may both be played at the same time, but A will not be played twice
     * at the same time.
     *
     * If the app wants to abort playback of the clip, it will call stopClip
     * and pass in the PlayId returned, among other things.
     *
     * When playback is complete, whether successful or aborted, the listener
     * must be called and passed the URL of the clip.
     *
     * @param[in] clip The clip to be played
     * @param[in] volume The volume to play at, between 0 (silent) and 1 (full volume)
     * @param[in] listener The listener to call back when playback ends
     * @param[in] URL The parameter to pass to the listener's playDone function
     * @param[out] playId A way of uniquely identifying the newly started playback
     * @param[in] fadeInMs duration of fade in for starting playing
     * @param[in] fadeOutMs duration of fade out for ending playing
     * @param[in] fadeInEase Fade in easing function
     * @param[in] fadeOutEase Fade out easing function
     * *
     * @return NFErr_OK if playback started successfully
     */
    virtual NFErr playClip(AudioClip clip, double volume,
                           AudioListener *listener, const std::string &URL,
                           PlayId &playId,
                           int fadeInMs = 0,
                           int fadeOutMs = 0,
                           Ease fadeInEase = EaseLinear,
                           Ease fadeOutEase = EaseLinear) = 0;

    /**
     * Prematurely stop playback of the specified clip.
     *
     * @param[in] clip The audio clip to stop playing
     * @param[in] URL The URL of the audio clip
     * @param[in] playId The playback ID returned as part of playClip()
     * @param[in] fadeOutMs duration of fade in when stopping playing
     */
    virtual NFErr stopClip(AudioClip clip,
                           const std::string &URL,
                           PlayId playId,
                           int fadeOutMs = 0,
                           Ease fadeOutEase = EaseLinear) = 0;

    /**
     * Set the "master" volume control of all the audio clip playback.
     *
     * @param[in] volume: between 0 (silent) and 1 (full volume)
     * @param[in] transitionMs: duration in millisecond to reach target volume
     *                          from current volume
     * @param[in] ease: easing function
     */
    virtual NFErr setVolume(double volume,
                         int transitionMs = 0,
                         IAudioMixer::Ease ease = IAudioMixer::EaseLinear) = 0;

    /**
     * Return the current "master" volume between 0 (silent) and 1 (full volume).
     */
    virtual double getVolume() = 0;
};

}} // namespace netflix::device


#endif // NRDP_AUDIOMIXER_H
