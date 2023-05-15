/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMDOMAINIMP_C
#include <drmtypes.h>
#include <drmhds.h>
#include <drmutilities.h>
#include <drmxmlparser.h>
#include <drmxmlbuildera.h>
#include <drmcrt.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <drmdomainstore.h>
#include <drmsoapxmlconstants.h>
#include <drmsoapxmlutility.h>
#include <drmdomainconstants.h>
#include <drmdomainimp.h>
#include <drmdomainkeyxmrparser.h>
#include <oem.h>
#include <drmlicacqv3constants.h>
#include <drmantirollbackclockconstants.h>
#include <drmantirollbackclock.h>
#include <drmlicreason.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/* Type ID of the domain datetime stored */
static const DRM_ID g_oDateTimeStoredTypeID =
{
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', 'a'
};

/*********************************************************************
**
** Function: _CalcJoinChallengeDataCharCount
**
** Synopsis: Function that calculates the character count of the portion
**           of data to be encrypted in the domain join challenge, based
**           on the device certificate.
**
** Arguments:
**
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
**
** Returns:                 A long integer value that is the character
**                          count of the calculated portion of data to
**                          be encrypted in the domain join challenge
**                          XML segment. 0 if the device certificate
**                          string is not valid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcJoinChallengeDataCharCount(
    __in DRM_DWORD f_cchDeviceCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    ChkArg( f_cchDeviceCert > 0 );

    /* <Data> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinDataTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <CertificateChain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinCertificateChainTag.cchString,
                                              f_cchDeviceCert,
                                              0,
                                              0 );

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
** Function: _CalcLeaveChallengeDataCharCount
**
** Synopsis: Function that calculates the character count of the portion
**           of data to be encrypted in the domain leave challenge, based
**           on the device certificate.
**
** Arguments:
**
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
**
** Returns:                 A long integer value that is the character
**                          count of the calculated portion of data to
**                          be encrypted in the domain leave challenge
**                          XML segment. 0 if the device certificate
**                          string is not valid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcLeaveChallengeDataCharCount(
    __in DRM_DWORD f_cchDeviceCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;

    ChkArg( f_cchDeviceCert > 0 );

    /* <Data> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveDataTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <CertificateChain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveCertificateChainTag.cchString,
                                              f_cchDeviceCert,
                                              0,
                                              0 );

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
** Function: _CalcJoinChallengeCharCount
**
** Synopsis: Function that calculates the character count of the domain
**           join challenge, based on the domain ID, the device certificate,
**           the friendly name, the custom data and the key information
**           used to encrypt portion of the challenge data.
**
** Arguments:
**
** [f_poDomainID]           -- Pointer to a domain ID.
** [f_poBBXContext]         -- Pointer to the BBX Context
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure that
**                             contains the key information to encrypt
**                             portion of the challenge data.
** [f_cchFriendlyName]      -- Size (number of characters) of a friendly
**                             name string.
** [f_cchCustomData]        -- Size (number of characters) of a custom
**                             data buffer.
**
** Returns:                 A long integer value that is the character
**                          count of the calculated domain join challenge
**                          XML segment. 0 if the device certificate string
**                          or the service ID is not valid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcJoinChallengeCharCount(
    __in const DRM_DOMAIN_ID  *f_poDomainID,
    __in const DRM_BB_CONTEXT *f_poBBXContext,
    __in       DRM_DWORD       f_cchDeviceCert,
    __in const DRM_XML_KEY    *f_poXMLKey,
    __in       DRM_DWORD       f_cchFriendlyName,
    __in       DRM_DWORD       f_cchCustomData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD cchSignatureXMLCount = 0;
    DRM_DWORD cchEncryptedCount = 0;
    DRM_DWORD cbEncryptedSize = 0;

    ChkArg( f_poDomainID != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_cchDeviceCert > 0 );

#if DRM_USE_SOAP

    cchCount += g_dastrXMLRootTag.cchString;

    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();

#endif

    /* <JoinDomain xmlns="..."> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinRootTag.cchString,
                                              0,
                                              g_dastrDomJoinRootAttribName.cchString,
                                              g_dastrDomJoinRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinChallenge2Tag.cchString,
                                              0,
                                              0,
                                              0 );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrDomJoinChallenge2AttribName.cchString,
                                                    g_dastrDomJoinChallenge2AttribValue.cchString );

    /* <Domain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinDomainTag.cchString,
                                              0,
                                              g_dastrDomJoinDomainAttrib1Name.cchString,
                                              g_dastrDomJoinDomainAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( 0,
                                              0,
                                              g_dastrDomJoinDomainAttrib2Name.cchString,
                                              g_dastrDomJoinDomainAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrSOAPPreserveSpaceAttribName.cchString,
                                                    g_dastrSOAPPreserveSpaceAttribValue.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinVersionTag.cchString,
                                              g_dastrDomainJoinVersion.cchString,
                                              0,
                                              0 );

    if ( !DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oServiceID ) )
    {
        /* <ServiceID> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinServiceIDTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                                  0,
                                                  0 );
    }

    if ( !DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oAccountID ) )
    {
        /* <AccountID> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinAccountIDTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                                  0,
                                                  0 );
    }

    if ( f_poDomainID->m_dwRevision != 0 )
    {
        /* <Revision> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinRevisionTag.cchString,
                                                  DRM_SOAPXML_CalcDWORDCharCount( f_poDomainID->m_dwRevision ),
                                                  0,
                                                  0 );
    }

    if ( f_cchFriendlyName > 0 )
    {
        /* <FriendlyName> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinFriendlyNameTag.cchString,
                                                  f_cchFriendlyName,
                                                  0,
                                                  0 );

        /* Possible alignment cost. */
        cchCount += SIZEOF( DRM_WORD );
    }

    if ( f_cchCustomData > 0 )
    {
        /* <CustomData> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomJoinCustomDataTag.cchString,
                                                  f_cchCustomData,
                                                  0,
                                                  0 );

        /* Possible alignment cost. */
        cchCount += SIZEOF( DRM_WORD );
    }

    /* Add the character count of client information XML segment. */
    cchCount += DRM_SOAPXML_CalcClientInfo(f_poBBXContext->pClientInfo);

    /* Calculate the character count of the data to be encrypted. */
    cchEncryptedCount = _CalcJoinChallengeDataCharCount( f_cchDeviceCert );

    cbEncryptedSize = cchEncryptedCount * SIZEOF( DRM_CHAR );

    /* Add the character count of the encrypted data. */
    ChkDR( DRM_XMLSIG_GetEncryptedDataNodeCharCount( f_poXMLKey,
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
** Function: _CalcLeaveChallengeCharCount
**
** Synopsis: Function that calculates the character count of the domain
**           leave challenge, based on the domain ID, the device certificate
**           and the key information used to encrypt portion of the
**           challenge data.
**
** Arguments:
**
** [f_poDomainID]           -- Pointer to a domain ID.
** [f_poBBXContext]         -- Pointer to the BBX Context
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure that
**                             contains the key information to encrypt
**                             portion of the challenge data.
** [f_cchCustomData]        -- Size (number of characters) of a custom
**                             data buffer.
**
** Returns:                 A long integer value that is the character
**                          count of the calculated domain leave challenge
**                          XML segment. 0 if the device certificate string
**                          is not valid or the domain ID is NULL.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _CalcLeaveChallengeCharCount(
    __in const DRM_DOMAIN_ID  *f_poDomainID,
    __in const DRM_BB_CONTEXT *f_poBBXContext,
    __in       DRM_DWORD       f_cchDeviceCert,
    __in const DRM_XML_KEY    *f_poXMLKey,
    __in       DRM_DWORD       f_cchCustomData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchCount = 0;
    DRM_DWORD cchSignatureXMLCount = 0;
    DRM_DWORD cchEncryptedCount = 0;
    DRM_DWORD cbEncryptedSize = 0;

    ChkArg( f_poDomainID != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( !DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oAccountID ) );
    ChkArg( f_cchDeviceCert > 0 );

#if DRM_USE_SOAP

    cchCount += g_dastrXMLRootTag.cchString;

    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();

#endif

    /* <LeaveDomain xmlns="..."> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveRootTag.cchString,
                                              0,
                                              g_dastrDomLeaveRootAttribName.cchString,
                                              g_dastrDomLeaveRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveChallenge2Tag.cchString,
                                              0,
                                              0,
                                              0 );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrDomLeaveChallenge2AttribName.cchString,
                                                    g_dastrDomLeaveChallenge2AttribValue.cchString );

    /* <Domain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveDomainTag.cchString,
                                              0,
                                              g_dastrDomLeaveDomainAttrib1Name.cchString,
                                              g_dastrDomLeaveDomainAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( 0,
                                              0,
                                              g_dastrDomLeaveDomainAttrib2Name.cchString,
                                              g_dastrDomLeaveDomainAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrSOAPPreserveSpaceAttribName.cchString,
                                                    g_dastrSOAPPreserveSpaceAttribValue.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveVersionTag.cchString,
                                              g_dastrDomainLeaveVersion.cchString,
                                              0,
                                              0 );

    /* <AccountID> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveAccountIDTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* Add the B64-ed character count of account ID. */
    cchCount += CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );

    if ( f_cchCustomData > 0 )
    {
        /* <CustomData> */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrDomLeaveCustomDataTag.cchString,
                                                  f_cchCustomData,
                                                  0,
                                                  0 );

        /* Possible alignment cost. */
        cchCount += SIZEOF( DRM_WORD );
    }

    /* Add the character count of client information XML segment. */
    cchCount += DRM_SOAPXML_CalcClientInfo(f_poBBXContext->pClientInfo);

    /* Calculate the character count of the data to be encrypted. */
    cchEncryptedCount = _CalcLeaveChallengeDataCharCount( f_cchDeviceCert );

    cbEncryptedSize = cchEncryptedCount * SIZEOF( DRM_CHAR );

    /* Add the character count of the encrypted data. */
    ChkDR( DRM_XMLSIG_GetEncryptedDataNodeCharCount( f_poXMLKey,
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
** Function: _BuildJoinChallengeDataXML
**
** Synopsis: Function that constructs a sub XML segment to be encrypted in
**           the domain join challenge XML string using the XML builder.
**
** Arguments:
**
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed XML segment string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildJoinChallengeDataXML(
    __in_opt DRM_VOID *f_pOEMContext,
    __in const DRM_BYTE *f_pbDeviceCert,
    __in DRM_DWORD f_cbDeviceCert,
    __out DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_BYTE *pbXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_SUBSTRING dasstrDeviceCert = EMPTY_DRM_SUBSTRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__BuildJoinChallengeDataXML );

    ChkDRMANSIString( f_pdastrXML );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );

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
                                    &g_dastrDomJoinDataTag ) );

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Add and open <CertificateChain> node.
    **
    ** Output:
    ** <Data>
    **      <CertificateChain>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrDomJoinCertificateChainTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add the device certificate.
    **
    ** Output:
    ** <Data>
    **      <CertificateChain>
    **          device certificate
    */

    /* Reserve a buffer starting at a DRM_WORD aligned address. */
    ChkDR( DRM_XMB_ReserveSpaceA( poXMLContext,
                                  SIZEOF( DRM_WORD ),
                                  CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                  &dasstrDeviceCert ) );

    DRM_BYT_MoveBytes( pchXML,
                       dasstrDeviceCert.m_ich,
                       f_pbDeviceCert,
                       0,
                       f_cbDeviceCert );

    /*
    ** Since the space reserved is guaranteed to start at a DRM_WORD aligned address,
    ** using pointer arithmatic is OK.
    */
    ChkDR( DRM_B64_EncodeA( (DRM_BYTE *)pchXML + __CB_DECL( dasstrDeviceCert.m_ich ),
                            f_cbDeviceCert,
                            pchXML + __CB_DECL( dasstrDeviceCert.m_ich ),
                            &dasstrDeviceCert.m_cch,
                            0 ) );

    /*
    ** Close <CertificateChain> node.
    **
    ** Output:
    ** <Data>
    **      <CertificateChain>
    **          ...
    **      </CertificateChain>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );

    /*
    ** Close <Data> node.
    **
    ** Output:
    ** <Data>
    **  ...
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
** Function: _BuildLeaveChallengeDataXML
**
** Synopsis: Function that constructs a sub XML segment to be encrypted in
**           the domain leave challenge XML string using the XML builder.
**
** Arguments:
**
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed XML segment string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildLeaveChallengeDataXML(
    __in_opt DRM_VOID *f_pOEMContext,
    __in const DRM_BYTE *f_pbDeviceCert,
    __in DRM_DWORD f_cbDeviceCert,
    __out DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _XMBContextA *poXMLContext = NULL;
    DRM_BYTE *pbXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_SUBSTRING dasstrDeviceCert = EMPTY_DRM_SUBSTRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__BuildLeaveChallengeDataXML );

    ChkDRMANSIString( f_pdastrXML );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );

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
                                    &g_dastrDomLeaveDataTag ) );

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    /*
    ** Add and open <CertificateChain> node.
    **
    ** Output:
    ** <Data>
    **  ...
    **      <CertificateChain>
    */
    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrDomLeaveCertificateChainTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add the device certificate.
    **
    ** Output:
    ** <Data>
    **  ...
    **      <CertificateChain>
    **          device certificate
    */

    /* Reserve a buffer starting at a DRM_WORD aligned address. */
    ChkDR( DRM_XMB_ReserveSpaceA( poXMLContext,
                                  SIZEOF( DRM_WORD ),
                                  CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                  &dasstrDeviceCert ) );

    DRM_BYT_MoveBytes( pchXML,
                       dasstrDeviceCert.m_ich,
                       f_pbDeviceCert,
                       0,
                       f_cbDeviceCert );

    /*
    ** Since the space reserved is guaranteed to start at a DRM_WORD aligned address,
    ** using pointer arithmatic is OK.
    */
    ChkDR( DRM_B64_EncodeA( (DRM_BYTE *)pchXML + __CB_DECL( dasstrDeviceCert.m_ich ),
                            f_cbDeviceCert,
                            pchXML + __CB_DECL( dasstrDeviceCert.m_ich ),
                            &dasstrDeviceCert.m_cch,
                            0 ) );

    /*
    ** Close <CertificateChain> node.
    **
    ** Output:
    ** <Data>
    **  ...
    **      <CertificateChain>
    **          ...
    **      </CertificateChain>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );

    /*
    ** Close <Data> node.
    **
    ** Output:
    ** <Data>
    **  ...
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
** Function: _BuildJoinChallengeXML
**
** Synopsis: Function that constructs a domain join challenge XML
**           string using the XML builder.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size (number of bytes) of internal buffer
**                             of a XML builder context.
**                             into the signature XML segment.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDomainID]           -- Pointer to a domain ID.
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure that
**                             contains the key information to encrypt
**                             the device certificate.
** [f_pchFriendlyName]      -- Pointer to a character buffer that contains
**                             the friendly name. It can be NULL.
** [f_cchFriendlyName]      -- Size (number of characters) of the friendly
**                             name string.
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pchCustomData]        -- Pointer to a character buffer that contains
**                             the custom data. It can be NULL.
** [f_cchCustomData]        -- Size (number of characters) of the custom
**                             data string.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed domain join challenge
**                             XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildJoinChallengeXML(
    __inout _XMBContextA *f_poXMLContext,
    __in DRM_DWORD f_cbXMLContext,
    __in DRM_BB_CONTEXT *f_poBBXContext,
    __in DRM_DOMAIN_ID *f_poDomainID,
    __in DRM_XML_KEY *f_poXMLKey,
    __in const DRM_CHAR *f_pchFriendlyName,
    __in DRM_DWORD f_cchFriendlyName,
    __in const DRM_BYTE *f_pbDeviceCert,
    __in DRM_DWORD f_cbDeviceCert,
    __in const DRM_CHAR *f_pchCustomData,
    __in DRM_DWORD f_cchCustomData,
    __out DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbServiceID[ __CB_DECL( CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrServiceID = EMPTY_DRM_STRING;
    DRM_BYTE rgbAccountID[ __CB_DECL( CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrAccountID = EMPTY_DRM_STRING;
    DRM_WCHAR rgwchRevision[ DRM_MAX_CCH_BASE10_DWORD_STRING + 1 ] = { 0 };
    DRM_CHAR rgchRevision[ __CB_DECL( DRM_MAX_CCH_BASE10_DWORD_STRING + 1 ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrRevision = EMPTY_DRM_STRING;
    DRM_DWORD cchData = 0;
    DRM_SUBSTRING dasstrCustomData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchCustomData = 0;
    DRM_SUBSTRING dasstrFriendlyName = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchFriendlyName = 0;
    DRM_SUBSTRING dasstrSignedNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrEncryptedDataNode = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrEncryptedXML = EMPTY_DRM_STRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__BuildJoinChallengeXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( ( f_pchFriendlyName == NULL && f_cchFriendlyName == 0 ) ||
            ( f_pchFriendlyName != NULL && f_cchFriendlyName > 0 ) );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );
    ChkArg( ( f_pchCustomData == NULL && f_cchCustomData == 0 ) ||
            ( f_pchCustomData != NULL && f_cchCustomData > 0 ) );
    ChkArg( f_pdastrXML != NULL );

#if DRM_USE_SOAP

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( f_poXMLContext, f_cbXMLContext ) );

    /*
    ** Add and open <JoinDomain> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <JoinDomain ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomJoinRootTag,
                              NULL,
                              &g_dastrDomJoinRootAttribName,
                              &g_dastrDomJoinRootAttribValue,
                              wttOpen ) );

#else
    /*
    ** Add and open <JoinDomain> node.
    **
    ** Output:
    ** <JoinDomain ...>
    */
    ChkDR( DRM_XMB_CreateDocumentA( f_cbXMLContext,
                                    ( DRM_BYTE * )f_poXMLContext,
                                    &g_dastrDomJoinRootTag ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrDomJoinRootAttribName,
                                  &g_dastrDomJoinRootAttribValue ) );
