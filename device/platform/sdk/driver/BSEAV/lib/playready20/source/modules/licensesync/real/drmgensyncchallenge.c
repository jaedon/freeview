/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmlicense.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmsyncstore.h>
#include <drmlicstore.h>
#include <drmxmlbuilder.h>
#include <drmcrt.h>
#include <drmlicensesyncconstants.h>

ENTER_PK_NAMESPACE_CODE;

/* criteria of license search */

const DRM_CONST_STRING* const g_apdstrActions [1] = { &g_dstrWMDRM_RIGHT_PLAYBACK };

/*********************************************************************
 *            PRIVATE FUNCTION _RequiredCharsForChallenge
 *
 *      function: determines required chars for challenge XML
 ********************************************************************/

static DRM_DWORD
_RequiredCharsForChallenge(DRM_DWORD cKIDsIn)
{
    DRM_DWORD cchRequired = g_cbXMLBuilderMinimum;
    DRM_DWORD cchKID      = 0;

    /* <DRMSYNCLIST type="challenge"> */

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagSyncList.cchString,
                                               0,
                                               g_dstrAttributeType.cchString,
                                               g_dstrChallenge.cchString);

    /* <RECORDS> */

    cchRequired += DRM_XMB_RequiredCharsForTag(g_dstrTagRecords.cchString,
                                               0,
                                               0,
                                               0);

    /* <KID value="base64"> */

    cchKID = DRM_XMB_RequiredCharsForTag(g_dstrTagSyncList.cchString,
                                         0,
                                         g_dstrLabelValue.cchString,
                                         CCH_BASE64_EQUIV (SIZEOF (DRM_KID)));

    cchRequired += cKIDsIn * cchKID;

    return cchRequired;
} /* _RequiredCharsForChallenge */

/*********************************************************************
 *                    PRIVATE FUNCTION SyncCountTest
 *
 * purpose: compare license count to threshold criterion
 *********************************************************************/

static DRM_RESULT SyncCountTest(DRM_DWORD                  cMaxCount,
                                DRM_LICENSE_STATE_DATA    *plsd,
                                DRM_BOOL                  *pfRequired)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(plsd       != NULL
        && pfRequired != NULL);

    ChkArg(plsd->dwNumCounts >= 1);

    *pfRequired = (cMaxCount         == DRM_SYNC_IGNORE_THRESHOLD_PARAMETER
               ||  plsd->dwCount [0]  < cMaxCount);

ErrorExit:
    return dr;
}

/*********************************************************************
 *                    PRIVATE FUNCTION SyncExpirationTest
 *
 * purpose: compare indicated license date to threshold criterion
 *    note: iDate is an index into the LICENSE_STATE_DATA's array
 *          of date structures; the number of valid array member
 *          (and the interpretation of them) depends on the license
 *          state value
 *********************************************************************/

