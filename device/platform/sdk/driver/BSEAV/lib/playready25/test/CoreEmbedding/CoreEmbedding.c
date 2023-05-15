/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmlicense.h>
#include <drmheaderparser.h>
#include <drmhdrbuilder.h>
#include <drmplayreadyobj.h>
#include <drmembedding.h>
#include <CoreEmbeddingTestAPI.h>
#include <drmmanagerimpl.h>

ENTER_PKTEST_NAMESPACE_CODE;

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(CoreEmbedding)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(CoreEmbedding)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

/*****************************************************************************
** Function:    Test_EMB_UpdateEmbeddedStore
**
** Synopsis:    Update the embedded store in a PlayReady object, using licenses from the HDS.
**
** Arguments:   Can pass any number of args with the following values:
**                    NULL_EMBEDDED_STORE                         - Pass NULL for Embedded Store Context
**                    NULL_LICENSE_STORE                          - Pass NULL for License Store Context
**                    NULL_LIC_EVAL_BACKUP                        - Pass NULL for License Evaluation Backup Context
**                    NULL_SCRATCH_BUFFER                         - Pass NULL for Scratch Buffer
**                    NULL_SCRATCH_BUFFER_SIZE                    - Pass NULL for Scratch Buffer Size
**                    SCRATCH_BUFFER_TOOSMALL                     - Pass Scratch Buffer Size smaller than required
**                    NULL_KID                                    - Pass NULL for KID
**                    NULL_MODIFIED_EST                           - Pass NULL for Modified EST Boolean
**                    SECSTORAGEUNINITIALIZED_LIC_EVAL            - Global Secure Storage in pLicEvalBackup uninitialized
**                    LIC_STORE_UNOPENED                          - License Store unopened
**                    EMB_STORE_UNINITIALIZED                     - Embedded Store uninitialized
**                    LIC_STORE_WITH_INVALID_DATASTORE            - License Stores with invalid data store pointers
**
** Returns:     DRM_SUCCESS       - Successfully embedded a license.
**              DRM_S_FALSE       - Did not update the embedded store, but no error occurred.
**              DRM_E_INVALIDARG  - An argument is invalid or improperly initialized.
*****************************************************************************/
DRM_RESULT DRM_CALL Test_EMB_UpdateEmbeddedStore( long argc, __in_ecount( argc ) char **argv )
{
#define NULL_EMBEDDED_STORE                        "NULL_EMBEDDED_STORE"
#define NULL_LICENSE_STORE                         "NULL_LICENSE_STORE"
#define NULL_LIC_EVAL_BACKUP                       "NULL_LIC_EVAL_BACKUP"
#define NULL_SCRATCH_BUFFER                        "NULL_SCRATCH_BUFFER"
#define NULL_SCRATCH_BUFFER_SIZE                   "NULL_SCRATCH_BUFFER_SIZE"
#define SCRATCH_BUFFER_TOOSMALL                    "SCRATCH_BUFFER_TOOSMALL"
#define NULL_KID                                   "NULL_KID"
#define NULL_MODIFIED_EST                          "NULL_MODIFIED_EST"
#define SECSTORAGEUNINITIALIZED_LIC_EVAL           "SECSTORAGEUNINITIALIZED_LIC_EVAL"
#define LIC_STORE_UNOPENED                         "LIC_STORE_UNOPENED"
#define EMB_STORE_UNINITIALIZED                    "EMB_STORE_UNINITIALIZED"
#define LIC_STORE_WITH_INVALID_DATASTORE           "LIC_STORE_WITH_INVALID_DATASTORE"
#define ALL_NULL                                   "ALL_NULL"

    DRM_RESULT                  dr                        = DRM_SUCCESS;
    DRM_APP_CONTEXT             *poManagerContext         = NULL;
    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal     = NULL;
    DRM_BOOL                    fModifiedEST              = FALSE;
    DRM_BOOL                    fESTOpened                = FALSE;
    DRM_CONST_STRING            dstrKID                   = EMPTY_DRM_STRING;
    DRM_KID                     oKID                      = { 0 };
    DRM_LICEVAL_CONTEXT         rgbLicEvalBuffer          = { 0 };
    DRM_DWORD                   cbScratchBuffer           = 0;
    DRM_BOOL                    fScratchBufferSizeSmall   = FALSE;
    DRM_BOOL                    fLicStoreDoNotOpen        = FALSE;
    DRM_DWORD                   i                         = 0;

    DRM_LICSTORE_CONTEXT        *pEmbeddedStore           = NULL;
    DRM_LICSTORE_CONTEXT        *pLicStore                = NULL;
    DRM_LICEVAL_CONTEXT         *pLicEvalBackup           = NULL;
    DRM_DOMAINSTORE_CONTEXT     *poDomainStoreContext     = NULL;
    DRM_BYTE                    *pbScratchBuffer          = NULL;
    DRM_DWORD                   *pcbScratchBuffer         = NULL;
    const DRM_KID               *poKID                    = NULL;
    DRM_BOOL                    *pfModifiedEST            = NULL;

    DRM_DST                     oDatastoreHDSUnitialized  = {eDRM_DST_NONE, {0}, NULL};
    DRM_DST                     oDatastoreEMBUnitialized  = {eDRM_DST_NONE, {0}, NULL};

    DRM_DST                     oOrigDatastoreEST                   = {eDRM_DST_NONE, {0}, NULL};
    DRM_BOOL                    fOrigDatastoreESTToBeRestored       = FALSE;
    DRM_DST                     *pLicStoreContextEmbedded_Datastore = NULL;
    DRM_DST                     *pLicStoreContextXML_Datastore      = NULL;
    DRM_DST                     *pLicStoreContextXMR_Datastore      = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    poAppContextInternal        = ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext;
    pEmbeddedStore              = &poAppContextInternal->oLicStoreContextEmbedded;
    poDomainStoreContext        = &poAppContextInternal->oDomainStoreContext;
    pLicStore                   = &poAppContextInternal->oLicStoreContextXMR;
    pLicEvalBackup              = &poAppContextInternal->oLicEvalContext;
    pbScratchBuffer             = poAppContextInternal->pbOpaqueBuffer + __CB_DECL( poAppContextInternal->cbTotalHeaderData);
    pcbScratchBuffer            = &cbScratchBuffer;
    poKID                       = &oKID;
    pfModifiedEST               = &fModifiedEST;

    while ( i < (DRM_DWORD) argc )
    {
        if ( argv[i] != NULL )
        {
            if ( ( DRMCRT_strncmp( argv[i], NULL_EMBEDDED_STORE, SIZEOF(NULL_EMBEDDED_STORE) ) == 0 ) )
            {
                pEmbeddedStore = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], NULL_LICENSE_STORE, SIZEOF(NULL_LICENSE_STORE) ) == 0 ) )
            {
                pLicStore = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], NULL_LIC_EVAL_BACKUP, SIZEOF(NULL_LIC_EVAL_BACKUP) ) == 0 ) )
            {
                pLicEvalBackup = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], SECSTORAGEUNINITIALIZED_LIC_EVAL, SIZEOF(SECSTORAGEUNINITIALIZED_LIC_EVAL) ) == 0 ) )
            {
                AssertChkArg( pLicEvalBackup != NULL );
                AssertChkArg( pLicEvalBackup->pcontextSSTGlobal != NULL );
                pLicEvalBackup->pcontextSSTGlobal->fInited = FALSE;
            }
            else if ( ( DRMCRT_strncmp( argv[i], NULL_SCRATCH_BUFFER, SIZEOF(NULL_SCRATCH_BUFFER) ) == 0 ) )
            {
                pbScratchBuffer = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], NULL_SCRATCH_BUFFER_SIZE, SIZEOF(NULL_SCRATCH_BUFFER_SIZE) ) == 0 ) )
            {
                pcbScratchBuffer = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], SCRATCH_BUFFER_TOOSMALL, SIZEOF(SCRATCH_BUFFER_TOOSMALL) ) == 0 ) )
            {
                fScratchBufferSizeSmall = TRUE;
            }
            else if ( ( DRMCRT_strncmp( argv[i], NULL_KID, SIZEOF(NULL_KID) ) == 0 ) )
            {
                poKID = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], NULL_MODIFIED_EST, SIZEOF(NULL_MODIFIED_EST) ) == 0 ) )
            {
                pfModifiedEST = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[i], LIC_STORE_UNOPENED, SIZEOF(LIC_STORE_UNOPENED) ) == 0 ) )
            {
                fLicStoreDoNotOpen = TRUE;
            }
            else if ( ( DRMCRT_strncmp( argv[i], EMB_STORE_UNINITIALIZED, SIZEOF(EMB_STORE_UNINITIALIZED) ) == 0 ) )
            {
                /* remember original values to reset them later */
                fOrigDatastoreESTToBeRestored = TRUE;
                memcpy(&oOrigDatastoreEST, &poAppContextInternal->oDatastoreEST, SIZEOF(oOrigDatastoreEST));
                pLicStoreContextEmbedded_Datastore = poAppContextInternal->oLicStoreContextEmbedded.pDatastore;

                memcpy(&poAppContextInternal->oDatastoreEST, &oDatastoreEMBUnitialized, SIZEOF(oDatastoreEMBUnitialized));
                poAppContextInternal->oLicStoreContextEmbedded.pDatastore = &oDatastoreEMBUnitialized;
            }
            else if ( ( DRMCRT_strncmp( argv[i], LIC_STORE_WITH_INVALID_DATASTORE, SIZEOF(LIC_STORE_WITH_INVALID_DATASTORE) ) == 0 ) )
            {
                /* remember original values to reset them later */
                pLicStoreContextXML_Datastore      = poAppContextInternal->oLicStoreContextXML.pDatastore;
                pLicStoreContextXMR_Datastore      = poAppContextInternal->oLicStoreContextXMR.pDatastore;

                poAppContextInternal->oLicStoreContextXML.pDatastore = &oDatastoreHDSUnitialized;
                poAppContextInternal->oLicStoreContextXMR.pDatastore = &oDatastoreEMBUnitialized;
            }
            else if ( ( DRMCRT_strncmp( argv[i], ALL_NULL, SIZEOF(ALL_NULL) ) == 0 ) )
            {
                pEmbeddedStore          = NULL;
                pLicStore               = NULL;
                pLicEvalBackup          = NULL;
                pbScratchBuffer         = NULL;
                pcbScratchBuffer        = NULL;
                poKID                   = NULL;
                pfModifiedEST           = NULL;
            }
        }
        i = i + 1;
    }

    /*
    ** Verify that a PlayReady Object with an embedded store record
    ** has been set using Drm_Content_SetProperty.
    */
    ChkBOOL( poAppContextInternal->eHeaderInContext != DRM_CSP_HEADER_NOT_SET
             && poAppContextInternal->cbTotalHeaderData > 0, DRM_E_TEST_PRO_HEADER_NOT_SET);
    ChkBOOL( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
             && poAppContextInternal->cbEmbeddedStore > 0
             && poAppContextInternal->pbEmbeddedStore != NULL, DRM_E_TEST_NON_PRO_HEADER_TYPE );

    /*
    ** Retrieve the KID from the Header
    */
    DSTR_FROM_PB( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                   poAppContextInternal->pwszHeaderData,
                   poAppContextInternal->cbHeaderData );
    ChkDR( DRM_HDR_GetAttribute( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                                 NULL,
                                 DRM_HEADER_ATTRIB_KID,
                                 &dstrKID,
                                 0 ) );
    ChkDR( DRM_UTL_DecodeKID( &dstrKID, &poAppContextInternal->KID ) );
    MEMCPY( &oKID, &poAppContextInternal->KID, SIZEOF( DRM_KID ) );

    /*
    ** Set up enumeration over embedded store and HDS (XMR) store
    ** Get the embedded license store from the PlayReady Object
    ** If it does not exist, do not create an empty one but simply error out.
    ** This is because this test api assumes that the embedded store should
    ** already be present
    */

    ChkDR( DRM_DST_OpenStore( poAppContextInternal->pOEMContext,
                              poAppContextInternal->pbEmbeddedStore,
                              poAppContextInternal->cbEmbeddedStore,
                              0,
                              &poAppContextInternal->oDatastoreEST ) );

    fESTOpened = TRUE;

    if ( fLicStoreDoNotOpen == FALSE )
    {
        ChkDR( DRM_LST_Open( &poAppContextInternal->oLicStoreContextEmbedded,
                             &poAppContextInternal->oDatastoreEST,
                             eDRM_LICENSE_STORE_XMR ) );
    }

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    cbScratchBuffer = poAppContextInternal->cbOpaqueBuffer - poAppContextInternal->cbTotalHeaderData;

    if ( fScratchBufferSizeSmall && ( cbScratchBuffer > 0 ) && ( pcbScratchBuffer != NULL ) )
    {
        /* DRM_EMB_UpdateEmbeddedStore has a limit on this size.*/
        *pcbScratchBuffer = DRM_MAX_LICENSESIZE - 1;
    }

    dr = DRM_EMB_UpdateEmbeddedStore( pEmbeddedStore,
                                      pLicStore,
                                      poDomainStoreContext,
                                      pLicEvalBackup,
                                      &rgbLicEvalBuffer,
                                      pbScratchBuffer,
                                      pcbScratchBuffer,
                                      poKID,
                                      pfModifiedEST);
    if ( fScratchBufferSizeSmall )
    {
        ChkBOOL( dr == DRM_E_BUFFERTOOSMALL
                 && *pcbScratchBuffer  == DRM_MAX_LICENSESIZE, DRM_E_TEST_UNEXPECTED_OUTPUT );

        dr = DRM_SUCCESS;
    }

ErrorExit:

    if ( poAppContextInternal != NULL )
    {
        /* Restore original values in the context if they were set to some uninitialzied or invalid values */
        if ( fOrigDatastoreESTToBeRestored )
        {
            memcpy(&poAppContextInternal->oDatastoreEST, &oOrigDatastoreEST, SIZEOF(oOrigDatastoreEST));
        }

        if ( pLicStoreContextEmbedded_Datastore != NULL )
        {
            poAppContextInternal->oLicStoreContextEmbedded.pDatastore = pLicStoreContextEmbedded_Datastore;
        }

        if ( pLicStoreContextXML_Datastore != NULL )
        {
            poAppContextInternal->oLicStoreContextXML.pDatastore = pLicStoreContextXML_Datastore;
        }

        if ( pLicStoreContextXMR_Datastore != NULL )
        {
            poAppContextInternal->oLicStoreContextXMR.pDatastore = pLicStoreContextXMR_Datastore;
        }

        if ( poAppContextInternal->oLicStoreContextEmbedded.fInited )
        {
            DRM_LST_Close( &poAppContextInternal->oLicStoreContextEmbedded );
        }

        if ( fESTOpened )
        {
            DRM_DST_CloseStore( &poAppContextInternal->oDatastoreEST );
        }
    }
    /*
    ** Return DRM_S_FALSE if no updates were made, or DRM_SUCCESS if updates were made.
    */
    if ( DRM_SUCCEEDED( dr ) )
    {
        dr = fModifiedEST ? DRM_SUCCESS : DRM_S_FALSE;
    }

    return dr;
}

BEGIN_TEST_API_LIST( CoreEmbedding )
    API_ENTRY(Test_EMB_UpdateEmbeddedStore)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;
