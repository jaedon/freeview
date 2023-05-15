/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** @file ITextToSpeech.h The header file for the text to speech engine.
 *

## Theory of Operation

The TTS DPI design places the entire TTS engine below the DPI and considers it as part of the platform.
In the context of all audio sources, the TTS "channel" is considered directly adjacent and orthogonal to all
other audio sources (media audio, audio mixer) and is mixed by the platform under their control.

Given this decision, we can categorize all DPI calls into these categories:

- **GET Platform configuration** -  Netflix accesses configuration that is set outside our application.

- **SAY an utterance** -  An utterance is a distinct phrase and is a string that results in a playable audio clip.
If an utterance is currently being output, then this request is queued and played out in order.  It is the platform's
responsibility to queue the requests and possibly split each utterance based on platform-specific memory and
performance restrictions.

- **SILENCE** -  Insert some amount of digital silence between utterances.  Silence is queued along with utterances in the calling order.

- **STOP** - Interrupt the current utterance and flush any outstanding ones (including silence requests).

- **CALLBACKS for state transitions** - We expect the TTS engine to inform us of state changes such as the audible start of
SAY request and the end of a SAY request.  The callbacks also inform us of changes in the requested verbosity level
and enabling/disabling TTS.

A typical flow of calls from the application is as follows:

1. At startup, the application reads the platform's TTS on/off state and verbosity level and registers for callbacks.
2. As the UI is navigated, the application invokes a series of SAY requests that are queued.  Sequencing the spoken
utterances with appropriate gaps of digital silence can be achieved either through explicitly requesting SILENCE, or
through waiting for the completion of one or more utterances.
3. Key presses that occur during an utterance may require TVUI to STOP the current utterance. For instance, navigating to a
UI page may involve generating a long series of utterances (title, synopsis, rating, listing available options etc.), so you should
stop and flush them if the user exits the page prior to the TTS engine saying the utterances.
*/

#ifndef NRDP_TEXTTOSPEECH_H
#define NRDP_TEXTTOSPEECH_H

#include <nrdbase/tr1.h>
#include <nrdbase/Variant.h>
#include <string>
#include <vector>

namespace netflix {
namespace device {

/** @class netflix::device::ITextToSpeech
    @brief The interface for the text to speech engine.
    */
class ITextToSpeech
{
public:
    virtual ~ITextToSpeech() { }

    /** Text-to-speech system capabilities */
    struct Capabilities
    {
        Capabilities()
            : enabled(true), supportsRate(false), supportsPitch(false), supportsVerbosity(false)
        {
        }
        /** The enabled property is set to true indicating that the platform is currently
         * enabling text to speech output.
         */
        bool enabled;

        /** The supportsRate property is set to true if the platform is able to vary the reading
         *  rate per utterance.
         */
        bool supportsRate;

        /** The supportsPitch property is set to true if the platform supports varying the
         *  pitch per utterance.
         */
        bool supportsPitch;

        /** The supportsVerbosity property is set if the platform supports setting the verbosity
         *  level through a UI element outside of the Netflix application.
         */
        bool supportsVerbosity;
    };

    /** Callback object which is invoked by the platform object upon event generation */
    struct Listener
    {
        virtual ~Listener() { }

        /** The saying callback is invoked at the start of an utterance being played by
         * the platform.  The argument id is the same id which was provided to the
         * original say() function.
         */
        virtual void saying(unsigned int id) = 0;

        /** The said platforom is invoked at the end of an utterance that is being played
         * by the platform.  The argument id is the same which was provided to the
         * original say() function
         */
        virtual void said(unsigned int id) = 0;

        /**
         * The stopped callback is invoked upon the completion of stopAndFlush.  The
         * argument id is the same as what was provided to original stopAndFlush()
         */
        virtual void stopped(unsigned int id) = 0;

        /**
         * The capabilitiesChanged callback is invoked if the underlying capabilities
         * of the platforom have been changed during runtime
         */
        virtual void capabilitiesChanged() = 0;

