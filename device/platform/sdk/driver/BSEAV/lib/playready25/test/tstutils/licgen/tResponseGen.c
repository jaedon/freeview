/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmxmlbuilder.h>
#include <drmblackbox.h>
#include <drmbase64.h>
#include <oempkcrypto.h>
#include <tResponsegen.h>
#include <SetClientID.h>
#include <drmwmdrmconstants.h>
#include <tstutils.h>

ENTER_PKTEST_NAMESPACE_CODE;

const  DRM_WCHAR        g_rgwchTagSLK            [] =  { ONE_WCHAR('S', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('\0', '\0')};
const  DRM_WCHAR        g_rgwchTagSLKID          [] =  { ONE_WCHAR('S', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('\0', '\0')};
const  DRM_WCHAR        g_rgwchTagSLKDATA        [] =  { ONE_WCHAR('S', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('K', '\0'), ONE_WCHAR('D', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('\0', '\0')};
const  DRM_WCHAR        g_rgwchLicVerAttrValue   [] =  { ONE_WCHAR('2', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('0', '\0'), ONE_WCHAR('\0', '\0')};
const  DRM_WCHAR        g_rgwchLicenseRespTag    [] =  { ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('P', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('S', '\0'),  ONE_WCHAR('E', '\0'),  ONE_WCHAR('\0', '\0')};
const  DRM_WCHAR        g_rgwchTagRevocationInfo [] =  { ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('V', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('O', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('I', '\0'),  ONE_WCHAR('N', '\0'),  ONE_WCHAR('F', '\0'),  ONE_WCHAR('O', '\0'),  ONE_WCHAR('\0', '\0')};

const  DRM_CONST_STRING g_dstrTagSLK                = CREATE_DRM_STRING( g_rgwchTagSLK    );
const  DRM_CONST_STRING g_dstrTagSLKID              = CREATE_DRM_STRING( g_rgwchTagSLKID  );
const  DRM_CONST_STRING g_dstrTagSLKDATA            = CREATE_DRM_STRING( g_rgwchTagSLKDATA);
const  DRM_CONST_STRING g_dstrLicVerAttrValue       = CREATE_DRM_STRING( g_rgwchLicVerAttrValue );
const  DRM_CONST_STRING g_dstrLicenseRespTag        = CREATE_DRM_STRING( g_rgwchLicenseRespTag );
const  DRM_CONST_STRING g_dstrTagRevocationInfo     = CREATE_DRM_STRING( g_rgwchTagRevocationInfo );

DRM_RESULT TestResponseInit(__in _XMBContext *pbContext, __in DRM_DWORD cbContext)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkDR(DRM_XMB_CreateDocument(cbContext, pbContext, &g_dstrLicenseRespTag));

ErrorExit:
    return dr;
}

DRM_RESULT TestResponseAddNode(__in _XMBContext *pbContext,
                            __in const DRM_CONST_STRING *pwszNodeName,
                            __in const DRM_CONST_STRING *pwszAttrName,
                            __in const DRM_CONST_STRING *pwszAttrValue,
                            __in const DRM_BYTE *pbData,
                            __in const DRM_DWORD cbData)
{
    DRM_RESULT dr;
    DRM_CONST_STRING wszEncoded = {0};

    /*encode data*/
    dr=DRM_B64_EncodeW(pbData, cbData, NULL, &wszEncoded.cchString, 0);
    if(dr!=DRM_E_BUFFERTOOSMALL){
        ChkDR(dr);
    }
#pragma prefast(push)
#pragma prefast(disable:22011) /* disable warning abot arithmetic overflox in malloc */
    ChkOverflow(wszEncoded.cchString * SIZEOF(DRM_WCHAR), wszEncoded.cchString);
    ChkMem(wszEncoded.pwszString = (DRM_WCHAR*)Oem_MemAlloc(wszEncoded.cchString * SIZEOF(DRM_WCHAR)));
#pragma prefast(pop)
    ChkDR(DRM_B64_EncodeW(pbData, cbData, (DRM_WCHAR*)wszEncoded.pwszString, &wszEncoded.cchString, 0));

    /*wrap the data*/
    ChkDR(DRM_XMB_OpenNode(pbContext, pwszNodeName));
    if (pwszAttrName && pwszAttrName->pwszString) {
        ChkDR(DRM_XMB_AddAttribute(pbContext, pwszAttrName, pwszAttrValue));
    }
    ChkDR(DRM_XMB_AddData(pbContext, &wszEncoded));
    ChkDR(DRM_XMB_CloseCurrNode(pbContext, NULL));

ErrorExit:
    FREEDRMSTRING( wszEncoded );
    return dr;
}

/*****************************************************************************
** Function:    TestResponseAddSLK
**
** Synopsis:    This inserts a properly formated V2 SLK node into an already
**      intialized XMBContext Response context. The SLK node includes two
**      sub nodes, the SLKID and the SLKDATA nodes. The first containing
**      the 16 byte session ID, b64 encoded. The second containing the 8 byte SLK Key,
**      encrypted by the device public key and b64 encoded.
**
** Arguments:   *pbContext  : _XMBContext already passed through a call to TestResponseInit
**              *pdstrSlkId : DRM_CONST_STRING with 16 bytes of data, in b64 encoded form
**              *pdstrSlkData : DRM_CONST_STRING with b64 encoded form (& encrypted) SLK
**
** Returns:     DRM_SUCCESS         - on success
**
** Note:    This function should be called BEFORE TestResponseAddLicense, not AFTER.
**
******************************************************************************/
DRM_RESULT TestResponseAddSLK(
    __in       _XMBContext      *pbContext,
    __in const DRM_CONST_STRING *pdstrSlkId,
    __in const DRM_CONST_STRING *pdstrSlkData )
{
        DRM_RESULT dr = DRM_SUCCESS;

        ChkDR(DRM_XMB_OpenNode(pbContext, &g_dstrTagSLK));
        ChkDR(DRM_XMB_OpenNode(pbContext, &g_dstrTagSLKID));
        ChkDR(DRM_XMB_AddData(pbContext, pdstrSlkId));
        ChkDR(DRM_XMB_CloseCurrNode(pbContext, NULL));
        ChkDR(DRM_XMB_OpenNode(pbContext, &g_dstrTagSLKDATA));
        ChkDR(DRM_XMB_AddData(pbContext, pdstrSlkData));
        ChkDR(DRM_XMB_CloseCurrNode(pbContext, NULL));
        ChkDR(DRM_XMB_CloseCurrNode(pbContext, NULL));

    ErrorExit:
        return dr;
}


DRM_RESULT TestResponseAddLicense(__in _XMBContext *pbContext, __in DRM_CONST_STRING *pwszLicense)
{
    DRM_RESULT dr;
    DRM_DWORD dwSize=0;
    DRM_BYTE *pbEncryptedBuff=NULL;
    DRM_CRYPTO_CONTEXT *pCryptoContext = NULL;

    ChkMem(pCryptoContext = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pCryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    /* Encrypt the license with the blackbox public key */
    dwSize = pwszLicense->cchString * SIZEOF(DRM_WCHAR);

    ChkOverflow( dwSize + DRM_ECC160_CIPHERTEXT_LEN, dwSize );
    ChkMem(pbEncryptedBuff = (DRM_BYTE*)Oem_MemAlloc(dwSize + DRM_ECC160_CIPHERTEXT_LEN));
    ChkDR(DRM_PK_EncryptLarge(NULL, &(g_ClientID.pk.pk.pk), (DRM_BYTE*)(pwszLicense->pwszString),
        dwSize, pbEncryptedBuff, DRMCIPHERKEYLEN_RC4, pCryptoContext));

     dwSize += DRM_ECC160_CIPHERTEXT_LEN;

    ChkDR(TestResponseAddNode(pbContext, &g_dstrLicense, &g_dstrAttributeVersion, &g_dstrLicVerAttrValue, pbEncryptedBuff, dwSize));

ErrorExit:
    SAFE_OEM_FREE(pbEncryptedBuff);
    SAFE_OEM_FREE(pCryptoContext);
    return dr;
}

DRM_RESULT TestResponseAddRevocationInfo(
    __in        _XMBContext      *pbContext,
    __in  const DRM_CONST_STRING *pwszRevocationInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR(DRM_XMB_OpenNode(pbContext, &g_dstrTagRevocationInfo));
    ChkDR(DRM_XMB_AddData(pbContext, pwszRevocationInfo));
    ChkDR(DRM_XMB_CloseCurrNode(pbContext, NULL));

ErrorExit:
    return dr;
}

DRM_RESULT TestResponseAddRevocation(
    __in        _XMBContext      *pbContext,
    __in  const DRM_CONST_STRING *pwszGUID,
    __in  const DRM_CONST_STRING *pwszRevocation )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR(DRM_XMB_OpenNode(pbContext, &g_dstrTagRevocation));
    ChkDR(DRM_XMB_AddAttribute(pbContext, &g_dstrAttributeType, pwszGUID));
    ChkDR(DRM_XMB_AddData(pbContext, pwszRevocation));
    ChkDR(DRM_XMB_CloseCurrNode(pbContext, NULL));

ErrorExit:
    return dr;
}

/*The caller is responsible to SAFE_OEM_FREE ppbResponse*/
DRM_RESULT TestResponseGetResponse(__in _XMBContext *pbContext, __deref_out_ecount_opt(*pcbResponse) DRM_BYTE **ppbResponse, __out DRM_DWORD *pcbResponse)
{
     DRM_RESULT dr = DRM_SUCCESS;
    DRM_STRING pwszString;
    DRM_DWORD cbResponse;

    ChkArg(ppbResponse);
    ChkDR(DRM_XMB_CloseDocument(pbContext,&pwszString));
    cbResponse = pwszString.cchString + 1;
    ChkMem(*ppbResponse = (DRM_BYTE *)Oem_MemAlloc(cbResponse));

    /*convert wide char to ansi*/
    DRM_UTL_DemoteUNICODEtoASCII((const DRM_WCHAR *)pwszString.pwszString,(DRM_CHAR *) *ppbResponse,pwszString.cchString);


    if (pcbResponse)
        *pcbResponse = cbResponse;
ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

