/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>
#include <drmxmlbuilder.h>
#include <oemsha1.h>
#include <drmlicense.h>
#include <drmhdrbuilder.h>
#include <drmkeygenerator.h>
#include <drmhdrbuilderconstants.h>

ENTER_PK_NAMESPACE_CODE;


/*************************************************************************
** Macros
**************************************************************************/
#define Tracked_N_ChkDR(hb,expr) {                         \
    if ( (hb)->dwItemsTracked == (hb)->dwItemsAdded ) \
    {                          \
        ChkDR((expr));         \
        ++(hb)->dwItemsAdded;  \
    }                          \
    ++(hb)->dwItemsTracked;    \
}


/*************************************************************************
** local constants
**************************************************************************/
/* default tag data */

static const DRM_WORD _cbXmlBuilderCtx=8192;    /* initial XML builder context size */

/*************************************************************************
** static functions
**************************************************************************/


/*
**
*/
static DRM_VOID
_ChangeState(
    _HBContext *pHB,
    _EHBContextState eState)
{
    pHB->eState = eState;
    pHB->dwItemsAdded = pHB->dwItemsTracked = 0;
}


/*
** Add the following node/subnode to the request:
**
**  <SIGNATURE>
**    <HASHALGORITHM type=\"SHA\"></HASHALGORITHM>
**    <SIGNALGORITHM type=\"MSDRM\"></SIGNALGORITHM>
**    <VALUE>QrUgBBheJVzAOOw2kbo*1sqJkwft4oXKNBPv56PpUhho5fS6XK6sTg==</VALUE>
**  </SIGNATURE>
*/
static DRM_RESULT _SignTheHeader(
    _HBContext         *pHB,
    DRM_CRYPTO_CONTEXT *pCryptoContext,
    DRM_CONST_STRING   *pdstrPrivKey,
    DRM_STRING         *pdstrDataNodeFragment)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(pHB);

    /* open <SIGNATURE> node */
    Tracked_N_ChkDR(pHB, DRM_XMB_OpenNode(pHB->pbXmlContext, &g_dstrTagSignature));

    /* add <HASHALGORITHM> node */
    Tracked_N_ChkDR(pHB, DRM_XMB_OpenNode(pHB->pbXmlContext, &g_dstrTagHashAlg));
    Tracked_N_ChkDR(pHB, DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeType, &g_dstrSHA));
    Tracked_N_ChkDR(pHB, DRM_XMB_CloseCurrNode(pHB->pbXmlContext, NULL));

    /* add <SIGNALGORITHM> node */
    Tracked_N_ChkDR(pHB, DRM_XMB_OpenNode(pHB->pbXmlContext, &g_dstrTagSignAlg));
    Tracked_N_ChkDR(pHB, DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeType, &g_dstrMSDRM));
    Tracked_N_ChkDR(pHB, DRM_XMB_CloseCurrNode(pHB->pbXmlContext, NULL));

    /* add <VALUE> node */
    {
        DRM_BYTE    rgbSignature[__CB_DECL( DRM_ECC160_SIGNATURE_LEN )] = { 0 };
        DRM_DWORD   cchEncoded  = 0;
        PRIVKEY     oPrivKey    = { 0 };
        DRM_DWORD   dwPrivKey   = SIZEOF(PRIVKEY);
        DRM_DWORD   cbSignature = DRM_ECC160_SIGNATURE_LEN;
        DRM_CONST_STRING dstrSignature = EMPTY_DRM_STRING;

        /* decode the signing key */
        ChkDR(DRM_B64_DecodeW(pdstrPrivKey, &dwPrivKey, (DRM_BYTE *)&oPrivKey, 0));

        /* sign the <DATA> node */
        ChkDR( DRM_PK_Sign( pCryptoContext->rgbCryptoContext,
                            eDRM_ECC_P160,
                            DRM_ECC160_PRIVATE_KEY_LEN,
                            ( const DRM_BYTE * )&oPrivKey,
                            CB_DSTR( pdstrDataNodeFragment ),
                            PB_DSTR( pdstrDataNodeFragment ),
                            &cbSignature,
                            rgbSignature ) );

        /* Base64 encode the signature */

        cchEncoded = CCH_BASE64_EQUIV( cbSignature );
        if ( (cchEncoded + 1) * SIZEOF(DRM_WCHAR) > SIZEOF( pHB->rgbLocalStack ) )
        {
            ChkDR(DRM_E_OUTOFMEMORY);       /* LOCALSTACKSIZE too small */
        }
        ZEROMEM(pHB->rgbLocalStack, (cchEncoded+1) * SIZEOF(DRM_WCHAR) );
        ChkDR(DRM_B64_EncodeW( rgbSignature,
                               DRM_ECC160_SIGNATURE_LEN,
                  (DRM_WCHAR *)pHB->rgbLocalStack,
                              &cchEncoded,
                              0));
        dstrSignature.pwszString = (DRM_WCHAR *)pHB->rgbLocalStack;
        dstrSignature.cchString = cchEncoded;

        Tracked_N_ChkDR(pHB, DRM_XMB_OpenNode(pHB->pbXmlContext, &g_dstrTagValue));
        Tracked_N_ChkDR(pHB, DRM_XMB_AddData(pHB->pbXmlContext, &dstrSignature));
        Tracked_N_ChkDR(pHB, DRM_XMB_CloseCurrNode(pHB->pbXmlContext, NULL));
    }

    /* close SIGNATURE node */
    Tracked_N_ChkDR(pHB, DRM_XMB_CloseCurrNode(pHB->pbXmlContext, NULL));

ErrorExit:
    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_HB_CreateHeader(
    __in_bcount_opt( *pcbHBContext ) DRM_BYTE *pbHBContext,
    IN     DRM_CONTENT_SET_PROPERTY  eHeaderType,
    IN OUT DRM_DWORD                *pcbHBContext )
{
    DRM_RESULT  dr        = DRM_SUCCESS;
    _HBContext *pHB       = NULL;
    DRM_DWORD   cbContext = 0;
    DRM_DWORD   cbTotal   = 0;
    DRM_DWORD   cbAligned = 0;
    DRM_DWORD   cbAdjust  = 0;

    /*
    ** V4.1 header building is not supported through the DRM_HB_* APIs.
    */
    ChkArg( eHeaderType == DRM_CSP_V2_HEADER
         || eHeaderType == DRM_CSP_V2_4_HEADER
         || eHeaderType == DRM_CSP_V4_HEADER );
    ChkArg( pcbHBContext );

    /* check given context size */
    cbContext = SIZEOF(_HBContext) - 1 + _cbXmlBuilderCtx;
    if ( pbHBContext == NULL || *pcbHBContext < cbContext )
    {
        /* minimum context size to start with */
        *pcbHBContext= cbContext;
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    /* Initialize Context
    ** note: pbXmlContext is physically located at end of pbBlackboxContext.
    */
    ZEROMEM(pbHBContext, *pcbHBContext);
    pHB = (_HBContext*)pbHBContext;
    pHB->wContextSize = (DRM_WORD)*pcbHBContext;

    /* init initial state */
    _ChangeState(pHB, eHBCreating);

    cbTotal = *pcbHBContext - SIZEOF(_HBContext) + 1;

    ChkDR(DRM_UTL_EnsureDataAlignment((DRM_BYTE*)pHB->rgbXmlContext,
                                       cbTotal,
                                       (DRM_BYTE**)&pHB->pbXmlContext,
                                       &cbAligned,
                                       SIZEOF(DRM_DWORD),
                                       &cbAdjust));

    pHB->wContextSize -= cbAdjust;

    pHB->eHeaderType = eHeaderType;

    /* Init XML Builder
    ** Add document root tag: <WRMHEADER version=X.0.0.0">
    */
    ChkDR(DRM_XMB_CreateDocument(cbAligned,
                                 pHB->pbXmlContext,
                                &g_dstrTagWrmHeader));

    if ( pHB->eHeaderType == DRM_CSP_V2_HEADER
      || pHB->eHeaderType == DRM_CSP_V2_4_HEADER )
    {
       ChkDR(DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeVersion, &g_dstrAttributeVersion2Value));
    }
    else if ( pHB->eHeaderType == DRM_CSP_V4_HEADER )
    {
       ChkDR(DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeHeaderXmlnsName, &g_dstrAttributeHeaderXmlnsValue));
       ChkDR(DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeVersion, &g_dstrAttributeVersion4Value));
    }
    else if ( pHB->eHeaderType == DRM_CSP_V4_1_HEADER )
    {
        ChkDR(DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeHeaderXmlnsName, &g_dstrAttributeHeaderXmlnsValue));
        ChkDR(DRM_XMB_AddAttribute(pHB->pbXmlContext, &g_dstrAttributeVersion, &g_dstrAttributeVersion4_1Value));
    }


    /* open <DATA> node to the root node  */
    ChkDR(DRM_XMB_OpenNode(pHB->pbXmlContext, &g_dstrTagData));

    /* change state */
    _ChangeState(pHB, eHBInited);

ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_ReallocHeader(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbOldHBContext,
    IN DRM_DWORD cbNewHBContext,
    __in_bcount( cbNewHBContext ) DRM_BYTE *pbNewHBContext)
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    DRM_DWORD   cbTotal   = 0;
    DRM_DWORD   cbAligned = 0;
    DRM_DWORD   cbAdjust  = 0;
    _HBContext *pOldHB = (_HBContext*)pbOldHBContext;
    _HBContext *pNewHB = (_HBContext*)pbNewHBContext;

    ChkArg(pbOldHBContext && pbNewHBContext  && pOldHB->eState!=eHBCreating
        && cbNewHBContext >= SIZEOF(_HBContext)   /* Prevent potential overflow */
        && cbNewHBContext > pOldHB->wContextSize);

    /* copy old context to new buffer */
    ZEROMEM(pbNewHBContext,  cbNewHBContext);
    pNewHB->wContextSize   = cbNewHBContext;
    pNewHB->dwItemsAdded   = pOldHB->dwItemsAdded;
    pNewHB->eState         = pOldHB->eState;
    pNewHB->fKIDAdded      = pOldHB->fKIDAdded;
    pNewHB->fLAINFOAdded   = pOldHB->fLAINFOAdded;
    pNewHB->fChecksumAdded = pOldHB->fChecksumAdded;

    cbTotal = cbNewHBContext - SIZEOF(_HBContext) + 1;
    ChkDR(DRM_UTL_EnsureDataAlignment((DRM_BYTE*)pNewHB->rgbXmlContext,
                                       cbTotal,
                                       (DRM_BYTE**)&pNewHB->pbXmlContext,
                                       &cbAligned,
                                       SIZEOF(DRM_DWORD),
                                       &cbAdjust));
    pNewHB->wContextSize -= cbAdjust;

    ChkDR(DRM_XMB_ReallocDocument(pOldHB->pbXmlContext,
                                  cbAligned,
                                  pNewHB->pbXmlContext));

ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_GetContextSize(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    OUT DRM_DWORD *pcbHBContext)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HBContext *pHB=(_HBContext*)pbHBContext;

    ChkArg(pbHBContext && pcbHBContext);  /* && pHB->eState!=eHBCreating); */

    /* current context size */
    *pcbHBContext = pHB->wContextSize;

ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_CloseHeader(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    IN  DRM_CRYPTO_CONTEXT *pCryptoContext,
    IN  DRM_CONST_STRING   *pdstrPrivKey,      /* base64 encoded */
    __deref_out_bcount( *pcbHeaderXML ) DRM_BYTE **ppbHeaderXML,
    OUT DRM_DWORD          *pcbHeaderXML)
{
    DRM_RESULT  dr                   = DRM_SUCCESS;
    _HBContext *pHB                  = (_HBContext*)pbHBContext;
    DRM_STRING  dstrDataNodeFragment = EMPTY_DRM_STRING;


    ChkArg( pbHBContext  != NULL );
    ChkArg( pcbHeaderXML != NULL );
    ChkArg( ppbHeaderXML != NULL );

    if ( pdstrPrivKey != NULL )
    {
        ChkArg( pCryptoContext != NULL );
    }

    if (pHB->eState!=eHBAddedData && pHB->eState!=eHBSigning)
    {
        if (!pHB->fKIDAdded)
        {
            ChkDR(DRM_E_NOKIDINHEADER);
        }
        else if (!pHB->fLAINFOAdded)
        {
            ChkDR(DRM_E_NOLAINFOINHEADER);
        }
        else if (!pHB->fChecksumAdded)
        {
            ChkDR(DRM_E_NOCHECKSUMINHEADER);
        }
        else
        {
            ChkDR(DRM_E_CH_INVALID_HEADER);
        }
    }

    if ( pHB->eState!= eHBSigning )
    {
        _ChangeState(pHB, eHBSigning);
    }

    /* close <DATA> node and get the XML fragment of it */
    Tracked_N_ChkDR(pHB, DRM_XMB_CloseCurrNode(pHB->pbXmlContext, &dstrDataNodeFragment));

    /* Sign the <DATA> node if a crypto context and private key were provided */
    if ( ( pHB->eHeaderType == DRM_CSP_V2_HEADER || pHB->eHeaderType == DRM_CSP_V2_4_HEADER)
      && ( pCryptoContext != NULL )
      && ( pdstrPrivKey->pwszString != NULL )
      && ( pdstrPrivKey->cchString > 0 ) )
    {
        ChkDR(_SignTheHeader(pHB, pCryptoContext, pdstrPrivKey, &dstrDataNodeFragment));
    }

    Tracked_N_ChkDR(pHB, DRM_XMB_CloseDocument(pHB->pbXmlContext, &pHB->szXMLString));

    *ppbHeaderXML = PB_DSTR( &pHB->szXMLString );
    *pcbHeaderXML = CB_DSTR( &pHB->szXMLString );

    /* make sure the context will not be used to call the API again */
    _ChangeState(pHB, eHBCreated);

ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_SetAttribute(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    IN const DRM_CONST_STRING *pdstrAttrName,
    IN const DRM_CONST_STRING *pdstrAttrValue)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HBContext *pHB=(_HBContext*)pbHBContext;

    ChkArg(pbHBContext);
    ChkDRMString(pdstrAttrName);
    ChkDRMString(pdstrAttrValue);
    ChkArg( pHB->eState == eHBInited ||
            pHB->eState == eHBAddingData ||
            pHB->eState == eHBAddedData );

    /* Add <ACTION> node */
    if ( pHB->eState!=eHBAddingData )
    {
        _ChangeState(pHB, eHBAddingData);
    }

    Tracked_N_ChkDR(pHB, DRM_XMB_OpenNode(pHB->pbXmlContext, pdstrAttrName));
    Tracked_N_ChkDR(pHB, DRM_XMB_AddData(pHB->pbXmlContext, pdstrAttrValue));
    Tracked_N_ChkDR(pHB, DRM_XMB_CloseCurrNode(pHB->pbXmlContext, NULL));

    _ChangeState(pHB, eHBAddedData);

ErrorExit:
    return dr;
}

/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_OpenNode(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    IN const DRM_CONST_STRING *pdstrNodeName )
{
    DRM_RESULT dr   = DRM_SUCCESS;
    _HBContext *pHB = (_HBContext*) pbHBContext;

    ChkArg( pbHBContext != NULL);
    ChkArg( pdstrNodeName !=NULL );

    ChkDR( DRM_XMB_OpenNode( pHB->pbXmlContext, pdstrNodeName) );

ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_HB_CloseCurrNode(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    _HBContext *pHB = (_HBContext*) pbHBContext;

    ChkArg( pbHBContext != NULL );
    ChkDR( DRM_XMB_CloseCurrNode( pHB->pbXmlContext, NULL ) );

ErrorExit:
    return dr;
}

/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_AddData(
    __in_bcount( __CB_DECL( SIZEOF( _HBContext ) ) ) DRM_BYTE *pbHBContext,
    IN const DRM_CONST_STRING *pdstrXMLData )
{
    DRM_RESULT dr   = DRM_SUCCESS;
    _HBContext *pHB = (_HBContext*) pbHBContext;

    ChkArg( pbHBContext != NULL);
    ChkArg( pdstrXMLData !=NULL );

    ChkDR( DRM_XMB_AddData(pHB->pbXmlContext, pdstrXMLData) );
ErrorExit:
    return dr;
}


/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_SetKeyID(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    IN const DRM_CONST_STRING *pdstrKID)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HBContext *pHB=(_HBContext*)pbHBContext;

    ChkArg(pbHBContext);
    ChkDRMString(pdstrKID);
    ChkArg ( pHB->eState == eHBInited || pHB->eState == eHBAddingData ||
        pHB->eState == eHBAddedData );

    if ( pHB->fKIDAdded )
    {
        ChkDR(DRM_E_DUPLICATEDHEADERATTRIBUTE);
    }

    /* Validate supplied KID, as well as store it for use in checksum calculation */
    ChkDR( DRM_UTL_DecodeKID( pdstrKID, &pHB->KID ) );

    /* Set the attribute */
    ChkDR(DRM_HB_SetAttribute(pbHBContext, &g_dstrTagKID, pdstrKID));

    pHB->fKIDAdded = TRUE;

ErrorExit:
    return dr;
}

