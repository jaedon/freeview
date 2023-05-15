/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <functionaltest.h>
#include <drmdevstore.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("FunctionalTest", DRM_DVS_IsDeviceStoreSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(NDRFunctionalTest)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(NDRFunctionalTest)
    END_TEST_MODULE_LIST
END_TEST_MAIN
