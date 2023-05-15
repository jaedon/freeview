/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsyncstore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_RESULT DRM_SNC_GetStoreDataForKID(
    __in                                      DRM_SYNC_CONTEXT          *f_pcontextSYN,
    __in const                                DRM_KID                   *f_pkid,
    __in_opt                                  DRM_DST_SLOT_HINT         *f_pslotHint,
    __out_opt                                 DRM_LICENSE_STATE_DATA    *f_plsd,
    __out_ecount_opt(*f_pcchContentHeader)    DRM_WCHAR                 *f_pwszContentHeader,
    __inout_opt                               DRM_DWORD                 *f_pcchContentHeader)
{
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_SNC_SetStoreDataForKID(
    IN       DRM_SYNC_CONTEXT          *f_pcontextSYN,
    IN const DRM_KID                   *f_pkid,
    IN       DRM_DST_SLOT_HINT         *f_pslotHint,
    IN const DRM_LICENSE_STATE_DATA    *f_plsd,
    IN const DRM_STRING                *f_pdstrContentHeader,
    IN       DRM_BOOL                   f_fCreateIfNotExisting )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_DeleteKID(
    IN       DRM_SYNC_CONTEXT *f_pcontextSYN,
    IN const DRM_KID          *f_pkid)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_OpenStore  (
    IN  DRM_DST                *f_pDatastore,
    OUT DRM_SYNC_CONTEXT       *f_pcontextSYN)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_CloseStore (
    IN  DRM_SYNC_CONTEXT       *f_pcontextSYN)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_InitEnum   (
    IN  DRM_SYNC_CONTEXT       *f_pcontextSYN,
    OUT DRM_SYNC_ENUM_CONTEXT  *f_pcontextSYNEnum)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_EnumNext   (
    IN  DRM_SYNC_ENUM_CONTEXT  *f_pcontextSYNEnum,
    OUT DRM_KID                *f_pkid,
    OUT DRM_DST_SLOT_HINT      *f_pslotHint)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_GetSyncStoreEntry(
    IN                                       DRM_SYNC_CONTEXT        *f_pcontextSYN,
    IN const                                 DRM_KID                 *f_pkid,
       OUT                                   DRM_LICENSE_STATE_DATA  *f_plsd,
       __out_ecount_opt(*f_pcchContentHeader)DRM_WCHAR               *f_pwszContentHeader,
    IN OUT                                   DRM_DWORD               *f_pcchContentHeader)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_UpdateKID(
    IN       DRM_SYNC_CONTEXT          *f_pcontextSYN,
    IN       DRM_VIEW_RIGHTS_CONTEXT   *f_pcontextViewRightsIn,
    IN const DRM_KID                   *f_pkid,
    IN const DRM_CONST_STRING          *f_pdstrContentHeader,
    IN       DRM_SYNC_UPDATE_CATEGORY   f_eSyncCategory )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

