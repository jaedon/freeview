/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <oemeccp256.h>
#include <oemkeyfile.h>
#include <drmactivation.h>

ENTER_PK_NAMESPACE_CODE;


#if DRM_KEYFILE_VERSION == 2
    static const DRM_DWORD g_KF_VERSION = KEYFILE_VERSION_2;
    static const DRM_DWORD g_KF_PLATFORM_VERSION = KEYFILE_PLATFORM_WINDOWSMOBILE7;
#else
    static const DRM_DWORD g_KF_VERSION = KEYFILE_VERSION_3;
    static const DRM_DWORD g_KF_PLATFORM_VERSION = KEYFILE_PLATFORM_PK;
#endif

/*
** Returns an error specific to keyfile are ainstead of a generic out-of-memory error
** thrown by a stack allocator.
*/
#define TranslateKeyfileOOMError(_dr) \
    ( ( _dr == DRM_E_OUTOFMEMORY ) ? DRM_E_KEYFILE_TOO_LARGE : _dr )


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
DRM_API DRM_RESULT DRM_CALL DRM_KF_Initialize (
    IN       DRM_VOID                    *f_pOEMContext,
    OUT   DRM_KEYFILE_CONTEXT         *f_pKeyFile)
{
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
DRM_API DRM_RESULT DRM_CALL DRM_KF_Uninitialize (
    IN   DRM_KEYFILE_CONTEXT           *f_pKeyFile)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );

    ChkBOOL( f_pKeyFile->hKeyfile == OEM_INVALID_HANDLE_VALUE, DRM_E_KEYFILE_FILE_NOT_CLOSED );

    ZEROMEM( f_pKeyFile, SIZEOF( DRM_KEYFILE_CONTEXT ) );

    f_pKeyFile->fInited = FALSE;

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
**              DRM_E_FILEREADERROR      An Error occured reading the file.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_Open (
    __inout  DRM_KEYFILE_CONTEXT *f_pKeyFile,
    __in     DRM_DWORD            f_fCreateIfNotExists,
    __in     DRM_DWORD            f_dwAccessMode )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkBOOL( f_pKeyFile->hKeyfile == OEM_INVALID_HANDLE_VALUE, DRM_E_FILEREADERROR );

    f_pKeyFile->hKeyfile = Oem_Keyfile_Open( f_pKeyFile->pOEMContext,
        f_dwAccessMode,
        OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
        f_fCreateIfNotExists ? OEM_OPEN_ALWAYS : OEM_OPEN_EXISTING,
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
        0xffffffff,
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
DRM_API DRM_RESULT DRM_CALL DRM_KF_Close(
    IN   DRM_KEYFILE_CONTEXT   *f_pKeyFile)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

    Oem_File_Unlock(f_pKeyFile->hKeyfile, 0, 0xffffffff);

    Oem_Keyfile_Close( f_pKeyFile->hKeyfile );
    f_pKeyFile->hKeyfile = OEM_INVALID_HANDLE_VALUE;

ErrorExit:
    return dr;
}


/*
** Note: When this function is successfully executed, the key file buffer is in the machine byte order.
*/
static DRM_RESULT DRM_CALL _LoadKeyfile (
    IN OUT DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN     DRM_BOOL               f_fCheckSig )
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

