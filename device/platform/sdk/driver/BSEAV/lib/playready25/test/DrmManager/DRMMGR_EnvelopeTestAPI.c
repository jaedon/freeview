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
** Function:    TestManagerCreateDecryptContext
**
** Synopsis:    Creates a decrypt context
**
**
** Arguments:   argv[0]             -- Base64 encoded content key
**              argv[1]             -- Optional encryption type
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerCreateDecryptContext( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr                                 = DRM_SUCCESS;
    DRM_SUPPORTED_CIPHERS eCipher                            = eDRM_AES_COUNTER_CIPHER;
    DRM_BYTE              rgbContentKey[DRMCIPHERKEYLEN_MAX] = {0};
    DRM_DWORD             cbContentKey                       = DRMCIPHERKEYLEN_MAX;
    DRM_SUBSTRING         dasstr                             = EMPTY_DRM_SUBSTRING;
    DRM_DECRYPT_CONTEXT  *poDecryptContext                   = NULL;

    ChkArg( argc >= 1 );

    /*
    ** Get shared test decrypt context
    */
    MAP_TO_GLOBAL_TEST_VAR( poDecryptContext, DECRYPT_CONTEXT_VAR );

    dasstr.m_ich = 0;
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[0] ), &dasstr.m_cch ) );
    ChkDR( DRM_B64_DecodeA( argv[0], &dasstr, &cbContentKey, rgbContentKey, 0 ) );

    if( argc >= 2 )
    {
        eCipher = (DRM_SUPPORTED_CIPHERS)atol( argv[1] );
    }

    ChkDR( DRM_CPHR_Init( (DRM_CIPHER_CONTEXT*)poDecryptContext, eCipher, cbContentKey, rgbContentKey ) );

ErrorExit:
    return dr;
}

/*
** Callback "destructor" for DRM_ENVELOPED_FILE_CONTEXT test variables:
** need to close the file to release it for the next test.
*/
DRM_VOID DRM_CALL _CleanupEnvelopedFileCtx( __in const DRM_VOID *f_pvTestVar )
{
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext = NULL;
    const DRM_TEST_VARIABLE *f_poTestVar = (const DRM_TEST_VARIABLE*)f_pvTestVar;
    if ( f_pvTestVar == NULL )
        return;

    DRM_TST_GetTestVar( f_poTestVar->szName, (DRM_VOID**)&pEnvContext, NULL );

    if ( pEnvContext != NULL
        && pEnvContext->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE
        && pEnvContext->pEnvFileHandle != NULL )
    {
        Oem_File_Close( pEnvContext->pEnvFileHandle );
        pEnvContext->pEnvFileHandle = OEM_INVALID_HANDLE_VALUE;
    }
}


/*****************************************************************************
** Function:    Test Drm_Envelope_Open
**
** Synopsis:    Tests the Open a file within the envelope archive
**
**
**
** Arguments:   argv[0]           -- index in the global array for the envelope context, NULL to use a NULL Envelope Context.
**              argv[1]           -- The envelope file name  to open
**              argv[2]           -- NULL flag to test for testing a NULL Manager Context
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerOpenEnvelope( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                  dr               = DRM_SUCCESS;
    DRM_CONST_STRING            dstrFilename     = EMPTY_DRM_STRING;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext      = NULL;
    DRM_APP_CONTEXT            *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc    >= 2  );

    MAP_TO_GLOBAL_TEST_VAR_Alloc_WithCallback( pEnvContext,
                                               argv[0],
                                               SIZEOF(DRM_ENVELOPED_FILE_CONTEXT),
                                               _CleanupEnvelopedFileCtx );

    if( NULL != argv[1] )
    {
        ChkDR( TST_UTL_MakePathString( NULL, argv[1], &dstrFilename ) );
    }

    if (argc > 2 && argv[2] == NULL)
    {
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_PASSING_FUNCTION_UNEXPECTED_NULL, "Test code is potentially passing invalid parameters intentionally." );
       ChkDR( Drm_Envelope_Open( NULL,
                                 NULL,
                                 dstrFilename.pwszString,
                                 pEnvContext ) );
PREFAST_POP
    }
    else
    {
       ChkDR( Drm_Envelope_Open( poManagerContext,
                                 NULL,
                                 dstrFilename.pwszString,
                                 pEnvContext ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrFilename );
    return dr;
}

/*****************************************************************************
** Function:    Test Drm_Envelope_DuplicateFileContext
**
** Synopsis:    Tests the DuplicateFileContext function. Will re-open the current envelope file handle.
**
**
**
** Arguments:   argv[0]           -- index in the global array for the envelope context, NULL to use a
**                                   NULL Envelope Context, INVALID to use all 0's
**              argv[1]           -- index in the global array for the new envelope context, NULL to use a NULL Envelope Context.
**              argv[2]           -- The envelope file name  to open
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerReOpenEnvelope( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr           = DRM_SUCCESS;
    DRM_CONST_STRING    dstrFilename = EMPTY_DRM_STRING;
    DRM_ENVELOPED_FILE_CONTEXT *pOldEnvContext = NULL;
    DRM_ENVELOPED_FILE_CONTEXT *pNewEnvContext = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited    = {0};

    ChkArg( argc    >= 3  );

    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pOldEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pOldEnvContext, argv[0] );
    }
    MAP_TO_GLOBAL_TEST_VAR_Alloc_WithCallback( pNewEnvContext,
                                               argv[1],
                                               SIZEOF(DRM_ENVELOPED_FILE_CONTEXT),
                                               _CleanupEnvelopedFileCtx );


    if( NULL != argv[2] )
    {
        ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrFilename ) );
    }

    ChkDR( Drm_Envelope_DuplicateFileContext( NULL,
                                              dstrFilename.pwszString,
                                              pOldEnvContext,
                                              pNewEnvContext ) );

ErrorExit:
    FREEDRMSTRING( dstrFilename );
    return dr;
}

/*****************************************************************************
** Function:    Test Drm_Envelope_Close API
**
** Synopsis:    Closes the enveloped file and all associated resources
**
**
** Arguments:   argv[0]        -- index in the global array for the envelope context, NULL to use a
**                                NULL Envelope Context, INVALID to use all 0's
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/

DRM_RESULT DRM_CALL TestManagerCloseEnvelope( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited = {0};

    ChkArg( argc    >= 1  );
    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pEnvContext, argv[0] );
    }

    ChkDR( Drm_Envelope_Close( pEnvContext ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    Tests Drm_Envelope_InitializeRead
**
** Synopsis:    Tests the specifies the decrypt context for the given enveloped file
**
**
** Arguments:   argv[0]     -- index in the global array for the envelope context, NULL to use a
**                             NULL Envelope Context, INVALID to use all 0's
**              argv[1]     -- if NULL, use a NULL decrypt Context
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerInitializeEnvelopeRead( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                  dr               = DRM_SUCCESS;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext      = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited      = {0};
    DRM_DECRYPT_CONTEXT        *poDecryptContext = NULL;

    ChkArg( argc    >= 1  );

    /*
    ** Get shared test decrypt context
    */
    MAP_TO_GLOBAL_TEST_VAR( poDecryptContext, DECRYPT_CONTEXT_VAR );

    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pEnvContext, argv[0] );
    }

    ChkDR( Drm_Envelope_InitializeRead( pEnvContext,
                                        ( argc > 1 && argv[1] == NULL ) ? NULL : poDecryptContext ) );

