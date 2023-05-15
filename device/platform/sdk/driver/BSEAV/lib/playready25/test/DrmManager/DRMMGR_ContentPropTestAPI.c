/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <drmheaderparser.h>
#include <drmutf.h>
#include <drmmathsafe.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

/* Maximum incremental modification of string argument (in character). */
#define MAX_ARG_MOD_INC_CCH     CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) )

/*****************************************************************************
** Function:    TestManagerSetHeader
**
** Synopsis:    Test wrapper function for Drm_Content_SetProperty when setting
**              V2 headers, V2.4 headers, or V4 headers
**
** Arguments:   argv[0]    : DRMManager context status: NULL or Normal
**              argv[1]    : Optional header file name (defaults to shared buffer if not specified)
**              argv[2]    : Optional header file type: "UTF-8" or "UTF-16" or "BINARY"
**              argv[3]    : Optional length for the header in characters
**              argv[4]    : Optional header type: "V2", "V2.4", "V4", "PLAYREADY", or "AUTO" (default)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/

DRM_RESULT DRM_CALL TestManagerSetHeader( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr           = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD                *pcbHeader   = NULL;
    DRM_DWORD                cbHeader     = 0;    /* Main buffer counter */
    DRM_BYTE                 *pbHeader    = NULL; /* Main byte buffer */
    DRM_CONST_STRING         dstrHeader   = EMPTY_DRM_STRING; /* Buffer for UTF-8 -> UTF-16 conversion */
    eDRM_HEADER_VERSION      eVersion     = DRM_HEADER_VERSION_UNKNOWN;
    DRM_CONTENT_SET_PROPERTY eProperty    = DRM_CSP_HEADER_NOT_SET;
    DRM_WORD                 wFileType    = FILE_TYPE_UTF8;
    DRM_APP_CONTEXT          *poManagerContext = NULL;
    DRM_BOOL                 fHeaderFromFile   = FALSE;
    DRM_BOOL                 fFreedstrHeader   = FALSE;
    DRM_DWORD                cchHeaderType     = 0;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc >= 1 );

    if ( argc > 1 && argv[1] != NULL )
    {
        /* Check if file type is specified */
        if ( ( argc >= 3 ) && ( argv[2] != NULL ) )
        {
            if ( ( DRMCRT_strlen( argv[2] ) >= 6 ) &&
                 ( DRMCRT_strncmp( argv[2], "UTF-16", 6) == 0 ) )
            {
                wFileType = FILE_TYPE_UTF16;
            }
            if ( ( DRMCRT_strlen( argv[2] ) >= 6 ) &&
                 ( DRMCRT_strncmp( argv[2], "BINARY", 6) == 0 ) )
            {
                wFileType = FILE_TYPE_BINARY;
            }
        }
        pcbHeader = &cbHeader;
        ChkBOOL( LoadTestFile( "files", argv[1], &pbHeader, pcbHeader )
              || LoadTestFile( NULL, argv[1], &pbHeader, pcbHeader ), DRM_E_TEST_INCOMPLETE );
        fHeaderFromFile = TRUE;


        if ( wFileType == FILE_TYPE_UTF16 )
        {
            dstrHeader.pwszString = (const DRM_WCHAR*)pbHeader;
            dstrHeader.cchString = *pcbHeader / SIZEOF( DRM_WCHAR );
        }
        else if ( wFileType == FILE_TYPE_UTF8 )
        {
            ChkDR( MakeDRMString( &dstrHeader, (DRM_CHAR*)pbHeader ) );
            wFileType = FILE_TYPE_UTF16;
            fFreedstrHeader = TRUE;
        }
        else if( wFileType != FILE_TYPE_BINARY )
        {
            /* Invalid file type */
            ChkArg( FALSE );
        }

        /*
        ** Validation if header version should be here because
        ** below is modification of header size
        ** Validation will not work after header size modification
        */
        if ( wFileType == FILE_TYPE_UTF16 )
        {
            ChkDR( DRM_HDR_GetHeaderVersion( &dstrHeader, &eVersion ) );
        }
    }
    else if ( argc == 1 )
    {
        MAP_TO_GLOBAL_TEST_VAR( pbHeader, PROPERTY_DATA_VAR );
        MAP_TO_GLOBAL_TEST_VAR( pcbHeader, PROPERTY_SIZE_VAR );
        wFileType = FILE_TYPE_BINARY;
    }

    if ( ( argc > 3 ) && ( argv[3] != NULL ) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[3]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[3], cch, 10, &cbHeader ) );
        ChkArg( cbHeader <= dstrHeader.cchString );
        dstrHeader.cchString = *pcbHeader;
    }

    if ( wFileType == FILE_TYPE_UTF16 )
    {
        DRM_CHAR *pch = NULL;
        ChkMem( pch = (DRM_CHAR*)Oem_MemAlloc( dstrHeader.cchString + 1 ) );
        ZEROMEM( pch, SIZEOF(DRM_CHAR) * ( dstrHeader.cchString + 1 ) );
        DRM_UTL_DemoteUNICODEtoASCII( dstrHeader.pwszString, pch, dstrHeader.cchString );
        Log( "", "Header:%s", pch );
        SAFE_OEM_FREE( pch );
    }
    else if ( wFileType == FILE_TYPE_UTF8 )
    {
        Log( "", "Header:%s", (DRM_CHAR*) pbHeader );
    }
    else if ( wFileType == FILE_TYPE_BINARY )
    {
        Log( "", "Header is binary" );
    }

    /* Check if the header type to use is specified */
    eProperty = DRM_CSP_AUTODETECT_HEADER;
    if ( ( argc > 4 ) && ( argv[4] != NULL ) )
    {
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[4] ), &cchHeaderType ) );
        if ( ( cchHeaderType >= 4 ) &&
             ( DRMCRT_strncmp( argv[4], "V2.4", 4 ) == 0 ) )
        {
            eProperty = DRM_CSP_V2_4_HEADER;
            Log("", "Specified header type is V2.4" );
        }
        else if ( ( cchHeaderType >= 2 ) &&
                  ( DRMCRT_strncmp( argv[4], "V2", 2 ) == 0 ) )
        {
            eProperty = DRM_CSP_V2_HEADER;
            Log("", "Specified header type is V2" );
        }
        else if ( ( cchHeaderType >= 4 ) &&
                  ( DRMCRT_strncmp( argv[4], "V4.1", 4 ) == 0 ) )
        {
            eProperty = DRM_CSP_V4_1_HEADER;
            Log( "", "Specified header type is V4.1" );
        }
        else if ( ( cchHeaderType >= 2 ) &&
                  ( DRMCRT_strncmp( argv[4], "V4", 2 ) == 0 ) )
        {
            eProperty = DRM_CSP_V4_HEADER;
            Log( "", "Specified header type is V4" );
        }
        else if ( ( cchHeaderType >= 9 ) &&
                  ( DRMCRT_strncmp( argv[4], "PLAYREADY", 9 ) == 0 ) )
        {
            eProperty = DRM_CSP_PLAYREADY_OBJ;
            Log("", "Specified header type is PLAYREADY " );
        }
        else if ( ( cchHeaderType >= 4 ) &&
                  ( DRMCRT_strncmp( argv[4], "AUTO", 4 ) == 0 ) )
        {
            eProperty = DRM_CSP_AUTODETECT_HEADER;
            Log( "", "Specified use of autodetection for header type " );
        }
        else
        {
            Log( "", "Unknown header type specified: %s", argv[4] );
        }
    }

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    if ( wFileType == FILE_TYPE_UTF16 )
    {
        ChkDR( Drm_Content_SetProperty( argv[0] ? poManagerContext : NULL,
                                        eProperty,
                                        (DRM_BYTE*)dstrHeader.pwszString,
                                        dstrHeader.cchString * SIZEOF( DRM_WCHAR ) ) );
    }
    else if ( ( wFileType == FILE_TYPE_BINARY ) || ( wFileType == FILE_TYPE_UTF8 ) )
    {
        ChkDR( Drm_Content_SetProperty( argv[0] ? poManagerContext : NULL,
                                        eProperty,
                                        pbHeader,
                                        pcbHeader == NULL ? 0 : *pcbHeader ) );
    }
    else
    {
        /* Invalid file type */
        ChkArg( FALSE );
    }
PREFAST_POP

ErrorExit:
    if ( fHeaderFromFile )
    {
        SAFE_OEM_FREE( pbHeader );
    }
    if ( fFreedstrHeader )
    {
        FREEDRMSTRING( dstrHeader );
    }
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetPROHeaderWithKID
**
** Synopsis:    Test wrapper function for Drm_Content_SetProperty when setting a
**              PRO header with a separate KID
**
** Arguments:   argv[0]    : DRMManager context status: NULL or Normal
**              argv[1]    : Header file name -- Header file should contain a PlayReady Header object
**              argv[2]    : KID
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetPROHeaderWithKID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                dr                    = DRM_E_TEST_INCOMPLETE;
    DRM_CONTENT_SET_PROPERTY  eProperty             = DRM_CSP_PLAYREADY_OBJ_WITH_KID;
    DRM_DWORD                 cbHeader              = 0;    /* Main buffer counter */
    DRM_BYTE                 *pbHeader              = NULL; /* Main byte buffer */
    DRM_APP_CONTEXT          *poManagerContext      = NULL;
    DRM_CONST_STRING          wszProperty           = {0};
    DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA oPROWithKID = { 0 };

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc >= 3 );

    if ( argc > 1 && argv[1] != NULL )
    {
        ChkBOOL( LoadTestFile( "files", argv[1], &pbHeader, &cbHeader )
              || LoadTestFile( NULL, argv[1], &pbHeader, &cbHeader ), DRM_E_TEST_INCOMPLETE );

        oPROWithKID.pbHeaderData = pbHeader;
        oPROWithKID.cbHeaderData = cbHeader;
    }

    if ( argc > 2 && argv[2] != NULL )
    {
        ChkDR( MakeDRMString( &wszProperty, argv[2] ) );
        oPROWithKID.pbKeyID = (const DRM_BYTE*) wszProperty.pwszString;
        oPROWithKID.cbKeyID = wszProperty.cchString * SIZEOF( DRM_WCHAR );
    }

    ChkDR( Drm_Content_SetProperty( argv[0] ? poManagerContext : NULL,
                                    eProperty,
                                    (DRM_BYTE*) &oPROWithKID,
                                    SIZEOF( DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA ) ) );