/* This function re-builds the keyfile data and optionally adds or updates the supplied cert or key into the
** format during building. If the cert type and/or key type is specified as INVALID, then the parameter is ignored.
** Note: When this function is successfully executed, the key file buffer is in the network byte order.
*/
static DRM_RESULT DRM_CALL _UpdateKeyfile (
    IN OUT                          DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN                              DRM_KEYFILE_CERT_TYPE  f_eCertType,
    __in_bcount_opt(f_cbCertData)   const DRM_BYTE        *f_pbCertData,
    IN                              DRM_DWORD              f_cbCertData,
    IN                              DRM_BOOL               f_fUpdateSecureStoreKey,
    IN                              DRM_KEYFILE_ASYMMETRIC_KEY_TYPE   f_eAsymmKeyType,
    __in_bcount_opt(f_cbPubKey)     const DRM_BYTE        *f_pbPubKey,
    IN                              DRM_DWORD              f_cbPubKey,
    IN                              DRM_ENCRYPTED_KEY     *f_pKey,
    __in_opt                        DRM_DEVICE_CERT_PARSER_CACHE *f_pCertParserCache )
{
    DRM_RESULT dr = DRM_SUCCESS;
    KF_KEYPAIR_V2_OBJECT *pKeyPair = NULL;
    KF_CERTIFICATE_CHAIN_OBJECT_V2 *pCertChain = NULL;
    DRM_DWORD  cbKeyfile = SIZEOF( f_pKeyFile->rgbKeyfileBuffer );
    KF_KEYPAIR_V2_OBJECT keyPair = {0};
    KF_CERTIFICATE_CHAIN_OBJECT_V2 certChain = {0};
    KF_SECURE_STORE_KEY_V2_OBJECT secureStoreKey = {0};
    KF_CERT_PARSER_CACHE_OBJECT certParserCache = {0};

    ChkDR( DRM_KF_Start( f_pKeyFile->rgbBuilderStack, SIZEOF( f_pKeyFile->rgbBuilderStack ), g_KF_VERSION, NULL, &f_pKeyFile->builderContext ) );
    
    /* Add platform object */
    ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_PLATFORM_OBJECT, &f_pKeyFile->keyfile.Platform ) );

    /* Add all the keypairs in the keyhistory */
    pKeyPair = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv2;
    while( pKeyPair != NULL && pKeyPair->fValid )
    {
        if( f_eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID
         && f_pbPubKey != NULL
         && f_cbPubKey == pKeyPair->xbbaPublicKeyData.cbData
         && pKeyPair->wCipherType == f_eAsymmKeyType
         && DRM_BYT_CompareBytes( pKeyPair->xbbaPublicKeyData.pbDataBuffer, pKeyPair->xbbaPublicKeyData.iData, f_pbPubKey, 0, f_cbPubKey ) == 0 )
        {
            /* The key to be added already exists, we just need to update it */

            ChkArg( f_pKey != NULL );

            pKeyPair->wKeySize = f_pKey->wKeySize;
            pKeyPair->wKeypairDecryptionMethod = f_pKey->wKeypairDecryptionMethod;
            pKeyPair->wReserved = f_pKey->wReserved;
            pKeyPair->guidKeypairDecryptionKeyID = f_pKey->guidKeypairDecryptionKeyID;

            /* Currently no support for expanding the size of keys */
            ChkFAIL( f_pKey->cbEncryptedKey == pKeyPair->xbbaEncryptedPrivateKey.cbData );
            DRM_BYT_CopyBytes( pKeyPair->xbbaEncryptedPrivateKey.pbDataBuffer, pKeyPair->xbbaEncryptedPrivateKey.iData, f_pKey->pbEncryptedKey, 0, f_pKey->cbEncryptedKey );

            /* Initialization vector size is constant */
            ChkFAIL( pKeyPair->xbbaInitializationVector.cbData == SIZEOF( f_pKey->rgbInitializationVector ) );
            DRM_BYT_CopyBytes( pKeyPair->xbbaInitializationVector.pbDataBuffer, pKeyPair->xbbaInitializationVector.iData, f_pKey->rgbInitializationVector, 0, SIZEOF( f_pKey->rgbInitializationVector ) );


            /* Set key to be added to 0 so we won't add it again later */
            f_eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID;
        }

        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_KEYPAIR_OBJECT_2, pKeyPair ) );

        pKeyPair = pKeyPair->pNext;
    }


    if( f_eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID )
    {
        /* New keypair is to be added to the key history */

        ChkArg( f_pbPubKey != NULL );
        ChkArg( f_pKey != NULL );

        keyPair.fValid = TRUE;
        keyPair.wCipherType = f_eAsymmKeyType;
        keyPair.wKeySize = f_pKey->wKeySize;
        keyPair.wKeypairDecryptionMethod = f_pKey->wKeypairDecryptionMethod;
        keyPair.wReserved = f_pKey->wReserved;
        keyPair.guidKeypairDecryptionKeyID = f_pKey->guidKeypairDecryptionKeyID;

        keyPair.xbbaPublicKeyData.fValid = TRUE;
        keyPair.xbbaPublicKeyData.cbData = f_cbPubKey;
        keyPair.xbbaPublicKeyData.iData = 0;
        keyPair.xbbaPublicKeyData.pbDataBuffer = (DRM_BYTE*)f_pbPubKey;

        keyPair.xbbaEncryptedPrivateKey.fValid = TRUE;
        keyPair.xbbaEncryptedPrivateKey.cbData = f_pKey->cbEncryptedKey;
        keyPair.xbbaEncryptedPrivateKey.iData = 0;
        keyPair.xbbaEncryptedPrivateKey.pbDataBuffer = f_pKey->pbEncryptedKey;

        keyPair.xbbaInitializationVector.fValid = TRUE;
        keyPair.xbbaInitializationVector.cbData = SIZEOF( f_pKey->rgbInitializationVector );
        keyPair.xbbaInitializationVector.iData = 0;
        keyPair.xbbaInitializationVector.pbDataBuffer = (DRM_BYTE*)f_pKey->rgbInitializationVector;


        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_KEYPAIR_OBJECT_2, &keyPair ) );
    }

    pCertChain = f_pKeyFile->keyfile.pCertChainV2;
    while( pCertChain != NULL && pCertChain->fValid )
    {
        if( f_eCertType != KF_CERT_TYPE_INVALID
            && (DRM_KEYFILE_CERT_TYPE)pCertChain->dwCertificateType == f_eCertType )
        {
            /* Update the cert */

            ChkArg( f_pbCertData != NULL );
            ChkArg( f_cbCertData > 0 );

            pCertChain->xbbaCertificateChain.pbDataBuffer = (DRM_BYTE*)f_pbCertData;
            pCertChain->xbbaCertificateChain.iData = 0;
            pCertChain->xbbaCertificateChain.cbData = f_cbCertData;

            /* Clear out cert type so we won't add it again */
            f_eCertType = KF_CERT_TYPE_INVALID;
        }

        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2, pCertChain ) );
        pCertChain = pCertChain->pNext;
    }

    if( f_eCertType != KF_CERT_TYPE_INVALID )
    {
        ChkArg( f_pbCertData != NULL );
        ChkArg( f_cbCertData > 0 );

        certChain.fValid = TRUE;
        certChain.dwCertificateType = f_eCertType;
        certChain.xbbaCertificateChain.pbDataBuffer = (DRM_BYTE*)f_pbCertData;
        certChain.xbbaCertificateChain.iData = 0;
        certChain.xbbaCertificateChain.cbData = f_cbCertData;
        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2, &certChain ) );
    }

    if( f_fUpdateSecureStoreKey )
    {
        ChkArg( f_pKey != NULL );
        ChkArg( f_pbPubKey == NULL );
        ChkArg( f_cbPubKey == 0 );

        secureStoreKey.fValid = TRUE;
        secureStoreKey.wCipherType = KF_SYMM_KEY_TYPE_AES;
        secureStoreKey.wKeySize = f_pKey->wKeySize;
        secureStoreKey.wKeypairDecryptionMethod = f_pKey->wKeypairDecryptionMethod;
        secureStoreKey.wReserved = f_pKey->wReserved;
        secureStoreKey.guidKeypairDecryptionKeyID = f_pKey->guidKeypairDecryptionKeyID;

        secureStoreKey.xbbaEncryptedSecureStoreKey.fValid = TRUE;
        secureStoreKey.xbbaEncryptedSecureStoreKey.cbData = f_pKey->cbEncryptedKey;
        secureStoreKey.xbbaEncryptedSecureStoreKey.iData = 0;
        secureStoreKey.xbbaEncryptedSecureStoreKey.pbDataBuffer = f_pKey->pbEncryptedKey;

        secureStoreKey.xbbaInitializationVector.fValid = TRUE;
        secureStoreKey.xbbaInitializationVector.cbData = SIZEOF( f_pKey->rgbInitializationVector );
        secureStoreKey.xbbaInitializationVector.iData = 0;
        secureStoreKey.xbbaInitializationVector.pbDataBuffer = (DRM_BYTE*)f_pKey->rgbInitializationVector;


        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_2, &secureStoreKey ) );
    }
    else if( f_pKeyFile->keyfile.SecureStoreKeyV2.fValid )
    {
        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_2, &f_pKeyFile->keyfile.SecureStoreKeyV2 ) );
    }

    if ( f_pCertParserCache != NULL )
    {
        certParserCache.fValid = TRUE;
        certParserCache.wVersion = DRM_CERT_PARSER_CACHE_VERSION_1;

        certParserCache.xbbaHash.fValid = TRUE;
        certParserCache.xbbaHash.cbData = DRM_SHA1_DIGEST_LEN;
        certParserCache.xbbaHash.iData = 0;
        certParserCache.xbbaHash.pbDataBuffer = f_pCertParserCache->m_rgbSHAHash;

        certParserCache.wlData.fValid = TRUE;
        certParserCache.wlData.cWORDs = SIZEOF( f_pCertParserCache->m_rgbPacked ) / SIZEOF( DRM_WORD );
        certParserCache.wlData.pwordBuffer = ( DRM_BYTE * )( f_pCertParserCache->m_rgbPacked );
        certParserCache.wlData.iWords = 0;

        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERT_PARSER_CACHE_OBJECT, &certParserCache ) );
    }
    else if( f_pKeyFile->keyfile.CertParserCache.fValid )
    {
        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERT_PARSER_CACHE_OBJECT, &f_pKeyFile->keyfile.CertParserCache ) );
    }

    f_pKeyFile->keyfile.Signature.fValid = TRUE;
    f_pKeyFile->keyfile.Signature.wSignatureType = KEYFILE_SIGNATURE_TYPE_OMAC_1;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.fValid = TRUE;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.cbData = DRM_AES_BLOCKLEN;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.iData = 0;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.pbDataBuffer = f_pKeyFile->rgbKeyfileBuffer; /* Dummy data, will be overwritten during StoreToFile */
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.fValid = TRUE;
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.cbData = SIZEOF( DRM_GUID );
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.iData = 0;
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.pbDataBuffer = f_pKeyFile->rgbKeyfileBuffer; /* Dummy data, will be overwritten during StoreToFile */

    ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_SIGNATURE_OBJECT, &f_pKeyFile->keyfile.Signature ) );

    ChkDR( DRM_XB_FinishFormat( &f_pKeyFile->builderContext, f_pKeyFile->rgbKeyfileBuffer, &cbKeyfile ) );
    f_pKeyFile->cbKeyfileBuffer = cbKeyfile;

    /* We need to move the keyfile data to the end of the buffer such that next time we update
    ** keyfile data we won't be stomping on the previous keyfile data while constructing the new keyfile data
    */
    f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
    ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
    f_pKeyFile->pbKeyfileBuffer -= cbKeyfile;

    /* Since DRM_XB_FinishFormat would have failed if cbKeyfile was larger than the keyfile buffer, an
    ** integer underflow should not be possible in the preceding line
    */
    DRM_BYT_MoveBytes( f_pKeyFile->pbKeyfileBuffer, 0, f_pKeyFile->rgbKeyfileBuffer, 0, cbKeyfile );

