/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _XMRLICGEN_H
#define _XMRLICGEN_H

#include "trightsparser.h"


#include "PKTestNameSpace.h"


extern const DRM_CONST_STRING g_dstrTagOPLExtension;
extern const DRM_CONST_STRING g_dstrAttribGuid;
extern const DRM_CONST_STRING g_dstrAttribValue;
extern const DRM_CONST_STRING g_dstrAttribLength;
extern const DRM_CONST_STRING g_dstrTagExPolicy;
extern const DRM_CONST_STRING g_dstrTagExPolicyRightsList;
extern const DRM_CONST_STRING g_dstrTagExPolicyRight;
extern const DRM_CONST_STRING g_dstrTagExPolicyRestrictionsList;
extern const DRM_CONST_STRING g_dstrTagExPolicyRestriction;
extern const DRM_CONST_STRING g_dstrAttribExPolicyObjectType;
extern const DRM_CONST_STRING g_dstrAttribExPolicyObjectFlags;
extern const DRM_CONST_STRING g_dstrAttribExPolicySubObjectType;
extern const DRM_CONST_STRING g_dstrAttribExPolicySubObjectFlags;
extern const DRM_CONST_STRING g_dstrAttribExPolicyPolicyData;
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateDate;
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateCount;
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateData;
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateSize;
extern const DRM_CONST_STRING g_dstrTagPlayExtendedRestriction;
extern const DRM_CONST_STRING g_dstrTagBurnExtendedRestriction;
extern const DRM_CONST_STRING g_dstrAttribExPolicySecureStateType;
extern const DRM_CONST_STRING g_dstrTagPolicyMetadataList;
extern const DRM_CONST_STRING g_dstrTagPolicyMetadata;
extern const DRM_CONST_STRING g_dstrAttribType;
extern const DRM_CONST_STRING g_dstrTagPlayEnablerGUIDList;
extern const DRM_CONST_STRING g_dstrTagPlayEnablerGUID;
extern const DRM_CONST_STRING g_dstrTagCopyEnablerGUIDList;
extern const DRM_CONST_STRING g_dstrTagCopyEnablerGUID;
extern const DRM_CONST_STRING g_dstrAttribAccountID;
extern const DRM_CONST_STRING g_dstrAttribRevision;


#define OP_LICGEN_TAMPERED_SIGNATURE 0xa 
#define OP_LICGEN_INVALID_UPLINK_CHECKSUM 0x10000
#define OP_LICGEN_INVALID_UPLINKX_CHECKSUM 0x20000


typedef enum
{
    ETYPE_DWORD = 0,
    ETYPE_WCHAR,
    ETYPE_BINARY
} EValueType;

typedef struct _XMRLICENSELINKLIST_TAG
{
    struct _XMRLICENSELINKLIST_TAG *pNext;
    struct _XMRLICENSELINKLIST_TAG *pLeaf;
    DRM_BYTE *pLicense;
    DRM_DWORD cbLicense;
    DRM_CONST_STRING szRevInfoForLicenseResponse;
} XMRLICENSELINKLIST;

typedef struct 
{
    DRM_DWORD dwVersionOverride;            /* Override the version number specified in the rights pool */
    DRM_CONST_STRING *pdstrKIDOverride;     /* Override the KID specified in the rights pool */
    DRM_CONST_STRING *pdstrRootKIDOverride; /* Override the ROOT KID specified in the rights pool */
    DRM_CONST_STRING *pdstrRIDOverride;     /* Override the license's RID */
    DRM_DWORD cbContentKeyOverride;         /* Length of the content key override */
    DRM_BYTE *pbContentKeyOverride;         /* Overrides the content key */
    DRM_DWORD cbIntegrityKeyOverride;       /* length of the integrity key override */
    DRM_BYTE *pbIntegrityKeyOverride;       /* Overrides the content key */
    DRM_CONST_STRING dstrKIDUsed;           /* returns the KID used.   Caller frees using FREEDRMSTRING */
    DRM_CONST_STRING dstrRootKIDUsed;       /* returns the RootKID used.  Caller frees using FREEDRMSTRING */
    DRM_DWORD cbContentKeyUsed;             /* __in the size of pbContentKeyUsed | __out returns the length of the content key used. */
    DRM_BYTE *pbContentKeyUsed;             /* returns the content key used.   */
    DRM_DWORD cbIntegrityKeyUsed;           /* __in the size of pbIntegrityKeyUsed | __out returns the length of the integrity key used. */
    DRM_BYTE *pbIntegrityKeyUsed;           /* returns the integrity key used.  */ 
    DRM_GUID *pguidDomainAccountID;         /* the AccountID for the domain to use */
    DRM_DWORD dwDomainRevision;             /* the Domain revision */
    DRM_DWORD dwMinimalRIV2;                /* The minimal RIV2 specified in WMDRM Rev_Info1 structure */
    DRM_DWORD dwMinimalRIV1;                /* The minimal RIV1 specified in MSPR Rev_Info2 structure */
    DRM_BYTE *pbPubKeyOverride;             /* the public key to use for encrypting the license */
    DRM_DWORD cbPubKeyOverride;             /* the length of the public key set in pbPubKey */
    DRM_CONST_STRING dstrLIDUsed;           /* returns the LID used.   Caller frees using FREEDRMSTRING */
    DRMSYSTEMTIME *pTestStartTime;          /* the start of the current test case */
} OptionalTestMakeXMRLicenseParams;

DRM_RESULT FreeOptionalTestMakeXMRLicenseParams( __in OptionalTestMakeXMRLicenseParams* f_pOptparams );

DRM_RESULT _MakeRandomKID(DRM_CONST_STRING *pKID);

DRM_RESULT TestMakeXMRLicense(
    DRM_DWORD dwOptions,
    LICENSERIGHTS *pRights,
    XMRLICENSELINKLIST **pList,
    OptionalTestMakeXMRLicenseParams *pOptionalParams
    );

DRM_RESULT TestFreeXMRLicenseList( XMRLICENSELINKLIST *pList );

DRM_RESULT tSetECCPubKey(DRM_BYTE * pPubKey, DRM_DWORD cbPubKey);

DRM_RESULT TSTUTIL_DeriveAuxKey(
    __in                                DRM_DWORD f_dwLocation,
    __in_bcount( DRM_AES_KEYSIZE_128 )  DRM_BYTE *f_pbBaseKey,
    __out_bcount( DRM_AES_KEYSIZE_128 ) DRM_BYTE *f_pbKey );

DRM_RESULT TSTUTIL_CalcContentKeyPrime( 
    __in_bcount( DRM_AES_KEYSIZE_128 )  DRM_BYTE *f_pbContentKey,
    __out_bcount( DRM_AES_KEYSIZE_128 ) DRM_BYTE *f_pbContentKeyPrime );

DRM_RESULT TSTUTIL_UnshuffleContentKey( 
    __inout_bcount( DRM_AES_KEYSIZE_128 * 2 ) DRM_BYTE *f_pbContentKey );

#endif /* _XMRLICGEN_H */
