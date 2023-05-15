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
 * $brcm_Workfile: nexus_stream_mux.h $
 * $brcm_Revision: 15 $
 * $brcm_Date: 8/16/12 3:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/stream_mux/include/nexus_stream_mux.h $
 * 
 * 15   8/16/12 3:28p vsilyaev
 * SW7425-3684: Added duration of muxed portion of stream/file
 * 
 * 14   7/19/12 12:57p vsilyaev
 * SW7425-3076: Added API to explicitly control memory allocation by the
 *  mux
 * 
 * 13   5/17/12 11:59a vsilyaev
 * SW7425-2606: Added setting for mux service period
 * 
 * 12   3/29/12 12:17p vsilyaev
 * SW7425-2694: Allow user to specify the stream PES id
 * 
 * 11   1/26/12 1:24p vsilyaev
 * SW7425-2271: Added user data configuration
 * 
 * 10   10/27/11 1:13p vsilyaev
 * SW7425-1236: Bumped number of pids(track) per stream
 * 
 * 9   6/9/11 1:04p vsilyaev
 * SW7425-484: Added non-realtime options
 * 
 * 8   2/8/11 5:15p vsilyaev
 * SW7425-30: Removed unused types
 * 
 * 7   2/8/11 5:12p vsilyaev
 * SW7425-39: Updated API so it could pass user/kernel proxy
 * 
 * 6   2/7/11 12:14p vsilyaev
 * SW7425-39: Added 'null_allowed' attributes
 * 
 * 5   12/30/10 12:10p jgarrett
 * SW7425-41: Linking audio mux output to stream mux
 * 
 * 4   12/28/10 5:45p vsilyaev
 * SW7425-39: Open Audio/Video PidChannel inside the stream mux module
 * 
 * 3   12/22/10 6:48p vsilyaev
 * SW7425-39: Updated MUX module
 * 
 * 2   12/13/10 3:46p vsilyaev
 * SW7425-39: Promote video/audio/pcr pids to dedicated structure, so they
 *  are missed by generator of the kernel driver code
 * 
 * 1   12/9/10 8:08p vsilyaev
 * SW7425-39: Mux module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_STREAM_MUX_H__
#define NEXUS_STREAM_MUX_H__

#include "nexus_video_encoder_output.h"
#include "nexus_playpump.h"
#include "nexus_stc_channel.h"
#include "nexus_audio_mux_output.h"
#include "nexus_message.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Muxing TS streams using XPT playback HW
**/

/**
Summary:
**/
typedef struct NEXUS_StreamMux *NEXUS_StreamMuxHandle;

typedef struct NEXUS_StreamMuxMemoryConfiguration {
    size_t  systemBufferSize; /* only accesses by the host */
    size_t  sharedBufferSize; /* accessed by both the host and devices(s) */
} NEXUS_StreamMuxMemoryConfiguration;

/**
Summary:
**/
typedef struct NEXUS_StreamMuxCreateSettings
{
    NEXUS_CallbackDesc finished; /* NEXUS_StreamMux_Finish has completed. NEXUS_StreamMux_Stop can be called for a clean finish. */
    NEXUS_StreamMuxMemoryConfiguration memoryConfiguration;
    NEXUS_HeapHandle    systemHeap; /* heap that is used to allocated memory for the  host only accessible buffer, if NULL, then OS allocator would be used */
    NEXUS_HeapHandle    sharedHeap; /* heap that is used to allocated memory for the host and devies(s) buffer, if NULL, then default heap would be used */
} NEXUS_StreamMuxCreateSettings;

/**
Summary:
**/
typedef struct NEXUS_StreamMuxConfiguration {
    unsigned servicePeriod;     /* (in milliseconds) Indicates the lowest frequency
                                 * the muxer is expected to be executed. 0 indicates
                                 * the default service period. 
                                 */
    unsigned latencyTolerance;   /* (in milliseconds) Indicates the maximum latency
                                 * the muxer is expected to handle without causing
                                 * underflow.
                                 */

    bool nonRealTime;           /* Normal operation for muxing in real time, if this is set to 'true' then muxing done in realtime */
    unsigned videoPids;
    unsigned audioPids;
    unsigned userDataPids;
    unsigned pcrPidinterval; /* periodic milliseconds */
    unsigned systemDataBitRate; /* in bits/sec.  1Mbps = 1000000.
                                 * If system data is split across multiple packets,
                                 * this value determines the gap between the packets.
                                 */
} NEXUS_StreamMuxConfiguration;


