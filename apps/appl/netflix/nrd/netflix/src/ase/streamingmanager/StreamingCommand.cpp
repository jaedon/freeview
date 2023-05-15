/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StreamingCommand.h"

using namespace netflix::ase;
using namespace std;

StreamingCommand::StreamingCommand(const shared_ptr<MediaPresentation> &pMediaPresentation)
    : mMediaPresentationPtr(pMediaPresentation)
{
}

SwitchTracksCommand::SwitchTracksCommand(const shared_ptr<MediaPresentation> &pMediaPresentation,
                                         const std::vector<uint32_t>& trackIndexes )
    : StreamingCommand(pMediaPresentation),
      mTrackIndexes(trackIndexes)
{
}

void
SwitchTracksCommand::execute()
{
    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runSwitchTracksCommand(mTrackIndexes);
}

SetSeekTimeCommand::SetSeekTimeCommand(const shared_ptr<MediaPresentation> &pMediaPresentation,
                                       AseTimeStamp const& seekTime)
    : StreamingCommand(pMediaPresentation),
      mSeekTime(seekTime)
{
}

void SetSeekTimeCommand::execute()
{
    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runSetSeekTimeCommand(mSeekTime);
}

RebufferCommand::RebufferCommand(
        shared_ptr<MediaPresentation> pMediaPresentation)
            : StreamingCommand(pMediaPresentation)
{
}

void RebufferCommand::execute()
{
    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runRebufferCommand();
}

SkipCommand::SkipCommand(
        shared_ptr<MediaPresentation> pMediaPresentation)
            : StreamingCommand(pMediaPresentation)
{
}

void SkipCommand::execute()
{
    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runSkipCommand();
}
AddManifestCommand::AddManifestCommand(
        shared_ptr<MediaPresentation> pMediaPresentation,
        shared_ptr<Manifest> pManifest)
            : StreamingCommand(pMediaPresentation),
              mManifestPtr(pManifest)
{
}

void AddManifestCommand::execute()
{

    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runAddManifestCommand(mManifestPtr);
}


NotifyLicenseAcquiredCommand::NotifyLicenseAcquiredCommand(
        shared_ptr<MediaPresentation> pMediaPresentation)
            : StreamingCommand(pMediaPresentation)
{
}

void NotifyLicenseAcquiredCommand::execute()
{

    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runNotifyLicenseAcquiredCommand();
}

SetVideoBitrateRangeCommand::SetVideoBitrateRangeCommand(
        shared_ptr<MediaPresentation> pMediaPresentation,
        std::vector<shared_ptr<BitrateRange> > const& videoBitrateRanges)
            : StreamingCommand(pMediaPresentation),
              mVideoBitrateRanges(videoBitrateRanges)
{
}

void SetVideoBitrateRangeCommand::execute()
{
    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runSetVideoBitrateRangeCommand(mVideoBitrateRanges);
}

SetStreamingBuffer::SetStreamingBuffer(
        shared_ptr<MediaPresentation> pMediaPresentation,
        uint32_t maxBufferLen,
        uint32_t minBufferLen)
            : StreamingCommand(pMediaPresentation),
              mMaxBufferLen(maxBufferLen),
              mMinBufferLen(minBufferLen)
{
}

void SetStreamingBuffer::execute()
{
    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runSetStreamingBufferCommand(mMaxBufferLen, mMinBufferLen);
}

UpdateDownloaderTrackId::UpdateDownloaderTrackId(
        shared_ptr<MediaPresentation> pMediaPresentation)
            : StreamingCommand(pMediaPresentation)
{
}

void UpdateDownloaderTrackId::execute()
{

    shared_ptr<MediaPresentation> pMediaPresentation = mMediaPresentationPtr.lock();
    if ( pMediaPresentation)
        pMediaPresentation->runUpdateDownloaderTrackIdCommand();
}
