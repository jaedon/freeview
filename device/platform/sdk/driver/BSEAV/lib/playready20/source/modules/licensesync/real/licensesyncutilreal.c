/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsyncstore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_SNC_IsLicenseSyncSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_SNC_IsLicenseSyncUnsupported(DRM_VOID)
{
    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;
