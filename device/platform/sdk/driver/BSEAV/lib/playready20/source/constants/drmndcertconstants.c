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

const                DRM_WCHAR      g_rgwchTagModulus                            [] =  { ONE_WCHAR('M', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagExponent                           [] =  { ONE_WCHAR('E', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagPrime0                             [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagPrime1                             [] =  { ONE_WCHAR('P', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagCRTExponent0                       [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('0', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagCRTExponent1                       [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchTagIQMP                               [] =  { ONE_WCHAR('I', '\0'),  ONE_WCHAR('Q', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('\0', '\0') };
const                DRM_WCHAR      g_rgwchWMDRMCertExponent                     [] =  { ONE_WCHAR('A', '\0'),  ONE_WCHAR('Q', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('\0', '\0') };

const                DRM_CHAR  g_rgchTagUnsignedTemplate       [] = "UNSIGNEDTEMPLATE";
#define g_rgchTagUnsignedTemplate_LEN 16
const                DRM_CHAR  g_rgchSecurityVersion           [] = "3.0.0.1";
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

