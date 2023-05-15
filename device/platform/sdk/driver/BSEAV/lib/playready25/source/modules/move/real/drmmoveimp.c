/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMOVEIMP_C
#include <drmmoveimp.h>
#include <drmmoveformat.h>
#include <drmxmlparser.h>
#include <drmxmlsig.h>
#include <drmsoapxmlconstants.h>
#include <drmsoapxmlutility.h>
#include <drmxmlsigconstants.h>
#include <drmmoveconstants.h>
#include <drmmovestore.h>
#include <drmconstants.h>
#include <drmlastinclude.h>


ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/*********************************************************************
**
** Function: _CalcMoveChallengeDataCharCount
**
** Synopsis: Function that calculates the character count of the portion
**           of data to be encrypted in the move challenge, based
**           on the device certificate.
**
** Arguments:
**
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
**
** Returns:                 A count of characters required to hold the data
**                          portion of a challenge.
**
**********************************************************************/
static DRM_DWORD _CalcMoveChallengeDataCharCount(
    __in DRM_DWORD f_cchDeviceCert )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchCount = 0;

    ChkArg( f_cchDeviceCert > 0 );

    /* <Data> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveDataTag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <SourceCertificateChain> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveCertificateChainTag.cchString,
                                              f_cchDeviceCert,
                                              0,
                                              0 );

    /* Add space used by XML builder context. */
    cchCount += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realignment.
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
** Function: _CalcChallengeCharCount
**
** Synopsis: Function that calculates the character count of the move or copy
**           challenge
**
** Arguments:
**
** [f_cchDeviceCert]        -- Size (number of characters) of a device
**                             certificate.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY structure that
**                             contains the key information to encrypt
**                             portion of the challenge data.
**
** Returns:                 A count of characters required to hold a fully built challenge
**
**********************************************************************/
static DRM_DWORD _CalcChallengeCharCount(
    __in DRM_DWORD    f_cchDeviceCert,
    __in DRM_DWORD    f_cLicensesToMove,
    __in DRM_XML_KEY *f_poXMLKey,
    __in DRM_DWORD    f_fMove )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchCount             = 0;
    DRM_DWORD  cchSignatureXMLCount = 0;

    ChkArg( f_cchDeviceCert > 0 );

    cchCount += g_dastrXMLRootTag.cchString;

    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();

    /* <MoveRequest xmlns="..."> or <CopyRequest xmlns="..."> */
    cchCount += DRM_XMB_RequiredCharsForTagA( f_fMove ? g_dastrMovMoveRootTag.cchString : g_dastrMovCopyRootTag.cchString,
                                              0,
                                              g_dastrMovMoveRootAttribName.cchString,
                                              g_dastrMovMoveRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveChallenge2Tag.cchString,
                                              0,
                                              0,
                                              0 );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrMovMoveChallenge2AttribName.cchString,
                                                    g_dastrMovMoveChallenge2AttribValue.cchString );

    /* <Move> or <Copy> */
    cchCount += DRM_XMB_RequiredCharsForTagA( f_fMove ? g_dastrMovMoveTag.cchString : g_dastrMovCopyTag.cchString,
                                              0,
                                              g_dastrMovMoveAttrib1Name.cchString,
                                              g_dastrMovMoveAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( 0,
                                              0,
                                              g_dastrMovMoveAttrib2Name.cchString,
                                              g_dastrMovMoveAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrSOAPPreserveSpaceAttribName.cchString,
                                                    g_dastrSOAPPreserveSpaceAttribValue.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveVersionTag.cchString,
                                              g_dastrMovMoveVersion.cchString,
                                              0,
                                              0 );

    if( f_fMove )
    {
        /* Multiple <LID> entries */
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveLidTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ),
                                                  0,
                                                  0 ) * f_cLicensesToMove;
    }
    else
    {
        /* Multiple <LIDInfo><OriginalCopyCount>Number</OriginalCopyCount><LID>b64 LID</LID></LIDInfo> entries */
        cchCount +=  (   DRM_XMB_RequiredCharsForTagA( g_dastrMovCopyLidInfoTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ),
                                                  0,
                                                  0 )
                       + DRM_XMB_RequiredCharsForTagA( g_dastrMovOriginalCopyCountTag.cchString,
                                                  DRM_MAX_CCH_BASE10_DWORD_STRING,
                                                  0,
                                                  0 )
                       + DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveLidTag.cchString,
                                                  CCH_BASE64_EQUIV( SIZEOF( DRM_LID ) ),
                                                  0,
                                                  0 )
                     ) * f_cLicensesToMove;
    }

    /* <Nonce> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveNonceTag.cchString,
                                              CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                              0,
                                              0 );

    /* Calculate the character count of the data to be encrypted. */
    cchCount += _CalcMoveChallengeDataCharCount( f_cchDeviceCert );

    /* Add the character count of the signature. */
    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchSignatureXMLCount ) );

    cchCount += cchSignatureXMLCount;

ErrorExit:

    return cchCount;
}

