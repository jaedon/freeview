/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMBCERTBUILDER_C
#include <drmbcertbuilder.h>
#include <drmblackbox.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define SIZEOF_OBJ_HEADER          SIZEOF( DRM_BCERT_OBJECT_HEADER )
#define SIZEOF_BASIC_INFO          SIZEOF( DRM_BCERT_BASIC_INFO )
#define SIZEOF_PC_INFO             SIZEOF( DRM_BCERT_PC_INFO )
#define SIZEOF_SILVERLIGHT_INFO    SIZEOF( DRM_BCERT_SILVERLIGHT_INFO )
#define SIZEOF_DEVICE_INFO         SIZEOF( DRM_BCERT_DEVICE_INFO )
#define SIZEOF_SERVER_INFO         SIZEOF( DRM_BCERT_SERVER_INFO )
#define SIZEOF_SECURITY_VERSION    SIZEOF( DRM_BCERT_SECURITY_VERSION )

/*********************************************************************
**
** Function: _NonSecureSignWithCertKeys
**
** Synopsis:
**           Sign device cert with the supplied ECC256 private key.
**
** Arguments:
**  [f_pOEMContext]             -- A pointer to the OEM Context; may be NULL
**  [f_pCryptoCtx]              -- A pointer to the crypto context
**  [f_pbPrivKey]                    Key to be used in P256 signing
**  [f_cbData]                  -- Size of data to be signed
**  [f_pbData]                  -- Pointer data to be signed
**  [f_pcbSignature]            -- Returned size of signature
**  [f_pbSignature]             -- Signature created.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**
**********************************************************************/
static DRM_RESULT _NonSecureSignWithCertKeys(
    __in_opt                                        DRM_VOID                           *f_pOEMContext,
    __inout                                         DRM_CRYPTO_CONTEXT                 *f_pCryptoCtx,
    __in_ecount_opt( 1 )                            PRIVKEY_P256                       *f_pbPrivKey,
    __in                                            DRM_DWORD                           f_cbData,
    __in_bcount( f_cbData ) const                   DRM_BYTE                           *f_pbData,
    __inout_ecount( 1 )                             DRM_DWORD                          *f_pcbSignature,
    __out_bcount( *f_pcbSignature )                 DRM_BYTE                           *f_pbSignature
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pbData );
    ChkArg( NULL != f_pcbSignature );
    ChkArg( NULL != f_pbSignature );
    ChkArg( NULL != f_pCryptoCtx );

    /* Remove once embedded group keys are used */
    ChkArg( NULL != f_pbPrivKey );

    ChkDR( DRM_PK_Sign( f_pCryptoCtx->rgbCryptoContext,
                        eDRM_ECC_P256,
                        ECC_P256_PRIVKEY_SIZE_IN_BYTES,
                        ( DRM_BYTE* )f_pbPrivKey,
                        f_cbData,
                        f_pbData,
                        f_pcbSignature,
                        f_pbSignature ) );

ErrorExit:

    if( f_pbPrivKey != NULL )
    {
        OEM_ECC_ZeroPrivateKey_P256( f_pbPrivKey );
    }

    return dr;
}

/*****************************************************************************
** Function:    _verifySecurityVersion1PlatformIdForLegacyObject
**
** Synopsis:    Verify the platform id field in the security version object
**              for ensuring that the LEGACY SecurityVersion object does
**              not use any objects the server (RMSDK) won't understand.
**              DO NOT CHANGE WHAT VALUES THIS METHOD SUPPORTS!
**
** Arguments:   [f_dwPlatformId]   : platform id to verify
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER - if platform id is invalid
**
** Notes:       None
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _verifySecurityVersion1PlatformIdForLegacyObject(
    __in DRM_DWORD f_dwPlatformId)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( ( f_dwPlatformId != DRM_BCERT_SECURITY_VERSION_PLATFORM_WINDOWS )
     && ( f_dwPlatformId != DRM_BCERT_SECURITY_VERSION_PLATFORM_OSX )
     && ( f_dwPlatformId != DRM_BCERT_SECURITY_VERSION_PLATFORM_WM_7 ) )
    {
        ChkDR( DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _calcManufacturerInfoSize
**
** Synopsis:    Calculates the size of the optional Manufacturer Information element.
**
** Arguments:   [f_pManufacturerStrings] : A pointer to the manufacturing strings; may be NULL
**              [f_pcbSize]              : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm is NULL
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcManufacturerInfoSize(
   __in_ecount_opt(1)  const DRM_BCERT_MANUFACTURER_STRINGS  *f_pManufacturerStrings,
   __inout                   DRM_DWORD                       *f_pcbSize )
{
    DRM_RESULT  dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcManufacturerInfoSize );

    ChkArg( f_pcbSize != NULL );

    if ( f_pManufacturerStrings != NULL )
    {
        *f_pcbSize = SIZEOF_OBJ_HEADER
                     + SIZEOF( DRM_DWORD )                                          /* flags */
                     + SIZEOF( DRM_DWORD )                       /* manufacturer name length */
                     + f_pManufacturerStrings->ManufacturerName.cb      /* manufacturer name */
                     + PAD_AMOUNT( f_pManufacturerStrings->ManufacturerName.cb )  /* padding */
                     + SIZEOF( DRM_DWORD )                              /* model name length */
                     + f_pManufacturerStrings->ModelName.cb                    /* model name */
                     + PAD_AMOUNT( f_pManufacturerStrings->ModelName.cb )         /* padding */
                     + SIZEOF( DRM_DWORD )                            /* model number length */
                     + f_pManufacturerStrings->ModelNumber.cb                /* model number */
                     + PAD_AMOUNT( f_pManufacturerStrings->ModelNumber.cb );      /* padding */
    }
    else
    {
        *f_pcbSize = 0;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _calcKeyInfoSize
**
** Synopsis:    Calculates the size of the Public Key Information element.
**
** Arguments:   [f_dwNumCertificateKeys] : The number of entries in the keys array
**              [f_rgoCertificateKeys]   : Array of cert public key structures
**              [f_pcbSize]              : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS                    - on success
**              DRM_E_INVALIDARG               - if the output param is NULL,
**                                               or num of cert keys is zero
**              DRM_E_BCERT_INVALID_KEY_LENGTH - if the key length is zero
**
** Notes:       For future usability, this routine does not check for a specific
**              public key length, just that it is non-zero.
**
******************************************************************************/
static DRM_RESULT _calcKeyInfoSize(
  __in     const DRM_DWORD                  f_dwNumCertificateKeys,
  __in     const DRM_BCERT_BUILDER_CERTKEY *f_rgoCertificateKeys,
  __inout        DRM_DWORD                 *f_pcbSize )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    DRM_DWORD   iCount = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcKeyInfoSize );

    ChkArg( f_dwNumCertificateKeys != 0 );
    ChkArg( f_rgoCertificateKeys   != NULL );
    ChkArg( f_pcbSize              != NULL );

    *f_pcbSize = SIZEOF_OBJ_HEADER
               + SIZEOF( DRM_DWORD );    /* number of keys */

    for ( iCount = 0; iCount < f_dwNumCertificateKeys; iCount++ )
    {
        ChkBOOL( f_rgoCertificateKeys[iCount].wKeyLength != 0, DRM_E_BCERT_INVALID_KEY_LENGTH );

        *f_pcbSize += SIZEOF( DRM_WORD )                                                /* type                    */
                    + SIZEOF( DRM_WORD )                                                /* length                  */
                    + SIZEOF( DRM_DWORD )                                               /* flags                   */
                    + ( f_rgoCertificateKeys[iCount].wKeyLength / CHAR_BIT )            /* key value               */
                    + PAD_AMOUNT( f_rgoCertificateKeys[iCount].wKeyLength / CHAR_BIT )  /* key value padding       */
                    + SIZEOF( DRM_DWORD )                                               /* number of usage entries */
                    + f_rgoCertificateKeys[iCount].dwNumKeyUsages * SIZEOF(DRM_DWORD);  /* usage set               */
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _calcFeatureInfoSize
**
** Synopsis:    Calculates the size of the Feature Information element.
**
** Arguments:   [f_dwNumFeatureEntries] : The number of entries in the features array; may be zero
**              [f_pcbSize]             : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm is NULL
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcFeatureInfoSize(
  __in     const DRM_DWORD     f_dwNumFeatureEntries,
  __inout        DRM_DWORD    *f_pcbSize )
{
    DRM_RESULT  dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcFeatureInfoSize );

    ChkArg( f_pcbSize != NULL );

    *f_pcbSize = SIZEOF_OBJ_HEADER
                 + SIZEOF( DRM_DWORD )                               /* number of entries */
                 + ( f_dwNumFeatureEntries * SIZEOF( DRM_DWORD ) );  /* the entries */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _calcExtDataSignKeyInfoSize
**
** Synopsis:    Calculates the size of extended data signature key info
**
** Arguments:   [f_pExtData]            : A pointer to all extended data input
**              [f_pcbSize]             : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm is NULL
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcExtDataSignKeyInfoSize(
  __in     const DRM_BCERT_EXT_BUILDER_DATA  *f_pExtData,
  __inout        DRM_DWORD                   *f_pcbSize )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcExtDataSignKeyInfoSize );

    ChkArg( f_pExtData != NULL );
    ChkArg( f_pcbSize != NULL );

    if ( f_pExtData->fValid )
    {
        *f_pcbSize = SIZEOF_OBJ_HEADER
            + SIZEOF( DRM_WORD )                                      /* type              */
            + SIZEOF( DRM_WORD )                                      /* length            */
            + SIZEOF( DRM_DWORD )                                     /* flags             */
            + ( f_pExtData->wExtDataKeyLength / CHAR_BIT )            /* key value         */
            + PAD_AMOUNT( f_pExtData->wExtDataKeyLength / CHAR_BIT ); /* key value padding */
    }
    else
    {
        *f_pcbSize = 0;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _calcExtDataContainerSize
**
** Synopsis:    Calculates the size of extended data container object
**              including everything in it.
**
** Arguments:   [f_pExtData]            : A pointer to all extended data input
**              [f_pcbSize]             : A pointer to a container size to be updated
**              [f_pcbRecordSize]       : A pointer to a record size to be updated
**              [f_pcbSignatureSize]    : A pointer to a signature size to be updated
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the output parm is NULL
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcExtDataContainerSize(
  __in        const DRM_BCERT_EXT_BUILDER_DATA *f_pExtData,
  __inout           DRM_DWORD                  *f_pcbContainerSize,
  __inout_opt       DRM_DWORD                  *f_pcbRecordSize,
  __inout_opt       DRM_DWORD                  *f_pcbSignatureSize )
{
    DRM_RESULT  dr                = DRM_SUCCESS;
    DRM_DWORD   dwBlobLength      = 0;
    DRM_DWORD   dwRecordSize      = 0;
    DRM_DWORD   dwSignatureSize   = 0;
    DRM_WORD    wSignatureLength  = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcExtDataContainerSize );

    ChkArg( f_pExtData != NULL );
    ChkArg( f_pcbContainerSize != NULL );

    if ( f_pExtData->fValid )
    {
        /*
        ** Even we add extended data record to the cert later
        ** its length and type should be known now.
        */
        ChkBOOL( f_pExtData->dwNumExtDataRecords == 1, DRM_E_BCERT_INVALID_NUMBER_EXTDATARECORDS );
        ChkBOOL( f_pExtData->pExtDataRecords != NULL, DRM_E_BCERT_INVALID_NUMBER_EXTDATARECORDS );

        ChkBOOL( f_pExtData->pExtDataRecords[0].dwDataLength > 0, DRM_E_BCERT_INVALID_EXTDATARECORD );

        dwBlobLength = f_pExtData->pExtDataRecords[0].dwDataLength;

        if ( f_pExtData->wExtDataSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256 )
        {
            wSignatureLength = SIZEOF( SIGNATURE_P256 );
        }
        else
        {
            ChkDR( DRM_E_BCERT_INVALID_SIGNATURE_TYPE );
        }

        dwRecordSize = SIZEOF_OBJ_HEADER                             /* extended data record header */
                     + SIZEOF( DRM_DWORD )                           /* data length */
                     + dwBlobLength                                  /* data blob */
                     + PAD_AMOUNT( dwBlobLength );                   /* padding */

        dwSignatureSize = SIZEOF_OBJ_HEADER                          /* signature obj header */
                        + SIZEOF( DRM_WORD )                         /* signature type */
                        + SIZEOF( DRM_WORD )                         /* signature length */
                        + wSignatureLength                           /* signature data */
                        + PAD_AMOUNT( wSignatureLength );            /* padding */

        *f_pcbContainerSize = SIZEOF_OBJ_HEADER                      /* container obj header */
                            + dwRecordSize
                            + dwSignatureSize;

        DRMASSERT( PAD_AMOUNT(*f_pcbContainerSize) == 0 );  /* calculated size is always 32 bit aligned */
    }
    else
    {
        dwRecordSize = 0;
        dwSignatureSize = 0;
        *f_pcbContainerSize = 0;
    }
    if ( f_pcbRecordSize != NULL )
    {
        *f_pcbRecordSize = dwRecordSize;
    }
    if ( f_pcbSignatureSize != NULL )
    {
        *f_pcbSignatureSize = dwSignatureSize;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _calcSignatureInfoSize
**
** Synopsis:    Calculates the size of the Signature Information element.
**
** Arguments:   [f_wSignatureType]    : The type of signature signing the certificate
**              [f_dwIssuerKeyLength] : The issuer key length (in bits)
**              [f_pcbSize]           : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS                        - on success
**              DRM_E_INVALIDARG                   - if the output parm is NULL
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE - if the signature type is unsupported
**
** Notes:       This routine will need to be modified if/when new signature types are supported.
**
******************************************************************************/
static DRM_RESULT _calcSignatureInfoSize(
  __in     const DRM_WORD      f_wSignatureType,
  __in     const DRM_DWORD     f_dwIssuerKeyLength,
  __inout        DRM_DWORD    *f_pcbSize )
{
    DRM_RESULT  dr               = DRM_SUCCESS;
    DRM_WORD    wSignatureLength = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcSignatureInfoSize );

    ChkArg( f_pcbSize != NULL );

    /*
    ** The signature length (in bytes) is determined from the signature type
    */
    if ( f_wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256 )
    {
        wSignatureLength = SIZEOF( SIGNATURE_P256 );
    }
    else
    {
        ChkDR( DRM_E_BCERT_INVALID_SIGNATURE_TYPE );
    }

    *f_pcbSize = SIZEOF_OBJ_HEADER
                + SIZEOF( DRM_WORD )                             /* signature type    */
                + SIZEOF( DRM_WORD )                             /* signature length  */
                + wSignatureLength                               /* signature data    */
                + PAD_AMOUNT( wSignatureLength )                 /* padding           */
                + SIZEOF( DRM_DWORD )                            /* issuer key length */
                + ( f_dwIssuerKeyLength / CHAR_BIT )             /* issuer key value  */
                + PAD_AMOUNT( f_dwIssuerKeyLength / CHAR_BIT );  /* padding           */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _calcDomainInfoSize
**
** Synopsis:    Calculates the size of the Domain Information element.
**
** Arguments:   [f_dwDomainURLLength] : The domain URL length in bytes, including NULL terminator
**              [f_pcbSize]           : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS                     - on success
**              DRM_E_INVALIDARG                - if the output parm is NULL
**              DRM_E_BCERT_DOMAIN_URL_TOO_LONG - if the length is greater than the max supported
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcDomainInfoSize(
  __in     const  DRM_DWORD     f_dwDomainURLLength,
  __inout         DRM_DWORD    *f_pcbSize )
{
    DRM_RESULT  dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcDomainInfoSize );

    ChkArg( f_pcbSize != NULL );

    /*
    ** Check supported Domain URL Length
    */
    ChkBOOL( f_dwDomainURLLength <= DRM_BCERT_MAX_DOMAIN_URL_LENGTH, DRM_E_BCERT_DOMAIN_URL_TOO_LONG );

    *f_pcbSize = SIZEOF_OBJ_HEADER
                 + DRM_BCERT_SERVICE_ID_LENGTH
                 + DRM_BCERT_ACCOUNT_ID_LENGTH
                 + SIZEOF( DRM_DWORD )                 /* Revision          */
                 + SIZEOF( DRM_DWORD )                 /* Domain URL length */
                 + f_dwDomainURLLength                 /* Domain URL        */
                 + PAD_AMOUNT( f_dwDomainURLLength );  /* padding           */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _calcMeteringInfoSize
**
** Synopsis:    Calculates the size of the Metering Information element.
**
** Arguments:   [f_dwMeteringURLLength] : The metering URL length in bytes, including NULL terminator
**              [f_pcbSize]             : A pointer to a size variable to be updated
**
** Returns:     DRM_SUCCESS                       - on success
**              DRM_E_INVALIDARG                  - if the output parm is NULL
**              DRM_E_BCERT_METERING_URL_TOO_LONG - if the length is greater than the max supported
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcMeteringInfoSize(
  __in     const  DRM_DWORD     f_dwMeteringURLLength,
  __inout         DRM_DWORD    *f_pcbSize )
{
    DRM_RESULT  dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcMeteringInfoSize );

    ChkArg( f_pcbSize != NULL );

    /*
    ** Check supported Metering URL Length
    */
    ChkBOOL( f_dwMeteringURLLength <= DRM_BCERT_MAX_METERING_URL_LENGTH, DRM_E_BCERT_METERING_URL_TOO_LONG );

    *f_pcbSize = SIZEOF_OBJ_HEADER
                 + DRM_BCERT_METERING_ID_LENGTH
                 + SIZEOF( DRM_DWORD )                   /* Metering URL length */
                 + f_dwMeteringURLLength                 /* Metering URL        */
                 + PAD_AMOUNT( f_dwMeteringURLLength );  /* padding           */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _calcCertSize
**
** Synopsis:    Calculates two values:
**              - the size of the whole certificate being added, and
**              - the size of the certificate up to the Signature Info object.
**
** Arguments:   [f_pData]              : Pointer to all the input data
**              [f_pcbSize]            : Pointer to a variable to update w/ cert size (in bytes)
**              [f_pcbSignatureInfo]   : Pointer to a variable to update w/ signing size (in bytes)
**
** Returns:     DRM_SUCCESS                   - on success
**              DRM_E_INVALIDARG              - if any parameter is NULL
**              DRM_E_BCERT_INVALID_CERT_TYPE - if the specified certificate type is unknown
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _calcCertSize(
  __in    const DRM_BCERT_BUILDER_DATA  *f_pData,
  __inout       DRM_DWORD               *f_pcbSize,
  __inout       DRM_DWORD               *f_pcbSignatureInfo,
  __inout       DRM_DWORD               *f_pcbExtendedData )
{
    DRM_RESULT  dr                   = DRM_SUCCESS;
    DRM_DWORD   cbFeatureInfo        = 0;
    DRM_DWORD   cbKeyInfo            = 0;
    DRM_DWORD   cbManufacturerInfo   = 0;
    DRM_DWORD   cbExtDataSignKeyInfo = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_calcCertSize );

    ChkArg( f_pData            != NULL );
    ChkArg( f_pcbSize          != NULL );
    ChkArg( f_pcbSignatureInfo != NULL );

    /*
    ** Calculate the sizes of the variable-length common elements
    */
    ChkDR( _calcFeatureInfoSize( f_pData->dwNumFeatureEntries, &cbFeatureInfo ) );
    ChkDR( _calcKeyInfoSize( f_pData->dwNumCertificateKeys, f_pData->pCertificateKeys, &cbKeyInfo ) );
    ChkDR( _calcSignatureInfoSize( f_pData->wSignatureType, f_pData->dwIssuerKeyLength, f_pcbSignatureInfo ) );
    ChkDR( _calcManufacturerInfoSize( f_pData->pManufacturerStrings, &cbManufacturerInfo ) );
    ChkDR( _calcExtDataSignKeyInfoSize( &f_pData->ExtendedData, &cbExtDataSignKeyInfo ) );
    ChkDR( _calcExtDataContainerSize( &f_pData->ExtendedData, f_pcbExtendedData, NULL, NULL ) );

    /*
    ** Add-up the sizes of all the common elements
    */
    *f_pcbSize = SIZEOF( DRM_BCERT_HEADER )
                 + SIZEOF_BASIC_INFO
                 + cbFeatureInfo
                 + cbKeyInfo
                 + cbManufacturerInfo
                 + cbExtDataSignKeyInfo
                 + *f_pcbSignatureInfo
                 + *f_pcbExtendedData;     /* Always add ext data to the certificate size */

    /*
    ** Add-on the size of any cert-specific element
    */
    switch ( f_pData->dwType )
    {
        case DRM_BCERT_CERTTYPE_ISSUER:
        case DRM_BCERT_CERTTYPE_CRL_SIGNER:
        case DRM_BCERT_CERTTYPE_SERVICE:
        case DRM_BCERT_CERTTYPE_APPLICATION:
        case DRM_BCERT_CERTTYPE_KEYFILESIGNER:
        case DRM_BCERT_CERTTYPE_LICENSESIGNER:
        {
            break;  /* accurate size already calculated above */
        }

        case DRM_BCERT_CERTTYPE_PC:
        {
            *f_pcbSize += SIZEOF_PC_INFO;
            break;
        }

        case DRM_BCERT_CERTTYPE_SILVERLIGHT:
        {
            *f_pcbSize += SIZEOF_SILVERLIGHT_INFO;
            break;
        }

        case DRM_BCERT_CERTTYPE_DEVICE:
        {
            *f_pcbSize += SIZEOF_DEVICE_INFO;
            break;
        }

        case DRM_BCERT_CERTTYPE_DOMAIN:
        {
            DRM_DWORD   cbDomainInfo = 0;
            ChkBOOL( f_pData->Type.DomainData.pDomainURL     != NULL, DRM_E_BCERT_DOMAIN_URL_NOT_SPECIFIED );
            ChkBOOL( f_pData->Type.DomainData.pDomainURL->cb >  0,    DRM_E_BCERT_DOMAIN_URL_NOT_SPECIFIED );
            _calcDomainInfoSize( f_pData->Type.DomainData.pDomainURL->cb, &cbDomainInfo );
            *f_pcbSize += cbDomainInfo;
            break;
        }

        case DRM_BCERT_CERTTYPE_METERING:
        {
            DRM_DWORD   cbMeteringInfo = 0;
            ChkBOOL( f_pData->Type.MeteringData.pMeteringURL     != NULL, DRM_E_BCERT_METERING_URL_NOT_SPECIFIED );
            ChkBOOL( f_pData->Type.MeteringData.pMeteringURL->cb >  0,    DRM_E_BCERT_METERING_URL_NOT_SPECIFIED );
            _calcMeteringInfoSize( f_pData->Type.MeteringData.pMeteringURL->cb, &cbMeteringInfo );
            *f_pcbSize += cbMeteringInfo;
            break;
        }

        case DRM_BCERT_CERTTYPE_SERVER:
        {
            *f_pcbSize += SIZEOF_SERVER_INFO;
            break;
        }

        default:
        {
            ChkDR( DRM_E_BCERT_INVALID_CERT_TYPE );
            break;
        }
    }

    /*
     * Conditionally add the size of the Security Version Object
     */
    if( f_pData->SecurityVersion.fValid )
    {
        if( _verifySecurityVersion1PlatformIdForLegacyObject(f_pData->SecurityVersion.dwPlatformIdentifier) == DRM_SUCCESS )
        {
            *f_pcbSize += SIZEOF_SECURITY_VERSION;
        }

        /*
        ** Add security version 2 object to PC, device, and issuer certs
        */
        if( f_pData->dwType == DRM_BCERT_CERTTYPE_PC
         || f_pData->dwType == DRM_BCERT_CERTTYPE_DEVICE
         || f_pData->dwType == DRM_BCERT_CERTTYPE_ISSUER )
        {
            *f_pcbSize += SIZEOF_SECURITY_VERSION;
        }
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _checkBuffer
**
** Synopsis:    Determines if the buffer provided by the user to contain the
**              new certificate chain is large enough.
**
** Arguments:   [f_pbParentChain] : A buffer pointer to an existing certificate chain; may be NULL
**              [f_pbNewChain]    : A buffer pointer for the new certificate chain; may be NULL
**              [f_pcbNewChain]   : A pointer to new chain buffer size (in bytes)
**              [f_dwCertSize]    : The size of the new certificate being added to chain
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_INVALIDARG     - if the size parm is NULL, or if the new cert size is zero
**              DRM_E_BUFFERTOOSMALL - if provided new chain buffer is missing or too small
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _checkBuffer (
    __in_opt                         const DRM_BYTE  *f_pbParentChain,
    __out_ecount_opt(*f_pcbNewChain) const DRM_BYTE  *f_pbNewChain,
    __inout                                DRM_DWORD *f_pcbNewChain,
    __in                             const DRM_DWORD  f_dwCertSize )
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_DWORD   dwMemoryNeeded = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_checkBuffer );

    ChkArg( f_pcbNewChain != NULL );
    ChkArg( f_dwCertSize  != 0    );

    /*
    ** If this is the first certificate in the chain, start with the size of the chain header.
    ** Otherwise, we are adding an additional certificate, so start with the memory needed
    ** for the existing/parent chain.
    */
    if ( f_pbParentChain == NULL )
    {
        dwMemoryNeeded = SIZEOF_CHAIN_HEADER;
    }
    else
    {
        NETWORKBYTES_TO_DWORD( dwMemoryNeeded, f_pbParentChain, DRM_BCERT_CHAIN_SIZE_OFFSET );
    }

    /*
    ** Add the size of the new certificate being added
    */
    dwMemoryNeeded += f_dwCertSize;

    /*
    ** Inform caller of memory requirement if the provided buffer is missing or insufficient
    */
    if ( ( f_pbNewChain == NULL ) || ( *f_pcbNewChain < dwMemoryNeeded ) )
    {
        *f_pcbNewChain = dwMemoryNeeded;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /*
    ** Buffer is big enough, update the parameter with the exact size of the new chain
    */
    *f_pcbNewChain = dwMemoryNeeded;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addAlignedData
**
** Synopsis:    Adds a sequence of bytes to a binary certificate, with padding
**              characters inserted at the end to ensure alignment on a 32-bit boundary.
**
** Arguments:   [f_pbDestination] : A pointer to a buffer to receive the bytes
**              [f_pdwDestOffset] : A pointer to the offset within the destination buffer; updated
**              [f_pbSource]      : Address of where to get the bytes from
**              [f_cbAmount]      : How many bytes to copy
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_pbDestination length defined by f_cbAmount and f_pdwDestOffset")
static DRM_RESULT _addAlignedData(
    __out_bcount(f_cbAmount + *f_pdwDestOffset + 3) DRM_BYTE   *f_pbDestination,
    __inout                                         DRM_DWORD  *f_pdwDestOffset,
    __in_bcount(f_cbAmount)                   const DRM_BYTE   *f_pbSource,
    __in                                      const DRM_DWORD   f_cbAmount )
PREFAST_POP
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_DWORD   dwPadCount = 0;
    DRM_BYTE    bPadValue  = '\0';


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addAlignedData );

    ChkArg( f_pbDestination != NULL );
    ChkArg( f_pdwDestOffset != NULL );
    ChkArg( f_pbSource      != NULL );
    ChkArg( f_cbAmount       > 0    );

    DRM_BYT_CopyBytes( f_pbDestination, *f_pdwDestOffset, f_pbSource, 0, f_cbAmount );
    *f_pdwDestOffset += f_cbAmount;

    /* Padding, if needed to align on 32-bit boundary */
    for ( dwPadCount = PAD_AMOUNT( f_cbAmount ); dwPadCount > 0; dwPadCount-- )
    {
        PUT_BYTE( f_pbDestination, (*f_pdwDestOffset)++, bPadValue );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    BCert_UpdateCertChainHeader
**
** Synopsis:    Increases the following two values in the certificate chain header:
**              - the total number of bytes in the certificate chain
**              - the count of the number of certificates in the certificate chain
**
** Arguments:   [f_pbBuffer] : A buffer pointer containing a certificate chain
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_cbCert]   : How many bytes were in the new certifcate just added
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if either parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL BCert_UpdateCertChainHeader(
    __inout_ecount(f_cbBuffer)       DRM_BYTE  *f_pbBuffer,
    __in                       const DRM_DWORD  f_cbBuffer,
    __in                       const DRM_DWORD  f_cbCert )   
{
    DRM_RESULT         dr      = DRM_SUCCESS;
    DRM_DWORD          cbChain = 0;
    DRM_DWORD          cCerts  = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_updateCertChainHeader );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_cbCert    > 0 );
    ChkArg( f_cbBuffer >= DRM_BCERT_CHAIN_SIZE_OFFSET + SIZEOF(DRM_DWORD) );
    ChkArg( f_cbBuffer >= DRM_BCERT_CHAIN_COUNT_OFFSET + SIZEOF(DRM_DWORD) );

    /*
    ** Update the length in bytes of binary certificate chain structure, including this structure.
    ** Get the current value, increment it, then set the new value.
    */
    NETWORKBYTES_TO_DWORD( cbChain, f_pbBuffer, DRM_BCERT_CHAIN_SIZE_OFFSET);
    cbChain += f_cbCert;
    DWORD_TO_NETWORKBYTES( f_pbBuffer, DRM_BCERT_CHAIN_SIZE_OFFSET, cbChain);

    /*
    ** Update the number of entries in this certificate chain.
    ** Get the current value, increment it, then set the new value.
    */
    NETWORKBYTES_TO_DWORD( cCerts, f_pbBuffer, DRM_BCERT_CHAIN_COUNT_OFFSET);
    cCerts += 1;
    DWORD_TO_NETWORKBYTES( f_pbBuffer, DRM_BCERT_CHAIN_COUNT_OFFSET, cCerts );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertChainHeader
**
** Synopsis:    Adds the certificate chain header to the certificate chain.
**
** Arguments:   [f_pbBuffer] : A pointer to an empty bufffer to contain the new certificate chain
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_dwFlags]  : The flags (a bit-field) to set in the certificate chain header
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the buffer pointer is NULL
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertChainHeader(
    __inout_ecount(f_cbBuffer)       DRM_BYTE  *f_pbBuffer,
    __in                       const DRM_DWORD  f_cbBuffer,
    __in                       const DRM_DWORD  f_dwFlags )
{
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_DWORD  iBuffer           = 0;

    /*
    ** The following variables are necessary because the big-endian version of
    ** the DWORD_TO_NETWORKBYTES macro cannot take a constant as the last parameter.
    */
    DRM_DWORD  dwChainHeaderTag  = DRM_BCERT_CHAIN_HEADER_TAG;
    DRM_DWORD  dwChainVersion    = DRM_BCERT_CHAIN_VERSION;
    DRM_DWORD  dwChainHeaderSize = SIZEOF_CHAIN_HEADER;
    DRM_DWORD  dwZero            = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertChainHeader );

    ChkArg( f_pbBuffer != NULL );

    /*
    ** Add the Chain Header Tag & Chain Version
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwChainHeaderTag );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwChainVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the length in bytes of binary certificate chain structure, including this structure.
    ** This will be updated after each certificate is added.
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwChainHeaderSize );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add any flags
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwFlags );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Number of entries in this certificate chain.
    ** This will be updated after each certificate is added.
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwZero );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertHeader
**
** Synopsis:    Adds a certificate header.
**
** Arguments:   [f_pbBuffer]     : The address at which to start writing these data
**              [f_cbBuffer]     : Size of f_pbBuffer in bytes
**              [f_dwCertSize]   : The full size of the certificate being added (in bytes)
**              [f_dwSignedSize] : The signed-size of the certificate being added (in bytes)
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the buffer pointer is NULL or the sizes are not correct
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertHeader(
    __inout_ecount(f_cbBuffer)       DRM_BYTE  *f_pbBuffer,
    __in                       const DRM_DWORD  f_cbBuffer,
    __in                       const DRM_DWORD  f_dwCertSize,
    __in                       const DRM_DWORD  f_dwSignedSize )
{
    DRM_RESULT  dr            = DRM_SUCCESS;
    DRM_DWORD   iBuffer       = 0;

    /*
    ** The following variables are necessary because the big-endian version of
    ** the DWORD_TO_NETWORKBYTES macro cannot take a constant as the last parameter.
    */
    DRM_DWORD   dwHeaderTag   = DRM_BCERT_HEADER_TAG;
    DRM_DWORD   dwCertVersion = DRM_BCERT_VERSION;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertHeader );

    ChkArg( f_pbBuffer   != NULL );
    ChkArg( f_dwCertSize  > f_dwSignedSize );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwHeaderTag );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, dwCertVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwCertSize );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwSignedSize );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertObjectHeader
**
** Synopsis:    Adds the header of a certificate object.
**
** Arguments:   [f_pbBuffer] : The address at which to start writing these data
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_dwFlags]  : The flags (a bit-field) for this object header
**              [f_dwType]   : The type of certificate object that this is for
**              [f_cbLength] : The length of this certificate object
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**
** Notes:       For future usability, this routine does not check the validity
**              of the certificate object type, just that it is non-zero.
**
******************************************************************************/
static DRM_RESULT _addCertObjectHeader(
    __inout_ecount(f_cbBuffer)       DRM_BYTE    *f_pbBuffer,
    __in                       const DRM_DWORD    f_cbBuffer,
    __in                       const DRM_WORD     f_dwFlags,
    __in                       const DRM_WORD     f_dwType,
    __in                       const DRM_DWORD    f_cbLength )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertObjectHeader );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_dwType   != 0 );
    ChkArg( f_cbLength != 0 );

    /*
    ** Add the Flags, Object Type, then Object Length.
    */
    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwFlags );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwType );
    iBuffer += SIZEOF( DRM_WORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_cbLength );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertBasicInfo
**
** Synopsis:    Adds the Basic Information object to a certificate.
**
** Arguments:   [f_pbBuffer] : The address at which to start writing these data
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_pData]    : A pointer to all the input data
**
** Returns:     DRM_SUCCESS                          - on success
**              DRM_E_INVALIDARG                     - if either parameter is invalid
**              DRM_E_BCERT_CERT_ID_NOT_SPECIFIED    - if the certificate ID is NULL
**              DRM_E_BCERT_PUBLIC_KEY_NOT_SPECIFIED - if the certificate's public key is NULL
**              DRM_E_BCERT_INVALID_SECURITY_LEVEL   - if the security level is not correct
**              or other return code from a function called within
**
** Notes:       This object is a fixed length in size.
**              For future usability, this routine does not check for specific security
**              level values, just that it is non-zero.
**
******************************************************************************/
static DRM_RESULT _addCertBasicInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                  *f_pbBuffer,
    __in                       const DRM_DWORD                  f_cbBuffer,
    __in                             DRM_BCERT_BUILDER_DATA    *f_pData )
{
    DRM_RESULT            dr          = DRM_SUCCESS;
    DRM_DWORD             iBuffer     = SIZEOF_OBJ_HEADER;
    DRM_SHA256_Context    SHAContext  = {0};
    DRM_SHA256_Digest     DigestValue = {0};
    const DRM_BYTE        rgbZeroClientID[DRM_BCERT_CLIENT_ID_LENGTH] = {0};


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertBasicInfo );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pData    != NULL );

    /*
    ** Ensure that the Client ID is zero for Issuer, CRL Signer, and Service certificates
    */
    if ( ( f_pData->dwType == DRM_BCERT_CERTTYPE_ISSUER
         || f_pData->dwType == DRM_BCERT_CERTTYPE_CRL_SIGNER
         || f_pData->dwType == DRM_BCERT_CERTTYPE_SERVICE )
      && ( MEMCMP( f_pData->ClientID.rgb, rgbZeroClientID, DRM_BCERT_CLIENT_ID_LENGTH ) != 0 ) )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    /*
    ** Check data pointers & security level
    */
    ChkBOOL( f_pData->pCertificateID  != NULL, DRM_E_BCERT_CERT_ID_NOT_SPECIFIED    );
    ChkBOOL( f_pData->dwNumCertificateKeys != 0 && f_pData->pCertificateKeys != NULL,
                DRM_E_BCERT_PUBLIC_KEY_NOT_SPECIFIED );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_BASIC,
                                 SIZEOF_BASIC_INFO ) );

    /*
    ** Add the Certificate ID, Security Level, then the Flags
    */
    DRM_BYT_CopyBytes( f_pbBuffer, iBuffer, f_pData->pCertificateID->rgb, 0, DRM_BCERT_CERT_ID_LENGTH );
    iBuffer += DRM_BCERT_CERT_ID_LENGTH;

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwSecurityLevel );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwBasicFlags );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Type (validity was determined previously when the cert size was calculated
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwType );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Calculate the Digest Value by SHA256 hashing the public key of this certificate
    */
    ChkDR( DRM_SHA256_Init( &SHAContext ) );
    ChkDR( DRM_SHA256_Update( &SHAContext,
                              f_pData->pCertificateKeys[0].KeyValue.m_rgbPubkey,
                              f_pData->pCertificateKeys[0].wKeyLength / (DRM_DWORD)CHAR_BIT ) );
    ChkDR( DRM_SHA256_Finalize( &SHAContext, &DigestValue ) );

    /*
    ** Add the Digest Value (calculated immediately above)
    */
    DRM_BYT_CopyBytes( f_pbBuffer, iBuffer, DigestValue.m_rgbDigest, 0, SHA256_DIGEST_SIZE_IN_BYTES );
    iBuffer += SHA256_DIGEST_SIZE_IN_BYTES;

    /*
    ** Add the Expiration Date
    */
    if ( f_pData->dwExpirationDate == 0 )
    {
        f_pData->dwExpirationDate = DRM_BCERT_DEFAULT_EXPIRATION_DATE;
    }
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwExpirationDate );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Client ID
    */
    DRM_BYT_CopyBytes( f_pbBuffer, iBuffer, f_pData->ClientID.rgb, 0, DRM_BCERT_CLIENT_ID_LENGTH );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertFeatureInfo
**
** Synopsis:    Adds the Feature Information object to a certificate.
**
** Arguments:   [f_pbBuffer]            : The address at which to start writing these data
**              [f_cbBuffer]            : Size of f_pbBuffer in bytes
**              [f_dwNumFeatureEntries] : The number of features in the features array; may be zero
**              [f_prgdwFeatureSet]     : A pointer to the features array; may be NULL
**              [f_pcbSize]             : Variable pointer to update w/ size of this object (in bytes)
**
** Returns:     DRM_SUCCESS                 - on success
**              DRM_E_INVALIDARG            - if any parameter is invalid
**              DRM_E_BCERT_INVALID_FEATURE - if any feature ID is not a supported value
**              or other return code from a function called within
**
** Notes:       For future usability, this routine does not check for appropriate certificate
**              type to feature mapping; that verification is part of the certificate parser.
**
******************************************************************************/
static DRM_RESULT _addCertFeatureInfo(
    __inout_ecount(f_cbBuffer)                   DRM_BYTE   *f_pbBuffer,
    __in                                   const DRM_DWORD   f_cbBuffer,
    __in                                   const DRM_DWORD   f_dwNumFeatureEntries,
    __in_ecount_opt(f_dwNumFeatureEntries) const DRM_DWORD  *f_prgdwFeatureSet,
    __inout                                      DRM_DWORD  *f_pcbSize )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iBuffer  = SIZEOF_OBJ_HEADER;
    DRM_DWORD   iFeature = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertFeatureInfo );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pcbSize  != NULL );

    ChkDR( _calcFeatureInfoSize( f_dwNumFeatureEntries, f_pcbSize ) );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_FEATURE,
                                 *f_pcbSize ) );

    /*
    ** Add the Number of Feature Entries
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwNumFeatureEntries );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Feature Set
    */
    for ( iFeature = 0; iFeature < f_dwNumFeatureEntries; iFeature++ )
    {
        ChkArg( f_prgdwFeatureSet != NULL );

        if ( f_prgdwFeatureSet[ iFeature ] == 0
          || f_prgdwFeatureSet[ iFeature ] > DRM_BCERT_FEATURE_MAX_VALUE )
        {
            ChkDR( DRM_E_BCERT_INVALID_FEATURE );
        }

        DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_prgdwFeatureSet[ iFeature ] );
        iBuffer += SIZEOF( DRM_DWORD );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertKeyInfo
**
** Synopsis:    Adds the Key Information object to a certificate.
**
** Arguments:   [f_pbBuffer] : The address at which to start writing these data
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_pData]    : A pointer to all the input data
**              [f_pcbSize]  : Variable pointer to update w/ the size of this object (in bytes)
**
** Returns:     DRM_SUCCESS                          - on success
**              DRM_E_INVALIDARG                     - if any parameter is invalid
**              DRM_E_BCERT_PUBLIC_KEY_NOT_SPECIFIED - if the certificate's public key is NULL
**              DRM_E_BCERT_KEY_USAGES_NOT_SPECIFIED - if the key usage set is empty
**              DRM_E_BCERT_INVALID_KEY_TYPE         - if the specified key type is unsupported
**              DRM_E_BCERT_INVALID_KEY_LENGTH       - if the specified key length is unsupported
**              DRM_E_BCERT_INVALID_KEY_USAGE        - if any key usage ID is not a supported value
**              or other return code from a function called within
**
** Notes:       For future usability, this routine does not check for appropriate certificate
**              type to key-usage mapping; that verification is part of the certificate parser.
**
******************************************************************************/
static DRM_RESULT _addCertKeyInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                *f_pbBuffer,
    __in                       const DRM_DWORD                f_cbBuffer,
    __in                       const DRM_BCERT_BUILDER_DATA  *f_pData,
    __inout                          DRM_DWORD               *f_pcbSize )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iBuffer  = SIZEOF_OBJ_HEADER;
    DRM_DWORD   iUsage   = 0;
    DRM_DWORD   iCount   = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertKeyInfo );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pData    != NULL );
    ChkArg( f_pcbSize  != NULL );

    ChkDR( _calcKeyInfoSize( f_pData->dwNumCertificateKeys, f_pData->pCertificateKeys, f_pcbSize ) );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_KEY,
                                 *f_pcbSize ) );

    /*
    ** Add number of cert keys
    */
    ChkBOOL( f_pData->dwNumCertificateKeys != 0 && f_pData->pCertificateKeys != NULL,
                DRM_E_BCERT_PUBLIC_KEY_NOT_SPECIFIED );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwNumCertificateKeys );
    iBuffer += SIZEOF( DRM_DWORD );

    for ( iCount = 0; iCount < f_pData->dwNumCertificateKeys; iCount++ )
    {
        /*
        ** Check supported key types and key lengths
        */
        ChkBOOL( f_pData->pCertificateKeys[iCount].wKeyLength == ECC_P256_PUBKEY_SIZE_IN_BITS,
                DRM_E_BCERT_INVALID_KEY_LENGTH );
        ChkBOOL( f_pData->pCertificateKeys[iCount].wKeyType == DRM_BCERT_KEYTYPE_ECC256,
                DRM_E_BCERT_INVALID_KEY_TYPE );
        /*
        ** Ensure that the key's usage set is provided
        */
        ChkBOOL( f_pData->pCertificateKeys[iCount].dwNumKeyUsages != 0
              && f_pData->pCertificateKeys[iCount].pKeyUsageSet != NULL,
                DRM_E_BCERT_KEY_USAGES_NOT_SPECIFIED );

        /*
        ** Add the Key Type, Key Length, Key Flags, Key Value, Number of Usages
        */
        WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->pCertificateKeys[iCount].wKeyType );
        iBuffer += SIZEOF( DRM_WORD );

        WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->pCertificateKeys[iCount].wKeyLength );
        iBuffer += SIZEOF( DRM_WORD );

        DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->pCertificateKeys[iCount].dwKeyFlags );
        iBuffer += SIZEOF( DRM_DWORD );

        ChkDR( _addAlignedData( f_pbBuffer,
                                &iBuffer,
                                f_pData->pCertificateKeys[iCount].KeyValue.m_rgbPubkey,
                                (f_pData->pCertificateKeys[iCount].wKeyLength / (DRM_DWORD)CHAR_BIT ) ) );

        DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->pCertificateKeys[iCount].dwNumKeyUsages );
        iBuffer += SIZEOF( DRM_DWORD );

        /*
        ** Add the Key Usage Set
        */
        for ( iUsage = 0; iUsage < f_pData->pCertificateKeys[iCount].dwNumKeyUsages; iUsage++ )
        {
            if ( f_pData->pCertificateKeys[iCount].pKeyUsageSet[iUsage] == 0
              || f_pData->pCertificateKeys[iCount].pKeyUsageSet[iUsage] > DRM_BCERT_KEYUSAGE_MAX_VALUE )
            {
                ChkDR( DRM_E_BCERT_INVALID_KEY_USAGE );
            }
            DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->pCertificateKeys[iCount].pKeyUsageSet[iUsage] );
            iBuffer += SIZEOF( DRM_DWORD );
        }
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertManufacturerString
**
** Synopsis:    Adds a single manufacturer information string to a certificate.
**
** Arguments:   [f_pbBuffer]              : The buffer in which to write these data
**              [f_cbBuffer]              : Size of f_pbBuffer in bytes
**              [f_pszManufacturerString] : Pointer to the string to add
**              [f_cbManufacturerString]  : The number of bytes in the string to add
**              [f_piBuffer]              : Index within the buffer for these data; updated
**
** Returns:     DRM_SUCCESS                            - on success
**              DRM_E_INVALIDARG                       - if any parameter is invalid
**              DRM_E_BCERT_STRING_NOT_NULL_TERMINATED - if the string is not null terminated
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertManufacturerString(
    __inout_ecount(f_cbBuffer)                  DRM_BYTE   *f_pbBuffer,
    __in                                  const DRM_DWORD   f_cbBuffer,
    __in_bcount_z(f_cbManufacturerString) const DRM_BYTE   *f_pszManufacturerString,
    __in                                  const DRM_DWORD   f_cbManufacturerString,
    __inout                                     DRM_DWORD  *f_piBuffer )
{
    DRM_RESULT  dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertManufacturerString );

    ChkArg( f_pbBuffer              != NULL );
    ChkArg( f_pszManufacturerString != NULL );
    ChkArg( f_piBuffer              != NULL );

    /*
    ** Add the string length and then the string itself
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, f_cbManufacturerString );
    *f_piBuffer += SIZEOF( DRM_DWORD );

    if ( f_cbManufacturerString > 0 )
    {
        /*
        ** Ensure that: 1) the string is NULL terminated and 2) the count includes the NULL
        */
        ChkBOOL( f_pszManufacturerString[ f_cbManufacturerString - 1 ] == '\0', DRM_E_BCERT_STRING_NOT_NULL_TERMINATED );

        ChkDR( _addAlignedData( f_pbBuffer, f_piBuffer, f_pszManufacturerString, f_cbManufacturerString ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertManufacturerInfo
**
** Synopsis:    Adds the optional Manufacturer Information object to a certificate.
**
** Arguments:   [f_pbBuffer]             : The address at which to start writing these data
**              [f_cbBuffer]             : Size of f_pbBuffer in bytes
**              [f_dwManufacturerFlags]  : The flags (a bit-field) for this object
**              [f_pManufacturerStrings] : Pointer to the manufacturer strings; may be NULL
**              [f_pcbSize]              : Variable pointer to update w/ size of this object (in bytes)
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if any parameter is invalid
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertManufacturerInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                        *f_pbBuffer,
    __in                       const DRM_DWORD                        f_cbBuffer,
    __in                       const DRM_DWORD                        f_dwManufacturerFlags,
    __in_ecount_opt(1)         const DRM_BCERT_MANUFACTURER_STRINGS  *f_pManufacturerStrings,
    __inout                          DRM_DWORD                       *f_pcbSize )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = SIZEOF_OBJ_HEADER;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertManufacturerInfo );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pcbSize  != NULL );

    ChkDR( _calcManufacturerInfoSize( f_pManufacturerStrings, f_pcbSize ) );

    if ( f_pManufacturerStrings != NULL )
    {
        /*
        ** Add the Object Header
        */
        ChkDR( _addCertObjectHeader( f_pbBuffer,
                                     f_cbBuffer,
                                     DRM_BCERT_OBJFLAG_EMPTY,
                                     DRM_BCERT_OBJTYPE_MANUFACTURER,
                                     *f_pcbSize ) );

        /*
        ** Add the Manufacturer Flags
        */
        DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_dwManufacturerFlags);
        iBuffer += SIZEOF( DRM_DWORD );

        /*
        ** Add the Manufacturer Name, Model Name, then Model Number
        */
        ChkDR( _addCertManufacturerString( f_pbBuffer,
                                           f_cbBuffer,
                                           f_pManufacturerStrings->ManufacturerName.rgb,
                                           f_pManufacturerStrings->ManufacturerName.cb,
                                           &iBuffer ) );

        ChkDR( _addCertManufacturerString( f_pbBuffer,
                                           f_cbBuffer,
                                           f_pManufacturerStrings->ModelName.rgb,
                                           f_pManufacturerStrings->ModelName.cb,
                                           &iBuffer ) );

        ChkDR( _addCertManufacturerString( f_pbBuffer,
                                           f_cbBuffer,
                                           f_pManufacturerStrings->ModelNumber.rgb,
                                           f_pManufacturerStrings->ModelNumber.cb,
                                           &iBuffer ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}




/*****************************************************************************
** Function:    _addExtDataSignKeyInfo
**
** Synopsis:    Adds extended data signature key info
**
** Arguments:   [f_pbBuffer] : The address at which to start writing these data
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_pExtData] : Pointer to all extended data input
**              [f_pcbSize]  : Variable pointer to update w/ size of this object (in bytes)
**
** Returns:     DRM_SUCCESS                             - on success
**              DRM_E_INVALIDARG                        - if any parameter is invalid
**              DRM_E_BCERT_EXTDATA_LENGTH_MUST_PRESENT - if extended data record length is not present
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addExtDataSignKeyInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                   *f_pbBuffer,
    __in                       const DRM_DWORD                   f_cbBuffer,
    __in                       const DRM_BCERT_EXT_BUILDER_DATA *f_pExtData,
    __inout                          DRM_DWORD                  *f_pcbSize )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = SIZEOF_OBJ_HEADER;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addExtDataSignKeyInfo );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pExtData != NULL );
    ChkArg( f_pcbSize  != NULL );

    ChkDR( _calcExtDataSignKeyInfoSize( f_pExtData, f_pcbSize ) );

    if ( f_pExtData->fValid )
    {
        /*
        ** Add the Object Header
        */
        ChkDR( _addCertObjectHeader( f_pbBuffer,
                                     f_cbBuffer,
                                     DRM_BCERT_OBJFLAG_EMPTY,
                                     DRM_BCERT_OBJTYPE_EXTDATASIGNKEY,
                                     *f_pcbSize ) );
        /*
        ** Add the Key Type, Key Length, Key Flags, Key Value
        */
        WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pExtData->wExtDataKeyType );
        iBuffer += SIZEOF( DRM_WORD );

        WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pExtData->wExtDataKeyLength );
        iBuffer += SIZEOF( DRM_WORD );

        DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pExtData->dwExtDataKeyFlags );
        iBuffer += SIZEOF( DRM_DWORD );

        ChkDR( _addAlignedData( f_pbBuffer,
                                &iBuffer,
                                f_pExtData->oPubKey.m_rgbPubkey,
                                (f_pExtData->wExtDataKeyLength / (DRM_DWORD)CHAR_BIT ) ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _addExtDataContainer
**
** Synopsis:    Adds extended data container object including everything it contains
**
** Arguments:   [f_pbBuffer]    : The address at which to start writing these data
**              [f_cbBuffer]    : Size of f_pbBuffer in bytes
**              [f_pExtData]    : Pointer to all extended data input
**              [f_pcbSize]     : Variable pointer to update w/ size of this object (in bytes)
**              [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS                           - on success
**              DRM_E_INVALIDARG                      - if any parameter is invalid
**              DRM_E_BCERT_PRIVATE_KEY_NOT_SPECIFIED - if the private key is NULL
**              DRM_E_BCERT_INVALID_KEY_LENGTH        - if the issuer key length is unsupported
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE    - if the signature type is unsupported
**              or other return code from a function called within
**
** Notes:       If actual record is not available yet the function does nothing.
**              But if we plan to add the record later total cert length should account for that.
**
******************************************************************************/
static DRM_RESULT _addExtDataContainer(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                   *f_pbBuffer,
    __in                       const DRM_DWORD                   f_cbBuffer,
    __in                       const DRM_BCERT_EXT_BUILDER_DATA *f_pExtData,
    __inout                          DRM_DWORD                  *f_pcbSize,
    __inout                          DRM_CRYPTO_CONTEXT         *f_pCryptoCtx )
{
    DRM_RESULT      dr                 = DRM_SUCCESS;
    DRM_DWORD       iBuffer            = 0;
    const DRM_BYTE *pbSignedData       = NULL;
    DRM_DWORD       dwSignedDataLength = 0;
    DRM_DWORD       dwRecordSize       = 0;
    DRM_DWORD       dwSignatureSize    = 0;
    PRIVKEY_P256    oPrivKey           = {0};
    SIGNATURE_P256  Signature          = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addExtDataContainer );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pExtData != NULL );
    ChkArg( f_pcbSize  != NULL );

    ChkDR( _calcExtDataContainerSize( f_pExtData, f_pcbSize, &dwRecordSize, &dwSignatureSize ) );

    if ( f_pExtData->fValid )
    {
        /*
        ** Even we add extended data record to the cert later
        ** its length and type should be known.
        */
        ChkBOOL( f_pExtData->dwNumExtDataRecords == 1, DRM_E_BCERT_INVALID_NUMBER_EXTDATARECORDS );
        ChkBOOL( f_pExtData->pExtDataRecords != NULL, DRM_E_BCERT_INVALID_NUMBER_EXTDATARECORDS );

        ChkBOOL( f_pExtData->pExtDataRecords[0].dwDataLength > 0, DRM_E_BCERT_INVALID_EXTDATARECORD );

        if ( f_pExtData->pExtDataRecords[0].pbData != NULL )
        {
            /*
            ** Container object header
            */
            ChkDR( _addCertObjectHeader( f_pbBuffer,
                                         f_cbBuffer,
                                         DRM_BCERT_OBJFLAG_CONTAINER_OBJ | DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                         DRM_BCERT_OBJTYPE_EXTDATACONTAINER,
                                         *f_pcbSize ) );
            iBuffer += SIZEOF_OBJ_HEADER;

            pbSignedData = f_pbBuffer + iBuffer; /* remember where signed data begins */

            /*
            ** Ext data record object header - HWID only
            */
            ChkDR( _addCertObjectHeader( f_pbBuffer + iBuffer,
                                         f_cbBuffer - iBuffer,
                                         f_pExtData->pExtDataRecords[0].Header.wFlags,
                                         f_pExtData->pExtDataRecords[0].Header.wType,
                                         dwRecordSize ) );
            iBuffer += SIZEOF_OBJ_HEADER;

            /*
            ** Data length
            */
            DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pExtData->pExtDataRecords[0].dwDataLength );
            iBuffer += SIZEOF( DRM_DWORD );

            /*
            ** Data Blob, padded
            */
            ChkDR( _addAlignedData( f_pbBuffer,
                                    &iBuffer,
                                    f_pExtData->pExtDataRecords[0].pbData,
                                    f_pExtData->pExtDataRecords[0].dwDataLength ) );

            if ( f_pExtData->wExtDataSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256 )
            {
                /*
                ** Signature object header
                */
                ChkDR( _addCertObjectHeader( f_pbBuffer + iBuffer,
                                             f_cbBuffer - iBuffer,
                                             DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                             DRM_BCERT_OBJTYPE_EXTDATASIGNATURE,
                                             dwSignatureSize ) );
                iBuffer += SIZEOF_OBJ_HEADER;

                /*
                ** Signature type
                */
                WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pExtData->wExtDataSignatureType );
                iBuffer += SIZEOF( DRM_WORD );

                /*
                ** Signature length
                */
                WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, SIZEOF( SIGNATURE_P256 ) );
                iBuffer += SIZEOF( DRM_WORD );

                /*
                ** Signature blob
                */
                DRM_BYT_CopyBytes( oPrivKey.m_rgbPrivkey,
                                   0,
                                   f_pExtData->oPrivKey.rgb,
                                   0,
                                   SIZEOF( PRIVKEY_P256 ) );

                dwSignedDataLength = SIZEOF_OBJ_HEADER                                   /* ext data record header */
                            + SIZEOF( DRM_DWORD )                                        /* record length */
                            + f_pExtData->pExtDataRecords[0].dwDataLength                /* record data */
                            + PAD_AMOUNT( f_pExtData->pExtDataRecords[0].dwDataLength ); /* padding */

                ChkDR( OEM_ECDSA_Sign_P256( pbSignedData,
                                            dwSignedDataLength,
                                            &oPrivKey,
                                            &Signature,
                                            (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ) );

                ChkDR( _addAlignedData(f_pbBuffer, &iBuffer, Signature.m_rgbSignature, SIZEOF( SIGNATURE_P256 ) ) );
            }
            else
            {
                ChkDR( DRM_E_BCERT_INVALID_SIGNATURE_TYPE );
            }

             ChkBOOL( iBuffer == *f_pcbSize, DRM_E_BCERT_INVALID_EXTDATA_LENGTH );
        }
        else
        {
            /*
            ** leave empty space of f_pcbSize length
            */
            DRM_BYT_SetBytes(f_pbBuffer, 0, *f_pcbSize, 'a');
        }
    }
    else
    {
        *f_pcbSize = 0;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    _addCertSignatureInfo
**
** Synopsis:    Adds the Signature Information object to a certificate.
**
** Arguments:   [f_pbBuffer]        : The address at which to start writing these data
**              [f_cbBuffer]        : Size of f_pbBuffer in bytes
**              [f_pData]           : A pointer to all the input data
**              [f_cbAmountToSign]  : The portion of this certificate to be signed (in bytes)
**              [f_cbSize]          : The previously calculated size of this object (in bytes)
**              [f_pBbxContext]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**              [f_pCryptoCtx]      : Pointer to a non-Blackbox crypto context.
**              [f_pOEMContext]     : A pointer to the OEM Context; may be NULL
**
** Returns:     DRM_SUCCESS                           - on success
**              DRM_E_INVALIDARG                      - if any parameter is invalid
**              DRM_E_BCERT_PRIVATE_KEY_NOT_SPECIFIED - if the private key is NULL
**              DRM_E_BCERT_ISSUER_KEY_NOT_SPECIFIED  - if the issuer key is NULL
**              DRM_E_BCERT_INVALID_KEY_LENGTH        - if the issuer key length is unsupported
**              DRM_E_BCERT_INVALID_SIGNATURE_TYPE    - if the signature type is unsupported
**              or other return code from a function called within
**
** Notes:       One and only one of f_pBbxContext or f_pCryptoCtx may be supplied, depending
**              upon whether f_pData contains a key handle or a key buffer. If f_pData
**              contains a key handle, then the blackbox owning the handle must be supplied.
**              If a key material buffer is supplied then either a blackbox or a crypto context
**              may be supplied.
**
******************************************************************************/
static DRM_RESULT _addCertSignatureInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                *f_pbBuffer,
    __in                       const DRM_DWORD                f_cbBuffer,
    __in                             DRM_BCERT_BUILDER_DATA  *f_pData,
    __in                       const DRM_DWORD                f_cbAmountToSign,
    __in                       const DRM_DWORD                f_cbSize,
    __inout_opt                      DRM_BB_CONTEXT          *f_pBbxContext,
    __inout_opt                      DRM_CRYPTO_CONTEXT      *f_pCryptoCtx,
    __in_opt                         DRM_VOID                *f_pOEMContext )
{
    DRM_RESULT          dr               = DRM_SUCCESS;
    DRM_DWORD           iBuffer          = SIZEOF_OBJ_HEADER;
    SIGNATURE_P256      Signature        = {0};
    DRM_DWORD           dwSigLength      = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;
    DRM_WORD            wSignatureLength = 0;
    DRM_BOOL            fUsePrivKey      = TRUE;
    DRM_CRYPTO_CONTEXT *pCryptoCtxToUse  = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertSignatureInfo );

    ChkArg( f_pbBuffer       != NULL );
    ChkArg( f_pData          != NULL );
    ChkArg( f_cbAmountToSign  > 0    );
    ChkArg( f_cbSize          > 0    );
    ChkArg( ( f_pBbxContext == NULL ) != ( f_pCryptoCtx == NULL ) );


    /*
    ** Check valid combination of keys
    */
    ChkArg( ( f_pData->hPrivateKey == OEM_CRYPTO_HANDLE_INVALID ) != ( f_pData->pPrivateKey == NULL ) );
    ChkBOOL( f_pData->dwIssuerKeyLength != 0, DRM_E_BCERT_ISSUER_KEY_NOT_SPECIFIED  );

    /*
    ** Check supported key lengths
    */
    ChkBOOL( f_pData->dwIssuerKeyLength == ECC_P256_PUBKEY_SIZE_IN_BITS, DRM_E_BCERT_INVALID_KEY_LENGTH );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_SIGNATURE,
                                 f_cbSize ) );

    if ( f_pData->wSignatureType == DRM_BCERT_SIGNATURE_TYPE_P256 )
    {
#if !DRM_SUPPORT_NONVAULTSIGNING
        if( f_pData->hPrivateKey == OEM_CRYPTO_HANDLE_INVALID )
        {
            PRIVKEY_P256 oZeroPrivKey = { 0 };

            /*
            ** If this is a key buffer signing and
            ** non-vault signing is not supported:
            ** The only caller that can pass a non-all-0 key in
            ** f_pData->pPrivateKey is the test code. In production
            ** only all-0 key will be in f_pData->pPrivateKey
            ** indicating that the vault signing will be used.
            */
            OEM_ECC_ZeroPrivateKey_P256( &oZeroPrivKey );

            fUsePrivKey = ( MEMCMP( f_pData->pPrivateKey,
                                    &oZeroPrivKey,
                                    SIZEOF( PRIVKEY_P256 ) ) != 0 );
        }
#endif /* !DRM_SUPPORT_NONVAULTSIGNING */

        /*
        ** Add the Signature Type
        */
        WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->wSignatureType );
        iBuffer += SIZEOF( DRM_WORD );

        /*
        ** Sign the data in this certificate. If a key handle has been supplied, use the 
        ** secure blackbox function; otherwise use the non-secure local signing function
        */
        if( f_pData->hPrivateKey != OEM_CRYPTO_HANDLE_INVALID )
        {
            ChkArg( f_pBbxContext != NULL );

            ChkDR( DRM_BBX_SignWithCertKeys( f_pBbxContext,
                                             DRM_DKT_PLAYREADY_MODEL,
                                             f_pData->hPrivateKey,
                                             f_cbAmountToSign,
                                             f_pbBuffer - f_cbAmountToSign,
                                             &dwSigLength,
                                             (DRM_BYTE*)&Signature ) );
        }
        else
        {
            if( f_pBbxContext != NULL )
            {
                pCryptoCtxToUse = &f_pBbxContext->CryptoContext;
            }
            else
            {
                pCryptoCtxToUse = f_pCryptoCtx;
            }

            ChkDR( _NonSecureSignWithCertKeys( f_pOEMContext,
                                               pCryptoCtxToUse,
                                               fUsePrivKey ? f_pData->pPrivateKey : NULL,
                                               f_cbAmountToSign,
                                               f_pbBuffer - f_cbAmountToSign,
                                               &dwSigLength,
                                               (DRM_BYTE*)&Signature ) );
        }

        wSignatureLength = (DRM_WORD) dwSigLength;
    }
    else
    {
        ChkDR( DRM_E_BCERT_INVALID_SIGNATURE_TYPE );
    }

    /*
    ** Add the signature length and signature data
    */
    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, wSignatureLength );
    iBuffer += SIZEOF( DRM_WORD );

    ChkDR( _addAlignedData(f_pbBuffer, &iBuffer, Signature.m_rgbSignature, wSignatureLength ) );

    /*
    ** Add the Issuer Key Length (in bits)
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pData->dwIssuerKeyLength );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Issuer Key (should be the public key from the next certificate in this chain)
    */
    ChkDR( _addAlignedData( f_pbBuffer,
                             &iBuffer,
                             f_pData->IssuerKey.m_rgbPubkey,
                             ( f_pData->dwIssuerKeyLength / CHAR_BIT ) ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}




/*****************************************************************************
** Function:    _addCertDomainInfo
**
** Synopsis:    Adds the optional Domain Information object to a certificate.
**
** Arguments:   [f_pbBuffer]    : The address at which to start writing these data
**              [f_cbBuffer]    : Size of f_pbBuffer in bytes
**              [f_pDomainData] : The domain data
**              [f_pcbSize]     : Variable pointer to update w/ size of this object (in bytes)
**
** Returns:     DRM_SUCCESS                            - on success
**              DRM_E_INVALIDARG                       - if any parameter is invalid
**              DRM_E_BCERT_ACCOUNT_ID_NOT_SPECIFIED   - if pointer to account ID is NULL
**              DRM_E_BCERT_SERVICE_ID_NOT_SPECIFIED   - if pointer to service ID is NULL
**              DRM_E_BCERT_DOMAIN_URL_NOT_SPECIFIED   - if pointer to domain URL is NULL, or 0 length
**              DRM_E_BCERT_STRING_NOT_NULL_TERMINATED - if the domain URL is not null terminated
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertDomainInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                *f_pbBuffer,
    __in                       const DRM_DWORD                f_cbBuffer,
    __in                       const DRM_BCERT_DOMAIN_DATA   *f_pDomainData,
    __inout                          DRM_DWORD               *f_pcbSize )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iBuffer  = SIZEOF_OBJ_HEADER;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertDomainInfo );

    ChkArg( f_pbBuffer    != NULL );
    ChkArg( f_pcbSize     != NULL );
    ChkArg( f_pDomainData != NULL );

    ChkBOOL( f_pDomainData->pAccountID != NULL, DRM_E_BCERT_ACCOUNT_ID_NOT_SPECIFIED );
    ChkBOOL( f_pDomainData->pServiceID != NULL, DRM_E_BCERT_SERVICE_ID_NOT_SPECIFIED );

    ChkBOOL( f_pDomainData->pDomainURL     != NULL, DRM_E_BCERT_DOMAIN_URL_NOT_SPECIFIED );
    ChkBOOL( f_pDomainData->pDomainURL->cb >  0,    DRM_E_BCERT_DOMAIN_URL_NOT_SPECIFIED );

    /*
    ** Ensure that: 1) the URL string is NULL terminated and 2) the count includes the NULL
    */
    ChkBOOL( f_pDomainData->pDomainURL->rgb[ f_pDomainData->pDomainURL->cb - 1 ] == '\0', DRM_E_BCERT_STRING_NOT_NULL_TERMINATED );

    ChkDR( _calcDomainInfoSize( f_pDomainData->pDomainURL->cb, f_pcbSize ) );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_DOMAIN,
                                 *f_pcbSize ) );

    /*
    ** Add the Service ID, Domain ID, and Revision
    */
    DRM_BYT_CopyBytes( f_pbBuffer, iBuffer, f_pDomainData->pServiceID->rgb, 0, DRM_BCERT_SERVICE_ID_LENGTH );
    iBuffer += DRM_BCERT_SERVICE_ID_LENGTH;

    DRM_BYT_CopyBytes( f_pbBuffer, iBuffer, f_pDomainData->pAccountID->rgb, 0, DRM_BCERT_ACCOUNT_ID_LENGTH );
    iBuffer += DRM_BCERT_ACCOUNT_ID_LENGTH;

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pDomainData->dwRevision );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Domain URL Length, then the Domain URL
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pDomainData->pDomainURL->cb );
    iBuffer += SIZEOF( DRM_DWORD );

    ChkDR( _addAlignedData(f_pbBuffer, &iBuffer, f_pDomainData->pDomainURL->rgb, f_pDomainData->pDomainURL->cb ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertPCInfo
**
** Synopsis:    Adds the optional PC Information object to a certificate.
**
** Arguments:   [f_pbBuffer] : The address at which to start writing these data
**              [f_cbBuffer] : Size of f_pbBuffer in bytes
**              [f_PCData]   : The PC data
**
** Returns:     DRM_SUCCESS                           - on success
**              DRM_E_INVALIDARG                      - if any parameter is invalid
**              DRM_E_BCERT_INVALID_SECURITY_VERSION  - if the security version is zero
**              or other return code from a function called within
**
** Notes:       This object is a fixed length in size.
**
******************************************************************************/
static DRM_RESULT _addCertPCInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                *f_pbBuffer,
    __in                       const DRM_DWORD                f_cbBuffer,
    __in                             DRM_BCERT_PC_DATA        f_PCData )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = SIZEOF_OBJ_HEADER;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertPCInfo );

    ChkArg( f_pbBuffer != NULL );

    ChkBOOL( f_PCData.dwSecurityVersion != 0, DRM_E_BCERT_INVALID_SECURITY_VERSION );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_PC,
                                 SIZEOF_PC_INFO ) );

    /*
    ** Add the Security Version
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_PCData.dwSecurityVersion );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    _addCertSilverLightInfo
**
** Synopsis:    Adds the optional SilverLight Information object to a certificate.
**
** Arguments:   [f_pbBuffer]        : The address at which to start writing these data
**              [f_cbBuffer]        : Size of f_pbBuffer in bytes
**              [f_SilverLightData] : The SilverLight data
**
** Returns:     DRM_SUCCESS                           - on success
**              DRM_E_INVALIDARG                      - if any parameter is invalid
**              DRM_E_BCERT_INVALID_SECURITY_VERSION  - if the security version is zero
**              or other return code from a function called within
**
** Notes:       This object is a fixed length in size.
**
******************************************************************************/
static DRM_RESULT _addCertSilverLightInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                   *f_pbBuffer,
    __in                       const DRM_DWORD                   f_cbBuffer,
    __in                             DRM_BCERT_SILVERLIGHT_DATA  f_SilverLightData )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = SIZEOF_OBJ_HEADER;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertSilverLightInfo );

    ChkArg( f_pbBuffer != NULL );

    ChkBOOL( f_SilverLightData.dwSecurityVersion != 0, DRM_E_BCERT_INVALID_SECURITY_VERSION );
    ChkBOOL( f_SilverLightData.dwPlatformIdentifier <= DRM_BCERT_SILVERLIGHT_PLATFORM_MAX, DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER);

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_SILVERLIGHT,
                                 SIZEOF_SILVERLIGHT_INFO ) );

    /*
    ** Add the Security Version
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_SilverLightData.dwSecurityVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Platform Identifier
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_SilverLightData.dwPlatformIdentifier );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    _addCertDeviceInfo
**
** Synopsis:    Adds the optional Device Information object to a certificate.
**
** Arguments:   [f_pbBuffer]    : The address at which to start writing these data
**              [f_cbBuffer]    : Size of f_pbBuffer in bytes
**              [f_pDeviceData] : The Device data
**
** Returns:     DRM_SUCCESS                                 - on success
**              DRM_E_INVALIDARG                            - if any parameter is invalid
**              DRM_E_BCERT_INVALID_MAX_LICENSE_SIZE        - if the max license size is unsupported
**              DRM_E_BCERT_INVALID_MAX_HEADER_SIZE         - if the max header size is unsupported
**              DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH - if the max license chain depth is unsupported
**              or other return code from a function called within
**
** Notes:       This object is a fixed length in size.
**
******************************************************************************/
static DRM_RESULT _addCertDeviceInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE              *f_pbBuffer,
    __in                       const DRM_DWORD              f_cbBuffer,
    __in                       const DRM_BCERT_DEVICE_DATA *f_pDeviceData )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iBuffer  = SIZEOF_OBJ_HEADER;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertDeviceInfo );

    ChkArg( f_pbBuffer    != NULL );
    ChkArg( f_pDeviceData != NULL );

    /*
    ** Check the supported maximum sizes
    */
    ChkBOOL( f_pDeviceData->cbMaxLicense    == DRM_BCERT_MAX_LICENSE_SIZE,        DRM_E_BCERT_INVALID_MAX_LICENSE_SIZE );
    ChkBOOL( f_pDeviceData->cbMaxHeader     == DRM_BCERT_MAX_HEADER_SIZE,         DRM_E_BCERT_INVALID_MAX_HEADER_SIZE );
    ChkBOOL( f_pDeviceData->dwMaxChainDepth == DRM_BCERT_MAX_LICENSE_CHAIN_DEPTH, DRM_E_BCERT_INVALID_MAX_LICENSE_CHAIN_DEPTH );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_DEVICE,
                                 SIZEOF_DEVICE_INFO ) );

    /*
    ** Add the max license size, max header size, and max license chain depth
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pDeviceData->cbMaxLicense );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pDeviceData->cbMaxHeader );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pDeviceData->dwMaxChainDepth );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _addCertMeteringInfo
**
** Synopsis:    Adds the optional Metering Information object to a certificate.
**
** Arguments:   [f_pbBuffer]        : The address at which to start writing these data
**              [f_cbBuffer]        : Size of f_pbBuffer in bytes
**              [f_MeteringData]    : The metering data
**              [f_pcbSize]         : Variable pointer to update w/ size of this object (in bytes)
**
** Returns:     DRM_SUCCESS                            - on success
**              DRM_E_INVALIDARG                       - if any parameter is invalid
**              DRM_E_BCERT_METERING_ID_NOT_SPECIFIED  - if pointer to metering ID is NULL
**              DRM_E_BCERT_METERING_URL_NOT_SPECIFIED - if pointer to metering URL is NULL, or 0 length
**              DRM_E_BCERT_STRING_NOT_NULL_TERMINATED - if the metering URL is not null terminated
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _addCertMeteringInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                *f_pbBuffer,
    __in                       const DRM_DWORD                f_cbBuffer,
    __in                             DRM_BCERT_METERING_DATA  f_MeteringData,
    __inout                          DRM_DWORD               *f_pcbSize )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iBuffer  = SIZEOF_OBJ_HEADER;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertMeteringInfo );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pcbSize  != NULL );

    ChkBOOL( f_MeteringData.pMeteringID != NULL, DRM_E_BCERT_METERING_ID_NOT_SPECIFIED );

    ChkBOOL( f_MeteringData.pMeteringURL     != NULL, DRM_E_BCERT_METERING_URL_NOT_SPECIFIED );
    ChkBOOL( f_MeteringData.pMeteringURL->cb >  0,    DRM_E_BCERT_METERING_URL_NOT_SPECIFIED );

    /*
    ** Ensure that: 1) the URL string is NULL terminated and 2) the count includes the NULL
    */
    ChkBOOL( f_MeteringData.pMeteringURL->rgb[ f_MeteringData.pMeteringURL->cb - 1 ] == '\0', DRM_E_BCERT_STRING_NOT_NULL_TERMINATED );

    ChkDR( _calcMeteringInfoSize( f_MeteringData.pMeteringURL->cb, f_pcbSize ) );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_METERING,
                                 *f_pcbSize ) );

    /*
    ** Add the Metering ID
    */
    DRM_BYT_CopyBytes( f_pbBuffer, iBuffer, f_MeteringData.pMeteringID->rgb, 0, DRM_BCERT_METERING_ID_LENGTH );
    iBuffer += DRM_BCERT_METERING_ID_LENGTH;


    /*
    ** Add the Metering URL Length, then the Metering URL
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_MeteringData.pMeteringURL->cb );
    iBuffer += SIZEOF( DRM_DWORD );

    ChkDR( _addAlignedData(f_pbBuffer, &iBuffer, f_MeteringData.pMeteringURL->rgb, f_MeteringData.pMeteringURL->cb ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*****************************************************************************
** Function:    _addCertServerInfo
**
** Synopsis:    Adds the optional Server Information object to a certificate.
**
** Arguments:   [f_pbBuffer]    : The address at which to start writing these data
**              [f_cbBuffer]    : Size of f_pbBuffer in bytes
**              [f_ServerData]  : The Server data
**
** Returns:     DRM_SUCCESS                           - on success
**              DRM_E_INVALIDARG                      - if any parameter is invalid
**              DRM_E_BCERT_INVALID_SECURITY_VERSION  - if the security version is zero
**              or other return code from a function called within
**
** Notes:       This object is a fixed length in size.
**
******************************************************************************/
static DRM_RESULT _addCertServerInfo(
    __inout_ecount(f_cbBuffer)       DRM_BYTE              *f_pbBuffer,
    __in                       const DRM_DWORD              f_cbBuffer,
    __in                             DRM_BCERT_SERVER_DATA  f_ServerData )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = SIZEOF_OBJ_HEADER;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertServerInfo );

    ChkArg( f_pbBuffer != NULL );

    ChkBOOL( f_ServerData.dwWarningDays != 0, DRM_E_BCERT_INVALID_WARNING_DAYS );

    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_MUST_UNDERSTAND,
                                 DRM_BCERT_OBJTYPE_SERVER,
                                 SIZEOF_SERVER_INFO ) );

    /*
    ** Add the Security Version
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_ServerData.dwWarningDays );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    _addCertSecurityVersion
**
** Synopsis:    Adds the optional Security Version object to a certificate.
**
** Arguments:   [f_pbBuffer]                : The address at which to start writing these data
**              [f_cbBuffer]                : Size of f_pbBuffer in bytes
**              [f_pSecurityVersionData]    : The SecurityVersion data
**              [f_HeaderType]              : The type of the security version object to add.
**
** Returns:     DRM_SUCCESS                             - on success
**              DRM_E_INVALIDARG                        - if any parameter is invalid
**              DRM_E_BCERT_INVALID_SECURITY_VERSION    - if the security version is zero
**              DRM_E_BCERT_INVALID_PLATFORM_IDENTIFIER - if the Platform ID is out of bounds of the acceptable IDs
**              or other return code from a function called within
**
** Notes:       This object is a fixed length in size.
**
******************************************************************************/
static DRM_RESULT _addCertSecurityVersion(
    __inout_ecount(f_cbBuffer)       DRM_BYTE                        *f_pbBuffer,
    __in                       const DRM_DWORD                        f_cbBuffer,
    __in                       const DRM_BCERT_SECURITY_VERSION_DATA *f_pSecurityVersionData,
    __in                             DRM_WORD                         f_HeaderType )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = SIZEOF_OBJ_HEADER;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_addCertSecurityVersion );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pSecurityVersionData != NULL );
    ChkArg( f_pSecurityVersionData->fValid );
    ChkArg(f_HeaderType == DRM_BCERT_OBJTYPE_SECURITY_VERSION || f_HeaderType == DRM_BCERT_OBJTYPE_SECURITY_VERSION_2);

    ChkBOOL( f_pSecurityVersionData->dwSecurityVersion != 0, DRM_E_BCERT_INVALID_SECURITY_VERSION );


    /*
    ** Add the Object Header
    */
    ChkDR( _addCertObjectHeader( f_pbBuffer,
                                 f_cbBuffer,
                                 DRM_BCERT_OBJFLAG_EMPTY,
                                 f_HeaderType,
                                 SIZEOF_SECURITY_VERSION ) );

    /*
    ** Add the Security Version
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pSecurityVersionData->dwSecurityVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    /*
    ** Add the Platform Identifier
    */
    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, f_pSecurityVersionData->dwPlatformIdentifier );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
