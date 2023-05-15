/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMKEYFILE_C
#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <oemeccp256.h>
#include <oemkeyfile.h>
#include <drmactivation.h>
#include <drmdeviceassets.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_KEYFILE_VERSION == 2
    static const DRM_WORD g_KF_PLATFORM_VERSION = KEYFILE_PLATFORM_WINDOWSMOBILE7;
#else
    static const DRM_WORD g_KF_PLATFORM_VERSION = KEYFILE_PLATFORM_PK;
#endif


/*******************************************************************************
** Function:    DRM_KF_Initialize
**
** Synopsis:    Initializes the keyfile context
**
**              [f_pOEMContext]  OEM context
**              [f_pKeyFile]     Keyfile context to initialize
**
** Returns:
**              DRM_SUCCESS       The function completed successfully.
**              DRM_E_INVALIDARG  One or more arguments were invalid
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_Initialize(
    __in_opt       DRM_VOID                    *f_pOEMContext,
    __out          DRM_KEYFILE_CONTEXT         *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pKeyFile != NULL );

    ZEROMEM( f_pKeyFile, SIZEOF( DRM_KEYFILE_CONTEXT ) );

    f_pKeyFile->pOEMContext = f_pOEMContext;
    f_pKeyFile->hKeyfile = OEM_INVALID_HANDLE_VALUE;
    f_pKeyFile->fInited = TRUE;

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_Uninitialize
**
** Synopsis:    Uninitializes the keyfile context
**
**              [f_pKeyFile]     Keyfile context to initialize
**
** Returns:
**              DRM_SUCCESS                    The function completed successfully.
**              DRM_E_INVALIDARG               One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED       The KeyFile context was not initialized prior to use.
**              DRM_E_KEYFILE_FILE_NOT_CLOSED  DRM_KF_Close was not called before calling this.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_Uninitialize(
    __inout   DRM_KEYFILE_CONTEXT           *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    if( f_pKeyFile != NULL )
    {
        ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );

        ChkBOOL( f_pKeyFile->hKeyfile == OEM_INVALID_HANDLE_VALUE, DRM_E_KEYFILE_FILE_NOT_CLOSED );

        ZEROMEM( f_pKeyFile, SIZEOF( DRM_KEYFILE_CONTEXT ) );

        f_pKeyFile->fInited = FALSE;
    }

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_Open
**
** Synopsis:    Opens the keyfile on disk
**
**              [f_pKeyFile]     Keyfile context
**              [f_dwAccessMode] The access mode to use when opening the file
**                               eg, OEM_GENERIC_READ, OEM_GENERIC_READ | OEM_GENERIC_WRITE
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**              DRM_E_FILE_READ_ERROR      An Error occured reading the file.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_Open(
    __inout  DRM_KEYFILE_CONTEXT *f_pKeyFile,
    __in     DRM_DWORD            f_fCreateIfNotExists,
    __in     DRM_DWORD            f_dwAccessMode )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkBOOL( f_pKeyFile->hKeyfile == OEM_INVALID_HANDLE_VALUE, DRM_E_FILE_READ_ERROR );

    f_pKeyFile->hKeyfile = Oem_Keyfile_Open( f_pKeyFile->pOEMContext,
        f_dwAccessMode,
        OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
        f_fCreateIfNotExists ? (DRM_DWORD)OEM_OPEN_ALWAYS : (DRM_DWORD)OEM_OPEN_EXISTING,
        OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    /*
    ** Now lock the whole file.  If the file is being opened for write
    ** then lock it for exclusive access.  This will force us to
    ** wait for all other threads that may have opened the file
    ** for read to close the file.
    **
    ** If the file is not being opened for write then lock it for
    ** shared access to allow multiple readers.
    */
    if ( ! Oem_File_Lock( f_pKeyFile->hKeyfile,
        f_dwAccessMode & OEM_GENERIC_WRITE ? TRUE : FALSE,
        0,
        MAX_SIGNED_TYPE(DRM_LONG),
        TRUE ) )
    {
        /*
        ** Could not lock the file, so close it and return an error.
        */
        Oem_Keyfile_Close( f_pKeyFile->hKeyfile );
        ChkDR( DRM_E_FILEOPEN );
    }

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_Close
**
** Synopsis:    Closes the keyfile on disk
**
**              [f_pKeyFile]     Keyfile context
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid.
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_Close(
    __in   DRM_KEYFILE_CONTEXT   *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

    Oem_File_Unlock(f_pKeyFile->hKeyfile, 0, MAX_SIGNED_TYPE(DRM_LONG));

    Oem_Keyfile_Close( f_pKeyFile->hKeyfile );
    f_pKeyFile->hKeyfile = OEM_INVALID_HANDLE_VALUE;

ErrorExit:
    return dr;
}


