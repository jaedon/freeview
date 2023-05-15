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

const                DRM_CHAR  g_rgchOpenEndTag                [] = "</";
#define g_rgchOpenEndTag_LEN 2
const                DRM_CHAR  g_rgchOpenCDATATag              [] = "<![CDATA[";
#define g_rgchOpenCDATATag_LEN 9
const                DRM_CHAR  g_rgchCloseCDATATag             [] = "]]>";
#define g_rgchCloseCDATATag_LEN 3


const                DRM_WCHAR      g_rgwchOpenTag                               [] =  { ONE_WCHAR('<', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchSpace                                 [] =  { ONE_WCHAR(' ', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchOpenEndTag                            [] =  { ONE_WCHAR('<', '\0'),  ONE_WCHAR('/', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchCloseTag                              [] =  { ONE_WCHAR('>', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchEqualQuote                            [] =  { ONE_WCHAR('=', '\0'),  ONE_WCHAR('\"', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchQuote                                 [] =  { ONE_WCHAR('\"', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchOpenCDATATag                          [] =  { ONE_WCHAR('<', '\0'),  ONE_WCHAR('!', '\0'),  ONE_WCHAR('[', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('[', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchCloseCDATATag                         [] =  { ONE_WCHAR(']', '\0'),  ONE_WCHAR(']', '\0'),  ONE_WCHAR('>', '\0'),  ONE_WCHAR('\0', '\0')};


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

