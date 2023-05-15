/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <KeyFileTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("KeyFile") 
    TEST_SCRIPT_ENTRY("KeyFile_BB") 
END_TEST_SCRIPT_LIST  

BEGIN_TEST_MAIN(KeyFile) 
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY(KeyFile) 
    END_TEST_MODULE_LIST 
END_TEST_MAIN
