/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <drmdomainstore.h>
#include <drmlicacqv3.h>

#include <drmlicacqv3constants.h>
#include <drmxmlparser.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <oemaescommon.h>
#include <oemaes.h>
#include <drmsoapxmlutility.h>

#include <LicAcqV3TestAPI.h>
#include <DRMManagerTestAPI.h>
#include <drmdla.h>
#include <drmwmdrm.h>
#include <drmsoapxmlconstants.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

ENTER_PKTEST_NAMESPACE_CODE

#define MAX_RIGHTS          5

DRM_APP_CONTEXT *g_poAppContext = NULL;
DRM_BOOL g_fUseNullAppContext = FALSE;

DRM_BYTE *g_pbRevocationBuffer = NULL;

DRM_DWORD g_numRights  = 0;
DRM_CONST_STRING *g_pwszRights[ MAX_RIGHTS ] = { 0 };

DRM_LICENSE_RESPONSE g_oLicenseResponse = { eUnknownProtocol };

const DRM_ANSI_CONST_STRING g_dastrSoapEnvelopeTag    = CREATE_DRM_ANSI_STRING( "soap:Envelope" );
const DRM_ANSI_CONST_STRING g_dastrSoapBodyTag        = CREATE_DRM_ANSI_STRING( "soap:Body" );
const DRM_ANSI_CONST_STRING g_dastrEncryptedDataTag   = CREATE_DRM_ANSI_STRING( "EncryptedData" );
const DRM_ANSI_CONST_STRING g_dastrDataTag            = CREATE_DRM_ANSI_STRING( "Data" );
const DRM_ANSI_CONST_STRING g_dastrSignatureTag       = CREATE_DRM_ANSI_STRING( "Signature" );

DRM_ANSI_CONST_STRING g_dastrLicenseChallenge       = {0};
DRM_ANSI_CONST_STRING g_dastrLicenseAcknowledgement = {0};
DRM_BB_CONTEXT g_oBBContextLicAqV3 = {0};

static PUBKEY_P256 Server_oECC256PubKey =
{
    0XC8, 0XB6, 0XAF, 0X16, 0XEE, 0X94, 0X1A, 0XAD,
    0XAA, 0X53, 0X89, 0XB4, 0XAF, 0X2C, 0X10, 0XE3,
    0X56, 0XBE, 0X42, 0XAF, 0X17, 0X5E, 0XF3, 0XFA,
    0XCE, 0X93, 0X25, 0X4E, 0X7B, 0X0B, 0X3D, 0X9B,
    0X98, 0X2B, 0X27, 0XB5, 0XCB, 0X23, 0X41, 0X32,
    0X6E, 0X56, 0XAA, 0X85, 0X7D, 0XBF, 0XD5, 0XC6,
    0X34, 0XCE, 0X2C, 0XF9, 0XEA, 0X74, 0XFC, 0XA8,
    0XF2, 0XAF, 0X59, 0X57, 0XEF, 0XEE, 0XA5, 0X62
};

static PRIVKEY_P256 Server_oECC256PrivKey =
{
    0X5A, 0X20, 0XB3, 0X26, 0X56, 0XDB, 0XCB, 0X50,
    0X8A, 0X34, 0X89, 0XC7, 0X9F, 0XD9, 0X4F, 0X65,
    0X36, 0X43, 0XFA, 0X46, 0X52, 0X66, 0XE3, 0XA7,
    0XC4, 0X41, 0X55, 0X43, 0XBF, 0XD5, 0X65, 0X3A
};

