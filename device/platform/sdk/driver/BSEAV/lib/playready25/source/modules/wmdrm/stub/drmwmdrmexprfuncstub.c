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

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_WMDRM_EXPRFUNC_IsValidFunction(
    __in const DRM_CONST_STRING *pStringToken,
    __in       DRM_DWORD        *pfnType )
{
    UNREFERENCED_PARAMETER( pStringToken );
    UNREFERENCED_PARAMETER( pfnType      );
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_LONG DRM_CALL DRM_WMDRM_EXPRFUNC_Arity(DRM_EXPR_FUNCTION_TYPE fnType)
{
    UNREFERENCED_PARAMETER( fnType );
    return -1;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPRFUNC_FunctionGetValue(
    __in const TOKEN                       *pOperator,
    __in       DRM_LONG                     cArgs,
    __in       TOKEN                        rgArgs[],
    __out      TOKEN                       *pOperandResult,
    __in       DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext )
{
    UNREFERENCED_PARAMETER( pOperator      );
    UNREFERENCED_PARAMETER( cArgs          );
    UNREFERENCED_PARAMETER( rgArgs         );
    UNREFERENCED_PARAMETER( pOperandResult );
    UNREFERENCED_PARAMETER( pEEContext     );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

