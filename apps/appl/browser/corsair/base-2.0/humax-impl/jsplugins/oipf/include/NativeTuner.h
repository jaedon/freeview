/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __NATIVE_TUNER
#define __NATIVE_TUNER

#include "NativeOOIFTypes.h"
#include "NativeSignalInfo.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
   Opaque reference to a Tuner in the native platform.
 */
typedef void *TunerHandle;

/**
   This data struct holds the static properties of a Tuner as
   defined in DAE2.1 7.3.9.1. The OOIF plugin will take over ownership
   of all strings in the data struct, strdup() or malloc() should be used
   in the native code to allocate memory for them.
*/
typedef struct {
	OOIFNumber id;
	char *name;
} TunerData;

/**
   Called when the Tuner instance is destroyed in the OOIF plugin.
   \param id Tuner instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_TunerReleaseHandle(TunerHandle id);

/**
   Populate the TunerData struct with the value of the Tuner properties
   defined in DAE2.1 7.3.9.1. The OOIF plugin will handle all getter calls on
   these parameter.
   \param id The instance identifier of the Tuner to get properties of
   \param data property struct to populate
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_TunerInitialize(TunerHandle id, TunerData *data);

/**
   Gets OIPF property enableTuner as defined in DAE2.1 7.3.9.1.
   \param id Tuner instance identifier
   \param retval Retval as defined in 7.3.9.1
   \return OOIF return code
*/
#if defined(HMX_WEBUI)
OOIFReturnCode native_HTunerGetEnabled(TunerHandle identifier, OOIFBoolean *retval);
#else
OOIFReturnCode native_TunerGetEnableTuner(TunerHandle id, OOIFBoolean *retval);
#endif

/**
   Gets OIPF property powerOnExternal as defined in DAE2.1 7.3.9.1.
   \param id Tuner instance identifier
   \param retval Retval as defined in 7.3.9.1
   \return OOIF return code
*/
#if defined(HMX_WEBUI)
OOIFReturnCode native_HTunerGetPowerOnExternal(TunerHandle identifier, OOIFBoolean *retval);
#else
OOIFReturnCode native_TunerGetPowerOnExternal(TunerHandle id, OOIFBoolean *retval);
#endif

/**
   Gets OIPF property SignlaInfo as defined in DAE2.1 7.3.9.1.
   \param id Tuner instance identifier
   \param retval Retval as defined in 7.3.9.1
   \return OOIF return code
*/
OOIFReturnCode native_TunerGetSignalInfo(TunerHandle id, SignalInfoHandle *retval);

/**
   Gets OIPF property idTypes as defined in DAE2.1 7.3.9.1.
   \param id Tuner instance identifier
   \param count Size of the array returned in types parameter
   \param types Property values array as defined in 7.3.9.1.
   The OOIF plugin will take over ownership of the array and
   will later call free() to de-allocate the memory.
   \return OOIF return code
*/
#if defined(HMX_WEBUI)
OOIFReturnCode native_HTunerGetIdTypes(TunerHandle identifier, int *count, char ***strings);
#else
OOIFReturnCode native_TunerGetIdTypes(TunerHandle id, int *count, char ***types);
#endif

/**
   Sets OIPF property enableTuner as defined in DAE2.1 7.3.9.1.
   \param id Tuner instance identifier
   \param value As defined in 7.3.9.1
   \return OOIF return code
*/
OOIFReturnCode native_TunerSetEnableTuner(TunerHandle id, OOIFBoolean value);

/**
   Sets OIPF property powerOnExternal as defined in DAE2.1 7.3.9.1.
   \param id Tuner instance identifier
   \param value As defined in 7.3.9.1
   \return OOIF return code
*/
OOIFReturnCode native_TunerSetPowerOnExternal(TunerHandle id, OOIFBoolean value);

#if defined(HMX_WEBUI)
#define	____Tuner_Getter_Functions____
OOIFReturnCode	native_HTunerGetId(TunerHandle identifier, OOIFNumber *retval);
OOIFReturnCode	native_HTunerGetName(TunerHandle identifier, const char **retval);
OOIFReturnCode	native_HTunerGetSignalInfoHandle(TunerHandle identifier, SignalInfoHandle *retval);
#endif
/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_TUNER */
#endif /* OIPF */