/*********************************************************************
**
** Function: _BuildChallengeDataXML
**
** Synopsis: Function that constructs a sub XML segment to be encrypted in
**           the challenge XML string using the XML builder.
**
** Arguments:
**
** [f_pbDeviceCert]         -- Pointer to a buffer that contains the passed in device
**                             certificate.
** [f_cbDeviceCert]         -- Size (number of bytes) of the passed in device certificate.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _BuildChallengeDataXML(
    __inout                                 _XMBContextA    *f_poXMLContextA,
    __in_bcount( f_cbDeviceCert )     const DRM_BYTE        *f_pbDeviceCert,
    __in                                    DRM_DWORD        f_cbDeviceCert ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_RESULT _BuildChallengeDataXML(
    __inout                                 _XMBContextA    *f_poXMLContextA,
    __in_bcount( f_cbDeviceCert )     const DRM_BYTE        *f_pbDeviceCert,
    __in                                    DRM_DWORD        f_cbDeviceCert )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR      *pchXML           = NULL;
    DRM_SUBSTRING  dasstrDeviceCert = EMPTY_DRM_SUBSTRING;

    ChkArg( f_poXMLContextA != NULL
         && f_pbDeviceCert  != NULL
         && f_cbDeviceCert   > 0 );

    /*
    ** Add and open <Data> node.
    **
    ** Output:
    ** <Data>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                             &g_dastrMovMoveDataTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContextA, &pchXML ) );

    /*
    ** Add and open <CertificateChain> node.
    **
    ** Output:
    ** <Data>
    **      <SourceCertificateChain>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                             &g_dastrMovMoveCertificateChainTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add the device certificate.
    **
    ** Output:
    ** <Data>
    **      <SourceCertificateChain>
    **          device certificate
    */

    /* Reserve a buffer starting at a DRM_WORD aligned address. */
    ChkDR( DRM_XMB_ReserveSpaceA( f_poXMLContextA,
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
    **      <SourceCertificateChain>
    **          ...
    **      </SourceCertificateChain>
    ** </Data>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:
    return dr;
}

static DRM_RESULT _BuildMoveOrCopyChallengeXML(
    __inout                                 _XMBContextA    *f_poXMLContext,
    __in                                    DRM_DWORD        f_cbXMLContext,
    __in                                    DRM_BB_CONTEXT  *f_poBBXContext,
    __in                              const DRM_XML_KEY     *f_poXMLKey,
    __in_bcount( f_cbDeviceCert )     const DRM_BYTE        *f_pbDeviceCert,
    __in                                    DRM_DWORD        f_cbDeviceCert,
    __out                                   DRM_ID          *f_pidNonce,
    __in_ecount(f_cLicensesToMove)    const DRM_LID         *f_prgLIDsToMove,
    __in                                    DRM_DWORD        f_cLicensesToMove,
    __in_ecount_opt(f_cLicensesToMove)const DRM_DWORD       *f_rgdwOriginalCopyCount,
    __in                                    DRM_BOOL         f_fMove,
    __out                                   DRM_ANSI_STRING *f_pdastrXML ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_RESULT _BuildMoveOrCopyChallengeXML(
    __inout                                 _XMBContextA    *f_poXMLContext,
    __in                                    DRM_DWORD        f_cbXMLContext,
    __in                                    DRM_BB_CONTEXT  *f_poBBXContext,
    __in                              const DRM_XML_KEY     *f_poXMLKey,
    __in_bcount( f_cbDeviceCert )     const DRM_BYTE        *f_pbDeviceCert,
    __in                                    DRM_DWORD        f_cbDeviceCert,
    __out                                   DRM_ID          *f_pidNonce,
    __in_ecount(f_cLicensesToMove)    const DRM_LID         *f_prgLIDsToMove,
    __in                                    DRM_DWORD        f_cLicensesToMove,
    __in_ecount_opt(f_cLicensesToMove)const DRM_DWORD       *f_rgdwOriginalCopyCount,
    __in                                    DRM_BOOL         f_fMove,
    __out                                   DRM_ANSI_STRING *f_pdastrXML )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD              iLicenseToMove          = 0;
    DRM_SUBSTRING          dasstrSignedNode        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrXML               = EMPTY_DRM_SUBSTRING;
    DRM_CHAR              *pchXML                  = NULL;

    DRM_CHAR rgchID[CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) + 1 ];
    DRM_DWORD cch = SIZEOF( rgchID );
    DRM_ANSI_CONST_STRING dastrID;

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );
    ChkArg( f_pdastrXML != NULL );

    if( !f_fMove )
    {
        /* If this is a copy request then we must be provided copy counts */
        ChkArg( f_rgdwOriginalCopyCount != NULL );
    }


    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( f_poXMLContext, f_cbXMLContext ) );

    /*
    ** Add and open <MoveRequest> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <MoveRequest ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              f_fMove ? &g_dastrMovMoveRootTag : &g_dastrMovCopyRootTag,
                              NULL,
                              &g_dastrMovMoveRootAttribName,
                              &g_dastrMovMoveRootAttribValue,
                              wttOpen ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRequest ...>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRequest ...>
    **      <challenge>
    **          <Challenge ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveChallenge2Tag,
                              NULL,
                              &g_dastrMovMoveChallenge2AttribName,
                              &g_dastrMovMoveChallenge2AttribValue,
                              wttOpen ) );
    /*
    ** Add and open <Move> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRequest ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Move ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              f_fMove ? &g_dastrMovMoveTag: &g_dastrMovCopyTag,
                              NULL,
                              &g_dastrMovMoveAttrib1Name,
                              &g_dastrMovMoveAttrib1Value,
                              wttOpen ) );

    /* Add the Id="SignedData" attribute */
    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrMovMoveAttrib2Name,
                                  &g_dastrMovMoveAttrib2Value ) );


    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPPreserveSpaceAttribName,
                                  &g_dastrSOAPPreserveSpaceAttribValue ) );
    /*
    ** Add and close <Version> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRequest ...>
    **      <challenge>
    **          <Challenge ...>
    **              <Move ...>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveVersionTag,
                              &g_dastrMovMoveVersion,
                              NULL,
                              NULL,
                              wttClosed ) );

    if( f_fMove )
    {
        for( iLicenseToMove = 0; iLicenseToMove < f_cLicensesToMove; iLicenseToMove++ )
        {
            cch = SIZEOF(rgchID);
            ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)&(f_prgLIDsToMove[iLicenseToMove]),
                                    SIZEOF(DRM_LID),
                                    rgchID,
                                   &cch,
                                    0 ) );

            ChkArg( cch <= CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) );    /* Will never trigger */

            rgchID[cch] = '\0';
            dastrID.pszString = rgchID;
            dastrID.cchString = cch;

            /*
            ** Add and close <LID> node.
            **
            ** Output:
            ** [<SOAP headers>] (optional)
            ** <MoveRequest ...>
            **      <challenge>
            **          <Challenge ...>
            **              <Move ...>
            **                  <LID>...</LID>
            */
            ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                      &g_dastrMovMoveLidTag,
                                      &dastrID,
                                      NULL,
                                      NULL,
                                      wttClosed ) );
        }
    }
    else
    {
        for( iLicenseToMove = 0; iLicenseToMove < f_cLicensesToMove; iLicenseToMove++ )
        {
            cch = SIZEOF(rgchID);
            ChkDR( DRM_B64_EncodeA( (DRM_BYTE*)&(f_prgLIDsToMove[iLicenseToMove]),
                                    SIZEOF(DRM_LID),
                                    rgchID,
                                   &cch,
                                    0 ) );

            ChkArg( cch <= CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) );    /* Will never trigger */

            rgchID[cch] = '\0';
            dastrID.pszString = rgchID;
            dastrID.cchString = cch;

            /* <LIDInfo> */
            ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                          &g_dastrMovCopyLidInfoTag,
                          NULL,
                          NULL,
                          NULL,
                          wttOpen ) );

            /*
            ** Add and close <LID> node.
            */
            ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                      &g_dastrMovMoveLidTag,
                                      &dastrID,
                                      NULL,
                                      NULL,
                                      wttClosed ) );

            cch = NO_OF( rgchID );

            ChkDR( DRM_UTL_NumberToStringA(
                f_rgdwOriginalCopyCount[iLicenseToMove],
                rgchID,
                cch,
                0,
                10,
                &cch ) );
            dastrID.pszString = rgchID;
            dastrID.cchString = cch;

            /*
            ** Add and close <OriginalCopyCount> node.
            */
            ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                      &g_dastrMovOriginalCopyCountTag,
                                      &dastrID,
                                      NULL,
                                      NULL,
                                      wttClosed ) );


            /* </LIDInfo> */
            ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );
        }
    }

    ChkDR( Oem_Random_GetBytes( NULL, f_pidNonce->rgb, SIZEOF(f_pidNonce->rgb) ) );

    cch = SIZEOF(rgchID);
    ChkDR( DRM_B64_EncodeA( f_pidNonce->rgb,
                            SIZEOF(DRM_ID),
                            rgchID,
                           &cch,
                            0 ) );

    ChkArg( cch <= CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) );    /* Will never trigger */

    rgchID[cch] = '\0';
    dastrID.pszString = rgchID;
    dastrID.cchString = cch;
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveNonceTag,
                              &dastrID,
                              NULL,
                              NULL,
                              wttClosed ) );

    ChkDR( _BuildChallengeDataXML( f_poXMLContext,
                                   f_pbDeviceCert,
                                   f_cbDeviceCert ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );

    /* The whole <Move> node (inclusive) is signed. */
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
    ** <MoveLicenseRequest ...>
    ** ...
    ** </MoveLicenseRequest>
    ** [</SOAP headers>] (optional)
    */
    ChkDR( DRM_XMB_CloseDocumentA( f_poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

ErrorExit:

    return dr;
}

static DRM_RESULT _ExtractAndStoreMovePages(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in    const DRM_SUBSTRING         *f_dasstrResponseNode,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_RESULT _ExtractAndStoreMovePages(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in    const DRM_SUBSTRING         *f_dasstrResponseNode,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL   fContinue = TRUE;

    /*
    **  TASKID 224581: Consider making input param...
    */
    DRM_BYTE        rgbStack[1000];
    DRM_DWORD       cbStack = SIZEOF(rgbStack);
    DRM_DWORD       iNode   = 0;

    for( iNode = 0; fContinue; iNode++ )
    {
        DRM_DWORD       cbPage = 0;
        DRM_MOVE_PAGE   page   = {0};
        DRM_SUBSTRING   dasstrNodeData    = EMPTY_DRM_SUBSTRING;

        dr = DRM_XML_GetNodeA(f_pdastrResponse->pszString,
                              f_dasstrResponseNode,
                             &g_dastrTagMovePage,
                              NULL,
                              NULL,
                              iNode,
                              NULL,
                             &dasstrNodeData);

        if( DRM_SUCCEEDED( dr ) )
        {
            ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                                   &dasstrNodeData,
                                   &cbPage,
                                    NULL,
                                    DRM_BASE64_DECODE_IN_PLACE ) );
            ChkDR( DRM_MP_ParsePage(
                rgbStack,
                cbStack,
                ((DRM_BYTE*)f_pdastrResponse->pszString)  + dasstrNodeData.m_ich,
                cbPage,
                &page ) );
            ChkDR( DRM_MOVST_UpdateMovePage( f_poMoveStoreContext,
                                             DRM_MOVE_PAGE_N(page.BasicInformation.dwPageNumber),
                                             ((DRM_BYTE*)f_pdastrResponse->pszString)  + dasstrNodeData.m_ich,
                                             cbPage ) );
            DRM_BYT_SetBytes(f_pdastrResponse->pszString,
                             dasstrNodeData.m_ich,
                             dasstrNodeData.m_cch,
                             'a');
        }
        else if( dr == DRM_E_XMLNOTFOUND )
        {
            fContinue = FALSE;
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );
    }
ErrorExit:
    return dr;
}

static DRM_RESULT _ExtractAndStoreMoveIndex(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in    const DRM_SUBSTRING         *f_pdasstrIndexData,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_RESULT _ExtractAndStoreMoveIndex(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in    const DRM_SUBSTRING         *f_pdasstrIndexData,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_MOVE_INDEX  index  = {0};
    DRM_DWORD       cbSize = 0;

    /*
    **  TASKID 224581: Consider making input param...
    */
    DRM_BYTE        rgbStack[1000];
    DRM_DWORD       cbStack = SIZEOF(rgbStack);

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                            f_pdasstrIndexData,
                           &cbSize,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );

    ChkDR( DRM_MOV_ParseIndex( rgbStack, cbStack, ((DRM_BYTE*)f_pdastrResponse->pszString) + f_pdasstrIndexData->m_ich, cbSize, &index, (DRM_BYTE*)f_poBBXContext ) );
    ChkDR( DRM_MOVST_UpdateMovePage( f_poMoveStoreContext,
                                     DRM_MOVE_PAGE_INDEX,
                                     ((DRM_BYTE*)f_pdastrResponse->pszString) + f_pdasstrIndexData->m_ich,
                                     cbSize ) );

    /*
    ** Fill the data back in with what could be valid base64.
    ** If not the decoded page may
    ** cause further calls into the xml parser to fail
    */

    DRM_BYT_SetBytes(f_pdastrResponse->pszString,
                     f_pdasstrIndexData->m_ich,
                     f_pdasstrIndexData->m_cch,
                     'a');

ErrorExit:
    return dr;
}

static DRM_RESULT _ProcessMoveDataFromResponse(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse,
    __in    const DRM_ID                *f_pidNonce,
    __in          DRM_BOOL               f_fMove ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_RESULT _ProcessMoveDataFromResponse(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse,
    __in    const DRM_ID                *f_pidNonce,
    __in          DRM_BOOL               f_fMove )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING   dasstrResponse     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrResponseNode = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrNonce        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrVersion      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrIndex        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrAllowed      = EMPTY_DRM_SUBSTRING;
    DRM_ID          id        = {0};
    DRM_DWORD       cbSize    = 0;

    ChkArg( f_poBBXContext != NULL );
    ChkDRMANSIString( f_pdastrResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                         &dasstrResponse ) );

    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, f_fMove ? &g_dastrMovMoveResponsePath : &g_dastrMovCopyResponsePath,      NULL, NULL, NULL, &dasstrResponseNode, g_chForwardSlash ) );
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, f_fMove ? &g_dastrMovMoveVersionPath  : &g_dastrMovCopyVersionPath,       NULL, NULL, NULL, &dasstrVersion,      g_chForwardSlash ) );
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, f_fMove ? &g_dastrMovMoveNoncePath    : &g_dastrMovCopyNoncePath,         NULL, NULL, NULL, &dasstrNonce,        g_chForwardSlash ) );
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, f_fMove ? &g_dastrMovMoveAllowedPath  : &g_dastrMovCopyAllowedPath,       NULL, NULL, NULL, &dasstrAllowed,      g_chForwardSlash ) );

    cbSize = SIZEOF( DRM_ID );

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                           &dasstrNonce,
                           &cbSize,
                            id.rgb,
                            0 ) );

    ChkBOOL( cbSize == SIZEOF( DRM_ID ), DRM_E_INVALID_MOVE_RESPONSE );
    ChkBOOL( 0 == MEMCMP( id.rgb, f_pidNonce->rgb, SIZEOF( DRM_ID ) ), DRM_E_MOVE_NONCE_MISMATCH );

    ChkBOOL( DRM_UTL_DASSTRStrings_CI_Equal( f_pdastrResponse->pszString, &dasstrAllowed, &g_dastrTrue ) == TRUE, f_fMove ? DRM_E_MOVE_DENIED : DRM_E_COPY_DENIED );

    if( f_fMove )
    {
        /*
        **  Move is allowed.  Extract the move index and store it.  Then loop through all move page entries and store those.
        */
        ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, &g_dastrMovMovePageIndexPath,    NULL, NULL, NULL, &dasstrIndex,   g_chForwardSlash ) );
        ChkDR( _ExtractAndStoreMoveIndex( f_poBBXContext, f_poMoveStoreContext, &dasstrIndex, f_pdastrResponse ) );
        ChkDR( _ExtractAndStoreMovePages( f_poBBXContext, f_poMoveStoreContext, &dasstrResponseNode, f_pdastrResponse ) );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateMoveChallenge(
    __in                                      DRM_VOID              *f_poOEMContext,
    __in                                      DRM_BB_CONTEXT        *f_poBBXContext,
    __in                                const DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in_bcount( f_cbDeviceCert )       const DRM_BYTE              *f_pbDeviceCert,
    __in                                      DRM_DWORD              f_cbDeviceCert,
    __in_ecount(f_cLicensesToMove)      const DRM_LID               *f_prgLIDsToMove,
    __in                                      DRM_DWORD              f_cLicensesToMove,
    __out                                     DRM_ID                *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE              *f_pbChallenge,
    __inout                                   DRM_DWORD             *f_pcbChallenge )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD        cchXMLContext = 0;
    _XMBContextA    *poXMLContext  = NULL;
    DRM_DWORD        cbXMLContext  = 0;
    DRM_ANSI_STRING  dastrXML      = EMPTY_DRM_STRING;
    DRM_XML_KEY      oXMLKey       = EMPTY_XML_KEY;

    ChkArg( f_poBBXContext != NULL );

    ChkArg( f_pbDeviceCert != NULL
         && f_cbDeviceCert > 0 );

    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
           *f_pcbChallenge > 0 );

    ChkArg( f_poMoveStoreContext != NULL
         && f_prgLIDsToMove      != NULL
         && f_cLicensesToMove     > 0
         && f_pidNonce           != NULL );

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poOEMContext, &oXMLKey, &f_poBBXContext->CryptoContext ) );

    cchXMLContext = _CalcChallengeCharCount( CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                             f_cLicensesToMove,
                                            &oXMLKey,
                                             TRUE);

    /* Add space used by XML builder context. */
    cchXMLContext += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realignment.
    */
    cchXMLContext += SIZEOF( DRM_DWORD );

    if (  f_pbChallenge == NULL
      || *f_pcbChallenge < cchXMLContext )
    {
        *f_pcbChallenge = cchXMLContext;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXMLContext,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildMoveOrCopyChallengeXML(
        poXMLContext,
        cbXMLContext,
        f_poBBXContext,
       &oXMLKey,
        f_pbDeviceCert,
        f_cbDeviceCert,
        f_pidNonce,
        f_prgLIDsToMove,
        f_cLicensesToMove,
        NULL,
        TRUE,
       &dastrXML ) );

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( &oXMLKey, SIZEOF( DRM_XML_KEY ) );

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessMoveResponse(
    __in                           DRM_BB_CONTEXT        *f_poBBXContext,
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE              *f_pbResponse,
    __in                           DRM_DWORD              f_cbResponse,
    __in                     const DRM_ID                *f_pNonce,
    __out                          DRM_RESULT            *f_pResult )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_BOOL fOverwriteResult = FALSE;
    DRM_DWORD dwProtocolType = DRM_SOAPXML_PROTOCOL_MOVE_REQ_RESPONSE;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_poMoveStoreContext != NULL );

    ChkArg( f_pNonce  != NULL
         && f_pResult != NULL );

    fOverwriteResult = TRUE;

    ChkDR( DRM_SOAPXML_ValidateProtocolSignature( f_pbResponse, f_cbResponse, NULL, 0, &f_poBBXContext->CryptoContext, &dwProtocolType ) );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    /*
    ** Process the move response and extract the list of LIDs that can be moved.
    ** the caller provides a non NULL pointer of a domain ID variable.
    */
    ChkDR( _ProcessMoveDataFromResponse( f_poBBXContext,
                                         f_poMoveStoreContext,
                                        &dastrResponse,
                                         f_pNonce,
                                         TRUE ) );

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );
    return dr;
}



