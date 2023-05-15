/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <oemkeyfile.h>
#include <drmmetering.h>
#include <drmsecureclock.h>
#include <drmactivation.h>
#include <drmwmdrm.h>

ENTER_PKTEST_NAMESPACE_CODE

/*
** Most of function has additional argument using as flags to
** decide which input should be overwritten:
**    1st bit overwrite APP_CONTEXT_VAR
**    2nd bit overwrite DECRYPT_CONTEXT_VAR
**    3rd bit overwrite g_pEncryptContext - depricated
**    4th bit overwrite g_oConditions - depricated
**    5th bit overwrite size
**    6th bit overwrite response
**    7th bit overwrite DRMMGR_READER_DATA_VAR
**    8th bit Don't free DRMMGR_READER_DATA_VAR
*/

const DRM_UINT64 g_qwDefaultInitializationVector = DRM_UI64LITERAL( 0, 100 );
static DRM_DWORD g_dwOriginalPlatformID = 0;
static DRM_DWORD g_dwOriginalWMRobustnessVersion = 0;
static DRM_DWORD g_dwOriginalPRRobustnessVersion = 0;

/********************************************************************************************
** Function:    TestManagerProcessRequest
**
** Synopsis:    Make sure that Drm_ProcessRequest returns DRM_E_NOTIMPL
**
** Arguments:   none
**
** Returns:     DRM_E_NOTIMPL
********************************************************************************************/
DRM_RESULT DRM_CALL TestManagerProcessRequest( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_DWORD  dwResponseResult1 = 0;
    DRM_DWORD  dwResponseResult2 = 0;
    DRM_DWORD  dwResponseResult3 = 0;
    DRM_DWORD  dwResponseResult4 = 0;
    DRM_BYTE   *pbData           = NULL;
    DRM_DWORD  cbData            = 0;
    DRM_APP_CONTEXT *poManagerContext = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );

    ChkDR( Drm_ProcessRequest( poManagerContext,
                               0,
                               0,
                               0,
                               0,
                               0,
                               &dwResponseResult1,
                               &dwResponseResult2,
                               &dwResponseResult3,
                               &dwResponseResult4,
                               pbData,
                               &cbData ) );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PreTestCase
**
** Synopsis:    Pretest case for the DRMManager test module
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_PRETESTCASE(DrmManager)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr       = DRM_SUCCESS;

    DRM_APP_CONTEXT          *poManagerContext   = NULL;
    DRM_BYTE                 *pbOpaqueBuffer     = NULL;
    DRM_DWORD                 cbOpaqueBuffer     = 0;
    DRM_DWORD                *pdwNumRights       = NULL;
    DRM_CONST_STRING         **ppRightsArray     = NULL;
    DRM_WORD                 *pwOPL              = NULL;
    DRM_DWORD                *pdwExtRestrictMod  = NULL;
    DRM_BOOL                 *pfNeedsCallBack    = NULL;
    DRM_DECRYPT_CONTEXT      *poDecryptContext   = NULL;
    DRM_BYTE                 *pbEncryptedData    = NULL;
    DRM_BYTE                 *pbData             = NULL;
    DRM_DWORD                *pcbData            = NULL;
    DRM_BYTE                 *pbContentPropData  = NULL;
    DRM_DWORD                *pcbContentPropData = NULL;
    DRM_AES_COUNTER_MODE_CONTEXT *pAesCtrContext = NULL;
    DRM_BYTE                 *prgbRC4Key         = NULL;
    DRM_BYTE                 *prgbAESKey         = NULL;
    DRM_BYTE                  rgbDefaultRC4Key[DRMCIPHERKEYLEN_RC4] = { 0x0D, 0x17, 0x52, 0x8C, 0x80, 0x19, 0x56 };
    DRM_BYTE                  rgbDefaultAESKey[DRM_AES_KEYSIZE_128] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    DRM_CHAR                 **pszFilePath       = NULL;
    DRM_UINT64               *pqwCurrTime        = NULL;
    DRM_UINT64               *pqwDiffTime        = NULL;
    DRM_LICENSE_RESPONSE     *poLicenseResponse  = NULL;
    DRM_LICENSE_RESPONSE      oDefaultLicResp    = { eUnknownProtocol };
    DRM_DWORD                *pcbDLRB            = NULL;
    DRM_STRING               *pdstrDLRB          = NULL;
    DRM_BYTE                 *pbRevocationBuffer = NULL;
    DRM_STRING               *pRevocation        = NULL;
    DRM_STRING               *pRevocationInfo    = NULL;
    DRM_METERCERT_ENUM       *poMeterCertEnum    = NULL;
    DRM_CONST_STRING         *pwszDeviceStore    = NULL;
    DRM_TEST_XPOL_DATA       *poXRightData       = NULL;
    DRM_TEST_XPOL_DATA       *poXRestrictionData = NULL;
    DRM_BYTE                 *pbNonce            = NULL;
    DRM_BYTE                 *pdrmidLID          = NULL;
    DRMSYSTEMTIME            *pSystemTime        = NULL;
    DRM_DWORD                *pdwSourceID        = NULL;

    GET_SHARED_APP_CONTEXT( poManagerContext );
    GET_SHARED_DEVICE_STORE( pwszDeviceStore );

    /*
    ** Set RESPONSE_XPOL_RIGHT_VAR
    */
    Log( "", "Setting RESPONSE_XPOL_RIGHT_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poXRightData, RESPONSE_XPOL_RIGHT_VAR, SIZEOF( DRM_TEST_XPOL_DATA ) );
    ZEROMEM( poXRightData, SIZEOF( DRM_TEST_XPOL_DATA ) );

    /*
    ** Set RESPONSE_XPOL_RESTRICTION_VAR
    */
    Log( "", "Setting RESPONSE_XPOL_RESTRICTION_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poXRestrictionData, RESPONSE_XPOL_RESTRICTION_VAR, SIZEOF( DRM_TEST_XPOL_DATA ) );
    ZEROMEM( poXRestrictionData, SIZEOF( DRM_TEST_XPOL_DATA ) );

    /*
    ** Set FILE_PATH_VAR for testing with multiple stores
    */
    Log( "", "Setting FILE_PATH_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pszFilePath, FILE_PATH_VAR, SIZEOF( DRM_CHAR* ) );
    *pszFilePath = NULL;

    /*
    ** Set shared time variables
    */
    Log( "", "Setting CURR_TIME_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pqwCurrTime, CURR_TIME_VAR, SIZEOF( DRM_UINT64 ) );
    Log( "", "Setting DIFF_TIME_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pqwDiffTime, DIFF_TIME_VAR, SIZEOF( DRM_UINT64 ) );

    /*
    ** Set shared encrpytion/decryption variables
    */
    Log( "", "Setting AES_COUNTER_MODE_CONTEXT_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pAesCtrContext, AES_COUNTER_MODE_CONTEXT_VAR, SIZEOF( DRM_AES_COUNTER_MODE_CONTEXT ) );

    Log( "", "Setting AES_KEY_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( prgbAESKey, AES_KEY_VAR, DRM_AES_KEYSIZE_128 * SIZEOF( DRM_BYTE ) );
    MEMCPY( prgbAESKey, rgbDefaultAESKey, DRM_AES_KEYSIZE_128 * SIZEOF( DRM_BYTE ) );
    Log( "", "Setting RC4_KEY_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( prgbRC4Key, RC4_KEY_VAR, DRMCIPHERKEYLEN_RC4 * SIZEOF( DRM_BYTE ) );
    MEMCPY( prgbRC4Key, rgbDefaultRC4Key, DRMCIPHERKEYLEN_RC4 * SIZEOF( DRM_BYTE ) );
    Log( "", "Setting DECRYPT_CONTEXT_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poDecryptContext, DECRYPT_CONTEXT_VAR, SIZEOF( DRM_DECRYPT_CONTEXT ) );
    ZEROMEM( poDecryptContext, SIZEOF( DRM_CIPHER_CONTEXT ) );
    Log( "", "Setting ENCRYPTED_DATA_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbEncryptedData, ENCRYPTED_DATA_VAR, MAXCIPHERDATA );

    Log( "", "Setting DRMMGR_READER_DATA_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbData, DRMMGR_READER_DATA_VAR, MAXCIPHERDATA );
    Log( "", "Setting DRMMGR_READER_DATA_SIZE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcbData, DRMMGR_READER_DATA_SIZE_VAR, SIZEOF( DRM_DWORD ) );
    *pcbData = MAXCIPHERDATA;

    Log( "", "Setting PROPERTY_DATA_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbContentPropData, PROPERTY_DATA_VAR, DRM_STANDARD_PLAYREADYOBJSIZE );
    Log( "", "Setting PROPERTY_SIZE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcbContentPropData, PROPERTY_SIZE_VAR, SIZEOF( DRM_DWORD ) );
    *pcbContentPropData = DRM_STANDARD_PLAYREADYOBJSIZE;

    /*
    ** Set shared rights variables
    */
    Log( "", "Setting NUMBER_OF_RIGHTS_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdwNumRights, NUMBER_OF_RIGHTS_VAR, SIZEOF( DRM_DWORD ) );
    *pdwNumRights = 0;

    Log( "", "Setting RIGHTS_ARRAY_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( ppRightsArray, RIGHTS_ARRAY_VAR, MAX_RIGHTS * SIZEOF( DRM_CONST_STRING* ) );

    /*
    ** Set shared Reader flags
    */
    Log( "", "Setting OPL_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pwOPL, OPL_VAR, 7 * SIZEOF( DRM_WORD ) );

    Log( "", "Setting EXT_RESTRICTION_MODULE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdwExtRestrictMod, EXT_RESTRICTION_MODULE_VAR, SIZEOF( DRM_DWORD ) );
    *pdwExtRestrictMod = 0;

    Log( "", "Setting NEEDS_CALLBACK_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pfNeedsCallBack, NEEDS_CALLBACK_VAR, SIZEOF( DRM_BOOL ) );
    *pfNeedsCallBack = FALSE;

    Log( "", "Setting RESTRICTED_SOURCE_ID_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdwSourceID, RESTRICTED_SOURCE_ID_VAR, SIZEOF( DRM_DWORD ) );
    *pdwSourceID = 0;

    /*
    ** Set license acquisition shared variables
    */
    Log( "", "Setting LICENSE_RESPONSE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( poLicenseResponse, LICENSE_RESPONSE_VAR, SIZEOF( DRM_LICENSE_RESPONSE ) );
    MEMCPY( poLicenseResponse, &oDefaultLicResp, SIZEOF( DRM_LICENSE_RESPONSE ) );

    Log( "", "Setting LICENSE_NONCE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbNonce, LICENSE_NONCE_VAR, CCH_BASE64_EQUIV( SIZEOF( DRM_KID) ) + 1 );
    ZEROMEM( pbNonce, CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 );

    Log( "", "Setting LICENSE_ID_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdrmidLID, LICENSE_ID_VAR, SIZEOF( DRM_GUID ) + 1 );
    ZEROMEM( pdrmidLID, SIZEOF( DRM_GUID ) + 1 );

    Log( "", "Setting LICENSE_REMOVAL_DATE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pSystemTime, LICENSE_REMOVAL_DATE_VAR, SIZEOF( DRMSYSTEMTIME ) );
    ZEROMEM( pSystemTime, SIZEOF( DRMSYSTEMTIME ) );
    Oem_Clock_GetSystemTime(NULL, pSystemTime);

    /*
    ** Set device license revocation blob variables
    */
    Log( "", "Setting DEVICE_LICENSE_REV_BLOB_SIZE_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pcbDLRB, DEVICE_LICENSE_REV_BLOB_SIZE_VAR, SIZEOF( DRM_DWORD ) );
    *pcbDLRB =  10000;
    Log( "", "Setting DEVICE_LICENSE_REV_BLOB_STRING_VAR" );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pdstrDLRB, DEVICE_LICENSE_REV_BLOB_STRING_VAR, SIZEOF( DRM_STRING ) );
    pdstrDLRB->pwszString = NULL;
    pdstrDLRB->cchString = 0;

    /*
    ** Free DEVICE_LICENSE_REV_BLOB_VAR
    */
    Log( "", "Setting DEVICE_LICENSE_REV_BLOB_VAR" );
    dr = DRM_TST_FreeTestVar( DEVICE_LICENSE_REV_BLOB_VAR );
    if ( dr == DRM_E_TEST_VARIABLE_NOTFOUND )
    {
        dr = DRM_SUCCESS;
    }
    ChkDR( dr );

    /*
    ** Reset the shared App Context
    */
    ChkDR( TestScanDevicePrivateKey( poManagerContext ) );

    Drm_Uninitialize( poManagerContext );
    ChkDR( Drm_GetOpaqueBuffer( poManagerContext, &pbOpaqueBuffer, &cbOpaqueBuffer ) );
    if( pbOpaqueBuffer == NULL )
    {
        GET_SHARED_DRMBUFFER( pbOpaqueBuffer );
    }
    MEMSET( poManagerContext, 0, SIZEOF( DRM_APP_CONTEXT ) );
    MEMSET( pbOpaqueBuffer, 0, TEST_DRM_BUFFER_SIZE );

    /*
    ** Delete the app context
    */
    RemoveDRMFile( RMFILE_STORE );

    /* Mark the secure clock as set in both the OEM layer, and the DRM manager context because the
    ** DRM manager context has already been initialized and read out the non-set OEM state */
    if( DRM_CLK_IsSecureClockSupported() )
    {
        DRM_APP_CONTEXT_INTERNAL *poManagerContextInt = (DRM_APP_CONTEXT_INTERNAL*)poManagerContext;

        ChkDR( Oem_Clock_SetResetState( NULL, FALSE ) );
        poManagerContextInt->fClockSet = TRUE;
    }

    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        CLIENTID clientID = {0};
        ChkDR( TestGetClientID( &clientID ) );
        ChkDR( TestLicResponseSetClientID( &clientID ) );
    }

    ChkDR( TestDrm_Platform_Initialize() );

    /*
    ** Call initialize using the shared device store name.
    ** For multiple store case, the previous store should be saved to a different location or the shared
    ** store name should be changed before calling this function
    */
    ChkDR( Drm_Initialize( poManagerContext, NULL, pbOpaqueBuffer, TEST_DRM_BUFFER_SIZE, pwszDeviceStore ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /*
        ** Set shared revocation variables after Drm_Initialize()
        */
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbRevocationBuffer, REVOCATION_BUFFER_VAR, REVOCATION_BUFFER_SIZE );
        ZEROMEM( pbRevocationBuffer, REVOCATION_BUFFER_SIZE );
        ChkDR( Drm_Revocation_SetBuffer( poManagerContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );

        Log( "", "Setting REVOCATION_STRING_VAR" );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pRevocation, REVOCATION_STRING_VAR, SIZEOF( DRM_STRING ) );
        pRevocation->pwszString = NULL;
        pRevocation->cchString = 0;

        Log( "", "Setting REV_INFO_VAR" );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pRevocationInfo, REV_INFO_VAR, SIZEOF( DRM_STRING ) );
        pRevocationInfo->pwszString = NULL;
        pRevocationInfo->cchString = 0;

    }

    if( DRM_METERING_IsMeteringSupported() )
    {
        /*
        ** Set METER_CERT_ENUM_CONTEXT_VAR
        */
        Log( "", "Setting METER_CERT_ENUM_CONTEXT_VAR" );
        MAP_TO_GLOBAL_TEST_VAR_Alloc( poMeterCertEnum, METER_CERT_ENUM_CONTEXT_VAR, SIZEOF( DRM_METERCERT_ENUM ) );
    }

    g_dwOriginalPlatformID          = g_dwPlatformID;
    g_dwOriginalWMRobustnessVersion = g_dwWMRobustnessVersion;
    g_dwOriginalPRRobustnessVersion = g_dwPRRobustnessVersion;

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    PostTestCase
**
** Synopsis:    Posttest case for the DRMManager test module
**
** Arguments:   none
**
** Returns:     DRM_SUCCESS
********************************************************************************************/
DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(DrmManager)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD                *pdwNumRights       = NULL;
    DRM_CONST_STRING         **ppRightsArray     = NULL;
    DRM_CONST_STRING         *pCurRight          = NULL;
    DRM_CHAR                 **pszFilePath       = NULL;
    DRM_STRING               *pRevocation        = NULL;
    DRM_STRING               *pRevocationInfo    = NULL;
    DRM_TEST_XPOL_DATA       *poXRightData       = NULL;
    DRM_TEST_XPOL_DATA       *poXRestrictionData = NULL;

    tResetSystemTimeOffset();

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        MAP_TO_GLOBAL_TEST_VAR( pRevocation, REVOCATION_STRING_VAR );
        MAP_TO_GLOBAL_TEST_VAR( pRevocationInfo, REV_INFO_VAR );
        FREEDRMSTRINGPOINTER( pRevocation );
        FREEDRMSTRINGPOINTER( pRevocationInfo );
    }

    /*
    ** Clean up second hds if one was used
    */
    if ( DRM_SUCCEEDED( DRM_TST_GetTestVar( FILE_PATH_VAR, ( DRM_VOID ** ) &pszFilePath, NULL ) ) )
    {
        if( *pszFilePath != NULL )
        {
            ChkDR( tRemoveFile( *pszFilePath ) );
        }
        SAFE_OEM_FREE( *pszFilePath );
    }

    /*
    ** Free all memory pointed to by shared pointers
    */
    ChkDR( DRM_TST_GetTestVar( NUMBER_OF_RIGHTS_VAR, ( DRM_VOID ** ) &pdwNumRights, NULL ) );
    ChkDR( DRM_TST_GetTestVar( RIGHTS_ARRAY_VAR, ( DRM_VOID ** ) &ppRightsArray, NULL ) );
    while ( ( pdwNumRights != NULL ) && ( *pdwNumRights > 0 ) )
    {
        *pdwNumRights = *pdwNumRights - 1;
        pCurRight = ppRightsArray[ *pdwNumRights ];
        FREEDRMSTRINGPOINTER( pCurRight );
        SAFE_OEM_FREE( pCurRight );
    }
    MAP_TO_GLOBAL_TEST_VAR( poXRightData, RESPONSE_XPOL_RIGHT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( poXRestrictionData, RESPONSE_XPOL_RESTRICTION_VAR );
    SAFE_OEM_FREE( poXRightData->pStateDateCountorBinary );
    SAFE_OEM_FREE( poXRightData->pbXPolData );
    SAFE_OEM_FREE( poXRestrictionData->pStateDateCountorBinary );
    SAFE_OEM_FREE( poXRestrictionData->pbXPolData );

    g_dwPlatformID          = g_dwOriginalPlatformID;
    g_dwWMRobustnessVersion = g_dwOriginalWMRobustnessVersion;
    g_dwPRRobustnessVersion = g_dwOriginalPRRobustnessVersion;

    /*
    ** Need to clean up key history list so that the next test starts from the clean state.
    */
    ChkDR( TestManagerFreeKeyfileKeys( 0, NULL ) );

ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST(DrmManager)
    API_ENTRY(TestManagerInitialize)
    API_ENTRY(TestSendChallenge)
    API_ENTRY(TestManagerGenerateChallenge)
    API_ENTRY(TestManagerGenerateLicenseAck)
    API_ENTRY(TestManagerProcessResponse)
    API_ENTRY(TestManagerProcessResponse_ModelRevoked)
    API_ENTRY(TestManagerProcessResponse_CheckResults)
    API_ENTRY(VerifyEmbeddingBehavior)
    API_ENTRY(VerifyEmbeddedLicenseAPI)
    API_ENTRY(VerifyNumRemovalDateLicenses)
    API_ENTRY(VerifySymmetricOptimizations)
    API_ENTRY(TestManagerProcessLicenseAckResponse)
    API_ENTRY(SetDomainID)
    API_ENTRY(TestProcessAdditionalData)
    API_ENTRY(GenerateDomainJoinChallenge)
    API_ENTRY(ProcessDomainJoinResponse)
    API_ENTRY(GenerateDomainLeaveChallenge)
    API_ENTRY(ProcessDomainLeaveResponse)
    API_ENTRY(GenerateDomainJoinResponse)
    API_ENTRY(TestManagerSetRights)
    API_ENTRY(TestManagerSetHeader)
    API_ENTRY(TestManagerSetPROHeaderWithKID)
    API_ENTRY(TestManagerSetV1Header)
    API_ENTRY(TestManagerGetLicenseData)
    API_ENTRY(TestManagerCommit)
    API_ENTRY(TestManagerEncrypt)
    API_ENTRY(TestManagerInitDecrypt)
    API_ENTRY(TestManagerDecrypt)
    API_ENTRY(TestManagerBind)
    API_ENTRY(TestManagerClose)
    API_ENTRY(TestManagerReinitialize)
    API_ENTRY(TestManagerUninitialize)
    API_ENTRY(TestManagerGenerateResponse)
    API_ENTRY(TestManagerAddLicenseToResponse)
    API_ENTRY(TestManagerAddLicenseFromFileToResponse)
    API_ENTRY(TestManagerParseNonceTokenFromChallenge)
    API_ENTRY(TestManagerInitResponse)
    API_ENTRY(Test_ChangeTimeSeconds)
    API_ENTRY(Test_SetFlagOEMTimeOffsetIsAbsolute)
    API_ENTRY(TestManagerPrepareOPLCallback)
    API_ENTRY(TestManagerInitializeForNewStore)
    API_ENTRY(TestManagerSetMachineClock)
    API_ENTRY(TestManagerGetDeviceProperty)
    API_ENTRY(TestManagerCleanLicenseStore)
    API_ENTRY(TestManagerGetLicenseState)
    API_ENTRY(TestManagerMakeDeviceLicenseRevocationBlob)
    API_ENTRY(TestManagerProcessDeviceLicenseRevocation)
    API_ENTRY(TestManagerGetSourceID)
    API_ENTRY(TestEnumLicense)
    API_ENTRY(TestManagerGetLicenseMID)
    API_ENTRY(TestManagerIsInclusionGUIDPresent)
    API_ENTRY(TestManagerCheckDecryptContext)
    API_ENTRY(TestManagerSetKID)
    API_ENTRY(TestManagerSetKIDFromList)
    API_ENTRY(TestManagerSetV41Header)
    API_ENTRY(TestManagerMeterCertInitEnum)
    API_ENTRY(TestManagerMeterCertEnumNext)
    API_ENTRY(TestManagerUpdateMeterCert)
    API_ENTRY(TestManagerUpdateMeterCertFromFile)
    API_ENTRY(TestManagerDeleteMeterCert)
    API_ENTRY(TestManagerInvalidateMeterCert)
    API_ENTRY(TestManagerInitLicenseSyncEnum)
    API_ENTRY(TestManagerLicenseSyncEnumNext)
    API_ENTRY(TestManagerDeleteLicensesByKID)
    API_ENTRY(TestPrintSyncStore)
    API_ENTRY(TestClearHeader)
    API_ENTRY(LoadResponseFromFile)
    API_ENTRY(TestManagerAddManyLicenses)
    API_ENTRY(TestManagerGenerateMeterChallenge)
    API_ENTRY(TestManagerProcessMeterResponse)
    API_ENTRY(TestManagerAddManyMeteringData)
    API_ENTRY(TestManagerClearHDSCaches)
    API_ENTRY(TestManagerGetLicenseRevInfoVersion)
    API_ENTRY(TestManagerSetRevocationInfo)
    API_ENTRY(TestManagerSetRevocation)
    API_ENTRY(TestManagerSetPRRevDataFromFile)
    API_ENTRY(TestManagerGetRevocationList)
    API_ENTRY(TestManagerCheckRevCRLVersion)
    API_ENTRY(TestManagerCheckRevCRLVersionWM7)
    API_ENTRY(TestManagerStoreRevocationLists)
    API_ENTRY(TestManagerStoreRevocationPackage)
    API_ENTRY(TestSetCrl)
    API_ENTRY(TestManagerSetModelNamefromReg)
    API_ENTRY(TestManagerCreateDecryptContext)
    API_ENTRY(TestManagerOpenEnvelope)
    API_ENTRY(TestManagerReOpenEnvelope)
    API_ENTRY(TestManagerCloseEnvelope)
    API_ENTRY(TestManagerInitializeEnvelopeRead)
    API_ENTRY(TestManagerGetEnvelopeSize)
    API_ENTRY(TestManagerReadEnvelope)
    API_ENTRY(TestManagerEnvelopeWritePlayReadyObject)
    API_ENTRY(TestManagerSeekEnvelope)
    API_ENTRY(TestManagerGetEnvelopeOriginalFilename)
    API_ENTRY(ProcessDomainJoinResponse)
    API_ENTRY(AddMultipleXMRLicenses)
    API_ENTRY(GenerateRandomHeader)
    API_ENTRY(TestManagerSetExtensiblePolicy)
    API_ENTRY(TestManagerAddXMRLicense)
    API_ENTRY(TestManagerAddXMRLicenseFromFile)
    API_ENTRY(TestManagerAddManyXMRLicenses)
    API_ENTRY(TestManagerAddManyXMRLicensesFromPool)
    API_ENTRY(TestManagerProcessRequest)
    API_ENTRY(TestManagerUpdateEmbeddedStore)
    API_ENTRY(TestManagerUpdateEmbeddedStoreCommit)
    API_ENTRY(TestManagerGetContentProperty)
    API_ENTRY(TestManagerSetSecureClock)
    API_ENTRY(TestManagerSetGetClockState)
    API_ENTRY(TestManagerCreateXMRLicense)
    API_ENTRY(TestManagerCreateXMRLicenseFromPool)
    API_ENTRY(TestManagerGenerateXMRResponse)
    API_ENTRY(TestManagerSetRobustnessVersion)
    API_ENTRY(TestManagerSetHardwareID)
    API_ENTRY(TestManagerValidateRobustnessVersion)
    API_ENTRY(TestManagerAddNewKeyfileKey)
    API_ENTRY(TestManagerFreeKeyfileKeys)
    API_ENTRY(TestManagerHashKeyfile)
    API_ENTRY(TestManagerResizeOpaqueBuffer)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

