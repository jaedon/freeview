/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_CLEANSTOREREAL_C
#include <drmcleanstore.h>
#include <drmliceval.h>
#include <drmutilitieslite.h>
#include <drmdeletedstore.h>
#include <drmremovaldatestore.h>
#include <drmmanager.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_DWORD g_dwPercentageTimeForDefrag = 10;
#define MAX_NUM_LICENSES_TO_REMOVE   10

typedef struct _tagLicenseToRemove
{
    DRM_KID            oKID;
    DRM_LID            oLID;
    DRM_DST_SLOT_HINT  oSlotHint;
} DRM_LICENSE_TO_REMOVE;

typedef struct _tagCleanupCallbackInfo
{
    pfnStoreCleanupProgress   pfnCallback;
    const DRM_VOID           *pvCallerData;
    DRM_DWORD                 dwCallbackInterval;
    DRM_DWORD                 cLicenses;
    DRM_DWORD                 dwLicenseIndex;
    DRM_DWORD                 cLicBetweenCBInterval;
    DRM_DWORD                 dwDefragFactor;
} DRM_CLEANUP_CALLBACK_INFO;

#define _REPORT_STATUS                                                                                 \
{                                                                                                      \
    if (   f_pCleanupCallback != NULL                                                                  \
        && f_pCleanupCallback->pfnCallback != NULL )                                                   \
    {                                                                                                  \
        f_pCleanupCallback->dwLicenseIndex++;                                                          \
        if ( f_pCleanupCallback->dwLicenseIndex % f_pCleanupCallback->cLicBetweenCBInterval == 0 )     \
        {                                                                                              \
            ChkDR( f_pCleanupCallback->pfnCallback( f_pCleanupCallback->pvCallerData,                  \
                                                    f_pCleanupCallback->dwLicenseIndex * 100 / ( f_pCleanupCallback->cLicenses + f_pCleanupCallback->dwDefragFactor ), \
                                                    f_pCleanupCallback->cLicenses - f_pCleanupCallback->dwLicenseIndex) );                                             \
        }                                                                                              \
    }                                                                                                  \
}

static DRM_RESULT DRM_CALL _RemoveLicenses(
    __in_ecount( 1 )                          DRM_LICSTORE_CONTEXT      *f_pLicStoreContext,
    __in_ecount(MAX_NUM_LICENSES_TO_REMOVE)   DRM_LICENSE_TO_REMOVE     *f_pLicensesToRemove,
    __in                                      DRM_DWORD                  f_cLicensesToRemove,
    __inout_ecount_opt( 1 )                   DRM_CLEANUP_CALLBACK_INFO *f_pCleanupCallback )
{
    DRM_RESULT            dr             = DRM_SUCCESS;
    DRM_DWORD             dwIdx          = 0;
    DRM_DST_ENUM_CONTEXT  dstEnumContext = { 0 };

    ChkArg( f_pLicensesToRemove != NULL );
    ChkArg( f_cLicensesToRemove <= MAX_NUM_LICENSES_TO_REMOVE );

    for ( dwIdx = 0; dwIdx < f_cLicensesToRemove; dwIdx++ )
    {

        /*
        ** Delete the license.
        **
        ** It's not an error if the delete failed - it may
        ** have been previously deleted.  Even if this deletion
        ** failed, we want to move on to attempting to delete
        ** the next license so check the return value and continue.
        **
        ** Call DRM_DST_InitSlotEnum to reset the namespace context because
        ** DRM_DST_DeleteSlot() uses the context to determine the starting
        ** point to search for the license and we want to start the search
        ** anew each time and not be dependent upon which licenses, if any,
        ** were previously deleted.
        */
        ChkDRContinue( DRM_DST_InitSlotEnum( f_pLicStoreContext->pDatastore,
                                            &f_pLicStoreContext->DstNamespaceContext,
                                            &f_pLicensesToRemove[dwIdx].oKID,
                           (DRM_DST_LOCKMODE)(eDRM_DST_LOCKSHARED | eDRM_DST_LOCKWAIT),
                                            &dstEnumContext ) );

        ChkDRContinue( DRM_DST_DeleteSlot( f_pLicStoreContext->pDatastore,
                                          &f_pLicStoreContext->DstNamespaceContext,
                                          &f_pLicensesToRemove[dwIdx].oKID,
                                          &f_pLicensesToRemove[dwIdx].oLID,
                                          &f_pLicensesToRemove[dwIdx].oSlotHint,
                                           FALSE ) );

        _REPORT_STATUS;
    }

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _RemoveLicensesBasedOnRemovalDate (
    __in_ecount( 1 )            DRM_LICSTORE_CONTEXT        *f_pLicStoreContext,
    __in                        DRM_DWORD                    f_dwRemovalDate,
    __in_bcount(f_cbBuffer )    DRM_BYTE                    *f_pbBuffer,
    __in                        DRM_DWORD                    f_cbBuffer,
    __inout_ecount( 1 )         DRM_DST                     *f_pDatastore,
    __inout_ecount_opt( 1 )     DRM_CLEANUP_CALLBACK_INFO   *f_pCleanupCallback )
{
    DRM_RESULT                     dr                    = DRM_SUCCESS;
    DRM_BOOL                       fStoreOpen            = FALSE;
    DRM_REMOVAL_DATE_CONTEXT       removalDateContext    = { 0 };
    DRM_REMOVAL_DATE_ENUM          removalDateEnum       = { 0 };
    DRM_DWORD                      dwRemovalDate         = 0;
    DRM_KID                        KID                   = { 0 };
    DRM_DST_SLOT_HINT              dstSlotHint           = { 0 };
    DRM_DWORD                      cLicenses             = 0;
    DRM_DWORD                      cbData                = 0;
    DRM_DWORD                      dwIdx                 = 0;
    DRM_LICENSE_TO_REMOVE          rgLicensesToRemove[MAX_NUM_LICENSES_TO_REMOVE];
    const DRM_REMOVAL_DATE_ENTRY  *pLicenseToRemove      = NULL;
    DRM_DWORD                      cLicensesToRemove     = 0;

    ChkArg( f_pLicStoreContext != NULL );
    ChkArg( f_pDatastore != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_cbBuffer > 0 );
    ChkArg( f_pDatastore != NULL );

    ChkDR( DRM_RDS_OpenStore( f_pDatastore, &removalDateContext ) );
    fStoreOpen = TRUE;

    dr = DRM_RDS_InitEnum( &removalDateContext,
                             ( DRM_DST_LOCKMODE ) ( eDRM_DST_LOCKEXCLUSIVE | eDRM_DST_LOCKWAIT ),
                             &removalDateEnum );
    /*
    ** Remap DRM_E_NOMORE to DRM_SUCCESS
    */
    if( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    while ( TRUE )
    {
        /*
        ** Iterate through all of the removal date entries
        */
        dr = DRM_RDS_EnumNext(&removalDateEnum,
                                &dwRemovalDate,
                                &KID,
                                &dstSlotHint,
                                &cLicenses );
        if( dr == DRM_E_NOMORE )
        {
            /*
            ** We're done with the loop
            */
            dr = DRM_SUCCESS;
            break;
        }

        ChkDR( dr );

        /*
        ** Check if the removal date entry falls in the desired range.
        ** If it does then delete all licenses associated with the removal
        ** date
        */
        if ( dwRemovalDate <= f_dwRemovalDate )
        {
            /*
            ** Get the license data for the removal date entry
            */
            cbData = f_cbBuffer;
            ChkDRContinue( DRM_RDS_GetRemovalDateEntry( &removalDateContext,
                                                 dwRemovalDate,
                                                &KID,
                                                &dstSlotHint,
                                                &cbData,
                                                 f_pbBuffer,
                                                &cLicenses ) );

            /*
            ** The data returned from GetRemovalDateEntry is expected to be
            ** # of licenses * SIZEOF( DRM_REMOVAL_DATE_ENTRY ) large.
            ** If the buffer is not at least that size then there is a problem.
            */
            if ( cbData < cLicenses * SIZEOF( DRM_REMOVAL_DATE_ENTRY ) )
            {
                ChkDRContinue( DRM_E_BUFFERTOOSMALL );
            }

            /*
            ** Get the licenses that must be deleted from the removal date entry.
            ** For efficiency sake, queue up the licenses to be deleted.  When the
            ** queue is full then delete all of the licenses in the queue.
            */
            for (dwIdx = 0; dwIdx < cLicenses; dwIdx++)
            {
                pLicenseToRemove = (const DRM_REMOVAL_DATE_ENTRY*) &f_pbBuffer[ dwIdx * SIZEOF( DRM_REMOVAL_DATE_ENTRY ) ];

                if ( cLicensesToRemove == MAX_NUM_LICENSES_TO_REMOVE )
                {
                    /*
                    ** Ignore the error return from _RemoveLicenses.  Even if it returns an error
                    ** continue on trying to delete other licenses but call MapDR so a TRACE message
                    ** will be created.
                    */
                    dr = _RemoveLicenses( f_pLicStoreContext,
                                          rgLicensesToRemove,
                                          cLicensesToRemove,
                                          f_pCleanupCallback );
                    if ( DRM_FAILED( dr ) )
                    {
                        MapDR( dr, DRM_SUCCESS );
                    }

                    cLicensesToRemove = 0;
                }

                MEMCPY( rgLicensesToRemove[cLicensesToRemove].oKID.rgb, KID.rgb, SIZEOF( DRM_KID ) );
                MEMCPY( rgLicensesToRemove[cLicensesToRemove].oLID.rgb, pLicenseToRemove->lid.rgb, SIZEOF( DRM_LID ) );

                rgLicensesToRemove[cLicensesToRemove].oSlotHint = pLicenseToRemove->slotHint;
                cLicensesToRemove++;

            }

            /*
            ** Now that the licenses have been deleted (or queued to be deleted),
            ** delete the removal date entry.
            */
            ChkDRContinue( DRM_RDS_EnumDeleteCurrent( &removalDateEnum ) );

        }
    }

    /*
    ** Delete any remaining licenses in the list.
    */
    if ( cLicensesToRemove > 0 )
    {
        ChkDR( _RemoveLicenses( f_pLicStoreContext,
                                rgLicensesToRemove,
                                cLicensesToRemove,
                                f_pCleanupCallback ) );
    }

ErrorExit:

    if ( fStoreOpen )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = DRM_RDS_CloseStore( &removalDateContext );

        if ( DRM_FAILED( dr2 ) && DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


static DRM_RESULT DRM_CALL _RemoveUnusableLicenses(
     __in_ecount( 1 )            DRM_LICSTORE_CONTEXT        *f_pLicStoreContext,
     __in_ecount( 1 )            DRM_LICSTOREENUM_CONTEXT    *f_pLicStoreEnumContext,
     __in                        DRM_DWORD                    f_dwFlags,
     __in_ecount( 1 )            DRM_LICEVAL_CONTEXT         *f_pLicEvalContext,
     __in                        DRM_DWORD                    f_dwRemovalDate,
     __in_bcount(f_cbBuffer )    DRM_BYTE                    *f_pbBuffer,
     __in                        DRM_DWORD                    f_cbBuffer,
     __inout_ecount( 1 )         DRM_DST                     *f_pDatastore,
    __inout_ecount_opt( 1 )     DRM_CLEANUP_CALLBACK_INFO   *f_pCleanupCallback )
{
    DRM_RESULT        dr                    = DRM_SUCCESS;
    DRM_BOOL          fDelete               = FALSE;
    DRM_DWORD         cbLicense             = 0;
    DRM_KID           KID                   = { 0 };
    DRM_LID           LID                   = { 0 };
    DRM_DST_SLOT_HINT slotHint              = { 0 };

    ChkArg( f_pLicStoreContext != NULL );
    ChkArg( f_pLicStoreEnumContext != NULL );
    ChkArg( f_pLicEvalContext != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pDatastore != NULL );

    /*
    **  Query for all licenses in the store
    */
    ChkDR( DRM_LST_InitEnum( f_pLicStoreContext,
                             NULL,
                             FALSE,
                             f_pLicStoreEnumContext ) );

    /*
    ** Loop will continue until DRM_E_NOMORE, which is remapped at ErrorExit,
    ** or a real failure occurs
    */
    while( TRUE )
    {
        cbLicense = 0;

        ChkDR( DRM_LST_EnumNext( f_pLicStoreEnumContext,
                                &KID,
                                &LID,
                                &slotHint,
                                &cbLicense ) );


        /*
        **  Ignore licenses whose size is greater than size of buffer available
        */
        if ( cbLicense <= f_cbBuffer )
        {
            /*
            **  Use buffer provided by user
            */
            dr =  DRM_LST_GetLicense(  f_pLicStoreContext,
                                      &KID,
                                      &LID,
                                      &slotHint,
                                       f_pbBuffer,
                                      &cbLicense );
            /*
            **  Ignore failure and continue deleting other licenses
            */
            if ( DRM_SUCCEEDED ( dr ) )
            {
                DRM_STACK_ALLOCATOR_CONTEXT oStack = {0};
                DRM_BYTE rgbPassword[DRM_SHA1_DIGEST_LEN];

                if( f_pLicEvalContext->fLicenseIsXMR )
                {
                    DRM_BYTE *pbStack = NULL;
                    DRM_DWORD cbStack = 0;

                    ChkDR( DRM_UTL_EnsureDataAlignment( f_pbBuffer + cbLicense,
                                                        f_cbBuffer - cbLicense,
                                                       &pbStack,
                                                       &cbStack,
                                                        SIZEOF( DRM_DWORD ),
                                                        NULL ) );

                    ChkDR( DRM_STK_Init( &oStack, pbStack, cbStack ) );
                    ChkDR( DRM_STK_Alloc( &oStack,
                                           SIZEOF( DRM_XMR_LICENSE ),
                              (DRM_VOID**)&f_pLicEvalContext->plicenseXMR ) );

                    ZEROMEM( f_pLicEvalContext->plicenseXMR, SIZEOF( DRM_XMR_LICENSE ) );
                    ChkDR( DRM_XMR_UnpackLicense( f_pbBuffer,
                                                  cbLicense,
                                                 &oStack,
                                                  f_pLicEvalContext->plicenseXMR ) );
                }
                else
                {
                    /*
                    ** Add info about this license to the license evaluator obj
                    */
                    DSTR_FROM_PB( &f_pLicEvalContext->dstrContentLicense, f_pbBuffer, cbLicense );
                    f_pLicEvalContext->fUseCachedAttribs = FALSE;
                }

                if( f_pLicEvalContext->fLicenseIsXMR
                 && ( f_pLicEvalContext->plicenseXMR != NULL )
                 && XMR_IS_REMOVAL_DATE_VALID( f_pLicEvalContext->plicenseXMR ) )
                {
                    if (   ( f_dwFlags & DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES )
                        && ( f_pLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.RemovalDate.dwRemovalDate <= f_dwRemovalDate ) )
                    {
                        /*
                        ** The license's removal date has passed.  This license should have been cleaned
                        ** up by _RemoveLicensesBasedOnRemovalDate() but perhaps there was a problem with
                        ** the entry in the removal date store.  Regardless, delete the license now but
                        ** do not save the deletion date.
                        */
                        ( void )DRM_LST_EnumDeleteAndSaveDate( f_pLicStoreEnumContext, FALSE );
                    }
                }
                else
                {
                    /*
                    ** Talk to BB to get the secstore password for this license
                    */
                    ChkDR( DRM_SST_CreateLicenseStatePassword( &LID,
                                                                rgbPassword,
                                                                f_pLicEvalContext->pcontextBBX ) );

                    dr = DRM_SST_OpenKeyTokens( f_pLicEvalContext->pcontextSSTLicense,
                                               &LID,
                                                NULL,
                                                rgbPassword,
                                                0,
                                                SECURE_STORE_LICENSE_DATA,
                                                f_pDatastore );
                    /*
                    **  Note there is no need to close this store.
                    **  We don't care to report actions we just peek inside
                    */
                    if ( dr == DRM_E_FILENOTFOUND )
                    {
                        /*
                        ** Data corruption - delete this license
                        */
                        ( void )DRM_LST_EnumDelete( f_pLicStoreEnumContext );
                    }
                    else if ( DRM_SUCCEEDED( dr ) )
                    {
                        /*
                        ** Save the store context
                        */
                        f_pLicEvalContext->pDatastore = f_pDatastore;

                        MEMCPY( f_pLicEvalContext->KID.rgb, KID.rgb, SIZEOF( DRM_KID ) );
                        MEMCPY( f_pLicEvalContext->LID.rgb, LID.rgb, SIZEOF( DRM_LID ) );

                        fDelete = FALSE;
                        if ( DRM_SUCCEEDED( DRM_LEVL_IsLicenseReadyForDeletion( f_pLicEvalContext,
                                                                               &fDelete ) )
                          && fDelete )
                        {
                            ( void )DRM_LST_EnumDeleteAndSaveDate( f_pLicStoreEnumContext,
                                                                   DRM_DELETEDSTORE_IsDeletedStoreSupported() );
                        }
                    }
                }
            }
            _REPORT_STATUS;
         }
    }

ErrorExit:

    /*
    ** Remap DRM_E_NOMORE to DRM_SUCCESS
    */
    if( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_CLEANSTORE_RemoveUnusableLicenses(
    __in                                                DRM_BYTE                    *f_pContextLEVL,
    __in_bcount( SIZEOF( DRM_LICSTORE_CONTEXT ) )       DRM_BYTE                    *f_pContextLSTXML,
    __in_bcount_opt( SIZEOF( DRM_LICSTORE_CONTEXT ) )   DRM_BYTE                    *f_pContextLSTXMR,
    __in                                                DRM_LICSTOREENUM_CONTEXT    *f_pLicStoreEnumContext,
    __in                                                DRM_DWORD                    f_dwFlags,
    __in_bcount(f_cbBuffer )                            DRM_BYTE                    *f_pbBuffer,
    __in                                                DRM_DWORD                    f_cbBuffer,
    __in_opt                                      const DRM_VOID                    *f_pvCallerData,
    __in                                                DRM_DWORD                    f_dwCallbackInterval,
    __in_opt                                            pfnStoreCleanupProgress      f_pfnCallback,
    __inout_ecount( 1 )                                 DRM_DST                     *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                 dr                    = DRM_SUCCESS;
    DRM_DWORD                  cLicenses             = 0;
    DRM_DWORD                  dwTemp                = 0;
    DRMFILETIME                ft                    = { 0 };
    DRM_DWORD                  dwCurrentTime         = 0;
    DRM_LICEVAL_CONTEXT       *pContextLEVL          = ( DRM_LICEVAL_CONTEXT * )f_pContextLEVL;
    DRM_LICSTORE_CONTEXT      *pLicStoreContext      = NULL;
    DRM_CLEANUP_CALLBACK_INFO  cleanupProgress       = { 0 };

    ChkArg( f_pLicStoreEnumContext != NULL );
    ChkArg( f_pbBuffer             != NULL );
    ChkArg( f_pDatastore           != NULL );

    if ( f_dwFlags & DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES )
    {
        /*
        ** If licenses are being cleaned by removal date then the caller must
        ** specify an XMR license store.
        */
        ChkArg( f_pContextLSTXMR != NULL );
    }

    if ( f_dwFlags & DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES )
    {
        ChkArg( f_pContextLEVL   != NULL );
        ChkArg( f_pContextLSTXML != NULL );
    }

    /*
    ** Get the current sytem time so that it can be used to delete removal date
    ** entries and associated licenses.
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ft );
    FILE_TIME_TO_DWORD( ft, dwCurrentTime );

    if ( f_pfnCallback )
    {
        cleanupProgress.pfnCallback        = f_pfnCallback;
        cleanupProgress.pvCallerData       = f_pvCallerData;
        cleanupProgress.dwCallbackInterval = f_dwCallbackInterval;

        if ( f_dwFlags & DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES )
        {
            /*
            ** Query for all XML licenses
            */
            ChkDR( DRM_LST_InitEnum( ( DRM_LICSTORE_CONTEXT * )f_pContextLSTXML,
                                     NULL,
                                     FALSE,
                                     f_pLicStoreEnumContext ) );

            /*
            **  Count the number of licenses in XML store
            */
            ChkDR( DRM_LST_LicCount( f_pLicStoreEnumContext, &cLicenses ) );

            /*
            ** Query for all XMR licenses
            ** Note that until LEVL supports XMR licenses, the XMR store cannot be fully
            ** cleaned up
            */
            if ( f_pContextLSTXMR != NULL )
            {
                ChkDR( DRM_LST_InitEnum( ( DRM_LICSTORE_CONTEXT * )f_pContextLSTXMR,
                                         NULL,
                                         FALSE,
                                         f_pLicStoreEnumContext ) );

                /*
                **  Count the number of licenses in XMR store
                */
                ChkDR( DRM_LST_LicCount( f_pLicStoreEnumContext, &dwTemp ) );

                cLicenses += dwTemp;
            }
        }
        else if ( f_dwFlags & DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES )
        {
            /*
            ** if f_dwFlags & DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES is true
            ** then all the licenses in both the XML and XMR stores have been
            ** counted and counting the removal date licenses would result in
            ** the licenses being double counted - thus the "else if" instead of
            ** just an "if".
            */

            /*
            ** Query for the count of licenses to delete based on removal date.
            */
            pLicStoreContext = (DRM_LICSTORE_CONTEXT*) f_pContextLSTXMR;
            ChkDR( DRM_RDS_GetLicenseCount(  pLicStoreContext->pDatastore,
                                            &dwCurrentTime,
                                            &cLicenses ) );
        }


        /*
        **  Callback to confirm whether cleanup should be started
        **  Caller returns an error to indicate that cleanup should be aborted
        */
        ChkDR( f_pfnCallback( f_pvCallerData, 0, cLicenses ) );

        cleanupProgress.cLicenses             = cLicenses;
        cleanupProgress.dwLicenseIndex        = 0;
        cleanupProgress.dwDefragFactor        = cLicenses * g_dwPercentageTimeForDefrag  / 100;
        cleanupProgress.cLicBetweenCBInterval = f_dwCallbackInterval * ( cLicenses + cleanupProgress.dwDefragFactor ) / 100;

        /*
        **  For certain small sizes of the
        **  store, the defrag factor might become zero
        **  Make sure that this value is reset to 1
        */
        if ( cleanupProgress.dwDefragFactor == 0 )
        {
            cleanupProgress.dwDefragFactor = 1;
        }

        /*
        **  For certain values of callbackinterval and size of the
        **  store, this value might become zero
        **  Make sure that this value is reset to 1
        */
        if ( cleanupProgress.cLicBetweenCBInterval == 0 )
        {
            cleanupProgress.cLicBetweenCBInterval = 1;
        }

    }

    if( f_dwFlags & DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES )
    {
        /*
        ** If the caller requested that licenses be deleted based on removal date
        ** then remove licenses based on removal date.
        */
        dr = _RemoveLicensesBasedOnRemovalDate( (DRM_LICSTORE_CONTEXT*) f_pContextLSTXMR,
                                                 dwCurrentTime,
                                                 f_pbBuffer,
                                                 f_cbBuffer,
                                                 f_pDatastore,
                                                &cleanupProgress );
        if ( DRM_FAILED( dr ) )
        {
            /*
            ** Continue on to cleaning up other licenses even if this function returns an error.
            ** But call MapDR so that a trace message will be written in check builds.
            */
            MapDR( dr, DRM_SUCCESS );
        }
    }

    if (  f_dwFlags & DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES )
    {
        /*
        **  Remove all unusable XML licenses
        */
        pContextLEVL->fLicenseIsXMR = FALSE;
        ChkDR( _RemoveUnusableLicenses( (DRM_LICSTORE_CONTEXT*) f_pContextLSTXML,
                                        f_pLicStoreEnumContext,
                                        f_dwFlags,
                                        pContextLEVL,
                                        dwCurrentTime,
                                        f_pbBuffer,
                                        f_cbBuffer,
                                        f_pDatastore,
                                       &cleanupProgress ) );

        if( f_pContextLSTXMR != NULL )
        {
            /*
            ** Remove all unusabe XMR licenses
            */
            pContextLEVL->fLicenseIsXMR = TRUE;
            ChkDR( _RemoveUnusableLicenses( (DRM_LICSTORE_CONTEXT*) f_pContextLSTXMR,
                                            f_pLicStoreEnumContext,
                                            f_dwFlags,
                                            pContextLEVL,
                                            dwCurrentTime,
                                            f_pbBuffer,
                                            f_cbBuffer,
                                            f_pDatastore,
                                           &cleanupProgress ) );
        }
    }


    if ( f_pfnCallback != NULL && cleanupProgress.dwLicenseIndex < cLicenses )
    {
        /*
        ** If some of the licenses were skipped (ie the buffer was too small
        ** to hold a license or a removal date only clean was done and
        ** some licenses were not processed because the removal date is in
        ** the future) then call the progress function one more time to update
        ** the percentage completed to indicate that all licenses have been
        ** processed.  But, instead of reporting 100% complete, report (100 -
        ** g_dwPercentageTimeForDefrag) % complete to indicate that the clean
        ** operation is not yet complete because the license store must still
        ** be defragged.
        */
        ChkDR( f_pfnCallback( f_pvCallerData, 100 - g_dwPercentageTimeForDefrag, 0 ) );
    }

ErrorExit:
    /*
    ** Remap DRM_E_NOMORE to DRM_SUCCESS
    */
    if( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