#endif

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomJoinChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    **      <challenge>
    **          <Challenge ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomJoinChallenge2Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrDomJoinChallenge2AttribName,
                                  &g_dastrDomJoinChallenge2AttribValue ) );

    /*
    ** Add and open <Domain> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomJoinDomainTag,
                              NULL,
                              &g_dastrDomJoinDomainAttrib1Name,
                              &g_dastrDomJoinDomainAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrDomJoinDomainAttrib2Name,
                                  &g_dastrDomJoinDomainAttrib2Value ) );


    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPPreserveSpaceAttribName,
                                  &g_dastrSOAPPreserveSpaceAttribValue ) );

    /*
    ** Add and close <Version> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomJoinVersionTag,
                              &g_dastrDomainJoinVersion,
                              NULL,
                              NULL,
                              wttClosed ) );

    if ( !DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oServiceID ) )
    {
        cchData = SIZEOF( rgbServiceID );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poDomainID->m_oServiceID,
                                SIZEOF( DRM_GUID ),
                                ( DRM_CHAR * )rgbServiceID,
                                &cchData,
                                0 ) );

        dastrServiceID.pszString = ( DRM_CHAR * )rgbServiceID;
        dastrServiceID.cchString = cchData;

        /*
        ** Add and close <ServiceID> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <ServiceID>...</ServiceID>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrDomJoinServiceIDTag,
                                  &dastrServiceID,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    if ( !DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oAccountID ) )
    {
        cchData = SIZEOF( rgbAccountID );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poDomainID->m_oAccountID,
                                SIZEOF( DRM_GUID ),
                                ( DRM_CHAR * )rgbAccountID,
                                &cchData,
                                0 ) );

        dastrAccountID.pszString = ( DRM_CHAR * )rgbAccountID;
        dastrAccountID.cchString = cchData;

        /*
        ** Add and close <AccountID> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <AccountID>...</AccountID>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrDomJoinAccountIDTag,
                                  &dastrAccountID,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    if ( f_poDomainID->m_dwRevision != 0 )
    {
        ChkDR( DRM_UTL_NumberToString( f_poDomainID->m_dwRevision,
                                       rgwchRevision,
                                       DRM_MAX_CCH_BASE10_DWORD_STRING + 1,
                                       0, 10, &cchData ) );

        DRMASSERT( cchData <= DRM_MAX_CCH_BASE10_DWORD_STRING );

        if ( cchData > DRM_MAX_CCH_BASE10_DWORD_STRING )
        {
            cchData = DRM_MAX_CCH_BASE10_DWORD_STRING;
        }

        /* Safe to use, input parameter is in ASCII */
        DRM_UTL_DemoteUNICODEtoASCII( rgwchRevision, rgchRevision, cchData + 1 );

        dastrRevision.pszString = rgchRevision;
        dastrRevision.cchString = cchData;

        /*
        ** Add and close <Revision> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <Revision>...</Revision>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrDomJoinRevisionTag,
                                  &dastrRevision,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    /* Estimate the size of the XML encoded friendly name. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchFriendlyName,
                                   f_cchFriendlyName,
                                   NULL,
                                   &cchFriendlyName ) );

    if ( cchFriendlyName > 0 )
    {
        ChkDR( DRM_SOAPXML_EnsureTagValueNativeByteAligned( f_poXMLContext,
                                                            &g_dastrDomJoinFriendlyNameTag ) );

        /*
        ** Add and open <FriendlyName> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <FriendlyName>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrDomJoinFriendlyNameTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContext,
                                      SIZEOF( DRM_BYTE ),
                                      cchFriendlyName,
                                      &dasstrFriendlyName ) );

        ChkDR( DRM_SOAPXML_EncodeData( f_pchFriendlyName,
                                       f_cchFriendlyName,
                                       pchXML + __CB_DECL( dasstrFriendlyName.m_ich ),
                                       &cchFriendlyName ) );

        /*
        ** Close <FriendlyName> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <FriendlyName>...</FriendlyNamea>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    /* Estimate the size of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                   f_cchCustomData,
                                   NULL,
                                   &cchCustomData ) );

    if ( cchCustomData > 0  )
    {
        ChkDR( DRM_SOAPXML_EnsureTagValueNativeByteAligned( f_poXMLContext,
                                                            &g_dastrDomJoinCustomDataTag ) );

        /*
        ** Add and open <CustomData> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <CustomData>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrDomJoinCustomDataTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContext,
                                      SIZEOF( DRM_BYTE ),
                                      cchCustomData,
                                      &dasstrCustomData ) );

        ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                       f_cchCustomData,
                                       pchXML + __CB_DECL( dasstrCustomData.m_ich ),
                                       &cchCustomData ) );

        /*
        ** Close <CustomData> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <JoinDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <CustomData>...</CustomData>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    /*
    ** Construct the client information XML segment.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  ...
    **                  <ClientInfo>
    **                      ...
    **                  </ClientInfo>
    */
    ChkDR( DRM_SOAPXML_BuildClientInfo( f_poXMLContext,
                                        f_poBBXContext->pClientInfo  ) );


    ChkDR( DRM_XMB_GetCurrentBufferPointerA( f_poXMLContext,
                                             TRUE,
                                             ( DRM_CHAR ** )&dastrEncryptedXML.pszString ) );

    dastrEncryptedXML.cchString = _CalcJoinChallengeDataCharCount( CCH_BASE64_EQUIV( f_cbDeviceCert ) );

    ChkDR( _BuildJoinChallengeDataXML( f_poBBXContext->pOEMContext,
                                       f_pbDeviceCert,
                                       f_cbDeviceCert,
                                       ( DRM_ANSI_STRING * )&dastrEncryptedXML ) );

    /* XML encrypt the data. */
    ChkDR( DRM_SOAPXML_EncryptDataWithXMLKey( f_poBBXContext->pOEMContext,
                                              f_poXMLKey,
                                              &dastrEncryptedXML,
                                              &f_poBBXContext->CryptoContext ) );

    /*
    ** Build <EncryptedData> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    **  ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  <EncryptedData>...</<EncryptedData>
    */
    ChkDR( DRM_XMLSIG_BuildEncryptedDataNode( f_poXMLContext,
                                              f_poXMLKey,
                                              dastrEncryptedXML.cchString,
                                              &dasstrEncryptedDataNode ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );

    /* The whole <Domain> node (inclusive) is signed. */
    ChkDR( DRM_XMLSIG_BuildSignatureNode( f_poXMLContext,
                                          f_poBBXContext,
                                          pchXML,
                                          &dasstrSignedNode ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <JoinDomain ...>
    ** ...
    ** </JoinDomain>
    ** [</SOAP headers>] (optional)
    */
    ChkDR( DRM_XMB_CloseDocumentA( f_poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildLeaveChallengeXML
**
** Synopsis: Function that constructs a domain leave challenge XML
**           string using the XML builder.
**
** Arguments:
**
** [f_poXMLContext]         -- Pointer to a XML builder context.
** [f_cbXMLContext]         -- Size (number of bytes) of internal buffer
**                             of a XML builder context.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_poDomainID]           -- Pointer to a domain ID.
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure that
**                             has the key information to encrypt the
**                             device certificate.
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pchCustomData]        -- Pointer to a character buffer that contains
**                             the custom data. It can be NULL.
** [f_cchCustomData]        -- Size (number of characters) of the custom
**                             data string.
** [f_pdastrXML]            -- Pointer to a DRM UTF8/ANSI string that is used
**                             to receive a constructed domain leave challenge
**                             XML string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildLeaveChallengeXML(
    __inout _XMBContextA *f_poXMLContext,
    __in DRM_DWORD f_cbXMLContext,
    __in DRM_BB_CONTEXT *f_poBBXContext,
    __in DRM_DOMAIN_ID *f_poDomainID,
    __in DRM_XML_KEY *f_poXMLKey,
    __in const DRM_BYTE *f_pbDeviceCert,
    __in DRM_DWORD f_cbDeviceCert,
    __in const DRM_CHAR *f_pchCustomData,
    __in DRM_DWORD f_cchCustomData,
    __out DRM_ANSI_STRING *f_pdastrXML )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbAccountID[ __CB_DECL( CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ) ] = { 0 };
    DRM_ANSI_CONST_STRING dastrAccountID = EMPTY_DRM_STRING;
    DRM_DWORD cchData = 0;
    DRM_SUBSTRING dasstrCustomData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchCustomData = 0;
    DRM_SUBSTRING dasstrSignedNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrEncryptedDataNode = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrEncryptedXML = EMPTY_DRM_STRING;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__BuildLeaveChallengeXML );

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );
    ChkArg( ( f_pchCustomData == NULL && f_cchCustomData == 0 ) ||
            ( f_pchCustomData != NULL && f_cchCustomData > 0 ) );
    ChkArg( f_pdastrXML != NULL );

#if DRM_USE_SOAP

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( f_poXMLContext, f_cbXMLContext ) );

    /*
    ** Add and open <LeaveDomain> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <LeaveDomain ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomLeaveRootTag,
                              NULL,
                              &g_dastrDomLeaveRootAttribName,
                              &g_dastrDomLeaveRootAttribValue,
                              wttOpen ) );

#else

    /*
    ** Add and open <LeaveDomain> node.
    **
    ** Output:
    ** <LeaveDomain ...>
    */
    ChkDR( DRM_XMB_CreateDocumentA( f_cbXMLContext,
                                    ( DRM_BYTE * )f_poXMLContext,
                                    &g_dastrDomLeaveRootTag ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrDomLeaveRootAttribName,
                                  &g_dastrDomLeaveRootAttribValue ) );

