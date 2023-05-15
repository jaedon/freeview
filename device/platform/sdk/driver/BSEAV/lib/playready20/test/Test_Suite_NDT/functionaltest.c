/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifdef MEMORY_LEAK_DEBUG
#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdafx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <tclib.h>
#include <drmcommon.h>
#include <drmxmlparser.h>
#include <drmdevstore.h>
#include <drmndtmanager.h>
#include <ndtmanager_internal.h>
//#include <wmdrmndresults.h>
#include <oemndt.h>
#include <ndtconstants.h>
#include <drmndtasf.h>
#include <drmcrt.h>
#include <ndtencrypt.h>
#include <ndtasfguids.h>
#include <ndtrevguids.h>
#include <ndtasf_internal.h>
#include <functionaltest.h>
#include <testdevstore.h>
#include <drmxmrbuilder.h>
#include <NdtDrmXmr.h>
#include <drmutilities.h>
#include <oemaes.h>
#include <drmdecrypt.h>
#include <drmptrlist.h>
#include <drmorderedlist.h>
#include <ndtlicense.h>
#include <tOEMImp.h>
#include <tstutils.h>
#include <ndtnetmsg.h>
#include <ndtreg.h>
#include <drmtoolsinit.h>

ENTER_PK_NAMESPACE_CODE;

extern DRM_API DRM_RESULT DRM_CALL _DRM_XMR_RootSignatureCallback(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount(f_cbData)               const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer
);

extern DRM_API DRM_RESULT DRM_CALL _ASFParsePayloadHeader(
    __in                           OEM_FILEHDL              hInFile,
    __out_bcount(FILE_BUFFER_SIZE) DRM_BYTE                *pbFileBuffer,
    __inout                        ASF_PARSER_STATE * const pState
);

extern DRM_API DRM_RESULT DRM_CALL _ASFParsePacketHeader(
    __in                           OEM_FILEHDL              hInFile,
    __out_bcount(FILE_BUFFER_SIZE) DRM_BYTE                *pbFileBuffer,
    __inout                        ASF_PARSER_STATE * const pState
);

EXIT_PK_NAMESPACE_CODE;

ENTER_PKTEST_NAMESPACE_CODE

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

#ifndef FILE_BUFFER_SIZE
#define FILE_BUFFER_SIZE  65536
#endif

#define MAXIMUM_HEADER_SIZE 8192

#define XMR_AES_SIGNATURE_LENGTH    16

const DRM_BYTE g_rgbRSAPrimes[2][__CB_DECL( DRM_RSA_CB_PRIME( eDRM_RSA_1024 ) )] =
{
    {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x63, 0x35, 0xA4, 0x9E,
        0xF7, 0x4B, 0xA4, 0xF9,
        0xE7, 0x6B, 0xAA, 0x0D,
        0xA0, 0x93, 0x25, 0x7B,
        0x36, 0xDC, 0x48, 0x48,
        0x95, 0x31, 0xA8, 0x18,
        0x84, 0xF4, 0x8E, 0x59,
        0x11, 0x1C, 0x6E, 0x5B,
        0x99, 0xB0, 0xA0, 0x75,
        0xA7, 0x91, 0x7B, 0xEE,
        0x1F, 0x4A, 0x27, 0x3D,
        0xFC, 0x74, 0x71, 0xF7,
        0x93, 0xBF, 0xB8, 0xAA,
        0x55, 0x17, 0x0E, 0x91,
        0x99, 0x83, 0xBF, 0x39,
        0x2B, 0xF4, 0xA6, 0x49
    },
    {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01,
        0x87, 0x1A, 0x66, 0x4A,
        0xC4, 0x90, 0x6E, 0xC0,
        0x24, 0xF4, 0x01, 0x30,
        0x22, 0x77, 0xD8, 0xE7,
        0xBC, 0xAA, 0x0B, 0x1D,
        0x1A, 0x0D, 0xA5, 0x5C,
        0x92, 0x53, 0x7D, 0x37,
        0xF1, 0x8E, 0xF5, 0x8A,
        0x00, 0x59, 0x1A, 0x1D,
        0xEC, 0xF5, 0x77, 0x63,
        0x69, 0xC5, 0x1C, 0x21,
        0x56, 0x90, 0x9D, 0x3F,
        0xA8, 0x6E, 0xED, 0xBE,
        0xAD, 0x60, 0x75, 0x62,
        0xC7, 0x50, 0x39, 0xC1,
        0xA2, 0xFF, 0x28, 0x2B
    }
};

const DRM_BYTE g_rgbRSAPrivExp[__CB_DECL( DRM_RSA_CB_PRIVEXP( eDRM_RSA_1024 ) )] =
{
    0x0E, 0xA9, 0x00, 0xCE,
    0x2C, 0xE4, 0x39, 0x07,
    0xE3, 0x87, 0xED, 0x98,
    0xDD, 0x5C, 0x93, 0xFA,
    0x16, 0x59, 0x68, 0xB0,
    0x82, 0x10, 0x2F, 0xC6,
    0x1A, 0x4B, 0xF2, 0xB2,
    0xE4, 0x5C, 0x53, 0x86,
    0xE4, 0x0C, 0xE9, 0x49,
    0xFC, 0x18, 0x94, 0xC2,
    0xD3, 0x42, 0xBB, 0xB6,
    0xD0, 0xAC, 0x5A, 0xB1,
    0xFE, 0xAF, 0x74, 0xCA,
    0x41, 0xDE, 0x9C, 0xC8,
    0xA5, 0xBD, 0xC2, 0x39,
    0xA9, 0x8A, 0xA3, 0x68,
    0x31, 0xFD, 0x5B, 0x2F,
    0xE8, 0x48, 0x19, 0x38,
    0x3B, 0xDC, 0x7F, 0xC8,
    0x2A, 0x6C, 0x84, 0x9C,
    0x62, 0x6B, 0xD6, 0x72,
    0x27, 0xBF, 0x33, 0x7E,
    0xA0, 0x1C, 0x4C, 0xFC,
    0x67, 0x61, 0xED, 0x1C,
    0x48, 0x5D, 0xC2, 0xEA,
    0x03, 0x9C, 0x2D, 0x6C,
    0xEE, 0xB3, 0xE6, 0x1D,
    0xFA, 0x8F, 0x90, 0x43,
    0x62, 0x37, 0x25, 0x1B,
    0xF0, 0x51, 0x96, 0x4D,
    0xE8, 0x06, 0x8A, 0xC9,
    0x6F, 0xB3, 0xF3, 0x71
};

#define STRINGIFY(x) #x

/*
** The below is an array of (starting point = uninitialized)
** NDTMGR_CONTEXT + metadata structures. The pointer identifies the current
** one selected by the script.
*/
ND_TRANSMITTER_CONTEXT g_NdtContexts[MAXNDTCONTEXTS];
ND_TRANSMITTER_CONTEXT *g_pCurNdtContext = NULL;

/*
** This is normally the App Context for g_pCurNdtContext. However, it may get
** overridden to NULL in order to test API parameter validation.
*/
NDTMGR_CONTEXT *g_pNdtDrmContext = NULL;

char g_cCertificateFilename[MAX_PATH];
DRM_DWORD g_cbCertificate;
DRM_BYTE  *g_pCertificate = NULL;

DRM_RSA_PRIVATE_KEY g_PrivateKey = {0};
DRM_AES_KEY  *g_pRootContentEncryptionKey = NULL;
DRM_AES_KEY  *g_pContentEncryptionKey = NULL;
DRM_CRYPTO_CONTEXT g_Crypto = { 0 };

DRM_WORD g_cbAction;
DRM_WORD g_cbActionOverride;

DRM_ID g_RegistrationSessionID;

OEM_FILEHDL               g_hOutputFiles[MAXMEDIASESSIONS];
DRM_ID                    g_KIDs[MAXMEDIASESSIONS];
XMR_LICENSE              *g_pXmrLicenses[MAXMEDIASESSIONS];
XMR_ROOT_LICENSE_INFO     g_RootXmrLicenseInfos[MAXMEDIASESSIONS];
DRM_AES_KEY              *g_pContentEncryptionKeys[MAXMEDIASESSIONS];
DRM_AES_KEY              *g_pRootContentEncryptionKeys[MAXMEDIASESSIONS];
XMR_GLOBAL_REQUIREMENTS   g_xmrMinEnvironments[MAXMEDIASESSIONS];
DRM_ID                    g_SerialNumbers[MAXMEDIASESSIONS];
DRM_ID                    g_MediaSessionIDs[MAXMEDIASESSIONS];
DRM_ID                    g_MediaSessionID;
DRM_WORD                  g_CurrentMediaSession;

DRM_ID g_SerialNumber;
DRM_ID g_drmRightsID;
DRM_ID g_Seed;

DRM_DWORD g_cbRegistrationResponseMsgOverride = 0;
DRM_DWORD g_cbRegistrationResponseMsg = 1024;
DRM_DWORD g_pbRegistrationResponseMsgNULL = 0;

DRM_DWORD g_cbRegistrationRequestMsgOverride = 0;
DRM_DWORD g_cbRegistrationRequestMsg;
DRM_DWORD g_pbRegistrationRequestMsgNULL = 0;

DRM_BOOL  g_fProximityStartMsgNULL = 0;
DRM_BOOL  g_fProximityResponseMsgNULL = 0;

DRM_DWORD g_cbLicenseChallengeMsgOverride = 0;
DRM_BYTEBLOB g_LicenseChallengeMsg = {NULL, 0};

DRM_DWORD g_cbLicenseResponseMsgOverride = -1;
DRM_DWORD g_cbLicenseResponseMsgNULL = 0;
DRM_DWORD g_pbLicenseResponseMsgNULL = 0;

DRM_ID    g_idChallenge;
DRM_ID    g_idChallengeOverride;
DRM_DWORD g_ChallengeIDOverride = 0;
DRM_BYTE  g_bChallengeSequenceNumber = 0;
DRM_BOOL  g_fTestProximityBUFFERTOOSMALL = FALSE;

DRM_BOOL  g_fMediaSessionIDNULL = FALSE;
DRM_BOOL  g_fASFFlagsNULL = FALSE;
DRM_DWORD g_dwASFFlags = 0;

DRM_BOOL  g_fpbDataNULL = FALSE;
DRM_BOOL  g_fpcbDataNULL = FALSE;

DRM_BOOL  g_fpbSampleDataNULL = FALSE;
DRM_BOOL  g_fpcbSampleDataNULL = FALSE;
DRM_DWORD g_cbSampleDataOverride = -1;
NDT_SAMPLEINFO g_SampleInfo;
DRM_BOOL  g_fpSampleInfoNULL = FALSE;

DRM_BOOL  g_fpbHeaderNULL = FALSE;
DRM_BOOL  g_fpcbHeaderNULL = FALSE;
DRM_DWORD g_cbHeaderOverride = -1;

DRM_BOOL  g_fpbLeafLicenseResponseNULL = FALSE;
DRM_BOOL  g_fpcbLeafLicenseResponseNULL = FALSE;
DRM_DWORD g_cbLeafLicenseResponseOverride = -1;

WMDRMNET_POLICY *g_pWMDRMNETPolicy = NULL;

DRM_BOOL  g_fpKIDNULL = FALSE;

DRM_DWORD g_dwControlCapabilities;
DRM_DWORD g_dwDRMStatus;
DRM_DWORD g_dwLicenseStatus;
DRM_DWORD g_dwDRMDeliveryOptions;

DRM_BYTE g_MessageTypeOverride;
DRM_BYTE g_MessageProtocolVersionOverride;

DRM_DWORD g_LastDataBlock;
DRM_BOOL  g_fEndOfData;

OEM_FILEHDL g_hOutputFile = OEM_INVALID_HANDLE_VALUE;

OEM_FILEHDL              g_hClearFile = OEM_INVALID_HANDLE_VALUE;
DRM_UINT64               g_qwFirstPacketOffset;
ASF_PARSER_STATE         g_ClearFileState;
ASF_PACKET_PARSER_INFO   g_ClearFilePacketInfo;
ASF_PAYLOAD_PARSER_INFO  g_ClearFilePayloadInfo;
PTR_LIST                 g_aryPtrListClearFileSamples[MAX_STREAMS_IN_ASF + 1];
TEMP_ASF_SAMPLE          g_FilePayloadFragments[MAX_STREAMS_IN_ASF + 1];

XMR_GLOBAL_REQUIREMENTS g_xmrMinEnvironment;

DRM_UINT64 g_qwTimeOffsetHNS;

XMR_ROOT_LICENSE_INFO g_RootXmrLicenseInfo = {NULL, NULL, NULL};
XMR_LICENSE *g_pXmrLicense = NULL;

DRM_DWORD g_CryptoDataNULL = 0;
DRM_DWORD g_EncryptionMode = 0;

DRM_ID g_KID;
NDT_ASFDELIVERY_MODE   g_eASFDeliveryMode;

DRM_BYTE  g_rgbProximityDetectionAddress[256] = { 'M', 'T', 'P', '\0' };
DRM_DWORD g_cbProximityDetectionAddress       = __CB_DECL(4);
DRM_BOOL  g_fAllowNewDeviceRegistration       = TRUE;

DRM_DWORD g_cbRevocationListResponse = 0;
DRM_BYTE *g_pbRevocationListResponse = 0;
DRM_DWORD g_cbRevocationListRequest = 0;
DRM_BYTE *g_pbRevocationListRequest = 0;
DRM_DWORD *g_pcbRevocationListRequest = &g_cbRevocationListRequest;
DRM_DWORD *g_pcbRevocationListResponse = &g_cbRevocationListResponse;
DRM_DWORD g_dwRevocationList = 0;
DRM_RVK_LIST *g_pRevocationLists = NULL;
DRM_DWORD g_cRevocationLists = 0;
DRM_BYTE *g_pbSharedRevBuffer = NULL;

DRM_DWORD g_NoHandleBufferTooSmall = 0;

DRM_BOOL g_fFailAllocation = FALSE;

DRM_DWORD g_FirstNextDataBlock;
DRM_DWORD g_nDataPackets;
DRM_DWORD g_dwDataPacketSize;

static DRM_DWORD jazz1_presentationtimes[] = {
     1578,
     1903,
     2228,
     2553,
     2797,
     3017,
     3313,
     3632,
     3963,
     4236,
     4614,
     4943,
     5258,
     5583,
     5914,
     6243,
     6593,
     6965,
     7348,
     7766,
     8146,
     8497,
     8834,
     9211,
     9519,
     9948,
    10308,
    10691,
    11063,
    11388,
    11748
};

static DRM_DWORD jazz_clear_sample_offsets[] = {
    2522,          /* Sample 0 */
    8498,
    14474,
    20450,
    26426,
    32402,         /* Sample 5 */
    38378,
    44354,
    50330,
    56306,
    62282,         /* Sample 10 */
    68258,
    74234,
    80210,
    86186,
    92162,         /* Sample 15 */
    98138,
    104114,
    110090,
    116066,
    122042,        /* Sample 20 */
    128018,
    133994,
    139970,
    145946,
    151922,        /* Sample 25 */
    157898,
    163874,
    169850,
    175826,
    181802         /* Sample 30 */
};

/* helper functions */
DRM_CHAR DRM_API TST_UTL_tolower( DRM_CHAR szChar )
{
    if ( szChar >= 'A' && szChar <= 'Z' )
    {
        return szChar - 'A' + 'a';
    }

    return szChar;
}

/* had to copy this function from pfddrmmanager.c because it's a static in there */
static DRM_VOID PackDWORDSInDRMID(
    DRM_ID *pId,
    DRM_DWORD dw1,
    DRM_DWORD dw2,
    DRM_DWORD dw3,
    DRM_DWORD dw4 )
{
    PUT_BYTE(pId->rgb, 0, ( dw1 >> 24));
    PUT_BYTE( pId->rgb,  1, ( dw1 >> 16) );
    PUT_BYTE( pId->rgb,  2, ( dw1 >> 8) );
    PUT_BYTE( pId->rgb,  3, ( dw1 ) );

    PUT_BYTE( pId->rgb,  4, ( dw2 >> 24) );
    PUT_BYTE( pId->rgb,  5, ( dw2 >> 16) );
    PUT_BYTE( pId->rgb,  6, ( dw2 >> 8) );
    PUT_BYTE( pId->rgb,  7, ( dw2 ) );

    PUT_BYTE( pId->rgb,  8, ( dw3 >> 24) );
    PUT_BYTE( pId->rgb,  9, ( dw3 >> 16) );
    PUT_BYTE( pId->rgb, 10, ( dw3 >> 8) );
    PUT_BYTE( pId->rgb, 11, ( dw3 ) );

    PUT_BYTE( pId->rgb, 12, ( dw4 >> 24) );
    PUT_BYTE( pId->rgb, 13, ( dw4 >> 16) );
    PUT_BYTE( pId->rgb, 14, ( dw4 >> 8) );
    PUT_BYTE( pId->rgb, 15, ( dw4 ) );
}

static DRM_RESULT _FindSession(
    IN     NDTMGR_CONTEXT                      *f_pNdtMgrContext,
    IN     DRM_ID                               f_idSession,
    IN     NDTMGR_SESSION_TYPE                  f_SessionType,
       OUT NDTMGR_INTERNAL_SESSION_CONTEXT    **f_ppSessionContext
)
{
    DRM_RESULT                       dr        = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL         *pContext  = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    NDTMGR_INTERNAL_SESSION_CONTEXT *pSession  = NULL;

    ChkArg( f_pNdtMgrContext != NULL && f_ppSessionContext != NULL );

    for(pSession = pContext->pFirstSession; pSession; pSession = pSession->pNextContext)
    {
        if ( 0 == MEMCMP(&pSession->idSession, &f_idSession, DRM_ID_SIZE) )
        {
            break;
        }
    }

    if(NULL == pSession || f_SessionType != pSession->sessionType)
    {
        ChkDR( DRM_E_ND_INVALID_SESSION );
    }

    *f_ppSessionContext = pSession;

ErrorExit:
    return dr;
}


DRM_RESULT ParseDRMID(DRM_ID *pTo, const char *pFrom)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *p  = (DRM_BYTE *) pFrom;
    int        i  = 0;

    for(i = 0; i < 16; i++)
    {
        char c;
        char c2;
        c = (char)TST_UTL_tolower(*p++);
        c2 = (char)TST_UTL_tolower(*p++);
        if(c == 0 || c2 == 0)
        {
            return DRM_E_INVALIDARG;
        }
       PUT_BYTE((DRM_BYTE *)pTo, i, (((c >= '0' && c <= '9') ? (c - '0') : (c >= 'a' && c <= 'f' ? (10 + c - 'a') : 0)) << 4) +
           ((c2 >= '0' && c2 <= '9') ? (c2 - '0') : (c2 >= 'a' && c2 <= 'f' ? (10 + c2 - 'a') : 0)));
    }
    return dr;
}

DRM_RESULT ParseDRMUINT64( DRM_UINT64 *pqwTo, const char *pFrom )
{
    const char *p      = NULL;
    DRM_WORD    wBase  = 10;
    DRM_DWORD   wDigit = 0;

    *pqwTo = DRM_UI64( 0 );

    /* skip past whitespace */
    for(p = pFrom; DRMCRT_iswspace( WCHAR_CAST( *p ) ); p++);

    if(*p == '0')
    {
        p++;
        if(*p == 'x' || *p == 'X')
        {
            wBase = 16;
            p++;
        }
        else
        {
            wBase = 8;
        }
    }

    for(; *p; p++)
    {
        if( DRMCRT_iswdigit( WCHAR_CAST( *p ) ) )
        {
            wDigit = *p - '0';
            if(wDigit > wBase)
            {
                break;
            }
        }
        else if( wBase == 16 && DRMCRT_iswxdigit( WCHAR_CAST ( *p ) ) )
        {
            wDigit = 10 + NATIVE_WCHAR( DRMCRT_towlower( WCHAR_CAST ( *p ) ) ) - 'a';
        }
        *pqwTo = DRM_UI64Add( DRM_UI64Mul( *pqwTo, DRM_UI64((DRM_DWORD) wBase) ), DRM_UI64((DRM_DWORD) wDigit) );
    }
    return DRM_SUCCESS;
}

