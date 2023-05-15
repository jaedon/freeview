/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOXMODULES_C
#include <blackboxlegacy.h>
#include <blackboxslk.h>
#include <blackboxinternal.h>
#include <oemeccp160.h>
#include <oemkeyfile.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmprofileconstants.h>
#include <drmwmdrm.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
**  Function:  DRM_BBX_GetClientId
**
**  Synopsis:  Return the WMDRM client ID.
**
**  Arguments:
**     [f_pClientId]   : returns the client ID
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_GetClientId(
    __out_ecount( 1 )    CLIENTID          *f_pClientId,
    __inout_ecount( 1 )  DRM_BB_CONTEXT    *f_pcontextBBX )
{
    return _Legacy_GetClientId( f_pClientId, f_pcontextBBX );
}


/*********************************************************************
**
**  Function:  DRM_BBX_SymmetricSign
**
**  Synopsis:  Creates a symmetric signature (that only the current blackbox can verify) over the data provided
**             by the caller
**
**  Arguments:
**     [f_pcontextBBX] -- Pointer to an initialized blackbox context
**     [f_pbData]      -- Pointer to the data to sign
**     [f_cbData]      -- Length of f_pbData in DRM_BYTEs
**     [f_pbExtraData] -- Additional data added to signed data.
**     [f_cbExtraData] -- Length of f_pbExtraData in bytes.
**     [f_rgbSymSig]   -- Buffer to place the resultant 'symmetric signature'
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SymmetricSign(
    __inout_ecount( 1 )                    DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbData )  const         DRM_BYTE       *f_pbData,
    __in                                   DRM_DWORD       f_cbData,
    __in_bcount_opt( f_cbExtraData ) const DRM_BYTE       *f_pbExtraData,
    __in                                   DRM_DWORD       f_cbExtraData,
    __out_bcount( DRM_SHA1_DIGEST_LEN )    DRM_BYTE        f_rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] )
{
    return _Legacy_SymmetricSign( f_pcontextBBX,
                                  f_pbData,
                                  f_cbData,
                                  f_pbExtraData,
                                  f_cbExtraData,
                                  f_rgbSymSig );
}


/******************************************************************************
** Function :   DRM_BBX_SignData
** Synopsis :   Sign data with the machine private key
** Arguments :  f_pcontextBBX       - Blackbox context
**              f_pbData            - Data to be signed
**              f_cbData            - Size of data to be signed
**              f_rgbSignature      - Signature
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SignData(
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbData )  const           DRM_BYTE       *f_pbData,
    __in                                     DRM_DWORD       f_cbData,
    __in                                     DRM_DWORD       f_ePrivKeyType,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN ) DRM_BYTE        f_rgbSignature[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )] )
{
    return _Legacy_SignData( f_pcontextBBX,
                             f_pbData,
                             f_cbData,
                             f_ePrivKeyType,
                             f_rgbSignature );
}


/*********************************************************************
**
**  Function:  DRM_BBX_SymmetricVerify
**
**  Synopsis:  Verifies a symmetric signtaure that was created by a call to DRM_BBX_SymmetricSign
**
**  Arguments:
**     [f_pcontextBBX] -- Pointer to an initialized blackbox context
**     [f_pPubKey]     -- Pointer to an ECC 160 public key whose matching private key will be used
**                        to do the verification. If it is NULL, the current private key will be used.
**     [f_pbData]      -- Pointer to the data to sign
**     [f_cbData]      -- Length of f_pbData in DRM_BYTEs
**     [f_pbExtraData] -- Additional data added to signed data.
**     [f_cbExtraData] -- Length of f_pbExtraData in bytes.
**     [f_pslk]        -- Optional.  If this parameter is provided the caller is asking to verify
**                        the signature with a BBX protected SLK key.
**     [f_rgbSymSig]   -- Buffer containing the signtature to compare against.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SymmetricVerify(
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt                           const PUBKEY         *f_pPubKey,
    __in_bcount( f_cbData )            const DRM_BYTE       *f_pbData,
    __in                                     DRM_DWORD       f_cbData,
    __in_bcount_opt( f_cbExtraData )   const DRM_BYTE       *f_pbExtraData,
    __in                                     DRM_DWORD       f_cbExtraData,
    __in_opt                           const DRM_SLK        *f_pslk,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE        f_rgbSymSig[__CB_DECL( DRM_SHA1_DIGEST_LEN )] )
{
    return _Legacy_SymmetricVerify( f_pcontextBBX,
                                    f_pPubKey,
                                    f_pbData,
                                    f_cbData,
                                    f_pbExtraData,
                                    f_cbExtraData,
                                    f_pslk,
                                    f_rgbSymSig );
}


/*********************************************************************
**
**  Function:  DRM_BBX_DecryptLicense
**
**  Synopsis:  Decrypt a license that was encryted for privacy reasons (not
**             security reasons) while traveling on the net during license
**             acquisition.
**
**  Arguments:
**     [f_pbEncryptedLicense] -- Pointer to an encrypted license buffer
**     [f_cbEncryptedLicense] -- Size, in bytes, of license buffer
**     [f_pbLicense]          -- returns the decrypted license
**     [f_pcontextBBX]        -- Pointer to an initialized blackbox context
**     [f_pDevPubKey]         -- Returns public key of decyrpting private key.
**
*********************************************************************/
DRM_API DRM_BOOL DRM_CALL DRM_BBX_DecryptLicense(
    __inout_bcount( f_cbEncryptedLicense )   DRM_BYTE       *f_pbEncryptedLicense,
    __in                                     DRM_DWORD       f_cbEncryptedLicense,
    __out_bcount_opt( f_cbEncryptedLicense - DRM_ECC160_CIPHERTEXT_LEN )
                                             DRM_BYTE       *f_pbLicense,
    __inout_ecount( 1 )                      DRM_BB_CONTEXT *f_pcontextBBX,
    __in_opt                           const PUBKEY         *f_pDevPubKey )

