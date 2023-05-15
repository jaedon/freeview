/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DrmManagerTestAPI.h>
#include <drmxmlparser.h>
#include <drmsoapxmlutility.h>
#include <drmsymopt.h>
#include <drmdla.h>
#include <drmmodelrevocation.h>

ENTER_PKTEST_NAMESPACE_CODE

#define GET_ADDITIONAL_DATA_TYPE( get_iIndex, get_dwType )             \
do {                                                                   \
    ChkArg( argv[ get_iIndex ] != NULL );                              \
    ChkDR( _ParseAdditionalDataType( argv[get_iIndex],&get_dwType ) ); \
} while( FALSE )


#define MAX_LICENSE_ACK     10
#define MAX_TRANSACTION_ID  100

/*
**  The following 3 GUIDs are the only inclusion list GUIDs that will be accepted by the
**  _LicenseProcessResponseCallback function
*/

/*
** {4117F57A-D8BD-4ac7-AD1B-D6FE5A3EA4E8}
*/
static const DRM_GUID g_rgguidInclusionListTestAcceptable[] =
{ { 0x4117f57a, 0xd8bd, 0x4ac7, { 0xad, 0x1b, 0xd6, 0xfe, 0x5a, 0x3e, 0xa4, 0xe8 } },

/*
** {95EE268C-2D75-493b-9708-E85E02ECF869}
*/
{ 0x95ee268c, 0x2d75, 0x493b, { 0x97, 0x8, 0xe8, 0x5e, 0x2, 0xec, 0xf8, 0x69 } },

/*
** {87CDF6BC-4E01-48cd-93B6-AB7B352B107D}
*/
{ 0x87cdf6bc, 0x4e01, 0x48cd, { 0x93, 0xb6, 0xab, 0x7b, 0x35, 0x2b, 0x10, 0x7d } } };

/*******************************************************************************
** Function:    _LicenseProcessResponseCallback
**
** Synopsis:    The callback function used to test processing of lic responses.
**
** Arguments:   [pvData]
**              [eReason]
**              [pvContext]
**
** Returns:
**              DRM_SUCCESS       The function completed successfully.
**              DRM_E_INVALIDARG  One or more arguments were invalid
**
*******************************************************************************/
DRM_RESULT DRM_CALL _LicenseProcessResponseCallback(
    IN DRM_VOID                        *pvData,
    IN DRM_STORE_LICENSE_CALLBACK_ENUM  eReason,
    IN DRM_VOID                        *pvContext )
{
    DRM_RESULT                          dr              = DRM_SUCCESS;
    DRM_INCLUSION_LIST_CALLBACK_STRUCT *pInclStruct     = (DRM_INCLUSION_LIST_CALLBACK_STRUCT*)pvData;
    DRM_DWORD                           i               = 0;
    DRM_DWORD                           j               = 0;
    DRM_DWORD                           dwNumValid      = 0;

    switch( eReason )
    {
        case DRM_STORE_LICENSE_NOTIFY_INCLUSION_LIST:

            ChkArg( pInclStruct != NULL );

            /*
            ** this should be 0 since we're looking at licenses individually, not in a chain
            */
            ChkArg( pInclStruct->dwChainDepth == 0 );

            for( i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++ )
            {
                if( pInclStruct->rgfInclusionListValid[i][0] )
                {
                    /*
                    ** We have a valid GUID here, check if its acceptable
                    */
                    DRM_BOOL fIsAcceptable = FALSE;

                    dwNumValid++;

                    for( j = 0; j < NO_OF( g_rgguidInclusionListTestAcceptable ); j++ )
                    {
                        if( DRM_UTL_AreGUIDEqual( &g_rgguidInclusionListTestAcceptable[j],
                                    &pInclStruct->rgInclusionList[i] ) )
                        {
                            fIsAcceptable = TRUE;
                            break;
                        }
                    }

                    ChkBOOL( fIsAcceptable, DRM_E_TEST_LICENSE_RESPONSE_ERROR);
                }
            }

            /*
            ** There must be at least one GUID
            */
            ChkBOOL( dwNumValid > 0, DRM_E_TEST_LICENSE_RESPONSE_ERROR);

            break;

        case DRM_STORE_LICENSE_NOTIFY_KID:
            /*
            ** Callbacks with this reason will have pvData as a DRM_CONST_STRING
            */
            break;
        default:
            ChkDR( DRM_E_TEST_LICENSE_RESPONSE_ERROR);
    }

ErrorExit:
    return dr;
}


