/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_ts_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/65 $
 * $brcm_Date: 9/25/12 10:40a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/src/stream/ts/bmuxlib_ts_priv.h $
 * 
 * Hydra_Software_Devel/65   9/25/12 10:40a delkert
 * SW7425-3958: Move relocation of system data from Pre-Q to after
 * ResetResources() call to prevent overwrite of pre-Q.
 * Add Mutex to Start() and AddSystemDataBuffers() to prevent corruption
 * of pending list during move.
 * Make a private version of AddSystemDataBuffers() without mutex for use
 * during the move (from within Start API)
 * Fix bug that incremented write offset if add to list failed.
 *
 * Hydra_Software_Devel/64   9/11/12 2:07p nilesh
 * SW7425-999: Fixed NULL packet insertion at EOS
 *
 * Hydra_Software_Devel/63   7/18/12 1:02p nilesh
 * SW7425-3076: Updated Memory API
 *
 * Hydra_Software_Devel/62   7/3/12 9:33a nilesh
 * SW7435-200: Added mutex logic for DoMux() and Stop()
 *
 * Hydra_Software_Devel/61   7/2/12 3:17p delkert
 * SW7425-3272: Fix bug in PCR packet where adaptation header flags were
 * all set.
 *
 * Hydra_Software_Devel/60   6/27/12 11:10a nilesh
 * SW7425-3076: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-3076/8   6/26/12 11:54a nilesh
 * SW7425-3076: Updated public API for providing memory buffer info
 *
 * Hydra_Software_Devel/SW7425-3076/7   6/20/12 1:17p nilesh
 * SW7425-3076: Rebase from mainline
 *
 * Hydra_Software_Devel/59   6/19/12 1:39p delkert
 * SW7425-3272: Fix bug in BMUXlib_TS_P_TSPacket_SetPCRPresent macro
 *
 * Hydra_Software_Devel/58   6/12/12 3:04p delkert
 * SW7425-3216: Add test code to dump system data
 *
 * Hydra_Software_Devel/57   6/5/12 12:33p nilesh
 * SW7425-3076: Merge MSP and memory optimizations to mainline
 *
 * Hydra_Software_Devel/SW7425-3076/6   6/20/12 11:59a nilesh
 * SW7425-3076: Use separate heaps for system and device memory
 *
 * Hydra_Software_Devel/SW7425-3076/5   6/14/12 4:10p nilesh
 * SW7425-3076: Added system data pre-Q support
 *
 * Hydra_Software_Devel/SW7425-3076/4   6/14/12 2:06p nilesh
 * SW7425-3076: Added cached bmem support.  Userdata Release Q allocated
 * differently than userdata queue.  Account for worst case end-to-end
 * delay for userdata queue.
 *
 * Hydra_Software_Devel/SW7425-3076/3   6/7/12 4:44p nilesh
 * SW7425-3076: Add sub heap support
 *
 * Hydra_Software_Devel/SW7425-3076/2   6/6/12 5:06p nilesh
 * SW7425-3076: Added size calculations. Converted to using BMEM for
 * userdata.
 *
 * Hydra_Software_Devel/SW7425-3076/1   6/4/12 1:44p nilesh
 * SW7425-3076: Reduced memory footprint.  Implemented variable MSP
 * algorithm to prevent underflow.
 *
 * Hydra_Software_Devel/56   5/1/12 1:01p nilesh
 * SW7425-1221: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1221/2   4/30/12 1:55p nilesh
 * SW7425-1221: Rebase from mainline
 *
 * Hydra_Software_Devel/55   4/24/12 5:19p nilesh
 * SW7425-2606: Added BMUXlib_TS_StartSettings.uiServicePeriod
 *
 * Hydra_Software_Devel/SW7425-1221/1   4/24/12 12:08p nilesh
 * SW7425-2246: Added 42-bit STC support
 *
 * Hydra_Software_Devel/54   3/27/12 8:31a delkert
 * SW7425-2598: Refactor packet parser to better handle packets that do
 * not conform to ETSI EN 300 472
 *
 * Hydra_Software_Devel/53   3/2/12 4:14p delkert
 * SW7425-1188: Add pacing of system data based on supplied timestamp
 * delta.
 *
 * Hydra_Software_Devel/52   2/22/12 12:56p delkert
 * SW7425-2217: Redefine data structures to better separate settings from
 * each userdata PID. Add *temporary* workaround to allow streams with
 * bad PTS to pass thru (for testing purposes only).
 *
 * Hydra_Software_Devel/51   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/3   2/15/12 11:42a delkert
 * SW7425-2217: Fix bug when freeing release Q free list.
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/14/12 11:01a delkert
 * SW7425-2217: Move userdata code to separate files. Cleanup.
 *
 * Hydra_Software_Devel/SW7425-2217/1   2/13/12 9:11a delkert
 * SW7425-2217: Initial working userdata pass-thru
 *
 * Hydra_Software_Devel/50   1/18/12 5:46p nilesh
 * SW7425-2174: Fixed stall detection
 *
 * Hydra_Software_Devel/49   1/9/12 4:11p nilesh
 * SW7425-999: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-999/1   1/9/12 12:24p nilesh
 * SW7425-999: More accurate calculation of next ESCR value
 *
 * Hydra_Software_Devel/48   12/20/11 1:46p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:44a delkert
 * SW7425-1869: Utilize InputGroup for input selection
 *
 * Hydra_Software_Devel/47   11/16/11 10:21a delkert
 * SWCHECKFRAME-114: Add accessor macros for SystemDataCompletedCount
 *
 * Hydra_Software_Devel/46   11/15/11 6:08p delkert
 * SWCHECKFRAME-114: Add private accessor macros to allow test code access
 * to mux state.  Modify mux code to use accessor macros
 *
 * Hydra_Software_Devel/45   11/15/11 4:54p nilesh
 * SW7425-1691: Reset stStartSettings and stFinishSettings on stop()
 *
 * Hydra_Software_Devel/44   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/9/11 2:07p nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/43   10/28/11 9:05a delkert
 * SW7425-1494: Remove redundant code. Improved error checking as a result
 * of API testing.
 *
 * Hydra_Software_Devel/42   10/6/11 5:10p nilesh
 * SW7425-721: Updated to use BMUXlib_Input routines
 *
 * Hydra_Software_Devel/41   7/1/11 2:44p nilesh
 * SW7425-831: Flush transport channels that are active, regardless of
 * whether or not there are packets pending.
 *
 * Hydra_Software_Devel/40   7/1/11 12:11p nilesh
 * SW7425-831: Added BMUXlib_TS_P_Flush
 *
 * Hydra_Software_Devel/39   6/16/11 12:34p nilesh
 * SW7425-734: The 4 bits before PTS field in the PESheader is now set to
 * '0011' if the DTS != PTS
 *
 * Hydra_Software_Devel/38   6/16/11 11:29a nilesh
 * SW7425-735: The six bits between PCR base and PCR extension in PCR
 * packet are now set to '111111'
 *
 * Hydra_Software_Devel/37   6/7/11 11:31p nilesh
 * SW7425-484: In non real time mode, TS muxlib waits for descriptors to
 * be available on all active inputs before initializing PCR and pacing
 * counter.
 *
 * Hydra_Software_Devel/36   5/26/11 4:35p nilesh
 * SW7425-484: Added non-realtime support
 *
 * Hydra_Software_Devel/35   5/11/11 11:55a nilesh
 * SW7425-447: Moved list management functions to a separare shared
 * utility
 *
 * Hydra_Software_Devel/34   4/25/11 2:58p nilesh
 * SW7425-433: If a transport channel's pending list is full (e.g. because
 * of stall), the corresponding A/V/S inputs are ignored.
 *
 * Hydra_Software_Devel/33   4/25/11 12:46a nilesh
 * SW7425-385: To prevent issues related to ESCR discontinuity, PCRs are
 * now generated (temporarily) based on current (PACING_COUNTER + MSP)
 * and not lowest A/V ESCR
 *
 * Hydra_Software_Devel/32   4/15/11 1:42p nilesh
 * SW7425-139: Added check for Mux Service Perioid violation
 *
 * Hydra_Software_Devel/31   4/13/11 5:07p nilesh
 * SW7425-75: Added metadata support
 *
 * Hydra_Software_Devel/30   4/12/11 4:21p nilesh
 * SW7425-303,SW7425-329: Added support for cached
 * BAVC_CompressedBufferStatus.pFrameBufferBaseAddress
 *
 * Hydra_Software_Devel/29   3/1/11 6:24p hongtaoz
 * SW7425-38: added compile option for muxlib debug data file capture;
 *
 * Hydra_Software_Devel/28   3/1/11 11:57a nilesh
 * SW7425-38: Added PES dbg dump support using #define
 * BMUXLIB_TS_P_DUMP_TRANSPORT_PES
 *
 * Hydra_Software_Devel/27   3/1/11 11:36a nilesh
 * SW7425-38: Added file descriptor dbg dump support using #define
 * BMUXLIB_TS_P_DUMP_[AUDIO/VIDEO/TRANSPORT]_DESC
 *
 * Hydra_Software_Devel/26   2/28/11 11:53p nilesh
 * SW7425-38: Fix potential descriptor scheduling issue when frames are
 * split into multiple descriptors and only first descriptor contains a
 * valid ESCR
 *
 * Hydra_Software_Devel/25   2/28/11 7:56p nilesh
 * SW7425-38: Set length in PES Header for audio
 *
 * Hydra_Software_Devel/24   2/28/11 7:00p nilesh
 * SW7425-111: Added late ESCR detection
 *
 * Hydra_Software_Devel/23   2/25/11 2:16p nilesh
 * SW7425-126: GetBufferStatus() is now only called once per encoder
 *
 * Hydra_Software_Devel/22   2/4/11 10:37a nilesh
 * SW7425-38: Commented out unnecessary callback APIs
 *
 * Hydra_Software_Devel/21   2/3/11 4:10p nilesh
 * SW7425-38: API Changes
 *
 * Hydra_Software_Devel/20   2/3/11 12:54p nilesh
 * SW7425-38: Fixed setting stream id in PES packet
 *
 * Hydra_Software_Devel/19   2/2/11 2:16p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddSystemDataBuffers() prototype
 *
 * Hydra_Software_Devel/18   2/2/11 1:28p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddTransportDescriptors() prototype
 *
 * Hydra_Software_Devel/17   2/2/11 11:38a nilesh
 * SW7425-38: Add streamID support
 *
 * Hydra_Software_Devel/16   1/31/11 5:56p nilesh
 * SW7425-38: Changed DoMux API
 *
 * Hydra_Software_Devel/15   1/25/11 10:40a nilesh
 * SW7425-38: Experimental variable PCR support now uses deltaDTS as
 * deltaPCR.
 *
 * Hydra_Software_Devel/14   1/19/11 2:11p nilesh
 * SW7425-38: Added EOS and LAST BPP support
 *
 * Hydra_Software_Devel/13   12/23/10 12:36p nilesh
 * SW7425-38: Fixed PCR Generation
 *
 * Hydra_Software_Devel/12   12/14/10 8:33p nilesh
 * SW7425-38: Added system data support
 *
 * Hydra_Software_Devel/11   12/14/10 1:46p nilesh
 * SW7425-38: Add queuing of system data into muxlib
 *
 * Hydra_Software_Devel/10   11/16/10 1:50p nilesh
 * SW7425-38: Added audio support
 *
 * Hydra_Software_Devel/9   11/10/10 2:46p nilesh
 * SW7425-38: Fixed PTS/DTS assignment in PES Header
 *
 * Hydra_Software_Devel/8   10/4/10 2:19p nilesh
 * SW7425-38: Added PCR pacing
 *
 * Hydra_Software_Devel/7   9/29/10 5:53p nilesh
 * SW7425-38: Added basic PCR support
 *
 * Hydra_Software_Devel/6   9/29/10 3:27p nilesh
 * SW7425-38: Slight restructure to support multiple audio/video PIDs
 *
 * Hydra_Software_Devel/5   9/28/10 5:01p nilesh
 * SW7425-38: Fix compiler warnings
 *
 * Hydra_Software_Devel/4   9/28/10 4:46p nilesh
 * SW7425-38: Added basic video muxing
 *
 * Hydra_Software_Devel/3   9/24/10 12:09p nilesh
 * SW7425-38: Updated transport and system data interface.  Added debug
 * dump of video descriptors.
 *
 * Hydra_Software_Devel/2   9/22/10 4:05p nilesh
 * SW7425-38: Stubbed BMUXlib functions
 *
 * Hydra_Software_Devel/1   9/21/10 5:27p nilesh
 * SW7425-38: Added GetDefaultCreateSettings/Create/Destroy functions
 *
 ***************************************************************************/

