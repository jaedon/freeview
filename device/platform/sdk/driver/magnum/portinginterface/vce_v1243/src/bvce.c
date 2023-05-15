/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce.c $
 * $brcm_Revision: Hydra_Software_Devel/265 $
 * $brcm_Date: 4/18/13 3:28p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce.c $
 * 
 * Hydra_Software_Devel/265   4/18/13 3:28p nilesh
 * SW7425-4614: Fixed deadlock when CABAC is not ready and the CDB/ITB is
 * full from previous unclean stop
 * 
 * Hydra_Software_Devel/264   4/8/13 4:26p nilesh
 * SW7425-4489: Fixed setting of target bitrate
 * 
 * Hydra_Software_Devel/263   4/8/13 3:04p nilesh
 * SW7425-4610: Print the PicArc PC during the watchdog
 * 
 * Hydra_Software_Devel/262   4/3/13 3:53p nilesh
 * SW7425-4614: Return error code in BVCE_Channel_Close and
 * BVCE_Channel_StopEncode
 * 
 * Hydra_Software_Devel/261   4/3/13 3:50p nilesh
 * SW7425-4614: Check GetStatus return code in FlushEncode
 * 
 * Hydra_Software_Devel/260   4/3/13 2:39p nilesh
 * SW7425-4614: Added support for flushing when ITB/CDB accessed directly
 * 
 * Hydra_Software_Devel/259   4/2/13 2:07p nilesh
 * SW7425-4676: Added support for reporting ARC Watchdog error code
 * 
 * Hydra_Software_Devel/258   4/1/13 4:31p nilesh
 * SW7425-4614: Fixed issue if channel is closed before output is closed
 * 
 * Hydra_Software_Devel/257   3/28/13 1:57p nilesh
 * SW7425-4676: VCE FW v4.0.9.0
 * 
 * Hydra_Software_Devel/256   3/21/13 4:36p nilesh
 * SW7425-4614: Fixed issue with FlushEncode using a stale output handle
 * 
 * Hydra_Software_Devel/255   3/21/13 12:41p nilesh
 * SW7425-4614: Fixed race condition between app calling Start/FlushEncode
 * and the mux thread calling GetBuffer/ConsumeBuffer using mutexes.
 * Cleaned up interface between output and channel modules.  Fixed
 * problem with flushed descriptors causin CABAC ready to be set
 * prematurely.
 * 
 * Hydra_Software_Devel/254   3/20/13 12:05p nilesh
 * SW7425-4614: Always allow starting a new encode when in the stopping
 * state
 * 
 * Hydra_Software_Devel/253   3/20/13 10:26a nilesh
 * SW7425-4612: BVCE_Channel_FlushEncode() now waits for EOS if the
 * channel is in the stopping state
 * 
 * Hydra_Software_Devel/252   3/19/13 6:34p nilesh
 * SW7435-688: Added BVCE_Channel_StopMode_eAbort support
 * 
 * Hydra_Software_Devel/251   3/19/13 4:19p nilesh
 * SW7425-4614: Fixed stop/close/open/start sequence when muxer thread
 * dies prematurely
 * 
 * Hydra_Software_Devel/250   3/19/13 1:57p nilesh
 * SW7435-688: Added BVCE_Channel_FlushEncode()
 * 
 * Hydra_Software_Devel/249   3/11/13 1:37p nilesh
 * SW7425-2568,FWVICE2-426: Fixed 'received' count in getStatus to include
 * drops due to HRD underflow.
 * 
 * Hydra_Software_Devel/248   3/8/13 2:01p nilesh
 * SW7425-4612: Fixed AcquireMutex seg fault in BVCE_Channel_Close
 * 
 * Hydra_Software_Devel/247   3/7/13 3:40p nilesh
 * SW7425-4612: Updated code to ensure immediate stop --> start is as fast
 * as possible
 * 
 * Hydra_Software_Devel/246   3/5/13 1:23p nilesh
 * SW7445-181: Changed BDBG_CASSERT to BDBG_CWARNING for struct size
 * validation
 * 
 * Hydra_Software_Devel/245   3/4/13 4:19p nilesh
 * SW7425-2568: Added stc snapshot, drop due to hrd underflow, and ets/dts
 * offset to status log
 * 
 * Hydra_Software_Devel/244   2/28/13 3:55p nilesh
 * SW7425-2568: Added drop due to HRD to status
 * 
 * Hydra_Software_Devel/243   2/28/13 10:47a nilesh
 * SW7425-4355: Populate uiEtsDtsOffset in status and video metadata
 * descriptor
 * 
 * Hydra_Software_Devel/242   2/27/13 4:59p nilesh
 * SW7425-4589: Still send EOS descriptor when immediate stop is used
 * 
 * Hydra_Software_Devel/241   2/26/13 12:13p nilesh
 * SW7425-2568: Added new config parameters to config log
 * 
 * Hydra_Software_Devel/240   2/11/13 2:05p nilesh
 * SW7425-4356: Set CONFIG_CHANNEL.NumParallelEncodes
 * 
 * Hydra_Software_Devel/239   1/30/13 4:06p nilesh
 * SW7425-4492: Added support for specifying the max number of channels
 * 
 * Hydra_Software_Devel/238   1/30/13 11:33a nilesh
 * SW7425-4489: Added VBR support
 * 
 * Hydra_Software_Devel/237   1/29/13 4:02p nilesh
 * SW7425-4486: VCE FW v4.0.8.0 integration
 * 
 * Hydra_Software_Devel/236   1/15/13 2:14p nilesh
 * SW7425-4385: Use EOS_SENT event in immediate stop command
 * 
 * Hydra_Software_Devel/235   1/15/13 1:16p nilesh
 * SW7425-4325: Removed temporary workaround for FWVICE2-330 (fixed in FW
 * v4.0.7.1)
 * 
 * Hydra_Software_Devel/234   1/15/13 1:10p nilesh
 * SW7425-4308: Removed temporary workaround for FWVICE2-321 (fixed in FW
 * v4.0.7.1)
 * 
 * Hydra_Software_Devel/233   1/15/13 12:56p nilesh
 * SW7435-567: Removed temporary workaround for FWVICE2-341 (fixed in FW
 * v4.0.7.1)
 * 
 * Hydra_Software_Devel/232   1/15/13 12:50p nilesh
 * SW7425-4356: Added
 * BVCE_Channel_StartEncodeSettings.uiNumParallelNRTEncodes
 * 
 * Hydra_Software_Devel/231   1/3/13 1:42p nilesh
 * SW7425-4378: Temporarily fix race condition with immediate stop command
 * and shutting down the VICE clocks
 * 
 * Hydra_Software_Devel/230   1/3/13 11:33a nilesh
 * SW7435-567: Temporary workaround for channel 1 only encoder hang
 * 
 * Hydra_Software_Devel/229   12/18/12 4:23p nilesh
 * SW7425-4344: Removed XUDlib dependency on VCE
 * 
 * Hydra_Software_Devel/228   12/12/12 3:58p nilesh
 * SW7425-4325: Temporary workaround for FWVICE2-330
 * 
 * Hydra_Software_Devel/227   12/10/12 3:59p nilesh
 * SW7425-4308: Temporary fix for invalid Dee error when dynamically
 * changing encode parameters
 * 
 * Hydra_Software_Devel/226   12/5/12 3:20p nilesh
 * SW7425-4230: Added BVCE_GetVersionInfo()
 * 
 * Hydra_Software_Devel/225   12/5/12 2:00p nilesh
 * SW7445-39: Added runtime BVN2VICE MBOX version validation
 * 
 * Hydra_Software_Devel/224   12/5/12 10:40a nilesh
 * SW7445-39: Added BVN2VICE API version check
 * 
 * Hydra_Software_Devel/223   12/3/12 5:38p nilesh
 * SW7445-39: FW v4.0.7.0
 * 
 * Hydra_Software_Devel/SW7445-39/5   11/20/12 3:16p nilesh
 * SW7445-39: Removed MPEG4 support from 7445
 * 
 * Hydra_Software_Devel/SW7445-39/4   11/20/12 2:31p nilesh
 * SW7445-39: Merge from FWVICE2-294
 * 
 * Hydra_Software_Devel/FWVICE2-294/1   11/8/12 9:07p nilesh
 * FWVICE2-294: Added EOS FW event support
 * 
 * Hydra_Software_Devel/SW7445-39/3   11/20/12 11:14a nilesh
 * SW7445-39: Rebase from mainline
 * 
 * Hydra_Software_Devel/222   11/16/12 12:39p nilesh
 * SW7435-448: Use BUDP_DCCparse_Format_LAST
 * 
 * Hydra_Software_Devel/SW7445-39/2   11/16/12 10:55a nilesh
 * SW7445-39: Added support for multiple slices
 * 
 * Hydra_Software_Devel/SW7445-39/1   11/15/12 3:36p nilesh
 * SW7445-39: Removed VC1
 * 
 * Hydra_Software_Devel/221   11/14/12 2:17p nilesh
 * SW7445-39: Set VICE ARCSS DATA ADDR OFFSET
 * 
 * Hydra_Software_Devel/220   11/6/12 4:08p nilesh
 * SW7425-4186: Added EOS event status
 * 
 * Hydra_Software_Devel/219   10/17/12 4:52p nilesh
 * SW7425-4099: Added BVN API version debug LUT for INIT command's
 * response
 * 
 * Hydra_Software_Devel/218   10/11/12 12:50p nilesh
 * SW7425-2568: Updated to use BVCE_Debug logging functions
 * 
 * Hydra_Software_Devel/217   10/11/12 12:09p nilesh
 * SW7425-2568: Updated to use BVCE_Debug logging functions
 * 
 * Hydra_Software_Devel/216   10/3/12 12:40p nilesh
 * SW7445-39: Added bvce_output.h
 * 
 * Hydra_Software_Devel/215   10/1/12 2:03p nilesh
 * SW7445-39: Split power and output code into separate files
 * 
 * Hydra_Software_Devel/214   9/21/12 12:52p nilesh
 * SW7425-2568: Fixed config log
 * 
 * Hydra_Software_Devel/213   8/29/12 2:02p nilesh
 * SW7425-3360,SW7425-3644: FNRT support, Endian Neutral API
 * 
 * Hydra_Software_Devel/SW7425-3360/10   8/15/12 1:57p nilesh
 * SW7425-3644: Merge from SW7425-3644
 * 
 * Hydra_Software_Devel/SW7425-3360/9   8/15/12 1:49p nilesh
 * SW7425-3360: Rebase from mainline
 * 
 * Hydra_Software_Devel/SW7425-3644/2   8/6/12 12:27p nilesh
 * SW7425-3644: Rebase from mainline
 * 
 * Hydra_Software_Devel/SW7425-3644/1   7/31/12 11:07a nilesh
 * SW7425-3644: Updated FW API to be endian neutral
 * 
 * Hydra_Software_Devel/SW7425-3360/8   8/10/12 10:54p nilesh
 * SW7425-3360: StopEncode when encoder is already stopped is now a NOP
 * 
 * Hydra_Software_Devel/SW7425-3360/7   8/3/12 12:22p nilesh
 * SW7425-3360: Fixed EOC + EOS behavior to no longer send a metadata
 * descriptor
 * 
 * Hydra_Software_Devel/SW7425-3360/6   7/25/12 1:32p nilesh
 * SW7425-3596: Rebase from mainline
 * 
 * Hydra_Software_Devel/210   7/25/12 1:29p nilesh
 * SW7425-3596: Use BMEM_AllocAligned with explicit alignment instead of
 * BMEM_Alloc
 * 
 * Hydra_Software_Devel/SW7425-3360/5   7/24/12 2:25p nilesh
 * SW7425-3357: Removed used of
 * BAVC_CompressedBufferDescriptor.pBaseBufferAddress
 * 
 * Hydra_Software_Devel/SW7425-3360/4   7/18/12 5:14p nilesh
 * SW7425-3360: Rebase from mainline
 * 
 * Hydra_Software_Devel/209   7/18/12 4:53p nilesh
 * SW7425-3156: Updated BVCE_Channel_ClearStatus behavior to be more
 * intuitive
 * 
 * Hydra_Software_Devel/SW7425-3360/3   7/5/12 5:53p nilesh
 * SW7425-3360: Removed debug prints
 * 
 * Hydra_Software_Devel/SW7425-3360/2   7/5/12 5:51p nilesh
 * SW7425-3360: Initial VCE FW FNRT support. Various bug fixes.
 * 
 * Hydra_Software_Devel/SW7425-3360/1   6/28/12 1:56p nilesh
 * SW7425-3360: Added FNRT support
 * 
 * Hydra_Software_Devel/208   6/27/12 3:51p nilesh
 * SW7435-200: Replaced critical section with mutex
 * 
 * Hydra_Software_Devel/207   6/26/12 11:58a nilesh
 * SW7435-200: Removed unnecessary critical section
 * 
 * Hydra_Software_Devel/206   6/7/12 9:21a nilesh
 * SW7425-3167: Removed bTrackInput and
 * bAllowGOPRestartOnSceneChangeDetect from BVCE_GOPStructure
 * 
 * Hydra_Software_Devel/205   6/6/12 3:03p nilesh
 * SW7425-3167: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-3167/1   6/5/12 11:50a nilesh
 * SW7425-3167: Removed bTrackInput and
 * bAllowGOPRestartOnSceneChangeDetect from BVCE_GOPStructure
 * 
 * Hydra_Software_Devel/204   5/24/12 10:19p nilesh
 * SW7425-3103: Added 15 and 20 fps support
 * 
 * Hydra_Software_Devel/203   5/22/12 3:14p nilesh
 * SW7425-2568: Added ud debug print when null ud entry added
 * 
 * Hydra_Software_Devel/202   5/10/12 3:29p nilesh
 * SW7425-2568: Add stc to userdata debug print
 * 
 * Hydra_Software_Devel/201   5/9/12 4:06p nilesh
 * SW7425-3023: Fix potential race conditions when writing to the CC user
 * data queue
 * 
 * Hydra_Software_Devel/200   5/2/12 3:19p nilesh
 * SW7425-2979: Zero out FW memory if secure boot is used
 * 
 * Hydra_Software_Devel/199   5/1/12 1:01p nilesh
 * SW7425-1221: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1221/2   4/23/12 5:19p nilesh
 * SW7425-2244: Added 42-bit STC snapshot to video metadata descriptor
 * 
 * Hydra_Software_Devel/SW7425-1221/1   4/23/12 4:50p nilesh
 * SW7425-2244: Added 42-bit STC support
 * 
 * Hydra_Software_Devel/198   4/11/12 4:13p nilesh
 * SW7425-2858: Fixed incorrect assert for max 608/708 line count.
 * Updated code to be more tolerant by truncating instead of asserting.
 * 
 * Hydra_Software_Devel/197   3/20/12 3:40p nilesh
 * SW7425-2568: Fix compilation error when BVCE_DUMP_OUTPUT_CDB=y
 * 
 * Hydra_Software_Devel/196   3/8/12 11:51a nilesh
 * SW7425-2568: Added BVCE_TEST_MODE compile time option
 * 
 * Hydra_Software_Devel/195   2/14/12 11:46a nilesh
 * SW7425-2220: Fixed BVCE_Output_Reset()
 * 
 * Hydra_Software_Devel/194   2/14/12 10:05a nilesh
 * SW7425-2220: Cleaned up state when StopEncode(eImmediate) is used
 * 
 * Hydra_Software_Devel/193   2/13/12 9:32p nilesh
 * SW7425-2265: Re-enabled MBArc watchdog callback
 * 
 * Hydra_Software_Devel/192   2/13/12 5:58p nilesh
 * SW7425-2220: StopEncode(eImmediate) now causes internal channel status
 * to change to eOpened immediately
 * 
 * Hydra_Software_Devel/191   2/10/12 8:38a nilesh
 * SW7425-2220: Fixed coverity warning
 * 
 * Hydra_Software_Devel/190   2/9/12 1:07p nilesh
 * SW7425-2265: Added BDBG_ERR in watchdog ISR.  Temporarily disable MBArc
 * Watchdog.
 * 
 * Hydra_Software_Devel/189   2/9/12 11:36a nilesh
 * SW7425-2265: Updated cmd debug prints
 * 
 * Hydra_Software_Devel/188   2/8/12 4:25p nilesh
 * SW7425-2220,SW7425-2349: Renamed
 * BVCE_Channel_StartEncodeSettings.bFastChannelChangeMode to
 * BVCE_Channel_StartEncodeSettings.bAdaptiveLowDelayMode
 * 
 * Hydra_Software_Devel/187   2/8/12 1:26p nilesh
 * SW7425-2220,SW7425-2078: Added fast channel change support. Added
 * support for sending worst case GOP structure.
 * 
 * Hydra_Software_Devel/186   1/26/12 11:29a nilesh
 * SW7425-2265: Changed worst case b-frames from 3 to 2.
 * 
 * Hydra_Software_Devel/185   1/26/12 11:22a nilesh
 * SW7425-2265: Use worst case GOP length in StartSettings instead of
 * EncodeSettings for calculating A2PDelay
 * 
 * Hydra_Software_Devel/184   1/25/12 4:49p nilesh
 * SW7425-2220,SW7425-997: Change BVCE_Channel_StopEncode() prototype to
 * support different stop modes
 * 
 * Hydra_Software_Devel/183   1/25/12 1:50p delkert
 * SW7425-2217: Set Original PTS Valid flag in descriptor upon frame start
 *
 * Hydra_Software_Devel/182   1/17/12 11:49p nilesh
 * SW7425-2102: Remove check for strictly increasing line offset in 608
 * user data
 *
 * Hydra_Software_Devel/181   1/10/12 1:25p nilesh
 * SW7435-1: Added support for multiple instances
 *
 * Hydra_Software_Devel/180   1/2/12 3:57p nilesh
 * SW7425-2078: Set Begin New RAP if GOP structure is changed dynamically
 *
 * Hydra_Software_Devel/179   12/20/11 2:48p nilesh
 * SW7425-1882: Added BVCE_DUMP_USERDATA_LOG build flag
 *
 * Hydra_Software_Devel/178   12/19/11 2:51p nilesh
 * SW7425-1995: Prevent BCHP_PWR_[Acquire/Release]Resource from being
 * called in ISR functions. Instead, require the encoder channel to be
 * started when calling ISR routines.
 *
 * Hydra_Software_Devel/177   12/19/11 11:26a nilesh
 * SW7425-960: Implemented BVCE_Channel_UserData_GetStatus_isr()
 *
 * Hydra_Software_Devel/176   12/19/11 10:43a nilesh
 * SW7425-1110: Reduce stack usage
 *
 * Hydra_Software_Devel/175   12/9/11 10:40a nilesh
 * SW7425-1566: Moved MultiChannelMode to
 * BVCE_Channel_OpenSettings.eMultiChannelMode
 *
 * Hydra_Software_Devel/174   12/8/11 1:36p nilesh
 * SW7425-960: Fixed 708 userdata packet start detection
 *
 * Hydra_Software_Devel/173   12/8/11 10:42a nilesh
 * SW7425-1882: Reduce stack usage
 *
 * Hydra_Software_Devel/172   12/7/11 4:34p nilesh
 * SW7425-960: Fixed 708 User Data Insertion
 *
 * Hydra_Software_Devel/171   12/7/11 3:43p nilesh
 * SW7425-960: Optimized BVCE_USERDATA debug log
 *
 * Hydra_Software_Devel/170   12/7/11 1:14p nilesh
 * SW7425-960: Enabled multiple userdata packet support
 *
 * Hydra_Software_Devel/169   12/6/11 4:13p nilesh
 * SW7425-960: Fixed compilation error
 *
 * Hydra_Software_Devel/168   12/6/11 4:06p nilesh
 * SW7425-960: Added support for sorting multiple userdata descriptors by
 * packet type
 *
 * Hydra_Software_Devel/167   12/6/11 11:57a nilesh
 * SW7425-960: Disabled multiple packet support
 *
 * Hydra_Software_Devel/166   12/6/11 11:44a nilesh
 * SW7425-960: Updated User Data FW API.  B-Picture user data support.
 * Multiple User Data support per field.
 *
 * Hydra_Software_Devel/165   12/5/11 5:00p nilesh
 * SW7425-960: Fixed user data debug print
 *
 * Hydra_Software_Devel/164   12/5/11 3:35p nilesh
 * SW7425-1848: Added BVCE_OpenSettings.bVerificationMode (Default is:
 * false)
 *
 * Hydra_Software_Devel/163   12/5/11 2:14p nilesh
 * SW7425-960: Added VCE user data support
 *
 * Hydra_Software_Devel/162   11/17/11 3:35p nilesh
 * SW7425-1746: Added BVCE_Channel_BeginNewRAP()
 *
 * Hydra_Software_Devel/161   11/16/11 4:16p nilesh
 * SW7425-1566: Change BVCE_MultiChannelMode enum to match FW enum
 *
 * Hydra_Software_Devel/160   11/16/11 4:10p nilesh
 * SW7425-1566: Added BVCE_MultiChannelMode support
 *
 * Hydra_Software_Devel/159   11/11/11 4:58p nilesh
 * SW7425-1663: Updated error flags
 *
 * Hydra_Software_Devel/158   10/24/11 4:48p nilesh
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 *
 * Hydra_Software_Devel/157   10/18/11 5:17p nilesh
 * SW7425-890: Added new error bits
 *
 * Hydra_Software_Devel/156   10/14/11 11:57a nilesh
 * SW7425-1213: Added Telemetry Parser
 *
 * Hydra_Software_Devel/155   10/7/11 12:11p nilesh
 * SW7425-1411: Fix coverity defects
 *
 * Hydra_Software_Devel/154   10/7/11 8:54a nilesh
 * SW7425-1411: Fix coverity defects
 *
 * Hydra_Software_Devel/153   10/6/11 4:52p nilesh
 * SW7425-891: Fixed incorrect warning message in
 * BVCE_Output_ConsumeBufferDescriptors()
 *
 * Hydra_Software_Devel/152   10/5/11 11:29a nilesh
 * SW7425-1411: Fix coverity defects
 *
 * Hydra_Software_Devel/151   10/4/11 4:46p nilesh
 * SW7425-891: Fixed STC value dumped for output context 1
 *
 * Hydra_Software_Devel/150   10/4/11 2:40p nilesh
 * SW7425-891: Fix compilation error when ITB Desc dump enabled
 *
 * Hydra_Software_Devel/149   9/26/11 3:46p nilesh
 * SW7425-1213: Added BVCE_DUMP_ARC_DEBUG_LOG build flag to enable dump of
 * raw debug log buffers to file
 *
 * Hydra_Software_Devel/148   9/26/11 3:21p nilesh
 * SW7425-1213: Added BVCE_Debug_SendCommand support
 *
 * Hydra_Software_Devel/147   9/26/11 9:52a pblanco
 * SW7425-1337: Wired remainder of descriptor queue depth code into
 * BVCE_Output_Open, BVCE_Output_GetBufferDescriptors and
 * BVCE_Output_ConsumeBufferDescriptors.
 *
 * Hydra_Software_Devel/146   9/23/11 10:23a nilesh
 * SW7425-1224: BVCE_Power_[Standby/Resume] now behave the same regardless
 * of whether power management support is enabled
 *
 * Hydra_Software_Devel/145   9/22/11 3:56p pblanco
 * SW7425-1337: Added default initialization code for
 * BVCE_Output_OpenSettings.uiDescriptorQueueDepth member.
 *
 * Hydra_Software_Devel/144   9/19/11 11:16a nilesh
 * SW7425-891: Removed suspend/resume references
 *
 * Hydra_Software_Devel/143   9/15/11 4:17p nilesh
 * SW7425-891: Implemented clean shutdown of MAU before SW INIT
 *
 * Hydra_Software_Devel/142   9/15/11 3:32p nilesh
 * SW7425-1292: Temporarily fixed values returned in GetChannelStatus
 *
 * Hydra_Software_Devel/141   9/15/11 3:04p nilesh
 * SW7425-890: Added compile time flag (BVCE_ENABLE_DUAL_ENCODE_RTS) to
 * force dual encode RTS for VIP_1
 *
 * Hydra_Software_Devel/140   9/14/11 2:55p nilesh
 * SW7425-890: Added support for 2 separate cabac data ready interrupts
 * for 7425 B0
 *
 * Hydra_Software_Devel/139   9/8/11 1:57p nilesh
 * SW7425-891: Add B0 and Dual Encode Support
 *
 * Hydra_Software_Devel/SW7425-891/1   9/7/11 12:34p nilesh
 * SW7425-891: Disable 7425 A0 workaround logic
 *
 * Hydra_Software_Devel/138   9/1/11 1:35p nilesh
 * SW7425-1195: Removed unused BVCE_P_SWAP32 macro
 *
 * Hydra_Software_Devel/137   8/31/11 3:50p nilesh
 * SW7425-1191: Fixed ITB/CDB cache coherency issue
 *
 * Hydra_Software_Devel/136   8/31/11 11:53a nilesh
 * SW7425-448: Removed extra clock acquire in BVCE_Close()
 *
 * Hydra_Software_Devel/135   8/31/11 11:37a nilesh
 * SW7425-448: Clock is now acquired in BVCE_Power_Standby() to prevent
 * GISB timeout
 *
 * Hydra_Software_Devel/134   8/25/11 12:04p nilesh
 * SW7425-1186: Fixed BVCE_Close() being called twice during error
 * handling
 *
 * Hydra_Software_Devel/133   8/22/11 10:31p nilesh
 * SW7425-1006: VCE FW API v2.0.0.2
 *
 * Hydra_Software_Devel/132   8/18/11 10:31a nilesh
 * SW7425-1006: Set default min BVN rate to 23.976 for compatibility with
 * existing transcoding applications
 *
 * Hydra_Software_Devel/131   8/15/11 5:28p nilesh
 * SW7425-448: Added Active Power Management
 * (BVCE_Power_Standby/BVCE_Power_Resume)
 *
 * Hydra_Software_Devel/130   8/11/11 11:30a nilesh
 * SW7425-1006: Removed compilation warning
 *
 * Hydra_Software_Devel/129   8/11/11 11:29a nilesh
 * SW7425-1006: VCE FW API v2.0
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/9   8/10/11 5:29p nilesh
 * SW7425-1006: Merge VCE_FW_API_v1_2 branch to VICE2_FW_Dev branch
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/8   8/10/11 4:49p nilesh
 * SW7425-448: Rebase against mainline
 *
 * Hydra_Software_Devel/128   8/10/11 4:21p nilesh
 * SW7425-448: Added Dynamic Power Management
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/7   8/2/11 9:32p nilesh
 * SW7425-448: Refactored power management logic.
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/6   8/1/11 3:38p nilesh
 * SW7425-448: Fixed dynamic power management
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/5   7/26/11 3:03p nilesh
 * SW7425-448: Fixed mismatch in BCHP_PWR_[Acquire/Release]Resource when
 * channel closed before all descriptors are consumed.
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/4   7/25/11 5:00p nilesh
 * SW7425-1006: Added support for unsupported resolution error bit
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/3   7/25/11 4:20p nilesh
 * SW7425-448: Added dynamic power management support
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/2   7/22/11 11:15p nilesh
 * SW7425-1: Additional CDB debug logs
 *
 * Hydra_Software_Devel/VICE2_FW_Dev/1   7/20/11 2:46p nilesh
 * SW7425-762,SW7425-787,SW7425-836,SW7425-837: NRT Fix Test FW v1.2.0.4
 *
 * Hydra_Software_Devel/127   7/18/11 2:07p pblanco
 * SW7425-931: Moved ITB structure and macro definitions from bvce.c to
 * bvce_priv.h
 *
 * Hydra_Software_Devel/126   6/22/11 3:40p delkert
 * SW7425-739: Add GOP Structure error checking
 *
 * Hydra_Software_Devel/125   6/15/11 5:00p nilesh
 * SW7425-671: Fixed version print formatting
 *
 * Hydra_Software_Devel/124   6/15/11 4:58p nilesh
 * SW7425-671: Pretty print FW/API versions.  Only print version string if
 * FW version indicates a debug build.
 *
 * Hydra_Software_Devel/123   6/15/11 4:46p nilesh
 * SW7425-671: Added FW API Version Validation.  Added FW Version print.
 *
 * Hydra_Software_Devel/122   6/14/11 11:33a nilesh
 * SW7425-448: Disable power management until API finalized
 *
 * Hydra_Software_Devel/121   6/10/11 11:33a nilesh
 * SW7425-448: Added dynamic power management
 *
 * Hydra_Software_Devel/120   6/8/11 1:10p nilesh
 * SW7425-625: Replaced ITB entry access from using bitfields to using
 * shift/mask
 *
 * Hydra_Software_Devel/119   6/8/11 12:10a nilesh
 * SW7425-519: Fixed seg fault when calling BVCE_Output_Reset() before
 * BVCE_Output_SetBuffers()
 *
 * Hydra_Software_Devel/118   6/6/11 5:37p nilesh
 * SW7425-608: Added support for unsupported BVN frame rate error
 *
 * Hydra_Software_Devel/117   6/2/11 2:15p delkert
 * SW7425-518: Add MPEG 4 Part 2 Data Unit Detection
 *
 * Hydra_Software_Devel/116   5/23/11 4:46p nilesh
 * SW7425-629: Use ENCODING_{Protocol}_LEVEL_XXX constants defined in
 * bvce_fw_api.h
 *
 * Hydra_Software_Devel/115   5/18/11 2:55p nilesh
 * SW7425-387: Added VC1 metadata support
 *
 * Hydra_Software_Devel/114   5/11/11 11:15a nilesh
 * SW7425-1: Flush ITB Descriptor Output Debug to prevent lost entries
 * during a crash
 *
 * Hydra_Software_Devel/113   4/20/11 9:13p nilesh
 * SW7425-396: Phase 2.0 Release
 *
 * Hydra_Software_Devel/7425_Phase2/3   4/20/11 3:30p nilesh
 * SW7425-393: Fix build warnings in release mode
 *
 * Hydra_Software_Devel/7425_Phase2/2   4/19/11 10:38p nilesh
 * SW7425-379: Removed PTS/DTS fix
 *
 * Hydra_Software_Devel/7425_Phase2/1   4/19/11 10:35p nilesh
 * SW7425-173: Updated with new event/error flag bit definitions
 *
 * Hydra_Software_Devel/112   4/19/11 3:16p nilesh
 * SW7425-171: Removed
 * BVCE_Channel_GetDefaultEncodeSettings_OnInputChange() since it is
 * redundant with BVCE_Channel_GetEncodeSettings_OnInputChange()
 *
 * Hydra_Software_Devel/111   4/19/11 2:48p nilesh
 * SW7425-379: Temporarily fix PTS/DTS skew until FW fix is officially
 * released
 *
 * Hydra_Software_Devel/110   4/19/11 1:42p nilesh
 * SW7425-289: Added STC snapshot to ITB Descriptor Dump
 *
 * Hydra_Software_Devel/109   4/19/11 10:52a nilesh
 * SW7425-115: Added BVCE_ENABLE_OUTPUT=y build option
 *
 * Hydra_Software_Devel/108   4/16/11 1:03a nilesh
 * SW7425-171: Added support for pending settings
 *
 * Hydra_Software_Devel/107   4/15/11 5:18p nilesh
 * SW7425-139: Fix CDB Depth calculations for debug
 *
 * Hydra_Software_Devel/106   4/15/11 1:11p nilesh
 * SW7425-289: Normalize SHR 1 frame skew by setting current ITB entry's
 * SHR using the next ITB entry
 *
 * Hydra_Software_Devel/105   4/14/11 3:41p nilesh
 * SW7425-171: Fixed FW --> PI mapping of error/event flags
 *
 * Hydra_Software_Devel/104   4/13/11 5:16p nilesh
 * SW7425-303: The frame buffer base address returned by
 * BVCE_Output_GetBufferStatus() is now a CACHED bmem address
 *
 * Hydra_Software_Devel/103   4/13/11 4:29p nilesh
 * SW7425-75: Added phase 2.0 metadata support
 *
 * Hydra_Software_Devel/102   4/12/11 4:25p nilesh
 * SW7425-303: The video buffer descriptor pointers returned by
 * BVCE_Output_GetBufferDescriptors() are now CACHED bmem addresses
 *
 * Hydra_Software_Devel/101   4/11/11 5:20p nilesh
 * SW7425-171: Added BVCE_Channel_SetEncodeSettings_OnInputChange() and
 * BVCE_Channel_GetDefaultEncodeSettings_OnInputChange()
 *
 * Hydra_Software_Devel/100   4/11/11 4:51p nilesh
 * SW7425-171: Added CASSERT to verify VCE PI/FW events/error masks are in
 * sync
 *
 * Hydra_Software_Devel/99   4/11/11 4:39p nilesh
 * SW7425-171: Separated events from errors in
 * BVCE_Channel_EventCallbackSettings and BVCE_Channel_Status
 *
 * Hydra_Software_Devel/98   4/11/11 11:57a nilesh
 * SW7425-75: Temporarily disabled metadata support.
 *
 * Hydra_Software_Devel/97   4/8/11 1:58p nilesh
 * SW7425-75: Removed stInfo from BAVC_VideoBufferStatus
 *
 * Hydra_Software_Devel/96   4/8/11 12:54p nilesh
 * SW7425-75: Removed constant metadata to Buffer Info
 *
 * Hydra_Software_Devel/95   4/7/11 4:01p nilesh
 * SW7425-75: Add support for video metadata (common)
 *
 * Hydra_Software_Devel/94   4/7/11 11:33a nilesh
 * SW7425-287: Reorder local variables in Data Unit detection loop
 *
 * Hydra_Software_Devel/93   4/7/11 11:26a nilesh
 * SW7425-287: Further optimize NALU detection by not using too many local
 * variables
 *
 * Hydra_Software_Devel/92   4/7/11 11:19a nilesh
 * SW7425-287: Optimize Data Unit detection loop to maximize use of local
 * variables
 *
 * Hydra_Software_Devel/91   4/7/11 10:56a nilesh
 * SW7425-171: No longer reset CABAC pointers if config is sent in middle
 * of encode.
 *
 * Hydra_Software_Devel/90   4/7/11 10:53a nilesh
 * SW7425-287: Optimized cache flush to only flush region containing new
 * ITB/CDB data
 *
 * Hydra_Software_Devel/89   4/6/11 1:00p nilesh
 * SW7425-287: Reduced the number of descriptors generated by the data
 * unit detection logic
 *
 * Hydra_Software_Devel/88   4/6/11 12:02p nilesh
 * SW7425-289: Normalize TicksPerBit 1 frame skew by setting current ITB
 * entry's TicksPerBit using the next ITB entry
 *
 * Hydra_Software_Devel/87   4/5/11 4:36p nilesh
 * SW7425-172,SW7425-173: Added device (watchdog) and channel (event)
 * callback suppport
 *
 * Hydra_Software_Devel/86   3/31/11 1:21p nilesh
 * SW7425-1: Phase 2.0 Test Release
 *
 * Hydra_Software_Devel/VCE_Phase_1_RC/3   3/16/11 12:23p nilesh
 * SW7425-171: Added BVN Metadata Change Event
 *
 * Hydra_Software_Devel/VCE_Phase_1_RC/2   3/16/11 11:43a nilesh
 * SW7425-1: Rebase from mainline
 *
 * Hydra_Software_Devel/85   3/30/11 4:39p nilesh
 * SW7425-74: Fixed cache flush logic
 *
 * Hydra_Software_Devel/84   3/29/11 4:09p nilesh
 * SW7425-74: Added H.264 NALU detection
 *
 * Hydra_Software_Devel/83   3/17/11 12:35p nilesh
 * SW7425-157: Removed BERR_TRACE from watchdog isr handler
 *
 * Hydra_Software_Devel/82   3/14/11 4:49p nilesh
 * SW7425-1: Added actual ITB/CDB depth in BVCE_DUMP_OUTPUT_ITB_DESC debug
 * dump
 *
 * Hydra_Software_Devel/81   3/14/11 11:30a nilesh
 * SW7425-1: Fixed compiler warnings when DEBUG=n
 *
 * Hydra_Software_Devel/VCE_Phase_1_RC/1   3/9/11 8:55p nilesh
 * SW7425-1: Phase 1.0 FW RC
 *
 * Hydra_Software_Devel/80   3/8/11 8:45p nilesh
 * SW7425-139: Use ITB Valid pointer instead of ITB Write-1 to prevent
 * issues with reading stale ITB data
 *
 * Hydra_Software_Devel/79   3/8/11 11:56a nilesh
 * SW7425-1: Fixed dbg print of DUMP_OUTPUT_ITB_DESC
 *
 * Hydra_Software_Devel/78   3/7/11 10:31p nilesh
 * SW7425-1: Added BVCE_DUMP_OUTPUT_ITB_DESC dbg dump compile flag
 *
 * Hydra_Software_Devel/77   3/7/11 2:18p nilesh
 * SW7425-1: Added compile time debug flags BVCE_DUMP_OUTPUT_[ITB/CDB]
 *
 * Hydra_Software_Devel/76   3/2/11 6:32p nilesh
 * SW7425-141: Changed "EndToEndDelay" to "A2PDelay"
 *
 * Hydra_Software_Devel/75   3/2/11 4:53p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 *
 * Hydra_Software_Devel/74   2/28/11 12:08p nilesh
 * SW7425-1: Added BVCE_CMD and BVCE_RSP debug output modules
 *
 * Hydra_Software_Devel/73   2/24/11 10:34a nilesh
 * SW7425-1: Cleaned up dbg messages
 *
 * Hydra_Software_Devel/72   2/23/11 5:49p nilesh
 * SW7425-1: Utilize BDBG_FILE_MODULE to aid debug
 *
 * Hydra_Software_Devel/71   2/23/11 2:21p nilesh
 * SW7425-1: Clean up
 *
 * Hydra_Software_Devel/70   2/22/11 5:44p nilesh
 * SW7425-102: Cleaned up end-to-end delay API
 *
 * Hydra_Software_Devel/69   2/22/11 3:27p nilesh
 * SW7425-1: Phase 1.0 FW RC
 *
 * Hydra_Software_Devel/68   2/15/11 2:48p nilesh
 * SW7425-1: Changed BVCE_Output_GetBufferDescriptors() prototype to be
 * more accurate
 *
 * Hydra_Software_Devel/67   2/15/11 1:31p nilesh
 * SW7425-96: Cleaned up heap logic to minimize usage of mapped system
 * heap
 *
 * Hydra_Software_Devel/66   2/13/11 12:13a hongtaoz
 * SW7425-96: separate ViCE2 FW heap and picture heap;
 *
 * Hydra_Software_Devel/65   2/8/11 12:36p nilesh
 * SW7425-1: Changed how protocol, profile, and level are specified
 *
 * Hydra_Software_Devel/64   2/7/11 10:56a nilesh
 * SW7425-1: Removed unused end-to-end delay setting
 *
 * Hydra_Software_Devel/63   2/4/11 2:37p nilesh
 * SW7425-1: Fixed large frame size bug caused by incorrect use of CDB
 * Valid vs Write when CDB pointers don't change between two calls to
 * BVCE_GetBufferDescriptors()
 *
 * Hydra_Software_Devel/62   2/3/11 4:49p nilesh
 * SW7425-1: Changed uint32_t to unsigned wherever applicable
 *
 * Hydra_Software_Devel/61   2/2/11 12:46p nilesh
 * SW7425-1: Changed BVCE_Channel_AddUserDataBuffers() prototype
 *
 * Hydra_Software_Devel/60   2/1/11 4:17p nilesh
 * SW7425-1: Combined common portions of audio/video compressed buffer
 * descriptors
 *
 * Hydra_Software_Devel/59   2/1/11 12:59p nilesh
 * SW7425-1: Fixed frame start detection if a picture is larger than CDB
 * depth
 *
 * Hydra_Software_Devel/58   1/26/11 5:41p nilesh
 * SW7425-1: Fix MB Arc debug log
 *
 * Hydra_Software_Devel/57   1/26/11 4:44p nilesh
 * SW7425-1: API Changes
 *
 * Hydra_Software_Devel/56   1/25/11 10:54a nilesh
 * SW7425-1: Optional CDB low/high watermark reporting
 *
 * Hydra_Software_Devel/55   1/20/11 4:21p nilesh
 * SW7425-1: CDB Pointers are now sampled prior to ITB Pointers to prevent
 * race condition when ITB is updated between ITB/CDB read sequence.  CDB
 * Valid vs CDB Write Pointer selection logic improved to allow for
 * pictures up to (CDB_Size - 32).
 *
 * Hydra_Software_Devel/54   1/20/11 3:22p nilesh
 * SW7425-1: Ignore dts_valid field in ITB since DTS is always sent
 *
 * Hydra_Software_Devel/53   1/19/11 2:06p nilesh
 * SW7425-1: Fixed off by 1 error when using actual cdb valid ptr
 *
 * Hydra_Software_Devel/52   1/18/11 3:13p nilesh
 * SW7425-1: Added EOS ITB Support
 *
 * Hydra_Software_Devel/51   1/17/11 1:54p nilesh
 * SW7425-1: Updated FW, Added ITB/CDB buffer/full ambiguity fix (actual
 * READ ptr now points to byte last read instead of next byte to be
 * read), Using WRITE-1 instead of VALID to work around bug where VALID
 * only updated at the end of a picture.
 *
 * Hydra_Software_Devel/50   1/14/11 11:12a nilesh
 * SW7425-1: Added MB Arc debug log support
 *
 * Hydra_Software_Devel/49   12/30/10 4:44p nilesh
 * SW7425-1: Temporarily fixed shutdown sequence
 *
 * Hydra_Software_Devel/48   12/29/10 3:49p nilesh
 * SW7425-1: Now using memory sizes provided in bvce_fw_api.h
 *
 * Hydra_Software_Devel/47   12/28/10 2:38p nilesh
 * SW7425-1: ITB Read Offset is now updated.  Fixed ITB wrap around bug.
 *
 * Hydra_Software_Devel/46   12/27/10 6:11p nilesh
 * SW7425-1: Add API to allow selection of STC that encoder should use
 *
 * Hydra_Software_Devel/45   12/23/10 3:58p nilesh
 * SW7425-38: Fix ITB/CDB Wrap Logic
 *
 * Hydra_Software_Devel/44   12/7/10 11:01a nilesh
 * SW7425-1: Fixed debug print typo
 *
 * Hydra_Software_Devel/43   12/7/10 10:48a nilesh
 * SW7425-1: Fixed typo causing seg fault
 *
 * Hydra_Software_Devel/42   12/7/10 10:42a nilesh
 * SW7425-1: Fixed debug print
 *
 * Hydra_Software_Devel/41   12/6/10 11:52a nilesh
 * SW7425-1: Moved mailbox register config to platform config
 *
 * Hydra_Software_Devel/40   12/3/10 12:17p nilesh
 * SW7425-1: No longer used cached memory when loading firmware
 *
 * Hydra_Software_Devel/39   12/1/10 3:42p nilesh
 * SW7425-1: Updated delay mode API.  Added logic that verifies BVN
 * metadata address at compile-time and run-time.
 *
 * Hydra_Software_Devel/38   11/18/10 4:26p nilesh
 * SW7425-1: Populate protocol in video buffer descriptor
 *
 * Hydra_Software_Devel/37   11/18/10 12:09p nilesh
 * SW7425-1: Fix watchdog
 *
 * Hydra_Software_Devel/36   11/18/10 11:30a nilesh
 * SW7425-1: Commented out helper functions until API is finalized
 *
 * Hydra_Software_Devel/35   11/17/10 4:41p nilesh
 * SW7425-1: Separated encode mode (static) and delay mode (dynamic)
 * settings
 *
 * Hydra_Software_Devel/34   11/17/10 3:56p nilesh
 * SW7425-1: Added more debug info when polling mailbox response.
 *
 * Hydra_Software_Devel/33   11/17/10 1:23p nilesh
 * SW7425-1: Moved MIPs debug register definitions to platform specific
 * file
 *
 * Hydra_Software_Devel/32   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 *
 * Hydra_Software_Devel/31   11/16/10 4:08p nilesh
 * SW7425-1: Fixed compiler warning
 *
 * Hydra_Software_Devel/30   11/11/10 12:04p nilesh
 * SW7425-1: Fixed interrupt handling.  Disabled until FW updated to use
 * correct interrupt bits.
 *
 * Hydra_Software_Devel/29   11/8/10 2:39p nilesh
 * SW7425-1: Fixed typo in configuring debug log mode
 *
 * Hydra_Software_Devel/28   11/8/10 1:28p nilesh
 * SW7425-1: Updated FW. Supports VCE PI provided debug log buffer.
 * Supports setting cabac endianess.  Supports static BVN2VICE metadata
 * address.
 *
 * Hydra_Software_Devel/27   11/4/10 2:51p nilesh
 * SW7425-1: Fixed compiler warning
 *
 * Hydra_Software_Devel/26   11/4/10 2:47p nilesh
 * SW7425-1: Update DRAM Log logic to use absolute read/write pointers.
 * Temporarily use FW allocated DRAM log.
 *
 * Hydra_Software_Devel/25   11/2/10 1:22p nilesh
 * SW7425-1: Update actual debug log buffer read offset in DCCM memory
 *
 * Hydra_Software_Devel/24   11/2/10 12:25p nilesh
 * SW7425-1: Added Debug Log Support for Pic Arc
 *
 * Hydra_Software_Devel/23   11/1/10 3:06p nilesh
 * SW7425-1: Fix potential overflow in frame rate calculation
 *
 * Hydra_Software_Devel/22   10/29/10 11:13p nilesh
 * SW7425-1: Fixed ITB/CDB Buffer Management.  Implemented low-delay mode.
 *
 * Hydra_Software_Devel/20   10/27/10 9:25a nilesh
 * SW7425-1: Minor Tweaks
 *
 * Hydra_Software_Devel/19   10/26/10 6:19p hongtaoz
 * SW7425-1: fixed a typo of ITB shadow read offset at initial;
 *
 * Hydra_Software_Devel/18   10/26/10 2:24p nilesh
 * SW7425-1: Initialize num video buffer descriptors
 *
 * Hydra_Software_Devel/17   10/26/10 1:45p nilesh
 * SW7425-1: CDB/ITB tweaks
 *
 * Hydra_Software_Devel/16   10/25/10 4:19p nilesh
 * SW7425-1: Added stripe width/multiple lookup using BCHP
 *
 * Hydra_Software_Devel/15   10/25/10 12:00p nilesh
 * SW7425-1: Updated to VCE FW released on 2010.10.25
 *
 * Hydra_Software_Devel/14   10/21/10 4:57p nilesh
 * SW7425-1: Fixed BVN Metadata Register Address Reporting
 *
 * Hydra_Software_Devel/13   10/21/10 4:08p nilesh
 * SW7425-1: Optimized BVCE_Channel_GetStatus()
 *
 * Hydra_Software_Devel/12   10/21/10 1:17p nilesh
 * SW7425-1: Added Data Ready Callback Support
 *
 * Hydra_Software_Devel/11   10/21/10 11:03a nilesh
 * SW7425-1: Support query of BVN2VICE metadata offset via channel status
 *
 * Hydra_Software_Devel/10   10/20/10 5:09p nilesh
 * SW7425-1: Fixed typo in debug message
 *
 * Hydra_Software_Devel/9   10/20/10 5:01p nilesh
 * SW7425-1: Fixed StartEncode
 *
 * Hydra_Software_Devel/8   10/19/10 5:13p nilesh
 * SW7425-1: Updated to use new AFL function prototype
 *
 * Hydra_Software_Devel/7   10/19/10 8:58a nilesh
 * SW7425-1: Fixed FW Boot/Init
 *
 * Hydra_Software_Devel/6   10/18/10 11:52a nilesh
 * SW7425-1: Fixed polling mechanism for VCE Boot
 *
 * Hydra_Software_Devel/5   10/13/10 5:04p nilesh
 * SW7425-1: Added error interrupt callback support.  Implemented
 * BVCE_Channel_[Get/Clear]Status.
 *
 * Hydra_Software_Devel/4   10/13/10 12:10p nilesh
 * SW7425-1: Implemented BVCE_Channel_[Open/Close],
 * BVCE_Channel_[Start/Stop]Encode, and
 * BVCE_Channel_[Set/Get]EncodeSettings
 *
 * Hydra_Software_Devel/3   10/12/10 1:44p nilesh
 * SW7425-1: Implemented subheap allocation.  Implemented BVCE_Output_xxx
 * functions.
 *
 * Hydra_Software_Devel/2   10/7/10 4:47p nilesh
 * SW7425-1: Updated to compile with BAFL api change
 *
 * Hydra_Software_Devel/1   10/7/10 11:09a nilesh
 * SW7425-1: Implemented BVCE_Open/BVCE_Close
 *
 ***************************************************************************/

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#include "btmr.h"
#include "bafl.h"

