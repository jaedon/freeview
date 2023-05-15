/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_ts_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/118 $
 * $brcm_Date: 9/11/12 2:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/src/stream/ts/bmuxlib_ts_priv.c $
 * 
 * Hydra_Software_Devel/118   9/11/12 2:07p nilesh
 * SW7425-999: Fixed NULL packet insertion at EOS
 * 
 * Hydra_Software_Devel/117   7/3/12 12:32p nilesh
 * SW7425-3061: Added proper handling of EMPTY_FRAME descriptors
 * 
 * Hydra_Software_Devel/116   6/27/12 11:10a nilesh
 * SW7425-3076: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-3076/7   6/21/12 10:40p nilesh
 * SW7425-3076: Flush cache of all buffers sent to XPT
 * 
 * Hydra_Software_Devel/SW7425-3076/6   6/20/12 1:17p nilesh
 * SW7425-3076: Rebase from mainline
 * 
 * Hydra_Software_Devel/115   6/12/12 3:56p nilesh
 * SW7425-3076: Disable system data stall message
 * 
 * Hydra_Software_Devel/114   6/5/12 12:33p nilesh
 * SW7425-3076: Merge MSP and memory optimizations to mainline
 * 
 * Hydra_Software_Devel/SW7425-3076/5   6/19/12 1:35p nilesh
 * SW7425-3076: Change from using stack to fifo for free buffer queues
 * 
 * Hydra_Software_Devel/SW7425-3076/4   6/6/12 5:06p nilesh
 * SW7425-3076: Added size calculations. Converted to using BMEM for
 * userdata.
 * 
 * Hydra_Software_Devel/SW7425-3076/3   6/5/12 10:15a nilesh
 * SW7425-3076: Cleaned up warning debug prints
 * 
 * Hydra_Software_Devel/SW7425-3076/2   6/4/12 5:12p nilesh
 * SW7425-3076: Clean up
 * 
 * Hydra_Software_Devel/SW7425-3076/1   6/4/12 1:44p nilesh
 * SW7425-3076: Reduced memory footprint.  Implemented variable MSP
 * algorithm to prevent underflow.
 * 
 * Hydra_Software_Devel/113   5/29/12 1:50p delkert
 * SW7425-2567: Add return code checks to fix coverity errors
 *
 * Hydra_Software_Devel/112   5/22/12 10:41a delkert
 * SW7425-2567: Add return code checking to fix coverity warnings
 *
 * Hydra_Software_Devel/111   5/1/12 1:01p nilesh
 * SW7425-1221: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1221/3   4/30/12 1:54p nilesh
 * SW7425-1221: Rebase from mainline
 *
 * Hydra_Software_Devel/110   4/24/12 5:19p nilesh
 * SW7425-2606: Added BMUXlib_TS_StartSettings.uiServicePeriod
 *
 * Hydra_Software_Devel/SW7425-1221/2   4/24/12 12:29p nilesh
 * SW7425-2246: Added BMUXLIB_TS_PCR debug message module
 *
 * Hydra_Software_Devel/SW7425-1221/1   4/24/12 12:08p nilesh
 * SW7425-2246: Added 42-bit STC support
 *
 * Hydra_Software_Devel/109   4/11/12 12:32p nilesh
 * SW7425-2857: Fixed BPP PES Length
 *
 * Hydra_Software_Devel/108   3/27/12 8:31a delkert
 * SW7425-2598: Refactor packet parser to better handle packets that do
 * not conform to ETSI EN 300 472
 *
 * Hydra_Software_Devel/106   3/6/12 9:49a delkert
 * SW7425-1188: Fix up timing error due to rounding to packet boundaries
 * and accumulated error
 *
 * Hydra_Software_Devel/105   3/5/12 10:03a delkert
 * SW7425-1188: Fix error in ceiling calc for PacketsToInsertionPoint
 *
 * Hydra_Software_Devel/104   3/2/12 4:32p delkert
 * SW7425-1188: Don't align the ESCR to packet boundary - use actual
 * insertion time.
 *
 * Hydra_Software_Devel/103   3/2/12 4:13p delkert
 * SW7425-1188: Add pacing of system data based on supplied timestamp
 * delta.
 *
 * Hydra_Software_Devel/102   2/22/12 12:56p delkert
 * SW7425-2217: Redefine data structures to better separate settings from
 * each userdata PID. Add *temporary* workaround to allow streams with
 * bad PTS to pass thru (for testing purposes only).
 *
 * Hydra_Software_Devel/101   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/4   2/16/12 4:22p nilesh
 * SW7425-2217: Rebase from mainline
 *
 * Hydra_Software_Devel/100   2/16/12 1:02p nilesh
 * SW7425-2315: NULL descriptors no longer send a null buffer address to
 * transport
 *
 * Hydra_Software_Devel/SW7425-2217/3   2/15/12 10:37a nilesh
 * SW7425-2217: Rebase from mainline
 *
 * Hydra_Software_Devel/99   2/14/12 9:14a nilesh
 * SW7425-2221: EOS descriptor is now returned to encoder(s)
 *
 * Hydra_Software_Devel/98   2/7/12 11:19a delkert
 * SW7425-2315: Fix coverity error (check return)
 *
 * Hydra_Software_Devel/97   2/1/12 4:22p nilesh
 * SW7425-2315: Added support for
 * BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/14/12 11:01a delkert
 * SW7425-2217: Move userdata code to separate files. Cleanup.
 *
 * Hydra_Software_Devel/SW7425-2217/1   2/13/12 9:11a delkert
 * SW7425-2217: Initial working userdata pass-thru
 *
 * Hydra_Software_Devel/96   1/18/12 5:46p nilesh
 * SW7425-2174: Fixed stall detection
 *
 * Hydra_Software_Devel/95   1/9/12 4:11p nilesh
 * SW7425-999: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-999/4   1/9/12 3:52p nilesh
 * SW7425-999: In NRT mode, to prevent premature ESCR stall on shutdown,
 * always send dummy/NULL descriptors to transport channels whose inputs
 * have all seen an EOS.
 *
 * Hydra_Software_Devel/SW7425-999/3   1/9/12 12:25p nilesh
 * SW7425-999: Removed unused code
 *
 * Hydra_Software_Devel/SW7425-999/2   1/9/12 12:24p nilesh
 * SW7425-999: More accurate calculation of next ESCR value
 *
 * Hydra_Software_Devel/SW7425-999/1   1/9/12 11:44a nilesh
 * SW7425-999: Use BMUXlib_InputGroup status to determine EOS
 *
 * Hydra_Software_Devel/94   12/20/11 1:46p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:44a delkert
 * SW7425-1869: Utilize InputGroup for input selection
 *
 * Hydra_Software_Devel/93   12/8/11 4:21p delkert
 * SW7425-1869: Remove scaling of ESCR values from
 * InputDescriptorCompareESCR()
 *
 * Hydra_Software_Devel/92   11/16/11 10:21a delkert
 * SWCHECKFRAME-114: Add accessor macros for SystemDataCompletedCount
 *
 * Hydra_Software_Devel/91   11/15/11 4:54p nilesh
 * SW7425-1691: Reset stStartSettings and stFinishSettings on stop()
 *
 * Hydra_Software_Devel/90   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/9/11 2:07p nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/89   10/7/11 12:36p nilesh
 * SW7425-721: Removed unused macro
 *
 * Hydra_Software_Devel/88   10/6/11 5:22p nilesh
 * SW7425-721: Fixed wrap scenario in ESCR discontinuity detection
 *
 * Hydra_Software_Devel/87   10/6/11 5:10p nilesh
 * SW7425-721: Updated to use BMUXlib_Input routines
 *
 * Hydra_Software_Devel/86   10/6/11 10:41a delkert
 * SW7425-1415: Add coverity annotations (previous workarounds do not work
 * for debug-only mode)
 *
 * Hydra_Software_Devel/85   10/5/11 5:55p delkert
 * SW7425-1415: Fix coverity issues
 *
 * Hydra_Software_Devel/84   7/6/11 3:27p nilesh
 * SW7425-831: PCRs are now generated based on largest A/V ESCR seen so
 * far (instead of smallest)
 *
 * Hydra_Software_Devel/83   7/1/11 2:44p nilesh
 * SW7425-831: Flush transport channels that are active, regardless of
 * whether or not there are packets pending.
 *
 * Hydra_Software_Devel/82   7/1/11 2:17p nilesh
 * SW7425-831: Only flush channels that have pending packets
 *
 * Hydra_Software_Devel/81   7/1/11 1:56p nilesh
 * SW7425-831: Fixed NRT stall deadlock during shutdown
 *
 * Hydra_Software_Devel/80   7/1/11 12:46p nilesh
 * SW7425-831: Added flush logic when in NRT mode
 *
 * Hydra_Software_Devel/79   7/1/11 12:11p nilesh
 * SW7425-831: Added BMUXlib_TS_P_Flush
 *
 * Hydra_Software_Devel/78   7/1/11 4:33a nilesh
 * SW7425-831: Fixed calculation of last pending ESCR during wrap
 *
 * Hydra_Software_Devel/77   7/1/11 3:59a nilesh
 * SW7425-831: Fix compiler warning
 *
 * Hydra_Software_Devel/76   7/1/11 3:51a nilesh
 * SW7425-831: When in non-realtime mode, generate PCRs based on pending
 * ESCRs instead of current ESCR.  Handle ESCR wrap when determining
 * smallest ESCR to use for PCR generation.
 *
 * Hydra_Software_Devel/75   6/15/11 1:32p nilesh
 * SW7425-699: Fixed overflow logic on system data channel
 *
 * Hydra_Software_Devel/74   6/15/11 12:32p nilesh
 * SW7425-699: Increased check for free space in pending list to 3
 * descriptors
 *
 * Hydra_Software_Devel/73   6/14/11 5:35p nilesh
 * SW7425-699: Fixed stall detection logic
 *
 * Hydra_Software_Devel/72   6/14/11 5:29p nilesh
 * SW7425-699: Fixed logic that checks for fullness of pending transport
 * descriptors list
 *
 * Hydra_Software_Devel/71   6/14/11 1:56p nilesh
 * SW7425-699: Disable error/warning messages that are not relevant in
 * non-realtime (NRT) mode
 *
 * Hydra_Software_Devel/70   6/8/11 12:00a nilesh
 * SW7425-484: Rename uiPacingCounterValue to uiPacingCounter.  Adjust
 * uiPacingCounter by mux delay.
 *
 * Hydra_Software_Devel/69   6/7/11 11:31p nilesh
 * SW7425-484: In non real time mode, TS muxlib waits for descriptors to
 * be available on all active inputs before initializing PCR and pacing
 * counter.
 *
 * Hydra_Software_Devel/68   5/26/11 4:35p nilesh
 * SW7425-484: Added non-realtime support
 *
 * Hydra_Software_Devel/67   5/11/11 11:55a nilesh
 * SW7425-447: Moved list management functions to a separare shared
 * utility
 *
 * Hydra_Software_Devel/66   4/25/11 2:58p nilesh
 * SW7425-433: If a transport channel's pending list is full (e.g. because
 * of stall), the corresponding A/V/S inputs are ignored.
 *
 * Hydra_Software_Devel/65   4/25/11 12:45a nilesh
 * SW7425-385: To prevent issues related to ESCR discontinuity, PCRs are
 * now generated (temporarily) based on current (PACING_COUNTER + MSP)
 * and not lowest A/V ESCR
 *
 * Hydra_Software_Devel/64   4/19/11 6:17p nilesh
 * SW7425-288: MP4 Mux Audio Support
 *
 * Hydra_Software_Devel/SW7425-288/1   4/19/11 3:41p nilesh
 * SW7425-288,SW7425-75: Added support for printing additional descriptor
 * fields
 *
 * Hydra_Software_Devel/63   4/15/11 11:38p nilesh
 * SW7425-88: Fixed compilation warning
 *
 * Hydra_Software_Devel/62   4/15/11 11:28p nilesh
 * SW7425-88: Fixed Audio EOS Handling
 *
 * Hydra_Software_Devel/61   4/15/11 4:21p nilesh
 * SW7425-75: Fix initial PCR seed by ensuring first ESCR is valid
 *
 * Hydra_Software_Devel/60   4/15/11 2:58p nilesh
 * SW7425-139: Added additional debug for late ESCRs
 *
 * Hydra_Software_Devel/59   4/13/11 5:07p nilesh
 * SW7425-75: Added metadata support
 *
 * Hydra_Software_Devel/58   4/12/11 4:21p nilesh
 * SW7425-303,SW7425-329: Added support for cached
 * BAVC_CompressedBufferStatus.pFrameBufferBaseAddress
 *
 * Hydra_Software_Devel/57   4/11/11 11:57a nilesh
 * SW7425-75: Temporarily disabled metadata support.
 *
 * Hydra_Software_Devel/56   4/8/11 1:22p nilesh
 * SW7425-75: Fixed BMUXLIB_DUMP_VIDEO_DESC debug print
 *
 * Hydra_Software_Devel/55   4/8/11 12:47p nilesh
 * SW7425-75: Removed constant metadata to Buffer Info
 *
 * Hydra_Software_Devel/54   4/7/11 4:26p nilesh
 * SW7425-306: Use 0xFF as PES stuffing bytes instead of 0x00
 *
 * Hydra_Software_Devel/53   4/7/11 4:02p nilesh
 * SW7425-75: Add support for video metadata (common)
 *
 * Hydra_Software_Devel/52   4/4/11 4:58p nilesh
 * SW7425-283: Fixed off by one in FIFO list creation causing "FIFO is
 * Full" error
 *
 * Hydra_Software_Devel/51   3/14/11 2:21p nilesh
 * SW7425-168: Fixed kernel oops caused by breaking strict-aliasing rules
 *
 * Hydra_Software_Devel/50   3/9/11 9:05a nilesh
 * SW7425-38: Fixed dbg print typo
 *
 * Hydra_Software_Devel/49   3/8/11 5:00p nilesh
 * SW7425-38: Added BERR_TRACE around private list routines
 *
 * Hydra_Software_Devel/48   3/1/11 1:44p nilesh
 * SW7425-38: Added units to file dump dbg
 *
 * Hydra_Software_Devel/47   3/1/11 12:07p nilesh
 * SW7425-38: Fixed compilation error
 *
 * Hydra_Software_Devel/46   3/1/11 11:57a nilesh
 * SW7425-38: Added PES dbg dump support using #define
 * BMUXLIB_TS_P_DUMP_TRANSPORT_PES
 *
 * Hydra_Software_Devel/45   3/1/11 11:36a nilesh
 * SW7425-38: Added file descriptor dbg dump support using #define
 * BMUXLIB_TS_P_DUMP_[AUDIO/VIDEO/TRANSPORT]_DESC
 *
 * Hydra_Software_Devel/44   3/1/11 12:32a nilesh
 * SW7425-38: Added BMUXLIB_TS_OUTPUT_DESC_ERROR dbg module
 *
 * Hydra_Software_Devel/43   3/1/11 12:02a nilesh
 * SW7425-38: Fixed Audio EOS detection
 *
 * Hydra_Software_Devel/42   2/28/11 11:53p nilesh
 * SW7425-38: Fix potential descriptor scheduling issue when frames are
 * split into multiple descriptors and only first descriptor contains a
 * valid ESCR
 *
 * Hydra_Software_Devel/41   2/28/11 11:24p nilesh
 * SW7425-38: Fix bug in PES Header length calculation when audio frame is
 * contained in multiple descriptors
 *
 * Hydra_Software_Devel/40   2/28/11 7:56p nilesh
 * SW7425-38: Set length in PES Header for audio
 *
 * Hydra_Software_Devel/39   2/28/11 7:00p nilesh
 * SW7425-111: Added late ESCR detection
 *
 * Hydra_Software_Devel/38   2/28/11 4:34p nilesh
 * SW7425-88: Added BMUXlib_TS_EOS dbg module.  Added temporary fix to
 * ignore incorrect EOS audio descriptors.
 *
 * Hydra_Software_Devel/37   2/25/11 2:15p nilesh
 * SW7425-126: GetBufferStatus() is now only called once per encoder
 *
 * Hydra_Software_Devel/36   2/24/11 10:35a nilesh
 * SW7425-38: Cleaned up dbg messages
 *
 * Hydra_Software_Devel/35   2/22/11 3:57p nilesh
 * SW7425-110: Changed BDBG_ASSERT to BDBG_ERR where necessary
 *
 * Hydra_Software_Devel/34   2/8/11 6:39p hongtaoz
 * SW7425-38: fixed audio PES stream ID assignment;
 *
 * Hydra_Software_Devel/33   2/3/11 4:10p nilesh
 * SW7425-38: API Changes
 *
 * Hydra_Software_Devel/32   2/3/11 12:01p nilesh
 * SW7425-38: Fixed debug print
 *
 * Hydra_Software_Devel/31   2/2/11 1:28p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddTransportDescriptors() prototype
 *
 * Hydra_Software_Devel/30   2/2/11 11:38a nilesh
 * SW7425-38: Add streamID support
 *
 * Hydra_Software_Devel/29   2/1/11 4:59p nilesh
 * SW7425-38: stream id to StartSettings
 *
 * Hydra_Software_Devel/28   2/1/11 4:16p nilesh
 * SW7425-38: Combined common portions of audio/video compressed buffer
 * descriptors
 *
 * Hydra_Software_Devel/27   1/31/11 10:50a nilesh
 * SW7425-38: Removed use of LLU constants
 *
 * Hydra_Software_Devel/26   1/25/11 10:40a nilesh
 * SW7425-38: Experimental variable PCR support now uses deltaDTS as
 * deltaPCR.
 *
 * Hydra_Software_Devel/25   1/19/11 2:11p nilesh
 * SW7425-38: Added EOS and LAST BPP support
 *
 * Hydra_Software_Devel/24   12/30/10 2:09p nilesh
 * SW7425-38: Fix PCR wrap
 *
 * Hydra_Software_Devel/23   12/27/10 6:18p nilesh
 * SW7425-38: Fixed initial PCR calculation
 *
 * Hydra_Software_Devel/22   12/23/10 4:39p nilesh
 * SW7425-38: Added emulation specific code
 *
 * Hydra_Software_Devel/21   12/23/10 12:36p nilesh
 * SW7425-38: Fixed PCR Generation
 *
 * Hydra_Software_Devel/20   12/21/10 5:42p nilesh
 * SW7425-38: Fixed PCR Offset Value
 *
 * Hydra_Software_Devel/19   12/21/10 4:53p nilesh
 * SW7425-38: Added ESCR wrap handling.  Initial PCR is now set based on
 * initial A/V frame's ESCR.
 *
 * Hydra_Software_Devel/18   12/14/10 8:33p nilesh
 * SW7425-38: Added system data support
 *
 * Hydra_Software_Devel/17   12/14/10 1:46p nilesh
 * SW7425-38: Add queuing of system data into muxlib
 *
 * Hydra_Software_Devel/16   12/13/10 4:49p hongtaoz
 * SW7425-38: use bit count tp compute PCR packet2packet timestamp delta;
 *
 * Hydra_Software_Devel/15   12/9/10 2:15p nilesh
 * SW7425-38: Fixed uiTransportLastESCR calculation for system data
 * transport channel
 *
 * Hydra_Software_Devel/14   12/8/10 4:44p nilesh
 * SW7425-38: Updated to use common BAVC_TsMux_DescConfig
 *
 * Hydra_Software_Devel/13   11/16/10 3:24p nilesh
 * SW7425-38: Added PCR support
 *
 * Hydra_Software_Devel/12   11/16/10 1:50p nilesh
 * SW7425-38: Added audio support
 *
 * Hydra_Software_Devel/11   11/10/10 2:46p nilesh
 * SW7425-38: Fixed PTS/DTS assignment in PES Header
 *
 * Hydra_Software_Devel/10   11/5/10 12:32p nilesh
 * SW7425-38: Implemented ESCR priority based queing of buffer
 * descriptors.
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
 * Hydra_Software_Devel/6   9/29/10 5:53p nilesh
 * SW7425-38: Added basic PCR support
 *
 * Hydra_Software_Devel/5   9/29/10 3:27p nilesh
 * SW7425-38: Slight restructure to support multiple audio/video PIDs
 *
 * Hydra_Software_Devel/4   9/28/10 5:01p nilesh
 * SW7425-38: Fix compiler warnings
 *
 * Hydra_Software_Devel/3   9/28/10 4:46p nilesh
 * SW7425-38: Added basic video muxing
 *
 * Hydra_Software_Devel/2   9/22/10 4:05p nilesh
 * SW7425-38: Stubbed BMUXlib functions
 *
 * Hydra_Software_Devel/1   9/21/10 5:27p nilesh
 * SW7425-38: Added GetDefaultCreateSettings/Create/Destroy functions
 *
 ***************************************************************************/

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#include "blst_slist.h"

#include "bmuxlib_ts.h"
#include "bmuxlib_ts_priv.h"

BDBG_MODULE(BMUXLIB_TS_PRIV);
BDBG_FILE_MODULE(BMUXLIB_TS_OUTPUT_DESC);
BDBG_FILE_MODULE(BMUXLIB_TS_OUTPUT_DESC_ERROR);
BDBG_FILE_MODULE(BMUXLIB_TS_EOS);
BDBG_FILE_MODULE(BMUXLIB_TS_SYSDATA);
BDBG_FILE_MODULE(BMUXLIB_TS_PCR);

const BMUXlib_TS_P_PESHeader s_stDefaultPESHeader =
{
 {
  0x00,0x00,0x01, /* Start Code */
  0xE0, /* Stream ID */
  0x00, 0x00, /* Packet Length */
  0x80, 0x00, /* Extension w/ PTS+DTS invalid */
  0x0A, /* PES Header Data Length */
  0xFF, /* PTS[32:30] */
  0xFF, 0xFF, /* PTS[29:15] */
  0xFF, 0xFF, /* PTS[14:00] */
  0xFF, /* DTS[32:30] */
  0xFF, 0xFF, /* DTS[29:15] */
  0xFF, 0xFF  /* DTS[14:00] */
 },
};

const BMUXlib_TS_P_TSPacket s_stDefaultTSPacket =
{
 {
    0x47, /* Sync Byte */
    0x00, 0x00, /* PUSI/PID */
    0x00, /* Scrabling/Adaptation/CC */
    /* Adaptation/Payload */
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF

 },
};

const BMUXlib_TS_P_BPPData s_stDefaultBPPData =
{
 {
    0x00,0x00,0x01, /* Start Code */
    0xE0, /* Stream ID */
    0x00, 0xB2, /* Packet Length (184 - 6 bytes)*/
    0x81, 0x01, /* Extension w/ PES Extension Flag Valid */
    0x14, /* PES Header Data Length (29 - 9 bytes) */
    0x80, /* PES Extension Flags w/ PES Private Data Valid */
    'B','R','C','M', /* "BRCM" signature for BPP */
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* Stuffing */
    0xFF,0xFF,0xFF, /* More Stuffing */
    0x00, /* Sub-Stream ID */
    0x00,0x00,0x00,0x00, /* BPP command */
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00
 },
};

/*************************/
/* Private Mux Functions */
/*************************/
BERR_Code
BMUXlib_TS_P_ConfigureTransport(
         BMUXlib_TS_Handle hMuxTS
         )
{
   bool bReadyToConfigure = false;

   BDBG_ENTER( BMUXlib_TS_P_ConfigureTransport );

   hMuxTS->status.stStartSettings.transport.stDeviceInterface.fGetTransportSettings(
            hMuxTS->status.stStartSettings.transport.stDeviceInterface.pContext,
            &hMuxTS->status.stTransportSettings);

   hMuxTS->status.stTransportSettings.uiMuxDelay = hMuxTS->status.stStartSettings.uiServiceLatencyTolerance;
   hMuxTS->status.stTransportSettings.uiMuxDelay += hMuxTS->status.stStartSettings.uiServicePeriod;


   /* Handle NRT/AFAP mode */
   if ( true == hMuxTS->status.stStartSettings.bNonRealTimeMode )
   {
      /* Seed the pacing counter based on the smallest initial ESCR seen */
      if ( true == hMuxTS->status.bFirstESCRValid )
      {
         hMuxTS->status.stTransportSettings.bNonRealTimeMode = true;
         hMuxTS->status.stTransportSettings.stNonRealTimeSettings.uiPacingCounter = hMuxTS->status.uiFirstESCR;
         hMuxTS->status.stTransportSettings.stNonRealTimeSettings.uiPacingCounter -= ( hMuxTS->status.stTransportSettings.uiMuxDelay * 27000 );

         bReadyToConfigure = true;
      }
   }
   else
   {
      bReadyToConfigure = true;
   }

   if ( true == bReadyToConfigure )
   {
      hMuxTS->status.stStartSettings.transport.stDeviceInterface.fSetTransportSettings(
               hMuxTS->status.stStartSettings.transport.stDeviceInterface.pContext,
               &hMuxTS->status.stTransportSettings);

      hMuxTS->status.bTransportConfigured = true;
   }

   BDBG_LEAVE( BMUXlib_TS_P_ConfigureTransport );

   return BERR_TRACE( BERR_SUCCESS );
}

