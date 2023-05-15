/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/69 $
 * $brcm_Date: 4/9/13 4:31p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_priv.h $
 * 
 * Hydra_Software_Devel/69   4/9/13 4:31p nilesh
 * FWVICE2-457: Added out of order ESCR debug warning
 * 
 * Hydra_Software_Devel/68   4/3/13 2:39p nilesh
 * SW7425-4614: Added support for flushing when ITB/CDB accessed directly
 * 
 * Hydra_Software_Devel/67   4/2/13 2:07p nilesh
 * SW7425-4676: Added support for reporting ARC Watchdog error code
 * 
 * Hydra_Software_Devel/66   3/21/13 12:41p nilesh
 * SW7425-4614: Fixed race condition between app calling Start/FlushEncode
 * and the mux thread calling GetBuffer/ConsumeBuffer using mutexes.
 * Cleaned up interface between output and channel modules.  Fixed
 * problem with flushed descriptors causin CABAC ready to be set
 * prematurely.
 * 
 * Hydra_Software_Devel/65   12/18/12 3:58p nilesh
 * SW7435-562: Added BVCE_Output_OpenSettings.hDescriptorMem
 * 
 * Hydra_Software_Devel/64   12/5/12 3:20p nilesh
 * SW7425-4230: Added BVCE_GetVersionInfo()
 * 
 * Hydra_Software_Devel/63   12/3/12 5:38p nilesh
 * SW7445-39: FW v4.0.7.0
 * 
 * Hydra_Software_Devel/SW7445-39/3   11/20/12 2:31p nilesh
 * SW7445-39: Merge from FWVICE2-294
 * 
 * Hydra_Software_Devel/FWVICE2-294/1   11/8/12 9:07p nilesh
 * FWVICE2-294: Added EOS FW event support
 * 
 * Hydra_Software_Devel/SW7445-39/2   11/16/12 10:55a nilesh
 * SW7445-39: Added support for multiple slices
 * 
 * Hydra_Software_Devel/SW7445-39/1   11/14/12 2:10p nilesh
 * SW7445-39: 7445 Emulation Bring-Up
 * 
 * Hydra_Software_Devel/62   10/11/12 12:50p nilesh
 * SW7425-2568: Updated to use BVCE_Debug logging functions
 * 
 * Hydra_Software_Devel/61   10/11/12 12:09p nilesh
 * SW7425-2568: Updated to use BVCE_Debug logging functions
 * 
 * Hydra_Software_Devel/60   10/5/12 10:17p nilesh
 * SW7445-39: Advanced ITB parser
 * 
 * Hydra_Software_Devel/59   10/3/12 4:26p nilesh
 * SW7425-2718: Added STC snapshot support
 * 
 * Hydra_Software_Devel/58   10/3/12 12:39p nilesh
 * SW7445-39: Added framework for updated ITB parser
 * 
 * Hydra_Software_Devel/57   10/1/12 2:03p nilesh
 * SW7445-39: Split power and output code into separate files
 * 
 * Hydra_Software_Devel/56   8/29/12 2:02p nilesh
 * SW7425-3360,SW7425-3644: FNRT support, Endian Neutral API
 * 
 * Hydra_Software_Devel/SW7425-3360/2   8/15/12 1:53p nilesh
 * SW7425-3360: Rebase from mainline
 * 
 * Hydra_Software_Devel/55   7/31/12 2:25p nilesh
 * SW7425-3622: Removed all uncached memory access
 * 
 * Hydra_Software_Devel/SW7425-3360/1   6/28/12 1:56p nilesh
 * SW7425-3360: Added FNRT support
 * 
 * Hydra_Software_Devel/54   6/27/12 3:51p nilesh
 * SW7435-200: Replaced critical section with mutex
 * 
 * Hydra_Software_Devel/53   3/8/12 11:51a nilesh
 * SW7425-2568: Added BVCE_TEST_MODE compile time option
 * 
 * Hydra_Software_Devel/52   2/14/12 11:46a nilesh
 * SW7425-2220: Fixed BVCE_Output_Reset()
 * 
 * Hydra_Software_Devel/51   2/8/12 1:33p nilesh
 * SW7425-2220: Added fast channel change support.
 * 
 * Hydra_Software_Devel/50   1/25/12 4:50p nilesh
 * SW7425-2220,SW7425-997: Change BVCE_Channel_StopEncode() prototype to
 * support different stop modes
 * 
 * Hydra_Software_Devel/49   1/2/12 3:57p nilesh
 * SW7425-2078: Set Begin New RAP if GOP structure is changed dynamically
 * 
 * Hydra_Software_Devel/48   12/20/11 3:55p nilesh
 * SW7425-1882: Added BVCE_DUMP_USERDATA_LOG build flag
 * 
 * Hydra_Software_Devel/47   12/19/11 11:26a nilesh
 * SW7425-960: Implemented BVCE_Channel_UserData_GetStatus_isr()
 * 
 * Hydra_Software_Devel/46   12/19/11 10:43a nilesh
 * SW7425-1110: Reduce stack usage
 * 
 * Hydra_Software_Devel/45   12/9/11 11:14a nilesh
 * SW7425-1566: Moved MultiChannelMode to
 * BVCE_Channel_OpenSettings.eMultiChannelMode
 * 
 * Hydra_Software_Devel/44   12/5/11 2:15p nilesh
 * SW7425-960: Added VCE user data support
 * 
 * Hydra_Software_Devel/43   11/17/11 3:35p nilesh
 * SW7425-1746: Added BVCE_Channel_BeginNewRAP()
 * 
 * Hydra_Software_Devel/42   11/16/11 4:10p nilesh
 * SW7425-1566: Added BVCE_MultiChannelMode support
 * 
 * Hydra_Software_Devel/41   10/14/11 11:57a nilesh
 * SW7425-1213: Added Telemetry Parser
 * 
 * Hydra_Software_Devel/40   9/26/11 3:46p nilesh
 * SW7425-1213: Added BVCE_DUMP_ARC_DEBUG_LOG build flag to enable dump of
 * raw debug log buffers to file
 * 
 * Hydra_Software_Devel/39   9/26/11 3:21p nilesh
 * SW7425-1213: Added BVCE_Debug_SendCommand support
 * 
 * Hydra_Software_Devel/38   9/14/11 2:55p nilesh
 * SW7425-890: Added support for 2 separate cabac data ready interrupts
 * for 7425 B0
 * 
 * Hydra_Software_Devel/37   8/15/11 5:28p nilesh
 * SW7425-448: Added Active Power Management
 * (BVCE_Power_Standby/BVCE_Power_Resume)
 * 
 * Hydra_Software_Devel/36   8/11/11 11:29a nilesh
 * SW7425-1006: VCE FW API v2.0
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/2   8/10/11 5:29p nilesh
 * SW7425-1006: Merge VCE_FW_API_v1_2 branch to VICE2_FW_Dev branch
 * 
 * Hydra_Software_Devel/35   8/10/11 4:22p nilesh
 * SW7425-448: Added Dynamic Power Management
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/1   7/25/11 4:20p nilesh
 * SW7425-448: Added dynamic power management support
 * 
 * Hydra_Software_Devel/34   7/18/11 2:07p pblanco
 * SW7425-931: Moved ITB structure and macro definitions from bvce.c to
 * bvce_priv.h
 * 
 * Hydra_Software_Devel/33   4/16/11 1:03a nilesh
 * SW7425-171: Added support for pending settings
 * 
 * Hydra_Software_Devel/32   4/13/11 4:29p nilesh
 * SW7425-303: Added phase 2.0 metadata support
 * 
 * Hydra_Software_Devel/31   4/12/11 4:25p nilesh
 * SW7425-303: The video buffer descriptor pointers returned by
 * BVCE_Output_GetBufferDescriptors() are now CACHED bmem addresses
 * 
 * Hydra_Software_Devel/30   4/11/11 5:20p nilesh
 * SW7425-171: Added BVCE_Channel_SetEncodeSettings_OnInputChange() and
 * BVCE_Channel_GetDefaultEncodeSettings_OnInputChange()
 * 
 * Hydra_Software_Devel/29   4/7/11 4:02p nilesh
 * SW7425-75: Add support for video metadata (common)
 * 
 * Hydra_Software_Devel/28   4/7/11 11:19a nilesh
 * SW7425-287: Optimize Data Unit detection loop to maximize use of local
 * variables
 * 
 * Hydra_Software_Devel/27   4/7/11 10:53a nilesh
 * SW7425-287: Optimized cache flush to only flush region containing new
 * ITB/CDB data
 * 
 * Hydra_Software_Devel/26   4/5/11 4:37p nilesh
 * SW7425-172,SW7425-173: Added device (watchdog) and channel (event)
 * callback suppport
 * 
 * Hydra_Software_Devel/25   3/29/11 4:09p nilesh
 * SW7425-74: Added H.264 NALU detection
 * 
 * Hydra_Software_Devel/24   3/7/11 10:32p nilesh
 * SW7425-1: Added BVCE_DUMP_OUTPUT_ITB_DESC dbg dump compile flag
 * 
 * Hydra_Software_Devel/23   3/7/11 2:18p nilesh
 * SW7425-1: Added compile time debug flags BVCE_DUMP_OUTPUT_[ITB/CDB]
 * 
 * Hydra_Software_Devel/22   2/22/11 5:44p nilesh
 * SW7425-102: Cleaned up end-to-end delay API
 * 
 * Hydra_Software_Devel/21   2/22/11 3:27p nilesh
 * SW7425-1: Phase 1.0 FW RC
 * 
 * Hydra_Software_Devel/20   2/15/11 1:31p nilesh
 * SW7425-96: Cleaned up heap logic to minimize usage of mapped system
 * heap
 * 
 * Hydra_Software_Devel/19   2/1/11 4:17p nilesh
 * SW7425-1: Combined common portions of audio/video compressed buffer
 * descriptors
 * 
 * Hydra_Software_Devel/18   2/1/11 12:59p nilesh
 * SW7425-1: Fixed frame start detection if a picture is larger than CDB
 * depth
 * 
 * Hydra_Software_Devel/17   1/26/11 5:41p nilesh
 * SW7425-1: Fix MB Arc debug log
 * 
 * Hydra_Software_Devel/16   1/25/11 10:42a nilesh
 * SW7425-1: Increased max video descriptors to 512
 * 
 * Hydra_Software_Devel/15   1/18/11 3:13p nilesh
 * SW7425-1: Added EOS ITB Support
 * 
 * Hydra_Software_Devel/14   1/17/11 1:54p nilesh
 * SW7425-1: Updated FW, Added ITB/CDB buffer/full ambiguity fix (actual
 * READ ptr now points to byte last read instead of next byte to be
 * read), Using WRITE-1 instead of VALID to work around bug where VALID
 * only updated at the end of a picture.
 * 
 * Hydra_Software_Devel/13   12/29/10 3:49p nilesh
 * SW7425-1: Now using memory sizes provided in bvce_fw_api.h
 * 
 * Hydra_Software_Devel/12   12/6/10 11:52a nilesh
 * SW7425-1: Moved mailbox register config to platform config
 * 
 * Hydra_Software_Devel/11   11/18/10 4:26p nilesh
 * SW7425-1: Populate protocol in video buffer descriptor
 * 
 * Hydra_Software_Devel/10   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 * 
 * Hydra_Software_Devel/9   11/2/10 12:25p nilesh
 * SW7425-1: Added Debug Log Support for Pic Arc
 * 
 * Hydra_Software_Devel/8   10/29/10 11:13p nilesh
 * SW7425-1: Fixed ITB/CDB Buffer Management.  Implemented low-delay mode.
 * 
 * Hydra_Software_Devel/7   10/21/10 4:57p nilesh
 * SW7425-1: Fixed BVN Metadata Register Address Reporting
 * 
 * Hydra_Software_Devel/6   10/21/10 11:03a nilesh
 * SW7425-1: Support query of BVN2VICE metadata offset via channel status
 * 
 * Hydra_Software_Devel/5   10/20/10 4:55p nilesh
 * SW7425-1: Updated memory parameters to match firmware
 * 
 * Hydra_Software_Devel/4   10/13/10 5:04p nilesh
 * SW7425-1: Added error interrupt callback support.  Implemented
 * BVCE_Channel_[Get/Clear]Status.
 * 
 * Hydra_Software_Devel/3   10/13/10 12:10p nilesh
 * SW7425-1: Implemented BVCE_Channel_[Open/Close],
 * BVCE_Channel_[Start/Stop]Encode, and
 * BVCE_Channel_[Set/Get]EncodeSettings
 * 
 * Hydra_Software_Devel/2   10/12/10 1:44p nilesh
 * SW7425-1: Implemented subheap allocation.  Implemented BVCE_Output_xxx
 * functions.
 * 
 * Hydra_Software_Devel/1   10/7/10 11:09a nilesh
 * SW7425-1: Implemented BVCE_Open/BVCE_Close
 *
 ***************************************************************************/

#ifndef BVCE_PRIV_H_
#define BVCE_PRIV_H_

#include "bvce_fw_api.h"
#include "bvce_platform.h"
#include "bvce_telem.h"
#include "bkni_multi.h"
#include "bvce_debug.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* DEBUG */
#ifndef BVCE_P_DUMP_OUTPUT_CDB
#define BVCE_P_DUMP_OUTPUT_CDB 0
#endif
#ifndef BVCE_P_DUMP_OUTPUT_ITB
#define BVCE_P_DUMP_OUTPUT_ITB 0
#endif

/* Signatures */
#define BVCE_P_SIGNATURE_OPENSETTINGS 0x86676901
#define BVCE_P_SIGNATURE_OUTPUTOPENSETTINGS 0x86676902
#define BVCE_P_SIGNATURE_ALLOCBUFFERSSETTINGS 0x86676903
#define BVCE_P_SIGNATURE_CHANNELOPENSETTINGS 0x86676904
#define BVCE_P_SIGNATURE_CHANNELENCODESETTINGS 0x86676905
#define BVCE_P_SIGNATURE_STARTENCODESETTINGS 0x86676906
#define BVCE_P_SIGNATURE_DEVICECALLBACKSETTINGS 0x86676907
#define BVCE_P_SIGNATURE_CHANNELCALLBACKSETTINGS 0x86676908
#define BVCE_P_SIGNATURE_CHANNELENCODESETTINGSONINPUTCHANGE 0x86676909
#define BVCE_P_SIGNATURE_ENCODESETTINGS 0x8667690A
#define BVCE_P_SIGNATURE_STOPENCODESETTINGS 0x8667690B

#define BVCE_P_SIGNATURE_DEVICEHANDLE 0x86676981
#define BVCE_P_SIGNATURE_OUTPUTHANDLE 0x86676982
#define BVCE_P_SIGNATURE_CHANNELHANDLE 0x86676984

#define BVCE_P_DEFAULT_CDB_SIZE MIN_CDB_SIZE_IN_BYTES
#define BVCE_P_DEFAULT_CDB_ALIGNMENT 8
#define BVCE_P_DEFAULT_ITB_SIZE MIN_ITB_SIZE_IN_BYTES
#define BVCE_P_DEFAULT_ITB_ALIGNMENT 8

#ifndef MIN_GENERAL_BUFFER_SIZE_IN_BYTES
#define MIN_GENERAL_BUFFER_SIZE_IN_BYTES 0
#endif

#define BVCE_P_DEFAULT_DEBUG_LOG_SIZE (512*1024)

#define BVCE_P_DEFAULT_USER_DATA_QUEUE_SIZE 32

#define BVCE_P_FIRMWARE_BOOT_TIMEOUT 1000
#define BVCE_P_FIRMWARE_COMMAND_TIMEOUT 1000

#define BVCE_P_MAX_VIDEODESCRIPTORS 3192

#define BVCE_P_MAX_METADATADESCRIPTORS 8

