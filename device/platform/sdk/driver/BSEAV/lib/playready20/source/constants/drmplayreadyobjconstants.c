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

const                DRM_WCHAR      g_rgwchOpenNodeWRMHeaderV4                   [] =  { ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'W', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ),
                                                                                         ONE_WCHAR( 'H', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'D', '\0' ),
                                                                                         ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( ' ', '\0' ), ONE_WCHAR( 'x', '\0' ),
                                                                                         ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( '=', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( 'h', '\0' ), ONE_WCHAR( 't', '\0' ),
                                                                                         ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( ':', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 'c', '\0' ), ONE_WCHAR( 'h', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                                                                         ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 'o', '\0' ),
                                                                                         ONE_WCHAR( 'f', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                                                                         ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'D', '\0' ),
                                                                                         ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( '2', '\0' ),
                                                                                         ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '7', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '3', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ),
                                                                                         ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'R', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'y', '\0' ),
                                                                                         ONE_WCHAR( 'H', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( ' ', '\0' ),
                                                                                         ONE_WCHAR( 'v', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( '=', '\0' ),
                                                                                         ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '4', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ),
                                                                                         ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ),
                                                                                         ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchOpenNodeWRMHeaderV4_1                 [] =  { ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'W', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ),
                                                                                         ONE_WCHAR( 'H', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'A', '\0' ), ONE_WCHAR( 'D', '\0' ),
                                                                                         ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( ' ', '\0' ), ONE_WCHAR( 'x', '\0' ),
                                                                                         ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( '=', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( 'h', '\0' ), ONE_WCHAR( 't', '\0' ),
                                                                                         ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( ':', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 'c', '\0' ), ONE_WCHAR( 'h', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                                                                         ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( 'o', '\0' ),
                                                                                         ONE_WCHAR( 'f', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                                                                         ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'D', '\0' ),
                                                                                         ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( '2', '\0' ),
                                                                                         ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '7', '\0' ), ONE_WCHAR( '/', '\0' ),
                                                                                         ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '3', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ),
                                                                                         ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'R', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'y', '\0' ),
                                                                                         ONE_WCHAR( 'H', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'd', '\0' ),
                                                                                         ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( '"', '\0' ), ONE_WCHAR( ' ', '\0' ),
                                                                                         ONE_WCHAR( 'v', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 's', '\0' ),
                                                                                         ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( '=', '\0' ),
                                                                                         ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '4', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '1', '\0' ),
                                                                                         ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ),
                                                                                         ONE_WCHAR( '"', '\0' ), ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagV2                                 [] =  { ONE_WCHAR( '2', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagV2_4                               [] =  { ONE_WCHAR( '2', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '4', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagV4                                 [] =  { ONE_WCHAR( '4', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagV4_1                               [] =  { ONE_WCHAR( '4', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' ) };
const                DRM_WCHAR      g_rgwchTagContentXML                         [] =  { ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'O', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'N', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'X', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'L', '\0' ), ONE_WCHAR( '\0', '\0' ) };

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenNodeWRMHeaderV4                   = CREATE_DRM_STRING( g_rgwchOpenNodeWRMHeaderV4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrOpenNodeWRMHeaderV4_1                 = CREATE_DRM_STRING( g_rgwchOpenNodeWRMHeaderV4_1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV2                                 = CREATE_DRM_STRING( g_rgwchTagV2 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV2_4                               = CREATE_DRM_STRING( g_rgwchTagV2_4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV4                                 = CREATE_DRM_STRING( g_rgwchTagV4 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV4_1                               = CREATE_DRM_STRING( g_rgwchTagV4_1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagContentXML                         = CREATE_DRM_STRING( g_rgwchTagContentXML );

EXIT_PK_NAMESPACE_CODE;

