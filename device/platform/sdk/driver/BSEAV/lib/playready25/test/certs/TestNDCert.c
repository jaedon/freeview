/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdafx.h>
#include <drmcommon.h>

#include <drmndcertbuilder.h>
#include <drmwmdrmnet.h>
#include <drmxmlrsakeys.h>

#include <TestBCert.h>
#include <drmndcertconstants.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

#define DEFAULT_BUFFER_SIZE 10240

/* PRIVKEY/Prime0 */
const DRM_WCHAR g_rgwchPRIVKEY_Prime0 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEY_Prime0 = CREATE_DRM_STRING( g_rgwchPRIVKEY_Prime0 );

/* PRIVKEY/Prime1 */
const DRM_WCHAR g_rgwchPRIVKEY_Prime1 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'i', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEY_Prime1 = CREATE_DRM_STRING( g_rgwchPRIVKEY_Prime1 );

/* PRIVKEY/CRTExponent0 */
const DRM_WCHAR g_rgwchPRIVKEY_CRTExponent0 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '0', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEY_CRTExponent0 = CREATE_DRM_STRING( g_rgwchPRIVKEY_CRTExponent0 );

/* PRIVKEY/CRTExponent1 */
const DRM_WCHAR g_rgwchPRIVKEY_CRTExponent1 [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'C', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'T', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'x', '\0' ), ONE_WCHAR( 'p', '\0' ), ONE_WCHAR( 'o', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( 'n', '\0' ), ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '1', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEY_CRTExponent1 = CREATE_DRM_STRING( g_rgwchPRIVKEY_CRTExponent1 );

/* PRIVKEY/IQMP */
const DRM_WCHAR g_rgwchPRIVKEY_IQMP [] =  {
    ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( 'R', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'V', '\0' ), ONE_WCHAR( 'K', '\0' ), ONE_WCHAR( 'E', '\0' ), ONE_WCHAR( 'Y', '\0' ), ONE_WCHAR( '/', '\0' ), ONE_WCHAR( 'I', '\0' ), ONE_WCHAR( 'Q', '\0' ), ONE_WCHAR( 'M', '\0' ), ONE_WCHAR( 'P', '\0' ), ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrPRIVKEY_IQMP = CREATE_DRM_STRING( g_rgwchPRIVKEY_IQMP );

/*****************************************************************************
** Function:    _GetXMLValueByXPath
**
** Synopsis:    Gets XML values by XPath
**
** Arguments:   [f_pdstrXML]    : pointer to UTF-16 XML source
**              [f_pdstrXMLPath]: pointer to XPath string
**              [f_pdstrValue]  : pointer to DRM string that will contain result value
**                                DRM string will contain pointer into XML source
**
** Returns:     DRM_SUCCESS       - on success
**              DRM_E_INVALIDARG  - if any parameter is invalid
**                                  or other return code from a function called within
**
******************************************************************************/
static DRM_RESULT _GetXMLValueByXPath(
    __in                      const DRM_CONST_STRING *f_pdstrXML,
    __in                      const DRM_CONST_STRING *f_pdstrXMLPath,
    __inout                         DRM_CONST_STRING *f_pdstrValue )
{
    DRM_RESULT       dr           = DRM_SUCCESS;

    DRMASSERT( f_pdstrXML     != NULL );
    DRMASSERT( f_pdstrXMLPath != NULL );
    DRMASSERT( f_pdstrValue   != NULL );

    ChkDR(DRM_XML_GetSubNodeByPath( f_pdstrXML,
                                    f_pdstrXMLPath,
                                    NULL,
                                    NULL,
                                    NULL,
                                    f_pdstrValue,
                                    g_wchForwardSlash) );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _ParseRSAPrivateKey
**
** Synopsis:    Parses XML to get RSA private key
**
** Arguments:   [f_pdstrPrivateKey]: pointer to UTF-16 XML source containing RSA private key
**              [f_poPrivateKey]   : pointer RSA private key data structure that will contain
**                                   result of the parsing
**
** Returns:     DRM_SUCCESS       - on success
**
******************************************************************************/
static DRM_RESULT _ParseRSAPrivateKey(
    __in  const DRM_CONST_STRING            *f_pdstrPrivateKey,
    __out       DRM_RSA_PRIVATE_KEY_2048BIT *f_poPrivateKey )
{
    DRM_RESULT       dr                      = DRM_SUCCESS;
    DRM_CONST_STRING dstrPrivKeyPrime0       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyPrime1       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyCRTExponent0 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyCRTExponent1 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyIQMP         = EMPTY_DRM_STRING;

    ChkDRMString( f_pdstrPrivateKey);
    ChkArg( f_poPrivateKey != NULL );

    ChkDR( _GetXMLValueByXPath( f_pdstrPrivateKey, &g_dstrPRIVKEY_Prime0,       &dstrPrivKeyPrime0 ) );
    ChkDR( _GetXMLValueByXPath( f_pdstrPrivateKey, &g_dstrPRIVKEY_Prime1,       &dstrPrivKeyPrime1 ) );
    ChkDR( _GetXMLValueByXPath( f_pdstrPrivateKey, &g_dstrPRIVKEY_CRTExponent0, &dstrPrivKeyCRTExponent0 ) );
    ChkDR( _GetXMLValueByXPath( f_pdstrPrivateKey, &g_dstrPRIVKEY_CRTExponent1, &dstrPrivKeyCRTExponent1 ) );
    ChkDR( _GetXMLValueByXPath( f_pdstrPrivateKey, &g_dstrPRIVKEY_IQMP,         &dstrPrivKeyIQMP ) );

    /*
    **  Parse XML to get private key
    */
    ChkDR( DRM_XML_RSA_ParseBase64PrivateKey( &dstrPrivKeyPrime0,
                                              &dstrPrivKeyPrime1,
                                              &dstrPrivKeyCRTExponent0,
                                              &dstrPrivKeyCRTExponent1,
                                              &dstrPrivKeyIQMP,
                                              f_poPrivateKey ) );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _CreateCompanyTemplate
**
** Synopsis:    Construct a company template XML string using the passed in
**              security level and manufacturer name strings.
**
** Arguments:   [f_pdastrSecurityLevel]: Pointer to a DRM ANSI string that contains
                                         the security level.
**              [f_pdastrManufName]:     Pointer to a DRM ANSI string that contains
**                                       the manufacturer name.
**              [f_pdastrTemplate]:      Pointer to a DRM ANSI string to store the
**                                       generated template string. Caller should
**                                       allocate enough buffer upon entering the
**                                       function.
**
** Returns:     DRM_SUCCESS             - On success.
**              DRM_E_BUFFERTOOSMALL    - If the passed in buffer is not big enough
**                                        to store the generated template string.
**
******************************************************************************/
static DRM_RESULT _CreateCompanyTemplate(
    __in    const DRM_ANSI_CONST_STRING  *f_pdastrSecurityLevel,
    __in    const DRM_ANSI_CONST_STRING  *f_pdastrManufName,
    __inout       DRM_ANSI_CONST_STRING  *f_pdastrTemplate )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *pXMLContextA = NULL;
    DRM_CHAR *pchXMLData = NULL;
    DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

    const DRM_ANSI_CONST_STRING *rgpdastrNodes[ 1 ] = { NULL };
    const DRM_ANSI_CONST_STRING *rgpdastrValues[ 1 ] = { NULL };

    ChkDRMANSIString( f_pdastrSecurityLevel );
    ChkDRMANSIString( f_pdastrManufName );
    ChkDRMANSIString( f_pdastrTemplate );

    ChkMem( pXMLContextA = ( _XMBContextA * )Oem_MemAlloc( MAX_DEVICE_CERT_TEMPLATE_SIZE ) );
    ZEROMEM( pXMLContextA, MAX_DEVICE_CERT_TEMPLATE_SIZE );
    pXMLContextA->cbContext = MAX_DEVICE_CERT_TEMPLATE_SIZE;

    ChkDR( DRM_XMB_CreateDocumentA( pXMLContextA->cbContext,
                                    ( DRM_BYTE * )pXMLContextA,
                                    &g_dastrTagUnsignedTemplate ) );

    rgpdastrValues[ 0 ] = f_pdastrSecurityLevel;
    ChkDR( DRM_ND_CB_AddAttribute( pXMLContextA,
                                   DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL,
                                   rgpdastrValues,
                                   1 ) );

    rgpdastrValues[ 0 ] = &g_dastrKeyUsageSignCert;
    ChkDR( DRM_ND_CB_AddAttribute( pXMLContextA,
                                   DRM_WMDRM_CERT_ATTR_KEY_USAGE,
                                   rgpdastrValues,
                                   1 ) );

    rgpdastrNodes[ 0 ] = &g_dastrTagManufacturerName;
    rgpdastrValues[ 0 ] = f_pdastrManufName;

    ChkDR( DRM_ND_CB_AddManufacturerNodes( pXMLContextA,
                                           NULL,
                                           rgpdastrNodes,
                                           NULL,
                                           NULL,
                                           rgpdastrValues,
                                           1 ) );

    ChkDR( DRM_XMB_CloseDocumentA( pXMLContextA, &dasstr ) );

    ChkDR( DRM_XMB_GetXMLBaseA( pXMLContextA, &pchXMLData ) );

    ChkBOOL( f_pdastrTemplate->cchString >= dasstr.m_cch, DRM_E_BUFFERTOOSMALL );

    MEMCPY( ( DRM_BYTE * )f_pdastrTemplate->pszString, ( DRM_BYTE * )pchXMLData, dasstr.m_cch );
    f_pdastrTemplate->cchString = dasstr.m_cch;

ErrorExit:

    SAFE_OEM_FREE( pXMLContextA );

    return dr;
}

/*****************************************************************************
** Function:    Test_CreateDeviceCertificate
**
** Synopsis:    Test creation of a device certificate.
**
** Arguments:   argv[0]   : The name of the model certificate file
**              argv[1]   : The name of the model private key
**              argv[2]   : Optional, size of processing buffer
**              argv[3]   : Optional, filename for ND device certificate
**              argv[4]   : Optional, filename for ND device RSA private key, binary form
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_CreateDeviceCertificate( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT          dr                      = DRM_SUCCESS;
    DRM_DWORD           cbOrigBuffer            = DEFAULT_BUFFER_SIZE;
    DRM_DWORD           cbRSAPubKeySize         = DRM_RSA_CB_PUBKEY_2048BIT;
    DRM_DWORD           cbRSAPrivKeySize        = DRM_RSA_CB_PRIVKEY_2048BIT;
    DRM_ANSI_STRING     dastrBuffer             = EMPTY_DRM_STRING;
    DRM_ANSI_STRING     dastrModelCert          = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrRSAModelPrivKey     = EMPTY_DRM_STRING;
    DRM_SUBSTRING       dasstrCollection        = EMPTY_DRM_SUBSTRING;
    DRM_CRYPTO_CONTEXT *pCryptoCtx              = NULL;
    DRM_CONST_STRING    dastrDevCertFilename    = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dastrDevPrivKeyFilename = EMPTY_DRM_STRING;
    DRM_RSA_PRIVATE_KEY_2048BIT oRSADevicePrivKey = { 0 };
    DRM_RSA_PUBLIC_KEY_2048BIT  oRSADevicePubKey  = { 0 };
    DRM_RSA_PRIVATE_KEY_2048BIT oRSAModelPrivKey  = { 0 };

    ChkArg( argc >= 2 );

    ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );

    ChkMem( pCryptoCtx = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    if ( argc > 2 && NULL != argv[2] )
    {
        cbOrigBuffer = (DRM_DWORD) strtoul( argv[2], NULL, 0 );
    }

    ChkMem( dastrBuffer.pszString = (DRM_CHAR*) Oem_MemAlloc( cbOrigBuffer ) );
    ZEROMEM( dastrBuffer.pszString, cbOrigBuffer );
    dastrBuffer.cchString = cbOrigBuffer;

    /* dstrModelCert is loaded using a file containing ANSI/UTF8 string. */
    ChkBOOL( LoadTestFile( "files",
                           argv[0],
                           (DRM_BYTE**)&dastrModelCert.pszString,
                           &dastrModelCert.cchString ),
             DRM_E_TEST_INVALIDARG );

    ChkBOOL( dastrBuffer.cchString >= dastrModelCert.cchString, DRM_E_BUFFERTOOSMALL );

    MEMCPY( dastrBuffer.pszString, dastrModelCert.pszString, dastrModelCert.cchString );
    dastrBuffer.cchString = dastrModelCert.cchString;

    ChkBOOL( LoadTestFile( "files",
                            argv[1],
                            (DRM_BYTE**)&dstrRSAModelPrivKey.pwszString,
                            &dstrRSAModelPrivKey.cchString ),
             DRM_E_TEST_INVALIDARG );

    dstrRSAModelPrivKey.cchString /= SIZEOF( DRM_WCHAR );

    ChkDR( _ParseRSAPrivateKey( &dstrRSAModelPrivKey,
                                &oRSAModelPrivKey ) );

    /* Create device private/public key pair */
    ChkDR( DRM_PK_GenKeyPair( pCryptoCtx,
                              eDRM_RSA,
                              eDRM_RSA_KEY_LENGTH_1024_2048BIT,
                              &cbRSAPubKeySize,
                              (DRM_BYTE*) &oRSADevicePubKey,
                              &cbRSAPrivKeySize,
                              (DRM_BYTE*) &oRSADevicePrivKey ) );

    ChkDR( DRM_NDCert_CreateDeviceCertificate( NULL,
                                               &dastrBuffer,
                                               cbOrigBuffer,
                                               &oRSAModelPrivKey,
                                               &oRSADevicePubKey,
                                               pCryptoCtx  ) );

    dasstrCollection.m_cch = dastrBuffer.cchString;
    ChkDR( DRM_WCP_VerifyCertificateCollection( dastrBuffer.pszString,
                                                &dasstrCollection,
                                                &g_dastrMSNDRootPubKeyB64,
                                                NULL,
                                                0,
                                                NULL,
                                                NULL,
                                                pCryptoCtx ) );

    /* Log device certificate into specified file */
    if(argc > 3 && NULL != argv[3] )
    {
        ChkDR( MakeDRMString( &dastrDevCertFilename, argv[3] ) );
        SaveToFileW( dastrDevCertFilename.pwszString,
                     (DRM_BYTE*)dastrBuffer.pszString,
                     dastrBuffer.cchString );
    }

    /* Log device private key into specified file */
    if(argc > 4 && NULL != argv[4] )
    {
        ChkDR( MakeDRMString( &dastrDevPrivKeyFilename, argv[4] ) );
        SaveToFileW( dastrDevPrivKeyFilename.pwszString,
                     (DRM_BYTE*)&oRSADevicePrivKey,
                     SIZEOF( DRM_RSA_PRIVATE_KEY_2048BIT ) );
    }

ErrorExit:

    SAFE_OEM_FREE( dastrBuffer.pszString );
    SAFE_OEM_FREE( dastrModelCert.pszString );
    FREEDRMSTRING( dstrRSAModelPrivKey );
    FREEDRMSTRING( dastrDevCertFilename );
    FREEDRMSTRING( dastrDevPrivKeyFilename );
    SAFE_OEM_FREE( pCryptoCtx );
    return dr;
}

/*****************************************************************************
** Function:    Test_CreateModelCertificate
**
** Synopsis:    Test the creation of a model certificate.
**
** Arguments:   argv[0]   : The file name of the unsigned template stored as UTF8/ANSI.
**              argv[1]   : The file name of the company RSA private key stored as Unicode.
**              argv[2]   : The file name of the B64 encoded RSA modulus of the company
**                          public key. Its matching exponent is defined in global variable
**                          g_dastrWMDRMCertExponent.
**              argv[3]   : Optional, file name of model certificate for output.
**              argv[4]   : Optional, file name for model RSA private key for output
**                          in binary form.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_CreateModelCertificate(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbRSAPubKeySize = DRM_RSA_CB_PUBKEY_2048BIT;
    DRM_DWORD cbRSAPrivKeySize = DRM_RSA_CB_PRIVKEY_2048BIT;
    DRM_ANSI_CONST_STRING dastrUnsignedTemplate = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRSACompanyPrivKey = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrRSACompanyPubKeyModulusB64 = EMPTY_DRM_STRING;
    DRM_CRYPTO_CONTEXT *pCryptoCtx = NULL;
    DRM_CONST_STRING dstrModelCertFilename = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrModelPrivKeyFilename = EMPTY_DRM_STRING;
    DRM_RSA_PRIVATE_KEY_2048BIT oRSACompanyPrivKey  = { 0 };
    DRM_RSA_PRIVATE_KEY_2048BIT oRSAModelPrivKey = { 0 };
    DRM_RSA_PUBLIC_KEY_2048BIT  oRSAModelPubKey  = { 0 };
    DRM_BYTE rgbModulus[ __CB_DECL( DRM_RSA_CB_MODULUS_MAX_2048BIT )] = { 0 };
    DRM_CHAR rgchModulusB64[ __CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX_2048BIT ) ) ] = { 0 };
    DRM_DWORD cbModulus = DRM_RSA_CB_MODULUS_MAX_2048BIT;
    DRM_DWORD cchModulusB64 = CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX_2048BIT );

    _XMBContextA *pXMLCtxA = NULL;

    ChkArg( argc >= 3 );

    ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );

    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    /* dastrUnsignedTemplate is loaded using a file containing ANSI/UTF8 string. */
    ChkBOOL( LoadTestFile( "files",
                           argv[ 0 ],
                           ( DRM_BYTE ** )&dastrUnsignedTemplate.pszString,
                           &dastrUnsignedTemplate.cchString ),
             DRM_E_TEST_INVALIDARG );

    ChkBOOL( LoadTestFile( "files",
                            argv[ 1 ],
                            ( DRM_BYTE ** )&dstrRSACompanyPrivKey.pwszString,
                            &dstrRSACompanyPrivKey.cchString ),
             DRM_E_TEST_INVALIDARG );

    dstrRSACompanyPrivKey.cchString /= SIZEOF( DRM_WCHAR );

    ChkDR( _ParseRSAPrivateKey( &dstrRSACompanyPrivKey,
                                &oRSACompanyPrivKey ) );

    /* dastrRSACompanyPubKeyModulusB64 is loaded using a file containing ANSI/UTF8 string. */
    ChkBOOL( LoadTestFile( "files",
                            argv[ 2 ],
                            ( DRM_BYTE ** )&dastrRSACompanyPubKeyModulusB64.pszString,
                            &dastrRSACompanyPubKeyModulusB64.cchString ),
             DRM_E_TEST_INVALIDARG );

    /* Create model private/public key pair. */
    ChkDR( DRM_PK_GenKeyPair( pCryptoCtx,
                              eDRM_RSA,
                              eDRM_RSA_KEY_LENGTH_1024_2048BIT,
                              &cbRSAPubKeySize,
                              ( DRM_BYTE * )&oRSAModelPubKey,
                              &cbRSAPrivKeySize,
                              ( DRM_BYTE * )&oRSAModelPrivKey ) );

    ChkDR( OEM_RSA_ParsePublicKey_2048BIT( &oRSAModelPubKey,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &cbModulus,
                                           rgbModulus ) );

    ChkDR( DRM_B64_EncodeA( rgbModulus,
                            cbModulus,
                            rgchModulusB64,
                            &cchModulusB64,
                            0 ) );

    ChkMem( pXMLCtxA = ( _XMBContextA * )Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ) );
    ZEROMEM( pXMLCtxA, MAX_DEVICE_CERT_SIZE );
    pXMLCtxA->cbContext = MAX_DEVICE_CERT_SIZE;

    ChkDR( DRM_ND_CB_CreateCertificateFile( pXMLCtxA,
                                            certtypeRoot,
                                            &oRSAModelPubKey,
                                            &dastrRSACompanyPubKeyModulusB64,
                                            &g_dastrWMDRMCertExponent,
                                            &oRSACompanyPrivKey,
                                            &dastrUnsignedTemplate,
                                            NULL,
                                            pCryptoCtx ) );

    /* Save model certificate into specified file. */
    if ( argc > 3 && NULL != argv[ 3 ] )
    {
        ChkDR( MakeDRMString( &dstrModelCertFilename, argv[ 3 ] ) );

        SaveToFileW( dstrModelCertFilename.pwszString,
                     ( DRM_BYTE * )pXMLCtxA->rgchXML,
                     pXMLCtxA->cchBuffer );
    }

    /* Save model private key into specified file. */
    if ( argc > 4 && NULL != argv[ 4 ] )
    {
        ChkDR( MakeDRMString( &dstrModelPrivKeyFilename, argv[ 4 ] ) );

        SaveToFileW( dstrModelPrivKeyFilename.pwszString,
                     ( DRM_BYTE * )&oRSAModelPrivKey,
                     SIZEOF( DRM_RSA_PRIVATE_KEY_2048BIT ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pXMLCtxA );
    SAFE_OEM_FREE( dastrUnsignedTemplate.pszString );
    FREEDRMSTRING( dstrRSACompanyPrivKey );
    SAFE_OEM_FREE( dastrRSACompanyPubKeyModulusB64.pszString );
    FREEDRMSTRING( dstrModelCertFilename );
    FREEDRMSTRING( dstrModelPrivKeyFilename );
    SAFE_OEM_FREE( pCryptoCtx );

    return dr;
}

/*****************************************************************************
** Function:    Test_CreateCompanyCertificate
**
** Synopsis:    Test the creation of a company certificate.
**
** Arguments:   argv[1]   : The file name of the root RSA private key stored as Unicode.
**              argv[2]   : The file name of the B64 encoded RSA modulus of the root
**                          public key. Its matching exponent is defined in global variable
**                          g_dastrWMDRMCertExponent.
**              argv[3]   : Optional, file name of company certificate for output.
**              argv[4]   : Optional, file name for company RSA private key for output
**                          in binary form.
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              or other return code from a function called within
**
** Notes:       Test files are required to be in a directory called "files" in
**              the same folder in which the executable is being run.
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_CreateCompanyCertificate(
    __in long argc,
    __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbRSAPubKeySize = DRM_RSA_CB_PUBKEY_2048BIT;
    DRM_DWORD cbRSAPrivKeySize = DRM_RSA_CB_PRIVKEY_2048BIT;
    DRM_ANSI_CONST_STRING dastrSecurityLevel = CREATE_DRM_ANSI_STRING( "150" );
    DRM_ANSI_CONST_STRING dastrManufName = CREATE_DRM_ANSI_STRING( "OEM" );
    DRM_ANSI_CONST_STRING dastrUnsignedTemplate = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrRSARootPrivKey = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrRSARootPubKeyModulusB64 = EMPTY_DRM_STRING;
    DRM_CRYPTO_CONTEXT *pCryptoCtx = NULL;
    DRM_CONST_STRING dstrCompanyCertFilename = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCompanyPrivKeyFilename = EMPTY_DRM_STRING;
    DRM_RSA_PRIVATE_KEY_2048BIT oRSARootPrivKey = { 0 };
    DRM_RSA_PRIVATE_KEY_2048BIT oRSACompanyPrivKey = { 0 };
    DRM_RSA_PUBLIC_KEY_2048BIT  oRSACompanyPubKey = { 0 };
    DRM_BYTE rgbModulus[ __CB_DECL( DRM_RSA_CB_MODULUS_MAX_2048BIT )] = { 0 };
    DRM_CHAR rgchModulusB64[ __CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX_2048BIT ) ) ] = { 0 };
    DRM_DWORD cbModulus = DRM_RSA_CB_MODULUS_MAX_2048BIT;
    DRM_DWORD cchModulusB64 = CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX_2048BIT );

    _XMBContextA *pXMLCtxA = NULL;

    ChkArg( argc >= 2 );

    ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );

    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    ChkBOOL( LoadTestFile( "files",
                            argv[ 0 ],
                            ( DRM_BYTE ** )&dstrRSARootPrivKey.pwszString,
                            &dstrRSARootPrivKey.cchString ),
             DRM_E_TEST_INVALIDARG );

    dstrRSARootPrivKey.cchString /= SIZEOF( DRM_WCHAR );

    ChkDR( _ParseRSAPrivateKey( &dstrRSARootPrivKey,
                                &oRSARootPrivKey ) );

    ChkBOOL( LoadTestFile( "files",
                            argv[ 1 ],
                            ( DRM_BYTE ** )&dastrRSARootPubKeyModulusB64.pszString,
                            &dastrRSARootPubKeyModulusB64.cchString ),
             DRM_E_TEST_INVALIDARG );

    dastrRSARootPubKeyModulusB64.cchString /= SIZEOF( DRM_WCHAR );

    ChkMem( dastrUnsignedTemplate.pszString = ( DRM_CHAR * )Oem_MemAlloc( MAX_DEVICE_CERT_TEMPLATE_SIZE ) );
    dastrUnsignedTemplate.cchString = MAX_DEVICE_CERT_TEMPLATE_SIZE / SIZEOF( DRM_WCHAR );

    ChkDR( _CreateCompanyTemplate( &dastrSecurityLevel,
                                   &dastrManufName,
                                   &dastrUnsignedTemplate ) );

    /* Create model private/public key pair. */
    ChkDR( DRM_PK_GenKeyPair( pCryptoCtx,
                              eDRM_RSA,
                              eDRM_RSA_KEY_LENGTH_1024_2048BIT,
                              &cbRSAPubKeySize,
                              ( DRM_BYTE * )&oRSACompanyPubKey,
                              &cbRSAPrivKeySize,
                              ( DRM_BYTE * )&oRSACompanyPrivKey ) );

    ChkDR( OEM_RSA_ParsePublicKey_2048BIT( &oRSACompanyPubKey,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &cbModulus,
                                           rgbModulus ) );

    ChkDR( DRM_B64_EncodeA( rgbModulus,
                            cbModulus,
                            rgchModulusB64,
                            &cchModulusB64,
                            0 ) );

    ChkMem( pXMLCtxA = ( _XMBContextA * )Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ) );
    ZEROMEM( pXMLCtxA, MAX_DEVICE_CERT_SIZE );
    pXMLCtxA->cbContext = MAX_DEVICE_CERT_SIZE;

    ChkDR( DRM_ND_CB_CreateCertificateFile( pXMLCtxA,
                                            certtypeRoot,
                                            &oRSACompanyPubKey,
                                            &dastrRSARootPubKeyModulusB64,
                                            &g_dastrWMDRMCertExponent,
                                            &oRSARootPrivKey,
                                            &dastrUnsignedTemplate,
                                            NULL,
                                            pCryptoCtx ) );

    /* Save company certificate into specified file. */
    if ( argc > 2 && NULL != argv[ 2 ] )
    {
        ChkDR( MakeDRMString( &dstrCompanyCertFilename, argv[ 2 ] ) );

        SaveToFileW( dstrCompanyCertFilename.pwszString,
                     ( DRM_BYTE * )pXMLCtxA->rgchXML,
                     pXMLCtxA->cchBuffer );
    }

    /* Save company private key into specified file. */
    if ( argc > 3 && NULL != argv[ 3 ] )
    {
        ChkDR( MakeDRMString( &dstrCompanyPrivKeyFilename, argv[ 3 ] ) );

        SaveToFileW( dstrCompanyPrivKeyFilename.pwszString,
                     ( DRM_BYTE * )&oRSACompanyPrivKey,
                     SIZEOF( DRM_RSA_PRIVATE_KEY_2048BIT ) );
    }

ErrorExit:

    SAFE_OEM_FREE( pXMLCtxA );
    SAFE_OEM_FREE( dastrUnsignedTemplate.pszString );
    FREEDRMSTRING( dstrRSARootPrivKey );
    SAFE_OEM_FREE( dastrRSARootPubKeyModulusB64.pszString );
    FREEDRMSTRING( dstrCompanyCertFilename );
    FREEDRMSTRING( dstrCompanyPrivKeyFilename );
    SAFE_OEM_FREE( pCryptoCtx );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

