/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <ndtrevguids.h>
#include <NdtDrmXmr.h>
#include <ndtnetmsg.h>
#include <functionaltest.h>
#include <ndtreg.h>
#include <tOEMImp.h>

ENTER_PKTEST_NAMESPACE_CODE

#define INITGUID

DRM_ID g_SerialNumber;
DRM_DWORD g_cbCertificate;
DRM_BYTE  *g_pCertificate = NULL;
DRM_RSA_PRIVATE_KEY  *g_pPrivateKey = NULL;
DRM_RSA_PRIVATE_KEY g_PrivateKey = {0};
DRM_CRYPTO_CONTEXT g_Crypto = { 0 };

XMR_ROOT_LICENSE_INFO g_XmrRootLicenseInfo;
DRM_BYTE *g_pbRootLicense = NULL;
DRM_DWORD g_cbRootLicense = 0;

DRM_BYTE *g_pbLeafLicense = NULL;
DRM_DWORD g_cbLeafLicense = 0;
XMR_LICENSE *g_pLeafXmrLicense;

DRM_DWORD g_cbRevocationListResponse = 0;
DRM_BYTE *g_pbRevocationListResponse = 0;
DRM_DWORD g_cbRevocationListRequest = 0;
DRM_BYTE *g_pbRevocationListRequest = 0;
DRM_DWORD g_dwRevocationList = 0;
DRM_RVK_LIST *g_pRevocationLists = NULL;
DRM_DWORD g_cRevocationLists = 0;

DRM_CHAR DRM_API TST_UTL_tolower( DRM_CHAR szChar )
{
    if ( szChar >= 'A' && szChar <= 'Z' )
    {
        return szChar - 'A' + 'a';
    }

    return szChar;
}

DRM_RESULT ParseGUID(DRM_GUID *pGuid, DRM_BYTE *pString)
{
    DRM_RESULT dr      = DRM_SUCCESS;
    char tmp[39]       = {0};
    DRM_DWORD bTmp     = 0;
    int i              = 0;

    ChkArg( pString != NULL && pGuid != NULL && strlen((char *)pString) == 38 );
    ChkArg( pString[0] == '{' && pString[37] == '}'); /* First and last char must be { & } */

    /* Validate the delimitar characters */

    if (pString[9]  != '-'
     || pString[14] != '-'
     || pString[19] != '-'
     || pString[24] != '-')
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    memset(tmp, '\0', 39);
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

static DRM_RESULT OpenFileIntoBuffer(IN const char* szFileName, OUT DRM_BYTE** ppBuffer, OUT DRM_DWORD* pcbBuffer)
{
    DRM_DWORD lSize;
    char* p;
    DRM_RESULT dr = DRM_SUCCESS;
    FILE *fp;

    fp = fopen(szFileName, "rb");
    if(NULL == fp)
    {
        TRACE(("Unable to open file %s.\n", szFileName));
        return DRM_E_FAIL;
    }

    if(fseek(fp, 0, SEEK_END) != 0)
    {
        TRACE(("Unable to seek to end of file %s.\n", szFileName));
        return DRM_E_FAIL;
    }

    lSize = ftell(fp);

    if(fseek(fp, 0, SEEK_SET) != 0)
    {
        TRACE(("Unable to seek to beginning of file %s.\n", szFileName));
        return DRM_E_FAIL;
    }

    *ppBuffer = (DRM_BYTE*) Oem_MemAlloc (lSize);
    if (*ppBuffer == NULL) return DRM_E_FAIL;

    p = (char*)*ppBuffer;

    *pcbBuffer = (DRM_DWORD)lSize;

    if(fread(p, 1, lSize, fp) != lSize)
    {
        TRACE(("Unable to read entire file %s\n", szFileName));
        Oem_MemFree(*ppBuffer);
        *ppBuffer = NULL;
        return DRM_E_FAIL;
    }
    /*** the whole file is loaded in the buffer. ***/
    /* terminate  */
    fclose(fp);

    return dr;
}

static DRM_DWORD ParseDRMID(DRM_ID *pTo, const char *pFrom)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *p = (DRM_BYTE *) pFrom;
    int i;

    for(i = 0; i < 16; i++)
    {
       char c = (char)TST_UTL_tolower(*p++);
       char c2 = (char)TST_UTL_tolower(*p++);
       PUT_BYTE((DRM_BYTE *)pTo, i, (((c >= '0' && c <= '9') ? (c - '0') : (c >= 'a' && c <= 'f' ? (10 + c - 'a') : 0)) << 4) +
           ((c2 >= '0' && c2 <= '9') ? (c2 - '0') : (c2 >= 'a' && c2 <= 'f' ? (10 + c2 - 'a') : 0)));
    }
    return dr;
}

DRM_RESULT DRM_CALL Test_SetSerialNumber(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    ParseDRMID(&g_SerialNumber, argv[0]);

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_SetCertificate(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);

    SAFE_OEM_FREE( g_pCertificate );
    ChkDR(OpenFileIntoBuffer(argv[0], &g_pCertificate, &g_cbCertificate));
    ChkMem(g_pCertificate);

 ErrorExit:
    return dr;
}

/**
 * Allows PrivateKey that gets passed to some functions to be set
 *
 * argv[0] = private key path
 */
DRM_RESULT DRM_CALL Test_SetPrivateKey(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cbPrivateKey = SIZEOF( DRM_RSA_PRIVATE_KEY );

    ChkDR( Oem_Device_GetKey( NULL,
                              DRM_DKT_PLAYREADY_NDR,
                              (DRM_BYTE*) &g_PrivateKey,
                              &cbPrivateKey ) );

    g_pPrivateKey = &g_PrivateKey;

 ErrorExit:

    return dr;
}

/**
 * Set private key directly, uses different RSA private key format and no hardcoded values
 *
 * argv[0] = path to file containing the private key dump
 */
DRM_RESULT DRM_CALL Test_SetPrivateKeyDirectly(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_BYTE   *pbTmp = NULL;
    DRM_DWORD   cbTmp = 0;

    ChkArg( argc == 1 && argv[0] );
    ChkDR( OpenFileIntoBuffer (argv[0], &pbTmp, &cbTmp ) );
    ChkMem( pbTmp) ;

    OEM_RSA_ZeroPrivateKey( &g_PrivateKey );

    MEMCPY( &g_PrivateKey, pbTmp, MIN( cbTmp, SIZEOF(DRM_RSA_PRIVATE_KEY) ) );

    g_pPrivateKey = &g_PrivateKey;

ErrorExit:
    SAFE_OEM_FREE( pbTmp );
    return dr;
}


