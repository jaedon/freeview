/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc_rap.h $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 7/23/12 3:05p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/avc/7038/bavc_rap.h $
 * 
 * Hydra_Software_Devel/16   7/23/12 3:05p nilesh
 * SW7425-3357: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-3357/1   6/28/12 12:20p nilesh
 * SW7425-3357: Added FNRT flags
 * 
 * Hydra_Software_Devel/14   3/23/12 11:29a jgarrett
 * SW7425-2378: Moving BAVC_AudioCompressionStd to bavc_rap.h, adding
 * Monkey Audio
 * 
 * Hydra_Software_Devel/13   2/10/12 1:41p jgarrett
 * SW7425-1221: Adding STC Snapshot
 * 
 * Hydra_Software_Devel/12   12/16/11 11:34a jgarrett
 * SW7425-1478: Adding BlockAlign and NumChannels to WMA metadata
 * 
 * Hydra_Software_Devel/11   12/7/11 5:44p jgarrett
 * SW7425-1478: Adding WMA encoder interface
 * 
 * Hydra_Software_Devel/10   4/18/11 8:54p jgarrett
 * SW7425-288: Adding ADTS parsing and metadata
 * 
 * Hydra_Software_Devel/9   4/17/11 1:55p jgarrett
 * SW7425-288: Adding audio codec to metadata
 * 
 * Hydra_Software_Devel/8   4/16/11 2:01p jgarrett
 * SW7425-288: Adding metadata interface
 * 
 * Hydra_Software_Devel/7   2/3/11 4:39p nilesh
 * SW7425-38,SW7425-56: Removed bavc_enc.h
 * 
 * Hydra_Software_Devel/6   2/1/11 4:57p nilesh
 * SW7425-38,SW7425-56: Added protocol to encoder buffer status
 * 
 * Hydra_Software_Devel/5   2/1/11 4:15p nilesh
 * SW7425-56,SW7425-38: Combined common portions of audio/video compressed
 * buffer descriptors
 * 
 * Hydra_Software_Devel/4   11/18/10 4:03p nilesh
 * SW7425-38: Added protocol to buffer descriptor.  Updated description of
 * transmission parameters.
 * 
 * Hydra_Software_Devel/3   11/16/10 1:49p nilesh
 * SW7425-38: Updated "flag" defines
 * 
 * Hydra_Software_Devel/2   10/19/10 1:18p nilesh
 * SW7425-38: Added TicksPerBit and SHR fields.  Changed uiOffset to
 * unsigned.
 * 
 * Hydra_Software_Devel/1   10/11/10 3:00p hongtaoz
 * SW7425-38: merged to mainline;
 * 
 * Hydra_Software_Devel/SW7425-38/1   9/21/10 2:06p nilesh
 * SW7425-38: Added A/V encoder common files
 *
 ***************************************************************************/

#ifndef BAVC_RAP_H__
#define BAVC_RAP_H__

#include "bavc_vce.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	Audio compression standards

Description:
    Enumeration of the various Audio compression standards.
See Also:

****************************************************************************/
#ifndef BAVC_AUDIOCOMPRESSSIONSTD_DEFINED
typedef enum BAVC_AudioCompressionStd
{
	BAVC_AudioCompressionStd_eMpegL1,           /* MPEG Layer 1 */
	BAVC_AudioCompressionStd_eMpegL2,           /* MPEG Layer 2 */
	BAVC_AudioCompressionStd_eMpegL3,           /* MPEG Layer 3 */
	BAVC_AudioCompressionStd_eAac,              /* AAC (ADTS) */
	BAVC_AudioCompressionStd_eAacAdts=BAVC_AudioCompressionStd_eAac,            /* AAC ADTS */
	BAVC_AudioCompressionStd_eAacLoas,          /* AAC LOAS */
	BAVC_AudioCompressionStd_eAacPlus,          /* AAC Plus (HE/SBR) (LOAS) */
	BAVC_AudioCompressionStd_eAacPlusLoas=BAVC_AudioCompressionStd_eAacPlus,    /* AAC Plus (HE/SBR) LOAS */
	BAVC_AudioCompressionStd_eAacPlusAdts,      /* AAC Plus (HE/SBR) ADTS */
	BAVC_AudioCompressionStd_eAc3,              /* AC3 */
	BAVC_AudioCompressionStd_eAc3Plus,          /* AC3_PLUS */
	BAVC_AudioCompressionStd_eAc3Lossless,      /* AC3 LOSSLESS*/
	BAVC_AudioCompressionStd_eDts,              /* DTS */
	BAVC_AudioCompressionStd_eDtshd,            /* DTSHD */
	BAVC_AudioCompressionStd_eDtsLegacy,        /* DTS legacy mode (14-bit), uses legacy frame sync */
	BAVC_AudioCompressionStd_eWmaStd,           /* WMA Standard */
	BAVC_AudioCompressionStd_eWmaStdTs,         /* WMA Standard with a 24-byte extended header */
	BAVC_AudioCompressionStd_eWmaPro,           /* WMA Pro */
	BAVC_AudioCompressionStd_eMlp,              /* MLP */
	BAVC_AudioCompressionStd_ePcm,              /* Raw PCM Data */
	BAVC_AudioCompressionStd_ePcmWav,           /* PCM input from a .wav source, requires header insertion */
	BAVC_AudioCompressionStd_eLpcmDvd,          /* DVD LPCM */
	BAVC_AudioCompressionStd_eLpcmHdDvd,        /* HD-DVD LPCM */
	BAVC_AudioCompressionStd_eLpcmBd,           /* Blu-Ray LPCM */
    BAVC_AudioCompressionStd_eLpcm1394,         /* IEEE 1394 LPCM */
	BAVC_AudioCompressionStd_eAmr,              /* Adaptive Multi-Rate compression (typically used w/3GPP) */
	BAVC_AudioCompressionStd_eDra,              /* Dynamic Resolution Adaptation.  Used in Blu-Ray and China Broadcasts. */
	BAVC_AudioCompressionStd_eCook,             /* Cook compression format, used in Real Audio 8 LBR */
	BAVC_AudioCompressionStd_eAdpcm,            /* MS ADPCM audio format */
	BAVC_AudioCompressionStd_eSbc,              /* Sub Band Codec used in Bluetooth A2DP audio */
    BAVC_AudioCompressionStd_eVorbis,           /* Vorbis audio codec.  Typically used with OGG or WebM container formats. */
    BAVC_AudioCompressionStd_eG711,             /* G.711 a-law and u-law companding.  Typically used for voice transmission. */
    BAVC_AudioCompressionStd_eG723_1,           /* G.723.1 Dual Rate Speech Coder for Multimedia Communications.  Used in H.324 and 3GPP 3G-324M.  This is different from G.723, which was superceded by G.726. */
    BAVC_AudioCompressionStd_eG726,             /* G.726 ADPCM speech codec.  Supercedes G.723 and G.721. */
    BAVC_AudioCompressionStd_eG729,             /* G.729 CS-ACELP speech codec.  Often used in VOIP applications. */
    BAVC_AudioCompressionStd_eFlac,             /* Free Lossless Audio Codec (see http://flac.sourceforge.net) */
    BAVC_AudioCompressionStd_eApe,              /* Monkey's Audio (see http://www.monkeysaudio.com/) */
	BAVC_AudioCompressionStd_eMax               /* Max value */
} BAVC_AudioCompressionStd;
#endif