static DRM_RESULT SyncExpirationTest(
                                     DRM_VOID               *pOEMContext,
                                     DRM_DWORD               oHoursFromNow,
                                     DRM_LICENSE_STATE_DATA *plsd,
                                     DRM_DWORD               iDate,
                                     DRM_BOOL               *pfSyncRequired,
                                     DRM_UINT64             *pui64ThresholdTime )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(plsd != NULL);

    ChkArg(iDate <        plsd->dwNumDates
        && iDate < NO_OF (plsd->datetime)
        && pui64ThresholdTime != NULL );

    if (oHoursFromNow == DRM_SYNC_IGNORE_THRESHOLD_PARAMETER)
    {
        *pfSyncRequired = TRUE;
    }
    else if (plsd->dwNumDates >= (iDate + 1))
    {
        DRM_UINT64 ui64Zero = DRM_UI64LITERAL( 0, 0 );
        DRM_UINT64  u64lsdTime;

        /*
        ** This calculation is cached between calls.  If the current value is
        ** 0 calculate the correct value, otherwise we assume it is valid
        */

        if (DRM_UI64Eql(*pui64ThresholdTime, ui64Zero))
        {
            DRMFILETIME filetime;

            Oem_Clock_GetSystemTimeAsFileTime(pOEMContext, &filetime);
            FILETIME_TO_UI64(filetime, *pui64ThresholdTime);

            if (oHoursFromNow > 0)
            {
                /* one FILETIME tick is 10^-7 seconds */

                *pui64ThresholdTime = DRM_UI64Add(*pui64ThresholdTime,
                                                   DRM_UI64Mul(DRM_UI64Mul(DRM_UI64(oHoursFromNow), DRM_UI64(3600)),
                                                               DRM_UI64(10000000)));
            }
        }

        FILETIME_TO_UI64(plsd->datetime [iDate], u64lsdTime );
        if (DRM_UI64Les(u64lsdTime, *pui64ThresholdTime))
        {
            *pfSyncRequired = TRUE;
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
 *                    PRIVATE FUNCTION DRM_SNC_IsSyncRequired
 *
 * purpose: compare license data to threshold criteria
 *********************************************************************/

DRM_BOOL DRM_SNC_IsSyncRequired(
    DRM_VOID                  *pOEMContext,
    DRM_DWORD                  cMaxCountIn,
    DRM_DWORD                  cMaxHoursIn,
    DRM_LICENSE_STATE_DATA    *plsd,
    DRM_UINT64                *pui64ThreshholdTime)
{
    DRM_BOOL   fSyncRequired = FALSE;
    DRM_RESULT dr            = DRM_SUCCESS;

    /*
    ** DRM_LICENSE_STATE_UNLIM: in theory we will never see this
    ** but it costs nothing to check for it.
    ** DRM_LICENSE_STATE_ENTRY_MARKED_FOR_DELETION: don't have to require sync'ing either,
    ** this entry will be deleted on the next call to DRM_SNC_GenerateSyncChallenge()
    */
    if ( plsd->dwCategory == DRM_LICENSE_STATE_UNLIM
        || plsd->dwCategory == DRM_LICENSE_STATE_ENTRY_MARKED_FOR_DELETION )
    {
        fSyncRequired =  FALSE;
    }
    else if ( cMaxCountIn == DRM_SYNC_IGNORE_THRESHOLD_PARAMETER
           && cMaxHoursIn == DRM_SYNC_IGNORE_THRESHOLD_PARAMETER )
    {
        fSyncRequired =  TRUE;
    }
    else
    {
        switch ( plsd->dwCategory )
        {
        /*
        ** Single-criterion cases
        */
        case DRM_LICENSE_STATE_COUNT:
            ChkDR(SyncCountTest (cMaxCountIn, plsd, &fSyncRequired));
            break;

        case DRM_LICENSE_STATE_UNTIL:
            ChkDR(SyncExpirationTest (pOEMContext, cMaxHoursIn, plsd, 0, &fSyncRequired, pui64ThreshholdTime));
            break;

        /*
        ** Don't sync yet if a license's FROM time hasn't started yet
        */
        case DRM_LICENSE_STATE_FROM_UNTIL:
            ChkDR(SyncExpirationTest (pOEMContext, cMaxHoursIn, plsd, 1, &fSyncRequired, pui64ThreshholdTime));
            break;

        case DRM_LICENSE_STATE_COUNT_FROM:
            ChkDR(SyncCountTest (cMaxCountIn, plsd, &fSyncRequired));
            break;

        case DRM_LICENSE_STATE_COUNT_FROM_UNTIL:

            ChkDR(SyncCountTest (cMaxCountIn, plsd, &fSyncRequired));

            if (fSyncRequired == FALSE)
            {
                ChkDR(SyncExpirationTest (pOEMContext, cMaxHoursIn, plsd, 1, &fSyncRequired, pui64ThreshholdTime));
            }
            break;

        /*
        ** Count until: sync on either criterion
        */
        case DRM_LICENSE_STATE_COUNT_UNTIL:
            ChkDR(SyncExpirationTest (pOEMContext, cMaxHoursIn, plsd, 0, &fSyncRequired, pui64ThreshholdTime));

            if (fSyncRequired == FALSE)
            {
                ChkDR(SyncCountTest (cMaxCountIn, plsd, &fSyncRequired));
            }
            break;

        /*
        ** On a one-use license, sync if that one use has happened
        */
        case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE:
            if (plsd->dwNumDates > 0)
            {
                ChkDR(SyncExpirationTest (pOEMContext, cMaxHoursIn, plsd, 0, &fSyncRequired, pui64ThreshholdTime));
            }
            else
            {
                fSyncRequired = FALSE;
            }
            break;

        case DRM_LICENSE_STATE_NORIGHT:
            fSyncRequired = TRUE;
            break;

        case DRM_LICENSE_STATE_FROM:
            fSyncRequired = FALSE;
            break;

        case DRM_LICENSE_STATE_NOT_FOUND:
            fSyncRequired = TRUE;
            break;
        }
    }

ErrorExit:
    return fSyncRequired;
}

DRM_RESULT DRM_SNC_GetLicenseDataForKID (
    IN  DRM_DST                    *f_pDatastore,
    IN  DRM_VIEW_RIGHTS_CONTEXT    *f_pcontextViewRightsIn,
    OUT DRM_LICENSE_STATE_DATA     *f_plsd)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pDatastore           != NULL
         && f_pcontextViewRightsIn != NULL
         && f_plsd                 != NULL );

    ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData( g_apdstrActions,
                                                   f_plsd,
                                                   NO_OF (g_apdstrActions),
                                                   f_pcontextViewRightsIn,
                                                   f_pDatastore,
                                                   TRUE,
                                                   DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES,
                                                   NULL,
                                                   NULL ) ); /* clean license store of expireds */
ErrorExit:
    return dr;
}

/*********************************************************************
 *                    PRIVATE FUNCTIONS DRM_SNC_IsStoreRefreshRequired
 *
 * purpose: if this license is count-based, we will need to refresh
 *          its data from the license store; counts can be decremented
 *          outside the control of the license store
 *********************************************************************/

DRM_BOOL DRM_SNC_IsStoreRefreshRequired (
    IN DRM_VOID               *f_pOEMContext,
    IN DRM_LICENSE_STATE_DATA *f_plsd)
{
    DRM_BOOL   fSyncRequired     = FALSE;
    DRM_UINT64 ui64ThresholdTime = DRM_UI64LITERAL(0, 0);

    if (f_plsd->dwCategory == DRM_LICENSE_STATE_COUNT
    ||  f_plsd->dwCategory == DRM_LICENSE_STATE_COUNT_FROM
    ||  f_plsd->dwCategory == DRM_LICENSE_STATE_COUNT_FROM_UNTIL
    ||  f_plsd->dwCategory == DRM_LICENSE_STATE_COUNT_UNTIL
    ||  f_plsd->dwCategory == DRM_LICENSE_STATE_FORCE_SYNC)
    {
        fSyncRequired = TRUE;
    }
    else
    {
        /* must refresh from the license store if it's expired */

        switch (f_plsd->dwCategory)
        {
        case DRM_LICENSE_STATE_UNTIL:
            SyncExpirationTest (f_pOEMContext, 0, f_plsd, 0, &fSyncRequired, &ui64ThresholdTime);
            break;

        /* don't sync yet if a license's FROM time hasn't started yet */

        case DRM_LICENSE_STATE_FROM_UNTIL:
            SyncExpirationTest (f_pOEMContext, 0, f_plsd, 1, &fSyncRequired, &ui64ThresholdTime);
            break;

        /* on a one-use license, sync if that one use has happened */

        case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE:
            if (f_plsd->dwNumDates > 0)
            {
                SyncExpirationTest (f_pOEMContext, 0, f_plsd, 0, &fSyncRequired, &ui64ThresholdTime);
            }
            else
            {
                fSyncRequired = FALSE;
            }
            break;
        }
    }

    return fSyncRequired;
}


