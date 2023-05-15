/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc_vce.h $
 * $brcm_Revision: Hydra_Software_Devel/25 $
 * $brcm_Date: 7/24/12 2:35p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/avc/7038/bavc_vce.h $
 * 
 * Hydra_Software_Devel/25   7/24/12 2:35p nilesh
 * SW7425-3357: changed pBufferBaseAddress to uiReserved
 * 
 * Hydra_Software_Devel/24   7/23/12 3:05p nilesh
 * SW7425-3357: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-3357/2   6/28/12 1:52p nilesh
 * SW7425-3357: Added
 * BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_CHUNK_ID_VALID
 * 
 * Hydra_Software_Devel/SW7425-3357/1   6/28/12 12:20p nilesh
 * SW7425-3357: Added FNRT flags
 * 
 * Hydra_Software_Devel/22   5/1/12 3:17p nilesh
 * SW7435-88: Added BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_END
 * 
 * Hydra_Software_Devel/21   4/23/12 5:06p nilesh
 * SW7425-2244: Added BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_STC_VALID
 * metadata flag
 * 
 * Hydra_Software_Devel/20   4/12/12 1:10p nilesh
 * SW7425-2244: Added BAVC_VideoMetadataDescriptor.stTiming.uiSTCSnapshot
 * 
 * Hydra_Software_Devel/19   4/12/12 12:56p nilesh
 * SW7425-2698: Added BAVC_CompressedBufferDescriptor.uiSTCSnapshot
 * 
 * Hydra_Software_Devel/18   2/1/12 12:30p nilesh
 * SW7425-2277: Added BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME to
 * support frame gaps
 * 
 * Hydra_Software_Devel/17   5/18/11 2:55p nilesh
 * SW7425-387: Added VC1 metadata support
 * 
 * Hydra_Software_Devel/16   4/13/11 4:29p nilesh
 * SW7425-303: Commented video metadata mechanism.  Added
 * BAVC_VideoMetadataType enum.
 * 
 * Hydra_Software_Devel/15   4/12/11 4:22p nilesh
 * SW7425-303,SW7425-329: Update comment indicating addresses in
 * BAVC_CompressedBufferStatus are cached
 * 
 * Hydra_Software_Devel/14   4/11/11 11:56a nilesh
 * SW7425-75: Removed BAVC_VideoBufferDescriptor.stMetadata struct.  Added
 * BAVC_CompressedBufferStatus.pMetadataBufferBaseAddress.
 * 
 * Hydra_Software_Devel/13   4/8/11 1:58p nilesh
 * SW7425-75: Removed stInfo from BAVC_VideoBufferStatus
 * 
 * Hydra_Software_Devel/12   4/8/11 12:47p nilesh
 * SW7425-75: Removed constant metadata to Buffer Info
 * 
 * Hydra_Software_Devel/11   4/7/11 4:01p nilesh
 * SW7425-75: Add support for video metadata (common)
 * 
 * Hydra_Software_Devel/10   3/29/11 4:07p nilesh
 * SW7425-74: Merge to mainline from mp4_mux_devel branch
 * 
 * Hydra_Software_Devel/mp4_mux_devel/1   3/14/11 7:38p delkert
 * SW7425-74: Add BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START and
 * uiDataUnitType to BAVC_VideoBufferDescriptor to support video data
 * unit indication (e.g. H.264 NALU)
 *
 * Hydra_Software_Devel/9   3/4/11 4:07p delkert
 * SW7425-73: Move EOS Flag since it is not a field validity flag (there
 * is no EOS field)
 *
 * Hydra_Software_Devel/8   2/8/11 12:35p nilesh
 * SW7425-1: Added eProfile and eProfile fields to BAVC_VideoBufferInfo
 * struct
 *
 * Hydra_Software_Devel/7   2/3/11 4:39p nilesh
 * SW7425-38,SW7425-1: Removed bavc_enc.h
 *
 * Hydra_Software_Devel/6   2/1/11 4:57p nilesh
 * SW7425-38,SW7425-1: Added protocol to encoder buffer status
 *
 * Hydra_Software_Devel/5   2/1/11 4:15p nilesh
 * SW7425-1,SW7425-38: Combined common portions of audio/video compressed
 * buffer descriptors
 *
 * Hydra_Software_Devel/4   11/18/10 4:08p nilesh
 * SW7425-38: Added protocol to buffer descriptor.  Updated description of
 * transmission parameters.  Added VC1 metadata to video buffer
 * descriptor.
 *
 * Hydra_Software_Devel/3   11/16/10 1:49p nilesh
 * SW7425-38: Updated "flag" defines
 *
 * Hydra_Software_Devel/2   10/19/10 1:17p nilesh
 * SW7425-38: Changed uiOffset to unsigned
 *
 * Hydra_Software_Devel/1   10/11/10 3:00p hongtaoz
 * SW7425-38: merged to mainline;
 *
 * Hydra_Software_Devel/SW7425-38/2   9/24/10 12:03p nilesh
 * SW7425-38: Added transmission parameters
 *
 * Hydra_Software_Devel/SW7425-38/1   9/21/10 2:06p nilesh
 * SW7425-38: Added A/V encoder common files
 *
 ***************************************************************************/

