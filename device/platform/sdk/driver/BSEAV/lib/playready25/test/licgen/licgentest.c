/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmprnd.h>
#include <drmlicgen.h>
#include <drmkeyfile.h>
#include <tstkeyfile.h>

#include <licgentest.h>
#include <prndtoplevelapitest.h>
#include <prndprotocoltest.h>
#include <DRMManagerTestAPI.h>
#include <drmmathsafe.h>
#include <oemhaltest.h>
#include <drmdeviceassets.h>


ENTER_PKTEST_NAMESPACE_CODE;

#define TEST_VAR_PRND_RXCTX               "ReceiverContext"
#define TEST_VAR_PRND_TXCTX               "TransmitterContext"
#define TEST_VAR_ENCRYPTED_DATA           "EncryptedData"
#define TEST_VAR_ENCRYPTED_LENGTH         "EncryptedLength"
#define TEST_VAR_ENCRYPTED_POINTER        "EncryptedPointer"

#define TEST_LICGEN_SAMPLESIZE             2
#define TEST_LICGEN_FIRSTSAMPLE            15
#define TEST_LICGEN_SAMPLE0                0
#define TEST_LICGEN_SAMPLE1                1

static const DRM_DWORD g_dwLicenseExpireTime             = 10; /* 10 seconds */

static const DRM_KID   g_keyID                           = { 0xa3, 0x22, 0xd4, 0xbe, 0x1d, 0xf7, 0x44, 0xed, 0xb5, 0x5a, 0xf2, 0xe0, 0xa8, 0x17, 0x5f, 0xb3 };
static const DRM_KID   g_kidRoot                         = { 0x10, 0x4f, 0x0d, 0x2e, 0xf7, 0xc1, 0x44, 0x1f, 0x84, 0xd4, 0x8b, 0x48, 0xa0, 0xa0, 0xf7, 0x73 };
static const DRM_BYTE  g_rgbClearData[]                  = "Hello World!!!, I am encrypted!!!";

static const DRM_WORD  g_wOPLCompressedDigitalVideo      = 100;
static const DRM_WORD  g_wOPLUncompressedDigitalVideo    = 101;
static const DRM_WORD  g_wOPLAnalogVideo                 = 102;
static const DRM_WORD  g_wOPLCompressedDigitalAudio      = 103;
static const DRM_WORD  g_wOPLUncompressedDigitalAudio    = 104;

static const DRM_GUID  g_guidExplicitAVOutputProtection  = { 0x9169ea19, 0xb3ce, 0x49a0, { 0x9a, 0x26, 0xb3, 0x7a, 0xc3, 0x7b, 0x55, 0xa8 } };
static const DRM_GUID  g_guidExplicitDAOutputProtection  = { 0x6036baa2, 0x440b, 0x4946, { 0xb6, 0x2c, 0x73, 0x4a, 0xdc, 0x74, 0xcd, 0xcf } };
static const DRM_DWORD g_dwExplicitAVExtension           = 1;
static const DRM_DWORD g_dwExplicitDAExtension           = 2;

static const DRM_GUID  g_guidPlayEnabler                 = { 0x67732466, 0xf481, 0x4291, { 0x97, 0xc, 0xac, 0x79, 0xc8, 0xab, 0x1e, 0x18 } };

static const DRM_DWORD g_dwSourceID                      = 23;

DRM_RESULT DRM_CALL MODULE_PRETESTCASE(licgentest)(long lTCID, const char *strTCName)
{
    DRM_RESULT                       dr                  = DRM_SUCCESS;
    DRM_PRND_RECEIVER_CONTEXT       *pRxCtx              = NULL;
    DRM_PRND_TRANSMITTER_CONTEXT    *pTxCtx              = NULL;
    DRM_APP_CONTEXT                 *pAppContext         = NULL;
    DRM_BYTE                        *pbEncryptedData     = NULL;
    DRM_DWORD                       *pdwEncryptedLength  = NULL;
    DRM_BYTE                       **ppbEncryptedPointer = NULL;
    DRM_BYTE                        *pbNonce             = NULL;

    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pRxCtx, TEST_VAR_PRND_RXCTX, SIZEOF( DRM_PRND_RECEIVER_CONTEXT ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pTxCtx, TEST_VAR_PRND_TXCTX, SIZEOF( DRM_PRND_TRANSMITTER_CONTEXT ) );

    /* Initialize the encrypted sample */
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbEncryptedData, TEST_VAR_ENCRYPTED_DATA, SIZEOF( g_rgbClearData ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pdwEncryptedLength, TEST_VAR_ENCRYPTED_LENGTH, TEST_LICGEN_SAMPLESIZE * SIZEOF( DRM_DWORD ) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( ppbEncryptedPointer, TEST_VAR_ENCRYPTED_POINTER, TEST_LICGEN_SAMPLESIZE * SIZEOF( DRM_BYTE *) );
    MAP_TO_GLOBAL_TEST_VAR_Alloc_And_Zero( pbNonce, TEST_VAR_PROXIMITY_NONCE , SIZEOF( DRM_ID ) );

    OEM_SECURE_MEMCPY( pbEncryptedData, g_rgbClearData, SIZEOF( g_rgbClearData ) );
    pdwEncryptedLength[ TEST_LICGEN_SAMPLE0 ]  = TEST_LICGEN_FIRSTSAMPLE;
    pdwEncryptedLength[ TEST_LICGEN_SAMPLE1 ]  = SIZEOF( g_rgbClearData ) - TEST_LICGEN_FIRSTSAMPLE;
    ppbEncryptedPointer[ TEST_LICGEN_SAMPLE0 ] = pbEncryptedData;
    ppbEncryptedPointer[ TEST_LICGEN_SAMPLE1 ] = pbEncryptedData + TEST_LICGEN_FIRSTSAMPLE;

    /* Set secure clock */
    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Oem_Clock_SetResetState( NULL, FALSE ) );
    ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->fClockSet = TRUE;

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        DRM_APP_CONTEXT_INTERNAL *pAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *) pAppContext;

        ChkDR( _CopyFileToDRMFolder( KEYFILE_FILE_NAME, KEYFILE_FILE_NAME ) );
        ChkMem( pAppContextInternal->pKeyFileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );
        ChkDR( DRM_KF_Initialize( NULL, pAppContextInternal->pKeyFileContext ) );
        pAppContextInternal->oBlackBoxContext.pKeyFileContext = pAppContextInternal->pKeyFileContext;
        ChkDR( TST_UTL_SetPlayReadyCertCachedValues( &pAppContextInternal->oBlackBoxContext ) );
    }
ErrorExit:
    return dr;
}

