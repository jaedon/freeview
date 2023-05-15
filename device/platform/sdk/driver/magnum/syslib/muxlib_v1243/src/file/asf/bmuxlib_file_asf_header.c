/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf_header.c $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 6/21/12 1:49p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf_header.c $
 * 
 * Hydra_Software_Devel/18   6/21/12 1:49p delkert
 * SW7425-3272: Use correctly sized variables for parameters to SetXX
 * macros to fix coverity errors
 *
 * Hydra_Software_Devel/17   6/20/12 10:24a delkert
 * SW7425-3272: Explicitly cast input data in the SetXX macros to
 * (hopefully) resolve coverity complaints.
 *
 * Hydra_Software_Devel/16   3/14/12 2:05p nilesh
 * SW7425-2618: AAC/AAC+ LOAS is not supported
 *
 * Hydra_Software_Devel/15   1/24/12 1:02p nilesh
 * SW7425-1476: Set bits per sample and avg bytes per second in type
 * specific field for WMA
 *
 * Hydra_Software_Devel/14   12/16/11 4:31p nilesh
 * SW7425-1476: Added support for setting audio block alignment and num
 * channels metadata for WMA.
 *
 * Hydra_Software_Devel/13   12/9/11 11:56a nilesh
 * SW7425-1476: Added support for populating WMA metadata in codec
 * specific data
 *
 * Hydra_Software_Devel/12   11/15/11 4:26p nilesh
 * SW7425-1691: Reset stStartSettings and stFinishSettings on stop()
 *
 * Hydra_Software_Devel/11   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/10/11 11:56a nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/10   10/11/11 4:41p nilesh
 * SW7425-1476: Initial WMA audio support
 *
 * Hydra_Software_Devel/9   7/18/11 6:51p nilesh
 * SW7425-937: Added H.264 video support
 *
 * Hydra_Software_Devel/8   6/9/11 5:00p nilesh
 * SW7425-684: Disable MP3 padding
 *
 * Hydra_Software_Devel/7   6/9/11 4:38p nilesh
 * SW7425-684: Added MP3 Audio Support
 *
 * Hydra_Software_Devel/6   5/19/11 5:34p nilesh
 * SW7425-477: Fixed AAC Audio
 *
 * Hydra_Software_Devel/5   5/18/11 2:55p nilesh
 * SW7425-387: Added VC1 metadata support
 *
 * Hydra_Software_Devel/4   5/17/11 3:41p nilesh
 * SW7425-447: Added AAC Audio Support
 *
 * Hydra_Software_Devel/3   5/11/11 4:03p nilesh
 * SW7425-447: Offset presentation by pre-roll
 *
 * Hydra_Software_Devel/2   5/11/11 3:22p nilesh
 * SW7425-447: Post-Process ASF.
 *
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_file_asf.h"
#include "bmuxlib_file_asf_priv.h"

#include "bmuxlib_file_asf_header.h"

BDBG_MODULE(BMUXLIB_FILE_ASF_HEADER);

/*********************/
/* Header Generation */
/*********************/

/* Header Extension Object (Mandatory, one only) */
#define BMUXlib_File_ASF_HeaderExtensionObject_MAXSIZE 46

static const uint8_t s_auiDefaultHeaderExtensionObject[BMUXlib_File_ASF_HeaderExtensionObject_MAXSIZE] =
{
 0xB5, 0x03, 0xBF, 0x5F, /* Object ID (Header Extension) */
 0x2E, 0xA9,
 0xCF, 0x11,
 0x8E, 0xE3,
 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,
 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Object Size (46 bytes) */
 0x11, 0xD2, 0xD3, 0xAB, /* Reserved 1 */
 0xBA, 0xA9,
 0xCF, 0x11,
 0x8E, 0xE6,
 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,
 0x06, 0x00, /* Reserved 2 */
 /* Header Extension Data Size */
 /* NO Header Extension Data */
};

void
BMUXlib_File_ASF_P_Header_SetHeaderExtension(
         BMUXlib_File_ASF_Handle hASFMux,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiHeaderExtStartOffset = *puiCurrentOffset;
   uint8_t *pHeaderExtension = hASFMux->pHeaderBuffer + uiHeaderExtStartOffset;

   BDBG_ASSERT( ( uiHeaderExtStartOffset + BMUXlib_File_ASF_HeaderExtensionObject_MAXSIZE) <= hASFMux->uiHeaderBufferSize );

   BKNI_Memcpy( pHeaderExtension,
                s_auiDefaultHeaderExtensionObject,
                BMUXlib_File_ASF_HeaderExtensionObject_MAXSIZE
                );

   *puiCurrentOffset += BMUXlib_File_ASF_HeaderExtensionObject_MAXSIZE;
}

/* Stream Properties Object (Mandatory, one per stream) */
#define BMUXlib_File_ASF_Audio_ErrorCorrection_MAXSIZE 8
static const uint8_t s_auiDefaultStreamProperties_Audio_ErrorCorrection[BMUXlib_File_ASF_Audio_ErrorCorrection_MAXSIZE] =
{
 0x01, /* Span (1) */
 0x00, 0x00, /* Virtual Packet Length (TODO:) */
 0x00, 0x00, /* Virtual Chunk Length (TODO:) */
 0x01, 0x00, /* Silence Data Length (1 byte) */
 0x00, /* Silence Data (0) */
};

void
BMUXlib_File_ASF_P_Header_SetStreamProperties_ErrorCorrectionData_Audio(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
#if 0
   unsigned uiErrorCorrectionStartOffset = *puiCurrentOffset;
   uint8_t *pErrorCorrectionData = hASFMux->pHeaderBuffer + uiErrorCorrectionStartOffset;

   /* Set Codec Specific Data */
   BDBG_ASSERT( ( uiErrorCorrectionStartOffset + BMUXlib_File_ASF_Audio_ErrorCorrection_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );
   BKNI_Memcpy( pErrorCorrectionData,
                s_auiDefaultStreamProperties_Audio_ErrorCorrection,
                BMUXlib_File_ASF_Audio_ErrorCorrection_MAXSIZE
                );
   *puiCurrentOffset += BMUXlib_File_ASF_Audio_ErrorCorrection_MAXSIZE;
#endif

   BSTD_UNUSED( hASFMux );
   BSTD_UNUSED( pInput );
   BSTD_UNUSED( puiCurrentOffset );

   /* TODO: Set Virtual Packet Length */
   /* TODO: Set Chunk Length */
}

#define BMUXlib_File_ASF_StreamProperties_CodecData_WMA_MAXSIZE 10

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SAMPLESPERBLOCK 0
#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_ENCODEOPTIONS 4
#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SUPERBLOCKALIGN 6

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetSamplesPerBlock(pHeader, spb) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SAMPLESPERBLOCK, (spb) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetEncodeOptions(pHeader, encodeoptions) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_ENCODEOPTIONS, (encodeoptions) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetSuperBlockAlign(pHeader, sba) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SUPERBLOCKALIGN, (sba) )

#if 0
static const uint8_t s_auiDefaultStreamProperties_CodecData_WMA[BMUXlib_File_ASF_StreamProperties_CodecData_WMA_MAXSIZE] =
{
   0x00, 0x00, 0x00, 0x00, /* Samples Per Block */
   0x00, 0x00, /* Encode Options */
   0x00, 0x00, 0x00, 0x00, /* Super Block Align */
};

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetCodecSpecificData(pHeader, pAudioSpecificData, size) \
         BKNI_Memcpy( (pHeader), \
                      pAudioSpecificData, \
                      size \
                      );
#endif

void
BMUXlib_File_ASF_P_Header_SetCodecData_WMA(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiWMAStartOffset = *puiCurrentOffset;
   uint8_t *pWMAData = hASFMux->pHeaderBuffer + uiWMAStartOffset;

   BSTD_UNUSED( pInput );

   /* Set Audio Specific Config */
   BDBG_ASSERT( ( *puiCurrentOffset + BMUXlib_File_ASF_StreamProperties_CodecData_WMA_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );

   *puiCurrentOffset += BMUXlib_File_ASF_StreamProperties_CodecData_WMA_MAXSIZE;

   BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetSamplesPerBlock( pWMAData, pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stWmaStd.uiSamplesPerBlock );
   BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetEncodeOptions( pWMAData, pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stWmaStd.uiEncodeOptions );
   BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_WMA_SetSuperBlockAlign( pWMAData, pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stWmaStd.uiSuperBlockAlign );
}

#define BMUXlib_File_ASF_StreamProperties_CodecData_MP3_MAXSIZE 12
static const uint8_t s_auiDefaultStreamProperties_CodecData_MP3[BMUXlib_File_ASF_StreamProperties_CodecData_MP3_MAXSIZE] =
{
 0x01, 0x00, /* ID (1 - MPEG) */
 0x02, 0x00, 0x00, 0x00, /* Flags (2 - PADDING=OFF) */
 0x00, 0x00, /* Block Size (TODO) */
 0x00, 0x00, /* Frames Per Block (TODO) */
 0x00, 0x00, /* Codec Delay (TODO) */
};

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_MP3_SetCodecSpecificData(pHeader, pAudioSpecificData, size) \
         BKNI_Memcpy( (pHeader), \
                      pAudioSpecificData, \
                      size \
                      );

void
BMUXlib_File_ASF_P_Header_SetCodecData_MP3(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiMP3StartOffset = *puiCurrentOffset;
   uint8_t *pMP3Data = hASFMux->pHeaderBuffer + uiMP3StartOffset;

   BSTD_UNUSED( pInput );

   /* Set Audio Specific Config */
   BDBG_ASSERT( ( *puiCurrentOffset + BMUXlib_File_ASF_StreamProperties_CodecData_MP3_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );

   *puiCurrentOffset += BMUXlib_File_ASF_StreamProperties_CodecData_MP3_MAXSIZE;

   BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_MP3_SetCodecSpecificData( pMP3Data,
                                                                                 s_auiDefaultStreamProperties_CodecData_MP3,
                                                                                 BMUXlib_File_ASF_StreamProperties_CodecData_MP3_MAXSIZE );
}

#ifndef BMUXlib_File_ASF_P_USE_AAC_RAW
#define BMUXlib_File_ASF_CodecData_AAC_MAXSIZE 12

static const uint8_t s_auiDefaultStreamProperties_CodecData_AAC[BMUXlib_File_ASF_CodecData_AAC_MAXSIZE] =
{
 /* Payload Type
  * 0-RAW.  The stream contains raw_data_block() elements only.
  * 1-ADTS. The stream contains an adts_sequence(), as defined by MPEG-2.
  * 2-ADIF. The stream contains an adif_sequence(), as defined by MPEG-2.
  * 3-LOAS. The stream contains an MPEG-4 audio transport stream with a
  *         synchronization layer LOAS and a multiplex layer LATM.
  * All other codes are reserved.
  */
 0x00, 0x00,
 /* Audio Profile Level
  * This is the 8-bit field audioProfileLevelIndication available in the
  * MPEG-4 object descriptor.  It is an indication (as defined in MPEG-4 audio)
  * of the audio profile and level required to process the content associated
  * with this stream. For example values 0x28-0x2B correspond to AAC Profile,
  * values 0x2C-0x2F correspond to HE-AAC profile and 0x30-0x33 for HE-AAC v2 profile.
  * If unknown, set to zero or 0xFE ("no audio profile specified").
  */
 0x00, 0x00,
 /* Struct Type
  * Currently only one data type is supported:
  * 0- AudioSpecificConfig() (as defined by MPEG-4 Audio, ISO/IEC 14496-3) will follow this structure.
  *    wfx.cbSize will indicate the total length including AudioSpecificConfig().
  *    Use HEAACWAVEFORMAT to gain easy access to the address of the first byte of
  *    AudioSpecificConfig() for parsing.
  *    Typical values for the size of AudioSpecificConfig (ASC) are:
  *    - 2 bytes for AAC or HE-AAC v1/v2 with implicit signaling of SBR,
  *    - 5 bytes for HE-AAC v1 with explicit signaling of SBR,
  *    - 7 bytes for HE-AAC v2 with explicit signaling of SBR and PS.
  *    The size may be longer than 7 bytes if the 4-bit channelConfiguration field in ASC is zero,
  *    which means program_config_element() is present in ASC.
  *
  * All other codes are reserved.
  */
 0x00, 0x00,
 0x00, 0x00, /* RESERVED 1 */
 0x00, 0x00, 0x00, 0x00, /* RESERVED 2 */
 /* Audio Specific Config */
};

#define BMUXlib_File_ASF_P_CodecData_AAC_PAYLOADTYPE 0
#define BMUXlib_File_ASF_P_CodecData_AAC_AUDIOPROFILELEVEL 2
#define BMUXlib_File_ASF_P_CodecData_AAC_AUDIOSPECIFICCONFIG 12

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetPayloadType(pHeader, codec) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_CodecData_AAC_PAYLOADTYPE, (codec) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetAudioProfileLevel(pHeader, codec) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_CodecData_AAC_AUDIOPROFILELEVEL, (codec) )
#else
#define BMUXlib_File_ASF_P_CodecData_AAC_AUDIOSPECIFICCONFIG 0
#endif

#define BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetCodecSpecificData(pHeader, pAudioSpecificData, size) \
         BKNI_Memcpy( (pHeader) + BMUXlib_File_ASF_P_CodecData_AAC_AUDIOSPECIFICCONFIG, \
                      pAudioSpecificData, \
                      size \
                      );

void
BMUXlib_File_ASF_P_Header_SetCodecData_AAC(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiAACStartOffset = *puiCurrentOffset;
   uint8_t *pAACData = hASFMux->pHeaderBuffer + uiAACStartOffset;

#ifndef BMUXlib_File_ASF_P_USE_AAC_RAW
   /* Set Codec Specific Data */
   BDBG_ASSERT( ( uiAACStartOffset + BMUXlib_File_ASF_CodecData_AAC_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );
   BKNI_Memcpy( pAACData,
                s_auiDefaultStreamProperties_CodecData_AAC,
                BMUXlib_File_ASF_CodecData_AAC_MAXSIZE
                );
   *puiCurrentOffset += BMUXlib_File_ASF_CodecData_AAC_MAXSIZE;

   /* Set Payload Type */
   switch ( pInput->bufferInfo.pstAudio->eProtocol )
   {
      case BAVC_AudioCompressionStd_eAacAdts:           /* AAC (ADTS) */
      case BAVC_AudioCompressionStd_eAacPlusAdts:       /* AAC Plus (HE/SBR) ADTS */
         BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetPayloadType( pAACData, 1 );
         break;

      case BAVC_AudioCompressionStd_eAacLoas:           /* AAC LOAS */
      case BAVC_AudioCompressionStd_eAacPlusLoas:       /* AAC Plus (HE/SBR) (LOAS) */
         BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetPayloadType( pAACData, 3 );
         break;

      default:
         BDBG_ERR(("Unsupported audio protocol (%d)", pInput->bufferInfo.pstAudio->eProtocol));
         break;
   }

   /* TODO: Set Audio Profile/Level */
   BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetAudioProfileLevel( pAACData, 0x0000 );
#endif

   /* Set Audio Specific Config */
   BDBG_ASSERT( ( *puiCurrentOffset + pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stAac.uiASCLengthBytes ) <= hASFMux->uiHeaderBufferSize );

   *puiCurrentOffset += pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stAac.uiASCLengthBytes;

   BMUXlib_File_ASF_P_StreamPropertiesObject_CodecData_AAC_SetCodecSpecificData( pAACData,
                                                                                 pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stAac.auiASC,
                                                                                 pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stAac.uiASCLengthBytes );
}

#define BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Audio_MAXSIZE 18
static const uint8_t s_auiDefaultStreamProperties_TypeSpecificData_Audio[BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Audio_MAXSIZE] =
{
 0x10, 0x16, /* Codec ID/Format Tag (HEAAC) */
 0x00, 0x00, /* Number of Channels (0 - Unknown) */
 0x00, 0x00, 0x00, 0x00, /* Samples Per Second */
 0x00, 0x00, 0x00, 0x00, /* Average Number of Bytes Per Second (0 - Unknown) */
 0x00, 0x00, /* Block Alignment (1) */
 0x00, 0x00, /* Bits Per Sample (0 - Unknown) */
 0x00, 0x00, /* Codec Specific Data Size */
 /* Codec Specific Data */
};

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_CODECID 0
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_NUMCHANNELS 2
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SAMPLESPERSECOND 4
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_AVGBYTESPERSECOND 8
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_BLOCKALIGNMENT 12
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_BITSPERSAMPLE 14
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_CODECSPECIFICDATASIZE 16

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecID(pHeader, codec) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_CODECID, (codec) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetNumberOfChannels(pHeader, channels) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_NUMCHANNELS, (channels) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetSamplesPerSecond(pHeader, sps) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SAMPLESPERSECOND, (sps) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetAvgBytesPerSecond(pHeader, abps) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_AVGBYTESPERSECOND, (abps) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetBlockAlignment(pHeader, ba) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_BLOCKALIGNMENT, (ba) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetBitsPerSample(pHeader, bps) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_BITSPERSAMPLE, (bps) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecSpecificDataSize(pHeader, size) \
   BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_CODECSPECIFICDATASIZE, (size) )

void
BMUXlib_File_ASF_P_Header_SetStreamProperties_TypeSpecificData_Audio(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiAudioStartOffset = *puiCurrentOffset;
   uint8_t *pStreamPropertiesAudio = hASFMux->pHeaderBuffer + uiAudioStartOffset;

   /* Set Type Specific Data */
   BDBG_ASSERT( ( uiAudioStartOffset + BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Audio_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );
   BKNI_Memcpy( pStreamPropertiesAudio,
                s_auiDefaultStreamProperties_TypeSpecificData_Audio,
                BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Audio_MAXSIZE
                );
   *puiCurrentOffset += BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Audio_MAXSIZE;

   switch ( pInput->bufferInfo.pstAudio->eProtocol )
   {
      case BAVC_AudioCompressionStd_eAacAdts:              /* AAC (ADTS) */
      case BAVC_AudioCompressionStd_eAacPlusAdts:      /* AAC Plus (HE/SBR) ADTS */
         /* Set Codec ID */
#ifdef BMUXlib_File_ASF_P_USE_AAC_RAW
         BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecID(
                  pStreamPropertiesAudio,
                  0x00FF );
#else
         BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecID(
                  pStreamPropertiesAudio,
                  0x1610 );
#endif

         {
            unsigned uiCodecSpecificDataStartOffset = *puiCurrentOffset;

            BMUXlib_File_ASF_P_Header_SetCodecData_AAC(
                     hASFMux,
                     pInput,
                     puiCurrentOffset
                     );

            /* Set Codec Specific Data Size */
            BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecSpecificDataSize( pStreamPropertiesAudio,
                                                                                                (*puiCurrentOffset - uiCodecSpecificDataStartOffset)
                                                                                   );
         }

         break;

      case BAVC_AudioCompressionStd_eMpegL3:
         /* Set Codec ID */
         BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecID(
                  pStreamPropertiesAudio,
                  0x0055 );

         {
            unsigned uiCodecSpecificDataStartOffset = *puiCurrentOffset;

            BMUXlib_File_ASF_P_Header_SetCodecData_MP3(
                     hASFMux,
                     pInput,
                     puiCurrentOffset
                     );

            /* Set Codec Specific Data Size */
            BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecSpecificDataSize( pStreamPropertiesAudio,
                                                                                                (*puiCurrentOffset - uiCodecSpecificDataStartOffset)
                                                                                   );
         }
         break;

      case BAVC_AudioCompressionStd_eWmaStd:
         /* Set Codec ID */
         BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecID(
                  pStreamPropertiesAudio,
                  0x0161 );

         {
            unsigned uiCodecSpecificDataStartOffset = *puiCurrentOffset;

            BMUXlib_File_ASF_P_Header_SetCodecData_WMA(
                     hASFMux,
                     pInput,
                     puiCurrentOffset
                     );

            BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetNumberOfChannels( pStreamPropertiesAudio, pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stWmaStd.uiNumChannels );
            BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetBlockAlignment( pStreamPropertiesAudio, pInput->stCodecSpecificData.audio.data.stCommon.uProtocolData.stWmaStd.uiBlockAlign );
            BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetBitsPerSample( pStreamPropertiesAudio, 16);
            if ( ( true == pInput->bCodecSpecificDataValid )
                 && ( true == pInput->stCodecSpecificData.audio.data.bCommonValid  )
                 && ( 0 != ( pInput->stCodecSpecificData.audio.data.stCommon.uiMetadataFlags & BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID ) ) )
            {
                /* Indicate max bytes per second */
                BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetAvgBytesPerSecond( pStreamPropertiesAudio, pInput->stCodecSpecificData.audio.data.stCommon.stBitrate.uiMax/8);
            }
            else
            {
                /* Indicate 192kbit/sec as default */
                BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetAvgBytesPerSecond( pStreamPropertiesAudio, 192000/8);
            }

            /* Set Codec Specific Data Size */
            BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetCodecSpecificDataSize( pStreamPropertiesAudio,
                                                                                                (*puiCurrentOffset - uiCodecSpecificDataStartOffset)
                                                                                   );
         }
         break;

      default:
         BDBG_ERR(("Unsupported audio protocol (%d)", pInput->bufferInfo.pstAudio->eProtocol));
         break;
   }

   /* Set Samples Per Second */
   BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Audio_SetSamplesPerSecond( pStreamPropertiesAudio,
                                                                                         pInput->stCodecSpecificData.audio.data.stCommon.stSamplingFrequency.uiSamplingFrequency );
}

/* H.264 Codec Specific */
static const uint8_t s_auiCompressionID_Video_H264[4] =
{
 'H','2','6','4'
};

/* VC1 Codec Specific */
static const uint8_t s_auiCompressionID_Video_VC1[4] =
{
 'W','M','V','3'
};

/* VC1 Sequence Layer Struct (Big Endian - MSB First)*/
#define BMUXlib_File_ASF_CodecData_VC1_MAXSIZE 4

#define BMUXlib_File_ASF_P_CodecData_VC1_PROFILE_MASK 0x0F
#define BMUXlib_File_ASF_P_CodecData_VC1_PROFILE_SHIFT 28

#define BMUXlib_File_ASF_P_CodecData_VC1_FRMRTQ_POSTPROC_MASK 0x07
#define BMUXlib_File_ASF_P_CodecData_VC1_FRMRTQ_POSTPROC_SHIFT 25

#define BMUXlib_File_ASF_P_CodecData_VC1_BITRTQ_POSTPROC_MASK 0x1F
#define BMUXlib_File_ASF_P_CodecData_VC1_BITRTQ_POSTPROC_SHIFT 20

#define BMUXlib_File_ASF_P_CodecData_VC1_LOOPFILTER_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_LOOPFILTER_SHIFT 19

#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_SHIFT 18
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_DEFAULT 0

#define BMUXlib_File_ASF_P_CodecData_VC1_MULTIRES_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_MULTIRES_SHIFT 17

#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_SHIFT 16
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_DEFAULT 1

#define BMUXlib_File_ASF_P_CodecData_VC1_FASTUVMC_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_FASTUVMC_SHIFT 15

#define BMUXlib_File_ASF_P_CodecData_VC1_EXTENDED_MV_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_EXTENDED_MV_SHIFT 14

#define BMUXlib_File_ASF_P_CodecData_VC1_DQUANT_MASK 0x03
#define BMUXlib_File_ASF_P_CodecData_VC1_DQUANT_SHIFT 12

#define BMUXlib_File_ASF_P_CodecData_VC1_VSTRANSFORM_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_VSTRANSFORM_SHIFT 11

#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_SHIFT 10
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_DEFAULT 0

#define BMUXlib_File_ASF_P_CodecData_VC1_OVERLAP_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_OVERLAP_SHIFT 9

#define BMUXlib_File_ASF_P_CodecData_VC1_SYNCMARKER_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_SYNCMARKER_SHIFT 8

#define BMUXlib_File_ASF_P_CodecData_VC1_RANGERED_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_RANGERED_SHIFT 7

#define BMUXlib_File_ASF_P_CodecData_VC1_MAXBFRAMES_MASK 0x07
#define BMUXlib_File_ASF_P_CodecData_VC1_MAXBFRAMES_SHIFT 4

#define BMUXlib_File_ASF_P_CodecData_VC1_QUANTIZER_MASK 0x03
#define BMUXlib_File_ASF_P_CodecData_VC1_QUANTIZER_SHIFT 2

#define BMUXlib_File_ASF_P_CodecData_VC1_FINTERPFLAG_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_FINTERPFLAG_SHIFT 1

#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_MASK 0x01
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_SHIFT 0
#define BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_DEFAULT 1

void
BMUXlib_File_ASF_P_Header_SetCodecData_VC1(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiVC1StartOffset = *puiCurrentOffset;
   uint8_t *pVC1Data = hASFMux->pHeaderBuffer + uiVC1StartOffset;

   /* Set Codec Specific Data */
   BDBG_ASSERT( ( uiVC1StartOffset + BMUXlib_File_ASF_CodecData_VC1_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );
   BKNI_Memset( pVC1Data,
                0,
                BMUXlib_File_ASF_CodecData_VC1_MAXSIZE
                );
   *puiCurrentOffset += BMUXlib_File_ASF_CodecData_VC1_MAXSIZE;

   /* Set Struct C */
   {
      uint32_t uiCodecData = 0;

      /* Set Profile */
      {
         BAVC_VideoCompressionProfile eProfile;
         uint8_t uiProfile = 0;

         if ( 0 != ( pInput->stCodecSpecificData.video.data.stCommon.uiMetadataFlags & BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_BUFFERINFO_VALID ) )
         {
            eProfile = pInput->stCodecSpecificData.video.data.stCommon.stBufferInfo.eProfile;
         }
         else
         {
            eProfile = pInput->bufferInfo.pstVideo->eProfile;
         }

         switch ( eProfile )
         {
            case BAVC_VideoCompressionProfile_eSimple:
               uiProfile = 0;
               break;

            case BAVC_VideoCompressionProfile_eMain:
               uiProfile = 4;
               break;

            case BAVC_VideoCompressionProfile_eAdvanced:
               uiProfile = 12;
               break;

            default:
               BDBG_ERR(("Unknown Profile (%d)", pInput->bufferInfo.pstVideo->eProfile ));
         }
         uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_PROFILE_MASK << BMUXlib_File_ASF_P_CodecData_VC1_PROFILE_SHIFT);
         uiCodecData |= ((uiProfile & BMUXlib_File_ASF_P_CodecData_VC1_PROFILE_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_PROFILE_SHIFT);
      }

      BDBG_ASSERT( 0 != ( pInput->stCodecSpecificData.video.data.stCommon.uiMetadataFlags & BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID ) );

      /* Set FRMRTQ_POSTPROC */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_FRMRTQ_POSTPROC_MASK << BMUXlib_File_ASF_P_CodecData_VC1_FRMRTQ_POSTPROC_SHIFT);
      uiCodecData |= ((pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.uiQuantizedFrameRatePostProcessing & BMUXlib_File_ASF_P_CodecData_VC1_FRMRTQ_POSTPROC_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_FRMRTQ_POSTPROC_SHIFT);

      /* Set BITRTQ_POSTPROC */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_BITRTQ_POSTPROC_MASK << BMUXlib_File_ASF_P_CodecData_VC1_BITRTQ_POSTPROC_SHIFT);
      uiCodecData |= ((pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.uiQuantizedBitratePostProcessing & BMUXlib_File_ASF_P_CodecData_VC1_BITRTQ_POSTPROC_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_BITRTQ_POSTPROC_SHIFT);

      /* Set LOOPFILTER */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_LOOPFILTER_MASK << BMUXlib_File_ASF_P_CodecData_VC1_LOOPFILTER_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bLoopFilter ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_LOOPFILTER_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_LOOPFILTER_SHIFT);

      /* Set RESERVED3 */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_MASK << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_SHIFT);
      uiCodecData |= ((BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_DEFAULT & BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED3_SHIFT);

      /* Set MULTIRES */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_MULTIRES_MASK << BMUXlib_File_ASF_P_CodecData_VC1_MULTIRES_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bMultiResolution ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_MULTIRES_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_MULTIRES_SHIFT);

      /* Set RESERVED4 */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_MASK << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_SHIFT);
      uiCodecData |= ((BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_DEFAULT & BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED4_SHIFT);

      /* Set FASTUVMC */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_FASTUVMC_MASK << BMUXlib_File_ASF_P_CodecData_VC1_FASTUVMC_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bFastUVMotionCompensation ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_FASTUVMC_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_FASTUVMC_SHIFT);

      /* Set EXTENDED_MV */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_EXTENDED_MV_MASK << BMUXlib_File_ASF_P_CodecData_VC1_EXTENDED_MV_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bExtendedMotionVector ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_EXTENDED_MV_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_EXTENDED_MV_SHIFT);

      /* Set DQUANT */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_DQUANT_MASK << BMUXlib_File_ASF_P_CodecData_VC1_DQUANT_SHIFT);
      uiCodecData |= ((pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.uiMacroblockQuantization & BMUXlib_File_ASF_P_CodecData_VC1_DQUANT_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_DQUANT_SHIFT);

      /* Set VSTRANSFORM */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_VSTRANSFORM_MASK << BMUXlib_File_ASF_P_CodecData_VC1_VSTRANSFORM_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bVariableSizedTransform ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_VSTRANSFORM_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_VSTRANSFORM_SHIFT);

      /* Set RESERVED5 */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_MASK << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_SHIFT);
      uiCodecData |= ((BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_DEFAULT & BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED5_SHIFT);

      /* Set OVERLAP */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_OVERLAP_MASK << BMUXlib_File_ASF_P_CodecData_VC1_OVERLAP_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bOverlappedTransform ? 1 : 0 )) << BMUXlib_File_ASF_P_CodecData_VC1_OVERLAP_SHIFT);

      /* Set SYNCMARKER */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_SYNCMARKER_MASK << BMUXlib_File_ASF_P_CodecData_VC1_SYNCMARKER_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bSyncMarker ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_SYNCMARKER_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_SYNCMARKER_SHIFT);

      /* Set RANGERED */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_RANGERED_MASK << BMUXlib_File_ASF_P_CodecData_VC1_RANGERED_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bRangeReduction ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_RANGERED_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_RANGERED_SHIFT);

      /* Set MAXBFRAMES */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_MAXBFRAMES_MASK << BMUXlib_File_ASF_P_CodecData_VC1_MAXBFRAMES_SHIFT);
      uiCodecData |= ((pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.uiMaxBFrames & BMUXlib_File_ASF_P_CodecData_VC1_MAXBFRAMES_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_MAXBFRAMES_SHIFT);

      /* Set QUANTIZER */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_QUANTIZER_MASK << BMUXlib_File_ASF_P_CodecData_VC1_QUANTIZER_SHIFT);
      uiCodecData |= ((pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.uiQuantizer & BMUXlib_File_ASF_P_CodecData_VC1_QUANTIZER_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_QUANTIZER_SHIFT);

      /* Set FINTERPFLAG */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_FINTERPFLAG_MASK << BMUXlib_File_ASF_P_CodecData_VC1_FINTERPFLAG_SHIFT);
      uiCodecData |= ((( pInput->stCodecSpecificData.video.data.stCommon.uProtocolData.stVC1.sequenceHeaderC.bFrameInterpolation ? 1 : 0 ) & BMUXlib_File_ASF_P_CodecData_VC1_FINTERPFLAG_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_FINTERPFLAG_SHIFT);

      /* Set RESERVED6 */
      uiCodecData &= ~(BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_MASK << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_SHIFT);
      uiCodecData |= ((BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_DEFAULT & BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_MASK) << BMUXlib_File_ASF_P_CodecData_VC1_RESERVED6_SHIFT);

      BMUXlib_File_ASF_P_Set32_BE( pVC1Data, 0, (uiCodecData) );
   }
}

#define BMUXlib_File_ASF_FormatData_Video_MAXSIZE 40

static const uint8_t s_auiDefaultFormatData_Video[BMUXlib_File_ASF_FormatData_Video_MAXSIZE] =
{
 /* Format Data - BITMAPINFOHEADER */
 0x00, 0x00, 0x00, 0x00, /* Format Data Size */
 0x00, 0x00, 0x00, 0x00, /* Image Width */
 0x00, 0x00, 0x00, 0x00, /* Image Height */
 0x01, 0x00, /* Reserved (Must be 0x01) */
 0x18, 0x00, /* Bits per Pixel Count */
 0x00, 0x00, 0x00, 0x00, /* Compression ID */
 0x00, 0x00, 0x00, 0x00, /* Image Size */
 0x00, 0x00, 0x00, 0x00, /* Horizontal Pixels Per Meter */
 0x00, 0x00, 0x00, 0x00, /* Vertical Pixels Per Meter */
 0x00, 0x00, 0x00, 0x00, /* Colors Used Count */
 0x00, 0x00, 0x00, 0x00, /* Important Colors Used Count */
 /* Codec Specific Data */
};

#define BMUXlib_File_ASF_P_FormatData_Video_SIZE 0
#define BMUXlib_File_ASF_P_FormatData_Video_WIDTH   4
#define BMUXlib_File_ASF_P_FormatData_Video_HEIGHT  8
#define BMUXlib_File_ASF_P_FormatData_Video_COMPRESSIONID 16
#define BMUXlib_File_ASF_P_FormatData_Video_IMAGESIZE 20

#define BMUXlib_File_ASF_P_FormatData_Video_SetDimensions(pHeader, width, height) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_FormatData_Video_WIDTH, (width) ) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_FormatData_Video_HEIGHT, (height) ) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_FormatData_Video_IMAGESIZE, (height*width*3) )

#define BMUXlib_File_ASF_P_FormatData_Video_SetCompressionID(pHeader, pCompressionID) \
   BKNI_Memcpy( (pHeader) + BMUXlib_File_ASF_P_FormatData_Video_COMPRESSIONID, \
                pCompressionID, \
                4 \
                );

#define BMUXlib_File_ASF_P_FormatData_Video_SetSize(pHeader, size) \
         BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_FormatData_Video_SIZE, (size) )

void
BMUXlib_File_ASF_P_Header_SetFormatData_Video(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiFormatDataStartOffset = *puiCurrentOffset;
   uint8_t *pFormatDataVideo = hASFMux->pHeaderBuffer + uiFormatDataStartOffset;
   uint32_t uiWidth = pInput->stCodecSpecificData.video.data.stCommon.stDimension.coded.uiWidth;
   uint32_t uiHeight = pInput->stCodecSpecificData.video.data.stCommon.stDimension.coded.uiHeight;
   /* Set Type Specific Data */
   BDBG_ASSERT( ( uiFormatDataStartOffset + BMUXlib_File_ASF_FormatData_Video_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );
   BKNI_Memcpy( pFormatDataVideo,
                s_auiDefaultFormatData_Video,
                BMUXlib_File_ASF_FormatData_Video_MAXSIZE
                );
   *puiCurrentOffset += BMUXlib_File_ASF_FormatData_Video_MAXSIZE;

   /* Set Dimensions */
   BMUXlib_File_ASF_P_FormatData_Video_SetDimensions( pFormatDataVideo, uiWidth, uiHeight );

   switch ( pInput->bufferInfo.pstVideo->eProtocol )
   {
      case BAVC_VideoCompressionStd_eVC1SimpleMain:
         /* Set Compression ID */
         BMUXlib_File_ASF_P_FormatData_Video_SetCompressionID(
                  pFormatDataVideo,
                  s_auiCompressionID_Video_VC1
                  );

         BMUXlib_File_ASF_P_Header_SetCodecData_VC1(
                  hASFMux,
                  pInput,
                  puiCurrentOffset
                  );

         /* Set Format Data Size */
         BMUXlib_File_ASF_P_FormatData_Video_SetSize( pFormatDataVideo,
                                                                             (*puiCurrentOffset - uiFormatDataStartOffset)
                                                                             );
         break;
      case BAVC_VideoCompressionStd_eH264:
         /* Set Compression ID */
         BMUXlib_File_ASF_P_FormatData_Video_SetCompressionID(
                  pFormatDataVideo,
                  s_auiCompressionID_Video_H264
                  );

         /* Set Format Data Size */
         BMUXlib_File_ASF_P_FormatData_Video_SetSize( pFormatDataVideo,
                                                      (*puiCurrentOffset - uiFormatDataStartOffset)
                                                      );
         break;
      default:
         BDBG_ERR(("Unsupported video protocol (%d)", pInput->bufferInfo.pstVideo->eProtocol));
         break;
   }
}

#define BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Video_MAXSIZE 11
static const uint8_t s_auiDefaultStreamProperties_TypeSpecificData_Video[BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Video_MAXSIZE] =
{
 0x00, 0x00, 0x00, 0x00, /* Encoded Image Width */
 0x00, 0x00, 0x00, 0x00, /* Encoded Image Height */
 0x02, /* Reserved (Must be 0x02) */
 0x00, 0x00, /* Format Data Size */
 /* Format Data */
};

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_WIDTH   0
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_HEIGHT  4
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_FORMATDATASIZE 9

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_SetDimensions(pHeader, width, height) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_WIDTH, (width) ) \
   BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_HEIGHT, (height) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_SetFormatDataSize(pHeader, size) \
         BMUXlib_File_ASF_P_Set16_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_FORMATDATASIZE, (size) )

void
BMUXlib_File_ASF_P_Header_SetStreamProperties_TypeSpecificData_Video(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiVideoStartOffset = *puiCurrentOffset;
   uint8_t *pStreamPropertiesVideo = hASFMux->pHeaderBuffer + uiVideoStartOffset;
   uint32_t uiWidth = pInput->stCodecSpecificData.video.data.stCommon.stDimension.coded.uiWidth;
   uint32_t uiHeight = pInput->stCodecSpecificData.video.data.stCommon.stDimension.coded.uiHeight;

   /* Set Type Specific Data */
   BDBG_ASSERT( ( uiVideoStartOffset + BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Video_MAXSIZE ) <= hASFMux->uiHeaderBufferSize );
   BKNI_Memcpy( pStreamPropertiesVideo,
                s_auiDefaultStreamProperties_TypeSpecificData_Video,
                BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Video_MAXSIZE
                );
   *puiCurrentOffset += BMUXlib_File_ASF_StreamProperties_TypeSpecificData_Video_MAXSIZE;

   /* Set Dimensions */
   BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_SetDimensions( pStreamPropertiesVideo, uiWidth, uiHeight );

   {
      unsigned uiFormatDataStartOffset = *puiCurrentOffset;

      BMUXlib_File_ASF_P_Header_SetFormatData_Video(
               hASFMux,
               pInput,
               puiCurrentOffset
               );

      /* Set Format Data Size */
      BMUXlib_File_ASF_P_StreamPropertiesObject_TypeSpecificData_Video_SetFormatDataSize( pStreamPropertiesVideo,
                                                                                          (*puiCurrentOffset - uiFormatDataStartOffset)
                                                                             );
   }
}


#define BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE 78

static const uint8_t s_auiDefaultStreamPropertiesObjectVideo[BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE] =
{
 /* Object ID (Stream Properties) */
 0x91, 0x07, 0xDC, 0xB7,
 0xB7, 0xA9,
 0xCF, 0x11,
 0x8E, 0xE6,
 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,
 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Object Size (78 bytes) */
 /* Stream Type (Video) */
 0xC0, 0xEF, 0x19, 0xBC,
 0x4D, 0x5B,
 0xCF, 0x11,
 0xA8, 0xFD,
 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B,
 /* Error Correction Type (None) */
 0x00, 0x57, 0xFB, 0x20,
 0x55, 0x5B,
 0xCF, 0x11,
 0xA8, 0xFD,
 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Time Offset */
 0x00, 0x00, 0x00, 0x00, /* Type-Specific Data Length */
 0x00, 0x00, 0x00, 0x00, /* Error Correction Data Length */
 0x00, 0x00, /* Flags */
 0x00, 0x00, 0x00, 0x00, /* Reserved */
 /* Type Specific Data (varies) */ /* TODO */
 /* Error Correction Data (varies) */
};

static const uint8_t s_auiDefaultStreamPropertiesObjectAudio[BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE] =
{
 /* Object ID (Stream Properties) */
 0x91, 0x07, 0xDC, 0xB7,
 0xB7, 0xA9,
 0xCF, 0x11,
 0x8E, 0xE6,
 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,
 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Object Size (78 bytes) */
 /* Stream Type (Audio) */
 0x40, 0x9E, 0x69, 0xF8,
 0x4D, 0x5B,
 0xCF, 0x11,
 0xA8, 0xFD,
 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B,
#if 0
 /* Error Correction Type (Audio Spread) */
 0x50, 0xCD, 0xC3, 0xBF,
 0x8F, 0x61,
 0xCF, 0x11,
 0x8B, 0xB2,
 0x00, 0xAA, 0x00, 0xB4, 0xE2, 0x20,
#else
 /* Error Correction Type (None) */
 0x00, 0x57, 0xFB, 0x20,
 0x55, 0x5B,
 0xCF, 0x11,
 0xA8, 0xFD,
 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B,
#endif
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Time Offset */
 0x00, 0x00, 0x00, 0x00, /* Type-Specific Data Length */
 0x00, 0x00, 0x00, 0x00, /* Error Correction Data Length */
 0x00, 0x00, /* Flags */
 0x00, 0x00, 0x00, 0x00, /* Reserved */
 /* Type Specific Data (varies) */ /* TODO */
 /* Error Correction Data (varies) */ /* TODO */
};

#define BMUXlib_File_ASF_P_StreamPropertiesObject_SIZE  16
#define BMUXlib_File_ASF_P_StreamPropertiesObject_TYPESPECIFICDATALENGTH   64
#define BMUXlib_File_ASF_P_StreamPropertiesObject_ERRORCORRECTIONDATALENGTH   68
#define BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS 72
#define BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_OFFSET 0
#define BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_SHIFT 0
#define BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_MASK 0x7F

#define BMUXlib_File_ASF_P_StreamPropertiesObject_SetTypeSpecificDataLength( pHeader, length ) \
         BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_TYPESPECIFICDATALENGTH, (length) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_SetErrorCorrectionDataLength( pHeader, length ) \
         BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_ERRORCORRECTIONDATALENGTH, (length) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_SetSize( pHeader, size ) \
         BMUXlib_File_ASF_P_Set64_LE( (pHeader), BMUXlib_File_ASF_P_StreamPropertiesObject_SIZE, (size) )

#define BMUXlib_File_ASF_P_StreamPropertiesObject_SetStreamNumber( pHeader, number ) \
         ((uint8_t*)(pHeader))[BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS + BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_OFFSET] &= ~BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_MASK; \
         ((uint8_t*)(pHeader))[BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS + BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_OFFSET] |= ((number << BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_SHIFT) & BMUXlib_File_ASF_P_StreamPropertiesObject_FLAGS_STREAMNUMBER_MASK)

void
BMUXlib_File_ASF_P_Header_SetStreamProperties(
         BMUXlib_File_ASF_Handle hASFMux,
         BMUXlib_File_ASF_P_Input *pInput,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiStreamStartOffset = *puiCurrentOffset;
   uint8_t *pStreamProperties = hASFMux->pHeaderBuffer + uiStreamStartOffset;
   uint64_t uiSize = 0;

   BDBG_ASSERT( ( uiStreamStartOffset + BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE) <= hASFMux->uiHeaderBufferSize );

   switch ( pInput->eType )
   {
      case BMUXlib_Input_Type_eVideo:
      {
         BKNI_Memcpy( pStreamProperties,
                      s_auiDefaultStreamPropertiesObjectVideo,
                      BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE
                      );
         *puiCurrentOffset += BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE;

         {
            unsigned uiTypeSpecificDataStartOffset = *puiCurrentOffset;

            BMUXlib_File_ASF_P_Header_SetStreamProperties_TypeSpecificData_Video(
                     hASFMux,
                     pInput,
                     puiCurrentOffset
                     );

            /* Set Type Specific Data Length */
            BMUXlib_File_ASF_P_StreamPropertiesObject_SetTypeSpecificDataLength( pStreamProperties,
                                                                                   (*puiCurrentOffset - uiTypeSpecificDataStartOffset)
                                                                                  );
         }
      }
         break;

      case BMUXlib_Input_Type_eAudio:
      {
         BKNI_Memcpy( pStreamProperties,
                      s_auiDefaultStreamPropertiesObjectAudio,
                      BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE
                      );
         *puiCurrentOffset += BMUXlib_File_ASF_StreamPropertiesObject_MAXSIZE;

         {
            unsigned uiTypeSpecificDataStartOffset = *puiCurrentOffset;

            BMUXlib_File_ASF_P_Header_SetStreamProperties_TypeSpecificData_Audio(
                     hASFMux,
                     pInput,
                     puiCurrentOffset
                     );

            /* Set Type Specific Data Length */
            BMUXlib_File_ASF_P_StreamPropertiesObject_SetTypeSpecificDataLength( pStreamProperties,
                                                                                   (*puiCurrentOffset - uiTypeSpecificDataStartOffset)
                                                                                  );
         }

         {
            unsigned uiErrorCorrectionDataStartOffset = *puiCurrentOffset;

            BMUXlib_File_ASF_P_Header_SetStreamProperties_ErrorCorrectionData_Audio(
                     hASFMux,
                     pInput,
                     puiCurrentOffset
                     );

            /* Set Type Specific Data Length */
            BMUXlib_File_ASF_P_StreamPropertiesObject_SetErrorCorrectionDataLength( pStreamProperties,
                                                                                   (*puiCurrentOffset - uiErrorCorrectionDataStartOffset)
                                                                                  );
         }
      }
         break;

      default:
         BDBG_ERR(("Unknown input type"));
   }

   /* Set Stream Number */
   BMUXlib_File_ASF_P_StreamPropertiesObject_SetStreamNumber( pStreamProperties,
                                                                pInput->uiStreamNumber
                                                                );

   /* Update Stream Properties w/ Size */
   uiSize = *puiCurrentOffset - uiStreamStartOffset;
   BMUXlib_File_ASF_P_StreamPropertiesObject_SetSize( pStreamProperties, uiSize );
}

/* File Properties Object (Mandatory, one only) */
#define BMUXlib_File_ASF_FilePropertiesObject_MAXSIZE 104

static const uint8_t s_auiDefaultFilePropertiesObject[BMUXlib_File_ASF_FilePropertiesObject_MAXSIZE] =
{
 0xA1, 0xDC, 0xAB, 0x8C, /* Object ID (File Properties) */
 0x47, 0xA9,
 0xCF, 0x11,
 0x8E, 0xE4,
 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,
 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Object Size (104 bytes) */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* File ID */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* File Size */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Creation Date */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Data Packets Count */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Play Duration */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Send Duration */
 0xD0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Preroll ( 2 seconds ) */
 0x01, 0x00, 0x00, 0x00, /* Flags () */
 0x00, 0x00, 0x00, 0x00, /* Min Data Packet Size */
 0x00, 0x00, 0x00, 0x00, /* Max Data Packet Size */
 0x00, 0x00, 0x00, 0x00, /* Max Bitrate */
};

#define BMUXlib_File_ASF_P_FilePropertiesObject_FILEID_OFFSET 24
#define BMUXlib_File_ASF_P_FilePropertiesObject_FILESIZE_OFFSET 40
#define BMUXlib_File_ASF_P_FilePropertiesObject_CREATIONDATE_OFFSET 48
#define BMUXlib_File_ASF_P_FilePropertiesObject_DATAPACKETSCOUNT_OFFSET 56
#define BMUXlib_File_ASF_P_FilePropertiesObject_PLAYDURATION_OFFSET 64
#define BMUXlib_File_ASF_P_FilePropertiesObject_SENDDURATION_OFFSET 72
#define BMUXlib_File_ASF_P_FilePropertiesObject_PREROLL_OFFSET 80
#define BMUXlib_File_ASF_P_FilePropertiesObject_FLAGS_OFFSET 88
#define BMUXlib_File_ASF_P_FilePropertiesObject_MINPACKETSIZE_OFFSET 92
#define BMUXlib_File_ASF_P_FilePropertiesObject_MAXPACKETSIZE_OFFSET 96
#define BMUXlib_File_ASF_P_FilePropertiesObject_MAXBITRATE_OFFSET 100

#define BMUXlib_File_ASF_P_FilePropertiesObject_SetFileID( pHeader, pFileId ) \
         BKNI_Memcpy( pHeader + BMUXlib_File_ASF_P_FilePropertiesObject_FILEID_OFFSET, \
                      pFileId, \
                      BMUXLIB_FILE_ASF_FILE_ID_LENGTH \
                      );

#define BMUXlib_File_ASF_P_FilePropertiesObject_SetMinPacketSize(pHeader, size) BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_FilePropertiesObject_MINPACKETSIZE_OFFSET, (size) )
#define BMUXlib_File_ASF_P_FilePropertiesObject_SetMaxPacketSize(pHeader, size) BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_FilePropertiesObject_MAXPACKETSIZE_OFFSET, (size) )
#define BMUXlib_File_ASF_P_FilePropertiesObject_SetPreroll(pHeader, preroll) BMUXlib_File_ASF_P_Set64_LE( (pHeader), BMUXlib_File_ASF_P_FilePropertiesObject_PREROLL_OFFSET, (preroll) )
#define BMUXlib_File_ASF_P_FilePropertiesObject_SetSeekableFlag( pHeader, seekable ) \
         ((uint8_t*)(pHeader))[BMUXlib_File_ASF_P_FilePropertiesObject_FLAGS_OFFSET+0] &= (uint8_t) ~0x02; \
         ((uint8_t*)(pHeader))[BMUXlib_File_ASF_P_FilePropertiesObject_FLAGS_OFFSET+0] |= (uint8_t) ( (seekable << 1) & 0x02 );

void
BMUXlib_File_ASF_P_Header_SetFileProperties(
         BMUXlib_File_ASF_Handle hASFMux,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiFileStartOffset = *puiCurrentOffset;
   uint8_t *pFileProperties = hASFMux->pHeaderBuffer + uiFileStartOffset;

   BDBG_ASSERT( ( uiFileStartOffset + BMUXlib_File_ASF_FilePropertiesObject_MAXSIZE) <= hASFMux->uiHeaderBufferSize );

   BKNI_Memcpy( pFileProperties,
                s_auiDefaultFilePropertiesObject,
                BMUXlib_File_ASF_FilePropertiesObject_MAXSIZE
                );

   *puiCurrentOffset += BMUXlib_File_ASF_FilePropertiesObject_MAXSIZE;

   /* Set File ID */
   BMUXlib_File_ASF_P_FilePropertiesObject_SetFileID( pFileProperties, hASFMux->status.stStartSettings.auiFileId );

   /* Set Pre-Roll */
   {
      uint64_t uiPreroll = BMUXlib_File_ASF_P_FilePropertiesObject_PREROLL_DEFAULT ;
      BMUXlib_File_ASF_P_FilePropertiesObject_SetPreroll( pFileProperties, uiPreroll );
   }

   /* Set Min Packet Size */
   BMUXlib_File_ASF_P_FilePropertiesObject_SetMinPacketSize( pFileProperties, hASFMux->stCreateSettings.uiPacketLength );

   /* Set Max Packet Size */
   BMUXlib_File_ASF_P_FilePropertiesObject_SetMaxPacketSize( pFileProperties, hASFMux->stCreateSettings.uiPacketLength );

   /* TODO: Set Max bitrate */

   /* Set Seekable Flag */
   if ( ( 1 == hASFMux->status.uiNumActiveInputs )
        && ( BMUXlib_Input_Type_eAudio == hASFMux->status.astActiveInputs[0].eType )
      )
   {
      /* Set Seekable Flag */
      BMUXlib_File_ASF_P_FilePropertiesObject_SetSeekableFlag( pFileProperties, 1 );
   }

   BKNI_Memcpy(
            hASFMux->status.stStats.auiFileFlags,
            (void*) ((unsigned) pFileProperties + BMUXlib_File_ASF_P_FilePropertiesObject_FLAGS_OFFSET),
            4
            );

   hASFMux->status.stOffsets.uiFileProperties = uiFileStartOffset;
}

/* Header Object (Mandatory, one only) */
#define BMUXlib_File_ASF_HeaderObject_MAXSIZE 30

static const uint8_t s_auiDefaultHeaderObject[BMUXlib_File_ASF_HeaderObject_MAXSIZE] =
{
 /* Object ID (Header) */
 0x30, 0x26, 0xB2, 0x75,
 0x8E, 0x66,
 0xCF, 0x11,
 0xA6, 0xD9,
 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Object Size (Must be filled in after header sub-objects are complete) */
 0x00, 0x00, 0x00, 0x00, /* Number of Header Objects (Must be filled in after header sub-objects are complete) */
 0x01, /* Reserved 1 (Must be 0x01) */
 0x02 /* Reserved 2 (Must be 0x02) */
};

#define BMUXlib_File_ASF_P_HeaderObject_OBJECTSIZE_OFFSET 16
#define BMUXlib_File_ASF_P_HeaderObject_NUMOBJECTS_OFFSET 24

#define BMUXlib_File_ASF_P_HeaderObject_SetSize(pHeader, size) BMUXlib_File_ASF_P_Set64_LE( (pHeader), BMUXlib_File_ASF_P_HeaderObject_OBJECTSIZE_OFFSET, (size) )
#define BMUXlib_File_ASF_P_HeaderObject_SetNumObjects(pHeader, count) BMUXlib_File_ASF_P_Set32_LE( (pHeader), BMUXlib_File_ASF_P_HeaderObject_NUMOBJECTS_OFFSET, (count) )

/* Data Object (Mandatory, one only) */
#define BMUXlib_File_ASF_DataObject_MAXSIZE 50

static const uint8_t s_auiDefaultDataObject[BMUXlib_File_ASF_DataObject_MAXSIZE] =
{
 /* Object ID (Data) */
 0x36, 0x26, 0xB2, 0x75,
 0x8E, 0x66,
 0xCF, 0x11,
 0xA6, 0xD9,
 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C,
 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Object Size (50 bytes) */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* File ID */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Total Data Packets */
 0x01,0x01, /* Reserved */
 /* Data Packets */
};

#define BMUXlib_File_ASF_P_DataObject_OBJECTSIZE_OFFSET 16
#define BMUXlib_File_ASF_P_DataObject_FILEID_OFFSET 24
#define BMUXlib_File_ASF_P_DataObject_TOTALDATAPACKETS_OFFSET 40

#define BMUXlib_File_ASF_P_DataObject_SetFileID( pHeader, pFileId ) \
         BKNI_Memcpy( pHeader + BMUXlib_File_ASF_P_DataObject_FILEID_OFFSET, \
                      pFileId, \
                      BMUXLIB_FILE_ASF_FILE_ID_LENGTH \
                      );

void
BMUXlib_File_ASF_P_Header_SetDataObject(
         BMUXlib_File_ASF_Handle hASFMux,
         unsigned *puiCurrentOffset
         )
{
   unsigned uiDataStartOffset = *puiCurrentOffset;
   uint8_t *pData= hASFMux->pHeaderBuffer + uiDataStartOffset;

   BDBG_ASSERT( ( uiDataStartOffset + BMUXlib_File_ASF_DataObject_MAXSIZE) <= hASFMux->uiHeaderBufferSize );

   BKNI_Memcpy( pData,
                s_auiDefaultDataObject,
                BMUXlib_File_ASF_DataObject_MAXSIZE
                );

   *puiCurrentOffset += BMUXlib_File_ASF_DataObject_MAXSIZE;

   /* Set File ID */
   BMUXlib_File_ASF_P_DataObject_SetFileID( pData, hASFMux->status.stStartSettings.auiFileId );

   hASFMux->status.stOffsets.uiDataObject = uiDataStartOffset;
}

