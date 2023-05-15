/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "stdafx.h"
#include "oemcipher.h"
#include "drmblackbox.h"
#include "drmlicense.h"
#include "blackboxtest.h"

ENTER_PKTEST_NAMESPACE_CODE

static DRM_RESULT ArgToBindingInfo( __in_z_opt char *arg, DRM_BINDING_INFO **ppB)
{
    int i;
    if (arg) {
        i = atoi(arg);
        if (i < 0 || i >= MAX_GLOBAL_BINDING_BLOB)
            return DRM_E_INVALIDARG;
        *ppB = &(g_binding[i]);
    } else
        *ppB = NULL;
    return DRM_SUCCESS;
}

/* Calculate the checksum */
static void SetCheckSum(DRM_BYTE *pData, DRM_DWORD cbData, DRM_BYTE pbCheckSum[CHECKSUM_LENGTH])
{
    DRM_BYTE res[DRM_SHA1_DIGEST_LEN+1];
    DRM_SHA_CONTEXT shaVal = DRM_SHA_CONTEXT_DEFAULT;
    DRM_LONG lCount;

    /* Run SHA on key SHA_ITERATIONS times. */
    /* First copy key into res. */
    if ( cbData > DRM_SHA1_DIGEST_LEN+1 )
        cbData = DRM_SHA1_DIGEST_LEN+1; /* Make sure we don't exceed buffer capacity in res. */

    memset( res, 0, sizeof(res) ); /* Init to 0. */
    memcpy( res, pData, cbData );   /* Copy key. */

    for ( lCount = 0; lCount < SHA_ITERATIONS; lCount++) {
        DRM_SHA_Init( &shaVal, eDRM_SHA_1 );
        DRM_SHA_Update( res, DRM_SHA1_DIGEST_LEN+1,&shaVal );
        DRM_SHA_Finalize( &shaVal, DRM_SHA1_DIGEST_LEN, res );
    }
    memcpy(pbCheckSum, res, CHECKSUM_LENGTH);
}

/* Return the blackbox pubkey to encrypt the content key. */
static DRM_RESULT GetBBPubKey(PUBKEY* pPubKey)
{
    memcpy(pPubKey, &(g_BBContext.cachedCertValues.m_BBCompatibilityCert.pk.pk), sizeof(PUBKEY));
    return DRM_SUCCESS;
}

