/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifdef MEMORY_LEAK_DEBUG
#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdafx.h>
#include <testSuiteNDT.h>
#include <drmdevstore.h>
#include <drmwmdrmnet.h>
#include <DRMManagerTestAPI.h>
#include <MeteringTestAPI.h>

static DRM_BOOL DRM_CALL _AreDeviceStoreAndWmdrmnetSupported( DRM_VOID )
{
    return DRM_DVS_IsDeviceStoreSupported() && DRM_WMDRMNET_IsWmdrmnetSupported();
}

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestSuiteNDT",                   _AreDeviceStoreAndWmdrmnetSupported)

#if !DRM_SUPPORT_SECUREOEMHAL
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestSuiteNDTCocktailEncryption", _AreDeviceStoreAndWmdrmnetSupported)
#endif /* DRM_SUPPORT_SECUREOEMHAL */

    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDevStore",                   _AreDeviceStoreAndWmdrmnetSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestSuiteNDTInternal",           _AreDeviceStoreAndWmdrmnetSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(NDTTestSuite)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(NDTTestSuite)
        PKTEST_MODULE_ENTRY(DrmManager)
        PKTEST_MODULE_ENTRY(Metering)
    END_TEST_MODULE_LIST
END_TEST_MAIN

