/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmxmlparser.h>
#include <drmdevcert.h>
#include <oemdes.h>
#include <oempkcrypto.h>
#include <drmbcertparser.h>
#include <oem.h>
#include <drmantirollbackclock.h>
#include <drmcertparsercache.h>
#include <drmdevcertconstants.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
** Function :   _GetCertAttribute
** Synopsis :   Get attribute from given devcert
** Arguments :  f_pdstrDevCert
**              f_eAttribute  - devcert attr to get
**              f_pdstrNode   - extracted attr node with <node> tag
**              f_pdstrValue  - extracted attr node value
******************************************************************************/
static DRM_RESULT _GetCertAttribute(
    IN const DRM_CONST_STRING        *f_pdstrDevCert,          /* devcert */
    IN       eDRM_DEVCERT_ATTRIBUTES  f_eAttribute,            /* devcert attr to get */
       OUT   DRM_CONST_STRING        *f_pdstrNode,             /* extracted attr node with <node> tag */
       OUT   DRM_CONST_STRING        *f_pdstrValue)
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    const DRM_CONST_STRING      *pdstrPath          = NULL;
    const DRM_CONST_STRING      *pdstrCertType      = NULL;
    const DRM_CONST_STRING      *pdstrTagSubNode    = NULL;
    DRM_CONST_STRING             dstrSubNode        = EMPTY_DRM_STRING;

    ChkArg( f_pdstrNode != NULL
        || f_pdstrValue != NULL );
    ChkDRMString( f_pdstrDevCert );

    switch(f_eAttribute)
    {
        /*
        **  Device Cert attributes
        */
        case DRM_DEVCERT_DEVICEPRIVKEY:
            pdstrCertType   = &g_dstrCertTypeDevice;
            pdstrPath       = &g_dstrXPathDeviceKeydata;
            break;
        case DRM_DEVCERT_DEVICEDATANODE:
            pdstrCertType   = &g_dstrCertTypeDevice;
            pdstrPath       = &g_dstrTagData;
            break;
        case DRM_DEVCERT_DEVICESIGNATURE:
            pdstrCertType   = &g_dstrCertTypeDevice;
            pdstrPath       = &g_dstrTagMSDRMSignature;
            break;
        case DRM_DEVCERT_DEVICEPUBKEY:
            pdstrCertType   = &g_dstrCertTypeDevice;
            pdstrPath       = &g_dstrXPathPubkeyData;
            break;

        case DRM_DEVCERT_SERIALNUMBER:
            pdstrCertType   = &g_dstrCertTypeDevice;
            pdstrPath       = &g_dstrXPathDataUniqueID;
            break;

        case DRM_DEVCERT_DEVICEMODELNAME:
            pdstrCertType   = &g_dstrCertTypeDevice;
            pdstrPath       = &g_dstrXPathGCName;
            break;


        /*
        **  Fallback attributes
        */
        case DRM_DEVCERT_COMPATIBILITYVERSION:
            pdstrTagSubNode = &g_dstrFallBack;
            pdstrCertType   = NULL;
            pdstrPath       = &g_dstrTagSecurityVersion;
            break;
        case DRM_DEVCERT_COMPATIBILITYCERT:
            pdstrTagSubNode = &g_dstrFallBack;
            pdstrCertType   = NULL;
            pdstrPath       = &g_dstrTagCertificate;
            break;

#if DRM_SUPPORT_DEVICE_SIGNING_KEY
        /*
        ** Signing key attributes
        */
        case DRM_DEVCERT_DEVICESIGNINGCERTIFICATE:
            pdstrTagSubNode = &g_dstrTagSigning;
            pdstrCertType   = NULL;
            pdstrPath       = &g_dstrTagCertificate;
            break;
#endif


        /*
        **  GC Attributes
        */
        case DRM_DEVCERT_SECURECLOCK:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCSecureClock;
            break;
        case DRM_DEVCERT_SECURECLOCKURL:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCSecureClockURL;
            break;
        case DRM_DEVCERT_SECURECLOCKPUBKEY:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCSecureClockPubKey;
            break;
        case DRM_DEVCERT_METERING:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCMetering;
            break;
        case DRM_DEVCERT_LICENSEACQUISITIONMODE:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCLicenseAcquisitionMode;
            break;
        case DRM_DEVCERT_LICENSESYNCMODE:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCLicenseSyncMode;
            break;
        case DRM_DEVCERT_SYMMETRICOPTIMIZATIONS:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCSymmOptsMode;
            break;
        case DRM_DEVCERT_ENCRYPTION:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCEncryption;
            break;
        case DRM_DEVCERT_SUPPORT_REVOCATION:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCSupportRevocation;
            break;
        case DRM_DEVCERT_MAXCHAINDEPTH:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCMaxChainDepth;
            break;
        case DRM_DEVCERT_MAXLICENSESIZE:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCMaxLicenseSize;
            break;
        case DRM_DEVCERT_MAXHEADERSIZE:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCMaxHeaderSize;
            break;
        case DRM_DEVCERT_GROUPSECURITYLEVEL:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathTagSecurityLevel;
            break;
        case DRM_DEVCERT_GROUPSECURITYVERSIONNUM:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathTagSecurityVersionNum;
            break;
        case DRM_DEVCERT_GROUPPLATFORMID:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathTagPlatformID;
            break;
        case DRM_DEVCERT_GROUPCERTPUBKEY:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathPubkeyData;
            break;
        case DRM_DEVCERT_GROUPCERTDATANODE:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrTagData;
            break;
        case DRM_DEVCERT_GROUPCERTSIGNATURE:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrTagMSDRMSignature;
            break;
        case DRM_DEVCERT_SUPPORTCRL:
            pdstrCertType   = &g_dstrCerttypeGroup;
            pdstrPath       = &g_dstrXPathGCSupportCRL;
            break;

        /*
        **  DAC attributes
        */
        case DRM_DEVCERT_DACSECURITYLEVEL:
            pdstrCertType   = &g_dstrCerttypeAuth;
            pdstrPath       = &g_dstrXPathTagSecurityLevel;
            break;
        case DRM_DEVCERT_SUBJECTID:
            pdstrCertType   = &g_dstrCerttypeAuth;
            pdstrPath       = &g_dstrXPathDACAuthID;
            break;
        case DRM_DEVCERT_DACPUBKEY:
            pdstrCertType   = &g_dstrCerttypeAuth;
            pdstrPath       = &g_dstrXPathPubkeyData;
            break;
        case DRM_DEVCERT_DACDATANODE:
            pdstrCertType   = &g_dstrCerttypeAuth;
            pdstrPath       = &g_dstrTagData;
            break;
        case DRM_DEVCERT_DACSIGNATURE:
            pdstrCertType   = &g_dstrCerttypeAuth;
            pdstrPath       = &g_dstrTagMSDRMSignature;
            break;
        case DRM_DEVCERT_DACROOTPUBKEY:
            pdstrCertType   = &g_dstrCerttypeAuthRoot;
            pdstrPath       = &g_dstrXPathPubkeyData;
            break;
        case DRM_DEVCERT_AUTHROOTDATANODE:
            pdstrCertType   = &g_dstrCerttypeAuthRoot;
            pdstrPath       = &g_dstrTagData;
            break;
        case DRM_DEVCERT_AUTHROOTSIGNATURE:
            pdstrCertType   = &g_dstrCerttypeAuthRoot;
            pdstrPath       = &g_dstrTagMSDRMSignature;
            break;

        default:
            ChkDR(DRM_E_INVALIDARG);
    }

    if (pdstrCertType != NULL)
    {
        ChkDR( DRM_XML_GetSubNode( f_pdstrDevCert,
                                   &g_dstrTagCertificate,
                                   &g_dstrAttributeType,
                                   pdstrCertType,
                                   0,
                                   &dstrSubNode,
                                   NULL,
                                   1 ) );
    }
    else
    {
        ChkDR( DRM_XML_GetSubNode( f_pdstrDevCert,
                                   pdstrTagSubNode,
                                   NULL,
                                   NULL,
                                   0,
                                   &dstrSubNode,
                                   NULL,
                                   1 ) );
    }

    ChkDR( DRM_XML_GetSubNodeByPath(&dstrSubNode,
                                     pdstrPath,
                                     NULL,
                                     NULL,
                                     f_pdstrNode,
                                     f_pdstrValue,
                                     g_wchForwardSlash ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_DCP_GetAttribute
**
** Synopsis: Function that parses the XML device cert attribute.
**           If the passed in cert parser cache is not NULL,
**           it first tries to find the attribute in the cert parser cache.
**           If it is not found, fall back to use XML parsing to get the
**           attribute value.
**           If the passed in cert parser cache is NULL, it just does the
**           XML parsing to get the attribute value.
**
** Arguments:
**
** [f_poParserCache]        -- Pointer to an optional cert parser cache.
** [f_pdstrDevCert]         -- Pointer to a XML device cert.
** [f_eAttribute]           -- Attribute who value is going to be retrieved.
** [f_pdstrNode]            -- Pointer to a DRM string to receive the substring
**                             of the attribute value (including tag).
** [f_pdstrValue]           -- Pointer to a DRM string to receive the substring
**                             of the attribute value (excluding tag).
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DCP_GetAttribute(
    __in_opt  DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache,
    __in      const DRM_CONST_STRING       *f_pdstrDevCert,
    __in      eDRM_DEVCERT_ATTRIBUTES       f_eAttribute,
    __out_opt DRM_CONST_STRING             *f_pdstrNode,
    __out_opt DRM_CONST_STRING             *f_pdstrValue )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DEVCERT, PERF_FUNC_DRM_DCP_GetAttribute);

    ChkDRMString( f_pdstrDevCert );
    ChkArg( f_eAttribute < DRM_DEVCERT_MAXATTRIBUTES );

    if ( DRM_CERTPARSERCACHE_IsCertParserCacheSupported()
      && f_poParserCache != NULL )
    {
        DRM_DEVICE_CERT_PARSER_CACHE_ENTRY oEntry = { 0 };
        DRM_CONST_STRING dstrNode = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
        DRM_BOOL fUseCache = TRUE;
        ChkDR( DRM_CERTPARSERCACHE_UnpackCertParserEntry(
            f_eAttribute,
            f_poParserCache->m_rgbPacked,
            SIZEOF( f_poParserCache->m_rgbPacked ),
            &oEntry ) );

        if ( !oEntry.m_fValid )
        {
            fUseCache = FALSE;
        }
        else if ( ( DRM_DWORD )( oEntry.m_iNodeStart ) >= f_pdstrDevCert->cchString ||
                  ( ( DRM_DWORD )( oEntry.m_iNodeStart ) +
                    ( DRM_DWORD )( oEntry.m_iNodeLength ) > f_pdstrDevCert->cchString ) )
        {
            fUseCache = FALSE;
        }
        else if ( ( DRM_DWORD )( oEntry.m_iValueStart ) >= f_pdstrDevCert->cchString ||
                  ( ( DRM_DWORD )( oEntry.m_iValueStart ) +
                    ( DRM_DWORD )( oEntry.m_iValueLength ) > f_pdstrDevCert->cchString ) )
        {
            fUseCache = FALSE;
        }

        if ( !fUseCache )
        {
            /* Fall back to use XML parsing to get the attribute. */
            ChkDR( _GetCertAttribute( f_pdstrDevCert,
                                      f_eAttribute,
                                      &dstrNode,
                                      &dstrValue ) );

            /* Memorize and save the result in the cache so it can be used later. */
            oEntry.m_iNodeStart = dstrNode.pwszString - f_pdstrDevCert->pwszString;
            oEntry.m_iNodeLength = ( DRM_WORD )dstrNode.cchString;

            oEntry.m_iValueStart = dstrValue.pwszString - f_pdstrDevCert->pwszString;
            oEntry.m_iValueLength = ( DRM_WORD )dstrValue.cchString;

            oEntry.m_fValid = TRUE;

            if ( f_pdstrNode != NULL )
            {
                f_pdstrNode->pwszString = dstrNode.pwszString;
                f_pdstrNode->cchString = dstrNode.cchString;
            }

            if ( f_pdstrValue != NULL )
            {
                f_pdstrValue->pwszString = dstrValue.pwszString;
                f_pdstrValue->cchString = dstrValue.cchString;
            }

            ChkDR( DRM_CERTPARSERCACHE_PackCertParserEntry(
                f_eAttribute,
                f_poParserCache->m_rgbPacked,
                SIZEOF( f_poParserCache->m_rgbPacked ),
                &oEntry ) );

            f_poParserCache->m_fUpdated = TRUE;
        }
        else
        {
            /* There is a previously parsed result, just return it to the caller. */
            if ( f_pdstrNode != NULL )
            {
                f_pdstrNode->pwszString = f_pdstrDevCert->pwszString + oEntry.m_iNodeStart;
                f_pdstrNode->cchString = oEntry.m_iNodeLength;
            }

            if ( f_pdstrValue != NULL )
            {
                f_pdstrValue->pwszString = f_pdstrDevCert->pwszString + oEntry.m_iValueStart;
                f_pdstrValue->cchString = oEntry.m_iValueLength;
            }
        }
    }
    else
    {
        ChkDR( _GetCertAttribute( f_pdstrDevCert,
                                  f_eAttribute,
                                  f_pdstrNode,
                                  f_pdstrValue ) );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DCP_StartCertParserCache
**
** Synopsis: Function that prepares the cert parser cache to be used by
**           the following calls of DRM_DCP_GetAttribute().
**
** Arguments:
**
** [f_pdstrDevCert]         -- Pointer to a XML device cert.
** [f_poKeyFileContext]     -- Optional pointer to a key file context.
** [f_poParserCache]        -- Pointer to a device cert parser cache to
**                             be initialized.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DCP_StartCertParserCache(
    __in     const DRM_CONST_STRING       *f_pdstrDevCert,
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __out    DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    return DRM_CERTPARSERCACHE_DCP_StartCertParserCache(
        f_pdstrDevCert,
        f_poKeyFileContext,
        f_poParserCache );
}

/*********************************************************************
**
** Function: DRM_DCP_StopCertParserCache
**
** Synopsis: Function that flushes the cert parser cache to the key file
**           if needed.
**
** Arguments:
**
** [f_poKeyFileContext]     -- Optional pointer to a key file context.
** [f_poParserCache]        -- Pointer to a device cert parser cache to
**                             be initialized.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DCP_StopCertParserCache(
    __in_opt DRM_KEYFILE_CONTEXT          *f_poKeyFileContext,
    __in     DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache )
{
    return DRM_CERTPARSERCACHE_DCP_StopCertParserCache(
        f_poKeyFileContext,
        f_poParserCache );
}

/******************************************************************************
**
** Function :   _DCP_VerifyBasicValues
** Synopsis :   Verify the existence of certain required nodes, as well as
**              validity of the data supplied
******************************************************************************/
static DRM_RESULT _DCP_VerifyBasicValues(
    __in_opt DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache,
    IN const DRM_CONST_STRING   *f_pdstrDevCert,
    IN       DRM_CRYPTO_CONTEXT *f_pcontextCrypto  )
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_CONST_STRING dstrValue      = EMPTY_DRM_STRING;
    DRM_LONG         lValue         = 0;
    DRM_LONG         lMaxSecLevel   = 0;

    /*
    **  No need to check args in internal function - always checked
    */

    /*
    **  Get max SECURITYLEVEL from DAC
    */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                 f_pdstrDevCert,
                                 DRM_DEVCERT_DACSECURITYLEVEL,
                                 NULL,
                                 &dstrValue ) );
    ChkDR( DRMCRT_wcsntol (dstrValue.pwszString, dstrValue.cchString, &lMaxSecLevel));

    /*
    **  Get GC security level
    */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                 f_pdstrDevCert,
                                 DRM_DEVCERT_GROUPSECURITYLEVEL,
                                 NULL,
                                 &dstrValue ) );
    ChkDR( DRMCRT_wcsntol (dstrValue.pwszString, dstrValue.cchString, &lValue));
    if ( lValue > lMaxSecLevel )
    {
        ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        dr = DRM_E_INVALIDDEVICECERTIFICATE;
    }

    return dr;
}


/******************************************************************************
**
** Function :   _DCP_VerifyCert
**
** Synopsis :   Verify any particular cert in the devcert
**
** Arguments:
**
** [f_poParserCache]        -- Pointer to an optional cert parser cache.
** [f_pdstrDevCert]         -- Pointer to a device cert string.
** [f_pcontextCrypto]       -- Pointer to a crypto context.
** [f_pdstrPubkey]          -- Pointer to a pubkey used to verify cert.
** [f_eDataLocation]        -- Attrib to locate the signed data in devcert.
** [f_eSignatureLocation]   -- Attrib to locate the signature over given data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
******************************************************************************/
static DRM_RESULT _DCP_VerifyCert(
    __in_opt    DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache,
    IN const    DRM_CONST_STRING             *f_pdstrDevCert,
    IN          DRM_CRYPTO_CONTEXT           *f_pcontextCrypto,
    IN  const   DRM_CONST_STRING             *f_pdstrPubkey,
    IN          eDRM_DEVCERT_ATTRIBUTES       f_eDataLocation,
    IN          eDRM_DEVCERT_ATTRIBUTES       f_eSignatureLocation )
{
    DRM_RESULT          dr              =   DRM_SUCCESS;
    DRM_CONST_STRING    dstrData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING    dstrSignature   = EMPTY_DRM_STRING;
    DRM_DWORD           cbPubkey        = SIZEOF(PUBKEY);
    DRM_DWORD           cbSignature     = SIZEOF(f_pcontextCrypto->signature);

    /*
    **  No need to check args in internal function - always checked
    */

    /*
    **  Get the DEVCERT\CERTIFICATE\DATA SECTION
    */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                 f_pdstrDevCert,
                                 f_eDataLocation,
                                 &dstrData,
                                 NULL ) );

    /*
    **  Decode the key and put it in the crypto context
    */
    ChkDR( DRM_B64_DecodeW(f_pdstrPubkey,
                          &cbPubkey,
                           f_pcontextCrypto->pubKey.y,
                           0) );

    /*
    **  Get the signature
    */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                 f_pdstrDevCert,
                                 f_eSignatureLocation,
                                 NULL,
                                 &dstrSignature ) );

    /*
    **  Verify the signature
    */
    ChkDR(DRM_B64_DecodeW( &dstrSignature,
                           &cbSignature,
                           f_pcontextCrypto->signature,
                           0 ) );

    ChkDR( DRM_PK_Verify( f_pcontextCrypto->rgbCryptoContext,
                          eDRM_ECC_P160,
                          DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE * )&f_pcontextCrypto->pubKey,
                          CB_DSTR( &dstrData ),
                          PB_DSTR( &dstrData ),
                          DRM_ECC160_SIGNATURE_LEN,
                          f_pcontextCrypto->signature ) );

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        dr = DRM_E_INVALIDDEVICECERTIFICATETEMPLATE;
    }
    return dr;
}