/*
** Note: When this function is successfully executed, the key file buffer is in the machine byte order.
*/
static DRM_RESULT DRMFORCEINLINE DRM_CALL _LoadKeyfile(
    __inout DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in    DRM_BOOL               f_fCheckSig )
{
    return DRM_KF_Parse(
        f_pKeyFile->pOEMContext,
        f_pKeyFile->rgbParserStack,
        SIZEOF( f_pKeyFile->rgbParserStack ),
        f_pKeyFile->pbKeyfileBuffer,
        f_pKeyFile->cbKeyfileBuffer,
        f_fCheckSig,
        &f_pKeyFile->keyfile );
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_LoadFromMemory(
    __in_ecount(f_cbKeyFile)   const DRM_BYTE            *f_pbKeyFile,
    __in                             DRM_DWORD            f_cbKeyFile,
    __inout                          DRM_KEYFILE_CONTEXT *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pKeyFile  != NULL );
    ChkArg( f_pbKeyFile != NULL );
    ChkArg( f_cbKeyFile  > 0    );

    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkBOOL( f_cbKeyFile <= SIZEOF( f_pKeyFile->rgbKeyfileBuffer ), DRM_E_KEYFILE_TOO_LARGE );

    f_pKeyFile->cbKeyfileBuffer = f_cbKeyFile;

    f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
    ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
    f_pKeyFile->pbKeyfileBuffer -= ( f_cbKeyFile + DRM_AES_BLOCKLEN );
    ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );

    MEMCPY( f_pKeyFile->pbKeyfileBuffer, f_pbKeyFile, f_cbKeyFile );
    ChkDR( _LoadKeyfile( f_pKeyFile, TRUE ) );
    f_pKeyFile->fLoaded = TRUE;

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_LoadFromFile
**
** Synopsis:    Loads, parses, and verifies the signature on the keyfile data from disk
**
**              [f_pKeyFile]     Keyfile context
**              [f_fValidateSignature] If TRUE, the keyfile signature will be validated
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_TOO_LARGE  The keyfile is too large for the buffer in the context
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_LoadFromFile(
    __inout   DRM_KEYFILE_CONTEXT        *f_pKeyFile,
    __in      DRM_BOOL                    f_fValidateSignature )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbKeyfileLen = 0;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( Oem_File_GetSize( f_pKeyFile->hKeyfile, &cbKeyfileLen ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( cbKeyfileLen <= SIZEOF( f_pKeyFile->rgbKeyfileBuffer ), DRM_E_KEYFILE_TOO_LARGE );

    f_pKeyFile->cbKeyfileBuffer = cbKeyfileLen;
    if( cbKeyfileLen == 0 )
    {
        /* We just created a new keyfile on disk, need to set the fixed values in the context for when it's
        ** stored back
        */

        ZEROMEM( &f_pKeyFile->keyfile, SIZEOF( f_pKeyFile->keyfile ) );

        f_pKeyFile->keyfile.Platform.fValid = TRUE;
        f_pKeyFile->keyfile.Platform.wPlatform = g_KF_PLATFORM_VERSION;
        f_pKeyFile->pbKeyfileBuffer = f_pKeyFile->rgbKeyfileBuffer;
    }
    else
    {
        /* We need to load the keyfile data at the end of the buffer such that next time we update
        ** keyfile data we won't be stomping on the previous keyfile data while constructing the new keyfile data.
        ** Leave some space (DRM_AES_BLOCKLEN (16 bytes)) in the end to allow padding of keyfile on some platforms.
        */
        f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
        f_pKeyFile->pbKeyfileBuffer -= ( cbKeyfileLen + DRM_AES_BLOCKLEN );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
        /* Since DRM_E_KEYFILE_TOO_LARGE check would have failed if cbKeyfileLen was larger than the keyfile buffer, an
        ** integer underflow should not be possible in the preceding line
        */

        ChkDR( Oem_Keyfile_LoadFromFile( f_pKeyFile ) );

        ChkDR( _LoadKeyfile( f_pKeyFile, TRUE ) );
    }

    f_pKeyFile->fLoaded = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_KEYFILE_CERT_TYPE DRM_CALL MapApiCertTypeToFormatCertType( __in DRM_KF_CERT_TYPE f_eCertType )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_KEYFILE_CERT_TYPE eFormatCertTypeToFind = KF_CERT_TYPE_INVALID;
    switch( f_eCertType )
    {
    case eKF_CERT_TYPE_NDT:       eFormatCertTypeToFind = KF_CERT_TYPE_NDT;                             break;
    case eKF_CERT_TYPE_WMDRM:     eFormatCertTypeToFind = KF_CERT_TYPE_WMDRM;                           break;
    case eKF_CERT_TYPE_PLAYREADY: eFormatCertTypeToFind = KF_CERT_TYPE_PLAYREADY;                       break;
    default:                      /* Do nothing - leave eFormatCertTypeToFind = KF_CERT_TYPE_INVALID */ break;
    }
    return eFormatCertTypeToFind;
}

/*******************************************************************************
** Function:    DRM_KF_GetCertificate
**
** Synopsis:    Retreives the certificate of the specified type from the keyfile
**
**              [f_pKeyFile]     Keyfile context
**              [f_eCertType]    Type of certificate to
**                               load
**              [f_ppbCertData]  Pointer to a buffer to receive a pointer to
**                               the certificate
**              [f_pcbCertData]  Pointer to a DWORD to receive the size in bytes
**                               of the certificate
**
** Returns:
**              DRM_SUCCESS                                The function complete
**                                                         d successfully.
**              DRM_E_INVALIDARG                           One or more arguments
**                                                         were invalid
**              DRM_E_KEYFILE_NOT_INITED                   The KeyFile context was
**                                                         not initialized prior to use.
**              DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND  The certificate of
**                                                         the specified type
**                                                         was not found
**
********************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_KEYFILE_CONTEXT* should not be const.")
DRM_API DRM_RESULT DRM_CALL DRM_KF_GetCertificate(
    __in                            DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                            DRM_KF_CERT_TYPE       f_eCertType,
    __out_ecount(*f_pcbCertData)    DRM_BYTE             **f_ppbCertData,
    __out_ecount(1)                 DRM_DWORD             *f_pcbCertData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;
    KF_CERTIFICATE_CHAIN_OBJECT_V2 *pCertChain = NULL;
    DRM_KEYFILE_CERT_TYPE eFormatCertTypeToFind = KF_CERT_TYPE_INVALID;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pKeyFile    != NULL );
    ChkArg( f_ppbCertData != NULL );
    ChkArg( f_pcbCertData != 0 );

    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    /* Perform an explicit mapping.  This ensures that the API can change without regaurd to the
    ** core format defines
    */
    eFormatCertTypeToFind = MapApiCertTypeToFormatCertType( f_eCertType );
    ChkArg( eFormatCertTypeToFind != KF_CERT_TYPE_INVALID );

    pCertChain = f_pKeyFile->keyfile.pCertChainV2;
    while( pCertChain != NULL && pCertChain->fValid )
    {
        if( (DRM_KEYFILE_CERT_TYPE)pCertChain->dwCertificateType == eFormatCertTypeToFind )
        {
            /* Found the cert */
            *f_ppbCertData = pCertChain->xbbaCertificateChain.pbDataBuffer + pCertChain->xbbaCertificateChain.iData;
            *f_pcbCertData = pCertChain->xbbaCertificateChain.cbData;

            goto ErrorExit;
        }
        pCertChain = pCertChain->pNext;
    }

    ChkDR( DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _FindAsymmetricKeyPair(
    __in                      const DRM_KEYFILE_CONTEXT             *f_pKeyFile,
    __in                            DRM_KEYFILE_ASYMMETRIC_KEY_TYPE  f_eAsymmKeyType,
    __in_bcount(f_cbPubKey)   const DRM_BYTE                        *f_pbPubKey,
    __in                            DRM_DWORD                        f_cbPubKey,
    __out                           DRM_ENCRYPTED_KEY               *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;


static DRM_NO_INLINE DRM_RESULT _FindAsymmetricKeyPair(
    __in                      const DRM_KEYFILE_CONTEXT             *f_pKeyFile,
    __in                            DRM_KEYFILE_ASYMMETRIC_KEY_TYPE  f_eAsymmKeyType,
    __in_bcount(f_cbPubKey)   const DRM_BYTE                        *f_pbPubKey,
    __in                            DRM_DWORD                        f_cbPubKey,
    __out                           DRM_ENCRYPTED_KEY               *f_pKey )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    KF_KEYPAIR_V2_OBJECT *pKeyPairV2 = NULL;
    KF_KEYPAIR_V3_OBJECT *pKeyPairV3 = NULL;

    ChkArg( f_pbPubKey != NULL );

    pKeyPairV3 = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv3;
    while( pKeyPairV3 != NULL && pKeyPairV3->fValid )
    {
        if( f_cbPubKey == pKeyPairV3->xbbaPublicKeyData.cbData
           && pKeyPairV3->wCipherType == f_eAsymmKeyType
           && OEM_SECURE_ARE_EQUAL(pKeyPairV3->xbbaPublicKeyData.pbDataBuffer + pKeyPairV3->xbbaPublicKeyData.iData, f_pbPubKey, f_cbPubKey ) )
        {
            /* Found the key */
            f_pKey->guidKeypairDecryptionKeyID = pKeyPairV3->guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = pKeyPairV3->wKeypairDecryptionMethod;
            f_pKey->wKeySize = pKeyPairV3->wKeySize;
            f_pKey->wReserved = pKeyPairV3->wReserved;

            f_pKey->cbEncryptedKey = pKeyPairV3->xbbaEncryptedPrivateKey.cbData;
            f_pKey->pbEncryptedKey = pKeyPairV3->xbbaEncryptedPrivateKey.pbDataBuffer + pKeyPairV3->xbbaEncryptedPrivateKey.iData;

            /* Initialization vector size is constant */
            ChkFAIL( SIZEOF( pKeyPairV3->qwInitializationVector ) <= SIZEOF( f_pKey->rgbInitializationVector ) );
            DRM_BYT_CopyBytes( f_pKey->rgbInitializationVector, 0, &pKeyPairV3->qwInitializationVector, 0, SIZEOF( pKeyPairV3->qwInitializationVector ) );

            goto ErrorExit;
        }
        pKeyPairV3 = pKeyPairV3->pNext;
    }

    pKeyPairV2 = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv2;

    while( pKeyPairV2 != NULL && pKeyPairV2->fValid )
    {
        if( f_cbPubKey == pKeyPairV2->xbbaPublicKeyData.cbData
            && pKeyPairV2->wCipherType == f_eAsymmKeyType
            && OEM_SECURE_ARE_EQUAL( pKeyPairV2->xbbaPublicKeyData.pbDataBuffer + pKeyPairV2->xbbaPublicKeyData.iData, f_pbPubKey, f_cbPubKey ) )
        {
            /* Found the key */
            f_pKey->guidKeypairDecryptionKeyID = pKeyPairV2->guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = pKeyPairV2->wKeypairDecryptionMethod;
            f_pKey->wKeySize = pKeyPairV2->wKeySize;
            f_pKey->wReserved = pKeyPairV2->wReserved;

            f_pKey->cbEncryptedKey = pKeyPairV2->xbbaEncryptedPrivateKey.cbData;
            f_pKey->pbEncryptedKey = pKeyPairV2->xbbaEncryptedPrivateKey.pbDataBuffer + pKeyPairV2->xbbaEncryptedPrivateKey.iData;

            /* Initialization vector size is constant */
            ChkFAIL( pKeyPairV2->xbbaInitializationVector.cbData == SIZEOF( f_pKey->rgbInitializationVector ) );
            DRM_BYT_CopyBytes( f_pKey->rgbInitializationVector, 0, pKeyPairV2->xbbaInitializationVector.pbDataBuffer, pKeyPairV2->xbbaInitializationVector.iData, SIZEOF( f_pKey->rgbInitializationVector ) );

            goto ErrorExit;
        }
        pKeyPairV2 = pKeyPairV2->pNext;
    }

    ChkDR( DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );
ErrorExit:
    return dr;
}

DRM_API DRM_KEYFILE_ASYMMETRIC_KEY_TYPE DRM_CALL MapApiKeyTypeToFormatKeyType( __in DRM_KF_KEY_TYPE f_eKeyType )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_KEYFILE_ASYMMETRIC_KEY_TYPE eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID;

    switch( f_eKeyType )
    {
    case eKF_KEY_TYPE_RSA:     eAsymmKeyType = KF_ASYMM_KEY_TYPE_RSA;                             break;
    case eKF_KEY_TYPE_ECC_160: eAsymmKeyType = KF_ASYMM_KEY_TYPE_ECC_160;                         break;
    case eKF_KEY_TYPE_ECC_256: eAsymmKeyType = KF_ASYMM_KEY_TYPE_ECC_256;                         break;
    case eKF_KEY_SECURE_STORE: /* Do nothing - leave eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID */ break;
    default:                   /* Do nothing - leave eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID */ break;
    }
    return eAsymmKeyType;
}

/*******************************************************************************
** Function:    DRM_KF_GetPrivateKey
**
** Synopsis:    Retreives an encrypted private key from the keyfile given the
**              corresponding public
**              key
**
**              [f_pKeyFile]     Keyfile context
**              [f_eKeyType]  Cipher type of the private key to retreive
**              [f_pbPubKey]     Public key corresponding to the private key
**                               desired
**              [f_cbPubKey]     Size in bytes of the public
**                               key
**              [f_pKey]         Pointer to a struct to receive the encrypted
**                               private key
**                               data
**
** Returns:
**              DRM_SUCCESS                          The function completed
**                                                   successfully.
**              DRM_E_INVALIDARG                     One or more arguments were
**                                                   invalid
**              DRM_E_KEYFILE_NOT_INITED             The KeyFile context was
**                                                   not initialized prior to use.
**              DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND  A private key with the
**                                                   specified constraints
**                                                   could not be found
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_NO_INLINE DRM_CALL DRM_KF_GetPrivateKey(
    __in                        const DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                              DRM_KF_KEY_TYPE        f_eKeyType,
    __in_bcount_opt(f_cbPubKey) const DRM_BYTE              *f_pbPubKey,
    __in                              DRM_DWORD              f_cbPubKey,
    __out_ecount(1)                   DRM_ENCRYPTED_KEY     *f_pKey )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pKeyFile != NULL );
    ChkArg( f_pKey != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    if( eKF_KEY_SECURE_STORE == f_eKeyType )
    {
        if( f_pKeyFile->keyfile.SecureStoreKeyV3.fValid )
        {
            f_pKey->guidKeypairDecryptionKeyID = f_pKeyFile->keyfile.SecureStoreKeyV3.guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = f_pKeyFile->keyfile.SecureStoreKeyV3.wKeypairDecryptionMethod;
            f_pKey->wKeySize = f_pKeyFile->keyfile.SecureStoreKeyV3.wKeySize;
            f_pKey->wReserved = f_pKeyFile->keyfile.SecureStoreKeyV3.wReserved;

            f_pKey->cbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV3.xbbaEncryptedSecureStoreKey.cbData;
            f_pKey->pbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV3.xbbaEncryptedSecureStoreKey.pbDataBuffer + f_pKeyFile->keyfile.SecureStoreKeyV3.xbbaEncryptedSecureStoreKey.iData;

            /* Initialization vector size is constant */
            ChkBOOL( SIZEOF( f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector ) <= SIZEOF( f_pKey->rgbInitializationVector ), DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );
            OEM_SECURE_MEMCPY( f_pKey->rgbInitializationVector, &f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector, SIZEOF( f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector ) );
        }
        else
        {
            ChkBOOL( f_pKeyFile->keyfile.SecureStoreKeyV2.fValid, DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );

            f_pKey->guidKeypairDecryptionKeyID = f_pKeyFile->keyfile.SecureStoreKeyV2.guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = f_pKeyFile->keyfile.SecureStoreKeyV2.wKeypairDecryptionMethod;
            f_pKey->wKeySize = f_pKeyFile->keyfile.SecureStoreKeyV2.wKeySize;
            f_pKey->wReserved = f_pKeyFile->keyfile.SecureStoreKeyV2.wReserved;

            f_pKey->cbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaEncryptedSecureStoreKey.cbData;
            f_pKey->pbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaEncryptedSecureStoreKey.pbDataBuffer + f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaEncryptedSecureStoreKey.iData;

            /* Initialization vector size is constant */
            ChkBOOL( f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.cbData == SIZEOF( f_pKey->rgbInitializationVector ), DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );
            OEM_SECURE_MEMCPY( f_pKey->rgbInitializationVector, f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.pbDataBuffer + f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.iData, SIZEOF( f_pKey->rgbInitializationVector ) );
        }
    }
    else
    {
        DRM_KEYFILE_ASYMMETRIC_KEY_TYPE eAsymmKeyType = MapApiKeyTypeToFormatKeyType( f_eKeyType );

        ChkArg( eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID );
        ChkArg( f_pbPubKey != NULL );

        ChkDR( _FindAsymmetricKeyPair( f_pKeyFile, eAsymmKeyType, f_pbPubKey, f_cbPubKey, f_pKey ) );
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