#ifndef BMUXLIB_TS_PRIV_H_
#define BMUXLIB_TS_PRIV_H_

#include "bmuxlib_list.h"
#include "bmuxlib_input.h"
#include "bkni_multi.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#define BMUXLIB_TS_P_FIXED_MUX_SERVICE_PERIOD 1
#define BMUXLIB_TS_P_MUX_SERVICE_PERIOD_DEFAULT 50

/* DEBUG */
#ifndef BMUXLIB_TS_P_DUMP_AUDIO_DESC
#define BMUXLIB_TS_P_DUMP_AUDIO_DESC 0
#endif
#ifndef BMUXLIB_TS_P_DUMP_VIDEO_DESC
#define BMUXLIB_TS_P_DUMP_VIDEO_DESC 0
#endif
#ifndef BMUXLIB_TS_P_DUMP_TRANSPORT_DESC
#define BMUXLIB_TS_P_DUMP_TRANSPORT_DESC 0
#endif
#ifndef BMUXLIB_TS_P_DUMP_TRANSPORT_PES
#define BMUXLIB_TS_P_DUMP_TRANSPORT_PES 0
#endif

#if ( BMUXLIB_TS_P_DUMP_AUDIO_DESC || BMUXLIB_TS_P_DUMP_VIDEO_DESC ||  BMUXLIB_TS_P_DUMP_TRANSPORT_DESC || BMUXLIB_TS_P_DUMP_TRANSPORT_PES || BMUXLIB_TS_P_TEST_MODE)
#include <stdio.h>
#endif