static DRM_DWORD _CalcRefreshChallengeCharCount(
    __in DRM_DWORD    f_cchDeviceCert,
    __in DRM_DWORD    f_cPagesToRefresh,
    __in DRM_XML_KEY *f_poXMLKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchCount             = 0;
    DRM_DWORD  cchSignatureXMLCount = 0;

    ChkArg( f_cchDeviceCert > 0 );

    cchCount += g_dastrXMLRootTag.cchString;

    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();


    /* <MoveRefreshRequest xmlns="..."> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveRootTag.cchString,
                                              0,
                                              g_dastrMovMoveRootAttribName.cchString,
                                              g_dastrMovMoveRootAttribValue.cchString );

    /* <challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveChallenge1Tag.cchString,
                                              0,
                                              0,
                                              0 );

    /* <Challenge> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveChallenge2Tag.cchString,
                                              0,
                                              0,
                                              0 );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrMovMoveChallenge2AttribName.cchString,
                                                    g_dastrMovMoveChallenge2AttribValue.cchString );

    /* <Refresh> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovRefreshTag.cchString,
                                              0,
                                              g_dastrMovMoveAttrib1Name.cchString,
                                              g_dastrMovMoveAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( 0,
                                              0,
                                              g_dastrMovMoveAttrib2Name.cchString,
                                              g_dastrMovMoveAttrib2Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForAttributeA( g_dastrSOAPPreserveSpaceAttribName.cchString,
                                                    g_dastrSOAPPreserveSpaceAttribValue.cchString );

    /* <Version> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveVersionTag.cchString,
                                              g_dastrMovMoveVersion.cchString,
                                              0,
                                              0 );

    /* <CurrentIndexVersion> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovRefreshCurIndexTag.cchString,
                                              g_dastrMovMoveVersion.cchString,
                                              0,
                                              0 );

    /* Multiple <Page> entries */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovRefreshPageTag.cchString,
                                              DRM_MAX_CCH_BASE10_DWORD_STRING,
                                              0,
                                              0 ) * f_cPagesToRefresh;

    /* <Nonce> */
    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrMovMoveNonceTag.cchString,
                                              CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ),
                                              0,
                                              0 );

    /* Calculate the character count of the data to be encrypted. */
    cchCount += _CalcMoveChallengeDataCharCount( f_cchDeviceCert );

    /* Add the character count of the signature. */
    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchSignatureXMLCount ) );

    cchCount += cchSignatureXMLCount;

