/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_DDC_VerifySymmetricSignature(
    IN DRM_VOID         *f_pOEMContext,
    IN DRM_CONST_STRING *f_pdstrCert )
{
    UNREFERENCED_PARAMETER( f_pOEMContext );
    UNREFERENCED_PARAMETER( f_pdstrCert );

    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

