/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>
#include <drmcontentrevocation.h>
#include <drmcontentrevocationconstants.h>
#include <drmxmlparser.h>
#include <drmwmdrmconstants.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation(
    IN const DRM_CONST_STRING   *pdstrContentRevocation,
    OUT      DRM_CONST_STRING   *pdstrLSPubKey,
    OUT      DRM_DWORD          *pdwSequenceNumber,
    OUT      DRM_CONST_STRING   *pdstrContentOwnerPubKey,
    OUT      DRM_CONST_STRING   *pdstrCondition,
    IN       DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    DRM_RESULT       dr = DRM_E_LOGICERR;
    DRM_CONST_STRING dstrData          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrContentPubKey = EMPTY_DRM_STRING;
    DRM_DWORD        dwSize = 0;

    ChkArg( pcontextCrypto         != NULL
         && pdstrContentRevocation != NULL );

    /* Initialize output parameters. */
    if (NULL != pdwSequenceNumber)
    {
        *pdwSequenceNumber = 0;
    }
    if (NULL != pdstrContentOwnerPubKey)
    {
        INIT_DRM_STRING( *pdstrContentOwnerPubKey );
    }
    if (NULL != pdstrCondition)
    {
        INIT_DRM_STRING( *pdstrCondition );
    }

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrContentRevocation, &g_dstrTagData, NULL, NULL, &dstrData, NULL, g_wchForwardSlash ) );

    /* Get Sequence Number */
    if (NULL != pdwSequenceNumber)
    {
        DRM_CONST_STRING dstrSequenceNumber = EMPTY_DRM_STRING;
        dr = DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrSequenceNumber, NULL, NULL, NULL, &dstrSequenceNumber, g_wchForwardSlash );
        if( DRM_SUCCEEDED( dr ) )
        {
            ChkDR (DRMCRT_wcsntol (dstrSequenceNumber.pwszString,
                            dstrSequenceNumber.cchString,
               (DRM_LONG *) pdwSequenceNumber));
        }
    }

    /* Get Content Owner PubKey     */
    ChkDR( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrContentPubKey, NULL, NULL, NULL, &dstrContentPubKey, g_wchForwardSlash ) );
    if (NULL != pdstrContentOwnerPubKey)
    {
        ASSIGN_DRM_STRING( *pdstrContentOwnerPubKey, dstrContentPubKey );
    }
    dwSize = SIZEOF( PUBKEY );
    ChkDR(DRM_B64_DecodeW(&dstrContentPubKey, &dwSize, (DRM_BYTE *)&(pcontextCrypto->pubKey), 0 ) );

    /* Get License Server PubKey. It is optional. */
    if (NULL != pdstrLSPubKey)
    {
        dr = DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrLSPubKey, NULL, NULL, NULL, pdstrLSPubKey, g_wchForwardSlash );
        if( DRM_FAILED( dr ) )
        {
            INIT_DRM_STRING( *pdstrLSPubKey );
        }
    }

    /* Get the Condition */
    if (NULL != pdstrCondition)
    {
        if( DRM_FAILED( DRM_XML_GetSubNodeByPath( &dstrData, &g_dstrCondition, NULL, NULL, NULL, pdstrCondition, g_wchForwardSlash) ) )
        {
            INIT_DRM_STRING( *pdstrCondition );
        }
    }

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrContentRevocation, &g_dstrTagSignature, NULL, NULL, &dstrContentPubKey, NULL, g_wchForwardSlash ) );

    /* Get the SHA hashalgorithm.  Don't do anything with it just make sure it is there */
    ChkDR( DRM_XML_GetSubNode( &dstrContentPubKey, &g_dstrTagHashAlg, &g_dstrAttributeType, &g_dstrSHA, NULL, &dstrData, NULL, 1 ) );

    /* Get the MSDRM signalgorithm.  Don't do anything with it just make sure it is there */
    ChkDR( DRM_XML_GetSubNode( &dstrContentPubKey, &g_dstrTagSignAlg, &g_dstrAttributeType, &g_dstrMSDRM, NULL, &dstrData, NULL, 1 ) );

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrContentRevocation, &g_dstrXPathSigValue, NULL, NULL, NULL, &dstrData, g_wchForwardSlash ) );
    dwSize = DRM_ECC160_SIGNATURE_LEN;
    ChkDR(DRM_B64_DecodeW(&dstrData, &dwSize, pcontextCrypto->signature, 0) );

    ChkDR( DRM_XML_GetSubNodeByPath( pdstrContentRevocation, &g_dstrTagData, NULL, NULL, &dstrData, NULL, g_wchForwardSlash ) );

    if( dstrData.cchString > ((DRM_DWORD)(-1)) ||
        dstrData.cchString * SIZEOF( DRM_WCHAR ) > ((DRM_DWORD)(-1)) )
    {
        ChkDR( DRM_E_ARITHMETIC_OVERFLOW );
    }

    /* Use PkCrypto to verify the signature of the DATA section        */
    ChkDR( DRM_PK_Verify( pcontextCrypto->rgbCryptoContext,
                          eDRM_ECC_P160,
                          DRM_ECC160_PUBLIC_KEY_LEN,
                          ( const DRM_BYTE * )&pcontextCrypto->pubKey,
                          CB_DSTR( &dstrData ),
                          PB_DSTR( &dstrData ),
                          DRM_ECC160_SIGNATURE_LEN,
                          pcontextCrypto->signature) );

    /* Everything looks good.  */
    dr = DRM_SUCCESS;