/* TODO: define "VideoSettings struct" to increase code modularity */
BERR_Code
BMUXlib_TS_P_ProcessCompletedBuffers(
   BMUXlib_TS_Handle hMuxTS
   )
{
   size_t auiInputDescriptorsConsumed[BMUXLIB_TS_MAX_INPUT_PIDS];

   BDBG_ENTER( BMUXlib_TS_P_ProcessCompletedBuffers );

   BKNI_Memset(
            auiInputDescriptorsConsumed,
            0,
            sizeof( size_t ) * BMUXLIB_TS_MAX_INPUT_PIDS
            );

   /* Get Completed Transport Descriptors */
   {
      size_t uiCompleted;
      BMUXlib_TS_TransportDescriptor *pstCurrentTransportDescriptor = NULL;
      BMUXlib_TS_P_TransportDescriptorMetaData *pstCurrentTransportDescriptorMetaData = NULL;
      uint32_t uiTransportChannelIndex;

      for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
      {
         BMUXlib_TS_TransportChannelInterface *pTransportInterface = &hMuxTS->status.stStartSettings.transport.stChannelInterface[uiTransportChannelIndex];

         if ( ( NULL != pTransportInterface->pContext )
              && ( NULL != pTransportInterface->fGetCompletedTransportDescriptors )
            )
         {
            pTransportInterface->fGetCompletedTransportDescriptors(
                     pTransportInterface->pContext,
                     &uiCompleted
                     );

            while ( 0 != uiCompleted )
            {
               BERR_TRACE( BMUXlib_List_Remove(
                        hMuxTS->hTransportDescriptorMetaDataPendingList[uiTransportChannelIndex],
                        (void**) &pstCurrentTransportDescriptorMetaData
                        ) );

               BERR_TRACE( BMUXlib_List_Remove(
                        hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex],
                        (void**) &pstCurrentTransportDescriptor
                        ) );

               /* Reclaim any internal buffers */
               switch ( pstCurrentTransportDescriptorMetaData->eDataType )
               {
                  case BMUXlib_TS_P_DataType_eNULL:
                     /* Do nothing */
                     break;

                  case BMUXlib_TS_P_DataType_eCDB:
                     /* Do nothing */
                     break;

                  case BMUXlib_TS_P_DataType_ePESHeader:
                     /* Put PES Header back on Free List */
                     BERR_TRACE( BMUXlib_List_Add(
                              hMuxTS->hPESHeaderFreeList,
                              (void*) pstCurrentTransportDescriptor->pBufferAddress
                              ) );
                     break;

                  case BMUXlib_TS_P_DataType_ePCRPacket:
                     /* Put PCR Packet back on Free List (TS) */
                     BERR_TRACE( BMUXlib_List_Add(
                              hMuxTS->hTSPacketFreeList,
                              (void*) pstCurrentTransportDescriptor->pBufferAddress
                              ) );
                     break;

                  case BMUXlib_TS_P_DataType_eBPP:
                     BDBG_MSG(("BPP Consumed by TS Muxer"));
                     if ( 0 != pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount )
                     {
                        auiInputDescriptorsConsumed[pstCurrentTransportDescriptorMetaData->uiInputIndex] += pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount;
                     }
                     break;

                  case BMUXlib_TS_P_DataType_eUserdataPTS:
                  {
                     BERR_Code rc;
                     /* return the userdata PTS entry to the free list */
                     rc = BERR_TRACE(BMUXlib_List_Add(hMuxTS->hUserdataPTSFreeList, (void *)pstCurrentTransportDescriptor->pBufferAddress));
                     /* there should always be room for the PTS entry on the free list */
                     BDBG_ASSERT(BERR_SUCCESS == rc);
                     break;
                  }

                  case BMUXlib_TS_P_DataType_eUserdataLocal:
                     /* used to identify use of local buffer for unwrapping split packet */
                     hMuxTS->status.stUserdataInfo[pstCurrentTransportDescriptorMetaData->uiInputIndex].bUnwrapInUse = false;
                     break;

                  case BMUXlib_TS_P_DataType_eMax:
                     BDBG_ERR(("Unknown data type"));
                     break;
               }

               switch ( pstCurrentTransportDescriptorMetaData->eSourceType )
               {
                  case BMUXlib_TS_P_SourceType_eAudio:
                  case BMUXlib_TS_P_SourceType_eVideo:
                     if ( BMUXlib_TS_P_DataType_eCDB == pstCurrentTransportDescriptorMetaData->eDataType )
                     {
                        auiInputDescriptorsConsumed[pstCurrentTransportDescriptorMetaData->uiInputIndex] += pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount;
                     }
                     break;

                  case BMUXlib_TS_P_SourceType_eSystem:
                     BMUXLIB_TS_P_GET_SYS_DATA_COMP_CNT(hMuxTS) += pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount;
                     break;

                  case BMUXlib_TS_P_SourceType_eUserdata:
                     BMUXlib_TS_P_Userdata_AddToReleaseQ(hMuxTS, pstCurrentTransportDescriptorMetaData->uiInputIndex, pstCurrentTransportDescriptor->uiBufferLength);
                     break;

                  case BMUXlib_TS_P_SourceType_eUnknown:
                  case BMUXlib_TS_P_SourceType_eMax:
                     /* Unknown Source Type */
                     break;
               }

               BERR_TRACE( BMUXlib_List_Add(
                        hMuxTS->hTransportDescriptorMetaDataFreeList,
                        pstCurrentTransportDescriptorMetaData
                        ) );

               BERR_TRACE( BMUXlib_List_Add(
                        hMuxTS->hTransportDescriptorFreeList,
                        pstCurrentTransportDescriptor
                        ) );

               hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].uiTransportDescriptorsQueued--;

               uiCompleted--;
            } /* end: while descriptors remain */
         } /* end: if interface valid */
      } /* end: for each transport channel */
   }

   /* Return completed descriptors back to encoders */
   {
      unsigned uiInputIndex;

      /* Return completed video descriptors */
      for ( uiInputIndex = 0; uiInputIndex < BMUXLIB_TS_MAX_INPUT_PIDS; uiInputIndex++ )
      {
         if ( 0 != auiInputDescriptorsConsumed[uiInputIndex] )
         {
            BMUXlib_Input_ConsumeDescriptors( hMuxTS->status.stInputMetaData[uiInputIndex].hInput, auiInputDescriptorsConsumed[uiInputIndex] );
         }
      }
   }

   /* free up any userdata that may have been returned from transport */
   BMUXlib_TS_P_Userdata_ProcessReleaseQueues(hMuxTS);

   BDBG_LEAVE( BMUXlib_TS_P_ProcessCompletedBuffers );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_CalculateNextESCR(
         uint32_t uiCurrentESCR,
         size_t uiLength,
         uint32_t uiPacket2PacketDeltaTimestamp,
         uint32_t *puiNextESCR
         )
{
   /* Set next expected ESCR value */
   uint64_t uiNextESCR = uiPacket2PacketDeltaTimestamp;

   BDBG_ASSERT( puiNextESCR );

   uiNextESCR *= (uiLength + (BMUXlib_TS_P_TSPacket_MAXSIZE - 1))/BMUXlib_TS_P_TSPacket_MAXSIZE;

   /* Scale expected delta by 1% to account for TS packetization overhead */
   uiNextESCR *= 100;
   uiNextESCR /= 99;

   uiNextESCR += uiCurrentESCR;

   *puiNextESCR = (uiNextESCR & 0xFFFFFFFF);

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_AddTransportDescriptor(
   BMUXlib_TS_Handle hMuxTS,
   unsigned uiTransportChannelIndex,
   BMUXlib_TS_TransportDescriptor *pstTransportDescriptor
   )
{

   if ( true == pstTransportDescriptor->stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid )
   {
      hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] = pstTransportDescriptor->stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp;
   }

   if ( true == pstTransportDescriptor->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid )
   {
      hMuxTS->status.uiTransportCurrentPacket2PacketTimestampDelta[uiTransportChannelIndex] = pstTransportDescriptor->stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta;
   }

   BMUXlib_TS_P_CalculateNextESCR(
      hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex],
      pstTransportDescriptor->uiBufferLength,
      hMuxTS->status.uiTransportCurrentPacket2PacketTimestampDelta[uiTransportChannelIndex],
      &hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex]
   );

   BMEM_FlushCache(
      hMuxTS->stCreateSettings.hMem,
      (void*) pstTransportDescriptor->pBufferAddress,
      pstTransportDescriptor->uiBufferLength
      );

   return BERR_TRACE( BMUXlib_List_Add(
            hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex],
            pstTransportDescriptor
            ) );
}

typedef struct BMUXlib_P_PESInfo
{
      BMUXlib_TS_P_SourceType eSourceType;
      unsigned uiTransportChannelIndex;
      uint8_t uiPESStreamID;

      uint64_t uiPTS;
      uint64_t uiDTS;
      uint32_t uiESCR;
      uint16_t uiTicksPerBit;
      int16_t iSHR;

      uint16_t uiLength;

      bool bSourceTypeValid;
      bool bTransportChannelIndexValid;

      bool bPTSValid;
      bool bDTSValid;
      bool bESCRValid;
      bool bTicksPerBitValid;
      bool bSHRValid;
      bool bLengthValid;

      bool bRAI;
} BMUXlib_P_PESInfo;

const BMUXlib_TS_P_SourceType BMUXLIB_TS_P_InputDescriptorTypeLUT[BMUXlib_Input_Type_eMax] =
{
   BMUXlib_TS_P_SourceType_eVideo, /* BMUXlib_Input_Type_eVideo */
   BMUXlib_TS_P_SourceType_eAudio, /* BMUXlib_Input_Type_eAudio */
};