typedef union BVCE_P_Command
{
      uint32_t data[HOST_CMD_BUFFER_SIZE / sizeof(uint32_t)];

      union {
            struct {
                  uint32_t uiCommand;
            } stGeneric;
            Vice2CmdInit_t stInit;
            Vice2CmdOpenChannel_t stOpenChannel;
            Vice2CmdConfigChannel_t stConfigChannel;
            Vice2CmdStartChannel_t stStartChannel;
            Vice2CmdStopChannel_t stStopChannel;
            Vice2CmdCloseChannel_t stCloseChannel;
            Vice2CmdGetChannelStatus_t stGetChannelStatus;
            Vice2CmdDebugChannel_t stDebugChannel;
      } type;
} BVCE_P_Command;

typedef union BVCE_P_Response
{
      uint32_t data[HOST_CMD_BUFFER_SIZE / sizeof(uint32_t)];

      union
      {
            struct {
                  uint32_t uiCommand;
                  uint32_t uiStatus;
            } stGeneric;
            Vice2CmdInitResponse_t stInit;
            Vice2CmdOpenChannelResponse_t stOpenChannel;
            Vice2CmdGetChannelStatusResponse_t stGetChannelStatus;
      } type;
} BVCE_P_Response;

typedef enum BVCE_P_Status
{
   BVCE_P_Status_eIdle,
   BVCE_P_Status_eOpened,
   BVCE_P_Status_eStarted,
   BVCE_P_Status_eStopping,

   BVCE_P_Status_eMax
} BVCE_P_Status;

BVCE_P_Status
BVCE_Channel_P_GetState(
   BVCE_Channel_Handle hVceCh
   );

void
BVCE_Channel_P_GetStartEncodeSettings(
   BVCE_Channel_Handle hVceCh,
   BVCE_Channel_StartEncodeSettings *pstStartEncodeSettings
   );

void
BVCE_Channel_P_GetEncodeSettings(
   BVCE_Channel_Handle hVceCh,
   BVCE_Channel_EncodeSettings *pstEncodeSettings
   );

void
BVCE_Channel_P_HandleEOSEvent(
   BVCE_Channel_Handle hVceCh
   );

typedef enum BVCE_P_HeapId
{
   BVCE_P_HeapId_eSystem,
   BVCE_P_HeapId_eGeneral,
   BVCE_P_HeapId_ePicture,
   BVCE_P_HeapId_eSecure,

   BVCE_P_HeapId_eMax
} BVCE_P_HeapId;

typedef struct BVCE_P_SubHeap
{
      void *pBufferUnCached;
      uint32_t uiBufferOffset;
      void *pBufferCached;
      size_t uiBufferSize;

      BMEM_Heap_Handle hMem;
} BVCE_P_SubHeap;

typedef struct BVCE_P_OutputBuffers
{
      BVCE_Output_AllocBuffersSettings stSettings;

      BMEM_Handle hITBMem;
      BMEM_Handle hCDBMem;

      void *pITBBufferUnCached;
      void *pITBBufferCached;
      uint32_t uiITBBufferOffset;

      void *pCDBBufferUnCached;
      void *pCDBBufferCached;
      uint32_t uiCDBBufferOffset;
} BVCE_P_OutputBuffers;

typedef struct BVCE_P_Output_Cache
{
      bool bValid;
      BVCE_Channel_StartEncodeSettings stStartEncodeSettings;
      BVCE_Channel_EncodeSettings stEncodeSettings;
} BVCE_P_Output_Cache;

typedef struct BVCE_P_Output_BufferCache
{
      uint32_t uiITBCacheValidOffset;
      uint32_t uiCDBCacheValidOffset;
} BVCE_P_Output_BufferCache;