#ifndef BAVC_VCE_H__
#define BAVC_VCE_H__

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/*****************/
/* COMMON FIELDS */
/*****************/

/* COMMON field validity flags */
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID         0x00000001
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID                 0x00000002
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID                0x00000004
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_TICKSPERBIT_VALID         0x00000008
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_SHR_VALID                 0x00000010
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_STCSNAPSHOT_VALID         0x00000020
/* Add more field validity flags here */

/* COMMON indicator flags */
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START               0x00010000
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS                       0x00020000
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME               0x00040000 /* PTS and ESCR are expected to be valid. LENGTH is expected to be 0. */
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_END                 0x00080000 /* If both FRAME_START and FRAME_END are set, then the descriptor references a complete frame */
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOC                       0x00100000 /* End Of Chunk indicator.  If set, then indicates this frame is the last frame of the FNRT chunk */

/* Add more indicator flags ABOVE this line */
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA                  0x40000000
#define BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EXTENDED                  0x80000000

typedef struct BAVC_CompressedBufferDescriptor
{
   uint32_t uiFlags;

   /* Timestamp Parameters */
   uint32_t uiOriginalPTS; /* 32-bit original PTS value (in 45 Khz or 27Mhz?) */
   uint64_t uiPTS; /* 33-bit PTS value (in 90 Khz) */
   uint64_t uiSTCSnapshot; /* 42-bit STC snapshot when frame received by the encoder (in 27Mhz) */

   /* Transmission Parameters */
   uint32_t uiESCR; /* Expected mux transmission start time for the first bit of the data (in 27 Mhz) */

   /* TicksPerBit and iSHR indicate the transmission speed for the data.  So, the total duration (in 27 Mhz)
    * for the transmission of this buffer is:
    *
    * uiDeltaESCR = ( uiTicksPerBit * ( uiLength * 8 ) );
    * if ( iSHR > 0 )
    * {
    *    uiDeltaESCR >>= iSHR;
    * }
    * else
    * {
    *    uiDeltaESCR <<= -iSHR;
    * }
    *
    * And the *earliest* valid transmission start time for the next buffer is simply:
    *
    * uiNextESCR = uiESCR + uiDeltaESCR;
    */
   uint16_t uiTicksPerBit;
   int16_t iSHR;

   /* Buffer Parameters */
   unsigned uiOffset; /* REQUIRED: offset of frame data from frame buffer base address (in bytes) */
   size_t uiLength;   /* REQUIRED: 0 if fragment is empty, e.g. for EOS entry (in bytes) */
   unsigned uiReserved; /* Unused field */
} BAVC_CompressedBufferDescriptor;

typedef struct BAVC_CompressedBufferStatus
{
      /* Note: The following are CACHED addresses.
       *
       * The encoder must guarantee cache coherence for the referenced data.
       * The consumer must convert to uncached or offset as needed if the data will
       * be consumed by HW.
       */
      const void *pFrameBufferBaseAddress; /* The cached address of the start of the CDB. */
      const void *pMetadataBufferBaseAddress; /* The cached address of the start of the metadata */
} BAVC_CompressedBufferStatus;

/*************************/
/* VIDEO SPECIFIC FIELDS */
/*************************/

/* VIDEO field validity flags */
#define BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID                 0x00000001

/* VIDEO indicator flags */
#define BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP                       0x00010000
/* indicates a video data unit (NALU, EBDU, etc) starts at the beginning of
   this descriptor  - if this is set, then the uiDataUnitID field is valid also */
#define BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START           0x00020000

#define BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_EXTENDED                  0x80000000

typedef struct BAVC_VideoBufferDescriptor
{
      BAVC_CompressedBufferDescriptor stCommon;
      uint32_t uiVideoFlags;

      /* Timestamp Parameters */
      uint64_t uiDTS; /* 33-bit DTS value (in 90 Kh or 27Mhz?) */

      /* Metadata */
      uint8_t uiDataUnitType; /* The meaning of this field depends on the flags that are
                               * set in stCommon.uiFlags or uiVideoFlags.
                               * These flags are *mutually exclusive*.
                               *
                               * uiVideoFlags = BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START
                               *  protocol-specific data unit identifier (e.g. for H.264, this is the NALU type)
                               *
                               * uiFlags = BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA
                               *  indicates the type of metadata that is contained in the buffer.
                               *  See BAVC_VideoMetadataType enum for possible types and values
                               */
} BAVC_VideoBufferDescriptor;