ErrorExit:
    SAFE_OEM_FREE( pbHeader );
    FREEDRMSTRING( wszProperty );
    return dr;
}

/*****************************************************************************
** Function:    TestManagerSetV1Header
**
** Synopsis:    Test wrapper function for Drm_Content_SetProperty when setting V1 header
**
** Arguments:   argv[0]    : DRMManager context status: NULL or Normal
**              argv[1]    : Header file name or NULL -- Header file should contain an ASF V1 Header object
**              argv[2]    : Optional boolean indicating that header type is specified (default is TRUE)
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetV1Header( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr                   = DRM_E_TEST_INCOMPLETE;
    DRM_BOOL                 fSpecifyHeaderType   = TRUE;
    DRM_DWORD                cbHeader             = 0;    /* Main buffer counter */
    DRM_BYTE                 *pbHeader            = NULL; /* Main byte buffer */
    DRM_CSP_V1_HEADER_DATA   V1Header             = {0};  /* V1 Header */
    DRM_DWORD                cbProtectionData     = 0;
    DRM_DWORD                cbOffset             = 0;
    DRM_CONTENT_SET_PROPERTY eProperty            = DRM_CSP_V1_HEADER;
    DRM_APP_CONTEXT          *poManagerContext    = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc >= 2 );

    /*
    ** Check if header type is specified
    */
    if ( ( argc >= 3 ) && ( argv[2] != NULL ) )
    {
        ChkDR( StringToBool( argv[2], &fSpecifyHeaderType ) );
    }

    /*
    ** Open file with V1 data (if specified)
    */
    if ( argv[1] != NULL )
    {
        ChkArg( LoadTestFile( "files", argv[1], &pbHeader, &cbHeader ) );

        /*
        ** Input is the ASF V1 header object.  Skip object ID and size (24 bytes)
        */
        cbOffset = 24;

        /*
        ** Read size of secret data
        */
        ChkArg( cbHeader >= cbOffset + SIZEOF(DRM_DWORD) );
        BYTES_TO_DWORD( V1Header.cbSecretData, pbHeader + cbOffset );
        cbOffset = cbOffset + SIZEOF(DRM_DWORD);

        /*
        ** Read secret data
        */
        ChkArg( cbHeader >= cbOffset + V1Header.cbSecretData );
        V1Header.pbSecretData = pbHeader + cbOffset;
        cbOffset = cbOffset + V1Header.cbSecretData;

        /*
        ** Read size of protection data
        */
        ChkArg( cbHeader >= cbOffset + SIZEOF(DRM_DWORD) );
        BYTES_TO_DWORD( cbProtectionData, pbHeader + cbOffset );
        cbOffset = cbOffset + SIZEOF(DRM_DWORD);

        /*
        ** Skip the protection data
        */
        cbOffset = cbOffset + cbProtectionData;

        /*
        ** Read the KID data size
        */
        ChkArg( cbHeader >= cbOffset + SIZEOF(DRM_DWORD) );
        BYTES_TO_DWORD( V1Header.cbKeyID, pbHeader + cbOffset );
        cbOffset = cbOffset + SIZEOF(DRM_DWORD);

        /*
        ** Read the KID
        */
        ChkArg( cbHeader >= cbOffset + V1Header.cbKeyID );
        V1Header.pbKeyID = pbHeader + cbOffset;
        cbOffset = cbOffset + V1Header.cbKeyID;

        /*
        ** Read the URL data size
        */
        ChkArg( cbHeader >= cbOffset + SIZEOF(DRM_DWORD) );
        BYTES_TO_DWORD( V1Header.cbURL, pbHeader + cbOffset );
        cbOffset = cbOffset + SIZEOF(DRM_DWORD);

        /*
        ** Read the URL data
        */
        ChkArg( cbHeader >= cbOffset + V1Header.cbURL );
        V1Header.pbURL = pbHeader + cbOffset;

    }

    /*
    ** Check if the header type to use is specified
    */
    if ( !fSpecifyHeaderType )
    {
        eProperty = DRM_CSP_AUTODETECT_HEADER;
        Log("", "Specified use of autodetection for header type " );
    }

    ChkDR( Drm_Content_SetProperty( argv[0] ? poManagerContext : NULL,
                                    eProperty,
                                    (DRM_BYTE*)&V1Header,
                                    SIZEOF( DRM_CSP_V1_HEADER_DATA ) ) );

    /*
    ** At this point, the test has passed
    */
    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE(pbHeader);
    return dr;
}

