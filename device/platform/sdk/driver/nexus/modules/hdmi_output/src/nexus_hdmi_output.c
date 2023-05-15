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
 * $brcm_Workfile: nexus_hdmi_output.c $
 * $brcm_Revision: 134 $
 * $brcm_Date: 10/24/12 11:49a $
 *
 * Module Description:
 *                      HdmiOutput: Specific interfaces for an HDMI/DVI output.
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/src/nexus_hdmi_output.c $
 * 
 * 134   10/24/12 11:49a rgreen
 * SW7435-441:  Set connectTimer to null in timer expired function
 *  NEXUS_HdmiOutput_P_RxConnectedTimer
 * 
 * 133   10/19/12 2:19p jgarrett
 * SW7358-417: Merge to main branch
 * 
 * SW7358-417/3   10/18/12 3:02p mpeteri
 * SW7358-417: update from main
 * 
 * 132   10/19/12 10:17a rgreen
 * SW7405-5557: Downgrade WRN message
 * 
 * 131   10/12/12 3:12p erickson
 * SW7435-419: change proc to BDBG_LOG to that they are still available
 *  with B_REFSW_DEBUG_LEVEL=err
 * 
 * 130   10/10/12 11:42a rgreen
 * SW7346-1047: Merge changes; enable TMDS only HDMI output is connected
 *  to display
 * 
 * SW7346-1047/4   10/10/12 9:54a rgreen
 * SW7346-1047: Remove nexusTmdsControl initialization
 * 
 * SW7346-1047/3   10/5/12 4:14p rgreen
 * SW7346-1047: Enable TMDS if HDMI Output is connected to display.  For
 *  GetStatus, check if the Rx is attached first.
 * 
 * SW7346-1047/2   9/27/12 5:09p rgreen
 * SW7346-1047: Add setting to control TMDS lines in app or nexus
 * 
 * SW7346-1047/1   9/26/12 5:17p rgreen
 * SW7346-1047: Add setting to control TMDS lines in app or nexus
 * 
 * 129   10/9/12 5:40p erickson
 * SWDEPRECATED-3811: fix proc BDBG_MSG
 * 
 * 128   10/5/12 2:38p rgreen
 * SW7425-3907: Remove error trace for not detecting RxSense
 * 
 * SW7358-417/2   10/10/12 11:08a mpeteri
 * SW7358-417: add changed settings detection for audio burst type
 * 
 * SW7358-417/1   10/3/12 12:21p mpeteri
 * SW7358-417: Changes to allow NEXUS to configure HDMI audio burst
 *  settings.
 * 
 * 127   9/25/12 6:28p rgreen
 * SW7425-3907: Report Rx may be powered down message on successful return
 *  of BHDM_GetReceiverSense
 * 
 * 126   9/13/12 2:14p rgreen
 * SW7425-3904: Merge changes Remove debug message
 * 
 * 125   9/13/12 1:37p rgreen
 * SW7425-3904: Merge changes
 * 
 * SW7425-3904/1   9/10/12 6:13p rgreen
 * SW7425-3904: Make RxSense retry and polling interval configurable
 * 
 * 124   8/15/12 3:03p rgreen
 * SW7425-3733: fix coverity issue; check all BHDM_RxDeviceAttached return
 *  values.
 * 
 * 123   8/2/12 6:52p rgreen
 * SW7358-361: Fix selection of SD colorimetry for user specified
 *  colorspace
 * 
 * 122   7/26/12 12:03p jtna
 * SW7425-3604: don't cancel already cancelled timers
 * 
 * 121   7/12/12 1:24p rgreen
 * SW7425-2989:Merge Changes
 * 
 * SW7425-2989/1   7/6/12 2:17p rgreen
 * SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
 * 
 * 120   7/3/12 12:00p jgarrett
 * SW7346-893: Enabling dither by default
 * 
 * 119   7/3/12 10:59a rgreen
 * SW7346-849:  Report additional status (from EDID) even if the Rx Device
 *  is not powered
 * 
 * 118   6/21/12 3:05p rgreen
 * SW7358-263, SW7425-3206: Shutdown all timers for when entering standby
 * 
 * 117   6/20/12 2:35p rgreen
 * SW7358-263, SW7425-3206: Use polling to detect RxSense state instead of
 *  interrupts
 * 
 * 116   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 115   6/18/12 1:35p rgreen
 * SW7425-3265: Read EDID after Hot Plug rather than after Rx is powered.
 *  EDID is available after Hot Plug connect event
 * 
 * 114   6/13/12 1:45p gmohile
 * SW7425-3241 : Remove check for connected state during standby/resume
 * 
 * 113   6/8/12 2:41p rgreen
 * SW7425-3159,SW7346-849: Merge changes
 * 
 * SW7346-849/2   6/8/12 2:01p rgreen
 * SW7425-3159,SW7346-849: Remove incorrect comment
 * 
 * SW7346-849/1   6/4/12 5:55p rgreen
 * SW7425-3159,SW7346-849:  Add rxPowered to HdmiOutputStatus
 * 
 * 112   6/1/12 5:26p rgreen
 * SW7425-3149: restore prior version
 * 
 * 111   5/31/12 10:47a rgreen
 * SW7425-3149: Enable TMDS lines in one location
 * 
 * 110   5/24/12 3:57p rgreen
 * SW7418-98,SW7231-797:  BHDM_DisableDisplay (disable TMDS Clock + Data)
 *  is only called when device is removed from the nexus display.  TMDS
 *  (data) lines are disabled when Rx Device is removed from the cable
 * 
 * 109   5/23/12 2:01p rgreen
 * SW7335-1383: Report Rx powered down as a debug warning
 * 
 * 108   5/23/12 11:59a vle
 * SW7335-1383: Fix incorrect power state when AddOutput after
 *  RemoveOutput.
 * 
 * 107   5/10/12 7:05p rgreen
 * SW7422-468,SW7552-163,SW7425-2075: Fix bug with hotplug state variable;
 *  report connected = true even if device is not powered since the HDMI
 *  Output  may have just been added to the display  Remove output if
 *  RxDevice has been disconnected while waiting to connect
 * 
 * 106   5/7/12 7:32p rgreen
 * SW7422-468,SW7552-163: enable TMDS lines  based on whether or not HDMI
 *  video (output) is connected to the Display.
 * 
 * 105   4/27/12 2:13p rgreen
 * SW7422-468,SW7552-163: enable TMDS lines  based on whether or not HDMI
 *  video (output) is connected to the Display.
 * 
 * 104   4/25/12 12:00p rgreen
 * SW7422-468,SW7552-163: Use correct setting to enable/disable TMDS lines
 *  during Nexus_HdmiOutput_P_GetState
 * 
 * 103   4/18/12 11:28a rgreen
 * SW7552-163: Merge changes for variable timer to detect RxSense
 * 
 * SW7552-163/2   4/16/12 2:47p mward
 * SW7552-163: Merge from main
 * 
 * 102   4/17/12 11:50a erickson
 * SW7346-737: enable HDMI CRC feature
 * 
 * 101   4/16/12 3:46p jgarrett
 * SW7425-2075: Adding MLP audio
 * 
 * 100   4/13/12 5:01p erickson
 * SW7346-737: stub out feature for now
 * 
 * 99   4/13/12 4:58p erickson
 * SW7346-737: add NEXUS_HdmiOutput_GetCrcData
 * 
 * SW7552-163/1   4/9/12 7:13p rgreen
 * SW7552-163: Implement variable timer for checking Receiver Sense
 *  (RxSense) power after a hot plug connect event
 * 
 * 98   4/2/12 2:08p vle
 * SW7420-2281: Reset aspectRatioChangeOnly flag appropriately to avoid
 *  possible pink screen after hot plug.
 * 
 * 97   3/27/12 3:20p rgreen
 * SW7346-740: Add delay between EDID read attempts after failure
 * 
 * 96   3/19/12 11:35a rgreen
 * SW7425-2650: Fix memory leak in HDM PI; Delcare/store supported Video
 *  ID Codes in hdm handlle vs malloc to build each time
 * 
 * 95   3/14/12 3:22p rgreen
 * SW7401-4417: Remove 130nm support
 * 
 * 94   2/29/12 1:54p rgreen
 * SW7425-2515: Remove CEC references from hdmi_output module; CEC code
 *  now  located in modules/cec
 * 
 * 93   2/28/12 2:57p rgreen
 * SW7420-2126: Fix incorrect reported Sample Rate  in
 *  NEXUS_HdmiOutput_GetStatus.  It should be based on internal setting;
 *  not Audio InfoFrame
 * 
 * 92   2/27/12 4:58p rgreen
 * SW7425-2498: Do not report Hdmi Output status if un-initialized
 * 
 * 91   2/23/12 10:49a rgreen
 * SW7125-1146,SW7408-317: Merge changes
 * 
 * SW7408-317/3   2/22/12 4:57p rgreen
 * SW7125-1146,SW7408-317: Update Hotplug callback; handle non-forced
 *  removal of Rx device
 * 
 * SW7408-317/2   2/22/12 1:14p rgreen
 * SW7125-1146,SW7408-317: Remove enabling of TMDS at hotplug interrupt,
 *  let higher level enable TMDS when ready; use _isr designation for isr
 *  variables
 * 
 * SW7408-317/1   2/21/12 6:33p rgreen
 * SW7125-1146,SW7408-317: Treat RxSense and HP events separately.  Update
 *  processing of both events
 * 
 * 90   2/9/12 1:24p jgarrett
 * SW7425-2074: Supporting 7.1 PCM
 * 
 * 89   2/3/12 1:12p rgreen
 * SW7125-1146,SW7231-345: For HdmiOuput_GetStatus return a status of not
 *  connected if the attached Rx is connected but not powered.
 * 
 * 88   2/1/12 2:06p rgreen
 * SW7125-1146,SW7231-345: Do not error out of HdmiOutput_GetStatus if
 *  unable to get monitor range;  report error trace and keep going; clean
 *  code and update comments
 * 
 * 87   1/23/12 11:41a rgreen
 * SW7125-1146: Merge Changes
 * 
 * SW7125-1146/1   1/19/12 2:35p rgreen
 * SW7125-1146:  Use  BHDM_RxDeviceAttached  vs state variable to
 *  determine if Rx is attached;
 * 
 * 86   1/12/12 4:04p mward
 * SW7425-1140:  Close BCEC before BHDM.  Otherwise, registers needed by
 *  CEC close may be powered down and bus error.
 * 
 * 85   1/12/12 11:24a gmohile
 * SW7425-1708 : S3 warm boot support
 * 
 * 84   1/4/12 4:01p rgreen
 * SW7405-5004: Use BAVC_HDMI_xxx vs BHDM_xxx  macros
 * 
 * 83   12/19/11 12:34p vle
 * SW7425-1140: Fix build issue without CEC support. CEC functionalities
 *  in NEXUS_HDMI_OUTPUT modules is enabled with build option
 *  BHDM_CEC_SUPPORT=y
 * 
 * 82   12/16/11 6:01p vle
 * SW7425-1140: Disable CEC functionalities in NEXUS_HDMI_OUTPUT modules.
 * 
 * 81   12/2/11 6:12p rgreen
 * SW7420-2126: Fix incorrect reported Audio Format in
 *  NEXUS_HdmiOutput_GetStatus
 * 
 * 80   12/2/11 4:54p jtna
 * SW7425-1708: propagate NEXUS_StandbySettings.wakeupSettings.cec ->
 *  BHDM_StandbySettings.bEnableWakeup
 * 
 * 79   11/22/11 6:11p vle
 * SW7425-1140: Update nexus hdmi_output cec to use new BCEC PI instead of
 *  BHDM_CEC
 * 
 * SW7425-1140/2   11/22/11 1:59p vle
 * SW7425-1140: fix build errors when build without CEC support.
 * 
 * SW7425-1140/1   11/16/11 12:19p vle
 * SW7425-1140: Update to use new CEC PI instead of BHDM_CEC
 * 
 * 78   10/12/11 4:34p rgreen
 * SW7400-3038: Use go to err_cec vs err_hdcp label on detection of CEC
 *  error.
 * 
 * 77   9/15/11 4:27p rgreen
 * SW7425-1296: Get SPD InfoFrame default settings from portinginterface
 *  inital default settings.
 * 
 * 76   9/6/11 4:33p vle
 * SW7335-1331: Fix coverity issue.
 * 
 * 75   7/29/11 2:42p vle
 * SW7405-5440: Do not force VGA over HDMI
 * 
 * 74   7/28/11 3:37p erickson
 * SW7346-319: merge
 * 
 * SW7346-319/1   7/26/11 6:55p vle
 * SW7346-319: Make sure to set HDMI data transfer mode even when
 *  disconnected.
 * 
 * 73   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 * 
 * SW7125-1014/1   7/20/11 4:37p gmohile
 * SW7125-1014 : Rework power management
 * 
 * 72   5/20/11 4:28p vle
 * SW7125-927: Add a check to only re-enable HDCP is HDMI is connected.
 * 
 * 71   5/20/11 4:12p vle
 * SW7550-696: Only reset CEC logical address when disconnecting
 * 
 * 70   5/6/11 4:13p jgarrett
 * SW7346-191: Making AC3 implicit with AC3+ support.  Same for DTS and
 *  DTS-HD.  Denon receivers report only DTS-HD but can decode both
 * 
 * 69   4/29/11 12:07p erickson
 * SW7125-798: revise NEXUS_HdmiOutput_SetPreEmphasisConfiguration
 * 
 * 68   4/21/11 5:22p rgreen
 * SW7425-365:  Fix incorrect mapping between BAVC_AudioBits and
 *  SampleBits contained in AudioInfoFrame
 * 
 * 67   4/20/11 7:19p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 66   2/25/11 2:52p gmohile
 * SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
 * 
 * 65   2/18/11 4:49p gmohile
 * SW7408-210 : Force disconnect not required in standby
 * 
 * 64   2/17/11 7:44p jtna
 * SW7420-1141: rework HDMI power management
 * 
 * 63   2/17/11 5:52p vle
 * SW7420-1289: Merge to mainline
 * 
 * SW7420-1289/2   2/17/11 10:03a vle
 * SW7420-1289: Add NEXUS APIs to get/set HDMI AVI InfoFrame, HDMI Audio
 *  Infoframe. Share structures for hdmi_input and hdmi_output
 * 
 * SW7420-1289/1   1/27/11 2:35p vle
 * SW7420-1289: Add NEXUS APIs to get/set HDMI AVI InfoFrame, HDMI Audio
 *  Infoframe
 * 
 * 62   2/15/11 5:33p gmohile
 * SW7408-210 : Merge initial version of module standby
 * 
 * 61   1/14/11 6:27p vle
 * SW7405-5062: Merge to mainline
 * 
 * SW7405-5062/1   12/16/10 3:09p vle
 * SW7405-5062: Make sure all hotplug event (disconnect or connect) get
 *  served correctly.
 * 
 * 60   12/3/10 9:27a erickson
 * SW7550-577: call BHDM_SetHdmiDataTransferMode based on
 *  NEXUS_DisplayTimingGenerator_eHdmiDvo
 * 
 * 59   11/11/10 4:57p vle
 * SW7420-1084, SW7400-2956: Merge to main branch
 * 
 * SW7420-184__SW7400-2956/2   11/8/10 2:01p vle
 * SW7420-1084, SW7400-2956: expose auto lipsync info and monitor range
 *  info from nexus to application. Take 2
 * 
 * SW7420-184__SW7400-2956/1   11/5/10 12:23p vle
 * SW7420-1084, SW7400-2956: expose auto lipsync info and monitor range
 *  info from nexus to application
 * 
 * 58   11/5/10 5:40p jgarrett
 * SW7125-698: Applying HDMI channel status information when it updates
 * 
 * 57   10/22/10 11:47a vle
 * SW7422-23: Fix nexus build errors
 * 
 * 56   10/14/10 5:26p rgreen
 * SW7125-659: streamline calls to BHDM_EDID_GetSupportedColorimetry
 * 
 * 55   10/8/10 11:57a vle
 * SW7405-4782: Merge to main branch
 * 
 * 54   9/22/10 6:58p vle
 * SW7405-4782: merge to main branch
 * 
 * SW7405-4782/3   10/7/10 12:04p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication - TAKE 2. First implementation attempt has a potential
 *  race condition.
 *
 * 54   9/22/10 6:58p vle
 * SW7405-4782: merge to main branch
 * 
 * SW7405-4782/2   9/20/10 2:59p vle
 * SW7405-4782: Make sure at least the first BHDM_EnableDisplay is
 *  executed.
 * 
 * SW7405-4782/1   9/10/10 8:50p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication.
 * 
 * 53   6/15/10 11:14a jtna
 * SW7401-4412: unwind in proper sequence. this also fixes a benign
 *  compiler warning on 7401/7403
 * 
 * 52   6/4/10 6:09p vle
 * SW7405-3994: Merge to main branch
 * 
 * SW7405-3994/3   6/4/10 2:34p vle
 * SW7405-3994: Add HDMI 3D Structures defines in NEXUS
 * 
 * SW7405-3994/2   5/14/10 6:12p vle
 * SW7405-3994: Add support to check for supported 3D formats.
 * 
 * 51   5/12/10 4:50p erickson
 * SW7405-3994: merge
 *
 * SW7405-3994/1   5/11/10 7:11p vle
 * SW7405-3994: report supported 3D formats to NEXUS.
 * 
 * 50   4/27/10 7:11p mward
 * SW7125-376: Eliminate warning for unused function.
 * 
 * 49   4/23/10 9:50a erickson
 * SW7420-676: merge
 *
 * SW7420-676/2   4/22/10 4:54p vle
 * SW7420-676: Fix incorrect programming in return size.
 *
 * SW7420-676/1   4/21/10 2:27p vle
 * SW7420-676: Add API to return supported video info
 *
 * 48   4/14/10 5:36p jtna
 * SW7125-314: Coverity Defect ID:21019 MISSING_BREAK
 *
 * 47   4/5/10 1:40p vle
 * SW7335-699: fix error in reporting PCM support on attached HDMI
 *  receivers
 *
 * 46   4/5/10 10:10a erickson
 * SW7405-3997: add CASSERT's
 *
 * 45   1/27/10 12:43p jgarrett
 * SW7405-3390: Fixing channel count for PCM to use CEA enum instead of
 *  int.
 *
 * 44   1/7/10 10:06p rgreen
 * SW7405-3740: Use isr function to make sure all HP events are processed
 *
 * 43   1/4/10 4:44p rgreen
 * SW7405-3390: Set Audio Infoframe Channel Count to 0 (Refer to stream
 *  header) for compressed audio formats
 *
 * 42   12/21/09 4:03p rgreen
 * SW3556-953: Map Spread Spectrum configuration to sweep frequencey and
 *  range.  Remove SS parameters from open
 *
 * 41   10/23/09 1:49p rgreen
 * SW7405-3016,SW7405-2686: Remove setting i2c software mode from
 *  hdmiOutput;  softi2c mode set in i2c module
 *
 * 40   10/12/09 4:51p erickson
 * SW7405-2818: clean up indenting
 *
 * 39   9/24/09 4:50p jgarrett
 * SW7405-2818: Allowing HDMI audio without video
 *
 * 38   9/14/09 2:26p vle
 * SW7405-2711, SW7405-2670: free rxSenseEvent upon NEXUS_HdmiOutput_Close
 *
 * 37   9/9/09 1:16p erickson
 * SW7405-2977: convert to NEXUS_AudioOutputPort, fix warning
 *
 * 36   9/4/09 5:04p jtna
 * SW7405-2962: notify display on hotplug even if HDMI cable initially
 *  unplugged
 *
 * 35   8/27/09 6:58p vle
 * SW7405-2711, SW7405-2760:
 * Add interrupt based RSEN support. Fix 7403 build issue.
 *
 * 34   8/27/09 10:21a vle
 * SW7405-2670:
 * Fix build issue if security module is disabled
 *
 * 33   8/25/09 3:27p vle
 * SW7405-2670:
 * Add support to enable/disable TMDS lines
 * Do not poll for receiver sense while encryption is still enabled.
 * Add implementation of interrupt based receiver sense
 *
 * 32   8/12/09 10:04a erickson
 * PR57625: added DDP support
 *
 * 31   7/30/09 4:06p vle
 * PR57232: Enable auto color space if connect to HDMI sink with only RGB
 *  capability.
 *
 * 30   7/21/09 3:20p erickson
 * PR54888: handle BAVC_HDMI_AudioInfoFrame_CodingType_eReferToStream case
 *
 * 29   7/21/09 9:42a erickson
 * PR54888: merged additional NEXUS_HdmiOutputStatus
 *
 * PR54888/1   7/14/09 7:15p vle
 * PR54888: Add support to retrieve audio/video property from HDMI.
 *
 * 28   7/16/09 1:16p rgreen
 * PR56609: Add default settings for Spread Spectrum; Default is disabled.
 *
 * 27   6/26/09 4:56p erickson
 * PR56102: notify the display if there's a hot plug event. if there's no
 *  display, it's a no-op.
 *
 * 26   6/24/09 3:11p mphillip
 * PR54926: Add /proc/hdmi_output
 *
 * 25   6/22/09 7:04p rgreen
 * PR56132: Add support for Get HDMI Vendor Specific Info Frame
 *
 * 24   6/22/09 5:31p rgreen
 * PR56132: Add initial support for Get HDMI Vendor Specific Info Frame
 *
 * 23   6/22/09 5:24p katrep
 * PR56132: Fixed compiler error
 *
 * 24   6/22/09 5:25p rgreen
 * PR56132: Add initial support for Get HDMI Vendor Specific Info Frame
 *
 * 22   6/22/09 3:12p rgreen
 * PR56132: Add support for HDMI Vendor Specific Info Frame
 *
 * 21   5/11/09 1:48p erickson
 * PR53373: remove colorDepth from NEXUS_HdmiOutputStatus
 *
 * 20   5/4/09 3:40p erickson
 * PR53373: merge HDMI 1.3a support
 *
 * PR53373_DeepColor/1   4/16/09 3:58p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 19   3/20/09 11:56a erickson
 * PR53356: allow BHDM_EDID_GetDetailTiming to iterate through all timing
 *  information until it find one that works.
 *
 * 18   3/17/09 9:19a erickson
 * PR53271: default autoColorSpace to true. fix warning.
 *
 * 17   3/16/09 4:37p erickson
 * PR53271: added manual colorSpace option to NEXUS_HdmiOutputSettings
 *
 * 16   12/17/08 11:38p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * 15   12/16/08 2:55p erickson
 * PR48987: merge
 *
 * HDMI_TX_Plugfest11_200811/4   12/5/08 4:34p vle
 * PR 48987: Update to use better variable name.
 *
 * HDMI_TX_Plugfest11_200811/3   11/13/08 7:34p vle
 * PR 48987: Add run time option to enable I2C bit bang mode.
 *
 * HDMI_TX_Plugfest11_200811/2   11/10/08 7:14p vle
 * PR48842: Fix HDCP in BRUTUS. Make sure to report the status to Brutus
 *  and restart HDCP authentication process on each format change.
 *
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:58p vle
 * PR48785: Add CEC support for NEXUS including setting up callback and
 *  adding CEC supports API
 *
 * 14   12/12/08 11:07a tmontoya
 * PR46849: Fixed by adding HDMI DB items as required.
 *
 * 13   10/17/08 5:37p jtna
 * PR46852: fixed HDMI race condition
 *
 * 12   10/1/08 3:11a jgarrett
 * PR 47325: Correcting order of calls to set audio infoframe packet
 *  relative to GetSettings/EnableDisplay
 *
 * 11   8/1/08 4:26p erickson
 * PR39979: add CEC support
 *
 * 10   6/24/08 2:35p erickson
 * PR43666: support hdmi_bypass_edid env variable for debug
 *
 * 9   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 8   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 7   4/2/08 11:29a jgarrett
 * PR 41220: Coverity CID 7840
 *
 * 6   3/31/08 1:53p erickson
 * PR41075: add BDBG_OBJECT
 *
 * 5   3/31/08 1:41p erickson
 * PR41077: added NEXUS_ASSERT_MODULE to _priv function
 *
 * 4   3/17/08 12:38p erickson
 * PR40103: replace NEXUS_QueueCallback with NEXUS_TaskCallback
 *
 * 3   2/21/08 5:58p jgarrett
 * PR 39017: Separating HDCP into a separate file
 *
 * 2   2/5/08 1:49p jgarrett
 * PR 39017: Setting audio output port
 *
 * 1   2/1/08 5:25p jgarrett
 * PR 39017: Adding hdmi_output
 *
 **************************************************************************/

#include "nexus_hdmi_output_module.h"
#include "priv/nexus_hdmi_output_priv.h"
#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_core_video.h"
#include "priv/nexus_core_audio.h"
#include "priv/nexus_core.h"
#include "bhdm.h"
#include "bhdm_edid.h"
#include "bhdm_hdcp.h"
#if defined(HUMAX_PLATFORM_BASE) 
#include "humax_hdmi_info.h"
#endif

BDBG_MODULE(nexus_hdmi_output);

#if NEXUS_NUM_HDMI_OUTPUTS

static NEXUS_HdmiOutput g_hdmiOutputs[NEXUS_NUM_HDMI_OUTPUTS];

static NEXUS_VideoFormat NEXUS_HdmiOutput_P_GetPreferredFormat(NEXUS_HdmiOutputHandle output);

static void NEXUS_HdmiOutput_P_TimerCallback(void *pContext);

static void NEXUS_HdmiOutput_P_HotplugCallback(void *pContext);
static void NEXUS_HdmiOutput_P_HotPlug_isr(void *context, int param, void *data) ;

static void NEXUS_HdmiOutput_P_StopTimers(NEXUS_HdmiOutputHandle hdmiOutput) ;

#if defined(HUMAX_PLATFORM_BASE) 
static void NEXUS_HdmiOutput_P_RxConnectedTimer(void *pContext) ;
#endif

void NEXUS_HdmiOutputModule_Print(void)
{
    int i;
    for (i=0 ; i < NEXUS_NUM_HDMI_OUTPUTS; i++) {
        BDBG_LOG(("HDMI %d:%s%s%s",i,
            g_hdmiOutputs[i].opened ? "o" : "-",
            g_hdmiOutputs[i].videoConnected ? "v" : "-",
            g_hdmiOutputs[i].hdcpStarted ? "E" : "-"));
        if (g_hdmiOutputs[i].videoConnected) {
            BDBG_LOG((" video: s:%p d:%p",g_hdmiOutputs[i].videoConnector.source,g_hdmiOutputs[i].videoConnector.destination));
            BDBG_LOG((" audio: t:%d o:%p md:%p p:%d",g_hdmiOutputs[i].audioConnector.objectType,g_hdmiOutputs[i].audioConnector.pObjectHandle,g_hdmiOutputs[i].audioConnector.pMixerData,g_hdmiOutputs[i].audioConnector.port));
        }
    }
}

NEXUS_Error NEXUS_HdmiOutputModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    int i;
    NEXUS_HdmiOutput *handle;    
    BERR_Code rc = BERR_SUCCESS;     
    
    BSTD_UNUSED(pSettings);        

    for(i=0; i<NEXUS_NUM_HDMI_OUTPUTS; i++) {
        handle = &g_hdmiOutputs[i];	
        if (!handle) { continue; }
        if (!handle->opened) { continue; }        
        
        if (enabled) {
	    NEXUS_HdmiOutput_P_StopTimers(handle) ;

	    if(pSettings->mode == NEXUS_StandbyMode_eDeepSleep) {
		NEXUS_UnregisterEvent(handle->hotplugEventCallback);
		NEXUS_HdmiOutput_P_UninitHdcp(handle);
		BHDM_Close(handle->hdmHandle);
		handle->resumeFromS3 = true;
	    } else {		
		BHDM_StandbySettings standbySettings;

		BHDM_GetDefaultStandbySettings(&standbySettings);
		rc = BHDM_Standby(handle->hdmHandle, &standbySettings);
		handle->tmdsEnabled = false;
		handle->resumeFromS3 = false;
	    }	
        } else {	    
	    if(handle->resumeFromS3) {
		BKNI_EventHandle hdmEvent;
		BREG_I2C_Handle i2cRegHandle = NEXUS_I2c_GetRegHandle(handle->openSettings.i2c, NEXUS_MODULE_SELF);		
		rc = BHDM_Open(&handle->hdmHandle, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, i2cRegHandle, &handle->hdmSettings);
		if(rc) {goto err;}

		rc = BHDM_GetEventHandle(handle->hdmHandle, BHDM_EventHotPlug, &hdmEvent);
		if(rc) {goto err;}
		rc = NEXUS_HdmiOutput_P_InitHdcp(handle);
		if(rc) {goto err;}
		handle->hotplugEventCallback = NEXUS_RegisterEvent(hdmEvent, NEXUS_HdmiOutput_P_HotplugCallback, handle);
		rc = BHDM_InstallHotplugChangeCallback(handle->hdmHandle, NEXUS_HdmiOutput_P_HotPlug_isr, handle, 0);
		if(rc) {goto err;}

		handle->lastState = NEXUS_HdmiOutputState_eMax ;		
		NEXUS_HdmiOutput_P_HotplugCallback(handle);
	    } else {
		rc = BHDM_Resume(handle->hdmHandle);
		handle->tmdsEnabled = true;
	    }
        }
err:
        if (rc) { return BERR_TRACE(rc); }
    }
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
#endif   
    return NEXUS_SUCCESS;
}

void NEXUS_HdmiOutput_GetDefaultOpenSettings( NEXUS_HdmiOutputOpenSettings *pSettings )
{
    BHDM_Settings hdmDefaultSettings ;

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BHDM_GetDefaultSettings(&hdmDefaultSettings) ;

    pSettings->spd.deviceType = hdmDefaultSettings.eSpdSourceDevice ;
    BKNI_Memcpy(pSettings->spd.vendorName, hdmDefaultSettings.SpdVendorName, BAVC_HDMI_SPD_IF_VENDOR_LEN) ;
    BKNI_Memcpy(pSettings->spd.description, hdmDefaultSettings.SpdDescription, BAVC_HDMI_SPD_IF_DESC_LEN) ;

    pSettings->maxEdidRetries = 3;
    pSettings->powerPollingInterval = 500;

    pSettings->maxRxSenseRetries = 20 ;
    pSettings->rxSenseInterval = 50 ;
	
}

