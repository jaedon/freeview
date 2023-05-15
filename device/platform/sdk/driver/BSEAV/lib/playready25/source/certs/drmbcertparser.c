/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMBCERTPARSER_C
#include <drmbcertparser.h>
#include <drmbcertparsercache.h>
#include <drmutf.h>
#include <drmcertcache.h>
#include <drmbytemanip.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

#define DoExpirationCheck(ft) ( ( (ft).dwLowDateTime != 0 ) && ( (ft).dwHighDateTime != 0 ) )

static DRM_RESULT _GetCachedPublicKeyByUsage(
    __inout_ecount_opt(1)               DRM_BB_CONTEXT             *f_pBlackboxContext,
    __in_ecount_opt(1)            const DRM_BB_KEYHANDLE_CALLBACKS *f_pKeyHandleCallbacks,
    __in_bcount(f_cbCertData)     const DRM_BYTE                   *f_pbCertData,
    __in                          const DRM_DWORD                   f_cbCertData,
    __in                          const DRM_DWORD                   f_dwKeyUsage,
    __inout_ecount(1)                   DRM_PUBLIC_KEY_CONTEXT     *f_pPubkeyContext,
    __in                                OEM_CRYPTO_HANDLE           f_hSigningKey,
    __out_ecount_opt(1)                 DRM_DWORD                  *f_pdwKeyUsageSet );

/*****************************************************************************
** Function:    _verifyAdjacentCerts
**
** Synopsis:    Verifies that certificates in a chain are correctly linked:
**              1. Issuer key value is the same as a key entry in the next certificate
**                 that issued previous one.
**              2. Security level of the next cert is not lower
**                 then that of the current cert.
**              3. The parent certificate has the appropriate issuer rights to issue
**                 the child certificate of a particular type with particular key usages and features.
**
** Arguments: [f_poChildCert]  : pointer to structure that contains information
**                               from the child certificate, cannot be NULL
**            [f_poParentCert] : pointer to structure with similar information
**                               from the parent certificate, cannot be NULL
**                               so this function will never be called for a chain with one cert.
**    [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH
**              DRM_E_BCERT_INVALID_SECURITY_LEVEL
**              DRM_E_BCERT_INVALID_KEY_USAGE
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _verifyAdjacentCerts(
    __in_ecount(1)  const DRM_BCERT_MINIMALDATA     *f_poChildCert,
    __in_ecount(1)  const DRM_BCERT_MINIMALDATA     *f_poParentCert,
    __inout_ecount(1) DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr                        = DRM_SUCCESS;
    DRM_DWORD  iKey                      = 0;
    DRM_DWORD  iCount                    = 0;
    DRM_DWORD  dwParentKeyUsageMask      = 0;
    DRM_DWORD  dwChildKeyUsageMask       = 0;
    DRM_BOOL   fSupportSecureClock       = FALSE;
    DRM_BOOL   fSupportAntiRollbackClock = FALSE;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_verifyAdjacentCerts );

    ChkArg( f_poChildCert != NULL );
    ChkArg( f_poParentCert != NULL );

    /*
    ** Parent cert type should be issuer because it has child certificate
    */

    DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                  f_poParentCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_ISSUER,
                                  DRM_E_BCERT_INVALID_CERT_TYPE );

    /*
    ** Security level of the parent cert is higher or equal to its child cert's.
    */

    DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                  f_poChildCert->oBasicInfo.dwSecurityLevel <= f_poParentCert->oBasicInfo.dwSecurityLevel,
                                  DRM_E_BCERT_INVALID_SECURITY_LEVEL );

    /*
    ** Issuer key for the current cert is the same as one of the key values in the child cert
    */
    dr = DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH;


    for ( iKey = 0; iKey < f_poParentCert->oKeyInfo.dwNumKeys; iKey++ )
    {
        if ( DRM_BYT_CompareBytes( f_poChildCert->oSignatureInfo.pIssuerKey,
                                   0,
                                   f_poParentCert->oKeyInfo.rgoKeys[iKey].pValue,
                                   0,
                                   DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH ) == 0 )
        {

            dr = DRM_SUCCESS;
            break;
        }
    }
    if ( DRM_FAILED(dr) )
    {

        DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext, FALSE, dr );
    }

    /*
    ** Collect the key usages of the parent certificate for all keys in the certificate
    */
    for ( iKey = 0; iKey < f_poParentCert->oKeyInfo.dwNumKeys; iKey++ )
    {
        dwParentKeyUsageMask |= f_poParentCert->oKeyInfo.rgoKeys[iKey].dwUsageSet;
    }


    /*
    ** Check that the current certificate has appropriate Issuer rights
    ** to issue a child certificate of particular type.
    */
    if ( !(dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_ALL ) ) ) /* Issuer-All can issue anything */
    {
        if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_DOMAIN )
        {
            /*
            ** Issuer-Domain can issue certificates of Type Domain.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_DEVICE )
        {
            /*
            ** Issuer-Device can issue certificate of Type Device.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_PC )
        {
            /*
            ** Issuer-Indiv can issue certificate of Type PC.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_INDIV),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SILVERLIGHT )
        {
            /*
            ** Issuer-SilverLight can issue certificate of Type SilverLight.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_APPLICATION )
        {
            /*
            ** Issuer-Application can issue certificate of Type Application.
            */
			printf("%s - _____________ !!! (%d) \n", __FUNCTION__, __LINE__);
            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_METERING)
        {
            /*
            ** Issuer-Metering can issue certificate of Type Metering.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_METERING),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_KEYFILESIGNER )
        {
            /*
            ** Issuer-SignKeyFile can issue certificate of Type KeyFileSigner
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVER )
        {
            /*
            ** Issuer-Server can issue certificate of Type Server
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SERVER),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_CRL_SIGNER )
        {
            /*
            ** Issuer-CLR can issue certificate of Type CRL Signer
            */

            ChkBOOL( dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_CRL), DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_ISSUER )
        {
            /*
            ** Issuer cert is issued by another issuer cert, and Issuer-related key usages should match.
            */
            for ( iKey = 0; iKey < f_poChildCert->oKeyInfo.dwNumKeys; iKey++ )
            {
                dwChildKeyUsageMask |= f_poChildCert->oKeyInfo.rgoKeys[iKey].dwUsageSet;
            }


            /*
            ** Cannot have Issuer-All child cert
            */
            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                !( dwChildKeyUsageMask & BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_ALL ) ),
                DRM_E_BCERT_INVALID_KEY_USAGE );
            /*
            ** Check key usages in a child cert:
            ** the parent cert is not Issuer-All and must have all issuer-... key usages that child cert has
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                    ( dwChildKeyUsageMask & BCERT_KEYUSAGE_PARENT_ISSUERS_MASK )
                        <= ( dwParentKeyUsageMask & BCERT_KEYUSAGE_PARENT_ISSUERS_MASK ),
                    DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_LICENSESIGNER )
        {
            /*
            ** Issuer-SignLicense can issue certificate of Type LicenseSigner
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SIGN_LICENSE),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else
        {
            /*
            ** Service cert type. If it has a parent cert it can only be Issuer_All cert, so fail
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVICE,
                                          DRM_E_BCERT_INVALID_CERT_TYPE );


            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext, FALSE, DRM_E_BCERT_INVALID_KEY_USAGE );
        }
    }

    /*
    ** Some features in a child cert can only be issued by a parent cert with specific key usages.
    */

    for ( iCount = 0; iCount < f_poChildCert->oFeatureInfo.dwNumFeatureEntries; iCount++ )
    {

        if (  f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_TRANSMITTER
           || f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_RECEIVER
           || f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_SHARED_CERTIFICATE )
        {
            /*
            ** Transmitter, Receiver, SharedCertificate can be issued by Issuer-Link.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_LINK),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_SECURE_CLOCK )
        {
            /*
            ** SecureClock can be issued by Issuer-Device.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
            fSupportSecureClock = TRUE;
        }
        else if ( f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK )
        {
            /*
            ** Anti-Rollback Clock can be issued by Issuer-Device.
            */

            DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                          dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE),
                                          DRM_E_BCERT_INVALID_KEY_USAGE );
            fSupportAntiRollbackClock = TRUE;
        }
    }

    /*
    ** Either SecureClock or Anti-Rollback Clock can be issued in one certificate, never both.
    */

    DRM_BCERT_CHKVERIFICATIONERR( f_pVerificationContext,
                                  !( fSupportSecureClock && fSupportAntiRollbackClock ),
                                  DRM_E_BCERT_INVALID_KEY_USAGE );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    DRM_BCert_GetCertChainInfo
**
** Synopsis:    Quickly scan through the certificate buffer extracting the 
**              lengths of each certificate into an array, along with the 
**              indices of each signing key in the parent certificate
**
** Arguments:   [f_pbCertData]       : data buffer that is being parsed
**              [f_cbCertData]       : size of the data buffer (in bytes)
**              [f_pbOffset]         : position to start parsing from
**              [f_dwNumCertficates] : the expected number of certificates to find
**              [f_rgdwCertOffsets ] : pointer to offset array to populate
**              [f_rgdwCertOffsets ] : pointer to key index array to populate
**              [f_rgpPublicKeys ]   : array of pointers to the public keys
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_CERT_HEADER_TAG
**              DRM_E_BCERT_INVALID_CERT_VERSION
**              DRM_E_BCERT_INVALID_CERT_LENGTH
**              DRM_E_BCERT_INVALID_SIGNEDCERT_LENGTH
**
** Notes:       None.
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_BCert_GetCertChainInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE          *f_pbCertData,
    __in                      const DRM_DWORD          f_cbCertData,
    __in                            DRM_DWORD          f_bOffset,
    __in                            DRM_DWORD          f_dwNumCertficates,
    __in                            DRM_DWORD          f_idxResolveUpTo,
    __in                            DRM_DWORD          f_dwRequiredKeyUsageType,
    __out                           DRM_DWORD          f_rgdwCertOffsets[DRM_BCERT_MAX_CERTS_PER_CHAIN],
    __out                           DRM_DWORD          f_rgdwKeyIndices[DRM_BCERT_MAX_CERTS_PER_CHAIN],
    __out                           DRM_BCERT_PUBKEY  *f_rgpPublicKeys[DRM_BCERT_MAX_CERTS_PER_CHAIN])
{
    DRM_RESULT                    dr                   = DRM_SUCCESS;
    DRM_DWORD                     dwOffset             = 0;
    DRM_DWORD                     iCert                = 0;
    DRM_DWORD                     iLastCert            = f_dwNumCertficates - 1;
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = { 0 };
    DRM_BCERT_SIGNATURE_INFO      oChildSignatureInfo  = { 0 };
    DRM_DWORD                     dwKeyUsageMatchMask  = (DRM_DWORD)BCERT_KEYUSAGE_BIT( f_dwRequiredKeyUsageType );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_bOffset    < f_cbCertData );
    ChkArg( f_dwNumCertficates >= 2 );
    ChkArg( f_dwNumCertficates <= DRM_BCERT_MAX_CERTS_PER_CHAIN );

    for( iCert = 0; iCert < DRM_BCERT_MAX_CERTS_PER_CHAIN; iCert++ )
    {
        f_rgpPublicKeys[iCert] = NULL;
    }

    /*
    ** If we are resolving up to a key in the leaf certificate, that key type must be specified
    */
    ChkArg( ( f_idxResolveUpTo == 0 ) != ( f_dwRequiredKeyUsageType == DRM_BCERT_KEYUSAGE_UNKNOWN ) );

    dwOffset = f_bOffset;

    ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_UNKNOWN ,
                                              NULL,
                                              FALSE,
                                              TRUE,
                                              NULL,
                                              0,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    for( iCert = 0; iCert < f_dwNumCertficates; iCert++ )
    {
        DRM_DWORD                dwStartOffset  = dwOffset;
        DRM_DWORD                dwNextOffset   = 0;
        DRM_BCERT_HEADER         oCertHeader    = { 0 };

        f_rgdwCertOffsets[ iCert ] = dwOffset;

        ChkDR( DRM_BCert_parseCertHeader( f_pbCertData, f_cbCertData, &dwOffset, &oCertHeader, &oVerificationContext ) );

        dwNextOffset = dwStartOffset + oCertHeader.cbCertificate;

        /*
        ** If this is not the leaf cert, then we need to find the key index of the key used to sign
        ** the child certificate.
        ** If this is the leaf cert, and we have been asked to resolve up to it, then look for a 
        ** suitable key
        */
        if( iCert > 0 || f_idxResolveUpTo == 0 )
        {
            DRM_BCERT_KEY_INFO oKeyInfo    = { 0 };
            DRM_DWORD          iKeyCounter = 0;
            DRM_BOOL           fFoundKey   = FALSE;

            /*
            ** Loop thru the objects until we find
            ** a key information or reach the end
            */
            ChkDR( DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                                     dwNextOffset, /* limit the search to just this certificate */
                                                     &dwOffset,
                                                     DRM_BCERT_OBJTYPE_KEY ) );

            ChkDR( DRM_BCert_parseKeyInfo( f_pbCertData,
                                           f_cbCertData,
                                           &dwOffset,
                                           DRM_BCERT_CERTTYPE_UNKNOWN,
                                           &oKeyInfo,
                                           &oVerificationContext ) );

            /*
            ** Find the key used to sign the child certificate
            */
            for( iKeyCounter = 0; iKeyCounter < oKeyInfo.dwNumKeys; iKeyCounter++ )
            {
                if( iCert > 0
                    && OEM_SECURE_ARE_EQUAL( oChildSignatureInfo.pIssuerKey,
                                             oKeyInfo.rgoKeys[ iKeyCounter ].pValue,
                                             SIZEOF( PUBKEY_P256 ) ) )
                {
                    /*
                    ** The keys match, store the key index
                    */
                    f_rgdwKeyIndices[iCert] = iKeyCounter;
                    f_rgpPublicKeys[iCert]  = oKeyInfo.rgoKeys[iKeyCounter].pValue;
                    fFoundKey               = TRUE;

                    break;
                }
                else if( iCert == 0
                        && ( oKeyInfo.rgoKeys[ iKeyCounter ].dwUsageSet | dwKeyUsageMatchMask ) != 0 )
                {
                    /*
                    ** The keys match, store the key index
                    */
                    f_rgdwKeyIndices[iCert] = iKeyCounter;
                    f_rgpPublicKeys[iCert]  = oKeyInfo.rgoKeys[iKeyCounter].pValue;
                    fFoundKey               = TRUE;

                    break;
                }
            }

            ChkBOOL( fFoundKey, DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH );
        }


        /*
        ** Now look for the signing key which will be used to search the next cert in the chain. Don't bother
        ** to do this for the last certificate as the signing key will be the root certificate key
        */
        if( iCert < iLastCert )
        {
            ChkDR( DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                                     dwNextOffset, /* limit the search to just this certificate */
                                                     &dwOffset,
                                                     DRM_BCERT_OBJTYPE_SIGNATURE ) );

            ChkDR( DRM_BCert_parseSignatureInfo( f_pbCertData,
                                                 f_cbCertData,
                                                 &dwOffset,
                                                 &oChildSignatureInfo,
                                                 &oVerificationContext ) );
        }

        ChkBOOL( dwOffset <= dwNextOffset, DRM_E_BCERT_INVALID_CERT_LENGTH );

        dwOffset = dwNextOffset;
    }

    ChkBOOL( dwOffset <= f_cbCertData, DRM_E_BCERT_INVALID_CERT_LENGTH );

ErrorExit:
    return dr;
}


/**********************************************************************************************************
** Function:    DRM_BCert_VerifySignature
**
** Synopsis:    Check that the Signature Data matches the ECDSA algorithm over the data
**              up to and not including signature element using IssuerKey in Signature Data.
**
** Arguments:   [f_pbCertificateBegin] : pointer to a place in a data buffer where certificate begins.
** Arguments:   [f_cbCertificate]      : length in bytes of certificate.
**              [f_dwSignedDataLength] : length in bytes of a signed data.
**              [f_poSignatureInfo]    : pointer to a signature info structure, cannot be NULL.
**              [f_pCryptoCtx]         : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**              [f_pcontextBBX]        : Pointer to DRM_BB_CONTEXT used for certificate caching functionality.
**              [f_pcontextSST]        : Pointer to DRM_SECSTORE_CONTEXT used for certificate caching functionality.
**              [f_pDatastore]         : Pointer to DRM_DST used for certificate caching functionality.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE
**              DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH
**
**********************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_VerifySignature(
    __in_ecount(__CB_DECL( f_cbCertificate)) const DRM_BYTE *f_pbCertificateBegin,
    __in              DRM_DWORD                f_cbCertificate,
    __in              DRM_DWORD                f_dwSignedDataLength,
    __inout_ecount(1) DRM_BCERT_SIGNATURE_INFO *f_poSignatureInfo,
    __inout_ecount(1) DRM_CRYPTO_CONTEXT       *f_pCryptoCtx,
    __in_opt          DRM_BB_CONTEXT           *f_pcontextBBX,
    __in_opt          DRM_SECSTORE_CONTEXT     *f_pcontextSST,
    __in_opt          DRM_DST                  *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_VerifySignature(
    __in_ecount(__CB_DECL( f_cbCertificate)) const DRM_BYTE *f_pbCertificateBegin,
    __in              DRM_DWORD                f_cbCertificate,
    __in              DRM_DWORD                f_dwSignedDataLength,
    __inout_ecount(1) DRM_BCERT_SIGNATURE_INFO *f_poSignatureInfo,
    __inout_ecount(1) DRM_CRYPTO_CONTEXT       *f_pCryptoCtx,
    __in_opt          DRM_BB_CONTEXT           *f_pcontextBBX,
    __in_opt          DRM_SECSTORE_CONTEXT     *f_pcontextSST,
    __in_opt          DRM_DST                  *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT     dr         = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_VerifySignature );

    ChkArg( f_pbCertificateBegin != NULL );
    ChkArg( f_cbCertificate > 0 );
    ChkArg( f_dwSignedDataLength > 0 );
    ChkArg( f_cbCertificate > f_dwSignedDataLength );
    ChkArg( f_poSignatureInfo != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    if( f_pcontextBBX != NULL && f_pcontextSST != NULL && f_pDatastore != NULL )
    {
        if( DRM_CERTCACHE_VerifyCachedCertificate( f_pcontextBBX,
                                                   f_pcontextSST,
                                                   f_pDatastore,
                                                   f_pbCertificateBegin,
                                                   f_cbCertificate ) )
        {
            /* Signature was found in the cache */
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }
        /* Signature not found, we'll have to validate asymmetrically */
    }

    /*
    ** Verify signature
    */
    ChkDR( OEM_ECDSA_Verify_P256(
            f_pbCertificateBegin,
            f_dwSignedDataLength,
            f_poSignatureInfo->pIssuerKey,
            (SIGNATURE_P256 *)f_poSignatureInfo->SignatureData.pValue,
            (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ) );

    if( f_pcontextBBX != NULL && f_pcontextSST != NULL && f_pDatastore != NULL )
    {
        /* Signature checked out, add it to the cache */
        ChkDR( DRM_CERTCACHE_AddCachedCertificate( f_pcontextBBX,
                                                   f_pcontextSST,
                                                   f_pDatastore,
                                                   f_pbCertificateBegin,
                                                   f_cbCertificate ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/********************************************************************************************
** Function:    DRM_BCert_FindObjectInCertByType
**
** Synopsis:    The function enumerates all objects in a cert until it finds the one
**              with a matching type, or reaches end of data buffer, or the last object
**              in certificate which a signature object.
**
** Arguments:   [f_pbCertData]   : data buffer to parse
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from, should point
**                                 to the beginning of some object's header
**                                 or the beginning of the certificate header
**                                 (a special case since cert header is not an object).
**                                 On exit if succeeded this offset should point to the beginning
**                                 of an object (including its header!)
**              [f_dwObjectType] : object type
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_NOMORE
**              DRM_E_ARITHMETIC_OVERFLOW - comes from DRM_BCert_getObjectHeader
**              DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG - comes from DRM_BCert_getObjectHeader
********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_FindObjectInCertByType(
    __in_bcount(f_cbCertData) const DRM_BYTE    *f_pbCertData,
    __in                      const DRM_DWORD   f_cbCertData,
    __inout_ecount(1)         DRM_DWORD         *f_pbOffset,
    __in                      const DRM_DWORD   f_dwObjectType )
{
    DRM_RESULT              dr          = DRM_SUCCESS;
    DRM_DWORD               dwOffset    = 0;
    DRM_DWORD               dwHeaderTag = 0;
    DRM_BCERT_OBJECT_HEADER oObjHeader  = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_FindObjectInCertByType );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );
    ChkArg( f_dwObjectType <= DRM_BCERT_OBJTYPE_MAX_VALUE );

    /*
    ** Check for special case:
    ** offset could point to the beginning of the cert which is not an object.
    ** If header is found skip over it.
    */
    dwOffset = *f_pbOffset;
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwHeaderTag,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );
    if( dwHeaderTag == DRM_BCERT_HEADER_TAG )
    {
        ChkOverflow( *f_pbOffset + SIZEOF( DRM_BCERT_HEADER ), *f_pbOffset );
       *f_pbOffset += SIZEOF( DRM_BCERT_HEADER );
    }

    /*
    ** loop thru the objects until we reach
    ** end of data, the last object in cert,
    ** or object of sought type
    */
    dr = DRM_E_NOMORE;

    while ( *f_pbOffset < f_cbCertData )
    {
        ChkDR( DRM_BCert_getObjectHeader( f_pbCertData,
                                          f_cbCertData,
                                          f_pbOffset,
                                          &oObjHeader ) );

        if ( oObjHeader.wType == f_dwObjectType )
        {
            dr = DRM_SUCCESS;
            break;
        }
        /*
        ** This is the last object in a cert and we are not looking for signature object.
        */
        if ( oObjHeader.wType == DRM_BCERT_OBJTYPE_SIGNATURE )
        {
            dr = DRM_E_NOMORE;
            break;
        }
        /*
        ** go to the next object
        */
        ChkOverflow( *f_pbOffset + oObjHeader.cbLength - SIZEOF( DRM_BCERT_OBJECT_HEADER ), *f_pbOffset );
        *f_pbOffset += oObjHeader.cbLength - SIZEOF( DRM_BCERT_OBJECT_HEADER );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    DRM_BCert_GetChainHeader
**
** Synopsis:    Parses a certificate chain header.
**
** Arguments:   [f_pbCertData] : data buffer that is being parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_poChain]    : pointer to a chain header structure,
**                               output parameter, cannot be NULL
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_CHAIN_HEADER_TAG
**              DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH
**              DRM_E_BCERT_INVALID_CHAIN_VERSION
**              DRM_E_BCERT_INVALID_CHAIN_LENGTH
**
** Notes:  If you need to enumerate all certificates in a chain and read
**         their content into DRM_BCERT_CERTIFICATE structure call this API first
**         1) to ensure the data looks like a cert chain, 2) find out total number
**         of certs in it, 3) move an offset to the beginning of the first certificate.
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_GetChainHeader(
    __in_bcount(f_cbCertData) const DRM_BYTE                *f_pbCertData,
    __in                      const DRM_DWORD               f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                     *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_CHAIN_HEADER        *f_poChain )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetChainHeader );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( *f_pbOffset < f_cbCertData );
    ChkArg( f_poChain != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Fill in a chain header structure
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poChain->dwHeaderTag,                 /* tag */
            f_pbCertData,
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poChain->dwVersion,                   /* version */
            f_pbCertData,
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poChain->cbChain,                     /* chain length */
            f_pbCertData,
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poChain->dwFlags,                     /* flags*/
            f_pbCertData,
            dwOffset,
            f_cbCertData );
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poChain->cCerts,                      /* number of certs in chain */
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    /*
    ** Verify that header constant tag is correct
    */
    ChkBOOL( f_poChain->dwHeaderTag == DRM_BCERT_CHAIN_HEADER_TAG,
             DRM_E_BCERT_INVALID_CHAIN_HEADER_TAG );

    /*
    ** There should be 1-6 certificates in a chain
    */
    ChkBOOL( f_poChain->cCerts > 0,
             DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );
    ChkBOOL( f_poChain->cCerts <= DRM_BCERT_MAX_CERTS_PER_CHAIN,
             DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );
    /*
    ** Version is always 1
    */
    ChkBOOL( f_poChain->dwVersion == DRM_BCERT_CHAIN_VERSION,
             DRM_E_BCERT_INVALID_CHAIN_VERSION );
    /*
    ** Chain length in bytes cannot be too small
    */
    ChkBOOL( f_poChain->cbChain >= DRM_MIN_CERT_CHAIN_LEN
                                   + f_poChain->cCerts * DRM_MIN_CERTIFICATE_LEN,
             DRM_E_BCERT_INVALID_CHAIN_LENGTH );
    /*
    ** There is enough data in a buffer to accomodate a chain length value
    */
    ChkOverflow( *f_pbOffset + f_poChain->cbChain, f_poChain->cbChain );
    ChkBOOL( f_cbCertData >= *f_pbOffset + f_poChain->cbChain,
             DRM_E_BUFFERTOOSMALL );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _setVerificationContext
