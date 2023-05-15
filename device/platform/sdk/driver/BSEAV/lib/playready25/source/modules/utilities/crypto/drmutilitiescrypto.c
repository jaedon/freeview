/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMUTILITIESCRYPTO_C
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmxmlparser.h>
#include <oemrc4.h>
#include <byteorder.h>
#include <oem.h>
#include <drmcertcache.h>
#include <drmblobcache.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function: DRM_UTL_IsCertDateOK
**
** Synopsis: test whether a given date is before todays date.
**
** Arguments:
**   [f_pbDate]     -- date as a byte array
**   f_pbData [0] -- century, e.g. 20
**   f_pbData [1] -- year mod 100 e.g. 4
**   f_pbData [2] -- month, 1-based
**   f_pbData [3] -- day-of-month, 1-based
**
** Example: March 21, 2004 = |20|4|3|21|
**                            0  1 2 3
** Returns TRUE if the date is reasonable
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_UTL_IsCertDateOK(
    __in_opt    DRM_VOID        *pOEMContext,
    __in_bcount( 4 ) const DRM_BYTE  *f_pbDate)
{
    DRMSYSTEMTIME systimeCurrent;
    DRM_BOOL      fOK = FALSE;
    DRM_BYTE      rgbDate[4]; /* Don't use __CB_DECL here */

    Oem_Clock_GetSystemTime(pOEMContext, &systimeCurrent);

    rgbDate[0] = GET_BYTE( f_pbDate, 0 );
    rgbDate[1] = GET_BYTE( f_pbDate, 1 );
    rgbDate[2] = GET_BYTE( f_pbDate, 2 );
    rgbDate[3] = GET_BYTE( f_pbDate, 3 );

    if ((rgbDate [0] * 100 + rgbDate [1]) > systimeCurrent.wYear)
    {
        fOK = TRUE;
    }
    else if ((rgbDate [0] * 100 + rgbDate [1]) < systimeCurrent.wYear)
    {
        fOK = FALSE;
    }
    else if (rgbDate [2] > systimeCurrent.wMonth)
    {
        fOK = TRUE;
    }
    else if (rgbDate [2] < systimeCurrent.wMonth)
    {
        fOK = FALSE;
    }
    else
    {
        fOK = (rgbDate [3] >= systimeCurrent.wDay);
    }

    return fOK;
}

/******************************************************************************
**
** Function :   _UTL_VerifySignature
**
** Synopsis :   Verify the xml signature over some data
**
** Arguments :  f_pdstrSignedData   -   The entire data that is signed
**              f_pdstrSignatureValue -   Sig value
**              f_pdstrCertChainXML -   <CERTIFICATECHAIN>...</CERTIFICATECHAIN>
**              f_fCheckExpiry      -   Check for cert expiration
**              f_fCheckCertChain   -   Verify cert chain. If this is false,
**                                      f_pcontextCrypto->pubKey must contain
**                                      the public key which will be used to
**                                      verify the signature.
**              f_eRootPubkey       -   Root Pubkey to use for cert verification
**              f_pcontextLEVL      -   License eval context.  The DST, pcontextSSTRevocation,
**                                      and BBX pointers must be valid
**
** Returns :    DRM_SUCCESS -   Signature verified successfully
**              DRM_E_INVALID_SIGNATURE -   Verification failed
**              some other problem
**
** Notes :
**
******************************************************************************/
static DRM_RESULT _UTL_VerifySignature(
    __in     const   DRM_CONST_STRING        *f_pdstrSignedData,
    __in     const   DRM_CONST_STRING        *f_pdstrSignatureValue,
    __in_opt const   DRM_CONST_STRING        *f_pdstrCertChainXML,
    __in             DRM_BOOL                 f_fCheckExpiry,
    __in             DRM_BOOL                 f_fCheckCertChain,
    __in             DRM_ROOTPUBKEY_CATEGORY  f_eRootPubkey,
    __in             DRM_LICEVAL_CONTEXT     *f_pcontextLEVL )
{
    DRM_BOOL         fFirstCertCheck =   TRUE;
    DRM_CONST_STRING dstrCert        =   EMPTY_DRM_STRING;
    DRM_DWORD        cbSignature     =   0;
    DRM_DWORD        iCert           =   0;
    DRM_RESULT       dr              =   DRM_SUCCESS;

    /*
    **  No need to verify input in internal function
    */

    if( DRM_CERTCACHE_IsCertCacheSupported() )
    {
        /*
        ** DRM_BLOBCACHE_Verify is smart enough to succeed setting fVerified to FALSE
        ** if any of the first three parameters (contexts) are NULL
        */
        DRM_BOOL fVerified = FALSE;

        ChkDR( DRM_BLOBCACHE_Verify( f_pcontextLEVL->pcontextBBX,
                                     f_pcontextLEVL->pcontextSSTRevocation,
                                     f_pcontextLEVL->pDatastore,
                                     PB_DSTR( f_pdstrSignedData ),
                                     CB_DSTR( f_pdstrSignedData ),
                                     PB_DSTR( f_pdstrSignatureValue ),
                                     CB_DSTR( f_pdstrSignatureValue ),
                                     &fVerified ) );

        if( fVerified )
        {
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
    }

    if (f_fCheckCertChain)
    {
        ChkDRMString(f_pdstrCertChainXML);
        /*
        **  Verify cert chain
        */
        while( TRUE ) /*    Loop will break when DRM_E_XMLNOTFOUND is hit */
        {
            DRM_DWORD cbCert = SIZEOF(CERT);

            dr  =   DRM_XML_GetSubNode( f_pdstrCertChainXML,
                                       &g_dstrTagCertificate,
                                        NULL,
                                        NULL,
                                        iCert,
                                        NULL,
                                       &dstrCert,
                                        1);
            iCert++;

            if (dr == DRM_E_XMLNOTFOUND)
            {
                break;
            }
            ChkDR(dr);

            ChkDR(DRM_B64_DecodeW( &dstrCert,
                                   &cbCert,
                       (DRM_BYTE*) &f_pcontextLEVL->pcontextBBX->CryptoContext.union_cert.cert,
                                    0) );

            ChkDR(  DRM_UTL_CheckCertificate( &f_pcontextLEVL->pcontextBBX->CryptoContext.union_cert.cert,
                                               fFirstCertCheck ? NULL : &f_pcontextLEVL->pcontextBBX->CryptoContext.pubKey,
                                               f_fCheckExpiry,
                                               f_pcontextLEVL,
                                               f_eRootPubkey) );

            MEMCPY( &(f_pcontextLEVL->pcontextBBX->CryptoContext.pubKey),
                    &(f_pcontextLEVL->pcontextBBX->CryptoContext.union_cert.cert.cd.pk),
                      SIZEOF(PUBKEY) );

            fFirstCertCheck = FALSE;
        }

        if( iCert == 1 )
        {
            dr = DRM_E_INVALID_SIGNATURE;   /*  No certificates */
            goto ErrorExit;
        }
        /*
        **  f_pcontextCrypto->pubKey now contains the pubkey needed for verification
        */
    }

    cbSignature = SIZEOF(f_pcontextLEVL->pcontextBBX->CryptoContext.signature);
    ChkDR(DRM_B64_DecodeW( f_pdstrSignatureValue,
                          &cbSignature,
                           f_pcontextLEVL->pcontextBBX->CryptoContext.signature,
                           0) );

    ChkDR( DRM_PK_Verify( f_pcontextLEVL->pcontextBBX->CryptoContext.rgbCryptoContext,
                          eDRM_ECC_P160,
                          DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE * )&f_pcontextLEVL->pcontextBBX->CryptoContext.pubKey,
                          CB_DSTR(f_pdstrSignedData),
                          PB_DSTR(f_pdstrSignedData),
                          DRM_ECC160_SIGNATURE_LEN,
                          f_pcontextLEVL->pcontextBBX->CryptoContext.signature) );

    if( DRM_CERTCACHE_IsCertCacheSupported() )
    {
        if( f_pcontextLEVL->pcontextBBX           != NULL
         && f_pcontextLEVL->pcontextSSTRevocation != NULL
         && f_pcontextLEVL->pDatastore            != NULL )
        {
            ChkDR( DRM_BLOBCACHE_Update( f_pcontextLEVL->pcontextBBX,
                                         f_pcontextLEVL->pcontextSSTRevocation,
                                         f_pcontextLEVL->pDatastore,
                                         PB_DSTR( f_pdstrSignedData ),
                                         CB_DSTR( f_pdstrSignedData ),
                                         PB_DSTR( f_pdstrSignatureValue ),
                                         CB_DSTR( f_pdstrSignatureValue ) ) );
        }
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _CheckCertificate(
    const CERT                 *f_pcert,
    const PUBKEY               *f_ppubkey,
          DRM_BOOL              f_fCheckDate,
          DRM_BB_CONTEXT       *f_pcontextBBX,
          DRM_SECSTORE_CONTEXT *f_pcontextSSTRevocation,
          DRM_DST              *f_pDatastore,
          DRM_VOID             *f_pOEMContext ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CheckCertificate(
    const CERT                 *f_pcert,
    const PUBKEY               *f_ppubkey,
          DRM_BOOL              f_fCheckDate,
          DRM_BB_CONTEXT       *f_pcontextBBX,
          DRM_SECSTORE_CONTEXT *f_pcontextSSTRevocation,
          DRM_DST              *f_pDatastore,
          DRM_VOID             *f_pOEMContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_DWORD  cbData = 0;
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BOOL   fUseCache = ( f_pcontextSSTRevocation != NULL && f_pDatastore != NULL );

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMUTILITIES, PERF_FUNC__CheckCertificate);

    ChkArg(f_pcert       != NULL
        && f_ppubkey     != NULL
        && f_pcontextBBX != NULL );

    BYTES_TO_DWORD(cbData, f_pcert->datalen);

    if (MEMCMP(f_pcert->certVersion, (DRM_BYTE *)CERT_VER, VERSION_LEN) != 0)
    {
        ChkDR(DRM_E_UNSUPPORTED_VERSION);
    }

    if (f_fCheckDate && !DRM_UTL_IsCertDateOK( f_pOEMContext, f_pcert->cd.expiryDate))
    {
        ChkDR(DRM_E_EXPIRED_CERT);
    }

    /*
    ** First check to see if the certificate signature is cached.
    ** If it is then we don't need to call DRM_PK_Verify
    */
    if( !fUseCache
     || !DRM_CERTCACHE_VerifyCachedCertificate( f_pcontextBBX, f_pcontextSSTRevocation, f_pDatastore, (const DRM_BYTE*) f_pcert, sizeof( *f_pcert ) ) )
    {
        if ( DRM_FAILED( DRM_PK_Verify( f_pcontextBBX->CryptoContext.rgbCryptoContext,
                                        eDRM_ECC_P160,
                                        DRM_ECC160_PUBLIC_KEY_LEN,
                                        ( const DRM_BYTE * )f_ppubkey,
                                        cbData,
                                        ( const DRM_BYTE * )&f_pcert->cd,
                                        DRM_ECC160_SIGNATURE_LEN,
                                        f_pcert->sign ) ) )
        {
            ChkDR(DRM_E_DRMUTIL_INVALID_CERT);
        }

        (DRM_VOID)DRM_CERTCACHE_AddCachedCertificate( f_pcontextBBX, f_pcontextSSTRevocation, f_pDatastore, (const DRM_BYTE*) f_pcert, sizeof( *f_pcert ) );
    }

ErrorExit:

/*
    DRM_PROFILING_USER_DATA_1(DRM_PROFILING_BINARY_HASH((DRM_BYTE*)f_pcert,SIZEOF(CERT)), 1);
    DRM_PROFILING_USER_DATA_2(DRM_PROFILING_BINARY_HASH((DRM_BYTE*)f_ppubkey,SIZEOF(PUBKEY)), 1);
*/
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function :   DRM_UTL_CheckCertificate
**
** Synopsis :   Verifies the signature on a certificate using either the
**              supplied public key, or the default for the supplied
**              DRM_ROOTPUBKEY_CATEGORY
**
** Arguments :  f_pcert     -   Cert to be checked
**              f_ppubkey   -   Pubkey to be used for verification - If it is
**                              NULL, use appropriate hard-coded key
**              f_fCheckCertDate    -   Check for expiration
**              f_pcontextCrypto   -   Crypto context
**              f_eRootPubkey   -   The root pubkey to use if f_ppubkey is NULL.
**                                  if it is WM_DRM_ROOTPUBKEY_CLK
**                                  f_pcontextCrypto->pubKey will hold the root
**                                  public key to use to verify the cert chain.
**
** Returns :    DRM_RESULT
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_CheckCertificate(
    __in const   CERT                    *f_pcert,
    __in const   PUBKEY                  *f_ppubkey,
    __in         DRM_BOOL                 f_fCheckCertDate,
    __in         DRM_LICEVAL_CONTEXT     *f_pcontextLEVL,
    __in         DRM_ROOTPUBKEY_CATEGORY  f_eRootPubkey )
{
    DRM_RESULT         dr         =   DRM_SUCCESS;
    const PUBKEY      *ppubkey    =   NULL;

    static PUBKEY const pubkeyMSCERT = { {
            0x4D, 0xBF, 0xD9, 0x0D, 0xD9, 0x6E, 0x8C, 0x9E,
            0x32, 0x5F, 0x4F, 0x3D, 0xEC, 0xA9, 0x84, 0x59,
            0x6B, 0x5E, 0x06, 0x86, 0xE7, 0xE2, 0xC2, 0x8B,
            0xDE, 0x14, 0x4B, 0x29, 0x2C, 0xEC, 0x4D, 0x1D,
            0x76, 0xFD, 0x5A, 0x14, 0x90, 0x3A, 0x10, 0x77
    } };

    static PUBKEY const pubkeyRootMeteringCert =
    {
        0x45, 0xB1, 0xA7, 0xE1, 0x90, 0x81, 0x98, 0x37,
        0x00, 0xCC, 0x89, 0xA7, 0x57, 0x24, 0x72, 0xB9,
        0xC1, 0x29, 0xA3, 0x62, 0xD9, 0x55, 0x74, 0x04,
        0x02, 0x7D, 0x6E, 0x69, 0x79, 0xE9, 0x6A, 0xD9,
        0x7A, 0x92, 0xE4, 0xF3, 0x4B, 0x6B, 0x42, 0x6C
    };

    ChkArg( f_pcontextLEVL              != NULL
         && f_pcontextLEVL->pcontextBBX != NULL);

    if (f_ppubkey != NULL)
    {
        ppubkey = f_ppubkey;
    }
    else
    {
        switch(f_eRootPubkey)
        {
        case WM_DRM_ROOTPUBKEY_LRB:
        case WM_DRM_ROOTPUBKEY_LICENSES:
            ppubkey = &pubkeyMSCERT;
            break;

        case WM_DRM_ROOTPUBKEY_CLK:
            ppubkey = &(f_pcontextLEVL->pcontextBBX->CryptoContext.pubKey);
            break;

        case WM_DRM_ROOTPUBKEY_MTR:
            ppubkey = &pubkeyRootMeteringCert;
            break;

        default :
            ChkDR(DRM_E_INVALIDARG);
        }
    }

    ChkDR( _CheckCertificate( f_pcert,
                              ppubkey,
                              f_fCheckCertDate,
                              f_pcontextLEVL->pcontextBBX,
                              f_pcontextLEVL->pcontextSSTRevocation,
                              f_pcontextLEVL->pDatastore,
                              f_pcontextLEVL->pcontextBBX->pOEMContext ) );

ErrorExit:
    return dr;
}

static const PUBKEY pubkeyMS =
{
    {
        0x4D, 0xBF, 0xD9, 0x0D, 0xD9, 0x6E, 0x8C, 0x9E,
        0x32, 0x5F, 0x4F, 0x3D, 0xEC, 0xA9, 0x84, 0x59,
        0x6B, 0x5E, 0x06, 0x86, 0xE7, 0xE2, 0xC2, 0x8B,
        0xDE, 0x14, 0x4B, 0x29, 0x2C, 0xEC, 0x4D, 0x1D,
        0x76, 0xFD, 0x5A, 0x14, 0x90, 0x3A, 0x10, 0x77
    }
};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_UTL_CheckCert(
    __in const CERT                *f_pcert,
    __in const PUBKEY              *f_ppubkey,
    __in       DRM_BOOL             f_fCheckDate,
    __in       DRM_LICEVAL_CONTEXT *f_pcontextLEVL )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pcontextLEVL              != NULL
         && f_pcontextLEVL->pcontextBBX != NULL);

    ChkDR(_CheckCertificate( f_pcert,
                            (f_ppubkey != NULL) ? f_ppubkey : &pubkeyMS,
                             f_fCheckDate,
                             f_pcontextLEVL->pcontextBBX,
                             f_pcontextLEVL->pcontextSSTRevocation,
                             f_pcontextLEVL->pDatastore,
                             f_pcontextLEVL->pcontextBBX->pOEMContext ) );

ErrorExit:
    return (dr);
}

DRM_API DRM_RESULT DRM_CALL DRM_UTL_CheckCertNoCache(
    __in const CERT               *f_pcert,
    __in const PUBKEY             *f_ppubkey,
    __in       DRM_BOOL            f_fCheckDate,
    __in       DRM_CRYPTO_CONTEXT *f_pcontextCRYP,
    __in_opt   DRM_VOID           *f_pOEMContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    ** The casting of f_pcontextCRYP to (DRM_BB_CONTEXT*) below is a safe cast.  When
    ** the underlying CheckCert functions are called, they will not use the BBX as a
    ** BBX, instead they will just use it as a crypto context.
    */

    ChkDR(_CheckCertificate( f_pcert,
                            (f_ppubkey != NULL) ? f_ppubkey : &pubkeyMS,
                             f_fCheckDate,
                             (DRM_BB_CONTEXT*)f_pcontextCRYP,
                             NULL,
                             NULL,
                             f_pOEMContext ) );

ErrorExit:
    return (dr);
}

DRM_API DRM_RESULT DRM_CALL DRM_UTL_CheckDACCert(
    __in const CERT                *f_pcert,
    __in const PUBKEY              *f_ppubkey,
    __in       DRM_BOOL             f_fCheckDate,
    __in       DRM_LICEVAL_CONTEXT *f_pcontextLEVL )
{
    DRM_RESULT    dr = DRM_SUCCESS;

    static const PUBKEY pubkeyDeviceAuthorizationCertificate =
    {
        {
            0x4D, 0xBF, 0xD9, 0x0D, 0xD9, 0x6E, 0x8C, 0x9E,
            0x32, 0x5F, 0x4F, 0x3D, 0xEC, 0xA9, 0x84, 0x59,
            0x6B, 0x5E, 0x06, 0x86, 0xE7, 0xE2, 0xC2, 0x8B,
            0xDE, 0x14, 0x4B, 0x29, 0x2C, 0xEC, 0x4D, 0x1D,
            0x76, 0xFD, 0x5A, 0x14, 0x90, 0x3A, 0x10, 0x77
        }
    };

    ChkArg( f_pcontextLEVL              != NULL
         && f_pcontextLEVL->pcontextBBX != NULL);

    ChkDR(_CheckCertificate( f_pcert,
                            (f_ppubkey != NULL) ? f_ppubkey : &pubkeyDeviceAuthorizationCertificate,
                             f_fCheckDate,
                             f_pcontextLEVL->pcontextBBX,
                             f_pcontextLEVL->pcontextSSTRevocation,
                             f_pcontextLEVL->pDatastore,
                             f_pcontextLEVL->pcontextBBX->pOEMContext ) );

ErrorExit:
    return (dr);
}

/**********************************************************************
** Function:    PKEncryptLarge
**
** Synopsis:    Encrypt large amoutn of data (using PKEncrypt with large data is slow, this is fast)
**
** Arguments:   [f_ppubkey]        -- Caller supplied public key to encrypt with
**              [f_pbClear]        -- Array of bytes to encrypt
**              [f_cbClear]        -- Length ob f_pbClear in bytes
**              [f_pbCipher]       -- Buffer to hold encrypted data
**              [f_cbKeySymmetric] -- Desired length of internal symmertic key to be created
**              [f_pcontextCRYP]   -- A DRM_CRYPTO_CONTEXT so PKCrypto operations can be performed.
**
** Notes:       out must be of length f_cbClear + DRM_ECC160_CIPHERTEXT_LEN
**              Internally a rc4 symmetric key will be created to encrypt the content (because it's fast) &
**              that symmertic key will be encrypted with the PUBKEY.
**              In place encryption is possible if f_pbCipher equals to f_pbClear.
***********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_PK_EncryptLarge(
    __in_opt                                                    DRM_VOID            *f_pOEMContext,
    __in                                                  const PUBKEY              *f_ppubkey,
    __inout_bcount( f_cbClear )                           const DRM_BYTE            *f_pbClear,
    __in                                                        DRM_DWORD            f_cbClear,
    __out_bcount( f_cbClear + DRM_ECC160_CIPHERTEXT_LEN )       DRM_BYTE            *f_pbCipher,
    __in                                                        DRM_DWORD            f_cbKeySymmetric,
    __in                                                        DRM_CRYPTO_CONTEXT  *f_pcontextCRYP )
{
    DRM_RESULT     dr          = DRM_SUCCESS;
    DRM_BYTE      *pbKey       = NULL; /* This needs __CB_DECL(DRM_ECC160_PLAINTEXT_LEN) bytes */
    RC4_KEYSTRUCT  rc4KS       = { 0 };
    DRM_DWORD      cbEncrypted = DRM_ECC160_CIPHERTEXT_LEN;

    ChkArg(f_ppubkey      != NULL
        && f_pbClear      != NULL
        && f_pbCipher     != NULL
        && f_pcontextCRYP != NULL);

    ChkArg(f_cbKeySymmetric <= (DRM_ECC160_PLAINTEXT_LEN - 2));

    if (f_cbKeySymmetric > DRMCIPHERKEYLEN_RC4)
    {
        f_cbKeySymmetric = DRMCIPHERKEYLEN_RC4;
    }

    /* Ensure the temporary buffer we are using is big enough */
    DRMCASSERT( DRM_ECC160_PLAINTEXT_LEN <= SIZEOF( f_pcontextCRYP->signature ) );
    pbKey = f_pcontextCRYP->signature;

    DRM_BYT_MoveBytes(f_pbCipher, DRM_ECC160_CIPHERTEXT_LEN, f_pbClear, 0, f_cbClear);

    PUT_BYTE( pbKey, 0, (DRM_BYTE) f_cbKeySymmetric );
    PUT_BYTE( pbKey, 1, (DRM_BYTE) PKSYMM_ALG_TYPE_RC4 );

    ChkDR(Oem_Random_GetBytes(f_pOEMContext, pbKey + __CB_DECL(2), f_cbKeySymmetric));
    ChkDR( DRM_PK_Encrypt( NULL,
                           f_pcontextCRYP->rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PUBLIC_KEY_LEN,
                           ( DRM_BYTE * )f_ppubkey,
                           DRM_ECC160_PLAINTEXT_LEN,
                           pbKey,
                           &cbEncrypted,
                           f_pbCipher ) );

    DRM_RC4_KeySetup(&rc4KS, f_cbKeySymmetric, pbKey      + __CB_DECL(2));
    DRM_RC4_Cipher  (&rc4KS, f_cbClear,        f_pbCipher + __CB_DECL(DRM_ECC160_CIPHERTEXT_LEN));

ErrorExit:
    return dr;
}