DRM_RESULT DRM_CALL Test_WmdrmNetPackRegistrationMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pRegistrationRequestMsg = NULL;
    DRM_DWORD cbRegistrationRequestMsg = 0;
    DRM_ID idSerialNumber;
    DRM_BYTE *pbDeviceCertificate = NULL;
    DRM_DWORD cbDeviceCertificate = 0;

    ChkDR( WmdrmNetPackRegistrationMessage( &g_SerialNumber,
                                            g_pCertificate,
                                            g_cbCertificate,
                                            &pRegistrationRequestMsg,
                                            &cbRegistrationRequestMsg ) );

    ChkDR( WmdrmNetUnpackRegistrationRequestMessage
               ( pRegistrationRequestMsg,
                 cbRegistrationRequestMsg,
                 &idSerialNumber,
                 &pbDeviceCertificate,
                 &cbDeviceCertificate ) );
    if( MEMCMP(idSerialNumber.rgb, g_SerialNumber.rgb, DRM_ID_SIZE) != 0 )
    {
        TRACE(("%s: Serial number mismatch.\n", __FUNCTION__ ));
        ChkDR( DRM_E_FAIL );
    }
    if(cbDeviceCertificate != g_cbCertificate ||
       MEMCMP( pbDeviceCertificate, g_pCertificate, g_cbCertificate ) != 0 )
    {
        TRACE(("%s: Device certificate mismatch.\n", __FUNCTION__ ));
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
/* uncomment when using with malloc()ing unpackoctets */
/*     SAFE_OEM_FREE( pbDeviceCertificate );*/
    SAFE_OEM_FREE( pRegistrationRequestMsg );
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetUnpackRegistrationResponseMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr             = DRM_SUCCESS;
    DRM_BYTE   *pbResponse     = NULL;
    DRM_DWORD   cbResponse     = 0;
    DRM_BYTE   *pbAddress      = NULL;
    DRM_WORD    cbAddress      = 0;
    DRM_ID      idSeed         = { 0 };
    DRM_ID      idSerialNumber = { 0 };
    DRM_ID      idSession      = { 0 };

    DRM_CRYPTO_CONTEXT *pCryptoCtx = NULL;

    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    ChkArg( argc == 1 );
    ChkDR( OpenFileIntoBuffer( argv[0], &pbResponse, &cbResponse ) );
    ChkDR( WmdrmNetUnpackRegistrationResponseMessage
               ( g_pPrivateKey,
                 pbResponse,
                 cbResponse,
                 &idSeed,
                 &idSerialNumber,
                 &idSession,
                 &pbAddress,
                 &cbAddress,
                 pCryptoCtx) );
    if( MEMCMP( idSerialNumber.rgb, g_SerialNumber.rgb, DRM_ID_SIZE ) != 0 )
    {
        TRACE(("Serial number mismatch in %s\n", argv[0]));
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:
    SAFE_OEM_FREE( pbResponse );
    SAFE_OEM_FREE( pCryptoCtx );
    /* SAFE_OEM_FREE(pbAddress); */
    return dr;
}


DRM_RESULT DRM_CALL Test_WmdrmNetPackProximityStartMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID idSession;
    DRM_ID idSessionOut;
    DRM_DWORD cbMessage = 0;
    DRM_BYTE *pbMessage = NULL;

    Oem_Random_GetBytes( NULL, idSession.rgb, DRM_ID_SIZE );

    ChkDR( WmdrmNetPackProximityStartMessage( &idSession, &pbMessage, &cbMessage ) );
    ChkDR( WmdrmNetUnpackProximityStartMessage( pbMessage, cbMessage, &idSessionOut ) );
    if( MEMCMP( idSessionOut.rgb, idSession.rgb, DRM_ID_SIZE ) != 0)
    {
        ChkDR( DRM_E_FAIL );
    }
ErrorExit:
    SAFE_OEM_FREE(pbMessage);
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetUnpackProximityChallengeMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID idSession;
    DRM_ID idSessionOut;
    DRM_ID idChallenge;
    DRM_ID idChallengeOut;
    DRM_DWORD cbMessage = 0;
    DRM_BYTE *pbMessage = NULL;
    DRM_BYTE bSequence = 0;
    DRM_BYTE bSequenceOut = 0;

    Oem_Random_GetBytes( NULL, idSession.rgb, DRM_ID_SIZE );
    Oem_Random_GetBytes( NULL, idChallenge.rgb, DRM_ID_SIZE );
    Oem_Random_GetBytes( NULL, &bSequence, SIZEOF( bSequence ) );

    ChkDR( WmdrmNetPackProximityChallengeMessage( bSequence, &idSession, &idChallenge, &pbMessage, &cbMessage ) );
    ChkDR( WmdrmNetUnpackProximityChallengeMessage( pbMessage, cbMessage, &bSequenceOut, &idSessionOut, &idChallengeOut ) );
    ChkBOOL( bSequenceOut == bSequence &&
             MEMCMP(idSessionOut.rgb, idSession.rgb, DRM_ID_SIZE) == 0 &&
             MEMCMP(idChallengeOut.rgb, idChallenge.rgb, DRM_ID_SIZE) == 0,
             DRM_E_FAIL );
ErrorExit:
    SAFE_OEM_FREE( pbMessage );
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetPackProximityResponseMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID idSession = {0};
    DRM_ID idSessionOut = {0};
    DRM_ID idChallenge = {0};
    DRM_ID idOriginalChallenge = {0};
    DRM_ID idNonce = {0};
    DRM_ID idSeed = {0};
    DRM_DWORD cbMessage = 0;
    DRM_BYTE *pbMessage = NULL;
    DRM_BYTE bSequence = 0;
    DRM_BYTE bSequenceOut = 0;
    DRM_AES_KEY *pKey = NULL;

    Oem_Random_GetBytes( NULL, idSession.rgb, DRM_ID_SIZE );
    Oem_Random_GetBytes( NULL, idChallenge.rgb, DRM_ID_SIZE );
    idOriginalChallenge = idChallenge;
    Oem_Random_GetBytes( NULL, idSeed.rgb, DRM_ID_SIZE );
    Oem_Random_GetBytes( NULL, &bSequence, SIZEOF( bSequence ) );

    pKey = WmdrmNetGetRegistrationKey( &idSeed, FALSE );

    ChkDR( WmdrmNetPackProximityResponseMessage( pKey, bSequence, &idSession, &idChallenge, &pbMessage, &cbMessage ) );
    ChkDR( WmdrmNetUnpackProximityResponseMessage( pbMessage, cbMessage, &bSequenceOut, &idSessionOut, &idNonce ) );
    ChkBOOL( bSequenceOut == bSequence &&
             MEMCMP( idSessionOut.rgb, idSession.rgb, DRM_ID_SIZE) == 0,
             DRM_E_FAIL );

    ChkDR( Oem_Aes_EcbDecryptData( pKey, (DRM_BYTE *)&idNonce, DRM_ID_SIZE ) );
    ChkBOOL( MEMCMP( idOriginalChallenge.rgb, idNonce.rgb, DRM_ID_SIZE ) == 0,
             DRM_E_FAIL );
ErrorExit:
    if(pKey)
    {
        Oem_Aes_ZeroKey( pKey );
    }
    SAFE_OEM_FREE( pbMessage );
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetUnpackProximityResultMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID idSession;
    DRM_ID idSessionOut;
    DRM_DWORD cbMessage = 0;
    DRM_BYTE *pbMessage = NULL;
    DRM_WORD wResult = 0;
    DRM_WORD wResultOut = 0;

    Oem_Random_GetBytes( NULL, idSession.rgb, DRM_ID_SIZE );
    Oem_Random_GetBytes( NULL, (DRM_BYTE *)&wResult, SIZEOF( wResult ) );

    ChkDR( WmdrmNetPackProximityResultMessage( &idSession, wResult, &pbMessage, &cbMessage ) );
    ChkDR( WmdrmNetUnpackProximityResultMessage( pbMessage, cbMessage, &idSessionOut, &wResultOut ) );
    ChkBOOL( MEMCMP( idSessionOut.rgb, idSession.rgb, DRM_ID_SIZE) == 0 &&
             wResult == wResultOut,
             DRM_E_FAIL );
ErrorExit:
     SAFE_OEM_FREE( pbMessage );
     return dr;
}

