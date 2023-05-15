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

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ASD_ExamineOutputLevels(
    IN     DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    IN     DRM_COPY_OPL            *f_pOutputLevels,
       OUT DRM_DWORD               *f_pdwResult )
{
    UNREFERENCED_PARAMETER( f_pcontextLQ );
    UNREFERENCED_PARAMETER( f_pOutputLevels );
    UNREFERENCED_PARAMETER( f_pdwResult );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ASD_ExamineOutputLevelsXMR(
    IN     DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
       OUT DRM_DWORD               *f_pdwResult )
{
    UNREFERENCED_PARAMETER( f_pcontextLQ );
    UNREFERENCED_PARAMETER( f_pdwResult );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ProcessCopyOutputLevelData(
    IN const DRM_CONST_STRING             *f_pdstrRestrictions,
    IN OUT   DRM_COPY_OPL                 *f_pCopyOPL,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pdstrRestrictions );
    UNREFERENCED_PARAMETER( f_pCopyOPL );
    UNREFERENCED_PARAMETER( f_pstack );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ProcessCopyOutputLevelDataXMR(
    IN const DRM_XMR_COPY_RIGHTS          *f_pxmrCopyRights,
    IN OUT   DRM_COPY_OPL                 *f_pCopyOPL,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    UNREFERENCED_PARAMETER( f_pxmrCopyRights );
    UNREFERENCED_PARAMETER( f_pCopyOPL );
    UNREFERENCED_PARAMETER( f_pstack );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

