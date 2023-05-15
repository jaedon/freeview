/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_MATHSAFETEST_H
#define __DRM_MATHSAFETEST_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** Test Math Safe functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestMultiplication( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(mathsafetest)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_MATHSAFETEST_H */

