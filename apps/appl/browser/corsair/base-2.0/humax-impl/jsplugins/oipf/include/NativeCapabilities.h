/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_CAPABILITIES
#define __NATIVE_CAPABILITIES

#include "NativeOOIFTypes.h"

#if defined(OIPF) && (defined(HBBTV) || defined(HBBTV_1_2_1))
# error "Do not mix OIPF with HBBTV or HBBTV_1_2_1 defines!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \defgroup Capability Capabilities
  @{

   This is the native interface for Capabilities as defined in DAE 7.15.3.
*/

/**
   Gets the OIPF property xmlCapabilities as defined in DAE 7.15.3.1
   \param retval Retval as defined in DAE 7.15.3.1
   \return OOIF return code
*/
OOIFReturnCode native_CapabilityGetXmlCapabilities(const char **retval);

/**
   This function corresponds to the OIPF function hasCapability() as
   defined in DAE 7.15.3.2.
   \param profileName As defined in DAE 7.15.3.2
   \param retval Retval as defined in DAE 7.15.3.2
   \return OOIF return code
 */
OOIFReturnCode native_CapabilityHasCapability(const char *profileName, OOIFBoolean *retval);

/**
   Gets the OIPF property extraSDVideoDecodes as defined in DAE 7.15.3.1.
   \param retval Retval as defined in DAE 7.15.3.1
   \return OOIF return code
 */
OOIFReturnCode native_CapabilityGetExtraSDVideoDecodes(OOIFNumber *retval);

/**
   Gets the OIPF property extraHDVideoDecodes as defined in DAE 7.15.3.1.
   \param retval Retval as defined in DAE 7.15.3.1
   \return OOIF return code
 */
OOIFReturnCode native_CapabilityGetExtraHDVideoDecodes(OOIFNumber *retval);

/*@}*/

/**
  \defgroup ObjectFactory ObjectFactory class
  @{

   Native interface for ObjectFactory, as defined in DAE 7.1.
*/

/**
   This function corresponds to the OIPF function isObjectSupported() as
   defined in DAE 7.1.1
   \param mimeType As defined in DAE 7.1.1
   \param retval Retval as defined in DAE 7.1.1
   \return OOIF return code
 */
OOIFReturnCode native_ObjectFactoryIsObjectSupported(const char * mimeType, OOIFBoolean *retval);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_CAPABILITIES */