DRM_RESULT DRM_CALL Test_DrmXmrUnpackRootLicense(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );

    SAFE_OEM_FREE( g_pbRootLicense );
    /* Free root license? */
    ChkDR( OpenFileIntoBuffer( argv[0], &g_pbRootLicense, &g_cbRootLicense ) );

    ChkDR( DrmXmrUnpackRootLicense( g_pPrivateKey,
                                    g_pbRootLicense,
                                    g_cbRootLicense,
                                    &g_XmrRootLicenseInfo ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DrmXmrUnpackLeafLicense(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );

    DrmXmrFreeLicense (g_pLeafXmrLicense );
    SAFE_OEM_FREE( g_pbLeafLicense );

    ChkDR( OpenFileIntoBuffer( argv[0], &g_pbLeafLicense, &g_cbLeafLicense ) );

    ChkDR( DrmXmrUnpackLeafLicense( g_XmrRootLicenseInfo,
                                    g_pbLeafLicense,
                                    g_cbLeafLicense,
                                    &g_pLeafXmrLicense ) );

ErrorExit:
    return dr;
}

static DRM_RESULT _DrmRevocationTypeEnumGUID( DRM_REVOCATION_TYPE_ENUM eRevType, DRM_GUID **ppGuid )
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

DRM_RESULT DRM_CALL Test_AddRevocationListToResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbRevocationList = NULL;
    DRM_DWORD cbRevocationList;
    DRM_CONST_STRING dstrSource;
    DRM_REVOCATION_TYPE_ENUM eRevType;
    DRM_GUID guid;
    DRM_GUID *pGuid = &guid;
    DRM_DWORD cbDecoded = 0;
    DRM_BOOL fDecode = TRUE;

    ChkArg( g_pRevocationLists != NULL && argc == 2 && g_dwRevocationList < g_cRevocationLists );

    if(DRM_FAILED(ParseGUID(pGuid, (DRM_BYTE *)argv[0])))
    {
        eRevType = (DRM_REVOCATION_TYPE_ENUM) strtoul( argv[0], NULL, 0 );
        ChkDR( _DrmRevocationTypeEnumGUID( eRevType, &pGuid ) );
    }
    if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeRiv, pGuid ) )
    {
        fDecode = FALSE;
    }

    ChkDR( OpenFileIntoBuffer( argv[1], &pbRevocationList, &cbRevocationList ) );

    if(fDecode)
    {
        dstrSource.pwszString = (DRM_WCHAR *)pbRevocationList;
        dstrSource.cchString = cbRevocationList / 2;
        dr = DRM_B64_DecodeW( &dstrSource, &cbDecoded, NULL, DRM_BASE64_DECODE_IN_PLACE );
        ChkDR( dr );

        g_pRevocationLists[g_dwRevocationList].pbRevocationList = pbRevocationList;
        g_pRevocationLists[g_dwRevocationList].cbRevocationList = cbDecoded;
    }
    else
    {
        g_pRevocationLists[g_dwRevocationList].pbRevocationList = pbRevocationList;
        g_pRevocationLists[g_dwRevocationList].cbRevocationList = cbRevocationList;
    }

    MEMCPY( &g_pRevocationLists[g_dwRevocationList].guidRevocationList, pGuid, DRM_ID_SIZE );
    g_dwRevocationList++;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_InitializeRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1);

    g_cRevocationLists = strtoul( argv[0], NULL, 0 );
    g_cbRevocationListResponse = 0;
    SAFE_OEM_FREE(g_pbRevocationListResponse);
    SAFE_OEM_FREE(g_pRevocationLists);
    g_pRevocationLists = (DRM_RVK_LIST *)Oem_MemAlloc(g_cRevocationLists * SIZEOF(DRM_RVK_LIST ) );
    ChkMem( g_pRevocationLists );
    g_dwRevocationList = 0;

