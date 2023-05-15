/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_DRMMANAGER_TEST_API_H
#define __DRM_DRMMANAGER_TEST_API_H

#include "PKTestNameSpace.h"

#include <drmmanager.h>
#include <licgenxmr.h>
#include <xmrlicgen.h>

ENTER_PKTEST_NAMESPACE

/*
** ---------------------------------------
** DRMManager Test Global Variable Names
** ---------------------------------------
*/
#define MAX_TEST_DATA 10
#define DECRYPT_CONTEXT_VAR "SharedDecryptContext"
#define AES_COUNTER_MODE_CONTEXT_VAR "SharedAesCtrContext"
extern const DRM_UINT64 g_qwDefaultInitializationVector;

#define PROPERTY_DATA_VAR "SharedPropertyData"
#define PROPERTY_SIZE_VAR "SharedPropertySize"

/*
** ---------------------------------------
** DRMManager DeviceLicenseRevocationBlob Global Variable  Names
** ---------------------------------------
*/
#define DEVICE_LICENSE_REV_BLOB_VAR         "SharedDLRB"
#define DEVICE_LICENSE_REV_BLOB_SIZE_VAR    "SharedDLRBSize"
#define DEVICE_LICENSE_REV_BLOB_STRING_VAR  "SharedDLRBString"


/*
** ---------------------------------------
** DRMManager Initialization Global Variable Names
** ---------------------------------------
*/
#define FILE_PATH_VAR "SharedFilePath"
#define CURR_TIME_VAR "SharedCurrTime"
#define DIFF_TIME_VAR "SharedDiffTime"

/*
** ---------------------------------------
** DRMManager License Acquisition Global Variable Names
** ---------------------------------------
*/
#define RESPONSECONTEXTSIZE     50000
#define RESPONSE_CONTEXT_VAR    "SharedResponseContext"
#define RESPONSE_VAR            "SharedResponse"
#define RESPONSE_SIZE_VAR       "SharedResponseSize"
#define CHALLENGE_URL_VAR       "SharedChallengeURL"
#define CHALLENGE_URL_SIZE_VAR  "SharedChallengeURLSize"
#define LICENSE_RESPONSE_VAR    "SharedLicenseResponse"

/*
** ---------------------------------------
** DRMManager License Response Global Variable Names
** ---------------------------------------
*/
#define RESPONSE_V2_SLK_ID         "V2SymmetricKeyID"
#define RESPONSE_V2_SLK            "V2SymmetricKey"
#define RESPONSE_V2_SLK_DATA       "V2SymmetricKeyData"

#define DRM_ID_B64_LEN             ( CCH_BASE64_EQUIV( SIZEOF( DRM_ID ) ) )
#define DRM_ID_B64_WCHAR_LEN       ( DRM_ID_B64_LEN * SIZEOF( DRM_WCHAR ) )
#define DRM_ID_B64_WCHAR_SZ_LEN    ( ( DRM_ID_B64_LEN + 1 ) * SIZEOF( DRM_WCHAR ) )

typedef struct __tagResponseKIDListData
{
    DRM_WCHAR     rgwchKID[ DRM_ID_B64_WCHAR_SZ_LEN ];
    DRM_BYTE      rgbContentKey[ DRM_AES_KEYSIZE_128 ];
} ResponseKIDListData;

#define RESPONSE_KIDLIST_DATA      "KIDListData"
#define RESPONSE_KIDLIST_INDEX     "KIDListIndex"

/*
** ---------------------------------------
** DRMManager Metering Global Variable Names
** ---------------------------------------
*/
#define METER_CERT_ENUM_CONTEXT_VAR "SharedMeterCertEnumContext"

/*
** ---------------------------------------
** DRMManager Reader Test Global Variable Names
** ---------------------------------------
*/
#define MAXCIPHERDATA   100000
#define MAX_RIGHTS      5
#define DRMMGR_READER_DATA_SIZE_VAR "SharedDRMMGRReaderData"
#define DRMMGR_READER_DATA_VAR  "SharedDRMMGRReaderDataSize"
#define ENCRYPTED_DATA_VAR      "SharedEncryptedData"
#define NUMBER_OF_RIGHTS_VAR    "SharedNumberOfRights"
#define RIGHTS_ARRAY_VAR        "SharedRightsArray"