#define BMUXLIB_TS_P_SCALE_MS_TO_27MHZ    (27000000 / 1000)

/* accessor macros for use in testing to manipulate mux state */
#define BMUXLIB_TS_P_GET_MUX_STATE(handle)         ((handle)->status.eState)
#define BMUXLIB_TS_P_SET_MUX_STATE(handle, state)  ((handle)->status.eState = (state))

/* accessors for sys data completed count */
#define BMUXLIB_TS_P_GET_SYS_DATA_COMP_CNT(handle)          ((handle)->status.uiSystemDataCompletedCount)
#define BMUXLIB_TS_P_SET_SYS_DATA_COMP_CNT(handle, count)   ((handle)->status.uiSystemDataCompletedCount = (count))

/**************/
/* Signatures */
/**************/
#define BMUXLIB_TS_P_SIGNATURE_CREATESETTINGS   0x77858801
#define BMUXLIB_TS_P_SIGNATURE_STARTSETTINGS    0x77858802
#define BMUXLIB_TS_P_SIGNATURE_MUXSETTINGS      0x77858803
#define BMUXLIB_TS_P_SIGNATURE_FINISHSETTINGS   0x77858804
#define BMUXLIB_TS_P_SIGNATURE_CONTEXT          0x77858805

/**************************/
/* PES Header Definitions */
/**************************/

