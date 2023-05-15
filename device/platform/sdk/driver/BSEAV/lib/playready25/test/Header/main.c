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

/*
** Since this test area needs the platform inited on some platforms but it doesn't use
** the default modules (such as PKCommon), call Oem_Platform_Init here rather than
** in Pre/Post test case so that we don't interfere with other test areas that use this
** module.
*/
static DRM_RESULT DRM_CALL WrapTestRun(long lTCID, DRM_BOOL fPreTestRun)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( fPreTestRun )
    {
        dr = Oem_Platform_Init();
    }
    else
    {
        dr = Oem_Platform_UnInit();
    }

    return dr;
}

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

