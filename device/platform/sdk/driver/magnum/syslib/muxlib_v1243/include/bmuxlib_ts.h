/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_ts.h $
 * $brcm_Revision: Hydra_Software_Devel/42 $
 * $brcm_Date: 4/5/13 5:29p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/include/bmuxlib_ts.h $
 * 
 * Hydra_Software_Devel/42   4/5/13 5:29p delkert
 * SW7425-4643: Update comments regarding cached memory, and PID
 * conflicts.
 *
 * Hydra_Software_Devel/41   7/20/12 10:33a nilesh
 * SW7425-3076: Removed deprecated settings
 *
 * Hydra_Software_Devel/40   7/18/12 1:02p nilesh
 * SW7425-3076: Updated Memory API
 *
 * Hydra_Software_Devel/39   6/27/12 11:10a nilesh
 * SW7425-3076: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-3076/9   6/26/12 11:54a nilesh
 * SW7425-3076: Updated public API for providing memory buffer info
 *
 * Hydra_Software_Devel/SW7425-3076/8   6/20/12 1:17p nilesh
 * SW7425-3076: Rebase from mainline
 *
 * Hydra_Software_Devel/38   6/5/12 12:33p nilesh
 * SW7425-3076: Merge MSP and memory optimizations to mainline
 *
 * Hydra_Software_Devel/SW7425-3076/7   6/20/12 12:31p nilesh
 * SW7425-3076: Updated memory config APIs
 *
 * Hydra_Software_Devel/SW7425-3076/6   6/20/12 11:59a nilesh
 * SW7425-3076: Use separate heaps for system and device memory
 *
 * Hydra_Software_Devel/SW7425-3076/5   6/7/12 4:44p nilesh
 * SW7425-3076: Add sub heap support
 *
 * Hydra_Software_Devel/SW7425-3076/4   6/6/12 5:06p nilesh
 * SW7425-3076: Added size calculations. Converted to using BMEM for
 * userdata.
 *
 * Hydra_Software_Devel/SW7425-3076/3   6/5/12 2:58p nilesh
 * SW7425-3076: Added BMUXlib_TS_GetMemoryConfig()
 *
 * Hydra_Software_Devel/SW7425-3076/2   6/4/12 5:12p nilesh
 * SW7425-3076: Clean up
 *
 * Hydra_Software_Devel/SW7425-3076/1   6/4/12 1:44p nilesh
 * SW7425-3076: Reduced memory footprint.  Implemented variable MSP
 * algorithm to prevent underflow.
 *
 * Hydra_Software_Devel/37   4/24/12 5:19p nilesh
 * SW7425-2606: Added BMUXlib_TS_StartSettings.uiServicePeriod
 *
 * Hydra_Software_Devel/36   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/3   2/16/12 4:22p nilesh
 * SW7425-2217: Rebase from mainline
 *
 * Hydra_Software_Devel/35   2/16/12 1:01p nilesh
 * SW7425-2217: Reduced max video pids to 1
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/13/12 9:11a delkert
 * SW7425-2217: Initial working userdata pass-thru
 *
 * Hydra_Software_Devel/SW7425-2217/1   1/25/12 2:03p delkert
 * SW7425-2217: Initial userdata API
 *
 * Hydra_Software_Devel/31   1/18/12 5:46p nilesh
 * SW7425-2174: Fixed stall detection
 *
 * Hydra_Software_Devel/30   11/9/11 10:46a nilesh
 * SW7425-1707: Changed uiInstance/uiOutputInstance to uiMuxId
 *
 * Hydra_Software_Devel/29   11/9/11 10:01a nilesh
 * SW7425-1707: Added support for setting
 * BMUXlib_Input_CreateSettings.uiOutputInstance
 *
 * Hydra_Software_Devel/28   10/28/11 9:04a delkert
 * SW7425-1494: Remove redundant code. Improved error checking as a result
 * of API testing.
 *
 * Hydra_Software_Devel/27   10/27/11 3:45p nilesh
 * SW7425-1236: Updated max audio/video PIDs to 16.  Increased max
 * transport instances count accordingly.
 *
 * Hydra_Software_Devel/26   6/8/11 12:00a nilesh
 * SW7425-484: Rename uiPacingCounterValue to uiPacingCounter.  Adjust
 * uiPacingCounter by mux delay.
 *
 * Hydra_Software_Devel/25   6/7/11 11:31p nilesh
 * SW7425-484: In non real time mode, TS muxlib waits for descriptors to
 * be available on all active inputs before initializing PCR and pacing
 * counter.
 *
 * Hydra_Software_Devel/24   5/26/11 4:34p nilesh
 * SW7425-484: Added non-realtime support
 *
 * Hydra_Software_Devel/23   2/28/11 6:49p nilesh
 * SW7425-111: Commented uiMuxDelay
 *
 * Hydra_Software_Devel/22   2/7/11 11:02a nilesh
 * SW7425-38: Removed unused mux mode setting
 *
 * Hydra_Software_Devel/21   2/4/11 10:37a nilesh
 * SW7425-38: Commented out unnecessary callback APIs
 *
 * Hydra_Software_Devel/20   2/4/11 10:21a nilesh
 * SW7425-38: Fixed BMUXlib_TS_Finish() comment
 *
 * Hydra_Software_Devel/19   2/3/11 4:10p nilesh
 * SW7425-38: API Changes
 *
 * Hydra_Software_Devel/18   2/2/11 2:16p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddSystemDataBuffers() prototype
 *
 * Hydra_Software_Devel/17   2/2/11 1:28p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddTransportDescriptors() prototype
 *
 * Hydra_Software_Devel/16   2/2/11 11:38a nilesh
 * SW7425-38: Add streamID support
 *
 * Hydra_Software_Devel/15   2/1/11 4:59p nilesh
 * SW7425-38: stream id to StartSettings
 *
 * Hydra_Software_Devel/14   1/31/11 5:56p nilesh
 * SW7425-38: Changed DoMux API
 *
 * Hydra_Software_Devel/13   1/28/11 2:43p nilesh
 * SW7425-38: Change GetDefaultXXX() return value to void
 *
 * Hydra_Software_Devel/12   12/14/10 1:45p nilesh
 * SW7425-38: Add queuing of system data into muxlib
 *
 * Hydra_Software_Devel/11   12/8/10 4:44p nilesh
 * SW7425-38: Updated to use common BAVC_TsMux_DescConfig
 *
 * Hydra_Software_Devel/10   11/18/10 4:09p nilesh
 * SW7425-38: Removed audio/video protocols. Protocol is now specified in
 * audio/video buffer descriptor.
 *
 * Hydra_Software_Devel/9   10/7/10 4:30p nilesh
 * SW7425-38: In BMUXlib_TS_StartSettings, removed uiInterval, renamed
 * uiIntervalMaxLatency to uiServiceLatencyTolerance, renamed
 * stOutputInterface to stChannelInterface, and renamed uiTransportIndex
 * to uiTransportChannelIndex.  Changed uint32_t to unsigned as needed.
 *
 * Hydra_Software_Devel/8   10/5/10 12:40p nilesh
 * SW7425-38: Updated to use transport device/channel
 *
 * Hydra_Software_Devel/7   10/4/10 2:18p nilesh
 * SW7425-38: Added PCR pacing
 *
 * Hydra_Software_Devel/6   9/28/10 4:46p nilesh
 * SW7425-38: Updated comments to clarify uncached virtual address usage
 *
 * Hydra_Software_Devel/5   9/24/10 12:09p nilesh
 * SW7425-38: Updated transport and system data interface.  Added debug
 * dump of video descriptors.
 *
 * Hydra_Software_Devel/4   9/23/10 10:57a nilesh
 * SW7425-38:  Added system data type enum.  Changed
 * EnqueueSystemDataBuffers to AddSystemDataBuffers and to accept both
 * the first and last linked list descriptor pointer.
 *
 * Hydra_Software_Devel/3   9/22/10 4:05p nilesh
 * SW7425-38: Stubbed BMUXlib functions
 *
 * Hydra_Software_Devel/2   9/21/10 5:27p nilesh
 * SW7425-38: Added GetDefaultCreateSettings/Create/Destroy functions
 *
 * Hydra_Software_Devel/1   9/20/10 4:43p nilesh
 * SW7425-38:
 * Added file element "bmuxlib_ts.c".
 * Added file element "bmuxlib_ts.h".
 * SW7425-38: Merged muxlib to mainline
 *
 * Hydra_Software_Devel/SW7425-38/1   9/17/10 12:31p nilesh
 * SW7425-38: Initial TS Muxer definition
 *
 ***************************************************************************/