/**********************************************************************
** Function:    DRM_PK_DecryptLarge
**
** Synopsis:    Dencrypt data encrypted with DRM_PK_EncryptLarge
**
** Arguments:   [f_ppubkey] -- Caller supplied private key to decrypt with
**              [f_pbCipher]    -- Array of bytes to decrypt
**              [f_cbCipher]    -- Length of f_pbCipher in bytes
**              [f_pbClear]   -- Buffer to hold decrypted data
**              [f_pcontextCRYP] -- A DRM_CRYPT_CONTEXT so PKCrypto operations can be performed.
**
** Notes:       out must be at least f_cbCipher - DRM_ECC160_CIPHERTEXT_LEN in length.
**              In place decryption is possible if f_pbClear equals to f_pbCipher.
***********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_PK_DecryptLarge(
    __in                                                   const PRIVKEY            *f_ppubkey,
    __inout_bcount( f_cbCipher)                            const DRM_BYTE           *f_pbCipher,
    __in                                                         DRM_DWORD           f_cbCipher,
    __out_bcount( f_cbCipher - DRM_ECC160_CIPHERTEXT_LEN )       DRM_BYTE           *f_pbClear,
    __in                                                         DRM_CRYPTO_CONTEXT *f_pcontextCRYP )
{
    DRM_RESULT     dr    = DRM_SUCCESS;
    DRM_BYTE      *pbKey = NULL;
    DRM_DWORD      cbKey = SIZEOF( f_pcontextCRYP->signature );
    RC4_KEYSTRUCT  rc4KS;

    ChkArg(f_ppubkey      != NULL
        && f_pbCipher     != NULL
        && f_pbClear      != NULL
        && f_pcontextCRYP != NULL);

    /* Ensure the temporary buffer we are using is big enough */
    DRMCASSERT( DRM_ECC160_PLAINTEXT_LEN <= SIZEOF( f_pcontextCRYP->signature ) );
    pbKey = f_pcontextCRYP->signature;

    ChkDR( DRM_PK_Decrypt( f_pcontextCRYP->rgbCryptoContext,
                           eDRM_ECC_P160,
                           DRM_ECC160_PRIVATE_KEY_LEN,
                           ( const DRM_BYTE * )f_ppubkey,
                           DRM_ECC160_CIPHERTEXT_LEN,
                           f_pbCipher,
                           &cbKey,
                           pbKey ) );

    if (GET_BYTE(pbKey,1) != PKSYMM_ALG_TYPE_RC4)
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    ChkOverflow( f_cbCipher, f_cbCipher - DRM_ECC160_CIPHERTEXT_LEN );

    DRM_BYT_MoveBytes(f_pbClear,
                      0,
                      f_pbCipher,
                      DRM_ECC160_CIPHERTEXT_LEN,
                      f_cbCipher - DRM_ECC160_CIPHERTEXT_LEN);

    DRM_RC4_KeySetup(&rc4KS, GET_BYTE(pbKey,0),                  pbKey + __CB_DECL(2));
    DRM_RC4_Cipher  (&rc4KS, f_cbCipher - DRM_ECC160_CIPHERTEXT_LEN, f_pbClear);

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_UTL_VerifyXMLSignature
**
** Synopsis :   Verify the xml signature over some data
**
** Arguments :  f_pdstrSignedData   -   The entire data that is signed
**              f_pdstrSignatureXML -   <SIGNATURE>...</SIGNATURE>
**              f_pdstrCertChainXML -   <CERTIFICATECHAIN>...</CERTIFICATECHAIN>
**              f_fCheckExpiry      -   Check for cert expiration
**              f_fCheckCertChain   -   Verify cert chain. If this is false,
**                                      f_pcontextCrypto->pubKey must contain
**                                      the public key which will be used to
**                                      verify the signature.
**              f_eRootPubkey       -   Root Pubkey to use for cert verification
**                                      if it is WM_DRM_ROOTPUBKEY_CLK,
**              f_pcontextLEVL      -   License eval context.  The BBX pointer must be valid.
**                                      The DST and pcontextSSTRevocation pointers must be valid to perform cert caching.
**
** Returns :    DRM_SUCCESS -   Signature verified successfully
**              DRM_E_INVALID_SIGNATURE -   Verification failed
**              some other problem
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_VerifyXMLSignature(
    __in     const   DRM_CONST_STRING        *f_pdstrSignedData,
    __in     const   DRM_CONST_STRING        *f_pdstrSignatureXML,
    __in_opt const   DRM_CONST_STRING        *f_pdstrCertChainXML,
    __in             DRM_BOOL                 f_fCheckExpiry,
    __in             DRM_BOOL                 f_fCheckCertChain,
    __in             DRM_ROOTPUBKEY_CATEGORY  f_eRootPubkey,
    __in             DRM_LICEVAL_CONTEXT     *f_pcontextLEVL )
{
    DRM_CONST_STRING dstrSignature = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTmp       = EMPTY_DRM_STRING;
    DRM_RESULT       dr            = DRM_SUCCESS;

    /*
    **  Verify input
    */
    ChkDRMString(f_pdstrSignedData);
    ChkDRMString(f_pdstrSignatureXML);
    if (f_fCheckCertChain)
    {
        ChkDRMString(f_pdstrCertChainXML);
    }

    ChkArg( f_pcontextLEVL              != NULL
         && f_pcontextLEVL->pcontextBBX != NULL );

    /*
    **  Verify Hashing algorithm
    */
    dr = DRM_XML_GetSubNode( f_pdstrSignatureXML,
                            &g_dstrTagHashAlg,
                            &g_dstrAttributeType,
                            &g_dstrSHA,
                             0,
                             NULL,
                            &dstrTmp,
                             1);
    if( DRM_FAILED(dr) )
    {
        ChkDR(DRM_E_UNSUPPORTED_ALGORITHM);
    }

    /*
    **  Verify Signature algorithm
    */
    dr = DRM_XML_GetSubNode( f_pdstrSignatureXML,
                            &g_dstrTagSignAlg,
                            &g_dstrAttributeType,
                            &g_dstrMSDRM,
                             0,
                             NULL,
                            &dstrTmp,
                            1);
    if( DRM_FAILED(dr) )
    {
        ChkDR(DRM_E_UNSUPPORTED_ALGORITHM);
    }

    /*
    **  Get Signature value
    */
    ChkDR( DRM_XML_GetSubNode( f_pdstrSignatureXML,
                              &g_dstrTagValue,
                               NULL,
                               NULL,
                               0,
                               NULL,
                              &dstrSignature,
                               1)  );

    dr = _UTL_VerifySignature( f_pdstrSignedData,
                              &dstrSignature,
                               f_pdstrCertChainXML,
                               f_fCheckExpiry,
                               f_fCheckCertChain,
                               f_eRootPubkey,
                               f_pcontextLEVL );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_UTL_VerifyXMLSignatureEx
**
** Synopsis :   Verify the xml signature over some data
**
** Arguments :  f_pdstrSignedData   -   The entire data that is signed
**              f_pdstrSignatureValue -   Sig value
**              f_pdstrCertChainXML -   <CERTIFICATECHAIN>...</CERTIFICATECHAIN>
**              f_fCheckExpiry      -   Check for cert expiration
**              f_fCheckCertChain   -   Verify cert chain. If this is false,
**                                      f_pcontextCrypto->pubKey must contain
**                                      the public key which will be used to
**                                      verify the signature.
**              f_eRootPubkey       -   Root Pubkey to use for cert verification
**              f_pcontextLEVL      -   License eval context.  The BBX pointer must be valid.
**                                      The DST and pcontextSSTRevocation pointers must be valid to perform cert caching.
**
** Returns :    DRM_SUCCESS -   Signature verified successfully
**              DRM_E_INVALID_SIGNATURE -   Verification failed
**              some other problem
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_VerifyXMLSignatureEx(
    __in     const   DRM_CONST_STRING        *f_pdstrSignedData,
    __in     const   DRM_CONST_STRING        *f_pdstrSignatureValue,
    __in_opt const   DRM_CONST_STRING        *f_pdstrCertChainXML,
    __in             DRM_BOOL                 f_fCheckExpiry,
    __in             DRM_BOOL                 f_fCheckCertChain,
    __in             DRM_ROOTPUBKEY_CATEGORY  f_eRootPubkey,
    __in             DRM_LICEVAL_CONTEXT     *f_pcontextLEVL )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    **  Verify input
    */
    ChkDRMString(f_pdstrSignedData);
    ChkDRMString(f_pdstrSignatureValue);
    if (f_fCheckCertChain)
    {
        ChkDRMString(f_pdstrCertChainXML);
    }
    ChkArg( f_pcontextLEVL              != NULL
         && f_pcontextLEVL->pcontextBBX != NULL );

    dr = _UTL_VerifySignature( f_pdstrSignedData,
                               f_pdstrSignatureValue,
                               f_pdstrCertChainXML,
                               f_fCheckExpiry,
                               f_fCheckCertChain,
                               f_eRootPubkey,
                               f_pcontextLEVL );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_UTL_BuildXMLSymmSignature
**
** Synopsis :   Build a XML symmetric signature node in the form of
**              <SYMSIGNATURE pubkey="B64 encoded ECC 160 public key">
**              B64 encoded signature</SYMSIGNATURE> in the caller provided
**              buffer.
**
** Arguments :  f_poPubKey          -   Pointer to an ECC 160 public key.
**              f_rgbSymSig         -   Pointer to a buffer that contains a
**                                      SHA-1 based symmetric signature.
**              f_pwchBuffer        -   Pointer to a wide character buffer that
**                                      is big enough to store the generated
**                                      signature XML node.
**              f_cchBuffer         -   Size (number of wide characters) of the
**                                      buffer mentioned above. It should be at
**                                      least CCH_SYMSIG_TAG.
**              f_pCkVerifiedGUID   -   Pointer to the ID GUID specifying whether the
**                                      content key signature is verified.
**
** Returns :    DRM_SUCCESS         -   Signature is successfully built.
**              DRM_E_INVALIDARG    -   If any of the arguments is invalid.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_UTL_BuildXMLSymmSignature(
    __in                                                  PUBKEY           *f_poPubKey,
    __in_bcount( __CB_DECL( DRM_SHA1_DIGEST_LEN ) ) const DRM_BYTE          f_rgbSymSig[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __in_ecount( f_cchBuffer )                            DRM_WCHAR        *f_pwchBuffer,
    __in                                                  DRM_DWORD         f_cchBuffer,
    __in_opt                                        const DRM_GUID         *f_pCkVerifiedGUID )
{
    DRM_RESULT dr                 = DRM_SUCCESS;
    DRM_WCHAR *pwszInsertionPoint = f_pwchBuffer;
    DRM_DWORD  cch                = 0;

    ChkArg( f_poPubKey   != NULL );
    ChkArg( f_pwchBuffer != NULL );
    ChkArg( f_cchBuffer  >= CCH_SYMSIG_TAG );

    /* Open node */
    *pwszInsertionPoint = g_wchLessThan;
    pwszInsertionPoint += 1;

    ChkDR( DRM_STR_StringCchCopyNW( pwszInsertionPoint,
                                    g_dstrTagSymSig.cchString + 1,
                                    g_dstrTagSymSig.pwszString,
                                    g_dstrTagSymSig.cchString ) );

    pwszInsertionPoint += g_dstrTagSymSig.cchString;

    /* Add pubkey attribute */
    *pwszInsertionPoint = g_wchSpace;
    pwszInsertionPoint += 1;

    ChkDR( DRM_STR_StringCchCopyNW( pwszInsertionPoint,
                                    g_dstrAttributePubkey.cchString + 1,
                                    g_dstrAttributePubkey.pwszString,
                                    g_dstrAttributePubkey.cchString ) );

    pwszInsertionPoint += g_dstrAttributePubkey.cchString;

    *pwszInsertionPoint = g_wchEqual;
    pwszInsertionPoint += 1;

    *pwszInsertionPoint = g_wchQuote;
    pwszInsertionPoint += 1;

    cch = CCH_BASE64_EQUIV( SIZEOF( PUBKEY ) );
    ChkDR(DRM_B64_EncodeW( ( DRM_BYTE * )f_poPubKey,
                            SIZEOF( PUBKEY ),
                            pwszInsertionPoint,
                            &cch,
                            DRM_BASE64_ENCODE_WMDRM ) );

    pwszInsertionPoint += cch;

    *pwszInsertionPoint = g_wchQuote;
    pwszInsertionPoint += 1;

    /* Add ID (CK verified) attribute */
    if ( f_pCkVerifiedGUID != NULL )
    {
        *pwszInsertionPoint = g_wchSpace;
        pwszInsertionPoint += 1;

        ChkDR( DRM_STR_StringCchCopyNW( pwszInsertionPoint,
                                        g_dstrAttributeVerifiedCK.cchString + 1,
                                        g_dstrAttributeVerifiedCK.pwszString,
                                        g_dstrAttributeVerifiedCK.cchString ) );

        pwszInsertionPoint += g_dstrAttributeVerifiedCK.cchString;

        *pwszInsertionPoint = g_wchEqual;
        pwszInsertionPoint += 1;

        *pwszInsertionPoint = g_wchQuote;
        pwszInsertionPoint += 1;

        ChkDR( DRM_UTL_GuidToString( f_pCkVerifiedGUID, pwszInsertionPoint ) );
        pwszInsertionPoint += DRM_GUID_STRING_LEN;

        *pwszInsertionPoint = g_wchQuote;
        pwszInsertionPoint += 1;
    }

    *pwszInsertionPoint = g_wchGreaterThan;
    pwszInsertionPoint += 1;

    /* SymSig value */
    cch = CCH_BASE64_EQUIV( DRM_SHA1_DIGEST_LEN );
    ChkDR( DRM_B64_EncodeW( f_rgbSymSig,
                            DRM_SHA1_DIGEST_LEN,
                            pwszInsertionPoint,
                            &cch,
                            0 ) );

    pwszInsertionPoint += cch;

    /* Close node */
    *pwszInsertionPoint = g_wchLessThan;
    pwszInsertionPoint += 1;

    *pwszInsertionPoint = g_wchForwardSlash;
    pwszInsertionPoint += 1;

    ChkDR( DRM_STR_StringCchCopyNW( pwszInsertionPoint,
                                    g_dstrTagSymSig.cchString + 1,
                                    g_dstrTagSymSig.pwszString,
                                    g_dstrTagSymSig.cchString ) );

    pwszInsertionPoint += g_dstrTagSymSig.cchString;

    *pwszInsertionPoint = g_wchGreaterThan;

ErrorExit:

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

