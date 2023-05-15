/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <SecStoreTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("TestSecStore")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(SecStore)                            
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS                           
        PKTEST_MODULE_ENTRY(SecStore)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN

