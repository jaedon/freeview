/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <ctype.h>
#include <drmxmlbuildera.h>
#include <XMLBuilderATestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("DailyTestXMLBuilderA")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(XMLBuilderA)                            
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS                           
        PKTEST_MODULE_ENTRY(XMLBuilderA)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN

