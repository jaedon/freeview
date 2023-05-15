/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmthumbnail.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_THUMBNAIL_IsThumbnailSupported(DRM_VOID)
{
    CLAW_AUTO_RANDOM_CIPHER
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_THUMBNAIL_IsThumbnailUnsupported(DRM_VOID)
{
    return TRUE;
}

EXIT_PK_NAMESPACE_CODE;