BERR_Code
BMUXlib_TS_P_PopulatePESInfoFromInputDescriptor(
         const BMUXlib_Input_Descriptor *pstDescriptor,
         BMUXlib_P_PESInfo *pstPESInfo
        )
{
   BDBG_ASSERT( pstDescriptor );
   BDBG_ASSERT( pstPESInfo );

   BKNI_Memset(
            pstPESInfo,
            0,
            sizeof( BMUXlib_P_PESInfo )
            );

   /* Common */
   pstPESInfo->bPTSValid = BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID( pstDescriptor );
   pstPESInfo->uiPTS = BMUXLIB_INPUT_DESCRIPTOR_PTS ( pstDescriptor );

   pstPESInfo->bESCRValid = BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( pstDescriptor );
   pstPESInfo->uiESCR = BMUXLIB_INPUT_DESCRIPTOR_ESCR( pstDescriptor );

   pstPESInfo->bTicksPerBitValid = BMUXLIB_INPUT_DESCRIPTOR_IS_TICKS_PER_BIT_VALID ( pstDescriptor );
   pstPESInfo->uiTicksPerBit = BMUXLIB_INPUT_DESCRIPTOR_TICKS_PER_BIT( pstDescriptor );

   pstPESInfo->bSHRValid = BMUXLIB_INPUT_DESCRIPTOR_IS_SHR_VALID( pstDescriptor );
   pstPESInfo->iSHR = BMUXLIB_INPUT_DESCRIPTOR_SHR( pstDescriptor );

   pstPESInfo->bRAI = BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME( pstDescriptor );

   pstPESInfo->eSourceType = BMUXLIB_TS_P_InputDescriptorTypeLUT[BMUXLIB_INPUT_DESCRIPTOR_TYPE( pstDescriptor )];
   pstPESInfo->bSourceTypeValid = true;

   switch ( pstPESInfo->eSourceType )
   {
      case BMUXlib_TS_P_SourceType_eVideo:
         pstPESInfo->bDTSValid = BMUXLIB_INPUT_DESCRIPTOR_IS_DTS_VALID( pstDescriptor );
         pstPESInfo->uiDTS = BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DTS( pstDescriptor );
         break;

      case BMUXlib_TS_P_SourceType_eAudio:
         pstPESInfo->bLengthValid = true;
         pstPESInfo->uiLength = BMUXLIB_INPUT_DESCRIPTOR_FRAMESIZE( pstDescriptor );
         /* We add 13 bytes to payload size to account for the Optional PES Header */
         pstPESInfo->uiLength += 13;
         break;

      default:
         break;
   }

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_InsertPESHeader(
         BMUXlib_TS_Handle hMuxTS,
         BMUXlib_P_PESInfo *pstPESInfo
         )
{
   BDBG_ASSERT( true == pstPESInfo->bTransportChannelIndexValid );

   if ( false == BMUXlib_List_IsEmpty(
                 hMuxTS->hPESHeaderFreeList
                 )
      )
   {
      void *pEntry = NULL;
      /* Allocate PES Header */
      BMUXlib_TS_P_PESHeader *pstCurrentPESHeader = NULL;

      BERR_TRACE( BMUXlib_List_Remove(
               hMuxTS->hPESHeaderFreeList,
               &pEntry
               ) );

      pstCurrentPESHeader = (BMUXlib_TS_P_PESHeader *) pEntry;

      /* Populate PES Header */
      BKNI_Memcpy(
               pstCurrentPESHeader->data,
               &s_stDefaultPESHeader,
               sizeof( BMUXlib_TS_P_PESHeader )
               );

      /* Set Stream ID */
      BMUXlib_TS_P_PESHeader_SetStreamID(
               pstCurrentPESHeader->data,
               pstPESInfo->uiPESStreamID
               );

      /* Set PTS */
      /* TODO: Optimize by not sending PTS portion of PES Header if PTS doesn't exist */
      if ( true == pstPESInfo->bPTSValid )
      {
         BMUXlib_TS_P_PESHeader_SetPTS(
                  pstCurrentPESHeader->data,
                  pstPESInfo->uiPTS
                  );
      }

      /* Set DTS */
      /* TODO: Optimize by not sending DTS portion of PES Header if DTS doesn't exist */
      if ( ( true == pstPESInfo->bDTSValid )
           && ( true == pstPESInfo->bPTSValid )
           && ( pstPESInfo->uiPTS != pstPESInfo->uiDTS )
         )
      {
         BMUXlib_TS_P_PESHeader_SetDTS(
                  pstCurrentPESHeader->data,
                  pstPESInfo->uiDTS
                  );
      }

      /* Set Length */
      if ( true == pstPESInfo->bLengthValid )
      {
         BMUXlib_TS_P_PESHeader_SetLength(
                  pstCurrentPESHeader->data,
                  pstPESInfo->uiLength
                  );
      }

      {
         /* Allocate Transport Descriptor */
         /* TODO: Use cached data access */
         BMUXlib_TS_TransportDescriptor *pstCurrentTransportDescriptor = NULL;
         BMUXlib_TS_P_TransportDescriptorMetaData *pstCurrentTransportDescriptorMetaData = NULL;
         void *pEntry = NULL;

         BERR_TRACE( BMUXlib_List_Remove(
                  hMuxTS->hTransportDescriptorMetaDataFreeList,
                  &pEntry
                  ) );

         pstCurrentTransportDescriptorMetaData = (BMUXlib_TS_P_TransportDescriptorMetaData *) pEntry;

         /* Populate Transport Meta Data */
         BKNI_Memset(
                  pstCurrentTransportDescriptorMetaData,
                  0,
                  sizeof( BMUXlib_TS_P_TransportDescriptorMetaData )
                  );

         pstCurrentTransportDescriptorMetaData->eDataType = BMUXlib_TS_P_DataType_ePESHeader;
         pstCurrentTransportDescriptorMetaData->eSourceType = pstPESInfo->eSourceType;

         BERR_TRACE( BMUXlib_List_Add(
                  hMuxTS->hTransportDescriptorMetaDataPendingList[pstPESInfo->uiTransportChannelIndex],
                  pstCurrentTransportDescriptorMetaData
                  ) );

         BERR_TRACE( BMUXlib_List_Remove(
                  hMuxTS->hTransportDescriptorFreeList,
                  &pEntry
                  ) );

         pstCurrentTransportDescriptor = (BMUXlib_TS_TransportDescriptor *) pEntry;

         /* Populate Transport Descriptor */
         BKNI_Memset(
                  pstCurrentTransportDescriptor,
                  0,
                  sizeof( BMUXlib_TS_TransportDescriptor )
                  );

         /* Set Buffer Info */
         pstCurrentTransportDescriptor->pBufferAddress = pstCurrentPESHeader;
         pstCurrentTransportDescriptor->uiBufferLength = sizeof( BMUXlib_TS_P_PESHeader );

         /* Set ESCR */
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid = pstPESInfo->bESCRValid;
         if ( true == pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid )
         {
            pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp = pstPESInfo->uiESCR;
         }

         /* Set Packet 2 Packet Timestamp Delta */
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid = pstPESInfo->bTicksPerBitValid;
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid &= pstPESInfo->bSHRValid;
         if ( true == pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid )
         {
            uint64_t uiTempPacket2PacketTimestampDelta = pstPESInfo->uiTicksPerBit;
            uiTempPacket2PacketTimestampDelta *= 184 * 8 * 99;
            uiTempPacket2PacketTimestampDelta /= 100;

            if ( pstPESInfo->iSHR > 0 )
            {
               pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta = uiTempPacket2PacketTimestampDelta >> pstPESInfo->iSHR;
            }
            else
            {
               pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta = uiTempPacket2PacketTimestampDelta << -pstPESInfo->iSHR;
            }
         }

         /* Set Random Access Indicator */
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bRandomAccessIndication = pstPESInfo->bRAI;

         BERR_TRACE( BMUXlib_TS_P_AddTransportDescriptor( hMuxTS, pstPESInfo->uiTransportChannelIndex, pstCurrentTransportDescriptor ) );
      }
   }
   else
   {
      return BERR_TRACE( BERR_UNKNOWN );
   }

   return BERR_TRACE( BERR_SUCCESS );
}

typedef struct BMUXlib_P_BPPInfo
{
      bool bSourceTypeValid;
      bool bTransportChannelIndexValid;
      bool bSkippedDescriptorsValid;
      bool bInputIndexValid;

      BMUXlib_TS_P_SourceType eSourceType;
      unsigned uiTransportChannelIndex;
      unsigned uiSkippedDescriptors;
      unsigned uiInputIndex;

      void *pBuffer;
      size_t uiLength;
} BMUXlib_P_BPPInfo;

BERR_Code
BMUXlib_TS_P_InsertBPP(
         BMUXlib_TS_Handle hMuxTS,
         BMUXlib_P_BPPInfo *pstBPPInfo
         )
{
   {
      /* Allocate Transport Descriptor */
      /* TODO: Use cached data access */
      BMUXlib_TS_TransportDescriptor *pstCurrentTransportDescriptor = NULL;
      BMUXlib_TS_P_TransportDescriptorMetaData *pstCurrentTransportDescriptorMetaData = NULL;
      void *pEntry = NULL;

      BERR_TRACE( BMUXlib_List_Remove(
               hMuxTS->hTransportDescriptorMetaDataFreeList,
               &pEntry
               ) );

      pstCurrentTransportDescriptorMetaData = (BMUXlib_TS_P_TransportDescriptorMetaData *) pEntry;

      /* Populate Transport Meta Data */
      BKNI_Memset(
               pstCurrentTransportDescriptorMetaData,
               0,
               sizeof( BMUXlib_TS_P_TransportDescriptorMetaData )
               );

      pstCurrentTransportDescriptorMetaData->eDataType = BMUXlib_TS_P_DataType_eBPP;
      pstCurrentTransportDescriptorMetaData->eSourceType = pstBPPInfo->eSourceType;

      if ( true == pstBPPInfo->bSkippedDescriptorsValid )
      {
         if ( 0 != pstBPPInfo->uiSkippedDescriptors )
         {
            pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount += pstBPPInfo->uiSkippedDescriptors;
            BDBG_ASSERT( pstBPPInfo->bInputIndexValid );
            pstCurrentTransportDescriptorMetaData->uiInputIndex = pstBPPInfo->uiInputIndex;
         }
      }


      BERR_TRACE( BMUXlib_List_Add(
               hMuxTS->hTransportDescriptorMetaDataPendingList[pstBPPInfo->uiTransportChannelIndex],
               pstCurrentTransportDescriptorMetaData
               ) );

      BERR_TRACE( BMUXlib_List_Remove(
               hMuxTS->hTransportDescriptorFreeList,
               &pEntry
               ) );

      pstCurrentTransportDescriptor = (BMUXlib_TS_TransportDescriptor *) pEntry;

      /* Populate Transport Descriptor */
      BKNI_Memset(
               pstCurrentTransportDescriptor,
               0,
               sizeof( BMUXlib_TS_TransportDescriptor )
               );

      /* Set Buffer Info */
      pstCurrentTransportDescriptor->pBufferAddress = pstBPPInfo->pBuffer;
      pstCurrentTransportDescriptor->uiBufferLength = pstBPPInfo->uiLength;

      BERR_TRACE( BMUXlib_TS_P_AddTransportDescriptor( hMuxTS, pstBPPInfo->uiTransportChannelIndex, pstCurrentTransportDescriptor ) );
   }

   return BERR_TRACE( BERR_SUCCESS );
}


typedef struct BMUXlib_P_TransportInfo
{
      BMUXlib_TS_P_SourceType eSourceType;
      unsigned uiTransportChannelIndex;

      void *pBuffer;
      size_t uiLength;

      unsigned uiSkippedDescriptors;
      unsigned uiInputIndex;

      bool bSourceTypeValid;
      bool bTransportChannelIndexValid;
      bool bBufferValid;
      bool bLengthValid;
      bool bSkippedDescriptorsValid;
      bool bInputIndexValid;
} BMUXlib_P_TransportInfo;

BERR_Code
BMUXlib_TS_P_PopulateTransportInfoFromInputDescriptor(
         const BMUXlib_Input_Descriptor *pstDescriptor,
         BMUXlib_P_TransportInfo *pstTransportInfo
        )
{
   BDBG_ASSERT( pstDescriptor );
   BDBG_ASSERT( pstTransportInfo );

   BKNI_Memset(
            pstTransportInfo,
            0,
            sizeof( BMUXlib_P_TransportInfo )
            );

   pstTransportInfo->uiLength = BMUXLIB_INPUT_DESCRIPTOR_LENGTH( pstDescriptor );
   pstTransportInfo->bLengthValid = true;

   pstTransportInfo->pBuffer = (void *) BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS( pstDescriptor );
   pstTransportInfo->bBufferValid = true;

   pstTransportInfo->eSourceType = BMUXLIB_TS_P_InputDescriptorTypeLUT[BMUXLIB_INPUT_DESCRIPTOR_TYPE( pstDescriptor )];
   pstTransportInfo->bSourceTypeValid = true;

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_InsertTransportDescriptor (
         BMUXlib_TS_Handle hMuxTS,
         BMUXlib_P_TransportInfo *pstTransportInfo
         )
{
   /* Allocate Transport Descriptor */
   /* TODO: Use cached data access */
   BMUXlib_TS_TransportDescriptor *pstCurrentTransportDescriptor = NULL;
   BMUXlib_TS_P_TransportDescriptorMetaData *pstCurrentTransportDescriptorMetaData = NULL;
   void *pEntry = NULL;

   BDBG_ASSERT( pstTransportInfo );
   BDBG_ASSERT( pstTransportInfo->bSourceTypeValid );
   BDBG_ASSERT( pstTransportInfo->bTransportChannelIndexValid );
   BDBG_ASSERT( pstTransportInfo->bBufferValid );
   BDBG_ASSERT( pstTransportInfo->bLengthValid );

   BERR_TRACE( BMUXlib_List_Remove(
            hMuxTS->hTransportDescriptorMetaDataFreeList,
            &pEntry
            ) );

   pstCurrentTransportDescriptorMetaData = (BMUXlib_TS_P_TransportDescriptorMetaData *) pEntry;

   /* Populate Transport Meta Data */
   BKNI_Memset(
            pstCurrentTransportDescriptorMetaData,
            0,
            sizeof( BMUXlib_TS_P_TransportDescriptorMetaData )
            );

   pstCurrentTransportDescriptorMetaData->eDataType = BMUXlib_TS_P_DataType_eCDB;
   pstCurrentTransportDescriptorMetaData->eSourceType = pstTransportInfo->eSourceType;
   pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount = 1;

   if ( true == pstTransportInfo->bSkippedDescriptorsValid )
   {
      pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount += pstTransportInfo->uiSkippedDescriptors;
   }

   if ( true == pstTransportInfo->bInputIndexValid )
   {
      pstCurrentTransportDescriptorMetaData->uiInputIndex = pstTransportInfo->uiInputIndex;
   }
   else
   {
      if ( ( BMUXlib_TS_P_SourceType_eAudio == pstTransportInfo->eSourceType )
           || ( BMUXlib_TS_P_SourceType_eVideo == pstTransportInfo->eSourceType ) )
      {
         BDBG_ERR(("uiInputIndex required for audio/video types"));
         BDBG_ASSERT(0);
      }
   }

   BERR_TRACE( BMUXlib_List_Add(
            hMuxTS->hTransportDescriptorMetaDataPendingList[pstTransportInfo->uiTransportChannelIndex],
            pstCurrentTransportDescriptorMetaData
            ) );

   BERR_TRACE( BMUXlib_List_Remove(
            hMuxTS->hTransportDescriptorFreeList,
            &pEntry
            ) );

   pstCurrentTransportDescriptor = (BMUXlib_TS_TransportDescriptor *) pEntry;

   /* Populate Transport Descriptor */
   BKNI_Memset(
            pstCurrentTransportDescriptor,
            0,
            sizeof( BMUXlib_TS_TransportDescriptor )
            );

   /* Set Buffer Info */
   pstCurrentTransportDescriptor->pBufferAddress = pstTransportInfo->pBuffer;
   pstCurrentTransportDescriptor->uiBufferLength = pstTransportInfo->uiLength;

   BERR_TRACE( BMUXlib_TS_P_AddTransportDescriptor( hMuxTS, pstTransportInfo->uiTransportChannelIndex, pstCurrentTransportDescriptor ) );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_InsertNULLTransportDescriptor (
         BMUXlib_TS_Handle hMuxTS,
         unsigned uiTransportChannelIndex,
         uint32_t uiESCR,
         BMUXlib_TS_P_DataType eDataType,
         BMUXlib_TS_P_SourceType eSourceType,
         unsigned uiInputIndex
         )
{
   /* Allocate Transport Descriptor */
   /* TODO: Use cached data access */
   BMUXlib_TS_TransportDescriptor *pstCurrentTransportDescriptor = NULL;
   BMUXlib_TS_P_TransportDescriptorMetaData *pstCurrentTransportDescriptorMetaData = NULL;
   void *pEntry = NULL;

   BDBG_ASSERT( hMuxTS );

   BERR_TRACE( BMUXlib_List_Remove(
            hMuxTS->hTransportDescriptorMetaDataFreeList,
            &pEntry
            ) );

   pstCurrentTransportDescriptorMetaData = (BMUXlib_TS_P_TransportDescriptorMetaData *) pEntry;

   /* Populate Transport Meta Data */
   BKNI_Memset(
            pstCurrentTransportDescriptorMetaData,
            0,
            sizeof( BMUXlib_TS_P_TransportDescriptorMetaData )
            );

   pstCurrentTransportDescriptorMetaData->eDataType = eDataType;
   pstCurrentTransportDescriptorMetaData->eSourceType = eSourceType;
   if ( BMUXlib_TS_P_DataType_eCDB == pstCurrentTransportDescriptorMetaData->eDataType )
   {
      pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount = 1;
      pstCurrentTransportDescriptorMetaData->uiInputIndex = uiInputIndex;
   }

   BERR_TRACE( BMUXlib_List_Add(
            hMuxTS->hTransportDescriptorMetaDataPendingList[uiTransportChannelIndex],
            pstCurrentTransportDescriptorMetaData
            ) );

   BERR_TRACE( BMUXlib_List_Remove(
            hMuxTS->hTransportDescriptorFreeList,
            &pEntry
            ) );

   pstCurrentTransportDescriptor = (BMUXlib_TS_TransportDescriptor *) pEntry;

   /* Populate Transport Descriptor */
   BKNI_Memset(
            pstCurrentTransportDescriptor,
            0,
            sizeof( BMUXlib_TS_TransportDescriptor )
            );

   /* Set Buffer Info */
   pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid = true;
   pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp = uiESCR;

   /* In case host translates address, we want to avoid sending buffer address = 0, so we just set something, but length to 0 */
   pstCurrentTransportDescriptor->pBufferAddress = hMuxTS->astBPPData[0].data;
   pstCurrentTransportDescriptor->uiBufferLength = 0;

   BERR_TRACE( BMUXlib_TS_P_AddTransportDescriptor( hMuxTS, uiTransportChannelIndex, pstCurrentTransportDescriptor ) );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_ProcessSystemData(
         BMUXlib_TS_Handle hMuxTS
         )
{
   BMUXlib_TS_P_TSPacket *pstPCRPacket = NULL;
   BMUXlib_TS_TransportDescriptor *pstCurrentTransportDescriptor = NULL;
   BMUXlib_TS_P_TransportDescriptorMetaData *pstCurrentTransportDescriptorMetaData = NULL;
   uint32_t uiLastPendingESCR = 0xFFFFFFFF;
   bool bLastPendingESCRValid = false;

   BDBG_ENTER( BMUXlib_TS_P_ProcessSystemData );

   BDBG_ASSERT( hMuxTS );

   if ( BMUXlib_List_IsFull ( hMuxTS->hTransportDescriptorPendingList[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex] ) )
   {
      if ( false == hMuxTS->status.stStartSettings.bNonRealTimeMode )
      {
         if ( false == hMuxTS->status.bTransportChannelStalled[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex] )
         {
            BDBG_ERR(("TRANSPORT[%d] Stalled...ignoring SYSTEM[%d]", hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex, 0));
         }
      }

      hMuxTS->status.bTransportChannelStalled[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex] = true;

      if ( false == hMuxTS->status.stStartSettings.bNonRealTimeMode )
      {
         return BERR_TRACE( BERR_UNKNOWN );
      }
      else
      {
         return BERR_TRACE( BERR_SUCCESS );
      }
   }
   else
   {
      hMuxTS->status.bTransportChannelStalled[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex] = false;
   }

   /* Determine how many PCRs we need to generate based on the
    * largest of the last pending ESCRs in the audio/video data */
    /* FIXME: This should be max of (last pending ESCR, starting ESCR + mux service period) */
   {
      uint32_t uiTransportChannelIndex;
      for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
      {
         if ( ( true == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].bActive )
              && ( false == BMUXlib_List_IsEmpty(
                     hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex]
                  )
              )
              && ( uiTransportChannelIndex != hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex )
            )
         {
            if ( /* Use largest ESCR */
                 ( ( ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] > uiLastPendingESCR )
                      && ( ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] - uiLastPendingESCR ) < 0x80000000 ) )
                   || ( ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] < uiLastPendingESCR )
                        && ( ( uiLastPendingESCR - hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex]) > 0x80000000 ) ) )
                 || ( false == bLastPendingESCRValid )
               )
               {
                  uiLastPendingESCR = hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex];
                  bLastPendingESCRValid = true;
               }
         }
      }
   }

   if ( ( true == bLastPendingESCRValid )
        && ( true == hMuxTS->status.stPCRInfo.bInitialized )
      )
   {
      /* Ticks per packet for system data
         We use 188 bytes, because we are always sending complete TS packets on the system data channel */
      uint64_t uiPacket2PacketTimestampDelta = ((uint64_t)BMUXlib_TS_P_TSPacket_MAXSIZE * 8 * 27000000) / hMuxTS->status.stMuxSettings.uiSystemDataBitRate;

      while ( ( false == BMUXlib_List_IsEmpty(
               hMuxTS->hTransportDescriptorFreeList
               )
           )
           &&
           ( false == BMUXlib_List_IsEmpty(
                    hMuxTS->hTSPacketFreeList
                    )
           )
           &&
           ( false == BMUXlib_List_IsFull(
                    hMuxTS->hTransportDescriptorPendingList[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex]
                    )
           )
           /* FIXME: In here make sure we use max of these last pending ESCRs and starting ESCR + Mux service period to ensure we have enough time to insert
              userdata even if video frames are very short */
           && ((((hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF) < uiLastPendingESCR) /* Next PCR's ESCR is smaller than last pending ESCR */
                 && ((uiLastPendingESCR - (hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF) < 0x80000000))) /* But not too small (E.g. ESCR = 0xFFFFFFFF and next PCR = 0x00000000, we don't want to schedule it yet */
               || (((hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF) >= uiLastPendingESCR) /* Next PCR's ESCR is larger than the last pending ESCR */
                 && (((hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF) - uiLastPendingESCR) > 0x80000000)) /* But much larger (E.g. ESCR = 0x00000000 and next PCR = 0xFFFFFFFF, we DO want to schedule it now */
           )
         )

      {
         void *pEntry = NULL;
         /* calculate Next PCR's expected ESCR value for future reference */
         uint64_t uiNextESCR = hMuxTS->status.stPCRInfo.uiESCR + (BMUXLIB_TS_P_SCALE_MS_TO_27MHZ * hMuxTS->status.stStartSettings.stPCRData.uiInterval);
         /* Handle MOD300 wrap */
         hMuxTS->status.stPCRInfo.uiNextESCR = (((uiNextESCR >> 32) % 0x258) << 32) | (uiNextESCR & 0xFFFFFFFF);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	if(hMuxTS->status.stStartSettings.stPCRData.bNonPcrInsert == false)
	{
#endif
         BERR_TRACE( BMUXlib_List_Remove(
                  hMuxTS->hTSPacketFreeList,
                  &pEntry
                  ) );

         pstPCRPacket = (BMUXlib_TS_P_TSPacket *) pEntry;

#if 0
         BDBG_ERR(("PCR --> %08llx/%08x %011llx (%09llx) %lld @%p", (hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF), uiLastPendingESCR, hMuxTS->status.stPCRInfo.uiESCR, hMuxTS->status.stPCRInfo.uiBase, ((uint64_t)hMuxTS->status.stPCRInfo.uiBase)*300+hMuxTS->status.stPCRInfo.uiExtension,pstPCRPacket ));
#endif

         /* Populate the PCR packet */
         BKNI_Memcpy(
                  pstPCRPacket->data,
                  &s_stDefaultTSPacket,
                  sizeof( BMUXlib_TS_P_TSPacket )
                  );

         BMUXlib_TS_P_TSPacket_SetPID(
                  pstPCRPacket->data,
                  hMuxTS->status.stStartSettings.stPCRData.uiPID
                  );

         BMUXlib_TS_P_TSPacket_SetAdaptationPresent(
                  pstPCRPacket->data,
                  true
                  );

         BMUXlib_TS_P_TSPacket_SetAdaptationLength(
                  pstPCRPacket->data,
                  183 /* Always 183 for TS packets that contain only adaptation and no payload */
                  );

         BMUXlib_TS_P_TSPacket_SetPCRPresent(
                  pstPCRPacket->data,
                  true
                  );

         BMUXlib_TS_P_TSPacket_SetPCRBase(
                  pstPCRPacket->data,
                  hMuxTS->status.stPCRInfo.uiBase
                  );

         BMUXlib_TS_P_TSPacket_SetPCRExtension(
                  pstPCRPacket->data,
                  hMuxTS->status.stPCRInfo.uiExtension
                  );

         BERR_TRACE( BMUXlib_List_Remove(
                  hMuxTS->hTransportDescriptorMetaDataFreeList,
                  &pEntry
                  ) );

         pstCurrentTransportDescriptorMetaData = (BMUXlib_TS_P_TransportDescriptorMetaData *) pEntry;

         /* Populate Transport Meta Data */
         BKNI_Memset(
                  pstCurrentTransportDescriptorMetaData,
                  0,
                  sizeof( BMUXlib_TS_P_TransportDescriptorMetaData )
                  );

         pstCurrentTransportDescriptorMetaData->eDataType = BMUXlib_TS_P_DataType_ePCRPacket;
         pstCurrentTransportDescriptorMetaData->eSourceType = BMUXlib_TS_P_SourceType_eSystem;

         BERR_TRACE( BMUXlib_List_Add(
                  hMuxTS->hTransportDescriptorMetaDataPendingList[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex],
                  pstCurrentTransportDescriptorMetaData
                  ) );

         BERR_TRACE( BMUXlib_List_Remove(
                  hMuxTS->hTransportDescriptorFreeList,
                  &pEntry
                  ) );

         pstCurrentTransportDescriptor = (BMUXlib_TS_TransportDescriptor *) pEntry;
         BDBG_ASSERT(pstCurrentTransportDescriptor);

         /* Populate Transport Descriptor */
         /* coverity[deref_ptr_in_call] */
         BKNI_Memset(
                  pstCurrentTransportDescriptor,
                  0,
                  sizeof( BMUXlib_TS_TransportDescriptor )
                  );

         /* Set ESCR */
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid = true;
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp = hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF;

         /* Set Packet 2 Packet Timestamp Delta */
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid = true;
         pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta = uiPacket2PacketTimestampDelta;

         /* Set Buffer Info */
         pstCurrentTransportDescriptor->pBufferAddress = (void *) pstPCRPacket;
         pstCurrentTransportDescriptor->uiBufferLength = sizeof( BMUXlib_TS_P_TSPacket );

         BERR_TRACE( BMUXlib_TS_P_AddTransportDescriptor( hMuxTS, hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex, pstCurrentTransportDescriptor ) );
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	}
	else
	{
		/* If not insert PCR ts packet and under the value is 0xFFFFFFFF,
		* this routine is not needed.
		* (Under the situation, it is going to be blocking until 0xFFFFFFFF, uiESCR value)
		*/
		if(uiLastPendingESCR == 0xFFFFFFFF)
			break;
	}
#endif
         /* system data starting ESCR = ESCR of PCR + delta(PCR) */
            hMuxTS->status.stSystemDataInfo.uiESCR = ( hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF );
         hMuxTS->status.stSystemDataInfo.uiESCR += uiPacket2PacketTimestampDelta;   /* only one packet sent for PCR */

         /* After inserting the PCR, we want to schedule as many userdata and system data buffer packets in between that we can */
         /* determine how many packets we can fit between PCRs ...
            (-1 to account for the PCR packet itself) */
         hMuxTS->status.stSystemDataInfo.uiPacketsUntilNextPCR =
                  ((hMuxTS->status.stPCRInfo.uiNextESCR - hMuxTS->status.stPCRInfo.uiESCR) / uiPacket2PacketTimestampDelta) - 1;

         /* if there is userdata, schedule it after the PCR ... */
         BMUXlib_TS_P_Userdata_SchedulePackets(hMuxTS);

         /* if there is still space, schedule system data ... */
         while ( ( hMuxTS->status.stSystemDataInfo.uiPacketsUntilNextPCR != 0 ) /* We have space in between PCR packets for more system data */
                    && ( false == BMUXlib_List_IsEmpty( /* We have transport descriptors available */
                             hMuxTS->hTransportDescriptorFreeList
                             )
                       )
                    && ( false == BMUXlib_List_IsEmpty( /* We have system data available */
                             hMuxTS->hSystemDataPendingList
                             )
                         )
                    && ( false == BMUXlib_List_IsFull(
                             hMuxTS->hTransportDescriptorPendingList[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex]
                         )
                       )
                  )
         {
            BMUXlib_TS_P_SystemDataInfo *pSystemDataInfo = &hMuxTS->status.stSystemDataInfo;
            uint32_t uiNextPCRESCR = hMuxTS->status.stPCRInfo.uiNextESCR;
            uint32_t uiPacketsNeeded;
            uint32_t uiPacketsToMux;
            BMUXlib_TS_SystemData *pstSystemDataBuffer = NULL;
            void *pEntry = NULL;
            uint32_t uiMinInsertionTime = pSystemDataInfo->uiESCR;

            BDBG_MODULE_MSG(BMUXLIB_TS_SYSDATA, ("Curr ESCR: %x, Next PCR: %x", pSystemDataInfo->uiESCR, uiNextPCRESCR));

            BMUXlib_List_GetHead(
                     hMuxTS->hSystemDataPendingList,
                     &pEntry
                     );

            pstSystemDataBuffer = (BMUXlib_TS_SystemData *) pEntry;

            /* SW7425-1188: Pace system data insertion according to specified timestamp delta */
            /* if first system data packet, insert it straight away regardless of its timestamp delta */
            if (pSystemDataInfo->bLastScheduledESCRValid)
            {
               uiMinInsertionTime = pSystemDataInfo->uiLastScheduledESCR +
                     (pstSystemDataBuffer->uiTimestampDelta * BMUXLIB_TS_P_SCALE_MS_TO_27MHZ);

               BDBG_MODULE_MSG(BMUXLIB_TS_SYSDATA, ("Delta = %x, Min Insertion = %x, Last Sched = %x",
                  pstSystemDataBuffer->uiTimestampDelta, uiMinInsertionTime, pSystemDataInfo->uiLastScheduledESCR));

               /* check if min insertion time < next PCR insertion time (accounting for wrap) */
               if ((int32_t)(uiNextPCRESCR - uiMinInsertionTime) > 0)
               {
                  /* see if the insertion of this data needs to be delayed within the current PCR interval */
                  if ((int32_t)(pSystemDataInfo->uiESCR - uiMinInsertionTime) < 0)
                  {
                     /* time to insertion point of this system data is min insertion time - current time */
                     uint32_t uiTimeToInsertionPoint = uiMinInsertionTime - pSystemDataInfo->uiESCR;
                     /* number of packets required to reach the desired insertion point (rounded
                        up to the next packet boundary) */
                     uint32_t uiPacketsToInsertionPoint = uiTimeToInsertionPoint / uiPacket2PacketTimestampDelta;

                     BDBG_MODULE_MSG(BMUXLIB_TS_SYSDATA, ("Packets to Insertion = %d (packets to PCR = %d), Time to insertion = %x, dP2P = %x",
                           uiPacketsToInsertionPoint, pSystemDataInfo->uiPacketsUntilNextPCR, uiTimeToInsertionPoint, uiPacket2PacketTimestampDelta));

                     if (uiPacketsToInsertionPoint >= pSystemDataInfo->uiPacketsUntilNextPCR)
                     {
                        /* no room for this data in this interval */
                        break; /* no need to update system data ESCR - it will get reset next interval based on PCR */
                     }
                     /* this is effectively inserting a "gap" between the PCR and the outgoing system data */
                     pSystemDataInfo->uiPacketsUntilNextPCR -= uiPacketsToInsertionPoint;
                     pSystemDataInfo->uiESCR += (uiPacketsToInsertionPoint * uiPacket2PacketTimestampDelta);
                     BDBG_MODULE_MSG(BMUXLIB_TS_SYSDATA, ("Updated: Packets to next PCR = %d, sys data ESCR = %x",
                        pSystemDataInfo->uiPacketsUntilNextPCR, pSystemDataInfo->uiESCR));
                  }
                  else
                  {
                     /* else, min insertion time <= current time
                        => this insertion needs to be scheduled now:
                        * incomplete system data from previous insertion, OR
                        * data that did not fit in previous interval, OR
                        * specified timestamp delta was smaller than the previous system data length, OR
                        * data that was to be scheduled at same time as PCR
                        So, insert this data now */
                     uiMinInsertionTime = pSystemDataInfo->uiESCR;    /* current insertion time */
                  }
               }
               else
               {
                  /* do not insert this system data yet - not time to insert it yet ... wait */
                  BDBG_MODULE_MSG(BMUXLIB_TS_SYSDATA, ("Waiting for system data insertion"));
                  break;
               }
            }

            pSystemDataInfo->uiLastScheduledESCR = uiMinInsertionTime;
            pSystemDataInfo->bLastScheduledESCRValid = true;
            /* ------------- end: SW7425-1188 ------------- */

            /* Compute how many packets we need to mux the next system data buffer */
            uiPacketsNeeded = ( pstSystemDataBuffer->uiSize / BMUXlib_TS_P_TSPacket_MAXSIZE ) - ( pSystemDataInfo->uiPacketsMuxedSoFar );
            uiPacketsToMux = ( uiPacketsNeeded < pSystemDataInfo->uiPacketsUntilNextPCR ) ? ( uiPacketsNeeded ) : pSystemDataInfo->uiPacketsUntilNextPCR;
            BDBG_MODULE_MSG(BMUXLIB_TS_SYSDATA, ("Packets Needed = %d, Packets to Mux = %d", uiPacketsNeeded, uiPacketsToMux));

            BERR_TRACE( BMUXlib_List_Remove(
                     hMuxTS->hTransportDescriptorMetaDataFreeList,
                     &pEntry
                     ) );

            pstCurrentTransportDescriptorMetaData = (BMUXlib_TS_P_TransportDescriptorMetaData *) pEntry;

            /* Populate Transport Meta Data */
            BKNI_Memset(
                     pstCurrentTransportDescriptorMetaData,
                     0,
                     sizeof( BMUXlib_TS_P_TransportDescriptorMetaData )
                     );

            pstCurrentTransportDescriptorMetaData->eDataType = BMUXlib_TS_P_DataType_eCDB;
            pstCurrentTransportDescriptorMetaData->eSourceType = BMUXlib_TS_P_SourceType_eSystem;

            BERR_TRACE( BMUXlib_List_Add(
                     hMuxTS->hTransportDescriptorMetaDataPendingList[hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex],
                     pstCurrentTransportDescriptorMetaData
                     ) );

            BERR_TRACE( BMUXlib_List_Remove(
                     hMuxTS->hTransportDescriptorFreeList,
                     &pEntry
                     ) );

            pstCurrentTransportDescriptor = (BMUXlib_TS_TransportDescriptor *) pEntry;
            BDBG_ASSERT(pstCurrentTransportDescriptor);

            /* Populate Transport Descriptor */
            /* coverity[deref_ptr_in_call] */
            BKNI_Memset(
                     pstCurrentTransportDescriptor,
                     0,
                     sizeof( BMUXlib_TS_TransportDescriptor )
                     );

            /* Set ESCR */
            pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid = true;
            pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp = pSystemDataInfo->uiESCR;

            /* Set Packet 2 Packet Timestamp Delta */
            {
               pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid = true;
               pstCurrentTransportDescriptor->stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta = uiPacket2PacketTimestampDelta;
            }

            /* Set Buffer Info */
            pstCurrentTransportDescriptor->pBufferAddress = (void *) ((uint32_t) pstSystemDataBuffer->pData + pSystemDataInfo->uiPacketsMuxedSoFar * BMUXlib_TS_P_TSPacket_MAXSIZE);
            pstCurrentTransportDescriptor->uiBufferLength = uiPacketsToMux * BMUXlib_TS_P_TSPacket_MAXSIZE;

            BERR_TRACE( BMUXlib_TS_P_AddTransportDescriptor( hMuxTS, hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex, pstCurrentTransportDescriptor ) );

            /* Compute the ESCR of the next system data packet */
            /* coverity[check_after_deref] */
            if ( NULL != pstCurrentTransportDescriptor )
            {
               pSystemDataInfo->uiESCR += ( uiPacket2PacketTimestampDelta * uiPacketsToMux );
            }
            BDBG_ASSERT((int32_t)((uint32_t)(hMuxTS->status.stPCRInfo.uiNextESCR & 0xFFFFFFFF) - pSystemDataInfo->uiESCR) >= 0);

            /* Bookkeeping */
            pSystemDataInfo->uiPacketsMuxedSoFar += uiPacketsToMux;
            pSystemDataInfo->uiPacketsUntilNextPCR -= uiPacketsToMux;
            if ( pSystemDataInfo->uiPacketsMuxedSoFar == ( pstSystemDataBuffer->uiSize / BMUXlib_TS_P_TSPacket_MAXSIZE ) )
            {
               BERR_TRACE( BMUXlib_List_Remove(
                        hMuxTS->hSystemDataPendingList,
                        (void**) &pstSystemDataBuffer
                        ) );
               pSystemDataInfo->uiPacketsMuxedSoFar = 0;
               pstCurrentTransportDescriptorMetaData->uiSourceDescriptorCount = 1;
            }
         } /* end: while system data to schedule */
         /* Set next ESCR and calculate relevant PCRBase/Extension */
         hMuxTS->status.stPCRInfo.uiESCR = hMuxTS->status.stPCRInfo.uiNextESCR;
         hMuxTS->status.stPCRInfo.uiBase = ((((hMuxTS->status.stPCRInfo.uiESCR / 300) >> 32) & 0x1 ) << 32 ) | ((hMuxTS->status.stPCRInfo.uiESCR / 300) & 0xFFFFFFFF);
         hMuxTS->status.stPCRInfo.uiExtension = (hMuxTS->status.stPCRInfo.uiESCR % 300) & 0x1FF;
      } /* end: while PCRs to insert */
   } /* end: if PCR and last pending ESCR valid */

   BDBG_LEAVE( BMUXlib_TS_P_ProcessSystemData );

   return BERR_TRACE( BERR_SUCCESS );
}

