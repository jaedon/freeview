/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmheaderparserconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchXPathHeaderUplinks                    [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathHeaderUplinks                    = CREATE_DRM_STRING( g_rgwchXPathHeaderUplinks );

EXIT_PK_NAMESPACE_CODE;