NEXUS_HdmiOutputHandle NEXUS_HdmiOutput_Open( unsigned index, const NEXUS_HdmiOutputOpenSettings *pSettings )
{
    BERR_Code errCode;
    BHDM_Settings hdmSettings;
    BREG_I2C_Handle i2cRegHandle;
    NEXUS_HdmiOutput *pOutput;
    BKNI_EventHandle hdmEvent ;
#if defined(HUMAX_PLATFORM_BASE) 
    static const uint8_t defaultVendorName[BAVC_HDMI_SPD_IF_VENDOR_LEN+1]  = HDMI_OUTPUT_VENDORNAME;
    static const uint8_t defaultDescription[BAVC_HDMI_SPD_IF_DESC_LEN+1] = HDMI_OUTPUT_DESCRIPTION;
#else
    static const uint8_t defaultVendorName[BAVC_HDMI_SPD_IF_VENDOR_LEN+1]  = "Broadcom";
    static const uint8_t defaultDescription[BAVC_HDMI_SPD_IF_DESC_LEN+1] = "STB Refsw Design";
    uint8_t *ptr ;
#endif
    
    uint8_t deviceAttached ;

    if ( index >= NEXUS_NUM_HDMI_OUTPUTS )
    {
        BDBG_ERR(("HDMI output %u not supported on this chipset", index));
        return NULL;
    }

    if ( NULL == pSettings || NULL == pSettings->i2c )
    {
        BDBG_ERR(("I2C Handle must be provided for HDMI output"));
        return NULL;
    }

    i2cRegHandle = NEXUS_I2c_GetRegHandle(pSettings->i2c, NEXUS_MODULE_SELF);
    if ( NULL == i2cRegHandle )
    {
        BDBG_ERR(("Invalid I2C Handle"));
        return NULL;
    }

    pOutput = &g_hdmiOutputs[index];

    if ( pOutput->opened )
    {
        BDBG_ERR(("HDMI output %u already opened", index));
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_HdmiOutput, pOutput);

    pOutput->openSettings = *pSettings;
    pOutput->hdcpFailureCallback = NEXUS_TaskCallback_Create(pOutput, NULL);
    pOutput->hdcpStateChangedCallback = NEXUS_TaskCallback_Create(pOutput, NULL);
    pOutput->hdcpSuccessCallback = NEXUS_TaskCallback_Create(pOutput, NULL);
    pOutput->hotplugCallback = NEXUS_TaskCallback_Create(pOutput, NULL);
    pOutput->notifyDisplay = NEXUS_TaskCallback_Create(pOutput, NULL);
    pOutput->sampleRate = BAVC_AudioSamplingRate_e48k;
    pOutput->audioBits = BAVC_AudioBits_e16;
    pOutput->audioFormat = BAVC_AudioFormat_ePCM;
    pOutput->audioNumChannels = 2;

    /* default NEXUS_HdmiOutputSettings */
    pOutput->settings.autoColorSpace = true;
    pOutput->settings.colorSpace = NEXUS_ColorSpace_eYCbCr444;
    pOutput->settings.colorDepth = NEXUS_HdmiColorDepth_e8bit;
    pOutput->settings.audioDitherEnabled = true;
    pOutput->settings.audioBurstType = NEXUS_SpdifOutputBurstType_ePause;

    BHDM_GetDefaultSettings(&hdmSettings);
    BDBG_ASSERT(pSettings->spd.deviceType < NEXUS_HdmiSpdSourceDeviceType_eMax);


    /* update Source Device Info Type */
    hdmSettings.eSpdSourceDevice = pSettings->spd.deviceType;

    /* update Source Device Info Vendor Name */

#if defined(HUMAX_PLATFORM_BASE)
/* Set Humax' SPD Data */ 
    BKNI_Memcpy(&hdmSettings.SpdVendorName, defaultVendorName, sizeof(defaultVendorName));
#else
    if ( pSettings->spd.vendorName[0] )
        ptr = (uint8_t *) pSettings->spd.vendorName ;
    else
        ptr = (uint8_t *) defaultVendorName ;
    BKNI_Memcpy(&hdmSettings.SpdVendorName, ptr, sizeof(hdmSettings.SpdVendorName));
#endif

    /* update Source Device Info Description */
#if defined(HUMAX_PLATFORM_BASE)
/* Set Humax' SPD Data */ 
    BKNI_Memcpy(hdmSettings.SpdDescription, defaultDescription, sizeof(defaultDescription));
#else
    if ( pSettings->spd.description[0] )
        ptr = (uint8_t *) &pSettings->spd.description ;
    else
        ptr = (uint8_t *) defaultDescription ; 
    BKNI_Memcpy(hdmSettings.SpdDescription, ptr, sizeof(hdmSettings.SpdDescription));
#endif

    #if BDBG_DEBUG_BUILD
    if (NEXUS_GetEnv("hdmi_bypass_edid"))
    {
        /***************************************************************/
        /* FOR DEBUG/TEST ONLY - Override EDID Checking  */
        /* set to true to disable EDID checking */
        hdmSettings.BypassEDIDChecking = true;
    }
    else {
        hdmSettings.BypassEDIDChecking = pSettings->bypassEdidChecking;
    }
    #endif


    /* Setup connectors */
    NEXUS_VIDEO_OUTPUT_INIT(&pOutput->videoConnector, NEXUS_VideoOutputType_eHdmi, pOutput);
    NEXUS_AUDIO_OUTPUT_INIT(&pOutput->audioConnector, NEXUS_AudioOutputType_eHdmi, pOutput);
    pOutput->audioConnector.port = NEXUS_AudioOutputPort_eMai;

#if BHDM_CONFIG_BTMR_SUPPORT
    hdmSettings.hTMR = g_pCoreHandles->tmr ;
#endif
    errCode = BHDM_Open(&pOutput->hdmHandle, 
        g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, i2cRegHandle, &hdmSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        return NULL;
    }
    pOutput->hdmSettings = hdmSettings;

    /* Register for Hot Plug Events */
    errCode = BHDM_GetEventHandle(pOutput->hdmHandle, BHDM_EventHotPlug, &hdmEvent);
    if (errCode)
        goto err_event;
    pOutput->hotplugEventCallback = 
        NEXUS_RegisterEvent(hdmEvent, NEXUS_HdmiOutput_P_HotplugCallback, pOutput);
    if ( NULL == pOutput->hotplugEventCallback )
        goto err_event;

    errCode = NEXUS_HdmiOutput_P_InitHdcp(pOutput);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_hdcp;
    }

    errCode = BHDM_InstallHotplugChangeCallback(
        pOutput->hdmHandle, NEXUS_HdmiOutput_P_HotPlug_isr, pOutput, 0);

    pOutput->lastState = NEXUS_HdmiOutputState_eMax ;

    /* force a hot plug if a device is attached */
    errCode = BHDM_RxDeviceAttached(pOutput->hdmHandle, &deviceAttached) ;
    BERR_TRACE(errCode) ;

#if defined(HUMAX_PLATFORM_BASE)
	pOutput->devicePowered = true;
#endif
    if (deviceAttached)
    {
        NEXUS_HdmiOutput_P_HotplugCallback(pOutput);
    }
    pOutput->opened = true;
	
    return pOutput;


err_hdcp:

    NEXUS_UnregisterEvent(pOutput->hotplugEventCallback);

err_event:
    BHDM_Close(pOutput->hdmHandle);
    return NULL;
}


static void NEXUS_HdmiOutput_P_StopTimers(NEXUS_HdmiOutputHandle hdmiOutput )
{
    NEXUS_OBJECT_ASSERT(NEXUS_HdmiOutput, hdmiOutput);

    if ( hdmiOutput->powerTimer )
    {
        NEXUS_CancelTimer(hdmiOutput->powerTimer);
        hdmiOutput->powerTimer	= NULL ;
    }

    if (hdmiOutput->postFormatChangeTimer)
    {
        NEXUS_CancelTimer(hdmiOutput->postFormatChangeTimer);
        hdmiOutput->postFormatChangeTimer = NULL;
        
    }
    if (hdmiOutput->connectTimer)
    {
        NEXUS_CancelTimer(hdmiOutput->connectTimer);
        hdmiOutput->connectTimer = NULL;
    }
}


static void NEXUS_HdmiOutput_P_Finalizer( NEXUS_HdmiOutputHandle hdmiOutput )
{
    NEXUS_OBJECT_ASSERT(NEXUS_HdmiOutput, hdmiOutput);

    if ( hdmiOutput->audioConnector.pMixerData )
    {
        BDBG_ERR(("Audio connector is still active.  Please call NEXUS_AudioOutput_Shutdown() first."));
        return;
    }
    if ( hdmiOutput->videoConnector.destination )
    {
        BDBG_ERR(("Video connector is still active.  Please call NEXUS_VideoOutput_Shutdown() first."));
        return;
    }

    NEXUS_HdmiOutput_P_StopTimers(hdmiOutput) ;
	
    NEXUS_UnregisterEvent(hdmiOutput->hotplugEventCallback);
    NEXUS_HdmiOutput_P_UninitHdcp(hdmiOutput);
    
    BHDM_Close(hdmiOutput->hdmHandle);

    NEXUS_TaskCallback_Destroy(hdmiOutput->hdcpFailureCallback);
    NEXUS_TaskCallback_Destroy(hdmiOutput->hdcpStateChangedCallback);
    NEXUS_TaskCallback_Destroy(hdmiOutput->hdcpSuccessCallback);
    NEXUS_TaskCallback_Destroy(hdmiOutput->hotplugCallback);
    NEXUS_TaskCallback_Destroy(hdmiOutput->notifyDisplay);
    if (hdmiOutput->crc.queue) {
        BKNI_Free(hdmiOutput->crc.queue);
    }

    /* memset of zero will also wipe out BDBG_OBJECT */
    BKNI_Memset(hdmiOutput, 0, sizeof(*hdmiOutput));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_HdmiOutput, NEXUS_HdmiOutput_Close);

/**
Summary:
Get current settings
**/
void NEXUS_HdmiOutput_GetSettings(
    NEXUS_HdmiOutputHandle output,
    NEXUS_HdmiOutputSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);
    *pSettings = output->settings;
}

NEXUS_Error NEXUS_HdmiOutput_SetSettings( NEXUS_HdmiOutputHandle output, const NEXUS_HdmiOutputSettings *pSettings )
{
    bool audioChanged = false;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    NEXUS_TaskCallback_Set(output->hotplugCallback, &pSettings->hotplugCallback);

    /* Save settings and return if not connected */
    if ( output->lastState != NEXUS_HdmiOutputState_eConnected )
    {
        output->settings = *pSettings;
        return BERR_SUCCESS;
    }

    /* See if audio settings have changed */
    if ( output->settings.audioDitherEnabled != pSettings->audioDitherEnabled ||
         output->settings.audioBurstType != pSettings->audioBurstType ||
         BKNI_Memcmp(&output->settings.audioChannelStatusInfo, &pSettings->audioChannelStatusInfo, sizeof(pSettings->audioChannelStatusInfo)) )
    {
        audioChanged = true;
    }

    output->settings = *pSettings;

    if (output->crc.size != pSettings->crcQueueSize) {
        void *new_ptr = NULL, *old_ptr;

        /* defer the free until after critical section */
        old_ptr = output->crc.queue;
        /* queue size of 1 is treated same as 0 because it can't hold anything */
        if (pSettings->crcQueueSize > 1) {
            new_ptr = BKNI_Malloc(pSettings->crcQueueSize * sizeof(output->crc.queue[0]));
            if (!new_ptr) {
                return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            }
        }

        /* must synchronize with ISR, so set state in CS */
        BKNI_EnterCriticalSection();
        output->crc.queue = new_ptr;
        output->crc.size = pSettings->crcQueueSize>1?pSettings->crcQueueSize:0;
        output->crc.wptr = output->crc.rptr = 0; /* flush */
        BKNI_LeaveCriticalSection();

        if (old_ptr) {
            BKNI_Free(old_ptr);
        }
    }    

    /* See NEXUS_VideoOutput_P_SetHdmiSettings_nosync for how HdmiOutputSettings are applied to VDC */

    /* After saving the settings, notify the Display module that new HdmiOutputSettings must be applied to VDC.
    A delay of one context switch is required to keep HdmiOutput and Display in separate synchronization contexts. */
    NEXUS_TaskCallback_Fire(output->notifyDisplay);
    if ( output->notifyAudioEvent && audioChanged )
    {
        BKNI_SetEvent(output->notifyAudioEvent);
    }

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_GetStatus( NEXUS_HdmiOutputHandle output, NEXUS_HdmiOutputStatus *pStatus )
{
    bool supportedVideoFormats[BFMT_VideoFmt_eMaxCount];
    uint16_t supported3DFormats[BFMT_VideoFmt_eMaxCount];       
    BHDM_EDID_AudioDescriptor supportedAudioFormats[BAVC_AudioFormat_eMaxCount];
    NEXUS_Error errCode;
    BHDM_EDID_RxVendorSpecificDB vendorDb;
    int i;
    uint8_t deviceAttached ;
    uint8_t RxSense;

    BHDM_Settings hdmSettings;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);
    BDBG_CASSERT(sizeof(NEXUS_HdmiOutputMonitorRange) == sizeof(BHDM_EDID_MonitorRange));
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    BKNI_Memset(supported3DFormats, 0, sizeof (supported3DFormats));

    /* Check for device */		
    errCode = BHDM_RxDeviceAttached(output->hdmHandle, &deviceAttached) ;
    BERR_TRACE(errCode) ;
    if (!deviceAttached) 
    {
        pStatus->connected = false ;
        return 0; 
    }
    pStatus->connected = true ;

    /* do not report status until ready */
    if (output->lastState == NEXUS_HdmiOutputState_eMax)
    {
        BDBG_MSG(("HdmiOutput not intialized ")) ;
        return 0; 
    }

    /* Get current RxSense; could still be reported OFF if power was just enabled */
    errCode = BHDM_GetReceiverSense(output->hdmHandle, &RxSense) ;
    pStatus->rxPowered = RxSense ;
    if ((!RxSense) && (!errCode))
    {
        BDBG_WRN(("Attached Rx may be powered down...")) ;
		#if defined(HUMAX_PLATFORM_BASE) 
		output->connectTimer = NEXUS_ScheduleTimer(output->openSettings.rxSenseInterval,
									  NEXUS_HdmiOutput_P_RxConnectedTimer, output) ;
		#endif
    }

    errCode = BHDM_EDID_IsRxDeviceHdmi(output->hdmHandle, &vendorDb, &pStatus->hdmiDevice);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        /* Keep going */
    }
    else {
        /* Copy over the HDMI DB specific data. */
        pStatus->physicalAddressA = vendorDb.PhysAddr_A;
        pStatus->physicalAddressB = vendorDb.PhysAddr_B;
        pStatus->physicalAddressC = vendorDb.PhysAddr_C;
        pStatus->physicalAddressD = vendorDb.PhysAddr_D;
    }

    errCode = BHDM_EDID_GetMonitorName(output->hdmHandle, (uint8_t *)pStatus->monitorName);
    if ( errCode )
    {
        pStatus->monitorName[0] = 0;
        errCode = BERR_TRACE(errCode);
        /* Keep going */
    }

    /* Get monitor range info */
    errCode = BHDM_EDID_GetMonitorRange(output->hdmHandle,(BHDM_EDID_MonitorRange *) &pStatus->monitorRange);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        /* Keep going */
    }
    
    /* Get preferred format */
    pStatus->preferredVideoFormat = NEXUS_HdmiOutput_P_GetPreferredFormat(output);

    /* Get all supported video formats */
    errCode = BHDM_EDID_GetSupportedVideoFormats(output->hdmHandle, supportedVideoFormats);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        /* Keep going */
    }
    else /* add supported regular (2D) formats */
    {
        for ( i = 0; i < BFMT_VideoFmt_eMaxCount; i++ )
        {
            if ( supportedVideoFormats[i] )
            {
                NEXUS_VideoFormat nexusFormat = NEXUS_P_VideoFormat_FromMagnum(i);
                if ( nexusFormat != NEXUS_VideoFormat_eUnknown )
                    pStatus->videoFormatSupported[nexusFormat] = true;
            }
        }
    }

    /* Get supported 3D Formats */
	errCode = BHDM_EDID_GetSupported3DFormats(output->hdmHandle, supported3DFormats);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        /* Keep going */
    }
    else 
    { 
        /* add supported 3D formats */
        for (i=0; i < BFMT_VideoFmt_eMaxCount; i++)
        {
            if (supported3DFormats[i])
            {
                NEXUS_VideoFormat nexusFormat = NEXUS_P_VideoFormat_FromMagnum(i);
                if ( nexusFormat != NEXUS_VideoFormat_eUnknown)
                    pStatus->hdmi3DFormatsSupported[nexusFormat] = supported3DFormats[i];
            }
        }
    }


    /* Get all supported audio formats */
    if ( pStatus->hdmiDevice )
    {
        errCode = BHDM_EDID_GetSupportedAudioFormats(output->hdmHandle, supportedAudioFormats);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            /* Keep going */
        }
        else {
            for ( i = 0; i < BAVC_AudioFormat_eMaxCount; i++ )
            {
                if ( supportedAudioFormats[i].Supported )
                {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) && !defined(CONFIG_HDMI_REFACTORING)
					NEXUS_AudioCodec	nAudioCodec;
					switch ( i )
					{
	                case BAVC_AudioFormat_ePCM:
	                    pStatus->maxAudioPcmChannels = supportedAudioFormats[i].AudioChannels;
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_ePcm] = true;
	                    nAudioCodec = NEXUS_AudioCodec_ePcm;
	                    break;
	                case BAVC_AudioFormat_eAC3:
	                    nAudioCodec = NEXUS_AudioCodec_eAc3;
	                    break;
	                case BAVC_AudioFormat_eMP3:
	                    nAudioCodec = NEXUS_AudioCodec_eMp3;
	                    break;
	                case BAVC_AudioFormat_eMPEG1:
	                case BAVC_AudioFormat_eMPEG2:
	                    nAudioCodec = NEXUS_AudioCodec_eMpeg;
	                    break;
	                case BAVC_AudioFormat_eAAC:
	                    nAudioCodec = NEXUS_AudioCodec_eAac;
	                    break;
	                case BAVC_AudioFormat_eDTS:
	                    nAudioCodec = NEXUS_AudioCodec_eDts;
	                    break;
                        case BAVC_AudioFormat_eDDPlus:
	                    nAudioCodec = NEXUS_AudioCodec_eAc3Plus;
	                    break;
                        case BAVC_AudioFormat_eMATMLP:
	                    nAudioCodec = NEXUS_AudioCodec_eMlp;
	                    break;
	                default:
	                    nAudioCodec = NEXUS_AudioCodec_eUnknown;
	                    break;
                    }
                    
                    pStatus->audioInfo[nAudioCodec].bSupported = supportedAudioFormats[i].Supported;
                    pStatus->audioInfo[nAudioCodec].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
#else
                    switch ( i )
                    {
                    case BAVC_AudioFormat_ePCM:
                        pStatus->maxAudioPcmChannels = supportedAudioFormats[i].AudioChannels;
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_ePcm] = true;            
                        pStatus->audioInfo[NEXUS_AudioCodec_ePcm].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
                        break;
                    case BAVC_AudioFormat_eMP3:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eMp3] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eMp3].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;                        
                        break;
                    case BAVC_AudioFormat_eMPEG1:
                    case BAVC_AudioFormat_eMPEG2:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eMpeg] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eMpeg].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;                                                
                        break;
                    case BAVC_AudioFormat_eAAC:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eAac] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eAac].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;                                                                        
                        break;
                    case BAVC_AudioFormat_eDTSHD:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eDtsHd] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eDtsHd].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
                        /* Fall Through (DTS-HD Implies DTS by the spec) */                        
                    case BAVC_AudioFormat_eDTS:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eDts] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eDts].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
                        break;
                    case BAVC_AudioFormat_eDDPlus:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eAc3Plus] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eAc3Plus].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
                        /* Fall Through (AC3+ Implies AC3 by the spec) */
                    case BAVC_AudioFormat_eAC3:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eAc3] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eAc3].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
                        break;
                    case BAVC_AudioFormat_eMATMLP:
                        pStatus->audioCodecSupported[NEXUS_AudioCodec_eMlp] = true;
                        pStatus->audioInfo[NEXUS_AudioCodec_eMlp].uSamplingRate = supportedAudioFormats[i].ucCeaSampleRates;
                        break;
                     default:
                        break;
                    }