#ifndef BMUXLIB_TS_H_
#define BMUXLIB_TS_H_

#include "bmuxlib.h"
#include "bavc_xpt.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* 7425 Specific Notes:
 *  - PCR and all other periodic system data need to use the same XPT PB Handle
 *  - Audio needs a dedicated XPT PB Handle
 *  - Video needs a dedicated XPT PB Handle
 *  - Only one video PID supported
 *  - Only one audio PID supported
 *  - PCR PID cannot be the same as video or audio PIDs
 */
#define BMUXLIB_TS_MAX_SYS_DATA_BR     27000000
#define BMUXLIB_TS_MIN_SYS_DATA_BR     100000

/***********************/
/* Transport Interface */
/***********************/
typedef struct BMUXlib_TS_TransportDescriptor
{
   const void *pBufferAddress; /* address of data to be muxed. */
   size_t uiBufferLength; /* length in bytes of data to be muxed */

   BAVC_TsMux_DescConfig stTsMuxDescriptorConfig;
} BMUXlib_TS_TransportDescriptor;

/* BMUXlib_AddTransportDescriptors -
 * Adds transport descriptors for muxing.
 */
typedef BERR_Code
(*BMUXlib_TS_AddTransportDescriptors)(
         void *pvContext,
         const BMUXlib_TS_TransportDescriptor *astTransportDescriptors, /* Array of pointers to transport descriptors */
         size_t uiCount, /* Count of descriptors in array */
         size_t *puiQueuedCount /* Count of descriptors queued (*puiQueuedCount <= uiCount) */
         );

/* BMUXlib_GetCompletedTransportDescriptors -
 * Returns the count of transport descriptors completed
 * since the *previous* call to BMUXlib_TS_GetCompletedTransportDescriptors
 */
typedef BERR_Code
(*BMUXlib_TS_GetCompletedTransportDescriptors)(
         void *pvContext,
         size_t *puiCompletedCount /* Count of descriptors completed */
         );

typedef struct BMUXlib_TS_TransportSettings
{
      /* TODO: Add settings */
      uint32_t uiMuxDelay; /* in milliseconds */

      /* Each transport channel's pacing counter is seeded with:
       *   If bNonRealTimeMode=false: STC-uiMuxDelay
       *   If bNonRealTimeMode=true : uiPacingCounter
       */

      bool bNonRealTimeMode; /* Non Real Time Mode (NRT/AFAP) */

      struct
      {
            unsigned uiPacingCounter; /* in 27Mhz clock ticks */
      } stNonRealTimeSettings;
} BMUXlib_TS_TransportSettings;

typedef BERR_Code
(*BMUXlib_TS_GetTransportSettings)(
         void *pvContext,
         BMUXlib_TS_TransportSettings *pstTransportSettings
         );

typedef BERR_Code
(*BMUXlib_TS_SetTransportSettings)(
         void *pvContext,
         const BMUXlib_TS_TransportSettings *pstTransportSettings
         );

typedef struct BMUXlib_TS_TransportStatus
{
      uint64_t uiSTC; /* 42-bit value in 27 Mhz */
      uint32_t uiESCR; /* 32-bit value in 27Mhz; */
      /* TODO: What other status? */
} BMUXlib_TS_TransportStatus;

typedef BERR_Code
(*BMUXlib_TS_GetTransportStatus)(
         void *pvContext,
         BMUXlib_TS_TransportStatus *pstTransportStatus
         );

typedef struct BMUXlib_TS_TransportDeviceInterface
{
      void *pContext;
      BMUXlib_TS_GetTransportSettings fGetTransportSettings;
      BMUXlib_TS_SetTransportSettings fSetTransportSettings;
      BMUXlib_TS_GetTransportStatus fGetTransportStatus;
} BMUXlib_TS_TransportDeviceInterface;

typedef struct BMUXlib_TS_TransportChannelInterface
{
      void *pContext;
      BMUXlib_TS_AddTransportDescriptors fAddTransportDescriptors;
      BMUXlib_TS_GetCompletedTransportDescriptors fGetCompletedTransportDescriptors;
} BMUXlib_TS_TransportChannelInterface;

/***********************/
/* User Data Interface */
/***********************/

/* Subsequent calls to BMUXlib_GetUserDataBuffer are non-destructive.  I.e. Any new data added is appended to the end of the existing buffers
* The userdata is expected to arrive as PES packets encapsulated in TS packets.
* All TS packets in the buffer must have the same PID.
* The buffer must start on a TS packet boundary
*/
typedef BERR_Code
(*BMUXlib_TS_GetUserDataBuffer)(
   void *pvContext,
   const void **pBuffer0,     /* Pointer to a buffer of TS Packets. E.g. *pBuffer0 points to the beginning if the buffer.  Can be NULL if uiLength0=0. */
   size_t *puiLength0,
   const void **pBuffer1,     /* Needed to handle FIFO wrap. Can be NULL if uiLength1=0. */
   size_t *puiLength1
   );

typedef BERR_Code
(*BMUXlib_TS_ConsumeUserDataBuffer)(
   void *pvContext,
   size_t uiNumBytesConsumed
   );

typedef struct BMUXlib_UserDataInterface
{
      void *pContext;
      BMUXlib_TS_GetUserDataBuffer fGetUserDataBuffer;
      BMUXlib_TS_ConsumeUserDataBuffer fConsumeUserDataBuffer;
} BMUXlib_TS_UserDataInterface;

/******************/
/* Create/Destroy */
/******************/
typedef struct BMUXlib_TS_P_Context *BMUXlib_TS_Handle;

typedef struct BMUXlib_TS_MemoryBuffers
{
   void* pSystemBuffer; /* Memory only accessed by the host. If NULL, allocated in BMUXlib_TS_Create() from system/kernel memory. */
   void* pSharedBuffer; /* Memory accessed by both the host and device(s). If NULL, allocated in BMUXlib_TS_Create() from specified hMem. */
} BMUXlib_TS_MemoryBuffers;

typedef struct BMUXlib_TS_MemoryConfig
{
   size_t uiSystemBufferSize; /* Only accessed by the host */
   size_t uiSharedBufferSize; /* Accessed by both the host and device(s) */
} BMUXlib_TS_MemoryConfig;

typedef struct BMUXlib_TS_CreateSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_TS_GetDefaultCreateSettings() */

      BMEM_Handle hMem; /* Required. Must be accessible by the MIPs. */

      BMUXlib_TS_MemoryConfig stMemoryConfig; /* The worst case memory usage for ALL possible mux
                                               * configurations used for this instance of TS Muxlib.
                                               * Should be populated using BMUXlib_TS_GetMemoryConfig()
                                               */
      BMUXlib_TS_MemoryBuffers stMemoryBuffers; /* (OPTIONAL) NULL, allocated in BMUXlib_TS_Create() */

      unsigned uiMuxId;     /* For debug: indicates the ID of this muxlib for multiple transcode systems */
} BMUXlib_TS_CreateSettings;

void
BMUXlib_TS_GetDefaultCreateSettings(
         BMUXlib_TS_CreateSettings *pCreateSettings
         );

/* BMUXlib_TS_Create - Allocates all system/device memory required for mux operation */
BERR_Code
BMUXlib_TS_Create(
         BMUXlib_TS_Handle *phMuxTS,  /* [out] TSMuxer handle returned */
         const BMUXlib_TS_CreateSettings *pstCreateSettings
         );

/* BMUXlib_TS_Destroy - Frees all system/device memory allocated */
BERR_Code
BMUXlib_TS_Destroy(
         BMUXlib_TS_Handle hMuxTS
         );

/****************/
/* Mux Settings */
/****************/
typedef struct BMUXlib_TS_MuxSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_TS_GetDefaultMuxSettings() */

      /* System Data */
      unsigned uiSystemDataBitRate; /* in bits/sec.  1Mbps = 1000000.
                                     * If system data is split across multiple packets,
                                     * this value determines the gap between the packets.
                                     */
} BMUXlib_TS_MuxSettings;

void
BMUXlib_TS_GetDefaultMuxSettings(
         BMUXlib_TS_MuxSettings *pstMuxSettings
         );

BERR_Code
BMUXlib_TS_SetMuxSettings(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_MuxSettings *pstMuxSettings
         );

BERR_Code
BMUXlib_TS_GetMuxSettings(
         BMUXlib_TS_Handle hMuxTS,
         BMUXlib_TS_MuxSettings *pstMuxSettings
         );

/**************/
/* Start/Stop */
/**************/
typedef struct BMUXlib_TS_PCRSystemData
{
      uint16_t uiPID;

      unsigned uiInterval; /* insertion interval (in milliseconds) */

      unsigned uiTransportChannelIndex; /* Which transport interface to use for PCR packets */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
      bool bNonPcrInsert; /* If true, not insert PCR TS packet */
#endif
} BMUXlib_TS_PCRSystemData;

