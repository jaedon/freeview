/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DEVCERTKEYGENIMPLREAL_C
#include <drmdevcertkeygen.h>
#include <devicedevcert.h>
#include <drmxmlparser.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <drmsecureclock.h>
#include <drmantirollbackclock.h>
#include <drmsymopt.h>
#include <drmrevocation.h>
#include <drmprnd.h>
#include <drmdevcertconstants.h>
#include <oemeccp160.h>
#include <drmconstants.h>
#include <oemkeyfileconsts.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_DDC_IsDevCertKeyGenSupported(DRM_VOID)   { return TRUE; }
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_DDC_IsDevCertKeyGenUnsupported(DRM_VOID) { return FALSE; }

#define MAX_WMDRMMD_FEATURE_FLAGS       4

/**********************************************************************************
** Function:    _CreateDevPrivateKeyAndCert
**
** Synopsis:    Creates new XML device certificate and adds it to keyfile
**
** Arguments:   [f_pOEMContext]       -- A pointer to the OEM Context; may be NULL
**              [f_poKeyFileContext]  -- keyfile context
**              [f_pdstrWMDRMCertStr] -- A pointer to a DRM String to contain the
**                                       XML device certificate
**              [f_pcontextCRYP]      -- A pointer to the crypto context
**
** Returns:     DRM_SUCCESS on success
**
** Notes:
************************************************************************************/
static DRM_RESULT DRM_CALL _CreateDevPrivateKeyAndCert(
    __in_opt               DRM_BB_CONTEXT          *f_pBbxContext,
    __in                   DRM_KEYFILE_CONTEXT     *f_poKeyFileContext,
    __inout                DRM_STRING              *f_pdstrWMDRMCertStr )
{
    DRM_RESULT        dr               = DRM_SUCCESS;
    DRM_CONST_STRING  dstrCertNodeData = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrInsert       = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrPubkey       = EMPTY_DRM_STRING;
    PRIVKEY           privkeyDevCert   = {0};
    PUBKEY            pubkeyDevCert    = {0};
    DRM_DWORD         cbSignature      = DRM_ECC160_SIGNATURE_LEN;
    DRM_DWORD         cbPubKey         = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD         cbPrivKey        = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_ENCRYPTED_KEY oEncryptedKey    = { 0 };
    PKCERT            cert             = {0};
    DRM_BYTE          rgbScratch[__CB_DECL(DRM_ECC160_PRIVATE_KEY_LEN
                                + (DRM_AES_BLOCKLEN
                                - DRM_ECC160_PRIVATE_KEY_LEN % DRM_AES_BLOCKLEN)% DRM_AES_BLOCKLEN) ] = {0};

    ChkArg( f_poKeyFileContext != NULL );
    ChkArg( f_pdstrWMDRMCertStr != NULL );
    ChkArg( f_pBbxContext != NULL );

    /*
    ** Make room for the key in the device certificate.  Find the node using the attribute.
    */
    ChkDR( DRM_DCP_GetAttribute( NULL,
                                 (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                 DRM_DEVCERT_DEVICEPUBKEY,
                                 NULL,
                                 &dstrPubkey ) );

    ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrWMDRMCertStr, &dstrPubkey ) );
    ChkDR( DRM_UTL_StringInsertBlankSubString(
                f_pdstrWMDRMCertStr,
                (DRM_DWORD)(dstrPubkey.pwszString - f_pdstrWMDRMCertStr->pwszString),
                CCH_BASE64_EQUIV(DRM_ECC160_PUBLIC_KEY_LEN ) ) );

    dstrPubkey.cchString = CCH_BASE64_EQUIV( DRM_ECC160_PUBLIC_KEY_LEN );

    /*
    ** Generate device cert's key pair, encrypt
    ** and set it in the keyfile right away
    */
    ChkDR(DRM_BBX_Legacy_GenerateECCKeyPair( &f_pBbxContext->CryptoContext,
                                             eDRM_ECC_P160,
                                             &cbPubKey,
                                             ( DRM_BYTE * )&pubkeyDevCert,
                                             &cbPrivKey,
                                            ( DRM_BYTE * )&privkeyDevCert) );

    /*
    ** copy to scratch buffer which is slightly larger and aligned by the size of AES cypher block
    */
    oEncryptedKey.pbEncryptedKey = rgbScratch;
    oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

    ChkDR( DRM_BBX_KF_Legacy_EncryptKey ( f_pBbxContext->pOEMContext,
                                        ( DRM_BYTE * )&privkeyDevCert,
                                        DRM_ECC160_PRIVATE_KEY_LEN,
                                        DRM_DKT_WMDRMPD_DEVICE,
                                        &oEncryptedKey ) );
    ChkDR( DRM_KF_SetPrivateKey ( f_poKeyFileContext,
                                  eKF_KEY_TYPE_ECC_160,
                                  ( DRM_BYTE * )&pubkeyDevCert,
                                  cbPubKey,
                                  &oEncryptedKey ) );

    /*
    ** Save key in the cert
    */

    /*
    ** Locate the security version
    */
    ChkDR( DRM_XML_GetSubNode( (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                               &g_dstrFallBack,
                               NULL,
                               NULL,
                               0,
                               NULL,
                               &dstrCertNodeData,
                               1 ) );
    ChkDR( DRM_XML_GetNode( &dstrCertNodeData,
                            &g_dstrTagDataSecVer,
                            NULL,
                            NULL,
                            0,
                            NULL,
                            &dstrInsert ) );
    {
        DRM_WORD rgwVersion[VERSION_LEN];
        DRM_LONG i = 0;
        ChkDR( DRM_UTL_GetVersionFromString( dstrInsert.pwszString,
                                             dstrInsert.cchString,
                                             rgwVersion ) );
        for( i = 0; i < VERSION_LEN; i++ )
        {
            PUT_BYTE( cert.pk.version, i, (DRM_BYTE)rgwVersion[i] );
        }
    }

    /*
    ** Make room and sign the data node with model-level private key
    */
    ChkDR( DRM_XML_GetSubNode(
             (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
             &g_dstrFallBack,
             NULL,
             NULL,
             0,
             NULL,
             &dstrCertNodeData,
             1 ) );

    ChkDR( DRM_XML_GetNode(
             &dstrCertNodeData,
             &g_dstrTagCertificate,
             &g_dstrRootSigValueVersionTag,
             &g_dstrRootSigValueVersionVal,
             0,
             NULL,
             &dstrInsert ) );

    ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrWMDRMCertStr, &dstrInsert ) );
    ChkDR( DRM_UTL_StringInsertBlankSubString(
            f_pdstrWMDRMCertStr,
            (DRM_DWORD)(dstrInsert.pwszString - f_pdstrWMDRMCertStr->pwszString),
            CCH_BASE64_EQUIV(SIZEOF(PKCERT)) ) );
    dstrInsert.cchString = CCH_BASE64_EQUIV(DRM_ECC160_SIGNATURE_LEN);
    MEMCPY(&cert.pk.pk, (DRM_BYTE*)&pubkeyDevCert, SIZEOF(PUBKEY));
    ChkDR( DRM_BBX_SignWithCertKeys( f_pBbxContext,
                                   DRM_DKT_WMDRMPD_FALLBACK,
                                   OEM_CRYPTO_HANDLE_INVALID,
                                   SIZEOF(PKV),
                                   (DRM_BYTE *)&cert.pk,
                                  &cbSignature,
                                   cert.sign ) );

    MEMCPY( PB_DSTR(&dstrInsert), &cert, SIZEOF(PKCERT) );

    /*
    ** B64 encode cert signature in place
    */
    dstrInsert.cchString = CCH_BASE64_EQUIV(SIZEOF(PKCERT));
    ChkDR( DRM_B64_EncodeW( PB_DSTR(&dstrInsert),
                            SIZEOF(PKCERT),
                            (DRM_WCHAR*)dstrInsert.pwszString,
                            &dstrInsert.cchString,
                            0 ) );

    /*
    ** B64 encode public key in place
    */
    ChkDR(DRM_B64_EncodeW( (DRM_BYTE*)&pubkeyDevCert,
                            SIZEOF(PUBKEY),
                            (DRM_WCHAR*)dstrPubkey.pwszString,
                            &dstrPubkey.cchString,
                            0) );

