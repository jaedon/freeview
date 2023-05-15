/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "drmlicense.h"
#include "drmheaderparser.h"
#include "drmhdrbuilder.h"
#include "drmplayreadyobj.h"
#include <HeaderTestAPI.h>

ENTER_PKTEST_NAMESPACE_CODE

DRM_CONST_STRING g_strContentHeader, g_strContentHeaderBackup, *g_pStrContentHeader;
DRM_BYTE  *g_pbHBContext = NULL;
DRM_DWORD g_cbHBContext = 0;


static DRM_RESULT _ResetGlobalHBContext( DRM_DWORD cbNewSize )
{
    DRM_RESULT dr = DRM_SUCCESS;

    SAFE_OEM_FREE(g_pbHBContext);
    ChkMem( g_pbHBContext = (DRM_BYTE*)Oem_MemAlloc( cbNewSize ) );
    g_cbHBContext = cbNewSize;
    ZEROMEM( g_pbHBContext, g_cbHBContext );

ErrorExit:
    return dr;
}

static DRM_RESULT DupDRMString(DRM_CONST_STRING* pTarget, DRM_CONST_STRING* pSource)
{
    DRM_RESULT dr = DRM_SUCCESS;
    pTarget->cchString = pSource->cchString;
#pragma prefast(push)
#pragma prefast(disable:22011) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( sizeof(DRM_WCHAR) * pTarget->cchString, pTarget->cchString );
    ChkMem(pTarget->pwszString = (DRM_WCHAR*)Oem_MemAlloc( sizeof(DRM_WCHAR) * max(1, pTarget->cchString) ) ); /* some testcases may have cchString == 0 */
#pragma prefast(pop)
    memcpy((void*)pTarget->pwszString, pSource->pwszString, sizeof(DRM_WCHAR) * max(1, pTarget->cchString));
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(Header)(long lTCID, const char *strTCName)
{
    g_strContentHeader.pwszString = g_strContentHeaderBackup.pwszString = NULL;
    g_strContentHeader.cchString = g_strContentHeaderBackup.cchString = 0;
    g_pStrContentHeader = NULL;
    g_pbHBContext = NULL;
    g_cbHBContext = 0;

    return DRM_SUCCESS;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(Header)(long lTCID, const char *strTCName)
{
    FREEDRMSTRING( g_strContentHeader );
    FREEDRMSTRING( g_strContentHeaderBackup );
    SAFE_OEM_FREE(g_pbHBContext);
    return DRM_SUCCESS;
}



/*
** Load test file into g_strContentHeader, make a backup and set up g_pStrContentHeader
** argv[0]: xml file name, NULL, or EMPTY
*/
DRM_RESULT DRM_CALL TestLoadHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE *pbData = NULL;
    DRM_DWORD cbData = 0;

    ChkArg(argc == 1);

    /*
    ** Clean up all global variables
    */
    FREEDRMSTRING( g_strContentHeader );
    FREEDRMSTRING( g_strContentHeaderBackup );
    SAFE_OEM_FREE(g_pbHBContext);
    g_strContentHeader.pwszString = g_strContentHeaderBackup.pwszString = NULL;
    g_strContentHeader.cchString = g_strContentHeaderBackup.cchString = 0;
    g_pStrContentHeader = NULL;
    g_pbHBContext = NULL;
    g_cbHBContext = 0;

    if (argv[0]) {
        if (*argv[0]) {
            ChkArg(LoadTestFile(NULL, argv[0], &pbData, &cbData));
        }

        ChkDR( MakeDRMString(&g_strContentHeader, pbData? (DRM_CHAR*)pbData: "") );

        ChkDR(DupDRMString(&g_strContentHeaderBackup, &g_strContentHeader));
        g_pStrContentHeader = &g_strContentHeader;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE(pbData);
    return dr;
}


/*
** Load test file into g_strContentHeader, make a backup and set up g_pStrContentHeader
** argv[0]: xml file name, NULL, or EMPTY
*/
DRM_RESULT DRM_CALL TestLoadWideCharHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_E_TEST_INCOMPLETE;

    ChkArg(argc >= 1);

    /*
    ** Clean up all global variables
    */
    FREEDRMSTRING( g_strContentHeader );
    FREEDRMSTRING( g_strContentHeaderBackup );
    SAFE_OEM_FREE(g_pbHBContext);
    g_strContentHeader.pwszString = g_strContentHeaderBackup.pwszString = NULL;
    g_strContentHeader.cchString = g_strContentHeaderBackup.cchString = 0;
    g_pStrContentHeader = NULL;
    g_pbHBContext = NULL;
    g_cbHBContext = 0;

    if( argv[0] != NULL )
    {
        if (*argv[0] != '\0' )
        {
            ChkBOOL( LoadTestFile( NULL, argv[0], (DRM_BYTE **) &(g_strContentHeader.pwszString), &(g_strContentHeader.cchString) ), DRM_E_FILEREADERROR );
            g_strContentHeader.cchString /= SIZEOF(DRM_WCHAR);
        }
        ChkDR(DupDRMString(&g_strContentHeaderBackup, &g_strContentHeader));
        g_pStrContentHeader = &g_strContentHeader;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*
** Test API DRM_HDR_Verify
** argv[0]: public key: validPubkey, wrongPubkey, InvalidPubkey, NULL
** argv[1]: crypto option: validCrypto, NULL
** argv[2]: return pointer option: NORMAL, NULL
** argv[3]: optional: expected returned value: 0, 1
*/
DRM_RESULT DRM_CALL TestContentHeaderVerify( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_LONG   lResult = 0;

    /*
    ** Need to free these
    */
    DRM_CONST_STRING   strPubKey       = EMPTY_DRM_STRING;
    DRM_CONST_STRING   strPubKeyBackup = EMPTY_DRM_STRING;
    DRM_CRYPTO_CONTEXT *pCryptoContext = NULL;

    ChkArg( argc >= 3 );

    /*
    ** set the public key
    */
    if ( argv[0] != NULL )
    {
        ChkDR( MakeDRMString( &strPubKey, argv[0] ) );
        ChkDR( DupDRMString( &strPubKeyBackup, &strPubKey ) );
    }

    /*
    ** init crypto context
    */
    if ( argv[1] != NULL )
    {
        ChkMem( pCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( sizeof(DRM_CRYPTO_CONTEXT) ) );
        DRMCRT_memset( pCryptoContext, 0, sizeof(DRM_CRYPTO_CONTEXT) );
    }

    dr = DRM_HDR_Verify(  g_pStrContentHeader,
                          argv[ 0 ] ? &strPubKey : NULL,
                          pCryptoContext,
                          argv[ 2 ] ? &lResult : NULL );

    /* Verify the content header is intact */
    if ( g_pStrContentHeader != NULL )
    {
        ChkBOOL( 0 == DRMCRT_wcsncmp( g_strContentHeader.pwszString,
                                      g_strContentHeaderBackup.pwszString,
                                      g_strContentHeaderBackup.cchString ),
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /*
    ** Verify the public key is intact
    */
    if ( argv[ 0 ] != NULL )
    {
        ChkBOOL( 0 == DRMCRT_wcsncmp( strPubKey.pwszString,
                                      strPubKeyBackup.pwszString,
                                      strPubKeyBackup.cchString ),
                 DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /*
    ** Verify the result value
    */
    if ( 3 <  argc && NULL != argv[ 3 ] )
    {
        ChkBOOL( lResult == atol(argv[ 3 ] ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

ErrorExit:
    FREEDRMSTRING( strPubKey );
    FREEDRMSTRING( strPubKeyBackup );
    SAFE_OEM_FREE(pCryptoContext);
    return dr;
}

/*
** Test API DRM_HDR_GetAttribute
** argv[0]: attribute name: EMPTY, NULL or an actual attribute name
** argv[1]: attribute index: int, valid range is 1 - 10
** argv[2]: pointer to return data option: NORMAL, NULL
** argv[3]: optional: expected returned attribute data
*/
DRM_RESULT DRM_CALL TestContentHeaderGetAttribute(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT             dr                     = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD              dwAttributeIndex       = 6;
    DRM_CONST_STRING       strReturnedAttribute   = EMPTY_DRM_STRING;

    /*
    ** Need to free these
    */
    DRM_CONST_STRING strAttributeName       = EMPTY_DRM_STRING;
    DRM_CONST_STRING strAttributeNameBackup = EMPTY_DRM_STRING;
    DRM_CONST_STRING strExpectedData        = EMPTY_DRM_STRING;

    ChkArg( argc >= 3 );
    ChkArg( argv[1] != NULL );

    /*
    ** Set the attribute name
    */
    if ( argv[0] != NULL )
    {
        ChkDR( MakeDRMString( &strAttributeName, argv[0] ) );
        ChkDR( DupDRMString( &strAttributeNameBackup, &strAttributeName ) );
    }

    /*
    ** Set the attribute index
    */
    ChkDR( DRMCRT_AtoDWORD( argv[1], (DRM_DWORD)strlen( argv[1] ), 10, &dwAttributeIndex ) );


    ChkDR(DRM_HDR_GetAttribute(
        g_pStrContentHeader,
        argv[0]? &strAttributeName: NULL,
        (eDRM_HEADER_ATTRIBUTES)dwAttributeIndex,
        argv[2]? &strReturnedAttribute: NULL, 0));

    /*
    ** verify the content string is intact
    */
    if ( g_pStrContentHeader != NULL )
    {
        ChkBOOL( 0 == DRMCRT_wcsncmp( g_strContentHeader.pwszString, g_strContentHeaderBackup.pwszString, g_strContentHeaderBackup.cchString ), DRM_E_FAIL );
    }

    /*
    ** verify the attribute name is intact
    */
    if ( argv[0] != NULL )
    {
        ChkBOOL( 0 == DRMCRT_wcsncmp( strAttributeName.pwszString, strAttributeNameBackup.pwszString, strAttributeNameBackup.cchString ), DRM_E_FAIL );
    }

    /*
    ** verify the expected return data
    */
    if ( ( argc > 3 ) && ( argv[3] != NULL ) )
    {
        ChkDR( MakeDRMString( &strExpectedData, argv[3] ) );
        ChkBOOL( 0 == DRMCRT_wcsncmp( strReturnedAttribute.pwszString, strExpectedData.pwszString, strReturnedAttribute.cchString ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /*
    ** At this point, the test has passed
    */
    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( strExpectedData );
    FREEDRMSTRING( strAttributeName );
    FREEDRMSTRING( strAttributeNameBackup );
    return dr;
}

/*
** Test API DRM_HDR_GetUplink
** argv[0]: uplink index to retrieve
** argv[1]: expected uplink KID or NULL
*/
DRM_RESULT DRM_CALL TestContentHeaderGetUplink(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr             = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD        dwUplinkNumber = 0;
    DRM_CONST_STRING szKID          = EMPTY_DRM_STRING;;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );

    ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 10, &dwUplinkNumber ) );

    ChkDR(DRM_HDR_GetUplink(
        g_pStrContentHeader,
        dwUplinkNumber,
        argv[1]? &szKID: NULL));

    /*
    ** verify the content string is intact
    */
    if ( g_pStrContentHeader != NULL )
    {
        ChkBOOL( 0 == DRMCRT_wcsncmp(g_strContentHeader.pwszString, g_strContentHeaderBackup.pwszString, g_strContentHeaderBackup.cchString ), DRM_E_FAIL );
    }


     /*
     ** verify the retrieved uplink KID
     */
    if ( argv[1] != NULL )
    {
        ChkBOOL( 0 == CompDRMString( &szKID, argv[1] ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*
** Test API DRM_HDR_GetHeaderVersion
** argv[0]: expected version number as specified in eDRM_HEADER_VERSION or NULL
*/
DRM_RESULT DRM_CALL TestGetHeaderVersion(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT           dr               = DRM_E_TEST_INCOMPLETE;
    eDRM_HEADER_VERSION  eVersion         = DRM_HEADER_VERSION_UNKNOWN;
    eDRM_HEADER_VERSION  eVersionExpected = DRM_HEADER_VERSION_UNKNOWN;
    DRM_DWORD            dwVersion        = 0;

    ChkArg( argc <= 1 );

    if ( argv[0] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[0], (DRM_DWORD)strlen( argv[0] ), 10, &dwVersion ) );
        eVersionExpected = (eDRM_HEADER_VERSION)dwVersion;
    }

    ChkDR( DRM_HDR_GetHeaderVersion( g_pStrContentHeader, &eVersion ) );

    /*
    ** verify the version number
    */

    if ( ( argv[0] != NULL ) && ( eVersion != eVersionExpected ) )
    {
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_CreateHeader
**
** Synopsis:    Test wrapper for the function DRM_HB_CreateHeader.
**
** Arguments:   argv[0]   : Header type: "V2", "V2.4", "V4", or "AUTO". Default is "Not Set" header
**              argv[1]   : Optional, initial size of buffer.  Default is NULL.
**              argv[2]   : Optional, initial buffer size value to use.  Default is NULL DWORD pointer.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_CreateHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT               dr           = DRM_E_TEST_INCOMPLETE;
    DRM_CONTENT_SET_PROPERTY eProperty    = DRM_CSP_HEADER_NOT_SET;
    DRM_DWORD                dwBufferSize = 0;
    DRM_DWORD                *pdwTestCB   = NULL;

    ChkArg( argc >= 1 );

    /*
    ** Check the header type
    */
    if ( argv[0] != NULL )
    {
        if ( ( DRMCRT_strlen( argv[0] ) >= 4 ) &&
             ( DRMCRT_strncmp( argv[0], "V2.4", 4) == 0 ) )
        {
            eProperty = DRM_CSP_V2_4_HEADER;
            Log("", "Test_HB_CreateHeader: Specified header type is V2.4" );
        }
        else if ( ( DRMCRT_strlen( argv[0] ) >= 2 ) &&
                  ( DRMCRT_strncmp( argv[0], "V2", 2) == 0 ) )
        {
            eProperty = DRM_CSP_V2_HEADER;
            Log("", "Test_HB_CreateHeader: Specified header type is V2" );
        }
        else if ( ( DRMCRT_strlen( argv[0] ) >= 4 ) &&
                  ( DRMCRT_strncmp( argv[0], "V4.1", 4) == 0 ) )
        {
            eProperty = DRM_CSP_V4_1_HEADER;
            Log("", "Test_HB_CreateHeader: Specified header type is V4.1" );
        }
        else if ( ( DRMCRT_strlen( argv[0] ) >= 2 ) &&
                  ( DRMCRT_strncmp( argv[0], "V4", 2) == 0 ) )
        {
            eProperty = DRM_CSP_V4_HEADER;
            Log("", "Test_HB_CreateHeader: Specified header type is V4" );
        }
        else if ( ( DRMCRT_strlen( argv[0] ) >= 4 ) &&
                  ( DRMCRT_strncmp( argv[0], "AUTO", 2) == 0 ) )
        {
            eProperty = DRM_CSP_AUTODETECT_HEADER;
            Log("", "Test_HB_CreateHeader: Specified use of autodetection for header type " );
        }
        else
        {
            Log("", "Test_HB_CreateHeader: Unknown header type specified: %s.  Using DRM_CSP_HEADER_NOT_SET.", argv[0]);
        }
    }

    /*
    ** Check for specified buffer size
    */
    if ( ( argc >= 2 ) && ( argv[1] != NULL ) )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[1], DRMCRT_strlen( argv[1] ), 10, &g_cbHBContext ) );
        ChkDR( _ResetGlobalHBContext( g_cbHBContext ) );
    }

    /*
    ** Check for specified buffer size value
    */
    if ( ( argc >= 3 ) && ( argv[2] != NULL ) )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[2], DRMCRT_strlen( argv[2] ), 10, &dwBufferSize ) );
        pdwTestCB = &dwBufferSize;
    }

    /*
    ** Call DRM_HB_CreateHeader, but expect DRM_E_BUFFERTOOSMALL
    */
    dr =  DRM_HB_CreateHeader( g_pbHBContext, eProperty, pdwTestCB );
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        Log("", "Test_HB_CreateHeader: Create Header requires resizing the global context" );
        /*
        ** Resize the context buffer
        */
        ChkDR( _ResetGlobalHBContext( dwBufferSize ) );

        /*
        ** Call DRM_HB_CreateHeader, expecting success this time
        */
        ChkDR( DRM_HB_CreateHeader( g_pbHBContext, eProperty, &g_cbHBContext ) );
    }
    else
    {
        Log("", "Test_HB_CreateHeader: Create Header succeeded without resizing the global context" );
        ChkDR( dr );
    }

    dr = DRM_SUCCESS;


ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_CloseHeader
**
** Synopsis:    Test wrapper for the function DRM_HB_CloseHeader.
**
** Arguments:   argv[0]   : Private Key (for signed headers) in base64
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** NOTE: The resulting header will be placed into the test global variable g_strContentHeader
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_CloseHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CRYPTO_CONTEXT      *pcontextCrypto   = NULL;
    DRM_CONST_STRING        dstrPrivKey       = EMPTY_DRM_STRING;
    DRM_BYTE                *pbXML            = NULL;
    DRM_DWORD               cbXML             = 0;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrPrivKey, argv[0] ) );

    /*
    ** Setup crypto context
    */
    ChkMem( pcontextCrypto = (DRM_CRYPTO_CONTEXT *)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcontextCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    /*
    ** Close the header
    */
    Log("", "DRM_HB_CloseHeader: Closing and signing header" );
    ChkDR( DRM_HB_CloseHeader( g_pbHBContext, pcontextCrypto, &dstrPrivKey, &pbXML, &cbXML ) );
    Log("", "DRM_HB_CloseHeader: Final header size is %d bytes", cbXML );

    /*
    ** Put header into global variables
    */
    FREEDRMSTRING( g_strContentHeader );
    g_strContentHeader.pwszString = NULL;
    FREEDRMSTRING( g_strContentHeaderBackup );
    g_strContentHeaderBackup.pwszString = NULL;

    ChkMem( g_strContentHeader.pwszString = (DRM_WCHAR*)Oem_MemAlloc( cbXML + 1 ) );
    g_strContentHeader.cchString =  cbXML / SIZEOF(DRM_WCHAR);
    ZEROMEM( (DRM_WCHAR*)g_strContentHeader.pwszString, cbXML + 1 );
    MEMCPY( (DRM_WCHAR*)g_strContentHeader.pwszString, pbXML, cbXML );

    Log("", "DRM_HB_CloseHeader: Copy XML Header" );
    ChkDR( DupDRMString( &g_strContentHeaderBackup, &g_strContentHeader ) );
    g_pStrContentHeader = &g_strContentHeader;

    dr = DRM_SUCCESS;


ErrorExit:
    SAFE_OEM_FREE( pcontextCrypto );
    FREEDRMSTRING( dstrPrivKey );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_ReallocHeader
**
** Synopsis:    Test wrapper for the function DRM_HB_ReallocHeader.
**
** Arguments:   argv[0]   : New size
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_ReallocHeader(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD               dwNewSize         = 0;
    DRM_BYTE                *pbNewHBContext   = NULL;
    _HBContext              *pbNewHB          = NULL;
    _HBContext              *pOldHB          = NULL;

    ChkArg( argc >= 1 );

    /*
    ** Check for new size
    */
    if ( argv[0] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[0], DRMCRT_strlen( argv[0] ), 10, &dwNewSize ) );
        ChkMem( pbNewHBContext = (DRM_BYTE *)Oem_MemAlloc( dwNewSize ) );
        ZEROMEM( pbNewHBContext, dwNewSize );
    }

    /*
    ** Initialize the new HB context
    */
    ChkDR( DRM_HB_CreateHeader( pbNewHBContext, DRM_CSP_V2_HEADER, &dwNewSize ) );

    pbNewHB = (_HBContext *)pbNewHBContext;
    pOldHB = (_HBContext *)g_pbHBContext;

    /*
    ** Realloc HB Context
    */
    ChkDR( DRM_HB_ReallocHeader( g_pbHBContext, dwNewSize, pbNewHBContext ) );

    /*
    ** Verify that new buffer matches old buffer
    */
    ChkBOOL( pbNewHB->dwItemsAdded == pOldHB->dwItemsAdded, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbNewHB->eState == pOldHB->eState, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbNewHB->fKIDAdded == pOldHB->fKIDAdded, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbNewHB->fLAINFOAdded == pOldHB->fLAINFOAdded, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbNewHB->fChecksumAdded == pOldHB->fChecksumAdded, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /*
    ** Assign new buffer to global pointer
    */
    SAFE_OEM_FREE( g_pbHBContext );
    g_pbHBContext = pbNewHBContext;
    pbNewHBContext = NULL;

    g_cbHBContext = dwNewSize;

    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbNewHBContext );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_GetContextSize
**
** Synopsis:    Test wrapper for the function DRM_HB_GetContextSize.  Verifies
**              that output size matches expected size
**
** Arguments:   argv[0]   : Expected size
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_GetContextSize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD               cbHBContext       = 0;
    DRM_DWORD               dwExpectedSize    = 0;

    ChkArg( argc >= 1 );

    if ( argv[0] != NULL )
    {
        ChkDR( DRMCRT_AtoDWORD( argv[0], DRMCRT_strlen( argv[0] ), 10, &dwExpectedSize ) );
    }

    Log("", "Test_HB_GetContextSize: Expected Context Size is %d", dwExpectedSize );
    ChkDR( DRM_HB_GetContextSize( g_pbHBContext, &cbHBContext ) );
    Log("", "Test_HB_GetContextSize: Actual Context Size is %d", cbHBContext );

    ChkBOOL( cbHBContext == g_cbHBContext, DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_OpenNode
**
** Synopsis:    Test wrapper for the function DRM_HB_OpenNode.
**
** Arguments:   argv[0]   : Name of node to open
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_OpenNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING        dstrNodeName      = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrNodeName, argv[0] ) );

    Log("", "Test_HB_OpenNode: Opening node: %s", argv[0] );
    ChkDR( DRM_HB_OpenNode( g_pbHBContext, &dstrNodeName ) );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrNodeName );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_CloseCurrNode
**
** Synopsis:    Test wrapper for the function DRM_HB_CloseCurrNode.
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_CloseCurrNode(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;

    ChkDR( DRM_HB_CloseCurrNode( g_pbHBContext ) );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_AddData
**
** Synopsis:    Test wrapper for the function DRM_HB_AddData.
**
** Arguments:   argv[0]   : Data
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_AddData(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING        dstrData          = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrData, argv[0] ) );

    Log("", "Test_HB_AddData: Adding data: %s", argv[0] );
    ChkDR( DRM_HB_AddData( g_pbHBContext, &dstrData ) );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrData );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_SetKeyID
**
** Synopsis:    Test wrapper for the function DRM_HB_SetKeyID.
**
** Arguments:   argv[0]   : KID in base64
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_SetKeyID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING        dstrKID           = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrKID, argv[0] ) );

    Log("", "Test_HB_SetKeyID: Adding <KID>%s</KID>", argv[0] );
    ChkDR( DRM_HB_SetKeyID( g_pbHBContext, &dstrKID ) );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrKID );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_SetLicAcqUrl
**
** Synopsis:    Test wrapper for the function DRM_HB_SetLicAcqUrl.
**
** Arguments:   argv[0]   : LAURL
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_SetLicAcqUrl(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING        dstrURL           = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrURL, argv[0] ) );

    Log("", "Test_HB_SetLicAcqUrl: Adding <LAURL>%s</LAURL>", argv[0] );
    ChkDR( DRM_HB_SetLicAcqUrl( g_pbHBContext, &dstrURL ) );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrURL );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_SetChecksum
**
** Synopsis:    Test wrapper for the function DRM_HB_SetChecksum.
**
** Arguments:   argv[0]   : Content Key (base 64)
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_SetChecksum(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING        dstrKey           = EMPTY_DRM_STRING;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( MakeDRMString( &dstrKey, argv[0] ) );

    Log("", "Test_HB_SetChecksum: Adding <CHECKSUM>%s</CHECKSUM>", argv[0] );
    ChkDR( DRM_HB_SetChecksum( g_pbHBContext, &dstrKey ) );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrKey );
    return dr;
}

/*****************************************************************************
** Function:    Test_HB_SetAttribute
**
** Synopsis:    Test wrapper for the function DRM_HB_SetAttribute.
**
** Arguments:   argv[0]   : Attribute Name
**              argv[1]   : Attribute Value
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_HB_SetAttribute(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT              dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING        dstrName          = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrValue         = EMPTY_DRM_STRING;

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrName, argv[0] ) );
    ChkDR( MakeDRMString( &dstrValue, argv[1] ) );

    Log("", "Test_HB_SetAttribute: %s=%s", argv[0], argv[1] );
    ChkDR( DRM_HB_SetAttribute( g_pbHBContext, &dstrName, &dstrValue ) );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrName );
    FREEDRMSTRING( dstrValue );
    return dr;
}

/*********************************************************
**
** Function: TestPlayReadyObjectParser
**
** Synopsis: Extracts V4 XML header from PlayReady object
**
** Arguments:
**  argv[0] -- file name for the input file
**  argv[1] -- path for the output file (V4 XML header object)
**
** Returns     DRM_SUCCESS if V4 XML header was sucessfully extracted from PlayReady object
**             DRM_E_TEST_INVALIDARG if input files is not PlayReady object
**             other DRM_RESULT I\O error otherwise
**
*********************************************************/
DRM_RESULT DRM_CALL TestPlayReadyObjectParser(long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_DWORD        cbPlayReadyObject = 0;
    DRM_BYTE        *pbPlayReadyObject = NULL;
    DRM_WCHAR       *pwszV4XMLHeader   = NULL;
    DRM_DWORD        cchV4XMLHeader    = 0;
    DRM_DWORD        dwV4XMLSize       = 0;
    DRM_CONST_STRING dstrOutFilename   = EMPTY_DRM_STRING;

    ChkArg( argc == 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkBOOL( LoadTestFile( NULL, argv[0], &pbPlayReadyObject, &cbPlayReadyObject ), DRM_E_FILEREADERROR );

    /*  DRM_PRO_GetRecord returns in last parameter count of bytes in record, not wide characters */
    ChkDR( DRM_PRO_GetRecord( pbPlayReadyObject, cbPlayReadyObject, PLAYREADY_WRMHEADER, (DRM_BYTE **)&pwszV4XMLHeader, &dwV4XMLSize ) );
    cchV4XMLHeader = dwV4XMLSize / SIZEOF( DRM_WCHAR );

    ChkDR( TST_UTL_MakePathString( NULL, argv[1], &dstrOutFilename ) );

    ChkBOOL( SaveToFileW(dstrOutFilename.pwszString, (DRM_BYTE*)pwszV4XMLHeader, cchV4XMLHeader*SIZEOF(DRM_WCHAR) ), DRM_E_FILEWRITEERROR );
ErrorExit:

    FREEDRMSTRING( dstrOutFilename );
    SAFE_OEM_FREE( pbPlayReadyObject );
    return dr;
}

/**********************************************************************
**
** Function: Test_DRM_PRO_CreatePlayReadyDRMHeader
**
** Synopsis: Tests the creation of PlayReady header.
**
**
** Arguments:
**            IN argc     - number of arguments
**            IN argv[0]  - Path to the V4 XML file.
**            IN argv[1]  - Flag for buffer parameter for PlayReady header. NULL or NORMAL
**            IN argv[2]  - Flag what to pass for size of PlayReady header. NULL - means detect from BUFFER_TOO_SMALL.
**                          Otherwize use provided value in argv[2]
**            IN argv[3]  - Expected cipher type. If it is not NULL, the value is compared against the cipher type in the
**                          generated PlayReady header.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG for invalid arguments
**              DRM_E_FAIL if PlayReady header fails validation.
**              Other errors returned by DRM_PRO_Create
**
**
**********************************************************************/
DRM_RESULT DRM_CALL Test_DRM_PRO_CreatePlayReadyDRMHeader
(
        IN long               argc,
        IN DRM_CHAR         **argv
)

{   DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_CONST_STRING    dstrHeader          = EMPTY_DRM_STRING;
    DRM_BYTE           *pbPlayReadyHeader   = NULL;
    DRM_DWORD           cbPlayReadyHeader   = 0;
    /*
    ** Variables for verification of PlayReady header.
    */
    DRM_WCHAR          *pwszV4Header    = 0;
    DRM_DWORD           cchV4Header     = 0;
    DRM_DWORD           dwV4HeaderSize  = 0;
    DRM_BYTE           *pbEmbeddedStore = NULL;
    DRM_DWORD           dwEmbedStoreSize= 0;
    DRM_SUPPORTED_CIPHERS eExpectedCipherType = eDRM_CIPHER_INVALID;
    DRM_SUPPORTED_CIPHERS eActualCipherType   = eDRM_CIPHER_INVALID;

    /*
    ** All parameters should be provided.
    */
    ChkArg( argc == 4 );

    /*
    ** If argv[0] is NULL, then no file and string dstrHeader is empty.
    */
    if ( argv[0] != NULL )
    {
        ChkArg( LoadTestFile(NULL, argv[0], (DRM_BYTE **)&dstrHeader.pwszString, &dstrHeader.cchString ) == TRUE );
        dstrHeader.cchString /= SIZEOF( DRM_WCHAR );
    }

    /*
    ** This would be the size of the header. If it is NULL, we get the size from the call of DRM_PRO_Create().
    */
    if ( argv[2] != NULL )
    {
        cbPlayReadyHeader = atoi( argv[2] );
    }
    else
    {   /*
        ** Call DRM_PRO_Create to get the size of the required buffer
        */
        dr = DRM_PRO_Create( &dstrHeader, pbPlayReadyHeader, &cbPlayReadyHeader );

        if ( dr != DRM_E_BUFFERTOOSMALL )
        {
            ChkDR( dr );
        }

    }

    /*
    ** Set the buffer for created PlayReady header.
    ** If it is indicated as NULL, we leave it as NULL.
    */
    if ( argv[1] != NULL )
    {
        ChkMem( pbPlayReadyHeader = ( DRM_BYTE *) Oem_MemAlloc( cbPlayReadyHeader ) );

    }

    if ( argv[3] != NULL )
    {
        eExpectedCipherType = ( DRM_SUPPORTED_CIPHERS )atoi( argv[3] );
    }

    /*
    ** Now calls the API
    */
    ChkDR( DRM_PRO_Create( &dstrHeader, pbPlayReadyHeader, &cbPlayReadyHeader ) );


    /*
    **  If API succeded, we try to verify that V4 XML object is inserted into PlayReady object
    */

   /*
   ** Validates PlayReady object and extracts V4 XML from there.
   */
   ChkDR( DRM_PRO_GetRecord( pbPlayReadyHeader, cbPlayReadyHeader, PLAYREADY_WRMHEADER, (DRM_BYTE **)&pwszV4Header, &dwV4HeaderSize ) );
   cchV4Header = dwV4HeaderSize / SIZEOF( DRM_WCHAR );
   /*
   ** Validates that V4 XML is the same as was inserted.
   */
   ChkBOOL( cchV4Header == dstrHeader.cchString, DRM_E_FAIL );
   ChkBOOL( 0 == MEMCMP( pwszV4Header, dstrHeader.pwszString, cchV4Header * SIZEOF( DRM_WCHAR ) ), DRM_E_FAIL );

   /*
   ** Check that embedded store is present:
   */
   ChkDR( DRM_PRO_GetRecord( pbPlayReadyHeader, cbPlayReadyHeader, PLAYREADY_EMBEDDED_LICENSE_STORE, &pbEmbeddedStore, &dwEmbedStoreSize ) );
   ChkBOOL( dwEmbedStoreSize == MAX_EMBEDDED_STORE_LEN, DRM_E_FAIL  );

   if ( eExpectedCipherType != eDRM_CIPHER_INVALID )
   {
        ChkDR( DRM_PRO_GetCipherType( pbPlayReadyHeader, cbPlayReadyHeader, &eActualCipherType ) );

        ChkBOOL( eActualCipherType == eExpectedCipherType, DRM_E_FAIL );
   }

ErrorExit:
    SAFE_OEM_FREE ( pbPlayReadyHeader );
    SAFE_OEM_FREE ( dstrHeader.pwszString );

    return dr;
}

/****************************************************************************************
**
** Function: Test_DRM_PRO_GetRecord
**
** Synopsis: Extracts records from PlayReady object
**
** Arguments:
**  argv[0] -- file name for the input file
**
** Returns     DRM_SUCCESS if record was sucessfully extracted from PlayReady object
**             DRM_E_TEST_INVALIDARG if input files is not PlayReady object
**             other DRM_RESULT I\O error otherwise
**
*****************************************************************************************/
DRM_RESULT DRM_CALL Test_DRM_PRO_GetRecord(long argc, __in_ecount(argc) char **argv )
{

    #define NULL_ARGUMENTS                    "NULL_ARGUMENTS"

    DRM_RESULT        dr                      = DRM_SUCCESS;
    DRM_DWORD         cbPlayReadyObject       = 0;
    DRM_BYTE         *pbPlayReadyObject       = NULL;
    DRM_WCHAR        *pbEmbeddedStoreFromPRO  = NULL;
    DRM_DWORD         cbEmbeddedStoreFromPRO  = 0;
    long              iArgument               = 0;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkBOOL( LoadTestFile( NULL, argv[0], &pbPlayReadyObject, &cbPlayReadyObject ), DRM_E_FILEREADERROR );

    while ( iArgument < argc )
    {
        if ( argv[iArgument] != NULL )
        {
            if ( ( DRMCRT_strncmp( argv[iArgument], NULL_ARGUMENTS, SIZEOF( NULL_ARGUMENTS ) ) == 0 ) )
            {
                SAFE_OEM_FREE( pbPlayReadyObject );
                pbPlayReadyObject = NULL;
                cbPlayReadyObject = 0;
            }
        }
        iArgument = iArgument + 1;
    }

    /*
    ** Check for embedded store
    */

    ChkDR( DRM_PRO_GetRecord( pbPlayReadyObject, cbPlayReadyObject, PLAYREADY_EMBEDDED_LICENSE_STORE, (DRM_BYTE **)&pbEmbeddedStoreFromPRO, &cbEmbeddedStoreFromPRO ) );

ErrorExit:

    SAFE_OEM_FREE( pbPlayReadyObject );

    #undef NULL_ARGUMENTS

    return dr;

}

/****************************************************************************************
**
** Function: Test_DRM_PRO_IsValidObject
**
** Synopsis: Extracts records from PlayReady object
**
** Arguments:
**  argv[0] -- file name for the input file
**
** Returns     DRM_SUCCESS if record was sucessfully extracted from PlayReady object
**             DRM_E_TEST_INVALIDARG if input files is not PlayReady object
**             other DRM_RESULT I\O error otherwise
**
*****************************************************************************************/
DRM_RESULT DRM_CALL Test_DRM_PRO_IsValidObject(long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT        dr                      = DRM_SUCCESS;
    DRM_DWORD         cbPlayReadyObject       = 0;
    DRM_BYTE         *pbPlayReadyObject       = NULL;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkBOOL( LoadTestFile( NULL, argv[0], &pbPlayReadyObject, &cbPlayReadyObject ), DRM_E_FILEREADERROR );

    ChkDR( DRM_PRO_IsValidObject( pbPlayReadyObject, cbPlayReadyObject ) );


ErrorExit:

    SAFE_OEM_FREE( pbPlayReadyObject );

    return dr;

}

/**********************************************************************
**
** Function: Test_DRM_PRO_ConvertV2HeaderToPlayReady
**
** Synopsis: Tests the conversion from a V2 header to a PlayReady object.
**
** Arguments:
**            IN argc     - number of arguments
**            IN argv[0]  - Path to the V2 XML file.
**            IN argv[1]  - Optional LA URL.
**            IN argv[2]  - Optional LUI URL.
**            IN argv[3]  - Optional domain service ID.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG for invalid arguments
**              DRM_E_FAIL if failed.
**
**********************************************************************/
DRM_RESULT DRM_CALL Test_DRM_PRO_ConvertV2HeaderToPlayReady(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr                = DRM_SUCCESS;
    DRM_BYTE           *pbV2Header        = NULL;
    DRM_DWORD           cbV2Header        = 0;
    DRM_BYTE           *pbV4Header        = NULL;
    DRM_DWORD           cbV4Header        = 0;
    DRM_CONST_STRING    dstrV4Header      = EMPTY_DRM_STRING;
    DRM_BYTE           *pbPlayReadyHeader = NULL;
    DRM_DWORD           cbPlayReadyHeader = 0;
    DRM_SUPPORTED_CIPHERS eCipherType     = eDRM_CIPHER_INVALID;
    DRM_CONST_STRING    dstrLAURL         = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrLUIURL        = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrDSID          = EMPTY_DRM_STRING;
    DRM_SUBSTRING       dasstr            = EMPTY_DRM_SUBSTRING;

    /*
    ** All parameters should be provided.
    */
    ChkArg( argc >= 1 );
    ChkArg( argv[ 0 ] != NULL );

    ChkArg( LoadTestFile( NULL, argv[ 0 ], ( DRM_BYTE ** )&pbV2Header, &cbV2Header ) == TRUE );

    if ( argc >= 2 && argv[ 1 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 1 ] ) + 1;
        dstrLAURL.cchString = dasstr.m_cch;

        ChkMem( dstrLAURL.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrLAURL.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrLAURL.pwszString, dstrLAURL.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 1 ], &dasstr, ( DRM_STRING * )&dstrLAURL );
        dstrLAURL.cchString -= 1;
    }

    if ( argc >= 3 && argv[ 2 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 2 ] ) + 1;
        dstrLUIURL.cchString = dasstr.m_cch;

        ChkMem( dstrLUIURL.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrLUIURL.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrLUIURL.pwszString, dstrLUIURL.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 2 ], &dasstr, ( DRM_STRING * )&dstrLUIURL );
        dstrLUIURL.cchString -= 1;
    }

    if ( argc >= 4 && argv[ 3 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 3 ] ) + 1;
        dstrDSID.cchString = dasstr.m_cch;

        ChkMem( dstrDSID.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrDSID.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrDSID.pwszString, dstrDSID.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 3 ], &dasstr, ( DRM_STRING * )&dstrDSID );
        dstrDSID.cchString -= 1;
    }

    dr = DRM_PRO_ConvertHeaderFromWmdrmToPlayReady(
                                    pbV2Header,
                                    cbV2Header,
                                    dstrLAURL.pwszString,
                                    dstrLAURL.cchString,
                                    dstrLUIURL.pwszString,
                                    dstrLUIURL.cchString,
                                    dstrDSID.pwszString,
                                    dstrDSID.cchString,
                                    NULL,
                                    0,
                                    NULL,
                                    &cbV4Header );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( DRM_E_FAIL );
    }

    ChkMem( pbV4Header = ( DRM_BYTE * )Oem_MemAlloc( cbV4Header ) );

    ChkDR( DRM_PRO_ConvertHeaderFromWmdrmToPlayReady(
                                    pbV2Header,
                                    cbV2Header,
                                    dstrLAURL.pwszString,
                                    dstrLAURL.cchString,
                                    dstrLUIURL.pwszString,
                                    dstrLUIURL.cchString,
                                    dstrDSID.pwszString,
                                    dstrDSID.cchString,
                                    NULL,
                                    0,
                                    pbV4Header,
                                    &cbV4Header ) );

    DSTR_FROM_PB( &dstrV4Header, pbV4Header, cbV4Header );

    dr = DRM_PRO_Create( &dstrV4Header,
                         NULL,
                         &cbPlayReadyHeader );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( DRM_E_FAIL );
    }

    ChkMem( pbPlayReadyHeader = ( DRM_BYTE * ) Oem_MemAlloc( cbPlayReadyHeader ) );

    ChkDR( DRM_PRO_Create( &dstrV4Header,
                           pbPlayReadyHeader,
                           &cbPlayReadyHeader ) );

    ChkDR( DRM_PRO_GetCipherType( pbPlayReadyHeader, cbPlayReadyHeader, &eCipherType ) );

    ChkBOOL( eCipherType == eDRM_RC4_CIPHER, DRM_E_FAIL );