ErrorExit:
    return dr;
}
/*****************************************************************************
** Function:    Tests Drm_Envelope_GetSize API
**
** Synopsis:    Verifies the return of the size of the enveloped file
**
** Arguments:   argv[0]     -- index in the global array for the envelope context, NULL to use a
**                             NULL Envelope Context, INVALID to use all 0's
**              argv[1]     -- Expected file size known value
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetEnvelopeSize( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_DWORD        cbSize = 0;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited = {0};

    ChkArg( argc    >= 2  );
    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pEnvContext, argv[0] );
    }


    ChkDR( Drm_Envelope_GetSize( pEnvContext, &cbSize ) );
    ChkBOOL( cbSize == (DRM_DWORD)atoi( argv[1] ), DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}
/*****************************************************************************
** Function:    Test the Drm_Envelope_Read API
**
** Synopsis:    Verify the API reads data from the enveloped file
**
** Arguments:   argv[0]           -- index in the global array for the envelope context, NULL to use
**                                   a NULL Envelope Context, INVALID to use all 0's
**              argv[1]           -- Clear file name to verify content read
**              argv[2]           -- Byte position to start read in clear content file
**              argv[3]           -- Number of bytes to read
**              argv[4]           -- Option Flag (can be combined)
**                                   1 -- NULL byte Pointer
**                                   2 -- NULL count read pointer
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
**              DRM_E_BUFFERTOOSMALL if *f_pcbBuffer smaller than required buffer size
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerReadEnvelope( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_BYTE        *pbIntended = NULL;
    DRM_BYTE        *pbRead = NULL;
    DRM_DWORD        cbRead = 0;
    DRM_DWORD       *pcbRead  = NULL;
    DRM_DWORD        cbToRead = 0;
    int              cbOffset = 0;
    DRM_DWORD        dwOption = 0;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited = {0};

    ChkArg( argc    >= 2  );
    ChkArg( argv[1] != NULL );

    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pEnvContext, argv[0] );
    }

    ChkArg( LoadTestFile( "files", argv[1], &pbIntended, &cbToRead )
         || LoadTestFile( NULL, argv[1], &pbIntended, &cbToRead ) );
    /*
    ** Check to read only first n bytes from file.
    */
    if (argc >= 3 && argv[2] != NULL && ((DRM_DWORD)atoi(argv[2]) < cbToRead))
    {
       cbOffset = atoi(argv[2]);
    }
    if ( argc >= 4 && argv[3] != NULL )
    {
       /*
       ** Read X bytes from start position
       */
       cbToRead = (DRM_DWORD)atoi(argv[3]);
    }
    else
    {
       /*
       ** Read bytes from start position to end of file.
       */
       cbToRead = cbToRead - cbOffset;
    }

    if ( argc >= 5 && argv[4] != NULL )
    {
        dwOption = (DRM_DWORD) strtoul( argv[4], NULL, 0 );
    }

    if ( ( dwOption & 1 ) == 0 )
    {
        ChkMem( pbRead = (DRM_BYTE *)Oem_MemAlloc( max(1, cbToRead) ) );
        ZEROMEM( pbRead, cbToRead );
    }
    if ( ( dwOption & 2 ) == 0 )
    {
        pcbRead = &cbRead;
    }
    ChkDR( Drm_Envelope_Read( pEnvContext, pbRead, cbToRead, pcbRead ) );

    ChkBOOL( cbRead <= cbToRead, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( cbRead == cbToRead, DRM_E_TEST_INCOMPLETE );

    ChkBOOL( DRM_BYT_CompareBytes(pbRead, 0, pbIntended, cbOffset, cbRead) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pbRead );
    SAFE_OEM_FREE( pbIntended );
    return dr;
}
/*****************************************************************************
** Function:    Test Drm_Envelope_Seek
** Synopsis:    Test the Seek in the enveloped file
** Arguments:   argv[0]        -- index in the global array for the envelope context, NULL to use a
**                                NULL Envelope Context, INVALID to use all 0's
**              argv[1]        -- Specifies what the distance to move is relative to
**              argv[2]        -- Specifies the number of bytes to seek
**              argv[3]        -- Specifies if it is a Random seek
**              argv[4]        -- Specifies to test FileContext as a NULL
**
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSeekEnvelope( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_DWORD       *pbRead = NULL;
    DRM_DWORD       cbSize = 0;
    DRM_LONG        newLocation = 0;
    int             iFromLocation = 0;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited = {0};

    ChkArg( argc    >= 3  );

    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pEnvContext, argv[0] );
    }

    /*
    ** Check if random seek position
    */
    if (argc >= 4 && 0 == strncmp( argv[3], "Random", 6 ) )
    {
       ChkDR( Drm_Envelope_GetSize( pEnvContext, &cbSize ) );
       Oem_Random_GetBytes( NULL, ( DRM_BYTE* ) &newLocation, SIZEOF( DRM_DWORD ) );
       if (newLocation > ( DRM_LONG ) cbSize)
       {
           newLocation = (DRM_LONG)( newLocation % cbSize );  /* make sure random seek is less than file size */
       }
    }
    else
    {
        newLocation = ( DRM_LONG ) atol( argv[2] );
    }
    iFromLocation = atoi( argv[1] );
    switch ( iFromLocation )
    {
        case 0:
            Log("Trace", "\t\tFrom TestManagerSeekEnvelope: Seek %d bytes from beginning of file.", newLocation);
            break;
        case 1:
            Log("Trace", "\t\tFrom TestManagerSeekEnvelope: Seek %d bytes from current position in file.", newLocation);
            break;
        case 2:
            Log("Trace", "\t\tFrom TestManagerSeekEnvelope: Seek %d bytes from end of file.", newLocation);
            break;
        default:
            ChkArg( iFromLocation >= 0 && iFromLocation <= 2 );
            break;
    }
    ChkDR( Drm_Envelope_Seek( ( argc >=5 && argv[4] == NULL ) ? NULL: pEnvContext,
                              newLocation,
                              (DRM_DWORD)iFromLocation,
                              pbRead ) );

ErrorExit:
    SAFE_OEM_FREE( pbRead );
    return dr;
}

