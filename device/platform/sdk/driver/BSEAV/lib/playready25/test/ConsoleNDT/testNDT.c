/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tclib.h>
#include <drmcommon.h>
#include <drmxmlparser.h>
#include <drmwmdrmnet.h>
#include <drmpfdmanager.h>
#include <oemndt.h>
#include <NdtDrmXmr.h>
#include <testNDT.h>
#include <pfddrmmtpextensions.h>
#include <drmutilities.h>
#include <oemaes.h>
#include <ndtrevguids.h>
#include <tOEMIMP.h>
#include <tstutils.h>
#include <ndtnetmsg.h>
#include <ndtreg.h>
#include <drmdevstore.h>
#include <DRMManagerTestAPI.h>
#include <drmtestrsautils.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

#ifndef MAX_PATH
#define MAX_PATH          260
#endif  /* MAX_PATH */

#define MAXIMUM_HEADER_SIZE 8192

const DRM_BYTE g_rgbRSAPrimes[2][__CB_DECL( DRM_RSA_CB_PRIME( eDRM_RSA_KEY_LENGTH_1024_2048BIT ) )] =
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

#define PFDMGR_CONTEXT_DRM_MANAGER_CONTEXT(p) &((PFDMGR_CONTEXT_INTERNAL *)p)->AppContext

#define STRINGIFY(x) #x

PFDMGR_CONTEXT *g_pCorrectPfdDrmContext = NULL;
PFDMGR_CONTEXT *g_pPfdDrmContext = NULL;
int g_Initialized = 0;

char g_cCertificateFilename[MAX_PATH];
DRM_DWORD g_cbCertificate;
DRM_BYTE  *g_pCertificate = NULL;

DRM_RSA_PRIVATE_KEY_2048BIT  *g_pPrivateKey = NULL;
DRM_RSA_PRIVATE_KEY_2048BIT g_PrivateKey = {0};
DRM_AES_KEY  *g_pContentEncryptionKey = NULL;

DRM_WORD g_cbAction = 0;
DRM_WORD g_cbActionOverride = 0;

DRM_DWORD g_dwTransactionID = 0;

DRM_DWORD g_dwMediaSessionID = 0;
DRM_DWORD g_dwCorrectMediaSessionID = 0;

DRM_ID g_SerialNumber = { 0 };
DRM_ID g_drmRightsID = { 0 };
DRM_ID g_Seed = { 0 };

DRM_ID g_RegistrationSessionID = { 0 };

DRM_DWORD g_cbRegistrationResponseMsgOverride = 0;
DRM_DWORD g_cbRegistrationResponseMsg = 1024;
DRM_DWORD g_pbRegistrationResponseMsgNULL = 0;

DRM_DWORD g_cbRegistrationRequestMsgOverride = 0;
DRM_DWORD g_cbRegistrationRequestMsg;

DRM_DWORD g_cbLicenseRequestMsgOverride = 0;
DRM_DWORD g_cbLicenseRequestMsg;

DRM_DWORD g_cbLicenseResponseMsgOverride = MAX_UNSIGNED_TYPE(DRM_DWORD);
DRM_DWORD g_cbLicenseResponseMsg = 1024;
DRM_DWORD g_pbLicenseResponseMsgNULL = 0;

DRM_DWORD g_dwChallenge1 = 0;
DRM_DWORD g_dwChallenge2 = 0;
DRM_DWORD g_dwChallenge3 = 0;
DRM_DWORD g_dwChallenge4 = 0;
DRM_DWORD g_NonceOverride = 0;

DRM_DWORD g_dwControlCapabilities = 0;
DRM_DWORD g_dwDRMStatus = 0;
DRM_DWORD g_dwLicenseStatus = 0;
DRM_DWORD g_dwDRMDeliveryOptions = 0;

DRM_BYTE g_MessageTypeOverride = 0;
DRM_BYTE g_MessageProtocolVersionOverride = 0;

DRM_DWORD g_LastDataBlock = 0;

OEM_FILEHDL g_hOutputFile = OEM_INVALID_HANDLE_VALUE;

DRM_DWORD g_dwEndOfData = 0;

XMR_GLOBAL_REQUIREMENTS g_xmrMinEnvironment;

DRM_DWORD g_dwTimeOffsetMilliseconds = 0;

XMR_LICENSE *g_pXmrLicense = NULL;

DRM_DWORD g_FirstNextDataBlock = 0;
DRM_DWORD g_nDataPackets = 0;
DRM_DWORD g_dwDataPacketSize = 0;

DRM_CONST_STRING g_dstrHdsPath = EMPTY_DRM_STRING;

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

/* had to copy this function from pfddrmmanager.c because it's a static in there */
static DRM_VOID PackDWORDSInDRMID(
    DRM_ID *pId,
    DRM_DWORD dw1,
    DRM_DWORD dw2,
    DRM_DWORD dw3,
    DRM_DWORD dw4 )
{
    /* No reason to use math safe - we're just converting each DWORD to an array of 4 bytes. */
    PUT_BYTE( pId->rgb,  0, (DRM_BYTE)( dw1 >> 24));
    PUT_BYTE( pId->rgb,  1, (DRM_BYTE)( dw1 >> 16) );
    PUT_BYTE( pId->rgb,  2, (DRM_BYTE)( dw1 >> 8) );
    PUT_BYTE( pId->rgb,  3, (DRM_BYTE)( dw1 ) );

    PUT_BYTE( pId->rgb,  4, (DRM_BYTE)( dw2 >> 24) );
    PUT_BYTE( pId->rgb,  5, (DRM_BYTE)( dw2 >> 16) );
    PUT_BYTE( pId->rgb,  6, (DRM_BYTE)( dw2 >> 8) );
    PUT_BYTE( pId->rgb,  7, (DRM_BYTE)( dw2 ) );

    PUT_BYTE( pId->rgb,  8, (DRM_BYTE)( dw3 >> 24) );
    PUT_BYTE( pId->rgb,  9, (DRM_BYTE)( dw3 >> 16) );
    PUT_BYTE( pId->rgb, 10, (DRM_BYTE)( dw3 >> 8) );
    PUT_BYTE( pId->rgb, 11, (DRM_BYTE)( dw3 ) );

    PUT_BYTE( pId->rgb, 12, (DRM_BYTE)( dw4 >> 24) );
    PUT_BYTE( pId->rgb, 13, (DRM_BYTE)( dw4 >> 16) );
    PUT_BYTE( pId->rgb, 14, (DRM_BYTE)( dw4 >> 8) );
    PUT_BYTE( pId->rgb, 15, (DRM_BYTE)( dw4 ) );
}

/* had to copy this function from pfddrmmanager.c because it's a static in there */
static DRM_VOID UnpackDWORDSFromDRMID(
    const DRM_ID *pId,
    DRM_DWORD *pdw1,
    DRM_DWORD *pdw2,
    DRM_DWORD *pdw3,
    DRM_DWORD *pdw4 )
{
  *pdw1 = GET_BYTE(pId->rgb,  0);
  *pdw1 <<= 8;
  *pdw1 += GET_BYTE(pId->rgb, 1);
  *pdw1 <<= 8;
  *pdw1 += GET_BYTE(pId->rgb, 2);
  *pdw1 <<= 8;
  *pdw1 += GET_BYTE(pId->rgb, 3);

  *pdw2 = GET_BYTE(pId->rgb,  4);
  *pdw2 <<= 8;
  *pdw2 += GET_BYTE(pId->rgb, 5);
  *pdw2 <<= 8;
  *pdw2 += GET_BYTE(pId->rgb, 6);
  *pdw2 <<= 8;
  *pdw2 += GET_BYTE(pId->rgb, 7);

  *pdw3 = GET_BYTE(pId->rgb,  8);
  *pdw3 <<= 8;
  *pdw3 += GET_BYTE(pId->rgb, 9);
  *pdw3 <<= 8;
  *pdw3 += GET_BYTE(pId->rgb, 10);
  *pdw3 <<= 8;
  *pdw3 += GET_BYTE(pId->rgb, 11);

  *pdw4 = GET_BYTE(pId->rgb,  12);
  *pdw4 <<= 8;
  *pdw4 += GET_BYTE(pId->rgb, 13);
  *pdw4 <<= 8;
  *pdw4 += GET_BYTE(pId->rgb, 14);
  *pdw4 <<= 8;
  *pdw4 += GET_BYTE(pId->rgb, 15);
}


static DRM_RESULT GetClearSampleFromFile(__in_z const char *szFileName, __in DRM_DWORD dwSampleNumber, __in DRM_DWORD cbSampleData, __out_ecount(cbSampleData) DRM_BYTE *pbBuffer)
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL fh;
    DRM_DWORD cb;
    DRM_CONST_STRING dstr;

    ChkDR( TST_UTL_MakePathString( NULL, szFileName, &dstr ) );

    fh = Oem_File_Open(NULL, dstr.pwszString, OEM_GENERIC_READ,  OEM_FILE_SHARE_NONE, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if ( fh == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_FAIL );
    }

    Oem_File_SetFilePointer( fh, (DRM_LONG)jazz_clear_sample_offsets[dwSampleNumber], OEM_FILE_BEGIN, NULL );
    Oem_File_Read(fh, pbBuffer, cbSampleData, &cb);
    Oem_File_Close(fh);

ErrorExit:
    FREEDRMSTRING( dstr );
    return dr;
}

