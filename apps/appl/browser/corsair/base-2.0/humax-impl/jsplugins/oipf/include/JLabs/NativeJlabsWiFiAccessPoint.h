/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABSWIFIACCESSPOINT__
#define __NATIVE_JLABSWIFIACCESSPOINT__

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *jlabsWiFiAccessPointHandle;

OOIFReturnCode native_jlabsWiFiAccessPointGetEssid(jlabsWiFiAccessPointHandle identifier, const char **retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetMacAddress(jlabsWiFiAccessPointHandle identifier, const char **retval);
OOIFReturnCode native_jlabsWiFiAccessPointSetEssid(jlabsWiFiAccessPointHandle identifier, const char *retval);


OOIFReturnCode native_jlabsWiFiAccessPointGetSecureTypeN(jlabsWiFiAccessPointHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetAuthenTypeN(jlabsWiFiAccessPointHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetEncryptTypeN(jlabsWiFiAccessPointHandle identifier, OOIFNumber *retval);

OOIFReturnCode native_jlabsWiFiAccessPointGetSecureType(jlabsWiFiAccessPointHandle identifier, const char **retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetAuthenType(jlabsWiFiAccessPointHandle identifier, const char **retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetEncryptType(jlabsWiFiAccessPointHandle identifier, const char **retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetMode(void * identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetWaveStrength(jlabsWiFiAccessPointHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsWiFiAccessPointGetConnectSpeed(jlabsWiFiAccessPointHandle identifier, OOIFNumber *retval);

OOIFReturnCode native_jlabsWiFiAccessPointReleaseHandle(void * identifier);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSWIFIACCESSPOINT__ */
