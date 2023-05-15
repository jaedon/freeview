/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <time.h>
#include <drmcommon.h>
#include <drmprofile.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_PROFILING

DRM_API DRM_UINT64 DRM_CALL Oem_Profiling_QueryPerformanceCounter( DRM_VOID )
{
    return DRM_UI64LITERAL( 0, clock() );
}

DRM_API DRM_UINT64 DRM_CALL Oem_Profiling_QueryPerformanceFrequency( DRM_VOID )
{
    return DRM_UI64LITERAL( 0, CLOCKS_PER_SEC );
}

#endif


EXIT_PK_NAMESPACE_CODE;
