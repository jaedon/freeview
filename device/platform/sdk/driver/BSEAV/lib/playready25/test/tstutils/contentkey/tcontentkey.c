/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#define BUFFERSIZE 20
#include <stdafx.h>

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

#include <drmcommon.h>
#include <drmbase64.h>
#include <drmxmlparser.h>
#include <drmkeygenerator.h>
#include <drmcontextsizes.h>
#include <tRightsParser.h>
#include <oemshahash.h>

extern DRM_CONST_STRING g_dstrDefaultCocktailKeySeed;
extern DRM_CONST_STRING g_dstrDefaultAESKeySeed;

ENTER_PKTEST_NAMESPACE;
extern DRM_RESULT DRM_CALL CalculateCheckSum(
    __in  const DRM_BYTE  *pbData,
    __in        DRM_DWORD  cbData,
    __out       DRM_BYTE  *pbCheckSum,
    __in        DRM_DWORD  cbCheckSum,
    __in  const DRM_KID   *pKID );

static const DRM_WCHAR dwchXMRVersion1[]                = { WCHAR_CAST('1'), WCHAR_CAST('\0') };
EXIT_PKTEST_NAMESPACE;

extern DRM_RESULT _GetRights(const DRM_CONST_STRING *dstrNode, LICENSERIGHTS **ppRetRights, DRM_CONST_STRING dstrRIV);

DRM_RESULT tDRM_B64_Decode2( __in DRM_CONST_STRING *szEncoded, __out DRM_DWORD *pcbDecode, __out DRM_BYTE **ppbDecode)
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( szEncoded != NULL );
    ChkArg( pcbDecode != NULL );
    ChkArg( ppbDecode != NULL );

    /* szEncoded size includes the null terminated char which breaks DRM_B64_Decode */
    if (szEncoded->cchString > 0 && !szEncoded->pwszString[szEncoded->cchString - 1])
    {
        szEncoded->cchString--;
    }

    dr = DRM_B64_DecodeW(szEncoded, pcbDecode, NULL, 0);
    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }
    SAFE_OEM_FREE(*ppbDecode);

    ChkMem(*ppbDecode = ( DRM_BYTE* ) Oem_MemAlloc((*pcbDecode)*SIZEOF(DRM_WCHAR)));
    ChkDR(DRM_B64_DecodeW(szEncoded, pcbDecode, *ppbDecode, 0));
ErrorExit:
    return dr;
}

DRM_RESULT tDRM_KG_GenerateContentKey2( __in DRM_CONST_STRING *pszKeySeed, __in DRM_CONST_STRING *pszKid, __out DRM_CONST_STRING *pszKey, __in DRM_BOOL fAESCrypto )
{
    DRM_RESULT            dr               = DRM_SUCCESS;
    DRM_DWORD             cchKID           = pszKid->cchString;
    DRM_DWORD             cchKey           = 0;
    DRM_SUPPORTED_CIPHERS eCipher          = eDRM_CIPHER_INVALID;
    DRM_DWORD             cbKeyLen         = 0;
    DRM_BYTE             *pbScratchKeySeed = NULL;
    DRM_DWORD             cbScratchKeySeed = 0;

    if ( fAESCrypto )
    {
        eCipher = eDRM_AES_COUNTER_CIPHER;
        cbKeyLen = DRM_AES_KEYSIZE_128;
    }
    else
    {
        eCipher = eDRM_RC4_CIPHER;
        cbKeyLen = DRMCIPHERKEYLEN_RC4;
    }

    cbScratchKeySeed = pszKeySeed->cchString * SIZEOF(DRM_WCHAR);
    ChkOverflow( pszKeySeed->cchString * SIZEOF(DRM_WCHAR), pszKeySeed->cchString );
    ChkMem(pbScratchKeySeed = (DRM_BYTE*)Oem_MemAlloc( cbScratchKeySeed ) );
    ZEROMEM( pbScratchKeySeed, cbScratchKeySeed );

    dr = DRM_KG_GenerateContentKeyB64(
        pszKeySeed->pwszString,
        pszKeySeed->cchString,
        pszKid->pwszString,
        cchKID,
        eCipher,
        cbKeyLen,
        NULL,
        &pszKey->cchString,
        pbScratchKeySeed,
        cbScratchKeySeed);

    if (dr == DRM_E_INVALIDARG && cchKID >= DRM_KEYSEED_BASE64_LEN) {
        /*    DRM_KG_GenerateContentKey limits the KID length. The content key generated from this function
            will not match the one from WMRM for long KIDs. This is OK for our test.
        */
        cchKID = DRM_KEYSEED_BASE64_LEN - 1;
        dr = DRM_KG_GenerateContentKeyB64(
            pszKeySeed->pwszString,
            pszKeySeed->cchString,
            pszKid->pwszString,
            cchKID,
            eCipher,
            cbKeyLen,
            NULL,
            &pszKey->cchString,
            pbScratchKeySeed,
            cbScratchKeySeed );
    }

    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }

    cchKey = pszKey->cchString;
    FREEDRMSTRINGPOINTER( pszKey );
    pszKey->cchString = cchKey;

    ChkMem(pszKey->pwszString = (DRM_WCHAR*)Oem_MemAlloc(pszKey->cchString * SIZEOF(DRM_WCHAR)*2));
    ChkDR(DRM_KG_GenerateContentKeyB64(
        (DRM_WCHAR*)pszKeySeed->pwszString,
        pszKeySeed->cchString,
        (DRM_WCHAR*)pszKid->pwszString,
        cchKID,
        eCipher,
        cbKeyLen,
        (DRM_WCHAR*)pszKey->pwszString,
        &pszKey->cchString,
        pbScratchKeySeed,
        cbScratchKeySeed ) );

