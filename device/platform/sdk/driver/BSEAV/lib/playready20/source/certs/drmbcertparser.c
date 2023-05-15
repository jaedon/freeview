/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmbcertparser.h>
#include <drmbcertparsercache.h>
#include <drmutf.h>
#include <drmcertcache.h>

ENTER_PK_NAMESPACE_CODE;

#define DoExpirationCheck(ft) ( ( (ft).dwLowDateTime != 0 ) && ( (ft).dwHighDateTime != 0 ) )

#define ChkVerificationErr( pVerificationCtx, fCondition, dwErr )                                                   \
do {                                                                                                                \
    if ( !(fCondition) )                                                                                            \
    {                                                                                                               \
        if ( pVerificationCtx->fCollectErrorsInArrayAndContinue == FALSE                                            \
            || pVerificationCtx->cResults >= DRM_MAX_VERIFICATIONRESULTS )                                          \
        {                                                                                                           \
            ChkDR(dwErr);                                                                                           \
        }                                                                                                           \
        else                                                                                                        \
        {                                                                                                           \
            pVerificationCtx->rgoResults[pVerificationCtx->cResults].cCertNumber = pVerificationCtx->cCurrentCert;  \
            pVerificationCtx->rgoResults[pVerificationCtx->cResults++].dwResult = (dwErr);                          \
        }                                                                                                           \
    }                                                                                                               \
} while(FALSE)



#define BCERT_KEYUSAGE_PARENT_ISSUERS_MASK                          \
    ( BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_INDIV )         \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_DEVICE )        \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_LINK )          \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN )        \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT )   \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION )   \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_CRL )           \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE )  \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_METERING )      \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SIGN_LICENSE ) )


/*****************************************************************************
** Function:    _isUTF8NullTerminatedData
**
** Synopsis:    Verify that byte buffer contains a null-terminated UTF-8 string.
**
** Arguments:   [f_pbData]   : pointer to byte data buffer
**              [f_cbData]   : size in bytes of f_pbData
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_BCERT_STRING_NOT_NULL_TERMINATED
**              DRM_E_UTF8_xxxx - if a string is not UTF-8
**
** Notes:       Strings of length 1 that contain no data and one null are okay.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _isUTF8NullTerminatedData(
    __in_bcount(f_cbData) const DRM_BYTE  *f_pbData,
    __in                  DRM_DWORD       f_cbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );

    ChkBOOL( GET_BYTE( f_pbData, f_cbData - 1 ) == 0x0, DRM_E_BCERT_STRING_NOT_NULL_TERMINATED );

    if ( f_cbData > 1 )
    {
        ChkDR( DRM_UTF8_VerifyBytes( f_pbData,
                                     f_cbData - 1,
                                     0,
                                     f_cbData - 2 ) );
    }

ErrorExit:
    return dr;
}

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
    ChkVerificationErr( f_pVerificationContext,
                        f_poParentCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_ISSUER,
                        DRM_E_BCERT_INVALID_CERT_TYPE );

    /*
    ** Security level of the parent cert is higher or equal to its child cert's.
    */
    ChkVerificationErr( f_pVerificationContext,
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
        ChkVerificationErr( f_pVerificationContext, FALSE, dr);
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
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_DEVICE )
        {
            /*
            ** Issuer-Device can issue certificate of Type Device.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_PC )
        {
            /*
            ** Issuer-Indiv can issue certificate of Type PC.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_INDIV),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SILVERLIGHT )
        {
            /*
            ** Issuer-SilverLight can issue certificate of Type SilverLight.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_APPLICATION )
        {
            /*
            ** Issuer-Application can issue certificate of Type Application.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_METERING)
        {
            /*
            ** Issuer-Metering can issue certificate of Type Metering.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_METERING),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_KEYFILESIGNER )
        {
            /*
            ** Issuer-SignKeyFile can issue certificate of Type KeyFileSigner
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVER )
        {
            /*
            ** Issuer-Server can issue certificate of Type Server
            */
            ChkVerificationErr( f_pVerificationContext,
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
            ChkVerificationErr( f_pVerificationContext,
                !( dwChildKeyUsageMask & BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_ALL ) ),
                DRM_E_BCERT_INVALID_KEY_USAGE );
            /*
            ** Check key usages in a child cert:
            ** the parent cert is not Issuer-All and must have all issuer-... key usages that child cert has
            */
            ChkVerificationErr( f_pVerificationContext,
                    ( dwChildKeyUsageMask & BCERT_KEYUSAGE_PARENT_ISSUERS_MASK )
                        <= ( dwParentKeyUsageMask & BCERT_KEYUSAGE_PARENT_ISSUERS_MASK ),
                    DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_LICENSESIGNER )
        {
            /*
            ** Issuer-SignLicense can issue certificate of Type LicenseSigner
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_SIGN_LICENSE),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else
        {
            /*
            ** Service cert type. If it has a parent cert it can only be Issuer_All cert, so fail
            */
            ChkVerificationErr( f_pVerificationContext,
                                f_poChildCert->oBasicInfo.dwType == DRM_BCERT_CERTTYPE_SERVICE,
                                DRM_E_BCERT_INVALID_CERT_TYPE );

            ChkVerificationErr( f_pVerificationContext, FALSE, DRM_E_BCERT_INVALID_KEY_USAGE );
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
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_LINK),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
        }
        else if ( f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_SECURE_CLOCK )
        {
            /*
            ** SecureClock can be issued by Issuer-Device.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
            fSupportSecureClock = TRUE;
        }
        else if ( f_poChildCert->oFeatureInfo.rgdwFeatureSet[ iCount ] == DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK )
        {
            /*
            ** Anti-Rollback Clock can be issued by Issuer-Device.
            */
            ChkVerificationErr( f_pVerificationContext,
                                dwParentKeyUsageMask & BCERT_KEYUSAGE_BIT(DRM_BCERT_KEYUSAGE_ISSUER_DEVICE),
                                DRM_E_BCERT_INVALID_KEY_USAGE );
            fSupportAntiRollbackClock = TRUE;
        }
    }

    /*
    ** Either SecureClock or Anti-Rollback Clock can be issued in one certificate, never both.
    */
    ChkVerificationErr( f_pVerificationContext,
                        !( fSupportSecureClock && fSupportAntiRollbackClock ),
                        DRM_E_BCERT_INVALID_KEY_USAGE );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    _getObjectHeader
**
** Synopsis:    Parses an object header. If object type is unknown and not
**              "must understand" function locates the next object header it can read.
**
** Arguments:   [f_pbCertData] : data buffer that is parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_poHeader]   : pointer to an object header structure, cannot be NULL
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if either f_pbOffset or an object length is too big
**              DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG - object length value is too big
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _getObjectHeader(
    __in_bcount(f_cbCertData) const DRM_BYTE          *f_pbCertData,
    __in                      const DRM_DWORD         f_cbCertData,
    __inout_ecount(1)         DRM_DWORD               *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_OBJECT_HEADER *f_poHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_getObjectHeader );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_pbOffset != NULL );
    ChkArg( *f_pbOffset < f_cbCertData );
    ChkArg( f_poHeader != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read all object header attributes
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poHeader->wFlags,
                                     f_pbCertData,
                                     dwOffset,
                                     f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_WORD( f_poHeader->wType,
                                     f_pbCertData,
                                     dwOffset,
                                     f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poHeader->cbLength,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );
    /*
    ** Object length should be small enough and inside data buffer.
    ** Note that object header is included into object length,
    ** so object length should also be at least the size of object header.
    */
    ChkOverflow( *f_pbOffset + f_poHeader->cbLength, *f_pbOffset );
    ChkBOOL( *f_pbOffset + f_poHeader->cbLength <= f_cbCertData,
             DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG );

    ChkBOOL( SIZEOF( DRM_BCERT_OBJECT_HEADER ) <= f_poHeader->cbLength,
             DRM_E_BCERT_OBJECTHEADER_LEN_TOO_SMALL );

    if ( f_poHeader->wType > DRM_BCERT_OBJTYPE_MAX_VALUE )
    {
        /*
        ** If object's type is unknown but flag is not "must understand"
        ** try to skip it and read the next one
        */
        ChkBOOL( f_poHeader->wFlags != DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                 DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        *f_pbOffset += f_poHeader->cbLength;

        ChkDR( _getObjectHeader( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 f_poHeader ) );
    }
    else
    {
        /*
        ** move current offset past object header and stop reading
        */
        ChkBoundsLT( dwOffset, f_cbCertData );
        *f_pbOffset = dwOffset;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    _parseCertHeader
**
** Synopsis:    Parses a certificate's header.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poCertHeader] : pointer to a certificate header structure, cannot be NULL
**      [f_pVerificationContext] : pointer to verification context structure
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
static DRM_NO_INLINE DRM_RESULT _parseCertHeader(
    __in_bcount(f_cbCertData) const DRM_BYTE   *f_pbCertData,
    __in                      const DRM_DWORD   f_cbCertData,
    __inout_ecount(1)         DRM_DWORD        *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_HEADER *f_poCertHeader,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseCertHeader );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poCertHeader != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->dwHeaderTag,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->dwVersion,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->cbCertificate,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );

    NETWORKBYTES_FROMBUFFER_TO_DWORD( f_poCertHeader->cbCertificateSigned,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );


    /*
    ** Header constant tag is correct
    */
    ChkVerificationErr( f_pVerificationContext,
                        f_poCertHeader->dwHeaderTag == DRM_BCERT_HEADER_TAG,
                        DRM_E_BCERT_INVALID_CERT_HEADER_TAG );
    /*
    ** Version is 1
    */
    ChkVerificationErr( f_pVerificationContext,
                        f_poCertHeader->dwVersion == DRM_BCERT_VERSION,
                        DRM_E_BCERT_INVALID_CERT_VERSION );
    /*
    ** length is at least DRM_MIN_CERTIFICATE_LEN and no bigger then
    ** the byte array we are parsing
    */
    ChkBOOL( f_poCertHeader->cbCertificate >= DRM_MIN_CERTIFICATE_LEN,
             DRM_E_BCERT_INVALID_CERT_LENGTH );
    ChkBOOL( f_poCertHeader->cbCertificate <= f_cbCertData,
             DRM_E_BCERT_INVALID_CERT_LENGTH );
    /*
    ** length to signature object is <= length
    */
    ChkBOOL( f_poCertHeader->cbCertificateSigned <= f_poCertHeader->cbCertificate,
             DRM_E_BCERT_INVALID_SIGNEDCERT_LENGTH );
    /*
    ** length up to Signature Object should be larger
    ** than minimal certificate length - signature length
    */
    ChkBOOL( f_poCertHeader->cbCertificateSigned
            >=  DRM_MIN_CERTIFICATE_LEN - DRM_BCERT_SIGNATURE_OBJECT_LEN,
            DRM_E_BCERT_INVALID_SIGNEDCERT_LENGTH );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseCertBasicInfo
**
** Synopsis:    Parses a certificate's basic info.
**
** Arguments:   [f_pbCertData]  : data buffer that is being parsed
**              [f_cbCertData]  : size of the data buffer (in bytes)
**              [f_pbOffset]    : position to start parsing from
**              [f_poBasicInfo] : pointer to a basic info structure, cannot be NULL
**     [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SECURITY_LEVEL
**              DRM_E_BCERT_INVALID_CERT_TYPE
**              DRM_E_BCERT_BASICINFO_CERT_EXPIRED
**              DRM_E_BCERT_EXTDATAFLAG_CERT_TYPE_MISMATCH
**
** Notes:       f_pbOffset points to basic info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseCertBasicInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE       *f_pbCertData,
    __in                      const DRM_DWORD      f_cbCertData,
    __inout_ecount(1)         DRM_DWORD            *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_BASIC_INFO *f_poBasicInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_DWORD   dwOffset     = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseCertBasicInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poBasicInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** read certificate ID
    */
    COPY_FROMBUFFER( f_poBasicInfo->CertificateID.rgb,
                     f_pbCertData,
                     dwOffset,
                     DRM_BCERT_CERT_ID_LENGTH,
                     f_cbCertData );

    /*
    ** read security level data
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poBasicInfo->dwSecurityLevel,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    /*
    ** read flags
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poBasicInfo->dwFlags,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    /*
    ** read and verify cert type
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poBasicInfo->dwType,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poBasicInfo->dwType <= DRM_BCERT_CERTTYPE_MAX_VALUE,
                        DRM_E_BCERT_INVALID_CERT_TYPE );

    /*
    ** read digest value, no verification is needed
    */
    COPY_FROMBUFFER(
            f_poBasicInfo->DigestValue.m_rgbDigest,
            f_pbCertData,
            dwOffset,
            SHA256_DIGEST_SIZE_IN_BYTES,
            f_cbCertData );

    /*
    ** read expiration date and verify it is post current time.
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poBasicInfo->dwExpirationDate,
            f_pbCertData,
            dwOffset,
            f_cbCertData );
    /*
    ** read client ID
    */
    COPY_FROMBUFFER(
            f_poBasicInfo->ClientID.rgb,
            f_pbCertData,
            dwOffset,
            DRM_BCERT_CLIENT_ID_LENGTH,
            f_cbCertData );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseDomainInfo
**
** Synopsis:    Parses a certificate's domain info. The data will not be copied
**              if we only verify it.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poDomainInfo] : pointer to a domain info structure, can be NULL
**                                 in which case we only do verification.
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_DOMAIN_URL_TOO_LONG
**
** Notes:       f_pbOffset points to domain info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseDomainInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE        *f_pbCertData,
    __in                      const DRM_DWORD       f_cbCertData,
    __inout_ecount(1)         DRM_DWORD             *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_DOMAIN_INFO *f_poDomainInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;
    DRM_DWORD  dwUrlLength = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseDomainInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    if ( f_poDomainInfo != NULL )
    {
        /*
        ** read service ID
        */
        COPY_FROMBUFFER( f_poDomainInfo->ServiceID.rgb,
                         f_pbCertData,
                         dwOffset,
                         DRM_BCERT_SERVICE_ID_LENGTH,
                         f_cbCertData );

        /*
        ** read account ID
        */
        COPY_FROMBUFFER( f_poDomainInfo->AccountID.rgb,
                         f_pbCertData,
                         dwOffset,
                         DRM_BCERT_ACCOUNT_ID_LENGTH,
                         f_cbCertData );

        /*
        ** read revision timestamp
        */
        NETWORKBYTES_FROMBUFFER_TO_DWORD(
                f_poDomainInfo->dwRevision,
                f_pbCertData,
                dwOffset,
                f_cbCertData );
    }
    else
    {
        dwOffset += DRM_BCERT_SERVICE_ID_LENGTH + DRM_BCERT_ACCOUNT_ID_LENGTH + SIZEOF( DRM_DWORD );
    }
    /*
    ** Read length of domain URL.  Note that it is length without padding.
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwUrlLength,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );
    /*
    ** Domain URL cannot be empty or too big
    */
    ChkBOOL( dwUrlLength > 0, DRM_E_NO_URL );
    ChkBOOL( dwUrlLength <= DRM_BCERT_MAX_DOMAIN_URL_LENGTH, DRM_E_BCERT_DOMAIN_URL_TOO_LONG );
    /*
    ** No overflow or reading outside buffer boundaries
    */
    ChkOverflow( dwOffset + dwUrlLength, dwOffset );
    ChkOverflow( dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength), dwOffset + dwUrlLength );
    ChkOverflow( f_cbCertData, dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength) );
    /*
    ** Domain URL is a null-terminated UTF-8 string
    */
    dr = _isUTF8NullTerminatedData( f_pbCertData + dwOffset, dwUrlLength );
    ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
    dr = DRM_SUCCESS;

    if ( f_poDomainInfo != NULL )
    {
        COPY_FROMBUFFER(
                f_poDomainInfo->DomainURL.rgb,
                f_pbCertData,
                dwOffset,
                dwUrlLength + PAD_AMOUNT( dwUrlLength ),
                f_cbCertData );

        f_poDomainInfo->DomainURL.cb = dwUrlLength;
    }
    else
    {
        dwOffset += dwUrlLength + PAD_AMOUNT( dwUrlLength );
    }

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parsePCInfo
**
** Synopsis:    Parses a certificate's PC info.
**
** Arguments:   [f_pbCertData] : data buffer that is being parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_poPCInfo]   : pointer to a PC info structure, can be NULL
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_HARDWARE_ID_TOO_LONG
**
** Notes:       f_pbOffset points to PC info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parsePCInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE    *f_pbCertData,
    __in                      const DRM_DWORD   f_cbCertData,
    __inout_ecount(1)         DRM_DWORD         *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_PC_INFO *f_poPCInfo )
{
    DRM_RESULT dr     = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parsePCInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );

    /*
    ** Read security version
    */
    if ( f_poPCInfo != NULL )
    {
        NETWORKBYTES_FROMBUFFER_TO_DWORD(
                f_poPCInfo->dwSecurityVersion,
                f_pbCertData,
                *f_pbOffset,
                f_cbCertData );
    }
    else
    {
        ChkOverflow( *f_pbOffset + SIZEOF( DRM_DWORD ), *f_pbOffset );
        ChkBoundsLT( *f_pbOffset + SIZEOF( DRM_DWORD ), f_cbCertData );
        *f_pbOffset += SIZEOF( DRM_DWORD );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseServerInfo
**
** Synopsis:    Parses a certificate's Server info.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poServerInfo] : pointer to a Server info structure, can be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to Server info data not including object header.
******************************************************************************/
static DRM_RESULT _parseServerInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE    *f_pbCertData,
    __in                      const DRM_DWORD   f_cbCertData,
    __inout_ecount(1)         DRM_DWORD         *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_SERVER_INFO *f_poServerInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  dwWarningDays    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseServerInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );

    /*
    ** Read warning start date
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            dwWarningDays,
            f_pbCertData,
            *f_pbOffset,
            f_cbCertData );
    ChkVerificationErr( f_pVerificationContext, dwWarningDays > 0, DRM_E_BCERT_INVALID_WARNING_DAYS );

    if ( f_poServerInfo != NULL )
    {
        f_poServerInfo->dwWarningDays = dwWarningDays;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseSilverLightInfo
**
** Synopsis:    Parses a certificate's SilverLight info.
**
** Arguments:   [f_pbCertData]        : data buffer that is being parsed
**              [f_cbCertData]        : size of the data buffer (in bytes)
**              [f_pbOffset]          : position to start parsing from
**              [f_poSilverLightInfo] : pointer to a SilverLight info structure, can be NULL
**           [f_pVerificationContext] : pointer to verification context structure
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_HARDWARE_ID_TOO_LONG
**
** Notes:       f_pbOffset points to Silverlight info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseSilverLightInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE             *f_pbCertData,
    __in                      const DRM_DWORD             f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                  *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_SILVERLIGHT_INFO *f_poSilverLightInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  dwData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseSilverLightInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    /*
    ** Read security version
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext, dwData != 0, DRM_E_BCERT_INVALID_SECURITY_VERSION );
    if ( f_poSilverLightInfo != NULL )
    {
        f_poSilverLightInfo->dwSecurityVersion = dwData;
    }

    /*
    ** Read platform identifier
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData <= DRM_BCERT_SILVERLIGHT_PLATFORM_MAX,
                        DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER);
    if ( f_poSilverLightInfo != NULL )
    {
        f_poSilverLightInfo->dwPlatformIdentifier = dwData;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseDeviceInfo
**
** Synopsis:    Parses a certificate's Device info.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**              [f_poDeviceInfo] : pointer to a device info structure, can be NULL
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_INVALID_MAX_LICENSE_SIZE
**              DRM_E_BCERT_INVALID_MAX_HEADER_SIZE
**              DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH
**
** Notes:       f_pbOffset points to device info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseDeviceInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE        *f_pbCertData,
    __in                      const DRM_DWORD       f_cbCertData,
    __inout_ecount(1)         DRM_DWORD             *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_DEVICE_INFO *f_poDeviceInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )

{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwData   = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseDeviceInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );

    /*
    ** Read and verify max license size
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData == DRM_BCERT_MAX_LICENSE_SIZE,
                        DRM_E_BCERT_INVALID_MAX_LICENSE_SIZE );
    if ( f_poDeviceInfo != NULL )
    {
        f_poDeviceInfo->cbMaxLicense = dwData;
    }

    /*
    ** Read and verify max header size
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData == DRM_BCERT_MAX_HEADER_SIZE,
                        DRM_E_BCERT_INVALID_MAX_HEADER_SIZE );
    if ( f_poDeviceInfo != NULL )
    {
        f_poDeviceInfo->cbMaxHeader = dwData;
    }
    /*
    ** Read and verify max chain depth
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        dwData == DRM_BCERT_MAX_LICENSE_CHAIN_DEPTH,
                        DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );
    if ( f_poDeviceInfo != NULL )
    {
        f_poDeviceInfo->dwMaxChainDepth = dwData;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseMeteringInfo
**
** Synopsis:    Parses a certificate's metering info. The data will not be copied
**              if we only verify it.
**
** Arguments:   [f_pbCertData]   : data buffer that is being parsed
**              [f_cbCertData]   : size of the data buffer (in bytes)
**              [f_pbOffset]     : position to start parsing from
**            [f_poMeteringInfo] : pointer to a metering info structure, can be NULL
**                                 in which case we only do verification.
**      [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_METERING_URL_TOO_LONG
**
** Notes:       f_pbOffset points to metering info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseMeteringInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE          *f_pbCertData,
    __in                      const DRM_DWORD          f_cbCertData,
    __inout_ecount(1)         DRM_DWORD               *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_METERING_INFO *f_poMeteringInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;
    DRM_DWORD  dwUrlLength = 0;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    if ( f_poMeteringInfo != NULL )
    {
        /*
        ** read metering ID
        */
        COPY_FROMBUFFER( f_poMeteringInfo->MeteringID.rgb,
                         f_pbCertData,
                         dwOffset,
                         DRM_BCERT_METERING_ID_LENGTH,
                         f_cbCertData );
    }
    else
    {
        ChkOverflow( dwOffset + DRM_BCERT_METERING_ID_LENGTH, dwOffset );
        dwOffset += DRM_BCERT_METERING_ID_LENGTH;
    }

    /*
    ** Read length of metering URL.  Note that it is length without padding.
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwUrlLength,
                                      f_pbCertData,
                                      dwOffset,
                                      f_cbCertData );
    /*
    ** Metering URL cannot be empty or too big
    */
    ChkBOOL( dwUrlLength > 0, DRM_E_NO_URL );
    ChkBOOL( dwUrlLength <= DRM_BCERT_MAX_METERING_URL_LENGTH, DRM_E_BCERT_METERING_URL_TOO_LONG );
    /*
    ** No overflow or reading outside buffer boundaries
    */
    ChkOverflow( dwOffset + dwUrlLength, dwOffset );
    ChkOverflow( dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength), dwOffset + dwUrlLength );
    ChkOverflow( f_cbCertData, dwOffset + dwUrlLength + PAD_AMOUNT(dwUrlLength) );
    /*
    ** Metering URL is a null-terminated UTF-8 string
    */
    dr = _isUTF8NullTerminatedData( f_pbCertData + dwOffset, dwUrlLength );
    ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
    dr = DRM_SUCCESS;

    if ( f_poMeteringInfo != NULL )
    {
        COPY_FROMBUFFER(
                f_poMeteringInfo->MeteringURL.rgb,
                f_pbCertData,
                dwOffset,
                dwUrlLength + PAD_AMOUNT( dwUrlLength ),
                f_cbCertData );

        f_poMeteringInfo->MeteringURL.cb = dwUrlLength;
    }
    else
    {
        dwOffset += dwUrlLength + PAD_AMOUNT( dwUrlLength );
    }

    *f_pbOffset = dwOffset;

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function:    _parseSecurityVersion
**
** Synopsis:    Parses the Security Version object in a Ceritifcate
**
** Arguments:   [f_pbCertData]         : data buffer that is being parsed
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poSecurityVersion]  : pointer to a SecurityVersion structure, can be NULL
**              [f_poSecurityVersion2] : pointer to a SecurityVersion2 structure, can be NULL
**           [f_pVerificationContext] : pointer to verification context structure
**
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER
**              DRM_E_BCERT_INVALID_SECURITY_VERSION
**
** Notes:       f_pbOffset points to Security Version data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseSecurityVersion(
    __in_bcount(f_cbCertData) const DRM_BYTE                *f_pbCertData,
    __in                      const DRM_DWORD                f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                     *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_SECURITY_VERSION    *f_poSecurityVersion,
    __inout_ecount_opt(1)     DRM_BCERT_SECURITY_VERSION    *f_poSecurityVersion2,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  dwData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseSecurityVersion );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg (!(f_poSecurityVersion != NULL && f_poSecurityVersion2 != NULL));
    ChkArg( f_pVerificationContext != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    /*
    ** Read security version
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );

    ChkVerificationErr( f_pVerificationContext, dwData != 0, DRM_E_BCERT_INVALID_SECURITY_VERSION );
    if ( f_poSecurityVersion != NULL )
    {
        f_poSecurityVersion->dwSecurityVersion = dwData;
    }
    if ( f_poSecurityVersion2 != NULL )
    {
        f_poSecurityVersion2->dwSecurityVersion = dwData;
    }
    /*
    ** Read platform identifier
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD( dwData,
                                      f_pbCertData,
                                      *f_pbOffset,
                                      f_cbCertData );
    if ( f_poSecurityVersion != NULL )
    {
        f_poSecurityVersion->dwPlatformIdentifier = dwData;
    }
    if ( f_poSecurityVersion2 != NULL )
    {
        f_poSecurityVersion2->dwPlatformIdentifier = dwData;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    _parseFeatureInfo
**
** Synopsis:    Parses feature info object. Feature info structure is always filled-in
**              because it is needed for adjacent cert verification.
**
** Arguments:   [f_pbCertData]    : data buffer that is being parsed
**              [f_cbCertData]    : size of the data buffer (in bytes)
**              [f_dwCertType]    : type of a certificate being parsed
**              [f_pbOffset]      : position to start parsing from
**              [f_poFeatureInfo] : pointer to a feature info structure, cannot be NULL
**       [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_MAX_FEATURES
**
** Notes:       f_pbOffset points to feature info data not including object header.
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseFeatureInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE         *f_pbCertData,
    __in                      const DRM_DWORD        f_cbCertData,
    __inout_ecount(1)         DRM_DWORD              *f_pbOffset,
    __in const                DRM_DWORD              f_dwCertType,
    __inout_ecount(1)         DRM_BCERT_FEATURE_INFO *f_poFeatureInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;
    DRM_DWORD  iCount   = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseFeatureInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poFeatureInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poFeatureInfo->dwNumFeatureEntries,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    if ( f_poFeatureInfo->dwNumFeatureEntries == 0 )
    {
        *f_pbOffset = dwOffset;
        goto ErrorExit; /* no features in the list */
    }

    /*
    ** Verify that list of features is not too long
    */
    ChkBOOL( f_poFeatureInfo->dwNumFeatureEntries <= DRM_BCERT_MAX_FEATURES,
             DRM_E_BCERT_INVALID_MAX_FEATURES );

    /*
    ** read each feature
    */
    for ( iCount = 0; iCount < f_poFeatureInfo->dwNumFeatureEntries; iCount++ )
    {
        NETWORKBYTES_FROMBUFFER_TO_DWORD(
                f_poFeatureInfo->rgdwFeatureSet[ iCount ],
                f_pbCertData,
                dwOffset,
                f_cbCertData );
    }

    DRMASSERT( dwOffset == *f_pbOffset
                         + SIZEOF( DRM_DWORD )
                         + f_poFeatureInfo->dwNumFeatureEntries * SIZEOF( DRM_DWORD ) );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseManufacturerString
**
** Synopsis:    Reads and verifies one manufacturer string structure.
**
** Arguments:   [f_pbCertData]           : data buffer that is being parsed
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pbOffset]             : position to start parsing from
**              [f_poManufacturerString] : pointer to a manufacturer string structure,
**                                         cannot be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseManufacturerString(
    __in_bcount(f_cbCertData) const DRM_BYTE                *f_pbCertData,
    __in                      const DRM_DWORD               f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                     *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_MANUFACTURER_STRING *f_poManufacturerString,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseManufacturerString );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poManufacturerString != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** String length
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poManufacturerString->cb,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    if ( f_poManufacturerString->cb > 0 )
    {
        ChkBOOL( f_poManufacturerString->cb <= DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH, DRM_E_BCERT_MANUFACTURER_STRING_TOO_LONG );

        /*
        ** Read string
        */
        ChkOverflow( f_poManufacturerString->cb + PAD_AMOUNT( f_poManufacturerString->cb ), f_poManufacturerString->cb );
        COPY_FROMBUFFER(
                f_poManufacturerString->rgb,
                f_pbCertData,
                dwOffset,
                f_poManufacturerString->cb
                    + PAD_AMOUNT( f_poManufacturerString->cb ),
                f_cbCertData );

        dr = _isUTF8NullTerminatedData( f_poManufacturerString->rgb,
                                          f_poManufacturerString->cb );
        ChkVerificationErr( f_pVerificationContext, DRM_SUCCEEDED(dr), dr );
        dr = DRM_SUCCESS;
    }

    DRMASSERT( dwOffset == *f_pbOffset
                         + SIZEOF( DRM_DWORD )                         /* string length */
                         + f_poManufacturerString->cb                  /* string data */
                         + PAD_AMOUNT( f_poManufacturerString->cb ) ); /* padding */

    *f_pbOffset = dwOffset;

    /*
    ** Recalculate manufacturer string length value
    ** for older data that may have too many trailing zeros at the end.
    */
    while ( f_poManufacturerString->cb > 1
        && GET_BYTE( f_poManufacturerString->rgb, f_poManufacturerString->cb - 2 ) == 0x0 )
    {
        f_poManufacturerString->cb--;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/************************************************************************************
** Function:    _parseManufacturerInfo
**
** Synopsis:    Parses manufacturer info object.
**
** Arguments:   [f_pbCertData]         : data buffer that is being parsed
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poManufacturerInfo] : pointer to a manufacturer info structure,
**                                       can be NULL in which case we just validate data
**                                       and do not copy it.
**            [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to manufacturer info data not including object header.
**************************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseManufacturerInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE                *f_pbCertData,
    __in                      const DRM_DWORD               f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                     *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_MANUFACTURER_INFO   *f_poManufacturerInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;
    DRM_BCERT_MANUFACTURER_STRING manuStr = { 0 };

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Flags - read and ignore
    */
    ChkOverflow( dwOffset + SIZEOF( DRM_DWORD ), dwOffset );
    dwOffset += SIZEOF( DRM_DWORD );

    /*
    ** Manufacturer name
    */
    ChkDR( _parseManufacturerString(
        f_pbCertData,
        f_cbCertData,
        &dwOffset,
        f_poManufacturerInfo != NULL ? &f_poManufacturerInfo->ManufacturerStrings.ManufacturerName : &manuStr,
        f_pVerificationContext ) );
    /*
    ** Model name
    */
    ChkDR( _parseManufacturerString(
        f_pbCertData,
        f_cbCertData,
        &dwOffset,
        f_poManufacturerInfo != NULL ? &f_poManufacturerInfo->ManufacturerStrings.ModelName : &manuStr,
        f_pVerificationContext ) );
    /*
    ** Model number
    */
    ChkDR( _parseManufacturerString(
        f_pbCertData,
        f_cbCertData,
        &dwOffset,
        f_poManufacturerInfo != NULL ? &f_poManufacturerInfo->ManufacturerStrings.ModelNumber : &manuStr,
        f_pVerificationContext ) );

    *f_pbOffset = dwOffset;

ErrorExit:
    return dr;
}

