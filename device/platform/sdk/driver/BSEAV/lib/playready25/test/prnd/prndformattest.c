/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <prndformattest.h>
#include <drmprnd.h>
#include <drmprndformat.h>
#include <drmprndprotocoltypes.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define TEST_VAR_PRND_STACK     "PRNDStack"
#define TEST_SIGNATURE_CONST    0xF7
#define TEST_SESSIONID_CONST    0x2E
#define TEST_OTHERGUID_CONST1   0x4A
#define TEST_OTHERGUID_CONST2   0xC6
#define TEST_OTHERGUID_CONST3   0xD1
#define TEST_OTHERGUID_CONST4   0x93

        /* Note: "PRND" == 0x50524E44 */
#define VERIFY_PRND_HEADER( pbMsg, iMsg, dwMessageConst, cbMessage )                                                    \
    do {                                                                                                                \
        DRM_DWORD dwVar = 0;                                                                                            \
        dwVar = MAKE_DWORD_BIG_ENDIAN(0x50524E44);                                                                      \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &dwVar,  SIZEOF(DRM_DWORD) ),    DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_DWORD);                                                                                      \
        dwVar = MAKE_DWORD_BIG_ENDIAN(dwMessageConst);                                                                  \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &dwVar,  SIZEOF(DRM_DWORD) ),    DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_DWORD);                                                                                      \
        dwVar = MAKE_DWORD_BIG_ENDIAN(0x00000001);                                                                      \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &dwVar,  SIZEOF(DRM_DWORD) ),    DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_DWORD);                                                                                      \
        dwVar = MAKE_DWORD_BIG_ENDIAN(cbMessage);                                                                       \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &dwVar,  SIZEOF(DRM_DWORD) ),    DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_DWORD);                                                                                      \
    } while( FALSE )

        /* Note: Verify sub-object size only when cbObject != 0 (indicating a fixed-size object). */
#define VERIFY_PRND_OBJECT_HEADER( wFlags, wObjectType, pbMsg, iMsg, cbObject )                                         \
    do {                                                                                                                \
        DRM_WORD  wVar  = 0;                                                                                            \
        DRM_DWORD dwVar = 0;                                                                                            \
        wVar = MAKE_WORD_BIG_ENDIAN(wFlags);                                                                            \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &wVar,  SIZEOF(DRM_WORD) ),     DRM_E_TEST_UNEXPECTED_OUTPUT );      \
        iMsg += SIZEOF(DRM_WORD);                                                                                       \
        wVar = MAKE_WORD_BIG_ENDIAN(wObjectType);                                                                       \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &wVar,  SIZEOF(DRM_WORD) ),     DRM_E_TEST_UNEXPECTED_OUTPUT );      \
        iMsg += SIZEOF(DRM_WORD);                                                                                       \
        dwVar = MAKE_DWORD_BIG_ENDIAN(cbObject);                                                                        \
        ChkBOOL( 0 == cbObject                                                                                          \
              || 0 == DRMCRT_memcmp( &pbMsg[iMsg], &dwVar,  SIZEOF(DRM_DWORD) ),   DRM_E_TEST_UNEXPECTED_OUTPUT );      \
        iMsg += SIZEOF(DRM_DWORD);                                                                                      \
    } while( FALSE )

#define VERIFY_PRND_WORD( w, pbMsg, iMsg )                                                                              \
    do {                                                                                                                \
        DRM_WORD wVar = MAKE_WORD_BIG_ENDIAN(w);                                                                        \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &wVar,   SIZEOF(DRM_WORD) ),     DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_WORD);                                                                                       \
    } while( FALSE )

#define VERIFY_PRND_DWORD( dw, pbMsg, iMsg )                                                                            \
    do {                                                                                                                \
        DRM_DWORD dwVar = MAKE_DWORD_BIG_ENDIAN(dw);                                                                    \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &dwVar,  SIZEOF(DRM_DWORD) ),    DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_DWORD);                                                                                      \
    } while( FALSE )

#define VERIFY_PRND_QWORD( qw, pbMsg, iMsg )                                                                            \
    do {                                                                                                                \
        DRM_UINT64 qwVar = MAKE_QWORD_BIG_ENDIAN(qw);                                                                   \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &qwVar,  SIZEOF(DRM_UINT64) ),   DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(DRM_UINT64);                                                                                     \
    } while( FALSE )

#define VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgb, pbMsg, iMsg )                                                           \
    do {                                                                                                                \
        VERIFY_PRND_WORD( SIZEOF(rgb), pbMsg, iMsg );                                                                   \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], rgb,     SIZEOF(rgb) ),          DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(rgb);                                                                                            \
    } while( FALSE )

#define VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgb, pbMsg, iMsg )                                                          \
    do {                                                                                                                \
        VERIFY_PRND_DWORD( SIZEOF(rgb), pbMsg, iMsg );                                                                  \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], rgb,     SIZEOF(rgb) ),          DRM_E_TEST_UNEXPECTED_OUTPUT );     \
        iMsg += SIZEOF(rgb);                                                                                            \
    } while( FALSE )

#define VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( b, cb, pbMsg, iMsg )                                                      \
    do {                                                                                                                \
        DRM_DWORD idx = 0;                                                                                              \
        for( idx = 0; idx < cb; idx++ )                                                                                 \
        {                                                                                                               \
            DRM_BYTE bTemp = b;                                                                                         \
            ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], &bTemp,  SIZEOF(DRM_BYTE) ), DRM_E_TEST_UNEXPECTED_OUTPUT );     \
            iMsg += SIZEOF(DRM_BYTE);                                                                                   \
        }                                                                                                               \
    } while( FALSE )

