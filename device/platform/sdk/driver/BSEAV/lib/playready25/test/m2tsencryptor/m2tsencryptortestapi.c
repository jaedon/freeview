/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmm2tsencryptor.h>
#include <drmm2tsbitstream.h>
#include <drmm2tsparsers.h>
#include <drmm2tsbuilders.h>
#include <drmm2tsbuffermgr.h>
#include <M2TSEncryptorTestAPI.h>
#include <DRMManagerTestAPI.h>
#include <prndprotocoltest.h>
#include <drmmathsafe.h>
#include <tstkeyfile.h>
#include <drmdeviceassets.h>

ENTER_PKTEST_NAMESPACE_CODE;

#define PACKETS_BUFFER_SIZE                         188 * 20000
#define MAX_PES_DATA_SIZE                           512 * 1024

#define MPEG2_VIDEO                                 0x02
#define MPEGA_AUDIO                                 0x03
#define ADTS                                        0x0F
#define H264                                        0x1B
#define DDPLUS                                      0x81

#define DDPLUS_HEADER_SIZE                          126             /* The AC3 header is 63 * 2 = 126 bytes */

#define FIRST_H264_NAL_UNIT_TYPE                    0x01
#define LAST_H264_NAL_UNIT_TYPE                     0x0C
#define FIRST_MPEG2_SLICE_DATA_START_CODE_VALUE     0x01
#define LAST_MPEG2_SLICE_DATA_START_CODE_VALUE      0xAF

#define HEADER_BYTE                                 'H'
#define OVERFLOW_PAYLOAD_BYTE                       'O'
#define REGULAR_PAYLOAD_BYTE                        'P'

#define PLAYREADY_PROTECTION_SYSTEM_ID  "\x9A\x04\xF0\x79\x98\x40\x42\x86\xab\x92\xe6\x5b\xe0\x88\x5f\x95"
/*
** NOTE: If you are adding new ES_INFO constants, please note that DO NOT use \x00 in your string
**       because the code uses DRMCRT_strlen() to get the length.
*/
#define ES_INFO_NONE                    ""
#define ES_INFO_ONE_NON_CA              "\x06\x02\xFF\xFF"  /* tag=6 length=2 */
#define ES_INFO_TWO_NON_CA              "\x06\x02\xFF\xFF\x01\x03\xFF\xFF\xFF"  /* tag=6 length=2, tag=0x01 length 0x03 */
#define ES_INFO_CA_AND_NON_CA           "\x09\x04\xaa\x10\x11\x10\x12\x05\xFF\xFF\xFF\xFF\xFF" /* tag=0x09 CA_system=0x0010 ca_PID=0x1110, tag=0x12 length=0x05 */
#define ES_INFO_CA_AND_CA               "\x09\x04\x01\x01\x11\x11\x09\x04\x01\x01\x11\x12" /* tag=0x09 CA_system=0x0101 ca_PID=0x1111, tag=0x09 CA_system=0x0101 ca_PID=0x1112 */
#define ES_INFO_CA_AND_NON_CA_AND_CA    "\x09\x04\x01\x01\x11\x11\x16\x03\xFF\xFF\xFF\x09\x04\x01\x01\x11\x12" /* tag=0x09 CA_system=0x0101 ca_PID=0x1111, tag=0x16 length=3, tag=0x09 CA_system=0x0101 ca_PID=0x1112 */

/* PES Headers */
DRM_BYTE g_rgbDDPLUS_PES_HEADER_PTS_2648661236[]    = { 0x00, 0x00, 0x01, 0xbd, 0x0f, 0x08, 0x84, 0x80, 0x05, 0x25, 0x77, 0x7d, 0xb9, 0xe9 };
DRM_BYTE g_rgbADTS_PES_HEADER_PTS_900000[]          = { 0x00, 0x00, 0x01, 0xc0, 0x36, 0x9a, 0x84, 0x80, 0x05, 0x21, 0x00, 0x37, 0x77, 0x41 };
DRM_BYTE g_rgbMPEG2_PES_HEADER_PTS_2648725394[]     = { 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x84, 0xc0, 0x0b, 0x35, 0x77, 0x81, 0xaf, 0x25, 0x15, 0x77, 0x81, 0x68, 0xc3, 0xff };
DRM_BYTE g_rgbH264_PES_HEADER_PTS_903060[]          = { 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x84, 0xc0, 0x0a, 0x31, 0x00, 0x37, 0x8f, 0x29, 0x11, 0x00, 0x37, 0x77, 0x41 };
DRM_BYTE g_rgbMPEGA_PES_HEADER_PTS_2119045161[]     = { 0x00, 0x00, 0x01, 0xc0, 0x01, 0xa9, 0x80, 0x80, 0x05, 0x23, 0xf9, 0x39, 0x20, 0x53 };

DRM_BYTE g_rgbMPEGA_PES_HEADER_NO_START_CODE[]      = { 0xFF, 0xFF, 0xFF, 0xc0, 0x01, 0xa9, 0x80, 0x80, 0x05, 0x23, 0xf9, 0x39, 0x20, 0x53 };
DRM_BYTE g_rgbMPEGA_PES_HEADER_NO_SECLEN[]          = { 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x80, 0x80, 0x05, 0x23, 0xf9, 0x39, 0x20, 0x53 };
DRM_BYTE g_rgbMPEGA_PES_HEADER_NO_PTS[]             = { 0x00, 0x00, 0x01, 0xc0, 0x01, 0xa9, 0x80, 0x00, 0x05, 0x23, 0xf9, 0x39, 0x20, 0x53 };

/* Audio frame headers */
DRM_BYTE g_rgbDDPLUS_FRAME_HEADER_FS_768[]          = { 0x0b, 0x77, 0x54, 0x09, 0x14, 0x40 };
DRM_BYTE g_rgbADTS_FRAME_HEADER_FS_371[]            = { 0xff, 0xf1, 0x4c, 0x80, 0x2e, 0x7f, 0xfc };
DRM_BYTE g_rgbMPEGA_FRAME_HEADER_FS_417[]           = { 0xff, 0xfb, 0x90, 0x64 };

DRM_BYTE g_rgbDDPLUS_FRAME_HEADER_FS_768_INVALID[]  = { 0x0b, 0x77, 0x54, 0x09, 0xF4, 0x40 };
DRM_BYTE g_rgbADTS_FRAME_HEADER_FS_371_INVALID[]    = { 0xff, 0xf1, 0x7c, 0x80, 0x2e, 0x7f, 0xfc };
DRM_BYTE g_rgbMPEGA_FRAME_HEADER_FS_417_INVALID[]   = { 0xff, 0xfb, 0x9F, 0x64 };

DRM_BOOL g_fAppContextInited                        = FALSE;

DRM_RESULT g_drErrorToInject                        = DRM_SUCCESS;

typedef struct __tgaENCRYPTION_LOG
{
    DRM_RESULT  drResult;
    DRM_DWORD   cInstances;
} ENCRYPTION_LOG;

#define MAX_TEST_ECMS            10
#define MAX_TEST_STREAMS         5
#define MAX_TEST_SAMPLES         20
#define MAX_TEST_PES_PACKETS     20
#define MAX_TEST_SUBSAMPLE       40

typedef struct __tagSUBSAMPLE_TEST_DATA
{
    DRM_WORD            cbClear;
    DRM_DWORD           cbEncrypted;
} SUBSAMPLE_TEST_DATA;

typedef struct __tagSAMPLE_TEST_DATA
{
    DRM_WORD            cSubsamples;
    SUBSAMPLE_TEST_DATA rgoSubsampleTestData[MAX_TEST_SUBSAMPLE];
} SAMPLE_TEST_DATA;

typedef struct __tagPES_TEST_DATA
{
    DRM_UINT64          qwPTS;
    DRM_BYTE            cSamples;
    SAMPLE_TEST_DATA    rgoSampleTestData[MAX_TEST_SAMPLES];
} PES_TEST_DATA;

typedef struct __tagSTREAM_TEST_DATA
{
    DRM_KID             oKID;
    DRM_BYTE            cPES;
    PES_TEST_DATA       rgoPESTestData[MAX_TEST_PES_PACKETS];
} STREAM_TEST_DATA;

typedef struct __tagECM_TEST_DATA
{
    DRM_BOOL            fHasPRO;
    DRM_BYTE            cStreams;
    STREAM_TEST_DATA    rgoStreamTestData[MAX_TEST_STREAMS];
} ECM_TEST_DATA;

#define MAX_ENCRYPTOR_ERRORS                        200

ENCRYPTION_LOG  g_rgoLogs[ MAX_ENCRYPTOR_ERRORS ]   = { 0 };
DRM_DWORD       g_cLogs                             = 0;

#define DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pPacket )          ( ( (pPacket)->wPacketFlags & DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET ) != 0 )
#define DRM_M2TS_GET_PID( rgbPacketData )                       ( ( ( (rgbPacketData)[1] & DRM_M2TS_PID_HIGH_BYTE_MASK ) << 8 ) + (rgbPacketData)[2] )

#define DRM_M2TS_SET_AND_INC_CONTINUITY_COUNTER( rgbPacketData, bCounter )                                          \
    do {                                                                                                            \
        (rgbPacketData)[3] = (DRM_BYTE)( ((rgbPacketData)[3] & ~DRM_M2TS_CONTINUITY_COUNTER_MASK) | bCounter );     \
        bCounter = (DRM_BYTE)( ( bCounter + 1 ) & DRM_M2TS_CONTINUITY_COUNTER_MASK );                               \
    } while( FALSE )

#define DRM_M2TS_APPEND_TO_END_OF_LIST( pList, pItem )                                      \
    do {                                                                                    \
        if( (pList)->pTail == NULL )                                                        \
        {                                                                                   \
            DRMASSERT( (pList)->cItems == 0                                                 \
                    && (pList)->pHead == NULL                                               \
                    && (pList)->pTail == NULL );                                            \
            (pList)->pHead = pItem;                                                         \
            (pList)->pTail = pItem;                                                         \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            (pList)->pTail->pNext = pItem;                                                  \
            (pList)->pTail = pItem;                                                         \
        }                                                                                   \
        (pItem)->pNext = NULL;                                                              \
        (pList)->cItems++;                                                                  \
    } while( FALSE )

#define DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( pList, pItem )                             \
    do {                                                                                    \
        if( (pList)->pTail == NULL )                                                        \
        {                                                                                   \
            DRMASSERT( (pList)->cItems == 0                                                 \
                    && (pList)->pHead == NULL                                               \
                    && (pList)->pTail == NULL );                                            \
            (pList)->pHead = pItem;                                                         \
            (pList)->pTail = pItem;                                                         \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            (pList)->pTail->pNextPerUnit = pItem;                                           \
            (pList)->pTail = pItem;                                                         \
        }                                                                                   \
        (pItem)->pNextPerUnit = NULL;                                                       \
        (pList)->cItems++;                                                                  \
    } while( FALSE )

#define SAFE_M2TS_OEM_FREE( pEncContext, pv, size )                                         \
    do {                                                                                    \
        if( pv != NULL )                                                                    \
        {                                                                                   \
            DRM_RESULT dr2;                                                                 \
            dr2 = DRM_M2TS_MemFree( (pEncContext), pv, size );                              \
            DRMASSERT( dr2 == DRM_SUCCESS );                                                \
            pv = NULL;                                                                      \
        }                                                                                   \
    } while( FALSE )

#define GET_STREAM_TYPE( iArgument, bStreamType )                                           \
    do {                                                                                    \
        if( DRMCRT_strncmp( argv[iArgument], "MPEG2_VIDEO", 11 ) == 0 )                     \
        {                                                                                   \
            bStreamType = MPEG2_VIDEO;                                                      \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument], "MPEGA_AUDIO", 11 ) == 0 )                \
        {                                                                                   \
            bStreamType = MPEGA_AUDIO;                                                      \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument], "ADTS", 4 ) == 0 )                        \
        {                                                                                   \
            bStreamType = ADTS;                                                             \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument], "H264", 4 ) == 0 )                        \
        {                                                                                   \
            bStreamType = H264;                                                             \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument], "DDPLUS", 6 ) == 0 )                      \
        {                                                                                   \
            bStreamType = DDPLUS;                                                           \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            ChkArg( FALSE );                                                                \
        }                                                                                   \
    } while ( FALSE )

#define GET_ES_INFO_STRING( iArgument, pbESInfo )                                           \
    do {                                                                                    \
        if( DRMCRT_strncmp( argv[iArgument ], "ES_INFO_NONE", 12 ) == 0 )                   \
        {                                                                                   \
            pbESInfo = (const DRM_BYTE *)ES_INFO_NONE;                                      \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument ], "ES_INFO_ONE_NON_CA", 18 ) == 0 )        \
        {                                                                                   \
            pbESInfo = (const DRM_BYTE *)ES_INFO_ONE_NON_CA;                                \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument ], "ES_INFO_TWO_NON_CA", 18 ) == 0 )        \
        {                                                                                   \
            pbESInfo = (const DRM_BYTE *)ES_INFO_TWO_NON_CA;                                \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument ], "ES_INFO_CA_AND_NON_CA", 21 ) == 0 )     \
        {                                                                                   \
            pbESInfo = (const DRM_BYTE *)ES_INFO_CA_AND_NON_CA;                             \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument ], "ES_INFO_CA_AND_CA", 17 ) == 0 )         \
        {                                                                                   \
            pbESInfo = (const DRM_BYTE *)ES_INFO_CA_AND_CA;                                 \
        }                                                                                   \
        else if( DRMCRT_strncmp( argv[iArgument ], "ES_INFO_CA_AND_NON_CA_AND_CA", 28 ) == 0 ) \
        {                                                                                   \
            pbESInfo = (const DRM_BYTE *)ES_INFO_CA_AND_NON_CA_AND_CA;                      \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            ChkArg( FALSE );                                                                \
        }                                                                                   \
    } while ( FALSE )

