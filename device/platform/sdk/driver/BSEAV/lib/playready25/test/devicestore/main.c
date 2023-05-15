/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmmanager.h>
#include <drmdevstore.h>
#include <drmdevicestore.h>
#include <testdevicestore.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("DailyTestDeviceStore", DRM_DVS_IsDeviceStoreSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(DeviceStore)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(DeviceStore)
    END_TEST_MODULE_LIST
END_TEST_MAIN