/*****************************************************************************
** Function:    TestClearHeader
**
** Synopsis:    Clear the header in the app context
**
** Arguments:
**              argv[0]  -- ignored
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestClearHeader(long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                dr                = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *pmgrInternal      = NULL;
    DRM_APP_CONTEXT          *poManagerContext  = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    pmgrInternal = (DRM_APP_CONTEXT_INTERNAL *) poManagerContext;

    ZEROMEM( pmgrInternal->pbDRMHeaderData, pmgrInternal->cbHeaderData );

    pmgrInternal->cbHeaderData = 0;
    pmgrInternal->eHeaderInContext = DRM_CSP_HEADER_NOT_SET;
ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerSetKID
**
** Synopsis:    Set the app context header as a specified V2 header
**
** Arguments:
**              argv[0] -- KID to set,
**              argv[1] -- opt, manager context, NULL if NULL, Uninited if BAD,  otherwise use the global.
**              argv[2] -- opt, ccKid size midifer (pos or neg integer) defaults to 0.
**              argv[3]    : Optional boolean indicating that header type is specified (default is TRUE)
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetKID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT        dr      = DRM_SUCCESS;
    DRM_WCHAR         rgwcKID[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + MAX_ARG_MOD_INC_CCH ] = { 0 };
    DRM_STRING        dstr    = EMPTY_DRM_STRING;
    DRM_LONG          icchMod = 0;
    DRM_BOOL          fSpecifyHeaderType = TRUE;
    DRM_APP_CONTEXT   *poManagerContext  = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkArg( argc >= 1 );

    if ( argc > 2 && argv[2] != NULL )
    {
        icchMod = atoi( argv[2] );
        ChkArg( icchMod >= -(DRM_LONG)MAX_ARG_MOD_INC_CCH &&
                icchMod <= (DRM_LONG)MAX_ARG_MOD_INC_CCH );
    }

    /*
    ** Check if header type is specified
    */
    if ( ( argc > 3 ) && ( argv[3] != NULL ) )
    {
        ChkDR( StringToBool( argv[3], &fSpecifyHeaderType ) );
    }

    if( argv[0] != NULL )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkArg( cch <= NO_OF( rgwcKID ) );

        dstr.pwszString = rgwcKID;
        dstr.cchString = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

        TRACE(("KID: %s", argv[0]));

        /*
        ** Convert to UTF-16 (DRM string)
        */
        ChkDR( DRM_STR_UTF8toDSTR( ( DRM_CHAR* )( argv[0] ), cch, &dstr ) );
    }

    dstr.cchString += icchMod;
    ChkDR( Drm_Content_SetProperty( ( argc >= 2 && argv[1] == NULL ) ? NULL : poManagerContext,
                                    fSpecifyHeaderType ? DRM_CSP_KID : DRM_CSP_AUTODETECT_HEADER,
                                    (DRM_BYTE *) dstr.pwszString,
                                    dstr.cchString * SIZEOF( DRM_WCHAR ) ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerSetKIDFromList
**
** Synopsis:    From the next entry in the KID List test global variable:
**              - Set the KID in the header.
**              - Set the defaut test variable, AES_KEY_VAR, that encrypt will use.
**
** Arguments:   argc : 0
**              argv : ignored
**
** Returns:     DRM_RESULT
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetKIDFromList( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT            dr                                                              = DRM_SUCCESS;
    ResponseKIDListData  *poKIDListData                                                   = NULL;
    DRM_DWORD            *pdwKIDListIndex                                                 = NULL;
    DRM_APP_CONTEXT      *poManagerContext                                                = NULL;
    DRM_BYTE             *prgbSharedKey                                                   = NULL;
#if DBG
    DRM_DWORD             cchEncoded                                                      = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 );
    DRM_CHAR              rgcB64ContentKey[ CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 ) + 1 ] = { 0 };
#endif


    GET_SHARED_APP_CONTEXT( poManagerContext );
    MAP_TO_GLOBAL_TEST_VAR( poKIDListData,   RESPONSE_KIDLIST_DATA  );
    MAP_TO_GLOBAL_TEST_VAR( pdwKIDListIndex, RESPONSE_KIDLIST_INDEX );

#if DBG

    {
        DRM_CHAR rgch[ NO_OF( poKIDListData[ *pdwKIDListIndex ].rgwchKID ) ] = {'\0'};
        DRM_UTL_DemoteUNICODEtoASCII( poKIDListData[ *pdwKIDListIndex ].rgwchKID, rgch, NO_OF(rgch) );
        Log( "Trace", "\t         KID %d = %s", ( *pdwKIDListIndex + 1 ), rgch );
    }

    ChkDR( DRM_B64_EncodeA(  poKIDListData[ *pdwKIDListIndex ].rgbContentKey,
                             DRM_AES_KEYSIZE_128,
                             rgcB64ContentKey,
                            &cchEncoded,
                             0 ) );
    Log( "Trace", "\t Content Key %d = %s", ( *pdwKIDListIndex + 1 ), rgcB64ContentKey );
#endif

    ChkDR( Drm_Content_SetProperty( poManagerContext,
                                    DRM_CSP_KID,
                                    (DRM_BYTE *)&( poKIDListData[ *pdwKIDListIndex ].rgwchKID ),
                                    DRM_ID_B64_WCHAR_LEN ) );

    MAP_TO_GLOBAL_TEST_VAR_Alloc( prgbSharedKey, AES_KEY_VAR, DRM_AES_KEYSIZE_128 );
    MEMCPY( prgbSharedKey, poKIDListData[ *pdwKIDListIndex ].rgbContentKey, DRM_AES_KEYSIZE_128 );

    ( *pdwKIDListIndex )++;

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    TestManagerSetV41Header
**
** Synopsis:    Set the app context header as an empty V4.1 header
**
** Arguments:Drm_Content_SetProperty
**              argv[0] -- opt, WRMHeader string,
**              argv[1] -- opt, size of WRMHeader string in bytes
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerSetV41Header( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT         dr                = DRM_SUCCESS;
    DRM_APP_CONTEXT   *poManagerContext  = NULL;
    const DRM_BYTE    *pbWRMHeader       = NULL;
    DRM_DWORD          cbWRMHeader       = 0;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    if( argc > 0 )
    {
        pbWRMHeader = (const DRM_BYTE *)argv[0];
    }

    if( argc > 1 && argv[1] != NULL )
    {
        cbWRMHeader = (DRM_DWORD)atoi( argv[1] );
    }

    ChkDR( Drm_Content_SetProperty( poManagerContext,
                                    DRM_CSP_V4_1_HEADER,
                                    pbWRMHeader,
                                    cbWRMHeader ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerUpdateEmbeddedStore
**
** Synopsis:    Update the embedded store in a PlayReady object, using licenses from the HDS.
**
** Arguments:   argv[0] -- opt, manager context, NULL if NULL, otherwise use the global.
**
**              Can pass args with the following values:
**                    NULL_APPCONTEXT_TEST                         - Pass NULL for App Context
**                    INVALID_EMBEDDED_STORE                       - Pass invalid embedded store for update
**                    ZEROED_EMBEEDED_STORE                        - Pass zeroed embedded store for update
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerUpdateEmbeddedStore( long argc, __in_ecount( argc ) char **argv )
{
    #define NULL_APPCONTEXT_TEST              "NULL_APPCONTEXT_TEST"
    #define INVALID_EMBEDDED_STORE            "INVALID_EMBEDDED_STORE"
    #define INVALID_EMBEDDED_STORE_CONTENT    "INVALID_EMBEDDED_STORE_CONTENT\0"
    #define ZEROED_EMBEEDED_STORE             "ZEROED_EMBEEDED_STORE"

    long                          iArgument                 = 0;
    DRM_DWORD                     cbNumToCopy               = 0;
    DRM_DWORD                     cbEmbeddedStoreOrig       = 0;
    DRM_RESULT                    dr                        = DRM_SUCCESS;
    DRM_APP_CONTEXT              *poManagerContext          = NULL;
    DRM_APP_CONTEXT_INTERNAL     *poAppContextInternal      = NULL;
    DRM_BOOL                      fRestoreOrigEmbeddedStore = FALSE;
    DRM_BYTE                     *pbEmbeddedStoreOrig       = NULL;

    /*
    ** Set up poManagerContext
    */
    if ( argc == 0 || argv[0] != NULL )
    {
        GET_SHARED_APP_CONTEXT( poManagerContext );
        poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext;
    }

    while ( iArgument < argc )
    {
        if ( argv[iArgument] != NULL )
        {
            if ( ( DRMCRT_strncmp( argv[iArgument], NULL_APPCONTEXT_TEST, SIZEOF( NULL_APPCONTEXT_TEST ) ) == 0 ) )
            {
                poManagerContext = NULL;
            }
            if ( ( DRMCRT_strncmp( argv[iArgument], INVALID_EMBEDDED_STORE, SIZEOF( INVALID_EMBEDDED_STORE ) ) == 0 ) )
            {
                AssertChkArg( poAppContextInternal != NULL );
                if ( poAppContextInternal->pbEmbeddedStore && poAppContextInternal->cbEmbeddedStore > 0 )
                {
                    fRestoreOrigEmbeddedStore = TRUE;
                    ChkMem( pbEmbeddedStoreOrig = (DRM_BYTE*)Oem_MemAlloc ( poAppContextInternal->cbEmbeddedStore ) );
                    ZEROMEM( pbEmbeddedStoreOrig, poAppContextInternal->cbEmbeddedStore );

                    MEMCPY( pbEmbeddedStoreOrig,
                            poAppContextInternal->pbEmbeddedStore,
                            poAppContextInternal->cbEmbeddedStore );

                   cbNumToCopy = SIZEOF(INVALID_EMBEDDED_STORE_CONTENT);
                   if ( cbNumToCopy > poAppContextInternal->cbEmbeddedStore )
                   {
                      cbNumToCopy = poAppContextInternal->cbEmbeddedStore;
                   }
                   MEMCPY( poAppContextInternal->pbEmbeddedStore,
                           INVALID_EMBEDDED_STORE_CONTENT,
                           cbNumToCopy );
                   cbEmbeddedStoreOrig = poAppContextInternal->cbEmbeddedStore;
                   poAppContextInternal->cbEmbeddedStore = cbNumToCopy;
                }
            }
            if ( ( DRMCRT_strncmp( argv[iArgument], ZEROED_EMBEEDED_STORE, SIZEOF( ZEROED_EMBEEDED_STORE ) ) == 0 ) )
            {
                AssertChkArg( poAppContextInternal != NULL );
                if ( poAppContextInternal->pbEmbeddedStore && poAppContextInternal->cbEmbeddedStore > 0 )
                {
                    fRestoreOrigEmbeddedStore = TRUE;
                    ChkMem( pbEmbeddedStoreOrig = (DRM_BYTE*)Oem_MemAlloc ( poAppContextInternal->cbEmbeddedStore ) );
                    ZEROMEM( pbEmbeddedStoreOrig, poAppContextInternal->cbEmbeddedStore );
                    cbEmbeddedStoreOrig = poAppContextInternal->cbEmbeddedStore;

                    MEMCPY( pbEmbeddedStoreOrig,
                            poAppContextInternal->pbEmbeddedStore,
                            poAppContextInternal->cbEmbeddedStore );
                    ZEROMEM( poAppContextInternal->pbEmbeddedStore,
                             poAppContextInternal->cbEmbeddedStore );
                }
            }
        }
        iArgument = iArgument + 1;
    }

    ChkDR( Drm_Content_UpdateEmbeddedStore( poManagerContext ) );

ErrorExit:

    if(fRestoreOrigEmbeddedStore)
    {
         MEMCPY( poAppContextInternal->pbEmbeddedStore,
                 pbEmbeddedStoreOrig,
                 poAppContextInternal->cbEmbeddedStore );
         poAppContextInternal->cbEmbeddedStore = cbEmbeddedStoreOrig;
    }

    SAFE_OEM_FREE( pbEmbeddedStoreOrig );

    #undef NULL_APPCONTEXT_TEST
    #undef INVALID_EMBEDDED_STORE
    #undef ZEROED_EMBEEDED_STORE
    #undef INVALID_EMBEDDED_STORE_CONTENT

    return dr;
}

/*****************************************************************************
** Function:    TestManagerUpdateEmbeddedStoreCommit
**
** Synopsis:    Tell the App Context that we completed updating the embedde store in the content.
**
** Arguments:   argv[0] -- opt, manager context, NULL if NULL, otherwise use the global.
**
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any parameter is invalid or
*****************************************************************************/
DRM_RESULT DRM_CALL TestManagerUpdateEmbeddedStoreCommit( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_APP_CONTEXT *poAppContext = NULL;

    if ( argc == 0 || argv[0] != NULL )
    {
        GET_SHARED_APP_CONTEXT( poAppContext );
    }

    ChkDR( Drm_Content_UpdateEmbeddedStore_Commit( poAppContext ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TestManagerGetContentProperty
**
** Synopsis:    Test wrapper function for Drm_Content_GetProperty
**
** Arguments:   argv[0]    : property index
**              argv[1]    : expected property for verification
**              argv[2]    : type of argv[1]; "BYTE" or "WCHAR" or "FILE"
**              argv[3]    : Special test specifier argument
**              argv[4]    : if argv[3] is set into "SAVE_PRO_TO_FILE"
**                           then this is the file name to save PRO into.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL TestManagerGetContentProperty( long argc, __in_ecount( argc ) char **argv )
{
    #define NULL_APPCONTEXT_PROPERTYDATAPTR       "NULL_APPCONTEXT_PROPERTYDATAPTR"
    #define SMALL_CB_PROPERTY_DATA                "SMALL_CB_PROPERTY_DATA"
    #define NULL_CB_PROPERTY_DATA                 "NULL_CB_PROPERTY_DATA"
    #define INVALID_PLAYREADYOBJECT               "INVALID_PLAYREADYOBJECT"
    #define EMPTY_PLAYREADYOBJECT                 "EMPTY_PLAYREADYOBJECT"
    #define INVALID_PLAYREADYOBJECT_CONTENT       "ARO\0"
    #define ZERO_CB_PROPERTY_SIZE                 "ZERO_CB_PROPERTY_SIZE"
    #define SAVE_PRO_TO_FILE                      "SAVE_PRO_TO_FILE"

    DRM_DWORD                 iArgument            = 0;
    DRM_DWORD                 cbNumToCopy          = 0;
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                *pcbProperty          = NULL;
    DRM_BYTE                 *pbProperty           = NULL;
    DRM_CONST_STRING          wszProperty          = {0};
    DRM_APP_CONTEXT          *poManagerContext     = NULL;
    DRM_DWORD                 cbFiledata           = 0;
    DRM_BYTE                 *pbFileData           = NULL;
    DRM_BOOL                  fUseZeroedPRO        = FALSE;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = NULL;
    DRM_BYTE                 *rgbDRMHeaderDataOrig = NULL;
    DRM_STRING                dstrPROFileName      = EMPTY_DRM_STRING;
    DRM_BOOL                  fSavePROToFile       = FALSE;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * ) poManagerContext;

    MAP_TO_GLOBAL_TEST_VAR( pbProperty, PROPERTY_DATA_VAR );
    MAP_TO_GLOBAL_TEST_VAR( pcbProperty, PROPERTY_SIZE_VAR );

    /*
    ** argv[0] for property index; argv[1] for verify, argv[2] is type of argv[1]: BYTE or WCHAR
    */
    ChkArg( argc >= 2 );
    ChkArg( argc <= 5 );
    ChkArg( argv[0] != NULL );

    while ( iArgument  < (DRM_DWORD) argc )
    {
        if ( argv[iArgument ] != NULL )
        {
            if ( ( DRMCRT_strncmp( argv[iArgument ], NULL_APPCONTEXT_PROPERTYDATAPTR, SIZEOF(NULL_APPCONTEXT_PROPERTYDATAPTR) ) == 0 ) )
            {
                poManagerContext = NULL;
                pbProperty = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[iArgument ], SMALL_CB_PROPERTY_DATA, SIZEOF(SMALL_CB_PROPERTY_DATA) ) == 0 ) )
            {
                AssertChkArg( pcbProperty != NULL );
                *pcbProperty = 1;
            }
            else if ( ( DRMCRT_strncmp( argv[iArgument ], ZERO_CB_PROPERTY_SIZE, SIZEOF(ZERO_CB_PROPERTY_SIZE) ) == 0 ) )
            {
                AssertChkArg( pcbProperty != NULL );
                *pcbProperty = 0;
            }
            else if ( ( DRMCRT_strncmp( argv[iArgument ], NULL_CB_PROPERTY_DATA, SIZEOF(NULL_CB_PROPERTY_DATA) ) == 0 ) )
            {
                pcbProperty = NULL;
            }
            else if ( ( DRMCRT_strncmp( argv[iArgument ], EMPTY_PLAYREADYOBJECT, SIZEOF(EMPTY_PLAYREADYOBJECT) ) == 0 ) )
            {
                fUseZeroedPRO = TRUE;
                ChkMem( rgbDRMHeaderDataOrig = (DRM_BYTE*)Oem_MemAlloc (__CB_DECL( DWORD_ALIGNED_SIZE( DRM_STANDARD_PLAYREADYOBJSIZE ) )) );
                ZEROMEM( rgbDRMHeaderDataOrig,__CB_DECL( DWORD_ALIGNED_SIZE( DRM_STANDARD_PLAYREADYOBJSIZE ) ) );
                MEMCPY( rgbDRMHeaderDataOrig,
                        poAppContextInternal->pbDRMHeaderData,
                        poAppContextInternal->cbTotalHeaderData );
                ZEROMEM( poAppContextInternal->pbDRMHeaderData,
                        poAppContextInternal->cbTotalHeaderData );
            }
            else if ( DRMCRT_strncmp( argv[iArgument ], INVALID_PLAYREADYOBJECT, SIZEOF(INVALID_PLAYREADYOBJECT) ) == 0 )
            {
                fUseZeroedPRO = TRUE;
                ChkMem( rgbDRMHeaderDataOrig = (DRM_BYTE*)Oem_MemAlloc (__CB_DECL( DWORD_ALIGNED_SIZE( DRM_STANDARD_PLAYREADYOBJSIZE ) )) );
                ZEROMEM( rgbDRMHeaderDataOrig, __CB_DECL( DWORD_ALIGNED_SIZE( DRM_STANDARD_PLAYREADYOBJSIZE ) ) );
                MEMCPY( rgbDRMHeaderDataOrig,
                        poAppContextInternal->pbDRMHeaderData,
                        poAppContextInternal->cbTotalHeaderData );

                if (poAppContextInternal->cbTotalHeaderData > 0)
                {
                    cbNumToCopy = SIZEOF(INVALID_PLAYREADYOBJECT_CONTENT);
                    if ( cbNumToCopy > poAppContextInternal->cbTotalHeaderData )
                    {
                    cbNumToCopy = poAppContextInternal->cbTotalHeaderData;
                    }
                    MEMCPY( poAppContextInternal->pbDRMHeaderData,
                            INVALID_PLAYREADYOBJECT_CONTENT,
                            cbNumToCopy );
                }
            }
            else if ( DRMCRT_strncmp( argv[iArgument], SAVE_PRO_TO_FILE, SIZEOF(SAVE_PRO_TO_FILE) ) == 0 )
            {
                ChkArg( iArgument + 1 < (DRM_DWORD) argc && argv[iArgument + 1] != NULL );
                FREEDRMSTRING( dstrPROFileName );
                ChkDR( TST_UTL_MakePathString( NULL, argv[iArgument + 1], (DRM_CONST_STRING*)&dstrPROFileName) );
                fSavePROToFile = TRUE;
                iArgument  = iArgument + 1;
            }
       }
       iArgument  = iArgument + 1;
    }

    dr = Drm_Content_GetProperty( poManagerContext,
                                 (DRM_CONTENT_GET_PROPERTY) atoi( argv[0] ),
                                 pbProperty,
                                 pcbProperty );

    if( DRM_E_BUFFERTOOSMALL == dr )
    {
        MAP_TO_GLOBAL_TEST_VAR_Realloc( pbProperty, PROPERTY_DATA_VAR, *pcbProperty );

        ChkDR( Drm_Content_GetProperty( poManagerContext,
                                        (DRM_CONTENT_GET_PROPERTY) atoi( argv[0] ),
                                        pbProperty,
                                        pcbProperty ) );
    }
    else
    {
        ChkDR(dr);
    }
    /*
    ** verify the property
    */

    if ( pbProperty && argv[1] )
    {
        if( atoi( argv[0] ) == DRM_CGP_HEADER_TYPE )
        {
            /*
            ** This property gets returned as a binary DRM_DWORD
            */
            DRM_CONTENT_SET_PROPERTY dcsp = (DRM_CONTENT_SET_PROPERTY)atoi( argv[1] );

            ChkBOOL( dcsp == *( (DRM_CONTENT_SET_PROPERTY*)pbProperty ), DRM_E_TEST_UNEXPECTED_CONTENT_PROPERTY );
        }
        else if( atoi( argv[0] ) == DRM_CGP_CIPHER_TYPE )
        {
            /*
            ** This property gets returned as a binary DRM_DWORD
            */
            DRM_SUPPORTED_CIPHERS dsc = (DRM_SUPPORTED_CIPHERS)atoi( argv[1] );

            ChkBOOL( dsc == *( (DRM_SUPPORTED_CIPHERS*)pbProperty ), DRM_E_TEST_UNEXPECTED_CONTENT_PROPERTY );
        }
        else if( atoi( argv[0] ) == DRM_CGP_DECRYPTORSETUP )
        {
            /*
            ** This property gets returned as a binary DRM_DWORD
            */
            eDRM_DECRYPTORSETUP_TYPE dsc = ( eDRM_DECRYPTORSETUP_TYPE )atoi( argv[ 1 ] );

            ChkBOOL( dsc == *( ( eDRM_DECRYPTORSETUP_TYPE* )pbProperty ), DRM_E_TEST_UNEXPECTED_CONTENT_PROPERTY );
        }
        else
        {
            if ( argc >= 3
              && DRMCRT_strlen( argv[2] ) == 4
              && DRMCRT_strncmp( argv[2], "BYTE", 4) == 0 )
            {
                ChkBOOL( !DRMCRT_strncmp( (DRM_CHAR*)pbProperty, argv[1], *pcbProperty ),
                         DRM_E_TEST_UNEXPECTED_CONTENT_PROPERTY );
            }
            else if( argc >= 3
              && DRMCRT_strlen( argv[2] ) == 4
              && DRMCRT_strncmp( argv[2], "FILE", 4 ) == 0 )
            {
                ChkBOOL( LoadTestFile( "files", argv[1], &pbFileData, &cbFiledata )
                     || LoadTestFile( NULL, argv[1], &pbFileData, &cbFiledata ), DRM_E_TEST_INCOMPLETE )

                ChkBOOL( MEMCMP( pbProperty, pbFileData, cbFiledata ) == 0, DRM_E_TEST_UNEXPECTED_CONTENT_PROPERTY );
            }
            else
            {
                ChkDR( MakeDRMString( &wszProperty, argv[1] ) );
                ChkBOOL( !DRMCRT_wcsncmp( (DRM_WCHAR*)pbProperty, wszProperty.pwszString,wszProperty.cchString ),
                         DRM_E_TEST_UNEXPECTED_CONTENT_PROPERTY );
            }
        }
    }


    if ( fSavePROToFile )
    {
        /* requested property should be DRM_CGP_PLAYREADY_OBJ */
        ChkArg( atoi( argv[0] ) == 3 );
        ChkBOOL( SaveToFileW( dstrPROFileName.pwszString, pbProperty, *pcbProperty ), DRM_E_FILE_WRITE_ERROR );
    }

ErrorExit:
    FREEDRMSTRING( dstrPROFileName );
    FREEDRMSTRING( wszProperty );
    SAFE_OEM_FREE( pbFileData );
    if (fUseZeroedPRO)
    {
         if( rgbDRMHeaderDataOrig )
         {
           MEMCPY(  poAppContextInternal->pbDRMHeaderData,
                    rgbDRMHeaderDataOrig,
                    poAppContextInternal->cbTotalHeaderData );
         }
         SAFE_OEM_FREE(rgbDRMHeaderDataOrig);
    }
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

