/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmodelrevocation.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_MODELREVOCATION_CheckIfPlayReadyModelRevoked(
    __in       DRM_VOID *f_pOEMContext,
    __in const DRM_BCRL *f_pPRMainCRL,
    __out      DRM_BOOL *f_pfStoreCRL )
{
    CLAW_AUTO_RANDOM_CIPHER
    UNREFERENCED_PARAMETER( f_pOEMContext );
    UNREFERENCED_PARAMETER( f_pPRMainCRL );
    return DRM_SUCCESS;  /* Model is never revoked if feature is not supported */
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