DRM_RESULT ParseGUID(DRM_GUID *pGuid, DRM_BYTE *pString)
{
    DRM_RESULT dr      = DRM_SUCCESS;
    char       tmp[39] = { 0 };
    DRM_DWORD  bTmp    = 0;
    int        i       = 0;

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

DRM_RESULT OpenFileIntoBuffer(IN const char* szFileName, OUT DRM_BYTE** ppBuffer, OUT DRM_DWORD* pcbBuffer, DRM_DWORD cbMax)
{
    DRM_DWORD     lSize            = 0;
    char          *p               = NULL;
    DRM_WCHAR     wszFilename[256] = { 0 };
    DRM_RESULT    dr               = DRM_SUCCESS;
    OEM_FILEHDL   fh               = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD     cb               = 0;
    DRM_SUBSTRING dsubstr          = EMPTY_DRM_SUBSTRING;
    DRM_STRING    dstr             = EMPTY_DRM_STRING;
    DRM_BOOL      fOldFail         = FALSE;

#ifdef __TMS320C55X__
    int i;

    for(i = 0; i < strlen(szFileName) + 1; i++)
    {
        PUT_BYTE(rgbFilename, i, szFileName[i]);
    }

    dstr.pwszString = wszFilename;
    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(szFileName);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(rgbFilename, &dsubstr, &dstr);
#else
    dstr.pwszString = wszFilename;
    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(szFileName);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(szFileName, &dsubstr, &dstr);
#endif /* __TMS320C55X__ */

    if ( dstr.cchString < NO_OF( wszFilename ) )
    {
        wszFilename[dstr.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL )
    }

    fh = Oem_File_Open(NULL, wszFilename, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if(fh == OEM_INVALID_HANDLE_VALUE)
    {
        TRACE(("Unable to open file %s.\n", szFileName));
        return DRM_E_FAIL;
    }

    Oem_File_GetSize(fh, &lSize);
    if(cbMax != 0 && lSize > cbMax)
    {
        lSize = cbMax;
    }
    if(lSize > 0x1fffe)
    {
        TRACE(("file %s too large to open into buffer\n", szFileName));
        return DRM_E_FAIL;
    }

    fOldFail = g_fFailAllocation;
    g_fFailAllocation = FALSE;
    *ppBuffer = (DRM_BYTE*) Oem_MemAlloc (lSize == 0 ? 1 : lSize);
    g_fFailAllocation = fOldFail;
    ChkMem (*ppBuffer);

    p = (char*)*ppBuffer;
    Oem_File_Read(fh, p, lSize, &cb);

    /*** the whole file is loaded in the buffer. ***/
    /* terminate  */
    *pcbBuffer = (DRM_DWORD)lSize;
    Oem_File_Close(fh);

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL CloseClearSampleFile()
{
    DRM_RESULT          dr            = DRM_SUCCESS;
    TEMP_ASF_SAMPLE    *pTmpAsfSample = NULL;
    int                 i             = 0;

    /* Deallocate resources used by GetClearSampleFromFile() and initialize clear sample variables */
    for (i = 1; i <= MAX_STREAMS_IN_ASF; i++)
    {
        while ( g_aryPtrListClearFileSamples[i].dwCurrentSize > 0 )
        {
            (DRM_VOID) DRM_PtrList_RemoveHead( &(g_aryPtrListClearFileSamples[i]), (void **)&pTmpAsfSample );
            DRMASSERT( pTmpAsfSample != NULL );
            SAFE_OEM_FREE( pTmpAsfSample->pbData );
            SAFE_OEM_FREE( pTmpAsfSample );
        }

        DRM_PtrList_Uninitialize( &(g_aryPtrListClearFileSamples[i]) );
    }

    g_qwFirstPacketOffset = DRM_UI64( 0 );

    /* Erase the parser state */
    ZEROMEM( &g_ClearFilePacketInfo, SIZEOF( g_ClearFilePacketInfo ) );
    ZEROMEM( &g_ClearFilePayloadInfo, SIZEOF( g_ClearFilePayloadInfo ) );
    ZEROMEM( &g_ClearFileState, SIZEOF( g_ClearFileState ) );
    g_ClearFileState.pPacketInfo  = &g_ClearFilePacketInfo;
    g_ClearFileState.pPayloadInfo = &g_ClearFilePayloadInfo;

    if ( OEM_INVALID_HANDLE_VALUE != g_hClearFile )
    {
        Oem_File_Close(g_hClearFile);
        g_hClearFile = OEM_INVALID_HANDLE_VALUE;
    }

    return dr;
}

static DRM_RESULT OpenClearSampleFile(IN const char *szFileName)
{
    DRM_WCHAR     wszFilename[256]   = { 0 };
    DRM_RESULT    dr                 = DRM_SUCCESS;
    DRM_SUBSTRING dsubstr            = EMPTY_DRM_SUBSTRING;
    DRM_STRING    dstr               = EMPTY_DRM_STRING;
    DRM_BYTE     *pData              = NULL;
    DRM_DWORD     dwWanted           = 0;
    DRM_DWORD     dwOffset           = 0;
    DRM_DWORD     cbActual           = 0;
    DRM_UINT64    qwSize             = DRM_UI64( 0 );
    DRM_UINT64    qwTotalDataPackets = DRM_UI64( 0 );
    DRM_DWORD     dwPacketSize       = 0;
    DRM_DWORD     dwHeaders          = 0;
    DRM_BOOL      fFound             = FALSE;
    int           i                  = 0;
    DRM_GUID      guidTmp            = { 0 };

#ifdef __TMS320C55X__
    for(i = 0; i < strlen(szFileName) + 1; i++)
    {
        PUT_BYTE(rgbFilename, i, szFileName[i]);
    }

    dstr.pwszString = wszFilename;
    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(szFileName);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(rgbFilename, &dsubstr, &dstr);
#else
    dstr.pwszString = wszFilename;
    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(szFileName);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(szFileName, &dsubstr, &dstr);
#endif /* __TMS320C55X__ */

    if ( dstr.cchString < NO_OF( wszFilename ) )
    {
        wszFilename[dstr.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL )
    }

    /* Close the old file and set up clear file variables */
    CloseClearSampleFile();

    /* Init all of the linked list that will store our samples */
    for (i = 1; i <= MAX_STREAMS_IN_ASF; i++)
    {
        ChkDR( DRM_PtrList_Initialize( &(g_aryPtrListClearFileSamples[i]), 256 ) );
    }

    /* Open the file */
    g_hClearFile = Oem_File_Open(NULL, wszFilename, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if ( g_hClearFile == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_FAIL );
    }
    Oem_File_SetFilePointer( g_hClearFile, 0, OEM_FILE_BEGIN, NULL );

    /* Read header object */
    dwWanted = SIZEOF( DRM_GUID   ) + /* Object ID */
               SIZEOF( DRM_UINT64 ) + /* Object Size */
               SIZEOF( DRM_DWORD  ) + /* Number of Header Objects */
               1                    + /* Reserved1 */
               1;                     /* Reserved2 */
    pData = (DRM_BYTE *)Oem_MemAlloc( dwWanted );
    ChkMem( pData );
    Oem_File_Read(g_hClearFile, pData, dwWanted, &cbActual);
    if ( cbActual != dwWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    dwOffset = 0;
    LoadGUIDOffset( guidTmp, pData, dwOffset );
    ChkArg( 0 == DRM_BYT_CompareBytes( &CLSID_ASFHeaderObjectV0, 0, &guidTmp, 0, SIZEOF( DRM_GUID ) ) );

    DRM_BYT_CopyBytes( &qwSize, 0, pData, dwOffset, SIZEOF(DRM_UINT64));
    FIX_ENDIAN_QWORD( qwSize );
    dwOffset += SIZEOF( DRM_UINT64 ); /* Object Size */

    DRM_BYT_CopyBytes( &dwHeaders, 0, pData, dwOffset, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD( dwHeaders );
    dwOffset += SIZEOF( DRM_DWORD ); /* Number of Header Objects */

    g_ClearFileState.qwHeaderSize = qwSize;
    g_ClearFileState.dwNoOfHeaders = dwHeaders;

    SAFE_OEM_FREE(pData);

    /* Read the rest of the header object into a buffer */
    dwWanted = (DRM_DWORD) DRM_UI64Low32(g_ClearFileState.qwHeaderSize) - dwWanted;
    pData = (DRM_BYTE *)Oem_MemAlloc( dwWanted );
    ChkMem( pData );
    Oem_File_Read(g_hClearFile, pData, dwWanted, &cbActual);
    if ( cbActual != dwWanted )
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    /* Search through the extended header objects for the file properties object so we can pull the number of packets out */
    dwOffset = 0;
    while ( TRUE != fFound )
    {
        LoadGUIDOffset( guidTmp, pData, dwOffset );
        if ( 0 == DRM_BYT_CompareBytes( &CLSID_ASFFilePropertiesObjectV2, 0, &guidTmp, 0, SIZEOF( DRM_GUID ) ) )
        {
            fFound = TRUE;
        }

        qwSize = DRM_UI64( 0 );
        DRM_BYT_CopyBytes( &qwSize, 0, pData, dwOffset, SIZEOF( DRM_UINT64 ) );
        FIX_ENDIAN_QWORD( qwSize );
        dwOffset += DRM_UI64Low32( qwSize ) - SIZEOF( DRM_GUID );

        ChkBOOL( dwOffset <= (cbActual - SIZEOF( DRM_GUID ) - SIZEOF( DRM_UINT64 ) ), DRM_E_FAIL );
    }

    /* Extract packet information from File Properties Object */
    DRM_BYT_CopyBytes(&dwPacketSize, 0, pData, dwOffset - 104 + 96, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD( dwPacketSize );
    g_ClearFileState.dwPacketSize = dwPacketSize;

    g_ClearFileState.iCurrentFileOffset = DRM_UI2I64( g_ClearFileState.qwHeaderSize );
    SAFE_OEM_FREE(pData);

    /* Read data object */
    dwWanted = SIZEOF( DRM_GUID   ) + /* Object ID */
               SIZEOF( DRM_UINT64 ) + /* Object Size */
               SIZEOF( DRM_GUID   ) + /* File ID */
               SIZEOF( DRM_UINT64 ) + /* Total Data Packets */
               SIZEOF( DRM_WORD   ) ; /* Reserved */
    pData = (DRM_BYTE *)Oem_MemAlloc( dwWanted );
    Oem_File_Read(g_hClearFile, pData, dwWanted, &cbActual);

    dwOffset = 0;
    dwOffset += SIZEOF( DRM_GUID ); /* Object ID */

    qwSize = DRM_UI64( 0 );
    DRM_BYT_CopyBytes( &qwSize, 0, pData, dwOffset, SIZEOF(DRM_UINT64));
    FIX_ENDIAN_QWORD( qwSize );
    dwOffset += SIZEOF( DRM_UINT64 ); /* Object Size */

    dwOffset += SIZEOF( DRM_GUID ); /* File ID */

    DRM_BYT_CopyBytes( &qwTotalDataPackets, 0, pData, dwOffset, SIZEOF(DRM_UINT64));
    FIX_ENDIAN_QWORD( qwTotalDataPackets );
    dwOffset += SIZEOF( DRM_UINT64 ); /* Total Data Packets */

    g_ClearFileState.dwTotalPackets = (DRM_DWORD) DRM_UI64Low32(qwTotalDataPackets);
    g_ClearFileState.tParseState = P_NEW_PACKET;
    g_ClearFileState.iCurrentFileOffset = DRM_I64Add( g_ClearFileState.iCurrentFileOffset, DRM_I64( dwWanted ) );
    g_ClearFileState.iCurrentPacketOffset = g_ClearFileState.iCurrentFileOffset;
    Oem_File_SetFilePointer( g_hClearFile, (DRM_LONG) DRM_UI64Low32( DRM_I2UI64( g_ClearFileState.iCurrentFileOffset ) ), OEM_FILE_BEGIN, NULL );

    g_qwFirstPacketOffset = DRM_I2UI64( g_ClearFileState.iCurrentPacketOffset );

    goto Exit;

ErrorExit:
    if ( OEM_INVALID_HANDLE_VALUE != g_hClearFile )
    {
        Oem_File_Close(g_hClearFile);
    }

 Exit:
    SAFE_OEM_FREE(pData);

    return dr;
}

static DRM_RESULT GetClearSampleFromFile(IN DRM_BOOL fResetFileOffset, IN DRM_DWORD dwStreamID, IN DRM_DWORD dwPresentationTime, OUT DRM_DWORD* pcbSampleData, OUT DRM_BYTE** ppbSampleData)
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_DWORD        cbActual          = 0;
    DRM_DWORD        cbTotalFound      = 0;
    DRM_DWORD        dwBufferSpaceAvailable    = 0;

    DRM_INT64        iNextPacketOffset = DRM_I64LITERAL( 0, 0 );
    DRM_BYTE        *pFileData         = NULL;
    DRM_BYTE        *pbPayloadData     = NULL;
    DRM_BOOL         fFoundSample      = FALSE;
    TEMP_ASF_SAMPLE *pTmpAsfSample     = NULL;
    int              i                 = 0;
    DRM_BOOL         fResult            = FALSE;

    ChkArg( dwStreamID <= MAX_STREAMS_IN_ASF );

    /* File buffer allocation */
    ChkMem( pFileData = (DRM_BYTE *)Oem_MemAlloc( FILE_BUFFER_SIZE ) );

    ChkMem(*ppbSampleData);

    /* Reset the searching file pointer if requested */
    if (fResetFileOffset)
    {
        for (i = 1; i <= MAX_STREAMS_IN_ASF; i++)
        {

            while ( g_aryPtrListClearFileSamples[i].dwCurrentSize > 0 )
            {
                (DRM_VOID) DRM_PtrList_RemoveHead( &(g_aryPtrListClearFileSamples[i]), (void **)&pTmpAsfSample );
                DRMASSERT( pTmpAsfSample != NULL );
                SAFE_OEM_FREE( pTmpAsfSample->pbData );
                SAFE_OEM_FREE( pTmpAsfSample );
            }
        }

        ZEROMEM( &g_ClearFilePacketInfo, SIZEOF( g_ClearFilePacketInfo ) );
        ZEROMEM( &g_ClearFilePayloadInfo, SIZEOF( g_ClearFilePayloadInfo ) );

        g_ClearFileState.iCurrentFileOffset = DRM_UI2I64( g_qwFirstPacketOffset );
        g_ClearFileState.iPreviousPacketOffset = DRM_UI2I64( g_qwFirstPacketOffset );
        g_ClearFileState.iCurrentPacketOffset = DRM_UI2I64( g_qwFirstPacketOffset );
        g_ClearFileState.iNextPacketOffset = DRM_UI2I64( g_qwFirstPacketOffset );
        g_ClearFileState.dwPacketsProcessed = 0;
        g_ClearFileState.tParseState = P_NEW_PACKET;
    }
    else
    {
        /* Copy in any fragments that we've already read, if available */
        /* pop the head off of the list for this stream id */
        if ( DRM_SUCCESS == DRM_PtrList_RemoveHead( &(g_aryPtrListClearFileSamples[dwStreamID]), (void **)&pTmpAsfSample ) )
        {
            /* assume that we will be asking for these samples in order. therefore, if we don't match the first sample, it's an error */
            DRMASSERT( pTmpAsfSample != NULL );
            ChkBOOL( pTmpAsfSample->dwPresentationTime == dwPresentationTime, DRM_E_FAIL );
            DRMASSERT( pTmpAsfSample->pbData != NULL );

            DRM_BYT_CopyBytes(*ppbSampleData, 0, pTmpAsfSample->pbData, 0, pTmpAsfSample->cbDataHave);
            cbTotalFound = pTmpAsfSample->cbDataHave;

            /* if we already have the complete sample, return the sample size */
            if (pTmpAsfSample->cbDataHave == pTmpAsfSample->cbDataNeed)
            {
                *pcbSampleData = pTmpAsfSample->cbDataHave;
                fFoundSample = TRUE;
            }

            /* Erase the fragments that we've stored */
            SAFE_OEM_FREE( pTmpAsfSample->pbData );
            SAFE_OEM_FREE( pTmpAsfSample );
        }
    }

    /* Search through packets for the correct payload */
    while (fFoundSample == FALSE && g_ClearFileState.dwPacketsProcessed < g_ClearFileState.dwTotalPackets)
    {
        /* Parse the PACKET header */
        if (g_ClearFileState.tParseState == P_NEW_PACKET)
        {
            ChkDR( _ASFParsePacketHeader(g_hClearFile, pFileData, &g_ClearFileState) );
            g_ClearFileState.dwPayload = 0;
        }

        if (!( g_ClearFileState.pPacketInfo->fEccPresent &&
               g_ClearFileState.pPacketInfo->fParityPacket ) )
        {
            while ( g_ClearFileState.dwPayload < g_ClearFileState.pPacketInfo->dwPayloads )
            {
                g_ClearFileState.tParseState = P_PAYLOAD_HEADER;

                /* Parse the PAYLOAD header */
                ChkDR( _ASFParsePayloadHeader(g_hClearFile, pFileData, &g_ClearFileState) );

                /* See if this is the correct payload or payload fragment */
                if ( g_ClearFileState.pPayloadInfo->bStreamId == dwStreamID && g_ClearFileState.pPayloadInfo->dwObjectPres == dwPresentationTime )
                {
                    /* Read the payload data into a temporary data buffer */
                    ChkMem( pbPayloadData = (DRM_BYTE *)Oem_MemAlloc( g_ClearFileState.pPayloadInfo->wPayloadSize ) );
                    fResult = Oem_File_Read(g_hClearFile, pbPayloadData, (DRM_DWORD) g_ClearFileState.pPayloadInfo->wPayloadSize, &cbActual);
                    DRMASSERT( fResult );
                    DRMASSERT( (DRM_DWORD) g_ClearFileState.pPayloadInfo->wPayloadSize == cbActual );

                    /* If the space needed is more that the pre-allocated buffer, then realloc the buffer */
                    dwBufferSpaceAvailable = *pcbSampleData - g_ClearFileState.pPayloadInfo->dwObjectOffset;
                    if(  dwBufferSpaceAvailable < cbActual )
                    {
                        *pcbSampleData = cbActual + g_ClearFileState.pPayloadInfo->dwObjectOffset;
                        ChkMem(*ppbSampleData = (DRM_BYTE *)Oem_MemRealloc(*ppbSampleData, *pcbSampleData ));
                    }

                    /* Copy the payload data into the sample data buffer */
                    DRM_BYT_CopyBytes(*ppbSampleData, g_ClearFileState.pPayloadInfo->dwObjectOffset, pbPayloadData, 0, cbActual);
                    cbTotalFound += cbActual;

                    SAFE_OEM_FREE(pbPayloadData);

                    /* Quit if we've found all of the payload fragments */
                    if (cbTotalFound == g_ClearFileState.pPayloadInfo->dwObjectSize)
                    {
                        *pcbSampleData = (DRM_DWORD) g_ClearFileState.pPayloadInfo->dwObjectSize;
                        fFoundSample = TRUE;
                    }
                }
                else if (fResetFileOffset == FALSE)
                {
                    DRMASSERT( g_ClearFileState.pPayloadInfo->bStreamId <= MAX_STREAMS_IN_ASF );

                    /* if the offset into the sample object is > 0, then we should be appending this data on to the last sample */
                    if ( g_ClearFileState.pPayloadInfo->dwObjectOffset == 0 )
                    {
                        /* Allocate space to save the fragment(s) */
                        ChkMem( pTmpAsfSample = (TEMP_ASF_SAMPLE *) Oem_MemAlloc( SIZEOF(TEMP_ASF_SAMPLE) ) );
                        pTmpAsfSample->pbData = (DRM_BYTE *)Oem_MemAlloc( g_ClearFileState.pPayloadInfo->dwObjectSize );
                        ChkMem( pTmpAsfSample->pbData );
                        pTmpAsfSample->dwPresentationTime = g_ClearFileState.pPayloadInfo->dwObjectPres;
                        pTmpAsfSample->cbDataHave = 0;
                        pTmpAsfSample->cbDataNeed = g_ClearFileState.pPayloadInfo->dwObjectSize;
                        ChkDR( DRM_PtrList_AddTail( &(g_aryPtrListClearFileSamples[g_ClearFileState.pPayloadInfo->bStreamId]), pTmpAsfSample ) );
                    }
                    else
                    {
                        /* get the tail of the linked list */
                        ChkDR( DRM_PtrList_GetTail( &(g_aryPtrListClearFileSamples[g_ClearFileState.pPayloadInfo->bStreamId]), (void **)&pTmpAsfSample ) );
                        ChkMem( pTmpAsfSample );

                        /* this should be the same sample. if not, we have a sequence error */
                        DRMASSERT( g_ClearFileState.pPayloadInfo->dwObjectPres == pTmpAsfSample->dwPresentationTime );
                    }

                    ChkMem( pbPayloadData = (DRM_BYTE *)Oem_MemAlloc( g_ClearFileState.pPayloadInfo->wPayloadSize ) );
                    DRMASSERT( pTmpAsfSample->dwPresentationTime == g_ClearFileState.pPayloadInfo->dwObjectPres );
                    fResult = Oem_File_Read(g_hClearFile, pbPayloadData, (DRM_DWORD) g_ClearFileState.pPayloadInfo->wPayloadSize, &cbActual);
                    DRMASSERT( fResult );
                    DRMASSERT( (DRM_DWORD) g_ClearFileState.pPayloadInfo->wPayloadSize == cbActual );

                    /* Copy the payload data into the sample data buffer */
                    DRM_BYT_CopyBytes(pTmpAsfSample->pbData, g_ClearFileState.pPayloadInfo->dwObjectOffset, pbPayloadData, 0, cbActual);
                    pTmpAsfSample->cbDataHave += cbActual;

                    SAFE_OEM_FREE(pbPayloadData);
                }
                else
                {
                    /* Skip over payload data */
                    g_ClearFileState.wPayStart = g_ClearFileState.pPayloadInfo->wPacketOffset +
                        g_ClearFileState.pPayloadInfo->wTotalSize -
                        g_ClearFileState.pPayloadInfo->wPayloadSize;
                    g_ClearFileState.iCurrentFileOffset = DRM_I64Add( g_ClearFileState.iCurrentFileOffset, DRM_I64((DRM_DWORD) g_ClearFileState.pPayloadInfo->wPayloadSize ) );
                    fResult = Oem_File_SetFilePointer( g_hClearFile,
                                                   (DRM_LONG) DRM_UI64Low32(DRM_I2UI64(g_ClearFileState.iCurrentFileOffset)),
                                                   OEM_FILE_BEGIN,
                                                   NULL );
                    DRMASSERT( fResult );
                }

                g_ClearFileState.dwPayload++;

                if (fFoundSample)
                {
                    break;
                }
            }
        }

        if (fFoundSample)
        {
            break;
        }

        /* Move to the next packet */
        g_ClearFileState.tParseState = P_NEW_PACKET;
        iNextPacketOffset = DRM_UI2I64( DRM_UI64Add( DRM_I2UI64( g_ClearFileState.iCurrentPacketOffset ),
                                                     DRM_UI64( g_ClearFileState.dwPacketSize) ) );
        fResult = Oem_File_SetFilePointer(g_hClearFile, DRM_UI64Low32(DRM_I2UI64(iNextPacketOffset)), OEM_FILE_BEGIN, NULL);
        DRMASSERT( fResult );
        g_ClearFileState.iCurrentFileOffset = iNextPacketOffset;
        g_ClearFileState.iCurrentPacketOffset = iNextPacketOffset;
        g_ClearFileState.dwPacketsProcessed++;
    }

    if (!fFoundSample)
    {
        ChkDR(DRM_E_FAIL);
    }

ErrorExit:
    SAFE_OEM_FREE(pFileData);

    return dr;
}

static DRM_RESULT _ParseLengthType(DRM_BYTE   bLenType,
                                   DRM_BYTE  *pbLen,
                                   DRM_DWORD  dwLenTypeOffset,
                                   DRM_DWORD *pdwTotalUsed,
                                   DRM_DWORD *pdwExplicitValue)
{
    DRM_RESULT dr = DRM_SUCCESS;

    *pdwExplicitValue = 0;

    switch( bLenType )
    {
    case 0x01:
        DRM_BYT_CopyBytes(pdwExplicitValue, 0, pbLen, dwLenTypeOffset, 1);
        *pdwTotalUsed += 1;
        break;
    case 0x02:
        DRM_BYT_CopyBytes(pdwExplicitValue, 0, pbLen, dwLenTypeOffset, SIZEOF(DRM_WORD));
        FIX_ENDIAN_WORD(*pdwExplicitValue);
        *pdwTotalUsed += SIZEOF(DRM_WORD);
        break;
    case 0x03:
        DRM_BYT_CopyBytes(pdwExplicitValue, 0, pbLen, dwLenTypeOffset, SIZEOF(DRM_DWORD));
        FIX_ENDIAN_DWORD(*pdwExplicitValue);
        *pdwTotalUsed += SIZEOF(DRM_DWORD);
        break;
    }

    return dr;
}


/* most of this was borrowed from pfdasftranscrypt.c */
static DRM_RESULT ParseASFPacketHeader(DRM_BYTE *pbPkt, DRM_DWORD dwPktOffset, ASFPACKET_PARSER_INFO *pPacketInfo, DRM_DWORD *pdwPayloadOffset)
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_BYTE   b           = 0;
    DRM_DWORD  dwTotalUsed = 0;

    /* Copy Error Correction data straight to the data buffer */
    pPacketInfo->fParityPacket = FALSE;
    pPacketInfo->dwParseOffset = 0;

    b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
    dwTotalUsed += 1;

    pPacketInfo->fEccPresent = (DRM_BOOL)( ( b & 0x80 ) == 0x80 );
    pPacketInfo->bECLen = 0;

    if ( pPacketInfo->fEccPresent )
    {
        /* Error correction Data is there */
        /*
        if ( b & 0x10 )
        {
            /* Means Opaque Data, Not Payloads */
            /*
            pPacketInfo->fParityPacket = TRUE;
            return DRM_SUCCESS;
        }
        */

        if ( b & 0x60 )
        {
            /* If error correction data is NOT coded with 4 bits */
            ChkDR( DRM_E_FAIL );
        }

        pPacketInfo->bECLen = ( b & 0x0f );
        if ( pPacketInfo->bECLen != 2 )
        {
            ChkDR( DRM_E_FAIL );
        }

        /* Skip over EC Data */
        dwTotalUsed += (DRM_DWORD)pPacketInfo->bECLen;

        /* Read first byte of the payload parsing information */
        b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
        dwTotalUsed += 1;
    }

    /* Length type flags */
    pPacketInfo->dwPacketLenTypeOffset = dwTotalUsed - 1;

    /* Padding length type */
    pPacketInfo->bPadLenType = ( b & 0x18 ) >> 3;
    if ( pPacketInfo->bPadLenType == 3 )
    {
       ChkDR( DRM_E_FAIL );
    }

    /* Packet len type */
    pPacketInfo->bPacketLenType = ( b & 0x60 ) >> 5;

    if ( pPacketInfo->bPacketLenType != 0 &&
         pPacketInfo->bPacketLenType != 2 )
    {
        ChkDR( DRM_E_FAIL );
    }

    /* Must be 0 */
    pPacketInfo->bSequenceLenType = ( b & 0x06 ) >> 1;

    pPacketInfo->fMultiPayloads = (DRM_BOOL)( 0x1 == (b & 0x01) );

    /* We only support 1 payload per packet */
    DRMASSERT( pPacketInfo->fMultiPayloads == FALSE );

    /*
     * Read property flags
     */
    b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
    dwTotalUsed += 1;

    /* Offset to media object offset and offset type
     * We add 2 to skip EC data size byte and length type flags
    */
    pPacketInfo->bOffsetBytes = pPacketInfo->bECLen + 2;
    /* Offset into media object length type should always be DWORD */
    pPacketInfo->bOffsetLenType = 3;

    /* It must be 5d according to specs */
    if ( b != 0x5d )
    {
        /* Stream Number len type is not 01 (bits 6,7) */
        if ( ( b & 0xc0 ) != 0x40 )
        {
            ChkDR( DRM_E_FAIL );
        }

        if ( ( b & 0x30 ) != 0x10 )
        {
            /* Obj number type len is not 01 */
            ChkDR( DRM_E_FAIL );
        }

        /* Bits 2,3 */
        pPacketInfo->bOffsetLenType = ( b & 0x0c ) >> 2;

        /* Should be 11 */
        if ( pPacketInfo->bOffsetLenType == 0 )
        {
            return DRM_E_FAIL;
        }
        else if ( pPacketInfo->bOffsetLenType < 3 )
        {
            /* not in spec */
            DRMASSERT( !"Offset into Media Object length type not in spec" );
            pPacketInfo->bOffsetBytes = pPacketInfo->bOffsetLenType;
        }

        if ( ( b & 0x03 ) != 0x01 )
        {
            /* Rep data len type is not 01 (bit 0,1) */
            ChkDR( DRM_E_FAIL );
        }
    }

    /* Packet length type */
    pPacketInfo->dwPacketLenOffset = dwTotalUsed;
    ChkDR( _ParseLengthType(pPacketInfo->bPacketLenType,
                            pbPkt,
                            dwPktOffset + dwTotalUsed,
                            &dwTotalUsed,
                            &(pPacketInfo->dwExplicitPacketLength)) );

    /* Sequence length type */
    pPacketInfo->dwSequenceOffset = dwTotalUsed;

    ChkDR( _ParseLengthType( pPacketInfo->bSequenceLenType,
                             pbPkt,
                             dwPktOffset + dwTotalUsed,
                             &dwTotalUsed,
                             &(pPacketInfo->dwSequenceNum) )  );

    /* Padding length type */
    pPacketInfo->dwPadLenOffset = dwTotalUsed;

    ChkDR( _ParseLengthType( pPacketInfo->bPadLenType,
                             pbPkt,
                             dwPktOffset + dwTotalUsed,
                             &dwTotalUsed,
                             &(pPacketInfo->dwPadding) )  );

    /* Now read Send Time and Duration (6 bytes) */

    DRM_BYT_CopyBytes( &pPacketInfo->dwSCR, 0, pbPkt, dwPktOffset + dwTotalUsed, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD( pPacketInfo->dwSCR);
    dwTotalUsed += SIZEOF( DRM_DWORD );

    DRM_BYT_CopyBytes( &pPacketInfo->wDuration, 0, pbPkt, dwPktOffset + dwTotalUsed, SIZEOF(DRM_WORD) );
    FIX_ENDIAN_WORD(pPacketInfo->wDuration);
    dwTotalUsed += SIZEOF( DRM_WORD );


    if ( pPacketInfo->fEccPresent && pPacketInfo->fParityPacket )
    {
        ChkDR( DRM_E_FAIL );
    }

    pPacketInfo->dwPayLenTypeOffset = 0;
    pPacketInfo->bPayLenType = 0;
    pPacketInfo->bPayBytes = 0;
    pPacketInfo->dwPayloads = 1;

    *pdwPayloadOffset = dwTotalUsed;
    pPacketInfo->dwParseOffset = dwPktOffset + dwTotalUsed;

ErrorExit:
    return dr;
}

/******************************************************************************/
static DRM_RESULT ParseASFPayloadHeader(DRM_BYTE *pbPkt,
                                        DRM_DWORD dwPktOffset,
                                        ASFPACKET_PARSER_INFO *pPacketInfo,
                                        ASFPAYLOAD_PARSER_INFO *pPayloadInfo,
                                        DRM_DWORD dwPacketSize,
                                        DRM_UINT64 *pqwCurrentSampleID,
                                        DRM_DWORD *pSampleDataOffset)
{
    DRM_RESULT dr              = DRM_SUCCESS;

    DRM_DWORD  dwPayloadSize   = 0;
    DRM_BYTE   b               = 0;
    DRM_DWORD  dwTotalUsed     = 0;
    DRM_DWORD  dwRepDataOffset = 0;
    DRM_WORD   wTotalDataBytes = 0;

    pPayloadInfo->wPacketOffset = (DRM_WORD) pPacketInfo->dwParseOffset;

    /* Read Stream ID and Media Object Number */
    b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
    dwTotalUsed += 1;

    pPayloadInfo->bStreamId = b & 0x7f;
    pPayloadInfo->fIsKeyFrame = ( ( b & 0x80 ) >> 7 );

    b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
    dwTotalUsed += 1;

    /* Only byte is accepted */
    pPayloadInfo->bObjectId = b;

    /*  Find out the Extn Data Size for this stream id */

    ChkDR( _ParseLengthType( pPacketInfo->bOffsetLenType,
                             pbPkt,
                             dwPktOffset + dwTotalUsed,
                             &dwTotalUsed,
                             &(pPayloadInfo->dwObjectOffset) )  );

    DRMASSERT( dwTotalUsed == (DRM_DWORD)( pPacketInfo->bOffsetBytes + 2 ) );

    /* Replicated data offset (from payload's perspective)
     * Replicated data offset = stream number length + media object number length
     * + offset into media object length
    */
    /*dwRepDataOffset = pPacketInfo->bOffsetBytes + 2;*/
    dwRepDataOffset = 1 + 1 + SIZEOF( DRM_DWORD );

    /* Read Replicated Data length */
    b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
    dwTotalUsed += 1;

    /* RepData Length. Only byte is accepted. */
    pPayloadInfo->bRepData = b;
    pPayloadInfo->dwObjectPres = 0xffffffff;

    /* If replicated data length is set to 1, this is compressed payload data */
    if ( pPayloadInfo->bRepData == 1 )
    {
        pPayloadInfo->fIsCompressedPayload = TRUE;
        pPayloadInfo->dwObjectPres = pPayloadInfo->dwObjectOffset;
        pPayloadInfo->dwObjectOffset = 0;
        pPayloadInfo->dwObjectSize = 0;

        /* Read Presentation Time Delta */
        b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
        dwTotalUsed += 1;

        pPayloadInfo->dwDeltaPresTime = b;
        wTotalDataBytes = 0;
    }
    else if( pPayloadInfo->bRepData >= 8 )
    {
        pPayloadInfo->fIsCompressedPayload = FALSE;

        /* Read Media Object Size and Media Object Presentation Time */
        DRM_BYT_CopyBytes( &pPayloadInfo->dwObjectSize, 0, pbPkt, dwPktOffset + dwTotalUsed, SIZEOF(DRM_DWORD));
        FIX_ENDIAN_DWORD(pPayloadInfo->dwObjectSize);
        dwTotalUsed += SIZEOF(DRM_DWORD);

        DRM_BYT_CopyBytes( &pPayloadInfo->dwObjectPres, 0, pbPkt, dwPktOffset + dwTotalUsed, SIZEOF(DRM_DWORD));
        FIX_ENDIAN_DWORD(pPayloadInfo->dwObjectPres);
        dwTotalUsed += SIZEOF(DRM_DWORD);
    }


    pPayloadInfo->wTotalSize = (DRM_WORD)( dwRepDataOffset +
                                           1 +
                                           pPayloadInfo->bRepData );

    DRMASSERT( pPayloadInfo->wTotalSize >= dwTotalUsed );

    if ( pPayloadInfo->wTotalSize > dwTotalUsed )
    {
        dwTotalUsed += 2;
        NETWORKBYTES_TO_QWORD( *pqwCurrentSampleID, pbPkt, dwPktOffset + dwTotalUsed );
        dwTotalUsed = pPayloadInfo->wTotalSize;
    }

    /* We only support one payload per packet */
    DRMASSERT( pPacketInfo->fMultiPayloads == FALSE );

    pPacketInfo->dwParseOffset += dwTotalUsed;
    if( pPacketInfo->dwExplicitPacketLength > 0 )
    {
        dwPayloadSize = pPacketInfo->dwExplicitPacketLength -
                        pPacketInfo->dwParseOffset -
                        pPayloadInfo->wTotalSize -
                        pPacketInfo->dwPadding;
    }
    else
    {
        /*
        dwPayloadSize = dwPacketSize -
                        pPacketInfo->dwParseOffset -
                        pPayloadInfo->wTotalSize -
                        pPacketInfo->dwPadding;
        */

        /* payload size = total packet size - packet header - padding */
        dwPayloadSize = dwPacketSize -
                        pPayloadInfo->wPacketOffset -
                        pPayloadInfo->wTotalSize -
                        pPacketInfo->dwPadding;
    }

    if ( 0 == wTotalDataBytes )
    {
        wTotalDataBytes = (DRM_WORD)dwPayloadSize;
    }

    pPayloadInfo->wPayloadSize = (DRM_WORD)dwPayloadSize;

    pPayloadInfo->wTotalSize += pPacketInfo->bPayBytes +
                                (DRM_WORD)pPayloadInfo->wPayloadSize;

    pPayloadInfo->wTotalDataBytes = wTotalDataBytes;

    pPacketInfo->dwParseOffset += dwPktOffset + pPayloadInfo->wTotalSize;

    *pSampleDataOffset = dwTotalUsed;

ErrorExit:
    return dr;
}

/* Map a WMDRMND_RESULT to a DRM_RESULT */
static DRM_RESULT MapProximityResultToDR(WMDRMND_RESULT wrResult)
{
    DRM_RESULT dr = DRM_SUCCESS;

    switch( wrResult )
    {
    case WMDRMND_E_INVALIDCERTIFICATE:
        dr = DRM_E_INVALIDDEVICECERTIFICATE;
        break;
    case WMDRMND_E_CERTIFICATEREVOKED:
        dr = DRM_E_CERTIFICATE_REVOKED;
        break;
    case WMDRMND_E_LICENSEUNAVAILABLE:
        dr = DRM_E_LICENSENOTFOUND;
        break;
    case WMDRMND_E_TRANSMITTERFAILURE:
        dr = DRM_E_FAIL;
        break;
    case WMDRMND_E_DEVICELIMITREACHED:
        dr = DRM_E_ND_DEVICE_LIMIT_REACHED;
        break;
    case WMDRMND_E_UNABLETOVERIFYPROXIMITY:
        dr = DRM_E_ND_UNABLE_TO_VERIFY_PROXIMITY;
        break;
    case WMDRMND_E_MUSTREGISTER:
        dr = DRM_E_DEVICE_NOT_REGISTERED;
        break;
    case WMDRMND_E_MUSTREVALIDATE:
        dr = DRM_E_ND_MUST_REVALIDATE;
        break;
    case WMDRMND_E_INVALIDPROXIMITYRESPONSE:
        dr = DRM_E_ND_INVALID_PROXIMITY_RESPONSE;
        break;
    case WMDRMND_E_INVALIDSESSION:
        dr = DRM_E_ND_INVALID_SESSION;
        break;
    case WMDRMND_E_UNABLETOOPENFILE:
        dr = DRM_E_FAIL;
        break;
    case WMDRMND_E_BADREQUEST:
        dr = DRM_E_ND_BAD_REQUEST;
        break;
    }

    return dr;
}


/**********************************************************************
**
** Function:    TestCreateAppContext
**
** Synopsis:    This function creates an app context and a revocation buffer
**              to go with it.
**
** Arguments:   [f_HdsPath]        - Supplies a filename for the datastore.
**              [f_ppAppContext]   - Supplies a variable that receives a
**                                   pointer to the allocated App Context, or
**                                   NULL on error.
**
** Returns:     DRM_RESULT
**
**********************************************************************/
static DRM_RESULT TestCreateAppContext(
    __in  DRM_CONST_STRING *f_HdsPath,
    __out DRM_APP_CONTEXT **f_ppAppContext
    )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_BYTE        *pbRevocationBuffer = NULL;
    DRM_APP_CONTEXT *pAppContext        = NULL;
    DRM_BYTE        *pbOpaqueBuffer     = NULL;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_HdsPath, TEST_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pAppContext ) );

    /*
    TRACE(("Initialized DrmMgrContext %p\n", pAppContext));
    */

    ChkMem( pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );

    ChkDR( Drm_Revocation_SetBuffer( pAppContext,
                                     pbRevocationBuffer,
                                     REVOCATION_BUFFER_SIZE ) );

    pbRevocationBuffer = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbRevocationBuffer );

    if (DRM_FAILED(dr))
    {
        DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pAppContext );
    }

    *f_ppAppContext = pAppContext;

    return dr;
}