ErrorExit:

    FREEDRMSTRING( dstrLAURL );
    FREEDRMSTRING( dstrLUIURL );
    FREEDRMSTRING( dstrDSID );

    SAFE_OEM_FREE ( pbV2Header );
    SAFE_OEM_FREE ( pbV4Header );
    SAFE_OEM_FREE ( pbPlayReadyHeader );

    return dr;
}

/*****************************************************************************
** Function:    Test_Drm_PlayReadyObject_ConvertFromWmdrmHeader
**
** Synopsis:    Test wrapper function for Drm_PlayReadyObject_ConvertFromWmdrmHeader
**
** Arguments:   argv[0]    : Filename of V2 header to be converted.
**              argv[1]    : Filename of expected V4 header after conversion.
**              argv[2]    : Silent LA URL.
**              argv[3]    : Non silent LA URL.
**              argv[4]    : Domain service ID.
**              argv[5]    : Include ELS flag.
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_Drm_PlayReadyObject_ConvertFromWmdrmHeader(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING dstrLAURL         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLUIURL        = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDSID          = EMPTY_DRM_STRING;
    DRM_DWORD        cbFileDataV2      = 0;
    DRM_BYTE        *pbFileDataV2_1    = NULL;
    DRM_BYTE        *pbFileDataV2_2    = NULL;
    DRM_DWORD        cbFileDataV4      = 0;
    DRM_BYTE        *pbFileDataV4_1    = NULL;
    DRM_BYTE        *pbFileDataV4_2    = NULL;
    DRM_BYTE        *pbV4Header        = NULL;
    DRM_DWORD        cbV4Header        = 0;
    DRM_DWORD        cbPlayReadyObject = 0;
    DRM_BYTE        *pbPlayReadyObject = NULL;
    DRM_BOOL         fIncludeELS       = TRUE;

    ChkArg( argc >= 3 );
    ChkArg( argv[ 0 ] != NULL && argv[ 1 ] != NULL );

    ChkBOOL( LoadTestFile( NULL, argv[ 0 ], &pbFileDataV2_1, &cbFileDataV2 ), DRM_E_TEST_INCOMPLETE );
    ChkMem( pbFileDataV2_2 = ( DRM_BYTE * )Oem_MemAlloc( cbFileDataV2 * SIZEOF( DRM_WCHAR ) ) );
    TST_UTL_mbstowcs( ( DRM_WCHAR * )pbFileDataV2_2, ( DRM_CHAR * )pbFileDataV2_1, cbFileDataV2 );

    ChkBOOL( LoadTestFile( NULL, argv[ 1 ], &pbFileDataV4_1, &cbFileDataV4 ), DRM_E_TEST_INCOMPLETE );
    ChkMem( pbFileDataV4_2 = ( DRM_BYTE * )Oem_MemAlloc( cbFileDataV4 * SIZEOF( DRM_WCHAR ) ) );
    TST_UTL_mbstowcs( ( DRM_WCHAR * )pbFileDataV4_2, ( DRM_CHAR * )pbFileDataV4_1, cbFileDataV4 );

    if ( argc >= 3 && argv[ 2 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrLAURL, argv[ 2 ] ) );
    }

    if ( argc >= 4 && argv[ 3 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrLUIURL, argv[ 3 ] ) );
    }

    if ( argc >= 5 && argv[ 4 ] != NULL )
    {
        ChkDR( MakeDRMString( &dstrDSID, argv[ 4 ] ) );
    }

    if ( argc >= 6 && argv[ 5 ] != NULL )
    {
        fIncludeELS = atoi( argv[ 5 ] );
    }

    dr = Drm_PlayReadyObject_ConvertFromWmdrmHeader( pbFileDataV2_2,
                                                     cbFileDataV2 * SIZEOF( DRM_WCHAR ),
                                                     dstrLAURL.pwszString,
                                                     dstrLAURL.cchString,
                                                     dstrLUIURL.pwszString,
                                                     dstrLUIURL.cchString,
                                                     dstrDSID.pwszString,
                                                     dstrDSID.cchString,
                                                     fIncludeELS,
                                                     NULL,
                                                     0,
                                                     NULL,
                                                     &cbPlayReadyObject );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( DRM_SUCCESS );
    }
    else
    {
        /*
        ** A success value will pass ChkDR. Anything but
        ** DRM_E_BUFFERTOOSMALL should fail here.
        */
        ChkBOOL( DRM_FAILED( dr ), DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkDR( dr );
    }

    ChkMem( pbPlayReadyObject = ( DRM_BYTE * )Oem_MemAlloc( cbPlayReadyObject ) );

    ChkDR( Drm_PlayReadyObject_ConvertFromWmdrmHeader( pbFileDataV2_2,
                                                       cbFileDataV2 * SIZEOF( DRM_WCHAR ),
                                                       dstrLAURL.pwszString,
                                                       dstrLAURL.cchString,
                                                       dstrLUIURL.pwszString,
                                                       dstrLUIURL.cchString,
                                                       dstrDSID.pwszString,
                                                       dstrDSID.cchString,
                                                       fIncludeELS,
                                                       NULL,
                                                       0,
                                                       pbPlayReadyObject,
                                                       &cbPlayReadyObject ) );

    ChkDR( DRM_PRO_GetRecord( pbPlayReadyObject,
                              cbPlayReadyObject,
                              PLAYREADY_WRMHEADER,
                              &pbV4Header,
                              &cbV4Header ) );

    ChkBOOL( cbV4Header == cbFileDataV4 * SIZEOF( DRM_WCHAR ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( MEMCMP( pbV4Header, pbFileDataV4_2, cbFileDataV4 * SIZEOF( DRM_WCHAR ) ) == 0,
             DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:

    SAFE_OEM_FREE( pbPlayReadyObject );

    SAFE_OEM_FREE( pbFileDataV4_2 );

    SAFE_OEM_FREE( pbFileDataV4_1 );

    SAFE_OEM_FREE( pbFileDataV2_2 );

    SAFE_OEM_FREE( pbFileDataV2_1 );

    FREEDRMSTRING( dstrLAURL );

    FREEDRMSTRING( dstrLUIURL );

    FREEDRMSTRING( dstrDSID );

    return dr;
}

/*****************************************************************************
** Function:    Test_Drm_ConvertFileFromWmdrmToPlayReady
**
** Synopsis:    Test wrapper function for converting a V2 WMDRM file into a
**              PlayReady file.
**
** Arguments:   argv[0]    : Filename of V2 WMDRM file to be converted.
**              argv[1]    : Filename of expected PlayReady file after conversion.
**              argv[2]    : Silent LA URL.
**              argv[3]    : Non silent LA URL.
**              argv[4]    : Domain service ID.
**              argv[5]    : Domain service ID. Optional
**
** Returns:     DRM_SUCCESS                  - on success
**
** Notes:
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_Drm_ConvertFileFromWmdrmToPlayReady(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_CONST_STRING dstrInput          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutput         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSilentURL      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNonSilentURL   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDSID           = EMPTY_DRM_STRING;

    ChkArg( argc >= 3 );

    ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrInput) );
    ChkDR( TST_UTL_MakePathString( NULL, argv[1], &dstrOutput) );
    ChkDR( MakeDRMString(&dstrSilentURL, argv[2]) );

    if ( argc >= 4 && argv[3] != NULL )
    {
        ChkDR( MakeDRMString(&dstrNonSilentURL, argv[3]) );
    }

    if ( argc >= 5 && argv[4] != NULL )
    {
        ChkDR( MakeDRMString(&dstrDSID, argv[4]) );
    }

    ChkDR( TST_UTL_ConvertFileFromWMDRMToPlayReady ( &dstrInput,
                                                     &dstrOutput,
                                                     &dstrSilentURL,
                                                     &dstrNonSilentURL,
                                                     &dstrDSID ) );

ErrorExit:
    FREEDRMSTRING( dstrInput );
    FREEDRMSTRING( dstrOutput );
    FREEDRMSTRING( dstrSilentURL );
    FREEDRMSTRING( dstrNonSilentURL );
    FREEDRMSTRING( dstrDSID );

    return dr;
}

/**********************************************************************
**
** Function: TestCreateHeader
**
** Synopsis: Tests the creation of a PlayReady RM header (V4 or V4.1).
**
** Arguments:
**            IN argc     - number of arguments
**            IN argv[0]  - Must be string "DRM_PRO_CreateRMHeader" or "Drm_Content_SetProperty"
**                          which indicates which function to call to create the header.
**            IN argv[1]  - Path to the V4 or V4.1 XML file to compare the result with.
**            IN argv[2]  - Cipher type. Valid values: AESCTR, COCKTAIL, INVALID  (Optional if Drm_Content_SetProperty)
**            IN argv[3]  - KID  (Optional if Drm_Content_SetProperty)
**            IN argv[4]  - Checksum  (Optional if Drm_Content_SetProperty)
**            IN argv[5]  - Optional LA URL.
**            IN argv[6]  - Optional LUI URL.
**            IN argv[7]  - Optional domain service ID.
**            IN argv[8]  - Optional Custom attributes.
**            IN argv[9]  - Optional Decryptor Setup. If supplied will cause V4.1 header to
**                          be generated, else v4. Valid values: ONDEMAND
**                          Note: V4.1 header will also be generated for Drm_Content_SetProperty
**                          if argv[2], argv[3], and/or argv[4] are NULL.
**            IN argv[10] - Header version to generate. Valid values: DRM_HEADER_VERSION_4, DRM_HEADER_VERSION_4_1
**                          if argv[9] is set then DRM_HEADER_VERSION_4 is an invalid value for this parameter.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG for invalid arguments
**              DRM_E_FAIL if failed.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestCreateHeader(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr                  = DRM_SUCCESS;
    DRM_BYTE           *pbBuffer            = NULL;
    DRM_BYTE           *pbV4ExpectedHeader  = NULL;
    DRM_DWORD           cbV4ExpectedHeader  = 0;
    DRM_BYTE           *pbV4Header          = NULL;
    DRM_DWORD           cbV4Header          = 0;
    DRM_SUPPORTED_CIPHERS eCipherType       = eDRM_CIPHER_INVALID;
    DRM_CONST_STRING    dstrKID             = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrChecksum        = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrLAURL           = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrLUIURL          = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrDSID            = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrCustomAttrs     = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrDecryptorSetup  = EMPTY_DRM_STRING;
    DRM_SUBSTRING       dasstr              = EMPTY_DRM_SUBSTRING;
    eDRM_HEADER_VERSION eHeaderVersion      = DRM_HEADER_VERSION_4; /* default to v4 */
    DRM_BOOL            fUseSetProperty     = FALSE;

    /*
    ** First two parameters should be provided.
    */
    ChkArg( argc >= 2 );

    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );

    /*
    ** Which function to call
    */
    if ( 0 == DRMCRT_strncmp( argv[ 0 ], "DRM_PRO_CreateRMHeader", DRMCRT_strlen( argv[ 0 ] ) ) )
    {
        fUseSetProperty = FALSE;
    }
    else if ( 0 == DRMCRT_strncmp( argv[ 0 ], "Drm_Content_SetProperty", DRMCRT_strlen( argv[ 0 ] ) ) )
    {
        fUseSetProperty = TRUE;
    }
    else
    {
        ChkArg( FALSE );
    }

    if( !fUseSetProperty )
    {
        ChkArg( argv[ 2 ] != NULL );
        ChkArg( argv[ 3 ] != NULL );
        ChkArg( argv[ 4 ] != NULL );
    }

    if( argv[ 2 ] == NULL
     || argv[ 3 ] == NULL
     || argv[ 4 ] == NULL )
    {
        eHeaderVersion = DRM_HEADER_VERSION_4_1;
    }

    ChkArg( LoadTestFile( NULL, argv[ 1 ], ( DRM_BYTE ** )&pbBuffer, &cbV4ExpectedHeader ) == TRUE );

    /*
    ** Skip 0xFF,0xFE at the beginning of the file
    */
    pbV4ExpectedHeader = pbBuffer + 2;
    cbV4ExpectedHeader -= 2;

    /*
    ** Cipher type
    */
    if( argv[ 2 ] != NULL )
    {
        if ( 0 == DRMCRT_strncmp( argv[ 2 ], "AESCTR", DRMCRT_strlen( argv[ 2 ] ) ) )
        {
            eCipherType = eDRM_AES_COUNTER_CIPHER;
        }
        else if ( 0 == DRMCRT_strncmp( argv[ 2 ], "COCKTAIL", DRMCRT_strlen( argv[ 2 ] ) ) )
        {
            eCipherType = eDRM_RC4_CIPHER;
        }
        else
        {
            ChkArg( FALSE );
        }
    }

    /*
    ** KID
    */
    if( argv[ 3 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 3 ] ) + 1;
        dstrKID.cchString = dasstr.m_cch;

        ChkMem( dstrKID.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrKID.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrKID.pwszString, dstrKID.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 3 ], &dasstr, ( DRM_STRING * )&dstrKID );
        dstrKID.cchString -= 1;
    }

    /*
    ** Checksum
    */
    if( argv[ 4 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 4 ] ) + 1;
        dstrChecksum.cchString = dasstr.m_cch;

        ChkMem( dstrChecksum.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrChecksum.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrChecksum.pwszString, dstrChecksum.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 4 ], &dasstr, ( DRM_STRING * )&dstrChecksum );
        dstrChecksum.cchString -= 1;
    }

    /*
    ** LAURL
    */
    if ( argc >= 6 && argv[ 5 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 5 ] ) + 1;
        dstrLAURL.cchString = dasstr.m_cch;

        ChkMem( dstrLAURL.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrLAURL.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrLAURL.pwszString, dstrLAURL.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 5 ], &dasstr, ( DRM_STRING * )&dstrLAURL );
        dstrLAURL.cchString -= 1;
    }

    /*
    ** UIURL
    */
    if ( argc >= 7 && argv[ 6 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 6 ] ) + 1;
        dstrLUIURL.cchString = dasstr.m_cch;

        ChkMem( dstrLUIURL.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrLUIURL.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrLUIURL.pwszString, dstrLUIURL.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 6 ], &dasstr, ( DRM_STRING * )&dstrLUIURL );
        dstrLUIURL.cchString -= 1;
    }

    /*
    ** DSID
    */
    if ( argc >= 8 && argv[ 7 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 7 ] ) + 1;
        dstrDSID.cchString = dasstr.m_cch;

        ChkMem( dstrDSID.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrDSID.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrDSID.pwszString, dstrDSID.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 7 ], &dasstr, ( DRM_STRING * )&dstrDSID );
        dstrDSID.cchString -= 1;
    }

    /*
    ** CustomAttrs
    */
    if ( argc >= 9 && argv[ 8 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 8 ] ) + 1;
        dstrCustomAttrs.cchString = dasstr.m_cch;

        ChkMem( dstrCustomAttrs.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrCustomAttrs.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrCustomAttrs.pwszString, dstrCustomAttrs.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 8 ], &dasstr, ( DRM_STRING * )&dstrCustomAttrs );
        dstrCustomAttrs.cchString -= 1;
    }

    /*
    ** DecryptorSetup
    */
    if ( argc >= 10 && argv[ 9 ] != NULL )
    {
        dasstr.m_ich = 0;
        dasstr.m_cch = ( DRM_DWORD )DRMCRT_strlen( argv[ 9 ] ) + 1;
        dstrDecryptorSetup.cchString = dasstr.m_cch;

        ChkMem( dstrDecryptorSetup.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( dstrDecryptorSetup.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrDecryptorSetup.pwszString, dstrDecryptorSetup.cchString * SIZEOF( DRM_WCHAR ) );

        DRM_UTL_PromoteASCIItoUNICODE( argv[ 9 ], &dasstr, ( DRM_STRING * )&dstrDecryptorSetup );
        dstrDecryptorSetup.cchString -= 1;
        eHeaderVersion = DRM_HEADER_VERSION_4_1;
    }

    /*
    ** header version override
    */
    if ( argc >= 11 && argv[ 10 ] != NULL )
    {
        if ( 0 == DRMCRT_strncmp( argv[ 10 ], "DRM_HEADER_VERSION_4_1", DRMCRT_strlen( argv[ 10 ] ) ) )
        {
            eHeaderVersion = DRM_HEADER_VERSION_4_1;
        }
        else if ( 0 == DRMCRT_strncmp( argv[ 10 ], "DRM_HEADER_VERSION_4", DRMCRT_strlen( argv[ 10 ] ) ) )
        {
            /*
            ** Make sure other parameters do not require this header to be a v4.1, if so fail. 
            */
            ChkBOOL( DRM_HEADER_VERSION_4_1 != eHeaderVersion, DRM_E_INVALIDARG ); 

            eHeaderVersion = DRM_HEADER_VERSION_4;
        }
        else
        {
            ChkDR( DRM_E_INVALIDARG );
        }
    }

    if( fUseSetProperty )
    {
        DRM_APP_CONTEXT *poAppContext = NULL;

        DRM_CSP_HEADER_COMPONENTS_DATA oComponents;
        oComponents.eHeaderVersion              = eHeaderVersion;
        oComponents.eCipherType                 = eCipherType;
        oComponents.dstrKID                     = dstrKID;
        oComponents.dstrCheckSum                = dstrChecksum;
        oComponents.dstrPlayReadySilentURL      = dstrLAURL;
        oComponents.dstrPlayReadyNonSilentURL   = dstrLUIURL;
        oComponents.dstrServiceID               = dstrDSID;
        oComponents.dstrCustomAttributesXML     = dstrCustomAttrs;
        oComponents.dstrDecryptorSetup          = dstrDecryptorSetup;

        GET_SHARED_APP_CONTEXT( poAppContext );
        ChkDR( Drm_Content_SetProperty( poAppContext,
                                        DRM_CSP_HEADER_COMPONENTS,
                                        (const DRM_BYTE*) &oComponents,
                                        SIZEOF( oComponents ) ) );

        dr = Drm_Content_GetProperty( poAppContext,
                                      DRM_CGP_HEADER,
                                      NULL,
                                     &cbV4Header );

        if ( dr != DRM_E_BUFFERTOOSMALL )
        {
            Log("", "Drm_Content_GetProperty: First call returned 0x%x", dr );
            ChkDR( DRM_E_FAIL );
        }

        ChkMem( pbV4Header = ( DRM_BYTE * )Oem_MemAlloc( cbV4Header ) );

        ChkDR( Drm_Content_GetProperty( poAppContext,
                                        DRM_CGP_HEADER,
                                        pbV4Header,
                                       &cbV4Header ) );
    }
    else
    {
        dr = DRM_PRO_CreateRMHeader(  eHeaderVersion,
                                      eCipherType,
                                     &dstrKID,
                                     &dstrChecksum,
                                     &dstrLAURL,
                                     &dstrLUIURL,
                                     &dstrDSID,
                                     &dstrCustomAttrs,
                                     &dstrDecryptorSetup,
                                      NULL,
                                     &cbV4Header );

        if ( dr != DRM_E_BUFFERTOOSMALL )
        {
            ChkDR( DRM_E_FAIL );
        }

        ChkMem( pbV4Header = ( DRM_BYTE * )Oem_MemAlloc( cbV4Header ) );

        ChkDR( DRM_PRO_CreateRMHeader(  eHeaderVersion,
                                        eCipherType,
                                       &dstrKID,
                                       &dstrChecksum,
                                       &dstrLAURL,
                                       &dstrLUIURL,
                                       &dstrDSID,
                                       &dstrCustomAttrs,
                                       &dstrDecryptorSetup,
                                        pbV4Header,
                                       &cbV4Header ) );
    }

    /*
    ** Compare result with expected
    */
    ChkBOOL( cbV4Header == cbV4ExpectedHeader, DRM_E_FAIL );
    ChkBOOL( 0 == MEMCMP( pbV4Header, pbV4ExpectedHeader, cbV4Header ), DRM_E_FAIL );

ErrorExit:

    FREEDRMSTRING( dstrKID );
    FREEDRMSTRING( dstrChecksum );
    FREEDRMSTRING( dstrLAURL );
    FREEDRMSTRING( dstrLUIURL );
    FREEDRMSTRING( dstrDSID );
    FREEDRMSTRING( dstrCustomAttrs );
    FREEDRMSTRING( dstrDecryptorSetup );

    SAFE_OEM_FREE ( pbBuffer );
    SAFE_OEM_FREE ( pbV4Header );

    return dr;
}

