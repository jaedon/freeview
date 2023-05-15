/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsyncstore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_RESULT DRM_SNC_GetLicenseDataForKID (
    IN  DRM_DST                    *f_pDatastore,
    IN  DRM_VIEW_RIGHTS_CONTEXT    *f_pcontextViewRightsIn,
    OUT DRM_LICENSE_STATE_DATA     *f_plsd)
{
    return DRM_E_NOTIMPL;
}

DRM_BOOL DRM_SNC_IsSyncRequired(
    DRM_VOID                  *pOEMContext,
    DRM_DWORD                  cMaxCountIn,
    DRM_DWORD                  cMaxHoursIn,
    DRM_LICENSE_STATE_DATA    *plsd,
    DRM_UINT64                *pui64ThreshholdTime)
{
    return FALSE;
}

DRM_BOOL DRM_SNC_IsStoreRefreshRequired (
    IN DRM_VOID               *f_pOEMContext,
    IN DRM_LICENSE_STATE_DATA *f_plsd)
{
    return FALSE;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_DidLicenseExpire(
    IN     DRM_SYNC_CONTEXT          *f_pcontextSync,
    IN     DRM_VIEW_RIGHTS_CONTEXT   *f_pcontextViewRights,
    IN     DRM_DST                   *f_pDatastore,
    IN      DRM_KID                  *f_pKID,
       OUT  DRM_BOOL                 *f_pfExpired )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SNC_GenerateSyncChallenge(
    IN     DRM_SYNC_CONTEXT            *f_pcontextSync,
    IN     DRM_VIEW_RIGHTS_CONTEXT     *f_pcontextViewRights,
    IN     DRM_DST                     *f_pDatastore,
    IN     DRM_STACK_ALLOCATOR_CONTEXT *f_pcontextStack,
    IN     DRM_DWORD                    f_cMaxCount,
    IN     DRM_DWORD                    f_cMaxHours,
    IN     DRM_DWORD                    f_iKIDStart,
    IN     DRM_DWORD                    f_cKIDs,
       OUT DRM_DWORD                   *f_piKIDNext,
       OUT DRM_DWORD                   *f_pcKIDs,
       OUT _XMBContext                 *f_pbChallenge,
    IN OUT DRM_DWORD                   *f_pcbChallenge)
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