/**********************************************************************
**
** Function:    TestDeleteAppContext
**
** Synopsis:    This function deletes an app context and frees its revocation
**              buffer. It is the counterpart to TestCreateAppContext.
**
** Arguments:   [f_pAppContext]            - Supplies a pointer to the App Context to free
**              [f_fUsingSharedAppContext] - Indicates whether or not using a shared AppContext
**
** Returns:     Nothing.
**
**********************************************************************/
static DRM_VOID TestDeleteAppContext(
    __in DRM_APP_CONTEXT *f_pAppContext,
    __in DRM_BOOL f_fUsingSharedAppContext
    )
{
    DRM_RESULT dr                 = DRM_SUCCESS;
    DRM_BYTE  *pbRevocationBuffer = NULL;
    DRM_DWORD  cbRevocationBuffer = 0;

    if ( ( !f_fUsingSharedAppContext ) && ( f_pAppContext != NULL ) )
    {
        DRM_BYTE *pbOpaqueBuffer = NULL;

        dr = Drm_GetOpaqueBuffer( f_pAppContext, &pbOpaqueBuffer );
        DRMASSERT( DRM_SUCCEEDED( dr ) );

        dr = Drm_Revocation_GetBuffer( f_pAppContext ,
                                       &pbRevocationBuffer,
                                       &cbRevocationBuffer );

        DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &f_pAppContext );

        if( DRM_SUCCEEDED( dr ) )
        {
            SAFE_OEM_FREE( pbRevocationBuffer );
        }
    }
    else
    {
        SAFE_OEM_FREE( g_pbSharedRevBuffer );
    }

}


/**********************************************************************
**
** Function:    TestCloseSession
**
** Synopsis:    This function closes an NDT session and frees the App Context.
**              created for it.
**
** Arguments:   [f_pNdContext]   - Supplies a pointer to the master NDT context.
**              [f_idSession]    - Supplies the ID of the session to delete.
**
** Returns:     DRM_RESULT.
**
**********************************************************************/
DRM_RESULT TestCloseSession(
    __in ND_TRANSMITTER_CONTEXT *f_pNdContext,
    __in DRM_ID                  f_idSession
    )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_APP_CONTEXT *pAppContext = NULL;

    dr = Drm_Ndt_Session_Close( f_pNdContext->pNdtMgrContext,
                                f_idSession,
                                &pAppContext );

    if( pAppContext != NULL )
    {
        TestDeleteAppContext( pAppContext, f_pNdContext->fSharedAppContext );
    }

    return dr;
}


/**********************************************************************
**
** Function:    TestAddCleanupEntry
**
** Synopsis:    This function adds a cleanup task to the current master NDT
**              context's cleanup list. During test case exit, this cleanup
**              task will be performed (if this weren't so, app contexts
**              would be leaked, pinning the HDS in memory).
**
** Arguments:   [f_eType] - Supplies one of:
**                          1. NdcPolicy (a policy object to free),
**                          2. NdcNdtSession (a non-ASF session object to free)
**                          3. NdcNdtAsfSession (an ASF session object to free)
**              [f_pArg]  - Supplies a pointer to the object to free.
**
** Returns:     DRM_RESULT (can fail due to low memory).
**
**********************************************************************/
DRM_RESULT TestAddCleanupEntry(
    __in ND_CLEANUP_OBJECT_TYPE f_eType,
    __in DRM_VOID *f_pArg
    )
{
    DRM_RESULT         dr = DRM_SUCCESS;
    ND_CLEANUP_OBJECT *pNdcObject;

    ChkMem( pNdcObject = (ND_CLEANUP_OBJECT *)Oem_MemAlloc(SIZEOF(ND_CLEANUP_OBJECT)) );
    TRACE(("Adding cleanup entry %p [%d %p]\n", g_pCurNdtContext, f_eType, f_pArg));

    pNdcObject->pNext = g_pCurNdtContext->pCleanupHead;
    g_pCurNdtContext->pCleanupHead = pNdcObject;

    pNdcObject->eObjectType = f_eType;
    pNdcObject->pArg = f_pArg;
ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    TestCleanupList
**
** Synopsis:    This function executes the cleanup tasks for the specified
**              NDT master context.
**
** Arguments:   [f_pNdContext] - Supplies the test structure that wraps an
**                               NDT context. This structure contains the
**                               cleanup list amongst other things.
**
** Returns:     Nothing.
**
**********************************************************************/
DRM_VOID TestCleanupList(
    __inout ND_TRANSMITTER_CONTEXT *f_pNdContext
    )
{
    ND_CLEANUP_OBJECT *pNdcObject;
    ND_CLEANUP_OBJECT *pNdcNext;
    DRM_APP_CONTEXT   *pAppContext = NULL;

    for (pNdcObject = f_pNdContext->pCleanupHead;
         pNdcObject != NULL;
         pNdcObject = pNdcNext)
    {
        /*
        TRACE(("Cleanup up entry %p [%d %p]\n", f_pNdContext, pNdcObject->eObjectType, pNdcObject->pArg));
        */

        pNdcNext = pNdcObject->pNext;

        switch (pNdcObject->eObjectType)
        {
        case NdcPolicy:
            SAFE_OEM_FREE( pNdcObject->pArg );
            break;

        case NdcNdtSession:
            TestCloseSession( f_pNdContext, *(DRM_ID *)pNdcObject->pArg );
            SAFE_OEM_FREE( pNdcObject->pArg );
            break;

        case NdcNdtAsfSession:
            Drm_Ndt_Asf_Close( f_pNdContext->pNdtMgrContext,
                               *(DRM_ID *)pNdcObject->pArg,
                               &pAppContext );
            if( pAppContext != NULL )
            {
                TestDeleteAppContext( pAppContext, f_pNdContext->fSharedAppContext );
            }
            SAFE_OEM_FREE( pNdcObject->pArg );
        }

        SAFE_OEM_FREE( pNdcObject );
    }

    f_pNdContext->pCleanupHead = NULL;
}


/**
 * Allows NDTMGR_CONTEXT * that gets passed to all NDT API Functions to be set to
 * an arbitrary value.
 *
 * argv[0] = value
 */
DRM_RESULT DRM_CALL Test_SetpNdtDrmContext(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_DWORD  tmp = 0;

    ChkArg(argc == 1 && argv[0]);

    tmp = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    if ( 0 == tmp )
    {
        g_pNdtDrmContext = NULL;
    }

ErrorExit:
    return dr;
}


/**
 * Allows PrivateKey that gets passed to some functions to be set
 *
 * argv[0] = path to file containing the private key exponent
 */
DRM_RESULT DRM_CALL Test_SetPrivateKey(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                    dr         = DRM_SUCCESS;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_1024;
    DRM_BYTE                     *pTmp       = NULL;
    DRM_DWORD                     cbTmp      = 0;

    ChkArg(argc == 1 && argv[0]);

    ChkDR(OpenFileIntoBuffer(argv[0], &pTmp, &cbTmp, 0));
    ChkMem(pTmp);

    OEM_RSA_ZeroPrivateKey(&g_PrivateKey);

    ChkDR(OEM_RSA_SetPrivateKey( eKeyLength,
                                 cbTmp,
                                 pTmp,
                                 DRM_RSA_CB_PRIME( eKeyLength ),
                                 g_rgbRSAPrimes[ 0 ],
                                 g_rgbRSAPrimes[ 1 ],
                                 &g_PrivateKey,
                                 &g_Crypto));

ErrorExit:
    SAFE_OEM_FREE(pTmp);
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

    ChkArg( argc == 1 && argv[ 0 ] );
    ChkDR( OpenFileIntoBuffer( argv[ 0 ], &pbTmp, &cbTmp, 0 ) );
    ChkMem( pbTmp) ;

    OEM_RSA_ZeroPrivateKey( &g_PrivateKey );

    MEMCPY( &g_PrivateKey, pbTmp, MIN( cbTmp, SIZEOF(DRM_RSA_PRIVATE_KEY) ) );

ErrorExit:

    SAFE_OEM_FREE( pbTmp );

    return dr;
}

/**
 * Allows Certificate that gets passed to some functions to be set
 *
 * argv[0] = path to Certificate
 */
DRM_RESULT DRM_CALL Test_SetCertificate(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);

    SAFE_OEM_FREE( g_pCertificate );
    ChkDR(OpenFileIntoBuffer(argv[0], &g_pCertificate, &g_cbCertificate, 0));
    ChkMem(g_pCertificate);

ErrorExit:
    return dr;
}

/**
 * Allows cbCertificate that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbCertificate
 */
DRM_RESULT DRM_CALL Test_SetcbCertificate(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbCertificate = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

static DRM_RESULT _CheckMap( const char* map[], DRM_DWORD cbMap, const char* sz)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dw = 0;

    ChkArg( map && sz );

    while( dw < cbMap && strcmp( sz, map[dw] ) )
    {
        dw++;
    }

    ChkBOOL( dw < cbMap , DRM_E_FAIL );

    dr = dw;

ErrorExit:
    return dr;
}


static void _FreeLicenses(void)
{
    if ( g_pXmrLicense != g_RootXmrLicenseInfo.pXmrLicense )
    {
        DrmXmrFreeLicense(g_pXmrLicense);
    }
    DrmXmrFreeLicense(g_RootXmrLicenseInfo.pXmrLicense);
    g_RootXmrLicenseInfo.pXmrLicense = NULL;
    g_pXmrLicense = NULL;
    SAFE_OEM_FREE(g_RootXmrLicenseInfo.pIDChainedChecksum);
    if ( NULL != g_RootXmrLicenseInfo.pAesContentKey )
    {
        Oem_Aes_ZeroKey(g_RootXmrLicenseInfo.pAesContentKey);
        g_RootXmrLicenseInfo.pAesContentKey = NULL;
    }

}

#define MAX_CRL_FILES 3

DRM_APP_CONTEXT* GetCurrentAppContext()
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( g_pCurNdtContext->fInitialized == TRUE );

ErrorExit:
    if(DRM_FAILED(dr))
    {
        return NULL;
    }
    return g_pCurNdtContext->DrmMgrContext;
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

/**
 * argv[0] - type of revocation list
 * argv[1] - file containing license response with revocation list
 */
DRM_RESULT DRM_CALL Test_SetRevocationList(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_BYTE        *pbBuffer       = NULL;
    DRM_DWORD        cbBuffer       = 0;
    DRM_APP_CONTEXT *pDrmMgrContext = GetCurrentAppContext();
    const char*      map[]          = { "APP", "DEVICE", "WMDRMNET" };
    DRM_DWORD        i              = 0;
    DRM_RVK_LIST     RevocationList = { 0 };
    DRM_GUID        *pGuid          = NULL;
    DRM_DWORD        cbDecoded      = 0;
    DRM_SUBSTRING    dasstrSource   = EMPTY_DRM_SUBSTRING;
    DRM_BOOL         fDecode        = TRUE;

    ChkArg(argc == 2 && argv[0] && argv[1]);
    if(strcmp(argv[0], "RIV") == 0)
    {
        pGuid = (DRM_GUID *) &GUID_WMDRMRevocationTypeRiv;
        fDecode = FALSE;
    }
    else
    {
        ChkArg( MAX_CRL_FILES == NO_OF(map) );
        ChkDR( i = _CheckMap( map, MAX_CRL_FILES, argv[0] ) )

        ChkDR( _DrmRevocationTypeEnumGUID( (DRM_REVOCATION_TYPE_ENUM) i, &pGuid ) );
    }

    MEMCPY( &RevocationList.guidRevocationList, pGuid, DRM_ID_SIZE );

    ChkDR( OpenFileIntoBuffer(argv[1],&pbBuffer,&cbBuffer, 0) );

    if(fDecode)
    {
        dasstrSource.m_ich = 0;
        dasstrSource.m_cch = cbBuffer;
        dr = DRM_B64_DecodeA( (DRM_CHAR *)pbBuffer, &dasstrSource, &cbDecoded, NULL, DRM_BASE64_DECODE_IN_PLACE );
        ChkDR( dr );

        RevocationList.pbRevocationList = pbBuffer;
        RevocationList.cbRevocationList = cbDecoded;
    }
    else
    {
        RevocationList.pbRevocationList = pbBuffer;
        RevocationList.cbRevocationList = cbBuffer;
    }

    ChkDR( Drm_Revocation_StoreRevListArray( pDrmMgrContext, 1, &RevocationList ));

ErrorExit:
    SAFE_OEM_FREE(pbBuffer);
    return dr;
}

/**
 * Allows pbData that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpbDataNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpbDataNULL = TRUE;

    return dr;
}

/**
 * Allows pcbData that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpcbDataNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpcbDataNULL = TRUE;

    return dr;
}

/**
 * Allows pbSampleData that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpbSampleDataNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpbSampleDataNULL = TRUE;

    return dr;
}

/**
 * Allows pcbSampleData that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpcbSampleDataNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpcbSampleDataNULL = TRUE;

    return dr;
}

/**
 * Allows cbSampleData that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbSampleData
 */
DRM_RESULT DRM_CALL Test_SetcbSampleData(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbSampleDataOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows pSampleInfo that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpSampleInfoNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpSampleInfoNULL = TRUE;

    return dr;
}

/**
 * Allows pbHeader that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpbHeaderNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpbHeaderNULL = TRUE;

    return dr;
}

/**
 * Allows pcbHeader that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpcbHeaderNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpcbHeaderNULL = TRUE;

    return dr;
}

/**
 * Allows cbHeader that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbHeader
 */
DRM_RESULT DRM_CALL Test_SetcbHeader(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbHeaderOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows pbLeafLicenseResponse that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpbLeafLicenseResponseNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpbLeafLicenseResponseNULL = TRUE;

    return dr;
}

/**
 * Allows pcbLeafLicenseResponse that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpcbLeafLicenseResponseNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpcbLeafLicenseResponseNULL = TRUE;

    return dr;
}

/**
 * Allows cbLeafLicenseResponse that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbLeafLicenseResponse
 */
DRM_RESULT DRM_CALL Test_SetcbLeafLicenseResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbLeafLicenseResponseOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows pKID that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetpKIDNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fpKIDNULL = TRUE;

    return dr;
}

/**
 * No arguments frees g_pWMDRMNETPolicy and sets it to NULL
 *
 * argv[0] = string          policy type
 * argv[1] = DRM_WORD        minimum security level
 * argv[2] = DRM_DWORD       minimum application revocation list version
 * argv[3] = DRM_DWORD       minimum device revocation list version
 */
DRM_RESULT DRM_CALL Test_SetWMDRMNETPolicy(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                       dr                    = DRM_SUCCESS;
    WMDRMNET_POLICY                 *pPolicy               = NULL;
    WMDRMNET_POLICY_TRANSCRYPTPLAY  *pPolicyTranscryptPlay = NULL;
    WMDRMNET_POLICY_TYPE             ePolicyType           = WMDRMNET_POLICY_TYPE_UNDEFINED;
    DRM_DWORD                        cbPolicy              = 0;

    if (argc == 0)
    {
        /* free the current policy */
        g_pWMDRMNETPolicy = NULL;
    }
    else
    {
        ChkArg(argc == 4 && argv[0] && argv[1] && argv[2] && argv[3]);

        /* Set the policy type */
        if ( 0 == strcmp(argv[0], "UNDEFINED") )
        {
            ePolicyType = WMDRMNET_POLICY_TYPE_UNDEFINED;
        }
        else if ( 0 == strcmp(argv[0], "TRANSCRYPTPLAY") )
        {
            ePolicyType =  WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY;
        }
        else if ( 0 == strcmp(argv[0], "INVALID_TYPE") )
        {
            ePolicyType =  (WMDRMNET_POLICY_TYPE)9999;
        }
        else
        {
            ChkDR(DRM_E_INVALIDARG);
        }

        /* Create and populate policy */
        cbPolicy = SIZEOF( WMDRMNET_POLICY ) +
                   SIZEOF( WMDRMNET_POLICY_TRANSCRYPTPLAY );
        pPolicy = (WMDRMNET_POLICY *)Oem_MemAlloc( cbPolicy );
        ChkMem( pPolicy );
        ChkDR( TestAddCleanupEntry( NdcPolicy, (DRM_VOID *)pPolicy ) );
        ZEROMEM( pPolicy, cbPolicy );

        pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *) pPolicy->pbPolicy;
        pPolicy->ePolicyType = ePolicyType;

        /* Set License Min App Security */
        pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel = (DRM_WORD) strtoul(argv[1], NULL, 0);

        /* Set Min App Revocation List Version */
        pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumAppRevocationListVersion = (DRM_DWORD) strtoul(argv[2], NULL, 0);

        /* Set Min WMDRM-ND Revocation List Version */
        pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumDeviceRevocationListVersion = (DRM_DWORD) strtoul(argv[3], NULL, 0);

        g_pWMDRMNETPolicy = pPolicy;
        pPolicy = NULL;
    }

ErrorExit:

    SAFE_OEM_FREE( pPolicy );

    return dr;
}

/**
 * Allows MediaSessionID that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_ID MediaSessionID
 */
DRM_RESULT DRM_CALL Test_SetMediaSessionID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    ParseDRMID(&g_MediaSessionID, argv[0]);

ErrorExit:
    return dr;
}

/**
 * Allows MediaSessionID that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetMediaSessionIDNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fMediaSessionIDNULL = TRUE;

    return dr;
}

/**
 * Allows g_dwASFFlags that gets passed to some functions to be set to
 * a NULL value.
 *
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetASFFlagsNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fASFFlagsNULL = TRUE;

    return dr;
}

/**
 * Allows RegistrationSessionID that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_ID RegistrationSessionID
 */
DRM_RESULT DRM_CALL Test_SetRegistrationSessionID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    ChkDR( ParseDRMID(&g_RegistrationSessionID, argv[0]) );

ErrorExit:
    return dr;
}

/**
 * Allows SerialNumber that gets passed to some functions to be set
 *
 * argv[0] = DRM_ID SerialNumber
 */
DRM_RESULT DRM_CALL Test_SetSerialNumber(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    ParseDRMID(&g_SerialNumber, argv[0]);

ErrorExit:
    return dr;
}

/**
 * Allows Seed that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] =  DRM_ID Seed
 */
DRM_RESULT DRM_CALL Test_SetSeed(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    ParseDRMID(&g_Seed, argv[0]);

ErrorExit:
    return dr;
}

/**
 * Allows cbRegistrationResponseMsg that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbRegistrationResponseMsgOverride
 */
DRM_RESULT DRM_CALL Test_SetcbRegistrationResponseMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);

    g_cbRegistrationResponseMsgOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows pbRegistrationResponseMsg to be set to NULL
 */
DRM_RESULT DRM_CALL Test_SetpbRegistrationResponseMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_pbRegistrationResponseMsgNULL = 1;

    return dr;
}

/**
 * Allows pbRegistrationRequestMsg to be set to NULL
 */
DRM_RESULT DRM_CALL Test_SetpbRegistrationRequestMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_pbRegistrationRequestMsgNULL = 1;

    return dr;
}

/**
 * Instructs to test proximity api calls to set pProximityStartMsg to NULL
 */
DRM_RESULT DRM_CALL Test_SetProximityStartMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fProximityStartMsgNULL = TRUE;

    return dr;
}

/**
 * Instructs to test proximity api calls to set pProximityResultMsg to NULL
 */
DRM_RESULT DRM_CALL Test_SetProximityResponseMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fProximityResponseMsgNULL = TRUE;

    return dr;
}

/**
 * Instructs to test proximity api calls to test for DRM_E_BUFFERTOOSMALL
 */
DRM_RESULT DRM_CALL Test_SetTestProximityBUFFERTOOSMALL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fTestProximityBUFFERTOOSMALL = TRUE;

    return dr;
}

/**
 * Allows cbRegistrationRequestMsg that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD RegistrationRequestMsgOverride
 */
DRM_RESULT DRM_CALL Test_SetcbRegistrationRequestMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0] && argv[0]);

    g_cbRegistrationRequestMsgOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows cbLicenseChallengeMsg that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbLicenseChallengeMsgOverride
 */
DRM_RESULT DRM_CALL Test_SetcbLicenseChallengeMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbLicenseChallengeMsgOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows cbLicenseResponseMsg that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbLicenseResponseMsgOverride
 */
DRM_RESULT DRM_CALL Test_SetcbLicenseResponseMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbLicenseResponseMsgOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * Allows cbLicenseResponseMsg to be set to NULL
 */
DRM_RESULT DRM_CALL Test_SetcbLicenseResponseMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_cbLicenseResponseMsgNULL = 1;

    return dr;
}

/**
 * Allows pbLicenseResponseMsg to be set to NULL
 */
DRM_RESULT DRM_CALL Test_SetpbLicenseResponseMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_pbLicenseResponseMsgNULL = 1;

    return dr;
}

/**
 * argv[0] = DRM_DWORD       f_dwChallenge1
 * argv[1] = DRM_DWORD       f_dwChallenge2
 * argv[2] = DRM_DWORD       f_dwChallenge3
 * argv[3] = DRM_DWORD       f_dwChallenge4
 */
DRM_RESULT DRM_CALL Test_SetChallengeID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  dwChallenge1 = 0;
    DRM_DWORD  dwChallenge2 = 0;
    DRM_DWORD  dwChallenge3 = 0;
    DRM_DWORD  dwChallenge4 = 0;

    ChkArg(argc == 4 && argv[0] && argv[1] && argv[2] && argv[3]);

    dwChallenge1 = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    dwChallenge2 = (DRM_DWORD) strtoul(argv[1], NULL, 0);
    dwChallenge3 = (DRM_DWORD) strtoul(argv[2], NULL, 0);
    dwChallenge4 = (DRM_DWORD) strtoul(argv[3], NULL, 0);

    PackDWORDSInDRMID(&g_idChallengeOverride, dwChallenge1, dwChallenge2, dwChallenge3, dwChallenge4);

    g_ChallengeIDOverride = 1;

ErrorExit:
    return dr;
}

/**
 * argv[0] = DRM_BYTE       MessageType
 */
DRM_RESULT DRM_CALL Test_SetMessageType(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);

    g_MessageTypeOverride = (DRM_BYTE) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * argv[0] = DRM_BYTE       MessageType
 */
DRM_RESULT DRM_CALL Test_SetMessageProtocolVersion(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);

    g_MessageProtocolVersionOverride = (DRM_BYTE) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

/**
 * argv[0] = milliseconds to sleep (or keyword for preset sleep time)
 */
DRM_RESULT DRM_CALL Test_ChangeTimeMilliseconds(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr       = DRM_SUCCESS;
    DRM_DWORD    sleepage = 0;  /* milliseconds */

    ChkArg(argc == 1 && argv[0]);

    if ( 0 == strcmp(argv[0], "REGISTRATION_TIMEOUT") )  /* is measured in seconds */
    {
        sleepage = (DRM_DWORD) (NDT_REGISTRATION_TIMEOUT + 1) * (DRM_DWORD) 1000;
    }
    else if ( 0 == strcmp(argv[0], "VALIDATION_TIMEOUT") )  /* is measured in seconds */
    {
        sleepage = (DRM_DWORD) (DRM_DEVICESTORE_VALIDATION_TIMEOUT + 1) * (DRM_DWORD) 1000;
    }
    else if ( 0 == strcmp(argv[0], "PROXIMITY_TIMEOUT") )  /* is measured in milliseconds */
    {
        sleepage = (DRM_DWORD) (PROXIMITY_RTT_THRESHOLD_MS + 1);
    }
    else
    {
        sleepage = strtoul(argv[0], NULL, 0);
    }

    if( sleepage > 0 )
    {
        if ( sleepage <= 1000 )  /* less than one second, simply delay here inline */
        {
            DRM_DWORD start;
            DRM_DWORD elapsed;

            start = Oem_Clock_GetTickCount( NULL );
            elapsed = start;
            while ( start + sleepage > elapsed )
            {
                elapsed = Oem_Clock_GetTickCount( NULL );
            }
        }
        else  /* greater than one second, adjust the system clock */
        {
            sleepage /= 1000; /* get back to seconds */
            tChangeSystemTime( sleepage );
        }
    }

#ifdef WINCE_TEST
    Sleep(2000);
#endif

ErrorExit:
    return dr;
}

/**
 * argv[0]  = IN  name of flag to check to retrieve: NDTMGR_ASF_WMDRMPROTECTED, NDTMGR_ASF_LICENSE_PLAYAVAILABLE
 * argv[1]  = IN  DRM_DWORD       state of flag: (1) set, (0) not set
 */
DRM_RESULT DRM_CALL Test_CheckASFFlags(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  dwFlagState = 0;

    ChkArg(argc == 2 && argv[0] && argv[1]);

    dwFlagState = (DRM_DWORD) strtoul(argv[1], NULL, 0);

    if ( 0 == strcmp(argv[0], "NDTMGR_ASF_WMDRMPROTECTED") )
    {
        ChkBOOL( (DRM_DWORD)( (g_dwASFFlags & NDTMGR_ASF_WMDRMPROTECTED) > 0 ) == dwFlagState, DRM_E_FAIL );
    }
    else if ( 0 == strcmp(argv[0], "NDTMGR_ASF_LICENSE_PLAYAVAILABLE") )
    {
        ChkBOOL( (DRM_DWORD)( (g_dwASFFlags & NDTMGR_ASF_LICENSE_PLAYAVAILABLE) > 0 ) == dwFlagState, DRM_E_FAIL );
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:

    return dr;
}

/**
 */
DRM_RESULT DRM_CALL Test_CheckLastDataBlock(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( g_LastDataBlock == 1, DRM_E_FAIL );

ErrorExit:

    return dr;
}


/*
 * argv[0] = actual transferred wma file (created by calling open media session and getalldatablocks)
 * argv[1] = expected file to compare against
 */
DRM_RESULT DRM_CALL Test_CheckNotTranscrypted(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_BYTE   *pbActual   = NULL;
    DRM_BYTE   *pbExpected = NULL;
    DRM_DWORD   cbActual   = 0;
    DRM_DWORD   cbExpected = 0;

    ChkArg( argc == 2 && argv[0] && argv[1] );

    ChkDR( OpenFileIntoBuffer(argv[0], &pbActual, &cbActual, 0) );
    ChkMem( pbActual );

    ChkDR( OpenFileIntoBuffer(argv[1], &pbExpected, &cbExpected, 0) );
    ChkMem( pbExpected );

    ChkArg( (cbActual == cbExpected) && 0 == (MEMCMP(pbActual, pbExpected, cbActual)) );

ErrorExit:

    SAFE_OEM_FREE( pbActual );
    SAFE_OEM_FREE( pbExpected );

    return dr;
}

/*
 * argv[0] = SourceID to compare against
 */
DRM_RESULT DRM_CALL Test_CheckSourceID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr         = DRM_SUCCESS;
    DRM_DWORD   dwSourceID = 0;

    ChkArg( argc == 1 && argv[0] && g_pXmrLicense != NULL );

    dwSourceID = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    ChkArg( g_pXmrLicense->gr.dwSourceId == dwSourceID );

ErrorExit:

    return dr;
}

/*
 * argv[0] = Riv to compare against
 */
DRM_RESULT DRM_CALL Test_CheckRivInLicense(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr    = DRM_SUCCESS;
    DRM_DWORD   dwRiv = 0;

    ChkArg( argc == 1 && argv[0] && g_pXmrLicense != NULL );

    dwRiv = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    ChkArg( g_pXmrLicense->gr.dwRevInfoSeq == dwRiv );

ErrorExit:

    return dr;
}

/*
 * argv[0] = File Containing Device Key to Compare Against
 */
DRM_RESULT DRM_CALL Test_CheckDeviceKey(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT      dr        = DRM_SUCCESS;
    XMR_DEVICE_KEY  DeviceKey = {0, 0, NULL};
    DRM_DWORD       cbTmp     = 0;
    DRM_BYTE       *pbTmp     = NULL;
    DRM_DWORD       ibTmp     = 0;

    ChkArg( argc == 1 && argv[0] && g_pXmrLicense != NULL );
    ChkArg( g_pXmrLicense->km.dk.pbModulus != NULL );

    ChkDR(OpenFileIntoBuffer(argv[0], &pbTmp, &cbTmp, 0));
    ChkMem(pbTmp);

    ibTmp = 0;

    ChkArg( cbTmp > (SIZEOF(DRM_DWORD) + SIZEOF(DRM_WORD)) );

    DRM_BYT_CopyBytes( (DRM_BYTE*)&DeviceKey.dwExponent, 0, pbTmp, ibTmp, SIZEOF(DRM_DWORD) );
    FIX_ENDIAN_DWORD( DeviceKey.dwExponent );
    ibTmp += SIZEOF( DRM_DWORD );
    ChkArg( DeviceKey.dwExponent == g_pXmrLicense->km.dk.dwExponent);

    DRM_BYT_CopyBytes( (DRM_BYTE*)&DeviceKey.wModulusLength, 0, pbTmp, ibTmp, SIZEOF(DRM_WORD) );
    FIX_ENDIAN_WORD( DeviceKey.wModulusLength );
    ibTmp += SIZEOF( DRM_WORD );
    ChkArg( DeviceKey.wModulusLength == g_pXmrLicense->km.dk.wModulusLength);
    ChkArg( cbTmp >= (SIZEOF(DRM_DWORD) + SIZEOF(DRM_WORD) + DeviceKey.wModulusLength) );

    ChkMem( DeviceKey.pbModulus = (DRM_BYTE *) Oem_MemAlloc(DeviceKey.wModulusLength) );
    DRM_BYT_CopyBytes( DeviceKey.pbModulus, 0, pbTmp, ibTmp, DeviceKey.wModulusLength);
    ChkArg( 0 == MEMCMP(DeviceKey.pbModulus, g_pXmrLicense->km.dk.pbModulus, g_pXmrLicense->km.dk.wModulusLength) );

ErrorExit:
    SAFE_OEM_FREE(pbTmp);
    SAFE_OEM_FREE(DeviceKey.pbModulus);

    return dr;
}

/*
 * argv[0] = SettingsObject to compare against
 */
DRM_RESULT DRM_CALL Test_CheckSettingsObject(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_WORD    wSettingsObject = 0;

    ChkArg( argc == 1 && argv[0] && g_pXmrLicense != NULL );

    wSettingsObject = (DRM_WORD) strtoul(argv[0], NULL, 0);
    ChkArg( g_pXmrLicense->gr.wReserved == wSettingsObject );

ErrorExit:

    return dr;
}