#define BVCE_P_ITBENTRY_ENTRYTYPE_OFFSET 0
#define BVCE_P_ITBENTRY_ENTRYTYPE_SHIFT 24
#define BVCE_P_ITBENTRY_ENTRYTYPE_MASK 0xFF

/* Base Entry */
#define BVCE_P_ITBENTRY_ERROR_OFFSET 0
#define BVCE_P_ITBENTRY_ERROR_SHIFT 23
#define BVCE_P_ITBENTRY_ERROR_MASK 0x00000001

#define BVCE_P_ITBENTRY_CDBADDRESS_OFFSET 1
#define BVCE_P_ITBENTRY_CDBADDRESS_SHIFT 0
#define BVCE_P_ITBENTRY_CDBADDRESS_MASK 0xFFFFFFFF

/* Timestamp Entry */
#define BVCE_P_ITBENTRY_DTSVALID_OFFSET 0
#define BVCE_P_ITBENTRY_DTSVALID_SHIFT 15
#define BVCE_P_ITBENTRY_DTSVALID_MASK 0x00000001

#define BVCE_P_ITBENTRY_PTS32_OFFSET 0
#define BVCE_P_ITBENTRY_PTS32_SHIFT 1
#define BVCE_P_ITBENTRY_PTS32_MASK 0x00000001

#define BVCE_P_ITBENTRY_DTS32_OFFSET 0
#define BVCE_P_ITBENTRY_DTS32_SHIFT 0
#define BVCE_P_ITBENTRY_DTS32_MASK 0x00000001

#define BVCE_P_ITBENTRY_PTS_OFFSET 1
#define BVCE_P_ITBENTRY_PTS_SHIFT 0
#define BVCE_P_ITBENTRY_PTS_MASK 0xFFFFFFFF

#define BVCE_P_ITBENTRY_DTS_OFFSET 2
#define BVCE_P_ITBENTRY_DTS_SHIFT 0
#define BVCE_P_ITBENTRY_DTS_MASK 0xFFFFFFFF

#define BVCE_P_ITBENTRY_IFRAME_OFFSET 3
#define BVCE_P_ITBENTRY_IFRAME_SHIFT 8
#define BVCE_P_ITBENTRY_IFRAME_MASK 0x00000001

/* Transmission Entry */
#define BVCE_P_ITBENTRY_SHR_OFFSET 1
#define BVCE_P_ITBENTRY_SHR_SHIFT 16
#define BVCE_P_ITBENTRY_SHR_MASK 0x0000FFFF

#define BVCE_P_ITBENTRY_TICKSPERBIT_OFFSET 1
#define BVCE_P_ITBENTRY_TICKSPERBIT_SHIFT 0
#define BVCE_P_ITBENTRY_TICKSPERBIT_MASK 0x0000FFFF

/* ESCR Entry */
#define BVCE_P_ITBENTRY_ESCR_OFFSET 1
#define BVCE_P_ITBENTRY_ESCR_SHIFT 0
#define BVCE_P_ITBENTRY_ESCR_MASK 0xFFFFFFFF

#define BVCE_P_ITBENTRY_OPTS_OFFSET 2
#define BVCE_P_ITBENTRY_OPTS_SHIFT 0
#define BVCE_P_ITBENTRY_OPTS_MASK 0xFFFFFFFF

#define BVCE_P_ITBENTRY_METADATA_OFFSET 3
#define BVCE_P_ITBENTRY_METADATA_SHIFT 0
#define BVCE_P_ITBENTRY_METADATA_MASK 0xFFFFFFFF

/* STC Entry */
#define BVCE_P_ITBENTRY_STCSNAPSHOT_UPPER_OFFSET 0
#define BVCE_P_ITBENTRY_STCSNAPSHOT_UPPER_SHIFT 0

#define BVCE_P_ITBENTRY_STCSNAPSHOT_UPPER_MASK 0x3FF

#define BVCE_P_ITBENTRY_STCSNAPSHOT_LOWER_OFFSET 1
#define BVCE_P_ITBENTRY_STCSNAPSHOT_LOWER_SHIFT 0
#define BVCE_P_ITBENTRY_STCSNAPSHOT_LOWER_MASK 0xFFFFFFFF

/* CRC Entry */
#define BVCE_P_ITBENTRY_CRCLOAD_OFFSET 0
#define BVCE_P_ITBENTRY_CRCLOAD_SHIFT 21
#define BVCE_P_ITBENTRY_CRCLOAD_MASK 0x00000001

#define BVCE_P_ITBENTRY_CRCCOMPARE_OFFSET 0
#define BVCE_P_ITBENTRY_CRCCOMPARE_SHIFT 20
#define BVCE_P_ITBENTRY_CRCCOMPARE_MASK 0x00000001

#define BVCE_P_ITBENTRY_CRCINIT_OFFSET 1
#define BVCE_P_ITBENTRY_CRCINIT_SHIFT 0
#define BVCE_P_ITBENTRY_CRCINIT_MASK 0xFFFFFFFF

#define BVCE_P_ITBENTRY_CRCREMAINDER_OFFSET 2
#define BVCE_P_ITBENTRY_CRCREMAINDER_SHIFT 0
#define BVCE_P_ITBENTRY_CRCREMAINDER_MASK 0xFFFFFFFF

#define BVCE_P_ITBEntry_Get(_pentry, _field) ((((uint32_t*)(_pentry))[BVCE_P_ITBENTRY_##_field##_OFFSET] >> BVCE_P_ITBENTRY_##_field##_SHIFT ) & BVCE_P_ITBENTRY_##_field##_MASK )

#define BVCE_P_ITBEntry_GetEntryType(_pentry) BVCE_P_ITBEntry_Get(_pentry, ENTRYTYPE)
#define BVCE_P_ITBEntry_GetCDBAddress(_pentry) BVCE_P_ITBEntry_Get(_pentry, CDBADDRESS)
#define BVCE_P_ITBEntry_GetError(_pentry) BVCE_P_ITBEntry_Get(_pentry, ERROR)
#define BVCE_P_ITBEntry_GetDTS(_pentry) ( ( ( (uint64_t) BVCE_P_ITBEntry_Get(_pentry, DTS32)) << 32 ) | BVCE_P_ITBEntry_Get(_pentry, DTS) )
#define BVCE_P_ITBEntry_GetDTSValid(_pentry) BVCE_P_ITBEntry_Get(_pentry, DTSVALID)
#define BVCE_P_ITBEntry_GetPTS(_pentry) ( ( ( (uint64_t) BVCE_P_ITBEntry_Get(_pentry, PTS32)) << 32 ) | BVCE_P_ITBEntry_Get(_pentry, PTS) )
#define BVCE_P_ITBEntry_GetIFrame(_pentry) BVCE_P_ITBEntry_Get(_pentry, IFRAME)
#define BVCE_P_ITBEntry_GetTicksPerBit(_pentry) ((uint16_t) BVCE_P_ITBEntry_Get(_pentry, TICKSPERBIT))
#define BVCE_P_ITBEntry_GetSHR(_pentry) ((int16_t) BVCE_P_ITBEntry_Get(_pentry, SHR))
#define BVCE_P_ITBEntry_GetESCR(_pentry) BVCE_P_ITBEntry_Get(_pentry, ESCR)
#define BVCE_P_ITBEntry_GetOriginalPTS(_pentry) BVCE_P_ITBEntry_Get(_pentry, OPTS)
#define BVCE_P_ITBEntry_GetMetadata(_pentry) BVCE_P_ITBEntry_Get(_pentry, METADATA)
#define BVCE_P_ITBEntry_GetStcSnapshot(_pentry) ( ( ( (uint64_t) BVCE_P_ITBEntry_Get(_pentry, STCSNAPSHOT_UPPER)) << 32 ) | BVCE_P_ITBEntry_Get(_pentry, STCSNAPSHOT_LOWER) )
#define BVCE_P_ITBEntry_GetCRCLoad(_pentry) BVCE_P_ITBEntry_Get(_pentry, CRCLOAD)
#define BVCE_P_ITBEntry_GetCRCCompare(_pentry) BVCE_P_ITBEntry_Get(_pentry, CRCCOMPARE)
#define BVCE_P_ITBEntry_GetCRCInit(_pentry) BVCE_P_ITBEntry_Get(_pentry, CRCINIT)
#define BVCE_P_ITBEntry_GetCRCRemainder(_pentry) BVCE_P_ITBEntry_Get(_pentry, CRCREMAINDER)

typedef struct BVCE_P_Output_ITB_IndexEntry
{
   unsigned uiSizeInITB;
   bool bError;
   uint32_t uiMetadata;
   unsigned uiCDBAddress;
   BAVC_VideoBufferDescriptor stFrameDescriptor;
} BVCE_P_Output_ITB_IndexEntry;

typedef enum BVCE_P_Output_BufferAccessMode
{
   BVCE_P_Output_BufferAccessMode_eUnknown = 0, /* eUnknown if BVCE_Output_GetRegisters() and BVCE_Output_ConsumeBufferDescriptors() weren't called */
   BVCE_P_Output_BufferAccessMode_eDirect = 1,  /* eDirect if BVCE_Output_GetRegisters() was called and BVCE_Output_ConsumeBufferDescriptors() was never called */
   BVCE_P_Output_BufferAccessMode_eDescriptor   /* eDescriptor if BVCE_Output_ConsumeBufferDescriptors() was called */
} BVCE_P_Output_BufferAccessMode;

