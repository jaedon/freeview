/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMRBUILDER_C
#include <drmcontextsizes.h>
#include <byteorder.h>
#include <drmxmr.h>
#include <drmxmrbuilder.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

const DRM_EXPORT_VAR XMR_BUILDER_INTERNAL_DEFINES g_xmrBuilderDefines[XMR_OBJECT_TYPE_MAXIMUM_DEFINED+1] =
{
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* XMR_OBJECT_TYPE_INVALID */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_OuterContainer,                                  FALSE,    FALSE, SIZEOF( DRM_XMR_OUTER_CONTAINER ),                     },    /* XMR_OBJECT_TYPE_OUTER_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_GlobalContainer,                                 FALSE,    FALSE, SIZEOF( DRM_XMR_GLOBAL_REQUIREMENTS ),                 },    /* XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_MinimumEnvironment,        DRM_XMR_Serialize_MinimumEnvironment, DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_MINIMUM_ENVIRONMENT ),                 },    /* XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_PlaybackContainer,                               FALSE,    TRUE,  SIZEOF( DRM_XMR_PLAYBACK_RIGHTS ),                     },    /* XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_OPLs,                      DRM_XMR_Serialize_OPLs,               DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS ),    },    /* XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT */
    { DRM_XMR_SetDefaults_UplinkKID,                 DRM_XMR_Serialize_UplinkKID,          DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_UPLINK_KID ),                          },    /* XMR_OBJECT_TYPE_UPLINK_KID_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_ExplicitAnalogVideoOutputProtectionContainer,    FALSE,    TRUE,  SIZEOF( DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION ),    },    /* XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER */
    { DRM_XMR_SetDefaults_VideoOutputConfig,         DRM_XMR_Serialize_VideoOutputConfig,  DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_VIDEO_OUTPUT_CONFIGURATION ),          },    /* XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_KeyMaterialContainer,                            FALSE,    FALSE, SIZEOF( DRM_XMR_KEY_MATERIAL ),                        },    /* XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER */
    { DRM_XMR_SetDefaults_ContentKey,                DRM_XMR_Serialize_ContentKey,         DRM_XMR_ParseStructure_Invalid,                                         FALSE,    FALSE, SIZEOF( DRM_XMR_CONTENT_KEY ),                         },    /* XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT */
    { DRM_XMR_SetDefaults_Invalid,                   DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    FALSE, SIZEOF( DRM_XMR_SIGNATURE ),                           },    /* XMR_OBJECT_TYPE_SIGNATURE_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_BYTEARRAY,          DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_SERIAL_NUMBER ),                       },    /* XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT */
    { DRM_XMR_SetDefaults_Rights,                    DRM_XMR_Serialize_WORD,               DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_RIGHTS ),                              },    /* XMR_OBJECT_TYPE_SETTINGS_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_CopyContainer,                                   FALSE,    TRUE,  SIZEOF( DRM_XMR_COPY_RIGHTS ),                         },    /* XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_PlaylistBurnContainer,                           FALSE,    TRUE,  SIZEOF( DRM_XMR_PLAYLIST_BURN_RIGHTS ),                },    /* XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_GUIDLIST,           DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_INCLUSION_LIST ),                      },    /* XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT */
    { DRM_XMR_SetDefaults_Priority,                  DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_PRIORITY ),                            },    /* XMR_OBJECT_TYPE_PRIORITY_OBJECT */
    { DRM_XMR_SetDefaults_Expiration,                DRM_XMR_Serialize_Expiration,         DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_EXPIRATION ),                          },    /* XMR_OBJECT_TYPE_EXPIRATION_OBJECT */
    { DRM_XMR_SetDefaults_Issuedate,                 DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_ISSUEDATE ),                           },    /* XMR_OBJECT_TYPE_ISSUEDATE_OBJECT */
    { DRM_XMR_SetDefaults_ExpirationAfterFirstUse,   DRM_XMR_Serialize_DWORD_Versioned,    DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_EXPIRATION_AFTER_FIRSTUSE ),           },    /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT */
    { DRM_XMR_SetDefaults_ExpirationAfterFirstStore, DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_EXPIRATION_AFTER_FIRSTSTORE ),         },    /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_GUID,               DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_METERING ),                            },    /* XMR_OBJECT_TYPE_METERING_OBJECT */
    { DRM_XMR_SetDefaults_Playcount,                 DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_PLAYCOUNT ),                           },    /* XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_EXCLUSION_LIST_OBJECT ( Shipped in Emerald Beta 1 )*/
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_INCLUSION_LIST_OBJECT ( Shipped in Emerald Beta 1 )*/
    { DRM_XMR_SetDefaults_GracePeriod,               DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_GRACE_PERIOD ),                        },    /* XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT */
    { DRM_XMR_SetDefaults_CopyCount,                 DRM_XMR_Serialize_DWORD_Versioned,    DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_COPY_COUNT ),                          },    /* XMR_OBJECT_TYPE_COPYCOUNT_OBJECT */
    { DRM_XMR_SetDefaults_CopyProtectionLevel,       DRM_XMR_Serialize_WORD,               DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_COPY_PROTECTION_LEVEL ),               },    /* XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_COPY_EXCLUSION_LIST_OBJECT ( Shipped in Emerald Beta 1 )*/
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_COPY_INCLUSION_LIST_OBJECT ( Shipped in Emerald Beta 1 )*/
    { DRM_XMR_SetDefaults_PlaylistBurnRights,        DRM_XMR_Serialize_PlaylistBurnRights, DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_PLAYLIST_BURN_RESTRICTIONS ),          },    /* XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_DWORD_Versioned,    DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_REVOCATION_INFORMATION_VERSION ),      },    /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT */
    { DRM_XMR_SetDefaults_RsaPubkey,                 DRM_XMR_Serialize_RsaPubkey,          DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_DEVICE_KEY ),                          },    /* XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT */
    { DRM_XMR_SetDefaults_SourceID,                  DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_SOURCEID ),                            },    /* XMR_OBJECT_TYPE_SOURCEID_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_EXPLICIT_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER ( Shipped in Emerald Beta 1 )*/
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Invalid,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT ( Shipped in Emerald Beta 1 )*/
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_RevocationContainer,                             FALSE,    TRUE,  SIZEOF( DRM_XMR_REVOCATION ),                          },    /* XMR_OBJECT_TYPE_REVOCATION_CONTAINER */
    { DRM_XMR_SetDefaults_RsaPubkey,                 DRM_XMR_Serialize_RsaPubkey,          DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_RSA_LICENSE_GRANTER_KEY ),             },    /* XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_UserID,             DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_USERID ),                              },    /* XMR_OBJECT_TYPE_USERID_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_RESTRICTED_SOURCEID ),                 },    /* XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_DomainID,           DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_DOMAIN_ID ),                           },    /* XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_EccDevicekey,       DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_ECC_DEVICE_KEY ),                      },    /* XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT */
    { DRM_XMR_SetDefaults_GenerationNumber,          DRM_XMR_Serialize_GenerationNumber,   DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_GENERATION_NUMBER ),                   },    /* XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT */
    { DRM_XMR_SetDefaults_PolicyMetadata,            DRM_XMR_Serialize_PolicyMetadata,     DRM_XMR_ParseStructure_PolicyMetadata,                                  TRUE,     TRUE,  SIZEOF( DRM_XMR_POLICY_METADATA ),                     },    /* XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT */
    { DRM_XMR_SetDefaults_OptimizedContentKey,       DRM_XMR_Serialize_OptimizedContentKey,DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_OPTIMIZED_CONTENT_KEY ),               },    /* XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_ExplicitDigitalAudioOutputProtectionContainer,   FALSE,    TRUE,  SIZEOF( DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION ),   },    /* XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),                   },    /* XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_ExpirationAfterFirstUse,   DRM_XMR_Serialize_DWORD_Versioned,    DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_EXPIRATION_AFTER_FIRSTUSE ),           },    /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT */
    { DRM_XMR_SetDefaults_AudioOutputConfig,         DRM_XMR_Serialize_AudioOutputConfig,  DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_AUDIO_OUTPUT_CONFIGURATION ),          },    /* XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_DWORD_Versioned,    DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_REVOCATION_INFORMATION_VERSION ),      },    /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT */
    { DRM_XMR_SetDefaults_EmbeddingBehavior,         DRM_XMR_Serialize_WORD,               DRM_XMR_ParseStructure_Invalid,                                         FALSE,    FALSE, SIZEOF( DRM_XMR_EMBEDDING_BEHAVIOR ),                  },    /* XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT */
    { DRM_XMR_SetDefaults_MinimumEnvironment,        DRM_XMR_Serialize_SecurityObject,     DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_MINIMUM_ENVIRONMENT ),                 },    /* XMR_OBJECT_TYPE_SECURITY_LEVEL */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_CopyToPCContainer,                               FALSE,    TRUE,  SIZEOF( DRM_XMR_COPY_TO_PC ),                          },    /* XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),                   },    /* XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_DWORD,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_MOVE_ENABLER ),                        },    /* XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_UNKNOWN_CONTAINER ),                   },    /* XMR_OBJECT_COPY_ENABLER_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_UnknownObject,      DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_UNKNOWN_OBJECT),                       },    /* XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_UnknownObject,      DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_UNKNOWN_OBJECT),                       },    /* XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT */
    { DRM_XMR_SetDefaults_UplinkKID,                 DRM_XMR_Serialize_UplinkKID2,         DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_UPLINK_KID ),                          },    /* XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_CopyContainer,                                   FALSE,    TRUE,  SIZEOF( DRM_XMR_COPY_RIGHTS ),                         },    /* XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER */
    { DRM_XMR_SetDefaults_CopyCount,                 DRM_XMR_Serialize_DWORD_Versioned,    DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_COPY_COUNT ),                          },    /* XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_READ_POLICY_CONTAINER */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_42 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_43 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_44 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_45 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_46 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_47 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_48 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_49 */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4A */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4B */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4C */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4D */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4E */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4F */
    { DRM_XMR_SetDefaults_RemovalDate,               DRM_XMR_Serialize_RemovalDate,        DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_REMOVAL_DATE ),                        },    /* XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT */
    { DRM_XMR_SetDefaults_AuxKey,                    DRM_XMR_Serialize_AuxKey,             DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_AUX_KEY ),                             },    /* XMR_OBJECT_TYPE_AUX_KEY_OBJECT */
    { DRM_XMR_SetDefaults_UplinkX,                   DRM_XMR_Serialize_UplinkX,            DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_UPLINKX ),                             },    /* XMR_OBJECT_TYPE_UPLINKX_OBJECT */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  0,                                                     },    /* Deprecated: XMR_OBJECT_TYPE_INVALID_RESERVED_53 (0x0053) was documented in PlayReady 2.0 CHM for Content Boundary Identification Object */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_UnknownObject,      DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_UNKNOWN_OBJECT ),                      },    /* XMR_OBJECT_TYPE_APPLICATION_ID_LIST */
    { DRM_XMR_SetDefaults_NoOP,                      DRM_XMR_Serialize_Empty,              DRM_XMR_ParseStructure_Invalid,                                         FALSE,    TRUE,  SIZEOF( DRM_XMR_REAL_TIME_EXPIRATION ),                },    /* XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION */
    { DRM_XMR_SetDefaults_NDTXAuthContainer,         DRM_XMR_Serialize_Container,          DRM_XMR_ParseStructure_NDTXAuthContainer,                               FALSE,    TRUE,  SIZEOF( DRM_XMR_ND_TX_AUTH_CONTAINER ),                },    /* XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER */
    { DRM_XMR_SetDefaults_NDTXAuthObject,            DRM_XMR_Serialize_NDTXAuthObject,     DRM_XMR_ParseStructure_Invalid,                                         TRUE,     TRUE,  SIZEOF( DRM_XMR_ND_TX_AUTH_OBJECT ),                   },    /* XMR_OBJECT_TYPE_ND_TX_AUTH_OBJECT */
};

