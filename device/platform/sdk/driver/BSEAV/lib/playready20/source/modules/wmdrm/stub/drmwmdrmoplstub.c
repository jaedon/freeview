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

DRM_API DRM_WORD DRM_CALL DRM_WMDRM_LICOPL_GetOPLLevel(
    IN const DRM_CONST_STRING *f_pdstrRestrictions,
    IN const DRM_CONST_STRING *f_pdstrCategory )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pdstrCategory     );
    return MAX_UNSIGNED_TYPE( DRM_WORD );
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICOPL_ProcessExclusions(
    IN const DRM_CONST_STRING             *f_pdstrRestrictions,
    IN OUT   DRM_OPL_OUTPUT_IDS           *f_pOPLIds,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pOPLIds           );
    UNREFERENCED_PARAMETER( f_pstack            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICOPL_ProcessInclusions(
    IN const DRM_CONST_STRING             *f_pdstrRestrictions,
    IN OUT   DRM_OPL_OUTPUT_IDS           *f_pOPLIds,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pOPLIds           );
    UNREFERENCED_PARAMETER( f_pstack            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICOPL_ProcessPlayOutputLevelData(
    IN const DRM_CONST_STRING             *f_pdstrRestrictions,
    IN OUT   DRM_PLAY_OPL_EX2             *f_pPlayOPL,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pPlayOPL          );
    UNREFERENCED_PARAMETER( f_pstack            );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICCOPYOPL_ASD_ExamineOutputLevels(
    IN     DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN     DRM_COPY_OPL            *f_pOutputLevels,
       OUT DRM_DWORD               *f_pdwResult )
{
    UNREFERENCED_PARAMETER( f_pcontextLQ    );
    UNREFERENCED_PARAMETER( f_pOutputLevels );
    UNREFERENCED_PARAMETER( f_pdwResult     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LICCOPYOPL_ProcessCopyOutputLevelData(
    IN const DRM_CONST_STRING             *f_pdstrRestrictions,
    IN OUT   DRM_COPY_OPL                 *f_pCopyOPL,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pCopyOPL          );
    UNREFERENCED_PARAMETER( f_pstack            );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

