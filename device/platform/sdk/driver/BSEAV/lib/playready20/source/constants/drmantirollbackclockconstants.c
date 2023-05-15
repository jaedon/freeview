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

const                DRM_WCHAR      g_rgwchDRM_LS_OLDSAVETIME_ATTR               [] =  { ONE_WCHAR('O', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0') };  /* L"OldSaveTime"; */

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_OLDSAVETIME_ATTR               = CREATE_DRM_STRING( g_rgwchDRM_LS_OLDSAVETIME_ATTR );

EXIT_PK_NAMESPACE_CODE;

