/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef DRM_XMR2XMLLIB_SUPPORTWM7
#error DRM_XMR2XMLLIB_SUPPORTWM7 must be defined
#endif /* #ifndef DRM_XMR2XMLLIB_SUPPORTWM7 */

#if DRM_XMR2XMLLIB_SUPPORTWM7
#include <util.h>
#endif /* DRM_XMR2XMLLIB_SUPPORTWM7 */

#include <drmutilitieslite.h>
#include <drmxmr.h>
#include <drmapppolicy.h>
#include <xmr2xml.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmconstants.h>
#include <drmtoolsconstants.h>
#include <drmwmdrmconstants.h>
#include <drmmathsafe.h>

#define DECLARE_CONST_STRING( Name, Value )        \
    const DRM_WCHAR  g_rgwch##Name[] = Value;      \
    DRM_CONST_STRING g_dstr##Name    = CREATE_DRM_STRING( g_rgwch##Name );

#define XMR_OBJECT_TYPE_READ_POLICY_CONTAINER 0x0041

DRM_DEFINE_GUID( ANALOG_VIDEO_AGC_OUTPUT,                         0xC3FD11C6, 0xF8B7, 0x4D20, 0xB0, 0x08, 0x1D, 0xB1, 0x7D, 0x61, 0xF2, 0xDA );
DRM_DEFINE_GUID( ANALOG_VIDEO_EXPLICIT_OUTPUT,                    0x2098DE8D, 0x7DDD, 0x4BAB, 0x96, 0xC6, 0x32, 0xEB, 0xB6, 0xFA, 0xBE, 0xA3 );
DRM_DEFINE_GUID( ANALOG_VIDEO_COMPONENT_OUTPUT,                   0x811C5110, 0x46C8, 0x4C6E, 0x81, 0x63, 0xC0, 0x48, 0x2A, 0x15, 0xD4, 0x7E );
DRM_DEFINE_GUID( ANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT,            0xD783A191, 0xE083, 0x4BAF, 0xB2, 0xDA, 0xE6, 0x9F, 0x91, 0x0B, 0x37, 0x72 );
DRM_DEFINE_GUID( ANALOG_VIDEO_CGMSA,                              0x2098DE8D, 0x7DDD, 0x4bab, 0x96, 0xC6, 0x32, 0xEB, 0xB6, 0xFA, 0xBE, 0xA3 );
DRM_DEFINE_GUID( DIGITIAL_AUDIO_SCMS,                             0x6D5CFA59, 0xC250, 0x4426, 0x93, 0x0E, 0xFA, 0xC7, 0x2C, 0x8F, 0xCF, 0xA6 );
DRM_DEFINE_GUID( MSPR_ACTION_COPY,                                0xD8C5502C, 0x41B1, 0x4681, 0x8B, 0x61, 0x8A, 0x16, 0x18, 0xA3, 0x1D, 0xA7 );
DRM_DEFINE_GUID( MSPR_ACTION_COPY_TO_CD,                          0xEC930B7D, 0x1F2D, 0x4682, 0xA3, 0x8B, 0x8A, 0xB9, 0x77, 0x72, 0x1D, 0x0D );
DRM_DEFINE_GUID( MSPR_ACTION_COPY_TO_ISDB,                        0xC3CF56E0, 0x7FF2, 0x4491, 0x80, 0x9F, 0x53, 0xE2, 0x1D, 0x3A, 0xBF, 0x07 );
DRM_DEFINE_GUID( MSPR_ACTION_COPY_TO_CPRM,                        0xCDD801AD, 0xA577, 0x48DB, 0x95, 0x0E, 0x46, 0xD5, 0xF1, 0x59, 0x2F, 0xAE );
DRM_DEFINE_GUID( MSPR_ACTION_COPY_TO_CSS,                         0x3CAF2814, 0xA7AB, 0x467C, 0xB4, 0xDF, 0x54, 0xAC, 0xC5, 0x6C, 0x66, 0xDC );
DRM_DEFINE_GUID( MSPR_ACTION_COPY_TO_DEVICE,                      0x6848955D, 0x516B, 0x4EB0, 0x90, 0xE8, 0x8F, 0x6D, 0x5A, 0x77, 0xB8, 0x5F );
DRM_DEFINE_GUID( MSPR_ACTION_COPY_TO_PC,                          0xCE480EDE, 0x516B, 0x40B3, 0x90, 0xE1, 0xD6, 0xCF, 0xC4, 0x76, 0x30, 0xC5 );
DRM_DEFINE_GUID( MSPR_ACTION_PLAY_USING_DTCP,                     0xD685030B, 0x0F4F, 0x43A6, 0xBB, 0xAD, 0x35, 0x6F, 0x1E, 0xA0, 0x04, 0x9A );

DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_ACP,                      0xC3FD11C6, 0xF8B7, 0x4D20, 0xB0, 0x08, 0x1D, 0xB1, 0x7D, 0x61, 0xF2, 0xDA );
DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_CGMSA,                    0x2098DE8D, 0x7DDD, 0x4BAB, 0x96, 0xC6, 0x32, 0xEB, 0xB6, 0xFA, 0xBE, 0xA3 );
DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_MAX_COMPONENT_RESOLUTION, 0x811C5110, 0x46C8, 0x4C6E, 0x81, 0x63, 0xC0, 0x48, 0x2A, 0x15, 0xD4, 0x7E );
DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_MAX_VGA_RESOLUTION,       0xD783A191, 0xE083, 0x4BAF, 0xB2, 0xDA, 0xE6, 0x9F, 0x91, 0x0B, 0x37, 0x72 );
DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_SCMS,                     0x6D5CFA59, 0xC250, 0x4426, 0x93, 0x0E, 0xFA, 0xC7, 0x2C, 0x8F, 0xCF, 0xA6 );

DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_DOWNSAMPLE_REQUIRED,      0x8f81a453, 0x998c, 0x4596, 0xb1, 0x50, 0xf8, 0x48, 0x6d, 0xf6, 0xf2, 0xe6 );
DRM_DEFINE_GUID( MSPR_INCLUSION_LIST_CPRM,                        0xCDD801AD, 0xA577, 0x48DB, 0x95, 0x0E, 0x46, 0xD5, 0xF1, 0x59, 0x2F, 0xAE );
DRM_DEFINE_GUID( MSPR_INCLUSION_LIST_CSS,                         0x3CAF2814, 0xA7AB, 0x467C, 0xB4, 0xDF, 0x54, 0xAC, 0xC5, 0x6C, 0x66, 0xDC );
DRM_DEFINE_GUID( MSPR_INCLUSION_LIST_DTCP,                        0xD685030B, 0x0F4F, 0x43A6, 0xBB, 0xAD, 0x35, 0x6F, 0x1E, 0xA0, 0x04, 0x9A );
DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_BEST_EFFORT_ACP,          0x9AD40B1F, 0x6494, 0x452C, 0x92, 0x09, 0xEE, 0x33, 0xA3, 0xFC, 0x11, 0xF9 );
DRM_DEFINE_GUID( MSPR_PROTECTION_SCHEME_BEST_EFFORT_CGMSA,        0x225CD36F, 0xF132, 0x49EF, 0xBA, 0x8C, 0xC9, 0x1E, 0xA2, 0x8E, 0x43, 0x69 );

DECLARE_CONST_STRING( RightsIdTag,                          L"RIGHTSID" );
DECLARE_CONST_STRING( VersionTag,                           L"VERSION" );
DECLARE_CONST_STRING( SignatureTag,                         L"SIGNATURE" );
DECLARE_CONST_STRING( ContainerOuter,                       L"CONTAINEROUTER" );
DECLARE_CONST_STRING( GlobalPoliciesContainer,              L"GLOBAL_POLICY_CONTAINER" );
DECLARE_CONST_STRING( RevInfoVer1,                          L"REV_INFO_VERSION_1" );
DECLARE_CONST_STRING( RevInfoVer2,                          L"REV_INFO_VERSION_2" );
DECLARE_CONST_STRING( RevInfoVerOtherVersion,               L"REV_INFO_VERSION_OTHER_VERSION" );
DECLARE_CONST_STRING( RevInfoVerOtherValue,                 L"REV_INFO_VERSION_OTHER_VALUE" );
DECLARE_CONST_STRING( RevocationContainer,                  L"REVOCATION_CONTAINER" );
DECLARE_CONST_STRING( MinimumEnvironment,                   L"MINIMUM_ENVIRONMENT_CONTAINER" );
DECLARE_CONST_STRING( SecurityLevelContainer,               L"SECURITY_LEVEL_CONTAINER" );
DECLARE_CONST_STRING( MinimumSecurityLevel,                 L"MINIMUM_SECURITY_LEVEL" );
DECLARE_CONST_STRING( MinimumAppRevocationListVersion,      L"MINIMUM_APP_REV_LIST_VER" );
DECLARE_CONST_STRING( MinimumDeviceRevocationListVersion,   L"MINIMUM_DEVICE_REV_LIST_VER" );
DECLARE_CONST_STRING( SerialNumber,                         L"SERIALNUMBER" );
DECLARE_CONST_STRING( Rights,                               L"RIGHTS" );
DECLARE_CONST_STRING( BeginTime,                            L"BEGINTIME" );
DECLARE_CONST_STRING( EndTime,                              L"ENDTIME" );
DECLARE_CONST_STRING( MeteringGuid,                         L"METERING_GUID" );
DECLARE_CONST_STRING( ExpirationAfterUse,                   L"EXPIRE_AFTER_FIRST_USE" );
DECLARE_CONST_STRING( ExpirationAfterStore,                 L"EXPIRE_AFTER_STORE" );
DECLARE_CONST_STRING( GracePeriod,                          L"GRACE_PERIOD" );
DECLARE_CONST_STRING( MaxPlaylistBurnCount,                 L"MAX_PLB_COUNT" );
DECLARE_CONST_STRING( TrackBurnCount,                       L"TRACK_BURN_COUNT" );
DECLARE_CONST_STRING( KeysContainer,                        L"KEYSCONTAINER" );
DECLARE_CONST_STRING( ContentKey,                           L"CONTENT_KEY" );
DECLARE_CONST_STRING( OptimizedContentKey,                  L"CONTENT_KEY_OPTIMIZED" );
DECLARE_CONST_STRING( KeyIdTag,                             L"KEYID" );
DECLARE_CONST_STRING( EncryptedKeyTag,                      L"ENCRYPTED_KEY" );
DECLARE_CONST_STRING( SymmetricCipherType,                  L"SYMMETRIC_CIPHER_TYPE" );
DECLARE_CONST_STRING( KeyEncryptionType,                    L"KEY_ENCRYPTION_TYPE" );
DECLARE_CONST_STRING( RC4_Cocktail,                         L"RC4 cocktail" );
DECLARE_CONST_STRING( AES_CTR,                              L"AES 128 (Counter Mode)" );
DECLARE_CONST_STRING( AES_ECB,                              L"AES 128 (ECB Mode)" );
DECLARE_CONST_STRING( AESSLK,                               L"AES 128 (Session Key)" );
DECLARE_CONST_STRING( Invalid,                              L"Invalid" );
DECLARE_CONST_STRING( RSA_1024,                             L"1024 bit RSA" );
DECLARE_CONST_STRING( Chained,                              L"Chained" );
DECLARE_CONST_STRING( ECC_256,                              L"ECC p256" );
DECLARE_CONST_STRING( ECC_256_WITH_KZ,                      L"ECC p256 with Kz" );
DECLARE_CONST_STRING( DeviceKey,                            L"DEVICE_KEY" );
DECLARE_CONST_STRING( Exponent,                             L"EXPONENT" );
DECLARE_CONST_STRING( Modulus,                              L"MODULUS" );
DECLARE_CONST_STRING( ECCDeviceKey,                         L"ECC_DEVICE_KEY" );
DECLARE_CONST_STRING( KeyData,                              L"KEY_DATA" );
DECLARE_CONST_STRING( ECCCurveType,                         L"ECC_CURVE" );
DECLARE_CONST_STRING( P256,                                 L"P256" );
DECLARE_CONST_STRING( UplinkKey,                            L"UPLINK_KEY" );
DECLARE_CONST_STRING( AuxKey,                               L"AUX_KEY" );
DECLARE_CONST_STRING( AuxKeyEntry,                          L"AUX_KEY_ENTRY" );
DECLARE_CONST_STRING( UplinkX,                              L"UPLINKX" );
DECLARE_CONST_STRING( Location,                             L"LOCATION" );
DECLARE_CONST_STRING( UplinkKidTag,                         L"UPLINK_KID" );
DECLARE_CONST_STRING( UplinkChainedChecksum,                L"CHAINED_CHECKSUM" );
DECLARE_CONST_STRING( UplinkChecksum,                       L"CHECKSUM" );
DECLARE_CONST_STRING( Signature,                            L"SIGNATURE" );
DECLARE_CONST_STRING( SignatureType,                        L"SIGNATURE_TYPE" );
DECLARE_CONST_STRING( Value,                                L"VALUE" );
DECLARE_CONST_STRING( OPL,                                  L"OPL" );
DECLARE_CONST_STRING( ExplicitAnalogVideoProtection,        L"ANALOG_VIDEO_PROTECTION" );
DECLARE_CONST_STRING( CompressedDigitalVideo,               L"COMPRESSED_DIGITAL_VIDEO" );
DECLARE_CONST_STRING( UncompressedDigitalVideo,             L"UNCOMPRESSED_DIGITAL_VIDEO" );
DECLARE_CONST_STRING( AnalogVideo,                          L"ANALOG_VIDEO" );
DECLARE_CONST_STRING( CompressedDigitalAudio,               L"COMPRESSED_DIGITAL_AUDIO" );
DECLARE_CONST_STRING( UncompressedDigitalAudio,             L"UNCOMPRESSED_DIGITAL_AUDIO" );
DECLARE_CONST_STRING( Config,                               L"CONFIG" );
DECLARE_CONST_STRING( Guid,                                 L"GUID" );
DECLARE_CONST_STRING( Count,                                L"COUNT" );
DECLARE_CONST_STRING( CopyProtectionLevel,                  L"COPY_PROTECTION_LEVEL" );
DECLARE_CONST_STRING( ExpirationTag,                        L"EXPIRATION" );
DECLARE_CONST_STRING( PlaybackPoliciesContainer,            L"PLAY_POLICY_CONTAINER" );
DECLARE_CONST_STRING( PlayCount,                            L"PLAYCOUNT" );
DECLARE_CONST_STRING( CopyCount,                            L"COPYCOUNT" );
DECLARE_CONST_STRING( CopyPoliciesContainer,                L"COPY_POLICY_CONTAINER" );
DECLARE_CONST_STRING( PlaylistBurnPoliciesContainer,        L"PLB_POLICY_CONTAINER" );
DECLARE_CONST_STRING( SHA256_HMAC,                          L"SHA-256 HMAC" );
DECLARE_CONST_STRING( AES128_OMAC,                          L"AES-128 OMAC" );
DECLARE_CONST_STRING( Unlimited,                            L"Unlimited" );
DECLARE_CONST_STRING( DomainID,                             L"DOMAIN_ID" );
DECLARE_CONST_STRING( DomainAccountID,                      L"Account ID" );
DECLARE_CONST_STRING( DomainRevision,                       L"Revision" );
DECLARE_CONST_STRING( UserID,                               L"USER_ID" );
DECLARE_CONST_STRING( RsaLicenseGranterKey,                 L"RSA_LICENSE_GRANTER_KEY" );
DECLARE_CONST_STRING( UnknownContainer,                     L"UNKNOWN_CONTAINER" );
DECLARE_CONST_STRING( UnknownObject,                        L"UNKNOWN_OBJECT" );
DECLARE_CONST_STRING( ObjectType,                           L"type" );
DECLARE_CONST_STRING( ObjectFlags,                          L"flags" );
DECLARE_CONST_STRING( ObjectPayload,                        L"OBJECT_PAYLOAD" );
DECLARE_CONST_STRING( ParsingError,                         L"<ERROR>Error 0xXXXXXXXX occurred parsing the license.</ERROR>" );     /* Refer to comments near usage in FormatXmrAsXml if modifying!! */
DECLARE_CONST_STRING( ExplicitDigitalAudioProtection,       L"DIGITAL_AUDIO_PROTECTION" );
DECLARE_CONST_STRING( PolicyMetaData,                       L"POLICY_META_DATA" );
DECLARE_CONST_STRING( EmbeddingBehaviorTag,                 L"EMBEDDING_BEHAVIOR" );
DECLARE_CONST_STRING( Ignore,                               L"IGNORE" );
DECLARE_CONST_STRING( Copy,                                 L"COPY" );
DECLARE_CONST_STRING( Move,                                 L"MOVE" );
DECLARE_CONST_STRING( CopyEnablerContainer,                 L"COPY_ENABLER_CONTAINER" );
DECLARE_CONST_STRING( CopyEnabler,                          L"COPY_ENABLER" );
DECLARE_CONST_STRING( PlayEnablerContainer,                 L"PLAY_ENABLER_CONTAINER" );
DECLARE_CONST_STRING( PlayEnabler,                          L"PLAY_ENABLER" );
DECLARE_CONST_STRING( MoveEnabler,                          L"MOVE_ENABLER" );
DECLARE_CONST_STRING( CopyPolicies2Container,               L"COPY_POLICY_2_CONTAINER" );
DECLARE_CONST_STRING( CopyToCD,                             L"COPY_TO_CD {EC930B7D-1F2D-4682-A38B-8AB977721D0D}" );
DECLARE_CONST_STRING( CopyToCSS,                            L"COPY_TO_CSS {3CAF2814-A7AB-467C-B4DF-54ACC56C66DC}" );
DECLARE_CONST_STRING( CopyToDevice,                         L"COPY_TO_DEVICE {6848955D-516B-4EB0-90E8-8F6D5A77B85F}" );
DECLARE_CONST_STRING( CopyToPC,                             L"COPY_TO_PC {CE480EDE-516B-40B3-90E1-D6CFC47630C5}" );
DECLARE_CONST_STRING( PlayUsingDTCP,                        L"PLAY_USING_DTCP {D685030B-0F4F-43A6-BBAD-356F1EA0049A}" );
DECLARE_CONST_STRING( CopyToISDB,                           L"COPY_TO_ISDB_OUTPUTS {C3CF56E0-7FF2-4491-809F-53E21D3ABF07}" );
DECLARE_CONST_STRING( CopyToCPRM,                           L"COPY_TO_CPRM {CDD801AD-A577-48DB-950E-46D5F1592FAE}" );
DECLARE_CONST_STRING( MSPR_ACTION_COPY,                     L"COPY {D8C5502C-41B1-4681-8B61-8A1618A31DA7}" );
DECLARE_CONST_STRING( ExecutePolicyContainer,               L"EXECUTE_POLICY_CONTAINER" );
DECLARE_CONST_STRING( ReadPolicyContainer,                  L"READ_POLICY_CONTAINER" );
DECLARE_CONST_STRING( RemovalDate,                          L"REMOVAL_DATE" );

DECLARE_CONST_STRING( ANALOG_VIDEO_AGC_OUTPUT,              L"ANALOG_VIDEO_AGC_OUTPUT {C3FD11C6-F8B7-4D20-B008-1DB17D61F2DA}" );
DECLARE_CONST_STRING( ANALOG_VIDEO_EXPLICIT_OUTPUT,         L"ANALOG_VIDEO_EXPLICIT_OUTPUT {2098DE8D-7DDD-4BAB-96C6-32EBB6FABEA3}" );
DECLARE_CONST_STRING( ANALOG_VIDEO_COMPONENT_OUTPUT,        L"ANALOG_VIDEO_COMPONENT_OUTPUT {811C5110-46C8-4C6E-8163-C0482A15D47E}" );
DECLARE_CONST_STRING( ANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT, L"ANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT {D783A191-E083-4BAF-B2DA-E69F910B3772}" );
DECLARE_CONST_STRING( ANALOG_VIDEO_CGMSA,                   L"ANALOG_VIDEO_CGMS-A {2098DE8D-7DDD-4bab-96C6-32EBB6FABEA3}" );
DECLARE_CONST_STRING( DIGITIAL_AUDIO_SCMS,                  L"DIGITAL_AUDIO_SCMS {6D5CFA59-C250-4426-930E-FAC72C8FCFA6}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_ACP,           L"MSPR_PROTECTION_SCHEME_ACP {C3FD11C6-F8B7-4D20-B008-1DB17D61F2DA}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_CGMSA,         L"MSPR_PROTECTION_SCHEME_CGMSA {2098DE8D-7DDD-4BAB-96C6-32EBB6FABEA3}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_MAX_COMPONENT_RESOLUTION,        L"MSPR_PROTECTION_SCHEME_MAX_COMPONENT_RESOLUTION {811C5110-46C8-4C6E-8163-C0482A15D47E}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_MAX_VGA_RESOLUTION, L"MSPR_PROTECTION_SCHEME_MAX_VGA_RESOLUTION {D783A191-E083-4BAF-B2DA-E69F910B3772}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_SCMS,                  L"DIGITAL_AUDIO_SCMS {6D5CFA59-C250-4426-930E-FAC72C8FCFA6}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_DOWNSAMPLE_REQUIRED,   L"MSPR_PROTECTION_SCHEME_DOWNSAMPLE_REQUIRED {8f81a453-998c-4596-b150-f8486df6f2xe6}" );
DECLARE_CONST_STRING( MSPR_INCLUSION_LIST_CPRM,                     L"MSPR_INCLUSION_LIST_CPRM {CDD801AD-A577-48DB-950E-46D5F1592FAE}" );
DECLARE_CONST_STRING( MSPR_INCLUSION_LIST_CSS,                      L"MSPR_INCLUSION_LIST_CSS {3CAF2814-A7AB-467C-B4DF-54ACC56C66DC}" );
DECLARE_CONST_STRING( MSPR_INCLUSION_LIST_DTCP,                     L"MSPR_INCLUSION_LIST_DTCP {D685030B-0F4F-43A6-BBAD-356F1EA0049A}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_BEST_EFFORT_ACP,       L"MSPR_PROTECTION_SCHEME_BEST_EFFORT_ACP {9AD40B1F-6494-452C-9209-EE33A3FC11F9}" );
DECLARE_CONST_STRING( MSPR_PROTECTION_SCHEME_BEST_EFFORT_CGMSA,     L"MSPR_PROTECTION_SCHEME_BEST_EFFORT_CGMSA {225CD36F-F132-49EF-BA8C-C91EA28E4369}" );

DECLARE_CONST_STRING( ExecutePolicy,                        L"EXECUTE_POLICY" );
DECLARE_CONST_STRING( WM7AppPolicyType,                     L"WM7 APP POLICY" );
DECLARE_CONST_STRING( PolicyType,                           L"POLICY_TYPE" );
DECLARE_CONST_STRING( PolicyFlags,                          L"FLAGS" );
DECLARE_CONST_STRING( RIVSequenceNumber,                    L"RIV_SEQUENCE_NUMBER" );
DECLARE_CONST_STRING( AppClientID,                          L"CLIENT_ID" );
DECLARE_CONST_STRING( AppServiceID,                         L"SERVICE_ID" );
DECLARE_CONST_STRING( AppBinding,                           L"APP_BINDING" );

typedef struct _NamedGuids
{
    const DRM_GUID   *m_pGuid;
    DRM_CONST_STRING *m_pdstrGuid;
} NAMEDGUIDS;

NAMEDGUIDS _rgnamedGuids[] =
{
    { &MSPR_ACTION_PLAY_USING_DTCP,          &g_dstrPlayUsingDTCP },
    { &ANALOG_VIDEO_AGC_OUTPUT,              &g_dstrANALOG_VIDEO_AGC_OUTPUT },
    { &ANALOG_VIDEO_EXPLICIT_OUTPUT,         &g_dstrANALOG_VIDEO_EXPLICIT_OUTPUT },
    { &ANALOG_VIDEO_COMPONENT_OUTPUT,        &g_dstrANALOG_VIDEO_COMPONENT_OUTPUT },
    { &ANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT, &g_dstrANALOG_VIDEO_COMPUTER_MONITOR_OUTPUT },
    { &ANALOG_VIDEO_CGMSA,                   &g_dstrANALOG_VIDEO_CGMSA },
    { &DIGITIAL_AUDIO_SCMS,                  &g_dstrDIGITIAL_AUDIO_SCMS },
#if DRM_XMR2XMLLIB_SUPPORTWM7
    { &DRM_WM7_APPLICATION_POLICY,           &g_dstrWM7AppPolicyType },
#endif /* DRM_XMR2XMLLIB_SUPPORTWM7 */
    { &MSPR_ACTION_COPY,                                &g_dstrMSPR_ACTION_COPY },
    { &MSPR_ACTION_COPY_TO_CSS,                         &g_dstrCopyToCSS },
    { &MSPR_ACTION_COPY_TO_DEVICE,                      &g_dstrCopyToDevice },
    { &MSPR_ACTION_COPY_TO_PC,                          &g_dstrCopyToPC },
    { &MSPR_ACTION_COPY_TO_CD,                          &g_dstrCopyToCD },
    { &MSPR_ACTION_COPY_TO_ISDB,                        &g_dstrCopyToISDB },
    { &MSPR_ACTION_COPY_TO_CPRM,                        &g_dstrCopyToCPRM },
    { &MSPR_PROTECTION_SCHEME_ACP,                      &g_dstrMSPR_PROTECTION_SCHEME_ACP },
    { &MSPR_PROTECTION_SCHEME_CGMSA,                    &g_dstrMSPR_PROTECTION_SCHEME_CGMSA },
    { &MSPR_PROTECTION_SCHEME_MAX_COMPONENT_RESOLUTION, &g_dstrMSPR_PROTECTION_SCHEME_MAX_COMPONENT_RESOLUTION },
    { &MSPR_PROTECTION_SCHEME_MAX_VGA_RESOLUTION,       &g_dstrMSPR_PROTECTION_SCHEME_MAX_VGA_RESOLUTION },
    { &MSPR_PROTECTION_SCHEME_SCMS,                     &g_dstrMSPR_PROTECTION_SCHEME_SCMS },
    { &MSPR_PROTECTION_SCHEME_DOWNSAMPLE_REQUIRED,      &g_dstrMSPR_PROTECTION_SCHEME_DOWNSAMPLE_REQUIRED },
    { &MSPR_INCLUSION_LIST_CPRM,                        &g_dstrMSPR_INCLUSION_LIST_CPRM },
    { &MSPR_INCLUSION_LIST_CSS,                         &g_dstrMSPR_INCLUSION_LIST_CSS },
    { &MSPR_INCLUSION_LIST_DTCP,                        &g_dstrMSPR_INCLUSION_LIST_DTCP },
    { &MSPR_PROTECTION_SCHEME_BEST_EFFORT_ACP,          &g_dstrMSPR_PROTECTION_SCHEME_BEST_EFFORT_ACP },
    { &MSPR_PROTECTION_SCHEME_BEST_EFFORT_CGMSA,        &g_dstrMSPR_PROTECTION_SCHEME_BEST_EFFORT_CGMSA },
};

DRM_RESULT DRM_CALL _PrintNameableGuid(       DRM_BYTE          *f_pbXmlBuffer,
                                        const DRM_GUID          *pGuid,
                                        const DRM_CONST_STRING  *pdstrLabel )
{
    DRM_RESULT              dr                             = DRM_SUCCESS;
    DRM_WCHAR               rgwchGuid[DRM_GUID_STRING_LEN] = {0};
    DRM_CONST_STRING        dstrGuid                       = {0};
    const DRM_CONST_STRING *pdstrGuid                      = NULL;
    DRM_DWORD               iGuid                          = 0;

    ChkArg( f_pbXmlBuffer != NULL );
    ChkArg( pGuid != NULL );
    ChkArg( pdstrLabel != NULL );

    dstrGuid.pwszString = rgwchGuid;
    dstrGuid.cchString  = NO_OF( rgwchGuid );

    ChkDR( DRM_UTL_GuidToString( ( DRM_GUID * )pGuid,
                                 rgwchGuid ) );

    for (iGuid = 0; iGuid < NO_OF( _rgnamedGuids ); iGuid++)
    {
        if ( DRM_UTL_AreGUIDEqual ( pGuid, _rgnamedGuids[ iGuid ].m_pGuid ) )
        {
            pdstrGuid = _rgnamedGuids[ iGuid ].m_pdstrGuid;
            break;
        }
    }

    if ( pdstrGuid == NULL )
    {
        pdstrGuid = &dstrGuid;
    }

    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, pdstrLabel, pdstrGuid, NULL, NULL, wttClosed) );

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _PrintXMRObjectTags( DRM_BYTE           *f_pbXmlBuffer,
                                                DRM_DWORD           f_cbXmlBuffer,
                                                DRM_WORD            f_wType,
                                                DRM_WORD            f_wFlags )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_CONST_STRING dstrLong;
    DRM_WCHAR        rgwcNum[32];

    rgwcNum[0] = g_wch0;
    rgwcNum[1] = g_wchx;
    dstrLong.cchString  = 6;
    dstrLong.pwszString = rgwcNum;

    ChkDR( DRM_UTL_NumberToString( f_wType, &rgwcNum[2], NO_OF( rgwcNum ) - 2, 4, 16, NULL ) );

    ChkDR( DRM_XMB_AddAttribute( (_XMBContext*)f_pbXmlBuffer, &g_dstrObjectType, &dstrLong));

    ChkDR( DRM_UTL_NumberToString( f_wFlags, &rgwcNum[2], NO_OF( rgwcNum ) - 2, 4, 16, NULL ) );

    ChkDR( DRM_XMB_AddAttribute( (_XMBContext*)f_pbXmlBuffer, &g_dstrObjectFlags, &dstrLong));

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _PrintUnknownObjects(       DRM_BYTE               *f_pbXmlBuffer,
                                                       DRM_DWORD               f_cbXmlBuffer,
                                                 const DRM_XMR_UNKNOWN_OBJECT *f_pObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    while( f_pObject != NULL
        && f_pObject->fValid )
    {
        switch (f_pObject->wType)
        {
        case XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrPlayEnabler ) );
            break;
        case XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrCopyEnabler ) );
            break;
        case XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrExecutePolicy ) );
            ChkDR( _PrintXMRObjectTags( f_pbXmlBuffer, f_cbXmlBuffer, f_pObject->wType, f_pObject->wFlags ) );
            break;
        default:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrUnknownObject ) );
            ChkDR( _PrintXMRObjectTags( f_pbXmlBuffer, f_cbXmlBuffer, f_pObject->wType, f_pObject->wFlags ) );
            break;
        }

        switch (f_pObject->wType)
        {
        case XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT:
        case XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT:
            {
                const DRM_GUID *pGuid = (const DRM_GUID *)(f_pObject->pbBuffer + f_pObject->ibData);

                ChkDR( _PrintNameableGuid( f_pbXmlBuffer, pGuid, &g_dstrGuid ) );
            }
            break;

#if DRM_XMR2XMLLIB_SUPPORTWM7
        case XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT:
            ChkDR( _PrintWM7AppPolicyData( f_pObject->pbBuffer + f_pObject->ibData, f_pObject->cbData, f_pbXmlBuffer, f_cbXmlBuffer, FALSE ) );
            break;
#endif /* DRM_XMR2XMLLIB_SUPPORTWM7 */

        default:
            ChkDR( AddBLOBToXml( &g_dstrObjectPayload, f_pObject->pbBuffer + f_pObject->ibData, f_pObject->cbData, f_pbXmlBuffer, f_cbXmlBuffer, FALSE ) );
            break;
        }

        ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );

        f_pObject = f_pObject->pNext;
    }