/******************************************************************************
**
** Function :   DRM_XMR_CreateExtensibleRestriction
**
** Synopsis :   Returns an XMR object for extensible Restriction
**
** Arguments :  f_pcontextBuilder - builder context
**              f_wType            - object type
**              f_wFlags           - XMR Flags
**              f_pbPolicy         - Policy Data
**              f_wPolicyLen       - Policy data length
**              f_dwSecureDataSize - Size of the data, which may or may not be same as f_dwStateLen
**              f_bStateType       - State data type
**              f_pvState          - State data
**              f_dwStateLen       - State data length
**              f_ppbRestriction   - XMR Restriction returned from the function
**              f_cbRestriction    - Length of above XMR Restriction
**
******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_XMR_CreateExtensibleRestriction(
    __in_ecount( 1 )                    const DRM_XMR_BUILDER_CONTEXT *f_pcontextBuilder,
    __in_bcount( f_wPolicyLen )         const DRM_BYTE                *f_pbPolicy,
    __in                                      DRM_WORD                 f_wPolicyLen,
    __in                                      DRM_DWORD                f_dwSecureDataSize,
    __in                                      DRM_BYTE                 f_bStateType,
    __in_bcount( f_dwStateLen )               void                    *f_pvState,
    __in                                      DRM_WORD                 f_dwStateLen,
    __deref_inout_bcount( *f_cbRestriction )  DRM_BYTE               **f_ppbRestriction,
    __inout_ecount( 1 )                       DRM_DWORD               *f_cbRestriction )
{
    DRM_RESULT                  dr               = DRM_SUCCESS;
    DRM_DWORD                   dwObjectSize     = 0;
    DRM_BYTE                   *pbXMRObject      = NULL;
    DRM_DWORD                   dwXMRObjectIndex = 0;

    DRM_XMR_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder = (DRM_XMR_BUILDER_CONTEXT_INTERNAL*) f_pcontextBuilder;

    ChkArg( f_pcontextBuilder  != NULL );
    ChkArg( f_ppbRestriction != NULL );
    ChkArg( f_cbRestriction != NULL );

    dwObjectSize = sizeof(DRM_WORD) +  /* Size of Policy data length field */
                   f_wPolicyLen;

    if( f_dwSecureDataSize > 0 )
    {
        dwObjectSize += sizeof(DRM_BYTE) +  /* Size of State data type field */
                        sizeof(DRM_WORD);   /* Size of State data length field */
    }

    if( f_pvState != NULL )
    {
        dwObjectSize += f_dwStateLen;
    }

    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                          dwObjectSize,
                          (DRM_VOID **)&pbXMRObject ) );

    WORD_TO_NETWORKBYTES( pbXMRObject, dwXMRObjectIndex, f_wPolicyLen );
    dwXMRObjectIndex += SIZEOF( DRM_WORD );

    DRM_BYT_CopyBytes( pbXMRObject,
                       dwXMRObjectIndex,
                       f_pbPolicy,
                       0,
                       f_wPolicyLen );

    dwXMRObjectIndex += f_wPolicyLen;

    if( f_dwSecureDataSize > 0 )
    {
        DRM_BYT_CopyBytes( pbXMRObject,
                           dwXMRObjectIndex,
                           &f_bStateType,
                           0,
                           sizeof(DRM_BYTE) );
        dwXMRObjectIndex += sizeof(DRM_BYTE);

        WORD_TO_NETWORKBYTES( pbXMRObject, dwXMRObjectIndex, ( DRM_WORD )f_dwSecureDataSize );
        dwXMRObjectIndex += sizeof(DRM_WORD);
    }

    if( f_pvState != NULL )
    {
        switch( f_bStateType )
        {
        case XMR_EXTENSIBLE_RESTRICTON_STATE_COUNT:
            DWORD_TO_NETWORKBYTES( pbXMRObject, dwXMRObjectIndex, *((DRM_DWORD *)f_pvState) );
            dwXMRObjectIndex += SIZEOF( DRM_DWORD );
            break;
        case XMR_EXTENSIBLE_RESTRICTON_STATE_DATE:
            QWORD_TO_NETWORKBYTES( pbXMRObject, dwXMRObjectIndex, *((DRM_UINT64 *)f_pvState) );
            dwXMRObjectIndex += SIZEOF( DRM_UINT64 );
            break;
        case XMR_EXTENSIBLE_RESTRICTON_STATE_BYTEARRAY:
            DRM_BYT_CopyBytes( pbXMRObject,
                               dwXMRObjectIndex,
                               (DRM_BYTE*)f_pvState,
                               0,
                               f_dwStateLen);
            dwXMRObjectIndex += sizeof(DRM_BYTE);
            break;
        }
        ChkDR( dr );
    }
    *f_ppbRestriction = pbXMRObject;
    *f_cbRestriction = dwObjectSize;

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_BOOL _DRM_XMR_IsNativeObject( __in DRM_WORD f_wType )
{
    /* Note: Internal function - No need to check input */

    if( f_wType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED )
    {
        /*
        ** Note: Values in XMR_OBJECT_TYPE which are greater than XMR_OBJECT_TYPE_MAXIMUM_DEFINED
        **  should still be included in this list.  That way, when XMR_OBJECT_TYPE_MAXIMUM_DEFINED
        **  is increased in a future release (as more native types get introduced), existing
        **  non-native types will already be handled properly by this function.
        **  Objects which are marked as external parse but are handled natively should be caught by the
        **  first if statement. All other native non-container objects should get handled by the second statement
        **  Containers and non-natively handled objects should fall through both statements.
        */
        if (    ( g_xmrCommonDefines[f_wType].wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE )
             && ( FALSE ) ) /*  No such objects.  Replace FALSE with ( f_wType == [object_type] ) || ( f_wType == [object_type] ) || ... */
        {
            return TRUE;
        }
        else if ( !( (    g_xmrCommonDefines[f_wType].wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE )
                       || f_wType == XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER
                       || f_wType == XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER
                       || f_wType == XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT
                       || f_wType == XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT
                       || f_wType == XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT
                       || f_wType == XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER ) )
        {
            return TRUE;
        }

    }

    return FALSE;
}

DRM_BOOL DRM_NO_INLINE DRM_XMR_IsDuplicateAllowed(
    __in_ecount( 1 ) const DRM_XMR_BUILDER_LISTNODE *f_plistnode )
{
    /* Note: Internal function - No need to check input */
    DRM_BOOL fRetVal = FALSE;
    if( _DRM_XMR_IsNativeObject( f_plistnode->Node.wType ) )
    {
        fRetVal =  g_xmrBuilderDefines[f_plistnode->Node.wType].fDuplicateAllowed;
    }
    else
    {
        const DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE *pUnkNode = ( const DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE * )f_plistnode;
        fRetVal = pUnkNode->fDuplicateAllowed;
    }

    return fRetVal;
}

/******************************************************************************
**
** Function :   DRM_XMR_GetParent
**
** Synopsis :   Returns the parent object type for a given node.
**
** Arguments :  plistnode - Node for which parent needs to be found
**
******************************************************************************/

DRM_WORD DRM_XMR_GetParent( __in_ecount( 1 ) const DRM_XMR_BUILDER_LISTNODE *f_plistnode )
{
    /* Note: Internal function - No need to check input */
    DRM_WORD wRetVal = XMR_OBJECT_TYPE_INVALID;
    if( _DRM_XMR_IsNativeObject( f_plistnode->Node.wType ) )
    {
        wRetVal = g_xmrCommonDefines[f_plistnode->Node.wType].wParent;
    }
    else
    {
        const DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE *pUnkNode = ( const DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE * )f_plistnode;
        wRetVal = pUnkNode->wParent;
    }

    return wRetVal;
}

/******************************************************************************
**
** Function :   _DRM_XMR_GetBuilderNode
**
** Synopsis :   Returns the builder node for a given XMR object type.
**              Returns null if object is not found
**
** Arguments :  f_pcontextBuilder - builder context
**              f_wType     - object type
**
******************************************************************************/

DRM_NO_INLINE DRM_XMR_BUILDER_NODE* _DRM_XMR_GetBuilderNode(
    __in_ecount( 1 ) const DRM_XMR_BUILDER_CONTEXT_INTERNAL *f_pcontextBuilder,
    __in                   DRM_WORD                          f_wType )
{
    /* Note: Internal function - No need to check input */
    DRM_XMR_BUILDER_NODE  *pNode    = NULL;

    if( _DRM_XMR_IsNativeObject( f_wType ) )
    {
        DRMASSERT( f_wType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */
        pNode = f_pcontextBuilder->rgpObjectNodes[f_wType];
    }
    else
    {
        DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE *pUnkList = f_pcontextBuilder->pUnknownObjects;
        while( pUnkList != NULL )
        {
            if( pUnkList->listNode.Node.wType == f_wType )
            {
                pNode = &( pUnkList->listNode.Node );
                break;
            }
            pUnkList = pUnkList->pNext;

        }
    }
    return pNode;
}

/******************************************************************************
**
** Function :   _DRM_XMR_GetObjectLength
**
** Synopsis :   Estimates the length of an object. For a container, it only
**              returns the length of the base object
**
** Arguments :
**              [f_wType]    : Object type
**              [f_pvObject] : Actual object passed as void (optional)
**
** Returns:
**              Returns the length of the object
**
******************************************************************************/
static DRM_NO_INLINE DRM_DWORD DRM_CALL _DRM_XMR_GetObjectLength(
    __in             DRM_WORD    f_wType,
    __in_opt const   DRM_VOID   *f_pvObject )
{
    DRM_DWORD cbLength = XMR_BASE_OBJECT_LENGTH;

    DRMASSERT( f_pvObject != NULL || g_xmrCommonDefines[f_wType].fIsContainer );

    switch( f_wType )
    {
    case XMR_OBJECT_TYPE_SETTINGS_OBJECT:
    case XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT:
    case XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT:
        cbLength += SIZEOF( DRM_WORD );
        break;
    case XMR_OBJECT_TYPE_PRIORITY_OBJECT:
    case XMR_OBJECT_TYPE_ISSUEDATE_OBJECT:
    case XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT:
    case XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT:
    case XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT:
    case XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT:
    case XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT:
    case XMR_OBJECT_TYPE_COPYCOUNT_OBJECT:
    case XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT:
    case XMR_OBJECT_TYPE_SOURCEID_OBJECT:
    case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT:
    case XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT:
    case XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT:
    case XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT:
    case XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT:
        cbLength += SIZEOF( DRM_DWORD );
        break;
    case XMR_OBJECT_TYPE_EXPIRATION_OBJECT:
    case XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT:
        cbLength += SIZEOF( DRM_DWORD ) * 2;
        break;
    case XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT:
        cbLength += SIZEOF( DRM_WORD ) + SIZEOF( DRM_DWORD ) * 2;
        break;
    case XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT:
        cbLength += SIZEOF( DRM_WORD ) * 5;
        break;
    case XMR_OBJECT_TYPE_METERING_OBJECT:
        cbLength += SIZEOF( DRM_GUID );
        break;
    case XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT:
        cbLength += SIZEOF( DRM_DWORD ) * 5;
        break;
    case XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID );
        cbLength += ( ( DRM_XMR_VIDEO_OUTPUT_CONFIGURATION * )f_pvObject )->cbConfigData;
        break;
    case XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID );
        cbLength += ( ( DRM_XMR_AUDIO_OUTPUT_CONFIGURATION * )f_pvObject )->cbConfigData;
        break;
    case XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID ) + SIZEOF( DRM_WORD ) * 3;
        cbLength += ( ( DRM_XMR_CONTENT_KEY * )f_pvObject )->cbEncryptedKey;
        break;
    case XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_WORD ) * 2;
        cbLength += ( ( DRM_XMR_OPTIMIZED_CONTENT_KEY * )f_pvObject )->cbEncryptedKey;
        break;
    case XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += ( ( DRM_XMR_BYTEARRAY * )f_pvObject )->cbData;
        break;
    case XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_DWORD );
        cbLength += ( ( DRM_XMR_INCLUSION_LIST * )f_pvObject )->cGUIDs * SIZEOF( DRM_GUID );
        break;
    case XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT:
    case XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_DWORD ) + SIZEOF( DRM_WORD );
        cbLength += ( ( DRM_XMR_RSA_PUBKEY * )f_pvObject )->cbModulus;
        break;
    case XMR_OBJECT_TYPE_UPLINK_KID_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID );
        cbLength += SIZEOF( DRM_WORD );
        cbLength += ( ( DRM_XMR_UPLINK_KID * )f_pvObject )->cbChainedCheckSum;
        break;
    case XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID );
        cbLength += ( 2 * SIZEOF( DRM_WORD ) );
        cbLength += ( ( DRM_XMR_UPLINK_KID * )f_pvObject )->cbChainedCheckSum;
        break;
    case XMR_OBJECT_TYPE_AUX_KEY_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += ( SIZEOF( DRM_WORD ) );
        cbLength += ( ( DRM_XMR_AUX_KEY * )f_pvObject )->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY );
        break;
    case XMR_OBJECT_TYPE_UPLINKX_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID );
        cbLength += SIZEOF( DRM_WORD );
        cbLength += ( ( DRM_XMR_UPLINKX * )f_pvObject )->cbCheckSum;
        cbLength += ( SIZEOF( DRM_WORD ) );
        cbLength += ( ( DRM_XMR_UPLINKX * )f_pvObject )->cEntries * SIZEOF( DRM_DWORD );
        break;
    case XMR_OBJECT_TYPE_USERID_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += ( ( DRM_XMR_USERID * )f_pvObject )->cbUserID;
        break;
    case XMR_OBJECT_TYPE_SIGNATURE_OBJECT:
        /*
        ** Estimate using the largest supported size
        ** SHA256 is larger than OMAC1
        */
        cbLength += XMR_SHA256_HMAC_SIGNATURE_LENGTH + SIZEOF(DRM_WORD) * 2;
        break;
    case XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += 2 * SIZEOF( DRM_WORD );
        cbLength += ( ( DRM_XMR_ECC_DEVICE_KEY * )f_pvObject )->cbKeyData;
        break;
    case XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_GUID );
        cbLength += ( ( DRM_XMR_POLICY_METADATA_OBJECT * )f_pvObject )->cbPolicyData;
        break;
    case XMR_OBJECT_TYPE_SECURITY_LEVEL:
        cbLength += SIZEOF( DRM_WORD );
        break;
    case XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT:
        break;
    case XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT:
        cbLength += SIZEOF( DRM_GUID );
        break;
    case XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT:
        cbLength += SIZEOF( DRM_GUID );
        break;
    case XMR_OBJECT_TYPE_ND_TX_AUTH_OBJECT:
        __analysis_assume( f_pvObject != NULL );
        cbLength += SIZEOF( DRM_WORD ) + (2 * SIZEOF(DRM_DWORD));
        cbLength += ( ( DRM_XMR_ND_TX_AUTH_OBJECT * )f_pvObject )->cbDataClient;
        cbLength += ( ( DRM_XMR_ND_TX_AUTH_OBJECT * )f_pvObject )->cbDataService;
        break;
    default:
        break;
    }

    return cbLength;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_NoOP
**
** Synopsis :   Return DRM_SUCCESS with no operation
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_NoOP(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    return DRM_SUCCESS;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_Invalid
**
** Synopsis :   Return DRM_E_INVALIDARG with no operation
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_E_INVALIDARG
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_Invalid(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    return DRM_E_INVALIDARG;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_MinimumEnvironment
**
** Synopsis :   Set defaults for a minimum environment object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_MinimumEnvironment(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT                   dr  = DRM_SUCCESS;
    DRM_XMR_MINIMUM_ENVIRONMENT *pMinimumEnvrionment =
                                    ( DRM_XMR_MINIMUM_ENVIRONMENT * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pMinimumEnvrionment->wMinimumSecurityLevel                = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;
    pMinimumEnvrionment->dwMinimumAppRevocationListVersion    = 0;
    pMinimumEnvrionment->dwMinimumDeviceRevocationListVersion = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_Expiration
**
** Synopsis :   Set defaults for an expiration object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_Expiration(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT          dr  = DRM_SUCCESS;
    DRM_XMR_EXPIRATION *pExpiration = ( DRM_XMR_EXPIRATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pExpiration->dwBeginDate = 0;
    pExpiration->dwEndDate   = XMR_UNLIMITED;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_OPLs
**
** Synopsis :   Set defaults for an OPL object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_OPLs(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS *pOPLs =
            ( DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pOPLs->wAnalogVideo              = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;
    pOPLs->wCompressedDigitalAudio   = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;
    pOPLs->wCompressedDigitalVideo   = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;
    pOPLs->wUncompressedDigitalAudio = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;
    pOPLs->wUncompressedDigitalVideo = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_VideoOutputConfig
**
** Synopsis :   Set defaults for a video output configuration object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_VideoOutputConfig(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION *pVideoOutputConfig =
            ( DRM_XMR_VIDEO_OUTPUT_CONFIGURATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pVideoOutputConfig->cbConfigData       = 0;
    pVideoOutputConfig->pbConfigDataBuffer = NULL;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_AudioOutputConfig
**
** Synopsis :   Set defaults for a audio output configuration object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_AudioOutputConfig(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_XMR_AUDIO_OUTPUT_CONFIGURATION *pAudioOutputConfig =
            ( DRM_XMR_AUDIO_OUTPUT_CONFIGURATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pAudioOutputConfig->cbConfigData       = 0;
    pAudioOutputConfig->pbConfigDataBuffer = NULL;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_PolicyMetadata
**
** Synopsis :   Set defaults for the Policy Metadata
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_PolicyMetadata(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_XMR_POLICY_METADATA *pPolicyMetadata =
            ( DRM_XMR_POLICY_METADATA * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pPolicyMetadata->cPolicyMetadataObjects     = 0;
    pPolicyMetadata->plistPolicyMetadataObjects = NULL;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_PlaylistBurnRights
**
** Synopsis :   Set defaults for a playlist burn rights object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_PlaylistBurnRights(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_XMR_PLAYLIST_BURN_RESTRICTIONS *pPlaylistBurnRights =
            ( DRM_XMR_PLAYLIST_BURN_RESTRICTIONS * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pPlaylistBurnRights->dwMaxPlaylistBurnCount   = XMR_UNLIMITED;
    pPlaylistBurnRights->dwPlaylistBurnTrackCount = XMR_UNLIMITED;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_ContentKey
**
** Synopsis :   Set defaults for a content key object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_ContentKey(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT           dr          = DRM_SUCCESS;
    DRM_XMR_CONTENT_KEY *pContentKey = ( DRM_XMR_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pContentKey->cbEncryptedKey           = 0;
    pContentKey->pbEncryptedKeyBuffer     = NULL;
    pContentKey->wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024;
    pContentKey->wSymmetricCipherType     = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_OptimizedContentKey
**
** Synopsis :   Set defaults for an optimized content key object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_OptimizedContentKey(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT                     dr          = DRM_SUCCESS;
    DRM_XMR_OPTIMIZED_CONTENT_KEY *pContentKey = ( DRM_XMR_OPTIMIZED_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pContentKey->cbEncryptedKey           = 0;
    pContentKey->pbEncryptedKeyBuffer     = NULL;
    pContentKey->iEncryptedKey            = 0;
    pContentKey->wKeyEncryptionCipherType = XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_INVALID;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_RsaPubkey
**
** Synopsis :   Set defaults for an RSA public key object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_RsaPubkey(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_XMR_RSA_PUBKEY *pRsaPubkey = ( DRM_XMR_RSA_PUBKEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pRsaPubkey->cbModulus       = 0;
    pRsaPubkey->pbModulusBuffer = NULL;
    pRsaPubkey->dwExponent      = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_UplinkKID
**
** Synopsis :   Set defaults for an uplink KID object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_UplinkKID(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_XMR_UPLINK_KID *pUplinkKID = ( DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pUplinkKID->cbChainedCheckSum       = 0;
    pUplinkKID->iChainedCheckSum        = 0;
    pUplinkKID->pbChainedCheckSumBuffer = NULL;
    pUplinkKID->iguidUplinkKID          = 0;
    pUplinkKID->pbguidUplinkKID         = NULL;
    pUplinkKID->wChecksumType           = XMR_UPLINK_CHECKSUM_TYPE_AESOMAC1;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_AuxKey(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_XMR_AUX_KEY    *pAuxKey    = ( DRM_XMR_AUX_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pAuxKey->cEntries       = 0;
    pAuxKey->pEntries       = NULL;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_UplinkX(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT          dr         = DRM_SUCCESS;
    DRM_XMR_UPLINKX    *pUplinkX   = ( DRM_XMR_UPLINKX * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pUplinkX->cbCheckSum       = 0;
    pUplinkX->iCheckSum        = 0;
    pUplinkX->pbCheckSumBuffer = NULL;
    pUplinkX->iguidUplinkKID   = 0;
    pUplinkX->pbguidUplinkKID  = NULL;

    pUplinkX->cEntries      = 0;
    pUplinkX->pdwLocation   = NULL;
    pUplinkX->pbKey         = NULL;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_NDTXAuthContainer(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT                     dr    = DRM_SUCCESS;
    DRM_XMR_ND_TX_AUTH_CONTAINER  *pAuth = ( DRM_XMR_ND_TX_AUTH_CONTAINER * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pAuth->pAuthObject      = NULL;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_NDTXAuthObject(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT                 dr      = DRM_SUCCESS;
    DRM_XMR_ND_TX_AUTH_OBJECT *pAuth   = ( DRM_XMR_ND_TX_AUTH_OBJECT * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pAuth->wVersion       = 0;

    pAuth->pbBufferClient = NULL;
    pAuth->ibDataClient   = 0;
    pAuth->cbDataClient   = 0;

    pAuth->pbBufferService = NULL;
    pAuth->ibDataService   = 0;
    pAuth->cbDataService   = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_CopyProtectionLevel
**
** Synopsis :   Set defaults for a copy protection level object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_CopyProtectionLevel(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_COPY_PROTECTION_LEVEL * )f_pvObject )->wValue = DRM_DEFAULT_MINIMUM_SECURITY_LEVEL;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_CopyCount
**
** Synopsis :   Set defaults for a copy count object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_CopyCount(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_COPY_COUNT * )f_pvObject )->dwValue = XMR_UNLIMITED;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_Playcount
**
** Synopsis :   Set defaults for a play count object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_Playcount(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_PLAYCOUNT * )f_pvObject )->dwValue = XMR_UNLIMITED;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_ExpirationAfterFirstStore
**
** Synopsis :   Set defaults for an expiration after first store object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_ExpirationAfterFirstStore(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_EXPIRATION_AFTER_FIRSTSTORE * )f_pvObject )->dwValue = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_ExpirationAfterFirstUse
**
** Synopsis :   Set defaults for an expiration after first use object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_ExpirationAfterFirstUse(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_EXPIRATION_AFTER_FIRSTUSE * )f_pvObject )->dwValue = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_Issuedate
**
** Synopsis :   Set defaults for an issue date object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_Issuedate(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_ISSUEDATE * )f_pvObject )->dwValue = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_GracePeriod
**
** Synopsis :   Set defaults for a grace perios object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_GracePeriod(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_GRACE_PERIOD * )f_pvObject )->dwValue = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_Priority
**
** Synopsis :   Set defaults for a priority object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_Priority(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_PRIORITY * )f_pvObject )->dwValue = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_SourceID
**
** Synopsis :   Set defaults for a source id object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_SourceID(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_SOURCEID * )f_pvObject )->dwValue = XMR_SOURCEID_NONE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_Rights
**
** Synopsis :   Set defaults for a rights object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_Rights(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_RIGHTS * )f_pvObject )->wValue = 0;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_GenerationNumber
**
** Synopsis :   Set defaults for the generation number objects
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrdefs.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_GenerationNumber(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_GENERATION_NUMBER * )f_pvObject )->dwValue = 1;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_EmbeddingBehavior
**
** Synopsis :   Set defaults for an embedding behavior object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_EmbeddingBehavior(
    __in_ecount( 1 )  const   DRM_VOID    *f_pvObject )
    {
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pvObject != NULL );

    ( ( DRM_XMR_EMBEDDING_BEHAVIOR * )f_pvObject )->wValue = XMR_EMBEDDING_BEHAVIOR_INVALID;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_SetDefaults_RemovalDate
**
** Synopsis :   Set defaults for a removal date object
**
** Arguments :
**      [f_pvObject] : Actual object passed as void
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pvObject was NULL
**
** Notes:
**      Prototyped in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_SetDefaults_RemovalDate(
    __in_ecount( 1 ) const  DRM_VOID  *f_pvObject )
{
    DRM_RESULT            dr           = DRM_SUCCESS;
    DRM_XMR_REMOVAL_DATE *pRemovalDate = ( DRM_XMR_REMOVAL_DATE * )f_pvObject;

    ChkArg( f_pvObject != NULL );

    pRemovalDate->dwRemovalDate = 0;

ErrorExit:
    return dr;
}

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_VOIDPTR_BUFFER_25120, "f_pvObject cannot be annotated in a way that PREFAST will understand" )
static DRM_NO_INLINE DRM_VOID _DRM_XMR_Serialize_UnknownBaseObject(
    __in                                                   DRM_WORD    f_wType,
    __in                                                   DRM_WORD    f_wFlags,
    __in                                                   DRM_DWORD   f_dwLength,
    __inout_bcount( XMR_BASE_OBJECT_LENGTH + *f_piBuffer ) DRM_BYTE   *f_pbBuffer,
    __inout_ecount( 1 )                                    DRM_DWORD  *f_piBuffer )
{
    DRM_DWORD dwIndex = *f_piBuffer;
    WORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_wFlags );
    dwIndex += SIZEOF( DRM_WORD );
    WORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_wType );
    dwIndex += SIZEOF( DRM_WORD );
    DWORD_TO_NETWORKBYTES( f_pbBuffer, dwIndex, f_dwLength );
    dwIndex += SIZEOF( DRM_DWORD );
    *f_piBuffer = dwIndex;
}
PREFAST_POP

DRM_API DRM_VOID DRM_CALL DRM_XMR_Serialize_BaseObject(
    __in                                                   DRM_WORD    f_wType,
    __in                                                   DRM_DWORD   f_dwLength,
    __inout_bcount( XMR_BASE_OBJECT_LENGTH + *f_piBuffer ) DRM_BYTE   *f_pbBuffer,
    __inout_ecount( 1 )                                    DRM_DWORD  *f_piBuffer )
{
    DRMASSERT( f_wType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );
    if ( f_wType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED )
    {
        _DRM_XMR_Serialize_UnknownBaseObject( f_wType,
                                              g_xmrCommonDefines[f_wType].wFlags,
                                              f_dwLength,
                                              f_pbBuffer,
                                              f_piBuffer );
    }
}


/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_Invalid(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    return DRM_E_INVALIDARG;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_Container(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr          = DRM_SUCCESS;
    const DRM_XMR_BUILDER_LISTNODE  *pnodeChild  = ( const DRM_XMR_BUILDER_LISTNODE * )f_pvObject;
    const DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE* pUnkNodeChild = NULL;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    /* Serialize all the child nodes */
    while ( pnodeChild != NULL )
    {
        if( _DRM_XMR_IsNativeObject( pnodeChild->Node.wType ) )
        {
            ChkBOOL( f_cbBuffer >= *f_piBuffer + XMR_BASE_OBJECT_LENGTH,
                     DRM_E_BUFFERTOOSMALL );

            DRM_XMR_Serialize_BaseObject( pnodeChild->Node.wType,
                                          pnodeChild->Node.cbLength,
                                          f_pbBuffer,
                                          f_piBuffer );

            if ( pnodeChild->Node.pvObject != NULL )
            {
                /* Call Serialize only if this container has any children */
                ChkDR( g_xmrBuilderDefines[pnodeChild->Node.wType].fnSerialize( pnodeChild->Node.pvObject,
                                                                                f_pbBuffer,
                                                                                f_cbBuffer,
                                                                                f_piBuffer ) );
            }
        }
        else
        {
            ChkBOOL( f_cbBuffer >= *f_piBuffer + pnodeChild->Node.cbLength,
                     DRM_E_BUFFERTOOSMALL );


            pUnkNodeChild = (const DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE *)pnodeChild;

            /* Now serialize XMR object into the buffer */
            _DRM_XMR_Serialize_UnknownBaseObject( pnodeChild->Node.wType,
                                                  pUnkNodeChild->wFlags,
                                                  pnodeChild->Node.cbLength,
                                                  f_pbBuffer,
                                                  f_piBuffer );

            if( pUnkNodeChild->wFlags & XMR_FLAGS_CONTAINER )
            {
                /* Call Serialize only if this container has any children */
                if ( pnodeChild->Node.pvObject != NULL )
                {
                    ChkDR( DRM_XMR_Serialize_Container( pnodeChild->Node.pvObject,
                                                        f_pbBuffer,
                                                        f_cbBuffer,
                                                        f_piBuffer ) );
                }
            }
            else
            {
                ChkDR( DRM_XMR_Serialize_UnknownObject( pnodeChild,
                                                        f_pbBuffer,
                                                        f_cbBuffer,
                                                        f_piBuffer ) );
            }
        }
        pnodeChild = pnodeChild->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_Empty(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    return DRM_SUCCESS;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_WORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT          dr     = DRM_SUCCESS;
    const DRM_XMR_WORD *pword  = ( const DRM_XMR_WORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_WORD ),
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, pword->wValue );
    *f_piBuffer += SIZEOF( DRM_WORD );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_DWORD(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT              dr     = DRM_SUCCESS;
    const DRM_XMR_DWORD    *pdword = ( const DRM_XMR_DWORD * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, pdword->dwValue );
    *f_piBuffer += SIZEOF( DRM_DWORD );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_DWORD_Versioned(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                       dr     = DRM_SUCCESS;
    const DRM_XMR_DWORD_VERSIONED   *pdword = ( const DRM_XMR_DWORD_VERSIONED * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, *f_piBuffer, pdword->dwValue );
    *f_piBuffer += SIZEOF( DRM_DWORD );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_GUID(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT             dr    = DRM_SUCCESS;
    const DRM_XMR_GUID    *pguid = ( const DRM_XMR_GUID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pguid->pguidBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + SIZEOF( DRM_GUID ),
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pguid->pguidBuffer,
                       pguid->iGuid,
                       SIZEOF( DRM_GUID ) );
    *f_piBuffer += SIZEOF( DRM_GUID );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_GUIDLIST(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr        = DRM_SUCCESS;
    DRM_DWORD                  iBuffer   = 0;
    DRM_DWORD                  iGuids    = 0;
    const DRM_XMR_GUIDLIST    *pguidlist = ( const DRM_XMR_GUIDLIST * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pguidlist->pguidBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_DWORD ) + SIZEOF( DRM_GUID ) * pguidlist->cGUIDs,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pguidlist->cGUIDs );
    iBuffer += SIZEOF( DRM_DWORD );

    for ( iGuids = 0; iGuids < pguidlist->cGUIDs; iGuids++ )
    {
        DRM_BYT_CopyBytes( f_pbBuffer,
                           iBuffer,
                           pguidlist->pguidBuffer,
                           pguidlist->iGuids + iGuids*SIZEOF( DRM_GUID ),
                           SIZEOF( DRM_GUID ) );
        iBuffer += SIZEOF( DRM_GUID );
    }
    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_BYTEARRAY(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr  = DRM_SUCCESS;
    const DRM_XMR_BYTEARRAY   *pbytearray = ( const DRM_XMR_BYTEARRAY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pbytearray->cbData == 0 || pbytearray->pbDataBuffer != NULL );

    ChkBOOL( f_cbBuffer >= *f_piBuffer + pbytearray->cbData,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pbytearray->pbDataBuffer,
                       pbytearray->iData,
                       pbytearray->cbData );
    *f_piBuffer += pbytearray->cbData;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_MinimumEnvironment(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                   dr                  = DRM_SUCCESS;
    DRM_DWORD                    iBuffer             = 0;
    const DRM_XMR_MINIMUM_ENVIRONMENT *pMinimumEnvrionment =
                                    ( const DRM_XMR_MINIMUM_ENVIRONMENT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 5 * SIZEOF( DRM_WORD ),
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pMinimumEnvrionment->wMinimumSecurityLevel );
    iBuffer += SIZEOF( DRM_WORD );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pMinimumEnvrionment->dwMinimumAppRevocationListVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pMinimumEnvrionment->dwMinimumDeviceRevocationListVersion );
    iBuffer += SIZEOF( DRM_DWORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_SecurityObject(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                   dr                  = DRM_SUCCESS;
    DRM_DWORD                    iBuffer             = 0;
    const DRM_XMR_MINIMUM_ENVIRONMENT *pMinimumEnvrionment =
                                    ( const DRM_XMR_MINIMUM_ENVIRONMENT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    ChkBOOL( pMinimumEnvrionment->dwMinimumAppRevocationListVersion == 0, DRM_E_INVALID_LICENSE );
    ChkBOOL( pMinimumEnvrionment->dwMinimumDeviceRevocationListVersion == 0, DRM_E_INVALID_LICENSE );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_WORD ),
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pMinimumEnvrionment->wMinimumSecurityLevel );
    iBuffer += SIZEOF( DRM_WORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_Expiration(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                  dr          = DRM_SUCCESS;
    DRM_DWORD                   iBuffer     = 0;
    const DRM_XMR_EXPIRATION   *pExpiration = ( const DRM_XMR_EXPIRATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 2 * SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pExpiration->dwBeginDate );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pExpiration->dwEndDate );
    iBuffer += SIZEOF( DRM_DWORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_OPLs(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS         *pOPLs =
                        ( const DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 5 * SIZEOF( DRM_WORD ),
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pOPLs->wCompressedDigitalVideo );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pOPLs->wUncompressedDigitalVideo );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pOPLs->wAnalogVideo );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pOPLs->wCompressedDigitalAudio );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pOPLs->wUncompressedDigitalAudio );
    iBuffer += SIZEOF( DRM_WORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_VideoOutputConfig(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_VIDEO_OUTPUT_CONFIGURATION *pVideoOutputConfig =
                        ( const DRM_XMR_VIDEO_OUTPUT_CONFIGURATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pVideoOutputConfig->pguidBuffer != NULL );
    ChkArg( pVideoOutputConfig->cbConfigData == 0 || pVideoOutputConfig->pbConfigDataBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_GUID ) + pVideoOutputConfig->cbConfigData,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pVideoOutputConfig->pguidBuffer,
                       pVideoOutputConfig->iGuid,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pVideoOutputConfig->pbConfigDataBuffer,
                       pVideoOutputConfig->iConfigData,
                       pVideoOutputConfig->cbConfigData );
    iBuffer += pVideoOutputConfig->cbConfigData;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_AudioOutputConfig(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_AUDIO_OUTPUT_CONFIGURATION *pAudioOutputConfig =
                        ( const DRM_XMR_AUDIO_OUTPUT_CONFIGURATION * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pAudioOutputConfig->pguidBuffer != NULL );
    ChkArg( pAudioOutputConfig->cbConfigData == 0 || pAudioOutputConfig->pbConfigDataBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_GUID ) + pAudioOutputConfig->cbConfigData,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pAudioOutputConfig->pguidBuffer,
                       pAudioOutputConfig->iGuid,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pAudioOutputConfig->pbConfigDataBuffer,
                       pAudioOutputConfig->iConfigData,
                       pAudioOutputConfig->cbConfigData );
    iBuffer += pAudioOutputConfig->cbConfigData;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_PolicyMetadata(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_POLICY_METADATA_OBJECT *pPolicyMetadata =
                        ( const DRM_XMR_POLICY_METADATA_OBJECT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pPolicyMetadata->pMetadataTypeGuidBuffer != NULL );
    ChkArg( pPolicyMetadata->cbPolicyData == 0 || pPolicyMetadata->pbPolicyDataBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_GUID ) + pPolicyMetadata->cbPolicyData,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pPolicyMetadata->pMetadataTypeGuidBuffer,
                       pPolicyMetadata->iMetadataTypeGuid,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pPolicyMetadata->pbPolicyDataBuffer,
                       pPolicyMetadata->iPolicyData,
                       pPolicyMetadata->cbPolicyData );
    iBuffer += pPolicyMetadata->cbPolicyData;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_PlaylistBurnRights(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                  dr      = DRM_SUCCESS;
    DRM_DWORD                   iBuffer = 0;
    const DRM_XMR_PLAYLIST_BURN_RESTRICTIONS  *pPlaylistBurnRights =
                            ( const DRM_XMR_PLAYLIST_BURN_RESTRICTIONS * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 2 * SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pPlaylistBurnRights->dwMaxPlaylistBurnCount );
    iBuffer += SIZEOF( DRM_DWORD );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pPlaylistBurnRights->dwPlaylistBurnTrackCount );
    iBuffer += SIZEOF( DRM_DWORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_UserID(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_USERID *pUserID =
                        ( const DRM_XMR_USERID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pUserID->cbUserID == 0 || pUserID->pbUserID != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + pUserID->cbUserID,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pUserID->pbUserID,
                       pUserID->iUserID,
                       pUserID->cbUserID );
    iBuffer += pUserID->cbUserID;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_ContentKey(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr             = DRM_SUCCESS;
    DRM_DWORD                  iBuffer        = 0;
    const DRM_XMR_CONTENT_KEY *pContentKey    = ( const DRM_XMR_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pContentKey->pbguidKeyID != NULL );
    ChkArg( pContentKey->cbEncryptedKey == 0 || pContentKey->pbEncryptedKeyBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_GUID ) + 3 * SIZEOF( DRM_WORD ) + pContentKey->cbEncryptedKey,
             DRM_E_BUFFERTOOSMALL );

    OEM_SECURE_MEMCPY( f_pbBuffer + iBuffer,
                       pContentKey->pbguidKeyID + pContentKey->iguidKeyID,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pContentKey->wSymmetricCipherType );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pContentKey->wKeyEncryptionCipherType );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pContentKey->cbEncryptedKey );
    iBuffer += SIZEOF( DRM_WORD );

    OEM_SECURE_MEMCPY( f_pbBuffer + iBuffer,
                       pContentKey->pbEncryptedKeyBuffer + pContentKey->iEncryptedKey,
                       pContentKey->cbEncryptedKey );
    iBuffer += pContentKey->cbEncryptedKey;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_RsaPubkey(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr         = DRM_SUCCESS;
    DRM_DWORD                  iBuffer    = 0;
    const DRM_XMR_RSA_PUBKEY  *pRsaPubkey = ( const DRM_XMR_RSA_PUBKEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pRsaPubkey->cbModulus == 0 || pRsaPubkey->pbModulusBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 3 * SIZEOF( DRM_WORD ) + pRsaPubkey->cbModulus,
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pRsaPubkey->dwExponent );
    iBuffer += SIZEOF( DRM_DWORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pRsaPubkey->cbModulus );
    iBuffer += SIZEOF( DRM_WORD );

    OEM_SECURE_MEMCPY( f_pbBuffer + iBuffer,
                       pRsaPubkey->pbModulusBuffer + pRsaPubkey->iModulus,
                       pRsaPubkey->cbModulus );
    iBuffer += pRsaPubkey->cbModulus;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_DomainID(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                  dr         = DRM_SUCCESS;
    DRM_DWORD                   iBuffer    = 0;
    const DRM_XMR_DOMAIN_ID    *pdomainID  = ( const DRM_XMR_DOMAIN_ID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pdomainID->pbAccountID != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
        iBuffer,
        pdomainID->pbAccountID,
        pdomainID->ibAccountID,
        SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    DWORD_TO_NETWORKBYTES( f_pbBuffer, iBuffer, pdomainID->dwRevision );
    iBuffer += SIZEOF( DRM_DWORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_EccDevicekey(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    DRM_DWORD                     iBuffer       = 0;
    const DRM_XMR_ECC_DEVICE_KEY *pEccPubkey    = ( const DRM_XMR_ECC_DEVICE_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( 0 == pEccPubkey->cbKeyData || NULL != pEccPubkey->pbKeyData );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 2 * SIZEOF( DRM_WORD ) + pEccPubkey->cbKeyData,
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pEccPubkey->wEccCurveType );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pEccPubkey->cbKeyData );
    iBuffer += SIZEOF( DRM_WORD );

    OEM_SECURE_MEMCPY( f_pbBuffer + iBuffer,
        pEccPubkey->pbKeyData + pEccPubkey->iKeyData,
        pEccPubkey->cbKeyData );
    iBuffer += pEccPubkey->cbKeyData;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}


/******************************************************************************
** Function :   DRM_XMR_Serialize_GenerationNumber
**
** Synopsis :   Serializes unknown object
**
** Arguments :
**      [f_pvObject] : Actual object returned as void
**      [f_pbBuffer] : Object Buffer
**      [f_piBuffer] : Index marking beginning of object in the buffer
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**
** Notes :  Prototyped in drmxmrdefs.h
******************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_GenerationNumber(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                        dr                = DRM_SUCCESS;
    DRM_DWORD                         iBuffer           = 0;
    const DRM_XMR_GENERATION_NUMBER  *pGenerationNumber = ( const DRM_XMR_GENERATION_NUMBER * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pGenerationNumber->dwValue );
    iBuffer += SIZEOF( DRM_DWORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_Serialize_UnknownObject
**
** Synopsis :   Serializes unknown object
**
** Arguments :
**      [f_pvObject] : Actual object returned as void
**      [f_cbObject] : Size of serialized object
**      [f_pbBuffer] : Object Buffer
**      [f_piBuffer] : Index marking beginning of object in the buffer
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
** Notes :  Prototyped in drmxmrbuilder.h
******************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_UnknownObject(
    __in_ecount( 1 )  const                    DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer )               DRM_BYTE    *f_pbBuffer,
    __in                                       DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )                        DRM_DWORD   *f_piBuffer )
{

    DRM_RESULT                  dr              = DRM_SUCCESS;
    const DRM_BYTE             *pbObject        = NULL;
    DRM_DWORD                   iBuffer         = 0;
    const DRM_XMR_BUILDER_NODE* pChildNode      = NULL;
    DRM_DWORD                   dwBufferToCopy  = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    pChildNode = (const DRM_XMR_BUILDER_NODE *)f_pvObject;

    if( pChildNode->cbLength < XMR_BASE_OBJECT_LENGTH )
    {
        ChkDR(DRM_E_FAIL);
    }

    dwBufferToCopy = pChildNode->cbLength - XMR_BASE_OBJECT_LENGTH;

    ChkBOOL( (f_cbBuffer >= iBuffer + dwBufferToCopy ),
             DRM_E_BUFFERTOOSMALL );

    pbObject = ( const DRM_BYTE * )pChildNode->pvObject;

    DRM_BYT_CopyBytes( f_pbBuffer,
                       *f_piBuffer,
                       pbObject,
                       0,
                       dwBufferToCopy);

    *f_piBuffer += dwBufferToCopy;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_UplinkKID(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr         = DRM_SUCCESS;
    DRM_DWORD                  iBuffer    = 0;
    const DRM_XMR_UPLINK_KID  *pUplinkKID = ( const DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pUplinkKID->cbChainedCheckSum == 0 || pUplinkKID->pbChainedCheckSumBuffer != NULL );
    ChkArg( pUplinkKID->pbguidUplinkKID != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_WORD ) + SIZEOF( DRM_GUID ) + pUplinkKID->cbChainedCheckSum,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pUplinkKID->pbguidUplinkKID,
                       pUplinkKID->iguidUplinkKID,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pUplinkKID->cbChainedCheckSum );
    iBuffer += SIZEOF( DRM_WORD );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pUplinkKID->pbChainedCheckSumBuffer,
                       pUplinkKID->iChainedCheckSum,
                       pUplinkKID->cbChainedCheckSum );
    iBuffer += pUplinkKID->cbChainedCheckSum;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_UplinkKID2(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                 dr         = DRM_SUCCESS;
    DRM_DWORD                  iBuffer    = 0;
    const DRM_XMR_UPLINK_KID  *pUplinkKID = ( const DRM_XMR_UPLINK_KID * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pUplinkKID->cbChainedCheckSum == 0 || pUplinkKID->pbChainedCheckSumBuffer != NULL );
    ChkArg( pUplinkKID->pbguidUplinkKID != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + ( 2 * SIZEOF( DRM_WORD ) ) + SIZEOF( DRM_GUID ) + pUplinkKID->cbChainedCheckSum,
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pUplinkKID->pbguidUplinkKID,
                       pUplinkKID->iguidUplinkKID,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pUplinkKID->wChecksumType );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pUplinkKID->cbChainedCheckSum );
    iBuffer += SIZEOF( DRM_WORD );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pUplinkKID->pbChainedCheckSumBuffer,
                       pUplinkKID->iChainedCheckSum,
                       pUplinkKID->cbChainedCheckSum );
    iBuffer += pUplinkKID->cbChainedCheckSum;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_OptimizedContentKey(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                           dr             = DRM_SUCCESS;
    DRM_DWORD                            iBuffer        = 0;
    const DRM_XMR_OPTIMIZED_CONTENT_KEY *pContentKey    = ( const DRM_XMR_OPTIMIZED_CONTENT_KEY * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pContentKey->cbEncryptedKey == 0 || pContentKey->pbEncryptedKeyBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + 2 * SIZEOF( DRM_WORD ) + pContentKey->cbEncryptedKey,
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pContentKey->wKeyEncryptionCipherType );
    iBuffer += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pContentKey->cbEncryptedKey );
    iBuffer += SIZEOF( DRM_WORD );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pContentKey->pbEncryptedKeyBuffer,
                       pContentKey->iEncryptedKey,
                       pContentKey->cbEncryptedKey );
    iBuffer += pContentKey->cbEncryptedKey;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_RemovalDate(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT                     dr           = DRM_SUCCESS;
    DRM_DWORD                      iBuffer      = 0;
    const DRM_XMR_REMOVAL_DATE    *pRemovalDate = ( const DRM_XMR_REMOVAL_DATE * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer + SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pRemovalDate->dwRemovalDate );
    iBuffer += SIZEOF( DRM_DWORD );

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_AuxKey(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_AUX_KEY *poAuxKey =
                        ( const DRM_XMR_AUX_KEY * )f_pvObject;
    DRM_DWORD i = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( poAuxKey->pEntries != NULL || poAuxKey->cEntries == 0);

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer +
                           SIZEOF( DRM_WORD ) +
                           poAuxKey->cEntries * SIZEOF( DRM_XMR_AUX_KEY_ENTRY ),
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, poAuxKey->cEntries );
    iBuffer += SIZEOF( DRM_WORD );

    for ( i = 0; i < poAuxKey->cEntries; i++ )
    {
        DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, poAuxKey->pEntries[ i ].dwLocation );
        iBuffer += SIZEOF( DRM_DWORD );

        OEM_SECURE_MEMCPY( f_pbBuffer + iBuffer,
                           poAuxKey->pEntries[ i ].rgbKey,
                           DRM_AES_KEYSIZE_128 );
        iBuffer += DRM_AES_KEYSIZE_128;
    }

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_UplinkX(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_UPLINKX *poUplinkX =
                        ( const DRM_XMR_UPLINKX * )f_pvObject;
    DRM_DWORD i = 0;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( poUplinkX->cbCheckSum == 0 || poUplinkX->pbCheckSumBuffer != NULL );
    ChkArg( poUplinkX->pbguidUplinkKID != NULL );
    ChkArg( poUplinkX->pdwLocation != NULL || poUplinkX->cEntries == 0);

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer +
                           SIZEOF( DRM_GUID ) +
                           SIZEOF( DRM_WORD ) +
                           poUplinkX->cbCheckSum +
                           SIZEOF( DRM_DWORD ) +
                           poUplinkX->cEntries * SIZEOF( DRM_DWORD ),
             DRM_E_BUFFERTOOSMALL );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       poUplinkX->pbguidUplinkKID,
                       poUplinkX->iguidUplinkKID,
                       SIZEOF( DRM_GUID ) );
    iBuffer += SIZEOF( DRM_GUID );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, poUplinkX->cbCheckSum );
    iBuffer += SIZEOF( DRM_WORD );

    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       poUplinkX->pbCheckSumBuffer,
                       poUplinkX->iCheckSum,
                       poUplinkX->cbCheckSum );
    iBuffer += poUplinkX->cbCheckSum;

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, poUplinkX->cEntries );
    iBuffer += SIZEOF( DRM_WORD );

    for ( i = 0; i < poUplinkX->cEntries; i++ )
    {
        DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, poUplinkX->pdwLocation[ i ] );
        iBuffer += SIZEOF( DRM_DWORD );
    }

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_Serialize_NDTXAuthObject(
    __in_ecount( 1 )  const      DRM_VOID    *f_pvObject,
    __inout_bcount( f_cbBuffer ) DRM_BYTE    *f_pbBuffer,
    __in                         DRM_DWORD    f_cbBuffer,
    __inout_ecount( 1 )          DRM_DWORD   *f_piBuffer )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_DWORD   iBuffer = 0;
    const DRM_XMR_ND_TX_AUTH_OBJECT *pAuth =
                        ( const DRM_XMR_ND_TX_AUTH_OBJECT * )f_pvObject;

    ChkArg( f_pvObject != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_piBuffer != NULL );
    ChkArg( pAuth->cbDataClient  != 0 && pAuth->pbBufferClient  != NULL );
    ChkArg( pAuth->cbDataService != 0 && pAuth->pbBufferService != NULL );

    iBuffer = *f_piBuffer;

    ChkBOOL( f_cbBuffer >= iBuffer                 +
                           SIZEOF( DRM_WORD )      +
                           SIZEOF( DRM_DWORD )     +
                           SIZEOF( DRM_DWORD )     +
                           pAuth->cbDataClient     +
                           pAuth->cbDataService,
             DRM_E_BUFFERTOOSMALL );

    WORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pAuth->wVersion );
    iBuffer += SIZEOF( DRM_WORD );

    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pAuth->cbDataClient );
    iBuffer += SIZEOF( DRM_DWORD );
    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pAuth->pbBufferClient,
                       pAuth->ibDataClient,
                       pAuth->cbDataClient );
    iBuffer += pAuth->cbDataClient;


    DWORD_TO_NETWORKBYTES ( f_pbBuffer, iBuffer, pAuth->cbDataService );
    iBuffer += SIZEOF( DRM_DWORD );
    DRM_BYT_CopyBytes( f_pbBuffer,
                       iBuffer,
                       pAuth->pbBufferService,
                       pAuth->ibDataService,
                       pAuth->cbDataService );
    iBuffer += pAuth->cbDataService;

    *f_piBuffer = iBuffer;

ErrorExit:
    return dr;
}

#define INSERT_OBJECT_IF_VALID( type, object, parenttype )  \
if ( object.fValid )\
{\
    ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,\
                          SIZEOF( DRM_XMR_BUILDER_LISTNODE ),\
                          ( DRM_VOID ** )&plistnode ) );\
    plistnode->Node.cbLength = _DRM_XMR_GetObjectLength( type, ( DRM_VOID * )&object );\
    plistnode->Node.pvObject = ( DRM_VOID * )&object;\
    plistnode->Node.wType    = type;\
    DRMASSERT( type <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */\
    f_pcontextBuilder->rgpObjectNodes[type] = &plistnode->Node;\
    if ( g_xmrCommonDefines[type].fIsContainer )\
    {\
        plistnode->Node.pvObject = NULL;\
        ChkDR( g_xmrBuilderDefines[type].fnParseStructure( f_pcontextBuilder, &object ) );\
    }\
    DRMASSERT( parenttype <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */\
    plistnode->pNext = ( DRM_XMR_BUILDER_LISTNODE * )f_pcontextBuilder->rgpObjectNodes[parenttype]->pvObject;\
    f_pcontextBuilder->rgpObjectNodes[parenttype]->pvObject = ( DRM_VOID * )plistnode;\
    f_pcontextBuilder->rgpObjectNodes[parenttype]->cbLength += plistnode->Node.cbLength;\
}


static DRM_NO_INLINE DRM_RESULT _DRM_XMR_InsertUnknownObject(
    __inout DRM_XMR_BUILDER_CONTEXT_INTERNAL       *f_pcontextBuilder,
    __in    DRM_XMR_UNKNOWN_OBJECT           const *f_pUnkobject,
    __in    DRM_WORD                                f_wparenttype )
{
    DRM_RESULT                                     dr             = DRM_SUCCESS;
    DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE        *pUnklistnode   = NULL;
    DRM_BYTE                                      *pbUnkXMRObject = NULL;
    DRM_DWORD                                      cbUnkXMRObject = 0;
    ChkArg( f_pcontextBuilder != NULL );
    ChkArg( f_pUnkobject != NULL );

    if ( f_pUnkobject->fValid )
    {
        /* V1 XMR doesn't support unknown objects */
        ChkArg( f_pcontextBuilder->dwVersion >= XMR_VERSION_2 );

        ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                  SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ),
                  ( DRM_VOID ** )&pUnklistnode ) );

        cbUnkXMRObject = f_pUnkobject->cbData;

        ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                  cbUnkXMRObject,
                  ( DRM_VOID ** )&pbUnkXMRObject ) );

        DRM_BYT_CopyBytes( pbUnkXMRObject,
                           0,
                           f_pUnkobject->pbBuffer,
                           f_pUnkobject->ibData,
                           f_pUnkobject->cbData );

        ZEROMEM( pUnklistnode, SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ) );
        pUnklistnode->listNode.Node.wType      = f_pUnkobject->wType;
        pUnklistnode->listNode.Node.cbLength   = cbUnkXMRObject + XMR_BASE_OBJECT_LENGTH;
        pUnklistnode->listNode.Node.pvObject   = pbUnkXMRObject;
        pUnklistnode->wParent                  = f_wparenttype;
        pUnklistnode->pNext                    = NULL;
        pUnklistnode->fDuplicateAllowed        = TRUE;
        pUnklistnode->wFlags                   = f_pUnkobject->wFlags;

        pUnklistnode->pNext    = f_pcontextBuilder->pUnknownObjects;
        f_pcontextBuilder->pUnknownObjects = pUnklistnode;

        if ( _DRM_XMR_IsNativeObject( f_wparenttype ) )
        {
            DRMASSERT( f_wparenttype <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */
            pUnklistnode->listNode.pNext = ( DRM_XMR_BUILDER_LISTNODE * )f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->pvObject;
            f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->pvObject = ( const DRM_VOID * )&( pUnklistnode->listNode );
            f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->cbLength += pUnklistnode->listNode.Node.cbLength;
            ChkOverflow( f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->cbLength, pUnklistnode->listNode.Node.cbLength );
        }
        else
        {
            DRM_XMR_BUILDER_NODE* pnodeParent = _DRM_XMR_GetBuilderNode( f_pcontextBuilder, f_wparenttype );

            if( pnodeParent == NULL )
            {
                ChkDR( DRM_E_XMR_OBJECT_NOT_FOUND );
            }

            /*
            **  Insert this node into parent's child list
            */

            pUnklistnode->listNode.pNext = ( DRM_XMR_BUILDER_LISTNODE * )pnodeParent->pvObject;
            pnodeParent->pvObject = ( const DRM_VOID * )&( pUnklistnode->listNode );
            pnodeParent->cbLength += pUnklistnode->listNode.Node.cbLength;
            ChkOverflow( pnodeParent->cbLength, pUnklistnode->listNode.Node.cbLength );
        }
    }

ErrorExit:
    return dr;
}

DRM_NO_INLINE DRM_RESULT _DRM_XMR_InsertUnknownContainer(
    __inout   DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in      DRM_XMR_UNKNOWN_CONTAINER           *f_pUnkContainer,
    __in      DRM_WORD                             f_wparenttype )
{
    DRM_RESULT                                     dr                   = DRM_SUCCESS;
    DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE        *pUnklistnode         = NULL;
    const DRM_XMR_UNKNOWN_OBJECT                  *pUnkObject           = NULL;
    DRM_XMR_UNKNOWN_CONTAINER                     *pUnkChildContainer   = NULL;

    ChkArg( f_pcontextBuilder != NULL );
    ChkArg( f_pUnkContainer != NULL );

    if ( f_pUnkContainer->fValid )
    {
        /* V1 XMR doesn't support unknown objects */
        ChkArg( f_pcontextBuilder->dwVersion >= XMR_VERSION_2 );

        ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                  SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ),
                  ( DRM_VOID ** )&pUnklistnode ) );

        /* Add the node in the XMR data structure */
        ZEROMEM( pUnklistnode, SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ) );
        pUnklistnode->listNode.Node.wType      = f_pUnkContainer->wType;
        pUnklistnode->listNode.Node.cbLength   = XMR_BASE_OBJECT_LENGTH;
        pUnklistnode->listNode.Node.pvObject   = NULL;
        pUnklistnode->wParent                  = f_wparenttype;
        pUnklistnode->pNext                    = NULL;
        pUnklistnode->fDuplicateAllowed        = TRUE;
        pUnklistnode->wFlags                   = f_pUnkContainer->wFlags;

        pUnklistnode->pNext    = f_pcontextBuilder->pUnknownObjects;
        f_pcontextBuilder->pUnknownObjects = pUnklistnode;

        /* Update the size of all the children before updating the size of the parent */
        pUnkObject = f_pUnkContainer->pObject;
        while( pUnkObject != NULL )
        {
            ChkDR( _DRM_XMR_InsertUnknownObject( f_pcontextBuilder,
                                                 pUnkObject,
                                                 pUnklistnode->listNode.Node.wType ) );

            pUnkObject = pUnkObject->pNext;
        }

        pUnkChildContainer = f_pUnkContainer->pUnkChildcontainer;
        while( pUnkChildContainer != NULL )
        {
            if( pUnkChildContainer->fValid )
            {
                ChkDR( _DRM_XMR_InsertUnknownContainer( f_pcontextBuilder,
                                                        pUnkChildContainer,
                                                        pUnkChildContainer->wType) );
            }
            pUnkChildContainer = pUnkChildContainer->pNext;
        }

        /* Update the size of the parent node */
        if ( _DRM_XMR_IsNativeObject( f_wparenttype ) )
        {
            DRMASSERT( f_wparenttype <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */
            pUnklistnode->listNode.pNext = ( DRM_XMR_BUILDER_LISTNODE * )f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->pvObject;
            f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->pvObject = ( const DRM_VOID * )&( pUnklistnode->listNode );
            f_pcontextBuilder->rgpObjectNodes[f_wparenttype]->cbLength += pUnklistnode->listNode.Node.cbLength;
        }
        else
        {
            DRM_XMR_BUILDER_NODE* pnodeParent = _DRM_XMR_GetBuilderNode( f_pcontextBuilder, f_wparenttype );

            if( pnodeParent == NULL )
            {
                ChkDR( DRM_E_XMR_OBJECT_NOT_FOUND );
            }
            /*
            **  Insert this node into parent's child list
            */
            pUnklistnode->listNode.pNext = ( DRM_XMR_BUILDER_LISTNODE * )pnodeParent->pvObject;
            pnodeParent->pvObject = ( const DRM_VOID * )&( pUnklistnode->listNode );
            pnodeParent->cbLength += pUnklistnode->listNode.Node.cbLength;
        }
    }

ErrorExit:
    return dr;
}


/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_Invalid(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    return DRM_E_INVALIDARG;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_GlobalContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                     dr               = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE      *plistnode        = NULL;
    const DRM_XMR_UNKNOWN_OBJECT  *pUnkObject       = NULL;
    DRM_XMR_GLOBAL_REQUIREMENTS   *pglobalContainer =
                        ( DRM_XMR_GLOBAL_REQUIREMENTS * )f_pvObject;
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPIRATION_OBJECT,
                            pglobalContainer->Expiration,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT,
                            pglobalContainer->InclusionList,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT,
                            pglobalContainer->ExpirationAfterStore,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_ISSUEDATE_OBJECT,
                            pglobalContainer->IssueDate,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT,
                            pglobalContainer->GracePeriod,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_METERING_OBJECT,
                            pglobalContainer->Metering,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_PRIORITY_OBJECT,
                            pglobalContainer->Priority,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_SETTINGS_OBJECT,
                            pglobalContainer->Rights,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT,
                            pglobalContainer->SerialNumber,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_SOURCEID_OBJECT,
                            pglobalContainer->SourceID,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT,
                            pglobalContainer->RestrictedSourceID,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );

    if( f_pcontextBuilder->dwVersion == XMR_VERSION_1 )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT,
                                pglobalContainer->RevocationInformationVersion,
                                XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );

        if( pglobalContainer->ExpirationAfterUse.fValid )
        {
            if( pglobalContainer->ExpirationAfterUse.dwVersion == 2 )
            {
                INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT,
                                        pglobalContainer->ExpirationAfterUse,
                                        XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
            }
            else
            {
                INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT,
                                        pglobalContainer->ExpirationAfterUse,
                                        XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
            }
        }
    }
    else
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT,
                                pglobalContainer->RevocationInformationVersion,
                                XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT,
                                pglobalContainer->ExpirationAfterUse,
                                XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    }

    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT,
                            pglobalContainer->DomainID,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT,
                            pglobalContainer->EmbeddingBehavior,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );

    /* Minimum Environment Object is not valid in V3+, Security Level Object is used instead */
    if( f_pcontextBuilder->dwVersion < XMR_VERSION_3 )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT,
                                pglobalContainer->MinimumEnvironment,
                                XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    }
    else
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_SECURITY_LEVEL,
                                pglobalContainer->MinimumEnvironment,
                                XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );
    }

    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT,
                            pglobalContainer->RemovalDate,
                            XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );

    /*
    ** for Policy Metadata, we don't add object using INSERT_OBJECT_IF_VALID as
    ** we can have multiple Metadata objects without a container of their own
    */
    if( pglobalContainer->PolicyMetadata.fValid )
    {
        ChkDR( DRM_XMR_ParseStructure_PolicyMetadata(  f_pcontextBuilder,
                                                      &pglobalContainer->PolicyMetadata ) );
    }

    if (   !pglobalContainer->SourceID.fValid
        && pglobalContainer->RestrictedSourceID.fValid )
    {
        ChkDR( DRM_E_INVALID_LICENSE );
    }

    pUnkObject = pglobalContainer->pUnknownObjects;
    while( pUnkObject != NULL )
    {
        if( pUnkObject->fValid )
        {
            ChkDR( _DRM_XMR_InsertUnknownObject( f_pcontextBuilder,
                                                 pUnkObject,
                                                 XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER ) );
        }
        pUnkObject = pUnkObject->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_OuterContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                   dr         = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE    *plistnode  = NULL;
    DRM_XMR_UNKNOWN_CONTAINER    *pUnkContainer = NULL;

    DRM_XMR_OUTER_CONTAINER     *pouterContainer =
                            ( DRM_XMR_OUTER_CONTAINER * )f_pvObject;
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT,
                            pouterContainer->generationNumber,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,
                            pouterContainer->containerKeys,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_REVOCATION_CONTAINER,
                            pouterContainer->containerRevocation,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER,
                            pouterContainer->containerPlaylistBurnPolicies,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );
    if( pouterContainer->containerCopyPolicies.fValid )
    {
        if( pouterContainer->containerCopyPolicies.dwVersion == 1 )
        {
            ChkBOOL( f_pcontextBuilder->dwVersion == XMR_VERSION_1, DRM_E_INVALID_LICENSE );
            INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER,
                                    pouterContainer->containerCopyPolicies,
                                    XMR_OBJECT_TYPE_OUTER_CONTAINER );
        }
        else
        {
            INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER,
                                    pouterContainer->containerCopyPolicies,
                                    XMR_OBJECT_TYPE_OUTER_CONTAINER );
        }
    }
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,
                            pouterContainer->containerPlaybackPolicies,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER,
                            pouterContainer->containerCopyToPCPolicies,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,
                            pouterContainer->containerGlobalPolicies,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );

    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER,
                            pouterContainer->containerNDTxAuth,
                            XMR_OBJECT_TYPE_OUTER_CONTAINER );

    pUnkContainer = &( pouterContainer->containerUnknown );
    while( pUnkContainer != NULL )
    {
        if( pUnkContainer->fValid )
        {
            ChkDR( _DRM_XMR_InsertUnknownContainer( f_pcontextBuilder,
                                                    pUnkContainer,
                                                    XMR_OBJECT_TYPE_OUTER_CONTAINER ) );
        }
        pUnkContainer = pUnkContainer->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_CopyToPCContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    /* This container has no children */
    return DRM_SUCCESS;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_PlaybackContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                    dr             = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE     *plistnode      = NULL;
    const DRM_XMR_UNKNOWN_OBJECT *pUnkObject     = NULL;
    DRM_XMR_PLAYBACK_RIGHTS      *pPlayback      =
                            ( DRM_XMR_PLAYBACK_RIGHTS * )f_pvObject;
    DRM_XMR_UNKNOWN_CONTAINER   *pUnkContainer  = NULL;
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER,
                            pPlayback->containerExplicitAnalogVideoProtection,
                            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER,
                            pPlayback->containerExplicitDigitalAudioProtection,
                            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT,
                            pPlayback->opl,
                            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT,
                            pPlayback->PlayCount,
                            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER );

    pUnkObject = pPlayback->pUnknownObjects;
    while( pUnkObject != NULL )
    {
        if( pUnkObject->fValid )
        {
            ChkDR( _DRM_XMR_InsertUnknownObject( f_pcontextBuilder,
                                                 pUnkObject,
                                                 XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER ) );
        }
        pUnkObject = pUnkObject->pNext;
    }
    pUnkContainer = &(pPlayback->UnknownContainer);
    while( pUnkContainer != NULL )
    {
        if( pUnkContainer->fValid )
        {
            ChkDR( _DRM_XMR_InsertUnknownContainer( f_pcontextBuilder,
                                                    pUnkContainer,
                                                    XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER) );
        }
        pUnkContainer = pUnkContainer->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_CopyContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                    dr             = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE     *plistnode      = NULL;
    const DRM_XMR_UNKNOWN_OBJECT *pUnkObject     = NULL;
    DRM_XMR_COPY_RIGHTS          *pCopyrights    = ( DRM_XMR_COPY_RIGHTS * )f_pvObject;
    DRM_XMR_UNKNOWN_CONTAINER    *pUnkContainer  = NULL;

    if( pCopyrights->fValid )
    {
        if( pCopyrights->dwVersion == 1 )
        {
            ChkBOOL( f_pcontextBuilder->dwVersion == XMR_VERSION_1, DRM_E_INVALID_LICENSE );
            INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_COPYCOUNT_OBJECT,
                                    pCopyrights->CopyCount,
                                    XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER );
            INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT,
                                    pCopyrights->CopyProtectionLevel,
                                    XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER );
        }
        else
        {
            INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT,
                                    pCopyrights->CopyCount,
                                    XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER );
            INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT,
                                    pCopyrights->MoveEnabler,
                                    XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER );

            pUnkObject = pCopyrights->pUnknownObjects;
            while( pUnkObject != NULL )
            {
                if( pUnkObject->fValid )
                {
                    ChkDR( _DRM_XMR_InsertUnknownObject( f_pcontextBuilder,
                                                         pUnkObject,
                                                         XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER ) );
                }
                pUnkObject = pUnkObject->pNext;
            }

            pUnkContainer = &(pCopyrights->UnknownContainer);
            while( pUnkContainer != NULL )
            {
                if( pUnkContainer->fValid )
                {
                    ChkDR( _DRM_XMR_InsertUnknownContainer( f_pcontextBuilder,
                                                            pUnkContainer,
                                                            XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER ) );
                }
                pUnkContainer = pUnkContainer->pNext;
            }
        }
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL
DRM_XMR_ParseStructure_ExplicitAnalogVideoOutputProtectionContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                   dr         = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE    *plistnode  = NULL;
    DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION    *pcontainerExplicitAnalogVideoProtection =
                        ( DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION * )f_pvObject;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST     *pConfigList =
                    pcontainerExplicitAnalogVideoProtection->plistOutputConfiguration;

    while( pConfigList != NULL && f_pcontextBuilder->rgpObjectNodes != NULL )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT,
                                pConfigList->Config,
                                XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER );

        pConfigList = pConfigList->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL
DRM_XMR_ParseStructure_ExplicitDigitalAudioOutputProtectionContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                   dr         = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE    *plistnode  = NULL;
    DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION    *pcontainerExplicitDigitalAudioProtection =
                        ( DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION * )f_pvObject;
    DRM_XMR_AUDIO_OUTPUT_CONFIGURATION_LIST     *pConfigList =
                    pcontainerExplicitDigitalAudioProtection->plistOutputConfiguration;

    while( pConfigList != NULL && f_pcontextBuilder->rgpObjectNodes != NULL )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT,
                                pConfigList->Config,
                                XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER );

        pConfigList = pConfigList->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_PolicyMetadata(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                        dr = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE         *plistnode  = NULL;
    DRM_XMR_POLICY_METADATA          *pPolicyMetadataObjects = ( DRM_XMR_POLICY_METADATA * )f_pvObject;
    DRM_XMR_POLICY_METADATA_LIST     *pPolicyMetadataList =  pPolicyMetadataObjects->plistPolicyMetadataObjects;

    while( pPolicyMetadataList != NULL && f_pcontextBuilder->rgpObjectNodes != NULL )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT,
                                pPolicyMetadataList->MetadataObject,
                                XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER );

        pPolicyMetadataList = pPolicyMetadataList->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_RevocationContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE    *plistnode      = NULL;
    DRM_XMR_REVOCATION          *pKeyMaterial   = ( DRM_XMR_REVOCATION * )f_pvObject;

    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT,
                            pKeyMaterial->RsaLicenseGranterKey,
                            XMR_OBJECT_TYPE_REVOCATION_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_USERID_OBJECT,
                            pKeyMaterial->UserID,
                            XMR_OBJECT_TYPE_REVOCATION_CONTAINER );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_KeyMaterialContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE    *plistnode      = NULL;
    DRM_XMR_KEY_MATERIAL        *pKeyMaterial   = ( DRM_XMR_KEY_MATERIAL * )f_pvObject;

    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,
                            pKeyMaterial->ContentKey,
                            XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT,
                            pKeyMaterial->DeviceKey,
                            XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT,
                            pKeyMaterial->ECCKey,
                            XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    if( f_pcontextBuilder->dwVersion == XMR_VERSION_1 )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_UPLINK_KID_OBJECT,
                                pKeyMaterial->UplinkKid,
                                XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    }
    else
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT,
                                pKeyMaterial->UplinkKid,
                                XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    }
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_AUX_KEY_OBJECT,
                            pKeyMaterial->AuxKey,
                            XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_UPLINKX_OBJECT,
                            pKeyMaterial->UplinkX,
                            XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );
    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT,
                            pKeyMaterial->OptimizedContentKey,
                            XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_PlaylistBurnContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                    dr            = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE     *plistnode     = NULL;
    const DRM_XMR_UNKNOWN_OBJECT *pUnkObject    = NULL;

    DRM_XMR_PLAYLIST_BURN_RIGHTS *pPlaylistBurn =
                                    ( DRM_XMR_PLAYLIST_BURN_RIGHTS * )f_pvObject;

    INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT,
                            pPlaylistBurn->Restrictions,
                            XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER );

    pUnkObject = pPlaylistBurn->pUnknownObjects;
    while( pUnkObject != NULL )
    {
        if( pUnkObject->fValid )
        {
            ChkDR( _DRM_XMR_InsertUnknownObject( f_pcontextBuilder,
                                                 pUnkObject,
                                                 XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER ) );
        }
        pUnkObject = pUnkObject->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** For summary, see prototype definition in drmxmrbuilder.h
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_ParseStructure_NDTXAuthContainer(
    __inout_ecount( 1 )     DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __in_ecount( 1 ) const  DRM_VOID                            *f_pvObject )
{
    DRM_RESULT                          dr             = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE           *plistnode      = NULL;
    const DRM_XMR_ND_TX_AUTH_CONTAINER *pAuthContainer = ( const DRM_XMR_ND_TX_AUTH_CONTAINER * )f_pvObject;
    DRM_XMR_ND_TX_AUTH_OBJECT          *pAuthObject    = NULL;

    pAuthObject = pAuthContainer->pAuthObject;
    while( pAuthObject != NULL )
    {
        INSERT_OBJECT_IF_VALID( XMR_OBJECT_TYPE_ND_TX_AUTH_OBJECT,
                                (*pAuthObject),
                                XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER );
        pAuthObject = pAuthObject->pNext;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _XMR_AddHierarchy
**
** Synopsis :   Adds all the ancestors of a node, and builds the correct
**              hierarchy
**
** Arguments :  f_pcontextBuilder -
**              f_plistnode       - Linked list node containing the node whose
**                      ancestors are to be added to the builder tree
**
** Returns :    DRM_E_XMR_OBJECT_ALREADY_EXISTS if the node already exists, or one
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _XMR_AddHierarchy(
    __inout_ecount( 1 ) DRM_XMR_BUILDER_CONTEXT_INTERNAL    *f_pcontextBuilder,
    __inout_ecount( 1 ) DRM_XMR_BUILDER_LISTNODE            *f_plistnode )
{
    /* Note: Internal function - No need to check input */
    DRM_RESULT                  dr              = DRM_SUCCESS;
    DRM_WORD                    wParent         = 0;
    DRM_XMR_BUILDER_LISTNODE   *plistnodeChild  = f_plistnode;

    wParent = DRM_XMR_GetParent( f_plistnode );
    ChkArg( wParent <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );

    /* This loop adds any ancestors not already included */
    while ( wParent != XMR_OBJECT_TYPE_INVALID )
    {
        ChkArg( wParent <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );   /* Ensure we don't buffer overrun rgpObjectNodes array */
        if ( f_pcontextBuilder->rgpObjectNodes[wParent] == NULL )
        {
            DRM_XMR_BUILDER_LISTNODE    *plistnode = NULL;

            /*
            **  Add object for this parent
            */
            ChkDR( DRM_STK_Alloc( &f_pcontextBuilder->contextStack,
                                  SIZEOF( DRM_XMR_BUILDER_LISTNODE ),
                                  ( DRM_VOID * * ) &plistnode ) );
            plistnode->Node.wType     = wParent;
            plistnode->Node.cbLength  = XMR_BASE_OBJECT_LENGTH
                                      + plistnodeChild->Node.cbLength;
            plistnode->Node.pvObject  = ( const DRM_VOID * )plistnodeChild;
            plistnode->pNext          = NULL;
            f_pcontextBuilder->rgpObjectNodes[wParent] = &plistnode->Node;
            plistnodeChild = plistnode;
            wParent = g_xmrCommonDefines[wParent].wParent;

        }
        else
        {
            DRM_XMR_BUILDER_NODE    *pnodeParent =
                                f_pcontextBuilder->rgpObjectNodes[wParent];
            if ( !DRM_XMR_IsDuplicateAllowed( plistnodeChild ) )
            {
                const DRM_XMR_BUILDER_LISTNODE    *plistnode =
                            ( const DRM_XMR_BUILDER_LISTNODE * )pnodeParent->pvObject;
                /*
                **  Check whether a duplicate exists
                */
                while( plistnode != NULL )
                {
                    if ( plistnode->Node.wType == plistnodeChild->Node.wType )
                    {
                        ChkDR( DRM_E_XMR_OBJECT_ALREADY_EXISTS );
                    }
                    plistnode = plistnode->pNext;
                }
            }

            /*
            **  Insert this node into parent's child list
            */
            plistnodeChild->pNext = ( DRM_XMR_BUILDER_LISTNODE * )pnodeParent->pvObject;
            pnodeParent->pvObject = ( const DRM_VOID * )plistnodeChild;
            pnodeParent->cbLength += plistnodeChild->Node.cbLength;
            break;
        }
    }

    /*
    **  Now update length of all ancestors of the current wParent
    */
    while ( wParent != XMR_OBJECT_TYPE_OUTER_CONTAINER )
    {
        DRMASSERT( g_xmrCommonDefines[wParent].wParent <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */
        f_pcontextBuilder->rgpObjectNodes[g_xmrCommonDefines[wParent].wParent]->cbLength +=
                                                plistnodeChild->Node.cbLength;
        wParent = g_xmrCommonDefines[wParent].wParent;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   XMR_AddUnknownHierarchy
**
** Synopsis :   Adds all the ancestors of a node, and builds the correct
**              hierarchy
**
** Arguments :  f_pcontextBuilder -
**              f_plistnode       - Linked list node containing the node whose
**                      ancestors are to be added to the builder tree
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is invalid
**      DRM_E_XMR_OBJECT_ALREADY_EXISTS
**          Node already exists
**      DRM_E_XMR_OBJECT_NOT_FOUND
**          Failed to find the parent node
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL  DRM_XMR_AddUnknownHierarchy (
    __inout_ecount( 1 )  DRM_XMR_BUILDER_CONTEXT_INTERNAL        *f_pcontextBuilder,
    __inout_ecount( 1 )  DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE  *f_plistnode )

{
    /* Note: Internal function - No need to check input */
    DRM_RESULT                   dr          = DRM_SUCCESS;
    DRM_WORD                     wParent     = 0;
    DRM_XMR_BUILDER_NODE        *pnodeParent = NULL;
    const DRM_XMR_BUILDER_NODE  *pnode       = NULL;

    DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE   *plistnodeChild  = f_plistnode;

    ChkArg( f_plistnode->listNode.Node.wType > XMR_OBJECT_TYPE_INVALID );

    wParent =  f_plistnode->wParent;
    pnode   = &( f_plistnode->listNode.Node );

    pnodeParent = _DRM_XMR_GetBuilderNode( f_pcontextBuilder, wParent );
    if( pnodeParent != NULL && ( pnode->wType == pnodeParent->wType ) )
    {
        ChkDR(DRM_E_FAIL);
    }

    if ( _DRM_XMR_IsNativeObject( wParent ) )
    {
        /*
        ** All known objects are deferred to _XMR_AddHierarchy
        */
        ChkDR( _XMR_AddHierarchy( f_pcontextBuilder, &( plistnodeChild->listNode ) ) );
    }
    else
    {
        /*
        ** We require that if the parent of the object is not a well known
        ** XMR object, parent should be added before adding the node itself.
        ** Therefore, the builder node for the parent should be non-null
        */

        if( pnodeParent == NULL )
        {
            ChkDR( DRM_E_XMR_OBJECT_NOT_FOUND );
        }

        /*
        **  Insert this node into parent's child list
        */

        f_plistnode->listNode.pNext = ( DRM_XMR_BUILDER_LISTNODE * )pnodeParent->pvObject;
        pnodeParent->pvObject = ( const DRM_VOID * )&( f_plistnode->listNode );
        pnodeParent->cbLength += f_plistnode->listNode.Node.cbLength;

        /*
        **  Now update length of all ancestors of the current wParent
        */

        while ( wParent != XMR_OBJECT_TYPE_OUTER_CONTAINER )
        {
            if ( _DRM_XMR_IsNativeObject( wParent ) )
            {
                DRMASSERT( g_xmrCommonDefines[wParent].wParent <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */
                f_pcontextBuilder->rgpObjectNodes[g_xmrCommonDefines[wParent].wParent]->cbLength +=
                                                        plistnodeChild->listNode.Node.cbLength;
                wParent = g_xmrCommonDefines[wParent].wParent;
            }
            else
            {
                wParent = DRM_XMR_GetParent((const DRM_XMR_BUILDER_LISTNODE *)pnodeParent);
                pnodeParent = _DRM_XMR_GetBuilderNode( f_pcontextBuilder, wParent );
                pnodeParent->cbLength += plistnodeChild->listNode.Node.cbLength;
            }
        }
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_StartLicense
**
** Synopsis :   Builder API used to initiate a builder context
**
** Arguments :  f_pbStack, f_cbStack - Stack used to allocate internal structures
**              f_pidRights - Rights ID ( same as LID in Crescent )
**              f_pcontextBuilder
**
** Notes    :   Memory will not be freed from this stack. The caller should just
**              throw away this stack buffer after the license has been built.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_StartLicense(
    __inout_bcount( f_cbStack ) DRM_BYTE                *f_pbStack,
    __in                        DRM_DWORD                f_cbStack,
    __in_ecount( 1 )      const DRM_ID                  *f_pidRights,
    __in                        DRM_DWORD                f_dwVersion,
    __inout_ecount( 1 )         DRM_XMR_BUILDER_CONTEXT *f_pcontextBuilder )
{
    DRM_RESULT                           dr  = DRM_SUCCESS;
    DRM_XMR_BUILDER_NODE                *pnodeOuterContainer  = NULL;
    DRM_XMR_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder =
                            ( DRM_XMR_BUILDER_CONTEXT_INTERNAL * )f_pcontextBuilder;

    ChkArg( pcontextBuilder != NULL );
    ChkArg( f_pidRights     != NULL );
    ChkArg( f_dwVersion     == XMR_VERSION_1
         || f_dwVersion     == XMR_VERSION_3 );  /* XMR_VERSION_2 is not supported */

    DRMCASSERT(NO_OF( g_xmrBuilderDefines ) == NO_OF(g_xmrCommonDefines ));

    pcontextBuilder->pidRights              = f_pidRights;
    pcontextBuilder->dwVersion              = f_dwVersion;
    pcontextBuilder->contextStack.cbStack   = f_cbStack;
    pcontextBuilder->contextStack.pbStack   = f_pbStack;
    pcontextBuilder->contextStack.nStackTop = 0;
    pcontextBuilder->pUnknownObjects        = NULL;

    /*
    **  Add a table for list of containers
    */
    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                          SIZEOF( DRM_XMR_BUILDER_NODE * ) * ( XMR_OBJECT_TYPE_MAXIMUM_DEFINED + 1 ),
                          ( DRM_VOID * * )&pcontextBuilder->rgpObjectNodes ) );
    ZEROMEM( pcontextBuilder->rgpObjectNodes,
             SIZEOF( DRM_XMR_BUILDER_NODE * ) * ( XMR_OBJECT_TYPE_MAXIMUM_DEFINED + 1 ) );

    /*
    **  Add the outer container object
    */
    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                          SIZEOF( DRM_XMR_BUILDER_NODE ),
                          ( DRM_VOID * * )&pnodeOuterContainer ) );
    pnodeOuterContainer->wType     = XMR_OBJECT_TYPE_OUTER_CONTAINER;
    pnodeOuterContainer->cbLength  = XMR_BASE_OBJECT_LENGTH;

    /*
    **  Add length of signature object to outer container. Signature object will
    **  not be added explicitly by the caller
    **  The length of the signature object is determined by the license version
    */
    pnodeOuterContainer->cbLength += XMR_BASE_OBJECT_LENGTH + SIZEOF( DRM_WORD ) * 2;

    /* XMR_VERSION_2 is not supported */
    switch( f_dwVersion )
    {
    case XMR_VERSION_1: /* Fall through */
    case XMR_VERSION_3:
        pnodeOuterContainer->cbLength += XMR_AES_OMAC_SIGNATURE_LENGTH;
        break;
    default:
        AssertChkArg( FALSE );
        break;
    }

    pnodeOuterContainer->pvObject  = NULL;
    pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_OUTER_CONTAINER] = pnodeOuterContainer;

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function :   DRM_XMR_InitializeObject
**
** Synopsis :   Sets the defaults for a particular object. Caller can then set
**              the required values it knows about
**
** Arguments :  f_wObjectType   - type
**              f_pvObject      - Object passed as a void
**
** Note      :  This method memsets the entire structure of the objects to 0. It
**              then sets the fValid flag on.
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMR_InitializeObject(
    __in                DRM_WORD    f_wObjectType,
    __inout_ecount( 1 ) DRM_VOID   *f_pvObject )
{
    DRM_RESULT      dr      = DRM_SUCCESS;
    /*  First field of each structure must be the fValid flag */
    DRM_BOOL       *pfValid = ( DRM_BOOL * )f_pvObject;

    ChkArg( f_wObjectType > XMR_OBJECT_TYPE_INVALID );
    ChkArg( f_wObjectType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );
    ChkArg( f_pvObject != NULL || g_xmrCommonDefines[f_wObjectType].fIsContainer );

    /*
    **  Set the defaults
    */
    if ( f_pvObject != NULL )
    {
        MEMSET( ( DRM_BYTE * )f_pvObject, 0, g_xmrBuilderDefines[f_wObjectType].cbStructureSize );
        ( *pfValid ) = TRUE;
        ChkDR( g_xmrBuilderDefines[f_wObjectType].fnSetDefaults( f_pvObject ) );
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_AddObject
**
** Synopsis :   Adds any object to the builder tree. This API can be used to add
**              either a leaf object, or a container structure
**
** Arguments :  f_pcontextBuilder   -
**              f_wObjectType   - type
**              f_pvObject      - Object passed as a void. This can be NULL, if
**                  the object is a container.
** Returns :    Unless a duplicate is permissible for this type, adding a
**              duplicate object will cause an error. If a container has been
**              explicitly added and then one of it's child nodes is added
**              separately, it is treated as a duplicate as well
**
** Notes :      The caller can add an empty container to a license.
**              The caller MUST make sure that no part of the object is freed,
**              or goes out of scope before the license is built. This is
**              because the internal builder context refers to the original
**              object ( instead of making a copy for efficiency reasons )
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_AddObject(
    __inout_ecount( 1 )    DRM_XMR_BUILDER_CONTEXT *f_pcontextBuilder,
    __in                   DRM_WORD                 f_wObjectType,
    __in_ecount( 1 ) const DRM_VOID                *f_pvObject )
{
    DRM_RESULT                  dr  = DRM_SUCCESS;
    DRM_XMR_BUILDER_LISTNODE   *plistnode  = NULL;
    DRM_XMR_BUILDER_CONTEXT_INTERNAL *pcontextBuilder =
                        ( DRM_XMR_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    ChkArg( pcontextBuilder != NULL );
    ChkArg( f_wObjectType >  XMR_OBJECT_TYPE_INVALID );
    ChkArg( f_wObjectType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );
    ChkArg( f_pvObject != NULL || g_xmrCommonDefines[f_wObjectType].fIsContainer );
    ChkArg( pcontextBuilder->rgpObjectNodes != NULL );

    if ( f_wObjectType != XMR_OBJECT_TYPE_OUTER_CONTAINER )
    {
        ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
                              SIZEOF( DRM_XMR_BUILDER_LISTNODE ),
                              ( DRM_VOID ** )&plistnode ) );
        plistnode->Node.cbLength = _DRM_XMR_GetObjectLength( f_wObjectType, f_pvObject );
        plistnode->Node.pvObject = f_pvObject;
        plistnode->Node.wType    = f_wObjectType;
        plistnode->pNext         = NULL;
        DRMASSERT( f_wObjectType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED );    /* Ensure we don't buffer overrun rgpObjectNodes array */
        pcontextBuilder->rgpObjectNodes[f_wObjectType] = &plistnode->Node;
        if ( g_xmrCommonDefines[f_wObjectType].fIsContainer && ( f_pvObject != NULL ) )
        {
            /*
            **  This will add all the children of this container and also update
            **  the length of f_pvObject
            */
            plistnode->Node.pvObject = NULL;
            ChkDR( g_xmrBuilderDefines[f_wObjectType].fnParseStructure( pcontextBuilder, f_pvObject ) );
        }
        ChkDR( _XMR_AddHierarchy( pcontextBuilder, plistnode ) );
    }
    else
    {
        ChkDR( g_xmrBuilderDefines[XMR_OBJECT_TYPE_OUTER_CONTAINER].fnParseStructure( pcontextBuilder, f_pvObject ) );
    }

ErrorExit:
    if ( DRM_FAILED( dr ) && ( plistnode != NULL ) )
    {
        ( void ) DRM_STK_Free( &pcontextBuilder->contextStack,
                             ( DRM_VOID * )plistnode );
    }
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_AddUnknownObject
**
** Synopsis :   Adds unknown object to the builder tree. This API can be used to add
**              either a leaf object, or a container structure
**
** Arguments :  f_pcontextBuilder   -
**              f_wObjectType   - type
**              f_pvObject      - Object passed as a void. This can be NULL, if
**                  the object is a container.
** Returns :    Unless a duplicate is permissible for this type, adding a
**              duplicate object will cause an error. If a container has been
**              explicitly added and then one of it's child nodes is added
**              separately, it is treated as a duplicate as well
**
** Notes :      The caller can add an empty container to a license.
**              The caller MUST make sure that no part of the object is freed,
**              or goes out of scope before the license is built. This is
**              because the internal builder context refers to the original
**              object ( instead of making a copy for efficiency reasons )
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMR_AddUnknownObject(
    __inout_ecount( 1 )                 DRM_XMR_BUILDER_CONTEXT *f_pcontextBuilder,
    __in                                DRM_WORD                 f_wObjectType,
    __in                                DRM_BOOL                 f_fDuplicateAllowed,
    __in                                DRM_WORD                 f_wParent,
    __in                                DRM_WORD                 f_wFlags,
    __in_bcount_opt( f_cbObject ) const DRM_BYTE                *f_pbObject,
    __in                                DRM_DWORD                f_cbObject )
{
    DRM_RESULT dr  = DRM_SUCCESS;
    DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE *plistnode  = NULL;
    DRM_XMR_BUILDER_CONTEXT_INTERNAL       *pcontextBuilder =
            ( DRM_XMR_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    ChkArg( pcontextBuilder != NULL );

    ChkArg( f_wObjectType > XMR_OBJECT_TYPE_INVALID );

    ChkArg( f_pbObject != NULL || f_wFlags & XMR_FLAGS_CONTAINER );


    if( !( ( f_wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE )
        || f_wObjectType == XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER
        || f_wObjectType == XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER
        || f_wObjectType == XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT
        || f_wObjectType == XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT ) )
    {
        ChkDR( DRM_E_XMR_INVALID_UNKNOWN_OBJECT );
    }

    ChkDR( DRM_STK_Alloc( &pcontextBuilder->contextStack,
        SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ),
        ( DRM_VOID ** )&plistnode ) );

    ZEROMEM( plistnode, SIZEOF( DRM_XMR_BUILDER_UNKNOWNOBJECT_LISTNODE ) );

    plistnode->listNode.Node.wType      = f_wObjectType;
    plistnode->listNode.Node.cbLength   = XMR_BASE_OBJECT_LENGTH + f_cbObject;
    plistnode->listNode.Node.pvObject   = f_pbObject;
    plistnode->wParent                  = f_wParent;
    plistnode->pNext                    = NULL;
    plistnode->fDuplicateAllowed        = f_fDuplicateAllowed;
    plistnode->wFlags                   = f_wFlags;

    /*
    ** Add it to linked list
    */
    plistnode->pNext    = pcontextBuilder->pUnknownObjects;
    pcontextBuilder->pUnknownObjects = plistnode;

    ChkDR( DRM_XMR_AddUnknownHierarchy ( pcontextBuilder, plistnode ) );

ErrorExit:
    if ( DRM_FAILED( dr ) && ( plistnode != NULL ) )
    {
        ( void ) DRM_STK_Free( &pcontextBuilder->contextStack,
            ( DRM_VOID * )plistnode );
    }
    return dr;
}

static DRM_VOID DRM_CALL _XMR_FilterNotMustUnderstandUnknownObjects(
    DRM_XMR_UNKNOWN_OBJECT      *f_pUnkObject )
{
    while( f_pUnkObject != NULL )
    {
        if( ( f_pUnkObject->wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0 )
        {
            f_pUnkObject->fValid = FALSE;
        }
        f_pUnkObject = f_pUnkObject->pNext;
    }
}

static DRM_VOID DRM_CALL _XMR_FilterNotMustUnderstandUnknownContainers(
    DRM_XMR_UNKNOWN_CONTAINER      *f_pUnkContainer )
{
    while( f_pUnkContainer != NULL )
    {
        /* Only filter unknown containers that are NOT must understand */
        if( ( f_pUnkContainer->wFlags & XMR_FLAGS_MUST_UNDERSTAND ) == 0 )
        {
            f_pUnkContainer->fValid = FALSE;
        }
        if( f_pUnkContainer->fValid )
        {
            _XMR_FilterNotMustUnderstandUnknownObjects( f_pUnkContainer->pObject );
        }

        f_pUnkContainer = f_pUnkContainer->pNext;
    }
}

DRM_API DRM_RESULT DRM_CALL DRM_XMR_FilterNonMustUnderstandObjectsByVersion(
    __in_ecount( 1 )       DRM_XMR_LICENSE  *f_pXmrLicense,
    __in                   DRM_DWORD         f_dwVersion )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    ChkArg( f_pXmrLicense != NULL );
    ChkArg( f_dwVersion   == XMR_VERSION_1
         || f_dwVersion   == XMR_VERSION_3 ); /* XMR_VERSION_2 is not supported */

    if( f_dwVersion == XMR_VERSION_3 )
    {
        /* Nothing to filter for version 3 */
        goto ErrorExit;
    }

    f_pXmrLicense->containerOuter.generationNumber.fValid = FALSE;
    f_pXmrLicense->containerOuter.containerGlobalPolicies.PolicyMetadata.fValid = FALSE;
    f_pXmrLicense->containerOuter.containerGlobalPolicies.EmbeddingBehavior.fValid = FALSE;
    f_pXmrLicense->containerOuter.containerCopyToPCPolicies.fValid = FALSE;

    _XMR_FilterNotMustUnderstandUnknownContainers(
             &f_pXmrLicense->containerOuter.containerUnknown);

    if( f_pXmrLicense->containerOuter.containerGlobalPolicies.fValid )
    {
        _XMR_FilterNotMustUnderstandUnknownObjects(
            f_pXmrLicense->containerOuter.containerGlobalPolicies.pUnknownObjects );
    }
    if( f_pXmrLicense->containerOuter.containerCopyPolicies.fValid )
    {
        _XMR_FilterNotMustUnderstandUnknownObjects(
            f_pXmrLicense->containerOuter.containerCopyPolicies.pUnknownObjects );
    }
    if( f_pXmrLicense->containerOuter.containerPlaybackPolicies.fValid )
    {
        _XMR_FilterNotMustUnderstandUnknownObjects(
            f_pXmrLicense->containerOuter.containerPlaybackPolicies.pUnknownObjects );
    }
    if( f_pXmrLicense->containerOuter.containerPlaylistBurnPolicies.fValid )
    {
        _XMR_FilterNotMustUnderstandUnknownObjects(
            f_pXmrLicense->containerOuter.containerPlaylistBurnPolicies.pUnknownObjects );
    }

    f_pXmrLicense->containerOuter.containerKeys.ECCKey.fValid = FALSE;
    f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.fValid = FALSE;

    /*
    ** The following V2 objects are must understand and are therefore NOT filtered by this function.
    **
    ** f_pXmrLicense->containerOuter.containerGlobalPolicies.DomainID.fValid = FALSE;
    ** f_pXmrLicense->containerOuter.containerPlaybackPolicies.containerExplicitDigitalAudioProtection.fValid = FALSE;
    */

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XMR_FinishLicense
**
** Synopsis :   This method takes all the information in the builder context,
**              figures out the appropriate objects and containers to be added
**              to the license, and builds the appropriate license. This method
**              will also verify that all the mandatory objects have been added
**              to the license, before this method is called.
**
** Arguments :  f_pcontextBuilder -
**              f_pvSignatureContext  - Context needed by caller to retrieve
**                                    appropriate integrity key
**              f_eSignature      - Signature type that the callback function will perform
**              f_pfnSignature    - Callback function to create signature using
**                                  Integrity key. Integrity key is not passed to
**                                  this function for security reasons.
**              f_pbLicense       - Buffer for license
**              f_pcbLicense      - Size of license buffer. If not sufficient,
**                  the method returns an error and sets this to the required
**                  length
**
** Returns :    DRM_E_BUFFERTOOSMALL, DRM_E_XMR_REQUIRED_OBJECT_MISSING
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMR_FinishLicense (
    __in_ecount( 1 ) const              DRM_XMR_BUILDER_CONTEXT *f_pcontextBuilder,
    __in_ecount_opt( 1 ) const          DRM_VOID                *f_pvSignatureContext,
    __in                                DRM_XMR_CB_SIGNATURE     f_pfnSignature,
    __inout_bcount_part_opt( *f_pcbLicense, *f_pcbLicense )
                                        DRM_BYTE                *f_pbLicense,
    __inout_ecount( 1 )                 DRM_DWORD               *f_pcbLicense )
{
    DRM_BYTE    *pbBuffer          = f_pbLicense;
    DRM_DWORD    cbLicense         = 0;
    DRM_WORD     cbSignature       = XMR_AES_OMAC_SIGNATURE_LENGTH;
    DRM_DWORD    cbSignatureObject = 0;
    DRM_DWORD    iLicense          = 0;
    DRM_RESULT   dr                = DRM_SUCCESS;
    DRM_WORD     wObjectType       = XMR_OBJECT_TYPE_INVALID;
    DRM_DWORD    eSignature        = XMR_SIGNATURE_TYPE_AES_128_OMAC;

    const DRM_XMR_BUILDER_CONTEXT_INTERNAL    *pcontextBuilder =
                            ( const DRM_XMR_BUILDER_CONTEXT_INTERNAL * ) f_pcontextBuilder;

    ChkArg( pcontextBuilder  != NULL );
    ChkArg( f_pfnSignature   != NULL );
    ChkArg( f_pcbLicense     != NULL );
    ChkArg( pcontextBuilder->rgpObjectNodes != NULL );
    AssertChkArg(
        ( pcontextBuilder->dwVersion == XMR_VERSION_1 )
     || ( pcontextBuilder->dwVersion == XMR_VERSION_3 ) );

    /*
    **  Ensure that required objects were added
    */
    for ( wObjectType =  XMR_OBJECT_TYPE_INVALID;
          wObjectType <= XMR_OBJECT_TYPE_MAXIMUM_DEFINED;
          wObjectType++ )
    {
        /*
        **  The logic here makes exception for XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT
        **  because Silverlight licenses don't require this object.
        */
        if (   pcontextBuilder->rgpObjectNodes[wObjectType] == NULL
            && !g_xmrBuilderDefines[wObjectType].fIsOptional
            && wObjectType != XMR_OBJECT_TYPE_SIGNATURE_OBJECT
            && wObjectType != XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT )
        {
            ChkDR( DRM_E_XMR_REQUIRED_OBJECT_MISSING );
        }
    }

    /* Special case Minimum Environment / Security Level: At least one must be specified but not both */
    if( ( pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT] == NULL )
     == ( pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_SECURITY_LEVEL]             == NULL ) )
    {
        ChkDR( DRM_E_XMR_REQUIRED_OBJECT_MISSING );
    }

    /* RIV 2 object is required for XMR V3 licenses */
    if (   pcontextBuilder->dwVersion == XMR_VERSION_3
        && pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT]  == NULL)
    {
        ChkDR( DRM_E_XMR_REQUIRED_OBJECT_MISSING );
    }

    cbLicense = XMR_HEADER_LENGTH
              + pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_OUTER_CONTAINER]->cbLength;

    if ( *f_pcbLicense < cbLicense || f_pbLicense == NULL )
    {
        *f_pcbLicense = cbLicense;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Serialize the Header */
    DWORD_TO_NETWORKBYTES( pbBuffer, iLicense, XMR_MAGIC_CONSTANT );
    iLicense += sizeof( DRM_DWORD );

    DWORD_TO_NETWORKBYTES( pbBuffer, iLicense, pcontextBuilder->dwVersion );
    iLicense += sizeof( DRM_DWORD );

    ChkBOOL( iLicense + SIZEOF( DRM_ID ) <= cbLicense, DRM_E_INVALIDARG );

    DRM_BYT_CopyBytes( pbBuffer,
                       iLicense,
                       pcontextBuilder->pidRights->rgb,
                       0,
                       SIZEOF( DRM_ID ) );
    iLicense += SIZEOF( DRM_ID );

    cbSignatureObject = cbSignature + XMR_BASE_OBJECT_LENGTH + 2 * SIZEOF( DRM_WORD );

    /* Serialize the Outer Container */
    ChkBOOL( XMR_BASE_OBJECT_LENGTH + iLicense <= cbLicense, DRM_E_BUFFERTOOSMALL );

    DRM_XMR_Serialize_BaseObject( XMR_OBJECT_TYPE_OUTER_CONTAINER,
                                  pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_OUTER_CONTAINER]->cbLength,
                                  pbBuffer,
                                  &iLicense );
    ChkDR( DRM_XMR_Serialize_Container( pcontextBuilder->rgpObjectNodes[XMR_OBJECT_TYPE_OUTER_CONTAINER]->pvObject,
                                        pbBuffer,
                                        cbLicense,
                                        &iLicense ) );

    ChkBOOL( iLicense + 6 * SIZEOF( DRM_WORD ) <= cbLicense, DRM_E_BUFFERTOOSMALL );

    /* Serialize the signature object */
    WORD_TO_NETWORKBYTES( pbBuffer, iLicense, XMR_FLAGS_MUST_UNDERSTAND );
    iLicense += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES( pbBuffer, iLicense, XMR_OBJECT_TYPE_SIGNATURE_OBJECT );
    iLicense += SIZEOF( DRM_WORD );

    DWORD_TO_NETWORKBYTES( pbBuffer, iLicense, cbSignatureObject );
    iLicense += SIZEOF( DRM_DWORD );

    WORD_TO_NETWORKBYTES( pbBuffer, iLicense, ( DRM_WORD )eSignature );
    iLicense += SIZEOF( DRM_WORD );

    WORD_TO_NETWORKBYTES( pbBuffer, iLicense, cbSignature );
    iLicense += SIZEOF( DRM_WORD );

    /* Create signature */
    ChkDR( f_pfnSignature( f_pvSignatureContext,
                           f_pbLicense,
                           cbLicense - cbSignatureObject,
                           pbBuffer,
                           *f_pcbLicense,
                           iLicense ) );

    *f_pcbLicense = cbLicense;

ErrorExit:

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
