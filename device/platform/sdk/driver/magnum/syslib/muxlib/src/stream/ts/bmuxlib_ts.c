/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_ts.c $
 * $brcm_Revision: Hydra_Software_Devel/93 $
 * $brcm_Date: 10/26/12 11:20a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/src/stream/ts/bmuxlib_ts.c $
 * 
 * Hydra_Software_Devel/93   10/26/12 11:20a nilesh
 * SW7435-442: Use parent heap for converting uncached memory (allocated
 * from sub-heap) to cached
 * 
 * Hydra_Software_Devel/92   9/25/12 10:40a delkert
 * SW7425-3958: Move relocation of system data from Pre-Q to after
 * ResetResources() call to prevent overwrite of pre-Q.
 * Add Mutex to Start() and AddSystemDataBuffers() to prevent corruption
 * of pending list during move.
 * Make a private version of AddSystemDataBuffers() without mutex for use
 * during the move (from within Start API)
 * Fix bug that incremented write offset if add to list failed.
 *
 * Hydra_Software_Devel/91   9/11/12 2:07p nilesh
 * SW7425-999: Fixed NULL packet insertion at EOS
 *
 * Hydra_Software_Devel/90   8/15/12 12:41p nilesh
 * SW7425-3684: Fixed assert when hInputGroup is NULL
 *
 * Hydra_Software_Devel/89   8/14/12 1:10p nilesh
 * SW7425-3684: Added support for reporting mux duration
 *
 * Hydra_Software_Devel/88   8/6/12 4:52p delkert
 * SW7425-2825: Add important comment regarding handling of
 * system/userdata upon finish.
 *
 * Hydra_Software_Devel/87   8/1/12 5:18p delkert
 * SW7425-3642: Remove redundant List_Destroy calls. Fix release-mode
 * compile warning. Implicitly stop mux during Destroy() if not already
 * stopped. Accumulate Transport Descriptor list counts per channel and
 * allocate once only.
 *
 * Hydra_Software_Devel/86   7/20/12 3:31p delkert
 * SW7425-3213: Resolve invalid context modification issues upon failed
 * Start()
 *
 * Hydra_Software_Devel/85   7/18/12 1:02p nilesh
 * SW7425-3076: Updated Memory API
 *
 * Hydra_Software_Devel/84   7/6/12 5:40p delkert
 * SW7425-3213: Fix bug in GetDefaultMemorySize() (mux settings were
 * overwritten). Fix checking for memory size of zero in
 * AllocateMemory(). Fix "out of memory" error checking in Start() to
 * prevent error code overwrite by a "no inputs" error.
 *
 * Hydra_Software_Devel/83   7/3/12 9:33a nilesh
 * SW7435-200: Added mutex logic for DoMux() and Stop()
 *
 * Hydra_Software_Devel/82   7/2/12 5:27p delkert
 * SW7425-3213: Fix bug where Destroy after start was segfaulting due to
 * free of unknown pointer
 *
 * Hydra_Software_Devel/81   7/2/12 10:21a nilesh
 * SW7425-3373: Fixed coverity error
 *
 * Hydra_Software_Devel/80   7/2/12 10:16a nilesh
 * SW7425-3373: Fixed coverity errors
 *
 * Hydra_Software_Devel/79   6/27/12 11:10a nilesh
 * SW7425-3076: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-3076/19   6/26/12 11:54a nilesh
 * SW7425-3076: Updated public API for providing memory buffer info
 *
 * Hydra_Software_Devel/SW7425-3076/18   6/25/12 11:25a nilesh
 * SW7425-3076: Always allocate at least 2 buffers of each type to allow
 * ping pong of buffers sent to transport to prevent buffer done
 * confusion when re-using a single buffer.
 *
 * Hydra_Software_Devel/SW7425-3076/17   6/20/12 1:17p nilesh
 * SW7425-3076: Rebase from mainline
 *
 * Hydra_Software_Devel/78   6/12/12 3:04p delkert
 * SW7425-3216: Add test code to dump system data
 *
 * Hydra_Software_Devel/SW7425-3076/16   6/20/12 12:30p nilesh
 * SW7425-3076: Updated memory config APIs
 *
 * Hydra_Software_Devel/SW7425-3076/15   6/20/12 11:59a nilesh
 * SW7425-3076: Use separate heaps for system and device memory
 *
 * Hydra_Software_Devel/SW7425-3076/14   6/19/12 1:35p nilesh
 * SW7425-3076: Change from using stack to fifo for free buffer queues
 *
 * Hydra_Software_Devel/SW7425-3076/13   6/14/12 4:10p nilesh
 * SW7425-3076: Added system data pre-Q support
 *
 * Hydra_Software_Devel/SW7425-3076/12   6/14/12 3:02p nilesh
 * SW7425-3076: Never allow MSP to go above the requested MSP
 *
 * Hydra_Software_Devel/SW7425-3076/11   6/14/12 2:06p nilesh
 * SW7425-3076: Added cached bmem support.  Userdata Release Q allocated
 * differently than userdata queue.  Account for worst case end-to-end
 * delay for userdata queue.
 *
 * Hydra_Software_Devel/SW7425-3076/10   6/7/12 4:44p nilesh
 * SW7425-3076: Add sub heap support
 *
 * Hydra_Software_Devel/SW7425-3076/9   6/6/12 5:06p nilesh
 * SW7425-3076: Added size calculations. Converted to using BMEM for
 * userdata.
 *
 * Hydra_Software_Devel/SW7425-3076/8   6/5/12 2:58p nilesh
 * SW7425-3076: Added BMUXlib_TS_GetMemoryConfig()
 *
 * Hydra_Software_Devel/SW7425-3076/7   6/5/12 10:24a nilesh
 * SW7425-3076: If underflowing, set MSP to service latency tolerance
 *
 * Hydra_Software_Devel/SW7425-3076/6   6/5/12 10:15a nilesh
 * SW7425-3076: Cleaned up warning debug prints
 *
 * Hydra_Software_Devel/SW7425-3076/5   6/5/12 10:00a nilesh
 * SW7425-3076: Warn if not enough memory allocated for requested start
 * settings
 *
 * Hydra_Software_Devel/SW7425-3076/4   6/5/12 9:47a nilesh
 * SW7425-3076: For default memory allocation, assume: 4x NRT, system data
 * bitrate = 2Mbps, and PCR interval = 20 ms
 *
 * Hydra_Software_Devel/SW7425-3076/3   6/5/12 9:14a nilesh
 * SW7425-3076: Properly limit the max descriptors a single channel can
 * use
 *
 * Hydra_Software_Devel/SW7425-3076/2   6/4/12 5:12p nilesh
 * SW7425-3076: Clean up
 *
 * Hydra_Software_Devel/SW7425-3076/1   6/4/12 1:44p nilesh
 * SW7425-3076: Reduced memory footprint.  Implemented variable MSP
 * algorithm to prevent underflow.
 *
 * Hydra_Software_Devel/76   4/25/12 10:42a nilesh
 * SW7425-2606: Reset start settings to default on Create/Stop
 *
 * Hydra_Software_Devel/75   4/24/12 5:19p nilesh
 * SW7425-2606: Added BMUXlib_TS_StartSettings.uiServicePeriod
 *
 * Hydra_Software_Devel/74   4/11/12 10:54a delkert
 * SW7425-2567: Made PCR interval > 100 warning only.
 *
 * Hydra_Software_Devel/73   4/5/12 2:12p delkert
 * SW7425-2567: Add verification of PCR settings (PID & interval) upon
 * Start().
 *
 * Hydra_Software_Devel/72   3/27/12 8:30a delkert
 * SW7425-2598: Refactor packet parser to better handle packets that do
 * not conform to ETSI EN 300 472
 *
 * Hydra_Software_Devel/71   2/29/12 3:44p nilesh
 * SW7425-2500: Fixed EOS detection
 *
 * Hydra_Software_Devel/70   2/22/12 12:56p delkert
 * SW7425-2217: Redefine data structures to better separate settings from
 * each userdata PID. Add *temporary* workaround to allow streams with
 * bad PTS to pass thru (for testing purposes only).
 *
 * Hydra_Software_Devel/69   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/5   2/15/12 11:42a delkert
 * SW7425-2217: Fix bug when freeing release Q free list.
 *
 * Hydra_Software_Devel/SW7425-2217/4   2/15/12 10:37a nilesh
 * SW7425-2217: Rebase from mainline
 *
 * Hydra_Software_Devel/68   2/8/12 4:34p nilesh
 * SW7425-2221,SW7425-2349: Removed BMUXlib_FinishMode_eImmediate
 *
 * Hydra_Software_Devel/67   2/8/12 4:19p nilesh
 * SW7425-2221,SW7425-2349: Allow calling BMUXlib_TS_Stop() w/o finish.
 * Behavior is the same as calling
 * BMUXlib_TS_Finish(eFinishMode=eImmediate) first.
 *
 * Hydra_Software_Devel/66   1/31/12 12:03p nilesh
 * SW7425-2221: Added BMUXlib_FinishMode_eImmediate support
 *
 * Hydra_Software_Devel/SW7425-2217/3   2/14/12 4:10p delkert
 * SW7425-2217: Remove obsolete code. Add error checking for userdata
 * inputs.
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/14/12 11:01a delkert
 * SW7425-2217: Move userdata code to separate files. Cleanup.
 *
 * Hydra_Software_Devel/SW7425-2217/1   2/13/12 9:11a delkert
 * SW7425-2217: Initial working userdata pass-thru
 *
 * Hydra_Software_Devel/65   1/20/12 4:43p delkert
 * SWCHECKFRAME-80: Initialize PID table to resolve coverity error.
 *
 * Hydra_Software_Devel/64   1/19/12 9:33a nilesh
 * SW7425-2174: Fixed parameter validation
 *
 * Hydra_Software_Devel/63   1/18/12 5:46p nilesh
 * SW7425-2174: Fixed stall detection
 *
 * Hydra_Software_Devel/62   1/9/12 4:11p nilesh
 * SW7425-999: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-999/2   1/9/12 3:52p nilesh
 * SW7425-999: In NRT mode, to prevent premature ESCR stall on shutdown,
 * always send dummy/NULL descriptors to transport channels whose inputs
 * have all seen an EOS.
 *
 * Hydra_Software_Devel/SW7425-999/1   1/9/12 11:44a nilesh
 * SW7425-999: Use BMUXlib_InputGroup status to determine EOS
 *
 * Hydra_Software_Devel/61   1/9/12 3:46p delkert
 * SW7425-2121: Modify input create settings to match changes to input API
 *
 * Hydra_Software_Devel/60   1/4/12 1:37p nilesh
 * SW7425-998: Added auto-finish support when EOS seen on all inputs
 *
 * Hydra_Software_Devel/59   12/20/11 1:45p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:44a delkert
 * SW7425-1869: Utilize InputGroup for input selection
 *
 * Hydra_Software_Devel/58   11/16/11 10:21a delkert
 * SWCHECKFRAME-114: Add accessor macros for SystemDataCompletedCount
 *
 * Hydra_Software_Devel/57   11/15/11 6:08p delkert
 * SWCHECKFRAME-114: Add private accessor macros to allow test code access
 * to mux state.  Modify mux code to use accessor macros
 *
 * Hydra_Software_Devel/56   11/15/11 4:54p nilesh
 * SW7425-1691: Reset stStartSettings and stFinishSettings on stop()
 *
 * Hydra_Software_Devel/55   11/10/11 5:15p nilesh
 * SW7425-1691: Added recovery logic for "failed Start" --> "Start"
 *
 * Hydra_Software_Devel/54   11/10/11 12:49p nilesh
 * SW7425-1691: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-1691/1   11/9/11 2:07p nilesh
 * SW7425-1691: Added support for stop-->start without requiring
 * destroy/create
 *
 * Hydra_Software_Devel/53   11/9/11 10:46a nilesh
 * SW7425-1707: Changed uiInstance/uiOutputInstance to uiMuxId
 *
 * Hydra_Software_Devel/52   11/9/11 10:01a nilesh
 * SW7425-1707: Added support for setting
 * BMUXlib_Input_CreateSettings.uiOutputInstance
 *
 * Hydra_Software_Devel/51   11/7/11 11:11a nilesh
 * SW7425-1236: Fixed debug log dump when there are multiple audio/video
 * inputs
 *
 * Hydra_Software_Devel/50   10/28/11 9:04a delkert
 * SW7425-1494: Remove redundant code. Improved error checking as a result
 * of API testing.
 *
 * Hydra_Software_Devel/49   10/27/11 3:41p nilesh
 * SW7425-1236: Fixed debug message prints
 *
 * Hydra_Software_Devel/48   10/17/11 3:30p nilesh
 * SW7425-721: Fixed memory leak
 *
 * Hydra_Software_Devel/47   10/6/11 5:10p nilesh
 * SW7425-721: Updated to use BMUXlib_Input routines
 *
 * Hydra_Software_Devel/46   10/5/11 5:55p delkert
 * SW7425-1415: Fix coverity issues
 *
 * Hydra_Software_Devel/45   7/1/11 2:44p nilesh
 * SW7425-831: Flush transport channels that are active, regardless of
 * whether or not there are packets pending.
 *
 * Hydra_Software_Devel/44   7/1/11 1:56p nilesh
 * SW7425-831: Fixed NRT stall deadlock during shutdown
 *
 * Hydra_Software_Devel/43   7/1/11 12:46p nilesh
 * SW7425-831: Added flush logic when in NRT mode
 *
 * Hydra_Software_Devel/42   7/1/11 1:43a nilesh
 * SW7425-827: Added BMUXlib_TS_PENDING debug message
 *
 * Hydra_Software_Devel/41   6/29/11 11:40a nilesh
 * SW7425-673: Fixed compilation error
 *
 * Hydra_Software_Devel/40   6/29/11 11:05a nilesh
 * SW7425-673: BMUXlib_DoMux_Status is now memset to 0 in DoMux()
 *
 * Hydra_Software_Devel/39   6/14/11 1:56p nilesh
 * SW7425-699: Disable error/warning messages that are not relevant in
 * non-realtime (NRT) mode
 *
 * Hydra_Software_Devel/38   5/11/11 11:55a nilesh
 * SW7425-447: Moved list management functions to a separare shared
 * utility
 *
 * Hydra_Software_Devel/37   4/25/11 12:45a nilesh
 * SW7425-385: To prevent issues related to ESCR discontinuity, PCRs are
 * now generated (temporarily) based on current (PACING_COUNTER + MSP)
 * and not lowest A/V ESCR
 *
 * Hydra_Software_Devel/36   4/23/11 10:35p hongtaoz
 * SW7425-385: increase stream mux total shared transport descriptors for
 * now to avoid interlock among audio, video and system data;
 *
 * Hydra_Software_Devel/35   4/15/11 1:42p nilesh
 * SW7425-139: Added check for Mux Service Perioid violation
 *
 * Hydra_Software_Devel/34   3/8/11 4:59p nilesh
 * SW7425-38: Increased total number of pending transport descriptors
 *
 * Hydra_Software_Devel/33   3/1/11 11:57a nilesh
 * SW7425-38: Added PES dbg dump support using #define
 * BMUXLIB_TS_P_DUMP_TRANSPORT_PES
 *
 * Hydra_Software_Devel/32   3/1/11 11:36a nilesh
 * SW7425-38: Added file descriptor dbg dump support using #define
 * BMUXLIB_TS_P_DUMP_[AUDIO/VIDEO/TRANSPORT]_DESC
 *
 * Hydra_Software_Devel/31   2/28/11 7:56p nilesh
 * SW7425-38: Set length in PES Header for audio
 *
 * Hydra_Software_Devel/30   2/28/11 7:00p nilesh
 * SW7425-111: Added late ESCR detection
 *
 * Hydra_Software_Devel/29   2/28/11 4:33p nilesh
 * SW7425-88: Added BMUXLIB_TS_FINISH dbg module.  Fixed issue with
 * pending descriptors not being queued to transport when in the
 * finishing_output state.
 *
 * Hydra_Software_Devel/28   2/22/11 3:47p nilesh
 * SW7425-110: Changed BDBG_ASSERT to BDBG_ERR where necessary
 *
 * Hydra_Software_Devel/27   2/7/11 11:02a nilesh
 * SW7425-38: Removed unused mux mode setting
 *
 * Hydra_Software_Devel/26   2/4/11 10:37a nilesh
 * SW7425-38: Commented out unnecessary callback APIs
 *
 * Hydra_Software_Devel/25   2/3/11 4:10p nilesh
 * SW7425-38: API Changes
 *
 * Hydra_Software_Devel/24   2/2/11 2:16p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddSystemDataBuffers() prototype
 *
 * Hydra_Software_Devel/23   2/2/11 1:28p nilesh
 * SW7425-38: Changed BMUXlib_TS_AddTransportDescriptors() prototype
 *
 * Hydra_Software_Devel/22   2/2/11 11:38a nilesh
 * SW7425-38: Add streamID support
 *
 * Hydra_Software_Devel/21   2/1/11 4:58p nilesh
 * SW7425-38: stream id to StartSettings
 *
 * Hydra_Software_Devel/20   1/31/11 5:56p nilesh
 * SW7425-38: Changed DoMux API
 *
 * Hydra_Software_Devel/19   1/28/11 2:43p nilesh
 * SW7425-38: Change GetDefaultXXX() return value to void
 *
 * Hydra_Software_Devel/18   1/20/11 3:07p nilesh
 * SW7425-38: Wait for all pending transport descriptors to be completed
 * triggering finish
 *
 * Hydra_Software_Devel/17   1/19/11 2:11p nilesh
 * SW7425-38: Added EOS and LAST BPP support
 *
 * Hydra_Software_Devel/16   12/30/10 7:11p nilesh
 * SW7425-38: Initialize puiQueuedCount before using
 *
 * Hydra_Software_Devel/15   12/30/10 2:51p nilesh
 * SW7425-38: Fix memory leak on destroy
 *
 * Hydra_Software_Devel/14   12/30/10 2:10p nilesh
 * SW7425-38: Disable shutdown check of pending descriptors
 *
 * Hydra_Software_Devel/13   12/23/10 12:31p nilesh
 * SW7425-38: Fixed debug print
 *
 * Hydra_Software_Devel/12   12/21/10 4:55p nilesh
 * SW7425-38: Added debug print for current STC/ESCR.
 *
 * Hydra_Software_Devel/11   12/14/10 1:45p nilesh
 * SW7425-38: Add queuing of system data into muxlib
 *
 * Hydra_Software_Devel/10   12/9/10 2:15p nilesh
 * SW7425-38: Fixed uiTransportLastESCR calculation for system data
 * transport channel
 *
 * Hydra_Software_Devel/9   10/7/10 4:30p nilesh
 * SW7425-38: In BMUXlib_TS_StartSettings, removed uiInterval, renamed
 * uiIntervalMaxLatency to uiServiceLatencyTolerance, renamed
 * stOutputInterface to stChannelInterface, and renamed uiTransportIndex
 * to uiTransportChannelIndex.  Changed uint32_t to unsigned as needed.
 *
 * Hydra_Software_Devel/8   10/4/10 2:18p nilesh
 * SW7425-38: Added PCR pacing
 *
 * Hydra_Software_Devel/7   9/29/10 5:53p nilesh
 * SW7425-38: Added basic PCR support
 *
 * Hydra_Software_Devel/6   9/29/10 3:27p nilesh
 * SW7425-38: Slight restructure to support multiple audio/video PIDs
 *
 * Hydra_Software_Devel/5   9/28/10 4:46p nilesh
 * SW7425-38: Added basic video muxing
 *
 * Hydra_Software_Devel/4   9/24/10 12:09p nilesh
 * SW7425-38: Updated transport and system data interface.  Added debug
 * dump of video descriptors.
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

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#include "bmuxlib_ts.h"
#include "bmuxlib_ts_priv.h"

#include "bmuxlib_input.h"

#if BMUXLIB_TS_P_TEST_MODE
#include <stdio.h>
#endif

BDBG_MODULE(BMUXLIB_TS);
BDBG_FILE_MODULE(BMUXLIB_TS_FINISH);
BDBG_FILE_MODULE(BMUXLIB_TS_PENDING);
BDBG_FILE_MODULE(BMUXLIB_TS_MSP);
BDBG_FILE_MODULE(BMUXLIB_TS_MEMORY);

/******************/
/* Create/Destroy */
/******************/

#define BMUXLIB_TS_P_SYSTEM_DATA_PRE_Q_COUNT 5
#define BMUXLIB_TS_P_NOMINAL_SYSTEM_DATA_BITRATE 2000000
#define BMUXLIB_TS_P_NOMINAL_PCR_INTERVAL 20
#define BMUXLIB_TS_P_MAX_NRT_SPEED 4
#define BMUXLIB_TS_P_MSP_COUNT 2
#define BMUXLIB_TS_P_MIN_DESCRIPTORS_PER_FRAME 2 /* for PES and CDB */
#define BMUXLIB_TS_P_EXTRA_DESCRIPTORS_PER_MSP 3 /* partial frame and/or CDB wrap and/or BPP packet */
#define BMUXLIB_TS_P_MAX_VIDEO_FPS 60
#define BMUXLIB_TS_P_MAX_AUDIO_FPS 50

#define BMUXLIB_TS_P_MAX(a,b) ((a > b) ? a : b)
#define BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP(a,b) ((b)?(((a) + ((b)-1))/(b)):0)

void
BMUXlib_TS_P_GetMemoryConfigTotal(
   const BMUXlib_TS_P_MemoryConfig *pstMemoryConfig,
   BMUXlib_TS_P_MemoryConfigTotal *pstMemoryConfigTotal
)
{
   BDBG_ASSERT( pstMemoryConfig );
   BDBG_ASSERT( pstMemoryConfigTotal );

   BKNI_Memset( pstMemoryConfigTotal, 0, sizeof( BMUXlib_TS_P_MemoryConfigTotal ) );
   {
      BMUXlib_TS_P_MemoryEntryType eMemoryEntryType;
      for ( eMemoryEntryType = 0; eMemoryEntryType < BMUXlib_TS_P_MemoryEntryType_eMax; eMemoryEntryType++ )
      {
         BMUXlib_TS_P_InputType eInputType;
         for ( eInputType=0; eInputType<BMUXlib_TS_P_InputType_eMax; eInputType++ )
         {
            BMUXlib_TS_P_MemoryType eMemoryType;
            for ( eMemoryType=0; eMemoryType<BMUXlib_TS_P_MemoryType_eMax; eMemoryType++ )
            {
               pstMemoryConfigTotal->astMemoryEntry[eMemoryEntryType].stMemoryConfig.stBufferInfo[eMemoryType].uiSize += pstMemoryConfig->astMemoryEntry[eMemoryEntryType][eInputType].stMemoryConfig.stBufferInfo[eMemoryType].uiSize;
            }
            pstMemoryConfigTotal->astMemoryEntry[eMemoryEntryType].uiCount += pstMemoryConfig->astMemoryEntry[eMemoryEntryType][eInputType].uiCount;
         }
      }
   }

   pstMemoryConfigTotal->stMemoryConfig = pstMemoryConfig->stMemoryConfig;
}

void
BMUXlib_TS_P_GetMemoryConfig(
   const BMUXlib_TS_MuxConfig *pstMuxConfig,
   BMUXlib_TS_P_MemoryConfig *pstMemoryConfig
)
{
   BDBG_ENTER( BMUXlib_TS_P_GetMemoryConfig );

   BDBG_ASSERT(pstMuxConfig);
   BDBG_ASSERT(pstMemoryConfig);

   BKNI_Memset( pstMemoryConfig, 0, sizeof( BMUXlib_TS_P_MemoryConfig ) );

   /* Determine number of descriptors required */
   /* MSP: 100 ms max
    * Video: 60 frames/sec max
    * Audio: ~50 frames/sec max
    * System: 1 Mbps/sec
    *    pcr: 20 pcrs/sec max (50 ms interval)
    *    system/user data: 1 Mbps/sec * sec/1000ms * 100ms/MSP * byte/8 bits * packet/188 bytes = ~ 67 packets/sec max
    *
    *    - 1 PES Header/frame
    *    - 2 Transport Descriptors/frame
    *    - 3 Additional Transport Descriptors for:
    *       - partial frame and/or
    *       - CDB wrap and/or
    *       - BPP packet
    *    video: = 60 frames/sec * sec/1000ms * 100ms/MSP = 6 frames/MSP
    *       --> 6 PES/MSP
    *       --> 12+3 Transport Descriptors/MSP
    *    audio:
    *       --> 5 PES/MSP
    *       --> 10+3 Transport Descriptors/MSP
    *    system:
    *       --> 20 TS Packets/MSP (pcr)
    *       --> 67 Transport Descriptors/MSP
    *       --> 67 System Data Buffers/MSP
    *
    */

   {
      unsigned uiMuxServicePeriod = (pstMuxConfig->stMuxStartSettings.uiServicePeriod + pstMuxConfig->stMuxStartSettings.uiServiceLatencyTolerance);
      /* Video */
      {
         unsigned uiNumFrames = BMUXLIB_TS_P_MAX(2,BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP((BMUXLIB_TS_P_MAX_VIDEO_FPS * uiMuxServicePeriod),1000)) * BMUXLIB_TS_P_MSP_COUNT;
         if ( true == pstMuxConfig->stMuxStartSettings.bNonRealTimeMode ) uiNumFrames *= BMUXLIB_TS_P_MAX_NRT_SPEED;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader][BMUXlib_TS_P_InputType_eVideo].uiCount = uiNumFrames;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eVideo].uiCount = (uiNumFrames * BMUXLIB_TS_P_MIN_DESCRIPTORS_PER_FRAME) + ( BMUXLIB_TS_P_EXTRA_DESCRIPTORS_PER_MSP * BMUXLIB_TS_P_MSP_COUNT );

         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader][BMUXlib_TS_P_InputType_eVideo].uiCount *= pstMuxConfig->stMuxStartSettings.uiNumValidVideoPIDs;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eVideo].uiCount *= pstMuxConfig->stMuxStartSettings.uiNumValidVideoPIDs;

         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eBPP][BMUXlib_TS_P_InputType_eVideo].uiCount = pstMuxConfig->stMuxStartSettings.uiNumValidVideoPIDs;
      }

      /* Audio */
      {
         unsigned uiNumFrames = BMUXLIB_TS_P_MAX(2,BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP((BMUXLIB_TS_P_MAX_AUDIO_FPS * uiMuxServicePeriod),1000)) * BMUXLIB_TS_P_MSP_COUNT;
         if ( true == pstMuxConfig->stMuxStartSettings.bNonRealTimeMode ) uiNumFrames *= BMUXLIB_TS_P_MAX_NRT_SPEED;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader][BMUXlib_TS_P_InputType_eAudio].uiCount = uiNumFrames;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eAudio].uiCount = (uiNumFrames * BMUXLIB_TS_P_MIN_DESCRIPTORS_PER_FRAME) + ( BMUXLIB_TS_P_EXTRA_DESCRIPTORS_PER_MSP * BMUXLIB_TS_P_MSP_COUNT );

         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader][BMUXlib_TS_P_InputType_eAudio].uiCount *= pstMuxConfig->stMuxStartSettings.uiNumValidAudioPIDs;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eAudio].uiCount *= pstMuxConfig->stMuxStartSettings.uiNumValidAudioPIDs;

         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eBPP][BMUXlib_TS_P_InputType_eAudio].uiCount = pstMuxConfig->stMuxStartSettings.uiNumValidAudioPIDs;
      }

      /* System */
      {
         unsigned uiNumFrames = BMUXLIB_TS_P_MAX(2,BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP(pstMuxConfig->stMuxSettings.uiSystemDataBitRate * uiMuxServicePeriod,1000 * 8 * 188)) * BMUXLIB_TS_P_MSP_COUNT;
         if ( true == pstMuxConfig->stMuxStartSettings.bNonRealTimeMode ) uiNumFrames *= BMUXLIB_TS_P_MAX_NRT_SPEED;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eSystem].uiCount = uiNumFrames;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eSystemData][BMUXlib_TS_P_InputType_eSystem].uiCount = uiNumFrames;
      }
      {
         unsigned uiNumPcrs = BMUXLIB_TS_P_MAX(2,BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP(uiMuxServicePeriod, pstMuxConfig->stMuxStartSettings.stPCRData.uiInterval)) * BMUXLIB_TS_P_MSP_COUNT;
         if ( true == pstMuxConfig->stMuxStartSettings.bNonRealTimeMode ) uiNumPcrs *= BMUXLIB_TS_P_MAX_NRT_SPEED;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket][BMUXlib_TS_P_InputType_eSystem].uiCount = uiNumPcrs;
      }
      /* TS User Data */
      {
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserDataUnwrap][BMUXlib_TS_P_InputType_eSystem].uiCount = pstMuxConfig->stMuxStartSettings.uiNumValidUserdataPIDs;
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserDataReleaseQ][BMUXlib_TS_P_InputType_eSystem].uiCount = (pstMuxConfig->stMuxStartSettings.uiNumValidUserdataPIDs * ( (BMUXLIB_TS_USERDATA_MAX_SEGMENTS-2)*( BMUXLIB_TS_USERDATA_MAX_PKTS_PER_VID * pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader][BMUXlib_TS_P_InputType_eVideo].uiCount ) + 1));
         pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eSystem].uiCount += pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserDataReleaseQ][BMUXlib_TS_P_InputType_eSystem].uiCount;
         {
            #define BMUXLIB_TS_P_MAX_VIDEO_END_TO_END_DELAY 2000
            unsigned uiNumFrames = BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP((BMUXLIB_TS_P_MAX_VIDEO_FPS * BMUXLIB_TS_P_MAX_VIDEO_END_TO_END_DELAY),1000);
            pstMemoryConfig->astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData][BMUXlib_TS_P_InputType_eSystem].uiCount = (BMUXLIB_TS_USERDATA_MAX_PKTS_PER_VID * pstMuxConfig->stMuxStartSettings.uiNumValidUserdataPIDs * uiNumFrames);
         }
      }
   }

   {
      BMUXlib_TS_P_MemoryEntryType eMemoryEntryType;

      for ( eMemoryEntryType = 0; eMemoryEntryType < BMUXlib_TS_P_MemoryEntryType_eMax; eMemoryEntryType++ )
      {
         BMUXlib_TS_P_InputType eInputType;

         for ( eInputType=0; eInputType<BMUXlib_TS_P_InputType_eMax; eInputType++ )
         {
#define BMUXLIB_TS_P_ADD_MEMORY_SIZE(size,p,i,j,type) (p)[i][j].stMemoryConfig.stBufferInfo[type].uiSize += BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP( size * (p)[i][j].uiCount, 4096 ) * 4096

            switch ( eMemoryEntryType )
            {
               case BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor:
                  /* Transport Descriptors */
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_TransportDescriptor ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eSystem);
                  /* Transport Descriptor Temp Array */
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_TransportDescriptor ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eSystem);
                  /* Transport Descriptor Metadata Array */
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_P_TransportDescriptorMetaData ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eSystem);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_ePESHeader:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_P_PESHeader ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eShared);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_eTransportPacket:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_P_TSPacket ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eShared);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_eSystemData:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_SystemData ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eShared);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_eBPP:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_P_BPPData ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eShared);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_eUserData:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_P_UserdataPending ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eSystem);
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof(BMUXlib_TS_P_UserdataPTSEntry), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eShared);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_eUserDataUnwrap:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof(BMUXlib_TS_P_TSPacket), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eShared);
                  break;

               case BMUXlib_TS_P_MemoryEntryType_eUserDataReleaseQ:
                  BMUXLIB_TS_P_ADD_MEMORY_SIZE(sizeof( BMUXlib_TS_P_UserdataReleaseQEntry ), pstMemoryConfig->astMemoryEntry,eMemoryEntryType,eInputType,BMUXlib_TS_P_MemoryType_eSystem);
                  break;

               default:
                  BDBG_ERR(("Unsupported memory entry type (%d)", eMemoryEntryType ));
                  break;
            }

            {
               BMUXlib_TS_P_MemoryType eMemoryType;
               for ( eMemoryType=0; eMemoryType<BMUXlib_TS_P_MemoryType_eMax; eMemoryType++ )
               {
                  pstMemoryConfig->stMemoryConfig.stBufferInfo[eMemoryType].uiSize += pstMemoryConfig->astMemoryEntry[eMemoryEntryType][eInputType].stMemoryConfig.stBufferInfo[eMemoryType].uiSize;
               }
            }
         }
      }
   }

   BDBG_LEAVE( BMUXlib_TS_P_GetMemoryConfig );
}