ErrorExit:
    return dr;
}

static DRM_RESULT DRM_CALL _PrintUnknownContainers(       DRM_BYTE                  *f_pbXmlBuffer,
                                                          DRM_DWORD                  f_cbXmlBuffer,
                                                    const DRM_XMR_UNKNOWN_CONTAINER *f_pContainer )
{
    DRM_RESULT dr = DRM_SUCCESS;

    while( f_pContainer != NULL
        && f_pContainer->fValid )
    {
        switch (f_pContainer->wType)
        {
        case XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrPlayEnablerContainer ) );
            break;
        case XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrCopyEnablerContainer ) );
            break;
        case XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrExecutePolicyContainer ) );
            break;
         case XMR_OBJECT_TYPE_READ_POLICY_CONTAINER:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrReadPolicyContainer ) );
            break;
        default:
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrUnknownContainer ) );
            ChkDR( _PrintXMRObjectTags( f_pbXmlBuffer, f_cbXmlBuffer, f_pContainer->wType, f_pContainer->wFlags ) );
            break;
        }

        ChkDR( _PrintUnknownObjects( f_pbXmlBuffer, f_cbXmlBuffer, f_pContainer->pObject ) );
        ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );

        f_pContainer = f_pContainer->pNext;
    }

ErrorExit:
    return dr;
}

DRM_RESULT DateTimeToString( DRM_UINT64 f_u64DateTime, DRM_STRING *f_pdstrDateTime )
{
    DRM_RESULT    dr       = DRM_E_FAIL;
    DRMFILETIME   filetime = { 0, 0 };
    DRMSYSTEMTIME date;

    UI64_TO_FILETIME( f_u64DateTime, filetime );

    if( Oem_Clock_FileTimeToSystemTime(&filetime, &date) )
    {
        DRM_WCHAR *pch = f_pdstrDateTime->pwszString;
        DRM_DWORD  cch = f_pdstrDateTime->cchString;

        ChkArg( cch >= 5 );
        ChkDR( DRM_UTL_NumberToString( date.wYear, pch, cch, 4, 10, NULL ) );
        pch += 4;
        cch -= 4;

        ChkArg( cch >= 2 );
        *pch = g_wchForwardSlash;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 3 );
        ChkDR( DRM_UTL_NumberToString( date.wMonth, pch, cch, 2, 10, NULL ) );
        pch += 2;
        cch -= 2;

        ChkArg( cch >= 2 );
        *pch = g_wchForwardSlash;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 3 );
        ChkDR( DRM_UTL_NumberToString( date.wDay, pch, cch, 2, 10, NULL ) );
        pch += 2;
        cch -= 2;

        ChkArg( cch >= 2 );
        *pch = g_wchSpace;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 3 );
        ChkDR( DRM_UTL_NumberToString( date.wHour, pch, cch, 2, 10, NULL ) );
        pch += 2;
        cch -= 2;

        ChkArg( cch >= 2 );
        *pch = g_wchColon;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 3 );
        ChkDR( DRM_UTL_NumberToString( date.wMinute, pch, cch, 2, 10, NULL ) );
        pch += 2;
        cch -= 2;

        ChkArg( cch >= 2 );
        *pch = g_wchColon;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 3 );
        ChkDR( DRM_UTL_NumberToString( date.wSecond, pch, cch, 2, 10, NULL ) );
        pch += 2;
        cch -= 2;

        ChkArg( cch >= 2 );
        *pch = g_wchSpace;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 2 );
        *pch = g_wchG;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 2 );
        *pch = g_wchM;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 2 );
        *pch = g_wchT;
        pch += 1;
        cch -= 1;

        ChkArg( cch >= 2 );
        *pch = g_wchNull;
        pch += 1;
        cch -= 1;

        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( f_pdstrDateTime->pwszString ), &f_pdstrDateTime->cchString ) );
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

DRM_RESULT AddGenericValueToXml( const DRM_BYTE             *f_pbValue,
                                       DRM_DWORD             f_cbValue,
                                 const DRM_CONST_STRING     *f_pdstrTagName,
                                       DRM_BYTE             *f_pbXml,
                                       DRM_DWORD             f_cbXml,
                                       DRM_BOOL              f_fAlsoDisplayAsDecimal,
                                       DRM_BOOL              f_fInNetworkByteOrder)
{
    DRM_RESULT dr          = DRM_E_FAIL;
    DRM_STRING dstrEncoded = EMPTY_DRM_STRING;
    DRM_STRING dstrID      = EMPTY_DRM_STRING;

    if( f_cbValue == SIZEOF(DRM_WORD) )
    {
        /*
        ** Assume it should be displayed as a WORD
        */

        DRM_CONST_STRING dstrLong;
        DRM_WCHAR        awchNum[64] = {0};
        DRM_WORD         wValue      = 0;
        DRM_WORD         wDecimal    = 0;

        wValue = *(const DRM_WORD *)f_pbValue;
        if( !f_fInNetworkByteOrder )
        {
            wDecimal = wValue;
        }
        else
        {
            NETWORKBYTES_TO_WORD( wDecimal, &wValue, 0 );
        }

        awchNum[0] = g_wch0;
        awchNum[1] = g_wchx;
        ChkDR( DRM_UTL_NumberToString( wValue, &awchNum[2], NO_OF( awchNum ) - 2, 4, 16, NULL ) );
        if (f_fAlsoDisplayAsDecimal)
        {
            DRM_DWORD cchUsed = 0;
            awchNum[6] = g_wchSpace;
            awchNum[7] = g_wchOpenParen;
            ChkDR( DRM_UTL_NumberToString( wDecimal, &awchNum[8], NO_OF( awchNum ) - 8, 0, 10, &cchUsed ) );
            awchNum[8+cchUsed] = g_wchCloseParen;
        }
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( awchNum ), &dstrLong.cchString ) );
        dstrLong.pwszString = awchNum;

        ChkDR( DRM_XMB_WriteTag( (_XMBContext*)f_pbXml, f_pdstrTagName, &dstrLong, NULL, NULL, wttClosed));                /*   <VALUE>long_value</VALUE>   */
    }
    else if( f_cbValue == SIZEOF(DRM_DWORD) )
    {
        /*
        ** Assume it should be displayed as a DWORD
        */

        DRM_CONST_STRING dstrLong;
        DRM_WCHAR        awchNum[64] = {0};
        DRM_DWORD        dwValue     = 0;
        DRM_DWORD        dwDecimal   = 0;

        dwValue = *(const DRM_DWORD *)f_pbValue;
        if( !f_fInNetworkByteOrder )
        {
            dwDecimal = dwValue;
        }
        else
        {
            NETWORKBYTES_TO_DWORD( dwDecimal, &dwValue, 0 );
        }

        awchNum[0] = g_wch0;
        awchNum[1] = g_wchx;
        ChkDR( DRM_UTL_NumberToString( dwValue, &awchNum[2], NO_OF( awchNum ) - 2, 8, 16, NULL ) );
        if (f_fAlsoDisplayAsDecimal)
        {
            DRM_DWORD cchUsed = 0;
            awchNum[10] = g_wchSpace;
            awchNum[11] = g_wchOpenParen;
            ChkDR( DRM_UTL_NumberToString( dwDecimal, &awchNum[12], NO_OF( awchNum ) - 12, 0, 10, &cchUsed ) );
            awchNum[12+cchUsed] = g_wchCloseParen;
        }
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( awchNum ), &dstrLong.cchString ) );
        dstrLong.pwszString = awchNum;

        ChkDR( DRM_XMB_WriteTag( (_XMBContext*)f_pbXml, f_pdstrTagName, &dstrLong, NULL, NULL, wttClosed));                /*   <VALUE>long_value</VALUE>   */
    }
    else if( f_cbValue == SIZEOF(DRM_BYTE) )
    {
        /*
        ** Assume it should be displayed as a BYTE
        */

        DRM_CONST_STRING dstrValue;
        DRM_WCHAR        awchNum[64] = {0};
        DRM_BYTE         bValue      = 0;

        bValue = *f_pbValue;

        awchNum[0] = g_wch0;
        awchNum[1] = g_wchx;
        ChkDR( DRM_UTL_NumberToString( bValue, &awchNum[2], NO_OF( awchNum ) - 2, 4, 16, NULL ) );
        if (f_fAlsoDisplayAsDecimal)
        {
            DRM_DWORD cchUsed = 0;
            awchNum[6] = g_wchSpace;
            awchNum[7] = g_wchOpenParen;
            ChkDR( DRM_UTL_NumberToString( bValue, &awchNum[8], NO_OF( awchNum ) - 8, 0, 10, &cchUsed ) );
            awchNum[8+cchUsed] = g_wchCloseParen;
        }
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( awchNum ), &dstrValue.cchString ) );
        dstrValue.pwszString = awchNum;

        ChkDR( DRM_XMB_WriteTag( (_XMBContext*)f_pbXml, f_pdstrTagName, &dstrValue, NULL, NULL, wttClosed));
    }
    else
    {
        DRM_DWORD        cchEncoded  = CCH_BASE64_EQUIV( f_cbValue ) + 1;
        DRM_DWORD        cbEncoded   = cchEncoded * SIZEOF(DRM_WCHAR);
        DRM_WCHAR       *pch         = NULL;

        ChkMem( dstrEncoded.pwszString = (DRM_WCHAR *) Oem_MemAlloc( cbEncoded ) );

        DRMCRT_memset( dstrEncoded.pwszString, 0, cbEncoded );

        dstrEncoded.cchString = cchEncoded;

        ChkDR( DRM_B64_EncodeW( (DRM_BYTE*)f_pbValue,
                                f_cbValue,
                                dstrEncoded.pwszString,
                                &(dstrEncoded.cchString),
                                0 ) );

        if( f_cbValue == SIZEOF(DRM_ID) )
        {
            DRM_WCHAR rgwchGuid[64] = { 0 };
            DRM_DWORD cb = 0;

            ChkDR( DRM_UTL_GuidToString( (DRM_GUID*)f_pbValue, rgwchGuid ) );

            ChkDR( DRM_SizeTToDWord( ( dstrEncoded.cchString + DRMCRT_wcslen( rgwchGuid ) + 4 ) * SIZEOF( DRM_WCHAR ), &cb ) );
            ChkMem( dstrID.pwszString = (DRM_WCHAR *) Oem_MemAlloc( cb ) );
            ZEROMEM( dstrID.pwszString, cb );

            pch = dstrID.pwszString;
            ChkDR( DRM_SizeTToDWord( dstrEncoded.cchString * SIZEOF( DRM_WCHAR ), &cb ) );
            DRMCRT_memcpy( pch, dstrEncoded.pwszString, cb );
            pch += cb / SIZEOF( DRM_WCHAR );

            *pch = g_wchSpace;
            pch++;
            *pch = g_wchOpenParen;
            pch++;
            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( rgwchGuid ) * SIZEOF( DRM_WCHAR ), &cb ) );
            DRMCRT_memcpy( pch, rgwchGuid, cb );
            pch += cb / SIZEOF( DRM_WCHAR );
            *pch = g_wchCloseParen;
            pch++;
            *pch = g_wchNull;
            pch++;

            ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( dstrID.pwszString ), &dstrID.cchString ) );

            ChkDR( DRM_XMB_WriteTag( (_XMBContext*)f_pbXml, f_pdstrTagName, (DRM_CONST_STRING*)&dstrID, NULL, NULL, wttClosed));                /*   <VALUE>long_value</VALUE>  */
        }
        else
        {
            ChkDR( DRM_XMB_WriteTag( (_XMBContext*)f_pbXml, f_pdstrTagName, (DRM_CONST_STRING*)&dstrEncoded, NULL, NULL, wttClosed));                /*   <VALUE>B64</VALUE>    */
        }
    }

    dr = DRM_SUCCESS;

