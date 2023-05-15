/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcleanstore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_CLEANSTORE_IsCleanStoreSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_CLEANSTORE_IsCleanStoreUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