#endif
                }
            }
        }
    }

    /* Save auto lipsync audio/video latency info from the attached Rx */
    pStatus->autoLipsyncInfo.audioLatency = vendorDb.Audio_Latency;
    pStatus->autoLipsyncInfo.videoLatency = vendorDb.Video_Latency;
    pStatus->autoLipsyncInfo.interlacedAudioLatency = vendorDb.Interlaced_Audio_Latency;
    pStatus->autoLipsyncInfo.interlacedVideoLatency = vendorDb.Interlaced_Video_Latency;

    errCode = BHDM_GetHdmiSettings(output->hdmHandle, &hdmSettings);
    if (errCode)
    {
        errCode = BERR_TRACE(errCode);
        /* Keep going */
    }
    else {
        pStatus->videoFormat = NEXUS_P_VideoFormat_FromMagnum(hdmSettings.eInputVideoFmt);
        pStatus->aspectRatio = NEXUS_P_AspectRatio_FromMagnum(hdmSettings.eAspectRatio);
        switch (hdmSettings.eColorimetry)
        {
        case BAVC_MatrixCoefficients_eItu_R_BT_709:
        case BAVC_MatrixCoefficients_eSmpte_170M:
            pStatus->colorSpace = NEXUS_ColorSpace_eYCbCr444;
            break;

        case BAVC_MatrixCoefficients_eHdmi_RGB:
        default:
            pStatus->colorSpace = NEXUS_ColorSpace_eRgb;
            break;
        }

        switch (hdmSettings.eAudioFormat) {
        case BAVC_AudioFormat_ePCM: pStatus->audioFormat = NEXUS_AudioCodec_ePcm; break;
        case BAVC_AudioFormat_eAC3: pStatus->audioFormat = NEXUS_AudioCodec_eAc3; break;
        case BAVC_AudioFormat_eMPEG1: pStatus->audioFormat = NEXUS_AudioCodec_eMpeg; break;
        case BAVC_AudioFormat_eMP3: pStatus->audioFormat = NEXUS_AudioCodec_eMp3; break;
        case BAVC_AudioFormat_eMPEG2: pStatus->audioFormat = NEXUS_AudioCodec_eMpeg; break;
        case BAVC_AudioFormat_eAAC: pStatus->audioFormat = NEXUS_AudioCodec_eAacPlus; break;
        case BAVC_AudioFormat_eDTS: pStatus->audioFormat = NEXUS_AudioCodec_eDts; break;
        case BAVC_AudioFormat_eDDPlus: pStatus->audioFormat = NEXUS_AudioCodec_eAc3Plus; break;
        case BAVC_AudioFormat_eWMAPro: pStatus->audioFormat = NEXUS_AudioCodec_eWmaPro; break;

        case BAVC_AudioFormat_eAVS:
        case BAVC_AudioFormat_eATRAC:
        case BAVC_AudioFormat_eOneBit:
        case BAVC_AudioFormat_eDTSHD:
        case BAVC_AudioFormat_eMATMLP:
        case BAVC_AudioFormat_eDST:

        default:
            /* If the codec is actually supported by our silicon, please extend NEXUS_AudioCodec. If not, this warning is sufficient. */
            BDBG_WRN(("Unable to convert HDMI audio info frame codec %d to NEXUS_HdmiOutputStatus.audioFormat", hdmSettings.eAudioFormat));
            break;
        }


        switch (hdmSettings.eAudioSamplingRate) {
        case BAVC_AudioSamplingRate_e32k : 
            pStatus->audioSamplingRate = 32000 ; break ;
        case BAVC_AudioSamplingRate_e44_1k : 
            pStatus->audioSamplingRate = 44100 ; break ;
        case BAVC_AudioSamplingRate_e48k : 
            pStatus->audioSamplingRate = 48000 ; break ;
        case BAVC_AudioSamplingRate_e88_2k : 
            pStatus->audioSamplingRate = 88200 ; break ;
        case BAVC_AudioSamplingRate_e96k: 
            pStatus->audioSamplingRate = 96000 ; break ;
        case BAVC_AudioSamplingRate_e176_4k: 
            pStatus->audioSamplingRate = 176400 ; break ;
        case BAVC_AudioSamplingRate_e192k: 
            pStatus->audioSamplingRate = 192000 ; break ;
        default: break;
        }

        switch (hdmSettings.eAudioBits) {
        case BAVC_AudioBits_e16: pStatus->audioSamplingSize = 16; break;
        case BAVC_AudioBits_e20: pStatus->audioSamplingSize = 20; break;
        case BAVC_AudioBits_e24: pStatus->audioSamplingSize = 24; break;
        default: break;
        }

        switch (hdmSettings.stAudioInfoFrame.ChannelCount) {
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e2Channels: pStatus->audioChannelCount = 2; break;
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e3Channels: pStatus->audioChannelCount = 3; break;
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e4Channels: pStatus->audioChannelCount = 4; break;
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e5Channels: pStatus->audioChannelCount = 5; break;
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e6Channels: pStatus->audioChannelCount = 6; break;
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e7Channels: pStatus->audioChannelCount = 7; break;
        case BAVC_HDMI_AudioInfoFrame_ChannelCount_e8Channels: pStatus->audioChannelCount = 8; break;
        default: break;
        }
    }

    return 0;
}

NEXUS_Error NEXUS_HdmiOutput_GetBasicEdidData( NEXUS_HdmiOutputHandle output, NEXUS_HdmiOutputBasicEdidData *pData )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);
    BDBG_ASSERT(NULL != pData);

    /* These structures are just copies of one another, this should keep them in sync */
    BDBG_CASSERT(sizeof(NEXUS_HdmiOutputBasicEdidData) == sizeof(BHDM_EDID_BasicData));

    errCode = BHDM_EDID_GetBasicData(output->hdmHandle,(BHDM_EDID_BasicData *)pData);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_GetEdidBlock( NEXUS_HdmiOutputHandle output, unsigned blockNum, NEXUS_HdmiOutputEdidBlock *pBlock )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);
    BDBG_ASSERT(NULL != pBlock);

    errCode = BHDM_EDID_GetNthBlock(output->hdmHandle, blockNum, pBlock->data, sizeof(pBlock->data));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

NEXUS_VideoOutput NEXUS_HdmiOutput_GetVideoConnector( NEXUS_HdmiOutputHandle output )
{
    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);
    return &output->videoConnector;
}

NEXUS_AudioOutput NEXUS_HdmiOutput_GetAudioConnector( NEXUS_HdmiOutputHandle output )
{
    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);
    return &output->audioConnector;
}

static NEXUS_VideoFormat NEXUS_HdmiOutput_P_GetPreferredFormat(NEXUS_HdmiOutputHandle output)
{
    NEXUS_Error errCode;
    BFMT_VideoFmt magnumFormat;
    NEXUS_VideoFormat nexusFormat;
    BHDM_EDID_DetailTiming detailedTiming ;
    uint8_t nativeFormat, videoIdCode;
    unsigned detailTimingNum = 1;

    magnumFormat = BFMT_VideoFmt_eDVI_640x480p;     /* Default to safe mode */

try_next_edid:
    /* first detailed EDID block contains preferred format */
    (void) BHDM_EDID_GetDetailTiming(output->hdmHandle, detailTimingNum, &detailedTiming, &magnumFormat);

    if (!BFMT_SUPPORT_HDMI(magnumFormat)) {
        detailTimingNum++;
        goto try_next_edid;
    }

    /* Translate the first selection */
    nexusFormat = NEXUS_P_VideoFormat_FromMagnum(magnumFormat);

    /* If preferred format is VESA, check for HD format */
    if ( BFMT_IS_VESA_MODE(magnumFormat) || NEXUS_VideoFormat_eUnknown == nexusFormat )
    {
        errCode = BHDM_EDID_GetVideoDescriptor(output->hdmHandle, detailTimingNum, &videoIdCode, &magnumFormat, &nativeFormat);
        if ( errCode == BERR_SUCCESS )
        {
            BDBG_WRN(("Using CEA-861 Video ID %d (HD) instead of Preferred VESA format", videoIdCode));
        }
    }

    /* Translate */
    nexusFormat = NEXUS_P_VideoFormat_FromMagnum(magnumFormat);
    if ( NEXUS_VideoFormat_eUnknown == nexusFormat )
    {
        BDBG_WRN(("Unknown preferred format %d, defaulting to NTSC", magnumFormat));
        return NEXUS_VideoFormat_eNtsc;
    }

    return nexusFormat;
}


static void NEXUS_HdmiOutput_P_RxRemoved(void *pContext) 
{
    NEXUS_HdmiOutputHandle output = (NEXUS_HdmiOutputHandle) pContext;

    /* if already waiting to connect, 
         cancel request to connect since the Rx has been removed */
    if (output->connectTimer)
    {
        NEXUS_CancelTimer(output->connectTimer) ;
        output->connectTimer = NULL ;
    }

    /* Connected -> disconnected or Connected -> powered down.  Disable output and fire callback */
    if ((!output->tmdsEnabled)
    &&  (output->videoConnected)) 
    {
        BDBG_MSG(("TMDS lines have already been disabled...")) ;
        return ;
    }

    /* disable TMDS lines (data) and notify app of disconnect */
    NEXUS_HdmiOutput_SetTmdsSignal(output, false) ;

    NEXUS_TaskCallback_Fire(output->hotplugCallback);

    NEXUS_HdmiOutput_P_HdcpNotifyHotplug(output);
}


static void NEXUS_HdmiOutput_P_RxConnectedTimer(void *pContext) 
{

    NEXUS_HdmiOutputState newState, lastState;
    NEXUS_HdmiOutputHandle output = (NEXUS_HdmiOutputHandle)pContext;
	
    output->connectTimer = NULL;
    newState = NEXUS_HdmiOutput_P_GetState(output);
    lastState = output->lastState;
    output->lastState = newState;

    if (lastState == NEXUS_HdmiOutputState_eConnected 
    &&   newState == NEXUS_HdmiOutputState_eConnected )
    {
        return ;
    }

    /* 
    ** after a hot plug (cable connected) event, some Rx Devices may require 
    ** extra time before it can be determined if the device is powered.  
    ** re-schedule the connectTimer until we have waited at least 
    ** output->openSettings.rxSenseInterval * output->openSettings.maxRxSenseRetries
    ** by default this is 50ms * 20 = 1000ms (1 second) 
     */
    if (newState == NEXUS_HdmiOutputState_ePoweredDown)
    {
#if defined(HUMAX_PLATFORM_BASE)
#else
        if (output->checkRxSenseCount < output->openSettings.maxRxSenseRetries)
#endif
        {
            output->connectTimer = NEXUS_ScheduleTimer(output->openSettings.rxSenseInterval,
                NEXUS_HdmiOutput_P_RxConnectedTimer, output) ;
            output->checkRxSenseCount++ ;
			
            BDBG_MSG(("Check for RxSense try %2d of %2d; Elapsed time: %4dms",
                output->checkRxSenseCount, output->openSettings.maxRxSenseRetries,
                output->checkRxSenseCount * output->openSettings.rxSenseInterval)) ;
#if defined(HUMAX_PLATFORM_BASE)
#else
            return ;
#endif
        }

    }

    /* RxSense has been checked the maximum number of times 
        OR the device is Connnected (powered ON)
    */


    if ( newState == NEXUS_HdmiOutputState_eDisconnected)
    {
        NEXUS_HdmiOutput_P_RxRemoved(output) ;
        return ;
    }

    if ( newState == NEXUS_HdmiOutputState_eConnected )
    {

        /* make sure state has not changed... 
            if device connection has changes, there is no need to contuinue 
        */

        newState = NEXUS_HdmiOutput_P_GetState(output);
        lastState = output->lastState;
        output->lastState = newState;

        if ( newState != lastState ) 
            return ;

        /* notify application of hotplug - only for status */
        NEXUS_TaskCallback_Fire(output->hotplugCallback);

        /* allow display to re-enable HDMI output */
        NEXUS_TaskCallback_Fire(output->notifyDisplay);

    }
    else if ( newState == NEXUS_HdmiOutputState_ePoweredDown )
    {
    #if defined(HUMAX_PLATFORM_BASE)
	/* add condition to remove infinite log*/
    	if(lastState != newState)
	#endif
        /* Disconnected -> powered down - treat as no event */
        BDBG_WRN(("Receiver powered down..."));

        NEXUS_HdmiOutput_SetTmdsSignal(output, false) ;
    }

    NEXUS_HdmiOutput_P_HdcpNotifyHotplug(output);
}


