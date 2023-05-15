/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMESTIMPLREAL_C
#include <drmest.h>
#include <drmembeddedstore_impl.h>
#include <drmerr.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* Static functions. */

/**********************************************************************
**
** Function :   _EST_PrepareUnusedBuffer
**
** Synopsis :   Function that initializes two EST context fields that manage
**              a buffer at the end of the buffer available for new slots.
**              The two fields are m_ibUnused and m_cbUnused.
**
** Arguments :
** [f_pESTContext]       : Pointer to an EST context.
**
** Returns :
**      DRM_SUCCESS      - Success.
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _EST_PrepareUnusedBuffer(
    __in DRM_EST_CONTEXT *f_pContextEST )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD ibCur           = DRM_EST_HEADER_LEN;
    DRM_DWORD cbSlot          = 0;
    DRM_BOOL fFound = FALSE;

    ChkArg( f_pContextEST != NULL );

    /* Search for the a slot with the first DWORD as 0. */
    while ( ibCur < f_pContextEST->m_cbEmbeddedStore )
    {
        NETWORKBYTES_TO_DWORD( cbSlot, f_pContextEST->m_pbEmbeddedStore, ibCur );

        if ( cbSlot == 0 )
        {
            /* A 0 size slot indicates the start of the unused buffer. */
            f_pContextEST->m_ibUnused = ibCur;
            f_pContextEST->m_cbUnused = f_pContextEST->m_cbEmbeddedStore - ibCur;

            fFound = TRUE;
            break;
        }
        else
        {
            ChkBOOL( cbSlot <= MAX_EMBEDDED_STORE_LEN, DRM_E_DST_CORRUPTED );
            ChkBOOL( cbSlot > DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
            /*
            ** In case cbSlot is corrupt and very large, we don't want to integer overflow
            ** So we use subtraction (A<=B-C) instead of addition (A+C<=B).
            ** We know we won't UNDERFLOW in subtraction because we verified B>C above.
            */
            ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
            ChkBOOL( PADDED_LEN(cbSlot) <= f_pContextEST->m_cbEmbeddedStore - ibCur, DRM_E_DST_CORRUPTED );

            ibCur += PADDED_LEN(cbSlot) ;
        }
    }

    if ( !fFound )
    {
        /* The whole EST buffer has already been used. */
        f_pContextEST->m_ibUnused = f_pContextEST->m_cbEmbeddedStore;
        f_pContextEST->m_cbUnused = 0;
    }

ErrorExit:

    return dr;
}

/**********************************************************************
**
** Function :   _EST_PrepareSlotToBeReturned
**
** Synopsis :   Funtion that fills fields of the passed in EST slot context
**              with information of a cached EST slot. Other optional output
**              parameters are filled by the function.
**
** Arguments :
** [f_pESTNSContext]     : Pointer to an EST namespace context.
** [f_pESTContext]       : Pointer to an EST context.
** [f_pSlotHint]         : Pointer to an EST slot hint.
** [f_eLockMode]         : Lock mode of the slot.
** [f_pESTSlotContext]   : Pointer to an EST slot context to be filled.
** [f_pKey1]             : Pointer to an optional buffer to receive the
**                         primary key of the cached EST slot.
** [f_pKey2]             : Pointer to an optional buffer to receive the
**                         secondary key of the cached EST slot.
** [f_pcbSize]           : Pointer to an optional variable to receive the
**                         size of the cached EST slot (no padding)
**
** Returns :
**      DRM_SUCCESS      - Success.
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _EST_PrepareSlotToBeReturned(
    __in            DRM_EST_NAMESPACE_CONTEXT *f_pESTNSContext,
    __in      const DRM_EST_CONTEXT           *f_pESTContext,
    __in            DRM_EST_SLOT_HINT         *f_pSlotHint,
    __in            DRM_DST_LOCKMODE           f_eLockMode,
    __out           DRM_EST_SLOT_CONTEXT      *f_pESTSlotContext,
    __out_opt       DRM_DST_KEY               *f_pKey1,
    __out_opt       DRM_DST_KEY               *f_pKey2,
    __out_opt       DRM_DWORD                 *f_pcbSize )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD ibCur           = DRM_EST_HEADER_LEN;
    DRM_DWORD cbSlot          = 0;

    ChkArg( f_pESTNSContext != NULL );
    ChkArg( f_pESTContext != NULL );
    ChkArg( f_pSlotHint != NULL );
    ChkArg( f_pESTSlotContext != NULL );

    DRMASSERT( f_pSlotHint->m_ibOffset >= DRM_EST_HEADER_LEN );
    DRMASSERT( f_pSlotHint->m_cbSlotLen > DRM_EST_SLOTHEADER_LEN );
    DRMASSERT( f_pSlotHint->m_ibOffset +
               PADDED_LEN(f_pSlotHint->m_cbSlotLen) <= f_pESTContext->m_ibUnused );

    ZEROMEM( f_pESTSlotContext, SIZEOF( DRM_EST_SLOT_CONTEXT ) );

    ibCur = f_pSlotHint->m_ibOffset;
    cbSlot = f_pSlotHint->m_cbSlotLen;

    ChkArg( ibCur >= DRM_EST_HEADER_LEN && ibCur < f_pESTContext->m_ibUnused );
    /*
    ** In case cbSlot is corrupt and very large, we don't want to integer overflow
    ** So we use subtraction (A<=B-C) instead of addition (A+C<=B).
    ** We know we won't UNDERFLOW in subtraction because we verified B>C above.
    */
    ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
    ChkArg( PADDED_LEN(cbSlot) <= f_pESTContext->m_ibUnused - ibCur );

    /* Skip the slot length field. */
    ibCur += SIZEOF( DRM_DWORD );

    /* Copy the primary key of the cached slot. */
    DRM_BYT_CopyBytes( ( DRM_BYTE * )&f_pESTSlotContext->m_oKey1,
                       0,
                       f_pESTContext->m_pbEmbeddedStore,
                       ibCur,
                       SIZEOF( DRM_DST_KEY ) );
    ibCur += SIZEOF( DRM_DST_KEY );

    /* Copy the secondary key of the cached slot. */
    DRM_BYT_CopyBytes( ( DRM_BYTE * )&f_pESTSlotContext->m_oKey2,
                       0,
                       f_pESTContext->m_pbEmbeddedStore,
                       ibCur,
                       SIZEOF( DRM_DST_KEY ) );
    ibCur += SIZEOF( DRM_DST_KEY );

    /* All locks are shared until locking is implemented fully. */
    f_pESTSlotContext->m_eLockmode = eDRM_DST_LOCKSHARED;

    MEMCPY( ( DRM_BYTE * )&f_pESTSlotContext->m_oSlotLocation,
            ( DRM_BYTE * )f_pSlotHint,
            SIZEOF( DRM_EST_SLOT_HINT ) );

    f_pESTSlotContext->m_eLockmode = f_eLockMode;
    f_pESTSlotContext->m_fSlotOpen = TRUE;
    f_pESTSlotContext->m_poESTNSContext = f_pESTNSContext;

    /* Copy information to the optional output parameters. */
    if ( f_pKey1 != NULL )
    {
        MEMCPY( ( DRM_BYTE * )f_pKey1,
                ( DRM_BYTE * )&f_pESTSlotContext->m_oKey1,
                SIZEOF( DRM_DST_KEY ) );
    }

    if ( f_pKey2 != NULL )
    {
        MEMCPY( ( DRM_BYTE * )f_pKey2,
                ( DRM_BYTE * )&f_pESTSlotContext->m_oKey2,
                SIZEOF( DRM_DST_KEY ) );
    }

    if ( f_pcbSize != NULL )
    {
        *f_pcbSize = f_pESTSlotContext->m_oSlotLocation.m_cbSlotLen -
                     DRM_EST_SLOTHEADER_LEN;
    }

ErrorExit:

    return dr;
}

