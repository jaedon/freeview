/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef MEDIA_CONSUMER_LISTENER_H
#define MEDIA_CONSUMER_LISTENER_H

namespace netflix {
class AseTimeVal;

namespace mediacontrol {

/**
 * A Lisenter interface for changes in MediaConsumer
 */
class MediaRequestManagerListener
{
public:
    typedef shared_ptr<MediaRequestManagerListener> MediaRequestManagerListenerPtr;

    /**
     * The http downloading is paused because the player enters draining state
     */
    virtual void onDownloadPaused(const AseTimeVal &time) = 0;

    /**
     * The http downloading is resumed
     */
    virtual void onDownloadResumed(const AseTimeVal &time) = 0;

    virtual void onRequestScheduled(uint32_t requestId) = 0;

    virtual void onPipeliningDisabled(std::string const& reason) = 0;

    virtual void onNetworkFailing(  uint32_t                downloadTrackId,
                                    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived ) = 0;

    virtual ~MediaRequestManagerListener() {}
};

typedef MediaRequestManagerListener::MediaRequestManagerListenerPtr MediaRequestManagerListenerPtr;
}}
#endif
