/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMETERCERTSTORE_C
#include <drmmetercertstore.h>
#include <drmviewrights.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_DST_NAMESPACE g_namespaceMeterCertStore =
{
    'm', 'e', 't', 'e', 'r', 'c', 'e', 'r',
    't', 's', 't', 'o', 'r', 'e', '\0', '\0'
};

#define DRM_METERCERT_STORE_SLOT_SIZE 2000

#define DRM_MIN_METERCERT_STORE_ENTRY_SIZE ( SIZEOF( DRM_DWORD ) * 3 )
#define DRM_METERCERT_FLAG_ACQ_PROTO_IS_V2 1

/*********************************************************************
 *                   INTERNAL FUNCTIONS _GetMIDStoreData
 *                                      _SetMIDStoreData
 *
 * purpose: get/set the data currently in the meter cert store for this MID
 *********************************************************************/


static DRM_RESULT _GetMIDStoreData(
    __in                                    DRM_METERCERT_CONTEXT   *f_pcontextMeterCert,
    __in                              const DRM_MID                 *f_pmid,
    __in_opt                                DRM_DST_SLOT_HINT       *f_pslotHint,
    __out_ecount_opt(*f_pcchLAINFO)         DRM_WCHAR               *f_pwszLAINFO,
    __inout_opt                             DRM_DWORD               *f_pcchLAINFO,
    __out_ecount_opt(1)                     DRM_BOOL                *f_pfIsV2,
    __out_ecount_opt(*f_pcbMeterCert)       DRM_BYTE                *f_pbMeterCert,
    __inout_opt                             DRM_DWORD               *f_pcbMeterCert )
{
    DRM_METERCERT_CONTEXT_PRIVATE *pcontextMeterCert = (DRM_METERCERT_CONTEXT_PRIVATE *) f_pcontextMeterCert;
    DRM_DWORD   cbSlot   = SIZEOF (DRM_LICENSE_STATE_DATA   );
    DRM_DWORD   cbIO     = 0;
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_RESULT drCached = DRM_SUCCESS;
    DRM_DWORD  cbData   = 0;
    DRM_BOOL   fSlotOpen = FALSE;
    DRM_BOOL   fSkipData = FALSE;
    DRM_DWORD  dwFlags   = 0;

    ChkArg( f_pcontextMeterCert != NULL
        &&  f_pmid              != NULL );

    if( f_pwszLAINFO != NULL )
    {
        ChkArg( f_pcchLAINFO != NULL );
    }

    if( f_pbMeterCert != NULL )
    {
        ChkArg( f_pcbMeterCert != NULL );
    }

    ChkDR( DRM_DST_OpenSlot( pcontextMeterCert->pDatastore,
                             &pcontextMeterCert->contextNameSpace,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                             f_pmid,
                             f_pmid,
                             f_pslotHint,
                             &cbSlot,
                             &pcontextMeterCert->contextDSTSlot) );
    fSlotOpen = TRUE;

    if( cbSlot < DRM_MIN_METERCERT_STORE_ENTRY_SIZE )
    {
        ChkDR( DRM_E_DST_CORRUPTED );
    }

    /*
    ** Read the flags
    */
    ChkDR( DRM_DST_SlotRead( pcontextMeterCert->pDatastore,
                             &pcontextMeterCert->contextDSTSlot,
                             SIZEOF( DRM_DWORD ),
                             (DRM_BYTE *)&dwFlags,
                             &cbIO ) );
    FIX_ENDIAN_DWORD(dwFlags);
    if( f_pfIsV2 != NULL )
    {
        *f_pfIsV2 = (dwFlags & DRM_METERCERT_FLAG_ACQ_PROTO_IS_V2) ? TRUE : FALSE;
    }

    /*
    ** Read the LAINFO
    */

    /* read the number of characters of the LAINFO */
    ChkDR( DRM_DST_SlotRead( pcontextMeterCert->pDatastore,
                             &pcontextMeterCert->contextDSTSlot,
                             SIZEOF( DRM_DWORD ),
                             (DRM_BYTE *)&cbData,
                             &cbIO ) );
    FIX_ENDIAN_DWORD(cbData);


    fSkipData = TRUE;
    if( f_pcchLAINFO != NULL )
    {
        /* They want either just the size of, or the actual LAINFO */

        if( cbData <= *f_pcchLAINFO * SIZEOF( DRM_WCHAR )
         && f_pwszLAINFO != NULL )
        {
            /* read the LAINFO itself */
            ChkDR( DRM_DST_SlotRead( pcontextMeterCert->pDatastore,
                                     &pcontextMeterCert->contextDSTSlot,
                                     cbData,
                                     (DRM_BYTE *)f_pwszLAINFO,
                                     &cbIO ) );
            ChkBOOL( cbIO == cbData, DRM_E_FILE_READ_ERROR );

            /* No need to skip over the LAINFO, we've passed it by reading it */
            fSkipData = FALSE;
        }
        else if ( cbData > *f_pcchLAINFO * SIZEOF( DRM_WCHAR ) )
        {
            drCached = DRM_E_BUFFERTOOSMALL;
        }

        *f_pcchLAINFO = cbData / SIZEOF( DRM_WCHAR );
    }

    if( fSkipData )
    {
        /* Skip over the LAINFO */
        ChkDR( DRM_DST_SlotSeek( pcontextMeterCert->pDatastore,
                                 &pcontextMeterCert->contextDSTSlot,
                                 (DRM_LONG)cbData,
                                 eDRM_DST_SEEKCUR,
                                 NULL ) );
    }


    /*
    ** Read the metering certificate
    */

    if( f_pcbMeterCert == NULL )
    {
        /* They don't care about the meter cert, so exit now */
        goto ErrorExit;
    }
    /* They want either just the size of, or the actual metercert */

    /* read the number of characters in the metercert */
    ChkDR( DRM_DST_SlotRead( pcontextMeterCert->pDatastore,
                             &pcontextMeterCert->contextDSTSlot,
                             SIZEOF( DRM_DWORD ),
                             (DRM_BYTE *)&cbData,
                             &cbIO ) );
    FIX_ENDIAN_DWORD(cbData);

    if( cbData <= *f_pcbMeterCert
      && f_pbMeterCert != NULL )
    {
        /* read the meter cert itself */
        ChkDR( DRM_DST_SlotRead( pcontextMeterCert->pDatastore,
                                 &pcontextMeterCert->contextDSTSlot,
                                 cbData,
                                 f_pbMeterCert,
                                 &cbIO ) );
        ChkBOOL( cbIO == cbData, DRM_E_FILE_READ_ERROR );
    }
    else if ( cbData > *f_pcbMeterCert )
    {
        drCached = DRM_E_BUFFERTOOSMALL;
    }

    *f_pcbMeterCert = cbData;

ErrorExit:

    if( DRM_SUCCEEDED( dr )
        && DRM_FAILED( drCached ) )
    {
        dr = drCached;
    }

    if( fSlotOpen )
    {
        /* Don't want to potentially overwrite a failure code */
        ( DRM_VOID )DRM_DST_CloseSlot( pcontextMeterCert->pDatastore,
                                       &pcontextMeterCert->contextDSTSlot );
    }

    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        dr = DRM_E_METERCERT_NOT_FOUND;
    }

    return dr;
} /* _GetMIDStoreData */

static DRM_RESULT _WriteStoreDataElement(
    __in                             const DRM_DST              *f_pDatastore,
    __in                                   DRM_DST_SLOT_CONTEXT *f_pcontextDSTSlot,
    __in                                   DRM_DWORD            *f_pcbSlot,
    __in                                   DRM_BOOL              f_fNewSlot,
    __in_bcount_opt( f_cbData )      const DRM_BYTE             *f_pbData,
    __in                                   DRM_DWORD             f_cbData,
    __in_bcount( f_cbScratchBuffer )       DRM_BYTE             *f_pbScratchBuffer,
    __in                                   DRM_DWORD             f_cbScratchBuffer,
    __in                                   DRM_BOOL              f_fInvalidate )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  cbOld    = 0;
    DRM_DWORD  cbData   = 0;
    DRM_DWORD  cbIO     = 0;

    ChkArg( f_pcontextDSTSlot   != NULL
         && f_pbScratchBuffer    != NULL );
    ChkArg( f_pDatastore != NULL );

    /* Read how many characters are in the slot for the old LAINFO */
    if( !f_fNewSlot )
    {
        /* If it's an existing slot, we need to read the size of the LAINFO currently stored */
        ChkDR( DRM_DST_SlotRead( f_pDatastore,
                                 f_pcontextDSTSlot,
                                 SIZEOF( DRM_DWORD ),
                                 (DRM_BYTE*)&cbOld,
                                 &cbIO ) );
        ChkBOOL( cbIO == SIZEOF(DRM_DWORD), DRM_E_FILE_READ_ERROR );
        FIX_ENDIAN_DWORD( cbOld );
    }
    else
    {
        cbOld = 0;
    }

    /* Write out the new size of the LAINFO if needed */
    if( f_pbData != NULL
        || f_fNewSlot
        || f_fInvalidate )
    {
        /* cbData is the size of the new LAINFO, if LAINFO is NULL then it'll
           be stored as blank (ie cbData = 0) */
        if( f_pbData != NULL )
        {
            cbData = f_cbData;
        }
        else
        {
            cbData = 0;
        }

        if( cbData != cbOld
          || f_fNewSlot )
        {
            /* The size doesn't match, or it's a new slot so we need to write out the new one */

            if( !f_fNewSlot )
            {
                /* If it's an existing slot, we need to back up since we already
                   read the LAINFO size and now are past it */
                ChkDR( DRM_DST_SlotSeek( f_pDatastore,
                                         f_pcontextDSTSlot,
                                         -((DRM_LONG)SIZEOF( DRM_DWORD )),
                                         eDRM_DST_SEEKCUR,
                                         NULL ) );
            }

            /* Write the new LAINFO size */
            FIX_ENDIAN_DWORD( cbData );
            ChkDR( DRM_DST_SlotWrite( f_pDatastore,
                                      f_pcontextDSTSlot,
                                      SIZEOF(DRM_DWORD),
                                      (DRM_BYTE*)&cbData,
                                      &cbIO ) );
            ChkBOOL( cbIO == SIZEOF(DRM_DWORD), DRM_E_FILE_WRITE_ERROR );
        }

    }

    /* Write out the new element */
    if( f_pbData == NULL && !f_fInvalidate )
    {
        /* They don't want to change the current element, so skip over it */
        ChkDR( DRM_DST_SlotSeek( f_pDatastore,
                                 f_pcontextDSTSlot,
                                 (DRM_LONG)cbOld,
                                 eDRM_DST_SEEKCUR,
                                 NULL ) );
    }
    else
    {
        if( cbOld != f_cbData
            && !f_fNewSlot )
        {
            /* The new size doesn't match the old, need to adjust the slot */
            DRM_DWORD dwOrigSeekPtr = 0;
            DRM_DWORD cbTemp = 0;

            /* Save the current seek pointer */
            ChkDR( DRM_DST_SlotSeek( f_pDatastore,
                                     f_pcontextDSTSlot,
                                     0,
                                     eDRM_DST_SEEKCUR,
                                     &dwOrigSeekPtr ) );

            /* Skip over the LAINFO */
            ChkDR( DRM_DST_SlotSeek( f_pDatastore,
                                     f_pcontextDSTSlot,
                                     (DRM_LONG)cbOld,
                                     eDRM_DST_SEEKCUR,
                                     NULL ) );

            /* Calculate how much data is in the slot past the current element */
            cbTemp = *f_pcbSlot - dwOrigSeekPtr - cbOld;

            if( cbTemp > f_cbScratchBuffer )
            {
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            if( cbTemp > 0 )
            {
                /* Keep this data (if there is any) */
                ChkDR( DRM_DST_SlotRead( f_pDatastore,
                                         f_pcontextDSTSlot,
                                         cbTemp,
                                         f_pbScratchBuffer,
                                         &cbIO ) );
            }

            *f_pcbSlot += ( f_cbData - cbOld);
            ChkDR( DRM_DST_SlotResize( f_pDatastore,
                                       f_pcontextDSTSlot,
                                       *f_pcbSlot ) );

            if( cbTemp > 0 )
            {
                /* Seek back to where the new element data goes */
                ChkDR( DRM_DST_SlotSeek( f_pDatastore,
                                         f_pcontextDSTSlot,
                                         (DRM_LONG)( *f_pcbSlot - cbTemp ),
                                         eDRM_DST_SEEKSET,
                                         NULL ) );

                /* Put back the data we saved */
                ChkDR( DRM_DST_SlotWrite( f_pDatastore,
                                          f_pcontextDSTSlot,
                                          cbTemp,
                                          f_pbScratchBuffer,
                                          &cbIO ) );
            }

            /* Seek back to where we were originally */
            ChkDR( DRM_DST_SlotSeek( f_pDatastore,
                                     f_pcontextDSTSlot,
                                     (DRM_LONG)dwOrigSeekPtr,
                                     eDRM_DST_SEEKSET,
                                     NULL ) );
        }

        if( f_cbData > 0 )
        {
            /* Write the new element data */
            ChkArg( f_pbData != NULL );
            ChkDR( DRM_DST_SlotWrite( f_pDatastore,
                                      f_pcontextDSTSlot,
                                      f_cbData,
                                      f_pbData,
                                      &cbIO ) );
        }
    }
ErrorExit:
    return dr;
}

DRM_NO_INLINE static DRM_RESULT _SetMIDStoreData(
    __in                                   DRM_METERCERT_CONTEXT  *f_pcontextMeterCert,
    __in                             const DRM_MID                *f_pmid,
    __in_opt                               DRM_DST_SLOT_HINT      *f_pslotHint,
    __in_opt                         const DRM_CONST_STRING       *f_pdstrLAINFO,
    __in_opt                         const DRM_BOOL               *f_pfIsV2,
    __in_bcount_opt( f_cbMeterCert ) const DRM_BYTE               *f_pbMeterCert,
    __in                                   DRM_DWORD               f_cbMeterCert,
    __in                                   DRM_BOOL                f_fCreateIfNotExisting,
    __in                                   DRM_BOOL                f_fInvalidate )
{
    DRM_METERCERT_CONTEXT_PRIVATE *pcontextMeterCert = (DRM_METERCERT_CONTEXT_PRIVATE *) f_pcontextMeterCert;
    DRM_DWORD                      cbSlot            = SIZEOF( DRM_DWORD );
    DRM_RESULT                     dr                = DRM_SUCCESS;
    DRM_BOOL                       fSlotOpen         = FALSE;
    DRM_BOOL                       fNewSlot          = FALSE;
    DRM_DWORD                      dwFlags           = 0;
    DRM_DWORD                      cbIO              = 0;

    ChkArg( f_pcontextMeterCert != NULL
         && f_pmid              != NULL );
    ChkArg( pcontextMeterCert->pDatastore != NULL );

    if( f_fCreateIfNotExisting )
    {
        /* Calculate the needed size of the slot */
        /* Three DWORDs store the flags, and character count for the LAINFO and meter cert */
        cbSlot = DRM_MIN_METERCERT_STORE_ENTRY_SIZE;

        /* Add the size of the LAINFO and meter cert if specified */
        if( f_pdstrLAINFO != NULL )
        {
            cbSlot += (f_pdstrLAINFO->cchString * SIZEOF( DRM_WCHAR ));
        }

        if( f_pbMeterCert != NULL )
        {
            cbSlot += f_cbMeterCert;
        }
    }

    dr = DRM_DST_OpenSlot( pcontextMeterCert->pDatastore,
                           &pcontextMeterCert->contextNameSpace,
                           (DRM_DWORD)( ( f_fCreateIfNotExisting ? eDRM_DST_OPEN_ALWAYS : eDRM_DST_OPEN_EXISTING )| eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT ),
                           f_pmid,
                           f_pmid,
                           f_pslotHint,
                          &cbSlot,
                          &pcontextMeterCert->contextDSTSlot);
    if( f_fCreateIfNotExisting && dr == DRM_S_FALSE )
    {
        fNewSlot = TRUE;
    }
    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        dr = DRM_E_METERCERT_NOT_FOUND;
    }
    ChkDR( dr );
    fSlotOpen = TRUE;

    if( cbSlot < DRM_MIN_METERCERT_STORE_ENTRY_SIZE )
    {
        /* We need at least two DWORDs for a valid entry */
        return DRM_E_DST_CORRUPTED;
    }

    /*
    ** Write out the flags
    */
    if( f_pfIsV2 != NULL )
    {
        dwFlags = (DRM_DWORD)( *f_pfIsV2 ? DRM_METERCERT_FLAG_ACQ_PROTO_IS_V2 : 0 );
        FIX_ENDIAN_DWORD( dwFlags );
        ChkDR( DRM_DST_SlotWrite( pcontextMeterCert->pDatastore,
                                  &pcontextMeterCert->contextDSTSlot,
                                  SIZEOF(DRM_DWORD),
                                  (DRM_BYTE*)&dwFlags,
                                  &cbIO ) );
        ChkBOOL( cbIO == SIZEOF(DRM_DWORD), DRM_E_FILE_WRITE_ERROR );
    }
    else
    {
        /* Skip it if they didn't want to change it */
        ChkDR( DRM_DST_SlotSeek( pcontextMeterCert->pDatastore,
                                 &pcontextMeterCert->contextDSTSlot,
                                 (DRM_LONG)SIZEOF( DRM_DWORD ),
                                 eDRM_DST_SEEKCUR,
                                 NULL ) );
    }

    /*
    ** Store the LAINFO
    */

    ChkDR( _WriteStoreDataElement( pcontextMeterCert->pDatastore,
                                  &pcontextMeterCert->contextDSTSlot,
                                  &cbSlot,
                                   fNewSlot,
                                   f_pdstrLAINFO != NULL ? PB_DSTR( f_pdstrLAINFO ) : NULL,
                                   f_pdstrLAINFO != NULL ? CB_DSTR( f_pdstrLAINFO ) : 0,
                                   pcontextMeterCert->pbBuffer,
                                   pcontextMeterCert->cbBuffer,
                                   FALSE ) );

    /*
    ** Store the metering certificate
    */

    ChkDR( _WriteStoreDataElement( pcontextMeterCert->pDatastore,
                                  &pcontextMeterCert->contextDSTSlot,
                                  &cbSlot,
                                   fNewSlot,
                                   f_pbMeterCert,
                                   f_cbMeterCert,
                                   pcontextMeterCert->pbBuffer,
                                   pcontextMeterCert->cbBuffer,
                                   f_fInvalidate ) );

#if !_DATASTORE_WRITE_THRU
    ChkDR ( DRM_DST_CommitNamespace( pcontextMeterCert->pDatastore,
                                     &pcontextMeterCert->contextNameSpace ) );
#endif

ErrorExit:
    if (fSlotOpen)
    {
        dr = DRM_DST_CloseSlot( pcontextMeterCert->pDatastore, &pcontextMeterCert->contextDSTSlot );
    }

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_MCS_DeleteMeterCert(
    __in       DRM_METERCERT_CONTEXT  *f_pcontextMeterCert,
    __in const DRM_MID                *f_pmid )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_METERCERT_CONTEXT_PRIVATE *pcontextMeterCert = (DRM_METERCERT_CONTEXT_PRIVATE *) f_pcontextMeterCert;

    ChkArg(f_pcontextMeterCert != NULL
        && f_pmid              != NULL);
    ChkArg( pcontextMeterCert->pDatastore != NULL );

    dr = DRM_DST_DeleteSlot( pcontextMeterCert->pDatastore,
                            &pcontextMeterCert->contextNameSpace,
                            f_pmid,
                            f_pmid,
                            NULL,
                            TRUE);

    if (dr == DRM_E_DST_SLOT_NOT_FOUND)
    {
        dr = DRM_E_METERCERT_NOT_FOUND;
    }
    else
    {
        ChkDR(dr);
    }

