/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontextsizes.h>
#include <byteorder.h>
#include <drmxmrparser.h>

ENTER_PK_NAMESPACE_CODE;

static const XMR_PARSER_INTERNAL_DEFINES g_xmrParserDefines[XMR_OBJECT_TYPE_MAXIMUM_DEFINED+1] =
{
                                                                   /*  V1     V2     V3 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_INVALID */
    { DRM_XMR_Parse_OuterContainer,                                {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_OUTER_CONTAINER */
    { DRM_XMR_Parse_GlobalContainer,                               {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER */
    { DRM_XMR_Parse_MinimumEnvironment,                            {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT */
    { DRM_XMR_Parse_PlaybackContainer,                             {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
    { DRM_XMR_Parse_OPLs,                                          {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT */
    { DRM_XMR_Parse_UplinkKID,                                     {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_UPLINK_KID_OBJECT    */
    { DRM_XMR_Parse_ExplicitAnalogVideoOutputProtectionContainer,  {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER */
    { DRM_XMR_Parse_VideoOutputConfig,                             {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT */
    { DRM_XMR_Parse_KeyMaterialContainer,                          {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER */
    { DRM_XMR_Parse_ContentKey,                                    {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT */
    { DRM_XMR_Parse_Signature,                                     {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SIGNATURE_OBJECT */
    { DRM_XMR_Parse_BYTEARRAY,                                     {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT */
    { DRM_XMR_Parse_WORD,                                          {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SETTINGS_OBJECT */
    { DRM_XMR_Parse_CopyContainer,                                 {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER */
    { DRM_XMR_Parse_PlaylistBurnContainer,                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER */
    { DRM_XMR_Parse_GUIDLIST,                                      {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT   */
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_PRIORITY_OBJECT  */
    { DRM_XMR_Parse_Expiration,                                    {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPIRATION_OBJECT    */
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_ISSUEDATE_OBJECT */
    { DRM_XMR_Parse_DWORD_Versioned,                               {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT */
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT   */
    { DRM_XMR_Parse_GUID,                                          {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_METERING_OBJECT  */
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_EXCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_INCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT */
    { DRM_XMR_Parse_DWORD_Versioned,                               {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPYCOUNT_OBJECT */
    { DRM_XMR_Parse_WORD,                                          {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT   */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_COPY_EXCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_COPY_INCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { DRM_XMR_Parse_PlaylistBurnRights,                            {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT    */
    { DRM_XMR_Parse_DWORD_Versioned,                               {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT */
    { DRM_XMR_Parse_RsaPubkey,                                     {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT */
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SOURCEID_OBJECT  */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_EXPLICIT_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER (Shipped in Emerald Beta 1)*/
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Deprecated: XMR_OBJECT_TYPE_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT (Shipped in Emerald Beta 1)*/
    { DRM_XMR_Parse_RevocationContainer,                           {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_REVOCATION_CONTAINER */
    { DRM_XMR_Parse_RsaPubkey,                                     {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT */
    { DRM_XMR_Parse_UserID,                                        {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_USERID_OBJECT */
    { DRM_XMR_Parse_Empty,                                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT  */
    { DRM_XMR_Parse_DomainID,                                      { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT  */
    { DRM_XMR_Parse_EccDevicekey,                                  { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT */
    { DRM_XMR_Parse_DWORD,                                         { FALSE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT */
    { DRM_XMR_Parse_PolicyMetadata,                                {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT */
    { DRM_XMR_Parse_OptimizedContentKey,                           { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT */
    { DRM_XMR_Parse_ExplicitDigitalAudioOutputProtectionContainer, {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER */
    { DRM_XMR_Parse_UnknownContainer,                              { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER */
    { DRM_XMR_Parse_DWORD_Versioned,                               { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT */
    { DRM_XMR_Parse_AudioOutputConfig,                             {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT */
    { DRM_XMR_Parse_DWORD_Versioned,                               { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT */
    { DRM_XMR_Parse_WORD,                                          { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT */
    { DRM_XMR_Parse_SecurityLevel,                                 { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_SECURITY_LEVEL */
    { DRM_XMR_Parse_CopyToPCContainer,                             {  TRUE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER */
    { DRM_XMR_Parse_UnknownContainer,                              { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER */
    { DRM_XMR_Parse_DWORD,                                         {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT */
    { DRM_XMR_Parse_UnknownContainer,                              {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER */
    { DRM_XMR_Parse_UnknownObject,                                 { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT */
    { DRM_XMR_Parse_UnknownObject,                                 {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT */
    { DRM_XMR_Parse_UplinkKID2,                                    { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT */
    { DRM_XMR_Parse_CopyContainer,                                 {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER */
    { DRM_XMR_Parse_DWORD_Versioned,                               {  TRUE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_READ_POLICY_CONTAINER */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_42 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_43 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_44 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_45 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_46 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_47 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_48 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_49 */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4A */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4B */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4C */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4D */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4E */
    { DRM_XMR_Parse_Invalid,                                       { FALSE, FALSE, FALSE } },   /* Reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4F */
    { DRM_XMR_Parse_RemovalDate,                                   { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT */
    { DRM_XMR_Parse_AuxKey,                                        { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_AUX_KEY_OBJECT */
    { DRM_XMR_Parse_UplinkX,                                       { FALSE, FALSE,  TRUE } },   /* XMR_OBJECT_TYPE_UPLINKX_OBJECT */
};

/******************************************************************************
**
** Function :   DRM_XMR_Parse_Invalid
**
** Synopsis :   Returns DRM_E_INVALIDARG
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
**      [f_pbBuffer] : Object Buffer
**      [f_iObject]  : Index marking beginning of object in the buffer
**      [f_cbObject] : Size of serialized object
**      [f_pvObject] : Actual object returned as void
**
** Returns :
**      DRM_E_INVALIDARG
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_Invalid(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    return DRM_E_INVALIDARG;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_OuterContainer
**
** Synopsis :   Parse an outer container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_OuterContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT               dr = DRM_SUCCESS;
    DRM_DWORD                iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD                 wFlags = 0;
    DRM_WORD                 wType = 0;
    DRM_DWORD                cbChild    = 0;
    DRM_XMR_OUTER_CONTAINER *pContainer = ( DRM_XMR_OUTER_CONTAINER * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
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
        NETWORKBYTES_TO_WORD( wFlags,  f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType,   f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + f_cbObject - iCurrent ), DRM_E_INVALIDLICENSE );
        
        switch ( wType )
        {
            case XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER:
                ChkBOOL( !pContainer->containerGlobalPolicies.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerGlobalPolicies ) );
                break;

            case XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER:
                ChkBOOL( !pContainer->containerPlaybackPolicies.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerPlaybackPolicies ) );
                break;

            case XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER:
            case XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER:
                ChkBOOL( !pContainer->containerCopyPolicies.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerCopyPolicies ) );
                pContainer->containerCopyPolicies.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER ) ? 1 : 2;

                /* Make sure that the Copy Count, if present, is the same version as the container */
                ChkBOOL(
                    !pContainer->containerCopyPolicies.CopyCount.fValid
                 || pContainer->containerCopyPolicies.dwVersion == pContainer->containerCopyPolicies.CopyCount.dwVersion, DRM_E_INVALIDLICENSE );

                break;

            case XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER:
                ChkBOOL( !pContainer->containerCopyToPCPolicies.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerCopyToPCPolicies ) );
                break;

            case XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER:
                ChkBOOL( !pContainer->containerPlaylistBurnPolicies.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerPlaylistBurnPolicies ) );
                break;

            case XMR_OBJECT_TYPE_REVOCATION_CONTAINER:
                ChkBOOL( !pContainer->containerRevocation.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerRevocation ) );
                break;

            case XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER:
                ChkBOOL( !pContainer->containerKeys.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->containerKeys ) );
                break;

            case XMR_OBJECT_TYPE_SIGNATURE_OBJECT:
                ChkBOOL( !pContainer->signature.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->signature ) );
                break;

            case XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT:
                ChkBOOL( !pContainer->generationNumber.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pContainer->generationNumber ) );
                break;

            default:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
                
                /*
                ** Only keep external-parse containers.
                ** We can't act on any other objects and shouldn't
                ** waste time/effort trying to parse them.
                */
                if( ( ( wFlags & XMR_FLAGS_CONTAINER ) != 0 )
                 && ( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 ) )
                {
                    if( !pContainer->containerUnknown.fValid )
                    {
                        ZEROMEM( &pContainer->containerUnknown, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                        /* It's the first container, no need to allocate */
                        ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                                f_pbLicense,
                                iCurrent,
                                cbChild,
                               &pContainer->containerUnknown ) );
                    }
                    else
                    {
                        /* It's another container, need to allocated another entry in the linked list */
                        DRM_XMR_UNKNOWN_CONTAINER *pUnknownContainer = NULL;

                        ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                      SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),
                                                      SIZEOF( DRM_DWORD ),
                                                      NULL,
                                                      ( DRM_VOID ** ) &pUnknownContainer ) );

                        ZEROMEM( pUnknownContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                        ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                                    f_pbLicense,
                                    iCurrent,
                                    cbChild,
                                    pUnknownContainer ) );

                        pUnknownContainer->pNext = pContainer->containerUnknown.pNext;
                        pContainer->containerUnknown.pNext = pUnknownContainer;
                    }
                }
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
** Function :   DRM_XMR_Parse_GlobalContainer
**
** Synopsis :   Parse a global container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_GlobalContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_DWORD   iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD    wFlags   = 0;
    DRM_WORD    wType    = 0;
    DRM_DWORD   cbChild  = 0;
    DRM_XMR_UNKNOWN_OBJECT *pUnknownObject = NULL;
    DRM_XMR_GLOBAL_REQUIREMENTS *pcontainerGlobal =
            ( DRM_XMR_GLOBAL_REQUIREMENTS * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
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
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT:
            case XMR_OBJECT_TYPE_SECURITY_LEVEL:
                ChkBOOL( !pcontainerGlobal->MinimumEnvironment.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->MinimumEnvironment ) );
                pcontainerGlobal->MinimumEnvironment.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT ) ? 1 : 2;
                break;
            case XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT:
                ChkBOOL( !pcontainerGlobal->SerialNumber.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->SerialNumber ) );
                break;
            case XMR_OBJECT_TYPE_SETTINGS_OBJECT:
                ChkBOOL( !pcontainerGlobal->Rights.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->Rights ) );
                break;
            case XMR_OBJECT_TYPE_PRIORITY_OBJECT:
                ChkBOOL( !pcontainerGlobal->Priority.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->Priority ) );
                break;

            case XMR_OBJECT_TYPE_EXPIRATION_OBJECT:
                ChkBOOL( !pcontainerGlobal->Expiration.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->Expiration ) );
                break;
            case XMR_OBJECT_TYPE_ISSUEDATE_OBJECT:
                ChkBOOL( !pcontainerGlobal->IssueDate.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->IssueDate ) );
                break;
            case XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT:
                ChkBOOL( !pcontainerGlobal->RemovalDate.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->RemovalDate ) );
                    break;
            case XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT:
                ChkBOOL( !pcontainerGlobal->GracePeriod.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->GracePeriod ) );
                break;
            case XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT:
            case XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT:
                ChkBOOL( !pcontainerGlobal->ExpirationAfterUse.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->ExpirationAfterUse ) );
                pcontainerGlobal->ExpirationAfterUse.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT ) ? 1 : 2;
                break;
            case XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT:
                ChkBOOL( !pcontainerGlobal->ExpirationAfterStore.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->ExpirationAfterStore ) );
                break;
            case XMR_OBJECT_TYPE_METERING_OBJECT:
                ChkBOOL( !pcontainerGlobal->Metering.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->Metering ) );
                break;
            case XMR_OBJECT_TYPE_SOURCEID_OBJECT:
                ChkBOOL( !pcontainerGlobal->SourceID.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->SourceID ) );
                break;
            case XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT:
                ChkBOOL( !pcontainerGlobal->RestrictedSourceID.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->RestrictedSourceID ) );
                break;
            case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT:
            case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT:
                ChkBOOL( !pcontainerGlobal->RevocationInformationVersion.fValid, DRM_E_INVALIDLICENSE);
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->RevocationInformationVersion ) );
                pcontainerGlobal->RevocationInformationVersion.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT ) ? 1 : 2;
                break;
            case XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT:
                ChkBOOL( !pcontainerGlobal->InclusionList.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->InclusionList ) );
                break;

            case XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT:
                ChkBOOL( !pcontainerGlobal->DomainID.fValid, DRM_E_INVALIDLICENSE);
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->DomainID ) );
                break;
            case XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT:
                ChkBOOL( !pcontainerGlobal->EmbeddingBehavior.fValid, DRM_E_INVALIDLICENSE);
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->EmbeddingBehavior ) );
                break;
            case XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT:
                /* dont' check for  !pcontainerGlobal->PolicyMetadata.fValid as it may
                   have been set to true for a previous Policy Metadata object */
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainerGlobal->PolicyMetadata ) );
                break;
            default:
                /*
                ** We don't understand it, but if it allows us to try to externally
                ** parse the object then we may be able to continue
                */
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0
                       || ( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 && ( wFlags & XMR_FLAGS_CONTAINER ) == 0 ), DRM_E_INVALIDLICENSE );

                /* Only bother keeping it if it's allowed to be parsed externally, otherwise
                ** we may as well ignore it since we can't do anything else with it */
                if( ( wFlags & XMR_FLAGS_CONTAINER ) == 0
                 && ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
                {
                    /* Alloc space and add to linked list */
                    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                  SIZEOF( DRM_XMR_UNKNOWN_OBJECT ),
                                                  SIZEOF( DRM_DWORD ),
                                                  NULL,
                                                  ( DRM_VOID ** ) &pUnknownObject ) );

                    ZEROMEM( pUnknownObject, SIZEOF( DRM_XMR_UNKNOWN_OBJECT ) );

                    ChkDR( DRM_XMR_Parse_UnknownObject( f_pStack,
                                                        f_pbLicense,
                                                        iCurrent,
                                                        cbChild,
                                                        pUnknownObject ) );

                    pUnknownObject->pNext = pcontainerGlobal->pUnknownObjects;
                    pcontainerGlobal->pUnknownObjects = pUnknownObject;
                }
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
** Function :   DRM_XMR_Parse_PlaybackContainer
**
** Synopsis :   Parse a playback container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_PlaybackContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD   wFlags   = 0;
    DRM_WORD   wType    = 0;
    DRM_DWORD  cbChild  = 0;
    DRM_XMR_UNKNOWN_OBJECT *pUnknownObject = NULL;

    DRM_XMR_PLAYBACK_RIGHTS *pcontainer =
            ( DRM_XMR_PLAYBACK_RIGHTS * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid                                          = TRUE;
    pcontainer->PlayCount.fValid                                = FALSE;
    pcontainer->opl.fValid                                      = FALSE;
    pcontainer->containerExplicitAnalogVideoProtection.fValid   = FALSE;
    pcontainer->containerExplicitDigitalAudioProtection.fValid  = FALSE;
    pcontainer->pUnknownObjects                                 = NULL;
    ZEROMEM( &pcontainer->UnknownContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT:
                ChkBOOL( !pcontainer->PlayCount.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->PlayCount ) );
                break;
            case XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT:
                ChkBOOL( !pcontainer->opl.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->opl ) );
                break;
            case XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER:
                ChkBOOL( !pcontainer->containerExplicitAnalogVideoProtection.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->containerExplicitAnalogVideoProtection ) );
                break;
            case XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER:
                ChkBOOL( !pcontainer->containerExplicitDigitalAudioProtection.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->containerExplicitDigitalAudioProtection ) );
                break;

            case XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
                ChkBOOL( ( wFlags & XMR_FLAGS_CONTAINER ) != 0, DRM_E_INVALIDLICENSE );

                if( !pcontainer->UnknownContainer.fValid )
                {
                    ZEROMEM( &pcontainer->UnknownContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                    ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                            f_pbLicense,
                            iCurrent,
                            cbChild,
                           &pcontainer->UnknownContainer ) );
                }
                else
                {
                    DRM_XMR_UNKNOWN_CONTAINER *pUnkContainer = NULL;

                    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                  SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),
                                                  SIZEOF( DRM_DWORD ),
                                                  NULL,
                                                  ( DRM_VOID ** ) &pUnkContainer ) );

                    ZEROMEM( pUnkContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                    ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                                f_pbLicense,
                                iCurrent,
                                cbChild,
                                pUnkContainer ) );
                    pUnkContainer->pNext = pcontainer->UnknownContainer.pNext;
                    pcontainer->UnknownContainer.pNext = pUnkContainer;
                }
                break;

            default:
                /*
                ** We don't understand it, but if it allows us to try to externally
                ** parse the object then we may be able to continue
                */
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0
                       || ( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 && ( wFlags & XMR_FLAGS_CONTAINER ) == 0 ), DRM_E_INVALIDLICENSE );

                /* Only bother keeping it if it's allowed to be parsed externally, otherwise
                ** we may as well ignore it since we can't do anything else with it */
                if( ( wFlags & XMR_FLAGS_CONTAINER ) == 0
                 && ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
                {
                    /* Alloc space and add to linked list */
                    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                  SIZEOF( DRM_XMR_UNKNOWN_OBJECT ),
                                                  SIZEOF( DRM_DWORD ),
                                                  NULL,
                                                  ( DRM_VOID ** ) &pUnknownObject ) );

                    ZEROMEM( pUnknownObject, SIZEOF( DRM_XMR_UNKNOWN_OBJECT ) );

                    ChkDR( DRM_XMR_Parse_UnknownObject( f_pStack,
                                                        f_pbLicense,
                                                        iCurrent,
                                                        cbChild,
                                                        pUnknownObject ) );

                    pUnknownObject->pNext = pcontainer->pUnknownObjects;
                    pcontainer->pUnknownObjects = pUnknownObject;
                }

                break;
        }
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_ExplicitAnalogVideoOutputProtectionContainer
**
** Synopsis :   Parse an analog video output protection container object
**
** Arguments :
**      [f_pStack]    : Stack for intermediate usage
**      [f_pbLicense] : Base address in license
**      [f_iObject]   : Index marking beginning of object in the buffer
**      [f_cbObject]  : Size of serialized object
**      [f_pvObject]  : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL
DRM_XMR_Parse_ExplicitAnalogVideoOutputProtectionContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD   wFlags   = 0;
    DRM_WORD   wType    = 0;
    DRM_DWORD  cbChild  = 0;
    DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION *pcontainer =
            ( DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION * )f_pvObject;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST  *pListNode  = NULL;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid = TRUE;
    pcontainer->cOutputProtectionIDs        = 0;
    pcontainer->cbRawData = f_cbObject - XMR_BASE_OBJECT_LENGTH;
    pcontainer->pbRawData = f_pbLicense;
    pcontainer->iRawData  = f_iObject + XMR_BASE_OBJECT_LENGTH;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT:
                ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                              SIZEOF( DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST ),
                                              SIZEOF( DRM_DWORD ),
                                              NULL,
                                              ( DRM_VOID ** )&pListNode ) );

                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pListNode->Config ) );
                pListNode->pNext = pcontainer->plistOutputConfiguration;
                pcontainer->plistOutputConfiguration = pListNode;
                pcontainer->cOutputProtectionIDs++;
                break;

            default:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_ExplicitDigitalAudioOutputProtectionContainer
**
** Synopsis :   Parse a digital audio output protection container object
**
** Arguments :
**      [f_pStack]    : Stack for intermediate usage
**      [f_pbLicense] : Base address in license
**      [f_iObject]   : Index marking beginning of object in the buffer
**      [f_cbObject]  : Size of serialized object
**      [f_pvObject]  : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL
DRM_XMR_Parse_ExplicitDigitalAudioOutputProtectionContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iCurrent = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD   wFlags   = 0;
    DRM_WORD   wType    = 0;
    DRM_DWORD  cbChild  = 0;
    DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION *pcontainer =
            ( DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION * )f_pvObject;
    DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST  *pListNode  = NULL;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid = TRUE;
    pcontainer->cOutputProtectionIDs        = 0;
    pcontainer->cbRawData = f_cbObject - XMR_BASE_OBJECT_LENGTH;
    pcontainer->pbRawData = f_pbLicense;
    pcontainer->iRawData  = f_iObject + XMR_BASE_OBJECT_LENGTH;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT:
                ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                              SIZEOF( DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST ),
                                              SIZEOF( DRM_DWORD ),
                                              NULL,
                                              ( DRM_VOID ** ) &pListNode ) );

                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pListNode->Config ) );
                pListNode->pNext = pcontainer->plistOutputConfiguration;
                pcontainer->plistOutputConfiguration = pListNode;
                pcontainer->cOutputProtectionIDs++;
                break;

            default:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_PolicyMetadata
**
** Synopsis :   Parse the policy metadata object
**
** Arguments :
**      [f_pStack]    : Stack for intermediate usage
**      [f_pbLicense] : Base address in license
**      [f_iObject]   : Index marking beginning of object in the buffer
**      [f_cbObject]  : Size of serialized object
**      [f_pvObject]  : Actual object returned as void
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_PolicyMetadata(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_XMR_POLICY_METADATA *pMetaData =
            ( DRM_XMR_POLICY_METADATA * )f_pvObject;
    DRM_XMR_POLICY_METADATA_LIST  *pListNode  = NULL;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_GUID ),
             DRM_E_INVALIDLICENSE );

    /* Since there can be multiple Policy Metadata objects, this function can get called multiple times
        ** Initialize the member variables only on the first call */
    if( pMetaData->fValid == FALSE )
    {
        pMetaData->fValid = TRUE;
        pMetaData->cPolicyMetadataObjects = 0;
        pMetaData->plistPolicyMetadataObjects = NULL;
    }

    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                  SIZEOF( DRM_XMR_POLICY_METADATA_LIST ),
                                  SIZEOF( DRM_DWORD ),
                                  NULL,
                                  ( DRM_VOID ** ) &pListNode ) );

    pListNode->MetadataObject.pMetadataTypeGuidBuffer = ( DRM_BYTE * )f_pbLicense;
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );
    pListNode->MetadataObject.iMetadataTypeGuid       = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pListNode->MetadataObject.pbPolicyDataBuffer      = ( DRM_BYTE * )f_pbLicense;
    ChkOverflow( pListNode->MetadataObject.iMetadataTypeGuid + SIZEOF( DRM_GUID ),pListNode->MetadataObject.iMetadataTypeGuid );
    pListNode->MetadataObject.iPolicyData             = pListNode->MetadataObject.iMetadataTypeGuid + SIZEOF( DRM_GUID );
    pListNode->MetadataObject.cbPolicyData            = ( f_cbObject - XMR_BASE_OBJECT_LENGTH ) - SIZEOF( DRM_GUID );
    pListNode->MetadataObject.fValid                  = TRUE;

    pListNode->pNext                                  = pMetaData->plistPolicyMetadataObjects;

    pMetaData->plistPolicyMetadataObjects             = pListNode;
    ChkOverflow( pMetaData->cPolicyMetadataObjects + 1, pMetaData->cPolicyMetadataObjects );
    pMetaData->cPolicyMetadataObjects++;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_RevocationContainer
**
** Synopsis :   Parse a revocation container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_RevocationContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD            wFlags     = 0;
    DRM_WORD            wType      = 0;
    DRM_DWORD           cbChild    = 0;
    DRM_XMR_REVOCATION *pcontainer = ( DRM_XMR_REVOCATION * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid                       = TRUE;
    pcontainer->RsaLicenseGranterKey.fValid  = FALSE;
    pcontainer->UserID.fValid                = FALSE;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + ( SIZEOF( DRM_WORD ) * 2 ) );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {

            case XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT:
                ChkBOOL( !pcontainer->RsaLicenseGranterKey.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->RsaLicenseGranterKey ) );
                break;
            case XMR_OBJECT_TYPE_USERID_OBJECT:
                ChkBOOL( !pcontainer->UserID.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->UserID ) );
                break;
            default:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_KeyMaterialContainer
**
** Synopsis :   Parse a key material container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_KeyMaterialContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT            dr         = DRM_SUCCESS;
    DRM_DWORD             iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD              wFlags     = 0;
    DRM_WORD              wType      = 0;
    DRM_DWORD             cbChild    = 0;
    DRM_XMR_KEY_MATERIAL *pcontainer = ( DRM_XMR_KEY_MATERIAL * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
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
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + ( SIZEOF( DRM_WORD ) * 2 ) );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT:
                ChkBOOL( !pcontainer->ContentKey.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->ContentKey ) );
                break;
            case XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT:
                ChkBOOL( !pcontainer->OptimizedContentKey.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->OptimizedContentKey ) );
                break;
            case XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT:
                ChkBOOL( !pcontainer->DeviceKey.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->DeviceKey ) );
                break;
            case XMR_OBJECT_TYPE_UPLINK_KID_OBJECT:
            case XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT:
                ChkBOOL( !pcontainer->UplinkKid.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->UplinkKid ) );
                pcontainer->UplinkKid.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_UPLINK_KID_OBJECT ) ? 1 : 2;
                break;
            case XMR_OBJECT_TYPE_UPLINKX_OBJECT:
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->UplinkX ) );
                break;
            case XMR_OBJECT_TYPE_AUX_KEY_OBJECT:
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                          f_pbLicense,
                                                          iCurrent,
                                                          cbChild,
                                                          &pcontainer->AuxKey ) );
                break;
            case XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT:
                ChkBOOL( !pcontainer->ECCKey.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->ECCKey ) );
                break;
            default:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
        };
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_CopyToPCContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE                    *f_pbLicense,
    IN          DRM_DWORD                    f_iObject,
    IN          DRM_DWORD                    f_cbObject,
    IN  OUT     DRM_VOID                    *f_pvObject )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_DWORD            iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD             wFlags     = 0;
    DRM_WORD             wType      = 0;
    DRM_DWORD            cbChild    = 0;
    DRM_XMR_COPY_TO_PC  *pcontainer = ( DRM_XMR_COPY_TO_PC * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid  = TRUE;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        /* We don't understand any children for this object */
        ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );

        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_CopyContainer
**
** Synopsis :   Parse a copy container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_CopyContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE                    *f_pbLicense,
    IN          DRM_DWORD                    f_iObject,
    IN          DRM_DWORD                    f_cbObject,
    IN  OUT     DRM_VOID                    *f_pvObject )
{
    DRM_RESULT           dr         = DRM_SUCCESS;
    DRM_DWORD            iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD             wFlags     = 0;
    DRM_WORD             wType      = 0;
    DRM_DWORD            cbChild    = 0;
    DRM_XMR_UNKNOWN_OBJECT *pUnknownObject = NULL;
    DRM_XMR_COPY_RIGHTS *pcontainer = ( DRM_XMR_COPY_RIGHTS * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid                      = TRUE;
    pcontainer->CopyCount.fValid            = FALSE;
    pcontainer->CopyProtectionLevel.fValid  = FALSE;
    pcontainer->MoveEnabler.fValid          = FALSE;
    pcontainer->pUnknownObjects             = NULL;
    ZEROMEM( &pcontainer->UnknownContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags,  f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType,   f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_COPYCOUNT_OBJECT:
            case XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT:
                ChkBOOL( !pcontainer->CopyCount.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->CopyCount ) );
                pcontainer->CopyCount.dwVersion =
                    ( wType == XMR_OBJECT_TYPE_COPYCOUNT_OBJECT ) ? 1 : 2;
                break;

            case XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT:
                ChkBOOL( !pcontainer->CopyProtectionLevel.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->CopyProtectionLevel ) );
                break;

            case XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT:
                ChkBOOL( !pcontainer->MoveEnabler.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->MoveEnabler ) );
                break;

            case XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER:
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0, DRM_E_INVALIDLICENSE );
                ChkBOOL( ( wFlags & XMR_FLAGS_CONTAINER ) != 0, DRM_E_INVALIDLICENSE );

                if( !pcontainer->UnknownContainer.fValid )
                {
                    ZEROMEM( &pcontainer->UnknownContainer , SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                    ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                            f_pbLicense,
                            iCurrent,
                            cbChild,
                            &pcontainer->UnknownContainer ) );
                }
                else
                {
                    DRM_XMR_UNKNOWN_CONTAINER *pUnkContainer = NULL;

                    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                  SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),
                                                  SIZEOF( DRM_DWORD ),
                                                  NULL,
                                                  ( DRM_VOID ** ) &pUnkContainer ) );

                    ZEROMEM( pUnkContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                    ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                                f_pbLicense,
                                iCurrent,
                                cbChild,
                                pUnkContainer ) );
                    pUnkContainer->pNext = pcontainer->UnknownContainer.pNext;
                    pcontainer->UnknownContainer.pNext = pUnkContainer;
                }
                break;

            default:
                /*
                ** We don't understand it, but if it allows us to try to externally
                ** parse the object then we may be able to continue
                */
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0
                       || ( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 && ( wFlags & XMR_FLAGS_CONTAINER ) == 0 ), DRM_E_INVALIDLICENSE );

                /* Only bother keeping it if it's allowed to be parsed externally, otherwise
                ** we may as well ignore it since we can't do anything else with it */
                if( ( wFlags & XMR_FLAGS_CONTAINER ) == 0
                 && ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
                {
                    /* Alloc space and add to linked list */
                    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                  SIZEOF( DRM_XMR_UNKNOWN_OBJECT ),
                                                  SIZEOF( DRM_DWORD ),
                                                  NULL,
                                                  ( DRM_VOID ** ) &pUnknownObject ) );

                    ZEROMEM( pUnknownObject, SIZEOF( DRM_XMR_UNKNOWN_OBJECT ) );

                    ChkDR( DRM_XMR_Parse_UnknownObject( f_pStack,
                                                        f_pbLicense,
                                                        iCurrent,
                                                        cbChild,
                                                        pUnknownObject ) );

                    pUnknownObject->pNext = pcontainer->pUnknownObjects;
                    pcontainer->pUnknownObjects = pUnknownObject;
                }
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
** Function :   DRM_XMR_Parse_CopyContainer
**
** Synopsis :   Parse a playlist burn container object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_PlaylistBurnContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT                    dr         = DRM_SUCCESS;
    DRM_DWORD                     iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD                      wFlags     = 0;
    DRM_WORD                      wType      = 0;
    DRM_DWORD                     cbChild    = 0;
    DRM_XMR_UNKNOWN_OBJECT *pUnknownObject   = NULL;
    DRM_XMR_PLAYLIST_BURN_RIGHTS *pcontainer = ( DRM_XMR_PLAYLIST_BURN_RIGHTS * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject > 0, DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );

    pcontainer->fValid = TRUE;
    pcontainer->Restrictions.fValid = FALSE;
    pcontainer->pUnknownObjects     = NULL;

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags,  f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType,   f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) * 2 );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        switch ( wType )
        {
            case XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT:
                ChkBOOL( !pcontainer->Restrictions.fValid, DRM_E_INVALIDLICENSE );
                ChkDR( g_xmrParserDefines[wType].fnParse( f_pStack,
                                                    f_pbLicense,
                                                    iCurrent,
                                                    cbChild,
                                                   &pcontainer->Restrictions ) );
                break;

            default:
                /*
                ** We don't understand it, but if it allows us to try to externally
                ** parse the object then we may be able to continue
                */
                ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0
                       || ( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 && ( wFlags & XMR_FLAGS_CONTAINER ) == 0 ), DRM_E_INVALIDLICENSE );

                /* Only bother keeping it if it's allowed to be parsed externally, otherwise
                ** we may as well ignore it since we can't do anything else with it */
                if( ( wFlags & XMR_FLAGS_CONTAINER ) == 0
                 && ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
                {
                    /* Alloc space and add to linked list */
                    ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                                  SIZEOF( DRM_XMR_UNKNOWN_OBJECT ),
                                                  SIZEOF( DRM_DWORD ),
                                                  NULL,
                                                  ( DRM_VOID ** ) &pUnknownObject ) );

                    ZEROMEM( pUnknownObject, SIZEOF( DRM_XMR_UNKNOWN_OBJECT ) );

                    ChkDR( DRM_XMR_Parse_UnknownObject( f_pStack,
                                                        f_pbLicense,
                                                        iCurrent,
                                                        cbChild,
                                                        pUnknownObject ) );

                    pUnknownObject->pNext = pcontainer->pUnknownObjects;
                    pcontainer->pUnknownObjects = pUnknownObject;
                }
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
** Function :   DRM_XMR_Parse_Empty
**
** Synopsis :   Parse an empty object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_Empty(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH, DRM_E_INVALIDLICENSE );

    ( ( DRM_XMR_EMPTY * )f_pvObject )->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_WORD
**
** Synopsis :   Parse a word object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_WORD(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT    dr    = DRM_SUCCESS;
    DRM_XMR_WORD *pword = ( DRM_XMR_WORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ), DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );

    pword->fValid = TRUE;
    NETWORKBYTES_TO_WORD( pword->wValue,
                          f_pbBuffer,
                          f_iObject + XMR_BASE_OBJECT_LENGTH );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_DWORD
**
** Synopsis :   Parse a dword object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_DWORD(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT     dr     = DRM_SUCCESS;
    DRM_XMR_DWORD *pdword = ( DRM_XMR_DWORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ), DRM_E_INVALIDLICENSE );
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
** Function :   DRM_XMR_Parse_DWORD_Versioned
**
** Synopsis :   Parse a dword object with version
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_DWORD_Versioned(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT     dr     = DRM_SUCCESS;
    DRM_XMR_DWORD_VERSIONED *pdword = ( DRM_XMR_DWORD_VERSIONED * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ), DRM_E_INVALIDLICENSE );
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
** Function :   DRM_XMR_Parse_GUID
**
** Synopsis :   Parse a GUID object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_GUID(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT    dr    = DRM_SUCCESS;
    DRM_XMR_GUID *pguid = ( DRM_XMR_GUID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_GUID ), DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );

    pguid->pguidBuffer = ( DRM_BYTE * )f_pbBuffer;
    pguid->iGuid       = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pguid->fValid      = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_GUIDLIST
**
** Synopsis :   Parse a GUID list object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_GUIDLIST(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT        dr        = DRM_SUCCESS;
    DRM_DWORD         iObject   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_GUIDLIST *pguidlist = ( DRM_XMR_GUIDLIST * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ), DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_DWORD( pguidlist->cGUIDs, f_pbBuffer, iObject );

    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject);
    iObject += SIZEOF( DRM_DWORD );

    ChkOverflow( f_iObject + f_cbObject, f_iObject );
    ChkOverflow( DRM_UINT_MAX, (DRM_DWORD)( (DRM_LONG) pguidlist->cGUIDs ) * ( (DRM_LONG) SIZEOF( DRM_GUID ) ) );
    ChkOverflow( iObject + pguidlist->cGUIDs * SIZEOF( DRM_GUID ), iObject );
    ChkBOOL( f_iObject + f_cbObject == iObject + pguidlist->cGUIDs * SIZEOF( DRM_GUID ),
             DRM_E_INVALIDLICENSE );

    pguidlist->pguidBuffer = ( DRM_BYTE * )f_pbBuffer;
    pguidlist->iGuids      = iObject;
    pguidlist->fValid      = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_BYTEARRAY
**
** Synopsis :   Parse a byte array object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_BYTEARRAY(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT         dr          = DRM_SUCCESS;
    DRM_XMR_BYTEARRAY *pbytearray  = ( DRM_XMR_BYTEARRAY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH, DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );

    pbytearray->cbData        = f_cbObject - XMR_BASE_OBJECT_LENGTH;
    pbytearray->pbDataBuffer  = ( DRM_BYTE * )f_pbBuffer;
    pbytearray->iData         = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pbytearray->fValid        = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_SecurityLevel(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT                   dr                  = DRM_SUCCESS;
    DRM_DWORD                    iObject             = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_MINIMUM_ENVIRONMENT *pMinimumEnvironment =
                                    ( DRM_XMR_MINIMUM_ENVIRONMENT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ),
             DRM_E_INVALIDLICENSE );
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
** Function :   DRM_XMR_Parse_MinimumEnvironment
**
** Synopsis :   Parse a minimum environment object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_MinimumEnvironment(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT                   dr                  = DRM_SUCCESS;
    DRM_DWORD                    iObject             = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_MINIMUM_ENVIRONMENT *pMinimumEnvironment =
                                    ( DRM_XMR_MINIMUM_ENVIRONMENT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) + SIZEOF( DRM_DWORD ) * 2,
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_WORD( pMinimumEnvironment->wMinimumSecurityLevel,
                          f_pbBuffer,
                          iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_DWORD( pMinimumEnvironment->dwMinimumAppRevocationListVersion,
                           f_pbBuffer,
                           iObject );
    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject );
    iObject += SIZEOF( DRM_DWORD );

    NETWORKBYTES_TO_DWORD( pMinimumEnvironment->dwMinimumDeviceRevocationListVersion,
                          f_pbBuffer,
                          iObject );
    pMinimumEnvironment->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_Expiration
**
** Synopsis :   Parse an expiration object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_Expiration(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_DWORD           iObject     = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_EXPIRATION *pExpiration = ( DRM_XMR_EXPIRATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ) * 2,
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_DWORD( pExpiration->dwBeginDate,
                           f_pbBuffer,
                           iObject );
    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject );
    iObject += SIZEOF( DRM_DWORD );

    NETWORKBYTES_TO_DWORD( pExpiration->dwEndDate,
                          f_pbBuffer,
                          iObject );
    pExpiration->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_OPLs
**
** Synopsis :   Parse an OPL object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_OPLs(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  iObject = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS *pOPLs =
            ( DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + ( SIZEOF( DRM_WORD ) * 5 ),
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    pOPLs->cbRawData = f_cbObject - XMR_BASE_OBJECT_LENGTH;
    pOPLs->pbRawData = f_pbBuffer;
    pOPLs->iRawData  = f_iObject + XMR_BASE_OBJECT_LENGTH;

    NETWORKBYTES_TO_WORD( pOPLs->wCompressedDigitalVideo, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pOPLs->wUncompressedDigitalVideo, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pOPLs->wAnalogVideo, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pOPLs->wCompressedDigitalAudio, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    NETWORKBYTES_TO_WORD( pOPLs->wUncompressedDigitalAudio, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );
    pOPLs->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_VideoOutputConfig
**
** Synopsis :   Parse a video output configuration object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_VideoOutputConfig(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION  *pVideoConfig =
            ( DRM_XMR_VIDEO_OUTPUT_CONFIGURATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_GUID ),
             DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject);

    pVideoConfig->pguidBuffer         = ( DRM_BYTE * )f_pbBuffer;
    pVideoConfig->iGuid               = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pVideoConfig->cbConfigData        = ( f_cbObject - XMR_BASE_OBJECT_LENGTH ) - SIZEOF( DRM_GUID );
    pVideoConfig->iConfigData         = 0;

    if ( pVideoConfig->cbConfigData > 0 )
    {
        ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                      pVideoConfig->cbConfigData,
                                      SIZEOF( DRM_DWORD ),
                                      NULL,
                                      ( DRM_VOID ** ) &pVideoConfig->pbConfigDataBuffer ) );

        ChkOverflow( pVideoConfig->iGuid + SIZEOF( DRM_GUID ), pVideoConfig->iGuid );
        DRM_BYT_CopyBytes( pVideoConfig->pbConfigDataBuffer,
                           pVideoConfig->iConfigData,
                           f_pbBuffer,
                           pVideoConfig->iGuid + SIZEOF( DRM_GUID ),
                           pVideoConfig->cbConfigData );

    }
    else
    {
        pVideoConfig->pbConfigDataBuffer = NULL;
    }

    pVideoConfig->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_AudioOutputConfig
**
** Synopsis :   Parse a audio output configuration object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_AudioOutputConfig(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_XMR_AUDIO_OUTPUT_CONFIGURATION  *pAudioConfig =
            ( DRM_XMR_AUDIO_OUTPUT_CONFIGURATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_GUID ),
             DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );

    pAudioConfig->pguidBuffer         = ( DRM_BYTE * )f_pbBuffer;
    pAudioConfig->iGuid               = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pAudioConfig->cbConfigData        = ( f_cbObject - XMR_BASE_OBJECT_LENGTH ) - SIZEOF( DRM_GUID );
    pAudioConfig->iConfigData         = 0;

    if ( pAudioConfig->cbConfigData > 0 )
    {
        ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                      pAudioConfig->cbConfigData,
                                      SIZEOF( DRM_DWORD ),
                                      NULL,
                                      ( DRM_VOID ** ) &pAudioConfig->pbConfigDataBuffer ) );

        ChkOverflow( pAudioConfig->iGuid + SIZEOF( DRM_GUID ), pAudioConfig->iGuid );
        DRM_BYT_CopyBytes( pAudioConfig->pbConfigDataBuffer,
                           pAudioConfig->iConfigData,
                           f_pbBuffer,
                           pAudioConfig->iGuid + SIZEOF( DRM_GUID ),
                           pAudioConfig->cbConfigData );

    }
    else
    {
        pAudioConfig->pbConfigDataBuffer = NULL;
    }
    pAudioConfig->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_PlaylistBurnRights
**
** Synopsis :   Parse a playlist burn rights object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_PlaylistBurnRights(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  iObject = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_PLAYLIST_BURN_RESTRICTIONS *pPlaylistBurnRestrictions =
                            ( DRM_XMR_PLAYLIST_BURN_RESTRICTIONS * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ) * 2,
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_DWORD( pPlaylistBurnRestrictions->dwMaxPlaylistBurnCount,
                           f_pbBuffer,
                           iObject );
    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject );
    iObject += SIZEOF( DRM_DWORD );

    NETWORKBYTES_TO_DWORD( pPlaylistBurnRestrictions->dwPlaylistBurnTrackCount,
                           f_pbBuffer,
                           iObject );
    pPlaylistBurnRestrictions->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_UserID
**
** Synopsis :   Parse a user ID object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_UserID(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT      dr      = DRM_SUCCESS;
    DRM_XMR_USERID *pUserID = ( DRM_XMR_USERID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH, DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );

    pUserID->pbUserID = ( DRM_BYTE * )f_pbBuffer;
    pUserID->iUserID  = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pUserID->cbUserID = f_cbObject - XMR_BASE_OBJECT_LENGTH;
    pUserID->fValid   = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_ContentKey
**
** Synopsis :   Parse a content key object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_ContentKey(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT           dr          = DRM_SUCCESS;
    DRM_DWORD            iObject     = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_CONTENT_KEY *pContentKey = ( DRM_XMR_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH
                         + ( SIZEOF( DRM_WORD ) * 3 )
                         + SIZEOF( DRM_GUID ), DRM_E_INVALIDLICENSE );
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
            DRM_E_INVALIDLICENSE );
    pContentKey->pbEncryptedKeyBuffer = ( DRM_BYTE * )f_pbBuffer;
    pContentKey->iEncryptedKey        = iObject;
    pContentKey->fValid               = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_RsaPubkey
**
** Synopsis :   Parse an RSA public key object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_RsaPubkey(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_RSA_PUBKEY *pRsaPubkey = ( DRM_XMR_RSA_PUBKEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) + SIZEOF( DRM_DWORD ),
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_DWORD( pRsaPubkey->dwExponent, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject );
    iObject += SIZEOF( DRM_DWORD );

    NETWORKBYTES_TO_WORD( pRsaPubkey->cbModulus, f_pbBuffer, iObject );
    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkOverflow( f_iObject + f_cbObject, f_iObject );
    ChkOverflow( iObject + pRsaPubkey->cbModulus, iObject );
    ChkBOOL( f_iObject + f_cbObject == iObject + pRsaPubkey->cbModulus, DRM_E_INVALIDLICENSE );
    pRsaPubkey->pbModulusBuffer = ( DRM_BYTE * )f_pbBuffer;
    pRsaPubkey->iModulus        = iObject;
    pRsaPubkey->fValid          = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_UplinkKID
**
** Synopsis :   Parse an uplink KID object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_UplinkKID(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_UPLINK_KID *pUplinkKID = ( DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) + SIZEOF( DRM_GUID ),
             DRM_E_INVALIDLICENSE );
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
    ChkBOOL( f_cbObject >= iObject - f_iObject + pUplinkKID->cbChainedCheckSum, DRM_E_INVALIDLICENSE );

    pUplinkKID->pbChainedCheckSumBuffer = ( DRM_BYTE * ) f_pbBuffer;
    pUplinkKID->iChainedCheckSum        = iObject;

    pUplinkKID->wChecksumType = XMR_UPLINK_CHECKSUM_TYPE_XMRV1;

    pUplinkKID->fValid = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_UplinkKID2(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_DWORD           iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_UPLINK_KID *pUplinkKID = ( DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + ( 2 * SIZEOF( DRM_WORD ) ) + SIZEOF( DRM_GUID ),
             DRM_E_INVALIDLICENSE );
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
    ChkBOOL( f_cbObject >= iObject - f_iObject + pUplinkKID->cbChainedCheckSum, DRM_E_INVALIDLICENSE );

    pUplinkKID->pbChainedCheckSumBuffer = ( DRM_BYTE * ) f_pbBuffer;
    pUplinkKID->iChainedCheckSum        = iObject;

    pUplinkKID->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_Signature
**
** Synopsis :   Parse a signature object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_Signature(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT         dr         = DRM_SUCCESS;
    DRM_DWORD          iObject    = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_SIGNATURE *pSignature = ( DRM_XMR_SIGNATURE * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + ( SIZEOF( DRM_WORD ) * 2 ),
             DRM_E_INVALIDLICENSE );
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
             DRM_E_INVALIDLICENSE );

    pSignature->pbSignatureBuffer = ( DRM_BYTE * )f_pbBuffer;
    pSignature->iSignature        = iObject;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_UnknownContainer
**
** Synopsis :   Parses any unknown container objects
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          A parsing error occurred
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_UnknownContainer(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbLicense,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT            dr         = DRM_SUCCESS;
    DRM_DWORD             iCurrent   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_WORD              wFlags     = 0;
    DRM_WORD              wType      = 0;
    DRM_DWORD             cbChild    = 0;
    DRM_XMR_UNKNOWN_OBJECT          *pUnknownObject         = NULL;
    DRM_XMR_UNKNOWN_CONTAINER       *pUnknownChildContainer = NULL;
    DRM_XMR_UNKNOWN_CONTAINER       *pcontainer             = ( DRM_XMR_UNKNOWN_CONTAINER * )f_pvObject;

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_pvObject  != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH,
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iCurrent, f_iObject );
    ChkOverflow( f_iObject + f_cbObject, f_iObject );
    ChkOverflow( f_iObject + SIZEOF( DRM_WORD ), f_iObject );

    pcontainer->fValid            = TRUE;
    NETWORKBYTES_TO_WORD( pcontainer->wFlags, f_pbLicense, f_iObject );
    NETWORKBYTES_TO_WORD( pcontainer->wType, f_pbLicense, f_iObject + SIZEOF( DRM_WORD ) );

    while ( iCurrent < f_iObject + f_cbObject )
    {
        NETWORKBYTES_TO_WORD( wFlags, f_pbLicense, iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ), iCurrent );
        ChkOverflow( iCurrent + SIZEOF( DRM_WORD ) * 2, iCurrent );
        NETWORKBYTES_TO_WORD( wType, f_pbLicense, iCurrent + SIZEOF( DRM_WORD ) );
        NETWORKBYTES_TO_DWORD( cbChild, f_pbLicense, iCurrent + ( SIZEOF( DRM_WORD ) * 2 ) );
        ChkBOOL( cbChild >= XMR_BASE_OBJECT_LENGTH && cbChild <= ( f_iObject + ( f_cbObject - iCurrent ) ), DRM_E_INVALIDLICENSE );

        /*
        ** We don't understand it, but if it allows us to try to externally
        ** parse the object then we may be able to continue
        */
        ChkBOOL( ( wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0
               || ( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 ), DRM_E_INVALIDLICENSE );

        /* Parse the object if any */
        if( ( wFlags & XMR_FLAGS_CONTAINER ) == 0 )
        {
            /*
            ** Only bother keeping it if it's allowed to be parsed externally, otherwise
            ** we may as well ignore it since we can't do anything else with it
            */
            if( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0
                || wType == XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT
                || wType == XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT )
            {
                /* Alloc space and add to linked list */
                ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                              SIZEOF( DRM_XMR_UNKNOWN_OBJECT ),
                                              SIZEOF( DRM_DWORD ),
                                              NULL,
                                              ( DRM_VOID ** ) &pUnknownObject ) );

                ZEROMEM( pUnknownObject, SIZEOF( DRM_XMR_UNKNOWN_OBJECT ) );

                ChkDR( DRM_XMR_Parse_UnknownObject( f_pStack,
                    f_pbLicense,
                    iCurrent,
                    cbChild,
                    pUnknownObject ) );

                pUnknownObject->pNext = pcontainer->pObject;
                pcontainer->pObject = pUnknownObject;
            }
        } /* Object parsing */
        else /* if( ( wFlags & XMR_FLAGS_CONTAINER ) != 0 ) */
        {
            /*
            ** Only bother keeping it if it's allowed to be parsed externally, otherwise
            ** we may as well ignore it since we can't do anything else with it
            */
            if( ( wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
            {
                /* Alloc space and add to linked list */
                ChkDR( DRM_STK_Alloc_Aligned( f_pStack,
                                              SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),
                                              SIZEOF( DRM_DWORD ),
                                              NULL,
                                              ( DRM_VOID ** ) &pUnknownChildContainer ) );

                ZEROMEM( pUnknownChildContainer, SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ) );

                ChkDR( DRM_XMR_Parse_UnknownContainer( f_pStack,
                    f_pbLicense,
                    iCurrent,
                    cbChild,
                    pUnknownChildContainer ) );

                pUnknownChildContainer->pNext = pcontainer->pUnkChildcontainer;
                pcontainer->pUnkChildcontainer = pUnknownChildContainer;
            }
        }
        ChkOverflow( iCurrent + cbChild, iCurrent );
        iCurrent += cbChild;
    }

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   DRM_XMR_Parse_UnknownObject
**
** Synopsis :   Parses any unknown objects
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_UnknownObject(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT              dr         = DRM_SUCCESS;
    DRM_XMR_UNKNOWN_OBJECT *pObject    = ( DRM_XMR_UNKNOWN_OBJECT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ),
             DRM_E_INVALIDLICENSE );
    ChkOverflow( f_iObject + XMR_BASE_OBJECT_LENGTH, f_iObject );
    ChkOverflow( f_iObject + SIZEOF( DRM_WORD ), f_iObject );

    pObject->fValid = TRUE;
    NETWORKBYTES_TO_WORD( pObject->wFlags, f_pbBuffer, f_iObject );
    NETWORKBYTES_TO_WORD( pObject->wType, f_pbBuffer, f_iObject + SIZEOF( DRM_WORD ) );

    pObject->pbBuffer = ( DRM_BYTE * )f_pbBuffer;
    pObject->ibData = f_iObject + XMR_BASE_OBJECT_LENGTH;
    pObject->cbData = f_cbObject - XMR_BASE_OBJECT_LENGTH;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_EccDevicekey
**
** Synopsis :   Parse an ECC device key object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_EccDevicekey(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT              dr      = DRM_SUCCESS;
    DRM_DWORD               iObject = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_ECC_DEVICE_KEY *pKey    = ( DRM_XMR_ECC_DEVICE_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ), DRM_E_INVALIDLICENSE );
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
** Function :   DRM_XMR_Parse_DomainID
**
** Synopsis :   Parse a Domain ID object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_DomainID(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT         dr        = DRM_SUCCESS;
    DRM_DWORD          iObject   = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_DOMAIN_ID *pDomainID = ( DRM_XMR_DOMAIN_ID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ) * 5, DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    pDomainID->fValid      = TRUE;
    pDomainID->pbAccountID = ( DRM_BYTE * )f_pbBuffer;
    pDomainID->ibAccountID = iObject;
    pDomainID->cbAccountID = SIZEOF( DRM_DWORD ) * 4;
    iObject += pDomainID->cbAccountID;

    NETWORKBYTES_TO_DWORD( pDomainID->dwRevision, f_pbBuffer, iObject );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_OptimizedContentKey
**
** Synopsis :   Parse a symmetrically optimized content key object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_OptimizedContentKey(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT                     dr          = DRM_SUCCESS;
    DRM_DWORD                      iObject     = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_OPTIMIZED_CONTENT_KEY *pContentKey = ( DRM_XMR_OPTIMIZED_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH
                         + ( SIZEOF( DRM_WORD ) * 2 ) , DRM_E_INVALIDLICENSE );
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
            DRM_E_INVALIDLICENSE );
    pContentKey->pbEncryptedKeyBuffer = ( DRM_BYTE * )f_pbBuffer;
    pContentKey->iEncryptedKey        = iObject;
    pContentKey->fValid               = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_RemovalDate
**
** Synopsis :   Parse a removal date object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_RemovalDate(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_DWORD             iObject      = f_iObject + XMR_BASE_OBJECT_LENGTH;
    DRM_XMR_REMOVAL_DATE *pRemovalDate = ( DRM_XMR_REMOVAL_DATE * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkBOOL( f_cbObject == XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_DWORD ),
             DRM_E_INVALIDLICENSE );
    ChkOverflow( iObject, f_iObject );

    NETWORKBYTES_TO_DWORD( pRemovalDate->dwRemovalDate,
                           f_pbBuffer,
                           iObject );
    ChkOverflow( iObject + SIZEOF( DRM_DWORD ), iObject );
    iObject += SIZEOF( DRM_DWORD );

    pRemovalDate->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_AuxKey
**
** Synopsis :   Parse an Aux Key object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_AuxKey(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iObject  = f_iObject;
    DRM_XMR_AUX_KEY *pAuxKey =
            ( DRM_XMR_AUX_KEY * )f_pvObject;

    ChkArg(  f_pbBuffer != NULL );
    ChkArg(  f_pvObject  != NULL );
    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + 
                           SIZEOF( DRM_WORD ),
             DRM_E_INVALIDLICENSE );

    ChkOverflow( iObject + XMR_BASE_OBJECT_LENGTH, iObject );
    iObject += XMR_BASE_OBJECT_LENGTH;

    NETWORKBYTES_TO_WORD( pAuxKey->cEntries, f_pbBuffer, iObject );

    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + 
                           SIZEOF( DRM_WORD ) + 
                           pAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ),
             DRM_E_INVALIDLICENSE );

    pAuxKey->pEntries = ( DRM_XMR_AUX_KEY_ENTRY * )( f_pbBuffer + iObject );

    ChkOverflow( iObject + ( pAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) ), iObject );
    iObject += ( pAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) );

    pAuxKey->fValid = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Parse_UplinkX
**
** Synopsis :   Parse an UplinkX object
**
** Arguments :
**      [f_pStack]   : Stack for intermediate usage
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
**      DRM_E_INVALIDLICENSE
**          f_cbObject is outside the expected range for the object type
**
** Notes :  Prototyped in drmxmrparser.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Parse_UplinkX(
    IN          DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    IN  const   DRM_BYTE    *f_pbBuffer,
    IN          DRM_DWORD    f_iObject,
    IN          DRM_DWORD    f_cbObject,
    IN  OUT     DRM_VOID    *f_pvObject )
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
             DRM_E_INVALIDLICENSE );

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
                           pUplinkX->cbCheckSum, DRM_E_INVALIDLICENSE );

    pUplinkX->pbCheckSumBuffer = ( DRM_BYTE * ) f_pbBuffer;
    pUplinkX->iCheckSum        = iObject;

    ChkOverflow( iObject + pUplinkX->cbCheckSum, iObject );
    iObject += pUplinkX->cbCheckSum;

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + 
                           SIZEOF( DRM_GUID ) +
                           SIZEOF( DRM_WORD ) + 
                           pUplinkX->cbCheckSum +
                           SIZEOF( DRM_WORD ), DRM_E_INVALIDLICENSE );

    NETWORKBYTES_TO_WORD( pUplinkX->cEntries, f_pbBuffer, iObject );

    ChkOverflow( iObject + SIZEOF( DRM_WORD ), iObject );
    iObject += SIZEOF( DRM_WORD );

    ChkBOOL( f_cbObject >= XMR_BASE_OBJECT_LENGTH + 
                           SIZEOF( DRM_GUID ) + 
                           SIZEOF( DRM_WORD ) + 
                           pUplinkX->cbCheckSum +
                           SIZEOF( DRM_WORD ) + 
                           pUplinkX->cEntries * SIZEOF( DRM_DWORD ),
             DRM_E_INVALIDLICENSE );

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
**      DRM_E_XMR_OBJECT_NOTFOUND
**          Failed to find the child node
**      DRM_E_INVALIDLICENSE
**          A child node was found that was too large for the current container
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _XMR_FindChild(
    IN        DRM_WORD         f_wType,
    IN  const DRM_BYTE        *f_pbBuffer,
    IN        DRM_DWORD        f_iCurrentNode,
    IN        DRM_DWORD        f_cbCurrentNode,
    OUT       DRM_DWORD       *f_piChild,
    OUT       DRM_DWORD       *f_pcbChild )
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
        ChkBOOL( cbNext > 0, DRM_E_INVALIDLICENSE );
        ChkOverflow( iNext + cbNext, iNext );
        iNext += cbNext;
    }
    iNext -= cbNext;
    ChkBOOL( wNextType == f_wType, DRM_E_XMR_OBJECT_NOTFOUND );
    ChkBOOL( cbNext <= ( f_iCurrentNode + f_cbCurrentNode - iNext ), DRM_E_INVALIDLICENSE );

    *f_piChild = iNext;
    *f_pcbChild = cbNext;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_FindObject
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
**      DRM_E_INVALIDLICENSE
**          The license buffer is an unexpected length
**      DRM_E_ARITHMETICOVERFLOW
**          An arithmetic overflow occurred during processing
**      DRM_E_XMR_OBJECT_NOTFOUND
**          Failed to find the object
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_FindObject(
    __in                             DRM_WORD   f_wObjectType,
    __in_bcount( f_cbLicense ) const DRM_BYTE  *f_pbLicense,
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

    ChkArg( f_pbLicense != NULL );
    ChkArg( f_piObject  != NULL );
    ChkArg( f_pcbObject != NULL );

    NETWORKBYTES_TO_DWORD( cbCurrent,
                           f_pbLicense,
                           iCurrent + SIZEOF( DRM_WORD ) * 2 );

    ChkOverflow( cbCurrent + XMR_HEADER_LENGTH, cbCurrent );

    ChkBOOL( f_cbLicense >= (cbCurrent + XMR_HEADER_LENGTH), DRM_E_INVALIDLICENSE );

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
                               f_pbLicense,
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
** Function :   DRM_XMR_UnpackLicense
**
** Synopsis :   Parse the license
**
** Arguments :
**      [f_pbLicense]   : License buffer
**      [f_cbLicense]   : License buffer size
**      [f_pStack]      : Stack pointer
**      [f_pXmrLicense] : Returns the parsed license
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or invalid
**      DRM_E_INVALIDLICENSE
**          The license buffer does not specify a valid license
**
** Notes     :  Callers must keep the stack around as long as they use the
**              contents of the structure returned. The structure might have
**              some intermediate nodes allocated out of this stack
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_UnpackLicense(
    __in_bcount( f_cbLicense ) const DRM_BYTE                    *f_pbLicense,
    __in                             DRM_DWORD                    f_cbLicense,
    __in                             DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __out                            DRM_XMR_LICENSE             *f_pXmrLicense )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  dwMagicConstant  = 0;
    DRM_DWORD  cbOuterContainer = 0;

    DRMCASSERT( DRM_XMR_MAX_UNPACK_ALLOC_FACTOR >= DRM_XMR_COMPUTED_MAX_UNPACK_ALLOC_FACTOR );

    ChkArg( f_pbLicense     != NULL );
    ChkArg( f_cbLicense     != 0 );
    ChkArg( f_pXmrLicense   != NULL );

    ChkBOOL( f_cbLicense > XMR_HEADER_LENGTH, DRM_E_INVALIDLICENSE );

    NETWORKBYTES_TO_DWORD( dwMagicConstant, f_pbLicense, 0 );

    ChkBOOL( dwMagicConstant == XMR_MAGIC_CONSTANT, DRM_E_INVALIDLICENSE );

    NETWORKBYTES_TO_DWORD( f_pXmrLicense->dwVersion, f_pbLicense, SIZEOF( DRM_DWORD ) );
    f_pXmrLicense->pbRightsIdBuffer = ( DRM_BYTE * ) f_pbLicense;
    f_pXmrLicense->iRightsId        = SIZEOF( DRM_DWORD ) * 2;
    ChkDR( DRM_XMR_GetContainer( f_pbLicense,
                                 f_cbLicense,
                                 XMR_OBJECT_TYPE_OUTER_CONTAINER,
                                 f_pStack,
                 ( DRM_VOID * ) &f_pXmrLicense->containerOuter ) );

    /*
    ** Verify that the length of the license buffer is consistent with the outer container size
    */
    NETWORKBYTES_TO_DWORD( cbOuterContainer, f_pbLicense, XMR_HEADER_LENGTH + 2 * SIZEOF( DRM_WORD ) );
    ChkBOOL( cbOuterContainer + XMR_HEADER_LENGTH == f_cbLicense, DRM_E_INVALIDLICENSE );

    f_pXmrLicense->pbSignedDataBuffer = ( DRM_BYTE * )f_pbLicense;
    f_pXmrLicense->iSignedData        = 0;
    f_pXmrLicense->cbSignedData       = f_cbLicense
                                      - XMR_BASE_OBJECT_LENGTH
                                      - SIZEOF( DRM_WORD ) * 2
                                      - f_pXmrLicense->containerOuter.signature.cbSignature;

    ChkDR( DRM_XMR_ValidateLicenseObjects( f_pXmrLicense ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_GetContainer
**
** Synopsis :   Parse the license to obtain a particular container
**
** Arguments :
**      [f_pbLicense]      : License buffer
**      [f_cbLicense]      : License buffer size
**      [f_wContainerType] : Container type
**      [f_pStack]         : Stack pointer
**      [f_pvContainer]    : Container object passed as void
**
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or invalid
**      DRM_E_INVALIDLICENSE
**          The license buffer does not specify a valid license
**      DRM_E_ARITHMETICOVERFLOW
**          An arithmetic overflow occurred during processing
**      DRM_E_XMR_OBJECT_NOTFOUND
**          Failed to find the container object
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_GetContainer(
    IN  const   DRM_BYTE                     *f_pbLicense,
    IN          DRM_DWORD                     f_cbLicense,
    IN          DRM_WORD                      f_wContainerType,
    IN          DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack,
    IN  OUT     DRM_VOID                     *f_pvContainer )
{
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_DWORD  iContainer      = 0;
    DRM_DWORD  cbContainer     = 0;
    DRM_DWORD  dwMagicConstant = 0;

    ChkArg( f_pbLicense   != NULL );
    ChkArg( f_cbLicense   != 0 );
    ChkArg( f_pvContainer != NULL );
    ChkArg( f_wContainerType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );
    ChkArg( g_xmrCommonDefines[f_wContainerType].fIsContainer );

    ChkBOOL( f_cbLicense > XMR_HEADER_LENGTH, DRM_E_INVALIDLICENSE );

    NETWORKBYTES_TO_DWORD( dwMagicConstant, f_pbLicense, 0 );
    ChkBOOL( dwMagicConstant == XMR_MAGIC_CONSTANT, DRM_E_INVALIDLICENSE );

    ChkDR( DRM_XMR_FindObject( f_wContainerType,
                               f_pbLicense,
                               f_cbLicense,
                               &iContainer,
                               &cbContainer ) );
    ChkDR( g_xmrParserDefines[f_wContainerType].fnParse( f_pstack,
                                                   f_pbLicense,
                                                   iContainer,
                                                   cbContainer,
                                                   f_pvContainer ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_PopulateInclusionList
**
** Synopsis :   Populate an XMR inclusion list
**
** Arguments :
**      [f_pInclusionList]  : Specifies the inclusion list to copy from
**      [f_rgInclusionList] : Specifies the set of GUIDs to populate
**      [f_rgfGuidValid]    : Specifies the validity flags to populate
**      [f_dwChainDepth]    : Specifies the chain depth
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or invalid
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMR_PopulateInclusionList(
    __in                                    DRM_XMR_INCLUSION_LIST *f_pInclusionList,
    __inout_ecount(DRM_MAX_INCLUSION_GUIDS) DRM_GUID                f_rgInclusionList[DRM_MAX_INCLUSION_GUIDS],
    __inout_ecount(DRM_MAX_INCLUSION_GUIDS) DRM_BOOL                f_rgfGuidValid[DRM_MAX_INCLUSION_GUIDS][DRM_MAX_LICENSE_CHAIN_DEPTH],
    __in                                    DRM_DWORD               f_dwChainDepth )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  iGuid = 0;
    DRM_DWORD  i     = 0;

    ChkArg( f_pInclusionList != NULL );
    ChkArg( f_dwChainDepth   < DRM_MAX_LICENSE_CHAIN_DEPTH );    
    ChkArg( f_pInclusionList->fValid );

    /*
    ** Set all the valid flags at this chain depth to FALSE for all GUIDs
    */
    for ( ; iGuid < DRM_MAX_INCLUSION_GUIDS; iGuid++ )
    {
        f_rgfGuidValid[iGuid][f_dwChainDepth] = FALSE;
    }

    for ( iGuid = 0; iGuid < f_pInclusionList->cGUIDs; iGuid++ )
    {
        if( f_dwChainDepth == 0 )
        {
            if( iGuid >= DRM_MAX_INCLUSION_GUIDS )
            {
                /*
                **  If this is a leaf license we have to stop at the Maximum number of guids
                */
                break;
            }

            /*
            **  For the leaf license we need to populate the output array of GUIDs
            **  and valid list as appropriate
            */
            ChkOverflow( f_pInclusionList->iGuids + ( SIZEOF( DRM_GUID ) * iGuid ), f_pInclusionList->iGuids );
            DRM_BYT_CopyBytes( &f_rgInclusionList[iGuid],
                                0,
                                f_pInclusionList->pguidBuffer,
                                f_pInclusionList->iGuids + ( SIZEOF( DRM_GUID ) * iGuid ),
                                SIZEOF( DRM_GUID ) );
            f_rgfGuidValid[iGuid][0] = TRUE;
        }
        else
        {
            /*
            **  For non-leaf licenses we should check to see if this guid is in the list,
            **  and if so is it valid at the previous chain level.
            **  If it is then it is still valid at the current level.
            */
            for( i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++ )
            {
                ChkOverflow( f_pInclusionList->iGuids + ( SIZEOF( DRM_GUID ) * iGuid ), f_pInclusionList->iGuids );
                if( DRM_BYT_CompareBytes( &f_rgInclusionList[i],
                                           0,
                                           f_pInclusionList->pguidBuffer,
                                           f_pInclusionList->iGuids + ( SIZEOF( DRM_GUID ) * iGuid ),
                                           SIZEOF( DRM_GUID ) ) == 0
                 && f_rgfGuidValid[i][f_dwChainDepth - 1] )
                {
                    f_rgfGuidValid[i][f_dwChainDepth] = TRUE;
                }
            }
        }
    }

ErrorExit:
    return dr;
}

/*
** Some objects are simply required.
** For V1 XMR, certain licenses from legacy tuners are incomplete,
** so we skip this validation for V1 XMR.
*/
#define VALIDATE_REQUIRED_OBJECT( pXmrLicense, IS_VALID_MACRO )             \
    ChkBOOL( ( f_pXmrLicense->dwVersion == XMR_VERSION_1 ) || IS_VALID_MACRO( pXmrLicense ), DRM_E_INVALIDLICENSE )

/* Use the look-up table to see if the object is supported for the given version */
#define VALIDATE_OPTIONAL_OBJECT( pXmrLicense, IS_VALID_MACRO, ObjType )                                            \
    ChkBOOL(                                                                                                        \
        !IS_VALID_MACRO( pXmrLicense )                                                                              \
     || g_xmrParserDefines[ObjType].rgfIsVersionSupported[(pXmrLicense)->dwVersion - 1], DRM_E_INVALIDLICENSE )

/* All Unknown Objects under the specified path must be V2 and up */
#define VALIDATE_UNKNOWN_OBJECTS_ARE_V2( pXmrLicense, IS_VALID_MACRO, ObjPath )     \
    ChkBOOL(                                                                        \
        !IS_VALID_MACRO( pXmrLicense )                                              \
     || ( (pXmrLicense)->containerOuter.ObjPath.pUnknownObjects == NULL )           \
     || ( (pXmrLicense)->dwVersion >= XMR_VERSION_2 ), DRM_E_INVALIDLICENSE )

/* All enabler containers must include one child which is the enabler type object, but no more than one of that type */
#define VALIDATE_ENABLERS( pXmrLicense, IS_VALID_MACRO, ObjPath, ContainerType, ObjectType, fAllowEnablers )                \
    do {                                                                                                                    \
        if( IS_VALID_MACRO( pXmrLicense ) )                                                                                 \
        {                                                                                                                   \
            DRM_XMR_UNKNOWN_CONTAINER *pUnkContainer =                                                                      \
                &pXmrLicense->containerOuter.ObjPath.UnknownContainer;                                                    \
            while( pUnkContainer != NULL )                                                                                  \
            {                                                                                                               \
                if( pUnkContainer->fValid )                                                                                 \
                {                                                                                                           \
                    ChkBOOL( fAllowEnablers, DRM_E_INVALIDLICENSE );                                                        \
                    if( pUnkContainer->wType == (ContainerType) )                                                           \
                    {                                                                                                       \
                        DRM_BOOL fHasEnabler = FALSE;                                                                       \
                        DRM_XMR_UNKNOWN_OBJECT *pUnkObject  = pUnkContainer->pObject;                                       \
                        while( pUnkObject != NULL )                                                                         \
                        {                                                                                                   \
                            if( pUnkObject->fValid && ( pUnkObject->wType == (ObjectType) ) )                               \
                            {                                                                                               \
                                ChkBOOL( !fHasEnabler, DRM_E_INVALIDLICENSE );                                              \
                                fHasEnabler = TRUE;                                                                         \
                            }                                                                                               \
                            pUnkObject = pUnkObject->pNext;                                                                 \
                        }                                                                                                   \
                        ChkBOOL( fHasEnabler, DRM_E_INVALIDLICENSE );                                                       \
                    }                                                                                                       \
                    else                                                                                                    \
                    {                                                                                                       \
                        ChkBOOL( pXmrLicense->dwVersion >= XMR_VERSION_2, DRM_E_INVALIDLICENSE );                           \
                    }                                                                                                       \
                }                                                                                                           \
                pUnkContainer = pUnkContainer->pNext;                                                                       \
            }                                                                                                               \
        }                                                                                                                   \
    } while( FALSE )


#define VALIDATE_RIGHTS_MASK( pXmrLicense, wValidMask )                                                                             \
    ChkBOOL(                                                                                                                        \
        ( (pXmrLicense)->containerOuter.containerGlobalPolicies.Rights.wValue == 0 )                                                \
     || ( (pXmrLicense)->containerOuter.containerGlobalPolicies.Rights.wValue                                                       \
     == ( (pXmrLicense)->containerOuter.containerGlobalPolicies.Rights.wValue & (DRM_WORD)(wValidMask) ) ), DRM_E_INVALIDLICENSE )

static DRM_NO_INLINE DRM_RESULT DRM_CALL _XMR_ValidateLicenseObjectsVersioned(
    IN          DRM_XMR_LICENSE             *f_pXmrLicense )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    if( XMR_IS_MINIMUM_ENVIRONMENT_VALID( f_pXmrLicense ) )
    {
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwVersion == 1, DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    if( XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID( f_pXmrLicense ) )
    {
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwVersion == 1
                  || f_pXmrLicense->containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    if( XMR_IS_REVOCATION_INFORMATION_VERSION_VALID( f_pXmrLicense ) )
    {
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwVersion == 1, DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    if( XMR_IS_UPLINK_KID_VALID( f_pXmrLicense ) )
    {
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.UplinkKid.dwVersion == 1, DRM_E_INVALIDLICENSE );
            ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.UplinkKid.wChecksumType == XMR_UPLINK_CHECKSUM_TYPE_XMRV1, DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.UplinkKid.dwVersion == 2, DRM_E_INVALIDLICENSE );
            ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.UplinkKid.wChecksumType == XMR_UPLINK_CHECKSUM_TYPE_AESOMAC1, DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    if( XMR_IS_COPY_VALID( f_pXmrLicense ) )
    {
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            /* V1 XMR supports both old and new Copy Policy Container Objects */
            ChkBOOL( f_pXmrLicense->containerOuter.containerCopyPolicies.dwVersion == 1
                  || f_pXmrLicense->containerOuter.containerCopyPolicies.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL( f_pXmrLicense->containerOuter.containerCopyPolicies.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    if( XMR_IS_COPYCOUNT_VALID( f_pXmrLicense ) )
    {
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            /* V1 XMR supports both old and new Copy Count Restriction Objects */
            ChkBOOL( f_pXmrLicense->containerOuter.containerCopyPolicies.CopyCount.dwVersion == 1
                  || f_pXmrLicense->containerOuter.containerCopyPolicies.CopyCount.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL( f_pXmrLicense->containerOuter.containerCopyPolicies.CopyCount.dwVersion == 2, DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ValidateLicenseObjects(
    IN          DRM_XMR_LICENSE             *f_pXmrLicense )
{
    DRM_RESULT                   dr                             = DRM_SUCCESS;
    DRM_XMR_UNKNOWN_CONTAINER   *pUnknownContainer              = NULL;
    DRM_XMR_UNKNOWN_OBJECT      *pUnknownObject                 = NULL;
    DRM_GUID                     dguidFromObject                = EMPTY_DRM_GUID;
    DRM_BOOL                     fHasCopyEnabler                = FALSE;
    DRM_BOOL                     fHasDomainInvalidCopyEnabler   = FALSE;

    ChkArg( f_pXmrLicense != NULL );

    /* XMR_VERSION_2 is not supported */
    ChkBOOL(
        ( f_pXmrLicense->dwVersion == XMR_VERSION_1 )
     || ( f_pXmrLicense->dwVersion == XMR_VERSION_3 ), DRM_E_INVALIDLICENSE );

    /* RID (LID) is required - this also validates that the outer container is valid */
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_RID_VALID );

    /*
    ** Validate "versioned" objects - objects that use the same structure
    ** but have different semantics based on the XMR version
    */
    ChkDR( _XMR_ValidateLicenseObjectsVersioned( f_pXmrLicense ) );

    /* Validate the Global Policy Container Object */
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_GLOBAL_POLICIES_VALID );
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_MINIMUM_ENVIRONMENT_VALID );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_SERIAL_NUMBER_RESTRICTION_VALID,        XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_RIGHTS_VALID,                           XMR_OBJECT_TYPE_SETTINGS_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_PRIORITY_VALID,                         XMR_OBJECT_TYPE_PRIORITY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_SOURCEID_VALID,                         XMR_OBJECT_TYPE_SOURCEID_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_RESTRICTED_SOURCEID_VALID,              XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EXPIRATION_VALID,                       XMR_OBJECT_TYPE_EXPIRATION_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_ISSUEDATE_VALID,                        XMR_OBJECT_TYPE_ISSUEDATE_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_REMOVAL_DATE_VALID,                     XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_GRACE_PERIOD_VALID,                     XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_METERING_VALID,                         XMR_OBJECT_TYPE_METERING_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EXPIRATION_VALID,                       XMR_OBJECT_TYPE_EXPIRATION_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EXPIRATION_AFTER_STORE_VALID,           XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_INCLUSIONLIST_VALID,                    XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT );
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_REVOCATION_INFORMATION_VERSION_VALID );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_DOMAIN_ID_VALID,                        XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EMBEDDING_BEHAVIOR_VALID,               XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_POLICYMETADATA_VALID,                   XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT );
    VALIDATE_UNKNOWN_OBJECTS_ARE_V2( f_pXmrLicense, XMR_IS_GLOBAL_POLICIES_VALID, containerGlobalPolicies );

    /* This object is versioned and must be validated by _XMR_ValidateLicenseObjectsVersioned */
    /* VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID,       XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT ); */

    /* Validate the Play Policy Container Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_PLAY_VALID,                             XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_PLAYCOUNT_VALID,                        XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_OPL_VALID,                              XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EXPLICITANALOGVIDEOPROTECTION_VALID,    XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_EXPLICITDIGITALAUDIOPROTECTION_VALID,   XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER );
    VALIDATE_UNKNOWN_OBJECTS_ARE_V2( f_pXmrLicense, XMR_IS_PLAY_VALID, containerPlaybackPolicies );
    VALIDATE_ENABLERS( f_pXmrLicense, XMR_IS_PLAY_VALID, containerPlaybackPolicies, XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER, XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT, f_pXmrLicense->dwVersion >= XMR_VERSION_2 );

    /* Validate the Copy Policy Container Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_COPYPROTECTIONLEVEL_VALID,              XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_MOVEENABLER_VALID,                      XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT );
    VALIDATE_UNKNOWN_OBJECTS_ARE_V2( f_pXmrLicense, XMR_IS_COPY_VALID, containerCopyPolicies );
    VALIDATE_ENABLERS( f_pXmrLicense, XMR_IS_COPY_VALID, containerCopyPolicies, XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER, XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT, TRUE );

    /* This object is versioned and must be validated by _XMR_ValidateLicenseObjectsVersioned */
    /* VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_COPY_VALID,                             XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER ); */

    /* This object is versioned and must be validated by _XMR_ValidateLicenseObjectsVersioned */
    /* VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_COPYCOUNT_VALID,                        XMR_OBJECT_TYPE_COPYCOUNT_OBJECT ); */

    /* Validate the PlaylistBurn Policy Container Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_PLAYLISTBURN_VALID,                     XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_PLAYLISTBURN_RESTRICTIONS_VALID,        XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT );
    VALIDATE_UNKNOWN_OBJECTS_ARE_V2( f_pXmrLicense, XMR_IS_PLAYLISTBURN_VALID, containerPlaylistBurnPolicies );

    /* Validate the Generation Number Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_GENERATION_NUMBER_VALID,                XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT );

    /* Validate the Copy To PC Container Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_COPY_TO_PC_VALID,                       XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER );

    /* Validate the outermost Unknown Container Object */
    ChkBOOL(
        !XMR_IS_UNKNOWN_CONTAINER_VALID(                f_pXmrLicense )
     || ( f_pXmrLicense->dwVersion >= XMR_VERSION_2 ), DRM_E_INVALIDLICENSE ); /* Unknown objects are V2 and up */

    /* Validate the Revocation Container Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_REVOCATION_VALID,                       XMR_OBJECT_TYPE_REVOCATION_CONTAINER );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_RSA_LICENSE_GRANTER_KEY_VALID,          XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_USERID_VALID,                           XMR_OBJECT_TYPE_USERID_OBJECT );

    /* Validate the Key Container Object */
    VALIDATE_REQUIRED_OBJECT( f_pXmrLicense, XMR_IS_KEY_MATERIAL_VALID ); /* All licenses require key material */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_CONTENT_KEY_VALID,                      XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT  ); /* Almost all licenses require a content key. License that are used during regeneration do not need a content key */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_OPTIMIZED_CONTENT_KEY_VALID,            XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_DEVICE_KEY_VALID,                       XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_ECC_DEVICE_KEY_VALID,                   XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_UPLINKX_VALID,                          XMR_OBJECT_TYPE_UPLINKX_OBJECT );
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_AUX_KEY_VALID,                          XMR_OBJECT_TYPE_AUX_KEY_OBJECT );

    /* This object is versioned and must be validated by _XMR_ValidateLicenseObjectsVersioned */
    /* VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_UPLINK_KID_VALID,                       XMR_OBJECT_TYPE_UPLINK_KID_OBJECT ); */

    /* Validate the Signature Object */
    VALIDATE_OPTIONAL_OBJECT( f_pXmrLicense, XMR_IS_SIGNATURE_VALID,                        XMR_OBJECT_TYPE_SIGNATURE_OBJECT );

    /* Validate the license doesn't have both uplink KID and uplinkX objects */
    ChkBOOL( !XMR_IS_UPLINK_KID_VALID( f_pXmrLicense ) || !XMR_IS_UPLINKX_VALID( f_pXmrLicense ), DRM_E_INVALIDLICENSE );

    if( XMR_IS_CONTENT_KEY_VALID( f_pXmrLicense ) )
    {
        /* Validate that the license has a valid encryption type */
        switch( f_pXmrLicense->dwVersion )
        {
        case XMR_VERSION_1:
            ChkBOOL(
                ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER  )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB ), DRM_E_INVALIDLICENSE );
            break;
        case XMR_VERSION_3:
            ChkBOOL(
                ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB )
             || ( f_pXmrLicense->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL    ), DRM_E_INVALIDLICENSE );
            break;
        default:
            AssertChkArg( FALSE ); /* Should never be reached - checked previously */
            break;
        }
    }

    if( XMR_IS_COPY_VALID( f_pXmrLicense )
     && ( ( f_pXmrLicense->dwVersion >= XMR_VERSION_3 )
       || ( f_pXmrLicense->containerOuter.containerCopyPolicies.dwVersion == 2 ) ) )
    {
        /*
        ** If we have a copy container 2 object,
        ** we MUST have at least one copy enabler under it.
        ** This block verifies that fact.
        */
        pUnknownContainer = &f_pXmrLicense->containerOuter.containerCopyPolicies.UnknownContainer;
        while( ( pUnknownContainer != NULL ) && pUnknownContainer->fValid )
        {
            if( pUnknownContainer->wType == XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER )
            {
                pUnknownObject = pUnknownContainer->pObject;
                while( ( pUnknownObject != NULL ) && pUnknownObject->fValid )
                {
                    if( pUnknownObject->wType == XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT )
                    {
                        /*
                        ** Check that the object contains a GUID
                        */
                        ChkBOOL( SIZEOF(DRM_GUID) == (pUnknownObject->cbData), DRM_E_INVALIDLICENSE );

                        fHasCopyEnabler = TRUE;

                        DRM_BYT_CopyBytes(
                            &dguidFromObject,
                            0,
                            pUnknownObject->pbBuffer,
                            pUnknownObject->ibData,
                            SIZEOF(DRM_GUID) );
                        if( IDENTICAL_GUIDS( &dguidFromObject, &DRM_ACTION_COPY_TO_PC )
                         || IDENTICAL_GUIDS( &dguidFromObject, &DRM_ACTION_COPY_TO_DEVICE ) )
                        {
                            /*
                            ** Domain licenses can only have a copy container
                            ** if the only copy enabler(s) present are NEITHER
                            ** Copy To PC nor Copy To Device
                            */
                            fHasDomainInvalidCopyEnabler = TRUE;
                        }
                    }
                    pUnknownObject = pUnknownObject->pNext;
                }
            }
            pUnknownContainer = pUnknownContainer->pNext;
        }

        ChkBOOL( fHasCopyEnabler, DRM_E_INVALIDLICENSE );
    }

    if( XMR_IS_DOMAIN_ID_VALID( f_pXmrLicense ) )
    {
        /* Domain ID must be 16 bytes */
        ChkBOOL(
            f_pXmrLicense->containerOuter.containerGlobalPolicies.DomainID.cbAccountID == SIZEOF( DRM_GUID ),
            DRM_E_INVALIDLICENSE );

        /*
        ** Make sure the license doesn't have one of the copy
        ** enablers which is invalid for domain-bound licenses
        */
        ChkBOOL( !fHasDomainInvalidCopyEnabler, DRM_E_INVALIDLICENSE );

        /*
        ** Make sure the license doesn't have an uplink KID or uplinkX, because leaf
        ** licenses cannot be domain-bound
        */
        ChkBOOL( !XMR_IS_UPLINK_KID_VALID( f_pXmrLicense ), DRM_E_INVALIDLICENSE );
        ChkBOOL( !XMR_IS_UPLINKX_VALID( f_pXmrLicense ), DRM_E_INVALIDLICENSE );
    }

    /* V2+ licenses don't support explicit minimum revocation lists - they use RIV instead */
    if( XMR_IS_MINIMUM_ENVIRONMENT_VALID( f_pXmrLicense )
     && f_pXmrLicense->dwVersion >= XMR_VERSION_2 )
    {
        ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumAppRevocationListVersion     == 0, DRM_E_INVALIDLICENSE );
        ChkBOOL( f_pXmrLicense->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumDeviceRevocationListVersion  == 0, DRM_E_INVALIDLICENSE );
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
                        DRM_E_INVALIDLICENSE );
        ChkBOOL( !IDENTICAL_GUIDS( f_pXmrLicense->containerOuter.containerKeys.UplinkX.pbguidUplinkKID
                                        + f_pXmrLicense->containerOuter.containerKeys.UplinkX.iguidUplinkKID,
                                   &guidEmpty ),
                        DRM_E_INVALIDLICENSE );
    }
    
ErrorExit:
    return dr;
}

#define DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE   ( XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) * 2 + DRM_AES_BLOCKLEN * 2 )

/******************************************************************************
**
** Function :   DRM_XMR_UpdateLicenseSizeForSymmKey
**
** Synopsis :   Parse the license and update the length of license to take into account
**           the length of the symmetrically bound content key.
**
** Arguments :
**      [f_pbLicense]        : License buffer
**      [f_cbLicense]        : License buffer size
**      [f_pcbLicenseUsed]      :  Pointer to the length of the license buffer used.
**      [f_pdwInsertionPoint] :Contains the index into the license
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or invalid
**      DRM_E_INVALIDLICENSE
**          The license buffer does not specify a valid license
**      DRM_E_BUFFERTOOSMALL
**          The content key was too large for the buffer space
**
** Notes     :  Callers must keep the stack around as long as they use the
**              contents of the structure returned. The structure might have
**              some intermediate nodes allocated out of this stack
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_UpdateLicenseSizeForSymmKey(
    __inout_bcount( f_cbLicense )     DRM_BYTE                      *f_pbLicense,
    __in                              DRM_DWORD                      f_cbLicense,
    __inout_ecount( 1 )               DRM_DWORD                     *f_pcbLicenseUsed,
    __in                              DRM_DWORD                     *f_pdwInsertionPoint )
{
    DRM_RESULT  dr                 = DRM_SUCCESS;
    DRM_DWORD   cbObject           = 0;

    ChkArg( NULL != f_pbLicense );
    ChkArg( NULL != f_pcbLicenseUsed );
    ChkArg( NULL != f_pdwInsertionPoint );

    ChkBOOL( *f_pcbLicenseUsed + DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE <= f_cbLicense,
             DRM_E_BUFFERTOOSMALL );     
    ChkOverflow( *f_pcbLicenseUsed, *f_pcbLicenseUsed - *f_pdwInsertionPoint );
    DRM_BYT_MoveBytes( f_pbLicense, 
                       *f_pdwInsertionPoint + DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE,                           
                       f_pbLicense, 
                       *f_pdwInsertionPoint, 
                       *f_pcbLicenseUsed - *f_pdwInsertionPoint ); /* Move everything down to make room */

    *f_pcbLicenseUsed += DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE;

    /*
    ** Add Optimized Content Key object constant data
    */
    WORD_TO_NETWORKBYTES( f_pbLicense, *f_pdwInsertionPoint, 0 ); 
    *f_pdwInsertionPoint += SIZEOF( DRM_WORD );
    WORD_TO_NETWORKBYTES( f_pbLicense, *f_pdwInsertionPoint, XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT );
    *f_pdwInsertionPoint += SIZEOF( DRM_WORD );
    DWORD_TO_NETWORKBYTES( f_pbLicense, *f_pdwInsertionPoint, DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE ); 
    *f_pdwInsertionPoint += SIZEOF( DRM_DWORD );
    WORD_TO_NETWORKBYTES( f_pbLicense, *f_pdwInsertionPoint, XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB );
    *f_pdwInsertionPoint += SIZEOF( DRM_WORD );
    WORD_TO_NETWORKBYTES ( f_pbLicense, *f_pdwInsertionPoint, DRM_AES_BLOCKLEN * 2 );
    *f_pdwInsertionPoint += SIZEOF( DRM_WORD );
    DRM_BYT_SetBytes( f_pbLicense, 
                      *f_pdwInsertionPoint, 
                      DRM_AES_BLOCKLEN * 2,
                      0x00 );

    /*
    ** Adjust sizes of key material container, outer container
    */
    ChkDR( DRM_XMR_FindObject( XMR_OBJECT_TYPE_OUTER_CONTAINER,
                               f_pbLicense,
                               *f_pcbLicenseUsed,
                               f_pdwInsertionPoint,
                               &cbObject ) );
    *f_pdwInsertionPoint += 2 * SIZEOF( DRM_WORD ); /* Insert size after flags, object type */
    ChkBOOL( *f_pdwInsertionPoint <= f_cbLicense, DRM_E_BUFFERTOOSMALL );  
    NETWORKBYTES_TO_DWORD( cbObject, f_pbLicense, *f_pdwInsertionPoint );
    cbObject += DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE;
    DWORD_TO_NETWORKBYTES( f_pbLicense, *f_pdwInsertionPoint, cbObject );

    ChkDR( DRM_XMR_FindObject( XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,
                               f_pbLicense,
                               *f_pcbLicenseUsed,
                               f_pdwInsertionPoint,
                               &cbObject ) );
    *f_pdwInsertionPoint += 2 * SIZEOF( DRM_WORD ); /* Insert size after flags, object type */
    ChkBOOL( *f_pdwInsertionPoint <= f_cbLicense, DRM_E_BUFFERTOOSMALL );  
    NETWORKBYTES_TO_DWORD( cbObject, f_pbLicense, *f_pdwInsertionPoint );
    cbObject += DRM_XMR_SYMMETRIC_CONTENT_KEY_AES_SIZE;
    DWORD_TO_NETWORKBYTES( f_pbLicense, *f_pdwInsertionPoint, cbObject );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMR_RequiresSST(
    __in     DRM_XMR_LICENSE *f_pXmrLicense,
    __out    DRM_BOOL        *f_pfRequiresSST )
{
    DRM_RESULT              dr      = DRM_SUCCESS;

    ChkArg( NULL != f_pXmrLicense );
    ChkArg( NULL != f_pfRequiresSST );

    *f_pfRequiresSST = FALSE;

    if( XMR_HAS_TIME_BASED_RESTRICTIONS( f_pXmrLicense )
     || XMR_IS_PLAYCOUNT_VALID( f_pXmrLicense )
     || XMR_IS_COPYCOUNT_VALID( f_pXmrLicense )
     || XMR_IS_METERING_VALID( f_pXmrLicense )
     || ( XMR_IS_PLAYLISTBURN_RESTRICTIONS_VALID( f_pXmrLicense )
       && f_pXmrLicense->containerOuter.containerPlaylistBurnPolicies.Restrictions.dwPlaylistBurnTrackCount != XMR_UNLIMITED ) )
    {
        *f_pfRequiresSST = TRUE;
        goto ErrorExit;
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
