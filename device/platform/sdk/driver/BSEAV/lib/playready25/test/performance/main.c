/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <drmactivation.h>
#include <PerformanceTestAPI.h>
#include <LiveTVTestAPI.h>
#include <DRMManagerTestAPI.h>
#include <HeaderTestAPI.h>


#if DRM_TEST_SUPPORT_ACTIVATION
    #include <ActivationTestAPI.h>
#endif

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("Performance")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN( Performance )
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY( Performance )
        PKTEST_MODULE_ENTRY( PKCommon )
        PKTEST_MODULE_ENTRY( LiveTV )
        PKTEST_MODULE_ENTRY( DrmManager )
        PKTEST_MODULE_ENTRY( Header )
#if DRM_TEST_SUPPORT_ACTIVATION
        PKTEST_MODULE_ENTRY( Activation )
#endif
    END_TEST_MODULE_LIST
END_TEST_MAIN

