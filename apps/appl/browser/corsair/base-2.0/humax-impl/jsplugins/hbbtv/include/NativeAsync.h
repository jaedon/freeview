/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OOIF_ASYNC
#ifndef NATIVE_ASYNC
#define NATIVE_ASYNC

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \defgroup Async Asynchronous
  @{

   This is the native interface for the handling Asynchronous behavior in OOIF.
   Using it, for some native function calls, the integration layer has the
   option to suspend the javascript engine in Opera, instead of returning a
   value immediately. The javascript engine can be resumed later by the
   integration layer. This allows it to perform asynchronously operations that
   would otherwise take a long time and block the entire browser's operation
   until completed.

   <b>Here is the full list of native functions that currently support
   asynchronous operations:</b><br>
   native_ApplicationGetPermissions(), native_AppPrivateDataGetCurrentChannel(), native_AVOutputGetAudioRange(), native_AVOutputGetDigitalAudioMode(), native_AVOutputGetEnabled(), native_AVOutputGetHDVideoFormat(), native_AVOutputGetName(), native_AVOutputGetSubtitleEnabled(), native_AVOutputGetSupportedAspectRatios(), native_AVOutputGetSupportedAudioRanges(), native_AVOutputGetSupportedDigitalAudioModes(), native_AVOutputGetSupportedHdVideoFormats(), native_AVOutputGetSupportedVideoModes(), native_AVOutputGetTVAspectRatio(), native_AVOutputGetType(), native_AVOutputGetVideoMode(), native_BookmarkGetName(), native_BookmarkGetTime(), native_BroadcastGetCurrentChannel(), native_BroadcastGetFullScreen(), native_BroadcastGetMaxOffset(), native_BroadcastGetPlaybackOffset(), native_BroadcastGetPlayPosition(), native_BroadcastGetPlaySpeed(), native_BroadcastGetPlaySpeeds(), native_BroadcastGetPlayState(), native_BroadcastGetProgrammes(), native_BroadcastGetRecordingState(), native_CapabilityGetExtraHDVideoDecodes(), native_CapabilityGetExtraSDVideoDecodes(), native_CapabilityGetXmlCapabilities(), native_ChannelConfigGetCurrentFavouriteList(), native_ChannelGetFavIDs(), native_ChannelGetGenres(), native_ConfigGetCountryId(), native_ConfigGetIsPINEntryLocked(), native_ConfigGetPreferredAudioLanguage(), native_ConfigGetPreferredSubtitleLanguage(), native_ConfigGetPvrEndPadding(), native_ConfigGetPvrPolicy(), native_ConfigGetPvrSaveDays(), native_ConfigGetPvrSaveEpisodes(), native_ConfigGetPvrStartPadding(), native_ConfigGetRegionId(), native_DiscInfoGetFree(), native_DiscInfoGetReserved(), native_DiscInfoGetTotal(), native_DiscInfoOnCreate(), native_DownloadGetAmountDownloaded(), native_DownloadGetContentID(), native_DownloadGetContentURL(), native_DownloadGetDescription(), native_DownloadGetDrmControl(), native_DownloadGetIconURL(), native_DownloadGetId(), native_DownloadGetName(), native_DownloadGetOriginSite(), native_DownloadGetOriginSiteName(), native_DownloadGetParentalRatings(), native_DownloadGetStartTime(), native_DownloadGetState(), native_DownloadGetTimeElapsed(), native_DownloadGetTimeRemaining(), native_DownloadGetTotalSize(), native_DownloadGetTransferType(), native_FavouriteListGetFavID(), native_FavouriteListGetName(), native_KeySetGetMaximumValue(), native_KeySetGetValue(), native_LocalSystemGetDeviceID(), native_LocalSystemGetHardwareVersion(), native_LocalSystemGetMute(), native_LocalSystemGetNetworkInterfaces(), native_LocalSystemGetOutputs(), native_LocalSystemGetPvrEnabled(), native_LocalSystemGetPvrSupport(), native_LocalSystemGetSerialNumber(), native_LocalSystemGetSoftwareVersion(), native_LocalSystemGetStandbyState(), native_LocalSystemGetSystemReady(), native_LocalSystemGetTvStandard(), native_LocalSystemGetVolume(), native_NetworkInterfaceGetConnected(), native_NetworkInterfaceGetEnabled(), native_NetworkInterfaceGetIPAddress(), native_NetworkInterfaceGetMacAddress(), native_ParentalRatingGetLabels(), native_ParentalRatingGetName(), native_ParentalRatingGetRegion(), native_ParentalRatingGetScheme(), native_ParentalRatingGetValue(), native_ParentalRatingSchemeGetName(), native_ParentalRatingSchemeGetThreshold(), native_ProgrammeGetAudioLanguages(), native_ProgrammeGetAudioType(), native_ProgrammeGetBlocked(), native_ProgrammeGetChannel(), native_ProgrammeGetChannelID(), native_ProgrammeGetDescription(), native_ProgrammeGetDuration(), native_ProgrammeGetEpisode(), native_ProgrammeGetGenre(), native_ProgrammeGetHasRecording(), native_ProgrammeGetID(), native_ProgrammeGetIDType(), native_ProgrammeGetIsHD(), native_ProgrammeGetIsMultilingual(), native_ProgrammeGetLocked(), native_ProgrammeGetLongDescription(), native_ProgrammeGetLongName(), native_ProgrammeGetName(), native_ProgrammeGetParentalRating(), native_ProgrammeGetRecordings(), native_ProgrammeGetShowType(), native_ProgrammeGetStartTime(), native_ProgrammeGetSubtitleLanguages(), native_ProgrammeGetSubtitles(), native_ProgrammeGetTotalEpisodes(), native_PVRGetAudioLanguages(), native_PVRGetAudioType(), native_PVRGetBlocked(), native_PVRGetBookmarks(), native_PVRGetDoNotDelete(), native_PVRGetGenres(), native_PVRGetID(), native_PVRGetIsHD(), native_PVRGetIsManual(), native_PVRGetIsMultilingual(), native_PVRGetIsWidescreen(), native_PVRGetLocked(), native_PVRGetParentalRatings(), native_PVRGetRecordingDuration(), native_PVRGetRecordingStartTime(), native_PVRGetSaveDays(), native_PVRGetSaveEpisodes(), native_PVRGetShowType(), native_PVRGetState(), native_PVRGetSubtitleLanguages(), native_PVRGetSubtitles(), native_PVRGetChannel(), native_PVRGetDescription(), native_PVRGetDuration(), native_PVRGetEndPadding(), native_PVRGetEpisode(), native_PVRGetIsSeries(), native_PVRGetLongDescription(), native_PVRGetLongName(), native_PVRGetName(), native_ScheduledRecordingGetParentalRating(), native_PVRGetProgrammeID(), native_PVRGetProgrammeIDType(), native_PVRGetRepeatDays(), native_PVRGetStartPadding(), native_PVRGetStartTime(), native_PVRGetTotalEpisodes(), native_SearchGetQuery(), native_SearchGetResult(), native_SearchGetSearchTarget(), native_SearchManagerGuideDaysAvailable(), native_SearchResultsGetLength(), native_SearchResultsGetOffset(), native_SearchResultsGetTotalSize(), native_VodGetCurrentTrackIndex(), native_VodGetData(), native_VodGetError(), native_VodGetFullScreen(), native_VodGetNrTracks(), native_VodGetPersist(), native_VodGetPlayPosition(), native_VodGetPlayState(), native_VodGetPlayTime(), native_VodGetSpeed()

 */

/**
  An opaque object used to refer to a suspended or suspendable javascript execution context.
*/
typedef void* ResumeToken;

/**
  The callback function that the native platform should call to get a
  ResumeToken for the current call.  This function can be called during any
  native function call from OOIF. If the currently called function does not
  support being suspended, the asyncResumeTokenGetter() will return NULL.
  Otherwise, it will return a ResumeToken. When a token has been successfully
  obtained, the integration layer may suspend the execution of the javascript
  engine by returning OOIF_RETURN_ASYNC.  To resume execution later, the see
  the asyncResume() function.
  \return A ResumeToken if the current function can be suspended, or NULL it not
*/
typedef ResumeToken (asyncResumeTokenGetter)();

/**
  The callback function that the native platform should call to resume
  execution of a previously suspended page. The resumed javascript engine
  will repeat the call to the function that caused it to be suspended.
  Note that the asyncResumer function must not be called with tokens that have
  been invalidated by the native_AsyncRevokeToken() function.
 \param token The ResumeToken indicating which application should be resumed.
*/
typedef void (asyncResumer)(ResumeToken token);

/**
  Registers the asyncResumer() function. This is called during the
  initialization of OOIF.
  \param resumer A pointer to the function
  \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AsyncSetResumer(asyncResumer resumer);

/**
  Registers the asyncResumeTokenGetter() function. This is called during the
  initialization of OOIF.
  \param getter A pointer to the function
  \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AsyncSetTokenGetter(asyncResumeTokenGetter getter);

/**
  This function is called if the ResumeToken of an application previously
  suspended by the integration layer becomes invalid, indicating that
  execution of this particular application can no longer be resumed. This
  typically happens when the page containing the application is navigated away
  from, reloaded, or closed.
  This will not be called for ResumeToken that have already been used to
  restart execution, as they are already invalid.
  \param token The token that is no longer valid.
  \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AsyncRevokeToken(ResumeToken token);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_ASYNC */
#endif /* OOIF_ASYNC */