#define BMUXlib_TS_P_PESHeader_MAXSIZE 19
typedef union BMUXlib_TS_P_PESHeader
{
      uint8_t data[BMUXlib_TS_P_PESHeader_MAXSIZE]; /* PES Header is 19 bytes */
#if 0
      struct {
            uint32_t uiStreamID:8;
            uint32_t uiStartCode:24; /* 000001h */

            uint32_t bPESExtensionFlag:1;
            uint32_t bPESCRCFlag:1;
            uint32_t bAdditionalCopyInfoFlag:1;
            uint32_t bDSMTrickModeFlag:1;
            uint32_t bESRateFlag:1;
            uint32_t bESCRFlag:1;
            uint32_t bDtsFlag:1;
            uint32_t bPtsFlag:1;
            uint32_t bOriginalFlag:1;
            uint32_t bCopyrightFlag:1;
            uint32_t bDataAlignmentIndicator:1;
            uint32_t bPESPriority:1;
            uint32_t uiPESScramblingControl:2;
            uint32_t uiExtensionCode:2; /* Always 10b */
            uint32_t uiPESPacketLength:16;

            uint8_t uiPESHeaderDataLength;

            uint32_t
      } fields;
#endif
} BMUXlib_TS_P_PESHeader;

#define BMUXlib_TS_P_PESHeader_SetStreamID(_pesHeader, _streamID) \
         _pesHeader[3] = ( _streamID & 0xFF );

#define BMUXlib_TS_P_PESHeader_SetPTS(_pesHeader, _pts) \
         _pesHeader[13] = ( ( ( _pts << 1  ) & 0xFE ) | (0x01) ); \
         _pesHeader[12] = (   ( _pts >> 7  ) & 0xFF ); \
         _pesHeader[11] = ( ( ( _pts >> 14 ) & 0xFE ) | (0x01) ); \
         _pesHeader[10] = (   ( _pts >> 22 ) & 0xFF ); \
         _pesHeader[9] =  ( ( ( _pts >> 29 ) & 0x0E ) | (0x21) ); \
         _pesHeader[7] |= 0x80;

#define BMUXlib_TS_P_PESHeader_SetDTS(_pesHeader, _pts) \
         _pesHeader[18] = ( ( ( _pts << 1  ) & 0xFE ) | (0x01) ); \
         _pesHeader[17] = (   ( _pts >> 7  ) & 0xFF ); \
         _pesHeader[16] = ( ( ( _pts >> 14 ) & 0xFE ) | (0x01) ); \
         _pesHeader[15] = (   ( _pts >> 22 ) & 0xFF ); \
         _pesHeader[14] = ( ( ( _pts >> 29 ) & 0x0E ) | (0x11) ); \
         _pesHeader[9] |= (0x30); \
         _pesHeader[7] |= 0x40;

#define BMUXlib_TS_P_PESHeader_SetLength(_pesHeader, _length) \
         _pesHeader[4] = ( ( _length >> 8 ) & 0xFF ); \
         _pesHeader[5] = ( ( _length )      & 0xFF );

/*************************/
/* TS Packet Definitions */
/*************************/
#define BMUXlib_TS_P_TSPacket_MAXSIZE 188
typedef union BMUXlib_TS_P_TSPacket
{
      uint8_t data[BMUXlib_TS_P_TSPacket_MAXSIZE]; /* TS Packet is 188 bytes */
} BMUXlib_TS_P_TSPacket;

#define BMUXlib_TS_P_TSPacket_SetPayloadUnitStart(_tsPacket, _bPayloadUnitStart) \
   _tsPacket[1] = ( _bPayloadUnitStart ? 0x40 : 0x00 ) | ( _tsPacket[1] & ~0x40);

#define BMUXlib_TS_P_TSPacket_SetPID(_tsPacket, _pid) \
   _tsPacket[1] = ( ( _pid >> 8 ) & 0x1F ) | ( _tsPacket[1] & ~0x1F ); \
   _tsPacket[2] = ( ( _pid      ) & 0xFF );

