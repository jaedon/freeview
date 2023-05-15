/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABS_PB_PROGRAMME
#define __NATIVE_JLABS_PB_PROGRAMME

#include "NativeOOIFTypes.h"
#include "NativeChannel.h"
#include "NativeRecording.h"
#include "NativeParentalControlManager.h"
#include "NativeAVComponent.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
   Opaque reference to a Programme in the native platform.
 */
typedef void* jlabsPbProgrammeHandle;

typedef struct 
{
	OOIFNumber		svcuid;
	OOIFNumber		eventId;	
	OOIFNumber		parentalRating;
	char 			*name;
	char			*description;
	char			*longDescription;
	OOIFNumber		startTime;
	OOIFNumber		duration;
	char			*channelID;
	char			*programmeID;
	OOIFNumber		programmeIDType;	
	OOIFBoolean		hasRecording;
	OOIFBoolean		subtitles;
	OOIFBoolean		isHD;
	OOIFNumber		content;
	OOIFNumber		digitalCopyControl;
	OOIFNumber		analogCopyControl;
	OOIFBoolean		is3D;
	OOIFBoolean		isCharged;
	char			*contractVertificationInfo;
	OOIFNumber		recordingStatus;
} jlabsPbProgrammeData;


/**
   Called when the Programme instance is destroyed in the OOIF plugin.
   \param id Programme instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_jlabsPbProgrammeReleaseHandle(jlabsPbProgrammeHandle id);
OOIFReturnCode native_jlabsPbProgrammeGetParentalRating(jlabsPbProgrammeHandle id, int *count, ParentalRatingHandle **handles);
OOIFReturnCode native_jlabsPbProgrammeGetName(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetDescription(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetLongDescription(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetStartTime(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetDuration(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetChannelID(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetProgrammeID(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetProgrammeIDType(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetChannel(jlabsPbProgrammeHandle id, ChannelHandle *retval);
OOIFReturnCode native_jlabsPbProgrammeGetProgrammeID(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetProgrammeIDType(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetChannel(jlabsPbProgrammeHandle id, ChannelHandle *retval);
OOIFReturnCode native_jlabsPbProgrammeGetHasRecording(jlabsPbProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_jlabsPbProgrammeGetSubtitles(jlabsPbProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_jlabsPbProgrammeGetIsHD(jlabsPbProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_jlabsPbProgrammeGetGenre(jlabsPbProgrammeHandle id, int *count, char ***genres);
OOIFReturnCode native_jlabsPbProgrammeGetField(jlabsPbProgrammeHandle id, const char *fieldId,	const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetDigitalCopyControl(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetAnalogCopyControl(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetIs3D(jlabsPbProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_jlabsPbProgrammeGetIsCharged(jlabsPbProgrammeHandle id, OOIFBoolean *retval);
OOIFReturnCode native_jlabsPbProgrammeGetContractVertificationInfo(jlabsPbProgrammeHandle id, const char **retval);
OOIFReturnCode native_jlabsPbProgrammeGetRecordingStatus(jlabsPbProgrammeHandle id, OOIFNumber *retval);
OOIFReturnCode native_jlabsPbProgrammeGetComponents(jlabsPbProgrammeHandle id, int componentType, int *count, AVComponentHandle **components);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_PROGRAMME */