/********************************************************************************************
** Function:    TestEncryptor_Init
**
** Synopsis:    Test Drm_M2ts_Initialize
**
** Arguments:   argv[0] : Max memory for encryptor
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_Init( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE   *phEncryptor    = NULL;
    DRM_DWORD                    cbMaxMemory    = 0;

    ChkArg( argc == 1 );
    ChkArg( HASARG( 0 ) );

    cbMaxMemory = ( DRM_DWORD )strtoul( argv[0], NULL, 0 );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    ChkDR( Drm_M2ts_Initialize( cbMaxMemory, phEncryptor ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_Uninitialize
**
** Synopsis:    Get the opaque buffers and destroy them before calling destroy function
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**              DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_Uninitialize( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE   *phEncryptor    = NULL;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    ChkDR( Drm_M2ts_Uninitialize( *phEncryptor ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_SetKeyData
**
** Synopsis:    Test Drm_m2ts_SetkeyData
**
** Arguments:   argv[0] : Key ID (B64 encoded)
**              argv[1] : PRO Type
**              argv[2] : Duration
**              argv[3] : License Type
**
** Returns:     DRM_SUCCESS
**              DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_SetKeyData( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                               dr                         = DRM_SUCCESS;
    DRM_APP_CONTEXT                         *pAppContext                = NULL;
    DRM_M2TS_ENCRYPTOR_HANDLE               *phEncryptor                = NULL;
    DRM_LICENSE_HANDLE                       hLicense                   = DRM_LICENSE_HANDLE_INVALID;
    DRM_CONST_STRING                         dstrKeyID                  = EMPTY_DRM_STRING;
    DRM_KID                                  oKID                       = { 0 };
    DRM_DWORD                                cbKID                      = SIZEOF( oKID );
    DRM_WORD                                 iArgument                  = 0;
    DRM_DWORD                                dwDuration                 = 0;
    DRM_M2TS_PRO_TYPE                        ePROType                   = eDRM_M2TS_PRO_TYPE_NO_PRO;
    DRM_LOCAL_LICENSE_TYPE                   eLicenseType               = eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR      oLicensePolicyDecriptor    = { 0 };
    DRM_DWORD                                cbRemoteCert               = 0;
    DRM_BYTE                                *pbRemoteCert               = NULL;
    DRM_GUID                                 oEmptyGUID                 = EMPTY_DRM_GUID;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );

    for( iArgument = 0; iArgument < 4; iArgument++ )
    {
        ChkArg( HASARG( iArgument ) );
    }

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );
    ChkDR( Oem_Clock_SetResetState( NULL, FALSE ) );
    ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->fClockSet = TRUE;

    ChkDR( MakeDRMString( &dstrKeyID, argv[ 0 ] ) );
    ChkDR( DRM_B64_DecodeW( &dstrKeyID, &cbKID, oKID.rgb, 0 ) );

    if( DRMCRT_strncmp( argv[1], "NO_PRO", 6 ) == 0 )
    {
        ePROType = eDRM_M2TS_PRO_TYPE_NO_PRO;
    }
    else if( DRMCRT_strncmp( argv[1], "KEY_ROTATION_WITH_LICENSE_IN_PRO", 32 ) == 0 )
    {
        ePROType = eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_LICENSE_IN_PRO;
    }
    else if( DRMCRT_strncmp( argv[1], "KEY_ROTATION_WITH_WRMHEADER_IN_PRO", 35 ) == 0 )
    {
        ePROType = eDRM_M2TS_PRO_TYPE_KEY_ROTATION_WITH_WRMHEADER_IN_PRO;
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

    dwDuration = (DRM_DWORD)strtoul( argv[2], NULL, 0 );

    if( DRMCRT_strncmp( argv[3], "LOCAL_BOUND_SIMPLE", 18 ) == 0 )
    {
        eLicenseType = eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE;
    }
    else if( DRMCRT_strncmp( argv[3], "LOCAL_BOUND_ROOT", 16 ) == 0 )
    {
        eLicenseType = eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT;
    }
    else if( DRMCRT_strncmp( argv[3], "LEAF", 4 ) == 0 )
    {
        eLicenseType = eDRM_LOCAL_LICENSE_LEAF;
    }
    else if( DRMCRT_strncmp( argv[3], "REMOTE_BOUND_SIMPLE", 19 ) == 0 )
    {
        eLicenseType = eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE;
    }
    else if( DRMCRT_strncmp( argv[3], "REMOTE_BOUND_ROOT", 17 ) == 0 )
    {
        eLicenseType = eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT;
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

    if(eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE ||
       eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT)
    {
        ChkDR( TST_KF_GetCertificate(
            ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
            eKF_CERT_TYPE_PLAYREADY,
            &pbRemoteCert,
            &cbRemoteCert ) );
    }

    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );
    oLicensePolicyDecriptor.wSecurityLevel   = 150;
    oLicensePolicyDecriptor.fCannotPersist   = TRUE;

    ChkDR( Drm_LocalLicense_CreateLicense(
        pAppContext,
        &oLicensePolicyDecriptor,
        eLicenseType,
        &oKID,
        cbRemoteCert,
        pbRemoteCert,
        DRM_LICENSE_HANDLE_INVALID,
        &hLicense ) );

    if( g_drErrorToInject == DRM_E_INVALIDARG )
    {
        /* Verify NULL inputs return invalid arg */
        ChkBOOL( DRM_E_INVALIDARG == Drm_M2ts_SetKeyData( NULL, hLicense, (DRM_GUID*)oKID.rgb, ePROType, dwDuration ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( DRM_E_INVALIDARG == Drm_M2ts_SetKeyData( *phEncryptor, NULL, (DRM_GUID*)oKID.rgb, ePROType, dwDuration ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( DRM_E_INVALIDARG == Drm_M2ts_SetKeyData( *phEncryptor, hLicense, (DRM_GUID*)NULL, ePROType, dwDuration ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkBOOL( DRM_E_INVALIDARG == Drm_M2ts_SetKeyData( *phEncryptor, hLicense, &oEmptyGUID, ePROType, dwDuration ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkDR( Drm_M2ts_SetKeyData( *phEncryptor, hLicense, (DRM_GUID*)oKID.rgb, ePROType, dwDuration ) );
    }

    ChkDR( Drm_LocalLicense_Release( &hLicense ) );

    hLicense = DRM_LICENSE_HANDLE_INVALID;

ErrorExit:
    FREEDRMSTRING( dstrKeyID );
    if( hLicense != DRM_LICENSE_HANDLE_INVALID )
    {
        DRM_RESULT dr2 = DRM_SUCCESS;
        dr2 = Drm_LocalLicense_Release( &hLicense );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


/********************************************************************************************
** Function:    TestEncryptor_SetEncryptAudio
**
** Synopsis:    Test Drm_M2ts_SetEncryptionProperty
**
** Arguments:   argv[0] : TRUE/FALSE to encrypt audio stream(s)
**
** Returns:     DRM_SUCCESS
**              DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_SetEncryptAudio( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE       *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTION_PROPERTY     oProperty;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );

    ChkArg( HASARG( 0 ) );

    MEMSET( &oProperty, 0, SIZEOF( oProperty ) );
    oProperty.ePropertyType = eDRM_M2TS_PROPERTY_TYPE_ENCYRPT_AUDIO;
    oProperty.Value.fValue = DRMCRT_strncmp( argv[0], "TRUE", 4 ) == 0;

    ChkDR( Drm_M2ts_SetEncryptionProperty( *phEncryptor, &oProperty ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_Encrypt
**
** Synopsis:    Encrypt TS pakcets stored in pbPacketsBuffer and save the encrypted packets
**              in pbEncryptedBuffer
**
** Arguments:   argv[0] : Key ID (B64 encoded)
**              argv[1] : TRUE/FALSE flag - embed license in PRO
**              argv[2] : Duration**
**
** Returns:     DRM_SUCCESS
**              DRM_S_MORE_DATA
**              DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
**              DRM_E_M2TS_NEED_KEY_DATA
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_Encrypt( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                   dr                         = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE   *phEncryptor                = NULL;
    DRM_BYTE                    *pbPacketsBuffer            = NULL;
    DRM_DWORD                   *pcbPacketsBuffer           = NULL;
    DRM_DWORD                   *pdwPacketsBufferOffset     = NULL;
    DRM_BYTE                    *pbEncryptedBuffer          = NULL;
    DRM_DWORD                   *pcbEncryptedBuffer         = NULL;
    DRM_DWORD                   *pdwEncryptedBufferOffset   = NULL;
    DRM_DWORD                    cbEncrypted                = 0;
    DRM_BOOL                     fFirstCall                 = TRUE;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedBufferOffset, TEST_VAR_M2TS_ENCRYPTED_BUFFER_OFFSET );

    cbEncrypted = *pcbEncryptedBuffer;

    while( TRUE )
    {
        /* Pass entire buffer in at once. Any further iterations just retrieve remaining data in response to DRM_S_MORE_DATA. */
        dr = Drm_M2ts_Encrypt( *phEncryptor,
                               fFirstCall ? pbPacketsBuffer : NULL,
                               fFirstCall ? *pdwPacketsBufferOffset : 0,
                               pbEncryptedBuffer + *pdwEncryptedBufferOffset,
                               &cbEncrypted,
                               TRUE );

        *pdwEncryptedBufferOffset += cbEncrypted;

        if( dr == DRM_E_M2TS_NEED_KEY_DATA && HASARG(0) )
        {
            ChkDR( TestEncryptor_SetKeyData( argc, argv ) );
        }
        else if( dr != DRM_S_MORE_DATA )
        {
            break;
        }

        /* Calculate remaining space in output buffer */
        cbEncrypted = *pcbEncryptedBuffer - *pdwEncryptedBufferOffset;
        fFirstCall = FALSE;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_EncryptWithMoreData
**
** Synopsis:    Encrypt TS pakcets stored in pbPacketsBuffer and save the encrypted packets
**              in pbEncryptedBuffer using a smaller buffer
**
** Arguments:   argv[0] : The size of buffer to pass to the Encrypt function each time.
**                        0 or not specified means to pass the entire buffer.
**
** Returns:     DRM_SUCCESS
**              DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
**              DRM_E_M2TS_NEED_KEY_DATA
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_EncryptWithMoreData( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                   dr                             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE   *phEncryptor                    = NULL;
    DRM_BYTE                    *pbPacketsBuffer                = NULL;
    DRM_DWORD                   *pcbPacketsBuffer               = NULL;
    DRM_DWORD                   *pdwPacketsBufferOffset         = NULL;
    DRM_BYTE                    *pbEncryptedBuffer              = NULL;
    DRM_DWORD                   *pcbEncryptedBuffer             = NULL;
    DRM_DWORD                   *pdwEncryptedBufferOffset       = NULL;
    DRM_DWORD                    cbEncrypted                    = 0;
    DRM_DWORD                    dwOffset                       = 0;
    DRM_DWORD                    dwDataInOffset                 = 0;
    DRM_DWORD                    cbDataIn                       = 0;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedBufferOffset, TEST_VAR_M2TS_ENCRYPTED_BUFFER_OFFSET );

    if( HASARG( 0 ) )
    {
        cbDataIn = (DRM_DWORD)strtoul( argv[0], NULL, 0 );
    }
    cbDataIn = cbDataIn == 0 ? *pdwPacketsBufferOffset : cbDataIn;

    while( dwDataInOffset < *pdwPacketsBufferOffset )
    {
        DRM_BOOL fFirstCall = TRUE;
        DRM_BOOL fLastBatch = ( dwDataInOffset + cbDataIn ) >= *pdwPacketsBufferOffset;

        do
        {
            /* Specify small output buffer size so we encrypt one packet for each call to encrypt */
            cbEncrypted = min( *pcbEncryptedBuffer - dwOffset, DRM_M2TS_PACKET_SIZE );

            /* Pass in the whole input buffer on the first iteration. Encrypt will cache it. */
            dr = Drm_M2ts_Encrypt( *phEncryptor,
                                    fFirstCall ? &pbPacketsBuffer[ dwDataInOffset ] : NULL,
                                    fFirstCall ? cbDataIn : 0,
                                    pbEncryptedBuffer + dwOffset,
                                    &cbEncrypted,
                                    fLastBatch );

            /* Apply new key data as needed */
            if( dr == DRM_E_M2TS_NEED_KEY_DATA && HASARG(0) )
            {
                ChkDR( TestEncryptor_SetKeyData( argc, argv ) );
                dr = DRM_E_M2TS_NEED_KEY_DATA;
            }
            else
            {
                ChkDR(dr);
            }

            /* 
            ** If we encrypted the first packet and there is still more data available (such that the encryptor returned DRM_S_MORE_DATA),
            ** test that attempting to pass in more data right away will fail with DRM_E_M2TS_GET_ENCRYPTED_DATA_FIRST. This should 
            ** fail before caching the dummy packet so as not to otherwise affect the output of the scenario. 
            */
            if( g_drErrorToInject == DRM_E_M2TS_GET_ENCRYPTED_DATA_FIRST && dr == DRM_S_MORE_DATA && dwOffset == 0 )
            {
                DRM_BYTE rgbDummy[DRM_M2TS_PACKET_SIZE] = { 0 };
                DRM_DWORD cbDummy = SIZEOF( rgbDummy );

                ChkBOOL( DRM_E_M2TS_GET_ENCRYPTED_DATA_FIRST == Drm_M2ts_Encrypt( *phEncryptor,
                                                                                  rgbDummy,
                                                                                  SIZEOF( rgbDummy ),
                                                                                  pbEncryptedBuffer + dwOffset,
                                                                                  &cbDummy,
                                                                                  TRUE ), DRM_E_TEST_UNEXPECTED_OUTPUT );
            }

            dwOffset += cbEncrypted;
            fFirstCall = FALSE;

        } while( dr == DRM_S_MORE_DATA || dr == DRM_E_M2TS_NEED_KEY_DATA);

        ChkDR( dr );

        dwDataInOffset += cbDataIn;
        cbDataIn = min( cbDataIn, *pdwPacketsBufferOffset - dwDataInOffset );
    }

    /* At this point we've encrypted everything in the input. Verify that calling Encrypt with no input data returns DRM_SUCCESS. */
    {
        DRM_BYTE rgbDummy[DRM_M2TS_PACKET_SIZE] = { 0 };
        DRM_DWORD cbDummy = SIZEOF( rgbDummy );

        ChkDR( Drm_M2ts_Encrypt( *phEncryptor,
                                  NULL,
                                  0,
                                  rgbDummy,
                                  &cbDummy,
                                  TRUE ) );
    }

    *pdwEncryptedBufferOffset = dwOffset;

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_BinaryReader
**
** Synopsis:    Test the read bits, skip bits, skip bytes, wirte bits, insert bytes internal APIs
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**              DRM_E_BUFFER_BOUNDS_EXCEEDED
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_BinaryReader( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                          dr                  = DRM_SUCCESS;
    DRM_BYTE                            bValue              = 0;
    DRM_WORD                            wValue              = 0;
    DRM_DWORD                           dwValue             = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT         oBSRContext         = { 0 };
    DRM_BYTE                            rgTestData1[]       = { 0x01, 0xFF, 0x12, 0x34, 0x56, 0x78, 0x00, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44 };
    DRM_BYTE                            rgTestData2[]       = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xF0 };
    DRM_BYTE                            rgTestData2After[]  = { 0x12, 0xc2, 0xab, 0x78, 0x01, 0x02, 0x9a, 0xbc };

    /* Init with rgTestData1 data */
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData1 ), rgTestData1, 0 ) );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == SIZEOF( rgTestData1 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test read bits, to read the first 6 bytes */
    bValue = 1;
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 0, &bValue ) );
    ChkBOOL( bValue == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 4, &bValue ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &wValue ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 32, &dwValue ) );
    ChkBOOL( bValue == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( wValue == 0x1ff, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( dwValue == 0x12345678, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == SIZEOF( rgTestData1 ) - 6, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test not allow to use smaller buffer */
    ChkBOOL( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 9, &bValue ) == DRM_E_INVALIDARG, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 17, &wValue ) == DRM_E_INVALIDARG, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 33, &dwValue ) == DRM_E_INVALIDARG, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test skip bits and bytes, to skip and read the rest of the bytes */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 0 ) );
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 0 ) );
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 1 ) );
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 5 ) );
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 8 ) );
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 2 ) );
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 13 ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 13, &dwValue ) );
    ChkBOOL( dwValue == 0x1344, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test out of bound */
    ChkBOOL( DRM_M2TS_BSR_SkipBits( &oBSRContext, 1 ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( oBSRContext.fOutOfRange == TRUE, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 1 ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 32, &dwValue ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContext, 8, bValue ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContext, 16, wValue ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_InsertBytes( &oBSRContext, 2, (const DRM_BYTE *)"\x01\x02" ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test out of bound with byte offset points to the last byte */
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData1 ), rgTestData1, SIZEOF( rgTestData1 ) - 1 ) );
    ChkBOOL( DRM_M2TS_BSR_SkipBits( &oBSRContext, 9 ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData1 ), rgTestData1, SIZEOF( rgTestData1 ) - 1 ) );
    ChkBOOL( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &wValue ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData1 ), rgTestData1, SIZEOF( rgTestData1 ) - 1 ) );
    ChkBOOL( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContext, 16, wValue ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData1 ), rgTestData1, SIZEOF( rgTestData1 ) - 1 ) );
    ChkBOOL( DRM_M2TS_BSR_InsertBytes( &oBSRContext, 2, (const DRM_BYTE *)"\x01\x02" ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Init with rgTestData2 data with initial offset at 1 */
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData2 ), rgTestData2, 1 ) );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == SIZEOF( rgTestData2 ) - 1, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test write bits */
    ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContext, 0, (DRM_BYTE)1 ) );
    ChkDR( DRM_M2TS_BSR_WriteBitsFromBYTE( &oBSRContext, 3, (DRM_BYTE)0x06 ) );
    ChkBOOL( rgTestData2[1] == 0xd4, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContext, 13, (DRM_WORD)0x2ab ) );
    ChkBOOL( rgTestData2[1] == 0xc2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( rgTestData2[2] == 0xab, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test insert bytes */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 3 ) );
    ChkBOOL( DRM_M2TS_BSR_InsertBytes( &oBSRContext, 2, (const DRM_BYTE *)"\x01\x02" ) == DRM_E_INVALIDARG, DRM_E_TEST_UNEXPECTED_OUTPUT ); /* not allow to insert bytes while the bit offset is not 0 */
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 5 ) );
    ChkDR( DRM_M2TS_BSR_InsertBytes( &oBSRContext, 2, (const DRM_BYTE *)"\x01\x02" ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &wValue ) );
    ChkBOOL( wValue == 0x9abc, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( MEMCMP( rgTestData2, rgTestData2After, SIZEOF( rgTestData2 ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test set counter */
    ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, SIZEOF( rgTestData1 ), rgTestData1, 1 ) );
    ChkDR( DRM_M2TS_BSR_SetCounter( &oBSRContext, 2 ) );
    ChkBOOL( oBSRContext.fUseCounter == TRUE, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 12, &wValue ) );
    ChkBOOL( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 5, &wValue ) == DRM_E_BUFFER_BOUNDS_EXCEEDED, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( oBSRContext.fOutOfRange == TRUE, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_BinaryReaderWithPacketList
**
** Synopsis:    Test the read bits, skip bits, skip bytes, wirte bits, insert bytes
**              using packet list as input
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**              DRM_E_BUFFER_BOUNDS_EXCEEDED
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_BinaryReaderWithPacketList( long argc, __in_ecount(argc) char **argv )
{
    #define                     cPackets              5
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_DWORD                   cbTotalPayload      = 0;
    DRM_BYTE                    bValue              = 1;
    DRM_DWORD                   dwValue             = 0;
    DRM_WORD                    iPacket             = 0;
    DRM_M2TS_PACKET             rgPackets[cPackets] = { 0 };
    DRM_M2TS_BIT_STREAM_CONTEXT oBSRContext         = { 0 };

    rgPackets[0].cbPayload = 2;
    rgPackets[1].cbPayload = 1;
    rgPackets[2].cbPayload = 3;
    rgPackets[3].cbPayload = 3;
    rgPackets[4].cbPayload = 10;

    /* Set the per unit chain */
    for( iPacket = 0; iPacket < cPackets - 1; iPacket++ )
    {
        rgPackets[ iPacket ].pNextPerUnit = &rgPackets[ iPacket + 1 ];
    }

    for( iPacket = 0; iPacket < cPackets; iPacket++ )
    {
        cbTotalPayload += rgPackets[ iPacket ].cbPayload;
    }

    /* Set the payload */
    rgPackets[0].rgbData[ DRM_M2TS_PACKET_SIZE - 2 ] = 0x12;    /* byte 1 */
    rgPackets[0].rgbData[ DRM_M2TS_PACKET_SIZE - 1 ] = 0x34;    /* byte 2 */
    rgPackets[1].rgbData[ DRM_M2TS_PACKET_SIZE - 1 ] = 0x56;    /* byte 3 */
    rgPackets[2].rgbData[ DRM_M2TS_PACKET_SIZE - 3 ] = 0x78;    /* byte 4 */
    rgPackets[2].rgbData[ DRM_M2TS_PACKET_SIZE - 2 ] = 0x9a;    /* byte 5 */
    rgPackets[2].rgbData[ DRM_M2TS_PACKET_SIZE - 1 ] = 0xbc;    /* byte 6 */
    rgPackets[3].rgbData[ DRM_M2TS_PACKET_SIZE - 3 ] = 0xde;    /* byte 7 */
    rgPackets[3].rgbData[ DRM_M2TS_PACKET_SIZE - 2 ] = 0xf0;    /* byte 8 */
    rgPackets[3].rgbData[ DRM_M2TS_PACKET_SIZE - 1 ] = 0xf1;    /* byte 9 */
    MEMSET( rgPackets[4].rgbData, 0xFF, sizeof(rgPackets[4].rgbData) );

    /* Init with packet list */
    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, &rgPackets[0] ) );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == cbTotalPayload, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test read and skip bits */
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 3, &bValue ) );
    ChkBOOL( bValue == 0x00, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_SkipBits( &oBSRContext, 1 ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 17, &dwValue ) );
    ChkBOOL( dwValue == 0x468a, DRM_E_TEST_UNEXPECTED_OUTPUT ); /* Expected value = 0010 0011 0100 0101 0 --> 0 0100 0110 1000 1010 */
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 2 ) );
    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 7, &bValue ) ); /* read the next 7 bits, 3 bits from byte 5 and 4 bits byte 6 */
    ChkBOOL( bValue == 0x2b, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test write bits */
    ChkDR( DRM_M2TS_BSR_WriteBitsFromWORD( &oBSRContext, 12, (DRM_WORD)0x0f35 ) ); /* update the last 4 bits of byte 7 and entire byte 7 */
    ChkBOOL( rgPackets[2].rgbData[ DRM_M2TS_PACKET_SIZE - 1 ] == 0xbf, DRM_E_TEST_UNEXPECTED_OUTPUT );
    rgPackets[3].rgbData[ DRM_M2TS_PACKET_SIZE - 3 ] = 0x35;

    /* Test insert bytes */
    ChkDR( DRM_M2TS_BSR_InsertBytes( &oBSRContext, 3, (const DRM_BYTE *)"\x01\x02\x03" ) ); /* insert 3 bytes starting from byte 8 */
    ChkBOOL( rgPackets[3].rgbData[ DRM_M2TS_PACKET_SIZE - 2 ] == 0x01, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( rgPackets[3].rgbData[ DRM_M2TS_PACKET_SIZE - 1 ] == 0x02, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( rgPackets[4].rgbData[ DRM_M2TS_PACKET_SIZE - rgPackets[4].cbPayload ] == 0x03, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 32, &dwValue ) );
    ChkBOOL( dwValue == 0xF0F1FFFF, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == (DRM_DWORD)(rgPackets[4].cbPayload - 5), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Init and test counter */
    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, &rgPackets[0] ) );
    ChkDR( DRM_M2TS_BSR_SetCounter( &oBSRContext, 3 ) );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == 3, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, 1 ) );
    ChkBOOL( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) == 2, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_ParsePacketHeader