{
    return _Legacy_DecryptLicense( f_pbEncryptedLicense,
                                   f_cbEncryptedLicense,
                                   f_pbLicense,
                                   f_pcontextBBX,
                                   f_pDevPubKey );
}


/*********************************************************************
**
**  Function:  DRM_BBX_AsymmetricToSymmetricKeyXML
**
**  Synopsis:  Takes an asymmetrically encrypted XML license and symmetrically
**             rebinds it to the current blackbox.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pbindinfo]   : Pointer to the binding info of the license that is being optmized
**     [f_pcbSymmKey]  : Length of the resultant symmetric content key
**
**  Notes:  The content key will be reencrypted and returned in the same buffer.
**          The new length of the symmetric key is returned in f_pcbSymmKey.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_AsymmetricToSymmetricKeyXML(
    __inout_ecount( 1 )      DRM_BB_CONTEXT    *f_pcontextBBX,
    __inout_ecount_opt( 1 )  DRM_BINDING_INFO  *f_pbindinfo,
    __out_ecount( 1 )        DRM_DWORD         *f_pcbSymmKey )
{
    return _AsymmetricToSymmetricKeyXML( f_pcontextBBX,
                                         f_pbindinfo,
                                         f_pcbSymmKey );
}


/*****************************************************************************
** Function:    DRM_BBX_KF_Legacy_DecryptKey
**
** Synopsis:    Decrypts data (a key) using a key stored in keyfile.
**
** Arguments:   [f_pOEMContext]     : A pointer to the OEM context; may be NULL
**              [f_pEncryptedKey]   : A structure containing encrypted key data
**              [f_eKeyType]        : Describes the key type being decrypted.
**                                    Different key types may need different
**                                    operations done on them. In this implementation
**                                    the type is ignored.
**              [f_pbDecryptedKey]  : A pointer to the buffer to receive the unique ID
**              [f_pcbDecryptedKey] : Buffer to receive the decrypted key of the size
**                                    indicated in the f_pcbEncryptedKey struct
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**              DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD - if invalid decryption method
**              if specified in the f_pcbEncryptedKey struct.
**
** Notes:       None.
**
******************************************************************************/

DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_Legacy_DecryptKey(
    __in_opt                                       DRM_VOID             *f_pOEMContext,
    __in                                     const DRM_ENCRYPTED_KEY    *f_pEncryptedKey,
    __in                                           DRM_DEVICE_KEY_TYPE   f_eKeyType,
    __inout_bcount_opt( *f_pcbDecryptedKey )       DRM_BYTE             *f_pbDecryptedKey,
    __inout                                        DRM_DWORD            *f_pcbDecryptedKey )
{
    return _KF_Legacy_DecryptKey( f_pOEMContext,
                                  f_pEncryptedKey,
                                  f_eKeyType,
                                  f_pbDecryptedKey,
                                  f_pcbDecryptedKey );
}


/***********************************************************************************
** Function:    DRM_BBX_KF_Legacy_EncryptKey
**
** Synopsis:    Encrypts data (a key) to be stored in the keyfile.
**
** Arguments:   [f_pOEMContext]   : A pointer to the OEM context; may be NULL
**              [f_pbClearKey]    : Buffer containing clear key to be encrypted
**              [f_cbClearKey]    : Size in bytes of the clear key to be encrypted
**              [f_eKeyType]      : See description for DRM_BBX_KF_Legacy_DecryptKey
**              [f_pEncryptedKey] : Structure to be populated with encrypted key
**                                  and metadata about encryption.
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**
**
** Notes:       f_pEncryptedKey->pbEncryptedKey buffer to store encrypted key value
**              is already allocated by a caller.
***********************************************************************************/
DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_Legacy_EncryptKey (
    __in_opt                        DRM_VOID                  *f_pOEMContext,
    __in_bcount(f_cbClearKey) const DRM_BYTE                  *f_pbClearKey,
    __in                            DRM_DWORD                  f_cbClearKey,
    __in                            DRM_DEVICE_KEY_TYPE        f_eKeyType,
    __out                           DRM_ENCRYPTED_KEY         *f_pEncryptedKey )
{
    return _KF_Legacy_EncryptKey( f_pOEMContext,
                                  f_pbClearKey,
                                  f_cbClearKey,
                                  f_eKeyType,
                                  f_pEncryptedKey );
}


