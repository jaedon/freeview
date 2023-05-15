/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_PERFORMANCE_TEST_API_H
#define __DRM_PERFORMANCE_TEST_API_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL TestPerfAllocateLogBuffer(long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPerfFreeLogBuffer(long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPerfDumpLogBuffer(long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPerfOn(long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPerfOff(long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPerfLogSeparator(long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( Performance )

EXIT_PKTEST_NAMESPACE

#endif

