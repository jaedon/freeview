/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_playpump.h $
 * $brcm_Revision: 50 $
 * $brcm_Date: 10/11/12 12:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/nexus_playpump.h $
 * 
 * 50   10/11/12 12:07p vsilyaev
 * SW7425-3955: Use upper 8 bits as PES sub-id for MPEG-2 TS streams
 * 
 * 49   7/6/12 1:03p erickson
 * SW7340-296: use nexus_register_NEXUS_PidChannelHandle instead of attr
 * 
 * 48   6/15/12 11:51a erickson
 * SW7425-3081: add IsTransportTypeSupported
 * 
 * 47   3/21/12 2:28p erickson
 * SW7340-296: use nexus_unregister_NEXUS_PidChannelHandle instead of
 *  secondary_destructor
 * 
 * 46   1/2/12 4:20p erickson
 * SW7425-2050: clarify substream ID for NEXUS_TransportType_eVob
 * 
 * 45   12/20/11 1:44p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 * 
 * 44   11/29/11 11:39a erickson
 * SW7420-2129: get current default heap using NEXUS_P_DefaultHeap
 * 
 * 43   10/24/11 1:38p erickson
 * SWDEPRECATED-2425: improve comments on picking fifoSize
 * 
 * 42   10/17/11 12:06p jtna
 * SW7425-1383: merge NEXUS_Playpump_GetAllPassPidChannelIndex
 * 
 * 41   8/29/11 1:55p erickson
 * SW7420-1148: NULL is allowed for trusted client
 * 
 * 40   8/25/11 10:53a erickson
 * SW7420-1148: can't allow NULL For NEXUS_PlaypumpOpenSettings because
 *  untrusted client must set the heap
 * 
 * 39   7/21/11 2:27p jtna
 * SW7125-1015: added NEXUS_Playpump_SubmitScatterGatherDescriptor()
 * 
 * 38   1/28/11 8:53a erickson
 * SW7420-1440: fix destructor and secondary_destructor options for
 *  NEXUS_Playpump_OpenPidChannel
 *
 * 37   12/13/10 6:59p vsilyaev
 * SW7425-39: Added support for MUX input
 *
 * 36   12/9/10 3:32p erickson
 * SW7420-1308: use unconditional nexus_dma_types.h in the public API
 *
 * 35   12/7/10 5:46p erickson
 * SW7420-1148: added null_allowed, boundsHeap
 *
 * 34   10/14/10 12:34p erickson
 * SW7420-1148: add secondary_destructor to support
 *  NEXUS_Playpump_ClosePidChannel, verify function attr in one place,
 *  support multiple param verifications per ioctl
 *
 * 32   10/1/10 9:41a erickson
 * SW7420-1009: support NEXUS_ANY_ID
 *
 * 31   8/10/10 3:33p erickson
 * SW7420-934: rename NEXUS_Playpump_ReadComplete to
 *  NEXUS_Playpump_WriteComplete
 *
 * 30   6/11/10 6:58p mphillip
 * SW7125-463: Allow the user to configure DMA data format in playpump
 *
 * 29   6/9/10 11:31a mphillip
 * SW7550-398: Enable encrypted PVR for 7550
 *
 * 28   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 *
 * 27   11/3/09 9:55a erickson
 * SW7405-3308: add teiError callback
 *
 * 26   10/1/09 5:02p erickson
 * SW7405-3087: add continuity count error checking and callback. default
 *  off.
 *
 * 25   6/11/09 3:52p erickson
 * PR55767: add NEXUS_PlaypumpSettings.timestamp.forceRestamping
 *
 * 24   5/28/09 11:31a erickson
 * PR55461: add PCR-based pacing
 *
 * 23   5/15/09 12:48p vsilyaev
 * PR 55193: Added function to return last parsed PTS by the media library
 *
 * 22   4/3/09 2:59p erickson
 * PR35457: update comments
 *
 * 21   1/26/09 11:03a vsilyaev
 * PR 51579: Adding originalTransportType to the PlaypumpSettings
 *
 * 20   12/4/08 3:29p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 *  stream parsing
 *
 * 19   12/1/08 12:53p erickson
 * PR49676: add comments on pidChannelIndex requirements for allPass pid
 *  channels
 *
 * 18   11/6/08 12:51p erickson
 * PR35457: update comment
 *
 * 17   10/30/08 10:26p erickson
 * PR47132: added NEXUS_PlaypumpSettings.blindSync
 *
 * 16   10/27/08 11:20a erickson
 * PR47232: added pacingTsRangeError to NEXUS_PlaypumpStatus
 *
 * 15   10/22/08 11:39a erickson
 * PR48216: allow transport to run with no dma or security modules
 *
 * 14   10/16/08 4:20p jgarrett
 * PR 47951: Adding index to status
 *
 * 13   10/9/08 5:48p erickson
 * PR47232: added NEXUS_PlaypumpSettings.timestamp.resetPacing
 *
 * 12   10/7/08 11:31p erickson
 * PR47232: extend NEXUS_PlaypumpSettings timestamp settings. reorganized
 *  into a nested structure.
 *
 * 11   10/6/08 1:59a erickson
 * PR47232: added NEXUS_Playpump_SetPause for IP STB throttling
 *
 * 10   10/6/08 1:30a erickson
 * PR47232: added NEXUS_PlaypumpSettings.pacingMaxError
 *
 * 9   9/4/08 12:47p erickson
 * PR46315: added NEXUS_PlaypumpSettings.allPass
 *
 * 8   7/29/08 3:07p erickson
 * PR45199: added NEXUS_PlaypumpSettings.maxDataRate
 *
 * 7   6/25/08 5:50p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * 6   6/24/08 11:50a vsilyaev
 * PR 38347: Use separate timestamps for audio and video streams
 *
 * 5   5/30/08 10:53a erickson
 * PR34925: add comment
 *
 * 4   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 3   2/1/08 5:05p vsilyaev
 * PR 38682: Added attributes to mark destructor and shutdown functions
 *
 * 2   1/25/08 2:35p vsilyaev
 * PR 38682: Added handling of 'attr' to the function arguments
 *
 * 1   1/18/08 2:15p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/19   1/9/08 3:22p erickson
 * PR34925: update docs
 *
 * Nexus_Devel/18   12/20/07 2:07p vsilyaev
 * PR 37574: Added playRate for playpump
 *
 * Nexus_Devel/17   12/4/07 2:38p erickson
 * PR36068: remove timestampEnabled
 *
 * Nexus_Devel/16   11/30/07 11:11a erickson
 * PR35457: api update
 *
 * Nexus_Devel/15   11/29/07 2:24p erickson
 * PR35457: doc update for 0.5 release
 *
 * Nexus_Devel/14   11/19/07 1:19p erickson
 * PR34925: split demux.h into separate interface files
 *
 * Nexus_Devel/13   10/19/07 4:05p vsilyaev
 * PR 36291: Added datatype for S/G playpump
 *
 * Nexus_Devel/12   10/15/07 2:54p vsilyaev
 * PR 35824: Added NEXUS_Playpump_GetDefaultSettings
 *
 * Nexus_Devel/11   10/12/07 5:03p vsilyaev
 * PR 35824: Added flag to enable/disable timestamps
 *
 * Nexus_Devel/10   10/11/07 6:25p vsilyaev
 * PR 35824: Added more playback code
 *
 * Nexus_Devel/9   10/10/07 5:09p vsilyaev
 * PR 35824: Added dependent include files
 *
 * Nexus_Devel/8   10/9/07 3:21p vsilyaev
 * PR 35824: Demoted Playpump_Pause to private function
 *
 * Nexus_Devel/7   10/9/07 2:10p vsilyaev
 * PR 35824: Reworked playpump API
 *
 * Nexus_Devel/6   10/4/07 12:26p erickson
 * PR34925: api updates
 *
 * Nexus_Devel/5   10/3/07 9:50a erickson
 * PR34925: coding convention
 *
 * Nexus_Devel/4   9/26/07 3:11p erickson
 * PR34925: initial playpump checkin
 *
 * Nexus_Devel/3   9/11/07 6:28p erickson
 * PR34254: update
 *
 * Nexus_Devel/2   8/28/07 11:57a erickson
 * PR34254: update
 *
 **************************************************************************/
