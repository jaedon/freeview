/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmviewrights.h>
#include <drmremovaldatestore.h>

ENTER_PK_NAMESPACE_CODE;

#define DRM_REMOVAL_DATE_STORE_MAX_NUMBER_OF_CHILD_NODES   16
#define DRM_REMOVAL_DATE_CONTENT_HEADER_SIZE 300
#define DRM_REMOVAL_DATE_LICENSE_ENTRY_SIZE   ( SIZEOF( DRM_LID ) + DRM_DST_SLOT_HINT_LEN )
#define DRM_MIN_REMOVAL_DATE_STORE_ENTRY_SIZE ( SIZEOF( DRM_DWORD ) + DRM_REMOVAL_DATE_LICENSE_ENTRY_SIZE )

static const DRM_DST_NAMESPACE g_namespaceRemovalDateStore =
{
    'r', 'e', 'm', 'o', 'v', 'a', 'l', 'd',
    'a', 't', 'e', 's', 't', 'r', '\0', '\0'
};

/*********************************************************************
** Function: _RemoveLID
**
** Synopsis: Searches for and removes removal date entries for the
**           specified KID/LID combination.  The absence of the specified
**           KID/LID combination is not an error.
**
** Parameters:
**
** [f_pRemovalDateEnumContext] -- Context for the removal date enumeration
** [f_dwRemovalDate]           -- Removal date of the license
** [f_pKID]                    -- KID of the license to be removed
** [f_pDateSlotHint]           -- Optional. Specifies a slot hint for the
**                                slot to open in the removal date namespace.
** [f_pLID]                    -- LID of the license to be removed
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _RemoveLID(
    __in_ecount( 1 )           DRM_REMOVAL_DATE_ENUM     *f_pRemovalDateEnumContext,
    __in                       DRM_DWORD                  f_dwRemovalDate,
    __in_ecount( 1 )           DRM_KID                   *f_pKID,
    __in_ecount_opt( 1 )       DRM_DST_SLOT_HINT         *f_pDateSlotHint,
    __in_ecount( 1 )     const DRM_LID                   *f_pLID )
{
    DRM_RESULT                        dr                  = DRM_SUCCESS;
    DRM_REMOVAL_DATE_ENUM_PRIVATE    *pRemovalDateEnum    = (DRM_REMOVAL_DATE_ENUM_PRIVATE*) f_pRemovalDateEnumContext;
    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pRemovalDateContext = NULL;
    DRM_DWORD                         cbSlot              = 0;
    DRM_DWORD                         cbIO                = 0;
    DRM_BOOL                          fSlotOpen           = FALSE;
    DRM_DST_KEY                       dstKey              = { 0 };
    DRM_DWORD                         dwNumLicensesInSlot = 0;
    DRM_DWORD                         cLicenses           = 0;
    DRM_REMOVAL_DATE_ENTRY            removalDateEntry    = { 0 };
    DRM_BOOL                          fLicenseRemoved     = FALSE;
    DRM_DWORD                         dwReadPointer       = 0;
    DRM_DWORD                         dwWritePointer      = 0;
    DRM_DWORD                         dwIdx               = 0;

    ChkArg( pRemovalDateEnum != NULL );
    ChkArg( f_pKID != NULL );
    ChkArg( f_pLID != NULL );

    pRemovalDateContext = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) pRemovalDateEnum->pcontextRemovalDate;
    ChkArg( pRemovalDateContext != NULL );
    ChkArg( pRemovalDateContext->pDatastore != NULL );

    /*
    ** Convert the removal date ( a DWORD) into a DRM_ID
    */
    DRMCASSERT(SIZEOF( DRM_DST_KEY ) >= SIZEOF( DRM_DWORD ) );
    DWORD_TO_BYTES( dstKey.rgb, f_dwRemovalDate );

    ChkDR( DRM_DST_OpenSlot( pRemovalDateContext->pDatastore,
                            &pRemovalDateContext->contextNameSpace,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                            &dstKey,
                             f_pKID,
                             f_pDateSlotHint,
                            &cbSlot,
                            &pRemovalDateContext->contextDSTSlot ) );
    fSlotOpen = TRUE;

    if( cbSlot < DRM_MIN_REMOVAL_DATE_STORE_ENTRY_SIZE )
    {
        /* We need at least the minimum entry size for a valid entry */
        return DRM_E_DSTCORRUPTED;
    }

    /* Read how many licenses are in the slot */
    ChkDR( DRM_DST_SlotRead( pRemovalDateContext->pDatastore,
                            &pRemovalDateContext->contextDSTSlot,
                             SIZEOF( DRM_DWORD ),
                             (DRM_BYTE*)&dwNumLicensesInSlot,
                             &cbIO ) );
    ChkBOOL( cbIO == SIZEOF( DRM_DWORD ), DRM_E_FILEREADERROR );
    FIX_ENDIAN_DWORD( dwNumLicensesInSlot );

    cLicenses = dwNumLicensesInSlot;

    /*
    ** Search for the LID in the licenses in the entry.
    ** If the LID is found then remove the information from the slot.
    */
    dwReadPointer = SIZEOF( DRM_DWORD );
    dwWritePointer = dwReadPointer;
    for ( dwIdx = 0; dwIdx < dwNumLicensesInSlot; dwIdx++ )
    {
        /*
        ** If a license has been removed then the read and write pointers
        ** are different and we have to seek back to the current read
        ** location.
        */
        if ( fLicenseRemoved )
        {
            /* Seek to the beginning of the next license entry to read */
            ChkDR( DRM_DST_SlotSeek( pRemovalDateContext->pDatastore,
                                     &pRemovalDateContext->contextDSTSlot,
                                     dwReadPointer,
                                     eDRM_DST_SEEKSET,
                                     NULL ) );
        }

        /* Read the next license info */
        ChkDR( DRM_DST_SlotRead( pRemovalDateContext->pDatastore,
                                &pRemovalDateContext->contextDSTSlot,
                                 SIZEOF( DRM_REMOVAL_DATE_ENTRY ),
                                 (DRM_BYTE*) &removalDateEntry,
                                 &cbIO ) );
        ChkBOOL( cbIO == SIZEOF( DRM_REMOVAL_DATE_ENTRY ), DRM_E_FILEREADERROR );

        if ( MEMCMP( removalDateEntry.lid.rgb, f_pLID->rgb, SIZEOF( DRM_LID ) ) == 0 )
        {
            /*
            ** LID found, remove it.
            */
            cLicenses--;
            fLicenseRemoved = TRUE;
        }
        else
        {
            if ( fLicenseRemoved )
            {
                /* Seek back to the beginning of the next license entry to write */
                ChkDR( DRM_DST_SlotSeek( pRemovalDateContext->pDatastore,
                                         &pRemovalDateContext->contextDSTSlot,
                                         dwWritePointer,
                                         eDRM_DST_SEEKSET,
                                         NULL ) );

                ChkDR( DRM_DST_SlotWrite( pRemovalDateContext->pDatastore,
                                         &pRemovalDateContext->contextDSTSlot,
                                          SIZEOF( DRM_REMOVAL_DATE_ENTRY ),
                                          (DRM_BYTE*) &removalDateEntry,
                                         &cbIO ) );

                ChkBOOL( cbIO == SIZEOF( DRM_REMOVAL_DATE_ENTRY ), DRM_E_FILEWRITEERROR );
            }

            dwWritePointer += SIZEOF( DRM_REMOVAL_DATE_ENTRY );
        }

        dwReadPointer += SIZEOF( DRM_REMOVAL_DATE_ENTRY );
    }

    if ( fLicenseRemoved )
    {
        if ( cLicenses > 0 )
        {
            /*
            ** Seek back to the beginning of the slot and write the new
            ** number of licenses.
            */
            ChkDR( DRM_DST_SlotSeek( pRemovalDateContext->pDatastore,
                                     &pRemovalDateContext->contextDSTSlot,
                                     0,
                                     eDRM_DST_SEEKSET,
                                     NULL ) );

            FIX_ENDIAN_DWORD( cLicenses ); /* Swap byte order if needed before storing */

            ChkDR( DRM_DST_SlotWrite( pRemovalDateContext->pDatastore,
                                     &pRemovalDateContext->contextDSTSlot,
                                      SIZEOF( DRM_DWORD ),
                                      (DRM_BYTE*) &cLicenses,
                                     &cbIO ) );

            FIX_ENDIAN_DWORD( cLicenses ); /* Swap byte order if needed after storing */

            /*
            ** Resize the slot.
            */
            ChkDR( DRM_DST_SlotResize( pRemovalDateContext->pDatastore,
                                      &pRemovalDateContext->contextDSTSlot,
                                       SIZEOF( DRM_DWORD ) + ( cLicenses * SIZEOF( DRM_REMOVAL_DATE_ENTRY ) ) ) );


        }
        else
        {
            /*
            ** If the last license was removed from the slot then
            ** close the slot and then delete the entire slot.
            */
            ChkDR( DRM_DST_CloseSlot( pRemovalDateContext->pDatastore, &pRemovalDateContext->contextDSTSlot ) );
            fSlotOpen = FALSE;
            ChkDR( DRM_RDS_EnumDeleteCurrent ( f_pRemovalDateEnumContext ) );
        }
    }