#define BMUXLIB_TS_P_ASSIGN_IF_LARGER(a,b,field) if ((a)->field < (b)->field) { ((a)->field = (b)->field); }

void
BMUXlib_TS_P_GetMaxMemoryConfig(
   const BMUXlib_TS_MuxConfig *astMuxConfig, /* Array of possible configurations */
   unsigned uiNumMuxConfig,
   BMUXlib_TS_P_MemoryConfig *pstMemoryConfig /* The memory config required to support all possible configurations */
   )
{
   unsigned i;

   BDBG_ENTER( BMUXlib_TS_P_GetMaxMemoryConfig );

   BDBG_ASSERT(astMuxConfig);
   BDBG_ASSERT(uiNumMuxConfig);
   BDBG_ASSERT(pstMemoryConfig);

   BKNI_Memset( pstMemoryConfig, 0, sizeof( BMUXlib_TS_P_MemoryConfig ) );

   for ( i = 0; (NULL != astMuxConfig) && (i < uiNumMuxConfig); i++ )
   {
      BMUXlib_TS_P_MemoryConfig stMuxMemoryConfig;

      BMUXlib_TS_P_GetMemoryConfig(
         &astMuxConfig[i],
         &stMuxMemoryConfig
      );

      {
         BMUXlib_TS_P_MemoryEntryType eMemoryEntryType;

         for ( eMemoryEntryType = 0; eMemoryEntryType < BMUXlib_TS_P_MemoryEntryType_eMax; eMemoryEntryType++ )
         {
            BMUXlib_TS_P_InputType eInputType;

            for ( eInputType=0; eInputType<BMUXlib_TS_P_InputType_eMax; eInputType++ )
            {
               BMUXlib_TS_P_MemoryType eMemoryType;
               for ( eMemoryType=0; eMemoryType<BMUXlib_TS_P_MemoryType_eMax; eMemoryType++ )
               {
                  BMUXLIB_TS_P_ASSIGN_IF_LARGER(pstMemoryConfig, &stMuxMemoryConfig, astMemoryEntry[eMemoryEntryType][eInputType].stMemoryConfig.stBufferInfo[eMemoryType].uiSize);
               }

               BMUXLIB_TS_P_ASSIGN_IF_LARGER(pstMemoryConfig, &stMuxMemoryConfig, astMemoryEntry[eMemoryEntryType][eInputType].uiCount);
            }
         }
      }

      {
         BMUXlib_TS_P_MemoryType eMemoryType;
         for ( eMemoryType=0; eMemoryType<BMUXlib_TS_P_MemoryType_eMax; eMemoryType++ )
         {
            BMUXLIB_TS_P_ASSIGN_IF_LARGER(pstMemoryConfig, &stMuxMemoryConfig, stMemoryConfig.stBufferInfo[eMemoryType].uiSize);
         }
      }
   }

   BDBG_LEAVE( BMUXlib_TS_P_GetMaxMemoryConfig );
}

/**********/
/* Memory */
/**********/
void
BMUXlib_TS_GetMemoryConfig(
         const BMUXlib_TS_MuxConfig *pstMuxConfig,
         BMUXlib_TS_MemoryConfig *pstMemoryConfig
         )
{
   BMUXlib_TS_P_MemoryConfig stMemoryConfig;

   BDBG_ENTER( BMUXlib_TS_GetMemoryConfig );

   BDBG_ASSERT(pstMuxConfig);
   BDBG_ASSERT(pstMemoryConfig);

   BKNI_Memset( &stMemoryConfig, 0, sizeof( BMUXlib_TS_P_MemoryConfig )  );
   BKNI_Memset( pstMemoryConfig, 0, sizeof( BMUXlib_TS_MemoryConfig )  );

   BMUXlib_TS_P_GetMaxMemoryConfig(
      pstMuxConfig,
      1,
      &stMemoryConfig
      );

   pstMemoryConfig->uiSystemBufferSize = stMemoryConfig.stMemoryConfig.stBufferInfo[BMUXlib_TS_P_MemoryType_eSystem].uiSize;
   pstMemoryConfig->uiSharedBufferSize = stMemoryConfig.stMemoryConfig.stBufferInfo[BMUXlib_TS_P_MemoryType_eShared].uiSize;

   BDBG_LEAVE( BMUXlib_TS_GetMemoryConfig );
}

void
BMUXlib_TS_P_GetDefaultMemorySize(
   BMUXlib_TS_MemoryConfig *pstMemoryConfig /* The memory size required to support all possible configurations */
   )
{
   BMUXlib_TS_MuxConfig stMuxConfig;

   BDBG_ENTER( BMUXlib_TS_P_GetDefaultMemorySize );

   BDBG_ASSERT( pstMemoryConfig );

   /* Determine sub heap allocation size */
   BMUXlib_TS_GetDefaultStartSettings(
      &stMuxConfig.stMuxStartSettings
      );

   BMUXlib_TS_GetDefaultMuxSettings(
      &stMuxConfig.stMuxSettings
      );

   /* Assume 6 audio, 1 video, 8 user data pids, and NRT mode */
   stMuxConfig.stMuxStartSettings.uiNumValidVideoPIDs = BMUXLIB_TS_MAX_VIDEO_PIDS;
   stMuxConfig.stMuxStartSettings.uiNumValidAudioPIDs = BMUXLIB_TS_MAX_AUDIO_PIDS;
   stMuxConfig.stMuxStartSettings.uiNumValidUserdataPIDs = BMUXLIB_TS_MAX_USERDATA_PIDS;
   stMuxConfig.stMuxStartSettings.bNonRealTimeMode = true;
   stMuxConfig.stMuxSettings.uiSystemDataBitRate = BMUXLIB_TS_P_NOMINAL_SYSTEM_DATA_BITRATE;
   stMuxConfig.stMuxStartSettings.stPCRData.uiInterval = BMUXLIB_TS_P_NOMINAL_PCR_INTERVAL;

   BMUXlib_TS_GetMemoryConfig(
      &stMuxConfig,
      pstMemoryConfig
      );

   BDBG_LEAVE( BMUXlib_TS_P_GetDefaultMemorySize );
}

void
BMUXlib_TS_GetDefaultCreateSettings(
         BMUXlib_TS_CreateSettings *pCreateSettings
         )
{
   BDBG_ENTER( BMUXlib_TS_GetDefaultCreateSettings );

   BDBG_ASSERT( pCreateSettings );

   BKNI_Memset(
            pCreateSettings,
            0,
            sizeof(BMUXlib_TS_CreateSettings)
            );

   pCreateSettings->uiSignature = BMUXLIB_TS_P_SIGNATURE_CREATESETTINGS;

   BMUXlib_TS_P_GetDefaultMemorySize(
      &pCreateSettings->stMemoryConfig
      );

   BDBG_LEAVE( BMUXlib_TS_GetDefaultCreateSettings );
   return;
}

void
BMUXlib_TS_P_GetBufferConfigFromMemoryConfig(
   const BMUXlib_TS_MemoryConfig *pstMemoryConfig,
   const BMUXlib_TS_MemoryBuffers *pstMemoryBuffers,
   BMUXlib_TS_P_BufferConfig *pstBufferConfig
   )
{
   BDBG_ENTER(BMUXlib_TS_P_GetBufferConfigFromMemoryConfig);

   BKNI_Memset( pstBufferConfig, 0, sizeof( BMUXlib_TS_P_BufferConfig ) );

   pstBufferConfig->stBufferInfo[BMUXlib_TS_P_MemoryType_eSystem].uiSize = pstMemoryConfig->uiSystemBufferSize;
   pstBufferConfig->stBufferInfo[BMUXlib_TS_P_MemoryType_eSystem].pBuffer = pstMemoryBuffers->pSystemBuffer;
   pstBufferConfig->stBufferInfo[BMUXlib_TS_P_MemoryType_eShared].uiSize = pstMemoryConfig->uiSharedBufferSize;
   pstBufferConfig->stBufferInfo[BMUXlib_TS_P_MemoryType_eShared].pBuffer = pstMemoryBuffers->pSharedBuffer;

   BDBG_LEAVE(BMUXlib_TS_P_GetBufferConfigFromMemoryConfig);
}

/* BMUXlib_TS_P_AllocateMemory - Allocates all system/device memory required for mux operation */
BERR_Code
BMUXlib_TS_P_AllocateMemory(
   BMUXlib_TS_Handle hMuxTS,
   BMEM_Handle hMem,
   BMUXlib_TS_P_BufferConfig *pstMemoryConfig
)
{
   BERR_Code rc = BERR_SUCCESS;
   BMEM_Settings memModuleSettings;
   BMEM_Heap_Settings memHeapSettings;
   BMUXlib_TS_P_MemoryType eMemoryType;

   BDBG_ENTER( BMUXlib_TS_P_AllocateMemory );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( hMem );
   BDBG_ASSERT( pstMemoryConfig );

   for ( eMemoryType=0; eMemoryType < BMUXlib_TS_P_MemoryType_eMax ; eMemoryType++ )
   {
      /* memory config must specify some amount of memory ... */
      if (0 == pstMemoryConfig->stBufferInfo[eMemoryType].uiSize)
      {
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      /* Create sub-heap */
      /* Create BMEM Module  */
      rc = BMEM_GetDefaultSettings(
               &memModuleSettings
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error getting default memory module settings"));
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE( rc );
      }

      rc = BMEM_Open(
               &hMuxTS->stSubHeap[eMemoryType].hMemModule,
               &memModuleSettings
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error opening memory module"));
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE( rc );
      }

      hMuxTS->stSubHeap[eMemoryType].uiSize = pstMemoryConfig->stBufferInfo[eMemoryType].uiSize;

      /* Only allocate memory for the sub heap if an explicit buffer is not passed in */
      if ( NULL != pstMemoryConfig->stBufferInfo[eMemoryType].pBuffer )
      {
         hMuxTS->stSubHeap[eMemoryType].pBuffer = pstMemoryConfig->stBufferInfo[eMemoryType].pBuffer;
      }
      else
      {
         hMuxTS->stSubHeap[eMemoryType].pBuffer = BMEM_AllocAligned(
            hMem,
            hMuxTS->stSubHeap[eMemoryType].uiSize,
            0,
            0
            );

         if ( NULL == hMuxTS->stSubHeap[eMemoryType].pBuffer )
         {
            BDBG_ERR(("Error allocating memory"));
            BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
            return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
         }
      }
      /* Get offset */
      rc = BMEM_ConvertAddressToOffset(
         hMuxTS->stCreateSettings.hMem,
         hMuxTS->stSubHeap[eMemoryType].pBuffer,
         &hMuxTS->stSubHeap[eMemoryType].uiBufferOffset
      );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting virtual address to offset"));
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE( rc );
      }

      /* Get Cached Address */
      rc = BMEM_ConvertAddressToCached(
         hMuxTS->stCreateSettings.hMem,
         hMuxTS->stSubHeap[eMemoryType].pBuffer,
         &hMuxTS->stSubHeap[eMemoryType].pBufferCached
      );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting general virtual address to cached"));
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE( rc );
      }

      /* Create BMEM Heap */
      rc = BMEM_Heap_GetDefaultSettings(
               &memHeapSettings
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error getting default memory heap settings"));
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE( rc );
      }

      memHeapSettings.eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;
      memHeapSettings.eSafetyConfig = BMEM_CONFIG_FASTEST;

      rc = BMEM_Heap_Create(
         hMuxTS->stSubHeap[eMemoryType].hMemModule,
         hMuxTS->stSubHeap[eMemoryType].pBuffer,
         hMuxTS->stSubHeap[eMemoryType].uiBufferOffset,
         hMuxTS->stSubHeap[eMemoryType].uiSize,
         &memHeapSettings,
         &hMuxTS->stSubHeap[eMemoryType].hMem
      );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error getting creating memory heap"));
         BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );
         return BERR_TRACE( rc );
      }
   }


   BDBG_LEAVE( BMUXlib_TS_P_AllocateMemory );

   return BERR_TRACE( rc );
}

void
BMUXlib_TS_P_FreeMemory(
   BMUXlib_TS_Handle hMuxTS,
   BMEM_Handle hMem
   )
{
   BMUXlib_TS_P_MemoryType eMemoryType;
   BMUXlib_TS_P_BufferConfig stMemoryConfigLocal;

   BDBG_ENTER( BMUXlib_TS_P_FreeMemory );

   BMUXlib_TS_P_GetBufferConfigFromMemoryConfig( &hMuxTS->stCreateSettings.stMemoryConfig, &hMuxTS->stCreateSettings.stMemoryBuffers, &stMemoryConfigLocal );

   for ( eMemoryType=0; eMemoryType < BMUXlib_TS_P_MemoryType_eMax; eMemoryType++ )
   {
      if ( NULL != hMuxTS->stSubHeap[eMemoryType].hMem )
      {
         BMEM_Heap_Destroy( hMuxTS->stSubHeap[eMemoryType].hMem );
      }

      /* Only free memory for the sub heap if an explicit buffer was not passed in */
      if ( NULL == stMemoryConfigLocal.stBufferInfo[eMemoryType].pBuffer )
      {
         if ( NULL != hMuxTS->stSubHeap[eMemoryType].pBuffer )
         {
            BMEM_Free(
               hMem,
               hMuxTS->stSubHeap[eMemoryType].pBuffer
            );
         }
         hMuxTS->stSubHeap[eMemoryType].pBuffer = NULL;
      }

      if ( NULL != hMuxTS->stSubHeap[eMemoryType].hMemModule )
      {
         BMEM_Close( hMuxTS->stSubHeap[eMemoryType].hMemModule );

         hMuxTS->stSubHeap[eMemoryType].hMemModule = NULL;
      }

      BKNI_Memset( &hMuxTS->stSubHeap[eMemoryType], 0, sizeof( hMuxTS->stSubHeap[eMemoryType] ) );
   }

   BDBG_LEAVE( BMUXlib_TS_P_FreeMemory );
}

void
BMUXlib_TS_P_ResetResources(
   BMUXlib_TS_Handle hMuxTS
   )
{
   unsigned i;

   BDBG_ENTER( BMUXlib_TS_P_ResetResources );

   /* Available Transport Descriptors */
   BMUXlib_List_Reset( hMuxTS->hTransportDescriptorFreeList );
   BMUXlib_List_Reset( hMuxTS->hTransportDescriptorMetaDataFreeList );

   for ( i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount; i++ )
   {
      BMUXlib_List_Add(
               hMuxTS->hTransportDescriptorFreeList,
               &hMuxTS->astTransportDescriptor[i]
               );

      BMUXlib_List_Add(
               hMuxTS->hTransportDescriptorMetaDataFreeList,
               &hMuxTS->astTransportDescriptorMetaData[i]
               );
   }

/* SW7425-3642: DCE 08/01/2012:
   FIXME: these do not seem to be called, since at this point they have
   not been allocated yet (ResetResources is only called from AllocateResources, and
   these items are allocated in Start() AFTER AllocateResources is called) */
#if 0
   /* TODO: Do a lazy free here */
   /* Pending Transport Descriptors */
   for ( i = 0; i < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; i++ )
   {
      if ( NULL != hMuxTS->hTransportDescriptorPendingList[i] )
      {
         BMUXlib_List_Destroy(
                  hMuxTS->hTransportDescriptorPendingList[i]
                  );

         hMuxTS->hTransportDescriptorPendingList[i] = NULL;
      }

      if ( NULL != hMuxTS->hTransportDescriptorPendingList[i] )
      {
         BMUXlib_List_Destroy(
                  hMuxTS->hTransportDescriptorMetaDataPendingList[i]
                  );

         hMuxTS->hTransportDescriptorMetaDataPendingList[i] = NULL;
      }
   }
#endif
   /* PES Headers */
   BMUXlib_List_Reset( hMuxTS->hPESHeaderFreeList );

   for ( i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader].uiCount; i++ )
   {
      BMUXlib_List_Add(
               hMuxTS->hPESHeaderFreeList,
               &hMuxTS->astPESHeader[i]
               );
   }

   /* TS Packets */
   BMUXlib_List_Reset(hMuxTS->hTSPacketFreeList);
   for ( i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount; i++ )
   {
      BMUXlib_List_Add(
               hMuxTS->hTSPacketFreeList,
               &hMuxTS->astTSPacket[i]
               );
   }

   /* Pending System Data */
   BMUXlib_List_Reset( hMuxTS->hSystemDataPendingList );

   /* push all available userdata pending entries onto the free list ... */
   BMUXlib_List_Reset(hMuxTS->hUserdataFreeList);

   for (i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData].uiCount; i++)
   {
      BMUXlib_List_Add(hMuxTS->hUserdataFreeList, &hMuxTS->astUserdataPending[i]);
   }