**
** Synopsis:    Checks consistency of verification context structure and zeroes
**              the array of return results.
**
** Arguments:   [f_pVerificationContext] : pointer to verification context
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
******************************************************************************/
static DRM_RESULT _setVerificationContext(
    __inout_ecount(1) DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pVerificationContext != NULL );

    ChkArg( f_pVerificationContext->dwType <= DRM_BCERT_CERTTYPE_MAX_VALUE );

    /*
    ** Crypto context cannot be NULL if the parser may be doing signature verification
    */
    ChkArg( f_pVerificationContext->pCryptoCtx != NULL
        || ( f_pVerificationContext->fVerifyCertSignatures == FALSE
            && f_pVerificationContext->fDontFailOnMissingExtData ) );

    f_pVerificationContext->cResults = 0;

    f_pVerificationContext->cCurrentCert = 0;

    f_pVerificationContext->dwCertificateChainExpiration = DRM_BCERT_DEFAULT_EXPIRATION_DATE;

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    DRM_BCert_ParseCertificateChain
**
** Synopsis:    The function parses binary data that represents device certificate chain
**              and verifies its validity. Optionally it can verify that all certs
**              are not expired, and the issuer key for of top certificate in a chain
**              matches specified value.
**
** Arguments:   [f_pbCertData]           : data buffer to parse
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pVerificationContext] : pointer to verification context structure that defines
**                                         optional verification conditions on the cert and may collect list of
**                                         verification errors not fatal for cert parser (i.e. parsing can continue).
**                                         Note that if on exit errors list is not empty this function will return
**                                         specific error code DRM_E_BCERT_VERIFICATION_ERRORS.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH
**              DRM_E_BCERT_VERIFICATION_ERRORS
******************************************************************************/
#ifdef PRDY_ROBUSTNESS
	DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_ParseCertificateChain(
		__in_bcount(f_cbCertData)			  const DRM_BYTE		  *f_pbCertData,
		__in								  const DRM_DWORD		  f_cbCertData,
		__inout 						DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
	{
		DRM_RESULT			   dr				= DRM_SUCCESS;
		
		DRM_BCERT_MINIMALDATA  * roCertData[2] = {NULL, NULL};
		DRM_DWORD			   bCurrOffset		= 0;
		DRM_BCERT_CHAIN_HEADER oChainHeader 	= { 0 };
	
		DRMFILETIME 		   ftCertTime		= { 0 };
		DRM_UINT64			   ui64CurrentTime	= DRM_UI64LITERAL( 0, 0 );
		DRM_UINT64			   uiCertTime		= DRM_UI64LITERAL( 0, 0 );
	
		roCertData[0] = Oem_MemAlloc(sizeof(DRM_BCERT_MINIMALDATA));
		roCertData[1] = Oem_MemAlloc(sizeof(DRM_BCERT_MINIMALDATA));
		memset(roCertData[0], 0, sizeof(DRM_BCERT_MINIMALDATA));
		memset(roCertData[1], 0, sizeof(DRM_BCERT_MINIMALDATA));
	
		DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_ParseCertificateChain );
	
		/* Refer to comments near definition of DRM_BCERT_MAX_KEY_USAGES in drmbcert.h */
		DRMCASSERT( DRM_BCERT_MAX_KEY_USAGES > DRM_BCERT_KEYUSAGE_MAX_VALUE );
		DRMCASSERT( DRM_BCERT_MAX_KEY_USAGES <= 32 );
	
		ChkArg( f_pbCertData != NULL );
		ChkArg( f_cbCertData > 0 );
		ChkDR( _setVerificationContext( f_pVerificationContext ) );
	
		/*
		** Parse a cert chain header
		*/
		ChkDR( DRM_BCert_GetChainHeader( f_pbCertData, f_cbCertData, &bCurrOffset, &oChainHeader ) );
	
		/*
		** Parse certificates in a chain
		*/
		for ( f_pVerificationContext->cCurrentCert = 0;
			  f_pVerificationContext->cCurrentCert < oChainHeader.cCerts;
			  f_pVerificationContext->cCurrentCert++ )
		{
			DRM_DWORD dwCertificateBegin = bCurrOffset;
	
			/*
			** Parse certificate and remember only data needed for verification purpose
			*/
			ChkDR( DRM_BCert_ParseRawCertificate( f_pbCertData,
									  f_cbCertData,
									  &bCurrOffset,
									  NULL,
									  roCertData[ f_pVerificationContext->cCurrentCert % 2 ],
									  f_pVerificationContext ) );
			/*
			** Calculate where certificate ends
			*/
			ChkOverflow( dwCertificateBegin + roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oCertHeader.cbCertificate, dwCertificateBegin );
			bCurrOffset = dwCertificateBegin + roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oCertHeader.cbCertificate;
	
			/*
			** Match leaf certificate type if it is provided
			*/
			if ( 0 == f_pVerificationContext->cCurrentCert
				&& f_pVerificationContext->dwType > 0 )
			{
				ChkBOOL( roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oBasicInfo.dwType == f_pVerificationContext->dwType,
						DRM_E_BCERT_INVALID_CERT_TYPE );
			}
			/*
			** Verify signature
			*/
			if( f_pVerificationContext->fVerifyCertSignatures )
			{
				ChkBoundsLT( dwCertificateBegin, f_cbCertData );
				ChkBoundsLT( roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oCertHeader.cbCertificateSigned, f_cbCertData - dwCertificateBegin );
				ChkDR( DRM_BCert_VerifySignature( f_pbCertData + dwCertificateBegin,
										 roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oCertHeader.cbCertificate,
										 roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oCertHeader.cbCertificateSigned,
										 &roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oSignatureInfo,
										 f_pVerificationContext->pCryptoCtx,
										 f_pVerificationContext->pBlackboxCtx,
										 f_pVerificationContext->pSecStoreCtx,
										 f_pVerificationContext->pDSTCtx ) );
			}
			/*
			** Check if the certificate is expired - optional
			*/
		#if 0 /* TODO - Playready */
			if ( DoExpirationCheck( f_pVerificationContext->ftExpirationTime ) )
			{
				FILETIME_TO_UI64( f_pVerificationContext->ftExpirationTime, ui64CurrentTime );
				CREATE_FILE_TIME( roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oBasicInfo.dwExpirationDate, ftCertTime );
				FILETIME_TO_UI64( ftCertTime, uiCertTime );
				ChkBOOL( !DRM_UI64Les( uiCertTime, ui64CurrentTime ),
						DRM_E_BCERT_BASICINFO_CERT_EXPIRED );
			}
		#endif
			/*
			** Additional verification between the certs
			*/
			if ( f_pVerificationContext->cCurrentCert > 0 )
			{
			
				ChkDR( _verifyAdjacentCerts( roCertData[(f_pVerificationContext->cCurrentCert + 1) % 2],
											 roCertData[f_pVerificationContext->cCurrentCert % 2],
											 f_pVerificationContext ) );
											 
			}
			/*
			** Check that Issuer key of the last cert is f_pRootPubkey.
			*/
			if ( f_pVerificationContext->pRootPubKey != NULL
				&& f_pVerificationContext->cCurrentCert == oChainHeader.cCerts - 1 )
			{
				ChkBOOL( DRM_BYT_CompareBytes(
							roCertData[ f_pVerificationContext->cCurrentCert % 2 ]->oSignatureInfo.pIssuerKey,
							0,
							f_pVerificationContext->pRootPubKey->m_rgbPubkey,
							0,
							SIZEOF( PUBKEY_P256 ) ) == 0,
						 DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH );
			}
	
			f_pVerificationContext->rgdwCertificateOffsets[f_pVerificationContext->cCurrentCert] = dwCertificateBegin;
		}
	
	ErrorExit:
		DRM_PROFILING_LEAVE_SCOPE;
	
		if(roCertData[0] != NULL) Oem_MemFree(roCertData[0]);
		if(roCertData[1] != NULL) Oem_MemFree(roCertData[1]);
	
		if ( f_pVerificationContext != NULL )
		{
			if ( f_pVerificationContext->cResults > 0 )
			{
				dr = DRM_E_BCERT_VERIFICATION_ERRORS;
			}
		}
		return dr;
	}
#else
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_ParseCertificateChain(
    __in_bcount(f_cbCertData)             const DRM_BYTE          *f_pbCertData,
    __in                                  const DRM_DWORD         f_cbCertData,
    __inout                         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT             dr               = DRM_SUCCESS;
    DRM_DWORD              bCurrOffset      = 0;
    DRM_BCERT_CHAIN_HEADER oChainHeader     = { 0 };
    DRM_BCERT_MINIMALDATA  roCertData[2]    = { 0, 0 };
    DRMFILETIME            ftCertTime       = { 0 };
    DRM_UINT64             ui64CurrentTime  = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64             uiCertTime       = DRM_UI64LITERAL( 0, 0 );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_ParseCertificateChain );

    /* Refer to comments near definition of DRM_BCERT_MAX_KEY_USAGES in drmbcert.h */
    DRMCASSERT( DRM_BCERT_MAX_KEY_USAGES > DRM_BCERT_KEYUSAGE_MAX_VALUE );
    DRMCASSERT( DRM_BCERT_MAX_KEY_USAGES <= 32 );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );

    ChkDR( _setVerificationContext( f_pVerificationContext ) );

    /*
    ** Parse a cert chain header
    */

    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData, f_cbCertData, &bCurrOffset, &oChainHeader ) );

    /*
    ** Parse certificates in a chain
    */

    for ( f_pVerificationContext->cCurrentCert = 0;
          f_pVerificationContext->cCurrentCert < oChainHeader.cCerts;
          f_pVerificationContext->cCurrentCert++ )
    {
        DRM_DWORD dwCertificateBegin = bCurrOffset;

        /*
        ** Parse certificate and remember only data needed for verification purpose
        */

        ChkDR( DRM_BCert_ParseRawCertificate( f_pbCertData,
                                              f_cbCertData,
                                              &bCurrOffset,
                                              NULL,
                                              &roCertData[ f_pVerificationContext->cCurrentCert % 2 ],
                                              f_pVerificationContext ) );


        /*
        ** Calculate where certificate ends
        */
        ChkOverflow( dwCertificateBegin + roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificate, dwCertificateBegin );
        bCurrOffset = dwCertificateBegin + roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificate;


        /*
        ** Match leaf certificate type if it is provided
        */
        if ( 0 == f_pVerificationContext->cCurrentCert
            && f_pVerificationContext->dwType > 0 )
        {

            ChkBOOL( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwType == f_pVerificationContext->dwType,
                    DRM_E_BCERT_INVALID_CERT_TYPE );
        }
        /*
        ** Verify signature
        */
        if( f_pVerificationContext->fVerifyCertSignatures )
        {
            ChkBoundsLT( dwCertificateBegin, f_cbCertData );
            ChkBoundsLT( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificateSigned, f_cbCertData - dwCertificateBegin );

            ChkDR( DRM_BCert_VerifySignature( f_pbCertData + dwCertificateBegin,
                                     roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificate,
                                     roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oCertHeader.cbCertificateSigned,
                                     &roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oSignatureInfo,
                                     f_pVerificationContext->pCryptoCtx,
                                     f_pVerificationContext->pBlackboxCtx,
                                     f_pVerificationContext->pSecStoreCtx,
                                     f_pVerificationContext->pDSTCtx ) );

        }
        /*
        ** Check if the certificate is expired - optional
        */
        if ( DoExpirationCheck( f_pVerificationContext->ftExpirationTime ) )
        {

            FILETIME_TO_UI64( f_pVerificationContext->ftExpirationTime, ui64CurrentTime );
            CREATE_FILE_TIME( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwExpirationDate, ftCertTime );
            FILETIME_TO_UI64( ftCertTime, uiCertTime );
            ChkBOOL( !DRM_UI64Les( uiCertTime, ui64CurrentTime ),
                    DRM_E_BCERT_BASICINFO_CERT_EXPIRED );

        }
        /*
        ** Update output value for when the chain expires
        */
        if( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwExpirationDate != 0
         && roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwExpirationDate  < f_pVerificationContext->dwCertificateChainExpiration )
        {

            f_pVerificationContext->dwCertificateChainExpiration = roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwExpirationDate;
        }
        /*
        ** Additional verification between the certs
        */
        if ( f_pVerificationContext->cCurrentCert > 0 )
        {

            ChkDR( _verifyAdjacentCerts( &roCertData[(f_pVerificationContext->cCurrentCert + 1) % 2],
                                         &roCertData[f_pVerificationContext->cCurrentCert % 2],
                                         f_pVerificationContext ) );

        }
        /*
        ** Check that Issuer key of the last cert is f_pRootPubkey.
        */
        if ( f_pVerificationContext->pRootPubKey != NULL
            && f_pVerificationContext->cCurrentCert == oChainHeader.cCerts - 1 )
        {

            ChkBOOL( DRM_BYT_CompareBytes(
                        roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oSignatureInfo.pIssuerKey,
                        0,
                        f_pVerificationContext->pRootPubKey->m_rgbPubkey,
                        0,
                        SIZEOF( PUBKEY_P256 ) ) == 0,
                     DRM_E_BCERT_ISSUERKEY_KEYINFO_MISMATCH );

        }

        f_pVerificationContext->rgdwCertificateOffsets[f_pVerificationContext->cCurrentCert] = dwCertificateBegin;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    if ( f_pVerificationContext != NULL )
    {
        if ( f_pVerificationContext->cResults > 0 )
        {
            dr = DRM_E_BCERT_VERIFICATION_ERRORS;
        }
    }
    return dr;
}
#endif