ErrorExit:
    return TranslateKeyfileOOMError(dr);
}

DRM_API DRM_RESULT DRM_CALL DRM_KF_LoadFromMemory (
    __in_ecount(f_cbKeyFile)   const DRM_BYTE            *f_pbKeyFile,
    __in                             DRM_DWORD            f_cbKeyFile,  
    __inout                          DRM_KEYFILE_CONTEXT *f_pKeyFile )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pKeyFile  != NULL );
    ChkArg( f_pbKeyFile != NULL );
    ChkArg( f_cbKeyFile  > 0    );

    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( f_cbKeyFile <= SIZEOF( f_pKeyFile->rgbKeyfileBuffer ), DRM_E_KEYFILE_TOO_LARGE );

    f_pKeyFile->cbKeyfileBuffer = f_cbKeyFile;

    f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
    ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
    f_pKeyFile->pbKeyfileBuffer -= f_cbKeyFile;

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
DRM_API DRM_RESULT DRM_CALL DRM_KF_LoadFromFile (
    IN OUT   DRM_KEYFILE_CONTEXT        *f_pKeyFile,
    IN       DRM_BOOL                    f_fValidateSignature)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cbKeyfileLen = 0;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

    ChkBOOL( Oem_File_GetSize( f_pKeyFile->hKeyfile, &cbKeyfileLen ), DRM_E_FILEREADERROR );
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
        ** keyfile data we won't be stomping on the previous keyfile data while constructing the new keyfile data
        */
        f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
        f_pKeyFile->pbKeyfileBuffer -= cbKeyfileLen;
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