ErrorExit:
    if (fSlotOpen)
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = DRM_DST_CloseSlot( pRemovalDateContext->pDatastore, &pRemovalDateContext->contextDSTSlot );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


/*********************************************************************
** Function: _SetRemovalDateStoreData
**
** Synopsis: Adds an entry into the removal date store with the
**           specified values.
**
** Parameters:
**
** [f_pcontextRemovalDate]  -- Context for the removal date store
** [f_dwRemovalDate]        -- Removal date to be added
** [f_pKID]                 -- KID of the license for which the removal
**                             date entry is being added
** [f_pLID]                 -- LID of the license for which the removal
**                             date entry is being added
** [f_pLicenseSlotHint]     -- Slot hint for the license for which the
**                             removal date entry is being added
** [f_pDateSlotHint]        -- Optional. Specifies a slot hint for the
**                             slot to open in the removal date namespace.
** [f_fCreateIfNotExisting] -- Indicates if a new slot should be created
**                             if one does not already exist
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _SetRemovalDateStoreData(
    __in_ecount( 1 )           DRM_REMOVAL_DATE_CONTEXT  *f_pContextRemovalDate,
    __in                       DRM_DWORD                  f_dwRemovalDate,
    __in_ecount( 1 )     const DRM_KID                   *f_pKID,
    __in_ecount( 1 )     const DRM_LID                   *f_pLID,
    __in_ecount( 1 )           DRM_DST_SLOT_HINT         *f_pLicenseSlotHint,
    __in_ecount_opt( 1 )       DRM_DST_SLOT_HINT         *f_pDateSlotHint,
    __in                       DRM_BOOL                   f_fCreateIfNotExisting )
{
    DRM_RESULT                        dr                  = DRM_SUCCESS;
    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pcontextRemovalDate = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) f_pContextRemovalDate;
    DRM_DWORD                         cbSlot              = 0;
    DRM_DWORD                         cbIO                = 0;
    DRM_DWORD                         cbRequired          = 0;
    DRM_BOOL                          fSlotOpen           = FALSE;
    DRM_BOOL                          fNewSlot            = FALSE;
    DRM_DST_KEY                       dstKey              = { 0 };
    DRM_DWORD                         dwNumLicensesInSlot = 0;

    ChkArg( f_pContextRemovalDate != NULL );
    ChkArg( f_pKID != NULL );
    ChkArg( f_pLID != NULL );
    ChkArg( f_pLicenseSlotHint != NULL );
    ChkArg( pcontextRemovalDate->pDatastore != NULL );

    if( f_fCreateIfNotExisting )
    {
        /* Calculate the needed size of the slot */
        cbSlot = DRM_MIN_REMOVAL_DATE_STORE_ENTRY_SIZE;
    }

    /*
    ** Convert the removal date ( a DWORD) into a DRM_ID
    */
    DRMCASSERT(SIZEOF( DRM_DST_KEY ) >= SIZEOF( DRM_DWORD ) );
    DWORD_TO_BYTES( dstKey.rgb, f_dwRemovalDate );

    dr = DRM_DST_OpenSlot( pcontextRemovalDate->pDatastore,
                          &pcontextRemovalDate->contextNameSpace,
                           ( f_fCreateIfNotExisting ? eDRM_DST_OPEN_ALWAYS : eDRM_DST_OPEN_EXISTING )| eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                          &dstKey,
                           f_pKID,
                           f_pDateSlotHint,
                          &cbSlot,
                          &pcontextRemovalDate->contextDSTSlot);
    if( f_fCreateIfNotExisting && dr == DRM_S_FALSE )
    {
        fNewSlot = TRUE;
    }
    ChkDR( dr );
    fSlotOpen = TRUE;

    if( cbSlot < DRM_MIN_REMOVAL_DATE_STORE_ENTRY_SIZE )
    {
        /* We need at least the minimum entry size for a valid entry */
        return DRM_E_DSTCORRUPTED;
    }

    if ( fNewSlot )
    {
        dwNumLicensesInSlot = 1;
    }
    else
    {
        /* Add a license to the current set of licenses. */
        /* If it's an existing slot, we need to read the size of the LAINFO currently stored */
        ChkDR( DRM_DST_SlotRead( pcontextRemovalDate->pDatastore,
                                &pcontextRemovalDate->contextDSTSlot,
                                 SIZEOF( DRM_DWORD ),
                                 (DRM_BYTE*)&dwNumLicensesInSlot,
                                 &cbIO ) );
        ChkBOOL( cbIO == SIZEOF( DRM_DWORD ), DRM_E_FILEREADERROR );
        FIX_ENDIAN_DWORD( dwNumLicensesInSlot );
        dwNumLicensesInSlot++;

        /*
        ** Calculate how much space is required in the slot
        */
        cbRequired = DRM_MIN_REMOVAL_DATE_STORE_ENTRY_SIZE + ( ( dwNumLicensesInSlot - 1 ) * DRM_REMOVAL_DATE_LICENSE_ENTRY_SIZE );

        /*
        ** If there is not enough space in the slot for the new entry then resize the slot
        */
        if ( cbRequired > cbSlot )
        {
            cbSlot = cbRequired;

            ChkDR( DRM_DST_SlotResize( pcontextRemovalDate->pDatastore,
                                      &pcontextRemovalDate->contextDSTSlot,
                                       cbSlot ) );
        }
    }

    /*
    ** Store the license count.  The license count has already been read so seek
    ** back to where it should be written.
    */

    /* Seek back to the beginning of the slot in order to write the license count */
    ChkDR( DRM_DST_SlotSeek( pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextDSTSlot,
                             0,
                             eDRM_DST_SEEKSET,
                             NULL ) );

    FIX_ENDIAN_DWORD( dwNumLicensesInSlot );
    ChkDR( DRM_DST_SlotWrite( pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextDSTSlot,
                              SIZEOF( DRM_DWORD ),
                              (DRM_BYTE*)&dwNumLicensesInSlot,
                              &cbIO ) );
    ChkBOOL( cbIO == SIZEOF( DRM_DWORD ), DRM_E_FILEWRITEERROR );
    FIX_ENDIAN_DWORD( dwNumLicensesInSlot ); /* Restore dwNumLicensesInSlot to keep using it */

    /*
    ** Store the LID
    */

    /*
    ** Seek to the end of the current license list
    */
    ChkDR( DRM_DST_SlotSeek( pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextDSTSlot,
                             DRM_REMOVAL_DATE_LICENSE_ENTRY_SIZE * ( dwNumLicensesInSlot - 1),
                             eDRM_DST_SEEKCUR,
                             NULL ) );

    ChkDR( DRM_DST_SlotWrite(  pcontextRemovalDate->pDatastore,
                              &pcontextRemovalDate->contextDSTSlot,
                               SIZEOF( DRM_LID ),
                               (DRM_BYTE*) f_pLID,
                              &cbIO ) );

    ChkBOOL( cbIO == SIZEOF( DRM_LID ), DRM_E_FILEWRITEERROR );

    /*
    ** Store the slot hint.  No need to seek since the slot hint
    ** is written immediately after the LID (which was just written).
    */
    ChkDR( DRM_DST_SlotWrite(  pcontextRemovalDate->pDatastore,
                              &pcontextRemovalDate->contextDSTSlot,
                               SIZEOF( DRM_DST_SLOT_HINT ),
                               (DRM_BYTE*) f_pLicenseSlotHint,
                              &cbIO ) );

    ChkBOOL( cbIO == SIZEOF( DRM_DST_SLOT_HINT ), DRM_E_FILEWRITEERROR );

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( pcontextRemovalDate->pDatastore,
                                   &pcontextRemovalDate->contextNameSpace ) );
