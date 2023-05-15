/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_PROGRAMME
#define __NATIVE_PROGRAMME

#include "NativeOOIFTypes.h"
#include "NativeChannel.h"
#include "NativeRecording.h"
#include "NativeParentalControlManager.h"
#include "NativeAVComponent.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
  \defgroup Programme Programme
  @{

   This is the native interface for the Programme class as defined in
   DAE 7.16.2.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
  Constants specified in DAE 7.16.2.1
*/
typedef enum {
	PROGRAMME_ID_TVA_CRID = 0,
	PROGRAMME_ID_DVB_EVENT = 1
#ifdef OIPF
	,
	PROGRAMME_ID_TVA_GROUP_CRID = 2
#endif /* OIPF */
#if defined(OIPF) && defined(HMX_WEBUI)
	, PROGRAMME_ID_FSAT_EVENT	= 10
#endif
} ProgrammeIDType;
#endif /* OIPF || HBBTV_1_2_1 */

/**
   Opaque reference to a Programme in the native platform.
 */
typedef void* ProgrammeHandle;

/**
   Creates a new Programme instance in the native platform. The
   returned handle will be used as unique identifier for the
   Programme instance in the OOIF plugin.
   \param id Reference to the new instance
   \param window_identifier The identifier of the window the create
   call originates from
   \param host The host URI of the application the create call
   originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_ProgrammeCreate(ProgrammeHandle *id, long int window_identifier, const char *host);

/**
   Called when the Programme instance is destroyed in the OOIF plugin.
   \param id Programme instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ProgrammeReleaseHandle(ProgrammeHandle id);

/**
   Gets the OIPF property name as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param name As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetName(ProgrammeHandle id, const char **name);

#ifdef OIPF
/**
   Gets the OIPF property longName as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param longName As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetLongName(ProgrammeHandle id, const char **longName);
#endif /* OIPF */

/**
   Gets the OIPF property description as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param description As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetDescription(ProgrammeHandle id, const char **description);

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   Gets the OIPF property longDescription as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param longDescription As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetLongDescription(ProgrammeHandle id, const char **longDescription);
#endif /* OIPF || HBBTV_1_2_1 */

/**
   Gets the OIPF property startTime as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param startTime As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetStartTime(ProgrammeHandle id, OOIFNumber *startTime);

/**
   Gets the OIPF property duration as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param duration As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetDuration(ProgrammeHandle id, OOIFNumber *duration);

/**
   Gets the OIPF property channelID as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param channelID As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetChannelID(ProgrammeHandle id, const char **channelID);

#ifdef OIPF
/**
   Gets the OIPF property episode as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param episode As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetEpisode(ProgrammeHandle id, OOIFNumber *episode);

/**
   Gets the OIPF property totalEpisodes as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param totalEpisodes As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetTotalEpisodes(ProgrammeHandle id, OOIFNumber *totalEpisodes);

/**
   Gets the OIPF property IMI as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param IMI As defined in 7.16.2.2
   \return OOIF return code
*/

#ifdef HMX_WEBUI
OOIFReturnCode native_ProgrammeGetSeason(RecordingHandle identifier, OOIFNumber *retval);
#endif

OOIFReturnCode native_ProgrammeGetIMI(ProgrammeHandle id, const char **IMI);

/**
   Gets the OIPF property groupCRIDs as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param count Number of components in the returned array
   \param groupCRIDs As defined in DAE 7.16.2.2. The OOIF framework
   takes ownership of the returned array and the strings it contains,
   and will free the memory when appropriate. malloc(), strdup(), or
   similar should be used in the native code to allocate memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetGroupCRIDs(ProgrammeHandle id, int *count, char ***groupCRIDs);
OOIFReturnCode native_ProgrammeGetCRIDSearchFlag(ProgrammeHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetProgrammeCRID(ProgrammeHandle id,const char **retval);
OOIFReturnCode native_ProgrammeGetRecommendationCRID(ProgrammeHandle id,const char **retval);

#endif /* OIPF */

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   Gets the OIPF property programmeID as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param programmeID As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetProgrammeID(ProgrammeHandle id, const char **programmeID);

/**
   Gets the OIPF property programmeIDType as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param programmeIDType As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetProgrammeIDType(ProgrammeHandle id, OOIFNumber *programmeIDType);

/**
   Sets the OIPF property programmeID as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param programmeID As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetProgrammeID(ProgrammeHandle id, const char* programmeID);

/**
   Sets the OIPF property programmeIDType as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param programmeIDType As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetProgrammeIDType(ProgrammeHandle id, OOIFNumber programmeIDType);
#endif /* OIPF || HBBTV_1_2_1 */

/**
   Sets the OIPF property name as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param name As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetName(ProgrammeHandle id, const char* name);

#ifdef OIPF
/**
   Sets the OIPF property longName as defined in DAE2.1 7.16.2.2.
   \param id Programme instance identifier
   \param longName As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetLongName(ProgrammeHandle id, const char* longName);
#endif /* OIPF */

/**
   Sets the OIPF property description as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param description As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetDescription(ProgrammeHandle id, const char* description);

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   Sets the OIPF property longDescription as defined in DAE2.1 7.16.2.2.
   \param id Programme instance identifier
   \param longDescription As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetLongDescription(ProgrammeHandle id, const char* longDescription);
#endif /* OIPF || HBBTV_1_2_1 */

/**
   Sets the OIPF property startTime as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param startTime As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetStartTime(ProgrammeHandle id, OOIFNumber startTime);

/**
   Sets the OIPF property duration as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param duration As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetDuration(ProgrammeHandle id, OOIFNumber duration);

/**
   Sets the OIPF property channelID as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param channelId As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetChannelID(ProgrammeHandle id, const char* channelId);

#ifdef OIPF
/**
   Sets the OIPF property episode as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param episode As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetEpisode(ProgrammeHandle id, OOIFNumber episode);

/**
   Sets the OIPF property totalEpisodes as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param totalEpisodes As defined in 7.16.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeSetTotalEpisodes(ProgrammeHandle id, OOIFNumber totalEpisodes);
#endif /* OIPF */

/**
   Gets the OIPF property parentalRating as defined in DAE 7.16.2.2.
   \param id Programme instance identifier
   \param count Number of components in the returned array
   \param handles Use this to return the parental ratings array. The
   OOIF plugin will take over ownership of the array and will later
   call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetParentalRating(ProgrammeHandle id, int *count, ParentalRatingHandle **handles);

#ifdef OIPF
/**
   Gets the OIPF property channel as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param channel As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetChannel(ProgrammeHandle id, ChannelHandle *channel);

/**
   Gets the OIPF property blocked as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param blocked As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetBlocked(ProgrammeHandle id, OOIFBoolean *blocked);

/**
   Gets the OIPF property showType as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param showType As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetShowType(ProgrammeHandle id, OOIFNumber *showType);

/**
   Gets the OIPF property subtitles as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param subtitles As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetSubtitles(ProgrammeHandle id, OOIFBoolean *subtitles);

/**
   Gets the OIPF property isHD as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param isHD As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetIsHD(ProgrammeHandle id, OOIFBoolean *isHD);

/**
   Gets the OIPF property audioType as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param audioType As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetAudioType(ProgrammeHandle id, OOIFNumber *audioType);

/**
   Gets the OIPF property isMultilingual as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param isMultilingual As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetIsMultilingual(ProgrammeHandle id, OOIFBoolean *isMultilingual);

/**
   Gets the OIPF property genre as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param count Number of components in the returned array
   \param genre As defined in 7.16.2.3.1. The OOIF framework takes ownership
   of the returned array and the strings it contains, and will free the memory
   when appropriate. malloc(), strdup(), or similar should be used in the native
   code to allocate memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetGenre(ProgrammeHandle id, int *count, char ***genre);

/**
   Gets the OIPF property hasRecording as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param hasRecording As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetHasRecording(ProgrammeHandle id, OOIFBoolean *hasRecording);

/**
   Gets the OIPF property audioLanguages as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param count Number of components in the returned array
   \param audioLanguages As defined in 7.16.2.3.1. The OOIF framework
   takes ownership of the returned array and the strings it contains,
   and will free the memory when appropriate. malloc(), strdup(), or
   similar should be used in the native code to allocate memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetAudioLanguages(ProgrammeHandle id, int *count, char ***audioLanguages);

/**
   Gets the OIPF property subtitleLanguages as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param count Number of components in the returned array
   \param subtitleLanguages As defined in 7.16.2.3.1. The OOIF
   framework takes ownership of the returned array and the strings
   it contains, and will free the memory when appropriate. malloc(),
   strdup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetSubtitleLanguages(ProgrammeHandle id, int *count, char ***subtitleLanguages);
#ifdef HMX_WEBUI
OOIFReturnCode native_ProgrammeGetSubtitleType(ProgrammeHandle id, OOIFNumber *subtitleType);
OOIFReturnCode native_ProgrammeGetSubtitleTypes(ProgrammeHandle id, int *count, char ***types);
#endif

/**
   Gets the OIPF property locked as defined in DAE 7.16.2.3.1.
   \param id Programme instance identifier
   \param locked As defined in 7.16.2.3.1
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetLocked(ProgrammeHandle id, OOIFBoolean *locked);

/**
   Gets the OIPF property recording as defined in DAE 7.16.2.5.
   \param id Programme instance identifier
   \param recording As defined in 7.16.2.5.
   \return OOIF return code
*/
#ifdef HMX_WEBUI
OOIFReturnCode native_ProgrammeGetScheduledRecording(ProgrammeHandle id, ScheduledRecordingHandle *retval);
OOIFReturnCode native_ProgrammeGetRecordings(ProgrammeHandle id, int *count, RecordingHandle **handles);
OOIFReturnCode native_ProgrammeGetRecording(ProgrammeHandle id, ScheduledRecordingHandle *retval);
/**
   get this programme information from IPEPG  dvb si
*/
OOIFReturnCode native_ProgrammeGetProgrammeIDSubtype(ProgrammeHandle identifier, const char **retval);
#else
OOIFReturnCode native_ProgrammeGetRecording(ProgrammeHandle id, PVRHandle *recording);
#endif

/**
  This function corresponds to the OIPF function getField() as defined in DAE 7.16.2.3.2.
   \param id Programme instance identifier
   \param fieldId As defined in 7.16.2.3.2.
   \param retval Field value as defined in 7.16.2.3.2.
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetField(ProgrammeHandle id, const char *fieldId, const char **retval);
#endif /* OIPF */
#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
  This function corresponds to the OIPF function getSIDescriptors() as defined in DAE 7.16.2.4.
   \param id Programme instance identifier
   \param descriptorTag As defined in 7.16.2.4.
   \param has_tagExtension Specifies if descriptorTagExtension parameter was provided
   by the calling java script. If false, value of descriptorTagExtension should not be used.
   \param descriptorTagExtension As defined in 7.16.2.4.
   \param count Number of components in the returned array
   \param sidescriptors As defined in 7.16.2.3.1. The OOIF framework
   takes ownership of the returned array and the strings it contains,
   and will free the memory when appropriate. malloc(), strdup(), or
   similar should be used in the native code to allocate memory for them.
   \param sidescriptor_lenghts Array of sidescriptors lengths. This array should have the same size as sidescriptors.
   The OOIF framework takes ownership of the returned array and the values it contains,
   and will free the memory when appropriate with free().
    malloc() should be used in the native code to allocate memory for them.
    Note: sidescriptor_lenghts can be NULL, in which case each sidescriptor string must be a plain null-terminated string.
    sidescriptor_lenghts is required when the strings in sidescriptors contain NULL bytes.
   \return OOIF return code
*/
OOIFReturnCode native_ProgrammeGetSIDescriptors(ProgrammeHandle id, OOIFNumber descriptorTag, OOIFBoolean has_tagExtension, OOIFNumber descriptorTagExtension, int *count, char ***sidescriptors, int **sidescriptor_lenghts);
#endif /* OIPF || HBBTV_1_2_1 */

#ifdef JLABS
OOIFReturnCode native_ProgrammeGetDigitalCopyControl(ProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_ProgrammeGetAnalogCopyControl(ProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_ProgrammeGetIs3D(ProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetIsCharged(ProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetContractVerificationInfo(ProgrammeHandle id, const char **retval);
OOIFReturnCode native_ProgrammeGetRecordingStatus(ProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_ProgrammeGetComponents(ProgrammeHandle id, OOIFNumber componentType, int *count, AVComponentHandle **retval);
OOIFReturnCode native_ProgrammeSetDigitalCopyControl(ProgrammeHandle id, OOIFNumber digitalCopyContorl);
OOIFReturnCode native_ProgrammeSetAnalogCopyControl(ProgrammeHandle id, OOIFNumber analogCopyControl);
OOIFReturnCode native_ProgrammeSetIs3D(ProgrammeHandle id, OOIFBoolean is3D);
OOIFReturnCode native_ProgrammeSetIsCharged(ProgrammeHandle id, OOIFBoolean isCharged);
OOIFReturnCode native_ProgrammeSetContractVerificationInfo(ProgrammeHandle id, const char *contractVerificationInfo);
OOIFReturnCode native_ProgrammeGetSharedProgrammeIds(ProgrammeHandle id, int *count,	char ***sharedProgrammeIds);
OOIFReturnCode native_ProgrammeGetSharedProgrammeInfo(ProgrammeHandle id, const char *sharedProgrammeId, ProgrammeHandle *pSharedProgramme);

#endif // JLABS


// MediaDeeplink
OOIFReturnCode native_ProgrammeGetMediaDeeplinkID(ProgrammeHandle id, const char **retval);
OOIFReturnCode native_ProgrammeGetMediaDeeplinkUrl(ProgrammeHandle id, const char **retval);
OOIFReturnCode native_ProgrammeGetMediaDeeplinkExpireDate(ProgrammeHandle id, OOIFNumber *retval);


// OnDemand
OOIFReturnCode native_ProgrammeGetOnDemandStartOfAvailability(ProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_ProgrammeGetOnDemandEndOfAvailability(ProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_ProgrammeGetOnDemandPublishedDuration(ProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_ProgrammeGetOnDemandIsFree(ProgrammeHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetOnDemandIsHD(ProgrammeHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetOnDemandIsMediaAvailable(ProgrammeHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetOnDemandIsFEPGAvailable(ProgrammeHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_ProgrammeGetOnDemandProgramUrl(ProgrammeHandle id, const char **retval);
OOIFReturnCode native_ProgrammeGetOnDemandAuxiliaryUrl(ProgrammeHandle id, const char **retval);
OOIFReturnCode native_ProgrammeGetOnDemandDeliveryMode(ProgrammeHandle id, const char **retval);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_PROGRAMME */