/*
 * argv[0] = GUID1 to compare against (optional)
 * argv[1] = GUID2 to compare against (optional)
 * argv[2] = GUID3 to compare against (optional)
 * ...
 */
DRM_RESULT DRM_CALL Test_CheckInclusionList(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr   = DRM_SUCCESS;
    DRM_WORD    i    = 0;
    DRM_GUID    guid = { 0 };

    ChkArg( g_pXmrLicense != NULL && g_pXmrLicense->gr.il.cGUIDS == (unsigned long)argc );
    for (i = 0; i < argc; i++)
    {
        ChkDR( ParseGUID(&guid, (DRM_BYTE *)argv[i]) );
        ChkArg( 0 == MEMCMP(&guid, g_pXmrLicense->gr.il.pGUIDS + i, SIZEOF(DRM_GUID)) );
    }

ErrorExit:

    return dr;
}

/* drmndtmanager.h */

/**
 *  argv[0] = InitData
 */
DRM_RESULT DRM_CALL Test_DRM_NDT_Initialize(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_BOOL         fAppContextInitialized = FALSE;
    g_pCurNdtContext->dstrInitData.pwszString = NULL;
    g_pCurNdtContext->dstrInitData.cchString = 0;
    g_pCurNdtContext->fSharedAppContext = FALSE;

    if ( 0 == argc)
    {
        DRM_CONST_STRING dstrTemp;
        DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

        ChkDR( tGetDeviceStorePathname( wszPathName, &dstrTemp ) );

        ChkOverflow( dstrTemp.cchString + 1, dstrTemp.cchString );
        ChkMem( g_pCurNdtContext->dstrInitData.pwszString = (DRM_WCHAR *) Oem_MemAlloc((dstrTemp.cchString + 1) * SIZEOF(DRM_WCHAR)) );
        MEMCPY( g_pCurNdtContext->dstrInitData.pwszString, dstrTemp.pwszString, (dstrTemp.cchString + 1) * SIZEOF(DRM_WCHAR) );
        g_pCurNdtContext->dstrInitData.cchString = dstrTemp.cchString;
    }
    else if ( ( 1 == argc ) && ( ( argv[0] != NULL) ) )
    {
        if ( 0 == strcmp(argv[0], "SHARED_APP_CONTEXT") )
        {
            // For tests that require usage of shared app context without creating new appcontext
            GET_SHARED_APP_CONTEXT( g_pCurNdtContext->DrmMgrContext );
            g_pCurNdtContext->fSharedAppContext = TRUE;
            SAFE_OEM_FREE( g_pbSharedRevBuffer );
            ChkMem( g_pbSharedRevBuffer = (DRM_BYTE *)Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );
            ChkDR( Drm_Revocation_SetBuffer( g_pCurNdtContext->DrmMgrContext,
                                             g_pbSharedRevBuffer,
                                             REVOCATION_BUFFER_SIZE ) );
        }
        else if ( 0 == strcmp(argv[0], "secondstore.hds" ) )
        {
            // For tests that need to use two device stores and switch between the two contexts associated with each device store
            DRM_CONST_STRING dstrFileName        = {0};
            DRM_CONST_STRING dstrDeviceStorePath = {0};
            DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

            ChkDR( MakeDRMString( &dstrFileName, argv[0] ) );
            ChkDR( tGetDRMFileName( wszPathName, &dstrDeviceStorePath, dstrFileName.pwszString ) );
            ChkOverflow( dstrDeviceStorePath.cchString + 1, dstrDeviceStorePath.cchString );
            ChkMem( g_pCurNdtContext->dstrInitData.pwszString = (DRM_WCHAR *) Oem_MemAlloc((dstrDeviceStorePath.cchString + 1) * SIZEOF(DRM_WCHAR)) );
            MEMCPY( g_pCurNdtContext->dstrInitData.pwszString, dstrDeviceStorePath.pwszString, (dstrDeviceStorePath.cchString + 1) * SIZEOF(DRM_WCHAR) );
            g_pCurNdtContext->dstrInitData.cchString = dstrDeviceStorePath.cchString;
       }
    }


    /* If it's NULL we'll want to test that Drm_Ndt_Initialize fails */
    if( g_pCurNdtContext->dstrInitData.pwszString != NULL && !g_pCurNdtContext->fSharedAppContext )
    {
        ChkDR( TestCreateAppContext( (DRM_CONST_STRING *)&g_pCurNdtContext->dstrInitData,
                                     &g_pCurNdtContext->DrmMgrContext ));

        fAppContextInitialized = TRUE;
    }

    ChkDR( Drm_Ndt_Initialize( g_pNdtDrmContext,
                               g_pCurNdtContext->DrmMgrContext ) );

    g_pCurNdtContext->fInitialized = TRUE;

ErrorExit:
    if ( DRM_FAILED(dr) )
    {
        if ( fAppContextInitialized )
        {
            TestDeleteAppContext( g_pCurNdtContext->DrmMgrContext, g_pCurNdtContext->fSharedAppContext );
        }

        FREEDRMSTRING( g_pCurNdtContext->dstrInitData );
    }

    return dr;
}

/**
 *  No parameters
 */
DRM_RESULT DRM_CALL Test_DRM_NDT_Uninitialize(long argc, __in_ecount(argc) char *argv[])
{
    Drm_Ndt_Uninitialize( g_pNdtDrmContext );
    g_pNdtDrmContext = NULL;

    if ( g_pCurNdtContext )
    {
        g_pCurNdtContext->fInitialized = FALSE;

        TestDeleteAppContext( g_pCurNdtContext->DrmMgrContext, g_pCurNdtContext->fSharedAppContext );

        FREEDRMSTRING( g_pCurNdtContext->dstrInitData );

        g_pCurNdtContext->pNdtMgrContext = NULL;
    }

    return DRM_SUCCESS;
}

/**
 *  argv[0] = context number to switch to (0 thru MAXNDTCONTEXTS - 1)
 */
DRM_RESULT DRM_CALL Test_SwitchNdtContexts(long argc, __in_ecount(argc) char *argv[])
{
    int        nContext = 0;
    DRM_RESULT dr       = DRM_SUCCESS;

    ChkArg( argc == 1 && argv[0] );

    nContext = strtoul( argv[0], NULL, 0 );
    ChkArg( nContext < MAXNDTCONTEXTS );

    g_pCurNdtContext = &g_NdtContexts[nContext];
    if( NULL == g_pCurNdtContext->pNdtMgrContext )
    {
        g_pCurNdtContext->pNdtMgrContext = (NDTMGR_CONTEXT *)Oem_MemAlloc( SIZEOF( NDTMGR_CONTEXT ) );
        ChkMem( g_pCurNdtContext->pNdtMgrContext );
    }
    g_pNdtDrmContext = g_pCurNdtContext->pNdtMgrContext;
ErrorExit:

    return dr;
}

/**
 *  argv[0] = session number to switch to (0 thru MAXMEDIASESSIONS).
 *            NOTE: switching to argv[0] = MAXMEDIASESSIONS is intended to be used for a negative test case.
 */
DRM_RESULT DRM_CALL Test_SwitchMediaSession(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD  nSession = 0;
    DRM_RESULT dr       = DRM_SUCCESS;

    ChkArg( argc == 1 && argv[0] );

    nSession = strtoul( argv[0], NULL, 0 );
    ChkArg( nSession < MAXMEDIASESSIONS );

    /* save the current info */
    g_hOutputFiles[g_CurrentMediaSession] = g_hOutputFile;
    MEMCPY( &g_KIDs[g_CurrentMediaSession], &g_KID, DRM_ID_SIZE );
    g_pXmrLicenses[g_CurrentMediaSession] = g_pXmrLicense;
    MEMCPY( &g_RootXmrLicenseInfos[g_CurrentMediaSession], &g_RootXmrLicenseInfo, SIZEOF( XMR_ROOT_LICENSE_INFO ) );
    g_pContentEncryptionKeys[g_CurrentMediaSession] = g_pContentEncryptionKey;
    g_pRootContentEncryptionKeys[g_CurrentMediaSession] = g_pRootContentEncryptionKey;
    MEMCPY( &g_xmrMinEnvironments[g_CurrentMediaSession], &g_xmrMinEnvironment, SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );
//    MEMCPY( &g_SerialNumbers[g_CurrentMediaSession], &g_SerialNumber, DRM_ID_SIZE );
    MEMCPY( &g_MediaSessionIDs[g_CurrentMediaSession], &g_MediaSessionID, DRM_ID_SIZE );

    g_CurrentMediaSession = nSession;

    /* load the desired info */
    g_hOutputFile = g_hOutputFiles[g_CurrentMediaSession];
    MEMCPY( &g_KID, &g_KIDs[g_CurrentMediaSession], DRM_ID_SIZE );
    g_pXmrLicense = g_pXmrLicenses[g_CurrentMediaSession];
    MEMCPY( &g_RootXmrLicenseInfo, &g_RootXmrLicenseInfos[g_CurrentMediaSession], SIZEOF( XMR_ROOT_LICENSE_INFO ) );
    g_pContentEncryptionKey = g_pContentEncryptionKeys[g_CurrentMediaSession];
    g_pRootContentEncryptionKey = g_pRootContentEncryptionKeys[g_CurrentMediaSession];
    MEMCPY( &g_xmrMinEnvironment, &g_xmrMinEnvironments[g_CurrentMediaSession], SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );
//    MEMCPY( &g_SerialNumber, &g_SerialNumbers[g_CurrentMediaSession], DRM_ID_SIZE );
    MEMCPY( &g_MediaSessionID, &g_MediaSessionIDs[g_CurrentMediaSession], DRM_ID_SIZE );

ErrorExit:

    return dr;
}

/**
 *  argv[0] = string determining whether this will be a REGISTRATION or CONTENT session
 */
DRM_RESULT DRM_CALL Test_NDTMGR_CreateSession(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT       dr  = DRM_SUCCESS;
    DRM_ID          *pId = NULL;
    DRM_APP_CONTEXT *pAppContext = NULL;
    DRM_BOOL         fAppContextInitialized = FALSE;

    ChkArg(argc == 1 && argv[0]);

    ChkMem( pId = (DRM_ID *)Oem_MemAlloc(DRM_ID_SIZE) );
    if ( 0 == strcmp(argv[0], "REGISTRATION") )
    {
        ChkDR( Drm_Ndt_Session_Create( NULL,
                                       g_pNdtDrmContext,
                                       NDTMGR_SESSION_TYPE_REGISTRATION,
                                       NULL,
                                       &g_RegistrationSessionID ) );
        MEMCPY( pId, &g_RegistrationSessionID, DRM_ID_SIZE );
    }
    else if ( 0 == strcmp(argv[0], "CONTENT") )
    {
        ChkDR( TestCreateAppContext( (DRM_CONST_STRING *)&g_pCurNdtContext->dstrInitData,
                                     &pAppContext ));

        fAppContextInitialized = TRUE;

        ChkDR( Drm_Ndt_Session_Create( NULL,
                                       g_pNdtDrmContext,
                                       NDTMGR_SESSION_TYPE_CONTENT,
                                       pAppContext,
                                       &g_MediaSessionID ) );

        MEMCPY( pId, &g_MediaSessionID, DRM_ID_SIZE );
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    ChkDR( TestAddCleanupEntry( NdcNdtSession, (DRM_VOID *)pId ) );
    pId = NULL;
ErrorExit:

    if ( DRM_FAILED(dr) && fAppContextInitialized  )
    {
        TestDeleteAppContext( pAppContext, FALSE );
    }

    SAFE_OEM_FREE( pId );
    return dr;
}

/**
 *  argv[0] = string determining whether this will be a REGISTRATION or CONTENT session
 */
DRM_RESULT DRM_CALL Test_NDTMGR_CloseSession(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_APP_CONTEXT *pAppContext = NULL;

    ChkArg(argc == 1 && argv[0]);

    if ( 0 == strcmp(argv[0], "REGISTRATION") )
    {
        ChkDR(Drm_Ndt_Session_Close(g_pNdtDrmContext,
                                    g_RegistrationSessionID,
                                    NULL));
    }
    else if ( 0 == strcmp(argv[0], "CONTENT") )
    {
        ChkDR(Drm_Ndt_Session_Close(g_pNdtDrmContext,
                                    g_MediaSessionID,
                                    &pAppContext));

        if ( pAppContext != NULL )
        {
            TestDeleteAppContext( pAppContext, FALSE );
        }
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

ErrorExit:
    return dr;
}

/**
 *  No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessRegistrationRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr                         = DRM_SUCCESS;
    DRM_BYTEBLOB RegistrationRequestMsg     = { 0 };
    DRM_BYTE     *pbRegistrationResponseMsg = NULL;
    DRM_ID       SerialNumber               = { 0 };
    DRM_BYTE     *pAddress                  = NULL;
    DRM_WORD     cbAddress                  = 0;
    DRM_BYTE     *pbRegistrationRequestMsg  = NULL;

    DRM_CRYPTO_CONTEXT *pCryptoCtx          = NULL;

    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    RegistrationRequestMsg.cbBlob = g_cbRegistrationRequestMsg;

    /* create the Request message to send in the data block */
    ChkDR(WmdrmNetPackRegistrationMessage(&g_SerialNumber,
                                          g_pCertificate,
                                          g_cbCertificate,
                                          &(RegistrationRequestMsg.pbBlob),
                                          &(RegistrationRequestMsg.cbBlob)));
    pbRegistrationRequestMsg = RegistrationRequestMsg.pbBlob;

    /* request */
    if ( g_pbRegistrationRequestMsgNULL )
    {
        RegistrationRequestMsg.pbBlob = NULL;
    }
    else
    {
        if (  g_MessageProtocolVersionOverride )
        {
            ChkBOOL( RegistrationRequestMsg.cbBlob > 0, DRM_E_BUFFERTOOSMALL );
            PUT_BYTE( RegistrationRequestMsg.pbBlob, 0,  g_MessageProtocolVersionOverride);
        }

        if ( g_MessageTypeOverride )
        {
            ChkBOOL( RegistrationRequestMsg.cbBlob > 1, DRM_E_BUFFERTOOSMALL );
            PUT_BYTE( RegistrationRequestMsg.pbBlob, 1, g_MessageTypeOverride);
        }
    }

    /*
    ** Optionally override the request message length only after the above optional adjustments
    */
    if ( g_cbRegistrationRequestMsgOverride )
    {
        g_cbRegistrationRequestMsg = g_cbRegistrationRequestMsgOverride;
        RegistrationRequestMsg.cbBlob = g_cbRegistrationRequestMsgOverride;
    }


    /* response */
    if ( g_cbRegistrationResponseMsgOverride )
    {
        g_cbRegistrationResponseMsg = g_cbRegistrationResponseMsgOverride;
    }

    if ( 0 == g_pbRegistrationResponseMsgNULL )
    {
        pbRegistrationResponseMsg = (DRM_BYTE *) Oem_MemAlloc(g_cbRegistrationResponseMsg);
        ChkMem(pbRegistrationResponseMsg);
    }

    ChkDR(Drm_Ndt_Registration_ProcessRequest( NULL,
                                               g_pNdtDrmContext,
                                               g_RegistrationSessionID,
                                               RegistrationRequestMsg,
                                               pbRegistrationResponseMsg,
                                               &g_cbRegistrationResponseMsg ) );

    ChkDR(WmdrmNetUnpackRegistrationResponseMessage(&g_PrivateKey,
                                                    pbRegistrationResponseMsg,
                                                    g_cbRegistrationResponseMsg,
                                                    &g_Seed,
                                                    &SerialNumber,
                                                    &g_RegistrationSessionID,
                                                    &pAddress,
                                                    &cbAddress,
                                                    pCryptoCtx));

    ChkArg(MEMCMP(&SerialNumber, &g_SerialNumber, 16) == 0);

ErrorExit:
    SAFE_OEM_FREE( pbRegistrationRequestMsg );
    SAFE_OEM_FREE( pbRegistrationResponseMsg );
    SAFE_OEM_FREE( pAddress );
    SAFE_OEM_FREE( pCryptoCtx );

    return dr;
}

/**
 * This function does the GetProximityChallenge, ProcessProximityResponse in one fell
 * swoop, in case the finer grain functions Test_NDTMGR_GetProximityChallenge and
 * Test_NDTMGR_GetProximityResponse are too slow to meet the 7ms deadline for
 * Proximity.
 *
 * argv[0] = sleepage
 */
DRM_RESULT DRM_CALL Test_Proximity(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT          dr                        = DRM_SUCCESS;
    DRM_BYTEBLOB        ProximityStartMessage     = { 0 };
    DRM_BYTEBLOB        ProximityChallengeMessage = { 0 };
    DRM_BYTEBLOB        ProximityResponseMessage  = { 0 };
    DRM_BYTEBLOB        ProximityResultMessage    = { 0 };

    DRM_ID              ProximityResultSessionID  = { 0 };
    DRM_WORD            ProximityResult           = 0;

    DRM_AES_KEY         *pKey                      = NULL;
    DRM_DWORD           sleepage                   = 0;

    DRM_TEST_OEM_CONTEXT  oTestOemContext = {0};
    DRM_TEST_OEM_CONTEXT *poTestOemContext = &oTestOemContext;

    ChkArg(argc == 1 && argv[0]);

    sleepage = strtoul(argv[0], NULL, 0);

    /*
    ** A sleepage duration of 0ms means that we want
    ** proximity detection to not fail due to duration.  However, this can
    ** happen if the OS's scheduler has a scheduling interval > 7ms (which is
    ** most likely the case) and swaps us out in the middle of this routine.
    ** This means that in order to enable automated nightly testing, this
    ** function needs to reliably not fail due to duration (when we expect it to).
    */
    if ( sleepage == 0 )
    {
        /*
        ** Get the current clock ticks and indicate that we don't want it to change
        */
        poTestOemContext->dwTickCount      = Oem_Clock_GetTickCount( poTestOemContext );
        poTestOemContext->fFreezeTickCount = TRUE;
    }

    /* initialize the DRM_BYTEBLOB buffers */
    ProximityStartMessage.pbBlob = NULL;
    ProximityStartMessage.cbBlob = 0;
    ProximityChallengeMessage.pbBlob = NULL;
    ProximityChallengeMessage.cbBlob = 0;
    ProximityResponseMessage.pbBlob = NULL;
    ProximityResponseMessage.cbBlob = 0;
    ProximityResultMessage.pbBlob = NULL;
    ProximityResultMessage.cbBlob = 0;

    /* get a registration key */
    pKey = WmdrmNetGetRegistrationKey(&g_Seed, FALSE);

    /* pack start message */
    if (!g_fProximityStartMsgNULL)
    {
        ChkDR(WmdrmNetPackProximityStartMessage(&g_RegistrationSessionID,
                                                &(ProximityStartMessage.pbBlob),
                                                &(ProximityStartMessage.cbBlob)));
    }

    /* call ProcessProximityStartMsg */
    dr = Drm_Ndt_Proximity_ProcessStartMsg( poTestOemContext,
                                            g_pNdtDrmContext,
                                            ProximityStartMessage,
                                            ProximityChallengeMessage.pbBlob,
                                            &(ProximityChallengeMessage.cbBlob ) );
    if ( g_fTestProximityBUFFERTOOSMALL )
    {
        ChkDR(dr);
    }
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        /* allocate challenge message buffer */
        ProximityChallengeMessage.pbBlob = (DRM_BYTE *) Oem_MemAlloc(ProximityChallengeMessage.cbBlob);
        ChkMem(ProximityChallengeMessage.pbBlob);
        dr = Drm_Ndt_Proximity_ProcessStartMsg( poTestOemContext,
                                                g_pNdtDrmContext,
                                                ProximityStartMessage,
                                                ProximityChallengeMessage.pbBlob,
                                                &(ProximityChallengeMessage.cbBlob ) );
    }
    ChkDR(dr);

    /* unpack the challenge message */
    dr = WmdrmNetUnpackProximityChallengeMessage(ProximityChallengeMessage.pbBlob,
                                                 ProximityChallengeMessage.cbBlob,
                                                 &g_bChallengeSequenceNumber,
                                                 &g_RegistrationSessionID,
                                                 &g_idChallenge);

    if ( DRM_E_ND_INVALID_MESSAGE_TYPE == dr )
    {
        /* check if the proximity result message came back instead of the challenge */
        ChkDR(WmdrmNetUnpackProximityResultMessage(ProximityChallengeMessage.pbBlob,
                                                   ProximityChallengeMessage.cbBlob,
                                                  &ProximityResultSessionID,
                                                  &ProximityResult));

        /* check that the Session ID is correct (the same) */
        if ( memcmp( &g_RegistrationSessionID, &ProximityResultSessionID, sizeof(g_RegistrationSessionID) ) != 0) {
            ChkDR(DRM_E_FAIL);
        }

        /* check the result */
        ChkDR(MapProximityResultToDR((WMDRMND_RESULT) ProximityResult));

        goto ErrorExit;
    }

    if( g_ChallengeIDOverride )
    {
        g_idChallenge = g_idChallengeOverride;
    }

    if( sleepage > 0 )
    {
        DRM_DWORD start;
        DRM_DWORD elapsed;

        start = Oem_Clock_GetTickCount( NULL );
        elapsed = start;
        while ( start + sleepage > elapsed )
        {
            elapsed = Oem_Clock_GetTickCount( NULL );
        }
    }

    /* encrypt the challenge message and pack proximity response msg */
    if (!g_fProximityResponseMsgNULL)
    {
        ChkDR(WmdrmNetPackProximityResponseMessage(pKey,
                                                   g_bChallengeSequenceNumber,
                                                   &g_RegistrationSessionID,
                                                   &g_idChallenge,
                                                   &(ProximityResponseMessage.pbBlob),
                                                   &(ProximityResponseMessage.cbBlob)));
    }

    /* Drm_Ndt_Proximity_ProcessResponseMsg */
    dr = Drm_Ndt_Proximity_ProcessResponseMsg( poTestOemContext,
                                               g_pNdtDrmContext,
                                               ProximityResponseMessage,
                                               ProximityResultMessage.pbBlob,
                                               &(ProximityResultMessage.cbBlob ) );
    if ( g_fTestProximityBUFFERTOOSMALL )
    {
        ChkDR(dr);
    }
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        /* allocate challenge message buffer */
        ProximityResultMessage.pbBlob = (DRM_BYTE *) Oem_MemAlloc(ProximityResultMessage.cbBlob);
        ChkMem(ProximityResultMessage.pbBlob);
        dr = Drm_Ndt_Proximity_ProcessResponseMsg( poTestOemContext,
                                                   g_pNdtDrmContext,
                                                   ProximityResponseMessage,
                                                   ProximityResultMessage.pbBlob,
                                                   &(ProximityResultMessage.cbBlob));
    }
    ChkDR(dr);

    /* unpack result */
    ChkDR(WmdrmNetUnpackProximityResultMessage(ProximityResultMessage.pbBlob,
                                               ProximityResultMessage.cbBlob,
                                               &ProximityResultSessionID,
                                               &ProximityResult));

    /* check that the Session ID is correct (the same) */
    if ( memcmp( &g_RegistrationSessionID, &ProximityResultSessionID, sizeof(g_RegistrationSessionID) ) != 0) {
        ChkDR(DRM_E_FAIL);
    }

    /* check the result */
    ChkDR(MapProximityResultToDR((WMDRMND_RESULT) ProximityResult));

ErrorExit:
    if ( pKey )
    {
        Oem_Aes_ZeroKey(pKey);
    }

    SAFE_OEM_FREE( ProximityStartMessage.pbBlob );
    SAFE_OEM_FREE( ProximityChallengeMessage.pbBlob );
    SAFE_OEM_FREE( ProximityResponseMessage.pbBlob );
    SAFE_OEM_FREE( ProximityResultMessage.pbBlob );

    return dr;
}

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessProximityStartMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr                        = DRM_SUCCESS;

    DRM_BYTEBLOB ProximityStartMessage     = { 0 };
    DRM_BYTEBLOB ProximityChallengeMessage = { 0 };

    /* initialize the DRM_BYTEBLOB buffers */
    ProximityStartMessage.pbBlob = NULL;
    ProximityStartMessage.cbBlob = 0;
    ProximityChallengeMessage.pbBlob = NULL;
    ProximityChallengeMessage.cbBlob = 0;

    /* pack start message */
    if (!g_fProximityStartMsgNULL)
    {
        ChkDR(WmdrmNetPackProximityStartMessage(&g_RegistrationSessionID,
                                                &(ProximityStartMessage.pbBlob),
                                                &(ProximityStartMessage.cbBlob)));
    }

    /* call ProcessProximityStartMsg */
    dr = Drm_Ndt_Proximity_ProcessStartMsg( NULL,
                                            g_pNdtDrmContext,
                                            ProximityStartMessage,
                                            ProximityChallengeMessage.pbBlob,
                                            &(ProximityChallengeMessage.cbBlob ) );
    if ( g_fTestProximityBUFFERTOOSMALL )
    {
        ChkDR(dr);
    }
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        /* allocate challenge message buffer */
        ProximityChallengeMessage.pbBlob = (DRM_BYTE *) Oem_MemAlloc(ProximityChallengeMessage.cbBlob);
        ChkMem(ProximityChallengeMessage.pbBlob);
        dr = Drm_Ndt_Proximity_ProcessStartMsg( NULL,
                                                g_pNdtDrmContext,
                                                ProximityStartMessage,
                                                ProximityChallengeMessage.pbBlob,
                                                &(ProximityChallengeMessage.cbBlob ) );
    }
    ChkDR(dr);

    /* unpack the challenge message */
    ChkDR(WmdrmNetUnpackProximityChallengeMessage(ProximityChallengeMessage.pbBlob,
                                                  ProximityChallengeMessage.cbBlob,
                                                  &g_bChallengeSequenceNumber,
                                                  &g_RegistrationSessionID,
                                                  &g_idChallenge));

ErrorExit:
    SAFE_OEM_FREE( ProximityStartMessage.pbBlob );
    SAFE_OEM_FREE( ProximityChallengeMessage.pbBlob );

    return dr;
}

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessProximityResponseMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr                      = DRM_SUCCESS;
    DRM_AES_KEY  *pKey                    = NULL;

    DRM_BYTEBLOB ProximityResponseMessage = { 0 };
    DRM_BYTEBLOB ProximityResultMessage   = { 0 };

    DRM_ID       ProximityResultSessionID = { 0 };
    DRM_WORD     ProximityResult          = 0;

    /* initialize the DRM_BYTEBLOB buffers */
    ProximityResponseMessage.pbBlob = NULL;
    ProximityResponseMessage.cbBlob = 0;
    ProximityResultMessage.pbBlob = NULL;
    ProximityResultMessage.cbBlob = 0;

    /* get a registration key */
    pKey = WmdrmNetGetRegistrationKey(&g_Seed, FALSE);

    /* encrypt the challenge message and pack proximity response msg */
    if (!g_fProximityResponseMsgNULL)
    {
        ChkDR(WmdrmNetPackProximityResponseMessage(pKey,
                                                   g_bChallengeSequenceNumber,
                                                   &g_RegistrationSessionID,
                                                   &g_idChallenge,
                                                   &(ProximityResponseMessage.pbBlob),
                                                   &(ProximityResponseMessage.cbBlob)));
    }

    /* NDTMGR_ProcessProximityResponseMsg */
    dr = Drm_Ndt_Proximity_ProcessResponseMsg( NULL,
                                               g_pNdtDrmContext,
                                               ProximityResponseMessage,
                                               ProximityResultMessage.pbBlob,
                                               &(ProximityResultMessage.cbBlob));
    if ( g_fTestProximityBUFFERTOOSMALL )
    {
        ChkDR(dr);
    }
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        /* allocate challenge message buffer */
        ProximityResultMessage.pbBlob = (DRM_BYTE *) Oem_MemAlloc(ProximityResultMessage.cbBlob);
        ChkMem(ProximityResultMessage.pbBlob);
        dr = Drm_Ndt_Proximity_ProcessResponseMsg( NULL,
                                                   g_pNdtDrmContext,
                                                   ProximityResponseMessage,
                                                   ProximityResultMessage.pbBlob,
                                                   &(ProximityResultMessage.cbBlob));
    }
    ChkDR(dr);

    /* unpack result */
    ChkDR(WmdrmNetUnpackProximityResultMessage(ProximityResultMessage.pbBlob,
                                               ProximityResultMessage.cbBlob,
                                               &ProximityResultSessionID,
                                               &ProximityResult));

    /* check that the Session ID is correct (the same) */
    if ( memcmp( &g_RegistrationSessionID, &ProximityResultSessionID, sizeof(g_RegistrationSessionID) ) != 0) {
        ChkDR(DRM_E_FAIL);
    }

    /* check the result */
    ChkDR(MapProximityResultToDR((WMDRMND_RESULT) ProximityResult));

ErrorExit:
    if ( pKey )
    {
        Oem_Aes_ZeroKey(pKey);
    }

    SAFE_OEM_FREE( ProximityResponseMessage.pbBlob );
    SAFE_OEM_FREE( ProximityResultMessage.pbBlob );

    return dr;
}

/**
 * argv[0] - rights id
 * argv[1] - action
 * argv[2] - license mode
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessLicenseChallenge(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                      dr                    = DRM_SUCCESS;
    DRM_BYTE                       *pAction               = NULL;
    DRM_BYTE                       *pbLicenseResponseMsg  = NULL;
    DRM_DWORD                      *pcbLicenseResponseMsg = NULL;
    DRM_DWORD                       cbLicenseResponseMsg  = 0;
    DRM_BYTE                       *pbLicense             = NULL;
    DRM_DWORD                       cbLicense             = 0;
    NDT_LICENSE_MODE                eLicenseMode          = NDT_LICENSE_MODE_SIMPLE;
    DRM_KID                        *pKID                  = NULL;
    WMDRMNET_POLICY_TRANSCRYPTPLAY *pPolicyTranscryptPlay = NULL;
    DRM_DWORD                       i                     = 0;

    /* Parse Command Line Args
     */
    ChkArg( argc == 3
            && argv[0]
            && argv[1]
            && argv[2] );

    ParseDRMID(&g_drmRightsID, argv[0]);
    if ( g_cbActionOverride )
    {
        g_cbAction = g_cbActionOverride;
    }
    else
    {
        g_cbAction = (DRM_WORD) strlen(argv[1]);
    }