#endif

ErrorExit:
    if (fSlotOpen)
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = DRM_DST_CloseSlot( pcontextRemovalDate->pDatastore, &pcontextRemovalDate->contextDSTSlot );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


/*********************************************************************
** Function: DRM_RDS_EnumDeleteCurrent
**
** Synopsis: deletes the currently enumerated slot from the removal
**           date store.
**
** Parameters:
**
** [f_pcontextRemovalDateEnum]  -- Enum context for the removal date store
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_EnumDeleteCurrent (
    __inout_ecount( 1 )  DRM_REMOVAL_DATE_ENUM    *f_pcontextRemovalDateEnum )
{
    DRM_RESULT                     dr                      = DRM_SUCCESS;
    DRM_BOOL                       fSlotOpen               = FALSE;
    DRM_REMOVAL_DATE_ENUM_PRIVATE *pcontextRemovalDateEnum = (DRM_REMOVAL_DATE_ENUM_PRIVATE *) f_pcontextRemovalDateEnum;

    ChkArg( f_pcontextRemovalDateEnum != NULL );

    dr = DRM_DST_SlotEnumReloadCurrent(  pcontextRemovalDateEnum->pcontextRemovalDate->pDatastore,
                                        &pcontextRemovalDateEnum->contextDSTEnum,
                                        &pcontextRemovalDateEnum->contextDSTSlot,
                                         NULL,
                                         NULL,
                                         NULL );

    ChkBOOL((dr != DRM_E_DSTBLOCKMISMATCH && dr != DRM_E_DSTSLOTNOTFOUND), DRM_E_SECURESTORE_CORRUPT);
    ChkDR(dr);
    fSlotOpen = TRUE;

    ChkDR( DRM_DST_SlotEnumDeleteCurrent(  pcontextRemovalDateEnum->pcontextRemovalDate->pDatastore,
                                          &pcontextRemovalDateEnum->contextDSTEnum,
                                          &pcontextRemovalDateEnum->contextDSTSlot ) );
    fSlotOpen = FALSE;

ErrorExit:
    if (fSlotOpen)
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = DRM_DST_CloseSlot( pcontextRemovalDateEnum->pcontextRemovalDate->pDatastore,
                                &pcontextRemovalDateEnum->contextDSTSlot );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


/*********************************************************************
** Function: DRM_RDS_DeleteLicenseEntry
**
** Synopsis: delete the indicated KID/LID combination from
**           the removal date store; its absence is not an error
**
** Parameters:
**
** [f_dwRemovalDate]        -- Removal date to be deleted
** [f_pKID]                 -- KID to be deleted
** [f_pKID]                 -- LID to be deleted
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_DeleteLicenseEntry(
    __inout_ecount( 1 )        DRM_DST     *f_pDatastore,
    __in_ecount( 1 )     const DRM_KID     *f_pKID,
    __in_ecount_opt( 1 ) const DRM_LID     *f_pLID )
{
    DRM_RESULT                 dr                     = DRM_SUCCESS;
    DRM_BOOL                   fStoreOpened           = FALSE;
    DRM_DWORD                  dwRemovalDate          = 0;
    DRM_DWORD                  cLicenses              = 0;
    DRM_KID                    KID                    = { 0 };
    DRM_REMOVAL_DATE_CONTEXT   removalDateContext     = { 0 };
    DRM_REMOVAL_DATE_ENUM      removalDateEnumContext = { 0 };
    DRM_DST_SLOT_HINT          dstSlotHint            = { 0 };

    ChkArg( f_pKID != NULL );

    ChkDR( DRM_RDS_OpenStore( f_pDatastore, &removalDateContext ) );
    fStoreOpened = TRUE;

    ChkDR( DRM_RDS_InitEnum( &removalDateContext,
                              ( DRM_DST_LOCKMODE ) ( eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT ),
                             &removalDateEnumContext ) );

    while( TRUE )
    {
        /*
        **  Look for next available removal date entry
        */
        ChkDR( DRM_RDS_EnumNext( &removalDateEnumContext,
                                 &dwRemovalDate,
                                 &KID,
                                 &dstSlotHint,
                                 &cLicenses ) );

        /*
        ** If the KID matches the specified KID
        */
        if ( MEMCMP( KID.rgb, f_pKID->rgb, SIZEOF( DRM_KID ) ) == 0 )
        {
            if ( f_pLID == NULL )
            {
                /*
                ** Delete the entire slot.
                */
                ChkDR( DRM_RDS_EnumDeleteCurrent ( &removalDateEnumContext ) );
            }
            else
            {
                /*
                ** Look for a matching LID.  If found, remove the LID
                ** and slot hint.
                */
                ChkDR( _RemoveLID( &removalDateEnumContext,
                                    dwRemovalDate,
                                   &KID,
                                   &dstSlotHint,
                                    f_pLID ) );
            }
        }
    }

ErrorExit:

    /*
    ** DRM_E_NOMORE is not an error
    */
    if ( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }

    if ( fStoreOpened )
    {
        DRM_RESULT dr2 = DRM_RDS_CloseStore( &removalDateContext );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    return dr;
}



/*********************************************************************
** Function: DRM_RDS_Delete
**
** Synopsis: delete the indicated removal date/KID combination from
**           the removal date store; its absence is not an error
**
** Parameters:
**
** [f_pcontextRemovalDate]  -- Context for the removal date store
** [f_dwRemovalDate]        -- Removal date to be deleted
** [f_pKID]                 -- KID to be deleted
** [f_pslotHint]            -- Optional.  Slot hint of hte removal date/
**                             KID combination.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_Delete(
    __inout_ecount( 1 )        DRM_REMOVAL_DATE_CONTEXT *f_pcontextRemovalDate,
    __in                       DRM_DWORD                 f_dwRemovalDate,
    __in_ecount( 1 )     const DRM_KID                  *f_pKID,
    __in_ecount_opt( 1 )       DRM_DST_SLOT_HINT        *f_pslotHint )
{
    DRM_RESULT                        dr                  = DRM_SUCCESS;
    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pcontextRemovalDate = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) f_pcontextRemovalDate;
    DRM_DST_KEY                       dstKey              = { 0 };

    ChkArg( f_pcontextRemovalDate != NULL );
    ChkArg( f_pKID != NULL );

    /*
    ** Convert the removal date ( a DWORD) into a DRM_ID
    */
    DRMCASSERT( SIZEOF( DRM_DST_KEY ) >= SIZEOF( DRM_DWORD ) );
    DWORD_TO_BYTES( dstKey.rgb, f_dwRemovalDate );

    dr = DRM_DST_DeleteSlot(  pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextNameSpace,
                             &dstKey,
                              f_pKID,
                              f_pslotHint,
                              TRUE );

    if (dr == DRM_E_DSTSLOTNOTFOUND)
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: DRM_RDS_OpenStore
**
** Synopsis: Opens the removal date store.
**
** Arguments:
**
** [f_pDatastore]           -- Datastore in which the removal date store exists
** [f_pcontextRemovalDate]  -- Context for the removal date store
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_OpenStore(
    __inout_ecount( 1 )  DRM_DST                  *f_pDatastore,
    __out_ecount( 1 )    DRM_REMOVAL_DATE_CONTEXT *f_pcontextRemovalDateOut)
{
    DRM_RESULT                        dr                  =  DRM_SUCCESS;
    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pcontextRemovalDate = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) f_pcontextRemovalDateOut;

    ChkArg( f_pDatastore != NULL );
    ChkArg( f_pcontextRemovalDateOut != NULL );

    DRMCASSERT( SIZEOF( DRM_REMOVAL_DATE_CONTEXT_PRIVATE ) <= SIZEOF( DRM_REMOVAL_DATE_CONTEXT ) );

    /* open the namespace for the RemovalDate Store */
    ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                  &g_namespaceRemovalDateStore,
                                  (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                  DRM_REMOVAL_DATE_STORE_MAX_NUMBER_OF_CHILD_NODES,
                                  SIZEOF( DRM_LICENSE_STATE_DATA ) + DRM_REMOVAL_DATE_CONTENT_HEADER_SIZE,
                                  &pcontextRemovalDate->contextNameSpace ) );

    pcontextRemovalDate->fInited = TRUE;
    pcontextRemovalDate->pDatastore = f_pDatastore;

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: DRM_RDS_CloseStore
**
** Synopsis: Closes the removal date store.
**
** Arguments:
**
** [f_pcontextRemovalDate]  -- Context for the removal date store
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_CloseStore(
    __inout_ecount( 1 )  DRM_REMOVAL_DATE_CONTEXT *f_pcontextRemovalDate)
{
    DRM_RESULT                        dr                  = DRM_SUCCESS;
    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pcontextRemovalDate = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) f_pcontextRemovalDate;

    ChkArg( f_pcontextRemovalDate != NULL );

    if (pcontextRemovalDate->fInited)
    {
        ChkDR( DRM_DST_CloseNamespace( pcontextRemovalDate->pDatastore,
                                      &pcontextRemovalDate->contextNameSpace ) );
    }

    ZEROMEM(f_pcontextRemovalDate, SIZEOF( DRM_REMOVAL_DATE_CONTEXT ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: DRM_RDS_InitEnum
**
** Synopsis: Initializes the enumeration in the removal date store.
**
** Arguments:
**
** [f_pcontextRemovalDate]       -- Context for the removal date store
** [f_eLockMode]                 -- Specifies the lock mode to use for enumerated slots.
** [f_pcontextRemovalDateEnum]   -- Context providing information about the
**                                  enumeration.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_InitEnum(
    __inout_ecount( 1 )  DRM_REMOVAL_DATE_CONTEXT    *f_pcontextRemovalDate,
    __in                 DRM_DST_LOCKMODE             f_eLockMode,
    __out_ecount( 1 )    DRM_REMOVAL_DATE_ENUM       *f_pcontextRemovalDateEnum )
{
    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pcontextRemovalDate     = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) f_pcontextRemovalDate;
    DRM_REMOVAL_DATE_ENUM_PRIVATE    *pcontextRemovalDateEnum = (DRM_REMOVAL_DATE_ENUM_PRIVATE *) f_pcontextRemovalDateEnum;
    DRM_RESULT dr =  DRM_SUCCESS;

    ChkArg( f_pcontextRemovalDate     != NULL );
    ChkArg( f_pcontextRemovalDateEnum != NULL );
    ChkArg( pcontextRemovalDate->fInited );

    DRMCASSERT( SIZEOF( DRM_REMOVAL_DATE_CONTEXT_PRIVATE ) <= SIZEOF( DRM_REMOVAL_DATE_CONTEXT ) );

    ZEROMEM(f_pcontextRemovalDateEnum, SIZEOF( DRM_REMOVAL_DATE_ENUM ) );

    dr = DRM_DST_InitSlotEnum( pcontextRemovalDate->pDatastore,
                               &pcontextRemovalDate->contextNameSpace,
                               NULL,
                               f_eLockMode,
                               &pcontextRemovalDateEnum->contextDSTEnum);

    if (DRM_SUCCEEDED (dr))
    {
        pcontextRemovalDateEnum->fAny = TRUE;
    }
    else if (dr == DRM_E_DSTSLOTNOTFOUND)
    {
        /* No removal dates found. */

        pcontextRemovalDateEnum->fAny = FALSE;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }

    pcontextRemovalDateEnum->pcontextRemovalDate = pcontextRemovalDate;
    pcontextRemovalDateEnum->fInited             = TRUE;

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: DRM_RDS_EnumNext
**
** Synopsis: Enumerates the next entry in the removal date store.
**
** Arguments:
**
** [f_pcontextRemovalDateEnum]   -- Context providing information about the
**                                  enumeration.
** [f_pdwRemovalDate]            -- The removal date associated with the slot
** [f_pKID]                      -- The KID associated with the slot
** [f_pSlotHint]                 -- Optional.  The slot hint for the slot.
** [f_pcLicenses]                -- Optional.  The count of licenses in the slot.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_NOMORE when there are no more items in the store
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_EnumNext(
    __inout_ecount( 1 )   DRM_REMOVAL_DATE_ENUM    *f_pcontextRemovalDateEnum,
    __out_ecount( 1 )     DRM_DWORD                *f_pdwRemovalDate,
    __out_ecount( 1 )     DRM_KID                  *f_pKID,
    __out_ecount_opt( 1 ) DRM_DST_SLOT_HINT        *f_pslotHint,
    __out_ecount_opt( 1 ) DRM_DWORD                *f_pcLicenses )
{
    DRM_REMOVAL_DATE_ENUM_PRIVATE *pcontextRemovalDateEnum = (DRM_REMOVAL_DATE_ENUM_PRIVATE *) f_pcontextRemovalDateEnum;
    DRM_RESULT                     dr                      = DRM_SUCCESS;
    DRM_DWORD                      cbSlot                  = SIZEOF( DRM_DWORD );
    DRM_DST                       *pDatastore              = NULL;
    DRM_DST_KEY                    dstRemovalDate          = { 0 };
    DRM_BOOL                       fSlotOpen               = FALSE;

    ChkArg( f_pcontextRemovalDateEnum != NULL );
    ChkArg( pcontextRemovalDateEnum->fInited);
    ChkArg( f_pdwRemovalDate != NULL );
    ChkArg( f_pKID != NULL );

    pDatastore = pcontextRemovalDateEnum->pcontextRemovalDate->pDatastore;

    DRMCASSERT( SIZEOF( DRM_REMOVAL_DATE_CONTEXT_PRIVATE ) <= SIZEOF( DRM_REMOVAL_DATE_CONTEXT ) );

    if  (pcontextRemovalDateEnum->fAny == FALSE)
    {
        dr = DRM_E_NOMORE;
        goto ErrorExit;
    }

    dr = DRM_DST_SlotEnumNext( pDatastore,
                               &pcontextRemovalDateEnum->contextDSTEnum,
                               &pcontextRemovalDateEnum->contextDSTSlot,
                               &dstRemovalDate,
                                f_pKID,
                               &cbSlot );

    if (   dr == DRM_E_DSTBLOCKMISMATCH
        || dr == DRM_E_DSTSLOTNOTFOUND)
    {
        ChkDR( DRM_E_NOMORE );
    }
    ChkDR( dr );
    fSlotOpen = TRUE;

    /*
    ** Convert the Removal date DRM_ID back to a DRM_DWORD
    */
    DRMCASSERT( SIZEOF( DRM_DST_KEY ) >= SIZEOF( DRM_DWORD ) );
    BYTES_TO_DWORD( *f_pdwRemovalDate, dstRemovalDate.rgb );

    if( f_pslotHint != NULL )
    {
        ChkDR( DRM_DST_MakeSlotHint( pDatastore,
                                    &pcontextRemovalDateEnum->contextDSTSlot,
                                     f_pslotHint ) );
    }

    if (f_pcLicenses != NULL )
    {
        DRM_DWORD cbRead = 0;

        /*
        ** Read the number of licenses
        */
        ChkDR( DRM_DST_SlotRead(  pDatastore,
                                 &pcontextRemovalDateEnum->contextDSTSlot,
                                  SIZEOF( DRM_DWORD ),
                                  (DRM_BYTE*) f_pcLicenses,
                                 &cbRead ) );

        FIX_ENDIAN_DWORD( *f_pcLicenses );
        ChkBOOL( cbRead == SIZEOF( DRM_DWORD), DRM_E_FILEREADERROR );
    }

ErrorExit:

    if ( fSlotOpen )
    {
        ( DRM_VOID )DRM_DST_CloseSlot( pDatastore, &pcontextRemovalDateEnum->contextDSTSlot );
    }

    return dr;
}


/*********************************************************************
**
** Function: DRM_RDS_AddRemovalDate
**
** Synopsis: Adds a removal date entry for the specified KID/LID.
**
** Arguments:
**
** [f_pDatastore]           -- Datastore to which the removal date will be added
** [f_pDstNamespaceContext] -- Optional.  Namespace context where the license is
**                             storied.  This is only required if f_pSlotHInt
**                             is NULL.  Will be used in generating a slot hint
**                             to be stored with the KID/LID.
** [f_dwRemovalDate         -- Removal date.  Number of seconds since 1/1/1970
** [f_pKID]                 -- The KID for which the removal date is being added
** [f_pLID]                 -- The LID for which the removal date is being added
** [f_pSlotHint]            -- Optional.  If not specified then a slot hint will
**                             be generated.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_AddRemovalDate(
    __inout_ecount( 1 )        DRM_DST                    *f_pDatastore,
    __in_ecount_opt( 1 )       DRM_DST_NAMESPACE_CONTEXT  *f_pDstNamespaceContext,
    __in                       DRM_DWORD                   f_dwRemovalDate,
    __in_ecount( 1 )           DRM_KID                    *f_pKID,
    __in_ecount( 1 )     const DRM_LID                    *f_pLID,
    __in_ecount_opt( 1 )       DRM_DST_SLOT_HINT          *f_pSlotHint )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_RESULT               dr2                = DRM_SUCCESS;
    DRM_REMOVAL_DATE_CONTEXT removalDateContext = { 0 };
    DRM_DST_SLOT_HINT        dstSlotHint        = { 0 };
    DRM_DST_SLOT_CONTEXT     dstSlotContext     = { 0 };
    DRM_DWORD                dwSize             = 0;
    DRM_BOOL                 fStoreOpened       = FALSE;
    DRM_BOOL                 fSlotOpened        = FALSE;

    ChkArg( f_pDatastore != NULL );
    ChkArg( f_pKID != NULL );
    ChkArg( f_pLID != NULL );

    /*
    ** If the caller did not specify a slot hint then make one now.
    */
    if ( f_pSlotHint == NULL )
    {
        ChkArg( f_pDstNamespaceContext != NULL );

        ChkDR( DRM_DST_OpenSlot ( f_pDatastore,
                                  f_pDstNamespaceContext,
                                  eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                                  f_pKID,
                                  f_pLID,
                                  NULL,
                                 &dwSize,
                                 &dstSlotContext ) );
        fSlotOpened = TRUE;

        ChkDR( DRM_DST_MakeSlotHint ( f_pDatastore,
                                     &dstSlotContext,
                                     &dstSlotHint ) );
    }

    ChkDR( DRM_RDS_OpenStore( f_pDatastore,
                              &removalDateContext) );
    fStoreOpened = TRUE;

    ChkDR( _SetRemovalDateStoreData( &removalDateContext,
                                      f_dwRemovalDate,
                                      f_pKID,
                                      f_pLID,
                                      f_pSlotHint != NULL ? f_pSlotHint : &dstSlotHint,
                                      NULL,
                                      TRUE ) );

ErrorExit:

    if ( fSlotOpened )
    {
        dr2 = DRM_DST_CloseSlot( f_pDatastore, &dstSlotContext );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    if ( fStoreOpened )
    {
        dr2 = DRM_RDS_CloseStore( &removalDateContext );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


/*********************************************************************
**
** Function: DRM_RDS_GetRemovalDateEntry
**
** Synopsis: Returns the license information associated with the specified
**           removal date/KID/slot hint.
**
** Arguments:
**
** [f_pcontextRemovalDate]   -- Context providing information about the
**                              removal date store
** [f_dwRemovalDate]         -- The removal date associated with the slot
** [f_pKID]                  -- The KID associated with the slot
** [f_pSlotHint]             -- Optional.  The slot hint for the slot.
** [f_pcbData]               -- On input size of f_pbData.  On output will
**                              contain the size of the license specific
**                              removal date data
** [f_pbData]                -- Returns the license specific removal date
**                              data.  On return, can be cast to
**                              DRM_REMOVAL_DATE_ENTRY*.
** [f_pcEntries]             -- The number of DRM_REMOVAL_DATE_ENTRY objects
**                              that are in f_pbData.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_BUFFERTOOSMALL if f_pbData is too small
**                          to hold all of the license specific data.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_GetRemovalDateEntry   (
   __inout_ecount( 1 )            DRM_REMOVAL_DATE_CONTEXT *f_pcontextRemovalDate,
   __in                           DRM_DWORD                 f_dwRemovalDate,
   __in_ecount( 1 )         const DRM_KID                  *f_pKID,
   __in_ecount_opt( 1 )           DRM_DST_SLOT_HINT        *f_pslotHint,
   __inout_ecount( 1 )            DRM_DWORD                *f_pcbData,
   __out_bcount(*f_pcbData)       DRM_BYTE                 *f_pbData,
   __out_ecount( 1 )              DRM_DWORD                *f_pcEntries )
{
    DRM_RESULT    dr            = DRM_SUCCESS;
    DRM_DWORD     cbSlot        = 0;
    DRM_DWORD     cbRead        = 0;
    DRM_DWORD     cbLicenseData = 0;
    DRM_BOOL      fSlotOpen     = FALSE;
    DRM_DST_KEY   dstKey        = { 0 };

    DRM_REMOVAL_DATE_CONTEXT_PRIVATE *pcontextRemovalDate = (DRM_REMOVAL_DATE_CONTEXT_PRIVATE *) f_pcontextRemovalDate;

    ChkArg( f_pcontextRemovalDate != NULL );
    ChkArg( f_pKID != NULL );
    ChkArg( f_pcEntries != NULL );
    ChkArg( f_pcbData != NULL );

    /*
    ** Convert the removal date ( a DWORD) into a DRM_ID
    */
    DRMCASSERT( SIZEOF( DRM_DST_KEY ) >= SIZEOF( DRM_DWORD ) );
    DWORD_TO_BYTES( dstKey.rgb, f_dwRemovalDate );

    ChkDR( DRM_DST_OpenSlot( pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextNameSpace,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                            &dstKey,
                             f_pKID,
                             f_pslotHint,
                             &cbSlot,
                             &pcontextRemovalDate->contextDSTSlot) );
    fSlotOpen = TRUE;

    /*
    ** Read the number of licenses
    */
    ChkDR( DRM_DST_SlotRead(  pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextDSTSlot,
                              SIZEOF( DRM_DWORD ),
                              (DRM_BYTE*) f_pcEntries,
                             &cbRead ) );

    FIX_ENDIAN_DWORD( *f_pcEntries );
    ChkBOOL( cbRead == SIZEOF( DRM_DWORD), DRM_E_FILEREADERROR );

    /*
    ** Check if f_pbData has enough space for all of the license information
    */
    cbLicenseData = *f_pcEntries * SIZEOF( DRM_REMOVAL_DATE_ENTRY );
    if ( *f_pcbData < cbLicenseData )
    {
        *f_pcbData = cbLicenseData;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_DST_SlotRead(  pcontextRemovalDate->pDatastore,
                             &pcontextRemovalDate->contextDSTSlot,
                              cbLicenseData,
                              f_pbData,
                             &cbRead ) );

    ChkBOOL( cbRead == cbLicenseData, DRM_E_FILEREADERROR );
    *f_pcbData = cbLicenseData;

ErrorExit:
    if( fSlotOpen )
    {
        /* Don't want to potentially overwrite a failure code so use dr2 */
        DRM_RESULT dr2 = DRM_SUCCESS;

        dr2 = DRM_DST_CloseSlot( pcontextRemovalDate->pDatastore,
                                &pcontextRemovalDate->contextDSTSlot );

        if ( DRM_FAILED( dr2 ) && DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }

    }

    return dr;
}


/**********************************************************************
** Function:    DRM_RDS_GetLicenseCount
**
** Synopsis:    Counts the number of licenses in the store that have
**              the specified removal date or an earlier removal date.
**
** Arguments:
**      [f_pDatastore]       : The datastore containing the removal date store
**      [f_pdwRemovalDate]   : Optional. The removal date to use when
**                             counting licenses.  If specified, all entries
**                             with a removal date <= *f_pdwRemovalDate will
**                             have the licenses counted.  If not specified
**                             then the licenses will be counted for all
**                             entries.
**      [f_pcLicenses]       : Returns the number of licenses that have a
**                             removal date of f_pdwRemovalDate or earlier.
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the required pointer parameters was NULL or the context
**          was not properly initialized
**
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RDS_GetLicenseCount(
    __inout_ecount( 1 )         DRM_DST     *f_pDatastore,
    __in_ecount_opt( 1 ) const  DRM_DWORD   *f_pdwRemovalDate,
    __out_ecount( 1 )           DRM_DWORD   *f_pcLicenses )
{
    DRM_RESULT               dr                      = DRM_SUCCESS;
    DRM_DWORD                dwRemovalDate           = 0;
    DRM_DWORD                cLicenses               = 0;
    DRM_BOOL                 fStoreOpened            = FALSE;
    DRM_KID                  KID                     = { 0 };
    DRM_DST_SLOT_HINT        dstSlotHint             = { 0 };
    DRM_REMOVAL_DATE_CONTEXT removalDateContext      = { 0 };
    DRM_REMOVAL_DATE_ENUM    contextRemovalDateEnum  = { 0 };

    ChkArg( f_pDatastore != NULL );
    ChkArg( f_pcLicenses != NULL );

    *f_pcLicenses = 0;

    ChkDR( DRM_RDS_OpenStore( f_pDatastore, &removalDateContext ) );
    fStoreOpened = TRUE;

    dr = DRM_RDS_InitEnum( &removalDateContext,
                            ( DRM_DST_LOCKMODE ) ( eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT ),
                           &contextRemovalDateEnum );

    /*
    ** DRM_E_NOMORE is not an error
    */
    if ( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }
    ChkDR( dr );


    while( TRUE )
    {
        /*
        **  Look for next available removal date entry
        */
        dr = DRM_RDS_EnumNext( &contextRemovalDateEnum,
                               &dwRemovalDate,
                               &KID,
                               &dstSlotHint,
                               &cLicenses );

        /*
        ** DRM_E_NOMORE is not an error
        */
        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }

        ChkDR( dr );

        /*
        ** If the removal date is less than or equal the specified date then
        ** increment the license count.
        */
        if ( f_pdwRemovalDate == NULL || *f_pdwRemovalDate >= dwRemovalDate )
        {
            *f_pcLicenses += cLicenses;
        }
    }

ErrorExit:

    if ( fStoreOpened )
    {
        DRM_RESULT dr2 = DRM_RDS_CloseStore( &removalDateContext );
        if ( DRM_SUCCEEDED( dr ) && DRM_FAILED( dr2 ) )
        {
            dr = dr2;
        }
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

