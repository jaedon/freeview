/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMLICACQV3_C
#include <drmtypes.h>
#include <drmdatastore.h>
#include <drmutilities.h>
#include <drmxmlparser.h>
#include <drmxmlbuildera.h>
#include <drmheaderparser.h>
#include <drmcrt.h>
#include <oemaescommon.h>
#include <oemaes.h>
#include <drmpubkeydeviceexclusion.h>
#include <drmsoapxmlconstants.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <drmsoapxmlutility.h>
#include <drmlicacqv3constants.h>
#include <drmlicacqv3.h>
#include <drmmanager.h>
#include <drmcontract.h>
#include <drmutf.h>
#include <oem.h>
#include <drmsymopt.h>
#include <drmbcertparser.h>
#include <drmbcrl.h>
#include <drmdomainstore.h>
#include <drmrevocation.h>
#include <drmapppolicy.h>
#include <drmremovaldatestore.h>
#include <drmlicreason.h>
#include <drmsyncstore.h>
#include <drmdomainapi.h>
#include <drmversionconstants.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/*********************************************************************
**
** Function: _DRMResultToANSIString
**
** Synopsis: Function that converts a DRM result into a string with the
**           format "0X08X" ( e.g. 0X00123456 )
**
** Arguments:
**
** [f_dwDR]                 -- The DRM result to be converted.
** [f_pchString]            -- Pointer to a variable that stored the size
**                             ( number of characters ) of the buffer during
**                             input, and receives the actual size of the
**                             result string during output.
** [f_pcchString]           -- Pointer to a buffer that receives the
**                             generated string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of the buffer is
**                          not big enough.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _DRMResultToANSIString(
    __in                        DRM_RESULT f_dwDR,
    __out_bcount(*f_pcchString) DRM_CHAR  *f_pchString,
    __inout                     DRM_DWORD *f_pcchString )
{
    DRM_RESULT dr = DRM_SUCCESS;
    /*
    ** Need to have space to hold 8 hex digits, 2 prefix characters.
    */
    DRM_DWORD cHexDigits = DRM_MAX_CCH_BASE16_DWORD_STRING + 2;
    DRM_DWORD dwValue = ( DRM_DWORD )f_dwDR;
    DRM_DWORD i = 0;
    DRM_DWORD dwTmp = 0;

    ChkArg( f_pchString != NULL );
    ChkArg( f_pcchString != NULL );

    /* Two for "0X", one for the trailing '\0'. */
    if ( *f_pcchString < cHexDigits )
    {
        *f_pcchString = cHexDigits;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    PUT_CHAR( f_pchString, 0, '0' );
    PUT_CHAR( f_pchString, 1, 'X' );

    for ( i = 8; i > 0; i-- )
    {
        dwTmp = dwValue % 16;

        if ( dwTmp >= 10 )
        {
            dwTmp = ( dwTmp - 10 ) + 'A';
        }
        else
        {
            dwTmp += '0';
        }

        PUT_CHAR( f_pchString, i + 1, ( DRM_BYTE )dwTmp );

        dwValue /= 16;
    }

    *f_pcchString = cHexDigits;

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: _GetMaxChallengeTempBufferCount
**
** Synopsis: Function that calculates the maximum count ( number of
**           UTF8/ANSI or Unicode characters ) of the temporary buffer
**           during the construction of the license acquisition challenge
**           XML.
**
**           Temporary buffer is used to:
**           Convert between Unicode strings and UTF8/ANSI strings.
**           B64 encode/decode data.
**
** Arguments:
**
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
**
** Returns:                 A long integer value that is the character
**                          count of the calculated maximum temporary
**                          buffer.
**
**********************************************************************/
static DRM_DWORD _GetMaxChallengeTempBufferCount(
    __in DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchMaxCount = DRM_MAX_CCH_BASE10_DWORD_STRING;
    DRM_DWORD  cchCount = 0;
    DRM_DWORD  i = 0;
    DRM_CHAR   pbTemp;

    ChkArg( f_poLicChlgContext != NULL );

    /*
    ** Consider the content header since it needs to be converted from
    ** Unicode to UTF8/ANSI.  On input, cchCount = 0 so the conversion
    ** routine will calculate the converted size but not actually convert
    *  the string.  On output, cchCount will have the size required to hold
    ** the converted string.
    */
    dr = DRM_STR_DSTRtoUTF8( (const DRM_STRING*) &f_poLicChlgContext->m_dstrContentHeader,
                                                 &pbTemp,
                                                 &cchCount );
    if ( DRM_SUCCEEDED( dr ) || dr == DRM_E_BUFFERTOOSMALL )
    {
        cchMaxCount = max( cchMaxCount, cchCount );
    }

    /*
    ** Consider all right ( action ) strings since they need to be converted
    ** from Unicode to UTF8/ANSI.
    */
    for ( i = 0; i < f_poLicChlgContext->m_cRights; i++ )
    {
        cchCount = f_poLicChlgContext->m_ppdstrRights[ i ]->cchString;

        cchMaxCount = max( cchMaxCount, cchCount );
    }

    /*
    ** Consider the playready client version
    */
    cchCount = g_dstrReqTagPlayReadyClientVersionData.cchString;

    cchMaxCount = max( cchMaxCount, cchCount );

    /* Consider the B-64 encoded size of the raw license state data. */
    cchCount = CCH_BASE64_EQUIV( SIZEOF( DRM_LICENSE_STATE_DATA ) );

    cchMaxCount = max( cchMaxCount, cchCount );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /*
        ** Consider the B64 encoded size of the revocation type ID ( GUID )
        ** and possibly other nodes that use GUID.
        */
        cchCount = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

        cchMaxCount = max( cchMaxCount, cchCount );
    }

ErrorExit:

    return cchMaxCount;
}

/*********************************************************************
**
** Function: _GetMaxAcknowledgementTempBufferCount
**
** Synopsis: Function that calculates the maximum count ( number of
**           UTF8/ANSI or Unicode characters ) of the temporary buffer
**           during the construction of the license acquisition
**           acknowledgement XML.
**
**           Temporary buffer is used to:
**           Convert between Unicode strings and UTF8/ANSI strings.
**           B64 encode/decode data.
**
** Arguments:
**
**                          N/A
**
** Returns:                 A long integer value that is the character
**                          count of the calculated maximum temporary
**                          buffer.
**
**********************************************************************/
static DRM_DWORD _GetMaxAcknowledgementTempBufferCount( DRM_VOID )
{
    DRM_DWORD cchMaxCount = DRM_MAX_CCH_BASE10_DWORD_STRING;
    DRM_DWORD cchCount = 0;

    /*
    ** Consider the B64 encoded size of the KID ( GUID ) in each license
    ** processing result.
    */
    cchCount = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

    cchMaxCount = max( cchMaxCount, cchCount );

    /*
    ** Consider the playready client version
    */
    cchCount = g_dstrReqTagPlayReadyClientVersionData.cchString;

    cchMaxCount = max( cchMaxCount, cchCount );

    return cchMaxCount;
}

/*********************************************************************
**
** Function: _CalcRevListInfoCharCount
**
** Synopsis: Function that calculates the character count of the
**           revocation version infomation XML segment in the license
**           acquisition challenge XML.
**
** Arguments:
**
** [f_poRevListInfo]        -- An array of structures that contain the
**                             revocation version information.
** [f_cRevListInfo]         -- Total number of elements in the revocation
**                             version information array mentioned above.
**
** Returns:                 A long integer value that is the character
**                          count of the revocation version information
**                          XML segment.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcRevListInfoCharCount(
    __in  _REVLIST_INFO *f_poRevListInfo,
    __in  DRM_DWORD f_cRevListInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD i = 0;
    DRM_GUID oEmptyGUID = EMPTY_DRM_GUID;

    ChkArg( f_poRevListInfo != NULL );
    ChkArg( f_cRevListInfo > 0 );

    /* <RevocationLists> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqRevocationListsTag.cchString,
                                              0,
                                              0,
                                              0 );

    for ( i = 0; i < f_cRevListInfo; i++ )
    {
        if ( f_poRevListInfo[ i ].m_poGUID == NULL
          || MEMCMP( f_poRevListInfo[ i ].m_poGUID, &oEmptyGUID, SIZEOF( DRM_GUID ) ) == 0 )
        {
            continue;
        }

        if ( f_poRevListInfo[ i ].m_dwVersion == DRM_APP_REVOCATION_VERSION_NONE )
        {
            // Convert legacy "Empty" version number to the PlayReady style.
            f_poRevListInfo[ i ].m_dwVersion = 0;
        }

        /* <RevListInfo> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqRevListInfoTag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        /* <ListID> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqRevListIDTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                                  0,
                                                  0 );

        /* <Version> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqRevVersionTag.cchString,
                                                  DRM_SOAPXML_CalcDWORDCharCount( f_poRevListInfo[ i ].m_dwVersion ),
                                                  0,
                                                  0 );
    }

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _CalcDeviceCertCharCount
**
** Synopsis: Function that calculates the character count of a
**           device certificate.
**
** Arguments:
**
** [f_poKeyFileContext]     -- Pointer to a key file context.
**
** Returns:                 A long integer value that is the character
**                          count of the device certificate.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcDeviceCertCharCount(
    __in DRM_KEYFILE_CONTEXT *f_poKeyFileContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__CalcDeviceCertCharCount );

    /* Intentionally pass in a NULL buffer to just retrieve the count. */
    ChkDR( DRM_SOAPXML_GetDeviceCert( f_poKeyFileContext, TRUE, NULL, &cchCount ) );

    /*
    ** Add one more DRM_BYTE to consider the possible realignment.
    */
    cchCount += SIZEOF( DRM_BYTE );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return cchCount;
}

