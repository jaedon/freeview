/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <MeteringTestAPI.h>


IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST                               
    TEST_SCRIPT_ENTRY("metering")
    TEST_SCRIPT_ENTRY("metering_internal")
#if DRM_TEST_SUPPORT_NET_IO && !defined (WINCE)
    TEST_SCRIPT_ENTRY("metering_netio")
#endif /* DRM_TEST_SUPPORT_NET_IO */
END_TEST_SCRIPT_LIST                                 

BEGIN_TEST_MAIN(Metering)                            
    BEGIN_TEST_MODULE_LIST                           
        PKTEST_MODULE_ENTRY(Metering)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN

