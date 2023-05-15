/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmhds.h"
#include <HDSTestAPI.h>

DRM_RESULT DRM_CALL WrapTestRun( long lTCID, DRM_BOOL fPreTestRun )
{
    return DRM_SUCCESS;
}

BEGIN_TEST_SCRIPT_LIST                               
    TEST_SCRIPT_ENTRY("DailyTestHDS")
    TEST_SCRIPT_ENTRY("NegativeTestHDS")
#if DRM_SUPPORT_MULTI_THREADING
    TEST_SCRIPT_ENTRY("NegativeMultiThreadingTestHDS")
#endif
    TEST_SCRIPT_ENTRY("InvalidArgumentTestHDS")
    TEST_SCRIPT_ENTRY("DailyTestHDS_noansi")    
END_TEST_SCRIPT_LIST                                 

BEGIN_TEST_MAIN(HDS)                            
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS                           
        PKTEST_MODULE_ENTRY(HDS)
        PKTEST_MODULE_ENTRY(PKCommon)
    END_TEST_MODULE_LIST                             
END_TEST_MAIN