/* SW7425-3642: DCE 08/01/2012:
   FIXME: these do not seem to be called, since at this point they have
   not been allocated yet (ResetResources is only called from AllocateResources, and
   these items are allocated in Start() AFTER AllocateResources is called) */
#if 0
   /* Pending Userdata */
   for (i = 0; i < BMUXLIB_TS_MAX_USERDATA_PIDS; i++)
   {
      if ( NULL != hMuxTS->hUserdataPendingList[i] )
      {
         BMUXlib_List_Destroy( hMuxTS->hUserdataPendingList[i] );
         hMuxTS->hUserdataPendingList[i] = NULL;
      }
   }
#endif

   /* Userdata PTS entries */
   BMUXlib_List_Reset( hMuxTS->hUserdataPTSFreeList );

   for (i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData].uiCount; i++)
   {
      BMUXlib_List_Add(hMuxTS->hUserdataPTSFreeList, hMuxTS->astUserdataPTS[i].aPTS);
   }


   /* Userdata release Q free list */
   {
      uint32_t uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserDataReleaseQ].uiCount;
      if ( 0 != uiCount )
      {
         hMuxTS->pUserdataReleaseQFreeList = hMuxTS->pUserdataReleaseQFreeListBase;
         BKNI_Memset(hMuxTS->pUserdataReleaseQFreeList, 0, sizeof(BMUXlib_TS_P_UserdataReleaseQEntry) * uiCount);
         for (i = 0; i < uiCount-1; i++)
            hMuxTS->pUserdataReleaseQFreeList[i].pNext = &hMuxTS->pUserdataReleaseQFreeList[i+1];
         hMuxTS->pUserdataReleaseQFreeList[i].pNext = NULL;
      }
   }

   BDBG_LEAVE( BMUXlib_TS_P_ResetResources );
}

BERR_Code
BMUXlib_TS_P_AllocateResources(
   BMUXlib_TS_Handle hMuxTS,
   const BMUXlib_TS_P_MemoryConfig *pstMemoryConfig
   )
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t i;
   BMUXlib_List_CreateSettings stListCreateSettings, stDefaultListCreateSettings;
   BMUXlib_List_Handle hSystemDataPendingList = NULL;
   BMUXlib_TS_SystemData *astSystemDataPendingList = NULL;
   uint32_t uiSystemDataPendingListCount = 0;

   BDBG_ENTER( BMUXlib_TS_P_AllocateResources );

   hMuxTS->status.stMemoryConfig = *pstMemoryConfig;
   BMUXlib_TS_P_GetMemoryConfigTotal( &hMuxTS->status.stMemoryConfig, &hMuxTS->status.stMemoryConfigTotal );

#if BDBG_DEBUG_BUILD
   {
      BMUXlib_TS_P_MemoryEntryType eMemoryEntryType;

      static const char* BMUXlib_TS_P_MemoryTypeLUT[BMUXlib_TS_P_MemoryEntryType_eMax] =
      {
         "eTransportDescriptor",
         "ePESHeader",
         "eTransportPacket",
         "eSystemData",
         "BPP",
         "UserData",
         "UserDataUnwrap",
         "UserDataReleaseQ"
      };

      for ( eMemoryEntryType = 0; eMemoryEntryType < BMUXlib_TS_P_MemoryEntryType_eMax; eMemoryEntryType++ )
      {
         BDBG_MODULE_MSG( BMUXLIB_TS_MEMORY, ("Memory Type[%2d]: %5d (%7d/%7d bytes) - %s",
            eMemoryEntryType,
            hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[eMemoryEntryType].uiCount,
            hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[eMemoryEntryType].stMemoryConfig.stBufferInfo[0].uiSize,
            hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[eMemoryEntryType].stMemoryConfig.stBufferInfo[1].uiSize,
            BMUXlib_TS_P_MemoryTypeLUT[eMemoryEntryType]
            ));
      }
      BDBG_MODULE_MSG( BMUXLIB_TS_MEMORY, ("Total Memory: %7d/%7d bytes",
         hMuxTS->status.stMemoryConfigTotal.stMemoryConfig.stBufferInfo[0].uiSize,
         hMuxTS->status.stMemoryConfigTotal.stMemoryConfig.stBufferInfo[0].uiSize
         ));
   }
#endif

   BMUXlib_List_GetDefaultCreateSettings(&stDefaultListCreateSettings);

   uiSystemDataPendingListCount =  hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eSystemData].uiCount;
   if ( 0 != uiSystemDataPendingListCount )
   {
      /* Allocate System Data Pending List */
      stListCreateSettings = stDefaultListCreateSettings;
      stListCreateSettings.uiCount = uiSystemDataPendingListCount;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc = BMUXlib_List_Create(&hSystemDataPendingList, &stListCreateSettings);
      if (BERR_SUCCESS != rc)
      {
         if ( NULL != hSystemDataPendingList )
         {
            BMUXlib_List_Destroy( hSystemDataPendingList );
         }
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(rc);
      }

      hMuxTS->astSystemDataPendingListUnCached = (BMUXlib_TS_SystemData*) BMEM_Alloc( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,  sizeof( BMUXlib_TS_SystemData ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eSystemData].uiCount );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astSystemDataPendingListUnCached, (void**)&astSystemDataPendingList);
      if ( ( BERR_SUCCESS != rc ) || ( NULL == astSystemDataPendingList ) )
      {
         BMUXlib_List_Destroy( hSystemDataPendingList );
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }

      /* switch pending list from pre-Q to the newly allocated Q */
      hMuxTS->hSystemDataPendingList = hSystemDataPendingList;
      hMuxTS->astSystemDataPendingList = astSystemDataPendingList;
      hMuxTS->status.uiSystemDataMaxCount = stListCreateSettings.uiCount;
      hMuxTS->status.uiSystemDataPendingListWriteOffset = 0;
   } /* end: allocate system data pending list */

   if ( 0 != hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount )
   {
      /* Allocate Transport Descriptor Array (system memory) */
      hMuxTS->astTransportDescriptorUnCached = ( BMUXlib_TS_TransportDescriptor* ) BMEM_Alloc( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  sizeof( BMUXlib_TS_TransportDescriptor ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astTransportDescriptorUnCached, (void**)&hMuxTS->astTransportDescriptor );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astTransportDescriptor ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }
      BKNI_Memset(
               hMuxTS->astTransportDescriptor,
               0,
               sizeof( BMUXlib_TS_TransportDescriptor ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount
               );

      hMuxTS->astTransportDescriptorTempUnCached = ( BMUXlib_TS_TransportDescriptor* ) BMEM_Alloc( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  sizeof( BMUXlib_TS_TransportDescriptor ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astTransportDescriptorTempUnCached, (void**)&hMuxTS->astTransportDescriptorTemp );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astTransportDescriptorTemp ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }
      BKNI_Memset(
               hMuxTS->astTransportDescriptorTemp,
               0,
               sizeof( BMUXlib_TS_TransportDescriptor ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount
               );


      /* Allocate Transport Meta Data Array (system memory) */
      hMuxTS->astTransportDescriptorMetaDataUnCached = ( BMUXlib_TS_P_TransportDescriptorMetaData* ) BMEM_Alloc( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  sizeof( BMUXlib_TS_P_TransportDescriptorMetaData ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astTransportDescriptorMetaDataUnCached, (void**)&hMuxTS->astTransportDescriptorMetaData );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astTransportDescriptorMetaData ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }
      BKNI_Memset(
               hMuxTS->astTransportDescriptorMetaData,
               0,
               sizeof( BMUXlib_TS_P_TransportDescriptorMetaData ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount
               );

      /* Allocate Transport Descriptor Free List */
      stListCreateSettings = stDefaultListCreateSettings;

         /* it seems free list is shared among all transcport playback instances */
      stListCreateSettings.uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc = BMUXlib_List_Create(
                  &hMuxTS->hTransportDescriptorFreeList,
                  &stListCreateSettings
                  );
      if (BERR_SUCCESS != rc)
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(rc);
      }

      rc = BMUXlib_List_Create(
                  &hMuxTS->hTransportDescriptorMetaDataFreeList,
                  &stListCreateSettings
                  );
      if (BERR_SUCCESS != rc)
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(rc);
      }
   }

   if ( 0 != hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader].uiCount )
   {
      /* Allocate PES Header Array (device memory) */
      hMuxTS->astPESHeaderUnCached = ( BMUXlib_TS_P_PESHeader* ) BMEM_AllocAligned(
               hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
               sizeof( BMUXlib_TS_P_PESHeader ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader].uiCount,
               0,
               0
               );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astPESHeaderUnCached, (void**)&hMuxTS->astPESHeader );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astPESHeader ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
      }

      /* Allocate PES Header Free List */
      stListCreateSettings = stDefaultListCreateSettings;

      stListCreateSettings.uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader].uiCount;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc =  BMUXlib_List_Create(&hMuxTS->hPESHeaderFreeList, &stListCreateSettings);
      if (BERR_SUCCESS != rc)
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(rc);
      }
   }

   if ( 0 != hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount )
   {
      /* Allocate TS Packet Array (device memory) */
      hMuxTS->astTSPacketUnCached = ( BMUXlib_TS_P_TSPacket* ) BMEM_AllocAligned(
               hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
               sizeof( BMUXlib_TS_P_TSPacket ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount,
               0,
               0
               );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astTSPacketUnCached, (void**)&hMuxTS->astTSPacket);
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astTSPacket ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
      }

      /* Pre-populate with default TS Packet */
      for (i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount; i++ )
      {
         BKNI_Memcpy(
                  &hMuxTS->astTSPacket[i],
                  &s_stDefaultTSPacket,
                  sizeof( BMUXlib_TS_P_TSPacket )
                  );
      }
   }

   if ( 0 != hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount )
   {
      /* Allocate TS Packet Free List */
      stListCreateSettings = stDefaultListCreateSettings;

      stListCreateSettings.uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc = BMUXlib_List_Create(&hMuxTS->hTSPacketFreeList, &stListCreateSettings);
      if (BERR_SUCCESS != rc)
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(rc);
      }

      /* Allocate BPP Data Array (device memory) */
      hMuxTS->astBPPDataUnCached = ( BMUXlib_TS_P_BPPData* ) BMEM_AllocAligned(
               hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
               sizeof( BMUXlib_TS_P_BPPData ) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eBPP].uiCount,
               0,
               0
               );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astBPPDataUnCached, (void**)&hMuxTS->astBPPData);
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astBPPData ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
      }

      /* Pre-populate with default Video BPP Data */
      for ( i = 0; i < hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eBPP].uiCount; i++ )
      {
         BKNI_Memcpy(
                  &hMuxTS->astBPPData[i],
                  &s_stDefaultBPPData,
                  sizeof ( BMUXlib_TS_P_BPPData )
                  );

         BMUXlib_TS_P_BPPData_SetCommand(hMuxTS->astBPPData[i].data, 0x82); /* "LAST" BTP/BPP op-code*/
      }
   }

   if ( 0 != hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData].uiCount )
   {
      /* Allocate Userdata Free List
         NOTE: This assumes that there is a fixed amount of userdata
         processed per video frame - thus, the PES count supplied by the user
         is used to allocate the necessary free list for userdata. */
      stListCreateSettings = stDefaultListCreateSettings;

      stListCreateSettings.uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData].uiCount;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc = BMUXlib_List_Create(&hMuxTS->hUserdataFreeList, &stListCreateSettings);
      if (BERR_SUCCESS != rc)
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( rc );
      }

      hMuxTS->astUserdataPendingUnCached = (BMUXlib_TS_P_UserdataPending *) BMEM_Alloc( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  sizeof( BMUXlib_TS_P_UserdataPending ) * stListCreateSettings.uiCount );
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astUserdataPendingUnCached, (void**)&hMuxTS->astUserdataPending );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astUserdataPending ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }

      /* Allocate userdata PTS storage - need ONE per userdata PES pending */
      stListCreateSettings = stDefaultListCreateSettings;

      stListCreateSettings.uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData].uiCount;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc = BMUXlib_List_Create(&hMuxTS->hUserdataPTSFreeList, &stListCreateSettings);
      if (BERR_SUCCESS != rc)
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE( rc );
      }

      hMuxTS->astUserdataPTSUnCached = (BMUXlib_TS_P_UserdataPTSEntry *)BMEM_AllocAligned(hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
            sizeof(BMUXlib_TS_P_UserdataPTSEntry) * stListCreateSettings.uiCount, 0, 0);
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astUserdataPTSUnCached, (void**)&hMuxTS->astUserdataPTS );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astUserdataPTS ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }

      /* Allocate userdata local buffering for "unwrapping" packets
         (each userdata input has its own buffer) */
      hMuxTS->astUserdataUnwrapUnCached = (BMUXlib_TS_P_TSPacket *)(BMEM_AllocAligned(hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
            sizeof(BMUXlib_TS_P_TSPacket) * hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserDataUnwrap].uiCount, 0, 0));
      rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->astUserdataUnwrapUnCached, (void**)&hMuxTS->astUserdataUnwrap );
      if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->astUserdataUnwrap ) )
      {
         BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }

      /* userdata release Q (need one entry for each pending segment entry) ... */
      {
         uint32_t uiCount = hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserDataReleaseQ].uiCount;
         hMuxTS->pUserdataReleaseQFreeListBaseUnCached = (BMUXlib_TS_P_UserdataReleaseQEntry *) BMEM_Alloc( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  sizeof( BMUXlib_TS_P_UserdataReleaseQEntry ) * uiCount );
         rc = BMEM_ConvertAddressToCached( hMuxTS->stCreateSettings.hMem, hMuxTS->pUserdataReleaseQFreeListBaseUnCached, (void**)&hMuxTS->pUserdataReleaseQFreeListBase );
         if ( ( BERR_SUCCESS != rc ) || ( NULL == hMuxTS->pUserdataReleaseQFreeListBase ) )
         {
            BDBG_LEAVE(BMUXlib_TS_P_AllocateResources);
            return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
         }
      }
   } /* end: userdata free list allocation */

   BMUXlib_TS_P_ResetResources( hMuxTS );

   /* SW7425-3958: */
   if ( 0 != uiSystemDataPendingListCount )
   {
      /* Use the allocated system data list: Move any system data queued prior to start to this list buffers */
      /* NOTE: This must be done AFTER ResetResources() since that call will clear the new queue (and the newly
         copied data!) */
      size_t uiCount;
      size_t uiQueuedCount;

      BMUXlib_List_GetNumEntries(
         hMuxTS->hSystemDataPendingListPreQ,
         &uiCount
         );

      if ( 0 != uiCount )
      {
         rc = BMUXlib_TS_P_AddSystemDataBuffers(
                  hMuxTS,
                  hMuxTS->astSystemDataPendingListPreQ,
                  uiCount,
                  &uiQueuedCount
                  );

         BDBG_ASSERT( BERR_SUCCESS == rc );
         BDBG_ASSERT( uiCount == uiQueuedCount );
      }

      BMUXlib_List_Reset(
         hMuxTS->hSystemDataPendingListPreQ
         );
   }

   BDBG_LEAVE( BMUXlib_TS_P_AllocateResources );

   return BERR_TRACE( rc );
}

