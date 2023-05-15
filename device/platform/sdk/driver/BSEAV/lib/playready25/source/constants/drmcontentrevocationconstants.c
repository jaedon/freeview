/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontentrevocationconstants.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE_CODE;

DRM_STR_CONST                DRM_WCHAR      g_rgwchContentRevocation                     [] =  { INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('A'),  INIT_WCHAR_OBFUS('T'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('O'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchSequenceNumber                        [] =  { INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Q'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('M'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('\0')};
DRM_STR_CONST                DRM_WCHAR      g_rgwchLSPubKey                              [] =  { INIT_WCHAR_OBFUS('L'),  INIT_WCHAR_OBFUS('I'),  INIT_WCHAR_OBFUS('C'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('N'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('S'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('V'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('R'),  INIT_WCHAR_OBFUS('P'),  INIT_WCHAR_OBFUS('U'),  INIT_WCHAR_OBFUS('B'),  INIT_WCHAR_OBFUS('K'),  INIT_WCHAR_OBFUS('E'),  INIT_WCHAR_OBFUS('Y'),  INIT_WCHAR_OBFUS('\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrContentRevocation                     = CREATE_DRM_STRING( g_rgwchContentRevocation );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSequenceNumber                        = CREATE_DRM_STRING( g_rgwchSequenceNumber );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrLSPubKey                              = CREATE_DRM_STRING( g_rgwchLSPubKey );

EXIT_PK_NAMESPACE_CODE;

