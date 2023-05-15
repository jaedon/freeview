/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_DDC_VerifySymmetricSignature(
    __in       DRM_VOID         *f_pOEMContext,
    __in const DRM_CONST_STRING *f_pdstrCert )
{
    UNREFERENCED_PARAMETER( f_pOEMContext );
    UNREFERENCED_PARAMETER( f_pdstrCert );

    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