/*********************************************************************
**
** Function: _CalcCertChainsCharCount
**
** Synopsis: Function that calculates the character count of the whole
**           certificate chains including device certificate and
**           a collection of domain certificates.
**
** Arguments:
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
**
** Returns:                 A long integer value that is the character
**                          count of the whole certificate chains.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcCertChainsCharCount(
    __in DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD cchDeviceCert = 0;

    ChkArg( f_poLicChlgContext != NULL );

    /* <CertificateChains> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqCertificateChainsTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <CertificateChain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqCertificateChainTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* Add the character count of the device certificate. */
    cchDeviceCert = _CalcDeviceCertCharCount( f_poLicChlgContext->m_poBBXContext->pKeyFileContext );

    /* Save the character count of the device certificate for future use. */
    f_poLicChlgContext->m_cchDeviceCert = cchDeviceCert;

    cchCount += cchDeviceCert;

    if( f_poLicChlgContext->m_poDomainStoreContext != NULL )
    {
        DRM_DWORD cchDomainCerts = 0;
        /* Add the character count of possible domain certificates. */
        ChkDR( DRM_DOM_LA_CalcDomainCertsCharCount( f_poLicChlgContext->m_poDomainStoreContext,
                                                   &f_poLicChlgContext->m_oServiceID,
                                                   &cchDomainCerts ) );

        /* Save the character count of the domain certificates for future use. */
        f_poLicChlgContext->m_cchDomainCerts = cchDomainCerts;

        cchCount += cchDomainCerts;
    }

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _CalcLicenseChallengeDataCharCount
**
** Synopsis: Function that calculates the character count of the data
**           portion XML segment of the license acquisition challenge
**           XML. The data portion may be encrypted by XML encryption.
**
** Arguments:
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
**
** Returns:                 A long integer value that is the character
**                          count of the data portion XML segment.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcLicenseChallengeDataCharCount(
    __in DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    ChkArg( f_poLicChlgContext != NULL );

    /* <Data> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqDataTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* Add the count of the certificate chains. */
    cchCount += _CalcCertChainsCharCount( f_poLicChlgContext );

    /* Add space used by XML builder context. */
    cchCount += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realighnment.
    */
    cchCount += SIZEOF( DRM_DWORD );

    /*
    ** Reserve space in order to pad the result XML string to
    ** the length of multiples of DRM_AES_BLOCKLEN (16).
    */
    cchCount += DRM_AES_BLOCKLEN;

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _CalcLicenseAcknowledgementDataCharCount
**
** Synopsis: Function that calculates the character count of the data
**           portion XML segment of the license acquisition acknowledgement
**           XML. The data portion may be encrypted by XML encryption.
**
** Arguments:
** [f_cchDeviceCert]        -- Size ( number of characters ) of a device
**                             certificate.
**
** Returns:                 A long integer value that is the character
**                          count of the data portion XML segment.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcLicenseAcknowledgementDataCharCount(
    __in DRM_DWORD f_cchDeviceCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    ChkArg( f_cchDeviceCert > 0 );

    /* <Data> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckDataTag.cchString,
                                              f_cchDeviceCert,
                                              0,
                                              0 );

    /* <CertificateChain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckCertificateChainTag.cchString,
                                              f_cchDeviceCert,
                                              0,
                                              0 );

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _CalcLicenseChallengeCharCount
**
** Synopsis: Function that calculates the character count of a license
**           acquisition challenge XML.
**
** Arguments:
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build a license
**                             acquisition challenge XML.
**
** Returns:                 A long integer value that is the character
**                          count of the license acquisition challenge
**                          XML.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcLicenseChallengeCharCount(
    __in DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD cchSignatureXMLCount = 0;
    DRM_DWORD cchEncryptedCount = 0;
    DRM_DWORD cbEncryptedSize = 0;
    DRM_DWORD cchCustomData = 0;
    DRM_GUID  oEmptyGUID = EMPTY_DRM_GUID;
    DRM_DWORD cchHeader = 0;
    DRM_CHAR  pbHeader;

    ChkArg( f_poLicChlgContext != NULL );
    ChkArg( f_poLicChlgContext->m_poBBXContext != NULL );

#if DRM_USE_SOAP

    /* Add the character count of the XML root header ( <?xml...> ). */
    cchCount += g_dastrXMLRootTag.cchString;

    /* Add the character count of the SOAP header. */
    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount( );

#endif

    /* <AcquireLicense> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqRootTag.cchString,
                                              0,
                                              g_dastrLicAcqRootAttribName.cchString,
                                              g_dastrLicAcqRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqChallenge2Tag.cchString,
                                              0,
                                              g_dastrLicAcqChallenge2AttribName.cchString,
                                              g_dastrLicAcqChallenge2AttribValue.cchString );
    /* <LA> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqLATag.cchString,
                                              0,
                                              g_dastrLicAcqChallengeVAAttrib1Name.cchString,
                                              g_dastrLicAcqChallengeVAAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA(
                                              g_dastrLicAcqChallengeVAAttrib2Name.cchString,
                                              g_dastrLicAcqChallengeVAAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA(
                                              g_dastrSOAPPreserveSpaceAttribName.cchString,
                                              g_dastrSOAPPreserveSpaceAttribValue.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqVersionTag.cchString,
                                              g_dastrLicAcqVersionValue.cchString,
                                              0,
                                              0 );

    /* <ContentHeader> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqContentHeaderTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* Add the character count of the content header.
    **
    ** Calculate the size to convert the string from Unicode to UTF8/ANSI.
    ** On input, cchHeader = 0 so the conversion routine will calculate the
    ** converted size but not actually convert the string.  On output, cchHeader
    ** will have the size required to hold the converted string.
    */
    dr = DRM_STR_DSTRtoUTF8( (const DRM_STRING*) &f_poLicChlgContext->m_dstrContentHeader,
                                                 &pbHeader,
                                                 &cchHeader );
    if ( DRM_SUCCEEDED( dr ) || dr == DRM_E_BUFFERTOOSMALL )
    {
        cchCount += cchHeader;
    }

    /* Add the character count of client information XML segment. */
    cchCount += DRM_SOAPXML_CalcClientInfo(f_poLicChlgContext->m_poBBXContext->pClientInfo);

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /* Add the character count of revocation informartion XML segment. */
        cchCount += _CalcRevListInfoCharCount( f_poLicChlgContext->m_rgoRevListInfo,
                                               MAX_REVLIST_INFO );
    }

    if ( f_poLicChlgContext->m_dastrCustomData.cchString > 0 )
    {
        /* <CustomData> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqCustomDataTag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        /* Cet the size  (number of characters) of the XML encoded custom data. */
        ChkDR( DRM_SOAPXML_EncodeData( ( DRM_CHAR * )f_poLicChlgContext->m_dastrCustomData.pszString,
                                       f_poLicChlgContext->m_dastrCustomData.cchString,
                                       NULL,
                                       &cchCustomData ) );

        /* Add the character count of the custom data. */
        cchCount += cchCustomData;

        /* Possible alignment cost. */
        cchCount += SIZEOF( DRM_WORD );
    }

    if ( MEMCMP( &f_poLicChlgContext->m_oNonce, &oEmptyGUID, SIZEOF( DRM_GUID ) ) != 0 )
    {
        /* <LicenseNonce> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqLicenseNonceTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ),
                                                  0,
                                                  0 );
    }

    /*
    ** Calculate the character count of the data portion of the license acquisition
    ** challenge XML that might be XML encrypted.
    */
    cchEncryptedCount += _CalcLicenseChallengeDataCharCount( f_poLicChlgContext );

    cbEncryptedSize = cchEncryptedCount * SIZEOF( DRM_CHAR );

    /*
    ** Add the character counte of the encrypted data portion of the license
    ** acquisition challenge XML.
    */
    ChkDR( DRM_XMLSIG_GetEncryptedDataNodeCharCount( &f_poLicChlgContext->m_oXMLKey,
                                                     cbEncryptedSize,
                                                     &cchEncryptedCount ) );

    cchCount += cchEncryptedCount;

    /* Add the character count of the signature. */
    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchSignatureXMLCount ) );

    cchCount += cchSignatureXMLCount;

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _CalcLicenseAcknowledgementCharCount
**
** Synopsis: Function that calculates the character count of a license
**           acquisition acknowledgement XML.
**
** Arguments:
** [f_cchTransactionID]     -- Size (number of characters) of a transaction ID.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poLicAcks]            -- An array of license acknowlegement structures
**                             that store the results of license processing.
** [f_cLicAcks]             -- Total number of elements of the license
**                             acknowledgement structures in the array
**                             mentioned above.
** [f_cchDeviceCert]        -- Size ( number of characters ) of a device
**                             certificate.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure that
**                             contains the key information to encrypt
**                             portion of the challenge data.
**
** Returns:                 A long integer value that is the character
**                          count of the license acquisition acknowledgement
**                          XML.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcLicenseAcknowledgementCharCount(
    __in                              DRM_DWORD        f_cchTransactionID,
    __in                        const DRM_BB_CONTEXT  *f_poBBXContext,
    __in_ecount_opt(f_cLicAcks) const DRM_LICENSE_ACK *f_poLicAcks,
    __in                              DRM_DWORD        f_cLicAcks,
    __in                              DRM_DWORD        f_cchDeviceCert,
    __in                        const DRM_XML_KEY     *f_poXMLKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD cchEncryptedCount = 0;
    DRM_DWORD cbEncryptedSize = 0;
    DRM_DWORD cchSignatureXMLCount = 0;
    DRM_DWORD i = 0;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( ( f_cLicAcks > 0 && f_poLicAcks != NULL ) ||
            f_cLicAcks == 0 );
    ChkArg( f_cchDeviceCert > 0 );
    ChkArg( f_poXMLKey != NULL );

#if DRM_USE_SOAP

    /* Add the character count of the XML root header ( <?xml...> ). */
    cchCount += g_dastrXMLRootTag.cchString;

    /* Add the character count of the SOAP header. */
    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount( );

#endif

    /* <AcknowledgeLicense> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckRootTag.cchString,
                                              0,
                                              g_dastrLicAckRootAttribName.cchString,
                                              g_dastrLicAckRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckChallenge2Tag.cchString,
                                              0,
                                              g_dastrLicAckChallenge2AttribName.cchString,
                                              g_dastrLicAckChallenge2AttribValue.cchString );

    /* <Ack> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckAckTag.cchString,
                                              0,
                                              g_dastrLicAckAckAttrib1Name.cchString,
                                              g_dastrLicAckAckAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrLicAckAckAttrib2Name.cchString,
                                                    g_dastrLicAckAckAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrSOAPPreserveSpaceAttribName.cchString,
                                                    g_dastrSOAPPreserveSpaceAttribValue.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckVersionTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <TransactionID> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckTransactionIDTag.cchString,
                                              f_cchTransactionID,
                                              0,
                                              0 );

    /* <ClientInfo> */
    cchCount += DRM_SOAPXML_CalcClientInfo(f_poBBXContext->pClientInfo);

    /* <LicenseStorageResults> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckLicenseStorageResultsTag.cchString,
                                              0,
                                              0,
                                              0 );

    for ( i = 0; i < f_cLicAcks; i++ )
    {
        /* <License> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckLicenseTag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        /* <KID> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckKIDTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                                  0,
                                                  0 );

        /* <LID> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckLIDTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                                  0,
                                                  0 );

        /* <Result> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckResultTag.cchString,
                                                  DRM_SOAPXML_CalcDWORDCharCount( (DRM_DWORD)f_poLicAcks->m_dwResult ),
                                                  0,
                                                  0 );
    }

    /*
    ** Calculate the character count of the data portion of the license acquisition
    ** acknowledgement XML that might be XML encrypted.
    */
    cchEncryptedCount = _CalcLicenseAcknowledgementDataCharCount( f_cchDeviceCert );

    cbEncryptedSize = cchEncryptedCount * SIZEOF( DRM_CHAR );

    /*
    ** Add the character count of the encrypted data portion of the license
    ** acquisition acknowledgement XML.
    */
    ChkDR( DRM_XMLSIG_GetEncryptedDataNodeCharCount( f_poXMLKey,
                                                     cbEncryptedSize,
                                                     &cchEncryptedCount ) );

    cchCount += cchEncryptedCount;

    /* Add the character count of the signature XML segment. */
    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchSignatureXMLCount ) );

    cchCount += cchSignatureXMLCount;

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _BuildCertChainsXML
**
** Synopsis: Function that constructs a certificate chains XML segment
**           that contains both a device certificate and possible domain
**           certification( s ). The certificate chains XML segment
**           is part of the license acquisition challenge XML.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildCertChainsXML(
    __inout     _XMBContextA            *f_poXMLContext,
    __in        DRM_LICCHLG_CONTEXT_V3  *f_poLicChlgContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrReservedBuffer = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildCertChainsXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_poLicChlgContext != NULL );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    DRMASSERT( pchXML != NULL );

    /*
    ** Add and open <CertificateChains> node.
    **
    ** Output:
    ** <CertificateChains>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqCertificateChainsTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Only build the device certificate XML segment when the device certificate
    ** is not empty. *f_poLicChlgContext should contain the known character count
    ** of the device certificate.
    */
    if ( f_poLicChlgContext->m_cchDeviceCert > 0 )
    {
        /*
        ** Add and open <CertificateChain> node.
        **
        ** Output:
        ** <CertificateChains>
        **      ...
        **      <CertificateChain>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqCertificateChainTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContext,
                                      SIZEOF( DRM_WORD ),
                                      f_poLicChlgContext->m_cchDeviceCert,
                                      &dasstrReservedBuffer ) );

        /*
        ** The reserved the buffer should be big enough to store the whole device
        ** certificates. In case the buffer has more bytes needed by the device
        ** certificate, those bytes should be set to ' ' so the server side XML
        ** parser can just ignore them.
        */
        MEMSET( pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                ' ',
                dasstrReservedBuffer.m_cch );

        ChkDR( DRM_SOAPXML_GetDeviceCert( f_poLicChlgContext->m_poBBXContext->pKeyFileContext,
                                          TRUE,
                                          pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                                          &dasstrReservedBuffer.m_cch ) );

        /*
        ** Close <CertificateChain> node.
        **
        ** Output:
        ** <CertificateChains>
        **      <CertificateChain>
        **          device certificate
        **      </CertificateChain>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    /*
    ** Only build the domain certificate XML segment when there is one or more
    ** domain certificate( s ). *f_poLicChlgContext should contain the known character
    ** count of the domain certificate( s ).
    **
    ** Output:
    ** <CertificateChains>
    **      ...
    **      <CertificateChain>
    **          domain certificate 1
    **      </CertificateChain>
    **      <CertificateChain>
    **          domain certificate 2
    **      </CertificateChain>
    **      ...
    */
    if ( f_poLicChlgContext->m_cchDomainCerts > 0 )
    {
        DRMASSERT( DRM_DOMAIN_IsDomainSupported() ); /* m_cchDomainCerts should still be zero if domains are not supported */
        ChkArg( f_poLicChlgContext->m_poDomainStoreContext != NULL );
        ChkDR( DRM_DOM_LA_BuildDomainCertsXML( f_poLicChlgContext->m_poDomainStoreContext,
                                              &f_poLicChlgContext->m_oServiceID,
                                               f_poXMLContext ) );
    }

    /*
    ** Close <CertificateChains> node.
    **
    ** Output:
    ** <CertificateChains>
    **      ...
    ** </CertificateChains>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildRevListInfoXML
**
** Synopsis: Function that constructs a revocation version information
**           XML segment that is part of the license acquisition
**           challenge XML.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_poStackAllocatorContext]
**                          -- Pointer to a stack allocator context.
** [f_poRevListInfo]        -- An array of structure that contains the
**                             revocation version information.
** [f_cRevListInfo]         -- Total number of elements in the revocation
**                             version information array mentioned above.
** [f_cchBuffer]            -- Maxmimum size ( number of character count )
**                             of the temporary buffer uses in this
**                             function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_RESULT _BuildRevListInfoXML(
    __inout _XMBContextA *f_poXMLContext,
    __in     DRM_STACK_ALLOCATOR_CONTEXT *f_poStackAllocatorContext,
    __in     _REVLIST_INFO *f_poRevListInfo,
    __in     DRM_DWORD f_cRevListInfo,
    __in     DRM_DWORD f_cchBuffer )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR rgwchScratch[ DRM_MAX_CCH_BASE10_DWORD_STRING + 1 ] = { 0 };
    DRM_CHAR* pchScratch = NULL;
    DRM_DWORD cchScratch = 0;
    DRM_ANSI_STRING dastrScratch = EMPTY_DRM_STRING;
    DRM_DWORD i = 0;
    DRM_GUID oEmptyGUID = EMPTY_DRM_GUID;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildRevListInfoXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_poStackAllocatorContext != NULL );
    ChkArg( f_poRevListInfo != NULL );
    ChkArg( f_cRevListInfo > 0 );
    ChkArg( f_cchBuffer >= DRM_MAX_CCH_BASE10_DWORD_STRING );

    /* Allocate the temporary buffer. */
    ChkDR( DRM_STK_Alloc( f_poStackAllocatorContext, f_cchBuffer, ( DRM_VOID ** )&pchScratch ) );

    dastrScratch.pszString = pchScratch;

    /*
    ** Add and open <RevocationLists> node.
    **
    ** Output:
    ** <RevocationLists>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqRevocationListsTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    for ( i = 0; i < f_cRevListInfo; i++ )
    {
        if ( f_poRevListInfo[ i ].m_poGUID == NULL
          || MEMCMP( f_poRevListInfo[ i ].m_poGUID, &oEmptyGUID, SIZEOF( DRM_GUID ) ) == 0 )
        {
            continue;
        }

        if ( f_poRevListInfo[ i ].m_dwVersion == DRM_APP_REVOCATION_VERSION_NONE )
        {
            // Convert legacy "Empty" version number to the PlayReady style.
            f_poRevListInfo[ i ].m_dwVersion = 0;
        }

        /*
        ** Add and open <RevocationLists> node.
        **
        ** Output:
        ** <RevocationLists>
        **      ...
        **      <RevListInfo>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqRevListInfoTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        cchScratch = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

        ChkBOOL( cchScratch <= f_cchBuffer, DRM_E_INVALIDARG );

        DRMASSERT( f_poRevListInfo[ i ].m_poGUID != NULL );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )f_poRevListInfo[ i ].m_poGUID,
                                SIZEOF( DRM_GUID ),
                                pchScratch,
                                &cchScratch,
                                0 ) );

        dastrScratch.cchString = cchScratch;

        /*
        ** Add and close <ListID> node.
        **
        ** Output:
        ** <RevocationLists>
        **      ...
        **      <RevListInfo>
        **          <ListID>...</ListID>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqRevListIDTag,
                                  ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        ChkDR( DRM_UTL_NumberToString( f_poRevListInfo[ i ].m_dwVersion,
                                       rgwchScratch,
                                       DRM_MAX_CCH_BASE10_DWORD_STRING + 1,
                                       0, 10, &cchScratch ) );

        /* Safe to use, input parameter is ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( rgwchScratch, pchScratch, cchScratch );

        dastrScratch.cchString = cchScratch;

        /*
        ** Add and close <Version> node.
        **
        ** Output:
        ** <RevocationLists>
        **      ...
        **      <RevListInfo>
        **          ...
        **          <Version>...</Version>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqRevVersionTag,
                                  ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        /*
        ** Close <RevListInfo> node.
        **
        ** Output:
        ** <RevocationLists>
        **      ...
        **      <RevListInfo>
        **          ...
        **      </RevListInfo>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    /*
    ** Close <RevocationLists> node.
    **
    ** Output:
    ** <RevocationLists>
    **      ...
    ** <RevocationLists>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );

ErrorExit:

    /* Allocated the temporary buffer. */
    if ( f_poStackAllocatorContext != NULL && pchScratch != NULL )
    {
        DRM_STK_Free( f_poStackAllocatorContext, pchScratch );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildLicenseChallengeDataXML
**
** Synopsis: Function that constructs the data portion of a license
**           acquisition challenge XML. The data portion may be
**           encrypted by XML encryption.
**
** Arguments:
**
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildLicenseChallengeDataXML(
    __in  DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext,
    __out DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_BYTE *pbXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchBuffer = 0;
    DRM_CHAR* pchScratch = NULL;
    DRM_ANSI_STRING dastrScratch = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseChallengeDataXML );

    ChkArg( f_poLicChlgContext != NULL );
    ChkDRMANSIString( f_pdastrXML );

    /* Calculate the maximum character count needed by temporary buffer. */
    cchBuffer = _GetMaxChallengeTempBufferCount( f_poLicChlgContext );

    ChkBOOL( cchBuffer >= DRM_MAX_CCH_BASE10_DWORD_STRING, DRM_E_INVALIDARG );

    /* Allocate the temporary buffer. */
    ChkDR( DRM_STK_Alloc( &f_poLicChlgContext->m_oStackAllocatorContext,
                          cchBuffer,
                          ( DRM_VOID ** )&pchScratch ) );

    dastrScratch.pszString = pchScratch;

    pbXMLContext = PB_DASTR( f_pdastrXML );
    cbXMLContext = CB_DASTR( f_pdastrXML );

    ChkDR( DRM_UTL_EnsureDataAlignment( pbXMLContext,
                                        cbXMLContext,
                                        ( DRM_BYTE ** )&poXMLContext,
                                        &cbXMLContext,
                                        SIZEOF( DRM_DWORD ),
                                        NULL ) );

    /*
    ** Add and open <Data> node.
    **
    ** Output:
    ** <Data>
    */
    ChkDR( DRM_XMB_CreateDocumentA( cbXMLContext,
                                    ( DRM_BYTE * )poXMLContext,
                                    &g_dastrLicAcqDataTag ) );

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Construct the certificate chains XML segment.
    **
    ** Output:
    ** <Data>
    **      ...
    **      <CertificateChains>
    **          ...
    **      </CertificateChains>
    */
    ChkDR( _BuildCertChainsXML( poXMLContext, f_poLicChlgContext ) );

    /*
    ** Close <Data> node.
    **
    ** Output:
    ** <Data>
    **      ...
    ** </Data>
    */
    ChkDR( DRM_XMB_CloseDocumentA( poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

    ChkDR( DRM_SOAPXML_PadANSIString( f_pdastrXML, DRM_AES_BLOCKLEN ) );

    DRM_BYT_MoveBytes( pbXMLContext, 0, pchXML, 0, f_pdastrXML->cchString );

    f_pdastrXML->pszString = ( DRM_CHAR * )pbXMLContext;

ErrorExit:

    /* Free the temporary buffer. */
    if ( f_poLicChlgContext != NULL && pchScratch != NULL )
    {
        DRM_STK_Free( &f_poLicChlgContext->m_oStackAllocatorContext, pchScratch );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildLicenseAcknowledgementDataXML
**
** Synopsis: Function that constructs the data portion of a license
**           acquisition acknowledgement XML. The data portion
**           may be encrypted by XML encryption.
**
** Arguments:
**
** [f_poKeyFileContext]     -- Pointer to a key file context.
** [f_cchDeviceCert]        -- Size ( number of characters ) of a device
**                             certificate.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildLicenseAcknowledgementDataXML(
    __in DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __in DRM_DWORD f_cchDeviceCert,
    __out DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_BYTE *pbXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrReservedBuffer = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseAcknowledgementDataXML );

    ChkArg( f_poKeyFileContext != NULL );
    ChkArg( f_cchDeviceCert > 0 );
    ChkDRMANSIString( f_pdastrXML );

    pbXMLContext = PB_DASTR( f_pdastrXML );
    cbXMLContext = CB_DASTR( f_pdastrXML );

    ChkDR( DRM_UTL_EnsureDataAlignment( pbXMLContext,
                                        cbXMLContext,
                                        ( DRM_BYTE ** )&poXMLContext,
                                        &cbXMLContext,
                                        SIZEOF( DRM_DWORD ),
                                        NULL ) );

    /*
    ** Add and open <Data> node.
    **
    ** Output:
    ** <Data>
    */
    ChkDR( DRM_XMB_CreateDocumentA( cbXMLContext,
                                    ( DRM_BYTE * )poXMLContext,
                                    &g_dastrLicAckDataTag ) );

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Add and open <CertificateChain> node.
    **
    ** Output:
    ** <Data>
    **      <CertificateChain>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrLicAckCertificateChainTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    ChkDR( DRM_XMB_ReserveSpaceA( poXMLContext,
                                  SIZEOF( DRM_WORD ),
                                  f_cchDeviceCert,
                                  &dasstrReservedBuffer ) );

    /*
    ** The reserved the buffer should be big enough to store the whole device
    ** certificates. In case the buffer has more bytes needed by the device
    ** certificate, those bytes should be set to ' ' so the server side XML
    ** parser can just ignore them.
    */
    MEMSET( pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
            ' ',
            dasstrReservedBuffer.m_cch );

    /*
    ** Construct the device certificate XML segment.
    **
    ** Output:
    ** <Data>
    **      <CertificateChain>
    **          Device certificate
    */

    ChkDR( DRM_SOAPXML_GetDeviceCert(  f_poKeyFileContext,
                                       TRUE,
                                       pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                                       &dasstrReservedBuffer.m_cch ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** <Data>
    **      ...
    ** </Data>
    */
    ChkDR( DRM_XMB_CloseDocumentA( poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

    ChkDR( DRM_SOAPXML_PadANSIString( f_pdastrXML, DRM_AES_BLOCKLEN ) );

    DRM_BYT_MoveBytes( pbXMLContext, 0, pchXML, 0, f_pdastrXML->cchString );

    f_pdastrXML->pszString = ( DRM_CHAR * )pbXMLContext;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildLicenseChallengeXML
**
** Synopsis: Function that constructs a license acquisition challenge
**           XML string.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size ( number of bytes ) of internal buffer
**                             of a XML builder context.
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed license acquisition
**                             challenge XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_RESULT _BuildLicenseChallengeXML(
    __inout _XMBContextA *f_poXMLContext,
    __in    DRM_DWORD f_cbXMLContext,
    __in    DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext,
    __out   DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrEncryptedDataNode = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrEncryptedXML = EMPTY_DRM_STRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchBuffer = 0;
    DRM_CHAR* pchScratch = NULL;
    DRM_DWORD cchScratch = 0;
    DRM_ANSI_STRING dastrScratch = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrCustomData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchCustomData = 0;
    DRM_GUID oEmptyGUID = EMPTY_DRM_GUID;
    DRM_SUBSTRING dasstrSignedNode = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseChallengeXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poLicChlgContext != NULL );
    ChkArg( f_poLicChlgContext->m_poBBXContext != NULL );
    ChkDRMString( &f_poLicChlgContext->m_dstrContentHeader );
    ChkArg( f_pdastrXML != NULL );

    /* Calculate the maximum character count needed by temporary buffer. */
    cchBuffer = _GetMaxChallengeTempBufferCount( f_poLicChlgContext );

    ChkBOOL( cchBuffer >= DRM_MAX_CCH_BASE10_DWORD_STRING, DRM_E_INVALIDARG );

    /* Allocate the temporary buffer. */
    ChkDR( DRM_STK_Alloc( &f_poLicChlgContext->m_oStackAllocatorContext,
                          cchBuffer,
                          ( DRM_VOID ** )&pchScratch ) );

    dastrScratch.pszString = pchScratch;

#if DRM_USE_SOAP

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( f_poXMLContext, f_cbXMLContext ) );

    /*
    ** Add and open <AcquireLicense> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <AcquireLicense>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqRootTag,
                              NULL,
                              &g_dastrLicAcqRootAttribName,
                              &g_dastrLicAcqRootAttribValue,
                              wttOpen ) );

#else

    /*
    ** Add and open <AcquireLicense> node.
    **
    ** Output:
    ** <AcquireLicense>
    */
    ChkDR( DRM_XMB_CreateDocumentA( f_cbXMLContext,
                                    ( DRM_BYTE * )f_poXMLContext,
                                    &g_dastrLicAcqRootTag ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrLicAcqRootAttribName,
                                  &g_dastrLicAcqRootAttribValue ) );

#endif

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      <challenge>
    **          <Challenge ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqChallenge2Tag,
                              NULL,
                              &g_dastrLicAcqChallenge2AttribName,
                              &g_dastrLicAcqChallenge2AttribValue,
                              wttOpen ) );

    /*
    ** Add and open <LA> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      <challenge>
    **          <Challenge ...>
    **              <LA ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqLATag,
                              NULL,
                              &g_dastrLicAcqChallengeVAAttrib1Name,
                              &g_dastrLicAcqChallengeVAAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrLicAcqChallengeVAAttrib2Name,
                                  &g_dastrLicAcqChallengeVAAttrib2Value ) );


    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPPreserveSpaceAttribName,
                                  &g_dastrSOAPPreserveSpaceAttribValue ) );


    /*
    ** Add and close <version> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      <challenge>
    **          <Challenge ...>
    **              <LA ...>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqVersionTag,
                              &g_dastrLicAcqVersionValue,
                              NULL,
                              NULL,
                              wttClosed ) );

    /*
    ** Attempt to convert the unicode string to UTF8
    */
    cchScratch = cchBuffer;
    ChkDR( DRM_STR_DSTRtoUTF8( (const DRM_STRING*) &f_poLicChlgContext->m_dstrContentHeader,
                                                    pchScratch,
                                                   &cchScratch ) );

    dastrScratch.cchString = cchScratch;

    /*
    ** [<SOAP headers>] ( optional )
    ** Add and close <ContentHeader> node.
    **
    ** Output:
    ** <AcquireLicense>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <LA ...>
    **                  ...
    **                  <ContentHeader>...</ContentHeader>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAcqContentHeaderTag,
                              ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                              NULL,
                              NULL,
                              wttClosed ) );

    /*
    ** Construct the client information XML segment.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <LA ...>
    **                  ...
    **                  <ClientInfo>
    **                      ...
    **                  </ClientInfo>
    */
    ChkDR( DRM_SOAPXML_BuildClientInfo( f_poXMLContext,
                                        f_poLicChlgContext->m_poBBXContext->pClientInfo ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /*
        ** Construct the revocation version information XML segment.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcquireLicense>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <LA ...>
        **                  ...
        **                  <RevocationLists>
        **                      ...
        **                  </RevocationLists>
        */
        ChkDR( _BuildRevListInfoXML( f_poXMLContext,
                                     &f_poLicChlgContext->m_oStackAllocatorContext,
                                     f_poLicChlgContext->m_rgoRevListInfo,
                                     MAX_REVLIST_INFO,
                                     cchBuffer ) );
    }

    /* Estimate the size of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( ( DRM_CHAR * )f_poLicChlgContext->m_dastrCustomData.pszString,
                                   f_poLicChlgContext->m_dastrCustomData.cchString,
                                   NULL,
                                   &cchCustomData ) );

    if ( cchCustomData > 0  )
    {
        ChkDR( DRM_SOAPXML_EnsureTagValueNativeByteAligned( f_poXMLContext,
                                                            &g_dastrLicAcqCustomDataTag ) );

        /*
        ** Add and open <CustomData> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcquireLicense>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <LA ...>
        **                  ...
        **                  <CustomData>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqCustomDataTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContext,
                                      SIZEOF( DRM_BYTE ),
                                      cchCustomData,
                                      &dasstrCustomData ) );

        ChkDR( DRM_SOAPXML_EncodeData( ( DRM_CHAR * )f_poLicChlgContext->m_dastrCustomData.pszString,
                                       f_poLicChlgContext->m_dastrCustomData.cchString,
                                       pchXML + __CB_DECL( dasstrCustomData.m_ich ),
                                       &cchCustomData ) );

        /*
        ** Close <CustomData> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcquireLicense>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <LA ...>
        **                  ...
        **                  <CustomData>...</CustomData>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    if ( MEMCMP( &f_poLicChlgContext->m_oNonce, &oEmptyGUID, SIZEOF( DRM_GUID ) ) != 0 )
    {
        cchScratch = CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) );

        ChkBOOL( cchScratch <= cchBuffer, DRM_E_INVALIDARG );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poLicChlgContext->m_oNonce,
                                SIZEOF( DRM_ID ),
                                pchScratch,
                                &cchScratch,
                                0 ) );

        dastrScratch.cchString = cchScratch;

        /*
        ** Construct the license nonce XML segment.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcquireLicense>
        **      ...
        **      <challenge>
        **          <Challenge>
        **              <LA ...>
        **                  ...
        **                  <LicenseNonce>
        **                      ...
        **                  </LicenseNonce>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqLicenseNonceTag,
                                  ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    ChkDR( DRM_XMB_GetCurrentBufferPointerA( f_poXMLContext,
                                             TRUE,
                                             ( DRM_CHAR ** )&dastrEncryptedXML.pszString ) );

    dastrEncryptedXML.cchString = _CalcLicenseChallengeDataCharCount( f_poLicChlgContext );

    /*
    ** Construct the data portion of the license acquisition challenge XML.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <LA ...>
    **                  ...
    **                  <Data>
    **                      ...
    **                  </Data>
    */
    ChkDR( _BuildLicenseChallengeDataXML( f_poLicChlgContext,
                                          ( DRM_ANSI_STRING * )&dastrEncryptedXML ) );

    /* AES encrypt the data portion of the license acquisition challenge XML. */
    ChkDR( DRM_SOAPXML_EncryptDataWithXMLKey( f_poLicChlgContext->m_poBBXContext->pOEMContext,
                                              &f_poLicChlgContext->m_oXMLKey,
                                              &dastrEncryptedXML,
                                              &f_poLicChlgContext->m_poBBXContext->CryptoContext) );

    /*
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <LA ...>
    **                  ...
    **                  <EncryptedData>
    **                      ...
    **                  </EncryptedData>
    */
    ChkDR( DRM_XMLSIG_BuildEncryptedDataNode( f_poXMLContext,
                                              &f_poLicChlgContext->m_oXMLKey,
                                              dastrEncryptedXML.cchString,
                                              &dasstrEncryptedDataNode ) );

    /*
    ** Close <LA> node
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );

    /* The whole <LA> node (inclusive) is signed. */
    ChkDR( DRM_XMLSIG_BuildSignatureNode( f_poXMLContext,
                                          f_poLicChlgContext->m_poBBXContext,
                                          pchXML,
                                          &dasstrSignedNode ) );


    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcquireLicense>
    **      ...
    ** </AcquireLicense>
    */
    ChkDR( DRM_XMB_CloseDocumentA( f_poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

ErrorExit:

    /* Free the temporary buffer. */
    if ( f_poLicChlgContext != NULL && pchScratch != NULL )
    {
        DRM_STK_Free( &f_poLicChlgContext->m_oStackAllocatorContext, pchScratch );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildLicenseAcknowledgementXML
**
** Synopsis: Function that constructs a license acquisition acknowledgement
**           XML string.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size ( number of bytes ) of internal buffer
**                             of a XML builder context.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poStackAllocatorContext]
**                          -- Pointer to a stack allocator context.
** [f_pdastrTransactionID]  -- Pointer to a DRM ANSI string that contains a
**                             transaction ID.
** [f_poLicAcks]            -- An array of license acknowlegement structures
**                             that store the results of license processing.
** [f_cLicAcks]             -- Total number of elements of the license
**                             acknowledgement structures in the array
**                             mentioned above.
** [f_cchDeviceCert]        -- Size ( number of characters ) of a device
**                             certificate.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildLicenseAcknowledgementXML(
    __inout                           _XMBContextA                *f_poXMLContext,
    __in                              DRM_DWORD                    f_cbXMLContext,
    __in                              DRM_BB_CONTEXT              *f_poBBXContext,
    __in                              DRM_STACK_ALLOCATOR_CONTEXT *f_poStackAllocatorContext,
    __in                        const DRM_ANSI_CONST_STRING       *f_pdastrTransactionID,
    __in_ecount_opt(f_cLicAcks) const DRM_LICENSE_ACK             *f_poLicAcks,
    __in                              DRM_DWORD                    f_cLicAcks,
    __in                              DRM_DWORD                    f_cchDeviceCert,
    __in                              DRM_XML_KEY                 *f_poXMLKey,
    __out                             DRM_ANSI_STRING             *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrSignedNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrEncryptedDataNode = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrEncryptedXML = EMPTY_DRM_STRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchBuffer = 0;
    DRM_CHAR* pchScratch = NULL;
    DRM_DWORD cchScratch = 0;
    DRM_ANSI_STRING dastrScratch = EMPTY_DRM_STRING;
    DRM_DWORD i = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildLicenseAcknowledgementXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poStackAllocatorContext != NULL );
    ChkDRMANSIString( f_pdastrTransactionID );
    ChkArg( ( f_cLicAcks > 0 && f_poLicAcks != NULL ) ||
            ( f_cLicAcks == 0 && f_poLicAcks == NULL ) );
    ChkArg( f_cchDeviceCert > 0 );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_pdastrXML != NULL );

    /* Calculate the maximum character count needed by temporary buffer. */
    cchBuffer = _GetMaxAcknowledgementTempBufferCount();

    ChkBOOL( cchBuffer >= DRM_MAX_CCH_BASE10_DWORD_STRING, DRM_E_INVALIDARG );

    /* Allocate the temporary buffer. */
    ChkDR( DRM_STK_Alloc( f_poStackAllocatorContext, cchBuffer, ( DRM_VOID ** )&pchScratch ) );

    dastrScratch.pszString = pchScratch;

#if DRM_USE_SOAP

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( f_poXMLContext, f_cbXMLContext ) );

    /*
    ** Add and open <AcknowledgeLicense> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <AcknowledgeLicense>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckRootTag,
                              NULL,
                              &g_dastrLicAckRootAttribName,
                              &g_dastrLicAckRootAttribValue,
                              wttOpen ) );

#else

    /*
    ** Add and open <AcknowledgeLicense> node.
    **
    ** Output:
    ** <AcknowledgeLicense>
    */
    ChkDR( DRM_XMB_CreateDocumentA( f_cbXMLContext,
                                    ( DRM_BYTE * )f_poXMLContext,
                                    &g_dastrLicAckRootTag ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrLicAckRootAttribName,
                                  &g_dastrLicAckRootAttribValue ) );

#endif

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      <challenge>
    **          <Challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckChallenge2Tag,
                              NULL,
                              &g_dastrLicAckChallenge2AttribName,
                              &g_dastrLicAckChallenge2AttribValue,
                              wttOpen ) );

    /*
    ** Add and open <Ack> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckAckTag,
                              NULL,
                              &g_dastrLicAckAckAttrib1Name,
                              &g_dastrLicAckAckAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrLicAckAckAttrib2Name,
                                  &g_dastrLicAckAckAttrib2Value ) );


    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPPreserveSpaceAttribName,
                                  &g_dastrSOAPPreserveSpaceAttribValue ) );

    /*
    ** Add and close <Version> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      <challenge>
    **          <Challenge>
    **              <Ack ...>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckVersionTag,
                              &g_dastrLicAckVersionValue,
                              NULL,
                              NULL,
                              wttClosed ) );

    /*
    ** Add and close <TransactionID> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  <TransactionID>...</TransactionID>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckTransactionIDTag,
                              f_pdastrTransactionID,
                              NULL,
                              NULL,
                              wttClosed ) );

    /*
    ** Construct the client information XML segment.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  <ClientInfo>
    **                      ...
    **                  </ClientInfo>
    */
    ChkDR( DRM_SOAPXML_BuildClientInfo( f_poXMLContext,
                                        f_poBBXContext->pClientInfo ) );

    /*
    ** Add and open <LicenseStorageResults> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  ...
    **                  <LicenseStorageResults>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrLicAckLicenseStorageResultsTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    for ( i = 0; i < f_cLicAcks; i++ )
    {
        /*
        ** Add and open <Licenses> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcknowledgeLicense>
        **      ...
        **      <challenge>
        **          <Challenge>
        **              <Ack>
        **                  ...
        **                  <LicenseStorageResults>
        **                      ...
        **                      <License>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAckLicenseTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        cchScratch = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

        ChkBOOL( cchScratch <= cchBuffer, DRM_E_INVALIDARG );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poLicAcks[ i ].m_oKID,
                                SIZEOF( DRM_GUID ),
                                pchScratch,
                                &cchScratch,
                                0 ) );

        dastrScratch.cchString = cchScratch;

        /*
        ** Add and close <KID> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcknowledgeLicense>
        **      ...
        **      <challenge>
        **          <Challenge>
        **              <Ack>
        **                  ...
        **                  <LicenseStorageResults>
        **                      ...
        **                      <License>
        **                          <KID>...</KID>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAckKIDTag,
                                  ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        cchScratch = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

        ChkBOOL( cchScratch <= cchBuffer, DRM_E_INVALIDARG );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poLicAcks[ i ].m_oLID,
                                SIZEOF( DRM_GUID ),
                                pchScratch,
                                &cchScratch,
                                0 ) );

        dastrScratch.cchString = cchScratch;

        /*
        ** Add and close <LID> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcknowledgeLicense>
        **      ...
        **      <challenge>
        **          <Challenge>
        **              <Ack>
        **                  ...
        **                  <LicenseStorageResults>
        **                      ...
        **                      <License>
        **                          ...
        **                          <LID>...</LID>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAckLIDTag,
                                  ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        cchScratch = cchBuffer;

        ChkDR( _DRMResultToANSIString( f_poLicAcks[ i ].m_dwResult, pchScratch, &cchScratch ) );

        dastrScratch.cchString = cchScratch;

        /*
        ** Add and close <Result> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcknowledgeLicense>
        **      ...
        **      <challenge>
        **          <Challenge>
        **              <Ack>
        **                  ...
        **                  <LicenseStorageResults>
        **                      ...
        **                      <License>
        **                          ...
        **                          <Result>...</Result>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAckResultTag,
                                  ( DRM_ANSI_CONST_STRING * )&dastrScratch,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        /*
        ** Close <License> node.
        **
        ** Output:
        ** [<SOAP headers>] ( optional )
        ** <AcknowledgeLicense>
        **      ...
        **      <challenge>
        **          <Challenge>
        **              <Ack>
        **                  ...
        **                  <LicenseStorageResults>
        **                      ...
        **                      <License>
        **                          ...
        **                      </License>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    /*
    ** Close <LicenseStorageResults> node.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  ...
    **                  <LicenseStorageResults>
    **                      ...
    **                  </LicenseStorageResults>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );

    ChkDR( DRM_XMB_GetCurrentBufferPointerA( f_poXMLContext,
                                             TRUE,
                                             ( DRM_CHAR ** )&dastrEncryptedXML.pszString ) );

    dastrEncryptedXML.cchString = _CalcLicenseAcknowledgementDataCharCount( f_cchDeviceCert );

    /*
    ** Construct the data portion of the license acquisition acknowledgement XML.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  ...
    **                  <Data>
    **                      <CertificateChain>
    **                          ...
    **                      </CertificateChain>
    **                  </Data>
    */
    ChkDR( _BuildLicenseAcknowledgementDataXML( f_poBBXContext->pKeyFileContext,
                                                f_cchDeviceCert,
                                                ( DRM_ANSI_STRING * )&dastrEncryptedXML ) );

    /* XML encrypt the data portion of the license acquisition acknowledgement XML. */
    ChkDR( DRM_SOAPXML_EncryptDataWithXMLKey( f_poBBXContext->pOEMContext,
                                              f_poXMLKey,
                                              &dastrEncryptedXML,
                                              &f_poBBXContext->CryptoContext) );


    /*
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  ...
    **                  <EncryptedData>
    **                      ...
    **                  </EncryptedData>
    */
    ChkDR( DRM_XMLSIG_BuildEncryptedDataNode( f_poXMLContext,
                                              f_poXMLKey,
                                              dastrEncryptedXML.cchString,
                                              &dasstrEncryptedDataNode ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );

    /*
    ** The whole <Ack> node ( inclusive ) is signed.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    **      <challenge>
    **          <Challenge>
    **              <Ack>
    **                  ...
    **              </Ack>
    **              <Signature>
    **                  ...
    **              </Signature>
    */
    ChkDR( DRM_XMLSIG_BuildSignatureNode( f_poXMLContext,
                                          f_poBBXContext,
                                          pchXML,
                                          &dasstrSignedNode ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** [<SOAP headers>] ( optional )
    ** <AcknowledgeLicense>
    **      ...
    ** </AcknowledgeLicense>
    */
    ChkDR( DRM_XMB_CloseDocumentA( f_poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

ErrorExit:

    /* Free the temporary buffer. */
    if ( f_poStackAllocatorContext != NULL && pchScratch != NULL )
    {
        DRM_STK_Free( f_poStackAllocatorContext, pchScratch );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_ParseServiceID
**
** Synopsis: Function that parses out a service ID from
**           the content header.
**
** Arguments:
**
** [f_pdstrContentHeader]   -- Pointer to a DRM string that contains the
**                             content header.
** [f_pServiceID]           -- Pointer to a GUID to receive a parsed out
**                             service ID.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_CH_INVALID_HEADER if header has a service
**                          ID which is not a base-64 encoded GUID
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LA_ParseServiceID(
    __in  const DRM_CONST_STRING *f_pdstrContentHeader,
    __out       DRM_GUID         *f_pServiceID )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_CONST_STRING    dstrServiceID   = EMPTY_DRM_STRING;
    DRM_DWORD           cbServiceID     = sizeof( *f_pServiceID );

    ChkArg( f_pdstrContentHeader != NULL );

    /* Try to extract the service ID from the content header. */
    dr = DRM_HDR_GetAttribute( f_pdstrContentHeader,
                                NULL,
                                DRM_HEADER_ATTRIB_DOMAIN_SERVICE_ID,
                               &dstrServiceID,
                                0 );

    if( dr != DRM_E_XMLNOTFOUND )
    {
        /* Service ID tag could be found or not, all other error should be thrown to caller */
        ChkDR( dr );

        ChkDR( DRM_B64_DecodeW( &dstrServiceID,
                                &cbServiceID,
                                ( DRM_BYTE * )f_pServiceID,
                                0 ) );

        ChkBOOL( cbServiceID == SIZEOF( DRM_GUID ), DRM_E_CH_INVALID_HEADER );
    }
    else
    {
        dr = DRM_SUCCESS;
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_ParseLicenseAcquisitionURL
**
** Synopsis: Function that parses out a license acquisition URL from
**           the content header.
**
** Arguments:
**
** [f_pdstrContentHeader]   -- Pointer to a DRM string that contains the
**                             content header.
** [f_fSilent]              -- Whether silent or non silent URL license
**                             acquisition URL should be returned. TRUE for
**                             silent license acquisition URL and FALSE for
**                             non silent license acquisition URL.
** [f_pchURL]               -- Pointer to a buffer to receive a parsed out
**                             URL. If it is NULL, the function returns an
**                             error and the size of the required buffer
**                             is stored in *f_pcchURL.
** [f_pcchURL]              -- Pointer to a variable that contains the size
**                             ( in characters ) of the URL buffer during input
**                             and receives the size ( in characters ) of the
**                             URL during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NO_URL if URL cannot be parsed out from
**                          the content header.
**
** Note: The function converts the URL in the content header from Unicode
** to UTF8/ANSI.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LA_ParseLicenseAcquisitionURL(
    __in                     const DRM_CONST_STRING *f_pdstrContentHeader,
    __in                           DRM_BOOL          f_fSilent,
    __out_ecount_opt( *f_pcchURL ) DRM_CHAR         *f_pchURL,
    __inout                        DRM_DWORD        *f_pcchURL )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrURL = EMPTY_DRM_STRING;
    DRM_DWORD cchTemp = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC_DRM_LA_ParseLicenseAcquisitionURL );

    ChkDRMString( f_pdstrContentHeader );
    ChkArg( f_pcchURL != NULL );
    ChkArg( f_pchURL == NULL || *f_pcchURL > 0 );

    dr = DRM_HDR_GetAttribute( f_pdstrContentHeader,
                               NULL,
                               f_fSilent ? DRM_HEADER_ATTRIB_LICENSE_ACQ_V4_URL :
                                           DRM_HEADER_ATTRIB_LICENSE_UI_URL,
                               &dstrURL,
                               0 );

    if ( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_E_NO_URL );
    }

    if ( f_pchURL == NULL || *f_pcchURL < dstrURL.cchString + 1 )
    {
        *f_pcchURL = dstrURL.cchString + 1;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if ( dstrURL.cchString > 0 )
    {
        cchTemp = *f_pcchURL - 1;

        ChkDR( DRM_UTL_XMLDecodeUA( dstrURL.pwszString,
                                    dstrURL.cchString,
                                    f_pchURL,
                                    &cchTemp ) );

        DRMASSERT( cchTemp < *f_pcchURL );

        /*
        ** PreFAST tool can't verify above assert is always true.
        */
        ChkBOOL( cchTemp < *f_pcchURL, DRM_E_FAIL );

        f_pchURL[ cchTemp ] = 0;

        *f_pcchURL = cchTemp + 1;
    }
    else
    {
        f_pchURL[ 0 ] = 0;

        *f_pcchURL = 1;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_ProcessRevocationPackage
**
** Synopsis: Function that extracts the revocation information from a
**           server response string and then processes and stores the
**           received revocation information.
**
** Arguments:
**
** [f_poLicRespContext]     -- Pointer to a data structure that contains
**                             information needed to process licenses.
** [f_pszResponse]          -- Pointer to a character buffer that contains
**                             the server response.
** [f_pcProcessed]          -- Pointer to a variable that receives the total number
**                             of revocation nodes processed.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_LRB_INVALID_REVOCATION_PACKAGE if XML structure
**                          of response is invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LA_ProcessRevocationPackage(
    __in        DRM_LICRESP_CONTEXT_V3 *f_poLicRespContext,
    __in  const DRM_ANSI_CONST_STRING  *f_pdastrResponse,
    __out       DRM_DWORD              *f_pcProcessed )
{
    DRM_RESULT          dr                    = DRM_SUCCESS;
    DRM_SUBSTRING       dasstrRevInfo         = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrRevocation      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrRevocationNode  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrListId          = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrListData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING       dasstrResponse        = EMPTY_DRM_SUBSTRING;
    DRM_BOOL            fFoundRevInfo         = FALSE;
    DRM_BOOL            fRevInfoPass          = FALSE;
    DRM_DWORD           iRevInfoNode          = 0;
    DRM_DWORD           iNode                 = 0;
    DRM_GUID            guidCRL               = {0};
    DRM_DWORD           cbCRL                 = SIZEOF( DRM_GUID );
    DRM_BYTE           *pbDecoded             = NULL;
    DRM_DWORD           cbDecoded             = 0;
    DRM_BOOL            fUpdated              = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC_DRM_LA_ProcessRevocationPackage );

    if( !DRM_REVOCATION_IsRevocationSupported() )
    {
        /* Nothing to do */
        goto ErrorExit;
    }

    ChkArg( f_poLicRespContext != NULL );
    ChkArg( f_pdastrResponse != NULL );
    ChkArg( f_pdastrResponse->pszString != NULL );
    ChkArg( f_pcProcessed != NULL );

    *f_pcProcessed = 0;

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

#if DRM_USE_SOAP
    dr = DRM_SOAPXML_SkipSOAPHeaders( f_pdastrResponse->pszString,
                                     &dasstrResponse );
    if( dr == DRM_E_SOAPXML_XML_FORMAT )
    {
        /* It's fine if there was no SOAP wrapper */
        dasstrResponse.m_ich = 0;
        dasstrResponse.m_cch = f_pdastrResponse->cchString;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }
#endif /* DRM_USE_SOAP */

    /*
    ** Extract <RevInfo> node from
    ** <AcquireLicenseResponse>/<AcquireLicenseResult>/<LicenseResponse> node or by itself.
    */
    if ( DRM_FAILED( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                                &dasstrResponse,
                                                &g_dastrLicRevInfoPath,
                                                NULL,
                                                NULL,
                                                NULL,
                                                &dasstrRevInfo,
                                                g_chForwardSlash ) ) )
    {
        if ( DRM_FAILED( DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                                           &dasstrResponse,
                                           &g_dastrRevInfo,
                                           NULL,
                                           NULL,
                                           0,
                                           NULL,
                                           &dasstrRevInfo ) ) )
        {
            /*
             *  Check if provided xml has required node
             */
            if ( DRM_FAILED( DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                                               &dasstrResponse,
                                               &g_dastrAcquireLicenseResponseTag,
                                               NULL,
                                               NULL,
                                               0,
                                               NULL,
                                               &dasstrRevInfo ) ) )
            {
                dr = DRM_E_REVOCATION_INVALID_PACKAGE ;
            }
            else
            {
                dr = DRM_SUCCESS;
            }
            goto ErrorExit;
        }
    }

    /*
    ** Iterate through the list looking for CRLs:
    ** 1. On the first pass, store CRLs but skip over RevInfo blobs (while
    **    remembering where we found them)
    ** 2. When all the CRLs have been updated, go back and process the one
    **    RevInfo blob we might have found.
    */
    for ( iNode = 0; TRUE; iNode++ )
    {
        /* Read one <Revocation> node from the <RevInfo> node. */
        dr = DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                               &dasstrRevInfo,
                               &g_dastrLicRevocationPath,
                               NULL,
                               NULL,
                               fRevInfoPass ? iRevInfoNode : iNode,
                               &dasstrRevocationNode,
                               &dasstrRevocation );

        if ( dr == DRM_E_XMLNOTFOUND )
        {
            if ( fRevInfoPass || !fFoundRevInfo )
            {
                ChkDR( DRM_SUCCESS );

                break;
            }
            else
            {
                fRevInfoPass = TRUE;
                continue;
            }
        }
        else
        {
            ChkDR( dr );
        }

        /* Get ListId information */
        ChkDR( DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                                 &dasstrRevocation,
                                 &g_dastrLicListIdPath,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                 &dasstrListId ) );

        /* Get ListData information */
        ChkDR( DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                                 &dasstrRevocation,
                                 &g_dastrLicListDataPath,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                 &dasstrListData ) );

        /* Process the revocation information. */
        ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                        &dasstrListId,
                        &cbCRL,
                        ( DRM_BYTE * )&guidCRL,
                        0 ) ) ;

        ChkBOOL( cbCRL == SIZEOF( DRM_GUID ), DRM_E_INVALID_REVOCATION_LIST );

        if( !fRevInfoPass && MEMCMP( &guidCRL, &g_guidRevocationTypeRevInfo, SIZEOF( DRM_GUID ) ) == 0 )
        {
            if( !fFoundRevInfo )
            {
                fFoundRevInfo = TRUE;
                iRevInfoNode = iNode;
            }
            continue;
        }
        else if( !fRevInfoPass && MEMCMP( &guidCRL, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) == 0 )
        {
            fFoundRevInfo = TRUE;
            iRevInfoNode = iNode;
            continue;
        }

        cbDecoded = CB_BASE64_DECODE( dasstrListData.m_cch ) + 1;
        ChkMem( pbDecoded = ( DRM_BYTE * )Oem_MemAlloc( cbDecoded ) );

        /* decode the XML for the new exclusion list. */
        ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                               &dasstrListData,
                               &cbDecoded,
                                pbDecoded,
                                0 ) );

        ChkDR( DRM_RVK_SetCRL( &f_poLicRespContext->m_poBBXContext->CryptoContext,
                                f_poLicRespContext->m_poRevStoreContext,
                               &guidCRL,
                                pbDecoded,
                                cbDecoded,
                                f_poLicRespContext->m_poLicEvalContext->pbRevocationBuffer,
                                f_poLicRespContext->m_poLicEvalContext->cbRevocationBuffer,
                               &fUpdated ) );

        SAFE_OEM_FREE( pbDecoded );

        (*f_pcProcessed)++;

        if( fRevInfoPass )
        {
            /* If we processed the revinfo then we're done */
            break;
        }
    }

ErrorExit:

    SAFE_OEM_FREE( pbDecoded );

    if ( fUpdated )
    {
        DRM_RVK_UpdateRevocationVersionsCache( f_poLicRespContext->m_poRevStoreContext,
                                              &f_poLicRespContext->m_poBBXContext->CryptoContext,
                                              &f_poLicRespContext->m_poLicEvalContext->idCRLsCurrent,
                                               f_poLicRespContext->m_poLicEvalContext->pbRevocationBuffer,
                                               f_poLicRespContext->m_poLicEvalContext->cbRevocationBuffer,
                                               NULL );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _GetClientIdFromDeviceCert
**
** Synopsis: Function that gets the client ID from the device cert
**
** Arguments:
**
** [f_poBlackBoxContext]    -- Pointer to a BlackBox context for cert process.
** [f_pbScratchBCertBuffer] -- Pointer to a scratch buffer for cert processing.
**                             The scratch buffer is used to hold the device
**                             cert.
** [f_cbScratchBCertBuffer] -- Size (number of bytes) of the scratch buffer.
**                             The caller, _ValidateLicenseResponse, allocates
**                             a buffer with size of DRM_MAX_LICENSESIZE (10K).
** [f_poClientId]           -- Pointer to DRM_BCERT_CLIENT_ID struct
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_DEVCERT_EXCEEDS_SIZE_LIMIT if size of scratch
**                          buffer can't hold the device cert.
**
**********************************************************************/
static DRM_RESULT _GetClientIdFromDeviceCert(
    __in            DRM_BB_CONTEXT          *f_poBlackBoxContext,
    __inout_bcount( f_cbScratchBCertBuffer )
                    DRM_BYTE                *f_pbScratchBCertBuffer,
    __in    const   DRM_DWORD                f_cbScratchBCertBuffer,
    __inout         DRM_BCERT_CLIENT_ID     *f_poClientId )
{
    DRM_RESULT              dr              = DRM_SUCCESS;
    DRM_DWORD               cbDeviceCert    = 0;
    DRM_DWORD               dwLeafCertStart = 0;
    DRM_BCERT_CHAIN_HEADER  oChainHeader    = {0};
    DRM_BCERT_CERTIFICATE   oLeafCert       = {0};

    ChkArg( f_poBlackBoxContext != NULL );
    ChkArg( f_pbScratchBCertBuffer != NULL );
    ChkArg( f_cbScratchBCertBuffer > 0 );
    ChkArg( f_poClientId != NULL );

    ChkDR( DRM_SOAPXML_GetDeviceCert( f_poBlackBoxContext->pKeyFileContext,
                                      FALSE,
                                      NULL,
                                      &cbDeviceCert ) );
    ChkBOOL( cbDeviceCert < f_cbScratchBCertBuffer, DRM_E_DEVCERT_EXCEEDS_SIZE_LIMIT );

    ChkDR( DRM_SOAPXML_GetDeviceCert( f_poBlackBoxContext->pKeyFileContext,
                                      FALSE,
                                      ( DRM_CHAR * )f_pbScratchBCertBuffer,
                                      &cbDeviceCert ) );

    ChkDR( DRM_BCert_GetChainHeader( f_pbScratchBCertBuffer,
                                     cbDeviceCert,
                                     &dwLeafCertStart,
                                     &oChainHeader ) );

    /*
    ** Get the leaf cert.
    */
    ChkDR( DRM_BCert_GetCertificate( f_pbScratchBCertBuffer,
                                     cbDeviceCert,
                                     &dwLeafCertStart,
                                     &oLeafCert,
                                     DRM_BCERT_CERTTYPE_DEVICE ) );

    /*
    ** Get the client ID from leaf cert.
    */
    MEMCPY( f_poClientId,
            &oLeafCert.BasicInfo.ClientID,
            SIZEOF( DRM_BCERT_CLIENT_ID ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _ValidateLicenseResponse
**
** Synopsis: Function that validates license response when <signature> node
**           is present.  If the caller opt-in validation but the <signature>
**           is missing then return an error.  The following nodes are verified:
**           - CertificateChain
**           - ResponseID
**           - Signature
**
** Arguments:
**
** [f_poLicRespContext]     -- Pointer to a data structure that contains
**                             information needed to process license response.
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that contains
**                             the server response.
** [f_pdasstrResponse]      -- Pointer to a sub string that defines the licenses
**                             string ( pre-B64 decoded ) within the server
**                             response buffer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_LICENSE_RESPONSE_SIGNATURE_MISSING if
**                          the <signature> node is not present in response.
**
**********************************************************************/
static DRM_RESULT _ValidateLicenseResponse(
    __in       DRM_LICRESP_CONTEXT_V3  *f_poLicRespContext,
    __in const DRM_ANSI_CONST_STRING   *f_pdastrResponse,
    __in const DRM_SUBSTRING           *f_pdasstrResponse )
{
    DRM_RESULT           dr                      = DRM_SUCCESS;
    DRM_SUBSTRING        dasstrSignature         = EMPTY_DRM_SUBSTRING;
    DRM_BCERT_CLIENT_ID  oClientId               = {0};
    DRM_DWORD            dwProtocolType          = DRM_SOAPXML_PROTOCOL_LIC_ACQ_RESPONSE;

    ChkArg( f_poLicRespContext != NULL );
    ChkDRMANSIString( f_pdastrResponse );
    ChkArg( f_pdasstrResponse != NULL );

    /*
    ** Extract <Signature> node from
    ** <AcquireLicenseResponse>/<AcquireLicenseResult>/<Response> node.
    */
    dr = DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                    f_pdasstrResponse,
                                    &g_dastrLicSignaturePath,
                                    NULL,
                                    NULL,
                                    &dasstrSignature,
                                    NULL,
                                    g_chForwardSlash );
    if( dr == DRM_E_XMLNOTFOUND )
    {
        if( 0 != ( f_poLicRespContext->m_eLicResponseFlag & DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED ) )
        {
            /*
            ** Caller doesn't requests response verification and there is no signature node in response.
            ** Continue response process.
            */
            dr = DRM_SUCCESS;
            goto ErrorExit;
        }

        ChkDR( DRM_E_LICENSE_RESPONSE_SIGNATURE_MISSING );
    }

    ChkDR( dr );

    /*
    ** Validate server's cert chain.
    */
    dr = DRM_RVK_VerifySigningCert( f_pdastrResponse,
                                    f_pdasstrResponse,
                                    &g_dastrLicSigningCertPath,
                                    f_poLicRespContext->m_pbLicenseBuffer,
                                    f_poLicRespContext->m_cbLicenseBufferSize,
                                    f_poLicRespContext->m_pbRevocationBuffer,
                                    f_poLicRespContext->m_cbRevocationBuffer,
                                    f_poLicRespContext->m_poBBXContext,
                                    f_poLicRespContext->m_poSecStoreGlobalContext,
                                    f_poLicRespContext->m_poDatastore,
                                    f_poLicRespContext->m_poRevStoreContext );

    if( dr == DRM_E_CERTIFICATE_REVOKED
     || dr == DRM_E_BCERT_BASICINFO_CERT_EXPIRED )
    {
        ChkDR( DRM_E_INVALID_LICENSE_RESPONSE_SIGNATURE );
    }
    ChkDR( dr );

    /*
    ** Validate signature.
    */
    dr = DRM_SOAPXML_ValidateProtocolSignature(
        (const DRM_BYTE *)f_pdastrResponse->pszString,
        f_pdastrResponse->cchString,
        f_poLicRespContext->m_pbLicenseBuffer,  /* use as scratch buffer for BCert.
                                                   m_pbLicenseBuffer has size of DRM_MAX_LICENSESIZE (10K) that
                                                   should be enough for BCert. */
        f_poLicRespContext->m_cbLicenseBufferSize,
        &f_poLicRespContext->m_poBBXContext->CryptoContext,
        &dwProtocolType );
    if( dr == DRM_E_XMLSIG_ECDSA_VERIFY_FAILURE
     || dr == DRM_E_XMLSIG_SHA_VERIFY_FAILURE )
    {
        ChkDR( DRM_E_INVALID_LICENSE_RESPONSE_SIGNATURE );
    }

    ChkDR( dr );

    /*
    ** Get the client ID (device ID) from device cert.
    */
    ChkDR( _GetClientIdFromDeviceCert(
        f_poLicRespContext->m_poBBXContext,
        f_poLicRespContext->m_pbLicenseBuffer,  /* use as scratch buffer for device cert.
                                                   m_pbLicenseBuffer has size of DRM_MAX_LICENSESIZE (10K) that
                                                   should be enough for device cert. */
        f_poLicRespContext->m_cbLicenseBufferSize,
        &oClientId ) );

    /*
    ** Validate responssId.
    */
    dr = DRM_LA_ValidateResponseId(
#ifdef TEE
		f_poLicRespContext,
#endif
        (const DRM_BYTE *)f_pdastrResponse->pszString,
        f_pdastrResponse->cchString,
        (const DRM_BYTE *)&oClientId,
        SIZEOF( oClientId ) );
    if( dr == DRM_E_XMLSIG_SHA_VERIFY_FAILURE )
    {
        ChkDR( DRM_E_INVALID_LICENSE_RESPONSE_ID );
    }

    ChkDR( dr );


ErrorExit:

    if( dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_E_LICENSE_RESPONSE_SIGNATURE_MISSING;
    }

    return dr;
}


/*********************************************************************
**
** Function: _ExtractLicensesFromLicenseResponse
**
** Synopsis: Function that extracts licenses from a server response
**           string and then processes and stores the licenses.
**
** Arguments:
**
** [f_poLicRespContext]     -- Pointer to a data structure that contains
**                             information needed to process licenses.
** [f_pszResponse]          -- Pointer to a character buffer that contains
**                             the server response.
** [f_pdasstrResponse]      -- Pointer to a sub string that defines the licenses
**                             string ( pre-B64 decoded ) within the server
**                             response buffer.
** [f_pSLKData]             -- Pointer to SLKData structure containing current
**                             session key information. May be NULL.
** [f_poLicAcks]            -- Array of structures that reveive the result of
**                             license processing and storage.
** [f_pcLicAcks]            -- Pointer to a variable that:
**                             On input, contains the maximum number of acks that
**                             can be used in f_poLicAcks.
**                             On output, receives the total number
**                             of elements used in the array mentioned above.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_LICACQ_TOO_MANY_LICENSES if *f_pcLicAcks is not large enough.
**
** Note:                    Call is responsible to provide the array that is
**                          used to store the result of license processing and
**                          storage.
**
**********************************************************************/
static DRM_RESULT _ExtractLicensesFromLicenseResponse(
    __in                                                   DRM_LICRESP_CONTEXT_V3 *f_poLicRespContext,
    __in                                                   DRM_ANSI_CONST_STRING  *f_pdastrResponse,
    __in                                             const DRM_SUBSTRING          *f_pdasstrResponse,
    __in_opt                                         const DRM_SLKDATA            *f_pSLKData,
    __out_ecount_part( *f_pcLicAcks, *f_pcLicAcks )        DRM_LICENSE_ACK        *f_poLicAcks,
    __inout                                                DRM_DWORD              *f_pcLicAcks )
{
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_SUBSTRING   dasstrLicenses  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrLicense   = EMPTY_DRM_SUBSTRING;
    DRM_DWORD       iNode           = 0;
    DRM_DWORD       cMaxLicAcks     = 0;
    DRM_DWORD       cLicenses       = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__ExtractLicensesFromLicenseResponse );

    ChkArg( f_poLicRespContext != NULL );
    ChkArg( f_pdastrResponse != NULL );
    ChkArg( f_pdasstrResponse != NULL );
    ChkArg( f_poLicAcks != NULL );
    ChkArg( f_pcLicAcks != NULL );

    cMaxLicAcks = *f_pcLicAcks;
    *f_pcLicAcks = 0;

    /*
    ** Extract <Licenses> node from
    ** <AcquireLicenseResponse>/<Response>/<AcquireLicenseResult>/<LicenseResponse> node.
    */
    ChkBOOL( f_pdastrResponse->cchString >=  f_pdasstrResponse->m_ich + f_pdasstrResponse->m_cch, DRM_E_INVALIDARG );
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      f_pdasstrResponse,
                                      &g_dastrLicLicensesPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrLicenses,
                                      g_chForwardSlash ) );

    ChkDR( DRM_XML_CountMatchingNodesA( f_pdastrResponse->pszString,
                                        &dasstrLicenses,
                                        &g_dastrLicLicensePath,
                                        NULL,
                                        NULL,
                                        &cLicenses ) );
    if ( cLicenses > cMaxLicAcks )
    {
        /* Make sure we have enough ack space */
        *f_pcLicAcks = cLicenses;
        ChkDR( DRM_E_LICACQ_TOO_MANY_LICENSES );
    }

    for ( ; ; )
    {
        /* Read one <License> node from the <Licenses> node. */
        dr = DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                               &dasstrLicenses,
                               &g_dastrLicLicensePath,
                               NULL,
                               NULL,
                               iNode,
                               NULL,
                               &dasstrLicense );

        if ( DRM_FAILED( dr ) )
        {
            if ( dr == DRM_E_XMLNOTFOUND )
            {
                ChkDR( DRM_SUCCESS );

                break;
            }
        }
        else
        {
            ChkDR( dr );
        }

        /* Add the license to the store etc. */
        if ( *f_pcLicAcks >= cMaxLicAcks )
        {
            /* This should never happen - we counted the nodes above with DRM_XML_CountMatchingNodesA */
            DRMASSERT( FALSE );
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
        else
        {
            f_poLicAcks[ *f_pcLicAcks ].m_dwResult = DRM_E_FAIL;

            ChkDR( DRM_LA_ProcessLicenseV3( f_poLicRespContext,
                                            (DRM_CHAR *) f_pdastrResponse->pszString,
                                            &dasstrLicense,
                                            f_pSLKData,
                                            &f_poLicAcks[ *f_pcLicAcks ] ) );

            if (  DRM_SNC_IsLicenseSyncSupported()
               && (   f_poLicRespContext->m_poLicEvalContext == NULL
                   || f_poLicRespContext->m_poLicEvalContext->plicenseXMR == NULL
                   || ! XMR_IS_REMOVAL_DATE_VALID( f_poLicRespContext->m_poLicEvalContext->plicenseXMR ) ) )
            {
                /*
                ** Failure of sync store update is ignored and the license processing
                ** should continue.
                */
                ChkDRContinue( DRM_SNC_UpdateKID( f_poLicRespContext->m_poSyncContext,
                                                  f_poLicRespContext->m_poViewRightsContext,
                                                  &f_poLicAcks[ *f_pcLicAcks ].m_oKID,
                                                  NULL,
                                                  DRM_SYNC_UPDATE_AGGREGATE ) );
            }


            ( *f_pcLicAcks )++;

            iNode++;
        }
    }

ErrorExit:

    if ( DRM_FAILED( dr ) && dr != DRM_E_LICACQ_TOO_MANY_LICENSES && f_pcLicAcks != NULL )
    {
        *f_pcLicAcks = 0;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_ValidateResponseId
**
** Synopsis: Function that verifies the hashed value in response ID node
**           matching a calculated hash value from license nonce and
**           device ID
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size (number of bytes) of the server response.
** [f_pbDeviceId]           -- Pointer to a buffer that contains the device
**                             ID (B64 decoded)
** [f_cbDeviceId]           -- Size (number of bytes) of device ID
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLSIG_SHA_VERIFY_FAILURE if the hashed
**                          value in response ID is invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LA_ValidateResponseId(
#ifdef TEE
	__in						DRM_LICRESP_CONTEXT_V3 *f_poLicRespContext,
#endif
    __in_bcount( f_cbResponse )   const DRM_BYTE       *f_pbResponse,
    __in                          const DRM_DWORD       f_cbResponse,
    __in_bcount( f_cbDeviceId )   const DRM_BYTE       *f_pbDeviceId,
    __in                          const DRM_DWORD       f_cbDeviceId )
{
    DRM_RESULT              dr                  = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING   dastrResponse       = EMPTY_DRM_STRING;
    DRM_SUBSTRING           dasstrResponse      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrNonce         = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING           dasstrResponseId    = EMPTY_DRM_SUBSTRING;
    DRM_DWORD               cbResponseId        = 0;
    DRM_DWORD               cbNonce             = 0;
    DRM_ID                  licNonce            = {0};
    DRM_SHA256_Context      SHAContext          = {0};
    DRM_SHA256_Digest       DigestValue         = {0};
    DRM_BYTE                rgbHashedResponseId[SHA256_DIGEST_SIZE_IN_BYTES] = {0};

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pbDeviceId != NULL );
    ChkArg( f_cbDeviceId > 0 );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );
#endif

    /*
    ** Extract <LicenseNonce> node from
    ** <AcquireLicenseResponse>/<AcquireLicenseResult>/<Response>/<LicenseResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA(
        dastrResponse.pszString,
        &dasstrResponse,
        &g_dastrLicNoncePath,
        NULL,
        NULL,
        NULL,
        &dasstrNonce,
        g_chForwardSlash ) );

    ChkArg( dasstrNonce.m_cch == CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) );
    cbNonce = SIZEOF( DRM_ID );
    ChkDR( DRM_B64_DecodeA(
        dastrResponse.pszString,
        &dasstrNonce,
        &cbNonce,
        (DRM_BYTE *)&licNonce,
        0 ) );

    /*
    ** Extract <ResponseID> node from
    ** <AcquireLicenseResponse>/<AcquireLicenseResult>/<Response>/<LicenseResponse> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA(
        dastrResponse.pszString,
        &dasstrResponse,
        &g_dastrLicResponseIdPath,
        NULL,
        NULL,
        NULL,
        &dasstrResponseId,
        g_chForwardSlash ) );

    ChkArg( dasstrResponseId.m_cch == CCH_BASE64_EQUIV( SIZEOF( rgbHashedResponseId ) ) );
    cbResponseId = SIZEOF( rgbHashedResponseId );
    ChkDR( DRM_B64_DecodeA(
        dastrResponse.pszString,
        &dasstrResponseId,
        &cbResponseId,
        (DRM_BYTE *)rgbHashedResponseId,
        0 ) );

    /*
    ** Calculate the Digest Value by SHA256 hashing the device ID and LicenseNonce
    */
    ChkDR( DRM_SHA256_Init( &SHAContext ) );
    ChkDR( DRM_SHA256_Update( &SHAContext,
                              f_pbDeviceId,
                              f_cbDeviceId ) );
    ChkDR( DRM_SHA256_Update( &SHAContext,
                              (DRM_BYTE *)&licNonce,
                              SIZEOF( DRM_ID ) ) );
    ChkDR( DRM_SHA256_Finalize( &SHAContext, &DigestValue ) );

    /*
    ** Expect the calculated SHA256 hash value to be same as Response ID
    */
    if( 0 != MEMCMP( DigestValue.m_rgbDigest, rgbHashedResponseId, SIZEOF( rgbHashedResponseId ) ) )
    {
        ChkDR( DRM_E_XMLSIG_SHA_VERIFY_FAILURE );
    }
#ifdef TEE
	MEMCPY(&f_poLicRespContext->licenseNonce, &licNonce, SIZEOF( licNonce ));
#endif

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_ProcessResponseV3
**
** Synopsis: Function that extracts, processes and stores licenses
**           received from the server. It also processes the revocation
**           data. A transaction ID is returned if found.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_poLicRespContext]     -- Pointer to a data structure that contains
**                             information needed to process licenses.
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that contains
**                             the server response.
** [f_pbTransactionID]      -- Pointer to a buffer that is used to receive
**                             the extracted transaction ID.
** [f_pcbTransactionID]     -- Pointer to a variable that receives the size
**                             (number of bytes) of the extracted transaction ID.
** [f_poLicAcks]            -- Array of structures that reveives the result of
**                             license processing and storage.
** [f_pcLicAcks]            -- Pointer to a variable that:
**                             On input, contains the maximum number of acks that
**                             can be used in f_poLicAcks.
**                             On output, receives the total number
**                             of elements used in the array mentioned above.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the passed in DRM string to
**                          store the transaction ID is not NULL but is not big
**                          enough.
**                          DRM_E_LICACQ_TOO_MANY_LICENSES if *f_pcLicAcks is not large enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LA_ProcessResponseV3(
    __inout                                                DRM_APP_CONTEXT          *f_poAppContext,
    __in                                                   DRM_LICRESP_CONTEXT_V3   *f_poLicRespContext,
    __in                                                   DRM_ANSI_CONST_STRING    *f_pdastrResponse,
    __out_bcount( *f_pcbTransactionID )                    DRM_BYTE                 *f_pbTransactionID,
    __inout                                                DRM_DWORD                *f_pcbTransactionID,
    __out_ecount_part( *f_pcLicAcks, *f_pcLicAcks )        DRM_LICENSE_ACK          *f_poLicAcks,
    __inout                                                DRM_DWORD                *f_pcLicAcks )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_SUBSTRING             dasstrResponse       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING             dasstrTransactionID  = EMPTY_DRM_SUBSTRING;
    const DRM_SLKDATA        *pSlkData             = NULL;
    DRM_SLKDATA               slkData              = { 0 };
    DRM_BOOL                  fValidSLK            = FALSE;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poLicRespContext != NULL );
    ChkDRMANSIString( f_pdastrResponse );
    ChkArg( f_pbTransactionID != NULL );
    ChkArg( f_pcbTransactionID != NULL );
    ChkArg( f_poLicAcks != NULL );
    ChkArg( f_pcLicAcks != NULL );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                        &dasstrResponse ) );

