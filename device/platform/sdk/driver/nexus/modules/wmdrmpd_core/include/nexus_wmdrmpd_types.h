/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_wmdrmpd_types.h $
* $brcm_Revision: 3 $
* $brcm_Date: 10/3/12 9:10a $
*
* API Description:
*   API name: wmdrmpd
*    Specific APIs related to Microsoft Windows Media DRM PD
*
* Revision History:
*
* $brcm_Log: /nexus/modules/wmdrmpd_core/include/nexus_wmdrmpd_types.h $
* 
* 3   10/3/12 9:10a erickson
* SW7425-3986: merge
* 
* SW7425-3986/1   9/24/12 3:44p hugost
* Added NEXUS_WmdrmPdAesCounterInfo structure
* 
* 2   8/23/12 3:57p jgarrett
* SW7425-3757: Merge to main branch
* 
* SW7425-3757/1   8/15/12 4:54p hugost
* Applied changes from API review. Refer to jira SW7231-913 for details.
* 
* 1   7/27/12 2:26p jgarrett
* SW7425-3281: Merge to main branch
* 
* SW7425-3281/5   7/17/12 4:44p jgarrett
* SW7425-3281: Incorporating code review feedback
* 
* SW7425-3281/4   7/13/12 3:04p piyushg
* SW7425-3281: Modifications based on nexus review.
* 
* SW7425-3281/3   7/8/12 4:54p piyushg
* SW7425-3281: Added support for playready licensce acquisition and Nexus
*  fileio
* 
* SW7425-3281/2   6/22/12 6:47a jgarrett
* SW7425-3281: Correcting attributes
* 
* SW7425-3281/1   6/20/12 2:35p piyushg
* SW7425-3281: Initial checkin of Nexus directory structure and header
*  files.
* 
***************************************************************************/
#ifndef NEXUS_WMDRMPD_TYPES_H_
#define NEXUS_WMDRMPD_TYPES_H_


#define NEXUS_WMDRMPD_GUID_LENGTH (16)   /* Lenght of Windows Media DRM GUID in bytes */
/***************************************************************************
Summary: 
GUID (Globally Unique Identifier) for Windows Media DRM
***************************************************************************/
typedef struct NEXUS_WmDrmPdGuid
{
    uint8_t   data[NEXUS_WMDRMPD_GUID_LENGTH];
} NEXUS_WmDrmPdGuid;

/***************************************************************************
Summary: 
ASF PSI (Protection System Identifier) Object for Microsoft PlayReady
***************************************************************************/
typedef struct NEXUS_WmDrmPdPsiObjectInfo
{
    NEXUS_WmDrmPdGuid systemId;     /* Defines the GUID of the Content Protection system. Microsoft PlayReady systems must use ASF_Content_Protection_System_Microsoft_PlayReady */
    uint32_t sysVersion;            /* Version specific to each Content Protection system. Microsoft PlayReady must use System Version = 1*/
} NEXUS_WmDrmPdPsiObjectInfo;

/***************************************************************************
Summary: 
ASF Digisign (Digital Signature) Object for Microsoft Windows Media DRM 10
***************************************************************************/
typedef struct NEXUS_WmDrmPdDigsignObjectInfo
{
    uint32_t       type;        /* Digital signature type */
} NEXUS_WmDrmPdDigsignObjectInfo;


/***************************************************************************
Summary: 
MP4 PSSH (Protection System Specific Header) Box for Microsoft PlayReady
***************************************************************************/
typedef struct NEXUS_WmDrmPdMp4PsshBoxInfo
{
    NEXUS_WmDrmPdGuid systemId;     /* Defines the GUID of the Content Protection system. Microsoft PlayReady systems must use SystemID=9A04F079-9840-4286-AB92-E65BE0885F95. */
} NEXUS_WmDrmPdMp4PsshBoxInfo;


/***************************************************************************
Summary: 
Encryption types supported.
 
Description: 
***************************************************************************/
typedef enum NEXUS_WmDrmPdEncryptionType
{
    NEXUS_WmDrmPdEncryptionType_eNone,                /* no encryption */             
    NEXUS_WmDrmPdEncryptionType_eWmdrm,               /* wmdrm encrypted asf */       
    NEXUS_WmDrmPdEncryptionType_eAesCtr,              /* AES CTR encrypted stream */  
    NEXUS_WmDrmPdEncryptionType_eAesCbc,              /* AES CBC encrypted stream */  
    NEXUS_WmDrmPdEncryptionType_eMax
} NEXUS_WmDrmPdEncryptionType;


#define NEXUS_WMDRMPD_KEY_ID_LENGTH (16)   /* Key identifier length in bytes */