#define BMUXLIB_TS_MAX_VIDEO_PIDS 1
#define BMUXLIB_TS_MAX_AUDIO_PIDS 6
#define BMUXLIB_TS_MAX_INPUT_PIDS ( BMUXLIB_TS_MAX_VIDEO_PIDS + BMUXLIB_TS_MAX_AUDIO_PIDS )
#define BMUXLIB_TS_MAX_SYSTEM_PIDS 1
#define BMUXLIB_TS_MAX_TRANSPORT_INSTANCES (BMUXLIB_TS_MAX_VIDEO_PIDS + BMUXLIB_TS_MAX_AUDIO_PIDS + BMUXLIB_TS_MAX_SYSTEM_PIDS)
#define BMUXLIB_TS_MAX_USERDATA_PIDS 8

typedef struct BMUXlib_TS_StartSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_TS_GetDefaultStartSettings() */

      /* Transport Data */
      struct {
            BMUXlib_TS_TransportDeviceInterface stDeviceInterface;
            BMUXlib_TS_TransportChannelInterface stChannelInterface[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES]; /* The transport interfaces */
      } transport;

      /* Video Data */
      struct {
            BMUXlib_VideoEncoderInterface stInputInterface;
            unsigned uiTransportChannelIndex; /* Which transport channel interface to use for this PID */
            uint16_t uiPID; /* The PID associated with this video data */
            uint8_t uiPESStreamID;
      } video[BMUXLIB_TS_MAX_VIDEO_PIDS];
      unsigned uiNumValidVideoPIDs; /* The number of valid video PIDs in the video encoder interface array */

      /* Audio Data */
      struct {
            BMUXlib_AudioEncoderInterface stInputInterface;
            unsigned uiTransportChannelIndex; /* Which transport channel interface to use for this PID */
            uint16_t uiPID; /* The PID associated with this audio data */
            uint8_t uiPESStreamID;
      } audio[BMUXLIB_TS_MAX_AUDIO_PIDS];
      unsigned uiNumValidAudioPIDs; /* The number of valid audio PIDs in the audio encoder interface array */

      /* System Data */
      BMUXlib_TS_PCRSystemData stPCRData; /* The first PCR packet is inserted before any audio and video
                                           * packets are muxed.  This will ensure that a PCR entry is
                                           * received before any A/V data is received by the decoder
                                           */

      /* User Data */
      /* The userdata is expected to arrive as PES packets encapsulated in TS packets.
         It is assumed that the companion video for the userdata is the first (and expected to be the only)
         video PID - if more than one video PID expected, support can be added at a later date
         It is also assumed that the userdata will be output on the sytem data transport channel.
         If required, support can be added to output to a different channel if/when required
         The application is responsible for ensuring that PIDs used do not conflict with other
         Userdata or System Data PIDs */
      struct {
            BMUXlib_TS_UserDataInterface stUserDataInterface;
      } userdata[BMUXLIB_TS_MAX_USERDATA_PIDS];
      unsigned uiNumValidUserdataPIDs;

   /* Mux Service Period (mux_delay = service_latency_tolerance + service_period) */
   unsigned uiServiceLatencyTolerance; /* (in milliseconds) Indicates the maximum latency
                                        * the muxer is expected to handle without causing
                                        * the transport to underflow.
                                        *
                                        * This value directly affects the overall mux latency
                                        * and the encoder's CDB buffer size
                                        */

   unsigned uiServicePeriod; /* (in milliseconds) Indicates the lowest frequency
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

   bool bNonRealTimeMode;  /* If FALSE: Normal operation for muxing in real time.  Pacing Counters will be based on STC.
                            *
                            * If TRUE: aka As Fast As Possible (AFAP) Mode. Pacing Counters will be based on initial ESCR(s).
                            */
} BMUXlib_TS_StartSettings;

void
BMUXlib_TS_GetDefaultStartSettings(
         BMUXlib_TS_StartSettings *pstStartSettings
         );

/* BMUXlib_TS_Start - Configures the mux HW */
BERR_Code
BMUXlib_TS_Start(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_StartSettings *pstStartSettings
         );

typedef struct BMUXlib_TS_FinishSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_TS_GetDefaultFinishSettings() */

      BMUXlib_FinishMode eFinishMode;
} BMUXlib_TS_FinishSettings;

void
BMUXlib_TS_GetDefaultFinishSettings(
         BMUXlib_TS_FinishSettings *pstFinishSettings
         );

/* BMUXlib_TS_Finish -
 * Schedules the muxer to finish
 * DoMux() returns the following state:
 *    - eFinishingInput: still consuming encoder output
 *    - eFinishingOutput: still muxing data.  encoders can be closed.
 *    - eFinished: muxing complete. muxer can be stopped.
 */