#endif

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomLeaveChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    **      <challenge>
    **          <Challenge ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomLeaveChallenge2Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrDomLeaveChallenge2AttribName,
                                  &g_dastrDomLeaveChallenge2AttribValue ) );

    /*
    ** Add and open <Domain> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomLeaveDomainTag,
                              NULL,
                              &g_dastrDomLeaveDomainAttrib1Name,
                              &g_dastrDomLeaveDomainAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrDomLeaveDomainAttrib2Name,
                                  &g_dastrDomLeaveDomainAttrib2Value ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPPreserveSpaceAttribName,
                                  &g_dastrSOAPPreserveSpaceAttribValue ) );

    /*
    ** Add and close <Version> node.
    **
    ** Output:
    ** <LeaveDomain ...>
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomLeaveVersionTag,
                              &g_dastrDomainLeaveVersion,
                              NULL,
                              NULL,
                              wttClosed ) );


    cchData = SIZEOF( rgbAccountID );

    ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )&f_poDomainID->m_oAccountID,
                            SIZEOF( DRM_GUID ),
                            ( DRM_CHAR * )rgbAccountID,
                            &cchData,
                            0 ) );

    dastrAccountID.pszString = ( DRM_CHAR * )rgbAccountID;
    dastrAccountID.cchString = cchData;

    /*
    ** Add and close <AccountID> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  ...
    **                  <AccountID>...</AccountID>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrDomLeaveAccountIDTag,
                              &dastrAccountID,
                              NULL,
                              NULL,
                              wttClosed ) );

    /*
    ** Construct the client information XML segment.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  ...
    **                  <ClientInfo>
    **                      ...
    **                  </ClientInfo>
    */
    ChkDR( DRM_SOAPXML_BuildClientInfo( f_poXMLContext,
                                        f_poBBXContext->pClientInfo ) );

    /* Estimate the size of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                   f_cchCustomData,
                                   NULL,
                                   &cchCustomData ) );

    if ( cchCustomData > 0  )
    {
        ChkDR( DRM_SOAPXML_EnsureTagValueNativeByteAligned( f_poXMLContext,
                                                            &g_dastrDomLeaveCustomDataTag ) );


        /*
        ** Add and open <CustomData> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <LeaveDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <CustomData>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrDomLeaveCustomDataTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContext,
                                      SIZEOF( DRM_BYTE ),
                                      cchCustomData,
                                      &dasstrCustomData ) );

        ChkDR( DRM_SOAPXML_EncodeData( f_pchCustomData,
                                       f_cchCustomData,
                                       pchXML + __CB_DECL( dasstrCustomData.m_ich ),
                                       &cchCustomData ) );

        /*
        ** Close <CustomData> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <LeaveDomain ...>
        **      ...
        **      <challenge>
        **          <Challenge ...>
        **              <Domain ...>
        **                  ...
        **                  <CustomData>...</CustomData>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

    ChkDR( DRM_XMB_GetCurrentBufferPointerA( f_poXMLContext,
                                             TRUE,
                                             ( DRM_CHAR ** )&dastrEncryptedXML.pszString ) );

    dastrEncryptedXML.cchString = _CalcJoinChallengeDataCharCount( CCH_BASE64_EQUIV( f_cbDeviceCert ) );

    ChkDR( _BuildLeaveChallengeDataXML( f_poBBXContext->pOEMContext,
                                        f_pbDeviceCert,
                                        f_cbDeviceCert,
                                        ( DRM_ANSI_STRING * )&dastrEncryptedXML ) );

    /* XML encrypt the data. */
    ChkDR( DRM_SOAPXML_EncryptDataWithXMLKey( f_poBBXContext->pOEMContext,
                                              f_poXMLKey,
                                              &dastrEncryptedXML,
                                              &f_poBBXContext->CryptoContext) );

    /*
    ** Build <EncryptedData> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    **  ...
    **      <challenge>
    **          <Challenge ...>
    **              <Domain ...>
    **                  ...
    **                  <EncryptedData>...</<EncryptedData>
    */
    ChkDR( DRM_XMLSIG_BuildEncryptedDataNode( f_poXMLContext,
                                              f_poXMLKey,
                                              dastrEncryptedXML.cchString,
                                              &dasstrEncryptedDataNode ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );

    /* The whole <Domain> node (inclusive) is signed. */
    ChkDR( DRM_XMLSIG_BuildSignatureNode( f_poXMLContext,
                                          f_poBBXContext,
                                          pchXML,
                                          &dasstrSignedNode ) );

    /*
    ** Close all nodes that are still open.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <LeaveDomain ...>
    ** ...
    ** </LeaveDomain>
    ** [</SOAP headers>] (optional)
    */
    ChkDR( DRM_XMB_CloseDocumentA( f_poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


#define DRM_CERTS_NEEDED        2
#define DRM_SERVICE_CERT_INDEX  1



/*********************************************************************
**
** Function: _ProcessJoinDataFromResponse
**
** Synopsis: Function that extracts and processes data from
**           the domain join response. They are: domain ID,
**           domain private key, domain certificate and possibly
**           previous revisions of domain private keys.
**           The extracted data is stored into the domain store.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that
**                             contains a domain join response XML
**                             received from the server.
** [f_poDomainID]           -- Optional pointer to a variable that is used to
**                             receive the domain id parsed out from the domain
**                             join response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ProcessJoinDataFromResponse(
    __in      DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in      DRM_BB_CONTEXT          *f_poBBXContext,
    __inout   DRM_ANSI_STRING         *f_pdastrResponse,
    __out_opt DRM_DOMAIN_ID           *f_poDomainID )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT       dr                             = DRM_SUCCESS;
    DRM_SUBSTRING    dasstrResponse                 = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dasstrServiceID                = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dasstrAccountID                = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dasstrB64CertificateData       = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_STRING  dastrCertificateData           = EMPTY_DRM_STRING;
    DRM_SUBSTRING    dasstrDomain                   = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dasstrVersion                  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dasstrDomainKeys               = EMPTY_DRM_SUBSTRING;
    DRM_DOMAIN_ID    oDomainID                      = EMPTY_DRM_DOMAIN_ID;
    DRM_DWORD        cbSize                         = 0;
    DRM_BYTE        *pbDomainKeysData               = NULL;
    DRM_DWORD        cbDomainKeysData               = 0;
    PUBKEY_P256      oECC256PubKey                  = {0};


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__ProcessJoinDataFromResponse );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkDRMANSIString( f_pdastrResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                         &dasstrResponse ) );

#endif

    /*
    ** Extract <Domain> node from the response
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrResponse,
                                      &g_dastrDomJoinDomainPath,
                                      NULL,
                                      NULL,
                                      &dasstrDomain,
                                      NULL,
                                      g_chForwardSlash ) );

    /*
    ** Extract <Version> node from the <Domain> node
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrDomain,
                                      &g_dastrDomJoinVersionPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrVersion,
                                      g_chForwardSlash ) );
    /*
    ** Ensure that this reply is the correct metering protocol version; it must match
    ** the domain protocol version that we sent in the challenge.
    ** This version # is not to be confused with the fact that the domain protocol is
    ** contained within a V3 SOAP protocol.
    */
    ChkBOOL ( DRM_UTL_DASSTRStringsEqual( f_pdastrResponse->pszString,
                                         &dasstrVersion,
                                         &g_dastrDomainJoinVersion ),
              DRM_E_PROTOCOL_VERSION_NOT_SUPPORTED );


    /*
    ** Extract <DomainKeys> node from the <Domain> node
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrDomain,
                                      &g_dastrDomJoinDomainKeysPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrDomainKeys,
                                      g_chForwardSlash ) );

    /*
    ** Extract <ServiceID> node from the <Domain> node
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrDomain,
                                      &g_dastrDomJoinServiceIDPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrServiceID,
                                      g_chForwardSlash ) );

    cbSize = SIZEOF( DRM_GUID );

    if ( DRM_FAILED( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                                      &dasstrServiceID,
                                      &cbSize,
                                      ( DRM_BYTE * )&oDomainID.m_oServiceID,
                                      0 ) ) )
    {
        ChkDR( DRM_E_DOMAIN_INVALID_GUID );
    }

    ChkBOOL( cbSize == SIZEOF( DRM_GUID ), DRM_E_DOMAIN_INVALID_GUID );

    /*
    ** Extract <AccountID> node from the <Domain> node
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrDomain,
                                      &g_dastrDomJoinAccountIDPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrAccountID,
                                      g_chForwardSlash ) );

    cbSize = SIZEOF( DRM_GUID );

    if ( DRM_FAILED( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                                      &dasstrAccountID,
                                      &cbSize,
                                      ( DRM_BYTE * )&oDomainID.m_oAccountID,
                                      0 ) ) )
    {
        ChkDR( DRM_E_DOMAIN_INVALID_GUID );
    }

    ChkBOOL( cbSize == SIZEOF( DRM_GUID ), DRM_E_DOMAIN_INVALID_GUID );


    /*
    ** Extract <CertificateChain> node from the <Domain> node
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      &dasstrDomain,
                                      &g_dastrDomJoinCertificateChainPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrB64CertificateData,
                                      g_chForwardSlash ) );

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                            &dasstrB64CertificateData,
                            &dastrCertificateData.cchString,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );

    if ( dasstrB64CertificateData.m_ich % CB_NATIVE_BYTE > 0 )
    {
        /*
        ** The code is running on a 16 bits/byte system and unfortunately
        ** dasstrB64CertificateData starts in the middle of a native
        ** byte. Need to shift the whole substring specified by
        ** dasstrB64CertificateData one DRM_BYTE to the higher address.
        ** It should be OK since in the worst case (assuming the previous B64
        ** decode does not leave any extra byte, which is highly unlikely),
        ** we will overwrite the first byte of the </EncryptedData> tag, which
        ** is OK since the whole <EncryptedData> node (including the enclosing
        ** tags) will be reset to blank strings before the function exits.
        */
        DRM_BYT_MoveBytes( f_pdastrResponse->pszString,
                           dasstrB64CertificateData.m_ich + 1,
                           f_pdastrResponse->pszString,
                           dasstrB64CertificateData.m_ich,
                           dastrCertificateData.cchString );

        dasstrB64CertificateData.m_ich += 1;
    }

    dastrCertificateData.pszString =
            f_pdastrResponse->pszString + __CB_DECL( dasstrB64CertificateData.m_ich );

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                            &dasstrDomainKeys,
                            &cbDomainKeysData,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE) );

    pbDomainKeysData = (DRM_BYTE*)f_pdastrResponse->pszString + dasstrDomainKeys.m_ich;

    /* Process and store the domain keys */
    ChkDR( DRM_BBX_StoreDomainPrivateKeys( f_poBBXContext,
                                           f_poDomainStoreContext,
                                           pbDomainKeysData,
                                           cbDomainKeysData,
                                          &oDomainID,
                                          &oECC256PubKey,
                                          &dastrCertificateData) );


    /* Return the domain id if f_poDomainID is not NULL.*/
    if ( f_poDomainID != NULL )
    {
        MEMCPY( f_poDomainID, &oDomainID, SIZEOF( DRM_DOMAIN_ID ) );
    }

