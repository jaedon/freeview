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

const                DRM_WCHAR      g_rgwchTagUplinks                            [] =  { ONE_WCHAR('U', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagLicenseRequest                     [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Q', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagV1Challenge                        [] =  { ONE_WCHAR('V', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('H', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchDefaultV1Data                         [] =  { ONE_WCHAR('N', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagActionList                         [] =  { ONE_WCHAR('A', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagAppSecurity                     [] =  { ONE_WCHAR('A', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagSubjectID1                      [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('J', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('1', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagSubjectID2                      [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('J', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('2', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagDrmkVersion                     [] =  { ONE_WCHAR('D', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagRevInfo                         [] =  { ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('F', '\0'), ONE_WCHAR('O', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagClientInfo                      [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagClientID                        [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchReqTagClientVersion                   [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('\0', '\0')};

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