#endif /* DRM_USE_SOAP */

    ChkDR( _ValidateLicenseResponse( f_poLicRespContext, f_pdastrResponse, &dasstrResponse ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_DWORD cProcessed = 0;

        /* Process any revocation information in the server response. */
        ChkDR( DRM_LA_ProcessRevocationPackage(  f_poLicRespContext,
                                                 f_pdastrResponse,
                                                &cProcessed ) );

        if( cProcessed > 0 )
        {
            /*
            ** Make sure the revocation version values cached in DRM manager get
            ** refreshed. No need to clear the fUpdatedRevocationList flag - the
            ** lic eval object will be zeroed out before being populated again
            ** prior to this call.
            */
            MEMCPY( &poAppContextInternal->idCRLsCurrent,
                    &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
                    SIZEOF( DRM_REVLIST_ID_VALUES ) );
        }
    }

    ChkDR( DRM_SYMOPT_LA_ExtractSLKDataFromLicenseResponse( f_poLicRespContext,
                                                            f_pdastrResponse,
                                                            &dasstrResponse,
                                                            &slkData,
                                                            &fValidSLK ) );

    if ( fValidSLK )
    {
        pSlkData = &slkData;
    }

    /* Extract and process all the revocation information in the server response. */
    ChkDR( _ExtractLicensesFromLicenseResponse( f_poLicRespContext,
                                                f_pdastrResponse,
                                                &dasstrResponse,
                                                pSlkData,
                                                f_poLicAcks,
                                                f_pcLicAcks ) );

    /*
    ** Extract <TransactionID> node from
    ** <AcquireLicenseResponse>/<AcquireLicenseResult>/Response/<LicenseResponse>/<Acknowledgement> node.
    */
    dr = DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                    &dasstrResponse,
                                    &g_dastrLicTransactionIDPath,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &dasstrTransactionID,
                                    g_chForwardSlash );

    if ( DRM_SUCCEEDED( dr ) )
    {
        if ( dasstrTransactionID.m_cch <= *f_pcbTransactionID )
        {
            DRM_BYT_CopyBytes( f_pbTransactionID,
                               0,
                               f_pdastrResponse->pszString,
                               dasstrTransactionID.m_ich,
                               dasstrTransactionID.m_cch );

            *f_pcbTransactionID = dasstrTransactionID.m_cch;
        }
        else
        {
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
    }
    else
    {
        *f_pcbTransactionID = 0;

        dr = DRM_SUCCESS;
    }

ErrorExit:

    if ( DRM_FAILED( dr ) && dr != DRM_E_LICACQ_TOO_MANY_LICENSES && f_pcLicAcks != NULL )
    {
        *f_pcLicAcks = 0;
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_CheckForCustomData
**
** Synopsis: Function that checks for custom data in a V3 LACQ response
**           If custom data is present, also verifies the response's signature
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the response
**                             response from the server.
** [f_cbResponse]           -- Size ( number of bytes ) of the response from the
**                             server. It must be bigger than 0.
**
** Returns:                 DRM_SUCCESS if the license has no custom data
**                          DRM_S_MORE_DATA if custom data is available from
**                          the server AND the signature was verified successfully.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**                          Errors from DRM_XMLSIG_VerifySignature
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LA_CheckForCustomData(
    __in_bcount( f_cbResponse ) const DRM_BYTE *f_pbResponse,
    __in                              DRM_DWORD f_cbResponse )
{
    DRM_RESULT              dr              = DRM_SUCCESS;
    DRM_BOOL                fHasCustomData  = FALSE;
    DRM_ANSI_CONST_STRING   dastrResponse   = EMPTY_DRM_STRING;
    DRM_SUBSTRING           dasstrResponse  = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP
    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                         &dasstrResponse ) );
#endif

    /*
    ** Try to detect whether there is custom data from the server.
    */
    if ( DRM_E_BUFFERTOOSMALL == DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                                                f_cbResponse,
                                                                DRM_SOAPXML_PROTOCOL_LIC_ACQ_RESPONSE,
                                                                NULL,
                                                                NULL ) )
    {
        /* Yes there is a custom data string from the server. */
        fHasCustomData = TRUE;
    }

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) && fHasCustomData )
    {
        dr = DRM_S_MORE_DATA;
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_GenerateChallengeV3
**
** Synopsis: Function that generates a license acquisition challenge XML to
**           be sent to the license server using the V3 protocol.
**
** Arguments:
**
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size in bytes
**                             of the challenge buffer during input and receives the
**                             size ( in bytes ) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffers to retrieve
**                          the URL or challenge data are not big enough.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LA_GenerateChallengeV3(
    __in                                DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE               *f_pbChallenge,
    __inout                             DRM_DWORD              *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchXML = 0;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;

    ChkArg( f_poLicChlgContext != NULL );
    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL || *f_pcbChallenge > 0 );

    cchXML = _CalcLicenseChallengeCharCount( f_poLicChlgContext );

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realighnment.
    */
    cchXML += SIZEOF( DRM_DWORD );

    /* Add space used by XML builder context. */
    cchXML += g_cbXMLBuilderMinimumA;

    if ( f_pbChallenge == NULL || *f_pcbChallenge < cchXML )
    {
        *f_pcbChallenge = cchXML;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXML,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildLicenseChallengeXML( poXMLContext,
                                      cbXMLContext,
                                      f_poLicChlgContext,
                                      &dastrXML ) );

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

#endif

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size ( number of bytes ) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_LA_GenerateAckV3
**
** Synopsis: Function that generates a license acquisition acknowledgement
**           XML to be sent to the license server using the V3 protocol.
**
** Arguments:
**
** [f_poOEMContext]         -- Pointer to an OEM specific context.
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_poStackAllocatorContext]
**                          -- Pointer to a stack allocator context.
** [f_poLicenseResponse]    -- Pointer to a structure that contains the license
**                             processing/storage result to send to the server.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size in bytes
**                             of the challenge buffer during input and receives the
**                             size ( in bytes ) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LA_GenerateAckV3(
    __in_opt                            DRM_VOID                    *f_poOEMContext,
    __in                                DRM_BB_CONTEXT              *f_poBBXContext,
    __in                                DRM_STACK_ALLOCATOR_CONTEXT *f_poStackAllocatorContext,
    __in                                DRM_LICENSE_RESPONSE        *f_poLicenseResponse,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE                    *f_pbChallenge,
    __inout                             DRM_DWORD                   *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchXML = 0;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_ANSI_CONST_STRING dastrTransactionID = EMPTY_DRM_STRING;
    DRM_DWORD cchDeviceCert = 0;
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_XML_KEY oXMLKey = EMPTY_XML_KEY;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poBBXContext->fInited );
    ChkArg( f_poStackAllocatorContext != NULL );
    ChkArg( f_poLicenseResponse != NULL );
    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL || *f_pcbChallenge > 0 );

    DASTR_FROM_PB( &dastrTransactionID,
                   f_poLicenseResponse->m_rgbTransactionID,
                   f_poLicenseResponse->m_cbTransactionID );

    cchDeviceCert = _CalcDeviceCertCharCount( f_poBBXContext->pKeyFileContext );

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poOEMContext,
                                   &oXMLKey,
                                   &f_poBBXContext->CryptoContext ) );

    cchXML = _CalcLicenseAcknowledgementCharCount( dastrTransactionID.cchString,
                                                   f_poBBXContext,
                                                   f_poLicenseResponse->m_rgoAcks,
                                                   f_poLicenseResponse->m_cAcks,
                                                   cchDeviceCert,
                                                   &oXMLKey );

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realighnment.
    */
    cchXML += SIZEOF( DRM_DWORD );

    /* Add space used by XML builder context. */
    cchXML += g_cbXMLBuilderMinimumA;

    if ( f_pbChallenge == NULL || *f_pcbChallenge < cchXML )
    {
        *f_pcbChallenge = cchXML;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXML,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildLicenseAcknowledgementXML( poXMLContext,
                                            cbXMLContext,
                                            f_poBBXContext,
                                            f_poStackAllocatorContext,
                                            &dastrTransactionID,
                                            f_poLicenseResponse->m_rgoAcks,
                                            f_poLicenseResponse->m_cAcks,
                                            cchDeviceCert,
                                            &oXMLKey,
                                            &dastrXML ) );

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

