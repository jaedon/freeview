/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <stdio.h>
#include <time.h>
#include <drmcommon.h>
#include <drmcrt.h>
#include <drmstkalloc.h>
#include <oempkcrypto.h>
#include <oemcommon.h>

#include <drmbcert.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <TestBCert.h>

#include <drmutilities.h>
#include <drmtoolsconstants.h>
#include <drmtoolsutils.h>
#include <drmxmlparser.h>
#include <drmxmlbuilder.h>
#include <drmxmlrsakeys.h>
#include <drmwmdrmnet.h>

ENTER_PKTEST_NAMESPACE_CODE

static DRM_CRYPTO_CONTEXT     g_CryptoCtx         = {0};


/* PUBKEY/Modulus */
const DRM_WCHAR g_rgwchPUBKEYModulus [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'u', '\0' ), ONE_WCHAR( 'l', '\0' ), ONE_WCHAR( 'u', '\0' ), ONE_WCHAR( 's', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPUBKEYModulus = CREATE_DRM_STRING( g_rgwchPUBKEYModulus );

/* PUBKEY/Exponent */
const DRM_WCHAR g_rgwchPUBKEYExponent [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'U', '\0' ), ONE_WCHAR( 'B', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPUBKEYExponent = CREATE_DRM_STRING( g_rgwchPUBKEYExponent );

/* PRIVKEY/Prime0 */
const DRM_WCHAR g_rgwchPRIVKEYPrime0 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEYPrime0 = CREATE_DRM_STRING( g_rgwchPRIVKEYPrime0 );

/* PRIVKEY/Prime1 */
const DRM_WCHAR g_rgwchPRIVKEYPrime1 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEYPrime1 = CREATE_DRM_STRING( g_rgwchPRIVKEYPrime1 );

/* PRIVKEY/CRTExponent0 */
const DRM_WCHAR g_rgwchPRIVKEYCRTExponent0 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEYCRTExponent0 = CREATE_DRM_STRING( g_rgwchPRIVKEYCRTExponent0 );

/* PRIVKEY/CRTExponent1 */
const DRM_WCHAR g_rgwchPRIVKEYCRTExponent1 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEYCRTExponent1 = CREATE_DRM_STRING( g_rgwchPRIVKEYCRTExponent1 );

/* PRIVKEY/IQMP */
const DRM_WCHAR g_rgwchPRIVKEYIQMP [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'Q', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEYIQMP = CREATE_DRM_STRING( g_rgwchPRIVKEYIQMP );

/*******************************************************************************
** Function:    _GetSubNodeByPath
**
** Synopsis:    Get subnode XML data by specified XML path
**
** Arguments:   [pdstrXML]     : pointer to XML data to parse
**              [pdstrXMLPath] : pointer string with XML path
**              [pdstrKey]     : pointer to result XML data
**
** Returns:     DRM_SUCCESS   if data were found
**
** Note:        Function verifies length of the XML path first
**              then gets subnode
**
*******************************************************************************/
static DRM_RESULT _GetSubNodeByPath(
    __in                    const DRM_CONST_STRING *pdstrXML,
    __in                    const DRM_CONST_STRING *pdstrXMLPath,
    __inout                       DRM_CONST_STRING *pdstrKey )
{
    DRM_RESULT       dr           = DRM_SUCCESS;

    DRMASSERT( pdstrXML     != NULL );
    DRMASSERT( pdstrXMLPath != NULL );
    DRMASSERT( pdstrKey     != NULL );

    ChkDR(DRM_XML_GetSubNodeByPath( pdstrXML,
                                    pdstrXMLPath,
                                    NULL,
                                    NULL,
                                    NULL,
                                    pdstrKey,
                                    g_wchForwardSlash) );
ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    _GetSubNodeByPathA
**
** Synopsis:    Get subnode XML data by specified XML path (UTF8/ANSI).
**
** Arguments:   [pchXML]      : pointer to XML data to parse.
**              [pdasstrXML]  : pointer to a substring with of the XML data.
**              [pchXMLPath]  : pointer to the XML path.
**              [cchXMLPath]  : Number of characters in the XML path.
**              [pdasstrKey]  : pointer to a substring to received the key.
**
** Returns:     DRM_SUCCESS   if data were found.
**
*******************************************************************************/
static DRM_RESULT _GetSubNodeByPathA(
    __in const DRM_CHAR *pchXML,
    __in const DRM_SUBSTRING *pdasstrXML,
    __in const DRM_CHAR *pchXMLPath,
    __in const DRM_DWORD cchXMLPath,
    __inout DRM_SUBSTRING *pdasstrKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrXMLPath = EMPTY_DRM_STRING;

    ChkArg( pchXML != NULL );
    ChkArg( pdasstrXML != NULL );
    ChkArg( pchXMLPath != NULL && cchXMLPath > 0 );
    ChkArg( pdasstrKey != NULL );

    dastrXMLPath.pszString = pchXMLPath;
    dastrXMLPath.cchString = cchXMLPath;

    ChkDR( DRM_XML_GetSubNodeByPathA( pchXML,
                                      pdasstrXML,
                                      &dastrXMLPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      pdasstrKey,
                                      g_wchForwardSlash ) );
ErrorExit:

    return dr;
}

#define     XML_KEY_GEN_INIT_SIZE   1024*10

/*******************************************************************************
** Function:    TestConversionRSAKeyToXML
**
** Synopsis:    Tests conversion of RSA to XML (In Unicode) form and back.
**
** Arguments:   [poRSAPubKey]  : pointer to RSA public key
**              [poRSAPrivKey] : pointer to RSA private key
**
** Returns:     DRM_SUCCESS      if conversion was successful
**
*******************************************************************************/
static DRM_RESULT TestConversionRSAKeyToXML(
    __in DRM_RSA_PUBLIC_KEY  *poRSAPubKey,
    __in DRM_RSA_PRIVATE_KEY *poRSAPrivKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RSA_PUBLIC_KEY  oRSAPubKeyTest = { 0 };
    DRM_RSA_PRIVATE_KEY oRSAPrivKeyTest = { 0 } ;
    _XMBContext        *pbXML = NULL;
    DRM_CONST_STRING    dstrClose = EMPTY_DRM_STRING;

    /* Temp data for RSA public/private key parsing */
    DRM_CONST_STRING dstrPubKeyModulus       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPubKeyExponent      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyPrime0       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyPrime1       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyCRTExponent0 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyCRTExponent1 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyIQMP         = EMPTY_DRM_STRING;

    ChkArg( poRSAPubKey != NULL );
    ChkArg( poRSAPrivKey != NULL );

    ZEROMEM( &oRSAPubKeyTest,  SIZEOF( DRM_RSA_PUBLIC_KEY ) );
    ZEROMEM( &oRSAPrivKeyTest, SIZEOF( DRM_RSA_PRIVATE_KEY ) );

    ChkMem( pbXML = (_XMBContext *) Oem_MemAlloc( XML_KEY_GEN_INIT_SIZE ) );
    ZEROMEM( pbXML, XML_KEY_GEN_INIT_SIZE );

    ChkDR( DRM_XMB_CreateDocument( XML_KEY_GEN_INIT_SIZE,
                                   pbXML,
                                   &g_dstrTagData ) );

    ChkDR( DRM_XML_RSA_WritePublicKeyNode( pbXML,
                                           &g_dstrTagPubkey,
                                           poRSAPubKey ) );

    ChkDR( DRM_XML_RSA_WritePrivateKeyNode( pbXML,
                                            &g_dstrTagPrivkey,
                                            poRSAPrivKey ) );

    ChkDR( DRM_XMB_CloseDocument( pbXML, (DRM_STRING*)&dstrClose ) );

    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPUBKEYModulus,       &dstrPubKeyModulus ) );
    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPUBKEYExponent,      &dstrPubKeyExponent ) );
    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPRIVKEYPrime0,       &dstrPrivKeyPrime0 ) );
    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPRIVKEYPrime1,       &dstrPrivKeyPrime1 ) );
    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPRIVKEYCRTExponent0, &dstrPrivKeyCRTExponent0 ) );
    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPRIVKEYCRTExponent1, &dstrPrivKeyCRTExponent1 ) );
    ChkDR( _GetSubNodeByPath( &dstrClose, &g_dstrPRIVKEYIQMP,         &dstrPrivKeyIQMP ) );

    ChkDR( DRM_XML_RSA_ParseBase64PublicKey( &dstrPubKeyModulus,
                                             &dstrPubKeyExponent,
                                             &oRSAPubKeyTest ) );

    ChkDR( DRM_XML_RSA_ParseBase64PrivateKey( &dstrPrivKeyPrime0,
                                              &dstrPrivKeyPrime1,
                                              &dstrPrivKeyCRTExponent0,
                                              &dstrPrivKeyCRTExponent1,
                                              &dstrPrivKeyIQMP,
                                              &oRSAPrivKeyTest ) );

    ChkBOOL( MEMCMP( poRSAPubKey , &oRSAPubKeyTest,  SIZEOF( DRM_RSA_PUBLIC_KEY  ) ) == 0, DRM_E_KEY_MISMATCH );
    ChkBOOL( MEMCMP( poRSAPrivKey, &oRSAPrivKeyTest, SIZEOF( DRM_RSA_PRIVATE_KEY ) ) == 0, DRM_E_KEY_MISMATCH );