static DRM_RESULT _ParseLengthType(      DRM_BYTE   bLenType,
                                   const DRM_BYTE  *pbLen,
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
        FIX_ENDIAN_DWORD(*pdwExplicitValue);
        *pdwTotalUsed += 1;
        break;
    case 0x02:
        DRM_BYT_CopyBytes(pdwExplicitValue, 0, pbLen, dwLenTypeOffset, SIZEOF(DRM_WORD));
        FIX_ENDIAN_DWORD(*pdwExplicitValue);
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
static DRM_RESULT ParseASFPacketHeader(const DRM_BYTE *pbPkt, DRM_DWORD dwPktOffset, ASFPACKET_PARSER_INFO *pPacketInfo, DRM_DWORD *pdwPayloadOffset)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   b = 0;
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

        pPacketInfo->bECLen = (DRM_BYTE)( b & 0x0f );
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
    pPacketInfo->bPadLenType = (DRM_BYTE)( ( b & 0x18 ) >> 3 );
    if ( pPacketInfo->bPadLenType == 3 )
    {
       ChkDR( DRM_E_FAIL );
    }

    /* Packet len type */
    pPacketInfo->bPacketLenType = (DRM_BYTE)( ( b & 0x60 ) >> 5 );

    if ( pPacketInfo->bPacketLenType != 0 &&
         pPacketInfo->bPacketLenType != 2 )
    {
        ChkDR( DRM_E_FAIL );
    }

    /* Must be 0 */
    pPacketInfo->bSequenceLenType = (DRM_BYTE)( ( b & 0x06 ) >> 1 );

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
    pPacketInfo->bOffsetBytes = (DRM_BYTE)( pPacketInfo->bECLen + 2 );
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
        pPacketInfo->bOffsetLenType = (DRM_BYTE)( ( b & 0x0c ) >> 2 );

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
static DRM_RESULT ParseASFPayloadHeader(const DRM_BYTE               *pbPkt,
                                              DRM_DWORD               dwPktOffset,
                                              ASFPACKET_PARSER_INFO  *pPacketInfo,
                                              ASFPAYLOAD_PARSER_INFO *pPayloadInfo,
                                              DRM_DWORD               dwPacketSize,
                                              DRM_UINT64             *pqwCurrentSampleID,
                                              DRM_DWORD              *pSampleDataOffset )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_DWORD dwPayloadSize = 0;
    DRM_BYTE b = 0;
    DRM_DWORD dwTotalUsed = 0;
    DRM_DWORD dwRepDataOffset = 0;
    DRM_WORD wTotalDataBytes = 0;

    pPayloadInfo->wPacketOffset = (DRM_WORD) pPacketInfo->dwParseOffset;

    /* Read Stream ID and Media Object Number */
    b = GET_BYTE(pbPkt, dwPktOffset + dwTotalUsed);
    dwTotalUsed += 1;

    pPayloadInfo->bStreamId = (DRM_BYTE)( b & 0x7f );
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

/**
 * Allows PFDMGR_CONTEXT * that gets passed to all PFD API Functions to be set to
 * an arbitrary value.
 *
 * argv[0] = value
 */
DRM_RESULT DRM_CALL Test_SetpPfdDrmContext(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  tmp;

    ChkArg(argc == 1 && argv[0]);

    tmp = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    if ( 0 == tmp )
    {
        g_pPfdDrmContext = NULL;
    }

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
    DRM_RESULT                             dr         = DRM_SUCCESS;
    DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT  eKeyLength = eDRM_RSA_KEY_LENGTH_1024_2048BIT;
    DRM_CRYPTO_CONTEXT                    *pCryptoCtx = NULL;
    DRM_BYTE                              *pTmp       = NULL;
    DRM_DWORD                              cbTmp      = 0;
    DRM_DWORD                              cbPrime    = 0;

    ChkArg(argc == 1 && argv[0]);

    ChkBOOL( LoadTestFile( NULL, argv[0], &pTmp, &cbTmp ), DRM_E_TEST_FILE_LOAD_ERROR );
    ChkMem(pTmp);

    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );

    if ( NULL != g_pPrivateKey )
    {
        OEM_RSA_ZeroPrivateKey_2048BIT(g_pPrivateKey);
        g_pPrivateKey = NULL;
    }

    cbPrime = DRM_RSA_CB_PRIME( eKeyLength );

    ChkDR( OEM_RSA_SetPrivateKey_2048BIT( eKeyLength,
                                          cbTmp,
                                          pTmp,
                                          cbPrime,
                                          g_rgbRSAPrimes[ 0 ],
                                          g_rgbRSAPrimes[ 1 ],
                                          g_pPrivateKey,
                                          pCryptoCtx ) );

 ErrorExit:
    SAFE_OEM_FREE( pTmp );
    SAFE_OEM_FREE( pCryptoCtx );

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

    ChkArg( argc == 1 && argv[0] );

    ChkDR( Test_ReadRSAPrivateKeyFromFile2048( argv[0], &g_PrivateKey ) );
    g_pPrivateKey = &g_PrivateKey;

ErrorExit:
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
    ChkBOOL(LoadTestFile(NULL, argv[0], &g_pCertificate, &g_cbCertificate), DRM_E_TEST_FILE_LOAD_ERROR);
    ChkMem(g_pCertificate);

 ErrorExit:
    return dr;
}

/**
 * Allows Device Certificate that gets passed to some functions to be set
 *
 * argv[0] = path to device certificate
 */
DRM_RESULT DRM_CALL Test_SetDeviceCertificate(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR  *filename = NULL;
    DRM_DWORD   size;
    DRM_STRING dstr;
    DRM_SUBSTRING dsubstr;
    DRM_WCHAR   pwszDevCertDotDat[] = {
        ONE_WCHAR('d', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('v', '\0'),  ONE_WCHAR('c', '\0'),
        ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('.', '\0'),
        ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0','\0')
    };

    ChkArg( argc == 1 && argv[0] );
    size = (DRM_DWORD) (strlen(argv[0]) + 1);
    filename = (DRM_WCHAR *) Oem_MemAlloc(size * SIZEOF(DRM_WCHAR));
    ChkMem( filename );

    dsubstr.m_ich = 0;
    dsubstr.m_cch = size - 1;
    dstr.pwszString = filename;
    dstr.cchString = size;
    DRM_UTL_PromoteASCIItoUNICODE(argv[0], &dsubstr, &dstr);
    filename[dstr.cchString] = 0;

    ChkDR( _CopyFileToDRMFolder(filename, pwszDevCertDotDat) );

 ErrorExit:
    return dr;
}

/**
 * Allows Data Store that gets passed to some functions to be set
 *
 * argv[0] = path to data store
 */
DRM_RESULT DRM_CALL Test_SetDataStore(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR  *filename = NULL;
    DRM_DWORD   size;
    DRM_SUBSTRING dsubstr;
    DRM_STRING  dstr;
    DRM_WCHAR   pwszSampleDotHds[] = {
        ONE_WCHAR('s', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),
        ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),
        ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0','\0')
    };

    ChkArg( argc == 1 && argv[0] );
    size = (DRM_DWORD) (strlen(argv[0]) + 1);
    filename = (DRM_WCHAR *) Oem_MemAlloc(size * SIZEOF(DRM_WCHAR));

    ChkMem( filename );
    dsubstr.m_ich = 0;
    dsubstr.m_cch = size - 1;
    dstr.pwszString = filename;
    dstr.cchString = size;
    DRM_UTL_PromoteASCIItoUNICODE(argv[0], &dsubstr, &dstr);
    filename[dstr.cchString] = 0;

    ChkDR( _CopyFileToDRMFolder(filename, pwszSampleDotHds) );
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
    DRM_DWORD dw = 0;

    ChkArg( map && sz );

    while( dw < cbMap && strcmp( sz, map[dw] ) )
    {
        dw++;
    }

    ChkBOOL( dw < cbMap , DRM_E_FAIL );

    dr = (DRM_RESULT)dw;

ErrorExit:
    return dr;
}

#define MAX_CRL_FILES 3

/**
 * argv[0] - type of revocation list
 * argv[1] - file containing license response with revocation list
 */
DRM_RESULT DRM_CALL Test_SetRevocationList(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbBuffer = NULL;
    DRM_DWORD cbBuffer;
    DRM_APP_CONTEXT *pDrmMgrContext = PFDMGR_CONTEXT_DRM_MANAGER_CONTEXT(g_pPfdDrmContext);
    const char* map[] = { "APP", "DEVICE", "WMDRMNET" };
    DRM_RESULT i = 0;
    DRM_RVK_LIST RevocationList;
    DRM_GUID *pGuid = NULL;
    DRM_DWORD cbDecoded = 0;
    DRM_SUBSTRING dasstrSource;
    DRM_BOOL fDecode = TRUE;

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

        ChkDR( Test_DrmRevocationTypeEnumGUID( (DRM_REVOCATION_TYPE_ENUM) i, &pGuid ) );
    }

    MEMCPY( &RevocationList.guidRevocationList, pGuid, DRM_ID_SIZE );

    ChkBOOL( LoadTestFile(NULL, argv[1],&pbBuffer,&cbBuffer), DRM_E_TEST_FILE_LOAD_ERROR );

    if(fDecode)
    {
        dasstrSource.m_ich = 0;
        dasstrSource.m_cch = cbBuffer;
        dr = DRM_B64_DecodeA( (const DRM_CHAR *) pbBuffer, &dasstrSource, &cbDecoded, NULL, DRM_BASE64_DECODE_IN_PLACE );
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

/*
Get Revocation List
argv[0]: type to retrieve: APP, DEVICE or WMDRMNET
*/
DRM_RESULT DRM_CALL Test_GetRevocationList(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_REVOCATION_TYPE_ENUM eRevType = DRM_REVOCATION_TYPE_WMDRM_ND;
    DRM_BYTE *pbBuffer = NULL;
    DRM_DWORD cbBuffer = 0;
    DRM_BYTE *pbExpected = NULL;
    DRM_DWORD cbExpected = 0;
    DRM_DWORD dwVersion;
    DRM_BYTE *pbDecoded = NULL;
    DRM_DWORD cbDecoded = 0;
    DRM_SUBSTRING dasstrEncodedData;
    DRM_STRING dstrRevocationList;
    DRM_STRING dstrTemplate = {NULL, 0};
    DRM_APP_CONTEXT *pDrmMgrContext = PFDMGR_CONTEXT_DRM_MANAGER_CONTEXT(g_pPfdDrmContext);

    ChkArg(argc >= 1);

    if ( 0 == strcmp(argv[0], "APP") )
    {
        eRevType =  DRM_REVOCATION_TYPE_WMDRM_APP;
    }
    else if ( 0 == strcmp(argv[0], "DEVICE") )
    {
        eRevType = DRM_REVOCATION_TYPE_WMDRM_DEVICE;
    }
    else if ( 0 == strcmp(argv[0], "WMDRMNET") )
    {
        eRevType = DRM_REVOCATION_TYPE_WMDRM_ND;
    }
    else
    {
        ChkArg(0 /* not a recognized guid */);
    }

    dr = Drm_Revocation_GetList(pDrmMgrContext,
                                   eRevType,
                                   pbBuffer,
                                   &cbBuffer,
                                   &dwVersion);
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        pbBuffer = (DRM_BYTE *) Oem_MemAlloc(cbBuffer);
        ChkMem(pbBuffer);
        dr = Drm_Revocation_GetList(pDrmMgrContext,
                                       eRevType,
                                       pbBuffer,
                                       &cbBuffer,
                                       &dwVersion);
    }
    ChkDR(dr);

    if ( 2 == argc )
    {
        if ( NULL == argv[1] ) {
            ChkArg( 0 == cbBuffer );
        }
        else
        {
            ChkArg(LoadTestFile(NULL,
                                argv[1],
                                &pbExpected,
                                &cbExpected));
            /* decode the XML for the new exclusion list */
            cbDecoded = cbExpected;
            pbDecoded = (DRM_BYTE *) Oem_MemAlloc(cbDecoded);
            ChkMem(pbDecoded);
            dasstrEncodedData.m_ich = 0;
            dasstrEncodedData.m_cch = cbExpected;
            ChkDR( DRM_B64_DecodeA( (DRM_CHAR *) pbExpected,
                                    &dasstrEncodedData,
                                    &cbDecoded,
                                    pbDecoded,
                                    0 ) );

            if ( DRM_REVOCATION_TYPE_WMDRM_ND == eRevType )
            {
                DSTR_FROM_PB( &dstrRevocationList, pbDecoded, cbDecoded );

                /* find the <TEMPLATE> tag and base64 decode its delicious contents */
                ChkDR( DRM_XML_GetSubNode(  (DRM_CONST_STRING *) &dstrRevocationList,
                                            (DRM_CONST_STRING *) &g_dstrTemplate,
                                            NULL,
                                            NULL,
                                            0,
                                            NULL,
                                            (DRM_CONST_STRING *) &dstrTemplate,
                                            1 ) );

                ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING*)&dstrTemplate,
                    &cbDecoded,
                    NULL,
                    DRM_BASE64_DECODE_IN_PLACE ) );

                ChkArg( cbDecoded == cbBuffer
                        &&  0 == memcmp(dstrTemplate.pwszString, pbBuffer, cbBuffer) );

            }
            else
            {
                ChkArg( cbDecoded == cbBuffer
                        &&  0 == memcmp(pbDecoded, pbBuffer, cbBuffer) );
            }
        }

    }

 ErrorExit:
    Oem_MemFree(pbExpected);
    Oem_MemFree(pbBuffer);
    Oem_MemFree(pbDecoded);

    return dr;
}

DRM_RESULT DRM_CALL Test_GetRevocationInfo(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   *pbRevInfo = NULL;
    DRM_DWORD   cbRevInfo;
    DRM_BYTE   *pbExpected = NULL;
    DRM_DWORD   cbExpected = 0;
    DRM_APP_CONTEXT *pDrmMgrContext = PFDMGR_CONTEXT_DRM_MANAGER_CONTEXT(g_pPfdDrmContext);

    dr = Drm_Revocation_GetList(pDrmMgrContext, DRM_REVOCATION_TYPE_WMDRM_REVINFO, pbRevInfo, &cbRevInfo, NULL);
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        pbRevInfo = (DRM_BYTE *) Oem_MemAlloc(cbRevInfo);
        ChkMem(pbRevInfo);
        dr = Drm_Revocation_GetList(pDrmMgrContext, DRM_REVOCATION_TYPE_WMDRM_REVINFO, pbRevInfo, &cbRevInfo, NULL);
    }
    ChkDR(dr);

    if ( 1 == argc )
    {
        if ( NULL == argv[0] )
        {
            ChkArg( 0 == cbRevInfo );
        }
        else
        {
            ChkArg(LoadTestFile(NULL,
                                argv[0],
                                &pbExpected,
                                &cbExpected));

            ChkArg( cbExpected == cbRevInfo
                    &&  0 == memcmp(pbExpected, pbRevInfo, cbRevInfo) );
        }
    }

ErrorExit:
    Oem_MemFree(pbRevInfo);
    Oem_MemFree(pbExpected);

    return dr;
}

/**
 * Allows cbAction that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_WORD cbAction
 */
DRM_RESULT DRM_CALL Test_SetcbAction(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( argc < 2)
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    ChkArg(argc == 1 && argv[1]);
    g_cbActionOverride = (DRM_WORD) strtoul(argv[1], NULL, 0);

 ErrorExit:
    return dr;
}

/**
 * Allows TransactionID that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD TransactionID
 */
DRM_RESULT DRM_CALL Test_SetTransactionID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_dwTransactionID = (DRM_DWORD) strtoul(argv[0], NULL, 0);

 ErrorExit:
    return dr;
}

/**
 * Allows dwMediaSessionID that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD dwMediaSessionID
 */
DRM_RESULT DRM_CALL Test_SetdwMediaSessionID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_dwMediaSessionID = (DRM_DWORD) strtoul(argv[0], NULL, 0);


 ErrorExit:
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
    ChkDR( ParseDRMID(&g_SerialNumber, argv[0]) );

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
    ChkDR( ParseDRMID(&g_Seed, argv[0]) );

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
 * Allows cbLicenseRequestMsg that gets passed to some functions to be set to
 * an arbitrary value.
 *
 * argv[0] = DRM_DWORD cbLicenseRequestMsgOverride
 */
DRM_RESULT DRM_CALL Test_SetcbLicenseRequestMsg(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1 && argv[0]);
    g_cbLicenseRequestMsgOverride = (DRM_DWORD) strtoul(argv[0], NULL, 0);

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
 * Allows pbLicenseResponseMsg to be set to NULL
 */
DRM_RESULT DRM_CALL Test_SetpbLicenseResponseMsgNULL(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_pbLicenseResponseMsgNULL = 1;

    return dr;
}

/**
 * argv[1] = DRM_DWORD       f_dwChallenge1
 * argv[2] = DRM_DWORD       f_dwChallenge2
 * argv[3] = DRM_DWORD       f_dwChallenge3
 * argv[4] = DRM_DWORD       f_dwChallenge4
 */
DRM_RESULT DRM_CALL Test_SetNonce(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 4 && argv[0] && argv[1] && argv[2] && argv[3]);

    g_dwChallenge1 = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    g_dwChallenge2 = (DRM_DWORD) strtoul(argv[1], NULL, 0);
    g_dwChallenge3 = (DRM_DWORD) strtoul(argv[2], NULL, 0);
    g_dwChallenge4 = (DRM_DWORD) strtoul(argv[3], NULL, 0);
    g_NonceOverride = 1;

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
DRM_RESULT DRM_CALL Test_Sleep(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    sleepage;  /* milliseconds */

    DRM_TEST_OEM_CONTEXT *poTestOemContext = NULL;
    MAP_TO_GLOBAL_TEST_VAR( poTestOemContext, NDT_TEST_OEM_CONTEXT_VAR );


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
        if( poTestOemContext->fFreezeTickCount )
        {
            poTestOemContext->dwTickCount      = poTestOemContext->dwTickCount + sleepage;
        }
        else
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
                tChangeSystemTime( (long)sleepage );
            }
        }
    }

#ifdef WINCE_TEST
    Sleep(2000);
#endif

 ErrorExit:
    return dr;
}

/**
 * argv[0]  = IN  DRM_DWORD       f_dwControlCapabilities
 */
DRM_RESULT DRM_CALL Test_CheckdwControlCapabilities(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1);

    ChkBOOL( g_dwControlCapabilities == (DRM_DWORD) strtoul(argv[0], NULL, 0), DRM_E_FAIL );

ErrorExit:

    return dr;
}

/**
 * argv[0]  = IN  DRM_DWORD       f_dwDRMStatus
 */
DRM_RESULT DRM_CALL Test_CheckdwDRMStatus(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1);

    ChkBOOL( g_dwDRMStatus == (DRM_DWORD) strtoul(argv[0], NULL, 0), DRM_E_FAIL );

ErrorExit:

    return dr;
}

/**
 * argv[0]  = IN  DRM_DWORD       f_dwLicenseStatus
 */
DRM_RESULT DRM_CALL Test_CheckdwLicenseStatus(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1);

    ChkBOOL( g_dwLicenseStatus == (DRM_DWORD) strtoul(argv[0], NULL, 0), DRM_E_FAIL );

ErrorExit:

    return dr;
}

/**
 * argv[0] = IN  DRM_DWORD       f_dwDRMDeliveryOptions
 */
DRM_RESULT DRM_CALL Test_CheckdwDeliveryOptions(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(argc == 1);

    ChkBOOL( g_dwDRMDeliveryOptions == (DRM_DWORD) strtoul(argv[0], NULL, 0), DRM_E_FAIL );

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
 * argv[0] = encrypted wma file (created by calling open media sesison and getalldatablocks)
 * argv[1] = clear wma file to compare against
 */
#if 0
DRM_RESULT DRM_CALL Test_CheckDecryption(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BYTE   *pbClear = NULL;
    DRM_BYTE   *pbEncrypted = NULL;
    DRM_BYTE   *pbDecrypted = NULL;
    DRM_DWORD   cbClear;
    DRM_DWORD   cbEncrypted;
    DRM_DWORD   cbClearDataObjectOffset;
    DRM_DWORD   cbClearDataPacketOffset;
    DRM_DWORD   cbClearPayloadOffset;
    DRM_DWORD   cbClearSampleDataOffset;
    DRM_DWORD   cbEncryptedDataObjectOffset;
    DRM_DWORD   cbEncryptedDataPacketOffset;
    DRM_DWORD   cbEncryptedPayloadOffset;
    DRM_DWORD   cbEncryptedSampleDataOffset;
    DRM_DWORD   nDataPackets;
    DRM_DWORD   cbSampleData;
    DRM_DWORD   dwClearDataPacketSize;
    DRM_DWORD   dwEncryptedDataPacketSize;
    DRM_DWORD   tmp;
    DRM_UINT64   qwCurrentSampleID;
    ASFPACKET_PARSER_INFO  ClearPacketInfo;
    ASFPAYLOAD_PARSER_INFO ClearPayloadInfo;
    ASFPACKET_PARSER_INFO  EncryptedPacketInfo;
    ASFPAYLOAD_PARSER_INFO EncryptedPayloadInfo;
    DRM_DWORD i;

    ChkArg( argc == 2 && argv[0] && argv[1] );


    ChkBOOL(LoadTestFile(NULL, argv[0], &pbEncrypted, &cbEncrypted), DRM_E_TEST_FILE_LOAD_ERROR);
    ChkMem(pbEncrypted);

    ChkBOOL(LoadTestFile(NULL, argv[1], &pbClear, &cbClear), DRM_E_TEST_FILE_LOAD_ERROR);
    ChkMem(pbClear);

    /* Get Offsets and Number of Data Packets for Clear file */
    cbClearDataObjectOffset = *((DRM_DWORD *) (pbClear + 16));
    nDataPackets = *((DRM_DWORD *) (pbClear + cbClearDataObjectOffset + 16 + 8 + 16));
    cbClearDataPacketOffset = cbClearDataObjectOffset + 16 + 8 + 16 + 8 + 2;
    tmp = *((DRM_DWORD *) (pbClear + cbClearDataObjectOffset + 16));
    dwClearDataPacketSize = (tmp - 50) / nDataPackets;

    /* Get Offsets for Encrypted file */
    cbEncryptedDataObjectOffset = *((DRM_DWORD *) (pbEncrypted + 16));
    cbEncryptedDataPacketOffset = cbEncryptedDataObjectOffset + 16 + 8 + 16 + 8 + 2;
    tmp = *((DRM_DWORD *) (pbEncrypted + cbEncryptedDataObjectOffset + 16));
    dwEncryptedDataPacketSize = (tmp - 50) / nDataPackets;

    for (i = 0; i < nDataPackets; i++)
    {
        /* Parse PACKET header */
        ChkDR( ParseASFPacketHeader(pbClear, cbClearDataPacketOffset, &ClearPacketInfo, &tmp) );
        cbClearPayloadOffset = cbClearDataPacketOffset + tmp;

        /* Parse PAYLOAD header */
        ChkDR( ParseASFPayloadHeader( pbClear,
                                      cbClearPayloadOffset,
                                      &ClearPacketInfo,
                                      &ClearPayloadInfo,
                                      dwClearDataPacketSize,
                                      &qwCurrentSampleID,
                                      &tmp) );
        cbClearSampleDataOffset = cbClearPayloadOffset + tmp;

        /* Parse PACKET header */
        if ( cbEncryptedDataPacketOffset > cbEncrypted )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
        ChkDR( ParseASFPacketHeader(pbEncrypted, cbEncryptedDataPacketOffset, &EncryptedPacketInfo, &tmp) );
        cbEncryptedPayloadOffset = cbEncryptedDataPacketOffset + tmp;

        /* Parse PAYLOAD header */
        if ( cbEncryptedPayloadOffset > cbEncrypted )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
        ChkDR( ParseASFPayloadHeader( pbEncrypted,
                                      cbEncryptedPayloadOffset,
                                      &EncryptedPacketInfo,
                                      &EncryptedPayloadInfo,
                                      dwEncryptedDataPacketSize,
                                      &qwCurrentSampleID,
                                      &tmp) );
        cbEncryptedSampleDataOffset = cbEncryptedPayloadOffset + tmp;
        cbSampleData = dwEncryptedDataPacketSize - (cbEncryptedSampleDataOffset - cbEncryptedDataPacketOffset) - EncryptedPacketInfo.dwPadding;

        /* Finally at the payload */
        if ( cbEncryptedSampleDataOffset + cbSampleData > cbEncrypted )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
        pbDecrypted = (DRM_BYTE *) OEM_malloc(cbEncrypted);
        DRM_BYT_CopyBytes(pbDecrypted, 0, pbEncrypted, cbEncryptedSampleDataOffset, cbSampleData);
        ChkDR( DrmAesCtrProcessData( g_pContentEncryptionKey,
                                     pbDecrypted,
                                     cbSampleData,
                                     &qwCurrentSampleID,
                                     DRM_UI64( 0 ),
                                     DRM_UI64( 0 ) ) );
        ChkArg( 0 == DRM_BYT_CompareBytes(pbClear, cbClearSampleDataOffset, pbDecrypted, 0, cbSampleData) );
        OEM_free(pbDecrypted);
        pbDecrypted = NULL;

        /* Add padding */
        cbClearDataPacketOffset += dwClearDataPacketSize;
        cbEncryptedDataPacketOffset += dwEncryptedDataPacketSize;
    }

ErrorExit:
    if (pbClear != NULL)
    {
        OEM_free( pbClear );
    }

    if (pbEncrypted != NULL)
    {
        OEM_free( pbEncrypted );
    }

    if (pbDecrypted != NULL)
    {
        OEM_free( pbDecrypted );
    }

    return dr;
}
#else
#endif

/*
 * argv[0] = actual transferred wma file (created by calling open media session and getalldatablocks)
 * argv[1] = expected file to compare against
 */
DRM_RESULT DRM_CALL Test_CheckNotTranscrypted(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BYTE   *pbActual = NULL;
    DRM_BYTE   *pbExpected = NULL;
    DRM_DWORD   cbActual;
    DRM_DWORD   cbExpected;

    ChkArg( argc == 2 && argv[0] && argv[1] );

    ChkBOOL( LoadTestFile(NULL, argv[0], &pbActual, &cbActual), DRM_E_TEST_FILE_LOAD_ERROR );
    ChkMem( pbActual );

    ChkBOOL( LoadTestFile(NULL, argv[1], &pbExpected, &cbExpected), DRM_E_TEST_FILE_LOAD_ERROR );
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
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD   dwSourceID;

    ChkArg( argc == 1 && argv[0] && g_pXmrLicense != NULL );

    dwSourceID = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    ChkArg( g_pXmrLicense->gr.dwSourceId == dwSourceID );

ErrorExit:

    return dr;
}

/*
 * argv[0] = File Containing Device Key to Compare Against
 */
DRM_RESULT DRM_CALL Test_CheckDeviceKey(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT      dr = DRM_SUCCESS;
    XMR_DEVICE_KEY  DeviceKey = {0, 0, NULL};
    DRM_DWORD       cbTmp;
    DRM_BYTE       *pbTmp = NULL;
    DRM_DWORD       ibTmp = 0;

    ChkArg( argc == 1 && argv[0] && g_pXmrLicense != NULL );
    ChkArg( g_pXmrLicense->km.dk.pbModulus != NULL );

    ChkBOOL(LoadTestFile(NULL, argv[0], &pbTmp, &cbTmp), DRM_E_TEST_FILE_LOAD_ERROR);
    ChkMem(pbTmp);

    ibTmp = 0;

    ChkArg( cbTmp > (SIZEOF(DRM_DWORD) + SIZEOF(DRM_WORD)) );

    DRM_BYT_CopyBytes( (DRM_BYTE*)&DeviceKey.dwExponent, 0, pbTmp, ibTmp, SIZEOF(DRM_DWORD) );
    /* Note: dwExponent is already in the correct endianness. */
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
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_WORD    wSettingsObject;

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
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_WORD    i;
    DRM_GUID    guid;

    ChkArg( g_pXmrLicense != NULL && g_pXmrLicense->gr.il.cGUIDS == (unsigned long)argc );
    for (i = 0; i < argc; i++)
    {
        ChkDR( ParseGUID(&guid, (DRM_BYTE *)argv[i]) );
        ChkArg( 0 == MEMCMP(&guid, g_pXmrLicense->gr.il.pGUIDS + i, SIZEOF(DRM_GUID)) );
    }

ErrorExit:

    return dr;
}

/* drmpfdmanager.h */

/**
 *  argv[0] = DeviceStoreName
 */
DRM_RESULT DRM_CALL Test_DRM_PFD_Initialize(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_STRING  dstrDeviceStoreName /*INIT_DRM_STRING(dstrDeviceStoreName)*/;
    DRM_CONST_STRING dstrTemp;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    dstrDeviceStoreName.pwszString = NULL;
    dstrDeviceStoreName.cchString = 0;

    if (( 0 == argc ) || ( 1 == argc && argv[0] != NULL ))
    {
        ChkDR( tGetDeviceStorePathname( wszPathName, &dstrTemp ) );
        ChkOverflow( dstrTemp.cchString + 1, dstrTemp.cchString );
        ChkMem( dstrDeviceStoreName.pwszString = (DRM_WCHAR *) Oem_MemAlloc((dstrTemp.cchString + 1) * SIZEOF(DRM_WCHAR)) );
        MEMCPY( dstrDeviceStoreName.pwszString, dstrTemp.pwszString, (dstrTemp.cchString + 1) * SIZEOF(DRM_WCHAR) );
        dstrDeviceStoreName.cchString = dstrTemp.cchString;
    }
    ASSIGN_DRM_STRING(g_dstrHdsPath, dstrDeviceStoreName);

    ChkDR(Drm_Pfd_Initialize(g_pPfdDrmContext,
                             NULL,
                             (DRM_CONST_STRING *) &g_dstrHdsPath));
    g_Initialized = 1;

ErrorExit:

    return dr;
}

/**
 *  No parameters
 */
DRM_RESULT DRM_CALL Test_DRM_PFD_Uninitialize(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    Drm_Pfd_Uninitialize(g_pPfdDrmContext);
    g_pPfdDrmContext = NULL;
    g_Initialized = 0;

    return dr;
}

/**
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Registration_ProcessRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pRegistrationRequestMsg = NULL;

    /* create the Request message to send in the data block */
    ChkDR(WmdrmNetPackRegistrationMessage(&g_SerialNumber,
                                          g_pCertificate,
                                          g_cbCertificate,
                                          &pRegistrationRequestMsg,
                                          &g_cbRegistrationRequestMsg));

    if ( g_MessageProtocolVersionOverride )
    {
        ChkBOOL( g_cbRegistrationRequestMsg > 0, DRM_E_BUFFERTOOSMALL );
        PUT_BYTE( pRegistrationRequestMsg, 0,  g_MessageProtocolVersionOverride);
    }

    if ( g_MessageTypeOverride )
    {
        ChkBOOL( g_cbRegistrationRequestMsg > 1, DRM_E_BUFFERTOOSMALL );
        PUT_BYTE( pRegistrationRequestMsg, 1, g_MessageTypeOverride );
    }

    /*
    ** Optionally override the request message length only after the above optional adjustments
    */
    if ( g_cbRegistrationRequestMsgOverride )
    {
        g_cbRegistrationRequestMsg = g_cbRegistrationRequestMsgOverride;
    }

    ChkDR( Drm_Pfd_Registration_ProcessRequest( NULL,
                                                g_pPfdDrmContext,
                                                pRegistrationRequestMsg,
                                                g_cbRegistrationRequestMsg,
                                                &g_dwTransactionID ) );

 ErrorExit:
    SAFE_OEM_FREE( pRegistrationRequestMsg );

    return dr;
}

