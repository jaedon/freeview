/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmantirollbackclockconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchDRM_LS_OLDSAVETIME_ATTR               [] =  { INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('v'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('\0') };  /* L"OldSaveTime"; */

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_OLDSAVETIME_ATTR               = CREATE_DRM_STRING( g_rgwchDRM_LS_OLDSAVETIME_ATTR );

EXIT_PK_NAMESPACE_CODE;

