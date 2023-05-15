/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _TSTRIGHTSPARSER_H_
#define _TSTRIGHTSPARSER_H_

#include <drmcommon.h>
#include <drmlicense.h>

#define IsRightSet(x) \
    (x.cchString > 0 && x.pwszString[0] != 0)

#define IsRightTrue(x)                                              \
    ( ( x.cchString > 0 )                                           \
   && ( DRMCRT_towlower( x.pwszString[0] ) == WCHAR_CAST('t') )     \
   && ( DRMCRT_towlower( x.pwszString[1] ) == WCHAR_CAST('r') )     \
   && ( DRMCRT_towlower( x.pwszString[2] ) == WCHAR_CAST('u') )     \
   && ( DRMCRT_towlower( x.pwszString[3] ) == WCHAR_CAST('e') )     \
   && ( DRMCRT_towlower( x.pwszString[4] ) == WCHAR_CAST('\0') ) )

DRM_RESULT DRM_CALL CopyDRMString( __out DRM_CONST_STRING *pdstrTarget, __in const DRM_CONST_STRING *pdstrSource );

#define RIGHTSPARSER_BUF_LEN 256

typedef struct
{
    /* XMR Object Type ID
     Must match the '<right-name> Policy Container Object' Id in the XMR spec, e.g. 'Execute Policy Container Object
     */
    DRM_DWORD          dwXMRObjectTypeId;

    /* XMRLicGen constant string used to add the right to the license */
    DRM_CONST_STRING     dstrAllowExtensibleRight;

} XMRLicGenExtensiblePolicyMapEntry;

#define g_cdwExtensiblePolicyMapMax 17
extern const XMRLicGenExtensiblePolicyMapEntry g_XMRLicGenExtensiblePolicyMap[g_cdwExtensiblePolicyMapMax];