#define BMUXlib_TS_P_TSPacket_SetAdaptationPresent(_tsPacket, _bAdaptationPresent) \
   _tsPacket[3] = ( _bAdaptationPresent ? 0x20 : 0x00 ) | (_tsPacket[3] & ~0x20 );

#define BMUXlib_TS_P_TSPacket_SetPayloadPresent(_tsPacket, _bPayloadPresent) \
   _tsPacket[3] = ( _bPayloadPresent ? 0x10 : 0x00 ) | (_tsPacket[3] & ~0x10 );

#define BMUXlib_TS_P_TSPacket_SetContinuityCounter(_tsPacket, _ContinuityCounter) \
   _tsPacket[3] = ( _ContinuityCounter & 0x0F ) | (_tsPacket[3] & ~0x0F );

extern const BMUXlib_TS_P_TSPacket s_stDefaultTSPacket;

/**************************/
/* PCR Packet Definitions */
/**************************/
#define BMUXlib_TS_P_TSPacket_SetAdaptationLength(_tsPacket, _AdaptationLength) \
   _tsPacket[4] = _AdaptationLength;   \
   if (_AdaptationLength != 0) _tsPacket[5] = 0

#define BMUXlib_TS_P_TSPacket_SetPCRPresent(_tsPacket, _bPCRPresent) \
   _tsPacket[5] = ( _bPCRPresent ? 0x10 : 0x00 ) | ( _tsPacket[5] & ~0x10 );

#define BMUXlib_TS_P_TSPacket_SetPCRBase( _tsPacket, _PCRBase ) \
   _tsPacket[ 6] = ( (_PCRBase >> 25 ) & 0xFF ); \
   _tsPacket[ 7] = ( (_PCRBase >> 17 ) & 0xFF ); \
   _tsPacket[ 8] = ( (_PCRBase >>  9 ) & 0xFF ); \
   _tsPacket[ 9] = ( (_PCRBase >>  1 ) & 0xFF ); \
   _tsPacket[10] = ( (_PCRBase <<  7 ) & 0x80 ) | ( _tsPacket[10] & 0x01 ) | 0x7E;

#define BMUXlib_TS_P_TSPacket_SetPCRExtension( _tsPacket, _PCRExtension ) \
   _tsPacket[10] = ( (_PCRExtension >>  8 ) & 0xFF ) | (_tsPacket[10] & 0x80 ) | 0x7E; \
   _tsPacket[11] = ( (_PCRExtension       ) & 0xFF ); \

/**************************/
/* BPP Data Definitions */
/**************************/
#define BMUXlib_TS_P_BPPData_MAXSIZE 184
typedef union BMUXlib_TS_P_BPPData
{
      uint8_t data[BMUXlib_TS_P_BPPData_MAXSIZE]; /* TS Packet is 188 bytes */
} BMUXlib_TS_P_BPPData;

#define BMUXlib_TS_P_BPPData_SetStreamID(_bppData, _streamID) \
         _bppData[3] = ( _streamID & 0xFF );

#define BMUXlib_TS_P_BPPData_SetCommand(_bppData, _command) \
         _bppData[30] = ( (_command >> 24) & 0xFF ); \
         _bppData[31] = ( (_command >> 16) & 0xFF ); \
         _bppData[32] = ( (_command >> 8 ) & 0xFF ); \
         _bppData[33] = ( (_command )      & 0xFF );

extern const BMUXlib_TS_P_BPPData s_stDefaultBPPData;

/******************************/
/* Mux TS Context Definitions */
/******************************/
typedef enum BMUXlib_TS_P_DataType
{
      BMUXlib_TS_P_DataType_eCDB,   /* this used for external inputs (i.e. video, audio, sys data, userdata) */
      BMUXlib_TS_P_DataType_ePESHeader,
      BMUXlib_TS_P_DataType_ePCRPacket,
      BMUXlib_TS_P_DataType_eBPP,
      BMUXlib_TS_P_DataType_eNULL,
      BMUXlib_TS_P_DataType_eUserdataPTS,    /* a PTS entry for userdata packet timing updates */
      BMUXlib_TS_P_DataType_eUserdataLocal,  /* local storage used for "unwrapping" userdata TS packets */

      BMUXlib_TS_P_DataType_eMax
} BMUXlib_TS_P_DataType;

/* if data type is CDB, this defines the source of the data */
typedef enum BMUXlib_TS_P_SourceType
{
      BMUXlib_TS_P_SourceType_eUnknown,
      BMUXlib_TS_P_SourceType_eVideo,
      BMUXlib_TS_P_SourceType_eAudio,
      BMUXlib_TS_P_SourceType_eSystem,
      BMUXlib_TS_P_SourceType_eUserdata,

      BMUXlib_TS_P_SourceType_eMax
} BMUXlib_TS_P_SourceType;

