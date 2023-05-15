/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __NATIVE_SIGNAL_INFO
#define __NATIVE_SIGNAL_INFO

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @name SignalInfo
	The SignalInfo class as defined in DAE2.1 7.3.10.
	@{*/

/**
   Opaque reference to a SignalInfo in the native platform.
 */
typedef void *SignalInfoHandle;

/**
   Called when the SignalInfo instance is destroyed in the OOIF plugin.
   \param id SignalInfo instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_SignalInfoReleaseHandle(SignalInfoHandle id);

/**
   Gets OIPF property strength as defined in DAE2.1 7.3.10.1.
   \param id SignalInfo instance identifier
   \param retval Retval as defined in 7.3.10.1
   \return OOIF return code
*/
OOIFReturnCode native_SignalInfoGetStrength(SignalInfoHandle id, OOIFNumber *retval);

/**
   Gets OIPF property quality as defined in DAE2.1 7.3.10.1.
   \param id SignalInfo instance identifier
   \param retval Retval as defined in 7.3.10.1
   \return OOIF return code
*/
OOIFReturnCode native_SignalInfoGetQuality(SignalInfoHandle id, OOIFNumber *retval);

/**
   Gets OIPF property ber as defined in DAE2.1 7.3.10.1.
   \param id SignalInfo instance identifier
   \param retval Retval as defined in 7.3.10.1
   \return OOIF return code
*/
OOIFReturnCode native_SignalInfoGetBer(SignalInfoHandle id, OOIFNumber *retval);

/**
   Gets OIPF property snr as defined in DAE2.1 7.3.10.1.
   \param id SignalInfo instance identifier
   \param retval Retval as defined in 7.3.10.1
   \return OOIF return code
*/
OOIFReturnCode native_SignalInfoGetSnr(SignalInfoHandle id, OOIFNumber *retval);

/**
   Gets OIPF property lock as defined in DAE2.1 7.3.10.1.
   \param id SignalInfo instance identifier
   \param retval Retval as defined in 7.3.10.1
   \return OOIF return code
*/
OOIFReturnCode native_SignalInfoGetLock(SignalInfoHandle id, OOIFBoolean *retval);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_SIGNAL_INFO */
#endif /* OIPF */
