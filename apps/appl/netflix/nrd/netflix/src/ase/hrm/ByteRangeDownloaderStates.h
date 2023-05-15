/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BYTE_RANGE_DOWNLOADERL_STATES_H
#define BYTE_RANGE_DOWNLOADERL_STATES_H

#include "ByteRangeDownloader.h"

namespace netflix {
namespace ase {

/**
 *  @class ByteRangeDownloaderBaseState
 */
class ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderBaseState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const = 0;

    /** Return the state name. */
    virtual const char* name() const = 0;

    /** Enter the state. */
    virtual void enter(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Exit the state. */
    virtual void exit(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Process HTTP connection event. */
    virtual int32_t processHttpConnection(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Process HTTP response event. */
    virtual int32_t processHttpResponse(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process timeout. */
    virtual int32_t processTimeout(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Pause downloading. */
    virtual int32_t pauseDownloading(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Resume downloading. */
    virtual int32_t resumeDownloading(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Notify the availability of the media buffer. */
    virtual int32_t notifyMediaBufferAvailability(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            bool mediaBufferAvailable );
};

/**
 *  @class ByteRangeDownloaderDisconnectedState
 */
class ByteRangeDownloaderDisconnectedState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderDisconnectedState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Enter the state. */
    virtual void enter(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Exit the state. */
    virtual void exit(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );
};

/**
 *  @class ByteRangeDownloaderWaitingToReconnectState
 */
class ByteRangeDownloaderWaitingToReconnectState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderWaitingToReconnectState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Enter the state. */
    virtual void enter(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Process timeout. */
    virtual int32_t processTimeout(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            ByteRangeReceivingTaskList& byteRangeReceivingTaskList );
};

/**
 *  @class ByteRangeDownloaderConnectingState
 */
class ByteRangeDownloaderConnectingState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderConnectingState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Process selected connection. */
    virtual int32_t processHttpConnection(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );
};

/**
 *  @class ByteRangeDownloaderIdleState
 */
class ByteRangeDownloaderIdleState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderIdleState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Process selected connection. */
    virtual int32_t processHttpConnection(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );
};

/**
 *  @class ByteRangeDownloaderDownloadingState
 */
class ByteRangeDownloaderDownloadingState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderDownloadingState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Enter the state. */
    virtual void enter(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Exit the state. */
    virtual void exit(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Process HTTP connection event. */
    virtual int32_t processHttpConnection(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Process HTTP response event. */
    virtual int32_t processHttpResponse(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Process timeout. */
    virtual int32_t processTimeout(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            ByteRangeReceivingTaskList& byteRangeReceivingTaskList );

    /** Pause downloading. */
    virtual int32_t pauseDownloading(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Notify the availability of the media buffer. */
    virtual int32_t notifyMediaBufferAvailability(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            bool mediaBufferAvailable );
};

/**
 *  @class ByteRangeDownloaderSuspendedState
 */
class ByteRangeDownloaderSuspendedState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderSuspendedState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Enter the state. */
    virtual void enter(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Exit the state. */
    virtual void exit(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Process HTTP connection event. */
    virtual int32_t processHttpConnection(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Resume downloading. */
    virtual int32_t resumeDownloading(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow );

    /** Notify the availability of the media buffer. */
    virtual int32_t notifyMediaBufferAvailability(
                            ByteRangeDownloader& byteRangeDownloader,
                            const AseTimeVal& timeNow,
                            bool mediaBufferAvailable );
};

/**
 *  @class ByteRangeDownloaderConnectionDownState
 */
class ByteRangeDownloaderConnectionDownState : public ByteRangeDownloaderBaseState
{
public:
    virtual ~ByteRangeDownloaderConnectionDownState() {}

    /** Return the state. */
    virtual ByteRangeDownloader::State state() const;

    /** Return the state name. */
    virtual const char* name() const;

    /** Enter the state. */
    virtual void enter(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );

    /** Exit the state. */
    virtual void exit(
                    ByteRangeDownloader& byteRangeDownloader,
                    const AseTimeVal& timeNow );
};

}}

#endif //BYTE_RANGE_DOWNLOADERL_STATES_H
