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

const                DRM_WCHAR      g_rgwchXPathHeaderUplinks                    [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('\0', '\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathHeaderUplinks                    = CREATE_DRM_STRING( g_rgwchXPathHeaderUplinks );

EXIT_PK_NAMESPACE_CODE;

