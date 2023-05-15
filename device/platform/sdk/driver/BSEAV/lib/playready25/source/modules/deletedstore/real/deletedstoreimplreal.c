/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DELETEDSTOREIMPLREAL_C
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmdeletedstore.h>
#include <drmdatastore.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

typedef struct _tagRecentExpiredContext
{
    DRM_DST_SLOT_CONTEXT      contextDSTSlot;
    DRM_DST_NAMESPACE_CONTEXT contextNameSpace;
    DRM_DST                  *pDatastore;
    DRM_BOOL                  fInited;
} DRM_RECENTEXPIRED_CONTEXT_PRIVATE;

typedef struct _tagRecentExpiredData
{
    DRMFILETIME datetime;
} DRM_RECENTEXPIRED_DATA;

/* opaque versions of contexts */

typedef struct __tagDRM_RECENTEXPIRED_CONTEXT
{
    DRM_BYTE rgbOpaqueData [__CB_DECL(SIZEOF(DRM_RECENTEXPIRED_CONTEXT_PRIVATE))];
} DRM_RECENTEXPIRED_CONTEXT;

/* low-level access to the recentexpired store */

static const DRM_DST_NAMESPACE g_namespaceRecentExpired =
{
    'r', 'e', 'c', 'e', 'n', 't', 'e', 'x',
    'p', 'i', 'r', 'e', 'd', '\0', '\0', '\0'
};

/**********************************************************************
** Function: _DRM_RCX_OpenStore
**
** Synopsis: opens the recentexpired store
**
** Parameters:
**
** [f_pDataStoreIn]         -- initialized DRM_DST
** [f_pcontextRCX]          -- recentexpired context data
 **********************************************************************/