void
BMUXlib_TS_S_StallTransport(
   BMUXlib_TS_Handle hMuxTS,
   unsigned uiTransportChannelIndex,
   bool bStalled
   )
{
#if 0
   if ( false == hMuxTS->status.stStartSettings.bNonRealTimeMode )
   {
      if ( bStalled != hMuxTS->status.bTransportChannelStalled[uiTransportChannelIndex] )
      {
         int32_t iAmountPending = ((int32_t) ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] - hMuxTS->status.stTransportStatus.uiESCR )) / 27000;
         if ( true == hMuxTS->status.bTransportConfigured )
         {
            /* Only print un-stall message if stall message was previously printed.
             * Only print stall message if less than mux delay worth of data is queued.
             */
            if ( ( ( false == bStalled )
                   && ( true == hMuxTS->status.bTransportChannelStalledPrintMessage[uiTransportChannelIndex] ) )
                 || ( ( true == bStalled )
                      && ( iAmountPending < (int32_t) hMuxTS->status.stStartSettings.uiServiceLatencyTolerance ) ) )
            {
               BDBG_ERR(("TRANSPORT[%d] %s (%d ms)", uiTransportChannelIndex, bStalled ? "Stalled" : "Un-Stalled", iAmountPending));
               hMuxTS->status.bTransportChannelStalledPrintMessage[uiTransportChannelIndex] = bStalled;
            }
         }
      }
   }
#endif

   hMuxTS->status.bTransportChannelStalled[uiTransportChannelIndex] = bStalled;
}

BERR_Code
BMUXlib_TS_P_ProcessNewBuffers(
   BMUXlib_TS_Handle hMuxTS
   )
{
   BMUXlib_Input_Handle hSelectedInput = NULL;
   uint32_t uiInputIndex;
   BMUXlib_InputGroup_Status stStatus;
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BMUXlib_TS_P_ProcessNewBuffers );

   /* Reset all inputs to be available if resources are available */
   for ( uiInputIndex = 0; uiInputIndex < hMuxTS->status.uiNumInputs; uiInputIndex++ )
   {
      if ( false == hMuxTS->status.stInputMetaData[uiInputIndex].bEOSSeen )
      {
         uint32_t uiTransportChannelIndex = hMuxTS->status.stInputMetaData[uiInputIndex].uiTransportChannelIndex;

         if ( true == BMUXlib_List_IsFull( hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex] ) )
         {
            /* disable this input (do not consider this input for selection until the next MSP) */
            BMUXlib_TS_S_StallTransport( hMuxTS, uiTransportChannelIndex, true );
            rc = BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hMuxTS->status.stInputMetaData[uiInputIndex].hInput, false);
            if (rc != BERR_SUCCESS)
               /* this should always succeed */
               return rc;
         }
         else
         {
            /* enable this input */
            BMUXlib_TS_S_StallTransport( hMuxTS, uiTransportChannelIndex, false );
            rc = BMUXlib_Input_ProcessNewDescriptors( hMuxTS->status.stInputMetaData[uiInputIndex].hInput );
            if (rc != BERR_SUCCESS)
               return rc;
            rc = BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hMuxTS->status.stInputMetaData[uiInputIndex].hInput, true);
            if (rc != BERR_SUCCESS)
               /* this should always succeed */
               return rc;
         }
      }
   }

   while ( false == BMUXlib_List_IsEmpty( /* We have transport descriptors available */
                  hMuxTS->hTransportDescriptorFreeList
                  ) )
   {
      BMUXlib_InputGroup_GetStatus(
         hMuxTS->status.hInputGroup,
         &stStatus);

      if ( 0 == stStatus.uiNumActiveInputs )
      {
         break;
      }


      /* In NRT mode, on startup, we want to wait for all inputs to have data to prevent starting an input prematurely
       * and then getting data on the other input whose transmission time is in the past */
      {
         BMUXlib_InputGroup_Settings stSettings;
         BMUXlib_InputGroup_GetSettings(hMuxTS->status.hInputGroup, &stSettings);

         if ( ( true == hMuxTS->status.stStartSettings.bNonRealTimeMode )
              && ( false == hMuxTS->status.bAllInputsReady ) )
         {
            stSettings.bWaitForAllInputs = true;

         }
         else
         {
            stSettings.bWaitForAllInputs = false;
         }
         rc = BMUXlib_InputGroup_SetSettings(hMuxTS->status.hInputGroup, &stSettings);
         if (rc != BERR_SUCCESS)
            return rc;

         BMUXlib_InputGroup_GetNextInput(hMuxTS->status.hInputGroup, &hSelectedInput);

         if ( ( true == stSettings.bWaitForAllInputs )
              && ( NULL != hSelectedInput ) )
         {
            hMuxTS->status.bAllInputsReady = true;
         }
      }

      if ( NULL != hSelectedInput )
      {
         BMUXlib_Input_Descriptor stDescriptor;
         BMUXlib_TS_P_InputMetaData *pInputMetadata;
         /* don't process userdata until determined otherwise
            (i.e. only on video and only on start of frame) */
         bool bProcessUserdata = false;
         bool bDescAvail;
         {
            BMUXlib_Input_CreateSettings stSettings;

            /* fetch the source metadata pointer for this input */
            BMUXlib_Input_GetCreateSettings(hSelectedInput, &stSettings);
            pInputMetadata = (BMUXlib_TS_P_InputMetaData *)stSettings.pMetadata;
            BDBG_ASSERT(pInputMetadata != NULL);
         }
         /* NOTE: The following should not fail, since an input would not be selected if there
            is no descriptor available */
         bDescAvail = BMUXlib_Input_PeekAtNextDescriptor( hSelectedInput, &stDescriptor );
         BDBG_ASSERT(bDescAvail == true);

         /* We want to initialize the first PCR value and ESCR based on the first A/V buffer we're muxing */
         if ( BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( &stDescriptor ) )
         {
            if ( false == hMuxTS->status.stPCRInfo.bInitialized )
            {
               unsigned uiIndex;
               bool bSTCSnapshotSmallestValid = false;
               uint64_t uiSTCSnapshotSmallest = 0;

               /* Check to see if STC snapshot exists in the input metadata */
               for ( uiIndex = 0; uiIndex < hMuxTS->status.uiNumInputs; uiIndex++ )
               {
                  if ( true == hMuxTS->status.stInputMetaData[uiIndex].bSTCSnapshotValid )
                  {
                     /* Pick smallest STC */
                     if ( false == bSTCSnapshotSmallestValid )
                     {
                        uiSTCSnapshotSmallest = hMuxTS->status.stInputMetaData[uiIndex].uiSTCSnapshot;
                        bSTCSnapshotSmallestValid = true;
                     }
                     else
                     {
                        uint32_t uiSTCSnapshotA = (uint32_t) (hMuxTS->status.stInputMetaData[uiIndex].uiSTCSnapshot >> 10);
                        uint32_t uiSTCSnapshotB = (uint32_t) (uiSTCSnapshotSmallest >> 10);

                        if ( ( ( uiSTCSnapshotA < uiSTCSnapshotB ) && ( ( uiSTCSnapshotB - uiSTCSnapshotA ) < 0x80000000 ) )
                             || ( ( uiSTCSnapshotA > uiSTCSnapshotB ) && ( ( uiSTCSnapshotA - uiSTCSnapshotB ) > 0x80000000 ) ) )
                        {
                           uiSTCSnapshotSmallest = hMuxTS->status.stInputMetaData[uiIndex].uiSTCSnapshot;
                        }
                     }
                  }
               }

               if ( true == bSTCSnapshotSmallestValid )
               {
                  hMuxTS->status.stPCRInfo.uiESCR = uiSTCSnapshotSmallest;
                  BDBG_MODULE_MSG(BMUXLIB_TS_PCR, ("Using STC Snapshot: %03x%08x", (uint32_t) (hMuxTS->status.stPCRInfo.uiESCR >> 32), (uint32_t) (hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF)));
               }
               else
               {
               hMuxTS->status.stPCRInfo.uiESCR = 0;
               hMuxTS->status.stPCRInfo.uiESCR |= (BMUXLIB_INPUT_DESCRIPTOR_ESCR( &stDescriptor ) & 0xFFFFFFFF);
                  BDBG_MODULE_MSG(BMUXLIB_TS_PCR, ("Using first ESCR: %03x%08x", (uint32_t) (hMuxTS->status.stPCRInfo.uiESCR >> 32), (uint32_t) (hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF)));
               }

               /* Handle MOD300 wrap */
               hMuxTS->status.stPCRInfo.uiESCR = (((hMuxTS->status.stPCRInfo.uiESCR >> 32) % 0x258) << 32) | (hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF);

               hMuxTS->status.stPCRInfo.uiBase = ((((hMuxTS->status.stPCRInfo.uiESCR / 300) >> 32) & 0x1 ) << 32 ) | ((hMuxTS->status.stPCRInfo.uiESCR / 300) & 0xFFFFFFFF);
               hMuxTS->status.stPCRInfo.uiExtension = (hMuxTS->status.stPCRInfo.uiESCR % 300) & 0x1FF;

               hMuxTS->status.stPCRInfo.bInitialized = true;

               BDBG_MODULE_MSG(BMUXLIB_TS_PCR, ("Initial PCR Base/Extension: %09llx/%02x", hMuxTS->status.stPCRInfo.uiBase, hMuxTS->status.stPCRInfo.uiExtension));
            }

            if ( false == hMuxTS->status.bFirstESCRValid )
            {
               /* TODO: We should probably wait for ALL inputs to ensure the smallest ESCR is chosen */
               hMuxTS->status.uiFirstESCR = BMUXLIB_INPUT_DESCRIPTOR_ESCR( &stDescriptor );
               hMuxTS->status.bFirstESCRValid = true;
            }
         } /* end: if ESCR valid */

            /* SW7425-75: Ignore metadata */
            if ( BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA( &stDescriptor ) )
            {
               /* extract video frame rate code for userdata timing */
               if (BMUXlib_Input_Type_eVideo == BMUXLIB_INPUT_DESCRIPTOR_TYPE(&stDescriptor))
               {
                  /* ensure the metatdata base address is valid, if not, ignore this descriptor */
                  if (NULL == stDescriptor.pBaseAddress)
                  {
                     BDBG_WRN(("Metadata base address not set: Ignoring video metadata descriptor ..."));
                  }
                  else
                  {
                     /* read the data unit type to determine if we can process this metadata type */
                     if (BAVC_VideoMetadataType_eCommon == BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(&stDescriptor))
                     {
                        BAVC_VideoMetadataDescriptor *pMetadata = (BAVC_VideoMetadataDescriptor *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(&stDescriptor);

                        pInputMetadata->eSourceFrameRateCode = (0 != (pMetadata->uiMetadataFlags & BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_FRAMERATE_VALID))?
                           pMetadata->stFrameRate.eFrameRateCode: BAVC_FrameRateCode_eUnknown;

                        if ( 0 != (pMetadata->uiMetadataFlags & BAVC_VIDEOMETADATADESCRIPTOR_FLAGS_TIMING_STC_VALID) )
                        {
                           pInputMetadata->bSTCSnapshotValid = true;
                           pInputMetadata->uiSTCSnapshot = pMetadata->stTiming.uiSTCSnapshot;
                           BDBG_MODULE_MSG(BMUXLIB_TS_PCR, ("Video STC Snapshot[%d]: %03x%08x", pInputMetadata->uiInputIndex, (uint32_t) (pInputMetadata->uiSTCSnapshot >> 32), (uint32_t) (pInputMetadata->uiSTCSnapshot & 0xFFFFFFFF)));
                        }
                        else
                        {
                           pInputMetadata->bSTCSnapshotValid = false;
                        }
                     }
                     /* else: ignore unrecognized metadata type */
                  }
               } /* end: video metadata */
               else if ( BMUXlib_Input_Type_eAudio == BMUXLIB_INPUT_DESCRIPTOR_TYPE(&stDescriptor) )
               {
                  /* ensure the metatdata base address is valid, if not, ignore this descriptor */
                  if (NULL == stDescriptor.pBaseAddress)
                  {
                     BDBG_WRN(("Metadata base address not set: Ignoring audio metadata descriptor ..."));
                  }
                  else
                  {
                     if ( BAVC_AudioMetadataType_eCommon == BMUXLIB_INPUT_DESCRIPTOR_AUDIO_DATA_UNIT_TYPE(&stDescriptor))
                     {
                        BAVC_AudioMetadataDescriptor *pMetadata = (BAVC_AudioMetadataDescriptor *)BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(&stDescriptor);

                        if ( 0 != (pMetadata->uiMetadataFlags & BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_TIMING_VALID) )
                        {
                           pInputMetadata->bSTCSnapshotValid = true;
                           pInputMetadata->uiSTCSnapshot = pMetadata->stTiming.uiSTCSnapshot;
                           BDBG_MODULE_MSG(BMUXLIB_TS_PCR, ("Audio STC Snapshot[%d]: %03x%08x", pInputMetadata->uiInputIndex, (uint32_t) (pInputMetadata->uiSTCSnapshot >> 32), (uint32_t) (pInputMetadata->uiSTCSnapshot & 0xFFFFFFFF)));
                        }
                        else
                        {
                           pInputMetadata->bSTCSnapshotValid = false;
                        }
                     }
                  }
               } /* end: audio metadata */
               pInputMetadata->uiMetadataDescriptorsSkipped++;
            }
            else
            {
               /* Determine if required resources are available to process this input descriptor */
                  unsigned uiNumTransportDescriptorsRequired = 0;
                  uint32_t uiNumTransportDescriptorsFree;
                  size_t uiSpaceAvailable;

                  if ( BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME( &stDescriptor ) )
                  {
                     /* Empty frames just need a NULL descriptor to be sent */
                     uiNumTransportDescriptorsRequired++;
                  }
                  else
                  {
                     if ( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stDescriptor ) )
                     {
                        /* Frame Start requires a PES Header and Transport Descriptor */
                        uiNumTransportDescriptorsRequired++;

                        if ( true == BMUXlib_List_IsEmpty(
                                      hMuxTS->hPESHeaderFreeList
                                      )
                           )
                        {
                           BDBG_MSG(("PES descriptor not available.  Will be processed next time."));
                           BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                           continue;
                        }
                     }

                     if ( 0 != BMUXLIB_INPUT_DESCRIPTOR_LENGTH( &stDescriptor ) )
                     {
                        /* Frame Data requires only a Transport Descriptor */
                        uiNumTransportDescriptorsRequired++;
                     }
                  }

                  if ( BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( &stDescriptor ) )
                  {
                     /* EOS requires a BPP (always available) and a Transport Descriptor */
                     uiNumTransportDescriptorsRequired++;
                  }


                  /* VERIFY at enough transport descriptors are available */
                  BMUXlib_List_GetNumEntries(
                           hMuxTS->hTransportDescriptorFreeList,
                           &uiNumTransportDescriptorsFree
                           );

                  if ( uiNumTransportDescriptorsFree < uiNumTransportDescriptorsRequired )
                  {
                     BDBG_MSG(("Transport descriptors not available.  Will be processed next time."));
                     BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                     continue;
                  }

                  BMUXlib_List_GetNumFree( hMuxTS->hTransportDescriptorPendingList[pInputMetadata->uiTransportChannelIndex], &uiSpaceAvailable );
                  if ( uiSpaceAvailable < uiNumTransportDescriptorsRequired )
                  {
                     BDBG_MSG(("Space not available in output.  Will be processed next time."));
                     BMUXlib_TS_S_StallTransport( hMuxTS, pInputMetadata->uiTransportChannelIndex, true );
                     BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                     continue;
                  }

               /* Check to see if we need to insert a PES Header */
               if ( BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME( &stDescriptor ) )
               {
                  BDBG_ASSERT( BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID( &stDescriptor ) );

                  /* Insert NULL */
                  /* Queue the null descriptor to flush the output */
                  if ( BERR_SUCCESS != BERR_TRACE(BMUXlib_TS_P_InsertNULLTransportDescriptor(
                           hMuxTS,
                           pInputMetadata->uiTransportChannelIndex,
                           BMUXLIB_INPUT_DESCRIPTOR_ESCR( &stDescriptor ),
                           BMUXlib_TS_P_DataType_eCDB,
                           BMUXLIB_TS_P_InputDescriptorTypeLUT[BMUXLIB_INPUT_DESCRIPTOR_TYPE( &stDescriptor )],
                           pInputMetadata->uiInputIndex
                           )))
                  {
                     BDBG_ERR(("Transport descriptors not available.  Will be processed next time."));
                     BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                     continue;
                  }
               }
               else
               {
                  if ( BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART( &stDescriptor ) )
                  {
                     BMUXlib_P_PESInfo stPESInfo;

                     BMUXlib_TS_P_PopulatePESInfoFromInputDescriptor(
                              &stDescriptor,
                              &stPESInfo
                              );

                     stPESInfo.uiTransportChannelIndex = pInputMetadata->uiTransportChannelIndex;
                     stPESInfo.bTransportChannelIndexValid = true;

                     stPESInfo.uiPESStreamID = pInputMetadata->uiPESStreamID;

                     /* Check to see if the following resources are available for inserting the PES header
                      *    1) Transport Descriptor
                      *    2) PES Header
                      */
                     if ( BERR_SUCCESS != BMUXlib_TS_P_InsertPESHeader(
                              hMuxTS,
                              &stPESInfo
                              )
                        )
                     {
                        BDBG_ERR(("PES descriptors not available.  Will be processed next time."));
                        BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                        continue;
                     }

                     bProcessUserdata = BMUXlib_TS_P_Userdata_FindTargetPTS(hMuxTS, pInputMetadata, &stDescriptor);

                     if ( ( true == stPESInfo.bESCRValid )
                          && ( true == stPESInfo.bTicksPerBitValid )
                          && ( true == stPESInfo.bSHRValid )
                        )
                     {
                        /* ESCR Discontinuity Detection */
                        {
                           unsigned uiESCRDelta;

                           if ( hMuxTS->status.uiTransportNextExpectedESCR[pInputMetadata->uiTransportChannelIndex] > stPESInfo.uiESCR )
                           {
                              uiESCRDelta = hMuxTS->status.uiTransportNextExpectedESCR[pInputMetadata->uiTransportChannelIndex] - stPESInfo.uiESCR;
                           }
                           else
                           {
                              uiESCRDelta = stPESInfo.uiESCR - hMuxTS->status.uiTransportNextExpectedESCR[pInputMetadata->uiTransportChannelIndex];
                           }

                           if ( ( 0xFFFFFFFF != hMuxTS->status.uiTransportNextExpectedESCR[pInputMetadata->uiTransportChannelIndex] )
                                 && ( uiESCRDelta > 27000000 * 5 )
                                 && ( uiESCRDelta < ( 0xFFFFFFFF - (27000000 * 5 ) ) ) )
                           {
                              BDBG_WRN(("ESCR Discontinuity Detected on Transport[%d]. (eESCR=%08x, aESCR=%08x, delta=%u (%u seconds)",
                                 pInputMetadata->uiTransportChannelIndex,
                                 hMuxTS->status.uiTransportNextExpectedESCR[pInputMetadata->uiTransportChannelIndex],
                                 stPESInfo.uiESCR,
                                 uiESCRDelta,
                                 uiESCRDelta / 27000000
                                 ));
                           }
                        }
                     }
                  }

                  /* Check to see if there is CDB data to mux */
                  if ( 0 != BMUXLIB_INPUT_DESCRIPTOR_LENGTH( &stDescriptor ) )
                  {
                     BMUXlib_P_TransportInfo stTransportInfo;

                     BMUXlib_TS_P_PopulateTransportInfoFromInputDescriptor(
                              &stDescriptor,
                              &stTransportInfo
                              );

                     stTransportInfo.uiTransportChannelIndex = pInputMetadata->uiTransportChannelIndex;
                     stTransportInfo.bTransportChannelIndexValid = true;

                     stTransportInfo.uiSkippedDescriptors = pInputMetadata->uiMetadataDescriptorsSkipped;
                     stTransportInfo.bSkippedDescriptorsValid = true;

                     stTransportInfo.uiInputIndex = pInputMetadata->uiInputIndex;
                     stTransportInfo.bInputIndexValid = true;

                     if ( BERR_SUCCESS != BMUXlib_TS_P_InsertTransportDescriptor(
                              hMuxTS,
                              &stTransportInfo
                              )
                        )
                     {
                        BDBG_ERR(("Transport descriptors not available!"));
                        BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                        continue;
                     }

                     pInputMetadata->uiMetadataDescriptorsSkipped = 0;
                  }
               }

               if ( BMUXLIB_INPUT_DESCRIPTOR_IS_EOS( &stDescriptor ) )
               {
                  /* Insert LAST BPP */
                  BMUXlib_P_BPPInfo stBPPInfo;

                  BKNI_Memset( &stBPPInfo, 0, sizeof( BMUXlib_P_BPPInfo ) );

                  stBPPInfo.uiTransportChannelIndex = pInputMetadata->uiTransportChannelIndex;
                  stBPPInfo.bTransportChannelIndexValid = true;

                  stBPPInfo.eSourceType = BMUXLIB_TS_P_InputDescriptorTypeLUT[BMUXLIB_INPUT_DESCRIPTOR_TYPE( &stDescriptor )];
                  stBPPInfo.bSourceTypeValid = true;

                  stBPPInfo.uiSkippedDescriptors = 1;
                  stBPPInfo.bSkippedDescriptorsValid = true;

                  stBPPInfo.uiInputIndex = pInputMetadata->uiInputIndex;
                  stBPPInfo.bInputIndexValid = true;

                  stBPPInfo.pBuffer = pInputMetadata->pBPPBuffer;
                  stBPPInfo.uiLength = sizeof ( BMUXlib_TS_P_BPPData );

                  if ( BERR_SUCCESS != BMUXlib_TS_P_InsertBPP(
                           hMuxTS,
                           &stBPPInfo
                           )
                     )
                  {
                     BDBG_ERR(("Transport descriptors not available.  Will be processed next time."));
                     BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
                     continue;
                  }

               BDBG_MODULE_MSG( BMUXLIB_TS_EOS, ("%s[%d] EOS Seen",
                     ( BMUXLIB_INPUT_DESCRIPTOR_TYPE( &stDescriptor ) == BMUXlib_Input_Type_eAudio ) ? "AUDIO" : "VIDEO",
                        pInputMetadata->uiInputIndex));

                  pInputMetadata->bEOSSeen = true;
                  BMUXlib_InputGroup_ActivateInput(hMuxTS->status.hInputGroup, hSelectedInput, false);
               }
         } /* end: else not metadata */

         if (bProcessUserdata)
            if (BERR_SUCCESS != BMUXlib_TS_P_Userdata_ProcessInputs( hMuxTS ))
               BDBG_ERR(("Unable to process userdata"));

         /* Process System Data (Generate PCR and schedule system data)
            Also schedules userdata */
         BMUXlib_TS_P_ProcessSystemData( hMuxTS );

         /* discard the descriptor that was "peeked" at previously
            (this should not fail) */
         bDescAvail = BMUXlib_Input_GetNextDescriptor( hSelectedInput, &stDescriptor );
         BDBG_ASSERT(bDescAvail == true);
      } /* end: if valid input selected */
      else
      {
         /* no input ready to process */
         break;
      }
   } /* end: while descriptors to process */

   /* TODO: Add BPP "TIMESTAMP" Packet support */

   BDBG_LEAVE( BMUXlib_TS_P_ProcessNewBuffers );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_P_ScheduleProcessedBuffers(
   BMUXlib_TS_Handle hMuxTS
   )
{
   uint32_t uiTransportChannelIndex;

   BDBG_ENTER( BMUXlib_TS_P_ScheduleProcessedBuffers );

   for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
   {
      BMUXlib_TS_TransportChannelInterface *pTransportInterface = &hMuxTS->status.stStartSettings.transport.stChannelInterface[uiTransportChannelIndex];

      if ( ( NULL != pTransportInterface->pContext )
           && ( NULL != pTransportInterface->fAddTransportDescriptors )
         )
      {
         const BMUXlib_TS_TransportDescriptor **astTransportDescriptors[2];
         const void *pDescriptors[2];
         size_t uiNumTransportDescriptors[2];
         uint32_t uiTotalQueued = 0;
         uint32_t i;

         BMUXlib_List_GetEntries(
                  hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex],
                  &pDescriptors[0],
                  &uiNumTransportDescriptors[0],
                  &pDescriptors[1],
                  &uiNumTransportDescriptors[1]
                  );

         astTransportDescriptors[0] = (const BMUXlib_TS_TransportDescriptor **) pDescriptors[0];
         astTransportDescriptors[1] = (const BMUXlib_TS_TransportDescriptor **) pDescriptors[1];

         /* Ignore the descriptors that are queued */
         {
            uint32_t uiQueuedPreviously = hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].uiTransportDescriptorsQueued;

            for ( i = 0; ( i < 2 ) && ( 0 != uiQueuedPreviously ) ; i++ )
            {
               if ( uiQueuedPreviously < uiNumTransportDescriptors[i] )
               {
                  astTransportDescriptors[i] += uiQueuedPreviously;
                  uiNumTransportDescriptors[i] -= uiQueuedPreviously;
                  uiQueuedPreviously -= uiQueuedPreviously;
               }
               else
               {
                  astTransportDescriptors[i] = NULL;
                  uiQueuedPreviously -= uiNumTransportDescriptors[i];
                  uiNumTransportDescriptors[i] = 0;
               }
            }

            if (0 != uiQueuedPreviously)
            {
               BDBG_ERR(("Mismatch between queued transport descriptors and total descriptors"));
            }
         }

         {
            unsigned uiTotalDescriptorsToQueue = 0;
            unsigned j;

            for ( i = 0; i < 2; i++ )
            {
               for ( j = 0; j < uiNumTransportDescriptors[i]; j++)
               {
                  hMuxTS->astTransportDescriptorTemp[uiTotalDescriptorsToQueue] = *astTransportDescriptors[i][j];
                  uiTotalDescriptorsToQueue++;
               }
            }

            if ( 0 != uiTotalDescriptorsToQueue )
            {
               pTransportInterface->fAddTransportDescriptors(
                                       pTransportInterface->pContext,
                                       hMuxTS->astTransportDescriptorTemp,
                                       uiTotalDescriptorsToQueue,
                                       &uiTotalQueued
                                       );
            }

            hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].uiTransportDescriptorsQueued += uiTotalQueued;
         }

         if ( 0 != uiTotalQueued )
         {
            bool bLateMessagePrinted = false;

            for ( i = 0; i < uiTotalQueued; i++ )
            {
               hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].uiTransportDescriptorsSent++;

               if ( true == hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid )
               {
                  uint32_t uiDelta;
                  if ( hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp < hMuxTS->status.stTransportStatus.uiESCR )
                  {
                     uiDelta = ( (0xFFFFFFFF - hMuxTS->status.stTransportStatus.uiESCR) + 1) + hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp;
                  }
                  else
                  {
                     uiDelta = hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp - hMuxTS->status.stTransportStatus.uiESCR;
                  }

                  if ( uiDelta >= 0x80000000 )
                  {
                     if ( false == bLateMessagePrinted )
                     {
                        BDBG_MODULE_MSG( BMUXLIB_TS_OUTPUT_DESC_ERROR, ("TRANSPORT[%d] Late ESCR Detected (PACING=%08x > ESCR=%08x) [%d ms]",
                                 uiTransportChannelIndex,
                                 hMuxTS->status.stTransportStatus.uiESCR,
                                 hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp,
                                 -(( hMuxTS->status.stTransportStatus.uiESCR - hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp ) / 27000)
                                 ));

                        bLateMessagePrinted = true;
                     }
                  }
#if 0
                  else
                  {
                     BDBG_MODULE_ERR( BMUXLIB_TS_OUTPUT_DESC, ("TRANSPORT[%d] OK ESCR Detected (PACING=%08x < ESCR=%08x)",
                              uiTransportChannelIndex,
                              hMuxTS->status.stTransportStatus.uiESCR,
                              hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp
                              ));
                  }
#endif
               }

               BDBG_MODULE_MSG( BMUXLIB_TS_OUTPUT_DESC, ("TRANSPORT[%d] @%08x (%08x), ts=%08x (%d), delta=%08x (%d), RAI=%d",
                        uiTransportChannelIndex,
                        hMuxTS->astTransportDescriptorTemp[i].pBufferAddress,
                        hMuxTS->astTransportDescriptorTemp[i].uiBufferLength,
                        hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp,
                        hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid,
                        hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta,
                        hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid,
                        hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bRandomAccessIndication
               ));

#if BMUXLIB_TS_P_DUMP_TRANSPORT_DESC
               if ( NULL == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hDescDumpFile )
               {
                  char fname[256];
                  sprintf(fname, "BMUXlib_TRANSPORT_DESC_%02d.csv", uiTransportChannelIndex);
                  hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hDescDumpFile = fopen(fname, "wb");
                  if ( NULL == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hDescDumpFile )
                  {
                     BDBG_ERR(("Error Creating Transport Descriptor Dump File (%s)", fname));
                  }
               }

               if ( NULL != hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hDescDumpFile )
               {
                  if ( false == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].bPrintedHeader )
                  {
                     fprintf( hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hDescDumpFile,
                              "channel,count,pacing(27Mhz),address,length,escr(27Mhz),escr valid,pkt2pkt,pk2pkt valid,rai\n"
                              );

                     hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].bPrintedHeader = true;
                  }

                  fprintf( hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hDescDumpFile,
                           "%10u,%10u,%10u,%p,%10u,%10u,%d,%10u,%d,%d\n",
                           uiTransportChannelIndex,
                           hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].uiTransportDescriptorsSent,
                           hMuxTS->status.stTransportStatus.uiESCR,
                           hMuxTS->astTransportDescriptorTemp[i].pBufferAddress,
                           hMuxTS->astTransportDescriptorTemp[i].uiBufferLength,
                           hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiNextPacketPacingTimestamp,
                           hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bNextPacketPacingTimestampValid,
                           hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta,
                           hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid,
                           hMuxTS->astTransportDescriptorTemp[i].stTsMuxDescriptorConfig.bRandomAccessIndication
                           );
               }