/**********************************************************************************************************
** Function:    _parseExtDataSignKeyInfo
**
** Synopsis:    Parse extended data signature key object.
**
** Arguments:   [f_pbCertData]           : data buffer that is being parsed
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pbOffset]             : position to start parsing from
**              [f_poExtDataSignKeyInfo] : pointer to a signature info structure, cannot be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to extended data signature key info data not including object header.
**********************************************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseExtDataSignKeyInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE                 *f_pbCertData,
    __in                      const DRM_DWORD                f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                      *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_EXTDATA_SIGNKEY_INFO *f_poExtDataSignKeyInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseExtDataSignKeyInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_poExtDataSignKeyInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read key type
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD(
            f_poExtDataSignKeyInfo->wType,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataSignKeyInfo->wType == DRM_BCERT_KEYTYPE_ECC256,
                        DRM_E_BCERT_INVALID_KEY_TYPE );

    /*
    ** Read key length
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD(
            f_poExtDataSignKeyInfo->wLength,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataSignKeyInfo->wLength == SIZEOF( PUBKEY_P256 ) * CHAR_BIT,
                        DRM_E_BCERT_INVALID_KEY_LENGTH );

    /*
    ** Read flags
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poExtDataSignKeyInfo->dwFlags,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    /*
    ** Store position of key in the cert buffer.
    */
    ChkOverflow( dwOffset + SIZEOF( DRM_BCERT_PUBKEY ), dwOffset );
    ChkBOOL( dwOffset + SIZEOF( DRM_BCERT_PUBKEY ) <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    f_poExtDataSignKeyInfo->pValue = ( DRM_BCERT_PUBKEY * )( f_pbCertData + dwOffset );
    dwOffset += SIZEOF( DRM_BCERT_PUBKEY );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************************************************
** Function:    _parseSignatureInfo
**
** Synopsis:    Parse signature info object. This function does not validate signature data,
**              it only checks data for correct size and values.
**
** Arguments:   [f_pbCertData]       : data buffer that is being parsed
**              [f_cbCertData]       : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poSignatureInfo]  : pointer to a signature info structure, cannot be NULL
**          [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE
**              DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH
**
** Notes:       f_pbOffset points to signature info data not including object header.
**********************************************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseSignatureInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE                 *f_pbCertData,
    __in                      const DRM_DWORD                f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                      *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_SIGNATURE_INFO       *f_poSignatureInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;
    DRM_DWORD  dwSize = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseSignatureInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_poSignatureInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read and verify signature type
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD(
            f_poSignatureInfo->wSignatureType,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poSignatureInfo->wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256,
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Read and verify signature data length
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD(
            f_poSignatureInfo->SignatureData.cb,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poSignatureInfo->SignatureData.cb == SIZEOF( SIGNATURE_P256 ),
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Store position of signature in the cert buffer.
    */
    dwSize = f_poSignatureInfo->SignatureData.cb + PAD_AMOUNT( f_poSignatureInfo->SignatureData.cb );
    ChkOverflow( dwOffset + dwSize, dwOffset );
    ChkBOOL( dwOffset + dwSize <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    f_poSignatureInfo->SignatureData.pValue = ( DRM_BYTE   * )( f_pbCertData + dwOffset );
    dwOffset += dwSize;

    /*
    ** Read issuer key length which should be SIZEOF( PUBKEY_P256 ) * CHAR_BIT == 0x200
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poSignatureInfo->dwIssuerKeyLength,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poSignatureInfo->dwIssuerKeyLength == SIZEOF( PUBKEY_P256 ) * CHAR_BIT,
                        DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH );

    /*
    ** Store position of issuer key in the cert buffer.
    */
    ChkOverflow( dwOffset + SIZEOF( DRM_BCERT_PUBKEY ), dwOffset );
    ChkBOOL( dwOffset + SIZEOF( DRM_BCERT_PUBKEY ) <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    f_poSignatureInfo->pIssuerKey = ( DRM_BCERT_PUBKEY * )( f_pbCertData + dwOffset );
    dwOffset += SIZEOF( DRM_BCERT_PUBKEY );

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************************************************
** Function:    _parseExtDataContainer
**
** Synopsis:    Parse extended data info object. This function does not validate signature data,
**              it only checks data for correct size and values.
**
** Arguments:   [f_pbCertData]         : data buffer that is being parsed
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from
**              [f_poExtDataContainer] : pointer to a extended data info structure, cannot be NULL
**            [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**
** Notes:       f_pbOffset points to ext data info object not including object header.
**********************************************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseExtDataContainer(
    __in_bcount(f_cbCertData) const DRM_BYTE                 *f_pbCertData,
    __in                      const DRM_DWORD                f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                      *f_pbOffset,
    __inout_ecount(1)         DRM_BCERT_EXTDATA_CONTAINER    *f_poExtDataContainer,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwOffset = 0;
    DRM_DWORD  dwSize = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseExtDataContainer );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_poExtDataContainer != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read ext data object header
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, &dwOffset, &f_poExtDataContainer->Records[0].Header ) );
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Records[0].Header.wType == DRM_BCERT_OBJTYPE_EXTDATA_HWID
                            && f_poExtDataContainer->Records[0].Header.wFlags == DRM_BCERT_OBJFLAG_EMPTY,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    /*
    ** Number of records is always 1
    */
    f_poExtDataContainer->cbRecordsCount = 1;

    /*
    ** Read record length
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poExtDataContainer->Records[0].dwDataLength,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    /*
    ** Read record data: do not copy data record into any buffer,
    ** only need to set a pointer to where it starts.
    */
    ChkOverflow( dwOffset + f_poExtDataContainer->Records[0].dwDataLength, dwOffset );
    ChkBOOL( dwOffset + f_poExtDataContainer->Records[0].dwDataLength <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    f_poExtDataContainer->Records[0].pbData = (DRM_BYTE*) (f_pbCertData + dwOffset);

    /*
    ** Skip down to the signature.
    */
    ChkOverflow( dwOffset + f_poExtDataContainer->Records[0].dwDataLength, dwOffset );
    ChkOverflow( dwOffset + f_poExtDataContainer->Records[0].dwDataLength + PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength ), dwOffset + f_poExtDataContainer->Records[0].dwDataLength );
    dwOffset += f_poExtDataContainer->Records[0].dwDataLength + PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength );

    /*
    ** Read ext data signature object header
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, &dwOffset, &f_poExtDataContainer->Signature.Header ) );
    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Signature.Header.wType == DRM_BCERT_OBJTYPE_EXTDATASIGNATURE
                           && f_poExtDataContainer->Signature.Header.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    /*
    ** Read and verify signature type
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD(
            f_poExtDataContainer->Signature.wSignatureType,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Signature.wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256,
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Read and verify signature data length
    */
    NETWORKBYTES_FROMBUFFER_TO_WORD(
            f_poExtDataContainer->Signature.SignatureData.cb,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkVerificationErr( f_pVerificationContext,
                        f_poExtDataContainer->Signature.SignatureData.cb == SIZEOF( SIGNATURE_P256 ),
                        DRM_E_BCERT_INVALID_SIGNATURE_TYPE );

    /*
    ** Store the position of signature in the cert buffer.
    */
    ChkOverflow( f_poExtDataContainer->Signature.SignatureData.cb + PAD_AMOUNT( f_poExtDataContainer->Signature.SignatureData.cb ), f_poExtDataContainer->Signature.SignatureData.cb );

    dwSize = ( f_poExtDataContainer->Signature.SignatureData.cb + PAD_AMOUNT( f_poExtDataContainer->Signature.SignatureData.cb ) );
    ChkOverflow( dwOffset + dwSize, dwOffset );
    ChkBOOL( dwOffset + dwSize <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    f_poExtDataContainer->Signature.SignatureData.pValue = ( DRM_BYTE * )( f_pbCertData + dwOffset );
    dwOffset += dwSize;

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
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


/**********************************************************************************************************
** Function:    _verifyExtDataSignature
**
** Synopsis:    Verifies signature over extended data record.
**
** Arguments:   [f_pbCertData]           : data buffer that is being parsed
**              [f_cbCertData]           : size of the data buffer (in bytes)
**              [f_pbSignedDataLength]   : Offset to where extended data record begins.
**              [f_poExtDataSignKeyInfo] : pointer to extended data signing key structure, cannot be NULL.
**              [f_poExtDataContainer]   : pointer to extended data container object, cannot be NULL.
**              [f_pVerificationContext] : pointer to DRM_BCERT_VERIFICATIONCONTEXT structure.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE
**              DRM_E_BCERT_INVALID_ISSUERKEY_LENGTH
**
**********************************************************************************************************/
static DRM_NO_INLINE DRM_RESULT _verifyExtDataSignature(
    __in_bcount(f_cbCertData) const DRM_BYTE         *f_pbCertData,
    __in                      const DRM_DWORD        f_cbCertData,
    __in DRM_DWORD                                   f_pbSignedDataOffset,
    __inout_ecount(1) DRM_BCERT_EXTDATA_SIGNKEY_INFO *f_poExtDataSignKeyInfo,
    __inout_ecount(1) DRM_BCERT_EXTDATA_CONTAINER    *f_poExtDataContainer,
    __inout_ecount(1) DRM_BCERT_VERIFICATIONCONTEXT  *f_pVerificationContext )
{
    DRM_RESULT     dr                 = DRM_SUCCESS;
    DRM_DWORD      dwSignedDataLength = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_verifyExtDataSignature );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkOverflow( f_cbCertData, f_pbSignedDataOffset );
    ChkArg( f_poExtDataSignKeyInfo != NULL );
    ChkArg( f_poExtDataContainer != NULL );
    ChkArg( f_pVerificationContext != NULL );

    DRMASSERT( f_pVerificationContext->fDontFailOnMissingExtData == FALSE );

    dwSignedDataLength = SIZEOF( DRM_BCERT_OBJECT_HEADER ) + SIZEOF( DRM_DWORD ); /* ext data record header & record length */

    ChkOverflow(dwSignedDataLength + f_poExtDataContainer->Records[0].dwDataLength, dwSignedDataLength );
    dwSignedDataLength += f_poExtDataContainer->Records[0].dwDataLength; /* record data */

    ChkOverflow( dwSignedDataLength + PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength ), dwSignedDataLength );
    dwSignedDataLength += PAD_AMOUNT( f_poExtDataContainer->Records[0].dwDataLength ); /* padding */

    ChkOverflow( f_cbCertData, f_pbSignedDataOffset + dwSignedDataLength );

    /*
    ** verify signature over extended data
    */
    ChkArg( f_pVerificationContext->pCryptoCtx != NULL );

    ChkDR( OEM_ECDSA_Verify_P256( f_pbCertData + f_pbSignedDataOffset,
                                  dwSignedDataLength,
                                  f_poExtDataSignKeyInfo->pValue,
                                  (SIGNATURE_P256 *)f_poExtDataContainer->Signature.SignatureData.pValue,
                                  (struct bigctx_t *)f_pVerificationContext->pCryptoCtx->rgbCryptoContext ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseKeyInfo
**
** Synopsis:    Parses key info object and fills-in key info structure.
**
** Arguments:   [f_pbCertData] : data buffer that is being parsed
**              [f_cbCertData] : size of the data buffer (in bytes)
**              [f_pbOffset]   : position to start parsing from
**              [f_dwCertType] : type of a certificate being parsed
**              [f_poKeyInfo]  : pointer to a signature info structure, cannot be NULL
**              [f_pVerificationContext] : pointer to verification context structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_ARITHMETIC_OVERFLOW - if offset is too big
**              DRM_E_BUFFERTOOSMALL - buffer is shorter than expected
**              DRM_E_BCERT_INVALID_KEY_TYPE
**              DRM_E_BCERT_INVALID_KEY_LENGTH
**              DRM_E_BCERT_INVALID_MAX_KEY_USAGES
**
** Notes:       f_pbOffset points to key info data not including object header.
**
******************************************************************************/

static DRM_NO_INLINE DRM_RESULT  _parseKeyInfo(
    __in_bcount(f_cbCertData) const DRM_BYTE                *f_pbCertData,
    __in                      const DRM_DWORD               f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                     *f_pbOffset,
    __in const                DRM_DWORD                     f_dwCertType,
    __inout_ecount(1)         DRM_BCERT_KEY_INFO            *f_poKeyInfo,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr                      = DRM_SUCCESS;
    DRM_DWORD  iCount                  = 0;
    DRM_DWORD  iKey                    = 0;
    DRM_DWORD  dwOffset                = 0;
    DRM_BOOL   fCheckRequiredKeyUsages = FALSE;
    DRM_DWORD  dwKeyUsageMask          = 0;
    DRM_DWORD  dwRequiredKeyUsageMask  = 0;
    DRM_DWORD  dwNumUsageEntries       = 0;
    DRM_DWORD  dwKeyUsageValue         = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseKeyInfo );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poKeyInfo != NULL );
    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwOffset = *f_pbOffset;

    /*
    ** Read number of certificate keys
    */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            f_poKeyInfo->dwNumKeys,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkBOOL( f_poKeyInfo->dwNumKeys > 0, DRM_E_BCERT_PUBLIC_KEY_NOT_SPECIFIED );

    ChkBOOL(f_poKeyInfo->dwNumKeys <= DRM_BCERT_MAX_KEY_USAGES, DRM_E_BCERT_TOO_MANY_PUBLIC_KEYS );

    for ( iKey = 0; iKey < f_poKeyInfo->dwNumKeys; iKey++ )
    {
        /*
        ** Read key type
        */
        NETWORKBYTES_FROMBUFFER_TO_WORD(
                f_poKeyInfo->rgoKeys[iKey].wType,
                f_pbCertData,
                dwOffset,
                f_cbCertData );

        ChkVerificationErr( f_pVerificationContext,
                            f_poKeyInfo->rgoKeys[iKey].wType == DRM_BCERT_KEYTYPE_ECC256,
                            DRM_E_BCERT_INVALID_KEY_TYPE );

        /*
        ** Read key length
        */
        NETWORKBYTES_FROMBUFFER_TO_WORD(
                f_poKeyInfo->rgoKeys[iKey].wLength,
                f_pbCertData,
                dwOffset,
                f_cbCertData );

        ChkVerificationErr( f_pVerificationContext, f_poKeyInfo->rgoKeys[iKey].wLength == SIZEOF( PUBKEY_P256 ) * CHAR_BIT,
                 DRM_E_BCERT_INVALID_KEY_LENGTH );

        /*
        ** Read flags
        */
        NETWORKBYTES_FROMBUFFER_TO_DWORD(
                f_poKeyInfo->rgoKeys[iKey].dwFlags,
                f_pbCertData,
                dwOffset,
                f_cbCertData );

        /*
        ** Store position of key in the cert buffer.
        */
        ChkOverflow( dwOffset + SIZEOF( DRM_BCERT_PUBKEY ), dwOffset );
        ChkBOOL( dwOffset + SIZEOF( DRM_BCERT_PUBKEY ) <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
        f_poKeyInfo->rgoKeys[iKey].pValue = ( DRM_BCERT_PUBKEY * )( f_pbCertData + dwOffset );
        dwOffset += SIZEOF( DRM_BCERT_PUBKEY );

        /*
        ** Read and validate number of key usage entries
        */
        NETWORKBYTES_FROMBUFFER_TO_DWORD(
                dwNumUsageEntries,
                f_pbCertData,
                dwOffset,
                f_cbCertData );

        ChkBOOL( dwNumUsageEntries > 0
                 && dwNumUsageEntries <= DRM_BCERT_MAX_KEY_USAGES,
                 DRM_E_BCERT_INVALID_MAX_KEY_USAGES );

        f_poKeyInfo->rgoKeys[iKey].dwUsageSet = 0;

        /*
        ** Read key usage set.
        */
        for ( iCount = 0; iCount < dwNumUsageEntries; iCount++ )
        {
            NETWORKBYTES_FROMBUFFER_TO_DWORD(
                    dwKeyUsageValue,
                    f_pbCertData,
                    dwOffset,
                    f_cbCertData );

            f_poKeyInfo->rgoKeys[iKey].dwUsageSet |= BCERT_KEYUSAGE_BIT( dwKeyUsageValue );
        }

        if ( fCheckRequiredKeyUsages )
        {
            dwKeyUsageMask |= f_poKeyInfo->rgoKeys[iKey].dwUsageSet; /* collect cert key usages */
        }
    }

    if ( fCheckRequiredKeyUsages )
    {
        for ( iCount = 0; iCount < f_pVerificationContext->cRequiredKeyUsages; iCount++ )
        {
            dwRequiredKeyUsageMask |= BCERT_KEYUSAGE_BIT( f_pVerificationContext->rgdwRequiredKeyUsages[iCount] );/* collect required key usages */
        }
        /*
        ** See that all required key usages are present
        */
        ChkBOOL( ( dwKeyUsageMask & dwRequiredKeyUsageMask ) == dwRequiredKeyUsageMask, DRM_E_BCERT_REQUIRED_KEYUSAGE_MISSING );
    }

    *f_pbOffset = dwOffset;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _parseCertificate
**
** Synopsis:    Internal function that parses one certificate in a chain.
**              For performance reasons it does not verify certificate signature.
**              Function may be called in two modes:
**              - If called by DRM_BCert_GetCertificate() it will fill in the whole
**                DRM_BCERT_CERTIFICATE structure.
**              - If called by DRM_BCert_ParseCertificateChain() it will remember
**                only minimal info from a cert needed for further verification.
**
** Arguments:   [f_pbCertData]    : data buffer that is being parsed
**              [f_cbCertData]    : size of the data buffer (in bytes)
**              [f_pbOffset]      : position to start parsing from
**              [f_poCertificate] : output parameter, pointer to a full certificate structure.
**                                  Cannot be NULL if f_poMinCertData == NULL.
**                                  Should be NULL if f_poMinCertData != NULL.
**              [f_poMinCertData] : output parameter, pointer to a structure to store minimal cert info.
**                                  Cannot be NULL if f_poCertificate == NULL.
**                                  Should be NULL if f_poCertificate != NULL.
**              [f_pVerificationContext] : Pointer to DRM_BCERT_VERIFICATIONCONTEXT structure. Cannot be NULL.
**
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER
**              DRM_E_BCERT_HWIDINFO_IS_MISSING
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _parseCertificate(
    __in_bcount(f_cbCertData) const DRM_BYTE                *f_pbCertData,
    __in                      const DRM_DWORD               f_cbCertData,
    __inout_ecount(1)         DRM_DWORD                     *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_CERTIFICATE         *f_poCertificate,
    __inout_ecount_opt(1)     DRM_BCERT_MINIMALDATA         *f_poMinCertData,
    __inout_ecount(1)         DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT               dr                = DRM_SUCCESS;
    DRM_DWORD                dwCertificateType = 0;
    DRM_DWORD                dwCertFlags       = DRM_BCERT_FLAGS_EMPTY;
    DRM_BCERT_OBJECT_HEADER  oObjHeader        = { 0 };
    DRM_VOID                 *pHelperPtr       = NULL;
    DRM_BCERT_EXTDATA_SIGNKEY_INFO oExtDataSignKeyInfo = { 0 }; /* if present this data is needed to verify ext data signature */
    DRM_DWORD                dwCertBegin       = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_parseCertificate );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );
    ChkArg( f_poCertificate != NULL || f_poMinCertData != NULL );
    ChkArg( f_poCertificate == NULL || f_poMinCertData == NULL );
    ChkArg( f_pVerificationContext != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );

    dwCertBegin = *f_pbOffset; /* Remember where the cert begins */

    /*
    ** Certificate header.
    */
    pHelperPtr = f_poCertificate != NULL ? &f_poCertificate->Header : &f_poMinCertData->oCertHeader;
    ChkDR( _parseCertHeader( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             ( DRM_BCERT_HEADER* ) pHelperPtr,
                             f_pVerificationContext ) );
    /*
    ** Basic info always follows certificate header.
    */
    pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->BasicInfo : &f_poMinCertData->oBasicInfo;
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &(( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header ) );

    ChkVerificationErr( f_pVerificationContext,
                        (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header.wFlags   == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                           && (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header.wType    == DRM_BCERT_OBJTYPE_BASIC
                           && (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->Header.cbLength == DRM_BCERT_BASIC_INFO_OBJECT_LEN,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    ChkDR( _parseCertBasicInfo( f_pbCertData,
                                f_cbCertData,
                                f_pbOffset,
                                ( DRM_BCERT_BASIC_INFO* )pHelperPtr,
                                f_pVerificationContext ) );

    /*
    ** Remember certificate type and flags
    */
    dwCertificateType = (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->dwType;
    dwCertFlags = (( DRM_BCERT_BASIC_INFO* ) pHelperPtr)->dwFlags;

    /*
    ** Get optional type-specific data object.
    ** Object type should match certificate type in basic info
    ** for device, PC, SilverLight, Metering, Server and Domain certificates.
    ** This code assumes that if a certificate type is Domain, PC, SilverLight, Metering, Server
    ** or Device then type-specific information must be present.
    ** Certificates of other types should not have a type-specific data object.
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );

    switch ( dwCertificateType )
    {
    case DRM_BCERT_CERTTYPE_DOMAIN:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_DOMAIN,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_DOMAIN_INFO* )&f_poCertificate->Type )->Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseDomainInfo( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 f_poCertificate != NULL ? ( DRM_BCERT_DOMAIN_INFO* )&f_poCertificate->Type : NULL,
                                 f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_PC:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_PC,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_PC_INFO* )&f_poCertificate->Type )->Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parsePCInfo( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             f_poCertificate != NULL ? ( DRM_BCERT_PC_INFO* )&f_poCertificate->Type : NULL ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_SILVERLIGHT:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SILVERLIGHT,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_SILVERLIGHT_INFO* )&f_poCertificate->Type )->Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseSilverLightInfo( f_pbCertData,
                                      f_cbCertData,
                                      f_pbOffset,
                                      f_poCertificate != NULL ? ( DRM_BCERT_SILVERLIGHT_INFO* )&f_poCertificate->Type : NULL,
                                      f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_DEVICE:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags   == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                               && oObjHeader.wType    == DRM_BCERT_OBJTYPE_DEVICE
                               && oObjHeader.cbLength == DRM_BCERT_DEVICE_INFO_OBJECT_LEN,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_DEVICE_INFO* )&f_poCertificate->Type )->Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseDeviceInfo( f_pbCertData,
                                 f_cbCertData,
                                 f_pbOffset,
                                 f_poCertificate != NULL ? ( DRM_BCERT_DEVICE_INFO* )&f_poCertificate->Type : NULL,
                                 f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_METERING:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                               && oObjHeader.wType  == DRM_BCERT_OBJTYPE_METERING,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_METERING_INFO* )&f_poCertificate->Type )->Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseMeteringInfo( f_pbCertData,
                                   f_cbCertData,
                                   f_pbOffset,
                                   f_poCertificate != NULL ? ( DRM_BCERT_METERING_INFO* )&f_poCertificate->Type : NULL,
                                   f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_SERVER:
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SERVER,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &( ( DRM_BCERT_SERVER_INFO* )&f_poCertificate->Type )->Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseServerInfo( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             f_poCertificate != NULL ? ( DRM_BCERT_SERVER_INFO* )&f_poCertificate->Type : NULL,
                             f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
        break;

    case DRM_BCERT_CERTTYPE_ISSUER:
    case DRM_BCERT_CERTTYPE_CRL_SIGNER:
    case DRM_BCERT_CERTTYPE_SERVICE:
    case DRM_BCERT_CERTTYPE_APPLICATION:
    case DRM_BCERT_CERTTYPE_KEYFILESIGNER:
    case DRM_BCERT_CERTTYPE_LICENSESIGNER:
        break; /* there is no type-specific info object for these cert types */

    default:
        ChkVerificationErr( f_pVerificationContext, FALSE, DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER ); /* unknown certificate type */
        break;
    }

    /*
    ** Security Version is optionally present now.
    */
    if( oObjHeader.wType == DRM_BCERT_OBJTYPE_SECURITY_VERSION )
    {
        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &f_poCertificate->SecurityVersion.Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        if ( f_poMinCertData != NULL )
        {
            DRM_BYT_CopyBytes( &f_poMinCertData->oSecurityVersion.Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }

        pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->SecurityVersion : &f_poMinCertData->oSecurityVersion;

        ChkDR( _parseSecurityVersion( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             ( DRM_BCERT_SECURITY_VERSION* ) pHelperPtr,
                             NULL,
                             f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
    }

    /*
    ** Security Version 2 is optionally present now.
    */
    if( oObjHeader.wType == DRM_BCERT_OBJTYPE_SECURITY_VERSION_2 )
    {
        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &f_poCertificate->SecurityVersion2.Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        if ( f_poMinCertData != NULL )
        {
            DRM_BYT_CopyBytes( &f_poMinCertData->oSecurityVersion2.Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }

        pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->SecurityVersion2 : &f_poMinCertData->oSecurityVersion2;

        ChkDR( _parseSecurityVersion( f_pbCertData,
                             f_cbCertData,
                             f_pbOffset,
                             NULL,
                             ( DRM_BCERT_SECURITY_VERSION* ) pHelperPtr,
                             f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
    }
    /*
    ** Feature info must be present
    */
    ChkVerificationErr( f_pVerificationContext,
                        oObjHeader.wType    == DRM_BCERT_OBJTYPE_FEATURE
                             && oObjHeader.wFlags   == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                             && oObjHeader.cbLength >= DRM_BCERT_FEATURES_OBJECT_MIN_LEN,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    if ( f_poCertificate != NULL )
    {
        DRM_BYT_CopyBytes( &f_poCertificate->FeatureInfo.Header,
                           0,
                           &oObjHeader,
                           0,
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }
    if ( f_poMinCertData != NULL )
    {
        DRM_BYT_CopyBytes( &f_poMinCertData->oFeatureInfo.Header,
                           0,
                           &oObjHeader,
                           0,
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }
    pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->FeatureInfo : &f_poMinCertData->oFeatureInfo;
    ChkDR( _parseFeatureInfo( f_pbCertData,
                              f_cbCertData,
                              f_pbOffset,
                              dwCertificateType,
                              ( DRM_BCERT_FEATURE_INFO* ) pHelperPtr,
                              f_pVerificationContext ) );

    /*
    ** Key info - object must be present
    */
    pHelperPtr = ( f_poCertificate != NULL ) ? &f_poCertificate->KeyInfo : &f_poMinCertData->oKeyInfo;

    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &(( DRM_BCERT_KEY_INFO* )pHelperPtr)->Header ) );

    ChkVerificationErr( f_pVerificationContext,
                        (( DRM_BCERT_KEY_INFO* )pHelperPtr)->Header.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                             && (( DRM_BCERT_KEY_INFO* )pHelperPtr)->Header.wType  == DRM_BCERT_OBJTYPE_KEY,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    ChkDR( _parseKeyInfo( f_pbCertData,
                          f_cbCertData,
                          f_pbOffset,
                          dwCertificateType,
                          ( DRM_BCERT_KEY_INFO* ) pHelperPtr,
                          f_pVerificationContext ) );

    /*
    ** Manufacturer info - optional
    */
    ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );

    if ( oObjHeader.wType == DRM_BCERT_OBJTYPE_MANUFACTURER )
    {
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_EMPTY,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &f_poCertificate->ManufacturerInfo.Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseManufacturerInfo( f_pbCertData,
                                       f_cbCertData,
                                       f_pbOffset,
                                       ( f_poCertificate != NULL ) ? &f_poCertificate->ManufacturerInfo : NULL,
                                       f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
    }

    /*
    ** Check for presence of a public key object to verify extended data
    ** also indicated by a flag in a basic info.
    ** For now only PC certs can have it, all other types do not (already verified this condition)
    */
    if ( (dwCertFlags & DRM_BCERT_FLAGS_EXTDATA_PRESENT) == DRM_BCERT_FLAGS_EXTDATA_PRESENT )
    {
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_EMPTY
                               && oObjHeader.wType  == DRM_BCERT_OBJTYPE_EXTDATASIGNKEY,
                            DRM_E_BCERT_EXTDATA_IS_NOT_PROVIDED );

        if ( f_poCertificate != NULL )
        {
            DRM_BYT_CopyBytes( &f_poCertificate->ExtDataSignatureInfo.Header,
                               0,
                               &oObjHeader,
                               0,
                               SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
        }
        ChkDR( _parseExtDataSignKeyInfo( f_pbCertData,
                                         f_cbCertData,
                                         f_pbOffset,
                                         ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataSignatureInfo : &oExtDataSignKeyInfo,
                                         f_pVerificationContext ) );

        ChkDR( _getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oObjHeader ) );
    }
    else
    {
        ChkVerificationErr( f_pVerificationContext,
                            oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                                && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SIGNATURE,
                            DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );
    }

    /*
    ** The last object (with exception of PC cert) is signature
    ** which must be present. Header is already read.
    */
    ChkVerificationErr( f_pVerificationContext,
                        oObjHeader.wFlags == DRM_BCERT_OBJFLAG_MUST_UNDERSTAND
                           && oObjHeader.wType  == DRM_BCERT_OBJTYPE_SIGNATURE,
                        DRM_E_BCERT_UNEXPECTED_OBJECT_HEADER );

    if ( f_poCertificate != NULL )
    {
        DRM_BYT_CopyBytes( &f_poCertificate->SignatureInfo.Header,
                           0,
                           &oObjHeader,
                           0,
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }
    if ( f_poMinCertData != NULL )
    {
        DRM_BYT_CopyBytes( &f_poMinCertData->oSignatureInfo.Header,
                           0,
                           &oObjHeader,
                           0,
                           SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
    }
    ChkDR( _parseSignatureInfo( f_pbCertData,
                                f_cbCertData,
                                f_pbOffset,
                                ( f_poCertificate != NULL ) ? &f_poCertificate->SignatureInfo : &f_poMinCertData->oSignatureInfo,
                                f_pVerificationContext ) );

    /*
    ** PC certificate should have signed extended data information.
    ** It follows certificate signature object.
    ** Sometimes the caller of the cert parser allows or expects partially built certificates
    ** and sets f_pVerificationContext->fDontFailOnMissingExtData into TRUE. In this case
    ** if extended data object is present we verify the basics (obj header and structure),
    ** but do not verify its signature. If extended data is missing do not fail.
    */
    if ( (dwCertFlags & DRM_BCERT_FLAGS_EXTDATA_PRESENT) == DRM_BCERT_FLAGS_EXTDATA_PRESENT )
    {
        DRM_BCERT_EXTDATA_CONTAINER oExtDataContainer    = { 0 };
        DRM_DWORD                   dwExtDataObjectStart = 0;

        if ( DRM_SUCCEEDED(_getObjectHeader( f_pbCertData, f_cbCertData, f_pbOffset, &oExtDataContainer.Header ))
                && oExtDataContainer.Header.wType == DRM_BCERT_OBJTYPE_EXTDATACONTAINER
                && oExtDataContainer.Header.wFlags == (DRM_BCERT_OBJFLAG_MUST_UNDERSTAND | DRM_BCERT_OBJFLAG_CONTAINER_OBJ) )
        {

            if ( f_poCertificate != NULL )
            {
                DRM_BYT_CopyBytes( &f_poCertificate->ExtDataContainer.Header,
                                   0,
                                   &oExtDataContainer.Header,
                                   0,
                                   SIZEOF( DRM_BCERT_OBJECT_HEADER ) );
            }

            /*
            ** Remember where extended data record object starts for signature verification.
            ** Container header has just been read, so this is the right place.
            */
            dwExtDataObjectStart = *f_pbOffset;

            ChkDR( _parseExtDataContainer( f_pbCertData,
                                           f_cbCertData,
                                           f_pbOffset,
                                           ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataContainer : &oExtDataContainer,
                                           f_pVerificationContext ) );
            /*
            ** Verify extended data signature
            */
            if ( !f_pVerificationContext->fDontFailOnMissingExtData )
            {
                ChkDR( _verifyExtDataSignature( f_pbCertData,
                                                f_cbCertData,
                                                dwExtDataObjectStart,
                                                ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataSignatureInfo : &oExtDataSignKeyInfo,
                                                ( f_poCertificate != NULL ) ? &f_poCertificate->ExtDataContainer : &oExtDataContainer,
                                                f_pVerificationContext ) );
            }

            /*
            ** Reserved space for extended data can be larger than the actual extended data objects.
            ** Need to update offset correctly to point to the beginning of the next cert.
            */
            ChkOverflow( dwCertBegin + ( ( f_poCertificate != NULL ) ? f_poCertificate->Header.cbCertificate : f_poMinCertData->oCertHeader.cbCertificate ), dwCertBegin );
            ChkBoundsLT( dwCertBegin + ( ( f_poCertificate != NULL ) ? f_poCertificate->Header.cbCertificate : f_poMinCertData->oCertHeader.cbCertificate ), f_cbCertData );
            *f_pbOffset = dwCertBegin + ( ( f_poCertificate != NULL ) ? f_poCertificate->Header.cbCertificate : f_poMinCertData->oCertHeader.cbCertificate );
        }
        else
        {
            /*
            ** This is a "partially built" certificate,
            ** Record DRM_E_BCERT_HWIDINFO_IS_MISSING code.
            ** The caller will calculate where the next cert begins.
            */
            if ( !f_pVerificationContext->fDontFailOnMissingExtData )
            {
                ChkVerificationErr( f_pVerificationContext, FALSE, DRM_E_BCERT_HWIDINFO_IS_MISSING );
            }
        }
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
**              DRM_E_ARITHMETIC_OVERFLOW - comes from _getObjectHeader
**              DRM_E_BCERT_OBJECTHEADER_LEN_TOO_BIG - comes from _getObjectHeader
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
        ChkDR( _getObjectHeader( f_pbCertData,
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

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    DRM_BCert_InitVerificationContext
**
** Synopsis:    Initializes parser verification context.
**
** Arguments:
**      [f_pftExpirationTime]           : these parameters correspond to the
**      [f_pRootPubKey]                   data fields in verification context
**      [f_dwCertType]                    structure. All pointer params are optional.
**      [f_pCryptoCtx]                    For default values and meanings see
**      [f_fVerifyCertSignatures]         DRM_BCERT_VERIFICATIONCONTEXT structure
**      [f_fDontFailOnMissingExtData]     in drmbcertparser.h
**      [f_pKeyUsages]
**      [f_NumKeyUsages]
**      [f_fCollectErrs]
**      [f_pVerificationContext]        : pointer to verification context
**                                        to be initialized. Cannot be NULL.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_InitVerificationContext(
    __in_opt                              DRMFILETIME                   *f_pftExpirationTime,
    __in_opt                        const PUBKEY_P256                   *f_pRootPubKey,
    __in                            const DRM_DWORD                      f_dwCertType,
    __in_opt                              DRM_CRYPTO_CONTEXT            *f_pCryptoCtx,
    __in                                  DRM_BOOL                       f_fVerifyCertSignatures,
    __in                                  DRM_BOOL                       f_fDontFailOnMissingExtData,
    __in_ecount_opt(f_NumKeyUsages) const DRM_DWORD                     *f_pKeyUsages,
    __in                                  DRM_DWORD                      f_NumKeyUsages,
    __in                                  DRM_BOOL                       f_fCollectErrs,
    __in_opt                              DRM_BB_CONTEXT                *f_pcontextBBX,
    __in_opt                              DRM_SECSTORE_CONTEXT          *f_pcontextSST,
    __in_opt                              DRM_DST                       *f_pDatastore,
    __inout                               DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pVerificationContext != NULL );

    ChkArg( f_pCryptoCtx != NULL || ( f_fVerifyCertSignatures == FALSE && f_fDontFailOnMissingExtData == TRUE ) );
    ChkArg( f_pKeyUsages == NULL || ( f_NumKeyUsages > 0 && f_NumKeyUsages <= DRM_BCERT_MAX_KEY_USAGES ) );

    ZEROMEM( f_pVerificationContext, SIZEOF( DRM_BCERT_VERIFICATIONCONTEXT ) );

    if ( f_pftExpirationTime != NULL )
    {
        DRM_BYT_CopyBytes(&f_pVerificationContext->ftExpirationTime, 0, f_pftExpirationTime, 0, SIZEOF(DRMFILETIME) );
    }

    f_pVerificationContext->pRootPubKey = f_pRootPubKey;
    f_pVerificationContext->dwType = f_dwCertType;
    f_pVerificationContext->pCryptoCtx = f_pCryptoCtx;
    f_pVerificationContext->fVerifyCertSignatures = f_fVerifyCertSignatures;
    f_pVerificationContext->fDontFailOnMissingExtData = f_fDontFailOnMissingExtData;
    f_pVerificationContext->pBlackboxCtx = f_pcontextBBX;
    f_pVerificationContext->pSecStoreCtx = f_pcontextSST;
    f_pVerificationContext->pDSTCtx = f_pDatastore;

    if ( f_pKeyUsages != NULL && f_NumKeyUsages > 0 )
    {
        DRM_BYT_CopyBytes( f_pVerificationContext->rgdwRequiredKeyUsages, 0, f_pKeyUsages, 0, SIZEOF(DRM_DWORD) * f_NumKeyUsages );
    }

    f_pVerificationContext->fCollectErrorsInArrayAndContinue = f_fCollectErrs;

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
			ChkDR( _parseCertificate( f_pbCertData,
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
		__in_bcount(f_cbCertData)			  const DRM_BYTE		  *f_pbCertData,
		__in								  const DRM_DWORD		  f_cbCertData,
		__inout 						DRM_BCERT_VERIFICATIONCONTEXT *f_pVerificationContext )
	{
		DRM_RESULT			   dr				= DRM_SUCCESS;
		DRM_DWORD			   bCurrOffset		= 0;
		DRM_BCERT_CHAIN_HEADER oChainHeader 	= { 0 };
		DRM_BCERT_MINIMALDATA  roCertData[2]	= { 0, 0 };
		DRMFILETIME 		   ftCertTime		= { 0 };
		DRM_UINT64			   ui64CurrentTime	= DRM_UI64LITERAL( 0, 0 );
		DRM_UINT64			   uiCertTime		= DRM_UI64LITERAL( 0, 0 );
	
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
			ChkDR( _parseCertificate( f_pbCertData,
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
		#if 0 /* TODO - Playready */
			if ( DoExpirationCheck( f_pVerificationContext->ftExpirationTime ) )
			{
				FILETIME_TO_UI64( f_pVerificationContext->ftExpirationTime, ui64CurrentTime );
				CREATE_FILE_TIME( roCertData[ f_pVerificationContext->cCurrentCert % 2 ].oBasicInfo.dwExpirationDate, ftCertTime );
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

/********************************************************************************************
** Function:    DRM_BCert_GetCertificate
**
** Synopsis:    Given the beginning of a certificate in a chain the function skips over it
**              and moves the offset to the next one. If DRM_BCERT_CERTIFICATE is not NULL
**              the certificate's content will be parsed and stored in it.
**
** Arguments:   [f_pbCertData]         : data buffer to parse
**              [f_cbCertData]         : size of the data buffer (in bytes)
**              [f_pbOffset]           : position to start parsing from, should point
**                                       to the beginning of some certificate. On return
**                                       this value will be updated with the next cert's beginning.
**              [f_poCertificate]      : pointer to a certificate structure, optional.
**              [f_dwRequiredCertType] : expected type of certificate, set into DRM_BCERT_CERTTYPE_UNKNOWN
**                                       if it is not important. Otherwise the function will return
**                                       DRM_BCERT_CERTTYPE_UNKNOWN if type does not match.
**                                       Also this parameter is ignored if f_poCertificate == NULL.
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_S_FALSE      - if it was the last certificate in chain.
**              DRM_E_INVALIDARG
**              DRM_E_ARITHMETIC_OVERFLOW
**              DRM_E_BCERT_INVALID_CERT_LENGTH
**              DRM_E_BCERT_INVALID_CERT_TYPE
**
** Note:  1. The function will not validate the certificate if f_poCertificate == NULL,
**           only skip over it.
**
**        2. If f_poCertificate != NULL then cert data will be checked for compliance with
**           format only! Cert signature, expiration date and public key will NOT be verified.
**           Call DRM_BCert_ParseCertificateChain() API for full validation!
**
**        3. If DRM_S_FALSE is returned it means that there are no more certs in a chain.
**           The f_pbOffset will still be updated so that an attempt to use this value
**           somewhere else would fail.
**
**        4. To enumerate all certs in a chain first call DRM_BCert_GetChainHeader(),
**           then DRM_BCert_GetCertificate. See notes for DRM_BCert_GetChainHeader.
********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BCert_GetCertificate(
    __in_bcount(f_cbCertData) const DRM_BYTE         *f_pbCertData,
    __in                      const DRM_DWORD        f_cbCertData,
    __inout_ecount(1)         DRM_DWORD              *f_pbOffset,
    __inout_ecount_opt(1)     DRM_BCERT_CERTIFICATE  *f_poCertificate,
    __in                      const DRM_DWORD        f_dwRequiredCertType )
{
    DRM_RESULT dr                  = DRM_SUCCESS;
    DRM_DWORD  dwOffset            = 0;
    DRM_DWORD  dwCertificateLength = 0;
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_DRM_BCert_GetCertificate );

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pbOffset != NULL );

    ChkOverflow( f_cbCertData, *f_pbOffset );
    dwOffset = *f_pbOffset;

    if ( f_poCertificate != NULL )  /* parse current certificate */
    {
        ChkDR( DRM_BCert_InitVerificationContext( NULL,
                                                  NULL,
                                                  f_dwRequiredCertType,
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

        ChkDR( _parseCertificate( f_pbCertData,
                                  f_cbCertData,
                                  &dwOffset,
                                  f_poCertificate,
                                  NULL,
                                  &oVerificationContext ) );

        dwCertificateLength = f_poCertificate->Header.cbCertificate;
    }
    else  /* only read the header */
    {
        DRM_BCERT_HEADER oCertHeader = { 0 };
        ChkDR( _parseCertHeader( f_pbCertData, f_cbCertData, &dwOffset, &oCertHeader, &oVerificationContext ) );

        dwCertificateLength = oCertHeader.cbCertificate;
    }
    ChkOverflow( *f_pbOffset + dwCertificateLength, *f_pbOffset );  /* cert length is not too big */
    ChkOverflow( f_cbCertData, *f_pbOffset + dwCertificateLength ); /* new offset is not too big */

    *f_pbOffset  += dwCertificateLength;  /* move the offset to the beginning of the next certificate */

    ChkBOOL( *f_pbOffset < f_cbCertData, DRM_S_FALSE ); /* otherwise we were at the last certificate in a chain */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


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
    __out_opt                           DRM_DWORD         *f_pdwKeyUsageSet )
{
    DRM_RESULT              dr             = DRM_SUCCESS;
    DRM_DWORD               bCurrOffset    = 0;
    DRM_BCERT_CHAIN_HEADER  oChainHeader   = {0};
    DRM_DWORD               iCertCounter   = 0;
    DRM_DWORD               iKeyCounter    = 0;
    DRM_BOOL                fFoundKey      = FALSE;
    DRM_DWORD               iFoundKey      = 0;
    DRM_BCERT_KEY_INFO      oKeyInfo       = {0};
    DRM_BCERT_VERIFICATIONCONTEXT oVerificationContext = {0};
    DRM_DWORD               dwBitMask      = BCERT_KEYUSAGE_BIT( f_dwKeyUsage );

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

    ChkDR( _parseKeyInfo( f_pbCertData,
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

    DRM_BYT_CopyBytes( f_pPubkey->m_rgbPubkey,
                       0,
                       oKeyInfo.rgoKeys[iFoundKey].pValue,
                       0,
                       SIZEOF( PUBKEY_P256 ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
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
DRM_API DRM_RESULT DRM_CALL DRM_BCert_GetSecurityVersion(
    __in_bcount(f_cbCertData) const DRM_BYTE      *f_pbCertData,
    __in                      const DRM_DWORD      f_cbCertData,
    __out_ecount(1)                 DRM_DWORD     *f_pdwSecurityVersion,
    __out_ecount(1)                 DRM_DWORD     *f_pdwPlatformIdentifier )
{
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

    ChkDR( _parseSecurityVersion( f_pbCertData,
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

    ChkDR( _parseManufacturerInfo( f_pbCertData,
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
** Arguments:   [f_pbCertData]    : data buffer to parse
**              [f_cbCertData]    : size of the data buffer (in bytes)
**              [f_pCachedValues] : pointer to the structure to store cached data from the cert.
**              [f_pOEMContext]   : pointer to OemContext structure
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**              DRM_E_BCERT_INVALID_CHAIN_LENGTH
**              DRM_E_BCERT_INVALID_CERT_TYPE
**
** Note: We assume that this data is valid so that we don't have to parse it again.
**       Function should only be called for device certificates.
***********************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BCert_LoadPropertiesCache(
   __in_bcount(f_cbCertData) const DRM_BYTE     *f_pbCertData,
   __in                      const DRM_DWORD     f_cbCertData,
   __inout DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues,
   __in_opt DRM_VOID                            *f_pOEMContext )
{
    DRM_RESULT              dr               = DRM_SUCCESS;
    DRM_DWORD               bLeafCertStart   = 0;
    DRM_DWORD               bGroupCertStart  = 0;
    DRM_DWORD               dwOffset         = 0;
    DRM_DWORD               dwNumFeatures    = 0;
    DRM_DWORD               dwCount          = 0;
    DRM_BCERT_CHAIN_HEADER  oChainHeader     = { 0 };
    DRM_DWORD               dwCertType       = 0;
    DRM_DWORD               dwKeyUsageSet    = 0 ;

    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData > 0 );
    ChkArg( f_pCachedValues != NULL );

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
    ** Read leaf certificate's type from basic info, it must be device
    */
    dwOffset = bLeafCertStart
             + SIZEOF( DRM_BCERT_HEADER )       /* certificate header */
             + SIZEOF( DRM_BCERT_OBJECT_HEADER) /* basic info's object header */
             + DRM_BCERT_CERT_ID_LENGTH         /* cert ID */
             + SIZEOF( DRM_DWORD )              /* security level */
             + SIZEOF( DRM_DWORD );             /* flags */
    NETWORKBYTES_FROMBUFFER_TO_DWORD(
            dwCertType,
            f_pbCertData,
            dwOffset,
            f_cbCertData );

    ChkBOOL( dwCertType == DRM_BCERT_CERTTYPE_DEVICE, DRM_E_BCERT_INVALID_CERT_TYPE );

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
    COPY_FROMBUFFER(
            f_pCachedValues->DeviceSerialNumber.rgb,
            f_pbCertData,
            dwOffset,
            DRM_BCERT_CLIENT_ID_LENGTH,
            f_cbCertData );

    /*
    ** read public keys for signing and encryption
    */
    ChkDR( DRM_BCert_GetPublicKeyByUsage( f_pbCertData,
                                          f_cbCertData,
                                          0,
                                          DRM_BCERT_KEYUSAGE_SIGN,
                                          &f_pCachedValues->DeviceKeySign.PublicKey,
                                          &dwKeyUsageSet ) );

    DRMASSERT( dwKeyUsageSet > 0 ); /* at least one key usage per key, and there should be two keys in the cert */

    ChkDR( DRM_BCert_GetPublicKeyByUsage( f_pbCertData,
                                          f_cbCertData,
                                          0,
                                          DRM_BCERT_KEYUSAGE_ENCRYPT_KEY,
                                          &f_pCachedValues->DeviceKeyEncrypt.PublicKey,
                                          &dwKeyUsageSet ) );

    DRMASSERT( dwKeyUsageSet > 0 );

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
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