/**********************************************************************
**
** Function: Test_DRM_PRO_CalculateRMHeaderChecksum
**
** Synopsis: Tests the conversion from a V2 header to a PlayReady object.
**
** Arguments:
**            IN argc     - number of arguments
**            IN argv[0]  - Cipher type. Valid values: AESCTR, COCKTAIL, INVALID
**            IN argv[1]  - B64-encoded content key.
**            IN argv[2]  - B64-encoded KID.
**            IN argv[3]  - Expected checksum, B64-encoded.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG for invalid arguments
**              DRM_E_FAIL if failed.
**
**********************************************************************/
DRM_RESULT DRM_CALL Test_DRM_PRO_CalculateRMHeaderChecksum(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrSource = { 0 };

    DRM_SUPPORTED_CIPHERS eCipherType = eDRM_CIPHER_INVALID;

    DRM_BYTE rgbContentKey[ DRMCIPHERKEYLEN_MAX ] = { 0 };
    DRM_DWORD cbContentKey = SIZEOF( rgbContentKey );

    DRM_KID oKID = { 0 };
    DRM_DWORD cbKID = SIZEOF( oKID );

    DRM_BYTE rgbExpectedChecksum[ max( CHECKSUM_LENGTH, DRM_AES_CK_CHECKSUM_LENGTH ) ] = { 0 };
    DRM_DWORD cbExpectedChecksum = SIZEOF( rgbExpectedChecksum );

    DRM_BYTE *pbChecksum = NULL;
    DRM_DWORD cbChecksum = 0;

    /*
    ** All parameters should be provided.
    */
    ChkArg( argc >= 4 );

    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );
    ChkArg( argv[ 2 ] != NULL );
    ChkArg( argv[ 3 ] != NULL );

    /*
    ** Cipher type
    */
    if ( 0 == DRMCRT_strncmp( argv[ 0 ], "AESCTR", DRMCRT_strlen( argv[ 0 ] ) ) )
    {
        eCipherType = eDRM_AES_COUNTER_CIPHER;
    }
    else if ( 0 == DRMCRT_strncmp( argv[ 0 ], "COCKTAIL", DRMCRT_strlen( argv[ 0 ] ) ) )
    {
        eCipherType = eDRM_RC4_CIPHER;
    }

    /*
    ** Content Key
    */
    dasstrSource.m_ich = 0;
    dasstrSource.m_cch = (DRM_DWORD) strlen( argv[ 1 ] );
    ChkDR( DRM_B64_DecodeA( argv[ 1 ], &dasstrSource, &cbContentKey, rgbContentKey, 0 ) );

    /*
    ** KID
    */
    dasstrSource.m_ich = 0;
    dasstrSource.m_cch = (DRM_DWORD) strlen( argv[ 2 ] );
    ChkDR( DRM_B64_DecodeA( argv[ 2 ], &dasstrSource, &cbKID, oKID.rgb, 0 ) );

    /*
    ** Expected checksum
    */
    dasstrSource.m_ich = 0;
    dasstrSource.m_cch = (DRM_DWORD) strlen( argv[ 3 ] );
    ChkDR( DRM_B64_DecodeA( argv[ 3 ], &dasstrSource, &cbExpectedChecksum, rgbExpectedChecksum, 0 ) );

    dr = DRM_PRO_CalculateRMHeaderChecksum(
                eCipherType,
                rgbContentKey,
                cbContentKey,
               &oKID,
                NULL,
               &cbChecksum );

    ChkBOOL( DRM_E_BUFFERTOOSMALL == dr, DRM_E_FAIL );

    ChkMem( pbChecksum = (DRM_BYTE *) Oem_MemAlloc( cbChecksum ) );
    ZEROMEM( pbChecksum, cbChecksum );

    ChkDR( DRM_PRO_CalculateRMHeaderChecksum(
                eCipherType,
                rgbContentKey,
                cbContentKey,
               &oKID,
                pbChecksum,
               &cbChecksum ) );

    /*
    ** Compare result with expected
    */
    ChkBOOL( cbChecksum == cbExpectedChecksum, DRM_E_FAIL );
    ChkBOOL( 0 == MEMCMP( pbChecksum, rgbExpectedChecksum, cbChecksum ), DRM_E_FAIL );

ErrorExit:

    return dr;
}

BEGIN_TEST_API_LIST( Header )
    API_ENTRY(TestLoadHeader)
    API_ENTRY(TestLoadWideCharHeader)
    API_ENTRY(TestContentHeaderGetUplink)
    API_ENTRY(TestContentHeaderGetAttribute)
    API_ENTRY(TestContentHeaderVerify)
    API_ENTRY(TestGetHeaderVersion)
    API_ENTRY(Test_HB_CreateHeader)
    API_ENTRY(Test_HB_CloseHeader)
    API_ENTRY(Test_HB_ReallocHeader)
    API_ENTRY(Test_HB_GetContextSize)
    API_ENTRY(Test_HB_OpenNode)
    API_ENTRY(Test_HB_CloseCurrNode)
    API_ENTRY(Test_HB_AddData)
    API_ENTRY(Test_HB_SetKeyID)
    API_ENTRY(Test_HB_SetLicAcqUrl)
    API_ENTRY(Test_HB_SetChecksum)
    API_ENTRY(Test_HB_SetAttribute)
    API_ENTRY(TestPlayReadyObjectParser)
    API_ENTRY(Test_DRM_PRO_GetRecord)
    API_ENTRY(Test_DRM_PRO_IsValidObject)
    API_ENTRY(Test_DRM_PRO_CreatePlayReadyDRMHeader)
    API_ENTRY(Test_DRM_PRO_ConvertV2HeaderToPlayReady)
    API_ENTRY(Test_Drm_PlayReadyObject_ConvertFromWmdrmHeader)
    API_ENTRY(Test_Drm_ConvertFileFromWmdrmToPlayReady)
    API_ENTRY(TestCreateHeader)
    API_ENTRY(Test_DRM_PRO_CalculateRMHeaderChecksum)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
