/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <prndformattest.h>
#include <prndprotocoltest.h>
#include <prndtoplevelapitest.h>
#include <prndnoassettxtest.h>
#include <drmprnd.h>
#include <DRMManagerTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL( "prndformattest",            DRM_PRND_IsPRNDSupported )
    TEST_SCRIPT_ENTRY_CONDITIONAL( "prndprotocoltest",          DRM_PRND_IsPRNDTransmitterSupported )
    TEST_SCRIPT_ENTRY_CONDITIONAL( "prndtoplevelapitest",       DRM_PRND_IsPRNDTransmitterSupported )
    TEST_SCRIPT_ENTRY_CONDITIONAL( "prndnoassettxtest",         DRM_PRND_IsPRNDReceiverSupported )
    TEST_SCRIPT_ENTRY_CONDITIONAL( "prndprotocoltest_internal", DRM_PRND_IsPRNDSupported )
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(prndtest)
BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY( prndformattest )
        PKTEST_MODULE_ENTRY( prndprotocoltest )
        PKTEST_MODULE_ENTRY( prndtoplevelapitest )
        PKTEST_MODULE_ENTRY( prndnoassettxtest )
        PKTEST_MODULE_ENTRY( DrmManager )
    END_TEST_MODULE_LIST
END_TEST_MAIN