#endif
#if BMUXLIB_TS_P_DUMP_TRANSPORT_PES
               if ( NULL == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hPESDumpFile )
               {
                  char fname[256];
                  sprintf(fname, "BMUXlib_TRANSPORT_%02d.%s", uiTransportChannelIndex, ( uiTransportChannelIndex == hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex ) ? "ts" : "pes");
                  hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hPESDumpFile = fopen(fname, "wb");
                  if ( NULL == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hPESDumpFile )
                  {
                     BDBG_ERR(("Error Creating Transport PES/TS Dump File (%s)", fname));
                  }
               }

               if ( NULL != hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hPESDumpFile )
               {
                  fwrite( hMuxTS->astTransportDescriptorTemp[i].pBufferAddress,
                           1,
                           hMuxTS->astTransportDescriptorTemp[i].uiBufferLength,
                           hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].hPESDumpFile
                           );
               }
#endif
            }
            BDBG_MSG(("TRANSPORT[%d] <-- %d", uiTransportChannelIndex,  uiTotalQueued));
         }
      }
   }

   BDBG_LEAVE( BMUXlib_TS_P_ScheduleProcessedBuffers );

   return BERR_TRACE( BERR_SUCCESS );
}

/* BMUXlib_TS_P_Flush -
 * inserts a dummy transport descriptor on each channel that has an ESCR equal
 * to the largest most recent ESCR on each channel */
