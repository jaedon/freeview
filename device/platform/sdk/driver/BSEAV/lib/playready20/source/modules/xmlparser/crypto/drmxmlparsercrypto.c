/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmxmlparser.h>
#include <drmcrt.h>
#include <drmbase64.h>
#include <drmutilities.h>
#include <oemcipher.h>
#include <oemshahmac.h>
#include <oem.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_XML_GetAndDecryptNode(
    __in  const DRM_CONST_STRING   *f_pdstrXML,
    __in  const DRM_CONST_STRING   *f_pdstrTag,
    __in  const DRM_CONST_STRING   *f_pdstrAttrName,
    __in  const DRM_CONST_STRING   *f_pdstrAttrValue,
    __in  const DRM_DWORD           f_iNode,           /* nth occurence of the node in the xml stirng, start from 0 */
    __in        DRM_CRYPTO_CONTEXT *f_pcontextCRYP,    /* Pointer to DRM_CRYPTO_CONTEXT */
    __in  const PRIVKEY            *f_pprivkey,        /* priv key to use for decrypt */
    __out       DRM_CONST_STRING   *f_pdstrNode,       /* either of these 2 parameter can be NULL but not both */
    __out       DRM_CONST_STRING   *f_pdstrNodeData )  /* data enclosed by the immediate <tag>...</tag> in the given XML string */
{
    return DRM_XML_GetAndDecryptSubNode(f_pdstrXML,
                                        f_pdstrTag,
                                        f_pdstrAttrName,
                                        f_pdstrAttrValue,
                                        f_iNode,
                                        f_pcontextCRYP,
                                        f_pprivkey,
                                        f_pdstrNode,
                                        f_pdstrNodeData,
                                        0);
}

/*
** extract license node from given XML string matching <tag AttrName="Value"> ... </tag>
*/
DRM_API DRM_RESULT DRM_CALL DRM_XML_GetAndDecryptSubNode(
    __in  const DRM_CONST_STRING   *f_pdstrXML,
    __in  const DRM_CONST_STRING   *f_pdstrTag,
    __in  const DRM_CONST_STRING   *f_pdstrAttrName,
    __in  const DRM_CONST_STRING   *f_pdstrAttrValue,
    __in  const DRM_DWORD           f_iNode,         /* nth occurence of the node in the xml stirng, start from 0 */
    __in        DRM_CRYPTO_CONTEXT *f_pcontextCRYP,  /* Pointer to DRM_CRYPTO_CONTEXT */
    __in  const PRIVKEY            *f_pprivkey,      /* priv key to use for decrypt */
    __out       DRM_CONST_STRING   *f_pdstrNode,     /* these two parameters can either be NULL but not both */
    __out       DRM_CONST_STRING   *f_pdstrNodeData,
    __in        DRM_DWORD           f_iLayer )       /* outermost layer is 0 */
{
    DRM_CONST_STRING dstrXMLNode     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrXMLNodeData = EMPTY_DRM_STRING;
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_DWORD        cbDecoded     = 0;
    DRM_DWORD        i           = 0;
    DRM_WCHAR       *_pwszString = NULL;

    ChkArg(f_pcontextCRYP  != NULL
        && f_pprivkey      != NULL);

    ChkArg(f_pdstrNode     != NULL
        || f_pdstrNodeData != NULL);

    /* get the node first */

    ChkDR(DRM_XML_GetSubNode(f_pdstrXML,
                             f_pdstrTag,
                             f_pdstrAttrName,
                             f_pdstrAttrValue,
                             f_iNode,
                            &dstrXMLNode,
                            &dstrXMLNodeData,
                             f_iLayer));

    /* base64 decode in-place */

    ChkDR(DRM_B64_DecodeW(&dstrXMLNodeData, &cbDecoded, NULL, DRM_BASE64_DECODE_IN_PLACE));

    /* decrypt the node data in-place */

    ChkDR (DRM_PK_DecryptLarge (f_pprivkey,
                                PB_DSTR(&dstrXMLNodeData),
                                cbDecoded,
                                PB_DSTR(&dstrXMLNodeData),
                                f_pcontextCRYP));

    /* fill the "empty space" with blanks */
    cbDecoded = (cbDecoded - DRM_ECC160_CIPHERTEXT_LEN) / SIZEOF(DRM_WCHAR);

    _pwszString = (DRM_WCHAR*)(dstrXMLNodeData.pwszString);
    for (i = 0; i < (dstrXMLNodeData.cchString - cbDecoded); i++)
    {
        _pwszString [cbDecoded+i] = g_wchSpace;
    }

    dstrXMLNodeData.cchString = cbDecoded;

    /* prepare for return value */
    if (f_pdstrNode != NULL)
    {
        ASSIGN_DRM_STRING(*f_pdstrNode, dstrXMLNode);
    }

    if (f_pdstrNodeData != NULL)
    {
        ASSIGN_DRM_STRING(*f_pdstrNodeData, dstrXMLNodeData);
    }

ErrorExit:

    return DRM_XML_TranslateXMLError(dr);
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_GetAndVerifyKeyedHashNode(
    __in                     const DRM_CONST_STRING *f_pdstrXML,
    __in                     const DRM_CONST_STRING *f_pdstrTag,
    __in                     const DRM_CONST_STRING *f_pdstrAttrName,
    __in                     const DRM_CONST_STRING *f_pdstrAttrValue,
    __in                     const DRM_DWORD         f_iNode,              /* nth occurence of the node in the xml stirng, start from 0 */
    __in                           DRM_HMAC_CONTEXT *f_pcontextHMAC,
    __in_ecount(f_cbKeyHash) const DRM_BYTE         *f_pbKeyHash,          /* Hash key for HMAC */
    __in                           DRM_DWORD         f_cbKeyHash,          /* byte count of HMAC */
    __in                           DRM_BOOL          f_fIncludeTag,
    __in                     const DRM_CONST_STRING *f_pdstrB64Signature,
    __out                          DRM_CONST_STRING *f_pdstrNode,          /* either of these 2 parameter can be NULL but not both */
    __out                          DRM_CONST_STRING *f_pdstrNodeData )     /* data enclosed by the immediate <tag>...</tag> in the given XML string */
{
    return DRM_XML_GetAndVerifyKeyedHashSubNode(f_pdstrXML,
                                                f_pdstrTag,
                                                f_pdstrAttrName,
                                                f_pdstrAttrValue,
                                                f_iNode,
                                                f_pcontextHMAC,
                                                f_pbKeyHash,
                                                f_cbKeyHash,
                                                f_fIncludeTag,
                                                f_pdstrB64Signature,
                                                f_pdstrNode,
                                                f_pdstrNodeData,
                                                0);
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_GetAndVerifyKeyedHashSubNode(
    __in                     const DRM_CONST_STRING       *f_pdstrXML,
    __in                     const DRM_CONST_STRING       *f_pdstrTag,
    __in                     const DRM_CONST_STRING       *f_pdstrAttrName,
    __in                     const DRM_CONST_STRING       *f_pdstrAttrValue,
    __in                     const DRM_DWORD               f_iNode,            /* nth occurence of the node in the xml stirng, start from 0 */
    __in                           DRM_HMAC_CONTEXT       *f_pcontextHMAC,     /* HMAC context */
    __in_ecount(f_cbKeyHash) const DRM_BYTE               *f_pbKeyHash,        /* Hash key for HMAC */
    __in                           DRM_DWORD               f_cbKeyHash,        /* byte count of HMAC */
    __in                           DRM_BOOL                f_fIncludeTag,
    __in                     const DRM_CONST_STRING       *f_pdstrB64Signature,
    __out                          DRM_CONST_STRING       *f_pdstrNode,        /* either of these 2 parameter can be NULL but not both */
    __out                          DRM_CONST_STRING       *f_pdstrNodeData,    /* data enclosed by the immediate <tag>...</tag> in the given XML string */
    __in                           DRM_DWORD               f_iLayer)           /* outermost layer is 0 */
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrXMLNode;
    DRM_CONST_STRING dstrXMLNodeData;
    DRM_DWORD        cbDecoded = DRM_SHA1_DIGEST_LEN;
    DRM_BYTE         rgbSign   [__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_BYTE         rgbVerify [__CB_DECL(DRM_SHA1_DIGEST_LEN)];

    ChkArg(f_pcontextHMAC      != NULL
        && f_pbKeyHash         != NULL
        && f_cbKeyHash         != 0
        && f_pdstrB64Signature != NULL);

    ChkArg(f_pdstrNode     != NULL
        || f_pdstrNodeData != NULL);

    ChkArg(f_pdstrB64Signature->cchString == DRM_SHA1_B64ENC_DIGEST_LEN);

    /* get the node first */
    ChkDR(DRM_XML_GetSubNode(f_pdstrXML,
                             f_pdstrTag,
                             f_pdstrAttrName,
                             f_pdstrAttrValue,
                             f_iNode,
                            &dstrXMLNode,
                            &dstrXMLNodeData,
                             f_iLayer));

    /* base64 decode the signature */
    ChkDR(DRM_B64_DecodeW(f_pdstrB64Signature, &cbDecoded, rgbSign, 0));

    ChkDR(DRM_HMAC_Init(f_pcontextHMAC, eDRM_SHA_1, f_pbKeyHash, f_cbKeyHash));

    if (f_fIncludeTag)
    {
        ChkDR(DRM_HMAC_Update(f_pcontextHMAC,
                              PB_DSTR (&dstrXMLNode),
                              CB_DSTR (&dstrXMLNode)));
    }
    else
    {
        ChkDR(DRM_HMAC_Update(f_pcontextHMAC,
                              PB_DSTR (&dstrXMLNodeData),
                              CB_DSTR (&dstrXMLNodeData)));
    }

    ChkDR(DRM_HMAC_Finalize(f_pcontextHMAC, rgbVerify, DRM_SHA1_DIGEST_LEN));

    if (MEMCMP(rgbVerify, rgbSign, DRM_SHA1_DIGEST_LEN) != 0)
    {
        ChkDR(DRM_E_HASHMISMATCH);
    }

    /* prepare for return value */
    if (f_pdstrNode != NULL)
    {
        ASSIGN_DRM_STRING(*f_pdstrNode, dstrXMLNode);
    }

    if (f_pdstrNodeData != NULL)
    {
        ASSIGN_DRM_STRING(*f_pdstrNodeData, dstrXMLNodeData);
    }

ErrorExit:

    return DRM_XML_TranslateXMLError(dr);
}

/*
** extract license node from given XML string matching <tag AttrName="Value"> ... </tag>
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetAndVerifyNode(
    __in  const DRM_CONST_STRING   *f_pdstrXML,
    __in  const DRM_CONST_STRING   *f_pdstrTag,
    __in  const DRM_CONST_STRING   *f_pdstrAttrName,
    __in  const DRM_CONST_STRING   *f_pdstrAttrValue,
    __in  const DRM_DWORD           f_iNode,           /* nth occurence of the node in the xml stirng, start from 0 */
    __in        DRM_CRYPTO_CONTEXT *f_pcontextCRYP,    /* Pointer to DRM_CRYPTO_CONTEXT */
    __in  const PUBKEY             *f_ppubkey,         /* pub key to use for verify */
    __in        DRM_BOOL            f_fIncludeTag,
    __in  const DRM_CONST_STRING   *f_pdstrB64Signature,
    __out       DRM_CONST_STRING   *f_pdstrNode,       /* either of these 2 parameter can be NULL but not both */
    __out       DRM_CONST_STRING   *f_pdstrNodeData )  /* data enclosed by the immediate <tag>...</tag> in the given XML string */
{
    return DRM_XML_GetAndVerifySubNode(f_pdstrXML,
                                       f_pdstrTag,
                                       f_pdstrAttrName,
                                       f_pdstrAttrValue,
                                       f_iNode,
                                       f_pcontextCRYP,
                                       f_ppubkey,
                                       f_fIncludeTag,
                                       f_pdstrB64Signature,
                                       f_pdstrNode,
                                       f_pdstrNodeData,
                                       0);
}

/*
** extract license node from given XML string matching <tag AttrName="Value"> ... </tag>
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetAndVerifySubNode(
    __in  const DRM_CONST_STRING   *f_pdstrXML,
    __in  const DRM_CONST_STRING   *f_pdstrTag,
    __in  const DRM_CONST_STRING   *f_pdstrAttrName,
    __in  const DRM_CONST_STRING   *f_pdstrAttrValue,
    __in  const DRM_DWORD           f_iNode,            /* nth occurence of the node in the xml stirng, start from 0 */
    __in        DRM_CRYPTO_CONTEXT *f_pcontextCRYP,     /* Pointer to DRM_CRYPTO_CONTEXT */
    __in  const PUBKEY             *f_ppubkey,          /* pub key to use for verify */
    __in        DRM_BOOL            f_fIncludeTag,
    __in  const DRM_CONST_STRING   *f_pdstrB64Signature,
    __out       DRM_CONST_STRING   *f_pdstrNode,        /* these two parameters can either be NULL but not both */
    __out       DRM_CONST_STRING   *f_pdstrNodeData,
    __in        DRM_DWORD           f_iLayer )          /* outermost layer is 0 */
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrXMLNode;
    DRM_CONST_STRING dstrXMLNodeData;
    DRM_DWORD        cbDecoded = DRM_ECC160_SIGNATURE_LEN;
    DRM_BYTE         rgbSign [__CB_DECL(DRM_ECC160_SIGNATURE_LEN)] = { 0 };

    ChkArg(f_pcontextCRYP      != NULL
        && f_ppubkey           != NULL
        && f_pdstrB64Signature != NULL);

    ChkArg(f_pdstrNode    != NULL
       || f_pdstrNodeData != NULL);

    ChkArg(f_pdstrB64Signature->cchString == DRM_ECC160_SIGNATURE_B64LEN);

    /* get the node first */
    ChkDR(DRM_XML_GetSubNode(f_pdstrXML,
                             f_pdstrTag,
                             f_pdstrAttrName,
                             f_pdstrAttrValue,
                             f_iNode,
                            &dstrXMLNode,
                            &dstrXMLNodeData,
                             f_iLayer));

    /* base64 decode the signature */
    ChkDR(DRM_B64_DecodeW(f_pdstrB64Signature, &cbDecoded, rgbSign, 0));

    /* verify the node with the signature */
    if (f_fIncludeTag)
    {
        ChkDR( DRM_PK_Verify( f_pcontextCRYP->rgbCryptoContext,
                              eDRM_ECC_P160,
                              DRM_ECC160_PUBLIC_KEY_LEN,
                              ( const DRM_BYTE * )f_ppubkey,
                              CB_DSTR( &dstrXMLNode ),
                              PB_DSTR( &dstrXMLNode ),
                              DRM_ECC160_SIGNATURE_LEN,
                              rgbSign ) );
    }
    else
    {
        ChkDR( DRM_PK_Verify( f_pcontextCRYP->rgbCryptoContext,
                              eDRM_ECC_P160,
                              DRM_ECC160_PUBLIC_KEY_LEN,
                              ( const DRM_BYTE * )f_ppubkey,
                              CB_DSTR( &dstrXMLNodeData ),
                              PB_DSTR( &dstrXMLNodeData ),
                              DRM_ECC160_SIGNATURE_LEN,
                              rgbSign ) );
    }

    /* prepare for return value */
    if (f_pdstrNode != NULL)
    {
        ASSIGN_DRM_STRING(*f_pdstrNode, dstrXMLNode);
    }
    if (f_pdstrNodeData != NULL)
    {
        ASSIGN_DRM_STRING(*f_pdstrNodeData, dstrXMLNodeData);
    }

ErrorExit:

    return DRM_XML_TranslateXMLError(dr);
}

EXIT_PK_NAMESPACE_CODE;