void
BMUXlib_TS_P_FreeResources(
   BMUXlib_TS_Handle hMuxTS
   )
{
   uint32_t i;

   BDBG_ENTER( BMUXlib_TS_P_FreeResources );

   /* cleanup input group and inputs - these are created by Start() */
   if (NULL != hMuxTS->status.hInputGroup)
   {
      BMUXlib_InputGroup_Destroy(hMuxTS->status.hInputGroup);
      hMuxTS->status.hInputGroup = NULL;
   }

   {
      unsigned uiInputIndex;

      for ( uiInputIndex = 0; uiInputIndex < BMUXLIB_TS_MAX_INPUT_PIDS; uiInputIndex++ )
      {
         if ( NULL != hMuxTS->status.stInputMetaData[uiInputIndex].hInput )
         {
            BMUXlib_Input_Destroy( hMuxTS->status.stInputMetaData[uiInputIndex].hInput );
            hMuxTS->status.stInputMetaData[uiInputIndex].hInput = NULL;
         }
      }
   }

#if BDBG_DEBUG_BUILD
#if 1
   /* Verify there aren't any pending buffers */

   for ( i = 0; i < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; i++ )
   {
      if ( true == hMuxTS->status.stTransportMetaData[i].bActive && NULL != hMuxTS->hTransportDescriptorPendingList[i])
      {
         if ( false == BMUXlib_List_IsEmpty( hMuxTS->hTransportDescriptorPendingList[i] ) )
         {
            BDBG_ERR(("Transport Descriptors Pending List[%d] not empty", i));
         }
      }
   }

   /* Verify all buffers have been returned to the free lists */
   {
      size_t uiCount;

      if ( NULL != hMuxTS->hTransportDescriptorFreeList )
      {
         BMUXlib_List_GetNumEntries(
                  hMuxTS->hTransportDescriptorFreeList,
                  &uiCount
                  );
         if ( hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount != uiCount )
         {
            BDBG_ERR(("All Transport Descriptors have not been freed (%d)", hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount - uiCount));
         }
      }

      if ( NULL != hMuxTS->hTransportDescriptorMetaDataFreeList )
      {
         BMUXlib_List_GetNumEntries(
                  hMuxTS->hTransportDescriptorMetaDataFreeList,
                  &uiCount
                  );
         if ( hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount != uiCount )
         {
            BDBG_ERR(("All Transport Metadata Descriptors have not been freed (%d)", hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor].uiCount - uiCount));
         }
      }

      if ( NULL != hMuxTS->hPESHeaderFreeList )
      {
         BMUXlib_List_GetNumEntries(
                  hMuxTS->hPESHeaderFreeList,
                  &uiCount
                  );
         if ( hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader].uiCount != uiCount )
         {
            BDBG_ERR(("All PES Headers have not been freed (%d)", hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_ePESHeader].uiCount - uiCount));
         }
      }

      if ( hMuxTS->hTSPacketFreeList )
      {
         BMUXlib_List_GetNumEntries(
                  hMuxTS->hTSPacketFreeList,
                  &uiCount
                  );
         if ( hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount != uiCount )
         {
            BDBG_ERR(("All TS Packets have not been freed (%d)", hMuxTS->status.stMemoryConfigTotal.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportPacket].uiCount - uiCount));
         }
      }
   }
#else
#warning "Check for pending descriptors on destroy has been disabled"
#endif
#endif

   if ( NULL != hMuxTS->astBPPDataUnCached )
   {
      BMEM_Free(
         hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
         hMuxTS->astBPPDataUnCached
         );
      hMuxTS->astBPPDataUnCached = NULL;
      hMuxTS->astBPPData = NULL;
   }

   if ( NULL != hMuxTS->astUserdataPendingUnCached )
   {
      BMEM_Free( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  hMuxTS->astUserdataPendingUnCached );
      hMuxTS->astUserdataPendingUnCached = NULL;
      hMuxTS->astUserdataPending = NULL;
   }

   if ( NULL != hMuxTS->pUserdataReleaseQFreeListBaseUnCached )
   {
      BMEM_Free( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,  hMuxTS->pUserdataReleaseQFreeListBaseUnCached );
      hMuxTS->pUserdataReleaseQFreeListBaseUnCached = NULL;
      hMuxTS->pUserdataReleaseQFreeListBase = NULL;
      hMuxTS->pUserdataReleaseQFreeList = NULL;
   }

   for (i = 0; i < BMUXLIB_TS_MAX_USERDATA_PIDS; i++)
   {
      if ( NULL != hMuxTS->hUserdataPendingList[i] )
      {
         BMUXlib_List_Destroy(hMuxTS->hUserdataPendingList[i]);
         hMuxTS->hUserdataPendingList[i] = NULL;
      }
   }

   if ( NULL != hMuxTS->hUserdataFreeList )
   {
      BMUXlib_List_Destroy(hMuxTS->hUserdataFreeList);
      hMuxTS->hUserdataFreeList = NULL;
   }

   if (NULL != hMuxTS->astUserdataPTSUnCached )
   {
      BMEM_Free(hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem, hMuxTS->astUserdataPTSUnCached);
      hMuxTS->astUserdataPTSUnCached = NULL;
      hMuxTS->astUserdataPTS = NULL;
   }

   if (NULL != hMuxTS->astUserdataUnwrapUnCached )
   {
      BMEM_Free(hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem, hMuxTS->astUserdataUnwrapUnCached);
      hMuxTS->astUserdataUnwrapUnCached = NULL;
      hMuxTS->astUserdataUnwrap = NULL;
   }

   if (NULL != hMuxTS->hUserdataPTSFreeList)
   {
      BMUXlib_List_Destroy(hMuxTS->hUserdataPTSFreeList);
      hMuxTS->hUserdataPTSFreeList = NULL;
   }

   if ( NULL != hMuxTS->astSystemDataPendingListUnCached )
   {
      BMEM_Free( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,  hMuxTS->astSystemDataPendingListUnCached );
      hMuxTS->astSystemDataPendingListUnCached = NULL;
      hMuxTS->astSystemDataPendingList = NULL;
   }

   if ( ( NULL != hMuxTS->hSystemDataPendingList )
        && ( hMuxTS->hSystemDataPendingListPreQ != hMuxTS->hSystemDataPendingList ) )
   {
      BMUXlib_List_Destroy(
            hMuxTS->hSystemDataPendingList
            );
      hMuxTS->hSystemDataPendingList = NULL;
   }

   if ( NULL != hMuxTS->hTSPacketFreeList )
   {
      BMUXlib_List_Destroy( hMuxTS->hTSPacketFreeList );

      hMuxTS->hTSPacketFreeList = NULL;
   }

   if ( NULL != hMuxTS->astTSPacketUnCached )
   {
      BMEM_Free(
               hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
               hMuxTS->astTSPacketUnCached
               );
      hMuxTS->astTSPacketUnCached = NULL;
      hMuxTS->astTSPacket = NULL;
   }

   if ( NULL != hMuxTS->hPESHeaderFreeList )
   {
      BMUXlib_List_Destroy( hMuxTS->hPESHeaderFreeList );

      hMuxTS->hPESHeaderFreeList = NULL;
   }

   if ( NULL != hMuxTS->astPESHeaderUnCached )
   {
      BMEM_Free(
               hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].hMem,
               hMuxTS->astPESHeaderUnCached
               );
      hMuxTS->astPESHeaderUnCached = NULL;
      hMuxTS->astPESHeader = NULL;
   }

   for ( i = 0; i < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; i++ )
   {
      if ( NULL != hMuxTS->hTransportDescriptorMetaDataPendingList[i] )
      {
         BMUXlib_List_Destroy( hMuxTS->hTransportDescriptorMetaDataPendingList[i] );

         hMuxTS->hTransportDescriptorMetaDataPendingList[i] = NULL;
      }

      if ( NULL != hMuxTS->hTransportDescriptorPendingList[i] )
      {
         BMUXlib_List_Destroy( hMuxTS->hTransportDescriptorPendingList[i] );

         hMuxTS->hTransportDescriptorPendingList[i] = NULL;
      }
   }

   if ( NULL != hMuxTS->hTransportDescriptorMetaDataFreeList )
   {
      BMUXlib_List_Destroy( hMuxTS->hTransportDescriptorMetaDataFreeList );

      hMuxTS->hTransportDescriptorMetaDataFreeList = NULL;
   }

   if ( NULL != hMuxTS->hTransportDescriptorFreeList )
   {
      BMUXlib_List_Destroy( hMuxTS->hTransportDescriptorFreeList );

      hMuxTS->hTransportDescriptorFreeList = NULL;
   }

   if ( NULL != hMuxTS->astTransportDescriptorMetaDataUnCached )
   {
      BMEM_Free( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,
               hMuxTS->astTransportDescriptorMetaDataUnCached
               );
      hMuxTS->astTransportDescriptorMetaDataUnCached = NULL;
      hMuxTS->astTransportDescriptorMetaData = NULL;
   }

   if ( NULL != hMuxTS->astTransportDescriptorTempUnCached )
   {
      BMEM_Free( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,
               hMuxTS->astTransportDescriptorTempUnCached
               );
      hMuxTS->astTransportDescriptorTempUnCached = NULL;
      hMuxTS->astTransportDescriptorTemp = NULL;
   }

   if ( NULL != hMuxTS->astTransportDescriptorUnCached )
   {
      BMEM_Free( hMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eSystem].hMem,
               hMuxTS->astTransportDescriptorUnCached
               );
      hMuxTS->astTransportDescriptorUnCached = NULL;
      hMuxTS->astTransportDescriptor = NULL;
   }

   BDBG_LEAVE( BMUXlib_TS_P_FreeResources );
}

void
BMUXlib_TS_P_Reset(
   BMUXlib_TS_Handle hMuxTS
   )
{
   BDBG_ENTER( BMUXlib_TS_P_Reset );

#ifdef BMUXLIB_TS_P_TEST_MODE
   if (NULL != hMuxTS->status.stSystemDataInfo.hCSVFile)
      fclose(hMuxTS->status.stSystemDataInfo.hCSVFile);
   if (NULL != hMuxTS->status.stSystemDataInfo.hDataFile)
      fclose(hMuxTS->status.stSystemDataInfo.hDataFile);
#endif

   BKNI_Memset( &hMuxTS->status, 0, sizeof( hMuxTS->status ) );

   BMUXlib_TS_GetDefaultMuxSettings(
            &hMuxTS->status.stMuxSettings
            );

   BMUXlib_TS_GetDefaultStartSettings(
            &hMuxTS->status.stStartSettings
            );

   /* return the system data pending list to using the pre-Q */
   hMuxTS->hSystemDataPendingList = hMuxTS->hSystemDataPendingListPreQ;
   hMuxTS->astSystemDataPendingList = hMuxTS->astSystemDataPendingListPreQ;
   hMuxTS->status.uiSystemDataMaxCount = BMUXLIB_TS_P_SYSTEM_DATA_PRE_Q_COUNT;
   BMUXlib_List_Reset(
      hMuxTS->hSystemDataPendingList
      );

   BDBG_LEAVE( BMUXlib_TS_P_Reset );
}

BERR_Code
BMUXlib_TS_Create(
         BMUXlib_TS_Handle *phMuxTS,  /* [out] TSMuxer handle returned */
         const BMUXlib_TS_CreateSettings *pstCreateSettings
         )
{
   BERR_Code rc = BERR_UNKNOWN;
   BMUXlib_TS_P_Context *pMuxTS;

   BDBG_ENTER( BMUXlib_TS_Create );

   BDBG_ASSERT( phMuxTS );
   BDBG_ASSERT( pstCreateSettings );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CREATESETTINGS == pstCreateSettings->uiSignature );

   /************************/
   /* Create MUX TS Handle */
   /************************/

   /* Set the handle to NULL in case the allocation fails */
   *phMuxTS = NULL;

   if (/* verify the required hMem handle is provided ... */
      /* NOTE: if software XPT ever used via the transport abstraction, then
         this _could_ be NULL, and Malloc should be used instead */
      (NULL == pstCreateSettings->hMem)
      )
   {
      BDBG_LEAVE(BMUXlib_TS_Create);
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   /* Allocate MUX TS Context (system memory) */
   pMuxTS = ( BMUXlib_TS_P_Context* ) BKNI_Malloc( sizeof( BMUXlib_TS_P_Context ) );
   if ( NULL == pMuxTS )
   {
      BDBG_LEAVE(BMUXlib_TS_Create);
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   /* Zero out the newly allocated context */
   BKNI_Memset(
            pMuxTS,
            0,
            sizeof( BMUXlib_TS_P_Context )
            );

   /* Set Signature */
   pMuxTS->uiSignature = BMUXLIB_TS_P_SIGNATURE_CONTEXT;

   /* Copy user specified settings */
   BKNI_Memcpy(
            &pMuxTS->stCreateSettings,
            pstCreateSettings,
            sizeof( BMUXlib_TS_CreateSettings )
            );

   /* Allocate sub heap */
   {
      BMUXlib_TS_P_BufferConfig stMemoryConfig;
      BKNI_Memset( &stMemoryConfig, 0, sizeof( BMUXlib_TS_P_BufferConfig ) );
      BMUXlib_TS_P_GetBufferConfigFromMemoryConfig( &pMuxTS->stCreateSettings.stMemoryConfig, &pMuxTS->stCreateSettings.stMemoryBuffers, &stMemoryConfig );

      rc = BMUXlib_TS_P_AllocateMemory( pMuxTS, pMuxTS->stCreateSettings.hMem, &stMemoryConfig );

      if ( BERR_SUCCESS != rc )
      {
         BMUXlib_TS_Destroy( pMuxTS );
         BDBG_LEAVE(BMUXlib_TS_Create);
         return BERR_TRACE(rc);
      }
   }

   /* Allocate System Data Pending List pre-Q */
   {
      BMUXlib_List_CreateSettings stListCreateSettings;
      BMUXlib_List_GetDefaultCreateSettings(&stListCreateSettings);

      stListCreateSettings.uiCount = BMUXLIB_TS_P_SYSTEM_DATA_PRE_Q_COUNT;
      stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

      rc = BMUXlib_List_Create(&pMuxTS->hSystemDataPendingListPreQ, &stListCreateSettings);
      if (BERR_SUCCESS != rc)
      {
         BMUXlib_TS_Destroy( pMuxTS );
         BDBG_LEAVE(BMUXlib_TS_Create);
         return BERR_TRACE(rc);
      }

      BDBG_ASSERT( pMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].uiSize > ( BMUXLIB_TS_P_SYSTEM_DATA_PRE_Q_COUNT * sizeof( BMUXlib_TS_SystemData ) ) );

      pMuxTS->astSystemDataPendingListPreQ = (BMUXlib_TS_SystemData *) ( (unsigned) pMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].pBufferCached + ( pMuxTS->stSubHeap[BMUXlib_TS_P_MemoryType_eShared].uiSize - ( BMUXLIB_TS_P_SYSTEM_DATA_PRE_Q_COUNT * sizeof( BMUXlib_TS_SystemData ) ) ) );
   }

   /* Create Mutex */
   rc = BKNI_CreateMutex( &pMuxTS->hMutex );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error creating mutex"));
      BMUXlib_TS_Destroy( pMuxTS );
      BDBG_LEAVE(BMUXlib_TS_Create);
      return BERR_TRACE(rc);
   }

   /* Reset State */
   BMUXlib_TS_P_Reset( pMuxTS );

   /* Provide handle to caller */
   *phMuxTS = pMuxTS;
   BDBG_LEAVE(BMUXlib_TS_Create);
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_Destroy(
         BMUXlib_TS_Handle hMuxTS
         )
{
   BDBG_ENTER(BMUXlib_TS_Destroy);
   BDBG_ASSERT( hMuxTS );
   /* the following signifies an attempt to free up something that was either
      a) not created by Create()
      b) has already been destroyed
   */
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   /* SW7425-3642: Stop the mux if it hasn't already been stopped
      - this is necessary since Stop() now frees resources */
   if (BMUXlib_State_eStopped != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS))
   {
      BERR_Code rc = BMUXlib_TS_Stop(hMuxTS);
      if (rc != BERR_SUCCESS)
      {
         BDBG_LEAVE(BMUXlib_TS_Destroy);
         return BERR_TRACE(rc);
      }
   }

   /* Destroy Mutex */
   if ( NULL != hMuxTS->hMutex )
   {
      BKNI_DestroyMutex( hMuxTS->hMutex );
      hMuxTS->hMutex= NULL;
   }

   /* Free System Data Pending List pre-Q */
   if ( NULL != hMuxTS->hSystemDataPendingListPreQ )
   {
      hMuxTS->astSystemDataPendingListPreQ = NULL;

      BMUXlib_List_Destroy( hMuxTS->hSystemDataPendingListPreQ );
      hMuxTS->hSystemDataPendingListPreQ = NULL;
   }

   BMUXlib_TS_P_FreeMemory(
      hMuxTS,
      hMuxTS->stCreateSettings.hMem
      );

   /* the following prevents accidental reuse of the context */
   hMuxTS->uiSignature = 0;
   BKNI_Free ( hMuxTS );

   BDBG_LEAVE(BMUXlib_TS_Destroy);
   return BERR_TRACE( BERR_SUCCESS );
}

