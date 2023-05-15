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

DRM_STR_CONST                DRM_WCHAR      g_rgwchTagSyncList                           [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagSyncList                           = CREATE_DRM_STRING( g_rgwchTagSyncList );

EXIT_PK_NAMESPACE_CODE;

