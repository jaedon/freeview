/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <NonceStoreTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY( "noncestore" )
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN( NonceStore )
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY( NonceStore )
    END_TEST_MODULE_LIST
END_TEST_MAIN