#endif

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size ( number of bytes ) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Drm_LicenseAcq_GenerateAck
**
** Synopsis: API that generates a license acquisition acknowledgement
**           XML to be sent to the license server using the V3 protocol.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_poLicenseResponse]    -- Pointer to a structure that contains the license
**                             processing/storage result to send to the server.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size in bytes
**                             of the challenge buffer during input and receives the
**                             size ( in bytes ) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_FALSE if there is no transaction ID.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseAcq_GenerateAck(
    __in                                DRM_APP_CONTEXT      *f_poAppContext,
    __in                                DRM_LICENSE_RESPONSE *f_poLicenseResponse,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE             *f_pbChallenge,
    __inout                             DRM_DWORD            *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_STACK_ALLOCATOR_CONTEXT oStackAllocatorContext = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC_Drm_LicenseAcq_GenerateAck );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_poLicenseResponse != NULL );
    ChkArg( f_poLicenseResponse->m_eType == eV3Protocol );
    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
            *f_pcbChallenge > 0 );

    if ( f_poLicenseResponse->m_cbTransactionID == 0 )
    {
        /*
        ** If there is no transaction ID there is no need to generate the
        ** acknowledgement.
        */
        dr = DRM_S_FALSE;
        goto ErrorExit;
    }

    /*
    ** Clear the previous response fields in the app context.
    */
    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    oStackAllocatorContext.pbStack = poAppContextInternal->pbDRMLicense;
    oStackAllocatorContext.cbStack = poAppContextInternal->cbDRMLicense;

    ChkDR( DRM_LA_GenerateAckV3( poAppContextInternal->pOEMContext,
                                 &poAppContextInternal->oBlackBoxContext,
                                 &oStackAllocatorContext,
                                 f_poLicenseResponse,
                                 f_pbChallenge,
                                 f_pcbChallenge ) );

