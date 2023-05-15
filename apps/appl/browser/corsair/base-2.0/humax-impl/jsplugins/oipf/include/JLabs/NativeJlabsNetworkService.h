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

typedef void *jlabsNetworkServiceHandle;
typedef void *jlabsWiFiAccessPointHandle;
typedef	void (NetworkServiceEventCallback)( int nSvcIndex, int nEvent, int nParam1, int nParam2 );

OOIFReturnCode native_jlabsNetworkServiceGetServiceName(jlabsNetworkServiceHandle identifier, const char **retval);

OOIFReturnCode native_jlabsNetworkServiceGetTargetName(jlabsNetworkServiceHandle identifier, const char **retval);
OOIFReturnCode native_jlabsNetworkServiceSetTargetName(jlabsNetworkServiceHandle identifier, const char *retval);

OOIFReturnCode native_jlabsNetworkServiceGet(jlabsNetworkServiceHandle identifier, const char *name, const char **retval);
OOIFReturnCode native_jlabsNetworkServiceSet(jlabsNetworkServiceHandle identifier, const char *name, const char *value, const char **retval);

OOIFReturnCode native_jlabsNetworkServiceRestart(jlabsNetworkServiceHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsNetworkServiceStop(jlabsNetworkServiceHandle identifier, OOIFNumber *retval);

OOIFReturnCode native_jlabsNetworkServiceReleaseHandle(jlabsNetworkServiceHandle identifier);

OOIFReturnCode native_jlabsNetworkServiceGetSvcIndex( jlabsNetworkServiceHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_jlabsNetworkServiceGetTargetIndex( jlabsNetworkServiceHandle identifier, OOIFNumber *retval );
OOIFReturnCode native_jlabsNetworkServiceRegisterEventHandler( NetworkServiceEventCallback  cbFunc);
OOIFReturnCode native_jlabsNetworkServiceUnRegisterEventHandler( NetworkServiceEventCallback  cbFunc);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSNETWORKSERVICE__ */
