/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmxmlparserconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_CHAR       g_rgchTagCDATA                               [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagCDATA_LEN 5
DRM_STR_CONST                DRM_CHAR       g_rgchTagCDATAOpenBracket                    [] = { INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('['), INIT_CHAR_OBFUS('\0')};
#define g_rgchTagCDATAOpenBracket_LEN 6


DRM_STR_CONST                DRM_WCHAR      g_rgwchTagCDATA                              [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagCDATAOpenBracket                   [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('['),  INIT_WCHAR_OBFUS('\0')};


const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagCDATA                             = CREATE_DRM_ANSI_STRING_EX( g_rgchTagCDATA, g_rgchTagCDATA_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagCDATAOpenBracket                  = CREATE_DRM_ANSI_STRING_EX( g_rgchTagCDATAOpenBracket, g_rgchTagCDATAOpenBracket_LEN );


const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCDATA                              = CREATE_DRM_STRING( g_rgwchTagCDATA );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCDATAOpenBracket                   = CREATE_DRM_STRING( g_rgwchTagCDATAOpenBracket );


EXIT_PK_NAMESPACE_CODE;

