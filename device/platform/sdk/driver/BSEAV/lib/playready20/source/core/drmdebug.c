/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;

DRM_VOID drmtype_asserts(void)
{
    DRMSIZEASSERT(sizeof(DRM_BYTE), 1);
    DRMSIZEASSERT(sizeof(DRM_CHAR), 1);
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
}





EXIT_PK_NAMESPACE_CODE;
