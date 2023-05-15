/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrmnetconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_WCHAR      g_rgwchXPathPrime0                           [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchXPathPrime1                           [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('/', '\0'), ONE_WCHAR('P', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchXPathCRTExponent0                     [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('/', '\0'), ONE_WCHAR('C', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchXPathCRTExponent1                     [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('/', '\0'), ONE_WCHAR('C', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchXPathIQMP                             [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('/', '\0'), ONE_WCHAR('I', '\0'),  ONE_WCHAR('Q', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('\0', '\0') };

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathPrime0                           = CREATE_DRM_STRING( g_rgwchXPathPrime0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathPrime1                           = CREATE_DRM_STRING( g_rgwchXPathPrime1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathCRTExponent0                     = CREATE_DRM_STRING( g_rgwchXPathCRTExponent0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathCRTExponent1                     = CREATE_DRM_STRING( g_rgwchXPathCRTExponent1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathIQMP                             = CREATE_DRM_STRING( g_rgwchXPathIQMP );

EXIT_PK_NAMESPACE_CODE;

