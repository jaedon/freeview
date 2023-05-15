/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmactivation.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_ACTIVATION_IsActivationSupported( DRM_VOID )
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_ACTIVATION_IsActivationUnsupported( DRM_VOID )
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;