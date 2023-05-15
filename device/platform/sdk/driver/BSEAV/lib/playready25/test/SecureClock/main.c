/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <SecClockTestAPI.h>
#include <drmsecureclock.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("secclock", DRM_CLK_IsSecureClockSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(SecClock)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(SecClock)
    END_TEST_MODULE_LIST
END_TEST_MAIN