/**
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Registration_GenerateResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT          dr                        = DRM_SUCCESS;
    DRM_BYTE           *pbRegistrationResponseMsg = NULL;
    DRM_ID              idSerialNumber            = { 0 };
    DRM_BYTE           *pAddress                  = NULL;
    DRM_WORD            cbAddress                 = 0;
    DRM_CRYPTO_CONTEXT *pCryptoCtx                = NULL;

    ChkMem( pCryptoCtx = ( DRM_CRYPTO_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( pCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    if ( g_cbRegistrationResponseMsgOverride )
    {
        g_cbRegistrationResponseMsg = g_cbRegistrationResponseMsgOverride;
    }
    if ( 0 == g_pbRegistrationResponseMsgNULL )
    {
        pbRegistrationResponseMsg = (DRM_BYTE *) Oem_MemAlloc(g_cbRegistrationResponseMsg);
        ChkMem(pbRegistrationResponseMsg);
    }

    ChkDR(Drm_Pfd_Registration_GenerateResponse(g_pPfdDrmContext,
                                             g_dwTransactionID,
                                             pbRegistrationResponseMsg,
                                                 &g_cbRegistrationResponseMsg));


    ChkDR(WmdrmNetUnpackRegistrationResponseMessage(g_pPrivateKey,
                                                    pbRegistrationResponseMsg,
                                                    g_cbRegistrationResponseMsg,
                                                    &g_Seed,
                                                    &idSerialNumber,
                                                    &g_RegistrationSessionID,
                                                    &pAddress,
                                                    &cbAddress,
                                                    pCryptoCtx));

    ChkArg(MEMCMP(&idSerialNumber, &g_SerialNumber, 16) == 0);


 ErrorExit:
    SAFE_OEM_FREE( pbRegistrationResponseMsg );
    SAFE_OEM_FREE( pAddress );
    SAFE_OEM_FREE( pCryptoCtx );

    return dr;
}

/**
 * This function does the GetProximityChallenge, ProcessProximityResponse in one fell
 * swoop, in case the finer grain functions Test_Drm_Pfd_Proximity_GenerateChallenge and
 * Test_Drm_Pfd_Proximity_ProcessResponse are too slow to meet the 7ms deadline for
 * Proximity.
 */
DRM_RESULT DRM_CALL Test_Proximity(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    dwChallenge1 = 0;
    DRM_DWORD    dwChallenge2 = 0;
    DRM_DWORD    dwChallenge3 = 0;
    DRM_DWORD    dwChallenge4 = 0;
    DRM_DWORD    dwEncryptedChallenge1 = 0;
    DRM_DWORD    dwEncryptedChallenge2 = 0;
    DRM_DWORD    dwEncryptedChallenge3 = 0;
    DRM_DWORD    dwEncryptedChallenge4 = 0;
    DRM_ID       nonce    = { 0 };
    DRM_AES_KEY *pKey     = NULL;
    DRM_DWORD    sleepage = 0;

    DRM_TEST_OEM_CONTEXT *poTestOemContext = NULL;
    MAP_TO_GLOBAL_TEST_VAR( poTestOemContext, NDT_TEST_OEM_CONTEXT_VAR );

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

    ChkDR( Drm_Pfd_Proximity_GenerateChallenge( poTestOemContext,
                                                g_pPfdDrmContext,
                                                g_dwTransactionID,
                                                &dwChallenge1,
                                                &dwChallenge2,
                                                &dwChallenge3,
                                                &dwChallenge4 ) );

    if( !g_NonceOverride )
    {
        g_dwChallenge1 = dwChallenge1;
        g_dwChallenge2 = dwChallenge2;
        g_dwChallenge3 = dwChallenge3;
        g_dwChallenge4 = dwChallenge4;
    }

    PackDWORDSInDRMID(&nonce, g_dwChallenge1, g_dwChallenge2, g_dwChallenge3, g_dwChallenge4);

    /* encrypt nonce */
    pKey = WmdrmNetGetRegistrationKey(&g_Seed, FALSE);

    ChkDR(Oem_Aes_EcbEncryptData(pKey, (DRM_BYTE *) &nonce, DRM_ID_SIZE));

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

    UnpackDWORDSFromDRMID(&nonce,
                          &dwEncryptedChallenge1,
                          &dwEncryptedChallenge2,
                          &dwEncryptedChallenge3,
                          &dwEncryptedChallenge4);

    ChkDR( Drm_Pfd_Proximity_ProcessResponse( poTestOemContext,
                                              g_pPfdDrmContext,
                                              g_dwTransactionID,
                                              dwEncryptedChallenge1,
                                              dwEncryptedChallenge2,
                                              dwEncryptedChallenge3,
                                              dwEncryptedChallenge4 ) );

 ErrorExit:
    poTestOemContext->fFreezeTickCount = FALSE;
    if ( pKey )
    {
        Oem_Aes_ZeroKey(pKey);
        SAFE_OEM_FREE(pKey);
    }

    return dr;
}


DRM_RESULT DRM_CALL Test_Drm_Pfd_Proximity_GenerateChallenge(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    dwChallenge1;
    DRM_DWORD    dwChallenge2;
    DRM_DWORD    dwChallenge3;
    DRM_DWORD    dwChallenge4;
    DRM_TEST_OEM_CONTEXT *poTestOemContext = NULL;
    MAP_TO_GLOBAL_TEST_VAR( poTestOemContext, NDT_TEST_OEM_CONTEXT_VAR );

    /*
    ** Get the current clock ticks and indicate that we don't want it to change
    */
    poTestOemContext->fFreezeTickCount = FALSE;
    poTestOemContext->dwTickCount      = Oem_Clock_GetTickCount( poTestOemContext );
    poTestOemContext->fFreezeTickCount = TRUE;

    ChkDR( Drm_Pfd_Proximity_GenerateChallenge( poTestOemContext,
                                                g_pPfdDrmContext,
                                                g_dwTransactionID,
                                                &dwChallenge1,
                                                &dwChallenge2,
                                                &dwChallenge3,
                                                &dwChallenge4 ) );

    if( !g_NonceOverride )
    {
        g_dwChallenge1 = dwChallenge1;
        g_dwChallenge2 = dwChallenge2;
        g_dwChallenge3 = dwChallenge3;
        g_dwChallenge4 = dwChallenge4;
    }


 ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL Test_Drm_Pfd_Proximity_ProcessResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_DWORD    dwEncryptedChallenge1;
    DRM_DWORD    dwEncryptedChallenge2;
    DRM_DWORD    dwEncryptedChallenge3;
    DRM_DWORD    dwEncryptedChallenge4;
    DRM_ID       nonce;
    DRM_AES_KEY *pKey = NULL;
    DRM_TEST_OEM_CONTEXT *poTestOemContext = NULL;
    MAP_TO_GLOBAL_TEST_VAR( poTestOemContext, NDT_TEST_OEM_CONTEXT_VAR );

    PackDWORDSInDRMID(&nonce, g_dwChallenge1, g_dwChallenge2, g_dwChallenge3, g_dwChallenge4);

    /* encrypt nonce */
    pKey = WmdrmNetGetRegistrationKey(&g_Seed, FALSE);

    ChkDR(Oem_Aes_EcbEncryptData(pKey, (DRM_BYTE *) &nonce, DRM_ID_SIZE));

    UnpackDWORDSFromDRMID(&nonce,
                          &dwEncryptedChallenge1,
                          &dwEncryptedChallenge2,
                          &dwEncryptedChallenge3,
                          &dwEncryptedChallenge4);

    ChkDR( Drm_Pfd_Proximity_ProcessResponse( poTestOemContext,
                                              g_pPfdDrmContext,
                                              g_dwTransactionID,
                                              dwEncryptedChallenge1,
                                              dwEncryptedChallenge2,
                                              dwEncryptedChallenge3,
                                              dwEncryptedChallenge4 ) );

 ErrorExit:

    poTestOemContext->fFreezeTickCount = FALSE;

    if ( pKey )
    {
        Oem_Aes_ZeroKey(pKey);
        SAFE_OEM_FREE( pKey );
    }

    return dr;
}


/**
 * argv[0] = DRM_ID           f_drmRightsID
 * argv[1] = char            *f_Action
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_License_ProcessRequest(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT      dr                 = DRM_SUCCESS;
    DRM_BYTE       *pLicenseRequestMsg = NULL;
    const DRM_BYTE *pAction            = NULL;

    /* Parse Command Line Args */
    ChkArg(argc == 2 && argv[0] && argv[1]);

    ChkDR( ParseDRMID(&g_drmRightsID, argv[0]) );

    if ( g_cbActionOverride )
    {
        g_cbAction = g_cbActionOverride;
    }
    else
    {
        g_cbAction = (DRM_WORD) strlen(argv[1]);
    }
    pAction = (const DRM_BYTE *) argv[1];

    /* License Request */

    ChkDR(WmdrmNetPackLicenseRequestMessage(&g_drmRightsID,
                                            &g_SerialNumber,
                                            g_pCertificate,
                                            g_cbCertificate,
                                            pAction,
                                            g_cbAction,
                                            &pLicenseRequestMsg,
                                            &g_cbLicenseRequestMsg));

    if ( g_cbLicenseRequestMsgOverride )
    {
        g_cbLicenseRequestMsg = g_cbLicenseRequestMsgOverride;
    }

    ChkDR( Drm_Pfd_License_ProcessRequest( NULL,
                                           g_pPfdDrmContext,
                                           g_dwMediaSessionID,
                                           pLicenseRequestMsg,
                                           g_cbLicenseRequestMsg ) );

 ErrorExit:
    if ( pLicenseRequestMsg )
    {
        Oem_MemFree(pLicenseRequestMsg);
    }

    return dr;
}

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_License_GenerateResponse(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr = DRM_SUCCESS;
    DRM_BYTE    *pLicenseResponseMsg = NULL;
    DRM_BYTE    *pLicense = NULL;
    DRM_DWORD    cbLicense;

    if ( g_cbLicenseResponseMsgOverride != -1 )
    {
        g_cbLicenseResponseMsg = g_cbLicenseResponseMsgOverride;
    }
    if ( 0 == g_pbLicenseResponseMsgNULL )
    {
        pLicenseResponseMsg = (DRM_BYTE *) Oem_MemAlloc(g_cbLicenseResponseMsg);
        ChkMem(pLicenseResponseMsg);
    }
    ChkDR( Drm_Pfd_License_GenerateResponse( g_pPfdDrmContext,
                                             g_dwMediaSessionID,
                                             pLicenseResponseMsg,
                                             &g_cbLicenseResponseMsg ) );

    ChkDR(WmdrmNetUnpackLicenseResponseMessage(pLicenseResponseMsg, g_cbLicenseResponseMsg, &pLicense, &cbLicense));

    if( g_pXmrLicense != NULL )
    {
        DrmXmrFreeLicense( g_pXmrLicense );
        g_pXmrLicense = NULL;
    }

    ChkArg( pLicense != NULL );
    ChkDR(DrmXmrUnpackLicense(g_pPrivateKey, pLicense, cbLicense, &g_pXmrLicense));

    if ( NULL != g_pContentEncryptionKey )
    {
        Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
        SAFE_OEM_FREE( g_pContentEncryptionKey );
    }
    ChkDR(DrmXmrGetKeyFromLicense(g_pPrivateKey, g_pXmrLicense, TRUE, &g_pContentEncryptionKey));

    /* Device ID */
    ChkArg((MEMCMP(&g_pXmrLicense->gr.SerialNumber, &g_SerialNumber, DRM_ID_SIZE) == 0));

    MEMCPY( &g_xmrMinEnvironment , &g_pXmrLicense->gr , SIZEOF( XMR_GLOBAL_REQUIREMENTS ) );

 ErrorExit:
    if ( NULL != pLicenseResponseMsg )
    {
        Oem_MemFree(pLicenseResponseMsg);
    }

    if( DRM_FAILED( dr )
        && g_pXmrLicense != NULL )
    {
        DrmXmrFreeLicense( g_pXmrLicense );
        g_pXmrLicense = NULL;
    }
    SAFE_OEM_FREE(pLicense);

    return dr;
}