#ifdef __TMS320C55X__
    pAction = (DRM_BYTE *) Oem_MemAlloc(g_cbAction + 1);
    for ( i = 0; i < g_cbAction; i++)
    {
        PUT_BYTE(pAction, i, argv[1][i]);
    }
#else
    pAction = (DRM_BYTE *) argv[1];
#endif

    if ( 0 == strcmp(argv[2], "SIMPLE") )
    {
        eLicenseMode = NDT_LICENSE_MODE_SIMPLE;
    }
    else if ( 0 == strcmp(argv[2], "CHAINED") )
    {
        eLicenseMode =  NDT_LICENSE_MODE_CHAINED;
    }
    else if ( 0 == strcmp(argv[2], "INVALID_MODE") )
    {
        eLicenseMode =  (NDT_LICENSE_MODE)9999;
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    /* License Request
     */
    ChkDR( WmdrmNetPackLicenseRequestMessage(&g_drmRightsID,
                                             &g_SerialNumber,
                                             g_pCertificate,
                                             g_cbCertificate,
                                             pAction,
                                             g_cbAction,
                                             &g_LicenseChallengeMsg.pbBlob,
                                             &g_LicenseChallengeMsg.cbBlob) );

    if ( g_cbLicenseChallengeMsgOverride )
    {
        g_LicenseChallengeMsg.cbBlob = g_cbLicenseChallengeMsgOverride;
    }

    if ( 0 == g_cbLicenseResponseMsgNULL )
    {
        pcbLicenseResponseMsg = &cbLicenseResponseMsg;
    }

    if (!g_fpKIDNULL)
    {
        pKID = &g_KID;
    }

    dr = Drm_Ndt_License_ProcessChallenge( NULL,
                                           g_pNdtDrmContext,
                                           g_MediaSessionID,
                                           g_LicenseChallengeMsg,
                                           eLicenseMode,
                                           (eLicenseMode == NDT_LICENSE_MODE_CHAINED ? NULL : g_pWMDRMNETPolicy),
                                           pKID,
                                           pbLicenseResponseMsg,
                                           pcbLicenseResponseMsg);

    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        if ( g_cbLicenseResponseMsgOverride != -1 )
        {
            *pcbLicenseResponseMsg = g_cbLicenseResponseMsgOverride;
        }
        if ( 0 == g_pbLicenseResponseMsgNULL )
        {
            pbLicenseResponseMsg = (DRM_BYTE *) Oem_MemAlloc(*pcbLicenseResponseMsg);
            ChkMem(pbLicenseResponseMsg);
        }

        dr = Drm_Ndt_License_ProcessChallenge( NULL,
                                               g_pNdtDrmContext,
                                               g_MediaSessionID,
                                               g_LicenseChallengeMsg,
                                               eLicenseMode,
                                               (eLicenseMode == NDT_LICENSE_MODE_CHAINED ? NULL : g_pWMDRMNETPolicy),
                                               pKID,
                                               pbLicenseResponseMsg,
                                               pcbLicenseResponseMsg);
    }
    ChkDR(dr);

    ChkDR(WmdrmNetUnpackLicenseResponseMessage(pbLicenseResponseMsg, *pcbLicenseResponseMsg, &pbLicense, &cbLicense));

    _FreeLicenses();

    if ( NDT_LICENSE_MODE_SIMPLE == eLicenseMode )
    {
        ChkDR(DrmXmrUnpackLicense(&g_PrivateKey, pbLicense, cbLicense, &g_pXmrLicense));
    }
    else if ( NDT_LICENSE_MODE_CHAINED == eLicenseMode )
    {
        ChkDR(DrmXmrUnpackRootLicense(&g_PrivateKey, pbLicense, cbLicense, &g_RootXmrLicenseInfo));
        g_pXmrLicense = g_RootXmrLicenseInfo.pXmrLicense;
    }

    if ( g_pContentEncryptionKey != g_pRootContentEncryptionKey && NULL != g_pContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
    }
    if ( NULL != g_pRootContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pRootContentEncryptionKey);
        g_pRootContentEncryptionKey = NULL;
    }
    g_pContentEncryptionKey = NULL;
    ChkDR(DrmXmrGetKeyFromLicense(&g_PrivateKey, g_pXmrLicense, TRUE, &g_pRootContentEncryptionKey));
    g_pContentEncryptionKey = g_pRootContentEncryptionKey;

    /* Check Device ID */
    ChkArg((MEMCMP(&g_pXmrLicense->gr.SerialNumber, &g_SerialNumber, DRM_ID_SIZE) == 0));

    /* Check the integrity of the policy */
    if(NDT_LICENSE_MODE_SIMPLE == eLicenseMode)
    {
        pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *) g_pWMDRMNETPolicy->pbPolicy;

        /* Check License Min App Security */
        ChkArg( g_pXmrLicense->gr.wMinAppSecurityLevel == pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel );

        /* Check License App Revocation List Version */
        ChkArg( g_pXmrLicense->gr.dwReserved1 == pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumAppRevocationListVersion );

        /* Check License Device Revocation List Version */
        ChkArg( g_pXmrLicense->gr.dwReserved2 == pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumDeviceRevocationListVersion );

        /* Check Source ID */
        ChkArg( g_pXmrLicense->gr.dwSourceId == pPolicyTranscryptPlay->globals.dwSourceID);

        /* Check Inclustion Lists */
        ChkArg( g_pXmrLicense->gr.il.cGUIDS == pPolicyTranscryptPlay->globals.InclusionList.cGUIDs );
        if (g_pXmrLicense->gr.il.cGUIDS > 0)
        {
            ChkArg((MEMCMP( g_pXmrLicense->gr.il.pGUIDS,  pPolicyTranscryptPlay->globals.InclusionList.pguidBuffer, (g_pXmrLicense->gr.il.cGUIDS * SIZEOF( DRM_GUID ))) == 0));
        }

        /* Check Minimum OPLs */
        /* wCompressedDigitalVideo */
        ChkArg( g_pXmrLicense->mopl.wReserved1 == pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalVideo );
        /* wUncompressedDigitalVideo */
        ChkArg( g_pXmrLicense->mopl.wUncompressedDigitalVideo == pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalVideo );
        /* wAnalogVideo */
        ChkArg( g_pXmrLicense->mopl.wAnalogVideo == pPolicyTranscryptPlay->playOPLs.minOPL.wAnalogVideo );
        /* wCompressedDigitalAudio */
        ChkArg( g_pXmrLicense->mopl.wReserved2 == pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalAudio );
        /* wUncompressedDigitalAudio */
        ChkArg( g_pXmrLicense->mopl.wUncompressedDigitalAudio == pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalAudio );

        /* Check OPL Output IDs */
        ChkArg( g_pXmrLicense->vopi.cEntries == pPolicyTranscryptPlay->playOPLs.vopi.cEntries );
        for (i = 0; i < g_pXmrLicense->vopi.cEntries; i++)
        {
            ChkArg((MEMCMP( &g_pXmrLicense->vopi.rgVop[i].guidId,  &pPolicyTranscryptPlay->playOPLs.vopi.rgVop[i].guidId, SIZEOF( DRM_GUID )) == 0));
            /* Note: bConfigData is noted to be limited to one byte of extended data in drmoutputlevels.c */
            ChkArg( g_pXmrLicense->vopi.rgVop[i].cbConfigData == 1 );
            ChkArg( *(g_pXmrLicense->vopi.rgVop[i].pbConfigData) == pPolicyTranscryptPlay->playOPLs.vopi.rgVop[i].bConfigData );
        }
    }

    MEMCPY( &g_xmrMinEnvironment , &g_pXmrLicense->gr , SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );

ErrorExit:
    SAFE_OEM_FREE(g_LicenseChallengeMsg.pbBlob);
    SAFE_OEM_FREE(pbLicenseResponseMsg);
    SAFE_OEM_FREE(pbLicense);

    if ( DRM_FAILED(dr)
         && g_pXmrLicense != NULL )
    {
        _FreeLicenses();
    }

#ifdef __TMS320C55X__
    SAFE_OEM_FREE(pAction);
#endif

    return dr;
}

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_GetLeafLicenseResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr                     = DRM_SUCCESS;
    DRM_KID      *pKID                   = NULL;
    DRM_KID       KID                    ={ 0 };
    DRM_BYTE     *pbLeafLicenseResponse  = NULL;
    DRM_DWORD    *pcbLeafLicenseResponse = NULL;
    DRM_DWORD     cbLeafLicenseResponse  = 0;
    DRM_BYTE     *pbLeafLicense          = NULL;
    DRM_DWORD     cbLeafLicense          = 0;

    if (!g_fpKIDNULL)
    {
        pKID = &KID;
    }
    if (!g_fpcbLeafLicenseResponseNULL)
    {
        pcbLeafLicenseResponse = &cbLeafLicenseResponse;
    }

    dr = Drm_Ndt_License_GenerateLeafResponse( NULL,
                                               g_pNdtDrmContext,
                                               g_MediaSessionID,
                                               g_pWMDRMNETPolicy,
                                               pKID,
                                               pbLeafLicenseResponse,
                                               pcbLeafLicenseResponse);

    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        if ( g_cbLeafLicenseResponseOverride != -1 )
        {
            *pcbLeafLicenseResponse = g_cbLeafLicenseResponseOverride;
        }
        if ( !g_fpbLeafLicenseResponseNULL )
        {
            pbLeafLicenseResponse = (DRM_BYTE *) Oem_MemAlloc(*pcbLeafLicenseResponse);
            ChkMem(pbLeafLicenseResponse);
        }

        dr = Drm_Ndt_License_GenerateLeafResponse( NULL,
                                                   g_pNdtDrmContext,
                                                   g_MediaSessionID,
                                                   g_pWMDRMNETPolicy,
                                                   pKID,
                                                   pbLeafLicenseResponse,
                                                   pcbLeafLicenseResponse);
    }
    ChkDR(dr);

    /* Unpack the license response.  Doing so verifies the response message. */
    ChkDR(WmdrmNetUnpackLicenseResponseMessage(pbLeafLicenseResponse, *pcbLeafLicenseResponse, &pbLeafLicense, &cbLeafLicense));

    if ( g_pXmrLicense != g_RootXmrLicenseInfo.pXmrLicense )
    {
        DrmXmrFreeLicense(g_pXmrLicense);
        g_pXmrLicense = NULL;
    }
    ChkDR(DrmXmrUnpackLeafLicense(g_RootXmrLicenseInfo, pbLeafLicense, cbLeafLicense, &g_pXmrLicense));

    if ( g_pRootContentEncryptionKey != g_pContentEncryptionKey && NULL != g_pContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
        g_pContentEncryptionKey = NULL;
    }
    ChkDR(DrmXmrGetKeyFromLicenseEx(FALSE, (DRM_BYTE *) g_pRootContentEncryptionKey, g_pXmrLicense, TRUE, &g_pContentEncryptionKey));

    /* Uplink KID */
    ChkArg((MEMCMP(&g_pXmrLicense->km.uk.idKID, &g_KID, DRM_ID_SIZE) == 0));

    MEMCPY( &g_xmrMinEnvironment , &g_pXmrLicense->gr , SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );

ErrorExit:
    SAFE_OEM_FREE(pbLeafLicenseResponse);
    SAFE_OEM_FREE(pbLeafLicense);

    return dr;
}

/* ndtencrypt.h */

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetCryptoDataNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_CryptoDataNULL = 1;

    return dr;
}

/**
 * argv[0] - encryption mode
 */
DRM_RESULT DRM_CALL Test_SetEncryptionMode(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 && argv[0] );
    g_EncryptionMode = strtoul( argv[0], NULL, 0 );


ErrorExit:
    return dr;
}

/**
 * argv[0]  = IN  char         filename of file to encrypt
 * argv[1]  = IN  DRM_UINT64   sample ID
 * argv[2]  = IN  DRM_UINT64   offset
 */
DRM_RESULT DRM_CALL Test_NDTMGR_Encrypt_ASF(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT           dr               = DRM_SUCCESS;
    DRM_BYTEBLOB         SampleData       = {0};
    DRM_UINT64           qwSampleID       = DRM_UI64((DRM_DWORD) 0);
    DRM_UINT64           qwOffset         = DRM_UI64((DRM_DWORD) 0);
    NDT_CRYPTO_DATA_ASF  ndtCryptoDataAsf = { 0 };
    DRM_BYTE            *CryptoData       = NULL;
    NDT_ENCRYPTION_MODE  EncryptionMode   = NDT_ENCRYPTION_MODE_ASF;

    ChkArg( argc == 3 && argv[0] && argv[1] && argv[2] );
    ChkDR ( OpenFileIntoBuffer( argv[0],
                                &SampleData.pbBlob,
                                &SampleData.cbBlob, 0 ) );
    ChkDR( ParseDRMUINT64( &qwSampleID, argv[1] ) );
    ChkDR( ParseDRMUINT64( &qwOffset, argv[2] ) );

    ndtCryptoDataAsf.SampleData = SampleData;
    ndtCryptoDataAsf.qwSampleID = qwSampleID;
    ndtCryptoDataAsf.qwOffset = qwOffset;

    if(!g_CryptoDataNULL)
    {
        CryptoData = (DRM_BYTE *) &ndtCryptoDataAsf;
    }
    if(g_EncryptionMode)
    {
        EncryptionMode = (NDT_ENCRYPTION_MODE)g_EncryptionMode;
    }
    ChkDR( NDTMGR_Encrypt( g_pNdtDrmContext,
                           g_MediaSessionID,
                           EncryptionMode,
                           CryptoData ) );

ErrorExit:
    SAFE_OEM_FREE( SampleData.pbBlob );
    return dr;
}

/**
 * argv[0]  = IN  char         filename of file to encrypt
 * argv[1]  = IN  DRM_UINT64   sample ID
 * argv[2]  = IN  DRM_UINT64   offset
 */
DRM_RESULT DRM_CALL Test_NDTMGR_Encrypt_MPEG(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT           dr               = DRM_SUCCESS;
    DRM_BYTEBLOB         SampleData       = {0};
    DRM_UINT64           qwSampleID       = DRM_UI64((DRM_DWORD) 0);
    DRM_UINT64           qwOffset         = DRM_UI64((DRM_DWORD) 0);
    NDT_CRYPTO_DATA_ASF  ndtCryptoDataAsf = { 0 };
    DRM_BYTE            *CryptoData       = NULL;
    NDT_ENCRYPTION_MODE  EncryptionMode   = NDT_ENCRYPTION_MODE_MPEG;

    ChkArg( argc == 3 && argv[0] && argv[1] && argv[2] );
    ChkDR ( OpenFileIntoBuffer( argv[0],
                                &SampleData.pbBlob,
                                &SampleData.cbBlob, 0 ) );
    ChkDR( ParseDRMUINT64( &qwSampleID, argv[1] ) );
    ChkDR( ParseDRMUINT64( &qwOffset, argv[2] ) );

    ndtCryptoDataAsf.SampleData = SampleData;
    ndtCryptoDataAsf.qwSampleID = qwSampleID;
    ndtCryptoDataAsf.qwOffset = qwOffset;

    if(!g_CryptoDataNULL)
    {
        CryptoData = (DRM_BYTE *) &ndtCryptoDataAsf;
    }
    if(g_EncryptionMode)
    {
        EncryptionMode = (NDT_ENCRYPTION_MODE)g_EncryptionMode;
    }
    ChkDR( NDTMGR_Encrypt( g_pNdtDrmContext,
                           g_MediaSessionID,
                           EncryptionMode,
                           CryptoData ) );

ErrorExit:
    SAFE_OEM_FREE( SampleData.pbBlob );
    return dr;
}

/* drmndtasf.h */

/**
 * argv[0]  = IN  char           *f_pszInputFileName
 * argv[1]  = IN  DRM_DWORD       f_dwASFDeliveryMode
 * argv[2]  = IN  char            f_pszOutputFileName
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_Init(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT             dr                          = DRM_SUCCESS;
    DRM_WCHAR              wszInputFileName[MAX_PATH]  = { 0 };
    DRM_WCHAR              wszOutputFileName[MAX_PATH] = { 0 };
    DRM_SUBSTRING          dsubstr                     = EMPTY_DRM_SUBSTRING;
    DRM_STRING             dstr                        = EMPTY_DRM_STRING;
    DRM_STRING             dstrInputFileName           = EMPTY_DRM_STRING;
    DRM_ID                *pMediaSessionID             = NULL;
    DRM_DWORD             *pdwASFFlags                 = NULL;
    DRM_APP_CONTEXT       *pAppContext                 = NULL;
    DRM_BOOL               fAppContextInitialized      = FALSE;
    DRM_BOOL               fAsfContextInitialized      = FALSE;
    DRM_BOOL               fClearToProtected           = FALSE;
#ifdef __TMS320C55X__
    int i;
#endif

    ChkArg(argc >= 3 && argv[0] && argv[1] && argv[2]);

    if ( 0 == strcmp(argv[1], "MUXED") )
    {
        g_eASFDeliveryMode = NDT_ASFDELIVERY_MODE_MUXED;
    }
    else if ( 0 == strcmp(argv[1], "SAMPLES") )
    {
        g_eASFDeliveryMode =  NDT_ASFDELIVERY_MODE_SAMPLES;
    }
    else if ( 0 == strcmp(argv[1], "INVALID_MODE") )
    {
        g_eASFDeliveryMode =  (NDT_ASFDELIVERY_MODE)9999;
    }
    else
    {
        ChkDR(DRM_E_INVALIDARG);
    }

    if (argc > 3 && argv[3] != NULL && strcmp(argv[3], "TRUE") == 0 )
    {
        fClearToProtected = TRUE;
    }

#ifdef __TMS320C55X__
    for(i = 0; i < strlen(argv[0]); i++)
    {
        PUT_BYTE(argv[0], i, argv[0][i]);
    }
    for(i = 0; i < strlen(argv[2]); i++)
    {
        PUT_BYTE(argv[2], i, argv[2][i]);
    }
#endif

    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[0]);
    dstr.pwszString = wszInputFileName;
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(argv[0], &dsubstr, &dstr);

    if ( dstr.cchString < MAX_PATH )
    {
        wszInputFileName[dstr.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL )
    }

    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[2]);
    dstr.pwszString = wszOutputFileName;
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(argv[2], &dsubstr, &dstr);

    if ( dstr.cchString < MAX_PATH )
    {
        wszOutputFileName[dstr.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL )
    }

    dstrInputFileName.pwszString = (DRM_WCHAR *) wszInputFileName;
    dstrInputFileName.cchString = (DRM_DWORD) wcslen( (wchar_t *)wszInputFileName);

    if (!g_fMediaSessionIDNULL)
    {
        pMediaSessionID = (DRM_ID *)Oem_MemAlloc( DRM_ID_SIZE );
    }

    if (!g_fASFFlagsNULL)
    {
        pdwASFFlags = &g_dwASFFlags;
    }

    ChkDR( TestCreateAppContext( (DRM_CONST_STRING *)&g_pCurNdtContext->dstrInitData,
                                 &pAppContext ));

    fAppContextInitialized = TRUE;

    ChkDR( Drm_Ndt_Asf_Init( NULL,
                             g_pNdtDrmContext,
                             dstrInputFileName,
                             g_eASFDeliveryMode,
                             pAppContext,
                             fClearToProtected,
                             pMediaSessionID,
                             pdwASFFlags ) );

    fAsfContextInitialized = TRUE;

    if(g_hOutputFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( g_hOutputFile );
    }
    g_hOutputFile = Oem_File_Open(NULL, wszOutputFileName, OEM_GENERIC_WRITE|OEM_GENERIC_READ, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
    if(g_hOutputFile == OEM_INVALID_HANDLE_VALUE)
    {
        TRACE(("Unable to open output file.\n"));
        ChkDR( DRM_E_FAIL );
    }

    MEMCPY( &g_MediaSessionID, pMediaSessionID, DRM_ID_SIZE );
    ChkDR( TestAddCleanupEntry( NdcNdtAsfSession, (DRM_VOID *)pMediaSessionID ) );
    g_FirstNextDataBlock = 1;

ErrorExit:
    if( DRM_FAILED(dr) )
    {
        if (fAsfContextInitialized)
        {
            Drm_Ndt_Asf_Close( g_pNdtDrmContext, *pMediaSessionID, NULL );
        }

        if (fAppContextInitialized)
        {
            TestDeleteAppContext( pAppContext, FALSE );
        }

        SAFE_OEM_FREE( pMediaSessionID );
    }

    return dr;
}

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_Close(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT       dr = DRM_SUCCESS;
    DRM_APP_CONTEXT *pAppContext = NULL;

    ChkDR( Drm_Ndt_Asf_Close( g_pNdtDrmContext,
                              g_MediaSessionID,
                              &pAppContext ) );

    if (g_hOutputFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( g_hOutputFile );
        g_hOutputFile = OEM_INVALID_HANDLE_VALUE;
    }

    if ( pAppContext != NULL )
    {
        TestDeleteAppContext( pAppContext, FALSE );
    }

ErrorExit:

    return dr;
}

/**
 * argv[0] - rights id
 * argv[1] - action
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_InitTranscryption(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr                    = DRM_SUCCESS;
    DRM_BYTE     *pAction               = NULL;
    DRM_BYTE     *pbLicenseResponseMsg  = NULL;
    DRM_DWORD    *pcbLicenseResponseMsg = NULL;
    DRM_DWORD     cbLicenseResponseMsg  = 0;
    DRM_BYTE     *pbLicense             = NULL;
    DRM_DWORD     cbLicense             = 0;

#ifdef __TMS320C55X__
    int i;
#endif

    /* Parse Command Line Args
     */
    ChkArg( argc >= 2
            && argv[0]
            && argv[1] );

    ParseDRMID(&g_drmRightsID, argv[0]);
    if ( g_cbActionOverride )
    {
        g_cbAction = g_cbActionOverride;
    }
    else
    {
        g_cbAction = (DRM_WORD) strlen(argv[1]);
    }
#ifdef __TMS320C55X__
    pAction = (DRM_BYTE *) Oem_MemAlloc(g_cbAction + 1);
    for ( i = 0; i < g_cbAction; i++)
    {
        PUT_BYTE(pAction, i, argv[1][i]);
    }
#else
    pAction = (DRM_BYTE *) argv[1];
#endif

    /* License Request
     */
    ChkDR( WmdrmNetPackLicenseRequestMessage(&g_drmRightsID,
                                             &g_SerialNumber,
                                             g_pCertificate,
                                             g_cbCertificate,
                                             pAction,
                                             g_cbAction,
                                             &g_LicenseChallengeMsg.pbBlob,
                                             &g_LicenseChallengeMsg.cbBlob) );

    if ( g_cbLicenseChallengeMsgOverride )
    {
        g_LicenseChallengeMsg.cbBlob = g_cbLicenseChallengeMsgOverride;
    }

    if ( 0 == g_cbLicenseResponseMsgNULL )
    {
        pcbLicenseResponseMsg = &cbLicenseResponseMsg;
    }

    dr = Drm_Ndt_Asf_InitTranscryption( NULL,
                                        g_pNdtDrmContext,
                                        g_MediaSessionID,
                                        g_LicenseChallengeMsg,
                                        &g_KID,
                                        pbLicenseResponseMsg,
                                        pcbLicenseResponseMsg);

    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        if ( g_cbLicenseResponseMsgOverride != -1 )
        {
            *pcbLicenseResponseMsg = g_cbLicenseResponseMsgOverride;
        }
        if ( 0 == g_pbLicenseResponseMsgNULL )
        {
            pbLicenseResponseMsg = (DRM_BYTE *) Oem_MemAlloc(*pcbLicenseResponseMsg);
            ChkMem(pbLicenseResponseMsg);
        }

        dr = Drm_Ndt_Asf_InitTranscryption( NULL,
                                            g_pNdtDrmContext,
                                            g_MediaSessionID,
                                            g_LicenseChallengeMsg,
                                            &g_KID,
                                            pbLicenseResponseMsg,
                                            pcbLicenseResponseMsg);
    }
    ChkDR(dr);

    if(argc > 2 && argv[2])
    {
        OEM_FILEHDL hOutFile;
        DRM_SUBSTRING dsubstr;
        DRM_STRING dstr;
        DRM_WCHAR wszOutfile[256];
        DRM_DWORD cbWritten;

#ifdef __TMS320C55X__
        for(i = 0; i < strlen(argv[2]) + 1; i++)
        {
            PUT_BYTE(argv[2], i, argv[2][i]);
        }

        dstr.pwszString = wszOutfile;
        dsubstr.m_ich = 0;
        dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[2]);
        dstr.cchString = dsubstr.m_cch;
        DRM_UTL_PromoteASCIItoUNICODE(argv[2], &dsubstr, &dstr);
#else
        dstr.pwszString = wszOutfile;
        dsubstr.m_ich = 0;
        dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[2]);
        dstr.cchString = dsubstr.m_cch;
        DRM_UTL_PromoteASCIItoUNICODE(argv[2], &dsubstr, &dstr);
#endif /* __TMS320C55X__ */
        if ( dstr.cchString < NO_OF( wszOutfile ) )
        {
            wszOutfile[dstr.cchString] = 0;
        }
        else
        {
            ChkDR( DRM_E_BUFFERTOOSMALL )
        }

        hOutFile = Oem_File_Open(NULL, wszOutfile, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
        if(hOutFile != OEM_INVALID_HANDLE_VALUE)
        {
            Oem_File_Write( hOutFile, pbLicenseResponseMsg, *pcbLicenseResponseMsg, &cbWritten );
            Oem_File_Close(hOutFile);
        }
    }

    ChkDR(WmdrmNetUnpackLicenseResponseMessage(pbLicenseResponseMsg, *pcbLicenseResponseMsg, &pbLicense, &cbLicense));

    _FreeLicenses();
    if ( NDT_ASFDELIVERY_MODE_MUXED == g_eASFDeliveryMode )
    {
        ChkDR(DrmXmrUnpackLicense(&g_PrivateKey, pbLicense, cbLicense, &g_pXmrLicense));
    }
    else if ( NDT_ASFDELIVERY_MODE_SAMPLES == g_eASFDeliveryMode )
    {
        ChkDR(DrmXmrUnpackRootLicense(&g_PrivateKey, pbLicense, cbLicense, &g_RootXmrLicenseInfo));
        g_pXmrLicense = g_RootXmrLicenseInfo.pXmrLicense;
    }

    if ( g_pContentEncryptionKey != g_pRootContentEncryptionKey && NULL != g_pContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
    }
    if ( NULL != g_pRootContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pRootContentEncryptionKey);
        g_pRootContentEncryptionKey = NULL;
    }
    g_pContentEncryptionKey = NULL;
    ChkDR(DrmXmrGetKeyFromLicense(&g_PrivateKey, g_pXmrLicense, TRUE, &g_pRootContentEncryptionKey));
    g_pContentEncryptionKey = g_pRootContentEncryptionKey;

    /* Device ID */
    ChkArg((MEMCMP(&g_pXmrLicense->gr.SerialNumber, &g_SerialNumber, DRM_ID_SIZE) == 0));

    MEMCPY( &g_xmrMinEnvironment , &g_pXmrLicense->gr , SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );

ErrorExit:
    SAFE_OEM_FREE(g_LicenseChallengeMsg.pbBlob);
    SAFE_OEM_FREE(pbLicenseResponseMsg);
    SAFE_OEM_FREE(pbLicense);

    if ( DRM_FAILED(dr) )
    {
        _FreeLicenses();
    }

#ifdef __TMS320C55X__
    SAFE_OEM_FREE(pAction);
#endif

    return dr;
}

/**
 * argv[0]  = IN  DRM_UINT64       f_qwTimeOffsetMilliseconds
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_Seek(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( argc == 1 );

    ChkDR( ParseDRMUINT64( &g_qwTimeOffsetHNS, argv[0] ) );

    /* Convert from milliseconds to hundreds of nanoseconds
     */
    g_qwTimeOffsetHNS = DRM_UI64Mul(g_qwTimeOffsetHNS, DRM_UI64(10000));

    ChkDR( Drm_Ndt_Asf_Seek( NULL, g_pNdtDrmContext, g_MediaSessionID, g_qwTimeOffsetHNS ) );

ErrorExit:

    return dr;
}

/* non ndtmanager and ndtasf API functions that are helpful for testing */

/**
 * argv[0] - packed registration response message file
 */
DRM_RESULT DRM_CALL Test_WmdrmNetUnpackRegistrationResponseMessage(long argc, __in_ecount(argc) char *argv[])
{
  DRM_RESULT  dr           = DRM_SUCCESS;
  DRM_BYTE   *pbData       = NULL;
  DRM_DWORD   pcbData      = 0;
  DRM_ID      SerialNumber = { 0 };
  DRM_BYTE   *pAddress     = NULL;
  DRM_WORD    cbAddress    = 0;
  DRM_CRYPTO_CONTEXT *pCryptoCtx = NULL;

  ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
  ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

  ChkDR(OpenFileIntoBuffer(argv[0], &pbData, &pcbData, 0));

  ChkDR(WmdrmNetUnpackRegistrationResponseMessage(&g_PrivateKey,
                          pbData,
                          pcbData,
                          &g_Seed,
                          &SerialNumber,
                          &g_RegistrationSessionID,
                          &pAddress,
                          &cbAddress,
                          pCryptoCtx));
    ChkArg(MEMCMP(&SerialNumber, &g_SerialNumber, 16) == 0);
ErrorExit:
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pCryptoCtx );
    return dr;
}

/**
 * argv[0] - packed license response message file
 */
