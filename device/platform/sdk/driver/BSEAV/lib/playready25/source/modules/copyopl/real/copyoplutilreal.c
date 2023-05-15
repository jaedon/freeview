/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_COPYOPLUTILREAL_C
#include <drmcopyopl.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_COPYOPL_IsCopyOplSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL DRM_COPYOPL_IsCopyOplUnsupported(DRM_VOID)
{
    return FALSE;
}

EXIT_PK_NAMESPACE_CODE;