**
** Synopsis:    Test DRM_M2TS_ParsePacketHeader by feeding it the packets stored in
**              TEST_VAR_M2TS_PACKETS_BUFFER
**
** Arguments:   argv[0] : PID
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_ParsePacketHeader( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr                     = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_CONTEXT          *pEncContext            = NULL;
    DRM_M2TS_ENCRYPTOR_HANDLE           *phEncryptor            = NULL;
    DRM_BYTE                            *pbPacketsBuffer        = NULL;
    DRM_DWORD                           *pcbPacketsBuffer       = NULL;
    DRM_DWORD                           *pdwPacketsBufferOffset = NULL;
    DRM_WORD                             wPID                   = DRM_M2TS_UNDEFINED_PID;
    DRM_BYTE                             bContinuityCounter     = 0;
    DRM_BOOL                             fUnitStart             = FALSE;
    DRM_M2TS_STREAM                     *pStream                = NULL;
    DRM_M2TS_PACKET                     *pPacket                = NULL;
    const DRM_BYTE                      *pbPacketData           = NULL;

    ChkArg( HASARG( 0 ) );
    wPID = (DRM_WORD)strtoul( argv[0], NULL, 0 );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );

    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    ChkDR( DRM_M2TS_GetStream( pEncContext, wPID, &pStream ) );

    pbPacketData = pbPacketsBuffer;
    while( pbPacketData < pbPacketsBuffer + *pdwPacketsBufferOffset )
    {
        ChkDR( DRM_M2TS_GetFreePacket(  pEncContext,
                                        0,
                                        eDRM_M2TS_PACKET_STATE_PENDING,
                                        TRUE,
                                        pbPacketData,
                                        &pPacket ) );

        ChkDR( DRM_M2TS_ParseTSPacketHeader( pPacket,
                                             &wPID,
                                             &fUnitStart,
                                             &bContinuityCounter ) );
        ChkArg( wPID == pStream->wPID );

        ChkDR( DRM_M2TS_ParsePacketHeader( pEncContext,
                                           pStream,
                                           pPacket ) );

        DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &pStream->oPacketList, pPacket );
        DRM_M2TS_APPEND_TO_END_OF_LIST( &pEncContext->oIncomingPacketList, pPacket );

        pPacket = NULL;

        pbPacketData += DRM_M2TS_PACKET_SIZE;
    }

ErrorExit:
    /* Set the buffer offset to 0 in order to run next test */
    if( pdwPacketsBufferOffset != NULL )
    {
        *pdwPacketsBufferOffset = 0;
    }

    SAFE_M2TS_OEM_FREE( pEncContext, pPacket, SIZEOF( DRM_M2TS_PACKET ) );

    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_RebuildPMTPacket
