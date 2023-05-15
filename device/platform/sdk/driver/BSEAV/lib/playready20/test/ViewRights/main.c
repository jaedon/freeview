/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmblackbox.h>
#include <oempkcrypto.h>
#include <drmheaderparser.h>
#include <drmlicacq.h>
#include <oemcipher.h>
#include <drmviewrights.h>
#include <drmrevocation.h>
#include <drmchain.h>
#include <drmdevcertacq.h>
#include <drmsecureclock.h>
#include <drmsyncstore.h>
#include <drmsecurestore.h>
#include <drmlicreason.h>
#include <ViewRightsTestAPI.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("ViewRights")
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(ViewRights)                            
    BEGIN_TEST_MODULE_LIST_NO_DEFAULTS                           
        PKTEST_MODULE_ENTRY(ViewRights)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN
