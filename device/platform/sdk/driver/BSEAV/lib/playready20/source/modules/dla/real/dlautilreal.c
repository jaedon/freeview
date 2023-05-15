/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdla.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_DLA_IsDLASupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_DLA_IsDLAUnsupported(DRM_VOID)
{
    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;

