/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_core.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 10/4/12 10:48a $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_core.h $
 * 
 * 10   10/4/12 10:48a hugost
 * SW7425-4032: Add custom data support to challenge generation
 * 
 * 9   8/15/12 12:30p hugost
 * SW7425-3573: Moved Drm_Initialize() call to bdrm_internal_create
 * 
 * 8   7/17/12 4:13p piyushg
 * SW7425-3281: Add playready kernel mode support and nexus drm
 * implementation.
 * 
 * 7   6/5/12 3:02p hugost
 * SW7425-3169: Modified playready to use new version of common crypto
 * library
 * 
 * 6   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 5   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef __DRMCORE_H__
#define __DRMCORE_H__

#include "basf_parser.h"
#include "basf_stream.h"
#include "bmp4_fragment_demux.h"
#include "nexus_memory.h"

#ifdef __KERNEL__
#include <generated/autoconf.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <drmcommon.h>
#else
#include <time.h>
#endif

#if NEXUS_DRM_BUILD
#include "nexus_dma.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define BDRM_MAX_DTTIME     (0x04)  /* max num of datetimes in a timed license */
#define BDRM_MAX_COUNTS     (0x04)  /* max num of counts in a count-limited license */

#define MAX_SAMPLE_ENC_BOX       (5)     

#define BDRM_MAX_INCLUSION_GUIDS     (20)  /*  Must match the value of DRM_MAX_INCLUSION_GUIDS in the playready SDK */
#define BDRM_MAX_LICENSE_CHAIN_DEPTH (2)   /* Must match the value of DRM_MAX_LICENSE_CHAIN_DEPTH in the playready SDK */

/* license states as sent back to the callback function
*/
typedef enum bdrm_license_state {
    bdrm_lic_pending        = (0),    /* license is about to be acquired/loaded */
    bdrm_lic_transferring   = (1),    /* license is currently being transferred from a remote location */
    bdrm_lic_localstore     = (2),    /* a valid license was found in the local store */
    bdrm_lic_acquired       = (3),    /* a valid license was loaded from the inet */ 
    bdrm_lic_faillocal      = (4),    /* could not load a license, locally from the hds */
    bdrm_lic_failtransfer   = (5),    /* could not get a license response for the challenge */
    bdrm_lic_expired        = (6),    /* license was found but is expired */
    bdrm_lic_failcount      = (7),    /* license was found but is played too many times */
    bdrm_lic_stored         = (8),    /* license was stored in the local hds */ 
    bdrm_lic_fail_no_url    = (9)     /* could not generate a challenge, no url was found in the stream header to send challenge to. */
} bdrm_license_state;


/* http status, sent in sync with the license states
*/
typedef enum bdrm_http_state {
    bdrm_http_status_failed_getpost          = (-7),  /* http get/post failure */
    bdrm_http_status_failed_connect          = (-6),  /* connection failure */
    bdrm_http_status_failed_internal         = (-5),  /* internal failure */
    bdrm_http_status_failed_response_read    = (-4),  /* could not read response */
    bdrm_http_status_failed_response_code    = (-3),  /* response code was not ok */
    bdrm_http_status_failed_no_license       = (-2),  /* response was ok but no license */
    bdrm_http_status_failed_generic          = (-1),  /* generic failure */
    bdrm_http_status_ok                      = (0),   /* response had a license */
    bdrm_http_status_undefined               = (1)    /* http was not involved */
} bdrm_http_state;

/* api func call return status
*/
typedef enum bdrm_err {
    bdrm_err_ok   = (0),                   /* successfull call */
    bdrm_err_fail = (1),                   /* call failed */
    bdrm_err_buffer_size  = (0x02),        /* buffer too small */
    bdrm_err_notfound,                     /* license not found */
    bdrm_err_license_expired               /* license expired */
} bdrm_err;

/* signifies drm encrypted or not
*/
typedef enum bdrm_encr_state {
    bdrm_encr_none     = (0),                /* no encryption */
    bdrm_encr_wmdrm    = (1),                /* wmdrm encrypted asf */
    bdrm_encr_aes_ctr  = (2),                /* AES CTR encrypted stream */
    bdrm_encr_aes_cbc  = (3),                /* AES CBC encrypted stream */
    bdrm_encr_max      = (4)
} bdrm_encr_state;

/* signifies drm encrypted or not
*/
typedef enum bdrm_hdr_state {
    bdrm_hdr_read_ok    = (0),             /* drm encrypted asf */
    bdrm_hdr_incomplete = (1),             /* not ready yet */
    bdrm_hdr_none       = (2)              /* no drm headers */
} bdrm_hdr_state;

/* forward decls to opaque types
*/
typedef struct bdrm_cfg* bdrm_cfg_t;        /* configuration handle */
typedef struct bdrm_context* bdrm_t;        /* drm context handle */
typedef struct bdrm_license* bdrm_license_t;/* drm license info handle */
typedef int bdrm_crypto_status;             /* headers read state */

/* license types enumeration
*/
typedef enum bdrm_license_type {
    bdrm_lictype_from,          /* license lower-bounded in time */
    bdrm_lictype_until,         /* license upper-bounded in time */
    bdrm_lictype_fromuntil,     /* license lower and upper bounded in time */
    bdrm_lictype_count,         /* license limited to number of playbacks */
    bdrm_lictype_unlimited,     /* license has unlimited playback rights */
    bdrm_lictype_noright,       /* license grants no rights */
    bdrm_lictype_unknown        /* license unknown state */
} bdrm_license_type;

