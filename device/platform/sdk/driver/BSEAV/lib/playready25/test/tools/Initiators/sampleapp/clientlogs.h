/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __CLIENTLOGS_H__
#define __CLIENTLOGS_H__

#include <drmcommon.h>
#include <drmtypes.h>

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

DRM_API DRM_VOID DRM_CALL LogMessage(
    IN DRM_CHAR *f_pszMessage, ... );

DRM_API DRM_RESULT DRM_CALL LogDrmString(
    DRM_CONST_STRING *f_dstrMsg );

DRM_API DRM_RESULT DRM_CALL LogPacket(
    IN DRM_BOOL f_fChallenge,
    IN DRM_BYTE *f_pbPacket,
    IN DRM_DWORD f_cbPacket );

EXIT_PKTEST_NAMESPACE

#endif /* __CLIENTLOGS_H__ */