typedef struct BMUXlib_TS_P_TransportDescriptorMetaData
{
      BMUXlib_TS_P_DataType eDataType;
      uint32_t uiSourceDescriptorCount; /* Set to one if the corresponding source descriptor should be freed when this transport descriptor is done */

      BMUXlib_TS_P_SourceType eSourceType;
      /* The following are only valid for audio/video/userdata source types */
      unsigned uiInputIndex;
} BMUXlib_TS_P_TransportDescriptorMetaData;

typedef struct BMUXlib_TS_P_PCRInfo
{
      bool bInitialized;

      uint64_t uiBase;
      uint16_t uiExtension;
      uint64_t uiESCR;
      uint64_t uiNextESCR;
} BMUXlib_TS_P_PCRInfo;

typedef struct BMUXlib_TS_P_SystemDataInfo
{
      bool bLastScheduledESCRValid;
      uint32_t uiESCR;
      uint32_t uiPacketsMuxedSoFar;
      uint32_t uiPacket2PacketTimestampDelta;
      uint32_t uiPacketsUntilNextPCR;
      uint32_t uiLastScheduledESCR;
#ifdef BMUXLIB_TS_P_TEST_MODE
      FILE *hCSVFile;         /* system data information for each "block" written */
      FILE *hDataFile;        /* binary system data written here */
      bool bCSVOpened;
#endif
} BMUXlib_TS_P_SystemDataInfo;

typedef struct BMUXlib_TS_P_InputMetaData
{
   BMUXlib_Input_Handle hInput;
   unsigned uiInputIndex;
   unsigned uiTransportChannelIndex;   /* Which transport channel interface to use for this PID */
   uint16_t uiPID;                     /* The PID associated with this input data */
   uint8_t uiPESStreamID;
   void *pBPPBuffer;

   /* State */
   bool bEOSSeen; /* true if EOS descriptor has been seen */

   unsigned uiMetadataDescriptorsSkipped; /* Number of source metdata descriptors skipped */
#if ( BMUXLIB_TS_P_DUMP_VIDEO_DESC | BMUXLIB_TS_P_DUMP_AUDIO_DESC )
   FILE *hDescDumpFile;
   bool bPrintedHeader;
#endif

   struct BMUXlib_TS_P_UserdataVideoInfo *pstUserdata;   /* information required by userdata related to this input (video only) */

   /* FIXME: Following used by userdata for verification of dPTS */
   BAVC_FrameRateCode eSourceFrameRateCode;

   bool bSTCSnapshotValid;
   uint64_t uiSTCSnapshot;
} BMUXlib_TS_P_InputMetaData;

typedef struct BMUXlib_TS_P_TransportMetaData
{
      bool bActive;
      uint32_t uiTransportDescriptorsQueued;
      uint32_t uiTransportDescriptorsSent;
#if BMUXLIB_TS_P_DUMP_TRANSPORT_DESC
      FILE *hDescDumpFile;
      bool bPrintedHeader;
#endif
#if BMUXLIB_TS_P_DUMP_TRANSPORT_PES
      FILE *hPESDumpFile;
#endif
} BMUXlib_TS_P_TransportMetaData;

typedef enum BMUXlib_TS_P_MemoryType
{
   BMUXlib_TS_P_MemoryType_eSystem, /* Only accessed by the host */
   BMUXlib_TS_P_MemoryType_eShared, /* Accessed by both the host and device(s) */

   BMUXlib_TS_P_MemoryType_eMax
} BMUXlib_TS_P_MemoryType;

typedef struct BMUXlib_TS_P_BufferInfo
{
   size_t uiSize;
   void *pBuffer;
} BMUXlib_TS_P_BufferInfo;

typedef struct BMUXlib_TS_P_BufferConfig
{
   BMUXlib_TS_P_BufferInfo stBufferInfo[BMUXlib_TS_P_MemoryType_eMax];
} BMUXlib_TS_P_BufferConfig;

typedef enum BMUXlib_TS_P_InputType
{
   BMUXlib_TS_P_InputType_eAudio,
   BMUXlib_TS_P_InputType_eVideo,
   BMUXlib_TS_P_InputType_eSystem,

   BMUXlib_TS_P_InputType_eMax
} BMUXlib_TS_P_InputType;

typedef enum BMUXlib_TS_P_MemoryEntryType
{
   BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor, /* A/V/S */
   BMUXlib_TS_P_MemoryEntryType_ePESHeader, /* A/V */
   BMUXlib_TS_P_MemoryEntryType_eTransportPacket, /* S-only (for PCR) */
   BMUXlib_TS_P_MemoryEntryType_eSystemData, /* S-only (for system data) */
   BMUXlib_TS_P_MemoryEntryType_eBPP, /* A/V */
   BMUXlib_TS_P_MemoryEntryType_eUserData, /* S-only (for user data) */
   BMUXlib_TS_P_MemoryEntryType_eUserDataUnwrap, /* S-only (for user data) */
   BMUXlib_TS_P_MemoryEntryType_eUserDataReleaseQ, /* S-only (for user data) */

   BMUXlib_TS_P_MemoryEntryType_eMax
} BMUXlib_TS_P_MemoryEntryType;