ErrorExit:
    return dr;
}


#if DRM_SUPPORT_DEVICE_SIGNING_KEY
/**********************************************************************************
** Function:    _CreateDevSigningKeyAndCert
**
** Synopsis:    Creates new XML device certificate and adds it to keyfile
**
** Arguments:   [f_pBlackBoxContext]  -- A pointer to the blackbox Context
**              [f_poKeyFileContext]  -- keyfile context
**              [f_pdstrWMDRMCertStr] -- A pointer to a DRM String to contain the
**                                       XML device certificate
**
** Returns:     DRM_SUCCESS on success
**
** Notes:
************************************************************************************/
static DRM_RESULT DRM_CALL _CreateDevSigningKeyAndCert(
    __in                   DRM_BB_CONTEXT          *f_pBlackBoxContext,
    __in                   DRM_KEYFILE_CONTEXT     *f_poKeyFileContext,
    __inout                DRM_STRING              *f_pdstrWMDRMCertStr
)
{
    DRM_RESULT        dr                   = DRM_SUCCESS;
    DRM_CONST_STRING  dstrCert             = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrSigningNodeData  = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrSecurityVersion  = EMPTY_DRM_STRING;

    PRIVKEY           privkey             = {0};
    PUBKEY            pubkey              = {0};
    DRM_DWORD         cbSignature         = DRM_ECC160_SIGNATURE_LEN;
    DRM_DWORD         cbPubKey            = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD         cbPrivKey           = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_ENCRYPTED_KEY oEncryptedKey       = {0};
    PKCERT            cert                = {0};
    DRM_BYTE          rgbScratch[__CB_DECL(DRM_ECC160_PRIVATE_KEY_LEN
                                + (DRM_AES_BLOCKLEN
                                - DRM_ECC160_PRIVATE_KEY_LEN % DRM_AES_BLOCKLEN)% DRM_AES_BLOCKLEN) ] = {0};

    ChkArg( f_poKeyFileContext != NULL );
    ChkArg( f_pdstrWMDRMCertStr != NULL );
    ChkArg( f_pBlackBoxContext != NULL );


    /*
    ** Make room for the signing cert in the device certificate.  Find the node using the attribute.
    */
    ChkDR( DRM_DCP_GetAttribute( NULL,
                                 (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                 DRM_DEVCERT_DEVICESIGNINGCERTIFICATE,
                                 NULL,
                                 &dstrCert ) );

    ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrWMDRMCertStr, &dstrCert ) );
    ChkDR( DRM_UTL_StringInsertBlankSubString(
            f_pdstrWMDRMCertStr,
            (DRM_DWORD)(dstrCert.pwszString - f_pdstrWMDRMCertStr->pwszString),
            CCH_BASE64_EQUIV(SIZEOF(PKCERT)) ) );
    dstrCert.cchString = CCH_BASE64_EQUIV(DRM_ECC160_SIGNATURE_LEN);

    /*
    ** Generate device cert's signing key pair, encrypt
    ** and set it in the keyfile right away
    */
    ChkDR(DRM_BBX_Legacy_GenerateECCKeyPair( &f_pBlackBoxContext->CryptoContext,
                                             eDRM_ECC_P160,
                                             &cbPubKey,
                                             ( DRM_BYTE * )&pubkey,
                                             &cbPrivKey,
                                            ( DRM_BYTE * )&privkey ) );

    /*
    ** copy to scratch buffer which is slightly larger and aligned by the size of AES cypher block
    */
    oEncryptedKey.pbEncryptedKey = rgbScratch;
    oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

    ChkDR( DRM_BBX_KF_Legacy_EncryptKey ( &f_pBlackBoxContext->CryptoContext,
                                          ( DRM_BYTE * )&privkey,
                                          DRM_ECC160_PRIVATE_KEY_LEN,
                                          DRM_DKT_WMDRMPD_DEVICE,
                                          &oEncryptedKey ) );
    ChkDR( DRM_KF_SetPrivateKey ( f_poKeyFileContext,
                                  eKF_KEY_TYPE_ECC_160,
                                  ( DRM_BYTE * )&pubkey,
                                  cbPubKey,
                                  &oEncryptedKey ) );

    /*
    ** Save key in the cert
    */

    /*
    ** Locate the security version for the signing cert
    */
    ChkDR( DRM_XML_GetSubNode( (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                               &g_dstrTagSigning,
                               NULL,
                               NULL,
                               0,
                               NULL,
                               &dstrSigningNodeData,
                               1 ) );
    ChkDR( DRM_XML_GetNode( &dstrSigningNodeData,
                            &g_dstrTagDataSecVer,
                            NULL,
                            NULL,
                            0,
                            NULL,
                            &dstrSecurityVersion ) );
    {
        DRM_WORD rgwVersion[VERSION_LEN];
        DRM_LONG i = 0;
        ChkDR( DRM_UTL_GetVersionFromString( dstrSecurityVersion.pwszString,
                                             dstrSecurityVersion.cchString,
                                             rgwVersion ) );
        for( i = 0; i < VERSION_LEN; i++ )
        {
            PUT_BYTE( cert.pk.version, i, (DRM_BYTE)rgwVersion[i] );
        }
    }

    /*
    ** Sign the data node with model-level private key
    */
    MEMCPY(&cert.pk.pk, (DRM_BYTE*)&pubkey, SIZEOF(PUBKEY));
    ChkDR( DRM_BBX_SignWithCertKeys( f_pBlackBoxContext,
                                     DRM_DKT_WMDRMPD_FALLBACK,
                                     OEM_CRYPTO_HANDLE_INVALID,
                                     SIZEOF(PKV),
                                    (DRM_BYTE *)&cert.pk,
                                    &cbSignature,
                                    cert.sign ) );

    MEMCPY( PB_DSTR(&dstrCert), &cert, SIZEOF(PKCERT) );

    /*
    ** B64 encode cert signature in place
    */
    dstrCert.cchString = CCH_BASE64_EQUIV(SIZEOF(PKCERT));
    ChkDR( DRM_B64_EncodeW( PB_DSTR(&dstrCert),
                            SIZEOF(PKCERT),
                            (DRM_WCHAR*)dstrCert.pwszString,
                            &dstrCert.cchString,
                            0 ) );

ErrorExit:
    return dr;
}
#endif /* DRM_SUPPORT_DEVICE_SIGNING_KEY */


