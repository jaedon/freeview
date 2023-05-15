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

typedef struct tagTEST_OEM_HAL_KEY_TYPE_MAP_ENTRY
{
    OEM_HAL_KEY_TYPE  eType;
    DRM_CHAR         *pszType;
} TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY;

#define CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY(type) { type, #type }

DRM_RESULT DRM_CALL GetOemKeyMapDataByName( __in const DRM_CHAR *f_pszType, __out TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY **f_ppMapEntry );
DRM_RESULT DRM_CALL GetOemKeyMapDataByType( __in OEM_HAL_KEY_TYPE f_eType, __out TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY **f_ppMapEntry );

extern TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY g_rgOemKeyTypes[];

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


/* OEM HAL test functions */
DRM_RESULT DRM_CALL TestOEMHalAllocateKey(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( OEM )

EXIT_PKTEST_NAMESPACE

#endif /* __OEMTEST_H */
