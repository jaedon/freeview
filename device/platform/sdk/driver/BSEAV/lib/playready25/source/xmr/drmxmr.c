/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMR_C
#include <drmcontextsizes.h>
#include <drmxmr.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

const DRM_EXPORT_VAR DRM_DISCARDABLE XMR_COMMON_INTERNAL_DEFINES g_xmrCommonDefines[XMR_OBJECT_TYPE_MAXIMUM_DEFINED+1] =
{
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_INVALID */
    { TRUE,  XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_CONTAINER                                       },    /* XMR_OBJECT_TYPE_OUTER_CONTAINER */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_CONTAINER                                       },    /* XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_UPLINK_KID_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_CONTAINER                                       },    /* XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER,   XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_CONTAINER                                       },    /* XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_SIGNATURE_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_SETTINGS_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_PRIORITY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_EXPIRATION_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_ISSUEDATE_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_METERING_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_EXCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_PLAYBACK_INCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER,                               XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_COPYCOUNT_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER,                               XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_COPY_EXCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_COPY_INCLUSION_LIST_OBJECT (Shipped in Emerald Beta 1)*/
    { FALSE, XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER,                 XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_SOURCEID_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_EXPLICIT_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER (Shipped in Emerald Beta 1)*/
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_UNCOMPRESSED_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT (Shipped in Emerald Beta 1)*/
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_REVOCATION_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_REVOCATION_CONTAINER,                                XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_REVOCATION_CONTAINER,                                XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_USERID_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER ,                            XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_CONTAINER                                       },    /* XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER,  XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_SECURITY_LEVEL */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER */
    { TRUE,  XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER,                             XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER,                              XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,                              XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER,                            XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* XMR_OBJECT_TYPE_READ_POLICY_CONTAINER */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_42 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_43 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_44 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_45 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_46 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_47 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_48 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_49 */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4A */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4B */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4C */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4D */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4E */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER | XMR_FLAGS_ALLOW_EXTERNAL_PARSE                                  },    /* reserved for: XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4F */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_AUX_KEY_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER,                              XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_UPLINKX_OBJECT */
    { FALSE, XMR_OBJECT_TYPE_INVALID,                                             XMR_FLAGS_NONE                                                                        },    /* Deprecated: XMR_OBJECT_TYPE_INVALID_RESERVED_53 (0x0053) was documented in PlayReady 2.0 CHM for Content Boundary Identification Object */
    { FALSE, XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,                           XMR_FLAGS_MUST_UNDERSTAND | XMR_FLAGS_ALLOW_EXTERNAL_PARSE | XMR_FLAGS_BEST_EFFORT    },    /* XMR_OBJECT_TYPE_APPLICATION_ID_LIST */
    { FALSE, XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER,                             XMR_FLAGS_MUST_UNDERSTAND                                                             },    /* XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION */
    { TRUE,  XMR_OBJECT_TYPE_OUTER_CONTAINER,                                     XMR_FLAGS_CONTAINER                                                                   },    /* XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER */
    { FALSE, XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER,                                XMR_FLAGS_NONE                                                                        },    /* XMR_OBJECT_TYPE_ND_TX_AUTH_OBJECT */
    /*
    ** NOTE:  When adding new objects or containers to XMR that have the Must Understand flag set, you have to review
    **        whether that object is required to be understood by WMDRM-ND.
    **        1. If it is required to be understood by WMDRM-ND, then it can be a backward compatability issue and you need to address it.
    **        2. Else you need to do one of the following:
    **           A. If it can be safely removed from the WMDRM-ND license, then filter it appropriately in the PC codebase.
    **              For example, the Metering restriction is removed because the WMDRM-ND Transmitter does metering on the source license.
    **           B. Else if it is *not* XMR_FLAGS_ALLOW_EXTERNAL_PARSE, then the PC codebase should explictly block conversion of
    **              such a license to WMDRM-ND.  Otherwise, the UX for WMDRM-ND streaming is less than ideal as it will fail
    **              processing on the WMDRM-ND receiver instead of on the WMDRM-ND transmitter.
    **           C. Else the PC codebase will already implicitly block conversion of such a license to WMDRM-ND and no action is required.
    **
    ** NOTE:  When adding new objects or containers to XMR that have the Must Understand flag set, you have to review
    **        whether that object is required to be understood by PlayReady-ND (PRND).
    **        Follow a similar algorithm to the above, but apply changes as needed to any codebase that supports a Transmitter.
    **        [When this note was written on 2012/06/27, only the PK supported a Transmitter.]
    */
};

EXIT_PK_NAMESPACE_CODE;