/******************************************************************************
** Function  :   DRM_DCP_VerifyDeviceCert
** Synopsis  :
** Arguments :
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DCP_VerifyDeviceCert(
    __in_opt   DRM_DEVICE_CERT_PARSER_CACHE *f_poParserCache,
    __in const DRM_CONST_STRING       *f_pdstrDevCert,
    __in       DRM_DWORD               f_dwVerifyFlags,
    __in_opt   DRM_KEYFILE_CONTEXT    *f_poKeyFileContext,
    __in       DRM_CRYPTO_CONTEXT     *f_pcontextCRYP )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrPubkey = EMPTY_DRM_STRING;

    /*
    **  Check input
    */
    ChkArg (f_pcontextCRYP != NULL);
    ChkArg((f_dwVerifyFlags & ~DRM_DCP_VERIFY_ENTIRE_DEVCERT) == 0);

    ChkDRMString( f_pdstrDevCert );


    /* verify basic values */
    ChkDR( _DCP_VerifyBasicValues( f_poParserCache, f_pdstrDevCert, f_pcontextCRYP ) );

    if( f_dwVerifyFlags & DRM_DCP_VERIFY_GROUP_CERT )
    {
        /*
        **  Get the Public DAC Key
        */
        ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                     f_pdstrDevCert,
                                     DRM_DEVCERT_DACPUBKEY,
                                     NULL,
                                     &dstrPubkey ) );

        ChkDR( _DCP_VerifyCert( f_poParserCache,
                                f_pdstrDevCert,
                                f_pcontextCRYP,
                               &dstrPubkey,
                                DRM_DEVCERT_GROUPCERTDATANODE,
                                DRM_DEVCERT_GROUPCERTSIGNATURE ) );
    }

    if (f_dwVerifyFlags & DRM_DCP_VERIFY_DEVICE_CERT)
    {
        DRM_DWORD cb = 0;

        /* Get the compatability certificate */
        ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                     f_pdstrDevCert,
                                     DRM_DEVCERT_COMPATIBILITYCERT,
                                     NULL,
                                     &dstrPubkey));
        cb = SIZEOF(PKCERT);
        if (DRM_FAILED(DRM_B64_DecodeW(&dstrPubkey,
                                       &cb,
                          (DRM_BYTE *) &f_pcontextCRYP->union_cert.pkcert,
                                        0)))
        {
            dr = DRM_E_INVALIDDEVICECERTIFICATE;
            goto ErrorExit;
        }

        /*
        **  Get the Public Group Key
        */
        ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                     f_pdstrDevCert,
                                     DRM_DEVCERT_GROUPCERTPUBKEY,
                                     NULL,
                                     &dstrPubkey ) );
        cb = SIZEOF(PUBKEY);
        if (DRM_FAILED(DRM_B64_DecodeW(&dstrPubkey,
                                       &cb,
                            (DRM_BYTE*)&f_pcontextCRYP->pubKey,
                                        0)))
        {
            ChkDR(DRM_E_INVALIDDEVICECERTIFICATE);
        }

        /* Verify the rest of the certificate */
        ChkDR( _DCP_VerifyCert( f_poParserCache,
                                f_pdstrDevCert,
                                f_pcontextCRYP,
                               &dstrPubkey,
                                DRM_DEVCERT_DEVICEDATANODE,
                                DRM_DEVCERT_DEVICESIGNATURE) );

    }

    if (f_dwVerifyFlags & DRM_DCP_VERIFY_DAC)
    {
        INIT_DRM_STRING(dstrPubkey);

        ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                     f_pdstrDevCert,
                                     DRM_DEVCERT_DACROOTPUBKEY,
                                     NULL,
                                     &dstrPubkey));

        ChkDR( _DCP_VerifyCert    (f_poParserCache,
                                   f_pdstrDevCert,
                                   f_pcontextCRYP,
                                  &dstrPubkey,
                                   DRM_DEVCERT_DACDATANODE,
                                   DRM_DEVCERT_DACSIGNATURE) );

        ChkDR( _DCP_VerifyCert    (f_poParserCache,
                                   f_pdstrDevCert,
                                   f_pcontextCRYP,
                                  &g_dstrMSRootPubKey,
                                   DRM_DEVCERT_AUTHROOTDATANODE,
                                   DRM_DEVCERT_AUTHROOTSIGNATURE));
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_DCP_LoadPropertiesCache(
    __in_opt DRM_DEVICE_CERT_PARSER_CACHE          *f_poParserCache,
    __in     const DRM_CONST_STRING                *f_pdstrDevCert,
    __in_opt DRM_KEYFILE_CONTEXT                   *f_poKeyFileContext,
    __in     DRM_CRYPTO_CONTEXT                    *f_poCryptoContext,
    __out    DRM_DEVICE_CERTIFICATE_CACHED_VALUES  *f_pcacheDevCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;
    DRM_DWORD cb     = 0;
    DRM_LONG  lValue = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DEVCERT, PERF_FUNC_DRM_DCP_LoadPropertiesCache);

    ChkDRMString( f_pdstrDevCert );
    ChkArg( f_pcacheDevCert != NULL
         && f_poCryptoContext != NULL );

    /*
    ** First get all the keys and certificates
    */

    ChkDR( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_DEVICEPUBKEY, NULL, &dstrValue ) );

    cb = SIZEOF( PUBKEY );

    if ( DRM_FAILED( DRM_B64_DecodeW( &dstrValue,
                                      &cb,
                                      ( DRM_BYTE* )&f_pcacheDevCert->pubkeyCurrent,
                                      0 ) ) )
    {
        ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

#if DRM_SUPPORT_DEVICE_SIGNING_KEY
    /*
    ** get the device signing cert
    */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_DEVICESIGNINGCERTIFICATE, NULL, &dstrValue ) );

    cb = SIZEOF(PKCERT);
    if (DRM_FAILED(DRM_B64_DecodeW(&dstrValue,
                                    &cb,
                        (DRM_BYTE*)&f_pcacheDevCert->m_pubKeySigningCert,
                                    0)))
    {
        ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

    /*
    ** Get the device public signing key from the cert and cache the value
    */
    MEMCPY( ( DRM_BYTE* )&f_pcacheDevCert->pubkeySigning,
             (DRM_BYTE* )&f_pcacheDevCert->m_pubKeySigningCert.pk.pk,
             SIZEOF( PUBKEY ) );

#endif

    /* get the Compatibility cert */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_COMPATIBILITYCERT, NULL, &dstrValue ) );

    cb = SIZEOF(PKCERT);
    if (DRM_FAILED(DRM_B64_DecodeW(&dstrValue,
                                    &cb,
                        (DRM_BYTE*)&f_pcacheDevCert->m_BBCompatibilityCert,
                                    0)))
    {
        ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

    /* Get the legacy version */
    if ( DRM_SUCCEEDED( DRM_DCP_GetAttribute( f_poParserCache,
                                              f_pdstrDevCert,
                                              DRM_DEVCERT_COMPATIBILITYVERSION,
                                              NULL,
                                              &dstrValue) ) )
    {
        if( dstrValue.cchString > NO_OF( f_pcacheDevCert->wszLegacyVersion ) - 1 )
        {
            ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
        }
        ChkDR( DRM_STR_StringCchCopyNW( f_pcacheDevCert->wszLegacyVersion,
                                NO_OF( f_pcacheDevCert->wszLegacyVersion ),
                                       dstrValue.pwszString,
                                       dstrValue.cchString ) );
        f_pcacheDevCert->wszLegacyVersion[dstrValue.cchString] = g_wchNull;
    }
    else
    {
        /* There is no compatability certificate version. */
        ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
    }

    /*
    ** Next populate all the feature flags in the cached structure
    */

    f_pcacheDevCert->dwFeatureFlags = 0;

    ChkDR( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_GROUPSECURITYLEVEL, NULL, &dstrValue ) );
    ChkDR(DRMCRT_wcsntol( dstrValue.pwszString, dstrValue.cchString, (DRM_LONG *) &f_pcacheDevCert->appSec ) );

    ChkDR( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_SUBJECTID, NULL, &dstrValue));
    ChkDR(DRMCRT_wcsntol(dstrValue.pwszString, dstrValue.cchString, (DRM_LONG *) &f_pcacheDevCert->appcd_subject));

    /* Get the secure clock from the devcert */
    if ( DRM_SUCCEEDED( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_SECURECLOCK, NULL, &dstrValue ) ) )
    {
        ChkDR( DRMCRT_wcsntol ( dstrValue.pwszString, dstrValue.cchString, &lValue ) );
        if( lValue == DRM_CLOCK_SECURE )
        {
            /* If the device supports secure clock then there must be a secure clock server public key in the cert */
            ChkDR( DRM_DCP_GetAttribute( f_poParserCache, f_pdstrDevCert, DRM_DEVCERT_SECURECLOCKPUBKEY, NULL, &dstrValue ) );
            cb = SIZEOF( PUBKEY );
            if( DRM_FAILED( DRM_B64_DecodeW( &dstrValue,
                                             &cb,
                                  (DRM_BYTE*)&(f_pcacheDevCert->pubkeySecureClockServer),
                                              0)))
            {
                ChkDR( DRM_E_INVALIDDEVICECERTIFICATE );
            }
            f_pcacheDevCert->dwFeatureFlags |= DRM_FEATURE_SECURE_CLOCK;
            if( DRM_ARCLK_IsAntirollbackClockSupported() )
            {
                f_pcacheDevCert->dwFeatureFlags |= DRM_FEATURE_ANTI_ROLLBACK_CLOCK;
            }
        }

        if( DRM_ARCLK_IsAntirollbackClockSupported() && lValue == DRM_CLOCK_ANTIROLLBACK )
        {
            f_pcacheDevCert->dwFeatureFlags |= DRM_FEATURE_ANTI_ROLLBACK_CLOCK;
        }
    }

    /* Get the Metering from the devcert */
    if ( DRM_SUCCEEDED( DRM_DCP_GetAttribute( f_poParserCache,
                                              f_pdstrDevCert,
                                              DRM_DEVCERT_METERING,
                                              NULL,
                                              &dstrValue) ) )
    {
        ChkDR( DRMCRT_wcsntol ( dstrValue.pwszString, dstrValue.cchString, &lValue ) );
        if( lValue == 1 )
        {
            f_pcacheDevCert->dwFeatureFlags |= DRM_FEATURE_METERING;
        }
    }

    /* Load DRM_DEVCERT_GROUPCERTPUBKEY so it can be stored in the cache. */
    ChkDR( DRM_DCP_GetAttribute( f_poParserCache,
                                 f_pdstrDevCert,
                                 DRM_DEVCERT_GROUPCERTPUBKEY,
                                 NULL,
                                 &dstrValue ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