static void NEXUS_HdmiOutput_P_ReadEdid(NEXUS_HdmiOutputHandle hdmiOutput) 
{
	
    unsigned i=0;
    BERR_Code errCode ;

    do
    {
        errCode = BHDM_EDID_Initialize(hdmiOutput->hdmHandle);
        if (errCode == BERR_SUCCESS)
            break ;

        /*
        this delay should really not be needed, but allow some time
        between EDID read attempts in case the Rx is still settling
        */
        BKNI_Sleep(100) ;
        if ((i + 1) < hdmiOutput->openSettings.maxEdidRetries)
        {
            BDBG_WRN(("Read EDID Try %d of %d",
            i + 1, hdmiOutput->openSettings.maxEdidRetries)) ;
        }
    } while ( i++ < hdmiOutput->openSettings.maxEdidRetries ) ;

    if ( errCode == BHDM_NO_RX_DEVICE )
    {
        /* Device was disconnected during retries.  Abort */
        BDBG_WRN(("Device Disconnected while trying to read EDID Line %d", 
            __LINE__)) ;
        hdmiOutput->lastState = NEXUS_HdmiOutputState_eDisconnected;
        return;
    }
	
    if ( i >= hdmiOutput->openSettings.maxEdidRetries )
    {
        BDBG_ERR(("Unable to read EDID after %d attempts", i));
        /* TODO: Bail out here or continue? -- currently continuing. */
    }
}


static void NEXUS_HdmiOutput_P_HotplugCallback(void *pContext)
{
    NEXUS_HdmiOutputState newState, lastState;
    NEXUS_HdmiOutputHandle output = (NEXUS_HdmiOutputHandle)pContext;
    bool local_forceDisconnect ;
    NEXUS_HdmiOutputState local_lastHotplugState ;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    BKNI_EnterCriticalSection();
        local_forceDisconnect = output->forceDisconnect_isr ;   /* force removal of HDMI */
        local_lastHotplugState = output->lastHotplugState_isr;  /* last isr state: removed, connected, powered down  */
		
        output->forceDisconnect_isr = false ;
    BKNI_LeaveCriticalSection();

    /* force a disconnect of HDMI device if cable is removed  */
    if (local_forceDisconnect)  
    {
        BDBG_MSG(("Hot Plug request to disable TMDS lines; State: %d  ", output->lastState)) ;

        if (local_lastHotplugState == NEXUS_HdmiOutputState_eDisconnected)
            output->lastState = NEXUS_HdmiOutputState_eDisconnected ;
        else 
            output->lastState = NEXUS_HdmiOutputState_eConnected ;

        NEXUS_HdmiOutput_P_RxRemoved(output) ;
        return ;
    }


    /* 
        After a HotPlug, Init, Resume, schedule a connection event for later...
        Use the time to allow the Rx to settle and to ensure and accurate RxSense reading
      */
    if ((local_lastHotplugState == NEXUS_HdmiOutputState_eConnected)
    || (output->lastState == NEXUS_HdmiOutputState_eMax))
    {
    #if defined(HUMAX_PLATFORM_BASE)
	/*remove condition because of  confiliction with  P_HDMI_EnableHdmiMode */
		if(output->devicePowered && output->videoConnected)
	#else
        if (output->videoConnected)
	#endif
        {
   
            if (!output->connectTimer)
            {
                /*  turn on TMDS lines after the cable (Hot Plug event) is connected 
                               and wait for  Rx to be ready */			
                if (!output->tmdsEnabled)
                {
                    NEXUS_HdmiOutput_SetTmdsSignal(output, true) ;
                }

                output->connectTimer = NEXUS_ScheduleTimer(output->openSettings.rxSenseInterval, 
                   NEXUS_HdmiOutput_P_RxConnectedTimer, output) ;

                output->checkRxSenseCount = 0  ;
            }
            else
            {
                /* report debug message to check if events are occurring consecutively */
                BDBG_WRN(("Connected event already pending...")) ;
            }
        }
        else
        {
        	BDBG_WRN(("Device in standby mode...")) ;
            newState = NEXUS_HdmiOutput_P_GetState(output);
            lastState = output->lastState;
            output->lastState = newState;
        
            if ( newState == lastState ) 
                return;
        }

        /* EDID should be available to read now */
        NEXUS_HdmiOutput_P_ReadEdid(output) ;

        /* notify app of changes in hot plug... 
            it will enable/disable video as needed */
    #if defined(HUMAX_PLATFORM_BASE)
		if(output->devicePowered)
	#else
        if (!output->videoConnected)
	#endif
	        NEXUS_TaskCallback_Fire(output->hotplugCallback) ;			

        return ;
    }


    newState = NEXUS_HdmiOutput_P_GetState(output);
    lastState = output->lastState;
    output->lastState = newState;

    if ( newState == lastState ) 
        return;

    else if (newState == NEXUS_HdmiOutputState_eConnected )
    {
        BDBG_MSG(("Immediate Connect Event... ")) ;
        output->lastState = lastState ;
        NEXUS_HdmiOutput_P_RxConnectedTimer(output) ;		
    }

    else if ( newState == NEXUS_HdmiOutputState_ePoweredDown )
    {
        /* Disconnected -> powered down - treat as no event */
        NEXUS_HdmiOutput_P_RxRemoved(output) ;
        BDBG_MSG(("Receiver powered down, no hotplug event."));
    }

    else if ( newState == NEXUS_HdmiOutputState_eDisconnected)
    {
        /* Disconnected -> powered down - treat as no event */
        NEXUS_HdmiOutput_P_RxRemoved(output) ;
        BDBG_MSG(("Receiver not connected..."));
    }

    else
    {
        /* should never get here */        
        BDBG_WRN(("HdmiOutputState (%d) not handled...", newState )) ;
        BERR_TRACE(NEXUS_UNKNOWN) ;
    }

}


static void NEXUS_HdmiOutput_P_HotPlug_isr(void *context, int param, void *data)
{
    NEXUS_HdmiOutputHandle hdmiOutput = context;
    bool deviceAttached =  * (bool *) data ;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(hdmiOutput, NEXUS_HdmiOutput);

    if (deviceAttached) {   
        hdmiOutput->lastHotplugState_isr = NEXUS_HdmiOutputState_eConnected ;
        hdmiOutput->forceDisconnect_isr = false ;
    }
    else
    {
        hdmiOutput->lastHotplugState_isr = NEXUS_HdmiOutputState_eDisconnected ;
        hdmiOutput->forceDisconnect_isr = true ;
    }

    BDBG_MSG(("HotPlug Status: %d", hdmiOutput->lastHotplugState_isr)) ;
}

#if defined(HUMAX_PLATFORM_BASE)
void NEXUS_HdmiOutput_HotplugCallback(void *pContext)
{
	NEXUS_HdmiOutput_P_HotplugCallback(pContext);
}
#else
void NEXUS_HdmiOutput_HotplugCallback(void *pContext)
{
}
#endif

NEXUS_HdmiOutputState NEXUS_HdmiOutput_P_GetState(NEXUS_HdmiOutputHandle output)
{
    BERR_Code errCode;
    uint8_t rxSense=0, attached=1;
    NEXUS_HdmiOutputState outputState ;

    errCode = BHDM_RxDeviceAttached(output->hdmHandle, &attached);
    BERR_TRACE(errCode) ;
	
    if (attached)
    {

#if NEXUS_HAS_SECURITY
        /* as long as encryption is enabled; no need to check for receiver sense */
        if (output->hdcpState == BHDCPlib_State_eEncryptionEnabled)
        {
            outputState = NEXUS_HdmiOutputState_eConnected ;
            goto done ;
        }
#endif

        /* device is connected... now check if it is powered */
        errCode = BHDM_GetReceiverSense(output->hdmHandle, &rxSense);
        if ( errCode == BERR_SUCCESS )
        {
            bool enableTmds ;
            if ( rxSense )
            {
                /* TMDS should be enabled based on whether or not 
                           HDMI video (output) is connected to the Display */
		#if defined(HUMAX_PLATFORM_BASE)
			if(output->devicePowered && output->videoConnected)
			{
				enableTmds	= true;
			}
			else
			{
				enableTmds	= false;
			}
		#else
				enableTmds	= output->videoConnected ;
		#endif
 
                outputState = NEXUS_HdmiOutputState_eConnected ;
            }
            else
            {
                outputState = NEXUS_HdmiOutputState_ePoweredDown ;
                enableTmds = false ;
            }

            /* enable/disable TMDS lines according to detected RxSense */
            /* RxSense could be in standby... call will enable all TMDS lines */
	#if defined(HUMAX_PLATFORM_BASE)			
            if (enableTmds != output->tmdsEnabled) 
	#else				
            if ((enableTmds != output->tmdsEnabled)
            && (output->videoConnected))
	#endif
            {
                BDBG_MSG(("TMDS State changed to %d", enableTmds)) ;
                NEXUS_HdmiOutput_SetTmdsSignal(output, enableTmds) ;
            }

            BDBG_MSG(("Attached Rx Powered? %s", 
                outputState == NEXUS_HdmiOutputState_eConnected ? "Yes" : "No")) ;

            BDBG_MSG(("HDMI Output connected to Display? %s ", 
                output->videoConnected  ? "Yes" : "No")) ;

            goto done ;
        }
    }

    /* device disconnected*/
    outputState = NEXUS_HdmiOutputState_eDisconnected ;

done :
    return outputState ;
}


static void NEXUS_HdmiOutput_P_TimerCallback(void *pContext)
{
    NEXUS_HdmiOutputHandle output = pContext;
    uint8_t receiverSense ;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    output->powerTimer = NULL ; /* clear timer used */

    BHDM_GetReceiverSense(output->hdmHandle, &receiverSense) ;

    if (receiverSense != output->lastReceiverSense)
    {
        BKNI_EnterCriticalSection() ;
            NEXUS_HdmiOutput_P_HotPlug_isr(output, 0, &receiverSense) ;
        BKNI_LeaveCriticalSection() ;
    }

    output->lastReceiverSense = receiverSense ;

    /* Re-arm timer */
    output->powerTimer = 
        NEXUS_ScheduleTimer(output->openSettings.powerPollingInterval, 
            NEXUS_HdmiOutput_P_TimerCallback, pContext);
}


/* VDC -> HDM callback info for rate changes */
void NEXUS_HdmiOutput_VideoRateChange_isr(
    NEXUS_HdmiOutputHandle handle,
    BAVC_VdcDisplay_Info *pDisplayInfo
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    BDBG_ASSERT(NULL != pDisplayInfo);

    BDBG_MSG(("HDMI Video Rate Change"));
    BHDM_AudioVideoRateChangeCB_isr(handle->hdmHandle, BHDM_Callback_Type_eVideoChange, pDisplayInfo);
}

/* RAP -> HDM callback for sample rate changes */
void NEXUS_HdmiOutput_AudioSampleRateChange_isr(
    NEXUS_HdmiOutputHandle handle,
    BAVC_AudioSamplingRate sampleRate
    )
{
    BAVC_Audio_Info audioInfo;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    handle->sampleRate = sampleRate;
    audioInfo.eAudioSamplingRate = sampleRate;

    BDBG_MSG(("HDMI Audio Sample Rate Change %d", sampleRate));
    if ( handle->tmdsEnabled )
    {
        BHDM_AudioVideoRateChangeCB_isr(handle->hdmHandle, BHDM_Callback_Type_eAudioChange, &audioInfo);
    }
}

/* Returns false if the format is not supported, true if it is */
bool NEXUS_HdmiOutput_GetColorimetry_priv(
    NEXUS_HdmiOutputHandle handle,
    BFMT_VideoFmt format,
    BAVC_MatrixCoefficients *pColorimetry
    )
{
    BERR_Code errCode;
    bool hdmiDevice;
    BHDM_EDID_RxVendorSpecificDB vendorDb;
    BHDM_OutputFormat   eOutputMode ;
    uint8_t supported;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    errCode = BHDM_EDID_IsRxDeviceHdmi(handle->hdmHandle, &vendorDb, &hdmiDevice);
    if ( errCode )
    {
        return false;
    }
    eOutputMode = hdmiDevice ? BHDM_OutputFormat_eHDMIMode : BHDM_OutputFormat_eDVIMode ;

    if (vendorDb.YCbCr444)  /* Support RGB & YCbCr 4:4:4 */
    {
        if (!handle->settings.autoColorSpace) /* User override of default colorspace */
        {
            if (handle->settings.colorSpace == NEXUS_ColorSpace_eRgb)  /* RGB  */
            {
                *pColorimetry = BAVC_MatrixCoefficients_eHdmi_RGB ; 
				goto colorimetry_selected ;
            }
            else if (handle->settings.colorSpace == NEXUS_ColorSpace_eYCbCr422) /* YCbCr 422 */
            {
            	    return false ; /* unsupported */
            }

            /* YCbCr 444: fallthrough and get colorimetry based on format */
        }

        /* autoColorSpace selected OR YCbCr 444 ; get correct colorimetry */
        errCode = BHDM_EDID_GetSupportedColorimetry(
            handle->hdmHandle, eOutputMode, format, pColorimetry) ;
        if ( errCode )
        {
            return false;
        }
    }

    /* RGB only or RGB & YCbCr 4:2:2 (which we don't support). Essentially, this is the same as RGB only capability.
        We need to send format with the supported colorimetry */
    else {
        BDBG_WRN(("Attached Rx device does not support YCbCr 4:4:4. Enable auto Color Space."));
        errCode = BHDM_EDID_GetSupportedColorimetry(
            handle->hdmHandle, eOutputMode, format, pColorimetry);
        if ( errCode )
        {
            return false;
        }
    }
colorimetry_selected:

    errCode = BHDM_EDID_VideoFmtSupported(handle->hdmHandle, format, &supported);
    if ( errCode )
    {
        return false;
    }

    return supported;
}

/* Set the notifyDisplay callback */
void NEXUS_HdmiOutput_SetDisconnectParams_priv(
    NEXUS_HdmiOutputHandle handle,
    bool masterMode,
    const NEXUS_CallbackDesc *notifyDisplay
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    NEXUS_ASSERT_MODULE();
    (void) BHDM_SetHdmiDataTransferMode(handle->hdmHandle, masterMode); 
    NEXUS_TaskCallback_Set(handle->notifyDisplay, notifyDisplay);
}