ErrorExit:
    return dr;
}

static DRM_RESULT _DrmRevocationTypeEnumFromGUID( DRM_GUID *pGuid, DRM_REVOCATION_TYPE_ENUM *peRevType )
{
    DRM_RESULT dr = DRM_SUCCESS;
    if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeApp, pGuid ) )
    {
        *peRevType = DRM_REVOCATION_TYPE_WMDRM_APP;
    }
    else if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeWmdrmpd, pGuid ) )
    {
        *peRevType = DRM_REVOCATION_TYPE_WMDRM_DEVICE;
    }
    else if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeWmdrmnd, pGuid ) )
    {
        *peRevType = DRM_REVOCATION_TYPE_WMDRM_ND;
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListResponseMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD dr = DRM_SUCCESS;

    dr = WmdrmNetPackRevocationListResponseMessage(g_cRevocationLists, g_pRevocationLists, g_pbRevocationListResponse, &g_cbRevocationListResponse );
    if(dr == DRM_E_BUFFERTOOSMALL)
    {
        SAFE_OEM_FREE(g_pbRevocationListResponse);
        ChkMem( g_pbRevocationListResponse = (DRM_BYTE *)Oem_MemAlloc( g_cbRevocationListResponse ) );
        dr = WmdrmNetPackRevocationListResponseMessage(g_cRevocationLists, g_pRevocationLists, g_pbRevocationListResponse, &g_cbRevocationListResponse );
    }
    ChkDR( dr );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListRequestMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD dr = DRM_SUCCESS;
    DRM_DWORD cguidsRevocationLists = 0;
    DRM_DWORD i;
    DRM_GUID *pguidsRevocationLists = NULL;

    cguidsRevocationLists = argc;
    if(cguidsRevocationLists)
    {
        ChkMem( pguidsRevocationLists = (DRM_GUID *)Oem_MemAlloc( DRM_ID_SIZE * cguidsRevocationLists ) );
        for(i = 0; i < cguidsRevocationLists; i++)
        {
            if(DRM_FAILED(ParseGUID( &pguidsRevocationLists[i], (DRM_BYTE *)argv[i] )))
            {
                TRACE(("unable to parse GUID argument %d\n", i + 1 ));
                ChkDR( DRM_E_INVALIDARG );
            }
        }
    }

    g_cbRevocationListRequest = 0;
    dr = WmdrmNetPackRevocationListRequestMessage(
             cguidsRevocationLists,
             pguidsRevocationLists,
             NULL,
             &g_cbRevocationListRequest );
    if(dr == DRM_E_BUFFERTOOSMALL)
    {
        SAFE_OEM_FREE( g_pbRevocationListRequest );
        g_pbRevocationListRequest = (DRM_BYTE *)Oem_MemAlloc( g_cbRevocationListRequest );
        ChkMem( g_pbRevocationListRequest );
        dr = WmdrmNetPackRevocationListRequestMessage(
                 cguidsRevocationLists,
                 pguidsRevocationLists,
                 g_pbRevocationListRequest,
                 &g_cbRevocationListRequest );
    }
    ChkDR( dr );

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL Test_DumpRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;
    DRM_DWORD cRevocationLists = 0;
    DRM_RVK_LIST *pRevocationLists = NULL;
    DRM_REVOCATION_TYPE_ENUM eRevType;

    dr = WmdrmNetUnpackRevocationListResponseMessage(
             g_pbRevocationListResponse,
             g_cbRevocationListResponse,
             &cRevocationLists,
             NULL );
    if(dr == DRM_E_BUFFERTOOSMALL)
    {
        ChkMem( pRevocationLists = (DRM_RVK_LIST *)Oem_MemAlloc( cRevocationLists * SIZEOF( DRM_RVK_LIST ) ) );
        dr = WmdrmNetUnpackRevocationListResponseMessage(
                 g_pbRevocationListResponse,
                 g_cbRevocationListResponse,
                 &cRevocationLists,
                 pRevocationLists );
    }
    ChkDR( dr );

    for(i = 0; i < cRevocationLists; i++)
    {
        if( DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeRiv, &pRevocationLists[i].guidRevocationList ) )
        {
            TRACE(("RIV"));
        }
        else
        {
            ChkDR( _DrmRevocationTypeEnumFromGUID (
                       &pRevocationLists[i].guidRevocationList,
                       &eRevType ) );
            switch(eRevType)
            {
            case DRM_REVOCATION_TYPE_WMDRM_APP:
                TRACE(("App revocation list"));
                break;
            case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
                TRACE(("Device revocation list"));
                break;
            case DRM_REVOCATION_TYPE_WMDRM_ND:
                TRACE(("WMDRMND revocation list"));
                break;
            default:
                TRACE(("Unknown enumeration value: %d", eRevType));
            }
        }
        TRACE((": %lu bytes\n", pRevocationLists[i].cbRevocationList));
    }