DRM_RESULT DRM_CALL Test_WmdrmNetUnpackLicenseResponseMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr        = DRM_SUCCESS;
    DRM_BYTE     *pbData    = NULL;
    DRM_DWORD     pcbData   = 0;
    DRM_BYTE     *pbLicense = NULL;
    DRM_DWORD     cbLicense = 0;

    ChkDR(OpenFileIntoBuffer(argv[0], &pbData, &pcbData, 0));

    ChkDR(WmdrmNetUnpackLicenseResponseMessage(pbData, pcbData, &pbLicense, &cbLicense));

    ChkDR(DrmXmrUnpackLicense(&g_PrivateKey, pbLicense, cbLicense, &g_pXmrLicense));
    ChkDR(DrmXmrGetKeyFromLicense(&g_PrivateKey, g_pXmrLicense, TRUE, &g_pContentEncryptionKey));

    /* Device ID */
    ChkArg((MEMCMP(&g_pXmrLicense->gr.SerialNumber, &g_SerialNumber, DRM_ID_SIZE) == 0));

    MEMCPY( &g_xmrMinEnvironment , &g_pXmrLicense->gr , SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );

ErrorExit:

    SAFE_OEM_FREE(pbData);

    return dr;
}

/*
 * argv[0] = data block size in bytes
 * Note: you must call Test_GetHeaders before calling this function with the following argument
 * argv[1] = (optional) path to clear sample file
*/
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadData(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                   dr                    = DRM_SUCCESS;
    DRM_DWORD                    cbData                = 0;
    DRM_DWORD                   *pcbData               = NULL;
    DRM_BYTE                    *pbData                = NULL;
    DRM_DWORD                    cbDataBlockOffset     = 0;
    DRM_DWORD                    cbWritten             = 0;
    DRM_DWORD                    dwFilePos             = 0;
    DRM_DWORD                    cbDataNeededForPacket = 0;
    DRM_DWORD                    cbDataLeftInDataBlock = 0;
    DRM_BYTE                     bStreamID             = 0;
    DRM_BYTE                    *pbPayloadData         = NULL;
    DRM_BYTE                    *pbPacketData          = NULL;
    DRM_DWORD                    cbPacketData          = 0;
    DRM_DWORD                    dwPacketSize          = 0;
    DRM_DWORD                    dwNumSamplesRead      = 0;
    DRM_UINT64                   qwCurrentSampleID     = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD                    cbDataPacketOffset    = 0;
    DRM_DWORD                    cbSampleData          = 0;
    DRM_BYTE                    *pbClear               = NULL;
    DRM_BYTE                    *pbDecrypted           = NULL;
    DRM_DWORD                    i                     = 0;
    ASF_PARSER_STATE             fileState             = { 0 };
    ASF_PACKET_PARSER_INFO       filePacketInfo        = { 0 };
    ASF_PAYLOAD_PARSER_INFO      filePayloadInfo       = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT aesCtrContext         = { 0 };
    DRM_BOOL                     fResult               = FALSE;

    ChkArg( argc == 1 || argc == 2 );

    /* initialize the array of sample buffers */
    for (i = 1; i <= MAX_STREAMS_IN_ASF; i++)
    {
        g_FilePayloadFragments[i].dwPresentationTime = 0;
        g_FilePayloadFragments[i].cbDataNeed = 0;
        g_FilePayloadFragments[i].cbDataHave = 0;
        if (g_FilePayloadFragments[i].pbData != NULL)
        {
            SAFE_OEM_FREE(g_FilePayloadFragments[i].pbData);
            g_FilePayloadFragments[i].pbData = NULL;
        }
    }

    /* initialize the parser state */
    ZEROMEM( &filePacketInfo, SIZEOF( filePacketInfo ) );
    ZEROMEM( &filePayloadInfo, SIZEOF( filePayloadInfo ) );
    ZEROMEM( &fileState, SIZEOF( fileState ) );
    fileState.pPacketInfo  = &filePacketInfo;
    fileState.pPayloadInfo = &filePayloadInfo;

    cbData = (DRM_DWORD) strtoul( argv[0], NULL, 0 );

    if( argc > 1 )
    {
        ChkArg( g_FirstNextDataBlock == 0 );

        dwPacketSize = g_dwDataPacketSize;
        ChkDR( OpenClearSampleFile(argv[1]) );
        fileState.dwPacketSize = dwPacketSize;

        /* get the current offset into the file */
        fResult = Oem_File_SetFilePointer( g_hOutputFile, (DRM_LONG) 0, OEM_FILE_CURRENT, &cbDataPacketOffset );
        DRMASSERT( fResult );

        pbPacketData = (DRM_BYTE*)Oem_MemAlloc( g_dwDataPacketSize );
        TRACE(("dwDataPacketSize is %lu\n", g_dwDataPacketSize));
        ChkMem( pbPacketData );
    }

    cbDataNeededForPacket = g_dwDataPacketSize;

    if (!g_fpcbDataNULL)
    {
        pcbData = &cbData;
    }

    if (!g_fpbDataNULL)
    {
        ChkMem( pbData = (DRM_BYTE *) Oem_MemAlloc( cbData ) );
    }

    dr = Drm_Ndt_Asf_ReadData( NULL, g_pNdtDrmContext, g_MediaSessionID, pbData, pcbData );
    if ( DRM_E_NOMORE != dr && DRM_FAILED( dr ) )
    {
        ChkDR( dr );
    }
    if (dr == DRM_E_NOMORE)
    {
        g_LastDataBlock = 1;
        dr = DRM_SUCCESS;
    }
    else
    {
        g_LastDataBlock = 0;
    }
    ChkDR( dr );

    /* write the data to the output file */
    Oem_File_Write( g_hOutputFile, pbData, cbData, &cbWritten );
    ChkBOOL( cbData == cbWritten, DRM_E_FAIL );

    if (argc == 2)
    {

        cbDataLeftInDataBlock = cbData;
        cbDataBlockOffset = 0;

        do
        {
            /* put received data into a file so we can use the internal asf methods to parse the packets and payloads */
            if (cbDataNeededForPacket <= cbDataLeftInDataBlock)
            {
                cbDataBlockOffset += cbDataNeededForPacket;
                cbDataLeftInDataBlock -= cbDataNeededForPacket;
                cbDataNeededForPacket = 0;
            }
            else
            {
                cbDataBlockOffset += cbDataLeftInDataBlock;
                cbDataNeededForPacket -= cbDataLeftInDataBlock;
                cbDataLeftInDataBlock = 0;
            }

            /* when we have a complete packet, parse it for samples */
            if ( 0 == cbDataNeededForPacket )
            {
                ChkBOOL( Oem_File_SetFilePointer( g_hOutputFile, (DRM_LONG) cbDataPacketOffset, OEM_FILE_BEGIN, &dwFilePos ), DRM_E_FAIL );

                /* parse the packet header */
                fileState.tParseState = P_NEW_PACKET;
                ChkDR( _ASFParsePacketHeader( g_hOutputFile, pbPacketData, &fileState ) );
                fileState.dwPayload = 0;

                /* parse each payload, storing fragments if needed */
                if (!( fileState.pPacketInfo->fEccPresent &&
                        fileState.pPacketInfo->fParityPacket ) )
                {
                    while ( fileState.dwPayload < fileState.pPacketInfo->dwPayloads )
                    {
                        /* Parse the PAYLOAD header */
                        fileState.tParseState = P_PAYLOAD_HEADER;
                        ChkDR( _ASFParsePayloadHeader( g_hOutputFile, pbPacketData, &fileState ) );

                        bStreamID = fileState.pPayloadInfo->bStreamId;

                        /* Allocate space to save the fragment(s) */
                        if ( g_FilePayloadFragments[bStreamID].pbData == NULL )
                        {
                            ChkMem( g_FilePayloadFragments[bStreamID].pbData = (DRM_BYTE *)Oem_MemAlloc( fileState.pPayloadInfo->dwObjectSize ) );
                        }

                        /* Read the payload data into a temporary data buffer */
                        ChkMem( pbPayloadData = (DRM_BYTE *)Oem_MemAlloc( fileState.pPayloadInfo->wPayloadSize ) );
                        fResult = Oem_File_Read(g_hOutputFile, pbPayloadData, (DRM_DWORD) fileState.pPayloadInfo->wPayloadSize, &cbPacketData);
                        DRMASSERT( fResult );
                        DRMASSERT( (DRM_DWORD) fileState.pPayloadInfo->wPayloadSize == cbPacketData );

                        /* Copy the payload data into the sample data buffer */
                        DRM_BYT_CopyBytes(g_FilePayloadFragments[bStreamID].pbData, fileState.pPayloadInfo->dwObjectOffset, pbPayloadData, 0, cbPacketData);
                        g_FilePayloadFragments[bStreamID].cbDataHave += cbPacketData;
                        g_FilePayloadFragments[bStreamID].cbDataNeed = fileState.pPayloadInfo->dwObjectSize;
                        g_FilePayloadFragments[bStreamID].dwPresentationTime = fileState.pPayloadInfo->dwObjectPres;

                        SAFE_OEM_FREE(pbPayloadData);

                        /* if we have a complete sample (payload) */
                        if ( g_FilePayloadFragments[bStreamID].cbDataHave == g_FilePayloadFragments[bStreamID].cbDataNeed )
                        {

                            /* Get sample data to be decrypted */
                            pbDecrypted = (DRM_BYTE *) Oem_MemAlloc(g_FilePayloadFragments[bStreamID].cbDataHave);
                            ChkMem(pbDecrypted);
                            DRM_BYT_CopyBytes(pbDecrypted, 0, g_FilePayloadFragments[bStreamID].pbData, 0, g_FilePayloadFragments[bStreamID].cbDataHave);

                            /* decrypt the sample */
                            if (NULL != g_pContentEncryptionKey)
                            {
                                /* Get the SampleID from the payload header */
                                /* NOTE: It is assumed that the SampleID is the last 8 bytes of the replicated data, and there are cases where it might not be */
                                /* The correct way to do this is to parse through the payload extensions in the payload header */
                                if (fileState.pPayloadInfo->bRepData >= 10)
                                {
                                    NETWORKBYTES_TO_QWORD( qwCurrentSampleID, fileState.pPayloadInfo->rgbRepData, fileState.pPayloadInfo->bRepData - 8 );
                                }
                                else
                                {
                                    ChkDR( DRM_E_FAIL );
                                }

                                aesCtrContext.qwInitializationVector = qwCurrentSampleID;
                                aesCtrContext.qwBlockOffset = DRM_UI64(0);
                                aesCtrContext.bByteOffset = 0;

                                ChkDR( Oem_Aes_CtrProcessData( g_pContentEncryptionKey,
                                                                pbDecrypted,
                                                                g_FilePayloadFragments[bStreamID].cbDataHave,
                                                                &aesCtrContext) );
                            }

                            /* Get corresponding clear sample */
                            cbSampleData = fileState.pPayloadInfo->dwObjectSize;
                            pbClear = (DRM_BYTE *)  Oem_MemAlloc(cbSampleData);
                            ChkMem(pbClear);
                            ChkDR( GetClearSampleFromFile(FALSE, bStreamID, g_FilePayloadFragments[bStreamID].dwPresentationTime, &cbSampleData, &pbClear) );

                            /* compare the corresponding samples */
                            ChkArg( 0 == DRM_BYT_CompareBytes(pbClear, 0, pbDecrypted, 0, cbSampleData) );

                            /* free the sample and reset the info */
                            SAFE_OEM_FREE(pbDecrypted);
                            SAFE_OEM_FREE(pbClear);
                            SAFE_OEM_FREE( g_FilePayloadFragments[bStreamID].pbData );
                            g_FilePayloadFragments[bStreamID].cbDataHave = 0;
                            g_FilePayloadFragments[bStreamID].cbDataNeed = 0;
                            g_FilePayloadFragments[bStreamID].dwPresentationTime = 0;

                            dwNumSamplesRead++;

                            if ( 0 == (dwNumSamplesRead % 250) )
                            {
                                TRACE(("%ld samples processed\n", dwNumSamplesRead));
                            }
                        }

                        fileState.dwPayload++;
                    }
                }

                cbDataNeededForPacket = g_dwDataPacketSize;
                cbDataPacketOffset += g_dwDataPacketSize;
            }

        } while ( cbDataLeftInDataBlock > 0 );
    }

    if (argc == 2)
    {
        TRACE(("%ld sample(s) processed\n", dwNumSamplesRead));
    }

ErrorExit:
    if( argc == 2 )
    {
        CloseClearSampleFile();
    }

    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE(pbDecrypted);
    SAFE_OEM_FREE(pbClear);
    SAFE_OEM_FREE(pbPacketData);

    return dr;
}

/*
 * argv[0] = data block size in bytes
 * Note: you must call Test_GetHeaders before calling this function with the following arguments
 * argv[1] = (optional) path to clear sample file
 * argv[2] = (optional) number of samples we expect in the provided file with clear samples
*/
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadAllData(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                   dr                    = DRM_SUCCESS;
    DRM_BYTE                    *pbNextDataBlock       = NULL;
    DRM_DWORD                    cbNextDataBlock       = 0;
    DRM_DWORD                    cbDataBlock           = 0;
    DRM_DWORD                    cbDataBlockOffset     = 0;
    DRM_DWORD                    cbWritten             = 0;
    DRM_DWORD                    dwDataBlockSize       = 0;
    DRM_DWORD                    dwFilePos             = 0;
    DRM_DWORD                    cbDataNeededForPacket = 0;
    DRM_DWORD                    cbDataLeftInDataBlock = 0;
    DRM_BYTE                     bStreamID             = 0;
    DRM_BYTE                    *pbPayloadData         = NULL;
    DRM_BYTE                    *pbPacketData          = NULL;
    DRM_DWORD                    cbPacketData          = 0;
    DRM_DWORD                    dwPacketSize          = 0;
    DRM_DWORD                    dwNumSamplesRead      = 0;
    DRM_DWORD                    dwNumSamplesExpected  = 0;
    DRM_UINT64                   qwCurrentSampleID     = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD                    cbDataPacketOffset    = 0;
    DRM_DWORD                    cbSampleData          = 0;
    DRM_BYTE                    *pbClear               = NULL;
    DRM_BYTE                    *pbDecrypted           = NULL;
    DRM_DWORD                    i                     = 0;
    ASF_PARSER_STATE             fileState             = { 0 };
    ASF_PACKET_PARSER_INFO       filePacketInfo        = { 0 };
    ASF_PAYLOAD_PARSER_INFO      filePayloadInfo       = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT aesCtrContext         = { 0 };
    DRM_BOOL                     fResult;

    ChkArg( argc >= 1 && argc <= 3 );

    /* initialize the array of sample buffers */
    for (i = 1; i <= MAX_STREAMS_IN_ASF; i++)
    {
        g_FilePayloadFragments[i].dwPresentationTime = 0;
        g_FilePayloadFragments[i].cbDataNeed = 0;
        g_FilePayloadFragments[i].cbDataHave = 0;
        if (g_FilePayloadFragments[i].pbData != NULL)
        {
            SAFE_OEM_FREE(g_FilePayloadFragments[i].pbData);
            g_FilePayloadFragments[i].pbData = NULL;
        }
    }

    /* initialize the parser state */
    ZEROMEM( &filePacketInfo, SIZEOF( filePacketInfo ) );
    ZEROMEM( &filePayloadInfo, SIZEOF( filePayloadInfo ) );
    ZEROMEM( &fileState, SIZEOF( fileState ) );
    fileState.pPacketInfo  = &filePacketInfo;
    fileState.pPayloadInfo = &filePayloadInfo;

    dwDataBlockSize = (DRM_DWORD) strtoul( argv[0], NULL, 0 );

    if( argc >= 2 )
    {
        ChkArg( g_FirstNextDataBlock == 0 );

        dwPacketSize = g_dwDataPacketSize;
        ChkDR( OpenClearSampleFile(argv[1]) );
        fileState.dwPacketSize = dwPacketSize;
        g_dwDataPacketSize = dwPacketSize;

        /* get the current offset into the file */
        fResult = Oem_File_SetFilePointer( g_hOutputFile, (DRM_LONG) 0, OEM_FILE_CURRENT, &cbDataPacketOffset );
        DRMASSERT( fResult );

        pbPacketData = (DRM_BYTE*)Oem_MemAlloc( g_dwDataPacketSize );
        ChkMem( pbPacketData );
    }

    if( argc == 3 )
    {
        dwNumSamplesExpected = strtoul(argv[2], NULL, 0);
    }

    cbDataBlock = dwDataBlockSize;
    ChkMem( pbNextDataBlock = (DRM_BYTE*)Oem_MemAlloc( dwDataBlockSize ) );

    cbDataNeededForPacket = g_dwDataPacketSize;

    i = 0;
    do
    {
        i++;

        cbNextDataBlock = cbDataBlock;
        dr = Drm_Ndt_Asf_ReadData( NULL, g_pNdtDrmContext, g_MediaSessionID, pbNextDataBlock, &cbNextDataBlock );
        if ( dr == DRM_E_NOMORE && cbNextDataBlock )
        {
            dr = DRM_SUCCESS;
            g_LastDataBlock = 1;
        }
        else
        {
            g_LastDataBlock = 0;
        }
        ChkDR( dr );

        /* write the data to the output file */
        ChkBOOL( Oem_File_SetFilePointer( g_hOutputFile, 0, OEM_FILE_END, &dwFilePos), DRM_E_FAIL );
        Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );
        ChkBOOL( cbNextDataBlock == cbWritten, DRM_E_FAIL );

        if (argc == 1)
        {
            continue;
        }

        cbDataLeftInDataBlock = cbNextDataBlock;
        cbDataBlockOffset = 0;

        do
        {
            /* put received data into a file so we can use the internal asf methods to parse the packets and payloads */
            if (cbDataNeededForPacket <= cbDataLeftInDataBlock)
            {
                cbDataBlockOffset += cbDataNeededForPacket;
                cbDataLeftInDataBlock -= cbDataNeededForPacket;
                cbDataNeededForPacket = 0;
            }
            else
            {
                cbDataBlockOffset += cbDataLeftInDataBlock;
                cbDataNeededForPacket -= cbDataLeftInDataBlock;
                cbDataLeftInDataBlock = 0;
            }

            /* when we have a complete packet, parse it for samples */
            if ( 0 == cbDataNeededForPacket )
            {
                ChkBOOL( Oem_File_SetFilePointer( g_hOutputFile, (DRM_LONG) cbDataPacketOffset, OEM_FILE_BEGIN, &dwFilePos ), DRM_E_FAIL );

                /* parse the packet header */
                fileState.tParseState = P_NEW_PACKET;
                ChkDR( _ASFParsePacketHeader( g_hOutputFile, pbPacketData, &fileState ) );
                fileState.dwPayload = 0;

                /* parse each payload, storing fragments if needed */
                if (!( fileState.pPacketInfo->fEccPresent &&
                       fileState.pPacketInfo->fParityPacket ) )
                {
                    while ( fileState.dwPayload < fileState.pPacketInfo->dwPayloads )
                    {
                        /* Parse the PAYLOAD header */
                        fileState.tParseState = P_PAYLOAD_HEADER;
                        ChkDR( _ASFParsePayloadHeader( g_hOutputFile, pbPacketData, &fileState ) );

                        bStreamID = fileState.pPayloadInfo->bStreamId;

                        /* Allocate space to save the fragment(s) */
                        if ( g_FilePayloadFragments[bStreamID].pbData == NULL )
                        {
                            ChkMem( g_FilePayloadFragments[bStreamID].pbData = (DRM_BYTE *)Oem_MemAlloc( fileState.pPayloadInfo->dwObjectSize ) );
                        }

                        /* Read the payload data into a temporary data buffer */
                        ChkMem( pbPayloadData = (DRM_BYTE *)Oem_MemAlloc( fileState.pPayloadInfo->wPayloadSize ) );
                        fResult = Oem_File_Read(g_hOutputFile, pbPayloadData, (DRM_DWORD) fileState.pPayloadInfo->wPayloadSize, &cbPacketData);
                        DRMASSERT( fResult );
                        DRMASSERT( (DRM_DWORD) fileState.pPayloadInfo->wPayloadSize == cbPacketData );

                        /* Copy the payload data into the sample data buffer */
                        DRM_BYT_CopyBytes(g_FilePayloadFragments[bStreamID].pbData, fileState.pPayloadInfo->dwObjectOffset, pbPayloadData, 0, cbPacketData);
                        g_FilePayloadFragments[bStreamID].cbDataHave += cbPacketData;
                        g_FilePayloadFragments[bStreamID].cbDataNeed = fileState.pPayloadInfo->dwObjectSize;
                        g_FilePayloadFragments[bStreamID].dwPresentationTime = fileState.pPayloadInfo->dwObjectPres;

                        SAFE_OEM_FREE(pbPayloadData);

                        /* if we have a complete sample (payload) */
                        if ( g_FilePayloadFragments[bStreamID].cbDataHave == g_FilePayloadFragments[bStreamID].cbDataNeed )
                        {

                            /* Get sample data to be decrypted */
                            pbDecrypted = (DRM_BYTE *) Oem_MemAlloc(g_FilePayloadFragments[bStreamID].cbDataHave);
                            ChkMem(pbDecrypted);
                            DRM_BYT_CopyBytes(pbDecrypted, 0, g_FilePayloadFragments[bStreamID].pbData, 0, g_FilePayloadFragments[bStreamID].cbDataHave);

                            /* decrypt the sample */
                            if (NULL != g_pContentEncryptionKey)
                            {
                                /* Get the SampleID from the payload header */
                                /* NOTE: It is assumed that the SampleID is the last 8 bytes of the replicated data, and there are cases where it might not be */
                                /* The correct way to do this is to parse through the payload extensions in the payload header */
                                if (fileState.pPayloadInfo->bRepData >= 10)
                                {
                                    NETWORKBYTES_TO_QWORD( qwCurrentSampleID, fileState.pPayloadInfo->rgbRepData, fileState.pPayloadInfo->bRepData - 8 );
                                }
                                else
                                {
                                    ChkDR( DRM_E_FAIL );
                                }

                                aesCtrContext.qwInitializationVector = qwCurrentSampleID;
                                aesCtrContext.qwBlockOffset = DRM_UI64(0);
                                aesCtrContext.bByteOffset = 0;

                                ChkDR( Oem_Aes_CtrProcessData( g_pContentEncryptionKey,
                                                             pbDecrypted,
                                                             g_FilePayloadFragments[bStreamID].cbDataHave,
                                                             &aesCtrContext) );
                            }

                            /* Get corresponding clear sample */
                            cbSampleData = fileState.pPayloadInfo->dwObjectSize;
                            pbClear = (DRM_BYTE *)  Oem_MemAlloc(cbSampleData);
                            ChkMem(pbClear);
                            ChkDR( GetClearSampleFromFile(FALSE, bStreamID, g_FilePayloadFragments[bStreamID].dwPresentationTime, &cbSampleData, &pbClear) );

                            /* compare the corresponding samples */
                            if ( 0 != DRM_BYT_CompareBytes(pbClear, 0, pbDecrypted, 0, cbSampleData) )
                            {
                                ChkDR( DRM_E_FAIL );
                            }

                            /* free the sample and reset the info */
                            SAFE_OEM_FREE(pbDecrypted);
                            SAFE_OEM_FREE(pbClear);
                            SAFE_OEM_FREE( g_FilePayloadFragments[bStreamID].pbData );
                            g_FilePayloadFragments[bStreamID].cbDataHave = 0;
                            g_FilePayloadFragments[bStreamID].cbDataNeed = 0;
                            g_FilePayloadFragments[bStreamID].dwPresentationTime = 0;

                            dwNumSamplesRead++;

                            if ( 0 == (dwNumSamplesRead % 250) )
                            {
                                TRACE(("%ld samples processed\n", dwNumSamplesRead));
                            }
                        }

                        fileState.dwPayload++;
                    }
                }

                cbDataNeededForPacket = g_dwDataPacketSize;
                cbDataPacketOffset += g_dwDataPacketSize;
            }

        } while ( cbDataLeftInDataBlock > 0 );

    } while ( g_LastDataBlock == 0 );

    if( argc > 1 )
    {
        TRACE(("%ld samples processed\n", dwNumSamplesRead));
    }
    if (argc == 3)
    {
        if (dwNumSamplesExpected != dwNumSamplesRead)
        {
            dr = DRM_E_FAIL;
        }
        TRACE(("%ld samples expected\n", dwNumSamplesExpected));
    }

ErrorExit:
    SAFE_OEM_FREE(pbNextDataBlock);

    if( argc >= 2 )
    {
        CloseClearSampleFile();
    }

    SAFE_OEM_FREE(pbDecrypted);
    SAFE_OEM_FREE(pbClear);
    SAFE_OEM_FREE(pbPacketData);

    return dr;
}

/**
 * argv[0] = sample size in bytes
 * argv[1] = (optional) file with clear samples
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadSample(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT      dr                         = DRM_SUCCESS;
    NDT_SAMPLEINFO *pSampleInfo                = NULL;
    DRM_DWORD       cbSampleData               = 0;
    DRM_DWORD      *pcbSampleData              = NULL;
    DRM_BYTE       *pbSampleData               = NULL;
    DRM_DWORD       cbWritten                  = 0;
    DRM_BYTE       *pbClear                    = NULL;
    DRM_BYTE       *pbDecrypted                = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT aesCtrContext = {0};

    ChkArg( argc == 1 || argc == 2 );
    cbSampleData = (DRM_DWORD) strtoul( argv[0], NULL, 0 );

    if (!g_fpSampleInfoNULL)
    {
        pSampleInfo = &g_SampleInfo;
    }
    if (!g_fpcbSampleDataNULL)
    {
        pcbSampleData = &cbSampleData;
    }
    if (!g_fpbSampleDataNULL && 0 != cbSampleData)
    {
        ChkMem( pbSampleData = (DRM_BYTE*)Oem_MemAlloc( cbSampleData ) );
    }

    dr = Drm_Ndt_Asf_ReadSample( NULL,
                                 g_pNdtDrmContext,
                                 g_MediaSessionID,
                                 pSampleInfo,
                                 pbSampleData,
                                 pcbSampleData);
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        if ( g_cbSampleDataOverride != -1 )
        {
            *pcbSampleData = g_cbSampleDataOverride;
        }
        if ( !g_fpbSampleDataNULL )
        {
            pbSampleData = (DRM_BYTE *) Oem_MemAlloc(*pcbSampleData);
            ChkMem(pbSampleData);
        }

        dr = Drm_Ndt_Asf_ReadSample( NULL,
                                     g_pNdtDrmContext,
                                     g_MediaSessionID,
                                     pSampleInfo,
                                     pbSampleData,
                                     pcbSampleData);
    }
    if ( DRM_E_NOMORE != dr && DRM_FAILED( dr ) )
    {
        ChkDR( dr );
    }

    if ( argc == 2 )
    {
        /* Get sample data to be decrypted */
        pbDecrypted = (DRM_BYTE *) Oem_MemAlloc(*pcbSampleData);
        ChkMem(pbDecrypted);
        DRM_BYT_CopyBytes(pbDecrypted, 0, pbSampleData, 0, *pcbSampleData);

        /* Get corresponding clear sample */
        pbClear = (DRM_BYTE *)  Oem_MemAlloc(*pcbSampleData);
        ChkMem(pbClear);
        ChkDR( OpenClearSampleFile(argv[1]) );
        ChkDR( GetClearSampleFromFile(TRUE, pSampleInfo->dwStreamID, DRM_UI64Low32(DRM_UI64Div(pSampleInfo->qwSampleTimeNS, DRM_UI64(10000))), pcbSampleData, &pbClear) );
        ChkDR( CloseClearSampleFile() );

        /* Decrypt and compare to jazz_clear sample */
        if (NULL != g_pContentEncryptionKey)
        {
            aesCtrContext.qwInitializationVector = pSampleInfo->qwSampleID;
            aesCtrContext.qwBlockOffset = DRM_UI64(0);
            aesCtrContext.bByteOffset = 0;

            ChkDR( Oem_Aes_CtrProcessData( g_pContentEncryptionKey,
                                           pbDecrypted,
                                           *pcbSampleData,
                                           &aesCtrContext) );
        }
        ChkBOOL( 0 == DRM_BYT_CompareBytes(pbClear, 0, pbDecrypted, 0, *pcbSampleData), DRM_E_FAIL );
    }

    /* If we've reached the end of the file, then this test is successful. */
    if (dr == DRM_E_NOMORE)
    {
        g_LastDataBlock = 1;
        dr = DRM_SUCCESS;
    }
    else
    {
        g_LastDataBlock = 0;
    }

    if( OEM_INVALID_HANDLE_VALUE != g_hOutputFile )
    {
        Oem_File_Write( g_hOutputFile, pbSampleData, cbSampleData, &cbWritten );
        ChkBOOL( cbSampleData == cbWritten, DRM_E_FAIL );
    }

ErrorExit:
    SAFE_OEM_FREE( pbSampleData );
    SAFE_OEM_FREE( pbClear );
    SAFE_OEM_FREE( pbDecrypted );

    return dr;
}