/* Set the notifyAudioEvent */
void NEXUS_HdmiOutput_SetNotifyAudioEvent_priv(
    NEXUS_HdmiOutputHandle handle,
    BKNI_EventHandle notifyAudioEvent
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    NEXUS_ASSERT_MODULE();  
    handle->notifyAudioEvent = notifyAudioEvent;
}

/* Connect video and set format parameters */
NEXUS_Error NEXUS_HdmiOutput_SetDisplayParams_priv(
    NEXUS_HdmiOutputHandle handle,
    BFMT_VideoFmt format,
    BAVC_MatrixCoefficients colorimetry,
    BFMT_AspectRatio aspectRatio,
    bool masterMode,
    const NEXUS_CallbackDesc *notifyDisplay
    )
{
    BHDM_EDID_RxVendorSpecificDB vendorDb;
    bool hdmiDevice;
    BHDM_Settings hdmSettings;
    BERR_Code errCode;
    BAVC_HDMI_AviInfoFrame hdmAviInfoFrameSettings;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    BHDM_GetHdmiSettings(handle->hdmHandle, &hdmSettings);

    errCode = BHDM_EDID_IsRxDeviceHdmi(handle->hdmHandle, &vendorDb, &hdmiDevice);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    hdmSettings.eOutputFormat = (hdmiDevice)?BHDM_OutputFormat_eHDMIMode:BHDM_OutputFormat_eDVIMode;
    hdmSettings.eAspectRatio = aspectRatio;
    hdmSettings.eInputVideoFmt = format;
    hdmSettings.eColorimetry = colorimetry;

    if (notifyDisplay) {
        NEXUS_TaskCallback_Set(handle->notifyDisplay, notifyDisplay);
    }

    errCode = BHDM_SetHdmiDataTransferMode(handle->hdmHandle, masterMode);
    if (errCode) {
        return BERR_TRACE(errCode);
    }

    BDBG_MSG(("Setting HDMI Video Format to %d", format));

    /* Aspect ratio change only, simply update AVI info frame is enough
        no need to go through BHDM_EnableDisplay */
    if (handle->aspectRatioChangeOnly && handle->tmdsEnabled) 
    {
        /* Save the new aspect ratio in HDMI settings */
        BHDM_SetHdmiSettings(handle->hdmHandle, &hdmSettings);

        /* When updating AVI InfoFrame packets, the aspect ratio from hdmi settings is used,
            not the setting in AviInfoFrameSettings is used. */
        BHDM_GetAVIInfoFramePacket(handle->hdmHandle, &hdmAviInfoFrameSettings);
        BHDM_SetAVIInfoFramePacket(handle->hdmHandle, &hdmAviInfoFrameSettings);
		
        /* Reset aspectRatioChangeOnly flag after checking */
        handle->aspectRatioChangeOnly = false;
    }
    else 
    {
        errCode = BHDM_EnableDisplay(handle->hdmHandle, &hdmSettings);
        handle->tmdsEnabled = true ;	
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
		
        (void) NEXUS_HdmiOutput_SetAudioParams_priv(handle, handle->audioBits,
            handle->sampleRate, handle->audioFormat, handle->audioNumChannels);

        if ( !handle->powerTimer 
        && handle->openSettings.powerPollingInterval )
        {
            handle->powerTimer = 
                NEXUS_ScheduleTimer(handle->openSettings.powerPollingInterval, 
                    NEXUS_HdmiOutput_P_TimerCallback, handle);

            if (!handle->powerTimer)
            {
                return BERR_TRACE(NEXUS_NOT_INITIALIZED) ;
            }
        }
		
    }

    handle->videoConnected = true;
    return BERR_SUCCESS;
}

/* Disconnect video */
NEXUS_Error NEXUS_HdmiOutput_Disconnect_priv(
    NEXUS_HdmiOutputHandle handle
    )
{
    BERR_Code errCode ;
	
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    handle->videoConnected = false;
    NEXUS_TaskCallback_Set(handle->notifyDisplay, NULL);
    errCode = BHDM_DisableDisplay(handle->hdmHandle);
    if (errCode)
        BERR_TRACE(errCode) ;
    handle->tmdsEnabled = false ;

    if ( handle->powerTimer )
    {
        NEXUS_CancelTimer(handle->powerTimer);
        handle->powerTimer = NULL ;
    }


    return errCode ;
}

/* Audio can only start if video is connected */
bool NEXUS_HdmiOutput_VideoIsConnected_priv(
    NEXUS_HdmiOutputHandle handle
    )
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    return handle->videoConnected;
}

/* Sanity checks for the audio module */
bool NEXUS_HdmiOutput_AudioFormatSupported_priv(
    NEXUS_HdmiOutputHandle handle,
    BAVC_AudioFormat format,
    unsigned numChannels    /* PCM only */
    )
{
    BERR_Code errCode;
    BHDM_EDID_AudioDescriptor audioFormats[BAVC_AudioFormat_eMaxCount] ;

    NEXUS_ASSERT_MODULE();
    errCode = BHDM_EDID_GetSupportedAudioFormats(handle->hdmHandle, audioFormats);
    if ( errCode )
    {
        return false;
    }

    if ( !audioFormats[format].Supported )
    {
        return false;
    }
    else if ( format == BAVC_AudioFormat_ePCM )
    {
        /* Hardware can only output stereo or 5.1 currently */
        switch ( numChannels )
        {
        case 2:
        case 6:
            if ( audioFormats[BAVC_AudioFormat_ePCM].AudioChannels < numChannels )
            {
                return false;
            }
            break;
        default:
            /* Not supported */
            return false;
        }
    }

    return true;
}

/* Set audio format information */
NEXUS_Error NEXUS_HdmiOutput_SetAudioParams_priv(
    NEXUS_HdmiOutputHandle handle,
    BAVC_AudioBits bitsPerSample,
    BAVC_AudioSamplingRate sampleRate,
    BAVC_AudioFormat format,
    unsigned numChannels    /* PCM only */
    )
{
    NEXUS_Error errCode;
    BHDM_Settings hdmSettings;
    BAVC_HDMI_AudioInfoFrame audioInfoFrame;

    NEXUS_ASSERT_MODULE();

    handle->sampleRate = sampleRate;
    handle->audioBits = bitsPerSample;
    handle->audioFormat = format;
    handle->audioNumChannels = numChannels;

    BHDM_GetHdmiSettings(handle->hdmHandle, &hdmSettings);

    if ( hdmSettings.eOutputFormat != BHDM_OutputFormat_eHDMIMode )
    {
        BDBG_MSG(("Not an HDMI Device"));
        return BERR_SUCCESS;
    }

    /* Don't enable TMDS on audio changes, only set this if we actually have a change. */
    if ( handle->tmdsEnabled )
    {
        errCode = BHDM_GetAudioInfoFramePacket(handle->hdmHandle, &audioInfoFrame);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        if ( format == BAVC_AudioFormat_ePCM )
        {
            /* PCM should specify the number of PCM channels */
            switch ( numChannels )
            {
            default:
                BDBG_WRN(("Invalid number of PCM channels.  Defaulting to 2"));
                /* coverity[unterminated_default] */
            case 2:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e2Channels;
                break;
            case 3:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e3Channels;
                break;
            case 4:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e4Channels;
                break;
            case 5:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e5Channels;
                break;
            case 6:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e6Channels;
                break;
            case 7:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e7Channels;
                break;
            case 8:
                audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_e8Channels;
                break;
            }
        }
        else
        {
            /* Compressed should always use refer to stream header */
            audioInfoFrame.ChannelCount = BAVC_HDMI_AudioInfoFrame_ChannelCount_eReferToStreamHeader;
        }

        if ( format == BAVC_AudioFormat_ePCM )
        {
            switch ( numChannels )
            {
            case 6:
                audioInfoFrame.SpeakerAllocation = BHDM_ChannelAllocation_e5_1;
                break;
            case 8:
                audioInfoFrame.SpeakerAllocation = BHDM_ChannelAllocation_e3_4_1;
                break;
            default:
                BDBG_WRN(("Unexpected PCM channel count %u", numChannels));
                /* Fall through */
            case 2:
                audioInfoFrame.SpeakerAllocation = BHDM_ChannelAllocation_eStereo;
                break;
            }
        }
        else
        {
            audioInfoFrame.SpeakerAllocation = BHDM_ChannelAllocation_eStereo;
        }
        audioInfoFrame.DownMixInhibit = 0;  /* default */
        audioInfoFrame.LevelShift = 0;  /* default */

        errCode = BHDM_SetAudioInfoFramePacket(handle->hdmHandle, &audioInfoFrame);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        BHDM_GetHdmiSettings(handle->hdmHandle, &hdmSettings);
        hdmSettings.eAudioSamplingRate = sampleRate;
        hdmSettings.eAudioBits = bitsPerSample;
        hdmSettings.eAudioFormat = format ;

        BDBG_MSG(("Enabling display - SR %d", hdmSettings.eAudioSamplingRate));
        errCode = BHDM_EnableDisplay(handle->hdmHandle, &hdmSettings);
        handle->tmdsEnabled = true ;
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        if ( !handle->powerTimer 
        && handle->openSettings.powerPollingInterval )
        {
            handle->powerTimer = 
                NEXUS_ScheduleTimer(handle->openSettings.powerPollingInterval, 
                    NEXUS_HdmiOutput_P_TimerCallback, handle);

            if (!handle->powerTimer)
            {
                return BERR_TRACE(NEXUS_NOT_INITIALIZED) ;
            }
        }
    }
    return NEXUS_SUCCESS;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
bool NEXUS_HdmiOutput_VideoFormatSupported_priv(
    NEXUS_HdmiOutputHandle handle,
    unsigned VideoFormat
    )
{
    BERR_Code errCode;
	uint8_t Supported;

    NEXUS_ASSERT_MODULE();
    errCode = BHDM_EDID_VideoFmtSupported(handle->hdmHandle, VideoFormat, &Supported);
    if ( errCode )
    {
        return false;
    }

    return Supported;
}
#endif


NEXUS_Error NEXUS_HdmiOutput_P_Shutdown(void)
{
    int i;

    for ( i = 0; i < NEXUS_NUM_HDMI_OUTPUTS; i++ )
    {
        if ( g_hdmiOutputs[i].opened )
        {
            BDBG_ERR(("Force closing HDMI output %p", &g_hdmiOutputs[i]));
            NEXUS_HdmiOutput_Close(&g_hdmiOutputs[i]);
        }
    }
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_SetAVMute(NEXUS_HdmiOutputHandle hdmiOutput, bool mute)
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hdmiOutput, NEXUS_HdmiOutput);

    /* store users value */
    hdmiOutput->avMuteSetting = mute;

    mute = mute || hdmiOutput->formatChangeMute;

    /* AvMute is HDMI concept only; no meaning in DVI mode */
    /* check for DVI and use black screen?? vs HDMI (AvMute) */
    errCode = BHDM_SetAvMute(hdmiOutput->hdmHandle, mute) ;
    if (errCode) return BERR_TRACE(errCode);

    return NEXUS_SUCCESS;
}


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
NEXUS_Error NEXUS_HdmiOutput_SetAudioMute(NEXUS_HdmiOutputHandle hdmiOutput, bool mute)
{
	BERR_Code mResult;
	
	mResult = BHDM_SetAudioMute(hdmiOutput->hdmHandle, mute);
	if(mResult != BERR_SUCCESS)
	{
		BDBG_ERR(("BHDM_SetAudioMute failed!\n"));
		return NEXUS_UNKNOWN;
	}

	return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_SetAVBlank(NEXUS_HdmiOutputHandle hdmiOutput, bool mute)
{
    BDBG_OBJECT_ASSERT(hdmiOutput, NEXUS_HdmiOutput);

	BHDM_SetAudioMute(hdmiOutput->hdmHandle, mute);
	BHDM_SetVideoBlank(hdmiOutput->hdmHandle, mute);

    return NEXUS_SUCCESS;
}
#else
NEXUS_Error NEXUS_HdmiOutput_SetAudioMute(NEXUS_HdmiOutputHandle hdmiOutput, bool mute)
{
    return NEXUS_SUCCESS;
}
NEXUS_Error NEXUS_HdmiOutput_SetAVBlank(NEXUS_HdmiOutputHandle hdmiOutput, bool mute)
{
    return NEXUS_SUCCESS;
}
#endif


#if defined(HUMAX_PLATFORM_BASE)
NEXUS_Error NEXUS_HdmiOutput_SetPowerState(NEXUS_HdmiOutputHandle hdmiOutput, bool bPowerOn)
{
	BDBG_OBJECT_ASSERT(hdmiOutput, NEXUS_HdmiOutput);
	hdmiOutput->devicePowered = bPowerOn;
	return NEXUS_SUCCESS;
}
#endif


bool NEXUS_HdmiOutput_VideoFormatSupported(NEXUS_HdmiOutputHandle handle, NEXUS_VideoFormat format)
{
    BERR_Code errCode;
    BFMT_VideoFmt videoFmt;
    uint8_t supported;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    errCode = NEXUS_P_VideoFormat_ToMagnum(format, &videoFmt);
    if (errCode)
        supported = false;

    errCode = BHDM_EDID_VideoFmtSupported(handle->hdmHandle, format, &supported);
    if (errCode)
        supported = false;

    return supported;
}

NEXUS_Error NEXUS_HdmiOutput_P_PreFormatChange_priv(NEXUS_HdmiOutputHandle hdmiOutput, bool aspectRatioChangeOnly)
{
    NEXUS_Error rc;
    NEXUS_HdmiOutputState state;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(hdmiOutput, NEXUS_HdmiOutput);

    hdmiOutput->aspectRatioChangeOnly = aspectRatioChangeOnly;
    if (aspectRatioChangeOnly)
        goto done;

    /* Check for device */
    state = NEXUS_HdmiOutput_P_GetState(hdmiOutput);
    
    BDBG_MSG(("PreFormatChange hdcp? %c; current state: %s", hdmiOutput->hdcpStarted?'y':'n',
        state == NEXUS_HdmiOutputState_eConnected?"connected":"disconnect/unpowered"));
    if (hdmiOutput->hdcpStarted && state == NEXUS_HdmiOutputState_eConnected )
    {
        rc = NEXUS_HdmiOutput_DisableHdcpAuthentication(hdmiOutput);
        if (rc) return BERR_TRACE(rc);

        hdmiOutput->enableHdcpPostFormatChange = true;
    }

    /* send AvMute to HDMI Receiver to minimize video flash/audio pops, etc.
       due to format change, color space, aspect ratio changes, etc. */
    hdmiOutput->formatChangeMute = true;
    rc = NEXUS_HdmiOutput_SetAVMute(hdmiOutput, hdmiOutput->avMuteSetting);
    if (rc) return BERR_TRACE(rc);

    /* Give receiver time to process the AvMute packet  */
    BKNI_Sleep(hdmiOutput->settings.preFormatChangeAvMuteDelay);

done:
    return 0;
}

static void NEXUS_HdmiOutput_P_PostFormatChangeTimer(void *context)
{
    NEXUS_HdmiOutputHandle hdmiOutput = context;
    NEXUS_Error rc;

    BDBG_MSG(("PostFormatChangeTimer"));
    hdmiOutput->postFormatChangeTimer = NULL;
    hdmiOutput->formatChangeMute = false;
    rc = NEXUS_HdmiOutput_SetAVMute(hdmiOutput, hdmiOutput->avMuteSetting);
    if (rc) rc = BERR_TRACE(rc);

    /* restart HDCP authentication */
    if (hdmiOutput->enableHdcpPostFormatChange) {
        hdmiOutput->enableHdcpPostFormatChange = false;
        rc = NEXUS_HdmiOutput_StartHdcpAuthentication(hdmiOutput);
        if (rc) rc = BERR_TRACE(rc);
    }
}

NEXUS_Error NEXUS_HdmiOutput_P_PostFormatChange_priv(NEXUS_HdmiOutputHandle hdmiOutput)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(hdmiOutput, NEXUS_HdmiOutput);
    BDBG_MSG(("PostFormatChange"));

    /* Give receiver time to finish processing format change before unmuting */
    if (hdmiOutput->postFormatChangeTimer) {
        NEXUS_CancelTimer(hdmiOutput->postFormatChangeTimer);
        hdmiOutput->postFormatChangeTimer = NULL;
    }

    hdmiOutput->postFormatChangeTimer = NEXUS_ScheduleTimer(hdmiOutput->settings.postFormatChangeAvMuteDelay, NEXUS_HdmiOutput_P_PostFormatChangeTimer, hdmiOutput);
    return 0;
}

