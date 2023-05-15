/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmviewrightsconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_WCHAR      g_rgwchDRM_LS_EXPIRATION_AFTER_FIRSTUSE_ATTR [] =  { ONE_WCHAR('H', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_BURNTOCDCOUNT_ATTR             [] =  { ONE_WCHAR('B', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_BEGINBURNTOCDCOUNT_ATTR        [] =  { ONE_WCHAR('B', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDRM_LS_BEGINPLAYCOUNT_ATTR            [] =  { ONE_WCHAR('B', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_EXPIRATION_AFTER_FIRSTUSE_ATTR = CREATE_DRM_STRING( g_rgwchDRM_LS_EXPIRATION_AFTER_FIRSTUSE_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_BURNTOCDCOUNT_ATTR             = CREATE_DRM_STRING( g_rgwchDRM_LS_BURNTOCDCOUNT_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_BEGINBURNTOCDCOUNT_ATTR        = CREATE_DRM_STRING( g_rgwchDRM_LS_BEGINBURNTOCDCOUNT_ATTR );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDRM_LS_BEGINPLAYCOUNT_ATTR            = CREATE_DRM_STRING( g_rgwchDRM_LS_BEGINPLAYCOUNT_ATTR );

EXIT_PK_NAMESPACE_CODE;