/***********************************************************************************************************
** Function:    DRM_BCert_GetPublicKeyByUsage
**
** Synopsis:    The function retrieves the public key of the cert with specified
**              key usage and index in a certificate chain.
**              On success returns a key value and an array of all key usages for the found key.
**              If key usage is set into DRM_BCERT_KEYUSAGE_UNKNOWN then the first public key will be returned.
**
** Arguments:   [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**              [f_dwCertIndex]     : index of certificate in the chain. Leaf cert is 0.
**              [f_dwKeyUsage]      : requested key usage of public key.
**              [f_pPubkey]         : pointer to a public key structure
**              [f_pdwKeyUsageSet]  : key usage set of the returned key.
**                                    This parameter is optional and can be NULL.
**              [f_pbCertStartOffset] : Optionally returns the index of the start
**                                      of the requested certificate
**              [f_pdwCertKeyIndex] : Optionally returns the index of the requested key
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
************************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_GetPublicKeyByUsage(
    __in_bcount(f_cbCertData)           const DRM_BYTE    *f_pbCertData,
    __in                                const DRM_DWORD    f_cbCertData,
    __in                                const DRM_DWORD    f_dwCertIndex,
    __in                                const DRM_DWORD    f_dwKeyUsage,
    __inout_ecount(1)                   PUBKEY_P256       *f_pPubkey,
    __out_opt                           DRM_DWORD         *f_pdwKeyUsageSet,
    __out_ecount_opt(1)                 DRM_DWORD         *f_pbCertStartOffset,
    __out_ecount_opt(1)                 DRM_DWORD         *f_pdwCertKeyIndex )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr               = DRM_SUCCESS;
    DRM_DWORD               bCurrOffset      = 0;
    DRM_BCERT_CHAIN_HEADER  oChainHeader     = {0};
    DRM_DWORD               iCertCounter     = 0;
    DRM_DWORD               iKeyCounter      = 0;
    DRM_BOOL                fFoundKey        = FALSE;
    DRM_DWORD               iFoundKey        = 0;
    DRM_BCERT_KEY_INFO      oKeyInfo         = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_DWORD               dwBitMask        = (DRM_DWORD)BCERT_KEYUSAGE_BIT( f_dwKeyUsage );
    DRM_DWORD               bCertStartOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetPublicKeyByUsage );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pPubkey != NULL );

    /*
    ** Parse a cert chain header and make sure
    ** that there are enough certs in the chain.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData,
                                     f_cbCertData,
                                     &bCurrOffset,
                                     &oChainHeader ) );

    ChkBOOL( oChainHeader.cCerts > f_dwCertIndex, DRM_E_BCERT_INVALID_CHAIN_LENGTH );

    /*
    ** bCurrOffset currently points to the 0-th leaf cert.
    ** Loop thru certificates if needed.
    ** Do not parse, only look for the start position.
    */
    for ( iCertCounter = 1; iCertCounter <= f_dwCertIndex; iCertCounter++ )
    {
        ChkDR( DRM_BCert_GetCertificate( f_pbCertData,
                                         f_cbCertData,
                                         &bCurrOffset,
                                         NULL,
                                         DRM_BCERT_CERTTYPE_UNKNOWN ) );
    }

    bCertStartOffset = bCurrOffset;

    OEM_ECC_ZeroPublicKey_P256( f_pPubkey );

    /*
    ** Loop thru the objects until we find
    ** a key information or reach the end
    */
    ChkDR( DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                             f_cbCertData,
                                             &bCurrOffset,
                                             DRM_BCERT_OBJTYPE_KEY ) );

    ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_UNKNOWN ,
                                              NULL,
                                              FALSE,
                                              TRUE,
                                              NULL,
                                              0,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_parseKeyInfo( f_pbCertData,
                                   f_cbCertData,
                                   &bCurrOffset,
                                   DRM_BCERT_CERTTYPE_UNKNOWN,
                                   &oKeyInfo,
                                   &oVerificationContext ) );

    if ( f_dwKeyUsage != DRM_BCERT_KEYUSAGE_UNKNOWN )
    {
        /*
        ** Find the first key with requested key usage
        */
        for ( iKeyCounter = 0; iKeyCounter < oKeyInfo.dwNumKeys; iKeyCounter++ )
        {
            if ( ( dwBitMask & ( oKeyInfo.rgoKeys[ iKeyCounter ].dwUsageSet ) ) != 0  )
            {
                iFoundKey = iKeyCounter;
                fFoundKey = TRUE;

                break;
            }
        }
    }
    else
    {
        iFoundKey = 0; /* if any keyusage is requested just get the first public key */
        fFoundKey = TRUE;
    }

    /*
    ** If the loop is finished then the key with requested usage was not found
    */
    ChkBOOL( fFoundKey, DRM_E_BCERT_NO_PUBKEY_WITH_REQUESTED_KEYUSAGE );

    /*
    ** Copy key usage bitmaps.
    */
    if ( f_pdwKeyUsageSet != NULL )
    {
        *f_pdwKeyUsageSet = oKeyInfo.rgoKeys[iFoundKey].dwUsageSet;
    }

    OEM_SECURE_MEMCPY( f_pPubkey->m_rgbPubkey,
                       oKeyInfo.rgoKeys[iFoundKey].pValue,
                       SIZEOF( PUBKEY_P256 ) );

    /*
    ** Return optional data, if required
    */
    if( f_pbCertStartOffset != NULL )
    {
        *f_pbCertStartOffset = bCertStartOffset;
    }

    if( f_pdwCertKeyIndex != NULL )
    {
        *f_pdwCertKeyIndex = iFoundKey;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/***********************************************************************************************************
** Function:    _GetCachedPublicKeyByUsage
**
** Synopsis:    The function retrieves the public key of the cert with specified
**              key usage and index in a certificate chain, along with it's key handle if necessary
**              On success returns a key value and an array of all key usages for the found key.
**              If key usage is set into DRM_BCERT_KEYUSAGE_UNKNOWN then the first public key will be returned.
**
** Arguments:   [f_pBlackboxContext]: optional blackbox context
                [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**              [f_dwCertIndex]     : index of certificate in the chain. Leaf cert is 0.
**              [f_dwKeyUsage]      : requested key usage of public key.
**              [f_pPubkey]         : pointer to a public key structure
**              [f_hSigningKey]     : handle to the signing key of this certificate. Must be specified if f_phPubkey 
**                                    is not NULL. 
**              [f_phPubkey]        : pointer to a key handle to fill with a BBX validated key handle. Can be NULL
**              [f_hSigningKey]     : the key that signs the supplied certificate
**              [f_pdwKeyUsageSet]  : key usage set of the returned key.
**                                    This parameter is optional and can be NULL.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
************************************************************************************************************/
static DRM_RESULT _GetCachedPublicKeyByUsage(
    __inout_ecount_opt(1)               DRM_BB_CONTEXT             *f_pBlackboxContext,
    __in_ecount_opt(1)            const DRM_BB_KEYHANDLE_CALLBACKS *f_pKeyHandleCallbacks,
    __in_bcount(f_cbCertData)     const DRM_BYTE                   *f_pbCertData,
    __in                          const DRM_DWORD                   f_cbCertData,
    __in                          const DRM_DWORD                   f_dwKeyUsage,
    __inout_ecount(1)                   DRM_PUBLIC_KEY_CONTEXT     *f_pPubkeyContext,
    __in                                OEM_CRYPTO_HANDLE           f_hSigningKey,
    __out_ecount_opt(1)                 DRM_DWORD                  *f_pdwKeyUsageSet )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr               = DRM_SUCCESS;
    DRM_DWORD               iFoundKey        = 0;
    DRM_DWORD               bCertStartOffset = 0;

    ChkArg( f_pBlackboxContext != NULL );
    ChkArg( f_pbCertData       != NULL );
    ChkArg( f_cbCertData       > 0 );
    ChkArg( f_pPubkeyContext   != NULL );
    ChkArg( ( f_pKeyHandleCallbacks == NULL ) == ( f_hSigningKey == OEM_CRYPTO_HANDLE_INVALID ) );

    ChkDR( DRM_BCert_GetPublicKeyByUsage( f_pbCertData,
                                          f_cbCertData,
                                          0,
                                          f_dwKeyUsage,
                                          &f_pPubkeyContext->PublicKey,
                                          f_pdwKeyUsageSet,
                                          &bCertStartOffset,
                                          &iFoundKey ) );

    /*
    ** If a key handle pointer has been supplied, load the public key into a
    ** register by verifying the certificate
    */
    if( f_pKeyHandleCallbacks != NULL )
    {
        ChkDR( f_pKeyHandleCallbacks->pfUnwrapPublicKeyHandle( f_pBlackboxContext,
                                                               f_hSigningKey,
                                                               f_pbCertData + bCertStartOffset,
                                                               f_cbCertData - bCertStartOffset,
                                                               iFoundKey,
                                                               f_pPubkeyContext ) );
    }

ErrorExit:
    return dr;
}


