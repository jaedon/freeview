/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <LiveTVTestAPI.h>
#include <drmxmrbuilder.h>
#include <drmxmrparser.h>
#include <drmkeygenerator.h>
#include <DRMManagerTestAPI.h>
#include <drmderivedkey.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define DRM_STACK_SIZE          DRM_MAX_LICENSESIZE

#define MAX_AUX_KEY_ENTRIES     ( 1250 + 64 * 8 )
#define MAX_UPLINKX_ENTRIES     ( 128 + 1 )

#define ROOT_LOCAITON           0x8000

typedef struct
{
    DRM_ID      m_oDrmidKID;
    DRM_ID      m_oDrmidLID;
    PUBKEY_P256 m_oPubKey;
    DRM_XMR_PLAYBACK_RIGHTS     m_oXmrPlay;
    DRM_XMR_MINIMUM_ENVIRONMENT m_oXmrMinEnv;
    DRM_XMR_CONTENT_KEY         m_oContentKey;
    DRM_XMR_ECC_DEVICE_KEY      m_oDeviceKey;
    CIPHERTEXT_P256             m_oEncryptedContentKey;
    DRM_XMR_REVOCATION_INFORMATION_VERSION m_oXmrRIV;
    DRM_BYTE    m_rgbChecksum[ DRM_AES_CK_CHECKSUM_LENGTH ];
    DRM_ID      m_oUplinkKID;
    DRM_BYTE    m_rgbContentKey[ DRM_AES_KEYSIZE_128 * 2 ];
} XMR_BUILDER_STATE;

static DRM_XMR_BUILDER_CONTEXT  g_oXmrBuilder;
static DRM_XMR_LICENSE          g_oXmrLicense;
static DRM_BYTE                 g_rgbXmrBuilderStack[ DRM_STACK_SIZE ];
static DRM_BYTE                 g_rgbXmrParserStack[ DRM_STACK_SIZE ];
static DRM_BYTE                 g_rgbLicense[ DRM_MAX_LICENSESIZE ];
static DRM_DWORD                g_cbLicense = DRM_MAX_LICENSESIZE;
static DRM_XMR_AUX_KEY          g_oAuxKey;
static DRM_XMR_AUX_KEY_ENTRY    g_rgoAuxKeyEntries[ MAX_AUX_KEY_ENTRIES ];
static DRM_XMR_UPLINKX          g_oUplinkX;
static DRM_DWORD                g_rgdwLocations[ MAX_UPLINKX_ENTRIES ];

static XMR_BUILDER_STATE        g_oXMRState = { 0 };

static DRM_BYTE g_rgbMasterKey[DRM_AES_KEYSIZE_128] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                                        0, 0, 0, 0, 0, 0, 0, 0 };

static DRM_BYTE g_rgbFixedSecondaryKey[ DRM_AES_KEYSIZE_128 ] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };

static DRM_ID g_rgbSampleDRMID = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };

static DRM_RESULT _CreateChecksum(
    __in const DRM_KID *                                f_poKID,
    __in_bcount( f_cbKey ) const DRM_BYTE *             f_pbKey,
    __in DRM_DWORD                                      f_cbKey,
    __out_bcount( DRM_AES_CK_CHECKSUM_LENGTH )DRM_BYTE* f_pbChecksum
    )
{
    DRM_RESULT  dr                                          = DRM_SUCCESS;
    DRM_BYTE    rgbBlock[ __CB_DECL( SIZEOF( DRM_KID ) ) ]  = { 0 };
    DRM_AES_KEY oAesKey                                     = { 0 };

    /* Skip parameter verification for internal function. */

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );
    MEMCPY( rgbBlock, f_poKID->rgb, SIZEOF( DRM_KID ) );

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbBlock, SIZEOF( DRM_KID ) ) );
    MEMCPY( f_pbChecksum, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    return dr;
}

static DRM_RESULT _CalcNextKey(
    DRM_BOOL                                        f_fDeriveRight,
    __inout_bcount( DRM_AES_KEYSIZE_128 ) DRM_BYTE *f_pbKey
    )
{
    DRM_RESULT  dr                                  = DRM_SUCCESS;
    DRM_AES_KEY oAesKey                             = { 0 };
    DRM_BYTE    rgbResult[ DRM_AES_KEYSIZE_128 ]    = { 0, 0, 0, 0, 0, 0, 0, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0 };

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );

    if ( f_fDeriveRight )
    {
        rgbResult[ 0 ] = 0x01;
    }

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbResult, DRM_AES_KEYSIZE_128 ) );

    MEMCPY( f_pbKey, rgbResult, DRM_AES_KEYSIZE_128 );