static DRM_RESULT _WriteKIDToList(
    IN const DRM_KID     *f_pkid,
    IN OUT   _XMBContext *f_pcontextXMB )
{
    DRM_RESULT       dr      = DRM_SUCCESS;
    DRM_WCHAR        awchKID [CCH_BASE64_EQUIV (SIZEOF (DRM_KID))];
    DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;

    dstrKID.pwszString =        awchKID;
    dstrKID.cchString  = NO_OF (awchKID);

    ChkDR(DRM_B64_EncodeW( (DRM_BYTE*)f_pkid,
                   SIZEOF (DRM_KID),
            (DRM_WCHAR *) dstrKID.pwszString,
                         &dstrKID.cchString,
                          0));

    /* <KID value="base64"></KID> */

    ChkDR(DRM_XMB_WriteTag(f_pcontextXMB,
                          &g_dstrTagKID,
                           NULL,
                          &g_dstrLabelValue,
                          &dstrKID,
                           wttClosed));
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: DRM_SNC_DidLicenseExpire
**
** Synopsis: Checks if the licenses for a given KID have expired
**
** Arguments:
** [f_pcontextSync]       - initialized sync context
** [f_pcontextViewRights] - initialized view rights context
** [f_pDatastore]         - initialized DST context context
** [f_pKID]               - KID
** [f_pfExpired]          - returns whether it expired
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SNC_DidLicenseExpire(
    IN     DRM_SYNC_CONTEXT          *f_pcontextSync,
    IN     DRM_VIEW_RIGHTS_CONTEXT   *f_pcontextViewRights,
    IN     DRM_DST                   *f_pDatastore,
    IN      DRM_KID                  *f_pKID,
       OUT  DRM_BOOL                 *f_pfExpired )
{
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_LICENSE_STATE_DATA      lsd             = {0};
    DRM_DST_SLOT_HINT           slotHint        = {0};
    DRM_UINT64                  ui64Threshold   = DRM_UI64(0);

    ChkArg( f_pcontextSync != NULL
        &&  f_pDatastore   != NULL
        &&  f_pfExpired    != NULL );

    *f_pfExpired = TRUE;

    dr = DRM_SNC_GetStoreDataForKID( f_pcontextSync, f_pKID, &slotHint, &lsd, NULL, NULL);
    if( dr == DRM_E_SYNC_ENTRYNOTFOUND )
    {
        /* Looks like we never had a license for this before */
        *f_pfExpired = FALSE;
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }
    ChkDR( dr );

    /*
    ** If KID is on the synclist because the license wasn't found when enumerating then we
    ** we should not assume that it expired.
    */
    if( lsd.dwCategory == DRM_LICENSE_STATE_NOT_FOUND )
    {
        *f_pfExpired = FALSE;
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    /*
    ** The following is dependent on the HDS implementation of the data store.
    ** This will need to be updated in order to support another data store hosting the sync store.
    */
    if (DRM_SNC_IsStoreRefreshRequired(((_HdsContext*)f_pDatastore)->pOEMContext, &lsd))
    {
        if( f_pcontextViewRights == NULL )
        {
            /* The sync entry needs to be updated but a view rights context wasn't supplied.
            ** we will assume the license expired */
            *f_pfExpired = TRUE;
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
        else
        {
            ZEROMEM(&lsd, SIZEOF (lsd));

            MEMCPY(&f_pcontextViewRights->KID, f_pKID, SIZEOF(DRM_KID));
            if( DRM_SUCCEEDED( DRM_SNC_GetLicenseDataForKID( f_pDatastore,
                                                             f_pcontextViewRights,
                                                             &lsd ) ) )
            {
                ChkDR(DRM_SNC_SetStoreDataForKID(  f_pcontextSync,
                                                   f_pKID,
                                                  &slotHint,
                                                  &lsd,
                                                   NULL,
                                                   FALSE ));
            }
            else
            {
                /* The sync entry needs to be updated but the license couldn't be evaluated,
                ** we can assume it has expired and been deleted. */
                *f_pfExpired = TRUE;
                dr = DRM_SUCCESS;
                goto ErrorExit;
            }
        }
    }

    /*
    ** The following is dependent on the HDS implementation of the data store.
    ** This will need to be updated in order to support another data store hosting the sync store.
    */
    *f_pfExpired = DRM_SNC_IsSyncRequired( ((_HdsContext*)f_pDatastore)->pOEMContext, 0, 0, &lsd, &ui64Threshold );

ErrorExit:
    return dr;
}

/******************************************************************************
** Function :   DRM_SNC_GenerateSyncChallenge
**
** Synopsis :   two-step challenge generation
**
** Arguments :
**
** [f_pcontextViewRights] - initialized view rights context
** [f_pDatastore]        - initialized DST context context
** [f_pcontextStack]      - stack context
** [f_cMaxCount]          - maximum remaining playcount of licenses to include
** [f_cMaxHours]          - maximum remaining time before expiration to include
** [f_iKIDStart]          - index in the sync store to begin, 0-based
** [f_cKIDs]              - max number of KIDs to return
** [f_piKIDNext]          - points to DWORD to receive index of next KID in store
** [f_pcKIDs]             - points to DWORD to receive number of KIDs returned
** [f_pbChallenge]        - buffer for challenge
** [f_pcbChallenge]       - size of challenge buffer
******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SNC_GenerateSyncChallenge(
    IN     DRM_SYNC_CONTEXT            *f_pcontextSync,
    IN     DRM_VIEW_RIGHTS_CONTEXT     *f_pcontextViewRights,
    IN     DRM_DST                     *f_pDatastore,
    IN     DRM_STACK_ALLOCATOR_CONTEXT *f_pcontextStack,
    IN     DRM_DWORD                    f_cMaxCount,
    IN     DRM_DWORD                    f_cMaxHours,
    IN     DRM_DWORD                    f_iKIDStart,
    IN     DRM_DWORD                    f_cKIDs,
       OUT DRM_DWORD                   *f_piKIDNext,
       OUT DRM_DWORD                   *f_pcKIDs,
       OUT _XMBContext                 *f_pbChallenge,
    IN OUT DRM_DWORD                   *f_pcbChallenge)
{
    DRM_SYNC_ENUM_CONTEXT  *pcontextSyncEnum   = NULL;
    DRM_UINT64              ui64ThreshholdTime = DRM_UI64LITERAL( 0, 0 );
    _XMBContext *pbData        = NULL;
    DRM_DWORD    cbAdjustment  = 0;
    DRM_DWORD    iKIDCurrent   = 0;
    DRM_DWORD    cchToClose    = 0;
    DRM_DWORD    cchKID        = 0;
    DRM_DWORD    cKIDs         = 0;
    DRM_RESULT   dr            = DRM_SUCCESS;
    DRM_BOOL     fBuildList    = FALSE;
    DRM_BOOL     fContinue     = TRUE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMGENSYNCCHALLENGE, PERF_FUNC_DRM_SNC_GenerateSyncChallenge);

    ChkArg(f_pcontextViewRights != NULL
        && f_pDatastore         != NULL
        && f_pcbChallenge       != NULL
        && f_pcontextStack      != NULL
        && f_pcontextSync       != NULL);

    if (f_pcKIDs != NULL)
    {
        *f_pcKIDs = 0;
    }

    if (f_piKIDNext != NULL)
    {
        *f_piKIDNext = 0;
    }

#if SIXTEEN_BIT_ADDRESSING
    /* some processors require WORD-alignment */

    if (f_pbChallenge != NULL)
    {
        ChkDR(DRM_UTL_EnsureDataAlignment((DRM_BYTE *)f_pbChallenge,
                                                     *f_pcbChallenge,
                                        (DRM_BYTE **)&pbData,
                                                      f_pcbChallenge,
                                                      SIZEOF(DRM_DWORD),
                                                     &cbAdjustment));
    }

#else
    pbData = f_pbChallenge;
#endif

    /* allocate large variables */
    /* NOTE: stack has been set to exactly this much and no more */

    ChkDR(DRM_STK_Alloc(f_pcontextStack, SIZEOF (DRM_SYNC_ENUM_CONTEXT), (DRM_VOID **) &pcontextSyncEnum));

    /* update any needed data from the license store and determine how many
       entries will be returned in XML */

   /* enumerate the Key IDs in the sync store */

    ChkDR(DRM_SNC_InitEnum(f_pcontextSync,
                           pcontextSyncEnum));

    /* enumerate the Key IDs in the synclist store and check them against
       aggregated license data */

    if (*f_pcbChallenge  > 0
    &&   f_pbChallenge  != NULL)
    {
        /* <DRMSYNCLIST type="challenge"> */

        if (DRM_SUCCEEDED( DRM_XMB_CreateDocument(*f_pcbChallenge - cbAdjustment, pbData, &g_dstrTagSyncList) )
         && DRM_SUCCEEDED( DRM_XMB_AddAttribute  ( pbData, &g_dstrAttributeType, &g_dstrChallenge) )
         && DRM_SUCCEEDED( DRM_XMB_WriteTag      ( pbData, &g_dstrTagRecords, NULL, NULL, NULL, wttOpen) ) )
        {
            /* 3 = </> */

            cchToClose = g_dstrTagSyncList.cchString + 3
                       + g_dstrTagRecords .cchString + 3;

            cchKID     = DRM_XMB_RequiredCharsForTag(g_dstrTagKID.cchString,
                                                     0,
                                                     g_dstrLabelValue.cchString,
                                                     CCH_BASE64_EQUIV(SIZEOF(DRM_KID)));

            fBuildList = TRUE;
        }
    }

    while (fContinue)
    {
        DRM_LICENSE_STATE_DATA     lsd;
        DRM_DST_SLOT_HINT          slotHint = {0};

        if (pbData != NULL)
        {
            DRM_DWORD cbRemaining = 0;

            ChkDR(DRM_XMB_RemainingBuffer(pbData, &cbRemaining));

            /*
            ** if we don't have enough buffer left for closing the challenge and one more KID,
            ** break the loop
            */

            if (cbRemaining < ((cchToClose + cchKID) * SIZEOF(DRM_WCHAR)))
            {
                break;
            }
        }

        dr = DRM_SNC_EnumNext(pcontextSyncEnum,
                             &f_pcontextViewRights->KID,
                             &slotHint);

        if (dr == DRM_E_NOMORE)
        {
            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR(dr);
        }

        /*
        ** if we have not yet reached the first requested KID
        ** then proceed directly to the next enumeration.
        */

        if ( iKIDCurrent < f_iKIDStart )
        {
            iKIDCurrent++;
            continue;
        }

        if ( iKIDCurrent >= ( f_iKIDStart + f_cKIDs ) )
        {
            break;
        }

        /*
        ** Now iKIDCurrent >= f_iKIDStart.
        ** Get the current entry from the license store
        */
        ChkDR(DRM_SNC_GetStoreDataForKID(f_pcontextSync,
                                        &f_pcontextViewRights->KID,
                                        &slotHint,
                                        &lsd,
                                        NULL,
                                        NULL));
        /*
        ** If entry is marked for deletion then remove it from the sync list
        ** and move on to the next one
        */
        if ( lsd.dwCategory == DRM_LICENSE_STATE_ENTRY_MARKED_FOR_DELETION )
        {
            ChkDR( DRM_SNC_DeleteKID( f_pcontextSync,
                                      &f_pcontextViewRights->KID ) );
            continue;
        }

        iKIDCurrent++;

        /*
        ** If this KID has count-based licenses
        ** we must refresh its data from the license store;
        ** date-based data will be updated along with licenses
        */
        if ( DRM_SNC_IsStoreRefreshRequired( f_pcontextViewRights->pBBContext->pOEMContext, &lsd ) )
        {
            ZEROMEM(&lsd, SIZEOF (lsd));

            if ( DRM_FAILED( DRM_SNC_GetLicenseDataForKID( f_pDatastore,
                                                           f_pcontextViewRights,
                                                           &lsd ) ) )
            {
                /*
                ** This license is probably deleted and so must be synced
                */
                if ( fBuildList )
                {
                    if ( DRM_FAILED( _WriteKIDToList( &f_pcontextViewRights->KID, f_pbChallenge ) ) )
                    {
                        fContinue = FALSE;
                    }
                }
                cKIDs++;
                continue;
            }
            else
            {
                ChkDR( DRM_SNC_SetStoreDataForKID( f_pcontextSync,
                                                   &f_pcontextViewRights->KID,
                                                   &slotHint,
                                                   &lsd,
                                                   NULL,
                                                   FALSE ) );
            }
        }

        /* determine if this requires a sync */

        if ( DRM_SNC_IsSyncRequired( f_pcontextViewRights->pBBContext->pOEMContext,
                                     f_cMaxCount,
                                     f_cMaxHours,
                                     &lsd,
                                     &ui64ThreshholdTime ) )
        {
            cKIDs++;

            if ( fBuildList )
            {
                if ( DRM_FAILED( _WriteKIDToList( &f_pcontextViewRights->KID, f_pbChallenge ) ) )
                {
                    fContinue  = FALSE;
                    fBuildList = FALSE;
                }
            }
        }

    } /* end while more KIDs */

    if ( fBuildList )
    {
        DRM_STRING strXML = EMPTY_DRM_STRING;

        if ( cKIDs == 0
        ||   DRM_FAILED(DRM_XMB_CloseCurrNode( pbData,  NULL ) )    /* </RECORDS> */
        ||   DRM_FAILED(DRM_XMB_CloseDocument( pbData, &strXML ) ) ) /* </DRMSYNCLIST> */
        {
            fBuildList = FALSE;
        }
        else
        {
            strXML.pwszString [strXML.cchString] = g_wchNull;

            /* save the results in output buffers */

            *f_pcbChallenge = CB_DSTR( &strXML );

            MEMMOVE( f_pbChallenge, strXML.pwszString, *f_pcbChallenge + SIZEOF ( DRM_WCHAR ) );
        }
    }

    if (f_pcKIDs != NULL)
    {
        *f_pcKIDs = cKIDs;
    }

    if (f_piKIDNext != NULL)
    {
        *f_piKIDNext = iKIDCurrent;
    }

    if (cKIDs == 0)
    {
        ChkDR(DRM_E_NOMORE);
    }
    else if ( !fBuildList )
    {
        /* We reached this point because the output buffers are NULL or
           they weren't big enough */
        *f_pcbChallenge = _RequiredCharsForChallenge(cKIDs) * SIZEOF (DRM_WCHAR)
                        + cbAdjustment;

        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

ErrorExit:

    SAFE_STK_FREE(f_pcontextStack, pcontextSyncEnum);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
} /* DRM_SNC_GenerateSyncChallenge */

EXIT_PK_NAMESPACE_CODE;

