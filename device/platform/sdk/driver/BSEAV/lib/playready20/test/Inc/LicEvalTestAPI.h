/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __LICEVALTESTAPI_H
#define __LICEVALTESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL TestLicEvalContextSetting                                      ( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestLicenseEvalPerformOperations                               ( long argc, __in_ecount( argc ) char **argv );
 
DECLARE_MODULE_DEFS(LicEval)

EXIT_PKTEST_NAMESPACE

#endif /* __LICEVALTESTAPI_H */
