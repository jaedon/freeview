/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_LISTENER_EVENT_H
#define MEDIACONTROL_LISTENER_EVENT_H

#include <cassert>

#include <nrdbase/Log.h>
#include <nrdbase/Time.h>

namespace netflix {
namespace mediacontrol {

/**
 * @class ListenerEvent ListenerEvent.h
 * @brief Parent of all events that can be fired to the listeners.
 */
template <class T>
class ListenerEvent
{
public:
    ListenerEvent(ullong sessionId) :
        sessionId_(sessionId)
    {}
    virtual ~ListenerEvent() {}

    virtual void fireTo(shared_ptr<T> /*listener*/) const {}

    ullong sessionId() const { return sessionId_; }

public:
    ullong sessionId_;
};

/**
 * @class StateChangedEvent ListenerEvent.h
 */
template <class T>
class StateChangedEvent : public ListenerEvent<T>
{
private:
    IAdaptiveStreamingPlayer::State state_;

public:
    StateChangedEvent(ullong sessionId, IAdaptiveStreamingPlayer::State state) :
        ListenerEvent<T>(sessionId),
        state_(state)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "stateChanged: new state = %s",
               (state_ == IAdaptiveStreamingPlayer::OPENING)? "OPENING" :
               (state_ == IAdaptiveStreamingPlayer::PLAYING)? "PLAYING" :
               (state_ == IAdaptiveStreamingPlayer::PAUSED) ? "PAUSED"  :
               (state_ == IAdaptiveStreamingPlayer::STOPPED)? "STOPPED" :
               (state_ == IAdaptiveStreamingPlayer::CLOSED) ? "CLOSED"  : "<unknown>");

        listener->stateChanged(ListenerEvent<T>::sessionId(), state_);
    }
};

/**
 * @class OpenCompleteEvent ListenerEvent.h
 */
template <class T>
class OpenCompleteEvent : public ListenerEvent<T>
{
public:
    OpenCompleteEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "openComplete");
        listener->openComplete(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class PlayerStreamSelectedEvent Listener.h
 */
template <class T>
class PlayerStreamSelectedEvent : public ListenerEvent<T>
{
private:
    int manifest_;
    int track_;
    int stream_;
    uint32_t startPts_;
    uint32_t rebuffer_;

public:
    PlayerStreamSelectedEvent(ullong sessionId, int manifest, int track, int stream,
                              uint32_t startPts, uint32_t rebuffer) :
        ListenerEvent<T>(sessionId),
        manifest_(manifest),
        track_(track),
        stream_(stream),
        startPts_(startPts),
        rebuffer_(rebuffer)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        listener->streamSelected(ListenerEvent<T>::sessionId(), manifest_, track_,
                                 stream_, Ticks(startPts_, Ticks::RES_1KHZ), rebuffer_);
    }
};

/**
 * @class StreamPresentingEvent Listener.h
 */
template <class T>
class PlayerStreamPresentingEvent: public ListenerEvent<T>
{
private:
    int manifest_;
    int track_;
    int stream_;
    uint32_t startPts_;

public:
    PlayerStreamPresentingEvent(ullong sessionId, int manifest, int track, int stream,
                                uint32_t startPts) :
        ListenerEvent<T>(sessionId),
        manifest_(manifest),
        track_(track),
        stream_(stream),
        startPts_(startPts)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        listener->streamPresenting(ListenerEvent<T>::sessionId(), manifest_, track_,
                                   stream_, Ticks(startPts_, Ticks::RES_1KHZ));
    }
};


/**
 * @class EndOfStreamEvent ListenerEvent.h
 */