/********************************************************************************************
** Function:    DRM_BCert_GetPublicKey
**
** Synopsis:    The function retrieves the public key of the specified cert
**              in a certificate chain. If there are several public keys in the cert
**              the first one will be returned.
**
** Arguments:   [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**              [f_dwCertIndex]     : index of certificate in the chain. Leaf cert is 0.
**              [f_pPubkey]         : pointer to a public key structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_GetPublicKey(
    __in_bcount(f_cbCertData) const DRM_BYTE    *f_pbCertData,
    __in                      const DRM_DWORD    f_cbCertData,
    __in                      const DRM_DWORD    f_dwCertIndex,
    __inout_ecount(1)         PUBKEY_P256       *f_pPubkey )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr             = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetPublicKey );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pPubkey != NULL );

    ChkDR( DRM_BCert_GetPublicKeyByUsage( f_pbCertData,
                                          f_cbCertData,
                                          f_dwCertIndex,
                                          DRM_BCERT_KEYUSAGE_UNKNOWN,
                                          f_pPubkey,
                                          NULL,
                                          NULL,
                                          NULL ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/********************************************************************************************
** Function:    DRM_BCert_GetSecurityVersion
**
** Synopsis:    This function retrieves the first occurance of Security Version information from
**              the cert chain walking up the chain starting from the leaf-most cert in a chain.
**
** Arguments:   [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**       [f_pdwSecurityVersion]     : pointer to a dword to retrieve the Security Version
**    [f_pdwPlatformIdentifier]     : pointer to a dword to retrieve the Platform ID.
**
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_S_FALSE      - No Security Version object is present, but no error occurred
**              DRM_E_INVALIDARG
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_GetSecurityVersion(
    __in_bcount(f_cbCertData) const DRM_BYTE      *f_pbCertData,
    __in                      const DRM_DWORD      f_cbCertData,
    __out_ecount(1)                 DRM_DWORD     *f_pdwSecurityVersion,
    __out_ecount(1)                 DRM_DWORD     *f_pdwPlatformIdentifier )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                    dr           = DRM_SUCCESS;
    DRM_DWORD                     bCurrOffset  = 0;
    DRM_BCERT_CHAIN_HEADER        oChainHeader = {0};
    DRM_DWORD                     iCertCounter = 0;
    DRM_BCERT_SECURITY_VERSION    oSecVersion2  = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_DWORD                     savedCurrOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetSecurityVersion );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pdwSecurityVersion    != NULL );
    ChkArg( f_pdwPlatformIdentifier != NULL );

    /*
    ** Parse a cert chain header and make sure
    ** that there are enough certs in the chain.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData,
                                     f_cbCertData,
                                     &bCurrOffset,
                                     &oChainHeader ) );

    /*
    ** bCurrOffset currently points to the 0-th leaf cert.
    ** Loop thru certificates till we find a cert with Security version2 object
    ** or a security version object.
    */
    for ( iCertCounter = 0; iCertCounter < oChainHeader.cCerts && dr != DRM_S_FALSE; iCertCounter++ )
    {
        savedCurrOffset = bCurrOffset;
        dr = DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                               f_cbCertData,
                                              &bCurrOffset,
                                               DRM_BCERT_OBJTYPE_SECURITY_VERSION_2 );
        if ( DRM_E_NOMORE == dr )
        {
            bCurrOffset = savedCurrOffset;
            /*
            ** Loop thru the objects until we find
            ** a key information or reach the end
            */
            dr = DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                                   f_cbCertData,
                                                  &bCurrOffset,
                                                   DRM_BCERT_OBJTYPE_SECURITY_VERSION );
        }

        if( DRM_SUCCESS == dr )
        {
            break;
        }
        dr = DRM_SUCCESS; /* Ignore errors and move on to next certificate */

        bCurrOffset = savedCurrOffset;
        ChkDR( DRM_BCert_GetCertificate( f_pbCertData,
                                         f_cbCertData,
                                         &bCurrOffset,
                                         NULL,
                                         DRM_BCERT_CERTTYPE_UNKNOWN ) );

    }

    if (DRM_S_FALSE == dr)
    {   /* We reached the end of the chain so return S_FALSE*/
        *f_pdwPlatformIdentifier = 0;
        *f_pdwSecurityVersion = 0;
        goto ErrorExit;
    }

    ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_UNKNOWN ,
                                              NULL,
                                              FALSE,
                                              TRUE,
                                              NULL,
                                              0,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_parseSecurityVersion( f_pbCertData,
                                           f_cbCertData,
                                           &bCurrOffset,
                                           NULL,
                                           &oSecVersion2,
                                           &oVerificationContext ) );

    *f_pdwSecurityVersion = oSecVersion2.dwSecurityVersion;
    *f_pdwPlatformIdentifier = oSecVersion2.dwPlatformIdentifier;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/********************************************************************************************
** Function:    DRM_BCert_GetManufacturerStrings
**
** Synopsis:    This function retrieves the first occurance of manufacturer strings from
**              the cert chain walking up the chain starting from the leaf-most cert in a chain.
**
** Arguments:   [f_pbCertData]      : data buffer to parse
**              [f_cbCertData]      : size of the data buffer (in bytes)
**    [f_pManufacturerStrings]      : pointer to a DRM_BCERT_MANUFACTURER_STRINGS.
**
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_S_FALSE      - No manufacturer info object is present, but no error occurred
**              DRM_E_INVALIDARG
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Some basic validation still occurs.
********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BCert_GetManufacturerStrings(
    __in_bcount(f_cbCertData) const DRM_BYTE      *f_pbCertData,
    __in                      const DRM_DWORD      f_cbCertData,
    __out_ecount(1)                 DRM_BCERT_MANUFACTURER_STRINGS *f_pManufacturerStrings )
{
    DRM_RESULT                    dr           = DRM_SUCCESS;
    DRM_DWORD                     bCurrOffset  = 0;
    DRM_BCERT_CHAIN_HEADER        oChainHeader = {0};
    DRM_DWORD                     iCertCounter = 0;
    DRM_BCERT_MANUFACTURER_INFO   oManufacturerInfo  = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_BOOL                      fFound       = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetManufacturerStrings );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pManufacturerStrings != NULL );

    /*
    ** Parse a cert chain header and make sure
    ** that there are enough certs in the chain.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData,
                                     f_cbCertData,
                                     &bCurrOffset,
                                     &oChainHeader ) );

    /*
    ** bCurrOffset currently points to the 0-th leaf cert.
    ** Loop thru certificates till we find a cert with manufacturer object.
    */
    for ( iCertCounter = 0; iCertCounter < oChainHeader.cCerts; iCertCounter++ )
    {
        dr = DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                               f_cbCertData,
                                              &bCurrOffset,
                                               DRM_BCERT_OBJTYPE_MANUFACTURER );

        if( DRM_SUCCESS == dr )
        {
            fFound = TRUE;
            break;
        }
        dr = DRM_SUCCESS; /* Ignore errors and move on to next certificate */

        ChkDR( DRM_BCert_GetCertificate( f_pbCertData,
                                         f_cbCertData,
                                         &bCurrOffset,
                                         NULL,
                                         DRM_BCERT_CERTTYPE_UNKNOWN ) );

    }

    if (!fFound)
    {   /* We reached the end of the chain so return S_FALSE*/
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                              NULL,
                                              DRM_BCERT_CERTTYPE_UNKNOWN ,
                                              NULL,
                                              FALSE,
                                              TRUE,
                                              NULL,
                                              0,
                                              FALSE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );

    ChkDR( DRM_BCert_parseManufacturerInfo( f_pbCertData,
                                            f_cbCertData,
                                            &bCurrOffset,
                                            &oManufacturerInfo,
                                            &oVerificationContext ) );

    *f_pManufacturerStrings = oManufacturerInfo.ManufacturerStrings;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************************
** Function:    DRM_BCert_LoadPropertiesCache
**
** Synopsis:    Fills in cachedBCertValues structure in DRM_BB_CONTEXT.
**              Function takes device certificate chain and reads thru a leaf cert
**              and group cert which is one level up.
**
** Arguments:   [f_pbCertData]          : data buffer to parse
**              [f_cbCertData]          : size of the data buffer (in bytes)
**              [f_pCachedValues]       : pointer to the structure to store cached data from the cert.
**              [f_pKeyHandleCallbacks] : pointer to handle management functions, if supported by
**                                        this black box
**              [f_pBlackboxContext]    : pointer to an initialized blackbox context
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_INVALID_CHAIN_LENGTH
**              DRM_E_BCERT_INVALID_CERT_TYPE
**
** Note: If f_fCacheHandles == FALSE we assume that this data is valid so that we 
**       don't have to parse it again.
         If f_fCacheHandles == TRUE signature and other validity checks
