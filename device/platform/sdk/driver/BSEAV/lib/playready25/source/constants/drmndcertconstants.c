/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmndcertconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchTagModulus                            [] =  { INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('u'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagExponent                           [] =  { INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'), INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagPrime0                             [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagPrime1                             [] =  { INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagCRTExponent0                       [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagCRTExponent1                       [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagIQMP                               [] =  { INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('Q'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchWMDRMCertExponent                     [] =  { INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('Q'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('\0') };

DRM_STR_CONST                DRM_CHAR  g_rgchTagUnsignedTemplate       [] = { INIT_CHAR_OBFUS('U'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('S'), INIT_CHAR_OBFUS('I'), INIT_CHAR_OBFUS('G'), INIT_CHAR_OBFUS('N'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('M'), INIT_CHAR_OBFUS('P'), INIT_CHAR_OBFUS('L'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('E'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagUnsignedTemplate_LEN 16
DRM_STR_CONST                DRM_CHAR  g_rgchSecurityVersion           [] = { INIT_CHAR_OBFUS('3'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('0'), INIT_CHAR_OBFUS('.'), INIT_CHAR_OBFUS('1'), INIT_CHAR_OBFUS('\0')};
#define g_rgchSecurityVersion_LEN 7

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagModulus                            = CREATE_DRM_STRING( g_rgwchTagModulus );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagExponent                           = CREATE_DRM_STRING( g_rgwchTagExponent );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagPrime0                             = CREATE_DRM_STRING( g_rgwchTagPrime0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagPrime1                             = CREATE_DRM_STRING( g_rgwchTagPrime1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCRTExponent0                       = CREATE_DRM_STRING( g_rgwchTagCRTExponent0 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCRTExponent1                       = CREATE_DRM_STRING( g_rgwchTagCRTExponent1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagIQMP                               = CREATE_DRM_STRING( g_rgwchTagIQMP );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrWMDRMCertExponent                     = CREATE_DRM_STRING( g_rgwchWMDRMCertExponent );

const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrTagUnsignedTemplate                 = CREATE_DRM_ANSI_STRING_EX( g_rgchTagUnsignedTemplate, g_rgchTagUnsignedTemplate_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING  g_dastrSecurityVersion                     = CREATE_DRM_ANSI_STRING_EX( g_rgchSecurityVersion, g_rgchSecurityVersion_LEN );

EXIT_PK_NAMESPACE_CODE;