template <class T>
class EndOfStreamEvent : public ListenerEvent<T>
{
public:
    EndOfStreamEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "endOfStream");
        listener->endOfStream(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class UnderflowEvent ListenerEvent.h
 */
template <class T>
class UnderflowEvent : public ListenerEvent<T>
{
public:
    UnderflowEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "underflow");
        listener->underflow(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class SkippedEvent ListenerEvent.h
 */
template <class T>
class SkippedEvent : public ListenerEvent<T>
{
public:
    SkippedEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "skipped");
        listener->skipped(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class BufferingEvent ListenerEvent.h
 */
template <class T>
class BufferingEvent : public ListenerEvent<T>
{
private:
    uint32_t percentage_;

public:
    BufferingEvent(ullong sessionId, uint32_t percentage) :
        ListenerEvent<T>(sessionId),
        percentage_(percentage)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "buffering: Percentage = %u", percentage_);
        listener->buffering(ListenerEvent<T>::sessionId(), percentage_);
    }
};

/**
 * @class BufferingCompleteEvent ListenerEvent.h
 */
template <class T>
class BufferingCompleteEvent : public ListenerEvent<T>
{
private:
    bool m_faststart;

public:
    BufferingCompleteEvent(ullong sessionId, bool faststart) :
        ListenerEvent<T>(sessionId),
        m_faststart(faststart)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "bufferingComplete");
        listener->bufferingComplete(ListenerEvent<T>::sessionId(), m_faststart);
    }
};

/**
 * @class VideoBitrateSelectionCompleteEvent ListenerEvent.h
 */
template <class T>
class VideoBitrateSelectionCompleteEvent : public ListenerEvent<T>
{
public:
    VideoBitrateSelectionCompleteEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "video bitrate selection completed.");
        listener->videoBitrateSelectionCompleted(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class UpdatePtsEvent ListenerEvent.h
 */
template <class T>
class UpdatePtsEvent : public ListenerEvent<T>
{
private:
    uint32_t pts_;
    ullong monotime_;
    bool firstEvent_;

public:
    UpdatePtsEvent(ullong sessionId, uint32_t pts, ullong monotimeInMS, bool firstEvent = false) :
        ListenerEvent<T>(sessionId),
        pts_(pts),
        monotime_(monotimeInMS),
        firstEvent_(firstEvent)
    {
    }

    virtual void fireTo(shared_ptr<T> listener) const
    {
        if (firstEvent_)
        {
            NTRACE(TRACE_MEDIACONTROL,
                   "First PTSUpdate sending from EventQueue: time=%llu",
                   Time::mono().ms());
        }

        NTRACE(TRACE_MEDIALISTENER, "updatePts: PTS = %u generated at %lld", pts_, monotime_);
        listener->updatePts(ListenerEvent<T>::sessionId(), Ticks(pts_, Ticks::RES_1KHZ), monotime_);
    }
};

/**
 * @class UpdateContentDuration ListenerEvent.h
 */
template <class T>
class UpdateContentDurationEvent : public ListenerEvent<T>
{
public:
    UpdateContentDurationEvent(ullong sessionId, uint32_t duration) :
        ListenerEvent<T>(sessionId),
        duration_(duration)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "updateContentDuration: %u", duration_);
        listener->updateContentDuration(ListenerEvent<T>::sessionId(), duration_);
    }

private:
    uint32_t duration_;
};

/**
 * @class UpdateVideoBitrateEvent ListenerEvent.h
 */
template <class T>
class UpdateVideoBitrateEvent : public ListenerEvent<T>
{
private:
    uint32_t kilobitsPerSecond_;

public:
    UpdateVideoBitrateEvent(ullong sessionId, uint32_t kilobitsPerSecond) :
        ListenerEvent<T>(sessionId),
        kilobitsPerSecond_(kilobitsPerSecond)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "updateVideoBitrate: bit-rate = %u kbps",
               kilobitsPerSecond_);
        listener->updateVideoBitrate(ListenerEvent<T>::sessionId(), kilobitsPerSecond_);
    }
};

/**
 * @class ErrorEvent ListenerEvent.h
 */
template <class T>
class ErrorEvent : public ListenerEvent<T>
{
private:
    NFErr error_;

public:
    ErrorEvent(ullong sessionId, const NFErr &error) :
        ListenerEvent<T>(sessionId),
        error_(error)
    {
        // Only MediaControl error codes (NFErr_MC_xxxx) should be sent to the user.
        assert(isMediaControlError(error_.peekCode()));
    }

    virtual void fireTo(shared_ptr<T> listener) const
    {
        Log::info(TRACE_MEDIALISTENER, "error: Error = %s", error_.toString().c_str());
        listener->error(ListenerEvent<T>::sessionId(), error_);
    }
};

