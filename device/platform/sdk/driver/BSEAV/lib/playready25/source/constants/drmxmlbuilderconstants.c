/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmxmlbuilderconstants.h>
#include <drmbytemanip.h>


ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_CHAR  g_rgchOpenEndTag                [] = { INIT_CHAR_OBFUS('<'), INIT_CHAR_OBFUS('/'), INIT_CHAR_OBFUS('\0')};
#define g_rgchOpenEndTag_LEN 2
DRM_STR_CONST                DRM_CHAR  g_rgchOpenCDATATag              [] = { INIT_CHAR_OBFUS('<'), INIT_CHAR_OBFUS('!'), INIT_CHAR_OBFUS('['), INIT_CHAR_OBFUS('C'), INIT_CHAR_OBFUS('D'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('T'), INIT_CHAR_OBFUS('A'), INIT_CHAR_OBFUS('['), INIT_CHAR_OBFUS('\0')};
#define g_rgchOpenCDATATag_LEN 9
DRM_STR_CONST                DRM_CHAR  g_rgchCloseCDATATag             [] = { INIT_CHAR_OBFUS(']'), INIT_CHAR_OBFUS(']'), INIT_CHAR_OBFUS('>'), INIT_CHAR_OBFUS('\0')};
#define g_rgchCloseCDATATag_LEN 3


DRM_STR_CONST                DRM_WCHAR      g_rgwchOpenTag                               [] =  { INIT_WCHAR_OBFUS('<'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchSpace                                 [] =  { INIT_WCHAR_OBFUS(' '),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchOpenEndTag                            [] =  { INIT_WCHAR_OBFUS('<'),  INIT_WCHAR_OBFUS('/'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchCloseTag                              [] =  { INIT_WCHAR_OBFUS('>'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchEqualQuote                            [] =  { INIT_WCHAR_OBFUS('='),  INIT_WCHAR_OBFUS('\"'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchQuote                                 [] =  { INIT_WCHAR_OBFUS('\"'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchOpenCDATATag                          [] =  { INIT_WCHAR_OBFUS('<'),  INIT_WCHAR_OBFUS('!'),  INIT_WCHAR_OBFUS('['),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('['),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchCloseCDATATag                         [] =  { INIT_WCHAR_OBFUS(']'),  INIT_WCHAR_OBFUS(']'),  INIT_WCHAR_OBFUS('>'),  INIT_WCHAR_OBFUS('\0')};


const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrOpenEndTag                           = CREATE_DRM_ANSI_STRING_EX( g_rgchOpenEndTag, g_rgchOpenEndTag_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrOpenCDATATag                         = CREATE_DRM_ANSI_STRING_EX( g_rgchOpenCDATATag, g_rgchOpenCDATATag_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrCloseCDATATag                        = CREATE_DRM_ANSI_STRING_EX( g_rgchCloseCDATATag, g_rgchCloseCDATATag_LEN );


const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenTag                               = CREATE_DRM_STRING( g_rgwchOpenTag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSpace                                 = CREATE_DRM_STRING( g_rgwchSpace );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenEndTag                            = CREATE_DRM_STRING( g_rgwchOpenEndTag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrCloseTag                              = CREATE_DRM_STRING( g_rgwchCloseTag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrEqualQuote                            = CREATE_DRM_STRING( g_rgwchEqualQuote );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrQuote                                 = CREATE_DRM_STRING( g_rgwchQuote );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenCDATATag                          = CREATE_DRM_STRING( g_rgwchOpenCDATATag );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrCloseCDATATag                         = CREATE_DRM_STRING( g_rgwchCloseCDATATag );



EXIT_PK_NAMESPACE_CODE;