/****************/
/* Mux Settings */
/****************/
void
BMUXlib_TS_GetDefaultMuxSettings(
         BMUXlib_TS_MuxSettings *pstMuxSettings
         )
{
   BDBG_ENTER( BMUXlib_TS_GetDefaultMuxSettings );

   BDBG_ASSERT( pstMuxSettings );

   BKNI_Memset(
            pstMuxSettings,
            0,
            sizeof( BMUXlib_TS_MuxSettings )
            );

   pstMuxSettings->uiSignature = BMUXLIB_TS_P_SIGNATURE_MUXSETTINGS;
   pstMuxSettings->uiSystemDataBitRate = 1000000;

   BDBG_LEAVE( BMUXlib_TS_GetDefaultMuxSettings );
   return;
}

BERR_Code
BMUXlib_TS_SetMuxSettings(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_MuxSettings *pstMuxSettings
         )
{
   BDBG_ENTER( BMUXlib_TS_SetMuxSettings );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( pstMuxSettings );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_MUXSETTINGS == pstMuxSettings->uiSignature );

   if ((pstMuxSettings->uiSystemDataBitRate > BMUXLIB_TS_MAX_SYS_DATA_BR)
      || (pstMuxSettings->uiSystemDataBitRate < BMUXLIB_TS_MIN_SYS_DATA_BR))
      return BERR_TRACE( BERR_INVALID_PARAMETER );

   hMuxTS->status.stMuxSettings = *pstMuxSettings;

   BDBG_LEAVE( BMUXlib_TS_SetMuxSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BMUXlib_TS_GetMuxSettings(
         BMUXlib_TS_Handle hMuxTS,
         BMUXlib_TS_MuxSettings *pstMuxSettings
         )
{
   BDBG_ENTER( BMUXlib_TS_GetMuxSettings );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( pstMuxSettings );

   *pstMuxSettings = hMuxTS->status.stMuxSettings;

   BDBG_LEAVE( BMUXlib_TS_GetMuxSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

/**************/
/* Start/Stop */
/**************/
void
BMUXlib_TS_GetDefaultStartSettings(
         BMUXlib_TS_StartSettings *pstStartSettings
         )
{
   BDBG_ENTER( BMUXlib_TS_GetDefaultStartSettings );

   BDBG_ASSERT( pstStartSettings );

   BKNI_Memset(
            pstStartSettings,
            0,
            sizeof( BMUXlib_TS_StartSettings )
            );

   pstStartSettings->uiSignature = BMUXLIB_TS_P_SIGNATURE_STARTSETTINGS;
   pstStartSettings->uiServiceLatencyTolerance = 20;
   pstStartSettings->uiServicePeriod = BMUXLIB_TS_P_MUX_SERVICE_PERIOD_DEFAULT;
   pstStartSettings->stPCRData.uiInterval = 50;

   BDBG_LEAVE( BMUXlib_TS_GetDefaultStartSettings );
   return;
}

/* BMUXlib_TS_Start - Configures the mux HW */
BERR_Code
BMUXlib_TS_Start(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_StartSettings *pstStartSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BMUXlib_TS_Start );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( pstStartSettings );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_STARTSETTINGS == pstStartSettings->uiSignature );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   /* verify required settings are present ... */
   if (( NULL == pstStartSettings->transport.stDeviceInterface.fGetTransportSettings)
         || (NULL == pstStartSettings->transport.stDeviceInterface.fSetTransportSettings)
         || (NULL == pstStartSettings->transport.stDeviceInterface.fGetTransportStatus))
   {
      BDBG_LEAVE( BMUXlib_TS_Start );
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* verify PCR parameters */
   if ((pstStartSettings->stPCRData.uiPID == 0) ||
       (pstStartSettings->stPCRData.uiInterval == 0))
   {
      BDBG_LEAVE( BMUXlib_TS_Start );
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if (pstStartSettings->stPCRData.uiInterval > 100)
      BDBG_WRN(("PCR Interval exceeds maximum of 100ms (as specified by ISO/IEC-13818-1)"));

   BKNI_AcquireMutex( hMuxTS->hMutex );

   /* Initialize state only if mux is stopped */
   if ( BMUXlib_State_eStopped == BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
   {
      unsigned uiPIDIndex;
      uint32_t aPIDTable[BMUXLIB_TS_MAX_VIDEO_PIDS+BMUXLIB_TS_MAX_AUDIO_PIDS]; /* table of PIDs found */
      BMUXlib_Input_Handle aInputTable[BMUXLIB_TS_MAX_VIDEO_PIDS+BMUXLIB_TS_MAX_AUDIO_PIDS];
      uint32_t i;
      BMUXlib_Input_CreateSettings stInputCreateSettings;
      BMUXlib_TS_P_BufferConfig stMemoryConfigLocal;
      uint32_t uiTransportChannelIndex;
      uint32_t uiNumChannels = 0;
      uint32_t aTransportDescriptorPendingListCountTable[BMUXLIB_TS_MAX_TRANSPORT_INSTANCES];

      /* NOTE: same count is used for both PendingList and MetadataPendingList */
      BKNI_Memset(aTransportDescriptorPendingListCountTable, 0, sizeof(aTransportDescriptorPendingListCountTable));

      for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
      {
         const BMUXlib_TS_TransportChannelInterface *pChannelInterface = &pstStartSettings->transport.stChannelInterface[uiTransportChannelIndex];
         hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] = 0xFFFFFFFF;
         /* count valid channel interfaces */
         if ((NULL != pChannelInterface->fAddTransportDescriptors)
            && (NULL != pChannelInterface->fGetCompletedTransportDescriptors))
            uiNumChannels++;
      }

      /* verify at least one channel is provided ... */
      if (0 == uiNumChannels)
      {
         BMUXlib_TS_P_FreeResources( hMuxTS );
         BMUXlib_TS_P_Reset( hMuxTS );    /* reset state since Expected ESCRs modified above */
         BKNI_ReleaseMutex( hMuxTS->hMutex );
         BDBG_LEAVE( BMUXlib_TS_Start );
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      BKNI_Memset(aPIDTable, 0, sizeof(aPIDTable));

      hMuxTS->status.stStartSettings = *pstStartSettings;
      if ((hMuxTS->status.stStartSettings.uiNumValidVideoPIDs > BMUXLIB_TS_MAX_VIDEO_PIDS)
         || (hMuxTS->status.stStartSettings.uiNumValidAudioPIDs > BMUXLIB_TS_MAX_AUDIO_PIDS)
         || (hMuxTS->status.stStartSettings.uiNumValidUserdataPIDs > BMUXLIB_TS_MAX_USERDATA_PIDS))
         BDBG_WRN(("Specified PID counts exceeds capabilities - excess ignored", NULL));

      if (hMuxTS->status.stStartSettings.uiServicePeriod == 0)
      {
         hMuxTS->status.stStartSettings.uiServicePeriod = BMUXLIB_TS_P_MUX_SERVICE_PERIOD_DEFAULT;
         BDBG_WRN(("Service period of 0 ms specified. Using default of %d ms", hMuxTS->status.stStartSettings.uiServicePeriod));
      }

      BMUXlib_TS_P_GetBufferConfigFromMemoryConfig( &hMuxTS->stCreateSettings.stMemoryConfig, &hMuxTS->stCreateSettings.stMemoryBuffers, &stMemoryConfigLocal );

      {
         /* Get the current memory config */
         BMUXlib_TS_MuxConfig stMuxConfig;
         BMUXlib_TS_P_MemoryConfig stMemoryConfig;

         BKNI_Memset( &stMuxConfig, 0, sizeof( BMUXlib_TS_MuxConfig ) );

         stMuxConfig.stMuxStartSettings = hMuxTS->status.stStartSettings;
         stMuxConfig.stMuxSettings = hMuxTS->status.stMuxSettings;

         BMUXlib_TS_P_GetMemoryConfig(
            &stMuxConfig,
            &stMemoryConfig
         );

         {
            BMUXlib_TS_P_MemoryType eMemoryType;

            for ( eMemoryType = 0; (eMemoryType < BMUXlib_TS_P_MemoryType_eMax) && ( BERR_SUCCESS == rc); eMemoryType++ )
            {
               if ( stMemoryConfigLocal.stBufferInfo[eMemoryType].uiSize < stMemoryConfig.stMemoryConfig.stBufferInfo[eMemoryType].uiSize )
               {
                  BDBG_ERR(("Not enough memory of type [%d] for requested configuration. (%d < %d)",
                     eMemoryType,
                     stMemoryConfigLocal.stBufferInfo[eMemoryType].uiSize,
                     stMemoryConfig.stMemoryConfig.stBufferInfo[eMemoryType].uiSize
                     ));

                  rc = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
               }
            }
         }

         if ( BERR_SUCCESS == rc )
         {
            /* Allocate resources from sub heap */
            rc = BMUXlib_TS_P_AllocateResources( hMuxTS, &stMemoryConfig );
            if ( BERR_SUCCESS == rc )
            {
               BMUXlib_TS_P_MemoryType eMemoryType;

               for ( eMemoryType = 0; (eMemoryType < BMUXlib_TS_P_MemoryType_eMax); eMemoryType++ )
               {
                  BMEM_HeapInfo stHeapInfo;

                  BMEM_GetHeapInfo( hMuxTS->stSubHeap[eMemoryType].hMem, &stHeapInfo);

                  BDBG_MODULE_MSG( BMUXLIB_TS_MEMORY, ("[%d] Total Required/Used (Slack): %u/%u (%d)",
                     eMemoryType,
                     stMemoryConfigLocal.stBufferInfo[eMemoryType].uiSize,
                     stHeapInfo.ulTotalAllocated,
                     stMemoryConfigLocal.stBufferInfo[eMemoryType].uiSize - stHeapInfo.ulTotalAllocated
                     ));
               }
            }
         }
      }
      if (rc != BERR_SUCCESS)
      {
         BMUXlib_TS_P_FreeResources( hMuxTS );
         BMUXlib_TS_P_Reset( hMuxTS );    /* reset state since Expected ESCRs modified above */
         BKNI_ReleaseMutex( hMuxTS->hMutex );
         BDBG_LEAVE( BMUXlib_TS_Start );
         return rc;
      }

      for ( uiPIDIndex = 0; (BERR_SUCCESS == rc) && (uiPIDIndex < BMUXLIB_TS_MAX_VIDEO_PIDS) && (uiPIDIndex < hMuxTS->status.stStartSettings.uiNumValidVideoPIDs); uiPIDIndex++ )
      {
         BMUXlib_VideoEncoderInterface *pEncoderInterface = &hMuxTS->status.stStartSettings.video[uiPIDIndex].stInputInterface;
         unsigned uiTransportChannelIndex = hMuxTS->status.stStartSettings.video[uiPIDIndex].uiTransportChannelIndex;

         BKNI_Memset( &hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs], 0, sizeof( BMUXlib_TS_P_InputMetaData ) );

         {
            BMUXlib_TS_TransportChannelInterface *pChannelInterface;
            pChannelInterface = &hMuxTS->status.stStartSettings.transport.stChannelInterface[hMuxTS->status.stStartSettings.video[uiPIDIndex].uiTransportChannelIndex];
            if ( ( NULL == pEncoderInterface->fGetBufferDescriptors )
                 || ( NULL == pEncoderInterface->fGetBufferStatus )
                 || ( NULL == pEncoderInterface->fConsumeBufferDescriptors )
                 || ( NULL == pChannelInterface->fAddTransportDescriptors )
                 || ( NULL == pChannelInterface->fGetCompletedTransportDescriptors )
               )
            {
               rc = BERR_TRACE(BERR_INVALID_PARAMETER);
               break;
            }
         }

         /* verify PID is not already in use ... */
         for (i = 0; i < hMuxTS->status.uiNumInputs; i++)
            if (hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPID == aPIDTable[i])
               break;
         if (i != hMuxTS->status.uiNumInputs)
         {
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
         }

         BMUXlib_Input_GetDefaultCreateSettings( &stInputCreateSettings );
         stInputCreateSettings.eType = BMUXlib_Input_Type_eVideo;
         stInputCreateSettings.interface.stVideo = *pEncoderInterface;
         stInputCreateSettings.eBurstMode = BMUXlib_Input_BurstMode_eDescriptor;
         stInputCreateSettings.pMetadata = &hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs];
         stInputCreateSettings.uiMuxId = hMuxTS->stCreateSettings.uiMuxId;
         stInputCreateSettings.uiTypeInstance = uiPIDIndex;
         /* by default, this input will be marked as "active" */
         rc = BERR_TRACE( BMUXlib_Input_Create( &hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].hInput,
                         &stInputCreateSettings
                        ));
         if (BERR_SUCCESS != rc)
            break;

         if ( 0 == hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPESStreamID )
         {
            BDBG_WRN(("video[%d] stream ID is invalid. Using %02x instead", uiPIDIndex, 0xE0+uiPIDIndex));
            hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPESStreamID = 0xE0 + uiPIDIndex;
         }

         BMUXlib_TS_P_BPPData_SetStreamID(
                     hMuxTS->astBPPData[hMuxTS->status.uiNumInputs].data,
                     hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPESStreamID
                     );

         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiInputIndex = hMuxTS->status.uiNumInputs;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiTransportChannelIndex = hMuxTS->status.stStartSettings.video[uiPIDIndex].uiTransportChannelIndex;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiPID = hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPID;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiPESStreamID = hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPESStreamID;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].pBPPBuffer = hMuxTS->astBPPData[hMuxTS->status.uiNumInputs].data;

         /* setup userdata companion video info */
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].pstUserdata = &hMuxTS->status.stUserdataVideoInfo[uiPIDIndex];

         hMuxTS->status.stTransportMetaData[hMuxTS->status.stStartSettings.video[uiPIDIndex].uiTransportChannelIndex].bActive = true;

         aInputTable[hMuxTS->status.uiNumInputs] = hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].hInput;
         /* save current PID for later PID collision detection ...*/
         aPIDTable[hMuxTS->status.uiNumInputs] = hMuxTS->status.stStartSettings.video[uiPIDIndex].uiPID;

         /* Determine amount of Transport Descriptor Pending List Entries (Video) */
         /* (A single allocation for the lists is done once all count requirements are established) */
         aTransportDescriptorPendingListCountTable[uiTransportChannelIndex] += BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP(hMuxTS->status.stMemoryConfig.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eVideo].uiCount, hMuxTS->status.stStartSettings.uiNumValidVideoPIDs);

         hMuxTS->status.uiNumInputs++;
      }  /* end: for each video input */

      for ( uiPIDIndex = 0; (BERR_SUCCESS == rc) && (uiPIDIndex < BMUXLIB_TS_MAX_AUDIO_PIDS) && (uiPIDIndex < hMuxTS->status.stStartSettings.uiNumValidAudioPIDs); uiPIDIndex++ )
      {
         BMUXlib_AudioEncoderInterface *pEncoderInterface = &hMuxTS->status.stStartSettings.audio[uiPIDIndex].stInputInterface;
         unsigned uiTransportChannelIndex = hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiTransportChannelIndex;

         BKNI_Memset( &hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs], 0, sizeof( BMUXlib_TS_P_InputMetaData ) );

         {
            BMUXlib_TS_TransportChannelInterface *pChannelInterface;
            pChannelInterface = &hMuxTS->status.stStartSettings.transport.stChannelInterface[hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiTransportChannelIndex];

            if ( ( NULL == pEncoderInterface->fGetBufferDescriptors )
                 || ( NULL == pEncoderInterface->fGetBufferStatus )
                 || ( NULL == pEncoderInterface->fConsumeBufferDescriptors )
                 || ( NULL == pChannelInterface->fAddTransportDescriptors )
                 || ( NULL == pChannelInterface->fGetCompletedTransportDescriptors )
               )
            {
               rc = BERR_TRACE(BERR_INVALID_PARAMETER);
               break;
            }
         }

         /* verify PID is not already in use ... */
         for (i = 0; i < hMuxTS->status.uiNumInputs; i++)
            if (hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPID == aPIDTable[i])
               break;
         if (i != hMuxTS->status.uiNumInputs)
         {
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
         }

         BMUXlib_Input_GetDefaultCreateSettings( &stInputCreateSettings );
         stInputCreateSettings.eType = BMUXlib_Input_Type_eAudio;
         stInputCreateSettings.interface.stAudio = *pEncoderInterface;
         stInputCreateSettings.eBurstMode = BMUXlib_Input_BurstMode_eFrame;
         stInputCreateSettings.pMetadata = &hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs];
         stInputCreateSettings.uiMuxId = hMuxTS->stCreateSettings.uiMuxId;
         stInputCreateSettings.uiTypeInstance = uiPIDIndex;
         /* by default, this input will be marked as "active" */
         rc = BERR_TRACE( BMUXlib_Input_Create( &hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].hInput,
                         &stInputCreateSettings
                        ));
         if (BERR_SUCCESS != rc)
            break;

         if ( 0 == hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPESStreamID )
         {
            BDBG_WRN(("audio[%d] stream ID is invalid. Using %02x instead", uiPIDIndex, 0xC0+uiPIDIndex));
            hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPESStreamID = 0xC0 + uiPIDIndex;
         }

         BMUXlib_TS_P_BPPData_SetStreamID(
                     hMuxTS->astBPPData[hMuxTS->status.uiNumInputs].data,
                     hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPESStreamID
                     );

         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiInputIndex = hMuxTS->status.uiNumInputs;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiTransportChannelIndex = hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiTransportChannelIndex;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiPID = hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPID;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].uiPESStreamID = hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPESStreamID;
         hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].pBPPBuffer = hMuxTS->astBPPData[hMuxTS->status.uiNumInputs].data;

         hMuxTS->status.stTransportMetaData[hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiTransportChannelIndex].bActive = true;

         aInputTable[hMuxTS->status.uiNumInputs] = hMuxTS->status.stInputMetaData[hMuxTS->status.uiNumInputs].hInput;
         /* save current PID for later PID collision detection ...*/
         aPIDTable[hMuxTS->status.uiNumInputs] = hMuxTS->status.stStartSettings.audio[uiPIDIndex].uiPID;

         /* Determine amount of Transport Descriptor Pending List Entries (Audio) */
         /* (A single allocation for the lists is done once all count requirements are established) */
         aTransportDescriptorPendingListCountTable[uiTransportChannelIndex] += BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP(hMuxTS->status.stMemoryConfig.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eAudio].uiCount, hMuxTS->status.stStartSettings.uiNumValidAudioPIDs);

         hMuxTS->status.uiNumInputs++;
      }  /* end: for each audio input */

      if (0 != hMuxTS->status.uiNumInputs)
      {
         if (BERR_SUCCESS == rc)
         {
            uint32_t uiTransportChannelIndex = hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex;
            BMUXlib_List_CreateSettings stListCreateSettings;

            /* check specified channel for system data is valid ... */
            if ((NULL == hMuxTS->status.stStartSettings.transport.stChannelInterface[uiTransportChannelIndex].fAddTransportDescriptors )
               || (NULL == hMuxTS->status.stStartSettings.transport.stChannelInterface[uiTransportChannelIndex].fGetCompletedTransportDescriptors ))
            {
               rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            }


            /* Determine amount of Transport Descriptor Pending List Entries (System) */
            /* (A single allocation for the lists is done once all count requirements are established) */
            aTransportDescriptorPendingListCountTable[uiTransportChannelIndex] += hMuxTS->status.stMemoryConfig.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eTransportDescriptor][BMUXlib_TS_P_InputType_eSystem].uiCount;
            hMuxTS->status.stTransportMetaData[uiTransportChannelIndex].bActive = true;

            /* Allocate Transport Descriptor Pending Lists */
            for (i = 0; rc == BERR_SUCCESS && i < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; i++)
            {
               if (false == hMuxTS->status.stTransportMetaData[i].bActive)
                  continue;   /* this transport channel is not active */

               BMUXlib_List_GetDefaultCreateSettings(&stListCreateSettings);

               stListCreateSettings.uiCount = aTransportDescriptorPendingListCountTable[i];
               stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;

               /* active channels must have descriptor lists */
               BDBG_ASSERT(0 != stListCreateSettings.uiCount);

               rc = BMUXlib_List_Create(
                        &hMuxTS->hTransportDescriptorPendingList[i],
                        &stListCreateSettings
                        );
               if ( BERR_SUCCESS == rc )
               {
                  rc = BMUXlib_List_Create(
                           &hMuxTS->hTransportDescriptorMetaDataPendingList[i],
                           &stListCreateSettings
                           );
               }
            }
         }

         if (BERR_SUCCESS == rc)
         {
            BMUXlib_List_CreateSettings stListCreateSettings;
            BMUXlib_List_GetDefaultCreateSettings(&stListCreateSettings);

            /* Allocate Userdata Pending List for each userdata input */
            stListCreateSettings.uiCount = BMUXLIB_TS_P_DIVIDE_WITH_ROUND_UP(hMuxTS->status.stMemoryConfig.astMemoryEntry[BMUXlib_TS_P_MemoryEntryType_eUserData][BMUXlib_TS_P_InputType_eSystem].uiCount, hMuxTS->status.stStartSettings.uiNumValidUserdataPIDs);
            stListCreateSettings.eType = BMUXlib_List_Type_eFIFO;
            for (i = 0; i < hMuxTS->status.stStartSettings.uiNumValidUserdataPIDs; i++)
            {
               rc = BMUXlib_List_Create(&hMuxTS->hUserdataPendingList[i], &stListCreateSettings);
               if (BERR_SUCCESS != rc)
               {
                  BERR_TRACE( rc );
                  break;
               }
            }
         }

         /* create the input group for the active inputs */
         if (BERR_SUCCESS == rc)
         {
            BMUXlib_InputGroup_CreateSettings stCreateSettings;
            BMUXlib_InputGroup_GetDefaultCreateSettings(&stCreateSettings);
            stCreateSettings.uiInputCount = hMuxTS->status.uiNumInputs;
            stCreateSettings.pInputTable = aInputTable;
            rc = BMUXlib_InputGroup_Create(&hMuxTS->status.hInputGroup, &stCreateSettings);
            if (BERR_SUCCESS == rc)
            {
               BMUXlib_InputGroup_Settings stSettings;
               BMUXlib_InputGroup_GetSettings(hMuxTS->status.hInputGroup, &stSettings);
               /* set the selector function to be used by InputGroup_GetNextInput() */
               stSettings.fSelector = BMUXlib_InputGroup_DescriptorSelectLowestESCR;
               rc = BMUXlib_InputGroup_SetSettings(hMuxTS->status.hInputGroup, &stSettings);
            }

            /* userdata */
            if (BERR_SUCCESS == rc)
               rc = BMUXlib_TS_P_UserdataInit(hMuxTS);
         }
         if ( BERR_SUCCESS == rc )
         {
            /* start the mux ... */
            BMUXLIB_TS_P_SET_MUX_STATE(hMuxTS, BMUXlib_State_eStarted);
         }
         else
         {
            /* Startup failed ... cleanup anything that may have been created */
            BMUXlib_TS_P_FreeResources( hMuxTS );
            BMUXlib_TS_P_Reset( hMuxTS );
         }
      }
      else
      {
         /* not enough inputs supplied */
         BMUXlib_TS_P_FreeResources( hMuxTS );
         BMUXlib_TS_P_Reset( hMuxTS );
         rc = BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   } /* end: if state = stopped */
   else
   {
      rc = BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   BKNI_ReleaseMutex( hMuxTS->hMutex );
   BDBG_LEAVE( BMUXlib_TS_Start );
   return rc;
}

