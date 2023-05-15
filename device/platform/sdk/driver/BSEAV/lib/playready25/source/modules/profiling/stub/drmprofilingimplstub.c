/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmprofile.h>
#include <drmerr.h>

#if DRM_SUPPORT_PROFILING
ENTER_PK_NAMESPACE;
DRM_RESULT Perf_Enter_Scope( PERF_SCOPE_CONTEXT * pScopeContext ) { return DRM_SUCCESS; }
DRM_RESULT Perf_Leave_Scope( PERF_SCOPE_CONTEXT * pScopeContext ) { return DRM_SUCCESS; }
EXIT_PK_NAMESPACE;
#endif /* DRM_SUPPORT_PROFILING */

