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

DRM_STR_CONST                DRM_WCHAR      g_rgwchXPathPrime0                           [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchXPathPrime1                           [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchXPathCRTExponent0                     [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchXPathCRTExponent1                     [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchXPathIQMP                             [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('/'), INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('Q'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('\0') };

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathPrime0                           = CREATE_DRM_STRING( g_rgwchXPathPrime0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathPrime1                           = CREATE_DRM_STRING( g_rgwchXPathPrime1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathCRTExponent0                     = CREATE_DRM_STRING( g_rgwchXPathCRTExponent0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathCRTExponent1                     = CREATE_DRM_STRING( g_rgwchXPathCRTExponent1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrXPathIQMP                             = CREATE_DRM_STRING( g_rgwchXPathIQMP );

EXIT_PK_NAMESPACE_CODE;