/*
    Create internal BindingInfo structure for other api
    argv[0]: index to the global BindingInfo to create
    argv[1]: modifications to the BindingInfo:
        BAD_CHECKSUM, BAD_SIGNATURE, BAD_CONTENTKEY, BAD_LSPUBKEY, WRONG_LSPUBKEY, BAD_BBPUBKEY, KEY_LENGTH #
    argv[2]: License Type (XML or XMR, defaults to XML if not given)
*/
DRM_RESULT DRM_CALL Test_CreateBindingInfo(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_BINDING_INFO *pBindingInfo;
    DRM_BYTE keyLength, ClearContentKey[__CB_DECL( DRM_ECC160_PLAINTEXT_LEN )];
    PRIVKEY lsPrvKey; /* license server private key to sign the content key */
    DRM_DWORD         cbPubKey    = DRM_ECC160_PUBLIC_KEY_LEN;
    DRM_DWORD         cbPrivKey   = DRM_ECC160_PRIVATE_KEY_LEN;
    DRM_DWORD         cbEncrypted = DRM_ECC160_CIPHERTEXT_LEN;
    DRM_DWORD         cbSignature = DRM_ECC160_SIGNATURE_LEN;

    ChkArg(argc >= 2);
    ChkArg( argv[1] != NULL );
    ChkDR(ArgToBindingInfo(argv[0], &pBindingInfo));
    ChkArg(pBindingInfo);

    memset(pBindingInfo, 0, sizeof(DRM_BINDING_INFO));

    /* Generate the random content key. The first byte is the key length.
        The actual key starts from the second byte. */
    ChkDR(Oem_Random_GetBytes(NULL, ClearContentKey, DRM_ECC160_PLAINTEXT_LEN));

    /* Set the content key length */
    if (!strncmp(argv[1], "KEY_LENGTH", 10))
        keyLength = (DRM_BYTE)atoi(argv[1] + 10);
    else
        keyLength = (DRM_BYTE)DRMCIPHERKEYLEN_RC4; /* drmcipher limits the key length to DRMCIPHERKEYLEN */
    ClearContentKey[0] = keyLength;

    /* Set the algorithm to ECC for now. Need to test chained licenses later using DES */
    pBindingInfo->m_fXMRLicense = FALSE;
    pBindingInfo->m_BindingInfo.m_XML.m_dwAlgorithm = eMSDRM_PK;

    /* Copy the blackbox public key to do the encryption. This is the pubkey sent up to the license server. */
    ChkDR(GetBBPubKey(&pBindingInfo->m_BindingInfo.m_XML.m_oPublKey));

    /* Encrypt the content key with the blackbox public key */
    ChkDR( DRM_PK_Encrypt( NULL,
                           g_BBContext.CryptoContext.rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PUBLIC_KEY_LEN,
                           ( DRM_BYTE * )&pBindingInfo->m_BindingInfo.m_XML.m_oPublKey,
                           DRM_ECC160_PLAINTEXT_LEN,
                           ClearContentKey,
                           &cbEncrypted,
                           pBindingInfo->m_BindingInfo.m_XML.m_rgbContentKey ) );


    /* Calculate the content key's checksum */
    SetCheckSum(ClearContentKey + 1, keyLength, pBindingInfo->m_oChecksum.m_rgbChecksumData );

    /* Generate the license server pub/private keys */
    ChkDR(DRM_PK_GenKeyPair(g_BBContext.CryptoContext.rgbCryptoContext,
                            eDRM_ECC_P160,
                            eDRM_RSA_UNKNOWN,
                            &cbPubKey,
                            ( DRM_BYTE * )&pBindingInfo->m_BindingInfo.m_XML.m_oLsPublKey,
                            &cbPrivKey,
                            ( DRM_BYTE * )&lsPrvKey));

    /* Sign the content key with the license server private key */
    if (keyLength)
    {
        ChkDR(DRM_PK_Sign(g_BBContext.CryptoContext.rgbCryptoContext,
                          eDRM_ECC_P160,
                          DRM_ECC160_PRIVATE_KEY_LEN,
                          ( const DRM_BYTE * )&lsPrvKey,
                          keyLength,
                          ClearContentKey + 1,
                          &cbSignature,
                          pBindingInfo->m_BindingInfo.m_XML.m_rgbSignature));
    }

    /* We have a valid binding info blob now. */

    /* Intentionally modify the binding info for other tests... */
    if (!strcmp(argv[1], "BAD_CHECKSUM")) /* flip a byte in the checksum */
        pBindingInfo->m_oChecksum.m_rgbChecksumData[1] ^= 0xff;

    else if (!strcmp(argv[1], "BAD_SIGNATURE")) /* flip a byte in the signatrue */
        pBindingInfo->m_BindingInfo.m_XML.m_rgbSignature[2] ^= 0xff;

    else if (!strcmp(argv[1], "BAD_CONTENTKEY")) /* flip a byte in the encrypted content key */
        pBindingInfo->m_BindingInfo.m_XML.m_rgbContentKey[1] ^= 0xff;

    else if (!strcmp(argv[1], "BAD_LSPUBKEY")) /* flip a byte in the license server pub key */
        pBindingInfo->m_BindingInfo.m_XML.m_oLsPublKey.y[2] ^= 0xff;

    else if (!strcmp(argv[1], "BAD_BBPUBKEY")) /* flip a byte in the blackbox pub key */
        pBindingInfo->m_BindingInfo.m_XML.m_oPublKey.y[2] ^= 0xff;

    else if (!strcmp(argv[1], "WRONG_LSPUBKEY")) { /* license server pubkey is different. Someone tries to re-sign the license */
        cbPubKey = DRM_ECC160_PUBLIC_KEY_LEN;
        cbPrivKey = DRM_ECC160_PRIVATE_KEY_LEN;

        ChkDR(DRM_PK_GenKeyPair(g_BBContext.CryptoContext.rgbCryptoContext,
                                eDRM_ECC_P160,
                                eDRM_RSA_UNKNOWN,
                                &cbPubKey,
                                ( DRM_BYTE * )&pBindingInfo->m_BindingInfo.m_XML.m_oLsPublKey,
                                &cbPrivKey,
                                ( DRM_BYTE * )&lsPrvKey));
    }

ErrorExit:
    return dr;
}

/*
    Test API DRM_BBX_CanBind
    argv[0]: index to the global BindingInfo created by Test_CreateBindingInfo
    argv[1]: blackbox context: NULL, NORMAL
*/
DRM_RESULT DRM_CALL Test_API_CanBind(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr;
    DRM_BINDING_INFO *pBindingInfo;
    DRM_BB_CONTEXT *pBBContext;

    ChkArg(argc == 2);
    ChkDR(ArgToBindingInfo(argv[0], &pBindingInfo));
    pBBContext = argv[1]? &g_BBContext: NULL;

    DRM_BBX_CanBind(pBindingInfo, 1, pBBContext, &dr);
ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE
