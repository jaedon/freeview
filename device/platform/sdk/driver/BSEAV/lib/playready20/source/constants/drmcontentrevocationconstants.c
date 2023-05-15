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

const                DRM_WCHAR      g_rgwchContentRevocation                     [] =  { ONE_WCHAR('C', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('A', '\0'),  ONE_WCHAR('T', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchSequenceNumber                        [] =  { ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Q', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('M', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('\0', '\0')};
const                DRM_WCHAR      g_rgwchLSPubKey                              [] =  { ONE_WCHAR('L', '\0'),  ONE_WCHAR('I', '\0'),  ONE_WCHAR('C', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('V', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('R', '\0'),  ONE_WCHAR('P', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('B', '\0'),  ONE_WCHAR('K', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('Y', '\0'),  ONE_WCHAR('\0', '\0')};

const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrContentRevocation                     = CREATE_DRM_STRING( g_rgwchContentRevocation );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrSequenceNumber                        = CREATE_DRM_STRING( g_rgwchSequenceNumber );
const DRM_DISCARDABLE DRM_CONST_STRING      g_dstrLSPubKey                              = CREATE_DRM_STRING( g_rgwchLSPubKey );

EXIT_PK_NAMESPACE_CODE;

