/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <ndtrevguids.h>
#include <ndtnetmsg.h>
#include <ndtreg.h>

ENTER_PKTEST_NAMESPACE_CODE;

DRM_CHAR DRM_API TST_UTL_tolower( DRM_CHAR szChar )
{
    if ( szChar >= 'A' && szChar <= 'Z' )
    {
        return szChar - 'A' + 'a';
    }

    return szChar;
}

DRM_RESULT ParseGUID(DRM_GUID *pGuid, __in_ecount(DRM_GUID_STRING_LEN) DRM_BYTE *pString)
{
    DRM_RESULT dr      = DRM_SUCCESS;
    char tmp[39]       = {0};
    DRM_BYTE bTmp      = 0;
    int i              = 0;

    ChkArg( pString != NULL && pGuid != NULL && DRMCRT_strlen((char *)pString) == 38 );
    ChkArg( pString[0] == '{' && pString[37] == '}'); /* First and last char must be { & } */

    /* Validate the delimitar characters */

    if (pString[9]  != '-'
     || pString[14] != '-'
     || pString[19] != '-'
     || pString[24] != '-')
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    MEMSET(tmp, '\0', 39);
    MEMCPY(tmp, (char *)pString, 39);
    tmp[ 9] = '\0';
    tmp[14] = '\0';
    tmp[19] = '\0';
    tmp[24] = '\0';

    /* Convert pieces to BYTES */

    pGuid->Data1 = (DRM_DWORD) strtoul(&tmp[ 1], NULL, 16);

    pGuid->Data2 = (DRM_WORD)  strtoul(&tmp[10], NULL, 16);
    pGuid->Data3 = (DRM_WORD)  strtoul(&tmp[15], NULL, 16);
#if TARGET_LITTLE_ENDIAN == 0
    REVERSE_BYTES_DWORD(pGuid->Data1);
    REVERSE_BYTES_WORD(pGuid->Data2);
    REVERSE_BYTES_WORD(pGuid->Data3);
#endif

    bTmp = (DRM_BYTE) strtoul(&tmp[22], NULL, 16);
    PUT_BYTE((DRM_BYTE *) &pGuid->Data4, 1, bTmp);
    tmp[22] = '\0';
    bTmp = (DRM_BYTE) strtoul(&tmp[20], NULL, 16);
    PUT_BYTE((DRM_BYTE *) &pGuid->Data4, 0, bTmp);

    for (i = 5; i >= 0; i--)
    {
        tmp[25 + i * 2 + 2] = '\0';
        bTmp = (DRM_BYTE) strtoul(&tmp[25 + i * 2], NULL, 16);
        PUT_BYTE((DRM_BYTE *) &pGuid->Data4, i + 2, bTmp);
    }

ErrorExit:
    return dr;
}

DRM_RESULT ParseDRMID(DRM_ID *pTo, const char *pFrom)
{
    DRM_RESULT       dr = DRM_SUCCESS;
    const DRM_CHAR  *p  = (const DRM_CHAR *) pFrom;
    int              i  = 0;

    for(i = 0; i < 16; i++)
    {
        char c = (char)TST_UTL_tolower(*p++);
        char c2 = (char)TST_UTL_tolower(*p++);
        if(c == 0 || c2 == 0)
        {
            ChkDR( DRM_E_INVALIDARG );
        }
        PUT_BYTE((DRM_BYTE *)pTo, i, (DRM_BYTE)((((c >= '0' && c <= '9') ? (c - '0') : (c >= 'a' && c <= 'f' ? (10 + c - 'a') : 0)) << 4) +
           ((c2 >= '0' && c2 <= '9') ? (c2 - '0') : (c2 >= 'a' && c2 <= 'f' ? (10 + c2 - 'a') : 0))));
    }
ErrorExit:
    return dr;
}

DRM_RESULT Test_DrmRevocationTypeEnumGUID( DRM_REVOCATION_TYPE_ENUM eRevType, DRM_GUID **ppGuid )
{
    DRM_RESULT dr = DRM_SUCCESS;

    switch(eRevType)
    {
        case DRM_REVOCATION_TYPE_WMDRM_APP:
            *ppGuid = (DRM_GUID *)&GUID_WMDRMRevocationTypeApp;
            break;
        case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
            *ppGuid = (DRM_GUID *)&GUID_WMDRMRevocationTypeWmdrmpd;
            break;
        case DRM_REVOCATION_TYPE_WMDRM_ND:
            *ppGuid = (DRM_GUID *)&GUID_WMDRMRevocationTypeWmdrmnd;
            break;
        default:
            ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    return dr;
}

/*
** The files that Test_ReadRSAPrivateKeyFromFile2048 handles
** were created when the max key size was 2048bit and have
** the 2048bit-sized structure serialized to disk within them.
** Thus, we have to read those files into the 2048BIT structure.
*/

DRM_RESULT DRM_CALL Test_ReadRSAPrivateKeyFromFile2048( const char *pszFile, DRM_RSA_PRIVATE_KEY_2048BIT *pPrivateKey )
{
    DRM_RESULT   dr     = DRM_SUCCESS;
    DRM_BYTE    *pbTmp  = NULL;
    DRM_DWORD    cbTmp  = 0;
    DRM_DWORD    cbCopy = 0;

    ChkArg( pszFile != NULL );
    ChkArg( pPrivateKey != NULL );

    OEM_RSA_ZeroPrivateKey_2048BIT( pPrivateKey );

    ChkBOOL( LoadTestFile( NULL, pszFile, &pbTmp, &cbTmp), DRM_E_TEST_FILE_LOAD_ERROR );
    ChkMem( pbTmp );
    ChkArg( cbTmp >= SIZEOF(DRM_DWORD) );

#if !TARGET_LITTLE_ENDIAN
    /* Files on disk were stored using little-endian serialization.  Fix their endianness if needed. */
    {
        DRM_DWORD *pdwTmp = (DRM_DWORD*)pbTmp;
        DRM_DWORD  cdwTmp = SIZEOF(INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT) / SIZEOF(DRM_DWORD);
        DRM_DWORD  iTmp   = 0;
        while( iTmp < cdwTmp )
        {
            FIX_ENDIAN_DWORD( pdwTmp[iTmp] );
            iTmp++;
        }
    }
#endif  /* !TARGET_LITTLE_ENDIAN */

    /* Enum sizes are compiler specific.  The test files were saved where the size was SIZEOF(DRM_DWORD), so the first DRM_DWORD bytes are the enum. */
#if defined (__GNUC__)
    /* __GNUC__ does not support using DRM_OFFSET_OF like this in a compile time assert */
    DRMASSERT( DRM_OFFSET_OF( INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT, eKeyLength ) == 0 );
#else
    DRMCASSERT( DRM_OFFSET_OF( INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT, eKeyLength ) == 0 );
#endif
    ((INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT*)pPrivateKey)->eKeyLength = (DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT)(*((const DRM_DWORD*)pbTmp));

    /* Just copy the rest of the key. */
    cbCopy = MIN( SIZEOF(DRM_RSA_PRIVATE_KEY_2048BIT) - SIZEOF(DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT),
                  cbTmp                               - SIZEOF(DRM_DWORD) );
    MEMCPY( ((DRM_BYTE*)pPrivateKey) + SIZEOF(DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT),
            pbTmp                    + SIZEOF(DRM_DWORD),
            cbCopy );

ErrorExit:
    SAFE_OEM_FREE( pbTmp );
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