NEXUS_Error NEXUS_HdmiOutput_SetColorDepth_priv(NEXUS_HdmiOutputHandle handle, NEXUS_HdmiColorDepth colorDepth)
{
#if NEXUS_HAS_HDMI_1_3
    BERR_Code rc = BERR_SUCCESS;
    BHDM_ColorDepth_Settings colorDepthSettings;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    colorDepthSettings.eBitsPerPixel = NEXUS_P_HdmiColorDepth_ToMagnum(colorDepth);
    BDBG_MSG(("Deep color mode: %d", colorDepthSettings.eBitsPerPixel));
    rc = BHDM_SetColorDepth(handle->hdmHandle, &colorDepthSettings);
    if (rc) return BERR_TRACE(rc);
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(colorDepth);
#endif

    return NEXUS_SUCCESS;
}

void NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(
    NEXUS_HdmiOutputHandle handle,
     NEXUS_HdmiOutputVendorSpecificInfoFrame *pVendorSpecificInfoFrame)
{
    BAVC_HDMI_VendorSpecificInfoFrame avcInfoFrame;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    BDBG_CASSERT(NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eMax == (NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat)BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eMax);
    BDBG_CASSERT(NEXUS_HdmiVendorSpecificInfoFrame_HDMIVIC_eMax == (NEXUS_HdmiVendorSpecificInfoFrame_HDMIVIC)BAVC_HDMI_VSInfoFrame_HDMIVIC_eMax);
    BDBG_CASSERT(NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eMax == (NEXUS_Video3DStructure)BAVC_HDMI_VSInfoFrame_3DStructure_eMax);
    BDBG_CASSERT(NEXUS_HdmiVendorSpecificInfoFrame_3DExtData_eMax == (NEXUS_Video3DSubSample)BAVC_HDMI_VSInfoFrame_3DExtData_eMax);

    BHDM_GetVendorSpecificInfoFrame(handle->hdmHandle, &avcInfoFrame);

    BKNI_Memset(pVendorSpecificInfoFrame, 0, sizeof(*pVendorSpecificInfoFrame)); /* memset makes it future proof */
    
    pVendorSpecificInfoFrame->ieeeRegId[0] = avcInfoFrame.uIEEE_RegId[0];
    pVendorSpecificInfoFrame->ieeeRegId[1] = avcInfoFrame.uIEEE_RegId[1];
    pVendorSpecificInfoFrame->ieeeRegId[2] = avcInfoFrame.uIEEE_RegId[2];
    pVendorSpecificInfoFrame->hdmiVideoFormat = 
            (NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat) avcInfoFrame.eHdmiVideoFormat;
    pVendorSpecificInfoFrame->hdmiVIC = 
            (NEXUS_HdmiVendorSpecificInfoFrame_HDMIVIC) avcInfoFrame.eHdmiVic;
    pVendorSpecificInfoFrame->hdmi3DStructure = 
            (NEXUS_HdmiVendorSpecificInfoFrame_3DStructure) avcInfoFrame.e3DStructure;
    pVendorSpecificInfoFrame->hdmi3DExtData = 
            (NEXUS_HdmiVendorSpecificInfoFrame_3DExtData) avcInfoFrame.e3DExtData;

    return  ;
}


NEXUS_Error NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputVendorSpecificInfoFrame *pVendorSpecificInfoFrame
    )
{
    BERR_Code rc = BERR_SUCCESS ;
    BAVC_HDMI_VendorSpecificInfoFrame avcInfoFrame;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    /* payload length must not include the IEEE Reg ID or checksum byte  */
    /* i.e. payload length < BAVC_HDMI_PB_LENGTH - 3 byte IEEE RegID - 1 byte checksum */

    avcInfoFrame.uIEEE_RegId[0] = pVendorSpecificInfoFrame->ieeeRegId[0];
    avcInfoFrame.uIEEE_RegId[1] = pVendorSpecificInfoFrame->ieeeRegId[1];
    avcInfoFrame.uIEEE_RegId[2] = pVendorSpecificInfoFrame->ieeeRegId[2]; 
    avcInfoFrame.eHdmiVideoFormat = 
        (BAVC_HDMI_VSInfoFrame_HDMIVideoFormat) pVendorSpecificInfoFrame->hdmiVideoFormat; 
    avcInfoFrame.eHdmiVic = 
        (BAVC_HDMI_VSInfoFrame_HDMIVIC) pVendorSpecificInfoFrame->hdmiVIC;
    avcInfoFrame.e3DStructure = 
        (BAVC_HDMI_VSInfoFrame_3DStructure) pVendorSpecificInfoFrame->hdmi3DStructure;
    avcInfoFrame.e3DExtData = 
        (BAVC_HDMI_VSInfoFrame_3DExtData) pVendorSpecificInfoFrame->hdmi3DExtData;
            
    rc = BHDM_SetVendorSpecificInfoFrame(handle->hdmHandle, &avcInfoFrame);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS ;
}


NEXUS_Error NEXUS_HdmiOutput_GetAviInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiAviInfoFrame *pAviInfoFrame
)
{
    BERR_Code rc = BERR_SUCCESS;
    BAVC_HDMI_AviInfoFrame avcAviInfoFrame;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);   
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_Colorspace) == sizeof(BAVC_HDMI_AviInfoFrame_Colorspace));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ActiveInfo) == sizeof(BAVC_HDMI_AviInfoFrame_ActiveInfo));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_BarInfo) == sizeof(BAVC_HDMI_AviInfoFrame_BarInfo));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_Colorimetry) == sizeof(BAVC_HDMI_AviInfoFrame_Colorimetry));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_PictureAspectRatio) == sizeof(BAVC_HDMI_AviInfoFrame_PictureAspectRatio));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ActiveFormatAspectRatio) == sizeof(BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatio));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_Scaling) == sizeof(BAVC_HDMI_AviInfoFrame_Scaling));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ITContent) == sizeof(BAVC_HDMI_AviInfoFrame_ITContent));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ExtendedColorimetry) == sizeof(BAVC_HDMI_AviInfoFrame_ExtendedColorimetry)); 
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_RGBQuantizationRange) == sizeof(BAVC_HDMI_AviInfoFrame_RGBQuantizationRange));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ContentType) == sizeof(BAVC_HDMI_AviInfoFrame_ContentType));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_YccQuantizationRange) == sizeof(BAVC_HDMI_AviInfoFrame_YccQuantizationRange));
    
    rc = BHDM_GetAVIInfoFramePacket(handle->hdmHandle, &avcAviInfoFrame);
    
    pAviInfoFrame->bOverrideDefaults = avcAviInfoFrame.bOverrideDefaults;
    
    pAviInfoFrame->eColorSpace = 
        (NEXUS_HdmiAviInfoFrame_Colorspace) avcAviInfoFrame.ePixelEncoding;
    pAviInfoFrame->eActiveInfo = 
        (NEXUS_HdmiAviInfoFrame_ActiveInfo) avcAviInfoFrame.eActiveInfo;
    pAviInfoFrame->eBarInfo = 
        (NEXUS_HdmiAviInfoFrame_BarInfo) avcAviInfoFrame.eBarInfo;
    pAviInfoFrame->eScanInfo = 
        (NEXUS_HdmiAviInfoFrame_ScanInfo) avcAviInfoFrame.eScanInfo;
    pAviInfoFrame->eColorimetry= 
        (NEXUS_HdmiAviInfoFrame_Colorimetry) avcAviInfoFrame.eColorimetry;

    pAviInfoFrame->ePictureAspectRatio = 
        (NEXUS_HdmiAviInfoFrame_PictureAspectRatio) avcAviInfoFrame.ePictureAspectRatio;
    pAviInfoFrame->eActiveFormatAspectRatio= 
        (NEXUS_HdmiAviInfoFrame_ActiveFormatAspectRatio) avcAviInfoFrame.eActiveFormatAspectRatio;
    pAviInfoFrame->eScaling= 
        (NEXUS_HdmiAviInfoFrame_Scaling) avcAviInfoFrame.eScaling;
    
    pAviInfoFrame->videoIdCode = avcAviInfoFrame.VideoIdCode;
    pAviInfoFrame->pixelRepeat = avcAviInfoFrame.PixelRepeat;

#if NEXUS_HAS_HDMI_1_3
    pAviInfoFrame->eITContent= 
        (NEXUS_HdmiAviInfoFrame_ITContent) avcAviInfoFrame.eITContent;
    pAviInfoFrame->eExtendedColorimetry= 
        (NEXUS_HdmiAviInfoFrame_ExtendedColorimetry) avcAviInfoFrame.eExtendedColorimetry;
    pAviInfoFrame->eRGBQuantizationRange= 
        (NEXUS_HdmiAviInfoFrame_RGBQuantizationRange) avcAviInfoFrame.eRGBQuantizationRange;
    pAviInfoFrame->eContentType = 
        (NEXUS_HdmiAviInfoFrame_ContentType) avcAviInfoFrame.eContentType;
    pAviInfoFrame->eYccQuantizationRange = 
        (NEXUS_HdmiAviInfoFrame_YccQuantizationRange) avcAviInfoFrame.eYccQuantizationRange;
#endif

    pAviInfoFrame->topBarEndLineNumber = avcAviInfoFrame.TopBarEndLineNumber;
    pAviInfoFrame->bottomBarStartLineNumber = avcAviInfoFrame.BottomBarStartLineNumber;
    pAviInfoFrame->leftBarEndPixelNumber = avcAviInfoFrame.LeftBarEndPixelNumber;
    pAviInfoFrame->rightBarEndPixelNumber = avcAviInfoFrame.RightBarEndPixelNumber;
    
    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_HdmiOutput_SetAviInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiAviInfoFrame *pAviInfoFrame
    )
{
    BERR_Code rc = BERR_SUCCESS;
    BAVC_HDMI_AviInfoFrame avcAviInfoFrame;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
		BHDM_Settings hdmSettings;	
#endif

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);   
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_Colorspace) == sizeof(BAVC_HDMI_AviInfoFrame_Colorspace));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ActiveInfo) == sizeof(BAVC_HDMI_AviInfoFrame_ActiveInfo));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_BarInfo) == sizeof(BAVC_HDMI_AviInfoFrame_BarInfo));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_Colorimetry) == sizeof(BAVC_HDMI_AviInfoFrame_Colorimetry));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_PictureAspectRatio) == sizeof(BAVC_HDMI_AviInfoFrame_PictureAspectRatio));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ActiveFormatAspectRatio) == sizeof(BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatio));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_Scaling) == sizeof(BAVC_HDMI_AviInfoFrame_Scaling));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ITContent) == sizeof(BAVC_HDMI_AviInfoFrame_ITContent));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ExtendedColorimetry) == sizeof(BAVC_HDMI_AviInfoFrame_ExtendedColorimetry)); 
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_RGBQuantizationRange) == sizeof(BAVC_HDMI_AviInfoFrame_RGBQuantizationRange));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_ContentType) == sizeof(BAVC_HDMI_AviInfoFrame_ContentType));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAviInfoFrame_YccQuantizationRange) == sizeof(BAVC_HDMI_AviInfoFrame_YccQuantizationRange));


    /* If set, all settings are to be used in generating AVI InfoFrame. Else, some of the fields will be 
        automatically generated instead (e.g. color space, colorimetry, etc.) */
    avcAviInfoFrame.bOverrideDefaults = pAviInfoFrame->bOverrideDefaults;

    avcAviInfoFrame.ePixelEncoding = 
        (BAVC_HDMI_AviInfoFrame_Colorspace) pAviInfoFrame->eColorSpace;
    avcAviInfoFrame.eActiveInfo = 
        (BAVC_HDMI_AviInfoFrame_ActiveInfo) pAviInfoFrame->eActiveInfo;
    avcAviInfoFrame.eBarInfo = 
        (BAVC_HDMI_AviInfoFrame_BarInfo) pAviInfoFrame->eBarInfo;
    avcAviInfoFrame.eScanInfo = 
        (BAVC_HDMI_AviInfoFrame_ScanInfo) pAviInfoFrame->eScanInfo;
    avcAviInfoFrame.eColorimetry = 
        (BAVC_HDMI_AviInfoFrame_Colorimetry) pAviInfoFrame->eColorimetry;
    
    avcAviInfoFrame.ePictureAspectRatio =   
        (BAVC_HDMI_AviInfoFrame_PictureAspectRatio) pAviInfoFrame->ePictureAspectRatio;

#if defined(HUMAX_PLATFORM_BASE)&& defined(CONFIG_HDMI_REFACTORING)
		BHDM_GetHdmiSettings(handle->hdmHandle, &hdmSettings);
		if(pAviInfoFrame->ePictureAspectRatio == 0) /* No Data */
		{
			hdmSettings.eAspectRatio = BFMT_AspectRatio_eUnknown;
		}
		if(pAviInfoFrame->ePictureAspectRatio == 1) /*4:3*/
		{
			hdmSettings.eAspectRatio = BFMT_AspectRatio_e4_3;
		}
		else /*16:9*/
		{
			hdmSettings.eAspectRatio = BFMT_AspectRatio_e16_9;
		}
		/* Save the new aspect ratio in HDMI settings */
		BHDM_SetHdmiSettings(handle->hdmHandle, &hdmSettings); 
#endif

    avcAviInfoFrame.eActiveFormatAspectRatio = 
        (BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatio) pAviInfoFrame->eActiveFormatAspectRatio;

    avcAviInfoFrame.eScaling = 
        (BAVC_HDMI_AviInfoFrame_Scaling) pAviInfoFrame->eScaling;
    
    avcAviInfoFrame.VideoIdCode = pAviInfoFrame->videoIdCode;
    avcAviInfoFrame.PixelRepeat = pAviInfoFrame->pixelRepeat;

#if NEXUS_HAS_HDMI_1_3
    avcAviInfoFrame.eITContent = 
        (BAVC_HDMI_AviInfoFrame_ITContent) pAviInfoFrame->eITContent;
    avcAviInfoFrame.eExtendedColorimetry =
        (BAVC_HDMI_AviInfoFrame_ExtendedColorimetry) pAviInfoFrame->eExtendedColorimetry;
    avcAviInfoFrame.eRGBQuantizationRange = 
        (BAVC_HDMI_AviInfoFrame_RGBQuantizationRange) pAviInfoFrame->eRGBQuantizationRange;
    avcAviInfoFrame.eContentType =
        (BAVC_HDMI_AviInfoFrame_ContentType) pAviInfoFrame->eContentType;
    avcAviInfoFrame.eYccQuantizationRange =
        (BAVC_HDMI_AviInfoFrame_YccQuantizationRange) pAviInfoFrame->eYccQuantizationRange;