/*
 * argv[0] = sample size in bytes
 * argv[1] = (optional) file with clear samples
 * argv[2] = (optional) number of samples we expect in the provided file with clear samples
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadAllSamples(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT      dr                         = DRM_SUCCESS;
    DRM_DWORD       cbSampleDataBlock          = 0;
    NDT_SAMPLEINFO *pSampleInfo                = NULL;
    DRM_DWORD       cbSampleData               = 0;
    DRM_DWORD      *pcbSampleData              = NULL;
    DRM_BYTE       *pbSampleData               = NULL;
    DRM_DWORD       cbWritten                  = 0;
    DRM_DWORD       dwNumSamplesRead           = 0;
    DRM_DWORD       dwNumSamplesExpected       = 0;
    DRM_BYTE       *pbClear                    = NULL;
    DRM_BYTE       *pbDecrypted                = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT aesCtrContext = {0};

    ChkArg( argc >= 1 );
    cbSampleDataBlock = (DRM_DWORD) strtoul( argv[0], NULL, 0 );
    cbSampleData = cbSampleDataBlock;
    if( argc == 3 )
    {
        dwNumSamplesExpected = strtoul(argv[2], NULL, 0);
        ChkDR( OpenClearSampleFile(argv[1]) );
    }

    if (!g_fpSampleInfoNULL)
    {
        pSampleInfo = &g_SampleInfo;
    }
    if (!g_fpcbSampleDataNULL)
    {
        pcbSampleData = &cbSampleData;
    }
    if (!g_fpbSampleDataNULL && 0 != cbSampleData)
    {
        ChkMem( pbSampleData = (DRM_BYTE*)Oem_MemAlloc( cbSampleData ) );
    }

    do
    {
        *pcbSampleData = cbSampleDataBlock;
        dr = Drm_Ndt_Asf_ReadSample( NULL, g_pNdtDrmContext, g_MediaSessionID, pSampleInfo, pbSampleData, pcbSampleData );
        if ( dr == DRM_SUCCESS )
        {
            Oem_File_Write( g_hOutputFile, pbSampleData, cbSampleData, &cbWritten );

            if ( argc >= 2 )
            {
                /* Get sample data to be decrypted */
                pbDecrypted = (DRM_BYTE *) Oem_MemAlloc(*pcbSampleData);
                ChkMem(pbDecrypted);
                DRM_BYT_CopyBytes(pbDecrypted, 0, pbSampleData, 0, *pcbSampleData);

                /* Get corresponding clear sample */
                pbClear = (DRM_BYTE *)  Oem_MemAlloc(*pcbSampleData);
                ChkMem(pbClear);
                ChkDR( GetClearSampleFromFile(FALSE, pSampleInfo->dwStreamID, DRM_UI64Low32(DRM_UI64Div(pSampleInfo->qwSampleTimeNS, DRM_UI64(10000))), pcbSampleData, &pbClear) );

                /* Decrypt and compare to clear sample */
                if (NULL != g_pContentEncryptionKey)
                {
                    aesCtrContext.qwInitializationVector = pSampleInfo->qwSampleID;
                    aesCtrContext.qwBlockOffset = DRM_UI64(0);
                    aesCtrContext.bByteOffset = 0;

                    ChkDR( Oem_Aes_CtrProcessData( g_pContentEncryptionKey,
                                                   pbDecrypted,
                                                   *pcbSampleData,
                                                   &aesCtrContext ) );
                }
                ChkBOOL( 0 == DRM_BYT_CompareBytes(pbClear, 0, pbDecrypted, 0, *pcbSampleData), DRM_E_FAIL );
                SAFE_OEM_FREE( pbDecrypted );
                SAFE_OEM_FREE( pbClear );
            }

        }
        else if ( DRM_E_NOMORE == dr )
        {
            TRACE(("Got all %ld samples successfully\n", dwNumSamplesRead));
            break;
        }
        else
        {
            /*    ChkDR( dr ); */
            TRACE(("Got an error on sample %lu (cbSampleData = %lu).  ignoring\n", dwNumSamplesRead, cbSampleData));
            if ( NULL != pSampleInfo )
            {
                TRACE(("Last valid(?) sample: stream = %ld, time = %ld size = %ld\n",
                        pSampleInfo->dwStreamID,
                        DRM_UI64Low32(pSampleInfo->qwSampleTimeNS),
                        cbSampleData));
            }
            Oem_File_Write( g_hOutputFile, pbSampleData, cbSampleData, &cbWritten );
        }
        dwNumSamplesRead++;
        if ( 0 == (dwNumSamplesRead % 250) )
        {
            TRACE(("%ld samples processed\n", dwNumSamplesRead));
        }
    } while (dr == DRM_SUCCESS);

    /* If we've reached the end of the file, then this test is successful. */
    if (dr == DRM_E_NOMORE)
    {
        g_fEndOfData = TRUE;
        g_LastDataBlock = 1;
        if (argc == 3 && dwNumSamplesExpected != dwNumSamplesRead)
        {
            dr = DRM_E_FAIL;
        }
        else
        {
            dr = DRM_SUCCESS;
        }
    }
    else
    {
        g_LastDataBlock = 0;
    }

ErrorExit:

    if( argc >= 2 )
    {
        CloseClearSampleFile();
    }

    SAFE_OEM_FREE( pbSampleData );
    SAFE_OEM_FREE( pbClear );
    SAFE_OEM_FREE( pbDecrypted );

    return dr;
}

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_GetLeafLicenseResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr                     = DRM_SUCCESS;
    DRM_KID      *pKID                   = NULL;
    DRM_BYTE     *pbLeafLicenseResponse  = NULL;
    DRM_DWORD    *pcbLeafLicenseResponse = NULL;
    DRM_DWORD     cbLeafLicenseResponse  = 0;
    DRM_BYTE     *pbLeafLicense          = NULL;
    DRM_DWORD     cbLeafLicense          = 0;

    if (!g_fpKIDNULL)
    {
        pKID = &g_KID;
    }
    if (!g_fpcbLeafLicenseResponseNULL)
    {
        pcbLeafLicenseResponse = &cbLeafLicenseResponse;
    }

    dr = Drm_Ndt_Asf_GenerateLeafLicenseResponse( NULL,
                                                  g_pNdtDrmContext,
                                                  g_MediaSessionID,
                                                  pKID,
                                                  pbLeafLicenseResponse,
                                                  pcbLeafLicenseResponse);

    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        if ( g_cbLeafLicenseResponseOverride != -1 )
        {
            *pcbLeafLicenseResponse = g_cbLeafLicenseResponseOverride;
        }
        if ( !g_fpbLeafLicenseResponseNULL )
        {
            pbLeafLicenseResponse = (DRM_BYTE *) Oem_MemAlloc(*pcbLeafLicenseResponse);
            ChkMem(pbLeafLicenseResponse);
        }

        dr = Drm_Ndt_Asf_GenerateLeafLicenseResponse( NULL,
                                                      g_pNdtDrmContext,
                                                      g_MediaSessionID,
                                                      pKID,
                                                      pbLeafLicenseResponse,
                                                      pcbLeafLicenseResponse);
    }
    ChkDR(dr);

    /* Unpack the license response.  Doing so verifies the response message. */
    ChkDR(WmdrmNetUnpackLicenseResponseMessage(pbLeafLicenseResponse, *pcbLeafLicenseResponse, &pbLeafLicense, &cbLeafLicense));

    if ( g_pXmrLicense != g_RootXmrLicenseInfo.pXmrLicense )
    {
        DrmXmrFreeLicense(g_pXmrLicense);
        g_pXmrLicense = NULL;
    }
    ChkDR(DrmXmrUnpackLeafLicense(g_RootXmrLicenseInfo, pbLeafLicense, cbLeafLicense, &g_pXmrLicense));

    if ( g_pRootContentEncryptionKey != g_pContentEncryptionKey && NULL != g_pContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
        g_pContentEncryptionKey = NULL;
    }
    ChkDR(DrmXmrGetKeyFromLicenseEx(FALSE, (DRM_BYTE *) g_pRootContentEncryptionKey, g_pXmrLicense, TRUE, &g_pContentEncryptionKey));

    /* Uplink KID */
    ChkArg((MEMCMP(&g_pXmrLicense->km.uk.idKID, &g_KID, DRM_ID_SIZE) == 0));

    MEMCPY( &g_xmrMinEnvironment , &g_pXmrLicense->gr , SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );

ErrorExit:
    SAFE_OEM_FREE(pbLeafLicenseResponse);
    SAFE_OEM_FREE(pbLeafLicense);

    return dr;
}

/**
 * argv[0] = (optional) path of ASF file to compare the header
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_GetHeader(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr           = DRM_SUCCESS;
    DRM_BYTE     *pbHeader     = NULL;
    DRM_DWORD    *pcbHeader    = NULL;
    DRM_DWORD     cbHeader     = 0;
    DRM_BYTE     *pFileHeader  = NULL;
    DRM_DWORD     cbFileHeader = 0;

    if (!g_fpcbHeaderNULL)
    {
        pcbHeader = &cbHeader;
    }

    dr = Drm_Ndt_Asf_GetHeader( NULL,
                                g_pNdtDrmContext,
                                g_MediaSessionID,
                                pbHeader,
                                pcbHeader);

    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        if ( g_cbHeaderOverride != -1 && NULL != pcbHeader )
        {
            *pcbHeader = g_cbHeaderOverride;
        }
        if ( !g_fpbHeaderNULL )
        {
            pbHeader = (DRM_BYTE *) Oem_MemAlloc(*pcbHeader);
            ChkMem(pbHeader);
        }

        dr = Drm_Ndt_Asf_GetHeader( NULL,
                                    g_pNdtDrmContext,
                                    g_MediaSessionID,
                                    pbHeader,
                                    pcbHeader);
    }
    ChkDR(dr);

    /* verify header information matches ASF content */
    if (argc == 1 && argv[0])
    {
        /* Open the file */
        ChkDR(OpenFileIntoBuffer(argv[0], &pFileHeader, &cbFileHeader, *pcbHeader));
        ChkMem(pFileHeader);

        /* Compare the header to the raw header from the file */
        ChkArg( 0 == MEMCMP(pbHeader, pFileHeader, *pcbHeader) );
    }


ErrorExit:
    SAFE_OEM_FREE(pbHeader);
    SAFE_OEM_FREE(pFileHeader);

    return dr;
}

/*
 * Note: this function gets the header object and
 * data object header.
 */
DRM_RESULT DRM_CALL Test_GetHeaders(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT               dr                          = DRM_SUCCESS;
    DRM_BOOL                 fFound                      = FALSE;
    DRM_BYTE                *pbNextDataBlock             = NULL;
    DRM_DWORD                cbNextDataBlock             = 0;
    DRM_DWORD                cbCurrentHeaderObjectOffset = 0;
    DRM_DWORD                cbWritten                   = 0;
    DRM_UINT64               qwDataPackets               = DRM_UI64( 0 );
    DRM_UINT64               cbDataObjectOffset          = DRM_UI64( 0 );
    DRM_GUID                 guidTmp                     = { 0 };
    DRM_DWORD                dwOffset                    = 0;

    /* Can only get the header info if this is a new session */
    ChkArg( g_FirstNextDataBlock == 1 );

    /* Extract Data Object offset from ASF Header */
    cbNextDataBlock = 30;
    pbNextDataBlock = (DRM_BYTE*)Oem_MemAlloc( cbNextDataBlock );
    ChkMem( pbNextDataBlock );

    ChkDR( Drm_Ndt_Asf_ReadData( NULL, g_pNdtDrmContext, g_MediaSessionID, pbNextDataBlock, &cbNextDataBlock ) );
    LoadGUIDOffset( guidTmp, pbNextDataBlock, dwOffset );
    ChkArg( 0 == DRM_BYT_CompareBytes( &CLSID_ASFHeaderObjectV0, 0, &guidTmp, 0, SIZEOF( DRM_GUID ) ) );
    DRM_BYT_CopyBytes(&cbDataObjectOffset, 0, pbNextDataBlock, SIZEOF( DRM_GUID ), SIZEOF(DRM_UINT64));
    FIX_ENDIAN_DWORD(cbDataObjectOffset);
    Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );
    SAFE_OEM_FREE( pbNextDataBlock );

    /* Read Header Object and Find File Properties Object */
    cbNextDataBlock = DRM_UI64Low32( DRM_UI64Sub( cbDataObjectOffset, DRM_UI64( 30 ) ) );
    pbNextDataBlock = (DRM_BYTE*)Oem_MemAlloc( cbNextDataBlock );
    ChkMem( pbNextDataBlock );
    ChkDR( Drm_Ndt_Asf_ReadData( NULL, g_pNdtDrmContext, g_MediaSessionID, pbNextDataBlock, &cbNextDataBlock ) );

    while ( TRUE != fFound )
    {
        DRM_UINT64 qwThisHeaderSize;

        dwOffset = cbCurrentHeaderObjectOffset;
        LoadGUIDOffset( guidTmp, pbNextDataBlock, dwOffset );
        if ( 0 == DRM_BYT_CompareBytes( &CLSID_ASFFilePropertiesObjectV2, 0, &guidTmp, 0, SIZEOF( DRM_GUID ) ) )
        {
            fFound = TRUE;
        }

        DRM_BYT_CopyBytes( &qwThisHeaderSize, 0, pbNextDataBlock, cbCurrentHeaderObjectOffset + SIZEOF( DRM_GUID ), SIZEOF( DRM_UINT64 ) );
        FIX_ENDIAN_QWORD( qwThisHeaderSize );
        cbCurrentHeaderObjectOffset += DRM_UI64Low32( qwThisHeaderSize );
    }

    /* Extract packet information from File Properties Object */
    DRM_BYT_CopyBytes(&qwDataPackets, 0, pbNextDataBlock, cbCurrentHeaderObjectOffset - 104 + 56, SIZEOF(DRM_UINT64));
    FIX_ENDIAN_QWORD( qwDataPackets );
    g_nDataPackets = DRM_UI64Low32( qwDataPackets );

    DRM_BYT_CopyBytes(&g_dwDataPacketSize, 0, pbNextDataBlock, cbCurrentHeaderObjectOffset - 104 + 96, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD( g_dwDataPacketSize );

    Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );
    SAFE_OEM_FREE( pbNextDataBlock );

    /* Move to the Start of the Data Packets */
    cbNextDataBlock = 50;
    pbNextDataBlock = (DRM_BYTE*)Oem_MemAlloc( cbNextDataBlock );
    ChkMem( pbNextDataBlock );

    ChkDR( Drm_Ndt_Asf_ReadData( NULL, g_pNdtDrmContext, g_MediaSessionID, pbNextDataBlock, &cbNextDataBlock ) );

    g_FirstNextDataBlock = 0;

    Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );
    SAFE_OEM_FREE(pbNextDataBlock);

ErrorExit:
    SAFE_OEM_FREE(pbNextDataBlock);

    return dr;
}

DRM_RESULT DRM_CALL Test_LinkEncryption(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT            dr                  = DRM_SUCCESS;
    DRM_WCHAR             wszFilename[256]    = { 0 };
    DRM_WCHAR             wszOutFilename[256] = { 0 };
    OEM_FILEHDL           hFile               = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL           hOutFile            = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD             lSize               = 0;
    DRM_SUBSTRING         dsubstr             = EMPTY_DRM_SUBSTRING;
    DRM_STRING            dstr                = EMPTY_DRM_STRING;
    DRM_DWORD             cbDataSegment       = 0;
    DRM_BYTE             *pbDataSegment       = NULL;
    DRM_DWORD             cbRead              = 0;
    NDT_CRYPTO_DATA_LINK  ndtCryptoDataLink   = { 0 };
    DRM_UINT64            qwDataSegmentId     = DRM_UI64(0);
    DRM_DWORD             dwFilePos           = 0;
    DRM_BYTEBLOB          blobData            = {0, 0};
    DRM_DWORD             cbData              = 0;
    DRM_BYTE             *pbData              = NULL;
    DRM_BYTE             *pbOrigData          = NULL;
    DRM_BYTEBLOB          DataSegmentDesc     = {0, 0};

#ifdef __TMS320C55X__
    int i;
#endif
    ZEROMEM( &ndtCryptoDataLink, SIZEOF( ndtCryptoDataLink ) );
    ChkArg( argc >= 3 && argv[0] != NULL && argv[1] != NULL );

#ifdef __TMS320C55X__
    for(i = 0; i < strlen(argv[0]) + 1; i++)
    {
        PUT_BYTE(rgbFilename, i, argv[0][i]);
    }
    for(i = 0; i < strlen(argv[1]) + 1; i++)
    {
        PUT_BYTE(rgbOutFilename, i, argv[1][i]);
    }

    dstr.pwszString = wszFilename;
    dsubstr.m_ich = 0;

    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[0]);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(rgbFilename, &dsubstr, &dstr);
    wszFilename[dstr.cchString] = 0;

    dstr.pwszString = wszOutFilename;
    dsubstr.m_ich = 0;

    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[1]);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(rgbOutFilename, &dsubstr, &dstr);
