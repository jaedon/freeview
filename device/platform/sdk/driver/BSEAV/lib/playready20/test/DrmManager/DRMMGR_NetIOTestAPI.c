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

#define GET_PROTOCOL_ACTION( get_iIndex, get_eAction )               \
do {                                                                 \
    ChkArg( argv[ get_iIndex ] != NULL );                            \
    ChkDR( _ParseProtocol( argv[get_iIndex],&get_eAction ) );        \
} while( FALSE )

static DRM_RESULT DRM_CALL _ParseProtocol( IN const DRM_CHAR *f_pszArg, __out_ecount(1) eDRM_TEST_NET_ACTION *f_eAction )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRMCRT_strncmp( f_pszArg, "LICACK", 6) == 0 )
    {
        *f_eAction = eDRM_TEST_NET_LICACK;
    }
    else if( DRMCRT_strncmp( f_pszArg, "LICGET_LEGACY", 13) == 0 )
    {
        *f_eAction = eDRM_TEST_NET_LICGET_LEGACY;
    }
    else if( DRMCRT_strncmp( f_pszArg, "LICGET", 6) == 0 )
    {
        *f_eAction = eDRM_TEST_NET_LICGET;
    }
    else if( DRMCRT_strncmp( f_pszArg, "LEAVE", 5) == 0 )
    {
        *f_eAction = eDRM_TEST_NET_DOMLEAVE;
    }
    else if( DRMCRT_strncmp( f_pszArg, "JOIN", 4) == 0 )
    {
        *f_eAction = eDRM_TEST_NET_DOMJOIN;
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
ErrorExit:
    return dr;
}



/*****************************************************************************
** Function:    TestSendChallenge
**
** Synopsis:    Uses tOEM_SendNetData to send the challenge to the server
**
** Arguments:
**      argv[0]: Protocol, LICACK, LICGET, LEAVE, JOIN, LICGET_LEGACY
**      argv[1]: Challenge Buffer in the global test data array
**      argv[2]: Response buffer in the global test data array
**      argv[3]: opt, Index in the global test data array holding the URL to use
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note: 
**
******************************************************************************/
DRM_RESULT DRM_CALL TestSendChallenge( long argc, __in_ecount(argc) char **argv )
{
#if DRM_TEST_SUPPORT_NET_IO
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_CHAR  *pszUrl            = NULL;
    DRM_BYTE  *pbChallenge       = NULL;
    DRM_DWORD *pcbChallenge      = NULL;
    DRM_BYTE  *pbFormattedChallenge  = NULL;
    DRM_BYTE  *pbResponse        = NULL;
    DRM_DWORD *pcbResponse       = NULL;
    DRM_BYTE  *pbResponse1       = NULL;
    DRM_DWORD  cbResponse1       = 0;
    DRM_CHAR  *pszChallengeURL   = NULL;
    eDRM_TEST_NET_ACTION eAction = eDRM_TEST_NET_UNKNOWN;

    ChkArg( argc > 2 );

    GET_PROTOCOL_ACTION( 0, eAction );
    MAP_TO_GLOBAL_TEST_VAR_EX( pbChallenge, pcbChallenge, argv[1] );

    /*
    ** Get CHALLENGE_URL_VAR
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( CHALLENGE_URL_VAR, ( DRM_VOID ** ) &pszChallengeURL, NULL ) ) )
    {
        pszChallengeURL = NULL;
    }

    if ( argc >= 4 && argv[3] != NULL )
    {
        MAP_TO_GLOBAL_TEST_VAR( pszUrl, argv[3] );
    }
    else
    {
        pszUrl = pszChallengeURL;
    }

    LogBlob( "SendChallenge", "Challenge:", pbChallenge, *pcbChallenge );

    SAFE_OEM_FREE( pbResponse1 );

    ChkDR( tOEM_SendNetData( pszUrl, eAction, pbChallenge, *pcbChallenge, &pbResponse1, &cbResponse1 ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_EX( pbResponse, pcbResponse, argv[2], cbResponse1 );
    MEMCPY( pbResponse, pbResponse1, cbResponse1 );
    LogBlob( "SendChallenge", "Response:", pbResponse, *pcbResponse );
    
ErrorExit:
    SAFE_OEM_FREE( pbResponse1 );
    if ( pbFormattedChallenge != NULL )
    {
        SAFE_OEM_FREE( pbFormattedChallenge );
    }
    return dr;
#else
    return DRM_E_NOTIMPL;
#endif
}

EXIT_PKTEST_NAMESPACE_CODE
