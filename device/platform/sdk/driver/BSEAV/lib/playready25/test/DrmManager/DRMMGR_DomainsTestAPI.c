/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

/*****************************************************************************
** Function:    SetDomainID
**
** Synopsis:
**
** Arguments:   argv[0]: index to the buffer for setting the ID
**              argv[1]: opt, index of the buffer holding the service ID
**              argv[2]: opt, index of the buffer holding the account ID
**              argv[3]: opt, revision ID. Default Value used if missing or NULL
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL SetDomainID( __in long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_DOMAIN_ID   *poDomainID         = NULL;
    DRM_SUBSTRING    dsstrTemp          = EMPTY_DRM_SUBSTRING;
    const DRM_CHAR  *szDefaultAccountID = "NjU0MzIxMDk4NzY1NDMyMQ==";
    const DRM_CHAR  *szDefaultServiceID = "AH+03juKbUGbHl1V/QIwRA==";
    const DRM_CHAR  *pszAccountID       = NULL;
    const DRM_CHAR  *pszServiceID       = NULL;
    DRM_DWORD        cbAccountID        = SIZEOF(DRM_GUID);
    DRM_DWORD        cbServiceID        = SIZEOF(DRM_GUID);

    ChkArg( argc >= 1 );
    MAP_TO_GLOBAL_TEST_VAR_Alloc(poDomainID, argv[0], SIZEOF( DRM_DOMAIN_ID ) );

    if ( argc >= 2 )
    {
        MAP_TO_GLOBAL_TEST_VAR(pszServiceID, argv[1]);
    }
    if ( argc >= 3 )
    {
        MAP_TO_GLOBAL_TEST_VAR(pszAccountID, argv[2]);
    }
    if ( pszAccountID == NULL )
    {
        pszAccountID = szDefaultAccountID;
    }
    if ( pszServiceID == NULL )
    {
        pszServiceID = szDefaultServiceID;
    }

    dsstrTemp.m_ich = 0;
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( pszServiceID ), &dsstrTemp.m_cch ) );
    ChkDR( DRM_B64_DecodeA( pszServiceID, &dsstrTemp, &cbServiceID, (DRM_BYTE *) &poDomainID->m_oServiceID, 0 ) );

    dsstrTemp.m_ich = 0;
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( pszAccountID ), &dsstrTemp.m_cch ) );
    ChkDR( DRM_B64_DecodeA( pszAccountID, &dsstrTemp, &cbAccountID, (DRM_BYTE *) &poDomainID->m_oAccountID, 0 ) );

    if ( argc > 3 && argv[3] != NULL && DRMCRT_strlen( argv[3] ) != 0 )
    {
        poDomainID->m_dwRevision = (DRM_DWORD) strtoul( argv[3], NULL, 0 );
    }
    else
    {
        poDomainID->m_dwRevision = 0;
    }

ErrorExit:
    return dr;
}

/********************************************************************
** Function:    ProcessDomainJoinResponse
**
** Synopsis:
**
** Arguments:   argv[0]: index in the global test data array holding the reponse
**              argv[1]: Response file, overrides the Buffer
**              argv[2]: Drm_JoinDomain_ProcessResponse flags
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
*********************************************************************/
DRM_RESULT DRM_CALL ProcessDomainJoinResponse( __in long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                      dr               = DRM_SUCCESS;
    DRM_RESULT                      dr2              = DRM_SUCCESS;
    DRM_BYTE                       *pbResponse       = NULL;
    DRM_DWORD                       cbResponse       = 0;
    DRM_DWORD                      *pcbResponse      = &cbResponse;
    DRM_APP_CONTEXT                *poManagerContext = NULL;
    DRM_PROCESS_DJ_RESPONSE_FLAG    dwFlags          = DRM_PROCESS_DJ_RESPONSE_SIGNATURE_NOT_REQUIRED;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc >= 1 );

    if ( argc == 1 || argv[1] == NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR_EX(pbResponse, pcbResponse, argv[0]);
    }
    if( argc > 1 && argv[1] != NULL )
    {
        SAFE_OEM_FREE( pbResponse );
        ChkBOOL( (LoadTestFile( "files", argv[1], &pbResponse, pcbResponse ) || LoadTestFile( "", argv[1], &pbResponse, pcbResponse )),
             DRM_E_FILE_READ_ERROR);
    }
    if( argc > 2 && argv[2] != NULL )
    {
        dwFlags = (DRM_PROCESS_DJ_RESPONSE_FLAG)strtol( argv[2], NULL, 0 );
    }

    ChkDR( Drm_JoinDomain_ProcessResponse( poManagerContext,
                                           dwFlags,
                                           pbResponse,
                                           *pcbResponse,
                                           &dr2,
                                           NULL ) );

ErrorExit:
    return dr;
}

/********************************************************************
** Function:    GenerateDomainJoinChallenge
**
** Synopsis:
**
** Arguments:   argv[0]: index in the global test data array holding the buffer
**              argv[1]: Index to the domain ID in the global test data array
**              argv[2]: custom data
**              argv[3]: friendly name
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
*********************************************************************/
DRM_RESULT DRM_CALL GenerateDomainJoinChallenge( __in long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_BYTE        *pbChallenge      = NULL;
    DRM_DWORD        cbChallenge      = 0;
    DRM_DWORD       *pcbChallenge     = &cbChallenge;
    DRM_APP_CONTEXT *poManagerContext = NULL;
    DRM_DOMAIN_ID   *poDomainID       = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc    >= 4 );
    ChkArg( argv[2] != NULL );
    ChkArg( argv[3] != NULL );

    MAP_TO_GLOBAL_TEST_VAR(poDomainID, argv[1]);

    dr = Drm_JoinDomain_GenerateChallenge( poManagerContext,
                                           DRM_REGISTER_CUSTOM_DATA,
                                           poDomainID,
                                           argv[3],
                                (DRM_DWORD)DRMCRT_strlen(argv[3]),
                                           argv[2],
                                (DRM_DWORD)DRMCRT_strlen( argv[2]),
                                           NULL,
                                           pcbChallenge );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc_EX(pbChallenge, pcbChallenge, argv[0], cbChallenge );
        ChkDR( Drm_JoinDomain_GenerateChallenge( poManagerContext,
                                                 DRM_REGISTER_CUSTOM_DATA,
                                                 poDomainID,
                                                 argv[3],
                                      (DRM_DWORD)DRMCRT_strlen(argv[3]),
                                                 argv[2],
                                      (DRM_DWORD)DRMCRT_strlen( argv[2]),
                                                 pbChallenge,
                                                 pcbChallenge) );
    }

ErrorExit:
    return dr;
}


/********************************************************************
** Function:    ProcessDomainJoinResponse
**
** Synopsis:
**
** Arguments:   argv[0]: index in the global test data array holding the buffer
**              argv[1]: Response file, overrides the Buffer
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
*********************************************************************/
DRM_RESULT DRM_CALL ProcessDomainLeaveResponse( __in long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_RESULT       dr2              = DRM_SUCCESS;
    DRM_BYTE        *pbResponse       = NULL;
    DRM_DWORD       *pcbResponse      = NULL;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc    >= 1 );

    MAP_TO_GLOBAL_TEST_VAR_EX(pbResponse, pcbResponse, argv[0]);

    if( argc > 1 && argv[1] != NULL )
    {
        SAFE_OEM_FREE( pbResponse );
        ChkBOOL( LoadTestFile( "files", argv[1], &pbResponse, pcbResponse ),
             DRM_E_FILE_READ_ERROR);
    }
    ChkDR( Drm_LeaveDomain_ProcessResponse( poManagerContext,
                                           pbResponse,
                                           *pcbResponse,
                                           &dr2 ) );
    if( argc > 2 && argv[2] != NULL )
    {
        ChkBOOL( strtol( argv[2], NULL, 0 ) == dr2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    return dr;
}

/********************************************************************
** Function:    GenerateDomainLeaveChallenge
**
** Synopsis:
**
** Arguments:   argv[0]: index in the global test data array holding the buffer
**              argv[1]: Index to the domain ID in the global test data array
**              argv[2]: custom data
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
*********************************************************************/
DRM_RESULT DRM_CALL GenerateDomainLeaveChallenge( __in long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_BYTE        *pbResponse       = NULL;
    DRM_DWORD        cbResponse       = 0;
    DRM_DWORD       *pcbResponse      = &cbResponse;
    DRM_DOMAIN_ID   *poDomainID       = NULL;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc    >= 3 );
    ChkArg( argv[2] != NULL );

    MAP_TO_GLOBAL_TEST_VAR(poDomainID, argv[1]);


    dr = Drm_LeaveDomain_GenerateChallenge( poManagerContext,
                                           DRM_REGISTER_CUSTOM_DATA,
                                           poDomainID,
                                           argv[2],
                                (DRM_DWORD)DRMCRT_strlen( argv[2]),
                                           NULL,
                                           pcbResponse );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc_EX(pbResponse, pcbResponse, argv[0], cbResponse );
        ChkDR( Drm_LeaveDomain_GenerateChallenge( poManagerContext,
                                                 DRM_REGISTER_CUSTOM_DATA,
                                                 poDomainID,
                                                 argv[2],
                                      (DRM_DWORD)DRMCRT_strlen( argv[2]),
                                                 pbResponse,
                                                 pcbResponse) );
    }

ErrorExit:
    return dr;
}

/********************************************************************
** Function:    GenerateDomainJoinResponse
**
** Synopsis:
**
** Arguments:   argv[0]: Global variable name in which to place the response
**              argv[1]: Custom Data (may be NULL)
**              argv[2]: Protocol Version (typically "2")
**              argv[3]: Account ID (b64)
**              argv[4]: Service ID (b64)
**              argv[5]: Domain Certificate Chain (raw binary file on disk)
**              argv[6]: Domain Revision Count (dword, base-10 encoded)
**              argv[7]: First Domain Revision (dword, base-16 encoded)
**              argv[8]: First Domain Private Key (b64)
**          Further argumens are additional pairs of Domain Revision + Domain Private Key
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
*********************************************************************/
DRM_RESULT DRM_CALL GenerateDomainJoinResponse( __in long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                     = DRM_SUCCESS;
    DRM_APP_CONTEXT *pAppContext            = NULL;
    DRM_BB_CONTEXT  *poBBXContext           = NULL;
    DRM_DWORD        cDomainRevisions       = 0;
    DRM_DWORD       *rgdwDomainRevisions    = NULL;
    DRM_BYTE        *pbClearDomainKeys      = NULL;
    DRM_BYTE       **ppbClearDomainKeys     = NULL;
    DRM_DWORD        idx                    = 0;
    DRM_BYTE        *pbCertChain            = NULL;
    DRM_DWORD        cbCertChain            = 0;
    DRM_CHAR        *pszCertChain           = NULL;
    DRM_DWORD        cchCertChain           = 0;
    DRM_SUBSTRING    dsstr                  = {0};
    DRM_DWORD        cch                    = 0;

    GET_SHARED_APP_CONTEXT( pAppContext );

    /* Grab blackbox context from app context */
    {
        DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )pAppContext;
        poBBXContext = &poAppContextInternal->oBlackBoxContext;
    }

    ChkArg( argc >= 9 );

    /* Parse domain revision count */
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[6]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[6], cch, 10, &cDomainRevisions ) );
    ChkArg( cDomainRevisions > 0 );

    /* Allocate domain revision buffers */
    ChkMem( rgdwDomainRevisions = (DRM_DWORD*)Oem_MemAlloc( cDomainRevisions * SIZEOF(DRM_DWORD) ) );
    ChkMem( pbClearDomainKeys =   (DRM_BYTE* )Oem_MemAlloc( cDomainRevisions * DRM_AES_KEYSIZE_128 * 2 ) );
    ChkMem( ppbClearDomainKeys =  (DRM_BYTE**)Oem_MemAlloc( cDomainRevisions * SIZEOF(DRM_BYTE*) ) );

    /* Populate domain revision buffers */
    for( idx = 0; idx < cDomainRevisions; idx++ )
    {
        DRM_DWORD       iArgRevision    = 7 + ( idx * 2 );
        DRM_DWORD       iArgPrivKey     = iArgRevision + 1;
        DRM_DWORD       cbDest          = DRM_AES_KEYSIZE_128 * 2;

        dsstr.m_cch = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 * 2 );

        ChkArg( (DRM_DWORD)argc > iArgPrivKey );
        ChkArg( DRMCRT_strlen( argv[iArgPrivKey] ) == dsstr.m_cch );
        ppbClearDomainKeys[idx] = &pbClearDomainKeys[ idx * DRM_AES_KEYSIZE_128 * 2 ];
        ChkDR( DRM_B64_DecodeA( argv[iArgPrivKey], &dsstr, &cbDest, ppbClearDomainKeys[idx], 0 ) );

        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[iArgRevision]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[iArgRevision], cch, 16, &rgdwDomainRevisions[idx] ) );
    }

    /* Load cert chain and b64 encode */
    ChkBOOL( LoadTestFile( "files", argv[5], &pbCertChain, &cbCertChain ), DRM_E_FILE_READ_ERROR );
    cchCertChain = CCH_BASE64_EQUIV( cbCertChain );
    ChkMem( pszCertChain =  (DRM_CHAR*)Oem_MemAlloc( cchCertChain + 1 ) );
    DRMCRT_memset( pszCertChain, 0, cchCertChain + 1 );
    ChkDR( DRM_B64_EncodeA( pbCertChain, cbCertChain, pszCertChain, &cchCertChain, 0 ) );

    ChkDR( Test_CreateNewDomainJoinResponse(
        poBBXContext,
        argv[0],
        argv[1],
        argv[2],
        argv[3],
        argv[4],
        pszCertChain,
        cDomainRevisions,
        rgdwDomainRevisions,
        ppbClearDomainKeys ) );

ErrorExit:
    SAFE_OEM_FREE( rgdwDomainRevisions );
    SAFE_OEM_FREE( pbClearDomainKeys );
    SAFE_OEM_FREE( ppbClearDomainKeys );
    SAFE_OEM_FREE( pbCertChain );
    SAFE_OEM_FREE( pszCertChain );
    return dr;
}


EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