#else
    dstr.pwszString = wszFilename;
    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[0]);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(argv[0], &dsubstr, &dstr);

    if ( dstr.cchString < NO_OF( wszFilename ) )
    {
        wszFilename[dstr.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    dstr.pwszString = wszOutFilename;
    dsubstr.m_ich = 0;
    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[1]);
    dstr.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(argv[1], &dsubstr, &dstr);
#endif /* __TMS320C55X__ */

    if ( dstr.cchString < NO_OF( wszOutFilename ) )
    {
        wszOutFilename[dstr.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    cbDataSegment = strtoul( argv[2], NULL, 0 );
    ChkArg( cbDataSegment != 0 );

    ChkMem( pbDataSegment = (DRM_BYTE*)Oem_MemAlloc( cbDataSegment ) );

    hFile = Oem_File_Open(NULL, wszFilename, OEM_GENERIC_READ,  OEM_FILE_SHARE_NONE, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if(hFile == OEM_INVALID_HANDLE_VALUE)
    {
        TRACE(("Unable to open file %s.\n", argv[0]));
        ChkDR( DRM_E_FAIL );
    }

    hOutFile = Oem_File_Open(NULL, wszOutFilename, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
    if(hOutFile == OEM_INVALID_HANDLE_VALUE)
    {
        TRACE(("Unable to open file %s.\n", argv[1]));
        ChkDR( DRM_E_FAIL );
    }

    Oem_File_GetSize(hFile, &lSize);

    ndtCryptoDataLink.Data.pbBlob = pbDataSegment;
    ndtCryptoDataLink.pDataSegmentDescriptor = &DataSegmentDesc;
    if(argc > 3)
    {
        ndtCryptoDataLink.fNoBlockID = strtoul(argv[3], NULL, 0);
    }
    if(argc > 4)
    {
        ndtCryptoDataLink.fNoEncrypt = strtoul(argv[4], NULL, 0);
    }

    if(argc > 3)
    {
        ndtCryptoDataLink.fNoBlockID = strtoul(argv[3], NULL, 0);
    }

    if(argc > 4)
    {
        ndtCryptoDataLink.fNoEncrypt = strtoul(argv[4], NULL, 0);
    }

    while(lSize)
    {
        DRM_DWORD cb;
        DRM_DWORD cbWritten;

        ndtCryptoDataLink.qwDataSegmentID = qwDataSegmentId;
        qwDataSegmentId = DRM_UI64Add( qwDataSegmentId, DRM_UI64( 1 ) );
        Oem_File_Read(hFile, pbDataSegment, cbDataSegment, &cbRead);
        lSize -= cbRead;
        ndtCryptoDataLink.Data.cbBlob = cbRead;
        dr = NDTMGR_Encrypt( g_pNdtDrmContext,
                             g_MediaSessionID,
                             NDT_ENCRYPTION_MODE_LINK,
                             &ndtCryptoDataLink );
        if(dr == DRM_E_BUFFERTOOSMALL)
        {
            SAFE_OEM_FREE(ndtCryptoDataLink.pDataSegmentDescriptor->pbBlob);
            ChkMem(ndtCryptoDataLink.pDataSegmentDescriptor->pbBlob =
                   (DRM_BYTE*)Oem_MemAlloc(ndtCryptoDataLink.pDataSegmentDescriptor->cbBlob));
            ChkDR( NDTMGR_Encrypt( g_pNdtDrmContext,
                                 g_MediaSessionID,
                                 NDT_ENCRYPTION_MODE_LINK,
                                 &ndtCryptoDataLink ) );
        }
        cb = ndtCryptoDataLink.pDataSegmentDescriptor->cbBlob +
             ndtCryptoDataLink.Data.cbBlob;
        Oem_File_Write(hOutFile, &cb, SIZEOF(DRM_DWORD), &cbWritten);
        ChkBOOL( cbWritten == SIZEOF(DRM_DWORD ), DRM_E_FAIL );
        Oem_File_Write(hOutFile,
                      ndtCryptoDataLink.pDataSegmentDescriptor->pbBlob,
                      ndtCryptoDataLink.pDataSegmentDescriptor->cbBlob,
                      &cbWritten);
        ChkBOOL( cbWritten == ndtCryptoDataLink.pDataSegmentDescriptor->cbBlob,
                 DRM_E_FAIL );

        Oem_File_Write(hOutFile,
                      ndtCryptoDataLink.Data.pbBlob,
                      ndtCryptoDataLink.Data.cbBlob,
                      &cbWritten);
        ChkBOOL( cbWritten == ndtCryptoDataLink.Data.cbBlob,
                 DRM_E_FAIL );
    }

    if(hOutFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( hOutFile );
        hOutFile = OEM_INVALID_HANDLE_VALUE;
    }

    Oem_File_SetFilePointer(hFile, 0, OEM_FILE_BEGIN, &dwFilePos);

    hOutFile = Oem_File_Open(NULL, wszOutFilename, OEM_GENERIC_READ,  OEM_FILE_SHARE_NONE, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if(hOutFile == OEM_INVALID_HANDLE_VALUE)
    {
        TRACE(("Unable to open file %s.\n", argv[1]));
        ChkDR( DRM_E_FAIL );
    }
    for(;;)
    {
        Oem_File_Read(hOutFile, &blobData.cbBlob, SIZEOF(DRM_DWORD), &cbRead);
        if(cbRead == 0)
        {
            break;
        }
        ChkBOOL(cbRead == SIZEOF(DRM_DWORD), DRM_E_FAIL);
        SAFE_OEM_FREE(blobData.pbBlob);
        ChkMem( blobData.pbBlob = (DRM_BYTE*)Oem_MemAlloc( blobData.cbBlob ) );
        Oem_File_Read(hOutFile, blobData.pbBlob, blobData.cbBlob, &cbRead);
        ChkBOOL(cbRead == blobData.cbBlob, DRM_E_FAIL);

        cbData = 0;
        dr = WmdrmNetLinkDecrypt( g_pContentEncryptionKey,
                                  WMDRMNET_ENCRYPTION_MODE_LINK,
                                  (DRM_VOID *)&blobData, NULL, &cbData  );
        if(dr == DRM_E_BUFFERTOOSMALL)
        {
            SAFE_OEM_FREE(pbData);
            pbData = (DRM_BYTE*)Oem_MemAlloc( cbData );
            ChkMem( pbData );
            ChkOverflow( blobData.cbBlob, blobData.cbBlob - cbData );
            DRM_BYT_CopyBytes( pbData, 0, blobData.pbBlob, blobData.cbBlob - cbData, cbData );
            dr = WmdrmNetLinkDecrypt( g_pContentEncryptionKey,
                                      WMDRMNET_ENCRYPTION_MODE_LINK,
                                     (DRM_VOID *)&blobData, pbData, &cbData );
        }
        ChkDR( dr );

        SAFE_OEM_FREE(pbOrigData);
        ChkMem(pbOrigData = (DRM_BYTE*)Oem_MemAlloc(cbData) );
        Oem_File_Read(hFile, pbOrigData, cbData, &cbRead);
        ChkBOOL( cbRead == cbData &&
                 MEMCMP( pbOrigData, pbData, cbData) == 0,
                 DRM_E_FAIL );
    }

ErrorExit:
    if(hOutFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( hOutFile );
        hOutFile = OEM_INVALID_HANDLE_VALUE;
    }
    if(hFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( hFile );
        hFile = OEM_INVALID_HANDLE_VALUE;
    }
    SAFE_OEM_FREE( pbDataSegment );
    if(ndtCryptoDataLink.pDataSegmentDescriptor)
    {
        SAFE_OEM_FREE( ndtCryptoDataLink.pDataSegmentDescriptor->pbBlob );
    }
    SAFE_OEM_FREE( blobData.pbBlob );
    SAFE_OEM_FREE( pbOrigData );
    SAFE_OEM_FREE(pbData);

    return dr;
}

/*
 * argv[0] - String containing proximity detection address
 */
DRM_RESULT DRM_CALL Test_SetProximityDetectionAddress(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( 1 == argc && NULL != argv[0] );

    g_cbProximityDetectionAddress = ( DRM_DWORD ) strlen(argv[0]) + 1; /* Don't forget the NUL terminator */

#ifdef __TMS320C55X__
    {
        int i;
        for (i = 0; i < g_cbProximityDetectionAddress; i++)
        {
            PUT_BYTE(argv[0], i, argv[0][i]);
        }
    }
#endif

    DRM_BYT_CopyBytes(g_rgbProximityDetectionAddress, 0, argv[0], 0, g_cbProximityDetectionAddress);

ErrorExit:

    return dr;
}

/*
 * argv[0] - "TRUE" = allow registration, "FALSE" = don't allow
 */
DRM_RESULT DRM_CALL Test_AllowNewDeviceRegistration(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc ==1 && NULL != argv[0] );

    if ( 0 == strcmp(argv[0], "TRUE") )
    {
        g_fAllowNewDeviceRegistration = TRUE;
    }
    else
    {
        g_fAllowNewDeviceRegistration = FALSE;
    }

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL Test_NDTMGR_GetRevocationListRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT               dr           = DRM_SUCCESS;
    DRM_REVOCATION_LIST_TYPE eRevListType = DRM_REVOCATION_LIST_ALL;

    ChkArg( argc == 1 );

    if( strcmp( argv[0], "ALL" ) == 0 )
    {
        eRevListType = DRM_REVOCATION_LIST_ALL;
    }
    else if ( strcmp( argv[0], "RIV" ) == 0 )
    {
        eRevListType = DRM_REVOCATION_LIST_RIV;
    }
    else if ( strcmp( argv[0], "APPS" ) == 0 )
    {
        eRevListType = DRM_REVOCATION_LIST_APPS;
    }
    else if ( strcmp( argv[0], "DEVICE" ) == 0 )
    {
        eRevListType = DRM_REVOCATION_LIST_DEVICE;
    }
    else if ( strcmp( argv[0], "NET" ) == 0 )
    {
        eRevListType = DRM_REVOCATION_LIST_NET;
    }
    else if( argv[0][0] >= '0' && argv[0][0] < '9')
    {
        eRevListType = (DRM_REVOCATION_LIST_TYPE)strtoul(argv[0], NULL, 0);
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    dr = Drm_Ndt_RevocationList_GenerateChallenge( g_pNdtDrmContext,
                                                   eRevListType,
                                                   NULL,
                                                   g_pcbRevocationListRequest );
    if(g_NoHandleBufferTooSmall == 0 && dr == DRM_E_BUFFERTOOSMALL)
    {
        SAFE_OEM_FREE( g_pbRevocationListRequest );
        ChkMem( g_pbRevocationListRequest = (DRM_BYTE*)Oem_MemAlloc( *g_pcbRevocationListRequest ) ) ;
        dr = Drm_Ndt_RevocationList_GenerateChallenge( g_pNdtDrmContext,
                                                       eRevListType,
                                                       g_pbRevocationListRequest,
                                                       g_pcbRevocationListRequest );
    }
    ChkDR( dr );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_NDTMGR_ProcessRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr                     = DRM_SUCCESS;
    DRM_BYTEBLOB RevocationListResponse = { 0 };

    RevocationListResponse.pbBlob = g_pbRevocationListResponse;
    RevocationListResponse.cbBlob = g_cbRevocationListResponse;

    if ( g_MessageProtocolVersionOverride )
    {
        PUT_BYTE( RevocationListResponse.pbBlob,
                  0,
                  g_MessageProtocolVersionOverride);
    }
    if ( g_MessageTypeOverride )
    {
        PUT_BYTE( RevocationListResponse.pbBlob, 1, g_MessageTypeOverride);
    }

    ChkDR( Drm_Ndt_RevocationList_ProcessResponse( g_pNdtDrmContext, RevocationListResponse ) );
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

DRM_RESULT DRM_CALL Test_InitializeRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1);

    g_cRevocationLists = strtoul( argv[0], NULL, 0 );
    g_cbRevocationListResponse = 0;
    SAFE_OEM_FREE(g_pbRevocationListResponse);
    SAFE_OEM_FREE(g_pRevocationLists);
    if(g_cRevocationLists != 0)
    {
        g_pRevocationLists = (DRM_RVK_LIST *)Oem_MemAlloc(g_cRevocationLists * SIZEOF(DRM_RVK_LIST ) );
        ChkMem( g_pRevocationLists );
        ZEROMEM( g_pRevocationLists, g_cRevocationLists * SIZEOF(DRM_RVK_LIST ) );
    }
    g_dwRevocationList = 0;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_AddRevocationListToResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                dr               = DRM_SUCCESS;
    DRM_BYTE                 *pbRevocationList = NULL;
    DRM_DWORD                 cbRevocationList = 0;
    DRM_CONST_STRING          dstrSource       = EMPTY_DRM_STRING;
    DRM_REVOCATION_TYPE_ENUM  eRevType         = DRM_REVOCATION_TYPE_WMDRM_APP;
    DRM_GUID                  guid             = { 0 };
    DRM_GUID                 *pGuid            = &guid;
    DRM_DWORD                 cbDecoded        = 0;
    DRM_BOOL                  fDecode          = TRUE;

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

    ChkDR( OpenFileIntoBuffer( argv[1], &pbRevocationList, &cbRevocationList, 0 ) );

    if(fDecode && cbRevocationList != 0)
    {
        dstrSource.pwszString = (DRM_WCHAR *) pbRevocationList;
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

DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListResponseMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD dr = DRM_SUCCESS;

    dr = WmdrmNetPackRevocationListResponseMessage(g_cRevocationLists, g_pRevocationLists, g_pbRevocationListResponse, &g_cbRevocationListResponse );
    if(dr == DRM_E_BUFFERTOOSMALL)
    {
        SAFE_OEM_FREE(g_pbRevocationListResponse);
        ChkMem( g_pbRevocationListResponse = (DRM_BYTE*)Oem_MemAlloc( g_cbRevocationListResponse ) );
        dr = WmdrmNetPackRevocationListResponseMessage(g_cRevocationLists, g_pRevocationLists, g_pbRevocationListResponse, &g_cbRevocationListResponse );
    }
    ChkDR( dr );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListRequestMessage(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD dr                    = DRM_SUCCESS;
    DRM_DWORD cguidsRevocationLists = 0;
    DRM_DWORD i                     = 0;
    DRM_GUID *pguidsRevocationLists = NULL;

    cguidsRevocationLists = argc;
    if(cguidsRevocationLists)
    {
        ChkMem( pguidsRevocationLists = (DRM_GUID *)Oem_MemAlloc( DRM_ID_SIZE * cguidsRevocationLists ) );
        for(i = 0; i < cguidsRevocationLists; i++)
        {
            if(DRM_FAILED(ParseGUID( &pguidsRevocationLists[i], (DRM_BYTE *) argv[i] )))
            {
                TRACE(("unable to parse GUID argument %d\n", i + 1 ));
                ChkDR( DRM_E_INVALIDARG );
            }
        }
    }

    dr = WmdrmNetPackRevocationListRequestMessage(
             cguidsRevocationLists,
             pguidsRevocationLists,
             NULL,
             g_pcbRevocationListRequest );
    if(g_NoHandleBufferTooSmall == 0 && dr == DRM_E_BUFFERTOOSMALL)
    {
        SAFE_OEM_FREE( g_pbRevocationListRequest );
        g_pbRevocationListRequest = (DRM_BYTE*)Oem_MemAlloc( *g_pcbRevocationListRequest );
        ChkMem( g_pbRevocationListRequest );
        dr = WmdrmNetPackRevocationListRequestMessage(
                 cguidsRevocationLists,
                 pguidsRevocationLists,
                 g_pbRevocationListRequest,
                 g_pcbRevocationListRequest );
    }
    ChkDR( dr );

ErrorExit:
    SAFE_OEM_FREE(pguidsRevocationLists);
    return dr;
}

DRM_RESULT DRM_CALL Test_DumpRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT               dr               = DRM_SUCCESS;
    DRM_DWORD                i                = 0;
    DRM_DWORD                cRevocationLists = 0;
    DRM_RVK_LIST            *pRevocationLists = NULL;
    DRM_REVOCATION_TYPE_ENUM eRevType         = DRM_REVOCATION_TYPE_WMDRM_APP;

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
            dr = _DrmRevocationTypeEnumFromGUID (
                       &pRevocationLists[i].guidRevocationList,
                       &eRevType );
            if(DRM_SUCCEEDED(dr ))
            {
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
            else
            {
                TRACE(("Unknown guid"));
                dr = DRM_SUCCESS;
            }
        }
        TRACE((": %lu bytes\n", pRevocationLists[i].cbRevocationList));
    }
ErrorExit:
    for(i = 0; i < cRevocationLists; i++)
    {
        SAFE_OEM_FREE( pRevocationLists[i].pbRevocationList );
    }
    SAFE_OEM_FREE( pRevocationLists );

    return dr;
}

DRM_RESULT DRM_CALL Test_NDTMGR_ProcessRevocationListRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD    dr                    = DRM_SUCCESS;
    DRM_BYTEBLOB RevocationListRequest = { 0 };

    RevocationListRequest.pbBlob = g_pbRevocationListRequest;
    RevocationListRequest.cbBlob = g_cbRevocationListRequest;
    if ( g_MessageProtocolVersionOverride )
    {
        PUT_BYTE( RevocationListRequest.pbBlob,
                  0,
                  g_MessageProtocolVersionOverride);
    }
    if ( g_MessageTypeOverride )
    {
        PUT_BYTE( RevocationListRequest.pbBlob, 1, g_MessageTypeOverride);
    }
    g_cbRevocationListResponse = 0;
    dr = Drm_Ndt_RevocationList_ProcessRequest( g_pNdtDrmContext,
                                                RevocationListRequest,
                                                NULL,
                                                g_pcbRevocationListResponse );
    if(g_NoHandleBufferTooSmall == 0 && dr == DRM_E_BUFFERTOOSMALL)
    {
        SAFE_OEM_FREE( g_pbRevocationListResponse );
        g_pbRevocationListResponse = (DRM_BYTE*)Oem_MemAlloc( *g_pcbRevocationListResponse);
        ChkMem( g_pbRevocationListResponse );
        dr = Drm_Ndt_RevocationList_ProcessRequest( g_pNdtDrmContext,
                                                    RevocationListRequest,
                                                    g_pbRevocationListResponse,
                                                    g_pcbRevocationListResponse );
    }
    ChkDR( dr );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL Test_SetNoHandleBufferTooSmall( long argc, __in_ecount(argc) char *argv[] )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );

    g_NoHandleBufferTooSmall = strtoul ( argv[0], NULL, 0 );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_CheckcbRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  dwVal = 0;

    ChkArg( argc == 1 );

    dwVal = strtoul( argv[0], NULL, 0 );
    ChkBOOL( dwVal == g_cbRevocationListResponse, DRM_E_FAIL );
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_SetcbRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  dwVal = 0;

    ChkArg( argc == 1 );

    dwVal = strtoul( argv[0], NULL, 0 );
    g_cbRevocationListResponse = dwVal;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_SetcbRevocationListRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  dwVal = 0;

    ChkArg( argc == 1 );

    dwVal = strtoul( argv[0], NULL, 0 );
    g_cbRevocationListRequest = dwVal;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_SetpcbRevocationListRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  dwVal = 0;

    ChkArg( argc == 1 );

    dwVal = strtoul( argv[0], NULL, 0 );
    g_pcbRevocationListRequest = (DRM_DWORD *)dwVal;
ErrorExit:
    return dr;
}
DRM_RESULT DRM_CALL Test_SetpcbRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  dwVal = 0;

    ChkArg( argc == 1 );

    dwVal = strtoul( argv[0], NULL, 0 );
    g_pcbRevocationListResponse = (DRM_DWORD *)dwVal;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_VerifyRevocationListResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_DWORD                dr               = DRM_SUCCESS;
    DRM_DWORD                i                = 0;
    DRM_DWORD                j                = 0;
    DRM_DWORD                cRevocationLists = 0;
    DRM_RVK_LIST            *pRevocationLists = NULL;
    DRM_REVOCATION_TYPE_ENUM eRevType         = DRM_REVOCATION_TYPE_WMDRM_APP;
    DRM_DWORD                dwMatched        = 0;

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

    ChkMem(pRevocationLists);
    for(i = 0; i < cRevocationLists; i++)
    {
#ifndef __TMS320C55X__
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
#endif

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
    for(i = 0; i < cRevocationLists; i++)
    {
        SAFE_OEM_FREE( pRevocationLists[i].pbRevocationList );
    }
    SAFE_OEM_FREE( pRevocationLists );
    return dr;
}

DRM_RESULT DRM_CALL Test_CheckRivSequenceNumber(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                  dr                = DRM_SUCCESS;
    WMDRMNET_MESSAGE_DESCRIPTOR MessageDescriptor = { 0 };
    DRM_DWORD                   cbRevInfo         = 0;
    DRM_BYTE                   *pbRevInfo         = NULL;
    DRM_DWORD                   dwId              = 0;
    DRM_DWORD                   dwCheckArg        = 0;
    DRM_DWORD                   dwLength          = 0;
    DRM_DWORD                   dwFormat          = 0;
    DRM_DWORD                   dwSeqNum          = 0;
    DRM_APP_CONTEXT            *pDrmMgrContext    = GetCurrentAppContext();
    DRM_STRING                  dstrSource        = EMPTY_DRM_STRING;
    DRM_DWORD                   cbDecoded         = 0;

    ChkArg( argc == 1 );

    dwCheckArg = strtoul( argv[0], NULL, 0 );

    dr = Drm_Revocation_GetList( pDrmMgrContext, DRM_REVOCATION_TYPE_WMDRM_REVINFO, NULL, &cbRevInfo, NULL );
    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbRevInfo = (DRM_BYTE*)Oem_MemAlloc( cbRevInfo ) );
        dr = Drm_Revocation_GetList( pDrmMgrContext, DRM_REVOCATION_TYPE_WMDRM_REVINFO, pbRevInfo, &cbRevInfo, NULL );
    }
    ChkDR( dr );

    dstrSource.pwszString = (DRM_WCHAR *) pbRevInfo;
    dstrSource.cchString = cbRevInfo / 2;
    ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *) &dstrSource, &cbDecoded, NULL, DRM_BASE64_DECODE_IN_PLACE ) );

    MessageDescriptor.Buffer = pbRevInfo;
    MessageDescriptor.BufferSize = cbDecoded;
    MessageDescriptor.Offset = 0;

    ChkBOOL( WmdrmNetUnpackDword( &MessageDescriptor, &dwId ), DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( dwId == 0x524c5649, DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( WmdrmNetUnpackDword( &MessageDescriptor, &dwLength ), DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( WmdrmNetUnpackDword( &MessageDescriptor, &dwFormat ), DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( WmdrmNetUnpackDword( &MessageDescriptor, &dwSeqNum ), DRM_E_ND_INVALID_MESSAGE );
    ChkBOOL( dwSeqNum == dwCheckArg, DRM_E_FAIL );

ErrorExit:
    SAFE_OEM_FREE( pbRevInfo );
    return dr;

}

DRM_RESULT DRM_CALL Test_CopyFile(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT    dr                          = DRM_SUCCESS;
    DRM_CONST_STRING dstrSource               = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDest                 = EMPTY_DRM_STRING;

    ChkArg( argc == 2 && argv[0] && argv[1] );

    ChkDR( MakeDRMString(&dstrSource, argv[0]) );
    ChkDR( MakeDRMString(&dstrDest, argv[1] ) );

    ChkDR( TST_OEM_CopyFile( dstrSource, dstrDest ) );

ErrorExit:
    FREEDRMSTRING( dstrSource );
    FREEDRMSTRING( dstrDest );
    return dr;
}

DRM_RESULT DRM_CALL Test_NDT_RetrieveLicenseDataV2(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT                       dr                        = DRM_SUCCESS;
    NDTMGR_INTERNAL_SESSION_CONTEXT *pContSessContext          = NULL;
    DRM_STRING                       dstrAction                = EMPTY_DRM_STRING;
    DRM_BYTE                        *pbContentEncryptionObject = NULL;
    DRM_DWORD                        cbContentEncryptionObject = 0;
    DRM_WCHAR                        wszAction[256]            = { 0 };
    DRM_SUBSTRING                    dsubstr                   = EMPTY_DRM_SUBSTRING;

    ChkDR( OpenFileIntoBuffer( argv[0], &pbContentEncryptionObject, &cbContentEncryptionObject, 0 ) );
#ifdef __TMS320C55X__
    {
        int i;
        for(i = 0; i < strlen(argv[1]); i++)
        {
            PUT_BYTE(argv[1], i, argv[1][i]);
        }
    }
#endif
    dsubstr.m_ich = 0;
    if( argc < 2 )
    {
        ChkDR( DRM_E_INVALIDARG);
    }

    dsubstr.m_cch = ( DRM_DWORD ) strlen(argv[1]);
    dstrAction.pwszString = wszAction;
    dstrAction.cchString = dsubstr.m_cch;
    DRM_UTL_PromoteASCIItoUNICODE(argv[1], &dsubstr, &dstrAction);

    if ( dstrAction.cchString < NO_OF( wszAction ) )
    {
        wszAction[dstrAction.cchString] = 0;
    }
    else
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( _FindSession( g_pNdtDrmContext,
                         g_MediaSessionID,
                         NDTMGR_SESSION_TYPE_CONTENT,
                         &pContSessContext ) );
    ChkDR( NDT_RetrieveLicenseDataV2( (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *)pContSessContext, pbContentEncryptionObject, cbContentEncryptionObject, (DRM_CONST_STRING *)&dstrAction ) );

ErrorExit:
    SAFE_OEM_FREE( pbContentEncryptionObject );
    return dr;
}

DRM_RESULT DRM_CALL Test_SetFailAllocation(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);

    g_fFailAllocation = (DRM_DWORD) strtoul(argv[0], NULL, 0);

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(NDTTestSuite)(long lTCID, const char *strTCName)
{
    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        DRM_RESULT dr = DRM_SUCCESS;
        int        i  = 0;
        DRM_CONST_STRING dstrSourceHDS = {0};
        DRM_CONST_STRING dstrDestHDS = {0};
        DRM_CONST_STRING dstrSourceCocktailHDS = {0};
        DRM_CONST_STRING dstrDestCocktailHDS = {0};

        /*
        ** Zero the array of potentially usable contexts.
        */
        ZEROMEM( g_NdtContexts, SIZEOF( ND_TRANSMITTER_CONTEXT ) * MAXNDTCONTEXTS );

        /*
        ** Setup the first context.
        */
        g_pCurNdtContext = &g_NdtContexts[0];
        g_pCurNdtContext->pNdtMgrContext = (NDTMGR_CONTEXT *) Oem_MemAlloc(SIZEOF(NDTMGR_CONTEXT));
        ChkMem( g_pCurNdtContext->pNdtMgrContext );

        /*
        ** Extract the overriddable context pointer.
        */
        g_pNdtDrmContext = g_pCurNdtContext->pNdtMgrContext;

        for ( i = 0; i < MAXMEDIASESSIONS; i++ )
        {
            g_hOutputFiles[i] = OEM_INVALID_HANDLE_VALUE;
            g_pXmrLicenses[i] = NULL;
            g_pContentEncryptionKeys[i] = NULL;
            g_pRootContentEncryptionKeys[i] = NULL;
        }
        ZEROMEM( g_KIDs, SIZEOF( DRM_ID ) * MAXMEDIASESSIONS );
        ZEROMEM( g_RootXmrLicenseInfos, SIZEOF( XMR_ROOT_LICENSE_INFO ) * MAXMEDIASESSIONS );
        ZEROMEM( g_xmrMinEnvironments, SIZEOF( XMR_GLOBAL_REQUIREMENTS ) * MAXMEDIASESSIONS );
        ZEROMEM( g_MediaSessionIDs, SIZEOF( DRM_ID ) * MAXMEDIASESSIONS );
        ZEROMEM( &g_MediaSessionID, SIZEOF( DRM_ID ) );
        g_CurrentMediaSession = 0;

        g_cbRegistrationRequestMsgOverride = 0;
        g_pbRegistrationRequestMsgNULL = 0;
        g_cbRegistrationResponseMsgOverride = 0;
        g_pbRegistrationResponseMsgNULL = 0;
        g_cbRegistrationResponseMsg = 1024;
        g_ChallengeIDOverride = 0;
        g_fProximityStartMsgNULL = FALSE;
        g_fProximityResponseMsgNULL = FALSE;
        g_fTestProximityBUFFERTOOSMALL = FALSE;
        g_fMediaSessionIDNULL = FALSE;
        g_fASFFlagsNULL = FALSE;
        g_dwASFFlags = 0;
        g_fpbDataNULL = FALSE;
        g_fpcbDataNULL = FALSE;
        g_fpbHeaderNULL = FALSE;
        g_fpcbHeaderNULL = FALSE;
        g_cbHeaderOverride = -1;
        g_fpbLeafLicenseResponseNULL = FALSE;
        g_fpcbLeafLicenseResponseNULL = FALSE;
        g_cbLeafLicenseResponseOverride = -1;
        g_cbActionOverride = 0;
        g_cbLicenseChallengeMsgOverride = 0;
        g_cbLicenseResponseMsgOverride = -1;
        g_cbLicenseResponseMsgNULL = 0;
        g_pbLicenseResponseMsgNULL = 0;
        g_pWMDRMNETPolicy = NULL;
        g_fpKIDNULL = FALSE;
        g_fpSampleInfoNULL = FALSE;
        g_fpbSampleDataNULL = FALSE;
        g_fpcbSampleDataNULL = FALSE;
        g_cbSampleDataOverride = -1;

        g_MessageTypeOverride = 0;
        g_MessageProtocolVersionOverride = 0;
        g_LastDataBlock = 0;
        g_fAllowNewDeviceRegistration = TRUE;
        g_CryptoDataNULL = 0;
        g_EncryptionMode = 0;

        g_dwDataPacketSize = 0;

        g_cbRevocationListRequest = 0;
        g_pcbRevocationListRequest = &g_cbRevocationListRequest;

        g_cbRevocationListResponse = 0;
        g_pcbRevocationListResponse = &g_cbRevocationListResponse;

        g_NoHandleBufferTooSmall = 0;
        g_qwFirstPacketOffset = DRM_UI64( 0 );

        ZEROMEM(g_FilePayloadFragments, SIZEOF(g_FilePayloadFragments));
        ZEROMEM( &g_aryPtrListClearFileSamples, SIZEOF( PTR_LIST ) * (MAX_STREAMS_IN_ASF + 1) );

        ChkDR( MakeDRMString(&dstrSourceHDS, "files\\sample.hds") );
        ChkDR( MakeDRMString(&dstrDestHDS, "sample.hds" ) );
        ChkDR( MakeDRMString(&dstrSourceCocktailHDS, "files\\cocktail_sample.hds") );
        ChkDR( MakeDRMString(&dstrDestCocktailHDS, "cocktail_sample.hds" ) );

        ChkDR( TST_OEM_CopyFile( dstrSourceHDS, dstrDestHDS ) );
        ChkDR( TST_OEM_CopyFile( dstrSourceCocktailHDS, dstrDestCocktailHDS ) );
        ChkDR( PreTestCase_OEM_DeviceStore( lTCID, strTCName ) );

        ChkDR( OEM_RSA_SetPrivateKey( eDRM_RSA_1024,
                                          SIZEOF( g_rgbRSAPrivExp ),
                                          g_rgbRSAPrivExp,
                                          SIZEOF( g_rgbRSAPrimes[0] ),
                                          g_rgbRSAPrimes[0],
                                          g_rgbRSAPrimes[1],
                                          &g_PrivateKey,
                                          &g_Crypto ) );

ErrorExit:
        return dr;
    }
    else
    {
        return DRM_E_NOTIMPL;
    }
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(NDTTestSuite)(long lTCID, const char *strTCName)
{
    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        DRM_DWORD         i                  = 0;

        /* Reverse any time changes in the test case.*/
        tResetSystemTimeOffset();

        g_fFailAllocation = 0;

        PostTestCase_OEM_DeviceStore( lTCID, strTCName ) ;

        for(i = 0; i < MAXNDTCONTEXTS; i++)
        {
            TestCleanupList( &g_NdtContexts[i] );

            if(g_NdtContexts[i].fInitialized)
            {
                DRMASSERT( g_NdtContexts[i].pNdtMgrContext != NULL );
                Drm_Ndt_Uninitialize( g_NdtContexts[i].pNdtMgrContext );
                TestDeleteAppContext( g_NdtContexts[i].DrmMgrContext, g_NdtContexts[i].fSharedAppContext  );
                // This check is needed in case of Shared App Context since multiple device stores are not created.
                if ( !(g_NdtContexts[i].fSharedAppContext))
                {
                    // This deletes the additional .hds files created during tests requiring multipled device stores
                    tRemoveFileW(g_NdtContexts[i].dstrInitData.pwszString);
                }

                FREEDRMSTRING( g_NdtContexts[i].dstrInitData );

                g_NdtContexts[i].fInitialized = FALSE;
                g_NdtContexts[i].fSharedAppContext = FALSE;
            }
            SAFE_OEM_FREE( g_NdtContexts[i].pNdtMgrContext );
        }

        SAFE_OEM_FREE( g_pCertificate );

        if ( g_hOutputFile != OEM_INVALID_HANDLE_VALUE )
        {
            Oem_File_Close( g_hOutputFile );
            g_hOutputFile = OEM_INVALID_HANDLE_VALUE;
        }

        OEM_RSA_ZeroPrivateKey(&g_PrivateKey);

        if ( g_pContentEncryptionKey != g_pRootContentEncryptionKey && NULL != g_pContentEncryptionKey )
        {
            Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
        }
        if ( NULL != g_pRootContentEncryptionKey )
        {
            Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pRootContentEncryptionKey);
            g_pRootContentEncryptionKey = NULL;
        }
        g_pContentEncryptionKey = NULL;

        MEMSET( &g_Crypto, 0, SIZEOF( DRM_CRYPTO_CONTEXT ) );

        if ( g_pXmrLicense != g_RootXmrLicenseInfo.pXmrLicense )
        {
            DrmXmrFreeLicense(g_pXmrLicense);
        }
        DrmXmrFreeLicense(g_RootXmrLicenseInfo.pXmrLicense);
        g_RootXmrLicenseInfo.pXmrLicense = NULL;
        g_pXmrLicense = NULL;
        SAFE_OEM_FREE(g_RootXmrLicenseInfo.pIDChainedChecksum);
        if ( NULL != g_RootXmrLicenseInfo.pAesContentKey )
        {
            Oem_Aes_ZeroKey(g_RootXmrLicenseInfo.pAesContentKey);
            g_RootXmrLicenseInfo.pAesContentKey = NULL;
        }

        for ( i = 0; i < MAXMEDIASESSIONS; i++ )
        {
            if ( i == g_CurrentMediaSession )
            {
                continue;
            }

            if ( g_hOutputFiles[i] != OEM_INVALID_HANDLE_VALUE )
            {
                Oem_File_Close( g_hOutputFiles[i] );
                g_hOutputFiles[i] = OEM_INVALID_HANDLE_VALUE;
            }

            if ( g_pContentEncryptionKeys[i] != g_pRootContentEncryptionKeys[i] && NULL != g_pContentEncryptionKeys[i] )
            {
                Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKeys[i]);
            }
            if ( NULL != g_pRootContentEncryptionKeys[i] )
            {
                Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pRootContentEncryptionKeys[i]);
                g_pRootContentEncryptionKeys[i] = NULL;
            }
            g_pContentEncryptionKeys[i] = NULL;

            if ( g_pXmrLicenses[i] != g_RootXmrLicenseInfos[i].pXmrLicense )
            {
                DrmXmrFreeLicense(g_pXmrLicenses[i]);
            }

            DrmXmrFreeLicense(g_RootXmrLicenseInfos[i].pXmrLicense);
            g_RootXmrLicenseInfos[i].pXmrLicense = NULL;
            g_pXmrLicenses[i] = NULL;
            SAFE_OEM_FREE(g_RootXmrLicenseInfos[i].pIDChainedChecksum);
            if ( NULL != g_RootXmrLicenseInfos[i].pAesContentKey )
            {
                Oem_Aes_ZeroKey(g_RootXmrLicenseInfos[i].pAesContentKey);
                g_RootXmrLicenseInfos[i].pAesContentKey = NULL;
            }
        }

        SAFE_OEM_FREE(g_pbRevocationListRequest);
        SAFE_OEM_FREE(g_pbRevocationListResponse);

        if(g_pRevocationLists)
        {
            for(i = 0; i < g_cRevocationLists; i++)
            {
                SAFE_OEM_FREE( g_pRevocationLists[i].pbRevocationList );
            }
        }
        SAFE_OEM_FREE( g_pRevocationLists );
        for(i = 0; i < NO_OF(g_FilePayloadFragments); i++)
        {
            SAFE_OEM_FREE(g_FilePayloadFragments[i].pbData);
        }
        CloseClearSampleFile();
        // This deletes all the cert and dat files from the DRM Folder
        RemoveDRMFile(RMFILE_ALL);
        return DRM_SUCCESS;
    }
    else
    {
        return DRM_E_NOTIMPL;
    }
}

BEGIN_TEST_API_LIST(NDTTestSuite)
    API_ENTRY(Test_SetpNdtDrmContext)
    API_ENTRY(Test_SetCertificate)
    API_ENTRY(Test_SetcbCertificate)
    API_ENTRY(Test_SetRegistrationSessionID)
    API_ENTRY(Test_SetMediaSessionID)
    API_ENTRY(Test_SetMediaSessionIDNULL)
    API_ENTRY(Test_SetASFFlagsNULL)
    API_ENTRY(Test_SetPrivateKey)
    API_ENTRY(Test_SetPrivateKeyDirectly)
    API_ENTRY(Test_SetSerialNumber)
    API_ENTRY(Test_SetChallengeID)
    API_ENTRY(Test_SetcbRegistrationRequestMsg)
    API_ENTRY(Test_SetcbRegistrationResponseMsg)
    API_ENTRY(Test_SetcbLicenseChallengeMsg)
    API_ENTRY(Test_SetcbLicenseResponseMsg)
    API_ENTRY(Test_SetpbDataNULL)
    API_ENTRY(Test_SetpcbDataNULL)
    API_ENTRY(Test_SetpbSampleDataNULL)
    API_ENTRY(Test_SetpcbSampleDataNULL)
    API_ENTRY(Test_SetcbSampleData)
    API_ENTRY(Test_SetpSampleInfoNULL)
    API_ENTRY(Test_SetpbHeaderNULL)
    API_ENTRY(Test_SetpcbHeaderNULL)
    API_ENTRY(Test_SetcbHeader)
    API_ENTRY(Test_SetpbLeafLicenseResponseNULL)
    API_ENTRY(Test_SetpcbLeafLicenseResponseNULL)
    API_ENTRY(Test_SetcbLeafLicenseResponse)
    API_ENTRY(Test_SetpKIDNULL)
    API_ENTRY(Test_SetWMDRMNETPolicy)
    API_ENTRY(Test_SetMessageType)
    API_ENTRY(Test_SetMessageProtocolVersion)
    API_ENTRY(Test_ChangeTimeMilliseconds)
    API_ENTRY(Test_SetRevocationList)
    API_ENTRY(Test_CheckASFFlags)
    API_ENTRY(Test_CheckLastDataBlock)
    API_ENTRY(Test_CheckNotTranscrypted)
    API_ENTRY(Test_CheckSourceID)
    API_ENTRY(Test_CheckRivInLicense)
    API_ENTRY(Test_CheckDeviceKey)
    API_ENTRY(Test_CheckSettingsObject)
    API_ENTRY(Test_CheckInclusionList)
    API_ENTRY(Test_NDTMGR_ASF_Init)
    API_ENTRY(Test_NDTMGR_ASF_InitTranscryption)
    API_ENTRY(Test_NDTMGR_ASF_Close)
    API_ENTRY(Test_NDTMGR_ASF_ReadData)
    API_ENTRY(Test_NDTMGR_ASF_ReadAllData)
    API_ENTRY(Test_NDTMGR_ASF_ReadSample)
    API_ENTRY(Test_NDTMGR_ASF_ReadAllSamples)
    API_ENTRY(Test_NDTMGR_ASF_Seek)
    API_ENTRY(Test_NDTMGR_ASF_GetLeafLicenseResponse)
    API_ENTRY(Test_NDTMGR_ASF_GetHeader)
    API_ENTRY(Test_DRM_NDT_Initialize)
    API_ENTRY(Test_DRM_NDT_Uninitialize)
    API_ENTRY(Test_SwitchNdtContexts)
    API_ENTRY(Test_SwitchMediaSession)
    API_ENTRY(Test_NDTMGR_CreateSession)
    API_ENTRY(Test_NDTMGR_CloseSession)
    API_ENTRY(Test_NDTMGR_ProcessRegistrationRequest)
    API_ENTRY(Test_NDTMGR_ProcessProximityStartMsg)
    API_ENTRY(Test_NDTMGR_ProcessProximityResponseMsg)
    API_ENTRY(Test_NDTMGR_ProcessLicenseChallenge)
    API_ENTRY(Test_NDTMGR_GetLeafLicenseResponse)
    API_ENTRY(Test_SetCryptoDataNULL)
    API_ENTRY(Test_SetEncryptionMode)
    API_ENTRY(Test_NDTMGR_Encrypt_ASF)
    API_ENTRY(Test_NDTMGR_Encrypt_MPEG)
    API_ENTRY(Test_LinkEncryption)
    API_ENTRY(Test_SetTestProximityBUFFERTOOSMALL)
    API_ENTRY(Test_SetProximityStartMsgNULL)
    API_ENTRY(Test_SetProximityResponseMsgNULL)
    API_ENTRY(Test_Proximity)
    API_ENTRY(Test_SetpbRegistrationRequestMsgNULL)
    API_ENTRY(Test_SetpbRegistrationResponseMsgNULL)
    API_ENTRY(Test_SetcbLicenseResponseMsgNULL)
    API_ENTRY(Test_SetpbLicenseResponseMsgNULL)
    API_ENTRY(Test_LinkEncryption)
    API_ENTRY(Test_SetProximityDetectionAddress)
    API_ENTRY(Test_AllowNewDeviceRegistration)
    API_ENTRY(Test_GetHeaders)
    API_ENTRY(Test_NDTMGR_ProcessRevocationListResponse)
    API_ENTRY(Test_NDTMGR_GetRevocationListRequest)
    API_ENTRY(Test_AddRevocationListToResponse)
    API_ENTRY(Test_InitializeRevocationListResponse)
    API_ENTRY(Test_WmdrmNetPackRevocationListResponseMessage)
    API_ENTRY(Test_WmdrmNetPackRevocationListRequestMessage)
    API_ENTRY(Test_NDTMGR_ProcessRevocationListRequest)
    API_ENTRY(Test_SetNoHandleBufferTooSmall)
    API_ENTRY(Test_SetcbRevocationListRequest)
    API_ENTRY(Test_SetpcbRevocationListRequest)
    API_ENTRY(Test_SetcbRevocationListResponse)
    API_ENTRY(Test_SetpcbRevocationListResponse)
    API_ENTRY(Test_CheckcbRevocationListResponse)
    API_ENTRY(Test_VerifyRevocationListResponse)
    API_ENTRY(Test_CheckRivSequenceNumber)
    API_ENTRY(Test_CopyFile)
    API_ENTRY(Test_DumpRevocationListResponse)
    API_ENTRY(Test_DRM_DeviceStore_FreeDevice)
    API_ENTRY(Test_DRM_DeviceStore_UnRegisterDevice)
    API_ENTRY(Test_DRM_DeviceStore_GetFirstDevice)
    API_ENTRY(Test_DRM_DeviceStore_GetNextDevice)
    API_ENTRY(Test_DRM_DeviceStore_GetDeviceByID)
    API_ENTRY(Test_DRM_DeviceStore_GetDeviceInfo)
    API_ENTRY(Test_DRM_DeviceStore_GetDeviceAttr)
    API_ENTRY(Test_DRM_DeviceStore_GetDeviceAttrCount)
    API_ENTRY(Test_DRM_DeviceStore_GetDeviceAttrByIndex)
    API_ENTRY(Test_DRM_DeviceStore_SetDeviceAttr)
    API_ENTRY(Test_DRM_DeviceStore_IsDeviceValid)
    API_ENTRY(Test_NDT_RetrieveLicenseDataV2)
    API_ENTRY(Test_SetFailAllocation)

#ifdef MEMORY_LEAK_DEBUG

    {NULL, NULL}};

    int iReturn;
    iReturn = DoTest(argc, argv, g_rgoTestScriptList, apiMap, PreTestCase, PostTestCase, WrapTestRun);

    if (_CrtDumpMemoryLeaks() == TRUE)
    {
        TRACE(("\nMemory Leak Detected!"));
    }

    return iReturn;
}

#else

END_TEST_API_LIST

#endif /* MEMORY_LEAK_DEBUG */

EXIT_PKTEST_NAMESPACE_CODE
