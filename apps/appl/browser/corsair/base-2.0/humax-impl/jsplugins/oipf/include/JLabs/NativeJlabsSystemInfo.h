/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_SYSTEMINFO
#define __NATIVE_JLABS_SYSTEMINFO

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*** B.3.4.1.6. jlabsSystemInfo Å¬·¡½º ***/

/* GETTER - read only */

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisModemStatus(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisDownFrequency(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoSetDocsisDownFrequency(OOIFNumber downFrequency, OOIFBoolean *pbSuccess);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisQamType(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisDownSignalLevel(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetdDocsisDownBer(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisDownSnr(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisUpFrequency(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisUpChannelId(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisUpBandWidth(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDocsisUpSignalLevel(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetCardIdBCAS(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDiscernBCAS(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetGroupIdBCAS(int *pNumOfGroupId, char ***retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetCardIdCCAS(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDiscernCCAS(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetGroupIdCCAS(int *pNumOfGroupId, char ***retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDeviceId(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDeviceModel(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDeviceSerialNumber(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDeviceSoftwareVersion(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetDeviceHardwareVersion(const char **retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetSystemUpdateDate(OOIFNumber *retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetCardStatusBCAS(OOIFBoolean*retval);

/* ... */
OOIFReturnCode native_jlabsSystemInfoGetCardStatusCCAS(OOIFBoolean *retval);

OOIFReturnCode native_jlabsSystemInfoGet(const char *name, const char **retval);

OOIFReturnCode native_jlabsSystemInfoStartSettingInfo(OOIFNumber settingInfoOpt, OOIFBoolean *bSuccess);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_SYSTEMINFO */

#endif // #if defined OIPF && defined JLABS