/*********************************************************************
**
** Function: _UseNullAppContext
**
** Synopsis: Function that set a global flag to indicate whether NULL
**           DRM App context should be used in the other test functions.
**           The value of g_fUseNullAppContext is passed in as argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _UseNullAppContext(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );

    g_fUseNullAppContext = atol( argv[ 0 ] );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _AddDomainCert
**
** Synopsis: Function that adds a certificate into the domain store. The
**           account ID is passed in as argv[1] (B64 encoded). The service
**           ID is passed in as argv[2] (B64 encoded). The certificate
**           data is read from a file, whose file name is passed in as argv[0].
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _AddDomainCert(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )g_poAppContext;
    DRM_DOMAINSTORE_CONTEXT oDomainStoreContext = { 0 };
    DRM_CONST_STRING dstrAccountID = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrServiceID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_GUID oServiceID = EMPTY_DRM_GUID;
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;
    DRM_BYTE                            rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] = {0};

    ChkArg( argc == 3 );
    ChkArg( argv[ 0 ] != NULL );
    ChkArg( argv[ 1 ] != NULL );
    ChkArg( argv[ 2 ] != NULL );

    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbCert, &cbCert ) );

    ChkDR( MakeDRMString( &dstrAccountID, argv[ 1 ] ) );
    ChkDR( MakeDRMString( &dstrServiceID, argv[ 2 ] ) );

    ChkDR( DRM_B64_DecodeW( &dstrAccountID,
                            &cbSize,
                            ( DRM_BYTE* )&oAccountID,
                            0 ) );

    cbSize = SIZEOF( DRM_GUID );

    ChkDR( DRM_B64_DecodeW( &dstrServiceID,
                            &cbSize,
                            ( DRM_BYTE* )&oServiceID,
                            0 ) );

    ChkDR( DRM_DOMST_OpenStore ( &poAppContextInternal->oDatastoreHDS,
                                 &oDomainStoreContext ) );

    ChkDR( DRM_DOMST_CreateDomainStorePassword( &oAccountID,
                                                &poAppContextInternal->oBlackBoxContext,
                                                rgbPasswordSST ) );

    dr = DRM_DOMST_AddCert ( rgbPasswordSST,
                             poAppContextInternal->oBlackBoxContext.pOEMContext,
                             &oDomainStoreContext,
                             &oAccountID,
                             &oServiceID,
                             pbCert,
                             cbCert );

    ChkDR( dr );

    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrAccountID );
    FREEDRMSTRING( dstrServiceID );
    SAFE_OEM_FREE( pbCert );

    DRM_DOMST_CloseStore( &oDomainStoreContext );

    return dr;
}

/*********************************************************************
**
** Function: _SetRights
**
** Synopsis: Function that adds a right string to a global rights array.
**           argv[0] is the name of the right.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _SetRights(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr=DRM_SUCCESS;

    ChkArg( argc == 1 );
    ChkArg(g_numRights < MAX_RIGHTS);

    ChkMem( g_pwszRights[ g_numRights ] =
            ( DRM_CONST_STRING * )Oem_MemAlloc( SIZEOF( DRM_CONST_STRING ) ) );

    ChkDR( MakeDRMString( g_pwszRights[ g_numRights++ ], argv[ 0 ] ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _SetHeader
**
** Synopsis: Function that sets reads a content header from a content
**           header file.
**           argv[0] is the file name of the content header file.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _SetHeader(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbHeader = 0;
    DRM_BYTE *pbHeader1 = NULL;
    DRM_BYTE *pbHeader2 = NULL;
    DRM_CONST_STRING dstrHeader = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrHeader1 = EMPTY_DRM_SUBSTRING;

    ChkArg( argc == 1 );


    ChkArg( LoadTestFile( NULL, argv[ 0 ],  &pbHeader1, &cbHeader ) );

    ChkMem( pbHeader2 = (DRM_BYTE *)Oem_MemAlloc( cbHeader * SIZEOF( DRM_WCHAR ) ) );

    DSTR_FROM_PB( &dstrHeader, pbHeader2, cbHeader * SIZEOF( DRM_WCHAR ) );

    dasstrHeader1.m_cch = cbHeader;

    DRM_UTL_PromoteASCIItoUNICODE( (const DRM_CHAR *)pbHeader1,
                                  &dasstrHeader1,
                                  ( DRM_STRING * )&dstrHeader );

    ChkDR( Drm_Content_SetProperty( g_poAppContext,
                                    DRM_CSP_AUTODETECT_HEADER,
                                    PB_DSTR( &dstrHeader ),
                                    CB_DSTR( &dstrHeader ) ) );

ErrorExit:
    SAFE_OEM_FREE( pbHeader2 );
    SAFE_OEM_FREE( pbHeader1 );
    return dr;
}

/*********************************************************************
**
** Function: _AddLicenseAcknowledgement
**
** Synopsis: Function that adds a license acknowledgement to the global
**           license acknowledgement array.
**           argv[0]: B64 encoded KID
**           argv[1]: Result code
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _AddLicenseAcknowledgement(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;
    DRM_DWORD cbSize = 0;

    ChkArg( argc == 2 );

    if ( g_oLicenseResponse.m_cAcks >= DRM_MAX_LICENSE_ACK )
    {
        goto ErrorExit;
    }

    ChkDR( MakeDRMString( &dstrKID, argv[ 0 ] ) );

    cbSize = ( DRM_DWORD )strlen( argv[ 0 ] );

    ChkDR( DRM_B64_DecodeW( &dstrKID,
                            &cbSize,
                            ( DRM_BYTE* )&g_oLicenseResponse.m_rgoAcks[ g_oLicenseResponse.m_cAcks ].m_oKID,
                            0 ) );

    g_oLicenseResponse.m_rgoAcks[ g_oLicenseResponse.m_cAcks ].m_dwResult = atoi( argv[ 1 ] );

    g_oLicenseResponse.m_cAcks++;

ErrorExit:
    FREEDRMSTRING( dstrKID );
    return dr;
}

/*********************************************************************
**
** Function: _SetTransactionID
**
** Synopsis: Function that sets a transaction ID to a global variable.
**           argv[0] is the transaction ID.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _SetTransactionID(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );
    ChkArg( strlen( argv[ 0 ] ) < DRM_MAX_TRANSACTION_ID );

    MEMCPY( g_oLicenseResponse.m_rgbTransactionID, argv[ 0 ], ( DRM_DWORD )strlen( argv[ 0 ] ) );

    g_oLicenseResponse.m_cbTransactionID = ( DRM_DWORD )strlen( argv[ 0 ] );

ErrorExit:
    return dr;
}



DRM_RESULT DRM_CALL _BlackBoxInit( DRM_BB_CONTEXT *pcontextBBX )
{
    DRM_RESULT dr;

    /* blackbox initialized */
    if( g_oBBContextLicAqV3.pKeyFileContext != NULL )
    {
        ChkDR( DRM_KF_Uninitialize( g_oBBContextLicAqV3.pKeyFileContext ) );
        SAFE_OEM_FREE( g_oBBContextLicAqV3.pKeyFileContext );
    }
    ChkDR( DRM_BBX_Shutdown( pcontextBBX ) );
    ChkDR( DRM_BBX_Initialize( NULL, pcontextBBX ) );

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( pcontextBBX ) );
    }

    ChkDR( TST_UTL_SetPlayReadyCertCachedValues( pcontextBBX ) );

    ChkDR( DRM_BBX_TestDeviceKeyPair( pcontextBBX ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _DO_NOT_USE_BBX_ECC256_Decrypt
**
** Synopsis: Function that does ECC-256 decryption of the passed in data
**           in place using the device's ECC-256 private key, whose matching
**           ECC-256 public key is the passed in ECC-256 public key.
**
**      IMPORTANT NOTE: THIS FUNCTION IS A COPY OF
**      DRM_BBX_ECC256_Decrypt to allow the decryption of the
**      Challenge & Ack Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pbData]               -- Pointer to a buffer that contains the
**                             data to be decrypted. After the decryption,
**                             it contains the decrypted data.
** [f_pcbData]              -- Pointer to a variable that contains the size
**                             (number of bytes) of the buffer to be
**                             decrypted. After the decryption, it receives
**                             the size (number of bytes) of the decrypted
**                             data.
** [f_poECC256PubKey]       -- Pointer to a device ECC-256 public key whose
**                             matching device ECC-256 private key should be
**                             used to decrypt the passed in data.
**                             If its content is all 0, caller assumes to
**                             use the current device private key to decrypt
**                             the data without checking whether
**                             *f_poECC256PubKey is the same as the current
**                             device public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_CRYPTO_PUBLIC_KEY_NOT_MATCH if the
**                          passed in ECC-256 public key does not match any
**                          device public key.
**
** Note:                    The caller ensures the passed in buffer is big
**                          enough to receive the decrypted data.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _DO_NOT_USE_BBX_ECC256_Decrypt(
    __inout_ecount( 1 )                DRM_BB_CONTEXT *f_poBBXContext,
    __inout_bcount( *f_pcbData )       DRM_BYTE       *f_pbData,
    __inout_ecount( 1 )                DRM_DWORD      *f_pcbData,
    __inout_ecount( 1 )          const PUBKEY_P256    *f_poECC256PubKey )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    PUBKEY_P256 oECC256PubKey = Server_oECC256PubKey;
    PRIVKEY_P256 oECC256PrivKey = Server_oECC256PrivKey;
    PLAINTEXT_P256  oPlainText      = { 0 };
    CIPHERTEXT_P256 oCipherText     = { 0 };

    ChkArg( f_poBBXContext   != NULL );
    ChkArg( f_pbData         != NULL );
    ChkArg( f_pcbData        != NULL );
    ChkArg( *f_pcbData       == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );
    ChkArg( f_poECC256PubKey != NULL );

    if ( MEMCMP( f_poECC256PubKey,
                 &oECC256PubKey,
                 SIZEOF( PUBKEY_P256 ) ) != 0 )
    {
        ZEROMEM( &oECC256PubKey, SIZEOF( PUBKEY_P256 ) );

        /* Check whether the input public key contains all 0. */
        if ( MEMCMP( f_poECC256PubKey,
             &oECC256PubKey,
             sizeof( PUBKEY_P256 ) ) != 0 )
        {
            /*
            ** The public key associated with an encrypted domain private key from
            ** the server does not match any public key on the device.
            */
            ChkDR( DRM_E_CRYPTO_PUBLIC_KEY_NOT_MATCH );
        }
    }

    ZEROMEM( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );
    ZEROMEM( &oCipherText, SIZEOF( CIPHERTEXT_P256 ) );

    MEMCPY( oCipherText.m_rgbCiphertext, f_pbData, *f_pcbData );

    ChkDR( OEM_ECC_Decrypt_P256( &oECC256PrivKey,
                                 &oCipherText,
                                 &oPlainText,
                                 (struct bigctx_t *)f_poBBXContext->CryptoContext.rgbCryptoContext ) );

    MEMCPY( f_pbData, oPlainText.m_rgbPlaintext, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );

    *f_pcbData = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;

    /* Zero the temoprary decoded buffer to avoid leaking secrets in the heap. */
    OEM_SECURE_ZERO_MEMORY( &oPlainText, SIZEOF( PLAINTEXT_P256 ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &oECC256PrivKey, SIZEOF( PRIVKEY_P256 ) );
    OEM_SECURE_ZERO_MEMORY( &oECC256PubKey, SIZEOF( PUBKEY_P256 ) );
    return dr;
}


/*********************************************************************
**
** Function: _DO_NOT_USE_ExtractCipherData
**
** Synopsis: Function that extracts a cipher data buffer contained in
**           a XML segment and applies B64 decoder.
**           This function does not decrypt cipher data.
**           The XML segment should be in the form of
**           <...>/<CipherData>/<CipherValue>, and the name of the root
**           node can be any string.
**
**      IMPORTANT NOTE: THIS FUNCTION IS A COPY OF
**      _ExtractCipherData to allow the decryption of the
**      Challenge & Ack Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_pszBase]              -- base XML data string for f_pdasstrXML
**                             and other substrings.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <CipherData> subnode.
** [f_ppbCipherData]        -- Pointer to a pointer to a buffer that
**                             receives the B64 decoded cipher data
**                             contained in the XML segment.
** [f_pcbCipherData]        -- Pointer to a variable to receive the size
**                             (number of bytes) of the B64 decoded cipher
**                             data contained in the XML segment.
** [f_pdasstrCipherData]    -- Pointer to a variable to receive the a
**                             substring that cover the encoded cipher data.
**                             It is the caller's responsiblity to cover
**                             the the substring with space (' ') to
**                             ensure the whole XML string is still valid
**                             for future parsing.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _DO_NOT_USE_ExtractCipherData(
    __in const DRM_CHAR *f_pszBase,
    __in const DRM_SUBSTRING *f_pdasstrXML,
    __deref_out_ecount_opt(*f_pcbCipherData) DRM_BYTE **f_ppbCipherData,
    __out DRM_DWORD *f_pcbCipherData,
    __out DRM_SUBSTRING *f_pdasstrCipherData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrCipherData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchExtra = 0;

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_ppbCipherData != NULL );
    ChkArg( f_pcbCipherData != NULL );

    *f_ppbCipherData = NULL;
    *f_pcbCipherData = 0;

    /*
    ** Extract <CipherValue> node from the passed in root node.
    ** It contains the cipher data string.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigCipherValuePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrCipherData,
                                      g_chForwardSlash ) );

    if ( f_pdasstrCipherData != NULL )
    {
        *f_pdasstrCipherData = dasstrCipherData;
    }

    *f_pcbCipherData = dasstrCipherData.m_cch;

    if ( *f_pcbCipherData > 0 )
    {
        ChkDR( DRM_B64_DecodeA( f_pszBase,
                                &dasstrCipherData,
                                f_pcbCipherData,
                                NULL,
                                DRM_BASE64_DECODE_IN_PLACE ) );

        if ( dasstrCipherData.m_ich % SIZEOF( DRM_DWORD ) != 0 )
        {
            /* If the encoded string is not beginning at an aligned address,
            ** shift it to make aligned.
            */
            cchExtra = SIZEOF( DRM_DWORD ) - ( dasstrCipherData.m_ich % SIZEOF( DRM_DWORD ) );

            /* *f_ppbCipherData will point to an aligned address. */
            *f_ppbCipherData = ( DRM_BYTE * )f_pszBase + __CB_DECL( dasstrCipherData.m_ich + cchExtra );

            DRM_BYT_MoveBytes( *f_ppbCipherData,
                               0,
                               ( DRM_BYTE * )f_pszBase,
                               dasstrCipherData.m_ich,
                               *f_pcbCipherData );
        }
        else
        {
            /* The encoded string is beginning at an aligned address. */
            *f_ppbCipherData = ( DRM_BYTE * )f_pszBase + __CB_DECL( dasstrCipherData.m_ich );
        }
    }
    else
    {
        /* If <CipherValue> does not have any content, something is wrong. */
        ChkDR( DRM_E_XMLSIG_FORMAT );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DO_NOT_USE_ExtractEncryptedKey
**
** Synopsis: Function that extracts and decrypts an AES key contained
**           in a XML segment.
**           The XML segment should have a root node named <EncryptedKey>.
**           Under it there are three subnodes:
**           1. <EncryptionMethod> that contains the method used to do
**              the encryption.
**           2. <KeyInfo> that contains an ECC-256 public key used to
**              encrypt the AES key. Its matching ECC-256 private key
**              should be used to decrypt the AES key.
**           3. <CipherData> that contains the encrypted AES key.
**
**      IMPORTANT NOTE: THIS FUNCTION IS A COPY OF
**      _DO_NOT_USE_ExtractEncryptedKey to allow the decryption of the
**      Challenge & Ack Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pszBase]              -- base XML data string, all substrings
**                             are operating on it.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <CipherData> subnode.
** [f_ppbKey]               -- Pointer to a pointer to a buffer that
**                             receives the decrypted AES key contained
**                             in the XML segment.
** [f_pcbKey]               -- Pointer to a variable to receive the size
**                             (number of bytes) of the decrypted AES key.
** [f_pdasstrKey]           -- Pointer to a variable to receive the a
**                             substring that cover the encoded key.
**                             It is the caller's responsiblity to cover
**                             the the substring with space (' ') to
**                             ensure the whole XML string is still valid
**                             for future parsing.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _DO_NOT_USE_ExtractEncryptedKey(
    __in DRM_BB_CONTEXT *f_poBBXContext,
    __in const DRM_CHAR *f_pszBase,
    __in const DRM_SUBSTRING *f_pdasstrXML,
    __deref_out_ecount_opt(*f_pcbKey) DRM_BYTE **f_ppbKey,
    __out DRM_DWORD *f_pcbKey,
    __out DRM_SUBSTRING *f_pdasstrKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrKeyInfo  = EMPTY_DRM_SUBSTRING;
    PUBKEY_P256 oECC256PubKey = { 0 };

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_ppbKey != NULL );
    ChkArg( f_pcbKey != NULL );
    ChkArg( f_pdasstrKey != NULL );

    *f_ppbKey = NULL;
    *f_pcbKey = 0;

    /*
    ** Extract <KeyInfo> node from <EncryptedKey> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigKeyInfoPath,
                                      NULL,
                                      NULL,
                                      &dasstrKeyInfo,
                                      NULL,
                                      g_chForwardSlash ) );

    /*
    ** Extract ECC-256 public key from <KeyInfo> node.
    */
    ChkDR( DRM_XMLSIG_ExtractSimpleECC256Key( f_pszBase,
                                              &dasstrKeyInfo,
                                              &oECC256PubKey ) );

    /*
    ** Extract the the encrypted AES key from the <CipherValue> node.
    */
    ChkDR( _DO_NOT_USE_ExtractCipherData( f_pszBase,
                               f_pdasstrXML,
                               f_ppbKey,
                               f_pcbKey,
                               f_pdasstrKey ) );

    /*
    ** ECC-256 decrypt the AES key in place using the matching private key of
    ** the public key.
    ** It assumes the size of the ECC-256 decrypted AES key can be contained in
    ** the original buffer, and the size of the ECC-256 decrypted AES key is
    ** equal or smaller than the size of the ECC-256 encrypted AES key.
    */
    ChkArg( *f_ppbKey != NULL );
    ChkDR( _DO_NOT_USE_BBX_ECC256_Decrypt( f_poBBXContext,
                                   *f_ppbKey,
                                   f_pcbKey,
                                   &oECC256PubKey ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DO_NOT_USE_ExtractEncryptedData2
**
** Synopsis: Function that extracts and decrypts a data buffer contained
**           in a XML segment.
**           The XML segment should have a root node named <EncryptedData>
**           and should have three subnodes under it. They are:
**           1. <EncryptionMethod> that contains the method used to do
**              the encryption.
**           2. <KeyInfo> that contains an AES key used to encrypt the
**              data.
**           3. <CipherData> that contains the encrypted data.
**
**      IMPORTANT NOTE: THIS FUNCTION IS A COPY OF
**      DRM_XMLSIG_ExtractEncryptedData to allow the decryption of the
**      Challenge & Ack Encrypted data nodes with the server private keys.
**      It is NOT intended for use outside of this test file.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pszBase]              -- base XML data string, all substrings
**                             are operating on it.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment of the whole <EncryptedData>.
** [f_pbData]               -- Pointer to to a buffer that receives the
**                             decrypted data contained in the XML segment.
** [f_pcbData]              -- Pointer to a variable to specify the size
**                             (number of bytes) of the buffer to store
**                             the decrypted data, It receives the actual
**                             size (number of bytes) of the decrypted
**                             data during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _DO_NOT_USE_ExtractEncryptedData2(
    __in DRM_BB_CONTEXT *f_poBBXContext,
    __in const DRM_CHAR *f_pszBase,
    __in const DRM_SUBSTRING *f_pdasstrXML,
    __out_ecount(*f_pcbData) DRM_BYTE *f_pbData,
    __inout DRM_DWORD *f_pcbData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrEncryptedKey = EMPTY_DRM_SUBSTRING;
    DRM_BYTE *pbKey = NULL;
    DRM_DWORD cbKey = 0;
    DRM_BYTE *pbCipherData = NULL;
    DRM_DWORD cbCipherData = 0;
    DRM_AES_KEY oAESKey = { 0 };
    DRM_SUBSTRING dasstrKey = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrCipherData = EMPTY_DRM_SUBSTRING;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_pbData != NULL );
    ChkArg( f_pcbData != NULL );

    /*
    ** Extract <EncryptedKey> node from <EncryptedData> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigEncryptedKeyPath,
                                      NULL,
                                      NULL,
                                      &dasstrEncryptedKey,
                                      NULL,
                                      g_chForwardSlash ) );

    /* Extract AES key from <EncryptedKey> node. */
    ChkDR( _DO_NOT_USE_ExtractEncryptedKey( f_poBBXContext,
                                           f_pszBase,
                                           &dasstrEncryptedKey,
                                           &pbKey,
                                           &cbKey,
                                           &dasstrKey ) );

    ChkBOOL( cbKey == DRM_XML_AES_KEY_SIZE, DRM_E_XMLSIG_FORMAT );
    ChkArg( pbKey != NULL );

    ChkDR( Oem_Aes_SetKey( &pbKey[ DRM_AES_KEYSIZE_128 ],
                           &oAESKey ) );

    /*
    ** Set all bytes of the decoded string to ' ' to prevent future parsing
    ** error of the current XML string.
    */
    DRM_BYT_SetBytes( ( DRM_BYTE * )f_pszBase,
                      dasstrKey.m_ich,
                      dasstrKey.m_cch,
                      ' ' );

    /* Extract the data to be AES decrypted. */
    ChkDR( _DO_NOT_USE_ExtractCipherData( f_pszBase,
                               f_pdasstrXML,
                               &pbCipherData,
                               &cbCipherData,
                               &dasstrCipherData ) );

    if ( cbCipherData < DRM_AES_KEYSIZE_128 )
    {
        ChkDR( DRM_E_XMLSIG_FORMAT );
    }

    /* Discount the size of the first DRM_AES_KEYSIZE_128 bytes of IV. */
    cbCipherData -= DRM_AES_KEYSIZE_128;

    if ( *f_pcbData >= cbCipherData )
    {
        *f_pcbData = cbCipherData;
    }
    else
    {
        /*
        ** If the size of the passed in buffer is smaller than the size of
        ** the decrypted data, exit with an error code.
        */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** AES decrypt the cipher data.
    ** The first DRM_AES_KEYSIZE_128 bytes of pbCipherData is the IV.
    */
    ChkArg( pbCipherData != NULL );
    ChkDR( Oem_Aes_CbcDecryptData( &oAESKey,
                                   pbCipherData + __CB_DECL( DRM_AES_KEYSIZE_128 ),
                                   cbCipherData,
                                   pbCipherData ) );

    MEMCPY( f_pbData, pbCipherData + __CB_DECL( DRM_AES_KEYSIZE_128 ), *f_pcbData );

    /*
    ** Set all bytes of the decoded string to ' ' to prevent future parsing
    ** error of the current XML string.
    */
    DRM_BYT_SetBytes( ( DRM_BYTE * )f_pszBase,
                      dasstrCipherData.m_ich,
                      dasstrCipherData.m_cch,
                      ' ' );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: TestDrm_LicenseAcq_GenerateChallenge
**
** Synopsis: Function that generates a license challenge XML.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_GenerateChallenge(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_CHAR rgchURL[ 1024 ];
    DRM_DWORD cchURL = 1024;

    ChkArg( argc == 1 );

    dr = Drm_LicenseAcq_GenerateChallenge( g_poAppContext,
                                           (const DRM_CONST_STRING **)g_pwszRights,
                                           g_numRights,
                                           NULL,
                                           NULL,
                                           0,
                                           rgchURL,
                                           &cchURL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &cbChallenge );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        if (dr!= DRM_SUCCESS)
       {
            ChkDR( dr );
        }
        else
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT);
        }
    }

    ChkBOOL( cchURL <= 1024, DRM_E_FAIL );

    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );

    ChkDR( Drm_LicenseAcq_GenerateChallenge( g_poAppContext,
                                             (const DRM_CONST_STRING **)g_pwszRights,
                                             g_numRights,
                                             NULL,
                                             NULL,
                                             0,
                                             rgchURL,
                                             &cchURL,
                                             NULL,
                                             NULL,
                                             pbChallenge,
                                             &cbChallenge ) );

    LogBlob("LicAcqV3","License Challenge:", pbChallenge, cbChallenge );

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );
    return dr;
}