**       are carried out in the blackbox for all keys, and key handles will be allocated
**       which must be cleared using DRM_BCert_CleanupPropertiesCache
**       Function should only be called for device certificates.
***********************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_LoadPropertiesCache(
   __in_bcount(f_cbCertData)        const DRM_BYTE                             *f_pbCertData,
   __in                             const DRM_DWORD                             f_cbCertData,
   __inout                                DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues,
   __in_ecount_opt(1)               const DRM_BB_KEYHANDLE_CALLBACKS           *f_pKeyHandleCallbacks,
   __inout_ecount(1)                      DRM_BB_CONTEXT                       *f_pBlackboxContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr               = DRM_SUCCESS;
    DRM_DWORD               bLeafCertStart   = 0;
    DRM_DWORD               bGroupCertStart  = 0;
    DRM_DWORD               dwOffset         = 0;
    DRM_DWORD               dwNumFeatures    = 0;
    DRM_DWORD               dwCount          = 0;
    DRM_BCERT_CHAIN_HEADER  oChainHeader     = { 0 };
    DRM_DWORD               dwKeyUsageSet    = 0 ;
    OEM_CRYPTO_HANDLE       hSigningKey      = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pCachedValues != NULL );
    ChkArg( f_pBlackboxContext != NULL );

    /*
    ** Parse a cert chain header and make sure
    ** that there are at least two certs in the chain.
    */
    ChkDR( DRM_BCert_GetChainHeader( f_pbCertData,
                                     f_cbCertData,
                                     &bLeafCertStart,
                                     &oChainHeader ) );

    ChkBOOL( oChainHeader.cCerts >= 2, DRM_E_BCERT_INVALID_CHAIN_LENGTH );

    /*
    ** Find the beginning of the next certificate
    */
    bGroupCertStart = bLeafCertStart;
    ChkDR( DRM_BCert_GetCertificate( f_pbCertData,
                                     f_cbCertData,
                                     &bGroupCertStart,
                                     NULL,
                                     DRM_BCERT_CERTTYPE_DEVICE ) );

    /*
    ** Read device serial number
    */
    dwOffset = bLeafCertStart
             + SIZEOF( DRM_BCERT_HEADER )       /* certificate header */
             + SIZEOF( DRM_BCERT_OBJECT_HEADER) /* basic info's object header */
             + DRM_BCERT_CERT_ID_LENGTH         /* cert ID */
             + SIZEOF( DRM_DWORD )              /* security level */
             + SIZEOF( DRM_DWORD )              /* flags */
             + SIZEOF( DRM_DWORD )              /* cert type */
             + SHA256_DIGEST_SIZE_IN_BYTES      /* digest */
             + SIZEOF( DRM_DWORD );             /* expiration */

    /*
    ** If the cache must include handles, then invoke the handle callbacks, otherwise 
    ** just zero the cache memory
    */
    if( f_pKeyHandleCallbacks != NULL )
    {
        ChkDR( f_pKeyHandleCallbacks->pfInitializeCaching( f_pBlackboxContext,
                                                           f_pCachedValues,
                                                           f_pbCertData,
                                                           f_cbCertData,
                                                           oChainHeader.cCerts,
                                                           bLeafCertStart,
                                                           &hSigningKey ) );
    }
    else
    {
        OEM_SECURE_ZERO_MEMORY( f_pCachedValues,
                                SIZEOF( DRM_BINARY_DEVICE_CERT_CACHED_VALUES ) );
    }

    COPY_FROMBUFFER(
            f_pCachedValues->DeviceSerialNumber.rgb,
            f_pbCertData,
            dwOffset,
            DRM_BCERT_CLIENT_ID_LENGTH,
            f_cbCertData );

    /*
    ** read public keys for signing and encryption
    */
    ChkDR( _GetCachedPublicKeyByUsage( f_pBlackboxContext,
                                       f_pKeyHandleCallbacks,
                                       f_pbCertData,
                                       f_cbCertData,
                                       DRM_BCERT_KEYUSAGE_SIGN,
                                       &f_pCachedValues->DeviceKeySign.PublicKeyContext,
                                       hSigningKey,
                                       &dwKeyUsageSet ) );

    DRMASSERT( dwKeyUsageSet > 0 ); /* at least one key usage per key, and there should be two keys in the cert */

    ChkDR( _GetCachedPublicKeyByUsage( f_pBlackboxContext,
                                       f_pKeyHandleCallbacks,
                                       f_pbCertData,
                                       f_cbCertData,
                                       DRM_BCERT_KEYUSAGE_ENCRYPT_KEY,
                                       &f_pCachedValues->DeviceKeyEncrypt.PublicKeyContext,
                                       hSigningKey,
                                       &dwKeyUsageSet ) );

    DRMASSERT( dwKeyUsageSet > 0 );

    dr = _GetCachedPublicKeyByUsage( f_pBlackboxContext,
                                     f_pKeyHandleCallbacks,
                                     f_pbCertData,
                                     f_cbCertData,
                                     DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY,
                                     &f_pCachedValues->DeviceKeyPrndEncrypt.PublicKeyContext,
                                     hSigningKey,
                                     &dwKeyUsageSet );

    if( DRM_SUCCEEDED( dr ) )
    {
        f_pCachedValues->fDeviceKeyPrndEncryptSet = TRUE;
    }
    else if( dr == DRM_E_BCERT_NO_PUBKEY_WITH_REQUESTED_KEYUSAGE )
    {
        /* PRND Encrypt Key KeyUsage is optional. */
        f_pCachedValues->fDeviceKeyPrndEncryptSet = FALSE;
        dr = DRM_SUCCESS;
    }
    ChkDR( dr );

    /*
    ** move to the next cert in the chain and read security level
    */
    dwOffset = bGroupCertStart                   /* the beginning of the next cert */
             + SIZEOF( DRM_BCERT_HEADER )        /* cert header */
             + SIZEOF( DRM_BCERT_OBJECT_HEADER ) /* basic info object header */
             + DRM_BCERT_CERT_ID_LENGTH;         /* cert ID */

    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_pCachedValues->dwGroupSecurityLevel,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    /*
    ** Move to the beginning of a group cert and find feature info
    */
    dwOffset = bGroupCertStart;

    ChkDR( DRM_BCert_FindObjectInCertByType( f_pbCertData,
                                             f_cbCertData,
                                             &dwOffset,
                                             DRM_BCERT_OBJTYPE_FEATURE ) );

    /*
    ** Read total number of features
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            dwNumFeatures,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    f_pCachedValues->dwFeatureFlags = 0;
    for ( dwCount = 0; dwCount < dwNumFeatures; dwCount++ )
    {
        DRM_DWORD dwFeature = 0;
        NETWORKBYTES_FROMBUFFER_TO_DWORD(
                dwFeature,
                f_pbCertData,
                dwOffset,
                f_cbCertData );
        /*
        ** convert into a bit mask and save
        */
        f_pCachedValues->dwFeatureFlags |= BCERT_FEATURE_BIT( dwFeature );
    }

ErrorExit:

    /*
    ** Do any cleanup required
    */
    if( f_pKeyHandleCallbacks != NULL )
    {
        f_pKeyHandleCallbacks->pfCompleteCaching( f_pBlackboxContext,
                                                  f_pCachedValues,
                                                  dr,
                                                  &hSigningKey );
    }
    else
    {
        if( DRM_FAILED( dr ) )
        {
            OEM_SECURE_ZERO_MEMORY( f_pCachedValues,
                                    SIZEOF( DRM_BINARY_DEVICE_CERT_CACHED_VALUES ) );
        }
    }

    return dr;
}


/*********************************************************************************
** Function:    DRM_BCert_CleanupPropertiesCache
**
** Synopsis:    Clean up the properties cache, releasing any held resources.
**
** Arguments:   [f_pCachedValues]       : pointer to the structure to store cached data from the cert.
**              [f_pKeyHandleCallbacks] : pointer to handle management functions, if supported by
**                                        this black box
**              [f_pBlackboxContext]    : pointer to an initialized blackbox context
**
**
** Note: 
**
***********************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_BCert_CleanupPropertiesCache(
   __inout                   DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues,
   __in_ecount_opt(1)  const DRM_BB_KEYHANDLE_CALLBACKS           *f_pKeyHandleCallbacks,
   __inout_ecount(1)         DRM_BB_CONTEXT                       *f_pBlackboxContext )
{
    /*
    ** Do any cleanup required
    */
    if( f_pKeyHandleCallbacks != NULL )
    {
        f_pKeyHandleCallbacks->pfCleanupCache( f_pBlackboxContext,
                                               f_pCachedValues );
    }
    else
    {
        OEM_SECURE_ZERO_MEMORY( f_pCachedValues,
                                SIZEOF( DRM_BINARY_DEVICE_CERT_CACHED_VALUES ) );
    }
}


EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;