/* aggregated license and http state
*/
typedef struct bdrm_acq_state {             
    bdrm_http_state http_state;         /* aggregated http state */                         
    bdrm_license_state license_state;   /* aggregated license state */
} bdrm_acq_state;

/* license info onbject, as sent back to the callback function
*/
typedef struct bdrm_license_info {
    uint32_t stream_id;                 /* license stream id */
    bdrm_license_type type;             /* license type */
    struct tm times[BDRM_MAX_DTTIME];   /* time(s) when the license is set to expire */
    uint8_t times_count;                /* counts number of times in array */
    uint8_t plays[BDRM_MAX_COUNTS];     /* how many times can we play this */ 
    uint8_t plays_count;                /* counts number of plays in array */ 
    uint32_t transferred;               /* bytes transferred */
    uint32_t total;                     /* bytes total */
} bdrm_license_info;

/* license challenge, as sent to the grabber handler
*/
typedef struct bdrm_license_challenge { 
    uint32_t len;                       /* bytes total */
    char* data;                         /* contents */
    char* url;                          /* url to send the challenge to . Could be null. Some streams don't have an URL tag in their header, like some Netflix 
                                           streams for example. In that case, a challenge is generated anyway. But it is assumed that the application knows where to sent it to */
    uint8_t non_quiet;                  /* non-quiet license acquisition */
    uint32_t app_security;              /* application security level */
} bdrm_license_challenge;   

/* license response blob, as received from the server
*/
typedef struct bdrm_license_response {  
    uint32_t len;                       /* bytes total */
    unsigned char* data;                /* contents */
    unsigned char* resp_start;          /* start of response within data */
} bdrm_license_response;

#ifdef PRDY_ROBUSTNESS
/* clock challenge
*/
typedef struct bdrm_clk_challenge { 
    uint32_t url_len;                   /* length of url */
    uint32_t ch_len;             /* length of challenge */    
    unsigned char* ch_data;      /* contents */
    char* url;                          /* url to send the challenge to */
} bdrm_clk_challenge;   

/* proceess clock response blob, as received from the server
*/
typedef struct bdrm_clk_response {  
    unsigned long len;        /* bytes total */
    unsigned char* data;      /* contents */
    unsigned long result;     /* result */
} bdrm_clk_response;


/* get secure clock structure
*/
typedef struct bdrm_clk_status {  
    unsigned char* securetime;      /* contents */
    unsigned long len;        /* bytes total */    
    unsigned long* flags;     /* Pointer to flag indicating the state of the secure clock */
} bdrm_clk_status;
#endif

/* fw decl to drm decryptor instance
*/
struct bdrm_decryptor;

/* func that gets called when local license state changes
*/
typedef bdrm_err(*fn_license_callback)(bdrm_acq_state* lic_acq_state, 
    bdrm_license_t license, const void* ctx);

/* func that gets called when the license is not found locally.
*/
typedef bdrm_err(*fn_license_extern)(const bdrm_license_challenge* lic_challenge, 
    bdrm_license_response* lic_response, const void* ctx);

typedef enum bdrm_policy_type{
    PLAY_OPL = 0x1,
    COPY_OPL = 0x2,
    INCLUSION_LIST = 0x3,
    EXTENDED_RESTRICTION_CONDITION = 0x4,
    EXTENDED_RESTRICTION_ACTION = 0x5,
    EXTENDED_RESTRICTION_QUERY = 0x6,
    SECURE_STATE_TOKEN_RESOLVE = 0x7
}bdrm_policy_type_e;


typedef struct bdvd_guid_s {
    uint32_t  Data1;
    uint16_t  Data2;
    uint16_t  Data3;
    uint8_t   Data4 [8];
} bdrm_guid_t;


typedef struct bdvd_video_out_protection_s{
    bdrm_guid_t guidId;
    uint8_t     bConfigData;
} bdrm_video_out_protection_t;

typedef struct bdrm_opl_play_s{

    uint32_t i_compressedDigitalVideo;    /* Mimimum output protection level to play compressed digital video */
    uint32_t i_uncompressedDigitalVideo;  /* Mimimum output protection level to play uncompressed digital video */
    uint32_t i_analogVideo;               /* Mimimum output protection level to play analog video */
    uint32_t i_compressedDigitalAudio;    /* Mimimum output protection level to play compressed digital audio */
    uint32_t i_uncompressedDigitalAudio;  /* Mimimum output protection level to play uncompressed digital video */

    /*oplIdReserved, refer to MS porting kit documentation on DRM_PLAY_OPL for more details */
    uint32_t     i_resv_cIds;                 /* Number of ids*/ 
    bdrm_guid_t *i_resv_rgIds;                /* Output protection level id reserved guid */

    /* Video output protection ids. */
    uint32_t                     i_vop_cEntries; /* Number of video output protections ids. */
    bdrm_video_out_protection_t *i_vop;          /* Pointer to array of video output protections ids. */

}bdrm_opl_play_t;


typedef struct bdrm_opl_copy_s
{
    uint32_t     i_minimumCopyLevel;              /* Minimum copy level */
    uint32_t     i_includes_cIds;                 /* Number of inclusions ids*/ 
    bdrm_guid_t *i_includes_rgIds;                /* Guid of inclusions on output protections levels */
    uint32_t     i_excludes_cIds;                 /* Number of restriction ids*/ 
    bdrm_guid_t *i_excludes_rgIds;                /* Guid of restrictons on output protections levels */

}bdrm_opl_copy_t;


typedef uint32_t bdrm_bool_t;


typedef struct bdrm_xmr_dword_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwValue;
} bdrm_xmr_dword_t;


typedef struct bdrm_xmr_word_s
{
    bdrm_bool_t  fValid;
    uint16_t     wValue;
} bdrm_xmr_word_t;

typedef struct bdrm_xmr_dword_versioned_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwVersion;
    uint32_t     dwValue;
} bdrm_xmr_dword_versioned_t;

typedef struct bdrm_xmr_guid_s
{
    bdrm_bool_t  fValid;
    uint8_t     *pguidBuffer;
    uint32_t     iGuid;
} bdrm_xmr_guid_t;

typedef struct bdrm_xmr_guidlist_s
{
    bdrm_bool_t  fValid;
    uint32_t     cGUIDs;
    uint8_t     *pguidBuffer;
    uint32_t     iGuids;
} bdrm_xmr_guidlist_t;

typedef struct bdrm_xmr_bytearray_s
{
    bdrm_bool_t  fValid;
    uint32_t     cbData;
    uint8_t     *pbDataBuffer;
    uint32_t     iData;
} bdrm_xmr_bytearray_t;

typedef struct bdrm_xmr_empty_s
{
    bdrm_bool_t  fValid;
} bdrm_xmr_empty_t;

typedef struct brdm_xmr_expiration_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwBeginDate;
    uint32_t     dwEndDate;
} brdm_xmr_expiration_t;


typedef struct bdrm_xmr_minimum_environment_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwVersion;
    uint16_t     wMinimumSecurityLevel;
    uint32_t     dwMinimumAppRevocationListVersion;
    uint32_t     dwMinimumDeviceRevocationListVersion;
} bdrm_xmr_minimum_environment_t;

typedef bdrm_xmr_dword_t           bdrm_xmr_issuedate_t;
typedef bdrm_xmr_dword_t           bdrm_xmr_grace_period_t;
typedef bdrm_xmr_guid_t            bdrm_xmr_metering_t;
typedef bdrm_xmr_dword_versioned_t bdrm_xmr_expiration_after_firstuse_t;
typedef bdrm_xmr_dword_t           bdrm_xmr_expiration_after_firststore_t;
typedef bdrm_xmr_guidlist_t        bdrm_xmr_inclusion_list_t;

typedef bdrm_xmr_bytearray_t           bdrm_xmr_serial_number_t;
typedef bdrm_xmr_word_t                bdrm_xmr_rights_t;
typedef bdrm_xmr_dword_versioned_t     bdrm_xmr_revocation_informaiton_version_t;
typedef bdrm_xmr_dword_t               bdrm_xmr_priority_t;
typedef bdrm_xmr_dword_t               bdrm_xmr_sourceid_t;
typedef bdrm_xmr_empty_t               bdrm_xmr_restricted_sourceid_t;
typedef bdrm_xmr_word_t                bdrm_xmr_embedding_behavior_t;

typedef struct bdrm_xmr_unknown_obj_s
{
    bdrm_bool_t                    fValid;
    uint16_t                       wType;
    uint16_t                       wFlags;
    uint8_t                       *pbBuffer;
    uint32_t                       ibData;
    uint32_t                       cbData;
    struct bdrm_xmr_unknown_obj_s *pNext;
} bdrm_xmr_unknown_obj_t;

typedef struct bdrm_xmr_unknown_container_s
{
   bdrm_bool_t                                fValid;
   uint16_t                                   wType;
   uint16_t                                   wFlags;
   bdrm_xmr_unknown_obj_t                    *pObject;
   struct bdrm_xmr_unknown_container_s       *pUnkChildcontainer;/* Linked list */
   struct bdrm_xmr_unknown_container_s       *pNext; /* Linked list */
} bdrm_xmr_unknown_container_t;


typedef struct bdrm_xmr_domain_id_s
{
    bdrm_bool_t  fValid;
    uint8_t     *pbAccountID;
    uint32_t     ibAccountID;
    uint32_t     cbAccountID;
    uint32_t     dwRevision;
} bdrm_xmr_domain_id_t;

typedef struct bdrm_xmr_policy_metadata_obj_s
{
    bdrm_bool_t  fValid;
    uint8_t     *pMetadataTypeGuidBuffer;
    uint32_t     iMetadataTypeGuid;
    uint32_t     cbPolicyData;
    uint8_t     *pbPolicyDataBuffer;
    uint32_t     iPolicyData;
} bdrm_xmr_policy_metadata_obj_t;

typedef struct bdrm_xmr_policy_metadata_list_s
{
    bdrm_xmr_policy_metadata_obj_t          MetadataObject;
    struct bdrm_xmr_policy_metadata_list_s *pNext;
} bdrm_xmr_policy_metadata_list_t;

typedef struct bdrm_xmr_policy_metadata_s
{
    bdrm_bool_t                      fValid;
    uint32_t                         cPolicyMetadataObjects;
    bdrm_xmr_policy_metadata_list_t *plistPolicyMetadataObjects;
} bdrm_xmr_policy_metadata_t;

