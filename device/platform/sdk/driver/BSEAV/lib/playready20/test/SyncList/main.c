/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <SyncListTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("Testsynclist")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(SyncList)                            
    BEGIN_TEST_MODULE_LIST                           
        PKTEST_MODULE_ENTRY(SyncList)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN

