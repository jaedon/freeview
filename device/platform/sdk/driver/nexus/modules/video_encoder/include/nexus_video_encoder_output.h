/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_encoder_output.h $
 * $brcm_Revision: 16 $
 * $brcm_Date: 7/25/12 5:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/include/nexus_video_encoder_output.h $
 * 
 * 16   7/25/12 5:34p vsilyaev
 * SW7425-3357: Fixed typo
 * 
 * 15   7/23/12 11:51a vsilyaev
 * SW7425-3357: Updated to match AVC module
 * 
 * 14   6/8/12 12:17p vsilyaev
 * SW7425-3156: Added ClearStatus API
 * 
 * 13   5/18/12 1:26p vsilyaev
 * SW7425-3088: Fixed description of NEXUS_VideoEncoder_GetBuffer
 * 
 * 12   4/17/12 3:34p vsilyaev
 * SW7425-2875: Use originalPts instead of originalPTS
 * 
 * 11   4/16/12 11:25a vsilyaev
 * SW7425-2875: Updated layout of the encoder descriptor
 * 
 * 10   10/24/11 2:00p vsilyaev
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 * 
 * SW7425-1580/1   10/24/11 4:48p nilesh
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 * 
 * 9   9/2/11 3:48p vsilyaev
 * SW7425-1225: Added video encoder descriptor flags
 * 
 * 8   4/15/11 6:05p vsilyaev
 * SW7425-232: Add proper attributes
 * 
 * 7   4/14/11 11:14a vsilyaev
 * SW7425-75: Propagate pMetadataBufferBaseAddress through from nexus
 *  video encoder to the muxes
 * 
 * 6   4/11/11 12:22p vsilyaev
 * SW7425-232: Updated to the latest vce code
 * 
 * 5   4/8/11 1:04p vsilyaev
 * SW7425-232: Updated to the latest mux code
 * 
 * 4   3/29/11 3:47p vsilyaev
 * SW7425-74: Added unitDataType
 * 
 * 3   2/7/11 9:41a vsilyaev
 * SW7425-40: Updated to match latest BVCE
 * 
 * 2   12/16/10 7:34p vsilyaev
 * SW7425-40: Added padding to the NEXUS_VideoEncoderDescriptor
 * 
 * 1   12/9/10 8:11p vsilyaev
 * SW7425-40: Video Encoder module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_ENCODER_OUTPUT_H__
#define NEXUS_VIDEO_ENCODER_OUTPUT_H__

#include "nexus_video_encoder_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
Summary:
Data returned by NEXUS_VideoEncoder_GetBuffer

Description:
This must match BAVC_VideoBufferDescriptor.
**/
typedef struct NEXUS_VideoEncoderDescriptor
{
    uint32_t flags;

    /* Timestamp Parameters */
    uint32_t originalPts; /* 32-bit original PTS value (in 45 KHz) */
    uint64_t pts; /* 33-bit PTS value (in 90 KHz) */
    uint64_t stcSnapshot; /* 42-bit STC snapshot when frame received by the encoder (in 27MHz) */

    /* Transmission Parameters */
    uint32_t escr; /* Expected mux transmission start time (in 27 MHz) */
    uint16_t ticksPerBit;
    int16_t shr;

    /* Buffer Parameters */
    unsigned offset; /* offset of frame data from frame buffer base address (in bytes) */
    size_t length;   /* 0 if fragment is empty, e.g. for EOS entry (in bytes) */
    unsigned encoderIndex;
    unsigned unused;

    uint32_t videoFlags;

    uint64_t dts; /* 33-bit DTS value (in 90 Khz) */

    uint8_t dataUnitType;

} NEXUS_VideoEncoderDescriptor;

/**
Summary:
Flags for the NEXUS_VideoEncoderDescriptor.flags field 
**/
 #define NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_ORIGINALPTS_VALID 0x00000001
 #define NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_PTS_VALID 0x00000002
 #define NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_FRAME_START 0x00010000
 #define NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_EOS 0x00020000
 #define NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_METADATA 0x40000000

/**
Summary:
Flags for the NEXUS_VideoEncoderDescriptor.videoFlags field 
**/
 #define NEXUS_VIDEOENCODERDESCRIPTOR_VIDEOFLAG_DTS_VALID 0x00000001
 #define NEXUS_VIDEOENCODERDESCRIPTOR_VIDEOFLAG_RAP 0x00010000
/**
Description:
 This flags indicates that a video data unit (NALU, EBDU, etc) starts at the beginning of
 this descriptor - if this is set, then the dataUnitType field is valid also 
**/
 #define NEXUS_VIDEOENCODERDESCRIPTOR_VIDEOFLAG_DATA_UNIT_START 0x00020000
 

/**
Summary:
The mux manager (or other consumer) will call this API to get encoded pictures from the NEXUS_VideoEncoder
**/
NEXUS_Error NEXUS_VideoEncoder_GetBuffer(
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderDescriptor **pBuffer, /* [out] attr{memory=cached} pointer to NEXUS_VideoEncoderDescriptor structs */
    size_t *pSize, /* [out] number of NEXUS_VideoEncoderPicture elements in pBuffer */
    const NEXUS_VideoEncoderDescriptor **pBuffer2, /* [out] attr{memory=cached} pointer to NEXUS_VideoEncoderDescriptor structs after wrap around */
    size_t *pSize2 /* [out] number of NEXUS_VideoEncoderPicture elements in pBuffer2 */
    );

/**
Summary:
Report how much data returned by last NEXUS_VideoEncoder_GetBuffer call was consumed
**/
NEXUS_Error NEXUS_VideoEncoder_ReadComplete(
    NEXUS_VideoEncoderHandle handle,
    unsigned descriptorsCompleted /* must be <= pSize+pSize2 returned by last NEXUS_VideoEncoder_GetPictures call. */
    );


#define NEXUS_VIDEOENCODER_ERROR_INVALID_INPUT_DIMENSION          0x00000001
#define NEXUS_VIDEOENCODER_ERROR_USER_DATA_LATE                   0x00000002
#define NEXUS_VIDEOENCODER_ERROR_USER_DATA_DUPLICATE              0x00000004
#define NEXUS_VIDEOENCODER_ERROR_ADJUSTS_WRONG_FRAME_RATE         0x00000008
#define NEXUS_VIDEOENCODER_ERROR_UNSUPPORTED_BVN_FRAME_RATE       0x00000010
#define NEXUS_VIDEOENCODER_ERROR_UNSUPPORTED_RESOLUTION           0x00000020
#define NEXUS_VIDEOENCODER_ERROR_MISMATCH_BVN_MIN_FRAME_RATE      0x00000040
#define NEXUS_VIDEOENCODER_ERROR_MISMATCH_BVN_PIC_RESOLUTION      0x00000080
#define NEXUS_VIDEOENCODER_ERROR_MAX_BITRATE_EXCEEDED             0x00000100
#define NEXUS_VIDEOENCODER_ERROR_BIN_BUFFER_FULL                  0x00000200
#define NEXUS_VIDEOENCODER_ERROR_CDB_FULL                         0x00000400
#define NEXUS_VIDEOENCODER_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL 0x00000800
#define NEXUS_VIDEOENCODER_ERROR_EBM_FULL                         0x00001000

#define NEXUS_VIDEOENCODER_EVENT_INPUT_CHANGE                     0x00000001


/**
Summary:
**/
typedef struct NEXUS_VideoEncoderStatus
{
    const void *bufferBase; /* attr{memory=cached} base pointer for NEXUS_VideoEncoderPicture.offset */
    const void *metadataBufferBase; /* attr{memory=cached} base pointer for NEXUS_VideoEncoderPicture.offset when it's used to carry metadata */
    uint32_t errorFlags;
    uint32_t eventFlags;

    unsigned errorCount;
    unsigned picturesReceived;
    unsigned picturesDroppedFRC;
    unsigned picturesDroppedErrors;
    unsigned picturesEncoded;
    uint32_t pictureIdLastEncoded;
} NEXUS_VideoEncoderStatus;

/**
Summary:
**/
void NEXUS_VideoEncoder_GetStatus(
    NEXUS_VideoEncoderHandle handle,
    NEXUS_VideoEncoderStatus *pStatus /* [out] */
    );



#ifdef __cplusplus
}
#endif


#endif /* NEXUS_VIDEO_ENCODER_OUTPUT_H__ */

