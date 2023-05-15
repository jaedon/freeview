/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmetering.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_METERING_IsMeteringSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_METERING_IsMeteringUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