ErrorExit:
    return dr;
}

static DRM_RESULT _DeriveKey(
    DRM_DWORD                                       f_dwLocation,
    __out_bcount( DRM_AES_KEYSIZE_128 ) DRM_BYTE *  f_pbKey
    )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   dwParent    = ROOT_LOCAITON;
    DRM_DWORD   dwLPParent  = 0, dwSBMParent = 0;
    DRM_BOOL    fDeriveRight= FALSE;

    MEMCPY( f_pbKey, g_rgbMasterKey, DRM_AES_KEYSIZE_128 );

    while ( dwParent != f_dwLocation )
    {
        dwLPParent  = LP( dwParent );
        dwSBMParent = SBM( dwParent );

        fDeriveRight = ( f_dwLocation & dwSBMParent ) != 0;
        ChkDR( _CalcNextKey( fDeriveRight, f_pbKey ) );

        dwParent = ( dwParent & ( ~dwSBMParent ) |
                   ( f_dwLocation & dwSBMParent ) |
                   ( dwSBMParent >> 1 ) );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _SignXmrLicense
**
** Synopsis: This is implementation of DRM_XMR_CB_SIGNATURE callback function
**
** Arguments:
**
** [f_pvSignatureContext]   - Pointer to a context that the caller uses to
**                            retrieve the integrity key.
** [f_pbLicense]            - Pointer to license buffer.
** [f_cbLicense]            - Size of license buffer.
** [f_pbSignatureBuffer]    - Pointer to base buffer to write the signature.
** [f_cbSignatureBuffer]    - Size of signature buffer.
** [f_ibSignatureBuffer]    - Offset (index) within the base buffer to write
**                            the signature.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
static DRM_RESULT DRM_CALL _SignXmrLicense(
    __in const DRM_VOID *                           f_pvSignatureContext,
    __in_bcount( f_cbLicense ) const DRM_BYTE *     f_pbLicense,
    __in const DRM_DWORD                            f_cbLicense,
    __out_bcount( f_cbSignatureBuffer )DRM_BYTE *   f_pbSignatureBuffer,
    __in const DRM_DWORD                            f_cbSignatureBuffer,
    __in const DRM_DWORD                            f_ibSignatureBuffer
    )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_AES_KEY oIntegrityKey   = { 0 };

    if ( f_pvSignatureContext != NULL )
    {
        /* Sanity check the arguments. */
        ChkArg( f_pbSignatureBuffer != NULL );
        ChkArg( f_cbSignatureBuffer - f_ibSignatureBuffer >= DRM_AES_BLOCKLEN );
        ChkArg( f_pbLicense != NULL );
        ChkArg( f_cbLicense >  0 );
        ChkArg( f_pbLicense == f_pbSignatureBuffer );
        ChkArg( f_ibSignatureBuffer >  f_cbLicense );

        /* Sign the license with integrity key. */
        ChkDR( Oem_Aes_SetKey( ( DRM_BYTE * )f_pvSignatureContext,
                               &oIntegrityKey ) );

        ChkDR( Oem_Omac1_Sign( &oIntegrityKey,
                               f_pbLicense,
                               0,
                               f_cbLicense,
                               f_pbSignatureBuffer + f_ibSignatureBuffer ) );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestStartLicense
**
** Synopsis: Function that starts the construction of a baseline XMR
**           license. More objects may be added in other functions.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestStartLicense(
    __in long                   argc,
    __in_ecount( argc ) char ** argv )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           dwVersion       = XMR_VERSION_3;
    DRM_CONST_STRING    dstrLID         = EMPTY_DRM_STRING;
    PRIVKEY_P256        privKey         = { 0 };
    DRM_CONST_STRING    dstrKID         = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrSeed        = EMPTY_DRM_STRING;
    DRM_CRYPTO_CONTEXT* pcryptoContext  = NULL;
    DRM_CHAR*           pstrRandomKeyID = NULL;
    DRM_CONST_STRING    dstrContentKey  = EMPTY_DRM_STRING;
    DRM_DWORD           cbBuffer        = 0;
    DRM_BYTE*           pbScratchKeySeed= NULL;

    ZEROMEM( &g_oXMRState, SIZEOF( g_oXMRState ) );

    ChkDR( Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&g_oXMRState.m_oDrmidLID, SIZEOF( DRM_GUID ) ) );

    ChkDR( DRM_XMR_StartLicense( g_rgbXmrBuilderStack,
                                 DRM_STACK_SIZE,
                                 &g_oXMRState.m_oDrmidLID,
                                 dwVersion,
                                 &g_oXmrBuilder ) );

    ChkMem(  pcryptoContext = ( DRM_CRYPTO_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pcryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SECURITY_LEVEL, &g_oXMRState.m_oXmrMinEnv ) );
    g_oXMRState.m_oXmrMinEnv.fValid = TRUE;
    g_oXMRState.m_oXmrMinEnv.wMinimumSecurityLevel = 150;
    ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_SECURITY_LEVEL, &g_oXMRState.m_oXmrMinEnv ) );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &g_oXMRState.m_oXmrPlay ) );
    g_oXMRState.m_oXmrPlay.fValid = TRUE;
    ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &g_oXMRState.m_oXmrPlay ) );

    /* Get shared random KID if it exists. */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RANDOM_KID_VAR, ( DRM_VOID ** )&pstrRandomKeyID, NULL ) ) )
    {
        pstrRandomKeyID = NULL;
    }

    ChkDR( MakeDRMString( &dstrSeed, "XVBovsmzhP9gRIZxWfFta3VVRPzVEWmJsazEJ46I" ) );

    /* Allocate memory for scratch buffer used in content key generation functions. */
