/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmxmlhash.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_XMLHASH_IsXmlHashSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_XMLHASH_IsXmlHashUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