static DRM_RESULT DRM_CALL _DRM_RCX_OpenStore(
    __in  DRM_DST                         *f_pDataStoreIn,
    __out DRM_RECENTEXPIRED_CONTEXT       *f_pcontextRCX )
{
    DRM_RESULT                dr           =  DRM_SUCCESS;
    DRM_RECENTEXPIRED_CONTEXT_PRIVATE *pcontextRCX = (DRM_RECENTEXPIRED_CONTEXT_PRIVATE *) f_pcontextRCX;

    ChkArg( f_pDataStoreIn != NULL
         && f_pcontextRCX != NULL );

    DRMCASSERT(SIZEOF(DRM_RECENTEXPIRED_CONTEXT_PRIVATE) <= SIZEOF(DRM_RECENTEXPIRED_CONTEXT));

    /* open the namespace for the recentexpired Store */
    ChkDR( DRM_DST_OpenNamespace( f_pDataStoreIn,
                                  &g_namespaceRecentExpired,
                                  (DRM_DST_OPENMODE)(eDRM_DST_CREATE_NEW | eDRM_DST_LOCKWAIT),
                                  16,
                                  SIZEOF(DRM_RECENTEXPIRED_DATA),
                                  &pcontextRCX->contextNameSpace ) );
    pcontextRCX->fInited    = TRUE;
    pcontextRCX->pDatastore = f_pDataStoreIn;

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: _DRM_RCX_CloseStore
**
** Synopsis: closes the recentexpired store
**
** Parameters:
**
** [f_pcontextRCX]          -- initialized recentexpired context data
 **********************************************************************/

static DRM_RESULT DRM_CALL _DRM_RCX_CloseStore(
    __in  DRM_RECENTEXPIRED_CONTEXT       *f_pcontextRCX )
{
    DRM_RECENTEXPIRED_CONTEXT_PRIVATE *pcontextRCX = (DRM_RECENTEXPIRED_CONTEXT_PRIVATE *) f_pcontextRCX;
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( f_pcontextRCX != NULL
         && pcontextRCX->fInited );

    if(pcontextRCX->fInited)
    {
        ChkDR(DRM_DST_CloseNamespace( pcontextRCX->pDatastore, &pcontextRCX->contextNameSpace));
    }

    ZEROMEM(f_pcontextRCX, SIZEOF(DRM_RECENTEXPIRED_CONTEXT));

ErrorExit:
    return dr;
}

/**********************************************************************
** Function: _DRM_RCX_GetKIDStoreData
**
** Synopsis: gets per-kid recentexpired data from the recentexpired store
**
** Parameters:
**
** [f_pcontextRCX]          -- initialized recentexpired context data
** [f_pkid]                 -- pointer to KID for which to get data
** [f_pdata]                -- per-kid recentexpired data
 **********************************************************************/

static DRM_RESULT DRM_CALL _DRM_RCX_GetKIDStoreData(
    __in        DRM_RECENTEXPIRED_CONTEXT      *f_pcontextRCX,
    __in  const DRM_KID                        *f_pkid,
    __out       DRM_RECENTEXPIRED_DATA         *f_pdata)
{
    DRM_RECENTEXPIRED_CONTEXT_PRIVATE *pcontextRCX = (DRM_RECENTEXPIRED_CONTEXT_PRIVATE*) f_pcontextRCX;
    DRM_DWORD   cbSlot   = SIZEOF(DRM_RECENTEXPIRED_DATA);
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BOOL   fSlotOpen = FALSE;
    DRM_UINT64 ui64;

    ChkArg( f_pcontextRCX != NULL
         && f_pkid        != NULL
         && f_pdata       != NULL
         && pcontextRCX->fInited );

    ChkDR( DRM_DST_OpenSlot( pcontextRCX->pDatastore,
                             &pcontextRCX->contextNameSpace,
                             eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKWAIT,
                             f_pkid,
                             f_pkid,
                             NULL,
                             &cbSlot,
                             &pcontextRCX->contextDSTSlot ) );

    fSlotOpen = TRUE;

    ChkDR( DRM_DST_SlotRead( pcontextRCX->pDatastore,
                             &pcontextRCX->contextDSTSlot,
                             SIZEOF(DRM_RECENTEXPIRED_DATA),
                             (DRM_BYTE*) f_pdata,
                             &cbSlot ) );

    FILETIME_TO_UI64(f_pdata->datetime, ui64 );
    FIX_ENDIAN_QWORD(ui64);
    UI64_TO_FILETIME( ui64, f_pdata->datetime );

ErrorExit:
    if(fSlotOpen)
    {
        dr = DRM_DST_CloseSlot( pcontextRCX->pDatastore, &pcontextRCX->contextDSTSlot );
    }

    return dr;
} /* _DRM_RCX_GetKIDStoreData */

/**********************************************************************
** Function: _DRM_RCX_SetKIDStoreData
**
** Synopsis: sets per-kid recentexpired data in the recentexpired store
**
** Parameters:
**
** [f_pcontextRCX]          -- initialized recentexpired context data
** [f_pkid]                 -- pointer to KID for which to get data
** [f_pdata]                -- per-kid recentexpired data
** [f_fCreateIfNotExisting] -- whether to create the data if not exists
 **********************************************************************/

static DRM_RESULT DRM_CALL _DRM_RCX_SetKIDStoreData(
    __in       DRM_RECENTEXPIRED_CONTEXT      *f_pcontextRCX,
    __in const DRM_KID                        *f_pkid,
    __in const DRM_RECENTEXPIRED_DATA         *f_pdata,
    __in       DRM_BOOL                        f_fCreateIfNotExisting )
{
    DRM_RECENTEXPIRED_CONTEXT_PRIVATE *pcontextRCX = (DRM_RECENTEXPIRED_CONTEXT_PRIVATE*) f_pcontextRCX;
    DRM_RECENTEXPIRED_DATA    lsd = { 0 };
    DRM_DWORD  cbSlot    = SIZEOF(DRM_RECENTEXPIRED_DATA);
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_BOOL   fSlotOpen = FALSE;
    DRM_UINT64 ui64;

    ChkArg( f_pcontextRCX != NULL
         && f_pkid        != NULL
         && f_pdata       != NULL
         && pcontextRCX->fInited );

    MEMCPY( &lsd, f_pdata, SIZEOF( lsd ) );

    FILETIME_TO_UI64(lsd.datetime, ui64 );
    FIX_ENDIAN_QWORD(ui64);
    UI64_TO_FILETIME( ui64, lsd.datetime );

    /* check to see if the entry already exists */
    dr = DRM_DST_OpenSlot( pcontextRCX->pDatastore,
                           &pcontextRCX->contextNameSpace,
                           eDRM_DST_OPEN_EXISTING | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                           f_pkid,
                           f_pkid,
                           NULL,
                           &cbSlot,
                           &pcontextRCX->contextDSTSlot );

    /* doesn't exist; create it */
    if( dr == DRM_E_DST_SLOT_NOT_FOUND
     && f_fCreateIfNotExisting )
    {
        ChkDR( DRM_DST_OpenSlot( pcontextRCX->pDatastore,
                                 &pcontextRCX->contextNameSpace,
                                 eDRM_DST_CREATE_NEW | eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT,
                                 f_pkid,
                                 f_pkid,
                                 NULL,
                                 &cbSlot,
                                 &pcontextRCX->contextDSTSlot ) );
    }
    else
    {
        ChkDR(dr);
    }

    fSlotOpen = TRUE;

    ChkDR( DRM_DST_SlotWrite( pcontextRCX->pDatastore,
                              &pcontextRCX->contextDSTSlot,
                              SIZEOF(DRM_RECENTEXPIRED_DATA),
                              (DRM_BYTE*) &lsd,
                              &cbSlot ) );

#if !_DATASTORE_WRITE_THRU
    ChkDR( DRM_DST_CommitNamespace( pcontextRCX->pDatastore, &pcontextRCX->contextNameSpace ) );
#endif

ErrorExit:
    if(fSlotOpen)
    {
        dr = DRM_DST_CloseSlot( pcontextRCX->pDatastore, &pcontextRCX->contextDSTSlot );
    }

    return dr;
} /* _DRM_RCX_SetKIDStoreData */

/*********************************************************************
** Function: _DRM_RCX_DeleteKID
**
** Synopsis: delete the indicated KID from the recentexpired store;
**           its absense is not an error
**
** Parameters:
**
** [f_pcontextRCX]          -- initialized RCX context
** [f_pkid]                 -- pointer to KID to be added/updated
**********************************************************************/

static DRM_RESULT DRM_CALL _DRM_RCX_DeleteKID(
    __in       DRM_RECENTEXPIRED_CONTEXT *f_pcontextRCX,
    __in const DRM_KID                   *f_pkid )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RECENTEXPIRED_CONTEXT_PRIVATE *pcontextRCX = (DRM_RECENTEXPIRED_CONTEXT_PRIVATE*) f_pcontextRCX;

    ChkArg( f_pcontextRCX != NULL
         && f_pkid        != NULL
         && pcontextRCX->fInited );

    dr = DRM_DST_DeleteSlot( pcontextRCX->pDatastore,
                             &pcontextRCX->contextNameSpace,
                             f_pkid,
                             f_pkid,
                             NULL,
                             TRUE );

    if(dr == DRM_E_DST_SLOT_NOT_FOUND)
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR(dr);
    }

ErrorExit:
    return dr;
} /* _DRM_RCX_DeleteKID */

/*
** Update the recent expired license data.
*/
DRM_API DRM_RESULT DRM_CALL DRM_DELETEDSTORE_UpdateRecentExpired(
    __in       DRM_DST            *pDatastore,
    __in const DRM_KID            *pKID )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_RECENTEXPIRED_CONTEXT   contextRCX;
    DRM_RECENTEXPIRED_DATA      data;

    ZEROMEM( &contextRCX, SIZEOF( contextRCX ) );
    ZEROMEM( &data, SIZEOF( data ) );

    ChkDR( _DRM_RCX_OpenStore(  pDatastore,
                              &contextRCX ) );

    /*
    ** Save the current datetime as the deletion datetime.
    ** Assume it is always later than the previous datetime.
    ** Don't worry about clock change scenario here.
    ** The only issue if the user changed the clock
    **  is that the "fast query" won't return "expired"
    **  for a recently expired license.
    ** The user will then see "no license"
    **  instead of "expired license".
    ** This is not important enough to do additional work.
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &data.datetime );

    ChkDR( _DRM_RCX_SetKIDStoreData( &contextRCX,
                                     pKID,
                                    &data,
                                     TRUE ) );

ErrorExit:
    (void) _DRM_RCX_CloseStore( &contextRCX );

    /*
    ** Updating the "recently expired" list should not be a fatal error.
    ** On DBG, we'll return failures to assist in debugging.
    ** On FRE, we'll simple return to S_OK to avoid failing the delete call.
    */