/*******************************************************************************
** Function:    _ParseAdditionalDataType
**
** Synopsis:    Parses f_pszArg to find the data get from the Additional Data
**              in the server response.
**
** Arguments:   [f_pszArg]   Null Terminated string to search for one of the
**                           defined types.
**                           Options are:
**                                        CUSTOM
**                                        REDIRECT_URL
**                                        ACCOUNT_ID
**                                        SERVICE_ID
**              [f_pdwType]  on output, holds the value of the type parsed.
**
** Returns:
**              DRM_SUCCESS       The function completed successfully.
**              DRM_E_INVALIDARG  One or more arguments were invalid
**
*******************************************************************************/
static DRM_RESULT DRM_CALL _ParseAdditionalDataType( IN const DRM_CHAR *f_pszArg, __out_ecount(1) DRM_DWORD *f_pdwType )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRMCRT_strncmp( f_pszArg, "CUSTOM", 6) == 0 )
    {
        *f_pdwType = DRM_GARD_CUSTOM_DATA;
    }
    else if( DRMCRT_strncmp( f_pszArg, "REDIRECT_URL", 12) == 0 )
    {
        *f_pdwType = DRM_GARD_REDIRECT_URL;
    }
    else if( DRMCRT_strncmp( f_pszArg, "ACCOUNT_ID", 10) == 0 )
    {
        *f_pdwType = DRM_GARD_ACCOUNT_ID;
    }
    else if( DRMCRT_strncmp( f_pszArg, "SERVICE_ID", 10) == 0 )
    {
        *f_pdwType = DRM_GARD_SERVICE_ID;
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }
ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerGenerateChallenge
**
** Synopsis:    Test wrapper function for Drm_LicenseAcq_GenerateChallenge
**
** Arguments:   argv[0]    : DRMManager context status: NULL or Normal
**              argv[1]    : Optional silent LA URL to use in the challenge (will be
**                           overwritten)
**              argv[2]    : Optional size of silent LA URL to use (may be overwritten)
**              argv[3]    : Optional non silent LA URL to use in the challenge (will be
**                           overwritten)
**              argv[4]    : Optional size of non silent LA URL to use (may be overwritten)
**              argv[5]    : Optional challenge (will be overwritten)
**              argv[6]    : Optional size of challenge (may be overwritten)
**              argv[7]    : Optional expected silent LA URL
**              argv[8]    : Optional expected silent LA URL size
**              argv[9]    : Optional expected non silent LA URL
**              argv[10]   : Optional expected non silent LA URL size
**              argv[11]   : Optional expected challenge string
**              argv[12]   : Misalignment
**              argv[13]   : Flags:
**                              (LSB) Bit 0 - indicates that argv[0] should be checked
**                               Bit 1 - indicates that the challenge should be stored
**                               Bit 2 - indicates that "\\r", "\\n", and "\\t" strings in
**                                       custom data argv[15] should be transmitted as
**                                       " \r", "\n ", and "\t " respectively, i.e.
**                                       convert them to REAL special characters.
**              argv[14]   : Global test data buffer for storing the challenge data
**              argv[15]   : Optional custom data
**              argv[16]   : Optional max allowed challenge size
**
** Returns:     DRM_SUCCESS                  - on success
**              DRM_E_TEST_URL_ERROR         - If expected URL does not match output URL
**              DRM_E_TEST_UNEXPECTED_OUTPUT - If expected part of challenge is not
**                                             present in the actual challenge
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGenerateChallenge( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                    = DRM_E_TEST_INCOMPLETE;
    DRM_RESULT       drInternal            = DRM_E_TEST_INCOMPLETE;
    DRM_APP_CONTEXT  *pLocalManagerContext = NULL;
    DRM_LONG         i                     = 0;
    DRM_DWORD        dwOption              = 0;
    DRM_DWORD        dwMisAlignment        = 0;
    DRM_DWORD        dwTempSilentURLSize   = 0;
    DRM_DWORD        dwTempNonSilentURLSize= 0;
    DRM_DWORD        dwTempChlgSize        = 0;
    DRM_DWORD        dwMaxChallengeSize    = 0;
    DRM_CHAR         *pszSilentUrl         = NULL;
    DRM_DWORD        cchSilentUrl          = 0;
    DRM_CHAR         *pszNonSilentUrl      = NULL;
    DRM_DWORD        cchNonSilentUrl       = 0;
    DRM_DWORD        cchChallenge          = 0;
    DRM_CHAR         *pszChallenge         = NULL;
    DRM_CHAR         *pszTempChallenge     = NULL;
    DRM_ANSI_STRING  dastrCustomData       = EMPTY_DRM_STRING;
    DRM_BYTE         *pbChallenge          = NULL;
    DRM_DWORD        *pcbChallenge         = NULL;
    DRM_DWORD        *pdwNumRights       = NULL;
    DRM_CONST_STRING **ppRightsArray     = NULL;
    DRM_CHAR         *pszChallengeURL    = NULL;
    DRM_DWORD        cchChallengeURL    = 0;

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom TestManagerGenerateChallenge: argv[%d]=%s", i, argv[i]);
    }

    if( argc < 14 )
    {
        Log("Trace", "\t\tFrom TestManagerGenerateChallenge: error in number of arguments.");
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

    ChkArg( argv[13] != NULL );
    ChkDR( DRMCRT_AtoDWORD( argv[13], DRMCRT_strlen( argv[13] ), 10, &dwOption ) );

    MAP_TO_GLOBAL_TEST_VAR( pdwNumRights, NUMBER_OF_RIGHTS_VAR );
    MAP_TO_GLOBAL_TEST_VAR( ppRightsArray, RIGHTS_ARRAY_VAR );

    /*
    ** Check if shared or local manager context is used
    */
    if ( (dwOption & 1) )
    {
        if ( argv[0] != NULL )
        {
            /*
            ** Context is non-NULL and non-Shared
            */
            ChkMem( pLocalManagerContext = (DRM_APP_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
            MEMSET( pLocalManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) );
        }
    }
    else
    {
        /*
        ** Use shared context
        */
        GET_SHARED_APP_CONTEXT( pLocalManagerContext );
    }

    /*
    ** get silent LA URL
    */
    if( argv[1] != NULL )
    {
        ChkMem( pszSilentUrl = (DRM_CHAR*)Oem_MemAlloc( DRMCRT_strlen( argv[1] ) + 1 ) );
        ZEROMEM( pszSilentUrl, DRMCRT_strlen( argv[1] ) + 1 );
        MEMCPY( pszSilentUrl, argv[1], DRMCRT_strlen( argv[1] ) );
        cchSilentUrl = DRMCRT_strlen( pszSilentUrl );
    }

    /*
    ** get silent URL size
    */
    if( argv[2] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 10, &dwTempSilentURLSize ) );
        ChkArg( dwTempSilentURLSize <= cchSilentUrl );
        cchSilentUrl = dwTempSilentURLSize;
    }

    /*
    ** get non silent LA URL
    */
    if( argv[3] != NULL )
    {
        ChkMem( pszNonSilentUrl = (DRM_CHAR*)Oem_MemAlloc( DRMCRT_strlen( argv[3] ) + 1 ) );
        ZEROMEM( pszNonSilentUrl, DRMCRT_strlen( argv[3] ) + 1 );
        MEMCPY( pszNonSilentUrl, argv[3], DRMCRT_strlen( argv[3] ) );
        cchNonSilentUrl = DRMCRT_strlen( pszNonSilentUrl );
    }

    /*
    ** get non silent URL size
    */
    if( argv[4] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[4], DRMCRT_strlen( argv[4] ), 10, &dwTempNonSilentURLSize ) );
        ChkArg( dwTempNonSilentURLSize <= cchNonSilentUrl );
        cchNonSilentUrl = dwTempNonSilentURLSize;
    }

    /*
    ** get challenge
    */
    if( argv[5] != NULL )
    {
        cchChallenge = DRMCRT_strlen( argv[5] );
        ChkMem( pszChallenge = ( DRM_CHAR* ) Oem_MemAlloc( cchChallenge + 1 ) );
        ZEROMEM( pszChallenge, cchChallenge + 1 );
        MEMCPY( pszChallenge, argv[5], cchChallenge + 1 );
    }

    /*
    ** get challenge size
    */
    if( argv[6] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[6], DRMCRT_strlen( argv[6] ), 10, &dwTempChlgSize ) );
        ChkArg( dwTempChlgSize <= cchChallenge );
        cchChallenge = dwTempChlgSize;
    }

    /*
    ** keep the old numbers
    */
    dwTempSilentURLSize = cchSilentUrl;
    dwTempNonSilentURLSize = cchNonSilentUrl;
    dwTempChlgSize = cchChallenge;

    /*
    ** get custom data
    */
    if( argc >= 16 && argv[15] != NULL )
    {
        dastrCustomData.cchString = DRMCRT_strlen( argv[15] );
        ChkMem( dastrCustomData.pszString = (DRM_CHAR *) Oem_MemAlloc( dastrCustomData.cchString + 1 ) );
        ZEROMEM( ( DRM_BYTE * )dastrCustomData.pszString, dastrCustomData.cchString + 1 );
        ChkDR( TST_UTL_StringCchCopyNA( dastrCustomData.pszString, dastrCustomData.cchString + 1, argv[15],dastrCustomData.cchString ) );

        if( ( dwOption & 0x4 ) == 0x4 )
        {
            DRM_DWORD idx;
            for( idx = 0; idx < dastrCustomData.cchString; idx++ )
            {
                if( dastrCustomData.pszString[idx] == '\\' )
                {
                    if( dastrCustomData.pszString[idx+1] == 'r' )
                    {
                        dastrCustomData.pszString[idx] = ' ';
                        dastrCustomData.pszString[idx+1] = '\r';
                    }
                    if( dastrCustomData.pszString[idx+1] == 'n' )
                    {
                        dastrCustomData.pszString[idx] = '\n';
                        dastrCustomData.pszString[idx+1] = ' ';
                    }
                    if( dastrCustomData.pszString[idx+1] == 't' )
                    {
                        dastrCustomData.pszString[idx] = '\t';
                        dastrCustomData.pszString[idx+1] = ' ';
                    }
                }
            }
        }
    }

    /*
    ** Generate challenge with specified settings
    */
    drInternal = Drm_LicenseAcq_GenerateChallenge( pLocalManagerContext,
                (const DRM_CONST_STRING **)ppRightsArray,
                *pdwNumRights,
                NULL,
                dastrCustomData.pszString,
                dastrCustomData.cchString,
                pszSilentUrl,
                argv[2]? &cchSilentUrl: NULL,
                pszNonSilentUrl,
                argv[4]? &cchNonSilentUrl: NULL,
                (DRM_BYTE *)pszChallenge,
                argv[6]? &cchChallenge: NULL );

    if( drInternal == DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** get misalignment
        */
        ChkArg( argv[12] != NULL );
        ChkDR( DRMCRT_AtoDWORD( argv[12], DRMCRT_strlen( argv[12] ), 10, &dwMisAlignment ) );

        /*
        ** Check if silent LA URL buffer needs to be resized
        */
        if( cchSilentUrl != dwTempSilentURLSize )
        {
            SAFE_OEM_FREE( pszSilentUrl );
            ChkMem( pszSilentUrl = (DRM_CHAR*)Oem_MemAlloc( ( cchSilentUrl + 1 ) ) );
            ZEROMEM( ( DRM_BYTE * )pszSilentUrl, cchSilentUrl + 1 );
        }

        /*
        ** Check if non silent LA URL buffer needs to be resized
        */
        if( cchNonSilentUrl != dwTempNonSilentURLSize )
        {
            SAFE_OEM_FREE( pszNonSilentUrl );
            ChkMem( pszNonSilentUrl = (DRM_CHAR*)Oem_MemAlloc( ( cchNonSilentUrl + 1 ) ) );
            ZEROMEM( ( DRM_BYTE * )pszNonSilentUrl, cchNonSilentUrl + 1 );
        }

        /*
        ** Check if Challenge buffer needs to be resized
        */
        if( cchChallenge != dwTempChlgSize )
        {
            SAFE_OEM_FREE( pszChallenge );
            ChkMem( pszChallenge = (DRM_CHAR*)Oem_MemAlloc( cchChallenge + 1 + dwMisAlignment ) );
            ZEROMEM( ( DRM_BYTE * )pszChallenge, cchChallenge + 1 + dwMisAlignment );
            pszTempChallenge = pszChallenge + dwMisAlignment;
        }

        /*
        ** Now generate challenge with proper buffer sizes
        */
        ChkDR( Drm_LicenseAcq_GenerateChallenge( pLocalManagerContext,
               (const DRM_CONST_STRING **)ppRightsArray,
               *pdwNumRights,
               NULL,
               dastrCustomData.pszString,
               dastrCustomData.cchString,
               pszSilentUrl,
               argv[2] != NULL || argv[7] != NULL ? &cchSilentUrl : NULL,
               pszNonSilentUrl,
               argv[4]? &cchNonSilentUrl: NULL,
               (DRM_BYTE *)pszTempChallenge,
               &cchChallenge) );
    }
    else
    {
        ChkDR( drInternal );
    }

    if ( dwOption & 2 ) /* store the challenge */
    {
        ChkArg( argc >= 15 );
        MAP_TO_GLOBAL_TEST_VAR_Alloc_EX( pbChallenge, pcbChallenge, argv[14], cchChallenge + 1 );
        *pcbChallenge = cchChallenge;
        MEMCPY( pbChallenge, pszTempChallenge, *pcbChallenge );
        PUT_CHAR( pbChallenge, *pcbChallenge, '\0' );

        Log( "DrmManagerGenerateChallenge", "Challenge: \n %s", pbChallenge );

        /*
        ** Create shared challenge url variable
        */
        if( argv[2] != NULL || argv[7] != NULL )
        {
            cchChallengeURL = cchSilentUrl;
            MAP_TO_GLOBAL_TEST_VAR_Alloc( pszChallengeURL, CHALLENGE_URL_VAR, cchChallengeURL + 1 );
            MEMCPY( pszChallengeURL, pszSilentUrl, cchChallengeURL + 1 );
        }

        Log( "DrmManagerGenerateChallenge", "URL: %s", pszChallengeURL );
    }

    /*
    ** verify silent LA url and size
    */
    if( argv[7] != NULL )
    {
        ChkBOOL( DRMCRT_strlen( argv[7] ) == ( cchSilentUrl - 1 ), DRM_E_TEST_URL_ERROR );
        ChkBOOL( MEMCMP( pszSilentUrl, argv[7], DRMCRT_strlen( argv[7] ) ) == 0, DRM_E_TEST_URL_ERROR );
    }

    if( argv[8] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[8], DRMCRT_strlen( argv[8] ), 10, &dwTempSilentURLSize ) );
        ChkBOOL( dwTempSilentURLSize == ( cchSilentUrl - 1 ), DRM_E_TEST_URL_ERROR );
    }

    /*
    ** verify non silent LA url and size
    */
    if( argv[9] != NULL )
    {
        ChkBOOL( DRMCRT_strlen( argv[9] ) == ( cchNonSilentUrl - 1 ), DRM_E_TEST_URL_ERROR );
        ChkBOOL( MEMCMP( pszNonSilentUrl, argv[9], DRMCRT_strlen( argv[9] ) ) == 0, DRM_E_TEST_URL_ERROR );
    }

    if( argv[10] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[10], DRMCRT_strlen( argv[10] ), 10, &dwTempNonSilentURLSize ) );
        ChkBOOL( dwTempNonSilentURLSize == ( cchNonSilentUrl - 1 ), DRM_E_TEST_URL_ERROR );
    }

    /*
    ** verify challenge
    */
    if( argv[11] != NULL )
    {
        ChkBOOL( strstr( pszChallenge, argv[11] ) != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if(argc >= 17 && argv[16] != NULL)
    {
        if(cchChallenge > 0)
        {
            ChkDR( DRMCRT_AtoDWORD( argv[16], DRMCRT_strlen( argv[16] ), 10, &dwMaxChallengeSize ) );
            if(dwMaxChallengeSize < cchChallenge)
            {
                Log("Trace", "From TestManagerGenerateChallenge: Actual challenge is larger than max. allowed challenge");
                ChkDR(DRM_E_FAIL);
            }
        }
    }

    /*
    ** At this point, the test has passed
    */
    dr = DRM_SUCCESS;

ErrorExit:
    if ( (dwOption&1) )
    {
        SAFE_OEM_FREE( pLocalManagerContext );
    }
    SAFE_OEM_FREE( pszChallenge );
    SAFE_OEM_FREE( pszSilentUrl );
    SAFE_OEM_FREE( pszNonSilentUrl );
    SAFE_OEM_FREE( dastrCustomData.pszString );
    return dr;
}



/*****************************************************************************
** Function:    TestManagerGenerateLicenseAck
**
** Synopsis:    generate a license ack
**
** Arguments:   argv[0]: index in global test data buffer which stores the challenge
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGenerateLicenseAck( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr                = DRM_SUCCESS;
    DRM_BYTE             *pbChallenge       = NULL;
    DRM_DWORD            *pcbChallenge      = NULL;
    DRM_DWORD             cbChallenge       = 0;
    DRM_APP_CONTEXT      *poManagerContext  = NULL;
    DRM_LICENSE_RESPONSE *poLicenseResponse = NULL;

    ChkArg( argc > 0 );
    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR( poLicenseResponse, LICENSE_RESPONSE_VAR );

    dr = Drm_LicenseAcq_GenerateAck(poManagerContext, poLicenseResponse, NULL, &cbChallenge );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc_EX( pbChallenge, pcbChallenge, argv[0], cbChallenge );
        ChkDR( Drm_LicenseAcq_GenerateAck( poManagerContext, poLicenseResponse, pbChallenge, pcbChallenge ) );
    }
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestProcessAdditionalData
**
** Synopsis:    Gets Additional Data From Response and Stores it in the specified buffer
**
** Arguments:
**              argv[0] -- Index of the response data in the global test data array
**              argv[1] -- Index in the global array for storing the data
**              argv[2] -- Data Type to get "CUSTOM", "REDIRECT_URL", "ACCOUNT_ID", or "SERVICE_ID"
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestProcessAdditionalData(long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT      dr                = DRM_SUCCESS;
    DRM_BYTE        *pbResponse       = NULL;
    DRM_DWORD        cbResponse       = 0;
    DRM_DWORD       *pcbResponse      = &cbResponse;
    DRM_CHAR        *pchData          = NULL;
    DRM_DWORD        cchData          = 0;
    DRM_DWORD       *pcchData         = &cchData;
    DRM_DWORD        dwType           = 0;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    ChkArg( argc >= 3 );
    GET_SHARED_APP_CONTEXT( poManagerContext );

    MAP_TO_GLOBAL_TEST_VAR_EX(pbResponse, pcbResponse, argv[0]);
    GET_ADDITIONAL_DATA_TYPE(2, dwType);

    dr = Drm_GetAdditionalResponseData( poManagerContext,
                                        pbResponse,
                                       *pcbResponse,
                                        dwType,
                                        NULL,
                                       &cchData );
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        if ( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_TEST_UNEXPECTED_OUTPUT;
        }
        ChkDR( dr );
    }

    MAP_TO_GLOBAL_TEST_VAR_Alloc_EX(pchData, pcchData, argv[1], cchData + 1 );

    ChkDR(Drm_GetAdditionalResponseData( poManagerContext,
                                        pbResponse,
                                       *pcbResponse,
                                        dwType,
                                        pchData,
                                        pcchData ) );

    Log("GetAdditionalData","Get Additional Data -- %s: %s", argv[2], pchData );

ErrorExit:
    return dr;
}

/*******************************************************************************************
** Function:    VerifySymmetricOptimizations
**
** Synopsis:
**
** Arguments:
**              argv[0] -- KID to Check
**              argv[1] -- LID to Check
**              argv[2] -- (optional, XML licenses only) - Expected optimization status, from DRM_SYMMOPT_STATUS
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
*******************************************************************************************/
DRM_RESULT DRM_CALL VerifySymmetricOptimizations( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr           = DRM_SUCCESS;
    if( DRM_SYMOPT_IsSymOptSupported() )
    {
        DRM_APP_CONTEXT_INTERNAL *pAppContext  = NULL;
        DRM_LICSTORE_CONTEXT     *poContextXML = NULL;
        DRM_LICSTORE_CONTEXT     *poContextXMR = NULL;
        DRM_KID                   oKid         = { 0 };
        DRM_DWORD                 cbKid        = DRM_ID_SIZE;
        DRM_LID                   oLid         = { 0 };
        DRM_BYTE                 *pbLicense    = NULL;
        DRM_DWORD                 cbLicense    = 0;
        DRM_BOOL                  fXMRLicense  = FALSE;
        DRM_WCHAR                 rgwchAttr[256] = {0};
        DRM_CONST_STRING          dstrKidValue   = EMPTY_DRM_STRING;
        DRM_CONST_STRING          dstrLidValue   = EMPTY_DRM_STRING;

        ChkArg( argc >= 2 );
        ChkArg( argv[0] != NULL );
        ChkArg( argv[1] != NULL );
        GET_SHARED_APP_CONTEXT( pAppContext );

        ChkBOOL( pAppContext->fLicStoreOpenXML && pAppContext->fLicStoreOpenXMR,
                 DRM_E_TEST_LICENSESTORE_NOT_OPEN );
        poContextXML = &(pAppContext->oLicStoreContextXML );
        poContextXMR = &(pAppContext->oLicStoreContextXMR );

        ChkArg( DRMCRT_strlen( argv[0] ) + 1  <= NO_OF( rgwchAttr ) );

        /*
        ** base64 decode the KID argument
        */
        TST_UTL_mbstowcs( rgwchAttr, argv[0], DRMCRT_strlen( argv[0] ) + 1 );
        dstrKidValue.pwszString = rgwchAttr;
        dstrKidValue.cchString = DRMCRT_strlen( argv[0] );
        ZEROMEM( &oKid, SIZEOF( DRM_KID ) );
        cbKid = SIZEOF( DRM_KID );
        ChkDR( DRM_B64_DecodeW( &dstrKidValue, &cbKid, oKid.rgb, 0 ) );
        ZEROMEM( rgwchAttr, SIZEOF( rgwchAttr ) );

        /*
        ** Convert the LID from a string to a GUID
        */
        TST_UTL_mbstowcs( rgwchAttr, argv[1],  DRMCRT_strlen( argv[1] ) + 1 );
        dstrLidValue.pwszString = rgwchAttr;
        dstrLidValue.cchString= ( DRM_DWORD )DRMCRT_strlen( argv[1] );
        ZEROMEM( &oLid, SIZEOF( DRM_LID ) );
        ChkDR( DRM_UTL_StringToGuid( &dstrLidValue, ( DRM_GUID* )oLid.rgb ) );

        /*
        ** Search for the license, try the XML store first
        */
        dr = DRM_LST_GetLicense( poContextXML, &oKid, &oLid, NULL, NULL, &cbLicense );
        if( dr == DRM_E_BUFFERTOOSMALL )
        {
            ChkMem( pbLicense = ( DRM_BYTE * ) Oem_MemAlloc( cbLicense ) );
            ZEROMEM( pbLicense, cbLicense );
            ChkDR( DRM_LST_GetLicense( poContextXML, &oKid, &oLid, NULL, pbLicense, &cbLicense ) );
        }
        else if ( dr == DRM_E_DSTSLOTNOTFOUND )
        {

            /*
            ** Now try the XMR store
            */
            dr = DRM_LST_GetLicense( poContextXMR, &oKid, &oLid, NULL, NULL, &cbLicense );
            if( dr == DRM_E_BUFFERTOOSMALL )
            {
                ChkMem( pbLicense = ( DRM_BYTE * ) Oem_MemAlloc( cbLicense ) );
                ZEROMEM( pbLicense, cbLicense );
                ChkDR( DRM_LST_GetLicense( poContextXMR, &oKid, &oLid, NULL, pbLicense, &cbLicense ) );
                fXMRLicense = TRUE;
            }
            else
            {
                ChkDR( dr );
            }
        }
        else
        {
            ChkDR( dr );
        }

        if ( fXMRLicense )
        {
#define __cbMaxStack 4096
            DRM_STACK_ALLOCATOR_CONTEXT oStack     = { 0 };
            DRM_BYTE        rgbStack[__cbMaxStack] = { 0 };
            DRM_XMR_LICENSE oXMRLicense = { 0 };

            oStack.nStackTop = 0;
            oStack.pbStack   = rgbStack;
            oStack.cbStack   = __cbMaxStack;
            ZEROMEM( rgbStack, __cbMaxStack );


            ChkDR( DRM_XMR_UnpackLicense( pbLicense, cbLicense, &oStack, &oXMRLicense ) );
            ChkBOOL( oXMRLicense.containerOuter.containerKeys.OptimizedContentKey.fValid,
                     DRM_E_TEST_UNEXPECTED_OUTPUT );
            ChkBOOL( oXMRLicense.containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType
                ==  XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB, DRM_E_TEST_UNEXPECTED_OUTPUT );

#undef __cbMaxStack

        }
        else
        {
            DRM_CONST_STRING   dstrLicense     = EMPTY_DRM_STRING;
            DRM_SYMMOPT_STATUS eStatus         = DRM_SYMMOPT_STATUS_NONE;
            DRM_SYMMOPT_STATUS eExpectedStatus = DRM_SYMMOPT_STATUS_COMPLETE;       /* Default to complete symm opt expected */

            if ( argc >= 3 )
            {
                ChkArg( argv[2] != NULL );
                eExpectedStatus = ( DRM_SYMMOPT_STATUS )strtoul( argv[2], NULL, 0 );
            }

            dstrLicense.pwszString = ( DRM_WCHAR * )pbLicense;
            dstrLicense.cchString  = cbLicense/SIZEOF( DRM_WCHAR );

            ChkDR( DRM_SYMOPT_LIC_CheckSymmOptStatus( &dstrLicense,
                                                      &eStatus,
                                                      NULL,
                                                      NULL,
                                                      NULL ) );


            ChkBOOL( eStatus == eExpectedStatus, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

ErrorExit:
    return dr;
}

/*******************************************************************************************
** Function:    TestManagerProcessResponse
**
** Synopsis:
**
** Arguments:
**              argv[0] --
**              argv[1] --  size of response to allocate if response overwrite flag is on
**              argv[2] --  data size to use if data size overwrite flag is on
**              argv[3] --  Flags
**                          0x1  (1)   Context is from the input
**                          0x10 (16)  Use argv[2] as data size
**                          0x20 (32)  Overwrite the response based on argv[1]
**                          0x40 (64)  Use _LicenseProcessResponseCallback for callback, else use NULL
**                          0x80 (128) Use two-pass call
**              argv[4] --  holds the index in the global test data buffer if it is to be used
**              argv[5] --  Optional. Expected SOAP error code from the server
**              argv[6] --  Optional. Expected number of licenses in the response
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
*******************************************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT      *pLocalManagerContext = NULL;
    DRM_LONG              i                    = 0;
    DRM_LONG              iOption              = 0;
    DRM_BYTE             *pbResponse           = NULL;
    DRM_DWORD             cbResponse           = 0;
    DRM_DWORD            *pcbResponse          = &cbResponse;
    DRM_LICENSE_RESPONSE *poLicenseResponse    = NULL;

    MAP_TO_GLOBAL_TEST_VAR( poLicenseResponse, LICENSE_RESPONSE_VAR );
    ZEROMEM( poLicenseResponse, SIZEOF( DRM_LICENSE_RESPONSE ) );

    for( i = 0; i < argc; i++ )
    {
        Log("Trace", "\t\tFrom TestManagerProcessResponse: argv[%d]=%s", i, argv[i]);
    }
    /*
    ** at least 4 arguments. The argv[3] one is used to decide whether we need to overwrite the context
    */
    if( argc < 4 )
    {
        Log("Trace", "\t\tFrom TestManagerProcessResponse: error in number of arguments.");
        ChkDR(DRM_E_TEST_INVALIDARG);
    }

    if( !IsDigitalOnlyString( argv[2] )
     || !IsDigitalOnlyString( argv[3] )
     || (  argc > 6 && argv[6] != NULL && !IsDigitalOnlyString( argv[6] ) ) )
    {
        Log("Trace", "\t\tFrom TestManagerProcessResponse: error in the argument.");
        ChkDR(DRM_E_TEST_INVALIDARG);
    }

    iOption=(DRM_LONG)atol(argv[3]);

    /*
    ** manager context
    */
    if( iOption & 1 ) /*Context is from the input*/
    {
        ChkMem( pLocalManagerContext = (DRM_APP_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
        MEMSET( pLocalManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) );
    }
    else /* Use shared context */
    {
        GET_SHARED_APP_CONTEXT( pLocalManagerContext );
    }

    /*
    ** overwrite the response
    */
    if( iOption & 32 )
    {
        SAFE_OEM_FREE( pbResponse );
        if( argv[1] )
        {
            ChkMem( pbResponse=(DRM_BYTE*)Oem_MemAlloc( DRMCRT_strlen( argv[1] ) + 1) );
            ZEROMEM( pbResponse, DRMCRT_strlen( argv[1] ) + 1 );
            MEMCPY( pbResponse, argv[1], DRMCRT_strlen( argv[1] ) + 1 );
            *pcbResponse=DRMCRT_strlen( argv[1] );
        }
        else
        {

            pbResponse = NULL;
            *pcbResponse = 0;
        }
    }
    else
    {
        /*
        ** argv[4] holds the index in the global test data buffer if it is to be used
        */
        if( argc > 4 && argv[4] != NULL && DRMCRT_strlen( argv[4] ) != 0 )
        {
            MAP_TO_GLOBAL_TEST_VAR_EX( pbResponse, pcbResponse, argv[4] );
        }
        else if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_VAR, ( DRM_VOID ** ) &pbResponse, &pcbResponse ) ) )
        {
            pbResponse = NULL;
            *pcbResponse = 0;
        }
    }
    /*
    ** overwrite data size
    */
    if( iOption & 16 )
    {
        *pcbResponse=(DRM_DWORD)atol( argv[2] );
    }

    ChkDR( DRM_Init_ProfileStack() );
    dr =   Drm_LicenseAcq_ProcessResponse( pLocalManagerContext,
                                           DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                           ( ( iOption & 64 ) == 64 ) ? (pfnLicenseResponseCallback)&_LicenseProcessResponseCallback : NULL,
                                           NULL,
                                           pbResponse,
                                           *pcbResponse,
                                           poLicenseResponse );

    if( DRM_FAILED( dr ) && dr != DRM_E_LICACQ_TOO_MANY_LICENSES )
    {
        /*
        ** Verify that poLicenseResponse->m_cAcks is zero in non-"too many licenses" failure cases
        */
        ChkBOOL( 0 == poLicenseResponse->m_cAcks, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    if( argc > 6 && argv[6] != NULL )
    {
        /*
        ** Verify that poLicenseResponse->m_cAcks is the test-specified value
        */
        if( DRM_SUCCEEDED( dr ) || dr == DRM_E_LICACQ_TOO_MANY_LICENSES )
        {
            ChkBOOL( (DRM_DWORD)atol( argv[6] ) == poLicenseResponse->m_cAcks, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

    if( iOption & 128 )
    {
        /*
        ** Two-pass call requested.  Verify the error code...
        */
        ChkBOOL( dr == DRM_E_LICACQ_TOO_MANY_LICENSES, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dr = DRM_SUCCESS;

        /*
        ** ...populate the license response variables with a large enough buffer...
        */
        ChkBOOL( poLicenseResponse != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        poLicenseResponse->m_cMaxAcks = poLicenseResponse->m_cAcks;
        ChkMem( poLicenseResponse->m_pAcks = (DRM_LICENSE_ACK*)Oem_MemAlloc( poLicenseResponse->m_cMaxAcks * SIZEOF( DRM_LICENSE_ACK ) ) );

        /*
        ** ...and recall the method.
        */
        dr =   Drm_LicenseAcq_ProcessResponse( pLocalManagerContext,
                                               DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                               ( ( iOption & 64 ) == 64 ) ? (pfnLicenseResponseCallback)&_LicenseProcessResponseCallback : NULL,
                                               NULL,
                                               pbResponse,
                                               *pcbResponse,
                                               poLicenseResponse );
    }

    /*
    ** ChkDR the actual return value from Drm_LicenseAcq_ProcessResponse
    */
    ChkDR( dr );

    if( argc > 5 && argv[5] != NULL )
    {
        DRM_DWORD dwResult = 0;

        ChkDR( DRMCRT_AtoDWORD( argv[5], (DRM_DWORD)strlen( argv[5] ), 0, (DRM_DWORD*)&dwResult ) );
        ChkBOOL( (DRM_RESULT)dwResult == poLicenseResponse->m_dwResult, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    if ( ( iOption & 32 ) != 0 && ( argv[1] != NULL ) )
    {
        SAFE_OEM_FREE( pbResponse );
    }
    if ( iOption & 1 )
    {
        SAFE_OEM_FREE( pLocalManagerContext );
    }
    return dr;
}

/*********************************************************************
**
** Function: TestManagerProcessResponse_ModelRevoked
**
** Synopsis: Call TestManagerProcessResponse() and confirm it returns with 0X8004C065 (DRM_E_DEVCERT_REVOKED).
**           Only appliable to WM7 test now
**
** Arguments:
**              argv[0] --
**              argv[1] --  size of response to allocate if response overwrite flag is on
**              argv[2] --  data size to use is data size overwrite flag is on
**              argv[3] --  Flags
**              argv[4] --  holds the index in the global test data buffer if it is to be used
**              argv[5] --  Optional. Expected SOAP error code from the server
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
*******************************************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessResponse_ModelRevoked( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( DRM_MODELREVOCATION_IsModelRevocationSupported() )
    {
        dr = TestManagerProcessResponse(argc, argv);
        ChkBOOL( dr == DRM_E_DEVCERT_REVOKED, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dr = DRM_SUCCESS;
    }
ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestManagerProcessResponse_CheckResults
**
** Synopsis: Compares the results of TestManagerProcessResponse to
**           expected values.
**
** Arguments:
**           argv[0] : KID of the license. (B64 encoded)
**           argv[1] : LID of the license.
**           argv[3] : (optional) is the expected dwResult.
**           argv[4] : (optional) is the expected dwFlags value from the ACK
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessResponse_CheckResults( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iLicense = 0;

    DRM_CHAR         rgchKID[CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) )] = { 0 };
    DRM_DWORD        cchKID        = 0;
    DRM_CONST_STRING dstrLID       = EMPTY_DRM_STRING;
    DRM_LID         *poLid         = NULL;
    DRM_DWORD        dwResult      = 0;
    DRM_BOOL         fMatchKID     = FALSE;
    DRM_BOOL         fMatchLID     = FALSE;
    DRM_BOOL         fMatchLicense = FALSE;
    DRM_DWORD        dwFlags       = 0;
    DRM_CHAR        *pstrLicenseID = NULL;

    DRM_LICENSE_RESPONSE *poLicenseResponse = NULL;
    MAP_TO_GLOBAL_TEST_VAR( poLicenseResponse, LICENSE_RESPONSE_VAR );

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    for( iLicense = 0; iLicense < poLicenseResponse->m_cAcks; iLicense++ )
    {
        /*
        ** Look for a stored license with the correct KID
        */
        cchKID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&poLicenseResponse->m_rgoAcks[iLicense].m_oKID,
                                SIZEOF( DRM_GUID ),
                                rgchKID,
                                &cchKID,
                                0 ) );
        fMatchKID = ( MEMCMP( argv[0], rgchKID, CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ) == 0 );

        if ( fMatchKID )
        {
            /*
            ** After the correct KID is found, check for a matching LID
            */

            /*
            ** Convert the LID from a string to a GUID
            */


            if( ( strncmp( argv[ 1 ], "NONCE", 5 )  == 0) )
            {
                MAP_TO_GLOBAL_TEST_VAR( poLid, LICENSE_ID_VAR );
                DRM_FAILED( DRM_TST_GetTestVar( LICENSE_NONCE_VAR, ( DRM_VOID ** ) &pstrLicenseID, NULL ));
                ChkDR( MakeDRMString( &dstrLID, pstrLicenseID ) );
                ChkDR( DRM_UTL_DecodeKID( &dstrLID, (DRM_KID*)poLid ) );
            }
            else if ( ( strncmp( argv[ 1 ], "SHARED", 6 )  == 0) )
            {
                DRM_FAILED( DRM_TST_GetTestVar( LICENSE_ID_VAR, ( DRM_VOID ** ) &poLid, NULL ));
            }
            else
            {
                ChkDR( MakeDRMString( &dstrLID , argv[1] ) );
                MAP_TO_GLOBAL_TEST_VAR( poLid, LICENSE_ID_VAR );
                ZEROMEM( poLid, SIZEOF( DRM_LID ) );
                ChkDR( DRM_UTL_StringToGuid( &dstrLID, ( DRM_GUID * )poLid ) );
            }

            fMatchLID = ( MEMCMP( poLid, &poLicenseResponse->m_rgoAcks[iLicense].m_oLID, SIZEOF( DRM_GUID ) ) == 0 );

            if ( fMatchLID )
            {
                fMatchLicense = TRUE;

                /*
                ** After the correct license is found, check against the expected storage result
                */
                if ( argc > 2 && argv[2] != NULL )
                {
                    dwResult = ( DRM_DWORD )strtoul( argv[2], NULL, 0 );
                    ChkBOOL( dwResult == ( DRM_DWORD )( poLicenseResponse->m_rgoAcks[iLicense].m_dwResult ), DRM_E_TEST_UNEXPECTED_OUTPUT );
                }
                /*
                ** Check the dwFlags value against expected
                */
                if ( argc > 3 && argv[3] != NULL )
                {
                    dwFlags = ( DRM_DWORD )strtoul( argv[3], NULL, 0 );
                    ChkBOOL( dwFlags == poLicenseResponse->m_rgoAcks[iLicense].m_dwFlags, DRM_E_TEST_UNEXPECTED_OUTPUT );
                }
            }
        }
    }

    if( !fMatchLicense )
    {
        ChkDR( DRM_E_LICENSENOTFOUND );
    }

ErrorExit:
    FREEDRMSTRING( dstrLID );

    return dr;
}

/*******************************************************************************************
** Function:    TestManagerProcessLicenseAckResponse
**
** Synopsis:
**
** Arguments:
**              argv[0] --  index to the response buffer in the global data array
**              argv[1] --  Optional. Expected SOAP error code in the response from the server
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
*******************************************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessLicenseAckResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_RESULT       dr1              = DRM_SUCCESS;
    DRM_BYTE        *pbResponse       = NULL;
    DRM_DWORD       *pcbResponse      = NULL;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    ChkArg( argc >= 1 );

    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR_EX(pbResponse, pcbResponse, argv[0] );

    ChkDR( Drm_LicenseAcq_ProcessAckResponse( poManagerContext,
                                              pbResponse,
                                              *pcbResponse,
                                              &dr1 ) );

    if( argc > 1 && argv[1] != NULL )
    {
        ChkBOOL( strtol( argv[1], NULL, 0 ) == dr1, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
