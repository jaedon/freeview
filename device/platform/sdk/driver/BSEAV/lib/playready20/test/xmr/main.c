/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <XmrTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("xmr")
END_TEST_SCRIPT_LIST

/*
** This module just uses the default modules
*/
BEGIN_TEST_MAIN(xmr)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(xmr)
    END_TEST_MODULE_LIST
END_TEST_MAIN