#define VERIFY_PRND_CUSTOM_DATA_TYPE( customTypeID, pbMsg, iMsg )                                                       \
    do {                                                                                                                \
        ChkBOOL( 0 == DRMCRT_memcmp( &pbMsg[iMsg], customTypeID, SIZEOF(DRM_ID) ), DRM_E_TEST_UNEXPECTED_OUTPUT );      \
        iMsg += SIZEOF(DRM_ID);                                                                                         \
    } while( FALSE )

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(prndformattest)(long lTCID, const char *strTCName)
{
    DRM_RESULT   dr         = DRM_SUCCESS;
    DRM_BYTE    *pbStack    = NULL;
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbStack, TEST_VAR_PRND_STACK, DRM_PRND_STACK_SIZE );
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(prndformattest)(long lTCID, const char *strTCName)
{
    return DRM_SUCCESS;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const")
static DRM_RESULT DRM_CALL _SignatureCB(
    __in_ecount( 1 )                      DRM_VOID                      *f_pvSignatureContext,
    __in                                  DRM_PRND_SIGNATURE_OPERATION   f_eSignatureOperation,
    __in_bcount( f_cbData )         const DRM_BYTE                      *f_pbData,
    __in                                  DRM_DWORD                      f_cbData,
    __out_bcount( f_cbSignatureBuffer )   DRM_BYTE                      *f_pbSignatureBuffer,
    __in                                  DRM_DWORD                      f_cbSignatureBuffer,
    __in                                  DRM_DWORD                      f_iSignatureBuffer,
    __in_opt                        const DRM_XB_BYTEARRAY              *f_pxbbaMIMKEncryptedWithReceiverPubkey,
    __in_opt                        const DRM_XB_BYTEARRAY              *f_pxbbaReceiverCertificate )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_WORD    eOp     = 0;
    DRM_DWORD   cbSig   = 0;

    DRMCASSERT( DRM_PRND_PROXIMITY_DETECTION_TYPE_UDP                                   == 0x0001 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_PROXIMITY_DETECTION_TYPE_TCP                                   == 0x0002 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_PROXIMITY_DETECTION_TYPE_TRANSPORT_AGNOSTIC                    == 0x0004 );    /* Const must match spec */

    DRMCASSERT( DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY                     == 0x0001 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI                                    == 0x0002 );    /* Const must match spec */

    DRMCASSERT( DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY                     == 0x0040 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI                                    == 0x0010 );    /* Const must match spec */

    DRMCASSERT( DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID                                    == 0x0001 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_CONTENT_IDENTIFIER_TYPE_CUSTOM                                 == 0x0002 );    /* Const must match spec */

    DRMCASSERT( DRM_PRND_ENCRYPTION_TYPE_ECC256_RECEIVER_PRND_ENCRYPTION_KEY            == 0x0001 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENCRYPTION_TYPE_AES_ECB_MK                                     == 0x0002 );    /* Const must match spec */

    DRMCASSERT( XB_FLAGS_MUST_UNDERSTAND                                                == 0x0001 );    /* Const must match spec */
    DRMCASSERT( XB_FLAGS_CONTAINER                                                      == 0x0002 );    /* Const must match spec */

    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_BASIC_INFORMATION              == 0x0001 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_CUSTOM_DATA                    == 0x0002 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_SIGNATURE                      == 0x0003 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_BASIC_INFORMATION             == 0x0004 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION         == 0x0005 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_CUSTOM_DATA                   == 0x0006 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_SIGNATURE                     == 0x0007 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_TRANSMITTER_AUTH              == 0x0017 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_REGISTRATION_ERROR_BASIC_INFORMATION                == 0x0008 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_START_BASIC_INFORMATION         == 0x0009 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION     == 0x000A );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION      == 0x000B );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION        == 0x000C );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_BASIC_INFORMATION                   == 0x000D );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_LICENSE_INFORMATION                 == 0x000E );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_CUSTOM_DATA                         == 0x000F );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_SIGNATURE                           == 0x0010 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_BASIC_INFORMATION                  == 0x0011 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_LIST                       == 0x0012 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION                == 0x0013 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_CUSTOM_DATA                        == 0x0014 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_SIGNATURE                          == 0x0015 );    /* Const must match spec */
    DRMCASSERT( DRM_PRND_ENTRY_TYPE_LICENSE_ERROR_BASIC_INFORMATION                     == 0x0016 );    /* Const must match spec */

    ChkBOOL( f_pvSignatureContext                       != NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    eOp = *((const DRM_WORD*)f_pvSignatureContext);

    switch( eOp )
    {
    case DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_ECC256_RECEIVER_SIGNING_KEY:
        cbSig = DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY;
        ChkBOOL( f_pxbbaMIMKEncryptedWithReceiverPubkey     == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pxbbaReceiverCertificate                 == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        break;
    case DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_ECC256_RECEIVER_SIGNING_KEY:
        cbSig = DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY;
        ChkBOOL( f_pxbbaMIMKEncryptedWithReceiverPubkey     == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pxbbaReceiverCertificate                 != NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        break;
    case DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI:
        cbSig = DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI;
        ChkBOOL( f_pxbbaMIMKEncryptedWithReceiverPubkey     == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pxbbaReceiverCertificate                 == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        break;
    case DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI:
        cbSig = DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI;
        ChkBOOL( f_pxbbaMIMKEncryptedWithReceiverPubkey     != NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pxbbaReceiverCertificate                 == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        break;
    case DRM_PRND_SIGNATURE_OPERATION_INVALID:
        cbSig = DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI;
        ChkBOOL( f_pxbbaMIMKEncryptedWithReceiverPubkey     == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( f_pxbbaReceiverCertificate                 == NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
        eOp = DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI;
        break;
    default:
        ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        break;
    }

    ChkBOOL( f_eSignatureOperation                      == eOp,     DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_pbData                                   != NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_cbData                                   > 0,        DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_pbSignatureBuffer                        != NULL,    DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( f_cbSignatureBuffer                        == cbSig,   DRM_E_TEST_UNEXPECTED_OUTPUT );

                                                                                 /* Base offset          wSignatureType     xbbaSignature.cbData (with XB_SIZE_RELATIVE_WORD) */
    ChkBOOL( f_pbSignatureBuffer + f_iSignatureBuffer   == f_pbData + f_cbData + XB_BASE_OBJECT_LENGTH + SIZEOF(DRM_WORD) + SIZEOF(DRM_WORD),   DRM_E_TEST_UNEXPECTED_OUTPUT );

    DRMCRT_memset( f_pbSignatureBuffer + f_iSignatureBuffer, TEST_SIGNATURE_CONST, f_cbSignatureBuffer );

ErrorExit:
    return dr;
}

#define TestPRNDFormatRegistrationRequest_Arg0_FLAGS_CustomData     0x00000001

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDFormatRegistrationRequest_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDFormatRegistrationRequest( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_BYTE                 rgbCert[]      = { 'C', 'E', 'R', 'T' };
    DRM_BYTE                 rgbCustom[]    = { 'C', 'D' };
    DRM_ID                   customTypeID   = { 0x76, 0xa3, 0x4b, 0x41, 0xc0, 0x5f, 0x4c, 0xc4, 0xbb, 0xf3, 0x6c, 0x98, 0x95, 0x2, 0xff, 0x79 };
    DRM_DWORD                iMsg           = 0;
    DRM_WORD                 wCtx           = DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_ECC256_RECEIVER_SIGNING_KEY;
    DRM_BOOL                 fCustomData    = FALSE;
    DRM_PRND_REGISTRATION_REQUEST_BASIC_INFORMATION oBasicInfo  = {0};
    DRM_PRND_REGISTRATION_REQUEST_MESSAGE           oMsg        = {0};
    DRM_PRND_CUSTOM_DATA                            oCustomData = {0};

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fCustomData     = 0 != ( dwArg0Flags & TestPRNDFormatRegistrationRequest_Arg0_FLAGS_CustomData );
    }

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_RegistrationRequestMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid                               = TRUE;
    oBasicInfo.dwFlags                              = 0xEAC82134;
    DRMCRT_memset( &oBasicInfo.guidPreviousSessionID, TEST_OTHERGUID_CONST3, SIZEOF( DRM_GUID ) );
    oBasicInfo.xbbaReceiverCertificate.pbDataBuffer = rgbCert;
    oBasicInfo.xbbaReceiverCertificate.cbData       = SIZEOF( rgbCert );
    oBasicInfo.dwReceiverRIV                        = 0xD3257A;
    oBasicInfo.dwSupportedProximityDetectionTypes   = 0xC96426D1;
    DRMCRT_memset( &oBasicInfo.guidInitialLicenseMessageID, TEST_OTHERGUID_CONST4, SIZEOF( DRM_GUID ) );
    oBasicInfo.qwReceiverCurrentTime                = DRM_UI64HL( 0x9D6BCE4C, 0x44F9B3CF );
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_BASIC_INFORMATION, &oBasicInfo ) );

    if( fCustomData )
    {
        OEM_SECURE_MEMCPY( &oCustomData.customDataTypeID, &customTypeID, SIZEOF( DRM_ID ) );
        oCustomData.xbbaCustomData.pbDataBuffer          = rgbCustom;
        oCustomData.xbbaCustomData.cbData                = SIZEOF( rgbCustom );

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_CUSTOM_DATA, &oCustomData ) );
    }

    /* Complete message */
    dr = DRM_PRND_RegistrationRequestMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_RegistrationRequestMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x52454752, cbMsg );                             /* Per spec: "REGR" */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_BASIC_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_OTHERGUID_CONST3, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbCert, pbMsg, iMsg );
    VERIFY_PRND_DWORD( oBasicInfo.dwReceiverRIV, pbMsg, iMsg );                                     /* Verify what we put */
    VERIFY_PRND_DWORD( oBasicInfo.dwSupportedProximityDetectionTypes, pbMsg, iMsg );                /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_OTHERGUID_CONST4, SIZEOF( DRM_GUID ), pbMsg, iMsg ); /* Verify what we put */
    VERIFY_PRND_QWORD( oBasicInfo.qwReceiverCurrentTime, pbMsg, iMsg );                             /* Verify what we put */

    if( fCustomData )
    {
        VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_CUSTOM_DATA, pbMsg, iMsg, 0 );
        VERIFY_PRND_CUSTOM_DATA_TYPE( &customTypeID, pbMsg, iMsg );
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbCustom, pbMsg, iMsg );
    }

    /* Signature Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_REQUEST_SIGNATURE, pbMsg, iMsg, 0 );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_TYPE_ECC256_RECEIVER_SIGNING_KEY, pbMsg, iMsg );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY, pbMsg, iMsg );
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SIGNATURE_CONST, DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY, pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    wCtx = DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_ECC256_RECEIVER_SIGNING_KEY;
    ChkDR( DRM_PRND_RegistrationRequestMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg, &_SignatureCB, &wCtx ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

#define TestPRNDFormatRegistrationResponse_Arg0_FLAGS_RevInfo       0x00000001
#define TestPRNDFormatRegistrationResponse_Arg0_FLAGS_CustomData    0x00000002
#define TestPRNDFormatRegistrationResponse_Arg0_FLAGS_TxAuth        0x00000004

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDFormatRegistrationResponse_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDFormatRegistrationResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_BYTE                 rgbRevInfo[]   = { 'R', 'I', 'V' };
    DRM_BYTE                 rgbChannel[]   = { 'C', 'H', 'N', 'N' };
    DRM_BYTE                 rgbMIMK[]      = { 'M', 'I', 'M', 'K' };
    DRM_BYTE                 rgbTxCert[]    = { 'T', 'X', 'C', 'E', 'R', 'T' };
    DRM_BYTE                 rgbTxSig[]     = { 'T', 'X', 'S', 'I', 'G' };
    DRM_ID                   customTypeID   = { 0x86, 0x1a, 0xc4, 0x6d, 0x1a, 0x4f, 0x4c, 0xe1, 0x98, 0x9f, 0x5a, 0x90, 0xb9, 0x6c, 0x61, 0x64 };
    DRM_BYTE                 rgbCustom[]    = { 'C', 'D', '5' };
    DRM_DWORD                iMsg           = 0;
    DRM_WORD                 wCtx           = DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI;
    DRM_BOOL                 fRevInfo       = FALSE;
    DRM_BOOL                 fCustomData    = FALSE;
    DRM_BOOL                 fTxAuth        = FALSE;
    DRM_PRND_REGISTRATION_RESPONSE_BASIC_INFORMATION     oBasicInfo  = {0};
    DRM_PRND_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION oProxInfo   = {0};
    DRM_PRND_REGISTRATION_RESPONSE_MESSAGE               oMsg        = {0};
    DRM_PRND_REGISTRATION_RESPONSE_TRANSMITTER_AUTH      oTxAuth     = {0};
    DRM_PRND_CUSTOM_DATA                                 oCustomData = {0};

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fRevInfo     = 0 != ( dwArg0Flags & TestPRNDFormatRegistrationResponse_Arg0_FLAGS_RevInfo );
        fCustomData  = 0 != ( dwArg0Flags & TestPRNDFormatRegistrationResponse_Arg0_FLAGS_CustomData );
        fTxAuth      = 0 != ( dwArg0Flags & TestPRNDFormatRegistrationResponse_Arg0_FLAGS_TxAuth );
    }

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_RegistrationResponseMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid                               = TRUE;
    oBasicInfo.dwFlags                              = 0xEA642EC0;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    if( fRevInfo )
    {
        oBasicInfo.xbbaTransmitterRevInfo.pbDataBuffer  = rgbRevInfo;
        oBasicInfo.xbbaTransmitterRevInfo.cbData        = SIZEOF( rgbRevInfo );
    }
    oBasicInfo.wMIMKEncryptionType                                      = 0x6893;
    oBasicInfo.xbbaMIMKEncryptedWithReceiverPubkey.pbDataBuffer         = rgbMIMK;
    oBasicInfo.xbbaMIMKEncryptedWithReceiverPubkey.cbData               = SIZEOF( rgbMIMK );

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_BASIC_INFORMATION, &oBasicInfo ) );

    if( fTxAuth )
    {
        oTxAuth.xbbaTransmitterCertificate.pbDataBuffer   = rgbTxCert;
        oTxAuth.xbbaTransmitterCertificate.cbData         = SIZEOF( rgbTxCert );
        oTxAuth.wTransmitterAuthSignatureType             = 0x6C1C;
        oTxAuth.xbbaTransmitterAuthSignature.pbDataBuffer = rgbTxSig;
        oTxAuth.xbbaTransmitterAuthSignature.cbData       = SIZEOF( rgbTxSig );

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_TRANSMITTER_AUTH, &oTxAuth ) );
    }

    if( fCustomData )
    {
        OEM_SECURE_MEMCPY( &oCustomData.customDataTypeID, &customTypeID, SIZEOF( DRM_ID ) );
        oCustomData.xbbaCustomData.pbDataBuffer          = rgbCustom;
        oCustomData.xbbaCustomData.cbData                = SIZEOF( rgbCustom );

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_CUSTOM_DATA, &oCustomData ) );
    }

    /* Add Proximity Info object */
    oProxInfo.fValid                                                  = TRUE;
    oProxInfo.dwProximityDetectionType                                = 0xDFF1773C;
    oProxInfo.xbbaTransmitterProximityDetectionChannel.pbDataBuffer   = rgbChannel;
    oProxInfo.xbbaTransmitterProximityDetectionChannel.cbData         = SIZEOF( rgbChannel );
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION, &oProxInfo ) );

    /* Complete message */
    dr = DRM_PRND_RegistrationResponseMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_RegistrationResponseMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x52454753, cbMsg );                           /* Per spec: "REGS" */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_BASIC_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    if( fRevInfo )
    {
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbRevInfo, pbMsg, iMsg );
    }
    else
    {
        VERIFY_PRND_DWORD( 0, pbMsg, iMsg );
    }
    VERIFY_PRND_WORD( oBasicInfo.wMIMKEncryptionType, pbMsg, iMsg );                /* Verify what we put */
    VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgbMIMK, pbMsg, iMsg );

    /* Proximity Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_PROXIMITY_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_DWORD( oProxInfo.dwProximityDetectionType, pbMsg, iMsg );           /* Verify what we put */
    VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgbChannel, pbMsg, iMsg );

    if( fTxAuth )
    {
        VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_NONE, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_TRANSMITTER_AUTH, pbMsg, iMsg, 0 );
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbTxCert, pbMsg, iMsg );
        VERIFY_PRND_WORD( oTxAuth.wTransmitterAuthSignatureType, pbMsg, iMsg );           /* Verify what we put */
        VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgbTxSig, pbMsg, iMsg );
    }

    if( fCustomData )
    {
        VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_CUSTOM_DATA, pbMsg, iMsg, 0 );
        VERIFY_PRND_CUSTOM_DATA_TYPE( &customTypeID, pbMsg, iMsg );
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbCustom, pbMsg, iMsg );
    }

    /* Signature Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_RESPONSE_SIGNATURE, pbMsg, iMsg, 0 );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI, pbMsg, iMsg );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI, pbMsg, iMsg );
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SIGNATURE_CONST, DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI, pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    wCtx = DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI;
    ChkDR( DRM_PRND_RegistrationResponseMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg, &_SignatureCB, &wCtx ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

DRM_RESULT DRM_CALL TestPRNDFormatRegistrationError( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_DWORD                iMsg           = 0;
    DRM_PRND_REGISTRATION_ERROR_BASIC_INFORMATION oBasicInfo = {0};
    DRM_PRND_REGISTRATION_ERROR_MESSAGE           oMsg       = {0};

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_RegistrationErrorMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid   = TRUE;
    oBasicInfo.dwFlags  = 0xFB6F86BE;
    oBasicInfo.dwResult = (DRM_DWORD)DRM_E_FAIL;
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_REGISTRATION_ERROR_BASIC_INFORMATION, &oBasicInfo ) );

    /* Complete message */
    dr = DRM_PRND_RegistrationErrorMessage_Finish( &xbContext, pbMsg, &cbMsg );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_RegistrationErrorMessage_Finish( &xbContext, pbMsg, &cbMsg ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x52454745, cbMsg );                           /* Per spec: "REGE" */
    ChkBOOL( cbMsg == 0x20, DRM_E_TEST_UNEXPECTED_OUTPUT );                         /* Per spec: This message has a fixed size. */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_REGISTRATION_ERROR_BASIC_INFORMATION, pbMsg, iMsg, 0x10 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_DWORD( oBasicInfo.dwResult, pbMsg, iMsg );                          /* Verify what we put */

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRND_RegistrationErrorMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionStart( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_DWORD                iMsg           = 0;
    DRM_PRND_PROXIMITY_DETECTION_START_BASIC_INFORMATION oBasicInfo = {0};
    DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE           oMsg       = {0};

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_ProximityDetectionStartMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid   = TRUE;
    oBasicInfo.dwFlags  = 0x5014AD8B;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_START_BASIC_INFORMATION, &oBasicInfo ) );

    /* Complete message */
    dr = DRM_PRND_ProximityDetectionStartMessage_Finish( &xbContext, pbMsg, &cbMsg );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_ProximityDetectionStartMessage_Finish( &xbContext, pbMsg, &cbMsg ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x50585354, cbMsg );                           /* Per spec: "PXST" */
    ChkBOOL( cbMsg == 0x2C, DRM_E_TEST_UNEXPECTED_OUTPUT );                         /* Per spec: This message has a fixed size. */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_START_BASIC_INFORMATION, pbMsg, iMsg, 0x1C );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRND_ProximityDetectionStartMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionChallenge( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_DWORD                iMsg           = 0;
    DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION oBasicInfo = {0};
    DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE           oMsg       = {0};

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_ProximityDetectionChallengeMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid  = TRUE;
    oBasicInfo.dwFlags = 0xCFED4D1E;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    DRMCRT_memset( &oBasicInfo.guidNonce,     TEST_OTHERGUID_CONST1, SIZEOF( DRM_GUID ) );
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION, &oBasicInfo ) );

    /* Complete message */
    dr = DRM_PRND_ProximityDetectionChallengeMessage_Finish( &xbContext, pbMsg, &cbMsg );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_ProximityDetectionChallengeMessage_Finish( &xbContext, pbMsg, &cbMsg ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x50584348, cbMsg );                           /* Per spec: "PXCH" */
    ChkBOOL( cbMsg == 0x3C, DRM_E_TEST_UNEXPECTED_OUTPUT );                         /* Per spec: This message has a fixed size. */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_CHALLENGE_BASIC_INFORMATION, pbMsg, iMsg, 0x2C );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_OTHERGUID_CONST1, SIZEOF( DRM_GUID ), pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRND_ProximityDetectionChallengeMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionResponse( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_DWORD                iMsg           = 0;
    DRM_PRND_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION oBasicInfo = {0};
    DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE           oMsg       = {0};

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid                                           = TRUE;
    oBasicInfo.dwFlags                                          = 0x59C063EA;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    oBasicInfo.wNonceEncryptionType                             = 0x03FC;
    DRMCRT_memset( &oBasicInfo.guidNonceEncryptedWithMK, TEST_OTHERGUID_CONST2, SIZEOF( DRM_GUID ) );
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, &oBasicInfo ) );

    /* Complete message */
    dr = DRM_PRND_ProximityDetectionResponseMessage_Finish( &xbContext, pbMsg, &cbMsg );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Finish( &xbContext, pbMsg, &cbMsg ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x50585253, cbMsg );                           /* Per spec: "PXRS" */
    ChkBOOL( cbMsg == 0x3E, DRM_E_TEST_UNEXPECTED_OUTPUT );                         /* Per spec: This message has a fixed size. */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESPONSE_BASIC_INFORMATION, pbMsg, iMsg, 0x2E );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_WORD( oBasicInfo.wNonceEncryptionType, pbMsg, iMsg );               /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_OTHERGUID_CONST2, SIZEOF( DRM_GUID ), pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRND_ProximityDetectionResponseMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