#if DBG
    return dr;
#else  /* DBG */
    return DRM_SUCCESS;
#endif /* DBG */
}

/*
** Check whether there's a recent expired license.
** If so, set *f_pfRecentExpired to TRUE.
** Also, clean up the store for this KID if the entry is old enough.
*/
DRM_API DRM_RESULT DRM_CALL DRM_DELETEDSTORE_CheckRecentExpired(
    __in                  DRM_VOID                *f_pcontextOEM,
    __in                  DRM_DST                 *f_pDatastore,
    __in            const DRM_KID                 *f_pKID,               /* kid to query */
    __out_ecount(1)       DRM_BOOL                *f_pfRecentExpired )   /* query result */
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_RECENTEXPIRED_CONTEXT   contextRCX;
    DRM_RECENTEXPIRED_DATA      data;
    DRMFILETIME                 ftCurrentTime;
    DRM_UINT64                  u64currentTime;
    DRM_UINT64                  u64currentTimeMinus3Months;
    DRMFILETIME                 ftCurrentTimeMinus3Months;

    ChkArg( f_pDatastore      != NULL
         && f_pKID            != NULL
         && f_pfRecentExpired != NULL );

    /*
    ** Note: Even if the result is already success for all actions,
    ** we should still hit the store to perform cleanup on old entries.
    */

    ZEROMEM( &contextRCX, SIZEOF( contextRCX ) );

    ChkDR( _DRM_RCX_OpenStore( f_pDatastore,
                              &contextRCX ) );

    dr = _DRM_RCX_GetKIDStoreData( &contextRCX,
                                   f_pKID,
                                  &data );
    if( dr == DRM_E_DST_SLOT_NOT_FOUND )
    {
        /* No recent expired - nothing to do */
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }
    else
    {
        ChkDR( dr );
    }

    /*
    **  Note that the mere existance of the entry is enough
    **  to add expired error to return value
    */

    *f_pfRecentExpired = TRUE;

    /* Now we're going to check if the entry is too old and should be deleted */

    Oem_Clock_GetSystemTimeAsFileTime( f_pcontextOEM, &ftCurrentTime );

    FILETIME_TO_UI64( ftCurrentTime, u64currentTime );
    /*
    ** Offset by 84 days (12 weeks)
    ** Filetime is measured in "100 nanosecond intervals"
    ** 84 days
    ** 24 hrs in a day
    ** 3600 secs in an hr
    ** 1000 ms in a sec
    ** 10000 "100 nanosecond intervals" in a ms
    */
    u64currentTimeMinus3Months = DRM_UI64Sub(
        u64currentTime,
        DRM_UI64Mul( DRM_UI64Mul( DRM_UI64Mul( DRM_UI64(84),
                                               DRM_UI64(24) ),
                                  DRM_UI64Mul( DRM_UI64(3600),
                                               DRM_UI64(1000) ) ),
                                  DRM_UI64(10000) ) );

    UI64_TO_FILETIME( u64currentTimeMinus3Months, ftCurrentTimeMinus3Months );

    /*
    **  if ( date.datetime < the current time - 3 months)
    */
    if( DRM_UTL_DateLessThan( &data.datetime, &ftCurrentTimeMinus3Months ) )
    {
        /*
        **  then entry is too old [delete it]
        */

        ChkDR( _DRM_RCX_DeleteKID( &contextRCX, f_pKID ) );
    }

ErrorExit:
    (void) _DRM_RCX_CloseStore( &contextRCX );

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
