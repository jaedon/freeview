/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSNETWORKSERVICE__
#define __NATIVE_JLABSNETWORKSERVICE__

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsRemoteControlFunctionHandle;
typedef	void (RCFEventCallback)( int nSvcIndex, int nEvent, int nParam1, int nParam2 );


OOIFReturnCode native_jlabsRemoteControlFunctionCreate(jlabsRemoteControlFunctionHandle *id, long int window_identifier, const char *host_uri);

OOIFReturnCode native_jlabsRemoteControlFunctionGetDeviceHandle(jlabsRemoteControlFunctionHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsRemoteControlFunctionGetDeviceUA(jlabsRemoteControlFunctionHandle identifier, const char **retval);
OOIFReturnCode native_jlabsRemoteControlFunctionServerSideXMLUIListing(jlabsRemoteControlFunctionHandle identifier, const char *UIList, OOIFBoolean advertise, OOIFBoolean *retval);
OOIFReturnCode native_jlabsRemoteControlFunctionSendRemoteMessage(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFNumber reqHandle, const char * hearders,  const char * message, OOIFBoolean *retval);
OOIFReturnCode native_jlabsRemoteControlFunctionSendMulticatNotify(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFNumber eventLevel, const char * notifCEHTML, const char *friendlyName, const char *profileList, OOIFBoolean *retval);
OOIFReturnCode native_jlabsRemoteControlFunctionSendInternalServerError(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFNumber reqHandle, OOIFBoolean *retval);
OOIFReturnCode native_jlabsRemoteControlFunctionDropConnection(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFBoolean *retval);


OOIFReturnCode native_jlabsRemoteControlFunctionReleaseHandle(jlabsRemoteControlFunctionHandle identifier);

OOIFReturnCode native_jlabsRemoteControlFunctionRegisterEventHandler( RCFEventCallback  cbFunc);
OOIFReturnCode native_jlabsRemoteControlFunctionUnRegisterEventHandler( RCFEventCallback  cbFunc);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSNETWORKSERVICE__ */
