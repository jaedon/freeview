/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmetercertrevocation.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_METERCERTREVOCATION_IsMeterCertRevocationSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_METERCERTREVOCATION_IsMeterCertRevocationUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