ErrorExit:

    /*
    ** Set all bytes of the certificate and domainkeys data portions of the original
    ** response XML string to ' ' to prevent future parsing error
    ** of the original response XML string.
    */
    if( NULL != f_pdastrResponse
     && NULL != f_pdastrResponse->pszString )
    {
        if( dasstrB64CertificateData.m_ich != 0 )
        {
            DRM_BYT_SetBytes( ( DRM_BYTE * )f_pdastrResponse->pszString,
                              dasstrB64CertificateData.m_ich,
                              dasstrB64CertificateData.m_cch,
                              ' ' );
        }

        if( dasstrDomainKeys.m_ich != 0 )
        {
            DRM_BYT_SetBytes( ( DRM_BYTE * )f_pdastrResponse->pszString,
                              dasstrDomainKeys.m_ich,
                              dasstrDomainKeys.m_cch,
                              ' ' );
        }
    }
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _GetLatestRevision
**
** Synopsis: Function that retrieve the latest revision number from
**           the domain store and updated the revision field of the
**           passed in domain ID, if the account ID of the passed in
**           domain ID is not empty.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context that has already
**                             been opened.
** [f_poDomainID]           -- Domain ID whose revision field is to be updated.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _GetLatestRevision(
    __in    DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout DRM_DOMAIN_ID *f_poDomainID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwMaxRevision = 0, dwKeyRevision = 0;
    DRM_DOMAINSTORE_ENUM_CONTEXT oDomainStoreEnumContext = { 0 };
    DRM_GUID oAccountID = EMPTY_DRM_GUID;

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poDomainID != NULL );

    if ( DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oAccountID ) )
    {
        f_poDomainID->m_dwRevision = 0;

        goto ErrorExit;
    }

    ChkDR( DRM_DOMST_InitEnumKey( f_poDomainStoreContext,
                                  &oDomainStoreEnumContext ) );

    for ( ; ; )
    {
        dr = DRM_DOMST_EnumNextKey( &oDomainStoreEnumContext,
                                    &oAccountID,
                                    &dwKeyRevision,
                                    NULL,
                                    NULL );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        if ( MEMCMP( &oAccountID, &f_poDomainID->m_oAccountID, SIZEOF( DRM_GUID ) ) != 0 )
        {
            /*
            ** Skip those domain private keys that do not match
            ** the passed in account ID.
            */
            continue;
        }

        if ( dwKeyRevision > dwMaxRevision )
        {
            dwMaxRevision = dwKeyRevision;
        }
    }

    f_poDomainID->m_dwRevision = dwMaxRevision;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _ParseJoinDomainChallengeQueryData
**
** Synopsis: This function Parses the Join Domain Challenge Query Data and populates the
**                the Domain Account ID, Friendly name and Custom data.
**
** Arguments:
** [f_pchQueryData]     -- The XML Join domain Query data in ANSI of the form:
**                         <JoinDomain type = \"challenge\">
**                           <Data>
**                             <ServiceID>Optional Base 64 encoded Service Account ID string in ANSI</ServiceID>
**                             <AccountID>Optional Base 64 encoded Domain Account ID string in ANSI</AccountID>
**                             <Revision>Optional Domain Revision</Revision>
**                             <FriendlyName>Optional Friendly Name in ANSI </FriendlyName>
**                             <CustomData>Optional Custom Data in ANSI</CustomData>
**                           </Data>
**                         </JoinDomain>
**
** [f_cchQueryData]      -- Size (in characters) of the Query Data Buffer.
**
** [f_poDomainID]        -- Domain ID in the sent in the challenge.
**
** [f_ppchFriendlyName]  -- Pointer to pointer which will point to the Friendly name in the XML.
**
** [f_pcchFriendlyName]  -- Size (number of characters) of the friendly name string.
**
** [f_ppchCustomData]    -- Pointer pointer which will point to the Custom data in the XML.
**
** [f_pcchCustomData]    -- Size (in characters) of the custom data buffer.
**
** Returns:              DRM_SUCCESS on success.
**                       DRM_E_INVALIDARG if the arguments are invalid.
**                       DRM_E_DOMAIN_INVALID_CUSTOM_DATA if the custom data cannot be parsed.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _ParseJoinDomainChallengeQueryData(
    __in     const DRM_CHAR        *f_pchQueryData,
    __in           DRM_DWORD        f_cchQueryData,
    __inout        DRM_DOMAIN_ID   *f_poDomainID,
    __inout        DRM_CHAR const **f_ppchFriendlyName,
    __inout        DRM_DWORD       *f_pcchFriendlyName,
    __inout        DRM_CHAR const  **f_ppchCustomData,
    __inout        DRM_DWORD       *f_pcchCustomData,
    __in_opt const DRM_BYTE        *f_pbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbServiceId = DRM_ID_SIZE;
    DRM_DWORD cbAccountId = DRM_ID_SIZE;
    DRM_SUBSTRING dsstrData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrSid = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrAid = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrRev = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrFname = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrCdata = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dsstrJdomain = EMPTY_DRM_SUBSTRING;
    DRM_BYTE *pbFreeBuffer = (DRM_BYTE*) f_pchQueryData;
    DRM_DWORD cbFreeBuffer = f_cchQueryData;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__ParseJoinDomainChallengeQueryData );

    ChkArg( f_poDomainID != NULL );
    ChkArg( f_ppchFriendlyName != NULL );
    ChkArg( f_pcchFriendlyName != NULL );
    ChkArg( f_ppchCustomData != NULL );
    ChkArg( f_pcchCustomData != NULL );
    ChkArg( f_pchQueryData != NULL &&
            f_cchQueryData > 0 );

    dsstrJdomain.m_ich = 0;
    dsstrJdomain.m_cch = f_cchQueryData;

    /* Look for <JoinDomain> tag */
    ChkDR( DRM_XML_GetNodeA( f_pchQueryData,
                             &dsstrJdomain,
                             &g_dastrDomJoinRootTag,
                             NULL,
                             NULL,
                             0,
                             &dsstrJdomain,
                             NULL ) );

    /* Get the <Data> node */
    ChkDR( DRM_XML_GetSubNodeA( f_pchQueryData,
                                &dsstrJdomain,
                                &g_dastrDomJoinDataTag,
                                NULL,
                                NULL,
                                0,
                                &dsstrData,
                                NULL,
                                1 ) );

    /*Get the <ServiceID> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                              &dsstrData,
                              &g_dastrDomJoinServiceIDTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dsstrSid,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrSid.m_cch != 0)
    {
        ChkDR(dr);

        /*Base 64 decode the Service ID and set it in the Domain ID struct */
        ChkDR( DRM_B64_DecodeA( f_pchQueryData,
                                &dsstrSid,
                                &cbServiceId,
                                (DRM_BYTE*)&(f_poDomainID->m_oServiceID),
                                0 ) );
    }

    /*Get the <AccountID> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                              &dsstrData,
                              &g_dastrDomJoinAccountIDTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dsstrAid,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrAid.m_cch != 0)
    {
        ChkDR(dr);

        /*Base 64 decode the Account ID and set it in the Domain ID struct */
        ChkDR( DRM_B64_DecodeA( f_pchQueryData,
                                &dsstrAid,
                                &cbAccountId,
                                (DRM_BYTE*)&(f_poDomainID->m_oAccountID),
                                0 ) );
    }

    /*Get the <Revision> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                              &dsstrData,
                              &g_dastrDomJoinRevisionTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dsstrRev,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrRev.m_cch != 0)
    {
        ChkDR(dr);

        ChkDR( DRMCRT_AtoDWORD(
            f_pchQueryData + dsstrRev.m_ich,
            dsstrRev.m_cch,
            10,
            &f_poDomainID->m_dwRevision ) );
    }

    /*Get the <FriendlyName> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                              &dsstrData,
                              &g_dastrDomJoinFriendlyNameTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dsstrFname,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrFname.m_cch != 0)
    {
        ChkDR(dr);

        *f_ppchFriendlyName = f_pchQueryData + __CB_DECL( dsstrFname.m_ich );
        *f_pcchFriendlyName = dsstrFname.m_cch;
    }
    else
    {
        *f_ppchFriendlyName = NULL;
        *f_pcchFriendlyName = 0;
    }

    /*Get the <CustomData> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                              &dsstrData,
                              &g_dastrDomJoinCustomDataTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dsstrCdata,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrCdata.m_cch != 0)
    {
        ChkDR(dr);

        *f_ppchCustomData = f_pchQueryData + __CB_DECL( dsstrCdata.m_ich );
        *f_pcchCustomData = dsstrCdata.m_cch;
    }
    else
    {
        dr = DRM_SUCCESS;

        *f_ppchCustomData = NULL;
        *f_pcchCustomData = 0;
    }

    if ( f_pbChallenge != NULL )
    {
        /*
          Since the Challenge Buffer has been allocated, we can reuse the
          Query data memory as free buffer to ensure WORD alignment.
          Copy the friendly name and customdata to start at a WORD boundary.
          by overwriting into the buffer we got for the Query data. The Query
          data buffer always starts at the WORD boundary.
        */

        if ( *f_ppchFriendlyName != NULL )
        {
            DRM_BYT_CopyBytes( pbFreeBuffer,
                               0,
                               *f_ppchFriendlyName,
                               0,
                               *f_pcchFriendlyName );

            *f_ppchFriendlyName = ( const DRM_CHAR * )pbFreeBuffer;

            pbFreeBuffer = pbFreeBuffer + __CB_DECL( *f_pcchFriendlyName );
            cbFreeBuffer = cbFreeBuffer - *f_pcchFriendlyName;

            ChkDR( DRM_UTL_EnsureDataAlignment( pbFreeBuffer,
                                                cbFreeBuffer,
                                                &pbFreeBuffer,
                                                &cbFreeBuffer,
                                                SIZEOF( DRM_WORD ),
                                                NULL ) );
        }

        if ( *f_ppchCustomData != NULL )
        {
            DRM_BYT_CopyBytes( pbFreeBuffer,
                               0,
                               *f_ppchCustomData,
                               0,
                               *f_pcchCustomData );

            *f_ppchCustomData = ( const DRM_CHAR * )pbFreeBuffer;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    if ( DRM_FAILED( dr ) && dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_E_DOMAIN_INVALID_CUSTOM_DATA;
    }

    return dr;
}

