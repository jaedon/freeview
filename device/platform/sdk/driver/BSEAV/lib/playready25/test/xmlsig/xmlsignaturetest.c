/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmxmlparser.h>
#include <drmxmlbuildera.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <drmsoapxmlutility.h>
#include <XMLSigTestAPI.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

/*
** Test-related constant strings.
*/

/* "Root" */
const DRM_WCHAR g_rgwchTestRootNode [] =
{
    ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 't', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrTestRootNode = CREATE_DRM_STRING( g_rgwchTestRootNode );

const DRM_ANSI_CONST_STRING g_dastrTestRootNode = CREATE_DRM_ANSI_STRING( "Root" );

/*
<Root>
    <KeyInfo ...>
        <KeyName>WMRMServer</KeyName>
    </KeyInfo>
</Root>
*/
const DRM_ANSI_CONST_STRING g_dastrTestKeyIdXMLDoc = CREATE_DRM_ANSI_STRING( "<Root><KeyInfo xmlns=\"http://www.w3.org/2000/09/xmldsig#\"><KeyName>WMRMServer</KeyName></KeyInfo></Root>" );

/* "KeyInfo/KeyValue/ECCKeyValue/PublicKey" */

const DRM_ANSI_CONST_STRING g_dastrTestPublicKeyPath = CREATE_DRM_ANSI_STRING( "KeyInfo/KeyValue/ECCKeyValue/PublicKey" );

/*
<Root>
    <KeyInfo>
        <KeyValue>
            <ECCKeyValue>
                <PublicKey>
                ...
                </PublicKey>
            </ECCKeyValue>
        </KeyValue>
    </KeyInfo>
</Root>
*/
const DRM_WCHAR g_rgwchTestECC256PublicKeyXMLDoc [] =
{
    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'o', '\0' ),
    ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'f', '\0' ), ONE_WCHAR( 'o', '\0' ),
    ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'u', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'C', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'V', '\0' ),
    ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'u', '\0' ), ONE_WCHAR( 'e', '\0' ),
    ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '<', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'u', '\0' ),
    ONE_WCHAR( 'b', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'c', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'u', '\0' ),
    ONE_WCHAR( 'b', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'c', '\0' ),
    ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'C', '\0' ),
    ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'y', '\0' ),
    ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'u', '\0' ),
    ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'l', '\0' ),
    ONE_WCHAR( 'u', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'e', '\0' ),
    ONE_WCHAR( 'y', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'f', '\0' ),
    ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( '>', '\0' ),

    ONE_WCHAR( '<', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'o', '\0' ),
    ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '>', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrTestECC256PublicKeyXMLDoc = CREATE_DRM_STRING( g_rgwchTestECC256PublicKeyXMLDoc );


const DRM_DWORD g_cchXMLBuilderInitialSize = 2000;

/************************************************************************************
**              Helper functions
************************************************************************************/
/***************************************************************************
**
** Function: _GetKeyInfoFormat
**
** Synopsis: Maps input integer values that come in argv[]
**           to DRM_XML_KEYINFO_TYPE
**
**
** Arguments:
**
** [f_cbKeyInfoFormat]  -- value to map.
**
** Returns:             DRM_XML_KEYINFO_TYPE
**
***************************************************************************/
static DRM_XML_KEYINFO_TYPE _GetKeyInfoFormat(
    __in     DRM_DWORD  f_cbKeyInfoFormat )
{
    if ( f_cbKeyInfoFormat == 1 )
    {
        return DRM_XML_KEYINFO_SIMPLE;
    }
    else if ( f_cbKeyInfoFormat == 2 )
    {
        return DRM_XML_KEYINFO_ENCRYPTED;
    }
    else
    {
        return DRM_XML_KEYINFO_INVALID;
    }
}