typedef struct bdrm_xmr_global_requirements_s
{
    bdrm_bool_t                                fValid;
    bdrm_xmr_minimum_environment_t             MinimumEnvironment;
    bdrm_xmr_serial_number_t                   SerialNumber;
    bdrm_xmr_rights_t                          Rights;
    bdrm_xmr_priority_t                        Priority;
    bdrm_xmr_sourceid_t                        SourceID;
    bdrm_xmr_restricted_sourceid_t             RestrictedSourceID;
    brdm_xmr_expiration_t                      Expiration;
    bdrm_xmr_issuedate_t                       IssueDate;
    bdrm_xmr_grace_period_t                    GracePeriod;
    bdrm_xmr_metering_t                        Metering;
    bdrm_xmr_expiration_after_firstuse_t       ExpirationAfterUse;
    bdrm_xmr_expiration_after_firststore_t     ExpirationAfterStore;
    bdrm_xmr_inclusion_list_t                  InclusionList;
    bdrm_xmr_revocation_informaiton_version_t  RevocationInformationVersion;
    bdrm_xmr_domain_id_t                       DomainID;
    bdrm_xmr_embedding_behavior_t              EmbeddingBehavior;
    bdrm_xmr_unknown_obj_t                    *pUnknownObjects;
    bdrm_xmr_policy_metadata_t                 PolicyMetadata;
} bdrm_xmr_global_requirements_t;


typedef bdrm_xmr_dword_t bdrm_xmr_playcount_t;

typedef struct bdrm_xmr_output_config_s
{
    bdrm_bool_t  fValid;
    uint8_t     *pguidBuffer;
    uint32_t     iGuid;
    uint32_t     cbConfigData;
    uint8_t     *pbConfigDataBuffer;
    uint32_t     iConfigData;
} bdrm_xmr_output_config_t;

typedef struct bdrm_xmr_output_config_list_s
{
    bdrm_xmr_output_config_t              Config;
    struct bdrm_xmr_output_config_list_s *pNext;
} bdrm_xmr_output_config_list_t;

typedef struct bdrm_xmr_explicit_output_protection_s
{
    bdrm_bool_t  fValid;
    uint32_t     cOutputProtectionIDs;
    bdrm_xmr_output_config_list_t *plistOutputConfiguration;
} bdrm_xmr_explicit_output_protection_t;

typedef bdrm_xmr_explicit_output_protection_t bdrm_xmr_explicit_analog_video_protection_t;
typedef bdrm_xmr_explicit_output_protection_t bdrm_xmr_explicit_digital_audio_protection_t;

typedef struct bdrm_xmr_minimium_output_protection_levels_s
{
    bdrm_bool_t fValid;
    uint16_t    wCompressedDigitalVideo;
    uint16_t    wUncompressedDigitalVideo;
    uint16_t    wAnalogVideo;
    uint16_t    wCompressedDigitalAudio;
    uint16_t    wUncompressedDigitalAudio;
} bdrm_xmr_minimium_output_protection_levels_t;


typedef struct bdrm_xmr_playback_rights_s
{
    bdrm_bool_t                                   fValid;
    bdrm_xmr_playcount_t                          PlayCount;
    bdrm_xmr_minimium_output_protection_levels_t  opl;
    bdrm_xmr_explicit_analog_video_protection_t   containerExplicitAnalogVideoProtection;
    bdrm_xmr_explicit_digital_audio_protection_t  containerExplicitDigitalAudioProtection;
    bdrm_xmr_unknown_obj_t                       *pUnknownObjects;
    bdrm_xmr_unknown_container_t                  UnknownContainer;
} bdrm_xmr_playback_rights_t;

typedef struct bdrm_xmr_copy_to_pc_s
{
    bdrm_bool_t                                   fValid;
} bdrm_xmr_copy_to_pc_t;

typedef bdrm_xmr_dword_t               bdrm_xmr_move_enabler_t;
typedef bdrm_xmr_dword_versioned_t     bdrm_xmr_copy_count;
typedef bdrm_xmr_word_t                bdrm_xmr_copy_protection_level_t;

typedef struct bdrm_xmr_copy_rights_s
{
    bdrm_bool_t                        fValid;
    uint32_t                           dwVersion;
    bdrm_xmr_copy_count                CopyCount;
    bdrm_xmr_copy_protection_level_t   CopyProtectionLevel;
    bdrm_xmr_move_enabler_t            MoveEnabler;
    bdrm_xmr_unknown_obj_t            *pUnknownObjects;
    bdrm_xmr_unknown_container_t       UnknownContainer;
} bdrm_xmr_copy_rights_t;


typedef struct bdrm_xmr_playlist_burn_restrictions_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwMaxPlaylistBurnCount;
    uint32_t     dwPlaylistBurnTrackCount;
} bdrm_xmr_playlist_burn_restrictions_t;

typedef struct bdrm_xmr_playlist_burn_rights_s
{
    bdrm_bool_t                            fValid;
    bdrm_xmr_playlist_burn_restrictions_t  Restrictions;
    bdrm_xmr_unknown_obj_t                *pUnknownObjects;
} bdrm_xmr_playlist_burn_rights_t;


typedef struct bdrm_xmr_rsa_pubkey_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwExponent;
    uint16_t     cbModulus;
    uint8_t     *pbModulusBuffer;
    uint32_t     iModulus;
} bdrm_xmr_rsa_pubkey_t;

typedef bdrm_xmr_rsa_pubkey_t bdmr_xmr_device_key_t;
typedef bdrm_xmr_rsa_pubkey_t bdrm_xmr_rsa_license_granter_key_t;

