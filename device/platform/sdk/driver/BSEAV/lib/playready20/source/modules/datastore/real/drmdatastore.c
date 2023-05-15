/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmdatastore.h>
#include <drmhds.h>
#include <drmest.h>

ENTER_PK_NAMESPACE_CODE;


/**********************************************************************
** Function:    DRM_DST_Init
**
** Synopsis:    Initializes a Data Store Context for a given implementation type.
**
** Arguments:
**      [f_eImplementationType] - Specifies a supported implementation type.
**      [f_pDSTContext]         - Specifies the DST Context.
**      [f_cbDSTContext]        - Specifies the actual size of f_pDSTContext in bytes.
**                                The actual context may be smaller than DRM_DST_CONTEXT_LEN,
**                                depending on the implementation.
**      [f_pDST]                - Returns the initialized data store.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
** Notes:       The function MUST be called in the main thread BEFORE
**              any of the child thread(s) starting sharing the same
**              context. If this function is called after the child
**              thread(s) started, the result is undefined.
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_Init (
    __in                          DRM_DST_IMPLEMENTATION_TYPE       f_eImplementationType,
    __in_bcount( f_cbDSTContext ) DRM_DST_CONTEXT                  *f_pDSTContext,
    __in                          DRM_DWORD                         f_cbDSTContext,
    __out                         DRM_DST                          *f_pDST ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_Init (
    __in                          DRM_DST_IMPLEMENTATION_TYPE       f_eImplementationType,
    __in_bcount( f_cbDSTContext ) DRM_DST_CONTEXT                  *f_pDSTContext,
    __in                          DRM_DWORD                         f_cbDSTContext,
    __out                         DRM_DST                          *f_pDST )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkArg( f_pDSTContext != NULL );

    f_pDST->poContextDST = f_pDSTContext;

    switch ( f_eImplementationType )
    {
    case eDRM_DST_HDS:
        ChkArg( f_cbDSTContext >= DRM_HDS_CONTEXT_LEN );
        ChkDR( DRM_HDS_InitializeDST( f_pDST ) );
        break;
    case eDRM_DST_EMBEDDED:
        ChkArg( f_cbDSTContext >= DRM_EST_CONTEXT_LEN );
        ChkDR( DRM_EST_InitializeDST( f_pDST ) );
        break;
    case eDRM_DST_NONE:
    default:
        return DRM_E_NOTIMPL;
    }

    ChkBOOL( f_pDST->oDataStoreInterface.pfnInit != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnInit( f_pDST->poContextDST ) );
ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_Uninit
**
** Synopsis:    Uninitializes a Data Store.
**
** Arguments:
**      [f_pDST] - Specifies the data store to uninitialize.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.

**
** Notes:       This function MUST be called in the main thread AFTER
**              any of the child thread(s) sharing the same context
**              stopped. If this function is called before the child
**              thread(s) stopped, the result is undefined.
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_Uninit ( __inout DRM_DST *f_pDST ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_Uninit ( __inout DRM_DST *f_pDST )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnUninit != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnUninit( f_pDST->poContextDST ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CreateStore
**
** Synopsis:    Creates a data store.
**
** Arguments:
**      [f_pvOEMContext]   - Optional pointer to OEM-specific context.
**      [f_pbStoreData]    - Specifies store initialization data.
**      [f_cbStoreData]    - Specifies the length of f_pbStoreData.
**      [f_dwBlockSize]    - Specifies the block size for the store.
**      [f_eBlkNumType]    - Specifies a block numbering type for the store.
**      [f_fTruncateExist] - Specifies whether to truncate an existing store at this location.
**      [f_dwInitSizeInKB] - Specifies the initial size of the data store in KB.
**      [f_pDST]           - Returns the updated data store.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CreateStore (
    __in_opt                           DRM_VOID          *f_pbOEMContext,
    __in_bcount( f_cbStoreData ) const DRM_BYTE          *f_pbStoreData,
    __in                               DRM_DWORD          f_cbStoreData,
    __in                         const DRM_DWORD          f_dwBlockSize,
    __in                               DRM_DST_BLKNUMTYPE f_eBlNumType,
    __in                               DRM_BOOL           f_fTruncateExist,
    __in                         const DRM_DWORD          f_dwInitSizeInKB,
    __inout                            DRM_DST            *f_pDST ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CreateStore (
    __in_opt                           DRM_VOID          *f_pbOEMContext,
    __in_bcount( f_cbStoreData ) const DRM_BYTE          *f_pbStoreData,
    __in                               DRM_DWORD          f_cbStoreData,
    __in                         const DRM_DWORD          f_dwBlockSize,
    __in                               DRM_DST_BLKNUMTYPE f_eBlNumType,
    __in                               DRM_BOOL           f_fTruncateExist,
    __in                         const DRM_DWORD          f_dwInitSizeInKB,
    __inout                            DRM_DST            *f_pDST )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCreateStore != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCreateStore( f_pbOEMContext,
                                                       f_pbStoreData,
                                                       f_cbStoreData,
                                                       f_dwBlockSize,
                                                       f_eBlNumType,
                                                       f_fTruncateExist,
                                                       f_dwInitSizeInKB,
                                                       f_pDST->poContextDST ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_OpenStore
**
** Synopsis:    Opens a previously created data store.
**
** Arguments:
**      [f_pOEMContext]      - Pointer to OEM-specific context data.
**      [f_pbStoreData]      - Specifies store initialization data.
**      [f_cbStoreData]      - Specifies the length of f_pbStoreData.
**      [f_dwGrowBySizeinKB] - Specifies the number of bytes to grow the store
**                             during resizing.
**      [f_pDST]             - Returns the updated data store.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_OpenStore (
    __in_opt                     DRM_VOID   *f_pOEMContext,
    __in_bcount( f_cbStoreData ) DRM_BYTE   *f_pbStoreData,
    __in                         DRM_DWORD   f_cbStoreData,
    __in                         DRM_DWORD   f_dwGrowBySizeinKB,
    __inout                      DRM_DST    *f_pDST ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_OpenStore (
    __in_opt                     DRM_VOID   *f_pOEMContext,
    __in_bcount( f_cbStoreData ) DRM_BYTE   *f_pbStoreData,
    __in                         DRM_DWORD   f_cbStoreData,
    __in                         DRM_DWORD   f_dwGrowBySizeinKB,
    __inout                      DRM_DST    *f_pDST )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnOpenStore != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnOpenStore( f_pOEMContext,
                                                     f_pbStoreData,
                                                     f_cbStoreData,
                                                     f_dwGrowBySizeinKB,
                                                     f_pDST->poContextDST ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CloseStore
**
** Synopsis:    Close the data store.  This will also Commit before close.
**
** Arguments:
**      [f_pDST] - Specifies the data store to close.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CloseStore ( __inout DRM_DST *f_pDST ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CloseStore ( __inout DRM_DST *f_pDST )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCloseStore != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCloseStore( f_pDST->poContextDST ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CommitStore
**
** Synopsis:    Flushes unwritten data to the store.
**
** Arguments:
**      [f_pDST] - Specifies the data store to flush.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DST_CommitStore ( __inout DRM_DST *f_pDST )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCommitStore != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCommitStore( f_pDST->poContextDST ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CleanupStore
**
** Synopsis:    Performs cleanup on a data store.
**
** Arguments:
**      [f_pDST]    - Specifies the data store to clean.
**      [f_fWait]   - Specifies whether to wait for locked slots to unlock.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DST_CleanupStore (
    __inout DRM_DST   *f_pDST,
    __in    DRM_BOOL   f_fWait )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCleanupStore != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCleanupStore( f_pDST->poContextDST, f_fWait ) );

ErrorExit:
    return dr;
}


/*
** ----------------------------------------------------------------------------
** Methods applied to namespaces within the data store.
** ----------------------------------------------------------------------------
*/

/**********************************************************************
** Function:    DRM_DST_OpenNamespace
**
** Synopsis:    Opens a specified namespace within a data store.
**
** Arguments:
**      [f_pDST]               - Specifies the data store.
**      [f_pbNamespace]        - Namespace identifier to open.
**      [f_cbNamespace]        - Specifies the length in bytes of f_pbNamespace.
**      [f_eOpenMode]          - Specifies the open mode.
**      [f_wMaxNumChildren]    - Specifies the number of children to create if a new
**                               namespace is to be created.
**      [f_dwExpectedSlotSize] - Expected size of bytes in slots for this namespace.
**      [f_pContextNS]         - Returns the context for the opened namespace.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_OpenNamespace (
    __inout        DRM_DST                   *f_pDST,
    __in    const  DRM_DST_NAMESPACE         *f_pNamespace,
    __in           DRM_DST_OPENMODE           f_eOpenMode,
    __in           DRM_WORD                   f_wMaxNumChildren,
    __in           DRM_DWORD                  f_dwExpectedSlotSize,
    __out          DRM_DST_NAMESPACE_CONTEXT *f_pContextNS ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_OpenNamespace (
    __inout        DRM_DST                   *f_pDST,
    __in    const  DRM_DST_NAMESPACE         *f_pNamespace,
    __in           DRM_DST_OPENMODE           f_eOpenMode,
    __in           DRM_WORD                   f_wMaxNumChildren,
    __in           DRM_DWORD                  f_dwExpectedSlotSize,
    __out          DRM_DST_NAMESPACE_CONTEXT *f_pContextNS )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnOpenNamespace != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnOpenNamespace( f_pDST->poContextDST,
                                                         f_pNamespace,
                                                         f_eOpenMode,
                                                         f_wMaxNumChildren,
                                                         f_dwExpectedSlotSize,
                                                         f_pContextNS ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_DeleteNamespace
**
** Synopsis:    Deletes a specified namespace and all sub-nodes from a data store.
**
** Arguments:
**      [f_pDST]   - Specifies the data store.
**      [f_pNS]    - Identifies the namespace to delete.
**      [f_fWait]  - Specifies whether to wait for locks before deletion.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DST_DeleteNamespace (
    __inout    DRM_DST           *f_pDST,
    __inout    DRM_DST_NAMESPACE *f_pNS,
    __in       DRM_BOOL           f_fWait )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnDeleteNamespace != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnDeleteNamespace( f_pDST->poContextDST,
                                                           f_pNS,
                                                           f_fWait ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CloseNamespace
**
** Synopsis:    Closes the specified namespace.
**
** Arguments:
**      [f_pDST]        - Specifies the data store.
**      [f_pContextNS]  - Specifies the context of the namespace to close.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CloseNamespace (
    __inout    DRM_DST                    *f_pDST,
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CloseNamespace (
    __inout    DRM_DST                    *f_pDST,
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCloseNamespace != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCloseNamespace( f_pContextNS ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CommitNamespace
**
** Synopsis:    Flushes open data to the specified namespace.
**
** Arguments:
**      [f_pDST]        - Specifies the data store.
**      [f_pContextNS]  - Specifies the context of the namespace to flush.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DST_CommitNamespace (
    __inout    DRM_DST                    *f_pDST,
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCommitNamespace != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCommitNamespace( f_pContextNS ) );

ErrorExit:
    return dr;
}


/*
** ----------------------------------------------------------------------------
** Methods to navigate, create, delete, open and close DST dataslots
** ----------------------------------------------------------------------------
*/

/**********************************************************************
** Function:    DRM_DST_MakeSlotHint
**
** Synopsis:    Initialize a slot hint based on the slot context passed.
**
** Arguments:
**      [f_pDST]          - Specifies the data store.
**      [f_pContextSlot]  - Specifies the slot context.
**      [f_pSlotHint]     - Returns the initialized slot hint.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_MakeSlotHint (
    __inout DRM_DST               *f_pDST,
    __in    DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out   DRM_DST_SLOT_HINT     *f_pSlotHint ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_MakeSlotHint (
    __inout DRM_DST               *f_pDST,
    __in    DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out   DRM_DST_SLOT_HINT     *f_pSlotHint )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnMakeSlotHint != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnMakeSlotHint( f_pContextSlot, f_pSlotHint ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_OpenSlot
**
** Synopsis:    Create or open a dataslot using given key1 and key2.
**
** Arguments:
**      [f_pDST]          - Specifies the data store.
**      [f_pContextNS]    - Specifies the namespace to search for a slot.
**      [f_dwMode]        - Combo of DRM_DST_OPENMODE and DRM_DSTSLOT_LOCKMODE.
**      [f_pKey1]         - Specifies the first key of the slot to open.
**      [f_pKey2]         - Specifies the second (unique) key of the slot to open.
**      [f_pSlotHint]     - Optional. Specifies a slot hint for the slot to open.
**      [f_pcbSlotSize]   - Specifies the slot context size. Returns the size of the data slot in bytes.
**      [f_pContextSlot]  - Returns the context for the opened slot.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_OpenSlot (
    __inout    DRM_DST                   *f_pDST,
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in       DRM_DWORD                  f_dwMode,
    __in const DRM_DST_KEY               *f_pKey1,
    __in const DRM_DST_KEY               *f_pKey2,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __inout    DRM_DWORD                 *f_pcbSlotSize,
    __out      DRM_DST_SLOT_CONTEXT      *f_pContextSlot ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_OpenSlot (
    __inout    DRM_DST                   *f_pDST,
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in       DRM_DWORD                  f_dwMode,
    __in const DRM_DST_KEY               *f_pKey1,
    __in const DRM_DST_KEY               *f_pKey2,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __inout    DRM_DWORD                 *f_pcbSlotSize,
    __out      DRM_DST_SLOT_CONTEXT      *f_pContextSlot )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnOpenSlot != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnOpenSlot( f_pContextNS,
                                                    f_dwMode,
                                                    f_pKey1,
                                                    f_pKey2,
                                                    f_pSlotHint,
                                                    f_pcbSlotSize,
                                                    f_pContextSlot ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_CloseSlot
**
** Synopsis:    Closes an open data slot.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextSlot] - Specifies the slot context.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CloseSlot (
    __inout  DRM_DST               *f_pDST,
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_CloseSlot (
    __inout  DRM_DST               *f_pDST,
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnCloseSlot != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnCloseSlot( f_pContextSlot ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_DeleteSlot
**
** Synopsis:    Deletes a dataslot with given key1 and key2.
**
** Arguments:
**      [f_pDST]       - Specifies the data store.
**      [f_pContextNS] - Specifies the namespace to search for the slot.
**      [f_dwMode]     - Combo of DRM_DST_OPENMODE and DRM_DSTSLOT_LOCKMODE.
**      [f_pKey1]      - Specifies the first key of the slot to delete.
**      [f_pKey2]      - Specifies the second (unique) key of the slot to delete.
**      [f_pSlotHint]  - Optional. Specifies a slot hint for the slot to delete.
**      [f_fWait]      - Specifies whether to wait for open locks before deletion.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_DeleteSlot (
    __inout    DRM_DST                   *f_pDST,
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in const DRM_DST_KEY               *f_pKey1,
    __in const DRM_DST_KEY               *f_pKey2,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __in       DRM_BOOL                   f_fWait ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_DeleteSlot (
    __inout    DRM_DST                   *f_pDST,
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in const DRM_DST_KEY               *f_pKey1,
    __in const DRM_DST_KEY               *f_pKey2,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __in       DRM_BOOL                   f_fWait )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnDeleteSlot != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnDeleteSlot( f_pContextNS,
                                                      f_pKey1,
                                                      f_pKey2,
                                                      f_pSlotHint,
                                                      f_fWait ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_InitSlotEnum
**
** Synopsis:    Initialized an enumeration of slots within a namespace.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextNS]   - Specifies the namespace to enumerate in.
**      [f_pKey1]        - Optional. Specifies the key to enumerate. If NULL, enumerates
**                         over all keys in the namespace.
**      [f_eLockMode]    - Specifies the lock mode to use for enumerated slots.
**      [f_pContextEnum] - Returns the initialized enumeration context.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_InitSlotEnum (
    __inout        DRM_DST                   *f_pDST,
    __in           DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in_opt const DRM_DST_KEY               *f_pKey1,
    __in           DRM_DST_LOCKMODE           f_eLockMode,
    __out          DRM_DST_ENUM_CONTEXT      *f_pContextEnum ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_InitSlotEnum (
    __inout        DRM_DST                   *f_pDST,
    __in           DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in_opt const DRM_DST_KEY               *f_pKey1,
    __in           DRM_DST_LOCKMODE           f_eLockMode,
    __out          DRM_DST_ENUM_CONTEXT      *f_pContextEnum )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnInitSlotEnum != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnInitSlotEnum( f_pContextNS,
                                                        f_pKey1,
                                                        f_eLockMode,
                                                        f_pContextEnum ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_SlotEnumNext
**
** Synopsis:    Retrieves the next slot from an enumeration.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextEnum] - Specifies the initialized enumeration context.
**      [f_pContextSlot] - Returns the slot data for the next slot in the enumeration.
**      [f_pKey1]        - Optional. Returns the first key value for the next slot.
**      [f_pKey2]        - Optional. Returns the second (unique) key value for the next slot.
**      [f_pcbSize]      - Optional. Returns the data slot size in bytes.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotEnumNext (
    __inout    DRM_DST               *f_pDST,
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pKey1,
    __out_opt  DRM_DST_KEY           *f_pKey2,
    __out_opt  DRM_DWORD             *f_pcbSize ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotEnumNext (
    __inout    DRM_DST               *f_pDST,
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pKey1,
    __out_opt  DRM_DST_KEY           *f_pKey2,
    __out_opt  DRM_DWORD             *f_pcbSize )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotEnumNext != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotEnumNext( f_pContextEnum,
                                                        f_pContextSlot,
                                                        f_pKey1,
                                                        f_pKey2,
                                                        f_pcbSize ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_SlotEnumReloadCurrent
**
** Synopsis:    Reload the current slot in the enumeration.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextEnum] - Specifies the initialized enumeration context.
**      [f_pContextSlot] - Returns the slot data for the current slot in the enumeration.
**      [f_pKey1]        - Optional. Returns the first key value for the slot.
**      [f_pKey2]        - Optional. Returns the second (unique) key value for the slot.
**      [f_pcbSize]      - Optional. Returns the data slot size in bytes.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotEnumReloadCurrent (
    __inout    DRM_DST               *f_pDST,
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pKey1,
    __out_opt  DRM_DST_KEY           *f_pKey2,
    __out_opt  DRM_DWORD             *f_pcbSize ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotEnumReloadCurrent (
    __inout    DRM_DST               *f_pDST,
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pKey1,
    __out_opt  DRM_DST_KEY           *f_pKey2,
    __out_opt  DRM_DWORD             *f_pcbSize )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotEnumReloadCurrent != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotEnumReloadCurrent( f_pContextEnum,
                                                                 f_pContextSlot,
                                                                 f_pKey1,
                                                                 f_pKey2,
                                                                 f_pcbSize ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_SlotEnumDeleteCurrent
**
** Synopsis:    Deletes the currently enumerated slot.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextEnum] - Specifies the initialized enumeration context.
**      [f_pContextSlot] - Specifies the slot context of the current slot.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
** Notes: The enumeration must be initialized with exclusive access.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotEnumDeleteCurrent (
    __inout   DRM_DST               *f_pDST,
    __inout   DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotEnumDeleteCurrent (
    __inout   DRM_DST               *f_pDST,
    __inout   DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotEnumDeleteCurrent != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotEnumDeleteCurrent( f_pContextEnum,
                                                                 f_pContextSlot ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_BlockScanDelete
**
** Synopsis:    Rapid deletion of multiple slots.
**
** Arguments:
**      [f_pDST]       - Specifies the data store.
**      [f_pContextNS] - Specifies the namespace context to delete from.
**      [f_rgKey1]     - Specifies a list of keys (first key) to delete.
**      [f_cKey1]      - Specifies the number of elements in f_rgKey1.
**      [f_pKey2]      - Specifies a unique key
**      [f_fWait]      - Specifies whether to wait for locks before deletion.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_BlockScanDelete (
    __inout                DRM_DST                    *f_pDST,
    __inout                DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS,
    __in_ecount( f_cKey1 ) DRM_DST_KEY                *f_rgKey1,
    __in                   DRM_DWORD                   f_cKey1,
    __in                   DRM_DST_KEY                *f_pKey2,
    __in                   DRM_BOOL                    f_fWait ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_BlockScanDelete (
    __inout                DRM_DST                    *f_pDST,
    __inout                DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS,
    __in_ecount( f_cKey1 ) DRM_DST_KEY                *f_rgKey1,
    __in                   DRM_DWORD                   f_cKey1,
    __in                   DRM_DST_KEY                *f_pKey2,
    __in                   DRM_BOOL                    f_fWait )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnBlockScanDelete != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnBlockScanDelete( f_pContextNS,
                                                           f_rgKey1,
                                                           f_cKey1,
                                                           f_pKey2,
                                                           f_fWait ) );

ErrorExit:
    return dr;
}


/*
** ----------------------------------------------------------------------------
** Methods to read, write and seek within a dataslot
** ----------------------------------------------------------------------------
*/

/**********************************************************************
** Function:    DRM_DST_SlotResize
**
** Synopsis:    Resizes an open data slot.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextSlot] - Specifies the slot context to resize.
**      [f_cbSize]       - Specifies the new size for the slot.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotResize (
    __inout  DRM_DST               *f_pDST,
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in     DRM_DWORD              f_cbSize ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotResize (
    __inout  DRM_DST               *f_pDST,
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in     DRM_DWORD              f_cbSize )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotResize != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotResize( f_pContextSlot, f_cbSize ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_SlotSeek
**
** Synopsis:    Set the seek pointer within an open data slot.
**
** Arguments:
**      [f_pDST]          - Specifies the data store.
**      [f_pContextSlot]  - Specifies the slot context.
**      [f_lOffset]       - Specifies the seek offset
**      [f_eSeekMode]     - Specifies the seek mode to use.
**      [f_pdwSeekPointer]- Optional. Returns the new seek pointer value.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotSeek (
    __inout   DRM_DST               *f_pDST,
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in      DRM_LONG               f_lOffset,
    __in      DRM_DST_SEEKMODE       f_eSeekMode,
    __out_opt DRM_DWORD             *f_pdwSeekPointer ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotSeek (
    __inout   DRM_DST               *f_pDST,
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in      DRM_LONG               f_lOffset,
    __in      DRM_DST_SEEKMODE       f_eSeekMode,
    __out_opt DRM_DWORD             *f_pdwSeekPointer )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotSeek != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotSeek( f_pContextSlot,
                                                    f_lOffset,
                                                    f_eSeekMode,
                                                    f_pdwSeekPointer ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_SlotRead
**
** Synopsis:    Read data from an open data slot.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextSlot] - Specifies the slot context.
**      [f_cbData]       - Specifies the size in bytes to read.
**      [f_pbData]       - The data buffer to read into.
**      [f_pcbRead]      - Returns the number of bytes read.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotRead (
    __inout                  DRM_DST               *f_pDST,
    __inout                  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                     DRM_DWORD              f_cbData,
    __out_bcount( f_cbData ) DRM_BYTE              *f_pbData,
    __out_opt                DRM_DWORD             *f_pcbRead ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotRead (
    __inout                  DRM_DST               *f_pDST,
    __inout                  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                     DRM_DWORD              f_cbData,
    __out_bcount( f_cbData ) DRM_BYTE              *f_pbData,
    __out_opt                DRM_DWORD             *f_pcbRead )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotRead != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotRead( f_pContextSlot,
                                                    f_cbData,
                                                    f_pbData,
                                                    f_pcbRead ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_DST_SlotWrite
**
** Synopsis:    Write data from an open and locked data slot.
**
** Arguments:
**      [f_pDST]         - Specifies the data store.
**      [f_pContextSlot] - Specifies the slot context.
**      [f_cbData]       - Specifies the size in bytes to write.
**      [f_pbData]       - The data buffer to write from.
**      [f_pcbWritten]   - Returns the number of bytes written.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotWrite (
    __inout                       DRM_DST               *f_pDST,
    __inout                       DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                          DRM_DWORD              f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __out_opt                     DRM_DWORD             *f_pcbWritten ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DST_SlotWrite (
    __inout                       DRM_DST               *f_pDST,
    __inout                       DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                          DRM_DWORD              f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __out_opt                     DRM_DWORD             *f_pcbWritten )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDST != NULL );
    ChkBOOL( f_pDST->oDataStoreInterface.pfnSlotWrite != NULL, DRM_E_INVALIDARG );
    ChkDR( f_pDST->oDataStoreInterface.pfnSlotWrite( f_pContextSlot,
                                                     f_cbData,
                                                     f_pbData,
                                                     f_pcbWritten ) );

ErrorExit:
    return dr;
}
EXIT_PK_NAMESPACE_CODE;