/*********************************************************************
**
** Function: _GenerateLicAcqChallenge
**
** Synopsis: Function that generates a license challenge XML.
**
** Arguments:
** [ f_pszServiceID ]       -- Optional pointer to a B64 encoded service ID.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _GenerateLicAcqChallenge( __in_opt const DRM_CHAR *f_pszServiceID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_CHAR rgchURL[ 1024 ];
    DRM_DWORD cchURL = 1024;
    DRM_CONST_STRING dstrServiceID = EMPTY_DRM_STRING;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;
    DRM_DWORD cbSize = SIZEOF( DRM_GUID );

    if ( f_pszServiceID != NULL )
    {
        ChkDR( MakeDRMString( &dstrServiceID, f_pszServiceID ) );

        ChkDR( DRM_B64_DecodeW( &dstrServiceID,
                                &cbSize,
                                ( DRM_BYTE * )&oDomainID.m_oServiceID,
                                0 ) );
    }

    dr = Drm_LicenseAcq_GenerateChallenge( g_poAppContext,
                                           (const DRM_CONST_STRING **)g_pwszRights,
                                           g_numRights,
                                           f_pszServiceID == NULL ? NULL : &oDomainID,
                                           NULL,
                                           0,
                                           rgchURL,
                                           &cchURL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &cbChallenge );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        if (dr!= DRM_SUCCESS)
        {
            ChkDR( dr );
        }
        else
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT);
        }
    }


    SAFE_OEM_FREE( pbChallenge );
    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );

    ChkBOOL( cchURL <= 1024, DRM_E_FAIL );

    ChkDR( Drm_LicenseAcq_GenerateChallenge( g_poAppContext,
                                             (const DRM_CONST_STRING **)g_pwszRights,
                                             g_numRights,
                                             f_pszServiceID == NULL ? NULL : &oDomainID,
                                             NULL,
                                             0,
                                             rgchURL,
                                             &cchURL,
                                             NULL,
                                             NULL,
                                             pbChallenge,
                                             &cbChallenge ) );

    LogBlob("LicAcqV3","License Challenge:", pbChallenge, cbChallenge );

    SAFE_OEM_FREE( g_dastrLicenseChallenge.pszString );
    g_dastrLicenseChallenge.cchString = cbChallenge;
    g_dastrLicenseChallenge.pszString = (const DRM_CHAR*)pbChallenge;
    pbChallenge = NULL;

    ChkDR( _BlackBoxInit( &g_oBBContextLicAqV3 ) );

ErrorExit:
    FREEDRMSTRING( dstrServiceID );
    SAFE_OEM_FREE( pbChallenge );
    return dr;
}


/*********************************************************************
**
** Function: VerifyBasicFormat_GenerateLicChallenge
**
** Synopsis: Verifies the basic xml format of the of the v3 License challenge,
**          and that all required nodes are present.
**          No arguments are required.
**
** Arguments:
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
**
** Returns:               DRM_SUCCESS on success.
**
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyBasicFormat_GenerateLicChallenge(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_ANSI_CONST_STRING dastrDecryptXmlData = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubSubData = EMPTY_DRM_SUBSTRING;

    DRM_BYTE *pData = NULL;
    DRM_DWORD cData = 0;

    ChkDR( _GenerateLicAcqChallenge( NULL ) );

/* Check that the challenge is in the correct place..
**
**  challenge tag & location:
**
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcquireLicense>
**                  ...
**                  <challenge>
**                      xml
**                  </challenge>
**              <AcquireLicense>
**          <soap:Body>
**      <soap:Envelope>
** <xml version>
**
**/
    dsstrXmlNode.m_cch = g_dastrLicenseChallenge.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

 /* Check that the challenge contains the Version node..
**
**  ActionList tag & location:
**
** <challenge>
**      <Challenge ...>
**          <LA ...>
**              <version>
**              </version>
**          </LA>
**      </Challenge>
** </challenge>
**
**/


    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLATag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqVersionTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