/**
 * @class AudioTrackChangedEvent ListenerEvent.h
 */
 template <class T>
class AudioTrackChangedEvent : public ListenerEvent<T>
{
public:
    AudioTrackChangedEvent(ullong sessionId, uint32_t trackIndex,
                           const std::string &trackId) :
        ListenerEvent<T>(sessionId),
        m_trackIndex(trackIndex),
        m_trackId(trackId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "audioTrackChanged: %u %s",
               m_trackIndex, m_trackId.c_str());
        listener->audioTrackChanged(ListenerEvent<T>::sessionId(),
                                    m_trackIndex, m_trackId);
    }

private:
    uint32_t m_trackIndex;
    std::string m_trackId;
};


/**
 * @class AudioTrackChangeStartedEvent ListenerEvent.h
 */
template <class T>
class AudioTrackChangeStartedEvent : public ListenerEvent<T>
{
public:
    AudioTrackChangeStartedEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "audioTrackChangeStarted");
        listener->audioTrackChangeStarted(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class AudioTrackChangeCompleteEvent ListenerEvent.h
 */
template <class T>
class AudioTrackChangeCompleteEvent : public ListenerEvent<T>
{
public:
    AudioTrackChangeCompleteEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "audioTrackChangeComplete");
        listener->audioTrackChangeComplete(ListenerEvent<T>::sessionId());
    }
};


/**
 * @class AudioTrackChangeRejectedEvent ListenerEvent.h
 */
template <class T>
class AudioTrackChangeRejectedEvent : public ListenerEvent<T>
{
public:
    AudioTrackChangeRejectedEvent(ullong sessionId) :
        ListenerEvent<T>(sessionId)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "audioTrackChangeRejected");
        listener->audioTrackChangeRejected(ListenerEvent<T>::sessionId());
    }
};

/**
 * @class VideoWindowChangedEvent ListenerEvent.h
 */
template <class T>
class VideoWindowChangedEvent : public ListenerEvent<T>
{
public:
    VideoWindowChangedEvent(ullong sessionId, const Rect& actualRect) :
        ListenerEvent<T>(sessionId),
        actualRect_(actualRect)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER,
               "videoWindowChanged: (%u, %u, %u, %u)",
               actualRect_.x,
               actualRect_.y,
               actualRect_.width,
               actualRect_.height);

        listener->videoWindowChanged(ListenerEvent<T>::sessionId(), actualRect_);
    }

private:
    Rect actualRect_;
};


/**
 * @class RestartPlaybackEvent ListenerEvent.h
 */
template <class T>
class RestartPlaybackEvent : public ListenerEvent<T>
{
public:
    RestartPlaybackEvent(ullong sessionId, const NFErr &err, const Ticks &pts) :
        ListenerEvent<T>(sessionId),
        m_error(err),
        m_pts(pts)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "restartPlayback: %llu",
               m_pts.value(Ticks::RES_1KHZ));
        listener->restartPlayback(ListenerEvent<T>::sessionId(), m_error, m_pts);
    }

private:
    NFErr m_error;
    Ticks m_pts;
};

/**
 * @class ManifestSelectedEvent ListenerEvent.h
 */
template <class T>
class ManifestSelectedEvent : public ListenerEvent<T>
{
public:
    ManifestSelectedEvent(ullong sessionId, int manifestIndex) :
        ListenerEvent<T>(sessionId),
        m_manifestIndex(manifestIndex)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "manifestSelected: %d", m_manifestIndex);
        listener->manifestSelected(ListenerEvent<T>::sessionId(), m_manifestIndex);
    }

private:
    int m_manifestIndex;
};

template <class T>
class HevcInfoEvent : public ListenerEvent<T>
{
public:
    HevcInfoEvent(ullong sessionId, const std::string &info) :
        ListenerEvent<T>(sessionId),
        m_info(info)
    {}

    virtual void fireTo(shared_ptr<T> listener) const
    {
        NTRACE(TRACE_MEDIALISTENER, "hevcInfo");
        listener->hevcInfo(ListenerEvent<T>::sessionId(), m_info);
    }
private:
    const std::string m_info;
};


}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_LISTENER_EVENT_H
