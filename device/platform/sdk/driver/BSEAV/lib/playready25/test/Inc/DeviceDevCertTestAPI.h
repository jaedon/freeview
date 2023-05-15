/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DEVICEDEVCERTTESTAPI_H
#define __DEVICEDEVCERTTESTAPI_H

#include <PKTestNameSpace.h>

ENTER_PKTEST_NAMESPACE

/* global data buffer */
#define GDATA_COUNT        2
#define GDATA_TOTAL_COUNT (GDATA_COUNT + 1)
#define GDATA_DEVCERT      GDATA_COUNT

extern  DRM_STRING         g_dstrData_DDC[GDATA_TOTAL_COUNT];
extern  DRM_BYTE          *g_pbData_DDC[GDATA_TOTAL_COUNT];
extern  DRM_DWORD          g_cbData_DDC[GDATA_TOTAL_COUNT];


DRM_RESULT DRM_CALL Test_SetRobustnessVersion(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetDeviceEnv(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_InitWithOemData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_GetDeviceCertificate(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_GetBinaryDeviceCertificate(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_CompareXMLDataBuffer(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_CompareBinaryDataBuffer(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_VerifyXMLDevcert(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_VerifyXMLDevcert_ModelInfo(long argc, __in_ecount(argc) char **argv);


DECLARE_MODULE_DEFS( DeviceDevCert )

EXIT_PKTEST_NAMESPACE


#endif /* __DEVICEDEVCERTTESTAPI_H */