void
BMUXlib_TS_GetDefaultFinishSettings(
         BMUXlib_TS_FinishSettings *pstFinishSettings
         )
{
   BDBG_ENTER( BMUXlib_TS_GetDefaultFinishSettings );

   BDBG_ASSERT( pstFinishSettings );

   BKNI_Memset(
            pstFinishSettings,
            0,
            sizeof( BMUXlib_TS_FinishSettings )
            );

   pstFinishSettings->uiSignature = BMUXLIB_TS_P_SIGNATURE_FINISHSETTINGS;
   pstFinishSettings->eFinishMode = BMUXlib_FinishMode_ePrepareForStop;

   BDBG_LEAVE( BMUXlib_TS_GetDefaultFinishSettings );
   return;
}

BERR_Code
BMUXlib_TS_Finish(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_FinishSettings *pstFinishSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BMUXlib_TS_Finish );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( pstFinishSettings );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_FINISHSETTINGS == pstFinishSettings->uiSignature );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   switch ( BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
   {
      case BMUXlib_State_eStarted:
         {
            hMuxTS->status.stFinishSettings = *pstFinishSettings;
            switch ( hMuxTS->status.stFinishSettings.eFinishMode )
            {
               case BMUXlib_FinishMode_ePrepareForStop:
                  BDBG_MODULE_MSG( BMUXLIB_TS_FINISH, ("Finishing Input") );
#if defined(HUMAX_PLATFORM_BASE)
                  /* Stop requested, no longer retrieving data from encoder(s), finalizing output */
                  BMUXLIB_TS_P_SET_MUX_STATE(hMuxTS, BMUXlib_State_eFinishingOutput);
#else                  
                  BMUXLIB_TS_P_SET_MUX_STATE(hMuxTS, BMUXlib_State_eFinishingInput);
#endif              
                  break;

               default:
                  BDBG_ERR(("Unknown finish mode specified"));
                  rc = BERR_TRACE( BERR_INVALID_PARAMETER );
                  break;
            }
         }
         break;

      case BMUXlib_State_eFinishingInput:
      case BMUXlib_State_eFinishingOutput:
      case BMUXlib_State_eFinished:
         /* do nothing if invoked from these states - already finishing! */
         rc = BERR_SUCCESS;
         break;

      default:
         rc = BERR_TRACE( BERR_NOT_SUPPORTED );
         break;
   }

   BDBG_LEAVE( BMUXlib_TS_Finish );
   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_TS_Stop(
         BMUXlib_TS_Handle hMuxTS
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BMUXlib_TS_Stop );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   BKNI_AcquireMutex( hMuxTS->hMutex );

   switch ( BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
   {
      case BMUXlib_State_eStarted:
      case BMUXlib_State_eFinishingInput:
         /* coverity[unterminated_case] */
      case BMUXlib_State_eFinishingOutput:
         BDBG_MSG(("Not finished yet. Output may be corrupt!"));

         /* coverity[fallthrough] */
      case BMUXlib_State_eFinished:
         BMUXLIB_TS_P_SET_MUX_STATE(hMuxTS, BMUXlib_State_eStopped);
#if BMUXLIB_TS_P_DUMP_VIDEO_DESC
         {
            uint32_t uiPIDIndex;

            for ( uiPIDIndex = 0; uiPIDIndex < BMUXLIB_TS_MAX_VIDEO_PIDS; uiPIDIndex++ )
            {
               if ( NULL != hMuxTS->status.stVideoSourceMetaData[uiPIDIndex].hDescDumpFile )
               {
                  fclose( hMuxTS->status.stVideoSourceMetaData[uiPIDIndex].hDescDumpFile );
                  hMuxTS->status.stVideoSourceMetaData[uiPIDIndex].hDescDumpFile = NULL;
               }
            }
         }
#endif
#if BMUXLIB_TS_P_DUMP_AUDIO_DESC
         {
            uint32_t uiPIDIndex;

            for ( uiPIDIndex = 0; uiPIDIndex < BMUXLIB_TS_MAX_AUDIO_PIDS; uiPIDIndex++ )
            {
               if ( NULL != hMuxTS->status.stAudioSourceMetaData[uiPIDIndex].hDescDumpFile )
               {
                  fclose( hMuxTS->status.stAudioSourceMetaData[uiPIDIndex].hDescDumpFile );
                  hMuxTS->status.stAudioSourceMetaData[uiPIDIndex].hDescDumpFile = NULL;
               }
            }
         }
#endif
#if BMUXLIB_TS_P_DUMP_TRANSPORT_DESC
         {
            uint32_t uiTransportIndex;

            for ( uiTransportIndex = 0; uiTransportIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportIndex++ )
            {
               if ( NULL != hMuxTS->status.stTransportMetaData[uiTransportIndex].hDescDumpFile )
               {
                  fclose( hMuxTS->status.stTransportMetaData[uiTransportIndex].hDescDumpFile );
                  hMuxTS->status.stTransportMetaData[uiTransportIndex].hDescDumpFile = NULL;
               }
            }
         }
#endif
#if BMUXLIB_TS_P_DUMP_TRANSPORT_PES
         {
            uint32_t uiTransportIndex;

            for ( uiTransportIndex = 0; uiTransportIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportIndex++ )
            {
               if ( NULL != hMuxTS->status.stTransportMetaData[uiTransportIndex].hPESDumpFile )
               {
                  fclose( hMuxTS->status.stTransportMetaData[uiTransportIndex].hPESDumpFile );
                  hMuxTS->status.stTransportMetaData[uiTransportIndex].hPESDumpFile = NULL;
               }
            }
         }
#endif
         break;

      case BMUXlib_State_eStopped:
         BDBG_WRN(("Already Stopped"));
         break;

      default:
         rc = BERR_INVALID_PARAMETER;
         break;
   }

   /* Reset State */
   BMUXlib_TS_P_FreeResources( hMuxTS );
   BMUXlib_TS_P_Reset( hMuxTS );

   BKNI_ReleaseMutex( hMuxTS->hMutex );
   BDBG_LEAVE( BMUXlib_TS_Stop );
   return BERR_TRACE( rc );
}

/***************/
/* System Data */
/***************/
BERR_Code
BMUXlib_TS_AddSystemDataBuffers(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_SystemData *astSystemDataBuffer, /* Array of system data buffers */
         size_t uiCount, /* Count of system data buffers in array */
         size_t *puiQueuedCount /* Count of system data buffers queued by muxer (*puiQueuedCount <= uiCount) */
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BMUXlib_TS_AddSystemDataBuffers );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( puiQueuedCount );
   BDBG_ASSERT( astSystemDataBuffer );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   BKNI_AcquireMutex( hMuxTS->hMutex );
   rc = BMUXlib_TS_P_AddSystemDataBuffers(hMuxTS, astSystemDataBuffer, uiCount, puiQueuedCount);
   BKNI_ReleaseMutex( hMuxTS->hMutex );
   BDBG_LEAVE( BMUXlib_TS_AddSystemDataBuffers );
   return BERR_TRACE( rc );
}

/* SW7425-3958: local version of the above call that does NOT lock with mutex so that this can be called
   by AllocateResources() from within Start().  */