static DRM_RESULT _CleanupTxCtx()
{
    DRM_RESULT                       dr             = DRM_SUCCESS;
    DRM_PRND_TRANSMITTER_CONTEXT    *pTxCtx         = NULL;
    DRM_APP_CONTEXT                 *pAppContext    = NULL;

    GET_SHARED_APP_CONTEXT_NOFAIL( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pTxCtx, TEST_VAR_PRND_TXCTX );

    {
        /*
        ** If the context is a member of pAppContext, then Drm_Uninitialize will EndSession and we shouldn't have to.
        ** If skipping EndSession causes us to leak, we found a bug!
        */
        const DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( const DRM_APP_CONTEXT_INTERNAL * )pAppContext;
        if( pAppContext == NULL || poAppContextInternal->poPrndTransmitterContext != pTxCtx )
        {
            ChkDR( DRM_PRND_Transmitter_EndSession( pTxCtx ) );
        }
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _CleanupRxCtx()
{
    DRM_RESULT                   dr         = DRM_SUCCESS;
    DRM_PRND_RECEIVER_CONTEXT   *pRxCtx     = NULL;
    DRM_APP_CONTEXT             *pAppContext    = NULL;

    GET_SHARED_APP_CONTEXT_NOFAIL( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pRxCtx, TEST_VAR_PRND_RXCTX );

    {
        /*
        ** If the context is a member of pAppContext, then Drm_Uninitialize will EndSession and we shouldn't have to.
        ** If skipping EndSession causes us to leak, we found a bug!
        */
        const DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( const DRM_APP_CONTEXT_INTERNAL * )pAppContext;
        if( pAppContext == NULL || poAppContextInternal->poPrndReceiverContext != pRxCtx )
        {
            ChkDR( DRM_PRND_Receiver_EndSession( pRxCtx ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(licgentest)(long lTCID, const char *strTCName)
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    if( DRM_PRND_IsPRNDTransmitterSupported() )
    {
        POSTTESTCASE_FUNCTION_CALL( _CleanupTxCtx() )
    }

    if( DRM_PRND_IsPRNDReceiverSupported() )
    {
        POSTTESTCASE_FUNCTION_CALL( _CleanupRxCtx() );
    }

    return dr;
}

static DRM_RESULT DRM_CALL _FindLicenseKeyCache(
    __in                          const DRM_APP_CONTEXT              *f_poAppContext,
    __in                          const DRM_KID                      *f_pKid,
    __out                               DRM_LOCAL_LICENSE_KEY_CACHE **f_ppoLicenseKeyCache )
{
    DRM_RESULT                   dr                     = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_KEY_CACHE *poLicenseKeyCache      = NULL;

    ChkArg( f_poAppContext       != NULL );
    ChkArg( f_pKid               != NULL );
    ChkArg( f_ppoLicenseKeyCache != NULL );

    ChkArg( ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext != NULL );

    *f_ppoLicenseKeyCache = NULL;
    poLicenseKeyCache = ((DRM_APP_CONTEXT_INTERNAL*)f_poAppContext)->poLocalLicenseContext->poLicenseKeyCache;

    while( poLicenseKeyCache != NULL )
    {
        if( OEM_SECURE_ARE_EQUAL( f_pKid, &poLicenseKeyCache->kid, SIZEOF( DRM_KID ) ) )
        {
            break;
        }

        poLicenseKeyCache = poLicenseKeyCache->poNextLicenseCache;
    }

    *f_ppoLicenseKeyCache = poLicenseKeyCache;

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _GenerateLicense(
    __inout              DRM_APP_CONTEXT                        *f_poAppContext,
    __in           const DRM_BOOL                                f_fPersist,
    __in           const DRM_LOCAL_LICENSE_TYPE                  f_eLicenseType,
    __in           const DRM_KID                                *f_pKid,
    __inout              DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR    *f_poLicensePolicyDecriptor,
    __in           const DRM_LICENSE_HANDLE                      f_pRootLinceHandle,
    __out                DRM_LICENSE_HANDLE                     *f_pLicenseHandle )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext             != NULL                       );
    ChkArg( f_poLicensePolicyDecriptor != NULL                       );
    ChkArg( f_pLicenseHandle           != DRM_LICENSE_HANDLE_INVALID );

    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( f_poLicensePolicyDecriptor ) );

    f_poLicensePolicyDecriptor->wSecurityLevel   = 150;
    f_poLicensePolicyDecriptor->fCannotPersist   = f_fPersist;

    ChkDR( Drm_LocalLicense_CreateLicense(
        f_poAppContext,
        f_poLicensePolicyDecriptor,
        f_eLicenseType,
        f_pKid,
        0,
        NULL,
        f_pRootLinceHandle,
        f_pLicenseHandle ) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _OPLCallback(
    __in const DRM_VOID                 *f_pvOutputLevelsData,
    __in       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in const DRM_VOID                 *f_pv )
{
    DRM_RESULT        dr                = DRM_SUCCESS;

    if( f_dwCallbackType == DRM_PLAY_OPL_CALLBACK )
    {
        DRM_PLAY_OPL_EX2 *pOutputLevels = (DRM_PLAY_OPL_EX2*)f_pvOutputLevelsData;
        ChkArg( pOutputLevels != NULL );

        if( pOutputLevels->minOPL.wCompressedDigitalVideo != g_wOPLCompressedDigitalVideo )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( pOutputLevels->minOPL.wUncompressedDigitalVideo != g_wOPLUncompressedDigitalVideo )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( pOutputLevels->minOPL.wAnalogVideo != g_wOPLAnalogVideo )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( pOutputLevels->minOPL.wCompressedDigitalAudio != g_wOPLCompressedDigitalAudio )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( pOutputLevels->minOPL.wUncompressedDigitalAudio != g_wOPLUncompressedDigitalAudio )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( pOutputLevels->vopi.cEntries != 1 )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( MEMCMP( &(pOutputLevels->vopi.rgVop[0].guidId), &g_guidExplicitAVOutputProtection, SIZEOF( DRM_GUID ) ) == 0 )
        {
            if( MEMCMP( &(pOutputLevels->vopi.rgVop[0].dwConfigData), &g_dwExplicitAVExtension, SIZEOF( DRM_DWORD ) ) != 0 )
            {
                ChkDR( DRM_E_TEST_OPL_MISMATCH );
            }
        }
        else
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( pOutputLevels->aopi.cEntries != 1 )
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }

        if( MEMCMP( &(pOutputLevels->aopi.rgAop[0].guidId), &g_guidExplicitDAOutputProtection, SIZEOF( DRM_GUID ) ) == 0 )
        {
            if( MEMCMP( &(pOutputLevels->aopi.rgAop[0].dwConfigData), &g_dwExplicitDAExtension, SIZEOF( DRM_DWORD ) ) != 0 )
            {
                ChkDR( DRM_E_TEST_OPL_MISMATCH );
            }
        }
        else
        {
            ChkDR( DRM_E_TEST_OPL_MISMATCH );
        }
    }
    else if( f_dwCallbackType == DRM_COPY_OPL_CALLBACK )
    {
        ChkDR( DRM_E_TEST_INVALID_OPL_CALLBACK );
    }
    else if( f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK )
    {
        /* do nothing */
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK )
    {
        /* do nothing */
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK )
    {
        const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT *pExtCallback = (const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT*)f_pvOutputLevelsData;

        if(    pExtCallback->pRestriction->wType  == XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT
            && pExtCallback->pRestriction->cbData == SIZEOF( DRM_GUID ) )
        {
            if( MEMCMP( pExtCallback->pRestriction->pbBuffer + pExtCallback->pRestriction->ibData,
                        &g_guidPlayEnabler,
                        SIZEOF( DRM_GUID ) ) != 0 )
            {
                ChkDR( DRM_E_TEST_DATA_VERIFICATION_FAILURE );
            }
        }
    }
    else if( f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK )
    {
        /* do nothing */
    }
    else if ( f_dwCallbackType == DRM_RESTRICTED_SOURCEID_CALLBACK )
    {
        const DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT *pCallback = (const DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT*)f_pvOutputLevelsData;
        ChkArg( pCallback != NULL );

        if( g_dwSourceID != pCallback->dwSourceID )
        {
            TRACE(("SourceID reported: %u", pCallback->dwSourceID));
            TRACE(("SourceID Expected: %u", g_dwSourceID));
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
    else
    {
        TRACE( ( "Unknown Callback type: %u", f_dwCallbackType ) );
        ChkDR( DRM_E_TEST_INVALID_OPL_CALLBACK );
    }

ErrorExit:
    return dr;
}

#define TestLicGenCreateLicense_Arg0_FLAGS_CannotPersist             0x00000001
#define TestLicGenCreateLicense_Arg0_FLAGS_Expires                   0x00000002
#define TestLicGenCreateLicense_Arg0_FLAGS_ExpireAfterFirstPlay      0x00000004
#define TestLicGenCreateLicense_Arg0_FLAGS_OutputProtection          0x00000008
#define TestLicGenCreateLicense_Arg0_FLAGS_SourceID                  0x00000010
#define TestLicGenCreateLicense_Arg0_FLAGS_PlayEnabler               0x00000020
#define TestLicGenCreateLicense_Arg0_FLAGS_RealTimeExpiration        0x00000040

DRM_RESULT DRM_CALL TestLicGenCreateLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                                   dr                                                                             = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR          oLicensePolicyDecriptor                                                        = { 0 };
    DRM_APP_CONTEXT                             *pAppContext                                                                    = NULL;

    DRM_LICENSE_HANDLE                           licenseHandle                                                                  = DRM_LICENSE_HANDLE_INVALID;
    DRM_BOOL                                     fCannotPersist                                                                 = FALSE;
    DRM_BOOL                                     fExpires                                                                       = FALSE;
    DRM_BOOL                                     fExpireAfterFirstPlay                                                          = FALSE;
    DRM_BOOL                                     fOutputProtection                                                              = FALSE;
    DRM_BOOL                                     fSourceID                                                                      = FALSE;
    DRM_BOOL                                     fPlayEnabler                                                                   = FALSE;
    DRM_BOOL                                     fRealTimeExpiration                                                            = FALSE;
    DRM_DECRYPT_CONTEXT                          oEncryptCtx                                                                    = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT                 oAesCtrCtx                                                                     = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT                 oAesCtrCtxDecrypt                                                              = { 0 };

    const DRM_CONST_STRING                      *pdstrPlay                                                                      = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DWORD                                    cchKid                                                                         = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    DRM_WCHAR                                    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ]                         = { 0 };

    DRM_BYTE                                     rgbClearData[DRM_AES_BLOCKLEN]                                                 = "Hello, world!";
    DRM_BYTE                                     rgbEncryptedData[DRM_AES_BLOCKLEN]                                             = { 0 };

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fCannotPersist        = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_CannotPersist );
        fExpires              = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_Expires );
        fExpireAfterFirstPlay = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_ExpireAfterFirstPlay );
        fOutputProtection     = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_OutputProtection );
        fSourceID             = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_SourceID );
        fPlayEnabler          = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_PlayEnabler );
        fRealTimeExpiration   = 0 != ( dwArg0Flags & TestLicGenCreateLicense_Arg0_FLAGS_RealTimeExpiration );
    }

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

    oLicensePolicyDecriptor.wSecurityLevel = 150;
    oLicensePolicyDecriptor.fCannotPersist = fCannotPersist;

    oLicensePolicyDecriptor.oExpiration.fValid      = TRUE;
    oLicensePolicyDecriptor.oExpiration.dwBeginDate = DRM_LOCAL_LICENSE_EXPIRATION_MIN_BEGIN_DATE;

    if( fExpires )
    {
        /* calculate absolute time */

        DRMFILETIME ftCurrent     = { 0 };
        DRM_DWORD   dwCurrentTime = 0;

        Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrent );
        FILE_TIME_TO_DWORD( ftCurrent, dwCurrentTime );

        ChkDR( DRM_DWordAdd( dwCurrentTime, g_dwLicenseExpireTime, &oLicensePolicyDecriptor.oExpiration.dwEndDate ) );
    }
    else
    {

        oLicensePolicyDecriptor.oExpiration.dwEndDate = DRM_LOCAL_LICENSE_INFINITE_EXPIRATION;
    }

    if( fExpireAfterFirstPlay )
    {
        oLicensePolicyDecriptor.oExpirationAfterFirstPlay.fValid = TRUE;
        oLicensePolicyDecriptor.oExpirationAfterFirstPlay.dwValue = g_dwLicenseExpireTime;
    }

    if( fRealTimeExpiration )
    {
        ChkArg( fExpires || fExpireAfterFirstPlay );
        oLicensePolicyDecriptor.fRealTimeExpiration = TRUE;
    }

    if( fOutputProtection )
    {
        /* Set output level */
        oLicensePolicyDecriptor.wCompressedDigitalVideo   = g_wOPLCompressedDigitalVideo;
        oLicensePolicyDecriptor.wUncompressedDigitalVideo = g_wOPLUncompressedDigitalVideo;
        oLicensePolicyDecriptor.wAnalogVideo              = g_wOPLAnalogVideo;
        oLicensePolicyDecriptor.wCompressedDigitalAudio   = g_wOPLCompressedDigitalAudio;
        oLicensePolicyDecriptor.wUncompressedDigitalAudio = g_wOPLUncompressedDigitalAudio;

        /* Initialize analog video protection */
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoExplicitAnalogVideoOutputProtections[ 0 ].oProtectionId, &g_guidExplicitAVOutputProtection, SIZEOF( DRM_GUID ) );
        OEM_SECURE_MEMCPY( oLicensePolicyDecriptor.rgoExplicitAnalogVideoOutputProtections[ 0 ].rgbConfigData, &g_dwExplicitAVExtension, SIZEOF( DRM_DWORD ) );
        oLicensePolicyDecriptor.rgoExplicitAnalogVideoOutputProtections[ 0 ].cbConfigData = SIZEOF( DRM_DWORD );
        oLicensePolicyDecriptor.cExplicitAnalogVideoOutputProtections = 1;

        /* Initialize digital video protection */
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoExplicitDigitalAudioOutputProtections[ 0 ].oProtectionId, &g_guidExplicitDAOutputProtection, SIZEOF( DRM_GUID ) );
        OEM_SECURE_MEMCPY( oLicensePolicyDecriptor.rgoExplicitDigitalAudioOutputProtections[ 0 ].rgbConfigData, &g_dwExplicitDAExtension, SIZEOF( DRM_DWORD ) );
        oLicensePolicyDecriptor.rgoExplicitDigitalAudioOutputProtections[ 0 ].cbConfigData = SIZEOF( DRM_DWORD );
        oLicensePolicyDecriptor.cExplicitDigitalAudioOoutputProtections = 1;
    }

    if( fSourceID )
    {
        oLicensePolicyDecriptor.oSourceId.fValid              = TRUE;
        oLicensePolicyDecriptor.oSourceId.dwSourceId          = g_dwSourceID;
        oLicensePolicyDecriptor.oSourceId.fRestrictedSourceId = TRUE;
    }

    if( fPlayEnabler )
    {
        OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[0], &g_guidPlayEnabler, SIZEOF( DRM_GUID ) );
        oLicensePolicyDecriptor.cPlayEnablers = 1;
    }

    ChkDR( Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle ) );

    /* store license in XMR instead of PRND because PRND store ignore expire after first play */
    dr = Drm_LocalLicense_StoreLicense(
         licenseHandle,
         eDRM_LOCAL_LICENSE_XMR_STORE );

    if( fCannotPersist )
    {
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_CANNOT_PERSIST_LICENSE );
        goto ErrorExit;
    }
    else
    {
        ChkDR( dr );
    }

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        goto ErrorExit;
    }

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&g_keyID,
        SIZEOF( g_keyID ),
        rgwchKid,
        &cchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ] = g_wchNull;

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_KID,
        (const DRM_BYTE*)rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );

    if( !fRealTimeExpiration )
    {
        /* Wait for g_dwLicenseExpireTime + 1 second for the license to expire */
        ChkDR( tChangeSystemTime( g_dwLicenseExpireTime + 1 ) );
    }

    dr = Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,              /* Passing only 1 right, play */
        _OPLCallback,
        NULL,
       &oEncryptCtx );

    if( fExpires && !fRealTimeExpiration )
    {
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICENSE_EXPIRED );
        goto ErrorExit;
    }
    else
    {
        ChkDR( dr );
    }

    OEM_SECURE_MEMCPY( rgbEncryptedData, rgbClearData, SIZEOF( rgbEncryptedData ) );

    ChkDR( Oem_Random_GetBytes(
         ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pOEMContext,
         (DRM_BYTE *)&oAesCtrCtx.qwInitializationVector,
         SIZEOF( oAesCtrCtx.qwInitializationVector ) ) );
    OEM_SECURE_MEMCPY( &oAesCtrCtxDecrypt, &oAesCtrCtx, SIZEOF( oAesCtrCtxDecrypt ) );

    ChkDR( Drm_Reader_Decrypt(
        &oEncryptCtx,
        &oAesCtrCtx,
         rgbEncryptedData,
         SIZEOF( rgbEncryptedData ) ) );

    ChkDR( Drm_Reader_Decrypt(
        &oEncryptCtx,
        &oAesCtrCtxDecrypt,
        rgbEncryptedData,
        SIZEOF( rgbEncryptedData ) ) );

    ChkBOOL( 0 == DRMCRT_memcmp( rgbClearData, rgbEncryptedData, SIZEOF( rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    if( fExpires && fRealTimeExpiration )
    {
        const DRM_DWORD cb = DRM_REAL_TIME_EXPIRATION_CHECK_BYTE_COUNT;
        DRM_BYTE *pb = NULL;

        /* Wait for g_dwLicenseExpireTime + 1 second for the license to expire */
        ChkDR( tChangeSystemTime( g_dwLicenseExpireTime + 1 ) );

        /* Even though we're past real-time expiration, we haven't yet decrypted enough bytes to reach the threshold here */
        ChkDR( Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtxDecrypt,
            rgbEncryptedData,
            SIZEOF( rgbEncryptedData ) ) );

        ChkMem( pb = (DRM_BYTE*)Oem_MemAlloc( cb ) );  /* Allocate DRM_REAL_TIME_EXPIRATION_CHECK_BYTE_COUNT of arbitrary data to reach the threshold */
        dr = Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtxDecrypt,
            pb,
            cb );
        Oem_MemFree( pb );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICENSE_REALTIME_EXPIRED );

        /* Make sure the decrypt ctx self-destructed */
        dr = Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtxDecrypt,
            rgbEncryptedData,
            SIZEOF( rgbEncryptedData ) );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_CIPHER_NOT_INITIALIZED );

        goto ErrorExit;
    }

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

    if( !fRealTimeExpiration )
    {
        /* Wait for g_dwLicenseExpireTime + 1 second for the license to expire after first play */
        ChkDR( tChangeSystemTime( g_dwLicenseExpireTime + 1 ) );
    }

    Drm_Reader_Close( &oEncryptCtx );

    dr = Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,              /* Passing only 1 right, play */
        _OPLCallback,
        NULL,
       &oEncryptCtx );

    if( fExpireAfterFirstPlay && !fRealTimeExpiration )
    {
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICENSE_EXPIRED );
        goto ErrorExit;
    }
    else
    {
        ChkDR( dr );
    }

    if( fExpireAfterFirstPlay && fRealTimeExpiration )
    {
        const DRM_DWORD cb = DRM_REAL_TIME_EXPIRATION_CHECK_BYTE_COUNT;
        DRM_BYTE *pb = NULL;

        /* Wait for g_dwLicenseExpireTime + 1 second for the license to expire after first play */
        ChkDR( tChangeSystemTime(  g_dwLicenseExpireTime + 1 ) );

        /* Even though we're past real-time expiration, we haven't yet decrypted enough bytes to reach the threshold here */
        ChkDR( Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtxDecrypt,
            rgbEncryptedData,
            SIZEOF( rgbEncryptedData ) ) );

        ChkMem( pb = (DRM_BYTE*)Oem_MemAlloc( cb ) );  /* Allocate DRM_REAL_TIME_EXPIRATION_CHECK_BYTE_COUNT of arbitrary data to reach the threshold */
        dr = Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtxDecrypt,
            pb,
            cb );
        Oem_MemFree( pb );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICENSE_REALTIME_EXPIRED );

        /* Make sure the decrypt ctx self-destructed */
        dr = Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtxDecrypt,
            rgbEncryptedData,
            SIZEOF( rgbEncryptedData ) );
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_CIPHER_NOT_INITIALIZED );

        goto ErrorExit;
    }

