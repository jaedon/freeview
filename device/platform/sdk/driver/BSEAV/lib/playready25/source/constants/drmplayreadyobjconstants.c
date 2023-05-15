/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmplayreadyobjconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchOpenNodeWRMHeaderV4                   [] =  { INIT_WCHAR_OBFUS( '<'), INIT_WCHAR_OBFUS( 'W'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'),
                                                                                         INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'D'),
                                                                                         INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( ' '), INIT_WCHAR_OBFUS( 'x'),
                                                                                         INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( '='), INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( 'h'), INIT_WCHAR_OBFUS( 't'),
                                                                                         INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'p'), INIT_WCHAR_OBFUS( ':'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'h'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'c'),
                                                                                         INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'o'),
                                                                                         INIT_WCHAR_OBFUS( 'f'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( 'c'),
                                                                                         INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 'D'),
                                                                                         INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( '2'),
                                                                                         INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '7'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '3'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 'P'),
                                                                                         INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'y'), INIT_WCHAR_OBFUS( 'R'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'), INIT_WCHAR_OBFUS( 'y'),
                                                                                         INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( ' '),
                                                                                         INIT_WCHAR_OBFUS( 'v'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( '='),
                                                                                         INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( '4'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'),
                                                                                         INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'),
                                                                                         INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( '>'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchOpenNodeWRMHeaderV4_1                 [] =  { INIT_WCHAR_OBFUS( '<'), INIT_WCHAR_OBFUS( 'W'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'),
                                                                                         INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'A'), INIT_WCHAR_OBFUS( 'D'),
                                                                                         INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( ' '), INIT_WCHAR_OBFUS( 'x'),
                                                                                         INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( '='), INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( 'h'), INIT_WCHAR_OBFUS( 't'),
                                                                                         INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( 'p'), INIT_WCHAR_OBFUS( ':'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'c'), INIT_WCHAR_OBFUS( 'h'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'c'),
                                                                                         INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 's'), INIT_WCHAR_OBFUS( 'o'),
                                                                                         INIT_WCHAR_OBFUS( 'f'), INIT_WCHAR_OBFUS( 't'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( 'c'),
                                                                                         INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'm'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 'D'),
                                                                                         INIT_WCHAR_OBFUS( 'R'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( '2'),
                                                                                         INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '7'), INIT_WCHAR_OBFUS( '/'),
                                                                                         INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '3'), INIT_WCHAR_OBFUS( '/'), INIT_WCHAR_OBFUS( 'P'),
                                                                                         INIT_WCHAR_OBFUS( 'l'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'y'), INIT_WCHAR_OBFUS( 'R'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'), INIT_WCHAR_OBFUS( 'y'),
                                                                                         INIT_WCHAR_OBFUS( 'H'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'a'), INIT_WCHAR_OBFUS( 'd'),
                                                                                         INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( ' '),
                                                                                         INIT_WCHAR_OBFUS( 'v'), INIT_WCHAR_OBFUS( 'e'), INIT_WCHAR_OBFUS( 'r'), INIT_WCHAR_OBFUS( 's'),
                                                                                         INIT_WCHAR_OBFUS( 'i'), INIT_WCHAR_OBFUS( 'o'), INIT_WCHAR_OBFUS( 'n'), INIT_WCHAR_OBFUS( '='),
                                                                                         INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( '4'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '1'),
                                                                                         INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'),
                                                                                         INIT_WCHAR_OBFUS( '"'), INIT_WCHAR_OBFUS( '>'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagV2                                 [] =  { INIT_WCHAR_OBFUS( '2'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagV2_4                               [] =  { INIT_WCHAR_OBFUS( '2'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '4'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagV4                                 [] =  { INIT_WCHAR_OBFUS( '4'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagV4_1                               [] =  { INIT_WCHAR_OBFUS( '4'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '1'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '.'), INIT_WCHAR_OBFUS( '0'), INIT_WCHAR_OBFUS( '\0') };
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagContentXML                         [] =  { INIT_WCHAR_OBFUS( 'C'), INIT_WCHAR_OBFUS( 'O'), INIT_WCHAR_OBFUS( 'N'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'E'), INIT_WCHAR_OBFUS( 'N'), INIT_WCHAR_OBFUS( 'T'), INIT_WCHAR_OBFUS( 'X'), INIT_WCHAR_OBFUS( 'M'), INIT_WCHAR_OBFUS( 'L'), INIT_WCHAR_OBFUS( '\0') };

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenNodeWRMHeaderV4                   = CREATE_DRM_STRING( g_rgwchOpenNodeWRMHeaderV4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenNodeWRMHeaderV4_1                 = CREATE_DRM_STRING( g_rgwchOpenNodeWRMHeaderV4_1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV2                                 = CREATE_DRM_STRING( g_rgwchTagV2 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV2_4                               = CREATE_DRM_STRING( g_rgwchTagV2_4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV4                                 = CREATE_DRM_STRING( g_rgwchTagV4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV4_1                               = CREATE_DRM_STRING( g_rgwchTagV4_1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagContentXML                         = CREATE_DRM_STRING( g_rgwchTagContentXML );

EXIT_PK_NAMESPACE_CODE;