/*********************************************************************
**
** Function: _ParseLeaveDomainChallengeQueryData
**
** Synopsis: This function Parses the Leave Domain Challenge Query Data and populates the
**           the Domain Account ID and Custom data.
**
** Arguments:
** [f_pchQueryData]     -- The XML Join domain Query data in ANSI of the form:
**                         <LeaveDomain type = \"challenge\">
**                           <Data>
**                             <ServiceID>Optional Base 64 encoded Service Account ID string in ANSI</ServiceID>
**                             <AccountID>Mandatory Base 64 encoded Domain Account ID string in ANSI</AccountID>
**                             <Revision>Optional Domain Revision</Revision>
**                             <CustomData>Optional Custom Data in ANSI</CustomData>
**                           </Data>
**                         </LeaveDomain>
**
** [f_cchQueryData]      -- Size (in characters) of the Query Data Buffer.
**
** [f_poDomainID]        -- Domain ID in the sent in the challenge.
**
** [f_ppchCustomData]    -- Pointer pointer which will point to the Custom data in the XML.
**
** [f_pcchCustomData]    -- Size (in characters) of the custom data buffer.
**
** Returns:              DRM_SUCCESS on success.
**                       DRM_E_INVALIDARG if the arguments are invalid.
**                       DRM_E_DOMAIN_INVALID_CUSTOM_DATA if the custom data cannot be parsed.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _ParseLeaveDomainChallengeQueryData(
    __in     const DRM_CHAR        *f_pchQueryData,
    __in           DRM_DWORD        f_cchQueryData,
    __inout        DRM_DOMAIN_ID   *f_poDomainID,
    __inout        DRM_CHAR const **f_ppchCustomData,
    __inout        DRM_DWORD       *f_pcchCustomData,
    __in_opt const DRM_BYTE        *f_pbChallenge )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_DWORD        cbServiceId   = DRM_ID_SIZE;
    DRM_DWORD        cbAccountId   = DRM_ID_SIZE;
    DRM_SUBSTRING    dsstrData     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dsstrSid      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dsstrAid      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dsstrRev      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dsstrCdata    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING    dsstrLdomain  = EMPTY_DRM_SUBSTRING;
    DRM_BYTE        *pbFreeBuffer  = (DRM_BYTE*) f_pchQueryData;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC__ParseLeaveDomainChallengeQueryData );

    ChkArg( f_poDomainID       != NULL );
    ChkArg( f_ppchCustomData   != NULL );
    ChkArg( f_pcchCustomData   != NULL );
    ChkArg( f_pchQueryData     != NULL
         && f_cchQueryData     >  0 );

    dsstrLdomain.m_ich = 0;
    dsstrLdomain.m_cch = f_cchQueryData;

    /* Look for <LeaveDomain> tag */
    ChkDR( DRM_XML_GetNodeA( f_pchQueryData,
                            &dsstrLdomain,
                            &g_dastrDomLeaveRootTag,
                             NULL,
                             NULL,
                             0,
                            &dsstrLdomain,
                             NULL ) );

    /* Get the <Data> node */
    ChkDR( DRM_XML_GetSubNodeA( f_pchQueryData,
                               &dsstrLdomain,
                               &g_dastrDomJoinDataTag,
                                NULL,
                                NULL,
                                0,
                               &dsstrData,
                                NULL,
                                1 ) );

    /*Get the <ServiceID> node from the <Data> node which is a mandatory tag.*/
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                             &dsstrData,
                             &g_dastrDomJoinServiceIDTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrSid,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrSid.m_cch != 0)
    {
        ChkDR(dr);

        /*Base 64 decode the Service ID and set it in the Domain ID struct */
        ChkDR( DRM_B64_DecodeA( f_pchQueryData,
                               &dsstrSid,
                               &cbServiceId,
                                (DRM_BYTE*)&(f_poDomainID->m_oServiceID),
                                0 ) );
    }

    /*Get the <AccountID> node from the <Data> node which is a mandatory tag.*/
    ChkDR( DRM_XML_GetSubNodeA( f_pchQueryData,
                             &dsstrData,
                             &g_dastrDomJoinAccountIDTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrAid,
                              1 ) );

    if( dsstrAid.m_cch == 0)
    {
        ChkDR( DRM_E_DOMAIN_INVALID_GUID );
    }
    else
    {
        /*Base 64 decode the Account ID and set it in the Domain ID struct */
        ChkDR( DRM_B64_DecodeA( f_pchQueryData,
                               &dsstrAid,
                               &cbAccountId,
                                (DRM_BYTE*)&(f_poDomainID->m_oAccountID),
                                0 ) );
    }

    /*Get the <Revision> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                              &dsstrData,
                              &g_dastrDomJoinRevisionTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                              &dsstrRev,
                              1 );

    if ( dr != DRM_E_XMLNOTFOUND && dsstrRev.m_cch != 0)
    {
        ChkDR(dr);

        ChkDR( DRMCRT_AtoDWORD(
            f_pchQueryData + dsstrRev.m_ich,
            dsstrRev.m_cch,
            10,
            &f_poDomainID->m_dwRevision ) );
    }

    /*Get the <CustomData> node from the <Data> node if available since it is optional */
    dr = DRM_XML_GetSubNodeA( f_pchQueryData,
                             &dsstrData,
                             &g_dastrDomJoinCustomDataTag,
                              NULL,
                              NULL,
                              0,
                              NULL,
                             &dsstrCdata,
                              1 );

    if( dr != DRM_E_XMLNOTFOUND && dsstrCdata.m_cch != 0)
    {

        ChkDR(dr);
        *f_ppchCustomData = f_pchQueryData + __CB_DECL( dsstrCdata.m_ich );
        *f_pcchCustomData = dsstrCdata.m_cch;
    }
    else
    {
        dr = DRM_SUCCESS;
        *f_ppchCustomData = NULL;
        *f_pcchCustomData = 0;
    }

    if( f_pbChallenge != NULL )
    {
        /*
          Since the Challenge Buffer has been allocated, we can reuse the
          Query data memory as free buffer to ensure WORD alignment.
          Copy the customdata to start at a WORD boundary by overwriting
          into the buffer we got for the Query data. The QueryData buffer
          always starts at the WORD boundary.
        */

        if( *f_ppchCustomData != NULL )
        {
            DRM_BYT_CopyBytes( pbFreeBuffer,
                               0,
                               *f_ppchCustomData,
                               0,
                               *f_pcchCustomData );

            *f_ppchCustomData = (const DRM_CHAR*)pbFreeBuffer;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    if ( DRM_FAILED( dr ) && dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_E_DOMAIN_INVALID_CUSTOM_DATA;
    }

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_GenerateJoinChallenge
**
** Synopsis: API that generates a domain join challenge to be sent to the
**           domain server to register a device to a domain. The challenge
**           includes the  identity of the device to be registered and
**           optionally the domain ID to which the device wants to be
**           registered.
**
** Arguments:
**
** [f_poOEMContext]         -- Pointer to an optional OEM context structure.
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_poDomainStoreContext] -- Pointer to a domain store context that has already
**                             been opened.
** [f_dwFlags]              -- Flag that indicates the type of custom data.
** [f_poDomainID]           -- Domain ID to be registered with the server. Some of
**                             its fields (service ID, account ID or revision)
**                             may be blank.
** [f_pchFriendlyName]      -- Pointer to a character buffer that contains
**                             the friendly name. It can be NULL.
** [f_cchFriendlyName]      -- Size (number of characters) of the friendly
**                             name string.
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pchData]              -- Pointer to a buffer that contains the custom data
**                             sent to the server. The format of the custom data
**                             is based on the value of f_dwFlags. It may be NULL.
**                             If the Query data is passed in with f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA
**                             then it should have the following XML Format:
**                             <JoinDomain type = \"challenge\">
**                              <Data>
**                               <ServiceID>Optional Base 64 encoded Service Account ID string in ANSI</ServiceID>
**                               <AccountID>Optional Base 64 encoded Domain Account ID string in ANSI</AccountID>
**                               <Revision>Optional Domain Revision</Revision>
**                               <FriendlyName>Optional Friendly Name in ANSI </FriendlyName>
**                               <CustomData>Optional Custom Data in ANSI</CustomData>
**                              </Data>
**                             </JoinDomain>
** [f_cchData]              -- Size (in characters) of the custom/query data buffer.
**                             It must be 0 if f_pchCustomData is NULL.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns
**                             an error and the size of the required buffer is
**                             stored in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size in bytes
**                             of the challenge buffer during input and receives the
**                             size (in bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE if the custom data
**                          type is incorrect.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA if the custom data cannot
**                          be parsed.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOM_GenerateJoinChallenge(
    __in_opt                                   DRM_VOID                 *f_poOEMContext,
    __in                                       DRM_BB_CONTEXT           *f_poBBXContext,
    __in                                       DRM_DOMAINSTORE_CONTEXT  *f_poDomainStoreContext,
    __in                                       DRM_DWORD                 f_dwFlags,
    __in_opt                                   DRM_DOMAIN_ID            *f_poDomainID,
    __in_ecount_opt( f_cchFriendlyName ) const DRM_CHAR                 *f_pchFriendlyName,
    __in                                       DRM_DWORD                 f_cchFriendlyName,
    __in_bcount_opt( f_cbDeviceCert )    const DRM_BYTE                 *f_pbDeviceCert,
    __in                                       DRM_DWORD                 f_cbDeviceCert,
    __in_ecount_opt( f_cchData)          const DRM_CHAR                 *f_pchData,
    __in                                       DRM_DWORD                 f_cchData,
    __out_bcount_opt( *f_pcbChallenge )        DRM_BYTE                 *f_pbChallenge,
    __inout                                    DRM_DWORD                *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchXMLContext = 0;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_XML_KEY oXMLKey = EMPTY_XML_KEY;
    DRM_DWORD cchEncodedCustomData = 0;
    DRM_DWORD cchEncodedFriendlyName = 0;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_GUID;
    const DRM_CHAR *pchCustomData = NULL;
    DRM_DWORD cchCustomData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_GenerateJoinChallenge );

    ChkArg( f_poBBXContext != NULL );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkBOOL( f_dwFlags == DRM_REGISTER_NULL_DATA ||
             f_dwFlags == DRM_REGISTER_CUSTOM_DATA ||
             f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA,
             DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE );

    ChkArg( f_cbDeviceCert > 0 );

    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
            *f_pcbChallenge > 0 );

    if( f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA )
    {
        ChkArg( f_pchData != NULL && f_cchData > 0 );
        f_poDomainID  = &oDomainID;
        ChkDR( _ParseJoinDomainChallengeQueryData( f_pchData,
                                                   f_cchData,
                                                   f_poDomainID,
                                                   &f_pchFriendlyName,
                                                   &f_cchFriendlyName,
                                                   &pchCustomData,
                                                   &cchCustomData,
                                                   f_pbChallenge ) );

    }
    else
    {
        ChkArg( f_poDomainID != NULL );

        ChkArg( ( f_pchFriendlyName == NULL && f_cchFriendlyName == 0 )
              ||( f_pchFriendlyName != NULL && f_cchFriendlyName > 0 ) );

        ChkArg( ( f_pchData == NULL && f_cchData == 0 )
              ||( f_pchData != NULL && f_cchData > 0 ) );

        /* The data f_pchData carries custom data */
        pchCustomData = f_pchData;
        cchCustomData = f_cchData;
     }

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poOEMContext, &oXMLKey, &f_poBBXContext->CryptoContext ) );

    /* Get the size (number of characters) of the XML encoded friendly name. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchFriendlyName,
                                   f_cchFriendlyName,
                                   NULL,
                                   &cchEncodedFriendlyName ) );

    /* Get the size (number of characters) of the XML encoded friendly name. */
    ChkDR( DRM_SOAPXML_EncodeData( f_pchFriendlyName,
                                   f_cchFriendlyName,
                                   NULL,
                                   &cchEncodedFriendlyName ) );

    /* Get the size (number of characters) of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( pchCustomData,
                                   cchCustomData,
                                   NULL,
                                   &cchEncodedCustomData ) );

    cchXMLContext = _CalcJoinChallengeCharCount( f_poDomainID,
                                                 f_poBBXContext,
                                                 CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                                 &oXMLKey,
                                                 cchEncodedFriendlyName,
                                                 cchEncodedCustomData );

    /* Add space used by XML builder context. */
    cchXMLContext += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realighnment.
    */
    cchXMLContext += SIZEOF( DRM_DWORD );

    if ( f_pbChallenge == NULL || *f_pcbChallenge < cchXMLContext )
    {
        *f_pcbChallenge = cchXMLContext;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if ( f_poDomainID->m_dwRevision == 0 )
    {
        ChkDR( _GetLatestRevision( f_poDomainStoreContext, f_poDomainID ) );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXMLContext,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildJoinChallengeXML( poXMLContext,
                                   cbXMLContext,
                                   f_poBBXContext,
                                   f_poDomainID,
                                   &oXMLKey,
                                   f_pchFriendlyName,
                                   f_cchFriendlyName,
                                   f_pbDeviceCert,
                                   f_cbDeviceCert,
                                   pchCustomData,
                                   cchCustomData,
                                   &dastrXML ) );

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

#endif

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( &oXMLKey, SIZEOF( DRM_XML_KEY ) );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_ProcessJoinResponse
**
** Synopsis: API that processes a domain join response sent by the domain
**           registration server. The domain response contains a domain
**           certificate and one or more domain private key that will be
**           added to the domain store by the device that calls the API.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_poDomainStoreContext] -- Pointer to a domain store context that has already
**                             been opened.
** [f_pbResponse]           -- Pointer to a buffer that contains the domain
**                             join response from the domain registration
**                             server.
** [f_cbResponse]           -- Size of the domain join response from the domain
**                             registration server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to a variable that receives the
**                             status code.
** [f_poDomainID]           -- Optional pointer to a variable that is used to
**                             receive the domain id parsed out from the domain
**                             join response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_MORE_DATA if custom data is available from
**                          the server.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_XML_FORMAT if SOAP format error happens.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOM_ProcessJoinResponse(
    __in                           DRM_BB_CONTEXT          *f_poBBXContext,
    __in                           DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE                *f_pbResponse,
    __in                           DRM_DWORD                f_cbResponse,
    __out                          DRM_RESULT              *f_pResult,
    __out_opt                      DRM_DOMAIN_ID           *f_poDomainID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_BOOL fOverwriteResult = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_ProcessJoinResponse );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    fOverwriteResult = TRUE;

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    /*
    ** Process the domain join response and extract the domain ID if
    ** the caller provides a non NULL pointer of a domain ID variable.
    */
    ChkDR( _ProcessJoinDataFromResponse( f_poDomainStoreContext,
                                         f_poBBXContext,
                                         &dastrResponse,
                                         f_poDomainID ) );

    /* Try to detect whether there is custom data from the server. */
    dr = DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                                 f_cbResponse,
                                                 DRM_SOAPXML_PROTOCOL_DOMAIN_JOIN_RESPONSE,
                                                 NULL,
                                                 NULL );


    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        /* Yes, there is a custom data string from the server. */
        dr = DRM_S_MORE_DATA;
    }
    else if ( dr == DRM_E_XMLNOTFOUND )
    {
        /* No, there is not a custom data string from the server. This is OK. */
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
** Function: DRM_DOM_GenerateLeaveChallenge
**
** Synopsis: API that generates a domain leave challenge to be sent to the
**           domain server to unregister a device from a domain. The challenge
**           includes the identity of the device to be unregistered and
**           the domain ID to which the device wants to be unregistered.
**           The device will remove the domain certificate and the domain
**           private keys from its domain store.
**
** Arguments:
**
** [f_poOEMContext]         -- Pointer to an optional OEM context structure.
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_poDomainStoreContext] -- Pointer to a domain store context that has
**                             already been opened.
** [f_dwFlags]              -- Flag that indicates the type of custom data.
** [f_poDomainID]           -- Domain ID to be unregistered with the server. Some of
**                             its fields (service ID, account ID or revision)
**                             may be blank.
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
** [f_pchData]              -- Pointer to a buffer that contains the custom data
**                             sent to the server. The format of the custom data
**                             is based on the value of f_dwFlags. It may be NULL.
**                             If the Query data is passed in with f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA
**                             then it should have the following XML Format:
**                             <LeaveDomain type = \"challenge\">
**                              <Data>
**                               <ServiceID>Optional Base 64 encoded Service Account ID string in ANSI</ServiceID>
**                               <AccountID>Mandatory Base 64 encoded Domain Account ID string in ANSI</AccountID>
**                               <Revision>Optional Domain Revision</Revision>
**                               <CustomData>Optional Custom Data in ANSI</CustomData>
**                              </Data>
**                             </LeaveDomain>
** [f_cchData]              -- Size (in characters) of the custom data buffer.
**                             It must be 0 if f_pchCustomData is NULL.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size in bytes
**                             of the challenge buffer during input and receives the
**                             size (in bytes) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE if the custom data
**                          type is incorrect.
**                          DRM_E_DOMAIN_INVALID_CUSTOM_DATA if the custom data cannot
**                          be parsed.
**                          DRM_E_DOMAIN_NOT_FOUND if domain to be left is not in the
**                          current domain store.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOM_GenerateLeaveChallenge(
    __in                                      DRM_VOID                  *f_poOEMContext,
    __in                                      DRM_BB_CONTEXT            *f_poBBXContext,
    __in                                      DRM_DOMAINSTORE_CONTEXT   *f_poDomainStoreContext,
    __in                                      DRM_DWORD                  f_dwFlags,
    __in_opt                                  DRM_DOMAIN_ID             *f_poDomainID,
    __in_bcount_opt( f_cbDeviceCert )   const DRM_BYTE                  *f_pbDeviceCert,
    __in                                      DRM_DWORD                  f_cbDeviceCert,
    __in_ecount_opt( f_cchData)         const DRM_CHAR                  *f_pchData,
    __in                                      DRM_DWORD                  f_cchData,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE                  *f_pbChallenge,
    __inout                                   DRM_DWORD                 *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchXMLContext = 0;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_XML_KEY oXMLKey = EMPTY_XML_KEY;
    DRM_DWORD cchEncodedCustomData = 0;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_GUID;
    const DRM_CHAR *pchCustomData = NULL;
    DRM_DWORD cchCustomData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_GenerateLeaveChallenge );

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poBBXContext->fInited );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkBOOL( f_dwFlags == DRM_REGISTER_NULL_DATA ||
             f_dwFlags == DRM_REGISTER_CUSTOM_DATA ||
             f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA,
             DRM_E_DOMAIN_INVALID_CUSTOM_DATA_TYPE );

    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
            *f_pcbChallenge > 0 );

    if( f_dwFlags == DRM_REGISTER_INDIRECT_QUERY_DATA )
    {
        ChkArg( f_pchData != NULL && f_cchData > 0 );

        f_poDomainID = &oDomainID;

        ChkDR( _ParseLeaveDomainChallengeQueryData( f_pchData,
                                                    f_cchData,
                                                    f_poDomainID,
                                                    &pchCustomData,
                                                    &cchCustomData,
                                                    f_pbChallenge ) );

    }
    else
    {
        ChkArg( f_poDomainID != NULL );
        ChkArg( !DRM_SOAPXML_IsBlankGUID( &f_poDomainID->m_oAccountID ) );
        ChkArg( ( f_pchData == NULL && f_cchData == 0 ) ||
                ( f_pchData != NULL && f_cchData > 0 ) );

        /* The data f_pchData carries custom data */
        pchCustomData = f_pchData;
        cchCustomData = f_cchData;
    }

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poOEMContext, &oXMLKey, &f_poBBXContext->CryptoContext ) );

    /* Get the size (number of characters) of the XML encoded custom data. */
    ChkDR( DRM_SOAPXML_EncodeData( pchCustomData,
                                   cchCustomData,
                                   NULL,
                                   &cchEncodedCustomData ) );

    cchXMLContext = _CalcLeaveChallengeCharCount( f_poDomainID,
                                                  f_poBBXContext,
                                                  CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                                  &oXMLKey,
                                                  cchEncodedCustomData );

    /* Add space used by XML builder context. */
    cchXMLContext += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realighnment.
    */
    cchXMLContext += SIZEOF( DRM_DWORD );

    if ( f_pbChallenge == NULL || *f_pcbChallenge < cchXMLContext )
    {
        *f_pcbChallenge = cchXMLContext;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXMLContext,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildLeaveChallengeXML( poXMLContext,
                                    cbXMLContext,
                                    f_poBBXContext,
                                    f_poDomainID,
                                    &oXMLKey,
                                    f_pbDeviceCert,
                                    f_cbDeviceCert,
                                    pchCustomData,
                                    cchCustomData,
                                    &dastrXML ) );

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

#endif

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

    /*
    ** DRM App context should have an already opened domain store during
    ** its initialization.
    */

    dr = DRM_DOMST_DeleteCert( f_poDomainStoreContext, &f_poDomainID->m_oAccountID );

    if ( dr == DRM_E_DOMAIN_STORE_DELETE_DATA )
    {
        /*
        ** If DRM_E_DOMAIN_STORE_DELETE_DATA is returned, it is possible that the device
        ** has not yet joined the domain. In this case, try to delete the keys anyway.
        ** and return a more user friendly error code to the caller.
        */
        DRM_DOMST_DeleteKeys( f_poDomainStoreContext, &f_poDomainID->m_oAccountID );

        dr = DRM_E_DOMAIN_NOT_FOUND;
    }
    else
    {
        ChkDR( dr );

        ChkDR( DRM_DOMST_DeleteKeys( f_poDomainStoreContext, &f_poDomainID->m_oAccountID ) );
    }

    /*
    ** If there is a failure between the deletion of domain certificate and key(s), it is
    ** possible to have a DID that has a domain private key(s) in the domain store
    ** but its matching domain certificate has already been deleted.
    */

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( &oXMLKey, SIZEOF( DRM_XML_KEY ) );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_ProcessLeaveResponse
**
** Synopsis: API that processes a domain leave response received from
**           the domain server.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the domain
**                             leave response from the domain registration
**                             server.
** [f_cbResponse]           -- Size of the domain leave response from the domain
**                             registration server. It must be bigger than 0.
** [f_pResult]              -- Optional pointer to a variable that receives the
**                             status code.
** [f_pBBXCtx]              -- Pointer to a black box context.
**
** Returns:                 DRM_SUCCESS on success.
                            DRM_S_MORE_DATA if custom data is available from