/***************************************************************************
**
** Function: _GetECC256KeyFormat
**
** Synopsis: Maps input integer values that come in argv[]
**           to DRM_XML_ECC256_KEY_FORMAT
**
**
** Arguments:
**
** [f_cbKeyInfoFormat]  -- value to map.
**
** Returns:             DRM_XML_ECC256_KEY_FORMAT
**
***************************************************************************/
static DRM_XML_ECC256_KEY_FORMAT _GetECC256KeyFormat(
    __in     DRM_DWORD  f_cbECC256Format )
{
    if ( f_cbECC256Format == 1 )
    {
        return DRM_XML_ECC256_PUBLIC_KEY_DATA;
    }
    else if ( f_cbECC256Format == 2 )
    {
        return DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID;
    }
    else if ( f_cbECC256Format == 3 )
    {
        return DRM_XML_ECC256_CLIENT_PUBLIC_KEY_ID;
    }
    else
    {
        return DRM_XML_ECC256_INVALID_KEY;
    }
}

/***************************************************************************
**
** Function: _GetECC256KeysImitation
**
** Synopsis: Temporary generator of public-private key pair
**           for test purpose.
**
** Arguments:
**
** [f_poDevPubKey]  -- pointer to a public key,
** [f_poDevPrivKey] -- pointer to a private key, one of the pointers may be NULL
**
** Returns:             DRM_RESULT
**
***************************************************************************/
static DRM_RESULT _GetECC256KeysImitation(
    __out_opt PUBKEY_P256 *f_poDevPubKey,
    __out_opt PRIVKEY_P256 *f_poDevPrivKey  )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    ** Hard coded ECC-256 public key for test purpose.
    */
    PUBKEY_P256 oECC256PubKey =
    {
        0X54, 0Xf6, 0X7f, 0Xa5, 0Xda, 0X7c, 0X9f, 0X57,
        0X19, 0Xe9, 0X22, 0Xc8, 0Xc5, 0X0a, 0X16, 0X68,
        0Xd6, 0X22, 0X56, 0X92, 0Xbe, 0X6c, 0X60, 0Xaa,
        0X89, 0X94, 0X8e, 0X1a, 0Xcf, 0X79, 0X98, 0X37,
        0X8f, 0X6f, 0X4c, 0Xb5, 0X11, 0Xa4, 0Xaf, 0Xc1,
        0X11, 0Xe7, 0X1e, 0X81, 0Xee, 0X83, 0X53, 0X75,
        0X3b, 0X38, 0X4f, 0X46, 0X57, 0X4e, 0Xea, 0X6d,
        0Xa4, 0Xaa, 0Xfa, 0X21, 0X6b, 0X70, 0Xec, 0Xc7
    };

    /*
    ** Hard coded ECC-256 private key for test purpose.
    */
    PRIVKEY_P256 oECC256PrivKey =
    {
        0X6a, 0X53, 0X06, 0X55, 0X87, 0X6c, 0X8e, 0X82,
        0X57, 0X0e, 0X0b, 0X41, 0Xe6, 0X64, 0Xee, 0X2c,
        0X08, 0X39, 0Xde, 0Xa6, 0X7e, 0Xd6, 0Xd2, 0Xdc,
        0Xca, 0X59, 0Xd6, 0Xc3, 0X9a, 0X34, 0X5a, 0X64
    };

    ChkArg( f_poDevPubKey != NULL ||
            f_poDevPrivKey != NULL );

    if ( f_poDevPubKey != NULL )
    {
        ZEROMEM( f_poDevPubKey, SIZEOF( PUBKEY_P256 ) );

        MEMCPY( f_poDevPubKey, &oECC256PubKey, SIZEOF( PUBKEY_P256 ) );
    }

    if ( f_poDevPrivKey != NULL )
    {
        ZEROMEM( f_poDevPrivKey, SIZEOF( PRIVKEY_P256 ) );

        MEMCPY( f_poDevPrivKey, &oECC256PrivKey, SIZEOF( PRIVKEY_P256 ) );
    }

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( &oECC256PubKey, SIZEOF( PUBKEY_P256 ) );


    OEM_SECURE_ZERO_MEMORY( &oECC256PrivKey, SIZEOF( PRIVKEY_P256 ) );

    return dr;
}

