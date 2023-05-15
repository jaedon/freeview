/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#include "stdafx.h"
#include "oemcipher.h"
#include "drmblackbox.h"
#include "drmlicense.h"
#include <blackboxtest.h>
#include "oemplatform.h"
#include <drmactivation.h>

extern const char g_rgchguidMPRG[];
const char g_rgchguidMPRG[] = "4d505247-00000000-0000-0000-0000-999999999999";

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
    TEST_SCRIPT_ENTRY("refbb")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(BlackBox)
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS
        PKTEST_MODULE_ENTRY(BlackBox)
    END_TEST_MODULE_LIST
END_TEST_MAIN