ErrorExit:

    return cchCount;
}

static DRM_RESULT _BuildRefreshChallengeXML(
    __inout                                 _XMBContextA    *f_poXMLContext,
    __in                                    DRM_DWORD        f_cbXMLContext,
    __in                                    DRM_BB_CONTEXT  *f_poBBXContext,
    __in                              const DRM_XML_KEY     *f_poXMLKey,
    __in_bcount( f_cbDeviceCert )     const DRM_BYTE        *f_pbDeviceCert,
    __in                                    DRM_DWORD        f_cbDeviceCert,
    __out                                   DRM_ID          *f_pidNonce,
    __in                                    DRM_DWORD        f_dwIndexSerialNumber,
    __in_ecount(f_cPagesToRefresh)    const DRM_DWORD       *f_pdwPagesToRefresh,
    __in                                    DRM_DWORD        f_cPagesToRefresh,
    __out                                   DRM_ANSI_STRING *f_pdastrXML ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_RESULT _BuildRefreshChallengeXML(
    __inout                                 _XMBContextA    *f_poXMLContext,
    __in                                    DRM_DWORD        f_cbXMLContext,
    __in                                    DRM_BB_CONTEXT  *f_poBBXContext,
    __in                              const DRM_XML_KEY     *f_poXMLKey,
    __in_bcount( f_cbDeviceCert )     const DRM_BYTE        *f_pbDeviceCert,
    __in                                    DRM_DWORD        f_cbDeviceCert,
    __out                                   DRM_ID          *f_pidNonce,
    __in                                    DRM_DWORD        f_dwIndexSerialNumber,
    __in_ecount(f_cPagesToRefresh)    const DRM_DWORD       *f_pdwPagesToRefresh,
    __in                                    DRM_DWORD        f_cPagesToRefresh,
    __out                                   DRM_ANSI_STRING *f_pdastrXML )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD              iPageToInclude          = 0;
    DRM_SUBSTRING          dasstrSignedNode        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrXML               = EMPTY_DRM_SUBSTRING;
    DRM_CHAR              *pchXML                  = NULL;

    DRM_CHAR rgchID[CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) + 1 ];
    DRM_DWORD cch = SIZEOF( rgchID );
    DRM_ANSI_CONST_STRING dastrID;

    ChkArg( f_poXMLContext != NULL );
    ChkArg( f_cbXMLContext > 0 );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_pbDeviceCert != NULL && f_cbDeviceCert > 0 );
    ChkArg( f_pdastrXML != NULL );

    /* Add SOAP headers. */
    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( f_poXMLContext, f_cbXMLContext ) );

    /*
    ** Add and open <MoveRequest> node.
    **
    ** Output:
    ** [<SOAP headers>]
    ** <MoveRefreshRequest ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovRefreshRootTag,
                              NULL,
                              &g_dastrMovMoveRootAttribName,
                              &g_dastrMovMoveRootAttribValue,
                              wttOpen ) );

    ChkDR( DRM_XMB_GetXMLBaseA( f_poXMLContext, &pchXML ) );

    /*
    ** Add and open <challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRefreshRequest ...>
    **      <challenge>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveChallenge1Tag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <Challenge> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRefreshRequest ...>
    **      <challenge>
    **          <Challenge ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveChallenge2Tag,
                              NULL,
                              &g_dastrMovMoveChallenge2AttribName,
                              &g_dastrMovMoveChallenge2AttribValue,
                              wttOpen ) );
    /*
    ** Add and open <Move> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRefreshRequest ...>
    **      ...
    **      <challenge>
    **          <Challenge ...>
    **              <Refresh ...>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovRefreshTag,
                              NULL,
                              &g_dastrMovMoveAttrib1Name,
                              &g_dastrMovMoveAttrib1Value,
                              wttOpen ) );

    /* Add the Id="SignedData" attribute */
    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrMovMoveAttrib2Name,
                                  &g_dastrMovMoveAttrib2Value ) );


    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContext,
                                  &g_dastrSOAPPreserveSpaceAttribName,
                                  &g_dastrSOAPPreserveSpaceAttribValue ) );
    /*
    ** Add and close <Version> node.
    **
    ** Output:
    ** [<SOAP headers>] (optional)
    ** <MoveRefreshRequest ...>
    **      <challenge>
    **          <Challenge ...>
    **              <Refresh ...>
    **                  <Version>...</Version>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveVersionTag,
                              &g_dastrMovMoveVersion,
                              NULL,
                              NULL,
                              wttClosed ) );

    DRMCASSERT( NO_OF( rgchID ) >= DRM_MAX_CCH_BASE10_DWORD_STRING );

    cch = NO_OF( rgchID );

    ChkDR( DRM_UTL_NumberToStringA(
        f_dwIndexSerialNumber,
        rgchID,
        cch,
        0,
        10,
        &cch ) );
    dastrID.pszString = rgchID;
    dastrID.cchString = cch;

    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovRefreshCurIndexTag,
                              &dastrID,
                              NULL,
                              NULL,
                              wttClosed ) );

    for( iPageToInclude = 0; iPageToInclude < f_cPagesToRefresh; iPageToInclude++ )
    {
        cch = NO_OF( rgchID );

        ChkDR( DRM_UTL_NumberToStringA(
            f_pdwPagesToRefresh[iPageToInclude],
            rgchID,
            cch,
            0,
            10,
            &cch ) );

        dastrID.pszString = rgchID;
        dastrID.cchString = cch;

        /*
        ** Add and close <LID> node.
        **
        ** Output:
        ** [<SOAP headers>] (optional)
        ** <MoveRefreshRequest ...>
        **      <challenge>
        **          <Challenge ...>
        **              <Refresh ...>
        **                  <Page>...</Page>
        */
        ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                                  &g_dastrMovRefreshPageTag,
                                  &dastrID,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    ChkDR( Oem_Random_GetBytes( NULL, f_pidNonce->rgb, SIZEOF(f_pidNonce->rgb) ) );

    cch = SIZEOF(rgchID);
    ChkDR( DRM_B64_EncodeA( f_pidNonce->rgb,
                            SIZEOF(DRM_ID),
                            rgchID,
                           &cch,
                            0 ) );

    ChkArg( cch <= CCH_BASE64_EQUIV( SIZEOF(DRM_ID) ) );    /* Will never trigger */

    rgchID[cch] = '\0';
    dastrID.pszString = rgchID;
    dastrID.cchString = cch;
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContext,
                              &g_dastrMovMoveNonceTag,
                              &dastrID,
                              NULL,
                              NULL,
                              wttClosed ) );

    ChkDR( _BuildChallengeDataXML( f_poXMLContext,
                                   f_pbDeviceCert,
                                   f_cbDeviceCert ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContext, &dasstrSignedNode ) );

    /* The whole <Refresh> node (inclusive) is signed. */
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
    ** <MoveRefreshRequest ...>
    ** ...
    ** </MoveRefreshRequest>
    ** [</SOAP headers>] (optional)
    */
    ChkDR( DRM_XMB_CloseDocumentA( f_poXMLContext, &dasstrXML ) );

    f_pdastrXML->pszString = pchXML;
    f_pdastrXML->cchString = dasstrXML.m_cch;