/* Check that the challenge contains the ContentHeader node..
**
**  ContentHeader tag & location:
**
**      <Challenge ...>
**          <LA ...>
**              <ContentHeader>
**              </ContentHeader>
**          </LA>
**      </Challenge>
**
**/

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqContentHeaderTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

/* Check that the challenge contains the ClientInfo node..
**
**  ClientInfo tag & location:
**
**      <Challenge ...>
**          <LA ...>
**              <ClientInfo>
**              </ClientInfo>
**          </LA>
**      </Challenge>
**
**/

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrReqTagClientInfo,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

/* Check that the challenge contains the ClientVersion node..
**
**  ClientVersion tag & location:
**
**      <Challenge ...>
**          <LA ...>
**              <ClientInfo>
**                  <ClientVersion>
**                  </ClientVersion>
**              </ClientInfo>
**          </LA>
**      </Challenge>
**
**/

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlSubNode,
               &g_dastrReqTagClientVersion,
                NULL,
                NULL,
                0,
               &dsstrXmlSubSubNode,
               &dsstrXmlSubSubData,
                1 ) );

/* Check that the challenge contains the LicenseNonce node..
**
**  Nonce tag & location:
**
**      <LA ...>
**          ...
**          <LicenseNonce>
**              ...
**          </LicenseNonce>
**      </LA>
**
**/

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLicenseNonceTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

/* Check that the challenge contains the CertificateChains & at least one CertificateChain node..
**
**  CertificateCollections tag & location:
**
** <challenge>
**      <Challenge ...>
**          <LA ...>
**              <Data>
**                  <CertificateChains>
**                      <CertificateChain>
**                      </CertificateChain>
**                  </CertificateChains>
**              </Data>
**          </LA>
**      </Challenge>
** </challenge>
**
**/

    dsstrXmlNode.m_cch = g_dastrLicenseChallenge.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLATag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrEncryptedDataTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    cData = dsstrXmlNode.m_cch;
    ChkMem( pData = (DRM_BYTE *) Oem_MemAlloc( cData ) );

    ChkDR( _DO_NOT_USE_ExtractEncryptedData2(
           &g_oBBContextLicAqV3,
            g_dastrLicenseChallenge.pszString,
           &dsstrXmlNode,
            pData,
            &cData ) );

    dastrDecryptXmlData.pszString = (const DRM_CHAR *) pData;
    dastrDecryptXmlData.cchString = cData;

    dsstrXmlNode.m_ich = 0;
    dsstrXmlNode.m_cch = cData;

    ChkDR( DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqCertificateChainsTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqCertificateChainTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

ErrorExit:
    SAFE_OEM_FREE(pData);
    SAFE_OEM_FREE(g_dastrLicenseChallenge.pszString);
    return dr;
}



/*********************************************************************
**
** Function: VerifyDeviceCertificate_GenerateLicChallenge
**
** Synopsis:    Verifies the location & data of the device/machine certificate
**          node contained within the challenge.
**
**
** Arguments:   No arguments are required.
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                             DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyDeviceCertificate_GenerateLicChallenge(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT  *pcryptoCtx = NULL;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;

    DRM_BYTE *pbData  = NULL;
    DRM_DWORD cbData  = 0;
    DRM_BYTE *pbBCert = NULL;
    DRM_DWORD cbBCert = 0;

    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;

    DRM_ANSI_CONST_STRING dastrDecryptXmlData   = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrChallengeDevCert = EMPTY_DRM_STRING;

    ChkMem( pcryptoCtx = (DRM_CRYPTO_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_CRYPTO_CONTEXT) ) );
    ZEROMEM( pcryptoCtx, SIZEOF(DRM_CRYPTO_CONTEXT) );

    ChkDR( _GenerateLicAcqChallenge( NULL ) );

/* Get the 2nd Challenge node:
**  Challenge tag & location:
**
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcquireLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <LA ...>
**                              <DATA>
**                                  ...
**                              </DATA>
**                          </LA>
**                      </Challenge>
**                  </challenge>
**              <AcquireLicense>
**          <soap:Body>
**      <soap:Envelope>
** <xml version>
**
**/

    dsstrXmlNode.m_cch = g_dastrLicenseChallenge.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLATag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrEncryptedDataTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    cbData = dsstrXmlNode.m_cch;
    ChkMem( pbData = (DRM_BYTE *) Oem_MemAlloc(cbData));

    ChkDR( _DO_NOT_USE_ExtractEncryptedData2(
           &g_oBBContextLicAqV3,
            g_dastrLicenseChallenge.pszString,
           &dsstrXmlNode,
            pbData,
           &cbData));


    dastrDecryptXmlData.pszString = (const DRM_CHAR *) pbData;
    dastrDecryptXmlData.cchString = cbData;

    dsstrXmlNode.m_ich = 0;
    dsstrXmlNode.m_cch = cbData;

/* Retrieve the device CertificateChain node :
**  tag & location:
**
** <xml version>
**      ...
**      <LA ...>
**          <Data>
**              ...
**              <CertificateChains>
**                  <CertificateChain>
**                      devicecert.dat
**                  </CertificateChain>
**                      ...
**              </CertificateChains>
**              ...
**          </Data>
**      </LA>
**      ...
** </xml version>
**/

    ChkDR( DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqCertificateChainsTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR( DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqCertificateChainTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkMem( pbCert = (DRM_BYTE*) Oem_MemAlloc(MAX_DEVICE_CERT_SIZE) );
    cbCert = MAX_DEVICE_CERT_SIZE;

    ChkDR( TST_UTL_GetPlayReadyCert(pbCert, &cbCert, pcryptoCtx) );

    dastrChallengeDevCert.pszString = dastrDecryptXmlData.pszString + dsstrXmlData.m_ich;
    dastrChallengeDevCert.cchString = cbCert;


    cbBCert = CCH_BASE64_EQUIV( cbCert );
    ChkMem( pbBCert = (DRM_BYTE*) Oem_MemAlloc( cbBCert ) );


    ChkDR( DRM_B64_EncodeA( (DRM_BYTE *)pbCert,
                            cbCert,
                            (DRM_CHAR *)pbBCert,
                           &cbBCert,
                            0 ) );

    ChkDR( MEMCMP(pbBCert, dastrChallengeDevCert.pszString , cbBCert));

ErrorExit:
    SAFE_OEM_FREE(pcryptoCtx);
    SAFE_OEM_FREE(pbBCert);
    SAFE_OEM_FREE(pbCert);
    SAFE_OEM_FREE(pbData);
    SAFE_OEM_FREE(g_dastrLicenseChallenge.pszString);
    return dr;
}




/*********************************************************************
**
** Function: VerifyDomainCertificateCollections_GenerateLicChallenge
**
** Synopsis:    Verifies the location & data of the domain certificate(s)
**          node contained within the challenge.
**
** Arguments:
**          There should be 1 or more arguments, argv[0] is the B64
**          encoded service ID (can be NULL ) to select which domain certificates
**          to be included in the challenge. argv[1], ... argv[i] should
**          be the location of files that contain the domain certificates to verify.
**          The domain certificates should be presented in the order they
**          were stored - argv[1] being the first stored domain certificate.
**
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyDomainCertificateCollections_GenerateLicChallenge(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_ANSI_CONST_STRING dastrDecryptXmlData   = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrChallengeDevCert = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData = EMPTY_DRM_SUBSTRING;

    DRM_BYTE *pData  = NULL;
    DRM_DWORD cData  = 0;
    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;
    DRM_BYTE *pbEncodedCert = NULL;
    DRM_DWORD cbEncodedCert = 0;

    DRM_DWORD iCert = 0;

    ChkArg( argc > 1 );

    ChkDR( _GenerateLicAcqChallenge( argv[ 0 ] ) );

/* Get the 2nd Challenge node:
**  Challenge tag & location:
**
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcquireLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <LA ...>
**                              <DATA>
**                                  ...
**                              </DATA>
**                          </LA>
**                      </Challenge>
**                  </challenge>
**              <AcquireLicense>
**          <soap:Body>
**      <soap:Envelope>
** <xml version>
**
**/
    dsstrXmlNode.m_cch = g_dastrLicenseChallenge.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

        ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLATag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrEncryptedDataTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    cData = dsstrXmlNode.m_cch;
    ChkMem( pData = (DRM_BYTE *) Oem_MemAlloc(cData));

    ChkDR( _DO_NOT_USE_ExtractEncryptedData2(
           &g_oBBContextLicAqV3,
            g_dastrLicenseChallenge.pszString,
           &dsstrXmlNode,
            pData,
           &cData ) );


    dastrDecryptXmlData.pszString = (const DRM_CHAR *) pData;
    dastrDecryptXmlData.cchString = cData;

    dsstrXmlNode.m_ich = 0;
    dsstrXmlNode.m_cch = cData;

/* Retrieve the device CertificateCollection node :
**  tag & location:
**
** <xml version>
**      ...
**      <LA ...>
**          <Data>
**              ...
**              <CertificateCollections>
**                      ...
**                  <CertificateCollection>
**                      <c:CertificateCollection>
**                          <c:Certificate>
**                              domaincert.dat
**                          </Certificate>
**                      </c:CertificateCollection>
**                  </CertificateCollection>
**              </CertificateCollections>
**              ...
**          </Data>
**      </LA>
**      ...
** </xml version>
**/


    ChkDR(  DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqCertificateChainsTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlData,
                1 ) );

    for(iCert = 1; iCert < (DRM_DWORD)argc; iCert++)
    {

        if(argv[ iCert ] == NULL)
        {
            break;
        }

        ChkDR(  DRM_XML_GetSubNodeA(
                    dastrDecryptXmlData.pszString,
                   &dsstrXmlSubNode,
                   &g_dastrLicAcqCertificateChainTag,
                    NULL,
                    NULL,
                    iCert,
                    NULL,
                   &dsstrXmlSubData,
                    1 ) );

        SAFE_OEM_FREE(pbCert);

        ChkArg( LoadTestFile( NULL, argv[ iCert ], &pbCert, &cbCert ) );

        cbEncodedCert = CCH_BASE64_EQUIV( cbCert );
        SAFE_OEM_FREE( pbEncodedCert );
        ChkMem( pbEncodedCert = ( DRM_BYTE * )Oem_MemAlloc( cbEncodedCert ) );

        ChkDR( DRM_B64_EncodeA( pbCert,
                                cbCert,
                                ( DRM_CHAR * )pbEncodedCert,
                                &cbEncodedCert,
                                0 ) );

        dastrChallengeDevCert.pszString = dastrDecryptXmlData.pszString + dsstrXmlSubData.m_ich;
        dastrChallengeDevCert.cchString = dsstrXmlSubData.m_cch;

        ChkBOOL( dastrChallengeDevCert.cchString == cbEncodedCert, DRM_E_FAIL );

        ChkBOOL( MEMCMP( pbEncodedCert, dastrChallengeDevCert.pszString, cbEncodedCert ) == 0, DRM_E_FAIL );
    }

ErrorExit:
    SAFE_OEM_FREE(pbEncodedCert);
    SAFE_OEM_FREE(pData);
    SAFE_OEM_FREE(pbCert);
    SAFE_OEM_FREE(g_dastrLicenseChallenge.pszString);
    return dr;
}


/*********************************************************************
**
** Function: TestDrm_LicenseAcq_GenerateChallenge
**
** Synopsis:Verifies the location & data of the content header
**          node contained within the challenge.
**
**
** Arguments:
**          There should be only 1 argument, and argv[0] should
**          be the location of a file representation of the header.
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyContentHeader_GenerateLicChallenge(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_ANSI_CONST_STRING   dastrHeaderXmlData = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData = EMPTY_DRM_SUBSTRING;

    DRM_BYTE *pbCert = NULL;
    DRM_DWORD cbCert = 0;

    ChkArg( (argc == 1) && (argv != NULL) );

    ChkDR( _GenerateLicAcqChallenge( NULL ) );


/* Get the 2nd Challenge node:
**  Challenge tag & location:
**
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcquireLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <LA ...>
**                              <DATA>
**                                  ...
**                              </DATA>
**                          </LA>
**                      </Challenge>
**                  </challenge>
**              <AcquireLicense>
**          <soap:Body>
**      <soap:Envelope>
** <xml version>
**
**/


    dsstrXmlNode.m_cch = g_dastrLicenseChallenge.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqLATag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

/* Retrieve the device ContentHeader node :
**  tag & location:
**
** <xml version>
**      ...
**      <LA ...>
**          ...
**          <ContentHeader>
**              header.xml
**          </ContentHeader>
**          ...
**      </LA>
**      ...
** </xml version>
**/

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseChallenge.pszString,
               &dsstrXmlNode,
               &g_dastrLicAcqContentHeaderTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    SAFE_OEM_FREE(pbCert);

    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbCert, &cbCert ) );

    dastrHeaderXmlData.pszString = g_dastrLicenseChallenge.pszString + dsstrXmlSubData.m_ich;
    dastrHeaderXmlData.cchString = cbCert;

    ChkDR( MEMCMP(pbCert , dastrHeaderXmlData.pszString, cbCert ) );


ErrorExit:
    SAFE_OEM_FREE(pbCert);
    SAFE_OEM_FREE(g_dastrLicenseChallenge.pszString);
    return dr;
}


/*********************************************************************
**
** Function: TestDrm_LicenseAcq_GenerateAck
**
** Synopsis: Function that generates a license acknowledgement XML.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_GenerateAck(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;

    ChkArg( argc == 1 );

    g_oLicenseResponse.m_eType = eV3Protocol;

    dr = Drm_LicenseAcq_GenerateAck( g_poAppContext,
                                     &g_oLicenseResponse,
                                     NULL,
                                     &cbChallenge );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        if (dr!= DRM_SUCCESS)
        {
            ChkDR( dr );
        }

        else
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT);
        }
    }

    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );

    ChkDR( Drm_LicenseAcq_GenerateAck( g_poAppContext,
                                       &g_oLicenseResponse,
                                       pbChallenge,
                                       &cbChallenge ) );

    LogBlob("LicAcqV3","License Ack:", pbChallenge, cbChallenge );

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );
    return dr;
}



/*********************************************************************
**
** Function: _GenerateAck
**
** Synopsis: Function that generates a license acknowledgement XML, and
**          initializes the global Blackbox context.
**
** Arguments:   No arguments are required.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL _GenerateAck( )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;

    dr = Drm_LicenseAcq_GenerateAck( g_poAppContext,
                                     &g_oLicenseResponse,
                                     NULL,
                                     &cbChallenge );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        if (dr!= DRM_SUCCESS)
        {
            ChkDR( dr );
        }
        else
        {
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT);
        }
    }


    SAFE_OEM_FREE( pbChallenge );

    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );

    ChkDR( Drm_LicenseAcq_GenerateAck( g_poAppContext,
                                       &g_oLicenseResponse,
                                       pbChallenge,
                                       &cbChallenge ) );

    LogBlob("LicAcqV3","License Ack:", pbChallenge, cbChallenge );

    SAFE_OEM_FREE( g_dastrLicenseAcknowledgement.pszString );
    g_dastrLicenseAcknowledgement.cchString = cbChallenge;
    g_dastrLicenseAcknowledgement.pszString = (const DRM_CHAR*)pbChallenge;
    pbChallenge = NULL;

    ChkDR( _BlackBoxInit( &g_oBBContextLicAqV3 ) );

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );
    return dr;
}


/*********************************************************************
**
** Function: VerifyTransactionID_GenerateAck
**
** Synopsis: Verifies the location & data of the transaction-id within the acknowledgement.
**
** Arguments:   No arguments are required.
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyTransactionID_GenerateAck(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_ANSI_CONST_STRING dastrTransactionIDXmlData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTransactionID              = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;

    DRM_CHAR *pchTransID = NULL;
    DRM_DWORD cchTransID = 0;

    ChkDR( _GenerateAck() );

/* Get the Ack node:
**  Ack tag & location:
**
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcknowledgeLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <Ack ...>
**                              <TransactionID>
**                                  ...
**                              </TransactionID>
**                          </Ack>
**                      </Challenge>
**                  </challenge>
**              </AcknowledgeLicense>
**          </soap:Body>
**      </soap:Envelope>
** </xml version>
**
**/

    dsstrXmlNode.m_cch = g_dastrLicenseAcknowledgement.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckAckTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

