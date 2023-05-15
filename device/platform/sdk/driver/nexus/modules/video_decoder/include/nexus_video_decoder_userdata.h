/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_userdata.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 9/29/11 10:18a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/include/nexus_video_decoder_userdata.h $
 * 
 * 13   9/29/11 10:18a erickson
 * SW7425-1367: added NEXUS_VideoDecoderStreamInformation.pictureCoding
 * 
 * 12   9/2/11 4:08p erickson
 * SWDEPRECATED-2425: update comments
 * 
 * 11   7/29/10 8:27a erickson
 * SW7405-4637: handle userdata ringbuffer wraps internally for easier app
 *  code
 *
 * 10   5/17/10 4:43p erickson
 * SWDEPRECATED-2425: clarify use of NEXUS_UserDataFormat
 *
 * 9   1/11/10 2:00p erickson
 * SW7405-3718: expand size of afdValue for XVD support
 *
 * 8   5/20/09 3:30p erickson
 * PR55042: refactor active format description support. add callback and 8
 *  bit status value.
 *
 * 7   3/26/09 11:16a erickson
 * PR53615: changed NEXUS_VideoDecoderUserDataStatus.formatParsed to a
 *  count. added lastFormatParsed.
 *
 * 6   3/5/09 12:08p erickson
 * PR52813: added NEXUS_UserDataFormat_eAfd53
 *
 * 5   5/8/08 3:49p erickson
 * PR41962: added PictureCoding for MPEG video
 *
 * 4   4/28/08 4:11p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 3   4/21/08 1:43p erickson
 * PR41962: added NEXUS_UserDataHeader.pictureCodingExtension
 *
 * 2   4/18/08 1:53p erickson
 * PR41533: impl EIA-708 parsing and capture
 *
 * 1   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_USERDATA_H__
#define NEXUS_VIDEO_DECODER_USERDATA_H__

#include "nexus_video_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Picture polarity
**/
typedef enum NEXUS_PicturePolarity
{
    NEXUS_PicturePolarity_eTopField,        /* for interlaced picture */
    NEXUS_PicturePolarity_eBottomField,     /* for interlaced picture */
    NEXUS_PicturePolarity_eFrame,           /* for progressive picture */
    NEXUS_PicturePolarity_eMax
} NEXUS_PicturePolarity;

/**
Summary:
Origin of userdata
**/
typedef enum NEXUS_UserDataType
{
    NEXUS_UserDataType_eSeq=1,       /* MPEG user data coming in the Sequence Header */
    NEXUS_UserDataType_eGop,         /* MPEG user data coming in the Gop Header */
    NEXUS_UserDataType_ePicture,     /* MPEG user data coming in Picture Header */
    NEXUS_UserDataType_eSei,         /* H.264 user data */
    NEXUS_UserDataType_eEntryPoint,  /* VC1 user data coming in entry point layer */
    NEXUS_UserDataType_eField,       /* VC1 user data coming in picture field layer */
    NEXUS_UserDataType_eFrame,       /* VC1 user data coming in picture frame layer */
    NEXUS_UserDataType_eSlice,       /* VC1 user data coming in slice layer */
    NEXUS_UserDataType_eMax
} NEXUS_UserDataType;

/**
Summary:
The header for all payload returned by NEXUS_VideoDecoder_GetUserDataBuffer.

Description:
Be aware that the size of NEXUS_UserDataHeader may change in future releases. You should never hardcode
the size, but use sizeof(NEXUS_UserDataHeader) in your code.
**/
typedef struct NEXUS_UserDataHeader
{
    unsigned blockSize;    /* Size of all data for this block. This includes the header (including the blockSize field), the payload which follows
                              the header, and any padding which follows the payload.
                              Padding may be used to ensure NEXUS_UserDataHeader is 32 bit aligned for efficient CPU access. */
    NEXUS_PicturePolarity polarity;
    NEXUS_UserDataType type;
    bool topFieldFirst;    /* Only valid for MPEG. */
    bool repeatFirstField; /* Only valid for MPEG. */
    NEXUS_PictureCoding pictureCoding; /* Type of picture coding. Only valid for MPEG2 video. */
    bool ptsValid;         /* pts is valid only if this is true. */
    uint32_t pts;          /* PTS that this userdata packet arrived on. Check ptsValid before reading this. */
    uint32_t pictureCodingExtension[2];  /* Array of 2 32 bit variable for storing Picture Coding Extension.
                                            Data is stored in big endian.
                                            In word-0 bits 31-30 are unused and in word-1 bits 31-20 are unused. */
    unsigned payloadSize;  /* Size of userdata payload which follows this header, in bytes. There may be padding which follows this.
                              Use blockSize to skip to the next userdata packet. */
} NEXUS_UserDataHeader;


/**
Summary:
Get raw userdata from digital stream.

Description:
UserData contains close captioning, subtitles or other information. This interface allows the application
to capture and process that data. The format of the data is stream-dependent.

VideoDecoder also has an internal connection to capture and parse EIA-608 and EIA-708 data as well as DSS subtitle data.
This data can be read with NEXUS_VideoInput_ReadClosedCaption.
The EIA-608 data can be automatically encoded to a VEC.

The format of the data stream is NEXUS_UserDataHeader, followed by a whole userdata packet.
Ringbuffer wraps are handled internally, so NEXUS_VideoDecoder_GetUserDataBuffer will always return whole packets including headers.

You must specify the NEXUS_VideoDecoderSettings.userDataEnabled to enable data flow to this interface.
Any data not consumed in time by the application will be dropped and an overflow warning will be printed to the console.

NEXUS_VideoDecoder_GetUserDataBuffer can be called multiple times in a row and is non-destructive.

See Also:
NEXUS_VideoDecoder_UserDataReadComplete
**/
NEXUS_Error NEXUS_VideoDecoder_GetUserDataBuffer(
    NEXUS_VideoDecoderHandle handle,
    void **pBuffer,   /* [out] attr{memory=cached} pointer to userdata */
    unsigned *pSize   /* [out] number of bytes of userdata */
    );

/*
Summary:
Tell the video decoder how much data was consumed from the last NEXUS_VideoDecoder_GetUserDataBuffer call.

Description:
NEXUS_VideoDecoder_UserDataReadComplete can only be called once after NEXUS_VideoDecoder_GetUserDataBuffer.
After calling NEXUS_VideoDecoder_UserDataReadComplete, you must call NEXUS_VideoDecoder_GetUserDataBuffer again before
processing any more data.

See Also:
NEXUS_VideoDecoder_GetUserDataBuffer
*/
void NEXUS_VideoDecoder_UserDataReadComplete(
    NEXUS_VideoDecoderHandle handle,
    unsigned size   /* number of bytes of userdata consumed by the application */
    );

/**
Summary:
Discard all userdata which is currently queued.
**/
void NEXUS_VideoDecoder_FlushUserData(
    NEXUS_VideoDecoderHandle videoDecoder
    );

/**
Summary:
Format of userdata that Nexus will filter after EIA-608/708 parsing for the Display module.

Description:
Some streams contain multiple formats of 608/708 data encoded into user data.
NEXUS_VideoDecoder_GetUserDataBuffer will return all user data in a raw form.
However, if NEXUS_VideoInputVbiSettings.closedCaptionEnabled is true, the EIA-608/708 parsed from the userdata must be filtered down to only one format.

This enum currently only applies to MPEG userdata. Use NEXUS_UserDataFormat_eAny for AVC or VC1.
**/
typedef enum NEXUS_UserDataFormat
{
    NEXUS_UserDataFormat_eAny = 0, /* Nexus will filter based on the first format it detects. */
    NEXUS_UserDataFormat_eAtsc53,
    NEXUS_UserDataFormat_eScte20,  /* formerly DVS 157 */
    NEXUS_UserDataFormat_eScte21,  /* formerly DVS 053 */
    NEXUS_UserDataFormat_eMax
} NEXUS_UserDataFormat;

/**
Summary:
Userdata status returned by NEXUS_VideoDecoder_GetUserDataStatus
**/
typedef struct NEXUS_VideoDecoderUserDataStatus
{
    unsigned formatParsed[NEXUS_UserDataFormat_eMax]; /* # of userdata packets for this format that have come out of the userdata parser since NEXUS_VideoDecoder_Start or NEXUS_VideoDecoder_FlushUserdata */
    NEXUS_UserDataFormat lastFormatParsed;
    uint32_t afdValue; /* Active format description value parsed from userdata. If this value changes, NEXUS_VideoDecoderSettings.afdChanged will be fired.
                          This AFD value is also passed directly to a connected VideoWindow. See NEXUS_VideoWindowAfdSettings for automatic display options. */
} NEXUS_VideoDecoderUserDataStatus;

/**
Summary:
Get status about userdata capture and parsing
**/
NEXUS_Error NEXUS_VideoDecoder_GetUserDataStatus(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderUserDataStatus *pStatus /* [out] */
    );


/**
Summary:
Tell the userdata parser what format to filter after parsing for the Display module

Description:
If the format is NEXUS_UserDataFormat_eAny (which is default), the parser will filter based on the first format found.

This filter does not apply to raw data captured from NEXUS_VideoDecoder_GetUserDataBuffer. If you want to filter
raw data, you must parse it in your application.
**/
NEXUS_Error NEXUS_VideoDecoder_SetUserDataFormatFilter(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_UserDataFormat format
    );

#ifdef __cplusplus
}
#endif

#endif
