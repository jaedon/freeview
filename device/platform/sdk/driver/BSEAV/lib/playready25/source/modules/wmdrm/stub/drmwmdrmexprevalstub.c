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
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

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
    __in    const DRM_CONST_STRING            *pdstrExpression,
    __inout       DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext,
    __out         TOKEN                       *ptResult )
{
    UNREFERENCED_PARAMETER( pdstrExpression );
    UNREFERENCED_PARAMETER( pEEContext      );
    UNREFERENCED_PARAMETER( ptResult        );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_LEVL_EvaluateExpression(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_CONST_STRING    *f_pdstrExpression,
    __out         DRM_BOOL            *f_pfValue )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    UNREFERENCED_PARAMETER( f_pdstrExpression );
    UNREFERENCED_PARAMETER( f_pfValue         );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