** Function:    BCert_SetDefaultBuilderData
**
** Synopsis:    Fills in all the data that are common to all certificate types with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pCertificateID]      : Pointer to the certificate ID
**              [f_dwType]              : The certificate type
**              [f_pPrivateKey]         : Pointer to the private key to use for signing
**              [f_hPrivateKey]         : Handle to the private key to use for signing
**              [f_pIssuerKey]          : Pointer to the issuer key
**              [f_pData]               : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       f_pPrivateKey or f_hPrivateKey may be specified, but not both.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetDefaultBuilderData(
   __in         DRM_BCERT_CERT_ID         *f_pCertificateID,
   __in  const  DRM_DWORD                 f_dwType,
   __in         PRIVKEY_P256              *f_pPrivateKey,
   __in         OEM_CRYPTO_HANDLE         f_hPrivateKey,
   __in  const  PUBKEY_P256               *f_pIssuerKey,
   __out        DRM_BCERT_BUILDER_DATA    *f_pData )
{
    f_pData->dwChainHeaderFlags   = DRM_BCERT_OBJFLAG_EMPTY;
    f_pData->pCertificateID       = f_pCertificateID;
    f_pData->dwSecurityLevel      = DRM_BCERT_SECURITYLEVEL_2000;
    f_pData->dwBasicFlags         = DRM_BCERT_FLAGS_EMPTY;
    f_pData->dwType               = f_dwType;
    f_pData->dwExpirationDate     = DRM_BCERT_DEFAULT_EXPIRATION_DATE;

    DRMCASSERT( SIZEOF( f_pData->ClientID.rgb ) == DRM_BCERT_CLIENT_ID_LENGTH ); 
    MEMSET( f_pData->ClientID.rgb, 0, SIZEOF( f_pData->ClientID.rgb ) );

    f_pData->dwNumFeatureEntries  = 0;
    f_pData->prgdwFeatureSet      = NULL;

    f_pData->dwManufacturerFlags  = DRM_BCERT_OBJFLAG_EMPTY;
    f_pData->pManufacturerStrings = NULL;
    f_pData->hPrivateKey          = f_hPrivateKey;
    f_pData->pPrivateKey          = f_pPrivateKey;
    f_pData->dwIssuerKeyLength    = ECC_P256_PUBKEY_SIZE_IN_BITS;

    MEMCPY( f_pData->IssuerKey.m_rgbPubkey, f_pIssuerKey->m_rgbPubkey, SIZEOF( PUBKEY_P256 ) );

    f_pData->wSignatureType       = DRM_BCERT_SIGNATURE_TYPE_P256;

    f_pData->ExtendedData.fValid                  = FALSE;
    f_pData->ExtendedData.wExtDataKeyType         = DRM_BCERT_KEYTYPE_ECC256;
    f_pData->ExtendedData.wExtDataKeyLength       = ECC_P256_PUBKEY_SIZE_IN_BITS;
    f_pData->ExtendedData.dwExtDataKeyFlags       = DRM_BCERT_OBJFLAG_EMPTY;
    f_pData->ExtendedData.dwNumExtDataRecords     = 0;
    f_pData->ExtendedData.pExtDataRecords         = NULL;
    f_pData->ExtendedData.wExtDataSignatureType   = DRM_BCERT_SIGNATURE_TYPE_P256;
}