**                          the server.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SOAPXML_XML_FORMAT if SOAP format error happens.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_DOM_ProcessLeaveResponse(
    __in_bcount( f_cbResponse )       DRM_BYTE       *f_pbResponse,
    __in                              DRM_DWORD       f_cbResponse,
    __out_ecount(1)                   DRM_RESULT     *f_pResult,
    __in                        const DRM_BB_CONTEXT *f_pBBXCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_BOOL fOverwriteResult = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_ProcessLeaveResponse );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pBBXCtx != NULL );

    fOverwriteResult = TRUE;

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    // The following line of code does the basic validation of message format.
    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )dastrResponse.pszString,
                                         &dasstrResponse ) );

#endif

    /* Try to detect whether there is custom data from the server. */
    dr = DRM_SOAPXML_ParseCustomDataForProtocol( f_pbResponse,
                                                 f_cbResponse,
                                                 DRM_SOAPXML_PROTOCOL_DOMAIN_LEAVE_RESPONSE,
                                                 NULL,
                                                 NULL );
    if ( dr == DRM_E_BUFFERTOOSMALL )
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

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_FindCert
**
** Synopsis: API that retrieves a domain certificate from the domain
**           store for a specific domain.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context that has
**                             already been opened.
** [f_pDomainID]            -- Domain ID of the domain whose domain certificate will
**                             be retrieved. Only the account ID field is used.
** [f_pbDomainCert]         -- Pointer to a buffer to receive a domain certificate
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbDomainCert.
** [f_pcbDomainCert]        -- Pointer to a variable that contains the size (in bytes)
**                             of the domain certificate buffer during input and
**                             receives the size (in bytes) of the domain certificate
**                             buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DOM_FindCert(
    __in                                       DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                                 const DRM_DOMAIN_ID           *f_poDomainID,
    __out_bcount_opt( *f_pcbDomainCert )       DRM_BYTE                *f_pbDomainCert,
    __inout                                    DRM_DWORD               *f_pcbDomainCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID oServiceID = EMPTY_DRM_GUID;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_FindCert );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_pcbDomainCert != NULL );
    ChkArg( f_pbDomainCert == NULL ||
            *f_pcbDomainCert > 0 );

    ChkDR( DRM_DOMST_GetCert( f_poDomainStoreContext,
                              &f_poDomainID->m_oAccountID,
                              &oServiceID,
                              f_pbDomainCert,
                              f_pcbDomainCert ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_InitCertEnum
**
** Synopsis: API that initializes an domain store enumerator context
**           for all the domain certificates.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context that has
**                             already been opened.
** [f_poDomainCertEnumContext]
**                          -- Pointer to a domain cert enumerator context
**                             for all the domain certificates. This API
**                             initialized the domain cert context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DOM_InitCertEnum(
    __in  DRM_DOMAINSTORE_CONTEXT      *f_poDomainStoreContext,
    __out DRM_DOMAIN_CERT_ENUM_CONTEXT *f_poDomainCertEnumContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_InitCertEnum );

    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poDomainCertEnumContext != NULL );

    ChkDR( DRM_DOMST_InitEnumCert( f_poDomainStoreContext,
                                   f_poDomainCertEnumContext ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_EnumNextCert
**
** Synopsis: API that enumerates the next domain certificate using an
**           initialized domain cert enumerator context.
**
** Arguments:
**
** [f_poDomainCertEnumContext]
**                          -- Pointer to an initialized domain cert
**                             enumerator context.
** [f_pcchDomainCert]       -- Pointer to a variable that receives the size
**                             (in characters) of the domain cert being
**                             enumerated. Caller should allocate buffer using
**                             the returned size and then call Drm_DomainCert_Find()
**                             to actually read the domain certificate.
** [f_poDomainCertInfo]     -- Pointer to a structure that receives additional
**                             information of the domain certificates retrieved.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE when there is no more element to be
**                          enumerated next.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_DOM_EnumNextCert(
    __in  DRM_DOMAIN_CERT_ENUM_CONTEXT  *f_poDomainCertEnumContext,
    __out DRM_DWORD                     *f_pcchDomainCert,
    __out DRM_DOMAINCERT_INFO           *f_poDomainCertInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DOMAINAPI, PERF_FUNC_DRM_DOM_EnumNextCert );

    ChkArg( f_poDomainCertEnumContext != NULL );
    ChkArg( f_poDomainCertEnumContext->m_fInited );
    ChkArg( f_pcchDomainCert != NULL );
    ChkArg( f_poDomainCertInfo != NULL );

    ChkDR( DRM_DOMST_EnumNextCert( f_poDomainCertEnumContext,
                                  &oDomainID.m_oAccountID,
                                  &oDomainID.m_oServiceID,
                                   NULL,
                                   f_pcchDomainCert ) );

    MEMCPY( &f_poDomainCertInfo->m_oDomainID, &oDomainID, SIZEOF( DRM_DOMAIN_ID ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_IsLicenseValidForELS(
    __in          DRM_DOMAINSTORE_CONTEXT     *f_pDomainStore,
    __inout const DRM_XMR_LICENSE             *f_pXMRLicense,
    __inout       DRM_BOOL                    *f_pfValidForELS )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_GUID   oAccountID   = { 0 };
    DRM_DWORD  cbCert       = 0;
    DRM_GUID   oScratchGuid = { 0 };

    ChkArg( f_pXMRLicense   != NULL );
    ChkArg( f_pfValidForELS != NULL );

    DRMASSERT( f_pXMRLicense->containerOuter.containerGlobalPolicies.DomainID.cbAccountID == SIZEOF( DRM_GUID ) );

    DRM_BYT_CopyBytes( &oAccountID,
                       0,
                       f_pXMRLicense->containerOuter.containerGlobalPolicies.DomainID.pbAccountID,
                       f_pXMRLicense->containerOuter.containerGlobalPolicies.DomainID.ibAccountID,
                       SIZEOF( DRM_GUID ) );

    dr = DRM_DOMST_GetCert( f_pDomainStore,
                            &oAccountID,
                            &oScratchGuid,
                            NULL,
                            &cbCert );

    /*
    ** This is returned if there is a certificate for this domain
    */
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        *f_pfValidForELS = TRUE;
        dr = DRM_SUCCESS;
    }
    /*
    ** This is returned if there is NO certificate for this domain
    */
    else if( dr == DRM_E_DOMAIN_STORE_GET_DATA )
    {
        *f_pfValidForELS = FALSE;
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_LA_CalcDomainCertsCharCount
**
** Synopsis: Function that calculates the character count of a XML segment
**           that contains a collection of domain certificates. Each domain
**           certificate is encapsulated under a <CertificateChain>
**           node.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context.
** [f_poServiceID]          -- Pointer to a service ID to specify domain
**                             certificates to be included into the
**                             challenge. If it is a blank then all domain
**                             certificates are included.
** [f_pcchDomainCerts]      -- Pointer to a variable that receives the
**                             character count of the domain certificate
**                             chains XML segment.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_LA_CalcDomainCertsCharCount(
    __in        DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in  const DRM_GUID                *f_poServiceID,
    __out       DRM_DWORD               *f_pcchDomainCerts )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_ENUM_CONTEXT oDomainStoreEnumContext = { 0 };
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_GUID oServiceID = EMPTY_DRM_GUID;
    DRM_DWORD cbDomainCert = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__CalcDomainCertsCharCount );

    ChkBOOL( DRM_DOMAIN_IsDomainSupported(), DRM_E_NOTIMPL );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poServiceID != NULL );
    ChkArg( f_pcchDomainCerts != NULL );

    *f_pcchDomainCerts = 0;

    ChkDR( DRM_DOMST_InitEnumCert( f_poDomainStoreContext,
                                   &oDomainStoreEnumContext ) );

    for ( ; ; )
    {
        dr = DRM_DOMST_EnumNextCert( &oDomainStoreEnumContext,
                                     &oAccountID,
                                     &oServiceID,
                                     NULL,
                                     &cbDomainCert );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        if ( cbDomainCert == 0 ||
             DRM_SOAPXML_IsBlankGUID( f_poServiceID ) ||
             MEMCMP( &oServiceID, f_poServiceID, SIZEOF( DRM_GUID ) ) != 0 )
        {
            /*
            ** Skip those domain certificates that do not belong
            ** to the passed in service ID.
            */
            continue;
        }

        cbDomainCert = CCH_BASE64_EQUIV( cbDomainCert );

        /*
        ** Add one more DRM_WORD to consider the possible realignment.
        */
        cbDomainCert += SIZEOF( DRM_WORD );

        *f_pcchDomainCerts += cbDomainCert;

        /* <CertificateChain> */
        *f_pcchDomainCerts += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqCertificateChainTag.cchString,
                                                            0,
                                                            0,
                                                            0 );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_LA_BuildDomainCertsXML
**
** Synopsis: Function that constructs a domain certificate chains
**           XML segment that contains one or more domain certificates.
**
** Arguments:
**
** [f_poDomainStoreContext] -- Pointer to a domain store context.
** [f_poServiceID]          -- Pointer to a service ID to specify domain
**                             certificates to be included into the
**                             challenge. If it is a blank then all domain
**                             certificates are included.
** [f_poXMLContext]         -- Pointer to a XML builder context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_LA_BuildDomainCertsXML(
    __in          DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in    const DRM_GUID                *f_poServiceID,
    __inout       _XMBContextA            *f_poXMLContext )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAINSTORE_ENUM_CONTEXT oDomainStoreEnumContext = { 0 };
    DRM_GUID oAccountID = EMPTY_DRM_GUID;
    DRM_GUID oServiceID = EMPTY_DRM_GUID;
    DRM_DWORD cbDomainCert = 0;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrReservedBuffer = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__BuildDomainCertsXML );

    ChkBOOL( DRM_DOMAIN_IsDomainSupported(), DRM_E_NOTIMPL );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_poServiceID != NULL );
    ChkArg( f_poXMLContext != NULL );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    DRMASSERT( pchXML != NULL );

    ChkDR( DRM_DOMST_InitEnumCert( f_poDomainStoreContext,
                                   &oDomainStoreEnumContext ) );

    for ( ; ; )
    {
        dr = DRM_DOMST_EnumNextCert( &oDomainStoreEnumContext,
                                     &oAccountID,
                                     &oServiceID,
                                     NULL,
                                     &cbDomainCert );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        if ( cbDomainCert == 0 ||
             DRM_SOAPXML_IsBlankGUID( f_poServiceID ) ||
             MEMCMP( &oServiceID, f_poServiceID, SIZEOF( DRM_GUID ) ) != 0 )
        {
            /*
            ** Skip those domain certificates that do not belong
            ** to the passed in service ID.
            */
            continue;
        }

        /*
        ** Add and open <CertificateChain> node.
        **
        ** Output:
        ** <CertificateChain>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrLicAcqCertificateChainTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContext,
                                      SIZEOF( DRM_WORD ),
                                      CCH_BASE64_EQUIV( cbDomainCert ),
                                      &dasstrReservedBuffer ) );

        /*
        ** The reserved the buffer should be big enough to store the all domain
        ** certificates. In case the buffer has more bytes needed by the domain
        ** certificates, those bytes should be set to ' ' so the server side XML
        ** parser can just ignore them.
        */
        MEMSET( pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                ' ',
                dasstrReservedBuffer.m_cch );

        ChkDR( DRM_DOMST_GetCert( f_poDomainStoreContext,
                                  &oAccountID,
                                  &oServiceID,
                                  ( DRM_BYTE * )pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                                  &cbDomainCert ) );

        DRMASSERT( CCH_BASE64_EQUIV( cbDomainCert ) == dasstrReservedBuffer.m_cch );

        ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                                cbDomainCert,
                                pchXML + __CB_DECL( dasstrReservedBuffer.m_ich ),
                                &dasstrReservedBuffer.m_cch,
                                0 ) );

        /*
        ** Close <CertificateChain> node.
        **
        ** Output:
        ** <CertificateChain>
        **      ...
        ** </CertificateChain>
        */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, NULL ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