ErrorExit:
    SAFE_OEM_FREE( pbXML );
    return dr;
}

/*******************************************************************************
** Function:    TestConversionRSAKeyToXML_A
**
** Synopsis:    Tests conversion of RSA to XML (in UTF8/ANSI ) form and back.
**
** Arguments:   [poRSAPubKey]  : pointer to RSA public key
**
** Returns:     DRM_SUCCESS      if conversion was successful
**
** Note:        Currently there is no UTF8/ANSI version of the pasring/writing
**              functions for RSA private key.
**
*******************************************************************************/
static DRM_RESULT TestConversionRSAKeyToXML_A(
    __in DRM_RSA_PUBLIC_KEY *poRSAPubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RSA_PUBLIC_KEY oRSAPubKeyTest = { 0 };
    _XMBContextA *pbXML = NULL;
    DRM_ANSI_CONST_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrClose = EMPTY_DRM_SUBSTRING;

    DRM_CHAR rgchPubKeyModulus[] = "PUBKEY/Modulus";
    DRM_CHAR rgchPubKeyExponent[] = "PUBKEY/Exponent";

    DRM_ANSI_CONST_STRING dastrTagData = CREATE_DRM_ANSI_STRING( "DATA" );
    DRM_ANSI_CONST_STRING dastrTagPubkey = CREATE_DRM_ANSI_STRING( "PUBKEY" );

    /* Temp data for RSA public/private key parsing */
    DRM_SUBSTRING dasstrPubKeyModulus = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrPubKeyExponent = EMPTY_DRM_SUBSTRING;

    ChkArg( poRSAPubKey != NULL );

    ZEROMEM( &oRSAPubKeyTest, SIZEOF( DRM_RSA_PUBLIC_KEY ) );

    ChkMem( pbXML = ( _XMBContextA * )Oem_MemAlloc( XML_KEY_GEN_INIT_SIZE ) );
    ZEROMEM( pbXML, XML_KEY_GEN_INIT_SIZE );

    ChkDR( DRM_XMB_CreateDocumentA( XML_KEY_GEN_INIT_SIZE,
                                    ( DRM_BYTE * )pbXML,
                                    &dastrTagData ) );

    ChkDR( DRM_XML_RSA_WritePublicKeyNodeA( pbXML,
                                            &dastrTagPubkey,
                                            poRSAPubKey ) );

    ChkDR( DRM_XMB_CloseDocumentA( pbXML, &dasstrClose ) );

    ChkDR( _GetSubNodeByPathA( pbXML->rgchXML,
                               &dasstrClose,
                               rgchPubKeyModulus,
                               DRMCRT_strlen( rgchPubKeyModulus ),
                               &dasstrPubKeyModulus ) );

    ChkDR( _GetSubNodeByPathA( pbXML->rgchXML,
                               &dasstrClose,
                               rgchPubKeyExponent,
                               DRMCRT_strlen( rgchPubKeyExponent ),
                               &dasstrPubKeyExponent ) );

    DASTR_FROM_PB( &dastrXML, pbXML->rgchXML, pbXML->cchBuffer );

    ChkDR( DRM_XML_RSA_ParseBase64PublicKeyA( &dastrXML,
                                              &dasstrPubKeyModulus,
                                              &dastrXML,
                                              &dasstrPubKeyExponent,
                                              &oRSAPubKeyTest ) );

    ChkBOOL( MEMCMP( poRSAPubKey , &oRSAPubKeyTest,  SIZEOF( DRM_RSA_PUBLIC_KEY  ) ) == 0,
             DRM_E_KEY_MISMATCH );

ErrorExit:

    SAFE_OEM_FREE( pbXML );

    return dr;
}