/*****************************************************************************
** Function:    BCert_SetKeyInfoBuilderData
**
** Synopsis:    Fills in all the data specific to the key info object with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pKeys]     : Pointer to the array of certificate keys
**              [f_dwNumKeys] : Number of elements in f_pKeys array.
**              [f_pData]     : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetKeyInfoBuilderData(
    __in_ecount(f_dwNumKeys) DRM_BCERT_BUILDER_CERTKEY  *f_pKeys,
    __in                     const DRM_DWORD             f_dwNumKeys,
    __out                    DRM_BCERT_BUILDER_DATA     *f_pData )
{
    DRM_DWORD iCount = 0;
    if ( f_pKeys != NULL )
    {
        for (; iCount < f_dwNumKeys; iCount++ )
        {
            f_pKeys[iCount].wKeyType = DRM_BCERT_KEYTYPE_ECC256;
            f_pKeys[iCount].wKeyLength = ECC_P256_PUBKEY_SIZE_IN_BITS;
            f_pKeys[iCount].dwKeyFlags = 0;
        }

        f_pData->dwNumCertificateKeys = f_dwNumKeys;
        f_pData->pCertificateKeys = f_pKeys;
    }
}


/*****************************************************************************
** Function:    BCert_SetDomainBuilderData
**
** Synopsis:    Fills in all the data specific to the Domain certificate type with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pServiceID] : Pointer to the service ID
**              [f_pAccountID] : Pointer to the account ID
**              [f_dwRevision] : Timestamp marking the generation of the domain key pair
**              [f_pClientID]  : Pointer to the client ID
**              [f_pDomainURL] : Pointer to the domain URL
**              [f_pData]      : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetDomainBuilderData(
   __in         DRM_BCERT_SERVICE_ID    *f_pServiceID,
   __in         DRM_BCERT_ACCOUNT_ID    *f_pAccountID,
   __in  const  DRM_DWORD                f_dwRevision,
   __in  const  DRM_BCERT_CLIENT_ID     *f_pClientID,
   __in         DRM_BCERT_DOMAIN_URL    *f_pDomainURL,
   __out        DRM_BCERT_BUILDER_DATA  *f_pData )
{
    f_pData->Type.DomainData.pServiceID = f_pServiceID;
    f_pData->Type.DomainData.pAccountID = f_pAccountID;
    f_pData->Type.DomainData.dwRevision = f_dwRevision;
    f_pData->Type.DomainData.pDomainURL = f_pDomainURL;

    MEMCPY( f_pData->ClientID.rgb, f_pClientID->rgb, DRM_BCERT_CLIENT_ID_LENGTH );
}



/*****************************************************************************
** Function:    BCert_SetPCBuilderData
**
** Synopsis:    Fills in all the data specific to the PC certificate type with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pHardwareID]       : Pointer to the hardware ID
**              [f_dwSecurityVersion] : The security version
**              [f_pPrivateKey]       : Pointer to a private key used to sign extended data
**              [f_pPublicKey]        : Pointer to a corresponding public key to verify signature
**                                      over extended data
**              [f_oRecord]           : Pointer to extended data structure, may be NULL
**              [f_pData            ] : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetPCBuilderData(
   __in    const  DRM_BCERT_CLIENT_ID      *f_pHardwareID,
   __in    const  DRM_DWORD                 f_dwSecurityVersion,  /* A.B.C.D */
   __in    const  PRIVKEY_P256             *f_pPrivateKey,
   __in    const  PUBKEY_P256              *f_pPublicKey,
   __in_opt       DRM_BCERT_EXTDATA_RECORD *f_oRecord,
   __out          DRM_BCERT_BUILDER_DATA   *f_pData )
{
    f_pData->Type.PCData.dwSecurityVersion = f_dwSecurityVersion;

    MEMCPY( f_pData->ClientID.rgb, f_pHardwareID->rgb, DRM_BCERT_CLIENT_ID_LENGTH );

    f_pData->ExtendedData.fValid = TRUE;
    f_pData->dwBasicFlags = DRM_BCERT_FLAGS_EXTDATA_PRESENT;

    MEMCPY( f_pData->ExtendedData.oPubKey.m_rgbPubkey, f_pPublicKey->m_rgbPubkey, SIZEOF( PUBKEY_P256 ) );

    MEMCPY( f_pData->ExtendedData.oPrivKey.rgb, f_pPrivateKey->m_rgbPrivkey, SIZEOF( PRIVKEY_P256 ) );

    f_pData->ExtendedData.dwNumExtDataRecords = 1;

    f_pData->ExtendedData.pExtDataRecords = f_oRecord;
}


