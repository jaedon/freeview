/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMLICENSEPARSERREAL_C
#include <drmwmdrm.h>
#include <drmsymopt.h>
#include <drmxmlparser.h>
#include <drmutilities.h>
#include <drmwmdrmconstants.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define RESTRICTED_COUNT    3
static const DRM_BYTE CERT_SUBJECT_RESTRICTED[RESTRICTED_COUNT][__CB_DECL(SUBJ_LEN)] =
{
    {   0, 0, 0, 2    },
    {   0, 0, 0, 3    },
    {   0, 0, 0, 4    }
};

static const PUBKEY g_BrLicSrvPubl =
{
    {
        0x34, 0x8b, 0x02, 0xae, 0xb6, 0x3a, 0x58, 0x8d,
        0xb6, 0xf5, 0x38, 0x3b, 0x46, 0x47, 0xb2, 0xcd,
        0xf5, 0x77, 0x67, 0x31, 0xaa, 0xe8, 0x33, 0x44,
        0x44, 0xdb, 0x90, 0x95, 0xbb, 0x7f, 0xcc, 0x51,
        0x80, 0x9d, 0x2d, 0x3a, 0x7f, 0xbd, 0x4c, 0x08
    }
};

/******************************************************************************
**
** Function :   DRM_WMDRM_LIC_GetAttribute
**
** Synopsis :   Get an attribute from a license
**
** Arguments :  pdstrLicense    - License string
**              pdstrAttrName   - Name of attribute
**              eAttribute      - Type of ettribute
**              pdstrLIData     - (Optional) LICENSORINFO\DATA section. If a
**                                valid string, attribute will be searched
**                                directly in this. On exit, the pointer will be
**                                updated
**              pdstrValue      - Value of attribute
**              chXPathSeparator- path separator to use
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetAttribute(
    __in         const DRM_CONST_STRING        *pdstrLicense,
    __in_opt     const DRM_CONST_STRING        *pdstrAttrName,
    __in               eDRM_LICENSE_ATTRIBUTES  eAttribute,
    __inout_opt        DRM_CONST_STRING        *pdstrLIData,
    __out              DRM_CONST_STRING        *pdstrValue,
    __in               DRM_WCHAR                chXPathSeparator )
{
    DRM_RESULT               dr = DRM_SUCCESS;
    DRM_CONST_STRING        *pdstrSectionData   = NULL;
    DRM_CONST_STRING         dstrSectionData    = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrLicenseBody    = EMPTY_DRM_STRING;
    const DRM_CONST_STRING  *pdstr              = NULL;
    DRM_BOOL                 fParseRevocationIndex = FALSE;

    ChkArg( pdstrLicense != NULL
         && pdstrValue   != NULL );

    if( chXPathSeparator == 0 )
    {
        chXPathSeparator = g_wchForwardSlash;
    }
    if ((eAttribute != DRM_LICENSE_ATTRIB_OTHER &&
         eAttribute != DRM_LICENSE_ATTRIB_REVOCATION &&
         eAttribute != DRM_LICENSE_ATTRIB_META_ATTRIBUTE ) &&
         NULL       != pdstrAttrName )
    {
        /* We allow user specified strings for META attributes, OTHER attribues and REVOCATION attributes */
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    if ( pdstrLIData == NULL )
    {
        pdstrSectionData = &dstrSectionData;
    }
    else
    {
        pdstrSectionData = pdstrLIData;
    }
    if ( pdstrLIData == NULL || pdstrLIData->cchString == 0 )
    {
        ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLIData, NULL, NULL, pdstrSectionData, NULL, g_wchForwardSlash ) );
    }

    switch( eAttribute )
    {

        case DRM_LICENSE_ATTRIB_VERSION:
            /* skip <?xml version="1.0"?>, if any */
            ChkDR(DRM_XML_GetNode(pdstrLicense, &g_dstrLicense, NULL, NULL, 0, &dstrLicenseBody, NULL));

            /* get the attr value */
            dr = DRM_XML_GetNodeAttribute( &dstrLicenseBody, &g_dstrAttributeVersion, pdstrValue);
            goto ErrorExit;

        case DRM_LICENSE_ATTRIB_KID:
            pdstr = &g_dstrTagKID;
            break;

        case DRM_LICENSE_ATTRIB_LID:
            pdstr = &g_dstrTagLID;
            break;

        case DRM_LICENSE_ATTRIB_METERING_ID:
            pdstr = &g_dstrTagMID;
            break;

        case DRM_LICENSE_ATTRIB_REVOCATION:
            /* What type of revocation are they asking for? */
            /* We special case app revocation... */
            ChkArg( pdstrAttrName ); /* An attribute name is required if revocation lists are needed */
            if( DRM_UTL_DSTRStringsEqual( &g_dstrAppRevocation, pdstrAttrName ) )
            {
                pdstr = &g_dstrAppRevocation;
                pdstrAttrName = NULL;
            }
            else
            {
                fParseRevocationIndex = TRUE;
                pdstr = &g_dstrTagRevocation;
            }
            break;

        case DRM_LICENSE_ATTRIB_ISSUEDATE:
            pdstr = &g_dstrIssueDate;
            break;

        case DRM_LICENSE_ATTRIB_CONTENTPUBKEY:
            pdstr = &g_dstrContentPubKey;
            break;

        case DRM_LICENSE_ATTRIB_PRIORITY:
            pdstr = &g_dstrPriority;
            break;

        case DRM_LICENSE_ATTRIB_CHAINEDKID:
            pdstr = &g_dstrUplinkKid;
            break;

        case DRM_LICENSE_ATTRIB_META_ATTRIBUTE:
            /* We are at the Data section.  We need to move to the META section */
            ChkDR( DRM_XML_GetSubNodeByPath( pdstrSectionData, &g_dstrMeta, NULL, NULL, pdstrValue, NULL, g_wchForwardSlash ) );
            pdstrSectionData = pdstrValue;
            /* Fall through as META and regular attribute are the same.  Only difference is that we needed to be at a different
            node for META, which we are now. */
            __fallthrough;
        case DRM_LICENSE_ATTRIB_OTHER:
            ChkArg( pdstrAttrName != NULL );
            pdstr = pdstrAttrName;
            pdstrAttrName = NULL;
            break;

        case DRM_LICENSE_ATTRIB_REVINFO:
            pdstr = &g_dstrTagRevInfoVersion;
            break;

        case DRM_LICENSE_ATTRIB_SOURCEID:
            pdstr = &g_dstrTagSourceID;
            break;

        default:
            ChkDR (DRM_E_INVALIDARG);
    }
    if( pdstrAttrName == NULL )
    {
        ChkDR( DRM_XML_GetSubNodeByPath( pdstrSectionData, pdstr, NULL, NULL, NULL, pdstrValue, chXPathSeparator ) );
    }
    else
    {
        ChkDR( DRM_XML_GetSubNodeByPath( pdstrSectionData, pdstr, &g_dstrAttributeType, pdstrAttrName, NULL, pdstrValue, chXPathSeparator ) );
    }

    if( fParseRevocationIndex )
    {
        DRM_CONST_STRING dstrValue = *pdstrValue;

        /*
        **  The caller is asking for a revocation list version requirement.  We should have a string that looks like
        **  <INDEX>x<INDEX> and we want to return x.
        */
        ChkDR( DRM_XML_GetSubNode( &dstrValue, &g_dstrTagIndex, NULL, NULL, 0, NULL, pdstrValue, 0));
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_WMDRM_LIC_GetEvent
**
** Synopsis :   Parse and get details about an event from a license
**
** Arguments :  pdstrLicense    - License string
**              pdstrEvent      - Name of event
**              pdstrTypeValue  - Type of event
**              pdstrLIData     - (Optional) LICENSORINFO\DATA section. If a
**                                valid string, attribute will be searched
**                                directly in this. On exit, the pointer will be
**                                updated
**              pdstrCondition  - Condition node from event
**              pdstrAction     - Action node from event
**              pdstrRestriction- Restriction node from event
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetEvent(
    __in        const DRM_CONST_STRING *pdstrLicense,
    __in        const DRM_CONST_STRING *pdstrEvent,
    __in_opt    const DRM_CONST_STRING *pdstrTypeValue,
    __inout_opt       DRM_CONST_STRING *pdstrLIData,
    __out_opt         DRM_CONST_STRING *pdstrCondition,
    __out_opt         DRM_CONST_STRING *pdstrAction,
    __out_opt         DRM_CONST_STRING *pdstrRestriction )
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_CONST_STRING    *pdstrSectionData   = NULL;
    DRM_CONST_STRING     dstrSectionData    = EMPTY_DRM_STRING;
    DRM_CONST_STRING     dstrListEvent      = EMPTY_DRM_STRING;

    if ( pdstrLIData == NULL)
    {
        pdstrSectionData = &dstrSectionData;
        ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLIData, NULL, NULL, pdstrSectionData, NULL, g_wchForwardSlash ) );
    }
    else
    {
        pdstrSectionData = pdstrLIData;
        if ( pdstrSectionData->cchString == 0)
        {
            pdstrSectionData = pdstrLIData;
            ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLIData, NULL, NULL, pdstrSectionData, NULL, g_wchForwardSlash ) );
        }
    }
    ChkDR( DRM_XML_GetSubNode( pdstrSectionData,
                               pdstrEvent,
                               pdstrTypeValue?&g_dstrAttributeType:NULL,
                               pdstrTypeValue,
                               0,
                               &dstrListEvent,
                               NULL,
                               1 ) );
    if (NULL != pdstrCondition)
    {
        /* ONSTORE must use ACTION.CDATA not CONDITION.CDATA... */
        if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrListEvent, &g_dstrCondition, NULL, NULL, NULL, pdstrCondition, g_wchForwardSlash) ) )
        {
            INIT_DRM_STRING( *pdstrCondition );
        }
    }

    if (NULL != pdstrAction)
    {
        if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrListEvent, &g_dstrAction, NULL, NULL, NULL, pdstrAction, g_wchForwardSlash) ) )
        {
            INIT_DRM_STRING( *pdstrAction );
        }
    }

    if (NULL != pdstrRestriction)
    {
        if( DRM_FAILED( DRM_XML_GetSubNode( &dstrListEvent,
                                            &g_dstrRestrictions,
                                            NULL,
                                            NULL,
                                            0,
                                            NULL,
                                            pdstrRestriction,
                                            1 ) ) )
        {
            INIT_DRM_STRING( *pdstrRestriction );
        }
    }

    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_VerifyCertChain(
    __in  const DRM_CONST_STRING    *pdstrLicense,
    __in        DRM_BOOL             fCheckExpiry,
    __in        DRM_LICEVAL_CONTEXT *pcontextLEVL,
    __out       DRM_LONG            *plResult )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrCert          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListCertChain = EMPTY_DRM_STRING;
    DRM_DWORD        dwIndex = 0;
    DRM_BOOL         fFirstCertCheck = TRUE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICPARSER, PERF_FUNC_DRM_WMDRM_LIC_VerifyCertChain);

    ChkArg( plResult     != NULL
         && pdstrLicense != NULL
         && pcontextLEVL != NULL );

    *plResult = 0;

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrCertChain, NULL, NULL, &dstrListCertChain, NULL, g_wchForwardSlash ) );

    for(dwIndex = 0; DRM_SUCCEEDED(dr); dwIndex++ )
    {
        DRM_DWORD dwSize = SIZEOF( CERT );
        dr = DRM_XML_GetSubNode( &dstrListCertChain,
                                 &g_dstrTagCertificate,
                                 NULL,
                                 NULL,
                                 dwIndex,
                                 NULL,
                                 &dstrCert,
                                 1 );
        if ( (dr == DRM_E_XMLNOTFOUND) && (dwIndex == 0) )
        {
            ChkDR( DRM_E_LIC_INVALID_LICENSE );
        }
        else if ( DRM_SUCCEEDED(dr) )
        {
            ChkDR(DRM_B64_DecodeW(&dstrCert,
                                  &dwSize,
                                  (DRM_BYTE*) &(pcontextLEVL->pcontextBBX->CryptoContext.union_cert.cert),
                                  0));
            ChkDR( DRM_UTL_CheckCert( &(pcontextLEVL->pcontextBBX->CryptoContext.union_cert.cert),
                                      fFirstCertCheck?NULL:&(pcontextLEVL->pcontextBBX->CryptoContext.pubKey),
                                      fCheckExpiry,
                                      pcontextLEVL) );
            MEMCPY( &(pcontextLEVL->pcontextBBX->CryptoContext.pubKey),
                    &(pcontextLEVL->pcontextBBX->CryptoContext.union_cert.cert.cd.pk),
                    SIZEOF( PUBKEY ) );
            fFirstCertCheck = FALSE;
        }
    }

    *plResult = 1;
    dr = DRM_SUCCESS;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetLSPubKey(
    __in const DRM_CONST_STRING   *pdstrLicense,
    __in       PUBKEY             *pbPubKey,
    __in       DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrCert          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListCertChain = EMPTY_DRM_STRING;
    DRM_DWORD        iNode = 0;

    if (pbPubKey == NULL)
    {
        dr = DRM_E_LIC_PARAM_NOT_OPTIONAL;
        goto ErrorExit;
    }

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrCertChain, NULL, NULL, &dstrListCertChain, NULL, g_wchForwardSlash ) );
    while( DRM_SUCCEEDED( DRM_XML_GetSubNode( &dstrListCertChain, &g_dstrTagCertificate, NULL, NULL, iNode, &dstrCert, NULL, 1 ) ) )
    {
        iNode++;
    }
    if( iNode == 0 )
    {
        dr = DRM_E_LIC_INVALID_LICENSE;
        goto ErrorExit;
    }

    ChkDR( DRM_XML_GetSubNode( &dstrListCertChain, &g_dstrTagCertificate, NULL, NULL, iNode - 1, NULL, &dstrCert, 1 ) );
    iNode = SIZEOF( CERT );
    ChkDR(DRM_B64_DecodeW(&dstrCert, &iNode, (DRM_BYTE *)&(pcontextCrypto->union_cert.cert), 0 ) );
    MEMCPY( pbPubKey, &(pcontextCrypto->union_cert.cert.cd.pk), SIZEOF(PUBKEY) );

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        dr = DRM_E_LIC_INVALID_LICENSE;
    }

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetEnablingBits(
    __in                                     const DRM_CONST_STRING   *pdstrLicense,
    __in                                           DRM_DWORD           dwIndex,
    __out                                          DRM_DWORD          *pdwAlgorithm,
    __out                                          PUBKEY             *pPubKey,
    __out_bcount_opt( *pdValueLen )                DRM_BYTE           *pbValue,
    __out                                          DRM_DWORD          *pdValueLen,
    __out                                          PUBKEY             *pVerPubKey,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN )       DRM_BYTE           *pbSignature,
    __out_bcount_opt( CHECKSUM_LENGTH )            DRM_BYTE            rgbChainedChecksum[CHECKSUM_LENGTH],
    __in                                           DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrString              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrEnablingBitsSection = EMPTY_DRM_STRING;
    DRM_DWORD        dwSize                  = SIZEOF( PUBKEY );
    DRM_BOOL         fSymmetricallyBound     = FALSE;
    DRM_BOOL         fLeafLicense            = FALSE;


    if ( pPubKey     == NULL
      || pdValueLen  == NULL
      || pbSignature == NULL )
    {
        dr = DRM_E_LIC_PARAM_NOT_OPTIONAL;
        goto ErrorExit;
    }

    dr = DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrDrmRestoreInfoEnablingBits, NULL, NULL, &dstrEnablingBitsSection, NULL, g_wchForwardSlash );
    if( DRM_FAILED( dr )
     && dwIndex == 0 )
    {
        /* There are no restore enabling bits.  So 0 index is the first regular enabling bits */
        dwIndex++;
    }

    if( dwIndex > 0 )
    {
        ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLIData, NULL, NULL, &dstrString, NULL, g_wchForwardSlash ) );
        dr = DRM_XML_GetSubNode( &dstrString, &g_dstrTagEnablingbits, NULL, NULL, dwIndex - 1, &dstrEnablingBitsSection, NULL, 1);
        if( DRM_FAILED( dr ) )
        {
            /* These was no ENABLINGBITS section.  This could be a chained license.  Look for CHAINEDENABLINGBITS */
            ChkDR( DRM_XML_GetSubNode( &dstrString, &g_dstrChainedEnablingBits, NULL, NULL, dwIndex - 1, &dstrEnablingBitsSection, NULL, 1) );

            fLeafLicense = TRUE;

            if( NULL != rgbChainedChecksum )
            {
                /* Get Chained Checksum */
                ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBitsSection, &g_dstrChainedCheckSum, NULL, NULL, NULL, &dstrString, g_wchForwardSlash ) );
                /* Decode PubKey */
                dwSize = CHECKSUM_LENGTH;
                ChkDR(DRM_B64_DecodeW(&dstrString, &dwSize, rgbChainedChecksum, 0) );
            }

        }
        else
        {
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBitsSection, &g_dstrTagPubkey, NULL, NULL, NULL, &dstrString, g_wchForwardSlash ) );
            /* Decode PubKey */
            dwSize = SIZEOF( PUBKEY );
            ChkDR( DRM_B64_DecodeW(&dstrString, &dwSize, (DRM_BYTE *)pPubKey, 0) );
        }

        /* Get PubKey     */
        if( DRM_SYMOPT_IsSymOptSupported()
            && !fLeafLicense
            && DRM_SUCCEEDED( DRM_XML_GetSubNode(  pdstrLicense,
                                                  &g_dstrTagSymValue,
                                                   NULL,
                                                   NULL,
                                                   0,
                                                  &dstrString,
                                                   NULL,
                                                   2 ) ) )
        {
            /*
            ** This root or simple license has been rebound to a symmetric key.
            ** Ensure that we return the correct values in this case.
            */
            fSymmetricallyBound = TRUE;
        }

        if ( NULL != pVerPubKey )
        {
            ChkDR( DRM_WMDRM_LIC_GetLSPubKey( pdstrLicense, pVerPubKey, pcontextCrypto ) );
        }
    }
    else if( NULL != pVerPubKey )
    {
        /* Get PubKey     */
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBitsSection, &g_dstrTagPubkey, NULL, NULL, NULL, &dstrString, g_wchForwardSlash ) );
        /* Decode PubKey */
        dwSize = SIZEOF( PUBKEY );
        ChkDR(DRM_B64_DecodeW(&dstrString, &dwSize, (DRM_BYTE *)pPubKey, 0) );
        MEMCPY(pVerPubKey, &g_BrLicSrvPubl, SIZEOF(PUBKEY) );
    }

    if (NULL != pdwAlgorithm )
    {
        if( fSymmetricallyBound )
        {
            *pdwAlgorithm = eMSDRM_SYM;
        }
        else
        {
            DRM_CONST_STRING dstrAlgo = EMPTY_DRM_STRING;

            *pdwAlgorithm = 0;

            ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBitsSection, &g_dstrTagHashAlgorithm, NULL, NULL, &dstrString, NULL, g_wchForwardSlash ) );
            ChkDR( DRM_XML_GetNodeAttribute( &dstrString, &g_dstrAttributeType, &dstrAlgo ) );
            if( DRM_UTL_DSTRStringsEqual( &dstrAlgo, &g_dstrMSDRM ) )
            {
                *pdwAlgorithm = eMSDRM_PK;
                /* Is is a eMSDRM_PK algorithm */
            }
            else if( DRM_UTL_DSTRStringsEqual( &dstrAlgo, &g_dstrMSDRM_CK ) )
            {
                *pdwAlgorithm = eMSDRM_CK;
                /* It is a eMSDRM_CK algorithm */
            }
            else
            {
                ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
            }
        }
    }

    /* Get Value     */
    if( fSymmetricallyBound )
    {
        ChkDR( DRM_XML_GetSubNode( pdstrLicense, &g_dstrTagSymValue, NULL, NULL, 0, NULL, &dstrString, 2) );
    }
    else
    {
        ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBitsSection, &g_dstrTagValue, NULL, NULL, NULL, &dstrString, g_wchForwardSlash ) );
    }

    /* Base64 Decode Value */
    ChkDR(DRM_B64_DecodeW(&dstrString, pdValueLen, pbValue, 0 ) );

    /* Get Signature     */
    ChkDR( DRM_XML_GetSubNodeByPath( &dstrEnablingBitsSection, &g_dstrTagSignature, NULL, NULL, NULL, &dstrString, g_wchForwardSlash ) );

    /* Decode Signature */
    dwSize = DRM_ECC160_SIGNATURE_LEN;
    ChkDR(DRM_B64_DecodeW(&dstrString, &dwSize, pbSignature, 0) );

    dr = DRM_SUCCESS;
ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_VerifySignature(
    __in const DRM_CONST_STRING *pdstrLicense,
    __in       DRM_BB_CONTEXT   *pcontextBBX,
    __out      DRM_LONG         *plResult )
{
    DRM_RESULT         dr                = DRM_SUCCESS;
    DRM_CONST_STRING   dstrLIData        = EMPTY_DRM_STRING;
    DRM_CONST_STRING   dstrSignature     = EMPTY_DRM_STRING;
    DRM_CONST_STRING   dstrLID           = EMPTY_DRM_STRING;
    DRM_CONST_STRING   dstrBRInfo        = EMPTY_DRM_STRING;
    DRM_DWORD          dwSize            = DRM_ECC160_SIGNATURE_LEN;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICPARSER, PERF_FUNC_DRM_WMDRM_LIC_VerifySignature);

    ChkArg ( plResult     != NULL
          && pdstrLicense != NULL
          && pcontextBBX  != NULL );

    *plResult = 0; /* Init. */

    ChkDR( DRM_WMDRM_LIC_GetLSPubKey(pdstrLicense, &pcontextBBX->CryptoContext.pubKey, &pcontextBBX->CryptoContext ) );

    /* Get DATA section     */
    ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLIData, NULL, NULL, &dstrLIData, NULL, g_wchForwardSlash ) );

    ChkDR( DRM_SYMOPT_LIC_VerifySymmetricSignatureIfPresent(
        pdstrLicense,
        pcontextBBX,
        &dstrLIData,
        &dstrSignature,
        plResult ) );
    if( *plResult != 0 )
    {
        goto ErrorExit;
    }

    /* Get the LID */
    ChkDR( DRM_XML_GetSubNodeByPath( &dstrLIData, &g_dstrTagLID, NULL, NULL, &dstrLID, NULL, g_wchForwardSlash ) );

    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLISigHashAlgo, &g_dstrAttributeType, &g_dstrSHA, &dstrSignature, NULL, g_wchForwardSlash ) ) )
    {
        dr = DRM_E_LIC_UNSUPPORTED_VALUE;
        goto ErrorExit;
    }

    if( DRM_FAILED( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLISigAlgo, &g_dstrAttributeType, &g_dstrMSDRM, &dstrSignature, NULL, g_wchForwardSlash ) ) )
    {
        dr = DRM_E_LIC_UNSUPPORTED_VALUE;
        goto ErrorExit;
    }

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLISigValue, NULL, NULL, NULL, &dstrSignature, g_wchForwardSlash ) );
    dwSize = DRM_ECC160_SIGNATURE_LEN;
    ChkDR(DRM_B64_DecodeW(&dstrSignature, &dwSize, pcontextBBX->CryptoContext.signature, 0 ) );

    if ( DRM_SUCCEEDED( DRM_PK_Verify( pcontextBBX->CryptoContext.rgbCryptoContext,
                                       eDRM_ECC_P160,
                                       DRM_ECC160_PUBLIC_KEY_LEN,
                                       ( const DRM_BYTE * )&pcontextBBX->CryptoContext.pubKey,
                                       CB_DSTR(&dstrLIData),
                                       PB_DSTR(&dstrLIData),
                                       DRM_ECC160_SIGNATURE_LEN,
                                       pcontextBBX->CryptoContext.signature) ) )
    {
        if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrDrmRestoreInfo, NULL, NULL, &dstrBRInfo, NULL, g_wchForwardSlash ) ) )
        {
            /* The DRMRESTOREINFO section exists so we need to validate that             */


            ChkDR( DRM_XML_GetSubNodeByPath( &dstrBRInfo, &g_dstrTagSignature, NULL, NULL, &dstrLIData, NULL, g_wchForwardSlash ) );

            /*
            ** Get the SHA hashalgorithm and MSDRM signalgorithm.
            ** Don't do anything with it just make sure it is there
            */
            ChkDR( DRM_XML_GetSubNode( &dstrLIData, &g_dstrTagHashAlg, &g_dstrAttributeType, &g_dstrSHA,   0, &dstrSignature, NULL, 1 ) );
            ChkDR( DRM_XML_GetSubNode( &dstrLIData, &g_dstrTagSignAlg, &g_dstrAttributeType, &g_dstrMSDRM, 0, &dstrSignature, NULL, 1 ) );

            ChkDR( DRM_XML_GetSubNodeByPath( &dstrBRInfo, &g_dstrXPathSigValue, NULL, NULL, NULL,       &dstrSignature, g_wchForwardSlash ) );
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrBRInfo, &g_dstrTagData,       NULL, NULL, &dstrLIData,   NULL,          g_wchForwardSlash ) );

            /* Get the LID that is in the DRMRESTOREINFO/DATA section */
            ChkDR( DRM_XML_GetSubNodeByPath( &dstrLIData,   &g_dstrTagLID,        NULL, NULL, &dstrBRInfo, NULL,          g_wchForwardSlash ) );

            /* Compare with the original LID to prevent a spoofing attack */
            if( !DRM_UTL_DSTRStringsEqual( &dstrBRInfo, &dstrLID ) )
            {
                /* The LID from the original license and the LID in the B/R info don't match */
                /* Someone isn't playing nice.  We fail the signature check because something is fishy. */
                dr = DRM_E_LIC_INVALID_LICENSE;
                goto ErrorExit;
            }
            dwSize = DRM_ECC160_SIGNATURE_LEN;
            ChkDR(DRM_B64_DecodeW(&dstrSignature, &dwSize, pcontextBBX->CryptoContext.signature, 0) );

            if( dstrLIData.cchString > ((DRM_DWORD)(-1)) ||
                dstrLIData.cchString * SIZEOF( DRM_WCHAR ) > ((DRM_DWORD)(-1)) )
            {
                dr = DRM_E_ARITHMETIC_OVERFLOW;
                goto ErrorExit;
            }

            if ( DRM_SUCCEEDED( DRM_PK_Verify( pcontextBBX->CryptoContext.rgbCryptoContext,
                                               eDRM_ECC_P160,
                                               DRM_ECC160_PUBLIC_KEY_LEN,
                                               ( const DRM_BYTE * )&g_BrLicSrvPubl,
                                               CB_DSTR(&dstrLIData),
                                               PB_DSTR(&dstrLIData),
                                               DRM_ECC160_SIGNATURE_LEN,
                                               pcontextBBX->CryptoContext.signature) ) )
            {
                *plResult = 1;
            }

        }
        else
        {
            *plResult = 1;
        }
    }
    dr = DRM_SUCCESS;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