#endif

    avcAviInfoFrame.TopBarEndLineNumber = pAviInfoFrame->topBarEndLineNumber;
    avcAviInfoFrame.BottomBarStartLineNumber = pAviInfoFrame->bottomBarStartLineNumber;
    avcAviInfoFrame.LeftBarEndPixelNumber = pAviInfoFrame->leftBarEndPixelNumber;
    avcAviInfoFrame.RightBarEndPixelNumber = pAviInfoFrame->rightBarEndPixelNumber;

    rc = BHDM_SetAVIInfoFramePacket(handle->hdmHandle, &avcAviInfoFrame);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_GetAudioInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiAudioInfoFrame *pAudioInfoFrame
)
{
    BERR_Code rc = BERR_SUCCESS;
    BAVC_HDMI_AudioInfoFrame avcAudioInfoFrame;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);   
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_ChannelCount) == sizeof(BAVC_HDMI_AudioInfoFrame_ChannelCount));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_CodingType) == sizeof(BAVC_HDMI_AudioInfoFrame_CodingType));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_SampleSize) == sizeof(BAVC_HDMI_AudioInfoFrame_SampleSize));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_SampleFrequency) == sizeof(BAVC_HDMI_AudioInfoFrame_SampleFrequency));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_DownMixInhibit) == sizeof(BAVC_HDMI_AudioInfoFrame_DownMixInhibit));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_LevelShift) == sizeof(BAVC_HDMI_AudioInfoFrame_LevelShift));   

    rc = BHDM_GetAudioInfoFramePacket(handle->hdmHandle, &avcAudioInfoFrame);
    if (rc) return BERR_TRACE(rc);

    pAudioInfoFrame->bOverrideDefaults = avcAudioInfoFrame.bOverrideDefaults;
    pAudioInfoFrame->eChannelCount = 
        (NEXUS_HdmiAudioInfoFrame_ChannelCount) avcAudioInfoFrame.ChannelCount;
    pAudioInfoFrame->eCodingType = 
        (NEXUS_HdmiAudioInfoFrame_CodingType) avcAudioInfoFrame.CodingType;
    pAudioInfoFrame->eSampleSize = 
        (NEXUS_HdmiAudioInfoFrame_SampleSize) avcAudioInfoFrame.SampleSize;
    pAudioInfoFrame->eSampleFrequency = 
        (NEXUS_HdmiAudioInfoFrame_SampleFrequency) avcAudioInfoFrame.SampleFrequency;

    pAudioInfoFrame->speakerAllocation = avcAudioInfoFrame.SpeakerAllocation;
    pAudioInfoFrame->eDownMixInhibit = 
        (NEXUS_HdmiAudioInfoFrame_DownMixInhibit) avcAudioInfoFrame.DownMixInhibit;
    pAudioInfoFrame->eLevelShift= 
        (NEXUS_HdmiAudioInfoFrame_LevelShift) avcAudioInfoFrame.LevelShift;
        
    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_HdmiOutput_SetAudioInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiAudioInfoFrame *pAudioInfoFrame
)
{
    BERR_Code rc = BERR_SUCCESS;
    BAVC_HDMI_AudioInfoFrame avcAudioInfoFrame;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);   
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_ChannelCount) == sizeof(BAVC_HDMI_AudioInfoFrame_ChannelCount));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_CodingType) == sizeof(BAVC_HDMI_AudioInfoFrame_CodingType));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_SampleSize) == sizeof(BAVC_HDMI_AudioInfoFrame_SampleSize));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_SampleFrequency) == sizeof(BAVC_HDMI_AudioInfoFrame_SampleFrequency));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_DownMixInhibit) == sizeof(BAVC_HDMI_AudioInfoFrame_DownMixInhibit));
    BDBG_CASSERT(sizeof(NEXUS_HdmiAudioInfoFrame_LevelShift) == sizeof(BAVC_HDMI_AudioInfoFrame_LevelShift));   


    avcAudioInfoFrame.bOverrideDefaults = pAudioInfoFrame->bOverrideDefaults;
    avcAudioInfoFrame.ChannelCount =
        (BAVC_HDMI_AudioInfoFrame_ChannelCount) pAudioInfoFrame->eChannelCount;
    avcAudioInfoFrame.CodingType =
        (BAVC_HDMI_AudioInfoFrame_CodingType) pAudioInfoFrame->eCodingType;
    avcAudioInfoFrame.SampleSize =
        (BAVC_HDMI_AudioInfoFrame_SampleSize) pAudioInfoFrame->eSampleSize;
    avcAudioInfoFrame.SampleFrequency =
        (BAVC_HDMI_AudioInfoFrame_SampleFrequency) pAudioInfoFrame->eSampleFrequency;
    
    avcAudioInfoFrame.SpeakerAllocation = pAudioInfoFrame->speakerAllocation;
    avcAudioInfoFrame.DownMixInhibit=
        (BAVC_HDMI_AudioInfoFrame_DownMixInhibit) pAudioInfoFrame->eDownMixInhibit;
    avcAudioInfoFrame.LevelShift=
        (BAVC_HDMI_AudioInfoFrame_LevelShift) pAudioInfoFrame->eLevelShift;

    rc = BHDM_SetAudioInfoFramePacket(handle->hdmHandle, &avcAudioInfoFrame);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_HdmiOutput_SetTmdsSignal(
    NEXUS_HdmiOutputHandle handle,
    bool tmdsEnable)
{
    BERR_Code rc = BERR_SUCCESS ;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    rc = BHDM_EnableTmdsOutput(handle->hdmHandle, tmdsEnable) ;
    handle->tmdsEnabled = tmdsEnable;

    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS ;
}

void NEXUS_HdmiOutput_GetTmdsSignal(
    NEXUS_HdmiOutputHandle handle,
    bool *pTmdsEnable)
{

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    *pTmdsEnable = handle->tmdsEnabled ;
    return;
}

NEXUS_Error NEXUS_HdmiOutput_GetSupportedVideoInfo(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputSupportedVideoInfo *pSupportedVideoInfo
)
{
    BERR_Code rc = BERR_SUCCESS;
    BHDM_EDID_VideoDescriptorInfo stHdmSupportedVideoInfo ;
    int i=0;

    BKNI_Memset(pSupportedVideoInfo, 0, sizeof(*pSupportedVideoInfo));

    rc = BHDM_EDID_GetSupportedVideoInfo(handle->hdmHandle, &stHdmSupportedVideoInfo);
    if (rc) return BERR_TRACE(rc);

    pSupportedVideoInfo->numSupportedVideoDescriptors = stHdmSupportedVideoInfo.numDescriptors;

    /* ensure we don't overrun the nexus struct */
#define MAX_NUM_SUPPORTED_VIDEO_DESC (sizeof(pSupportedVideoInfo->supportedVideoIDCode)/sizeof(pSupportedVideoInfo->supportedVideoIDCode[0]))

    if (pSupportedVideoInfo->numSupportedVideoDescriptors > MAX_NUM_SUPPORTED_VIDEO_DESC) {
        pSupportedVideoInfo->numSupportedVideoDescriptors = MAX_NUM_SUPPORTED_VIDEO_DESC;
    }

    for (i=0; i < pSupportedVideoInfo->numSupportedVideoDescriptors; i++) 
    {
        pSupportedVideoInfo->supportedVideoIDCode[i] =
            stHdmSupportedVideoInfo.VideoIDCode[i] ;
    }

    return NEXUS_SUCCESS;
}



#if (BCHP_CHIP == 7400) || (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) \
    || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7408) \
    || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7550)
#define NEXUS_HDMI_OUTPUT_65NM_PLATFORMS 1
#else
#define NEXUS_HDMI_OUTPUT_40NM_PLATFORMS 1
#endif

NEXUS_Error NEXUS_HdmiOutput_GetPreEmphasisConfiguration(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputPreEmphasisConfiguration *pPreEmphasisConfiguration
)
{
    BERR_Code rc = BERR_SUCCESS ;
    BHDM_PreEmphasis_Configuration stHdmPreEmphasisConfiguration;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    BKNI_Memset(&stHdmPreEmphasisConfiguration, 0, sizeof(BHDM_PreEmphasis_Configuration));

    rc = BHDM_GetPreEmphasisConfiguration(handle->hdmHandle, &stHdmPreEmphasisConfiguration);
    if (rc) return BERR_TRACE(rc);

    /* retrieve current pre-emphasis configurations */
    pPreEmphasisConfiguration->preEmphasis_CK = stHdmPreEmphasisConfiguration.uiPreEmphasis_CK;
    pPreEmphasisConfiguration->preEmphasis_Ch0 = stHdmPreEmphasisConfiguration.uiPreEmphasis_Ch0;
    pPreEmphasisConfiguration->preEmphasis_Ch1 = stHdmPreEmphasisConfiguration.uiPreEmphasis_Ch1;
    pPreEmphasisConfiguration->preEmphasis_Ch2 = stHdmPreEmphasisConfiguration.uiPreEmphasis_Ch2;

#if NEXUS_HDMI_OUTPUT_65NM_PLATFORMS
    pPreEmphasisConfiguration->driverAmp = stHdmPreEmphasisConfiguration.uiDriverAmp;
    pPreEmphasisConfiguration->refCntl = stHdmPreEmphasisConfiguration.uiRefCntl;
    pPreEmphasisConfiguration->termR = stHdmPreEmphasisConfiguration.uiTermR;
#endif

#if NEXUS_HDMI_OUTPUT_40NM_PLATFORMS
    pPreEmphasisConfiguration->hfEn = stHdmPreEmphasisConfiguration.uiHfEn;
    pPreEmphasisConfiguration->currentRatioSel = stHdmPreEmphasisConfiguration.uiCurrentRatioSel;
    pPreEmphasisConfiguration->kP = stHdmPreEmphasisConfiguration.uiKP;
    pPreEmphasisConfiguration->kI = stHdmPreEmphasisConfiguration.uiKI;
    pPreEmphasisConfiguration->kA = stHdmPreEmphasisConfiguration.uiKA;
#endif  

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_HdmiOutput_SetPreEmphasisConfiguration(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputPreEmphasisConfiguration *pPreEmphasisConfiguration
)
{
    BERR_Code rc = BERR_SUCCESS ;
    BHDM_PreEmphasis_Configuration stHdmPreEmphasisConfiguration;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    
    rc = BHDM_GetPreEmphasisConfiguration(handle->hdmHandle, &stHdmPreEmphasisConfiguration);
    if (rc) return BERR_TRACE(rc);

    /* Set specified pre emphasis configurations */
    stHdmPreEmphasisConfiguration.uiPreEmphasis_CK = pPreEmphasisConfiguration->preEmphasis_CK;
    stHdmPreEmphasisConfiguration.uiPreEmphasis_Ch0 = pPreEmphasisConfiguration->preEmphasis_Ch0;
    stHdmPreEmphasisConfiguration.uiPreEmphasis_Ch1 = pPreEmphasisConfiguration->preEmphasis_Ch1;
    stHdmPreEmphasisConfiguration.uiPreEmphasis_Ch2 = pPreEmphasisConfiguration->preEmphasis_Ch2;

#if NEXUS_HDMI_OUTPUT_65NM_PLATFORMS
    stHdmPreEmphasisConfiguration.uiDriverAmp = pPreEmphasisConfiguration->driverAmp;
    stHdmPreEmphasisConfiguration.uiRefCntl = pPreEmphasisConfiguration->refCntl;
    stHdmPreEmphasisConfiguration.uiTermR = pPreEmphasisConfiguration->termR;
#endif
    
#if NEXUS_HDMI_OUTPUT_40NM_PLATFORMS
    stHdmPreEmphasisConfiguration.uiHfEn = pPreEmphasisConfiguration->hfEn;
    stHdmPreEmphasisConfiguration.uiCurrentRatioSel = pPreEmphasisConfiguration->currentRatioSel;
    stHdmPreEmphasisConfiguration.uiKP = pPreEmphasisConfiguration->kP;
    stHdmPreEmphasisConfiguration.uiKI = pPreEmphasisConfiguration->kI;
    stHdmPreEmphasisConfiguration.uiKA = pPreEmphasisConfiguration->kA;
#endif  

    rc = BHDM_SetPreEmphasisConfiguration(handle->hdmHandle, &stHdmPreEmphasisConfiguration);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_GetCrcData( NEXUS_HdmiOutputHandle handle, NEXUS_HdmiOutputCrcData *pData, unsigned numEntries, unsigned *pNumEntriesReturned )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    *pNumEntriesReturned = 0;
    if (pData == NULL) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return 0;
    }
    /* no critical section needed for this type of producer/consumer */
    while (*pNumEntriesReturned < numEntries && handle->crc.wptr != handle->crc.rptr && handle->crc.queue) {
        pData[*pNumEntriesReturned] = handle->crc.queue[handle->crc.rptr];
        (*pNumEntriesReturned)++;
        if (++handle->crc.rptr == handle->crc.size) {
            handle->crc.rptr = 0;
        }
    }
    return 0;
}
    
void NEXUS_HdmiOutput_P_Vsync_isr( void *context )
{
    NEXUS_HdmiOutputHandle handle = context;
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    if (handle->crc.queue) {
        NEXUS_HdmiOutputCrcData *pData;
        BERR_Code rc;
        BHDM_CrcData crc;
        
        rc = BHDM_GetCrcValue_isr(handle->hdmHandle, &crc);
        if (rc) return;
        
        pData = &handle->crc.queue[handle->crc.wptr];
        pData->crc = crc.crc;
        if (++handle->crc.wptr == handle->crc.size) {
            handle->crc.wptr = 0;
        }
        if (handle->crc.wptr == handle->crc.rptr) {
            BDBG_WRN(("HDMI Tx CRC overflow"));
        }
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
NEXUS_Error NEXUS_HdmiOutput_SetAVInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    unsigned int AspectRatio,
    unsigned int ActiveFormat) 
{
    BERR_Code rc = BERR_SUCCESS;
    BAVC_HDMI_AviInfoFrame avInfoFrame;
#if defined(HUMAX_PLATFORM_BASE) 
    BHDM_Settings hdmSettings;	
#endif
	
    rc = BHDM_GetAVIInfoFramePacket(handle->hdmHandle, &avInfoFrame);
    if(rc != BERR_SUCCESS)
    {
         return NEXUS_UNKNOWN;   
    }
	
#if defined(HUMAX_PLATFORM_BASE)
    BHDM_GetHdmiSettings(handle->hdmHandle, &hdmSettings);
	if(AspectRatio == 0) /* No Data */
	{
		hdmSettings.eAspectRatio = BFMT_AspectRatio_eUnknown;
	}
    if(AspectRatio == 1) /*4:3*/
    {
        hdmSettings.eAspectRatio = BFMT_AspectRatio_e4_3;
    }
    else /*16:9*/
    {
        hdmSettings.eAspectRatio = BFMT_AspectRatio_e16_9;
    }
    /* Save the new aspect ratio in HDMI settings */
    BHDM_SetHdmiSettings(handle->hdmHandle, &hdmSettings); 
#endif

    avInfoFrame.ePictureAspectRatio = AspectRatio;
    avInfoFrame.eActiveFormatAspectRatio = ActiveFormat;
    BHDM_SetAVIInfoFramePacket(handle->hdmHandle, &avInfoFrame);
    if(rc != BERR_SUCCESS)
    {
         return NEXUS_UNKNOWN;   
    }
    return NEXUS_SUCCESS;
}
#else
NEXUS_Error NEXUS_HdmiOutput_SetAVInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    unsigned int AspectRatio,
    unsigned int ActiveFormat)
{
	return NEXUS_SUCCESS;
}
#endif

#endif /* #if NEXUS_NUM_HDMI_OUTPUTS */


