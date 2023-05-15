/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __POLICYSTATETESTAPI_H
#define __POLICYSTATETESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE
 
DRM_RESULT DRM_CALL TestPolicyGetStateData( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPolicySetStateData( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( PolicyState )

EXIT_PKTEST_NAMESPACE

#endif /* __POLICYSTATETESTAPI_H */