#ifndef NEXUS_PLAYPUMP_H__
#define NEXUS_PLAYPUMP_H__

#include "nexus_types.h"
#include "nexus_pid_channel.h"
#include "nexus_dma_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the Playpump Interface.

Description:
The Playpump Interface provides data feeding for PVR playback functionality.
Each Interface corresponds to one hardware playback channel.

See nexus/examples/playpump.c for an example application.

See NEXUS_PlaybackHandle for a high-level Playback Interface.
See NEXUS_RecpumpHandle for the corresponding low-level PVR record Interface.
See NEXUS_VideoDecoderHandle and NEXUS_AudioDecoderHandle for decoder-based PVR and trick mode options.
**/
typedef struct NEXUS_Playpump *NEXUS_PlaypumpHandle;

/**
Summary:
Settings used in NEXUS_Playpump_Open
**/
typedef struct NEXUS_PlaypumpOpenSettings
{
    size_t fifoSize;            /* size of playpump's fifo in bytes.
                                   The minimum size depends on the maximum bitrate of your streams and the system latency for filling the buffers.
                                   The system latency depends on i/o performance and OS scheduling.
                                   If the size chosen is too small, you will underflow the decoders. */
    unsigned alignment;         /* You may want to specify the alignment of the fifo allocation to prevent any wasted buffer because of memory alignment requirements.
                                   See NEXUS_Playppump_ReadComplete and its skip parameter for more information about alignment requirements. */
    unsigned numDescriptors;    /* More descriptors are needed to perform certain trick modes. See NEXUS_Playpump_GetBuffer for details. */
    bool streamMuxCompatible;     /* If set to true,  this playpump could be used with the StreamMux module */

    NEXUS_HeapHandle heap;      /* optional heap for fifo allocation */
    NEXUS_HeapHandle boundsHeap; /* optional heap to bounds check all scatter-gather descriptors */
} NEXUS_PlaypumpOpenSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_Playpump_GetDefaultOpenSettings(
    NEXUS_PlaypumpOpenSettings *pSettings /* [out] */
    );

/**
Summary:
Open a new Playpump interface.
**/
NEXUS_PlaypumpHandle NEXUS_Playpump_Open( /* attr{destructor=NEXUS_Playpump_Close} */
    unsigned index, /* index of hardware playback channel. NEXUS_ANY_ID is supported. */
    const NEXUS_PlaypumpOpenSettings *pSettings /* attr{null_allowed=y} may be NULL for default settings */
    );

/**
Summary:
Close the Playpump interface.
**/
void NEXUS_Playpump_Close(
    NEXUS_PlaypumpHandle playpump
    );

/**
Summary:
Determines method of feeding data into playpump.
**/
typedef enum NEXUS_PlaypumpMode
{
    NEXUS_PlaypumpMode_eFifo,         /* Playpump allocates a block of memory which serves as the playback fifo.
                                         The user must copy data into this buffer with the CPU or DMA. */
    NEXUS_PlaypumpMode_eSegment,      /* This is like Fifo, except the user must prepend NEXUS_PlaypumpSegment headers to every data block. */
    NEXUS_PlaypumpMode_eScatterGather /* DEPRECATED: use NEXUS_Playpump_SubmitScatterGatherDescriptor() instead.
                                         In this mode, playpump's fifo is a fifo of NEXUS_PlaypumpDesc structures. Each descriptor has a pointer to
                                         the data to be played. That data is not copied but is read directly into transport hardware. */
} NEXUS_PlaypumpMode;

/**
Summary:
Base rate for normal play used in NEXUS_PlaypumpSettings.playRate.
**/
#define NEXUS_NORMAL_PLAY_SPEED 1000

/**
Summary:
Settings for playpump
**/
typedef struct NEXUS_PlaypumpSettings
{
    NEXUS_TransportType transportType;          /* Type of data stream going into playpump. Note that if the data must be packetized,
                                                   you can get the resulting NEXUS_TransportType from NEXUS_PidChannelStatus. */

    NEXUS_TransportType originalTransportType;  /* If the user packetizes intput to playpump, this describes the input. If there is no
                                                   packetization, this is member should be set to NEXUS_TransportType_eUnknown. */

    bool continuityCountEnabled;                /* If true, transport will check for correct continuity counters and discard bad packets.
                                                   This must be set to false for trick modes. Default is false. */

    /* Type of original transport stream */
    NEXUS_PlaypumpMode mode;                    /* Method of pushing data into playpump. This setting determines the format of data the app will feed to playpump. */
    NEXUS_CallbackDesc dataCallback;            /* Callback when space becomes available. User should call NEXUS_Playpump_GetBuffer.
                                                   You will not receive another callback until NEXUS_Playpump_GetBuffer is called. */
    NEXUS_CallbackDesc errorCallback;           /* Callback called when error was detected in ptocessing of the stream data */
    NEXUS_CallbackDesc ccError;                 /* Continuity Count Error - raised when continuity counter of next packet does not have the next counter value.
                                                   continuityCountEnabled must be set to true. See NEXUS_PidChannelStatus.continuityCountErrors to get a count of errors per pid. */
    NEXUS_CallbackDesc teiError;                /* Transport Error Indicator - error status from a demodulator */

    int playRate;                               /* Rate in units of NEXUS_NORMAL_PLAY_SPEED. It's used for certain type of streams to control the direction of presentation time. */

    NEXUS_DmaHandle securityDma;                /* Instance of DMA that is used for security processing, NULL if security processing is not required */
    NEXUS_DmaDataFormat securityDmaDataFormat;
    NEXUS_KeySlotHandle securityContext;

    unsigned maxDataRate;                       /* Maximum data rate for the playback parser band in units of bits per second. Default is typically 108000000 (i.e. 108 Mbps).
                                                   If you increase this, you need to analyze total transport bandwidth and overall system bandwidth. */
    bool blindSync;                             /* Force transport to not look for sync bytes. */

    /* all pass settings */
    bool allPass;                               /* If true, NEXUS_Playpump_OpenPidChannel's pid param is ignored and the resulting pid channel can be used for all-pass record.
                                                   Also set acceptNullPackets to true if you want to capture the entire stream.
                                                   When opening the allPass pid channel, set NEXUS_PlaypumpOpenPidChannelSettings.pidSettings.pidChannelIndex to the HwPidChannel obtained 
                                                   from NEXUS_ParserBand_GetAllPassPidChannelIndex(). */
    bool acceptNullPackets;                     /* If true and allPass is true, NULL packets are not discarded. */


    struct {
        NEXUS_TransportTimestampType type;      /* The type of timestamping in this stream. */
        uint16_t pcrPacingPid;                  /* Some chips can pace playback using a PCR pid. Set timestamp.pcrPacingPid to a non-zero value and set timestamp.pacing = true. */

        bool pacing;                            /* If true, pace this stream using embedded timestamp information or using pcr-based pacing.
                                                   For timestamp-based pacing, set timestampType as well.
                                                   For pcr-based pacing, set pcrPacingPid as well. */
        unsigned pacingMaxError;                /* Set the maximum allowable timestamp error (TS_RANGE_ERROR) used during playback pacing. */
        NEXUS_Timebase timebase;                /* Timebase used for timestamp. Use NEXUS_Timebase_eInvalid for freerun. */
        bool parityCheckDisable;                /* Set true to disable timestamp parity check. */
        bool pacingOffsetAdjustDisable;         /* Set true to adjust the pacing timestamps with the reference offset */
        bool resetPacing;                       /* Set true if you want the pacing start bit reset when a TS_RANGE_ERROR interrupt occurs */
        bool forceRestamping;                   /* Set true if you want to restamp the incoming timestamps or if you are recording and want to add timestamps */
    } timestamp;

} NEXUS_PlaypumpSettings;

/**
Summary:
Settings used in NEXUS_Playpump_OpenPidChannel
**/
typedef struct NEXUS_PlaypumpOpenPidChannelSettings
{
    NEXUS_PidChannelSettings pidSettings;
    NEXUS_PidType pidType;
    bool allowTimestampReordering;                     /* For some media formats it's required to reorder timestamps prior to sending data to the decoders, this flag controls whether playpump would reourder timestamps */
    union
    {
        struct
        {
            NEXUS_AudioCodec codec; /* Audio codec information is required for VOB substream id remapping. */
        } audio;
    } pidTypeSettings;
} NEXUS_PlaypumpOpenPidChannelSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_Playpump_GetDefaultOpenPidChannelSettings(
    NEXUS_PlaypumpOpenPidChannelSettings *pSettings /* [out] */
    );

/*
Summary:
Open a playpump NEXUS_PidChannelHandle.

Description:
This differs from a pid channel opened by NEXUS_PidChannel_Open because it
allows Playpump to get additional information about the pid.
NEXUS_Playpump_OpenPidChannel can be called before or after Start and Stop.
*/
NEXUS_PidChannelHandle NEXUS_Playpump_OpenPidChannel(
    NEXUS_PlaypumpHandle playpump,
    unsigned pid,                   /* Substream selector.
                                       For NEXUS_TransportType_eTs, the lower 16 bits are the PID and the bits 23..16 are the Sub-Stream ID.
                                       For NEXUS_TransportType_eMpeg2Pes, this is the Stream ID.
                                       For NEXUS_TransportType_eVob, the lower 8 bits are the Stream ID and the bits 15..8 are the Sub-Stream ID.
                                       For NEXUS_TransportType_eEs, this is ignored. */
    const NEXUS_PlaypumpOpenPidChannelSettings *pSettings /* attr{null_allowed=y} may be NULL for default settings */
    );

/*
Summary:
This function closes a pid channel opened by NEXUS_Playpump_OpenPidChannel
*/
NEXUS_Error NEXUS_Playpump_ClosePidChannel(
    NEXUS_PlaypumpHandle playpump,
    NEXUS_PidChannelHandle pidChannel
    );

