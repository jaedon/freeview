/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_AV_COMPONENT
#define __NATIVE_AV_COMPONENT

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \defgroup AVComponent AVComponent class
  @{

   This is the native interface for the The AVComponent class as
   defined in DAE1.1 7.13.4.2 and DAE 7.16.5
 */

/**
   Opaque reference to an AVComponent in the native platform.
 */
typedef void* AVComponentHandle;

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   The following constants define type of AV component
   as described in the DAE 7.16.5.1
*/
typedef enum
{
	COMPONENT_TYPE_VIDEO = 0,
	COMPONENT_TYPE_AUDIO = 1,
	COMPONENT_TYPE_SUBTITLE = 2
} AVComponentType;

/**
   This data struct holds AV Component properties as defined in
   DAE 7.16.5.2.1. The OIPF plugin will take over ownership
   of all strings in the data struct, strdup() should be used in the
   native code to allocate memory for them. The 'has_*' flags set to false
   correspond to 'Undefined' value of associated parameter.
*/
typedef struct
{
	OOIFBoolean has_componentTag;
	OOIFNumber componentTag;
	OOIFBoolean has_pid;
	OOIFNumber pid;
	AVComponentType type;
	char* encoding;
	OOIFBoolean encrypted;
	union
	{
		/**
		   This data struct holds AV Video Component properties as defined in DAE 7.16.5.3.1.
		   They should be populated only when the type property equals COMPONENT_TYPE_VIDEO
		*/
		struct
		{
			OOIFBoolean has_aspectRatio;
			OOIFNumber aspectRatio;
		};
		/**
		   This data struct holds AV Audio Component properties as defined in DAE 7.16.5.4.1.
		   They should be populated only when the type property equals COMPONENT_TYPE_AUDIO
		*/
		struct
		{
			char* audio_language;
			OOIFBoolean audioDescription;
			OOIFNumber audioChannels;
		};
		/**
		   This data struct holds AV Audio Component properties as defined in DAE 7.16.5.5.1.
		   They should be populated only when the type property equals COMPONENT_TYPE_SUBTITLE
		*/
		struct
		{
			char* subtitle_language;
			OOIFBoolean hearingImpaired;
		};
	};
} AVComponentData;
#else
/**
   This data struct holds AV Component properties as defined in DAE
   7.13.4.2.1. The OIPF plugin will take over ownership
   of all strings in the data struct, strdup() should be used in the
   native code to allocate memory for them. The 'has_*' flags set to false
   correspond to 'Undefined' value of associated parameter.
*/
typedef struct {
#ifdef OIPF
        OOIFNumber componentTag;
        OOIFNumber pid;
#endif /* OIPF */
	OOIFNumber type;
	char *encoding;
	OOIFBoolean encrypted;
	OOIFBoolean has_aspectRatio;
	OOIFNumber aspectRatio;
	char *language;
	OOIFBoolean audioDescription;
	OOIFNumber audioChannels;
	OOIFBoolean hearingImpaired;
} AVComponentData;
#endif /* OIPF || HBBTV_1_2_1 */

/**
   Populate the data struct fields with the property values of an AV
   Component instance as defined in DAE1.1 7.13.4.2.1 and
   DAE 7.16.5.2.1. The OOIF Plugin will handle all getter calls.
   \param identifier The handle of the AVComponent
   \param data The data struct to be initialized
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AVComponentInitialize(AVComponentHandle identifier, AVComponentData *data);

/**
   Called when the AVComponent instance is destroyed in the OOIF plugin.
   \param id AVComponent instance identifier
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_AVComponentReleaseHandle(AVComponentHandle id);
/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_AV_COMPONENT */