#pragma prefast(push)
#pragma prefast(disable:22022) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( dstrSeed.cchString * SIZEOF( DRM_WCHAR ), dstrSeed.cchString );
    ChkMem( pbScratchKeySeed = ( DRM_BYTE * )Oem_MemAlloc( dstrSeed.cchString * SIZEOF( DRM_WCHAR ) ) );
    ZEROMEM( pbScratchKeySeed, dstrSeed.cchString * SIZEOF( DRM_WCHAR ) );
#pragma prefast(pop)

    /* Generate KID. */
    ChkMem( dstrKID.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( ( DRM_GUID_STRING_LEN + 1 ) * SIZEOF( DRM_WCHAR ) ) );
    dstrKID.cchString = DRM_GUID_STRING_LEN + 1;
    ChkDR( DRM_KG_GenerateKeyID( NULL, ( DRM_WCHAR * )dstrKID.pwszString, &dstrKID.cchString ) );
    ChkDR( DRM_UTL_DecodeKID( &dstrKID, &g_oXMRState.m_oDrmidKID ) );

    /* Generate content key. */
    dstrContentKey.cchString = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 );
    ChkMem( dstrContentKey.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( ( dstrContentKey.cchString + 1 ) * SIZEOF( DRM_WCHAR ) ) );
    ChkDR( DRM_KG_GenerateContentKeyB64( dstrSeed.pwszString,
                                         dstrSeed.cchString,
                                         dstrKID.pwszString,
                                         dstrKID.cchString,
                                         eDRM_AES_COUNTER_CIPHER,
                                         DRM_AES_KEYSIZE_128,
                                         ( DRM_WCHAR * )dstrContentKey.pwszString,
                                         &dstrContentKey.cchString,
                                         pbScratchKeySeed,
                                         dstrSeed.cchString * SIZEOF( DRM_WCHAR ) ) );

    cbBuffer = DRM_AES_KEYSIZE_128;
    ChkDR( DRM_B64_DecodeW( &dstrContentKey, &cbBuffer, g_oXMRState.m_rgbContentKey + DRM_AES_KEYSIZE_128, 0 ) );
    ChkBOOL( ( cbBuffer == DRM_AES_KEYSIZE_128 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Generate random integrity key. */
    ChkDR( OEM_ECC_GenerateHMACKey_P256( ( PLAINTEXT_P256 *)g_oXMRState.m_rgbContentKey, ( struct bigctx_t * )&pcryptoContext->rgbCryptoContext ) );

    /* Generate random key pair. */
    ChkDR( OEM_ECC_GenKeyPair_P256( &g_oXMRState.m_oPubKey, &privKey, ( struct bigctx_t * )&pcryptoContext->rgbCryptoContext ) );

    /* Encrypt content with asym public key. */
    ChkDR( OEM_ECC_Encrypt_P256( &g_oXMRState.m_oPubKey, ( PLAINTEXT_P256 * )g_oXMRState.m_rgbContentKey, &g_oXMRState.m_oEncryptedContentKey, ( struct bigctx_t * )&pcryptoContext->rgbCryptoContext ) );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, &g_oXMRState.m_oDeviceKey ) );
    g_oXMRState.m_oDeviceKey.fValid         = TRUE;
    g_oXMRState.m_oDeviceKey.wEccCurveType  = XMR_ECC_CURVE_TYPE_P256;
    g_oXMRState.m_oDeviceKey.pbKeyData      = g_oXMRState.m_oPubKey.m_rgbPubkey;
    g_oXMRState.m_oDeviceKey.cbKeyData      = SIZEOF( PUBKEY_P256 );
    g_oXMRState.m_oDeviceKey.iKeyData       = 0;
    ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, &g_oXMRState.m_oDeviceKey ) );

    /* Set up and add the encrypted content key to the license. */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &g_oXMRState.m_oContentKey ) );

    g_oXMRState.m_oContentKey.pbguidKeyID   = g_oXMRState.m_oDrmidKID.rgb;
    g_oXMRState.m_oContentKey.iguidKeyID    = 0;
    g_oXMRState.m_oContentKey.wSymmetricCipherType      = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
    g_oXMRState.m_oContentKey.wKeyEncryptionCipherType  = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256;
    g_oXMRState.m_oContentKey.pbEncryptedKeyBuffer      = g_oXMRState.m_oEncryptedContentKey.m_rgbCiphertext;
    g_oXMRState.m_oContentKey.cbEncryptedKey    = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;
    g_oXMRState.m_oContentKey.iEncryptedKey     = 0;

    ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &g_oXMRState.m_oContentKey ) );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &g_oXMRState.m_oXmrRIV ) );
    g_oXMRState.m_oXmrRIV.fValid    = TRUE;
    g_oXMRState.m_oXmrRIV.dwValue   = 0;
    ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &g_oXMRState.m_oXmrRIV ) );