DRM_RESULT DRM_CALL Test_CheckXMRLicenseMinEnvironment(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    const char *map[] = { "SECURITY_LEVEL", "APP_CRL_VERSION", "DEVICE_CRL_VERSION" };
    DRM_RESULT i = 0;
    DRM_DWORD dwVersion;

    ChkArg( argc == 2 && argv[0] && argv[1] );
    ChkDR( i = _CheckMap( map, 3, argv[0]) );

    dr = DRM_SUCCESS;

    dwVersion = strtoul(argv[1], NULL, 0);

    switch(i)
    {
    case 0:
        ChkBOOL( (DRM_WORD)dwVersion == g_xmrMinEnvironment.wMinAppSecurityLevel , DRM_E_FAIL );
        break;
    case 1:
        ChkBOOL( dwVersion == g_xmrMinEnvironment.dwReserved1, DRM_E_FAIL );
        break;
    case 2:
        ChkBOOL( dwVersion == g_xmrMinEnvironment.dwReserved2, DRM_E_FAIL );
        break;
    default:
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    return dr;
}

/**
 * argv[0]  = IN  char           *f_pszInputFileName
 * argv[1]  = IN  DRM_DWORD       f_dwForceNewSession
 * argv[2]  = IN  DRM_WORD        f_wFileType
 * argv[3]  = IN  char           *f_pszOutputFileName
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_OpenMediaSession(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_CONST_STRING dstrInputFileName  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutputFileName = EMPTY_DRM_STRING;
    DRM_DWORD        dwForceNewSession  = 0;
    DRM_WORD         wFileType          = 0;

    ChkArg(argc == 4 && argv[0] && argv[3]);

    dwForceNewSession = (DRM_DWORD) strtoul( argv[1], NULL, 0 );
    wFileType         = (DRM_WORD ) strtoul( argv[2], NULL, 0 );

    ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrInputFileName ) );
    ChkDR( TST_UTL_MakePathString( NULL, argv[3], &dstrOutputFileName ) );

    if( g_hOutputFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( g_hOutputFile );
        g_hOutputFile = OEM_INVALID_HANDLE_VALUE;
    }

    ChkDR( Drm_Pfd_Aavt_OpenMediaSession( NULL,
                                          g_pPfdDrmContext,
                                          (DRM_CONST_STRING *) &g_dstrHdsPath,
                                          (DRM_WCHAR *) dstrInputFileName.pwszString,
                                          wFileType,
                                          dwForceNewSession,
                                          FALSE,
                                          &g_dwMediaSessionID,
                                          &g_dwControlCapabilities,
                                          &g_dwDRMStatus,
                                          &g_dwLicenseStatus,
                                          &g_dwDRMDeliveryOptions ) );
    g_dwCorrectMediaSessionID = g_dwMediaSessionID;

    g_hOutputFile = Oem_File_Open(NULL, dstrOutputFileName.pwszString, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( g_hOutputFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FAIL );

    g_FirstNextDataBlock = 1;

ErrorExit:
    FREEDRMSTRING( dstrInputFileName );
    FREEDRMSTRING( dstrOutputFileName );
    return dr;
}

/**
 * no parameters
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_CloseMediaSession(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    if (g_hOutputFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( g_hOutputFile );
        g_hOutputFile = OEM_INVALID_HANDLE_VALUE;
    }

    ChkDR( Drm_Pfd_Aavt_CloseMediaSession( g_pPfdDrmContext, g_dwMediaSessionID ) );

ErrorExit:

    return dr;
}

/*
 * argv[0] = data block size in bytes
 * argv[1] = (optional) current time position to check against
 * argb[2] = (optional) path to clear sample file
 *
 * *****
 * * NOTE: current time position checking only works for jazz1.wma
 * *       you must call getnextdatablock with a large enough block
 * *       size to get the entire header
 * *****
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_GetNextDataBlock(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BYTE   *pbNextDataBlock = NULL;
    DRM_DWORD   cbNextDataBlock;
    DRM_DWORD   cbWritten;

    ASFPACKET_PARSER_INFO  PacketInfo;
    ASFPAYLOAD_PARSER_INFO PayloadInfo;
    DRM_DWORD   tmp = 0;
    DRM_DWORD   dwClearPacketNum = 0;
    DRM_UINT64  qwCurrentSampleID = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD cbDataObjectOffset = 0;
    DRM_DWORD cbDataPacketOffset = 0;
    DRM_DWORD cbPayloadOffset = 0;
    DRM_DWORD cbSampleDataOffset = 0;
    DRM_DWORD cbSampleData = 0;
    DRM_BYTE *pbClear = NULL;
    DRM_BYTE *pbDecrypted = NULL;

    ChkArg( argc >= 1 );

    if ( 0 == strcmp(argv[0], "PACKET") )
    {
        /* Note: you must call Test_GetHeaders before supplying "PACKET" to this function */
        ChkArg( g_FirstNextDataBlock == 0 );
        cbNextDataBlock = g_dwDataPacketSize;
    }
    else
    {
        cbNextDataBlock = (DRM_DWORD) strtoul( argv[0], NULL, 0 );
    }

    pbNextDataBlock = (DRM_BYTE *)Oem_MemAlloc( cbNextDataBlock );
    ChkMem( pbNextDataBlock );

    dr = Drm_Pfd_Aavt_GetNextDataBlock( NULL,
                                        g_pPfdDrmContext,
                                        g_dwMediaSessionID,
                                        &g_dwEndOfData,
                                        pbNextDataBlock,
                                        &cbNextDataBlock );
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

    if ( g_FirstNextDataBlock )
    {
        g_FirstNextDataBlock = 0;
        /* first call to get next data block will return the asf header,
         * which is needed to get the packet size and number of data
         * packets in the data object
         */
        DRM_BYT_CopyBytes(&cbDataObjectOffset, 0, pbNextDataBlock, 16, SIZEOF(DRM_DWORD));
        FIX_ENDIAN_DWORD(cbDataObjectOffset);
        DRM_BYT_CopyBytes(&g_nDataPackets, 0, pbNextDataBlock, cbDataObjectOffset + 16 + 8 + 16, SIZEOF(DRM_DWORD));
        FIX_ENDIAN_DWORD(g_nDataPackets);
        cbDataPacketOffset = cbDataObjectOffset + 16 + 8 + 16 + 8 + 2;
        DRM_BYT_CopyBytes(&tmp, 0, pbNextDataBlock, cbDataObjectOffset + 16, SIZEOF(DRM_DWORD));
        FIX_ENDIAN_DWORD(tmp);
        g_dwDataPacketSize = (tmp - 50) / g_nDataPackets;

    }

    if ( argc >= 2 )
    {
        ChkDR( ParseASFPacketHeader(pbNextDataBlock, cbDataPacketOffset, &PacketInfo, &tmp) );
        cbPayloadOffset = cbDataPacketOffset + tmp;
        ChkDR( ParseASFPayloadHeader(pbNextDataBlock,
                                     cbPayloadOffset,
                                     &PacketInfo,
                                     &PayloadInfo,
                                     g_dwDataPacketSize,
                                     &qwCurrentSampleID,
                                     &tmp) );
        cbSampleDataOffset = cbPayloadOffset + tmp;
        ChkOverflow( cbSampleDataOffset, cbDataPacketOffset );
        ChkOverflow( g_dwDataPacketSize, cbSampleDataOffset-cbDataPacketOffset );
        ChkOverflow( g_dwDataPacketSize-(cbSampleDataOffset-cbDataPacketOffset), g_dwDataPacketSize-(cbSampleDataOffset-cbDataPacketOffset)-PacketInfo.dwPadding);
        cbSampleData = g_dwDataPacketSize - (cbSampleDataOffset - cbDataPacketOffset) - PacketInfo.dwPadding;

        dwClearPacketNum = 0;
        {
            DRM_DWORD i;
            for (i = 30; i > 0; i--)
            {
                if ( PayloadInfo.dwObjectPres >= jazz1_presentationtimes[i] )
                {
                    dwClearPacketNum = i;
                    break;
                }
            }
        }

        TRACE(("Current Time  : %ld\n", PayloadInfo.dwObjectPres));
        TRACE(("Current Sample: %ld\n", dwClearPacketNum));
        TRACE(("Known Time %ld == Actual Time %ld\n", jazz1_presentationtimes[dwClearPacketNum], PayloadInfo.dwObjectPres));

        ChkArg( jazz1_presentationtimes[dwClearPacketNum] == PayloadInfo.dwObjectPres );

    }

    if ( argc >= 3 )
    {
        DRM_AES_COUNTER_MODE_CONTEXT ctrCtx;

        /* Get sample data to be decrypted */
        pbDecrypted = (DRM_BYTE *) Oem_MemAlloc(cbSampleData);
        ChkMem(pbDecrypted);
        DRM_BYT_CopyBytes(pbDecrypted, 0, pbNextDataBlock, cbSampleDataOffset, cbSampleData);

        /* Get corresponding clear sample */
        pbClear = (DRM_BYTE *)  Oem_MemAlloc(cbSampleData);
        ChkMem(pbClear);
        ChkDR( GetClearSampleFromFile(argv[2], dwClearPacketNum, cbSampleData, pbClear) );

        ZEROMEM( &ctrCtx, SIZEOF( ctrCtx ) );
        ctrCtx.qwInitializationVector = qwCurrentSampleID;

        /* Decrypt and compare to jazz_clear sample */
        ChkDR( Oem_Aes_CtrProcessData( g_pContentEncryptionKey,
                                     pbDecrypted,
                                     cbSampleData,
                                     &ctrCtx ) );
        ChkArg( 0 == DRM_BYT_CompareBytes(pbClear, 0, pbDecrypted, 0, cbSampleData) );
    }

    Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );

ErrorExit:

    if (pbNextDataBlock != NULL)
    {
        Oem_MemFree( pbNextDataBlock );
    }

    SAFE_OEM_FREE(pbClear);
    SAFE_OEM_FREE(pbDecrypted);

    return dr;
}

