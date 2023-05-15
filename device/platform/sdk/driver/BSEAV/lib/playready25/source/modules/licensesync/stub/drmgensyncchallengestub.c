/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsyncstore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_RESULT DRM_SNC_GetLicenseDataForKID(
    __in  DRM_DST                    *f_pDatastore,
    __in  DRM_VIEW_RIGHTS_CONTEXT    *f_pcontextViewRightsIn,
    __out DRM_LICENSE_STATE_DATA     *f_plsd )
{
    return DRM_E_NOTIMPL;
}

DRM_BOOL DRM_SNC_IsSyncRequired(
    __in_opt DRM_VOID                  *pOEMContext,
    __in     DRM_DWORD                  cMaxCountIn,
    __in     DRM_DWORD                  cMaxHoursIn,
    __in     DRM_LICENSE_STATE_DATA    *plsd,
    __inout  DRM_UINT64                *pui64ThreshholdTime )
{
    return FALSE;
}

DRM_BOOL DRM_SNC_IsStoreRefreshRequired(
    __in DRM_VOID               *f_pOEMContext,
    __in DRM_LICENSE_STATE_DATA *f_plsd )
{
    return FALSE;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_DidLicenseExpire(
    __in            DRM_SYNC_CONTEXT          *f_pcontextSync,
    __in_opt        DRM_VIEW_RIGHTS_CONTEXT   *f_pcontextViewRights,
    __in            DRM_DST                   *f_pDatastore,
    __in      const DRM_KID                   *f_pKID,
    __out           DRM_BOOL                  *f_pfExpired )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_GenerateSyncChallenge(
    __in      DRM_SYNC_CONTEXT            *f_pcontextSync,
    __in      DRM_VIEW_RIGHTS_CONTEXT     *f_pcontextViewRights,
    __in      DRM_DST                     *f_pDatastore,
    __in      DRM_STACK_ALLOCATOR_CONTEXT *f_pcontextStack,
    __in      DRM_DWORD                    f_cMaxCount,
    __in      DRM_DWORD                    f_cMaxHours,
    __in      DRM_DWORD                    f_iKIDStart,
    __in      DRM_DWORD                    f_cKIDs,
    __out_opt DRM_DWORD                   *f_piKIDNext,
    __out_opt DRM_DWORD                   *f_pcKIDs,
    __out_opt _XMBContext                 *f_pbChallenge,
    __inout   DRM_DWORD                   *f_pcbChallenge )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