ErrorExit:

    ChkECC( ECC_Drm_LicenseAcq_GenerateAck, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: Drm_LicenseAcq_ProcessAckResponse
**
** Synopsis: API that processes a license acquisition acknowledgement
**           response from the license server using the V3 protocol.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_pbResponse]           -- Pointer to a buffer that contains the acknowledgement
**                             response from the server.
** [f_cbResponse]           -- Size ( number of bytes ) of the response from the
**                             server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to a variable that receives the
**                             status code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_MORE_DATA if custom data is available from
**                          the server.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseAcq_ProcessAckResponse(
    __in                        const DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse )       DRM_BYTE        *f_pbResponse,
    __in                              DRM_DWORD        f_cbResponse,
    __out_opt                         DRM_RESULT      *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =
        ( const DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_ANSI_CONST_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_BOOL fOverwriteResult = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC_Drm_LicenseAcq_ProcessAckResponse );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    fOverwriteResult = TRUE;

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;
#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                         &dasstrResponse ) );
#endif

    /* Try to detect whether there is custom data from the server. */
    dr = DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                            f_cbResponse,
                                            DRM_SOAPXML_PROTOCOL_LIC_ACQ_ACK_RESPONSE,
                                            NULL,
                                            NULL );
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        /* Yes there is a custom data string from the server. */
        dr = DRM_S_MORE_DATA;
    }
    else
    {
        dr = DRM_SUCCESS;
    }

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );

    ChkECC( ECC_Drm_LicenseAcq_ProcessAckResponse, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/********************************************************************************
** Function:    DRM_LA_ProcessLicenseV3
**
** Synopsis:    Process one license in XMR format. Parses a license
**              and tries to evaluate it to see if it can be stored
**              in the license store.
**
** Arguments:
** [f_poLicRespContext] -- Pointer to a data structure that contains
**                         information needed to process licenses.
** [f_pbResponse]       -- Response string received from license server
** [f_pdasstrLicense]   -- ANSI substring over response string that represents
**                         a single license's data.
** [f_pSLKID]           -- Contains the current SLK ID (optional)
** [f_poLicAck]         -- to receive the reason code
**
** Returns:     DRM_SUCCESS on success, or failed with
**              DRM_E_INVALIDARG,
**              DRM_E_LICENSE_VERSION_NOT_SUPPORTED,
**              DRM_E_INVALID_LICENSE, and
**              error codes from Licesne store.
********************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_pszResponse length defined by f_pdasstrLicense")
DRM_API DRM_RESULT DRM_CALL DRM_LA_ProcessLicenseV3(
    __in                                                                      DRM_LICRESP_CONTEXT_V3 *f_poLicRespContext,
    __inout_ecount( f_pdasstrLicense->m_ich + f_pdasstrLicense->m_cch )       DRM_CHAR               *f_pszResponse,
    __in                                                                const DRM_SUBSTRING          *f_pdasstrLicense,
    __in_opt                                                            const DRM_SLKDATA            *f_pSLKData,
    __out                                                                     DRM_LICENSE_ACK        *f_poLicAck )
PREFAST_POP
{
    DRM_RESULT       dr                   = DRM_SUCCESS;
    DRM_RESULT       drReason             = DRM_SUCCESS;
    DRM_CONST_STRING dstrKID              = EMPTY_DRM_STRING;
    DRM_WCHAR        wchKID[CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) * 2] = { 0 }; /* Ensure we have enough room for a B64 encoded KID, and leave some wiggle room. */
    DRM_DWORD        cchKID               = NO_OF( wchKID );
    DRM_DWORD        dwPriority           = 0;
    DRM_DWORD        cbVersionLicense     = 0;
    DRM_DWORD        cbAlignedLicenseSize = 0;
    DRM_DWORD        cbAlignment          = 0;
    DRM_BYTE         *pbLicenseXMR        = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT stack = { 0 };
    DRM_LICEVAL_CONTEXT *poLicEvalContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC_DRM_LA_ProcessLicenseV3 );

    ChkArg( f_poLicRespContext != NULL );
    ChkArg( f_pszResponse != NULL );
    ChkArg( f_pdasstrLicense != NULL );
    ChkArg( f_pdasstrLicense->m_cch > 0 );
    ChkArg( f_poLicAck != NULL );

    poLicEvalContext = f_poLicRespContext->m_poLicEvalContext;
    ChkArg( poLicEvalContext != NULL );

    drReason = DRM_B64_DecodeA( f_pszResponse,
                                f_pdasstrLicense,
                               &cbVersionLicense,
                                NULL,
                                DRM_BASE64_DECODE_IN_PLACE );

    if ( DRM_FAILED( drReason ) )
    {
        goto ErrorExit;
    }

    /*
    ** Make room to put the priority in front of the license for storage.
    */
    pbLicenseXMR = ( DRM_BYTE* )( f_pszResponse + f_pdasstrLicense->m_ich );

    ChkDR( DRM_UTL_EnsureDataAlignment( pbLicenseXMR,
                                        f_pdasstrLicense->m_cch,
                                       &pbLicenseXMR,
                                       &cbAlignedLicenseSize,
                                        SIZEOF( DRM_DWORD ),
                                       &cbAlignment ) );

    /*
    ** State that after DRM_DWORD right shift and alignment there is room to copy decoded license
    */
    DRMASSERT( SIZEOF( DRM_DWORD ) + cbAlignment + cbVersionLicense <= f_pdasstrLicense->m_cch );

    MEMMOVE( pbLicenseXMR + __CB_DECL( SIZEOF( DRM_DWORD ) ),
             f_pszResponse + f_pdasstrLicense->m_ich,
             cbVersionLicense );

    ZEROMEM( poLicEvalContext->plicenseXMR, SIZEOF( *poLicEvalContext->plicenseXMR ) );

    stack.nStackTop = 0;
    stack.cbStack   = __CB_DECL( DRM_MAX_LICENSESIZE );
    stack.pbStack   = f_poLicRespContext->m_pbLicenseBuffer;

    /*
    ** Clear all flags. Later, if the license is embeddable, we
    ** will set the following flag: DRM_LICENSE_ACK_FLAGS_EMBED
    */
    f_poLicAck->m_dwFlags = 0;

    drReason = DRM_XMR_UnpackLicense( pbLicenseXMR + __CB_DECL( SIZEOF( DRM_DWORD ) ),
                                      cbVersionLicense,
                                      &stack,
                                      poLicEvalContext->plicenseXMR );

    if ( DRM_FAILED( drReason ) || poLicEvalContext->plicenseXMR == NULL )
    {
        f_poLicAck->m_dwResult = DRM_E_INVALID_LICENSE;
        goto ErrorExit;
    }
    if ( poLicEvalContext->plicenseXMR->dwVersion != XMR_VERSION_3 )
    {
        f_poLicAck->m_dwResult = DRM_E_LICENSE_VERSION_NOT_SUPPORTED;
        goto ErrorExit;
    }

    poLicEvalContext->fLicenseIsXMR = TRUE;

    /*
    ** Parse the KID, LID, and priority from the license
    */
    DRM_BYT_CopyBytes( poLicEvalContext->LID.rgb,
                       0,
                       poLicEvalContext->plicenseXMR->pbRightsIdBuffer,
                       poLicEvalContext->plicenseXMR->iRightsId,
                       SIZEOF( poLicEvalContext->LID.rgb ) );

    DRM_BYT_CopyBytes( poLicEvalContext->KID.rgb,
                       0,
                       poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ContentKey.pbguidKeyID,
                       poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ContentKey.iguidKeyID,
                       SIZEOF( poLicEvalContext->KID.rgb ) );

    ChkDR( DRM_B64_EncodeW( poLicEvalContext->KID.rgb,
                            SIZEOF( poLicEvalContext->KID.rgb ),
                            wchKID,
                            &cchKID,
                            0 ) );

    dstrKID.pwszString = wchKID;
    dstrKID.cchString  = cchKID;

    MEMCPY( &f_poLicAck->m_oKID, poLicEvalContext->KID.rgb, DRM_ID_SIZE );

    MEMCPY( &f_poLicAck->m_oLID, poLicEvalContext->LID.rgb, DRM_ID_SIZE );

    if( XMR_IS_PRIORITY_VALID( poLicEvalContext->plicenseXMR ) )
    {
        dwPriority = poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.Priority.dwValue;
    }
    else
    {
        dwPriority = 0;
    }

    drReason = DRM_LA_EvaluateLicense( poLicEvalContext, f_poLicRespContext->m_poDatastore );

    /*
    ** Store the license if the license evaluated ok
    */
    if ( DRM_SUCCEEDED( drReason ) )
    {
        /*
        ** Pre-optimized license
        */
        if ( poLicEvalContext->plicenseXMR->containerOuter.containerKeys.OptimizedContentKey.fValid )
        {
            /*
            ** Licenses that are pre-optimized without an SLK can be stored as-is
            */
            if ( poLicEvalContext->plicenseXMR->containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType
              == XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB_SLK )
            {
                if( DRM_SYMOPT_IsSymOptSupported() )
                {
                    /*
                    ** Domain-bound licenses should never use an SLK
                    */
                    /*
                    ** SLK-optimized device-bound licenses can be re-bound to current BBX
                    */
                    if ( poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.DomainID.fValid
                      || f_pSLKData == NULL )
                    {
                        drReason = DRM_E_INVALID_LICENSE;
                    }
                    else
                    {
                        drReason = DRM_BBX_RebindSymmetricKeyXMR( poLicEvalContext->pcontextBBX,
                                                                 &f_pSLKData->slk,
                                                                  poLicEvalContext->plicenseXMR );
                    }
                }
                else
                {
                    drReason = DRM_E_INVALID_LICENSE;
                }
            }
            else if ( poLicEvalContext->plicenseXMR->containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType
                   != XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB )
            {
                drReason = DRM_E_INVALID_LICENSE;
            }
        }
        else if ( poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ContentKey.wKeyEncryptionCipherType
               == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256 ||
                  poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ContentKey.wKeyEncryptionCipherType
               == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ )
        {
            if( DRM_SYMOPT_IsSymOptSupported() )
            {
                DRM_DWORD        cbSymmKey = 0;
                DRM_BINDING_INFO bindInfo  = { 0 };

                bindInfo.m_fXMRLicense = TRUE;
                bindInfo.m_BindingInfo.m_XMR.m_pxmrLicense      =  poLicEvalContext->plicenseXMR;
                bindInfo.m_BindingInfo.m_XMR.m_pxmrContentKey   = &poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ContentKey;
                bindInfo.m_BindingInfo.m_XMR.m_pxmrDeviceEccKey = &poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ECCKey;
                bindInfo.m_BindingInfo.m_XMR.m_pxmrSignature    = &poLicEvalContext->plicenseXMR->containerOuter.signature;
                bindInfo.m_BindingInfo.m_XMR.m_pxmrDomainID     = &poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.DomainID;

                /*
                ** Re-bind the license to a symmetric key
                */
                drReason = DRM_APPP_AsymmetricToSymmetricKeyXMR( poLicEvalContext->pcontextBBX,
                                                                 &bindInfo,
                                                                 pbLicenseXMR + __CB_DECL( SIZEOF( DRM_DWORD ) ),
                                                                 f_pdasstrLicense->m_cch,
                                                                 &cbVersionLicense,
                                                                 &stack,
                                                                 &cbSymmKey );
            }
        }
        else if ( poLicEvalContext->plicenseXMR->containerOuter.containerKeys.ContentKey.wKeyEncryptionCipherType
               != XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE )
        {
            /*
            ** If not asymmetric or pre-optimized, must be chained
            */
            drReason = DRM_E_INVALID_LICENSE;
        }

        if ( DRM_SUCCEEDED( drReason ) )
        {
            /*
            ** Validity of global policies container is checked
            ** by XMR_IS_RIGHTS_VALID
            */
            if ( XMR_IS_RIGHTS_VALID( poLicEvalContext->plicenseXMR ) &&
                 ( poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.Rights.wValue &
                   XMR_RIGHTS_CANNOT_PERSIST ) == XMR_RIGHTS_CANNOT_PERSIST )
            {
                drReason = DRM_NST_AddLicense( f_poLicRespContext->m_poNonceStoreContext,
                                               cbVersionLicense,
                                               pbLicenseXMR,
                                               &poLicEvalContext->KID,
                                               &poLicEvalContext->LID,
                                               dwPriority );
            }
            else
            {

                drReason = DRM_LST_AddLicense( f_poLicRespContext->m_poLicStoreContext,
                                               cbVersionLicense,
                                               pbLicenseXMR,
                                               &poLicEvalContext->KID,
                                               &poLicEvalContext->LID,
                                               dwPriority );

                if ( DRM_SUCCEEDED( drReason ) && XMR_IS_REMOVAL_DATE_VALID( poLicEvalContext->plicenseXMR ) )
                {
                    /*
                    ** If the license has a valid removal date object then add the removal date
                    ** to the removal date store.
                    */
                    ChkDR( DRM_RDS_AddRemovalDate ( f_poLicRespContext->m_poDatastore,
                                                   &f_poLicRespContext->m_poLicStoreContext->DstNamespaceContext,
                                                    poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.RemovalDate.dwRemovalDate,
                                                   &poLicEvalContext->KID,
                                                   &poLicEvalContext->LID,
                                                    NULL ) );
                }

                /*
                ** Once the license is added to the store, set the DRM_LICENSE_ACK_FLAGS_EMBED flag
                */
                if ( XMR_IS_EMBEDDING_BEHAVIOR_VALID( poLicEvalContext->plicenseXMR )
                    && ( poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue == XMR_EMBEDDING_BEHAVIOR_COPY
                    || poLicEvalContext->plicenseXMR->containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue == XMR_EMBEDDING_BEHAVIOR_MOVE ) )
                {
                    f_poLicAck->m_dwFlags |= DRM_LICENSE_ACK_FLAGS_EMBED;
                }
            }
        }
    }

    f_poLicAck->m_dwResult = drReason;

ErrorExit:
    /*
    ** Fill the data back in with what could be valid base64.
    ** If not the decoded license may
    ** cause further calls into the xml parser to fail.
    */
    if( f_pszResponse != NULL && f_pdasstrLicense != NULL )
    {
        DRM_BYT_SetBytes( f_pszResponse,
                          f_pdasstrLicense->m_ich,
                          f_pdasstrLicense->m_cch,
                          'a' );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