#include "bavc.h"
#include "bavc_vce_mbox.h"

#include "bvce.h"
#include "bvce_priv.h"
#include "bvce_image.h"

#include "bvce_platform.h"
#include "bvce_fw_api.h"
#include "bvce_debug.h"
#include "bvce_telem.h"

#include "bvce_output.h"
#include "bvce_power.h"

BDBG_MODULE(BVCE);
BDBG_FILE_MODULE(BVCE_CMD);
BDBG_FILE_MODULE(BVCE_RSP);
BDBG_FILE_MODULE(BVCE_USERDATA);

/**********************/
/* Encoder Open/Close */
/**********************/
static const BVCE_OpenSettings s_stDefaultOpenSettings =
{
 BVCE_P_SIGNATURE_OPENSETTINGS, /* Signature */

 0, /* Instance */

 /* Memory and Heaps */
 {
  NULL,
  NULL
 },
 NULL, /* General Heap */
 NULL, /* Picture Heap */
 NULL, /* Secure Heap */
 /* Memory Config */
 {
  MIN_GENERAL_BUFFER_SIZE_IN_BYTES*BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS,
  MIN_PICTURE_BUFFER_SIZE_IN_BYTES*BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS,
  MIN_SECURE_BUFFER_SIZE_IN_BYTES*BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS,
 },

 /* Firmware and Boot */
 NULL, /* Image Interface */
 NULL, /* Image Context */
 NULL, /* Boot Callback */
 NULL, /* Boot Callback Data */

 /* Debug */
 NULL, /* Timer Handle */
 /* Debug Log Buffer Size */
 {
  BVCE_P_DEFAULT_DEBUG_LOG_SIZE,
  BVCE_P_DEFAULT_DEBUG_LOG_SIZE,
 },
 /* Debug Log Buffering Mode */
 {
  BVCE_Debug_BufferingMode_eDiscardNewData, /* TODO: Which default do we want to PicArc? */
  BVCE_Debug_BufferingMode_eDiscardNewData  /* TODO: Which default do we want to MBArc? */
 },
 false, /* Verification Mode */
 false, /* A2N Picture Drop */
};

void
BVCE_GetDefaultOpenSettings(
         BVCE_OpenSettings  *pstOpenSettings /* [out] Default VCE settings */
         )
{
   BDBG_ENTER( BVCE_GetDefaultOpenSettings );

   BDBG_ASSERT( pstOpenSettings );

   if ( NULL != pstOpenSettings )
   {
      *pstOpenSettings = s_stDefaultOpenSettings;

#if !(BVCE_USE_CUSTOM_IMAGE)
      pstOpenSettings->pImgInterface = &BVCE_IMAGE_Interface;
#if !(BVCE_USE_CUSTOM_CONTEXT)
      pstOpenSettings->pImgContext = BVCE_IMAGE_Context;
#endif
#endif

   }

   BDBG_LEAVE( BVCE_GetDefaultOpenSettings );
   return;
}

BERR_Code
BVCE_P_CreateTimer(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BVCE_P_CreateTimer );

   if ( NULL != hVce->stOpenSettings.hTimer )
   {
      BTMR_Settings tmrSettings;

      rc = BTMR_GetDefaultTimerSettings(&tmrSettings);

      if (BERR_SUCCESS == rc)
      {
         tmrSettings.type = BTMR_Type_eSharedFreeRun;
         tmrSettings.exclusive = false;

         rc = BTMR_CreateTimer(hVce->stOpenSettings.hTimer,
                               &hVce->hTimer,
                               &tmrSettings);

      }
   }

   BDBG_LEAVE( BVCE_P_CreateTimer );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_DestroyTimer(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BVCE_P_DestroyTimer );

   if ( NULL != hVce->hTimer )
   {
      rc = BTMR_DestroyTimer(
               hVce->hTimer
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_WRN(("Error destroying timer"));
      }
   }

   BDBG_LEAVE( BVCE_P_DestroyTimer );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_Reset(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_Reset );

   BERR_TRACE( BVCE_Platform_P_PreReset( hVce->stOpenSettings.uiInstance, hVce->handles.hReg ) );

   rc = BVCE_Platform_P_WriteRegisterList(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stViceReset.astRegisterSettings,
            hVce->stPlatformConfig.stViceReset.uiRegisterCount
            );

   BDBG_LEAVE( BVCE_P_Reset );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_AllocateDeviceMemory(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BVCE_P_AllocateDeviceMemory );

   /* Allocate memory */
   hVce->cabac_cmd_memory.hMem = hVce->channel_memory.hMem[BVCE_P_HeapId_eSecure];
   hVce->cabac_cmd_memory.uiBufferSize = MIN_CMD_BUFFER_SIZE_IN_BYTES;
   hVce->cabac_cmd_memory.pBufferUnCached = BMEM_AllocAligned(
         hVce->cabac_cmd_memory.hMem,
         hVce->cabac_cmd_memory.uiBufferSize,
         9,
         0
         );

   if ( NULL == hVce->cabac_cmd_memory.pBufferUnCached )
   {
      BDBG_ERR(("Error allocating memory"));
      return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
   }

   /* Get offset */
   rc = BMEM_ConvertAddressToOffset(
         hVce->cabac_cmd_memory.hMem,
         hVce->cabac_cmd_memory.pBufferUnCached,
         &hVce->cabac_cmd_memory.uiBufferOffset
         );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error converting virtual address to offset"));
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_AllocateDeviceMemory );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_FreeDeviceMemory(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BVCE_P_FreeDeviceMemory );

   /* Free Memory */
   if ( NULL != hVce->cabac_cmd_memory.pBufferUnCached )
   {
      BMEM_Free(
            hVce->cabac_cmd_memory.hMem,
            hVce->cabac_cmd_memory.pBufferUnCached
            );
   }

   BDBG_LEAVE( BVCE_P_FreeDeviceMemory );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_AllocateChannelMemory(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BMEM_Settings memModuleSettings;
   BMEM_Heap_Settings memHeapSettings;
   BVCE_P_HeapId eHeapId;

   BDBG_ENTER( BVCE_P_AllocateChannelMemory );

   /* FW doesn't support different general/picture memory buffers so combine general/picture memory */
   hVce->channel_memory.stHeapInfo[BVCE_P_HeapId_ePicture].uiBufferSize = hVce->stOpenSettings.stMemoryConfig.uiGeneralMemSize + hVce->stOpenSettings.stMemoryConfig.uiPictureMemSize;
   hVce->channel_memory.stHeapInfo[BVCE_P_HeapId_eSecure].uiBufferSize = hVce->stOpenSettings.stMemoryConfig.uiSecureMemSize;

   /* Create BMEM Module  */
   rc = BMEM_GetDefaultSettings(
            &memModuleSettings
            );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error getting default memory module settings"));
      return BERR_TRACE( rc );
   }

   rc = BMEM_Open(
            &hVce->channel_memory.hMemModule,
            &memModuleSettings
            );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error opening memory module"));
      return BERR_TRACE( rc );
   }

   for ( eHeapId = 0; eHeapId < BVCE_P_HeapId_eMax; eHeapId++ )
   {
      /* Allocate Memory and Create Sub Heap(s) */
      if ( 0 != hVce->channel_memory.stHeapInfo[eHeapId].uiBufferSize )
      {
         hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached = BMEM_AllocAligned(
                  hVce->channel_memory.hMem[eHeapId],
                  hVce->channel_memory.stHeapInfo[eHeapId].uiBufferSize,
                  9,
                  0
                  );

         if ( NULL == hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached )
         {
            BDBG_ERR(("Error allocating memory"));
            return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
         }

         /* Get offset */
         rc = BMEM_ConvertAddressToOffset(
                  hVce->channel_memory.hMem[eHeapId],
                  hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached,
                  &hVce->channel_memory.stHeapInfo[eHeapId].uiBufferOffset
                  );
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error converting virtual address to offset"));
            return BERR_TRACE( rc );
         }

         /* Get Cached Address */
         rc = BMEM_ConvertAddressToCached(
                  hVce->channel_memory.hMem[eHeapId],
                  hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached,
                  &hVce->channel_memory.stHeapInfo[eHeapId].pBufferCached
                  );
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error converting general virtual address to cached"));
            return BERR_TRACE( rc );
         }

         /* Create BMEM Heap */
         rc = BMEM_Heap_GetDefaultSettings(
                  &memHeapSettings
                  );
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error getting default memory heap settings"));
            return BERR_TRACE( rc );
         }

         memHeapSettings.eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;
         memHeapSettings.eSafetyConfig = BMEM_CONFIG_FASTEST;

         rc = BMEM_Heap_Create(
                  hVce->channel_memory.hMemModule,
                  hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached,
                  hVce->channel_memory.stHeapInfo[eHeapId].uiBufferOffset,
                  hVce->channel_memory.stHeapInfo[eHeapId].uiBufferSize,
                  &memHeapSettings,
                  &hVce->channel_memory.stHeapInfo[eHeapId].hMem
                  );
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error getting creating memory heap"));
            return BERR_TRACE( rc );
         }
      }
   }

   BDBG_LEAVE( BVCE_P_AllocateChannelMemory );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_FreeChannelMemory(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BVCE_P_HeapId eHeapId;

   BDBG_ENTER( BVCE_P_FreeChannelMemory );

   /* Destroy Sub Heaps */
   for ( eHeapId = 0; eHeapId < BVCE_P_HeapId_eMax; eHeapId++ )
   {
      if ( NULL != hVce->channel_memory.stHeapInfo[eHeapId].hMem )
      {
         BMEM_Heap_Destroy(
                  hVce->channel_memory.stHeapInfo[eHeapId].hMem
                  );
      }

      if ( NULL != hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached )
      {
         BMEM_Free(
                  hVce->channel_memory.hMem[eHeapId],
                  hVce->channel_memory.stHeapInfo[eHeapId].pBufferUnCached
                  );
      }
   }

   /* Destroy Memory Module */
   if ( NULL != hVce->channel_memory.hMemModule )
   {
      BMEM_Close(
               hVce->channel_memory.hMemModule
               );
   }

   BDBG_LEAVE( BVCE_P_FreeChannelMemory );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_AllocateFirmwareMemory(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   uint32_t i;

   BDBG_ENTER( BVCE_P_AllocateFirmwareMemory );

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      hVce->fw.uiCodeSize[i] = BVCE_PLATFORM_P_FW_CODE_SIZE;
      hVce->fw.pCodeBufferUnCached[i] = BMEM_AllocAligned(
               hVce->fw.hMem[i],
               hVce->fw.uiCodeSize[i],
               12,
               0
               );

      if ( NULL == hVce->fw.pCodeBufferUnCached[i] )
      {
         BDBG_ERR(("Error allocating FW code buffer [%d]", i));
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }

      rc = BMEM_ConvertAddressToCached(
               hVce->fw.hMem[i],
               hVce->fw.pCodeBufferUnCached[i],
               &hVce->fw.pCodeBufferCached[i]
               );

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting FW code buffer address to cached [%d]", i));
         return BERR_TRACE( rc );
      }
   }

   BDBG_LEAVE( BVCE_P_AllocateFirmwareMemory );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_FreeFirmwareMemory(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;

   uint32_t i;

   BDBG_ENTER( BVCE_P_FreeFirmwareMemory );

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      if ( NULL != hVce->fw.pCodeBufferUnCached[i] )
      {
         rc = BMEM_Free(
                  hVce->fw.hMem[i],
                  hVce->fw.pCodeBufferUnCached[i]
                  );

         if ( BERR_SUCCESS != rc )
         {
            BDBG_WRN(("Error freeing FW code buffer [%d]", i));
         }
      }
   }

   BDBG_LEAVE( BVCE_P_FreeFirmwareMemory );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_LoadFirmware(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;
   uint32_t i;
   uint32_t uiRegValue;
   uint32_t uiCodeStart;

   BDBG_ENTER( BVCE_P_LoadFirmware );

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      if ( NULL != hVce->stOpenSettings.pARCBootCallback )
      {
         /* If boot call back is set, zero out FW image.  */
         BKNI_Memset(hVce->fw.pCodeBufferCached[i], 0, hVce->fw.uiCodeSize[i]);
      }

      rc = BAFL_Load(
               hVce->stOpenSettings.pImgInterface,
               hVce->stOpenSettings.pImgContext,
               i,
               hVce->fw.pCodeBufferCached[i], /* Virtual Address */
               hVce->fw.uiCodeSize[i],
               false,
               &hVce->fw.astFirmwareLoadInfo[i]
               );

      if ( BERR_SUCCESS != rc )
      {
         return BERR_TRACE( rc );
      }

      BMEM_FlushCache(
         hVce->fw.hMem[i],
         hVce->fw.pCodeBufferCached[i],
         hVce->fw.uiCodeSize[i]
      );
   }

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      /* Write Arc[0] Code/Size registers */
      BMEM_ConvertAddressToOffset(
               hVce->fw.hMem[i],
               hVce->fw.astFirmwareLoadInfo[i].stCode.pStartAddress,
               &uiCodeStart
               );

      BDBG_MSG(("Arc[%d] FW Loaded paddr=%08x vaddr=%08x caddr=%08x",
               i,
               uiCodeStart,
               hVce->fw.pCodeBufferUnCached[i],
               hVce->fw.pCodeBufferCached[i]));

      uiRegValue = uiCodeStart;

      BREG_Write32(
               hVce->handles.hReg,
               hVce->stPlatformConfig.stCore[i].uiInstructionStartPhysicalAddress,
               uiRegValue
               );


#if BDBG_DEBUG_BUILD
         {
           uint32_t uiRegValueActual = BREG_Read32(
                                                   hVce->handles.hReg,
                                                   hVce->stPlatformConfig.stCore[i].uiInstructionStartPhysicalAddress
                                                   );

           BDBG_MSG(("@0x%08x <-- 0x%08x (0x%08x) - ARC[%d] %s",
                    hVce->stPlatformConfig.stCore[i].uiInstructionStartPhysicalAddress,
                    uiRegValue,
                    uiRegValueActual,
                    i,
                    "Instruction Start Address"
                    ));
         }
#endif

      /* NOTE: This code assumes the data is loaded AFTER the code */
   #if 0
      BDBG_ASSERT( hVce->fw.astFirmwareLoadInfo[0].stCode.uiOffset < hVce->fw.astFirmwareLoadInfo[0].stData.uiOffset );
   #endif
      BMEM_ConvertAddressToOffset(
               hVce->fw.hMem[i],
               hVce->fw.astFirmwareLoadInfo[i].stData.pStartAddress,
               &uiRegValue
               );

      uiRegValue -= uiCodeStart;
      uiRegValue += hVce->fw.astFirmwareLoadInfo[i].stData.uiSize;

      BREG_Write32(
               hVce->handles.hReg,
               hVce->stPlatformConfig.stCore[i].uiDataSpaceStartRelativeOffset,
               uiRegValue
               );

      if ( 0 != hVce->stPlatformConfig.stCore[i].uiDataSpaceStartSystemOffset )
      {
         uiRegValue = i ? 0x00000 : 0x00000; /* This is hard-coded */

         BREG_Write32(
                  hVce->handles.hReg,
                  hVce->stPlatformConfig.stCore[i].uiDataSpaceStartSystemOffset,
                  uiRegValue
                  );
      }

#if BDBG_DEBUG_BUILD
      {
           uint32_t uiRegValueActual = BREG_Read32(
                                                   hVce->handles.hReg,
                                                   hVce->stPlatformConfig.stCore[i].uiDataSpaceStartRelativeOffset
                                                   );

           BDBG_MSG(("@0x%08x <-- 0x%08x (0x%08x) - ARC[%d] %s",
                    hVce->stPlatformConfig.stCore[i].uiDataSpaceStartRelativeOffset,
                    uiRegValue,
                    uiRegValueActual,
                    i,
                    "Data Start Address"
                    ));
         }