/**********************************************************************
**
** Function :   _EST_OpenSlot
**
** Synopsis :   Attempts to open an existing slot.
**
** Arguments :
**      [f_pContextEST]  : Specifies the EST context.
**      [f_dwMode]       : Specifies the mode (only lock mode is used)
**      [f_pKey1]        : Specifies the optional primary key of the slot to open.
**      [f_pKey2]        : Specifies the optional secondary key of the slot to open.
**      [f_pSlotHint]    : Optional. Contains a slot hint for the slot to open.
**                         Not used here.
**      [f_pContextSlot] : Returns the slot context.
**
** Returns :
**      DRM_SUCCESS      - Success.
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _EST_OpenSlot(
    __in     const DRM_EST_CONTEXT           *f_pContextEST,
    __in           DRM_DWORD                  f_dwMode,
    __in_opt const DRM_DST_KEY               *f_pKey1,
    __in_opt const DRM_DST_KEY               *f_pKey2,
    __in_opt       DRM_EST_SLOT_HINT         *f_pSlotHint,
    __out          DRM_EST_SLOT_CONTEXT      *f_pContextSlot )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD ibCur           = DRM_EST_HEADER_LEN;
    DRM_DWORD cbSlot          = 0;
    DRM_BOOL fFound           = FALSE;

    ChkArg( f_pContextEST != NULL );
    ChkArg( f_pContextSlot != NULL );

    /* Search all slots ahead of the unused buffer. */
    while ( ibCur < f_pContextEST->m_ibUnused )
    {
        NETWORKBYTES_TO_DWORD( cbSlot, f_pContextEST->m_pbEmbeddedStore, ibCur );
        ChkBOOL( cbSlot <= MAX_EMBEDDED_STORE_LEN, DRM_E_DST_CORRUPTED );
        ChkBOOL( cbSlot > DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
        /*
        ** In case cbSlot is corrupt and very large, we don't want to integer overflow
        ** So we use subtraction (A<=B-C) instead of addition (A+C<=B).
        ** We know we won't UNDERFLOW in subtraction because we verified B>C above.
        */
        ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
        ChkBOOL( PADDED_LEN(cbSlot) <= f_pContextEST->m_ibUnused - ibCur, DRM_E_DST_CORRUPTED );

        ibCur += SIZEOF( DRM_DWORD );

        if ( f_pKey1 != NULL )
        {
            /*
            ** If the passed in primary key is not NULL, check it against
            ** the current slot being examined.
            */
            if ( DRM_BYT_CompareBytes( f_pContextEST->m_pbEmbeddedStore,
                                       ibCur,
                                       ( DRM_BYTE * )f_pKey1,
                                       0,
                                       SIZEOF( DRM_DST_KEY ) ) != 0 )
            {
                /* Skip the two keys. */
                ibCur += 2 * SIZEOF( DRM_DST_KEY );

                /* Skip the data portion of the slot. */
                ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
                ibCur += ( PADDED_LEN(cbSlot) - DRM_EST_SLOTHEADER_LEN );

                continue;
            }
        }

        /* Cache the primary key of the slot being examined. */
        DRM_BYT_CopyBytes( ( DRM_BYTE * )&f_pContextSlot->m_oKey1,
                           0,
                           f_pContextEST->m_pbEmbeddedStore,
                           ibCur,
                           SIZEOF( DRM_DST_KEY ) );

        /* Skip the primary key. */
        ibCur += SIZEOF( DRM_DST_KEY );

        if ( f_pKey2 != NULL )
        {
            /*
            ** If the passed in secondary key is not NULL, check it against
            ** the current slot being examined.
            */
            if ( DRM_BYT_CompareBytes( f_pContextEST->m_pbEmbeddedStore,
                                       ibCur,
                                       ( DRM_BYTE * )f_pKey2,
                                       0,
                                       SIZEOF( DRM_DST_KEY ) ) != 0 )
            {
                /* Skip the secondary key. */
                ibCur += SIZEOF( DRM_DST_KEY );

                /* Skip the data portion of the slot. */
                ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
                ibCur += ( PADDED_LEN(cbSlot) - DRM_EST_SLOTHEADER_LEN );

                continue;
            }
        }

        /* Cache the secondary key of the slot being examined. */
        DRM_BYT_CopyBytes( ( DRM_BYTE * )&f_pContextSlot->m_oKey2,
                           0,
                           f_pContextEST->m_pbEmbeddedStore,
                           ibCur,
                           SIZEOF( DRM_DST_KEY ) );

        /* Skip the secondary key. */
        ibCur += SIZEOF( DRM_DST_KEY );

        fFound = TRUE;
        break;
    }

    if ( fFound )
    {
        /* Found the requested slot. Copy data into the slot context. */

        /* All locks are shared until locking is implemented fully */
        f_pContextSlot->m_eLockmode = eDRM_DST_LOCKSHARED;

        /* Write the location (starting index and size) of the slot within the overall EST buffer. */
        f_pContextSlot->m_oSlotLocation.m_ibOffset = ibCur - DRM_EST_SLOTHEADER_LEN;
        f_pContextSlot->m_oSlotLocation.m_cbSlotLen = cbSlot;

        f_pContextSlot->m_fSlotOpen = TRUE;
    }
    else
    {
        ChkDR( DRM_E_DST_SLOT_NOT_FOUND );
    }

ErrorExit:

    return dr;
}

/**********************************************************************
**
** Function :   _EST_CreateSlot
**
** Synopsis :   Attempts to create a new slot.
**
** Arguments :
**      [f_pContextEST]  : Specifies the EST context.
**      [f_dwMode]       : Specifies the mode (only lock mode is used)
**      [f_pKey1]        : Specifies the first key of the slot to create.
**      [f_pKey2]        : Specifies the second key of the slot to create.
**      [f_cbSize]       : Specifies the desired size for the slot.
**      [f_pContextSlot] : Returns the slot context.
**
** Returns :
**      DRM_SUCCESS      - Success.
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _EST_CreateSlot(
    __in        DRM_EST_CONTEXT           *f_pContextEST,
    __in        DRM_DWORD                  f_dwMode,
    __in const  DRM_DST_KEY               *f_pKey1,
    __in const  DRM_DST_KEY               *f_pKey2,
    __in        DRM_DWORD                  f_cbSize,
    __out       DRM_EST_SLOT_CONTEXT      *f_pContextSlot )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD ibCur           = 0;
    DRM_DWORD cbSlot          = 0;

    ChkArg( f_pContextEST != NULL );
    ChkArg( f_pKey1 != NULL );
    ChkArg( f_pKey2 != NULL );
    ChkArg( f_cbSize > 0 );
    ChkArg( f_pContextSlot != NULL );

    /*
    ** The size of the unused EST buffer should be big enough to hold the requested slot size plus
    ** a fixed slot header.
    */
    ChkOverflow( PADDED_LEN(f_cbSize), f_cbSize );
    ChkOverflow( PADDED_LEN(f_cbSize) + DRM_EST_SLOTHEADER_LEN, PADDED_LEN(f_cbSize) );
    ChkBOOL( f_pContextEST->m_cbUnused >=
             PADDED_LEN(f_cbSize) + DRM_EST_SLOTHEADER_LEN, DRM_E_DST_STORE_FULL );

    ibCur = f_pContextEST->m_ibUnused;
    cbSlot = f_cbSize + DRM_EST_SLOTHEADER_LEN;

    /* Initialize the underlying buffer of the whole slot to 0, padding bytes included. */
    ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
    DRM_BYT_SetBytes( f_pContextEST->m_pbEmbeddedStore, ibCur, PADDED_LEN(cbSlot), 0 );

    /* Write the length of the slot. */
    DWORD_TO_NETWORKBYTES( f_pContextEST->m_pbEmbeddedStore, ibCur, cbSlot );
    ibCur += SIZEOF( DRM_DWORD );

    /* Write the primary key of the slot. */
    DRM_BYT_CopyBytes( f_pContextEST->m_pbEmbeddedStore,
                       ibCur,
                       ( DRM_BYTE * )f_pKey1,
                       0,
                       SIZEOF( DRM_DST_KEY ) );
    ibCur += SIZEOF( DRM_DST_KEY );

    /* Write the secondary key of the slot. */
    DRM_BYT_CopyBytes( f_pContextEST->m_pbEmbeddedStore,
                       ibCur,
                       ( DRM_BYTE * )f_pKey2,
                       0,
                       SIZEOF( DRM_DST_KEY ) );
    ibCur += SIZEOF( DRM_DST_KEY );

    /* Initialize the slot context provided by the caller. */
    MEMCPY( ( DRM_BYTE * )&f_pContextSlot->m_oKey1,
            ( DRM_BYTE * )f_pKey1,
            SIZEOF( DRM_DST_KEY ) );

    MEMCPY( ( DRM_BYTE * )&f_pContextSlot->m_oKey2,
            ( DRM_BYTE * )f_pKey2,
            SIZEOF( DRM_DST_KEY ) );

    /* All locks are shared until locking is implemented fully. */
    f_pContextSlot->m_eLockmode = eDRM_DST_LOCKSHARED;

    /* Write the location (starting index and size) of the slot within the overall EST buffer. */
    f_pContextSlot->m_oSlotLocation.m_ibOffset = ibCur - DRM_EST_SLOTHEADER_LEN;
    f_pContextSlot->m_oSlotLocation.m_cbSlotLen = cbSlot;

    /* Reduce the size of the unused EST buffer. */
    ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
    f_pContextEST->m_ibUnused += PADDED_LEN(cbSlot);
    f_pContextEST->m_cbUnused -= PADDED_LEN(cbSlot);
    DRMASSERT( f_pContextEST->m_ibUnused + f_pContextEST->m_cbUnused ==
               f_pContextEST->m_cbEmbeddedStore );

    f_pContextEST->m_fIsDirty = TRUE;

ErrorExit:

    return dr;
}

