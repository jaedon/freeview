/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmembedding.h>
#include <drmrevocation.h>
#include <drmdomainapi.h>

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    _AddLicenseWithPriority
**
** Synopsis:    Adds a license to the license store with the priority.
**
** Arguments:
**
** [f_pLicStore]            -- License store to add the licens to.
** [f_pbLicenseBuffer]      -- Pointer to the license buffer containing the license.
** [f_cbLicenseBuffer]      -- Size of the license buffer.
** [f_cbLicense]            -- Size of the license in bytes.
** [f_pKID]                 -- Pointer to the KID for the license.
** [f_pLID]                 -- Pointer to the LID for the license.
** [f_dwPriority]           -- Specifies the license priority
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _AddLicenseWithPriority(
    __in                                DRM_LICSTORE_CONTEXT *f_pLicStore,
    __inout_bcount( f_cbLicenseBuffer ) DRM_BYTE             *f_pbLicenseBuffer,
    __in                                DRM_DWORD             f_cbLicenseBuffer,
    __in                                DRM_DWORD             f_cbLicense,
    __in                                DRM_KID              *f_pKID,
    __in                                DRM_LID              *f_pLID,
    __in                                DRM_DWORD             f_dwPriority )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( f_cbLicense + SIZEOF( DRM_DWORD ) <= f_cbLicenseBuffer, DRM_E_OUTOFMEMORY );

    DRM_BYT_MoveBytes( f_pbLicenseBuffer,
                       SIZEOF( DRM_DWORD ),
                       f_pbLicenseBuffer,
                       0,
                       f_cbLicense );

    ChkDR( DRM_LST_AddLicense( f_pLicStore,
                               f_cbLicense,
                               f_pbLicenseBuffer,
                               f_pKID,
                               f_pLID,
                               f_dwPriority ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _CopyLicenseBetweenStores
**
** Synopsis:    Copies the specified license from one store into the second store.
**
** Arguments:
**
** [f_pLicStoreSource]      -- Source store for the license.
** [f_pLicStoreDest]        -- Destination store for the license.
** [f_pKID]                 -- Pointer to the KID for the license.
** [f_pLID]                 -- Pointer to the LID for the license.
** [f_pbLicenseBuffer]      -- Pointer to the license buffer.
** [f_cbLicenseBuffer]      -- Size of the license buffer.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _CopyLicenseBetweenStores(
    __in                                DRM_LICSTORE_CONTEXT *f_pLicStoreSource,
    __in                                DRM_LICSTORE_CONTEXT *f_pLicStoreDest,
    __in                                DRM_KID              *f_pKID,
    __in                                DRM_LID              *f_pLID,
    __inout_bcount( f_cbLicenseBuffer ) DRM_BYTE             *f_pbLicenseBuffer,
    __in                                DRM_DWORD             f_cbLicenseBuffer )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbLicense = f_cbLicenseBuffer - SIZEOF( DRM_DWORD );

    ChkDR( DRM_LST_GetLicense( f_pLicStoreSource,
                               f_pKID,
                               f_pLID,
                               NULL,
                               f_pbLicenseBuffer,
                              &cbLicense ) );

    ChkDR( _AddLicenseWithPriority( f_pLicStoreDest,
                                    f_pbLicenseBuffer,
                                    f_cbLicenseBuffer,
                                    cbLicense,
                                    f_pKID,
                                    f_pLID,
                                    0 ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    _RetrieveAndUnpackLicense
**
** Synopsis:    Retrieves the specified license from the store and unpacks it.
**
** Arguments:
**
** [f_pLicStore]            -- License Store to retrieve the license from.
** [f_pKID]                 -- Pointer to the KID for the license.
** [f_pLID]                 -- Pointer to the LID for the license.
** [f_pSlotHint]            -- Pointer to a slot hint for the license (optional)
** [f_pbLicense]            -- Pointer to the license buffer.
** [f_cbLicenseBuffer]      -- Specifies the total length of the scratch buffer
** [f_pStack]               -- Pointer to the stack context
** [f_pcbLicense]           -- Returns the size of the license.
** [f_pXMRLicense]          -- Returns the unpacked XMR license
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _RetrieveAndUnpackLicense(
    __in                                DRM_LICSTORE_CONTEXT        *f_pLicStore,
    __in                                DRM_KID                     *f_pKID,
    __in                                DRM_LID                     *f_pLID,
    __in_opt                            DRM_DST_SLOT_HINT           *f_pSlotHint,
    __inout_bcount( f_cbLicenseBuffer ) DRM_BYTE                    *f_pbLicenseBuffer,
    __in                                DRM_DWORD                    f_cbLicenseBuffer,
    __inout                             DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __out                               DRM_DWORD                   *f_pcbLicense,
    __out                               DRM_XMR_LICENSE             *f_pXMRLicense )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pcbLicense != NULL );

    *f_pcbLicense = f_cbLicenseBuffer;
    ChkDR( DRM_LST_GetLicense( f_pLicStore,
                               f_pKID,
                               f_pLID,
                               f_pSlotHint,
                               f_pbLicenseBuffer,
                               f_pcbLicense ) );

    f_pStack->pbStack = f_pbLicenseBuffer + __CB_DECL( *f_pcbLicense + SIZEOF( DRM_DWORD ) );
    f_pStack->cbStack = f_cbLicenseBuffer - ( *f_pcbLicense + SIZEOF( DRM_DWORD ) );
    f_pStack->nStackTop = 0;

    ChkDR( DRM_XMR_UnpackLicense( f_pbLicenseBuffer,
                                 *f_pcbLicense,
                                  f_pStack,
                                  f_pXMRLicense ) );
ErrorExit:
    return dr;
}
/**********************************************************************
**
** Function:    _IsLicenseValidForELS
**
** Synopsis:    Checks that the License has a Root and/or domain cert in the HDS if needed.
**
** Arguments:
**
** [f_pEmbeddedStore]         -- Embedded License Store to receive licenses.
** [f_pLicStore]              -- License Store to scan for licenses to embed. Must be the HDS XMR Store.
** [f_pDomainStore]           -- Domain store context.
** [f_pXMRLicense]            -- The unpacked XMR license
** [f_pfValidForELS]          -- Pointer to a BOOL to state if License is Valid. This value is undefined on function failure.
**
** Returns:
**      DRM_SUCCESS           -- No error occured, the License may not be valid for Embedding.
**      DRM_E_INVALID_ARG     -- f_pXMRLicense or f_pfValidForELS was NULL or a called function had an invalid arg.
**      other DRM Error codes
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _IsLicenseValidForELS(
    __in                                DRM_LICSTORE_CONTEXT        *f_pEmbeddedStore,
    __in                                DRM_LICSTORE_CONTEXT        *f_pLicStore,
    __in                                DRM_DOMAINSTORE_CONTEXT     *f_pDomainStore,
    __inout                             DRM_XMR_LICENSE             *f_pXMRLicense,
    __inout                             DRM_BOOL                    *f_pfValidForELS )
{
    DRM_RESULT dr           = DRM_SUCCESS;

    ChkArg( NULL != f_pXMRLicense   );
    ChkArg( NULL != f_pfValidForELS );
    *f_pfValidForELS = FALSE;

    if ( XMR_IS_EMBEDDING_BEHAVIOR_VALID( f_pXMRLicense )
       && ( f_pXMRLicense->containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue ==  XMR_EMBEDDING_BEHAVIOR_COPY
         || f_pXMRLicense->containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue ==  XMR_EMBEDDING_BEHAVIOR_MOVE ) )
    {
        *f_pfValidForELS = TRUE;

        /*
        ** If the license is domain-bound, verify that there is a domain cert for it
        ** in the domain store
        */
        if ( XMR_IS_DOMAIN_ID_VALID( f_pXMRLicense ) )
        {
            ChkDR( DRM_DOM_IsLicenseValidForELS( f_pDomainStore, f_pXMRLicense, f_pfValidForELS ) );
        }
        /*
        ** Otherwise, if it is a leaf license, verify that a root exists in the HDS
        */
        else if ( XMR_IS_UPLINK_KID_VALID( f_pXMRLicense ) )
        {
            DRM_LICSTOREENUM_CONTEXT oEnumHDS     = { 0 };
            DRM_GUID                 oScratchGuid = { 0 };
            DRM_BYT_CopyBytes( &oScratchGuid,
                                0,
                                f_pXMRLicense->containerOuter.containerKeys.UplinkKid.pbguidUplinkKID,
                                f_pXMRLicense->containerOuter.containerKeys.UplinkKid.iguidUplinkKID,
                                SIZEOF( DRM_GUID ) );

            ChkDR( DRM_LST_InitEnum( f_pLicStore,
                                    ( DRM_KID * )&oScratchGuid,
                                     FALSE,
                                    &oEnumHDS ) );
            *f_pfValidForELS = ( oEnumHDS.eMode != eDRM_LICENSE_STORE_MODE_NONE );
        }
    }

ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    _DoLicenseBumpOff
**
** Synopsis:    Performs the following 'bump-off' algorithm on the embedded license store:
**              If there are pending licenses in the list, do -
**                  - Delete licenses belonging to domains we don't have a cert for
**                  - Delete licenses with lower priority than those in the list
**
** Arguments:
**
** [f_pEmbeddedStore]         -- Embedded License Store to receive licenses.
** [f_pLicStore]              -- License Store to scan for licenses to embed. Must be the HDS XMR Store.
** [f_pDomainStore]           -- Domain store context.
** [f_pKID]                   -- Pointer to the KID for the license.
** [f_pLID]                   -- Pointer to the LID for the license.
** [f_pPendingAddList]        -- Array of LIDs pending add to the ELS.
** [f_pdwPendingAddIssueDate] -- Array of priority values corresponding to f_pPendingAddList.
** [f_pcPendingAddList]       -- Element count of f_pPendingAddList and f_pdwPendingAddPri.
** [f_pbLicense]              -- Pointer to the license buffer.
** [f_cbLicense]              -- Size of the license buffer.
** [f_pSlotHint]              -- Pointer to a slot hint for the license (optional)
** [f_cbScratchBuffer]        -- Specifies the total length of the scratch buffer
** [f_pStack]                 -- Pointer to the stack context
** [f_pXMRLicense]            -- Returns the unpacked XMR license
** [f_pfModifiedEST]          -- Pointer to a BOOL to state if the f_pEmbeddedStore was modified.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _DoLicenseBumpOff(
    __in                                      DRM_LICSTORE_CONTEXT        *f_pEmbeddedStore,
    __in                                      DRM_LICSTORE_CONTEXT        *f_pLicStore,
    __in                                      DRM_DOMAINSTORE_CONTEXT     *f_pDomainStore,
    __in                                      DRM_KID                     *f_pKID,
    __in                                      DRM_LID                     *f_pLID,
    __in_ecount( f_cPendingAddList )          DRM_LID                     *f_pPendingAddList,
    __in_ecount( f_cPendingAddList )    const DRM_DWORD                   *f_pdwPendingAddIssueDate,
    __in                                      DRM_DWORD                    f_cPendingAddList,
    __inout_bcount( f_cbScratchBuffer )       DRM_BYTE                    *f_pbScratchBuffer,
    __in                                      DRM_DWORD                    f_cbScratchBuffer,
    __in_opt                                  DRM_DST_SLOT_HINT           *f_pSlotHint,
    __inout                                   DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __inout                                   DRM_XMR_LICENSE             *f_pXMRLicense,
    __inout                                   DRM_BOOL                    *f_pfModifiedEST )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_WORD   i            = 0;
    DRM_BOOL   fKeepGoing   = FALSE;
    DRM_BOOL   fDeleted     = FALSE;
    DRM_DWORD  cbLicense    = 0;
    DRM_BOOL   fValidForELS = FALSE;

    DRM_LICSTOREENUM_CONTEXT oEnumELS = { 0 };

    ChkArg( f_pfModifiedEST != NULL );

    if ( f_cPendingAddList == 0 )
    {
        goto ErrorExit;
    }

    /*
    ** Start enumerating embedded licenses
    */
    ChkDR( DRM_LST_InitEnum( f_pEmbeddedStore,
                             f_pKID,
                             FALSE, /* enumerate in KID-filtered mode */
                            &oEnumELS ) );
    dr = DRM_LST_EnumNext( &oEnumELS,
                           f_pKID,
                           f_pLID,
                           f_pSlotHint,
                          &cbLicense );
    if ( dr != DRM_E_NOMORE )
    {
        ChkDR( dr );
        fKeepGoing = TRUE;
    }
    while ( i < f_cPendingAddList && fKeepGoing )
    {
        /*
        ** Try to add the next license
        */
        dr = _CopyLicenseBetweenStores( f_pLicStore,
                                        f_pEmbeddedStore,
                                        f_pKID,
                                       &f_pPendingAddList[i],
                                        f_pbScratchBuffer,
                                        f_cbScratchBuffer );

        /*
        ** If adding fails, enumerate ELS licenses until
        ** one from a bogus domain is found or with a bogus ROOT, and delete
        ** If nomore is reached, then fKeepGoing = false
        */
        if ( dr == DRM_E_DSTSTOREFULL )
        {
            fKeepGoing = TRUE;
            fDeleted   = FALSE;

            while ( !fDeleted && fKeepGoing )
            {
                ChkDR( _RetrieveAndUnpackLicense( f_pEmbeddedStore,
                                                  f_pKID,
                                                  f_pLID,
                                                  f_pSlotHint,
                                                  f_pbScratchBuffer,
                                                  f_cbScratchBuffer,
                                                  f_pStack,
                                                 &cbLicense,
                                                  f_pXMRLicense ) );

                /*
                ** Check to see if we should remove the license from the ELS
                */
                ChkDR( _IsLicenseValidForELS( f_pEmbeddedStore,
                                              f_pLicStore,
                                              f_pDomainStore,
                                              f_pXMRLicense,
                                             &fValidForELS ) );

                if( !fValidForELS )
                {
                    ChkDR( DRM_LST_EnumDelete( &oEnumELS ) );
                   *f_pfModifiedEST = TRUE;
                    fDeleted = TRUE;
                }
                dr = DRM_LST_EnumNext( &oEnumELS,
                                        f_pKID,
                                        f_pLID,
                                        f_pSlotHint,
                                       &cbLicense );

                /*
                ** Did not delete license, move to next license
                */
                if ( dr == DRM_E_NOMORE )
                {
                    dr = DRM_SUCCESS;
                    fKeepGoing = fDeleted;
                }
                else
                {
                    ChkDR( dr );
                }
            }
        }
        else
        {
            ChkDR( dr );
            *f_pfModifiedEST = TRUE;
            i++;
        }
    }
    for ( ; i < f_cPendingAddList; i++ )
    {
        DRM_DWORD dwIssueDateThreshhold = f_pdwPendingAddIssueDate[i];

        /*
        ** Remove the oldest license from the ELS
        */
        ChkDR( DRM_LST_InitEnum( f_pEmbeddedStore,
                                 f_pKID,
                                 FALSE,
                                &oEnumELS ) );

        fKeepGoing = TRUE;
        while ( fKeepGoing )
        {
            dr = DRM_LST_EnumNext( &oEnumELS,
                                    f_pKID,
                                    f_pLID,
                                    f_pSlotHint,
                                   &cbLicense );
            if ( dr == DRM_E_NOMORE )
            {
                dr = DRM_SUCCESS;
                fKeepGoing = FALSE;
            }
            else
            {
                ChkDR( dr );
                ChkDR( _RetrieveAndUnpackLicense( f_pEmbeddedStore,
                                                  f_pKID,
                                                  f_pLID,
                                                  f_pSlotHint,
                                                  f_pbScratchBuffer,
                                                  f_cbScratchBuffer,
                                                  f_pStack,
                                                 &cbLicense,
                                                  f_pXMRLicense ) );

                /*
                ** If the license is below the IssueDate threshhold, or it doesn't have one, move it into the HDS.
                */
                if ( !XMR_IS_ISSUEDATE_VALID( f_pXMRLicense )
                  || dwIssueDateThreshhold > f_pXMRLicense->containerOuter.containerGlobalPolicies.IssueDate.dwValue )
                {
                    /*
                    ** Copy to HDS, then delete from ELS
                    */
                    dr = _CopyLicenseBetweenStores( f_pEmbeddedStore,
                                                    f_pLicStore,
                                                    f_pKID,
                                                    f_pLID,
                                                    f_pbScratchBuffer,
                                                    f_cbScratchBuffer );
                    if ( dr != DRM_E_DUPLICATE_LICENSE )
                    {
                        ChkDR( dr );
                    }
                    ChkDR( DRM_LST_EnumDelete( &oEnumELS ) );
                    fKeepGoing = TRUE;
                    *f_pfModifiedEST = TRUE;

                    /*
                    ** Try to add the next license
                    */
                    dr = _CopyLicenseBetweenStores( f_pLicStore,
                                                    f_pEmbeddedStore,
                                                    f_pKID,
                                                   &f_pPendingAddList[i],
                                                    f_pbScratchBuffer,
                                                    f_cbScratchBuffer );
                    if ( dr == DRM_E_DSTSTOREFULL )
                    {
                        dr = DRM_SUCCESS;
                    }
                    else
                    {
                        ChkDR( dr );
                        fKeepGoing = FALSE;
                    }
                }
            }
        }
    }
ErrorExit:
    return dr;
}



/**********************************************************************
**
** Function:    DRM_EMB_UpdateEmbeddedStore
**
** Synopsis:    Searches for suitable licenses in the HDS and embeds
**              them, if found, into the Embedded Store.
**
** Arguments:
**
** [f_pEmbeddedStore]       -- Embedded License Store to receive licenses.
** [f_pLicStore]            -- License Store to scan for licenses to embed. Must be the HDS XMR Store.
** [f_poDomainStoreContext] -- Domain store context.
** [f_pLicEvalBackup]       -- Initialized LicEval Context to use to setup and reset the eval buffer.
** [f_rgbLicEvalBuffer]     -- Empty buffer large enough for a liceval context. This will be churned during embedding.
** [f_pbScratchBuffer]      -- Generic scratch buffer for use with the Stack Allocator context.
** [f_pcbScratchBuffer]     -- Size of the Scratch buffer. Receives the needed size if the buffer is too small.
** [f_poKID]                -- Pointer to the KID for which to embed licenses.
** [f_pfModifiedEST]        -- Pointer to a BOOL to state if the f_pEmbeddedStore was modified.
**
** Returns:                 DRM_SUCCESS          - No error occurred.
**                          DRM_E_INVALIDARG     - An argument is invalid or improperly initialized.
**                          DRM_E_BUFFERTOOSMALL - Returned if the scratch buffer is too small.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_EMB_UpdateEmbeddedStore(
    __in                                                       DRM_LICSTORE_CONTEXT    *f_pEmbeddedStore,
    __in                                                       DRM_LICSTORE_CONTEXT    *f_pLicStore,
    __in                                                       DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in const                                                 DRM_LICEVAL_CONTEXT     *f_pLicEvalBackup,
    __in_bcount( __CB_DECL( SIZEOF( DRM_LICEVAL_CONTEXT ) ) )  DRM_BYTE                 f_rgbLicEvalBuffer[ ],
    __in_bcount_opt( *f_pcbScratchBuffer )                     DRM_BYTE                *f_pbScratchBuffer,
    __inout                                                    DRM_DWORD               *f_pcbScratchBuffer,
    __in                                                       DRM_KID                 *f_poKID,
    __out                                                      DRM_BOOL                *f_pfModifiedEST
 )
{
#define MAX_EMBEDDED_BUMPOFF 20 /* Maximum # of licenses that can be 'bumped off' */
    DRM_RESULT                        dr                                            = DRM_SUCCESS;
    DRM_BOOL                          fDeleteLicense                                = FALSE;
    DRM_BOOL                          fSecStoreLicOpen                              = FALSE;
    DRM_BOOL                          fRepeatLID                                    = FALSE;
    DRM_KID                           oKID                                          = { 0 };
    DRM_LID                           oLID                                          = { 0 };
    DRM_LID                           oLIDLast                                      = { 0 };
    DRM_DST_SLOT_HINT                 oSlotHint                                     = { 0 };
    DRM_DWORD                         cbLicense                                     = 0;
    DRM_STACK_ALLOCATOR_CONTEXT       oStack                                        = { 0 };
    DRM_LICSTORE_CONTEXT             *rgpLicStores[2]                               = { NULL };
    DRM_DWORD                         cLicStores                                    = 0;
    DRM_LICSTORE_CONTEXT             *pLicStore                                     = NULL;
    DRM_LICEVAL_CONTEXT              *pLicEval                                      = NULL;
    DRM_XMR_LICENSE                   oLicenseXMR                                   = { 0 };
    DRM_LICSTOREENUM_MULTIPLE_CONTEXT oLicEnumMultiple                              = { 0 };
    DRM_LID                           oPendingAddList[MAX_EMBEDDED_BUMPOFF]         = { 0 };
    DRM_DWORD                         rgdwPendingAddIssueDate[MAX_EMBEDDED_BUMPOFF] = { 0 };
    DRM_WORD                          cPendingAddList                               = 0;
    DRM_DWORD                         cbLicenseTemp                                 = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMEMB, PERF_FUNC_DRM_EMB_UpdateEmbeddedStore );

    ChkArg( NULL != f_pEmbeddedStore );
    ChkArg( NULL != f_pLicStore );
    ChkArg( NULL != f_pLicEvalBackup );
    ChkArg( NULL != f_poDomainStoreContext );
    ChkArg( NULL != f_poKID );
    ChkArg( NULL != f_pcbScratchBuffer );

    if ( *f_pcbScratchBuffer < DRM_MAX_LICENSESIZE + SIZEOF( DRM_DWORD ) )
    {
        *f_pcbScratchBuffer = DRM_MAX_LICENSESIZE + SIZEOF( DRM_DWORD );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Use an internal temporary LicEval Buffer
    */
    pLicEval = ( DRM_LICEVAL_CONTEXT * ) f_rgbLicEvalBuffer;

    /*
    ** Ensure that we are not trying to embed licenses from the xml or temp stores
    */
    ChkArg( f_pLicStore->eType == eDRM_LICENSE_STORE_XMR );

    /*
    ** Set up enumeration over embedded store and HDS (XMR) store
    */
    rgpLicStores[cLicStores++] = f_pEmbeddedStore;
    rgpLicStores[cLicStores++] = f_pLicStore;

    /*
    ** Search for a suitable license for this KID
    */
    ChkDR( DRM_LST_InitEnumMultiple( rgpLicStores,
                                     cLicStores,
                                     f_poKID,
                                     TRUE,
                                    &oLicEnumMultiple ) );

    while ( TRUE )
    {
        DRM_BYTE rgbSSTPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };

        MEMCPY( &oLIDLast, &oLID, SIZEOF( DRM_LID ) );
        dr = DRM_LST_EnumNextMultiple( &oLicEnumMultiple,
                                       &oKID,
                                       &oLID,
                                       &pLicStore,
                                       &oSlotHint,
                                       &cbLicense );

        /*
        ** If no more licenses are found
        */
        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR( dr );

        /*
        ** Check for licenses with copies in both stores,
        ** as they require less processing
        */
        fRepeatLID = ( MEMCMP( &oLIDLast, &oLID, SIZEOF( DRM_LID ) ) == 0 );

        if ( cbLicense > *f_pcbScratchBuffer )
        {
            *f_pcbScratchBuffer += cbLicense;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        ChkDR( _RetrieveAndUnpackLicense( pLicStore,
                                         &oKID,
                                         &oLID,
                                         &oSlotHint,
                                          f_pbScratchBuffer,
                                         *f_pcbScratchBuffer,
                                         &oStack,
                                         &cbLicense,
                                         &oLicenseXMR ) );

        /*
        ** Delete any expired licenses found
        */

        if ( fSecStoreLicOpen )
        {
            fSecStoreLicOpen = FALSE;
            ZEROMEM(  pLicEval->pcontextSSTLicense,
                  SIZEOF(  DRM_SECSTORE_CONTEXT ) );
        }

        /*
        ** Reset the LicEval Object with the backup one.
        */
        MEMCPY( f_rgbLicEvalBuffer, f_pLicEvalBackup, SIZEOF( DRM_LICEVAL_CONTEXT ) );

        /*
        ** Clear and set some liceval information
        */
        if( DRM_REVOCATION_IsRevocationSupported() )
        {
            if( pLicEval->cbRevocationBuffer > 0 )
            {
                ZEROMEM( pLicEval->pbRevocationBuffer, pLicEval->cbRevocationBuffer );
            }
        }
        pLicEval->dwFlags = LICEVAL_VERIFY_IGNORE_VERIFICATION;
        pLicEval->fLicenseIsXMR = TRUE;
        pLicEval->plicenseXMR   = &oLicenseXMR;

        /*
        ** Talk to blackbox to get the secure store password for this license
        */
        ChkDR( DRM_SST_CreateLicenseStatePassword( &oLID,
                                                    rgbSSTPassword,
                                                    pLicEval->pcontextBBX ) );

        ChkDR( DRM_SST_OpenKeyTokens(  pLicEval->pcontextSSTLicense,
                                      &oLID,
                                       NULL,
                                       rgbSSTPassword,
                                       0,
                                       SECURE_STORE_LICENSE_DATA,
                                       f_pLicStore->pDatastore ) );
        fSecStoreLicOpen = TRUE;

        /*
        ** Check for deletion
        */
        ChkDR( DRM_LEVL_IsLicenseReadyForDeletion( pLicEval,
                                                  &fDeleteLicense ) );
        if ( fDeleteLicense )
        {
            ChkDR( DRM_LST_EnumDeleteMultiple( &oLicEnumMultiple ) );

            /*
            ** Set fModifiedEST to TRUE if a license was deleted from the EST.
            */
            if ( pLicStore == f_pEmbeddedStore )
            {
                *f_pfModifiedEST = TRUE;
            }
        }
        /*
        ** If it is an HDS license, verify that it is embeddable, then embed,
        ** if there is room in the EST.
        */
        else if ( pLicStore == f_pLicStore && !fRepeatLID )
        {
            DRM_BOOL fShouldEmbed = FALSE;
            /*
            ** Check whether this license is 'embeddable'
            */
            ChkDR( _IsLicenseValidForELS( f_pEmbeddedStore,
                                          f_pLicStore,
                                          f_poDomainStoreContext,
                                          &oLicenseXMR,
                                          &fShouldEmbed ) );


            if ( fShouldEmbed )
            {
                /*
                ** Make sure this is not a duplicate license
                */
                cbLicenseTemp = 0;
                dr = DRM_LST_GetLicense( f_pEmbeddedStore,
                                        &oKID,
                                        &oLID,
                                         NULL,
                                         NULL,
                                        &cbLicenseTemp );

                /*
                ** Found a license, so skip adding it again
                */
                if ( dr == DRM_E_BUFFERTOOSMALL )
                {
                    dr = DRM_SUCCESS;
                }
                else if ( dr != DRM_E_DSTSLOTNOTFOUND )
                {
                    ChkDR( dr );
                }
                else
                {
                    /*
                    ** Embed the license
                    */
                    dr = _AddLicenseWithPriority( f_pEmbeddedStore,
                                                  f_pbScratchBuffer,
                                                 *f_pcbScratchBuffer,
                                                  cbLicense,
                                                 &oKID,
                                                 &oLID,
                                                  0 );

                    /*
                    ** If the store is full, add this license to the 'to add' list
                    */
                    if ( dr == DRM_E_DSTSTOREFULL )
                    {
                        if ( cPendingAddList < MAX_EMBEDDED_BUMPOFF )
                        {
                            MEMCPY( &oPendingAddList[cPendingAddList], &oLID, SIZEOF( DRM_LID ) );
                            if( XMR_IS_ISSUEDATE_VALID( &oLicenseXMR ) )
                            {
                                rgdwPendingAddIssueDate[cPendingAddList] = oLicenseXMR.containerOuter.containerGlobalPolicies.IssueDate.dwValue;
                            }
                            else
                            {
                                rgdwPendingAddIssueDate[cPendingAddList] = 0;
                            }
                            cPendingAddList++;
                        }
                        dr = DRM_SUCCESS;
                    }
                    else
                    {
                        ChkDR( dr );
                        *f_pfModifiedEST = TRUE;
                    }
                }
            }
        }
        /*
        ** Do nothing for licenses which are already embedded and are not expired
        */
    }

    /*
    ** Licenses were pending add but the store was full
    */
    if ( cPendingAddList > 0 )
    {
        ChkDR( _DoLicenseBumpOff( f_pEmbeddedStore,
                                  f_pLicStore,
                                  f_poDomainStoreContext,
                                 &oKID,
                                 &oLID,
                                  oPendingAddList,
                                  rgdwPendingAddIssueDate,
                                  cPendingAddList,
                                  f_pbScratchBuffer,
                                 *f_pcbScratchBuffer,
                                 &oSlotHint,
                                 &oStack,
                                 &oLicenseXMR,
                                  f_pfModifiedEST ) );
    }
ErrorExit:
    if ( fSecStoreLicOpen )
    {
        ZEROMEM( pLicEval->pcontextSSTLicense, SIZEOF(  DRM_SECSTORE_CONTEXT ) );
    }
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
#undef MAX_EMBEDDED_BUMPOFF
}

EXIT_PK_NAMESPACE_CODE;