/* BMUXlib_File_ASF_SetHeader - will return true when the header has been written
 */
bool
BMUXlib_File_ASF_P_Header_WriteHeader(
         BMUXlib_File_ASF_Handle hASFMux
         )
{
   unsigned uiHeaderStartOffset = 0;
   uint8_t *pHeader = hASFMux->pHeaderBuffer + uiHeaderStartOffset;
   unsigned uiCurrentOffset = uiHeaderStartOffset;
   unsigned uiNumObjects = 0;
   uint64_t uiSize = 0;

   BDBG_ASSERT( ( uiHeaderStartOffset + BMUXlib_File_ASF_HeaderObject_MAXSIZE) <= hASFMux->uiHeaderBufferSize );

   BKNI_Memcpy( pHeader,
                s_auiDefaultHeaderObject,
                BMUXlib_File_ASF_HeaderObject_MAXSIZE
                );

   uiCurrentOffset += BMUXlib_File_ASF_HeaderObject_MAXSIZE;

   /* Write File Properties */
   BMUXlib_File_ASF_P_Header_SetFileProperties( hASFMux, &uiCurrentOffset);
   uiNumObjects++;

   /* Write Stream Properties (for each active stream) */
   {
      unsigned uiInputIndex;

      for ( uiInputIndex = 0; ( uiInputIndex < hASFMux->status.uiNumActiveInputs ); uiInputIndex++ )
      {
         BMUXlib_File_ASF_P_Input *pInput = &hASFMux->status.astActiveInputs[uiInputIndex];

         BMUXlib_File_ASF_P_Header_SetStreamProperties( hASFMux, pInput, &uiCurrentOffset);
         uiNumObjects++;
      }
   }

   /* Write Header Extension */
   BMUXlib_File_ASF_P_Header_SetHeaderExtension( hASFMux, &uiCurrentOffset);
   uiNumObjects++;

   /* Update Header w/ Size */
   uiSize = uiCurrentOffset - uiHeaderStartOffset;
   BMUXlib_File_ASF_P_HeaderObject_SetSize( pHeader, uiSize );

   /* Update Header w/ Object Count */
   BMUXlib_File_ASF_P_HeaderObject_SetNumObjects( pHeader, uiNumObjects );

   /* Write Data Object */
   BMUXlib_File_ASF_P_Header_SetDataObject( hASFMux, &uiCurrentOffset);

   /* Write header to output */
   {
      BMUXlib_Output_Descriptor stOutputDescriptor;

      BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

      stOutputDescriptor.stStorage.bWriteOperation = true;
      stOutputDescriptor.stStorage.pBufferAddress = pHeader;
      stOutputDescriptor.stStorage.uiLength = uiCurrentOffset;
      stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stStats.uiFileSize;

      hASFMux->status.stStats.uiFileSize += stOutputDescriptor.stStorage.uiLength;

      BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
               hASFMux->hOutput,
               &stOutputDescriptor,
               NULL
               ));
   }
#if 0
   {
      unsigned i;
      for ( i = 0; i < uiCurrentOffset; i++ )
      {
         if ( i && ( 0 == (i % 16)) ) BKNI_Printf("\n");
         BKNI_Printf("%02x ", pHeader[i]);
      }

#include <stdio.h>
      FILE *f = fopen("test.asf", "wb");
      fwrite(pHeader, 1, uiCurrentOffset, f);
      fclose(f);
   }
#endif

   return true;
}

bool
BMUXlib_File_ASF_P_Header_PostProcessHeader(
         BMUXlib_File_ASF_Handle hASFMux
         )
{
   bool bResult = true;
   bool bBlocked = false;

   BDBG_ENTER( BMUXlib_File_ASF_P_Header_PostProcessHeader );

   while ( ( false == bBlocked ) && ( true == bResult ) )
   {
      switch ( hASFMux->status.ePostProcessState )
      {
         case BMUXlib_File_ASF_P_PostProcess_State_eStart:
            BMUXlib_File_ASF_P_Set64_LE(
                     hASFMux->status.stStats.auiFileSize,
                     0,
                     hASFMux->status.stStats.uiFileSize
                     );

            BMUXlib_File_ASF_P_Set64_LE(
                     hASFMux->status.stStats.auiDataPacketsCount,
                     0,
                     hASFMux->status.stStats.uiDataPacketsCount
                     );

            BMUXlib_File_ASF_P_Set64_LE(
                     hASFMux->status.stStats.auiPlayDuration,
                     0,
                     hASFMux->status.stStats.uiPlayDuration
                     );

            BMUXlib_File_ASF_P_Set64_LE(
                     hASFMux->status.stStats.auiSendDuration,
                     0,
                     hASFMux->status.stStats.uiSendDuration
                     );

            BMUXlib_File_ASF_P_Set64_LE(
                     hASFMux->status.stStats.auiDataObjectSize,
                     0,
                     hASFMux->status.stStats.uiFileSize - hASFMux->status.stOffsets.uiDataObject
                     );

            hASFMux->status.ePostProcessState++;
            BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            break;
         /* File Properties */
         case BMUXlib_File_ASF_P_PostProcess_State_eFileSize:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiFileSize;
               stOutputDescriptor.stStorage.uiLength = 8;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiFileProperties + BMUXlib_File_ASF_P_FilePropertiesObject_FILESIZE_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_PostProcess_State_eDataPacketsCount_File:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiDataPacketsCount;
               stOutputDescriptor.stStorage.uiLength = 8;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiFileProperties + BMUXlib_File_ASF_P_FilePropertiesObject_DATAPACKETSCOUNT_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_PostProcess_State_ePlayDuration:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiPlayDuration;
               stOutputDescriptor.stStorage.uiLength = 8;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiFileProperties + BMUXlib_File_ASF_P_FilePropertiesObject_PLAYDURATION_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_PostProcess_State_eSendDuration:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiSendDuration;
               stOutputDescriptor.stStorage.uiLength = 8;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiFileProperties + BMUXlib_File_ASF_P_FilePropertiesObject_SENDDURATION_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_PostProcess_State_eBroadcastFlag:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               hASFMux->status.stStats.auiFileFlags[0] &= 0xFE;

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiFileFlags;
               stOutputDescriptor.stStorage.uiLength = 4;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiFileProperties + BMUXlib_File_ASF_P_FilePropertiesObject_FLAGS_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         /* Data Object */
         case BMUXlib_File_ASF_P_PostProcess_State_eDataObjectSize:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiDataObjectSize;
               stOutputDescriptor.stStorage.uiLength = 8;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiDataObject + BMUXlib_File_ASF_P_DataObject_OBJECTSIZE_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_PostProcess_State_eTotalDataPackets:
            if ( true == BMUXlib_Output_IsSpaceAvailable( hASFMux->hOutput ) )
            {
               BMUXlib_Output_Descriptor stOutputDescriptor;
               /* Queue to Output */
               BKNI_Memset( &stOutputDescriptor, 0, sizeof( BMUXlib_Output_Descriptor ) );

               stOutputDescriptor.stStorage.bWriteOperation = true;
               stOutputDescriptor.stStorage.pBufferAddress = hASFMux->status.stStats.auiDataPacketsCount;
               stOutputDescriptor.stStorage.uiLength = 8;
               stOutputDescriptor.stStorage.uiOffset = hASFMux->status.stOffsets.uiDataObject + BMUXlib_File_ASF_P_DataObject_TOTALDATAPACKETS_OFFSET;

               BERR_TRACE( BMUXlib_Output_AddNewDescriptor(
                        hASFMux->hOutput,
                        &stOutputDescriptor,
                        NULL
               ));

               hASFMux->status.ePostProcessState++;
               BDBG_MSG(("Transitioning to Post Process State: %d", hASFMux->status.ePostProcessState));
            }
            else
            {
               bBlocked = true;
            }
            break;

         case BMUXlib_File_ASF_P_PostProcess_State_eDone:
            bResult = false;
            break;
      }
   }

   BDBG_LEAVE( BMUXlib_File_ASF_P_Header_PostProcessHeader );

   return bResult;
}
