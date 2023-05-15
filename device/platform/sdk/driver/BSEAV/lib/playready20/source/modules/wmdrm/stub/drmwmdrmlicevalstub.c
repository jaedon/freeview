/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LEVL_PerformOperationsXML(
    IN OUT   DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    IN       eDRM_LICEVAL_OPERATIONS       f_eOperation,
    IN       eDRM_LICEVAL_OPERATION_STATE  f_eOperationState,
    IN const DRM_CONST_STRING             *f_pdstrAction,
    OUT      DRM_BOOL                     *f_pfPerform,
    OUT      DRM_BOOL                     *f_pfActionExisted,
    IN const DRM_DST                      *f_pDatastore )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    UNREFERENCED_PARAMETER( f_eOperation      );
    UNREFERENCED_PARAMETER( f_eOperationState );
    UNREFERENCED_PARAMETER( f_pdstrAction     );
    UNREFERENCED_PARAMETER( f_pfPerform       );
    UNREFERENCED_PARAMETER( f_pfActionExisted );
    UNREFERENCED_PARAMETER( f_pDatastore      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LEVL_GetLicenseReasonForUnusable(
    IN  const DRM_LICEVAL_CONTEXT           *f_pcontextLicEval,
    OUT       DRM_LONG                      *f_plReasonForUnusable )
{
    /* Leave f_plReasonForUnusable unchanged */
    UNREFERENCED_PARAMETER( f_pcontextLicEval     );
    UNREFERENCED_PARAMETER( f_plReasonForUnusable );
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LEVL_ParseV2License(
    __in      const DRM_CONST_STRING *f_pdstrLicense,
    __in      const DRM_CONST_STRING *f_pdstrAttrName,
    __out_opt       DRM_DWORD        *f_pdwValue,
    __out_opt       DRMFILETIME      *f_pfiletime )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense  );
    UNREFERENCED_PARAMETER( f_pdstrAttrName );
    UNREFERENCED_PARAMETER( f_pdwValue      );
    UNREFERENCED_PARAMETER( f_pfiletime     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_LEVL_HasSAP(
    __in      const DRM_CONST_STRING *f_pdstrLicense )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense );
    return FALSE;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_CheckSymmOptStatus(
    __in      const  DRM_CONST_STRING   *f_pdstrLicense,
    __out            DRM_SYMMOPT_STATUS *f_eStatus,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNode,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNodeData,
    __out_opt        DRM_GUID           *f_pCkVerifiedGUID )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense         );
    UNREFERENCED_PARAMETER( f_eStatus              );
    UNREFERENCED_PARAMETER( f_pdstrSymmSigNode     );
    UNREFERENCED_PARAMETER( f_pdstrSymmSigNodeData );
    UNREFERENCED_PARAMETER( f_pCkVerifiedGUID      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_UpdateLicenseWithSymmetricData(
    __inout        DRM_STRING          *f_pdstrLicense,
    __in           DRM_DWORD            f_cchLicenseMax,
    __in           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in           DRM_BOOL             f_fVerifiedCK,
    __in_opt const DRM_SLK             *f_pslk )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense  );
    UNREFERENCED_PARAMETER( f_cchLicenseMax );
    UNREFERENCED_PARAMETER( f_pcontextLEVL  );
    UNREFERENCED_PARAMETER( f_fVerifiedCK   );
    UNREFERENCED_PARAMETER( f_pslk          );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_VerifySymmetricSignatureIfPresent(
    IN const DRM_CONST_STRING *f_pdstrLicense,
    IN       DRM_BB_CONTEXT   *f_pcontextBBX,
    IN       DRM_CONST_STRING *f_pdstrLIData,
    IN       DRM_CONST_STRING *f_pdstrSignature,
       OUT   DRM_LONG         *f_plResult )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense   );
    UNREFERENCED_PARAMETER( f_pcontextBBX    );
    UNREFERENCED_PARAMETER( f_pdstrLIData    );
    UNREFERENCED_PARAMETER( f_pdstrSignature );
    UNREFERENCED_PARAMETER( f_plResult       );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_PerformAppRevocationCheck(
    IN     DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    IN     DRM_CONST_STRING            *f_pdstrContentLicense,
    IN     APPCERT                     *f_pAppCert,
    __in_bcount( f_cbRevocationBuffer ) DRM_BYTE *f_pbRevocationBuffer,
    IN     DRM_DWORD                    f_cbRevocationBuffer,
    IN OUT DRM_REVLIST_ID_VALUES       *f_pRevListIDs )
{
    UNREFERENCED_PARAMETER( f_pContextCrypto      );
    UNREFERENCED_PARAMETER( f_pContextRev         );
    UNREFERENCED_PARAMETER( f_pdstrContentLicense );
    UNREFERENCED_PARAMETER( f_pAppCert            );
    UNREFERENCED_PARAMETER( f_pbRevocationBuffer  );
    UNREFERENCED_PARAMETER( f_cbRevocationBuffer  );
    UNREFERENCED_PARAMETER( f_pRevListIDs         );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetCopyOPLRestriction(
    __inout     DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __in  const DRM_CONST_STRING        *f_pdstrAction,
    __in  const DRM_CONST_STRING        *f_pdstrRightUsed,
    __out       DRM_BOOL                *f_pfHasOPLs,
    __out       DRM_DWORD               *f_pdwResult )
{
    UNREFERENCED_PARAMETER( f_pcontextLQ     );
    UNREFERENCED_PARAMETER( f_pdstrAction    );
    UNREFERENCED_PARAMETER( f_pdstrRightUsed );
    UNREFERENCED_PARAMETER( f_pfHasOPLs      );
    UNREFERENCED_PARAMETER( f_pdwResult      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_LIC_HasOPLRestriction(
    __in const DRM_CONST_STRING *f_pdstrLicense,
    __in const DRM_CONST_STRING *f_pdstrRightUsed )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense   );
    UNREFERENCED_PARAMETER( f_pdstrRightUsed );
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_WMDRM_LIC_ShouldCheckClockRollback(
    __in const DRM_CONST_STRING *f_pdstrLicense )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense );
    return FALSE;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

