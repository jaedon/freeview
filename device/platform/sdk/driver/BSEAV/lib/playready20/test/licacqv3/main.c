/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include <LicAcqV3TestAPI.h>
#include <DrmManagerTestAPI.h>


IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST                               
    TEST_SCRIPT_ENTRY("licacqv3")
END_TEST_SCRIPT_LIST                                 

BEGIN_TEST_MAIN(LicAcqV3)                            
    BEGIN_TEST_MODULE_LIST                           
        PKTEST_MODULE_ENTRY(LicAcqV3)                 
        PKTEST_MODULE_ENTRY(DrmManager)
    END_TEST_MODULE_LIST                             
END_TEST_MAIN

