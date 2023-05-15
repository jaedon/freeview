/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF

#ifndef __NATIVE_DISC
#define __NATIVE_DISC

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * See DAE 7.16.4
 * \defgroup Disc Storage (DiscInfo)
 * @{
 */


/**
   Called when a new DiscInfo instance is created. The return
   value may be used for capability checks.
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_DiscInfoOnCreate();

/**
  Gets OIPF property free as defined in DAE 7.16.4.1
  \param retval Property value as defined in DAE 7.16.4.1
  \return OOIF return code
*/
OOIFReturnCode native_DiscInfoGetFree(OOIFNumber *retval);

/**
   Gets OIPF property total as defined in DAE 7.16.4.1
   \param retval Property value as defined in DAE 7.16.4.1
   \return OOIF return code
*/
OOIFReturnCode native_DiscInfoGetTotal(OOIFNumber *retval);

/**
   Gets OIPF property reserved as defined in DAE 7.16.4.1
   \param retval Property value as defined in DAE 7.16.4.1
   \return OOIF return code
*/
OOIFReturnCode native_DiscInfoGetReserved(OOIFNumber *retval);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_DISC */
#endif /* OIPF */
