/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmlicense.h>
#include <drmdevcert.h>
#include <drmxmlparser.h>
#include <drmwmdrm.h>
#include <DCParserTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL( "dcParser", DRM_WMDRM_IsWMDRMSupported )
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN( DCParser )
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY( DCParser )
    END_TEST_MODULE_LIST
END_TEST_MAIN