** Function: _DomainStore_SetAndGetDomainSavedDateTime
**
** Synopsis: Sets and gets the saved datetime for the given account ID
**
** Arguments:
**
** [f_rgbPasswordSST]       -- Domain Store Secure Store Password generated by
**                             DRM_ARCLK_DOMST_CreateDomainStorePassword.
** [f_pOEMContext]          -- Pointer to an optional OEM context structure.
** [f_poDomainStoreContext] -- Pointer to a domain store context initialized
**                             via DRM_DOMST_OpenStore().
** [f_poAccountID]          -- Pointer to an account ID of the domain.
**                             It cannot be NULL.
** [f_pui64GlobalSavedTime] -- Optional.  If not NULL, then the saved datetime
**                             for the given account ID will be overwritten
**                             with this value IF this value is greater
**                             than the current system time.
** [f_pui64SavedTime]       -- Optional.  If not NULL, it is set to the value
**                             of the saved datetime for the given account ID.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _DomainStore_SetAndGetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __in_opt                           const DRM_UINT64              *f_pui64GlobalSavedTime,
    __out_opt                                DRM_UINT64              *f_pui64SavedTime )
{
    DRM_RESULT                       dr                                                 = DRM_SUCCESS;
    DRM_DOMAINSTORE_CONTEXT_PRIVATE *poDomainStoreContextPrivate                        =
        ( DRM_DOMAINSTORE_CONTEXT_PRIVATE * )f_poDomainStoreContext;
    TOKEN                            token                                              = {0};
    DRMFILETIME                      ftCurrentTime                                      = {0};
    DRM_UINT64                       ui64CurrentTime                                    = DRM_UI64LITERAL( 0, 0 );

    ChkDR( DRM_SST_OpenKeyTokens( &poDomainStoreContextPrivate->m_oSSTContext,
                                  ( DRM_ID * )f_poAccountID,
                                  &g_oDateTimeStoredTypeID,
                                  f_rgbPasswordSST,
                                  DRM_SECURE_STORE_OPEN_ALWAYS,
                                  SECURE_STORE_GLOBAL_DATA,
                                  poDomainStoreContextPrivate->m_poDST ) );

    ChkDR( DRM_SST_LoadKeyTokens( &poDomainStoreContextPrivate->m_oSSTContext,
                                  DRM_SECURE_STORE_OPEN_ALWAYS,
                                  poDomainStoreContextPrivate->m_poDST ) );

    ChkDR( DRM_SST_GetTokenValue( &poDomainStoreContextPrivate->m_oSSTContext,
                                  &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                  &token ) );

    if( dr == DRM_S_FALSE )
    {
        /*
        ** This must be the first time the domain has been acted upon
        ** wrt domain clock rollback functionality, either because the
        ** domain is being stored the first time OR because we've upgraded
        ** from a version which didn't have this functionality.
        ** Either way, we only SET the value if it's the first time.
        */
        ZEROMEM( &token, SIZEOF( token ) );

        Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext,
                                           &ftCurrentTime );
        FILETIME_TO_UI64( ftCurrentTime, ui64CurrentTime );

        token.TokenType         = TOKEN_DATETIME;
        token.val.u64DateTime   = ui64CurrentTime;

        ChkDR( DRM_SST_SetTokenValue( &poDomainStoreContextPrivate->m_oSSTContext,
                                      &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                      &token ) );
    }

    ChkBOOL( token.TokenType == TOKEN_DATETIME, DRM_E_WRONG_TOKEN_TYPE );

    if( f_pui64GlobalSavedTime != NULL )
    {
        /*
        ** The caller requested that we use the Global saved instead of
        ** the current system time (if the global saved time is greater).
        */
        if( DRM_UI64Les( token.val.u64DateTime, *f_pui64GlobalSavedTime ) )
        {
            token.val.u64DateTime = *f_pui64GlobalSavedTime;

            ChkDR( DRM_SST_SetTokenValue( &poDomainStoreContextPrivate->m_oSSTContext,
                                          &g_dstrDRM_LS_OLDSAVETIME_ATTR,
                                          &token ) );
        }
    }

    ChkDR( DRM_SST_CloseKey( &poDomainStoreContextPrivate->m_oSSTContext,
                             poDomainStoreContextPrivate->m_poDST,
                             NULL,
                             NULL ) );

    if( f_pui64SavedTime != NULL )
    {
        *f_pui64SavedTime = token.val.u64DateTime;
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_DOM_ARCLK_ClockRollbackDetected
**
** Synopsis: Updates saved datetimes for all domains after clock rollback.
**           Call this function when clock rollback is detected.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [ui64GlobalSavedTime]    -- Global saved datetime
**                             i.e. last known good datetime before rollback.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_ClockRollbackDetected(
    __in  DRM_BB_CONTEXT          *f_poBBXContext,
    __in  DRM_UINT64               ui64GlobalSavedTime )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                      dr                      = DRM_SUCCESS;
    DRM_DOMAINSTORE_ENUM_CONTEXT    oDomainStoreEnumContext = {0};
    DRM_GUID                        guidAccountID           = {0};
    DRM_DST_SLOT_HINT               oSlotHint               = {0};
    DRM_BOOL                        fDone                   = FALSE;

    ChkArg( f_poBBXContext != NULL );

    ChkDR( DRM_DOMST_InitEnumCert( f_poBBXContext->pDomainStoreContext,
                                   &oDomainStoreEnumContext ) );

    while( !fDone )
    {
        dr = DRM_DOMST_EnumNextCert( &oDomainStoreEnumContext,
                                     &guidAccountID,
                                     NULL,
                                     &oSlotHint,
                                     NULL );
        if( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            fDone = TRUE;
        }
        else
        {
            DRM_BYTE rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] = {0};

            ChkDR( dr );

            ChkDR( DRM_ARCLK_DOMST_CreateDomainStorePassword( &guidAccountID,
                                                              f_poBBXContext,
                                                              rgbPasswordSST ) );

            ChkDR( _DomainStore_SetAndGetDomainSavedDateTime( rgbPasswordSST,
                                                              f_poBBXContext->pOEMContext,
                                                              f_poBBXContext->pDomainStoreContext,
                                                              &guidAccountID,
                                                              &ui64GlobalSavedTime,
                                                              NULL ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_LEVL_DisableOnRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR,
    __in          DRM_UINT64           f_u64MachineDateTime,
    __out         DRM_BOOL            *f_pfDisabled )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT               dr               = DRM_SUCCESS;
    const DRM_XMR_DOMAIN_ID *pLicenseDomainId = NULL;

    ChkArg( f_pContextLicEval != NULL );
    ChkArg( f_pXMR            != NULL );
    ChkArg( f_pfDisabled      != NULL );
    *f_pfDisabled = FALSE;

    pLicenseDomainId = &f_pXMR->containerOuter.containerGlobalPolicies.DomainID;
    if( pLicenseDomainId->cbAccountID == SIZEOF( DRM_GUID ) )
    {
        DRM_GUID    guidAccountID                                   = {0};
        DRM_UINT64  ui64DomainSavedDateTime                         = DRM_UI64LITERAL( 0, 0 );
        DRM_BYTE    rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)]  = {0};

        DRM_BYT_CopyBytes( &guidAccountID,
                           0,
                           pLicenseDomainId->pbAccountID,
                           pLicenseDomainId->ibAccountID,
                           pLicenseDomainId->cbAccountID );

        ChkDR( DRM_ARCLK_DOMST_CreateDomainStorePassword( &guidAccountID,
                                                          f_pContextLicEval->pcontextBBX,
                                                          rgbPasswordSST ) );

        ChkDR( DRM_ARCLK_DOMST_GetDomainSavedDateTime( rgbPasswordSST,
                                                       f_pContextLicEval->pcontextBBX->pOEMContext,
                                                       f_pContextLicEval->pcontextBBX->pDomainStoreContext,
                                                       &guidAccountID,
                                                       &ui64DomainSavedDateTime ) );

        if( DRM_UI64Les( f_u64MachineDateTime, ui64DomainSavedDateTime ) )
        {
            TRACE(("Domain-based Clock rollback detected in DRM_LEVL_PerformOperationsXMR."));
            f_pContextLicEval->lReasonForFail = LR_LICENSE_TIME_CHECK_FAILURE;
            *f_pfDisabled = TRUE;
        }
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_CreateDomainStorePassword(
    __in                          const DRM_GUID       *f_poAccountID,
    __in                                DRM_BB_CONTEXT *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE        f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_poAccountID != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_rgbPasswordSST != NULL );

    ChkDR( DRM_SST_CreateLicenseStatePassword( ( DRM_ID * )f_poAccountID,
                                               f_rgbPasswordSST,
                                               f_poBBXContext ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __out_opt                                DRM_UINT64              *f_puiDomainSavedDateTime )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkDR( _DomainStore_SetAndGetDomainSavedDateTime( f_rgbPasswordSST,
                                                      f_pOEMContext,
                                                      f_poDomainStoreContext,
                                                      f_poAccountID,
                                                      NULL,
                                                      f_puiDomainSavedDateTime ) );

ErrorExit:
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

