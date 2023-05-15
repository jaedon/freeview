/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>
#include <functionaltest.h>
#include <drmwmdrmnet.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("TestRevocationPlayReadyPK")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(Revocation)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(DrmManager)
        PKTEST_MODULE_ENTRY(NDTTestSuite)                   
    END_TEST_MODULE_LIST
END_TEST_MAIN
