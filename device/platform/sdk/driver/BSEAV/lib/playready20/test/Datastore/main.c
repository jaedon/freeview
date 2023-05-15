/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmdatastore.h"
#include <DatastoreTestAPI.h>

DRM_RESULT DRM_CALL WrapTestRun( long lTCID, DRM_BOOL fPreTestRun )
{
    return DRM_SUCCESS;
}

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY( "datastore" )
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN( DATASTORE )
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY( DATASTORE )
    END_TEST_MODULE_LIST
END_TEST_MAIN