#endif
   }

   BDBG_LEAVE( BVCE_P_LoadFirmware );

   return BERR_TRACE( BERR_SUCCESS );
}

void BVCE_P_Mailbox_isr(
         void *pContext,
         int iParam
         )
{
   BVCE_Handle hVce = (BVCE_Handle) pContext;
   BSTD_UNUSED(iParam);

   BDBG_ENTER( BVCE_P_Mailbox_isr );

   BKNI_SetEvent_isr( hVce->events.hMailbox );

   BDBG_LEAVE( BVCE_P_Mailbox_isr );
}

void BVCE_P_Watchdog_isr(
         void *pContext,
         int iParam
         )
{
   BVCE_Handle hVce = ( BVCE_Handle ) pContext;
   unsigned uiWatchdogErrorCode = 0;
   unsigned uiProgramCounter = 0;

   BDBG_ENTER( BVCE_P_Watchdog_isr );
   BSTD_UNUSED(iParam);

   if ( 0 != hVce->fw.dccm.uiWatchdogErrorCodeBaseAddress[iParam] )
   {
      uiWatchdogErrorCode = BREG_Read32(
                  hVce->handles.hReg,
                  hVce->fw.dccm.uiWatchdogErrorCodeBaseAddress[iParam]
                  );
   }

   if ( 0 != hVce->stPlatformConfig.stDebug.uiPicArcPC )
   {
      uiProgramCounter = BREG_Read32(
                  hVce->handles.hReg,
                  hVce->stPlatformConfig.stDebug.uiPicArcPC
                  );
   }

   BDBG_ERR(("ARC[%d] Watchdog w/ error code: %08x! (PicARC PC = %08x)", iParam, uiWatchdogErrorCode, uiProgramCounter));

   if ( ( true == hVce->callbacks.stCallbackSettings.stWatchdog.bEnable )
        && ( NULL != hVce->callbacks.stCallbackSettings.stWatchdog.fCallback )
      )
   {
      hVce->callbacks.stCallbackSettings.stWatchdog.fCallback(
               hVce->callbacks.stCallbackSettings.stWatchdog.pPrivateContext,
               hVce->callbacks.stCallbackSettings.stWatchdog.iPrivateParam,
               NULL
               );
   }

   BDBG_LEAVE( BVCE_P_Watchdog_isr );
}

void BVCE_P_Event_isr(
         void *pContext,
         int iParam
         )
{
   BVCE_Handle hVce = ( BVCE_Handle ) pContext;
   uint32_t uiChannelErrorStatus;
   uint32_t i;

   BDBG_ENTER( BVCE_P_Event_isr );

   BSTD_UNUSED(iParam);

   /* We handle the device error interrupt by reading the
    * channel error flags and then executing the channel's error
    * callback iff that channel is marked as having an error
    */

   /* Read device channel error status flag */
   uiChannelErrorStatus = BREG_Read32(
               hVce->handles.hReg,
               hVce->fw.dccm.uiChannelErrorStatusBaseAddress
               );

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS; i++ )
   {
      if ( 0 != ( ( uiChannelErrorStatus >> i ) & 0x01 ) )
      {
         /* Trigger Channel's Error Callback */
         if ( ( true == hVce->channels[i].context.stCallbackSettings.stEvent.bEnable )
              && ( NULL != hVce->channels[i].context.stCallbackSettings.stEvent.fCallback )
            )
         {
            BDBG_MSG(("Executing channel[%d] error callback", i));
            hVce->channels[i].context.stCallbackSettings.stEvent.fCallback(
                     hVce->channels[i].context.stCallbackSettings.stEvent.pPrivateContext,
                     hVce->channels[i].context.stCallbackSettings.stEvent.iPrivateParam,
                     NULL
                     );
         }
      }
   }

   BDBG_LEAVE( BVCE_P_Event_isr );
}

void BVCE_P_DataReady_isr(
         void *pContext,
         int iParam
         )
{
   BVCE_Handle hVce = ( BVCE_Handle ) pContext;

   BDBG_ENTER( BVCE_P_DataReady_isr );

   BSTD_UNUSED( iParam );

   hVce->outputs[iParam].context.state.bCabacInitialized = true;

   BDBG_LEAVE( BVCE_P_DataReady_isr );
}

