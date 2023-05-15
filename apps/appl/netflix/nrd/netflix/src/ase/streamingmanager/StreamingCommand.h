/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STREAMING_COMMAND_H
#define STREAMING_COMMAND_H

#include "MediaPresentation2.h"

namespace netflix {
namespace ase {

/**
 * This is not a thread safe command queue
 */
class StreamingCommand
{
public:
    StreamingCommand(const shared_ptr<MediaPresentation> &pMediaPresentation);
    // execute the command
    virtual void execute() = 0;

    virtual ~StreamingCommand() {}

protected:
    weak_ptr<MediaPresentation>     mMediaPresentationPtr;
};

class SwitchTracksCommand : public StreamingCommand
{
public:
    // TODO: add manifest index here
    SwitchTracksCommand(const shared_ptr<MediaPresentation> &pMediaPresentation,
                        const std::vector<uint32_t>& trackIndexes );

    virtual void execute();

protected:
    std::vector<uint32_t>           mTrackIndexes;

};

// XXX: Inside this command, handle the tasks in original start and STARTING state
class SetSeekTimeCommand : public StreamingCommand
{
public:
    SetSeekTimeCommand(const shared_ptr<MediaPresentation> &pMediaPresentation,
                       AseTimeStamp const& seekTime);

    virtual void execute();
protected:
    AseTimeStamp                    mSeekTime;
};

class RebufferCommand : public StreamingCommand
{
public:
    RebufferCommand(
            shared_ptr<MediaPresentation> pMediaPresentation);

    virtual void execute();
};

class SkipCommand : public StreamingCommand
{
public:
    SkipCommand(
            shared_ptr<MediaPresentation> pMediaPresentation);

    virtual void execute();
};

class AddManifestCommand : public StreamingCommand
{
public:
    AddManifestCommand(
            shared_ptr<MediaPresentation> pMediaPresentation,
            shared_ptr<Manifest> pManifest);

    virtual void execute();
protected:
    shared_ptr<Manifest>            mManifestPtr;
};



class NotifyLicenseAcquiredCommand : public StreamingCommand
{
public:
    NotifyLicenseAcquiredCommand(
            shared_ptr<MediaPresentation> pMediaPresentation);

    virtual void execute();
};

class SetVideoBitrateRangeCommand : public StreamingCommand
{
public:
    SetVideoBitrateRangeCommand(
            shared_ptr<MediaPresentation> pMediaPresentation,
            std::vector<shared_ptr<BitrateRange> > const& videoBitrateRanges);

    virtual void execute();
protected:
    std::vector<shared_ptr<BitrateRange> > mVideoBitrateRanges;
};

class SetStreamingBuffer : public StreamingCommand
{
public:
    SetStreamingBuffer(
            shared_ptr<MediaPresentation> pMediaPresentation,
            uint32_t maxBufferLen,
            uint32_t minBufferLen);

    virtual void execute();
protected:
    uint32_t    mMaxBufferLen;
    uint32_t    mMinBufferLen;
};

class UpdateDownloaderTrackId : public StreamingCommand
{
public:
    UpdateDownloaderTrackId(
            shared_ptr<MediaPresentation> pMediaPresentation);

    virtual void execute();
};

}}
#endif
