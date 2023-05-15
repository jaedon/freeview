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
    __in                                         DRM_SYNC_CONTEXT           *f_pcontextSYN,
    __in                                   const DRM_KID                    *f_pkid,
    __in_opt                                     DRM_DST_SLOT_HINT          *f_pslotHint,
    __out_opt                                    DRM_LICENSE_STATE_DATA     *f_plsd,
    __out_ecount_opt(*f_pcchContentHeader)       DRM_WCHAR                  *f_pwszContentHeader,
    __inout_ecount_opt(1)                        DRM_DWORD                  *f_pcchContentHeader )
{
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_SNC_SetStoreDataForKID(
    __in           DRM_SYNC_CONTEXT          *f_pcontextSYN,
    __in     const DRM_KID                   *f_pkid,
    __in_opt       DRM_DST_SLOT_HINT         *f_pslotHint,
    __in_opt const DRM_LICENSE_STATE_DATA    *f_plsd,
    __in_opt const DRM_STRING                *f_pdstrContentHeader,
    __in           DRM_BOOL                   f_fCreateIfNotExisting )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_DeleteKID(
    __in       DRM_SYNC_CONTEXT *f_pcontextSYN,
    __in const DRM_KID          *f_pkid )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_OpenStore(
    __in  DRM_DST                *f_pDatastore,
    __out DRM_SYNC_CONTEXT       *f_pcontextSYN )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_CloseStore(
    __in  DRM_SYNC_CONTEXT       *f_pcontextSYN )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_InitEnum(
    __in  DRM_SYNC_CONTEXT       *f_pcontextSYN,
    __out DRM_SYNC_ENUM_CONTEXT  *f_pcontextSYNEnum )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_EnumNext(
    __in  DRM_SYNC_ENUM_CONTEXT  *f_pcontextSYNEnum,
    __out DRM_KID                *f_pkid,
    __out DRM_DST_SLOT_HINT      *f_pslotHint )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_GetSyncStoreEntry(
    __in                                       DRM_SYNC_CONTEXT        *f_pcontextSYN,
    __in                                 const DRM_KID                 *f_pkid,
    __out_opt                                  DRM_LICENSE_STATE_DATA  *f_plsd,
    __out_ecount_opt(*f_pcchContentHeader)     DRM_WCHAR               *f_pwszContentHeader,
    __inout_opt                                DRM_DWORD               *f_pcchContentHeader )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_UpdateKID(
    __in           DRM_SYNC_CONTEXT          *f_pcontextSYN,
    __in_opt       DRM_VIEW_RIGHTS_CONTEXT   *f_pcontextViewRightsIn,
    __in     const DRM_KID                   *f_pkid,
    __in_opt const DRM_CONST_STRING          *f_pdstrContentHeader,
    __in           DRM_SYNC_UPDATE_CATEGORY   f_eSyncCategory )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

