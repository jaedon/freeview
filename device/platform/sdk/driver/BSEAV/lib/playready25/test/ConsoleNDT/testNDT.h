/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __TESTNDT_H
#define __TESTNDT_H

#include "PKTestNameSpace.h"
ENTER_PKTEST_NAMESPACE

#define NDT_TEST_OEM_CONTEXT_VAR "SharedNDTOEMTestContext"

/* helper functions */

DRM_RESULT DRM_CALL Test_SetpPfdDrmContext(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbCertificate(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetCertificate(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetDeviceCertificate(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetDataStore(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetPrivateKey(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbAction(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetTransactionID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetRegistrationSessionID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetdwMediaSessionID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetSerialNumber(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetNonce(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbRegistrationRequestMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbRegistrationResponseMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbRegistrationResponseMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbLicenseRequestMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetcbLicenseResponseMsg(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetpbLicenseResponseMsgNULL(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetMessageType(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetMessageProtocolVersion(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Sleep(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_SetRevocationList(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_GetRevocationList(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_GetRevocationInfo(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_CheckdwControlCapabilities(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckdwDRMStatus(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckdwLicenseStatus(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckdwDeliveryOptions(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckLastDataBlock(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckDecryption(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckNotTranscrypted(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckSourceID(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckDeviceKey(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckSettingsObject(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_CheckInclusionList(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_CheckXMRLicenseMinEnvironment(long argc, __in_ecount(argc) char *argv[]);

/* drmpfdmanager.h */

/**
 *  argv[0] = DeviceStoreName
 */
DRM_RESULT DRM_CALL Test_DRM_PFD_Initialize(long argc, __in_ecount(argc) char *argv[]);

/**
 *  No parameters
 */
DRM_RESULT DRM_CALL Test_DRM_PFD_Uninitialize(long argc, __in_ecount(argc) char *argv[]);


/**
 * argv[0] = SerialNumber
 * argv[1] = CertificatPath
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Registration_ProcessRequest(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Registration_GenerateResponse(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] = TESTOUTPUTPARAM  f_checkChallenge
 * argv[1] = DRM_DWORD        f_pdwChallenge1
 * argv[2] = DRM_DWORD        f_pdwChallenge2
 * argv[3] = DRM_DWORD        f_pdwChallenge3
 * argv[4] = DRM_DWORD        f_pdwChallenge4
 *           ignored if argv[0] == TESTOUTPUTPARAM_IGNORE
 *           compared with argv[1..4] if argv[0] == TESTOUTPUTPARAM_USE_SPECIFIED
 */
DRM_RESULT DRM_CALL Test_Proximity(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_Drm_Pfd_Proximity_GenerateChallenge(long argc, __in_ecount(argc) char *argv[]);
DRM_RESULT DRM_CALL Test_Drm_Pfd_Proximity_ProcessResponse(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0] = DRM_ID           f_drmRightsID
 * argv[1] = char            *f_DeviceCertificatePath
 * argv[2] = char            *f_Action
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_License_ProcessRequest(long argc, __in_ecount(argc) char *argv[]);

/**
 * No parameters
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_License_GenerateResponse(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0]  = IN  char           *f_pszInputFileName
 * argv[1]  = IN  DRM_DWORD       f_dwForceNewSession
 * argv[2]  = IN char             f_pszOutputFileName
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_OpenMediaSession(long argc, __in_ecount(argc) char *argv[]);

/**
 * no parameters
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_CloseMediaSession(long argc, __in_ecount(argc) char *argv[]);

/*
 * argv[0] = data block size in bytes
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_GetNextDataBlock(long argc, __in_ecount(argc) char *argv[]);

/*
 * argv[0] = data block size in bytes
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_GetAllDataBlocks(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_GetHeaders(long argc, __in_ecount(argc) char *argv[]);

/**
 * argv[0]  = IN  DRM_DWORD       f_dwTimeOffsetInMilliseconds
 */
DRM_RESULT DRM_CALL Test_Drm_Pfd_Aavt_SetCurrentTimePosition(long argc, __in_ecount(argc) char *argv[]); /* not supported yet */


/* non pfddrmmanager and pfdaavt API functions that are helpful for testing */

/**
 * argv[0] = IN char *f_ValidationFile
 */
DRM_RESULT DRM_CALL Test_ValidateAllData(long argc, __in_ecount(argc) char *argv[]);

DRM_RESULT DRM_CALL Test_Drm_Pfd_GetMTPStatusFromDRMRESULT(long argc, __in_ecount(argc) char *argv[]);

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


DECLARE_MODULE_DEFS(ConsoleNDTTests)

EXIT_PKTEST_NAMESPACE

#endif /*__TESTNDT_H*/