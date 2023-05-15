/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdeviceassets.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_DEVICEASSETS_IsDeviceAssetsSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_DEVICEASSETS_IsDeviceAssetsUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