/***************************************************************************
Summary: 
MP4 Scheme Information Box for Microsoft PlayReady
***************************************************************************/
typedef struct NEXUS_WmDrmPdMp4SchemeInfo 
{
    NEXUS_WmDrmPdEncryptionType algorithm;  /* Default algorithm used to encrypt the track.*/
    unsigned  ivSize;      /* Default size in bytes of the Initialization Vector (IV) for this track. 
                               8 -  Specifies 64-bit initialization vectors. Supported for AES-CTR.
                               16  Specifies 128-bit initialization vectors. Supported for both AES-CTR and AES-CBC. */
    uint8_t   keyId[NEXUS_WMDRMPD_KEY_ID_LENGTH];   /* Default key identifier for this track. */
}NEXUS_WmDrmPdMp4SchemeInfo;

/***************************************************************************
Summary: 
MP4 Track Encryption Box for Microsoft PlayReady.
***************************************************************************/
typedef struct NEXUS_WmDrmPdMp4TrackEncryptionBox
{
    uint8_t version;
    uint32_t flags;
    NEXUS_WmDrmPdMp4SchemeInfo info;
} NEXUS_WmDrmPdMp4TrackEncryptionBox;

/***************************************************************************
Summary: 
MP4 Scheme Type Box for Microsoft PlayReady.
***************************************************************************/
typedef struct NEXUS_WmDrmPdMp4SchemeTypeBox
{
    uint8_t  version;
    uint32_t flags;
    uint32_t schemeType;
    uint32_t schemeVersion;
}NEXUS_WmDrmPdMp4SchemeTypeBox;

/***************************************************************************
Summary: 
MP4 Original Format Box for Microsoft PlayReady.
***************************************************************************/
typedef struct NEXUS_WmDrmPdMp4OriginalFormatBox 
{
    uint32_t codingName;
} NEXUS_WmDrmPdMp4OriginalFormatBox;

/***************************************************************************
Summary: 
MP4 Protection Scheme Box for Microsoft PlayReady. One per track. 
Describes the protection scheme to enforce on a given track
***************************************************************************/
typedef struct NEXUS_WmDrmPdMp4ProtectionSchemeInfo{
    NEXUS_WmDrmPdMp4SchemeTypeBox       schemeType;
    NEXUS_WmDrmPdMp4OriginalFormatBox   originalFormat;
    NEXUS_WmDrmPdMp4TrackEncryptionBox  trackEncryption;
    uint32_t                            trackId;
}NEXUS_WmDrmPdMp4ProtectionSchemeInfo;


/***************************************************************************
Summary: 
License Challenge Information
***************************************************************************/
typedef struct NEXUS_WmDrmPdLicenseChallenge
{
    unsigned     dataLength;        /* Length of data field in bytes */
    const char  *pData;             /* attr{memory=cached} Challenge Data */
    const char  *pUrl;              /* attr{memory=cached} URL the challenge data
                                       should be sent to.  Some streams may not have a URL
                                       tag in the header, in which case this pointer is NULL.
                                       In that case, it is assumed the application will know
                                       the appropriate server to send the challenge to. */
    bool         nonQuiet;          /* If true, non-quiet license acquisition is requested. */
    unsigned     appSecurityLevel;  /* Application Security Level */
    unsigned     maxResponseLength; /* Size of the response buffer in bytes */
    char        *pResponseBuffer;   /* attr{memory=cached} Buffer the response should be written to. */
} NEXUS_WmDrmPdLicenseChallenge;

/***************************************************************************
Summary: 
AES Counter Mode information.
***************************************************************************/
typedef struct NEXUS_WmdrmPdAesCounterInfo
{
    uint64_t nonce;
    uint64_t blockCounter;
    size_t   byteOffset;     /* Byte Offset within the current AES block. */
}NEXUS_WmdrmPdAesCounterInfo;

/***************************************************************************
Summary: 
License Policy Types 
 
Description: 
Values in this enumeration will match the DRM_POLICY_CALLBACK_TYPE enum 
defined by the Microsoft SDK. 
***************************************************************************/
typedef enum NEXUS_WmDrmPdPolicyType
{
    NEXUS_WmDrmPdPolicyType_ePlay = 0x1,
    NEXUS_WmDrmPdPolicyType_eCopy = 0x2,
    NEXUS_WmDrmPdPolicyType_eInclusionList = 0x3,
    NEXUS_WmDrmPdPolicyType_eExtendedRestrictionCondition = 0x4,    /* Currently not supported */
    NEXUS_WmDrmPdPolicyType_eExtendedRestrictionAction = 0x5,       /* Currently not supported */
    NEXUS_WmDrmPdPolicyType_eExtendedRestrictionQuery = 0x6,        /* Currently not supported */
    NEXUS_WmDrmPdPolicyType_eSecureStateTokenResolve = 0x7          /* Currently not supported */
}NEXUS_WmDrmPdPolicyType;