/*******************************************************************************
** Function:    DRM_KF_StoreToFile
**
** Synopsis:    Stores and signs the cached keyfile data to disk
**
**              [f_pKeyFile]     Keyfile context
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_StoreToFile (
    IN OUT   DRM_KEYFILE_CONTEXT           *f_pKeyFile)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  iObject  = 0;
    DRM_DWORD  cbObject = 0;
    DRM_GUID   guidAESDecryptionKeyID;
    DRM_DWORD iSignatureFromObjectStart = 0;
    DRM_DWORD iCertificateFromObjectStart = 0;

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->cbKeyfileBuffer > 0 );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pKeyFile->fLoaded );

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        /* If this device is using activation the flow would be a call to DRM_KF_LoadFromMemory
        ** followed by a call to StoreToFile.  In this case the keyfile is already signed
        ** and ready to write to disk.
        */

    }
    else
    {
        /* In the non activation case we need to rebuild the keyfile including signature
        ** before we can write to disk
        */

        /* Convert the data in the internal key file buffer to network byte order. */
        ChkDR( _UpdateKeyfile( f_pKeyFile, KF_CERT_TYPE_INVALID, NULL, 0, FALSE, KF_ASYMM_KEY_TYPE_INVALID, NULL, 0, NULL, NULL ) );

        /* Offset of the signature and certificate data into the signature object */
        iSignatureFromObjectStart = XB_BASE_OBJECT_LENGTH
            + SIZEOF(DRM_DWORD)
            + f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.cbData
            + SIZEOF( DRM_WORD )
            + SIZEOF( DRM_WORD );

        iCertificateFromObjectStart = XB_BASE_OBJECT_LENGTH + SIZEOF(DRM_DWORD);

        /* Caculcate the signature */
        ChkDR( DRM_XB_FindObject(
                KF_ENTRY_TYPE_SIGNATURE_OBJECT,
                &(g_kfFormatDescription[ADJUST_VERSION_TO_INDEX(g_KF_VERSION)]),
                f_pKeyFile->pbKeyfileBuffer,
                f_pKeyFile->cbKeyfileBuffer,
                &iObject,
                &cbObject ) );
    
        ChkArg( iObject + max( iSignatureFromObjectStart, iCertificateFromObjectStart ) < f_pKeyFile->cbKeyfileBuffer );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iSignatureFromObjectStart, f_pKeyFile->pbKeyfileBuffer );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iSignatureFromObjectStart + iObject, f_pKeyFile->pbKeyfileBuffer + iSignatureFromObjectStart );

        ChkDR( DRM_BBX_KF_CalculateOmac( f_pKeyFile->pOEMContext,
            &guidAESDecryptionKeyID,
            TRUE,
            f_pKeyFile->pbKeyfileBuffer,
            iObject,
            f_pKeyFile->pbKeyfileBuffer + iObject + iSignatureFromObjectStart ) );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iCertificateFromObjectStart, f_pKeyFile->pbKeyfileBuffer );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iCertificateFromObjectStart + iObject, f_pKeyFile->pbKeyfileBuffer + iCertificateFromObjectStart );
        MEMCPY( f_pKeyFile->pbKeyfileBuffer + iObject + iCertificateFromObjectStart, &guidAESDecryptionKeyID, SIZEOF( DRM_GUID ) );
    }

    /* Write the keyfile data to file */
    ChkDR( Oem_Keyfile_StoreToFile( f_pKeyFile ) );

    /* Convert the data in the internal key file buffer to machine byte order. */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

