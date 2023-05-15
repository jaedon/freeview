/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <testNDR.h>
#include <drmdevstore.h>
#include <drmwmdrmnet.h>

static DRM_BOOL DRM_CALL _AreDeviceStoreAndWmdrmnetSupported( DRM_VOID )
{
    return DRM_DVS_IsDeviceStoreSupported() && DRM_WMDRMNET_IsWmdrmnetSupported();
}

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("FunctionalTest", _AreDeviceStoreAndWmdrmnetSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(NDRFunctionalTest)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(NDRFunctionalTest)
    END_TEST_MODULE_LIST
END_TEST_MAIN
