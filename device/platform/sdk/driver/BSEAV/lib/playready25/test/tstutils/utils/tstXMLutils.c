/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tstXMLutils.h"
#include "drmcrt.h"
#include "drmxmlparser.h"
#include <drmwmdrmconstants.h>


DRM_RESULT DRM_CALL VerifyXMLDataSigature(const DRM_CONST_STRING *pwszXML, const DRM_CONST_STRING *pwszPubKey)
{
    DRM_RESULT dr;
    PUBKEY pubKey;
    DRM_DWORD dwSize;
    DRM_CONST_STRING wszSignature, wszData;
    DRM_CRYPTO_CONTEXT *pCrypto = NULL;

    ChkMem(pCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_CRYPTO_CONTEXT)));
    ZEROMEM( pCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    dwSize = SIZEOF(PUBKEY);
    ChkDR(DRM_B64_DecodeW(pwszPubKey, &dwSize, (DRM_BYTE*)&pubKey, 0));

    ChkDR(DRM_XML_GetSubNodeByPath(pwszXML, &g_dstrXPathSigValue, NULL, NULL, NULL, &wszSignature, g_wchForwardSlash));

    ChkDR(DRM_XML_GetAndVerifySubNode(pwszXML, &g_dstrTagData, NULL, NULL, 0,
        pCrypto, &pubKey, TRUE, &wszSignature, &wszData, NULL, 1));
ErrorExit:
    SAFE_OEM_FREE(pCrypto);
    return dr;
}