/**********************************************************************
**
** Function :   DRM_EST_InitializeDST
**
** Synopsis :   Initializes function pointers and other members of the DST
**              structure for use with an EST implementation.
**
** Arguments :
**      [f_pDst] : Returns the initialized DST structure.
**
** Returns :
**      DRM_SUCCESS      - Success.
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_InitializeDST(
    __out DRM_DST *f_pDst )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDst != NULL );

    /*
    ** Context size verification
    */
    DRMCASSERT( SIZEOF( DRM_EST_CONTEXT )           <= SIZEOF( DRM_DST_CONTEXT ) );
    DRMCASSERT( SIZEOF( DRM_EST_NAMESPACE_CONTEXT ) <= SIZEOF( DRM_DST_NAMESPACE_CONTEXT ) );
    DRMCASSERT( SIZEOF( DRM_EST_SLOT_CONTEXT )      <= SIZEOF( DRM_DST_SLOT_CONTEXT ) );
    DRMCASSERT( SIZEOF( DRM_EST_SLOT_HINT )         <= SIZEOF( DRM_DST_SLOT_HINT ) );
    DRMCASSERT( SIZEOF( DRM_EST_ENUM_CONTEXT )      <= SIZEOF( DRM_DST_ENUM_CONTEXT ) );

    /*
    ** Slot header is guaranteed to be 32-bit aligned
    */
    DRMCASSERT( PAD_AMOUNT(DRM_EST_SLOTHEADER_LEN) == 0 );

    f_pDst->eImplementationType                          = eDRM_DST_EMBEDDED;
    f_pDst->oDataStoreInterface.pfnInit                  = DRM_EST_Init;
    f_pDst->oDataStoreInterface.pfnUninit                = DRM_EST_Uninit;
    f_pDst->oDataStoreInterface.pfnCreateStore           = DRM_EST_CreateStore;
    f_pDst->oDataStoreInterface.pfnOpenStore             = DRM_EST_OpenStore;
    f_pDst->oDataStoreInterface.pfnCloseStore            = DRM_EST_CloseStore;
    f_pDst->oDataStoreInterface.pfnCommitStore           = DRM_EST_CommitStore;
    f_pDst->oDataStoreInterface.pfnCleanupStore          = DRM_EST_CleanupStore;
    f_pDst->oDataStoreInterface.pfnOpenNamespace         = DRM_EST_OpenNamespace;
    f_pDst->oDataStoreInterface.pfnDeleteNamespace       = DRM_EST_DeleteNamespace;
    f_pDst->oDataStoreInterface.pfnCloseNamespace        = DRM_EST_CloseNamespace;
    f_pDst->oDataStoreInterface.pfnCommitNamespace       = DRM_EST_CommitNamespace;
    f_pDst->oDataStoreInterface.pfnMakeSlotHint          = DRM_EST_MakeSlotHint;
    f_pDst->oDataStoreInterface.pfnOpenSlot              = DRM_EST_OpenSlot;
    f_pDst->oDataStoreInterface.pfnCloseSlot             = DRM_EST_CloseSlot;
    f_pDst->oDataStoreInterface.pfnDeleteSlot            = DRM_EST_DeleteSlot;
    f_pDst->oDataStoreInterface.pfnInitSlotEnum          = DRM_EST_InitSlotEnum;
    f_pDst->oDataStoreInterface.pfnSlotEnumNext          = DRM_EST_SlotEnumNext;
    f_pDst->oDataStoreInterface.pfnSlotEnumReloadCurrent = DRM_EST_SlotEnumReloadCurrent;
    f_pDst->oDataStoreInterface.pfnSlotEnumDeleteCurrent = DRM_EST_SlotEnumDeleteCurrent;
    f_pDst->oDataStoreInterface.pfnBlockScanDelete       = DRM_EST_BlockScanDelete;
    f_pDst->oDataStoreInterface.pfnSlotResize            = DRM_EST_SlotResize;
    f_pDst->oDataStoreInterface.pfnSlotSeek              = DRM_EST_SlotSeek;
    f_pDst->oDataStoreInterface.pfnSlotRead              = DRM_EST_SlotRead;
    f_pDst->oDataStoreInterface.pfnSlotWrite             = DRM_EST_SlotWrite;
    f_pDst->oDataStoreInterface.pfnSlotReadPtr           = DRM_EST_SlotReadPtr;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_Init
