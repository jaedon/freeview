/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __FUNCTIONALTEST_H
#define __FUNCTIONALTEST_H

#include "tclib.h"
#include "drmndtmanager.h"

#define MAXNDTCONTEXTS 5
#define MAXMEDIASESSIONS 11 /* on extra for a negative test */

#ifndef MAX_STREAMS_IN_ASF
#define MAX_STREAMS_IN_ASF  127
#endif  /* MAX_STREAMS_IN_ASF */

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

typedef struct __tagTEMP_ASF_SAMPLE
{
    DRM_DWORD      dwPresentationTime;
    DRM_BYTE      *pbData;
    DRM_DWORD      cbDataNeed;
    DRM_DWORD      cbDataHave;
} TEMP_ASF_SAMPLE;

DRM_RESULT DRM_CALL Test_SetpNdtDrmContext(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbCertificate(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetCertificate(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetPrivateKey(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetPrivateKeyDirectly(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetRegistrationSessionID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetMediaSessionID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetMediaSessionIDNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetASFFlagsNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetSerialNumber(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetChallengeID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbRegistrationRequestMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbRegistrationRequestMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbRegistrationResponseMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbRegistrationResponseMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetProximityStartMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetProximityResponseMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetTestProximityBUFFERTOOSMALL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbLicenseChallengeMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbLicenseResponseMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbLicenseResponseMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbLicenseResponseMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetMessageType(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetMessageProtocolVersion(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_ChangeTimeMilliseconds(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetRevocationList(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbDataNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpcbDataNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbSampleDataNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpcbSampleDataNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbSampleData(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpSampleInfoNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbHeaderNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpcbHeaderNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbHeader(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbLeafLicenseResponseNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpcbLeafLicenseResponseNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbLeafLicenseResponse(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpKIDNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetWMDRMNETPolicy(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetProximityDetectionAddress(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_AllowNewDeviceRegistration(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_CheckASFFlags(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckLastDataBlock(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckNotTranscrypted(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckSourceID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckRivInLicense(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckDeviceKey(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckSettingsObject(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckInclusionList(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL CloseClearSampleFile();

/* drmndtmanager.h */

/**
 *  argv[0] = InitData
 */
DRM_RESULT DRM_CALL Test_DRM_NDT_Initialize(long argc, __in_ecount(argc) char *argv[]);

/**
 *  No parameters
 */
DRM_RESULT DRM_CALL Test_DRM_NDT_Uninitialize(long argc, __in_ecount(argc) char *argv[]);

/**
 *  argv[0] = context number to switch to (0 thru MAXNDTCONTEXTS - 1)
 */
DRM_RESULT DRM_CALL Test_SwitchNdtContexts(long argc, __in_ecount(argc) char *argv[]);

/**
 *  argv[0] = session number to switch to (0 thru MAXMEDIASESSIONS - 1)
 */
DRM_RESULT DRM_CALL Test_SwitchMediaSession(long argc, __in_ecount(argc) char *argv[]);

/**
 *  argv[0] = string determining whether this will be a REGISTRATION or CONTENT session
 */
DRM_RESULT DRM_CALL Test_NDTMGR_CreateSession(long argc, __in_ecount(argc) char *argv[]);

/**
 *  argv[0] = string determining whether this will be a REGISTRATION or CONTENT session
 */
DRM_RESULT DRM_CALL Test_NDTMGR_CloseSession(long argc, __in_ecount(argc) char *argv[]);

/**
 *  No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessRegistrationRequest(long argc, __in_ecount(argc) char *argv[]);

/**
 * This function does the GetProximityChallenge, ProcessProximityResponse in one fell
 * swoop, in case the finer grain functions Test_NDTMGR_GetProximityChallenge and
 * Test_NDTMGR_GetProximityResponse are too slow to meet the 7ms deadline for
 * Proximity.
 *
 * argv[0] = sleepage
 */
DRM_RESULT DRM_CALL Test_Proximity(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessProximityStartMsg(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessProximityResponseMsg(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] - rights id
 * argv[1] - action
 * argv[2] - license mode
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessLicenseChallenge(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_GetLeafLicenseResponse(long argc, __in_ecount(argc) char *argv[]);


/* ndtencrypt.h */

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_SetCryptoDataNULL(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] - encryption mode
 */
DRM_RESULT DRM_CALL Test_SetEncryptionMode(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0]  = IN  char         filename of file to encrypt
 * argv[1]  = IN  DRM_UINT64   sample ID
 * argv[2]  = IN  DRM_UINT64   offset
 */
DRM_RESULT DRM_CALL Test_NDTMGR_Encrypt_ASF(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0]  = IN  char         filename of file to encrypt
 * argv[1]  = IN  DRM_UINT64   sample ID
 * argv[2]  = IN  DRM_UINT64   offset
 */
DRM_RESULT DRM_CALL Test_NDTMGR_Encrypt_MPEG(long argc, __in_ecount(argc) char *argv[]);


/* drmndtasf.h */

/**
 * argv[0]  = IN  char           *f_pszInputFileName
 * argv[1]  = IN  DRM_DWORD       f_dwForceNewSession
 * argv[2]  = IN  char            f_pszOutputFileName
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_Init(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] - rights id
 * argv[1] - action
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_InitTranscryption(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_Close(long argc, __in_ecount(argc) char *argv[]);

/*
 * argv[0] = data block size in bytes
 * Note: you must call Test_GetHeaders before calling this function with the following argument
 * argv[1] = (optional) path to clear sample file
*/
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadData(long argc, __in_ecount(argc) char *argv[]);

/*
 * argv[0] = data block size in bytes
 * Note: you must call Test_GetHeaders before calling this function with the following arguments
 * argv[1] = (optional) path to clear sample file
 * argv[2] = (optional) number of samples we expect in the provided file with clear samples
*/
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadAllData(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] = sample size in bytes
 * argv[1] = (optional) file with clear samples
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadSample(long argc, __in_ecount(argc) char *argv[]);

/*
 * argv[0] = sample size in bytes
 * argv[1] = (optional) file with clear samples
 * argv[2] = (optional) number of samples we expect in the provided file with clear samples
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_ReadAllSamples(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0]  = IN  DRM_UINT64       f_dwTimeOffsetMilliseconds
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_Seek(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_GetLeafLicenseResponse(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] = (optional) path of ASF file to compare the header
 */
DRM_RESULT DRM_CALL Test_NDTMGR_ASF_GetHeader(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_GetHeaders(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_LinkEncryption(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_NDTMGR_ProcessRevocationListResponse(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_NDTMGR_GetRevocationListRequest(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_AddRevocationListToResponse(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_InitializeRevocationListResponse(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListResponseMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_WmdrmNetPackRevocationListRequestMessage(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_NDTMGR_ProcessRevocationListRequest(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetNoHandleBufferTooSmall( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_CheckcbRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_SetpcbRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_SetcbRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_SetcbRevocationListRequest( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_SetpcbRevocationListRequest( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_VerifyRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_CheckRivSequenceNumber( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_CopyFileToDRMFolder( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_DumpRevocationListResponse( long argc, __in_ecount(argc) char *argv[] );

DRM_RESULT DRM_CALL Test_NDT_RetrieveLicenseDataV2( long argc, __in_ecount(argc) char *argv[] );
DRM_RESULT DRM_CALL Test_SetFailAllocation( long argc, __in_ecount(argc) char *argv[] );

typedef struct __tagASFPACKET_PARSER_INFO
{
    DRM_BOOL    fParityPacket;
    DRM_BOOL    fEccPresent;
    DRM_BOOL    fMultiPayloads;
    DRM_DWORD   dwParseOffset;
    DRM_BYTE    bECLen;
    DRM_BYTE    bPacketLenType;
    DRM_BYTE    bPadLenType;
    DRM_BYTE    bSequenceLenType;
    DRM_DWORD   dwPacketLenTypeOffset;
    DRM_BYTE    bOffsetBytes;
    DRM_BYTE    bOffsetLenType;
    DRM_BYTE    bPayLenType;
    DRM_BYTE    bPayBytes;
    DRM_DWORD   dwPacketLenOffset;
    DRM_DWORD   dwExplicitPacketLength;
    DRM_DWORD   dwSequenceOffset;
    DRM_DWORD   dwSequenceNum;
    DRM_DWORD   dwPadLenOffset;
    DRM_DWORD   dwPadding;
    DRM_DWORD   dwSCR;
    DRM_DWORD   dwPayLenTypeOffset;
    DRM_DWORD   dwPayloads;
    DRM_WORD     wDuration;
} ASFPACKET_PARSER_INFO;

typedef struct __tagASFPAYLOAD_PARSER_INFO
{
    DRM_WORD     wPacketOffset;
    DRM_WORD     wTotalSize;

    DRM_BYTE     bStreamId;
    DRM_BYTE     bObjectId;
    DRM_BYTE     bRepData;
    DRM_BYTE     bJunk;

    DRM_DWORD    dwObjectOffset;
    DRM_DWORD    dwObjectSize;
    DRM_DWORD    dwObjectPres;

    DRM_BOOL     fIsKeyFrame;
    DRM_BOOL     fIsCompressedPayload;
    DRM_WORD     wPayloadSize;
    DRM_WORD     wTotalDataBytes;
    DRM_DWORD    dwDeltaPresTime;

    DRM_WORD     wBytesRead;
    DRM_BYTE     bSubPayloadState;
    DRM_BYTE     bNextSubPayloadSize;
    DRM_WORD     wSubpayloadLeft;
    DRM_WORD     wSubCount;
    DRM_BOOL     fEncrypted;
} ASFPAYLOAD_PARSER_INFO;

struct __tagND_CLEANUP_OBJECT;
typedef struct __tagND_TRANSMITTER_CONTEXT
{
    DRM_STRING       dstrInitData;

    NDTMGR_CONTEXT  *pNdtMgrContext;
    DRM_BOOL         fInitialized;    
    DRM_BOOL         fSharedAppContext;
    DRM_APP_CONTEXT *DrmMgrContext;
    struct __tagND_CLEANUP_OBJECT *pCleanupHead;
} ND_TRANSMITTER_CONTEXT;


typedef enum __tagND_CLEANUP_OBJECT_TYPE
{
    NdcPolicy,
    NdcNdtSession,
    NdcNdtAsfSession
} ND_CLEANUP_OBJECT_TYPE;

typedef struct __tagND_CLEANUP_OBJECT
{
    struct __tagND_CLEANUP_OBJECT *pNext;
    ND_CLEANUP_OBJECT_TYPE         eObjectType;
    DRM_VOID                      *pArg;
} ND_CLEANUP_OBJECT;

DECLARE_MODULE_DEFS(NDTTestSuite)

EXIT_PKTEST_NAMESPACE

#endif /*__FUNCTIONALTEST_H*/
