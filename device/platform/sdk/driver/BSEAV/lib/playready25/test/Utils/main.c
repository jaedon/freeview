/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmbase64.h>
#include <drmcontract.h>
#include <UtilTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("UtilTestCore")
    TEST_SCRIPT_ENTRY("utiltest")
    TEST_SCRIPT_ENTRY_CONDITIONAL("ErrorContractTest", DRM_CONTRACT_IsContractSupported)
    
#if WINCE_TEST
    TEST_SCRIPT_ENTRY("utiltestCE")
#endif
    
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(UtilTest)
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY(UtilTest)
    END_TEST_MODULE_LIST
END_TEST_MAIN