DRM_RESULT DRM_CALL TestPRNDFormatProximityDetectionResult( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_DWORD                iMsg           = 0;
    DRM_PRND_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION oBasicInfo = {0};
    DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE           oMsg       = {0};

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_ProximityDetectionResultMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid   = TRUE;
    oBasicInfo.dwFlags  = 0x409918F3;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    oBasicInfo.dwResult = (DRM_DWORD)DRM_E_INVALIDARG;
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, &oBasicInfo ) );

    /* Complete message */
    dr = DRM_PRND_ProximityDetectionResultMessage_Finish( &xbContext, pbMsg, &cbMsg );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_ProximityDetectionResultMessage_Finish( &xbContext, pbMsg, &cbMsg ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x50585254, cbMsg );                           /* Per spec: "PXRT" */
    ChkBOOL( cbMsg == 0x30, DRM_E_TEST_UNEXPECTED_OUTPUT );                         /* Per spec: This message has a fixed size. */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_PROXIMITY_DETECTION_RESULT_BASIC_INFORMATION, pbMsg, iMsg, 0x20 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_DWORD( oBasicInfo.dwResult, pbMsg, iMsg );                          /* Verify what we put */

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRND_ProximityDetectionResultMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

#define TestPRNDFormatLicenseRequest_Arg0_FLAGS_CustomData     0x00000001
#define TestPRNDFormatLicenseRequest_Arg0_FLAGS_ContentID      0x00000002

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDFormatLicenseRequest_Arg0_FLAGS_* values above.
**               Default: 0
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDFormatLicenseRequest( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_DWORD                iMsg           = 0;
    DRM_WORD                 wCtx           = DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI;
    DRM_ID                   customTypeID   = { 0xa1, 0x31, 0xed, 0x6f, 0xd3, 0x73, 0x71, 0x4d, 0xb4, 0xa1, 0xee, 0x1e, 0xe1, 0xac, 0x2a, 0x9f };
    DRM_BYTE                 rgbCustom[]    = { 'C', 'D', '1' };
    DRM_BYTE                 rgbCID[]       = { 'K', 'I', 'D' };
    DRM_BOOL                 fCustomData    = FALSE;
    DRM_BOOL                 fContentID     = FALSE;
    DRM_PRND_LICENSE_REQUEST_BASIC_INFORMATION      oBasicInfo  = {0};
    DRM_PRND_LICENSE_REQUEST_LICENSE_INFORMATION    oLicInfo    = {0};
    DRM_PRND_LICENSE_REQUEST_MESSAGE                oMsg        = {0};
    DRM_PRND_CUSTOM_DATA                            oCustomData = {0};

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fCustomData     = 0 != ( dwArg0Flags & TestPRNDFormatLicenseRequest_Arg0_FLAGS_CustomData );
        fContentID      = 0 != ( dwArg0Flags & TestPRNDFormatLicenseRequest_Arg0_FLAGS_ContentID );
    }

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_LicenseRequestMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid                               = TRUE;
    oBasicInfo.dwFlags                              = 0x1619BB4B;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    oBasicInfo.dwReceiverRIV                        = 0x8823DC93;
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_BASIC_INFORMATION, &oBasicInfo ) );

    if( fCustomData )
    {
        OEM_SECURE_MEMCPY( &oCustomData.customDataTypeID, &customTypeID, SIZEOF( DRM_ID ) );
        oCustomData.xbbaCustomData.pbDataBuffer          = rgbCustom;
        oCustomData.xbbaCustomData.cbData                = SIZEOF( rgbCustom );

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_CUSTOM_DATA, &oCustomData ) );
    }

    /* Add License Info object */
    oLicInfo.fValid                             = TRUE;
    oLicInfo.wRequestedAction                   = 0x5A88;
    oLicInfo.wRequestedActionQualifier          = 0xF286;
    oLicInfo.wContentIdentifierType             = 0x9A89;
    if( fContentID )
    {
        oLicInfo.xbbaContentIdentifier.pbDataBuffer = rgbCID;
        oLicInfo.xbbaContentIdentifier.cbData       = SIZEOF( rgbCID );
    }
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_LICENSE_INFORMATION, &oLicInfo ) );

    /* Complete message */
    dr = DRM_PRND_LicenseRequestMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_LicenseRequestMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x4C494352, cbMsg );                           /* Per spec: "LICR" */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_BASIC_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_DWORD( oBasicInfo.dwReceiverRIV, pbMsg, iMsg );                     /* Verify what we put */

    /* License Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_LICENSE_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_WORD( oLicInfo.wRequestedAction, pbMsg, iMsg );                     /* Verify what we put */
    VERIFY_PRND_WORD( oLicInfo.wRequestedActionQualifier, pbMsg, iMsg );            /* Verify what we put */
    VERIFY_PRND_WORD( oLicInfo.wContentIdentifierType, pbMsg, iMsg );               /* Verify what we put */
    if( fContentID )
    {
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbCID, pbMsg, iMsg );
    }
    else
    {
        VERIFY_PRND_DWORD( 0, pbMsg, iMsg );
    }

    if( fCustomData )
    {
        VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_CUSTOM_DATA, pbMsg, iMsg, 0 );
        VERIFY_PRND_CUSTOM_DATA_TYPE( &customTypeID, pbMsg, iMsg );
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbCustom, pbMsg, iMsg );
    }

    /* Signature Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_REQUEST_SIGNATURE, pbMsg, iMsg, 0 );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI, pbMsg, iMsg );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI, pbMsg, iMsg );
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SIGNATURE_CONST, DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI, pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Use invalid to represent DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI when f_pxbbaMIMKEncryptedWithReceiverPubkey is not passed in */
    wCtx = DRM_PRND_SIGNATURE_OPERATION_INVALID;
    ChkDR( DRM_PRND_LicenseRequestMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg, &_SignatureCB, &wCtx ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

#define TestPRNDFormatLicenseTransmit_Arg0_FLAGS_RevInfo            0x00000001
#define TestPRNDFormatLicenseTransmit_Arg0_FLAGS_CustomData         0x00000002

/*****************************************************************************
** Arguments:
**      argv[0]: Bitmask of Flags.
**               See TestPRNDFormatLicenseTransmit_Arg0_FLAGS_* values above.
**               Default: 0
**      argv[1]: Number of licenses to transmit.  Must be 1, 2, or 3.
**               Default: 3
******************************************************************************/
DRM_RESULT DRM_CALL TestPRNDFormatLicenseTransmit( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT               dr             = DRM_SUCCESS;
    DRM_BYTE                *pbStack        = NULL;
    DRM_DWORD                cbStack        = 0;
    DRM_DWORD               *pcbStack       = NULL;
    DRM_XB_BUILDER_CONTEXT   xbContext      = {0};
    DRM_BYTE                *pbMsg          = NULL;
    DRM_DWORD                cbMsg          = 0;
    DRM_BYTE                 rgbRevInfo[]   = { 'R', 'I', 'V' };
    DRM_BYTE                 rgbLicense1[]  = { 'L', 'I', 'C', '1' };
    DRM_BYTE                 rgbLicense2[]  = { 'L', 'I', 'C', '2' };
    DRM_BYTE                 rgbLicense3[]  = { 'L', 'I', 'C', '3' };
    DRM_ID                   customTypeID   = { 0x76, 0x4d, 0x8e, 0x0f, 0x9c, 0x41, 0x48, 0x82, 0xb0, 0x70, 0xa, 0xf5, 0x29, 0x48, 0x42, 0xc2 };
    DRM_BYTE                 rgbCustom[]    = { 'C', 'U', 'S', 'T', 'O', 'M' };
    DRM_DWORD                iMsg           = 0;
    DRM_BOOL                 fRevInfo       = FALSE;
    DRM_BOOL                 fCustomData    = FALSE;
    DRM_WORD                 wCtx           = DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI;
    DRM_PRND_LICENSE_TRANSMIT_BASIC_INFORMATION     oBasicInfo  = {0};
    DRM_PRND_LICENSE_TRANSMIT_LICENSE_INFORMATION   oLicInfo[3] = {0};
    DRM_PRND_CUSTOM_DATA                            oCustomData = {0};
    DRM_PRND_LICENSE_TRANSMIT_MESSAGE               oMsg        = {0};
    DRM_DWORD                                       cLicenses      = NO_OF( oLicInfo );

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fRevInfo     = 0 != ( dwArg0Flags & TestPRNDFormatLicenseTransmit_Arg0_FLAGS_RevInfo );
        fCustomData  = 0 != ( dwArg0Flags & TestPRNDFormatLicenseTransmit_Arg0_FLAGS_CustomData );
    }

    if( HASARG(1) )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[1], cch, 10, &cLicenses ) );
    }
    ChkArg( cLicenses >= 1 ); /* Protocol requires at least one license */
    ChkArg( cLicenses <= 3 );

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_LicenseTransmitMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid                               = TRUE;
    oBasicInfo.dwFlags                              = 0x16D99151;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    DRMCRT_memset( &oBasicInfo.guidCurrentLicenseMessageID, TEST_OTHERGUID_CONST2, SIZEOF( DRM_GUID ) );
    if( fRevInfo )
    {
        oBasicInfo.xbbaTransmitterRevInfo.pbDataBuffer  = rgbRevInfo;
        oBasicInfo.xbbaTransmitterRevInfo.cbData        = SIZEOF( rgbRevInfo );
    }

    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_BASIC_INFORMATION, &oBasicInfo ) );

    if( fCustomData )
    {
        OEM_SECURE_MEMCPY( &oCustomData.customDataTypeID, &customTypeID, SIZEOF( DRM_ID ) );
        oCustomData.xbbaCustomData.pbDataBuffer  = rgbCustom;
        oCustomData.xbbaCustomData.cbData        = SIZEOF( rgbCustom );

        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_CUSTOM_DATA, &oCustomData ) );
    }

    /* Add License Info Object #1 */
    oLicInfo[0].fValid                                         = TRUE;
    oLicInfo[0].xbbaReceiverBoundOrLeafXMRLicense.pbDataBuffer = rgbLicense1;
    oLicInfo[0].xbbaReceiverBoundOrLeafXMRLicense.cbData       = SIZEOF( rgbLicense1 );
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, &oLicInfo[0] ) );

    /* Add License Info Object #2 */
    if( cLicenses > 1 )
    {
        oLicInfo[1].fValid                                         = TRUE;
        oLicInfo[1].xbbaReceiverBoundOrLeafXMRLicense.pbDataBuffer = rgbLicense2;
        oLicInfo[1].xbbaReceiverBoundOrLeafXMRLicense.cbData       = SIZEOF( rgbLicense2 );
        ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, &oLicInfo[1] ) );

        /* Add License Info Object #3 */
        if( cLicenses > 2 )
        {
            oLicInfo[2].fValid                                         = TRUE;
            oLicInfo[2].xbbaReceiverBoundOrLeafXMRLicense.pbDataBuffer = rgbLicense3;
            oLicInfo[2].xbbaReceiverBoundOrLeafXMRLicense.cbData       = SIZEOF( rgbLicense3 );
            ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, &oLicInfo[2] ) );
        }
    }

    /* Complete message */
    dr = DRM_PRND_LicenseTransmitMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_LicenseTransmitMessage_Finish( &xbContext, pbMsg, &cbMsg, &_SignatureCB, &wCtx ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x4C494354, cbMsg );                           /* Per spec: "LICT" */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_BASIC_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_OTHERGUID_CONST2, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    if( fRevInfo )
    {
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbRevInfo, pbMsg, iMsg );
    }
    else
    {
        VERIFY_PRND_DWORD( 0, pbMsg, iMsg );
    }

    /* License List Object */
    /* Note: Not explicitly added, but implicitly added when we added the first License Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND | XB_FLAGS_CONTAINER, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_LIST, pbMsg, iMsg, 0 );

    if( cLicenses > 1 )
    {
        if( cLicenses > 2 )
        {
            /* License Info Object #3 */
            VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, pbMsg, iMsg, 0 );
            VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgbLicense3, pbMsg, iMsg );
        }

        /* License Info Object #2 */
        VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, pbMsg, iMsg, 0 );
        VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgbLicense2, pbMsg, iMsg );
    }

    /* License Info Object #1 */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_LICENSE_INFORMATION, pbMsg, iMsg, 0 );
    VERIFY_PRND_FIXED_SIZE_WORD_ARRAY( rgbLicense1, pbMsg, iMsg );

    if( fCustomData )
    {
        VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_CUSTOM_DATA, pbMsg, iMsg, 0 );
        VERIFY_PRND_CUSTOM_DATA_TYPE( &customTypeID, pbMsg, iMsg );
        VERIFY_PRND_FIXED_SIZE_DWORD_ARRAY( rgbCustom, pbMsg, iMsg );
    }

    /* Signature Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_TRANSMIT_SIGNATURE, pbMsg, iMsg, 0 );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_TYPE_AES_OMAC1_MI, pbMsg, iMsg );
    VERIFY_PRND_WORD( DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI, pbMsg, iMsg );
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SIGNATURE_CONST, DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI, pbMsg, iMsg );

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Use invalid to represent DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI when f_pxbbaMIMKEncryptedWithReceiverPubkey is not passed in */
    wCtx = DRM_PRND_SIGNATURE_OPERATION_INVALID;
    ChkDR( DRM_PRND_LicenseTransmitMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg, &_SignatureCB, &wCtx ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

DRM_RESULT DRM_CALL TestPRNDFormatLicenseError( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                               dr         = DRM_SUCCESS;
    DRM_BYTE                                *pbStack    = NULL;
    DRM_DWORD                                cbStack    = 0;
    DRM_DWORD                               *pcbStack   = NULL;
    DRM_XB_BUILDER_CONTEXT                   xbContext  = {0};
    DRM_BYTE                                *pbMsg      = NULL;
    DRM_DWORD                                cbMsg      = 0;
    DRM_DWORD                                iMsg       = 0;
    DRM_PRND_LICENSE_ERROR_BASIC_INFORMATION oBasicInfo = {0};
    DRM_PRND_LICENSE_ERROR_MESSAGE           oMsg       = {0};

    MAP_TO_GLOBAL_TEST_VAR_EX( pbStack, pcbStack, TEST_VAR_PRND_STACK );
    cbStack = *pcbStack;

    ChkDR( DRM_PRND_LicenseErrorMessage_Start( pbStack, cbStack, &xbContext ) );

    /* Add Basic Info object */
    oBasicInfo.fValid   = TRUE;
    oBasicInfo.dwFlags  = 0xBFB0C520;
    DRMCRT_memset( &oBasicInfo.guidSessionID, TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ) );
    oBasicInfo.dwResult = (DRM_DWORD)DRM_E_PARAMETERS_MISMATCHED;
    ChkDR( DRM_XB_AddEntry( &xbContext, DRM_PRND_ENTRY_TYPE_LICENSE_ERROR_BASIC_INFORMATION, &oBasicInfo ) );

    /* Complete message */
    dr = DRM_PRND_LicenseErrorMessage_Finish( &xbContext, pbMsg, &cbMsg );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( cbMsg > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkMem( pbMsg = (DRM_BYTE*)Oem_MemAlloc( cbMsg ) );
    ChkDR( DRM_PRND_LicenseErrorMessage_Finish( &xbContext, pbMsg, &cbMsg ) );

    /* Verify message conforms to spec EXACTLY */
    VERIFY_PRND_HEADER( pbMsg, iMsg, 0x4C494345, cbMsg );                           /* Per spec: "LICE" */
    ChkBOOL( cbMsg == 0x30, DRM_E_TEST_UNEXPECTED_OUTPUT );                         /* Per spec: This message has a fixed size. */

    /* Basic Info Object */
    VERIFY_PRND_OBJECT_HEADER( XB_FLAGS_MUST_UNDERSTAND, DRM_PRND_ENTRY_TYPE_LICENSE_ERROR_BASIC_INFORMATION, pbMsg, iMsg, 0x20 );
    VERIFY_PRND_DWORD( oBasicInfo.dwFlags, pbMsg, iMsg );                           /* Verify what we put */
    VERIFY_PRND_ARRAY_OF_REPEATING_BYTES( TEST_SESSIONID_CONST, SIZEOF( DRM_GUID ), pbMsg, iMsg );
    VERIFY_PRND_DWORD( oBasicInfo.dwResult, pbMsg, iMsg );                          /* Verify what we put */

    /* Should be at end of message now */
    ChkBOOL( iMsg == cbMsg, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRND_LicenseErrorMessage_Parse( pbStack, cbStack, pbMsg, cbMsg, &oMsg ) );

ErrorExit:
    SAFE_OEM_FREE( pbMsg );
    return dr;
}

#define VERIFY_INC_LMID_BYTE( act, exp ) do {                                                           \
        if( (act) != (exp) )                                                                            \
        {                                                                                               \
            Log( "Trace", "Failed: 0x%x 0x%x 0x%x 0x%x", pdw128[0], pdw128[1], pdw128[2], pdw128[3] );  \
            fFailed = TRUE;                                                                             \
        }                                                                                               \
    } while(FALSE)

