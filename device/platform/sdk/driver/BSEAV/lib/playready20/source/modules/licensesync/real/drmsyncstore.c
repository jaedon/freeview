/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcontextsizes.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmviewrights.h>
#include <drmheaderparser.h>
#include <drmchain.h>
#include <drmsyncstore.h>

ENTER_PK_NAMESPACE_CODE;

#define DRM_SYNC_STORE_CONTENT_HEADER_SIZE 300

static const DRM_DST_NAMESPACE g_namespaceSyncStore =
{
    's', 'y', 'n', 'c', 's', 't', 'o', 'r',
    'e', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

/*********************************************************************
** Function: DRM_SNC_DeleteKID
**
** Synopsis: delete the indicated KID from the sync store; its absense
**           is not an error
**
** Parameters:
**
** [f_pcontextSYN]          -- initialized SYNC context
** [f_pkid]                 -- pointer to KID to be added/updated
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_DeleteKID(
    IN       DRM_SYNC_CONTEXT *f_pcontextSYN,
    IN const DRM_KID          *f_pkid)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SYNC_CONTEXT_PRIVATE *pcontextSync = (DRM_SYNC_CONTEXT_PRIVATE *) f_pcontextSYN;

    ChkArg(f_pcontextSYN != NULL
        && f_pkid        != NULL);

    dr = DRM_DST_DeleteSlot( pcontextSync->pDatastore,
                             &pcontextSync->contextNameSpace,
                             f_pkid,
                             f_pkid,
                             NULL,
                             TRUE );

    if (dr == DRM_E_DSTSLOTNOTFOUND)
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR(dr);
    }

ErrorExit:
    return dr;
} /* DRM_SNC_DeleteKID */

/**********************************************************************
 *                     PUBLIC FUNCTION DRM_SNC_OpenStore
 **********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_OpenStore  (
    IN  DRM_DST                *f_pDatastore,
    OUT DRM_SYNC_CONTEXT       *f_pcontextSYN)
{
    DRM_RESULT                dr           =  DRM_SUCCESS;
    DRM_SYNC_CONTEXT_PRIVATE *pcontextSync = (DRM_SYNC_CONTEXT_PRIVATE *) f_pcontextSYN;

    ChkArg (f_pDatastore   != NULL
         && f_pcontextSYN != NULL);

    DRMCASSERT (SIZEOF (DRM_SYNC_CONTEXT_PRIVATE) <= SIZEOF (DRM_SYNC_CONTEXT));

    pcontextSync->pDatastore = f_pDatastore;

    /* open the namespace for the Sync Store */
    ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                  &g_namespaceSyncStore,
                                  (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                  16,
                                  SIZEOF( DRM_LICENSE_STATE_DATA ) + DRM_SYNC_STORE_CONTENT_HEADER_SIZE,
                                  &pcontextSync->contextNameSpace ) );
    pcontextSync->fInited = TRUE;

ErrorExit:
    return dr;
}