ErrorExit:

    Drm_Reader_Close( &oEncryptCtx );

    if( licenseHandle != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &licenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    tResetSystemTimeOffset();

    return dr;
}

DRM_RESULT DRM_CALL TestLicGenCreateWDRMHeaderAndPRO( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                                   dr                         = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR          oLicensePolicyDecriptor    = { 0 };
    DRM_APP_CONTEXT                             *pAppContext                = NULL;
    DRM_LICENSE_HANDLE                           licenseHandle              = DRM_LICENSE_HANDLE_INVALID;
    DRM_DWORD                                    cbHeader                   = 0;
    DRM_BYTE                                    *pbHeader                   = NULL;
    DRM_DWORD                                    cbPROHeader                = 0;
    DRM_BYTE                                    *pbPROHeader                = NULL;
    DRM_DWORD                                    cbRecord                   = 0;
    DRM_BYTE                                    *pbRecord                   = NULL;

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );
    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );
    oLicensePolicyDecriptor.wSecurityLevel   = 150;
    oLicensePolicyDecriptor.fCannotPersist   = TRUE;

    /* Create normal header with KID */
    ChkDR( Drm_LocalLicense_CreateRMHeader( &g_keyID, &cbHeader, &pbHeader ) );
    ChkBOOL( cbHeader > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbHeader != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle ) );

    ChkDR( Drm_LocalLicense_CreatePlayReadyObject( licenseHandle, cbHeader, pbHeader, &cbPROHeader, &pbPROHeader ) );
    ChkBOOL( cbPROHeader > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbPROHeader != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Check both ELS and WRMHeader exist */
    ChkDR( DRM_PRO_GetRecord( pbPROHeader,
                              cbPROHeader,
                              PLAYREADY_WRMHEADER,
                             &pbRecord,
                             &cbRecord ) );

    ChkDR( DRM_PRO_GetRecord( pbPROHeader,
                              cbPROHeader,
                              PLAYREADY_EMBEDDED_LICENSE_STORE,
                             &pbRecord,
                             &cbRecord ) );

    DRM_PRND_SAFEMEMFREE( pbHeader );
    cbHeader = 0;
    DRM_PRND_SAFEMEMFREE( pbPROHeader );
    cbPROHeader = 0;

    /* Create OnDemand Header without KID */
    ChkDR( Drm_LocalLicense_CreateRMHeader( NULL, &cbHeader, &pbHeader ) );
    ChkBOOL( cbHeader > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbHeader != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( Drm_LocalLicense_CreatePlayReadyObject( NULL, cbHeader, pbHeader, &cbPROHeader, &pbPROHeader ) );
    ChkBOOL( cbPROHeader > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbPROHeader != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( DRM_PRO_GetRecord( pbPROHeader,
                              cbPROHeader,
                              PLAYREADY_WRMHEADER,
                             &pbRecord,
                             &cbRecord ) );

    ChkDR( DRM_PRO_GetRecord( pbPROHeader,
                              cbPROHeader,
                              PLAYREADY_EMBEDDED_LICENSE_STORE,
                             &pbRecord,
                             &cbRecord ) );

    DRM_PRND_SAFEMEMFREE( pbHeader );
    cbHeader = 0;
    DRM_PRND_SAFEMEMFREE( pbPROHeader );
    cbPROHeader = 0;

    /* Create PRO Without WRMHeader */
    ChkDR( Drm_LocalLicense_CreateRMHeader( &g_keyID, &cbHeader, &pbHeader ) );
    ChkBOOL( cbHeader > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbHeader != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkDR( Drm_LocalLicense_CreatePlayReadyObject( licenseHandle, 0, NULL, &cbPROHeader, &pbPROHeader ) );
    ChkBOOL( cbPROHeader > 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( pbPROHeader != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    dr = DRM_PRO_GetRecord( pbPROHeader,
                            cbPROHeader,
                            PLAYREADY_WRMHEADER,
                           &pbRecord,
                           &cbRecord );

    DRM_TEST_EXPECT_ERROR( dr, DRM_E_RECORD_NOT_FOUND );

    ChkDR( DRM_PRO_GetRecord( pbPROHeader,
                              cbPROHeader,
                              PLAYREADY_EMBEDDED_LICENSE_STORE,
                             &pbRecord,
                             &cbRecord ) );
ErrorExit:

    if( licenseHandle != DRM_LICENSE_HANDLE_INVALID )
    {
        Drm_LocalLicense_Release( &licenseHandle );
    }

    DRM_PRND_SAFEMEMFREE( pbHeader );
    DRM_PRND_SAFEMEMFREE( pbPROHeader );
    return dr;
}


DRM_RESULT DRM_CALL TestLicGenCreatePROAndDecrypt( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR     oLicensePolicyDecriptor                                = { 0 };
    DRM_APP_CONTEXT                        *pAppContext                                            = NULL;
    DRM_LICENSE_HANDLE                      licenseHandle                                          = DRM_LICENSE_HANDLE_INVALID;
    DRM_DWORD                               cbHeader                                               = 0;
    DRM_BYTE                               *pbHeader                                               = NULL;
    DRM_DECRYPT_CONTEXT                     oEncryptCtx                                            = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT            oAesCtrCtx                                             = { 0 };
    const DRM_CONST_STRING                 *pdstrPlay                                              = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_UINT64                              qwIV                                                   = DRM_UI64HL( 0, 0 );
    DRM_BYTE                               *pbEncryptedData                                        = NULL;
    DRM_DWORD                              *pdwEncryptedLength                                     = NULL;
    DRM_BYTE                              **ppbEncryptedPointer                                    = NULL;
    DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA     oPROwKid                                               = { 0 };
    DRM_CONST_STRING                        dstrKid                                                = EMPTY_DRM_STRING;
    DRM_WCHAR                               rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ]  = { 0 };
    DRM_DWORD                               cchKid                                                 = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, TEST_VAR_ENCRYPTED_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedLength, TEST_VAR_ENCRYPTED_LENGTH );
    MAP_TO_GLOBAL_TEST_VAR( ppbEncryptedPointer, TEST_VAR_ENCRYPTED_POINTER );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    /* cannot embed local bound license */
    ChkDR( _GenerateLicense(
        pAppContext,
        FALSE,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_keyID,
       &oLicensePolicyDecriptor,
        NULL,
       &licenseHandle ) );

    dr =  Drm_LocalLicense_CreatePlayReadyObject(
        licenseHandle,
        0,
        NULL,
       &cbHeader,
       &pbHeader );
    ChkBOOL( dr == DRM_E_LICGEN_EMBED_LOCAL_LICENSE, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_LocalLicense_Release( &licenseHandle ) );

    licenseHandle = DRM_LICENSE_HANDLE_INVALID;
    dr            = DRM_SUCCESS;

    /* create remote bound license to embed and test */
    ChkDR( _GenerateLicense(
        pAppContext,
        TRUE,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
       &oLicensePolicyDecriptor,
        NULL,
       &licenseHandle ) );

    ChkDR( Drm_LocalLicense_CreatePlayReadyObject(
        licenseHandle,
        0,
        NULL,
       &cbHeader,
       &pbHeader ) );

    ChkDR( Drm_LocalLicense_AddRef(
        licenseHandle ) );

    ChkDR( Drm_LocalLicense_Release( &licenseHandle ) );

    /* make sure license handle can still be used after release */
    DRM_PRND_SAFEMEMFREE( pbHeader );
    pbHeader = NULL;
    cbHeader = 0;
    ChkDR( Drm_LocalLicense_CreatePlayReadyObject(
        licenseHandle,
        0,
        NULL,
       &cbHeader,
       &pbHeader ) );

    ChkDR( Drm_LocalLicense_EncryptSample(
        licenseHandle,
        2,
        pdwEncryptedLength,
        ppbEncryptedPointer,
       &qwIV ) );

    dr = Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_PLAYREADY_OBJ,
        pbHeader,
        cbHeader );

    DRM_TEST_EXPECT_ERROR( dr, DRM_E_CH_INVALID_HEADER );

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&g_keyID,
        SIZEOF( DRM_KID ),
        rgwchKid,
       &cchKid,
        0 ) );

    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = g_wchNull;

    /* cchKid of DSTR_FROM_PB is in BYTES and not WCHAR */
    DSTR_FROM_PB( &dstrKid, rgwchKid, cchKid * SIZEOF( DRM_WCHAR ) );

    oPROwKid.pbKeyID = (const DRM_BYTE*)dstrKid.pwszString;
    oPROwKid.cbKeyID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR );

    oPROwKid.pbHeaderData = pbHeader;
    oPROwKid.cbHeaderData = cbHeader;

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_PLAYREADY_OBJ_WITH_KID,
        (DRM_BYTE*)&oPROwKid,
        SIZEOF( oPROwKid ) ) );

    ChkDR( Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,              /* Passing only 1 right, play */
        NULL,
        NULL,
       &oEncryptCtx ) );

    oAesCtrCtx.qwInitializationVector = qwIV;

    ChkDR( Drm_Reader_Decrypt(
        &oEncryptCtx,
        &oAesCtrCtx,
         (DRM_BYTE*)pbEncryptedData,
         SIZEOF( g_rgbClearData ) ) );

    ChkBOOL( 0 == DRMCRT_memcmp( g_rgbClearData, pbEncryptedData, SIZEOF( g_rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

    /* license handle set to null after final release */
    ChkDR( Drm_LocalLicense_Release( &licenseHandle ) );
    ChkBOOL( licenseHandle == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:

    Drm_Reader_Close( &oEncryptCtx );

    if( licenseHandle != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &licenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    DRM_PRND_SAFEMEMFREE( pbHeader );

    return dr;
}

#define TestLicGenTopLevel_Arg0_FLAGS_CannotPersist      0x00000001
#define TestLicGenTopLevel_Arg0_FLAGS_OpaqueBuf          0x00000002

DRM_RESULT DRM_CALL TestLicGenTopLevel( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                              dr                                                     = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR     oLicensePolicyDecriptor                                = { 0 };
    DRM_APP_CONTEXT                        *pAppContext                                            = NULL;

    DRM_LICENSE_HANDLE                      licenseHandle                                          = DRM_LICENSE_HANDLE_INVALID;
    DRM_BOOL                                fCannotPersist                                         = FALSE;
    DRM_BOOL                                fOpaqueBuf                                             = FALSE;

    DRM_DWORD                               cbXMRLicense                                           = 0;
    DRM_BYTE                               *pbXMRLicense                                           = NULL;
    DRM_KID                                 keyID                                                  = { 0 };

    DRM_DECRYPT_CONTEXT                     oEncryptCtx                                            = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT            oAesCtrCtx                                             = { 0 };

    const DRM_CONST_STRING                 *pdstrPlay                                              = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DWORD                               cchKid                                                 = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    DRM_WCHAR                               rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = { 0 };
    DRM_UINT64                              qwIV                                                   = DRM_UI64HL( 0, 0 );

    DRM_BYTE                               *pbEncryptedData                                        = NULL;
    DRM_DWORD                              *pdwEncryptedLength                                     = NULL;
    DRM_BYTE                              **ppbEncryptedPointer                                    = NULL;

    OEM_OPAQUE_BUFFER_HANDLE                hBufSource                                             = OEM_OPAQUE_BUFFER_HANDLE_INVALID;
    OEM_OPAQUE_BUFFER_HANDLE                hBufEncrypted                                          = OEM_OPAQUE_BUFFER_HANDLE_INVALID;
    OEM_OPAQUE_BUFFER_HANDLE                hBufDecrypted                                          = OEM_OPAQUE_BUFFER_HANDLE_INVALID;

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, TEST_VAR_ENCRYPTED_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedLength, TEST_VAR_ENCRYPTED_LENGTH );
    MAP_TO_GLOBAL_TEST_VAR( ppbEncryptedPointer, TEST_VAR_ENCRYPTED_POINTER );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    if( HASARG(0) )
    {
        DRM_DWORD   cch         = 0;
        DRM_DWORD   dwArg0Flags = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwArg0Flags ) );
        fCannotPersist = 0 != ( dwArg0Flags & TestLicGenTopLevel_Arg0_FLAGS_CannotPersist );
        fOpaqueBuf     = 0 != ( dwArg0Flags & TestLicGenTopLevel_Arg0_FLAGS_OpaqueBuf );
    }

    /* Tx: Generate a license with KID */
    ChkDR( _GenerateLicense(
        pAppContext,
        fCannotPersist,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_keyID,
       &oLicensePolicyDecriptor,
        NULL,
       &licenseHandle ) );

    ChkDR( Drm_LocalLicense_GetKID(
        licenseHandle,
       &keyID ) );

    ChkBOOL( 0 == DRMCRT_memcmp( &keyID, &g_keyID, SIZEOF( DRM_KID ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_LocalLicense_GetXMRLicense(
        licenseHandle,
       &cbXMRLicense,
       &pbXMRLicense ) );

    ChkBOOL( cbXMRLicense == ((DRM_LOCAL_LICENSE_SESSION_CONTEXT*)licenseHandle)->cbXMRLicense, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( 0 == DRMCRT_memcmp( pbXMRLicense, ((DRM_LOCAL_LICENSE_SESSION_CONTEXT*)licenseHandle)->pbXMRLicense, cbXMRLicense ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Rx: Store the license in the hds and bind license */
    dr = Drm_LocalLicense_StoreLicense(
        licenseHandle,
        eDRM_LOCAL_LICENSE_XMR_STORE );

    if( fCannotPersist )
    {
        DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_CANNOT_PERSIST_LICENSE );

        goto ErrorExit;
    }
    else
    {
        ChkDR( dr );
    }

    if( fOpaqueBuf )
    {
        ChkDR( OEM_HAL_TEST_OpaqueBufferCreateWithData( g_rgbClearData, SIZEOF(g_rgbClearData), &hBufSource ) );
        ChkDR( OEM_HAL_TEST_OpaqueBufferCreate( &hBufEncrypted ) );

        ChkDR( Drm_LocalLicense_EncryptOpaqueSample(
            licenseHandle,
            NULL,
            hBufSource,
            hBufEncrypted,
            SIZEOF(g_rgbClearData),
            &qwIV ) );
    }
    else
    {
        ChkDR( Drm_LocalLicense_EncryptSample(
            licenseHandle,
            2,
            pdwEncryptedLength,
            ppbEncryptedPointer,
           &qwIV ) );
    }

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        goto ErrorExit;
    }

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&g_keyID,
        SIZEOF( g_keyID ),
        rgwchKid,
        &cchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ] = g_wchNull;

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_KID,
        (const DRM_BYTE*)rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );

    ChkDR( Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,              /* Passing only 1 right, play */
        NULL,
        NULL,
       &oEncryptCtx ) );

    oAesCtrCtx.qwInitializationVector = qwIV;

    if( fOpaqueBuf )
    {
        DRM_BYTE rgbDecryptedData[SIZEOF(g_rgbClearData)] = {0};
        ChkDR( OEM_HAL_TEST_OpaqueBufferCreate( &hBufDecrypted ) );

        ChkDR( Drm_Reader_DecryptOpaque(
            &oEncryptCtx,
            &oAesCtrCtx,
            hBufEncrypted,
            hBufDecrypted,
            SIZEOF( g_rgbClearData ) ) );

        ChkDR( OEM_HAL_TEST_OpaqueBufferRead(
            hBufDecrypted,
            rgbDecryptedData,
            SIZEOF( rgbDecryptedData ) ) );

        *pdwEncryptedLength = SIZEOF(g_rgbClearData);
        OEM_SECURE_MEMCPY( pbEncryptedData, rgbDecryptedData, SIZEOF( rgbDecryptedData ) );
    }
    else
    {
        ChkDR( Drm_Reader_Decrypt(
            &oEncryptCtx,
            &oAesCtrCtx,
             (DRM_BYTE*)pbEncryptedData,
             SIZEOF( g_rgbClearData ) ) );
    }

    ChkBOOL( 0 == DRMCRT_memcmp( g_rgbClearData, pbEncryptedData, SIZEOF( g_rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

ErrorExit:
    Drm_Reader_Close( &oEncryptCtx );

    if( licenseHandle != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &licenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    (DRM_VOID)OEM_HAL_TEST_OpaqueBufferDestroy( &hBufSource );
    (DRM_VOID)OEM_HAL_TEST_OpaqueBufferDestroy( &hBufEncrypted );
    (DRM_VOID)OEM_HAL_TEST_OpaqueBufferDestroy( &hBufDecrypted );
    DRM_PRND_SAFEMEMFREE( pbXMRLicense );
    return dr;
}

DRM_RESULT DRM_CALL TestLicGenRootLeafLicense( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                              dr                                                     = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR     oLicensePolicyDecriptor                                = { 0 };
    DRM_APP_CONTEXT                        *pAppContext                                            = NULL;

    DRM_LICENSE_HANDLE                      rootLicenseHandle                                      = DRM_LICENSE_HANDLE_INVALID;
    DRM_LICENSE_HANDLE                      leafLicenseHandle                                      = DRM_LICENSE_HANDLE_INVALID;

    DRM_DECRYPT_CONTEXT                     oEncryptCtx                                            = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT            oAesCtrCtx                                             = { 0 };

    const DRM_CONST_STRING                 *pdstrPlay                                              = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DWORD                               cchKid                                                 = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    DRM_WCHAR                               rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = { 0 };

    DRM_BYTE                               *pbEncryptedData                                        = NULL;
    DRM_DWORD                              *pdwEncryptedLength                                     = NULL;
    DRM_BYTE                              **ppbEncryptedPointer                                    = NULL;

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, TEST_VAR_ENCRYPTED_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedLength, TEST_VAR_ENCRYPTED_LENGTH );
    MAP_TO_GLOBAL_TEST_VAR( ppbEncryptedPointer, TEST_VAR_ENCRYPTED_POINTER );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    /* Tx: Generate a license with KID */
    ChkDR( _GenerateLicense(
        pAppContext,
        FALSE,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT,
       &g_kidRoot,
       &oLicensePolicyDecriptor,
        NULL,
       &rootLicenseHandle ) );

    ChkDR( _GenerateLicense(
        pAppContext,
        FALSE,
        eDRM_LOCAL_LICENSE_LEAF,
       &g_keyID,
       &oLicensePolicyDecriptor,
        rootLicenseHandle,
       &leafLicenseHandle ) );

    ChkDR( Drm_LocalLicense_EncryptSample(
        leafLicenseHandle,
        2,
        pdwEncryptedLength,
        ppbEncryptedPointer,
       &oAesCtrCtx.qwInitializationVector ) );

    /* Rx: Store the license in the hds and bind license */
    ChkDR( Drm_LocalLicense_StoreLicense(
        leafLicenseHandle,
        eDRM_LOCAL_LICENSE_XMR_STORE ) );

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        goto ErrorExit;
    }

    ChkDR( DRM_B64_EncodeW(
        (const DRM_BYTE*)&g_keyID,
        SIZEOF( g_keyID ),
        rgwchKid,
        &cchKid,
        0 ) );
    rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ] = g_wchNull;

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_KID,
        (const DRM_BYTE*)rgwchKid,
        SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );

    dr = Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,               /* Passing only 1 right, play */
        NULL,
        NULL,
       &oEncryptCtx );

    /* We can't bind because there's no root license */
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_UPLINK_LICENSE_NOT_FOUND );

    ChkDR( Drm_LocalLicense_StoreLicense(
        rootLicenseHandle,
        eDRM_LOCAL_LICENSE_XMR_STORE ) );

    ChkDR( Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,               /* Passing only 1 right, play */
        NULL,
        NULL,
       &oEncryptCtx ) );

    ChkDR( Drm_Reader_Decrypt(
        &oEncryptCtx,
        &oAesCtrCtx,
         pbEncryptedData,
         SIZEOF( g_rgbClearData ) ) );

    ChkBOOL( 0 == DRMCRT_memcmp( g_rgbClearData, pbEncryptedData, SIZEOF( g_rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

ErrorExit:

    Drm_Reader_Close( &oEncryptCtx );

    if( leafLicenseHandle != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &leafLicenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    if( rootLicenseHandle != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &rootLicenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    return dr;
}


DRM_RESULT DRM_CALL TestLicGenCache( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR    oLicensePolicyDecriptor                                 = { 0 };
    DRM_APP_CONTEXT                       *pAppContext                                             = NULL;
    DRM_LICENSE_HANDLE                     licenseHandle1                                          = DRM_LICENSE_HANDLE_INVALID;
    DRM_LICENSE_HANDLE                     licenseHandle2                                          = DRM_LICENSE_HANDLE_INVALID;
    DRM_LOCAL_LICENSE_KEY_CACHE           *poLicenseCache                                          = NULL;

    DRM_BYTE                              *pbEncryptedData                                         = NULL;
    DRM_DWORD                             *pdwEncryptedLength                                      = NULL;
    DRM_BYTE                             **ppbEncryptedPointer                                     = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT           oAesCtrCtx                                              = { 0 };
    DRM_DECRYPT_CONTEXT                    oEncryptCtx                                             = { 0 };
    const DRM_CONST_STRING                *pdstrPlay                                               = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DWORD                              cchKid                                                  = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    DRM_WCHAR                              rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ]  = { 0 };

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, TEST_VAR_ENCRYPTED_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedLength, TEST_VAR_ENCRYPTED_LENGTH );
    MAP_TO_GLOBAL_TEST_VAR( ppbEncryptedPointer, TEST_VAR_ENCRYPTED_POINTER );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    /* create two license and make sure they are cached */
    ChkDR( _GenerateLicense(
        pAppContext,
        FALSE,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_keyID,
       &oLicensePolicyDecriptor,
        NULL,
       &licenseHandle1 ) );

    ChkDR( _FindLicenseKeyCache(
        pAppContext,
       &g_keyID,
       &poLicenseCache ) );
    ChkBOOL( poLicenseCache != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( _GenerateLicense(
        pAppContext,
        FALSE,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_kidRoot,
       &oLicensePolicyDecriptor,
        NULL,
       &licenseHandle2 ) );

    ChkDR( _FindLicenseKeyCache(
        pAppContext,
       &g_kidRoot,
       &poLicenseCache ) );
    ChkBOOL( poLicenseCache != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* encrypt a content and close the license */
    ChkDR( Drm_LocalLicense_EncryptSample(
        licenseHandle1,
        2,
        pdwEncryptedLength,
        ppbEncryptedPointer,
       &oAesCtrCtx.qwInitializationVector ) );

    ChkDR( Drm_LocalLicense_Release( &licenseHandle1 ) );
    licenseHandle1 = DRM_LICENSE_HANDLE_INVALID;

    /* license should still be cached after close handle */
    ChkDR( _FindLicenseKeyCache(
        pAppContext,
       &g_keyID,
       &poLicenseCache ) );

    ChkBOOL( poLicenseCache != NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* create a new license and make sure it can decrypt the content that's
    ** encrypted using the old license
    */
    ChkDR( _GenerateLicense(
        pAppContext,
        FALSE,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_keyID,
       &oLicensePolicyDecriptor,
        NULL,
       &licenseHandle1 ) );

    ChkDR( Drm_LocalLicense_StoreLicense(
        licenseHandle1,
        eDRM_LOCAL_LICENSE_XMR_STORE ) );

    if( DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( DRM_B64_EncodeW(
            (const DRM_BYTE*)&g_keyID,
            SIZEOF( g_keyID ),
            rgwchKid,
            &cchKid,
            0 ) );
        rgwchKid[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) ] = g_wchNull;

        ChkDR( Drm_Content_SetProperty(
            pAppContext,
            DRM_CSP_KID,
            (const DRM_BYTE*)rgwchKid,
            SIZEOF( rgwchKid ) - SIZEOF( DRM_WCHAR ) ) );

        ChkDR( Drm_Reader_Bind(
            pAppContext,
           &pdstrPlay,
            1,               /* Passing only 1 right, play */
            NULL,
            NULL,
           &oEncryptCtx ) );

        ChkDR( Drm_Reader_Decrypt(
           &oEncryptCtx,
           &oAesCtrCtx,
            pbEncryptedData,
            SIZEOF( g_rgbClearData ) ) );

        ChkBOOL( 0 == DRMCRT_memcmp( g_rgbClearData, pbEncryptedData, SIZEOF( g_rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkDR( Drm_Reader_Commit(
            pAppContext,
            NULL,
            NULL ) );
    }

/* fast forward in time */
    ChkDR( tChangeSystemTime( DRM_MAX_LOCAL_LICENSE_CACHE_TIME_IN_SECONDS + 1) );

    ChkDR( Drm_LocalLicense_Release( &licenseHandle1 ) );
    licenseHandle1 = DRM_LICENSE_HANDLE_INVALID;

    /* license2 shouldn't be cached anymore */
    ChkDR( _FindLicenseKeyCache(
        pAppContext,
       &g_kidRoot,
       &poLicenseCache ) );
    ChkBOOL( poLicenseCache == NULL, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    Drm_Reader_Close( &oEncryptCtx );

    if( licenseHandle1 != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &licenseHandle1 );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }
    if( licenseHandle2 != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &licenseHandle2 );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    tResetSystemTimeOffset();
    return dr;
}

DRM_RESULT DRM_CALL TestLicGenCreateLicenseWithRemoteCert( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                              dr                          = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR     oLicensePolicyDecriptor     = { 0 };
    DRM_APP_CONTEXT                        *pAppContext                 = NULL;
    DRM_LICENSE_HANDLE                      licenseHandle               = DRM_LICENSE_HANDLE_INVALID;
    DRM_DECRYPT_CONTEXT                     oEncryptCtx                 = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT            oAesCtrCtx                  = { 0 };

    const DRM_CONST_STRING                 *pdstrPlay                   = &g_dstrWMDRM_RIGHT_PLAYBACK;
    DRM_DWORD                               cbHeader                    = 0;
    DRM_BYTE                               *pbHeader                    = NULL;
    DRM_DWORD                               cbRMHeader                  = 0;
    DRM_BYTE                               *pbRMHeader                  = NULL;
    DRM_UINT64                              qwIV                        = DRM_UI64HL( 0, 0 );

    DRM_DWORD                               cbRemoteCert                = 0;
    DRM_BYTE                               *pbRemoteCert                = NULL;
    DRM_BYTE                               *pbEncryptedData             = NULL;
    DRM_DWORD                              *pdwEncryptedLength          = NULL;
    DRM_BYTE                              **ppbEncryptedPointer         = NULL;
    DRM_BOOL                                fCloseKeyFile               = FALSE;

    GET_SHARED_APP_CONTEXT( pAppContext );
    MAP_TO_GLOBAL_TEST_VAR( pbEncryptedData, TEST_VAR_ENCRYPTED_DATA );
    MAP_TO_GLOBAL_TEST_VAR( pdwEncryptedLength, TEST_VAR_ENCRYPTED_LENGTH );
    MAP_TO_GLOBAL_TEST_VAR( ppbEncryptedPointer, TEST_VAR_ENCRYPTED_POINTER );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        DRM_APP_CONTEXT_INTERNAL *pAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *) pAppContext;

        pAppContextInternal->oBlackBoxContext.pKeyFileContext = pAppContextInternal->pKeyFileContext;
        ChkDR( TST_KF_Open( pAppContextInternal->pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
        fCloseKeyFile = TRUE;
        ChkDR( TST_KF_LoadFromFile( pAppContextInternal->pKeyFileContext, TRUE ) );
        ChkDR( TST_UTL_SetPlayReadyCertCachedValues( &pAppContextInternal->oBlackBoxContext ) );
    }

    ChkDR( TST_KF_GetCertificate(
                ((DRM_APP_CONTEXT_INTERNAL*)pAppContext)->oBlackBoxContext.pKeyFileContext,
                eKF_CERT_TYPE_PLAYREADY,
                &pbRemoteCert,
                &cbRemoteCert ) );

    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

    oLicensePolicyDecriptor.wSecurityLevel   = 150;
    oLicensePolicyDecriptor.fCannotPersist   = TRUE;

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_LOCAL_BOUND_SIMPLE,
       &g_keyID,
        cbRemoteCert,
        pbRemoteCert,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_LOCAL_LICENSE_WITH_REMOTE_CERTIFICATE );

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_DEVICE_NOT_REGISTERED );

    ChkDR( Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        cbRemoteCert,
        pbRemoteCert,
        NULL,
       &licenseHandle ) );

    ChkDR( Drm_LocalLicense_CreateRMHeader( &g_keyID, &cbRMHeader, &pbRMHeader ) );

    ChkDR( Drm_LocalLicense_CreatePlayReadyObject(
        licenseHandle,
        cbRMHeader,
        pbRMHeader,
       &cbHeader,
       &pbHeader ) );

    ChkDR( Drm_LocalLicense_EncryptSample(
        licenseHandle,
        2,
        pdwEncryptedLength,
        ppbEncryptedPointer,
       &qwIV ) );

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        goto ErrorExit;
    }

    ChkDR( Drm_Content_SetProperty(
        pAppContext,
        DRM_CSP_PLAYREADY_OBJ,
        pbHeader,
        cbHeader ) );

    ChkDR( Drm_Reader_Bind(
        pAppContext,
       &pdstrPlay,
        1,              /* Passing only 1 right, play */
        NULL,
        NULL,
       &oEncryptCtx ) );

    oAesCtrCtx.qwInitializationVector = qwIV;

    ChkDR( Drm_Reader_Decrypt(
        &oEncryptCtx,
        &oAesCtrCtx,
         (DRM_BYTE*)pbEncryptedData,
         SIZEOF( g_rgbClearData ) ) );

    ChkBOOL( 0 == DRMCRT_memcmp( g_rgbClearData, pbEncryptedData, SIZEOF( g_rgbClearData ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( Drm_Reader_Commit(
        pAppContext,
        NULL,
        NULL ) );

ErrorExit:
    DRM_PRND_SAFEMEMFREE( pbHeader );
    DRM_PRND_SAFEMEMFREE( pbRMHeader );

    Drm_Reader_Close( &oEncryptCtx );

    if( !DRM_DEVICEASSETS_IsDeviceAssetsSupported() &&  pAppContext != NULL && fCloseKeyFile )
    {
        DRM_APP_CONTEXT_INTERNAL *pAppContextInternal = (DRM_APP_CONTEXT_INTERNAL *) pAppContext;
        DRM_RESULT dr2 = DRM_KF_Close( pAppContextInternal->pKeyFileContext );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }

    if( licenseHandle != DRM_LICENSE_HANDLE_INVALID)
    {
        DRM_RESULT dr2 = Drm_LocalLicense_Release( &licenseHandle );
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = dr2;
        }
    }
    return dr;
}

DRM_RESULT DRM_CALL TestLicGenInvalidDescriptor( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                                   dr                                                                             = DRM_SUCCESS;
    DRM_LOCAL_LICENSE_POLICY_DESCRIPTOR          oLicensePolicyDecriptor                                                        = { 0 };
    DRM_APP_CONTEXT                             *pAppContext                                                                    = NULL;
    DRM_LICENSE_HANDLE                           licenseHandle                                                                  = DRM_LICENSE_HANDLE_INVALID;

    GET_SHARED_APP_CONTEXT( pAppContext );
    ChkDR( Drm_Reinitialize( pAppContext ) );

    ChkDR( Drm_LocalLicense_InitializePolicyDescriptor( &oLicensePolicyDecriptor ) );

    oLicensePolicyDecriptor.wSecurityLevel = 150;
    oLicensePolicyDecriptor.fCannotPersist = FALSE;

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_PERSISTENT_REMOTE_LICENSE );

    oLicensePolicyDecriptor.fCannotPersist = TRUE;

    oLicensePolicyDecriptor.oExpirationAfterFirstPlay.fValid = TRUE;
    oLicensePolicyDecriptor.oExpirationAfterFirstPlay.dwValue = g_dwLicenseExpireTime;

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_EXPIRE_AFTER_FIRST_PLAY_REMOTE_LICENSE );

    oLicensePolicyDecriptor.oExpirationAfterFirstPlay.fValid = FALSE;

    /* Set non-supported PlayEnabler and expect an error */
    OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[0], &g_guidPlayEnabler, SIZEOF( DRM_GUID ) );
    oLicensePolicyDecriptor.cPlayEnablers = 1;

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_PLAY_ENABLER_REMOTE_LICENSE );

    /* Set supported PlayEnaber and expect success */
    OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[0], &DRM_PLAYENABLER_UNKNOWN_OUTPUT, SIZEOF( DRM_GUID ) );
    oLicensePolicyDecriptor.cPlayEnablers = 1;
    ChkDR( Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle ) );
    ChkDR( Drm_LocalLicense_Release( &licenseHandle) );

    OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[1], &DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT, SIZEOF( DRM_GUID ) );
    oLicensePolicyDecriptor.cPlayEnablers = 2;
    ChkDR( Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle ) );
    ChkDR( Drm_LocalLicense_Release( &licenseHandle) );

    /* Set mixed non-supported PlayEnabler and supported PlayEnaber, expect an error */
    OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[2], &g_guidPlayEnabler, SIZEOF( DRM_GUID ) );
    oLicensePolicyDecriptor.cPlayEnablers = 3;

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_PLAY_ENABLER_REMOTE_LICENSE );

    /* Set duplicate PlayEnabler and expect an error */
    OEM_SECURE_MEMCPY( &oLicensePolicyDecriptor.rgoPlayEnablers[2], &DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT, SIZEOF( DRM_GUID ) );
    oLicensePolicyDecriptor.cPlayEnablers = 3;

    dr = Drm_LocalLicense_CreateLicense(
        pAppContext,
       &oLicensePolicyDecriptor,
        eDRM_LOCAL_LICENSE_REMOTE_BOUND_SIMPLE,
       &g_keyID,
        0,
        NULL,
        NULL,
       &licenseHandle );
    DRM_TEST_EXPECT_ERROR_AND_MAP_TO_SUCCESS( dr, DRM_E_LICGEN_DUPLICATE_PLAY_ENABLER );

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(licgentest)
    API_ENTRY(TestLicGenCreateLicense)
    API_ENTRY(TestLicGenCreateWDRMHeaderAndPRO)
    API_ENTRY(TestLicGenCreatePROAndDecrypt)
    API_ENTRY(TestLicGenTopLevel)
    API_ENTRY(TestLicGenRootLeafLicense)
    API_ENTRY(TestLicGenCache)
    API_ENTRY(TestLicGenCreateLicenseWithRemoteCert)
    API_ENTRY(TestPRNDTopLevelRegistration)
    API_ENTRY(TestPRNDTopLevelProximityDetection)
    API_ENTRY(TestManagerStoreRevocationPackage)
    API_ENTRY(TestPRNDCRLTimeOffset)
    API_ENTRY(TestLicGenInvalidDescriptor)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE;