/***************************************************************************
**
** Function: _CreateTestXMBContextA
**
** Synopsis: Creates ANSI version of XML builder with one root node.
**           TestDRM_XMLSIG_Build...A() functions will use it.
**
**
** Arguments:
**
** [f_ppoXMBA]   -- a pointer to a newly created XML builder context.
**
** Returns:                 DRM_RESULT
**
** Note: a caller must release memory allocated by this function
***************************************************************************/
static DRM_RESULT _CreateTestXMBContextA(
    __out _XMBContextA **f_ppoXMBA
    )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbSize    = 0;
    _XMBContextA *poXMBA = NULL;

    ChkArg( f_ppoXMBA != NULL );

    cbSize = g_cchXMLBuilderInitialSize * SIZEOF(DRM_CHAR);

    /* Create xml builder with <Root> top-level node */
    ChkMem( poXMBA = (_XMBContextA*)Oem_MemAlloc( cbSize ) );

    ChkDR( DRM_XMB_CreateDocumentA( cbSize,
                                   (DRM_BYTE*)poXMBA,
                                   &g_dastrTestRootNode ) );

ErrorExit:
    if( f_ppoXMBA != NULL )
    {
        *f_ppoXMBA = poXMBA;
    }

    return dr;
}


/************************************************************************************
**                  DRM_XMLSIG_Get...()
************************************************************************************/


/***************************************************************************
**
** Function: TestDRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount
**
** Synopsis: Function that calculates the size of a simple KeyInfo XML node
**           depending on a format of ECC-256 public key presentation
**           which is defined by argv[0]
**           and compares it with a number passed in argv[1].
**
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if calculated result did not match.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount(
    __in long argc,
    __in_ecount(argc) char **argv )

{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_DWORD  cchSize       = 0;

    ChkArg( argc == 2 );

    ChkDR( DRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount(
            _GetECC256KeyFormat( (DRM_DWORD)atol( argv[0] ) ),
            &cchSize ) );

    if ( (DRM_LONG) cchSize == atoi( argv[1] ) )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    return dr;

}


/***************************************************************************
**
** Function: TestDRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount
**
** Synopsis: Function that calculates the size of encrypted KeyInfo XML node,
**           argv[0] is a type of KeyInfo ( plain or encrypted ),
**           argv[1] is a format of ECC-256 public key presentation.
**           Calculated size is compared with a number passed in argv[2].
**
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if calculated result did not match.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/
DRM_RESULT DRM_CALL  TestDRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount(
    __in long argc,
    __in_ecount(argc) char **argv )

{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   cchSize = 0;
    DRM_XML_KEY oXMLKey = EMPTY_XML_KEY;

    ChkArg( argc == 3 );

    oXMLKey.m_eKeyInfoType = _GetKeyInfoFormat( (DRM_DWORD)atol( argv[0] ) );
    oXMLKey.m_eKeyFormat = _GetECC256KeyFormat( (DRM_DWORD)atol( argv[1] ) );

    ChkDR( DRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount( &oXMLKey, &cchSize ) );

    if ( (DRM_LONG) cchSize == atoi( argv[2] ) )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    return dr;

}


/***************************************************************************
**
** Function: TestDRM_XMLSIG_GetSignatureNodeCharCount
**
** Synopsis: Function that calculates the size of <Signature> tag
**           in XML signature and compares it with argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if calculated result did not match.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_XMLSIG_GetSignatureNodeCharCount(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  cchSize = 0;

    ChkArg( argc == 1 );

    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchSize ) );

    if ( (DRM_LONG) cchSize == atoi( argv[0] ) )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    return dr;
}


/***************************************************************************
**
** Function: TestDRM_XMLSIG_GetEncryptedDataNodeCharCount
**
** Synopsis: Function that calculates the size of <EncryptedData> tag.
**           String to put inside XML tag is passed in argv[0].
**           argv[1] is a type of KeyInfo ( plain or encrypted ),
**           argv[2] is a format of ECC256 public key presentation.
**           Calculated size is compared with a number passed in argv[3].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if calculated result did not match.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_XMLSIG_GetEncryptedDataNodeCharCount(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_DWORD   cchSize        = 0;
    DRM_XML_KEY oXMLKey        = EMPTY_XML_KEY;
    DRM_CONST_STRING  dstrData = EMPTY_DRM_STRING;
    DRM_DWORD   cbDataSize     = 0;

    ChkArg( argc == 4 );

    ChkDR( MakeDRMString( &dstrData, argv[0] ) );

    oXMLKey.m_eKeyInfoType = _GetKeyInfoFormat( (DRM_DWORD)atol( argv[1] ) );
    oXMLKey.m_eKeyFormat = _GetECC256KeyFormat( (DRM_DWORD)atol( argv[2] ) );

    cbDataSize = CB_DSTR( &dstrData );

    ChkDR( DRM_XMLSIG_GetEncryptedDataNodeCharCount( &oXMLKey, cbDataSize, &cchSize ) );

    if ( (DRM_LONG) cchSize == atoi( argv[3] ) )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    FREEDRMSTRING( dstrData );
    return dr;
}

/************************************************************************************
**                  DRM_XMLSIG_Build...()
************************************************************************************/

