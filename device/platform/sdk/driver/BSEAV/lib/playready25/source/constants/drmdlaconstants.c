/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdlaconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchTagUplinks                            [] =  { INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagLicenseRequest                     [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Q'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagV1Challenge                        [] =  { INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('H'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchDefaultV1Data                         [] =  { INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagActionList                         [] =  { INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagAppSecurity                     [] =  { INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagSubjectID1                      [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('J'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('1'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagSubjectID2                      [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('J'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('2'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagDrmkVersion                     [] =  { INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagRevInfo                         [] =  { INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagClientInfo                      [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('F'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagClientID                        [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchReqTagClientVersion                   [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagUplinks                            = CREATE_DRM_STRING( g_rgwchTagUplinks );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLicenseRequest                     = CREATE_DRM_STRING( g_rgwchTagLicenseRequest );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagV1Challenge                        = CREATE_DRM_STRING( g_rgwchTagV1Challenge );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrDefaultV1Data                         = CREATE_DRM_STRING( g_rgwchDefaultV1Data );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagActionList                         = CREATE_DRM_STRING( g_rgwchTagActionList );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagAppSecurity                     = CREATE_DRM_STRING( g_rgwchReqTagAppSecurity );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagSubjectID1                      = CREATE_DRM_STRING( g_rgwchReqTagSubjectID1 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagSubjectID2                      = CREATE_DRM_STRING( g_rgwchReqTagSubjectID2 );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagDrmkVersion                     = CREATE_DRM_STRING( g_rgwchReqTagDrmkVersion );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagRevInfo                         = CREATE_DRM_STRING( g_rgwchReqTagRevInfo );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagClientInfo                      = CREATE_DRM_STRING( g_rgwchReqTagClientInfo );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagClientID                        = CREATE_DRM_STRING( g_rgwchReqTagClientID );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrReqTagClientVersion                   = CREATE_DRM_STRING( g_rgwchReqTagClientVersion );

EXIT_PK_NAMESPACE_CODE;