/*
 * argv[0] = data block size in bytes
 * argv[1] = (optional) path to clear sample file
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_GetAllDataBlocks(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BYTE   *pbNextDataBlock = NULL;
    DRM_DWORD   cbNextDataBlock;
    DRM_DWORD   cbDataBlock;
    DRM_DWORD   cbWritten;
    DRM_DWORD i = 0;
    DRM_DWORD size;

    ASFPACKET_PARSER_INFO  PacketInfo;
    ASFPAYLOAD_PARSER_INFO PayloadInfo;
    DRM_DWORD   tmp;
    DRM_UINT64  qwCurrentSampleID = DRM_UI64LITERAL( 0, 0 );
    DRM_DWORD cbDataPacketOffset = 0;
    DRM_DWORD cbPayloadOffset = 0;
    DRM_DWORD cbSampleDataOffset = 0;
    DRM_DWORD cbSampleData = 0;
    DRM_BYTE *pbClear = NULL;
    DRM_BYTE *pbDecrypted = NULL;
    DRM_BOOL fGettingPackets = FALSE;

    ChkArg( argc >= 1 );

    if ( 0 == strcmp(argv[0], "PACKET") )
    {
        /* Note: you must call Test_GetHeaders before supplying "PACKET" to this function */
        ChkArg( g_FirstNextDataBlock == 0 );
        size = g_dwDataPacketSize;
        fGettingPackets = TRUE;
    }
    else
    {
        size = (DRM_DWORD) strtoul( argv[0], NULL, 0 );
    }

    cbDataBlock     = size;
    pbNextDataBlock = (DRM_BYTE *)Oem_MemAlloc( size );
    ChkMem( pbNextDataBlock );

    do
    {
        cbNextDataBlock = cbDataBlock;
        dr = Drm_Pfd_Aavt_GetNextDataBlock( NULL,
                                            g_pPfdDrmContext,
                                            g_dwMediaSessionID,
                                            &g_dwEndOfData,
                                            pbNextDataBlock,
                                            &cbNextDataBlock );
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

        if ( TRUE == fGettingPackets )
        {
            DRM_AES_COUNTER_MODE_CONTEXT ctrCtx;

            ChkDR( ParseASFPacketHeader(pbNextDataBlock, cbDataPacketOffset, &PacketInfo, &tmp) );
            cbPayloadOffset = cbDataPacketOffset + tmp;
            ChkDR( ParseASFPayloadHeader(pbNextDataBlock,
                                         cbPayloadOffset,
                                         &PacketInfo,
                                         &PayloadInfo,
                                         g_dwDataPacketSize,
                                         &qwCurrentSampleID,
                                         &tmp) );
            cbSampleDataOffset = cbPayloadOffset + tmp;
            ChkOverflow( cbSampleDataOffset, cbDataPacketOffset );
            ChkOverflow( g_dwDataPacketSize, cbSampleDataOffset-cbDataPacketOffset );
            ChkOverflow( g_dwDataPacketSize-(cbSampleDataOffset-cbDataPacketOffset), g_dwDataPacketSize-(cbSampleDataOffset-cbDataPacketOffset)-PacketInfo.dwPadding );

            cbSampleData = g_dwDataPacketSize - (cbSampleDataOffset - cbDataPacketOffset) - PacketInfo.dwPadding;

            /* Get sample data to be decrypted */
            pbDecrypted = (DRM_BYTE *) Oem_MemAlloc(cbSampleData);
            ChkMem(pbDecrypted);
            DRM_BYT_CopyBytes(pbDecrypted, 0, pbNextDataBlock, cbSampleDataOffset, cbSampleData);

            /* Get corresponding clear sample */
            pbClear = (DRM_BYTE *)  Oem_MemAlloc(cbSampleData);
            ChkMem(pbClear);
            ChkArg( argc >= 2 );
            ChkDR( GetClearSampleFromFile(argv[1], i, cbSampleData, pbClear) );

            ZEROMEM( &ctrCtx, SIZEOF( ctrCtx ) );
            ctrCtx.qwInitializationVector = qwCurrentSampleID;

            /* Decrypt and compare to jazz_clear sample */
            ChkDR( Oem_Aes_CtrProcessData( g_pContentEncryptionKey,
                                         pbDecrypted,
                                         cbSampleData,
                                         &ctrCtx ) );
            ChkArg( 0 == DRM_BYT_CompareBytes(pbClear, 0, pbDecrypted, 0, cbSampleData) );
            SAFE_OEM_FREE(pbDecrypted);
            SAFE_OEM_FREE(pbClear);
        }

        Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );
        i++;
    } while ( g_LastDataBlock == 0 );

ErrorExit:

    if (pbNextDataBlock != NULL)
    {
        Oem_MemFree( pbNextDataBlock );
    }

    SAFE_OEM_FREE(pbDecrypted);
    SAFE_OEM_FREE(pbClear);

    return dr;
}

/*
 * Note: this function gets the header object and
 * data object header.
 */