/***************************************************************************
**
** Function: TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyName
**
** Synopsis: Function that creates a simple <KeyInfo> with ECC-256 key
**           and compares it with a pre-built string. ECC-256 key is pre-shared.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if verifications fail.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyName(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT             dr              = DRM_SUCCESS;
    _XMBContextA*          poXMBA          = NULL;
    PUBKEY_P256 oECC256PubKey = { 0 };
    DRM_SUBSTRING          dasstrXML         = EMPTY_DRM_SUBSTRING;

    /*
    ** Create xml builder with <Root> top-level node
    */
    ChkDR( _CreateTestXMBContextA( &poXMBA ) );

    /*
    ** Call API
    */
    ChkDR( DRM_XMLSIG_BuildSimpleECC256KeyInfoNode(
                                poXMBA,
                                &oECC256PubKey,
                                DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID ) );

    ChkDR( DRM_XMB_CloseDocumentA( poXMBA, &dasstrXML ) );

    /*
    ** compare results
    */
    if ( !DRM_UTL_DASSTRStringsEqual( poXMBA->rgchXML,
                                      &dasstrXML,
                                      &g_dastrTestKeyIdXMLDoc ) )
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    SAFE_OEM_FREE( poXMBA );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyValue
**
** Synopsis: Function that creates a simple <KeyInfo> with ECC-256 key
**           and verifies that piblic key tag is present.
**           ECC-256 key params are put into XML string.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if verifications fail.
**                          DRM_E_INVALIDARG if the arguments are invalid.
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyValue(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT    dr             = DRM_SUCCESS;
    _XMBContextA* poXMBA         = NULL;
    DRM_SUBSTRING dasstrXML      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrPublicKey  = EMPTY_DRM_SUBSTRING;
    PUBKEY_P256 oECC256PubKey = { 0 };

    /*
    ** initialize a public key
    */
    ChkDR( _GetECC256KeysImitation( &oECC256PubKey, NULL ) );

    /*
    ** Create xml builder with <Root> top-level node
    */
    ChkDR( _CreateTestXMBContextA( &poXMBA ) );

    /* Call API */
    ChkDR( DRM_XMLSIG_BuildSimpleECC256KeyInfoNode( poXMBA,
                                                     &oECC256PubKey,
                                                     DRM_XML_ECC256_PUBLIC_KEY_DATA ) );

    ChkDR( DRM_XMB_CloseDocumentA( poXMBA, &dasstrXML ) );

    /*Verify that module node is present and its length is correct */
    ChkDR( DRM_XML_GetSubNodeByPathA( poXMBA->rgchXML,
                                      &dasstrXML,
                                      &g_dastrTestPublicKeyPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrPublicKey,
                                      g_chForwardSlash ) );

    if ( dasstrPublicKey.m_cch != CCH_BASE64_EQUIV( ECC_P256_POINT_SIZE_IN_BYTES ) )
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    SAFE_OEM_FREE( poXMBA );

    return dr;
}


/***************************************************************************
**
** Function: TestDRM_XMLSIG_BuildSignatureNode
**
** Synopsis: Function that creates a <Signature> tag,
**           verifes that <SignedInfo>,
**           <SignatureValue> and <KeyInfo> tags are present
**           Data string used to calculare digest is in argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if verification fails.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/

DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildSignatureNode(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr            = DRM_SUCCESS;
    _XMBContextA          *poXMBA       = NULL;
    DRM_SUBSTRING         dasstrXML     = EMPTY_DRM_SUBSTRING;
    DRM_CHAR              szInput[1200] = {0};
    DRM_SUBSTRING         dasstrInput   = EMPTY_DRM_SUBSTRING;
    DRM_BB_CONTEXT        *poBBXContext = NULL;

    ChkArg( argc == 1 );
    dasstrInput.m_ich = 0;
    dasstrInput.m_cch = ( DRM_DWORD ) min( strlen(argv[0]), SIZEOF( szInput ) );
    MEMCPY( szInput, argv[0], dasstrInput.m_cch );

    /*
    ** Create xml builder with <Root> top-level node
    */
    ChkDR( _CreateTestXMBContextA( &poXMBA ) );

    ChkDR( tGetBBContext( &poBBXContext ) );

    ChkDR( DRM_XMLSIG_BuildSignatureNode( poXMBA,
                                          poBBXContext,
                                          szInput,
                                          &dasstrInput ) );

    ChkDR( DRM_XMB_CloseDocumentA( poXMBA, &dasstrXML ) );

ErrorExit:
    tFreeBBContext( poBBXContext );

    SAFE_OEM_FREE( poXMBA );

    return dr;
}

