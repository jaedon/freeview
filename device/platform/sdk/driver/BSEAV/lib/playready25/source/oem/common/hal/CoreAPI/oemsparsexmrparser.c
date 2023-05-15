/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/******************************************************************************
**
** This file defines a minimal XMR license parser to enable parsing specific objects out of XMR inside the secure crypto core.
** OEMs should not need to change this file in any way (except to get it to compile properly).
** If the device supports a secure crypto core, this file should compile ONLY for that core.
**
*******************************************************************************
*/

#define DRM_BUILDING_OEMSPARSEXMRPARSER_C

#include <oemsparsexmrparser.h>
#include <oemcommon.h>
#include <drmmathsafe.h>

#include <drmerr.h>

#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

static const XMR_SPARSE_PARSER_INTERNAL_DEFINES g_xmrParserDefines[XMR_OBJECT_TYPE_MAXIMUM_DEFINED+1] =
{
                                                    /*  V1     V2     V3 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_INVALID */
    { OEM_XMR_Parse_OuterContainer,                 {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_OUTER_CONTAINER */
    { OEM_XMR_Parse_GlobalContainer,                {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT */
    { OEM_XMR_Parse_UplinkKID,                      {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_UPLINK_KID_OBJECT    */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT */
    { OEM_XMR_Parse_KeyMaterialContainer,           {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER */
    { OEM_XMR_Parse_ContentKey,                     {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT */
    { OEM_XMR_Parse_Signature,                      {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SIGNATURE_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT */
    { OEM_XMR_Parse_WORD,                           {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SETTINGS_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT   */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_PRIORITY_OBJECT  */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPIRATION_OBJECT    */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_ISSUEDATE_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT   */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_METERING_OBJECT  */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_EXCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_INCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPYCOUNT_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT   */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_COPY_EXCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_COPY_INCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT    */
    { OEM_XMR_Parse_DWORD_Versioned,                {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT */
    { OEM_XMR_Parse_RsaPubkey,                      {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SOURCEID_OBJECT  */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_EXPLICIT_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER (Shipped in Emerald Beta 1)*/
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT (Shipped in Emerald Beta 1)*/
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_REVOCATION_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_USERID_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT  */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT  */
    { OEM_XMR_Parse_EccDevicekey,                   { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT */
    { OEM_XMR_Parse_OptimizedContentKey,            { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT */
    { OEM_XMR_Parse_DWORD_Versioned,                { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT */
    { OEM_XMR_Parse_SecurityLevel,                  { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SECURITY_LEVEL */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT */
    { OEM_XMR_Parse_UplinkKID2,                     { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER */
    { OEM_XMR_Parse_Ignore,                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_READ_POLICY_CONTAINER */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_42 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_43 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_44 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_45 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_46 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_47 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_48 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_49 */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4A */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4B */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4C */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4D */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4E */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4F */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT */
    { OEM_XMR_Parse_AuxKey,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_AUX_KEY_OBJECT */
    { OEM_XMR_Parse_UplinkX,                        { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_UPLINKX_OBJECT */
    { OEM_XMR_Parse_Invalid,                        { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_INVALID_RESERVED_53 (0x0053) was documented in PlayReady 2.0 CHM for Content Boundary Identification Object */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_APPLICATION_ID_LIST */
    { OEM_XMR_Parse_Ignore,                         { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION */
};

/******************************************************************************
**
** Function :   OEM_XMR_Parse_Invalid
**
** Synopsis :   Returns DRM_E_INVALIDARG
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_E_INVALIDARG
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_Invalid(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    return DRM_E_INVALIDARG;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_WORD(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT    dr      = DRM_SUCCESS;
    DRM_DWORD     iObject = 0;
    DRM_XMR_WORD *pword   = ( DRM_XMR_WORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ), DRM_E_INVALID_LICENSE );   /* Constant, no overflow check required */
    ChkDR( DRM_DWordAdd( f_iObject, XMR_BASE_OBJECT_LENGTH, &iObject ) );

    NETWORKBYTES_TO_WORD( pword->wValue, f_pbBuffer, iObject );
    ChkDR( DRM_DWordAdd( iObject, SIZEOF( DRM_WORD ), &iObject ) );

    pword->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_Parse_OuterContainer
**
** Synopsis :   Parse an outer container object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_OuterContainer(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT               dr = DRM_SUCCESS;
    DRM_DWORD                iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD                 wFlags = 0;
    DRM_WORD                 wType = 0;
    DRM_DWORD                cbChild    = 0;
    DRM_XMR_OUTER_CONTAINER *pContainer = ( DRM_XMR_OUTER_CONTAINER * )f_pvObject;

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALID_LICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pContainer->fValid                                  = TRUE;
    pContainer->containerGlobalPolicies.fValid          = FALSE;
    pContainer->containerPlaybackPolicies.fValid        = FALSE;
    pContainer->containerCopyPolicies.fValid            = FALSE;
    pContainer->containerCopyToPCPolicies.fValid        = FALSE;
    pContainer->containerPlaylistBurnPolicies.fValid    = FALSE;
    pContainer->generationNumber.fValid                 = FALSE;
    pContainer->containerUnknown.fValid                 = FALSE;
    pContainer->containerRevocation.fValid              = FALSE;
    pContainer->containerKeys.fValid                    = FALSE;
    pContainer->signature.fValid                        = FALSE;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags,  f_pbBuffer, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType,   f_pbBuffer, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbBuffer, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + f_cbObject - iCurrent ), DRM_E_INVALID_LICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER:
                ChkBOOL( !pContainer->containerGlobalPolicies.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pContainer->containerGlobalPolicies ) );
                break;

            case XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER:
                ChkBOOL( !pContainer->containerKeys.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pContainer->containerKeys ) );
                break;

            case XMR_OBJECT_TYPE_SIGNATURE_OBJECT:
                ChkBOOL( !pContainer->signature.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pContainer->signature ) );
                break;

            default:

                break;
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_Parse_GlobalContainer
**
** Synopsis :   Parse a global container object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_GlobalContainer(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD    wFlags   = 0;
    DRM_WORD    wType    = 0;
    DRM_DWORD   cbChild  = 0;
    DRM_XMR_GLOBAL_REQUIREMENTS *pcontainerGlobal =
            ( DRM_XMR_GLOBAL_REQUIREMENTS * )f_pvObject;

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALID_LICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainerGlobal->fValid                                = TRUE;
    pcontainerGlobal->MinimumEnvironment.fValid             = FALSE;
    pcontainerGlobal->SerialNumber.fValid                   = FALSE;
    pcontainerGlobal->Rights.fValid                         = FALSE;
    pcontainerGlobal->Priority.fValid                       = FALSE;
    pcontainerGlobal->SourceID.fValid                       = FALSE;
    pcontainerGlobal->RestrictedSourceID.fValid             = FALSE;
    pcontainerGlobal->Expiration.fValid                     = FALSE;
    pcontainerGlobal->RealTimeExpiration.fValid             = FALSE;
    pcontainerGlobal->IssueDate.fValid                      = FALSE;
    pcontainerGlobal->RemovalDate.fValid                    = FALSE;
    pcontainerGlobal->GracePeriod.fValid                    = FALSE;
    pcontainerGlobal->Metering.fValid                       = FALSE;
    pcontainerGlobal->ExpirationAfterUse.fValid             = FALSE;
    pcontainerGlobal->ExpirationAfterStore.fValid           = FALSE;
    pcontainerGlobal->InclusionList.fValid                  = FALSE;
    pcontainerGlobal->RevocationInformationVersion.fValid   = FALSE;
    pcontainerGlobal->DomainID.fValid                       = FALSE;
    pcontainerGlobal->EmbeddingBehavior.fValid              = FALSE;
    pcontainerGlobal->pUnknownObjects                       = NULL;
    pcontainerGlobal->PolicyMetadata.fValid                 = FALSE;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbBuffer, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbBuffer, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbBuffer, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALID_LICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_SECURITY_LEVEL:
                ChkBOOL( !pcontainerGlobal->MinimumEnvironment.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainerGlobal->MinimumEnvironment ) );
                pcontainerGlobal->MinimumEnvironment.dwVersion = 2;
                break;
            case XMR_OBJECT_TYPE_SETTINGS_OBJECT:
                ChkBOOL( !pcontainerGlobal->Rights.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainerGlobal->Rights ) );
                break;
            case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT:
            case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT:
                ChkBOOL( !pcontainerGlobal->RevocationInformationVersion.fValid, DRM_E_INVALID_LICENSE);
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainerGlobal->RevocationInformationVersion ) );
                pcontainerGlobal->RevocationInformationVersion.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT ) ? (DRM_DWORD)1 : (DRM_DWORD)2;
                break;

            default:
                break;
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_KeyMaterialContainer
**
** Synopsis :   Parse a key material container object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_KeyMaterialContainer(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT            dr         = DRM_SUCCESS;
    DRM_DWORD             iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD              wFlags     = 0;
    DRM_WORD              wType      = 0;
    DRM_DWORD             cbChild    = 0;
    DRM_XMR_KEY_MATERIAL *pcontainer = ( DRM_XMR_KEY_MATERIAL * )f_pvObject;

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALID_LICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid                     = TRUE;
    pcontainer->ContentKey.fValid          = FALSE;
    pcontainer->OptimizedContentKey.fValid = FALSE;
    pcontainer->DeviceKey.fValid           = FALSE;
    pcontainer->ECCKey.fValid              = FALSE;
    pcontainer->UplinkKid.fValid           = FALSE;
    pcontainer->UplinkX.fValid             = FALSE;
    pcontainer->AuxKey.fValid              = FALSE;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbBuffer, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbBuffer, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbBuffer, iCurrent + ( SIZEOF( DRM_WORD ) * 2 ) );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALID_LICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT:
                ChkBOOL( !pcontainer->ContentKey.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainer->ContentKey ) );
                break;
            case XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT:
                ChkBOOL( !pcontainer->OptimizedContentKey.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainer->OptimizedContentKey ) );
                break;
            case XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT:
                ChkBOOL( !pcontainer->DeviceKey.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainer->DeviceKey ) );
                break;
            case XMR_OBJECT_TYPE_UPLINK_KID_OBJECT:
            case XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT:
                ChkBOOL( !pcontainer->UplinkKid.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainer->UplinkKid ) );
                pcontainer->UplinkKid.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_UPLINK_KID_OBJECT ) ? (DRM_DWORD)1 : (DRM_DWORD)2;
                break;
            case XMR_OBJECT_TYPE_UPLINKX_OBJECT:
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainer->UplinkX ) );
                break;
            case XMR_OBJECT_TYPE_AUX_KEY_OBJECT:
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                          &pcontainer->AuxKey ) );
                break;
            case XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT:
                ChkBOOL( !pcontainer->ECCKey.fValid, DRM_E_INVALID_LICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pbBuffer,
                                                          iCurrent,
                                                          cbChild,
                                                         &pcontainer->ECCKey ) );
                break;
            default:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALID_LICENSE );
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_DWORD_Versioned
**
** Synopsis :   Parse a dword object with version
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_DWORD_Versioned(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT     dr     = DRM_SUCCESS;
    DRM_XMR_DWORD_VERSIONED *pdword = ( DRM_XMR_DWORD_VERSIONED * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ), DRM_E_INVALID_LICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );

    pdword->fValid = TRUE;
    NETWORKBYTES_TO_DWORD( pdword->dwValue,
                           f_pbBuffer,
                           f_iObject + XMR_BASE_OBJECT_LENGTH );

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_ContentKey
**
** Synopsis :   Parse a content key object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_ContentKey(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT           dr          = DRM_SUCCESS;
    DRM_DWORD            iObject     = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_CONTENT_KEY *pContentKey = ( DRM_XMR_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH
                         + ( SIZEOF( DRM_WORD ) * 3 )
                         + SIZEOF( DRM_GUID ), DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    pContentKey->pbguidKeyID = ( DRM_BYTE * )f_pbBuffer;
    pContentKey->iguidKeyID  = iObject;
    ChkOverflow( iObject + SIZEOF( DRM_GUID ), iObject );
    iObject += SIZEOF( DRM_GUID );

    NETWORKBYTES_TO_WORD( pContentKey->wSymmetricCipherType, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pContentKey->wKeyEncryptionCipherType, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pContentKey->cbEncryptedKey, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkOverflow( f_iObject + f_cbObject, f_iObject );
    ChkOverflow( iObject + pContentKey->cbEncryptedKey, iObject );
    ChkBOOL( f_iObject + f_cbObject == iObject + pContentKey->cbEncryptedKey,
            DRM_E_INVALID_LICENSE );
    pContentKey->pbEncryptedKeyBuffer = ( DRM_BYTE * )f_pbBuffer;
    pContentKey->iEncryptedKey        = iObject;
    pContentKey->fValid               = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_Parse_RsaPubkey
**
** Synopsis :   Parse an RSA public key object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_RsaPubkey(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_RSA_PUBKEY *pRsaPubkey = ( DRM_XMR_RSA_PUBKEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) + SIZEOF( DRM_DWORD ),
             DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_DWORD( pRsaPubkey->dwExponent, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject );
    iObject += SIZEOF( DRM_DWORD );

    NETWORKBYTES_TO_WORD( pRsaPubkey->cbModulus, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkOverflow( f_iObject + f_cbObject, f_iObject );
    ChkOverflow( iObject + pRsaPubkey->cbModulus, iObject );
    ChkBOOL( f_iObject + f_cbObject == iObject + pRsaPubkey->cbModulus, DRM_E_INVALID_LICENSE );
    pRsaPubkey->pbModulusBuffer = ( DRM_BYTE * )f_pbBuffer;
    pRsaPubkey->iModulus        = iObject;
    pRsaPubkey->fValid          = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_Parse_UplinkKID
**
** Synopsis :   Parse an uplink KID object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_UplinkKID(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_UPLINK_KID *pUplinkKID = ( DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) + SIZEOF( DRM_GUID ),
             DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    pUplinkKID->pbguidUplinkKID = ( DRM_BYTE * )f_pbBuffer;
    pUplinkKID->iguidUplinkKID  = iObject;
    ChkOverflow( iObject + SIZEOF( DRM_GUID ), iObject );
    iObject += SIZEOF( DRM_GUID );

    NETWORKBYTES_TO_WORD( pUplinkKID->cbChainedCheckSum, f_pbBuffer,  iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    /* If we got this far, then (iObject-f_iObject) is always >0, thus no possibility of underflow.
    */
    ChkOverflow( iObject - f_iObject + pUplinkKID->cbChainedCheckSum, iObject - f_iObject );
    ChkBOOL( f_cbObject >= iObject - f_iObject + pUplinkKID->cbChainedCheckSum, DRM_E_INVALID_LICENSE );

    pUplinkKID->pbChainedCheckSumBuffer = ( DRM_BYTE * ) f_pbBuffer;
    pUplinkKID->iChainedCheckSum        = iObject;

    pUplinkKID->wChecksumType = XMR_UPLINK_CHECKSUM_TYPE_XMRV1;

    pUplinkKID->fValid = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_UplinkKID2(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_UPLINK_KID *pUplinkKID = ( DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + ( 2 * SIZEOF( DRM_WORD ) ) + SIZEOF( DRM_GUID ),
             DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    pUplinkKID->pbguidUplinkKID = ( DRM_BYTE * )f_pbBuffer;
    pUplinkKID->iguidUplinkKID  = iObject;
    ChkOverflow( iObject + SIZEOF( DRM_GUID ), iObject );
    iObject += SIZEOF( DRM_GUID );

    NETWORKBYTES_TO_WORD( pUplinkKID->wChecksumType, f_pbBuffer,  iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pUplinkKID->cbChainedCheckSum, f_pbBuffer,  iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    /* If we got this far, then (iObject-f_iObject) is always >0, thus no possibility of underflow.
    */
    ChkOverflow( iObject - f_iObject + pUplinkKID->cbChainedCheckSum, iObject - f_iObject);
    ChkBOOL( f_cbObject >= iObject - f_iObject + pUplinkKID->cbChainedCheckSum, DRM_E_INVALID_LICENSE );

    pUplinkKID->pbChainedCheckSumBuffer = ( DRM_BYTE * ) f_pbBuffer;
    pUplinkKID->iChainedCheckSum        = iObject;

    pUplinkKID->fValid = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_SecurityLevel(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT                   dr                  = DRM_SUCCESS;
    DRM_DWORD                    iObject             = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_MINIMUM_ENVIRONMENT *pMinimumEnvironment = ( DRM_XMR_MINIMUM_ENVIRONMENT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ),
             DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_WORD( pMinimumEnvironment->wMinimumSecurityLevel,
                          f_pbBuffer,
                          iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    pMinimumEnvironment->dwMinimumAppRevocationListVersion = 0;
    pMinimumEnvironment->dwMinimumDeviceRevocationListVersion = 0;
    pMinimumEnvironment->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_Parse_Signature
**
** Synopsis :   Parse a signature object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_Signature(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT         dr         = DRM_SUCCESS;
    DRM_DWORD          iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_SIGNATURE *pSignature = ( DRM_XMR_SIGNATURE * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + ( SIZEOF( DRM_WORD ) * 2 ),
             DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    pSignature->fValid = TRUE;
    NETWORKBYTES_TO_WORD( pSignature->wType, f_pbBuffer,  iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pSignature->cbSignature, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkOverflow( XMR_BASE_OBJECT_LENGTH + ( SIZEOF( DRM_WORD ) * 2 ) + pSignature->cbSignature, pSignature->cbSignature );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + ( SIZEOF( DRM_WORD ) * 2 ) + pSignature->cbSignature,
             DRM_E_INVALID_LICENSE );

    pSignature->pbSignatureBuffer = ( DRM_BYTE * )f_pbBuffer;
    pSignature->iSignature        = iObject;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_EccDevicekey
**
** Synopsis :   Parse an ECC device key object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_EccDevicekey(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT              dr      = DRM_SUCCESS;
    DRM_DWORD               iObject = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_ECC_DEVICE_KEY *pKey    = ( DRM_XMR_ECC_DEVICE_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ), DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    pKey->fValid      = TRUE;

    NETWORKBYTES_TO_WORD( pKey->wEccCurveType, f_pbBuffer,  iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pKey->cbKeyData, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    pKey->pbKeyData = ( DRM_BYTE * )f_pbBuffer;
    pKey->iKeyData = iObject;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_OptimizedContentKey
**
** Synopsis :   Parse a symmetrically optimized content key object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_OptimizedContentKey(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT                     dr          = DRM_SUCCESS;
    DRM_DWORD                      iObject     = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_OPTIMIZED_CONTENT_KEY *pContentKey = ( DRM_XMR_OPTIMIZED_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH
                         + ( SIZEOF( DRM_WORD ) * 2 ) , DRM_E_INVALID_LICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_WORD( pContentKey->wKeyEncryptionCipherType, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pContentKey->cbEncryptedKey, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkOverflow( f_iObject + f_cbObject, f_iObject );
    ChkOverflow( iObject + pContentKey->cbEncryptedKey, iObject );
    ChkBOOL( f_iObject + f_cbObject == iObject + pContentKey->cbEncryptedKey,
            DRM_E_INVALID_LICENSE );
    pContentKey->pbEncryptedKeyBuffer = ( DRM_BYTE * )f_pbBuffer;
    pContentKey->iEncryptedKey        = iObject;
    pContentKey->fValid               = TRUE;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_AuxKey
**
** Synopsis :   Parse an Aux Key object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_AuxKey(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iObject  = f_iObject;
    DRM_XMR_AUX_KEY *pAuxKey =
            ( DRM_XMR_AUX_KEY * )f_pvObject;

    ChkArg(  f_pbBuffer != NULL );
    ChkArg(  f_pvObject  != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH +
                           SIZEOF( DRM_WORD ),
             DRM_E_INVALID_LICENSE );

    ChkOverflow( iObject + XMR_BASE_OBJECT_LENGTH, iObject );
    iObject += XMR_BASE_OBJECT_LENGTH;

    NETWORKBYTES_TO_WORD( pAuxKey->cEntries, f_pbBuffer, iObject );

    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH +
                           SIZEOF( DRM_WORD ) +
                           pAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ),
             DRM_E_INVALID_LICENSE );

    pAuxKey->pEntries = ( DRM_XMR_AUX_KEY_ENTRY * )( f_pbBuffer + iObject );

    ChkOverflow( iObject + ( pAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) ), iObject );
    iObject += ( pAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) );

    pAuxKey->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_Parse_UplinkX
**
** Synopsis :   Parse an UplinkX object
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALID_LICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_UplinkX(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iObject  = f_iObject;
    DRM_XMR_UPLINKX *pUplinkX =
            ( DRM_XMR_UPLINKX * )f_pvObject;

    ChkArg(  f_pbBuffer != NULL );
    ChkArg(  f_pvObject  != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH +
                           SIZEOF( DRM_GUID ) +
                           SIZEOF( DRM_WORD ),
             DRM_E_INVALID_LICENSE );

    ChkOverflow( iObject + XMR_BASE_OBJECT_LENGTH, iObject );
    iObject += XMR_BASE_OBJECT_LENGTH;

    pUplinkX->pbguidUplinkKID = ( DRM_BYTE * )f_pbBuffer;
    pUplinkX->iguidUplinkKID  = iObject;

    ChkOverflow( iObject + SIZEOF( DRM_GUID ), iObject );
    iObject += SIZEOF( DRM_GUID );

    NETWORKBYTES_TO_WORD( pUplinkX->cbCheckSum, f_pbBuffer,  iObject );

    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH +
                           SIZEOF( DRM_GUID ) +
                           SIZEOF( DRM_WORD ) +
                           pUplinkX->cbCheckSum, DRM_E_INVALID_LICENSE );

    pUplinkX->pbCheckSumBuffer = ( DRM_BYTE * ) f_pbBuffer;
    pUplinkX->iCheckSum        = iObject;

    ChkOverflow( iObject + pUplinkX->cbCheckSum, iObject );
    iObject += pUplinkX->cbCheckSum;

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH +
                           SIZEOF( DRM_GUID ) +
                           SIZEOF( DRM_WORD ) +
                           pUplinkX->cbCheckSum +
                           SIZEOF( DRM_WORD ), DRM_E_INVALID_LICENSE );

    NETWORKBYTES_TO_WORD( pUplinkX->cEntries, f_pbBuffer, iObject );

    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH +
                           SIZEOF( DRM_GUID ) +
                           SIZEOF( DRM_WORD ) +
                           pUplinkX->cbCheckSum +
                           SIZEOF( DRM_WORD ) +
                           pUplinkX->cEntries * SIZEOF( DRM_DWORD ),
             DRM_E_INVALID_LICENSE );

    pUplinkX->pdwLocation = ( DRM_DWORD * )( f_pbBuffer + iObject );

    ChkOverflow( iObject + ( pUplinkX->cEntries * SIZEOF( DRM_DWORD ) ), iObject );
    iObject += ( pUplinkX->cEntries * SIZEOF( DRM_DWORD ) );

    pUplinkX->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _XMR_FindChild
**
** Synopsis :   Used by Parser to find a child in a node buffer
**
** Arguments :
**      [f_wObjectType]   : Object type to find
**      [f_pbBuffer]      : Buffer
**      [f_iCurrentNode]  : Buffer index marking start of parent node
**      [f_cbCurrentNode] : Size in bytes of parent node
**      [f_piObject]      : Buffer index marking start of required child
**      [f_pcbObject]     : Size in bytes of required child
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**      DRM_E_ARITHMETICOVERFLOW
**          An arithmetic overflow occurred during processing
**      DRM_E_XMR_OBJECT_NOT_FOUND
**          Failed to find the child node
**      DRM_E_INVALID_LICENSE
**          A child node was found that was too large for the current container
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _XMR_FindChild(
    __in        DRM_WORD         f_wType,
    __in  const DRM_BYTE        *f_pbBuffer,
    __in        DRM_DWORD        f_iCurrentNode,
    __in        DRM_DWORD        f_cbCurrentNode,
    __out       DRM_DWORD       *f_piChild,
    __out       DRM_DWORD       *f_pcbChild )
{
    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_DWORD   iNext     = f_iCurrentNode + XMR_BASE_OBJECT_LENGTH;
    DRM_DWORD   cbNext    = 0;
    DRM_WORD    wNextType = 0;

    ChkArg( f_pbBuffer  != NULL );
    ChkArg( f_piChild   != NULL );
    ChkArg( f_pcbChild  != NULL );

    ChkOverflow( iNext, f_iCurrentNode );
    ChkOverflow( f_iCurrentNode + f_cbCurrentNode, f_iCurrentNode );
    ChkOverflow( f_iCurrentNode + f_cbCurrentNode,
                 f_iCurrentNode + f_cbCurrentNode - XMR_BASE_OBJECT_LENGTH );   /* Underflow check */

    while ( wNextType != f_wType
          && iNext < ( f_iCurrentNode + f_cbCurrentNode - XMR_BASE_OBJECT_LENGTH ) )
    {
        ChkOverflow( iNext + SIZEOF( DRM_WORD ), iNext );
        NETWORKBYTES_TO_WORD( wNextType,
                              f_pbBuffer,
                              iNext + SIZEOF( DRM_WORD ) );
        ChkOverflow( iNext + SIZEOF( DRM_WORD ), iNext );
        NETWORKBYTES_TO_DWORD( cbNext,
                               f_pbBuffer,
                               iNext + SIZEOF( DRM_WORD )*2 );
        ChkBOOL( cbNext > 0, DRM_E_INVALID_LICENSE );
        ChkOverflow( iNext + cbNext, iNext );
        iNext += cbNext;
    }
    iNext -= cbNext;
    ChkBOOL( wNextType == f_wType, DRM_E_XMR_OBJECT_NOT_FOUND );
    ChkBOOL( cbNext <= ( f_iCurrentNode + f_cbCurrentNode - iNext ), DRM_E_INVALID_LICENSE );

    *f_piChild = iNext;
    *f_pcbChild = cbNext;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_FindObject
**
** Synopsis :   Used by Parser to find some object in a license buffer
**
** Arguments :
**      [f_wObjectType] : Object type to find
**      [f_pbBuffer]    : License buffer
**      [f_cbBuffer]    : License buffer size
**      [f_piObject]    : Buffer index marking start of required object
**      [f_pcbObject]   : Size in bytes of required object
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**      DRM_E_INVALID_LICENSE
**          The license buffer is an unexpected length
**      DRM_E_ARITHMETICOVERFLOW
**          An arithmetic overflow occurred during processing
**      DRM_E_XMR_OBJECT_NOT_FOUND
**          Failed to find the object
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_FindObject(
    __in                             DRM_WORD   f_wObjectType,
    __in_bcount( f_cbLicense ) const DRM_BYTE  *f_pbBuffer,
    __in                             DRM_DWORD  f_cbLicense,
    __out                            DRM_DWORD *f_piObject,
    __out                            DRM_DWORD *f_pcbObject )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_WORD    wParent     = f_wObjectType;
    DRM_WORD    wLevel      = 1;
    DRM_DWORD   iCurrent    = XMR_HEADER_LENGTH;
    DRM_DWORD   iNext       = XMR_HEADER_LENGTH;
    DRM_DWORD   cbCurrent   = 0;
    DRM_DWORD   cbNext      = 0;
    DRM_WORD    wHierarchy[XMR_MAXIMUM_OBJECT_DEPTH] = { 0 };

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piObject  != NULL );
    ChkArg( f_pcbObject != NULL );

    NETWORKBYTES_TO_DWORD( cbCurrent,
                           f_pbBuffer,
                           iCurrent + SIZEOF( DRM_WORD ) * 2 );

    ChkOverflow( cbCurrent + XMR_HEADER_LENGTH, cbCurrent );

    ChkBOOL( f_cbLicense >= (cbCurrent + XMR_HEADER_LENGTH), DRM_E_INVALID_LICENSE );

    for ( ; ( wParent != XMR_OBJECT_TYPE_OUTER_CONTAINER
         && wLevel   < XMR_MAXIMUM_OBJECT_DEPTH
         && wParent <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED ); wLevel++ )
    {
        wHierarchy[wLevel] = wParent;
        wParent = g_xmrCommonDefines[wParent].wParent;
    }
    wLevel--;

    /*
    **  First find node wHierarchy[wLevel], followed by wHierarchy[wLevel-1],
    **  etc till wHierarchy[1]
    */
    while ( wLevel > 0 && wLevel < XMR_MAXIMUM_OBJECT_DEPTH )
    {
        ChkDR( _XMR_FindChild( wHierarchy[wLevel],
                               f_pbBuffer,
                               iCurrent,
                               cbCurrent,
                               &iNext,
                               &cbNext ) );
        iCurrent = iNext;
        cbCurrent = cbNext;
        wLevel--;
    }
    *f_piObject  = iCurrent;
    *f_pcbObject = cbCurrent;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_SparseUnpackLicense
**
** Synopsis :   Parse the license, extracting those elements required for
**              OEM HAL license validation
**
** Arguments :
**      [f_pbBuffer]    : License buffer
**      [f_cbLicense]   : License buffer size
**      [f_pXmrLicense] : Returns the parsed license
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or invalid
**      DRM_E_INVALID_LICENSE
**          The license buffer does not specify a valid license
**
** Notes : This function does not unpack the entire XMR license: DRM_XMR_UnpackLicense
**         should be used for this. This function simply extracts the information
**         required by the OEM HAL layer to validate that the license is valid and
**         signed correctly
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_SparseUnpackLicense(
    __in_bcount( f_cbLicense ) const DRM_BYTE                    *f_pbBuffer,
    __in                             DRM_DWORD                    f_cbLicense,
    __out                            DRM_XMR_LICENSE             *f_pXmrLicense )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  dwMagicConstant  = 0;
    DRM_DWORD  cbOuterContainer = 0;

    DRMCASSERT( DRM_XMR_MAX_UNPACK_ALLOC_FACTOR >= DRM_XMR_COMPUTED_MAX_UNPACK_ALLOC_FACTOR );

    ChkArg( f_pbBuffer     != NULL );
    ChkArg( f_cbLicense     != 0 );
    ChkArg( f_pXmrLicense   != NULL );

    ChkBOOL( f_cbLicense > XMR_HEADER_LENGTH, DRM_E_INVALID_LICENSE );

    NETWORKBYTES_TO_DWORD( dwMagicConstant, f_pbBuffer, 0 );

    ChkBOOL( dwMagicConstant == XMR_MAGIC_CONSTANT, DRM_E_INVALID_LICENSE );

    NETWORKBYTES_TO_DWORD( f_pXmrLicense->dwVersion, f_pbBuffer, SIZEOF( DRM_DWORD ) );
    f_pXmrLicense->pbRightsIdBuffer = ( DRM_BYTE * ) f_pbBuffer;
    f_pXmrLicense->iRightsId        = XMR_RID_OFFSET;
    ChkDR( OEM_XMR_GetContainer( f_pbBuffer,
                                 f_cbLicense,
                                 XMR_OBJECT_TYPE_OUTER_CONTAINER,
                 ( DRM_VOID * ) &f_pXmrLicense->containerOuter ) );

    /*
    ** Verify that the length of the license buffer is consistent with the outer container size
    */
    NETWORKBYTES_TO_DWORD( cbOuterContainer, f_pbBuffer, XMR_HEADER_LENGTH + 2 * SIZEOF( DRM_WORD ) );
    ChkBOOL( cbOuterContainer + XMR_HEADER_LENGTH == f_cbLicense, DRM_E_INVALID_LICENSE );

    f_pXmrLicense->pbSignedDataBuffer = ( DRM_BYTE * )f_pbBuffer;
    f_pXmrLicense->iSignedData        = 0;
    f_pXmrLicense->cbSignedData       = f_cbLicense
                                      - XMR_BASE_OBJECT_LENGTH
                                      - SIZEOF( DRM_WORD ) * 2
                                      - f_pXmrLicense->containerOuter.signature.cbSignature;

    ChkDR( OEM_XMR_ValidateSparseLicenseObjects( f_pXmrLicense ) );

    f_pXmrLicense->pbXMRLic = ( DRM_BYTE * )f_pbBuffer;
    f_pXmrLicense->cbXMRLic = f_cbLicense;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   OEM_XMR_GetContainer
**
** Synopsis :   Parse the license to obtain a particular container
**
** Arguments :
**      [f_pbBuffer]       : License buffer
**      [f_cbLicense]      : License buffer size
**      [f_wContainerType] : Container type
**      [f_pvContainer]    : Container object passed as void
**
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or invalid
**      DRM_E_INVALID_LICENSE
**          The license buffer does not specify a valid license
**      DRM_E_ARITHMETICOVERFLOW
**          An arithmetic overflow occurred during processing
**      DRM_E_XMR_OBJECT_NOT_FOUND
**          Failed to find the container object
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_GetContainer(
    __in    const DRM_BYTE                     *f_pbBuffer,
    __in          DRM_DWORD                     f_cbLicense,
    __in          DRM_WORD                      f_wContainerType,
    __inout       DRM_VOID                     *f_pvContainer )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_DWORD  iContainer      = 0;
    DRM_DWORD  cbContainer     = 0;
    DRM_DWORD  dwMagicConstant = 0;

    ChkArg( f_pbBuffer   != NULL );
    ChkArg( f_cbLicense   != 0 );
    ChkArg( f_pvContainer != NULL );
    ChkArg( f_wContainerType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );
    ChkArg( g_xmrCommonDefines[f_wContainerType].fIsContainer );

    ChkBOOL( f_cbLicense > XMR_HEADER_LENGTH, DRM_E_INVALID_LICENSE );

    NETWORKBYTES_TO_DWORD( dwMagicConstant, f_pbBuffer, 0 );
    ChkBOOL( dwMagicConstant == XMR_MAGIC_CONSTANT, DRM_E_INVALID_LICENSE );

    ChkDR( OEM_XMR_FindObject( f_wContainerType,
                               f_pbBuffer,
                               f_cbLicense,
                               &iContainer,
                               &cbContainer ) );
    ChkDR( g_xmrParserDefines[f_wContainerType].fnParse( f_pbBuffer,
                                                         iContainer,
                                                         cbContainer,
                                                         f_pvContainer ) );

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   OEM_XMR_Parse_Ignore
**
** Synopsis :   Parse an object that should be ignored
**
** Arguments :
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_E_LOGICERR
**          This should never be called
**
** Notes :  Prototyped in oemsparsexmrparser.h
**
******************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: f_pvObject cannot be const for function to match parsing fn prototype")
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_Parse_Ignore(
    __in    const DRM_BYTE                    *f_pbBuffer,
    __in          DRM_DWORD                    f_iObject,
    __in          DRM_DWORD                    f_cbObject,
    __inout       DRM_VOID                    *f_pvObject )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_E_LOGICERR;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH, DRM_E_INVALID_LICENSE );

ErrorExit:
    return dr;
}


/*
** Some objects are simply required.
** For V1 XMR, certain licenses from legacy tuners are incomplete,
** so we skip this validation for V1 XMR.
*/
#define VALIDATE_REQUIRED_OBJECT( pXmrLicense, IS_VALID_MACRO )             \
    ChkBOOL( ( f_pXmrLicense->dwVersion == XMR_VERSION_1 ) || IS_VALID_MACRO( pXmrLicense ), DRM_E_INVALID_LICENSE )

/* Use the look-up table to see if the object is supported for the given version */
#define VALIDATE_OPTIONAL_OBJECT( pXmrLicense, IS_VALID_MACRO, ObjType )                                            \
    ChkBOOL(                                                                                                        \
        !IS_VALID_MACRO( pXmrLicense )                                                                              \
     || g_xmrParserDefines[ObjType].rgfIsVersionSupported[(pXmrLicense)->dwVersion - 1], DRM_E_INVALID_LICENSE )

#define VALIDATE_RIGHTS_MASK( pXmrLicense, wValidMask )                                                                             \
    ChkBOOL(                                                                                                                        \
        ( (pXmrLicense)->containerOuter.containerGlobalPolicies.Rights.wValue == 0 )                                                \
     || ( (pXmrLicense)->containerOuter.containerGlobalPolicies.Rights.wValue                                                       \
     == ( (pXmrLicense)->containerOuter.containerGlobalPolicies.Rights.wValue & (DRM_WORD)(wValidMask) ) ), DRM_E_INVALID_LICENSE )

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_XMR_ValidateSparseLicenseObjects(
    __in const DRM_XMR_LICENSE *f_pXmrLicense )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pXmrLicense != NULL );

    /* XMR_VERSION_2 is not supported */
    ChkBOOL(
        ( f_pXmrLicense->dwVersion == XMR_VERSION_1 )
     || ( f_pXmrLicense->dwVersion == XMR_VERSION_3 ), DRM_E_INVALID_LICENSE );

    /* RID (LID) is required - this also validates that the outer container is valid */
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_RID_VALID );

    /* Validate the Global Policy Container Object */
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_GLOBAL_POLICIES_VALID );
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_MINIMUM_ENVIRONMENT_VALID );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_RIGHTS_VALID,                           XMR_OBJECT_TYPE_SETTINGS_OBJECT );
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_REVOCATION_INFORMATION_VERSION_VALID );

    /* Validate the Key Container Object */
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_KEY_MATERIAL_VALID ); /* All licenses require key material */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_CONTENT_KEY_VALID,                      XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT  ); /* Almost all licenses require a content key. License that are used during regeneration do not need a content key */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_OPTIMIZED_CONTENT_KEY_VALID,            XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_DEVICE_KEY_VALID,                       XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_ECC_DEVICE_KEY_VALID,                   XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_UPLINKX_VALID,                          XMR_OBJECT_TYPE_UPLINKX_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_AUX_KEY_VALID,                          XMR_OBJECT_TYPE_AUX_KEY_OBJECT );

    /* Validate the Signature Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_SIGNATURE_VALID,                        XMR_OBJECT_TYPE_SIGNATURE_OBJECT );

    /* Validate the license doesn't have both uplink KID and uplinkX objects */
    ChkBOOL( !XMR_IS_UPLINK_KID_VALID( f_pXmrLicense ) || !XMR_IS_UPLINKX_VALID( f_pXmrLicense ), DRM_E_INVALID_LICENSE );

    if( XMR_IS_CONTENT_KEY_VALID( f_pXmrLicense ) )
    {
        /* Validate that the license has a valid encryption type */
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            ChkBOOL(
                ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER  )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB ), DRM_E_INVALID_LICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL(
                ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL    ), DRM_E_INVALID_LICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    /* Validate the rights mask values */
    if( XMR_IS_RIGHTS_VALID( f_pXmrLicense ) )
    {
        /* Validate that the rights mask takes only valid values for the license version */
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            /* Don't validate rights mask for V1 licenses for compat with legacy components */
            break;
        case XMR_VERSION_3:
            VALIDATE_RIGHTS_MASK(
                f_pXmrLicense,
                ( XMR_RIGHTS_CANNOT_PERSIST
                | XMR_RIGHTS_CANNOT_BIND_LICENSE
                | XMR_RIGHTS_TEMP_STORE_ONLY ) );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    /* Validate if the license has a uplinkx then it has to have a valid UplinkKID pointer in the uplinkx object */
    if ( XMR_IS_UPLINKX_VALID( f_pXmrLicense ) )
    {
        DRM_GUID guidEmpty = EMPTY_DRM_GUID;

        ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.UplinkX.pbguidUplinkKID
                        + f_pXmrLicense->containerOuter.containerKeys.UplinkX.iguidUplinkKID != NULL,
                        DRM_E_INVALID_LICENSE );

        ChkBOOL( !OEM_SECURE_ARE_EQUAL( &f_pXmrLicense->containerOuter.containerKeys.UplinkX.pbguidUplinkKID[f_pXmrLicense->containerOuter.containerKeys.UplinkX.iguidUplinkKID],
                                        &guidEmpty,
                                        SIZEOF( DRM_GUID ) ), DRM_E_INVALID_LICENSE );
    }

ErrorExit:
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;