**
** Synopsis:    Test DRM_M2TS_RebuildPMTPacket
**
** Arguments:   argv[0] : wPID
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_RebuildPMTPacket( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr             = DRM_SUCCESS;
    DRM_M2TS_STREAM                     *pStream        = NULL;
    DRM_WORD                             wPID           = DRM_M2TS_UNDEFINED_PID;
    DRM_M2TS_ENCRYPTOR_HANDLE           *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT          *pEncContext    = NULL;

    ChkArg( HASARG( 0 ) );
    wPID = (DRM_WORD)strtoul( argv[0], NULL, 0 );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    ChkDR( DRM_M2TS_GetStream( pEncContext, wPID, &pStream ) );
    ChkDR( DRM_M2TS_RebuildPMTPacket( pEncContext, pStream ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_GetFreePacket
**
** Synopsis:    Test DRM_M2TS_GetFreePacket
**
** Arguments:   argv[0] : Payload length
**              argv[1] : Packet state
**              argv[2] : Packet data
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_GetFreePacket( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE           *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT          *pEncContext    = NULL;
    DRM_M2TS_PACKET                     *pPacket        = NULL;
    DRM_BYTE                             rgbPacketData[ DRM_M2TS_PACKET_SIZE ] = { 0 };
    DRM_BYTE                             cbPayload      = 0;
    DRM_M2TS_PACKET_STATE                ePacketState   = eDRM_M2TS_PACKET_STATE_PENDING;
    const DRM_BYTE                      *pbPacketData   = NULL;
    DRM_DWORD                            dwArgv2Len     = 0;

    ChkArg( argc == 3 );
    ChkArg( HASARG( 0 ) );
    ChkArg( HASARG( 1 ) );
    ChkArg( HASARG( 2 ) );

    cbPayload = (DRM_BYTE)strtoul( argv[0], NULL, 0 );
    if( 0 == DRMCRT_strncmp( argv[1], "eDRM_M2TS_PACKET_STATE_PENDING", 30 ) )
    {
        ePacketState = eDRM_M2TS_PACKET_STATE_PENDING;
    }
    else if( 0 == DRMCRT_strncmp( argv[1], "eDRM_M2TS_PACKET_STATE_BAD", 26 ) )
    {
        ePacketState = eDRM_M2TS_PACKET_STATE_BAD;
    }
    else if( 0 == DRMCRT_strncmp( argv[1], "eDRM_M2TS_PACKET_STATE_READY", 28 ) )
    {
        ePacketState = eDRM_M2TS_PACKET_STATE_READY;
    }
    else
    {
        ChkArg( FALSE );
    }

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[2] ), &dwArgv2Len ) );
    pbPacketData = (const DRM_BYTE *)argv[2];
    MEMCPY( rgbPacketData, pbPacketData, min( SIZEOF( rgbPacketData ), dwArgv2Len ) );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    ChkDR( DRM_M2TS_GetFreePacket( pEncContext,
                                   cbPayload,
                                   ePacketState,
                                   FALSE,
                                   rgbPacketData,
                                   &pPacket ) );

    ChkBOOL( pPacket->cbPayload == cbPayload, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pPacket->ePacketState == ePacketState, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( MEMCMP( pPacket->rgbData, pbPacketData, min( DRM_M2TS_PACKET_SIZE, dwArgv2Len ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pPacket->cbHeader == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pPacket->cbPayloadLastPES == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( !DRM_M2TS_IS_LAST_OF_LAST_PES_PACKET( pPacket ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pPacket->pECMData == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pPacket->pNext == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pPacket->pNextPerUnit == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_M2TS_OEM_FREE( pEncContext, pPacket, SIZEOF( DRM_M2TS_PACKET ) );
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_TSPacketHeaderParser
**
** Synopsis:    Test DRM_M2TS_ParseTSPacketHeader
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_TSPacketHeaderParser( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT      dr                              = DRM_SUCCESS;
    DRM_WORD        wPID                            = DRM_M2TS_UNDEFINED_PID;
    DRM_BOOL        fUnitStart                      = FALSE;
    DRM_BYTE        bContinuityCounter              = 0;
    DRM_BYTE        rgbHeader1[]                    = { 0x47, 0x40, 0x00, 0x10 };       /* wPID = 0, Unit Start, payload = 184 */
    DRM_BYTE        rgbHeader2[]                    = { 0x47, 0x41, 0x00, 0x10 };       /* wPID = 256, Unit Start, payload = 184 */
    DRM_BYTE        rgbHeader3[]                    = { 0x47, 0x17, 0x8b, 0x12 };       /* wPID = 6027, Not Unit Start, payload = 184 */
    DRM_BYTE        rgbHeader4[]                    = { 0x47, 0x41, 0x01, 0x30, 0x9e }; /* wPID = 257, Unit Start, adaptation length = 158, payload = 25 */
    DRM_BYTE        rgbHeader5[]                    = { 0x47, 0x41, 0x01, 0x30, 0xfe }; /* Expect to fail becasue the adaptation field is too long */
    DRM_BYTE        rgbHeader6[]                    = { 0x12, 0x40, 0x00, 0x10 };       /* Expect to fail becasue the sync_byte is not 0x47 */
    DRM_M2TS_PACKET oPacket                         = { 0 };

    MEMCPY( oPacket.rgbData, rgbHeader1, SIZEOF(rgbHeader1) );
    ChkDR( DRM_M2TS_ParseTSPacketHeader( &oPacket, &wPID, &fUnitStart, &bContinuityCounter ) );
    ChkBOOL( wPID == 0 && fUnitStart && oPacket.cbPayload == DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( oPacket.rgbData, rgbHeader2, SIZEOF(rgbHeader2) );
    ChkDR( DRM_M2TS_ParseTSPacketHeader( &oPacket, &wPID, &fUnitStart, &bContinuityCounter ) );
    ChkBOOL( wPID == 256 && fUnitStart && oPacket.cbPayload == DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( oPacket.rgbData, rgbHeader3, SIZEOF(rgbHeader3) );
    ChkDR( DRM_M2TS_ParseTSPacketHeader( &oPacket, &wPID, &fUnitStart, &bContinuityCounter ) );
    ChkBOOL( wPID == 6027 && !fUnitStart && oPacket.cbPayload == DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( oPacket.rgbData, rgbHeader4, SIZEOF(rgbHeader4) );
    ChkDR( DRM_M2TS_ParseTSPacketHeader( &oPacket, &wPID, &fUnitStart, &bContinuityCounter ) );
    ChkBOOL( wPID == 257 && fUnitStart && oPacket.cbPayload == 25, DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( oPacket.rgbData, rgbHeader5, SIZEOF(rgbHeader5) );
    ChkBOOL( DRM_M2TS_ParseTSPacketHeader( &oPacket, &wPID, &fUnitStart, &bContinuityCounter ) == DRM_E_M2TS_ADAPTATION_LENGTH_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );

    MEMCPY( oPacket.rgbData, rgbHeader6, SIZEOF(rgbHeader6) );
    ChkBOOL( DRM_M2TS_ParseTSPacketHeader( &oPacket, &wPID, &fUnitStart, &bContinuityCounter ) == DRM_E_M2TS_PACKET_SYNC_BYTE_INVALID, DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_InsertRandomBytes
**
** Synopsis:    Insert a random byte string to the stream to make the sync byte out of sync
**
** Arguments:   argv[0] : char string (the random bytes)
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_InsertRandomBytes( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT   dr                     = DRM_SUCCESS;
    DRM_BYTE    *pbPacketsBuffer        = NULL;
    DRM_DWORD   *pdwPacketsBufferOffset = NULL;
    DRM_DWORD    cbRandomString         = 0;

    ChkArg( HASARG( 0 ) );

    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );

    cbRandomString = (DRM_WORD)DRMCRT_strlen( (const DRM_CHAR *)argv[0] );

    MEMCPY( &pbPacketsBuffer[ *pdwPacketsBufferOffset ], argv[0], cbRandomString );

    *pdwPacketsBufferOffset += cbRandomString;

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    _BuildPackets
**
** Synopsis:    Build TS packet from payload and store in TEST_VAR_M2TS_PACKETS_BUFFER
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
static DRM_RESULT DRM_CALL _BuildPackets(
    __in const DRM_WORD     f_wPID,
    __in const DRM_BYTE    *f_pbPayload,
    __in const DRM_DWORD    f_cbPayload,
    __in const DRM_BYTE     f_nApaptationFiledLength,
    __in       DRM_BYTE    *f_pnNextCC,
    __in const DRM_BOOL     f_fInterruptedCC )
{
    DRM_RESULT   dr                     = DRM_SUCCESS;
    DRM_BYTE    *pbPacketsBuffer        = NULL;
    DRM_DWORD   *pcbPacketsBuffer       = NULL;
    DRM_DWORD   *pdwPacketsBufferOffset = NULL;
    DRM_BYTE    *pbPacketData           = NULL;
    DRM_DWORD    cbRemaining            = f_cbPayload;
    DRM_BYTE     bPakcetOffset          = 0;
    DRM_BOOL     fUnitStart             = TRUE;

    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );

    while( cbRemaining > 0 )
    {
        ChkArg( *pdwPacketsBufferOffset < *pcbPacketsBuffer );

        pbPacketData = pbPacketsBuffer + *pdwPacketsBufferOffset;
        MEMSET( pbPacketData, 0, DRM_M2TS_PACKET_SIZE );

        ChkDR( DRM_M2TS_SetTSPacketHeader(  fUnitStart,
                                            f_wPID,
                                            FALSE,
                                            FALSE,
                                            *f_pnNextCC,
                                            DRM_M2TS_MAX_TS_PAYLOAD_SIZE,
                                            pbPacketData ) );

        if( g_drErrorToInject == DRM_E_M2TS_PACKET_SYNC_BYTE_INVALID )
        {
            pbPacketData[0] = 0xFF; /* anything other than 0x47 */
        }

        /* f_fInterruptedCC skips a number */
        *f_pnNextCC += f_fInterruptedCC ? 2 : 1;
        *f_pnNextCC &= 0x0F;

        bPakcetOffset = 4; /* to point to the first byte after the payload or adaptation area */

        /* Add adaptation field */
        if( fUnitStart )
        {
            /* Adaptation field in the unit start packet */
            if(  f_nApaptationFiledLength > 0 )
            {
                pbPacketData[ bPakcetOffset++ ] = g_drErrorToInject != DRM_E_M2TS_ADAPTATION_LENGTH_INVALID ? (DRM_BYTE)f_nApaptationFiledLength : (DRM_BYTE)( DRM_M2TS_MAX_ADAPTATION_FIELD_LENGTH + 1 );
                pbPacketData[ bPakcetOffset++ ] = 0;
                MEMSET( pbPacketData + bPakcetOffset, DRM_M2TS_STUFFING_BYTE, (DRM_SIZE_T)( f_nApaptationFiledLength - 1 ) );
                bPakcetOffset += f_nApaptationFiledLength - 1;

                /* Set adaptation field flag */
                pbPacketData[3] |= 0x20;
            }

            if( g_drErrorToInject == DRM_E_M2TS_NOT_UNIT_START_PACKET )
            {
                /* reset unit start flag */
                pbPacketData[1] &= ~DRM_M2TS_UNIT_START_MASK;
            }
        }
        else if( cbRemaining < (DRM_BYTE)(DRM_M2TS_PACKET_SIZE - bPakcetOffset) )
        {
            /* Adaptation field in the last packet (or last two packets) */
            DRM_BYTE cbApaptationField = 0;
            ChkDR( DRM_DWordToByte( max( 1, DRM_M2TS_PACKET_SIZE - bPakcetOffset - cbRemaining - 1 ), &cbApaptationField ) );
            pbPacketData[ bPakcetOffset++ ] = cbApaptationField;
            pbPacketData[ bPakcetOffset++ ] = 0;
            MEMSET( pbPacketData + bPakcetOffset, DRM_M2TS_STUFFING_BYTE, (DRM_SIZE_T)( cbApaptationField - 1 ) );
            bPakcetOffset += cbApaptationField - 1;

            /* Set adaptation field flag */
            pbPacketData[3] |= 0x20;
        }

        /* copy the payload */
        if( bPakcetOffset < DRM_M2TS_PACKET_SIZE )
        {
            DRM_BYTE cbToCopy = 0;
            ChkDR( DRM_DWordToByte( (DRM_BYTE)min( (DRM_BYTE)(DRM_M2TS_PACKET_SIZE - bPakcetOffset), cbRemaining ), &cbToCopy ) );

            MEMCPY( pbPacketData + bPakcetOffset, f_pbPayload + f_cbPayload - cbRemaining, cbToCopy );
            cbRemaining -= cbToCopy;

            /* Set payload flag */
            pbPacketData[3] |= 0x10;
        }

        *pdwPacketsBufferOffset += DRM_M2TS_PACKET_SIZE;
        fUnitStart = FALSE;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_BuildPAT
**
** Synopsis:    Build PAT packets
**
** Arguments:   argv[0] : Has missing packet by making the CC interruptedly
**              argv[1] : Adaptation field length
**              argv[2] : Program Number
**              argv[3] : PID of PMT
**              Note: Repeat Program Number and PID of PMT as needed
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_BuildPAT( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                   dr                                     = DRM_SUCCESS;
    DRM_BYTE                    *pnNextPATCC                            = NULL;
    DRM_BYTE                     cbApaptationField                      = 0;
    DRM_BOOL                     fInterruptedCC                         = FALSE;
    DRM_WORD                     iArgument                              = 0;
    DRM_BYTE                     rgbPayload[ DRM_M2TS_PACKET_SIZE * 2 ] = { 0 };
    DRM_WORD                     wPayloadOffset                         = 0;
    DRM_WORD                     wSectionLength                         = 0;
    DRM_WORD                     wPorgramNumber                         = 0;
    DRM_WORD                     wPID                                   = DRM_M2TS_UNDEFINED_PID;
    DRM_M2TS_BIT_STREAM_CONTEXT  oBSRContext                            = { 0 };

    ChkArg( argc >= 4 );
    ChkArg( HASARG( 0 ) && HASARG( 1 ) && HASARG( 2 ) && HASARG( 3 ) );

    MAP_TO_GLOBAL_TEST_VAR( pnNextPATCC, TEST_VAR_M2TS_PAT_CC );

    /* Set the payload to zeros, so we have the table_ID = 0 and section_length = 0 */
    MEMSET( rgbPayload, 0, SIZEOF( rgbPayload ) );

    if( g_drErrorToInject == DRM_E_M2TS_TABLE_ID_INVALID )
    {
        /* The valid table ID for PAT is 0 on the 6th byte of the packet. */
        rgbPayload[1] = 0x01; /* anything other than 0x00 */
    }

    /* Set current_next_inidcator to 1 */
    rgbPayload[6] |= 0x01;

    /* Set wPayloadOffset to 9 to point to the first program_number and PID loop */
    wPayloadOffset = 9;
    iArgument = 2;
    while( HASARG( iArgument ) && HASARG( iArgument + 1 ) )
    {
        wPorgramNumber = (DRM_WORD)strtoul( argv[iArgument], NULL, 0 );
        wPID = (DRM_WORD)strtoul( argv[iArgument + 1], NULL, 0 );

        WORD_TO_NETWORKBYTES( rgbPayload, wPayloadOffset, wPorgramNumber );
        WORD_TO_NETWORKBYTES( rgbPayload, wPayloadOffset + SIZEOF( DRM_WORD ), wPID );

        wPayloadOffset += SIZEOF( DRM_WORD ) + SIZEOF( DRM_WORD );
        iArgument += 2;
    }

    /* Add four bytes for the CRC */
    wPayloadOffset += DRM_M2TS_CRC_SIZE;

    /* update section length */
    wSectionLength = (DRM_WORD)( wPayloadOffset - 4 );
    if( g_drErrorToInject == DRM_E_M2TS_PAT_HEADER_INVALID )
    {
        /* Add 1 to make it incorrect */
        WORD_TO_NETWORKBYTES( rgbPayload, 2, (DRM_WORD)( wSectionLength + 1 ) );
    }
    else if( g_drErrorToInject == DRM_E_M2TS_SECTION_LENGTH_INVALID )
    {
        WORD_TO_NETWORKBYTES( rgbPayload, 2, DRM_M2TS_MINIMUN_PAT_SECTION_LENGTH - 1 );
    }
    else
    {
        WORD_TO_NETWORKBYTES( rgbPayload, 2, wSectionLength );
    }

    /* Update the CRC */
    if( g_drErrorToInject != DRM_E_M2TS_CRC_FIELD_INVALID )
    {
        ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, wPayloadOffset, rgbPayload, 1 ) ); /* Set offset to 1 to skip pointer field */
        ChkDR( DRM_M2TS_BSR_UpdateCRC( &oBSRContext, (DRM_WORD)( wPayloadOffset - DRM_M2TS_CRC_SIZE - 1 ) ) ); /* -1 for the pointer field */
    }

    fInterruptedCC = DRMCRT_strncmp( argv[0], "TRUE", 4 ) == 0;
    cbApaptationField = (DRM_BYTE)strtoul( argv[1], NULL, 0 );

    ChkDR( _BuildPackets( 0, rgbPayload, wPayloadOffset, cbApaptationField, pnNextPATCC, fInterruptedCC ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_VerifyPAT
**
** Synopsis:    Varify the results of DRM_M2TS_ParsePacketHeader
**
** Arguments:   argv[0] : Program Number
**              argv[1] : PID of PMT
**              Note: Repeat Program Number and PID of PMT as needed
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_VerifyPAT( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr             = DRM_SUCCESS;
    DRM_WORD                             iArgument      = 0;
    DRM_WORD                             wProgramNumber = 0;
    DRM_WORD                             wPID_PMT       = DRM_M2TS_UNDEFINED_PID;
    DRM_M2TS_STREAM                     *pStream        = NULL;
    const DRM_M2TS_PACKET               *pPacket        = NULL;
    DRM_M2TS_ENCRYPTOR_HANDLE           *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT          *pEncContext    = NULL;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = ( DRM_M2TS_ENCRYPTOR_CONTEXT * )*phEncryptor;

    /* We should have the PAT stream */
    ChkDR( DRM_M2TS_GetStream( pEncContext, 0, &pStream ) );
    ChkBOOL( pStream->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Every packet should be in ready state */
    pPacket = pStream->oPacketList.pHead;
    while( pPacket != NULL )
    {
        ChkBOOL( pPacket->ePacketState == eDRM_M2TS_PACKET_STATE_READY, DRM_E_TEST_UNEXPECTED_OUTPUT );
        pPacket = pPacket->pNextPerUnit;
    }

    while( HASARG( iArgument ) && HASARG( iArgument + 1 ) )
    {
        wProgramNumber = (DRM_WORD)strtoul( argv[iArgument], NULL, 0 );
        wPID_PMT = (DRM_WORD)strtoul( argv[iArgument + 1], NULL, 0 );

        if( wProgramNumber == 0 )
        {
            /* this is network program, the program_map_PID shouldn't be on the stream list */
            ChkBOOL( DRM_M2TS_GetStream( pEncContext, wPID_PMT, &pStream ) == DRM_E_NOT_FOUND, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        else
        {
            /* this is not network program, the program_map_PID should be on the stream list */
            ChkDR( DRM_M2TS_GetStream( pEncContext, wPID_PMT, &pStream ) );
        }

        iArgument += 2;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    _BuildPMT
**
** Synopsis:    Build PMT pakcets
**
** Arguments:   f_fCompletedPMT : Indicates whether to build a completed PMT packet(s) or not
**              argv[0] : Has missing packet by making the CC interruptedly
**              argv[1] : PID of PMT packet
**              argv[2] : Adaptation field length
**              argv[3] : program_info_length
**              argv[4] : Stream type
**              argv[5] : elementary PID for audio/video PES
**              argv[6] : ES_info
**              Note: Repeat Stream type, elementary PID and ES_Info per stream
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
static DRM_RESULT DRM_CALL _BuildPMT(
    __in          const DRM_BOOL    f_fCompletedPMT,
    __in                long        argc,
    __in_ecount(argc)   char      **argv )
{
    DRM_RESULT                   dr                                         = DRM_SUCCESS;
    DRM_BYTE                    *pnNextPMTCC                                = NULL;
    DRM_BYTE                     rgbPayload[ DRM_M2TS_PACKET_SIZE * 10 ]    = { 0 };   /* The payload size for up to 10 packets */
    DRM_BOOL                     fInterruptedCC                             = FALSE;
    DRM_WORD                     wProgramInfoLength                         = 0;
    DRM_BYTE                     cbApaptationField                          = 0;
    DRM_WORD                     iArgument                                  = 0;
    DRM_WORD                     wPayloadOffset                             = 0;
    DRM_BYTE                     bStreamType                                = DRM_M2TS_STREAM_TYPE_RESERVED;
    DRM_WORD                     wPID_PMT                                   = DRM_M2TS_UNDEFINED_PID;
    DRM_WORD                     wPID                                       = DRM_M2TS_UNDEFINED_PID;
    DRM_WORD                     wSectionLength                             = 0;
    const DRM_BYTE              *pbESInfo                                   = NULL;
    DRM_WORD                     wESInfoLength                              = 0;
    DRM_M2TS_BIT_STREAM_CONTEXT  oBSRContext                                = { 0 };

    ChkArg( argc >= 7 );
    ChkArg( HASARG( 0 ) && HASARG( 1 ) && HASARG( 2 ) && HASARG( 3 ) )

    fInterruptedCC = DRMCRT_strncmp( argv[0], "TRUE", 4 ) == 0;
    wPID_PMT = (DRM_WORD)strtoul( argv[1], NULL, 0 );
    cbApaptationField = (DRM_BYTE)strtoul( argv[2], NULL, 0 );
    wProgramInfoLength = (DRM_WORD)strtoul( argv[3], NULL, 0 );

    MAP_TO_GLOBAL_TEST_VAR( pnNextPMTCC, TEST_VAR_M2TS_PMT_CC );

    /* Set the payload to zeros */
    MEMSET( rgbPayload, 0, SIZEOF( rgbPayload ) );

    /* Set PMT table ID */
    rgbPayload[1] = g_drErrorToInject != DRM_E_M2TS_TABLE_ID_INVALID ? (DRM_BYTE)0x02 : (DRM_BYTE)0xFF;

    /* Set current_next_inidcator to 1 */
    rgbPayload[6] |= 0x01;

    iArgument = 4;
    wPayloadOffset = 11;

    /* set the program info length */
    WORD_TO_NETWORKBYTES( rgbPayload, wPayloadOffset, g_drErrorToInject != DRM_E_M2TS_PROGRAM_INFO_LENGTH_INVALID ? wProgramInfoLength : MAX_UNSIGNED_TYPE( DRM_WORD ) );
    wPayloadOffset += SIZEOF( DRM_WORD ) + wProgramInfoLength;

    /* Set stream type, PID, and ES_info for each tream */
    while( HASARG( iArgument ) && HASARG( iArgument + 1 ) && HASARG( iArgument + 2 ) )
    {
        wPID = (DRM_WORD)strtoul( argv[iArgument + 1], NULL, 0 );

        GET_STREAM_TYPE( iArgument, bStreamType );
        GET_ES_INFO_STRING( (iArgument + 2), pbESInfo );

        rgbPayload[ wPayloadOffset++ ] = bStreamType;
        WORD_TO_NETWORKBYTES( rgbPayload, wPayloadOffset, wPID );
        wPayloadOffset += SIZEOF( DRM_WORD );

        wESInfoLength = (DRM_WORD)DRMCRT_strlen( (const DRM_CHAR *)pbESInfo );
        WORD_TO_NETWORKBYTES( rgbPayload, wPayloadOffset, wESInfoLength );
        wPayloadOffset += SIZEOF( DRM_WORD );

        MEMCPY( rgbPayload + wPayloadOffset, pbESInfo, wESInfoLength );
        wPayloadOffset += wESInfoLength;
        DRMASSERT( wPayloadOffset <= SIZEOF( rgbPayload ) );

        iArgument += 3;
    }

    /* Add four bytes for the CRC */
    wPayloadOffset += DRM_M2TS_CRC_SIZE;

    /* update section length */
    wSectionLength = (DRM_WORD)( wPayloadOffset - 4 );
    if( g_drErrorToInject == DRM_E_M2TS_PMT_HEADER_INVALID )
    {
        WORD_TO_NETWORKBYTES( rgbPayload, 2, (DRM_WORD)( wSectionLength + 1 ) );
    }
    else if( g_drErrorToInject == DRM_E_M2TS_SECTION_LENGTH_INVALID )
    {
        WORD_TO_NETWORKBYTES( rgbPayload, 2, DRM_M2TS_MINIMUN_PMT_SECTION_LENGTH - 1 );
    }
    else
    {
        WORD_TO_NETWORKBYTES( rgbPayload, 2, wSectionLength );
    }

    if( g_drErrorToInject != DRM_E_M2TS_CRC_FIELD_INVALID )
    {
        ChkDR( DRM_M2TS_BSR_Init( &oBSRContext, wPayloadOffset, rgbPayload, 1 ) ); /* set offset to 1 to skip pointer field */
        ChkDR( DRM_M2TS_BSR_UpdateCRC( &oBSRContext, (DRM_WORD)( wPayloadOffset - DRM_M2TS_CRC_SIZE - 1 ) ) ); /* -1 for the pointer field */
    }

    if (!f_fCompletedPMT)
    {
        /*
        ** Add extra bytes (e.g. 300 bytes which need anohter TS packet to be completed) to section legnth field
        ** to make the encryptor to wait for it and eventually drop it because the extra TS packet is not in the stream.
        */
        WORD_TO_NETWORKBYTES( rgbPayload, 2, f_fCompletedPMT ? wSectionLength : (DRM_WORD)(wSectionLength + 300) );
    }

    ChkDR( _BuildPackets( wPID_PMT, rgbPayload, wPayloadOffset, cbApaptationField, pnNextPMTCC, fInterruptedCC ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL TestEncryptor_BuildPMT( long argc, __in_ecount(argc) char **argv )
{
    return _BuildPMT(TRUE, argc, argv);
}

DRM_RESULT DRM_CALL TestEncryptor_BuildIncompletedPMT( long argc, __in_ecount(argc) char **argv )
{
    return _BuildPMT(FALSE, argc, argv);
}

/********************************************************************************************
** Function:    _HasRemovablePID
**
** Synopsis:    Check whether the PID is on the list of removable PIDs
**
********************************************************************************************/
static DRM_BOOL DRM_CALL _HasRemovablePID(
    __in const DRM_M2TS_ENCRYPTOR_CONTEXT   *pEncContext,
    __in const DRM_WORD                      f_wPID )
{
    const DRM_M2TS_REMOVABLE_PID  *pRemovablePID  = pEncContext->oRemovablePIDList.pHead;

    while( pRemovablePID != NULL )
    {
        if( f_wPID == pRemovablePID->wPID )
        {
            return TRUE;
        }
        pRemovablePID = pRemovablePID->pNext;
    }
    return FALSE;
}

/********************************************************************************************
** Function:    TestEncryptor_VerifyPMT
**
** Synopsis:    Verify PMT
**
** Arguments:   argv[0] : PID of PMT stream
**              argv[1] : Stream type
**              argv[2] : elementary PID for audio/video PES
**              argv[3] : ES_info
**              Note: Repeat Stream type, elementary PID and ES_Info per stream
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_VerifyPMT( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_WORD                         iArgument      = 0;
    DRM_BYTE                         bStreamType    = DRM_M2TS_STREAM_TYPE_RESERVED;
    DRM_WORD                         wPID_PES       = DRM_M2TS_UNDEFINED_PID;
    DRM_WORD                         wPID_PMT       = DRM_M2TS_UNDEFINED_PID;
    DRM_M2TS_STREAM                 *pStream        = NULL;
    const DRM_M2TS_PACKET           *pPacket        = NULL;
    const DRM_BYTE                  *pbESInfo       = NULL;
    DRM_M2TS_ENCRYPTOR_HANDLE       *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT      *pEncContext    = NULL;

    ChkArg( HASARG( 0 ) );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    wPID_PMT = (DRM_WORD)strtoul( argv[0], NULL, 0 );

    /* We should have the PAM stream */
    ChkDR( DRM_M2TS_GetStream( pEncContext, wPID_PMT, &pStream ) );
    ChkBOOL( pStream->ePESState == eDRM_M2TS_PES_STATE_HAS_FULL_PAYLOAD, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Every packet should still be in pending state */
    pPacket = pStream->oPacketList.pHead;
    while( pPacket != NULL )
    {
        ChkBOOL( pPacket->ePacketState == eDRM_M2TS_PACKET_STATE_PENDING, DRM_E_TEST_UNEXPECTED_OUTPUT );
        pPacket = pPacket->pNextPerUnit;
    }

    iArgument = 1;

    while( HASARG( iArgument ) && HASARG( iArgument + 1 ) && HASARG( iArgument + 2 ) )
    {
        /* the audio/video stream should be available */
        GET_STREAM_TYPE( iArgument, bStreamType );
        wPID_PES = (DRM_WORD)strtoul( argv[iArgument + 1], NULL, 0 );
        ChkDR( DRM_M2TS_GetStream( pEncContext, wPID_PES, &pStream ) );

        /* Check the other CA_system's PID(s) is on the removable list */
        GET_ES_INFO_STRING( (iArgument + 2), pbESInfo );
        if( pbESInfo == (const DRM_BYTE *)ES_INFO_CA_AND_NON_CA )
        {
            /* 0x0010 (ca_PID) contains in ES_INFO_CA_AND_NON_CA string */
            ChkBOOL( _HasRemovablePID( pEncContext, 0x1110 ), DRM_E_TEST_UNEXPECTED_OUTPUT);
        }
        else if( pbESInfo == (const DRM_BYTE *)ES_INFO_CA_AND_CA
              || pbESInfo == (const DRM_BYTE *)ES_INFO_CA_AND_NON_CA_AND_CA )
        {
            /* 0x1111 and 0x1112 (ca_PID) contain in ES_INFO_CA_AND_CA and ES_INFO_CA_AND_NON_CA_AND_CA strings */
            ChkBOOL( _HasRemovablePID( pEncContext, 0x1111 ), DRM_E_TEST_UNEXPECTED_OUTPUT );
            ChkBOOL( _HasRemovablePID( pEncContext, 0x1112 ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        }

        iArgument += 3;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_VerifyPMTAfterRebuilt
**
** Synopsis:    Verify PMT
**
** Arguments:   argv[0] : PID of PMT stream
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_VerifyPMTAfterRebuilt( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr             = DRM_SUCCESS;
    DRM_WORD                             wPID_PMT       = DRM_M2TS_UNDEFINED_PID;
    DRM_M2TS_STREAM                     *pStream        = NULL;
    DRM_M2TS_PACKET                     *pPacket        = NULL;
    DRM_M2TS_PACKET                     *pPacketTemp    = NULL;
    DRM_M2TS_ENCRYPTOR_HANDLE           *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT          *pEncContext    = NULL;

    ChkArg( HASARG( 0 ) );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    wPID_PMT = (DRM_WORD)strtoul( argv[0], NULL, 0 );

    ChkDR( DRM_M2TS_GetStream( pEncContext, wPID_PMT, &pStream ) );

    /* Every packet should still be in ready state */
    pPacket = pStream->oPacketList.pHead;
    while( pPacket != NULL )
    {
        ChkBOOL( pPacket->ePacketState == eDRM_M2TS_PACKET_STATE_READY, DRM_E_TEST_UNEXPECTED_OUTPUT );
        pPacket = pPacket->pNextPerUnit;
    }

    /*
    ** Try to parse it again to ensure the PMT is in order
    ** But we need to hack it first
    ** 1) make the pNext same as pNextPerUnit becasue DRM_M2TS_ParsePacketHeader uses pNextPerUnit chain
    ** 2) clear pStream's packet list
    ** 3) feed the original packets to the parser again
    */
    pPacket = pStream->oPacketList.pHead;
    while( pPacket != NULL )
    {
        pPacketTemp = pPacket;
        pPacket->pNext = pPacket->pNextPerUnit;
        pPacket = pPacket->pNextPerUnit;
        pPacketTemp->pNextPerUnit = NULL;
    }

    pStream->ePESState = eDRM_M2TS_PES_STATE_BEGIN;
    pPacket = pStream->oPacketList.pHead;

    pStream->oPacketList.cItems = 0;
    pStream->oPacketList.pHead = NULL;
    pStream->oPacketList.pTail = NULL;

    while( pPacket != NULL )
    {
        ChkDR( DRM_M2TS_ParsePacketHeader( pEncContext,
                                            pStream,
                                            pPacket ) );
        DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &pStream->oPacketList, pPacket );
        pPacket = pPacket->pNext;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_CheckECMPID
**
** Synopsis:    Verify ECM's PID
**
** Arguments:   argv[0] : Expected ECM PID
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_CheckECMPID( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                        dr                         = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE        *phEncryptor                = NULL;
    const DRM_M2TS_ENCRYPTOR_CONTEXT *pEncContext                = NULL;

    ChkArg( argc == 1 );
    ChkArg( HASARG( 0 ) );

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (const DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    ChkBOOL( pEncContext->wPID_CA == (DRM_WORD)strtoul( argv[ 0 ], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_ParseECM
**
** Synopsis:    Parse ECM packets from encryptor output and perform some initial verification
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**              DRM_E_BUFFERTOOSMALL
**              DRM_E_TEST_UNEXPECTED_OUTPUT
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_ParseECM( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                       dr                         = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE       *phEncryptor                = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT      *pEncContext                = NULL;
    DRM_M2TS_BIT_STREAM_CONTEXT      oBSRContext                = { 0 };
    DRM_BYTE                        *pbEncryptedBuffer          = NULL;
    DRM_DWORD                       *pdwEncryptedBufferOffset   = NULL;
    DRM_DWORD                        dwPacketOffset             = 0;
    DRM_M2TS_STREAM                 *pStream                    = NULL;
    DRM_M2TS_PACKET                 *pPacket                    = NULL;
    const DRM_BYTE                  *pbRawData                  = NULL;
    DRM_WORD                         wPID                       = 0;
    DRM_BOOL                         fUnitStart                 = FALSE;
    DRM_BYTE                         bContinuityCounter         = 0;
    DRM_DWORD                        dwStartCode                = 0;
    DRM_BYTE                         bStreamId                  = 0;
    DRM_WORD                         cbPacketLength             = 0;
    DRM_BYTE                         bECMVersion                = 0;
    DRM_BYTE                         cPSSH                      = 0;
    DRM_BYTE                         iPSSH                      = 0;
    DRM_BYTE                         iStream                    = 0;
    DRM_DWORD                        iByte                      = 0;
    DRM_DWORD                       *pcECMTestData              = 0;
    ECM_TEST_DATA                   *prgoECMTestData            = NULL;
    ECM_TEST_DATA                   *pECMTestData               = NULL;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    MAP_TO_GLOBAL_TEST_VAR( prgoECMTestData, TEST_VAR_M2TS_ECM_TEST_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pcECMTestData, TEST_VAR_M2TS_ECM_COUNT );

    *pcECMTestData = 0;
    MEMSET( prgoECMTestData, 0, SIZEOF( ECM_TEST_DATA ) * MAX_TEST_ECMS );

    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedBufferOffset, TEST_VAR_M2TS_ENCRYPTED_BUFFER_OFFSET );

    /* Create ECM stream */
    ChkDR( DRM_M2TS_CreateStream( pEncContext, pEncContext->wPID_CA, eDRM_M2TS_PACKET_TYPE_ECM, &pStream ) );

    /* Build ECM stream  */
    while( dwPacketOffset < *pdwEncryptedBufferOffset )
    {
        pbRawData = pbEncryptedBuffer + dwPacketOffset;
        wPID = (DRM_WORD)DRM_M2TS_GET_PID( pbRawData );
        if ( wPID == pEncContext->wPID_CA )
        {
            ChkDR( DRM_M2TS_GetFreePacket( pEncContext,
                                           0,
                                           eDRM_M2TS_PACKET_STATE_PENDING,
                                           FALSE,   /* assign packet no. */
                                           pbRawData,
                                           &pPacket ) );
            ChkDR( DRM_M2TS_ParseTSPacketHeader( pPacket, &wPID, &fUnitStart, &bContinuityCounter ) );
            DRM_M2TS_APPEND_TO_END_OF_LIST( &pEncContext->oIncomingPacketList, pPacket );
            DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &pStream->oPacketList, pPacket );
        }

        dwPacketOffset += DRM_M2TS_PACKET_SIZE;
    }

    /* Parse the ECM packet */
    ChkDR( DRM_M2TS_BSR_Init_UsePacket( &oBSRContext, pStream->oPacketList.pHead ) );
    while( DRM_M2TS_BSR_GetRemainingBytes( &oBSRContext ) > 0 )
    {
        ChkBOOL( *pcECMTestData < MAX_TEST_ECMS, DRM_E_BUFFERTOOSMALL );
        pECMTestData = &prgoECMTestData[ *pcECMTestData ];

        /* Check Start code */
        ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 24, &dwStartCode ) );
        ChkBOOL( dwStartCode == 1, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Check stream ID */
        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &bStreamId ) );
        ChkBOOL( bStreamId == 0xF0, DRM_E_TEST_UNEXPECTED_OUTPUT ); /* 0xF0 is the ECM's stream ID */

        /* Get ECM PES length */
        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &cbPacketLength ) );

        /* Check ECM version */
        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 4, &bECMVersion ) );
        ChkBOOL( bECMVersion == 0x1, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Parse and verify PSSH */
        pECMTestData->fHasPRO = FALSE;
        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 4, &cPSSH ) );
        for( iPSSH = 0; iPSSH < cPSSH; iPSSH++ )
        {
            DRM_DWORD cbPRO                 = 0;
            DRM_ID    oProtectSystemID      = { 0 };

            for( iByte = 0; iByte < SIZEOF( oProtectSystemID.rgb ); iByte++ )
            {
                ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &oProtectSystemID.rgb[iByte] ) );
            }

            /* Make sure the protected system ID is PlayReady */
            ChkBOOL( 0 == MEMCMP( oProtectSystemID.rgb, PLAYREADY_PROTECTION_SYSTEM_ID, SIZEOF( oProtectSystemID.rgb ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

            pECMTestData->fHasPRO = TRUE;

            /* Skip the PRO */
            ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 32, &cbPRO ) );
            ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, cbPRO ) );
        }

        /* Parse and verify SENC */
        ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &pECMTestData->cStreams ) );
        ChkBOOL( pECMTestData->cStreams <= MAX_TEST_STREAMS, DRM_E_BUFFERTOOSMALL );

        for( iStream = 0; iStream < pECMTestData->cStreams; iStream++ )
        {
            DRM_BYTE            bScramblingControl      = 0;
            DRM_BYTE            bStreamParamsIncluded   = 0;
            DRM_WORD            wStreamPID              = 0;
            DRM_BYTE            cbIV                    = 8;
            DRM_BYTE            iPES                    = 0;
            STREAM_TEST_DATA   *pStreamTestData         = NULL;

            pStreamTestData = &pECMTestData->rgoStreamTestData[ iStream ];

            /* Scrambling control should be 01 */
            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 2, &bScramblingControl ) );
            ChkBOOL( bScramblingControl == 1, DRM_E_TEST_UNEXPECTED_OUTPUT );

            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 1, &bStreamParamsIncluded ) );
            ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 13, &wStreamPID ) );

            if( bStreamParamsIncluded == 1 )
            {
                DRM_DWORD dwAlgorithmId = 0;

                /* Check Algorithm ID */
                ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 24, &dwAlgorithmId ) );
                ChkBOOL( dwAlgorithmId == 1, DRM_E_TEST_UNEXPECTED_OUTPUT ); /* AlgorithmID 1 is AESCTR-128 */

                /* Check IV size */
                ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &cbIV ) );
                ChkBOOL( cbIV == 8, DRM_E_TEST_UNEXPECTED_OUTPUT ); /* Support 8-byte IV only */

                for( iByte = 0; iByte < SIZEOF( pStreamTestData->oKID.rgb ); iByte++ )
                {
                    ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &pStreamTestData->oKID.rgb[iByte] ) );
                }
            }

            /* Parse and check PES */

            /* Check PES count */
            ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 8, &pStreamTestData->cPES ) );
            ChkBOOL( pStreamTestData->cPES <= MAX_TEST_PES_PACKETS, DRM_E_BUFFERTOOSMALL );

            for( iPES = 0; iPES < pStreamTestData->cPES; iPES++ )
            {
                DRM_DWORD            dwPTSHigh          = 0;
                DRM_DWORD            dwPTSLow           = 0;
                DRM_BYTE             iSample            = 0;
                PES_TEST_DATA       *pPESTestData       = NULL;

                pPESTestData = &pStreamTestData->rgoPESTestData[iPES];

                /* Extract PTS (check in VerifyECM) */
                ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 1, &dwPTSHigh ) );
                ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 32, &dwPTSLow ) );
                pPESTestData->qwPTS = DRM_UI64HL( dwPTSHigh, dwPTSLow );

                /* Check sample count */
                ChkDR( DRM_M2TS_BSR_ReadBitsToBYTE( &oBSRContext, 7, &pPESTestData->cSamples ) );
                ChkBOOL( pPESTestData->cSamples <= MAX_TEST_SAMPLES, DRM_E_BUFFERTOOSMALL );

                for( iSample = 0; iSample < pPESTestData->cSamples; iSample++ )
                {
                    DRM_WORD            iSubsample      = 0;
                    SAMPLE_TEST_DATA   *pSampleTestData = NULL;

                    pSampleTestData = &pPESTestData->rgoSampleTestData[ iSample ];

                    /* Skip IV */
                    ChkDR( DRM_M2TS_BSR_SkipBytes( &oBSRContext, cbIV ) );

                    /* Check subsample count */
                    ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &pSampleTestData->cSubsamples ) );
                    ChkBOOL( pSampleTestData->cSubsamples <= MAX_TEST_SUBSAMPLE, DRM_E_BUFFERTOOSMALL );

                    for( iSubsample = 0; iSubsample < pSampleTestData->cSubsamples; iSubsample++ )
                    {
                        SUBSAMPLE_TEST_DATA *pSubsampleTestData = NULL;

                        pSubsampleTestData = &pSampleTestData->rgoSubsampleTestData[ iSubsample ];

                        /* Check clear byte count */
                        ChkDR( DRM_M2TS_BSR_ReadBitsToWORD( &oBSRContext, 16, &pSubsampleTestData->cbClear ) );

                        /* Check protected byte count */
                        ChkDR( DRM_M2TS_BSR_ReadBitsToDWORD( &oBSRContext, 32, &pSubsampleTestData->cbEncrypted ) );
                    }
                }
            }
        }

        (*pcECMTestData)++;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_VerifyECM
