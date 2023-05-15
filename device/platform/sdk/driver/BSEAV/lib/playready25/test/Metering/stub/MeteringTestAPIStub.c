/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <MeteringTestAPI.h>

/*
** Empty stub module for test areas that link against Metering.
** Since metering is a link time feature in Dev code, we need to be able to switch test
** inclusion at link time as well.
*/

ENTER_PKTEST_NAMESPACE_CODE

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE(Metering)

BEGIN_TEST_API_LIST(Metering)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