BERR_Code
BVCE_P_EnableInterrupts(
         BVCE_Handle hVce
         )
{
   BDBG_ENTER( BVCE_P_EnableInterrupts );

   BVCE_Platform_P_WriteRegisterList(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stViceInterruptEnable.astRegisterSettings,
            hVce->stPlatformConfig.stViceInterruptEnable.uiRegisterCount
            );

   BDBG_LEAVE( BVCE_P_EnableInterrupts );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_DisableInterrupts(
         BVCE_Handle hVce
         )
{
   BDBG_ENTER( BVCE_P_DisableInterrupts );

   BVCE_Platform_P_WriteRegisterList(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stViceInterruptDisable.astRegisterSettings,
            hVce->stPlatformConfig.stViceInterruptDisable.uiRegisterCount
            );

   BDBG_LEAVE( BVCE_P_DisableInterrupts );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_SetupCallback(
         BVCE_Handle hVce,
         BINT_CallbackHandle *phCallback,
         BINT_Id intId,
         BINT_CallbackFunc func, /* [in] Callback function that should be called when the specified interrupt triggers */
         void * pParm1, /* [in] Parameter that is returned to callback function when interrupt triggers */
         int parm2 /* [in] Parameter that is returned to callback function when interrupt triggers */
         )
{
   BERR_Code rc;
   BDBG_ENTER( BVCE_P_SetupCallback );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( phCallback );

   if ( NULL != *phCallback )
   {
      rc = BINT_DisableCallback(
               *phCallback
               );
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   } else {
      rc = BINT_CreateCallback(
               phCallback,
               hVce->handles.hInt,
               intId,
               func,
               pParm1,
               parm2
               );

      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   rc = BINT_EnableCallback(
            *phCallback
            );
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

   BDBG_LEAVE( BVCE_P_SetupCallback );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_TeardownCallback(
         BINT_CallbackHandle hCallback
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BVCE_P_TeardownCallback );

   if ( NULL != hCallback )
   {
      rc = BERR_TRACE( BINT_DisableCallback(
               hCallback
               ));

      rc = BERR_TRACE( BINT_DestroyCallback(
               hCallback
               ));
   }

   BDBG_LEAVE( BVCE_P_TeardownCallback );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_SetupInterrupts(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER( BVCE_P_SetupInterrupts );

   /* Install BINT ISR Handlers */
   /* Mailbox Interrupt  */
   rc = BVCE_P_SetupCallback(
            hVce,
            &hVce->callbacks.hMailbox,
            hVce->stPlatformConfig.stInterrupt.idMailbox,
            BVCE_P_Mailbox_isr,
            hVce,
            0
            );
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* Arc Watchdog Interrupts */
   {
      uint32_t i = 0;
      for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
      {
         rc = BVCE_P_SetupCallback(
                  hVce,
                  &hVce->callbacks.ahWatchdog[i],
                  hVce->stPlatformConfig.stInterrupt.idWatchdog[i],
                  BVCE_P_Watchdog_isr,
                  hVce,
                  i
                  );
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }
      }
   }

   /* Error interrupt */
   rc = BVCE_P_SetupCallback(
            hVce,
            &hVce->callbacks.hEvent,
            hVce->stPlatformConfig.stInterrupt.idEvent,
            BVCE_P_Event_isr,
            hVce,
            0
            );
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

#if !BVCE_P_POLL_FW_DATAREADY
   /* Data Ready */
   {
      uint32_t i = 0;

      for ( i = 0; i < BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS; i++ )
      {
         if ( 0 != hVce->stPlatformConfig.stInterrupt.idDataReady[i] )
         {
            rc = BVCE_P_SetupCallback(
               hVce,
               &hVce->callbacks.ahDataReady[i],
               hVce->stPlatformConfig.stInterrupt.idDataReady[i],
               BVCE_P_DataReady_isr,
               hVce,
               i
            );
         }
      }
      if (rc != BERR_SUCCESS )
      {
         return BERR_TRACE(rc);
      }
   }
#endif

   BDBG_LEAVE( BVCE_P_SetupInterrupts );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_TeardownInterrupts(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_TeardownInterrupts );

   /* Uninstall BINT ISR Handlers */
   /* Mailbox Interrupt  */
   rc = BVCE_P_TeardownCallback(
            hVce->callbacks.hMailbox
            );
   hVce->callbacks.hMailbox = NULL;
   if ( BERR_SUCCESS != rc )
   {
      BDBG_WRN(("Error destroying mailbox callback"));
   }

   /* Arc Watchdog Interrupts */
   {
      uint32_t i = 0;

      for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
      {
         rc = BVCE_P_TeardownCallback(
                  hVce->callbacks.ahWatchdog[i]
                  );
         hVce->callbacks.ahWatchdog[i] = NULL;
         if ( BERR_SUCCESS != rc )
         {
            BDBG_WRN(("Error destroying watchdog[%d] callback", i));
         }
      }
   }

   /* Error interrupt */
   rc = BVCE_P_TeardownCallback(
            hVce->callbacks.hEvent
            );
   hVce->callbacks.hEvent = NULL;
   if ( BERR_SUCCESS != rc )
   {
      BDBG_WRN(("Error destroying error callback"));
   }

   /* Data Ready */
   {
      uint32_t i = 0;

      for ( i = 0; i < BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS; i++ )
      {
         if ( NULL != hVce->callbacks.ahDataReady[i] )
         {
            rc = BVCE_P_TeardownCallback(
                     hVce->callbacks.ahDataReady[i]
                     );
            hVce->callbacks.ahDataReady[i] = NULL;
         }
      }

      if (rc != BERR_SUCCESS )
      {
         BDBG_WRN(("Error destroying dataready[%d] callback", i));
      }
   }

   BDBG_LEAVE( BVCE_P_TeardownInterrupts );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_CreateEvents(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_CreateEvents );

   rc = BKNI_CreateEvent(
            &hVce->events.hMailbox
            );

   BDBG_LEAVE( BVCE_P_CreateEvents );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_DestroyEvents(
         BVCE_Handle hVce
         )
{
   BDBG_ENTER( BVCE_P_DestroyEvents );

   if ( NULL != hVce->events.hMailbox )
   {
      BKNI_DestroyEvent(
               hVce->events.hMailbox
               );
   }

   BDBG_LEAVE( BVCE_P_DestroyEvents );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_Boot(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_Boot );

   hVce->fw.dccm.uiRegisterBaseAddress[0] = hVce->stPlatformConfig.stCore[0].uiDCCMBase;
   hVce->fw.dccm.uiRegisterBaseAddress[1] = hVce->stPlatformConfig.stCore[1].uiDCCMBase;

#if BVCE_P_POLL_FW_MBX
   /* zero out the response mailbox*/

   BREG_Write32(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stMailbox.uiVice2HostMailboxAddress,
            0
            );

   BDBG_MSG(("ARC[0] PC = %08x, STATUS32 = %08x",
	     BREG_Read32(
			 hVce->handles.hReg,
			 hVce->stPlatformConfig.stDebug.uiPicArcPC
			 ),
	     BREG_Read32(
			 hVce->handles.hReg,
			 hVce->stPlatformConfig.stDebug.uiPicArcStatus32
			 )
	     ));
#endif

   /* Boot Core */
   if ( NULL == hVce->stOpenSettings.pARCBootCallback )
   {
      BVCE_Platform_P_WriteRegisterList(
               hVce->handles.hReg,
               hVce->stPlatformConfig.stViceBoot.astRegisterSettings,
               hVce->stPlatformConfig.stViceBoot.uiRegisterCount
               );
   }
   else
   {
      uint32_t i;
      BAFL_BootInfo stBootInfo;
      BAFL_FirmwareInfo stFirmwareInfo[BVCE_PLATFORM_P_NUM_ARC_CORES];


      BKNI_Memset(
               &stBootInfo,
               0,
               sizeof( BAFL_BootInfo )
               );

      BKNI_Memset(
               &stFirmwareInfo,
               0,
               sizeof( BAFL_FirmwareInfo ) * BVCE_PLATFORM_P_NUM_ARC_CORES
               );


      for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
      {
         stFirmwareInfo[i].stCode = hVce->fw.astFirmwareLoadInfo[i].stCode;
         stFirmwareInfo[i].uiArcInstance = i;

         if ( i != BVCE_PLATFORM_P_NUM_ARC_CORES - 1)
         {
            stFirmwareInfo[i].pNext = &stFirmwareInfo[i + 1];
         }
      }

      stBootInfo.eMode = BAFL_BootMode_eNormal;
      stBootInfo.pstArc = &stFirmwareInfo[0];

      hVce->stOpenSettings.pARCBootCallback(
               hVce->stOpenSettings.pARCBootCallbackData,
               &stBootInfo
               );
   }

   /* Verify Boot */
#if BVCE_P_POLL_FW_MBX
   {
      uint32_t i;

      rc = BERR_UNKNOWN;

      /* poll for response */
      for ( i = 0; i < BVCE_P_POLL_FW_COUNT; i++)
      {
#if 1 /* Do not sleep in emulation */
	 BKNI_Sleep(200);
#else
	 {
	   uint32_t uiTemp = 0xFFFFFFFF;
	   volatile uint32_t uiFoo = uiTemp;
	   while (uiFoo)
	     {
	       uiFoo = uiTemp;
	       uiTemp--;
	     };
	 }
#endif
	 {
	   uint32_t uiHost2Vice = BREG_Read32(
					      hVce->handles.hReg,
					      hVce->stPlatformConfig.stMailbox.uiHost2ViceMailboxAddress
					      );

	   BDBG_MSG(("ARC[0] PC = %08x, STATUS32 = %08x, L2 = %08x, Host2Vice %08x, Vice2Host %08x",
	             BREG_Read32(
	                         hVce->handles.hReg,
	                         hVce->stPlatformConfig.stDebug.uiPicArcPC
	                         ),
	             BREG_Read32(
	                         hVce->handles.hReg,
	                         hVce->stPlatformConfig.stDebug.uiPicArcStatus32
	                         ),
	             BREG_Read32(
	                      hVce->handles.hReg,
	                      hVce->stPlatformConfig.stInterrupt.uiInterruptStatusRegister
	                      ),
	      uiHost2Vice,
	      BREG_Read32(
	         hVce->handles.hReg,
	         hVce->stPlatformConfig.stMailbox.uiVice2HostMailboxAddress
	      )
		     ));

	   /* Wait for zero value in Host2Vice Mailbox Register */

	   if ( 0 == uiHost2Vice )
	   {
	     rc = BERR_SUCCESS;
	     break;
	   }
	 }
      }
   }
#else
   rc = BKNI_WaitForEvent(
            hVce->events.hMailbox,
            BVCE_P_FIRMWARE_BOOT_TIMEOUT
            );
#endif
   {
      /* Retrieve the boot sequence # */
     uint32_t uiHost2Vice = BREG_Read32(
			       hVce->handles.hReg,
			       hVce->stPlatformConfig.stMailbox.uiHost2ViceMailboxAddress
			       );

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("FW Boot Timeout (%08x)", uiHost2Vice));
         return BERR_TRACE(rc);
      }
      else if ( 0 != uiHost2Vice )
      {
         BDBG_ERR(("FW Boot Failed (%08x)", uiHost2Vice));
         return BERR_TRACE( BERR_UNKNOWN );
      }
   }

   /* Retrieve the DCCM offset of the command buffer */
   hVce->fw.dccm.uiCommandBufferBaseOffset = BREG_Read32(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stMailbox.uiVice2HostMailboxAddress
            );

   BDBG_MSG(("ARC[0] Vice2Host = %08x",
	     hVce->fw.dccm.uiCommandBufferBaseOffset
	     ));

   hVce->fw.dccm.uiCommandBufferBaseAddress = hVce->fw.dccm.uiRegisterBaseAddress[0] + hVce->fw.dccm.uiCommandBufferBaseOffset;

   BDBG_LEAVE( BVCE_P_Boot );

   return BERR_TRACE( BERR_SUCCESS );
}

#if BVCE_P_ENABLE_WATCHDOG
BERR_Code
BVCE_P_EnableWatchdog(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_EnableWatchdog );

   /* Setup Watchdog */
   rc = BVCE_Platform_P_WriteRegisterList(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stViceWatchdogEnable.astRegisterSettings,
            hVce->stPlatformConfig.stViceWatchdogEnable.uiRegisterCount
            );

   BDBG_LEAVE( BVCE_P_EnableWatchdog );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_DisableWatchdog(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_DisableWatchdog );

   /* Setup Watchdog */
   rc = BVCE_Platform_P_WriteRegisterList(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stViceWatchdogDisable.astRegisterSettings,
            hVce->stPlatformConfig.stViceWatchdogDisable.uiRegisterCount
            );

   BDBG_LEAVE( BVCE_P_DisableWatchdog );

   return BERR_TRACE( rc );
}
#endif

typedef struct BVCE_P_CommandDebug
{
      uint32_t uiCommand;
      char *szCommandParameterName[HOST_CMD_BUFFER_SIZE/sizeof(uint32_t)];
      char *szResponseParameterName[HOST_CMD_BUFFER_SIZE/sizeof(uint32_t)];
} BVCE_P_CommandDebug;

void BVCE_P_ValidateStructSizes(void)
{

   BDBG_CWARNING( sizeof( Vice2CmdInit_t ) == 8*4 );
   BDBG_CWARNING( sizeof( Vice2CmdInitResponse_t ) == 11*4 );
   BDBG_CWARNING( sizeof( Vice2CmdOpenChannel_t ) == 7*4 );
   BDBG_CWARNING( sizeof( Vice2CmdOpenChannelResponse_t ) == 3*4 );
   BDBG_CWARNING( sizeof( Vice2CmdStartChannel_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdStartChannelResponse_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdStopChannel_t ) == 3*4 );
   BDBG_CWARNING( sizeof( Vice2CmdStopChannelResponse_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdCloseChannel_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdCloseChannelResponse_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdConfigChannel_t ) == 29*4 );
   BDBG_CWARNING( sizeof( Vice2CmdConfigChannelResponse_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdDebugChannel_t ) == (3*4 + COMMAND_BUFFER_SIZE_BYTES));
   BDBG_CWARNING( sizeof( Vice2CmdDebugChannelResponse_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdGetChannelStatus_t ) == 2*4 );
   BDBG_CWARNING( sizeof( Vice2CmdGetChannelStatusResponse_t ) == (2*4 + 12*4 + 1*8) );
   BDBG_CWARNING( sizeof( Vice2CmdGetDeviceStatus_t ) == 1*4 );
   BDBG_CWARNING( sizeof( Vice2CmdGetDeviceStatusResponse_t ) == (2*4 + 2*4) );
}

static const BVCE_P_CommandDebug BVCE_P_CommandLUT[] =
{
   {
    VICE2_CMD_INITIALIZE,
    { "Command (Init)",
      "API version",
      "Stripe Width",
      "Stripe Multiple",
      "Endianess",
      "CABAC Command Buffer Base",
      "CABAC Command Buffer Size",
      "Verification Mode"
    },
    { "Response (Init)",
      "Status",
      "Firmware Version",
      "Debug Buffer Info Base (PicArc)",
      "Debug Buffer Info Base (MBArc)",
      "Channel Error Status Base",
      "FW API Version",
      "Version String",
      "BVN API Version",
      "Watchdog Error Code Base (PicArc)",
      "Watchdog Error Code Base (MBArc)",
    },
   },

   {
    VICE2_CMD_OPEN_CHANNEL,
    {
     "Command (Open)",
     "Channel ID",
     "Non-Secure Buffer Base",
     "Non-Secure Buffer Size",
     "Secure Buffer Base",
     "Secure Buffer Size",
     "Max Number of Channels"
    },
    {
     "Response (Open)",
     "Status",
     "User Data Queue Info Base",
    },
   },

   {
    VICE2_CMD_START_CHANNEL,
    {
     "Command (Start)",
     "Channel ID",
    },
    {
     "Response (Start)",
     "Status",
    },
   },

   {
    VICE2_CMD_STOP_CHANNEL,
    {
     "Command (Stop)",
     "Channel ID",
     "Flags",
    },
    {
     "Response (Stop)",
     "Status",
    },
   },

   {
    VICE2_CMD_CLOSE_CHANNEL,
    {
     "Command (Close)",
     "Channel ID",
    },
    {
     "Response (Close)",
     "Status",
    },
   },

   {
    VICE2_CMD_CONFIG_CHANNEL,
    {
     "Command (Config)",
     "Channel ID",
     "Protocol",
     "Profile",
     "Level",
     "Frame Rate",
     "Bit Rate (Max)",
     "GOP Structure",
     "GOP Length",
     "Mode",
     "Input Type",
     "Event Mask",
     "Flags",
     "STC Index",
     "Context ID",
     "ITB Buffer Offset",
     "ITB Buffer Size",
     "CDB Buffer Offset",
     "CDB Buffer Size",
     "A2P Delay",
     "Min/Max Frame Rate",
     "Rate Buffer Delay",
     "BVN Frame Rate",
     "Max Width/Height",
     "Max GOP Structure",
     "Number of Slices Per Picture",
     "Force Intra Mode Settings",
     "Bit Rate (Target)",
     "Number of Parallel Encodes (FNRT)",
    },
    {
     "Response (Config)",
     "Status",
    },
   },

   {
    VICE2_CMD_DEBUG_CHANNEL,
    {
     "Command (Debug)",
     "Channel ID",
     "Arc ID",
     "Command[0..32]"
    },
    {
     "Response (Debug)",
     "Status",
    },
   },

   {
    VICE2_CMD_GET_CHANNEL_STATUS,
    {
     "Command (Status)",
     "Channel ID",
    },
    {
     "Response (Status)",
     "Status",
     "Status Info: Error Flags",
     "Status Info: Total Errors",
     "Status Info: Pics Encoded",
     "Status Info: Pics Configured",
     "Status Info: Pics Dropped (FRC)",
     "Status Info: Pics Dropped (Performance)",
     "Status Info: Pics Dropped (Not Started)",
     "Status Info: Last Picture ID Encoded",
     "Status Info: FW State",
     "Status Info: Picture Loop Status",
     "Status Info: STC Snapshot (DWORD 1)",
     "Status Info: STC Snapshot (DWORD 0)",
     "Status Info: Pics Dropped (HRD Underflow)",
     "Status Info: ETS/DTS Offset",
    },
   },
};

#define BVCE_P_WriteRegisters( handle, addr, buffer, size ) BVCE_P_WriteRegistersNew( handle, addr, buffer, size, false );
#define BVCE_P_WriteRegisters_isr( handle, addr, buffer, size ) BVCE_P_WriteRegistersNew( handle, addr, buffer, size, true );

BERR_Code
BVCE_P_WriteRegistersNew(
         BVCE_Handle hVce,
	 unsigned uiRegStartAddress,
	 const uint32_t *pBuffer,
	 size_t uiSize,
	 bool bIsr
	 )
{
   size_t i;
   bool bIsCommand = false;
   uint32_t uiCommandIndex = 0;

   BDBG_ENTER(BVCE_P_WriteRegisters);

   bIsCommand = ( uiRegStartAddress == hVce->fw.dccm.uiCommandBufferBaseAddress );

   if ( true == bIsCommand )
   {
      bIsCommand = false;
      for ( uiCommandIndex = 0; uiCommandIndex < sizeof( BVCE_P_CommandLUT ) / sizeof( BVCE_P_CommandDebug ); uiCommandIndex++ )
      {
         if ( BVCE_P_CommandLUT[uiCommandIndex].uiCommand == pBuffer[0] )
         {
            bIsCommand = true;
            break;
         }
      }
   }

   if ( true == bIsr )
   {
      BVCE_Power_P_AcquireResource_isr(
            hVce,
            BVCE_Power_Type_eClock
            );
   }
   else
   {
      BVCE_Power_P_AcquireResource(
            hVce,
            BVCE_Power_Type_eClock
            );
   }

   for ( i = 0; i < ( uiSize / sizeof( uint32_t ) ); i++ )
   {
      if ( true == bIsCommand )
      {
         BDBG_MODULE_MSG( BVCE_CMD, ("@%08x <-- %08x - %s",
                  uiRegStartAddress + (i*sizeof( uint32_t )),
                  pBuffer[i],
                  BVCE_P_CommandLUT[uiCommandIndex].szCommandParameterName[i]
                  ));
      }
      else
      {
         BDBG_MSG(("@%08x <-- %08x",
                  uiRegStartAddress + (i*sizeof( uint32_t )),
                  pBuffer[i]));
      }
     BREG_Write32(
		  hVce->handles.hReg,
		  uiRegStartAddress + (i*sizeof( uint32_t )),
		  pBuffer[i]
		  );
   }

   if ( true == bIsr )
   {
      BVCE_Power_P_ReleaseResource_isr(
            hVce,
            BVCE_Power_Type_eClock
            );
   }
   else
   {
      BVCE_Power_P_ReleaseResource(
            hVce,
            BVCE_Power_Type_eClock
            );
   }

   BDBG_LEAVE(BVCE_P_WriteRegisters);

   return BERR_TRACE( BERR_SUCCESS );
}

#define BVCE_P_ReadRegisters( handle, addr, buffer, size ) BVCE_P_ReadRegistersNew( handle, addr, buffer, size, false )
#define BVCE_P_ReadRegisters_isr( handle, addr, buffer, size ) BVCE_P_ReadRegistersNew( handle, addr, buffer, size, true )

BERR_Code
BVCE_P_ReadRegistersNew(
         BVCE_Handle hVce,
	 unsigned uiRegStartAddress,
	 uint32_t *pBuffer,
	 size_t uiSize, /* In bytes (32-bit multiple) */
	 bool bIsr
	 )
{
   size_t i;
   bool bIsCommand = false;
   uint32_t uiCommandIndex = 0;

   BDBG_ENTER(BVCE_P_ReadRegisters);

   bIsCommand = ( uiRegStartAddress == hVce->fw.dccm.uiCommandBufferBaseAddress );

   if ( true == bIsr )
   {
      BVCE_Power_P_AcquireResource_isr(
            hVce,
            BVCE_Power_Type_eClock
            );
   }
   else
   {
      BVCE_Power_P_AcquireResource(
            hVce,
            BVCE_Power_Type_eClock
            );
   }

   for ( i = 0; i < ( uiSize / sizeof( uint32_t ) ); i++ )
   {
     pBuffer[i] = BREG_Read32(
			      hVce->handles.hReg,
			      uiRegStartAddress + (i*sizeof( uint32_t ))
			      );

     /* Look up command */
     if ( ( true == bIsCommand ) &&
          ( 0 == i ) )
     {
        bIsCommand = false;
        for ( uiCommandIndex = 0; uiCommandIndex < sizeof( BVCE_P_CommandLUT ) / sizeof( BVCE_P_CommandDebug ); uiCommandIndex++ )
        {
           if ( BVCE_P_CommandLUT[uiCommandIndex].uiCommand == pBuffer[0] )
           {
              bIsCommand = true;
              break;
           }
        }
     }

     if ( true == bIsCommand )
     {
        BDBG_MODULE_MSG( BVCE_RSP, ("@%08x --> %08x - %s",
                 uiRegStartAddress + (i*sizeof( uint32_t )),
                 pBuffer[i],
                 BVCE_P_CommandLUT[uiCommandIndex].szResponseParameterName[i]
                 ));
     }
     else
     {
        BDBG_MSG(("@%08x --> %08x",
                  uiRegStartAddress + (i*sizeof( uint32_t )),
                  pBuffer[i]));
     }
   }

   if ( true == bIsr )
   {
      BVCE_Power_P_ReleaseResource_isr(
            hVce,
            BVCE_Power_Type_eClock
            );
   }
   else
   {
      BVCE_Power_P_ReleaseResource(
            hVce,
            BVCE_Power_Type_eClock
            );
   }

   BDBG_LEAVE(BVCE_P_ReadRegisters);

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_SendCommand_impl(
         BVCE_Handle hVce,
         const BVCE_P_Command *pstCommand,
         BVCE_P_Response *pstResponse
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_impl );

   BKNI_ResetEvent(
            hVce->events.hMailbox
            );

   /* Write the command to the command buffer in DCCM */
   BDBG_MSG(("Sending Command"));
   BVCE_P_WriteRegisters(
			 hVce,
			 hVce->fw.dccm.uiCommandBufferBaseAddress,
			 pstCommand->data,
			 sizeof( BVCE_P_Command )
			 );

#if BVCE_P_POLL_FW_MBX
   /* zero out the response mailbox*/
   BREG_Write32(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stMailbox.uiVice2HostMailboxAddress,
            0
            );
#endif

   /* Write the command buffer base offset to the HOST2VICE mailbox */
   BREG_Write32(
            hVce->handles.hReg,
            hVce->stPlatformConfig.stMailbox.uiHost2ViceMailboxAddress,
            hVce->fw.dccm.uiCommandBufferBaseOffset
            );

   /* Wait for event */
#if BVCE_P_POLL_FW_MBX
   {
      uint32_t i;

      rc = BERR_UNKNOWN;

      /* poll for response */
      for ( i = 0; i < BVCE_P_POLL_FW_COUNT; i++)
      {
         uint32_t uiVice2Host;

         BKNI_Sleep(200);

         /* Wait for non-zero value in Vice2Host Mailbox Register */
         uiVice2Host = BREG_Read32(
                           hVce->handles.hReg,
                           hVce->stPlatformConfig.stMailbox.uiVice2HostMailboxAddress
                           );

         BDBG_MSG(("ARC[0] PC = %08x, STATUS32 = %08x, L2 = %08x, Vice2Host %08x",
                    BREG_Read32(
                                hVce->handles.hReg,
                                hVce->stPlatformConfig.stDebug.uiPicArcPC
                                ),
                    BREG_Read32(
                                hVce->handles.hReg,
                                hVce->stPlatformConfig.stDebug.uiPicArcStatus32
                                ),
                    BREG_Read32(
                             hVce->handles.hReg,
                             hVce->stPlatformConfig.stInterrupt.uiInterruptStatusRegister
                             ),
                    uiVice2Host
                    ));

         if ( 0 != uiVice2Host
            )
         {
            rc = BERR_SUCCESS;
            break;
         }
      }
   }
#else
   rc = BKNI_WaitForEvent(
            hVce->events.hMailbox,
            BVCE_P_FIRMWARE_COMMAND_TIMEOUT
            );
#endif

   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Firmware Command Timeout"));
      return BERR_TRACE( rc );
   }

   /* Copy Response */
   {
      uint32_t uiResponseOffset;

      uiResponseOffset = BREG_Read32(
               hVce->handles.hReg,
               hVce->stPlatformConfig.stMailbox.uiVice2HostMailboxAddress
               );

      BDBG_MSG(("Received Response at DCCM Offset: %08x", uiResponseOffset));

      if ( 0 != uiResponseOffset )
      {
         /* Copy the response buffer from DCCM */
	BVCE_P_ReadRegisters(
			     hVce,
			     hVce->fw.dccm.uiRegisterBaseAddress[0] + uiResponseOffset,
			     pstResponse->data,
			     sizeof( BVCE_P_Response )
			     );

         if ( 0 != pstResponse->type.stGeneric.uiStatus )
         {
            BDBG_ERR(("Command %08x Failed with Status (%d)",
                     pstResponse->type.stGeneric.uiCommand,
                     pstResponse->type.stGeneric.uiStatus
                     ));
            return BERR_TRACE( BERR_UNKNOWN );
         }
      }
      else
      {
         BDBG_WRN(("Response Buffer not received"));
         return BERR_TRACE( BERR_UNKNOWN );
      }
   }

   BDBG_LEAVE( BVCE_P_SendCommand_impl );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_SendCommand(
         BVCE_Handle hVce,
         const BVCE_P_Command *pstCommand,
         BVCE_P_Response *pstResponse
         )
{
   BERR_Code rc;

   BDBG_ENTER(BVCE_P_SendCommand);

   BVCE_Power_P_AcquireResource(
         hVce,
         BVCE_Power_Type_eClock
         );

   rc = BVCE_P_SendCommand_impl( hVce,
                            pstCommand,
                            pstResponse
                            );

   BVCE_Power_P_ReleaseResource(
         hVce,
         BVCE_Power_Type_eClock
         );

   BDBG_LEAVE(BVCE_P_SendCommand);

   return BERR_TRACE( rc );
}

typedef enum BVCE_P_DDRDeviceTech
{
   BVCE_P_DDRDeviceTech_e256,
   BVCE_P_DDRDeviceTech_e512,
   BVCE_P_DDRDeviceTech_e1024,
   BVCE_P_DDRDeviceTech_e2048,
   BVCE_P_DDRDeviceTech_e4096,
   BVCE_P_DDRDeviceTech_e8192,

   BVCE_P_DDRDeviceTech_eMax
} BVCE_P_DDRDeviceTech;

typedef enum BVCE_P_DDRWidth
{
   BVCE_P_DDRWidth_e64,
   BVCE_P_DDRWidth_e32,
   BVCE_P_DDRWidth_e16,

   BVCE_P_DDRWidth_eMax
} BVCE_P_DDRWidth;

typedef struct BVCE_P_DDRSettings
{
      uint32_t uiStripeWidth;
      uint32_t uiStripeMultiple;
} BVCE_P_DDRSettings;

static const BVCE_P_DDRSettings s_stDDRSettings[BVCE_P_DDRDeviceTech_eMax][BVCE_P_DDRWidth_eMax] =
{
 /* BVCE_P_DDRDeviceTech_e256 */
 { { E_STRIPE_WIDTH_128,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e64 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e32 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_32 },  /* BVCE_P_DDRWidth_e16 */
 },
 /* BVCE_P_DDRDeviceTech_e512 */
 { { E_STRIPE_WIDTH_128, E_STRIPE_MULTI_128 },  /* BVCE_P_DDRWidth_e64 */
   { E_STRIPE_WIDTH_128,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e32 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e16 */
 },
 /* BVCE_P_DDRDeviceTech_e1024 */
 { { E_STRIPE_WIDTH_128, E_STRIPE_MULTI_128 },  /* BVCE_P_DDRWidth_e64 */
   { E_STRIPE_WIDTH_128,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e32 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e16 */
 },
 /* BVCE_P_DDRDeviceTech_e2048 */
 { { E_STRIPE_WIDTH_128, E_STRIPE_MULTI_128 },  /* BVCE_P_DDRWidth_e64 */
   { E_STRIPE_WIDTH_128,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e32 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e16 */
 },
 /* BVCE_P_DDRDeviceTech_e4096 */
 { { E_STRIPE_WIDTH_128, E_STRIPE_MULTI_128 },  /* BVCE_P_DDRWidth_e64 */
   { E_STRIPE_WIDTH_128,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e32 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e16 */
 },
 /* BVCE_P_DDRDeviceTech_e8192 */
 { { E_STRIPE_WIDTH_128, E_STRIPE_MULTI_128 },  /* BVCE_P_DDRWidth_e64 */
   { E_STRIPE_WIDTH_128,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e32 */
   {  E_STRIPE_WIDTH_64,  E_STRIPE_MULTI_64 },  /* BVCE_P_DDRWidth_e16 */
 },
};

BERR_Code
BVCE_P_SendCommand_Init(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_Init );

   BDBG_CASSERT( HOST2VICE_MBOX_OFFSET == BAVC_VICE_MBOX_OFFSET_HOST2VICE );
   BDBG_CASSERT( VICE2HOST_MBOX_OFFSET == BAVC_VICE_MBOX_OFFSET_VICE2HOST );
   BDBG_CASSERT( BVN2VICE_MBOX_OFFSET == BAVC_VICE_MBOX_OFFSET_BVN2VICE );
   BDBG_CASSERT( BVN2VICE_MBOX_PAYLOAD_OFFSET == BAVC_VICE_MBOX_OFFSET_BVN2VICE_DATA_0_START );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stInit.Command = VICE2_CMD_INITIALIZE;
   hVce->fw.stCommand.type.stInit.API_Version = VICE2_API_VERSION;

   /* Get stripe multiple/width from BCHP */
   {
      uint32_t uiDDRDeviceTech;
      BVCE_P_DDRDeviceTech eDDRDeviceTech;

      uint32_t uiDDRWidth;
      BVCE_P_DDRWidth eDDRWidth;

      rc = BCHP_GetFeature(
               hVce->handles.hChp,
               BCHP_Feature_eMemCtrl1DDRDeviceTechCount,
               &uiDDRDeviceTech
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error getting DDR Device Tech Count"));
         return BERR_TRACE( rc );
      }
      switch ( uiDDRDeviceTech )
      {
         case 256:
            eDDRDeviceTech = BVCE_P_DDRDeviceTech_e256;
            break;
         case 512:
            eDDRDeviceTech = BVCE_P_DDRDeviceTech_e512;
            break;
         case 1024:
            eDDRDeviceTech = BVCE_P_DDRDeviceTech_e1024;
            break;
         case 2048:
            eDDRDeviceTech = BVCE_P_DDRDeviceTech_e2048;
            break;
         case 4096:
            eDDRDeviceTech = BVCE_P_DDRDeviceTech_e4096;
            break;
         case 8192:
            eDDRDeviceTech = BVCE_P_DDRDeviceTech_e8192;
            break;
         default:
            BDBG_ERR(("Unrecognized DDR device tech (%d)", uiDDRDeviceTech));
            return BERR_TRACE( BERR_UNKNOWN );
      }

      rc = BCHP_GetFeature(
               hVce->handles.hChp,
               BCHP_Feature_eMemCtrl1DramWidthCount,
               &uiDDRWidth
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error getting DDR Width"));
         return BERR_TRACE( rc );
      }
      switch ( uiDDRWidth )
      {
         case 64:
            eDDRWidth = BVCE_P_DDRWidth_e64;
            break;
         case 32:
            eDDRWidth = BVCE_P_DDRWidth_e32;
            break;
         case 16:
            eDDRWidth = BVCE_P_DDRWidth_e16;
            break;
         default:
            BDBG_ERR(("Unrecognized DDR width (%d)", uiDDRWidth));
            return BERR_TRACE( BERR_UNKNOWN );
      }

      hVce->fw.stCommand.type.stInit.nStripeWidth = s_stDDRSettings[eDDRDeviceTech][eDDRWidth].uiStripeWidth;
      hVce->fw.stCommand.type.stInit.nStripeMultiple = s_stDDRSettings[eDDRDeviceTech][eDDRWidth].uiStripeMultiple;

#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
      /* Set the CABAC CDB to little endian mode so we can dump the ES using the MIPs */
      hVce->fw.stCommand.type.stInit.DeviceEndianess = 1;
#else
      hVce->fw.stCommand.type.stInit.DeviceEndianess = 0;
#endif

      hVce->fw.stCommand.type.stInit.DeviceSG_CABACCmdBuffPtr = hVce->cabac_cmd_memory.uiBufferOffset;
      hVce->fw.stCommand.type.stInit.DeviceSG_CABACCmdBuffSize = hVce->cabac_cmd_memory.uiBufferSize;
   }

   hVce->fw.stCommand.type.stInit.VerificationModeFlag = 0;
   hVce->fw.stCommand.type.stInit.VerificationModeFlag |= hVce->stOpenSettings.bVerificationMode ? INIT_CMD_VERIFICATION_MODE_MASK : 0;
   hVce->fw.stCommand.type.stInit.VerificationModeFlag |= hVce->stOpenSettings.bA2NPictureDrop  ? INIT_CMD_A2N_MASK : 0;

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   hVce->fw.debug[0].uiBufferInfoBaseAddress = hVce->fw.dccm.uiRegisterBaseAddress[0] + (uint32_t) hVce->fw.stResponse.type.stInit.pPicArcDebugBuff;
   hVce->fw.debug[1].uiBufferInfoBaseAddress = hVce->fw.dccm.uiRegisterBaseAddress[1] + (uint32_t) hVce->fw.stResponse.type.stInit.pMbArcDebugBuff;
   hVce->fw.dccm.uiChannelErrorStatusBaseAddress = hVce->fw.dccm.uiRegisterBaseAddress[0] + (uint32_t) hVce->fw.stResponse.type.stInit.pStatusBase;
   hVce->fw.dccm.uiWatchdogErrorCodeBaseAddress[0] = hVce->fw.dccm.uiRegisterBaseAddress[0] + (uint32_t) hVce->fw.stResponse.type.stInit.pPicArcWdogErrCodeBase;
   hVce->fw.dccm.uiWatchdogErrorCodeBaseAddress[1] = hVce->fw.dccm.uiRegisterBaseAddress[1] + (uint32_t) hVce->fw.stResponse.type.stInit.pMbArcWdogErrCodeBase;

   hVce->stVersionInfo.uiFirmwareVersion = hVce->fw.stResponse.type.stInit.Version;
   hVce->stVersionInfo.uiFirmwareApiVersion = hVce->fw.stResponse.type.stInit.FwApiVersion;
   hVce->stVersionInfo.uiBvn2ViceApiVersion = hVce->fw.stResponse.type.stInit.BvnApiVersion;

   BDBG_WRN(("FW v%d.%d.%d.%d (0x%08x) [API v%d.%d.%d.%d (0x%08x)]",
            ( hVce->fw.stResponse.type.stInit.Version >> 24 ) & 0xFF,
            ( hVce->fw.stResponse.type.stInit.Version >> 16 ) & 0xFF,
            ( hVce->fw.stResponse.type.stInit.Version >>  8 ) & 0xFF,
            ( hVce->fw.stResponse.type.stInit.Version >>  0 ) & 0xFF,
            hVce->fw.stResponse.type.stInit.Version,
            ( hVce->fw.stResponse.type.stInit.FwApiVersion >> 24 ) & 0xFF,
            ( hVce->fw.stResponse.type.stInit.FwApiVersion >> 16 ) & 0xFF,
            ( hVce->fw.stResponse.type.stInit.FwApiVersion >>  8 ) & 0xFF,
            ( hVce->fw.stResponse.type.stInit.FwApiVersion >>  0 ) & 0xFF,
            hVce->fw.stResponse.type.stInit.FwApiVersion
            ));

   if ( 0 != ( hVce->fw.stResponse.type.stInit.Version & 0x80000000 ) )
   {
      /* DEBUG Build */
      if ( 0 != hVce->fw.stResponse.type.stInit.pszVersionStr )
      {
         void* szVersion;

         BMEM_ConvertOffsetToAddress(
                  hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
                  (unsigned) hVce->fw.stResponse.type.stInit.pszVersionStr,
                  &szVersion
                  );

         if ( NULL != szVersion )
         {
            BDBG_WRN(("%s", szVersion));
         }
      }
   }

   if ( hVce->fw.stResponse.type.stInit.FwApiVersion != VICE2_API_VERSION )
   {
      BDBG_ERR(("FW API Version Mismatch: Expected 0x%08x but got 0x%08x", VICE2_API_VERSION, hVce->fw.stResponse.type.stInit.FwApiVersion ));
   }

   if ( BAVC_VICE_BVN2VICE_MAJORREVISION_ID != BAVC_GET_FIELD_DATA(hVce->fw.stResponse.type.stInit.BvnApiVersion, VICE_BVN2VICE, MAJORREVISION ) )
   {
      BDBG_ERR(("BVN API MAJOR Version Mismatch: Expected %d.%d but got %d.%d",
         BAVC_VICE_BVN2VICE_MAJORREVISION_ID,
         BAVC_VICE_BVN2VICE_MINORREVISION_ID,
         BAVC_GET_FIELD_DATA(hVce->fw.stResponse.type.stInit.BvnApiVersion, VICE_BVN2VICE, MAJORREVISION ),
         BAVC_GET_FIELD_DATA(hVce->fw.stResponse.type.stInit.BvnApiVersion, VICE_BVN2VICE, MINORREVISION )
         ));

      return BERR_TRACE( BERR_UNKNOWN );
   }
   else if ( BAVC_VICE_BVN2VICE_MINORREVISION_ID != BAVC_GET_FIELD_DATA(hVce->fw.stResponse.type.stInit.BvnApiVersion, VICE_BVN2VICE, MINORREVISION ) )
   {
      BDBG_WRN(("BVN API MINOR Version Mismatch: Expected %d.%d but got %d.%d",
         BAVC_VICE_BVN2VICE_MAJORREVISION_ID,
         BAVC_VICE_BVN2VICE_MINORREVISION_ID,
         BAVC_GET_FIELD_DATA(hVce->fw.stResponse.type.stInit.BvnApiVersion, VICE_BVN2VICE, MAJORREVISION ),
         BAVC_GET_FIELD_DATA(hVce->fw.stResponse.type.stInit.BvnApiVersion, VICE_BVN2VICE, MINORREVISION )
         ));
   }

   BDBG_LEAVE( BVCE_P_SendCommand_Init );

   return BERR_TRACE( rc );
}

static const uint32_t BVCE_P_MultiChannelModeLUT[BVCE_MultiChannelMode_eMax] =
{
   BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS,
   1,
   BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS
};

BERR_Code
BVCE_P_SendCommand_OpenChannel(
         BVCE_Handle hVce,
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_OpenChannel );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stOpenChannel.Command = VICE2_CMD_OPEN_CHANNEL;
   hVce->fw.stCommand.type.stOpenChannel.uiChannel_id = hVceCh->stOpenSettings.uiInstance;
   hVce->fw.stCommand.type.stOpenChannel.pNonSecureBufferBase = hVceCh->memory[BVCE_P_HeapId_ePicture].uiBufferOffset;
   hVce->fw.stCommand.type.stOpenChannel.uiNonSecureBufferSize = hVceCh->memory[BVCE_P_HeapId_ePicture].uiBufferSize;
   hVce->fw.stCommand.type.stOpenChannel.pSecureBufferBase = hVceCh->memory[BVCE_P_HeapId_eSecure].uiBufferOffset;
   hVce->fw.stCommand.type.stOpenChannel.uiSecureBufferSize = hVceCh->memory[BVCE_P_HeapId_eSecure].uiBufferSize;
   if (  BVCE_MultiChannelMode_eCustom == hVceCh->stOpenSettings.eMultiChannelMode )
   {
      if ( hVceCh->stOpenSettings.uiMaxNumChannels > BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS )
      {
         BDBG_WRN(("The specificed max number of channels (%u) is higher than what is supported by this device(%u), setting max equal to what is supported",
            hVceCh->stOpenSettings.uiMaxNumChannels,
            BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS
            ));

         hVce->fw.stCommand.type.stOpenChannel.uiMaxNumChannels = BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS;
      }
      else
      {
         hVce->fw.stCommand.type.stOpenChannel.uiMaxNumChannels = hVceCh->stOpenSettings.uiMaxNumChannels;
      }
   }
   else
   {
      hVce->fw.stCommand.type.stOpenChannel.uiMaxNumChannels = BVCE_P_MultiChannelModeLUT[hVceCh->stOpenSettings.eMultiChannelMode];
   }

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   /* Handle User Data Queue Info Base */
   hVceCh->userdata.dccm.uiUserDataQueueInfoAddress = hVce->fw.dccm.uiRegisterBaseAddress[0] + (uint32_t) hVce->fw.stResponse.type.stOpenChannel.pUserDataQInfoBase;

   BDBG_LEAVE( BVCE_P_SendCommand_OpenChannel );

   return BERR_TRACE( rc );
}

#define BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED 0xFFFFFFFF

static const uint32_t BVCE_P_ProfileH264LUT[BAVC_VideoCompressionProfile_eMax] =
{
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eUnknown */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSimple */
 ENCODING_AVC_PROFILE_MAIN, /* BAVC_VideoCompressionProfile_eMain */
 ENCODING_AVC_PROFILE_HIGH, /* BAVC_VideoCompressionProfile_eHigh */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eAdvance */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eJizhun */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSnrScalable */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSpatiallyScalable */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eAdvancedSimple */
 ENCODING_AVC_PROFILE_BASELINE, /* BAVC_VideoCompressionProfile_eBaseline */
};

static const uint32_t BVCE_P_ProfileMPEG2LUT[BAVC_VideoCompressionProfile_eMax] =
{
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eUnknown */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSimple */
 ENCODING_MPEG2_PROFILE_MAIN, /* BAVC_VideoCompressionProfile_eMain */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eHigh */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eAdvance */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eJizhun */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSnrScalable */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSpatiallyScalable */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eAdvancedSimple */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eBaseline */
};

static const uint32_t BVCE_P_ProfileMPEG4LUT[BAVC_VideoCompressionProfile_eMax] =
{
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eUnknown */
 ENCODING_MPEG4_PROFILE_SIMPLE, /* BAVC_VideoCompressionProfile_eSimple */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eMain */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eHigh */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eAdvance */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eJizhun */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSnrScalable */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eSpatiallyScalable */
 ENCODING_MPEG4_PROFILE_ADVANCED_SIMPLE, /* BAVC_VideoCompressionProfile_eAdvancedSimple */
 BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED, /* BAVC_VideoCompressionProfile_eBaseline */
};

#define BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED 0xFFFFFFFF

static const uint32_t BVCE_P_LevelH264LUT[BAVC_VideoCompressionLevel_eMax] =
{
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eUnknown */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e00 */
 ENCODING_AVC_LEVEL_10, /* BAVC_VideoCompressionLevel_e10 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e1B */
 ENCODING_AVC_LEVEL_11, /* BAVC_VideoCompressionLevel_e11 */
 ENCODING_AVC_LEVEL_12, /* BAVC_VideoCompressionLevel_e12 */
 ENCODING_AVC_LEVEL_13, /* BAVC_VideoCompressionLevel_e13 */
 ENCODING_AVC_LEVEL_20, /* BAVC_VideoCompressionLevel_e20 */
 ENCODING_AVC_LEVEL_21, /* BAVC_VideoCompressionLevel_e21 */
 ENCODING_AVC_LEVEL_22, /* BAVC_VideoCompressionLevel_e22 */
 ENCODING_AVC_LEVEL_30, /* BAVC_VideoCompressionLevel_e30 */
 ENCODING_AVC_LEVEL_31, /* BAVC_VideoCompressionLevel_e31 */
 ENCODING_AVC_LEVEL_32, /* BAVC_VideoCompressionLevel_e32 */
 ENCODING_AVC_LEVEL_40, /* BAVC_VideoCompressionLevel_e40 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e41 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e42 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e50 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e51 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e60 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e62 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eLow */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eMain */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eHigh */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eHigh1440 */
};

static const uint32_t BVCE_P_LevelMPEG2LUT[BAVC_VideoCompressionLevel_eMax] =
{
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eUnknown */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e00 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e10 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e1B */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e11 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e12 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e13 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e20 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e21 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e22 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e30 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e31 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e32 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e40 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e41 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e42 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e50 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e51 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e60 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e62 */
 ENCODING_MPEG2_LEVEL_LOW, /* BAVC_VideoCompressionLevel_eLow */
 ENCODING_MPEG2_LEVEL_MAIN, /* BAVC_VideoCompressionLevel_eMain */
 ENCODING_MPEG2_LEVEL_HIGH, /* BAVC_VideoCompressionLevel_eHigh */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eHigh1440 */
};

static const uint32_t BVCE_P_LevelMPEG4LUT[BAVC_VideoCompressionLevel_eMax] =
{
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eUnknown */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e00 */
 ENCODING_MPEG4_LEVEL_1, /* BAVC_VideoCompressionLevel_e10 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e1B */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e11 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e12 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e13 */
 ENCODING_MPEG4_LEVEL_2, /* BAVC_VideoCompressionLevel_e20 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e21 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e22 */
 ENCODING_MPEG4_LEVEL_3, /* BAVC_VideoCompressionLevel_e30 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e31 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e32 */
 ENCODING_MPEG4_LEVEL_4, /* BAVC_VideoCompressionLevel_e40 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e41 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e42 */
 ENCODING_MPEG4_LEVEL_5, /* BAVC_VideoCompressionLevel_e50 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e51 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e60 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_e62 */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eLow */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eMain */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eHigh */
 BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED, /* BAVC_VideoCompressionLevel_eHigh1440 */
};

static const uint32_t BVCE_P_InputTypeLUT[BAVC_ScanType_eProgressive + 1] =
{
 ENCODER_INPUT_TYPE_INTERLACED, /* BAVC_ScanType_eInterlaced */
 ENCODER_INPUT_TYPE_PROGRESSIVE, /* BAVC_ScanType_eProgressive */
};

static const uint32_t BVCE_P_FrameTimeLUT[BAVC_FrameRateCode_eMax] =
{
 134,  /* Unknown */
 42,  /* 23.976 */
 42,  /* 24 */
 40,  /* 25 */
 34,  /* 29.97 */
 34,  /* 30 */
 20,  /* 50 */
 17,  /* 59.94 */
 17,  /* 60 */
 67,  /* 14.985 */
 134,  /* 7.493 */
 100,  /* 10 */
 67,  /* 15 */
 50   /* 20 */
};

static const uint32_t BVCE_P_FrameRateLUT[BAVC_FrameRateCode_eMax] =
{
 0000,  /* Unknown */
 ENCODING_FRAME_RATE_2397,  /* 23.976 */
 ENCODING_FRAME_RATE_2400,  /* 24 */
 ENCODING_FRAME_RATE_2500,  /* 25 */
 ENCODING_FRAME_RATE_2997,  /* 29.97 */
 ENCODING_FRAME_RATE_3000,  /* 30 */
 ENCODING_FRAME_RATE_5000,  /* 50 */
 ENCODING_FRAME_RATE_5994,  /* 59.94 */
 ENCODING_FRAME_RATE_6000,  /* 60 */
 ENCODING_FRAME_RATE_1498,  /* 14.985 */
 0000,  /* 7.493 */
 0000,  /* 10 */
 ENCODING_FRAME_RATE_1500,  /* 15 */
 ENCODING_FRAME_RATE_2000   /* 20 */
};

const BAVC_FrameRateCode BVCE_P_FrameRateReverseLUT[BVCE_P_EncoderFrameRate_eMax] =
{
 BAVC_FrameRateCode_e14_985, /* BVCE_P_EncoderFrameRate_e1498 */
 BAVC_FrameRateCode_e15, /* BVCE_P_EncoderFrameRate_e1500 */
 BAVC_FrameRateCode_e20, /* BVCE_P_EncoderFrameRate_e2000 */
 BAVC_FrameRateCode_e23_976, /* BVCE_P_EncoderFrameRate_e2397 */
 BAVC_FrameRateCode_e24, /* BVCE_P_EncoderFrameRate_e2400 */
 BAVC_FrameRateCode_e25, /* BVCE_P_EncoderFrameRate_e2500 */
 BAVC_FrameRateCode_e29_97, /* BVCE_P_EncoderFrameRate_e2997 */
 BAVC_FrameRateCode_e30, /* BVCE_P_EncoderFrameRate_e3000 */
 BAVC_FrameRateCode_e50, /* BVCE_P_EncoderFrameRate_e5000 */
 BAVC_FrameRateCode_e59_94, /* BVCE_P_EncoderFrameRate_e5994 */
 BAVC_FrameRateCode_e60 /* BVCE_P_EncoderFrameRate_e6000 */
};

static const uint32_t BVCE_P_EventMaskLUT[32] =
{
 ( 1 << VICE2_EVENT_BVN_METADATA_CHANGE_BIT ), /* BVCE_CHANNEL_STATUS_FLAGS_EVENT_INPUT_CHANGE */
 ( 1 << VICE2_EVENT_EOS_SENT_BIT ), /* BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS */
};

static const uint32_t BVCE_P_ErrorMaskLUT[32] =
{
 ( 1 << VICE2_ERROR_INVALID_INPUT_DIMENSION_BIT ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_INVALID_INPUT_DIMENSION */
 ( 1 << VICE2_ERROR_USER_DATA_LATE_BIT ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_LATE */
 ( 1 << VICE2_ERROR_USER_DATA_DUPLICATE_BIT ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_DUPLICATE */
 ( 1 << VICE2_ERROR_FW_ADJUSTS_WRONG_FRAME_RATE ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_ADJUSTS_WRONG_FRAME_RATE */
 ( 1 << VICE2_ERROR_UNSUPPORTED_BVN_FRAME_RATE ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_BVN_FRAME_RATE */
 ( 1 << VICE2_ERROR_UNSUPPORTED_RESOLUTION ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_RESOLUTION */
 ( 1 << VICE2_ERROR_BVN_FRAMERATE_IS_SMALLER_THAN_THE_MINIMUM_ALLOWED ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_MIN_FRAME_RATE */
 ( 1 << VICE2_ERROR_MISMATCH_BVN_PIC_RESOLUTION ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_PIC_RESOLUTION */
 ( 1 << VICE2_ERROR_FW_INCREASED_BITRATE_ABOVE_MAX ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_MAX_BITRATE_EXCEEDED */
 ( 1 << VICE2_ERROR_BIN_BUFFER_IS_FULL ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_BIN_BUFFER_FULL */
 ( 1 << VICE2_ERROR_CDB_IS_FULL ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_CDB_FULL */
 ( 1 << VICE2_ERROR_PICARC_TO_CABAC_DINO_BUFFER_IS_FULL ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL */
 ( 1 << VICE2_ERROR_EBM_IS_FULL ), /* BVCE_CHANNEL_STATUS_FLAGS_ERROR_EBM_FULL */
};

static const uint32_t BVCE_P_EventMaskReverseLUT[32] =
{
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0, /* Reserved */
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS, /* VICE2_EVENT_EOS_SENT_BIT */
 BVCE_CHANNEL_STATUS_FLAGS_EVENT_INPUT_CHANGE, /* VICE2_EVENT_BVN_METADATA_CHANGE_BIT */
};

static const uint32_t BVCE_P_ErrorMaskReverseLUT[32] =
{
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_INVALID_INPUT_DIMENSION, /* VICE2_ERROR_INVALID_INPUT_DIMENSION_BIT */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_LATE, /* VICE2_ERROR_USER_DATA_LATE_BIT */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_DUPLICATE, /* VICE2_ERROR_USER_DATA_DUPLICATE_BIT */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_ADJUSTS_WRONG_FRAME_RATE, /* VICE2_ERROR_FW_ADJUSTS_WRONG_FRAME_RATE */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_BVN_FRAME_RATE, /* VICE2_ERROR_UNSUPPORTED_BVN_FRAME_RATE */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_RESOLUTION, /* VICE2_ERROR_UNSUPPORTED_RESOLUTION */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_MIN_FRAME_RATE, /* VICE2_ERROR_BVN_FRAMERATE_IS_SMALLER_THAN_THE_MINIMUM_ALLOWED */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_PIC_RESOLUTION, /* VICE2_ERROR_MISMATCH_BVN_PIC_RESOLUTION */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_MAX_BITRATE_EXCEEDED, /* VICE2_ERROR_FW_INCREASED_BITRATE_ABOVE_MAX */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_BIN_BUFFER_FULL, /* VICE2_ERROR_BIN_BUFFER_IS_FULL */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_CDB_FULL, /* VICE2_ERROR_CDB_IS_FULL */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL, /* VICE2_ERROR_PICARC_TO_CABAC_DINO_BUFFER_IS_FULL */
 BVCE_CHANNEL_STATUS_FLAGS_ERROR_EBM_FULL, /* VICE2_ERROR_EBM_IS_FULL */
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0, /* RESERVED */
};

BERR_Code
BVCE_P_GOPStructureLUT(
   const BVCE_GOPStructure *pstGOPStructure,
   unsigned *puiGOPStructure,
   unsigned *puiGOPLength
   )
{
   *puiGOPStructure = 0;
   *puiGOPLength = 0;

   if ( 0 == pstGOPStructure->uiNumberOfPFrames )
   {
      *puiGOPStructure = ENCODING_GOP_STRUCT_I;
      *puiGOPLength = 1;
   }
   else
   {
      if ( 0xFFFFFFFF == pstGOPStructure->uiNumberOfPFrames )
      {
         *puiGOPStructure = ENCODING_GOP_STRUCT_INFINITE_IP;
      }
      else
      {
         if ( 0 == pstGOPStructure->uiNumberOfBFrames )
         {
            *puiGOPStructure = ENCODING_GOP_STRUCT_IP;
            *puiGOPLength = 1 + pstGOPStructure->uiNumberOfPFrames;
         }
         else
         {
            if ( pstGOPStructure->uiNumberOfBFrames > 3 )
            {
               BDBG_ERR(("Number of consecutive B frames not allowed to exceed 3"));
               return BERR_TRACE( BERR_INVALID_PARAMETER );
            }

            switch ( pstGOPStructure->uiNumberOfBFrames )
            {

               case 1:
                  *puiGOPStructure = ENCODING_GOP_STRUCT_IBP;
                  break;

               case 2:
                  *puiGOPStructure = ENCODING_GOP_STRUCT_IBBP;
                  break;

               case 3:
                  *puiGOPStructure = ENCODING_GOP_STRUCT_IBBBP;
                  break;

               case 0:
               default:
                  BDBG_ERR(("Invalid Parameter"));
                  return BERR_TRACE( BERR_INVALID_PARAMETER );
            }

            *puiGOPLength = 1 + pstGOPStructure->uiNumberOfPFrames + pstGOPStructure->uiNumberOfPFrames*pstGOPStructure->uiNumberOfBFrames;
         }
      }

      *puiGOPStructure &= GOP_STRUCTURE_MASK;

      if ( true == pstGOPStructure->bAllowOpenGOP )
      {
         *puiGOPStructure |= ALLOW_OPEN_GOP_STRUCTURE_MASK;
      }
   }

   return BERR_TRACE( BERR_SUCCESS );
}

/* ensure that the GOP structure selected by encoder settings is supported by the encoder ... */
BERR_Code
BVCE_P_VerifyGopStructure(
         BVCE_Handle hVce
         )
{
   if (~GOP_LENGTH_MASK & hVce->fw.stCommand.type.stConfigChannel.GopLength)
   {
      BDBG_ERR(("GOP length (%d) exceeds maximum supported by encoder: %d", hVce->fw.stCommand.type.stConfigChannel.GopLength, GOP_LENGTH_MASK));
      return BERR_TRACE(BERR_NOT_SUPPORTED);
   }

   switch (hVce->fw.stCommand.type.stConfigChannel.GopStructure & GOP_STRUCTURE_MASK)
   {
      case ENCODING_GOP_STRUCT_I:
      case ENCODING_GOP_STRUCT_IP:
      case ENCODING_GOP_STRUCT_INFINITE_IP:
         /* supported in all encoders */
         break;

      case ENCODING_GOP_STRUCT_IBP:
         /* not supported yet ... */
         BDBG_ERR(("GOP Structure of IBP not supported"));
         return BERR_TRACE(BERR_NOT_SUPPORTED);

      case ENCODING_GOP_STRUCT_IBBP:
         /* supported in AVC and MPEG 2, not supported in all others ... */
         if ((ENCODING_STD_H264 != hVce->fw.stCommand.type.stConfigChannel.Protocol)
            && (ENCODING_STD_MPEG2 != hVce->fw.stCommand.type.stConfigChannel.Protocol))
         {
            BDBG_ERR(("GOP Structure of IBBP not supported"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
         }
         /* else is supported ... */
         /* check GOP length - must be 1 + 3*N */
         if ((hVce->fw.stCommand.type.stConfigChannel.GopLength - 1) % 3)
         {
            BDBG_ERR(("GOP length (%d) invalid for IBBP GOP Structure - must be 1 + 3*N"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
         }
         break;

      case ENCODING_GOP_STRUCT_IBBBP:
         /* not supported yet ... */
         BDBG_ERR(("GOP Structure of IBBBP not supported"));
         return BERR_TRACE(BERR_NOT_SUPPORTED);

      case ENCODING_GOP_STRUCT_TRACK_INPUT:
         /* not supported yet ... */
         BDBG_ERR(("GOP Structure input tracking not supported"));
         return BERR_TRACE(BERR_NOT_SUPPORTED);

      default:
         BDBG_ERR(("Invalid GOP Structure %d", hVce->fw.stCommand.type.stConfigChannel.GopStructure & GOP_STRUCTURE_MASK));
         return BERR_TRACE(BERR_NOT_SUPPORTED);
   }

   if (hVce->fw.stCommand.type.stConfigChannel.GopStructure & ALLOW_OPEN_GOP_STRUCTURE_MASK)
   {
      BDBG_ERR(("Open GOP Structure not supported"));
      return BERR_TRACE(BERR_NOT_SUPPORTED);
   }
   return BERR_SUCCESS;
}

unsigned
BVCE_P_EncodeModeLUT(
   const BVCE_Channel_StartEncodeSettings *pstStartEncodeSettings
   )
{
   if ( true == pstStartEncodeSettings->bNonRealTimeEncodeMode )
   {
      return ENCODER_MODE_AFAP;
   }
   else if ( true == pstStartEncodeSettings->bPipelineLowDelayMode )
   {
      return ENCODER_MODE_LOW_DELAY;
   }
   else
   {
      return ENCODER_MODE_HIGH_DELAY;
   }
}

typedef struct BVCE_P_SendCommand_ConfigChannel_Settings
{
   bool bOnInputChange;
   bool bBeginNewRAP;
   bool bFastChannelChange;
} BVCE_P_SendCommand_ConfigChannel_Settings;

BERR_Code
BVCE_P_SendCommand_ConfigChannel(
         BVCE_Handle hVce,
         BVCE_Channel_Handle hVceCh,
         BVCE_P_SendCommand_ConfigChannel_Settings *pstConfigChannelSettings
         )
{
   BERR_Code rc;
   BVCE_Channel_EncodeSettings *pstEncodeSettings;

   BDBG_ENTER( BVCE_P_SendCommand_ConfigChannel );

   if ( ( NULL != pstConfigChannelSettings ) && ( true == pstConfigChannelSettings->bOnInputChange ) )
   {
      pstEncodeSettings = &hVceCh->stPendingEncodeSettings;
   }
   else
   {
      pstEncodeSettings = &hVceCh->stEncodeSettings;
   }

#if BVCE_P_TEST_MODE
   if ( NULL == hVceCh->hConfigLog )
   {
      char fname[256];
      sprintf(fname, "BVCE_CONFIG_%02d_%02d_%03d.csv", hVceCh->hVce->stOpenSettings.uiInstance, hVceCh->stOpenSettings.uiInstance, hVceCh->uiConfigLogInstance);
      hVceCh->uiConfigLogInstance++;

      if ( false == BVCE_Debug_P_OpenLog( fname, &hVceCh->hConfigLog ) )
      {
         BDBG_ERR(("Error Creating BVCE Channel Config Dump File (%s)", fname));
      }
      else
      {
         BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, "count,nrt,pipeline low delay,adaptive low delay,protocol,profile,level,input type,stc index,output index,rate buffer delay,num parallel encodes" );
         BDBG_CWARNING( sizeof( BVCE_Channel_StartEncodeSettings ) == 72 );

         BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, ",on input change,new rap,fast channel change");
         BDBG_CWARNING( sizeof( BVCE_P_SendCommand_ConfigChannel_Settings ) == 3 );

         BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, ",frame rate,frame rate mode,bitrate,bitrate target,A2P delay,p frames,b frames,open gop,itfp,num slices");
         BDBG_CWARNING( sizeof( BVCE_Channel_EncodeSettings ) == 44 );

         BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, "\n" );
      }
   }

   if ( NULL != hVceCh->hConfigLog )
   {
      /* Start Encode Settings */
      BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
         ( NULL == hVceCh->stStartEncodeSettings.hOutputHandle ) ? (unsigned)-1 : hVceCh->stStartEncodeSettings.hOutputHandle->uiDescriptorCount,
         hVceCh->stStartEncodeSettings.bNonRealTimeEncodeMode,
         hVceCh->stStartEncodeSettings.bPipelineLowDelayMode,
         hVceCh->stStartEncodeSettings.bAdaptiveLowDelayMode,
         hVceCh->stStartEncodeSettings.stProtocolInfo.eProtocol,
         hVceCh->stStartEncodeSettings.stProtocolInfo.eProfile,
         hVceCh->stStartEncodeSettings.stProtocolInfo.eLevel,
         hVceCh->stStartEncodeSettings.eInputType,
         hVceCh->stStartEncodeSettings.uiStcIndex,
         ( NULL == hVceCh->stStartEncodeSettings.hOutputHandle ) ? (unsigned)-1 : hVceCh->stStartEncodeSettings.hOutputHandle->stOpenSettings.uiInstance,
         hVceCh->stStartEncodeSettings.uiRateBufferDelay,
         hVceCh->stStartEncodeSettings.uiNumParallelNRTEncodes
      );

      BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, ",%u,%u,%u",
         pstConfigChannelSettings->bOnInputChange,
         pstConfigChannelSettings->bBeginNewRAP,
         pstConfigChannelSettings->bFastChannelChange
         );

      /* Encode Settings */
      BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, ",%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
         pstEncodeSettings->stFrameRate.eFrameRate,
         pstEncodeSettings->stFrameRate.bVariableFrameRateMode,
         pstEncodeSettings->stBitRate.uiMax,
         pstEncodeSettings->stBitRate.uiTarget,
         pstEncodeSettings->uiA2PDelay,
         pstEncodeSettings->stGOPStructure.uiNumberOfPFrames,
         pstEncodeSettings->stGOPStructure.uiNumberOfBFrames,
         pstEncodeSettings->stGOPStructure.bAllowOpenGOP,
         pstEncodeSettings->bITFPEnable,
         pstEncodeSettings->uiNumSlicesPerPic
      );

      BVCE_Debug_P_WriteLog_isr( hVceCh->hConfigLog, "\n" );
   }
#endif

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stConfigChannel.Command = VICE2_CMD_CONFIG_CHANNEL;
   hVce->fw.stCommand.type.stConfigChannel.uiChannel_id = hVceCh->stOpenSettings.uiInstance;
   if ( hVceCh->stStartEncodeSettings.stProtocolInfo.eProtocol > BVCE_P_VIDEOCOMPRESSIONSTD_MAX )
   {
      BDBG_ERR(("Unrecognized video compression protocol (%d)", hVceCh->stStartEncodeSettings.stProtocolInfo.eProtocol));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }
   hVce->fw.stCommand.type.stConfigChannel.Protocol = BVCE_P_ProtocolLUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eProtocol];
   switch ( hVce->fw.stCommand.type.stConfigChannel.Protocol )
   {
      case ENCODING_STD_H264:
         hVce->fw.stCommand.type.stConfigChannel.Profile = BVCE_P_ProfileH264LUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eProfile];
         hVce->fw.stCommand.type.stConfigChannel.Level = BVCE_P_LevelH264LUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eLevel];
         break;

      case ENCODING_STD_MPEG2:
         hVce->fw.stCommand.type.stConfigChannel.Profile = BVCE_P_ProfileMPEG2LUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eProfile];
         hVce->fw.stCommand.type.stConfigChannel.Level = BVCE_P_LevelMPEG2LUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eLevel];
         break;

      case ENCODING_STD_MPEG4:
         hVce->fw.stCommand.type.stConfigChannel.Profile = BVCE_P_ProfileMPEG4LUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eProfile];
         hVce->fw.stCommand.type.stConfigChannel.Level = BVCE_P_LevelMPEG4LUT[hVceCh->stStartEncodeSettings.stProtocolInfo.eLevel];
         break;

      case BVCE_P_VIDEOCOMPRESSIONSTD_UNSUPPORTED:
      default:
         BDBG_ERR(("Unsupported video compression protocol (%d)", hVceCh->stStartEncodeSettings.stProtocolInfo.eProtocol));
         return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED == hVce->fw.stCommand.type.stConfigChannel.Profile )
   {
      BDBG_ERR(("Unsupported video compression profile (%d)", hVceCh->stStartEncodeSettings.stProtocolInfo.eProfile));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   if ( BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED == hVce->fw.stCommand.type.stConfigChannel.Level )
   {
      BDBG_ERR(("Unsupported video compression level (%d)", hVceCh->stStartEncodeSettings.stProtocolInfo.eLevel));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   hVce->fw.stCommand.type.stConfigChannel.FrameRate = BVCE_P_FrameRateLUT[pstEncodeSettings->stFrameRate.eFrameRate];
   if ( 0 == hVce->fw.stCommand.type.stConfigChannel.FrameRate )
   {
      BDBG_ERR(("Unsupported frame rate (%d)", pstEncodeSettings->stFrameRate.eFrameRate));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   hVce->fw.stCommand.type.stConfigChannel.Flags |= (( true == pstEncodeSettings->stFrameRate.bVariableFrameRateMode ) << CONFIG_FLAG_RATE_MODE_POS); /* Variable Rate */

   hVce->fw.stCommand.type.stConfigChannel.MaxBitrate = pstEncodeSettings->stBitRate.uiMax;

   /* SW7425-4489: Added VBR support */
   hVce->fw.stCommand.type.stConfigChannel.TargetBitrate = pstEncodeSettings->stBitRate.uiTarget;

   if ( hVce->fw.stCommand.type.stConfigChannel.TargetBitrate > hVce->fw.stCommand.type.stConfigChannel.MaxBitrate )
   {
      BDBG_WRN(("The specificed Target bitrate (%u) is higher than Max bitrate (%u), setting Target equal to Max",
         hVce->fw.stCommand.type.stConfigChannel.TargetBitrate,
         hVce->fw.stCommand.type.stConfigChannel.MaxBitrate
         ));

      hVce->fw.stCommand.type.stConfigChannel.TargetBitrate = pstEncodeSettings->stBitRate.uiMax;
   }

   if ( ( NULL != pstConfigChannelSettings ) && ( true == pstConfigChannelSettings->bOnInputChange ) )
   {
      hVce->fw.stCommand.type.stConfigChannel.Flags |= (1 << CONFIG_FLAG_APPLY_BITRATE_BVN_CHANGE_POS);
   }

   /* Calculate GOP Structure */
   {
      BERR_Code rc;
      unsigned uiGOPStructure = 0;
      unsigned uiGOPLength = 0;

      rc = BVCE_P_GOPStructureLUT(
         &pstEncodeSettings->stGOPStructure,
         &uiGOPStructure,
         &uiGOPLength
         );

      if ( BERR_SUCCESS != rc )
      {
         return BERR_TRACE( rc );
      }

      hVce->fw.stCommand.type.stConfigChannel.GopStructure = uiGOPStructure;
      hVce->fw.stCommand.type.stConfigChannel.GopLength = uiGOPLength;

      if ( true == hVceCh->stStartEncodeSettings.bPipelineLowDelayMode )
      {
         switch ( uiGOPStructure )
         {
            case ENCODING_GOP_STRUCT_INFINITE_IP:
               break;

            default:
                  BDBG_ERR(("Infinite IP is the required GOP structure when pipeline low delay mode=%d",
                           hVceCh->stStartEncodeSettings.bPipelineLowDelayMode
                  ));
                  return BERR_TRACE( BERR_INVALID_PARAMETER );
               break;
         }
      }

      /* ensure the selected GOP structure is supported by the encoder ... */
      rc = BVCE_P_VerifyGopStructure(hVce);
      if (BERR_SUCCESS != rc)
         return rc;
   }

   /* Calculate Worst Case GOP Structure */
   {
      BERR_Code rc;
      unsigned uiGOPStructure = 0;
      unsigned uiGOPLength = 0;

      rc = BVCE_P_GOPStructureLUT(
         &hVceCh->stStartEncodeSettings.stBounds.stGOPStructure,
         &uiGOPStructure,
         &uiGOPLength
         );

      if ( BERR_SUCCESS != rc )
      {
         return BERR_TRACE( rc );
      }

      hVce->fw.stCommand.type.stConfigChannel.MaxAllowedGopStructure = uiGOPStructure;
   }

   /* Ensure mode is supported by multi channel config */
   switch ( hVceCh->stOpenSettings.eMultiChannelMode )
   {
      case BVCE_MultiChannelMode_eSingle:
         /* No limitations */
         break;
      case BVCE_MultiChannelMode_eMulti:
         if ( true == hVceCh->stStartEncodeSettings.bPipelineLowDelayMode )
         {
            BDBG_ERR(("BVCE_Channel_StartEncodeSettings.bPipelineLowDelayMode must be FALSE when BVCE_EncodeSettings.eMultiChannelMode=BVCE_MultiChannelMode_eMulti"));
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }
         break;
      case BVCE_MultiChannelMode_eMultiNRTOnly:
         if ( false == hVceCh->stStartEncodeSettings.bNonRealTimeEncodeMode )
         {
            BDBG_ERR(("BVCE_Channel_StartEncodeSettings.bNonRealTimeEncodeMode must be TRUE when BVCE_EncodeSettings.eMultiChannelMode=BVCE_MultiChannelMode_eMultiNRTOnly"));
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }
         break;
      case BVCE_MultiChannelMode_eCustom:
         if ( ( hVceCh->stOpenSettings.uiMaxNumChannels != 1 )
              && ( true == hVceCh->stStartEncodeSettings.bPipelineLowDelayMode ) )
         {
            BDBG_ERR(("BVCE_Channel_StartEncodeSettings.bPipelineLowDelayMode must be FALSE when hVceCh->stOpenSettings.uiMaxNumChannels != 1"));
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }
         break;
      default:
         BDBG_ERR(("Unrecognized eMultiChannelMode=%d", hVceCh->stOpenSettings.eMultiChannelMode));
         return BERR_TRACE( BERR_INVALID_PARAMETER );
         break;
   };

   hVce->fw.stCommand.type.stConfigChannel.Mode = BVCE_P_EncodeModeLUT( &hVceCh->stStartEncodeSettings );

   if ( hVceCh->stStartEncodeSettings.eInputType > BAVC_ScanType_eProgressive )
   {
      BDBG_ERR(("Unrecognized input type (%d)", hVceCh->stStartEncodeSettings.eInputType));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }
   hVce->fw.stCommand.type.stConfigChannel.InputType = BVCE_P_InputTypeLUT[ hVceCh->stStartEncodeSettings.eInputType ];

   /* Set Event Mask */
   {
      uint32_t uiMask;
      uint8_t uiIndex;

      BDBG_CASSERT( VICE2_ERROR_INVALID_INPUT_DIMENSION_BIT     == 0 );
      BDBG_CASSERT( VICE2_ERROR_USER_DATA_LATE_BIT              == 1 );
      BDBG_CASSERT( VICE2_ERROR_USER_DATA_DUPLICATE_BIT         == 2 );
      BDBG_CASSERT( VICE2_ERROR_FW_ADJUSTS_WRONG_FRAME_RATE     == 3 );
      BDBG_CASSERT( VICE2_EVENT_BVN_METADATA_CHANGE_BIT         == 31 );

      hVce->fw.stCommand.type.stConfigChannel.EventMask = 0;

      uiMask = hVceCh->stCallbackSettings.stEvent.uiEventMask;

      uiIndex = 0;

      while ( uiMask )
      {
         if ( uiMask & 0x1 )
         {
            hVce->fw.stCommand.type.stConfigChannel.EventMask |= BVCE_P_EventMaskLUT[uiIndex];
         }

         uiMask >>= 1;
         uiIndex++;
      }

      uiMask = hVceCh->stCallbackSettings.stEvent.uiErrorMask;
      uiIndex = 0;

      while ( uiMask )
      {
         if ( uiMask & 0x1 )
         {
            hVce->fw.stCommand.type.stConfigChannel.EventMask |= BVCE_P_ErrorMaskLUT[uiIndex];
         }

         uiMask >>= 1;
         uiIndex++;
      }
   }

   /* TODO: Update cabac reset when implementing suspend/resume */
   /* Do not reset the cabac context pointers if the channel is already started because we are doing a
    * reconfig (e.g. changing bitrate)
    */
   if ( BVCE_P_Status_eStarted != hVceCh->eStatus )
   {
      hVce->fw.stCommand.type.stConfigChannel.Flags |= (1 << CONFIG_FLAG_RESET_CABAC_CONTEXT_POS);
      BVCE_Output_Reset(
               hVceCh->stStartEncodeSettings.hOutputHandle
               );
   }

   hVce->fw.stCommand.type.stConfigChannel.ContextID = hVceCh->stStartEncodeSettings.hOutputHandle->stOpenSettings.uiInstance;
   hVce->fw.stCommand.type.stConfigChannel.ITBBufPtr = hVceCh->stStartEncodeSettings.hOutputHandle->hOutputBuffers->uiITBBufferOffset;
   hVce->fw.stCommand.type.stConfigChannel.ITBBufSize = hVceCh->stStartEncodeSettings.hOutputHandle->hOutputBuffers->stSettings.stConfig.Itb.Length;
   hVce->fw.stCommand.type.stConfigChannel.CDBBufPtr = hVceCh->stStartEncodeSettings.hOutputHandle->hOutputBuffers->uiCDBBufferOffset;
   hVce->fw.stCommand.type.stConfigChannel.CDBBufSize = hVceCh->stStartEncodeSettings.hOutputHandle->hOutputBuffers->stSettings.stConfig.Cdb.Length;

   /* Set ITFP Mode */
   if ( false == hVceCh->stEncodeSettings.bITFPEnable )
   {
      hVce->fw.stCommand.type.stConfigChannel.Flags |= (1 << CONFIG_FLAG_ITFP_DISABLED_POS);
   }

   /* Set STC Select */
   hVce->fw.stCommand.type.stConfigChannel.StcID = hVceCh->stStartEncodeSettings.uiStcIndex;

   /* Set end-to-end delay */
   if ( 0 == pstEncodeSettings->uiA2PDelay )
   {
      BVCE_A2PDelay stA2PDelay;
      BVCE_GetA2PDelayInfo(&hVceCh->stEncodeSettings, &hVceCh->stStartEncodeSettings, &stA2PDelay);

      pstEncodeSettings->uiA2PDelay = stA2PDelay.uiMin;
   }
   hVce->fw.stCommand.type.stConfigChannel.A2PDelay = pstEncodeSettings->uiA2PDelay;

   hVce->fw.stCommand.type.stConfigChannel.FrameRateLimit = ( ( (uint32_t) BVCE_P_FrameRateLUT[hVceCh->stStartEncodeSettings.stBounds.stFrameRate.eMin]) << CONFIG_FRAME_RATE_LIMIT_MIN_SHIFT ) & CONFIG_FRAME_RATE_LIMIT_MIN_MASK;
   hVce->fw.stCommand.type.stConfigChannel.FrameRateLimit |= ( ( (uint32_t)BVCE_P_FrameRateLUT[hVceCh->stStartEncodeSettings.stBounds.stFrameRate.eMax]) << CONFIG_FRAME_RATE_LIMIT_MAX_SHIFT ) & CONFIG_FRAME_RATE_LIMIT_MAX_MASK;
   hVce->fw.stCommand.type.stConfigChannel.RateBufferDelayInMs = hVceCh->stStartEncodeSettings.uiRateBufferDelay;
   hVce->fw.stCommand.type.stConfigChannel.MinAllowedBvnFrameRate = BVCE_P_FrameRateLUT[hVceCh->stStartEncodeSettings.stBounds.stInputFrameRate.eMin];
   hVce->fw.stCommand.type.stConfigChannel.MaxPictureSizeInPels = ( ( (uint32_t)hVceCh->stStartEncodeSettings.stBounds.stDimensions.stMax.uiWidth) << CONFIG_MAX_PICTURE_SIZE_IN_PELS_WIDTH_SHIFT ) & CONFIG_MAX_PICTURE_SIZE_IN_PELS_WIDTH_MASK;
   hVce->fw.stCommand.type.stConfigChannel.MaxPictureSizeInPels |= ( ( (uint32_t)hVceCh->stStartEncodeSettings.stBounds.stDimensions.stMax.uiHeight) << CONFIG_MAX_PICTURE_SIZE_IN_PELS_HEIGHT_SHIFT ) & CONFIG_MAX_PICTURE_SIZE_IN_PELS_HEIGHT_MASK;;

   /* Set Begin New Rap */
   if ( ( NULL != pstConfigChannelSettings ) && ( true == pstConfigChannelSettings->bBeginNewRAP ) )
   {
      hVce->fw.stCommand.type.stConfigChannel.Flags |= (1 << CONFIG_FLAG_NEW_RAP_POS);
   }

   /* Set Fast Channel Change */
   if ( ( NULL != pstConfigChannelSettings ) && ( true == pstConfigChannelSettings->bFastChannelChange ) )
   {
      hVce->fw.stCommand.type.stConfigChannel.Flags |= (1 << CONFIG_FLAG_FAST_CHANNEL_CHANGE_POS);
   }

   /* Set Number of Slices */
   {
#ifdef BVCE_PLATFORM_P_SUPPORT_MULTIPLE_SLICES
      if ( pstEncodeSettings->uiNumSlicesPerPic > 16 )
      {
         BDBG_ERR(("BVCE_Channel_EncodeSettings.uiNumSlicesPerPic must be <= 16."));
         return BERR_TRACE( BERR_INVALID_PARAMETER );
      }
      else
      {
         hVce->fw.stCommand.type.stConfigChannel.NumSlicesPerPicture = pstEncodeSettings->uiNumSlicesPerPic;
      }
#else
      if ( 0 != pstEncodeSettings->uiNumSlicesPerPic )
      {
         BDBG_ERR(("Multiple Slices are not supported on this platform.  BVCE_Channel_EncodeSettings.uiNumSlicesPerPic must be 0."));
         return BERR_TRACE( BERR_INVALID_PARAMETER );
      }
#endif
   }

   /* Set number of parallel transcodes (FNRT) */
   if ( ( 2 < hVceCh->stStartEncodeSettings.uiNumParallelNRTEncodes ) && ( false == hVceCh->stStartEncodeSettings.bNonRealTimeEncodeMode ) )
   {
      BDBG_WRN(("Number of Parallel Encodes is greater than 2, but channel is not configured in NRT mode"));
   }

   hVce->fw.stCommand.type.stConfigChannel.NumParallelEncodes = hVceCh->stStartEncodeSettings.uiNumParallelNRTEncodes;

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_SendCommand_ConfigChannel );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_SendCommand_StartChannel(
         BVCE_Handle hVce,
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_StartChannel );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stStartChannel.Command = VICE2_CMD_START_CHANNEL;
   hVce->fw.stCommand.type.stStartChannel.uiChannel_id = hVceCh->stOpenSettings.uiInstance;

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_SendCommand_StartChannel );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_SendCommand_StopChannel(
         BVCE_Handle hVce,
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_StopChannel );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stStopChannel.Command = VICE2_CMD_STOP_CHANNEL;
   hVce->fw.stCommand.type.stStopChannel.uiChannel_id = hVceCh->stOpenSettings.uiInstance;

   switch ( hVceCh->stStopEncodeSettings.eStopMode )
   {
      case BVCE_Channel_StopMode_eImmediate:
      case BVCE_Channel_StopMode_eAbort:
         hVce->fw.stCommand.type.stStopChannel.Flags |= (1 << STOP_FLAG_FAST_CHANNEL_STOP_POS);
         break;

      case BVCE_Channel_StopMode_eNormal:
      default:
         /* Do Nothing */
         break;
   }

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_SendCommand_StopChannel );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_SendCommand_GetChannelStatus(
         BVCE_Handle hVce,
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_GetChannelStatus );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stGetChannelStatus.Command = VICE2_CMD_GET_CHANNEL_STATUS;
   hVce->fw.stCommand.type.stGetChannelStatus.uiChannel_id = hVceCh->stOpenSettings.uiInstance;

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_SendCommand_GetChannelStatus );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_P_SendCommand_CloseChannel(
         BVCE_Handle hVce,
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_CloseChannel );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stCloseChannel.Command = VICE2_CMD_CLOSE_CHANNEL;
   hVce->fw.stCommand.type.stCloseChannel.uiChannel_id = hVceCh->stOpenSettings.uiInstance;

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_SendCommand_CloseChannel );

   return BERR_TRACE( rc );
}

static const uint32_t BVCE_P_ArcInstanceLUT[BVCE_ArcInstance_eMax] =
{
   PROC_ID_PICARC, /* BVCE_ArcInstance_ePicArc */
   PROC_ID_MBARC,  /* BVCE_ArcInstance_eMBArc */
};

BERR_Code
BVCE_P_SendCommand_DebugCommand(
         BVCE_Handle hVce,
         BVCE_ArcInstance eARCInstance,
         char *szCommand,
         size_t uiLength
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_P_SendCommand_DebugCommand );

   BKNI_Memset(
            &hVce->fw.stCommand,
            0,
            sizeof( BVCE_P_Command )
            );

   BKNI_Memset(
            &hVce->fw.stResponse,
            0,
            sizeof( BVCE_P_Response )
            );

   hVce->fw.stCommand.type.stDebugChannel.Command = VICE2_CMD_DEBUG_CHANNEL;
   hVce->fw.stCommand.type.stDebugChannel.uiChannel_id = 0;
   hVce->fw.stCommand.type.stDebugChannel.ProcID = BVCE_P_ArcInstanceLUT[eARCInstance];

   BKNI_Memset( hVce->fw.stCommand.type.stDebugChannel.aCommands, 0, COMMAND_BUFFER_SIZE_BYTES );
   BDBG_ASSERT( uiLength <= ( COMMAND_BUFFER_SIZE_BYTES - 1 ) );
   BKNI_Memcpy( hVce->fw.stCommand.type.stDebugChannel.aCommands, szCommand, uiLength );

   rc = BVCE_P_SendCommand(
            hVce,
            &hVce->fw.stCommand,
            &hVce->fw.stResponse
            );

   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BDBG_LEAVE( BVCE_P_SendCommand_DebugCommand );

   return BERR_TRACE( rc );
}

static const uint32_t BVCE_P_DebugBufferModeLUT[BVCE_Debug_BufferingMode_eMax] =
{
 VICE2_DEBUG_BUFFER_MODE_OVERWRITE, /* BVCE_Debug_BufferingMode_eOverwriteOldData */
 VICE_DEBUG_BUFFER_MODE_STANDARD,   /* BVCE_Debug_BufferingMode_eDiscardNewData */
};

BERR_Code
BVCE_P_FreeDebugLog(
         BVCE_Handle hVce
         )
{
   uint32_t i;

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      if ( NULL != hVce->fw.debug[i].hVceTelem )
      {
         BVCE_Telem_Destroy( hVce->fw.debug[i].hVceTelem );
         hVce->fw.debug[i].hVceTelem = NULL;
      }

      if ( NULL != hVce->fw.debug[i].pBufferUnCached )
      {
         BMEM_Free(
                  hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
                  hVce->fw.debug[i].pBufferUnCached
                  );

         hVce->fw.debug[i].pBufferUnCached = NULL;
         hVce->fw.debug[i].pBufferCached = NULL;
      }
   }

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_AllocateDebugLog(
   BVCE_Handle hVce
   )
{
   BERR_Code rc = BERR_SUCCESS;
   uint32_t i;

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      if ( 0 != hVce->stOpenSettings.uiDebugLogBufferSize[i] )
      {
         hVce->fw.debug[i].pBufferUnCached = BMEM_Alloc(
                  hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
                  hVce->stOpenSettings.uiDebugLogBufferSize[i]
                  );

         if ( NULL == hVce->fw.debug[i].pBufferUnCached )
         {
            return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
         }

         rc = BMEM_ConvertAddressToOffset(
                  hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
                  hVce->fw.debug[i].pBufferUnCached,
                  &hVce->fw.debug[i].uiOffset
                  );
         if ( BERR_SUCCESS != rc )
         {
            BVCE_P_FreeDebugLog( hVce );
            return BERR_TRACE( rc );
         }

         /* Get cached address */
         rc = BMEM_ConvertAddressToCached(
            hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
            hVce->fw.debug[i].pBufferUnCached,
            &hVce->fw.debug[i].pBufferCached
         );
         if ( BERR_SUCCESS != rc )
         {
            BVCE_P_FreeDebugLog( hVce );
            return BERR_TRACE( rc );
         }
      }

      {
         BVCE_Telem_Settings stTelemSettings;

         BVCE_Telem_GetDefaultCreateSettings( &stTelemSettings );

         rc = BVCE_Telem_Create( &hVce->fw.debug[i].hVceTelem, &stTelemSettings );
         if ( BERR_SUCCESS != rc )
         {
            BVCE_P_FreeDebugLog( hVce );
            return BERR_TRACE( rc );
         }
      }
   }

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_P_SetupDebugLog(
         BVCE_Handle hVce
         )
{
   uint32_t i;

   for ( i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
   {
      if ( ( 0 != hVce->stOpenSettings.uiDebugLogBufferSize[i] )
           && ( 0 != hVce->fw.debug[i].uiBufferInfoBaseAddress )
         )
      {
         Vice2DebugBufferInfo_t stDebugBufferInfo;

         BKNI_Memset(
               &stDebugBufferInfo,
               0,
               sizeof( Vice2DebugBufferInfo_t )
               );

         BVCE_P_ReadRegisters(
                  hVce,
                  hVce->fw.debug[i].uiBufferInfoBaseAddress,
                  (uint32_t*) (&stDebugBufferInfo),
                  sizeof(  Vice2DebugBufferInfo_t )
                  );

         stDebugBufferInfo.uiMode = BVCE_P_DebugBufferModeLUT[hVce->stOpenSettings.eDebugLogBufferMode[i]];

         stDebugBufferInfo.uiSize = hVce->stOpenSettings.uiDebugLogBufferSize[i];


         stDebugBufferInfo.uiPhysicalOffset = hVce->fw.debug[i].uiOffset;
         stDebugBufferInfo.uiReadOffset = stDebugBufferInfo.uiPhysicalOffset;
         stDebugBufferInfo.uiWriteOffset = stDebugBufferInfo.uiPhysicalOffset;

         BDBG_MSG(("Debug Log[%d] @%08x (%d bytes)",
                  i,
                  hVce->fw.debug[i].pBufferCached,
                  hVce->stOpenSettings.uiDebugLogBufferSize[i]
                 ));

         BVCE_P_WriteRegisters(
                  hVce,
                  hVce->fw.debug[i].uiBufferInfoBaseAddress,
                  (uint32_t*) (&stDebugBufferInfo),
                  sizeof(  Vice2DebugBufferInfo_t )
                  );
      }
   }

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Open(
         BVCE_Handle *phVce, /* [out] VCE Device handle returned */
         BCHP_Handle hChp,   /* [in] Chip handle */
         BREG_Handle hReg,   /* [in] Register handle */
         BMEM_Handle hMem,   /* [in] System Memory handle */
         BINT_Handle hInt,   /* [in] Interrupt handle */
         const BVCE_OpenSettings *pstOpenSettings /* [in] VCE Device Open settings */
         )
{
   BERR_Code rc;
   BVCE_Handle hVce = NULL;

   BDBG_ENTER( BVCE_Open );

   /* Validate Parameters */
   BDBG_ASSERT( phVce );
   BDBG_ASSERT( hChp );
   BDBG_ASSERT( hReg );
   BDBG_ASSERT( hMem );
   BDBG_ASSERT( hInt );
   BDBG_ASSERT( pstOpenSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_OPENSETTINGS == pstOpenSettings->uiSignature );

   /* Validate Instance Number */
   if ( pstOpenSettings->uiInstance >= BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES )
   {
      BDBG_ERR(("Invalid instance number (%d).  Max Instances Supported is %d", pstOpenSettings->uiInstance, BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES));
   }

   /* Allocate Context */
   *phVce = NULL;

   hVce = ( BVCE_Handle ) BKNI_Malloc( sizeof( BVCE_P_Context ) );
   if ( NULL == hVce )
   {
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }
   BKNI_Memset(
            hVce,
            0,
            sizeof( BVCE_P_Context )
            );

   hVce->uiSignature = BVCE_P_SIGNATURE_DEVICEHANDLE;
   hVce->stOpenSettings = *pstOpenSettings;

   /* TODO: Print Settings */

#if BVCE_P_ENABLE_UART
   BVCE_Platform_P_EnableUART(
      hReg
      );
#endif

   /* Setup Device Handles */
   hVce->handles.hChp = hChp;
   hVce->handles.hReg = hReg;
   hVce->handles.hInt = hInt;

   /* Setup Heaps */
   hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem] = hMem;
   {
      uint32_t i;

      for (i = 0; i < BVCE_PLATFORM_P_NUM_ARC_CORES; i++ )
      {
         if ( NULL != hVce->stOpenSettings.hFirmwareMem[i] )
         {
            hVce->fw.hMem[i] = hVce->stOpenSettings.hFirmwareMem[i];
         }
         else
         {
            hVce->fw.hMem[i] = hMem;
         }
      }
   }

   if ( NULL != hVce->stOpenSettings.hGeneralMem )
   {
      hVce->channel_memory.hMem[BVCE_P_HeapId_eGeneral] = hVce->stOpenSettings.hGeneralMem;
   }
   else
   {
      hVce->channel_memory.hMem[BVCE_P_HeapId_eGeneral] = hMem;
   }

   if ( NULL != hVce->stOpenSettings.hPictureMem )
   {
      hVce->channel_memory.hMem[BVCE_P_HeapId_ePicture] = hVce->stOpenSettings.hPictureMem;
   }
   else
   {
      hVce->channel_memory.hMem[BVCE_P_HeapId_ePicture] = hVce->channel_memory.hMem[BVCE_P_HeapId_eGeneral];
   }

   if ( NULL != hVce->stOpenSettings.hSecureMem )
   {
      hVce->channel_memory.hMem[BVCE_P_HeapId_eSecure] = hVce->stOpenSettings.hSecureMem;
   }
   else
   {
      hVce->channel_memory.hMem[BVCE_P_HeapId_eSecure] = hVce->channel_memory.hMem[BVCE_P_HeapId_eGeneral];
   }

   /* Create timer */
   rc = BVCE_P_CreateTimer(
            hVce
            );
   if (BERR_SUCCESS != rc)
   {
      BDBG_WRN(("Error creating timer"));
   }

   /* Check for Image Interface/Context */
#if !(BVCE_USE_CUSTOM_IMAGE)
   if (hVce->stOpenSettings.pImgInterface != &BVCE_IMAGE_Interface)
   {
      BDBG_WRN(("*******************"));
      BDBG_WRN(("You've linked in the default VCE BIMG interface and context."));
      BDBG_WRN(("However, you are providing your own version(s) to BVCE_Open()."));
      BDBG_WRN(("You should compile with BVCE_USE_CUSTOM_IMAGE=1 to prevent linkage"));
      BDBG_WRN(("of the default BIMG interface and context to reduce the binary size"));
      BDBG_WRN(("*******************"));
   }

#if !(BVCE_USE_CUSTOM_CONTEXT)
   if (hVce->stOpenSettings.pImgContext != BVCE_IMAGE_Context)
   {
      BDBG_WRN(("*******************"));
      BDBG_WRN(("You've linked in the default VCE BIMG context."));
      BDBG_WRN(("However, you are providing your own version to BVCE_Open()."));
      BDBG_WRN(("You should compile with BVCE_USE_CUSTOM_CONTEXT=1 to prevent linkage"));
      BDBG_WRN(("of the default BIMG context to reduce the binary size"));
      BDBG_WRN(("*******************"));
   }
#endif
#endif

   if ((hVce->stOpenSettings.pImgInterface == NULL) ||
       (hVce->stOpenSettings.pImgContext == NULL)) {
      BDBG_ERR(("*******************"));
      BDBG_ERR(("You've compiled with either BVCE_USE_CUSTOM_IMAGE=1 or BVCE_USE_CUSTOM_CONTEXT=1."));
      BDBG_ERR(("However, you have NOT provided your own version(s) of"));
      BDBG_ERR(("the BIMG interface and context to BVCE_Open()."));
      BDBG_ERR(("If you want to use the default BIMG, use BVCE_USE_CUSTOM_IMAGE=0 or BVCE_USE_CUSTOM_CONTEXT=0"));
      BDBG_ERR(("Otherwise, you MUST provide your own implementation of BIMG."));
      BDBG_ERR(("*******************"));
   }

   rc = BVCE_Platform_P_GetConfig(
            pstOpenSettings->uiInstance,
            &hVce->stPlatformConfig
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_AllocateDeviceMemory(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_AllocateChannelMemory(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_AllocateFirmwareMemory(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_AllocateDebugLog(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_CreateEvents(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   rc = BVCE_Power_Resume( hVce );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Close( hVce );
      return BERR_TRACE( rc );
   }

   /* Assign handle */
   *phVce = hVce;

   BDBG_LEAVE( BVCE_Open );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Close(
         BVCE_Handle hVce
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_Close );

   BDBG_ASSERT( hVce );

   BVCE_Power_P_AcquireResource(
         hVce,
         BVCE_Power_Type_ePower
         );

   rc = BERR_TRACE( BVCE_Power_Standby(
            hVce
            ));

   rc = BERR_TRACE( BVCE_P_DestroyEvents(
               hVce
               ));

   rc = BERR_TRACE( BVCE_P_FreeDebugLog(
               hVce
               ));

   rc = BERR_TRACE( BVCE_P_FreeFirmwareMemory(
               hVce
               ));

   rc = BERR_TRACE( BVCE_P_FreeChannelMemory(
               hVce
               ));

   rc = BERR_TRACE( BVCE_P_FreeDeviceMemory(
               hVce
               ));

   rc = BERR_TRACE( BVCE_P_DestroyTimer(
               hVce
               ));

#if BVCE_P_DUMP_ARC_DEBUG_LOG
   {
      BVCE_ArcInstance eArcInstance;

      for ( eArcInstance = 0; eArcInstance < BVCE_ArcInstance_eMax; eArcInstance++ )
      {
         BVCE_Debug_P_CloseLog( hVce->hDebugLogDumpFile[eArcInstance] );
         hVce->hDebugLogDumpFile[eArcInstance] = NULL;
      }
   }
#endif

   BKNI_Free(
            hVce
            );

   BDBG_LEAVE( BVCE_Close );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_GetTotalChannels(
         BVCE_Handle hVce,
         unsigned *puiTotalChannels
         )
{
   BDBG_ENTER( BVCE_GetTotalChannels );

   BDBG_ASSERT( puiTotalChannels );
   BSTD_UNUSED( hVce );

   if ( NULL == puiTotalChannels )
   {
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   *puiTotalChannels = BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS;

   BDBG_LEAVE( BVCE_GetTotalChannels );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_GetVersionInfo(
   BVCE_Handle hVce,
   BVCE_VersionInfo *pstVersionInfo
   )
{
   BDBG_ENTER( BVCE_GetVersionInfo );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( pstVersionInfo );

   if ( NULL == pstVersionInfo )
   {
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   *pstVersionInfo = hVce->stVersionInfo;

   BDBG_LEAVE( BVCE_GetVersionInfo );

   return BERR_TRACE( BERR_SUCCESS );
}

/*********/
/* Debug */
/*********/
BERR_Code
BVCE_Debug_ReadBuffer(
         BVCE_Handle hVce,
         BVCE_ArcInstance eARCInstance,
         char *szBuffer,   /* [in] pointer to buffer where log is copied to */
         size_t uiBufferSize,  /* [in] maximum number of bytes to copy to buffer */
         size_t *puiBytesRead  /* [out] number of bytes copied from debug log */
         )
{
   Vice2DebugBufferInfo_t stDebugBufferInfo;
   size_t uiInputLengthRead = 0;

   BDBG_ENTER( BVCE_Debug_ReadBuffer );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( szBuffer );
   BDBG_ASSERT( puiBytesRead );

   *puiBytesRead = 0;

   if ( ( 0 == hVce->fw.debug[eARCInstance].uiBufferInfoBaseAddress )
        || ( NULL == hVce->fw.debug[eARCInstance].pBufferCached )
      )
   {
      BDBG_ERR(("Debug Log Doesn't Exist for ARC Instance %d", eARCInstance));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BVCE_P_ReadRegisters(
            hVce,
            hVce->fw.debug[eARCInstance].uiBufferInfoBaseAddress,
            (uint32_t*) (&stDebugBufferInfo),
            sizeof( Vice2DebugBufferInfo_t )
            );

   if ( hVce->stOpenSettings.eDebugLogBufferMode[eARCInstance] == BVCE_Debug_BufferingMode_eOverwriteOldData )
   {
      /* TODO: We need to tell the FW to stop writing to the buffer while we are reading it out */
      BDBG_ERR(("BVCE_Debug_BufferingMode_eOverwriteOldData not supported"));
      return BERR_TRACE( BERR_NOT_SUPPORTED );
   }

   /* Parse the data */
   {
      const uint8_t *pInputBuffer0;
      size_t uiInputLength0;
      const uint8_t *pInputBuffer1 = NULL;
      size_t uiInputLength1 = 0;

      pInputBuffer0 = ((uint8_t*)hVce->fw.debug[eARCInstance].pBufferCached) + (stDebugBufferInfo.uiReadOffset - stDebugBufferInfo.uiPhysicalOffset);

      if ( stDebugBufferInfo.uiReadOffset <= stDebugBufferInfo.uiWriteOffset )
      {
         uiInputLength0 = stDebugBufferInfo.uiWriteOffset - stDebugBufferInfo.uiReadOffset;
      }
      else
      {
         uiInputLength0 = (stDebugBufferInfo.uiPhysicalOffset + stDebugBufferInfo.uiSize) - stDebugBufferInfo.uiReadOffset;

         pInputBuffer1 = (uint8_t*)hVce->fw.debug[eARCInstance].pBufferCached;
         uiInputLength1 = (stDebugBufferInfo.uiWriteOffset - stDebugBufferInfo.uiPhysicalOffset);
      }

      BMEM_FlushCache(
         hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
         (void*) pInputBuffer0,
         uiInputLength0
         );

      if ( NULL != pInputBuffer1 )
      {
         BMEM_FlushCache(
            hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
            (void*) pInputBuffer1,
            uiInputLength1
            );
      }

      BVCE_Telem_Parse(
         hVce->fw.debug[eARCInstance].hVceTelem,
         pInputBuffer0,
         uiInputLength0,
         pInputBuffer1,
         uiInputLength1,
         &uiInputLengthRead,
         szBuffer,
         uiBufferSize,
         puiBytesRead
         );

#if BVCE_P_DUMP_ARC_DEBUG_LOG
   if ( NULL == hVce->hDebugLogDumpFile[eARCInstance] )
   {
      char fname[256];
      sprintf(fname, "BVCE_ARC_DEBUG_LOG_%02d_%02d.log", hVce->stOpenSettings.uiInstance, eARCInstance);

      if ( false == BVCE_Debug_P_OpenLog( fname, &hVce->hDebugLogDumpFile[eARCInstance] ) )
      {
         BDBG_ERR(("Error Creating BVCE ARC Debug Log Dump File (%s)", fname));
      }
   }

   if ( NULL != hVce->hDebugLogDumpFile[eARCInstance] )
   {
       size_t uiBytesLeftToWrite = uiInputLengthRead;
       size_t uiBytesToWrite = 0;

       if ( uiBytesLeftToWrite > uiInputLength0 )
       {
          uiBytesToWrite = uiInputLength0;
       }
       else
       {
          uiBytesToWrite = uiBytesLeftToWrite;
       }

       BVCE_Debug_P_WriteLogBuffer_isr( hVce->hDebugLogDumpFile[eARCInstance], pInputBuffer0, uiBytesToWrite );

      uiBytesLeftToWrite -= uiBytesToWrite;

      if ( 0 != uiBytesLeftToWrite )
      {
         BVCE_Debug_P_WriteLogBuffer_isr( hVce->hDebugLogDumpFile[eARCInstance], pInputBuffer1, uiBytesLeftToWrite );
      }
   }
#endif
   }

   if ( 0 != uiInputLengthRead )
   {
      stDebugBufferInfo.uiReadOffset += uiInputLengthRead;
      if ( stDebugBufferInfo.uiReadOffset >= ( stDebugBufferInfo.uiPhysicalOffset + stDebugBufferInfo.uiSize ) )
      {
         stDebugBufferInfo.uiReadOffset -= stDebugBufferInfo.uiSize;
      }

      /* coverity[address_of] */
      /* coverity[callee_ptr_arith] */
      BVCE_P_WriteRegisters(
               hVce,
               hVce->fw.debug[eARCInstance].uiBufferInfoBaseAddress + ( ((uint32_t)(&stDebugBufferInfo.uiReadOffset)) - ((uint32_t)&stDebugBufferInfo)),
               (uint32_t*) (&stDebugBufferInfo.uiReadOffset),
               sizeof( stDebugBufferInfo.uiReadOffset )
               );
   }

   BDBG_LEAVE( BVCE_Debug_ReadBuffer );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Debug_SendCommand(
         BVCE_Handle hVce,
         BVCE_ArcInstance eARCInstance,
         char        *szCommand  /* [in] pointer to a double null terminated command string of debug uart commands */
         )
{
   BERR_Code rc = BERR_SUCCESS;
   size_t uiCommandStartIndex;
   size_t uiCommandLength = 0;
   size_t uiCommandCurrentIndex = 0;

   BDBG_ENTER( BVCE_Debug_SendCommand );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( szCommand );

   /* Parse szCommand and send commands one at a time to FW */
   do
   {
      uiCommandStartIndex = uiCommandCurrentIndex;
      uiCommandLength = 0;

      while ( '\0' != szCommand[uiCommandCurrentIndex] )
      {
         uiCommandCurrentIndex++;
         uiCommandLength++;
      }
      /* Include NULL terminator */
      uiCommandCurrentIndex++;
      uiCommandLength++;

      if ( uiCommandLength > 1 )
      {
         if ( uiCommandLength < ( COMMAND_BUFFER_SIZE_BYTES - 1) )
         {
            BDBG_MSG(("Sending Debug Command: %s", &szCommand[uiCommandStartIndex] ));
            rc = BVCE_P_SendCommand_DebugCommand(
                     hVce,
                     eARCInstance,
                     &szCommand[uiCommandStartIndex],
                     uiCommandLength
                     );

            if ( rc != BERR_SUCCESS )
            {
               BDBG_ERR(("Error Sending Debug Command: %s (All subsequent commands have been aborted)", &szCommand[uiCommandStartIndex] ));
            }
         }
         else
         {
            BDBG_ERR(("Command \"%s\" is longer than support command length", &szCommand[uiCommandStartIndex]));
            rc = BERR_TRACE( BERR_INVALID_PARAMETER );
         }
      }
   } while ( ( BERR_SUCCESS == rc ) && ( '\0' != szCommand[uiCommandCurrentIndex] ) );

   BDBG_LEAVE( BVCE_Debug_SendCommand );
   return BERR_TRACE( rc );
}


/************/
/* Watchdog */
/************/
BERR_Code
BVCE_ProcessWatchdog(
         BVCE_Handle hVce
         )
{
   BDBG_ENTER( BVCE_ProcessWatchdog );

   BDBG_ASSERT( hVce );

   /* TODO: */
   BSTD_UNUSED( hVce );

   BDBG_LEAVE( BVCE_ProcessWatchdog );
   return BERR_TRACE( BERR_NOT_SUPPORTED );
}

/*******************/
/* Device Callback */
/*******************/
void
BVCE_GetDefaultCallbackSettings(
         BVCE_CallbackSettings* pstCallbackSettings
         )
{
   BDBG_ENTER( BVCE_GetDefaultCallbackSettings );

   BDBG_ASSERT( pstCallbackSettings );

   BKNI_Memset(
            pstCallbackSettings,
            0,
            sizeof( BVCE_CallbackSettings )
            );

   pstCallbackSettings->uiSignature = BVCE_P_SIGNATURE_DEVICECALLBACKSETTINGS;

   BDBG_LEAVE( BVCE_GetDefaultCallbackSettings );
   return;
}

BERR_Code
BVCE_SetCallbackSettings(
         BVCE_Handle hVce,
         const BVCE_CallbackSettings* pstCallbackSettings
         )
{
   BDBG_ENTER( BVCE_SetCallbackSettings );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( pstCallbackSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_DEVICECALLBACKSETTINGS == pstCallbackSettings->uiSignature );

   BKNI_EnterCriticalSection();

   hVce->callbacks.stCallbackSettings = *pstCallbackSettings;

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE( BVCE_SetCallbackSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_GetCallbackSettings(
         BVCE_Handle hVce,
         BVCE_CallbackSettings* pstCallbackSettings
         )
{
   BDBG_ENTER( BVCE_GetCallbackSettings );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( pstCallbackSettings );

   *pstCallbackSettings = hVce->callbacks.stCallbackSettings;

   BDBG_LEAVE( BVCE_GetCallbackSettings );
   return BERR_TRACE( BERR_SUCCESS );
}


#if 0
/********************/
/* Helper Functions */
/********************/

void
BVCE_GetDefaultEncoderOutputSettings(
         BVCE_EncoderOutputSettings *pstEncoderOutputSettings
         )
{
   BDBG_ENTER( BVCE_GetDefaultEncoderOutputSettings );

   BDBG_ASSERT( pstEncoderOutputSettings );

   /* TODO: */

   BDBG_LEAVE( BVCE_GetDefaultEncoderOutputSettings );
   return;
}

BERR_Code
BVCE_GetMinimumRequiredMemoryConfig(
         const BVCE_EncoderOutputSettings *pstEncoderOutputSettings,
         BVCE_MemoryConfig *pstMemoryConfig,
         BAVC_CdbItbConfig *pstCdbItbConfig
         )
{
   BDBG_ENTER( BVCE_GetMinimumRequiredMemoryConfig );

   BDBG_ASSERT( pstEncoderOutputSettings );
   BDBG_ASSERT( pstMemoryConfig );
   BDBG_ASSERT( pstCdbItbConfig );

   /* TODO: */

   BDBG_LEAVE( BVCE_GetMinimumRequiredMemoryConfig );
   return BERR_TRACE( BERR_NOT_SUPPORTED );
}
#endif

/**********************/
/* Channel Open/Close */
/**********************/
static const BVCE_Channel_OpenSettings s_stDefaultChannelOpenSettings =
{
 BVCE_P_SIGNATURE_CHANNELOPENSETTINGS, /* Signature */
 0, /* Channel Instance */
 NULL, /* System Heap */
 NULL, /* General Heap */
 NULL, /* Secure Heap */
 NULL, /* Picture Heap */
 /* Memory Config */
 {
   MIN_GENERAL_BUFFER_SIZE_IN_BYTES,
   MIN_PICTURE_BUFFER_SIZE_IN_BYTES,
   MIN_SECURE_BUFFER_SIZE_IN_BYTES,
 },
 BVCE_MultiChannelMode_eMulti,
 0
};

void
BVCE_Channel_GetDefaultOpenSettings(
         BVCE_Channel_OpenSettings *pstChSettings
         )
{
   BDBG_ENTER( BVCE_Channel_GetDefaultOpenSettings );

   BDBG_ASSERT( pstChSettings );

   if ( NULL != pstChSettings )
   {
      *pstChSettings = s_stDefaultChannelOpenSettings;
   }

   BDBG_LEAVE( BVCE_Channel_GetDefaultOpenSettings );

   return;
}

#define BVCE_UserData_P_PacketDescriptor_MAX_PER_FIELD BVCE_FW_P_UserData_PacketType_eMax

BERR_Code
BVCE_Channel_Open(
         BVCE_Handle hVce,
         BVCE_Channel_Handle *phVceCh,
         const BVCE_Channel_OpenSettings *pstChOpenSettings /* [in] VCE Channel settings */
         )
{
   BERR_Code rc;
   BVCE_Channel_Handle hVceCh;
   BVCE_P_HeapId eHeapId;

   BDBG_ENTER( BVCE_Channel_Open );

   BDBG_ASSERT( hVce );
   BDBG_ASSERT( phVceCh );
   BDBG_ASSERT( pstChOpenSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_CHANNELOPENSETTINGS == pstChOpenSettings->uiSignature );

   if ( pstChOpenSettings->uiInstance >= BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS )
   {
      BDBG_ERR(("Invalid channel instance (%d) specified", pstChOpenSettings->uiInstance));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   /* Validate the MultiChannelMode settings */
   {
      unsigned uiInstance;

      switch ( pstChOpenSettings->eMultiChannelMode )
      {
         case BVCE_MultiChannelMode_eSingle:
            /* Ensure no other channels are open */
            for (uiInstance = 0; uiInstance < BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS; uiInstance++ )
            {
               if ( BVCE_P_Status_eIdle != hVce->channels[uiInstance].context.eStatus )
               {
                  BDBG_ERR(("Cannot open channel in MultiChannelMode_eSingle when other channels are open.  Channel[%d] is open.",
                     uiInstance
                     ));
                  return BERR_TRACE( BERR_INVALID_PARAMETER );
               }
            }
            break;

         default:
            /* Ensure other open channels have same MultiChannelMode */
            for (uiInstance = 0; uiInstance < BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS; uiInstance++ )
            {
               if ( BVCE_P_Status_eIdle != hVce->channels[uiInstance].context.eStatus )
               {
                  if ( pstChOpenSettings->eMultiChannelMode != hVce->channels[uiInstance].context.stOpenSettings.eMultiChannelMode )
                  {
                     BDBG_ERR(("All channels must be opened with the same MultiChannelMode. Channel[%d] is open with MultiChannelMode=%d.",
                        uiInstance,
                        hVce->channels[uiInstance].context.stOpenSettings.eMultiChannelMode
                        ));
                     return BERR_TRACE( BERR_INVALID_PARAMETER );
                  }
               }
            }
            break;
      }
   }

   hVceCh = &hVce->channels[pstChOpenSettings->uiInstance].context;

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         /* We're good to go */
         break;

      case BVCE_P_Status_eOpened:
         BDBG_ERR(("Channel is already open"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eStarted:
         BDBG_ERR(("Channel is already started"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         BDBG_ERR(("Channel is already open and has not completed stopping"));
         return BERR_TRACE( BERR_UNKNOWN );

      default:
         BDBG_ERR(("Unknown state"));
         return BERR_TRACE( BERR_UNKNOWN );
   }

   BKNI_Memset(
            hVceCh,
            0,
            sizeof( BVCE_P_Channel_Context )
            );

   hVceCh->uiSignature = BVCE_P_SIGNATURE_CHANNELHANDLE;
   hVceCh->hVce = hVce;
   hVceCh->eStatus = BVCE_P_Status_eOpened;
   hVceCh->stOpenSettings = *pstChOpenSettings;
   BVCE_Channel_GetDefaultEncodeSettings(
            &hVceCh->stEncodeSettings
            );

   /* Select heaps */
   if ( NULL != hVceCh->stOpenSettings.hChannelSystemMem )
   {
      hVceCh->memory[BVCE_P_HeapId_eSystem].hMem = hVceCh->stOpenSettings.hChannelSystemMem;
   }
   else
   {
      hVceCh->memory[BVCE_P_HeapId_eSystem].hMem = hVceCh->hVce->channel_memory.stHeapInfo[BVCE_P_HeapId_eSystem].hMem;
   }

   if ( NULL != hVceCh->stOpenSettings.hChannelGeneralMem )
   {
      hVceCh->memory[BVCE_P_HeapId_eGeneral].hMem = hVceCh->stOpenSettings.hChannelGeneralMem;
   }
   else
   {
      hVceCh->memory[BVCE_P_HeapId_eGeneral].hMem = hVceCh->hVce->channel_memory.stHeapInfo[BVCE_P_HeapId_eGeneral].hMem;
   }

   if ( NULL != hVceCh->stOpenSettings.hChannelPictureMem )
   {
      hVceCh->memory[BVCE_P_HeapId_ePicture].hMem = hVceCh->stOpenSettings.hChannelPictureMem;
   }
   else
   {
      hVceCh->memory[BVCE_P_HeapId_ePicture].hMem = hVceCh->hVce->channel_memory.stHeapInfo[BVCE_P_HeapId_ePicture].hMem;
   }

   if ( NULL != hVceCh->stOpenSettings.hChannelSecureMem )
   {
      hVceCh->memory[BVCE_P_HeapId_eSecure].hMem = hVceCh->stOpenSettings.hChannelSecureMem;
   }
   else
   {
      hVceCh->memory[BVCE_P_HeapId_eSecure].hMem = hVceCh->hVce->channel_memory.stHeapInfo[BVCE_P_HeapId_eSecure].hMem;
   }

   /* Allocate Memory */

   /* FW doesn't support different general/picture memory buffers so combine general/picture memory */
   hVceCh->memory[BVCE_P_HeapId_ePicture].uiBufferSize = hVceCh->stOpenSettings.stMemoryConfig.uiGeneralMemSize + hVceCh->stOpenSettings.stMemoryConfig.uiPictureMemSize;
   hVceCh->memory[BVCE_P_HeapId_eSecure].uiBufferSize = hVceCh->stOpenSettings.stMemoryConfig.uiSecureMemSize;

   for ( eHeapId = 0; eHeapId < BVCE_P_HeapId_eMax; eHeapId++ )
   {
      /* Allocate Memory */
      if ( 0 != hVceCh->memory[eHeapId].uiBufferSize )
      {
         hVceCh->memory[eHeapId].pBufferUnCached = BMEM_AllocAligned(
                  hVceCh->memory[eHeapId].hMem,
                  hVceCh->memory[eHeapId].uiBufferSize,
                  9,
                  0
                  );

         if ( NULL == hVceCh->memory[eHeapId].pBufferUnCached )
         {
            BDBG_ERR(("Error allocating memory"));
            return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
         }

         /* Get offset */
         rc = BMEM_ConvertAddressToOffset(
                  hVceCh->memory[eHeapId].hMem,
                  hVceCh->memory[eHeapId].pBufferUnCached,
                  &hVceCh->memory[eHeapId].uiBufferOffset
                  );
         if ( BERR_SUCCESS != rc )
         {
            BDBG_ERR(("Error converting virtual address to offset"));
            return BERR_TRACE( rc );
         }
      }
   }
   /* Allocate user data queue */
   if ( 0 != BVCE_FW_P_USERDATA_QUEUE_LENGTH )
   {
      hVceCh->userdata.uiDescriptorBufferSize = BVCE_FW_P_USERDATA_QUEUE_LENGTH * BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH * BVCE_UserData_P_PacketDescriptor_MAX_PER_FIELD;

      hVceCh->userdata.pDescriptorBufferUnCached = BMEM_Alloc(
               hVceCh->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
               hVceCh->userdata.uiDescriptorBufferSize
               );

      if ( NULL == hVceCh->userdata.pDescriptorBufferUnCached )
      {
         BDBG_ERR(("Error allocating user data queue"));
         BVCE_Channel_Close( hVceCh );
         return BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
      }

      /* Get offset */
      rc = BMEM_ConvertAddressToOffset(
               hVceCh->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
               hVceCh->userdata.pDescriptorBufferUnCached,
               &hVceCh->userdata.uiDescriptorBufferOffset
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting virtual address to offset"));
         BVCE_Channel_Close( hVceCh );
         return BERR_TRACE( rc );
      }

      /* Get offset */
      rc = BMEM_ConvertAddressToCached(
               hVceCh->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
               hVceCh->userdata.pDescriptorBufferUnCached,
               &hVceCh->userdata.pDescriptorBufferCached
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error converting virtual address to cached"));
         BVCE_Channel_Close( hVceCh );
         return BERR_TRACE( rc );
      }
   }

   BVCE_Power_P_AcquireResource(
         hVce,
         BVCE_Power_Type_eClock
         );

   /* Send Open Channel Command */
   rc = BVCE_P_SendCommand_OpenChannel(
            hVce,
            hVceCh
            );
   if ( BERR_SUCCESS != rc )
   {
      BVCE_Channel_Close( hVceCh );
      return BERR_TRACE( rc );
   }

   if ( 0 != hVce->stPlatformConfig.stMailbox.uiBvn2ViceMailboxAddress )
   {
      uint32_t uiBVNMetaDataOffset = BREG_Read32(
               hVce->handles.hReg,
               hVce->stPlatformConfig.stMailbox.uiBvn2ViceMailboxAddress
               );

      BDBG_ASSERT( BVN2VICE_MBOX_PAYLOAD_OFFSET == uiBVNMetaDataOffset );

      hVceCh->uiBVNMetaDataRegisterStartAddress = hVce->fw.dccm.uiRegisterBaseAddress[0] + uiBVNMetaDataOffset;
      hVceCh->stStatus.uiBVNMetaDataRegisterStartAddress = hVceCh->uiBVNMetaDataRegisterStartAddress;
   }

#if BVCE_P_DUMP_USERDATA_LOG
         /* Debug */
         if ( NULL == hVceCh->userdata.hUserDataLog )
         {
            char fname[256];
            sprintf(fname, "BVCE_USERDATA_LOG_%02d_%02d.csv", hVceCh->hVce->stOpenSettings.uiInstance, hVceCh->stOpenSettings.uiInstance);

            if ( false == BVCE_Debug_P_OpenLog( fname, &hVceCh->userdata.hUserDataLog ) )
            {
               BDBG_ERR(("Error Creating BVCE User Data Log Dump File (%s)", fname));
            }
            else
            {
               BVCE_Debug_P_WriteLog_isr(hVceCh->userdata.hUserDataLog, "stc,queue_read_offset,queue_write_offset,queue_metadata_stg_id,queue_metadata_polarity,queue_metadata_length,offset,index,length,type,analog,num_lines,cc_metadata,cc_data_0,cc_data_1\n");
            }
         }
#endif

   BVCE_Power_P_ReleaseResource(
         hVce,
         BVCE_Power_Type_eClock
         );

   *phVceCh = hVceCh;

   BDBG_LEAVE( BVCE_Channel_Open );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Channel_Close(
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;
   BVCE_P_HeapId eHeapId;

   BDBG_ENTER( BVCE_Channel_Close );

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         BDBG_ERR(("Channel is not open"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eOpened:
         /* We're good to go */
         break;

      case BVCE_P_Status_eStarted:
         /* Let's stop the channel and then close */
         BDBG_WRN(("Channel is started, stopping/flushing first..."));
         {
            BVCE_Channel_StopEncodeSettings stChStopEncodeSettings;

            BVCE_Channel_GetDefaultStopEncodeSettings(&stChStopEncodeSettings);

            stChStopEncodeSettings.eStopMode = BVCE_Channel_StopMode_eAbort;

            rc = BVCE_Channel_StopEncode(
               hVceCh,
               &stChStopEncodeSettings
               );
         }
         break;

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         BDBG_WRN(("Channel is in process of stopping, flushing..."));
         BVCE_Channel_FlushEncode( hVceCh );
         break;

      default:
         BDBG_ERR(("Unknown state"));
         return BERR_TRACE( BERR_UNKNOWN );
   }

#if BVCE_P_TEST_MODE
   BVCE_Debug_P_CloseLog( hVceCh->hStatusLog );
   hVceCh->hStatusLog = NULL;
#endif

   /* Send Close Channel Command */
   rc = BERR_TRACE( BVCE_P_SendCommand_CloseChannel(
            hVceCh->hVce,
            hVceCh
            ));

   hVceCh->eStatus = BVCE_P_Status_eIdle;


   if ( NULL != hVceCh->userdata.pDescriptorBufferUnCached )
   {
      BMEM_Free(
               hVceCh->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
               hVceCh->userdata.pDescriptorBufferUnCached
               );
   }

   for ( eHeapId = 0; eHeapId < BVCE_P_HeapId_eMax; eHeapId++ )
   {
      if ( NULL != hVceCh->memory[eHeapId].pBufferUnCached )
      {
         BMEM_Free(
                  hVceCh->memory[eHeapId].hMem,
                  hVceCh->memory[eHeapId].pBufferUnCached
                  );
      }
   }

   BDBG_LEAVE( BVCE_Channel_Close );

   return BERR_TRACE( rc );
}

/*********************/
/* Channel Callbacks */
/*********************/
void
BVCE_Channel_GetDefaultCallbackSettings(
         BVCE_Channel_CallbackSettings* pstCallbackSettings
         )
{
   BDBG_ENTER( BVCE_Channel_GetDefaultCallbackSettings );

   BDBG_ASSERT( pstCallbackSettings );

   BKNI_Memset(
            pstCallbackSettings,
            0,
            sizeof( BVCE_Channel_CallbackSettings )
            );

   pstCallbackSettings->uiSignature = BVCE_P_SIGNATURE_CHANNELCALLBACKSETTINGS;

   BDBG_LEAVE( BVCE_Channel_GetDefaultCallbackSettings );
   return;
}

BERR_Code
BVCE_Channel_SetCallbackSettings(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_CallbackSettings* pstCallbackSettings
         )
{
   BERR_Code rc;
   bool bConfigNow = true;

   BDBG_ENTER( BVCE_Channel_SetCallbackSettings );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstCallbackSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_CHANNELCALLBACKSETTINGS == pstCallbackSettings->uiSignature );

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         BDBG_ERR(("Channel is not open"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eOpened:
         bConfigNow = false; /* Channel will be configured at StartEncode */
         break;

      case BVCE_P_Status_eStarted:
         bConfigNow = true; /* Channel is started, so needs to configure only dynamic parameters */
         break;

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         BDBG_ERR(("Channel not completed stopping, will be configured when re-started"));
         bConfigNow = false; /* Channel will be configured at StartEncode */
         break;

      default:
         BDBG_ERR(("Unknown state"));
         return BERR_TRACE( BERR_UNKNOWN );
   }

   if ( ( hVceCh->stCallbackSettings.stEvent.uiEventMask == pstCallbackSettings->stEvent.uiEventMask )
        && ( hVceCh->stCallbackSettings.stEvent.uiErrorMask == pstCallbackSettings->stEvent.uiErrorMask )
      )
   {
      /* Error/Event masks haven't changed, so no need to send a config command to the FW */
      bConfigNow = false;
   }

   BKNI_EnterCriticalSection();

   hVceCh->stCallbackSettings = *pstCallbackSettings;

   BKNI_LeaveCriticalSection();

   if ( true == bConfigNow )
   {
      /* Send Config Channel Command */
      rc = BVCE_P_SendCommand_ConfigChannel(
               hVceCh->hVce,
               hVceCh,
               NULL
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error configuring channel"));
         return BERR_TRACE( rc );
      }
   }

   BDBG_LEAVE( BVCE_Channel_SetCallbackSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Channel_GetCallbackSettings(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_CallbackSettings* pstCallbackSettings
         )
{
   BDBG_ENTER( BVCE_Channel_GetCallbackSettings );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstCallbackSettings );

   *pstCallbackSettings = hVceCh->stCallbackSettings;

   BDBG_LEAVE( BVCE_Channel_GetCallbackSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

/*******************************/
/* Channel Start/Settings/Stop */
/*******************************/
static const BVCE_Channel_StartEncodeSettings s_stDefaultStartEncodeSettings =
{
 BVCE_P_SIGNATURE_STARTENCODESETTINGS, /* Signature */
 false, /* Real Time Encode Mode */
 false, /* Pipeline Normal Delay Mode */
 false, /* Fast Channel Change Mode is OFF */
 {
  BAVC_VideoCompressionStd_eH264, /* Protocol */
  BAVC_VideoCompressionProfile_eHigh, /* Profile */
  BAVC_VideoCompressionLevel_e40, /* Level */
 },
 BAVC_ScanType_eProgressive, /* Scan Type */
 NULL, /* Output Handle */
 1, /* Encoder defaults to STC[1].  Decoder typically uses STC[0]. */
 {
    {
       BAVC_FrameRateCode_e14_985,
       BAVC_FrameRateCode_e60,
    },
    {
       BAVC_FrameRateCode_e23_976,
    },
    {
       {
          DEFAULT_A2P_MAX_PIC_WIDTH_PELS,
          DEFAULT_A2P_MAX_PIC_HEIGT_PELS,
       },
    },
    {
     14,    /* uiNumberOfPFrames */
     2,     /* uiNumberOfBFrames */
     false, /* bAllowOpenGop = false */
    },
 },
 DEFAULT_A2P_RATE_BUFFER,
 0, /* uiNumParallelNRTEncodes */
};

void
BVCE_Channel_GetDefaultStartEncodeSettings(
         BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings
         )
{
   BDBG_ENTER( BVCE_Channel_GetDefaultStartEncodeSettings );

   BDBG_ASSERT( pstChStartEncodeSettings );

   *pstChStartEncodeSettings = s_stDefaultStartEncodeSettings;

   BDBG_LEAVE( BVCE_Channel_GetDefaultStartEncodeSettings );
   return;
}

/* BVCE_Channel_StartEncode - Configures the encoder and starts the encode process.
 *
 * Note: BVCE_Channel_SetEncodeSettings() should be called before this to set up the initial encode parameters
 */
BERR_Code
BVCE_Channel_P_StartEncode_impl(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings
         )
{
   BERR_Code rc;
   BVCE_P_SendCommand_ConfigChannel_Settings stConfigChannelSettings;

   BDBG_ENTER( BVCE_Channel_P_StartEncode_impl );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstChStartEncodeSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_STARTENCODESETTINGS == pstChStartEncodeSettings->uiSignature );
   BDBG_ASSERT( NULL != pstChStartEncodeSettings->hOutputHandle );
   BDBG_ASSERT( NULL != pstChStartEncodeSettings->hOutputHandle->hOutputBuffers );

   hVceCh->stStartEncodeSettings = *pstChStartEncodeSettings;

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         BDBG_ERR(("Channel is not open"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eOpened:
         /* We're good to go */
         break;

      case BVCE_P_Status_eStarted:
         BDBG_ERR(("Channel is already started"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         BDBG_ERR(("Channel has not completed stopping, mux should flush any stale descriptors"));
         break;

      default:
         BDBG_ERR(("Unknown state"));
         return BERR_TRACE( BERR_UNKNOWN );
   }

#if BVCE_P_TEST_MODE
   BVCE_Debug_P_CloseLog( hVceCh->hStatusLog );
   hVceCh->hStatusLog = NULL;
#endif

   BKNI_Memset( &stConfigChannelSettings, 0, sizeof( BVCE_P_SendCommand_ConfigChannel_Settings ) );

   stConfigChannelSettings.bFastChannelChange = hVceCh->stStartEncodeSettings.bAdaptiveLowDelayMode;

   /* Send Config Channel Command */
   rc = BVCE_P_SendCommand_ConfigChannel(
            hVceCh->hVce,
            hVceCh,
            &stConfigChannelSettings
            );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error configuring channel"));
      return BERR_TRACE( rc );
   }

   /* Reset Status */
   rc = BVCE_Channel_ClearStatus(
            hVceCh,
            NULL
            );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error clearing channel status"));
      return BERR_TRACE( rc );
   }

   /* Send Start Channel Command */
   rc = BVCE_P_SendCommand_StartChannel(
            hVceCh->hVce,
            hVceCh
            );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error starting channel"));
      return BERR_TRACE( rc );
   }

   pstChStartEncodeSettings->hOutputHandle->state.hVceCh = hVceCh;
   /* SW7425-4186: Clear EOS event on start encode */
   hVceCh->stStatus.uiEventFlags &= ~BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS;

   /* Set Channel Status */
   hVceCh->eStatus = BVCE_P_Status_eStarted;

   BDBG_LEAVE( BVCE_Channel_P_StartEncode_impl );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Channel_StartEncode(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_Channel_StartEncode );

   BVCE_Power_P_AcquireResource(
         hVceCh->hVce,
         BVCE_Power_Type_eClock
         );

   rc = BVCE_Channel_P_StartEncode_impl(
         hVceCh,
         pstChStartEncodeSettings
         );

   if ( rc != BERR_SUCCESS )
   {
      BVCE_Power_P_ReleaseResource(
            hVceCh->hVce,
            BVCE_Power_Type_eClock
            );
   }

   BDBG_LEAVE( BVCE_Channel_StartEncode );

   return BERR_TRACE( rc );
}

static const BVCE_Channel_StopEncodeSettings s_stDefaultStopEncodeSettings =
{
 BVCE_P_SIGNATURE_STOPENCODESETTINGS, /* Signature */
 BVCE_Channel_StopMode_eNormal        /* Stop Mode */
};

void
BVCE_Channel_GetDefaultStopEncodeSettings(
         BVCE_Channel_StopEncodeSettings *pstChStopEncodeSettings
         )
{
   BDBG_ENTER( BVCE_Channel_GetDefaultStopEncodeSettings );

   BDBG_ASSERT( pstChStopEncodeSettings );

   *pstChStopEncodeSettings = s_stDefaultStopEncodeSettings;

   BDBG_LEAVE( BVCE_Channel_GetDefaultStopEncodeSettings );
   return;
}

/* BVCE_Channel_StopEncode - Stops the encode process.
 */
BERR_Code
BVCE_Channel_StopEncode(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_StopEncodeSettings *pstChStopEncodeSettings
         )
{
   BERR_Code rc;
   BDBG_ENTER( BVCE_Channel_StopEncode );

   if ( NULL == pstChStopEncodeSettings )
   {
      hVceCh->stStopEncodeSettings = s_stDefaultStopEncodeSettings;
   }
   else
   {
      BDBG_ASSERT( BVCE_P_SIGNATURE_STOPENCODESETTINGS == pstChStopEncodeSettings->uiSignature );
      hVceCh->stStopEncodeSettings = *pstChStopEncodeSettings;
   }

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         BDBG_ERR(("Channel is not open"));
         BDBG_LEAVE( BVCE_Channel_StopEncode );
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eOpened:
         BDBG_LEAVE( BVCE_Channel_StopEncode );
         return BERR_TRACE( BERR_SUCCESS );

      case BVCE_P_Status_eStarted:
         {
            BVCE_Channel_Status stChannelStatus;
            rc = BVCE_Channel_GetStatus( hVceCh, &stChannelStatus );

            if ( rc != BERR_SUCCESS )
            {
               BDBG_LEAVE( BVCE_Channel_StopEncode );
               return BERR_TRACE( rc );
            }

            /* Check if in-band stop has already occurred */
            if ( 0 != ( stChannelStatus.uiEventFlags & BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS ) )
            {
               hVceCh->eStatus = BVCE_P_Status_eOpened;
               BDBG_LEAVE( BVCE_Channel_StopEncode );
               return BERR_TRACE( BERR_SUCCESS );
            }
         }
         break;

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         BDBG_ERR(("Channel is already stopping"));
         BDBG_LEAVE( BVCE_Channel_StopEncode );
         return BERR_TRACE( BERR_UNKNOWN );

      default:
         BDBG_ERR(("Unknown state"));
         BDBG_LEAVE( BVCE_Channel_StopEncode );
         return BERR_TRACE( BERR_UNKNOWN );
   }

#if BVCE_P_TEST_MODE
   BVCE_Debug_P_CloseLog( hVceCh->hConfigLog );
   hVceCh->hConfigLog = NULL;
#endif

   /* Send Stop Channel Command */
   rc = BVCE_P_SendCommand_StopChannel(
            hVceCh->hVce,
            hVceCh
            );
   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error stopping channel"));
      return BERR_TRACE( rc );
   }

   /* Set Channel Status */
   hVceCh->eStatus = BVCE_P_Status_eStopping; /* State changes to eOpened when EOS ITB entry is seen */

#if BVCE_P_DUMP_USERDATA_LOG
   BVCE_Debug_P_CloseLog( hVceCh->userdata.hUserDataLog );
   hVceCh->userdata.hUserDataLog = NULL;
#endif

   if ( BVCE_Channel_StopMode_eAbort == hVceCh->stStopEncodeSettings.eStopMode )
   {
      BVCE_Channel_FlushEncode( hVceCh );
   }

   BDBG_LEAVE( BVCE_Channel_StopEncode );
   return BERR_TRACE( rc );
}

void
BVCE_Channel_P_FlushEncode(
         BVCE_Channel_Handle hVceCh
         )
{
   BDBG_ENTER( BVCE_Channel_P_FlushEncode );

   if ( NULL != hVceCh->stStartEncodeSettings.hOutputHandle )
   {
      BVCE_Output_Flush( hVceCh->stStartEncodeSettings.hOutputHandle );
   }

   BDBG_LEAVE( BVCE_Channel_P_FlushEncode );
}

BERR_Code
BVCE_Channel_FlushEncode(
   BVCE_Channel_Handle hVceCh
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BVCE_Channel_P_FlushEncode );

   if ( BVCE_P_Status_eStopping == hVceCh->eStatus )
   {
#define BVCE_P_STOP_RECOVERY_DELAY 10
#define BVCE_P_STOP_RECOVERY_TIME 500
      unsigned uiNumIterations = BVCE_P_STOP_RECOVERY_TIME / BVCE_P_STOP_RECOVERY_DELAY;

      rc = BVCE_Channel_GetStatus( hVceCh, &hVceCh->stStatus );
      while ( ( uiNumIterations != 0 )
              && ( BVCE_P_Status_eOpened != hVceCh->eStatus )
              && ( BERR_SUCCESS == rc )
            )
      {
         BVCE_Channel_P_FlushEncode( hVceCh );
         BKNI_Sleep(BVCE_P_STOP_RECOVERY_DELAY);
         rc = BVCE_Channel_GetStatus( hVceCh, &hVceCh->stStatus );
         uiNumIterations--;
      }

      if ( 0 == ( hVceCh->stStatus.uiEventFlags & BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS ) )
      {
         rc = BERR_TRACE( BERR_UNKNOWN );
         BDBG_ERR(("VCE FW not done with stop, yet"));
      }

      if ( ( NULL != hVceCh->stStartEncodeSettings.hOutputHandle )
           && ( BVCE_P_Status_eOpened != hVceCh->eStatus )
           && ( BVCE_P_Output_BufferAccessMode_eDirect != hVceCh->stStartEncodeSettings.hOutputHandle->state.eBufferAccessMode )
          )
      {
         rc = BERR_TRACE( BERR_UNKNOWN );
         BDBG_ERR(("EOS not seen in ITB, yet"));
      }

      BVCE_Channel_P_HandleEOSEvent(
         hVceCh
         );
   }
   else
   {
      BVCE_Channel_P_FlushEncode( hVceCh );
   }

   BDBG_LEAVE( BVCE_Channel_FlushEncode );
   return BERR_TRACE( rc );
}

/********************/
/* Channel Settings */
/********************/

static const BVCE_Channel_EncodeSettings s_stDefaultChEncodeSettings =
{
 BVCE_P_SIGNATURE_CHANNELENCODESETTINGS, /* Signature */
 /* Frame Rate */
 {
  BAVC_FrameRateCode_e30,  /* 30 fps */
  false, /* frame rate mode=fixed*/
 },
 {
  6000000, /* Max Bit Rate */
  0, /* Target Bit Rate */
 },
 0, /* end-to-end delay */
 {
  14,    /* uiNumberOfPFrames */
  0,     /* uiNumberOfBFrames */
  false, /* bAllowOpenGop = false */
 },
 true, /* bITFPEnable */
 0, /* uiNumSlicesPerPic */
};

void
BVCE_Channel_GetDefaultEncodeSettings(
         BVCE_Channel_EncodeSettings *pstChEncodeSettings
         )
{
   BDBG_ENTER( BVCE_Channel_GetDefaultEncodeSettings );

   BDBG_ASSERT( pstChEncodeSettings );

   if ( NULL == pstChEncodeSettings )
   {
      return;
   }

   *pstChEncodeSettings = s_stDefaultChEncodeSettings;

   BDBG_LEAVE( BVCE_Channel_GetDefaultEncodeSettings );
   return;
}

BERR_Code
BVCE_Channel_SetEncodeSettings(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_EncodeSettings *pstChEncodeSettings
         )
{
   BERR_Code rc;
   BVCE_P_SendCommand_ConfigChannel_Settings stConfigChannelSettings;
   bool bConfigNow = true;

   BDBG_ENTER( BVCE_Channel_SetEncodeSettings );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstChEncodeSettings );

   BKNI_Memset( &stConfigChannelSettings, 0, sizeof( BVCE_P_SendCommand_ConfigChannel_Settings ) );

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         BDBG_ERR(("Channel is not open"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eOpened:
         bConfigNow = false; /* Channel will be configured at StartEncode */
         break;

      case BVCE_P_Status_eStarted:
         bConfigNow = true; /* Channel is started, so needs to configure only dynamic parameters */
         /* SW7425-2078: Set Begin New RAP if GOP structure changes */
         if ( ( pstChEncodeSettings->stGOPStructure.bAllowOpenGOP != hVceCh->stEncodeSettings.stGOPStructure.bAllowOpenGOP )
              || ( pstChEncodeSettings->stGOPStructure.uiNumberOfBFrames != hVceCh->stEncodeSettings.stGOPStructure.uiNumberOfBFrames )
              || ( pstChEncodeSettings->stGOPStructure.uiNumberOfPFrames != hVceCh->stEncodeSettings.stGOPStructure.uiNumberOfPFrames )
            )
         {
            stConfigChannelSettings.bBeginNewRAP = true;
         }
         break;

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         if ( BVCE_Channel_StopMode_eNormal == hVceCh->stStopEncodeSettings.eStopMode )
         {
            BDBG_ERR(("Channel not completed stopping, will be configured when re-started"));
         }
         bConfigNow = false; /* Channel will be configured at StartEncode */
         break;

      default:
         BDBG_ERR(("Unknown state"));
         return BERR_TRACE( BERR_UNKNOWN );
   }

   hVceCh->stEncodeSettings = *pstChEncodeSettings;

   if ( true == bConfigNow )
   {
      if ( true == hVceCh->bPendingEncodeSettings )
      {
         BDBG_WRN(("BVCE_Channel_SetEncodeSettings() - There are pending settings that have not, yet, been applied!"));
      }

      /* Send Config Channel Command */
      rc = BVCE_P_SendCommand_ConfigChannel(
               hVceCh->hVce,
               hVceCh,
               &stConfigChannelSettings
               );

      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error configuring channel"));
         return BERR_TRACE( rc );
      }

      if ( true == hVceCh->bPendingEncodeSettings )
      {
         hVceCh->bPendingEncodeSettings = false;
      }
   }

   BDBG_LEAVE( BVCE_Channel_SetEncodeSettings );
   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Channel_GetEncodeSettings(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_EncodeSettings *pstChEncodeSettings
         )
{
   BERR_Code rc;

   BDBG_ENTER( BVCE_Channel_GetEncodeSettings );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstChEncodeSettings );

   /* We get status to make sure we get any pending settings that were applied first */
   rc = BVCE_Channel_GetStatus( hVceCh, NULL );

   *pstChEncodeSettings = hVceCh->stEncodeSettings;

   BDBG_LEAVE( BVCE_Channel_GetEncodeSettings );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Channel_P_GetDefaultEncodeSettings_OnInputChange(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_EncodeSettings_OnInputChange *pstChEncodeSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstChEncodeSettings );

   BDBG_ENTER( BVCE_Channel_P_GetDefaultEncodeSettings_OnInputChange );

   /* We get status to make sure we get any pending settings that were applied first */
   rc = BVCE_Channel_GetStatus( hVceCh, NULL );

   BKNI_Memset( pstChEncodeSettings,
                0,
                sizeof( BVCE_Channel_EncodeSettings_OnInputChange )
              );

   pstChEncodeSettings->uiSignature = BVCE_P_SIGNATURE_CHANNELENCODESETTINGSONINPUTCHANGE;

   pstChEncodeSettings->stBitRate = hVceCh->stEncodeSettings.stBitRate;

   BDBG_LEAVE( BVCE_Channel_P_GetDefaultEncodeSettings_OnInputChange );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Channel_BeginNewRAP(
         BVCE_Channel_Handle hVceCh
         )
{
   BERR_Code rc;
   BVCE_P_SendCommand_ConfigChannel_Settings stConfigChannelSettings;

   BDBG_ENTER( BVCE_Channel_BeginNewRAP );

   if ( BVCE_P_Status_eStarted != hVceCh->eStatus )
   {
      BDBG_ERR(("Channel must be started to insert a RAP point"));
      return BERR_TRACE( BERR_UNKNOWN );
   }

   BKNI_Memset( &stConfigChannelSettings, 0, sizeof( BVCE_P_SendCommand_ConfigChannel_Settings ) );

   stConfigChannelSettings.bBeginNewRAP = true;

   /* Send Config Channel Command */
   rc = BVCE_P_SendCommand_ConfigChannel(
            hVceCh->hVce,
            hVceCh,
            &stConfigChannelSettings
            );

   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("BVCE_Channel_BeginNewRAP: Error configuring channel"));
   }

   BDBG_LEAVE( BVCE_Channel_BeginNewRAP );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Channel_SetEncodeSettings_OnInputChange(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_EncodeSettings_OnInputChange *pstChEncodeSettings
         )
{
   BERR_Code rc;
   bool bConfigNow = true;
   BVCE_P_SendCommand_ConfigChannel_Settings stConfigChannelSettings;

   BDBG_ENTER( BVCE_Channel_SetEncodeSettings_OnInputChange );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstChEncodeSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_CHANNELENCODESETTINGSONINPUTCHANGE == pstChEncodeSettings->uiSignature );

   BKNI_Memset( &stConfigChannelSettings, 0, sizeof( BVCE_P_SendCommand_ConfigChannel_Settings ) );

   switch ( hVceCh->eStatus )
   {
      case BVCE_P_Status_eIdle:
         BDBG_ERR(("Channel is not open"));
         return BERR_TRACE( BERR_UNKNOWN );

      case BVCE_P_Status_eOpened:
         BDBG_ERR(("Channel not started, will be configured immediately"));
         bConfigNow = false; /* Channel will be configured at StartEncode */
         break;

      case BVCE_P_Status_eStarted:
         bConfigNow = true; /* Channel is started, so needs to configure only dynamic parameters */
         break;

      case BVCE_P_Status_eStopping:
         /* Continue with warning */
         BDBG_ERR(("Channel not completed stopping, will be configured when re-started"));
         bConfigNow = false; /* Channel will be configured at StartEncode */
         break;

      default:
         BDBG_ERR(("Unknown state"));
         return BERR_TRACE( BERR_UNKNOWN );
   }

   hVceCh->stPendingEncodeSettings = hVceCh->stEncodeSettings;
   if ( hVceCh->stPendingEncodeSettings.stBitRate.uiMax == pstChEncodeSettings->stBitRate.uiMax )
   {
      /* settings haven't changed, so no need to send config command */
      bConfigNow = false;
   }
   else
   {
      hVceCh->stPendingEncodeSettings.stBitRate = pstChEncodeSettings->stBitRate;
   }

   if ( true == bConfigNow )
   {

      if ( true == hVceCh->bPendingEncodeSettings )
      {
         BDBG_WRN(("BVCE_Channel_SetEncodeSettings_OnInputChange() - There are pending settings that have not, yet, been applied"));
      }

      stConfigChannelSettings.bOnInputChange = true;

      /* Send Config Channel Command */
      rc = BVCE_P_SendCommand_ConfigChannel(
               hVceCh->hVce,
               hVceCh,
               &stConfigChannelSettings
               );
      if ( BERR_SUCCESS != rc )
      {
         BDBG_ERR(("Error configuring channel"));
         return BERR_TRACE( rc );
      }

      hVceCh->bPendingEncodeSettings = true;
   }

   BDBG_LEAVE( BVCE_Channel_SetEncodeSettings_OnInputChange );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BVCE_Channel_GetEncodeSettings_OnInputChange(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_EncodeSettings_OnInputChange *pstChEncodeSettings
         )
{
   BERR_Code rc;
   BDBG_ENTER( BVCE_Channel_GetEncodeSettings_OnInputChange );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstChEncodeSettings );

   rc = BVCE_Channel_P_GetDefaultEncodeSettings_OnInputChange( hVceCh, pstChEncodeSettings );

   if ( true == hVceCh->bPendingEncodeSettings )
   {
      pstChEncodeSettings->stBitRate = hVceCh->stPendingEncodeSettings.stBitRate;
   }

   BDBG_LEAVE( BVCE_Channel_GetEncodeSettings_OnInputChange );

   return BERR_TRACE( rc );
}

/******************/
/* Channel Status */
/******************/

BERR_Code
BVCE_Channel_GetStatus(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_Status *pChannelStatus
         )
{
   BERR_Code rc;
   BDBG_ENTER( BVCE_Channel_GetStatus );

   BDBG_ASSERT( hVceCh );

   /* Get Updated Channel Status from FW */
   rc = BERR_TRACE( BVCE_P_SendCommand_GetChannelStatus(
            hVceCh->hVce,
            hVceCh
            ));

   if ( BERR_SUCCESS != rc )
   {
      BDBG_ERR(("Error getting channel status"));
   }
   else
   {
      /* Update local copy of channel status */

      /* Set Event/Error Flags */
      {
         uint32_t uiFlags;
         uint8_t uiIndex;

         uiFlags = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.uiEventFlags;
         uiIndex = 0;

         while ( uiFlags )
         {
            if ( 0 != ( uiFlags & 0x1 ) )
            {
               hVceCh->stStatus.uiEventFlags |= BVCE_P_EventMaskReverseLUT[uiIndex];

               /* Process Pending Changes */
               if ( ( true == hVceCh->bPendingEncodeSettings )
                    && ( BVCE_CHANNEL_STATUS_FLAGS_EVENT_INPUT_CHANGE == BVCE_P_EventMaskReverseLUT[uiIndex] )
                  )
               {
                  hVceCh->stEncodeSettings = hVceCh->stPendingEncodeSettings;
                  hVceCh->bPendingEncodeSettings = false;
               }

            }

            uiFlags >>= 1;
            uiIndex++;
         }

         uiFlags = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.uiEventFlags;
         uiIndex = 0;

         while ( uiFlags )
         {
            if ( 0 != ( uiFlags & 0x1 ) )
            {
               hVceCh->stStatus.uiErrorFlags |= BVCE_P_ErrorMaskReverseLUT[uiIndex];
            }

            uiFlags >>= 1;
            uiIndex++;
         }
      }

      hVceCh->stStatus.uiTotalErrors = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.uiTotalErrors;
      hVceCh->stStatus.uiTotalPicturesReceived = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsMbArcFinished
              + hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsVipDroppedDueToFRC
              + hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsVipDroppedDueToPerformance
              + hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsVipDroppedDueToHRDUnderFlow;
      hVceCh->stStatus.uiTotalPicturesDroppedDueToFrameRateConversion = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsVipDroppedDueToFRC;
      hVceCh->stStatus.uiTotalPicturesDroppedDueToErrors = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsVipDroppedDueToPerformance;
      hVceCh->stStatus.uiTotalPicturesEncoded = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsMbArcFinished;
      hVceCh->stStatus.uiLastPictureIdEncoded = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.PicId;
      hVceCh->stStatus.uiSTCSnapshot = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.uiSTCSnapshot;
      hVceCh->stStatus.uiTotalPicturesDroppedDueToHRDUnderflow = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.NumOfPicsVipDroppedDueToHRDUnderFlow;
      hVceCh->stStatus.uiEtsDtsOffset = hVceCh->hVce->fw.stResponse.type.stGetChannelStatus.StatusInfoStruct.uiEtsDtsOffset;
   }

   if ( NULL != pChannelStatus )
   {
      *pChannelStatus = hVceCh->stStatus;

#if BVCE_P_TEST_MODE
   if ( NULL == hVceCh->hStatusLog )
   {
      char fname[256];
      sprintf(fname, "BVCE_STATUS_%02d_%02d_%03d.csv", hVceCh->hVce->stOpenSettings.uiInstance, hVceCh->stOpenSettings.uiInstance, hVceCh->uiStatusLogInstance);
      hVceCh->uiStatusLogInstance++;

      if ( false == BVCE_Debug_P_OpenLog( fname, &hVceCh->hStatusLog ) )
      {
         BDBG_ERR(("Error Creating BVCE Channel Status Dump File (%s)", fname));
      }
      else
      {
         BVCE_Debug_P_WriteLog_isr( hVceCh->hStatusLog, "count,error flags,event flags,total errors,received,dropped (frc),dropped (err),encoded,stc snapshot,dropped (hrd underflow),ets/dts offset (27Mhz)");
         BVCE_Debug_P_WriteLog_isr( hVceCh->hStatusLog, "\n" );
      }
   }

   if ( NULL != hVceCh->hStatusLog )
   {
      BDBG_CWARNING( sizeof( hVceCh->stStatus ) == (12*4 + 1*8));

      /* Start Encode Settings */
      BVCE_Debug_P_WriteLog_isr( hVceCh->hStatusLog, "%u,%u,%u,%u,%u,%u,%u,%u,%llu,%u,%u",
         ( NULL == hVceCh->stStartEncodeSettings.hOutputHandle ) ? (unsigned)-1 : hVceCh->stStartEncodeSettings.hOutputHandle->uiDescriptorCount,
         hVceCh->stStatus.uiErrorFlags,
         hVceCh->stStatus.uiEventFlags,
         hVceCh->stStatus.uiTotalErrors,
         hVceCh->stStatus.uiTotalPicturesReceived,
         hVceCh->stStatus.uiTotalPicturesDroppedDueToFrameRateConversion,
         hVceCh->stStatus.uiTotalPicturesDroppedDueToErrors,
         hVceCh->stStatus.uiTotalPicturesEncoded,
         hVceCh->stStatus.uiSTCSnapshot,
         hVceCh->stStatus.uiTotalPicturesDroppedDueToHRDUnderflow,
         hVceCh->stStatus.uiEtsDtsOffset
      );

      BVCE_Debug_P_WriteLog_isr( hVceCh->hStatusLog, "\n" );
   }
#endif
   }

   BDBG_LEAVE( BVCE_Channel_GetStatus );
   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Channel_ClearStatus(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_Status *pChannelStatus
         )
{
   BDBG_ENTER( BVCE_Channel_ClearStatus );

   BDBG_ASSERT( hVceCh );

   if ( NULL != pChannelStatus )
   {
      hVceCh->stStatus.uiErrorFlags &= ~pChannelStatus->uiErrorFlags;
      hVceCh->stStatus.uiEventFlags &= ~pChannelStatus->uiEventFlags;

#define BVCE_STATUS_FIELD(_field) \
      if ( 0 != pChannelStatus->_field )\
      {\
         hVceCh->stStatus._field = 0;\
      }
#include "bvce_status.inc"
   }
   else
   {
      BKNI_Memset(
               &hVceCh->stStatus,
               0,
               sizeof( BVCE_Channel_Status )
               );
   }

   hVceCh->stStatus.uiBVNMetaDataRegisterStartAddress = hVceCh->uiBVNMetaDataRegisterStartAddress;

   BDBG_LEAVE( BVCE_Channel_ClearStatus );
   return BERR_TRACE( BERR_SUCCESS );
}

#define BVCE_UserData_P_Set32_LE( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) ((data) >> 0 ) & 0xFF; \
   ((uint8_t*)(pBuffer))[offset+1] = (uint8_t) ((data) >> 8 ) & 0xFF; \
   ((uint8_t*)(pBuffer))[offset+2] = (uint8_t) ((data) >> 16) & 0xFF; \
   ((uint8_t*)(pBuffer))[offset+3] = (uint8_t) ((data) >> 24) & 0xFF; \
}

#define BVCE_UserData_P_Set16_LE( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) ((data) >> 0 ) & 0xFF; \
   ((uint8_t*)(pBuffer))[offset+1] = (uint8_t) ((data) >> 8 ) & 0xFF; \
}

#define BVCE_UserData_P_Get16_LE( pBuffer, offset ) ((((uint16_t) ((uint8_t*)(pBuffer))[offset+1]) << 8) | ((uint8_t*)(pBuffer))[offset+0])

#define BVCE_UserData_P_Set8( pBuffer, offset, data ) \
{\
   ((uint8_t*)(pBuffer))[offset+0] = (uint8_t) (data); \
}

#define BVCE_FW_P_UserData_PacketDescriptor_Length_OFFSET 0
#define BVCE_FW_P_UserData_PacketDescriptor_Type_OFFSET 2
#define BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET 4

#define BVCE_FW_P_UserData_CCPayload_NumValid608Lines_OFFSET 0
#define BVCE_FW_P_UserData_CCPayload_608Data_OFFSET (BVCE_FW_P_UserData_CCPayload_NumValid608Lines_OFFSET + 1)
#define BVCE_FW_P_UserData_CCPayload_Get608Line_OFFSET(ui608Line) ( BVCE_FW_P_UserData_CCPayload_608Data_OFFSET + ((1 + BVCE_FW_P_UserData_Payload_CC_608_BYTES_PER_LINE_MAX) * ui608Line ))

#define BVCE_FW_P_UserData_CCPayload_NumValid708Lines_OFFSET BVCE_FW_P_UserData_Payload_CC_608_LENGTH
#define BVCE_FW_P_UserData_CCPayload_708Data_OFFSET (BVCE_FW_P_UserData_CCPayload_NumValid708Lines_OFFSET+1)
#define BVCE_FW_P_UserData_CCPayload_Get708Line_OFFSET(ui708Line) ( BVCE_FW_P_UserData_CCPayload_708Data_OFFSET + ((1 + BVCE_FW_P_UserData_Payload_CC_708_BYTES_PER_LINE_MAX) * ui708Line ))


#define BVCE_P_UserData_PacketType_UNSUPPORTED 0xFFFF

static const uint16_t BVCE_P_UserData_PacketTypeLUT[BUDP_DCCparse_Format_LAST] =
{
   BVCE_P_UserData_PacketType_UNSUPPORTED, /* BUDP_DCCparse_Format_Unknown */
   BVCE_FW_P_UserData_PacketType_eSCTE_20, /* BUDP_DCCparse_Format_DVS157 */
   BVCE_FW_P_UserData_PacketType_eATSC_A53, /* BUDP_DCCparse_Format_ATSC53 */
   BVCE_FW_P_UserData_PacketType_eSCTE_21, /* BUDP_DCCparse_Format_DVS053 */
   BVCE_P_UserData_PacketType_UNSUPPORTED, /* BUDP_DCCparse_Format_SEI */
   BVCE_P_UserData_PacketType_UNSUPPORTED, /* BUDP_DCCparse_Format_SEI2 */
   BVCE_P_UserData_PacketType_UNSUPPORTED, /* BUDP_DCCparse_Format_AFD53 */
};

static void
BVCE_UserData_P_ParsePacketDescriptor(
   const BUDP_Encoder_PacketDescriptor *pPacketDescriptor,
   void *pFWPacketDescriptor
   )
{
   unsigned i;
   volatile void * pCCPayload = (void*) ((uint32_t) pFWPacketDescriptor + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET);

   BDBG_ENTER( BVCE_UserData_P_ParsePacketDescriptor );

   BKNI_Memset( pFWPacketDescriptor, 0, BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH );

   BDBG_ASSERT( BVCE_P_UserData_PacketType_UNSUPPORTED != BVCE_P_UserData_PacketTypeLUT[pPacketDescriptor->ePacketFormat] );

   BVCE_UserData_P_Set16_LE(pFWPacketDescriptor, BVCE_FW_P_UserData_PacketDescriptor_Length_OFFSET, BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH);
   BVCE_UserData_P_Set16_LE(pFWPacketDescriptor, BVCE_FW_P_UserData_PacketDescriptor_Type_OFFSET, BVCE_P_UserData_PacketTypeLUT[pPacketDescriptor->ePacketFormat]);

   {
      unsigned uiNumValid608Lines = 0;
      unsigned uiNumValid708Lines = 0;
      signed iPreviousLineOffset = -1;

      for (i = 0; i < pPacketDescriptor->data.stDvs157.stCC.uiNumLines; i++ )
      {
         if ( true == pPacketDescriptor->data.stDvs157.stCC.astLine[i].bIsAnalog )
         {
            /* Process 608 Data */
            volatile uint8_t* p608Payload = (uint8_t*)((uint32_t) pCCPayload + BVCE_FW_P_UserData_CCPayload_Get608Line_OFFSET(uiNumValid608Lines));

            if ( uiNumValid608Lines > BVCE_FW_P_UserData_Payload_CC_608_LINES_PER_FIELD_MAX )
            {
               BDBG_ERR(("Error: Number of Valid 608 Lines (%d) is greater than max (%d). Dropping extra lines.", uiNumValid608Lines, BVCE_FW_P_UserData_Payload_CC_608_LINES_PER_FIELD_MAX));
               uiNumValid608Lines = BVCE_FW_P_UserData_Payload_CC_608_LINES_PER_FIELD_MAX;
            }

            p608Payload[0] = 0;
            iPreviousLineOffset = pPacketDescriptor->data.stDvs157.stCC.astLine[i].line_offset;

            p608Payload[0] &= ~BVCE_FW_P_UserData_Payload_CC_608Metadata_LineOffset_MASK;
            p608Payload[0] |= ( pPacketDescriptor->data.stDvs157.stCC.astLine[i].line_offset << BVCE_FW_P_UserData_Payload_CC_608Metadata_LineOffset_SHIFT ) & BVCE_FW_P_UserData_Payload_CC_608Metadata_LineOffset_MASK;

            p608Payload[0] &= ~BVCE_FW_P_UserData_Payload_CC_608Metadata_Priority_MASK;
            p608Payload[0] |= ( pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_priority << BVCE_FW_P_UserData_Payload_CC_608Metadata_Priority_SHIFT ) & BVCE_FW_P_UserData_Payload_CC_608Metadata_Priority_MASK;

            if ( true == pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_valid )
            {
               p608Payload[0] &= ~BVCE_FW_P_UserData_Payload_CC_608Metadata_Valid_MASK;
               p608Payload[0] |= ( 1 << BVCE_FW_P_UserData_Payload_CC_608Metadata_Valid_SHIFT ) & BVCE_FW_P_UserData_Payload_CC_608Metadata_Valid_MASK;
            }

            p608Payload[1] = pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_data_1;
            p608Payload[2] = pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_data_2;

            uiNumValid608Lines++;
         }
         else
         {
            /* Process 708 Data */
            volatile uint8_t* p708Payload = (uint8_t*)((uint32_t) pCCPayload + BVCE_FW_P_UserData_CCPayload_Get708Line_OFFSET(uiNumValid708Lines));

            if ( uiNumValid708Lines > BVCE_FW_P_UserData_Payload_CC_708_LINES_PER_FIELD_MAX )
            {
               BDBG_ERR(("Error: Number of Valid 708 Lines (%d) is greater than max (%d). Dropping extra lines.", uiNumValid708Lines, BVCE_FW_P_UserData_Payload_CC_708_LINES_PER_FIELD_MAX));
               uiNumValid708Lines = BVCE_FW_P_UserData_Payload_CC_708_LINES_PER_FIELD_MAX;
            }

            p708Payload[0] = 0;

            if ( true == pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_valid )
            {
               p708Payload[0] &= ~BVCE_FW_P_UserData_Payload_CC_708Metadata_Valid_MASK;
               p708Payload[0] |= ( 1 << BVCE_FW_P_UserData_Payload_CC_708Metadata_Valid_SHIFT ) & BVCE_FW_P_UserData_Payload_CC_708Metadata_Valid_MASK;
            }

            if ( pPacketDescriptor->data.stDvs157.stCC.astLine[i].seq.cc_type == 0x3 )
            {
               p708Payload[0] &= ~BVCE_FW_P_UserData_Payload_CC_708Metadata_PacketStart_MASK;
               p708Payload[0] |= ( 1 << BVCE_FW_P_UserData_Payload_CC_708Metadata_PacketStart_SHIFT ) & BVCE_FW_P_UserData_Payload_CC_708Metadata_PacketStart_MASK;
            }

            p708Payload[1] = pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_data_1;
            p708Payload[2] = pPacketDescriptor->data.stDvs157.stCC.astLine[i].cc_data_2;

            uiNumValid708Lines++;
         }
      }

      BVCE_UserData_P_Set8(pCCPayload, BVCE_FW_P_UserData_CCPayload_NumValid608Lines_OFFSET, uiNumValid608Lines);
      BVCE_UserData_P_Set8(pCCPayload, BVCE_FW_P_UserData_CCPayload_NumValid708Lines_OFFSET, uiNumValid708Lines);
   }

   BDBG_LEAVE( BVCE_UserData_P_ParsePacketDescriptor );
}

/*************/
/* User Data */
/*************/
/* BVCE_Channel_UserData_AddBuffers_isr - adds user data field info structs(s) to user data queue for stream insertion */
BERR_Code
BVCE_Channel_UserData_AddBuffers_isr(
         BVCE_Channel_Handle hVceCh,
         const BUDP_Encoder_FieldInfo *pstUserDataFieldInfo, /* Pointer to first field info descriptor */
         size_t uiCount, /* Count of user data field buffer info structs */
         size_t *puiQueuedCount /* Count of user data field info structs queued by encoder (*puiQueuedCount <= uiCount) */
         )
{
   BERR_Code rc = BERR_SUCCESS;
   volatile BVCE_FW_P_UserData_Queue stUserDataQueue;

   BDBG_ENTER( BVCE_Channel_UserData_AddBuffers_isr );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstUserDataFieldInfo );
   BDBG_ASSERT( puiQueuedCount );

   BKNI_Memset( (void*) &stUserDataQueue, 0, sizeof( BVCE_FW_P_UserData_Queue ) );

   *puiQueuedCount = 0;

   if ( BVCE_P_Status_eStarted == hVceCh->eStatus )
   {
      /* Get FW Queue Info */
      BVCE_P_ReadRegisters_isr(
               hVceCh->hVce,
               hVceCh->userdata.dccm.uiUserDataQueueInfoAddress,
               (uint32_t*) (&stUserDataQueue),
               sizeof( BVCE_FW_P_UserData_Queue )
               );

#if BVCE_P_DUMP_USERDATA_LOG
      if ( stUserDataQueue.uiReadOffset == ( ( stUserDataQueue.uiWriteOffset + 1 ) % BVCE_FW_P_USERDATA_QUEUE_LENGTH ) )
      {
         if ( NULL != hVceCh->userdata.hUserDataLog )
         {
            BVCE_Debug_P_WriteLog_isr(hVceCh->userdata.hUserDataLog, "%d,%d,%d\n",
               stUserDataQueue.uiReadOffset,
               stUserDataQueue.uiWriteOffset,
               pstUserDataFieldInfo->uiStgPictureId
            );
         }
      }
#endif

      while ( ( stUserDataQueue.uiReadOffset != ( ( stUserDataQueue.uiWriteOffset + 1 ) % BVCE_FW_P_USERDATA_QUEUE_LENGTH ) )
              && ( *puiQueuedCount < uiCount ) )
      {
         BVCE_FW_P_UserData_PacketType ePacketType;
         unsigned uiSourceDescNum = 0;
         unsigned uiTargetDescNum = 0;
         unsigned uiDescriptorOffset = ( stUserDataQueue.uiWriteOffset * BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH * BVCE_UserData_P_PacketDescriptor_MAX_PER_FIELD);
         unsigned uiBaseFWPacketDescriptorOffset = hVceCh->userdata.uiDescriptorBufferOffset + uiDescriptorOffset;
         void *pBaseFWPacketDescriptor =  (void*) ( (uint32_t) hVceCh->userdata.pDescriptorBufferCached + uiDescriptorOffset );

         BDBG_ASSERT( BVCE_UserData_P_PacketDescriptor_MAX_PER_FIELD >= pstUserDataFieldInfo->uiNumDescriptors );

         /* FW wants packets to be inserted in ascending packet type order, so for now, just do a brute force traverse of all the descriptors
          * process them in ascending packet type order
          */
         for ( ePacketType = 0; ePacketType < BVCE_FW_P_UserData_PacketType_eMax; ePacketType++ )
         {
            for ( uiSourceDescNum = 0; uiSourceDescNum < pstUserDataFieldInfo->uiNumDescriptors; uiSourceDescNum++ )
            {

               /* Only add packet types that FW understands */
               if ( ePacketType == BVCE_P_UserData_PacketTypeLUT[pstUserDataFieldInfo->stPacketDescriptor[uiSourceDescNum].ePacketFormat] )
               {
                  void *pFWPacketDescriptor = (void*) ( (uint32_t) pBaseFWPacketDescriptor + uiTargetDescNum*BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH );
                  const BUDP_Encoder_PacketDescriptor *pPacketDescriptor = &pstUserDataFieldInfo->stPacketDescriptor[uiSourceDescNum];

                  /* Populate BVCE_FW_P_UserData_PacketDescriptor */
                  BVCE_UserData_P_ParsePacketDescriptor( pPacketDescriptor, pFWPacketDescriptor );

                  uiTargetDescNum++;
               }
            }
         }

         BMEM_FlushCache(
            hVceCh->hVce->channel_memory.hMem[BVCE_P_HeapId_eSystem],
            pBaseFWPacketDescriptor,
            uiTargetDescNum*BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH
            );

         /* Populate BVCE_FW_P_UserData_QueueEntry */
         stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata = 0;

         stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata &= ~BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_MASK;
         stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata |= ( pstUserDataFieldInfo->uiStgPictureId << BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_SHIFT ) & BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_MASK;

         switch ( pstUserDataFieldInfo->ePolarity )
         {
            case BAVC_Polarity_eTopField:
               /* Do Nothing */
               break;

            case BAVC_Polarity_eBotField:
               stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata |= ( 1 << BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_SHIFT ) & BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_MASK;
               break;

            default:
               BDBG_ERR(("Unsupported polarity: %d", pstUserDataFieldInfo->ePolarity));
               return BERR_TRACE( BERR_INVALID_PARAMETER );
               break;
         }

         stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata |= ( ( BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH * uiTargetDescNum ) << BVCE_FW_P_UserData_QueueEntry_Metadata_Length_SHIFT ) & BVCE_FW_P_UserData_QueueEntry_Metadata_Length_MASK;

         stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiOffset = uiBaseFWPacketDescriptorOffset;

#if BVCE_P_DUMP_USERDATA_LOG
         /* Debug */
         if ( NULL != hVceCh->userdata.hUserDataLog )
         {
            unsigned i;

            for ( i = 0; i < uiTargetDescNum; i++ )
            {
               void *pFWPacketDescriptor = (void*) ( (uint32_t) pBaseFWPacketDescriptor + i*BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH );

               {
                  unsigned uiNum608Lines = ((uint8_t*) pFWPacketDescriptor)[BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_NumValid608Lines_OFFSET];
                  unsigned line;

                  for ( line = 0; line < uiNum608Lines; line++ )
                  {

                     BVCE_Debug_P_WriteLog_isr(hVceCh->userdata.hUserDataLog, "%u,%u,%u,%d,%d,%d,%u,%d,%d,%d,%d,%d,%d,%d,%d\n",
                        BREG_Read32(
                           hVceCh->hVce->handles.hReg,
                           hVceCh->hVce->stPlatformConfig.stDebug.uiSTC[hVceCh->stOpenSettings.uiInstance]),
                        stUserDataQueue.uiReadOffset,
                        stUserDataQueue.uiWriteOffset,
                        (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_SHIFT,
                        (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_SHIFT,
                        (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_Length_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_Length_SHIFT,
                        stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiOffset,
                        i,
                        BVCE_UserData_P_Get16_LE( pFWPacketDescriptor, BVCE_FW_P_UserData_PacketDescriptor_Length_OFFSET ),
                        BVCE_UserData_P_Get16_LE( pFWPacketDescriptor, BVCE_FW_P_UserData_PacketDescriptor_Type_OFFSET ),
                        true,
                        uiNum608Lines,
                        ((uint8_t*) pFWPacketDescriptor)[0 + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_Get608Line_OFFSET(line)],
                        ((uint8_t*) pFWPacketDescriptor)[1 + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_Get608Line_OFFSET(line)],
                        ((uint8_t*) pFWPacketDescriptor)[2 + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_Get608Line_OFFSET(line)]
                     );
                  }
               }

               {
                  unsigned uiNum708Lines = ((uint8_t*) pFWPacketDescriptor)[BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_NumValid708Lines_OFFSET];
                  unsigned line;

                  for ( line = 0; line < uiNum708Lines; line++ )
                  {

                     BVCE_Debug_P_WriteLog_isr(hVceCh->userdata.hUserDataLog, "%u,%u,%u,%d,%d,%d,%u,%d,%d,%d,%d,%d,%d,%d,%d\n",
                        BREG_Read32(
                           hVceCh->hVce->handles.hReg,
                           hVceCh->hVce->stPlatformConfig.stDebug.uiSTC[hVceCh->stOpenSettings.uiInstance]),
                        stUserDataQueue.uiReadOffset,
                        stUserDataQueue.uiWriteOffset,
                        (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_SHIFT,
                        (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_SHIFT,
                        (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_Length_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_Length_SHIFT,
                        stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiOffset,
                        i,
                        BVCE_UserData_P_Get16_LE( pFWPacketDescriptor, BVCE_FW_P_UserData_PacketDescriptor_Length_OFFSET ),
                        BVCE_UserData_P_Get16_LE( pFWPacketDescriptor, BVCE_FW_P_UserData_PacketDescriptor_Type_OFFSET ),
                        false,
                        uiNum708Lines,
                        ((uint8_t*) pFWPacketDescriptor)[0 + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_Get708Line_OFFSET(line)],
                        ((uint8_t*) pFWPacketDescriptor)[1 + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_Get708Line_OFFSET(line)],
                        ((uint8_t*) pFWPacketDescriptor)[2 + BVCE_FW_P_UserData_PacketDescriptor_CCPayload_OFFSET + BVCE_FW_P_UserData_CCPayload_Get708Line_OFFSET(line)]
                     );
                  }
               }
            }

            if ( 0 == uiTargetDescNum )
            {
               BVCE_Debug_P_WriteLog_isr(hVceCh->userdata.hUserDataLog, "%u,%u,%u,%d,%d,%d,%u,-,-,-,-,-,-,-,-\n",
                  BREG_Read32(
                     hVceCh->hVce->handles.hReg,
                     hVceCh->hVce->stPlatformConfig.stDebug.uiSTC[hVceCh->stOpenSettings.uiInstance]),
                  stUserDataQueue.uiReadOffset,
                  stUserDataQueue.uiWriteOffset,
                  (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_SHIFT,
                  (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_SHIFT,
                  (stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiMetadata & BVCE_FW_P_UserData_QueueEntry_Metadata_Length_MASK) >> BVCE_FW_P_UserData_QueueEntry_Metadata_Length_SHIFT,
                  stUserDataQueue.astQueue[stUserDataQueue.uiWriteOffset].uiOffset
                  );
            }
         }
#endif

         /* Update *puiQueuedCount */
         (*puiQueuedCount)++;
         stUserDataQueue.uiWriteOffset = ( stUserDataQueue.uiWriteOffset + 1 ) % BVCE_FW_P_USERDATA_QUEUE_LENGTH;
         hVceCh->userdata.uiQueuedBuffers++;

         pstUserDataFieldInfo = BUDP_ENCODER_FIELDINFO_GET_NEXT (pstUserDataFieldInfo);
      }

      if ( 0 != *puiQueuedCount )
      {
         /* Update FW Queue Info */
         /* Update Queue First */
         unsigned uiWriteOffset = ( (uint32_t) (stUserDataQueue.astQueue) - (uint32_t) (&stUserDataQueue.uiReadOffset) );
         /* coverity[address_of] */
         /* coverity[callee_ptr_arith] */
         BVCE_P_WriteRegisters_isr(
                  hVceCh->hVce,
                  hVceCh->userdata.dccm.uiUserDataQueueInfoAddress + uiWriteOffset,
                  (uint32_t*) (stUserDataQueue.astQueue),
                  sizeof( BVCE_FW_P_UserData_Queue ) - uiWriteOffset
                  );

         /* Update Write Offset AFTER queue is updated */
         uiWriteOffset = ( (uint32_t) (&stUserDataQueue.uiWriteOffset) - (uint32_t) (&stUserDataQueue.uiReadOffset) );
         /* coverity[address_of] */
         /* coverity[callee_ptr_arith] */
         BVCE_P_WriteRegisters_isr(
                  hVceCh->hVce,
                  hVceCh->userdata.dccm.uiUserDataQueueInfoAddress + uiWriteOffset,
                  (uint32_t*) (&stUserDataQueue.uiWriteOffset),
                  sizeof( stUserDataQueue.uiWriteOffset )
                  );
      }
   }
   else
   {
      BDBG_MODULE_WRN( BVCE_USERDATA, ("Cannot queue user data when channel is not started") );
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BDBG_LEAVE( BVCE_Channel_UserData_AddBuffers_isr );
   return BERR_TRACE( rc );
}


BERR_Code
BVCE_Channel_UserData_GetStatus_isr(
      BVCE_Channel_Handle hVceCh,
      BAVC_VideoUserDataStatus *pstUserDataStatus
      )
{
   BERR_Code rc = BERR_SUCCESS;
   BVCE_FW_P_UserData_Queue stUserDataQueue;

   BDBG_ENTER( BVCE_Channel_UserData_GetStatus_isr );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstUserDataStatus );

   BKNI_Memset( pstUserDataStatus, 0, sizeof( BAVC_VideoUserDataStatus ) );

   if ( BVCE_P_Status_eStarted == hVceCh->eStatus )
   {
      BKNI_Memset( &stUserDataQueue, 0, sizeof( BVCE_FW_P_UserData_Queue ) );

      /* Get FW Queue Info */
      BVCE_P_ReadRegisters_isr(
               hVceCh->hVce,
               hVceCh->userdata.dccm.uiUserDataQueueInfoAddress,
               (uint32_t*) (&stUserDataQueue),
               sizeof( BVCE_FW_P_UserData_Queue )
               );

      if ( stUserDataQueue.uiReadOffset < stUserDataQueue.uiWriteOffset )
      {
         pstUserDataStatus->uiPendingBuffers = stUserDataQueue.uiWriteOffset - stUserDataQueue.uiReadOffset;
      }
      else
      {
         pstUserDataStatus->uiPendingBuffers = BVCE_FW_P_USERDATA_QUEUE_LENGTH - stUserDataQueue.uiReadOffset;
         pstUserDataStatus->uiPendingBuffers += stUserDataQueue.uiWriteOffset;
      }

      pstUserDataStatus->uiCompletedBuffers = hVceCh->userdata.uiQueuedBuffers - pstUserDataStatus->uiPendingBuffers;
      hVceCh->userdata.uiQueuedBuffers = pstUserDataStatus->uiPendingBuffers;
   }
   else
   {
      BDBG_MODULE_WRN( BVCE_USERDATA, ("Cannot get user data status when channel is not started") );
      rc = BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BDBG_LEAVE( BVCE_Channel_UserData_GetStatus_isr );
   return BERR_TRACE( rc );
}

/********************/
/* Helper Functions */
/********************/
BERR_Code
BVCE_GetA2PDelayInfo(
         const BVCE_Channel_EncodeSettings *pstChEncodeSettings,
         const BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings,
         BVCE_A2PDelay *pstA2PDelay
         )
{
   BDBG_ENTER( BVCE_GetA2PDelayInfo );

   BDBG_ASSERT( pstChEncodeSettings );
   BDBG_ASSERT( pstChStartEncodeSettings );
   BDBG_ASSERT( BVCE_P_SIGNATURE_STARTENCODESETTINGS == pstChStartEncodeSettings->uiSignature );
   BDBG_ASSERT( BVCE_P_SIGNATURE_CHANNELENCODESETTINGS == pstChEncodeSettings->uiSignature );
   BDBG_ASSERT( pstA2PDelay );

   if ( NULL != pstA2PDelay )
   {
      BKNI_Memset( pstA2PDelay, 0, sizeof( BVCE_A2PDelay ) );

      if ( ( NULL != pstChEncodeSettings )
           && ( NULL != pstChStartEncodeSettings )
         )
      {
         uint32_t uiProtocol;
         uint32_t uiProfile;
         uint32_t uiLevel;
         uint32_t uiA2PDelayMin;
         uint32_t uiA2PDelayMax;
         uint32_t uiGOPStructure;

         uiProtocol = BVCE_P_ProtocolLUT[pstChStartEncodeSettings->stProtocolInfo.eProtocol];

         switch ( uiProtocol )
         {
            case ENCODING_STD_H264:
               uiProfile = BVCE_P_ProfileH264LUT[pstChStartEncodeSettings->stProtocolInfo.eProfile];
               uiLevel = BVCE_P_LevelH264LUT[pstChStartEncodeSettings->stProtocolInfo.eLevel];
               break;

            case ENCODING_STD_MPEG2:
               uiProfile = BVCE_P_ProfileMPEG2LUT[pstChStartEncodeSettings->stProtocolInfo.eProfile];
               uiLevel = BVCE_P_LevelMPEG2LUT[pstChStartEncodeSettings->stProtocolInfo.eLevel];
               break;

            case ENCODING_STD_MPEG4:
               uiProfile = BVCE_P_ProfileMPEG4LUT[pstChStartEncodeSettings->stProtocolInfo.eProfile];
               uiLevel = BVCE_P_LevelMPEG4LUT[pstChStartEncodeSettings->stProtocolInfo.eLevel];
               break;

            case BVCE_P_VIDEOCOMPRESSIONSTD_UNSUPPORTED:
            default:
               BDBG_ERR(("Unsupported video compression protocol (%d)", pstChStartEncodeSettings->stProtocolInfo.eProtocol));
               return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         if ( BVCE_P_VIDEOCOMPRESSIONPROFILE_UNSUPPORTED == uiProfile )
         {
            BDBG_ERR(("Unsupported video compression profile (%d)", pstChStartEncodeSettings->stProtocolInfo.eProfile));
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         if ( BVCE_P_VIDEOCOMPRESSIONLEVEL_UNSUPPORTED == uiLevel )
         {
            BDBG_ERR(("Unsupported video compression level (%d)", pstChStartEncodeSettings->stProtocolInfo.eLevel));
            return BERR_TRACE( BERR_INVALID_PARAMETER );
         }

         {
            unsigned uiGOPLength = 0;

            BVCE_P_GOPStructureLUT(
               &pstChStartEncodeSettings->stBounds.stGOPStructure,
               &uiGOPStructure,
               &uiGOPLength
               );

            uiGOPStructure &= GOP_STRUCTURE_MASK;
         }

         uiA2PDelayMin = BVCE_FW_P_CalcVideoA2Pdelay(
            &uiA2PDelayMax,
                  uiProtocol,
                  uiProfile,
                  uiLevel,
                  BVCE_P_FrameRateLUT[pstChEncodeSettings->stFrameRate.eFrameRate],
                  pstChEncodeSettings->stBitRate.uiMax,
            BVCE_P_EncodeModeLUT( pstChStartEncodeSettings ),
            pstChStartEncodeSettings->uiRateBufferDelay,
            BVCE_P_FrameRateLUT[pstChStartEncodeSettings->stBounds.stFrameRate.eMin],
            BVCE_P_FrameRateLUT[pstChStartEncodeSettings->stBounds.stInputFrameRate.eMin],
            BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS,
            pstChEncodeSettings->bITFPEnable,
            BVCE_P_InputTypeLUT[pstChStartEncodeSettings->eInputType],
            uiGOPStructure,
            pstChStartEncodeSettings->stBounds.stDimensions.stMax.uiWidth,
            pstChStartEncodeSettings->stBounds.stDimensions.stMax.uiHeight,
            pstChStartEncodeSettings->uiNumParallelNRTEncodes
         );

         pstA2PDelay->uiMin = uiA2PDelayMin;
         pstA2PDelay->uiMax = uiA2PDelayMax;
      }
   }

   BDBG_LEAVE( BVCE_GetA2PDelayInfo );
   return BERR_TRACE( BERR_SUCCESS );
}

/* Power Management */
BERR_Code
BVCE_Power_Standby(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;
   unsigned uiChannelNum = 0;

   BDBG_ENTER( BVCE_Power_Standby );

   /* Verify that we are not already in a standby state */
   if ( 0 == BVCE_Power_P_QueryResource( hVce, BVCE_Power_Type_ePower ) )
   {
      BDBG_ERR(("Already in standby!"));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BVCE_Power_P_AcquireResource(
         hVce,
         BVCE_Power_Type_eClock
         );

   /* Verify that all channels are stopped.  Keep track of channels that are open */
   for ( uiChannelNum = 0; uiChannelNum < BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS; uiChannelNum++ )
   {
      switch( hVce->channels[uiChannelNum].context.eStatus )
      {
         case BVCE_P_Status_eStarted:
         case BVCE_P_Status_eStopping:
            BDBG_ERR(("BVCE_Power_Standby called while channel[%d] is still actively encoding", uiChannelNum));
            return BERR_TRACE( BERR_INVALID_PARAMETER );

         case BVCE_P_Status_eOpened:
            hVce->channels[uiChannelNum].bResume = true;
            BVCE_Channel_Close( &hVce->channels[uiChannelNum].context );
            break;

         case BVCE_P_Status_eIdle:
            hVce->channels[uiChannelNum].bResume = false;
            break;

         default:
            BDBG_ERR(("Unknown channel state (%d)", hVce->channels[uiChannelNum].context.eStatus));
            BDBG_ASSERT(0);
      }
   }

   /* Shutdown the encoder gracefully */
#if BVCE_P_ENABLE_WATCHDOG
   rc = BERR_TRACE( BVCE_P_DisableWatchdog(
            hVce
            ));
#endif

   rc = BERR_TRACE( BVCE_P_DisableInterrupts(
               hVce
               ));

   rc = BERR_TRACE( BVCE_P_Reset(
            hVce
            ));

   rc = BERR_TRACE( BVCE_P_TeardownInterrupts(
               hVce
               ));

   /* Power down the encoder */
   BVCE_Power_P_ReleaseAllResources(
         hVce
         );

   BDBG_LEAVE( BVCE_Power_Standby );

   return BERR_TRACE( rc );
}

BERR_Code
BVCE_Power_Resume(
         BVCE_Handle hVce
         )
{
   BERR_Code rc = BERR_SUCCESS;
   unsigned uiChannelNum = 0;

   BDBG_ENTER( BVCE_Power_Resume );

   /* Verify that we in a standby state */
   if ( 0 != BVCE_Power_P_QueryResource( hVce, BVCE_Power_Type_ePower ) )
   {
      BDBG_ERR(("Not in standby!"));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   /* Power up the encoder */
   BVCE_Power_P_AcquireResource(
         hVce,
         BVCE_Power_Type_ePower
         );

   BVCE_Power_P_AcquireResource(
         hVce,
         BVCE_Power_Type_eClock
         );

   /* Startup the encoder */
   rc = BVCE_P_Reset(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_LoadFirmware(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_SetupInterrupts(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_EnableInterrupts(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

#if BVCE_P_ENABLE_WATCHDOG
   rc = BVCE_P_EnableWatchdog(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }
#endif

   rc = BVCE_P_Boot(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   /* Send Init Command */
   rc = BVCE_P_SendCommand_Init(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   rc = BVCE_P_SetupDebugLog(
            hVce
            );
   if ( BERR_SUCCESS != rc )
   {
      return BERR_TRACE( rc );
   }

   BVCE_Power_P_ReleaseResource(
      hVce,
      BVCE_Power_Type_eClock
      );

   /* Re-Open any channels that were open when standby was called */
   for ( uiChannelNum = 0; uiChannelNum < BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS; uiChannelNum++ )
   {
      if ( true == hVce->channels[uiChannelNum].bResume )
      {
         BVCE_Channel_Handle hVceCh;
         BVCE_Channel_OpenSettings stChannelOpenSettings = hVce->channels[uiChannelNum].context.stOpenSettings;

         BVCE_Channel_Open(
            hVce,
            &hVceCh,
            &stChannelOpenSettings
            );

         hVce->channels[uiChannelNum].bResume = false;
      }
   }

   BDBG_LEAVE( BVCE_Power_Resume );

   return BERR_TRACE( rc );
}

BVCE_P_Status
BVCE_Channel_P_GetState(
   BVCE_Channel_Handle hVceCh
   )
{
   BDBG_ENTER( BVCE_Channel_P_GetState );

   BDBG_ASSERT( hVceCh );

   BDBG_LEAVE( BVCE_Channel_P_GetState );
   return hVceCh->eStatus;
}

void
BVCE_Channel_P_GetStartEncodeSettings(
   BVCE_Channel_Handle hVceCh,
   BVCE_Channel_StartEncodeSettings *pstStartEncodeSettings
   )
{
   BDBG_ENTER( BVCE_Channel_P_GetStartEncodeSettings );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstStartEncodeSettings );

   *pstStartEncodeSettings = hVceCh->stStartEncodeSettings;

   BDBG_LEAVE( BVCE_Channel_P_GetStartEncodeSettings );
}

void
BVCE_Channel_P_GetEncodeSettings(
   BVCE_Channel_Handle hVceCh,
   BVCE_Channel_EncodeSettings *pstEncodeSettings
   )
{
   BDBG_ENTER( BVCE_Channel_P_GetEncodeSettings );

   BDBG_ASSERT( hVceCh );
   BDBG_ASSERT( pstEncodeSettings );

   *pstEncodeSettings = hVceCh->stEncodeSettings;

   BDBG_LEAVE( BVCE_Channel_P_GetEncodeSettings );
}

void
BVCE_Channel_P_HandleEOSEvent(
   BVCE_Channel_Handle hVceCh
   )
{
   BDBG_ENTER( BVCE_Channel_P_HandleEOSEvent );

   /* SW7425-4186: Set EOS event when EOS ITB entry is seen */
   hVceCh->stStatus.uiEventFlags |= BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS;

   if ( ( BVCE_P_Status_eStarted == hVceCh->eStatus )
        || ( BVCE_P_Status_eStopping == hVceCh->eStatus ) )
   {
      BVCE_Power_P_ReleaseResource(
            hVceCh->hVce,
            BVCE_Power_Type_eClock
            );

      hVceCh->eStatus = BVCE_P_Status_eOpened;
   }

   /* Detach from output */
   hVceCh->stStartEncodeSettings.hOutputHandle = NULL;

   BDBG_LEAVE( BVCE_Channel_P_HandleEOSEvent );
}


