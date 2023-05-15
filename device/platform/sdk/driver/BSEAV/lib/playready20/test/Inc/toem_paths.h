/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TOEM_PATHS_H_
#define _TOEM_PATHS_H_ 1

#include "drmcommon.h"
#include "tOEMImp.h"
#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_RESULT DRM_CALL TST_OEM_InitPaths();
DRM_RESULT DRM_CALL TST_OEM_GetTestDataDirectory( __inout_z DRM_CHAR f_pszDataDirectory[DRM_MAX_PATH] );
DRM_RESULT DRM_CALL TST_OEM_GetTestRunDirectory(  __inout_z DRM_CHAR f_pszResultDirectory[DRM_MAX_PATH] );

EXIT_PKTEST_NAMESPACE

#endif /* _TOEM_PATHS_H_ */