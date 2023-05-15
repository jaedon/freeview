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
#include <functionaltest.h>
#include <drmdevstore.h>
#include <DrmManagerTestAPI.h>
#include <MeteringTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestSuiteNDT", DRM_DVS_IsDeviceStoreSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestSuiteNDTCocktailEncryption", DRM_DVS_IsDeviceStoreSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDevStore", DRM_DVS_IsDeviceStoreSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestSuiteNDTInternal", DRM_DVS_IsDeviceStoreSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(NDTTestSuite)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(NDTTestSuite)
        PKTEST_MODULE_ENTRY(DrmManager)
        PKTEST_MODULE_ENTRY(Metering)
    END_TEST_MODULE_LIST
END_TEST_MAIN