/***************************************************************************
**
** Function: TestDRM_XMLSIG_BuildEncryptedDataNode
**
** Synopsis: Function that creates encrypted <EncryptedData> tag,
**           verifes that <EncryptionMethod>
**           and <CipherData><CipherValue> tags are present,
**           input string has been 64-base encoded and placed
**           inside <CipherValue> tag.
**           Data string to put into <CipherValue> tag is in argv[0]
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_FAIL if verification fails.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/

DRM_RESULT DRM_CALL TestDRM_XMLSIG_BuildEncryptedDataNode(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT            dr                      = DRM_SUCCESS;
    _XMBContextA          *poXMBA                 = NULL;
    DRM_XML_KEY           oXMLKey                 = EMPTY_XML_KEY;
    DRM_SUBSTRING         dasstrXML               = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrOutput            = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrEncryptedDataNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrNode              = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrData              = EMPTY_DRM_SUBSTRING;
    DRM_CHAR              szData[500]             = {0};
    DRM_ANSI_CONST_STRING dastrInputData          = EMPTY_DRM_STRING;
    DRM_CHAR              *pchXML                 = NULL;
    DRM_ANSI_CONST_STRING dastrB64EncodedStr      = {0};
    DRM_CHAR              *pchData                = NULL;
    DRM_DWORD             cchData                 = 0;
    DRM_CRYPTO_CONTEXT    *pcryptoCtx             = NULL;

    ChkArg( argc == 1 );

    ChkMem( pcryptoCtx = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    dastrInputData.cchString = ( DRM_DWORD ) min( strlen( argv[0] ), SIZEOF( szData ) );
    MEMCPY( szData, argv[0], dastrInputData.cchString );
    dastrInputData.pszString = szData;

    ChkDR( DRM_SOAPXML_PadANSIString( ( DRM_ANSI_STRING * )&dastrInputData, DRM_AES_BLOCKLEN ) );

    /*
    ** Create xml builder with <Root> top-level node
    */
    ChkDR( _CreateTestXMBContextA( &poXMBA ) );

    /*
    ** encrypt input data with ECC-256/AES key and encrypt AES key itself
    */
    ChkDR( DRM_SOAPXML_InitXMLKey( NULL, &oXMLKey, pcryptoCtx ) );
    ChkDR( DRM_SOAPXML_EncryptDataWithXMLKey( NULL, &oXMLKey, &dastrInputData, pcryptoCtx ) );

    ChkDR( DRM_XMB_GetCurrentBufferPointerA( poXMBA,
                                             FALSE,
                                             &pchXML ) );

    DRM_BYT_MoveBytes( pchXML, 0, dastrInputData.pszString, 0, dastrInputData.cchString );

    ChkDR( DRM_XMLSIG_BuildEncryptedDataNode( poXMBA,
                                              &oXMLKey,
                                              dastrInputData.cchString,
                                              &dasstrOutput ) );

    ChkDR( DRM_XMB_CloseDocumentA( poXMBA, &dasstrXML ) );

    /*
    ** Verify that <EncryptedData> is present under <Root> node
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( poXMBA->rgchXML,
                                     &dasstrXML,
                                 &g_dastrSigEncryptedDataTag,
                                 NULL,
                                 NULL,
                                 &dasstrEncryptedDataNode,
                                 NULL,
                                 g_chForwardSlash ) );

    /*
    ** Verify that <EncryptionMethod> is present
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( poXMBA->rgchXML,
                                      &dasstrEncryptedDataNode,
                                      &g_dastrSigEncryptionMethodTag,
                                      NULL,
                                      NULL,
                                      &dasstrNode,
                                      NULL,
                                      g_chForwardSlash ) );

    /*
    ** Verify that <CipherData><CipherValue> is present
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( poXMBA->rgchXML,
                                       &dasstrEncryptedDataNode,
                                 &g_dastrSigCipherValuePath,
                                 NULL,
                                 NULL,
                                 &dasstrNode,
                                 &dasstrData,
                                 g_chForwardSlash ) );

    /*
    ** Verify that data inside CipherValue is a 64-base encoded input string
    */
    cchData = CCH_BASE64_EQUIV( dastrInputData.cchString );

    ChkMem( pchData = ( DRM_CHAR * )Oem_MemAlloc( cchData * SIZEOF( DRM_CHAR ) ) );

    ChkDR( DRM_B64_EncodeA( (const DRM_BYTE *)szData,
                            dastrInputData.cchString,
                            pchData,
                            &cchData,
                            0 ) );
    dastrB64EncodedStr.pszString = pchData;
    dastrB64EncodedStr.cchString = cchData;

    if ( !DRM_UTL_DASSTRStringsEqual( poXMBA->rgchXML, &dasstrData, &dastrB64EncodedStr ) )
    {
        ChkDR(DRM_E_FAIL);
    }

ErrorExit:
    SAFE_OEM_FREE( pcryptoCtx );
    SAFE_OEM_FREE( poXMBA );
    SAFE_OEM_FREE( pchData );

    return dr;
}

/***************************************************************************
**
** Function: TestDRM_XMLSIG_ExtractSimpleECC256Key
**
** Synopsis: Function parses a simple <KeyInfo>
**           and extracts public key iformation.
**           Data string to be parsed is in argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/

DRM_RESULT DRM_CALL TestDRM_XMLSIG_ExtractSimpleECC256Key(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT    dr           = DRM_SUCCESS;
    DRM_CHAR      szBase[1200] = { 0 };
    DRM_SUBSTRING dasstrInput  = EMPTY_DRM_SUBSTRING;
    PUBKEY_P256 oECC256PubKey = { 0 };

    ChkArg( argc <= 2 );
    dasstrInput.m_ich = 0;
    dasstrInput.m_cch = ( DRM_DWORD ) min( strlen( argv[0] ), SIZEOF( szBase ) );
    MEMCPY( szBase, argv[0], dasstrInput.m_cch );

    ChkDR( DRM_XMLSIG_ExtractSimpleECC256Key( szBase, &dasstrInput, &oECC256PubKey ) );

ErrorExit:
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_XMLSIG_VerifySignature
**
** Synopsis: Function receives a string that was signed
**           and its signature, calls signature verification.
**           Data string that was signed is in argv[0],
**           signature string is in argv[1].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
***************************************************************************/