typedef struct BVCE_P_Output_Context
{
      uint32_t uiSignature;

      BVCE_Handle hVce;
      BVCE_P_Status eStatus;
      BVCE_Output_OpenSettings stOpenSettings;
      BAVC_VideoContextMap stRegisters;
      BVCE_OutputBuffers_Handle hOutputBuffers;
      BAVC_VideoBufferDescriptor *astDescriptorsUnCached;
      BAVC_VideoBufferDescriptor *astDescriptors;
      BAVC_VideoMetadataDescriptor *astMetadataDescriptorsUnCached;
      BAVC_VideoMetadataDescriptor *astMetadataDescriptors;
      BKNI_MutexHandle hMutex;

      BVCE_P_Output_ITB_IndexEntry *astIndex;

      struct
      {
         BVCE_Channel_Handle hVceCh;
         bool bCabacInitialized;

         struct
         {
            uint32_t uiShadowReadOffset; /* Points to the ITB entry that needs to be parsed next */
            uint32_t uiShadowDepth;

            uint32_t uiBaseOffset;
            uint32_t uiEndOffset;
            uint32_t uiValidOffset;
            uint32_t uiReadOffset;
            uint32_t uiWriteOffset;

            uint32_t uiIndexWriteOffset;
            uint32_t uiIndexReadOffset;
            uint32_t uiIndexShadowReadOffset;

            bool bReadHackDone;
         } stITBBuffer;

         uint32_t uiCDBBufferShadowReadOffset; /* Points to the CDB location that needs to be muxed next */
         uint32_t uiCDBBufferShadowValidOffset;
         uint8_t uiCDBBufferShadowValidOffsetDataUnitDetectionDelay;
         uint32_t uiCDBBufferReadAheadValidOffset;

         uint32_t uiDescriptorWriteOffset;
         uint32_t uiDescriptorReadOffset;

         uint32_t uiMetadataDescriptorWriteOffset;
         uint32_t uiMetadataDescriptorReadOffset;

         /* 7425A0: The following is a hack to prevent HW/SW from getting confused about CDB buffer
          * empty vs fullness.  We always offset the ITB/CDB read pointers by 1 less than what was
          * actually read so that the HW thinks there's always at least 1 byte in the CDB buffer.
          * This essentially changes the meaning of the READ pointer from "the byte TO BE read NEXT"
          * to "the bytes THAT WAS read LAST"
          */
         bool bCDBReadHackDone;
         bool bEOSITBEntrySeen;
         bool bEOSDescriptorSent;
         bool bDataUnitDetected;
         uint8_t uiDataUnitType;
         /* 7425A0: END HACK */

         bool bFrameStart;

         BVCE_P_Output_Cache stChannelCache;
         uint32_t uiDataUnitDetectionShiftRegister;

         BVCE_P_Output_BufferCache stBufferCache;

         bool bMetadataSent;
         unsigned uiMetadataPreviousChunkId;

         unsigned uiPendingDescriptors;
         unsigned uiConsumedDescriptors;

         BVCE_P_Output_BufferAccessMode eBufferAccessMode;

         bool bPreviousESCRValid;
         uint32_t uiPreviousESCR;

         bool bChannelStatusValid;
         BVCE_Channel_Status stChannelStatus;
      } state;

      BMEM_Handle hDescriptorMem;

#if BVCE_P_DUMP_OUTPUT_CDB
      BVCE_Debug_Log_Handle hCDBDumpFile;
#endif
#if BVCE_P_DUMP_OUTPUT_ITB
      BVCE_Debug_Log_Handle hITBDumpFile;
#endif
#if BVCE_P_DUMP_OUTPUT_ITB_DESC
      BVCE_Debug_Log_Handle hITBDescDumpFile;
#endif
#if BVCE_P_TEST_MODE
      BVCE_Debug_Log_Handle hDescriptorLog;
      unsigned uiDescriptorLogInstance;
      unsigned uiDescriptorCount;
#endif
} BVCE_P_Output_Context;

typedef struct BVCE_P_Channel_Context
{
      uint32_t uiSignature;

      BVCE_Handle hVce;
      BVCE_P_Status eStatus;
      BVCE_Channel_OpenSettings stOpenSettings;

      struct
      {
            BMEM_Handle hMem;
            size_t uiBufferSize;
            void *pBufferUnCached;
            uint32_t uiBufferOffset;
      } memory[BVCE_P_HeapId_eMax];

      struct {
         size_t uiDescriptorBufferSize;
         void *pDescriptorBufferUnCached;
         void *pDescriptorBufferCached;
         unsigned uiDescriptorBufferOffset;
         struct {
               uint32_t uiUserDataQueueInfoAddress;
         } dccm;
         unsigned uiQueuedBuffers;
#if BVCE_P_DUMP_USERDATA_LOG
         BVCE_Debug_Log_Handle hUserDataLog;
#endif
      } userdata;

      uint32_t uiBVNMetaDataRegisterStartAddress;

      BVCE_Channel_StartEncodeSettings stStartEncodeSettings;
      BVCE_Channel_EncodeSettings stEncodeSettings;
      BVCE_Channel_EncodeSettings stPendingEncodeSettings;
      bool bPendingEncodeSettings;
      BVCE_Channel_CallbackSettings stCallbackSettings;
      BVCE_Channel_StopEncodeSettings stStopEncodeSettings;

      BVCE_Channel_Status stStatus;

#if BVCE_P_TEST_MODE
      BVCE_Debug_Log_Handle hConfigLog;
      unsigned uiConfigLogInstance;
      BVCE_Debug_Log_Handle hStatusLog;
      unsigned uiStatusLogInstance;
#endif
} BVCE_P_Channel_Context;