**
** Synopsis:    Initializes an embedded store context.
**
** Arguments:
**      [f_pDSTContext]         - Specifies the DST Context.
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_Init (
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;

    ChkArg( f_pContextDST != NULL );

    poESTContext->m_fInited         = TRUE;
    poESTContext->m_fStoreOpen      = FALSE;
    poESTContext->m_fIsDirty        = FALSE;
    poESTContext->m_cbEmbeddedStore = 0;
    poESTContext->m_pbEmbeddedStore = NULL;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_Uninit
**
** Synopsis:    Uninitializes an Embedded Data Store context.
**
** Arguments:
**      [f_pContextDST] - Specifies the data store context to uninitialize.
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_Uninit (
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;

    ChkArg( f_pContextDST != NULL );

    poESTContext->m_fInited         = FALSE;
    poESTContext->m_fStoreOpen      = FALSE;
    poESTContext->m_fIsDirty        = FALSE;
    poESTContext->m_cbEmbeddedStore = 0;
    poESTContext->m_pbEmbeddedStore = NULL;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CreateStore
**
** Synopsis:    Creates an embedded data store.
**
** Arguments:
**      [f_pvOEMContext]   - Optional pointer to OEM-specific context.
**      [f_pbStoreData]    - Specifies store initialization data.
**      [f_cbStoreData]    - Specifies the length of f_pbStoreData.
**      [f_dwBlockSize]    - Specifies the block size for the store.
**      [f_eBlkNumType]    - Specifies a block numbering type for the store.
**      [f_fTruncateExist] - Specifies whether to truncate an existing store at this location.
**      [f_dwInitSizeInKB] - Specifies the initial size of the data store in KB.
**      [f_pContextDST]    - Returns the updated data store context.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CreateStore (
    __in_opt                           DRM_VOID          *f_pbOEMContext,
    __in_bcount( f_cbStoreData ) const DRM_BYTE          *f_pbStoreData,
    __in                               DRM_DWORD          f_cbStoreData,
    __in                         const DRM_DWORD          f_dwBlockSize,
    __in                               DRM_DST_BLKNUMTYPE f_eBlNumType,
    __in                               DRM_BOOL           f_fTruncateExist,
    __in                         const DRM_DWORD          f_dwInitSizeInKB,
    __inout                            DRM_DST_CONTEXT   *f_pContextDST )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;
    DRM_WORD         wCurVersion  = DRM_EST_CURRENT_VERSION;
    DRM_WORD         wMinVersion  = DRM_EST_MINIMUM_VERSION;

    ChkArg( f_pContextDST != NULL );
    ChkArg( poESTContext->m_fInited );
    ChkArg( f_pbStoreData != NULL );

    ChkBOOL( f_cbStoreData > DRM_EST_HEADER_LEN, DRM_E_BUFFERTOOSMALL );

    /*
    ** Check for an existing store
    */
    ChkBOOL( f_fTruncateExist || MEMCMP( f_pbStoreData, DRM_EST_HEADER_CONST, DRM_EST_HEADER_CONST_LEN ) != 0,
             DRM_E_DST_EXISTS );

    poESTContext->m_fStoreOpen      = TRUE;
    poESTContext->m_fIsDirty        = TRUE;
    poESTContext->m_cbEmbeddedStore = f_cbStoreData;
    poESTContext->m_pbEmbeddedStore = ( DRM_BYTE * )f_pbStoreData;

    if ( f_fTruncateExist )
    {
        ZEROMEM( poESTContext->m_pbEmbeddedStore, poESTContext->m_cbEmbeddedStore );
    }

    /*
    ** Initialize the embedded store
    */
    MEMCPY( poESTContext->m_pbEmbeddedStore, DRM_EST_HEADER_CONST, DRM_EST_HEADER_CONST_LEN );
    WORD_TO_NETWORKBYTES( poESTContext->m_pbEmbeddedStore, DRM_EST_HEADER_CONST_LEN, wCurVersion );
    WORD_TO_NETWORKBYTES( poESTContext->m_pbEmbeddedStore, DRM_EST_HEADER_CONST_LEN + SIZEOF( DRM_WORD ), wMinVersion );
    DWORD_TO_NETWORKBYTES( poESTContext->m_pbEmbeddedStore, DRM_EST_HEADER_CONST_LEN + 2 * SIZEOF( DRM_WORD ), f_cbStoreData );

    ChkDR( _EST_PrepareUnusedBuffer( poESTContext ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_OpenStore
**
** Synopsis:    Opens a previously created embedded store.
**
** Arguments:
**      [f_pOEMContext]      - Pointer to OEM-specific context data.
**      [f_pbStoreData]      - Specifies store initialization data.
**      [f_cbStoreData]      - Specifies the length of f_pbStoreData.
**      [f_dwGrowBySizeinKB] - Specifies the number of bytes to grow the store
**                             during resizing.
**      [f_pContextDST]      - Returns the updated data store context.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_OpenStore (
    __in_opt                     DRM_VOID        *f_pOEMContext,
    __in_bcount( f_cbStoreData ) DRM_BYTE        *f_pbStoreData,
    __in                         DRM_DWORD        f_cbStoreData,
    __in                         DRM_DWORD        f_dwGrowBySizeinKB,
    __inout                      DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;
    DRM_DWORD        cbExpected   = 0;
    DRM_WORD         wCurVersion  = 0;
    DRM_WORD         wMinVersion  = 0;

    ChkArg( f_pContextDST != NULL );
    ChkArg( poESTContext->m_fInited );
    ChkArg( f_pbStoreData != NULL );

    ChkBOOL( f_cbStoreData > DRM_EST_HEADER_LEN, DRM_E_BUFFERTOOSMALL );

    /*
    ** Verify store header
    */
    ChkBOOL( MEMCMP( f_pbStoreData, DRM_EST_HEADER_CONST, DRM_EST_HEADER_CONST_LEN ) == 0,
             DRM_E_DST_CORRUPTED);

    NETWORKBYTES_TO_WORD( wCurVersion, f_pbStoreData, DRM_EST_HEADER_CONST_LEN );
    NETWORKBYTES_TO_WORD( wMinVersion, f_pbStoreData, DRM_EST_HEADER_CONST_LEN + SIZEOF( DRM_WORD ) );
    ChkBOOL( wMinVersion >= DRM_EST_MINIMUM_VERSION, DRM_E_DST_NOT_COMPATIBLE );
    ChkBOOL( wMinVersion <= DRM_EST_CURRENT_VERSION, DRM_E_DST_NOT_COMPATIBLE );
    ChkBOOL( wMinVersion <= wCurVersion, DRM_E_DST_CORRUPTED );

    NETWORKBYTES_TO_DWORD( cbExpected, f_pbStoreData, DRM_EST_HEADER_CONST_LEN + 2 * SIZEOF( DRM_WORD ) );
    ChkBOOL( cbExpected <= f_cbStoreData, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( cbExpected <= MAX_EMBEDDED_STORE_LEN, DRM_E_DST_CORRUPTED ); /* total ELS size cannot be > 10K */

    poESTContext->m_fStoreOpen      = TRUE;
    poESTContext->m_fIsDirty        = FALSE;
    poESTContext->m_cbEmbeddedStore = cbExpected;
    poESTContext->m_pbEmbeddedStore = f_pbStoreData;

    ChkDR( _EST_PrepareUnusedBuffer( poESTContext ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CloseStore
**
** Synopsis:    Close the embedded data store.
**
** Arguments:
**      [f_pContextDST] - Specifies the data store context to close.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CloseStore ( __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;

    ChkArg( f_pContextDST != NULL );
    ChkArg( poESTContext->m_fInited );

    poESTContext->m_fStoreOpen       = FALSE;
    poESTContext->m_cbEmbeddedStore  = 0;
    poESTContext->m_pbEmbeddedStore  = NULL;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CommitStore
**
** Synopsis:    Flushes unwritten data to the embedded store.
**
** Arguments:
**      [f_pContextDST] - Specifies the data store context to flush.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CommitStore (
    __inout DRM_DST_CONTEXT *f_pContextDST )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;

    ChkArg( f_pContextDST != NULL );
    ChkArg( poESTContext->m_fInited );
    ChkArg( poESTContext->m_fStoreOpen );

    poESTContext->m_fIsDirty = FALSE;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CleanupStore
**
** Synopsis:    Performs cleanup on an embedded data store.
**
** Arguments:
**      [f_pContextDST] - Specifies the data store context to clean.
**      [f_fWait]       - Specifies whether to wait for locked slots to unlock.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Parameters must match generic drmdatastore.h interface")
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CleanupStore(
    __inout       DRM_DST_CONTEXT *f_pContextDST,
    __in          DRM_BOOL         f_fWait )
PREFAST_POP
{
    DRM_RESULT             dr           = DRM_SUCCESS;
    const DRM_EST_CONTEXT *poESTContext = ( const DRM_EST_CONTEXT * )f_pContextDST;

    ChkArg( f_pContextDST != NULL );
    ChkArg( poESTContext->m_fInited );
    ChkArg( poESTContext->m_fStoreOpen );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_OpenNamespace
**
** Synopsis:    Opens a specified namespace within an embedded data store.
**
** Arguments:
**      [f_pContextDST]        - Specifies the data store context.
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_OpenNamespace (
    __inout        DRM_DST_CONTEXT           *f_pContextDST,
    __in    const  DRM_DST_NAMESPACE         *f_pNamespace,
    __in           DRM_DST_OPENMODE           f_eOpenMode,
    __in           DRM_WORD                   f_wMaxNumChildren,
    __in           DRM_DWORD                  f_dwExpectedSlotSize,
    __out          DRM_DST_NAMESPACE_CONTEXT *f_pContextNS )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_EST_CONTEXT *poESTContext = ( DRM_EST_CONTEXT * )f_pContextDST;
    DRM_EST_NAMESPACE_CONTEXT *poESTNSContext = ( DRM_EST_NAMESPACE_CONTEXT * )f_pContextNS;

    ChkArg( f_pContextDST != NULL );
    ChkArg( f_pContextNS  != NULL );
    ChkArg( poESTContext->m_fInited );
    ChkArg( poESTContext->m_fStoreOpen );

    poESTNSContext->m_fNamespaceOpen = TRUE;
    poESTNSContext->m_poESTContext   = poESTContext;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_DeleteNamespace
**
** Synopsis:    Deletes a specified namespace and all sub-nodes from an
**              embedded data store.
**
** Arguments:
**      [f_pContextDST] - Specifies the data store context.
**      [f_pNS]         - Identifies the namespace to delete.
**      [f_fWait]       - Specifies whether to wait for locks before deletion.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_DeleteNamespace(
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Parameters must match generic drmdatastore.h interface")
    __inout    DRM_DST_CONTEXT   *f_pContextDST,
    __inout    DRM_DST_NAMESPACE *f_pNS,
PREFAST_POP
    __in       DRM_BOOL           f_fWait )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pContextDST != NULL );
    ChkArg( f_pNS         != NULL );

    dr = DRM_E_NOTIMPL;
ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CloseNamespace
**
** Synopsis:    Closes the specified namespace.
**
** Arguments:
**      [f_pContextNS]  - Specifies the context of the namespace to close.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CloseNamespace (
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    DRM_RESULT                 dr             = DRM_SUCCESS;
    DRM_EST_NAMESPACE_CONTEXT *poESTNSContext = ( DRM_EST_NAMESPACE_CONTEXT * )f_pContextNS;

    ChkArg( f_pContextNS  != NULL );
    ChkArg( poESTNSContext->m_fNamespaceOpen );

    poESTNSContext->m_fNamespaceOpen = FALSE;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CommitNamespace
**
** Synopsis:    Flushes open data to the specified namespace.
**
** Arguments:
**      [f_pContextNS]  - Specifies the context of the namespace to flush.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CommitNamespace (
    __inout    DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS )
{
    return DRM_SUCCESS;
}

/**********************************************************************
** Function:    DRM_EST_MakeSlotHint
**
** Synopsis:    Initialize a slot hint based on the slot context passed.
**
** Arguments:
**      [f_pContextSlot]  - Specifies the slot context.
**      [f_pSlotHint]     - Returns the initialized slot hint.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_MakeSlotHint (
    __in    DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out   DRM_DST_SLOT_HINT     *f_pSlotHint )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;

    ChkArg( f_pContextSlot != NULL );
    ChkArg( f_pSlotHint    != NULL );

    MEMCPY( f_pSlotHint, &poESTSlotContext->m_oSlotLocation, SIZEOF( DRM_EST_SLOT_HINT ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_OpenSlot
**
** Synopsis:    Create or open a dataslot using given key1 and key2.
**
** Arguments:
**      [f_pContextNS]    - Specifies the namespace to search for a slot.
**      [f_dwMode]        - Combo of DRM_DST_OPENMODE and DRM_DSTSLOT_LOCKMODE.
**      [f_pKey1]         - Specifies the first key of the slot to open.
**      [f_pKey2]         - Specifies the second (unique) key of the slot to open.
**      [f_pSlotHint]     - Optional. Specifies a slot hint for the slot to open.
**      [f_pcbSlotSize]   - Specifies the slot context size.
**                          Returns the size of the data slot in bytes (no padding)
**      [f_pContextSlot]  - Returns the context for the opened slot.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_OpenSlot (
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in       DRM_DWORD                  f_dwMode,
    __in const DRM_DST_KEY               *f_pKey1,
    __in const DRM_DST_KEY               *f_pKey2,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __inout    DRM_DWORD                 *f_pcbSlotSize,
    __out      DRM_DST_SLOT_CONTEXT      *f_pContextSlot )
{
    DRM_RESULT                 dr               = DRM_SUCCESS;
    DRM_RESULT                 dr2              = DRM_SUCCESS;
    DRM_EST_NAMESPACE_CONTEXT *poESTNSContext   = ( DRM_EST_NAMESPACE_CONTEXT * )f_pContextNS;
    DRM_EST_CONTEXT           *poESTContext     = NULL;
    DRM_EST_SLOT_CONTEXT      *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;

    ChkArg( f_pContextNS   != NULL );
    ChkArg( f_pKey1        != NULL );
    ChkArg( f_pKey2        != NULL );
    ChkArg( f_pcbSlotSize  != NULL );
    ChkArg( f_pContextSlot != NULL );
    ChkArg( poESTNSContext->m_poESTContext != NULL );
    ChkArg( poESTNSContext->m_fNamespaceOpen );

    poESTContext = poESTNSContext->m_poESTContext;
    DRMASSERT( poESTContext != NULL );

    ChkArg( poESTContext->m_fStoreOpen );
    ChkBOOL( poESTContext->m_cbEmbeddedStore > DRM_EST_HEADER_LEN + DRM_EST_SLOTHEADER_LEN,
             DRM_E_BUFFERTOOSMALL );

    /*
    ** Attempt to open existing slot
    */
    if ( f_dwMode & eDRM_DST_OPEN_EXISTING
      || f_dwMode & eDRM_DST_OPEN_ALWAYS )
    {
        dr2 = _EST_OpenSlot( poESTContext,
                             f_dwMode,
                             f_pKey1,
                             f_pKey2,
                             ( DRM_EST_SLOT_HINT * )f_pSlotHint,
                             poESTSlotContext );

        if ( DRM_SUCCEEDED( dr2 ) )
        {
            *f_pcbSlotSize = poESTSlotContext->m_oSlotLocation.m_cbSlotLen - DRM_EST_SLOTHEADER_LEN;
            goto ErrorExit; /* Success. Do not create slot */
        }
        else if ( f_dwMode & eDRM_DST_OPEN_EXISTING )
        {
            ChkDR( dr2 ); /* Failed to open slot. */
        }
        dr2 = DRM_S_FALSE; /* False indicates a slot was not found */
    }

    /* Try to create the slot. */
    dr = _EST_CreateSlot( poESTContext,
                          f_dwMode,
                          f_pKey1,
                          f_pKey2,
                         *f_pcbSlotSize,
                          poESTSlotContext );

    if ( DRM_FAILED( dr ) )
    {
        /* If create slot fails, error should always be returned to the caller. */
        dr2 = dr;
    }

ErrorExit:

    if ( DRM_SUCCEEDED( dr2 ) )
    {
        dr = dr2;
    }

    if ( DRM_SUCCEEDED( dr ) && poESTSlotContext != NULL )
    {
        poESTSlotContext->m_fSlotOpen      = TRUE;
        poESTSlotContext->m_ibSeekPtr      = 0;
        poESTSlotContext->m_poESTNSContext = poESTNSContext;
    }

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_CloseSlot
**
** Synopsis:    Closes an open data slot.
**
** Arguments:
**      [f_pContextSlot] - Specifies the slot context.
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_CloseSlot (
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;

    ChkArg( f_pContextSlot != NULL );
    ChkArg( poESTSlotContext->m_fSlotOpen );

    /*
    ** Zero out the following members:
    ** poESTSlotContext->m_fSlotOpen      = FALSE;
    ** poESTSlotContext->m_poESTNSContext = NULL;
    ** poESTSlotContext->m_ibSeekPtr      = 0;
    */
    ZEROMEM( poESTSlotContext, SIZEOF( DRM_EST_SLOT_CONTEXT ) );

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_DeleteSlot
**
** Synopsis:    Deletes a dataslot with given key1 and key2.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_DeleteSlot (
    __in       DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in const DRM_DST_KEY               *f_pKey1,
    __in const DRM_DST_KEY               *f_pKey2,
    __in_opt   DRM_DST_SLOT_HINT         *f_pSlotHint,
    __in       DRM_BOOL                   f_fWait )
{
    DRM_RESULT                 dr              = DRM_SUCCESS;
    DRM_EST_NAMESPACE_CONTEXT *poESTNSContext  = ( DRM_EST_NAMESPACE_CONTEXT * )f_pContextNS;
    DRM_EST_CONTEXT           *poESTContext     = NULL;
    DRM_EST_SLOT_CONTEXT       oESTSlotContext = { 0 };
    DRM_DWORD                  ibTarget = 0;
    DRM_DWORD                  ibSource = 0;
    DRM_DWORD                  cbCopy   = 0;

    ChkArg( poESTNSContext != NULL );
    ChkArg( f_pKey2 != NULL );

    poESTContext = poESTNSContext->m_poESTContext;
    DRMASSERT( poESTContext != NULL );

    /*
    ** Open the slot with exclusive access
    */
    __analysis_assume( poESTContext != NULL );
    ChkDR( _EST_OpenSlot( poESTContext,
                          eDRM_DST_LOCKEXCLUSIVE,
                          f_pKey1,
                          f_pKey2,
                          ( DRM_EST_SLOT_HINT * )f_pSlotHint,
                          &oESTSlotContext ) );

    DRMASSERT( oESTSlotContext.m_oSlotLocation.m_ibOffset >= DRM_EST_HEADER_LEN );
    DRMASSERT( oESTSlotContext.m_oSlotLocation.m_cbSlotLen > DRM_EST_SLOTHEADER_LEN );
    DRMASSERT( oESTSlotContext.m_oSlotLocation.m_ibOffset +
               PADDED_LEN(oESTSlotContext.m_oSlotLocation.m_cbSlotLen) <= poESTContext->m_ibUnused );

    /*
    ** In order to delete a slot, just shift all subsequent slots forward and
    ** overwrite the slot to be deleted.
    */
    ibTarget = oESTSlotContext.m_oSlotLocation.m_ibOffset;
    ChkOverflow( ibTarget + PADDED_LEN(oESTSlotContext.m_oSlotLocation.m_cbSlotLen), ibTarget );
    ibSource = ibTarget + PADDED_LEN(oESTSlotContext.m_oSlotLocation.m_cbSlotLen);
    cbCopy = poESTContext->m_ibUnused - ibSource;
    ChkOverflow( poESTContext->m_ibUnused, cbCopy );

    DRM_BYT_MoveBytes( poESTContext->m_pbEmbeddedStore,
                       ibTarget,
                       poESTContext->m_pbEmbeddedStore,
                       ibSource,
                       cbCopy );

    /* Increase the size of the unused EST buffer. */
    poESTContext->m_ibUnused -= PADDED_LEN(oESTSlotContext.m_oSlotLocation.m_cbSlotLen);
    poESTContext->m_cbUnused += PADDED_LEN(oESTSlotContext.m_oSlotLocation.m_cbSlotLen);
    DRMASSERT( poESTContext->m_ibUnused + poESTContext->m_cbUnused ==
               poESTContext->m_cbEmbeddedStore );

    /*
    ** Clear the leftover bytes belonging to the unused buffer after memory
    ** shift is done.
    */
    DRM_BYT_SetBytes( poESTContext->m_pbEmbeddedStore,
                      poESTContext->m_ibUnused,
                      poESTContext->m_cbUnused,
                      0 );

    poESTContext->m_dwDeleteCount++;
    poESTContext->m_fIsDirty = TRUE;

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_InitSlotEnum
**
** Synopsis:    Initialized an enumeration of slots within a namespace.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_InitSlotEnum (
    __in           DRM_DST_NAMESPACE_CONTEXT *f_pContextNS,
    __in_opt const DRM_DST_KEY               *f_pKey1,
    __in           DRM_DST_LOCKMODE           f_eLockMode,
    __out          DRM_DST_ENUM_CONTEXT      *f_pContextEnum )
{
    DRM_RESULT                 dr               = DRM_SUCCESS;
    DRM_EST_NAMESPACE_CONTEXT *poESTNSContext   = ( DRM_EST_NAMESPACE_CONTEXT * )f_pContextNS;
    const DRM_EST_CONTEXT     *poESTContext     = NULL;
    DRM_EST_ENUM_CONTEXT      *poESTEnumContext = ( DRM_EST_ENUM_CONTEXT  * )f_pContextEnum;
    DRM_DWORD                  ibCur            = DRM_EST_HEADER_LEN;
    DRM_DWORD                  cbSlot           = 0;

    ChkArg( poESTNSContext   != NULL );
    ChkArg( poESTNSContext->m_poESTContext != NULL );
    ChkArg( poESTEnumContext != NULL );

    poESTContext = poESTNSContext->m_poESTContext;
    DRMASSERT( poESTContext != NULL );

    ZEROMEM( poESTEnumContext, SIZEOF( DRM_EST_ENUM_CONTEXT ) );

    if ( f_pKey1 != NULL )
    {
        /*
        ** Save the key that is used to determine the candidancy of the slots being examined.
        ** If the key is NULL, all slots are eligible to be enumerated.
        */
        MEMCPY( &poESTEnumContext->m_oKey1, f_pKey1, SIZEOF( DRM_DST_KEY ) );
    }

    poESTEnumContext->m_eLockMode = f_eLockMode;

    /*
    ** Calculate the total number of slots (both candidate and non-candidate) in
    ** the EST. The enumeration examination starts with slot 0.
    */
    while ( ibCur < poESTContext->m_ibUnused )
    {
        NETWORKBYTES_TO_DWORD( cbSlot, poESTContext->m_pbEmbeddedStore, ibCur );
        ChkBOOL( cbSlot <= MAX_EMBEDDED_STORE_LEN, DRM_E_DST_CORRUPTED );
        ChkBOOL( cbSlot > DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
        /*
        ** In case cbSlot is corrupt and very large, we don't want to integer overflow
        ** So we use subtraction (A<=B-C) instead of addition (A+C<=B).
        ** We know we won't UNDERFLOW in subtraction because we verified B>C above.
        */
        ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
        ChkBOOL( PADDED_LEN(cbSlot) <= poESTContext->m_ibUnused - ibCur, DRM_E_DST_CORRUPTED );
        ibCur += PADDED_LEN(cbSlot);

        poESTEnumContext->m_cSlots++;
    }

    poESTEnumContext->m_fInited        = TRUE;
    poESTEnumContext->m_poESTNSContext = poESTNSContext;
    poESTEnumContext->m_dwCurrentDeleteCount = poESTContext->m_dwDeleteCount;

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_SlotEnumNext
**
** Synopsis:    Retrieves the next slot from an enumeration.
**
** Arguments:
**      [f_pContextEnum] - Specifies the initialized enumeration context.
**      [f_pContextSlot] - Returns the slot data for the next slot in the enumeration.
**      [f_pKey1]        - Optional. Returns the first key value for the next slot.
**      [f_pKey2]        - Optional. Returns the second (unique) key value for the next slot.
**      [f_pcbSize]      - Optional. Returns the data slot size in bytes (no padding)
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotEnumNext (
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pKey1,
    __out_opt  DRM_DST_KEY           *f_pKey2,
    __out_opt  DRM_DWORD             *f_pcbSize )
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    DRM_EST_ENUM_CONTEXT  *poESTEnumContext = ( DRM_EST_ENUM_CONTEXT * )f_pContextEnum;
    const DRM_EST_CONTEXT *poESTContext     = NULL;
    DRM_EST_SLOT_CONTEXT  *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;
    DRM_DWORD              ibCur            = DRM_EST_HEADER_LEN;
    DRM_DWORD              cbSlot           = 0;
    DRM_DWORD              i                = 0;
    DRM_GUID               oEmptyKey        = EMPTY_DRM_GUID;

    ChkArg( f_pContextEnum                     != NULL );
    ChkArg( f_pContextSlot                     != NULL );
    ChkArg( poESTEnumContext->m_poESTNSContext != NULL );
    ChkArg( poESTEnumContext->m_poESTNSContext->m_poESTContext != NULL );
    ChkArg( poESTEnumContext->m_fInited );
    ChkArg( f_pContextSlot != NULL );

    poESTContext = poESTEnumContext->m_poESTNSContext->m_poESTContext;
    DRMASSERT( poESTContext != NULL );

    DRMCASSERT( SIZEOF( DRM_GUID ) == SIZEOF( DRM_DST_KEY ) );

    if( poESTContext->m_dwDeleteCount > poESTEnumContext->m_dwCurrentDeleteCount )
    {
        /* A deletion has occurred since we started, need to restart the enumeration */
        poESTEnumContext->m_dwCurrentDeleteCount = poESTContext->m_dwDeleteCount;
        poESTEnumContext->m_iSlot = 0;
    }

    /*
    ** Check whether the enumeration index has already reached the end of all
    ** available slots.
    */
    ChkBOOL( poESTEnumContext->m_iSlot < poESTEnumContext->m_cSlots, DRM_E_NOMORE );

    /* Skip the slots that have been visited before. */
    for ( i = 0; i < poESTEnumContext->m_iSlot; ++i )
    {
        if( ibCur >= poESTContext->m_ibUnused )
        {
            /* If we hit here it means someone deleted slots between InitSlotEnum and now */
            ChkDR( DRM_E_NOMORE );
        }

        NETWORKBYTES_TO_DWORD( cbSlot, poESTContext->m_pbEmbeddedStore, ibCur );
        ChkBOOL( cbSlot <= MAX_EMBEDDED_STORE_LEN, DRM_E_DST_CORRUPTED );
        ChkBOOL( cbSlot > DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
        ChkBOOL( ibCur <= poESTContext->m_ibUnused, DRM_E_DST_CORRUPTED );
        /*
        ** In case cbSlot is corrupt and very large, we don't want to integer overflow
        ** So we use subtraction (A<=B-C) instead of addition (A+C<=B).
        ** We know we won't UNDERFLOW in subtraction because we verified B>C above.
        */
        ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
        ChkBOOL( PADDED_LEN(cbSlot) <= poESTContext->m_ibUnused - ibCur, DRM_E_DST_CORRUPTED );
        ibCur += PADDED_LEN(cbSlot);
    }

    while ( ibCur < poESTContext->m_ibUnused &&
            poESTEnumContext->m_iSlot < poESTEnumContext->m_cSlots )
    {
        NETWORKBYTES_TO_DWORD( cbSlot, poESTContext->m_pbEmbeddedStore, ibCur );
        ChkBOOL( cbSlot <= MAX_EMBEDDED_STORE_LEN, DRM_E_DST_CORRUPTED );
        ChkBOOL( cbSlot > DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
        /*
        ** In case cbSlot is corrupt and very large, we don't want to integer overflow
        ** So we use subtraction (A<=B-C) instead of addition (A+C<=B).
        ** We know we won't UNDERFLOW in subtraction because we verified B>C above.
        */
        ChkBOOL( PADDED_LEN(cbSlot) >= cbSlot, DRM_E_DST_CORRUPTED );
        ChkBOOL( PADDED_LEN(cbSlot) <= poESTContext->m_ibUnused - ibCur, DRM_E_DST_CORRUPTED );

        ibCur += SIZEOF( DRM_DWORD );

        /*
        ** If the primary key is not empty, use it to check whether the current
        ** slot being examined is a candidate of the enumeration.
        */
        if ( MEMCMP( &poESTEnumContext->m_oKey1, &oEmptyKey, SIZEOF( DRM_DST_KEY ) ) != 0 &&
             DRM_BYT_CompareBytes( poESTContext->m_pbEmbeddedStore,
                                   ibCur,
                                   ( DRM_BYTE * )&poESTEnumContext->m_oKey1,
                                   0,
                                   SIZEOF( DRM_DST_KEY ) ) != 0 )
        {
            ChkBOOL( cbSlot >= DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
            ChkOverflow( PADDED_LEN( cbSlot - DRM_EST_SLOTHEADER_LEN ), cbSlot - DRM_EST_SLOTHEADER_LEN );
            ibCur += ( 2 * SIZEOF( DRM_DST_KEY ) + PADDED_LEN( cbSlot - DRM_EST_SLOTHEADER_LEN ) );
            DRMASSERT( ibCur <= poESTContext->m_ibUnused );

            poESTEnumContext->m_iSlot++;
        }
        else
        {
            /*
            ** Cache the location of the current slot. The location will be used when the slot is
            ** reloaded or deleted.
            */
            poESTEnumContext->m_oSlotHint.m_ibOffset = ibCur - SIZEOF( DRM_DWORD );
            poESTEnumContext->m_oSlotHint.m_cbSlotLen = cbSlot;

            /* Set the slot valid flag to allow reload current and delete current operations. */
            poESTEnumContext->m_fSlotValid = TRUE;

            break;
        }
    }

    if ( ibCur >= poESTContext->m_ibUnused ||
         poESTEnumContext->m_iSlot >= poESTEnumContext->m_cSlots )
    {
        /* No matching slot is found. */
        ChkDR( DRM_E_NOMORE );
    }

    /* Fill the information of the current slot into the slot context provided by the caller. */
    ChkDR( _EST_PrepareSlotToBeReturned( poESTEnumContext->m_poESTNSContext,
                                         poESTContext,
                                         &poESTEnumContext->m_oSlotHint,
                                         poESTEnumContext->m_eLockMode,
                                         poESTSlotContext,
                                         f_pKey1,
                                         f_pKey2,
                                         f_pcbSize ) );

    /* Increase the starting index of the slot to be enumerated next. */
    poESTEnumContext->m_iSlot++;

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_SlotEnumReloadCurrent
**
** Synopsis:    Reload the current slot in the enumeration.
**
** Arguments:
**      [f_pContextEnum] - Specifies the initialized enumeration context.
**      [f_pContextSlot] - Returns the slot data for the current slot in the enumeration.
**      [f_pKey1]        - Optional. Returns the first key value for the slot.
**      [f_pKey2]        - Optional. Returns the second (unique) key value for the slot.
**      [f_pcbSize]      - Optional. Returns the data slot size in bytes (no padding).
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotEnumReloadCurrent (
    __inout    DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
    __out      DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __out_opt  DRM_DST_KEY           *f_pKey1,
    __out_opt  DRM_DST_KEY           *f_pKey2,
    __out_opt  DRM_DWORD             *f_pcbSize )
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    DRM_EST_ENUM_CONTEXT  *poESTEnumContext = ( DRM_EST_ENUM_CONTEXT * )f_pContextEnum;

    ChkArg( poESTEnumContext != NULL );
    ChkArg( poESTEnumContext->m_poESTNSContext != NULL );
    ChkArg( poESTEnumContext->m_poESTNSContext->m_poESTContext != NULL );
    ChkArg( poESTEnumContext->m_fInited );
    ChkArg( f_pContextSlot != NULL );

    /* If the current slot has not yet been enumerated, return with error. */
    ChkBOOL( poESTEnumContext->m_fSlotValid, DRM_E_NOMORE );

    /* Fill the information of the current slot into the slot context provided by the caller. */
    ChkDR( _EST_PrepareSlotToBeReturned( poESTEnumContext->m_poESTNSContext,
                                         poESTEnumContext->m_poESTNSContext->m_poESTContext,
                                         &poESTEnumContext->m_oSlotHint,
                                         poESTEnumContext->m_eLockMode,
                                         ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot,
                                         f_pKey1,
                                         f_pKey2,
                                         f_pcbSize ) );

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_SlotEnumDeleteCurrent
**
** Synopsis:    Deletes the currently enumerated slot.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotEnumDeleteCurrent (
    __inout   DRM_DST_ENUM_CONTEXT  *f_pContextEnum,
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Parameters must match generic drmdatastore.h interface")
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot )
PREFAST_POP
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    DRM_EST_ENUM_CONTEXT  *poESTEnumContext = ( DRM_EST_ENUM_CONTEXT * )f_pContextEnum;
    DRM_EST_CONTEXT       *poESTContext     = NULL;
    DRM_DWORD              ibTarget         = 0;
    DRM_DWORD              ibSource         = 0;
    DRM_DWORD              cbCopy           = 0;

    ChkArg( poESTEnumContext != NULL );
    ChkArg( poESTEnumContext->m_poESTNSContext != NULL );
    ChkArg( poESTEnumContext->m_poESTNSContext->m_poESTContext != NULL );
    ChkArg( poESTEnumContext->m_fInited );

    /*
    ** Although this parameter is not used for EST, it is still checked to be consistent
    ** with other data store implementation.
    */
    ChkArg( f_pContextSlot != NULL );

    /* If the current slot has not yet been enumerated, return with error. */
    ChkBOOL( poESTEnumContext->m_fSlotValid, DRM_E_NOMORE );

    /*
    ** Since there is at least one slot that has already been enumerated,
    ** the total number of slots should be at least 0.
    */
    DRMASSERT( poESTEnumContext->m_cSlots > 0 );

    poESTContext = poESTEnumContext->m_poESTNSContext->m_poESTContext;
    DRMASSERT( poESTContext != NULL );

    DRMASSERT( poESTEnumContext->m_oSlotHint.m_ibOffset >= DRM_EST_HEADER_LEN );
    DRMASSERT( poESTEnumContext->m_oSlotHint.m_cbSlotLen > DRM_EST_SLOTHEADER_LEN );
    DRMASSERT( poESTEnumContext->m_oSlotHint.m_ibOffset +
               PADDED_LEN(poESTEnumContext->m_oSlotHint.m_cbSlotLen) <= poESTContext->m_ibUnused );

    /*
    ** Delete the current slot using the cached slot hint information.
    ** In order to delete a slot, just shift all subsequent slots forward and
    ** overwrite the slot to be deleted.
    */
    ibTarget = poESTEnumContext->m_oSlotHint.m_ibOffset;
    ChkOverflow( ibTarget + PADDED_LEN(poESTEnumContext->m_oSlotHint.m_cbSlotLen), ibTarget );
    ibSource = ibTarget + PADDED_LEN(poESTEnumContext->m_oSlotHint.m_cbSlotLen);
    cbCopy = poESTContext->m_ibUnused - ibSource;
    ChkOverflow( poESTContext->m_ibUnused, cbCopy );

    DRM_BYT_MoveBytes( poESTContext->m_pbEmbeddedStore,
                       ibTarget,
                       poESTContext->m_pbEmbeddedStore,
                       ibSource,
                       cbCopy );

    /* Increase the size of the unused EST buffer. */
    poESTContext->m_ibUnused -= PADDED_LEN(poESTEnumContext->m_oSlotHint.m_cbSlotLen);
    poESTContext->m_cbUnused += PADDED_LEN(poESTEnumContext->m_oSlotHint.m_cbSlotLen);
    DRMASSERT( poESTContext->m_ibUnused + poESTContext->m_cbUnused ==
               poESTContext->m_cbEmbeddedStore );

    /*
    ** Set all unused EST buffer to 0 to clear the leftover bytes from the
    ** deleted slot.
    */
    DRM_BYT_SetBytes( poESTContext->m_pbEmbeddedStore,
                      poESTContext->m_ibUnused,
                      poESTContext->m_cbUnused,
                      0 );

    /* The current slot is not valid to be used any more. */
    poESTEnumContext->m_fSlotValid = FALSE;

    /* Reduce the total number of candidate slots. */
    poESTEnumContext->m_cSlots--;

    if ( poESTEnumContext->m_iSlot > 0 )
    {
        poESTEnumContext->m_iSlot--;
    }

    /* Increment the delete count, but for this particular enumerator we don't need to
    ** restart our loop since we already corrected for the deletion
    */
    if( poESTContext->m_dwDeleteCount++ == poESTEnumContext->m_dwCurrentDeleteCount )
    {
        poESTEnumContext->m_dwCurrentDeleteCount = poESTContext->m_dwDeleteCount;
    }

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_BlockScanDelete
**
** Synopsis:    Rapid deletion of multiple slots.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_BlockScanDelete (
    __inout                DRM_DST_NAMESPACE_CONTEXT  *f_pContextNS,
    __in_ecount( f_cKey1 ) DRM_DST_KEY                *f_rgKey1,
    __in                   DRM_DWORD                   f_cKey1,
    __in                   DRM_DST_KEY                *f_pKey2,
    __in                   DRM_BOOL                    f_fWait )
{
    return DRM_E_NOTIMPL; /* Not yet implemented for the embedded store. */
}

/**********************************************************************
** Function:    DRM_EST_SlotResize
**
** Synopsis:    Resizes an open data slot.
**
** Arguments:
**      [f_pContextSlot] - Specifies the slot context to resize.
**      [f_cbSize]       - Specifies the new size for the slot
**                         (padding is not included).
**
** Returns:
**      DRM_SUCCESS      - Success
**      DRM_E_INVALIDARG - An argument was NULL or improperly initialized.
**
***********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotResize (
    __inout  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in     DRM_DWORD              f_cbSize )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;
    DRM_EST_CONTEXT      *poESTContext     = NULL;
    DRM_LONG              nDiff            = 0;
    DRM_LONG              nPaddedDiff      = 0;
    DRM_DWORD             ibTarget         = 0;
    DRM_DWORD             ibSource         = 0;
    DRM_DWORD             cbCopy           = 0;

    ChkArg( poESTSlotContext != NULL );
    ChkArg( poESTSlotContext->m_poESTNSContext != NULL );
    ChkArg( poESTSlotContext->m_poESTNSContext->m_poESTContext != NULL );
    ChkArg( f_cbSize > 0 );

    poESTContext = poESTSlotContext->m_poESTNSContext->m_poESTContext;
    DRMASSERT( poESTContext != NULL );

    /* Size diff without padding, tells if slot size should be rewritten */
    nDiff = (DRM_LONG)( ((DRM_LONG)f_cbSize) - (DRM_LONG)( ((DRM_LONG)poESTSlotContext->m_oSlotLocation.m_cbSlotLen) - ((DRM_LONG)DRM_EST_SLOTHEADER_LEN) ) );
    /* "Real" size diff, tells if subsequent slots should be moved and size of unused buffer changed */
    ChkOverflow( PADDED_LEN(f_cbSize), f_cbSize );
    ChkBOOL( poESTSlotContext->m_oSlotLocation.m_cbSlotLen >= DRM_EST_SLOTHEADER_LEN, DRM_E_DST_CORRUPTED );
    ChkOverflow( PADDED_LEN(poESTSlotContext->m_oSlotLocation.m_cbSlotLen - DRM_EST_SLOTHEADER_LEN ), poESTSlotContext->m_oSlotLocation.m_cbSlotLen - DRM_EST_SLOTHEADER_LEN );
    nPaddedDiff = (DRM_LONG)( ((DRM_LONG)PADDED_LEN(f_cbSize)) - ((DRM_LONG)PADDED_LEN( poESTSlotContext->m_oSlotLocation.m_cbSlotLen - DRM_EST_SLOTHEADER_LEN )) );

    if ( nDiff == 0 )
    {
        /*
        ** If the size to be changed is the same as the size of the slot,
        ** no change is necessary.
        */
        DRMASSERT( nPaddedDiff == 0 );
        goto ErrorExit;
    }
    else if ( nPaddedDiff > 0 )
    {
        /*
        ** Make sure there is enough space of the unused buffer to satisfy the growth.
        */
        ChkBOOL( poESTContext->m_cbUnused >= ( DRM_DWORD )nPaddedDiff,
                 DRM_E_BUFFERTOOSMALL);
    }

    DRMASSERT( poESTSlotContext->m_oSlotLocation.m_ibOffset >= DRM_EST_HEADER_LEN );
    DRMASSERT( poESTSlotContext->m_oSlotLocation.m_cbSlotLen > DRM_EST_SLOTHEADER_LEN );
    DRMASSERT( poESTSlotContext->m_oSlotLocation.m_ibOffset +
               PADDED_LEN(poESTSlotContext->m_oSlotLocation.m_cbSlotLen) <= poESTContext->m_ibUnused );

    /* Update the new size of the slot being resized. */
    DWORD_TO_NETWORKBYTES( poESTContext->m_pbEmbeddedStore,
                           poESTSlotContext->m_oSlotLocation.m_ibOffset,
                           f_cbSize + DRM_EST_SLOTHEADER_LEN );

    if ( nPaddedDiff != 0 )
    {
        /*
        ** To relaize the effect of changing the size of a slot. Shift the content of the subsequent
        ** slots forward when the slot being resized shrinks, or backward when the slot being resized
        ** grows.
        */
        ibTarget = poESTSlotContext->m_oSlotLocation.m_ibOffset + PADDED_LEN(f_cbSize + DRM_EST_SLOTHEADER_LEN);
        ChkOverflow( ibTarget, poESTSlotContext->m_oSlotLocation.m_ibOffset );
        ibSource = poESTSlotContext->m_oSlotLocation.m_ibOffset + PADDED_LEN(poESTSlotContext->m_oSlotLocation.m_cbSlotLen);
        ChkOverflow( ibSource, poESTSlotContext->m_oSlotLocation.m_ibOffset );
        cbCopy = poESTContext->m_ibUnused - ibSource;
        ChkOverflow( poESTContext->m_ibUnused, cbCopy );

        DRM_BYT_MoveBytes( poESTContext->m_pbEmbeddedStore,
                           ibTarget,
                           poESTContext->m_pbEmbeddedStore,
                           ibSource,
                           cbCopy );
    }

    /* Update the size of the unused EST buffer. */
    poESTContext->m_ibUnused = ( DRM_DWORD )( ( DRM_LONG )poESTContext->m_ibUnused + nPaddedDiff );
    poESTContext->m_cbUnused = ( DRM_DWORD )( ( DRM_LONG )poESTContext->m_cbUnused - nPaddedDiff );
    DRMASSERT( poESTContext->m_ibUnused + poESTContext->m_cbUnused ==
               poESTContext->m_cbEmbeddedStore );

    if ( nPaddedDiff < 0 )
    {
        /* If the size shrinks, overwrite the leftover bytes of the unused EST buffer with 0. */
        DRM_BYT_SetBytes( poESTContext->m_pbEmbeddedStore,
                          poESTContext->m_ibUnused,
                          poESTContext->m_cbUnused,
                          0 );
    }
    else
    {
        /* If the size grows, overwrite the newly allocated slot buffer with 0. */
        DRMASSERT( nDiff >= 0 );
        DRM_BYT_SetBytes( poESTContext->m_pbEmbeddedStore,
                          poESTSlotContext->m_oSlotLocation.m_ibOffset + poESTSlotContext->m_oSlotLocation.m_cbSlotLen,
                          (DRM_SIZE_T)nDiff,
                          0 );
    }

    /* Update the size of the passed in slot context. */
    poESTSlotContext->m_oSlotLocation.m_cbSlotLen = f_cbSize + DRM_EST_SLOTHEADER_LEN;

    /* Should reset the seek pointer after the slot is resized. */
    poESTSlotContext->m_ibSeekPtr = 0;

ErrorExit:

    return dr;
}

/**********************************************************************
** Function:    DRM_EST_SlotSeek
**
** Synopsis:    Set the seek pointer within an open data slot.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotSeek (
    __inout   DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in      DRM_LONG               f_lOffset,
    __in      DRM_DST_SEEKMODE       f_eSeekMode,
    __out_opt DRM_DWORD             *f_pdwSeekPointer )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;
    DRM_DWORD             ibExpected       = 0;

    ChkArg( f_pContextSlot != NULL );
    ChkArg( poESTSlotContext->m_fSlotOpen );

    /*
    ** Set the seek pointer
    */
    switch ( f_eSeekMode )
    {
    case eDRM_DST_SEEKCUR:
        ibExpected = poESTSlotContext->m_ibSeekPtr + f_lOffset;
        break;
    case eDRM_DST_SEEKEND:
        ibExpected = ( poESTSlotContext->m_oSlotLocation.m_cbSlotLen - DRM_EST_SLOTHEADER_LEN ) - f_lOffset;
        break;
    case eDRM_DST_SEEKSET:
        AssertChkArg( f_lOffset >= 0 );
        ibExpected = (DRM_DWORD)f_lOffset;
        break;
    default:
        ChkArg( FALSE );
    }
    /*
    ** Verify that the new pointer is within bounds
    */
    ChkBOOL( ibExpected + DRM_EST_SLOTHEADER_LEN <= poESTSlotContext->m_oSlotLocation.m_cbSlotLen,
             DRM_E_DST_SEEK_ERROR );
    poESTSlotContext->m_ibSeekPtr = ibExpected;

    if ( f_pdwSeekPointer != NULL )
    {
        *f_pdwSeekPointer = ibExpected;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_SlotRead
**
** Synopsis:    Read data from an open data slot.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotRead (
    __inout                  DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                     DRM_DWORD              f_cbData,
    __out_bcount( f_cbData ) DRM_BYTE              *f_pbData,
    __out_opt                DRM_DWORD             *f_pcbRead )
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT  *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;
    const DRM_EST_CONTEXT *poESTContext     = NULL;
    DRM_DWORD              ibCurr           = 0;
    DRM_DWORD              cbRead           = f_cbData;

    ChkArg( f_pContextSlot != NULL );
    ChkArg( f_pbData       != NULL );
    ChkArg( poESTSlotContext->m_fSlotOpen );
    ChkArg( poESTSlotContext->m_poESTNSContext != NULL );
    ChkArg( poESTSlotContext->m_poESTNSContext->m_poESTContext != NULL );

    /*
    ** Read from data store:
    */
    poESTContext = poESTSlotContext->m_poESTNSContext->m_poESTContext;
    ibCurr = poESTSlotContext->m_oSlotLocation.m_ibOffset + DRM_EST_SLOTHEADER_LEN + poESTSlotContext->m_ibSeekPtr;

    if ( ibCurr + cbRead > poESTSlotContext->m_oSlotLocation.m_ibOffset + poESTSlotContext->m_oSlotLocation.m_cbSlotLen )
    {
        cbRead = poESTSlotContext->m_oSlotLocation.m_ibOffset + poESTSlotContext->m_oSlotLocation.m_cbSlotLen - ibCurr;
    }
    DRM_BYT_CopyBytes( f_pbData, 0, poESTContext->m_pbEmbeddedStore, ibCurr, cbRead );
    poESTSlotContext->m_ibSeekPtr += cbRead;

    if ( f_pcbRead != NULL )
    {
        *f_pcbRead = cbRead;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_EST_SlotWrite
**
** Synopsis:    Write data from an open and locked data slot.
**
** Arguments:
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
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EST_SlotWrite (
    __inout                       DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __in                          DRM_DWORD              f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE              *f_pbData,
    __out_opt                     DRM_DWORD             *f_pcbWritten )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;
    DRM_EST_CONTEXT      *poESTContext     = NULL;
    DRM_DWORD             ibCurr           = 0;
    DRM_DWORD             cbWritten        = f_cbData;

    ChkArg( f_pContextSlot != NULL );
    ChkArg( f_pbData       != NULL );
    ChkArg( poESTSlotContext->m_fSlotOpen );
    ChkArg( poESTSlotContext->m_poESTNSContext != NULL );
    ChkArg( poESTSlotContext->m_poESTNSContext->m_poESTContext != NULL );

    /*
    ** Write to data store:
    */
    poESTContext = poESTSlotContext->m_poESTNSContext->m_poESTContext;
    ibCurr = poESTSlotContext->m_oSlotLocation.m_ibOffset + DRM_EST_SLOTHEADER_LEN + poESTSlotContext->m_ibSeekPtr;

    if ( ibCurr + cbWritten > poESTSlotContext->m_oSlotLocation.m_ibOffset + poESTSlotContext->m_oSlotLocation.m_cbSlotLen )
    {
        cbWritten = poESTSlotContext->m_oSlotLocation.m_ibOffset + poESTSlotContext->m_oSlotLocation.m_cbSlotLen - ibCurr;
    }
    DRM_BYT_CopyBytes( poESTContext->m_pbEmbeddedStore, ibCurr, f_pbData, 0, cbWritten );

    poESTSlotContext->m_ibSeekPtr += cbWritten;

    if ( f_pcbWritten != NULL )
    {
        *f_pcbWritten = cbWritten;
    }
    poESTContext->m_fIsDirty = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_EST_SlotReadPtr(
    __inout                           DRM_DST_SLOT_CONTEXT  *f_pContextSlot,
    __deref_out_bcount( *f_pcbData )  DRM_BYTE             **f_ppbData,
    __out                             DRM_DWORD             *f_pcbData )
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    DRM_EST_SLOT_CONTEXT  *poESTSlotContext = ( DRM_EST_SLOT_CONTEXT * )f_pContextSlot;
    const DRM_EST_CONTEXT *poESTContext     = NULL;
    DRM_DWORD              ibCurr           = 0;
    DRM_DWORD              cbData           = 0;

    ChkArg( f_pContextSlot != NULL );
    ChkArg( f_ppbData      != NULL );
    ChkArg( f_pcbData      != NULL );
    ChkArg( poESTSlotContext->m_fSlotOpen );
    ChkArg( poESTSlotContext->m_poESTNSContext != NULL );
    ChkArg( poESTSlotContext->m_poESTNSContext->m_poESTContext != NULL );

    poESTContext = poESTSlotContext->m_poESTNSContext->m_poESTContext;
    ibCurr = poESTSlotContext->m_oSlotLocation.m_ibOffset + DRM_EST_SLOTHEADER_LEN + poESTSlotContext->m_ibSeekPtr;
    cbData = poESTSlotContext->m_oSlotLocation.m_ibOffset + poESTSlotContext->m_oSlotLocation.m_cbSlotLen - ibCurr;

    /* Note: Not 16-bit addressing safe */
    *f_ppbData = poESTContext->m_pbEmbeddedStore + ibCurr;
    *f_pcbData = cbData;

    poESTSlotContext->m_ibSeekPtr += cbData;

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
