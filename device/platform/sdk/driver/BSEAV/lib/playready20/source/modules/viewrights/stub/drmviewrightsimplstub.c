/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmviewrights.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_GetLicenseAggregateData(
    __in_ecount( f_cActionsQueried) const DRM_CONST_STRING*  const f_rgpdstrAction[],
    __out_opt                             DRM_LICENSE_STATE_DATA   f_rgStateData[],
    __in                                  DRM_DWORD                f_cActionsQueried,
    __inout_opt                           DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __inout                               DRM_DST                 *f_pDatastore,
    __in                                  DRM_BOOL                 f_fDeleteExpiredLicenses,
    __in                                  DRM_DWORD                f_dwAggregationType,
    __in_opt                              DRMPFNPOLICYCALLBACK     f_pfnPolicyCallback,
    __in_opt                        const DRM_VOID                *f_pvCallbackParameter )
{
    UNREFERENCED_PARAMETER( f_rgpdstrAction          );
    UNREFERENCED_PARAMETER( f_rgStateData            );
    UNREFERENCED_PARAMETER( f_cActionsQueried        );
    UNREFERENCED_PARAMETER( f_pcontextLQ             );
    UNREFERENCED_PARAMETER( f_pDatastore             );
    UNREFERENCED_PARAMETER( f_fDeleteExpiredLicenses );
    UNREFERENCED_PARAMETER( f_dwAggregationType      );
    UNREFERENCED_PARAMETER( f_pfnPolicyCallback      );
    UNREFERENCED_PARAMETER( f_pvCallbackParameter    );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_IsAllowed(
    __in    const DRM_CONST_STRING        *f_pdstrAction,
    __inout       DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __inout       DRM_DST                 *f_pDatastore )
{
    UNREFERENCED_PARAMETER( f_pdstrAction );
    UNREFERENCED_PARAMETER( f_pcontextLQ  );
    UNREFERENCED_PARAMETER( f_pDatastore  );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_InitCanDoQuery(
    __in        DRM_CRYPTO_CONTEXT       *f_pDrmCrypto,
    __in        DRM_LICSTORE_CONTEXT     *f_pbLicenseStoreXML,
    __in        DRM_LICSTORE_CONTEXT     *f_pbLicenseStoreXMR,
    __in_ecount( DRM_MAX_LICENSE_CHAIN_DEPTH)   DRM_LICSTOREENUM_CONTEXT *f_rgpLicQueryContext[DRM_MAX_LICENSE_CHAIN_DEPTH],
    __in        DRM_LICEVAL_CONTEXT      *f_pLicEval,
    __in        DRM_BYTE                 *f_pbBuffer,
    __in        DRM_DWORD                 f_cbBuffer,
    __in        DRM_LONG                  f_lDeviceAppSec,
    __in        DRM_BOOL                  f_fDeviceHasSerialNum,
    __in        DRM_DWORD                 f_dwDeviceCopyOPL,
    __in  const DRM_GUID                 *f_pDeviceGUID,
    __in  const DRM_CONST_STRING         *f_pdstrDevCert,
    __in        DRM_DWORD                 f_cCacheSlots,
    __in        DRM_CANDO_QUERY_CACHE    *f_rgCacheSlots,
    __out       DRM_VIEW_RIGHTS_CONTEXT  *f_pcontextLQ )
{
    UNREFERENCED_PARAMETER( f_pDrmCrypto          );
    UNREFERENCED_PARAMETER( f_pbLicenseStoreXML   );
    UNREFERENCED_PARAMETER( f_pbLicenseStoreXMR   );
    UNREFERENCED_PARAMETER( f_rgpLicQueryContext  );
    UNREFERENCED_PARAMETER( f_pLicEval            );
    UNREFERENCED_PARAMETER( f_pbBuffer            );
    UNREFERENCED_PARAMETER( f_cbBuffer            );
    UNREFERENCED_PARAMETER( f_lDeviceAppSec       );
    UNREFERENCED_PARAMETER( f_fDeviceHasSerialNum );
    UNREFERENCED_PARAMETER( f_dwDeviceCopyOPL     );
    UNREFERENCED_PARAMETER( f_pDeviceGUID         );
    UNREFERENCED_PARAMETER( f_pdstrDevCert        );
    UNREFERENCED_PARAMETER( f_cCacheSlots         );
    UNREFERENCED_PARAMETER( f_rgCacheSlots        );
    UNREFERENCED_PARAMETER( f_pcontextLQ          );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_VIEWRIGHTS_CandoQuery(
    __in                                   DRM_KID                 *f_pKID,
    __in                                   DRM_DWORD                f_cActionsQueried,
    __in_ecount( f_cActionsQueried ) const DRM_CONST_STRING        *f_rgpdstrAction[],
    __inout                                DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __inout                                DRM_DST                 *f_pDatastore,
    __in                                   DRM_BOOL                 f_fDeleteExpiredLicenses,
    __out_ecount( f_cActionsQueried )      DRM_DWORD                f_rgResults[] )
{
    UNREFERENCED_PARAMETER( f_pKID                   );
    UNREFERENCED_PARAMETER( f_cActionsQueried        );
    UNREFERENCED_PARAMETER( f_rgpdstrAction          );
    UNREFERENCED_PARAMETER( f_pcontextLQ             );
    UNREFERENCED_PARAMETER( f_pDatastore             );
    UNREFERENCED_PARAMETER( f_fDeleteExpiredLicenses );
    UNREFERENCED_PARAMETER( f_rgResults              );

    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