ErrorExit:

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateRefreshChallenge(
    __in                                DRM_VOID              *f_poOEMContext,
    __in                                DRM_BB_CONTEXT        *f_poBBXContext,
    __in_bcount( f_cbDeviceCert ) const DRM_BYTE              *f_pbDeviceCert,
    __in                                DRM_DWORD              f_cbDeviceCert,
    __in                                DRM_DWORD              f_dwIndexSerialNumber,
    __in_ecount(f_cPagesToRefresh)const DRM_DWORD             *f_pdwPagesToRefresh,
    __in                                DRM_DWORD              f_cPagesToRefresh,
    __out                               DRM_ID                *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE              *f_pbChallenge,
    __inout                             DRM_DWORD             *f_pcbChallenge )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD        cchXMLContext = 0;
    _XMBContextA    *poXMLContext  = NULL;
    DRM_DWORD        cbXMLContext  = 0;
    DRM_ANSI_STRING  dastrXML      = EMPTY_DRM_STRING;
    DRM_XML_KEY      oXMLKey       = EMPTY_XML_KEY;

    ChkArg( f_poBBXContext != NULL );

    ChkArg( f_pbDeviceCert != NULL
         && f_cbDeviceCert > 0 );

    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
           *f_pcbChallenge > 0 );

    ChkArg( f_pidNonce != NULL );

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poOEMContext, &oXMLKey, &f_poBBXContext->CryptoContext ) );

    cchXMLContext = _CalcRefreshChallengeCharCount( CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                                    f_cPagesToRefresh,
                                                   &oXMLKey );

    /* Add space used by XML builder context. */
    cchXMLContext += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realignment.
    */
    cchXMLContext += SIZEOF( DRM_DWORD );

    if (  f_pbChallenge == NULL
      || *f_pcbChallenge < cchXMLContext )
    {
        *f_pcbChallenge = cchXMLContext;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXMLContext,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildRefreshChallengeXML(
        poXMLContext,
        cbXMLContext,
        f_poBBXContext,
        &oXMLKey,
        f_pbDeviceCert,
        f_cbDeviceCert,
        f_pidNonce,
        f_dwIndexSerialNumber,
        f_pdwPagesToRefresh,
        f_cPagesToRefresh,
        &dastrXML ) );

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( &oXMLKey, SIZEOF( DRM_XML_KEY ) );

    return dr;
}


