/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXLEGACYSTUB_C
#include <blackboxlegacy.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
** Functions defining whether legacy (WMDRM) crypto keys and algorithms are supported
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsLegacyCryptoSupported(DRM_VOID)
{
    return FALSE;
}
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsLegacyCryptoUnsupported(DRM_VOID)
{
    return TRUE;
}


#if DBG
DRM_RESULT DRM_CALL _TestDeviceKeyPair( 
    __in_ecount( 1 ) DRM_BB_CONTEXT *f_pcontextBBX  )
{
    return DRM_E_NOTIMPL;
}
#endif /* DBG */


DRM_API DRM_RESULT DRM_CALL _Legacy_GetClientId(
    __out_ecount( 1 )    CLIENTID          *f_pClientId,
    __inout_ecount( 1 )  DRM_BB_CONTEXT    *f_pcontextBBX )
{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL DRM_BBX_Legacy_SymmetricCipher(
    __in                                DRM_BOOL  f_fEncrypt,
    __in_bcount_opt( f_cbKey )   const  DRM_BYTE *f_pbKey,
    __in                                DRM_DWORD f_cbKey,
    __in_bcount( f_cbData )             DRM_BYTE *f_pbDataIn,
    __out_bcount_opt( f_cbData )        DRM_BYTE *f_pbDataOut,
    __in                                DRM_DWORD f_cbData )
{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL _Legacy_SymmetricSign(
    __inout_ecount( 1 )                    DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbData )  const         DRM_BYTE       *f_pbData,
    __in                                   DRM_DWORD       f_cbData,
    __in_bcount_opt( f_cbExtraData ) const DRM_BYTE       *f_pbExtraData,
    __in                                   DRM_DWORD       f_cbExtraData,
    __out_bcount( DRM_SHA1_DIGEST_LEN )    DRM_BYTE        f_rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] )
{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL _Legacy_SignData(
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbData )  const           DRM_BYTE       *f_pbData,
    __in                                     DRM_DWORD       f_cbData,
    __in                                     DRM_DWORD       f_ePrivKeyType,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN ) DRM_BYTE        f_rgbSignature[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )] )
{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL _Legacy_SymmetricVerify(
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt                           const PUBKEY         *f_pPubKey,
    __in_bcount( f_cbData )            const DRM_BYTE       *f_pbData,
    __in                                     DRM_DWORD       f_cbData,
    __in_bcount_opt( f_cbExtraData )   const DRM_BYTE       *f_pbExtraData,
    __in                                     DRM_DWORD       f_cbExtraData,
    __in_opt                           const DRM_SLK        *f_pslk,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE        f_rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] )
{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_BOOL DRM_CALL _Legacy_DecryptLicense(
    __inout_bcount( f_cbEncryptedLicense )   DRM_BYTE       *f_pbEncryptedLicense,
    __in                                     DRM_DWORD       f_cbEncryptedLicense,
    __out_bcount_opt( f_cbEncryptedLicense - DRM_ECC160_CIPHERTEXT_LEN )
                                             DRM_BYTE       *f_pbLicense,
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt                           const PUBKEY         *f_pDevPubKey )

{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL _AsymmetricToSymmetricKeyXML(
    __inout_ecount( 1 )      DRM_BB_CONTEXT    *f_pcontextBBX,
    __inout_ecount_opt( 1 )  DRM_BINDING_INFO  *f_pbindinfo,
    __out_ecount( 1 )        DRM_DWORD         *f_pcbSymmKey )
{
    return DRM_E_NOTIMPL;
}


DRM_RESULT _DecryptContentKeyXML(
    __in_ecount( f_dwChainLen ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                              DRM_DWORD            f_dwChainLen,
    __inout_ecount( 1 )               DRM_BB_CONTEXT      *f_pcontextBBX,
    __in                              DRM_BOOL             f_IgnoreChecksum,
    __in                              DRM_BOOL             f_IgnoreCKSig,
    __in                              DRM_DWORD            f_cbKeyBuff,
    __out_bcount( f_cbKeyBuff )       DRM_BYTE            *f_pbKeyBuff,
    __out_ecount( 1 )                 DRM_DWORD           *f_pcbKey,
    __out_opt                         PRIVKEY             *f_pPrivKey )
{
    return DRM_E_NOTIMPL;
}


DRM_RESULT DRM_API DRM_CALL _KF_Legacy_DecryptKey(
    __in_opt                                       DRM_VOID             *f_pOEMContext,
    __in                                     const DRM_ENCRYPTED_KEY    *f_pEncryptedKey,
    __in                                           DRM_DEVICE_KEY_TYPE   f_eKeyType,
    __inout_bcount_opt( *f_pcbDecryptedKey )       DRM_BYTE             *f_pbDecryptedKey,
    __inout                                        DRM_DWORD            *f_pcbDecryptedKey )
{
    return DRM_E_NOTIMPL;
}


DRM_RESULT DRM_API DRM_CALL _KF_Legacy_EncryptKey (
    __in_opt                        DRM_VOID                  *f_pOEMContext,
    __in_bcount(f_cbClearKey) const DRM_BYTE                  *f_pbClearKey,
    __in                            DRM_DWORD                  f_cbClearKey,
    __in                            DRM_DEVICE_KEY_TYPE        f_eKeyType,
    __out                           DRM_ENCRYPTED_KEY         *f_pEncryptedKey )
{
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL _Legacy_GenerateECCKeyPair(
    __inout                             DRM_CRYPTO_CONTEXT                 *f_pCryptoCtx,
    __in                                DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPublicKey,
    __out_bcount( *f_pcbPublicKey )     DRM_BYTE                           *f_pbPublicKey,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPrivateKey,
    __out_bcount( *f_pcbPrivateKey )    DRM_BYTE                           *f_pbPrivateKey)
{
    return DRM_E_NOTIMPL;
}


DRM_RESULT _GetDevicePrivkey(
    __inout_ecount( 1 )    DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt         const PUBKEY         *f_pDevPubKey,
    __out_ecount( 1 )      PRIVKEY        *f_pDevPrivKey )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;

