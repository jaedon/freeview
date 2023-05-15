/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <licgentest.h>
#include <drmprnd.h>
#include <drmlicgen.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("licgentest", DRM_LICGEN_IsLicGenSupported)
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(licgentest)
BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(licgentest)
    END_TEST_MODULE_LIST
END_TEST_MAIN

