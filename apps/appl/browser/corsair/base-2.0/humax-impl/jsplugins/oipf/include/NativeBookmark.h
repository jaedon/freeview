/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF

#ifndef __NATIVE_BOOKMARK
#define __NATIVE_BOOKMARK

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \addtogroup SchedRec
  @{

   This is the native interface for the The Bookmark class
   as defined in DAE 7.10.8
*/

/**
   Opaque reference to a Bookmark in the native platform.
*/
typedef void* BookmarkHandle;

/**
   Gets OIPF property time as defined in DAE 7.10.8.1.
   \param identifier Bookmark instance identifier
   \param retval Retval as defined in 7.10.8.1
   \return OOIF return code
*/
OOIFReturnCode native_BookmarkGetTime(BookmarkHandle identifier, OOIFNumber *retval);

/**
   Gets OIPF property name as defined in DAE 7.10.8.1.
   \param identifier Bookmark instance identifier
   \param retval Retval as defined in 7.10.8.1
   \return OOIF return code
*/
OOIFReturnCode native_BookmarkGetName(BookmarkHandle identifier, const char **retval);

/**
   Called when the Bookmark instance is destroyed in the OOIF plugin.
   \param id Bookmark instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_BookmarkReleaseHandle(BookmarkHandle id);

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_BOOKMARK */
#endif /* OIPF */