ErrorExit:

    if( dstrEncoded.pwszString != NULL )
    {
        Oem_MemFree( dstrEncoded.pwszString );
    }
    if( dstrID.pwszString != NULL )
    {
        Oem_MemFree( dstrID.pwszString );
    }

    return dr;
}

DRM_RESULT AddBLOBToXml(
    const DRM_CONST_STRING *f_pdstrTag,
    const DRM_BYTE         *f_pbTokenData,
          DRM_DWORD         f_cbTokenData,
          DRM_BYTE         *f_pbXmlBuffer,
          DRM_DWORD         f_cbXmlBuffer,
          DRM_BOOL          f_fB64Encode )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrData = EMPTY_DRM_STRING;
    DRM_WCHAR *pwchEncoded = NULL;
    DRM_DWORD cwchEncoded = 0;

    ChkDRMString( f_pdstrTag );
    ChkArg( f_pbTokenData != NULL );
    ChkArg( f_pbXmlBuffer != NULL );

    if( f_fB64Encode )
    {
        cwchEncoded = CCH_BASE64_EQUIV( f_cbTokenData );

        ChkMem( pwchEncoded = ( DRM_WCHAR * )Oem_MemAlloc( cwchEncoded * SIZEOF( DRM_WCHAR ) ) );

        ChkDR( DRM_B64_EncodeW( f_pbTokenData,
                                f_cbTokenData,
                                pwchEncoded,
                                &cwchEncoded,
                                0 ) );

        dstrData.pwszString = pwchEncoded;
        dstrData.cchString = cwchEncoded;
    }
    else
    {
        DRM_DWORD i = 0;
        DRM_WCHAR *pch = NULL;
        ChkMem( pwchEncoded = ( DRM_WCHAR * )Oem_MemAlloc( ( f_cbTokenData * 3 + 1 ) * SIZEOF( DRM_WCHAR ) ) );

        pch = pwchEncoded;
        for( i = 0; i < f_cbTokenData; i++ )
        {
            ChkDR( DRM_UTL_NumberToString( f_pbTokenData[ i ], pch, 3, 2, 16, NULL ) );
            pch[2] = g_wchSpace;
            pch += 3;
        }
        *pch = g_wchNull;

        dstrData.pwszString = pwchEncoded;
        ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( pwchEncoded ), &dstrData.cchString ) );
    }

    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer,
                             f_pdstrTag,
                             &dstrData,
                             NULL,
                             NULL,
                             wttClosed ) );

ErrorExit:
    SAFE_OEM_FREE( pwchEncoded );
    return dr;
}

DRM_RESULT FormatXmrAsXml(
    __in_ecount( f_cbXMR ) const DRM_BYTE       *f_pbXMR,
    __in                         DRM_DWORD       f_cbXMR,
    __out_ecount(f_cbXmlBuffer)  DRM_BYTE       *f_pbXmlBuffer,
    __in                         DRM_DWORD       f_cbXmlBuffer,
    __out_ecount(1)              DRM_STRING     *f_pdstrXml )
{
    DRM_RESULT      dr         = DRM_E_FAIL;
    DRM_XMR_LICENSE xmrLicense = {0};
    DRM_BYTE  *pbStack = NULL;
    DRM_DWORD  cbStack = 1024;
    DRM_STACK_ALLOCATOR_CONTEXT stack = { 0 };
    DRM_WCHAR rgwchDateTime[100];
    DRM_WCHAR rgwchGuid[38];

    ChkArg( f_pbXMR != NULL
        &&  f_pbXmlBuffer != NULL
        &&  f_cbXmlBuffer >  0
        &&  f_pdstrXml    != NULL );

    ChkMem( pbStack = (DRM_BYTE*)Oem_MemAlloc( cbStack ) );

    stack.cbStack = cbStack;
    stack.pbStack = pbStack;

    /*
    ** The license store prefixes the license with the 4-byte value of Priority
    */

    ChkDR( DRM_XMR_UnpackLicense( f_pbXMR, f_cbXMR, &stack, &xmrLicense  ) );

    ChkDR( DRM_XMB_CreateDocument( f_cbXmlBuffer, ( _XMBContext * )f_pbXmlBuffer, &g_dstrTagLicense ) );  /* <LICENSE> */

    ChkDR( AddGenericValueToXml( xmrLicense.pbRightsIdBuffer + xmrLicense.iRightsId, sizeof( DRM_ID ), &g_dstrRightsIdTag, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );
    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.dwVersion, sizeof( xmrLicense.dwVersion ), &g_dstrVersionTag,  f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );

    if( xmrLicense.containerOuter.fValid )
    {
        DRMFILETIME ftXMR       = {0};
        DRM_UINT64  ui64XMR     = DRM_UI64LITERAL( 0, 0 );

        ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrContainerOuter ) );

        if( xmrLicense.containerOuter.containerGlobalPolicies.fValid )
        {

            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrGlobalPoliciesContainer ) );

            if( xmrLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.fValid )
            {
                DRM_DWORD dwTemp = xmrLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel;

                if ( ( xmrLicense.dwVersion == 1 ) || ( xmrLicense.dwVersion == 2 ) )
                {
                    ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrMinimumEnvironment ) );
                }
                else
                {
                    ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrSecurityLevelContainer ) );
                }

                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumAppRevocationListVersion,    sizeof( DRM_DWORD ), &g_dstrMinimumAppRevocationListVersion,    f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumDeviceRevocationListVersion, sizeof( DRM_DWORD ), &g_dstrMinimumDeviceRevocationListVersion, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&dwTemp,                                                                                                    sizeof( DRM_DWORD ), &g_dstrMinimumSecurityLevel,               f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.SerialNumber.fValid )
            {
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerGlobalPolicies.SerialNumber.pbDataBuffer +
                                             xmrLicense.containerOuter.containerGlobalPolicies.SerialNumber.iData,
                                             xmrLicense.containerOuter.containerGlobalPolicies.SerialNumber.cbData,
                                            &g_dstrSerialNumber,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.Rights.fValid )
            {
                DRM_BOOL  fSet = FALSE;
                DRM_WCHAR rgwchRightsString[500] = {0};
                DRM_STRING dstrRights;

                if( XMR_RIGHTS_CANNOT_PERSIST & xmrLicense.containerOuter.containerGlobalPolicies.Rights.wValue )
                {
                    ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L"XMR_RIGHTS_CANNOT_PERSIST" ) );
                    fSet = TRUE;
                }

                if( XMR_RIGHTS_CANNOT_BIND_LICENSE & xmrLicense.containerOuter.containerGlobalPolicies.Rights.wValue )
                {
                    if( fSet )
                    {
                        ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L" | " ) );
                    }
                    ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L"XMR_RIGHTS_CANNOT_BIND_LICENSE" ) );
                    fSet = TRUE;
                }

                if( XMR_RIGHTS_ALLOW_BACKUP_RESTORE & xmrLicense.containerOuter.containerGlobalPolicies.Rights.wValue )
                {
                    if( fSet )
                    {
                        ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L" | " ) );
                    }
                    ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L"XMR_RIGHTS_ALLOW_BACKUP_RESTORE" ) );
                    fSet = TRUE;
                }

                /*
                ** It's possible that XMR_RIGHTS_COLLABORATIVE_PLAY right
                **  doesn't exist.
                ** However, PDRM licenses that supported PLAY
                **  always supported COLLABORATIVE_PLAY too.
                ** So if this license supports play AND
                **  it was converted from PDRM,
                **  then we allow use of the basic PLAY right.
                ** Note: We can NOT simply change the V1->V3 conversion
                **  logic to add the COLLABORATIVE_PLAY rights
                **  to PDRM licenses because we've already shipped
                **  code that does this conversion without adding
                **  that right.  Without this actual evaluation code
                **  handling this situation, people will lose
                **  their COLLABORATIVE_PLAY rights on those licenses.
                */
                if( XMR_RIGHTS_COLLABORATIVE_PLAY & xmrLicense.containerOuter.containerGlobalPolicies.Rights.wValue
                 || ( XMR_IS_PLAY_VALID( &xmrLicense )
                   && XMR_IS_SOURCEID_VALID( &xmrLicense )
                   && xmrLicense.containerOuter.containerGlobalPolicies.SourceID.dwValue == XMR_SOURCEID_PDRM ) )
                {
                    if( fSet )
                    {
                        ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L" | " ) );
                    }
                    ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L"XMR_RIGHTS_COLLABORATIVE_PLAY" ) );
                    fSet = TRUE;
                }

                if( XMR_RIGHTS_BASE_LICENSE & xmrLicense.containerOuter.containerGlobalPolicies.Rights.wValue )
                {
                    if( fSet )
                    {
                        ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L" | " ) );
                    }
                    ChkDR( DRM_STR_StringCchCatW( rgwchRightsString, NO_OF( rgwchRightsString ), L"XMR_RIGHTS_BASE_LICENSE" ) );
                }

                dstrRights.pwszString = rgwchRightsString;
                ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( rgwchRightsString ), &dstrRights.cchString ) );

                /* It's possible that none of these flags are set. */
                if( dstrRights.cchString != 0 )
                {
                    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrRights, (DRM_CONST_STRING*)&dstrRights, NULL, NULL, wttClosed) );
                }
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.Priority.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.Priority.dwValue, sizeof( DRM_DWORD ), &g_dstrPriority, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.SourceID.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.SourceID.dwValue, sizeof( DRM_DWORD ), &g_dstrSourceID, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.Expiration.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrExpirationTag ) );
                if( xmrLicense.containerOuter.containerGlobalPolicies.Expiration.dwBeginDate != 0 )
                {
                    DRM_STRING dstrDateTime = {0};
                    dstrDateTime.pwszString = rgwchDateTime;
                    dstrDateTime.cchString = NO_OF( rgwchDateTime );
                    CREATE_FILE_TIME( xmrLicense.containerOuter.containerGlobalPolicies.Expiration.dwBeginDate, ftXMR );
                    FILETIME_TO_UI64( ftXMR, ui64XMR );
                    dr = DateTimeToString( ui64XMR, &dstrDateTime );
                    if ( DRM_SUCCEEDED( dr ) )
                    {
                        ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrBeginTime, (DRM_CONST_STRING*) &dstrDateTime, NULL, NULL, wttClosed) );
                    }
                    else
                    {
                        ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrBeginTime, (DRM_CONST_STRING*) &g_dstrInvalid, NULL, NULL, wttClosed) );
                    }
                }

                if( xmrLicense.containerOuter.containerGlobalPolicies.Expiration.dwEndDate != 0 )
                {
                    if ( xmrLicense.containerOuter.containerGlobalPolicies.Expiration.dwEndDate != XMR_UNLIMITED )
                    {
                        DRM_STRING dstrDateTime = {0};
                        dstrDateTime.pwszString = rgwchDateTime;
                        dstrDateTime.cchString = NO_OF( rgwchDateTime );
                        CREATE_FILE_TIME( xmrLicense.containerOuter.containerGlobalPolicies.Expiration.dwEndDate, ftXMR );
                        FILETIME_TO_UI64( ftXMR, ui64XMR );
                        dr = DateTimeToString( ui64XMR, &dstrDateTime );
                        if ( DRM_SUCCEEDED( dr ) )
                        {
                            ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEndTime, (DRM_CONST_STRING*) &dstrDateTime, NULL, NULL, wttClosed) );
                        }
                        else
                        {
                            ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEndTime, (DRM_CONST_STRING*) &g_dstrInvalid, NULL, NULL, wttClosed) );
                        }
                    }
                    else
                    {
                        ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEndTime, &g_dstrUnlimited, NULL, NULL, wttClosed) );
                    }
                }
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.RemovalDate.fValid )
            {
                DRM_STRING dstrDateTime = {0};
                dstrDateTime.pwszString = rgwchDateTime;
                dstrDateTime.cchString = NO_OF( rgwchDateTime );
                CREATE_FILE_TIME( xmrLicense.containerOuter.containerGlobalPolicies.RemovalDate.dwRemovalDate, ftXMR );
                FILETIME_TO_UI64( ftXMR, ui64XMR );
                ChkDR( DateTimeToString( ui64XMR, &dstrDateTime ) );
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrRemovalDate, (DRM_CONST_STRING*)&dstrDateTime, NULL, NULL, wttClosed) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.IssueDate.fValid )
            {
                DRM_STRING dstrDateTime = {0};
                dstrDateTime.pwszString = rgwchDateTime;
                dstrDateTime.cchString = NO_OF( rgwchDateTime );
                CREATE_FILE_TIME( xmrLicense.containerOuter.containerGlobalPolicies.IssueDate.dwValue, ftXMR );
                FILETIME_TO_UI64( ftXMR, ui64XMR );
                ChkDR( DateTimeToString( ui64XMR, &dstrDateTime ) );
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrIssueDate, (DRM_CONST_STRING*)&dstrDateTime, NULL, NULL, wttClosed) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.Metering.fValid )
            {
                DRM_STRING dstrGuid = {0};
                dstrGuid.pwszString = rgwchGuid;
                dstrGuid.cchString = NO_OF( rgwchGuid );

                ChkDR( DRM_UTL_GuidToString( ( DRM_GUID * )(&(xmrLicense.containerOuter.containerGlobalPolicies.Metering.pguidBuffer[xmrLicense.containerOuter.containerGlobalPolicies.Metering.iGuid])),
                                             rgwchGuid ) );
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrMeteringGuid, (DRM_CONST_STRING*)&dstrGuid, NULL, NULL, wttClosed) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.DomainID.fValid )
            {
                DRM_STRING dstrGuid = {0};
                dstrGuid.pwszString = rgwchGuid;
                dstrGuid.cchString = NO_OF( rgwchGuid );

                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrDomainID) );

                ChkDR( DRM_UTL_GuidToString( ( DRM_GUID * )(&(xmrLicense.containerOuter.containerGlobalPolicies.DomainID.pbAccountID[xmrLicense.containerOuter.containerGlobalPolicies.DomainID.ibAccountID])),
                                             rgwchGuid ) );
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrDomainID, (DRM_CONST_STRING*)&dstrGuid, NULL, NULL, wttClosed) );

                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.DomainID.dwRevision, sizeof( DRM_DWORD ), &g_dstrDomainRevision, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );

                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.ExpirationAfterUse.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwValue,   sizeof( DRM_DWORD ), &g_dstrExpirationAfterUse,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.ExpirationAfterStore.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.ExpirationAfterStore.dwValue, sizeof( DRM_DWORD ), &g_dstrExpirationAfterStore, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.GracePeriod.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.GracePeriod.dwValue,   sizeof( DRM_DWORD ), &g_dstrGracePeriod,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.fValid )
            {
                if (xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwVersion == 1)
                {
                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue,   sizeof( DRM_DWORD ), &g_dstrRevInfoVer1,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                }
                else if (xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwVersion == 2)
                {
                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue,   sizeof( DRM_DWORD ), &g_dstrRevInfoVer2,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                }
                else
                {
                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwVersion,   sizeof( DRM_DWORD ), &g_dstrRevInfoVerOtherVersion,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue,   sizeof( DRM_DWORD ), &g_dstrRevInfoVerOtherValue,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                }
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.PolicyMetadata.fValid )
            {
                DRM_DWORD iMetaData = 0;
                DRM_XMR_POLICY_METADATA_LIST *plist = xmrLicense.containerOuter.containerGlobalPolicies.PolicyMetadata.plistPolicyMetadataObjects;

                for( iMetaData = 0;
                     (plist != NULL) && (iMetaData < xmrLicense.containerOuter.containerGlobalPolicies.PolicyMetadata.cPolicyMetadataObjects);
                     iMetaData++, plist = plist->pNext )
                {
                    if( plist->MetadataObject.fValid )
                    {
                        DRM_STRING dstrGuid = {0};
                        dstrGuid.pwszString = rgwchGuid;
                        dstrGuid.cchString = NO_OF( rgwchGuid );

                        ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrPolicyMetaData ) );

                        ChkDR( DRM_UTL_GuidToString( ( DRM_GUID * )(&(plist->MetadataObject.pMetadataTypeGuidBuffer[plist->MetadataObject.iMetadataTypeGuid])),
                                                    rgwchGuid ) );

                        ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrGuid, (DRM_CONST_STRING*)&dstrGuid, NULL, NULL, wttClosed) );
                        ChkDR( AddGenericValueToXml( (DRM_BYTE*)plist->MetadataObject.pbPolicyDataBuffer + plist->MetadataObject.iPolicyData, plist->MetadataObject.cbPolicyData, &g_dstrValue, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );
                        ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
                    }
                }
            }

            if( xmrLicense.containerOuter.containerGlobalPolicies.InclusionList.fValid )
            {
                DRM_DWORD iGuid = 0;
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrInclusionList ) );

                for( iGuid = 0;
                     iGuid < xmrLicense.containerOuter.containerGlobalPolicies.InclusionList.cGUIDs;
                     iGuid++ )
                {
                    const DRM_GUID *pGuid = (const DRM_GUID *)(xmrLicense.containerOuter.containerGlobalPolicies.InclusionList.pguidBuffer
                        + xmrLicense.containerOuter.containerGlobalPolicies.InclusionList.iGuids
                        + (iGuid * SIZEOF( DRM_GUID ) ) );

                    ChkDR( _PrintNameableGuid( f_pbXmlBuffer, pGuid, &g_dstrGuid ) );
                }

                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }

            ChkDR( AddGenericValueToXml( xmrLicense.pbRightsIdBuffer + xmrLicense.iRightsId, sizeof( DRM_ID ), &g_dstrRightsIdTag, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );

            if ( xmrLicense.containerOuter.containerGlobalPolicies.EmbeddingBehavior.fValid )
            {
                if ( xmrLicense.containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue == XMR_EMBEDDING_BEHAVIOR_IGNORE )
                {
                    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEmbeddingBehaviorTag, &g_dstrIgnore, NULL, NULL, wttClosed) );
                }
                else if ( xmrLicense.containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue == XMR_EMBEDDING_BEHAVIOR_COPY )
                {
                    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEmbeddingBehaviorTag, &g_dstrCopy, NULL, NULL, wttClosed) );
                }
                else if ( xmrLicense.containerOuter.containerGlobalPolicies.EmbeddingBehavior.wValue == XMR_EMBEDDING_BEHAVIOR_MOVE )
                {
                    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEmbeddingBehaviorTag, &g_dstrMove, NULL, NULL, wttClosed) );
                }
                else
                {
                    ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrEmbeddingBehaviorTag, &g_dstrInvalid, NULL, NULL, wttClosed) );
                }
            }

            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        if( xmrLicense.containerOuter.containerRevocation.fValid )
        {
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrRevocationContainer ) );

            if( xmrLicense.containerOuter.containerRevocation.UserID.fValid )
            {
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerRevocation.UserID.pbUserID +
                                             xmrLicense.containerOuter.containerRevocation.UserID.iUserID,
                                             xmrLicense.containerOuter.containerRevocation.UserID.cbUserID,
                                            &g_dstrUserID,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
            }
            if( xmrLicense.containerOuter.containerRevocation.RsaLicenseGranterKey.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrRsaLicenseGranterKey) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerRevocation.RsaLicenseGranterKey.dwExponent, sizeof( DRM_DWORD ), &g_dstrExponent, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerRevocation.RsaLicenseGranterKey.pbModulusBuffer +
                                             xmrLicense.containerOuter.containerRevocation.RsaLicenseGranterKey.iModulus,
                                             xmrLicense.containerOuter.containerRevocation.RsaLicenseGranterKey.cbModulus,
                                            &g_dstrModulus,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        if( xmrLicense.containerOuter.containerPlaybackPolicies.fValid )
        {

            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrPlaybackPoliciesContainer ) );
            if( xmrLicense.containerOuter.containerPlaybackPolicies.PlayCount.fValid
             && xmrLicense.containerOuter.containerPlaybackPolicies.PlayCount.dwValue != XMR_UNLIMITED )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.PlayCount.dwValue, sizeof( DRM_DWORD ), &g_dstrPlayCount, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }
            else
            {
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrPlayCount, &g_dstrUnlimited, NULL, NULL, wttClosed) );
            }

            if( xmrLicense.containerOuter.containerPlaybackPolicies.opl.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrOPL ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.opl.wCompressedDigitalVideo,    sizeof( DRM_WORD ), &g_dstrCompressedDigitalVideo,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.opl.wUncompressedDigitalVideo,  sizeof( DRM_WORD ), &g_dstrUncompressedDigitalVideo, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.opl.wAnalogVideo,               sizeof( DRM_WORD ), &g_dstrAnalogVideo,              f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.opl.wCompressedDigitalAudio,    sizeof( DRM_WORD ), &g_dstrCompressedDigitalAudio,   f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.opl.wUncompressedDigitalAudio,  sizeof( DRM_WORD ), &g_dstrUncompressedDigitalAudio, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitAnalogVideoProtection.fValid )
            {
                DRM_DWORD iProtectionID = 0;
                DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST *pConfig = xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitAnalogVideoProtection.plistOutputConfiguration;

                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrExplicitAnalogVideoProtection ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitAnalogVideoProtection.cOutputProtectionIDs, sizeof( DRM_DWORD ), &g_dstrCount, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                for( iProtectionID = 0;
                     iProtectionID < xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitAnalogVideoProtection.cOutputProtectionIDs
                  && pConfig != NULL;
                     iProtectionID++, pConfig = pConfig->pNext )
                {
                    const DRM_GUID *pGuid = (const DRM_GUID *)(&(pConfig->Config.pguidBuffer[pConfig->Config.iGuid]));

                    ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrConfig ) );

                    ChkDR( _PrintNameableGuid( f_pbXmlBuffer, pGuid, &g_dstrGuid ) );

                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)pConfig->Config.pbConfigDataBuffer + pConfig->Config.iConfigData, pConfig->Config.cbConfigData, &g_dstrValue, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, TRUE ) );
                    ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
                }
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitDigitalAudioProtection.fValid )
            {
                DRM_DWORD iProtectionID = 0;
                DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST *pConfig = xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitDigitalAudioProtection.plistOutputConfiguration;

                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrExplicitDigitalAudioProtection ) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitDigitalAudioProtection.cOutputProtectionIDs, sizeof( DRM_DWORD ), &g_dstrCount, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                for( iProtectionID = 0;
                     iProtectionID < xmrLicense.containerOuter.containerPlaybackPolicies.containerExplicitDigitalAudioProtection.cOutputProtectionIDs
                  && pConfig != NULL;
                     iProtectionID++, pConfig = pConfig->pNext )
                {
                    const DRM_GUID *pGuid = (const DRM_GUID *)(&(pConfig->Config.pguidBuffer[pConfig->Config.iGuid]));

                    ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrConfig ) );

                    ChkDR( _PrintNameableGuid( f_pbXmlBuffer, pGuid, &g_dstrGuid ) );

                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)pConfig->Config.pbConfigDataBuffer + pConfig->Config.iConfigData, pConfig->Config.cbConfigData, &g_dstrValue, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, TRUE ) );
                    ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
                }
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }

            ChkDR( _PrintUnknownObjects( f_pbXmlBuffer, f_cbXmlBuffer, xmrLicense.containerOuter.containerPlaybackPolicies.pUnknownObjects ) );
            ChkDR( _PrintUnknownContainers( f_pbXmlBuffer, f_cbXmlBuffer, &xmrLicense.containerOuter.containerPlaybackPolicies.UnknownContainer ) );

            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        if( xmrLicense.containerOuter.containerCopyPolicies.fValid )
        {
            if (xmrLicense.containerOuter.containerCopyPolicies.dwVersion == 1)
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrCopyPoliciesContainer ) );
            }
            else
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrCopyPolicies2Container ) );
            }
            if( xmrLicense.containerOuter.containerCopyPolicies.CopyCount.fValid
             && xmrLicense.containerOuter.containerCopyPolicies.CopyCount.dwValue != XMR_UNLIMITED )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerCopyPolicies.CopyCount.dwValue, sizeof( DRM_DWORD ), &g_dstrCopyCount, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }
            else
            {
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrCopyCount, &g_dstrUnlimited, NULL, NULL, wttClosed) );
            }
            if( xmrLicense.containerOuter.containerCopyPolicies.CopyProtectionLevel.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerCopyPolicies.CopyProtectionLevel.wValue, sizeof( DRM_WORD ), &g_dstrCopyProtectionLevel, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }
            if ( xmrLicense.containerOuter.containerCopyPolicies.MoveEnabler.fValid )
            {
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerCopyPolicies.MoveEnabler.dwValue, sizeof( DRM_DWORD ), &g_dstrMoveEnabler, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
            }

            ChkDR( _PrintUnknownObjects( f_pbXmlBuffer, f_cbXmlBuffer, xmrLicense.containerOuter.containerCopyPolicies.pUnknownObjects ) );
            ChkDR( _PrintUnknownContainers( f_pbXmlBuffer, f_cbXmlBuffer, &xmrLicense.containerOuter.containerCopyPolicies.UnknownContainer ) );

            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        if( xmrLicense.containerOuter.containerPlaylistBurnPolicies.fValid )
        {
            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrPlaylistBurnPoliciesContainer ) );
            if( xmrLicense.containerOuter.containerPlaylistBurnPolicies.Restrictions.fValid )
            {
                if( xmrLicense.containerOuter.containerPlaylistBurnPolicies.Restrictions.dwMaxPlaylistBurnCount != XMR_UNLIMITED )
                {
                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaylistBurnPolicies.Restrictions.dwMaxPlaylistBurnCount,   sizeof( DRM_DWORD ), &g_dstrMaxPlaylistBurnCount, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                }

                if( xmrLicense.containerOuter.containerPlaylistBurnPolicies.Restrictions.dwPlaylistBurnTrackCount != XMR_UNLIMITED )
                {
                    ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerPlaylistBurnPolicies.Restrictions.dwPlaylistBurnTrackCount, sizeof( DRM_DWORD ), &g_dstrTrackBurnCount, f_pbXmlBuffer, f_cbXmlBuffer, TRUE, FALSE ) );
                }
            }

            ChkDR( _PrintUnknownObjects( f_pbXmlBuffer, f_cbXmlBuffer, xmrLicense.containerOuter.containerPlaylistBurnPolicies.pUnknownObjects ) );

            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        if( xmrLicense.containerOuter.containerUnknown.fValid )
        {
            const DRM_XMR_UNKNOWN_CONTAINER *pUnkCont = &xmrLicense.containerOuter.containerUnknown;

            ChkDR( _PrintUnknownContainers( f_pbXmlBuffer, f_cbXmlBuffer, pUnkCont ) );
        }

        if( xmrLicense.containerOuter.containerKeys.fValid )
        {
            const DRM_CONST_STRING *pdstrType = NULL;

            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrKeysContainer ) );
            if( xmrLicense.containerOuter.containerKeys.ContentKey.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrContentKey ) );
                switch( xmrLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType )
                {
                case XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR: pdstrType = &g_dstrAES_CTR; break;
                case XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB: pdstrType = &g_dstrAES_ECB; break;
                case XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER : pdstrType = &g_dstrRC4_Cocktail; break;
                case XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL   : pdstrType = &g_dstrRC4_Cocktail; break;
                default:                                        pdstrType = &g_dstrInvalid; break;
                }
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrSymmetricCipherType, pdstrType, NULL, NULL, wttClosed) );

                switch( xmrLicense.containerOuter.containerKeys.ContentKey.wKeyEncryptionCipherType )
                {
                case XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024:              pdstrType = &g_dstrRSA_1024; break;
                case XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE:       pdstrType = &g_dstrChained; break;
                case XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256:               pdstrType = &g_dstrECC_256; break;
                case XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ:       pdstrType = &g_dstrECC_256_WITH_KZ; break;
                default:                                                   pdstrType = &g_dstrInvalid; break;
                }
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrKeyEncryptionType, pdstrType, NULL, NULL, wttClosed) );

                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.ContentKey.pbguidKeyID +
                                             xmrLicense.containerOuter.containerKeys.ContentKey.iguidKeyID,
                                             sizeof( DRM_ID ),
                                            &g_dstrKeyIdTag,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.ContentKey.pbEncryptedKeyBuffer +
                                             xmrLicense.containerOuter.containerKeys.ContentKey.iEncryptedKey,
                                             xmrLicense.containerOuter.containerKeys.ContentKey.cbEncryptedKey,
                                            &g_dstrEncryptedKeyTag,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerKeys.OptimizedContentKey.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrOptimizedContentKey) );
                switch( xmrLicense.containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType )
                {
                case XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB:       pdstrType = &g_dstrAES_ECB;     break;
                case XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB_SLK:   pdstrType = &g_dstrAESSLK;  break;
                default:                                                  pdstrType = &g_dstrInvalid; break;
                }
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrKeyEncryptionType, pdstrType, NULL, NULL, wttClosed) );

                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer +
                                             xmrLicense.containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey,
                                             xmrLicense.containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey,
                                            &g_dstrEncryptedKeyTag,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerKeys.DeviceKey.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrDeviceKey) );
                ChkDR( AddGenericValueToXml( (DRM_BYTE*)&xmrLicense.containerOuter.containerKeys.DeviceKey.dwExponent, sizeof( DRM_DWORD ), &g_dstrExponent, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.DeviceKey.pbModulusBuffer +
                                             xmrLicense.containerOuter.containerKeys.DeviceKey.iModulus,
                                             xmrLicense.containerOuter.containerKeys.DeviceKey.cbModulus,
                                            &g_dstrModulus,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerKeys.ECCKey.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrECCDeviceKey) );

                switch( xmrLicense.containerOuter.containerKeys.ECCKey.wEccCurveType )
                {
                    case XMR_ECC_CURVE_TYPE_P256: pdstrType = &g_dstrP256; break;
                    default:                      pdstrType = &g_dstrInvalid; break;
                }
                ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrECCCurveType, pdstrType, NULL, NULL, wttClosed) );

                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.ECCKey.pbKeyData +
                                             xmrLicense.containerOuter.containerKeys.ECCKey.iKeyData,
                                             xmrLicense.containerOuter.containerKeys.ECCKey.cbKeyData,
                                            &g_dstrKeyData,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerKeys.UplinkKid.fValid )
            {
                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrUplinkKey) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.UplinkKid.pbguidUplinkKID +
                                             xmrLicense.containerOuter.containerKeys.UplinkKid.iguidUplinkKID,
                                             sizeof( DRM_ID ),
                                            &g_dstrUplinkKidTag,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.UplinkKid.pbChainedCheckSumBuffer +
                                             xmrLicense.containerOuter.containerKeys.UplinkKid.iChainedCheckSum,
                                             xmrLicense.containerOuter.containerKeys.UplinkKid.cbChainedCheckSum,
                                            &g_dstrUplinkChainedChecksum,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerKeys.UplinkX.fValid )
            {
                DRM_DWORD i = 0;
                DRM_DWORD dwLocationLittleEndian = 0;
                DRM_DWORD *poEntries = xmrLicense.containerOuter.containerKeys.UplinkX.pdwLocation;

                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrUplinkX) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.UplinkX.pbguidUplinkKID +
                                             xmrLicense.containerOuter.containerKeys.UplinkX.iguidUplinkKID,
                                             sizeof( DRM_ID ),
                                            &g_dstrUplinkKidTag,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );
                ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.containerKeys.UplinkX.pbCheckSumBuffer +
                                             xmrLicense.containerOuter.containerKeys.UplinkX.iCheckSum,
                                             xmrLicense.containerOuter.containerKeys.UplinkX.cbCheckSum,
                                            &g_dstrUplinkChecksum,
                                             f_pbXmlBuffer,
                                             f_cbXmlBuffer,
                                             FALSE,
                                             FALSE ) );

                for ( i = 0; i < xmrLicense.containerOuter.containerKeys.UplinkX.cEntries; i++ )
                {
                    NETWORKBYTES_TO_DWORD( dwLocationLittleEndian, &poEntries[ i ], 0 );

                    ChkDR( AddGenericValueToXml( ( DRM_BYTE * )&dwLocationLittleEndian,
                                                 SIZEOF( DRM_DWORD ),
                                                 &g_dstrLocation,
                                                 f_pbXmlBuffer,
                                                 f_cbXmlBuffer,
                                                 FALSE,
                                                 FALSE ) );
                }
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            if( xmrLicense.containerOuter.containerKeys.AuxKey.fValid )
            {
                DRM_DWORD i = 0;
                DRM_DWORD dwLocationLittleEndian = 0;
                DRM_XMR_AUX_KEY_ENTRY *poEntries = xmrLicense.containerOuter.containerKeys.AuxKey.pEntries;

                ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrAuxKey) );

                for ( i = 0; i < xmrLicense.containerOuter.containerKeys.AuxKey.cEntries; i++ )
                {
                    ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrAuxKeyEntry) );

                    NETWORKBYTES_TO_DWORD( dwLocationLittleEndian, &poEntries[ i ].dwLocation, 0 );
                    ChkDR( AddGenericValueToXml( ( DRM_BYTE * )&dwLocationLittleEndian,
                                                 SIZEOF( DRM_DWORD ),
                                                 &g_dstrLocation,
                                                 f_pbXmlBuffer,
                                                 f_cbXmlBuffer,
                                                 FALSE,
                                                 FALSE ) );
                    ChkDR( AddGenericValueToXml( poEntries[ i ].rgbKey,
                                                 DRM_AES_KEYSIZE_128,
                                                 &g_dstrValue,
                                                 f_pbXmlBuffer,
                                                 f_cbXmlBuffer,
                                                 FALSE,
                                                 FALSE ) );
                    ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
                }
                ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
            }
            ChkDR( AddGenericValueToXml( xmrLicense.pbRightsIdBuffer + xmrLicense.iRightsId, sizeof( DRM_ID ), &g_dstrRightsIdTag, f_pbXmlBuffer, f_cbXmlBuffer, FALSE, FALSE ) );
            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        if( xmrLicense.containerOuter.signature.fValid )
        {
            const DRM_CONST_STRING *pdstrType = NULL;

            ChkDR( DRM_XMB_OpenNode( ( _XMBContext * )f_pbXmlBuffer, &g_dstrSignature ) );
            switch( xmrLicense.containerOuter.signature.wType )
            {
            case XMR_SIGNATURE_TYPE_AES_128_OMAC: pdstrType = &g_dstrAES128_OMAC; break;
            case XMR_SIGNATURE_TYPE_SHA_256_HMAC: pdstrType = &g_dstrSHA256_HMAC; break;
            default:                              pdstrType = &g_dstrInvalid; break;
            }
            ChkDR( DRM_XMB_WriteTag( ( _XMBContext * )f_pbXmlBuffer, &g_dstrSignatureType, pdstrType, NULL, NULL, wttClosed) );

            ChkDR( AddGenericValueToXml( xmrLicense.containerOuter.signature.pbSignatureBuffer +
                                         xmrLicense.containerOuter.signature.iSignature,
                                         xmrLicense.containerOuter.signature.cbSignature,
                                        &g_dstrValue,
                                         f_pbXmlBuffer,
                                         f_cbXmlBuffer,
                                         FALSE,
                                         FALSE ) );

            ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
        }

        ChkDR( DRM_XMB_CloseCurrNode( ( _XMBContext * )f_pbXmlBuffer, NULL ) );
    }

    ChkDR( DRM_XMB_CloseDocument( ( _XMBContext * )f_pbXmlBuffer, f_pdstrXml ) );    /* </LICENSE> */
    dr = DRM_SUCCESS;

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        if( f_pbXmlBuffer != NULL && ( f_cbXmlBuffer >= SIZEOF( DRM_WCHAR ) * ( DRMCRT_wcslen( g_dstrParsingError.pwszString ) + 1 ) ) )
        {
            ZEROMEM( f_pbXmlBuffer, f_cbXmlBuffer );
            DRMCRT_memcpy( f_pbXmlBuffer, g_dstrParsingError.pwszString, DRMCRT_wcslen( g_dstrParsingError.pwszString ) );
            /*
            ** We are directly indexing to where we know the XXXXXXXX string appears in ParsingError.
            ** If the string changes, then the index 15 has to change too or the string won't come out right.
            */
            (DRM_VOID) DRM_UTL_NumberToString( (DRM_DWORD)dr, &((DRM_WCHAR*)f_pbXmlBuffer)[15], 9, 8, 16, NULL );
            (DRM_VOID) DRM_SizeTToDWord( DRMCRT_wcslen( (const DRM_WCHAR *) f_pbXmlBuffer ), &f_pdstrXml->cchString );
            f_pdstrXml->pwszString = (DRM_WCHAR *) f_pbXmlBuffer;
        }
        dr = DRM_SUCCESS;
    }
    if( pbStack != NULL )
    {
        Oem_MemFree( pbStack );
    }
    return dr;
}

