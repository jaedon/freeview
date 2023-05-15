/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmbase64.h>
#include <drmcrt.h>
#include <drmutilities.h>
#include <oemcipher.h>
#include <drmxmlbuilder.h>
#include <drmxmlbuilderconstants.h>

ENTER_PK_NAMESPACE_CODE;

#define CB_XML_BUFFER_MINIMUM  100
#define CWCH_DWORD_STACK       (SIZEOF(DRM_DWORD)/SIZEOF(DRM_WCHAR))
#define CCH_SIMULATION_MININUM (2*CWCH_DWORD_STACK)

/******************************************************************************
** Function:   _GetPushedDWORD
**
** Synopsis:   pop a DWORD value from the end of the stack
**
** Arguments:  f_pcontextXML - the XML context in use; must be inited
**             f_iOffset     - an offset from the current stack position
**             f_pdwValue    - pointer to variable to hold the retrieved value
******************************************************************************/
static DRM_RESULT _GetPushedDWORD( __in  _XMBContext *f_pcontextXML,
                                   __in  DRM_DWORD    f_iOffset,
                                   __out DRM_DWORD   *f_pdwValue)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pcontextXML != NULL
        && f_pdwValue    != NULL);

    ChkArg((f_pcontextXML->wNextOpenNodePos + f_iOffset) >= CWCH_DWORD_STACK);

    *f_pdwValue  =  (DRM_DWORD)  (f_pcontextXML->XmlString [f_pcontextXML->wNextOpenNodePos + f_iOffset + 1]);
    *f_pdwValue += ((DRM_DWORD) ((f_pcontextXML->XmlString [f_pcontextXML->wNextOpenNodePos + f_iOffset])) << 16);

ErrorExit:
    return dr;
}

/*
** Close the current opened node
*/
static DRM_RESULT DRM_CALL _CloseCurrNode(
    __inout   _XMBContext *pXmlContext,
    __in      DRM_BOOL     fOkayToCloseRoot,
    __out_opt DRM_STRING  *pdstrXMLString)   /* optional parameter */
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_DWORD    wLen        = 0;
    DRM_DWORD    wNodePos    = 0;
    DRM_DWORD    cwchMinStack = 0;

    ChkArg(pXmlContext->fInited);

    /*
    ** make sure the stack is not empty. Stack is count by CWCH_DWORD_STACK.
    ** to close a node, we must have at least ONE CWCH_DWORD_STACK on stack.
    ** if fOkayToCloseRoot is FALSE, we must have at least two CWCH_DWORD_STACK
    ** on stack.
    */
    cwchMinStack = 2 * CWCH_DWORD_STACK;  /* do not pop off the root node */
    if (fOkayToCloseRoot)
    {
        cwchMinStack = CWCH_DWORD_STACK;  /* root node */
    }

    if ( (pXmlContext->wNextOpenNodePos + cwchMinStack) >= pXmlContext->wBuffSize )    /* do not pop off the root node !*/
    {
        dr = DRM_E_NOMORE;    /* stack is empty */
        goto ErrorExit;
    }

    if ( !pXmlContext->fIsSimMode )
    {
        /* parse length of node name */

        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &wNodePos));

        if ( pdstrXMLString != NULL )
        {
            pdstrXMLString->pwszString = &(pXmlContext->XmlString[wNodePos - 1]);
        }

        while (pXmlContext->XmlString [wNodePos + wLen] != g_wchSpace
            && pXmlContext->XmlString [wNodePos + wLen] != g_wchGreaterThan )
        {
            wLen++;
        }

        if ( pXmlContext->wNextStringPos + wLen      < pXmlContext->wNextStringPos
          || pXmlContext->wNextStringPos + wLen + 3  < pXmlContext->wNextStringPos
          || pXmlContext->wNextStringPos + wLen + 3 >= pXmlContext->wNextOpenNodePos )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* print </tag> to the XML string*/
        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                   g_dstrOpenEndTag.pwszString,
                                   g_dstrOpenEndTag.cchString) );
        pXmlContext->wNextStringPos += 2;

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                  &pXmlContext->XmlString[wNodePos],
                                   wLen) );
        pXmlContext->wNextStringPos += wLen;

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                   g_dstrCloseTag.pwszString,
                                   g_dstrCloseTag.cchString) );
        pXmlContext->wNextStringPos += 1;

        /* pop the node position from end of XML string */
        pXmlContext->wNextOpenNodePos += CWCH_DWORD_STACK;
    }
    else
    {
        DRM_DWORD cchPop = 0;

        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &cchPop));
        ChkDR(_GetPushedDWORD(pXmlContext, 2 * CWCH_DWORD_STACK, &wNodePos));

        /* calculate length of the close tag now 3 = "</>" */

        pXmlContext->wNextStringPos += cchPop + 3;

        /* pop the node position and tag name size from end of XML string */

        pXmlContext->wNextOpenNodePos += 2 * CWCH_DWORD_STACK;
    }

    if ( pdstrXMLString != NULL )
    {
        pdstrXMLString->cchString = pXmlContext->wNextStringPos - wNodePos + 1;
    }

ErrorExit:
    return dr;
}

/*
** calcuate the current node size
*/
static DRM_RESULT DRM_CALL _CalcNodeSize(
    __in  _XMBContext *pbXmlContext,
    __out DRM_DWORD   *pcchContent,  /* ch count of node content */
    __out DRM_DWORD   *pcchTagName)  /* ch count of node tag */
{
    DRM_RESULT   dr       = DRM_SUCCESS;
    DRM_DWORD    wLen     = 0;
    DRM_DWORD    wNodePos = 0;

    ChkArg( pbXmlContext != NULL
         && pcchContent  != NULL
         && pcchTagName  != NULL
         && pbXmlContext->fInited );

    if ( pbXmlContext->fIsSimMode )
    {
        _GetPushedDWORD(pbXmlContext, CWCH_DWORD_STACK,  pcchTagName);
        _GetPushedDWORD(pbXmlContext, 2 * CWCH_DWORD_STACK, &wNodePos);

        /* this is not accurate in case the open node has attributes */
        *pcchContent = pbXmlContext->wNextStringPos - wNodePos - *pcchTagName - 1;
    }
    else
    {
        /* parse length of node name */

        _GetPushedDWORD(pbXmlContext, CWCH_DWORD_STACK, &wNodePos);

        while (pbXmlContext->XmlString [wNodePos + wLen] != g_wchSpace
            && pbXmlContext->XmlString [wNodePos + wLen] != g_wchGreaterThan )
        {
            wLen++;
        }
        *pcchTagName = wLen;

        /* parse first position of node content */
        while (pbXmlContext->XmlString[wNodePos + wLen] != g_wchGreaterThan )
        {
            wLen++;
        }
        *pcchContent = pbXmlContext->wNextStringPos - wNodePos - wLen - 1;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_EncryptAndCloseCurrNode(
    __inout         _XMBContext        *pXmlContext,
    __in            DRM_CRYPTO_CONTEXT *pCryptoContext,    /* Pointer to DRM_CRYPTO_CONTEXT */
    __in_opt        DRM_VOID           *pOEMContext,
    __in      const PUBKEY             *pPubkey,           /* pub key to use for encrypt */
    __out_opt       DRM_STRING         *pdstrXMLFragment)  /* optional: XML fragment of the current node, optional. */
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    cchContent = 0;
    DRM_DWORD    cbCryptSize= 0;
    DRM_DWORD    cchEncoded = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_EncryptAndCloseCurrNode );

    ChkArg( pXmlContext   != NULL
         && pCryptoContext != NULL
         && pPubkey        != NULL
         && pXmlContext->fInited );


    /*
    ** 1. check available buffer size:
    **    req size = SIZEOF(WCHAR) * ((blob_size + DRM_ECC160_CIPHERTEXT_LEN + 2) / 3 * 4)
    ** 2. call EncryptLarge to encrypt the blob in place
    ** 3. call Base64Encode the encrypted data
    ** 4. close the node
    */

    /* check available space */
    {
        DRM_DWORD cchTagName=0;

        ChkDR(_CalcNodeSize(pXmlContext, &cchContent, &cchTagName));

        /* req size for encryption */
        cbCryptSize = cchContent * SIZEOF(DRM_WCHAR) + DRM_ECC160_CIPHERTEXT_LEN;

        /* req size for base64 encode after encryption */
        cchEncoded = CCH_BASE64_EQUIV( cbCryptSize );

        if ( !pXmlContext->fIsSimMode )
        {
            if( pXmlContext->wNextStringPos - cchContent  > pXmlContext->wNextStringPos
             || pXmlContext->wNextStringPos - cchContent >= pXmlContext->wBuffSize )
            {
                ChkDR(DRM_E_BUFFERTOOSMALL);
            }
        }
    }

    if ( !pXmlContext->fIsSimMode )
    {
        /* encrypt the content */
        ChkDR(DRM_PK_EncryptLarge(pOEMContext,
                                  pPubkey,
                     (DRM_BYTE*)(&pXmlContext->XmlString[pXmlContext->wNextStringPos-cchContent]),
                                  cchContent * SIZEOF(DRM_WCHAR),
                     (DRM_BYTE*)(&pXmlContext->XmlString[pXmlContext->wNextStringPos-cchContent]), /* encrypt in place */
                                  DRMCIPHERKEYLEN_RC4,
                                  pCryptoContext));

        /* base64 encode the crypted blob */
        ChkDR(DRM_B64_EncodeW( (DRM_BYTE*)(&pXmlContext->XmlString[pXmlContext->wNextStringPos-cchContent]),
                                            cbCryptSize,
                                           &pXmlContext->XmlString[pXmlContext->wNextStringPos-cchContent],  /* encode in place */
                                           &cchEncoded,
                                            0));
    }

    /* update wNextStringPos */
    pXmlContext->wNextStringPos = pXmlContext->wNextStringPos - cchContent + cchEncoded;

    /* now close the node */
    ChkDR(_CloseCurrNode(pXmlContext, FALSE, pdstrXMLFragment));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_SignAndCloseCurrNode(
    __inout                                       _XMBContext        *pbXmlContext,
    __in                                          DRM_CRYPTO_CONTEXT *pCryptoContext,  /* Pointer to DRM_CRYPTO_CONTEXT */
    __in    const                                 PRIVKEY            *pPrivkey,        /* pub key to use for encrypt */
    __in                                          DRM_BOOL            fIncludeTag,
    __out_bcount_opt(DRM_ECC160_SIGNATURE_LEN)    DRM_BYTE            rgbSign   [__CB_DECL(DRM_ECC160_SIGNATURE_LEN)], /* cannot be both NULL */
    __out_ecount_opt(DRM_ECC160_SIGNATURE_B64LEN) DRM_WCHAR           wszB64Sign[DRM_ECC160_SIGNATURE_B64LEN], /* cannot be both NULL */
    __out_opt                                     DRM_STRING         *pdstrXMLFragment)  /* optional: XML fragment of the current node, optional. */
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    cchContent=0;
    DRM_DWORD    cchTagName=0;
    DRM_STRING   dstrXMLFragment = EMPTY_DRM_STRING;
    DRM_BYTE     _rgbSign[__CB_DECL(DRM_ECC160_SIGNATURE_LEN)]={0};
    DRM_BYTE    *prgbSign = _rgbSign;
    DRM_DWORD    cbSignature = DRM_ECC160_SIGNATURE_LEN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_SignAndCloseCurrNode );

    ChkArg( pbXmlContext   != NULL
         && pCryptoContext != NULL
         && pPrivkey       != NULL
         && ( rgbSign      != NULL
           || wszB64Sign   != NULL )
         && pbXmlContext->fInited   );

    ChkDR(_CalcNodeSize(pbXmlContext, &cchContent, &cchTagName));

    if ( rgbSign != NULL )
    {
        prgbSign = rgbSign;
    }

    if ( !pbXmlContext->fIsSimMode )
    {
        if ( !fIncludeTag )
        {
            /* sign the content */
            ChkDR(DRM_PK_Sign(pCryptoContext->rgbCryptoContext,
                              eDRM_ECC_P160,
                              DRM_ECC160_PRIVATE_KEY_LEN,
                              ( const DRM_BYTE * )pPrivkey,
                              cchContent * SIZEOF(DRM_WCHAR),
                              (DRM_BYTE*)(&pbXmlContext->XmlString[pbXmlContext->wNextStringPos-cchContent]),
                              &cbSignature,
                              prgbSign));
        }
    }

    /* close the node */
    ChkDR(_CloseCurrNode(pbXmlContext, FALSE, &dstrXMLFragment));

    if ( !pbXmlContext->fIsSimMode )
    {
        if ( fIncludeTag )
        {
            /* sign the content */
            ChkDR(DRM_PK_Sign(pCryptoContext->rgbCryptoContext,
                              eDRM_ECC_P160,
                              DRM_ECC160_PRIVATE_KEY_LEN,
                              ( const DRM_BYTE * )pPrivkey,
                              CB_DSTR( &dstrXMLFragment ),
                              PB_DSTR( &dstrXMLFragment ),
                              &cbSignature,
                              prgbSign));
        }

        /* Base64 encode the signature */
        if ( wszB64Sign )
        {
            DRM_DWORD cchEncoded=DRM_ECC160_SIGNATURE_B64LEN;

            ChkDR(DRM_B64_EncodeW( prgbSign,
                                   cbSignature,
                                   (DRM_WCHAR *)wszB64Sign,
                                   &cchEncoded,
                                   0));
        }
    }

    if ( pdstrXMLFragment )
    {
        *pdstrXMLFragment = dstrXMLFragment;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_KeyedHashAndCloseCurrNode(
    __inout                                      _XMBContext        *pbXmlContext,
    __in                                         DRM_HMAC_CONTEXT   *pHmacContext,     /* HMAC context */
    __in_bcount(cbHashkey) const                 DRM_BYTE           *pbHashkey,        /* Hash key for HMAC */
    __in                                         DRM_DWORD           cbHashkey,        /* byte count of HMAC */
    __in                                         DRM_BOOL            fIncludeTag,
    __out_bcount_opt(DRM_SHA1_DIGEST_LEN)        DRM_BYTE            rgbSign   [__CB_DECL(DRM_SHA1_DIGEST_LEN)],       /* cannot be both NULL */
    __out_ecount_opt(SHA_B64ENC_DIGEST_LEN_CALC) DRM_WCHAR           wszB64Sign[DRM_SHA1_B64ENC_DIGEST_LEN], /* cannot be both NULL */
    __out_opt                                    DRM_STRING         *pdstrXMLFragment)  /* optional: XML fragment of the current node, optional. */
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    cchContent = 0;
    DRM_DWORD    cchTagName = 0;
    DRM_STRING   dstrXMLFragment = EMPTY_DRM_STRING;
    DRM_BYTE     _rgbSign [__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_BYTE    *prgbSign = _rgbSign;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_KeyedHashAndCloseCurrNode );

    DRMCASSERT( SHA_B64ENC_DIGEST_LEN_CALC == DRM_SHA1_B64ENC_DIGEST_LEN );

    ChkArg( pbXmlContext != NULL
         && pHmacContext != NULL
         && pbHashkey    != NULL
         && cbHashkey    != 0
         && ( rgbSign    != NULL
           || wszB64Sign != NULL )
         && pbXmlContext->fInited );

    ChkDR(_CalcNodeSize(pbXmlContext, &cchContent, &cchTagName));

    if ( rgbSign )
    {
        prgbSign = rgbSign;
    }


    if ( !pbXmlContext->fIsSimMode )
    {
        ChkDR(DRM_HMAC_Init(pHmacContext,eDRM_SHA_1,pbHashkey,cbHashkey));
        if ( !fIncludeTag )
        {
            /* sign the content */
            ChkDR(DRM_HMAC_Update( pHmacContext,
                      (DRM_BYTE*)(&pbXmlContext->XmlString[pbXmlContext->wNextStringPos - cchContent]),
                                   cchContent * SIZEOF(DRM_WCHAR)));
        }
    }

    /* close the node */
    ChkDR(_CloseCurrNode(pbXmlContext, FALSE, &dstrXMLFragment));

    if ( !pbXmlContext->fIsSimMode )
    {
        if ( fIncludeTag )
        {
            /* sign the content */
            ChkDR(DRM_HMAC_Update(pHmacContext, PB_DSTR(&dstrXMLFragment), CB_DSTR(&dstrXMLFragment)));
        }
        ChkDR(DRM_HMAC_Finalize(pHmacContext,prgbSign,DRM_SHA1_DIGEST_LEN));

        /* Base64 encode the signature */
        if ( wszB64Sign )
        {
            DRM_DWORD cchEncoded=DRM_SHA1_B64ENC_DIGEST_LEN;

            ChkDR(DRM_B64_EncodeW(prgbSign,
                                  DRM_SHA1_DIGEST_LEN,
                     (DRM_WCHAR *)wszB64Sign,
                                 &cchEncoded,
                                  0));
        }
    }

    if ( pdstrXMLFragment )
    {
        *pdstrXMLFragment = dstrXMLFragment;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