ErrorExit:
    SAFE_OEM_FREE( pRevocationLists );

    return dr;
}

DRM_RESULT DRM_CALL Test_VerifyRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD dr = DRM_SUCCESS;
    DRM_DWORD i;
    DRM_DWORD j;
    DRM_DWORD cRevocationLists = 0;
    DRM_RVK_LIST *pRevocationLists = NULL;
    DRM_REVOCATION_TYPE_ENUM eRevType;
    DRM_DWORD dwMatched = 0;

    dr = WmdrmNetUnpackRevocationListResponseMessage(
             g_pbRevocationListResponse,
             g_cbRevocationListResponse,
             &cRevocationLists,
             NULL );
    if(dr == DRM_E_BUFFERTOOSMALL)
    {
        ChkMem( pRevocationLists = (DRM_RVK_LIST *)Oem_MemAlloc( cRevocationLists * SIZEOF( DRM_RVK_LIST ) ) );
        dr = WmdrmNetUnpackRevocationListResponseMessage(
                 g_pbRevocationListResponse,
                 g_cbRevocationListResponse,
                 &cRevocationLists,
                 pRevocationLists );
    }
    ChkDR( dr );

    if(g_cRevocationLists)
    {
        if(g_cRevocationLists != cRevocationLists)
        {
            TRACE(("Differing number of revocation lists.\n"));
            ChkDR( DRM_E_FAIL );
        }
    }
    for(i = 0; i < cRevocationLists; i++)
    {
        if( DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeRiv, &pRevocationLists[i].guidRevocationList ) )
        {
            TRACE(("RIV"));
        }
        else
        {
            ChkDR( _DrmRevocationTypeEnumFromGUID (
                       &pRevocationLists[i].guidRevocationList,
                       &eRevType ) );
            switch(eRevType)
            {
            case DRM_REVOCATION_TYPE_WMDRM_APP:
                TRACE(("App revocation list"));
                break;
            case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
                TRACE(("Device revocation list"));
                break;
            case DRM_REVOCATION_TYPE_WMDRM_ND:
                TRACE(("WMDRMND revocation list"));
                break;
            default:
                TRACE(("Unknown enumeration value: %d", eRevType));
            }
        }
        TRACE((": %lu bytes\n", pRevocationLists[i].cbRevocationList));

        for(j = 0; j < g_cRevocationLists; j++)
        {
            if(MEMCMP( &pRevocationLists[i].guidRevocationList, &g_pRevocationLists[j].guidRevocationList, DRM_ID_SIZE ) == 0)
            {
                if(pRevocationLists[i].cbRevocationList != g_pRevocationLists[j].cbRevocationList)
                {
                    TRACE(("Differing number of bytes for revocation list %d (%lu != %lu)\n", i + 1, pRevocationLists[i].cbRevocationList, g_pRevocationLists[j].cbRevocationList));
                    ChkDR( DRM_E_FAIL );
                }
                if(MEMCMP( pRevocationLists[i].pbRevocationList, g_pRevocationLists[j].pbRevocationList, pRevocationLists[i].cbRevocationList ) != 0 )
                {
                    TRACE(("Different byte content for revocation list %d\n", i + 1));
                    ChkDR( DRM_E_FAIL);
                }
                dwMatched++;
            }
        }

    }
    if(dwMatched != g_cRevocationLists )
    {
        TRACE(("Not all revocation lists matched with partners.\n"));
        ChkDR( DRM_E_FAIL );
    }
    if(dwMatched)
    {
        TRACE(("Revocation list response matched with stored.\n"));
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetUnpackRevocationListRequestMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_GUID *guidsRevocationLists = NULL;
    DRM_DWORD cguidsRevocationLists = 0;
    DRM_REVOCATION_TYPE_ENUM eRevType;
    DRM_DWORD i;

    if(DRM_FAILED( dr = WmdrmNetUnpackRevocationListRequestMessage
               ( g_pbRevocationListRequest,
                 g_cbRevocationListRequest,
                 &cguidsRevocationLists,
                 &guidsRevocationLists ) ) )
    {
        /* Either DRM_E_ND_INVALID_MESSAGE, DRM_E_ND_INVALID_MESSAGE_VERSION, or
           DRM_E_ND_INVALID_MESSAGE_TYPE. */
        TRACE(("Invalid Revocation List Response Message: 0x%lx\n", dr));
        ChkDR( DRM_E_FAIL );
    }

    for(i = 0; i < cguidsRevocationLists; i++)
    {
        /* RIV is a special case */
        if( DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeRiv,
                             &guidsRevocationLists[i] ) )
        {
            TRACE(("RIV\n"));
            continue;
        }
        ChkDR( _DrmRevocationTypeEnumFromGUID( &guidsRevocationLists[i],
                                               &eRevType ) );
        switch(eRevType)
        {
        case DRM_REVOCATION_TYPE_WMDRM_APP:
            TRACE(("App revocation list\n"));
            break;
        case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
            TRACE(("Device revocation list\n"));
            break;
        case DRM_REVOCATION_TYPE_WMDRM_ND:
            TRACE(("WMDRMND revocation list\n"));
            break;
        default:
            TRACE(("Unknown enumeration value: %d\n", eRevType));
            ChkDR(DRM_E_FAIL);
        }
    }