/*
Summary:
Close all pid channels opened by NEXUS_Playpump_OpenPidChannel
*/
void NEXUS_Playpump_CloseAllPidChannels(
    NEXUS_PlaypumpHandle playpump
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_Playpump_GetDefaultSettings(
    NEXUS_PlaypumpSettings *pSettings /* [out] */
    );

/**
Summary:
Get current settings
**/
void NEXUS_Playpump_GetSettings(
    NEXUS_PlaypumpHandle playpump,
    NEXUS_PlaypumpSettings *pSettings /* [out] */
    );

/**
Summary:
Set new settings

Description:
If you use NEXUS_Playback, be aware that it will call this function with NEXUS_PlaybackSettings.playpumpSettings
any will overwrite any application use of this function.
**/
NEXUS_Error NEXUS_Playpump_SetSettings(
    NEXUS_PlaypumpHandle playpump,
    const NEXUS_PlaypumpSettings *pSettings
    );

/**
Summary:
Start playback

Description:
After calling this function, the app is responsible to call NEXUS_Playpump_GetBuffer,
feed in data, then call NEXUS_Playpump_WriteComplete.
**/
NEXUS_Error NEXUS_Playpump_Start(
    NEXUS_PlaypumpHandle playpump
    );

/**
Summary:
Stop playback
**/
void NEXUS_Playpump_Stop(
    NEXUS_PlaypumpHandle playpump
    );

/*
Summary:
Throw away all data currently in playback fifo.

Description:
This does not flush any downstream consumer like a decoders.
See NEXUS_VideoDecoder_Flush and NEXUS_AudioDecoder_Flush.
*/
NEXUS_Error NEXUS_Playpump_Flush(
    NEXUS_PlaypumpHandle playpump
    );

#if defined(HUMAX_PLATFORM_BASE)
void *NEXUS_Playpump_GetBufferStatus(NEXUS_PlaypumpHandle p);
#endif

/*
Summary:
Get a pointer and size for the next location in the buffer which can accept playpump data.

Description:
NEXUS_Playpump_GetBuffer is non-destructive. You can safely call it multiple times.

The buffer and size returned by NEXUS_Playpump_GetBuffer are valid until you call NEXUS_Playpump_Flush or NEXUS_Playpump_WriteComplete.

The size returned by NEXUS_Playpump_GetBuffer can be limited by two things: space in the ring buffer or free descriptors
used to send data to the transport block. If you are performing trick modes with lots of small reads,
you may find that NEXUS_Playpump_GetBuffer says there is no space available even though
NEXUS_PlaypumpStatus reports free space. The solution is to increase the number of descriptors
in NEXUS_PlaypumpOpenSettings.
*/
NEXUS_Error NEXUS_Playpump_GetBuffer(
    NEXUS_PlaypumpHandle playpump,
    void **pBuffer, /* [out] attr{memory=cached} pointer to memory mapped region that is ready for playback data */
    size_t *pSize /* [out] total number of writeable, contiguous bytes which buffer is pointing to */
    );

/**
Summary:
Notify Playpump of how much data was added into the buffer.

Description:
You can only call NEXUS_Playpump_WriteComplete once after a NEXUS_Playpump_GetBuffer call.
After calling it, you must call NEXUS_Playpump_GetBuffer before adding more data.

The skip parameter allows an application to copy data into the playpump buffer with
whatever alignment it desired. This is needed for "direct I/O" from disk drives which requires
all memory access to be page aligned (e.g. 4K).
**/
NEXUS_Error NEXUS_Playpump_WriteComplete(
    NEXUS_PlaypumpHandle playpump,
    size_t skip,            /* skip is the number of bytes at the beginning of the current buffer pointer
                               which Playpump should skip over. */
    size_t amountUsed       /* amountUsed is the number of bytes, following any skip bytes,
                               which Playpump should feed into transport. */
    );

/* backward compatibility */
#define NEXUS_Playpump_ReadComplete NEXUS_Playpump_WriteComplete

/**
Summary:
Playpump scatter gather Descriptor
**/
typedef struct NEXUS_PlaypumpScatterGatherDescriptor
{
    void *addr;    /* attr{memory=cached} address of memory region */
    size_t length; /* size of the memory region */
} NEXUS_PlaypumpScatterGatherDescriptor;

/**
Summary:
Submit scatter gather descriptor to playpump
**/
NEXUS_Error NEXUS_Playpump_SubmitScatterGatherDescriptor(
    NEXUS_PlaypumpHandle playpump,
    const NEXUS_PlaypumpScatterGatherDescriptor *pDesc, /* attr{nelem=numDescriptors} */
    size_t numDescriptors,
    size_t *pNumConsumed /* [out] */
    );

/**
Summary:
Status returned by NEXUS_Playpump_GetStatus
**/
typedef struct NEXUS_PlaypumpStatus
{
    bool started;               /* Has NEXUS_Playpump_Start been called? */
    size_t fifoDepth;           /* Depth in bytes of the playback buffer */
    size_t fifoSize;            /* Size in bytes of the playback buffer */
    size_t descFifoDepth;       /* Number of active (i.e. busy) descriptors */
    size_t descFifoSize;        /* Number of allocated descriptors */
    void *bufferBase;           /* attr{memory=cached} Pointer to the base of the playback buffer.
                                   This can be used for calculating your exact position
                                   in the buffer for alignment considerations. */
    uint64_t bytesPlayed;       /* Total number of bytes played since starting */
    unsigned index;             /* Index of this playpump */

    unsigned pacingTsRangeError; /* Number of TS_RANGE_ERROR's if pacing is activated. */
    unsigned syncErrors;         /* count of sync errors found in the stream */
    unsigned resyncEvents;       /* count of resync handled in the stream */
    unsigned streamErrors;       /* count of other errors found in the stream */
    NEXUS_PtsType mediaPtsType;    /* Type of the current PTS */
    uint32_t mediaPts;    /* if playpump does inline media streeam conversion (i.e. AVI to PES), then this is the last known PTS (in units of 45KHz) for the first converted pid (track) */
} NEXUS_PlaypumpStatus;

/**
Summary:
Get current status of playpump
**/
NEXUS_Error NEXUS_Playpump_GetStatus(
    NEXUS_PlaypumpHandle playpump,
    NEXUS_PlaypumpStatus *pStatus /* [out] */
    );

/**
Summary:
Signature used in NEXUS_PlaypumpSegment.signature when in NEXUS_PlaypumpMode_eSegment mode.
**/
#define NEXUS_PLAYPUMP_SEGMENT_SIGNATURE ((((uint32_t)'B')<<24) | (((uint32_t)'P')<<16 ) | (((uint32_t)'L')<<8) | 0)

/**
Summary:
Segment header used in NEXUS_PlaypumpMode_eSegment mode.

Description:
When feeding data into Playpump, the user must prepend NEXUS_PlaypumpSegment before every data block.
**/
typedef struct NEXUS_PlaypumpSegment
{
    uint32_t length;    /* Size of the segment. This includes the size of the NEXUS_PlaypumpSegment header and all data following the header. */
    uint64_t offset;    /* 64 bit offset in the source file for this segment */
    uint32_t timestamp; /* timestamp for this segment */
    struct {
        uint16_t stream_id;
        int16_t  timestamp_delta;
    } timestamp_delta[9];
    uint32_t signature; /* This field must be set to BPLAYPUMP_SEGMENT_SIGNATURE. Nexus uses this to validate the segmented data stream. */
} NEXUS_PlaypumpSegment;

/**
Summary:
Descriptor used in NEXUS_PlaypumpMode_eScatterGather mode to address data to be read into transport hardware.

Description:
Memory must be physically contiguous.
**/
typedef struct NEXUS_PlaypumpDesc
{
    void *addr;    /* attr{memory=cached} address of memory region.
                      This address is currently not converted to/from Linux kernel mode. Therefore NEXUS_PlaypumpMode_eScatterGather is only usable
                      in Linux user mode. */
    size_t length; /* size of the memory region */
} NEXUS_PlaypumpDesc;

/**
Summary:
Pause or unpause the transport playback hardware

Description:
This does not pause decoders; it pauses the transport playback hardware state machine (aka "micro pause").
Be aware this could result in loss of data if the application unpauses before a consumer
is attached. Nexus normally uses this same hardware pause to ensure this does not occur.
**/
NEXUS_Error NEXUS_Playpump_SetPause(
    NEXUS_PlaypumpHandle playpump,
    bool paused
    );

/**
Summary:
Suspend or resume pacing

Description:
NEXUS_PlaypumpSettings.timebase.pacing must be true for this to be used.
**/
NEXUS_Error NEXUS_Playpump_SuspendPacing(
    NEXUS_PlaypumpHandle playpump,
    bool suspended
    );

/**
Summary:
Determine the PID channel that will be used in allPass mode. 
 
Description: 
Each playpump will use a different PID channel when operating in
allPass mode. This API provides the channel number for the given
playpump. 
**/
NEXUS_Error NEXUS_Playpump_GetAllPassPidChannelIndex(
    NEXUS_PlaypumpHandle playpump, 
    unsigned *pHwPidChannel
    );

/**
Summary:
Learn if playpump supports the transport type

Description:
Some transport types, like eMp4 and eMkv, require higher level support. See NEXUS_Playback_IsTransportTypeSupported.
**/
void NEXUS_Playpump_IsTransportTypeSupported(
    NEXUS_PlaypumpHandle playpump,
    NEXUS_TransportType transportType,
    bool *pIsSupported
    );

#ifdef __cplusplus
}
#endif

#endif