/*********************************************************************
**
**  Function:  DRM_BBX_RebindSymmetricKeyXMR
**
**  Synopsis:  Takes an license which is symmetrically optimized to an SLK
**             and re-encrypts it to the device (or domain) symmetric key.
**             Legacy or current crypto is inferred from SLK size.
**             Reencrypts and updates the specified XMR license.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pslk]        : Pointer to current SLK data
**     [f_pXmrLicense] : Pointer to an XMR license object to update with the new key and signature
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_RebindSymmetricKeyXMR(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XMR_LICENSE      *f_pXmrLicense )
{
    return _SLK_RebindSymmetricKeyXMR( f_pcontextBBX, f_pslk, f_pXmrLicense );
}


/*********************************************************************
**
**  Function:  DRM_BBX_RebindSLK
**
**  Synopsis:  Takes an SLK (ILA Session Key) encrypted with the device
**             private key and re-encrypts it to the corresponding symmetric key,
**             updating the appropriate members of the SLK structure.
**             Legacy or current crypto is inferred from encrypted key size.
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pbEncSLK]    : Pointer to the encrypted SLK data
**     [f_cbEncSLK]    : Length of f_pbEncSLK
**     [f_pslk]        : Returns the re-encrypted SLK and it's length
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_RebindSLK(
    __inout_ecount( 1 )             DRM_BB_CONTEXT *f_pcontextBBX,
    __in_bcount( f_cbEncSLK ) const DRM_BYTE       *f_pbEncSLK,
    __in                            DRM_DWORD       f_cbEncSLK,
    __out_ecount( 1 )               DRM_SLK        *f_pslk )
{
    return _SLK_RebindSLK( f_pcontextBBX, f_pbEncSLK, f_cbEncSLK, f_pslk );
}


/*********************************************************************
**
**  Function:  DRM_BBX_RebindSymmetricKeyXML
**
**  Synopsis:  Takes an license which is symmetrically optimized to an SLK
**             and re-encrypts it to the device (or domain) symmetric key.
**             Legacy or current crypto is inferred from SLK size.
**             Reencrypts in place and verifies the content key signature,
**             using the specified Binding Info struct
**
**  Arguments:
**     [f_pcontextBBX] : Pointer to an initialized blackbox context
**     [f_pslk]        : Pointer to current SLK data
**     [f_pXmlBindInfo]: Pointer to XML Bind info structure containing the key to reencrypt,
**                       and signature data to verify.
**     [f_cbSymmKey]   : Length of the symmetrically encrypted key data.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_RebindSymmetricKeyXML(
    __inout         DRM_BB_CONTEXT       *f_pcontextBBX,
    __in      const DRM_SLK              *f_pslk,
    __inout         DRM_XML_BINDING_INFO *f_pXmlBindInfo,
    __in            DRM_DWORD             f_cbSymmKey )
{
    return _SLK_RebindSymmetricKeyXML( f_pcontextBBX, f_pslk, f_pXmlBindInfo, f_cbSymmKey );
}


/*********************************************************************
**
** Function: DRM_BBX_Legacy_GenerateECCKeyPair
**
** Synopsis:
**           Generation of keypairs is an important operation from
**           robustness perspective - private key of is one
**           of the most important robustness assets. Blackbox.c file
**           and oem files are the source files that should be the
**           only ones to be edited to increase the protection of
**           robustness assets.
**           In the given implementation this function just calls
**           a plain one to generate a P160 or P256 ECC key pair.
**           It should be called whenever a P160/P256 key pair for
**           encryption/decryption is generated.
**
** Arguments:
**  [f_pCryptoCtx]              -- A pointer to the crypto context
**  [f_eAlgorithm]              -- Enum describing what algorithm to
**                                 use for key generation.
**  [f_pcbPublicKey]            -- Size of pubkey buffer
**  [f_pbPublicKey]             -- Pubkey buffer
**  [f_pcbPrivateKey]           -- Size of privkey buffer
**  [f_pbPrivateKey]            -- Privkey buffer
**
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_BBX_Legacy_GenerateECCKeyPair(
    __inout                             DRM_CRYPTO_CONTEXT                 *f_pCryptoCtx,
    __in                                DRM_PKCRYPTO_SUPPORTED_ALGORITHMS   f_eAlgorithm,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPublicKey,
    __out_bcount( *f_pcbPublicKey )     DRM_BYTE                           *f_pbPublicKey,
    __inout_ecount( 1 )                 DRM_DWORD                          *f_pcbPrivateKey,
    __out_bcount( *f_pcbPrivateKey )    DRM_BYTE                           *f_pbPrivateKey)
{
    return _Legacy_GenerateECCKeyPair( f_pCryptoCtx,
                                       f_eAlgorithm,
                                       f_pcbPublicKey,
                                       f_pbPublicKey,
                                       f_pcbPrivateKey,
                                       f_pbPrivateKey );
}


EXIT_PK_NAMESPACE_CODE;