ErrorExit:
    SAFE_OEM_FREE(guidsRevocationLists);
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(NDRFunctionalTest)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( SetNDREnv( NDR_MODEL_CERT, NDR_MODEL_PRIVATE_KEY, TRUE ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(NDRFunctionalTest)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

static DRM_RESULT DRM_CALL WrapTestRun(long lTCID, DRM_BOOL fPreTestRun)
{
    return DRM_SUCCESS;
}

BEGIN_TEST_API_LIST(NDRFunctionalTest)
    API_ENTRY(Test_SetSerialNumber)
    API_ENTRY(Test_SetCertificate)
    API_ENTRY(Test_SetPrivateKey)
    API_ENTRY(Test_SetPrivateKeyDirectly)
    API_ENTRY(Test_WmdrmNetPackRegistrationMessage)
    API_ENTRY(Test_WmdrmNetUnpackRegistrationResponseMessage)
    API_ENTRY(Test_WmdrmNetPackProximityStartMessage)
    API_ENTRY(Test_WmdrmNetUnpackProximityChallengeMessage)
    API_ENTRY(Test_WmdrmNetPackProximityResponseMessage)
    API_ENTRY(Test_WmdrmNetUnpackProximityResultMessage)
    API_ENTRY(Test_DrmXmrUnpackRootLicense)
    API_ENTRY(Test_DrmXmrUnpackLeafLicense)
    API_ENTRY(Test_AddRevocationListToResponse)
    API_ENTRY(Test_InitializeRevocationListResponse)
    API_ENTRY(Test_WmdrmNetPackRevocationListResponseMessage)
    API_ENTRY(Test_WmdrmNetPackRevocationListRequestMessage)
    API_ENTRY(Test_VerifyRevocationListResponse)
    API_ENTRY(Test_DumpRevocationListResponse)
    API_ENTRY(Test_WmdrmNetUnpackRevocationListRequestMessage)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE
