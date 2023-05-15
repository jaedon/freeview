/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMLICGENXMRHELPER_C

#include <drmlicgenxmrhelper.h>
#include <drmprndprotocol.h>
#include <drmmanager.h>
#include <drmmathsafe.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_NO_INLINE DRM_RESULT SetSecurityLevel(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_WORD                                           f_wSecurityLevel,
    __inout                             DRM_XMR_MINIMUM_ENVIRONMENT                       *f_pxmrMinimumEnvironment )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder            != NULL );
    ChkArg( f_pxmrMinimumEnvironment != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SECURITY_LEVEL, f_pxmrMinimumEnvironment ) );
    f_pxmrMinimumEnvironment->wMinimumSecurityLevel = f_wSecurityLevel;
    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_SECURITY_LEVEL, f_pxmrMinimumEnvironment ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetCannotPersist(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_BOOL                                           f_fCannotPersist,
    __inout                             DRM_XMR_RIGHTS                                    *f_pxmrRights )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder != NULL );
    ChkArg( f_pxmrRights  != NULL );

    if( f_fCannotPersist )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SETTINGS_OBJECT, f_pxmrRights ) );
        f_pxmrRights->wValue |= XMR_RIGHTS_CANNOT_PERSIST;
        ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_SETTINGS_OBJECT, f_pxmrRights ) );
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetExpirationDate(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_EXPIRATION                      *f_poExpiration,
    __in                                DRM_DWORD                                          f_dwSubtractFromBeginDate,
    __in                                DRM_DWORD                                          f_dwAddToEndDate,
    __inout                             DRM_XMR_EXPIRATION                                *f_pxmrExpiration )
{
    DRM_RESULT  dr            = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder      != NULL );
    ChkArg( f_poExpiration     != NULL );
    ChkArg( f_pxmrExpiration   != NULL );

    /* nothing to do if expiration is not valid */
    if( !f_poExpiration->fValid )
    {
        goto ErrorExit;
    }

    ChkArg( f_poExpiration->dwBeginDate < f_poExpiration->dwEndDate );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EXPIRATION_OBJECT, f_pxmrExpiration ) );

    if( f_poExpiration->dwBeginDate == DRM_LOCAL_LICENSE_EXPIRATION_MIN_BEGIN_DATE )
    {
        /* Use current time if no BeginDate is specified */
        DRMFILETIME ftCurrent = { 0 };

        Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrent );
        FILE_TIME_TO_DWORD( ftCurrent, f_pxmrExpiration->dwBeginDate );

        ChkDR( DRM_DWordSub( f_pxmrExpiration->dwBeginDate, f_dwSubtractFromBeginDate, &f_pxmrExpiration->dwBeginDate ) );
    }
    else
    {
        if( f_poExpiration->dwBeginDate >= f_dwSubtractFromBeginDate )
        {
            ChkDR( DRM_DWordSub( f_poExpiration->dwBeginDate, f_dwSubtractFromBeginDate, &f_pxmrExpiration->dwBeginDate ) );
        }
    }

    if( f_poExpiration->dwEndDate == DRM_LOCAL_LICENSE_INFINITE_EXPIRATION )
    {
        f_pxmrExpiration->dwEndDate = XMR_UNLIMITED;
    }
    else
    {
        if( f_poExpiration->dwEndDate <= MAX_UNSIGNED_TYPE(DRM_DWORD) - f_dwAddToEndDate )
        {
            ChkDR( DRM_DWordAdd( f_poExpiration->dwEndDate, f_dwAddToEndDate, &f_pxmrExpiration->dwEndDate ) );
        }
    }

    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_OBJECT, f_pxmrExpiration ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetExpirationAfterFirstPlay(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_EXPIRATION_AFTER_FIRST_PLAY     *f_poExpirationAfterFirstPlay,
    __inout                             DRM_XMR_EXPIRATION_AFTER_FIRSTUSE                 *f_pxmrExpiration )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder                != NULL );
    ChkArg( f_poExpirationAfterFirstPlay != NULL );
    ChkArg( f_pxmrExpiration             != NULL );

    if( f_poExpirationAfterFirstPlay->fValid )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT, f_pxmrExpiration ) );
        f_pxmrExpiration->dwValue = f_poExpirationAfterFirstPlay->dwValue;
        ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT, f_pxmrExpiration ) );
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetRealTimeExpiration(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_BOOL                                           f_fRealTimeExpiration,
    __inout                             DRM_XMR_REAL_TIME_EXPIRATION                      *f_pxmrRealTimeExpiration )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder            != NULL );
    ChkArg( f_pxmrRealTimeExpiration != NULL );

    if( f_fRealTimeExpiration )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION, f_pxmrRealTimeExpiration ) );
        ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION, f_pxmrRealTimeExpiration ) );
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetSourceID(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_SOURCE_ID                       *f_poSourceID,
    __inout                             DRM_XMR_SOURCEID                                  *f_pxmrSourceID,
    __inout                             DRM_XMR_RESTRICTED_SOURCEID                       *f_pxmrRestrictedSourceID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder            != NULL );
    ChkArg( f_poSourceID             != NULL );
    ChkArg( f_pxmrSourceID           != NULL );
    ChkArg( f_pxmrRestrictedSourceID != NULL );

    if( f_poSourceID->fValid )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SOURCEID_OBJECT, f_pxmrSourceID ) );

        f_pxmrSourceID->dwValue = f_poSourceID->dwSourceId;
        ChkDR( DRM_XMR_AddObject(f_pxmrBuilder, XMR_OBJECT_TYPE_SOURCEID_OBJECT, f_pxmrSourceID ) );

        if( f_poSourceID->fRestrictedSourceId )
        {
            ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT, f_pxmrRestrictedSourceID ) );
            ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT, f_pxmrRestrictedSourceID ) );
        }
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetExplicitOutputProtection(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_WORD                                           f_wXmrObjectType,
    __in                          const DRM_WORD                                           f_cExplicitProtection,
    __in                          const DRM_LOCAL_LICENSE_EXPLICIT_OUTPUT_PROTECTION      *f_poExplicitProtection,
    __inout                             DRM_XMR_EXPLICIT_OUTPUT_PROTECTION                *f_pXmrExplicitProtection )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  iProtection = 0;

    ChkArg( f_pxmrBuilder            != NULL );
    ChkArg(    f_wXmrObjectType      == XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER
            || f_wXmrObjectType      == XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER );
    ChkArg( f_cExplicitProtection    <= DRM_MAX_LOCAL_LICENSE_EXPLICIT_OUTPUT_PROTECTIONS );
    ChkArg( f_pXmrExplicitProtection != NULL );

    /* if no explicit protection, there's nothing to do */
    if( f_cExplicitProtection == 0 )
    {
        goto ErrorExit;
    }
    ChkArg( f_poExplicitProtection   != NULL );

    ChkDR( DRM_XMR_InitializeObject( f_wXmrObjectType, f_pXmrExplicitProtection ) );

    for( iProtection = 0; iProtection < f_cExplicitProtection; iProtection++ )
    {
        DRM_XMR_OUTPUT_CONFIGURATION_LIST *pOutputNode = NULL;

        ChkMem( pOutputNode = (DRM_XMR_OUTPUT_CONFIGURATION_LIST*)Drm_Prnd_MemAlloc( SIZEOF( DRM_XMR_OUTPUT_CONFIGURATION_LIST ) ) );
        OEM_SECURE_ZERO_MEMORY( pOutputNode, SIZEOF( DRM_XMR_OUTPUT_CONFIGURATION_LIST ) );

        pOutputNode->pNext = NULL;
        pOutputNode->Config.fValid = TRUE;
        pOutputNode->Config.iGuid  = 0;
        pOutputNode->Config.iConfigData = 0;
        ChkMem( pOutputNode->Config.pguidBuffer = (DRM_BYTE*)Drm_Prnd_MemAlloc( SIZEOF( DRM_GUID ) ) );
        OEM_SECURE_MEMCPY( pOutputNode->Config.pguidBuffer, &f_poExplicitProtection[iProtection].oProtectionId, SIZEOF( DRM_GUID ) );

        if( f_poExplicitProtection[iProtection].cbConfigData > 0 )
        {
            ChkArg( f_poExplicitProtection[iProtection].cbConfigData <= DRM_MAX_LOCAL_LICENSE_EXPLICIT_OUTPUT_PROTECTION_CONFIG_DATA );
            ChkMem( pOutputNode->Config.pbConfigDataBuffer = (DRM_BYTE*)Drm_Prnd_MemAlloc( f_poExplicitProtection[iProtection].cbConfigData ) );
            OEM_SECURE_MEMCPY(
                pOutputNode->Config.pbConfigDataBuffer,
                f_poExplicitProtection[iProtection].rgbConfigData,
                f_poExplicitProtection[iProtection].cbConfigData );

            pOutputNode->Config.cbConfigData = f_poExplicitProtection[iProtection].cbConfigData;
        }
        else
        {
            pOutputNode->Config.pbConfigDataBuffer = NULL;
            pOutputNode->Config.cbConfigData       = 0;
        }

        f_pXmrExplicitProtection->cOutputProtectionIDs++;

        pOutputNode->pNext = f_pXmrExplicitProtection->plistOutputConfiguration;
        f_pXmrExplicitProtection->plistOutputConfiguration = pOutputNode;
    }

    f_pXmrExplicitProtection->cbRawData = 0;
    f_pXmrExplicitProtection->pbRawData = NULL;
    f_pXmrExplicitProtection->iRawData  = 0;

    ChkDR( DRM_XMR_AddObject(f_pxmrBuilder, f_wXmrObjectType, f_pXmrExplicitProtection ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT FreeExplicitOutputProtection(
    __inout                             DRM_XMR_EXPLICIT_OUTPUT_PROTECTION                *f_pXmrExplicitProtection )
{
    DRM_RESULT dr                                  = DRM_SUCCESS;
    DRM_DWORD  iProtection                         = 0;
    DRM_XMR_OUTPUT_CONFIGURATION_LIST *pOutputNode = NULL;

    ChkArg( f_pXmrExplicitProtection                       != NULL );
    ChkArg( f_pXmrExplicitProtection->cOutputProtectionIDs <= DRM_MAX_LOCAL_LICENSE_EXPLICIT_OUTPUT_PROTECTIONS );

    pOutputNode = f_pXmrExplicitProtection->plistOutputConfiguration;

    for( iProtection = 0; iProtection < f_pXmrExplicitProtection->cOutputProtectionIDs; iProtection++ )
    {
        DRM_BYTE *pTempNode = NULL;

        ChkArg( pOutputNode != NULL );

        DRM_PRND_SAFEMEMFREE( pOutputNode->Config.pguidBuffer );
        DRM_PRND_SAFEMEMFREE( pOutputNode->Config.pbConfigDataBuffer );

        pTempNode   = (DRM_BYTE*)pOutputNode;
        pOutputNode = pOutputNode->pNext;

        DRM_PRND_SAFEMEMFREE( pTempNode );
    }

    f_pXmrExplicitProtection->plistOutputConfiguration = NULL;
    f_pXmrExplicitProtection->cOutputProtectionIDs     = 0;

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetPlayEnabler(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_DWORD                                          f_cPlayEnablers,
    __in                          const DRM_GUID                                          *f_pguidPlayEnablers )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  iPlayEnabler = 0;

    ChkArg( f_pxmrBuilder       != NULL );
    ChkArg( f_cPlayEnablers     <= DRM_MAX_LOCAL_LICENSE_PLAY_ENABLERS );

    /* Nothing to do if count is 0 */
    if( f_cPlayEnablers == 0 )
    {
        goto ErrorExit;
    }

    ChkArg( f_pguidPlayEnablers != NULL );

    for( iPlayEnabler = 0; iPlayEnabler < f_cPlayEnablers; iPlayEnabler++ )
    {
        ChkDR( DRM_XMR_AddUnknownObject(
            f_pxmrBuilder,
            XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER,
            TRUE,
            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,
            XMR_FLAGS_CONTAINER,
            NULL,
            0 ) );

        ChkDR( DRM_XMR_AddUnknownObject(
            f_pxmrBuilder,
            XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT,
            TRUE,
            XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER,
            0,
            (DRM_BYTE*)&f_pguidPlayEnablers[iPlayEnabler],
            SIZEOF( DRM_GUID ) ) );
    }

ErrorExit:
    return dr;
}


DRM_NO_INLINE DRM_RESULT SetOPLs(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_WORD                                           f_wCompressedDigitalVideo,
    __in                          const DRM_WORD                                           f_wUncompressedDigitalVideo,
    __in                          const DRM_WORD                                           f_wAnalogVideo,
    __in                          const DRM_WORD                                           f_wCompressedDigitalAudio,
    __in                          const DRM_WORD                                           f_wUncompressedDigitalAudio,
    __inout                             DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS          *f_pxmrMinOPL )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder != NULL );
    ChkArg( f_pxmrMinOPL  != NULL );

    /* We only need to set the output protection level if any of the levels are greater than DRM_LOCAL_LICENSE_MINIMUM_PROTECTION_LEVEL */
    if(    f_wCompressedDigitalVideo   > DRM_LOCAL_LICENSE_MINIMUM_PROTECTION_LEVEL
        || f_wUncompressedDigitalVideo > DRM_LOCAL_LICENSE_MINIMUM_PROTECTION_LEVEL
        || f_wAnalogVideo              > DRM_LOCAL_LICENSE_MINIMUM_PROTECTION_LEVEL
        || f_wCompressedDigitalAudio   > DRM_LOCAL_LICENSE_MINIMUM_PROTECTION_LEVEL
        || f_wUncompressedDigitalAudio > DRM_LOCAL_LICENSE_MINIMUM_PROTECTION_LEVEL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, f_pxmrMinOPL ) );

        f_pxmrMinOPL->wCompressedDigitalVideo   = f_wCompressedDigitalVideo;
        f_pxmrMinOPL->wUncompressedDigitalVideo = f_wUncompressedDigitalVideo;
        f_pxmrMinOPL->wAnalogVideo              = f_wAnalogVideo;
        f_pxmrMinOPL->wCompressedDigitalAudio   = f_wCompressedDigitalAudio;
        f_pxmrMinOPL->wUncompressedDigitalAudio = f_wUncompressedDigitalAudio;

        ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, f_pxmrMinOPL ) );
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetContentKey(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_LOCAL_LICENSE_TYPE                             f_eLicenseType,
    __in                          const DRM_BYTE                                          *f_pbKeyID,
    __in                          const DRM_DWORD                                          f_cbEncryptedKey,
    __in_bcount( f_cbEncryptedKey )     DRM_BYTE                                          *f_pbEncryptedKey,
    __inout                             DRM_XMR_CONTENT_KEY                               *f_pxmrContentKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder    != NULL );
    ChkArg( f_pbKeyID        != NULL );
    ChkArg( f_cbEncryptedKey  > 0    );
    ChkArg( f_pbEncryptedKey != NULL );
    ChkArg( f_pxmrContentKey != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, f_pxmrContentKey ) );

    /* Default content key setting that can be overridden by particular license */
    f_pxmrContentKey->pbguidKeyID              = (DRM_BYTE*)f_pbKeyID;
    f_pxmrContentKey->iguidKeyID               = 0;
    f_pxmrContentKey->wSymmetricCipherType     = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;

    f_pxmrContentKey->pbEncryptedKeyBuffer     = f_pbEncryptedKey;
    ChkDR( DRM_DWordToWord( f_cbEncryptedKey, &f_pxmrContentKey->cbEncryptedKey ) );
    f_pxmrContentKey->iEncryptedKey            = 0;
    f_pxmrContentKey->wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256;

    if(    f_eLicenseType == eDRM_LOCAL_LICENSE_LOCAL_BOUND_ROOT
        || f_eLicenseType == eDRM_LOCAL_LICENSE_REMOTE_BOUND_ROOT )
    {
        f_pxmrContentKey->wSymmetricCipherType     = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB;
    }

    if( f_eLicenseType == eDRM_LOCAL_LICENSE_LEAF )
    {
        f_pxmrContentKey->wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE;
    }

    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, f_pxmrContentKey ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetECCDeviceKey(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_DWORD                                          f_cbEncryptedKey,
    __in_bcount( f_cbEncryptedKey )     DRM_BYTE                                          *f_pbEncryptedKey,
    __inout                             DRM_XMR_ECC_DEVICE_KEY                            *f_pxmrECCDeviceKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder      != NULL );
    ChkArg( f_cbEncryptedKey    > 0    );
    ChkArg( f_pbEncryptedKey   != NULL );
    ChkArg( f_pxmrECCDeviceKey != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, f_pxmrECCDeviceKey ) );

    f_pxmrECCDeviceKey->wEccCurveType = XMR_ECC_CURVE_TYPE_P256;
    f_pxmrECCDeviceKey->pbKeyData     = f_pbEncryptedKey;
    ChkDR( DRM_DWordToWord( f_cbEncryptedKey, &f_pxmrECCDeviceKey->cbKeyData ) );
    f_pxmrECCDeviceKey->iKeyData      = 0;

    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, f_pxmrECCDeviceKey ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetUplinkKID(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_BYTE                                          *f_pbKeyID,
    __in                          const DRM_BYTE                                          *f_pbCheckSum,
    __inout                             DRM_XMR_UPLINK_KID                                *f_pxmrUplinkKID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder     != NULL );
    ChkArg( f_pbKeyID         != NULL );
    ChkArg( f_pbCheckSum      != NULL );
    ChkArg( f_pxmrUplinkKID   != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT, f_pxmrUplinkKID ) );

    f_pxmrUplinkKID->pbguidUplinkKID         = (DRM_BYTE*)f_pbKeyID;
    f_pxmrUplinkKID->iguidUplinkKID          = 0;

    f_pxmrUplinkKID->cbChainedCheckSum       = DRM_AES_CK_CHECKSUM_LENGTH;
    f_pxmrUplinkKID->pbChainedCheckSumBuffer = (DRM_BYTE*)f_pbCheckSum;
    f_pxmrUplinkKID->iChainedCheckSum        = 0;

    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT, f_pxmrUplinkKID ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetIssueDate(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __inout                             DRM_XMR_ISSUEDATE                                 *f_pxmrIssueDate )
{
    DRM_RESULT  dr            = DRM_SUCCESS;
    DRMFILETIME filetimeLocal = { 0 };

    ChkArg( f_pxmrBuilder    != NULL );
    ChkArg( f_pxmrIssueDate  != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ISSUEDATE_OBJECT, f_pxmrIssueDate ) );

    Oem_Clock_GetSystemTimeAsFileTime( NULL, &filetimeLocal );
    FILE_TIME_TO_DWORD( filetimeLocal, f_pxmrIssueDate->dwValue );

    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_ISSUEDATE_OBJECT, f_pxmrIssueDate ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetRevInfo(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __in                          const DRM_DWORD                                          f_dwVersion,
    __inout                             DRM_XMR_REVOCATION_INFORMATION_VERSION            *f_pxmrRIV )
{
    DRM_RESULT  dr            = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder != NULL );
    ChkArg( f_pxmrRIV     != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, f_pxmrRIV ) );

    f_pxmrRIV->dwValue = f_dwVersion;
    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, f_pxmrRIV ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT SetEmbeddedBehavior(
    __inout                             DRM_XMR_BUILDER_CONTEXT                           *f_pxmrBuilder,
    __inout                             DRM_XMR_EMBEDDING_BEHAVIOR                        *f_pxmrEmbBehavior )
{
    DRM_RESULT  dr            = DRM_SUCCESS;

    ChkArg( f_pxmrBuilder     != NULL );
    ChkArg( f_pxmrEmbBehavior != NULL );

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, f_pxmrEmbBehavior ) );

    f_pxmrEmbBehavior->wValue = XMR_EMBEDDING_BEHAVIOR_IGNORE;
    ChkDR( DRM_XMR_AddObject( f_pxmrBuilder, XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, f_pxmrEmbBehavior ) );

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT DRM_CALL Local_License_SignLicense(
    __inout                             const DRM_VOID  *f_pvSignatureContext,
    __in_bcount(f_cbData)               const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __inout                             const DRM_DWORD  f_ibSignatureBuffer )
{
    DRM_RESULT                           dr         = DRM_SUCCESS;
    Local_License_SignLicense_Data      *pSignData  = (Local_License_SignLicense_Data*)f_pvSignatureContext;

    ChkArg( f_pvSignatureContext != NULL );

    /*
    ** f_pbData / f_cbData is the unsigned portion of the license.
    ** f_pbSignatureBuffer / f_cbSignatureBuffer is the whole license buffer, not just the buffer for the actual signature.
    */
    ChkArg( f_ibSignatureBuffer + DRM_AES_BLOCKLEN == f_cbSignatureBuffer );

    ChkDR( DRM_PRND_Tx_SignWithAESOMAC(
        pSignData->poBlackBoxContext,
        pSignData->handleCI,
        f_pbData,
        f_cbData,
        f_pbSignatureBuffer + f_ibSignatureBuffer ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