/*****************************************************************************
** Function:    BCert_SetSilverLightBuilderData
**
** Synopsis:    Fills in all the data specific to the SilverLight certificate type with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pHardwareID]       : Pointer to the hardware ID
**              [f_dwSecurityVersion] : The security version
**              [f_pData            ] : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetSilverLightBuilderData(
   __in    const  DRM_BCERT_CLIENT_ID     *f_pHardwareID,
   __in    const  DRM_DWORD                f_dwSecurityVersion,  /* A.B.C.D */
   __in    const  DRM_DWORD                f_dwPlatformIdentifier,
   __out          DRM_BCERT_BUILDER_DATA  *f_pData )
{
    f_pData->Type.SilverLightData.dwSecurityVersion    = f_dwSecurityVersion;
    f_pData->Type.SilverLightData.dwPlatformIdentifier = f_dwPlatformIdentifier;

    MEMCPY( f_pData->ClientID.rgb, f_pHardwareID->rgb, DRM_BCERT_CLIENT_ID_LENGTH );
}


/*****************************************************************************
** Function:    BCert_SetDeviceBuilderData
**
** Synopsis:    Fills in all the data specific to the Device certificate type with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pSerialNum] : Pointer to the device's serial number
**              [f_pData]      : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetDeviceBuilderData(
   __in     const DRM_BCERT_CLIENT_ID     *f_pSerialNum,
   __out          DRM_BCERT_BUILDER_DATA  *f_pData )
{
    f_pData->Type.DeviceData.cbMaxLicense    = DRM_BCERT_MAX_LICENSE_SIZE;
    f_pData->Type.DeviceData.cbMaxHeader     = DRM_BCERT_MAX_HEADER_SIZE;
    f_pData->Type.DeviceData.dwMaxChainDepth = DRM_BCERT_MAX_LICENSE_CHAIN_DEPTH;

    MEMCPY( f_pData->ClientID.rgb, f_pSerialNum->rgb, DRM_BCERT_CLIENT_ID_LENGTH );
}

/*****************************************************************************
** Function:    BCert_SetMeteringBuilderData
**
** Synopsis:    Fills in all the data specific to the Metering certificate type with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_pMeteringID] : Pointer to the metering ID
**              [f_pMeteringURL] : Pointer to the metering URL
**              [f_pData]      : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetMeteringBuilderData(
   __in         DRM_BCERT_METERING_ID    *f_pMeteringID,
   __in         DRM_BCERT_METERING_URL   *f_pMeteringURL,
   __out        DRM_BCERT_BUILDER_DATA   *f_pData )
{
    f_pData->Type.MeteringData.pMeteringID = f_pMeteringID;
    f_pData->Type.MeteringData.pMeteringURL = f_pMeteringURL;
}


/*****************************************************************************
** Function:    BCert_SetServerBuilderData
**
** Synopsis:    Fills in all the data specific to the Server certificate type with default values.
**              Those data that do not have default values are parameters to this function.
**              It is completely optional to call this function, it is provided as a convenience.
**              As such, it does not duplicate any input data validity checking; input data
**              validity checks are in the code that builds the certificate chain.
**
** Arguments:   [f_dwWarningDays] : When to start Server cert expiration warning (in seconds since Jan 1, 1970, UTC)
**              [f_pData             ] : Pointer to the builder data stucture to fill in
**
** Returns:     void
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL BCert_SetServerBuilderData(
   __in    const  DRM_DWORD                 f_dwWarningDays,  /* in seconds since Jan 1, 1970, UTC */
   __out          DRM_BCERT_BUILDER_DATA   *f_pData )
{
    f_pData->Type.ServerData.dwWarningDays = f_dwWarningDays;
}


