/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __MPEG2TSENCRYPTORTESTAPI_H
#define __MPEG2TSENCRYPTORTESTAPI_H

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

/*
** Shared Variable Names
*/
#define TEST_VAR_M2TS_ENCRYPTOR_HANDLE              "M2TSEncryptorHandle"
#define TEST_VAR_M2TS_PACKETS_BUFFER                "M2TSPacketsBuffer"
#define TEST_VAR_M2TS_PACKETS_BUFFER_SIZE           "M2TSPacketsBufferSize"
#define TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET         "M2TSPacketsBufferOffset"
#define TEST_VAR_M2TS_ENCRYPTED_BUFFER              "M2TSEncryptedBuffer"
#define TEST_VAR_M2TS_ENCRYPTED_BUFFER_SIZE         "M2TSEncryptedBufferSize"
#define TEST_VAR_M2TS_ENCRYPTED_BUFFER_OFFSET       "M2TSEncryptedBufferOffset"
#define TEST_VAR_M2TS_PAT_CC                        "M2TSPATCC"
#define TEST_VAR_M2TS_PMT_CC                        "M2TSPMTCC"
#define TEST_VAR_M2TS_ECM_TEST_DATA                 "M2TSECMTestData"
#define TEST_VAR_M2TS_ECM_COUNT                     "M2TSECMCount"

/* Test Encryptor APIs */
DRM_RESULT DRM_CALL TestEncryptor_Init( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_Uninitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_Encrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_EncryptWithMoreData( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BinaryReader( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BinaryReaderWithPacketList( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_ParsePacketHeader( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_GetFreePacket( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_TSPacketHeaderParser( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_RebuildPMTPacket( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_SetKeyData( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_SetEncryptAudio( long argc, __in_ecount( argc ) char **argv );

/* Test helpers */
DRM_RESULT DRM_CALL TestEncryptor_BuildPAT( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_VerifyPAT( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BuildPMT( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BuildIncompletedPMT( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_VerifyPMT( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_VerifyPMTAfterRebuilt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BuildMPEG2PES( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BuildAudioPES( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_BuildH264PES( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_ParseECM( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_VerifyECM( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_ClearPacketsBuffer( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_ClearEncryptedBuffer( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_InjectDataIntoPacketsBuffer( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestEncryptor_InsertRandomBytes( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS( M2TSENCRYPTOR )

EXIT_PKTEST_NAMESPACE

#endif /* __MPEG2TSENCRYPTORTESTAPI_H */