typedef struct BMUXlib_TS_P_MemoryEntry
{
   size_t uiCount; /* Number of entries */
   BMUXlib_TS_P_BufferConfig stMemoryConfig;  /* Total Size for all entries */
} BMUXlib_TS_P_MemoryEntry;

typedef struct BMUXlib_TS_P_MemoryConfigTotal
{
   BMUXlib_TS_P_MemoryEntry astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eMax];

   BMUXlib_TS_P_BufferConfig stMemoryConfig; /* Contains the number of bytes required to allocate the items listed above */
} BMUXlib_TS_P_MemoryConfigTotal;

typedef struct BMUXlib_TS_P_MemoryConfig
{
   BMUXlib_TS_P_MemoryEntry astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eMax][BMUXlib_TS_P_InputType_eMax];

   BMUXlib_TS_P_BufferConfig stMemoryConfig;
} BMUXlib_TS_P_MemoryConfig;

#include "bmuxlib_ts_userdata.h"

typedef struct BMUXlib_TS_P_Context
{
      uint32_t uiSignature;     /* [DO NOT MODIFY] Populated by BMUXlib_TS_Create() */

      /* Create */
      BMUXlib_TS_CreateSettings stCreateSettings;
      BKNI_MutexHandle hMutex;

      BMUXlib_TS_TransportDescriptor *astTransportDescriptorUnCached;
      BMUXlib_TS_TransportDescriptor *astTransportDescriptor;
      BMUXlib_TS_TransportDescriptor *astTransportDescriptorTempUnCached;
      BMUXlib_TS_TransportDescriptor *astTransportDescriptorTemp;
      BMUXlib_List_Handle hTransportDescriptorFreeList; /* Stack containing free transport descriptors */
      BMUXlib_List_Handle hTransportDescriptorPendingList[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES]; /* FIFO containing the pending transport descriptors
                                                                                                     * for each transport interface.  Entries move between
                                                                                                     * the pending and free lists */

      BMUXlib_TS_P_TransportDescriptorMetaData *astTransportDescriptorMetaDataUnCached;
      BMUXlib_TS_P_TransportDescriptorMetaData *astTransportDescriptorMetaData;
      BMUXlib_List_Handle hTransportDescriptorMetaDataFreeList; /* Stack containing free transport metadata descriptors */
      BMUXlib_List_Handle hTransportDescriptorMetaDataPendingList[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES]; /* FIFO containing the pending transport metadata descriptors
                                                                                                             * for each transport interface.  Entries move between
                                                                                                             * the pending and free lists */

      BMUXlib_TS_P_PESHeader *astPESHeaderUnCached;
      BMUXlib_TS_P_PESHeader *astPESHeader;
      BMUXlib_List_Handle hPESHeaderFreeList; /* Stack containing free PES Header buffers */

      BMUXlib_TS_P_TSPacket *astTSPacketUnCached;
      BMUXlib_TS_P_TSPacket *astTSPacket;
      BMUXlib_List_Handle hTSPacketFreeList; /* Stack containing TS Packet buffers */

      BMUXlib_TS_P_BPPData *astBPPDataUnCached;
      BMUXlib_TS_P_BPPData *astBPPData; /* Used for BPP "LAST" command.  Can be reused within a playback. */


      BMUXlib_List_Handle hSystemDataPendingList;
      BMUXlib_TS_SystemData *astSystemDataPendingListUnCached;
      BMUXlib_TS_SystemData *astSystemDataPendingList;

      BMUXlib_List_Handle hSystemDataPendingListPreQ;
      BMUXlib_TS_SystemData *astSystemDataPendingListPreQ;

      BMUXlib_List_Handle hUserdataPendingList[BMUXLIB_TS_MAX_USERDATA_PIDS]; /* data pending for sending to transport for each userdata input */
      BMUXlib_List_Handle hUserdataFreeList;
      BMUXlib_TS_P_UserdataPending *astUserdataPendingUnCached;
      BMUXlib_TS_P_UserdataPending *astUserdataPending;

      /* the memory used to store updated PTS values for userdata PES packets */
      BMUXlib_List_Handle hUserdataPTSFreeList;
      BMUXlib_TS_P_UserdataPTSEntry *astUserdataPTSUnCached;
      BMUXlib_TS_P_UserdataPTSEntry *astUserdataPTS;

      /* memory used for "unwrapping" TS packets for sending to transport */
      BMUXlib_TS_P_TSPacket *astUserdataUnwrapUnCached;
      BMUXlib_TS_P_TSPacket *astUserdataUnwrap;

      /* NOTE: The following cannot use a "List" object since these do not support
         removal from arbitrary nodes */
      BMUXlib_TS_P_UserdataReleaseQEntry *pUserdataReleaseQFreeList;
      BMUXlib_TS_P_UserdataReleaseQEntry *pUserdataReleaseQFreeListBaseUnCached;
      BMUXlib_TS_P_UserdataReleaseQEntry *pUserdataReleaseQFreeListBase;

      struct
      {
         BMEM_ModuleHandle hMemModule;
         size_t uiSize;
         void *pBuffer;
         uint32_t uiBufferOffset;
         void *pBufferCached;

         BMEM_Handle hMem;
      } stSubHeap[BMUXlib_TS_P_MemoryType_eMax];

      /* Status */
      struct
      {
         /* Settings */
         BMUXlib_TS_MuxSettings stMuxSettings;

         /* Start Settings */
         BMUXlib_TS_StartSettings stStartSettings;

         /* Finish Settings */
         BMUXlib_TS_FinishSettings stFinishSettings;

         BMUXlib_State eState;
         bool bTransportConfigured; /* Set to true when initialization is complete (E.g. Initial PCR Value seeded, transport is configured, etc.) */

         bool bTransportChannelStalled[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES];
         bool bTransportChannelStalledPrintMessage[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES];
         uint32_t uiTransportNextExpectedESCR[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES]; /* Contains the expected ESCR for the next
                                                                                    * descriptor that will be added to the queue.
                                                                                    * Used to control insertion of pcr and system data packets */

         uint32_t uiTransportCurrentPacket2PacketTimestampDelta[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES]; /* Contains the current pkt2pkt ts delta for this channel.
                                                                                                      * Used to calculate nextExpectedESCR.
                                                                                                      */

         BMUXlib_TS_P_PCRInfo stPCRInfo;
         BMUXlib_TS_P_SystemDataInfo stSystemDataInfo;
         bool bFirstESCRValid;
         uint32_t uiFirstESCR;

         bool bTransportStatusValid;
         BMUXlib_TS_TransportStatus stTransportStatus;
         BMUXlib_TS_TransportStatus stPreviousTransportStatus;

         BMUXlib_TS_TransportSettings stTransportSettings;

         /* Metadata */
         BMUXlib_TS_P_InputMetaData stInputMetaData[BMUXLIB_TS_MAX_VIDEO_PIDS + BMUXLIB_TS_MAX_AUDIO_PIDS];
         unsigned uiNumInputs;
         BMUXlib_TS_P_TransportMetaData stTransportMetaData[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES];
         bool bAllInputsReady;

         BMUXlib_InputGroup_Handle hInputGroup;

         /* System Data */
         unsigned uiSystemDataPendingListWriteOffset;
         uint32_t uiSystemDataCompletedCount;

         /* userdata */
         BMUXlib_TS_P_UserdataStatus stUserdataStatus;
         BMUXlib_TS_P_UserdataVideoInfo stUserdataVideoInfo[BMUXLIB_TS_MAX_VIDEO_PIDS];   /* companion video info */
         BMUXlib_TS_P_UserdataInfo stUserdataInfo[BMUXLIB_TS_MAX_USERDATA_PIDS];

         BMUXlib_DoMux_Status stDoMuxStatus;

         BMUXlib_TS_P_MemoryConfig stMemoryConfig;
         BMUXlib_TS_P_MemoryConfigTotal stMemoryConfigTotal;

         unsigned uiSystemDataMaxCount;
      } status;

} BMUXlib_TS_P_Context;