DRM_RESULT DRM_CALL TestDRM_XMLSIG_VerifySignature(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT          dr                = DRM_SUCCESS;
    DRM_CHAR            szData[1200]      = { 0 };
    DRM_CHAR            szSignature[1200] = { 0 };
    DRM_SUBSTRING       dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrSignature   = EMPTY_DRM_SUBSTRING;
    DRM_BB_CONTEXT      *poBBXCtx         = NULL;

    ChkArg( argc == 2 );
    MAP_TO_GLOBAL_TEST_VAR( poBBXCtx, XMLSIG_BBXCTX_VAR );

    dasstrData.m_ich = 0;
    dasstrData.m_cch = ( DRM_DWORD ) min( strlen( argv[0] ), SIZEOF( szData ) ) ;
    MEMCPY( szData, argv[0], dasstrData.m_cch );

    dasstrSignature.m_ich = 0;
    dasstrSignature.m_cch = ( DRM_DWORD ) min( strlen( argv[1] ), SIZEOF( szSignature ) );
    MEMCPY( szSignature, argv[1], dasstrSignature.m_cch );

    ChkDR( DRM_XMLSIG_VerifySignature( szData,
                                       &dasstrData,
                                       szSignature,
                                       &dasstrSignature,
                                       &g_oWMRMECC256PubKey,
                                       &poBBXCtx->CryptoContext ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: PreTestCase
**
** Synopsis: Function that initializes the environment before the running
**           of each test case.
**
** Arguments:
**
** [lTCID]                  -- Test case ID.
** [strTCName]              -- Test case name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE(XMLSig)(long lTCID, const char *strTCName)
{
    DRM_RESULT      dr          = DRM_SUCCESS;
    DRM_BB_CONTEXT  *poBBXCtx   = NULL;

    /*
    ** Set blackbox context shared variables
    */
    Log( "", "Setting XMLSIG_BBXCTX_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poBBXCtx, XMLSIG_BBXCTX_VAR, SIZEOF( DRM_BB_CONTEXT ) );
    ZEROMEM( poBBXCtx, SIZEOF( DRM_BB_CONTEXT ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: PostTestCase
**
** Synopsis: Function that cleans up the environment after the running
**           of each test case.
**
**           Any dynamically allocated memory is released here.
**
** Arguments:
**
** [lTCID]                  -- Test case ID.
** [strTCName]              -- Test case name.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(XMLSig)(long lTCID, const char *strTCName)
{
    DRM_RESULT      dr          = DRM_SUCCESS;
    DRM_BB_CONTEXT  *poBBXCtx   = NULL;

    MAP_TO_GLOBAL_TEST_VAR( poBBXCtx, XMLSIG_BBXCTX_VAR );
    ZEROMEM( poBBXCtx, SIZEOF( DRM_BB_CONTEXT ) );

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( XMLSig )
    API_ENTRY(TestDRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount)
    API_ENTRY(TestDRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount)
    API_ENTRY(TestDRM_XMLSIG_GetSignatureNodeCharCount)
    API_ENTRY(TestDRM_XMLSIG_GetEncryptedDataNodeCharCount)
    API_ENTRY(TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyName)
    API_ENTRY(TestDRM_XMLSIG_BuildSimpleECC256KeyInfoKeyValue)
    API_ENTRY(TestDRM_XMLSIG_BuildSignatureNode)
    API_ENTRY(TestDRM_XMLSIG_BuildEncryptedDataNode)
    API_ENTRY(TestDRM_XMLSIG_ExtractSimpleECC256Key)
    API_ENTRY(TestDRM_XMLSIG_VerifySignature)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