typedef struct BAVC_VideoBufferInfo
{
      BAVC_VideoCompressionStd eProtocol;
      BAVC_VideoCompressionProfile eProfile;
      BAVC_VideoCompressionLevel eLevel;
} BAVC_VideoBufferInfo;

typedef struct BAVC_VideoBufferStatus
{
      BAVC_CompressedBufferStatus stCommon;
} BAVC_VideoBufferStatus;

#define BAVC_VideoContextMap BAVC_XptContextMap

/******************/
/* VIDEO Metadata */
/******************/
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID          0x00000001
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_FRAMERATE_VALID        0x00000002
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_DIMENSION_CODED_VALID  0x00000004
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_BUFFERINFO_VALID       0x00000008
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID    0x00000010
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_STC_VALID       0x00000020
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_CHUNK_ID_VALID  0x00000040
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#define BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_ETS_DTS_OFFSET_VALID  0x00000080
#endif
typedef enum BAVC_VideoMetadataType
{
   BAVC_VideoMetadataType_eCommon, /* BAVC_VideoMetadataDescriptor */

   /* This enum cannot contain more than 256 entries because uiDataUnitType is defined as a uint8_t */
   BAVC_VideoMetadataType_eMax
} BAVC_VideoMetadataType;

typedef struct BAVC_VideoMetadata_VC1
{
      /* Sequence Header A (VERT_SIZE and HORIZ_SIZE): See BAVC_VideoMetadataDescriptor.stDimension.coded */

      /* Sequence Header B */
      struct
      {
            /* For LEVEL: See BAVC_VideoMetadataDescriptor.stBufferInfo.eLevel */
            bool bCBR; /* CBR */
            uint32_t uiHRDBuffer; /* HRD_BUFFER (Only 24 bits are valid) */
            /* For HRD_RATE: See BAVC_VideoMetadataDescriptor.stBitrate */
            /* For FRAMERATE: See BAVC_VideoMetadataDescriptor.stFrameRate */
      } sequenceHeaderB;

      struct
      {
            /* For Profile: See BAVC_VideoMetadataDescriptor.stBufferInfo.eProfile */
            uint8_t uiQuantizedFrameRatePostProcessing; /* FRMRTQ_POSTPROC (Only 3 bits are valid) */
            uint8_t uiQuantizedBitratePostProcessing; /* BITRTQ_POSTPROC (Only 5 bits are valid) */
            bool bLoopFilter; /* LOOPFILTER */
            bool bMultiResolution; /* MULTIRES */
            bool bFastUVMotionCompensation; /* FASTUVMC */
            bool bExtendedMotionVector; /* EXTENDED_MV */
            uint8_t uiMacroblockQuantization; /* DQUANT (Only 2 bits are valid) */
            bool bVariableSizedTransform; /* VSTRANSFORM */
            bool bOverlappedTransform; /* OVERLAP */
            bool bSyncMarker; /* SYNCMARKER */
            bool bRangeReduction; /* RANGERED */
            uint8_t uiMaxBFrames; /* MAXBFRAMES (Only 3 bits are valid) */
            uint8_t uiQuantizer; /* QUANTIZER (Only 2 bits are valid) */
            bool bFrameInterpolation; /* FINTERPFLAG */
      } sequenceHeaderC;
} BAVC_VideoMetadata_VC1;

typedef struct BAVC_VideoMetadataDescriptor
{
      uint32_t uiMetadataFlags;

      BAVC_VideoBufferInfo stBufferInfo;

      struct
      {
            uint32_t uiMax; /* in bits/sec */
      } stBitrate;

      struct
      {
            BAVC_FrameRateCode eFrameRateCode;
      } stFrameRate;

      struct
      {
            struct
            {
                  uint16_t uiWidth;
                  uint16_t uiHeight;
            } coded;
      } stDimension;

      union
      {
            BAVC_VideoMetadata_VC1 stVC1; /* Applies for BAVC_VideoCompressionStd_eVC1SimpleMain */
      } uProtocolData;

      struct
      {
            uint64_t uiSTCSnapshot; /* Initial 42-bit STC Snapshot from video encode */
            unsigned uiChunkId; /* The FNRT chunk ID for the subsequent frame descriptors */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
            unsigned uiEtsDtsOffset; /* The ETS to DTS offset for the encode session as determined by RC */
#endif
      } stTiming;
} BAVC_VideoMetadataDescriptor;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
/***************************/
/* VIDEO Closed Captioning */
/***************************/
typedef struct BAVC_VideoUserDataStatus
{
        unsigned uiPendingBuffers; /* Buffers pending */
        unsigned uiCompletedBuffers; /* Buffers completed since previous call to XXX_GetStatusUserDataBuffers_isr() */
} BAVC_VideoUserDataStatus;
#endif

#ifdef __cplusplus
}
#endif

#endif /* BAVC_VCE_H__ */