/*************************/
/* Private Mux Functions */
/*************************/
BERR_Code
BMUXlib_TS_P_ConfigureTransport(
         BMUXlib_TS_Handle hMuxTS
         );

BERR_Code
BMUXlib_TS_P_ProcessCompletedBuffers(
         BMUXlib_TS_Handle hMuxTS
         );

BERR_Code
BMUXlib_TS_P_ProcessSystemData(
         BMUXlib_TS_Handle hMuxTS
         );

BERR_Code
BMUXlib_TS_P_ProcessNewBuffers(
         BMUXlib_TS_Handle hMuxTS
         );

BERR_Code
BMUXlib_TS_P_ScheduleProcessedBuffers(
         BMUXlib_TS_Handle hMuxTS
         );

bool
BMUXlib_TS_P_Flush(
         BMUXlib_TS_Handle hMuxTS,
         bool bFlushSystemData
         );

BERR_Code
BMUXlib_TS_P_AddTransportDescriptor(
         BMUXlib_TS_Handle hMuxTS,
         unsigned uiTransportChannelIndex,
         BMUXlib_TS_TransportDescriptor *pstTransportDescriptor
         );

BERR_Code
BMUXlib_TS_P_AddSystemDataBuffers(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_SystemData *astSystemDataBuffer, /* Array of system data buffers */
         size_t uiCount, /* Count of system data buffers in array */
         size_t *puiQueuedCount /* Count of system data buffers queued by muxer (*puiQueuedCount <= uiCount) */
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_TS_PRIV_H_ */
