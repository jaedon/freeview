/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <ToolTestAPI.h>
#include <drmsecureclock.h>
#include <drmdla.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("ToolTest")
    TEST_SCRIPT_ENTRY_CONDITIONAL("ToolTestSecureClockOn", DRM_CLK_IsSecureClockSupported)
    TEST_SCRIPT_ENTRY("ToolTestInternal")
#if DRM_TEST_SUPPORT_NET_IO
    TEST_SCRIPT_ENTRY_CONDITIONAL("ToolTestDLA", DRM_DLA_IsDLASupported)
#endif /* DRM_TEST_SUPPORT_NET_IO */
#if DRM_SUPPORT_TRACING
#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_XBOX
#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_IOS
#if DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_WM7
    TEST_SCRIPT_ENTRY( "Tracing" )
#endif /* DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_WM7 */
#endif /* DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_IOS */
#endif /* DRM_BUILD_PROFILE != DRM_BUILD_PROFILE_XBOX */
#endif /* DRM_SUPPORT_TRACING */
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(ToolTest)
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY(ToolTest)
    END_TEST_MODULE_LIST
END_TEST_MAIN