/* Get the Transaction node & Verify contents:
**  <Ack>
**      ...
**      <TransactionID>
**      </TransactionID>
**      ...
**  </Ack>
**/

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckTransactionIDTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );


    dastrTransactionIDXmlData.pszString = g_dastrLicenseAcknowledgement.pszString + dsstrXmlData.m_ich;
    dastrTransactionIDXmlData.cchString =  dsstrXmlData.m_cch;

    DSTR_FROM_PB(&dstrTransactionID, g_oLicenseResponse.m_rgbTransactionID, g_oLicenseResponse.m_cbTransactionID);

    /* Safe to use, input parameter is ASCII */
    DRM_UTL_DemoteUNICODEtoASCII(dstrTransactionID.pwszString, pchTransID, cchTransID);

    ChkDR( MEMCMP(dastrTransactionIDXmlData.pszString,  pchTransID , cchTransID));


ErrorExit:
    SAFE_OEM_FREE(pchTransID);
    SAFE_OEM_FREE(g_dastrLicenseAcknowledgement.pszString);
    return dr;
}



/*********************************************************************
**
** Function: VerifyLicenseStorageResults_GenerateAck
**
** Synopsis: Verifies the location & data of the license storage result & KID
**          node contained within the Acknowledgement.
**
** Arguments:   No arguments are required.
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyLicenseStorageResults_GenerateAck(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData = EMPTY_DRM_SUBSTRING;

    DRM_ANSI_CONST_STRING dastrLicenseData = EMPTY_DRM_STRING;

    DRM_CHAR *pchResult = NULL;
    DRM_DWORD cchResult = 0;
    DRM_CHAR *pchKID    = NULL;
    DRM_DWORD cchKID    = 0;

    DRM_DWORD iLicense = 0;

    ChkDR( _GenerateAck() );

/* Get the Ack node:
**  Ack tag & location:
**
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcknowledgeLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <Ack ...>
**                              ...
**                          </Ack>
**                      </Challenge>
**                  </challenge>
**              </AcknowledgeLicense>
**          </soap:Body>
**      </soap:Envelope>
** </xml version>
**
**/


    dsstrXmlNode.m_cch = g_dastrLicenseAcknowledgement.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckAckTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

/* Get the LicenseStorage node & Verify contents:
**  <Ack>
**      ...
**      <LicenseStorageResults>
**          <License>
**              <KID>...</KID>
**              <Result>...</Result>
**          </License>
**      </LicenseStorageResults>
**      ...
**  </Ack>
**/

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckLicenseStorageResultsTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    cchKID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    ChkMem( pchKID = ( DRM_CHAR * )Oem_MemAlloc( cchKID ) );

    for(iLicense = 0; iLicense < g_oLicenseResponse.m_cAcks; iLicense++)
    {

        ChkDR(  DRM_XML_GetSubNodeA(
                    g_dastrLicenseAcknowledgement.pszString,
                   &dsstrXmlNode,
                   &g_dastrLicAckLicenseTag,
                    NULL,
                    NULL,
                    iLicense,
                   &dsstrXmlSubNode,
                   &dsstrXmlSubData,
                    1 ) );

        ChkDR(  DRM_XML_GetSubNodeA(
                    g_dastrLicenseAcknowledgement.pszString,
                   &dsstrXmlSubNode,
                   &g_dastrLicAckKIDTag,
                    NULL,
                    NULL,
                    0,
                   &dsstrXmlSubNode,
                   &dsstrXmlSubData,
                    1 ) );

        dastrLicenseData.pszString = g_dastrLicenseAcknowledgement.pszString + dsstrXmlSubData.m_ich;

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&g_oLicenseResponse.m_rgoAcks[ iLicense ].m_oKID,
                                    SIZEOF( DRM_GUID ),
                                    pchKID,
                                    &cchKID,
                                    0 ) );

        ChkDR( MEMCMP(dastrLicenseData.pszString, pchKID ,  cchKID) );


        ChkDR(  DRM_XML_GetSubNodeA(
                    g_dastrLicenseAcknowledgement.pszString,
                   &dsstrXmlNode,
                   &g_dastrLicAckLicenseTag,
                    NULL,
                    NULL,
                    iLicense,
                   &dsstrXmlSubNode,
                   &dsstrXmlSubData,
                    1 ) );

        ChkDR(  DRM_XML_GetSubNodeA(
                    g_dastrLicenseAcknowledgement.pszString,
                   &dsstrXmlSubNode,
                   &g_dastrLicAckResultTag,
                    NULL,
                    NULL,
                    0,
                   &dsstrXmlSubNode,
                   &dsstrXmlSubData,
                    1 ) );

        dastrLicenseData.pszString = g_dastrLicenseAcknowledgement.pszString + dsstrXmlSubData.m_ich;

        SAFE_OEM_FREE( pchResult );
        ChkMem(pchResult = (DRM_CHAR*) Oem_MemAlloc( DRM_MAX_CCH_BASE16_DWORD_STRING + 1 ));

        cchResult = TST_UTL_NumberToAnsiString(
                               (DRM_DWORD) g_oLicenseResponse.m_rgoAcks[ iLicense ].m_dwResult,
                                           16,
                                           DRM_MAX_CCH_BASE16_DWORD_STRING,
                                           DRM_MAX_CCH_BASE16_DWORD_STRING + 1,
                                           pchResult);

        ChkDR( MEMCMP(dastrLicenseData.pszString, "0X" , 2) );
        ChkDR( MEMCMP(dastrLicenseData.pszString + 2, pchResult, cchResult) );

    }

ErrorExit:
    SAFE_OEM_FREE( pchKID );
    SAFE_OEM_FREE( pchResult );
    SAFE_OEM_FREE( g_dastrLicenseAcknowledgement.pszString );
    return dr;
}


/*********************************************************************
**
** Function: VerifyDeviceCertificate_GenerateAck
**
** Synopsis: Verifies the location & data of the device certificate(s)
**          node contained within the challenge.
**
** Arguments: No arguments are required.
**
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyDeviceCertificate_GenerateAck(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CRYPTO_CONTEXT  *pcryptoCtx = NULL;

    DRM_ANSI_CONST_STRING dastrDecryptXmlData   = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrChallengeDevCert = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData    = EMPTY_DRM_SUBSTRING;

    DRM_BYTE *pData   = NULL;
    DRM_DWORD cData   = 0;
    DRM_BYTE *pbBCert = NULL;
    DRM_DWORD cbBCert = 0;
    DRM_BYTE *pbCert  = NULL;
    DRM_DWORD cbCert  = 0;

    ChkMem( pcryptoCtx = (DRM_CRYPTO_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_CRYPTO_CONTEXT) ) );
    ZEROMEM( pcryptoCtx, SIZEOF(DRM_CRYPTO_CONTEXT) );

    ChkDR( _GenerateAck() );

/* Get the ack node:
**  ack tag & location:
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcknowledgeLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <Ack ...>
**                              ...
**                          </Ack>
**                      </Challenge>
**                  </challenge>
**              </AcknowledgeLicense>
**          </soap:Body>
**      </soap:Envelope>
** </xml version>
**
**/

    dsstrXmlNode.m_cch = g_dastrLicenseAcknowledgement.cchString;
    dsstrXmlNode.m_ich = 0;

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckAckTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

/* Retrieve the device CertificateCollection node :
**  tag & location:
**
** <xml version>
**      ...
**      <ack>
**          <Data>
**              ...
**              <CertificateCollections>
**              </CertificateCollections>
**              ...
**          </Data>
**      </Challenge>
**      ...
** </xml version>
**/

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrEncryptedDataTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    cData = dsstrXmlNode.m_cch;

    ChkMem( pData = (DRM_BYTE *) Oem_MemAlloc(cData));

    ChkDR(  _DO_NOT_USE_ExtractEncryptedData2(
                &g_oBBContextLicAqV3,
                 g_dastrLicenseAcknowledgement.pszString,
                &dsstrXmlNode,
                 pData,
                &cData));

    dastrDecryptXmlData.pszString = (const DRM_CHAR *) pData;
    dastrDecryptXmlData.cchString = cData;

    dsstrXmlNode.m_ich = 0;
    dsstrXmlNode.m_cch = cData;

    ChkDR(  DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckCertificateChainTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkMem( pbCert = (DRM_BYTE*) Oem_MemAlloc(MAX_DEVICE_CERT_SIZE) );
    cbCert = MAX_DEVICE_CERT_SIZE;

    ChkDR( TST_UTL_GetPlayReadyCert(pbCert, &cbCert, pcryptoCtx) );

    dastrChallengeDevCert.pszString = dastrDecryptXmlData.pszString + dsstrXmlData.m_ich;
    dastrChallengeDevCert.cchString = cbCert;

    cbBCert = CCH_BASE64_EQUIV(cbCert);
    ChkMem( pbBCert = (DRM_BYTE*) Oem_MemAlloc(cbBCert) );


    ChkDR( DRM_B64_EncodeA( (DRM_BYTE *)pbCert,
                            cbCert,
                            (DRM_CHAR *)pbBCert,
                           &cbBCert,
                            0 ) );

    ChkDR( MEMCMP(pbBCert, dastrChallengeDevCert.pszString , cbBCert));

ErrorExit:
    SAFE_OEM_FREE( pcryptoCtx );
    SAFE_OEM_FREE( pbBCert );
    SAFE_OEM_FREE( pData );
    SAFE_OEM_FREE( pbCert );
    return dr;
}