typedef struct _LicenseRights
{
struct _LicenseRights *pLeaf;
struct _LicenseRights *pNext;
DRM_CONST_STRING Description;
DRM_CONST_STRING LicenseType;
DRM_CONST_STRING AllowPlay;
DRM_CONST_STRING PlayCount;
DRM_CONST_STRING AllowCopy;
DRM_CONST_STRING CopyCount;
DRM_CONST_STRING AllowTransferToSDMI;
DRM_CONST_STRING AllowTransferToNonSDMI;
DRM_CONST_STRING TransferCount;
DRM_CONST_STRING CopyProtectionLevel;
DRM_CONST_STRING AnalogVideo;
DRM_CONST_STRING AnalogVideoExtensions;
DRM_CONST_STRING UnCompressedDigitalVideo;
DRM_CONST_STRING UnCompressedDigitalAudio;
DRM_CONST_STRING CompressedDigitalVideo;
DRM_CONST_STRING CompressedDigitalAudio;
DRM_CONST_STRING AllowBackUpRestore;
DRM_CONST_STRING AllowBurnToCD;
DRM_CONST_STRING BurnToCDCount;
DRM_CONST_STRING AllowPlaylistBurn;
DRM_CONST_STRING PlaylistBurnTrackCount;
DRM_CONST_STRING MaxPlaylistBurnCount;
DRM_CONST_STRING AllowCollaborativePlay;
DRM_CONST_STRING BeginDate;
DRM_CONST_STRING ExpirationDate;
DRM_CONST_STRING ExpirationAfterFirstUse;
DRM_CONST_STRING ExpirationAfterFirstPlay;
DRM_CONST_STRING ExpirationOnStore;
DRM_CONST_STRING GracePeriod;
DRM_CONST_STRING DeleteOnClockRollback;
DRM_CONST_STRING DisableOnClockRollback;
DRM_CONST_STRING MinimumSecurityLevel;
DRM_CONST_STRING MinimumAppCrlVersion;
DRM_CONST_STRING MinimumDeviceCrlVersion;
DRM_CONST_STRING MinimumClientSDKSecurity;
DRM_CONST_STRING PMRights;
DRM_CONST_STRING PMAppSecurity;
DRM_CONST_STRING PMExpirationDate;
DRM_CONST_STRING AllowSaveStreamProtected;
DRM_CONST_STRING MeteringCert;
DRM_CONST_STRING MeteringID;
DRM_CONST_STRING KID;
DRM_CONST_STRING Priority;
DRM_CONST_STRING RevInfoVersion;
DRM_CONST_STRING IndivVersion;
DRM_CONST_STRING LID;
DRM_CONST_STRING RevocationList;
DRM_CONST_STRING ContentPubKey;
DRM_CONST_STRING RemovalDate;
DRM_CONST_STRING IssueDate;
DRM_CONST_STRING ServerPrivateKey;
DRM_CONST_STRING ServerPublicKey;
DRM_CONST_STRING ServerSeed;
DRM_CONST_STRING WMDRMNETRevocationIndex;
DRM_CONST_STRING DeviceRevocationIndex;
DRM_CONST_STRING Meta;
DRM_CONST_STRING TestOptions;
DRM_CONST_STRING ExcludeApps;
DRM_CONST_STRING SAPMode;
DRM_CONST_STRING UplinkKIDs;
DRM_CONST_STRING LGPUBKEY;
DRM_CONST_STRING AttributeList;
DRM_CONST_STRING UseAESContentKey;
DRM_CONST_STRING UseECCPubKey;
DRM_CONST_STRING SourceID;
DRM_CONST_STRING UnCompressedDigitalAudioLevels;
DRM_CONST_STRING UID;
DRM_CONST_STRING Cert1;
DRM_CONST_STRING Cert2;
DRM_CONST_STRING ContentKey;
DRM_CONST_STRING InclusionList;
DRM_CONST_STRING CannotPersist;
DRM_CONST_STRING RestrictedSourceID;
DRM_CONST_STRING WMDRMNETRevocationData;
DRM_CONST_STRING XMRVersion;
DRM_CONST_STRING XPolicy;
DRM_CONST_STRING XPolicyPlayExtended;
DRM_CONST_STRING XPolicyBurnExtended;
DRM_CONST_STRING DigitalAudioRestrictions;
DRM_CONST_STRING PolicyMetadataList;
DRM_CONST_STRING CanNotBind;
DRM_CONST_STRING DoNotSign;
DRM_CONST_STRING RSAPublicKey;
DRM_CONST_STRING ECCPublicKey;
DRM_CONST_STRING UplinkContentKey;
DRM_CONST_STRING Use16ByteCheckSum;
DRM_CONST_STRING RevInfoForLicenseResponse;
DRM_CONST_STRING EmbedLicense;  // Valid values if present - Ignore, Copy
DRM_CONST_STRING DomainID;
DRM_CONST_STRING DomainAccountID;
DRM_CONST_STRING DomainRevision;
DRM_CONST_STRING SerialNumber;
DRM_CONST_STRING PlayEnablerGUIDList;
DRM_CONST_STRING CopyEnablerGUIDList;
DRM_CONST_STRING CopyCount2;
DRM_CONST_STRING DeviceKey;
DRM_CONST_STRING VerifyExtensibleObject;
DRM_CONST_STRING ExtensibleDepth;
DRM_CONST_STRING VerifyPlayRightsExtensibleContainer;
DRM_CONST_STRING VerifyPlayRightsExtensibleObject;
DRM_CONST_STRING AllowExtensibleRights[g_cdwExtensiblePolicyMapMax];
DRM_CONST_STRING MoveEnabler;
DRM_CONST_STRING TempStoreOnly;
DRM_CONST_STRING AuxKey;
DRM_CONST_STRING UplinkX;
} LICENSERIGHTS;

/**
    GetLicenseRights - Gets a LICENSERIGHTS structure for a test case
    @Param pRightsPool - string pointer to block of memory containing the rights pool
    @Param pTestCaseID - string pointer to test case ID
    @Param ppLicenseRights - returns pointer to a LICENSERIGHTS structure
    @Return A valid DRM_RESULT value
*/

DRM_RESULT GetLicenseRights(const DRM_WCHAR *pRightsPool, const DRM_WCHAR *pTestCaseID, LICENSERIGHTS **ppLicenseRights );
DRM_RESULT GetLicenseRightsAndSetRIV(const DRM_WCHAR *pRightsPool, const DRM_WCHAR *pTestCaseID, LICENSERIGHTS **ppLicenseRights, DRM_CONST_STRING dstrRIV );
DRM_RESULT FreeLicenseRights(LICENSERIGHTS *pLicenseRights);
DRM_RESULT GetRevInfo(const DRM_WCHAR* pRevInfoPool, const DRM_WCHAR* pwszRevInfoID, DRM_CONST_STRING* pRevInfo);
DRM_RESULT _GetTestCaseNode(const DRM_CONST_STRING *pdstrFile, const DRM_WCHAR *pTestCaseID, DRM_CONST_STRING *pdstrCaseNode);


#endif