static DRM_RESULT _ProcessRefreshDataFromResponse(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse,
    __in    const DRM_ID                *f_pidNonce ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_RESULT _ProcessRefreshDataFromResponse(
    __in          DRM_BB_CONTEXT        *f_poBBXContext,
    __in          DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout       DRM_ANSI_STRING       *f_pdastrResponse,
    __in    const DRM_ID                *f_pidNonce )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING   dasstrResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrNonce    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrVersion  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING   dasstrIndex    = EMPTY_DRM_SUBSTRING;
    DRM_ID          id        = {0};
    DRM_DWORD       cbSize    = 0;

    ChkArg( f_poBBXContext != NULL );
    ChkDRMANSIString( f_pdastrResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = f_pdastrResponse->cchString;

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders( ( DRM_CHAR * )f_pdastrResponse->pszString,
                                         &dasstrResponse ) );

    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, &g_dastrMovRefreshVersionPath, NULL, NULL, NULL, &dasstrVersion, g_chForwardSlash ) );
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, &g_dastrMovRefreshNoncePath,   NULL, NULL, NULL, &dasstrNonce,   g_chForwardSlash ) );

    cbSize = SIZEOF( DRM_ID );

    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                           &dasstrNonce,
                           &cbSize,
                            id.rgb,
                            0 ) );

    ChkBOOL( cbSize == SIZEOF( DRM_ID ), DRM_E_INVALID_MOVE_RESPONSE );
    ChkBOOL( 0 == MEMCMP( id.rgb, f_pidNonce->rgb, SIZEOF( DRM_ID ) ), DRM_E_MOVE_NONCE_MISMATCH );

    /*
    **  Move is allowed.  Extract the move index and store it.  Then loop through all move page entries and store those.
    */
    dr = DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, &g_dastrMovRefreshPageIndexPath,    NULL, NULL, NULL, &dasstrIndex,   g_chForwardSlash );

    if( dr != DRM_E_XMLNOTFOUND )
    {
        ChkDR( dr );
        ChkDR( _ExtractAndStoreMoveIndex( f_poBBXContext, f_poMoveStoreContext, &dasstrIndex, f_pdastrResponse ) );
    }
    else
    {
        DRM_SUBSTRING dasstrResponseNode = EMPTY_DRM_SUBSTRING;

        /* XML Not found means no index was returned -- this is valid */
        dr = DRM_SUCCESS;

        ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString, &dasstrResponse, &g_dastrMovRefreshResponsePath, NULL, NULL, NULL, &dasstrResponseNode, g_chForwardSlash ) );
        ChkDR( _ExtractAndStoreMovePages( f_poBBXContext, f_poMoveStoreContext, &dasstrResponseNode, f_pdastrResponse ) );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessRefreshResponse(
    __in                           DRM_BB_CONTEXT        *f_poBBXContext,
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE              *f_pbResponse,
    __in                           DRM_DWORD              f_cbResponse,
    __in                     const DRM_ID                *f_pidNonce,
    __out                          DRM_RESULT            *f_pResult )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_BOOL fOverwriteResult = FALSE;
    DRM_DWORD dwProtocolType = DRM_SOAPXML_PROTOCOL_MOVE_REFRESH_RESPONSE;

    ChkArg( f_poBBXContext       != NULL
         && f_poMoveStoreContext != NULL
         && f_pidNonce           != NULL
         && f_pResult            != NULL );

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );

    fOverwriteResult = TRUE;

    ChkDR( DRM_SOAPXML_ValidateProtocolSignature( f_pbResponse, f_cbResponse, NULL, 0, &f_poBBXContext->CryptoContext, &dwProtocolType ) );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    /*
    ** Process the move response and extract the list of LIDs that can be moved.
    ** the caller provides a non NULL pointer of a domain ID variable.
    */
    ChkDR( _ProcessRefreshDataFromResponse( f_poBBXContext,
                                            f_poMoveStoreContext,
                                           &dastrResponse,
                                            f_pidNonce ) );

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateCopyChallenge(
    __in                                      DRM_VOID              *f_poOEMContext,
    __in                                      DRM_BB_CONTEXT        *f_poBBXContext,
    __in                                const DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __in_bcount( f_cbDeviceCert )       const DRM_BYTE              *f_pbDeviceCert,
    __in                                      DRM_DWORD              f_cbDeviceCert,
    __in_ecount(f_cLicensesToCopy)      const DRM_LID               *f_prgLIDsToCopy,
    __in_ecount(f_cLicensesToCopy)      const DRM_DWORD             *f_rgdwOriginalCopyCount,
    __in                                      DRM_DWORD              f_cLicensesToCopy,
    __out                                     DRM_ID                *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE              *f_pbChallenge,
    __inout                                   DRM_DWORD             *f_pcbChallenge )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD        cchXMLContext = 0;
    _XMBContextA    *poXMLContext  = NULL;
    DRM_DWORD        cbXMLContext  = 0;
    DRM_ANSI_STRING  dastrXML      = EMPTY_DRM_STRING;
    DRM_XML_KEY      oXMLKey       = EMPTY_XML_KEY;

    ChkArg( f_poBBXContext != NULL );

    ChkArg( f_pbDeviceCert != NULL
         && f_cbDeviceCert > 0 );

    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL ||
           *f_pcbChallenge > 0 );

    ChkArg( f_poMoveStoreContext    != NULL
         && f_prgLIDsToCopy         != NULL
         && f_rgdwOriginalCopyCount != NULL
         && f_cLicensesToCopy        > 0
         && f_pidNonce              != NULL );

    ChkDR( DRM_SOAPXML_InitXMLKey( f_poOEMContext, &oXMLKey, &f_poBBXContext->CryptoContext ) );

    cchXMLContext = _CalcChallengeCharCount( CCH_BASE64_EQUIV( f_cbDeviceCert ),
                                             f_cLicensesToCopy,
                                            &oXMLKey,
                                             FALSE );

    /* Add space used by XML builder context. */
    cchXMLContext += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after realignment.
    */
    cchXMLContext += SIZEOF( DRM_DWORD );

    if (  f_pbChallenge == NULL
      || *f_pcbChallenge < cchXMLContext )
    {
        *f_pcbChallenge = cchXMLContext;

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbChallenge,
                                          cchXMLContext,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( _BuildMoveOrCopyChallengeXML(
        poXMLContext,
        cbXMLContext,
        f_poBBXContext,
        &oXMLKey,
        f_pbDeviceCert,
        f_cbDeviceCert,
        f_pidNonce,
        f_prgLIDsToCopy,
        f_cLicensesToCopy,
        f_rgdwOriginalCopyCount,
        FALSE,
        &dastrXML ) );

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

    ChkBOOL( dastrXML.cchString <= *f_pcbChallenge,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbChallenge, dastrXML.pszString, dastrXML.cchString );

    /* Return the actual size (number of bytes) of the generated challenge. */
    *f_pcbChallenge = dastrXML.cchString;

ErrorExit:

    OEM_SECURE_ZERO_MEMORY( &oXMLKey, SIZEOF( DRM_XML_KEY ) );

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessCopyResponse(
    __in                           DRM_BB_CONTEXT        *f_poBBXContext,
    __in                           DRM_MOVESTORE_CONTEXT *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE              *f_pbResponse,
    __in                           DRM_DWORD              f_cbResponse,
    __in                     const DRM_ID                *f_pNonce,
    __out                          DRM_RESULT            *f_pResult )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ANSI_STRING dastrResponse = EMPTY_DRM_STRING;
    DRM_BOOL fOverwriteResult = FALSE;
    DRM_DWORD dwProtocolType = DRM_SOAPXML_PROTOCOL_COPY_REQ_RESPONSE;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pNonce  != NULL
         && f_pResult != NULL );

    fOverwriteResult = TRUE;

    ChkDR( DRM_SOAPXML_ValidateProtocolSignature( f_pbResponse, f_cbResponse, NULL, 0, &f_poBBXContext->CryptoContext, &dwProtocolType ) );

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    /*
    ** Process the move response and extract the list of LIDs that can be moved.
    ** the caller provides a non NULL pointer of a domain ID variable.
    */
    ChkDR( _ProcessMoveDataFromResponse( f_poBBXContext,
                                         f_poMoveStoreContext,
                                        &dastrResponse,
                                         f_pNonce,
                                         FALSE) );

ErrorExit:

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  fOverwriteResult ? f_pResult : NULL );
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;