/**********************************************************************
 *                     PUBLIC FUNCTION DRM_SNC_CloseStore
 **********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_CloseStore (
    IN  DRM_SYNC_CONTEXT       *f_pcontextSYN)
{
    DRM_SYNC_CONTEXT_PRIVATE *pcontextSync = (DRM_SYNC_CONTEXT_PRIVATE *) f_pcontextSYN;
    DRM_RESULT    dr = DRM_SUCCESS;

    ChkArg (f_pcontextSYN != NULL);

    if (pcontextSync->fInited)
    {
        ChkDR( DRM_DST_CloseNamespace( pcontextSync->pDatastore,
                                       &pcontextSync->contextNameSpace ) );
    }

    ZEROMEM(f_pcontextSYN, SIZEOF (DRM_SYNC_CONTEXT));

ErrorExit:
    return dr;
}

/**********************************************************************
 *                    PUBLIC FUNCTION DRM_SNC_InitEnum
 ***********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_InitEnum   (
    IN  DRM_SYNC_CONTEXT       *f_pcontextSYN,
    OUT DRM_SYNC_ENUM_CONTEXT  *f_pcontextSYNEnum)
{
    DRM_SYNC_CONTEXT_PRIVATE      *pcontextSync     = (DRM_SYNC_CONTEXT_PRIVATE *)      f_pcontextSYN;
    DRM_SYNC_ENUM_CONTEXT_PRIVATE *pcontextSyncEnum = (DRM_SYNC_ENUM_CONTEXT_PRIVATE *) f_pcontextSYNEnum;
    DRM_RESULT dr =  DRM_SUCCESS;

    ChkArg(f_pcontextSYN         != NULL
        && f_pcontextSYNEnum     != NULL
        && pcontextSync->fInited == TRUE);

    DRMCASSERT (SIZEOF (DRM_SYNC_CONTEXT_PRIVATE) <= SIZEOF (DRM_SYNC_CONTEXT));

    ZEROMEM(f_pcontextSYNEnum, SIZEOF (DRM_SYNC_ENUM_CONTEXT));

    dr = DRM_DST_InitSlotEnum( pcontextSync->pDatastore,
                               &pcontextSync->contextNameSpace,
                               NULL,
                               (DRM_DST_LOCKMODE)(eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT),
                              &pcontextSyncEnum->contextDSTEnum);

    if (DRM_SUCCEEDED (dr))
    {
        pcontextSyncEnum->fAny = TRUE;
    }
    else if (dr == DRM_E_DSTSLOTNOTFOUND)
    {
        /* No KIDs found. */

        pcontextSyncEnum->fAny = FALSE;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR (dr);
    }

    pcontextSyncEnum->fInited      = TRUE;
    pcontextSyncEnum->pcontextSync = pcontextSync;

ErrorExit:
    return dr;
}

/**********************************************************************
 *                   PUBLIC FUNCTION DRM_SNC_EnumNext
 ***********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_EnumNext   (
    IN  DRM_SYNC_ENUM_CONTEXT  *f_pcontextSYNEnum,
    OUT DRM_KID                *f_pkid,
    OUT DRM_DST_SLOT_HINT      *f_pslotHint)
{
    DRM_SYNC_ENUM_CONTEXT_PRIVATE *pcontextSyncEnum = (DRM_SYNC_ENUM_CONTEXT_PRIVATE *) f_pcontextSYNEnum;
    DRM_DST_KEY                    keyHash          = {0};
    DRM_RESULT                     dr               = DRM_SUCCESS;
    DRM_DWORD                      cbSlot           = SIZEOF (DRM_DWORD);
    DRM_DST                       *pDatastore       = NULL;

    ChkArg (f_pcontextSYNEnum         != NULL
         && f_pkid                    != NULL
         && pcontextSyncEnum->fInited == TRUE
         && pcontextSyncEnum->pcontextSync != NULL
         && pcontextSyncEnum->pcontextSync->pDatastore != NULL);

    pDatastore = pcontextSyncEnum->pcontextSync->pDatastore;

    DRMCASSERT (SIZEOF (DRM_SYNC_CONTEXT_PRIVATE) <= SIZEOF (DRM_SYNC_CONTEXT));

    if  (pcontextSyncEnum->fAny == FALSE)
    {
        ChkDR(DRM_E_NOMORE);
    }

    dr = DRM_DST_SlotEnumNext( pDatastore,
                               &pcontextSyncEnum->contextDSTEnum,
                               &pcontextSyncEnum->contextDSTSlot,
                               &keyHash,
                               &pcontextSyncEnum->itemSync.kid,
                               &cbSlot );

    if (dr == DRM_E_DSTBLOCKMISMATCH
    ||  dr == DRM_E_DSTSLOTNOTFOUND)
    {
        ChkDR (DRM_E_SYNC_ENTRYNOTFOUND);
    }

    ChkDR (dr); /* includes DRM_E_NOMORE */

    MEMCPY (f_pkid->rgb,
            pcontextSyncEnum->itemSync.kid.rgb,
            SIZEOF (DRM_KID));

    if( f_pslotHint != NULL )
    {
        DRM_DST_MakeSlotHint( pDatastore,
                              &pcontextSyncEnum->contextDSTSlot,
                              f_pslotHint );
    }

    ChkDR( DRM_DST_CloseSlot( pDatastore, &pcontextSyncEnum->contextDSTSlot ) );