typedef struct BVCE_P_Context
{
      uint32_t uiSignature;

      BVCE_OpenSettings stOpenSettings;

      BTMR_TimerHandle hTimer;

      struct
      {
            BCHP_Handle hChp;
            BREG_Handle hReg;
            BINT_Handle hInt;
      } handles;

      struct
      {
            BMEM_Handle hMem[BVCE_PLATFORM_P_NUM_ARC_CORES];
            void *pCodeBufferUnCached[BVCE_PLATFORM_P_NUM_ARC_CORES];
            void *pCodeBufferCached[BVCE_PLATFORM_P_NUM_ARC_CORES];
            size_t uiCodeSize[BVCE_PLATFORM_P_NUM_ARC_CORES];

            BAFL_FirmwareLoadInfo astFirmwareLoadInfo[BVCE_PLATFORM_P_NUM_ARC_CORES];

            struct
            {
                  uint32_t uiRegisterBaseAddress[BVCE_PLATFORM_P_NUM_ARC_CORES];
                  uint32_t uiCommandBufferBaseOffset;
                  uint32_t uiCommandBufferBaseAddress;
                  uint32_t uiChannelErrorStatusBaseAddress;
                  uint32_t uiWatchdogErrorCodeBaseAddress[BVCE_PLATFORM_P_NUM_ARC_CORES];
            } dccm;

            struct
            {
                  uint32_t uiBufferInfoBaseAddress;
                  void *pBufferUnCached;
                  uint32_t uiOffset;
                  void *pBufferCached;
                  BVCE_Telem_Handle hVceTelem;
            } debug[BVCE_PLATFORM_P_NUM_ARC_CORES];

            BVCE_P_Command stCommand;
            BVCE_P_Response stResponse;
      } fw;


      struct
      {
            BINT_CallbackHandle ahWatchdog[BVCE_PLATFORM_P_NUM_ARC_CORES];
            BINT_CallbackHandle hMailbox;
            BINT_CallbackHandle hEvent;
            BINT_CallbackHandle ahDataReady[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];

            BVCE_CallbackSettings stCallbackSettings;
      } callbacks;

      struct
      {
            BKNI_EventHandle hMailbox;
      } events;

      struct
      {
            BMEM_ModuleHandle hMemModule;
            BMEM_Handle hMem[BVCE_P_HeapId_eMax];
            BVCE_P_SubHeap stHeapInfo[BVCE_P_HeapId_eMax];
      } channel_memory;

      struct
      {
            BVCE_P_Output_Context context;
      } outputs[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];

      struct
      {
            BVCE_P_Channel_Context context;
            bool bResume;
      } channels[BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS];

      struct
      {
            BMEM_Handle hMem;
            size_t uiBufferSize;
            void *pBufferUnCached;
            uint32_t uiBufferOffset;
      } cabac_cmd_memory;

      BVCE_Platform_P_Config stPlatformConfig;

#if BVCE_P_DUMP_ARC_DEBUG_LOG
      BVCE_Debug_Log_Handle hDebugLogDumpFile[BVCE_PLATFORM_P_NUM_ARC_CORES];
#endif

      BVCE_VersionInfo stVersionInfo;
} BVCE_P_Context;

typedef enum BVCE_P_EncoderFrameRate
{
   BVCE_P_EncoderFrameRate_e1498, /* ENCODING_FRAME_RATE_1498 */
   BVCE_P_EncoderFrameRate_e1500, /* ENCODING_FRAME_RATE_1500 */
   BVCE_P_EncoderFrameRate_e2000, /* ENCODING_FRAME_RATE_2000 */
   BVCE_P_EncoderFrameRate_e2397, /* ENCODING_FRAME_RATE_2397 */
   BVCE_P_EncoderFrameRate_e2400, /* ENCODING_FRAME_RATE_2400 */
   BVCE_P_EncoderFrameRate_e2500, /* ENCODING_FRAME_RATE_2500 */
   BVCE_P_EncoderFrameRate_e2997, /* ENCODING_FRAME_RATE_2997 */
   BVCE_P_EncoderFrameRate_e3000, /* ENCODING_FRAME_RATE_3000 */
   BVCE_P_EncoderFrameRate_e5000, /* ENCODING_FRAME_RATE_5000 */
   BVCE_P_EncoderFrameRate_e5994, /* ENCODING_FRAME_RATE_5994 */
   BVCE_P_EncoderFrameRate_e6000, /* ENCODING_FRAME_RATE_6000 */

   BVCE_P_EncoderFrameRate_eMax
} BVCE_P_EncoderFrameRate;

extern const BAVC_FrameRateCode BVCE_P_FrameRateReverseLUT[];



#ifdef __cplusplus
}
#endif

#endif /* BVCE_PRIV_H_ */
