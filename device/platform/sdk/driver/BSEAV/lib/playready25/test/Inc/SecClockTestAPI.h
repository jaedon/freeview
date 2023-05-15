/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_SECCLOCKTEST_H_
#define _DRM_SECCLOCKTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** SecClock Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** SecClock Non-APIMAP Functions
** ---------------------------------------
*/

/*
** ---------------------------------------
** SecClock APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL Test_API_MGR_Initialize(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_MGR_ClkGenerateChallenge(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_MGR_ClkProcessResponse(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_API_MGR_GetSecureClock(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetChallengeState(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_SetSecureClock(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL Test_ChangeClock(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( SecClock )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_SECCLOCKTEST_H_ */
