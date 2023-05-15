/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_NETIO_TEST_API_H
#define __DRM_NETIO_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

#define CHALLENGE_URL_VAR       "SharedChallengeURL"

DRM_RESULT DRM_CALL TestSendChallenge( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(NETIO)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_NETIO_TEST_API_H*/
