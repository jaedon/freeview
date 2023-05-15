/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcopyopl.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ASD_ExamineOutputLevels(
    __in  const DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __in  const DRM_COPY_OPL            *f_pOutputLevels,
    __out       DRM_DWORD               *f_pdwResult )
{
    UNREFERENCED_PARAMETER( f_pcontextLQ );
    UNREFERENCED_PARAMETER( f_pOutputLevels );
    UNREFERENCED_PARAMETER( f_pdwResult );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ASD_ExamineOutputLevelsXMR(
    __in  DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __out DRM_DWORD               *f_pdwResult )
{
    UNREFERENCED_PARAMETER( f_pcontextLQ );
    UNREFERENCED_PARAMETER( f_pdwResult );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ProcessCopyOutputLevelData(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_COPY_OPL                 *f_pCopyOPL,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pCopyOPL );
    UNREFERENCED_PARAMETER( f_pstack );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ProcessCopyOutputLevelDataXMR(
    __in    const DRM_XMR_COPY_RIGHTS          *f_pxmrCopyRights,
    __inout       DRM_COPY_OPL                 *f_pCopyOPL,
    __in    const DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pxmrCopyRights );
    UNREFERENCED_PARAMETER( f_pCopyOPL );
    UNREFERENCED_PARAMETER( f_pstack );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