/*****************************************************************************
** Function:    Test the Drm_Envelope_WritePlayReadyObject API
**
** Synopsis:    Verify the API writes the PlayReady Object back to the envelope file.
**
** Arguments:   argv[0]           -- index in the global array for the envelope context, NULL to use
**                                   a NULL Envelope Context, INVALID to use all 0's
**              argv[1]           -- The envelope file name to reopen for write.
**              argv[2]           -- NULL flag to test for testing a NULL Manager Context
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid
**              Or any return code as defined in drmresults.h
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerEnvelopeWritePlayReadyObject( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                  dr               = DRM_SUCCESS;
    DRM_CONST_STRING            dstrFilename     = EMPTY_DRM_STRING;
    DRM_ENVELOPED_FILE_CONTEXT *pEnvContext      = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited      = {0};
    DRM_APP_CONTEXT            *poManagerContext = NULL;

    ChkArg( argc >= 2 );

    /*
    ** Set up the envelope context pointer: NULL, INVALID, or from index into the global array
    */
    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        pEnvContext = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pEnvContext, argv[0] );
    }

    /*
    ** Set up the path name to the envelope file
    */
    if( argv[1] != NULL )
    {
        ChkDR( TST_UTL_MakePathString( NULL, argv[1], &dstrFilename ) );
    }

    /*
    ** Get the app context if non-NULL
    */
    if ( argc < 3 || argv[2] != NULL )
    {
        GET_SHARED_APP_CONTEXT( poManagerContext );
    }

    ChkDR( Drm_Envelope_WritePlayReadyObject( poManagerContext,
                                              NULL,
                                              dstrFilename.pwszString,
                                              pEnvContext ) );

ErrorExit:
    FREEDRMSTRING( dstrFilename );
    return dr;
}


/*
** Test getting the original filename from an envelope
**
** Arguments:
**              argv[0] -- index in the global array for the envelope context, NULL to use a
**                         NULL Envelope Context, INVALID to use all 0's
**              argv[1] -- original filename for comparison
**              argv[2] -- option flag, values can be combined. "1"  must be set to use any options.
**                         0 -- normal
**                         1 -- modify values
**                         2 -- NULL string pointer
**                         4 -- 0 size
**                         8 -- NULL size pointer ( 4 and 8 are mutually exclusive, 8 takes precedence )
*/
DRM_RESULT DRM_CALL TestManagerGetEnvelopeOriginalFilename( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchFilename = 0;
    DRM_DWORD *pcchFilename = &cchFilename;
    DRM_WCHAR *pwszFilename = NULL;
    DRM_CONST_STRING dstrExpected = EMPTY_DRM_STRING;
    DRM_ENVELOPED_FILE_CONTEXT *poEnvFile = NULL;
    DRM_ENVELOPED_FILE_CONTEXT  EnvUninited = {0};
    DRM_DWORD iOption = 0;

    if ( argv[0] != NULL && DRMCRT_strncmp( argv[0], "INVALID", 6 ) == 0 )
    {
        poEnvFile = &EnvUninited;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( poEnvFile, argv[0] );
    }

    ChkArg( argc >= 2 && argv[1] != NULL );

    if( argc > 2 )
    {
        ChkArg( argv[2] != NULL );
        iOption = ( DRM_DWORD ) strtoul( argv[2], NULL, 0 );
    }

    if ( (iOption & 1) != 0 )
    {
        if ( (iOption & 2) == 0 )
        {
            ChkMem( pwszFilename = (DRM_WCHAR*)Oem_MemAlloc(20 * SIZEOF( DRM_WCHAR )) );
            ZEROMEM( ( DRM_BYTE * )pwszFilename, 20 * SIZEOF( DRM_WCHAR ) );
        }
        if ( (iOption & 4) == 0 )
        {
            cchFilename = 20;
        }
        if ( (iOption & 8) != 0 )
        {
            pcchFilename = NULL;
        }
    }



    dr = Drm_Envelope_GetOriginalFilename( poEnvFile,
                                 pwszFilename,
                                 pcchFilename );

    if( DRM_E_BUFFERTOOSMALL == dr && iOption == 0 )
    {
        ChkMem( pwszFilename = (DRM_WCHAR*)Oem_MemAlloc(cchFilename * SIZEOF( DRM_WCHAR )) );
        ZEROMEM( ( DRM_BYTE * )pwszFilename, cchFilename * SIZEOF( DRM_WCHAR ) );

        ChkDR( Drm_Envelope_GetOriginalFilename( poEnvFile,
                                       pwszFilename,
                                       pcchFilename ) );
    }
    else
    {
        ChkDR(dr);
    }

    ChkDR( MakeDRMString(&dstrExpected, argv[1]) );

    ChkBOOL( cchFilename == dstrExpected.cchString, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRMCRT_wcsncmp((DRM_WCHAR*)pwszFilename, dstrExpected.pwszString,dstrExpected.cchString) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pwszFilename );
    FREEDRMSTRING( dstrExpected );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