/*********************************************************************
**
** Function: VerifyBasicFormat_GenerateAck
**
** Synopsis: Verifies the basic xml format of the of the v3 License Ack,
**          and that all required nodes are present.
**
** Arguments:   No arguments are required.
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_RESULT DRM_CALL VerifyBasicFormat_GenerateAck(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_ANSI_CONST_STRING dastrDecryptXmlData = EMPTY_DRM_STRING;

    DRM_SUBSTRING dsstrXmlNode       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubNode    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlData       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubSubNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrXmlSubSubData = EMPTY_DRM_SUBSTRING;

    DRM_BYTE *pData = NULL;
    DRM_DWORD cData = 0;

    ChkDR( _GenerateAck() );

    dsstrXmlNode.m_cch = g_dastrLicenseAcknowledgement.cchString;
    dsstrXmlNode.m_ich = 0;

/* Check that the ack is in the correct place..
**
**  ack tag & location:
** <xml version>
**      <soap:Envelope>
**          <soap:Body>
**              <AcknowledgeLicense>
**                  <challenge>
**                      <Challenge ...>
**                          <Ack ...>
**                              ...
**                          </Ack>
**                      </Challenge>
**                  </challenge>
**              </AcknowledgeLicense>
**          </soap:Body>
**      </soap:Envelope>
** </xml version>
**
**/

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrSoapBodyTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckRootTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge1Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

/* Check that the ack contains Challenge, Version, Ack & Signature nodes.
**
**  tags & location:
**
**  <Challenge ...>
**      <Ack ...>
**          <Version> ... </Version>
**          <TransactionID> ... </TransactionID>
**          <LicenseStorage Results> ... </LicenseStorage Results>
**          <Data> ... </Data>
**      </Ack>
**      <Signature> ... </Signature>
**  </Challenge>
**
**/
    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckChallenge2Tag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrSignatureTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckAckTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );

/* Check that the Ack node contains the correct subnodes.
**
**  tags & location:
**          <Ack>
**              <Version> ... </Version>
**              <Data>
**                  <CertificateCollection> ... </CertificateCollection>
**              </Data>
**              <TransactionID> ... </TransactionID>
**              <LicenseStorageResults> ... </LicenseStorageResults>
**          </Ack>
**/


    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckVersionTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckTransactionIDTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckLicenseStorageResultsTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrReqTagClientInfo,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlSubData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlSubNode,
               &g_dastrReqTagClientVersion,
                NULL,
                NULL,
                0,
               &dsstrXmlSubSubNode,
               &dsstrXmlSubSubData,
                1 ) );

    ChkDR(  DRM_XML_GetSubNodeA(
                g_dastrLicenseAcknowledgement.pszString,
               &dsstrXmlNode,
               &g_dastrEncryptedDataTag,
                NULL,
                NULL,
                0,
               &dsstrXmlNode,
               &dsstrXmlData,
                1 ) );




    cData = dsstrXmlNode.m_cch;
    ChkMem(pData = (DRM_BYTE *) Oem_MemAlloc(cData));

    ChkDR( _DO_NOT_USE_ExtractEncryptedData2(
                      &g_oBBContextLicAqV3,
                       g_dastrLicenseAcknowledgement.pszString,
                      &dsstrXmlNode,
                       pData,
                      &cData));



    dastrDecryptXmlData.pszString = (const DRM_CHAR *) pData;
    dastrDecryptXmlData.cchString = cData;

    dsstrXmlNode.m_ich = 0;
    dsstrXmlNode.m_cch = cData;

    ChkDR(  DRM_XML_GetSubNodeA(
                dastrDecryptXmlData.pszString,
               &dsstrXmlNode,
               &g_dastrLicAckCertificateChainTag,
                NULL,
                NULL,
                0,
               &dsstrXmlSubNode,
               &dsstrXmlData,
                1 ) );

ErrorExit:
    SAFE_OEM_FREE( pData );
    SAFE_OEM_FREE( g_dastrLicenseAcknowledgement.pszString );
    return dr;
}


/*********************************************************************
**
** Function: TestDrm_LicenseAcq_ProcessResponse
**
** Synopsis: Function that processes a license challenge response from
**           the server.
**           argv[0] is the file that contains the license challenge response.
**           argv[1] is used to instruct the code to do something special.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_BYTE  *pbResponse    = NULL;
    DRM_DWORD  cbResponse    = 0;
    DRM_DWORD *pcbResponse   = &cbResponse;
    DRM_DWORD  dwSpecialMode = 0;
    DRM_BYTE  *pbDataIn      = NULL;
    DRM_DWORD  cbDataIn      = 0;

    ChkArg( argc == 1 || argc == 2 );

    if ( argc == 2 )
    {
        dwSpecialMode = (DRM_DWORD)atoi( argv[ 1 ] );
    }

    if( NULL != argv[0] )
    {
        ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbResponse, pcbResponse ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR_EX( pbResponse, pcbResponse, RESPONSE_VAR );
    }

    pbDataIn = pbResponse;
    cbDataIn = *pcbResponse;

    if ( dwSpecialMode != 0 )
    {
        if ( dwSpecialMode == 1 )
        {
            pbDataIn = NULL;
        }
        else if ( dwSpecialMode == 2 )
        {
            cbDataIn = 0;
        }
    }

    g_oLicenseResponse.m_eType = eUnknownProtocol;
    g_oLicenseResponse.m_dwResult = DRM_SUCCESS;

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( Drm_LicenseAcq_ProcessResponse( g_fUseNullAppContext ? NULL: g_poAppContext,
                                           DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                           NULL,
                                           NULL,
                                           pbDataIn,
                                           cbDataIn,
                                           &g_oLicenseResponse ) );
PREFAST_POP

ErrorExit:
    if( NULL != argv[0] )
    {
        SAFE_OEM_FREE( pbResponse );
    }
    return dr;
}

/*********************************************************************
**
** Function: TestDrm_LicenseAcq_ProcessResponse_LicResult
**
** Synopsis: Function that processes a license challenge response from
**           the server, and check the storage results of a particular license
**           against an expected result.
**
**           argv[0] is the file that contains the license challenge response.
**           argv[1] is the KID of the license.
**           argv[2] is the LID of the license.
**           argv[3] is the expected result.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse_LicResult(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pbResponse  = NULL;
    DRM_DWORD  cbResponse  = 0;
    DRM_DWORD *pcbResponse = &cbResponse;
    DRM_BYTE  *pbDataIn    = NULL;
    DRM_DWORD  cbDataIn    = 0;
    DRM_DWORD  iLicense    = 0;

    DRM_CHAR *pchKID    = NULL;
    DRM_DWORD cchKID    = 0;
    DRM_CHAR *pchLID    = NULL;
    DRM_DWORD cchLID    = 0;

    DRM_RESULT matchKID = FALSE;
    DRM_RESULT matchLID = FALSE;
    DRM_RESULT matchLicense = FALSE;

    ChkArg( argc == 4);

    if( NULL != argv[0] )
    {
        ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbResponse, pcbResponse ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR_EX( pbResponse, pcbResponse, RESPONSE_VAR );
    }

    pbDataIn = pbResponse;
    cbDataIn = *pcbResponse;

    g_oLicenseResponse.m_eType = eUnknownProtocol;
    g_oLicenseResponse.m_dwResult = DRM_SUCCESS;

    ChkDR( Drm_LicenseAcq_ProcessResponse( g_fUseNullAppContext ? NULL: g_poAppContext,
                                           DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                           NULL,
                                           NULL,
                                           pbDataIn,
                                           cbDataIn,
                                           &g_oLicenseResponse ) );

    for(iLicense = 0; iLicense < g_oLicenseResponse.m_cAcks; iLicense++)
    {

        /* Look for a stored license with the correct KID */

        cchKID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
        SAFE_OEM_FREE( pchKID );
        ChkMem(pchKID = (DRM_CHAR*) Oem_MemAlloc( cchKID ));
        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&g_oLicenseResponse.m_rgoAcks[ iLicense ].m_oKID,
                                    SIZEOF( DRM_GUID ),
                                    pchKID,
                                    &cchKID,
                                    0 ) );
        matchKID = MEMCMP( argv[1] , pchKID ,  cchKID) ;

        if ( matchKID == 0 )
        {

            /* After the correct KID is found, check for a matching LID */

            cchLID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
            SAFE_OEM_FREE( pchLID );
            ChkMem(pchLID = (DRM_CHAR*) Oem_MemAlloc( cchLID ));

            ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&g_oLicenseResponse.m_rgoAcks[ iLicense ].m_oLID,
                                        SIZEOF( DRM_GUID ),
                                        pchLID,
                                        &cchLID,
                                        0 ) );
            matchLID = MEMCMP( argv[2] , pchLID ,  cchLID) ;

            if ( matchLID == 0 )
            {
                DRM_RESULT drExpected = (DRM_RESULT) strtoul( argv[3], NULL, 0 );
                matchLicense = TRUE;

                /* After the correct license is found, check agaisnt the expected storage result */
                ChkBOOL( g_oLicenseResponse.m_rgoAcks[ iLicense ].m_dwResult == drExpected , DRM_E_TEST_UNEXPECTED_OUTPUT );

            }


        }

    }

    if( !matchLicense )
    {
        ChkDR( DRM_E_LICENSE_NOT_FOUND );
    }

ErrorExit:
    if( ( argc >= 1 ) && ( NULL != argv[0] ) )
    {
        SAFE_OEM_FREE( pbResponse );
    }
    SAFE_OEM_FREE( pchKID );
    SAFE_OEM_FREE( pchLID );

    return dr;
}

