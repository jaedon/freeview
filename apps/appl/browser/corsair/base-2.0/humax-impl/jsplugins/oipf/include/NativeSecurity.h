/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_SECURITY
#define __NATIVE_SECURITY

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup Security Security
   @{

   This is the native interface for handling security functions that
   are outside of OIPF/HbbTV defined objects.
 */

/**
    If jsplugin.ini is configured to use callbacks (see \ref activation),
    this function is called when new document is loaded to determine if OOIF
    should be activated on it.
    \param protocol Indicates which protocol the page was loaded on
	\param hostname Indicates which host the page was loaded from
    \param port Indicates which port on the host was used to access the page
    \param retval Set this to TRUE to allow loading of OOIF on this
    page, FALSE to refuse it
    \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SecurityAllowAccess(const char *protocol, const char *hostname, int port, OOIFBoolean *retval);

/** @} */
#ifdef HBBTV_SECURITY
/**
  \defgroup Permissions Permissions
  @{

  This is the native interface for handling security and permission
  mechanisms as described in OIPF DAE 10.1 and HBBTV 11.1.

  Note that these mechanisms differs between OIPF and HbbTV.
  */


/**
  Checks if a context is trusted in a window identified by
  window_identifier. If OOIF_RETURN_OK is returned, the context will
  be considered 'trusted' as defined in HBBTV
  A1 Table 19.

  \param window_identifier The identifier of the window the
  context is running in
  \param host_uri The context host URI
  \return OOIF_RETURN_OK if context is trusted, OOIF_RETURN_SECURITY_ERROR if
  the context is not.
  */
OOIFReturnCode native_PermissionsIsTrusted(long int window_identifier, const char *host_uri);

/**
  Checks if a context is broadcast related in a window identified by
  window_identifier. If OOIF_RETURN_OK is returned, the context will
  be considered 'broadcast related' as defined in HBBTV
  A1 Table 19.

  \param window_identifier The identifier of the window the
  context is running in
  \param host_uri The context host URI
  \return OOIF_RETURN_OK if context is broadcast related,
  OOIF_RETURN_SECURITY_ERROR if the context is not.
  */
OOIFReturnCode native_PermissionsIsBroadcastRelated(long int window_identifier, const char *host_uri);

/** @} */
#endif /* HBBTV_SECURITY */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_SECURITY */
