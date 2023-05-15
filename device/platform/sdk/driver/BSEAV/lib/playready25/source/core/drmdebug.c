/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMDEBUG_C
#include <drmdebug.h>
#include <drmlicense.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_VOID drmtype_asserts(void)
{
    DRMSIZEASSERT(sizeof(DRM_BYTE),   1);
    DRMSIZEASSERT(sizeof(DRM_CHAR),   1);
    DRMSIZEASSERT(sizeof(DRM_WCHAR),  2);
#if SIXTEEN_BIT_ADDRESSING
    DRMSIZEASSERT(sizeof (DRM_SHORT), 1);
    DRMSIZEASSERT(sizeof (DRM_WORD),  1);
    DRMSIZEASSERT(sizeof (DRM_LONG),  2);
    DRMSIZEASSERT(sizeof (DRM_DWORD), 2);
    DRMSIZEASSERT(sizeof (DRM_INT64), 4);
#else
    DRMSIZEASSERT(sizeof (DRM_SHORT), 2);
    DRMSIZEASSERT(sizeof (DRM_WORD),  2);
    DRMSIZEASSERT(sizeof (DRM_LONG),  4);
    DRMSIZEASSERT(sizeof (DRM_DWORD), 4);
    DRMSIZEASSERT(sizeof (DRM_INT64), 8);
#endif

    /*
    ** Device cert and licenses will be staged to share the same buffer
    */
    DRMSIZEASSERT(MAX_DEVICE_CERT_SIZE, DRM_MAX_LICENSESIZE);
}

EXIT_PK_NAMESPACE_CODE;

