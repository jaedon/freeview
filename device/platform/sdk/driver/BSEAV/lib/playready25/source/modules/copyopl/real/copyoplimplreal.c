/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_COPYOPLIMPLREAL_C
#include <drmcopyopl.h>
#include <drmwmdrm.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function :   DRM_COPYOPL_ASD_ExamineOutputLevels
** Synopsis :   Examine the output level data from DRM
** Arguments :
** Returns :    DRM_SUCCESS if output level is allowed
**              DRM_S_FALSE otherwise
** Notes :
** Algorithm :
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ASD_ExamineOutputLevels(
    __in  const DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __in  const DRM_COPY_OPL            *f_pOutputLevels,
    __out       DRM_DWORD               *f_pdwResult )
{
    return DRM_WMDRM_LICCOPYOPL_ASD_ExamineOutputLevels( f_pcontextLQ, f_pOutputLevels, f_pdwResult );
}

/*****************************************************************************
**
** Function  :   DRM_COPYOPL_ASD_ExamineOutputLevelsXMR
**
** Synopsis  :   Examine the output level data from DRM
**
** Arguments :  [f_pcontextLQ] : Specifies the ViewRights context
**              [f_pdwResult]  : Returns result flags
**
** Returns   :
**      DRM_SUCCESS
**          output level is allowed
**      DRM_S_FALSE
**          output level is not allowed
**      DRM_E_INVALIDARG
**          One of the parameters was NULL
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ASD_ExamineOutputLevelsXMR(
    __in  DRM_VIEW_RIGHTS_CONTEXT *f_pcontextLQ,
    __out DRM_DWORD               *f_pdwResult )
{
    DRM_RESULT                 dr          = DRM_SUCCESS;
    const DRM_XMR_COPY_RIGHTS *pCopyRights = NULL;

    ChkArg( f_pcontextLQ != NULL );
    ChkArg( f_pdwResult  != NULL );

    pCopyRights = &( f_pcontextLQ->licXMR.containerOuter.containerCopyPolicies );
    f_pdwResult = 0;

    /*
    ** check the OPL level
    */
    if( XMR_IS_COPYPROTECTIONLEVEL_VALID( &f_pcontextLQ->licXMR )
     && ( DRM_DWORD )( pCopyRights->CopyProtectionLevel.wValue ) > f_pcontextLQ->dwDeviceCopyOPL )
    {
       *f_pdwResult |= DRM_LICQUERY_NOT_ENABLED_COPY_OPL_TOO_LOW;
        dr = DRM_S_FALSE;
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_COPYOPL_ProcessCopyOutputLevelData
**
**  Synopsis:
**
**  Arguments:
**     [f_pdstrRestrictions] -- String from XML blob that contains the entire RESTRICTIONS section
**     [f_pCopyOPL]          -- Copy OPL struct that will be populated with output level data
**     [f_pstack]            -- Stack allocator context that will be used to allocate room for the Ids
**
**  Notes:  Caller should provide a clean, unused stack context.  Caller shouldn't free data directly from stack
**          context.  Instead just throw entire stack context away when finished with the OPL structures.
**
*********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ProcessCopyOutputLevelData(
    __in    const DRM_CONST_STRING             *f_pdstrRestrictions,
    __inout       DRM_COPY_OPL                 *f_pCopyOPL,
    __in          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    return DRM_WMDRM_LICCOPYOPL_ProcessCopyOutputLevelData( f_pdstrRestrictions, f_pCopyOPL, f_pstack );
}

/*********************************************************************
**
**  Function:  DRM_COPYOPL_ProcessCopyOutputLevelDataXMR
**
**  Synopsis:  Process a Copy OPL for XMR
**
**  Arguments:
**     [f_pxmrCopyRights] : XMR Copy rights structure to parse the OPL from
**     [f_pCopyOPL]       : Copy OPL struct that will be populated with output level data
**     [f_pstack]         : Stack allocator context that will be used to allocate room for IDs
**
** Returns:
**      DRM_SUCCESS:
**          Success
**      DRM_E_INVALIDARG:
**          One of the arguments was NULL or improperly initialized
**
**  Notes:  Caller should provide a clean, unused stack context.
**          Caller shouldn't free data directly from stack context.
**          Instead just throw entire stack context away when finished with the OPL structures.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_COPYOPL_ProcessCopyOutputLevelDataXMR(
    __in    const DRM_XMR_COPY_RIGHTS          *f_pxmrCopyRights,
    __inout       DRM_COPY_OPL                 *f_pCopyOPL,
    __in    const DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCopyOPL          != NULL );
    ChkArg( f_pstack            != NULL );
    ChkArg( f_pstack->pbStack   != NULL );
    ChkArg( f_pstack->cbStack    > 0 );
    ChkArg( f_pxmrCopyRights    != NULL );

    ZEROMEM( f_pCopyOPL, SIZEOF( *f_pCopyOPL ) );

    if( f_pxmrCopyRights->CopyProtectionLevel.fValid )
    {
        f_pCopyOPL->wMinimumCopyLevel = f_pxmrCopyRights->CopyProtectionLevel.wValue;
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

