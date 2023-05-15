/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <LiveTVTestAPI.h>
#include <DRMManagerTestAPI.h>
#include <HeaderTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY( "LiveTV" )
END_TEST_SCRIPT_LIST


BEGIN_TEST_MAIN( LiveTV )
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY( LiveTV )
        PKTEST_MODULE_ENTRY( PKCommon )
        PKTEST_MODULE_ENTRY( DrmManager )
        PKTEST_MODULE_ENTRY( Header )
    END_TEST_MODULE_LIST
END_TEST_MAIN