ErrorExit:
    return dr;
}

/*********************************************************************
 *                   INTERNAL FUNCTIONS DRM_SNC_GetStoreDataForKID
 *                                      DRM_SNC_SetStoreDataForKID
 *
 * purpose: get/set the data currently in the sync store for this KID
 *********************************************************************/


DRM_RESULT DRM_SNC_GetStoreDataForKID(
    __in                                      DRM_SYNC_CONTEXT           *f_pcontextSYN,
    __in const                                DRM_KID                    *f_pkid,
    __in_opt                                  DRM_DST_SLOT_HINT          *f_pslotHint,
    __out_opt                                 DRM_LICENSE_STATE_DATA     *f_plsd,
    __out_ecount_opt(*f_pcchContentHeader)    DRM_WCHAR                  *f_pwszContentHeader,
    __inout_opt                                   DRM_DWORD                  *f_pcchContentHeader)
{
    DRM_SYNC_CONTEXT_PRIVATE *pcontextSync = (DRM_SYNC_CONTEXT_PRIVATE *) f_pcontextSYN;
    DRM_DWORD   cbSlot   = SIZEOF (DRM_LICENSE_STATE_DATA   );
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BOOL   fSlotOpen = FALSE;
    DRM_WORD   iMember   = 0;
    DRM_DWORD  cchContentHeaderBuffer = 0;

    if( f_pcchContentHeader != NULL )
    {
        cchContentHeaderBuffer = *f_pcchContentHeader;
        *f_pcchContentHeader = 0;
    }

    dr = DRM_DST_OpenSlot( pcontextSync->pDatastore,
                           &pcontextSync->contextNameSpace,
                           eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                           f_pkid,
                           f_pkid,
                           f_pslotHint,
                           &cbSlot,
                           &pcontextSync->contextDSTSlot);

    if( dr == DRM_E_DSTSLOTNOTFOUND )
    {
         ChkDR (DRM_E_SYNC_ENTRYNOTFOUND);
    }
    else
    {
        ChkDR( dr );
        fSlotOpen = TRUE;

        if( f_plsd != NULL )
        {
            ChkDR( DRM_DST_SlotRead( pcontextSync->pDatastore,
                                     &pcontextSync->contextDSTSlot,
                                     SIZEOF (DRM_LICENSE_STATE_DATA ),
                         (DRM_BYTE *)f_plsd,
                                     &cbSlot ) );
        }
        else
        {
            /* Skip over the LSD */
            ChkDR( DRM_DST_SlotSeek( pcontextSync->pDatastore,
                                     &pcontextSync->contextDSTSlot,
                                     SIZEOF( DRM_LICENSE_STATE_DATA ),
                                     eDRM_DST_SEEKCUR,
                                     NULL ) );
        }
    }


    if( f_plsd != NULL )
    {
        FIX_ENDIAN_DWORD(f_plsd->dwCategory);
        FIX_ENDIAN_DWORD(f_plsd->dwStreamId);
        FIX_ENDIAN_DWORD(f_plsd->dwNumCounts);
        FIX_ENDIAN_DWORD(f_plsd->dwNumDates);
        FIX_ENDIAN_DWORD(f_plsd->dwVague);

        for (iMember = 0;
             iMember < NO_OF(f_plsd->datetime);
             iMember++)
        {
            DRM_UINT64 ui64;
            FILETIME_TO_UI64(f_plsd->datetime [iMember], ui64 );
            FIX_ENDIAN_QWORD(ui64);
            UI64_TO_FILETIME( ui64, f_plsd->datetime [iMember] );
        }

        for (iMember = 0;
             iMember < NO_OF(f_plsd->dwCount);
             iMember++)
        {
            FIX_ENDIAN_DWORD(f_plsd->dwCount [iMember]);
        }
    }

    if( f_pcchContentHeader != NULL )
    {
        /* read the number of characters of the ContentHeader */
        ChkDR( DRM_DST_SlotRead( pcontextSync->pDatastore,
                                 &pcontextSync->contextDSTSlot,
                                 SIZEOF( DRM_DWORD ),
                     (DRM_BYTE *)f_pcchContentHeader,
                                 &cbSlot ) );

        /* only process if the ContentHeader is actually present in the slot */
        if( cbSlot == SIZEOF( DRM_DWORD ) )
        {
            FIX_ENDIAN_DWORD(*f_pcchContentHeader);

            if( cchContentHeaderBuffer >= *f_pcchContentHeader
             && f_pwszContentHeader != NULL )
            {
                /* read the ContentHeader itself */
                ChkDR ( DRM_DST_SlotRead( pcontextSync->pDatastore,
                                          &pcontextSync->contextDSTSlot,
                                          *f_pcchContentHeader * SIZEOF ( DRM_WCHAR ),
                              (DRM_BYTE *)f_pwszContentHeader,
                                          &cbSlot ) );

                ChkBOOL( cbSlot == ( *f_pcchContentHeader * SIZEOF ( DRM_WCHAR ) ), DRM_E_FILEREADERROR );
            }
            else
            {
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
        }
        else
        {
            *f_pcchContentHeader = 0;
        }
    }

ErrorExit:
    if (fSlotOpen)
    {
        DRM_RESULT dr1 = DRM_SUCCESS;
        dr1 = DRM_DST_CloseSlot( pcontextSync->pDatastore,
                                &pcontextSync->contextDSTSlot );
        DRMASSERT( DRM_SUCCEEDED( dr1 ) );
    }

    return dr;
}

DRM_RESULT DRM_SNC_SetStoreDataForKID(
    IN       DRM_SYNC_CONTEXT          *f_pcontextSYN,
    IN const DRM_KID                   *f_pkid,
    IN       DRM_DST_SLOT_HINT         *f_pslotHint,
    IN const DRM_LICENSE_STATE_DATA    *f_plsd,
    IN const DRM_STRING                *f_pdstrContentHeader,
    IN       DRM_BOOL                   f_fCreateIfNotExisting )
{
    DRM_SYNC_CONTEXT_PRIVATE    *pcontextSync   = (DRM_SYNC_CONTEXT_PRIVATE *) f_pcontextSYN;
    DRM_LICENSE_STATE_DATA       lsd            = { 0 };
    DRM_DWORD                    cbSlot         = SIZEOF (DRM_LICENSE_STATE_DATA   );
    DRM_DWORD                    cbIO           = 0;
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_BOOL                     fSlotOpen      = FALSE;
    DRM_WORD                     iMember        = 0;
    DRM_BOOL                     fSlotNew       = FALSE;

    if( f_plsd != NULL )
    {
        if( f_plsd->dwCategory == DRM_LICENSE_STATE_UNLIM )
        {
            /* This license never expires, remove it from the sync list */
            ChkDR( DRM_SNC_DeleteKID( f_pcontextSYN, f_pkid ) );
            goto ErrorExit;

        }

        MEMCPY( &lsd, f_plsd, SIZEOF( lsd ) );

        FIX_ENDIAN_DWORD(lsd.dwCategory);
        FIX_ENDIAN_DWORD(lsd.dwStreamId);
        FIX_ENDIAN_DWORD(lsd.dwNumCounts);
        FIX_ENDIAN_DWORD(lsd.dwNumDates);
        FIX_ENDIAN_DWORD(lsd.dwVague);

        for (iMember = 0;
             iMember < NO_OF(lsd.datetime);
             iMember++)
        {
            DRM_UINT64 ui64;
            FILETIME_TO_UI64(lsd.datetime [iMember], ui64 );
            FIX_ENDIAN_QWORD(ui64);
            UI64_TO_FILETIME( ui64, lsd.datetime [iMember] );
        }

        for (iMember = 0;
             iMember < NO_OF(lsd.dwCount);
             iMember++)
        {
            FIX_ENDIAN_DWORD(lsd.dwCount [iMember]);
        }
    }
    else
    {
        /* Clear out the category to the default */
        lsd.dwCategory = DRM_LICENSE_STATE_FORCE_SYNC;
    }

    if( f_pdstrContentHeader != NULL
     && f_fCreateIfNotExisting)
    {
        cbSlot += SIZEOF( DRM_DWORD ) + (f_pdstrContentHeader->cchString * SIZEOF(DRM_WCHAR));
    }
    /* check to see if the entry already exists */
    ChkDR( DRM_DST_OpenSlot( pcontextSync->pDatastore,
                             &pcontextSync->contextNameSpace,
                             ( f_fCreateIfNotExisting ? eDRM_DST_OPEN_ALWAYS : eDRM_DST_OPEN_EXISTING ) | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                             f_pkid,
                             f_pkid,
                             f_pslotHint,
                             &cbSlot,
                             &pcontextSync->contextDSTSlot) );

    if( f_fCreateIfNotExisting && dr == DRM_S_FALSE )
    {
        fSlotNew = TRUE;
    }
    fSlotOpen = TRUE;

    /* If they gave us an LSD then write it to disk,
    ** or if it's a new slot clear out the LSD,
    ** otherwise skip over the current LSD
    */
    if( f_plsd != NULL
     || fSlotNew )
    {
        ChkDR( DRM_DST_SlotWrite( pcontextSync->pDatastore,
                                  &pcontextSync->contextDSTSlot,
                                  SIZEOF(DRM_LICENSE_STATE_DATA   ),
                      (DRM_BYTE *)&lsd,
                                  &cbIO));
        DRMASSERT( cbIO == SIZEOF( DRM_LICENSE_STATE_DATA   ) );
    }
    else
    {
        ChkDR( DRM_DST_SlotSeek( pcontextSync->pDatastore,
                                 &pcontextSync->contextDSTSlot,
                                 SIZEOF( DRM_LICENSE_STATE_DATA ),
                                 eDRM_DST_SEEKSET,
                                 NULL ) );
    }

    /* If they've specified an ContentHeader then let's store it */
    if( f_pdstrContentHeader != NULL )
    {
        DRM_DWORD  cchContentHeader = f_pdstrContentHeader->cchString;

        /* If the length is 0 then we don't care if they gave us a NULL */
        ChkArg( cchContentHeader == 0 || f_pdstrContentHeader->pwszString != NULL );

        FIX_ENDIAN_DWORD(cchContentHeader);

        /* If the current slot is too small or large then resize it  */
        if( cbSlot != ( SIZEOF( DRM_LICENSE_STATE_DATA    ) + SIZEOF( DRM_DWORD ) + (cchContentHeader * SIZEOF( DRM_WCHAR )) ) )
        {
            ChkDR( DRM_DST_SlotResize( pcontextSync->pDatastore,
                                       &pcontextSync->contextDSTSlot,
                                       ( SIZEOF( DRM_LICENSE_STATE_DATA    )
                                       + SIZEOF( DRM_DWORD )
                                       + (cchContentHeader * SIZEOF( DRM_WCHAR ) ) ) ) );

            /* The slot resize operation will reset our seek pointer back to 0, so we need to fix that */
            ChkDR( DRM_DST_SlotSeek( pcontextSync->pDatastore,
                                     &pcontextSync->contextDSTSlot,
                                     SIZEOF( DRM_LICENSE_STATE_DATA ),
                                     eDRM_DST_SEEKSET,
                                     NULL ) );
        }

        /* Write out the number of characters of the ContentHeader string */
        ChkDR( DRM_DST_SlotWrite( pcontextSync->pDatastore,
                                  &pcontextSync->contextDSTSlot,
                                  SIZEOF( DRM_DWORD ),
                      (DRM_BYTE *)&cchContentHeader,
                                  &cbSlot));

        if( cchContentHeader > 0 )
        {
            /* Write out the the ContentHeader string */
            ChkDR( DRM_DST_SlotWrite( pcontextSync->pDatastore,
                                      &pcontextSync->contextDSTSlot,
                                      cchContentHeader * SIZEOF( DRM_WCHAR ),
                          (DRM_BYTE *)f_pdstrContentHeader->pwszString,
                                      &cbSlot ) );
        }
    }

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( pcontextSync->pDatastore,
                                    &pcontextSync->contextNameSpace ) );
#endif

ErrorExit:
    if (fSlotOpen)
    {
        DRM_DST_CloseSlot( pcontextSync->pDatastore, &pcontextSync->contextDSTSlot );
    }

    return dr;
}