**
** Synopsis:    Verify ECM packets
**
** Arguments:   argv[0] : TRUE/FALSE flag - has PRO
**              argv[1] : Stream count (repeat 2 to 3 for each stream)
**              argv[2] : KID
**              argv[3] : PES count (repeat 4 to 5 for each PES)
**              argv[4] : PTS
**              argv[5] : Sample count (repeat 6 for each sample)
**              argv[6] : Subsample count (repeat 7 and 8 for each subsample)
**              argv[7] : Clear bytes
**              argv[8] : Protected bytes
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_VerifyECM( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_BYTE             iStream            = 0;
    DRM_WORD             iArgument          = 0;
    DRM_DWORD            iECM               = 0;
    DRM_DWORD           *pcECMTestData      = 0;
    ECM_TEST_DATA       *prgoECMTestData    = NULL;
    ECM_TEST_DATA       *pECMTestData       = NULL;
    DRM_CONST_STRING     dstrKeyID          = EMPTY_DRM_STRING;

    ChkArg( argc >= 5 );
    for( iArgument = 0; iArgument < 5; iArgument++ )
    {
        ChkArg( HASARG( iArgument ) );
    }

    MAP_TO_GLOBAL_TEST_VAR( prgoECMTestData, TEST_VAR_M2TS_ECM_TEST_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pcECMTestData, TEST_VAR_M2TS_ECM_COUNT );


    /* ECM packets should have been parsed by ParseECM. Iterate through parsed packets and perform additional verification. */
    iArgument = 0;

    while( HASARG( iArgument ) )
    {
        ChkBOOL( *pcECMTestData > iECM, DRM_E_TEST_UNEXPECTED_OUTPUT );

        pECMTestData = &prgoECMTestData[ iECM ];

        /* Check for PRO */
        ChkBOOL( ( DRMCRT_strncmp( argv[iArgument++], "TRUE", 4 ) == 0 ) == pECMTestData->fHasPRO, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Check stream count */
        ChkBOOL( pECMTestData->cStreams == (DRM_BYTE)strtoul( argv[iArgument++], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

        for( iStream = 0; iStream < pECMTestData->cStreams; iStream++ )
        {
            DRM_KID             oExpectedKID            = { 0 };
            DRM_DWORD           cbKID                   = SIZEOF( oExpectedKID );
            DRM_BYTE            iPES                    = 0;
            STREAM_TEST_DATA   *pStreamTestData         = NULL;

            pStreamTestData = &pECMTestData->rgoStreamTestData[ iStream ];

            ChkArg( HASARG( iArgument ) && HASARG( iArgument + 1 ) &&  HASARG( iArgument + 2 ) && HASARG( iArgument + 3 ) );

            FREEDRMSTRING( dstrKeyID );
            ChkDR( MakeDRMString( &dstrKeyID, argv[ iArgument++ ] ) );
            ChkDR( DRM_B64_DecodeW( &dstrKeyID, &cbKID, oExpectedKID.rgb, 0 ) );

            /* Check KID */
            ChkBOOL( 0 == MEMCMP( pStreamTestData->oKID.rgb, oExpectedKID.rgb, SIZEOF( pStreamTestData->oKID.rgb ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

            /* Check PES count */
            ChkBOOL( pStreamTestData->cPES == (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

            for( iPES = 0; iPES < pStreamTestData->cPES; iPES++ )
            {
                DRM_BYTE             iSample        = 0;
                const PES_TEST_DATA *pPESTestData   = NULL;

                pPESTestData = &pStreamTestData->rgoPESTestData[ iPES ];

                /* Check PTS */
                /* We currently only support the low-order DWORD of the QWORD */
                ChkBOOL( DRM_UI64Eql( pPESTestData->qwPTS, DRM_UI64HL( 0, strtoul( argv[iArgument++], NULL, 0 ) ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

                /* Check sample count */
                ChkBOOL( pPESTestData->cSamples == (DRM_BYTE)strtoul( argv[iArgument++], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

                for( iSample = 0; iSample < pPESTestData->cSamples; iSample++ )
                {
                    DRM_WORD                iSubsample          = 0;
                    const SAMPLE_TEST_DATA *pSampleTestData     = NULL;

                    ChkArg( HASARG( iArgument ) );

                    pSampleTestData = &pPESTestData->rgoSampleTestData[ iSubsample ];

                    /* Check subsample count */
                    ChkBOOL( pSampleTestData->cSubsamples == (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

                    for( iSubsample = 0; iSubsample < pSampleTestData->cSubsamples; iSubsample++ )
                    {
                        const SUBSAMPLE_TEST_DATA *pSubsampleTestData = NULL;

                        ChkArg( HASARG( iArgument ) && HASARG( iArgument + 1 ) );

                        pSubsampleTestData = &pSampleTestData->rgoSubsampleTestData[ iSubsample ];

                        /* Check clear byte count */
                        ChkBOOL( pSubsampleTestData->cbClear == (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );

                        /* Check protected byte count */
                        ChkBOOL( pSubsampleTestData->cbEncrypted == (DRM_DWORD)strtoul( argv[iArgument++], NULL, 0 ), DRM_E_TEST_UNEXPECTED_OUTPUT );
                    }
                }
            }
        }

        iECM++;
    }

    ChkBOOL( *pcECMTestData == iECM, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    FREEDRMSTRING( dstrKeyID );
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_BuildH264PES
**
** Synopsis:    Build and verify H264 video PES packets.  Only NAL06, NAL07 and NAL08 are in
**              clear, all other units are protected.
**
** Arguments:   argv[0] : Has missing packet by making the CC interruptedly
**              argv[1] : PID of the PES packets
**              argv[2] : Adaptation field length
**              argv[3] : PES header
**              argv[4] : Data alignment flag (TRUE/FALSE)
**              argv[5] : Size of the start code, either 3 or 4
**              argv[6] : Number of PES packets (repeat 7 for every PES packet)
**              argv[7] : Number of NAL units (repeat 8-9 for every NAL unit)
**              argv[8] : NAL Type, the expected types are from "NAL01" to "NAL12"
**              argv[9] : Number of bytes for the NAL data
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_BuildH264PES( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                                             = DRM_SUCCESS;
    DRM_WORD         wPID                                           = DRM_M2TS_UNDEFINED_PID;
    DRM_BOOL         fInterruptedCC                                 = FALSE;
    static DRM_BYTE  bNextCC                                        = 0;
    DRM_BYTE         cbApaptationField                              = 0;
    DRM_BYTE        *pbPESData                                      = NULL;
    DRM_DWORD        cbPESData                                      = MAX_PES_DATA_SIZE;
    DRM_DWORD        dwOffset                                       = 0;
    DRM_BOOL         fDataAlignment                                 = TRUE;
    DRM_DWORD        cPES                                           = 0;
    DRM_BYTE         cbStartCode                                    = 0;
    DRM_WORD         cNALUnits                                      = 0;
    DRM_BYTE         bNALType                                       = 0;
    DRM_WORD         cbNalData                                      = 0;
    const DRM_BYTE  *pbPESHeader                                    = NULL;
    DRM_WORD         cbPESHeader                                    = 0;
    DRM_BYTE         rgbUnalignedData[DRM_M2TS_MAX_TS_PAYLOAD_SIZE] = { 0 };
    DRM_BYTE         cbUnalignedData                                = 0;
    DRM_WORD         iArgument                                      = 0;
    DRM_DWORD        iPES                                           = 0;
    DRM_DWORD        iNALUnit                                       = 0;

    ChkArg( argc >= 6 );
    for( iArgument = 0; iArgument <= 6; iArgument++ )
    {
        ChkArg( HASARG( iArgument ) );
    }

    ChkMem( pbPESData = ( DRM_BYTE * )Oem_MemAlloc( cbPESData ) );

    fInterruptedCC = DRMCRT_strncmp( argv[0], "TRUE", 4 ) == 0;
    wPID = (DRM_WORD)strtoul( argv[1], NULL, 0 );
    cbApaptationField = (DRM_BYTE)strtoul( argv[2], NULL, 0 );
    ChkBOOL( cbApaptationField < DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_TEST_INVALIDARG );

    /* PES HEader */
    if( DRMCRT_strncmp( argv[3], "H264_PES_HEADER_PTS_903060", 26 ) == 0 )
    {
        pbPESHeader = g_rgbH264_PES_HEADER_PTS_903060;
        cbPESHeader = SIZEOF( g_rgbH264_PES_HEADER_PTS_903060 );
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

    /* Data alignment flag */
    fDataAlignment = DRMCRT_strncmp( argv[4], "TRUE", 4 ) == 0;

    /* Number of bytes for the start code */
    cbStartCode = (DRM_BYTE)strtoul( argv[5], NULL, 0 );
    ChkBOOL( cbStartCode == 3 || cbStartCode == 4, DRM_E_TEST_INVALIDARG );

    /* Number of PES packets */
    cPES = (DRM_DWORD)strtoul( argv[6], NULL, 0 );

    iArgument = 7;
    for( iPES = 0; iPES < cPES; iPES++ )
    {
        ChkBOOL( HASARG( iArgument ), DRM_E_TEST_INVALIDARG );

        dwOffset = 0;

        /* copy the PES header */
        ChkBOOL( cbPESData > cbPESHeader, DRM_E_TEST_INVALIDARG );
        MEMCPY( pbPESData, pbPESHeader, cbPESHeader );
        dwOffset += cbPESHeader;

        /* Change data alignment flag */
        if( fDataAlignment )
        {
            pbPESData[6] |= 0x04; /* Set data alignment flag to 1*/
        }
        else
        {
            pbPESData[6] &= 0xFB; /* Set data alignment flag to 0*/
        }

        /* If there is left over from previous PES (unaligned) then copy it */
        if( cbUnalignedData > 0 )
        {
            DRMASSERT( !fDataAlignment && iPES > 0 );
            MEMCPY( pbPESData + dwOffset, rgbUnalignedData, cbUnalignedData );
            dwOffset += cbUnalignedData;
        }

        cNALUnits = (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 );
        for( iNALUnit = 0; iNALUnit < cNALUnits; iNALUnit++ )
        {
            ChkBOOL( HASARG( iArgument ) && HASARG( iArgument + 1 ), DRM_E_TEST_INVALIDARG );

            /* Get the NAL unit type, the expected names are NLA01 to NAL12 */
            ChkBOOL( DRMCRT_strlen( argv[iArgument] ) == 5, DRM_E_TEST_INVALIDARG );
            bNALType = (DRM_BYTE)strtoul( argv[iArgument++] + 3, NULL, 10 ); /* plus 3 to skip the prefix of NAL */
            ChkBOOL( bNALType >= FIRST_H264_NAL_UNIT_TYPE && bNALType <= LAST_H264_NAL_UNIT_TYPE, DRM_E_TEST_INVALIDARG);

            /* Get the NLA data length */
            cbNalData = (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 );
            ChkBOOL( dwOffset + cbStartCode + 1 + cbNalData <= cbPESData, DRM_E_TEST_INVALIDARG ); /* Exceed the max buffer size */

            /* Now add the NAL unit */
            MEMSET( pbPESData + dwOffset, 0, (DRM_SIZE_T)( cbStartCode - 1 ) );  /* Set the 0x0000 or 0x000000 of the start code */
            dwOffset += cbStartCode - 1;
            pbPESData[dwOffset++] = 0x01; /* the 0x01 of the 0x000001 or 0x00000001 */
            pbPESData[dwOffset++] = bNALType;
            MEMSET( pbPESData + dwOffset, 0xAA, cbNalData );  /* Set 0xAA to the entire NAL data */
            dwOffset += cbNalData;
        }

        /* if it is not data aligned and not last PES */
        cbUnalignedData = 0;
        if( !fDataAlignment && ( iPES + 1 ) < cPES )
        {
            cbUnalignedData = ( dwOffset + cbApaptationField ) % DRM_M2TS_MAX_TS_PAYLOAD_SIZE;
            if( cbUnalignedData > 0 )
            {
                /* Copy the unaligned data to rgbUnalignedData wait for next PES to pick it up */
                dwOffset -= cbUnalignedData;
                MEMCPY( rgbUnalignedData, pbPESData + dwOffset, cbUnalignedData );
            }
        }

        ChkDR( _BuildPackets( wPID,
                              pbPESData,
                              dwOffset,
                              cbApaptationField,
                              &bNextCC,
                              fInterruptedCC ) );
    }

ErrorExit:
    SAFE_OEM_FREE( pbPESData );
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_BuildMPEG2PES
**
** Synopsis:    Build and verify MPEG2 video PES packets.  Only S001 (slice data 0x01) to SL075
**              (slice data 0xAF) are protected including the start code (0x000001) + start code
**              value byte, all others are in clear.
**
** Arguments:   argv[0] : Has missing packet by making the CC interruptedly
**              argv[1] : PID of the PES packets
**              argv[2] : Adaptation field length
**              argv[3] : PES header
**              argv[4] : Data alignment flag (TRUE/FALSE)
**              argv[5] : Number of PES packets (repeat 8 for every PES packet)
**              argv[6] : Number of start codes (repeat 7-8 for every start code)
**              argv[7] : start code value, the expected calues are 00, B0-B8 and S001-S175 (for the slice data, value from 0x01 through 0xAF)
**              argv[8] : Number of bytes for the payload of the start code value
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_BuildMPEG2PES( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                                             = DRM_SUCCESS;
    DRM_WORD         wPID                                           = DRM_M2TS_UNDEFINED_PID;
    DRM_BOOL         fInterruptedCC                                 = FALSE;
    static DRM_BYTE  bNextCC                                        = 0;
    DRM_BYTE         cbApaptationField                              = 0;
    DRM_BYTE        *pbPESData                                      = NULL;
    DRM_DWORD        cbPESData                                      = MAX_PES_DATA_SIZE;
    DRM_DWORD        dwOffset                                       = 0;
    DRM_BOOL         fDataAlignment                                 = TRUE;
    DRM_DWORD        cPES                                           = 0;
    DRM_BYTE         cbStartCode                                    = 3;
    DRM_WORD         cStartCodes                                    = 0;
    DRM_BYTE         bStartCodeValue                                = 0;
    DRM_WORD         cbStartCodePayload                             = 0;
    const DRM_BYTE  *pbPESHeader                                    = NULL;
    DRM_WORD         cbPESHeader                                    = 0;
    DRM_BYTE         rgbUnalignedData[DRM_M2TS_MAX_TS_PAYLOAD_SIZE] = { 0 };
    DRM_BYTE         cbUnalignedData                                = 0;
    DRM_WORD         iArgument                                      = 0;
    DRM_DWORD        iPES                                           = 0;
    DRM_DWORD        iStartCode                                     = 0;

    ChkArg( argc >= 5 );
    for( iArgument = 0; iArgument <= 5; iArgument++ )
    {
        ChkArg( HASARG( iArgument ) );
    }

    ChkMem( pbPESData = ( DRM_BYTE * )Oem_MemAlloc( cbPESData ) );

    fInterruptedCC = DRMCRT_strncmp( argv[0], "TRUE", 4 ) == 0;
    wPID = (DRM_WORD)strtoul( argv[1], NULL, 0 );
    cbApaptationField = (DRM_BYTE)strtoul( argv[2], NULL, 0 );
    ChkBOOL( cbApaptationField < DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_TEST_INVALIDARG );

    /* PES HEader */
    if( DRMCRT_strncmp( argv[3], "MPEG2_PES_HEADER_PTS_2648725394", 31 ) == 0 )
    {
        pbPESHeader = g_rgbMPEG2_PES_HEADER_PTS_2648725394;
        cbPESHeader = SIZEOF( g_rgbMPEG2_PES_HEADER_PTS_2648725394 );
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

    /* Data alignment flag */
    fDataAlignment = DRMCRT_strncmp( argv[4], "TRUE", 4 ) == 0;

    /* Number of PES packets */
    cPES = (DRM_DWORD)strtoul( argv[5], NULL, 0 );

    iArgument = 6;
    for( iPES = 0; iPES < cPES; iPES++ )
    {
        ChkBOOL( HASARG( iArgument ), DRM_E_TEST_INVALIDARG );

        dwOffset = 0;

        /* copy the PES header */
        ChkBOOL( cbPESData > cbPESHeader, DRM_E_TEST_INVALIDARG );
        MEMCPY( pbPESData, pbPESHeader, cbPESHeader );
        dwOffset += cbPESHeader;

        /* Change data alignment flag */
        if( fDataAlignment )
        {
            pbPESData[6] |= 0x04; /* Set data alignment flag to 1*/
        }
        else
        {
            pbPESData[6] &= 0xFB; /* Set data alignment flag to 0*/
        }

        /* If there is left over from previous PES (unaligned) then copy it */
        if( cbUnalignedData > 0 )
        {
            DRMASSERT( !fDataAlignment && iPES > 0 );
            MEMCPY( pbPESData + dwOffset, rgbUnalignedData, cbUnalignedData );
            dwOffset += cbUnalignedData;
        }

        cStartCodes = (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 );
        for( iStartCode = 0; iStartCode < cStartCodes; iStartCode++ )
        {
            DRM_DWORD cbStartCodeArgument = 0;

            ChkBOOL( HASARG( iArgument ) && HASARG( iArgument + 1 ), DRM_E_TEST_INVALIDARG );

            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( argv[iArgument] ), &cbStartCodeArgument ) );

            /* Get the start code value, the expected values are 00, B0-B8, S001-S157 */
            if( argv[iArgument][0] == '0' )
            {
                ChkBOOL( cbStartCodeArgument == 2 || argv[iArgument][1] == '0', DRM_E_TEST_INVALIDARG );
                bStartCodeValue = 0;
            }
            else if( argv[iArgument][0] == 'B' )
            {
                ChkBOOL( cbStartCodeArgument == 2 && argv[iArgument][1] >= '0' && argv[iArgument][1] <= '8', DRM_E_TEST_INVALIDARG );
                bStartCodeValue = (DRM_BYTE)strtoul( argv[iArgument] + 1, NULL, 10 );
                bStartCodeValue |= 0xB0;
            }
            else if( argv[iArgument][0] == 'S' )
            {
                ChkBOOL( cbStartCodeArgument == 4, DRM_E_TEST_INVALIDARG );
                bStartCodeValue = (DRM_BYTE)strtoul( argv[iArgument] + 1, NULL, 10 );
                ChkBOOL( bStartCodeValue >= FIRST_MPEG2_SLICE_DATA_START_CODE_VALUE && bStartCodeValue <= LAST_MPEG2_SLICE_DATA_START_CODE_VALUE, DRM_E_TEST_INVALIDARG );
            }
            else
            {
                ChkDR( DRM_E_TEST_INVALIDARG );
            }
            iArgument++;

            /* Get the length of the start code payload */
            cbStartCodePayload = (DRM_WORD)strtoul( argv[iArgument++], NULL, 0 );
            ChkBOOL( dwOffset + cbStartCode + 1 + cbStartCodePayload <= cbPESData, DRM_E_TEST_INVALIDARG ); /* Exceed the max buffer size */

            /* Now add the start code and its payload */
            MEMSET( pbPESData + dwOffset, 0, (DRM_SIZE_T)( cbStartCode - 1 ) );  /* Set the 0x0000 of the start code */
            dwOffset += cbStartCode - 1;
            pbPESData[dwOffset++] = 0x01; /* the 0x01 of the 0x000001 */
            pbPESData[dwOffset++] = bStartCodeValue;
            MEMSET( pbPESData + dwOffset, 0xAA, cbStartCodePayload );  /* Set 0xAA to the entire NAL data */
            dwOffset += cbStartCodePayload;
        }

        /* if it is not data aligned and not last PES */
        cbUnalignedData = 0;
        if( !fDataAlignment && ( iPES + 1 ) < cPES )
        {
            cbUnalignedData = ( dwOffset + cbApaptationField ) % DRM_M2TS_MAX_TS_PAYLOAD_SIZE;
            if( cbUnalignedData > 0 )
            {
                /* Copy the unaligned data to rgbUnalignedData wait for next PES to pick it up */
                dwOffset -= cbUnalignedData;
                MEMCPY( rgbUnalignedData, pbPESData + dwOffset, cbUnalignedData );
            }
        }

        ChkDR( _BuildPackets( wPID,
                              pbPESData,
                              dwOffset,
                              cbApaptationField,
                              &bNextCC,
                              fInterruptedCC ) );
    }

ErrorExit:
    SAFE_OEM_FREE( pbPESData );
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_BuildAudioPES
**
** Synopsis:    Build and verify DDPlus/ADTS/MPEGA PES packets.  The frame header is in clear
**              and the frame data is protected.
**
** Arguments:   argv[0] : Has missing packet by making the CC interruptedly
**              argv[1] : PID of the PES packets
**              argv[2] : Adaptation field length
**              argv[3] : PES header
**              argv[4] : Data alignment flag (TRUE/FALSE)
**              argv[5] : Number of PES packets (repeat 6 for every PES packet)
**              argv[6] : Frame header
**              argv[7] : Numer of audio samples
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_BuildAudioPES( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                                             = DRM_SUCCESS;
    DRM_WORD         wPID                                           = DRM_M2TS_UNDEFINED_PID;
    DRM_BOOL         fInterruptedCC                                 = FALSE;
    static DRM_BYTE  bNextCC                                        = 0;
    DRM_BYTE         cbApaptationField                              = 0;
    DRM_BYTE        *pbPESData                                      = NULL;
    DRM_DWORD        cbPESData                                      = MAX_PES_DATA_SIZE;
    DRM_DWORD        dwOffset                                       = 0;
    const DRM_BYTE  *pbFrameHeader                                  = NULL;
    DRM_DWORD        cbFrameHeader                                  = 0;
    DRM_DWORD        cbFrameHeaderActual                            = 0;
    DRM_DWORD        cbFrame                                        = 0;
    DRM_BOOL         fDataAlignment                                 = TRUE;
    DRM_DWORD        cPES                                           = 0;
    DRM_DWORD        cSamples                                       = 0;
    const DRM_BYTE  *pbPESHeader                                    = NULL;
    DRM_WORD         cbPESHeader                                    = 0;
    DRM_BYTE         rgbUnalignedData[DRM_M2TS_MAX_TS_PAYLOAD_SIZE] = { 0 };
    DRM_BYTE         cbUnalignedData                                = 0;
    DRM_WORD         iArgument                                      = 0;
    DRM_DWORD        iPES                                           = 0;
    DRM_DWORD        iSample                                        = 0;

    ChkArg( argc >= 5 );
    for( iArgument = 0; iArgument <= 5; iArgument++ )
    {
        ChkArg( HASARG( iArgument ) );
    }

    ChkMem( pbPESData = ( DRM_BYTE * )Oem_MemAlloc( cbPESData ) );

    fInterruptedCC = DRMCRT_strncmp( argv[0], "TRUE", 4 ) == 0;
    wPID = (DRM_WORD)strtoul( argv[1], NULL, 0 );
    cbApaptationField = (DRM_BYTE)strtoul( argv[2], NULL, 0 );
    ChkBOOL( cbApaptationField < DRM_M2TS_MAX_TS_PAYLOAD_SIZE, DRM_E_TEST_INVALIDARG );

    /* PES HEader */
    if( DRMCRT_strncmp( argv[3], "DDPLUS_PES_HEADER_PTS_2648661236", 32 ) == 0 )
    {
        pbPESHeader = g_rgbDDPLUS_PES_HEADER_PTS_2648661236;
        cbPESHeader = SIZEOF( g_rgbDDPLUS_PES_HEADER_PTS_2648661236 );
    }
    else if( DRMCRT_strncmp( argv[3], "ADTS_PES_HEADER_PTS_900000", 26 ) == 0 )
    {
        pbPESHeader = g_rgbADTS_PES_HEADER_PTS_900000;
        cbPESHeader = SIZEOF( g_rgbADTS_PES_HEADER_PTS_900000 );
    }
    else if( DRMCRT_strncmp( argv[3], "MPEGA_PES_HEADER_PTS_2119045161", 31 ) == 0 )
    {
        pbPESHeader = g_rgbMPEGA_PES_HEADER_PTS_2119045161;
        cbPESHeader = SIZEOF( g_rgbMPEGA_PES_HEADER_PTS_2119045161 );
    }
    else if( DRMCRT_strncmp( argv[3], "MPEGA_PES_HEADER_NO_START_CODE", 30 ) == 0 )
    {
        pbPESHeader = g_rgbMPEGA_PES_HEADER_NO_START_CODE;
        cbPESHeader = SIZEOF( g_rgbMPEGA_PES_HEADER_NO_START_CODE );
    }
    else if( DRMCRT_strncmp( argv[3], "MPEGA_PES_HEADER_NO_SECLEN", 26 ) == 0 )
    {
        pbPESHeader = g_rgbMPEGA_PES_HEADER_NO_SECLEN;
        cbPESHeader = SIZEOF( g_rgbMPEGA_PES_HEADER_NO_SECLEN );
    }
    else if( DRMCRT_strncmp( argv[3], "MPEGA_PES_HEADER_NO_PTS", 23 ) == 0 )
    {
        pbPESHeader = g_rgbMPEGA_PES_HEADER_NO_PTS;
        cbPESHeader = SIZEOF( g_rgbMPEGA_PES_HEADER_NO_PTS );
    }
    else
    {
        ChkDR( DRM_E_TEST_INVALIDARG );
    }

    /* Data alignment flag */
    fDataAlignment = DRMCRT_strncmp( argv[4], "TRUE", 4 ) == 0;

    /* Number of PES packets */
    cPES = (DRM_DWORD)strtoul( argv[5], NULL, 0 );

    iArgument = 6;
    for( iPES = 0; iPES < cPES; iPES++ )
    {
        ChkBOOL( HASARG( iArgument ) && HASARG( iArgument + 1 ), DRM_E_TEST_INVALIDARG );

        dwOffset = 0;

        /* copy the PES header */
        ChkBOOL( cbPESData > cbPESHeader, DRM_E_TEST_INVALIDARG );
        MEMCPY( pbPESData, pbPESHeader, cbPESHeader );
        dwOffset += cbPESHeader;

        /* Change data alignment flag */
        if( fDataAlignment )
        {
            pbPESData[6] |= 0x04; /* Set data alignment flag to 1*/
        }
        else
        {
            pbPESData[6] &= 0xFB; /* Set data alignment flag to 0*/
        }

        /* If there is left over from previous PES (unaligned) then copy it */
        if( cbUnalignedData > 0 )
        {
            DRMASSERT( !fDataAlignment && iPES > 0 );
            MEMCPY( pbPESData + dwOffset, rgbUnalignedData, cbUnalignedData );
            dwOffset += cbUnalignedData;
        }

        /* Get the frame header */
        if( DRMCRT_strncmp( argv[iArgument], "DDPLUS_FRAME_HEADER_FS_768", 26 ) == 0 )
        {
            pbFrameHeader = g_rgbDDPLUS_FRAME_HEADER_FS_768;
            cbFrameHeader = SIZEOF( g_rgbDDPLUS_FRAME_HEADER_FS_768 );
            cbFrame = 768;
            cbFrameHeaderActual = DDPLUS_HEADER_SIZE;
        }
        else if( DRMCRT_strncmp( argv[iArgument], "INVALID_DDPLUS_FRAME_HEADER_FS_768", 34 ) == 0 )
        {
            pbFrameHeader = g_rgbDDPLUS_FRAME_HEADER_FS_768_INVALID;
            cbFrameHeader = SIZEOF( g_rgbDDPLUS_FRAME_HEADER_FS_768_INVALID );
            cbFrame = 768;
            cbFrameHeaderActual = DDPLUS_HEADER_SIZE;
        }
        else if( DRMCRT_strncmp( argv[iArgument], "ADTS_FRAME_HEADER_FS_371", 24 ) == 0 )
        {
            pbFrameHeader = g_rgbADTS_FRAME_HEADER_FS_371;
            cbFrameHeader = SIZEOF( g_rgbADTS_FRAME_HEADER_FS_371 );
            cbFrame = 371;
            cbFrameHeaderActual = cbFrameHeader;
        }
        else if( DRMCRT_strncmp( argv[iArgument], "INVALID_ADTS_FRAME_HEADER_FS_371", 32 ) == 0 )
        {
            pbFrameHeader = g_rgbADTS_FRAME_HEADER_FS_371_INVALID;
            cbFrameHeader = SIZEOF( g_rgbADTS_FRAME_HEADER_FS_371_INVALID );
            cbFrame = 371;
            cbFrameHeaderActual = cbFrameHeader;
        }
        else if( DRMCRT_strncmp( argv[iArgument], "MPEGA_FRAME_HEADER_FS_417", 25 ) == 0 )
        {
            pbFrameHeader = g_rgbMPEGA_FRAME_HEADER_FS_417;
            cbFrameHeader = SIZEOF( g_rgbMPEGA_FRAME_HEADER_FS_417 );
            cbFrame = 417;
            cbFrameHeaderActual = cbFrameHeader;
        }
        else if( DRMCRT_strncmp( argv[iArgument], "INVALID_MPEGA_FRAME_HEADER_FS_417", 33 ) == 0 )
        {
            pbFrameHeader = g_rgbMPEGA_FRAME_HEADER_FS_417_INVALID;
            cbFrameHeader = SIZEOF( g_rgbMPEGA_FRAME_HEADER_FS_417_INVALID );
            cbFrame = 417;
            cbFrameHeaderActual = cbFrameHeader;
        }
        else
        {
            ChkDR( DRM_E_TEST_INVALIDARG );
        }
        iArgument++;

        /* Get the sample count */
        cSamples = (DRM_DWORD)strtoul( argv[iArgument++], NULL, 0 );

        /* Create sample data */
        for( iSample = 0; iSample < cSamples; iSample++ )
        {
            ChkBOOL( dwOffset + cbFrame <= cbPESData, DRM_E_TEST_INVALIDARG ); /* Exceed the max buffer size */
            /* copy the frame header */
            MEMCPY( pbPESData + dwOffset, pbFrameHeader, cbFrameHeader );
            dwOffset += cbFrameHeader;

            /* fill the remaining of the frame header with 0x00 */
            ChkBOOL( cbFrameHeader <= cbFrameHeaderActual, DRM_E_TEST_INVALIDARG );
            MEMSET( pbPESData + dwOffset, 0x00, cbFrameHeaderActual - cbFrameHeader );
            dwOffset += cbFrameHeaderActual - cbFrameHeader;

            /* Fill the frame data with 0xAA */
            ChkBOOL( cbFrameHeaderActual < cbFrame, DRM_E_TEST_INVALIDARG );
            MEMSET( pbPESData + dwOffset, 0xAA, cbFrame - cbFrameHeaderActual );
            dwOffset += cbFrame - cbFrameHeaderActual;
        }

        /* if it is not data aligned and not last PES */
        cbUnalignedData = 0;
        if( !fDataAlignment && ( iPES + 1 ) < cPES )
        {
            cbUnalignedData = ( dwOffset + cbApaptationField ) % DRM_M2TS_MAX_TS_PAYLOAD_SIZE;
            if( cbUnalignedData > 0 )
            {
                /* Copy the unaligned data to rgbUnalignedData wait for next PES to pick it up */
                dwOffset -= cbUnalignedData;
                MEMCPY( rgbUnalignedData, pbPESData + dwOffset, cbUnalignedData );
            }
        }

        ChkDR( _BuildPackets( wPID,
                              pbPESData,
                              dwOffset,
                              cbApaptationField,
                              &bNextCC,
                              fInterruptedCC ) );
    }

ErrorExit:
    SAFE_OEM_FREE( pbPESData );
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_InjectDataIntoPacketsBuffer
**
** Synopsis:    Inject 0xFF bytes into the packet input buffer
**
** Arguments:   argv[0] : Number of bytes to inject
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_InjectDataIntoPacketsBuffer( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr                     = DRM_SUCCESS;
    DRM_BYTE                            *pbPacketsBuffer        = NULL;
    DRM_DWORD                           *pcbPacketsBuffer       = NULL;
    DRM_DWORD                           *pdwPacketsBufferOffset = NULL;
    DRM_BYTE                             dwByte                 = 0xFF;
    DRM_DWORD                            cbNumBytes             = 0;

    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );

    ChkArg( argc == 1 && HASARG( 0 ) );
    cbNumBytes = ( DRM_DWORD )strtoul( argv[0], NULL, 0 );
    ChkArg( *pdwPacketsBufferOffset + cbNumBytes <= *pcbPacketsBuffer );

    MEMSET( (pbPacketsBuffer + *pdwPacketsBufferOffset), dwByte, cbNumBytes );

ErrorExit:

    return dr;
}


/********************************************************************************************
** Function:    TestEncryptor_ClearPacketsBuffer
**
** Synopsis:    Clear the packet input buffer
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_ClearPacketsBuffer( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr                     = DRM_SUCCESS;
    DRM_BYTE                            *pbPacketsBuffer        = NULL;
    DRM_DWORD                           *pcbPacketsBuffer       = NULL;
    DRM_DWORD                           *pdwPacketsBufferOffset = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbPacketsBuffer, TEST_VAR_M2TS_PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwPacketsBufferOffset, TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET );

    *pdwPacketsBufferOffset = 0;

    MEMSET(pbPacketsBuffer, 0, *pcbPacketsBuffer);

ErrorExit:

    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_ClearEncryptedBuffer
**
** Synopsis:    Clear the encrypted packet output buffer
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_ClearEncryptedBuffer( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                           dr                       = DRM_SUCCESS;
    DRM_BYTE                            *pbEncryptedBuffer        = NULL;
    DRM_DWORD                           *pcbEncryptedBuffer       = NULL;
    DRM_DWORD                           *pdwEncryptedBufferOffset = NULL;

    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER );
    MAP_TO_GLOBAL_TEST_VAR( pcbEncryptedBuffer, TEST_VAR_M2TS_ENCRYPTED_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedBufferOffset, TEST_VAR_M2TS_ENCRYPTED_BUFFER_OFFSET );

    *pdwEncryptedBufferOffset = 0;

    MEMSET(pbEncryptedBuffer, 0, *pcbEncryptedBuffer);

ErrorExit:

    return dr;
}



/********************************************************************************************
** Function:    _VerifyPackets
**
** Synopsis:    Verify the adaptation field, header bytes, overflow bytes and payload.
**
********************************************************************************************/
static DRM_RESULT DRM_CALL _VerifyPackets(
    __in  const DRM_M2TS_PES  *f_pPES,
    __in  const DRM_BOOL       f_fKeptOverflowPayload )
{
    DRM_RESULT                 dr             = DRM_SUCCESS;
    const DRM_M2TS_PACKET     *pPacket        = NULL;
    DRM_BOOL                   fHasAdaptation = FALSE;
    DRM_BYTE                   bOffset        = 0;
    DRM_BYTE                   bExpected      = 0;
    DRM_BYTE                   iPhase         = 0;
    DRM_BYTE                   iByte          = 0;
    DRM_BYTE                   cBytes         = 0;

    ChkArg( f_pPES != NULL );

    pPacket = f_pPES->oPacketList.pHead;
    while( pPacket != NULL )
    {
        bOffset = 4;    /* to skip the TS header */

        for( iPhase = 0; iPhase < 4; iPhase++ )
        {
            cBytes = 0;

            switch( iPhase )
            {
            case 0: /* Adaptation */
                bExpected = DRM_M2TS_STUFFING_BYTE;
                fHasAdaptation = ( pPacket->rgbData[3] & DRM_M2TS_ADAPTATION_FIELD_MASK ) != 0;
                if( fHasAdaptation )
                {
                    cBytes = (DRM_BYTE)( pPacket->rgbData[4] > 0 ? pPacket->rgbData[4] - 1 : 0 );
                    bOffset++;
                    if( cBytes >= 1 )
                    {
                        bOffset++;
                    }
                }
                break;

            case 1: /* Header */
                bExpected = HEADER_BYTE;
                cBytes = pPacket->cbHeader;
                break;

            case 2: /* overflow payload */
                bExpected = OVERFLOW_PAYLOAD_BYTE;
                cBytes = pPacket->cbPayloadLastPES;
                ChkBOOL( f_fKeptOverflowPayload || cBytes == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
                break;

            case 3: /* regular payload */
                bExpected = REGULAR_PAYLOAD_BYTE;
                if( f_fKeptOverflowPayload && pPacket->rgbData[ bOffset ] != bExpected )
                {
                    /* This packet's header and regulare payload have been dropped, only the overflow payload left */
                    bExpected = OVERFLOW_PAYLOAD_BYTE;
                }
                cBytes = (DRM_BYTE)( pPacket->cbPayload - pPacket->cbHeader - pPacket->cbPayloadLastPES );
                break;

            default:
                DRMASSERT( FALSE );
                break;
            }

            for( iByte = 0; iByte < cBytes; iByte++ )
            {
                ChkBOOL( pPacket->rgbData[ bOffset++ ] == bExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );
            }
        }

        pPacket = pPacket->pNextPerUnit;
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    _SetPacketData
**
** Synopsis:    Initialize a packet with the data passed in the arguments
**
********************************************************************************************/
static DRM_RESULT DRM_CALL _SetPacketData(
    __inout         DRM_M2TS_ENCRYPTOR_CONTEXT      *f_pEncContext,
    __inout         DRM_M2TS_PES                    *f_pPES,
    __in      const DRM_M2TS_PACKET_STATE            f_ePacketState,
    __in      const DRM_BOOL                         f_fUnitStart,
    __in      const DRM_BYTE                         f_cbPayload,
    __in      const DRM_BYTE                         f_cbHeader,
    __in      const DRM_BYTE                         f_cbPayloadLastPES,
    __in      const DRM_WORD                         f_wPacketFlags,
    __inout         DRM_BYTE                        *f_pbContinuityCounter )
{
    DRM_RESULT       dr                             = DRM_SUCCESS;
    DRM_M2TS_PACKET *pPacket                        = NULL;
    DRM_BYTE         rgbData[DRM_M2TS_PACKET_SIZE]  = { 0 };
    DRM_BYTE         cbAdaptation                   = 0;
    DRM_BYTE         bOffset                        = 0;

    ChkArg( f_pEncContext != NULL );
    ChkArg( f_pPES != NULL );
    ChkArg( f_cbPayload >= f_cbHeader + f_cbPayloadLastPES );
    ChkArg( DRM_M2TS_MAX_TS_PAYLOAD_SIZE - f_cbPayload != 1 );  /* not allow to have 1-byte adaptation */

    ChkDR( DRM_M2TS_GetFreePacket( f_pEncContext, 0, f_ePacketState, FALSE, rgbData, &pPacket ) );
    DRM_M2TS_APPEND_TO_END_OF_LIST( &f_pEncContext->oIncomingPacketList, pPacket );
    DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &f_pPES->oPacketList, pPacket );

    if( f_fUnitStart )
    {
        pPacket->rgbData[1] |= DRM_M2TS_UNIT_START_MASK;
    }

    DRM_M2TS_SET_AND_INC_CONTINUITY_COUNTER( pPacket->rgbData, *f_pbContinuityCounter );

    pPacket->cbPayload        = f_cbPayload;
    pPacket->cbHeader         = f_cbHeader;
    pPacket->cbPayloadLastPES = f_cbPayloadLastPES;
    pPacket->wPacketFlags    |= f_wPacketFlags;

    bOffset = 4;  /* To skip the TS header which is 4 bytes */

    cbAdaptation = (DRM_BYTE)( DRM_M2TS_MAX_TS_PAYLOAD_SIZE - f_cbPayload );
    if( cbAdaptation > 0 )
    {
        pPacket->rgbData[3] |= DRM_M2TS_ADAPTATION_FIELD_MASK;
        pPacket->rgbData[4] = (DRM_BYTE)( cbAdaptation - 1 );
        if( cbAdaptation > 1 )
        {
            pPacket->rgbData[5] = 0;
            if( cbAdaptation > 2 )
            {
                MEMSET( pPacket->rgbData + 6, DRM_M2TS_STUFFING_BYTE, (DRM_SIZE_T)( pPacket->rgbData[4] - 1 ) );
            }
        }
        bOffset += cbAdaptation;
    }

    if( f_cbPayload > 0 )
    {
        pPacket->rgbData[3] |= DRM_M2TS_PAYLOAD_MASK;
    }

    if( f_cbHeader > 0 )
    {
        MEMSET( pPacket->rgbData + bOffset, HEADER_BYTE, f_cbHeader );
        bOffset += f_cbHeader;
    }

    if( f_cbPayloadLastPES > 0 )
    {
        MEMSET( pPacket->rgbData + bOffset, OVERFLOW_PAYLOAD_BYTE, f_cbPayloadLastPES );
        bOffset += f_cbPayloadLastPES;
    }

    if( bOffset < DRM_M2TS_PACKET_SIZE )
    {
        DRMASSERT( DRM_M2TS_PACKET_SIZE - bOffset == f_cbPayload - f_cbHeader - f_cbPayloadLastPES );
        MEMSET( pPacket->rgbData + bOffset, REGULAR_PAYLOAD_BYTE, (DRM_SIZE_T)( DRM_M2TS_PACKET_SIZE - bOffset ) );
    }

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_DropOveflowPayload
**
** Synopsis:    Test DRM_M2TS_DropOverflowPayload API
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_DropOveflowPayload( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                       dr                             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE       *phEncryptor                    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT      *pEncContext                    = NULL;
    DRM_M2TS_PES                     oPES                           = { 0 };
    DRM_BYTE                         bContinuityCounter             = 0;
    const DRM_DWORD                  cPacketsToAdd                  = 11;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, TRUE,   65,  11,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184,   0,   1, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184,  55,   1, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184, 183,   1, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184,   0,  23, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184,  78,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184,   0,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 184,  20, 120, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE, 135,   0, 122, DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE,  88,   0,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_READY, FALSE,  18,   0,   0, 0, &bContinuityCounter ) );

    ChkDR( DRM_M2TS_DropOverflowPayload( pEncContext, &oPES.oPacketList ) );

    ChkBOOL( oPES.oPacketList.cItems == cPacketsToAdd, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pEncContext->oIncomingPacketList.cItems == cPacketsToAdd, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( _VerifyPackets( &oPES, FALSE ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_KeepOveflowPayload
**
** Synopsis:    Test DRM_M2TS_KeepOverflowPayload API
**
** Arguments:   None
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_KeepOveflowPayload( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                       dr                             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE       *phEncryptor                    = NULL;
    DRM_M2TS_ENCRYPTOR_CONTEXT      *pEncContext                    = NULL;
    DRM_BYTE                         bContinuityCounter             = 0;
    DRM_M2TS_PES                     oPESLast                       = { 0 };
    DRM_M2TS_PES                     oPES                           = { 0 };
    DRM_M2TS_PACKET_LIST             oPacketList                    = { 0 };
    const DRM_M2TS_PACKET           *pPacket                        = NULL;
    DRM_M2TS_PACKET                 *pNewPacket                     = NULL;
    const DRM_DWORD                  cCurrentPES                    = 10;
    const DRM_DWORD                  cPacketsWithOverflowPlayload   = 5;
    const DRM_DWORD                  cPacketsPESLast                = 1;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );
    pEncContext = (DRM_M2TS_ENCRYPTOR_CONTEXT *)*phEncryptor;

    /* Set one packet in the last PES first in order to append the overflow packets to it */
    ChkDR( _SetPacketData( pEncContext, &oPESLast, eDRM_M2TS_PACKET_STATE_PENDING, TRUE,   65,  11,   0, 0, &bContinuityCounter ) );

    /* Set the packets to the current PES */
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, TRUE,   65,  11,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 184,   0, 183, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 184,   1, 183, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 184,   0,  23, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 184,  78,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 184,   0,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 184,  20, 120, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE, 135,   0, 122, DRM_M2TS_PACKET_FLAG_LAST_OF_LAST_PES_PACKET, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE,  88,   0,   0, 0, &bContinuityCounter ) );
    ChkDR( _SetPacketData( pEncContext, &oPES, eDRM_M2TS_PACKET_STATE_PENDING, FALSE,  18,   0,   0, 0, &bContinuityCounter ) );

    /* Duplicate the packets in the current PES that have the overflow payload */
    pPacket = oPES.oPacketList.pHead;
    while( pPacket != NULL )
    {
        if( pPacket->cbPayloadLastPES > 0  )
        {
            ChkDR( DRM_M2TS_GetFreePacket( pEncContext,
                                           pPacket->cbPayload,
                                           eDRM_M2TS_PACKET_STATE_PENDING,
                                           FALSE,
                                           pPacket->rgbData,
                                           &pNewPacket ) );

            pNewPacket->cbHeader = pPacket->cbHeader;
            pNewPacket->cbPayloadLastPES = pPacket->cbPayloadLastPES;

            DRM_M2TS_APPEND_TO_END_OF_PER_UNIT_LIST( &oPacketList, pNewPacket );
        }

        pPacket = pPacket->pNextPerUnit;
    }

    ChkDR( DRM_M2TS_KeepOverflowPayload( pEncContext, &oPESLast, &oPacketList ) );

    ChkBOOL( oPESLast.oPacketList.cItems == cPacketsWithOverflowPlayload + cPacketsPESLast, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pEncContext->oIncomingPacketList.cItems == cCurrentPES + cPacketsWithOverflowPlayload + cPacketsPESLast, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( _VerifyPackets( &oPESLast, TRUE ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    _LoggingCallback
**
** Synopsis:    Callback function to record encryption logs
**
** Arguments:   None
**
** Returns:     None
**
********************************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const");
static DRM_VOID DRM_CALL _LoggingCallback(
    __inout    DRM_VOID     *f_pCallbackContext,
    __in const DRM_M2TS_LOG *f_poLog )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_BOOL    fProcessed  = FALSE;
    DRM_DWORD   iEntry      = 0;

    if( f_poLog != NULL )
    {
        Log ( "Trace", "0x%8.8x logged by encryptor!\n", f_poLog->drResult);

        for( iEntry = 0; iEntry < g_cLogs && !fProcessed; iEntry++ )
        {
            if( g_rgoLogs[ iEntry ].drResult == f_poLog->drResult )
            {
                fProcessed = TRUE;
                g_rgoLogs[ iEntry ].cInstances++;
            }
        }

        if( !fProcessed )
        {
            if( g_cLogs < NO_OF( g_rgoLogs ) )
            {
                g_rgoLogs[ g_cLogs ].drResult = f_poLog->drResult;
                g_rgoLogs[ g_cLogs ].cInstances++;
                g_cLogs++;
            }
            else
            {
                Log ( "Trace", "Warning: Too many errors the encryption log is full!\n" );
            }
        }
    }
    else
    {
        Log ( "Trace", "Warning: Encryptor passed an unexpected NULL DRM_M2TS_LOG object!\n" );
    }

    UNREFERENCED_PARAMETER( f_pCallbackContext );
}


/********************************************************************************************
** Function:    TestEncryptor_InjectError
**
** Synopsis:    Inject an error to the packet to be built after this test action.  This action
**              doesn't keep a history of the privious error codes, it always overrides the
**              previous one.
**
** Arguments:   argv[0] : DRM error to inject or DRM_SUCCESS to reset
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_InjectError( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr = DRM_SUCCESS;

    ChkArg( argc == 1 );
    ChkArg( HASARG( 0 ) );

    g_drErrorToInject = ( DRM_RESULT )strtoul( argv[ 0 ], NULL, 0 );

ErrorExit:
    return dr;
}


/********************************************************************************************
** Function:    TestEncryptor_SetLoggingCallback
**
** Synopsis:    Call Drm_M2ts_SetEncryptionProperty to set the logging callback
**
** Arguments:   None
**
** Returns:     DRM_SUCCESS
**              DRM_E_M2TS_CONTEXT_NOT_INITIALIZED
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_SetLoggingCallback( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_M2TS_ENCRYPTOR_HANDLE       *phEncryptor    = NULL;
    DRM_M2TS_ENCRYPTION_PROPERTY     oProperty;

    MAP_TO_GLOBAL_TEST_VAR( phEncryptor, TEST_VAR_M2TS_ENCRYPTOR_HANDLE );

    MEMSET( &oProperty, 0, SIZEOF( oProperty ) );
    oProperty.ePropertyType = eDRM_M2TS_PROPERTY_TYPE_ENABLE_LOGGING;
    oProperty.Value.LoggingCallback.pfnLoggingCallback = _LoggingCallback;

    ChkDR( Drm_M2ts_SetEncryptionProperty( *phEncryptor, &oProperty ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestEncryptor_CheckLoggedResults
**
** Synopsis:    Build and verify DDPlus/ADTS/MPEGA PES packets.  The frame header is in clear
**              and the frame data is protected.
**
** Arguments:   argv[0] : Expected result code
**              argv[1] : Expected number of instance for the result
**              Not: Repeat above pair of arguments for each result code.
**                   If the test doesn't expect any logs from the encryptor
**                   don't specify the arguments.
**
** Returns:     DRM_SUCCESS
**              DRM_E_INVALIDARG
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestEncryptor_CheckLoggedResults( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    DRM_BOOL    fAsExpected     = TRUE;
    DRM_RESULT  drResult        = DRM_SUCCESS;
    DRM_DWORD   cInstances      = 0;
    DRM_WORD    iArgument       = 0;
    DRM_DWORD   iLog            = 0;

    while( HASARG( iArgument ) )
    {
        DRM_BOOL fFound = FALSE;

        ChkArg( HASARG( iArgument + 1 ) );

        drResult    = ( DRM_RESULT )strtoul( argv[ iArgument ], NULL, 0 );
        cInstances  = ( DRM_DWORD )strtoul( argv[ iArgument + 1 ], NULL, 0 );

        for( iLog = 0 ; iLog < g_cLogs && !fFound; iLog++ )
        {
            if( g_rgoLogs[ iLog ].drResult == drResult )
            {
                if( g_rgoLogs[ iLog ].cInstances != cInstances )
                {
                    Log ( "Trace", "0x%8.8x expected to occured %d times, but occured %d times!\n", drResult, cInstances, g_rgoLogs[ iLog ].cInstances );
                    fAsExpected = FALSE;
                }
                fFound = TRUE;
            }
        }

        if( !fFound && ( g_rgoLogs[ iLog ].cInstances != 0 ) )
        {
            Log ( "Trace", "0x%8.8x expected to occured %d times, but occured 0 times!\n", drResult, cInstances );
            fAsExpected = FALSE;
        }

        iArgument += 2;
    }

    ChkBOOL( fAsExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PreTestCase
**
** Synopsis:    Pretest case for the DST test module
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE( M2TSENCRYPTOR )( long lTCID, const char *strTCName )
{
    DRM_RESULT                   dr                         = DRM_SUCCESS;
    DRM_APP_CONTEXT             *pAppContext                = NULL;
    DRM_M2TS_ENCRYPTOR_HANDLE   *phEncryptor                = NULL;
    DRM_BYTE                    *pbPacketsBuffer            = NULL;
    DRM_DWORD                   *pcbPacketsBuffer           = NULL;
    DRM_DWORD                   *pdwPacketsBufferOffset     = NULL;
    DRM_BYTE                    *pbEncryptedBuffer          = NULL;
    DRM_DWORD                   *pcbEncryptedBuffer         = NULL;
    DRM_DWORD                   *pdwEncryptedBufferOffset   = NULL;
    DRM_BYTE                    *pnNextPATCC                = NULL;
    DRM_BYTE                    *pnNextPMTCC                = NULL;
    ECM_TEST_DATA               *prgoECMTestData            = NULL;
    DRM_DWORD                   *pcECMTestData              = NULL;
    DRM_KEYFILE_CONTEXT         *poKeyfileContext           = NULL;

    g_drErrorToInject   = DRM_SUCCESS;
    g_fAppContextInited = FALSE;

    MEMSET( g_rgoLogs, 0, SIZEOF( g_rgoLogs ) );
    g_cLogs = 0;

    ChkDR( SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME,
                         PRIVATE_KEY_FILE_NAME,
                         GROUP_CERT_FILE_NAME,
                         GC_PRIVATE_KEY_FILE_NAME,
                         TRUE ) );

    g_fAppContextInited = TRUE;

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( _CopyFileToDRMFolder( KEYFILE_FILE_NAME, KEYFILE_FILE_NAME ) );
        /*
        ** Set pointer to keyfile context in the blackbox
        */
        ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );
        ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
        ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext = poKeyfileContext;
        ChkDR( TST_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
        ChkDR( TST_KF_LoadFromFile( poKeyfileContext, TRUE ) );
        ChkDR( TST_UTL_SetPlayReadyCertCachedValues( &((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext ) );
        poKeyfileContext = NULL;
    }

    /*
    ** Set shared variables
    */
    MAP_TO_GLOBAL_TEST_VAR_Alloc( phEncryptor,              TEST_VAR_M2TS_ENCRYPTOR_HANDLE,         SIZEOF( DRM_M2TS_ENCRYPTOR_HANDLE ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbPacketsBuffer,          TEST_VAR_M2TS_PACKETS_BUFFER,           PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcbPacketsBuffer,         TEST_VAR_M2TS_PACKETS_BUFFER_SIZE,      SIZEOF( DRM_DWORD ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdwPacketsBufferOffset,   TEST_VAR_M2TS_PACKETS_BUFFER_OFFSET,    SIZEOF( DRM_DWORD ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbEncryptedBuffer,        TEST_VAR_M2TS_ENCRYPTED_BUFFER,         PACKETS_BUFFER_SIZE );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcbEncryptedBuffer,       TEST_VAR_M2TS_ENCRYPTED_BUFFER_SIZE,    SIZEOF( DRM_DWORD ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdwEncryptedBufferOffset, TEST_VAR_M2TS_ENCRYPTED_BUFFER_OFFSET,  SIZEOF( DRM_DWORD ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pnNextPATCC,              TEST_VAR_M2TS_PAT_CC,                   SIZEOF( DRM_BYTE ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pnNextPMTCC,              TEST_VAR_M2TS_PMT_CC,                   SIZEOF( DRM_BYTE ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( prgoECMTestData,          TEST_VAR_M2TS_ECM_TEST_DATA,            SIZEOF( ECM_TEST_DATA ) * MAX_TEST_ECMS );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcECMTestData,            TEST_VAR_M2TS_ECM_COUNT,                SIZEOF( DRM_DWORD ) );

    *pcbPacketsBuffer           = PACKETS_BUFFER_SIZE;
    *pdwPacketsBufferOffset     = 0;
    *pcbEncryptedBuffer         = PACKETS_BUFFER_SIZE;
    pdwEncryptedBufferOffset    = 0;
    *pnNextPATCC                = 0;
    *pnNextPMTCC                = 0;

ErrorExit:
    SAFE_OEM_FREE( poKeyfileContext );
    return dr;
}

/********************************************************************************************
** Function:    PostTestCase
**
** Synopsis:    Posttest case for the DST test module
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( M2TSENCRYPTOR )( long lTCID, const char *strTCName )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_APP_CONTEXT     *pAppContext    = NULL;

    if( g_fAppContextInited )
    {
        GET_SHARED_APP_CONTEXT( pAppContext );
        if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
        {
            DRM_KF_Close( ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext );
        }
        Drm_Uninitialize( pAppContext );
    }

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( M2TSENCRYPTOR )
    API_ENTRY( TestEncryptor_Init )
    API_ENTRY( TestEncryptor_Uninitialize )
    API_ENTRY( TestEncryptor_BinaryReader )
    API_ENTRY( TestEncryptor_BinaryReaderWithPacketList )
    API_ENTRY( TestEncryptor_ParsePacketHeader )
    API_ENTRY( TestEncryptor_GetFreePacket )
    API_ENTRY( TestEncryptor_TSPacketHeaderParser )
    API_ENTRY( TestEncryptor_RebuildPMTPacket )
    API_ENTRY( TestEncryptor_SetKeyData )
    API_ENTRY( TestEncryptor_SetEncryptAudio )
    API_ENTRY( TestEncryptor_Encrypt )
    API_ENTRY( TestEncryptor_EncryptWithMoreData )
    API_ENTRY( TestEncryptor_BuildPAT )
    API_ENTRY( TestEncryptor_VerifyPAT )
    API_ENTRY( TestEncryptor_BuildPMT )
    API_ENTRY( TestEncryptor_BuildIncompletedPMT )
    API_ENTRY( TestEncryptor_VerifyPMT )
    API_ENTRY( TestEncryptor_VerifyPMTAfterRebuilt )
    API_ENTRY( TestEncryptor_BuildMPEG2PES )
    API_ENTRY( TestEncryptor_BuildH264PES )
    API_ENTRY( TestEncryptor_BuildAudioPES )
    API_ENTRY( TestEncryptor_ParseECM )
    API_ENTRY( TestEncryptor_VerifyECM )
    API_ENTRY( TestEncryptor_InjectDataIntoPacketsBuffer )
    API_ENTRY( TestEncryptor_ClearPacketsBuffer )
    API_ENTRY( TestEncryptor_ClearEncryptedBuffer )
    API_ENTRY( TestEncryptor_InjectError )
    API_ENTRY( TestEncryptor_SetLoggingCallback )
    API_ENTRY( TestEncryptor_CheckLoggedResults )
    API_ENTRY( TestEncryptor_DropOveflowPayload )
    API_ENTRY( TestEncryptor_KeepOveflowPayload )
    API_ENTRY( TestEncryptor_CheckECMPID )
    API_ENTRY( TestEncryptor_InsertRandomBytes )
    API_ENTRY( TestPRNDCRLTimeOffset )
    API_ENTRY( TestManagerStoreRevocationPackage )
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;
