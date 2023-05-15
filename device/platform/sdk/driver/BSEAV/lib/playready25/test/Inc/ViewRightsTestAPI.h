/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRM_VIEWRIGHTSTEST_H_
#define _DRM_VIEWRIGHTSTEST_H_

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** ViewRights Test Global Variable Names
** ---------------------------------------
*/

/*
** ---------------------------------------
** ViewRights Non-APIMAP Functions
** ---------------------------------------
*/

/*
** ---------------------------------------
** ViewRights APIMAP Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestGetLicenseAggregateData(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestIsAllowed(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestAddLicense(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestVerifyStateData(long argc, __in_ecount(argc) char **argv);

DECLARE_MODULE_DEFS( ViewRights )

EXIT_PKTEST_NAMESPACE

#endif /* _DRM_VIEWRIGHTSTEST_H_ */
