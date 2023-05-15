/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <LicEvalTestAPI.h>
#include <DRMManagerTestAPI.h>
#include <drmantirollbackclock.h>


IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("LicEval")
    TEST_SCRIPT_ENTRY_CONDITIONAL("LicEval_AntiRollback", DRM_ARCLK_IsAntirollbackClockSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(LicEval)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(LicEval)
        PKTEST_MODULE_ENTRY(DrmManager)
    END_TEST_MODULE_LIST
END_TEST_MAIN