ErrorExit:
    FREEDRMSTRING( dstrLID );
    FREEDRMSTRING( dstrKID );
    FREEDRMSTRING( dstrSeed );
    SAFE_OEM_FREE( pcryptoContext );
    SAFE_OEM_FREE( pbScratchKeySeed );
    SAFE_OEM_FREE( dstrContentKey.pwszString );
    return dr;
}

/*********************************************************************
**
** Function: TestFinishLicense
**
** Synopsis: Function that signs and completes the construction of a
**           XMR license.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestFinishLicense(
    __in long                   argc,
    __in_ecount( argc ) char ** argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_cbLicense = SIZEOF( g_rgbLicense );

    ChkDR( DRM_XMR_FinishLicense( &g_oXmrBuilder,
                                  g_oXMRState.m_rgbContentKey,
                                  _SignXmrLicense,
                                  g_rgbLicense,
                                  &g_cbLicense ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestUnpackLicense
**
** Synopsis: Function that unpacks a XMR license buffer.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestUnpackLicense(
    __in long                   argc,
    __in_ecount( argc )char **  argv )
{
    DRM_RESULT                  dr      = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT oStack  = { 0 };

    ZEROMEM( g_rgbXmrParserStack, DRM_MAX_LICENSESIZE );
    ZEROMEM( &g_oXmrLicense, SIZEOF( DRM_XMR_LICENSE ) );

    ChkDR( DRM_STK_Init( &oStack, g_rgbXmrParserStack, DRM_MAX_LICENSESIZE ) );
    ChkDR( DRM_XMR_UnpackLicense( g_rgbLicense, g_cbLicense, &oStack, &g_oXmrLicense) );

    g_oXmrLicense.pbXMRLic = g_rgbLicense;
    g_oXmrLicense.cbXMRLic = g_cbLicense;

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestVerifyLicense
**
** Synopsis: Function that verifies a XMR license with Aux Key or UplinkX
**           objects.
**
** Arguments:
**
** argv[0] ... argv[n]      -- Value of Locations that should exist in the
**                             Aux Key or UplinkX object.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestVerifyLicense(
    __in long                   argc,
    __in_ecount( argc ) char ** argv
    )
{
    DRM_RESULT      dr                      = DRM_SUCCESS;
    DRM_BOOL        fAuxKey                 = FALSE, fUplinkX = FALSE;
    DRM_XMR_AUX_KEY *poAuxKey               = NULL;
    DRM_XMR_UPLINKX *poUplinkX              = NULL;
    DRM_DWORD       dwLocation1, dwLocation2= 0;
    DRM_LONG        i                       = 0;

    ChkArg( argc >= 1 );

    if ( XMR_IS_AUX_KEY_VALID( &g_oXmrLicense ) )
    {
        fAuxKey = TRUE;
        poAuxKey = &g_oXmrLicense.containerOuter.containerKeys.AuxKey;

        ChkBOOL( poAuxKey->cEntries == argc, DRM_E_TEST_UNEXPECTED_OUTPUT );

        for ( i = 0; i < argc; i++ )
        {
            dwLocation1 = (DRM_DWORD)atol( argv[ i ] );
            NETWORKBYTES_TO_DWORD( dwLocation2, &poAuxKey->pEntries[ i ].dwLocation, 0 );
            ChkBOOL( dwLocation1 == dwLocation2, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

    if ( XMR_IS_UPLINKX_VALID( &g_oXmrLicense ) )
    {
        fUplinkX = TRUE;
        poUplinkX = &g_oXmrLicense.containerOuter.containerKeys.UplinkX;

        ChkBOOL( poUplinkX->cEntries == argc, DRM_E_TEST_UNEXPECTED_OUTPUT );

        for ( i = 0; i < argc; i++ )
        {
            dwLocation1 = (DRM_DWORD)atol( argv[ i ] );
            NETWORKBYTES_TO_DWORD( dwLocation2, &poUplinkX->pdwLocation[ i ], 0 );
            ChkBOOL( dwLocation1 == dwLocation2, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }

    ChkBOOL( ( fAuxKey ^ fUplinkX ) != 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestAddAuxKeyObject
**
** Synopsis: Function that constructs an Aux Key object and adds it to
**           the XMR license being built.
**
** Arguments:
**
** argv[0] ... argv[n]      -- Value of Locations to be included in the
**                             Aux Key object.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/

DRM_RESULT DRM_CALL TestAddAuxKeyObject(
    __in long                   argc,
    __in_ecount( argc ) char ** argv
    )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_AES_KEY oAesKey = { 0 };
    DRM_LONG    i       = 0;

    ChkArg( argc >= 1 );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_AUX_KEY_OBJECT, &g_oAuxKey ) );

    ChkDR( Oem_Aes_SetKey( g_oXMRState.m_rgbContentKey + DRM_AES_KEYSIZE_128, &oAesKey ) );

    for ( i = 0; i < argc; i++ )
    {
        g_rgoAuxKeyEntries[ i ].dwLocation = (DRM_DWORD)atol( argv[ i ] );
        ChkDR( _DeriveKey( g_rgoAuxKeyEntries[ i ].dwLocation, g_rgoAuxKeyEntries[ i ].rgbKey ) );
        ChkDR( Oem_Aes_EcbDecryptData( &oAesKey, g_rgoAuxKeyEntries[ i ].rgbKey, DRM_AES_KEYSIZE_128 ) );
    }

    ChkDR( DRM_LongToWord( argc, &g_oAuxKey.cEntries ) );
    g_oAuxKey.pEntries = g_rgoAuxKeyEntries;

    ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_AUX_KEY_OBJECT, &g_oAuxKey ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: TestAddUplinkXObject
**
** Synopsis: Function that constructs an UplinkX object and optionally
**           adds it to the XMR license being built.
**
** Arguments:
**
** argv[0]                  -- Flag indicating whether UplinkX should be
**                             add to the XMR license being built.
** argv[1] ... argv[n]      -- Value of Locations to be included in the
**                             UplinkX object.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL TestAddUplinkXObject(
    __in long                   argc,
    __in_ecount( argc ) char ** argv
    )
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_LONG    i               = 0;
    DRM_BOOL    fAddToLicense   = TRUE;

    ChkArg( argc >= 2 );

    fAddToLicense = atol( argv[ 0 ] );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_UPLINKX_OBJECT, &g_oUplinkX ) );

    for ( i = 1; i < argc; i++ )
    {
        g_rgdwLocations[ i - 1 ] = (DRM_DWORD)atol( argv[ i ] );
    }

    /* UplinkKID is never set, so we will set it here so that UpLinkX is valid */
    MEMCPY( g_oXMRState.m_oUplinkKID.rgb, g_rgbSampleDRMID.rgb, DRM_ID_SIZE );
    g_oUplinkX.pbguidUplinkKID   = g_oXMRState.m_oUplinkKID.rgb;
    g_oUplinkX.iguidUplinkKID    = 0;

    ChkDR( _CreateChecksum( &g_oXMRState.m_oDrmidKID,
                            g_oXMRState.m_rgbContentKey,
                            DRM_AES_KEYSIZE_128,
                            g_oXMRState.m_rgbChecksum ) );

    g_oUplinkX.pbCheckSumBuffer     = g_oXMRState.m_rgbChecksum;
    g_oUplinkX.cbCheckSum           = SIZEOF( g_oXMRState.m_rgbChecksum );
    g_oUplinkX.iCheckSum            = 0;
    ChkDR( DRM_LongToWord( argc - 1, &g_oUplinkX.cEntries ) );
    g_oUplinkX.pdwLocation          = g_rgdwLocations;

    if ( fAddToLicense )
    {
        ChkDR( DRM_XMR_AddObject( &g_oXmrBuilder, XMR_OBJECT_TYPE_UPLINKX_OBJECT, &g_oUplinkX ) );
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

