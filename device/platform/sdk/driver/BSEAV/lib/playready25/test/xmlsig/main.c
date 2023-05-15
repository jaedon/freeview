/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmxmlparser.h>
#include <drmxmlbuildera.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <drmsoapxmlutility.h>
#include <XMLSigTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("xmlsignature")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(XMLSig)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(XMLSig)
    END_TEST_MODULE_LIST
END_TEST_MAIN