ErrorExit:
    SAFE_OEM_FREE( pbScratchKeySeed );

    return dr;
}

/*****************************************************************************
** Function: _GenerateContentKey
**
** Synopsis: Generates a content key using a server seed and KID
**
** Arguments:   pszServerSeed - the server seed.
**              pszKid - the KID to base the content key on
**              pcbClearKey - size of the clear content key
**              ppbClearContentKey - the clear content key.  Free using SAFE_OEM_FREE
**              cbCheckSum - the size of pbCheckSum
**              pbCheckSum - buffer to put the check sum in
**              dwXMRVersion - the version of XMR to use
**              fAESCrypto - If true use AESCrypto to generate the key
**
** Returns:
**          DRM_SUCCESS
*****************************************************************************/
DRM_RESULT DRM_CALL _GenerateContentKey( __in  DRM_CONST_STRING *pszServerSeed,
                                         __in  DRM_CONST_STRING *pszKid,
                                         __out DRM_DWORD        *pcbClearKey,
                                         __out DRM_BYTE        **ppbClearContentKey,
                                         __in  DRM_DWORD         cbCheckSum,
                                         __out DRM_BYTE         *pbCheckSum,
                                         __in  DRM_DWORD         dwXmrVersion,
                                         __in  DRM_BOOL          fAESCrypto )
{
    DRM_RESULT       dr                            = DRM_SUCCESS;
    DRM_CONST_STRING szStr                         = {0};
    DRM_BYTE         pbDesKey[DES_KEYSIZE]         = {0};
    DRM_BYTE         pbAESKey[DRM_AES_KEYSIZE_128] = {0};
    DRM_KID          drmKID                        = {0};
    DRM_DWORD        cb                            = SIZEOF(DRM_KID);

    ChkArg( pszServerSeed );
    ChkArg( pszKid );
    ChkArg( pcbClearKey );
    ChkArg( ppbClearContentKey );

    /* generate the content key with keyid and keyseed */
    ChkDR(tDRM_KG_GenerateContentKey2( pszServerSeed, pszKid, &szStr, dwXmrVersion == XMR_VERSION_3 ? fAESCrypto : FALSE ) );

    /* Decode the content key */
    ChkDR(tDRM_B64_Decode2(&szStr, pcbClearKey, ppbClearContentKey));

    if (pbCheckSum && cbCheckSum)    /* want checksum */
    {
        if( dwXmrVersion == XMR_VERSION_1 )
        {
            /* The uplink key is an 8-byte DES key. We pad a zero byte to the 7-byte content key to the DES key */
            memcpy(pbDesKey, *ppbClearContentKey, min(DES_KEYSIZE, *pcbClearKey));

            /* Add the CHAINCHECKSUM block */
            ChkDR(CalculateCheckSum(pbDesKey, DES_KEYSIZE, pbCheckSum, cbCheckSum, NULL));
        }
        else if ( dwXmrVersion == XMR_VERSION_3 )
        {
            /* The V3XMR checksum algorithm requires the KID and generates an 8-byte checksum */
            ChkArg( pszKid != NULL );

            memcpy(pbAESKey, *ppbClearContentKey, min(DRM_AES_KEYSIZE_128, *pcbClearKey));

            ChkDR(DRM_B64_DecodeW(pszKid, &cb, (DRM_BYTE  *)&drmKID, 0));
            ChkDR(CalculateCheckSum(pbAESKey, DRM_AES_KEYSIZE_128, pbCheckSum, cbCheckSum, &drmKID));
        }
        else
        {
            ChkDR( DRM_E_NOTIMPL );
        }
    }

ErrorExit:
    FREEDRMSTRING( szStr );
    return dr;
}

/*****************************************************************************
** Function: _tGetContentKeyFromLicenseRightsNode
**
** Synopsis: Gets a content key from a license rights node.
**
** Arguments:   pNode - the license rights node.
**              ppContentKey - the buffer to put the content key
**              pcbContentKey - the lenfth of pbContentKey
**              pwszOverrideKID - Overrides the KID specified in the rights node
**
** Returns:
**          DRM_SUCCESS
*****************************************************************************/
DRM_RESULT DRM_CALL _tGetContentKeyFromLicenseRightsNode( __in  const DRM_WCHAR *pNode,
                                                          __out DRM_BYTE       **ppContentKey,
                                                          __out DRM_DWORD       *pcbContentKey,
                                                          __in  const DRM_WCHAR *pwszOverrideKID)
{
    DRM_RESULT        dr              = DRM_SUCCESS;
    DRM_CONST_STRING  dstrNode        = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrRiv         = EMPTY_DRM_STRING;
    DRM_CONST_STRING  dstrKID         = EMPTY_DRM_STRING;
    DRM_CONST_STRING *pdstrServerSeed = NULL;
    DRM_DWORD         cbClear         = 0;
    DRM_DWORD         dwXmrVersion    = XMR_VERSION_3;
    DRM_BOOL          fAESCrypto      = TRUE;
    DRM_BYTE         *pbClear         = NULL;
    LICENSERIGHTS    *pRights         = NULL;
    LICENSERIGHTS    *pTopRightsNode  = NULL;

    ChkArg(pNode);
    ChkArg(ppContentKey);
    ChkArg(pcbContentKey);

    dstrNode.pwszString = pNode;
    dstrNode.cchString = (DRM_DWORD)DRMCRT_wcslen(pNode);

    ChkDR(_GetRights(&dstrNode, &pTopRightsNode, dstrRiv));

    if( pTopRightsNode->pLeaf != NULL )
    {
        pRights = pTopRightsNode->pLeaf;  // use the first leaf found
    }
    else
    {
        pRights = pTopRightsNode;
    }

    if ( IsRightSet(pRights->XMRVersion) )
    {
        if ( 0 == DRMCRT_wcsicmp( pRights->XMRVersion.pwszString, dwchXMRVersion1 ) )
        {
            dwXmrVersion = XMR_VERSION_1;
        }
    }

    if ( IsRightSet(pRights->UseAESContentKey) && !IsRightTrue(pRights->UseAESContentKey) )
    {
        fAESCrypto = FALSE;
    }

    if (IsRightSet(pRights->ContentKey))
    {
        dr = DRM_B64_DecodeW(&pRights->ContentKey, &cbClear, NULL, 0);
        if (dr == DRM_E_BUFFERTOOSMALL)
        {
            ChkMem(pbClear = (DRM_BYTE*)Oem_MemAlloc(cbClear));
            ChkDR(DRM_B64_DecodeW(&pRights->ContentKey, &cbClear, pbClear, 0));
            *ppContentKey = pbClear;
            pbClear = NULL;
            *pcbContentKey = cbClear;
        }
        else
        {
            ChkDR(dr);
            ChkDR(DRM_E_XMLSIG_INVALID_KEY_FORMAT);  /* error out if DRM_SUCCESS */
        }
    }
    else
    {
        if (IsRightSet(pRights->ServerSeed))
        {
            pdstrServerSeed = &pRights->ServerSeed;
        }
        else
        {
            if( fAESCrypto )
            {
                pdstrServerSeed = &g_dstrDefaultAESKeySeed;
            }
            else
            {
                pdstrServerSeed = &g_dstrDefaultCocktailKeySeed;
            }
        }

        if (pwszOverrideKID)
        {
            dstrKID.pwszString = pwszOverrideKID;
            dstrKID.cchString = (DRM_DWORD)DRMCRT_wcslen(pwszOverrideKID);
        }
        else
        {
            dstrKID.pwszString = pRights->KID.pwszString;
            dstrKID.cchString = pRights->KID.cchString;
        }
        ChkDR( _GenerateContentKey( pdstrServerSeed, &dstrKID, pcbContentKey, ppContentKey, 0, NULL, dwXmrVersion, fAESCrypto ) );
    }


ErrorExit:
    FreeLicenseRights(pTopRightsNode);
    SAFE_OEM_FREE( pbClear );

    return dr;
}