BERR_Code
BMUXlib_TS_Finish(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_FinishSettings *pstFinishSettings
         );

/* BMUXlib_TS_Stop -
 * Flushes/resets the mux HW immediately.
 * For a clean stop, this should only be called after the
 * "finished" event occurs after calling BMUXlib_TS_Finish()
 * This function may need to be called without BMUXlib_TS_Finish()
 * in cases where an abrupt stop is needed (e.g. USB Hard Drive is
 * unplugged).
 */
BERR_Code
BMUXlib_TS_Stop(
         BMUXlib_TS_Handle hMuxTS
         );

/**********/
/* Memory */
/**********/
typedef struct BMUXlib_TS_MuxConfig
{
   BMUXlib_TS_StartSettings stMuxStartSettings;
   BMUXlib_TS_MuxSettings stMuxSettings;
} BMUXlib_TS_MuxConfig;

/* BMUXlib_TS_GetMemoryConfig - Returns the memory required for the specified mux configuration.
 * The following fields are relevant for the memory calculations:
 *
 * BMUXlib_TS_MuxConfig
 *    BMUXlib_TS_StartSettings
 *       uiServicePeriod
 *       uiServiceLatencyTolerance
 *       bNonRealTimeMode
 *       uiNumValidVideoPIDs
 *       uiNumValidAudioPIDs
 *       uiNumValidUserdataPIDs
 *       stPCRData.uiInterval
 *    BMUXlib_TS_MuxSettings
 *       uiSystemDataBitRate
 */
void
BMUXlib_TS_GetMemoryConfig(
         const BMUXlib_TS_MuxConfig *pstMuxConfig,
         BMUXlib_TS_MemoryConfig *pstMemoryConfig
         );

/***************/
/* System Data */
/***************/
/* This API will be used for both periodic and non-periodic data.
 * For Periodic data (PAT/PMT), the app needs to add/queue packets on a periodic basis
 *
 * System Data is scheduled and interleaved with the internally generated PCR packets.
 * If system data packets spans across a PCR packet boundary, the muxer will split the
 * system data packets to ensure PCR packets have highest priority.
 */
typedef struct BMUXlib_TS_SystemData
{
      unsigned uiTimestampDelta; /* (in milliseconds) Time this packet should start to be sent out
                                  * relative to the start of the previous system data buffer.
                                  * "0" indicates to send the packet out ASAP.
                                  */

      size_t uiSize; /* in bytes. Must be multiple of TS Packet size (188 bytes) */
      const void *pData; /* address of data to be muxed.
                          * Formatted as TS Packets.
                          * CC counter must be set by the application.
                          * User responsible for ensuring PIDs do not conflict
                          * with other System Data or Userdata PIDs
                          */
} BMUXlib_TS_SystemData;

/* BMUXlib_TS_AddSystemDataBuffers -
 * Adds system data buffers(s) to the stream.  The buffers that are queued must remain intact until the muxing has completed.
 */
BERR_Code
BMUXlib_TS_AddSystemDataBuffers(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_SystemData *astSystemDataBuffer, /* Array of system data buffers */
         size_t uiCount, /* Count of system data buffers in array */
         size_t *puiQueuedCount /* Count of system data buffers queued by muxer (*puiQueuedCount <= uiCount) */
         );

/* BMUXlib_TS_GetCompletedSystemDataBuffers -
 * Returns the count of transport descriptors completed
 * since the *previous* call to BMUXlib_TS_GetCompletedSystemDataBuffers.
 *
 * Completed buffers can be reused by the application immediately.
 */
BERR_Code
BMUXlib_TS_GetCompletedSystemDataBuffers(
         BMUXlib_TS_Handle hMuxTS,
         size_t *puiCompletedCount /* Returns count of system data buffers fully muxed */
         );

/******************/
/* Suspend/Resume */
/******************/
/* TODO: Suspend/Resume API */

/***********/
/* Execute */
/***********/
/* BMUXlib_TS_DoMux - will get the buffer descriptors for Audio and
 * Video, generate PES headers, PCR packets, etc, as needed and feed
 * LLDs to XPT PB.  It will also update the A/V descriptors muxed.
 */
BERR_Code
BMUXlib_TS_DoMux(
   BMUXlib_TS_Handle hMuxTS,
   BMUXlib_DoMux_Status *pstStatus
   );



#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_TS_H_ */