/*******************************************************************************
** Function:    Test_RSAToXML
**
** Synopsis:    Tests convertion of RSA to XML form and back
**
**
** Notes:       No input argumenta are used
**
*******************************************************************************/
DRM_RESULT DRM_CALL Test_RSAToXML( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT          dr = DRM_SUCCESS;
    DRM_RSA_PUBLIC_KEY  oRSAPubKey  = { 0 };
    DRM_RSA_PRIVATE_KEY oRSAPrivKey = { 0 };

    ZEROMEM( &g_CryptoCtx, DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE );
    ZEROMEM( &oRSAPubKey,  SIZEOF( DRM_RSA_PUBLIC_KEY ) );
    ZEROMEM( &oRSAPrivKey, SIZEOF( DRM_RSA_PRIVATE_KEY ) );

    ChkDR( DRMTOOLS_CreateKeyPair( &g_CryptoCtx,
                                   eDRM_RSA,
                                   eDRM_RSA_1024,
                                   (DRM_BYTE*) &oRSAPubKey,
                                   DRM_RSA_CB_PUBKEY,
                                   (DRM_BYTE*) &oRSAPrivKey,
                                   DRM_RSA_CB_PRIVKEY,
                                   NULL,
                                   NULL ) );

    ChkDR( TestConversionRSAKeyToXML( &oRSAPubKey, &oRSAPrivKey ) );

/* RSA key length of 2048 is supported only if WMDRMNET is supported */
    if( DRM_WMDRMNET_IsWmdrmnetSupported() )
    {
        ZEROMEM( &g_CryptoCtx, DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE );
        ZEROMEM( &oRSAPubKey,  SIZEOF( DRM_RSA_PUBLIC_KEY ) );
        ZEROMEM( &oRSAPrivKey, SIZEOF( DRM_RSA_PRIVATE_KEY ) );

        ChkDR( DRMTOOLS_CreateKeyPair( &g_CryptoCtx,
                                       eDRM_RSA,
                                       eDRM_RSA_2048,
                                       (DRM_BYTE*) &oRSAPubKey,
                                       DRM_RSA_CB_PUBKEY,
                                       (DRM_BYTE*) &oRSAPrivKey,
                                       DRM_RSA_CB_PRIVKEY,
                                       NULL,
                                       NULL ) );

        ChkDR( TestConversionRSAKeyToXML( &oRSAPubKey, &oRSAPrivKey ) );

        ChkDR( TestConversionRSAKeyToXML_A( &oRSAPubKey ) );
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