/**********************************************************************************
** Function:    DRM_DDC_SetXMLDeviceCertificateAndKey
**
** Synopsis:    Creates new XML device certificate and adds it to keyfile
**
** Arguments:   [f_pBlackBoxContext]  -- A pointer to the blackbox Context
**              [f_poKeyFileContext]  -- keyfile context
**              [f_pdstrWMDRMCertStr] -- A pointer to a DRM String to contain the
**                                       XML device certificate
**
** Returns:     DRM_SUCCESS on success
**
** Notes:
************************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_SetXMLDeviceCertificateAndKey(
    __in           DRM_BB_CONTEXT      *f_pBlackBoxContext,
    __in           DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __inout        DRM_STRING          *f_pdstrWMDRMCertStr )
{
    DRM_RESULT       dr                       = DRM_SUCCESS;
    DRM_DWORD        cbDevCert                = 0;
    DRM_DWORD        cchMaxDevCert            = 0;
    DRM_DWORD        cbUniqueID               = 0;
    PRIVKEY          GCPrivKey                = {0};
    DRM_CONST_STRING dstrCertNodeData         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataNodeData         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrInsert               = EMPTY_DRM_STRING;
    DRM_DWORD        cchFutureCertSize        = 0;
    DRM_STRING       dstrOemModelName         = EMPTY_DRM_STRING;
    DRM_WCHAR        rgwchOemModelName[ (DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH * 6 / SIZEOF( DRM_WCHAR )) + 1 ] = {0};
#if !DRM_SUPPORT_KEYFILE
    DRM_BYTE         rgbSymmSig[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_DWORD        cbGCPrivKey              = SIZEOF( PRIVKEY );
#endif /* !DRM_SUPPORT_KEYFILE */

    ChkPtr( f_pBlackBoxContext );

    /*
    ** Generate WMDRM-MD certificate.
    */

    /*
    ** Get devcert template. And check the size. If size of Template is
    ** greater than MAX_DEVICE_CERT_TEMPLATE_SIZE, return error
    */
    cchMaxDevCert = f_pdstrWMDRMCertStr->cchString;

    cbDevCert = min(MAX_DEVICE_CERT_TEMPLATE_SIZE, cchMaxDevCert * SIZEOF(DRM_WCHAR));
    dr = Oem_Device_GetCert( f_pBlackBoxContext->pOEMContext,
                             DRM_DCT_WMDRMPD_TEMPLATE,
                             PB_DSTR( f_pdstrWMDRMCertStr ),
                             &cbDevCert );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( DRM_E_INVALID_DEVCERT_TEMPLATE );
    }
    else if( DRM_FAILED( dr ) )
    {
        /*Its a fatal error.*/
        ChkDR( DRM_E_DEVCERT_READ_ERROR );
    }

    f_pdstrWMDRMCertStr->cchString = cbDevCert / SIZEOF( DRM_WCHAR );

    /*
    ** Don't bother to verify the signature of the DAC and GC certificates.
    ** If they are bad there is no recovery anyway.  Upstream applications will
    ** (PC and WMRM) will validate this before issuing licenses anyway.
    */

    /* calc size needed for model name*/
    dr = Oem_Device_GetModelInfo( f_pBlackBoxContext->pOEMContext, NULL, &dstrOemModelName.cchString );
    if( dr == DRM_E_NOTIMPL )
    {
        /* Model name string for the devcert not supported on this platform */
        dstrOemModelName.cchString = 0;
    }
    else if( dr == DRM_E_BUFFERTOOSMALL || DRM_SUCCEEDED(dr) )
    {
        /* XML encode the model name in case it contains characters like '<' etc */
        DRM_WCHAR rgwchOemModelNameTemp[ (DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF( DRM_WCHAR )) + 1 ] = {0};
        DRM_CONST_STRING dstrOemModelNameTemp = EMPTY_DRM_STRING;

        ChkBOOL( dstrOemModelName.cchString < DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF( DRM_WCHAR ) + 1, DRM_E_BCERT_MANUFACTURER_STRING_TOO_LONG );

        dstrOemModelNameTemp.cchString = (DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF( DRM_WCHAR )) + 1;
        dstrOemModelNameTemp.pwszString = rgwchOemModelNameTemp;
        ChkDR( Oem_Device_GetModelInfo( f_pBlackBoxContext->pOEMContext, rgwchOemModelNameTemp, &dstrOemModelNameTemp.cchString) );

        // *6 to reserve enough buffer for extreme case if every char need to be encoded:
        dstrOemModelName.pwszString = rgwchOemModelName;
        dstrOemModelName.cchString = DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH * 6 / SIZEOF( DRM_WCHAR );

        ChkDR( DRM_UTL_XMLEncode( &dstrOemModelNameTemp, FALSE, &dstrOemModelName) );
    }
    else
    {
        ChkDR( dr );
    }

    /* calc size needed for unique ID */
    dr = Oem_Device_GetUniqueId( f_pBlackBoxContext->pOEMContext, DRM_DIT_WMDRMPD_ID, NULL, &cbUniqueID);
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    cchFutureCertSize = f_pdstrWMDRMCertStr->cchString
                              + dstrOemModelName.cchString                                /* size of the model name string */
                              + cbUniqueID / SIZEOF( DRM_WCHAR )                          /* size of B64 encoded unique id */
                              + CCH_BASE64_EQUIV(DRM_ECC160_PUBLIC_KEY_LEN)               /* size of B64 encoded Device Pubkey */
                              + CCH_BASE64_EQUIV(SIZEOF(PKCERT))                          /* size of B64 encoded PKCERT */
                              + ( DRM_SYMOPT_IsSymOptSupported() ?
                                  ( g_dstrAttributePubkey.cchString + 4                   /* pubkey="" plus one space */
                                  + CCH_BASE64_EQUIV( SIZEOF( PUBKEY ) )                  /* size of B64 encoded device public key */
                                  + CCH_BASE64_EQUIV(DRM_SHA1_DIGEST_LEN) )               /* size of B64 encoded symmetric signature of device/data node */
                                : 0 )
                              + CCH_BASE64_EQUIV(DRM_ECC160_SIGNATURE_LEN);               /* size of B64 encoded signature of device/data node */

    if ( cchMaxDevCert < cchFutureCertSize )
    {
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    /*
    ** Generate device unique ID and
    ** fill in uniqueID in "DEVCERT/CERTIFICATE type="DEVICE"/DATA/UNIQUEID"
    */
    ChkDR( DRM_DCP_GetAttribute( NULL,
                                 (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                 DRM_DEVCERT_SERIALNUMBER,
                                 NULL,
                                 &dstrInsert ) );
    ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrWMDRMCertStr,
                                          &dstrInsert ) );
    ChkDR( DRM_UTL_StringInsertBlankSubString(
                f_pdstrWMDRMCertStr,
                (DRM_DWORD)(dstrInsert.pwszString - f_pdstrWMDRMCertStr->pwszString),
                cbUniqueID / SIZEOF( DRM_WCHAR ) ) );

    ChkDR( Oem_Device_GetUniqueId( f_pBlackBoxContext->pOEMContext,
                                   DRM_DIT_WMDRMPD_ID,
                                   (DRM_BYTE*)dstrInsert.pwszString,
                                   &cbUniqueID ) );

    if( dstrOemModelName.cchString > 0 )
    {
        /*
        ** Fill in the model name in the devcert if applicable on this platform
        */
        ChkDR( DRM_DCP_GetAttribute( NULL,
                                     (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                     DRM_DEVCERT_DEVICEMODELNAME,
                                     NULL,
                                     &dstrInsert ) );
        ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrWMDRMCertStr,
                                              &dstrInsert ) );
        ChkDR( DRM_UTL_StringInsertBlankSubString(
                    f_pdstrWMDRMCertStr,
                    (DRM_DWORD)(dstrInsert.pwszString - f_pdstrWMDRMCertStr->pwszString),
                    dstrOemModelName.cchString ) );
        MEMCPY( (DRM_BYTE*)dstrInsert.pwszString,
            (DRM_BYTE*)dstrOemModelName.pwszString,
            dstrOemModelName.cchString * SIZEOF( DRM_WCHAR ) );
    }

    /*
    ** Fill in Pubkey in DEVCERT/CERTIFICATE type="DEVICE"/DATA/PUBLICKEY
    */
    {
        PUBKEY            GCPubkey       = { 0 };       /* parse from GC <DATA>/<PUBLICKEY> */
        DRM_DWORD         cbDestination  = SIZEOF(GCPubkey);
        DRM_DWORD         cbSignature    = DRM_ECC160_SIGNATURE_LEN;

        /*
        ** Locate model-level Public key
        */
        ChkDR( DRM_DCP_GetAttribute( NULL,
                                     (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                     DRM_DEVCERT_GROUPCERTPUBKEY,
                                     NULL,
                                     &dstrInsert ) );
        DRM_B64_DecodeW( &dstrInsert, &cbDestination, (DRM_BYTE*)&GCPubkey, 0 );

        /*
        ** Create the device encryption keys/cert
        */
        ChkDR( _CreateDevPrivateKeyAndCert( f_pBlackBoxContext,
                                            f_poKeyFileContext,
                                            f_pdstrWMDRMCertStr ) );

#if DRM_SUPPORT_DEVICE_SIGNING_KEY
        /*
        ** Create the device signing keys/cert
        */
        ChkDR( _CreateDevSigningKeyAndCert( f_pBlackBoxContext,
                                            f_poKeyFileContext,
                                            f_pdstrWMDRMCertStr ) );
#endif

        /*
        ** Make room and sign the data node with model-level private key
        */
        ChkDR( DRM_XML_GetSubNode( (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                   &g_dstrTagCertificate,
                                   &g_dstrAttributeType,
                                   &g_dstrTagDevice,
                                   0,
                                   NULL,
                                   &dstrCertNodeData,
                                   1 ) );

        ChkDR(DRM_XML_GetNode( &dstrCertNodeData,
                               &g_dstrTagMSDRMSignature,
                               NULL,
                               NULL,
                               0,
                               NULL,
                               &dstrInsert ) );

        ChkDR( DRM_UTL_StringRemoveSubString( f_pdstrWMDRMCertStr, &dstrInsert ) );
        ChkDR( DRM_UTL_StringInsertBlankSubString(
                f_pdstrWMDRMCertStr,
                (DRM_DWORD)(dstrInsert.pwszString - f_pdstrWMDRMCertStr->pwszString),
                CCH_BASE64_EQUIV(DRM_ECC160_SIGNATURE_LEN) ) );
        dstrInsert.cchString = CCH_BASE64_EQUIV(DRM_ECC160_SIGNATURE_LEN);

        /*
        ** Sign the device cert with model-level private key
        */
        ChkDR( DRM_XML_GetNode( &dstrCertNodeData,
                                &g_dstrTagData,
                                NULL,
                                NULL,
                                0,
                                &dstrDataNodeData,
                                NULL ) );

        ChkDR(DRM_BBX_SignWithCertKeys( f_pBlackBoxContext,
                                        DRM_DKT_WMDRMPD_GROUP,
                                        OEM_CRYPTO_HANDLE_INVALID,
                                        CB_DSTR(&dstrDataNodeData),
                                        PB_DSTR(&dstrDataNodeData),
                                        &cbSignature,
                                        PB_DSTR(&dstrInsert)));


        /* B64 encode the signature in place */
        ChkDR(DRM_B64_EncodeW(
                PB_DSTR(&dstrInsert),
                cbSignature,
                (DRM_WCHAR*)dstrInsert.pwszString,
                &dstrInsert.cchString,
                0));

#if !DRM_SUPPORT_KEYFILE
        if( DRM_SYMOPT_IsSymOptSupported() )
        {
            /*
            ** Create the symmetric signature for the device to use
            */
            ChkDR( DRM_XML_GetSubNode( (DRM_CONST_STRING*)f_pdstrWMDRMCertStr,
                                       &g_dstrTagCertificate,
                                       &g_dstrAttributeType,
                                       &g_dstrTagDevice,
                                        0,
                                        NULL,
                                       &dstrCertNodeData,
                                       1 ) );


            ChkDR(DRM_XML_GetNode( &dstrCertNodeData,
                                   &g_dstrTagSymSig,
                                    NULL,
                                    NULL,
                                    0,
                                   &dstrInsert,
                                    NULL));

            ChkDR(DRM_UTL_StringRemoveSubString(f_pdstrWMDRMCertStr, &dstrInsert));

            ChkDR(DRM_UTL_StringInsertBlankSubString( f_pdstrWMDRMCertStr,
                                          (DRM_DWORD)(dstrInsert.pwszString - f_pdstrWMDRMCertStr->pwszString),
                                                     CCH_SYMSIG_TAG ) );

            ChkDR( Oem_Device_GetKey( f_pBlackBoxContext->pOEMContext,
                                      DRM_DKT_WMDRMPD_GROUP,
                                      (DRM_BYTE *)&GCPrivKey,
                                      &cbGCPrivKey ) );

            ChkDR( DRM_HMAC_CreateMAC( eDRM_SHA_1,
                                       (DRM_BYTE*)&GCPrivKey,
                                       SIZEOF( GCPrivKey ),
                                       PB_DSTR(&dstrDataNodeData),
                                       0,
                                       CB_DSTR(&dstrDataNodeData),
                                       rgbSymmSig,
                                       0,
                                       DRM_SHA1_DIGEST_LEN ) );


            ChkDR( DRM_UTL_BuildXMLSymmSignature( &GCPubkey,
                                                  rgbSymmSig,
                                                  ( DRM_WCHAR * )dstrInsert.pwszString,
                                                  CCH_SYMSIG_TAG,
                                                  NULL ) );
        }
#endif  /* !DRM_SUPPORT_KEYFILE */
    }
    ChkOverflow( cchMaxDevCert, f_pdstrWMDRMCertStr->cchString );

    ChkDR( DRM_KF_SetCertificate ( f_poKeyFileContext,
                                   eKF_CERT_TYPE_WMDRM,
                                   PB_DSTR( f_pdstrWMDRMCertStr ),
                                   CB_DSTR( f_pdstrWMDRMCertStr ) ) );

ErrorExit:
    OEM_ECC_ZeroPrivateKey_P160( &GCPrivKey );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_CreateXMLDeviceCertificateAndKey(
    __in_opt       DRM_VOID                         *f_pOEMContext,
    __in           DRM_KEYFILE_CONTEXT              *f_poKeyFileContext,
    __in           DRM_DEVICE_CERT_PARSER_CACHE     *f_poParserCache,
    __in           DRM_BB_CONTEXT                   *f_pBlackBoxContext,
    __inout        DRM_STRING                       *f_pdstrWMDRMCertStr )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poKeyFileContext  != NULL );
    ChkArg( f_poParserCache     != NULL );
    ChkArg( f_pBlackBoxContext  != NULL );
    ChkArg( f_pdstrWMDRMCertStr != NULL );

    ChkDR( DRM_DDC_SetXMLDeviceCertificateAndKey( f_pBlackBoxContext,
                                                  f_poKeyFileContext,
                                                  f_pdstrWMDRMCertStr ) );

    ChkDR( DRM_DCP_StartCertParserCache( ( const DRM_CONST_STRING * )f_pdstrWMDRMCertStr,
                                         f_poKeyFileContext,
                                         f_poParserCache ) );

    ChkDR( DRM_DDC_LoadXMLCertPropertiesCache( f_pOEMContext,
                                               f_poParserCache,
                                               f_pBlackBoxContext,
                                               f_poKeyFileContext,
                                               (DRM_CONST_STRING*)f_pdstrWMDRMCertStr ) );

    ChkDR( DRM_DCP_StopCertParserCache( f_poKeyFileContext,
                                        f_poParserCache ) );

ErrorExit:
    return dr;
}