typedef struct bdrm_xmr_userid_s
{
    bdrm_bool_t   fValid;
    uint32_t      cbUserID;
    uint8_t      *pbUserID;
    uint32_t      iUserID;
} bdrm_xmr_userid_t;

typedef struct bdrm_xmr_revocation_s
{
    bdrm_bool_t                         fValid;
    bdrm_xmr_rsa_license_granter_key_t  RsaLicenseGranterKey;
    bdrm_xmr_userid_t                   UserID;
} bdrm_xmr_revocation_t;

/* Structures for Key Material container */
typedef struct bdrm_xmr_content_key_s
{
    bdrm_bool_t  fValid;
    uint8_t     *pbguidKeyID;
    uint32_t     iguidKeyID;
    uint16_t     wSymmetricCipherType;
    uint16_t     wKeyEncryptionCipherType;
    uint16_t     cbEncryptedKey;
    uint8_t     *pbEncryptedKeyBuffer;
    uint32_t     iEncryptedKey;
} bdrm_xmr_content_key_t;

typedef struct bdrm_xmr_optimized_content_key_s
{
    bdrm_bool_t    fValid;
    uint16_t       wKeyEncryptionCipherType;
    uint16_t       cbEncryptedKey;
    uint8_t       *pbEncryptedKeyBuffer;
    uint32_t       iEncryptedKey;
} bdrm_xmr_optimized_content_key_t;

typedef struct bdrm_xmr_ecc_device_key_s
{
    bdrm_bool_t  fValid;
    uint16_t     wEccCurveType;
    uint32_t     iKeyData;
    uint16_t     cbKeyData;
    uint8_t     *pbKeyData;
} bdrm_xmr_ecc_device_key_t;


typedef struct bdrm_xmr_uplink_kid_s
{
    bdrm_bool_t  fValid;
    uint32_t     dwVersion;
    uint8_t     *pbguidUplinkKID;
    uint32_t     iguidUplinkKID;
    uint16_t     cbChainedCheckSum;
    uint8_t     *pbChainedCheckSumBuffer;
    uint32_t     iChainedCheckSum;
    uint16_t     wChecksumType;
} bdrm_xmr_uplink_kid_t;

typedef struct bdrm_xmr_key_material_s
{
    bdrm_bool_t                        fValid;
    bdrm_xmr_content_key_t            ContentKey;
    bdrm_xmr_optimized_content_key_t  OptimizedContentKey;
    bdmr_xmr_device_key_t             DeviceKey;
    bdrm_xmr_ecc_device_key_t         ECCKey;
    bdrm_xmr_uplink_kid_t             UplinkKid;
} bdrm_xmr_key_material_t;

typedef struct bdrm_xmr_signature_s
{
    bdrm_bool_t  fValid;
    uint16_t     wType;
    uint8_t     *pbSignatureBuffer;
    uint32_t     iSignature;
    uint16_t     cbSignature;
} bdrm_xmr_signature_t;

typedef bdrm_xmr_dword_t bdrm_xmr_generation_number_t;

typedef struct bdrm_xmr_outer_container_s
{
    bdrm_bool_t                       fValid;
    bdrm_xmr_global_requirements_t    containerGlobalPolicies;
    bdrm_xmr_playback_rights_t        containerPlaybackPolicies;
    bdrm_xmr_copy_rights_t            containerCopyPolicies;
    bdrm_xmr_copy_to_pc_t             containerCopyToPCPolicies;
    bdrm_xmr_playlist_burn_rights_t   containerPlaylistBurnPolicies;
    bdrm_xmr_generation_number_t      generationNumber;
    bdrm_xmr_unknown_container_t      containerUnknown;
    bdrm_xmr_revocation_t             containerRevocation;
    bdrm_xmr_key_material_t           containerKeys;
    bdrm_xmr_signature_t              signature;
} bdrm_xmr_outer_container_t;

typedef struct bdrm_xmr_license_s
{
    uint8_t                    *pbRightsIdBuffer;
    uint32_t                    iRightsId;
    uint32_t                    dwVersion;
    uint8_t                    *pbSignedDataBuffer;
    uint32_t                    iSignedData;
    uint32_t                    cbSignedData;
    bdrm_xmr_outer_container_t  containerOuter;
    uint8_t                    *pbXMRLic;
    uint32_t                    cbXMRLic;
} bdrm_xmr_license_t;


typedef struct bdrm_ext_restrict_callback_info_s
{
    uint16_t                             wRightID;
    bdrm_xmr_unknown_obj_t              *pRestriction;
    bdrm_xmr_license_t                  *pXMRLicense;
    void *                               pContextSST;
} bdrm_ext_restrict_callback_info_t;


typedef struct bdrm_inclusion_list_s
{
    bdrm_guid_t  rgInclusionList       [BDRM_MAX_INCLUSION_GUIDS];
    bdrm_bool_t  rgfInclusionListValid [BDRM_MAX_INCLUSION_GUIDS][BDRM_MAX_LICENSE_CHAIN_DEPTH];
    uint32_t     dwChainDepth;
}bdrm_inclusion_list_t;


typedef unsigned short PRDY_DRM_WCHAR;

typedef struct bdmr_const_string_s
{
    const PRDY_DRM_WCHAR *pwszString;
    uint32_t       cchString;
} bdrm_const_string_t;

typedef struct bdrm_byteblob_s
{
    uint8_t *pbBlob;
    uint32_t cbBlob;
} bdrm_byteblob_t;