/*****************************************************************************
** Function:    BCert_AddCert
**
** Synopsis:    Adds a certificate to a certificate chain.
**              If this function is called with a NULL new chain buffer pointer,
**              then the buffer size parm is updated with the required size (in bytes).
**
** Arguments: [f_pbParentChain]   : A buffer pointer to an existing certificate chain; may be NULL
**              [f_pbNewChain]    : A buffer pointer for the new certificate chain; may be NULL
**              [f_pcbNewChain]   : A pointer to new chain buffer size (in bytes); may be updated
**              [f_pData]         : A pointer to all the input data
**              [f_pBbxContext]   : Pointer to Blackbox context.
**              [f_pCryptoCtx]    : Pointer to a non-Blackbox crypto context.
**              [f_pOEMContext]   : A pointer to the OEM Context; may be NULL
**
** Returns:     DRM_SUCCESS                - on success
**              DRM_E_INVALIDARG           - if any parameter is invalid
**              DRM_E_BCERT_CHAIN_TOO_DEEP - if the existing chain is already at maximum capacity
**              or other return code from a function called within
**
** Notes:       One and only one of f_pBbxContext or f_pCryptoCtx may be supplied, depending
**              upon whether f_pData contains a key handle or a key buffer. If f_pData
**              contains a key handle, then the blackbox owning the handle must be supplied.
**              If a key material buffer is supplied then either a blackbox or a crypto context
**              may be supplied.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL BCert_AddCert(
   __in_opt                         const DRM_BYTE                *f_pbParentChain,
   __out_ecount_opt(*f_pcbNewChain)       DRM_BYTE                *f_pbNewChain,
   __inout                                DRM_DWORD               *f_pcbNewChain,
   __in                                   DRM_BCERT_BUILDER_DATA  *f_pData,
   __inout                                DRM_BB_CONTEXT          *f_pBbxContext,
   __inout                                DRM_CRYPTO_CONTEXT      *f_pCryptoCtx,
   __in_opt                               DRM_VOID                *f_pOEMContext )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_DWORD           iOffset         = 0;
    DRM_DWORD           cbCert          = 0;
    DRM_DWORD           cbElement       = 0;
    DRM_DWORD           cbSignatureInfo = 0;
    DRM_DWORD           cbExtendedData  = 0;
    DRM_CRYPTO_CONTEXT *pCryptoCtxToUse = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_BCert_AddCert );

    ChkArg( f_pcbNewChain != NULL );
    ChkArg( f_pData       != NULL );
    ChkArg( ( f_pBbxContext == NULL ) != ( f_pCryptoCtx == NULL ) );

    if( f_pBbxContext != NULL )
    {
        pCryptoCtxToUse = &f_pBbxContext->CryptoContext;
    }
    else
    {
        pCryptoCtxToUse = f_pCryptoCtx;
    }

    /*
    ** Ensure that we are not adding more than the maximum certificates per chain
    */
    if ( f_pbParentChain != NULL )
    {
        DRM_DWORD   cCerts = 0;
        NETWORKBYTES_TO_DWORD( cCerts, f_pbParentChain, DRM_BCERT_CHAIN_COUNT_OFFSET );
        ChkBOOL( cCerts < DRM_BCERT_MAX_CERTS_PER_CHAIN, DRM_E_BCERT_CHAIN_TOO_DEEP );
    }

    /*
    ** Calculate the exact size of this certificate and its signature info portion.
    ** This function also validates the certificate type.
    */
    ChkDR( _calcCertSize( f_pData, &cbCert, &cbSignatureInfo, &cbExtendedData ) );

    /*
    ** Ensure that enough memory for the new certificate chain has been allocated by the caller
    */
    ChkDR( _checkBuffer( f_pbParentChain, f_pbNewChain, f_pcbNewChain, cbCert ) );

    /*
    ** If this is the first certificate in the chain, add a chain header,
    ** otherwise, copy the existing chain header.
    */
    if ( f_pbParentChain == NULL )
    {
        ChkDR( _addCertChainHeader( f_pbNewChain, *f_pcbNewChain, f_pData->dwChainHeaderFlags ) );
    }
    else
    {
        DRM_BYT_CopyBytes( f_pbNewChain, 0, f_pbParentChain, 0, SIZEOF_CHAIN_HEADER );
    }
    iOffset += SIZEOF_CHAIN_HEADER;

    /*
    ** Add the Certificate Header
    */
    ChkDR( _addCertHeader( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, cbCert, cbCert - cbSignatureInfo - cbExtendedData ) );
    iOffset += SIZEOF( DRM_BCERT_HEADER );

    /*
    ** Add the Basic Information
    */
    ChkDR( _addCertBasicInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, f_pData ) );
    iOffset += SIZEOF_BASIC_INFO;

    switch ( f_pData->dwType )
    {
        case DRM_BCERT_CERTTYPE_PC:
        {
            ChkDR( _addCertPCInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, f_pData->Type.PCData ) );
            iOffset += SIZEOF_PC_INFO;
            break;
        }

        case DRM_BCERT_CERTTYPE_SILVERLIGHT:
        {
            ChkDR( _addCertSilverLightInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, f_pData->Type.SilverLightData ) );
            iOffset += SIZEOF_SILVERLIGHT_INFO;
            break;
        }

        case DRM_BCERT_CERTTYPE_DEVICE:
        {
            ChkDR( _addCertDeviceInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, &f_pData->Type.DeviceData ) );
            iOffset += SIZEOF_DEVICE_INFO;
            break;
        }

        case DRM_BCERT_CERTTYPE_DOMAIN:
        {
            ChkDR( _addCertDomainInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, &f_pData->Type.DomainData, &cbElement ) );
            iOffset += cbElement;
            break;
        }

        case DRM_BCERT_CERTTYPE_METERING:
        {
            ChkDR( _addCertMeteringInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, f_pData->Type.MeteringData, &cbElement ) );
            iOffset += cbElement;
            break;
        }

        case DRM_BCERT_CERTTYPE_SERVER:
        {
            ChkDR( _addCertServerInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, f_pData->Type.ServerData ) );
            iOffset += SIZEOF_SERVER_INFO;
            break;
        }

        default:
        {
            break;
        }
    }

    /*
    ** Add the Security Version Object if Requested
    */
    if ( f_pData->SecurityVersion.fValid )
    {
        if (_verifySecurityVersion1PlatformIdForLegacyObject(f_pData->SecurityVersion.dwPlatformIdentifier) == DRM_SUCCESS)
        {
            ChkDR( _addCertSecurityVersion( f_pbNewChain + iOffset,
                                            *f_pcbNewChain - iOffset,
                                            &f_pData->SecurityVersion,
                                            DRM_BCERT_OBJTYPE_SECURITY_VERSION ) );
            iOffset += SIZEOF_SECURITY_VERSION;
        }

        /*
        ** Add security version 2 object to PC, device, and issuer certs
        */
        if( f_pData->dwType == DRM_BCERT_CERTTYPE_PC
         || f_pData->dwType == DRM_BCERT_CERTTYPE_DEVICE
         || f_pData->dwType == DRM_BCERT_CERTTYPE_ISSUER )
        {
            ChkDR( _addCertSecurityVersion( f_pbNewChain + iOffset,
                                            *f_pcbNewChain - iOffset,
                                            &f_pData->SecurityVersion,
                                            DRM_BCERT_OBJTYPE_SECURITY_VERSION_2 ) );
            iOffset += SIZEOF_SECURITY_VERSION;
        }

    }

    /*
    ** Add the Feature Information
    */
    ChkDR( _addCertFeatureInfo( f_pbNewChain + iOffset,
                                *f_pcbNewChain - iOffset,
                                f_pData->dwNumFeatureEntries,
                                f_pData->prgdwFeatureSet,
                                &cbElement ) );
    iOffset += cbElement;

    /*
    ** Add the Public Key Information
    */
    ChkDR( _addCertKeyInfo( f_pbNewChain + iOffset, *f_pcbNewChain - iOffset, f_pData, &cbElement ) );
    iOffset += cbElement;

    /*
    ** Add the Manufacturer Information
    */
    ChkDR( _addCertManufacturerInfo( f_pbNewChain + iOffset,
                                     *f_pcbNewChain - iOffset,
                                     f_pData->dwManufacturerFlags,
                                     f_pData->pManufacturerStrings,
                                     &cbElement) );
    iOffset += cbElement;


    /*
    ** Add the Extended Data Signature Information
    */
    ChkDR( _addExtDataSignKeyInfo( f_pbNewChain + iOffset,
                                   *f_pcbNewChain - iOffset,
                                   &f_pData->ExtendedData,
                                   &cbElement) );
    iOffset += cbElement;

    /*
    ** Future elements are added here
    */

    /*
    ** Add the Signature Information
    */
    ChkDR( _addCertSignatureInfo( f_pbNewChain + iOffset,
                                  *f_pcbNewChain - iOffset,
                                  f_pData,
                                  ( cbCert - cbSignatureInfo - cbExtendedData ),  /* # of bytes to sign */
                                  cbSignatureInfo,
                                  f_pBbxContext,
                                  f_pCryptoCtx,
                                  f_pOEMContext ) );
    iOffset += cbSignatureInfo;

    /*
    ** Add the Extended Data
    */
    ChkDR( _addExtDataContainer( f_pbNewChain + iOffset,
                                 *f_pcbNewChain - iOffset,
                                 &f_pData->ExtendedData,
                                 &cbElement,
                                 pCryptoCtxToUse ) );

    iOffset += cbElement;

    /*
    ** If this is not the first certificate in the chain, then we need to copy the parent chain
    ** after the certificate that we just added
    */
    if ( f_pbParentChain != NULL )
    {
        DRM_DWORD    cbParentChain = 0;

        NETWORKBYTES_TO_DWORD( cbParentChain, f_pbParentChain, DRM_BCERT_CHAIN_SIZE_OFFSET );
        cbParentChain -= SIZEOF_CHAIN_HEADER;
        DRM_BYT_CopyBytes( f_pbNewChain, iOffset, f_pbParentChain, SIZEOF_CHAIN_HEADER, cbParentChain );
    }

    /*
    ** Finally, we need to update the certificate chain header with the new byte size and count
    */
    ChkDR( BCert_UpdateCertChainHeader( f_pbNewChain, *f_pcbNewChain, cbCert ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**************************************************************************************************************
** Function:    BCert_AddExtendedDataToCert
**
** Synopsis:    Adds extended data into a leaf certificate in the provided certificate chain.
**              Since empty space is already reserved we only need to fill it with data in place.
**
** Arguments:   [f_pbCertChain]         : A buffer pointer to an existing certificate chain; cannot be NULL
**              [f_cbCertData]          : size of the cert chain buffer
**              [f_pExtData]            : A pointer to all extended data input
**              [f_pCryptoCtx]          : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS                - on success
**              DRM_E_INVALIDARG           - if any parameter is invalid
**              DRM_E_BCERT_CHAIN_TOO_DEEP - if the existing chain is already at maximum capacity
**              or other return code from a function called within
**
** Notes:       Just like BCert_AddCert this function does not verify the cert chain.
**              The leaf cert's and cert chain's lengths do not change - they are already set into correct values.
**************************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL BCert_AddExtendedDataToCert(
   __inout_bcount(f_cbCertData)       DRM_BYTE                     *f_pbCertChain,
   __in                         const DRM_DWORD                     f_cbCertData,
   __in                         const DRM_BCERT_EXT_BUILDER_DATA   *f_pExtData,
   __inout                            DRM_CRYPTO_CONTEXT           *f_pCryptoCtx )
{
    DRM_RESULT dr                     = DRM_SUCCESS;
    DRM_DWORD  iOffset                = 0;
    DRM_DWORD  cbExtDataInfoLength    = 0;
    DRM_DWORD  cbLeafCertTotalLength  = 0;
    DRM_DWORD  cbLeafCertSignedLength = 0;
    DRM_DWORD  cbSignatureObjLength   = 0;
    DRM_DWORD  cbElement              = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BCERT, PERF_FUNC_BCert_AddExtendedDataToCert );

    ChkArg( f_pbCertChain != NULL );
    ChkArg( f_pExtData != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    /*
    ** Calculate size of extended data object to be added.
    */
    ChkDR( _calcExtDataContainerSize( f_pExtData, &cbExtDataInfoLength, NULL, NULL ) );
    ChkBOOL( cbExtDataInfoLength > 0, DRM_E_BCERT_EXTDATA_IS_NOT_PROVIDED );

    /*
    ** Now find an offset to write extended data.
    ** Total cert length = signed length
    **                   + signature obj length
    **                   + reserved space for extended data
    */

    /*
    ** Jump over cert chain header
    */
    iOffset = SIZEOF_CHAIN_HEADER   /* cert chain header     */
            + SIZEOF(DRM_DWORD)     /* cert header - tag     */
            + SIZEOF(DRM_DWORD);    /* cert header - version */
    ChkBOOL( iOffset + SIZEOF(DRM_DWORD) <= f_cbCertData, DRM_E_BUFFERTOOSMALL );

    /*
    ** Read the length of the first cert in the chain.
    */
    NETWORKBYTES_TO_DWORD( cbLeafCertTotalLength, f_pbCertChain, iOffset );
    ChkBOOL( SIZEOF_CHAIN_HEADER + cbLeafCertTotalLength <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    ChkOverflow( cbLeafCertTotalLength, cbExtDataInfoLength );

    /*
    ** Read length of a signed portion of the cert.
    */
    iOffset += SIZEOF(DRM_DWORD);
    ChkBOOL( iOffset + SIZEOF(DRM_DWORD) <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    NETWORKBYTES_TO_DWORD( cbLeafCertSignedLength, f_pbCertChain, iOffset );
    ChkOverflow( cbLeafCertTotalLength, cbLeafCertSignedLength );

    /*
    ** Read length of a signature object
    */
    iOffset = SIZEOF_CHAIN_HEADER     /* cert chain header  */
            + cbLeafCertSignedLength  /* signed length      */
            + SIZEOF(DRM_WORD)        /* obj header - flags */
            + SIZEOF(DRM_WORD);       /* obj header - type  */
    ChkBOOL( iOffset + SIZEOF(DRM_DWORD) <= f_cbCertData, DRM_E_BUFFERTOOSMALL );
    NETWORKBYTES_TO_DWORD( cbSignatureObjLength, f_pbCertChain, iOffset );

    /*
    ** Write extended data container obj
    */
    iOffset = SIZEOF_CHAIN_HEADER     /* cert chain header    */
            + cbLeafCertSignedLength  /* signed length        */
            + cbSignatureObjLength;   /* signature obj length */
    ChkBOOL( iOffset + cbExtDataInfoLength <= f_cbCertData, DRM_E_BUFFERTOOSMALL );

    ChkDR( _addExtDataContainer( f_pbCertChain + iOffset,
                                 f_cbCertData - iOffset,
                                 f_pExtData,
                                 &cbElement,
                                 f_pCryptoCtx ) );

    /*
    ** No need to update cert length or cert chain length - it's already correct.
    */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


EXIT_PK_NAMESPACE_CODE