/**
Summary:
**/
void NEXUS_StreamMux_GetDefaultCreateSettings(
    NEXUS_StreamMuxCreateSettings *pSettings   /* [out] */
    );

/**
Summary:
**/
NEXUS_StreamMuxHandle NEXUS_StreamMux_Create( /* attr{destructor=NEXUS_StreamMux_Destroy}  */
    const NEXUS_StreamMuxCreateSettings *pSettings     /* attr{null_allowed=y} Pass NULL for default settings */
    );

/**
Summary:
**/
void NEXUS_StreamMux_Destroy(
    NEXUS_StreamMuxHandle handle
    );


#define NEXUS_MAX_MUX_PIDS 16
typedef struct NEXUS_StreamMuxVideoPid {
    uint16_t pid; /* PID in which to insert the encoded video data */
    uint8_t pesId; /* pes stream ID */
    NEXUS_PlaypumpHandle playpump;
    NEXUS_VideoEncoderHandle encoder; /* if NULL, then not enabled */
} NEXUS_StreamMuxVideoPid;

typedef struct NEXUS_StreamMuxAudioPid {
    uint16_t pid; /* PID in which to insert the encoded audio data */
    uint8_t pesId; /* pes stream ID */
    NEXUS_PlaypumpHandle playpump;
    NEXUS_AudioMuxOutputHandle muxOutput; /* if NULL, then not enabled */
} NEXUS_StreamMuxAudioPid;

typedef struct NEXUS_StreamMuxPcrPid {
    unsigned pid; /* PID in which it to insert the PCR */
    NEXUS_PlaypumpHandle playpump;
    unsigned interval; /* periodic milliseconds */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    bool nonPcrInsert;		/* Normal TS file should include PCR TSPacket, but M3U8 do not need it */
#endif
} NEXUS_StreamMuxPcrPid;

typedef struct NEXUS_StreamMuxUserDataPid {
    NEXUS_MessageHandle message; /* The userdata is expected to arrive as PES packets encapsulated in TS packets. Application is responsible for calling NEXUS_Message_Start prior to calling NEXUS_StreamMux_Start and call NEXUS_Message_Stop after mux session was completed */
} NEXUS_StreamMuxUserDataPid;

/**
Summary:
**/
typedef struct NEXUS_StreamMuxStartSettings
{
    NEXUS_TransportType transportType;
    NEXUS_StcChannelHandle stcChannel;

    NEXUS_StreamMuxVideoPid video[NEXUS_MAX_MUX_PIDS];
    NEXUS_StreamMuxAudioPid audio[NEXUS_MAX_MUX_PIDS];
    NEXUS_StreamMuxPcrPid pcr;
    NEXUS_StreamMuxUserDataPid userdata[NEXUS_MAX_MUX_PIDS];

    unsigned latencyTolerance;   /* (in milliseconds) Indicates the maximum latency
                                 * the muxer is expected to handle without causing
                                 * underflow.
                                 *
                                 * This value directly affects the overall mux latency
                                 * and the encoder's CDB buffer size
                                 */

    bool nonRealTime;           /* Normal operation for muxing in real time, if this is set to 'true' then muxing done in realtime */

    int nonRealTimeRate;        /* Rate in units of NEXUS_NORMAL_PLAY_SPEED for non realtime muxing, it's the upper bound to the rate of muxing, due to the underlying limitations rate could be only be power of 2, for example 1*NEXUS_NORMAL_PLAY_SPEED , 2*NEXUS_NORMAL_PLAY_SPEED, 4*NEXUS_NORMAL_PLAY_SPEED */

    unsigned servicePeriod;     /* (in milliseconds) Indicates the lowest frequency
                                 * the muxer is expected to be executed. 0 indicates
                                 * the default service period.
                                 *
                                 * Smaller Service Period = Higher Frequency of execution
                                 *
                                 * To prevent underflow at the transport, the application
                                 * MUST guarantee that the specified service period can be
                                 * met.
                                 *
                                 * This value directly affects the overall mux latency
                                 * and the encoder's CDB buffer size
                                 */
} NEXUS_StreamMuxStartSettings;

