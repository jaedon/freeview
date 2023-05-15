/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>
#include <drmxmlparser.h>
#include <drmutilities.h>
#include <drmwmdrm.h>
#include <drmsymoptconstants.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: DRM_SYMOPT_LIC_CheckSymmOptStatus
**
** Synopsis: Returns status of symmetric optimizations for a license.
**           Status values are defined in DRM_SYMMOPT_STATUS
**
** Arguments:
**
** [f_pdstrLicense]         -- The license string.
** [f_eStatus]              -- Returns the status (from DRM_SYMMOPT_STATUS)
** [f_pdstrSymmSigNode]     -- (optional) Returns the SYMSIGNATURE node if it exists.
** [f_pdstrSymmSigNodeData] -- (optional) Returns the SYMSIGNATURE data if the node exists
** [f_pCkVerifiedGUID]      -- (optional) Returns the GUID containing the CKVerified bit
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_CheckSymmOptStatus(
    __in      const  DRM_CONST_STRING   *f_pdstrLicense,
    __out            DRM_SYMMOPT_STATUS *f_eStatus,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNode,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNodeData,
    __out_opt        DRM_GUID           *f_pCkVerifiedGUID )
{
    return DRM_WMDRM_LIC_CheckSymmOptStatus( f_pdstrLicense, f_eStatus, f_pdstrSymmSigNode, f_pdstrSymmSigNodeData, f_pCkVerifiedGUID );
}

/*********************************************************************
**
**  Function:  DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData
**
**  Synopsis:  Given a license update it with a symmetric signature and symmetrically encrypted content key.
**             This drastically improves binding and playback performance by offloading those asymmetric
**             operations to a one time hit here.
**
**  Arguments:
**     [f_pdstrLicense]    -- License to add symmetric data to
**     [f_cchLicenseMax]   -- Maximum license size that can fix in the f_pdstrLicense buffer
**     [f_pcontextLEVL]    -- Pointer to a license evaluator context.
**     [f_fVerifiedCK]     -- Specifies whether the content key has already been verified
**     [f_pslk]            -- Pointer to the SLK the license is currently encrypted to (if applicable).
**                            If f_pslk is NULL and the license is symm opted, will attempt to verify using
**                            the current bbx slk.
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData(
    __inout        DRM_STRING          *f_pdstrLicense,
    __in           DRM_DWORD            f_cchLicenseMax,
    __in           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in           DRM_BOOL             f_fVerifiedCK,
    __in_opt const DRM_SLK             *f_pslk )
{
    return DRM_WMDRM_LIC_UpdateLicenseWithSymmetricData( f_pdstrLicense, f_cchLicenseMax, f_pcontextLEVL, f_fVerifiedCK, f_pslk );
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_VerifySymmetricSignatureIfPresent(
    IN const DRM_CONST_STRING *f_pdstrLicense,
    IN       DRM_BB_CONTEXT   *f_pcontextBBX,
    IN       DRM_CONST_STRING *f_pdstrLIData,
    IN       DRM_CONST_STRING *f_pdstrSignature,
       OUT   DRM_LONG         *f_plResult )
{
    return DRM_WMDRM_LIC_VerifySymmetricSignatureIfPresent( f_pdstrLicense, f_pcontextBBX, f_pdstrLIData, f_pdstrSignature, f_plResult );
}

EXIT_PK_NAMESPACE_CODE;

