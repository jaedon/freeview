/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <ctype.h>
#include <drmxmlparser.h>
#include <XMLParserTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("DailyTestXMLParser")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(XMLParser)                            
    BEGIN_TEST_MODULE_LIST                           
        PKTEST_MODULE_ENTRY(XMLParser)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN

