/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmhdrbuilderconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchAttributeHeaderXmlnsName              [] =  { INIT_WCHAR_OBFUS('x'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('n'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('\0') };
/* "http://schemas.microsoft.com/DRM/2007/03/PlayReadyHeader" */
DRM_STR_CONST                DRM_WCHAR      g_rgwchAttributeHeaderXmlnsValue             [] =  { INIT_WCHAR_OBFUS('h'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('p'),  INIT_WCHAR_OBFUS(':'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('c'),  INIT_WCHAR_OBFUS('h'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('i'),  INIT_WCHAR_OBFUS('c'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('s'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('f'),  INIT_WCHAR_OBFUS('t'),  INIT_WCHAR_OBFUS('.'),  INIT_WCHAR_OBFUS('c'),  INIT_WCHAR_OBFUS('o'),  INIT_WCHAR_OBFUS('m'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('2'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('7'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('0'),  INIT_WCHAR_OBFUS('3'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('l'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('y'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('a'),  INIT_WCHAR_OBFUS('d'),  INIT_WCHAR_OBFUS('e'),  INIT_WCHAR_OBFUS('r'),  INIT_WCHAR_OBFUS('\0')  };

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeHeaderXmlnsName              = CREATE_DRM_STRING( g_rgwchAttributeHeaderXmlnsName );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrAttributeHeaderXmlnsValue             = CREATE_DRM_STRING( g_rgwchAttributeHeaderXmlnsValue );

EXIT_PK_NAMESPACE_CODE;