DRM_API DRM_KEYFILE_CERT_TYPE DRM_CALL MapApiCertTypeToFormatCertType( DRM_KF_CERT_TYPE f_eCertType )
{
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
DRM_API DRM_RESULT DRM_CALL DRM_KF_GetCertificate (
    IN     DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN     DRM_KF_CERT_TYPE       f_eCertType,
    OUT    DRM_BYTE             **f_ppbCertData,
    IN OUT DRM_DWORD             *f_pcbCertData)
{
    DRM_RESULT dr = DRM_SUCCESS;
    KF_CERTIFICATE_CHAIN_OBJECT_V2 *pCertChain = NULL;
    DRM_KEYFILE_CERT_TYPE eFormatCertTypeToFind = KF_CERT_TYPE_INVALID;

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

/*******************************************************************************
** Function:    DRM_KF_SetCertificate
**
** Synopsis:    Sets a certificate in the keyfile data cache
**
**              [f_pKeyFile]    Keyfile context
**              [f_eCertType]   Type of certificate to
**                              set
**              [f_pbCertData]  Certificate data to be
**                              set
**              [f_cbCertData]  Size in bytes of the certificate
**                              data
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid.
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_SetCertificate (
    IN                              DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN                              DRM_KF_CERT_TYPE       f_eCertType,
    __in_bcount(f_cbCertData)       const DRM_BYTE        *f_pbCertData,
    IN                              DRM_DWORD              f_cbCertData)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE_CERT_TYPE eFormatCertTypeToSet = KF_CERT_TYPE_INVALID;

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_E_KEYFILE_UPDATE_NOT_ALLOWED );
    }
    
    ChkArg( f_pKeyFile != NULL );
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData != 0 );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    /* Perform an explicit mapping.  This ensures that the API can change without regaurd to the
    ** core format defines
    */
    eFormatCertTypeToSet = MapApiCertTypeToFormatCertType( f_eCertType );
    ChkArg( eFormatCertTypeToSet != KF_CERT_TYPE_INVALID );

    ChkDR( _UpdateKeyfile( f_pKeyFile, eFormatCertTypeToSet, f_pbCertData, f_cbCertData, FALSE, KF_ASYMM_KEY_TYPE_INVALID, NULL, 0, NULL, NULL ) );

    /* Load the keyfile format back into the context */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

DRM_RESULT _FindAsymmetricKeyPair(
    IN                              DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN                              DRM_KEYFILE_ASYMMETRIC_KEY_TYPE f_eAsymmKeyType,
    __in_bcount(f_cbPubKey)     const DRM_BYTE        *f_pbPubKey,
    IN                              DRM_DWORD              f_cbPubKey,
    OUT                             DRM_ENCRYPTED_KEY     *f_pKey)
{
    DRM_RESULT dr = DRM_SUCCESS;
    KF_KEYPAIR_V2_OBJECT *pKeyPairV2 = NULL;
    KF_KEYPAIR_V3_OBJECT *pKeyPairV3 = NULL;

    ChkArg( f_pbPubKey != NULL );
    
    pKeyPairV3 = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv3;
    while( pKeyPairV3 != NULL && pKeyPairV3->fValid )
    {
        if( f_cbPubKey == pKeyPairV3->xbbaPublicKeyData.cbData
           && pKeyPairV3->wCipherType == f_eAsymmKeyType
           && DRM_BYT_CompareBytes( pKeyPairV3->xbbaPublicKeyData.pbDataBuffer, pKeyPairV3->xbbaPublicKeyData.iData, f_pbPubKey, 0, f_cbPubKey ) == 0 )
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
            && DRM_BYT_CompareBytes( pKeyPairV2->xbbaPublicKeyData.pbDataBuffer, pKeyPairV2->xbbaPublicKeyData.iData, f_pbPubKey, 0, f_cbPubKey ) == 0 )
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

DRM_API DRM_KEYFILE_ASYMMETRIC_KEY_TYPE DRM_CALL MapApiKeyTypeToFormatKeyType( DRM_KF_KEY_TYPE f_eKeyType )
{
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
DRM_API DRM_RESULT DRM_CALL DRM_KF_GetPrivateKey (
    IN                              DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN                              DRM_KF_KEY_TYPE        f_eKeyType,
    __in_bcount_opt(f_cbPubKey)     const DRM_BYTE        *f_pbPubKey,
    IN                              DRM_DWORD              f_cbPubKey,
    OUT                             DRM_ENCRYPTED_KEY     *f_pKey)
{
    DRM_RESULT dr = DRM_SUCCESS;

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
            DRM_BYT_CopyBytes( f_pKey->rgbInitializationVector, 0, &f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector, 0, SIZEOF( f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector ) );
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
            DRM_BYT_CopyBytes( f_pKey->rgbInitializationVector, 0, f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.pbDataBuffer, f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.iData, SIZEOF( f_pKey->rgbInitializationVector ) );
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

/*******************************************************************************
** Function:    DRM_KF_SetPrivateKey
**
** Synopsis:    Set an encrypted private key into the keyfile cache
**
**              [f_pKeyFile]     Keyfile context
**              [f_eKeyType]  Cipher type of the supplied encrypted private
**                               key
**              [f_pbPubKey]     Public key corresponding to the supplied privat
**                               e key
**              [f_cbPubKey]     Size in bytes of the public
**                               key
**              [f_pKey]         Struct containing the encrypted private
**                               key
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_SetPrivateKey (
    IN                              DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    IN                              DRM_KF_KEY_TYPE        f_eKeyType,
    __in_bcount_opt(f_cbPubKey)     const DRM_BYTE        *f_pbPubKey,
    IN                              DRM_DWORD              f_cbPubKey,
    IN                              DRM_ENCRYPTED_KEY     *f_pKey)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE_ASYMMETRIC_KEY_TYPE eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID;
    DRM_BOOL fUpdateSecureStoreKey = FALSE;

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_E_KEYFILE_UPDATE_NOT_ALLOWED );
    }

    ChkArg( f_pKeyFile != NULL );
    ChkArg( f_pKey != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    if( eKF_KEY_SECURE_STORE == f_eKeyType )
    {
        fUpdateSecureStoreKey = TRUE;
    }
    else
    {
        eAsymmKeyType = MapApiKeyTypeToFormatKeyType( f_eKeyType );

        ChkArg( eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID );
    }

    ChkDR( _UpdateKeyfile(
        f_pKeyFile,
        KF_CERT_TYPE_INVALID,
        NULL,
        0,
        fUpdateSecureStoreKey,
        eAsymmKeyType,
        f_pbPubKey,
        f_cbPubKey,
        f_pKey,
        NULL ) );

    /* Load the keyfile format back into the context */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_SetCertParserCache
**
** Synopsis:    Set the device cert parser cache BLOB into the key file cache.
**
**              [f_pKeyFile]     Keyfile context
**              [f_pCertParserCache]
**                               Pointer to a device cert parser cache BLOB.
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_SetCertParserCache (
    IN  DRM_KEYFILE_CONTEXT          *f_pKeyFile,
    IN  DRM_DEVICE_CERT_PARSER_CACHE *f_pCertParserCache)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_E_KEYFILE_UPDATE_NOT_ALLOWED );
    }

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );
    ChkArg( f_pCertParserCache != NULL );

    ChkDR( _UpdateKeyfile( f_pKeyFile, KF_CERT_TYPE_INVALID, NULL, 0, FALSE, KF_ASYMM_KEY_TYPE_INVALID, NULL, 0, NULL, f_pCertParserCache ) );

    /* Load the keyfile format back into the context */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