/**********************************************************************************
** Function:    DRM_DDC_SetBinaryDeviceCertificateAndKey
** Synopsis:    Creates new binary device certificate and adds it to keyfile
** Arguments:   [f_pOEMContext]  -- A pointer to the OEM Context; may be NULL
**              [f_pBinaryDevCertChain] -- A pointer to a buffer to contain the
**                                  binary device certificate
**              [f_pcbBinaryDevCertChain] -- Size in bytes of the binary device certificate
**              [f_pBlackBoxContext] -- A pointer to the blackbox context
** Returns:     DRM_SUCCESS on success
** Notes:
************************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DDC_SetBinaryDeviceCertificateAndKey(
    __in_opt                               DRM_VOID             *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT  *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE             *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD            *f_pcbBinaryDevCertChain,
    __inout                                DRM_BB_CONTEXT       *f_pBlackBoxContext )
{
    DRM_RESULT                       dr                 = DRM_SUCCESS;
    DRM_DWORD                        cbBuffer          = 0;
    DRM_BYTE                        *pbModelCertChain  = NULL;
    DRM_DWORD                        cbModelCertChain  = 0;
    DRM_DWORD                        cbDeviceCert      = 0;
    DRM_DWORD                        cbUniqueID        = DRM_BCERT_CLIENT_ID_LENGTH;
    OEM_CRYPTO_HANDLE                hPrivKey          = OEM_CRYPTO_HANDLE_INVALID;
    DRM_GUID                         guidNull          = EMPTY_DRM_GUID;
    PUBKEY_P256                      IssuerKey         = {0};
    DRM_BCERT_CERT_ID                CertificateID     = {0};
    DRM_BCERT_CLIENT_ID              SerialNum         = {0};
    DRM_BCERT_BUILDER_DATA           BuilderData       = {0};
    DRM_DWORD                        cchModelInfo      = DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF( DRM_WCHAR )+1;
    DRM_WCHAR                        rgwchOemModelInfo[(DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF( DRM_WCHAR ))+1] = {0};
    DRM_BCERT_MANUFACTURER_STRINGS   ManufacturerStrings = {0};
    DRM_DWORD                        rgdwFeatureSet[MAX_WMDRMMD_FEATURE_FLAGS] = {0};
    DRM_BCERT_BUILDER_CERTKEY        oCertKeys[DRM_BINARY_DEVICE_CERT_MAX_KEYUSAGES] = {0};
    OEM_CRYPTO_HANDLE                PrivKeys[DRM_BINARY_DEVICE_CERT_MAX_KEYUSAGES]   = {OEM_CRYPTO_HANDLE_INVALID};
    DRM_DWORD                        idx                  = 0;
    DRM_DWORD                        cbPubKey             = ECC_P256_POINT_SIZE_IN_BYTES;
    DRM_DWORD                        rgdwKeyUsages[]      = { DRM_BCERT_KEYUSAGE_SIGN, DRM_BCERT_KEYUSAGE_ENCRYPT_KEY, DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY };
    DRMFILETIME                      ftUnlimited          = {0};
    DRM_BCERT_VERIFICATIONCONTEXT    oVerificationContext = {0};
    DRM_ENCRYPTED_KEY                oEncryptedKey        = { 0 };
    DRM_BYTE                         rgbScratch[KF_MAX_ENCRYPTION_BUFFER] = {0};
    DRM_DWORD                        cTotalKeysInFinalCert  = (DRM_DWORD)( DRM_PRND_IsPRNDReceiverSupported() ? 3 : 2 );

    /*
    ** Load the group/model certificate (a.k.a. the parent chain) into the bottom
    ** half of the buffer provided.
    */
    ChkArg( f_pBinaryDevCertChain != NULL );
    ChkArg( f_pcbBinaryDevCertChain != NULL );

    cbBuffer  = *f_pcbBinaryDevCertChain;
    cbModelCertChain = cbBuffer - DWORD_ALIGNED_SIZE( cbBuffer / 2 );
    if ( cbBuffer < cbModelCertChain ) /* overflow check */
    {
        return DRM_E_BUFFERTOOSMALL;
    }
    pbModelCertChain = f_pBinaryDevCertChain + DWORD_ALIGNED_SIZE( cbBuffer / 2 );


    /*
    ** Build a new certificate chain: add device cert to model cert chain and sign
    */
    dr = Oem_Device_GetCert( f_pOEMContext,
                             DRM_DCT_PLAYREADY_TEMPLATE,
                             pbModelCertChain,
                             &cbModelCertChain );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( DRM_E_INVALID_DEVCERT_TEMPLATE );
    }
    else if( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_E_DEVCERT_READ_ERROR );
    }

    /*
    ** Generate a unique Identifier for this certificate.
    */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext, CertificateID.rgb, DRM_BCERT_CERT_ID_LENGTH ) );

    /*
    ** Generate an ECC P256 key pairs and store them in keyfile
    */

    /*
    **  The first of the calls is for signing key
    */
    ChkDR( DRM_BBX_GenerateECCKeyPair( f_pBlackBoxContext,
                                       OEM_HAL_KEY_TYPE_ECC_P256_SIGN,
                                       &cbPubKey,
                                       oCertKeys[SIGNING_KEY_IDX].KeyValue.m_rgbPubkey,
                                       &PrivKeys[SIGNING_KEY_IDX] ) );

    cbPubKey  = ECC_P256_POINT_SIZE_IN_BYTES;

    /*
    **  Second the encryption/decryption keypair
    */
    ChkDR( DRM_BBX_GenerateECCKeyPair( f_pBlackBoxContext,
                                       OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT,
                                       &cbPubKey,
                                       oCertKeys[ENCRYPTION_KEY_IDX].KeyValue.m_rgbPubkey,
                                       &PrivKeys[ENCRYPTION_KEY_IDX] ) );

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        cbPubKey  = ECC_P256_POINT_SIZE_IN_BYTES;

        /*
        **  Finally the PlayReady massage protect keypair
        */
        ChkDR( DRM_BBX_GenerateECCKeyPair( f_pBlackBoxContext,
                                           OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT,
                                           &cbPubKey,
                                           oCertKeys[PRND_ENCRYPTION_KEY_IDX].KeyValue.m_rgbPubkey,
                                           &PrivKeys[PRND_ENCRYPTION_KEY_IDX] ) );
    }

    /*
    ** copy to scratch buffer which is more than big enough
    */
    oEncryptedKey.pbEncryptedKey = rgbScratch;
    oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

    ChkDR( DRM_BBX_KF_EncryptKey ( f_pBlackBoxContext,
                                   PrivKeys[SIGNING_KEY_IDX],
                                   &oEncryptedKey ) );

    ChkDR( DRM_KF_SetPrivateKey ( f_poKeyFileContext,
                                  eKF_KEY_TYPE_ECC_256,
                                  oCertKeys[SIGNING_KEY_IDX].KeyValue.m_rgbPubkey,
                                  DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH,
                                  &oEncryptedKey ) );

    oEncryptedKey.pbEncryptedKey = rgbScratch;
    oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

    ChkDR( DRM_BBX_KF_EncryptKey ( f_pBlackBoxContext,
                                   PrivKeys[ENCRYPTION_KEY_IDX],
                                   &oEncryptedKey ) );

    ChkDR( DRM_KF_SetPrivateKey ( f_poKeyFileContext,
                                  eKF_KEY_TYPE_ECC_256,
                                  oCertKeys[ENCRYPTION_KEY_IDX].KeyValue.m_rgbPubkey,
                                  DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH,
                                  &oEncryptedKey ) );

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        oEncryptedKey.pbEncryptedKey = rgbScratch;
        oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

        ChkDR( DRM_BBX_KF_EncryptKey ( f_pBlackBoxContext,
                                       PrivKeys[PRND_ENCRYPTION_KEY_IDX],
                                       &oEncryptedKey ) );

        ChkDR( DRM_KF_SetPrivateKey ( f_poKeyFileContext,
                                      eKF_KEY_TYPE_ECC_256,
                                      oCertKeys[PRND_ENCRYPTION_KEY_IDX].KeyValue.m_rgbPubkey,
                                      DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH,
                                      &oEncryptedKey ) );
    }

    /*
    ** Get the device's serial number.
    ** Get the "issuer key" (the group/model cert's public key).
    ** Get the private key with which to sign this certificate (the group/model cert's private key).
    */
    ChkDR( Oem_Device_GetUniqueId( f_pOEMContext,
                                   DRM_DIT_PLAYREADY_ID,
                                   SerialNum.rgb,
                                   &cbUniqueID ) );
    ChkDR( DRM_BCert_GetPublicKey( pbModelCertChain,
                                   cbModelCertChain,
                                   0,
                                   &IssuerKey ) );

    ChkDR( DRM_BBX_GetPreloadedKey( f_pBlackBoxContext,
                                    OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN,
                                    &guidNull,
                                    &hPrivKey ) );

    /*
    ** Fill-in the input data structure used by the binary certificate builder
    */
    BCert_SetDefaultBuilderData( &CertificateID,
                                 DRM_BCERT_CERTTYPE_DEVICE,
                                 NULL,
                                 hPrivKey,
                                 &IssuerKey,
                                 &BuilderData );

    /*
    ** Fill-in the manufacturer strings
    */
    dr = Oem_Device_GetModelInfo(f_pOEMContext, rgwchOemModelInfo, &cchModelInfo);
    if( dr != DRM_E_NOTIMPL )
    {
        ChkDR( dr );
        ChkBOOL(cchModelInfo <= DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH / SIZEOF( DRM_WCHAR ), DRM_E_BCERT_MANUFACTURER_STRING_TOO_LONG);

        MEMCPY( ManufacturerStrings.ModelName.rgb, rgwchOemModelInfo, cchModelInfo * SIZEOF(DRM_WCHAR) );

        ManufacturerStrings.ModelName.cb = cchModelInfo * SIZEOF( DRM_WCHAR );
        BuilderData.pManufacturerStrings = &ManufacturerStrings;
    }
    else
    {
        /*
        ** If Oem_Device_GetModelInfo returns DRM_E_NOTIMPL, try to propagate
        ** the manufacturer strings from the parent certificate, if any.
        */
        ChkDR( DRM_BCert_GetManufacturerStrings( pbModelCertChain,
                                                 cbModelCertChain,
                                                 &ManufacturerStrings ) );

        if ( dr == DRM_SUCCESS )
        {
            BuilderData.pManufacturerStrings = &ManufacturerStrings;
        }
    }

    oCertKeys[SIGNING_KEY_IDX].dwNumKeyUsages = 1;
    oCertKeys[SIGNING_KEY_IDX].pKeyUsageSet = (DRM_DWORD*)&rgdwKeyUsages[SIGNING_KEY_IDX];
    oCertKeys[ENCRYPTION_KEY_IDX].dwNumKeyUsages = 1;
    oCertKeys[ENCRYPTION_KEY_IDX].pKeyUsageSet = (DRM_DWORD*)&rgdwKeyUsages[ENCRYPTION_KEY_IDX];

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        oCertKeys[PRND_ENCRYPTION_KEY_IDX].dwNumKeyUsages = 1;
        oCertKeys[PRND_ENCRYPTION_KEY_IDX].pKeyUsageSet = (DRM_DWORD*)&rgdwKeyUsages[PRND_ENCRYPTION_KEY_IDX];
    }

    BCert_SetKeyInfoBuilderData( (DRM_BCERT_BUILDER_CERTKEY*)oCertKeys, cTotalKeysInFinalCert, &BuilderData );

    BCert_SetDeviceBuilderData( &SerialNum, &BuilderData );

    /*
    ** Set the feature flags in the binary cert builder
    */
    BuilderData.dwNumFeatureEntries = 0;
    BuilderData.prgdwFeatureSet = rgdwFeatureSet;

    if( DRM_CLK_IsSecureClockSupported() )
    {
        rgdwFeatureSet[BuilderData.dwNumFeatureEntries++] = DRM_BCERT_FEATURE_SECURE_CLOCK;
    }
    else if( DRM_ARCLK_IsAntirollbackClockSupported() )
    {
        rgdwFeatureSet[BuilderData.dwNumFeatureEntries++] = DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK;
    }

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        rgdwFeatureSet[BuilderData.dwNumFeatureEntries++] = DRM_BCERT_FEATURE_SUPPORTS_CRLS;
    }

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        rgdwFeatureSet[BuilderData.dwNumFeatureEntries++] = DRM_BCERT_FEATURE_RECEIVER;
    }

    if( DRM_PRND_IsPRNDTransmitterSupported() )
    {
        rgdwFeatureSet[BuilderData.dwNumFeatureEntries++] = DRM_BCERT_FEATURE_TRANSMITTER;
    }

    /*
    ** Use the security level from the group/model certificate in this leaf certificate
    */
    DRM_BCert_GetLeafSecurityLevel( BuilderData.dwSecurityLevel, pbModelCertChain );

    /*
    ** use the security version from the parent
    */
    ChkDR( DRM_BCert_GetSecurityVersion( pbModelCertChain,
                                         cbModelCertChain,
                                        &BuilderData.SecurityVersion.dwSecurityVersion,
                                        &BuilderData.SecurityVersion.dwPlatformIdentifier ) );

    /*
    ** dr is DRM_S_FALSE if the cert has no SecurityVersion to include.
    ** dr is DRM_SUCCESS if the cert has a SecurityVersion to include.
    */
    BuilderData.SecurityVersion.fValid = ( dr == DRM_SUCCESS );

    cbDeviceCert = cbBuffer - cbModelCertChain;
    ChkDR( BCert_AddCert( pbModelCertChain, f_pBinaryDevCertChain, &cbDeviceCert, &BuilderData, f_pBlackBoxContext, NULL, f_pOEMContext ) );
    *f_pcbBinaryDevCertChain = cbDeviceCert;

    /*
    ** Verify the newly built device certificate chain, then save it on the device.
    */
    /*
    ** Initialize verification context: full verification.
    ** We only accept certs that don't expire (i.e. expire at 0xFFFFFFFF).
    ** Device cert key must have Sign and Encrypt key usages
    */
    CREATE_FILE_TIME( MAX_UNSIGNED_TYPE( DRM_DWORD ), ftUnlimited );

    ChkDR( DRM_BCert_InitVerificationContext( &ftUnlimited,
                                              (const PUBKEY_P256*) g_rgbMSPlayReadyRootIssuerPubKey,
                                              DRM_BCERT_CERTTYPE_DEVICE,
                                              &f_pBlackBoxContext->CryptoContext,
                                              DRM_DCP_VERIFY_DEVCERT, /* TASK: see if we can optimize verification here */
                                              FALSE,
                                              rgdwKeyUsages,
                                              cTotalKeysInFinalCert,
                                              FALSE,
                                              NULL, /* Cert caching parameters NULL as we generally don't verify signatures on device certs so optimization not needed here */
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );
    ChkDR( DRM_BCert_ParseCertificateChain( f_pBinaryDevCertChain,
                                            cbDeviceCert,
                                            &oVerificationContext ) );

    /*
    ** See if robustness version and platform ID of the cert
    ** matches same data in PlayReady.dll
    */
    ChkDR( DRM_DDC_MatchRobustnessPlatformAndModel( f_pOEMContext,
                                                    f_pBinaryDevCertChain,
                                                    cbDeviceCert,
                                                    TRUE ) );

    ChkDR( DRM_KF_SetCertificate ( f_poKeyFileContext,
                                   eKF_CERT_TYPE_PLAYREADY,
                                   f_pBinaryDevCertChain,
                                   cbDeviceCert ) );

ErrorExit:

    DRM_BBX_SafeReleaseKey( f_pBlackBoxContext, &hPrivKey );

    for( idx = 0; idx < DRM_BINARY_DEVICE_CERT_MAX_KEYUSAGES; idx++ )
    {
        DRM_BBX_SafeReleaseKey( f_pBlackBoxContext, &PrivKeys[idx] );
    }
    OEM_SECURE_ZERO_MEMORY( rgbScratch, SIZEOF(rgbScratch) );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_CreateBinaryDeviceCertificateAndKey(
    __in_opt                               DRM_VOID             *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT  *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE             *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD            *f_pcbBinaryDevCertChain,
    __in                                   DRM_BB_CONTEXT       *f_pBlackBoxContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poKeyFileContext      != NULL );
    ChkArg( f_pBinaryDevCertChain   != NULL );
    ChkArg( f_pcbBinaryDevCertChain != NULL );
    ChkArg( f_pBlackBoxContext      != NULL );

    ChkDR( DRM_DDC_SetBinaryDeviceCertificateAndKey( f_pOEMContext,
                                                     f_poKeyFileContext,
                                                     f_pBinaryDevCertChain,
                                                     f_pcbBinaryDevCertChain,
                                                     f_pBlackBoxContext ) );

    ChkDR( DRM_DDC_LoadBinaryCertPropertiesCache( f_pOEMContext,
                                                  f_pBlackBoxContext,
                                                  f_pBinaryDevCertChain,
                                                  *f_pcbBinaryDevCertChain ) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_DDC_CreateSecureStoreKeyAndSaveKeyFile(
    __in_opt            DRM_VOID       *f_pOEMContext,
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBlackBoxContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pBlackBoxContext != NULL );

    ChkDR( DRM_BBX_SetupSecureStoreKey( f_pBlackBoxContext, f_pOEMContext ) );
    ChkDR( DRM_KF_StoreToFile( f_pBlackBoxContext->pKeyFileContext ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

