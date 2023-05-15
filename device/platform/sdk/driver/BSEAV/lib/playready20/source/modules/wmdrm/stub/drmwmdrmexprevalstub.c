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

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_ExtractLongToken(
    DRM_CONST_STRING *pdstrExpression,
    TOKEN            *pToken )
{
    UNREFERENCED_PARAMETER( pdstrExpression );
    UNREFERENCED_PARAMETER( pToken          );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_ExtractDateToken(
    DRM_CONST_STRING *pdstrExpression,
    TOKEN            *pToken)
{
    UNREFERENCED_PARAMETER( pdstrExpression );
    UNREFERENCED_PARAMETER( pToken          );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_EvaluateExpression(
    IN const DRM_CONST_STRING            *pdstrExpression,
    IN OUT   DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext,
       OUT   TOKEN                       *ptResult )
{
    UNREFERENCED_PARAMETER( pdstrExpression );
    UNREFERENCED_PARAMETER( pEEContext      );
    UNREFERENCED_PARAMETER( ptResult        );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_LEVL_EvaluateExpression(
    IN OUT   DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN const DRM_CONST_STRING    *f_pdstrExpression,
    OUT      DRM_BOOL            *f_pfValue )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    UNREFERENCED_PARAMETER( f_pdstrExpression );
    UNREFERENCED_PARAMETER( f_pfValue         );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