        /**
         * The verbosityChanged callback is invoked if the platform has had its
         * verbosity level changed by the user at runtime.
         */
        virtual void verbosityChanged() = 0;

        /**
         * The propertiesChanged callback is invoked if a property is set
         * asynchronously by a caller
         */
        virtual void propertiesChanged() = 0;
    };

    /** Sets the callback object to be invoked on events generated by the platform */
    virtual void setListener(const shared_ptr<Listener>& listener);

    /** Returns the current set of text-to-speech capabilities */
    virtual Capabilities getCapabilities() const = 0;

    /**
     * The say method is called with a UTF-8 string of arbitrary length.
     * The call to say generates an utterance which is played by the platform.
     * If the platform is currently playing an utterance due to a previous
     * call to say the the current say call is queued for playback.  The id is arbitrary
     * and is used by the callbacks that indicate the start and stop of the utterance.
     *
     * The platform issues callbacks that mark both the start and end of each utterance being played
     * with the provided ID as argument to the callback.
     *
     * param id Arbitrary ID that should be unique per utterance which is sent back with callbacks
     */
    virtual void say(unsigned int id, const std::string& text) = 0;

    /**
     * The silence method is similar to the say method except that it queues a specified length
     * of digital silence to be played.
     *
     * param id Arbitrary ID that should be unique per utterance which is sent back with callbacks
     * param ms Length of digital silence to play in milliseconds
     */
    virtual void silence(unsigned int id, unsigned int ms) = 0;

    /**
     * The stopAndFlush method is used to immediately stop audio output and flushes any outstanding
     * queued utterances and silence calls.  Callbacks are not issued for these outstanding requests.
     * A callback with the matching ID is issued when the stopAndFlush operation completes.
     */
    virtual void stopAndFlush(unsigned int id) = 0;

    /**
     * @brief Return a vector of supported languages
     * @return String vector of supported languages
     */
    virtual std::vector<std::string> languages() const = 0;

    /**
     * @brief Sets the current text-to-speech language
     *
     * @param language Set the current language for text-to-speech in BCP47 format.
     */
    virtual void setLanguage(const std::string& language) = 0;

    /**
     * @brief Return the currently selected language
     * @return Currently selected language as a string in BCP47 format.
     */
    virtual std::string language() const = 0;

    /**
     * @brief Return a list of currently available voices
     * @return String vector of available voices
     */
    virtual std::vector<std::string> voices() const = 0;

    /**
     * @brief Set the current voice
     *
     * @param voice Sets the current voice
     */
    virtual void setVoice(const std::string& voice) = 0;

    /**
     * @brief Returns currently selected voice
     * @return Currently selected voice
     */
    virtual std::string voice() const = 0;

    virtual void setProperty(const std::string& key, const netflix::Variant& value);

    /**
     * @brief Returns reading rate in words per minute
     * @return Reading rate in words per minute
     */
    virtual unsigned int rate() const = 0;

    /**
     * @brief Returns current baseline pitch in Hz
     * @return Returns current baseline pitch in Hz
     */
    virtual unsigned int pitch() const = 0;

    /**
     * @brief List of possible verbosity levels
     * @details The underlying platform is assumed to have these verbosity levels which can be
     * be selected through a system menu.
     */
    enum Verbosity {
        /**
         * @brief Lower verbosity setting
         */
        VerbosityLow,
        /**
         * @brief Higher verbosity setting
         */
        VerbosityHigh
    };

    /**
     * @brief Return current verbosity level
     * @return Current verbosity level
     */
    virtual Verbosity verbosity() const = 0;

protected:
    shared_ptr<Listener> mListener;
};

/**
 * @brief Sets the callback object
 * @details This function sets the callback object which contains the various methods which are to be
 * invoked for various platform state transitions.
 *
 * @param listener Callback object
 */
inline void ITextToSpeech::setListener(const shared_ptr<Listener>& listener)
{
    mListener = listener;
}

inline void ITextToSpeech::setProperty(const std::string&, const netflix::Variant&)
{
}

}} // namespace netflix::device

#endif