bool
BMUXlib_TS_P_Flush(
         BMUXlib_TS_Handle hMuxTS,
         bool bFlushSystemData
         )
{
   BMUXlib_InputGroup_Status stStatus;
   uint32_t uiTransportChannelIndexWithLargestESCR = 0;
   unsigned uiLastPendingESCR = 0;
   bool bLastPendingESCRValid = false;

   BDBG_ENTER( BMUXlib_TS_P_Flush );

   {
      uint32_t uiTransportChannelIndex;

      /* Determine the largest ESCR to use for flushing the outputs */
      for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
      {
         if ( ( true == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].bActive )
              && ( false == BMUXlib_List_IsEmpty( hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex] ) )
              && ( ( ( ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] > uiLastPendingESCR )
                       && ( ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] - uiLastPendingESCR ) < 0x80000000 ) )
                     || ( ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] < uiLastPendingESCR )
                          && ( ( uiLastPendingESCR - hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] ) > 0x80000000 ) )
                   )
                   || ( false == bLastPendingESCRValid )
                 )
            )
         {
            uiTransportChannelIndexWithLargestESCR = uiTransportChannelIndex;
            uiLastPendingESCR = hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex];
            bLastPendingESCRValid = true;
         }
      }

      if ( false == bLastPendingESCRValid )
      {
         BDBG_MODULE_MSG( BMUXLIB_TS_EOS, ("No pending descriptors, so flush not needed") );
         return true;
      }

      if ( ( ( hMuxTS->status.stTransportStatus.uiESCR > uiLastPendingESCR )
             && ( ( hMuxTS->status.stTransportStatus.uiESCR - uiLastPendingESCR ) < 0x80000000 ) )
           || ( ( hMuxTS->status.stTransportStatus.uiESCR < uiLastPendingESCR )
                && ( ( uiLastPendingESCR - hMuxTS->status.stTransportStatus.uiESCR ) > 0x80000000 ) ) )
      {
         BDBG_MODULE_MSG( BMUXLIB_TS_EOS, ("Current ESCR is larger than the last pending ESCR, so skipping NULL"));
         return true;
      }

      BMUXlib_InputGroup_GetStatus(hMuxTS->status.hInputGroup, &stStatus);

      /* Process each transport channel */
      for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
      {
         if ( ( 0 == stStatus.uiNumActiveInputs ) /* Process all transport channels if all inputs are done */
              || ( ( uiTransportChannelIndex != uiTransportChannelIndexWithLargestESCR ) /* Do not process the transport channel with the largest ESCR */
                 && ( ( uiTransportChannelIndex != hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex ) /* Do not process the system data transport channel */
                      || ( true == bFlushSystemData ) ) ) /* but DO process the system data channel it if finishing input */
            )
         {
            if ( true == hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].bActive ) /* Process transport channels that are active */
            {
               bool bAllInputsForTransportChannelHasSeenEOS = false;

               unsigned uiIndex;

               /* Check if EOS seen on all input channels associated with this Transport Channel */
               for ( uiIndex = 0; uiIndex < hMuxTS->status.uiNumInputs ; uiIndex++ )
               {
                  if ( uiTransportChannelIndex == hMuxTS->status.stInputMetaData[uiIndex].uiTransportChannelIndex )
                  {
                     bAllInputsForTransportChannelHasSeenEOS = hMuxTS->status.stInputMetaData[uiIndex].bEOSSeen;

                     if ( false == bAllInputsForTransportChannelHasSeenEOS )
                     {
                        break;
                     }
                  }
               }

               if ( ( uiTransportChannelIndex == hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex )
                    && ( true == bFlushSystemData ) )
               {
                  bAllInputsForTransportChannelHasSeenEOS = true;
               }

               if ( true == bAllInputsForTransportChannelHasSeenEOS )
               {
                  if ( ( true == BMUXlib_List_IsEmpty( hMuxTS->hTransportDescriptorFreeList ) ) /* There is an available descriptor */
                       || ( true == BMUXlib_List_IsFull( hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex]) ) /* There is room to insert the entry */
                     )
                  {
                     BDBG_MODULE_WRN( BMUXLIB_TS_EOS, ("Not enough resources to insert NULL entry on transport[%d]", uiTransportChannelIndex));
                     return false;
                  }
                  else
                  {
                     BERR_Code rc;

                     /* Queue the null descriptor to flush the output */
                     rc = BERR_TRACE(BMUXlib_TS_P_InsertNULLTransportDescriptor(
                              hMuxTS,
                              uiTransportChannelIndex,
                              uiLastPendingESCR,
                              BMUXlib_TS_P_DataType_eNULL,
                              BMUXlib_TS_P_SourceType_eUnknown,
                              0
                              ));

                     BDBG_MODULE_MSG( BMUXLIB_TS_EOS, ("Inserting NULL descriptor (escr=%08x) on transport[%d]", uiLastPendingESCR, uiTransportChannelIndex));

                     BDBG_ASSERT( BERR_SUCCESS == rc );
                  }
               }
            }
         }
      }
   }

   BDBG_LEAVE( BMUXlib_TS_P_Flush );

   return true;
}