DRM_RESULT DRM_CALL Test_GetHeaders(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BYTE *pbNextDataBlock = NULL;
    DRM_DWORD cbNextDataBlock = 0;
    DRM_DWORD cbWritten;
    DRM_DWORD   tmp;
    DRM_DWORD cbDataObjectOffset = 0;

    /* Can only get the header info if this is a new session */
    ChkArg( g_FirstNextDataBlock == 1 );

    /* Get a chunk of data large enough to fit the header */
    cbNextDataBlock = MAXIMUM_HEADER_SIZE;
    pbNextDataBlock = (DRM_BYTE *)Oem_MemAlloc( cbNextDataBlock );
    ChkMem( pbNextDataBlock );

    ChkDR( Drm_Pfd_Aavt_GetNextDataBlock( NULL,
                                          g_pPfdDrmContext,
                                          g_dwMediaSessionID,
                                          &g_dwEndOfData,
                                          pbNextDataBlock,
                                          &cbNextDataBlock ) );
    g_FirstNextDataBlock = 0;

    /* Extract packet information from Data Object Header */
    DRM_BYT_CopyBytes(&cbDataObjectOffset, 0, pbNextDataBlock, 16, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD(cbDataObjectOffset);
    DRM_BYT_CopyBytes(&g_nDataPackets, 0, pbNextDataBlock, cbDataObjectOffset + 16 + 8 + 16, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD(g_nDataPackets);
    DRM_BYT_CopyBytes(&tmp, 0, pbNextDataBlock, cbDataObjectOffset + 16, SIZEOF(DRM_DWORD));
    FIX_ENDIAN_DWORD(tmp);
    g_dwDataPacketSize = (tmp - 50) / g_nDataPackets;
    Oem_File_Write( g_hOutputFile, pbNextDataBlock, cbNextDataBlock, &cbWritten );

    /* Rewind back to the first packet */
    g_dwTimeOffsetMilliseconds = 0;
    ChkDR( Drm_Pfd_Aavt_SetCurrentTimePosition( NULL,
                                                g_pPfdDrmContext,
                                                g_dwMediaSessionID,
                                                g_dwTimeOffsetMilliseconds ) );

ErrorExit:
    SAFE_OEM_FREE(pbNextDataBlock);

    return dr;
}

/**
 * argv[0]  = IN  DRM_DWORD       f_dwTimeOffsetInMilliseconds
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_SetCurrentTimePosition(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( argc == 1 );

    g_dwTimeOffsetMilliseconds = (DRM_DWORD) strtoul(argv[0], NULL, 0);
    ChkDR( Drm_Pfd_Aavt_SetCurrentTimePosition( NULL,
                                                g_pPfdDrmContext,
                                                g_dwMediaSessionID,
                                                g_dwTimeOffsetMilliseconds ) );

ErrorExit:

    return dr;
}


/* non pfddrmmanager and pfdaavt API functions that are helpful for testing */

/**
 * argv[0] = IN char *f_ValidationFile
 */
DRM_RESULT DRM_CALL Test_ValidateAllData(long argc, __in_ecount(argc) char *argv[])
{
    return DRM_SUCCESS;
}

/**
 * Validate that MTP messages are mapped correctly as expected.
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_GetMTPStatusFromDRMRESULT(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT drRes;

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_SUCCESS);
    ChkArg(drRes ==  0x2001 /* MTP_RESPONSECODE_OK */);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_INVALID_DEVICE_CERTIFICATE);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_INVALID_CERTIFICATE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_CERTIFICATE_REVOKED);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_CERTIFICATE_REVOKED);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_LICENSE_NOT_FOUND);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_LICENSE_UNAVAILABLE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_LICENSE_UNAVAILABLE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_DEVICE_LIMIT_REACHED);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_DEVICE_LIMIT_REACHED);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_UNABLE_TO_VERIFY_PROXIMITY);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_UNABLE_TO_VERIFY_PROXIMITY);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_DEVICE_NOT_REGISTERED);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_MUST_REGISTER);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_MUST_REVALIDATE);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_MUST_REVALIDATE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_INVALID_PROXIMITY_RESPONSE);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_INVALID_PROXIMITY_RESPONSE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_INVALID_SESSION);
    ChkArg(drRes ==  MTP_RESPONSECODE_AAVT_INVALID_MEDIA_SESSION_ID);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_FILEOPEN);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_UNABLE_TO_OPEN_FILE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_BAD_REQUEST);
    ChkArg(drRes ==  MTP_RESPONSECODE_WMDRMND_BAD_REQUEST);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_NOMORE);
    ChkArg(drRes ==  MTP_RESPONSECODE_AAVT_NO_MORE_DATA);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_ND_MEDIA_SESSION_LIMIT_REACHED);
    ChkArg(drRes ==  MTP_RESPONSECODE_AAVT_MEDIA_SESSION_LIMIT_REACHED);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(DRM_E_DEVICE_ALREADY_REGISTERED);
    ChkArg(drRes == MTP_RESPONSECODE_WMDRMND_TRANSMITTER_FAILURE);

    drRes = Drm_Pfd_GetMTPStatusFromDRMRESULT(0xffffffff);
    ChkArg(drRes == MTP_RESPONSECODE_WMDRMND_TRANSMITTER_FAILURE);

 ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(ConsoleNDTTests)(long lTCID, const char *strTCName)
{
    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        DRM_RESULT dr = DRM_SUCCESS;
        /* If you change the following strings, be sure to update the offsets
           in the CopyFileToDRMFolder_ndt calls so the second argument points
           to a string containing just the final path component. */
        DRM_CONST_STRING dstrDataStoreSrc = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrDataStoreDst = EMPTY_DRM_STRING;
        DRM_WCHAR rgwchPriv[] = {
            ONE_WCHAR('f', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'),
            ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\\','\0'),
            ONE_WCHAR('p', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'),
            ONE_WCHAR('v', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
            ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0','\0')
        };
        DRM_WCHAR rgwchDevCert[] = {
            ONE_WCHAR('f', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'),
            ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\\','\0'),
            ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'),
            ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'),
            ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
            ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')
        };
        DRM_WCHAR rgwchTemplate[] = {
            ONE_WCHAR('f', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('l', '\0'),
            ONE_WCHAR('e', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\\','\0'),
            ONE_WCHAR('d', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('v', '\0'),
            ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'),
            ONE_WCHAR('t', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'),
            ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'),
            ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'),
            ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
            ONE_WCHAR('t', '\0'), ONE_WCHAR('\0','\0')
        };
        DRM_WCHAR rgwchZgPriv[] = {
            ONE_WCHAR('z', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('p', '\0'),
            ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('v', '\0'),
            ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('a', '\0'),
            ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0')
        };
        DRM_WCHAR rgwchBGroupCert[] = {
            ONE_WCHAR('b', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('r', '\0'),
            ONE_WCHAR('o', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'),
            ONE_WCHAR('c', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('r', '\0'),
            ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('d', '\0'),
            ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0','\0')
        };

        DRM_TEST_OEM_CONTEXT *poTestOemContext = NULL;
        MAP_TO_GLOBAL_TEST_VAR_Alloc( poTestOemContext, NDT_TEST_OEM_CONTEXT_VAR, SIZEOF( DRM_TEST_OEM_CONTEXT ) );
        ZEROMEM( poTestOemContext, SIZEOF( DRM_TEST_OEM_CONTEXT ) );

        g_pCorrectPfdDrmContext = (PFDMGR_CONTEXT *) Oem_MemAlloc(SIZEOF(PFDMGR_CONTEXT));
        ChkMem(g_pCorrectPfdDrmContext)
        g_pPfdDrmContext = g_pCorrectPfdDrmContext;
        g_cbRegistrationRequestMsgOverride = 0;
        g_cbRegistrationResponseMsgOverride = 0;
        g_pbRegistrationResponseMsgNULL = 0;
        g_cbRegistrationResponseMsg = 1024;
        g_NonceOverride = 0;
        g_cbActionOverride = 0;
        g_cbLicenseRequestMsgOverride = 0;
        g_cbLicenseResponseMsgOverride = MAX_UNSIGNED_TYPE(DRM_DWORD);
        g_pbLicenseResponseMsgNULL = 0;
        g_cbLicenseResponseMsg = 1024;
        g_dwMediaSessionID = 0;
        g_dwCorrectMediaSessionID = 0;

        g_dwChallenge1 = 0;
        g_dwChallenge2 = 0;
        g_dwChallenge3 = 0;
        g_dwChallenge4 = 0;
        g_MessageTypeOverride = 0;
        g_MessageProtocolVersionOverride = 0;
        g_LastDataBlock = 0;

        ChkDR( TST_UTL_MakePathString( NULL, "files\\sample.hds", &dstrDataStoreSrc ) );
        ChkDR( TST_UTL_MakePathString( NULL, "sample.hds", &dstrDataStoreDst ) );

        ChkDR(TST_OEM_CopyFile(dstrDataStoreSrc,dstrDataStoreDst));
        ChkDR(_CopyFileToDRMFolder(rgwchPriv, &(rgwchPriv[6])));
        ChkDR(_CopyFileToDRMFolder(rgwchDevCert, &(rgwchDevCert[6])));
        ChkDR(_CopyFileToDRMFolder(rgwchTemplate, &(rgwchTemplate[6])));
        ChkDR(_CopyFileToDRMFolder(rgwchBGroupCert, &(rgwchBGroupCert[0])));
        ChkDR(_CopyFileToDRMFolder(rgwchZgPriv, &(rgwchZgPriv[0])));

ErrorExit:
        FREEDRMSTRING( dstrDataStoreSrc );
        FREEDRMSTRING( dstrDataStoreDst );
        return dr;
    }
    else
    {
        return DRM_E_NOTIMPL;
    }
}



DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(ConsoleNDTTests)(long lTCID, const char *strTCName)
{
    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        if( g_dwCorrectMediaSessionID != 0 )
        {
            Drm_Pfd_Aavt_CloseMediaSession( g_pCorrectPfdDrmContext, g_dwCorrectMediaSessionID );
            g_dwCorrectMediaSessionID = 0;
        }

        if (g_Initialized)
        {
            Drm_Pfd_Uninitialize(g_pCorrectPfdDrmContext);
            g_Initialized = 0;
        }
        if ( NULL != g_pCorrectPfdDrmContext )
        {
            Oem_MemFree(g_pCorrectPfdDrmContext);
            g_pCorrectPfdDrmContext = NULL;
        }

        if( NULL != g_pCertificate )
        {
            Oem_MemFree(g_pCertificate);
            g_pCertificate = NULL;
        }

        if ( g_hOutputFile != OEM_INVALID_HANDLE_VALUE )
        {
            Oem_File_Close( g_hOutputFile );
            g_hOutputFile = OEM_INVALID_HANDLE_VALUE;
        }

        if ( NULL != g_pPrivateKey )
        {
            OEM_RSA_ZeroPrivateKey_2048BIT(g_pPrivateKey);
            g_pPrivateKey = NULL;
        }

        if ( NULL != g_pContentEncryptionKey )
        {
            Oem_Aes_ZeroKey((DRM_AES_KEY *) g_pContentEncryptionKey);
            SAFE_OEM_FREE( g_pContentEncryptionKey );
        }

        if( NULL != g_pXmrLicense )
        {
            DrmXmrFreeLicense( g_pXmrLicense );
            g_pXmrLicense = NULL;
        }

        SAFE_OEM_FREE( g_dstrHdsPath.pwszString );

        return DRM_SUCCESS;
    }
    else
    {
        return DRM_E_NOTIMPL;
    }
}


BEGIN_TEST_API_LIST(ConsoleNDTTests)
    API_ENTRY(Test_SetpPfdDrmContext)
    API_ENTRY(Test_SetCertificate)
    API_ENTRY(Test_SetDeviceCertificate)
    API_ENTRY(Test_SetDataStore)
    API_ENTRY(Test_SetcbCertificate)
    API_ENTRY(Test_SetcbAction)
    API_ENTRY(Test_SetTransactionID)
    API_ENTRY(Test_SetRegistrationSessionID)
    API_ENTRY(Test_SetdwMediaSessionID)
    API_ENTRY(Test_SetPrivateKey)
    API_ENTRY(Test_SetPrivateKeyDirectly)
    API_ENTRY(Test_SetSerialNumber)
    API_ENTRY(Test_SetNonce)
    API_ENTRY(Test_SetcbRegistrationRequestMsg)
    API_ENTRY(Test_SetcbRegistrationResponseMsg)
    API_ENTRY(Test_SetcbLicenseRequestMsg)
    API_ENTRY(Test_SetcbLicenseResponseMsg)
    API_ENTRY(Test_SetMessageType)
    API_ENTRY(Test_SetMessageProtocolVersion)
    API_ENTRY(Test_Sleep)
    API_ENTRY(Test_SetRevocationList)
    API_ENTRY(Test_GetRevocationList)
    API_ENTRY(Test_GetRevocationInfo)
    API_ENTRY(Test_CheckdwControlCapabilities)
    API_ENTRY(Test_CheckdwDRMStatus)
    API_ENTRY(Test_CheckdwLicenseStatus)
    API_ENTRY(Test_CheckdwDeliveryOptions)
    API_ENTRY(Test_CheckLastDataBlock)
    API_ENTRY(Test_CheckNotTranscrypted)
    API_ENTRY(Test_CheckSourceID)
    API_ENTRY(Test_CheckDeviceKey)
    API_ENTRY(Test_CheckSettingsObject)
    API_ENTRY(Test_CheckInclusionList)
    API_ENTRY(Test_DRM_PFD_Initialize)
    API_ENTRY(Test_DRM_PFD_Uninitialize)
    API_ENTRY(Test_Drm_Pfd_Registration_ProcessRequest)
    API_ENTRY(Test_Drm_Pfd_Registration_GenerateResponse)
    API_ENTRY(Test_Proximity)
    API_ENTRY(Test_Drm_Pfd_Proximity_GenerateChallenge)
    API_ENTRY(Test_Drm_Pfd_Proximity_ProcessResponse)
    API_ENTRY(Test_Drm_Pfd_License_ProcessRequest)
    API_ENTRY(Test_Drm_Pfd_License_GenerateResponse)
    API_ENTRY(Test_CheckXMRLicenseMinEnvironment)
    API_ENTRY(Test_Drm_Pfd_Aavt_OpenMediaSession)
    API_ENTRY(Test_Drm_Pfd_Aavt_CloseMediaSession)
    API_ENTRY(Test_Drm_Pfd_Aavt_GetNextDataBlock)
    API_ENTRY(Test_Drm_Pfd_Aavt_GetAllDataBlocks)
    API_ENTRY(Test_GetHeaders)
    API_ENTRY(Test_Drm_Pfd_Aavt_SetCurrentTimePosition)
    API_ENTRY(Test_ValidateAllData)
    API_ENTRY(Test_Drm_Pfd_GetMTPStatusFromDRMRESULT)
    API_ENTRY(Test_SetpbRegistrationResponseMsgNULL)
    API_ENTRY(Test_SetpbLicenseResponseMsgNULL)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

static DRM_BOOL DRM_CALL _AreDeviceStoreAndWmdrmnetSupported( DRM_VOID )
{
    return DRM_DVS_IsDeviceStoreSupported() && DRM_WMDRMNET_IsWmdrmnetSupported();
}

/* BEGIN future contents of ConsoleNDT's main.c module */
IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY_CONDITIONAL("FunctionalTest", _AreDeviceStoreAndWmdrmnetSupported)
#if PFD_MAX_CONTENT_SESSIONS == 5
    TEST_SCRIPT_ENTRY_CONDITIONAL("FunctionalTest-MaxContentSessions-5", _AreDeviceStoreAndWmdrmnetSupported)
#else   /* PFD_MAX_CONTENT_SESSIONS */
    TEST_SCRIPT_ENTRY_CONDITIONAL("FunctionalTest-MaxContentSessions-1", _AreDeviceStoreAndWmdrmnetSupported)
#endif  /* PFD_MAX_CONTENT_SESSIONS */
END_TEST_SCRIPT_LIST


BEGIN_TEST_MAIN(ConsoleNDTTests)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(ConsoleNDTTests)
        PKTEST_MODULE_ENTRY(DrmManager)
    END_TEST_MODULE_LIST
END_TEST_MAIN
/* END future contents of ConsoleNDT's main.c module */

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

