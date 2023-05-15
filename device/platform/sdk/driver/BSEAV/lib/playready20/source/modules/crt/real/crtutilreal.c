/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcrt.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_CRT_IsCRTSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_CRT_IsCRTUnsupported(DRM_VOID)
{
    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;
