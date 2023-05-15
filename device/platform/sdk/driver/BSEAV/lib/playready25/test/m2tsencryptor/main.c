/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <drmm2tsencryptor.h>
#include <M2TSEncryptorTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY( "m2tsencryptor" )
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN( M2TSENCRYPTOR )
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY( M2TSENCRYPTOR )
    END_TEST_MODULE_LIST
END_TEST_MAIN