BERR_Code
BMUXlib_TS_P_AddSystemDataBuffers(
         BMUXlib_TS_Handle hMuxTS,
         const BMUXlib_TS_SystemData *astSystemDataBuffer, /* Array of system data buffers */
         size_t uiCount, /* Count of system data buffers in array */
         size_t *puiQueuedCount /* Count of system data buffers queued by muxer (*puiQueuedCount <= uiCount) */
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BMUXlib_TS_P_AddSystemDataBuffers );

   *puiQueuedCount = 0;

   while ( ( uiCount != *puiQueuedCount )
           && ( false == BMUXlib_List_IsFull( hMuxTS->hSystemDataPendingList ) )
         )
   {
      if ( (0 == astSystemDataBuffer[*puiQueuedCount].uiSize)
          || (0 != ( astSystemDataBuffer[*puiQueuedCount].uiSize % 188 ))) {
         BDBG_ERR(("System buffer @%p size of %d bytes is NOT a multiple of MPEG TS packet size (188 bytes)",
                  &astSystemDataBuffer[*puiQueuedCount],
                  astSystemDataBuffer[*puiQueuedCount].uiSize
                  ));

         rc = BERR_INVALID_PARAMETER;
         break;
      }
      else
      {
         hMuxTS->astSystemDataPendingList[hMuxTS->status.uiSystemDataPendingListWriteOffset] = astSystemDataBuffer[*puiQueuedCount];
         rc = BMUXlib_List_Add(
                  hMuxTS->hSystemDataPendingList,
                  (void *) &hMuxTS->astSystemDataPendingList[hMuxTS->status.uiSystemDataPendingListWriteOffset]
                  );
         if (rc != BERR_SUCCESS)
         {
            BERR_TRACE( rc );
            break;
         }
         hMuxTS->status.uiSystemDataPendingListWriteOffset = (hMuxTS->status.uiSystemDataPendingListWriteOffset + 1) % hMuxTS->status.uiSystemDataMaxCount;

#ifdef BMUXLIB_TS_P_TEST_MODE
         /* SW7425-3958: do not write data to CSV files if these entries are due to move of data from pre-Q to pending Q */
         if (astSystemDataBuffer != hMuxTS->astSystemDataPendingListPreQ)
         {
            /* Write CSV containing info about system data: offset in data file, length, insertion interval, etc */
            /* system data is written as raw binary data to the data file */
            FILE *hCSVFile = hMuxTS->status.stSystemDataInfo.hCSVFile;
            FILE *hDataFile = hMuxTS->status.stSystemDataInfo.hDataFile;

            if (!hMuxTS->status.stSystemDataInfo.bCSVOpened)
            {
               char fname[256];
               sprintf(fname, "BMUXlib_TS_SystemData_%2.2d.csv", hMuxTS->stCreateSettings.uiMuxId);
               hCSVFile = fopen(fname, "w");
               if (NULL == hCSVFile)
                  BDBG_ERR(("Error Creating System Data CSV File (%s)", fname));
               hMuxTS->status.stSystemDataInfo.hCSVFile = hCSVFile;
               sprintf(fname, "BMUXlib_TS_SystemData_%2.2d.dat", hMuxTS->stCreateSettings.uiMuxId);
               hDataFile = fopen(fname, "wb");
               if (NULL == hDataFile)
                  BDBG_ERR(("Error Creating System Data File (%s)", fname));
               hMuxTS->status.stSystemDataInfo.hDataFile = hDataFile;

               hMuxTS->status.stSystemDataInfo.bCSVOpened = true;

               if (hCSVFile != NULL)
                  fprintf(hCSVFile, "offset, length, insertion_interval\n");
            }
            if (hCSVFile != NULL && hDataFile != NULL)
            {
               fprintf(hCSVFile, "%u, %u, %u\n", (unsigned)ftell(hDataFile), astSystemDataBuffer[*puiQueuedCount].uiSize,
                  astSystemDataBuffer[*puiQueuedCount].uiTimestampDelta);
               fwrite(astSystemDataBuffer[*puiQueuedCount].pData, sizeof(uint8_t),
                  astSystemDataBuffer[*puiQueuedCount].uiSize, hDataFile);
            }
         }
#endif
      }
      (*puiQueuedCount)++;
   }
   BDBG_LEAVE( BMUXlib_TS_P_AddSystemDataBuffers );
   return BERR_TRACE( rc );
}

BERR_Code
BMUXlib_TS_GetCompletedSystemDataBuffers(
         BMUXlib_TS_Handle hMuxTS,
         size_t *puiCompletedCount /* Returns count of system data buffers fully muxed */
         )
{
   BDBG_ENTER( BMUXlib_TS_GetCompletedSystemDataBuffers );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( puiCompletedCount );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   *puiCompletedCount = BMUXLIB_TS_P_GET_SYS_DATA_COMP_CNT(hMuxTS);
   BMUXLIB_TS_P_SET_SYS_DATA_COMP_CNT(hMuxTS, 0);

   BDBG_LEAVE( BMUXlib_TS_GetCompletedSystemDataBuffers );
   return BERR_TRACE( BERR_SUCCESS );
}

/***********/
/* Execute */
/***********/
BERR_Code
BMUXlib_TS_DoMux(
   BMUXlib_TS_Handle hMuxTS,
   BMUXlib_DoMux_Status *pstStatus
   )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BMUXlib_TS_DoMux );

   BDBG_ASSERT( hMuxTS );
   BDBG_ASSERT( pstStatus );
   BDBG_ASSERT( BMUXLIB_TS_P_SIGNATURE_CONTEXT == hMuxTS->uiSignature );

   BKNI_AcquireMutex( hMuxTS->hMutex );

   BKNI_Memset( pstStatus, 0, sizeof( BMUXlib_DoMux_Status ) );

   /* Debug code to print ESCR for current execution */
   if (NULL != hMuxTS->status.stStartSettings.transport.stDeviceInterface.fGetTransportStatus)
   {
      hMuxTS->status.stPreviousTransportStatus = hMuxTS->status.stTransportStatus;

      hMuxTS->status.stStartSettings.transport.stDeviceInterface.fGetTransportStatus(
               hMuxTS->status.stStartSettings.transport.stDeviceInterface.pContext,
               &hMuxTS->status.stTransportStatus
               );

      if ( false == hMuxTS->status.stStartSettings.bNonRealTimeMode )
      {
         if ( true == hMuxTS->status.bTransportStatusValid )
         {
            if ( hMuxTS->status.stTransportStatus.uiESCR > hMuxTS->status.stPreviousTransportStatus.uiESCR )
            {
               uint32_t uiMaxExpectedMSP = hMuxTS->status.stStartSettings.uiServiceLatencyTolerance + hMuxTS->status.stDoMuxStatus.uiNextExecutionTime;
               uint32_t uiDeltaESCR = hMuxTS->status.stTransportStatus.uiESCR - hMuxTS->status.stPreviousTransportStatus.uiESCR;
               uiDeltaESCR /= 27000;

               if ( uiDeltaESCR > uiMaxExpectedMSP )
               {
                  BDBG_WRN(("MUX Service Latency Too Large! (%d > %d ms)", uiDeltaESCR, uiMaxExpectedMSP ));
               }
            }
         }
      }

      hMuxTS->status.bTransportStatusValid = true;

      BDBG_MSG(("STC = %011llx, ESCR = %08x",
               hMuxTS->status.stTransportStatus.uiSTC,
               hMuxTS->status.stTransportStatus.uiESCR
               ));
   }

   if ( ( BMUXlib_State_eStopped != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
        && ( BMUXlib_State_eFinished != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
       )
   {
      BMUXlib_TS_P_ProcessCompletedBuffers( hMuxTS );

      if ( BMUXlib_State_eFinishingOutput == BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
      {
         unsigned uiIndex;
         bool bMuxOutputComplete = true;

         /* Make sure all transport descriptors have been consumed */
         for ( uiIndex = 0; uiIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiIndex++ )
         {
            if ( true == hMuxTS->status.stTransportMetaData[uiIndex].bActive )
            {
               if ( false == BMUXlib_List_IsEmpty( hMuxTS->hTransportDescriptorPendingList[uiIndex] ) )
               {
                  BDBG_MODULE_MSG( BMUXLIB_TS_FINISH, ("TRANSPORT[%d] has pending descriptors", uiIndex) );
                  bMuxOutputComplete = false;
                  break;
               }
            }
         }

         if ( true == bMuxOutputComplete )
         {
            BDBG_MODULE_MSG( BMUXLIB_TS_FINISH, ("Finished") );
            BMUXLIB_TS_P_SET_MUX_STATE(hMuxTS, BMUXlib_State_eFinished);
         }
      }
      else if ( ( BMUXlib_State_eFinishingInput == BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
                || ( BMUXlib_State_eStarted == BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
              )
      {
         bool bMuxInputComplete = true;

         BMUXlib_TS_P_ProcessNewBuffers( hMuxTS );

         /* Check if we've seen an EOS on all active A/V Channels */
         {
            unsigned uiIndex;

             /* Check if EOS seen on all input channels associated with this Transport Channel */
             for ( uiIndex = 0; uiIndex < hMuxTS->status.uiNumInputs ; uiIndex++ )
             {
               if ( false == hMuxTS->status.stInputMetaData[uiIndex].bEOSSeen )
               {
                  bMuxInputComplete = false;
                  break;
               }
             }
         }

         /* SW7425-998: Auto-Finish if EOS is seen on all active inputs */
         if ( BMUXlib_State_eStarted == BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
         {
            if ( true == bMuxInputComplete )
            {
               BMUXlib_TS_FinishSettings stFinishSettings;

               BMUXlib_TS_GetDefaultFinishSettings( &stFinishSettings );

               BDBG_MODULE_MSG( BMUXLIB_TS_FINISH, ("Auto Finish") );
               BMUXlib_TS_Finish(
                        hMuxTS,
                        &stFinishSettings
                        );
            }
         }

            /* SW7425-831: If we've seen an EOS on all inputs, and we're in NRT mode,
             * then we want to flush all output channels by inserting a dummy descriptor
             * with an ESCR=to the largestESCR.  That way, the XPT PB Pause logic won't
             * trigger if one output finishes before the others.
             */

         /* SW7425-999: In NRT mode, we need to insert dummy descriptors for all
          * inputs that have seen EOS to ensure the they don't cause the non-EOS
          * inputs to stall
          */
         if ( true == hMuxTS->status.stStartSettings.bNonRealTimeMode )
         {
            bMuxInputComplete &= BMUXlib_TS_P_Flush( hMuxTS, bMuxInputComplete);
         }

         if ( BMUXlib_State_eFinishingInput == BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
         {
            /* SW7425-2825: Currently, this code will throw out pending system or userdata
               after video and audio inputs have finished, under the assumption that such
               data corresponds to future data, or that loss of the userdata for the last
               frame of video is insignificant.  If this is not true, then code will need
               to be added here to process the pending system data and userdata.  However,
               it is important to be aware that the ESCR currently ceases to advance after
               audio and video complete, and so the change would need to flag completion of
               audio/video and subsequently use the system data queue's ESCR as the last
               pending ESCR instead (see ProcessSystemData()).
            */
            if ( true == bMuxInputComplete )
            {
               BDBG_MODULE_MSG( BMUXLIB_TS_FINISH, ("Finishing Output") );

               BMUXLIB_TS_P_SET_MUX_STATE(hMuxTS, BMUXlib_State_eFinishingOutput);
            }
         }
      }

      if ( false == hMuxTS->status.bTransportConfigured )
      {
         BMUXlib_TS_P_ConfigureTransport( hMuxTS );
      }

      if ( true == hMuxTS->status.bTransportConfigured )
      {
         BMUXlib_TS_P_ScheduleProcessedBuffers( hMuxTS );
      }
   }

   if ( ( BMUXlib_State_eStopped != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
        && ( BMUXlib_State_eFinished != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
       )
   {
      unsigned uiTransportIndex;

      for ( uiTransportIndex = 0; uiTransportIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportIndex++ )
      {
         if ( true == hMuxTS->status.stTransportMetaData[uiTransportIndex].bActive )
         {

            size_t uiTransportPending;

            BMUXlib_List_GetNumEntries(
                     hMuxTS->hTransportDescriptorPendingList[uiTransportIndex],
                     &uiTransportPending
                     );

            BDBG_MODULE_MSG(BMUXLIB_TS_PENDING, ("T[%2d]=%08x (ESCR=%08x)",
               uiTransportIndex,
               uiTransportPending,
               hMuxTS->status.uiTransportNextExpectedESCR[uiTransportIndex]
            ));
         }
      }
   }

#if 0
   {
      size_t uiNumTransportDescriptorsFree;
      size_t uiTSPacketsFree;
      size_t uiPESHeadersFree;
      size_t uiSystemDataFree;

      BMUXlib_List_GetNumEntries(
               hMuxTS->hTransportDescriptorFreeList,
               &uiNumTransportDescriptorsFree
               );

      BMUXlib_List_GetNumEntries(
               hMuxTS->hTSPacketFreeList,
               &uiTSPacketsFree
               );

      BMUXlib_List_GetNumEntries(
               hMuxTS->hPESHeaderFreeList,
               &uiPESHeadersFree
               );

      BMUXlib_List_GetNumFree(
               hMuxTS->hSystemDataPendingList,
               &uiSystemDataFree
               );

      BDBG_WRN(("t:%02d, pes:%02d, ts:%02d, sys:%02d",
         uiNumTransportDescriptorsFree,
         uiPESHeadersFree,
         uiTSPacketsFree,
         uiSystemDataFree
         ));
   }
#endif

   pstStatus->eState = BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS);

   pstStatus->uiNextExecutionTime = hMuxTS->status.stStartSettings.uiServicePeriod;

   /* Adjust MSP if we are underflowing */
   if ( ( true == hMuxTS->status.bTransportConfigured )
        && ( false == hMuxTS->status.stStartSettings.bNonRealTimeMode ) )
   {
      unsigned uiTransportChannelIndex = 0;
      int32_t iSmallestAmountPending = 0x7FFFFFFF;
      unsigned uiSmallestTransportChannelIndex = 0;

      for ( uiTransportChannelIndex = 0; uiTransportChannelIndex < BMUXLIB_TS_MAX_TRANSPORT_INSTANCES; uiTransportChannelIndex++ )
      {
         if ( true == hMuxTS->status.bTransportChannelStalled[uiTransportChannelIndex] )
         {
            int32_t iAmountPending = ((int32_t) ( hMuxTS->status.uiTransportNextExpectedESCR[uiTransportChannelIndex] - hMuxTS->status.stTransportStatus.uiESCR )) / 27000;

            if ( iAmountPending < iSmallestAmountPending )
            {
               iSmallestAmountPending = iAmountPending;
               uiSmallestTransportChannelIndex = uiTransportChannelIndex;
            }
         }
      }

      if ( iSmallestAmountPending != 0x7FFFFFFF )
      {
         if ( iSmallestAmountPending < 0 )
         {
            pstStatus->uiNextExecutionTime = hMuxTS->status.stStartSettings.uiServiceLatencyTolerance;

            BDBG_MODULE_WRN(BMUXLIB_TS_MSP, ("Underflow on transport[%d]! (%d ms behind)", uiSmallestTransportChannelIndex, iSmallestAmountPending));
         }
         else if ( (uint32_t) iSmallestAmountPending > hMuxTS->status.stStartSettings.uiServiceLatencyTolerance )
         {
            pstStatus->uiNextExecutionTime = (uint32_t) iSmallestAmountPending - hMuxTS->status.stStartSettings.uiServiceLatencyTolerance;
         }
         else
         {
            pstStatus->uiNextExecutionTime = (uint32_t) iSmallestAmountPending / 2;
         }
         BDBG_MODULE_MSG(BMUXLIB_TS_MSP, ("MSP Adjusted to (%d ms) (%d ms pending)", pstStatus->uiNextExecutionTime, iSmallestAmountPending));
      }
   }

   if ( pstStatus->uiNextExecutionTime > hMuxTS->status.stStartSettings.uiServicePeriod )
   {
      pstStatus->uiNextExecutionTime = hMuxTS->status.stStartSettings.uiServicePeriod;
   }

   /* SW7425-3684: Update completed duration */
   if ( ( BMUXlib_State_eStopped != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
        && ( BMUXlib_State_eFinished != BMUXLIB_TS_P_GET_MUX_STATE(hMuxTS) )
        && ( NULL != hMuxTS->status.hInputGroup )
       )
   {
      BMUXlib_InputGroup_Status stStatus;

      BMUXlib_InputGroup_GetStatus(
         hMuxTS->status.hInputGroup,
         &stStatus
         );

      pstStatus->uiCompletedDuration = stStatus.uiDuration;
   }

   hMuxTS->status.stDoMuxStatus = *pstStatus;

   BKNI_ReleaseMutex( hMuxTS->hMutex );
   BDBG_LEAVE( BMUXlib_TS_DoMux );
   return BERR_TRACE( rc );
}
