/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmlicense.h>
#include <drmheaderparser.h>
#include <drmhdrbuilder.h>
#include <drmplayreadyobj.h>
#include <CoreEmbeddingTestAPI.h>
#include <DRMManagerTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("CoreEmbedding")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(CoreEmbedding)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(DrmManager)
        PKTEST_MODULE_ENTRY(CoreEmbedding)
    END_TEST_MODULE_LIST
END_TEST_MAIN

