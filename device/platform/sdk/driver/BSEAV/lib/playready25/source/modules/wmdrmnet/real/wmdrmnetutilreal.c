/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_WMDRMNETUTILREAL_C
#include <drmwmdrmnet.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_WMDRMNET_IsWmdrmnetSupported(DRM_VOID) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_WMDRMNET_IsWmdrmnetSupported(DRM_VOID)
{
    CLAW_AUTO_RANDOM_CIPHER
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRMNET_IsWmdrmnetUnsupported(DRM_VOID)
{
    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;