/***************************************************************************
Summary: 
Bounds for policy information
***************************************************************************/
#define NEXUS_WMDRMPD_MAX_INCLUSION_GUIDS     (20)  /*  Must match the value of DRM_MAX_INCLUSION_GUIDS in the playready SDK */
#define NEXUS_WMDRMPD_MAX_EXCLUSION_GUIDS     (20)
#define NEXUS_WMDRMPD_MAX_LICENSE_CHAIN_DEPTH (2)   /* Must match the value of DRM_MAX_LICENSE_CHAIN_DEPTH in the playready SDK */
#define NEXUS_WMDRMPD_MAX_VIDEO_OUTPUT_PROTECTION    (4)  /* Maximum number of video output protection entries in an XML license */

/***************************************************************************
Summary: 
Video Output Protection Entry for Play Policies 
***************************************************************************/
typedef struct NEXUS_WmDrmPdVideoOutProtection
{
    NEXUS_WmDrmPdGuid  guid;
    uint8_t            configData;
} NEXUS_WmDrmPdVideoOutProtection;

/***************************************************************************
Summary: 
Play Policy Definition
***************************************************************************/
typedef struct NEXUS_WmDrmPdPlayPolicy
{
    uint32_t compressedDigitalVideo;    /* Mimimum output protection level to play compressed digital video */
    uint32_t uncompressedDigitalVideo;  /* Mimimum output protection level to play uncompressed digital video */
    uint32_t analogVideo;               /* Mimimum output protection level to play analog video */
    uint32_t compressedDigitalAudio;    /* Mimimum output protection level to play compressed digital audio */
    uint32_t uncompressedDigitalAudio;  /* Mimimum output protection level to play uncompressed digital video */

    /* Video output protection ids fetched from the XML license. The number varies from license to license */
    unsigned                         numVideoOutputEntries;    /* Number of video output protections ids. */
    NEXUS_WmDrmPdVideoOutProtection  videoOutputEntries[NEXUS_WMDRMPD_MAX_VIDEO_OUTPUT_PROTECTION];          /* Pointer to array of video output protections IDs. */

}NEXUS_WmDrmPdPlayPolicy;

/***************************************************************************
Summary: 
Copy Policy Definition
***************************************************************************/
typedef struct NEXUS_WmDrmPdCopyPolicy
{
    uint32_t           minimumCopyLevel;                        /* Minimum copy level */

    /* The number of inclusion ids is fetch from the XML license. This number varies from license to license */
    unsigned           numInclusionIds;                         /* Number of inclusions IDs*/ 
    NEXUS_WmDrmPdGuid  inclusionIds[NEXUS_WMDRMPD_MAX_INCLUSION_GUIDS]; /* Guid of inclusions on output protections levels */

    /* The number of eclsusion ids is fetch from the XML license. This number varies from license to license */
    unsigned           numExclusionIds;                         /* Number of restriction IDs*/ 
    NEXUS_WmDrmPdGuid  exclusionIds[NEXUS_WMDRMPD_MAX_EXCLUSION_GUIDS]; /* Guid of restrictons on output protections levels */
}NEXUS_WmDrmPdCopyPolicy;

/***************************************************************************
Summary: 
Inclusion List Definition
***************************************************************************/
typedef struct NEXUS_WmDrmPdInclusionList
{
    NEXUS_WmDrmPdGuid   inclusionList      [NEXUS_WMDRMPD_MAX_INCLUSION_GUIDS];
    bool                inclusionListValid [NEXUS_WMDRMPD_MAX_INCLUSION_GUIDS][NEXUS_WMDRMPD_MAX_LICENSE_CHAIN_DEPTH];
    unsigned            chainDepth;
}NEXUS_WmDrmPdInclusionList;

/***************************************************************************
Summary: 
Policy Status
***************************************************************************/
typedef struct NEXUS_WmDrmPdPolicyStatus
{
    NEXUS_WmDrmPdPolicyType policyType;
    union
    {
        NEXUS_WmDrmPdPlayPolicy    play;
        NEXUS_WmDrmPdCopyPolicy    copy;
        NEXUS_WmDrmPdInclusionList inclusionList;
    } policy;
} NEXUS_WmDrmPdPolicyStatus;

#endif /* #ifndef NEXUS_WMDRMPD_TYPES_H_ */
