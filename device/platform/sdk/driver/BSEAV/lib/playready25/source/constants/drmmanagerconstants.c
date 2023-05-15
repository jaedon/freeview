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

DRM_STR_CONST                DRM_WCHAR      g_rgwchTagLicenseStateRequestRoot            [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagCanBind                            [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('D'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagDeviceLicenseRevocationRoot        [] =  { INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchTagRevocationPubKey                   [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('G'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagLicenseStateRequestRoot            = CREATE_DRM_STRING( g_rgwchTagLicenseStateRequestRoot );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagCanBind                            = CREATE_DRM_STRING( g_rgwchTagCanBind );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagDeviceLicenseRevocationRoot        = CREATE_DRM_STRING( g_rgwchTagDeviceLicenseRevocationRoot );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrTagRevocationPubKey                   = CREATE_DRM_STRING( g_rgwchTagRevocationPubKey );

EXIT_PK_NAMESPACE_CODE;