#define OPL_VAR "SharedOPL"
#define EXT_RESTRICTION_MODULE_VAR "SharedExtRestrictionModule"
#define RESTRICTED_SOURCE_ID_VAR "SharedRestrictedSourceID"
#define NEEDS_CALLBACK_VAR "SharedNeedsCallBack"

extern const char       *g_guidWMDRMDeviceCopy;
extern const char       *g_guidPDDRMDeviceCopy;
extern const char       *g_guidExtension1;
extern const char       *g_guidExtension2;
extern const DRM_DWORD   g_dwExtension1;
extern const DRM_DWORD   g_dwExtension2;

/*
** ---------------------------------------
** DRMManager Revocation Test Global Variable Names
** ---------------------------------------
*/
#define REV_INFO_VAR "SharedRevocationInfo"
#define REVOCATION_STRING_VAR "SharedRevocationString"


/*
** ---------------------------------------
** DRMManager  Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL SetGlobalTestVar( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessRequest( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Content Get and Set Property Test API Functions
** ---------------------------------------
*/
#define FILE_TYPE_UNDEFINED 0
#define FILE_TYPE_UTF8      1
#define FILE_TYPE_UTF16     2
#define FILE_TYPE_BINARY    3
DRM_RESULT DRM_CALL TestManagerSetHeader( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetPROHeaderWithKID( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetV1Header( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestClearHeader(long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetKID( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetKIDFromList( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetV41Header( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetContentProperty( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerUpdateEmbeddedStore( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerUpdateEmbeddedStoreCommit( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerConvertHeaderFromWmdrmToPlayReady( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager DeviceLicenseRevocationBlob Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerMakeDeviceLicenseRevocationBlob( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessDeviceLicenseRevocation( long argc, __in_ecount( argc ) char **argv );


/*
** ---------------------------------------
** DRMManager Device Property Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerGetDeviceProperty( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Device Store Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL RestoreDeviceStore( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL SaveDeviceStore( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Domains Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL SetDomainID( __in long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL ProcessDomainJoinResponse( IN long argc, IN __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL GenerateDomainJoinChallenge( IN long argc, IN __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL ProcessDomainLeaveResponse( IN long argc, IN __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL GenerateDomainLeaveChallenge( IN long argc, IN __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Encrypt Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerEncrypt( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Envelope Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerCreateDecryptContext( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerOpenEnvelope( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerReOpenEnvelope( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCloseEnvelope( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerInitializeEnvelopeRead( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetEnvelopeSize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerReadEnvelope( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSeekEnvelope( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetEnvelopeOriginalFilename( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerEnvelopeWritePlayReadyObject( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager HDS Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerClearHDSCaches( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Initialization Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerInitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerReinitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerUninitialize( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerInitializeForNewStore( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerResizeOpaqueBuffer( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager License Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL GetLicenseEnum(DRM_KID KID, DRM_DWORD* pdwCount, DRM_LICSTORE_CONTEXT* pLicStoreContext, DRM_LICSTOREENUM_CONTEXT** ppLicEnumContext);
DRM_RESULT DRM_CALL TestEnumLicense( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetSourceID( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerIsInclusionGUIDPresent( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyEmbeddingBehavior( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyEmbeddedLicenseAPI( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL VerifyNumRemovalDateLicenses( long argc, __in_ecount( argc ) char **argv );


/*
** ---------------------------------------
** DRMManager License Acquisition Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerGenerateChallenge( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGenerateLicenseAck( long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL TestProcessAdditionalData(long argc, __in_ecount(argc) char **argv );
DRM_RESULT DRM_CALL VerifySymmetricOptimizations( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessResponse_ModelRevoked( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessResponse_CheckResults( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessLicenseAckResponse( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager License Response Test API Functions
** ---------------------------------------
*/

DRM_RESULT DRM_CALL TestManagerParseNonceTokenFromChallenge( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerInitResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddLicenseToResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddLicenseFromFileToResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGenerateResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGenerateXMRResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCreateXMRLicense( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCreateXMRLicenseFromPool( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddXMRLicense( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddXMRLicenseFromFile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddManyXMRLicenses( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddManyXMRLicensesFromPool( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddManyLicenses( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL LoadResponseFromFile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL AddMultipleXMRLicenses( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL GenerateRandomHeader( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetExtensiblePolicy( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager License Query Test API Functions
** ---------------------------------------
*/

/*
** GetLicenseData
*/
static const DRM_WCHAR playonpcstring1 []        = { ONE_WCHAR('P', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('y', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR backupstring1[]           = { ONE_WCHAR('B', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('u', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR burntocdstring1 []        = { ONE_WCHAR('P', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('b', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR createpmlicensestring1 [] = { ONE_WCHAR('C', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('A', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('_', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('N', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR badstring []              = { ONE_WCHAR('B', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('\0', '\0') };
static const DRM_WCHAR foostring []              = { ONE_WCHAR('F', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('\0', '\0') };

DRM_RESULT DRM_CALL TestManagerGetLicenseData( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetLicenseState( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Metering Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerGenerateMeterChallenge( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerProcessMeterResponse( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddManyMeteringData( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetLicenseMID( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerMeterCertInitEnum( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerMeterCertEnumNext( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerUpdateMeterCert( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerUpdateMeterCertFromFile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerInvalidateMeterCert( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerDeleteMeterCert( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager NetIO Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestSendChallenge( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Reader Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerPrepareOPLCallback( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL ExtensiblePolicyModule0(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule1(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule2(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule3(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule4(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule5(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule6(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule7(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule8(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule9(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule10(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule11(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL ExtensiblePolicyModule12(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT * f_pCallback, TOKEN* f_pToken);
DRM_RESULT DRM_CALL OPLCallback(
    IN const DRM_VOID                 *f_pvOutputLevelsData,
    IN       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    IN const DRM_VOID                 *f_pv );
DRM_RESULT DRM_CALL TestManagerBind( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerInitDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerDecrypt( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCommit( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetRights( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCheckDecryptContext( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Revocation Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerSetRevocationInfo( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetRevocation( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetPRRevDataFromFile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetRevocationList( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCheckRevCRLVersion( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerCheckRevCRLVersionWM7( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerStoreRevocationLists( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerGetLicenseRevInfoVersion( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerStoreRevocationPackage(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestManagerSetModelNamefromReg(long argc, __in_ecount(argc) char **argv);
DRM_RESULT DRM_CALL TestSetCrl(long argc, __in_ecount(argc) char **argv);

/*
** ---------------------------------------
** DRMManager Secure Clock Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerSetSecureClock( long argc, __in_ecount( argc ) char **argv );

#if SIXTEEN_BIT_ADDRESSING
DRM_VOID *DRM_CALL testDRM_16B_Pack8BitBytes(
    DRM_NATIVE_BYTE *f_pbNative,
    int        f_cbNative,
    DRM_VOID        *f_pvPacked,
    DRM_DWORD        f_fOptions);

#endif

/*
** ---------------------------------------
** DRMManager Upgrade API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerSetRobustnessVersion( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerValidateRobustnessVersion( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerAddNewKeyfileKey( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerFreeKeyfileKeys( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerHashKeyfile( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetHardwareID( long argc, __in_ecount( argc ) char **argv );


/*
** ---------------------------------------
** DRMManager Store Management Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL  ReportProgress(
    IN const DRM_VOID  *pvCallerData,
    IN       DRM_DWORD  cLicensesProcessed,
    IN       DRM_DWORD  cTotalLicenses
    );
DRM_RESULT DRM_CALL TestManagerCleanLicenseStore( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerDeleteLicensesByKID( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Sync List Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL TestManagerInitLicenseSyncEnum( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerLicenseSyncEnumNext( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestPrintSyncStore( long argc, __in_ecount( argc ) char **argv );

/*
** ---------------------------------------
** DRMManager Time Test API Functions
** ---------------------------------------
*/
DRM_RESULT DRM_CALL Test_ChangeTimeSeconds( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL Test_SetFlagOEMTimeOffsetIsAbsolute( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetMachineClock( long argc, __in_ecount( argc ) char **argv );
DRM_RESULT DRM_CALL TestManagerSetGetClockState( long argc, __in_ecount( argc ) char **argv );

DECLARE_MODULE_DEFS(DrmManager)

EXIT_PKTEST_NAMESPACE

#endif /* __DRM_DRMMANAGER_TEST_API_H */
