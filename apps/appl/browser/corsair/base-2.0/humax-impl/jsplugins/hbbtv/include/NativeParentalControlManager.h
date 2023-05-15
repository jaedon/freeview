/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_PARENTAL_CONTROL
#define __NATIVE_PARENTAL_CONTROL

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   Called when a new ParentalControlManager instance is created. The
   return value may be used for capability and security checks.
   \param window The identifier of the window the create call originates from
   \param host The host URI of the application the create call originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_ParentalControlManagerOnCreate(long int window, const char *host);

/**
  \defgroup ParentalControl Parental Rating and Parental Control
  @{

   This is the native interface for the Parental rating and parental
   control APIs as defined in DAE 7.9.
 */

/**
   Opaque reference to a ParentalRatingScheme in the native platform.
 */
typedef void *ParentalRatingSchemeHandle;
/**
   Opaque reference to a ParentalRatingHandle in the native platform.
 */
typedef void *ParentalRatingHandle;

/**
   Gets the OIPF property parentalRatingSchemes as defined in DAE 7.9.1.1.
   \param count Number of returned schemes
   \param handles Return the rating schemes known by the OITF
   here. The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ParentalControlGetParentalRatingSchemes(int *count, ParentalRatingSchemeHandle **handles);

#ifdef OIPF
/**
    Gets the OIPF property isPINEntryLocked as defined in DAE 7.9.1.1.
    \param retval Property value as defined in DAE 7.9.1.1
    \return OOIF return code
*/
OOIFReturnCode native_ParentalControlGetIsPINEntryLocked(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function setParentalControlStatus() as
   defined in DAE 7.9.1.2.
   \param pcPIN As defined in DAE 7.9.1.2
   \param enable As defined in DAE 7.9.1.2
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlSetParentalControlStatus(const char *pcPIN, OOIFBoolean enable, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function getParentalControlStatus() as
   defined in DAE 7.9.1.2.
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlGetParentalControlStatus(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function getBlockUnrated() as
   defined in DAE 7.9.1.2.
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlGetBlockUnrated(OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function setParentalControlPIN() as
   defined in DAE 7.9.1.2.
   \param oldPcPIN As defined in DAE 7.9.1.2
   \param newPcPIN As defined in DAE 7.9.1.2
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlSetParentalControlPIN(const char *oldPcPIN, const char *newPcPIN, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function unlockWithParentalControlPIN() as
   defined in DAE 7.9.1.2.
   \param pcPIN As defined in DAE 7.9.1.2
   \param target Either a ChannelHandle, VodHandle, or VideoBroadcastHandle
   indicating the object to be unlocked
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlUnlockWithParentalControlPIN(const char *pcPIN, void *target, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function verifyParentalControlPIN() as
   defined in DAE 7.9.1.2.
   \param pcPIN As defined in DAE 7.9.1.2
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlVerifyParentalControlPIN(const char *pcPIN, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function setBlockUnrated() as
   defined in DAE 7.9.1.2.
   \param pcPIN As defined in DAE 7.9.1.2
   \param block As defined in DAE 7.9.1.2
   \param retval Function return value as defined in DAE 7.9.1.2
   \return OOIF return code
 */
OOIFReturnCode native_ParentalControlSetBlockUnrated(const char *pcPIN, OOIFBoolean block, OOIFNumber *retval);
#endif /* OIPF */

/*------------------------------------------
  ParentalRatingScheme
  ------------------------------------------*/
/** @name ParentalRatingScheme
	The ParentalRatingScheme class as defined in DAE 7.9.2.
	@{*/

/**
   Gets all the ratings in a parental rating scheme.
   \param identifier The identifier of the parental rating scheme instance.
   \param count The number of returned ratings
   \param handles Return the ratings here. The OOIF plugin will take
   over ownership of the array and will later call free() to de-allocate the memory.
   Use strdup() to allocate strings.
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_ParentalRatingSchemeGetRatings(ParentalRatingSchemeHandle id, int *count, const char ***handles);

/**
   Called when the ParentalRatingScheme instance is destroyed in the OOIF plugin.
   \param id ParentalRatingScheme instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ParentalRatingSchemeReleaseHandle(ParentalRatingSchemeHandle id);

/**
   Gets the OIPF property name as defined in DAE 7.9.2.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingSchemeGetName(ParentalRatingSchemeHandle id, const char **retval);

/**
   Gets the OIPF property threshold as defined in DAE 7.9.2.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.2.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingSchemeGetThreshold(ParentalRatingSchemeHandle id, ParentalRatingHandle *retval);

/**
   This function corresponds to the OIPF function iconUri() as
   defined in DAE 7.9.2.2.
   \param id ParentalRatingScheme instance identifier
   \param index as defined in DAE 7.9.2.2
   \param retval Retval as defined in DAE 7.9.2.2
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingSchemeIconUri(ParentalRatingSchemeHandle id, OOIFNumber index, const char **retval);

/*@}*/
/*------------------------------------------
 ParentalRatingSchemeCollection
 ------------------------------------------*/
/** @name ParentalRatingSchemeCollection
	The ParentalRatingSchemeCollection class as defined in DAE 7.9.3.
	@{*/
#ifdef OIPF
/**
   This function corresponds to the OIPF function addParentalRatingScheme() as
   defined in DAE 7.9.3.1.
   \param name As defined in DAE 7.9.3.1
   \param values As defined in DAE 7.9.3.1
   \param retval Function return value as defined in DAE 7.9.3.1
   \return OOIF return code
*/
OOIFReturnCode native_PRSCollectionAddParentalRatingScheme(const char *name, const char *values, ParentalRatingSchemeHandle *retval);
#endif /* OIPF */

/*@}*/
/*------------------------------------------
  ParentalRating
  ------------------------------------------*/
/** @name ParentalRating
	The ParentalRating class as defined in DAE 7.9.4.
	@{*/

/**
   Called when the ParentalRating instance is destroyed in the OOIF plugin.
   \param id ParentalRating instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_ParentalRatingReleaseHandle(ParentalRatingHandle id);

/**
   Gets the OIPF property name as defined in DAE 7.9.4.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.4.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingGetName(ParentalRatingHandle id, const char **retval);

/**
	Gets the OIPF property scheme as defined in DAE 7.9.4.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.4.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingGetScheme(ParentalRatingHandle id, const char **retval);

/**
   Gets the OIPF property value as defined in DAE 7.9.4.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.4.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingGetValue(ParentalRatingHandle id, OOIFNumber *retval);

/**
   Gets the OIPF property labels as defined in DAE 7.9.4.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.4.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingGetLabels(ParentalRatingHandle id, OOIFNumber *retval);

/**
   Gets the OIPF property region as defined in DAE 7.9.4.1.
   \param id ParentalRatingScheme instance identifier
   \param retval Retval as defined in DAE 7.9.4.1
   \return OOIF return code
*/
OOIFReturnCode native_ParentalRatingGetRegion(ParentalRatingHandle id, const char **retval);

#if defined(HBBTV_PVR) || defined(OIPF)
/*@}*/
/*------------------------------------------
  ParentalRatingCollection
  ------------------------------------------*/
/** @name ParentalRatingCollection
	The ParentalRatingCollection class as defined in DAE 7.9.5.
	@{*/

/**
   This function corresponds to the OIPF function addParentalRating() as
   defined in DAE 7.9.5.2.
   \param id Identifier of the object instance that holds the
   ParentalRatingCollection to add the new rating to (e.g., a
   ChannelHandle or a ProgrammeHandle)
   \param scheme As defined in DAE 7.9.5.2
   \param name As defined in DAE 7.9.5.2
   \param has_value Will be FALSE if parameter 'value' is undefined
   \param value As defined in DAE 7.9.5.2
   \param labels As defined in DAE 7.9.5.2
   \param region As defined in DAE 7.9.5.2
   \param window_identifier The identifier of the window the create
   call originates from
   \param host The host URI of the application the create call
   originates from
   \param retval A reference to the created parental rating instance
   in the native layer
   \return OOIF return code
 */
OOIFReturnCode native_PRCollectionAddParentalRating(void *id, const char *scheme, const char *name, OOIFBoolean has_value, OOIFNumber value, OOIFNumber labels, const char *region, long int window_identifier, const char *host, ParentalRatingHandle *retval);

/*@}*/
#endif /* HBBTV_PVR || OIPF */

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_PARENTAL_CONTROL */