/*********************************************************************
**
** Function: TestDrm_LicenseAcq_ProcessResponse_ServerErrorChecking
**
** Synopsis: Function meant to process a server error response.
**
**           argv[0] is the file that contains the license challenge response.
**           argv[1] should be expected server error code returned in the challenge.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse_ServerErrorChecking(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_BYTE  *pbResponse  = NULL;
    DRM_DWORD  cbResponse  = 0;
    DRM_DWORD *pcbResponse = &cbResponse;
    DRM_RESULT errorCode   = 0;
    DRM_BYTE  *pbDataIn    = NULL;
    DRM_DWORD  cbDataIn    = 0;

    ChkArg( argc == 1 || argc == 2 );

    if ( argc == 2 )
    {
        errorCode = strtol( argv[ 1 ] , NULL , 0 );
    }

    if( NULL != argv[0] )
    {
        ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbResponse, pcbResponse ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR_EX( pbResponse, pcbResponse, RESPONSE_VAR );
    }

    pbDataIn = pbResponse;
    cbDataIn = *pcbResponse;

    g_oLicenseResponse.m_eType = eUnknownProtocol;
    g_oLicenseResponse.m_dwResult = DRM_SUCCESS;

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( Drm_LicenseAcq_ProcessResponse( g_fUseNullAppContext ? NULL: g_poAppContext,
                                           DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                           NULL,
                                           NULL,
                                           pbDataIn,
                                           cbDataIn,
                                           &g_oLicenseResponse ) );
PREFAST_POP

    ChkArg( g_oLicenseResponse.m_dwResult == errorCode );

ErrorExit:
    if( NULL != argv[0] )
    {
        SAFE_OEM_FREE( pbResponse );
    }

    return dr;
}


/*********************************************************************
**
** Function: TestDrm_LicenseAcq_ProcessResponse_ResponseVerification
**
** Synopsis: Function that processes a license challenge response from
**           the server.
**           argv[0] is the file that contains the license challenge response.
**           argv[1] is the flag, 0 to opt-in and 1 to opt-out response verification.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessResponse_ResponseVerification(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_BYTE  *pbResponse    = NULL;
    DRM_DWORD  cbResponse    = 0;
    DRM_DWORD *pcbResponse   = &cbResponse;
    DRM_DWORD  dwFlags       = 0;
    DRM_BYTE  *pbDataIn      = NULL;
    DRM_DWORD  cbDataIn      = 0;

    ChkArg( argc == 2 );

    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbResponse, pcbResponse ) );
    dwFlags = (DRM_DWORD)strtol( argv[ 1 ] , NULL , 0 );

    pbDataIn = pbResponse;
    cbDataIn = *pcbResponse;

    g_oLicenseResponse.m_eType = eUnknownProtocol;
    g_oLicenseResponse.m_dwResult = DRM_SUCCESS;

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_INVALID_PARAM_VALUE_1, "Test code is potentially passing invalid parameters intentionally." );
    ChkDR( Drm_LicenseAcq_ProcessResponse( g_fUseNullAppContext ? NULL: g_poAppContext,
                                           (DRM_PROCESS_LIC_RESPONSE_FLAG)dwFlags,
                                           NULL,
                                           NULL,
                                           pbDataIn,
                                           cbDataIn,
                                           &g_oLicenseResponse ) );
PREFAST_POP

ErrorExit:
    if( NULL != argv[0] )
    {
        SAFE_OEM_FREE( pbResponse );
    }

    return dr;
}

/*********************************************************************
**
** Function: TestDRM_LA_ValidateResponseId
**
** Synopsis: Function that validates responseId in a license challenge response
**           from the server.
**           argv[0] is the file that contains the license challenge response.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDRM_LA_ValidateResponseId(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT           dr            = DRM_SUCCESS;
    DRM_BYTE            *pbResponse    = NULL;
    DRM_DWORD            cbResponse    = 0;
    DRM_DWORD           *pcbResponse   = &cbResponse;
    const DRM_BYTE      *pbDataIn      = NULL;
    DRM_DWORD            cbDataIn      = 0;
    DRM_BCERT_CLIENT_ID  oClientId     = {0};

    ChkArg( argc == 1 );

    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbResponse, pcbResponse ) );

    pbDataIn = pbResponse;
    cbDataIn = *pcbResponse;

    g_oLicenseResponse.m_eType = eUnknownProtocol;
    g_oLicenseResponse.m_dwResult = DRM_SUCCESS;

    memset( &oClientId, 0x0A, SIZEOF(DRM_BCERT_CLIENT_ID) );

    ChkDR( DRM_LA_ValidateResponseId( pbDataIn,
                                      cbDataIn,
                                      (DRM_BYTE *)&oClientId,
                                      SIZEOF( oClientId ) ) );

ErrorExit:
    if( NULL != argv[0] )
    {
        SAFE_OEM_FREE( pbResponse );
    }

    return dr;
}

/*********************************************************************
**
** Function: TestDrm_LicenseAcq_ProcessAckResponse
**
** Synopsis: Function that processes a license acknowledgement response
**           from the server.
**           argv[0] is the file that contains the license acknowledgement
**           response.
**           argv[1] is used to instruct the code to do something special.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- Pointer to a array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestDrm_LicenseAcq_ProcessAckResponse(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_DWORD dwSpecialMode = 0;
    DRM_BYTE *pbDataIn = NULL;
    DRM_DWORD cbDataIn = 0;
    DRM_RESULT dr1 = DRM_SUCCESS;

    ChkArg( argc == 1 || argc == 2 );

    if ( argc == 2 )
    {
        dwSpecialMode = (DRM_DWORD)atoi( argv[1] );
    }

    ChkArg( LoadTestFile( NULL, argv[ 0 ], &pbResponse, &cbResponse ) );

    pbDataIn = pbResponse;
    cbDataIn = cbResponse;

    if ( dwSpecialMode != 0 )
    {
        if ( dwSpecialMode == 1 )
        {
            pbDataIn = NULL;
        }
        else if ( dwSpecialMode == 2 )
        {
            cbDataIn = 0;
        }
    }

    ChkDR( Drm_LicenseAcq_ProcessAckResponse( g_fUseNullAppContext ? NULL: g_poAppContext,
                                              pbDataIn,
                                              cbDataIn,
                                              &dr1 ) );

ErrorExit:
    SAFE_OEM_FREE( pbResponse );
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
DRM_RESULT DRM_CALL MODULE_PRETESTCASE( LicAcqV3 )(
    long lTCID,
    const char *strTCName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING *pdstrStoreName = NULL;

    FREE_SHARED_APP_CONTEXT( g_poAppContext, pdstrStoreName  );

    RemoveDRMFile( RMFILE_ALL );

    ChkDR( SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME,
                         PRIVATE_KEY_FILE_NAME,
                         GROUP_CERT_FILE_NAME,
                         GC_PRIVATE_KEY_FILE_NAME,
                         TRUE ) );

    INIT_SHARED_APP_CONTEXT( g_poAppContext, pdstrStoreName );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        ChkMem( g_pbRevocationBuffer = ( DRM_BYTE * )Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );
        ChkDR( Drm_Revocation_SetBuffer( g_poAppContext, g_pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    }

    g_fUseNullAppContext = FALSE;

    g_oLicenseResponse.m_cAcks = 0;

    if( g_oBBContextLicAqV3.pKeyFileContext == NULL )
    {
        ChkMem( g_oBBContextLicAqV3.pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
        ChkDR( DRM_KF_Initialize( NULL, g_oBBContextLicAqV3.pKeyFileContext ) );
        ChkDR( DRM_KF_Open( g_oBBContextLicAqV3.pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
        ChkDR( DRM_KF_LoadFromFile( g_oBBContextLicAqV3.pKeyFileContext, TRUE ) );
        ChkDR( DRM_BBX_SetupSecureStoreKey( &g_oBBContextLicAqV3, NULL ) );
        ChkDR( DRM_KF_Close( g_oBBContextLicAqV3.pKeyFileContext ) );
    }

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
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( LicAcqV3 )(
    long lTCID,
    const char *strTCName )
{
    while (g_numRights > 0)
    {
        g_numRights--;
        FREEDRMSTRINGPOINTER( g_pwszRights[ g_numRights ] );
        SAFE_OEM_FREE( g_pwszRights[ g_numRights ] );
    }

    if( g_oBBContextLicAqV3.pKeyFileContext != NULL )
    {
        DRM_KF_Uninitialize(g_oBBContextLicAqV3.pKeyFileContext);
        SAFE_OEM_FREE(g_oBBContextLicAqV3.pKeyFileContext);
    }
    DRM_BBX_Shutdown( &g_oBBContextLicAqV3 );

    SAFE_OEM_FREE( g_pbRevocationBuffer );
    SAFE_OEM_FREE( g_dastrLicenseAcknowledgement.pszString );
    SAFE_OEM_FREE( g_dastrLicenseChallenge.pszString );

    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST( LicAcqV3 )
    API_ENTRY( _UseNullAppContext )
    API_ENTRY( _AddDomainCert )
    API_ENTRY( _SetRights )
    API_ENTRY( _SetHeader )
    API_ENTRY( _AddLicenseAcknowledgement )
    API_ENTRY( _SetTransactionID )
    API_ENTRY( TestDrm_LicenseAcq_GenerateChallenge )
    API_ENTRY( TestDrm_LicenseAcq_GenerateAck )
    API_ENTRY( TestDrm_LicenseAcq_ProcessResponse )
    API_ENTRY( TestDrm_LicenseAcq_ProcessResponse_ServerErrorChecking )
    API_ENTRY( TestDrm_LicenseAcq_ProcessResponse_LicResult )
    API_ENTRY( TestDrm_LicenseAcq_ProcessResponse_ResponseVerification )
    API_ENTRY( TestDrm_LicenseAcq_ProcessAckResponse )
    API_ENTRY( TestDRM_LA_ValidateResponseId )
    API_ENTRY( VerifyBasicFormat_GenerateLicChallenge )
    API_ENTRY( VerifyDeviceCertificate_GenerateLicChallenge )
    API_ENTRY( VerifyContentHeader_GenerateLicChallenge )
    API_ENTRY( VerifyDomainCertificateCollections_GenerateLicChallenge )
    API_ENTRY( VerifyTransactionID_GenerateAck )
    API_ENTRY( VerifyLicenseStorageResults_GenerateAck )
    API_ENTRY( VerifyDeviceCertificate_GenerateAck )
    API_ENTRY( VerifyBasicFormat_GenerateAck )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

