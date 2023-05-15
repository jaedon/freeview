/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmlicense.h"
#include "drmheaderparser.h"
#include "drmhdrbuilder.h"
#include "drmplayreadyobj.h"
#include <HeaderTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("header")
#if TARGET_LITTLE_ENDIAN
    TEST_SCRIPT_ENTRY("header_littleendian")
#endif
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(Header)
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY(Header)
    END_TEST_MODULE_LIST
END_TEST_MAIN

