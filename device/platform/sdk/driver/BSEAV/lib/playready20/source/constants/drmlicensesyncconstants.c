/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmlicensesyncconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_WCHAR      g_rgwchTagSyncList                           [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSyncList                           = CREATE_DRM_STRING( g_rgwchTagSyncList );

EXIT_PK_NAMESPACE_CODE;

