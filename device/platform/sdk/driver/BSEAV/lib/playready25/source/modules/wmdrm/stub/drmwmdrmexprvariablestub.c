/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_GlobalSetVariable(
    const DRM_CONST_STRING *pdstrToken,
    const TOKEN            *pNewValue,
          TOKEN            *pResult,
          DRM_VOID         *pvOpaqueData)
{
    UNREFERENCED_PARAMETER( pdstrToken   );
    UNREFERENCED_PARAMETER( pNewValue    );
    UNREFERENCED_PARAMETER( pResult      );
    UNREFERENCED_PARAMETER( pvOpaqueData );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_GlobalGetVariable(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData )
{
    UNREFERENCED_PARAMETER( pdstrToken   );
    UNREFERENCED_PARAMETER( pResult      );
    UNREFERENCED_PARAMETER( pvOpaqueData );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;