/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_SetLicAcqUrl(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    IN const DRM_CONST_STRING *pdstrURL)
{
    DRM_RESULT dr=DRM_SUCCESS;
    _HBContext *pHB=(_HBContext*)pbHBContext;

    ChkArg(pbHBContext);
    ChkDRMString(pdstrURL);
    ChkArg ( pHB->eState == eHBInited || pHB->eState == eHBAddingData ||
        pHB->eState == eHBAddedData );

    if ( pHB->fLAINFOAdded )
    {
        ChkDR(DRM_E_DUPLICATEDHEADERATTRIBUTE);
    }
    ChkDR(DRM_HB_SetAttribute(pbHBContext, &g_dstrTagLAINFO, pdstrURL));
    pHB->fLAINFOAdded = TRUE;

ErrorExit:
    return dr;
}


/*
** Perform checksum algo to the given content key and set the check to header
*/
DRM_API DRM_RESULT DRM_CALL DRM_HB_SetChecksum(
    __in_bcount( SIZEOF( _HBContext ) ) DRM_BYTE *pbHBContext,
    IN const DRM_CONST_STRING *pdstrContentKey)
{
    DRM_RESULT dr = DRM_SUCCESS;
    _HBContext      *pHB        = (_HBContext*)pbHBContext;
    DRM_DWORD        cchEncoded = 0;
    DRM_BYTE         rgbKeyContent [__CB_DECL(DRMCIPHERKEYLEN_MAX)] = { 0 };
    DRM_DWORD        cbContentKey = DRMCIPHERKEYLEN_MAX;
    DRM_CONST_STRING dstrChecksum = EMPTY_DRM_STRING;

    ChkArg(pbHBContext);
    ChkDRMString(pdstrContentKey);
    ChkArg ( pHB->eState == eHBInited || pHB->eState == eHBAddingData ||
        pHB->eState == eHBAddedData );

    ChkBOOL( !pHB->fChecksumAdded, DRM_E_DUPLICATEDHEADERATTRIBUTE);
    ChkBOOL( pHB->fKIDAdded, DRM_E_NOKIDINHEADER ); /* KID is now required due to AES checksum algorithm */

    ChkDR(DRM_B64_DecodeW(pdstrContentKey, &cbContentKey, rgbKeyContent, 0));

    if( cbContentKey  == DRMCIPHERKEYLEN_RC4 )
    {
        DRM_SHA_CONTEXT  shaVal     = DRM_SHA_CONTEXT_DEFAULT;
        DRM_BYTE         res           [__CB_DECL(DRM_SHA1_DIGEST_LEN + 1)]     = { 0 };
        DRM_LONG         lCount     = 0;

        /* Run SHA on key SHA_ITERATIONS times. */
        /* First copy key into res. */
        DRMASSERT ( cbContentKey < DRM_SHA1_DIGEST_LEN+1 ); /* Make sure we don't exceed buffer capacity in res. */
        ZEROMEM(res, DRM_SHA1_DIGEST_LEN+1);
        ChkBOOL( (__CB_DECL(DRM_SHA1_DIGEST_LEN + 1) >= cbContentKey), DRM_E_BUFFERTOOSMALL);
        MEMCPY (res, rgbKeyContent, cbContentKey);
        for (lCount = 0; lCount < SHA_ITERATIONS; lCount++)
        {
            ChkDR( DRM_SHA_Init( &shaVal, eDRM_SHA_1 ) );
            ChkDR( DRM_SHA_Update(res, DRM_SHA1_DIGEST_LEN+1, &shaVal ) );
            ChkDR( DRM_SHA_Finalize( &shaVal, DRM_SHA1_DIGEST_LEN, res ) );
        }

        /* take first CHECKSUM_LENGTH bytes of res and use it to encode. */
        cchEncoded = CCH_BASE64_EQUIV( CHECKSUM_LENGTH );
        ChkBOOL( (cchEncoded * SIZEOF(DRM_WCHAR)) < LOCALSTACKSIZE , DRM_E_OUTOFMEMORY); /* LOCALSTACKSIZE too small */
        ZEROMEM(pHB->rgbLocalStack, cchEncoded * SIZEOF(DRM_WCHAR));
        ChkDR(DRM_B64_EncodeW(res, CHECKSUM_LENGTH, (DRM_WCHAR*)pHB->rgbLocalStack, &cchEncoded, 0));
    }
    else if( cbContentKey == DRM_AES_KEYSIZE_128 )
    {
        DRM_AES_KEY aesKey = {0};
        DRM_BYTE    rgbBlock[SIZEOF(DRM_KID)];

        ChkDR( Oem_Aes_SetKey( rgbKeyContent, &aesKey ) );
        MEMCPY( rgbBlock, pHB->KID.rgb, SIZEOF( DRM_KID ) );
        dr = Oem_Aes_EcbEncryptData(&aesKey, rgbBlock, SIZEOF( DRM_KID ) );
        Oem_Aes_ZeroKey( &aesKey );
        ChkDR( dr );

        cchEncoded = CCH_BASE64_EQUIV( DRM_AES_CK_CHECKSUM_LENGTH );
        ChkBOOL( (cchEncoded * SIZEOF(DRM_WCHAR)) < LOCALSTACKSIZE, DRM_E_OUTOFMEMORY); /* LOCALSTACKSIZE too small */
        ZEROMEM(pHB->rgbLocalStack, cchEncoded * SIZEOF(DRM_WCHAR));
        ChkDR(DRM_B64_EncodeW(rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH, (DRM_WCHAR*)pHB->rgbLocalStack, &cchEncoded, 0));
    }
    else
    {
        ChkDR( DRM_E_CH_BAD_KEY );
    }

    /* set the attribute */
    dstrChecksum.pwszString = (DRM_WCHAR*)pHB->rgbLocalStack;
    dstrChecksum.cchString = cchEncoded;
    ChkDR(DRM_HB_SetAttribute(pbHBContext, &g_dstrTagChecksum, &dstrChecksum));
    pHB->fChecksumAdded = TRUE;

ErrorExit:
    return dr;
}



EXIT_PK_NAMESPACE_CODE;