typedef struct bdrm_token_s
{
    uint32_t TokenType;
    union
    {
        uint64_t             u64DateTime;
        long                 lValue;
        bdrm_const_string_t  stringValue;
        uint32_t             fnValue;
        bdrm_byteblob_t      byteValue;
    } val;
}bdrm_token_t;

typedef struct bdrm_secure_state_token_resolve_data_s
{
    bdrm_const_string_t *pdstrAttributeName;
    bdrm_token_t        *pOnDisk;
    bdrm_token_t        *pOriginallyIntendedValue;
    bdrm_bool_t          fNewAttribute;
    uint16_t             wRightID;
    uint16_t             wRestrictionID;
   
}bdrm_secure_state_token_resolve_data_t;

typedef struct bdrm_policy_s
{
    bdrm_policy_type_e type;
    union{
        bdrm_opl_play_t play;
        bdrm_opl_copy_t copy;
        bdrm_inclusion_list_t                   inc_list;
        bdrm_ext_restrict_callback_info_t       condition;
        bdrm_ext_restrict_callback_info_t       action;
        bdrm_ext_restrict_callback_info_t       query;
        bdrm_secure_state_token_resolve_data_t  token_res;
    }t;
}bdrm_policy_t;


/* func that gets called when the license contains output protection levels.
*/
/* Backward compatibility with drmcore.h for existing projects */
#define bdrm_opl_t       bdrm_policy_t
#define fn_opl_extern    fn_policy_extern
#define bdrm_opl_type_e  bdrm_policy_type_e

typedef bdrm_err(*fn_policy_extern)(bdrm_policy_t *policy, const void* ctx);

/* content encryption object
*/
typedef struct basf_cencr_object {
    bdrm_hdr_state state;               /* headers state */
    basf_dword sec_dlen;                /* security data length */
    basf_byte  *sec_data;               /* security data */
    basf_dword prot_tlen;               /* protocol length */
    basf_byte  *prot_type;              /* protocol type */
    basf_dword keyid_len;               /* key id length */
    basf_byte  *keyid;                  /* key id data */
    basf_dword licurl_len;              /* license url length */
    basf_byte  *licurl;                 /* license url data */
} basf_cencr_object;

/* extended content encryption objectd
*/
typedef struct basf_xcencr_object {
    bdrm_hdr_state state;               /* headers state */
    basf_dword size;                    /* extended encr size */
    basf_byte  *data;                   /* extended encr data */
} basf_xcencr_object;

/* digital signature object
*/
typedef struct basf_digsign_object {
    bdrm_hdr_state state;               /* headers state */
    basf_dword type;                    /* digital signature type */
    basf_dword len;                     /* digital signature blob len */
    basf_byte* data;                    /* digital signature blob */
} basf_digsign_object;


/* PLAYREADY: Protection system identifier object
*/
typedef struct basf_psi_object {
    bdrm_hdr_state state;               /* headers state */
    basf_guid systemId;                 /* Defines the GUID of the Content Protection system. Microsoft PlayReady systems must use ASF_Content_Protection_System_Microsoft_PlayReady */
    basf_dword sysversion;              /* Version specific to each Content Protection system. Microsoft PlayReady must use System Version = 1*/
    basf_dword size;                    /* Specifies the size in bytes of the data field */
    basf_byte* data;                    /* The content protection data for the PlayReady Object */
} basf_psi_object;

/* holds asf header handlers. We use this to pass the context through
** the callback.
*/                                    
typedef struct basf_drm_object_handler {
    basf_parser_object_handler handler;        /* standard asf handler */
    bdrm_t ctx;                         /* back-ptr to drm context */
} basf_drm_object_handler;

/* holds initialized/uninitialized state for object handlers
*/
typedef enum basf_drm_handlers_state {
    basf_drm_handlers_init      = 0,    /* handlers were initialized */
    basf_drm_handlers_uninit    = 1     /* handlers were not initialized */
} basf_drm_handlers_state;

/* transport type. could be either external or default ('http')
*/
typedef enum bdrm_transport_type {
    bdrm_default_transport      = 0,    /* default transport */
    bdrm_external_transport     = 1     /* external, user provided transport */
} bdrm_transport_type;

/* holds drm sub-header handlers
*/
typedef struct basf_drm_handlers {
    basf_drm_handlers_state state;
    basf_drm_object_handler digsign_handler; /* digital signature handler */         
    basf_drm_object_handler xencr_handler;   /* extended content encryption handler */      
    basf_drm_object_handler encr_handler;    /* content encryption handler */ 
    basf_drm_object_handler psi_handler;     /*PLAYREADY : Protection system Identifier */
} basf_drm_handlers;

/* Container supported by the DRM module */
typedef enum bdrm_cnt_type {
    bdrm_cnt_asf = (0),
    bdrm_cnt_mp4
}bdrm_cnt_type_e;

/* holds init params for drm
*/
typedef struct bdrm_cfg {
    unsigned int magic;                 /* magic number */
    basf_parser_t asf;                  /* asf context */
    fn_license_callback lic_callback;   /* external status callback */
    void* lic_callback_ctx;             /* external status callback context */
    fn_license_extern lic_extern;       /* external grabber func */   
    bdrm_transport_type transport;      /* use default grabber or external function */
    void* lic_extern_ctx;               /* external grabber func context */

    fn_policy_extern opl_cback;         /* external policy callback func */
    void* opl_ctx;                      /* external policy func context */

    uint8_t quiet;                      /* do quiet license acq */
    char user_data_store_file_path[128];    /* user data store file name */
    bool ext_asf_scanning;              /* do external asf scanning */

    NEXUS_HeapHandle hHeap;
    bdrm_cnt_type_e cnt_type;           /* Container stream type*/
} bdrm_cfg;

/* func that resets container header objects */
typedef bdrm_err (*fn_reset_headers)(bdrm_t drm);
/* func that resets container drm header objects */
typedef void (*fn_destroy_drm_headers)(bdrm_t drm);
/* func that resets container drm objects */
typedef void (*fn_destroy_drm_objs)(bdrm_t drm);
/* func that is used to detect the type of encryption of the stream */
typedef bdrm_encr_state (*fn_is_drm_encrypted)(bdrm_t drm);
/* func that is used to retrieve the license server XML info */
typedef void (*fn_get_xml_info)(bdrm_t drm,  uint8_t **pbXML, uint32_t *cbXML); 


typedef struct drm_cnt_fcts{
    fn_reset_headers       reset_headers;
    fn_destroy_drm_headers destroy_headers;
    fn_destroy_drm_objs    destroy_drm_objs;
    fn_is_drm_encrypted    is_drmencrypted;
    fn_get_xml_info        get_xml_info;
}drm_cnt_fcts_t;


/* creator function for drm instance
*/
bdrm_t bdrm_open (
    const char* file,                           /* [in] file - path to the input drm content file */
    bdrm_cfg* cfg                               /* [in] cfg - drm configuration struct */
);

/* init drm, install header hooks
*/
bdrm_err bdrm_init_handlers (
    bdrm_t drm                                  /* [in] drm - drm context, obtained from a bdrm_init */
);

/* go through the drm headers, extract info
*/
bdrm_err bdrm_load_license (
    bdrm_t drm,                                 /* [in] drm - drm context */
    bdrm_license_t* license                     /* [out] lic_info - license information */
);

/* called when encr flag is found
*/
bdrm_encr_state bdrm_is_drmencrypted (
    bdrm_t drm                                  /* [in] drm - drm context */
);

bdrm_err
bdrm_cleanup_license_store(bdrm_t drm);

#ifdef PRDY_ROBUSTNESS
/* This function is called from the drm app to obtain client info which is later sent to a license server.
 * This function will allocate a buffer big enough to store the client info and return the address
 * through pdata. 
 * pdata : (OUT) Address where the client info will be stored
 * len   : (OUT) length of pdata in bytes.
 * Note that the function bdrm_clean_client_info() must be called to free the allocated buffer when it is no
 * longer needed.
 */
bdrm_err
bdrm_get_client_info(bdrm_t drm, uint8_t **pdata, uint32_t *len);

/* Cleans up the buffer allocated to hold the client info */
bdrm_err 
bdrm_clean_client_info(uint8_t *pdata);

/* Feed the license to the DRM module for storage and verification
 * plicense : Pointer to the begining of the license response (correspond to resp_start of bdrm_license_response structure
 * len : Length of the license in bytes. 
 */
bdrm_err 
bdrm_feed_pdla_license (bdrm_t drm, uint8_t *plicense, uint32_t len);

#endif

/* determines if the asf is encr. Called from the drm app to obtain a
** challenge which is later sent to a license server
*/
bdrm_err bdrm_gen_challenge (
    bdrm_t drm,                                 /* [in] drm - drm context */
    const char *pCustomData,                    /* [in] Custom data that can be sent to the server. Can be null */
    uint32_t customDataLen,                     /* Lenght of the custom data field. */
    bdrm_license_challenge* challenge,          /* [out] challenge - generated challenge to send to the server */
    bdrm_license_t license                      /* [in] license - object used for license notification */
);   

/* cleans up the components in the challenge, as they are allocated in the
** bdrm_gen_challenge func below.
*/
bdrm_err bdrm_clean_challenge (
    const bdrm_license_challenge* challenge     /* [in] challenge - to be cleaned up */
);

/* feeds the license blob obtained from the server, back to the drm
** engine for storage and verification
*/
bdrm_err bdrm_feed_license (
    bdrm_t drm,                                 /* [in] drm - drm context */
    bdrm_license_response* response,            /* [in] response - license blob response, as sent by the server */
    bdrm_license_t license                      /* [in] license - object used for license notification */
);     

/* drm close function; resource destruction happens here.
*/
bdrm_err bdrm_close (
    bdrm_t drm                                  /* [in] drm - drm context to clean up */
);                                          


bdrm_err bdrm_get_license_challenge(
    bdrm_t drm,
    bdrm_license_challenge *p_challenge
);

/* handles typical drm operations in this func. Implementors may decide
** to unwrap the contents of this func and do their own processing.
*/
bdrm_err bdrm_handle_drm (
    bdrm_t drm                                  /* [in] drm - drm context to run handler in */
);

/* verifies the crypto and manager contexts for validity
*/
bdrm_err bdrm_validate_ctx (
    bdrm_t drm                                  /* [in] drm - drm context to verify */
);

/* obtain a reference to the internal decryptor instance
*/
bdrm_err bdrm_dcrypt_instance ( 
    bdrm_t drm,                                 /* [in] drm instance*/
    struct bdrm_decryptor** dcrypt              /* [out] decryptor object */
);

/* retreives the number of active license properties
*/
uint8_t bdrm_license_get_count (                /* [ret] number of properties in one bind*/
    bdrm_license_t license                      /* [in]  license property handle*/
);

/* retreives one active license property at a time
*/
void bdrm_license_get_item (
    bdrm_license_t license,                     /* [in] license property handle */ 
    uint8_t index,                              /* [in] index into property array */
    bdrm_license_info* lic_info                 /* [out] license info structure */
);

/* updates the asf stream config 
*/
void bdrm_update_asf_stream_cfg (
    void *application_cnxt,                     /* [in] custom app context */
    unsigned stream_id,                         /* [in] asf stream id */
    basf_stream_cfg *cfg                        /* [out] stream configuration */
); 

/* Status callback func install
*/
void bdrm_install_lic_callback (bdrm_t ctx, fn_license_callback pfn_lic_callback, void* params);

/* External handling of license traffic func install
*/
void bdrm_install_lic_extern (bdrm_t ctx, fn_license_extern pfn_lic_extern, void* params);


#ifdef PRDY_ROBUSTNESS
/* Initialize oemtime module */ 
bdrm_err bdrm_oem_time_init(void);

/* UnInitialize oemtime module */ 
bdrm_err bdrm_oem_time_uninit(void);

/* Called from the drm app to obtain a clock challenge
*/
bdrm_err bdrm_gen_clk_challenge (
    bdrm_t drm,                                 /* [in] drm - drm context */
    bdrm_clk_challenge* challenge           /* [out] challenge - generated challenge to send to the server */
);   


/* Called once the challenge structure is not needed
*/
bdrm_err bdrm_clean_clk_challenge (
    bdrm_clk_challenge* challenge
);


/* Wrapper 'Free data' for OEM_Free
*/
void bdrm_free_buffer(void* ptr);

/* process the clock response
*/
bdrm_err bdrm_clk_process_response (
    bdrm_t drm,                       /* [in] drm - drm context */
    bdrm_clk_response* response      /* [in] response - license blob response, as sent by the server */
);     


/* get the clock status
*/
bdrm_err bdrm_clk_get_status (
    bdrm_t drm,                       /* [in] drm - drm context */
    bdrm_clk_status* status      /* [out] status */
);     

#if 0
/*!+!hla bdrm_erase_uname_hds_files() will not work in kernel do this op in nexus wmdrm module?*/
/* revoke the wmdrmpd keys and dev cert, dev cert template, etc.
*/
bdrm_err bdrm_wmdrm_srm_revoke(void);
#endif

#endif

#if NEXUS_DRM_BUILD

/* Store a "local" copy of a Nexus DMA channel handle.
*/
void bdrm_set_dma_handle( 
    NEXUS_DmaHandle hDmaChannel     /* [in] handle to a Nexus DMA channel */
);

/* Retrieve the Nexus DMA handle.
*/
NEXUS_DmaHandle bdrm_get_dma_handle( 
    void
);
#endif

#ifdef NEXUS_WMDRMPD_FILE_HANDLING

/* File handling commands
*/
typedef enum bdrm_file_op
{
    bdrm_file_open,
    bdrm_file_close,
    bdrm_file_read,
    bdrm_file_write,
    bdrm_file_seek,
    bdrm_file_tell,
    bdrm_file_flush
}bdrm_file_op;

/* File handling parameters
*/
typedef struct bdrm_file_params
{
    void* nexus_drm_handle;
    void* fp;
    char* filename;
    char* filemode;
    unsigned int size;
    void* pData;
    int offset;
    int origin;
}bdrm_file_params;

/* Callback function for  file handling */
typedef bdrm_err (*fn_file_ops_callback)(bdrm_file_op eCmd, bdrm_file_params* params);

/* Sets the file operation handling */
void bdrm_set_file_handle(
    fn_file_ops_callback func,
    void* nexus_drm_file_handle
);
#endif

/* TODO - start - dummy to make build happy */
bdrm_err bdrm_check_license(
    bdrm_t drm,                /* [in] drm manager context */
    bdrm_license_t* license,   /* [in] license holder */
    char* pWrmHdr,             /* [in] wrmheader data */
    unsigned int size          /* [in] wrmheader size */
    );

uint32_t bdrm_query_license_count(void);


bdrm_err bdrm_get_license(uint32_t  uiIndex,         /* [in]      */ 
                             char*     pszLicense,      /* [out]     */
                             uint32_t* pcbLicenseLen);   /* [in, out] */


bdrm_err
bdrm_get_license_state(bdrm_t drm,                          /* [in]   */
                       uint16_t* usLicHeader,               /* [in]   */
                       uint32_t  cbLicHeaderLen,            /* [in]   */
                       uint32_t* puiLicenseCategory,        /* [out]  */   
                       uint32_t* puiCurrentPlayBackCount,   /* [out]  */
                       uint32_t* puiMaxPlayBackCount,       /* [out]  */
                       struct tm* puiValidFromTime,         /* [out]  */
                       struct tm* puiValidUntilTime);        /* [out]  */ 


bdrm_err
bdrm_get_client_info(bdrm_t drm, uint8_t **pdata, uint32_t *len);

/* TODO - end - dummy to make build happy */

/* TODO - start RY: might need this? */
/* set asf handle 
*/
void bdrm_set_asf (bdrm_t drm, basf_parser_t asf);

/* TODO - end */



#ifdef __cplusplus
}
#endif

#endif /*__DRMCORE_H__*/

