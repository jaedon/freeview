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

const                DRM_CHAR       g_rgchTagCDATA                               [] = "CDATA";
#define g_rgchTagCDATA_LEN 5
const                DRM_CHAR       g_rgchTagCDATAOpenBracket                    [] = "CDATA[";
#define g_rgchTagCDATAOpenBracket_LEN 6


const                DRM_WCHAR      g_rgwchTagCDATA                              [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagCDATAOpenBracket                   [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('[', '\0'),  ONE_WCHAR('\0', '\0')};


const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagCDATA                             = CREATE_DRM_ANSI_STRING_EX( g_rgchTagCDATA, g_rgchTagCDATA_LEN );
const DRM_DISCARDABLE DRM_ANSI_CONST_STRING g_dastrTagCDATAOpenBracket                  = CREATE_DRM_ANSI_STRING_EX( g_rgchTagCDATAOpenBracket, g_rgchTagCDATAOpenBracket_LEN );


const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCDATA                              = CREATE_DRM_STRING( g_rgwchTagCDATA );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCDATAOpenBracket                   = CREATE_DRM_STRING( g_rgwchTagCDATAOpenBracket );


EXIT_PK_NAMESPACE_CODE;