DRM_RESULT DRM_CALL TestPRNDFormatIncrementLMID( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_BOOL   fFailed  = FALSE;

    const DRM_DWORD rgdwTestValues[] = {
        0x00000000,

        0x00000001,
        0x01000000,

        0x00C519FF,
        0xFF19C500,

        0x48FFFFFF,
        0xFFFFFF48,

        0x77777777,

        0x8D3AFFFF,
        0xFFFF3A8D,

        0xFFFFFFFE,
        0xEFFFFFFF,

        0xFFFFFFFF
    };

    DRM_DWORD iHiHi = 0;
    DRM_DWORD iHi   = 0;
    DRM_DWORD iLo   = 0;
    DRM_DWORD iLoLo = 0;
    DRM_GUID  lmid;
    DRM_GUID  lmidPlusPlus;
    DRM_DWORD iByte = 0;
    const DRM_BYTE  rgbMax[SIZEOF(lmid)] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    for( iHiHi = 0; iHiHi < NO_OF( rgdwTestValues ); iHiHi++ )
    {
        for( iHi = 0; iHi < NO_OF( rgdwTestValues ); iHi++ )
        {
            for( iLo = 0; iLo < NO_OF( rgdwTestValues ); iLo++ )
            {
                for( iLoLo = 0; iLoLo < NO_OF( rgdwTestValues ); iLoLo++ )
                {
                    DRM_DWORD *pdw128 = (DRM_DWORD*)&lmid;
                    pdw128[0] = rgdwTestValues[iHiHi];
                    pdw128[1] = rgdwTestValues[iHi];
                    pdw128[2] = rgdwTestValues[iLo];
                    pdw128[3] = rgdwTestValues[iLoLo];

                    MEMCPY( &lmidPlusPlus, &lmid, SIZEOF(lmid) );

                    DRM_PRND_TxRx_IncrementLMID( &lmidPlusPlus );

                    /* bu prndtest!PKTEST::TestPRNDFormatIncrementLMID+0x1c5 "db @@(&lmid) L0x10;db @@(&lmidPlusPlus) L0x10;dv iByte;gc" */
                    if( MEMCMP( rgbMax, &lmid, SIZEOF(lmid) ) == 0 )
                    {
                        /* Special case the max value since it doesn't wrap to zero but wraps to 1 */
                        VERIFY_INC_LMID_BYTE( ((DRM_BYTE*)&lmidPlusPlus)[SIZEOF(lmid)-1], 1 );
                        for( iByte = 0; iByte < SIZEOF(lmid)-1; iByte++ )
                        {
                            VERIFY_INC_LMID_BYTE( ((DRM_BYTE*)&lmidPlusPlus)[iByte], 0 );
                        }
                    }
                    else
                    {
                        DRM_BOOL fAdded = FALSE;
                        for( iByte = SIZEOF(lmid); iByte > 0; iByte-- )
                        {
                            if( !fAdded )
                            {
                                if( ((DRM_BYTE*)&lmid)[iByte-1] != 0xFF )
                                {
                                    VERIFY_INC_LMID_BYTE( ((DRM_BYTE*)&lmidPlusPlus)[iByte-1], ((DRM_BYTE*)&lmid)[iByte-1] + 1 );
                                    fAdded = TRUE;
                                }
                                else
                                {
                                    VERIFY_INC_LMID_BYTE( ((DRM_BYTE*)&lmidPlusPlus)[iByte-1], 0 );
                                    /* Don't set fAdded.  We carry the add to the next byte. */
                                }
                            }
                            else
                            {
                                VERIFY_INC_LMID_BYTE( ((DRM_BYTE*)&lmidPlusPlus)[iByte-1], ((DRM_BYTE*)&lmid)[iByte-1] );
                            }
                        }
                    }
                }
            }
        }
    }

    ChkBOOL( !fFailed, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(prndformattest)
    API_ENTRY(TestPRNDFormatRegistrationRequest)
    API_ENTRY(TestPRNDFormatRegistrationResponse)
    API_ENTRY(TestPRNDFormatRegistrationError)
    API_ENTRY(TestPRNDFormatProximityDetectionStart)
    API_ENTRY(TestPRNDFormatProximityDetectionChallenge)
    API_ENTRY(TestPRNDFormatProximityDetectionResponse)
    API_ENTRY(TestPRNDFormatProximityDetectionResult)
    API_ENTRY(TestPRNDFormatLicenseRequest)
    API_ENTRY(TestPRNDFormatLicenseTransmit)
    API_ENTRY(TestPRNDFormatLicenseError)
    API_ENTRY(TestPRNDFormatIncrementLMID)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

