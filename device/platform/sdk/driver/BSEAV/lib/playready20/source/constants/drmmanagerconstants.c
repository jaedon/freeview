/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmanagerconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

const                DRM_WCHAR      g_rgwchTagLicenseStateRequestRoot            [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagCanBind                            [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('D', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagDeviceLicenseRevocationRoot        [] =  { ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchTagRevocationPubKey                   [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('G', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('\0', '\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLicenseStateRequestRoot            = CREATE_DRM_STRING( g_rgwchTagLicenseStateRequestRoot );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCanBind                            = CREATE_DRM_STRING( g_rgwchTagCanBind );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDeviceLicenseRevocationRoot        = CREATE_DRM_STRING( g_rgwchTagDeviceLicenseRevocationRoot );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagRevocationPubKey                   = CREATE_DRM_STRING( g_rgwchTagRevocationPubKey );

EXIT_PK_NAMESPACE_CODE;

