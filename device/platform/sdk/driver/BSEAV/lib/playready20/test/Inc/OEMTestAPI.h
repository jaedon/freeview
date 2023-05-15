/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __OEMTEST_H
#define __OEMTEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/* globals */
extern OEM_FILEHDL    g_hHDSFileHandle;


DRM_RESULT DRM_CALL TestOEMSystemTimeFileTimeConversion(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEM_SECURE_ZERO_MEMORY(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEMStringCchCopyN(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEMGetFileSize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEMBlockHeaderCache(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEMClearBlockHeaderCache(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEMMemRealloc(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestOEMDeviceVerifyNonFakeId(long argc, __in_ecount(argc) char **argv);


DECLARE_MODULE_DEFS( OEM )

EXIT_PKTEST_NAMESPACE

#endif /* __OEMTEST_H */