ErrorExit:
    return ( dr );
}


DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LIC_GetContentRevocation(
    IN const DRM_CONST_STRING   *pdstrLicense,
    IN       DRM_DWORD           dwSequenceNumber,
    OUT      DRM_DWORD          *pdwSequenceNumber,
    OUT      DRM_CONST_STRING   *pdstrContentRevocation,
    OUT      DRM_CONST_STRING   *pdstrContentOwnerPubKey,
    OUT      DRM_CONST_STRING   *pdstrCondition,
    IN       DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrSectionData = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLSPubKey    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTemp        = EMPTY_DRM_STRING;

    ChkArg( pdstrLicense            != NULL
         && pdstrContentRevocation  != NULL
         && pdstrContentOwnerPubKey != NULL );

    if( DRM_FAILED ( DRM_XML_GetSubNodeByPath( pdstrLicense, &g_dstrLIData, NULL, NULL, &dstrSectionData, NULL, g_wchForwardSlash ) ) )
    {
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    if( DRM_FAILED ( DRM_XML_GetSubNode( &dstrSectionData, &g_dstrContentRevocation, NULL, NULL, dwSequenceNumber, pdstrContentRevocation, &dstrTemp, 1 ) ) )
    {
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    /* Verify and set output parameters. */
    ChkDR( DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation( pdstrContentRevocation, &dstrLSPubKey, pdwSequenceNumber, pdstrContentOwnerPubKey, pdstrCondition, pcontextCrypto) );

    /* If license server pubkey is in the content revocation list, make sure that it corresponds to that of this license. */
    /* If the license server pubkey exists and is non-empty string, make sure it matched the one in license. */
    if ( NULL != dstrLSPubKey.pwszString )
    {
        DRM_DWORD dwSize = SIZEOF( PUBKEY );

        /* Retrieve pubkey of license server for this license. */
        ChkDR( DRM_WMDRM_LIC_GetLSPubKey( pdstrLicense, &(pcontextCrypto->pubKey), pcontextCrypto ) );
        ChkDR(DRM_B64_DecodeW(&dstrLSPubKey, &dwSize, (DRM_BYTE*) &(pcontextCrypto->pubKey2), 0));
        if (MEMCMP((DRM_BYTE*) &(pcontextCrypto->pubKey), (DRM_BYTE*) &(pcontextCrypto->pubKey2), SIZEOF( PUBKEY ) ) != 0 )
        {
            ChkDR( DRM_E_KEY_MISMATCH );
        }
    }
    dr = DRM_SUCCESS;
ErrorExit:
    return(dr);
}

EXIT_PK_NAMESPACE_CODE;