/*********************************************************************
** Function: DRM_SNC_GetSyncStoreEntry
**
** Synopsis: Looks up the ContentHeader for a given KID if it exists
**
** Parameters:
**
** [f_pcontextSYN]          -- initialized SYNC context
** [f_pkid]                 -- pointer to KID to be added/updated
** [f_plsd]                        -- optional pointer to a license state data struct
** [f_pwszContentHeader]           -- optional pointer to a buffer that will hold the ContentHeader
** [f_pcchContentHeader]           -- optional pointer to the number of wide characters in the ContentHeader
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_GetSyncStoreEntry(
    IN                                       DRM_SYNC_CONTEXT        *f_pcontextSYN,
    IN const                                 DRM_KID                 *f_pkid,
       OUT                                   DRM_LICENSE_STATE_DATA  *f_plsd,
       __out_ecount_opt(*f_pcchContentHeader)DRM_WCHAR               *f_pwszContentHeader,
    IN OUT                                   DRM_DWORD               *f_pcchContentHeader)
{
    DRM_RESULT dr        = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMSYNCSTORE, PERF_FUNC_DRM_SNC_GetSyncStoreEntry );

    ChkArg ( f_pcontextSYN != NULL
         && f_pkid        != NULL );

    ChkDR( DRM_SNC_GetStoreDataForKID( f_pcontextSYN, f_pkid, NULL, f_plsd, f_pwszContentHeader, f_pcchContentHeader ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
** Function: DRM_SNC_UpdateKID
**
** Synopsis: force the updating of the license state data of a KID
**           entry in the sync store.
**         * if the pcontextViewRightsIn parameter is not NULL, the
**           license store is read for the data
**         * if the pcontextViewRightsIn parameter is NULL, the data
**           is cleared.  This typically follows the deletion of a
**           license
**
** Parameters:
**
** [f_pcontextSYN]          -- initialized SYNC context
** [f_pcontextViewRightsIn] -- optional VIEWRIGHTS context (see above)
** [f_pkid]                 -- pointer to KID to be added/updated
** [f_pdstrContentHeader]   -- content header to be stored
** [f_eSyncCategory]        -- determines how the License State Category is set:
**                             DRM_SYNC_UPDATE_AGGREGATE -- Determines the category by aggregating
**                                 license state data
**                             DRM_SYNC_UPDATE_FORCE_SYNC -- sets the category to force a synchronization
**                                 with the license store the next time it can
**                             DRM_SYNC_UPDATE_NO_LICENSE -- Sets the category to NO_LICENSE
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_UpdateKID(
    IN       DRM_SYNC_CONTEXT          *f_pcontextSYN,
    IN       DRM_VIEW_RIGHTS_CONTEXT   *f_pcontextViewRightsIn,
    IN const DRM_KID                   *f_pkid,
    IN const DRM_CONST_STRING          *f_pdstrContentHeader,
    IN       DRM_SYNC_UPDATE_CATEGORY   f_eSyncCategory )
{
    DRM_LICENSE_STATE_DATA       lsd            = { 0 };
    DRM_SYNC_CONTEXT_PRIVATE    *pcontextSync   = (DRM_SYNC_CONTEXT_PRIVATE *) f_pcontextSYN;
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_KID                      kid            = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMSYNCSTORE, PERF_FUNC_DRM_SNC_UpdateKID );

    ChkArg ( f_pcontextSYN != NULL );

    if(    f_pdstrContentHeader == NULL
        && f_pcontextViewRightsIn != NULL
        && f_pcontextViewRightsIn->pLicEval != NULL )
    {
        /* They didn't explicitly give us a content header, but we can grab it from the liceval */
        f_pdstrContentHeader = &f_pcontextViewRightsIn->pLicEval->dstrContentHeader;

        /* We should validate it first though */
        if( f_pdstrContentHeader != NULL
         && f_pdstrContentHeader->pwszString != NULL
         && f_pdstrContentHeader->cchString > 0 )
        {
            DRM_STRING dstrKID = EMPTY_DRM_STRING;

            /* Use the Content Header only if the current content header has the correct KID */
            if( DRM_SUCCEEDED( DRM_HDR_GetAttribute(
                f_pdstrContentHeader,
                NULL,
                DRM_HEADER_ATTRIB_KID,
                (DRM_CONST_STRING*)&dstrKID,
                0) ) )
            {
                ChkDR( DRM_UTL_DecodeKID( (DRM_CONST_STRING*)&dstrKID, &kid ) );

                if( f_pkid != NULL )
                {
                    if( MEMCMP( &kid, f_pkid, SIZEOF( DRM_KID ) ) != 0 )
                    {
                        f_pdstrContentHeader = NULL;
                    }
                }
                else
                {
                    /* They didn't explicitly give us a KID, so we use what's in the header */
                    f_pkid = &kid;
                }
            }
            else
            {
                f_pdstrContentHeader = NULL;
            }
        }
    }

    ChkArg( f_pkid != NULL );

    switch( f_eSyncCategory )
    {
    case DRM_SYNC_UPDATE_AGGREGATE:
        if( f_pcontextViewRightsIn != NULL )
        {
            /*
            ** If they gave us a view rights context, then try to update the LSD
            */
            MEMCPY(&f_pcontextViewRightsIn->KID, f_pkid, SIZEOF (DRM_KID));

            ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData( g_apdstrActions,
                                                           &lsd,
                                                           NO_OF( g_apdstrActions ),
                                                           f_pcontextViewRightsIn,
                                                           pcontextSync->pDatastore,
                                                           TRUE,
                                                           DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES,
                                                           NULL,
                                                           NULL ) );

            /*
            ** If this is an UNLIMITED license, and there already was a sync entry for this KID
            ** mark the sync entry to be deleted next time sync list is generated.
            */
            if ( lsd.dwCategory == DRM_LICENSE_STATE_UNLIM )
            {
                dr = DRM_SNC_GetStoreDataForKID( f_pcontextSYN, f_pkid, NULL, NULL, NULL, NULL);
                if( DRM_SUCCEEDED(dr) )
                {
                    lsd.dwCategory = DRM_LICENSE_STATE_ENTRY_MARKED_FOR_DELETION;
                }
                dr = DRM_SUCCESS;
            }
            else
            {
                (DRM_VOID)DRM_SNC_SetStoreDataForKID( f_pcontextSYN, f_pkid, NULL, &lsd, (DRM_STRING*)f_pdstrContentHeader, TRUE );
            }
        }
        else
        {
            /*
            ** Leave the LSD as-is and just update the content header
            */
            (DRM_VOID)DRM_SNC_SetStoreDataForKID(f_pcontextSYN, f_pkid, NULL, NULL, (DRM_STRING*)f_pdstrContentHeader, TRUE);
        }
        break;
    case DRM_SYNC_UPDATE_NO_LICENSE:
        /*
        ** A call with a DRM_SYNC_UPDATE_NO_LICENSE means that the KID should
        ** be stored in the synclist and a license should be requested.
        */
        lsd.dwCategory = DRM_LICENSE_STATE_NOT_FOUND;
        (DRM_VOID)DRM_SNC_SetStoreDataForKID(f_pcontextSYN, f_pkid, NULL, &lsd, (DRM_STRING*)f_pdstrContentHeader, TRUE);
        break;
    case DRM_SYNC_UPDATE_FORCE_SYNC:
        /*
        ** A call with a DRM_SYNC_UPDATE_FORCE_SYNC means that the KID should
        ** be stored in the synclist and updated from the license store
        ** next time the sync list is generated.
        */
        lsd.dwCategory = DRM_LICENSE_STATE_FORCE_SYNC;
        (DRM_VOID)DRM_SNC_SetStoreDataForKID(f_pcontextSYN, f_pkid, NULL, &lsd, (DRM_STRING*)f_pdstrContentHeader, TRUE);
        break;
    default:
        ChkArg( FALSE );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
} /* DRM_SNC_UpdateKID */

EXIT_PK_NAMESPACE_CODE;