typedef struct NEXUS_StreamMuxOutput
{
    NEXUS_PidChannelHandle video[NEXUS_MAX_MUX_PIDS];
    NEXUS_PidChannelHandle audio[NEXUS_MAX_MUX_PIDS];
} NEXUS_StreamMuxOutput;

/**
Summary:
**/
void NEXUS_StreamMux_GetDefaultStartSettings(
    NEXUS_StreamMuxStartSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_StreamMux_Start(
    NEXUS_StreamMuxHandle handle,
    const NEXUS_StreamMuxStartSettings *pSettings,
    NEXUS_StreamMuxOutput *pMuxOutput
    );

/**
Summary:
Finalize all data to Playpump for an impending stop

Description:
For a clean shutdown, call NEXUS_StreamMux_Finish, wait for finished callback, then call NEXUS_StreamMux_Stop.
**/
void NEXUS_StreamMux_Finish(
    NEXUS_StreamMuxHandle handle
    );

/**
Summary:
Stop muxing data immediately.
**/
void NEXUS_StreamMux_Stop(
    NEXUS_StreamMuxHandle handle
    );

/**
Summary:
System data (e.g. PSI) to be sent using NEXUS_StreamMux_AddSystemDataBuffers
**/
typedef struct NEXUS_StreamMuxSystemData
{
    unsigned timestampDelta; /* (in milliseconds) Time this packet should start to be sent out
                              relative to the start of the previous system data buffer.
                              "0" indicates to send the packet out ASAP. */
    size_t size; /* size of pData in bytes. Must be multiple of TS Packet size (188 bytes) */
    const void *pData; /* attr{memory=cached} address of data to be muxed. must be allocated using NEXUS_Memory_Allocation. 
                          memory pointed to by pData must remain intact until NEXUS_StreamMux_GetCompletedSystemDataBuffers indicates
                          that is is completed. */
} NEXUS_StreamMuxSystemData;

/**
Summary:
**/
void NEXUS_StreamMux_GetDefaultSystemData(
    NEXUS_StreamMuxSystemData *pSystemDataBuffer /* [out] */
    );

/**
Summary:
Add system data to the stream with a specified delay
**/
NEXUS_Error NEXUS_StreamMux_AddSystemDataBuffer(
    NEXUS_StreamMuxHandle handle,
    const NEXUS_StreamMuxSystemData *pSystemDataBuffer /* system data buffer */
    );

/**
Summary:
Learn what system data has been sent into the outgoing stream

Description:
Data is completed in FIFO order. Caller must keep track of submitted and completed entries.
**/
void NEXUS_StreamMux_GetCompletedSystemDataBuffers(
    NEXUS_StreamMuxHandle handle,
    size_t *pCompletedCount /* [out] */
    );
         
/**
Summary:
**/
typedef struct NEXUS_StreamMuxStatus
{
    unsigned duration; /* file duration (in miliseconds) completed at current time */
} NEXUS_StreamMuxStatus;

/**
Summary:
**/
NEXUS_Error NEXUS_StreamMux_GetStatus(
    NEXUS_StreamMuxHandle handle,
    NEXUS_StreamMuxStatus *pStatus /* [out] */
    );

/**
Summary:
**/
void NEXUS_StreamMux_GetDefaultConfiguration(
    NEXUS_StreamMuxConfiguration *pConfiguration /* [out] */
    );

/**
Summary: 
Return nenmory configuration for given mux configuration.

Description:
Different mux configuration may require different amount of memory. This function would return memory budged required for given configuration.
This configuration would need then to be passed into the NEXUS_StreamMux_Create. If application wants to support multiple different configurations
with the same instance of the NEXUS_StreamMux, it would need to call this funcation for each configuration and combine results.
**/
void NEXUS_StreamMux_GetMemoryConfiguration(
    const NEXUS_StreamMuxConfiguration *pConfiguration,
    NEXUS_StreamMuxMemoryConfiguration *pMemoryConfiguration
    );


#ifdef __cplusplus
}
#endif


#endif /* NEXUS_STREAM_MUX_H__ */