ErrorExit:
    return dr;
} /* DRM_MCS_DeleteMeterCert */

DRM_API DRM_RESULT DRM_CALL DRM_MCS_InvalidateMeterCert(
    __in       DRM_METERCERT_CONTEXT  *f_pcontextMeterCert,
    __in const DRM_MID                *f_pmid )
{
    DRM_RESULT dr            = DRM_SUCCESS;

    ChkArg(f_pcontextMeterCert != NULL
        && f_pmid              != NULL );

   ChkDR( _SetMIDStoreData( f_pcontextMeterCert, f_pmid, NULL, NULL, NULL, NULL, 0, FALSE, TRUE ) );

ErrorExit:
    return dr;
} /* DRM_MCS_InvalidateMeterCert */

DRM_API DRM_RESULT DRM_CALL DRM_MCS_OpenStore(
    __in                      DRM_DST               *f_pDatastore,
    __in_bcount( f_cbBuffer ) DRM_BYTE              *f_pbBuffer,
    __in                      DRM_DWORD              f_cbBuffer,
    __out                     DRM_METERCERT_CONTEXT *f_pcontextMeterCert )
{
    DRM_RESULT                     dr                =  DRM_SUCCESS;
    DRM_METERCERT_CONTEXT_PRIVATE *pcontextMeterCert = (DRM_METERCERT_CONTEXT_PRIVATE *) f_pcontextMeterCert;

    ChkArg (f_pDatastore        != NULL
         && f_pcontextMeterCert != NULL);

    DRMCASSERT (SIZEOF (DRM_METERCERT_CONTEXT_PRIVATE) <= SIZEOF (DRM_METERCERT_CONTEXT));

    ZEROMEM( f_pcontextMeterCert, SIZEOF( DRM_METERCERT_CONTEXT ) );

    /* open the namespace for the Meter Cert Store */
    ChkDR( DRM_DST_OpenNamespace( f_pDatastore,
                                  &g_namespaceMeterCertStore,
                                  (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                  16,
                                  DRM_METERCERT_STORE_SLOT_SIZE,
                                  &pcontextMeterCert->contextNameSpace ) );
    pcontextMeterCert->pDatastore = f_pDatastore;

    pcontextMeterCert->pbBuffer = f_pbBuffer;
    pcontextMeterCert->cbBuffer = f_cbBuffer;

    pcontextMeterCert->fInited  = TRUE;

ErrorExit:

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_CloseStore(
    __in  DRM_METERCERT_CONTEXT       *f_pcontextMeterCert)
{
    DRM_METERCERT_CONTEXT_PRIVATE *pcontextMeterCert = (DRM_METERCERT_CONTEXT_PRIVATE *) f_pcontextMeterCert;
    DRM_RESULT                     dr                = DRM_SUCCESS;

    ChkArg( f_pcontextMeterCert != NULL );

    if( pcontextMeterCert->fInited )
    {
        ChkDR( DRM_DST_CloseNamespace( pcontextMeterCert->pDatastore,
                                       &pcontextMeterCert->contextNameSpace ) );
    }

    ZEROMEM( f_pcontextMeterCert, SIZEOF( DRM_METERCERT_CONTEXT ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_InitEnum(
    __in  DRM_METERCERT_CONTEXT       *f_pcontextMeterCert,
    __out DRM_METERCERT_ENUM          *f_pcontextMeterCertEnum )
{
    DRM_METERCERT_CONTEXT_PRIVATE *pcontextMeterCert     = (DRM_METERCERT_CONTEXT_PRIVATE *) f_pcontextMeterCert;
    DRM_METERCERT_ENUM_PRIVATE    *pcontextMeterCertEnum = (DRM_METERCERT_ENUM_PRIVATE *) f_pcontextMeterCertEnum;
    DRM_RESULT dr =  DRM_SUCCESS;

    ChkArg( f_pcontextMeterCert         != NULL
         && f_pcontextMeterCertEnum     != NULL );
    ChkArg( pcontextMeterCert->fInited );

    DRMCASSERT (SIZEOF (DRM_METERCERT_CONTEXT_PRIVATE) <= SIZEOF (DRM_METERCERT_CONTEXT));

    ZEROMEM(f_pcontextMeterCertEnum, SIZEOF (DRM_METERCERT_ENUM));

    dr = DRM_DST_InitSlotEnum( pcontextMeterCert->pDatastore,
                               &pcontextMeterCert->contextNameSpace,
                               NULL,
                               (DRM_DST_LOCKMODE)(eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT),
                               &pcontextMeterCertEnum->contextDSTEnum);

    if (DRM_SUCCEEDED (dr))
    {
        pcontextMeterCertEnum->fAny = TRUE;
    }
    else if (dr == DRM_E_DST_SLOT_NOT_FOUND)
    {
        /* No MIDs found. */

        pcontextMeterCertEnum->fAny = FALSE;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR (dr);
    }

    pcontextMeterCertEnum->pcontextMeterCert = (DRM_METERCERT_CONTEXT_PRIVATE*)f_pcontextMeterCert;
    pcontextMeterCertEnum->fInited           = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_EnumNext(
    __in  DRM_METERCERT_ENUM          *f_pcontextMeterCertEnum,
    __out DRM_MID                     *f_pmid,
    __out DRM_DST_SLOT_HINT           *f_pslotHint )
{
    DRM_METERCERT_ENUM_PRIVATE *pcontextMeterCertEnum = (DRM_METERCERT_ENUM_PRIVATE *) f_pcontextMeterCertEnum;
    DRM_RESULT                  dr                    = DRM_SUCCESS;
    DRM_DWORD                   cbSlot                = SIZEOF (DRM_DWORD);
    const DRM_DST              *pDatastore            = NULL;

    ChkArg (f_pcontextMeterCertEnum != NULL
         && f_pmid                  != NULL);
    ChkArg (pcontextMeterCertEnum->fInited);

    pDatastore = pcontextMeterCertEnum->pcontextMeterCert->pDatastore;

    DRMCASSERT (SIZEOF (DRM_METERCERT_CONTEXT_PRIVATE)
            <= SIZEOF (DRM_METERCERT_CONTEXT));

    if  (pcontextMeterCertEnum->fAny == FALSE)
    {
        dr = DRM_E_NOMORE;
        goto ErrorExit;
    }

    dr = DRM_DST_SlotEnumNext( pDatastore,
                               &pcontextMeterCertEnum->contextDSTEnum,
                               &pcontextMeterCertEnum->contextDSTSlot,
                               f_pmid,
                               NULL,
                               &cbSlot );

    if (dr == DRM_E_DST_BLOCK_MISMATCH
    ||  dr == DRM_E_DST_SLOT_NOT_FOUND)
    {
        ChkDR (DRM_E_NOMORE);
    }
    ChkDR (dr);

    if( f_pslotHint != NULL )
    {
        DRM_DST_MakeSlotHint( pDatastore,
                              &pcontextMeterCertEnum->contextDSTSlot,
                              f_pslotHint );
    }

    ChkDR( DRM_DST_CloseSlot( pDatastore, &pcontextMeterCertEnum->contextDSTSlot ) );

ErrorExit:

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_GetMeterCertByMID(
    __in                                       DRM_METERCERT_CONTEXT   *f_pcontextMeterCert,
    __in                                 const DRM_MID                 *f_pmid,
    __out_ecount_opt(*f_pcchLAINFO)            DRM_WCHAR               *f_pwszLAINFO,
    __inout_opt                                DRM_DWORD               *f_pcchLAINFO,
    __out_opt                                  DRM_BOOL                *f_pfIsV2,
    __out_bcount_opt(*f_pcbMeterCert)          DRM_BYTE                *f_pbMeterCert,
    __inout_opt                                DRM_DWORD               *f_pcbMeterCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING,  PERF_FUNC_Drm_GetMeterCert_By_MID);

    ChkArg ( f_pcontextMeterCert != NULL
          && f_pmid              != NULL );

    ChkDR( _GetMIDStoreData( f_pcontextMeterCert, f_pmid, NULL, f_pwszLAINFO, f_pcchLAINFO, f_pfIsV2, f_pbMeterCert, f_pcbMeterCert ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_UpdateMeterCert(
    __in                                   DRM_METERCERT_CONTEXT      *f_pcontextMeterCert,
    __in_opt                         const DRM_CONST_STRING           *f_pdstrLAINFO,
    __in                                   DRM_BOOL                    f_fIsV2,
    __in_bcount_opt( f_cbMeterCert ) const DRM_BYTE                   *f_pbMeterCert,
    __in                                   DRM_DWORD                   f_cbMeterCert,
    __in                             const DRM_MID                    *f_pmid )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_METERING, PERF_FUNC_Drm_MeterCert_Update);

    ChkArg( f_pcontextMeterCert != NULL
         && f_pmid              != NULL);

    ChkDR( _SetMIDStoreData(f_pcontextMeterCert, f_pmid, NULL, f_pdstrLAINFO, &f_fIsV2, f_pbMeterCert,  f_cbMeterCert, TRUE, FALSE ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

