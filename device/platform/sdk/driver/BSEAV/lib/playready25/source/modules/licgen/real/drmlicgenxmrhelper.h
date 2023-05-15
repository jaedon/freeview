/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMLICGENXMRHELPER_H_
#define _DRMLICGENXMRHELPER_H_ 1

#include <drmlicgen.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_RESULT SetSecurityLevel(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_WORD                                           f_wSecurityLevel,
    __inout                             DRM_XMR_MINIMUM_ENVIRONMENT                       *f_pxmrMinimumEnvironment ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetCannotPersist(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_BOOL                                           f_fCannotPersist,
    __inout                             DRM_XMR_RIGHTS                                    *f_pxmrRights ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetExpirationDate(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_EXPIRATION                      *f_poExpiration,
    __in                                DRM_DWORD                                          f_dwSubtractFromBeginDate,
    __in                                DRM_DWORD                                          f_dwAddToEndDate,
    __inout                             DRM_XMR_EXPIRATION                                *f_pxmrExpiration ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetExpirationAfterFirstPlay(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_EXPIRATION_AFTER_FIRST_PLAY     *f_poExpirationAfterFirstPlay,
    __inout                             DRM_XMR_EXPIRATION_AFTER_FIRSTUSE                 *f_pxmrExpiration ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetRealTimeExpiration(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_BOOL                                           f_fRealTimeExpiration,
    __inout                             DRM_XMR_REAL_TIME_EXPIRATION                      *f_pxmrRealTimeExpiration ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetSourceID(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_SOURCE_ID                       *f_poSourceID,
    __inout                             DRM_XMR_SOURCEID                                  *f_pxmrSourceID,
    __inout                             DRM_XMR_RESTRICTED_SOURCEID                       *f_pxmrRestrictedSourceID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetOPLs(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_WORD                                           f_wCompressedDigitalVideo,
    __in                          const DRM_WORD                                           f_wUncompressedDigitalVideo,
    __in                          const DRM_WORD                                           f_wAnalogVideo,
    __in                          const DRM_WORD                                           f_wCompressedDigitalAudio,
    __in                          const DRM_WORD                                           f_wUncompressedDigitalAudio,
    __inout                             DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS          *f_pxmrMinOPL ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetExplicitOutputProtection(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_WORD                                           f_wXmrObjectType,
    __in                          const DRM_WORD                                           f_cExplicitProtection,
    __in                          const DRM_LOCAL_LICENSE_EXPLICIT_OUTPUT_PROTECTION      *f_poExplicitProtection,
    __inout                             DRM_XMR_EXPLICIT_OUTPUT_PROTECTION                *f_pXmrExplicitProtection ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT FreeExplicitOutputProtection(
    __inout                             DRM_XMR_EXPLICIT_OUTPUT_PROTECTION                *f_pXmrExplicitProtection ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetPlayEnabler(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_DWORD                                          f_cPlayEnablers,
    __in                          const DRM_GUID                                          *f_pguidPlayEnablers )DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetContentKey(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_TYPE                             f_eLicenseType,
    __in                          const DRM_BYTE                                          *f_rgbKeyID,
    __in                          const DRM_DWORD                                          f_cbEncryptedKey,
    __in_bcount( f_cbEncryptedKey )     DRM_BYTE                                          *f_pbEncryptedKey,
    __inout                             DRM_XMR_CONTENT_KEY                               *f_pxmrContentKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetECCDeviceKey(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_DWORD                                          f_cbEncryptedKey,
    __in_bcount( f_cbEncryptedKey )     DRM_BYTE                                          *f_pbEncryptedKey,
    __inout                             DRM_XMR_ECC_DEVICE_KEY                            *f_pxmrECCDeviceKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetUplinkKID(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_BYTE                                          *f_pbKeyID,
    __in                          const DRM_BYTE                                          *f_pbCheckSum,
    __inout                             DRM_XMR_UPLINK_KID                                *f_pxmrUplinkKID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetIssueDate(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __inout                             DRM_XMR_ISSUEDATE                                 *f_pxmrIssueDate ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetRevInfo(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_DWORD                                          f_dwVersion,
    __inout                             DRM_XMR_REVOCATION_INFORMATION_VERSION            *f_pxmrRIV ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_RESULT SetEmbeddedBehavior(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __inout                             DRM_XMR_EMBEDDING_BEHAVIOR                        *f_pxmrEmbBehavior ) DRM_NO_INLINE_ATTRIBUTE;

typedef struct __tag_Local_License_SignLicense_Data
{
    DRM_BB_CONTEXT      *poBlackBoxContext;
    OEM_CRYPTO_HANDLE    handleCI;
} Local_License_SignLicense_Data;

DRM_NO_INLINE DRM_RESULT DRM_CALL Local_License_SignLicense(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount(f_cbData)               const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer ) DRM_NO_INLINE_ATTRIBUTE;


EXIT_PK_NAMESPACE;

#endif /* _DRMLICGENXMRHELPER_H_ */

