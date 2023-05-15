/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SYMOPTUTILREAL_C
#include <drmsymopt.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_SECUREOEMHAL
#error DRM_SUPPORT_SECUREOEMHAL is set.  You can not implement a secure HAL that supports symmetric optimizations.  You must link to the stub version of the symopt module.
#endif /* DRM_SUPPORT_SECUREOEMHAL */

DRM_API DRM_BOOL DRM_CALL DRM_SYMOPT_IsSymOptSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_SYMOPT_IsSymOptUnsupported(DRM_VOID)
{
    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;

