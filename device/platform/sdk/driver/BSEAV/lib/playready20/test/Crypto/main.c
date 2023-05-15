/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <cryptotest.h>
#include <drmwmdrmnet.h>

IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST                               
    TEST_SCRIPT_ENTRY("crypto")
    /*restrict crypto_win to either WinCE or 64bit Windows, no ANSI*/
#if defined(_WIN32_WCE)
    TEST_SCRIPT_ENTRY("crypto_win")
#endif
    TEST_SCRIPT_ENTRY_CONDITIONAL("crypto_2048", DRM_WMDRMNET_IsWmdrmnetSupported)
END_TEST_SCRIPT_LIST                                 

BEGIN_TEST_MAIN(Crypto)                            
    BEGIN_TEST_MODULE_LIST                           
        PKTEST_MODULE_ENTRY(Crypto)                  
    END_TEST_MODULE_LIST                             
END_TEST_MAIN
