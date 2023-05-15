/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmodelrevocation.h>

ENTER_PK_NAMESPACE_CODE;

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL DRM_MODELREVOCATION_IsModelRevocationSupported(DRM_VOID)
{
    CLAW_AUTO_RANDOM_CIPHER
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_MODELREVOCATION_IsModelRevocationUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;