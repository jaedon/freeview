/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

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
    DRM_RESULT     dr                 = DRM_SUCCESS;
    DRM_DOMAIN_ID *poDomainID         = NULL;
    DRM_SUBSTRING  dsstrTemp          = EMPTY_DRM_SUBSTRING;
    DRM_CHAR      *szDefaultAccountID = "NjU0MzIxMDk4NzY1NDMyMQ==";
    DRM_CHAR      *szDefaultServiceID = "AH+03juKbUGbHl1V/QIwRA==";
    DRM_CHAR      *pszAccountID       = NULL;
    DRM_CHAR      *pszServiceID       = NULL;
    DRM_DWORD      cbAccountID        = SIZEOF(DRM_GUID);
    DRM_DWORD      cbServiceID        = SIZEOF(DRM_GUID);

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
    dsstrTemp.m_cch = DRMCRT_strlen( pszServiceID );
    ChkDR( DRM_B64_DecodeA( pszServiceID, &dsstrTemp, &cbServiceID, (DRM_BYTE *) &poDomainID->m_oServiceID, 0 ) );

    dsstrTemp.m_ich = 0;
    dsstrTemp.m_cch = DRMCRT_strlen( pszAccountID );
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
DRM_RESULT DRM_CALL ProcessDomainJoinResponse( IN long argc, IN __in_ecount( argc ) char **argv )
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
             DRM_E_FILEREADERROR);
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
DRM_RESULT DRM_CALL GenerateDomainJoinChallenge( IN long argc, IN __in_ecount( argc ) char **argv )
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
DRM_RESULT DRM_CALL ProcessDomainLeaveResponse( IN long argc, IN __in_ecount( argc ) char **argv )
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
             DRM_E_FILEREADERROR);
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
DRM_RESULT DRM_CALL GenerateDomainLeaveChallenge( IN long argc, IN __in_ecount( argc ) char **argv )
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

EXIT_PKTEST_NAMESPACE_CODE