/*************************/
/* AUDIO SPECIFIC FIELDS */
/*************************/
#define BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID                    0x00000001
#define BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_SAMPLING_FREQUENCY_VALID         0x00000002
#define BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID              0x00000004
#define BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_TIMING_VALID                     0x00000008

#define BAVC_AUDIO_SPECIFIC_CONFIG_MAX_LENGTH (8)

typedef enum BAVC_AudioMetadataType
{
    BAVC_AudioMetadataType_eCommon, /* BAVC_AudioMetadataDescriptor */

    /* This enum cannot contain more than 256 entries because uiDataUnitType is defined as a uint8_t */
    BAVC_AudioMetadataType_eMax
} BAVC_AudioMetadataType;

typedef struct BAVC_AudioMetadataDescriptor
{
    uint32_t uiMetadataFlags;

    struct
    {
        unsigned uiMax; /* in bits/sec */
    } stBitrate;

    struct
    {
        unsigned uiSamplingFrequency;   /* In Hz */
    } stSamplingFrequency;

    struct
    {
        uint64_t uiSTCSnapshot; /* Initial 42-bit STC Snapshot from audio encode */
        unsigned uiChunkId; /* The FNRT chunk ID for the subsequent frame descriptors */
    } stTiming;

    BAVC_AudioCompressionStd eProtocol; /* Audio Compression Protocol */
    union
    {
        struct
        {
            uint8_t auiASC[BAVC_AUDIO_SPECIFIC_CONFIG_MAX_LENGTH];  /* Audio Specific Config from ISO 11496-3 */
            unsigned uiASCLengthBits; /* Length In Bits */
            unsigned uiASCLengthBytes;/* Length In Bytes - Since this is a bitfield extra bytes will be 0 filled */
        } stAac;                      /* Applies for BAVC_AudioCompressionStd_eAacAdts, BAVC_AudioCompressionStd_eAacLoas, 
                                         BAVC_AudioCompressionStd_eAacPlusAdts, BAVC_AudioCompressionStd_eAacPlusLoas */
        struct
        {
            unsigned uiSamplesPerBlock;
            unsigned uiEncodeOptions;
            unsigned uiSuperBlockAlign;
            unsigned uiBlockAlign;
            unsigned uiNumChannels;
        } stWmaStd;                   /* Applies for BAVC_AudioCompressionStd_eWmaStd */    
    } uProtocolData;
} BAVC_AudioMetadataDescriptor;

typedef struct BAVC_AudioBufferDescriptor
{
    BAVC_CompressedBufferDescriptor stCommon;

    /* Audio Specifics */

    /* Offset to RAW Frame Data and length. */      
    unsigned uiRawDataOffset;  /* For most codecs, this will be equivalent to stCommon.uiOffset but if the data is encapsulated in another 
                                  container (e.g. AAC ADTS) this will reflect the offset to the raw data block within the encapsulated 
                                  frame. */
    size_t   uiRawDataLength;  /* For most codecs, this will be equivalent to stCommon.uiLength but if the data is encapsulated in another 
                                  container (e.g. AAC ADTS) this will reflect the length of the raw data block within the encapsulated 
                                  frame. */
    /* Metadata */
    uint8_t uiDataUnitType; /* If stCommon.uiFlags == BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA
                             *  this indicates the type of metadata that is contained in the buffer.
                             *  See BAVC_AudioMetadataType enum for possible types and values
                             */
} BAVC_AudioBufferDescriptor;

typedef struct BAVC_AudioBufferInfo
{
    BAVC_AudioCompressionStd eProtocol;
} BAVC_AudioBufferInfo;

typedef struct BAVC_AudioBufferStatus
{
    BAVC_CompressedBufferStatus stCommon;
} BAVC_AudioBufferStatus;

#define BAVC_AudioContextMap BAVC_XptContextMap


#ifdef __cplusplus
}
#endif

#endif /* BAVC_RAP_H__ */
