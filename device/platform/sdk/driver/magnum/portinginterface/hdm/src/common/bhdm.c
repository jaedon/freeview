/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bhdm.c $
* $brcm_Revision: Hydra_Software_Devel/355 $
* $brcm_Date: 10/24/12 12:38p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/hdm/src/common/bhdm.c $
* 
* Hydra_Software_Devel/376   8/2/13 2:20p rgreen
* SW7429-710: Some 40nm chips require swapping of the HDMI_TX_PHY channel
* settings
* 
* Hydra_Software_Devel/355   10/24/12 12:38p rgreen
* SW7408-349: Fix bug in BHDM_GetReceiverSense return value.  return
* success when TMDS is disabled; RxSense will be false
* 
* Hydra_Software_Devel/354   10/2/12 3:28p rgreen
* SW7425-3907, SW7405-5557: Show RxSense ON/OFF message only when
* DEBUG_RSEN is enabled.  Report HDMI Mode and Format for debugging
* 
* Hydra_Software_Devel/353   9/27/12 2:41p vle
* SW7425-3829: Fix coverity issue
* 
* Hydra_Software_Devel/352   9/25/12 6:12p rgreen
* SW7425-3907: When TMDS output has been disabled, do not set hotplug
* event due to change in state of RxSense detection
* 
* Hydra_Software_Devel/351   9/24/12 1:50p rgreen
* SW7429-273: Disable all interrupts prior to stanby; re-enable on resume
* 
* Hydra_Software_Devel/350   9/20/12 3:26p vle
* SW7445-23: Add HDM PI for 7445
* 
* Hydra_Software_Devel/349   9/11/12 4:15p rgreen
* SW7435-306: Make the compilation of the new analog alternative BFMT
* formats configurable.  Will allow compilation of latest HDM with older
* reference code.  By default the formats are enabled
* 
* Hydra_Software_Devel/348   9/10/12 1:42p rgreen
* SW7420-2349: Update BHDM_P_RxDeviceAttaced to isr function.  Update
* calls to add Enter/Leave Critical Section
* 
* Hydra_Software_Devel/347   8/24/12 5:00p rgreen
* SW7425-3803: Add Phy PowerDown for standby/close
* 
* Hydra_Software_Devel/346   8/15/12 1:09p rgreen
* SW7425-3735: Resolve coverity issues
* 
* Hydra_Software_Devel/345   8/14/12 6:00p rgreen
* SW7435-306: Translate alternative NTSC,480p formats (480+ lines) to
* digital formats (exactly 480 lines).
* 
* Hydra_Software_Devel/344   8/10/12 11:45a rgreen
* SW7408-309,SW7418-113: Set HotPlug event for RxSense changes. Use
* private RxDeviceAttached functions; remove use of static Remove unused
* RxSense code (RxSense is returned as ON or OFF)
* 
* Hydra_Software_Devel/343   8/7/12 11:08a cng
* SW7429-204, SW7425-2989: Enable format detection timer only when a
* device is attached. During resume, configure hdmi output if a device
* was connected during standby.
*
* Hydra_Software_Devel/342   7/30/12 5:46p rgreen
* SW7346-926: Skip FIFO centering algorithm  when in Master Mode
* 
* Hydra_Software_Devel/341   7/20/12 3:27p vle
* SW7346-923: Reverse changes. Do not remove unused power management
* TX_CEC reference since that will affect HDMI video output.
* 
* Hydra_Software_Devel/340   7/17/12 4:49p rgreen
* SW7346-923: Destroy any created timers when entering standby; remove
* unused power management TX_CEC reference
* 
* Hydra_Software_Devel/339   7/16/12 2:48p rgreen
* SW7425-2989: Format Change Detection available in 40nm and newer chips.
* Disable for earlier chips
* 
* Hydra_Software_Devel/338   7/12/12 10:29p rgreen
* SW7425-2989: restore 65nm deleted declaration
* 
* Hydra_Software_Devel/337   7/12/12 5:13p rgreen
* SW7425-2989: Fix dual hotplug interrupt compilation
* 
* Hydra_Software_Devel/336   7/12/12 1:23p rgreen
* SW7425-2989: Merge changes
* 
* Hydra_Software_Devel/SW7425-2989/2   7/10/12 6:06p rgreen
* SW7425-2989: Fix DestroyTimer for same timer.
* 
* Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:16p rgreen
* SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
* 
* Hydra_Software_Devel/335   6/29/12 3:33p vle
* SW7425-2851: fix coverity error
* 
* Hydra_Software_Devel/334   6/20/12 2:35p rgreen
* SW7358-263, SW7425-3206: Use polling to detect RxSense state instead of
* interrupts
* 
* Hydra_Software_Devel/333   6/14/12 11:34p rgreen
* SW7425-3206, SW7425-3241: Fix 65nm register reference for
* HDMI_TX_PHY_RESET_CTL
* 
* Hydra_Software_Devel/332   6/14/12 10:44a rgreen
* SW7425-3206, SW7425-3241:  Enable/Disable RSEN interrupts to match
* state of the TMDS lines
* 
* Hydra_Software_Devel/331   6/1/12 3:32p vle
* SW7425-3126: Add support for 720p@24Hz and 720p@30Hz 3D frame packing
* format. Populate VIC information for 3D formats.
* 
* Hydra_Software_Devel/330   5/22/12 8:17p rgreen
* SW7335-1383,SW7425-2075: Fix MAI Channel ordering for 65nm. Move MAI
* Channel Ordering to isr time when notified of audio changes
* 
* Hydra_Software_Devel/329   5/11/12 4:54p rgreen
* SW7425-3039:  Modify RxSense interrupt to notify of changes between OFF
* and ON.  Clock only (standby) or Clock+Data are treated the same
* (powered) and do not invoke a callback when changing between these
* states
* 
* Hydra_Software_Devel/328   5/8/12 6:43a rgreen
* SW7422-467,SW7231-797,SW7358-263: Fix RDB name for 65nm used to disable
* TMDS clock when closing display
* 
* Hydra_Software_Devel/327   5/7/12 7:53p rgreen
* SW7422-467,SW7231-797,SW7358-263: Merge changes
* 
* Hydra_Software_Devel/SW7422-467/2   5/7/12 6:14p rgreen
* SW7422-467,SW7231-797,SW7358-263:   Disable TMDS Clock line in
* BHDM_Display; Clock must be left on when attached Rx is in standby or
* disconnected so RSEN change can be detected.  Disable TMDS Clock and
* data lines when closing device
* 
* Hydra_Software_Devel/SW7422-467/1   5/2/12 9:54p rgreen
* SW7422-467,SW7231-797,SW7358-263: Remove enabling of TMDS lines at
* BHDM_Open, enable when data is first output.   When disabling TMDS
* lines, keep RSEN interrupt enabled and the TMDS Clock enabled for
* detecting Receiver Power changes
* 
* Hydra_Software_Devel/326   5/4/12 3:39p vle
* SW7422-467: turn off Rx Sense interrupt debug message by default.
* 
* Hydra_Software_Devel/325   5/3/12 6:25p rgreen
* SW7425-2957, SW7420-2293: SW7425-2737: Recenter FIFO only if there is a
* video format change
* 
* Hydra_Software_Devel/324   5/1/12 11:06a vle
* SW7425-2666: Fix coverity issue and a mistake in the previous merge.
* 
* Hydra_Software_Devel/323   4/30/12 4:22p vle
* SW7425-2666: Merge to mainline
* 
* Hydra_Software_Devel/322   4/25/12 2:26p rgreen
* SW7422-468,SW7552-163: Remove enable of TMDS lines from
* GetReceiverSense_isr.  app/higher level should set as needed
* 
* Hydra_Software_Devel/321   4/24/12 4:29p rgreen
* SW7422-469: Configure phy before enabling TMDS lines
* 
* Hydra_Software_Devel/320   4/24/12 2:31p rgreen
* SW7425-2075: Merge Changes
* 
* Hydra_Software_Devel/SW7425-2075/2   4/20/12 1:29p gskerl
* SW7425-2075: Merged updates from main branch.
* 
* Hydra_Software_Devel/SW7425-2666_FifoTest/1   4/19/12 10:24a rgreen
* SW7425-2666:  Update FIFO recenter algorithm
* 
* Hydra_Software_Devel/318   4/16/12 2:48p vle
* SW7346-737: Merge to mainline
* 
* Hydra_Software_Devel/SW7346-737/1   4/12/12 4:21p vle
* SW7346-737: Initial attempt adding CRC support in HDM
* 
* Hydra_Software_Devel/SW7425-2075/1   4/12/12 11:58a jgarrett
* SW7425-2075: Removing MAI Channel Reordering in HBR mode
* 
* Hydra_Software_Devel/317   4/3/12 11:20a rgreen
* SW7425-2818,SW7552-163:  Always keep TMDS clock channel enabled to
* ensure detecting ReceiverSense if attached Rx powers down just the
* data channels when in standby
* 
* Hydra_Software_Devel/316   3/23/12 4:12p rgreen
* SW7405-5557: Remove informative warning start-up message; report only
* in the case of an error
* 
* Hydra_Software_Devel/315   3/22/12 4:04p rgreen
* SW7425-2666: Update algorithm for checking for valid video into the
* HDMI core
* 
* Hydra_Software_Devel/314   2/29/12 4:05p rgreen
* SW7125-1146,SW7408-317,SW7425-2518: Fix RxSense status callback value
* to use RxSense status vs HotPlug status
* 
* Hydra_Software_Devel/313   2/29/12 1:36p rgreen
* SW7425-2515,SW7425-2518: Remove unused CEC code which is now located in
* CEC pi;  Fix RxSense debug message.
* 
* Hydra_Software_Devel/312   2/23/12 10:49a rgreen
* SW7125-1146,SW7408-317: Merge changes
* 
* Hydra_Software_Devel/SW7408-317/2   2/22/12 1:14p rgreen
* SW7125-1146,SW7408-317: Remove enabling of TMDS at hotplug interrupt,
* let higher level enable TMDS when ready
* 
* Hydra_Software_Devel/SW7408-317/1   2/21/12 6:32p rgreen
* SW7125-1146,SW7408-317: Treat RxSense and HP events separately.  Update
* processing of both events
* 
* Hydra_Software_Devel/311   2/17/12 4:09p rgreen
* SW7125-1146,SW7408-317:  leave TMDS enabled after Rx is connected.
* Some devices require time to allow the RxSense to be read; Force
* reading RxSense when GetReceiverSense is called
* 
* Hydra_Software_Devel/310   2/10/12 9:49a rgreen
* SW7231-627: Set bForceEnableDisplay to false after BHDM_EnableDisplay
* 
* Hydra_Software_Devel/309   2/9/12 3:59p rgreen
* SW7231-345,SW7125-1146,SW7425-2361: Refactor HDMI Power Management;
* separate TMDS power from clock
* 
* Hydra_Software_Devel/308   2/8/12 2:23p rgreen
* SW7231-345,SW7125-1146: Merge Changes
* 
* Hydra_Software_Devel/SW7125-1146/6   2/7/12 9:09a rgreen
* SW7231-345,SW7125-1146: On Resume, always return success if Rx device
* has been disconneced.  Fixes forced assert for BHDM_EnableDisplay
* 
* Hydra_Software_Devel/SW7125-1146/5   2/3/12 5:20p rgreen
* SW7231-345,SW7125-1146:  Do not enable/disable TMDS lines when in power
* management mode;
* 
* Hydra_Software_Devel/SW7125-1146/4   2/3/12 3:12p rgreen
* SW7231-345,SW7125-1146: Check Receiver Sense whenever a RxDevice is
* connected.  Leave TMDS enabled if Rx is powered; Remove isr RxSense
* check in GetReceiverSense call... use state variable instead
* 
* Hydra_Software_Devel/SW7125-1146/3   2/1/12 2:04p rgreen
* SW7231-345,SW7125-1146:  Enable/Disable RSEN interrupt in EnableDisplay
* and DisableDisplay calls to prevent isr call when registers are
* powered down (crash).
* 
* Hydra_Software_Devel/307   1/27/12 2:10p vle
* SW7125-1146: merge to mainline
* 
* Hydra_Software_Devel/SW7125-1146/2   1/26/12 5:35p vle
* SW7125-1146: Get RSEN setting at isr vs event time for applicable
* platforms
* 
* Hydra_Software_Devel/306   1/23/12 11:26a rgreen
* SW7125-1146: Merge Changes
* 
* Hydra_Software_Devel/SW7125-1146/1   1/19/12 2:34p rgreen
* SW7125-1146: Enable TMDS at open to fix interrupt issue; Get RSEN
* setting at isr vs event time; Add debug configurations for RSEN amd
* TMDS settings
* 
* Hydra_Software_Devel/305   1/6/12 6:02p vle
* SW7435-11: implement support for check/clearHotplugInterrupt for 7435
* 
* Hydra_Software_Devel/304   1/6/12 2:58p vle
* SW7435-11: Add support for 7435
* 
* Hydra_Software_Devel/303   1/4/12 3:30p rgreen
* SW7405-5004: Use BAVC_HDMI_xxx macros vs BHDM_xxx macros; use bool
* return types in BHDM_P_HdmiSettingsChange vs integer type
* 
* Hydra_Software_Devel/302   12/20/11 2:03p rgreen
* SW7425-2037:  Force reconfigure of display after TMDS lines are
* disabled/re-enabled
* 
* Hydra_Software_Devel/301   12/12/11 3:31p rgreen
* SW7405-4677:  Merge changes
* 
* Hydra_Software_Devel/SW7405-4677/1   11/22/11 3:31p rgreen
* SW7405-4677: Adjust HDMI transition timeout based on video format
* 
* Hydra_Software_Devel/300   12/2/11 4:53p jtna
* SW7425-1708: fix standby wakeup. the if logic was flipped
* 
* Hydra_Software_Devel/299   11/30/11 11:21a rgreen
* SW7405-5004: Merge changes
* 
* Hydra_Software_Devel/SW7425-1140/3   11/30/11 11:14a rgreen
* SW7405-5004: Replace BKNI_Memcmp for structure with field by field
* comparison
* 
* Hydra_Software_Devel/298   11/22/11 6:02p vle
* SW7425-1140: Merge to mainline.  Remove all CEC functionality out of
* HDM PI.
* 
* Hydra_Software_Devel/SW7425-1140/2   11/22/11 5:47p vle
* SW7425-1140: Add BHDM_CONFIG_CEC_LEGACY_SUPPORT for backward compatible
* for CEC legacy platforms.
* 
* Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:16p vle
* SW7425-1140: Remove all CEC functionalities out of HDM PI
* 
* Hydra_Software_Devel/297   11/14/11 2:15p rgreen
* SW7425-1710: Update BHDM_CONFIG macro usage.  Describe specific
* functionality vs chip process
* 
* Hydra_Software_Devel/296   11/4/11 4:34p rgreen
* SW7125-1128,SW7125-1138:  Use register based General Control Packet
* only when attached receiver supports Deep Color
* 
* Hydra_Software_Devel/295   10/18/11 1:06p vle
* SWBLURAY-26139: Ensure CEC_ADDR is set to UNREGISTER address (0xF) by
* default.
* 
* Hydra_Software_Devel/294   10/11/11 4:50p vle
* SW7429-5: Add support for 7429.
* 
* Hydra_Software_Devel/293   9/15/11 4:27p rgreen
* SW7425-1296: Set SPD Default Descrption to Reference Board vs Reference
* Sw
* 
* Hydra_Software_Devel/292   9/14/11 3:17p rgreen
* SW7420-2059: Remove macro date/time macros in Open
* 
* Hydra_Software_Devel/291   9/8/11 4:55p rgreen
* SW7425-610: Fix enum mapping for 1280x1024
* 
* Hydra_Software_Devel/290   9/8/11 3:56p rgreen
* SW7425-610,SW7325-664: Add support for 1280x1024; Add more descriptive
* message when BFMT format is not supported;  Initialize AVI InfoFrame
* Bar Info to invalid
* 
* Hydra_Software_Devel/289   8/17/11 6:37p vle
* SW7231-322: Add support for 7231B0
* 
* Hydra_Software_Devel/288   7/9/11 2:48a vle
* SW7422-460: Fix build errors on older 65nm platforms (7405/7335, etc.)
* 
* Hydra_Software_Devel/287   7/1/11 10:48a vle
* SW7422-460: merge bdvd changes to mainline
* 
* Hydra_Software_Devel/bdvd_v4.0/2   6/30/11 2:40p rbshah
* SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
* 
* Hydra_Software_Devel/bdvd_v4.0/1   6/28/11 5:31p rbshah
* SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
* 
* Hydra_Software_Devel/286   6/28/11 4:29p vle
* SW7346-234: Merge to mainline
* 
* Hydra_Software_Devel/SW7346-234/1   6/24/11 6:07p vle
* SW7346-234: Disable RSEN interrupt until the PHY is powered on.
* 
* Hydra_Software_Devel/285   6/7/11 6:44p vle
* SW7425-532: Add HDMI CEC support for 40nm chip. Use correct CEC
* interrupt.
* 
* Hydra_Software_Devel/284   5/20/11 4:06p vle
* SW7550-696: Reset CEC logical address when HPD disconnect
* 
* Hydra_Software_Devel/283   2/17/11 7:38p jtna
* SW7420-1141: rework HDMI power management. break public API to allow
* for StandbySettings
* 
* Hydra_Software_Devel/282   1/11/11 4:38p jtna
* SW7420-1377: remove unnecessary power management code for DVO support
* 
* Hydra_Software_Devel/281   12/21/10 3:27p vle
* SW7422-130: Merge to main branch
* 
* Hydra_Software_Devel/SW7422-130/4   12/21/10 2:10p vle
* SW7422-130: Update VSPO parameters
* 
* Hydra_Software_Devel/SW7422-130/3   12/20/10 7:45p vle
* SW7422-130: When configure HDMI in master mode, do not RECENTER fifo
* and USE_FULL has to be set to 0
* 
* Hydra_Software_Devel/280   12/16/10 5:07p vle
* SW7422-130: merge to main branch
* 
* Hydra_Software_Devel/SW7422-130/2   12/9/10 6:49p vle
* SW7422-130: Update parameters for 1080p@30_3DOU format
* 
* Hydra_Software_Devel/SW7422-130/1   12/8/10 7:26p vle
* SW7422-130: Add support for 3DTV formats.
* 
* Hydra_Software_Devel/279   12/9/10 6:01p vle
* SW7420-1177: Add DVO support for 7420
* 
* Hydra_Software_Devel/278	 12/8/10 5:07p rgreen
* SW7572-215: Update default Audio Infoframe Channel Count to 2 channels
* (value of 1).  Make use of enumerations instead of integers.
* 
* Hydra_Software_Devel/278	 12/8/10 4:56p rgreen
* SW7572-215: Update default Audio Infoframe Channel Count to 2 channels
* (CC=1).  Make use of enumerations instead of integers
* 
* Hydra_Software_Devel/277   11/29/10 4:13p vle
* SW7422-97: Fix compiler warning
* 
* Hydra_Software_Devel/276   11/24/10 11:12a vle
* SW7422-97: Update HDMI PHY Settings for 40nm core.
* 
* Hydra_Software_Devel/275   11/19/10 5:02p vle
* SW7422-23: RSEN interrupt is available in 40nm core.
* 
* Hydra_Software_Devel/274   10/20/10 6:48p jtna
* SW7420-972: added standby/resume functions. removed references to
* 108MHz BCHP_PWR node.
* 
* Hydra_Software_Devel/273   10/7/10 6:47p jtna
* SW7420-972: merge hdmi power management
* 
* Hydra_Software_Devel/SW7420-972/1   10/6/10 7:00p jtna
* SW7420-972: BCHP_PWR power management for HDM
* 
* Hydra_Software_Devel/272   9/29/10 4:13p vle
* SW7422-23: Fix build issues without CEC enable
* 
* Hydra_Software_Devel/271   9/28/10 7:18p vle
* SW7422-23: Refactor HDMI code to isolate platform dependent code
* furthermore. Add support for 7422 and other 40nm platforms.
* 
* Hydra_Software_Devel/270   9/28/10 12:20p agin
* SWNOOS-425:  Fix GHS compiler error.
* 
* Hydra_Software_Devel/269   9/22/10 7:10p vle
* SW7405-4782: Merge to main branch
* 
* Hydra_Software_Devel/SW7405-4782/1   9/10/10 8:43p vle
* SW7405-4782: Changing aspect ratio does not require HDCP re-
* authentication.
* 
* Hydra_Software_Devel/268   9/7/10 7:20p vle
* SW7420-1054: isr calls need to be protected by a critical section
* 
* Hydra_Software_Devel/267   6/28/10 6:59p vle
* SW7208-59: Add API to control the HDMI data transfer mode, master or
* slave
* 
* Hydra_Software_Devel/266   5/13/10 5:18p vle
* SW7420-579: Fix build error if enable DEBUG_AVI_INFOFRAME debug build.
* 
* Hydra_Software_Devel/265   5/5/10 10:35a vle
* SW7420-294: PAL_M should be treating as 480i, not 576i
* 
* Hydra_Software_Devel/264   4/19/10 12:36p vle
* SW7420-686: Update misleading error message.
* 
* Hydra_Software_Devel/263   4/15/10 7:05p vle
* SW7420-543: fix pixel repetition (2x, 4x) for 480p
* 
* Hydra_Software_Devel/263   4/15/10 7:04p vle
* SW7420-543: fix pixel repetition (2x, 4x) on 480p
* 
* Hydra_Software_Devel/262   4/14/10 7:19p rgreen
* SW7405-3997: Fix creation of IEEE RegID for VSI transmission: SW7405-
* 3997:
* 
* Hydra_Software_Devel/261   4/12/10 3:30p vle
* SW7325-716, SW7405-4195: missing condition compile for platforms
* without 1080p 50/60hz support.
* 
* Hydra_Software_Devel/260   4/2/10 6:40p vle
* SW7601-172: Rename to clearly indicate SetGamutMetadataPacket is a
* private API.
* 
* Hydra_Software_Devel/259   3/29/10 3:06p vle
* SW7335-685: Fix DRIFT FIFO under flow/ over flow when authenticate with
* DVI receivers.
* 
* Hydra_Software_Devel/258   3/26/10 4:33p vle
* SW7601-172: Merge xvYCC support with Gamut Metadata Packet transmission
* from bdvd branch
* 
* Hydra_Software_Devel/257	 3/16/10 6:45p vle
* SW3548-2820: Initialized default setting for hdmi 3D structure setting
* 
* Hydra_Software_Devel/256   2/26/10 5:17p vle
* SW7405-3016: Remove software i2c settings from HDM PI. This mode is
* configure in I2C module
* 
* Hydra_Software_Devel/255   2/23/10 12:48a vle
* SW7420-579: Refactor HDMI PI.
* 
* Hydra_Software_Devel/31   1/18/10 6:47p vle
* SW7342-129: Add BHDM_GetPreEmphasisConfiguration and
* BHDM_SetPreEmphasisConfiguration implementations.
* 
* Hydra_Software_Devel/30   1/15/10 3:34p vle
* SW7401-4355: Add support for Vendor Specific Infoframe
* 
* Hydra_Software_Devel/29   1/8/10 5:19p vle
* SW7405-3740: Port changes to 7420, 7468, and other platforms.
* Add isr callback to provide immediate notifcation of HP changes vs
* waiting for event processing
* 
* Hydra_Software_Devel/28   1/7/10 5:10p vle
* SW7420-536: Add method to use RAM packets to generate GCP Packets for
* various platforms.
* 
* Hydra_Software_Devel/27   1/6/10 4:38p vle
* SW3548-2670: Centralize all CEC timing configurations
* 
* Hydra_Software_Devel/26   10/1/09 11:38p agin
* SWNOOS-375: Fixed compiler error.
* 
* Hydra_Software_Devel/25   9/22/09 1:31p vle
* SW7601-164: Take out the sanity check in BHDM_SetColorDepth because
* this is a bug found by LG, PR16484 in DVD projects
* 
* Hydra_Software_Devel/24   9/21/09 6:13p vle
* SW7601-164: merge in changes in bdvd_v3.0 branch.
* 
* Hydra_Software_Devel/bdvd_v3.0/7   9/21/09 3:01p rbshah
* PR16772[DVD]:[ see HiDef-DVD bug tracking system for more info ].  The
* ability to have the app. get/set the N value for HW-CTS was
* added as precautionary measure. Appears it's not needed anymore
* so taking it out.
* 
* Hydra_Software_Devel/23   9/18/09 3:40p vle
* SW7601-164: Merge fixes from DVD projects (bdvd_v3.0 branch) into main
* Hydra branch.
* 
* Hydra_Software_Devel/bdvd_v3.0/6   9/14/09 12:54p rbshah
* PR16484[DVD]:[ see HiDef-DVD bug tracking system for more info ].
* Removed unnecessary (and erroneous) cross checking of TV's capability
* and our configuration of the desired color-space (YCC vs. RGB).
* 
* Hydra_Software_Devel/bdvd_v3.0/5   9/11/09 4:04p rbshah
* PR16468[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Back
* out MAI bus channel assignment change made on Hydra_Software. It
* was made for 7400 but breaks HBR on 76xx. Refer to Digital Video GNATs
* PR_48151 for more.
* 
* Hydra_Software_Devel/bdvd_v3.0/4   9/2/09 3:31p rbshah
* PR16468[DVD]:[ see HiDef-DVD bug tracking system for more info ].
* Merged with the latest portinginterface/hdm and syslib/hdcplib files.
* 
* Hydra_Software_Devel/bdvd_v3.0/3   8/18/09 1:22p rbshah
* PR_16257[DVD]:[ see HiDef-DVD bug tracking system for more info ].
* This only takes care of HDMI.
* 
* Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/4   7/27/09 2:53p rbshah
* PR_15746[DVD]:[ see HiDef-DVD bug tracking system for more info ].
* Merge fix from Hydra_Software (version 20). Also addresses PR15748.
* Credit goes to Ian L. and Mark P. for chasing this down with Vincent Y.
* 
* Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/3   7/14/09 1:30p rbshah
* PR_15413[DVD]:[ see HiDef-DVD bug tracking system for more info ].
* Merge work from the HDMI certification branch. Also addresses PR15437
* and PR15220 (merged from v2.0). Plus coverity PR15782.
* 
* Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/bdvd_hdmi_cert_v2.1/1   7/8/09 12:18p rbshah
* Various fixes for Auto Hardware Ri,Pj checking. I2C changes are
* temporary. Switch from software to hardware Ri checking by default.
* 
* Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/2   4/13/09 7:28p etrudeau
* PR_13071[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Add
* support for HDMI Consumer Electronic Control (CEC) for remote control
* of player and other interaction with HDMI devices.
* To disable, export pe_disable_cec=y
* 
* Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/1   4/3/09 6:12p rbshah
* PR_13071[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Add
* CEC support at the BDVD/Display API. This is really a back port
* from bdvd_v2.0 and dev_pr13071 branches.
* 
* Hydra_Software_Devel/bdvd_v3.0/2   8/12/09 5:01p rbshah
* PR_15295[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Fix
* race between VDC and HDMI. The PHY PLL programming was being
* done in two places (for Dune) and should only be in one place.
* 
* Hydra_Software_Devel/22   9/1/09 2:00p vle
* SW7630-17: Add support for Grain 7630
* 
* Hydra_Software_Devel/21   8/26/09 3:41p vle
* SW7405-2670:
* Add implementation of interrupt based receiver sense
* 
* Hydra_Software_Devel/20   7/24/09 11:17a vle
* PR56954: Fix HDMI audio channel mapping.
* 
* Hydra_Software_Devel/19   7/22/09 7:35p vle
* PR56776: Prevent HDCP An Timeout
* 
* Hydra_Software_Devel/18   7/14/09 4:04p vle
* PR56606: Fix coverity issue.
* 
* Hydra_Software_Devel/17   7/13/09 11:42a vle
* PR53475: downgrade CEC interrupt message to warning instead of error.
* 
* Hydra_Software_Devel/16   6/25/09 3:04p vle
* PR56132, PR48151:
* Add support for Get HDMI Vendor Specific Info Frame
* Correct channel assignment for PCM 5.1
* 
* Hydra_Software_Devel/15   5/12/09 2:16a vle
* PR 54851: Implement a dynamic mode in the PI where HDCP 1.1 feature
* will be disabled when a pre-define occurance of Pj Link Failures is
* met.
* 
* Hydra_Software_Devel/14   5/11/09 11:51a vle
* PR52978: Add support for 7635
* 
* Hydra_Software_Devel/bdvd_v3.0/1   5/1/09 9:29a rbshah
* PR_14471[DVD]:[ see HiDef-DVD bug tracking system for more info ].
* Bring HDMI to life. Audio is muted unconditionally all the time to
* avoid noisy output (given the chip bug).
* 
* Qucik verification of HDCP and all supported video resolutions.
* 
* Reviewer: Narasimha.
* 
* Hydra_Software_Devel/13   4/8/09 11:52a vle
* PR52978: Add support for 7635
* 
* Hydra_Software_Devel/12   3/11/09 6:29p vle
* PR52873: Merge changes from bdvd_v2.0 branch to Hydra branch
* Ensure we don't miss a hot-plug pulse whenever we're waiting.
* Ensure HDMI input/drift FIFO is initialized and re-centered, disable
* GCP packet generation when connect to DVI monitors.
* Updated all the Audio CRP N-CTS values (both SW and HW CTS modes)
* 
* Hydra_Software_Devel/bdvd_v2.0/21   3/9/09 1:58p rbshah
* PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Updated
* all the Audio CRP N-CTS tables with those generated by the HW
* folks (Vincent Y. and Chris P.). We now have "official" values for both
* HW and SW CTS modes.
* 
* Also modified the Display/HDMI API so application can only set N values
* related to HW CTS. In other words, prior API for updating SW CTS values
* was removed.
* 
* Tested all audio sample rates (except 176.4KHz) at all color depths and
* all relevant pixel clock rates using both HW and CW CTS with the Astro.
* 
* Hydra_Software_Devel/bdvd_v2.0/17   2/11/09 11:30a rbshah
* PR_13097 [ see HiDef-DVD bug tracking system for more info ].  Ensure
* HDMI input/drift FIFO is initialized and re-centered when we're
* connected to DVI monitors.
* 
* Also ensure GCP packet generation is disabled when we're in DVI mode.
* 
* Hydra_Software_Devel/bdvd_v2.0/12   1/29/09 3:41p rbshah
* PR_12826 [ see HiDef-DVD bug tracking system for more info ].  Ensure
* we don't miss a hot-plug pulse whenever we're waiting (in
* the PI as well as BDVD). Also removed an unnecessary wait before
* starting HDCP.
* 
* Hydra_Software_Devel/11   3/9/09 3:20p vle
* PR50570, PR50918, PR49277, PR49652, PR52873:
* Add API to mute/unmute audio, update pixel repitition support, add
* SetPixelDataOverride API for transmission of black video. Merge
* changes/updates from bdvd_v2.0 to main branch.
* 
* Hydra_Software_Devel/10   3/3/09 8:23p vle
* PR50569: Add HW Ri/Pj checking feature. Merged from bdvd branch after
* Rajul's testing effort.
* 
* Hydra_Software_Devel/9   2/17/09 6:48p vle
* PR 51414, PR 45656: Leave HDMI PHY settings to VDC for 7420 platform.
* Update call back debug message
* 
* Hydra_Software_Devel/bdvd_v2.0/11   1/27/09 4:18p rbshah
* PR_12826 [ see HiDef-DVD bug tracking system for more info ].  Add
* logic to wait for stable video to arrive from VEC before we
* start the HDCP authentication process.
* 
* This checkin may not solve the Simplay failure at hand, but it's
* something that needs to be done anyway.
* 
* Hydra_Software_Devel/bdvd_v2.0/10   1/23/09 10:14a rbshah
* PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Checkin
* code drop from Anthony Le for Auto Ri,Pj feature in the
* 7601B0 (Digital Video PR50569).
* 
* This is disabled by default and will be turned on once it has
* been tested and soaked.
* 
* Did verify the A0 build!
*  
* Hydra_Software_Devel/bdvd_v2.0/9   1/21/09 5:57p rbshah
* PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Fix A0
* build broken with my checkin earlier for adding the HDMI
* audio mute functionality.
* 
* Hydra_Software_Devel/bdvd_v2.0/8   1/21/09 11:56a rbshah
* PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Enhance
* Display/HDMI API to allow application to mute/unmute just
* audio (Digital Video PR50570).
* 
* This feature is only available on the 7601B0.
* 
* Hydra_Software_Devel/bdvd_v2.0/7   1/20/09 5:06p rbshah
* PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Take 2:
* need to shift the 8-bit color component values to 12-bits.
* 
* Hydra_Software_Devel/bdvd_v2.0/6   1/20/09 4:02p rbshah
* PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Use new
* B0 feature to transmit black video correctly in YCC color
* space [Digital Video PR49652].
* 
* Hydra_Software_Devel/bdvd_v2.0/5   1/16/09 10:08a rbshah
* PR_12698 [ GCP Packet error happen in ATC test ].  Fix GCP Subpacket
* programming one more time.
* 
* Hydra_Software_Devel/bdvd_v2.0/4   1/14/09 3:41p rbshah
* PR_10346 [ 7601 HDMI: uod bring up. ].  Continue to iron out wrinkles
* (including regressions) with deep color
* and pixel repetition support. Following specific fixes:
* 
* -- Use 4 to indicate 24-bit color in GCP only if deep color receiver
* -- Corrected Pixel Repeat bits for 480i and 576 (2x,4x) in AVI
* Infoframe
* -- Updated HDMI PHY settings for 480i/576i 4x, 36-bit color [162MHz]
* -- Simulate VDC rate change for pixel repeat and deep color changes
* 
* Tested all pixel repeat and deep color combinations with Astro as well
* as with Onkyo.
* 
* Hydra_Software_Devel/8   1/12/09 7:30p vle
* PR49277: Merge to main branch
* Add pixel repetition support
* 
* Hydra_Software_Devel/7   1/12/09 1:49p vle
* PR50918: Update HDMI PHY settings for 1080p deep color mode.
* 
* Hydra_Software_Devel/PR49277/2   1/12/09 7:18p vle
* PR49277: Update to add pixel repetition support.
* 
* Hydra_Software_Devel/bdvd_v2.0/3   1/12/09 5:18p rbshah
* PR_12066 [ Enable pixel repetition to allow HBR audio at lower video
* resolutions ].  Wrap up pixel repetition support at bdvd api. Quick
* verification of
* 480p and 576p 2x, 4x and 480i 4x at all color depths.
* 
* Hydra_Software_Devel/bdvd_v2.0/2   1/9/09 5:57p rbshah
* PR_12066 [ Enable pixel repetition to allow HBR audio at lower video
* resolutions ].  Re-work VDC HDMI RM table and lookup. Also absorb
* pixel repetition
* support in BHDM from Anthony Le.
* 
* Still work in progress.
* 
* Hydra_Software_Devel/PR49277/1   1/8/09 4:30p vle
* PR49277: Add pixel repetition support for 480p
* 
* Hydra_Software_Devel/6   12/23/08 7:33p vle
* PR50081: Merge to main branch.
* Fix to save the color depth settings.
* Also update to use new GCP configuration scheme for AVMute
* Add deep color support
* 
* Hydra_Software_Devel/PR50081/3   12/18/08 3:23p vle
* PR50081: Fix to save the color depth settings.
* Also update to use new GCP configuration scheme for AVMute and update
* settings to address multi channel PCM issue on 480p.
* 
* Hydra_Software_Devel/PR50081/2   12/11/08 2:18p vle
* PR50081: Add deep color support for 7601.
* 
* Hydra_Software_Devel/PR50081/1   12/8/08 5:30p vle
* PR50081: add deep color support.
* 
* Hydra_Software_Devel/5   12/2/08 8:02p vle
* PR48987: Add setting to enable I2C bit bang mode.
* 
* Hydra_Software_Devel/4   12/2/08 11:21a vle
* PR49651: Fix CEC compiling issue for 7601/7420
* PR44535: Add settings to allow user to always force enable display even
* with the same HDMI settings.
* 
* Hydra_Software_Devel/3   10/28/08 10:35p vle
* PR44535: Update CTS programming using Vincent latest calculated values.
* Merge changes from branch bdvd v2.0
* 
* Hydra_Software_Devel/2   10/9/08 4:35p vle
* PR44535: Remove unused variable.
* 
* Hydra_Software_Devel/1   10/9/08 4:06p vle
* PR44535: Merge to main branch support for 7601
* 
* Hydra_Software_Devel/PR44535/5   10/2/08 8:46p vle
* PR44535: Update CTS , N-value programming after confirming with KK and
* Martin for various audio sampling rates.
* 
* Hydra_Software_Devel/PR44535/4   9/24/08 1:46p vle
* PR44535: Update HDMI_TX_PHY_CTL_1 and HDMI_TX_PHY_CTL_2 settings
* 
* Hydra_Software_Devel/PR44535/3   9/17/08 6:18p vle
* PR44535: CTS settings should be updated for video rate change callback
* only. Additional FIFO_CTL settings required.
* 
* Hydra_Software_Devel/PR44535/2   9/12/08 2:54p vle
* PR44535: Update CTS settings. Add 1080p support.
* 
* Hydra_Software_Devel/PR44535/bdvd_v2.0/1   9/5/08 4:57p rbshah
* PR_10374 [ Adding support for 7601 ].  Initial checkin for HDMI
* support. Not functional yet, but app_hdmi now
* compiles and runs. EDID is operational. Video support requires a few
* VDC tweaks that will come shortly (next day or two).
* 
* Some of the BHDM changes provided by Anthony Le.
* 
* Hydra_Software_Devel/PR44535/1   8/6/08 7:47p vle
* PR44535: Initial version
* 
***************************************************************************/

#include "bstd.h"

#include "bhdm.h"
#include "bhdm_priv.h"
#include "bhdm_hdcp.h"

#if BHDM_CONFIG_BTMR_SUPPORT
#include "bhdm_debug.h"
#endif

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(BHDM) ;


#define	BHDM_CHECK_RC( rc, func )	          \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)



#if defined(BHDM_CONFIG_DVO_SUPPORT)
#define DVO_PORT 1
#define HDMI_PORT 0
#else
#define DVO_PORT 0
#define HDMI_PORT 1
#endif


/******************************************************************************
Summary:
Enumerated Type of the input display formats for the HDMI core.  These are
indices to the BHDM_P_InputVideoFmt table. See BHDM_SupportedVideoFormats.
*******************************************************************************/
typedef enum
{
	BHDM_InputVideoFmt_ePowerUp = 0, /* Invalid Power Up Format */
	BHDM_InputVideoFmt_e640x480p,    /* Safe Mode */
	BHDM_InputVideoFmt_e1080i,
	BHDM_InputVideoFmt_e480p,
	BHDM_InputVideoFmt_e480i,
	BHDM_InputVideoFmt_e720p,
	BHDM_InputVideoFmt_e720p_24Hz,
	BHDM_InputVideoFmt_e1080p_24Hz,
	BHDM_InputVideoFmt_e1080p_25Hz,
	BHDM_InputVideoFmt_e1080p_30Hz,

#if BHDM_CONFIG_1080P_5060HZ_SUPPORT
	BHDM_InputVideoFmt_e1080p,
	BHDM_InputVideoFmt_e1080p_50Hz,	
#endif

	BHDM_InputVideoFmt_e1080i_50Hz,
	BHDM_InputVideoFmt_e720p_50Hz,
	BHDM_InputVideoFmt_e576p_50Hz,
	
	BHDM_InputVideoFmt_e576i_50Hz,
	
	BHDM_InputVideoFmt_e720p_3DOU,
	BHDM_InputVideoFmt_e720p_50Hz_3DOU,
	BHDM_InputVideoFmt_e720p_24Hz_3DOU,
	BHDM_InputVideoFmt_e720p_30Hz_3DOU,
	BHDM_InputVideoFmt_e1080p_24Hz_3DOU,
	BHDM_InputVideoFmt_e1080p_30Hz_3DOU,

#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	BHDM_InputVideoFmt_e1080p_24Hz_3D,
	BHDM_InputVideoFmt_e720p_3D,
	BHDM_InputVideoFmt_e720p_50Hz_3D,
#endif
	BHDM_InputVideoFmt_eCustom,
	
	BHDM_InputVideoFmt_800x600p,                 
	BHDM_InputVideoFmt_1024x768p,                
	BHDM_InputVideoFmt_1280x768p,                
	BHDM_InputVideoFmt_1280x1024p,                
	BHDM_InputVideoFmt_1280x720p_50Hz,
	BHDM_InputVideoFmt_1280x720p
} BHDM_P_InputVideoFmt ;


/******************************************************************************
Summary:
HDMI Format Structure; 
*******************************************************************************/
typedef enum _BHDM_P_POLARITY_
{
	BHDM_POLARITY_eNEGATIVE,
	BHDM_POLARITY_ePOSITIVE
} BHDM_P_POLARITY ;


typedef struct _BHDM_P_DISPLAY_FORMAT_
{
	uint8_t    FormatName[20] ;
	uint16_t H_ActivePixels ;
	BHDM_P_POLARITY H_Polarity ;
	BHDM_P_POLARITY V_Polarity ;

	uint16_t H_FrontPorch ;
	uint16_t H_SyncPulse ;
	uint16_t H_BackPorch ;

	uint16_t V_ActiveLinesField0 ;
	uint16_t V_FrontPorchField0 ;
	uint16_t V_SyncPulseField0 ;

	uint16_t V_BackPorchField0 ;
	uint16_t V_SyncPulseOffsetField0 ;

	uint16_t V_ActiveLinesField1 ;
	uint16_t V_FrontPorchField1 ;
	uint16_t V_SyncPulseField1 ;

	uint16_t V_BackPorchField1 ;
	uint16_t V_SyncPulseOffsetField1 ;

} BHDM_P_DISPLAY_FORMAT_DEF ;



/******************************************************************************
Summary:
HDMI Format Definitions (1080i, 720p, etc)
*******************************************************************************/
static const BHDM_P_DISPLAY_FORMAT_DEF BHDM_VideoFmtParams[] =
{
	/* 
	Name         
	H_ActivePixels      HorizPolarity       VertPolarity
	HorizFP 	 HorizSyncPulse      HorizBackPorch  
	VertActiveLinesField0 VertFrontPorchField0 VertSyncPulseField0  VertBackPorchField0 VertSyncPulseOffsetField0  
	VertActiveLinesField1 VertFrontPorchField1 VertSyncPulseField1  VertBackPorchField1 VertSyncPulseOffsetField1  
	*/

	{ /* Invalid Format - Power Up Values */
	"PowerUp ",  0, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE,
	0,	 0,	 0,	 
	0,	 0,	 0,	 0,	 0,
	0,	 0,	 0,	 0,	 0
	},

	{ /* 640 x 480 p	*/
	"640x480p",  640, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE,
	16,  96,  48, 
	480, 10,   2,  33,  0,                      
	480, 10,   2,  33,  0
	},
	
	{ /* 1920 x 1080 i	*/
	"1080i   ",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	88,   44, 148, 
	540,   2,   5,  15, 0, 
	540,   2,   5,  16, 1100,
	},

	{ /* 720 x 480 p	*/
	"480p    ",  720, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE, 
	16,   62, 60,	
	480,   9,  6,  30, 0, 
	480,   9,  6,  30, 0,
	},

	{ /* 720 x 480 i	            */
	"480i    ",  1440, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE, 
	38,  124, 114, 
	240,   4,   3,  15,  0, 
	240,   4,   3,  16,  858,
	},

	{ /* 1280 x 720 p	            */
	"720p    ",  1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	110,  40, 220, 
	720,   5,   5,  20,  0, 
	720,   5,   5,  20,  0,
	},
	
	{ /* 1280 x 720 p	@ 24            */
	"720p@24 ",  1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	2585,  40, 220, 
	720,   5,   5,  20,  0, 
	720,   5,   5,  20,  0,
	},

	{ /* 1920 x 1080 p	 @ 23.976/24 */
	"1080p@24",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	638,   44, 148, 
	1080,   4,   5,  36, 0, 
	1080,   4,   5,  36, 0,
	},
	
	{ /* 1920 x 1080p	@ 25*/
	"1080p@25",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	528,   44, 148, 
	1080,   4,   5,  36, 0, 
	1080,   4,   5,  36, 0,
	},
	
	{ /* 1920 x 1080p	@ 30 - See CEA-861B Errata document */
	"1080p@30",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	88,     44, 148, 
	1080,   4,   5,  36, 0, 
	1080,   4,   5,  36, 0,
	},

#if BHDM_CONFIG_1080P_5060HZ_SUPPORT
	{ /* 1920 x 1080 p	 @ 59.94/60 */
	"1080p",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	88,   44, 148, 
	1080,   4,   5,  36, 0, 
	1080,   4,   5,  36, 0,
	},
#endif
	
	/**************** 50 Hz Formats ****************/	
	/* 
	Name  	H_ActivePixels      HorizPolarity       VertPolarity
	HorizFP 	 HorizSyncPulse      HorizBackPorch  
	VertActiveLinesField0 VertFrontPorchField0 VertSyncPulseField0  VertBackPorchField0 VertSyncPulseOffsetField0  
	VertActiveLinesField1 VertFrontPorchField1 VertSyncPulseField1  VertBackPorchField1 VertSyncPulseOffsetField1  
	*/

#if	BHDM_CONFIG_1080P_5060HZ_SUPPORT
	{ /* 1920 x 1080 p	 @ 50 */
	"1080p@50",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	528,   44, 148, 
	1080,	4,	 5,  36, 0, 
	1080,	4,	 5,  36, 0,
	},
#endif

	{ /* 1920 x 1080 i	*/
	"1080i@50", 1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	528,  44, 148, 
	540,   2,   5,  15, 0,  
	540,   2,   5,  16, 1320,
	},
	
	{ /* 1280 x 720 p	            */
	"720p@50 ", 1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	440,  40, 220, 
	720,   5,   5,  20,  0, 
	720,   5,   5,  20,  0,
	},

	{ /* 720 x 576 p	            */
	"576p@50 ", 720, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE, 
	12,  64,  68, 
	576,  5,   5,  39,  0, 
	576,  5,   5,  39,  0,
	},

	{ /* 720 x 576 i	            */
	#if defined(HUMAX_PLATFORM_BASE)/*patch from broadcom */
	"576i@50 ", 1440, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE, 
	#else
	"576i@50 ", 1440, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	#endif
	24,  126,  138, 
	288,  2,   3,  19,  0, 
	288,  2,   3,  20,  864,
	},

	/************
	* 3D formats */
	/* 
	Name	H_ActivePixels		HorizPolarity		VertPolarity
	HorizFP 	 HorizSyncPulse 	 HorizBackPorch  
	VertActiveLinesField0 VertFrontPorchField0 VertSyncPulseField0	VertBackPorchField0 VertSyncPulseOffsetField0  
	VertActiveLinesField1 VertFrontPorchField1 VertSyncPulseField1	VertBackPorchField1 VertSyncPulseOffsetField1  
	*/
	
	{ /* 720p@60Hz__3DOU				*/
	"720p@60__3DOU ",  1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	110,  40, 220, 
	1470,	5,	 5,  20,  0, 
	1470,	5,	 5,  20,  0,
	},

	{ /* 720p@50Hz__3DOU		*/
	"720p@50__3DOU ", 1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	440,  40, 220, 
	1470,	5,	5,	20,  0, 
	1470,	5,	5,	20,  0,
	},

	{ /* 720p@24Hz__3DOU				*/
	"720p@24__3DOU ",  1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	2585,  40, 220, 
	1470,	5,	 5,  20,  0, 
	1470,	5,	 5,  20,  0,
	},

	{ /* 720p@30Hz__3DOU		*/
	"720p@30__3DOU ", 1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	440,  40, 220, 
	1470,	5,	5,	20,  0, 
	1470,	5,	5,	20,  0,
	},

	{ /* 1080p@23.976/24Hz__3DOU */
	"1080p@24__3DOU ",	1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	638,   44, 148, 
	2205,	4,	 5,  36, 0, 
	2205,	4,	 5,  36, 0,
	},
	
	{ /* 1080p@30__3DOU */
	"1080p@30__3DOU",  1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,	
	88, 	44, 148, 
	2205,	4,	 5,  36, 0, 
	2205,	4,	 5,  36, 0,
	},
	
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	{ /* 1920 x 1080 p	 @ 24 3D */
	"1080p@24 3D", 1920, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,
	638,   44, 148,
	2205,   4,   5,  36, 0,
	2205,   4,   5,  36, 0,
	},

	{ /* 1280 x 720 p	 @ 60 3D */
	"720p@60 3D ", 1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,
	110,   40, 220,
	1470,   5,   5,  20, 0,
	1470,   5,   5,  20, 0,
	},

	{ /* 1280 x 720 p	 @ 50 3D */
	"720p@50 3D ", 1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE,
	440,   40, 220,
	1470,   5,   5,  20, 0,
	1470,   5,   5,  20, 0,
	},
#endif

	{ /* Custom Place Holder         */
	"Custom  ", 0, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	0,  0,  0, 
	0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,
	},
	
	/* 
	Name    H_ActivePixels      HorizPolarity       VertPolarity
	HorizFP 	 HorizSyncPulse      HorizBackPorch  
	VertActiveLinesField0 VertFrontPorchField0 VertSyncPulseField0  VertBackPorchField0 VertSyncPulseOffsetField0  
	VertActiveLinesField1 VertFrontPorchField1 VertSyncPulseField1  VertBackPorchField1 VertSyncPulseOffsetField1  
	*/
	
	{ /* 800 x 600                   */
	"800x600 ", 800, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	40,  128,  88, 
	600,  1,  4,  23,  0, 
	600,  1,  4,  23,  0, 
	},
	
	{ /* 1024 x 768                   */
	"1024x768 ", 1024, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_eNEGATIVE, 
	24,  136,  160, 
	768,  3,  6,  29,  0,
	768,  3,  6,  29,  0,
	},
	
	{ /* 1280 x 768                   */
	"1280x768 ", 1280, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_ePOSITIVE, 
	64,  128,  192, 
	768,  3,  7,  20,  0, 
	768,  3,  7,  20,  0, 
	},

	{ /* 1280 x 1024                   */
	"1280x1024", 1280, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	48,  112, 248, 
	1024,  1,  3,  38,  0, 
	1024,  1,  3,  38,	0
	},
		

	{ /* Place Holder for 1280x720 50Hz        */
	"1280x720*", 0, BHDM_POLARITY_ePOSITIVE, BHDM_POLARITY_ePOSITIVE, 
	0,  0,  0, 
	0,  0,  0,  0,  0, 
	0,  0,  0,  0,  0,
	},
	
	{ /* 1280 x 720                   */
	"1280x720 ", 1280, BHDM_POLARITY_eNEGATIVE, BHDM_POLARITY_ePOSITIVE, 
	64,  128,  192, 
	720,  3,  7,  20,  0, 
	720,  3,  7,  20,  0, 
	}
} ; /* BHDM_VideoFmtParams[] */


static BERR_Code BHDM_P_ConfigurePixelRepeater(
	BHDM_Handle hHDMI, BFMT_VideoFmt eVideoFmt, BAVC_HDMI_PixelRepetition ePixelRepetition) ;

static BERR_Code BHDM_P_ConfigureInputVideoFmt(
	BHDM_Handle hHDMI, BFMT_VideoFmt eVideoFmt, BAVC_HDMI_PixelRepetition ePixelRepetition) ;
	
static void BHDM_P_HandleMaiFormatUpdate_isr(BHDM_Handle hHDMI) ;	


#if BHDM_CONFIG_BTMR_SUPPORT
static BERR_Code BHDM_P_CreateTimer(BHDM_Handle hHDM, BTMR_TimerHandle * timerHandle, uint8_t timerId) ;

static BERR_Code BHDM_P_DestroyTimer(BHDM_Handle hHDM, BTMR_TimerHandle timerHandle, uint8_t timerId) ;

static void BHDM_P_TimerExpiration_isr (BHDM_Handle hHDM, int parm2) ;
#endif

	

#if BHDM_CONFIG_DVO_SUPPORT
BERR_Code BHDM_P_EnableDvoDisplay(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings) ;
#endif


#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
static void BHDM_P_GetReceiverSense_isr(BHDM_Handle hHDMI, bool *RxSense) ;
#endif

/******************************************************************************
Summary:
Interrupt Callback Table to describe interrupt Names, ISRs, and Masks 
*******************************************************************************/
typedef struct BHDM_P_InterruptCbTable
{
	BINT_Id       	  IntrId;
	BINT_CallbackFunc pfCallback;
	int               iParam2; 
	bool              enable ; /* debug purposes */
} BHDM_P_InterruptCbTable ;



#define BHDM_INT_CB_DEF(intr,  id, enable) \
	{intr, BHDM_P_HandleInterrupt_isr, id, enable},


static const BHDM_P_InterruptCbTable BHDM_Interrupts[] =
{
#if BHDM_CONFIG_DUAL_HPD_SUPPORT
	BHDM_INT_CB_DEF(BCHP_INT_ID_HP_REMOVED_INTR,
		BHDM_INTR_eHOTPLUG_REMOVED, true)
	BHDM_INT_CB_DEF(BCHP_INT_ID_HP_CONNECTED_INTR,
		BHDM_INTR_eHOTPLUG_CONNECTED, true)
#else
	BHDM_INT_CB_DEF(BCHP_INT_ID_HOTPLUG_INTR,
		BHDM_INTR_eHOTPLUG, true)
#endif

	BHDM_INT_CB_DEF(BCHP_INT_ID_DRIFT_FIFO_FULL_MINUS_INTR,
		BHDM_INTR_eDF_FULL_MINUS, false)
	BHDM_INT_CB_DEF(BCHP_INT_ID_DRIFT_FIFO_ALMOST_FULL_INTR, 
		BHDM_INTR_eDF_ALMOST_FULL, false)
	BHDM_INT_CB_DEF(BCHP_INT_ID_DRIFT_FIFO_EMPTY_MINUS_INTR,
		BHDM_INTR_eDF_EMPTY_MINUS, false)
	BHDM_INT_CB_DEF(BCHP_INT_ID_DRIFT_FIFO_ALMOST_EMPTY_INTR, 
		BHDM_INTR_eDF_ALMOST_EMPTY, false)
	
	BHDM_INT_CB_DEF(BCHP_INT_ID_ILLEGAL_WRITE_TO_ACTIVE_RAM_PACKET_INTR,
		BHDM_INTR_ePKT_WRITE_ERR, false)

	BHDM_INT_CB_DEF(BCHP_INT_ID_HDCP_REPEATER_ERR_INTR,
		BHDM_INTR_eHDCP_REPEATER_ERR, HDMI_PORT)
	BHDM_INT_CB_DEF(BCHP_INT_ID_HDCP_V_MISMATCH_INTR,
		BHDM_INTR_eHDCP_V_MISMATCH, HDMI_PORT)
	BHDM_INT_CB_DEF(BCHP_INT_ID_HDCP_V_MATCH_INTR,
		BHDM_INTR_eHDCP_V_MATCH, HDMI_PORT)
	BHDM_INT_CB_DEF(BCHP_INT_ID_HDCP_RI_INTR,
		BHDM_INTR_eHDCP_RI, HDMI_PORT)
	BHDM_INT_CB_DEF(BCHP_INT_ID_HDCP_AN_READY_INTR,
		BHDM_INTR_eHDCP_AN, HDMI_PORT)
	
	BHDM_INT_CB_DEF(BCHP_INT_ID_PACKET_FIFO_OVERFLOW_INTR,
		BHDM_INTR_ePKT_OVERFLOW, false)
	BHDM_INT_CB_DEF(BCHP_INT_ID_HDCP_PJ_INTR,
		BHDM_INTR_eHDCP_PJ, false)
	BHDM_INT_CB_DEF(BCHP_INT_ID_MAI_FORMAT_UPDATE_INTR,
		BHDM_INTR_eMAI_FORMAT_UPDATE, true)

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	BHDM_INT_CB_DEF(BCHP_INT_ID_PJ_MISMATCH_INTR,
		BHDM_INTR_eHDCP_PJ_MISMATCH, HDMI_PORT)
	BHDM_INT_CB_DEF(BCHP_INT_ID_RI_A_MISMATCH_INTR,
		BHDM_INTR_eHDCP_RI_A_MISMATCH, HDMI_PORT)
	BHDM_INT_CB_DEF(BCHP_INT_ID_RI_B_MISMATCH_INTR,
		BHDM_INTR_eHDCP_RI_B_MISMATCH, HDMI_PORT)
#endif

	
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
#if  !BHDM_CONFIG_RSEN_POLLING
	BHDM_INT_CB_DEF(BCHP_INT_ID_RSEN_UPDATE_INTR, 
		BHDM_INTR_eRSEN, true)
#endif		
#endif
} ;


/*******************************************************************************
BERR_Code BHDM_GetDefaultSettings
Summary: Get the default settings for the HDMI device.
*******************************************************************************/
BERR_Code BHDM_GetDefaultSettings(
   BHDM_Settings *pDefault  /* [in] pointer to memory to hold default settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	BDBG_ENTER(BHDM_GetDefaultSettings) ;

	BDBG_ASSERT(pDefault) ;
	BKNI_Memset(pDefault, 0, sizeof(BHDM_Settings));


	/* Set default settings */

	pDefault->eInputVideoFmt = BFMT_VideoFmt_e1080i;
	pDefault->eTimebase = BAVC_Timebase_e0;
	pDefault->eOutputPort = BHDM_OutputPort_eHDMI;
	pDefault->eOutputFormat = BHDM_OutputFormat_eDVIMode;
	pDefault->eColorimetry = BAVC_MatrixCoefficients_eHdmi_RGB;
	pDefault->eAspectRatio = BFMT_AspectRatio_e4_3;
	pDefault->ePixelRepetition = BAVC_HDMI_PixelRepetition_eNone;

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	pDefault->stColorDepth.eBitsPerPixel = BAVC_HDMI_BitsPerPixel_e24bit;
#endif


	/**** AVI Info Frame Structure ****/
	{
		pDefault->stAviInfoFrame.bOverrideDefaults = false;

		/* RGB or YCbCr (Y1Y0) generated from BHDM_Settings eColorimetry 
		   always RGB444 or YCbCr444 */

		pDefault->stAviInfoFrame.eActiveInfo = BAVC_HDMI_AviInfoFrame_ActiveInfo_eValid; /* A0 */
		pDefault->stAviInfoFrame.eBarInfo = BAVC_HDMI_AviInfoFrame_BarInfo_eInvalid ; /* B1B0 */
		pDefault->stAviInfoFrame.eScanInfo = BAVC_HDMI_AviInfoFrame_ScanInfo_eOverScanned;	/* S1S0 */

		/* Colorimetry (C1C0) generated from BHDM_Settings eColorimetry
		(BAVC_MatrixCoefficients) requires conversion to HDMI AVI values */
				   
		/* Picture AR (M1M0) generated from BHDM_Settings eAspectRatio 
		(BFMT_AspectRatio) which requires conversion to HDMI AVI values 
		unless overriden */
		pDefault->stAviInfoFrame.ePictureAspectRatio = 
				BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e4_3; 
		pDefault->stAviInfoFrame.eActiveFormatAspectRatio = 
				BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatio_ePicture; /* R3R0 */
		pDefault->stAviInfoFrame.eScaling = 
				BAVC_HDMI_AviInfoFrame_Scaling_eNoScaling, /* SC1SC0 */
				
		/* Video ID Code (VICn) generated from BHDM_Settings eInputVideoFmt */
		pDefault->stAviInfoFrame.VideoIdCode = BFMT_VideoFmt_eNTSC,
				
		/* Pixel Repeat (PRn) generated from BHDM_Settings eInputVideoFmt 
		   (BFMT_VideoFmt) */
		
#if BAVC_HDMI_1_3_SUPPORT
		pDefault->stAviInfoFrame.eITContent = 
				BAVC_HDMI_AviInfoFrame_ITContent_eNoData;
		pDefault->stAviInfoFrame.eExtendedColorimetry = 
				BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvReserved;
		pDefault->stAviInfoFrame.eRGBQuantizationRange = 
				BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eDefault;
#endif	

		/* bar info */
		pDefault->stAviInfoFrame.TopBarEndLineNumber = 0;
		pDefault->stAviInfoFrame.BottomBarStartLineNumber = 0;
		pDefault->stAviInfoFrame.LeftBarEndPixelNumber = 0;
		pDefault->stAviInfoFrame.RightBarEndPixelNumber = 0;
	}


	/**** AUDIO Info Frame Structure *****/
	{
		pDefault->stAudioInfoFrame.ChannelCount = 
			BAVC_HDMI_AudioInfoFrame_ChannelCount_e2Channels ;
		
		pDefault->stAudioInfoFrame.CodingType = 
			BAVC_HDMI_AudioInfoFrame_CodingType_eReferToStream ;
		
		pDefault->stAudioInfoFrame.SampleSize = 
			BAVC_HDMI_AudioInfoFrame_SampleSize_eReferToStreamHeader ;
		
		/* Sample Freq always Refer To Stream Header for PCM and compressed */
		pDefault->stAudioInfoFrame.SampleFrequency = 
			BAVC_HDMI_AudioInfoFrame_SampleFrequency_eReferToStreamHeader ;
		
		pDefault->stAudioInfoFrame.SpeakerAllocation = BHDM_ChannelAllocation_eStereo;
		pDefault->stAudioInfoFrame.DownMixInhibit = 0;
		pDefault->stAudioInfoFrame.LevelShift = 0;
	}
		
	pDefault->eAudioSamplingRate = BAVC_AudioSamplingRate_e48k;
	pDefault->eAudioFormat = BAVC_AudioFormat_ePCM;
	pDefault->eAudioBits = BAVC_AudioBits_e16;
	
	pDefault->eSpdSourceDevice = BHDM_SPD_SourceDevice_eDigitalSTB;
	BKNI_Memcpy(pDefault->SpdVendorName, "Broadcom", 
		BAVC_HDMI_SPD_IF_VENDOR_LEN);
	BKNI_Memcpy(pDefault->SpdDescription, "Reference Board", 
		BAVC_HDMI_SPD_IF_DESC_LEN);
		
	/**** Vendor Specific Info Frame ****/
	{
		pDefault->stVendorSpecificInfoFrame.uIEEE_RegId[0] = 0x03;
		pDefault->stVendorSpecificInfoFrame.uIEEE_RegId[1] = 0x0C;
		pDefault->stVendorSpecificInfoFrame.uIEEE_RegId[2] = 0x0;
		pDefault->stVendorSpecificInfoFrame.eHdmiVideoFormat = 
				BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone;
		pDefault->stVendorSpecificInfoFrame.eHdmiVic = 0;
		pDefault->stVendorSpecificInfoFrame.e3DStructure = 0;
		pDefault->stVendorSpecificInfoFrame.e3DExtData = 
				BAVC_HDMI_VSInfoFrame_3DExtData_eNone;
	}

	pDefault->CalculateCts = false;
	pDefault->uiDriverAmpDefault = BHDM_CONFIG_DRIVER_AMP_DEFAULT;
	pDefault->AltDvoPath = false;
	pDefault->BypassEDIDChecking = false; /* bypass EDID checking; DEBUG ONLY!*/ 
	pDefault->bFifoMasterMode = false;	/* use slave mode */
	pDefault->bForceEnableDisplay = false;
	pDefault->bEnableAutoRiPjChecking = false;
	

	BDBG_LEAVE(BHDM_GetDefaultSettings) ;
	return rc ;
}


void BHDM_P_EnableInterrupts(BHDM_Handle hHDMI) 
{
	BERR_Code rc ;
	uint8_t i ;

	/* enable specified interrupt callbacks */
	for( i = 0; i < MAKE_INTR_ENUM(LAST) ; i++ )
	{
		/* now enable it; if specified for startup */	
		if (!BHDM_Interrupts[i].enable)
			continue ;

		/* enable Receiver Sense Interrupts when output is first enabled */
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT && !BHDM_CONFIG_RSEN_POLLING
		if (i == MAKE_INTR_ENUM(RSEN)) 
			continue ;		
#endif

		rc = BINT_EnableCallback( hHDMI->hCallback[i]) ;
		BERR_TRACE(rc) ;
	}
}


void BHDM_P_DisableInterrupts(BHDM_Handle hHDMI) 
{
	BERR_Code rc ;
	uint8_t i ;

	/* enable specified interrupt callbacks */
	for( i = 0; i < MAKE_INTR_ENUM(LAST) ; i++ )
	{
		/* now enable it; if specified for startup */	
		if (!BHDM_Interrupts[i].enable)
			continue ;

		/* enable Receiver Sense Interrupts when output is first enabled */
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT && !BHDM_CONFIG_RSEN_POLLING
		if (i == MAKE_INTR_ENUM(RSEN)) 
			continue ;		
#endif

		rc = BINT_DisableCallback( hHDMI->hCallback[i]) ;
		BERR_TRACE(rc) ;
	}
}




/******************************************************************************
BERR_Code BHDM_Open
Summary: Open/Initialize the HDMI device 
*******************************************************************************/
BERR_Code BHDM_Open(
   BHDM_Handle *phHDMI,       /* [out] HDMI handle */
   BCHP_Handle hChip,         /* [in] Chip handle */
   BREG_Handle hRegister,     /* [in] Register Interface to HDMI Tx Core */
   BINT_Handle hInterrupt,    /* [in] Interrupt handle */
   BREG_I2C_Handle hI2cRegHandle,      /* [in] I2C Interface to HDMI Rx */
   const BHDM_Settings  *pSettings /* [in] default HDMI settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	BHDM_Handle hHDMI = NULL ;
	uint32_t Register ;
	uint8_t i ;

	BDBG_ENTER(BHDM_Open) ;

	/* verify parameters */
	BDBG_ASSERT(hChip) ;
	BDBG_ASSERT(hRegister) ;
	BDBG_ASSERT(hInterrupt) ;

	if (!pSettings)
	{
		BDBG_ERR(("BHDM_Settings required for BHDM_Open")) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}


	if((pSettings->eOutputPort == BHDM_OutputPort_eHDMI) && (HDMI_PORT)) {	
		BDBG_ASSERT(hI2cRegHandle) ;
	}
 
	/* output port/compilation consistency check */

	/* if ASSERT here BHDM_Settings may not be properly set */
		
	BDBG_ASSERT(
		((pSettings->eOutputPort == BHDM_OutputPort_eHDMI) && (HDMI_PORT))
	||   ((pSettings->eOutputPort == BHDM_OutputPort_eDVO) && (DVO_PORT))) ;
	
	
	/* create the HDMI Handle */
	hHDMI = (BHDM_Handle) BKNI_Malloc(sizeof(BHDM_P_Handle)) ;
	if (!hHDMI)
	{
		BDBG_ERR(("Unable to allocate memory for HDMI Handle")) ;
		rc = BERR_OUT_OF_SYSTEM_MEMORY ;
		goto done ;
	}
	
	/* zero out all memory associated with the HDMI Device Handle before using */
	BKNI_Memset(hHDMI, 0, sizeof(BHDM_P_Handle)) ;

	/* assign the handles passed in as parameters */
	hHDMI->hChip      = hChip ;     
	hHDMI->hRegister  = hRegister ; 
	hHDMI->hInterrupt = hInterrupt ;
	hHDMI->hI2cRegHandle       = hI2cRegHandle ;

	/* save the settings and set the output port-- once set it never changes */
	/* coverity[dead_error_condition] */
	if (!pSettings)
	{
		/* copy the selected default settings for this HDMI handle */
		BHDM_GetDefaultSettings(&hHDMI->DeviceSettings);
		hHDMI->eOutputPort = BHDM_OutputPort_eHDMI ;
	}
	else
	{
		hHDMI->DeviceSettings = *pSettings ;
		hHDMI->eOutputPort = pSettings->eOutputPort ;
	}
	
	/* set the default pixel clock rate to 27MHz to match 480i default */
	hHDMI->eInputPixelClock = BHDM_PixelClock_e27 ;

	
	/* set default output format to DVI - dynamic based on attached monitor */
	hHDMI->DeviceSettings.eOutputFormat = BHDM_OutputFormat_eDVIMode ;

	/* Reset the HDMI core.. make sure other cores are not held in reset */
	BHDM_P_ResetHdmiCore (hHDMI);

#if BHDM_CONFIG_BTMR_SUPPORT

	BDBG_ASSERT(pSettings->hTMR) ;
	hHDMI->hTMR = pSettings->hTMR ;

	/* create a hot plug countdown timer */
	BHDM_P_CreateTimer(hHDMI, 
		&hHDMI->TimerHotPlug, BHDM_P_TIMER_eHotPlug) ;

	/* create a hot plug countdown timer */
	BHDM_P_CreateTimer(hHDMI, 
		&hHDMI->TimerFormatChange, BHDM_P_TIMER_eFormatDetection) ;

#endif

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CLK
	/* Power up everything. This has to happen after the soft reset because the soft reset touches 
	the same registers that BCHP_PWR touches. The soft reset does not require HDMI clocks to be on */
	BCHP_PWR_AcquireResource(hChip, BCHP_PWR_RESOURCE_HDMI_TX_CLK);
#endif

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
	BCHP_PWR_AcquireResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif

	/* all and any register R/W must happen after the AcquireResource */
#if BHDM_CONFIG_40NM_SUPPORT || BHDM_CONFIG_28NM_SUPPORT
	Register = BREG_Read32( hHDMI->hRegister, BCHP_DVP_HT_CORE_REV);
#else
	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_CORE_REV );
#endif 

	/* display version information */
	BDBG_MSG(("*****************************************")) ;
	BDBG_MSG(("HDMI Core Version 0x%08X", Register)) ;
	BDBG_MSG(("$brcm_Workfile: bhdm.c $")) ;
	BDBG_MSG(("$brcm_Revision: Hydra_Software_Devel/355 $")) ;
	BDBG_MSG(("$brcm_Date: 10/24/12 12:38p $")) ;
	BDBG_MSG(("*****************************************")) ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	/* Ensure HDMI is not in control of BSCC (I2C) block. This is to prevent I2C got locked 
		by HDMI core in the case ctrl-c was use to terminate the software AND 
		HW Ri/Pj checking was enabled */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_1, CHECK_MODE, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1, Register) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_1, CHECK_MODE));
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1, Register) ;
#endif

	/* Power on PHY */
	BHDM_P_PowerOnPhy (hHDMI) ;

	/* check if something is connected and stored */
	/* interrupts are not enabled yet, so no need for critical section here */
	BHDM_P_RxDeviceAttached_isr(hHDMI, &hHDMI->RxDeviceAttached) ;

	
	/* Reset the EDID for reading */	
	hHDMI->edidStatus = BHDM_EDID_STATE_eInitialize;  /* Set EDID Initialization flag */

	/* enable HDCP Pj Checking by default */
	hHDMI->HdcpOptions.PjChecking = true ;
	
	/* Create Events for use with Interrupts */
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventHDCP))) ;
	
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventHotPlug))) ;
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventFIFO))) ;
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventRAM))) ;
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventHDCPRiValue))) ;
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventHDCPPjValue))) ;
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventHDCPRepeater))) ;
	BHDM_CHECK_RC(rc, BKNI_CreateEvent(&(hHDMI->BHDM_EventRxSense))) ;


	/* register/enable interrupt callbacks */
	for( i = 0; i < MAKE_INTR_ENUM(LAST) ; i++ )
	{
		/* 
		** DEBUG
		** Create ALL interrupt callbacks
		** enable debug callbacks as needed;
		*/

		BHDM_CHECK_RC( rc, BINT_CreateCallback(
			&(hHDMI->hCallback[i]), hHDMI->hInterrupt, 
			BHDM_Interrupts[i].IntrId,
			BHDM_P_HandleInterrupt_isr, (void *) hHDMI, i ));
			
		/* clear interrupt callback */
		BHDM_CHECK_RC(rc, BINT_ClearCallback( hHDMI->hCallback[i])) ;
	}
	BHDM_P_EnableInterrupts(hHDMI) ;

	/* enable the output port with the correct output format DVO/HDMI */		
#if DVO_PORT
#if BHDM_CONFIG_DVO_SUPPORT
	if (hHDMI->eOutputPort != BHDM_OutputPort_eHDMI) /* HDMI Port is default setting */
	{
		BHDM_CHECK_RC( rc, BHDM_DVO_P_EnableDvoPort(hHDMI, pSettings->eOutputFormat)) ;
		hHDMI->DeviceSettings.BypassEDIDChecking = true ;
	}
	
#else
#error "Unknown/Unsupported chip for DVO port"
#endif	
#endif


#if BHDM_CONFIG_HDMI_1_3_SUPPORT && BHDM_CONFIG_65NM_SUPPORT
	/* set the pre-emphasis mode to DeepColorMode. This settings only makes a difference when
		running at pixel clock higher than 148.5Mhz and makes no harm for other clock rate */
	BHDM_P_SetPreEmphasisMode(hHDMI, BHDM_PreEmphasis_eDeepColorMode, 0);
#endif


#if BHDM_CONFIG_PLL_KICKSTART_WORKAROUND
	hHDMI->uiPllKickStartCount = 0 ;
#endif

	/* keep created pointer */
	*phHDMI = hHDMI ;

	/* Update audio channel map based on MAI_FORMAT */
	BKNI_EnterCriticalSection();
	BHDM_P_HandleMaiFormatUpdate_isr(hHDMI);
	BKNI_LeaveCriticalSection();

done:
	if (rc != BERR_SUCCESS) 
	{
		if (hHDMI != NULL)	
		{
			BKNI_Free(hHDMI);
		}
		*phHDMI=NULL;

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CLK
		/* on failure, power everything down */
		BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_HDMI_TX_CLK);
#endif

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		/* on failure, power everything down */
		BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif
	}

	BDBG_LEAVE(BHDM_Open) ;
	return rc ;
} /* end BHDM_Open */



#if BHDM_CONFIG_DVO_SUPPORT
/******************************************************************************
BERR_Code BHDM_P_EnableDvoDisplay
Summary: Display output to the DVO output port 
*******************************************************************************/
BERR_Code BHDM_P_EnableDvoDisplay(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings) 
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_P_EnableDvoDisplay) ;

#if BHDM_CONFIG_DEBUG_FIFO
	/* always disable FIFO callbacks when changing the Rate Manager */	
	BHDM_CHECK_RC(rc, BHDM_P_EnableFIFOInterrupts(hHDMI, false)) ;
#endif	

	BHDM_P_PowerOnPhy(hHDMI);
	
	/****  15 Enable TMDS Buffers */
	BHDM_EnableTmdsOutput(hHDMI, true) ;
	
	/*RB*  07 Configure HDMI to the Input Display Format - Scheduler */
	BHDM_CHECK_RC(rc, BHDM_P_ConfigureInputVideoFmt(hHDMI, NewHdmiSettings->eInputVideoFmt,
						NewHdmiSettings->ePixelRepetition)) ;

	/* save the new HDMI Settings we used to enable the HDMI device for this HDMI handle */
	hHDMI->DeviceSettings = *NewHdmiSettings ;

#if BHDM_CONFIG_DEBUG_FIFO
	/* dump FIFO pointers for debugging */		
	BHDM_P_CaptureFIFOData(hHDMI) ;
	
	/* re-enable FIFO Interrupts */	
	BHDM_CHECK_RC(rc, BHDM_P_EnableFIFOInterrupts(hHDMI, true)) ;
#endif

done:
	BDBG_LEAVE(BHDM_P_EnableDvoDisplay) ;

	return rc ;
}
#endif


bool BHDM_P_HdmiSettingsChange(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings)
{
	if ((hHDMI->DeviceSettings.eInputVideoFmt == NewHdmiSettings->eInputVideoFmt)
	&& (hHDMI->DeviceSettings.eTimebase == NewHdmiSettings->eTimebase)
	&& (hHDMI->DeviceSettings.eOutputPort == NewHdmiSettings->eOutputPort)
	&& (hHDMI->DeviceSettings.eOutputFormat == NewHdmiSettings->eOutputFormat)
	&& (hHDMI->DeviceSettings.eColorimetry == NewHdmiSettings->eColorimetry)
	&& (hHDMI->DeviceSettings.eAspectRatio == NewHdmiSettings->eAspectRatio)
	&& (hHDMI->DeviceSettings.ePixelRepetition == NewHdmiSettings->ePixelRepetition)

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	&& (hHDMI->DeviceSettings.stColorDepth.eBitsPerPixel == NewHdmiSettings->stColorDepth.eBitsPerPixel)
#endif


	/**** AVI Info Frame Structure ****/	
	&& (hHDMI->DeviceSettings.stAviInfoFrame.ePixelEncoding == NewHdmiSettings->stAviInfoFrame.ePixelEncoding)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.bOverrideDefaults == NewHdmiSettings->stAviInfoFrame.bOverrideDefaults)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eActiveInfo == NewHdmiSettings->stAviInfoFrame.eActiveInfo)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eBarInfo == NewHdmiSettings->stAviInfoFrame.eBarInfo)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eScanInfo == NewHdmiSettings->stAviInfoFrame.eScanInfo)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eColorimetry == NewHdmiSettings->stAviInfoFrame.eColorimetry)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eActiveInfo == NewHdmiSettings->stAviInfoFrame.eActiveInfo)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.ePictureAspectRatio == NewHdmiSettings->stAviInfoFrame.ePictureAspectRatio)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eActiveFormatAspectRatio == NewHdmiSettings->stAviInfoFrame.eActiveFormatAspectRatio)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eScaling == NewHdmiSettings->stAviInfoFrame.eScaling)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.VideoIdCode == NewHdmiSettings->stAviInfoFrame.VideoIdCode)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eActiveInfo == NewHdmiSettings->stAviInfoFrame.eActiveInfo)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.PixelRepeat == NewHdmiSettings->stAviInfoFrame.PixelRepeat)	
#if BAVC_HDMI_1_3_SUPPORT
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eITContent == NewHdmiSettings->stAviInfoFrame.eITContent)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eExtendedColorimetry == NewHdmiSettings->stAviInfoFrame.eExtendedColorimetry)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eRGBQuantizationRange == NewHdmiSettings->stAviInfoFrame.eRGBQuantizationRange)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eYccQuantizationRange == NewHdmiSettings->stAviInfoFrame.eYccQuantizationRange)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.eContentType == NewHdmiSettings->stAviInfoFrame.eContentType)
#endif
	&& (hHDMI->DeviceSettings.stAviInfoFrame.TopBarEndLineNumber== NewHdmiSettings->stAviInfoFrame.TopBarEndLineNumber)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.BottomBarStartLineNumber == NewHdmiSettings->stAviInfoFrame.BottomBarStartLineNumber)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.LeftBarEndPixelNumber == NewHdmiSettings->stAviInfoFrame.LeftBarEndPixelNumber)
	&& (hHDMI->DeviceSettings.stAviInfoFrame.RightBarEndPixelNumber == NewHdmiSettings->stAviInfoFrame.RightBarEndPixelNumber)			


	/**** Audio Info Frame Structure ****/	
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.bOverrideDefaults == NewHdmiSettings->stAudioInfoFrame.bOverrideDefaults)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.ChannelCount == NewHdmiSettings->stAudioInfoFrame.ChannelCount)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.CodingType == NewHdmiSettings->stAudioInfoFrame.CodingType)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.SampleSize == NewHdmiSettings->stAudioInfoFrame.SampleSize)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.SampleFrequency == NewHdmiSettings->stAudioInfoFrame.SampleFrequency)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.SpeakerAllocation == NewHdmiSettings->stAudioInfoFrame.SpeakerAllocation)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.DownMixInhibit == NewHdmiSettings->stAudioInfoFrame.DownMixInhibit)
	&& (hHDMI->DeviceSettings.stAudioInfoFrame.LevelShift == NewHdmiSettings->stAudioInfoFrame.LevelShift)

	
	&& (hHDMI->DeviceSettings.eAudioSamplingRate == NewHdmiSettings->eAudioSamplingRate)
	&& (hHDMI->DeviceSettings.eAudioFormat == NewHdmiSettings->eAudioFormat)
	&& (hHDMI->DeviceSettings.eAudioBits == NewHdmiSettings->eAudioBits)

	&& (hHDMI->DeviceSettings.eAudioFormat == NewHdmiSettings->eAudioFormat)

	
	/**** Source Product Description ****/	
	&& (hHDMI->DeviceSettings.eSpdSourceDevice == NewHdmiSettings->eSpdSourceDevice)
	&& (BKNI_Memcmp(hHDMI->DeviceSettings.SpdVendorName, 
	                      NewHdmiSettings->SpdVendorName, BAVC_HDMI_SPD_IF_VENDOR_LEN) == 0)
	&& (BKNI_Memcmp(hHDMI->DeviceSettings.SpdDescription, 
	                      NewHdmiSettings->SpdDescription, BAVC_HDMI_SPD_IF_DESC_LEN) == 0)
	

	/**** Vendor Specific Info Frame Structure ****/	
	&& (BKNI_Memcmp(hHDMI->DeviceSettings.stVendorSpecificInfoFrame.uIEEE_RegId,
	                NewHdmiSettings->stVendorSpecificInfoFrame.uIEEE_RegId, 
	                BAVC_HDMI_IEEE_REGID_LEN) == 0)
	
	&& (hHDMI->DeviceSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat == 
	          NewHdmiSettings->stVendorSpecificInfoFrame.eHdmiVideoFormat)
	

	&& (hHDMI->DeviceSettings.stVendorSpecificInfoFrame.eHdmiVic == 
	          NewHdmiSettings->stVendorSpecificInfoFrame.eHdmiVic)
	/* or */
	&& (hHDMI->DeviceSettings.stVendorSpecificInfoFrame.e3DStructure == 
	          NewHdmiSettings->stVendorSpecificInfoFrame.e3DStructure)
	          
	&& (hHDMI->DeviceSettings.stVendorSpecificInfoFrame.e3DExtData == 
	          NewHdmiSettings->stVendorSpecificInfoFrame.e3DExtData)
	          


	&& (hHDMI->DeviceSettings.CalculateCts == NewHdmiSettings->CalculateCts)
	&& (hHDMI->DeviceSettings.uiDriverAmpDefault == NewHdmiSettings->uiDriverAmpDefault)
	&& (hHDMI->DeviceSettings.AltDvoPath == NewHdmiSettings->AltDvoPath)
	
	&& (hHDMI->DeviceSettings.BypassEDIDChecking == NewHdmiSettings->BypassEDIDChecking)
	&& (hHDMI->DeviceSettings.bFifoMasterMode == NewHdmiSettings->bFifoMasterMode)
	&& (hHDMI->DeviceSettings.bForceEnableDisplay == NewHdmiSettings->bForceEnableDisplay)
	&& (hHDMI->DeviceSettings.bEnableAutoRiPjChecking == NewHdmiSettings->bEnableAutoRiPjChecking))
		return false ;
	else
		return true ;
}


/******************************************************************************
BERR_Code BHDM_EnableTmdsOutput
Summary: Enable (Display) TMDS Output
*******************************************************************************/
BERR_Code BHDM_EnableTmdsOutput(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
   bool bEnableTmdsOutput	/* [in] boolean to enable/disable */
)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BHDM_EnableTmdsOutput) ;
	BDBG_ASSERT( hHDMI ) ;

	BKNI_EnterCriticalSection() ;
		BHDM_P_EnableTmdsOutput_isr(hHDMI, bEnableTmdsOutput) ;
	BKNI_LeaveCriticalSection() ;

	BDBG_LEAVE(BHDM_EnableTmdsOutput) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_EnableDisplay
Summary: Display output to the HDMI device 
*******************************************************************************/
BERR_Code BHDM_EnableDisplay(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings) 
{
	BERR_Code rc = BERR_SUCCESS ;
	uint32_t Register ;
	uint8_t DeviceAttached ;
	uint8_t timeoutFrames ;
	uint8_t HdmiModeSet ;
	uint8_t bHPInterrupt = false;
	bool HdmiVideoFormatChange = false ;

	uint8_t FrameDelay ;
	const BFMT_VideoInfo *pVideoInfo ;

	BDBG_ENTER(BHDM_EnableDisplay) ;

#if BHDM_CONFIG_DVO_SUPPORT
	BSTD_UNUSED(Register) ;
	BSTD_UNUSED(DeviceAttached) ;
	BSTD_UNUSED(timeoutFrames) ;
	BSTD_UNUSED(HdmiModeSet) ;
	BSTD_UNUSED(bHPInterrupt);
		
	rc = BHDM_P_EnableDvoDisplay(hHDMI, NewHdmiSettings) ;
	
	goto done ;
#else

	/* check if a Receiver is Attached */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &DeviceAttached)) ;

	if (!DeviceAttached)
	{
		BDBG_WRN(("No DVI/HDMI Receiver is Attached")) ;
		rc = BHDM_NO_RX_DEVICE ;
		goto done;
	}

	if (!NewHdmiSettings->bForceEnableDisplay 
	&&  !BHDM_P_HdmiSettingsChange(hHDMI, NewHdmiSettings)
#if defined(HUMAX_PLATFORM_BASE)
	/* to fix mismatch between Nexus and Magnum */
	&& hHDMI->tmdsEnabled
#endif
	)
	{
		BDBG_MSG(("No change in HDMI settings...")) ;
		rc = BERR_SUCCESS;
		goto done;
	}	

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	/* Additional PHY settings */
	BHDM_CHECK_RC(rc, BHDM_P_ConfigurePhy(hHDMI, NewHdmiSettings));

	/* ensure GCP is disabled in DVI mode */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_GCP_CONFIG); 
	Register &= ~BCHP_MASK(HDMI_GCP_CONFIG, GCP_ENABLE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_GCP_CONFIG, Register); 
#endif

	/****  15 Enable TMDS Buffers */
 	BHDM_EnableTmdsOutput(hHDMI, true) ;

	/* indicate HDMI Output (Clock + Data) are ENABLED */
	hHDMI->OutputEnabled = true ;

	/* Initialize HDMI core */
	/****  01 Determine Supported Output Mode (DVI, HDMI) from Rx */

	/****  03 HDMI_TODO Configure the Pre-Emphasis */

	/****  05 Check for Lock & Nominal Difference */

#if BHDM_CONFIG_DEBUG_FIFO
	/* always disable FIFO callbacks when changing the Rate Manager */	
	BHDM_P_EnableFIFOInterrupts(hHDMI, false) ;
#endif	

	
	/*RB*  07 Configure HDMI to the Input Display Format - Scheduler */
	BHDM_CHECK_RC(rc, BHDM_P_ConfigureInputVideoFmt(hHDMI, NewHdmiSettings->eInputVideoFmt,
						NewHdmiSettings->ePixelRepetition)) ;

	
	/*RB*  08 Configure Pixel Repeater for rate < 25MPixels */
	if  (NewHdmiSettings->eOutputFormat == BHDM_OutputFormat_eHDMIMode)
		BHDM_CHECK_RC(rc, BHDM_P_ConfigurePixelRepeater(hHDMI, NewHdmiSettings->eInputVideoFmt,
						NewHdmiSettings->ePixelRepetition)) ;

	/*RK*  09 Configure the Encoders */

	/* output port already selected during at BHDM_Open */

	/*CP*  09.5 Configure the BCH Encoder    */  
	Register = 
	    /* clear DISABLE_PARITY bit  - reset value */
		  BCHP_FIELD_DATA(HDMI_BCH_CONFIGURATION, DISABLE_PARITY, 0)
	    /* set   FEEDBACK_MASK = 8'h83   - reset value */    
		| BCHP_FIELD_DATA(HDMI_BCH_CONFIGURATION, FEEDBACK_MASK,  0x83) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_BCH_CONFIGURATION, Register) ; 

	/*RK*  14 Configure PERT Off */
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_PERT_CONFIG, (uint32_t) 0) ;

	if (NewHdmiSettings->eOutputFormat == BHDM_OutputFormat_eHDMIMode)
	{
		/*CP*  13 Enable/Configure RAM Packets */
		BHDM_CHECK_RC(rc, BHDM_InitializePacketRAM(hHDMI)) ;

		/* configure audio input to HDMI */
		BHDM_P_ConfigureInputAudioFmt(hHDMI, &NewHdmiSettings->stAudioInfoFrame) ;
		
		/* 
		*CP*  12 Configure Audio Clock Regeneration Packet 
		** Audio Clock Regeneration Packet now re-configured
		** upon Video Rate Change Callback 
		*/

		/* all configuration complete; request and switch to HDMI mode	*/
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL) ;
		Register &= ~(BCHP_MASK(HDMI_SCHEDULER_CONTROL, HDMI_MODE_REQUEST)
				| BCHP_MASK(HDMI_SCHEDULER_CONTROL, USE_POST_LINE_KEEP_OUT)
				| BCHP_MASK(HDMI_SCHEDULER_CONTROL, POST_LINE_KEEP_OUT));
		Register |= 
			( BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, HDMI_MODE_REQUEST, 1)
			| BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, USE_POST_LINE_KEEP_OUT, 0)
			| BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, POST_LINE_KEEP_OUT, 48)
			| BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, reserved0, 0) ) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL, (uint32_t) Register) ;
		
		/* wait for confirmation of transition to HDMI mode */
		HdmiModeSet = 0 ;

		/* number of frames to delay while waiting to switch from DVI to HDMI mode */		
#define BHDM_P_TRANSITION_TIMEOUT_FRAMES 5
		timeoutFrames = BHDM_P_TRANSITION_TIMEOUT_FRAMES ; 
		
		pVideoInfo = BFMT_GetVideoFormatInfoPtr(NewHdmiSettings->eInputVideoFmt) ;

		/* configure wait delay based on the referesh rate */
		if (pVideoInfo->ulVertFreq >= 5994) 
			FrameDelay = 17 ;
		else if (pVideoInfo->ulVertFreq >= 5000) 
			FrameDelay = 20 ;
		else if (pVideoInfo->ulVertFreq >= 2997) 
			FrameDelay = 33 ;
		else /* 24/25 Hz refresh rate */
			FrameDelay = 42 ;		
		
		BHDM_ClearHotPlugInterrupt(hHDMI);
		do
		{
			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL) ;
			HdmiModeSet = (uint8_t) BCHP_GET_FIELD_DATA(Register, HDMI_SCHEDULER_CONTROL, HDMI_MODE_ACTIVE) ;
			if (HdmiModeSet)
				break ;
			
			BKNI_Sleep(FrameDelay) ;

			/* Break out if HPD was pulsed. */
			BHDM_CheckHotPlugInterrupt(hHDMI, &bHPInterrupt);
			if (bHPInterrupt == true)
				break ;

		} while ( timeoutFrames-- ) ;

		/* report error if timeout waiting to transition to HDMI mode */
		if (!timeoutFrames && !HdmiModeSet && !bHPInterrupt)
		{
			BDBG_ERR(("Timeout waiting (%d frames * %dms) %dms to transition to HDMI Mode", 
				BHDM_P_TRANSITION_TIMEOUT_FRAMES, FrameDelay, 
				(uint16_t) (BHDM_P_TRANSITION_TIMEOUT_FRAMES * FrameDelay))) ;
		}

		if (!HdmiModeSet)
		{
			BDBG_ERR(("Timeout Error transitioning to HDMI mode")) ;
			rc = BERR_TIMEOUT ;
			goto done ;
		}

		BDBG_WRN(("Output Mode:  HDMI (Audio+Video)")) ;
	} /* END if HDMI Mode */
#if BDBG_DEBUG_BUILD	
	else
		BDBG_WRN(("Output Mode: DVI (Video Only)")) ;
#endif		

	/* 
	At initial installation of the Audio Rate Change callback, there is no initial
	callback; force an update here to use settings contained in the NewHdmiSettings 
	*/
	if (hHDMI->DeviceSettings.eAudioSamplingRate != NewHdmiSettings->eAudioSamplingRate)	
	{	
		BAVC_Audio_Info AudioData ;       
		AudioData.eAudioSamplingRate = NewHdmiSettings->eAudioSamplingRate ;
			
		BKNI_EnterCriticalSection() ;
			BHDM_AudioVideoRateChangeCB_isr(hHDMI, 
				BHDM_Callback_Type_eManualAudioChange, &AudioData) ;
		BKNI_LeaveCriticalSection() ;
	}

	/* note if the Video Format Has Changed */	
	if (NewHdmiSettings->eInputVideoFmt != hHDMI->DeviceSettings.eInputVideoFmt)
		HdmiVideoFormatChange = true ;
	
	/* save the new HDMI Settings we used to enable the HDMI device for this HDMI handle */
	hHDMI->DeviceSettings = *NewHdmiSettings ;
	
	/* format and send out HDMI info packets */
	if  ((hHDMI->DeviceSettings.eOutputFormat == BHDM_OutputFormat_eHDMIMode))
	{
		/* set and enable the General Control Packet */
		/* keep AvMute at its current state in case modified earlier */
		BHDM_CHECK_RC(rc, BHDM_SetAvMute(hHDMI, hHDMI->AvMuteState)) ;
		
		/* set and enable the AVI Info Frame */
		BHDM_CHECK_RC(rc, BHDM_SetAVIInfoFramePacket(hHDMI, 
			&hHDMI->DeviceSettings.stAviInfoFrame)) ;

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
		/* set and enable Gamut Metadata packets if xvYCC; clear otherwise */
		if ((hHDMI->DeviceSettings.eColorimetry == BAVC_MatrixCoefficients_eXvYCC_601) 
		|| 	(hHDMI->DeviceSettings.eColorimetry == BAVC_MatrixCoefficients_eXvYCC_709)) 
			BHDM_CHECK_RC(rc, BHDM_P_SetGamutMetadataPacket(hHDMI)) ;
		else
			BHDM_CHECK_RC(rc, BHDM_DisablePacketTransmission(hHDMI, BHDM_PACKET_eGamutMetadata_ID)) ;

		/* set the color depth */
		BHDM_CHECK_RC(rc, BHDM_SetColorDepth(hHDMI, 
			&hHDMI->DeviceSettings.stColorDepth));
#endif		
		
		/* set and enable the Audio Info Frame */
		BHDM_CHECK_RC(rc, BHDM_SetAudioInfoFramePacket(hHDMI, 
			&hHDMI->DeviceSettings.stAudioInfoFrame)) ;
		
		/* set and enable the Source Product Description Info Frame */
		BHDM_CHECK_RC(rc, BHDM_SetSPDInfoFramePacket(hHDMI)) ;

		/* set and enable the Vendor Specific Info Info Frame */
		BHDM_CHECK_RC(rc, BHDM_SetVendorSpecificInfoFrame(hHDMI, 
					&hHDMI->DeviceSettings.stVendorSpecificInfoFrame)) ;		
	}


	/* recenter/initialize the FIFO only if the video format has changed */	
	if (HdmiVideoFormatChange || hHDMI->DeviceSettings.bForceEnableDisplay)
	{
		BHDM_CHECK_RC(rc, BHDM_InitializeDriftFIFO(hHDMI)) ;
	}

#if BHDM_CONFIG_DEBUG_FIFO
	{
		BHDM_P_FIFO_DATA FifoData ;

		/* dump FIFO pointers for debugging */		
		BHDM_P_CaptureFIFOData(hHDMI, &FifoData) ;
		
		/* re-enable FIFO Interrupts */	
		BHDM_P_EnableFIFOInterrupts(hHDMI, true) ;
	}
#endif
#endif

/* enable RSEN interrupt AFTER	turning ON TMDS lines */
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT

#if !BHDM_CONFIG_RSEN_POLLING
#if BHDM_CONFIG_DEBUG_RSEN
	BDBG_WRN(("Enabling RSEN Interrupts")) ;
#endif
	BINT_EnableCallback(hHDMI->hCallback[MAKE_INTR_ENUM(RSEN)]) ;
#endif

#endif	


#if BHDM_CONFIG_BTMR_SUPPORT

#if BHDM_CONFIG_FORMAT_CHANGE_DETECTION_SUPPORT
	/* schedule check to detect format changes */
	BHDM_DEBUG_P_Monitor_FormatChangesStart(hHDMI) ;
#endif

#endif

done:
	hHDMI->DeviceSettings.bForceEnableDisplay = false ;	
	BDBG_LEAVE(BHDM_EnableDisplay) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_DisableDisplay
Summary: Disable Display output from the HDMI Tx 
*******************************************************************************/
BERR_Code BHDM_DisableDisplay(
   BHDM_Handle hHDMI  /* [in] HDMI handle */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint32_t Register ;

	BDBG_ENTER(BHDM_DisableDisplay) ;

/* disable RSEN interrupt BEFORE  turning OFF TMDS lines */
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT

#if !BHDM_CONFIG_RSEN_POLLING
#if BHDM_CONFIG_DEBUG_RSEN
	BDBG_WRN(("Disabling RSEN Interrupts")) ;
#endif
	BINT_DisableCallback(hHDMI->hCallback[MAKE_INTR_ENUM(RSEN)]) ;	
#endif 

#endif
		
	/* turn off the TMDS data lines */
	BHDM_EnableTmdsOutput(hHDMI, false) ;

	/* turn off the TMDS Clock separately */
#if BHDM_CONFIG_65NM_SUPPORT
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL) ;
		Register |= BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_CK_PWRDN) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;
#else
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL) ;
#if BHDM_CONFIG_SWAP_DEFAULT_PHY_CHANNELS
		Register |= BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, TX_2_PWRDN) ;
#else
		Register |= BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, TX_CK_PWRDN) ;
#endif
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL, Register) ;
#endif
	

	/* set flag to force EnableDisplay if/when TMDS lines are re-enabled */
	/* See BHDM_EnableDisplay */
	hHDMI->DeviceSettings.bForceEnableDisplay = true ;

	/* indicate HDMI Output (Clock + Data) is completely DISABLED */
	hHDMI->OutputEnabled = false ;
	
	BDBG_LEAVE(BHDM_DisableDisplay) ;
	return rc ;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
/******************************************************************************
BERR_Code BHDM_SetVideoBlank
Summary: Set the Video Blank (True/False) functionality for HDMI.
*******************************************************************************/
BERR_Code BHDM_SetVideoBlank(
	BHDM_Handle hHDMI,              /* [in] HDMI handle */
	bool bEnableBlank              /* [in] boolean to enable/disable */
)
{
	BERR_Code		rc = BERR_SUCCESS;
	uint32_t 			Register ;
		
	Register = BREG_Read32(hHDMI->hRegister,  BCHP_HDMI_CP_TST) ;
	Register &= 
		~(BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ONES) 
		| BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ZEROS)) ;

	if ( bEnableBlank )
		Register |= BCHP_FIELD_DATA(HDMI_CP_TST,  I_TST_FORCE_VIDEO_ALL_ZEROS, 1) ;
	else
		Register |= BCHP_FIELD_DATA(HDMI_CP_TST,  I_TST_FORCE_VIDEO_ALL_ZEROS, 0) ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_TST, Register) ;

	return rc;

}
#else
BERR_Code BHDM_SetVideoBlank(
	BHDM_Handle hHDMI,              /* [in] HDMI handle */
	bool bEnableBlank              /* [in] boolean to enable/disable */
)
{
	return BERR_SUCCESS;
}
#endif
	
/******************************************************************************
BERR_Code BHDM_Close
Summary: Close the HDMI connection to the HDMI Rx.
*******************************************************************************/
BERR_Code BHDM_Close(
   BHDM_Handle hHDMI  /* [in] HDMI handle */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint32_t i ;
	BHDM_EDID_P_VideoDescriptor *pVideoDescriptor ;

	
	BDBG_ENTER(BHDM_Close) ;
	BDBG_ASSERT( hHDMI );

#if BHDM_CONFIG_BTMR_SUPPORT
	BHDM_P_DestroyTimer(hHDMI, hHDMI->TimerHotPlug, BHDM_P_TIMER_eHotPlug) ;
	BHDM_P_DestroyTimer(hHDMI, hHDMI->TimerFormatChange, BHDM_P_TIMER_eFormatDetection) ;
#endif

	/* make sure display is disabled  */
	BHDM_DisableDisplay(hHDMI);

	BHDM_P_PowerOffPhy (hHDMI) ;

	BHDM_P_DisableInterrupts(hHDMI) ;
		
	/* Disable and Destroy the HDMI Callbacks */
	for ( i = 0; i < MAKE_INTR_ENUM(LAST); i++ )
	{
		/* all interrupts are now created; destroy all on close */		
		BHDM_CHECK_RC( rc, BINT_DestroyCallback( hHDMI->hCallback[i] ) );
	}
	
	BKNI_DestroyEvent((hHDMI->BHDM_EventHDCP)) ;
	
	BKNI_DestroyEvent((hHDMI->BHDM_EventHotPlug)) ;
	BKNI_DestroyEvent((hHDMI->BHDM_EventFIFO)) ;
	BKNI_DestroyEvent((hHDMI->BHDM_EventRAM)) ;
	BKNI_DestroyEvent((hHDMI->BHDM_EventHDCPRiValue)) ;
	BKNI_DestroyEvent((hHDMI->BHDM_EventHDCPPjValue)) ;
	BKNI_DestroyEvent((hHDMI->BHDM_EventHDCPRepeater)) ;
	BKNI_DestroyEvent((hHDMI->BHDM_EventRxSense)) ;

	/* delete previous video descriptors if they exist */
	if (!BLST_Q_EMPTY(&hHDMI->AttachedEDID.VideoDescriptorList))
	{
		for (pVideoDescriptor=BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList) ;
			pVideoDescriptor ; 
			pVideoDescriptor=BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList)) 
		{ 
			BLST_Q_REMOVE_HEAD(&hHDMI->AttachedEDID.VideoDescriptorList, link); 
			BKNI_Free(pVideoDescriptor); /* free memory */
		}
	}	
	
#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		/* release the CEC	*/
		BCHP_PWR_ReleaseResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CLK
		/*  power down the TX Clock */
		BCHP_PWR_ReleaseResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CLK);
#endif

	/* free memory associated with the HDMI handle */
	BKNI_Memset(hHDMI, 0, sizeof(BHDM_P_Handle)) ;

	BKNI_Free( (void *) hHDMI) ;
	
done:
	BDBG_LEAVE(BHDM_Close) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_ClearHotPlugInterrupt
Summary: Cler the HotPlug Interrupt.
*******************************************************************************/
BERR_Code BHDM_ClearHotPlugInterrupt(
   BHDM_Handle hHDMI        /* [in] HDMI handle */
)
{
	BDBG_ENTER(BHDM_ClearHotPlugInterrupt) ;
	BDBG_ASSERT( hHDMI );

	BHDM_P_ClearHotPlugInterrupt(hHDMI);

	BDBG_LEAVE(BHDM_ClearHotPlugInterrupt) ;
	return BERR_SUCCESS ;
}


/******************************************************************************
BERR_Code BHDM_CheckHotPlugInterrupt
Summary: Check the HotPlug Interrupt.
*******************************************************************************/
BERR_Code BHDM_CheckHotPlugInterrupt(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bHotPlugInterrupt	/* [out] Interrupt asserted or not */
)
{
	BDBG_ENTER(BHDM_CheckHotPlugInterrupt) ;
	BDBG_ASSERT( hHDMI );

	BHDM_P_CheckHotPlugInterrupt(hHDMI, bHotPlugInterrupt) ;

	BDBG_LEAVE(BHDM_CheckHotPlugInterrupt) ;
	return BERR_SUCCESS ;
}


/******************************************************************************
BERR_Code BHDM_RxDeviceAttached
Summary: Check for an attached Rx Device.
*******************************************************************************/
BERR_Code BHDM_RxDeviceAttached(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
	uint8_t *bDeviceAttached
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint8_t status;

	BDBG_ENTER(BHDM_RxDeviceAttached) ;
	
	BDBG_ASSERT( hHDMI );


	/* DVO interface is always attached; as opposed to HDMI Port */
	if (hHDMI->eOutputPort == BHDM_OutputPort_eDVO)
	{
		*bDeviceAttached = 1 ;
		return rc ;
	}

	/* read hotplug register and check if connected */
	BKNI_EnterCriticalSection() ;
		BHDM_P_RxDeviceAttached_isr(hHDMI, &status) ;
	BKNI_LeaveCriticalSection() ;


	if (status) {
		*bDeviceAttached = 1 ;
	}
	else {
		*bDeviceAttached = 0 ;
		BDBG_MSG(("RxDeviceAttached: No DVI/HDMI Device Detected")) ;
	}

	BDBG_LEAVE(BHDM_RxDeviceAttached) ;
	return rc ;
}



/******************************************************************************
BERR_Code BHDM_ResetHdmiRx
Summary: Reset HDMI/DVI Rx device (for non-compliant receivers)
*******************************************************************************/
BERR_Code BHDM_ResetHdmiRx(
   BHDM_Handle hHDMI	    /* [in] HDMI handle */
)
{
	BERR_Code      rc = BERR_SUCCESS;

	BDBG_ENTER(BHDM_ResetHdmiRx) ;
	BDBG_ASSERT( hHDMI );

	BHDM_EnableTmdsOutput(hHDMI, false) ;
	BKNI_Sleep(200) ;  /* 200 ms */
	BHDM_EnableTmdsOutput(hHDMI, true) ;

	BDBG_LEAVE(BHDM_ResetHdmiRx) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_SetAvMute
Summary: Set the AvMute (True/False) functionality for HDMI.
*******************************************************************************/
BERR_Code BHDM_SetAvMute(
   BHDM_Handle hHDMI,              /* [in] HDMI handle */
   bool bEnableAvMute              /* [in] boolean to enable/disable */
)
{
	uint32_t Register ;
	uint32_t AvMuteFlag ;
	BERR_Code rc = BERR_SUCCESS;
	uint8_t vsync ;

	BDBG_ENTER(BHDM_SetAvMute) ;
	BDBG_ASSERT( hHDMI );

	/* AvMute valid for HDMI only */
	if  (hHDMI->DeviceSettings.eOutputFormat != BHDM_OutputFormat_eHDMIMode)
		goto done ;
	

	if (!hHDMI->tmdsEnabled) {
		/* warn, but keep going */
		BDBG_WRN(("BHDM_SetAvMute called while display is disabled"));
	}

	if (bEnableAvMute)
	{
		AvMuteFlag = 0x01 ; /* AvMute ON */
		vsync = 0 ;
	}
	else 
	{
		AvMuteFlag = 0x10 ; /* AvMute OFF */
		vsync = 1 ;
	}

	BDBG_MSG(("AvMute %d", bEnableAvMute)) ;
	

#if BHDM_CONFIG_REGISTER_GCP_FOR_AV_MUTE

	/* Modify AV Mute settings */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_GCP_WORD_1);
	Register &= ~ BCHP_MASK(HDMI_GCP_WORD_1, GCP_SUBPACKET_BYTE_0);
	Register |= BCHP_FIELD_DATA(HDMI_GCP_WORD_1, GCP_SUBPACKET_BYTE_0, AvMuteFlag);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_GCP_WORD_1, Register) ; 

	/* Enable GCP packets */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_GCP_CONFIG);
	Register &= ~ BCHP_MASK(HDMI_GCP_CONFIG, GCP_ENABLE);
	Register |= BCHP_FIELD_DATA(HDMI_GCP_CONFIG, GCP_ENABLE,  1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_GCP_CONFIG, Register) ; 
		
#else

#if BHDM_CONFIG_DISABLE_MUX_VSYNC_ON_AVMUTE
	/* disable/enable MUX_VSYNC when muting/unmuting */ 	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG) ;
	Register &= ~BCHP_MASK(HDMI_CP_CONFIG, I_MUX_VSYNC) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, I_MUX_VSYNC, vsync) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG, Register) ;
#endif


	/* Disable the GCP (RAM_PACKET_0) in the Active RAM */
	/* Preserve other Active RAM Packets */	
	BHDM_CHECK_RC(rc, BHDM_DisablePacketTransmission(hHDMI, BHDM_PACKET_eGCP_ID)) ;

	/* Modify GCP Registers in Packet RAM */
	Register = 
		  BCHP_FIELD_DATA(HDMI_RAM_GCP_0, HEADER_BYTE_0,  0x03) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_0, HEADER_BYTE_1,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_0, HEADER_BYTE_2,  0x00) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_0, Register) ; 

	Register =	
		  BCHP_FIELD_DATA(HDMI_RAM_GCP_1, SUBPACKET_1_BYTE_3,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_1, SUBPACKET_1_BYTE_2,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_1, SUBPACKET_1_BYTE_1,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_1, SUBPACKET_1_BYTE_0, AvMuteFlag) ;
	/* Write all four identical GCP subpackets bytes 3-0 */
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_1, Register) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_3, Register) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_5, Register) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_7, Register) ; 

	Register = 
		  BCHP_FIELD_DATA(HDMI_RAM_GCP_2, SUBPACKET_1_BYTE_6,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_2, SUBPACKET_1_BYTE_5,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_2, SUBPACKET_1_BYTE_4,  0x00) 
		| BCHP_FIELD_DATA(HDMI_RAM_GCP_2, SUBPACKET_1_PARITY_BYTE_BCH_64_56,  0x00) ;

	/* Write all four identical GCP subpackets bytes 6-4 */
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_2, Register) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_4, Register) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_6, Register) ; 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RAM_GCP_8, Register) ; 

	/* Re-Enable the GCP in the Active RAM */
	/* Preserve current Active RAM packets */
	BHDM_CHECK_RC(rc, BHDM_EnablePacketTransmission(hHDMI, BHDM_PACKET_eGCP_ID)) ;
	
#endif

	hHDMI->AvMuteState = bEnableAvMute ;

done:
	BDBG_LEAVE(BHDM_SetAvMute) ;
	return rc ;
}  /* END BHDM_SetAvMute */


/***************************************************************************
BHDM_P_ConfigurePixelRepeater
Summary: Configure the Pixel Repeat Register
****************************************************************************/
static BERR_Code BHDM_P_ConfigurePixelRepeater(
	BHDM_Handle hHDMI,           /* [in] HDMI handle */
	BFMT_VideoFmt eVideoFmt, /* [in] Input Display Format */
	BAVC_HDMI_PixelRepetition ePixelRepetition /* [in] Input Pixel Repetition */
)
{
	BERR_Code      rc = BERR_SUCCESS;
	uint32_t  Register ;
	uint8_t pixelRepeat;

	BDBG_ENTER(BHDM_P_ConfigurePixelRepeater) ;
	

#ifdef BHDM_CONFIG_BLURAY_PLATFORMS
	pixelRepeat = ePixelRepetition;
#else
	/**********************
	VEC will handle pixel replication up to 54Mhz only. For 480p 4x pixel repetition
	(applicable for 7420 and later only) 27x4=108Mhz, HDMI core will handle a 2:1 pixel
	replication from the 54Mhz input from VDC 
	***********************/
	if (ePixelRepetition == BAVC_HDMI_PixelRepetition_e4x)
		pixelRepeat = 1;
	else
		/* Actual pixel repeat is done by the VEC up to 54Mhz  */
	pixelRepeat = 0;
#endif	

	
	switch (eVideoFmt) 
	{

	case BFMT_VideoFmt_eNTSC :  
	case BFMT_VideoFmt_eNTSC_J :         /* 480i (Japan) */

#if BHDM_CONFIG_ANALOG_TRANSLATION_SUPPORT
	case BFMT_VideoFmt_e720x482_NTSC :	 /* 720x482i NSTC-M for North America */		
   	case BFMT_VideoFmt_e720x482_NTSC_J : /* 720x482i Japan */
	case BFMT_VideoFmt_e720x483p :			/* 720x483p */
#endif

	/* PAL Formats */
	case BFMT_VideoFmt_ePAL_B  :
	case BFMT_VideoFmt_ePAL_B1 :
	case BFMT_VideoFmt_ePAL_D  :
	case BFMT_VideoFmt_ePAL_D1 :
	case BFMT_VideoFmt_ePAL_G  :
	case BFMT_VideoFmt_ePAL_H  :
	case BFMT_VideoFmt_ePAL_I  :
	case BFMT_VideoFmt_ePAL_K  :
	case BFMT_VideoFmt_ePAL_M  :
	case BFMT_VideoFmt_ePAL_N  :
	case BFMT_VideoFmt_ePAL_NC :
	case BFMT_VideoFmt_eSECAM :  /* 576i LDK/SECAM (France :Russia) */
	
		/* For above formats Pixel Repeat Handled by VEC */
		/* Pixel Repeat Register should still be 0 */
	
	case BFMT_VideoFmt_e1080i  :
	case BFMT_VideoFmt_e720p   :
	case BFMT_VideoFmt_e480p   :
		
	case BFMT_VideoFmt_eDVI_640x480p :     /* DVI Safe mode for computer monitors */
		
	case BFMT_VideoFmt_e1080i_50Hz :       /* HD 1080i 50Hz (Europe) */
	case BFMT_VideoFmt_e720p_50Hz :        /* HD 720p 50Hz (Australia) */
	case BFMT_VideoFmt_e576p_50Hz :        /* HD 576p 50Hz (Australia) */
	case BFMT_VideoFmt_e1250i_50Hz :
		
	case BFMT_VideoFmt_e720p_24Hz : 
	case BFMT_VideoFmt_e1080p_24Hz :      /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	case BFMT_VideoFmt_e1080p_25Hz :      /* HD 1080p 25Hz, 2640 sample per line, SMPTE 274M-1998 */
	case BFMT_VideoFmt_e1080p_30Hz :      /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */
	case BFMT_VideoFmt_e1080p_50Hz :	  /* HD 1080p 50Hz, 2200 sample per line, SMPTE 274M-1998 */
	case BFMT_VideoFmt_e1080p :			  /* HD 1080p 60Hz, 2200 sample per line, SMPTE 274M-1998 */

	/* 3D Formats */
	case BFMT_VideoFmt_e720p_60Hz_3DOU_AS:
	case BFMT_VideoFmt_e720p_50Hz_3DOU_AS:
	case BFMT_VideoFmt_e720p_24Hz_3DOU_AS:
	case BFMT_VideoFmt_e720p_30Hz_3DOU_AS:		
	case BFMT_VideoFmt_e1080p_24Hz_3DOU_AS:
	case BFMT_VideoFmt_e1080p_30Hz_3DOU_AS:
		
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	case BFMT_VideoFmt_e1080p_24Hz_3D :	  /* HD 1080p 24Hz 3D, 2750 sample per line */
	case BFMT_VideoFmt_e720p_3D :	          /* HD 720p 60Hz 3D, 1650 sample per line */
	case BFMT_VideoFmt_e720p_50Hz_3D :	  /* HD 720p 50Hz 3D, 1980 sample per line */
#endif
		break ;

	default :
		/* HDMI can now support PC formats other than VGA  */
		/* if not specified as a PC format return INVALID PARAMETER */
		
		if (! BFMT_IS_VESA_MODE(eVideoFmt))
		{
			BDBG_ERR(("Invalid eVideoFmt : %d", eVideoFmt)) ;
			rc = BERR_INVALID_PARAMETER ;
		}
	}

	/* Set Pixel Repititions value / Preserve other HDMI_MISC_CONTROL bits */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_MISC_CONTROL) ;
	Register &= ~BCHP_MASK(HDMI_MISC_CONTROL, PIXEL_REPEAT) ;
	Register |= BCHP_FIELD_DATA(HDMI_MISC_CONTROL, PIXEL_REPEAT, pixelRepeat) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_MISC_CONTROL, Register); 
	
	BDBG_MSG(("Pixel Repeater Register: %X", Register)) ;

	BDBG_LEAVE(BHDM_P_ConfigurePixelRepeater) ;
	return rc ;
}



/******************************************************************************
BERR_Code BHDM_InitializeDriftFIFO
Summary: Initialize the Drift FIFO
*******************************************************************************/
BERR_Code BHDM_InitializeDriftFIFO(
   BHDM_Handle hHDMI		/* [in] HDMI handle */
)
{
	BERR_Code   rc = BERR_SUCCESS;
	BREG_Handle hRegister ;
	uint32_t    Register ;
	uint8_t timeoutMs ;
	uint8_t RecenterDone ;
	uint8_t bHPInterrupt = false ;
	uint32_t timeDelayed;
	bool masterMode;
	bool bAuthenticated ;

	BDBG_ENTER(BHDM_InitializeDriftFIFO) ;
	BDBG_ASSERT( hHDMI );

	hRegister = hHDMI->hRegister ;

	/* do not recenter FIFO if HDCP is enabled */
	Register = BREG_Read32(hRegister, BCHP_HDMI_CP_CONFIG) ;
	bAuthenticated = BCHP_GET_FIELD_DATA(Register, HDMI_CP_CONFIG, I_MUX_VSYNC) ;
	if (bAuthenticated)
	{
		BDBG_WRN(("HDCP is enabled ; <RECENTER FIFO> aborted...")) ;
		goto done ;
	}
	

	rc = BHDM_GetHdmiDataTransferMode(hHDMI, &masterMode);
	
	/**** 06 Recenter the Drift FIFO */
	/*	
	**	Read the  HDMI_FIFO_CTL Register...  
	**	Clear the RECENTER bit               
	**	Write the HDMI_FIFO_CTL Register     
	**	Set   the RECENTER bit               
	**	Read      RECENTER_DONE bit until equal to one 
	*/

	/* set   the FIFO CTL RECENTER bit */
	if (!masterMode)
	{
		Register = BREG_Read32(hRegister, BCHP_HDMI_FIFO_CTL) ;
                Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved0) ;
                Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved1) ;
                Register &= ~BCHP_MASK(HDMI_FIFO_CTL, RECENTER) ;
		BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 

			Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, RECENTER, 1) ;
		BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 

#if BHDM_CONFIG_DEBUG_FIFO
		BDBG_WRN(("<RECENTER FIFO>")) ;
#endif
	
                BHDM_ClearHotPlugInterrupt(hHDMI);
                for (timeDelayed=0; timeDelayed<1000; timeDelayed+=50) 
                {
                        BKNI_Delay(50) ;

                        /* Bail out if HPD was pulsed. */
                        BHDM_CheckHotPlugInterrupt(hHDMI, &bHPInterrupt);
                        if (bHPInterrupt == true)
                                break ;
                }

                /* set   the FIFO CTL RECENTER bit */
		Register = BREG_Read32(hRegister, BCHP_HDMI_FIFO_CTL) ;
                Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved0) ;
                Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved1) ;
                Register &= ~BCHP_MASK(HDMI_FIFO_CTL, RECENTER) ;
		BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 

			Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, RECENTER, 1) ;
		BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 
	

                /* Wait for RECENTER_DONE bit to be equal to one */
                timeoutMs = 10 ;
                do
                {
			Register = BREG_Read32(hRegister, BCHP_HDMI_FIFO_CTL) ;
                        RecenterDone = BCHP_GET_FIELD_DATA(Register, HDMI_FIFO_CTL, RECENTER_DONE) ;
                        if (RecenterDone)
                                break ;

#if BHDM_CONFIG_DEBUG_FIFO
			BDBG_WRN(("Wait for <RECENTER FIFO> %dms", timeoutMs)) ;
#endif
                        BKNI_Sleep(1) ;
                } while ( timeoutMs-- ) ;
		
                if (!RecenterDone)
                {
			BDBG_ERR(("<RECENTER FIFO> Timed out...")) ;
                        rc = BERR_TIMEOUT ;
                        goto done ;
                }

#if BHDM_CONFIG_DEBUG_FIFO
		BDBG_WRN(("<RECENTER FIFO> done")) ;
#endif
		
	}
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_FIFO_CTL) ;
	Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved0) ;
	Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved1) ;
	
	/* USE_FULL should always be set to 0 in master mode */
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, masterMode?0:1) ;
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_EMPTY, 1) ;
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, reserved0, 0) ;
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, reserved1, 0) ;
	BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 

done:
	BDBG_LEAVE(BHDM_InitializeDriftFIFO) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_CheckForValidVideo
Summary: Check the input video display format to the HDMI core is enabled/stable
*******************************************************************************/
BERR_Code BHDM_CheckForValidVideo(
	BHDM_Handle hHDMI              /* [in] HDMI handle */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	BREG_Handle hRegister ;
	bool bMasterMode ;
	BHDM_P_FIFO_DATA FifoData ;
	uint8_t uiInitialRdAddr , uiInitialWrAddr ;
	uint8_t uiCurrentRdAddr , uiCurrentWrAddr ;

	hRegister = hHDMI->hRegister ;
	BKNI_Memset(&FifoData, 0, sizeof(BHDM_P_FIFO_DATA));

	/* make sure PLL is up/running */
#if BHDM_CONFIG_65NM_SUPPORT
	{
		uint32_t Register ;
		bool bPllPoweredDown ;
	
		/* check if pll is powered on */
		Register = BREG_Read32(hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL) ;

		/* use bUnderFlow variable to check pll pwrdn status */
		bPllPoweredDown = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PLL_PWRDN) ;
		if (bPllPoweredDown)
		{
			BDBG_ERR(("PLL is powered down; HDCP Authentication cannot proceed")) ;
			rc = BHDM_HDCP_PLL_PWRDN ;
			goto done ;
		}
		
		BDBG_MSG(("PLL power... OK")) ;
	}
#endif

        /* Check Drift FIFO, but do so only if we're in Slave mode */
	rc = BHDM_GetHdmiDataTransferMode(hHDMI, &bMasterMode);
	if (rc)
	{
		BDBG_ERR(("Unable to determine Master Mode setting")) ;
		return (rc) ;
	}

	if (bMasterMode)
		goto done ;
		
	{
		uint8_t uiInitialFullness, uiCurrentFullness ;
		
		BHDM_P_CaptureFIFOData(hHDMI, &FifoData) ;
			uiInitialFullness = FifoData.uiFullness ;
			uiInitialRdAddr = FifoData.uiRdAddr ;
			uiInitialWrAddr = FifoData.uiWrAddr ;
		
		if ((FifoData.bUnderFlow) || (FifoData.bOverFlow))
		{
 			 if ( (uiInitialFullness > 178) || (uiInitialFullness < 78 )) 
			 {
			 	BDBG_WRN(("FIFO is railed, fullness: %d give up and try again later", uiInitialFullness)) ;
				BHDM_InitializeDriftFIFO(hHDMI);
				rc = BHDM_HDCP_FIFO_UNDERFLOW ;
				goto done ;
 			 }

			 /* valid FIFO */
		 	/* confirm current fullness/stability */
			BHDM_P_CaptureFIFOData(hHDMI, &FifoData) ;
				uiCurrentFullness = FifoData.uiFullness ;
				uiCurrentRdAddr = FifoData.uiRdAddr ;
				uiCurrentWrAddr = FifoData.uiWrAddr ;

			if ((uiInitialRdAddr == uiCurrentRdAddr) 
			|| (uiInitialWrAddr == uiCurrentWrAddr))
			{
				BDBG_ERR(("Video to HDMI Core is stalled...")) ;
				rc = BHDM_UNSUPPORTED_VIDEO_FORMAT ;
				goto done ;
			}

#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif
			
			if (ABS(uiCurrentFullness - uiInitialFullness) > 6)
			 {
			 	BDBG_WRN(("FIFO is unstable (1) ; Initial Fullness: %d Current Fullness %d, recenter and try again",
					uiInitialFullness, uiCurrentFullness)) ;
				BHDM_InitializeDriftFIFO(hHDMI);
				rc = BHDM_HDCP_FIFO_UNDERFLOW ;
				goto done ;
 			 }
			

		 	/* confirm current fullness/stability */
			BHDM_P_CaptureFIFOData(hHDMI, &FifoData) ;
				uiInitialFullness = FifoData.uiFullness ;
				uiInitialRdAddr = FifoData.uiRdAddr ;
				uiInitialWrAddr = FifoData.uiWrAddr ;
			
			if (ABS(uiCurrentFullness - uiInitialFullness) > 6)
			 {
			 	BDBG_WRN(("FIFO is unstable (2) ; Initial Fullness: %d Current Fullness %d, recenter and try again",
					uiInitialFullness, uiCurrentFullness)) ;
				BHDM_InitializeDriftFIFO(hHDMI);
				rc = BHDM_HDCP_FIFO_UNDERFLOW ;
				goto done ;
 			 }
		}
	}

	/* FIFO is centered if between 122 <= x <= 134 */
#if defined(HUMAX_PLATFORM_BASE)
	if (!((FifoData.uiFullness >= 122) &&  (FifoData.uiFullness <= 134)) && !bMasterMode)
#else
	if (!((FifoData.uiFullness >= 122) &&  (FifoData.uiFullness <= 134)))
#endif
	{
		BDBG_WRN(("FIFO fullness %d is not in range  122 <= x <= 134 ; <RECENTER FIFO>",
			FifoData.uiFullness)) ;
		BHDM_InitializeDriftFIFO(hHDMI);
		rc = BHDM_HDCP_FIFO_UNDERFLOW ;
		goto done ;
	}
	BDBG_WRN(("Video data to HDMI Core... OK")) ;



done:
	return rc ;
		
}


/******************************************************************************
BERR_Code BHDM_P_ConfigureInputVideoFmt
Summary: Set the input video display format to the HDMI core
*******************************************************************************/
static BERR_Code BHDM_P_ConfigureInputVideoFmt(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
   BFMT_VideoFmt eVideoFmt,                   /* [in] input format to HDMI core */
   BAVC_HDMI_PixelRepetition ePixelRepetition /* [in] Input Pixel Repetition */
)
{
#if BDBG_DEBUG_BUILD	
	static const char * sPolarity[] = {"-", "+"} ;
#endif	
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;
	uint8_t     i, index ;
	
	typedef struct BHDM_BFMT_VideoFormat
	{
		BFMT_VideoFmt eVideoFmt ;
		uint8_t index ;
	} BHDM_BFMT_VideoFormat ;
	
	static const  BHDM_BFMT_VideoFormat BHDM_SupportedVideoFormats[] =
	{
		{BFMT_VideoFmt_eDVI_640x480p,  BHDM_InputVideoFmt_e640x480p}, /* DVI/HDMI Safe mode */

	
		{BFMT_VideoFmt_eNTSC,   BHDM_InputVideoFmt_e480i},       /* 480i,NSTC-M for North America */
		{BFMT_VideoFmt_eNTSC_J, BHDM_InputVideoFmt_e480i},   /* 480i (Japan) */
		{BFMT_VideoFmt_e480p,   BHDM_InputVideoFmt_e480p},   /* HD 480p */

#if BHDM_CONFIG_ANALOG_TRANSLATION_SUPPORT
		{BFMT_VideoFmt_e720x482_NTSC, BHDM_InputVideoFmt_e480i}, /* 720x482i NSTC-M for North America */
		{BFMT_VideoFmt_e720x482_NTSC_J, BHDM_InputVideoFmt_e480i}, /* 720x482i Japan */
		{BFMT_VideoFmt_e720x483p, BHDM_InputVideoFmt_e480p}, 
#endif			

		{BFMT_VideoFmt_e720p,   BHDM_InputVideoFmt_e720p},   /* HD 720p */
		{BFMT_VideoFmt_e720p_24Hz,   BHDM_InputVideoFmt_e720p_24Hz},   /* HD 720p */
		{BFMT_VideoFmt_e1080i,  BHDM_InputVideoFmt_e1080i},  /* HD 1080i */

		{BFMT_VideoFmt_e1080p_24Hz,  BHDM_InputVideoFmt_e1080p_24Hz},  /* HD 1080p */
		{BFMT_VideoFmt_e1080p_25Hz,  BHDM_InputVideoFmt_e1080p_25Hz},  /* HD 1080p */
		{BFMT_VideoFmt_e1080p_30Hz,  BHDM_InputVideoFmt_e1080p_30Hz},  /* HD 1080p */

#if BHDM_CONFIG_1080P_5060HZ_SUPPORT
		{BFMT_VideoFmt_e1080p,  BHDM_InputVideoFmt_e1080p},  /* HD 1080p */
		{BFMT_VideoFmt_e1080p_50Hz,  BHDM_InputVideoFmt_e1080p_50Hz},  /* HD 1080p */
#endif

		{BFMT_VideoFmt_e1080i_50Hz, BHDM_InputVideoFmt_e1080i_50Hz},  /* HD 1080i 50Hz (Europe) */
		{BFMT_VideoFmt_e720p_50Hz,  BHDM_InputVideoFmt_e720p_50Hz},   /* HD 720p 50Hz (Australia) */
		{BFMT_VideoFmt_e576p_50Hz,  BHDM_InputVideoFmt_e576p_50Hz},   /* HD 576p 50Hz (Australia) */
	
		{BFMT_VideoFmt_ePAL_B , BHDM_InputVideoFmt_e576i_50Hz},   /* Austrilia,*/
		{BFMT_VideoFmt_ePAL_B1, BHDM_InputVideoFmt_e576i_50Hz},   /* Hungry */
		{BFMT_VideoFmt_ePAL_D , BHDM_InputVideoFmt_e576i_50Hz},   /* China */
		{BFMT_VideoFmt_ePAL_D1, BHDM_InputVideoFmt_e576i_50Hz},  /* Poland */
		{BFMT_VideoFmt_ePAL_G , BHDM_InputVideoFmt_e576i_50Hz},   /* Europe */
		{BFMT_VideoFmt_ePAL_H , BHDM_InputVideoFmt_e576i_50Hz},   /* Europe */
		{BFMT_VideoFmt_ePAL_K , BHDM_InputVideoFmt_e576i_50Hz},   /* Europe */
		{BFMT_VideoFmt_ePAL_I , BHDM_InputVideoFmt_e576i_50Hz},    /* U.K. */ 
		{BFMT_VideoFmt_ePAL_M , BHDM_InputVideoFmt_e480i},   /* 480i (Brazil) */
		{BFMT_VideoFmt_ePAL_N , BHDM_InputVideoFmt_e576i_50Hz},   /* 576i (Paraguay,Uruguay)*/
		{BFMT_VideoFmt_ePAL_NC, BHDM_InputVideoFmt_e576i_50Hz},  /* 576i N combination (Argentina) */
		{BFMT_VideoFmt_eSECAM,  BHDM_InputVideoFmt_e576i_50Hz},  /* 576i LDK/SECAM (France :Russia) */
			
		/* 3D formats */
		{BFMT_VideoFmt_e720p_60Hz_3DOU_AS,	BHDM_InputVideoFmt_e720p_3DOU},
		{BFMT_VideoFmt_e720p_50Hz_3DOU_AS,	BHDM_InputVideoFmt_e720p_50Hz_3DOU},
		{BFMT_VideoFmt_e720p_24Hz_3DOU_AS,	BHDM_InputVideoFmt_e720p_24Hz_3DOU},		
		{BFMT_VideoFmt_e720p_30Hz_3DOU_AS,	BHDM_InputVideoFmt_e720p_30Hz_3DOU},		
		{BFMT_VideoFmt_e1080p_24Hz_3DOU_AS, BHDM_InputVideoFmt_e1080p_24Hz_3DOU},
		{BFMT_VideoFmt_e1080p_30Hz_3DOU_AS, BHDM_InputVideoFmt_e1080p_30Hz_3DOU},
			
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
		{BFMT_VideoFmt_e1080p_24Hz_3D,  BHDM_InputVideoFmt_e1080p_24Hz_3D},  /* HD 1080p 24Hz 3D */
		{BFMT_VideoFmt_e720p_3D,       BHDM_InputVideoFmt_e720p_3D},         /* HD 720p 60Hz 3D */
		{BFMT_VideoFmt_e720p_50Hz_3D,  BHDM_InputVideoFmt_e720p_50Hz_3D},  /* HD 720p 50Hz 3D */
#endif
		/* DVI formats */
		{BFMT_VideoFmt_eCUSTOM_1366x768p, BHDM_InputVideoFmt_eCustom}, 
			
		{BFMT_VideoFmt_eDVI_800x600p,  BHDM_InputVideoFmt_800x600p}, 
		{BFMT_VideoFmt_eDVI_1024x768p, BHDM_InputVideoFmt_1024x768p}, 
		{BFMT_VideoFmt_eDVI_1280x768p, BHDM_InputVideoFmt_1280x768p},
		{BFMT_VideoFmt_eDVI_1280x1024p_60Hz, BHDM_InputVideoFmt_1280x1024p},
		
		{BFMT_VideoFmt_eDVI_1280x720p_50Hz, BHDM_InputVideoFmt_1280x720p_50Hz},
		{BFMT_VideoFmt_eDVI_1280x720p, BHDM_InputVideoFmt_1280x720p}
	} ;

	uint8_t pixelRepetitionCount;


	BDBG_ENTER(BHDM_P_ConfigureInputVideoFmt) ;
	BDBG_ASSERT( hHDMI );
	
	
	/* translate the input video fmt to an index in the HDMI internal table */
	index = BHDM_InputVideoFmt_ePowerUp ;
	for (i = 0 ; i < sizeof(BHDM_SupportedVideoFormats) / sizeof(BHDM_BFMT_VideoFormat) ; i++) 
		if (eVideoFmt == BHDM_SupportedVideoFormats[i].eVideoFmt) 
		{
			index = BHDM_SupportedVideoFormats[i].index ;
			break ;
		}

	if (index == BHDM_InputVideoFmt_ePowerUp)
	{
		const BFMT_VideoInfo *pVideoInfo ;

		pVideoInfo = BFMT_GetVideoFormatInfoPtr(eVideoFmt) ;
		BDBG_ERR(("BFMT_VideoFmt: %s (%d) is UNKNOWN/UNSUPPORTED in HDM", 
			pVideoInfo->pchFormatStr, eVideoFmt)) ;
		BSTD_UNUSED(pVideoInfo) ; /* supress coverity message for non-debug builds */
		rc = BERR_NOT_SUPPORTED ;		
		goto done ;
	}

	/* the pixel repetition count/multiply */
	pixelRepetitionCount = ePixelRepetition + 1;
		
	/* display format information for debug purposes */
	BDBG_WRN(("Output Fmt:   %s %d [%s]  x %d/%d [%s]",
		BHDM_VideoFmtParams[index].FormatName,
		BHDM_VideoFmtParams[index].H_ActivePixels,
		sPolarity[BHDM_VideoFmtParams[index].V_Polarity],
		BHDM_VideoFmtParams[index].V_ActiveLinesField0,
		BHDM_VideoFmtParams[index].V_ActiveLinesField1,
		sPolarity[BHDM_VideoFmtParams[index].H_Polarity])) ;
		
#if BDBG_DEBUG_BUILD
	if (index == BHDM_InputVideoFmt_eCustom)
		BDBG_WRN(("Using Custom DVI/DVO format...")) ;
		
	BDBG_MSG(("Horz Front Porch: %3d ",
		BHDM_VideoFmtParams[index].H_FrontPorch)) ;
	BDBG_MSG(("Horz Back  Porch: %3d ",
		BHDM_VideoFmtParams[index].H_BackPorch )) ;
	BDBG_MSG(("Horz SyncPulse:   %3d ",
		BHDM_VideoFmtParams[index].H_SyncPulse )) ;
	BDBG_MSG(("                         FIELD 0     FIELD 1")) ;
	BDBG_MSG(("Vert Front Porch :        %3d          %3d",
		BHDM_VideoFmtParams[index].V_FrontPorchField0,
		BHDM_VideoFmtParams[index].V_FrontPorchField1)) ;
	BDBG_MSG(("Vert Back  Porch :        %3d          %3d",
		BHDM_VideoFmtParams[index].V_BackPorchField0, 
		BHDM_VideoFmtParams[index].V_BackPorchField1)) ;
	BDBG_MSG(("Vert Sync Pulse :         %3d          %3d",
		BHDM_VideoFmtParams[index].V_SyncPulseField0, 
		BHDM_VideoFmtParams[index].V_SyncPulseField1)) ;
	BDBG_MSG(("Vert Sync Pulse Offset :  %3d          %3d",
		BHDM_VideoFmtParams[index].V_SyncPulseOffsetField0,
		BHDM_VideoFmtParams[index].V_SyncPulseOffsetField1)) ;
#endif		


#if BHDM_CONFIG_SCHEDULER_REV_2
	/* always load the HORZA first; locks faster */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_HFP, BHDM_VideoFmtParams[index].H_FrontPorch * pixelRepetitionCount)
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_VPOL, BHDM_VideoFmtParams[index].V_Polarity)
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_HPOL, BHDM_VideoFmtParams[index].H_Polarity)
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_HAP, BHDM_VideoFmtParams[index].H_ActivePixels * pixelRepetitionCount) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HORZA, Register) ;

	/* Load HORZB Register */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_HORZB, MANUAL_HSP, BHDM_VideoFmtParams[index].H_SyncPulse * pixelRepetitionCount )
		| BCHP_FIELD_DATA(HDMI_HORZB, MANUAL_HBP, BHDM_VideoFmtParams[index].H_BackPorch * pixelRepetitionCount ) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HORZB, Register) ;

#else
	/* LEGACY */
	/* always load the HORZA first; locks faster */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_VPOL, BHDM_VideoFmtParams[index].V_Polarity)
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_HPOL, BHDM_VideoFmtParams[index].H_Polarity)
		| BCHP_FIELD_DATA(HDMI_HORZA, MANUAL_HAP, BHDM_VideoFmtParams[index].H_ActivePixels * pixelRepetitionCount) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HORZA, Register) ;

	/* Load HORZB Register */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_HORZB, MANUAL_HFP, BHDM_VideoFmtParams[index].H_FrontPorch * pixelRepetitionCount)
		| BCHP_FIELD_DATA(HDMI_HORZB, MANUAL_HSP, BHDM_VideoFmtParams[index].H_SyncPulse * pixelRepetitionCount )
		| BCHP_FIELD_DATA(HDMI_HORZB, MANUAL_HBP, BHDM_VideoFmtParams[index].H_BackPorch * pixelRepetitionCount ) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HORZB, Register) ;
#endif

	/* Load VERTA0 Register */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_VERTA0, MANUAL_VSP0, BHDM_VideoFmtParams[index].V_SyncPulseField0 )
		| BCHP_FIELD_DATA(HDMI_VERTA0, MANUAL_VFP0, BHDM_VideoFmtParams[index].V_FrontPorchField0 )
		| BCHP_FIELD_DATA(HDMI_VERTA0, MANUAL_VAL0, BHDM_VideoFmtParams[index].V_ActiveLinesField0 ) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_VERTA0, Register) ;


	/* Load VERTB0 Register */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_VERTB0, MANUAL_VSPO0, BHDM_VideoFmtParams[index].V_SyncPulseOffsetField0 )       
		| BCHP_FIELD_DATA(HDMI_VERTB0, MANUAL_VBP0, BHDM_VideoFmtParams[index].V_BackPorchField0 ) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_VERTB0, Register) ;


	/* Load VERTA1 Register */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_VERTA1, MANUAL_VSP1, BHDM_VideoFmtParams[index].V_SyncPulseField1 )
		| BCHP_FIELD_DATA(HDMI_VERTA1, MANUAL_VFP1, BHDM_VideoFmtParams[index].V_FrontPorchField1 )
		| BCHP_FIELD_DATA(HDMI_VERTA1, MANUAL_VAL1, BHDM_VideoFmtParams[index].V_ActiveLinesField1 ) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_VERTA1, Register) ;


	/* Load VERTB1 Register */
	Register = 
		(uint32_t) 0
		| BCHP_FIELD_DATA(HDMI_VERTB1, MANUAL_VSPO1, BHDM_VideoFmtParams[index].V_SyncPulseOffsetField1 ) 
		| BCHP_FIELD_DATA(HDMI_VERTB1, MANUAL_VBP1, BHDM_VideoFmtParams[index].V_BackPorchField1 ) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_VERTB1, Register) ;


done:
	BDBG_LEAVE(BHDM_P_ConfigureInputVideoFmt) ;
	return rc ;
}


/***************************************************************************
BERR_Code BHDM_GetEventHandle
Summary: Get the event handle for checking HDMI events.
****************************************************************************/
BERR_Code BHDM_GetEventHandle(
   BHDM_Handle hHDMI,           /* [in] HDMI handle */
   BHDM_EventType eEventType,
   BKNI_EventHandle *pBHDMEvent	/* [out] event handle */
)
{
	BERR_Code      rc = BERR_SUCCESS;

	BDBG_ENTER(BHDM_GetEventHandle) ;
	BDBG_ASSERT( hHDMI );		   

	switch (eEventType)
	{
	case BHDM_EventHDCP :
		*pBHDMEvent = hHDMI->BHDM_EventHDCP ;
		break ;

	case BHDM_EventHotPlug :
		*pBHDMEvent = hHDMI->BHDM_EventHotPlug ;
		break ;

	case BHDM_EventFIFO : /* debugging event */
		*pBHDMEvent = hHDMI->BHDM_EventFIFO ;
		break ;

	case BHDM_EventRAM :  /* debugging event */
		*pBHDMEvent = hHDMI->BHDM_EventRAM ;
		break ;

	case BHDM_EventHDCPRiValue :
		*pBHDMEvent = hHDMI->BHDM_EventHDCPRiValue ;
		break ;

	case BHDM_EventHDCPPjValue :
		*pBHDMEvent = hHDMI->BHDM_EventHDCPPjValue ;
		break ;

	case BHDM_EventHDCPRepeater:
		*pBHDMEvent = hHDMI->BHDM_EventHDCPRepeater;
		break;

	case BHDM_EventRxSense:
		*pBHDMEvent = hHDMI->BHDM_EventRxSense;
		break;			

	default :
		BDBG_ERR(("Invalid Event Type: %d", eEventType)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}


done:
	BDBG_LEAVE(BHDM_GetEventHandle) ;
	return rc ;
}


#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
static void BHDM_P_GetReceiverSense_isr(BHDM_Handle hHDMI, bool *RxSense)
{
	uint32_t Register;
	uint32_t PowerDownCtl ;
	uint8_t ReceiverSense ;
	uint8_t Ch0, Ch1, Ch2, Clock ;
	uint8_t RxSenseRegisterStatus ;


#if BHDM_CONFIG_65NM_SUPPORT
	PowerDownCtl = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL) ;
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_STATUS) ;
	Clock = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_STATUS, STATUS_RSEN_CK) ;
	Ch2   = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_STATUS, STATUS_RSEN_2) ;
	Ch1   = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_STATUS, STATUS_RSEN_1) ;
	Ch0   = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_STATUS, STATUS_RSEN_0) ;	
#else
	PowerDownCtl = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL) ;
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_STATUS) ;
#if BHDM_CONFIG_SWAP_DEFAULT_PHY_CHANNELS
	Ch2 = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_STATUS, RSEN_CK) ;
	Clock	= BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_STATUS, RSEN_2) ;
#else
	Clock = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_STATUS, RSEN_CK) ;
	Ch2   = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_STATUS, RSEN_2) ;
#endif
	Ch1   = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_STATUS, RSEN_1) ;
	Ch0   = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_STATUS, RSEN_0) ;
#endif	

	ReceiverSense = Clock + Ch2 + Ch1 + Ch0 ;

	RxSenseRegisterStatus =
		  Clock << 3
		| Ch2 << 2
		| Ch1 << 1
		| Ch0 ;

	/* If all ReceiverSense lines are all enabled */
	if (ReceiverSense == 4)
	{
		ReceiverSense = 0x0F; 
		*RxSense = true ;
	}
	/* If only the Clock is enabled, consider all TMDS lines enabled */
	else if (Clock) 
	{
		ReceiverSense = 0x08;
#if BHDM_CONFIG_DEBUG_RSEN
		BDBG_WRN(("Attached Rx Clock detected... ;  Report RxSense detected")) ;
#endif
		*RxSense = true ;
	}
	else
	{
		*RxSense = false ;
	}
		
	
#if BHDM_CONFIG_DEBUG_RSEN
	{
		static const uint8_t yes[] = "Y" ;
		static const uint8_t no[] = "N" ;
		
		BDBG_WRN(("Tx (PWRDN_CTL= 0x%x) Rx Power (RSEN= 0x%X) Report: %s -- Clock: %s ;   Ch2: %s ; Ch1: %s ; Ch0: %s", 
			PowerDownCtl, RxSenseRegisterStatus, 
			*RxSense ? yes : no,
			Clock? yes : no, 
			Ch2 ? yes : no, Ch1 ? yes : no, Ch0 ? yes : no)) ;
	}
#endif

	if (*RxSense != hHDMI->rxSensePowerDetected)
	{
		/* notify of changes only if clock or clock/data are enabled 
		     i.e. not in  standby mode or tmds lines turned off 
		*/
		if (hHDMI->OutputEnabled)
			BKNI_SetEvent(hHDMI->BHDM_EventHotPlug) ;

#if BHDM_CONFIG_DEBUG_RSEN
		BDBG_WRN(("Attached Rx Powered %s", *RxSense ? "ON" : "OFF")) ;
#endif
		hHDMI->rxSensePowerDetected = ReceiverSense;
	}
	return;
}
#endif



/******************************************************************************
void BHDM_P_Hotplug_isr
Summary: Process Hot Plug Interrupt
*******************************************************************************/
void BHDM_P_Hotplug_isr(BHDM_Handle hHDMI)
{
#if !BHDM_CONFIG_DUAL_HPD_SUPPORT
	uint32_t Register ;

	uint8_t RxDeviceAttached ;  

	BHDM_P_RxDeviceAttached_isr(hHDMI, &RxDeviceAttached) ;

	/* if same state return without setting an event */
	if (RxDeviceAttached == hHDMI->RxDeviceAttached)
	{
		BDBG_MSG(("Skip Multiple Hotplug Interrupts: %s", 
			RxDeviceAttached ? "CONNECTED" : "REMOVED")) ;
		return ;
	}
	hHDMI->RxDeviceAttached = RxDeviceAttached;
		
	if (!hHDMI->RxDeviceAttached)
	{
		BDBG_WRN(("HotPlug: DEVICE REMOVED!!")) ;
		/* reset the HDMI core to DVI Mode whenever disconnected */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL) ;
		Register &= ~(
			  BCHP_MASK(HDMI_SCHEDULER_CONTROL, HDMI_MODE_REQUEST)
			| BCHP_MASK(HDMI_SCHEDULER_CONTROL, ALWAYS_VERT_KEEP_OUT)) ;
		Register |=  BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, reserved0, 0)  ;
		
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL, (uint32_t) Register) ;
		hHDMI->DeviceSettings.eOutputFormat = BHDM_OutputFormat_eDVIMode ;
		
		/* always disable AvMute after a hot plug */
		hHDMI->AvMuteState = false ;
		hHDMI->rxSensePowerDetected = false ;
	}
	else
	{
#if BDBG_DEBUG_BUILD		
		BDBG_WRN(("HotPlug: DEVICE CONNECTED")) ;
#endif
	}
	
	hHDMI->edidStatus = BHDM_EDID_STATE_eInitialize;	/* Set Initialize EDID read flag */
	
	/* abort any pending HDCP requests */
	hHDMI->AbortHdcpAuthRequest = 1 ;
	hHDMI->HdcpOptions.numPjFailures = 0;

	/* Set CLEAR_RDB_AUTHENTICATED BIT only - all other bits must be zero */
	Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_CLEAR_RDB_AUTHENTICATED, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HDCP_CTL, Register) ;		

	if (hHDMI->pfHotplugChangeCallback)
	{
		hHDMI->pfHotplugChangeCallback(hHDMI->pvHotplugChangeParm1, 
			hHDMI->iHotplugChangeParm2, &hHDMI->RxDeviceAttached) ;
	}

	BKNI_SetEvent(hHDMI->BHDM_EventHotPlug) ;
#else
	BSTD_UNUSED(hHDMI) ;
#endif
}


/******************************************************************************
void BHDM_P_HandleInterrupt_isr
Summary: Handle interrupts from the HDMI core.
*******************************************************************************/
void BHDM_P_HandleInterrupt_isr(
	void *pParam1,						/* [in] Device handle */
	int parm2							/* [in] not used */
)
{
	BHDM_Handle hHDMI ;

	uint8_t MASK_INTERRUPTS = 1 ;  /* debug tool */
	uint32_t rc ;

	hHDMI = (BHDM_Handle) pParam1 ;
	BDBG_ASSERT(hHDMI) ;
	
	/* 
	-- Interrupts to be handled
	18 RSEN_UPDATE_INTR
	17 HDCP_RI_B_MISMATCH_INTR
	16 HDCP_RI_A_MISMATCH_INTR
	15 HDCP_PJ_MISMATCH_INTR
	
	14 MAI_FORMAT_UPDATE_INTR
	13 HDCP_PJ_INTR 
	11 HDCP_AN_READY_INTR 
	10 HDCP_RI_INTR 
	09 HDCP_V_MATCH_INTR 
	08 HDCP_V_MISMATCH_INTR 
	07 HDCP_REPEATER_ERR_INTR 

	06 CEC_INTR  ---> handled by CEC PI 

	05 PKT_WRITE_ERR_INTR 
	12 PACKET_FIFO_OVERFLOW_INTR  

	04 DRIFT_FIFO_ALMOST_EMPTY_INTR 
	03 DRIFT_FIFO_EMPTY_MINUS_INTR 
	02 DRIFT_FIFO_ALMOST_FULL_INTR 
	01 DRIFT_FIFO_FULL_MINUS_INTR 

	00 HOTPLUG_INTR 
	*/
	
	switch (parm2)
	{

#if BHDM_CONFIG_DUAL_HPD_SUPPORT

	case MAKE_INTR_ENUM(HOTPLUG_REMOVED) :					   /* 00 */ 
	{
		uint32_t Register ;
		
		BDBG_WRN(("HotPlug (Dual Intr) : DEVICE REMOVED!!")) ;
		hHDMI->RxDeviceAttached = 0;
		hHDMI->hotplugInterruptFired = true;
		
		/* reset the HDMI core to DVI Mode whenever disconnected */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL) ;
		Register &= ~(
			  BCHP_MASK(HDMI_SCHEDULER_CONTROL, HDMI_MODE_REQUEST)
			| BCHP_MASK(HDMI_SCHEDULER_CONTROL, ALWAYS_VERT_KEEP_OUT)) ;
		Register |=  BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, reserved0, 0)	;
		
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_SCHEDULER_CONTROL, (uint32_t) Register) ;
		hHDMI->DeviceSettings.eOutputFormat = BHDM_OutputFormat_eDVIMode ;

		/* always disable AvMute after a hot plug */
		hHDMI->AvMuteState = false ;

		/* abort any pending HDCP requests */
		hHDMI->AbortHdcpAuthRequest = 1 ;
		hHDMI->HdcpOptions.numPjFailures = 0;
		
		/* Set CLEAR_RDB_AUTHENTICATED BIT only - all other bits must be zero */
		Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_CLEAR_RDB_AUTHENTICATED, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HDCP_CTL, Register) ;		

		/* Fire hotplug callback */
		if (hHDMI->pfHotplugChangeCallback)
		{
			hHDMI->pfHotplugChangeCallback(hHDMI->pvHotplugChangeParm1, 
				hHDMI->iHotplugChangeParm2, &hHDMI->RxDeviceAttached) ;
		}
		
		BKNI_SetEvent(hHDMI->BHDM_EventHotPlug) ;
		break ;
	}

	case MAKE_INTR_ENUM(HOTPLUG_CONNECTED) :					   /* 01 */ 		

		BDBG_WRN(("HotPlug  (Dual Intr): DEVICE CONNECTED")) ;
		hHDMI->RxDeviceAttached = 1;
		hHDMI->hotplugInterruptFired = true;		
		hHDMI->edidStatus = BHDM_EDID_STATE_eInitialize;	/* Set Initialize EDID read flag */		

		/* Fire hotplug callback */
		if (hHDMI->pfHotplugChangeCallback)
		{
			hHDMI->pfHotplugChangeCallback(hHDMI->pvHotplugChangeParm1, 
				hHDMI->iHotplugChangeParm2, &hHDMI->RxDeviceAttached) ;
		}

		BKNI_SetEvent(hHDMI->BHDM_EventHotPlug) ;
		break ;


#else /* Single HPD interrupt */

	case MAKE_INTR_ENUM(HOTPLUG) :					   /* 00 */ 

#if BHDM_CONFIG_BTMR_SUPPORT
		if (BHDM_CONFIG_HOTPLUG_DELAY_MS)
		{
			/* wait before processing hot plug */
			BDBG_WRN(("Delay %d ms before Hotplug Callback", 
				BHDM_CONFIG_HOTPLUG_DELAY_MS)) ;

			BTMR_StartTimer_isr(hHDMI->TimerHotPlug, 
				BHDM_P_MILLISECOND * BHDM_CONFIG_HOTPLUG_DELAY_MS) ;

			break ;
		}
#endif
		
		BHDM_P_Hotplug_isr(hHDMI) ;
		break ;
#endif 



	case MAKE_INTR_ENUM(DF_FULL_MINUS) :			   /* 01 */ 
		BDBG_ERR(("Int0x%x FULL MINUS!", parm2));
		if (MASK_INTERRUPTS) 
		{
			BHDM_CHECK_RC( rc, BINT_DisableCallback_isr( hHDMI->hCallback[parm2] ) );
			BDBG_MSG(("Full Minus Int Masked")) ;
		}
		BKNI_SetEvent(hHDMI->BHDM_EventFIFO) ;
			
		break ;

	
	case MAKE_INTR_ENUM(DF_ALMOST_FULL) : 			   /* 02 */ 
		BDBG_ERR(("Int0x%x ALMOST FULL!", parm2));
		if (MASK_INTERRUPTS) 
		{
			BHDM_CHECK_RC( rc, BINT_DisableCallback_isr( hHDMI->hCallback[parm2] ) );
			BDBG_MSG(("Almost Full Int Masked")) ;
		}
		BKNI_SetEvent(hHDMI->BHDM_EventFIFO) ;
		break ;

	
	case MAKE_INTR_ENUM(DF_EMPTY_MINUS) :			   /* 03 */ 
		BDBG_ERR(("Int0x%x EMPTY MINUS!", parm2));
		if (MASK_INTERRUPTS)
		{
			BHDM_CHECK_RC( rc, BINT_DisableCallback_isr( hHDMI->hCallback[parm2] ) );
			BDBG_MSG(("Empty Minus Int Masked")) ;
		}
		BKNI_SetEvent(hHDMI->BHDM_EventFIFO) ;
		break ;

	
	case MAKE_INTR_ENUM(DF_ALMOST_EMPTY) : 		       /* 04 */ 
		BDBG_ERR(("Int0x%x ALMOST EMPTY!", parm2));
		if(MASK_INTERRUPTS)
		{
			BHDM_CHECK_RC( rc, BINT_DisableCallback_isr( hHDMI->hCallback[parm2] ) );
			BDBG_MSG(("Almost Empty Int Masked")) ;
		}
		BKNI_SetEvent(hHDMI->BHDM_EventFIFO) ;
		break ;


	case MAKE_INTR_ENUM(PKT_WRITE_ERR) :               /* 05 */ 
	case MAKE_INTR_ENUM(PKT_OVERFLOW) :			       /* 12 */ 
		BDBG_ERR(("Int0x%x!", parm2));
		BKNI_SetEvent(hHDMI->BHDM_EventRAM) ;
		break ;

	case MAKE_INTR_ENUM(HDCP_RI) :					   /* 10 */ 
#if 0	
		BDBG_MSG(("Int0x%x RiValue!", parm2)); 
#endif		
		if (hHDMI->HDCP_RiCount == 0) /* skip processing r0 value twice */
			break ;
		
		BKNI_SetEvent(hHDMI->BHDM_EventHDCPRiValue) ;
		break ;

	
	case MAKE_INTR_ENUM(HDCP_PJ) :					   /* 13 */ 
#if 0	
		BDBG_MSG(("Int0x%x PjValue!", parm2)); 
#endif		
		BKNI_SetEvent(hHDMI->BHDM_EventHDCPPjValue) ;
		break ;

	case MAKE_INTR_ENUM(MAI_FORMAT_UPDATE):
#if BHDM_CONFIG_DEBUG_MAI_CHANNEL_MAP		
		BDBG_MSG(("MAI FORMAT UPDATE Interrupt (%x%x)!", parm2));
#endif
		BHDM_P_HandleMaiFormatUpdate_isr(hHDMI);
		break;
		
#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	case MAKE_INTR_ENUM(HDCP_RI_A_MISMATCH):		/* 15 */
		BDBG_ERR(("Ri mismatch Interrupt (0x%x)!", parm2));
		hHDMI->HDCP_AutoRiMismatch_A = 1;
		BKNI_SetEvent(hHDMI->BHDM_EventHDCPRiValue);
		break;
		
	case MAKE_INTR_ENUM(HDCP_RI_B_MISMATCH):		/* 16 */
		BDBG_ERR(("Ri mismatch Interrupt (0x%x)!", parm2));
		hHDMI->HDCP_AutoRiMismatch_B = 1;
		BKNI_SetEvent(hHDMI->BHDM_EventHDCPRiValue);
		break;
		
	case MAKE_INTR_ENUM(HDCP_PJ_MISMATCH):			/* 14 */
		BDBG_ERR(("Pj mismatch Interrupt (0x%x)!", parm2));
		hHDMI->HDCP_AutoPjMismatch = 1;
		BKNI_SetEvent(hHDMI->BHDM_EventHDCPPjValue);
		break;
#endif
		
	case MAKE_INTR_ENUM(HDCP_REPEATER_ERR) :		   /* 07 */ 
	case MAKE_INTR_ENUM(HDCP_V_MISMATCH) :			   /* 08 */ 
		BDBG_ERR(("Repeater Interrupt (0x%x)!", parm2));
		BKNI_SetEvent(hHDMI->BHDM_EventHDCPRepeater);
		break;
		
	case MAKE_INTR_ENUM(HDCP_V_MATCH) :				   /* 09 */ 
		BDBG_WRN(("BHDM Interrupt ID=0x%x!", parm2));
		/* continue to setting event */
	case MAKE_INTR_ENUM(HDCP_AN) :					   /* 11 */ 
		BKNI_SetEvent(hHDMI->BHDM_EventHDCP) ;
		break ;


#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT && !BHDM_CONFIG_RSEN_POLLING
	case MAKE_INTR_ENUM(RSEN) :							/* 17 */
	{
		bool RxSense ;
		
#if BHDM_CONFIG_DEBUG_RSEN
		BDBG_WRN(("RxSense Interrupt!")) ;
#endif
		/* update the RxSense value */
		BHDM_P_GetReceiverSense_isr(hHDMI, &RxSense) ;

		if (RxSense == hHDMI->rxSensePowerDetected)
		{
#if BHDM_CONFIG_DEBUG_RSEN
			BDBG_WRN(("No change in RxSense Status of  %s", RxSense ? "On" : "Off")) ;
#endif
			return ;
		}
		
		hHDMI->rxSensePowerDetected = RxSense ;
			
		
#if BHDM_CONFIG_DEBUG_RSEN
		BDBG_WRN(("RxSense: %s", 
			hHDMI->rxSensePowerDetected ? "ON" : "OFF" )) ;
#endif


		if (hHDMI->pfRxSenseChangeCallback)
		{
			hHDMI->pfRxSenseChangeCallback(hHDMI->pvRxSenseChangeParm1, 
				hHDMI->iRxSenseChangeParm2, &hHDMI->rxSensePowerDetected) ;
		}
		
		BKNI_SetEvent(hHDMI->BHDM_EventRxSense);
		break;
	}
#endif


	default :
		BDBG_WRN(("BHDM Unknown Interrupt ID=0x%x !", parm2 ));
	}

done:
	rc = 0 ;
	/* L2 interrupts are reset automatically */
}



/*******************************************************************************
BERR_Code BHDM_GetHdmiSettings
Summary: Get the current settings for the HDMI device.
*******************************************************************************/
BERR_Code BHDM_GetHdmiSettings(BHDM_Handle hHDMI, /* [in] handle to HDMI device */
	BHDM_Settings *pHdmiSettings  /* [in] pointer to memory to hold the current
	                                  HDMI settings */
)
{
	BERR_Code      rc = BERR_SUCCESS;
	BDBG_ENTER(BHDM_GetHdmiSettings) ;

	BDBG_ASSERT(pHdmiSettings) ;
	BDBG_ASSERT(hHDMI) ;

	BKNI_Memset(pHdmiSettings, 0, sizeof(BHDM_Settings)) ;
	BKNI_Memcpy(pHdmiSettings, &hHDMI->DeviceSettings, sizeof(BHDM_Settings)) ;

	BDBG_LEAVE(BHDM_GetHdmiSettings) ;
	return rc ;
}


/*******************************************************************************
BERR_Code BHDM_SetHdmiSettings
Summary: Save the current settings for the HDMI device.

Note: the settings for HDMI device will be saved when calling BHDM_EnableDisplay
         This call should not be use if BHDM_EnableDisplay is used

See Also:
	BHDM_EnableDisplay()
*******************************************************************************/
BERR_Code BHDM_SetHdmiSettings(BHDM_Handle hHDMI, /* [in] handle to HDMI device */
	BHDM_Settings *pHdmiSettings  /* [in] pointer to memory to hold the current
									  HDMI settings */
)
{
	BERR_Code rc = BERR_SUCCESS;
	BDBG_ENTER(BHDM_SetHdmiSettings) ;

	BDBG_ASSERT(pHdmiSettings) ;
	BDBG_ASSERT(hHDMI) ;

	BKNI_Memcpy(&(hHDMI->DeviceSettings), pHdmiSettings, sizeof(BHDM_Settings));

	BDBG_LEAVE(BHDM_SetHdmiSettings) ;
	return rc ;
}


void BHDM_P_CaptureFIFOData(BHDM_Handle hHDMI, BHDM_P_FIFO_DATA *FifoData) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;

	hRegister = hHDMI->hRegister ;

	/* get the CAPTURE POINTER data for debug, fine tuning, etc. */	
	Register = BREG_Read32(hRegister, BCHP_HDMI_FIFO_CTL); 
		Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved0) ;
		Register &= ~BCHP_MASK(HDMI_FIFO_CTL, reserved1) ;
		Register &= ~BCHP_MASK(HDMI_FIFO_CTL, CAPTURE_POINTERS) ;
	
		Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, CAPTURE_POINTERS, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 
	
		Register &= ~BCHP_MASK(HDMI_FIFO_CTL, CAPTURE_POINTERS) ;
	BREG_Write32(hRegister, BCHP_HDMI_FIFO_CTL, Register); 
	

	/* check for over/underflow */
	Register = BREG_Read32(hRegister, BCHP_HDMI_READ_POINTERS) ;
	FifoData->bOverFlow = 
		BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_OVERFLOW) ;

	FifoData->bUnderFlow = 
		BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_UNDERFLOW) ;

	FifoData->uiRdAddr = 
		(uint8_t) BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_RD_ADDR_7_0) ;
	
	FifoData->uiWrAddr = 
		(uint8_t) BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_WR_ADDR_7_0) ;
	
	FifoData->uiFullness = 
		(uint16_t) ((FifoData->uiWrAddr - FifoData->uiRdAddr + 256) % 256) ;

#if BHDM_CONFIG_DEBUG_FIFO
	/* read the pointers */
	{
		uint8_t RdPtr, WrPtr ;

		bool 
			bOverflow, bUnderflow, 
			bAlmostFull, bFullMinus, 
			bAlmostEmpty, bEmptyMinus ;
		
		BDBG_WRN(("----------------- Begin Drift FIFO Debug Dump -----------------")) ;

		Register = BREG_Read32(hRegister, BCHP_HDMI_READ_POINTERS); 

			RdPtr = BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_RD_ADDR_7_0) ;
			WrPtr = BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_WR_ADDR_7_0) ;

			bAlmostFull = 
				BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_ALMOST_FULL) ;
			bFullMinus= 
				BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_FULL_MINUS) ;
			bOverflow = 
				BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_OVERFLOW) ;
			bAlmostEmpty = 
				BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_ALMOST_EMPTY) ;
			bEmptyMinus = 
				BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_EMPTY_MINUS) ;
			bUnderflow = 
				BCHP_GET_FIELD_DATA(Register, HDMI_READ_POINTERS, DRIFT_UNDERFLOW) ;


		BDBG_WRN(("Read/Write Pointer: %d / %d    FULLNESS: %d", 
			 RdPtr, WrPtr, FifoData->uiFullness)) ;

		if ( bUnderflow && bOverflow)
		{
			BDBG_WRN(("UNDERFLOW / OVERFLOW: %d / %d", 
				bUnderflow, bOverflow)) ;
		}
				
		if (bAlmostFull && bFullMinus)
		{
			BDBG_WRN(("ALMOST_FULL: %d  FULL_MINUS: %d", 
				bAlmostFull, bFullMinus)) ;
		}

		if (bAlmostEmpty && bEmptyMinus)
		{
			BDBG_WRN(("ALMOST_EMPTY: %d  EMPTY_MINUS %d",  
				bAlmostEmpty, bEmptyMinus)) ;
		}

		BDBG_WRN(("-----------------  End Drift FIFO Debug Dump ------------------")) ;
	}
#endif
}


BERR_Code BHDM_SetTimebase(
   BHDM_Handle hHDMI,          /* [in] HDMI handle */
   BAVC_Timebase eTimebase     /* [in] Timebase */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	
	hHDMI->DeviceSettings.eTimebase = eTimebase ;
	return rc ;	
}


BERR_Code BHDM_GetTimebase(
   BHDM_Handle hHDMI,          /* [in] HDMI handle */
   BAVC_Timebase *eTimebase     /* [out] Timebase */
)
{
	BERR_Code      rc = BERR_SUCCESS;
	
	*eTimebase = hHDMI->DeviceSettings.eTimebase  ;
	return rc ;	
}


void BHDM_P_VideoFmt2CEA861Code(BFMT_VideoFmt eVideoFmt, 
	BFMT_AspectRatio eAspectRatio, BAVC_HDMI_PixelRepetition ePixelRepetition, uint8_t *VideoID)
{
	switch (eVideoFmt)
	{
	case BFMT_VideoFmt_e1080i  :           /* HD 1080i */
		*VideoID = 5 ;
		break ;
		
	case BFMT_VideoFmt_e720p   :           /* HD 720p */
	case BFMT_VideoFmt_e720p_60Hz_3DOU_AS:	/* 720p 60Hz 3D frame packing */
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	case BFMT_VideoFmt_e720p_3D :          /* HD 720p 3D */
#endif
		*VideoID = 4 ;
		break ;
		
	case BFMT_VideoFmt_e480p   :           /* HD 480p */
		if (eAspectRatio == BFMT_AspectRatio_e16_9)
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 3 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 15 ;
				break;
			case BAVC_HDMI_PixelRepetition_e4x:
				*VideoID = 36 ;
				break;
			default:
				break;
			}
		}
		else   /* default 4:3 */
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 2 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 14 ;
				break;
			case BAVC_HDMI_PixelRepetition_e4x:
				*VideoID = 35 ;
				break;
			default:
				break;
			}
		}
		break ;
	
	case BFMT_VideoFmt_eNTSC   :		   /* 480i, NSTC-M for North America */
	case BFMT_VideoFmt_eNTSC_J :		   /* 480i (Japan) */
	case BFMT_VideoFmt_ePAL_M  :		   /* 525-lines (Brazil) */
		if (eAspectRatio == BFMT_AspectRatio_e16_9)
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 7 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 11 ;
				break;
			default:
				break;
			}
		}
		else   /* default 4:3 */
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 6 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 10 ;
				break;
			default:
				break;
			}
		}
		break ;
	
	case BFMT_VideoFmt_ePAL_B  :		   /* Australia */
	case BFMT_VideoFmt_ePAL_B1 :		   /* Hungary */
	case BFMT_VideoFmt_ePAL_D  :		   /* China */
	case BFMT_VideoFmt_ePAL_D1 :		   /* Poland */
	case BFMT_VideoFmt_ePAL_G  :		   /* Europe */
	case BFMT_VideoFmt_ePAL_H  :		   /* Europe */
	case BFMT_VideoFmt_ePAL_K  :		   /* Europe */
	case BFMT_VideoFmt_ePAL_I  :		   /* U.K. */
	case BFMT_VideoFmt_ePAL_N  :		   /* Jamaica, Uruguay */
	case BFMT_VideoFmt_ePAL_NC :		   /* N combination (Argentina) */
	case BFMT_VideoFmt_eSECAM  :		   /* LDK/SECAM (France,Russia) */
		if (eAspectRatio == BFMT_AspectRatio_e16_9)
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 22 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 26 ;
				break;
			default:
				break;
			}
		}
		else   /* default 4:3 */
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 21 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 25 ;
				break;
			default:
				break;
			}
		}
		break ;
	
	case BFMT_VideoFmt_e1250i_50Hz :	   /* HD 1250i 50Hz, another 1080i_50hz standard SMPTE 295M */
		BDBG_WRN(("Verify AVI Frame Video Code for 1250i Format")) ;
		/* fall through to use 1080i 50Hz Video Code */
	case BFMT_VideoFmt_e1080i_50Hz :	   /* HD 1080i 50Hz, 1125 line, SMPTE 274M */
		*VideoID = 20 ;
		break ;
	
	case BFMT_VideoFmt_e720p_50Hz  :	   /* HD 720p 50Hz (Australia) */
	case BFMT_VideoFmt_e720p_50Hz_3DOU_AS: /* 720p 50Hz 3D Frame Packing */
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	case BFMT_VideoFmt_e720p_50Hz_3D :	   /* HD 720p 50Hz 3D */
#endif
		*VideoID  = 19 ;
		break ;

	case BFMT_VideoFmt_e720p_24Hz:			/* 720p 24Hz */
	case BFMT_VideoFmt_e720p_24Hz_3DOU_AS:	/* 720p 24Hz 3D frame packing */
		*VideoID  = 60 ;
		break ;

	case BFMT_VideoFmt_e720p_25Hz:			/* 720p 25Hz */
		*VideoID  = 61 ;
		break ;

	case BFMT_VideoFmt_e720p_30Hz:			/* 720p 30Hz */
	case BFMT_VideoFmt_e720p_30Hz_3DOU_AS:	/* 720p 30Hz 3D frame packing */
		*VideoID  = 62 ;
		break ;
	
	case BFMT_VideoFmt_e576p_50Hz  :	   /* HD 576p 50Hz (Australia) */
		if (eAspectRatio  == BFMT_AspectRatio_e16_9)
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 18 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 30 ;
				break;
			case BAVC_HDMI_PixelRepetition_e4x:
				*VideoID = 38 ;
				break;
			default:
				break;
			}
		}
		else   /* default 4:3 */
		{
			switch (ePixelRepetition)
			{
			case BAVC_HDMI_PixelRepetition_eNone:
				*VideoID = 17 ;
				break;
			case BAVC_HDMI_PixelRepetition_e2x:
				*VideoID = 29 ;
				break;
			case BAVC_HDMI_PixelRepetition_e4x:
				*VideoID = 37 ;
				break;
			default:
				break;
			}
		}
		break ;
	
	case BFMT_VideoFmt_eDVI_640x480p :	   /* DVI Safe mode for computer monitors */
		*VideoID = 1 ;
		break ;
		
	case BFMT_VideoFmt_e1080p	:	  /* HD 1080p 60Hz */
	case BFMT_VideoFmt_e1080p_60Hz_3DOU_AS:
	case BFMT_VideoFmt_e1080p_60Hz_3DLR:
		*VideoID = 16 ;
		break ;
		
	case BFMT_VideoFmt_e1080p_50Hz	:	  /* HD 1080p 50Hz */
		*VideoID = 31 ;
		break ;
		
	case BFMT_VideoFmt_e1080p_30Hz	:	  /* HD 1080p 30Hz */
	case BFMT_VideoFmt_e1080p_30Hz_3DOU_AS:
		*VideoID = 34 ;
		break ;
		
	case BFMT_VideoFmt_e1080p_24Hz	:	  /* HD 1080p 24Hz */
	case BFMT_VideoFmt_e1080p_24Hz_3DOU_AS:
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	case BFMT_VideoFmt_e1080p_24Hz_3D :	  /* HD 1080p 24Hz 3D */
#endif
		*VideoID = 32 ;
		break ;
		
	case BFMT_VideoFmt_e1080p_25Hz	:	  /* HD 1080p 25Hz */
		*VideoID = 33 ;
		break ;

	case BFMT_VideoFmt_eDVI_800x600p:
	case BFMT_VideoFmt_eDVI_1024x768p:
	case BFMT_VideoFmt_eDVI_1280x768p:
	case BFMT_VideoFmt_eDVI_1280x720p_50Hz:
	case BFMT_VideoFmt_eDVI_1280x720p:
		*VideoID = 0;
		BDBG_MSG(("No Video ID Code available for DVI formats"));
		break;

	default :
		*VideoID = 0 ;
		BDBG_ERR(("BFMT_VideoFmt %d NOT IMPLEMENTED",  eVideoFmt)) ;
		break ;
	}
}



/******************************************************************************
Summary:
Set the pixel repetition setting 
*******************************************************************************/
BERR_Code BHDM_SetPixelRepetition(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_PixelRepetition ePixelRepetition
)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BHDM_SetPixelRepetition) ;
	BDBG_ASSERT(hHDMI);

	switch (ePixelRepetition)
	{
	case BAVC_HDMI_PixelRepetition_eNone:
	case BAVC_HDMI_PixelRepetition_e2x:
	case BAVC_HDMI_PixelRepetition_e3x:
	case BAVC_HDMI_PixelRepetition_e4x:
	case BAVC_HDMI_PixelRepetition_e5x:
	case BAVC_HDMI_PixelRepetition_e6x:
	case BAVC_HDMI_PixelRepetition_e7x:
	case BAVC_HDMI_PixelRepetition_e8x:
	case BAVC_HDMI_PixelRepetition_e9x:
	case BAVC_HDMI_PixelRepetition_e10x:
		hHDMI->DeviceSettings.ePixelRepetition = ePixelRepetition;
		break;

	default:
		BDBG_ERR(("Invalid Pixel Repetition"));
		rc = BERR_INVALID_PARAMETER;
		break;
	}
	
	BDBG_LEAVE(BHDM_SetPixelRepetition) ;	
	return rc;

}


/******************************************************************************
Summary:
Get the pixel repetition setting 
*******************************************************************************/
BERR_Code BHDM_GetPixelRepetition(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_PixelRepetition *ePixelRepetition
)
{
	BDBG_ENTER(BHDM_GetPixelRepetition) ;
	BDBG_ASSERT(hHDMI);
	
	if (ePixelRepetition)
	{
		*ePixelRepetition = hHDMI->DeviceSettings.ePixelRepetition;
	}

	BDBG_LEAVE(BHDM_GetPixelRepetition) ;
	return BERR_SUCCESS;
}


BERR_Code BHDM_ConfigurePreEmphasis(
	BHDM_Handle hHDMI, BHDM_PreEmphasisSetting eValue)
{
	BERR_Code      rc = BERR_SUCCESS ;
	uint8_t uDriverAmp ;
	
	switch (eValue)
	{
	case BHDM_PreEmphasis_eOFF :
		uDriverAmp = hHDMI->DeviceSettings.uiDriverAmpDefault  ;
		break ;
		
	case BHDM_PreEmphasis_eLOW :
	case BHDM_PreEmphasis_eMED :
	case BHDM_PreEmphasis_eMAX :
		uDriverAmp = 0xF ;
		break ;
		
	default :
		rc = BERR_INVALID_PARAMETER ;
		BDBG_ERR(("Invalid PreEmphasis Value specified ")) ;
		goto done ;
	}

	BHDM_P_SetPreEmphasisMode (hHDMI, eValue, uDriverAmp);
	
done:
	return rc ;
}


#if BHDM_CONFIG_PRE_EMPHASIS_SUPPORT
/******************************************************************************
Summary:
	Get current pre-emphasis settings
*******************************************************************************/
BERR_Code BHDM_GetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
)
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDM_GetPreEmphasisConfiguration);

	BHDM_P_GetPreEmphasisConfiguration (hHDMI, stPreEmphasisConfig);

	BDBG_LEAVE(BHDM_GetPreEmphasisConfiguration);
	return rc;
}


/******************************************************************************
Summary:
	Set pre-emphasis settings with the provided information
*******************************************************************************/
BERR_Code BHDM_SetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
)
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDM_SetPreEmphasisConfiguration);
	
	/* Set Preemphasis configurations */
	BHDM_P_SetPreEmphasisConfiguration(hHDMI, stPreEmphasisConfig);


	BDBG_LEAVE(BHDM_SetPreEmphasisConfiguration);
	return rc;
}
#endif


/******************************************************************************
Summary:
	Set data transferring mode (master or slave) for HDMI
*******************************************************************************/
BERR_Code BHDM_SetHdmiDataTransferMode(
	BHDM_Handle hHDMI,
	bool masterMode
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;
	
	BDBG_ENTER(BHDM_SetHdmiDataTransferMode);

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_FIFO_CTL) ;
	Register &= ~( BCHP_MASK(HDMI_FIFO_CTL, MASTER_OR_SLAVE_N)
				| BCHP_MASK(HDMI_FIFO_CTL, USE_FULL));
		
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, MASTER_OR_SLAVE_N, masterMode?1:0)		
				| BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, masterMode?0:1);			 
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;

	BDBG_LEAVE(BHDM_SetHdmiDataTransferMode);
	return rc;
}


/******************************************************************************
Summary:
	Get the current data transferring mode (master or slave) for HDMI
*******************************************************************************/
BERR_Code BHDM_GetHdmiDataTransferMode(
	BHDM_Handle hHDMI,
	bool *masterMode
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;
	
	BDBG_ENTER(BHDM_GetHdmiDataTransferMode);

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_FIFO_CTL) ;
	*masterMode = (bool) BCHP_GET_FIELD_DATA(Register, HDMI_FIFO_CTL, MASTER_OR_SLAVE_N);		

	BDBG_LEAVE(BHDM_GetHdmiDataTransferMode);
	return rc;
}


/******************************************************************************
Summary:
	Get power status of attached receiver 
*******************************************************************************/
#define BHDM_RECEIVER_SENSE_ON 0x0F
#define BHDM_RECEIVER_SENSE_OFF 0x00
BERR_Code BHDM_GetReceiverSense(
	BHDM_Handle hHDMI, uint8_t *ReceiverSense)
{
	BERR_Code rc;
	uint8_t DeviceAttached;
	bool RxSense = false ;

	
	/* check if a Receiver is Attached */
	rc = BHDM_RxDeviceAttached(hHDMI, &DeviceAttached);

	if (!DeviceAttached)
	{
#if BHDM_CONFIG_DEBUG_RSEN
		BDBG_WRN(("Detect RxSense: No DVI/HDMI Device Attached")) ;
#endif
		*ReceiverSense = false ;
		hHDMI->rxSensePowerDetected	= false ;
		return BHDM_NO_RX_DEVICE ;
	}

	#if defined(HUMAX_PLATFORM_BASE)
	/*remove condition because of  confiliction with  P_HDMI_EnableHdmiMode */
	if(0)
	#else
	if (!hHDMI->OutputEnabled)
	#endif
	{
#if BHDM_CONFIG_DEBUG_RSEN
		BDBG_WRN(("Detect RxSense: Required HDMI (TMDS) output is disabled")) ;
#endif
		*ReceiverSense = false ;
		hHDMI->rxSensePowerDetected	= false ;
		return BERR_SUCCESS ;
	}

#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
	
	/* interrupt should always update the latest updated RSEN value */
	BKNI_EnterCriticalSection() ;
		BHDM_P_GetReceiverSense_isr(hHDMI, &RxSense) ;
		*ReceiverSense = hHDMI->rxSensePowerDetected ? 
			BHDM_RECEIVER_SENSE_ON 	: BHDM_RECEIVER_SENSE_OFF ;
	BKNI_LeaveCriticalSection() ;
#endif

	return BERR_SUCCESS ;
}


BERR_Code BHDM_GetColorimetry(BHDM_OutputFormat eOutputFormat, 
	BFMT_VideoFmt eVideoFmt, BAVC_MatrixCoefficients *eColorimetry) 
{
	BERR_Code 	rc = BERR_SUCCESS ;

	BDBG_WRN(("Use BHDM_EDID_GetSupportedColorimetry() instead; returns recommended and supported colorimetry")) ;
	
	
	/* default to Limited Range RGB */	
	*eColorimetry = BAVC_MatrixCoefficients_eHdmi_RGB ;
	
	/* use YPrPb for HDMI monitors */
	if (eOutputFormat == BHDM_OutputFormat_eHDMIMode)
	{
		switch (eVideoFmt)
		{
		case BFMT_VideoFmt_eDVI_640x480p  :  /* except for VGA mode */
			BDBG_MSG(("Use Full Range RGB")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eDvi_Full_Range_RGB ;
			break ;
		
		case BFMT_VideoFmt_eNTSC :
		case BFMT_VideoFmt_e480p :
		case BFMT_VideoFmt_eNTSC_J :
			
#if BHDM_CONFIG_ANALOG_TRANSLATION_SUPPORT
		case BFMT_VideoFmt_e720x482_NTSC :         /* 720x482i NSTC-M for North America */
		case BFMT_VideoFmt_e720x483p :
		case BFMT_VideoFmt_e720x482_NTSC_J :	   /* 720x482i Japan */
#endif
			BDBG_MSG(("Use YCrCb SD")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eSmpte_170M ;   /* SD Colorspace */
			break ;
		
		default : /* all others */
			BDBG_MSG(("Use YCrCb HD")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eItu_R_BT_709 ; /* HD Colorspace */
		}
	}
#if BDBG_DEBUG_BUILD			
	else
		BDBG_MSG(("DVI Monitors/Mode support RGB Only")) ;
#endif
	
	
	/* use full range rgb for DVI and HDMI 640x480 */	
	if (*eColorimetry == BAVC_MatrixCoefficients_eHdmi_RGB)
	{
		switch (eVideoFmt)
		{
		default :
			BDBG_MSG(("Use Limited Range RGB")) ;
			/* keep limited range RGB */
			break ;
			
		case BFMT_VideoFmt_eDVI_640x480p  :
		case BFMT_VideoFmt_eDVI_800x600p  :
		case BFMT_VideoFmt_eDVI_1024x768p :
		case BFMT_VideoFmt_eDVI_1280x768p :
		case BFMT_VideoFmt_eDVI_1280x720p_50Hz :
		case BFMT_VideoFmt_eDVI_1280x720p :
		case BFMT_VideoFmt_eDVI_1280x720p_ReducedBlank :
			BDBG_MSG(("Use Full Range RGB")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eDvi_Full_Range_RGB ;
			break ;
		}
	}
	
	return rc  ;
}


/******************************************************************************
Summary: install Hot Plug Change Callback to notify of HP detect changes 
*******************************************************************************/
BERR_Code BHDM_InstallHotplugChangeCallback(
	BHDM_Handle hHDMI,			/* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr, /* [in] cb for hot plug changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDM_InstallHotplugChangeCallback) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}	

	BKNI_EnterCriticalSection() ;
		hHDMI->pfHotplugChangeCallback = pfCallback_isr ;
		hHDMI->pvHotplugChangeParm1 = pvParm1 ;
		hHDMI->iHotplugChangeParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDM_InstallHotplugChangeCallback);
	
	return rc ;
}


/******************************************************************************
Summary: Uninstall HotPlug Change Callback
*******************************************************************************/
BERR_Code BHDM_UnInstallHotplugChangeCallback( 
	BHDM_Handle hHDMI,						 /* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr) /* [in] cb for format changes */
{
	BERR_Code rc = BERR_SUCCESS ;	
	BSTD_UNUSED(pfCallback_isr) ;

	BDBG_LEAVE(BHDM_UnInstallHotplugChangeCallback) ;
	
	BKNI_EnterCriticalSection() ;
		hHDMI->pfHotplugChangeCallback = (BHDM_CallbackFunc) NULL ; 
	BKNI_LeaveCriticalSection();
	
	BDBG_LEAVE(BHDM_UnInstallHotplugChangeCallback) ;
	return rc;
}



/******************************************************************************
Summary: install Receiver Sense Change Callback to notify of RxSense power detect changes 
*******************************************************************************/
BERR_Code BHDM_InstallRxSenseChangeCallback(
	BHDM_Handle hHDMI,			/* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr, /* [in] cb for Receiver Sense changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDM_InstallRxSenseChangeCallback) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}	

	BKNI_EnterCriticalSection() ;
		hHDMI->pfRxSenseChangeCallback = pfCallback_isr ;
		hHDMI->pvRxSenseChangeParm1 = pvParm1 ;
		hHDMI->iRxSenseChangeParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDM_InstallRxSenseChangeCallback);
	
	return rc ;
#else
	BSTD_UNUSED( hHDMI) ;
	BSTD_UNUSED(pfCallback_isr) ;
	BSTD_UNUSED(pvParm1) ;
	BSTD_UNUSED(iParm2)  ;

	return BERR_SUCCESS ;

#endif
}


/******************************************************************************
Summary: Uninstall RxSense Change Callback
*******************************************************************************/
BERR_Code BHDM_UnInstallRxSenseChangeCallback( 
	BHDM_Handle hHDMI,						 /* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr) /* [in] cb for format changes */
{
#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
	BERR_Code rc = BERR_SUCCESS ;	
	BSTD_UNUSED(pfCallback_isr) ;

	BDBG_LEAVE(BHDM_UnInstallRxSenseChangeCallback) ;
	
	BKNI_EnterCriticalSection() ;
		hHDMI->pfRxSenseChangeCallback = (BHDM_CallbackFunc) NULL ; 
	BKNI_LeaveCriticalSection();
	
	BDBG_LEAVE(BHDM_UnInstallRxSenseChangeCallback) ;
	return rc;
#else
	BSTD_UNUSED( hHDMI) ;
	BSTD_UNUSED(pfCallback_isr) ;
	return BERR_SUCCESS ;
#endif
}



void BHDM_GetDefaultStandbySettings(
	BHDM_StandbySettings *pSettings
	)
{
	pSettings->bEnableWakeup = true;
}

/******************************************************************************
Summary: Enter standby mode
*******************************************************************************/
BERR_Code BHDM_Standby(
	BHDM_Handle hHDMI, /* [in] HDMI Handle */
	const BHDM_StandbySettings *pSettings
	)
{
	BHDM_StandbySettings settings;
	BERR_Code rc ;

	if (hHDMI->standby) {
		BDBG_ERR(("Already in standby"));
		return BERR_UNKNOWN;
	}
    
	if (pSettings==NULL) {
		BHDM_GetDefaultStandbySettings(&settings);
	}
	else {
		settings = *pSettings;
	}

	BHDM_P_DisableInterrupts(hHDMI) ;	

#if BHDM_CONFIG_BTMR_SUPPORT
	BHDM_P_DestroyTimer(hHDMI, hHDMI->TimerHotPlug, BHDM_P_TIMER_eHotPlug) ;
	BHDM_P_DestroyTimer(hHDMI, hHDMI->TimerFormatChange, BHDM_P_TIMER_eFormatDetection) ;
#endif

	if (!settings.bEnableWakeup) {
		/* if wakeup from CEC is not needed, then it doesn't need to be powered */
#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		BCHP_PWR_ReleaseResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif
	}

	hHDMI->enableWakeup = settings.bEnableWakeup;
	hHDMI->standby = true;
	rc = BHDM_DisableDisplay(hHDMI); /* this will release the HDMI_TX_TMDS resource */

	/* Power off PHY */
	BHDM_P_PowerOffPhy (hHDMI) ;

	

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CLK
	BCHP_PWR_ReleaseResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CLK) ;
#endif

	return rc ;
	
}


/******************************************************************************
Summary: Resume standby mode
*******************************************************************************/
BERR_Code BHDM_Resume(
	BHDM_Handle hHDMI /* [in] HDMI Handle */
	)
{
	BERR_Code rc ;
	uint8_t RxDeviceAttached ; 
	
	if (!hHDMI->standby) 
	{
		BDBG_ERR(("Not in standby"));
		return BERR_UNKNOWN;        
	}

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CLK
		BCHP_PWR_AcquireResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CLK);
#endif

	if (!hHDMI->enableWakeup) {
#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		BCHP_PWR_AcquireResource(hHDMI->hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif
	}

	/* Power on PHY */
	BHDM_P_PowerOnPhy (hHDMI) ;


#if BHDM_CONFIG_BTMR_SUPPORT
	/* re-create a hot plug countdown timer */
	BHDM_P_CreateTimer(hHDMI, 
		&hHDMI->TimerHotPlug, BHDM_P_TIMER_eHotPlug) ;

	/* re-create a format change detection timer */
	BHDM_P_CreateTimer(hHDMI, 
		&hHDMI->TimerFormatChange, BHDM_P_TIMER_eFormatDetection) ;
#endif
	

	hHDMI->standby = false;

	BKNI_EnterCriticalSection() ;
		BHDM_P_RxDeviceAttached_isr(hHDMI, &RxDeviceAttached) ;
	BKNI_LeaveCriticalSection() ;
	
	if ( RxDeviceAttached && (hHDMI->RxDeviceAttached != RxDeviceAttached) )
	{
		BKNI_SetEvent(hHDMI->BHDM_EventHotPlug);
		hHDMI->RxDeviceAttached = RxDeviceAttached;
		return BERR_SUCCESS;
	}
	
	BHDM_P_EnableInterrupts(hHDMI) ;	
	rc = BHDM_EnableDisplay(hHDMI, &hHDMI->DeviceSettings); /* this will acquire the HDMI_TX_TMDS resource */
	if (rc == BHDM_NO_RX_DEVICE)
	{
		/* no Rx device connected when resuming.. this is not an error */
		BDBG_MSG(("After Resume, if no device is connected, report success")) ;
		rc = BERR_SUCCESS ;
	}
	
	/* always redo HDMI output reconfiguration when enabling display */
	hHDMI->DeviceSettings.bForceEnableDisplay = true ;

	return rc ;
}


BERR_Code BHDM_GetCrcValue_isr(
	BHDM_Handle hHDMI,	/* [in] HDMI Handle */
	BHDM_CrcData *stCrcData
)
{
#if BHDM_CONFIG_CRC_SUPPORT
	uint32_t Register;
	
	Register = BREG_Read32(hHDMI->hRegister, REGADDR_CRC_CHECK_STATUS_0) ;
	stCrcData->crc = BCHP_GET_FIELD_DATA(Register, REGNAME_CRC_CHECK_STATUS_0, CRC_VALUE);
	
#else
	BSTD_UNUSED(hHDMI);
	BSTD_UNUSED(stCrcData);
#endif

	return BERR_SUCCESS;
}

static void BHDM_P_HandleMaiFormatUpdate_isr(BHDM_Handle hHDMI)
{
	uint32_t Register;
	BAVC_AudioSamplingRate eSampleRate;
	uint8_t 
		MaiChannel0, MaiChannel1,
		MaiChannel2, MaiChannel3,
		MaiChannel4, MaiChannel5,
		MaiChannel6, MaiChannel7 ;

	/**** Default Set Channel Map 	*/
	/* set CHANNEL_0_MAP = 0  - reset value */
	/* set CHANNEL_1_MAP = 1  - reset value */
	/* set CHANNEL_2_MAP = 2  - reset value */
	/* set CHANNEL_3_MAP = 3  - reset value */
	/* set CHANNEL_4_MAP = 4  - reset value */
	/* set CHANNEL_5_MAP = 5  - reset value */
	/* set CHANNEL_6_MAP = 6  - reset value */
	/* set CHANNEL_7_MAP = 7  - reset value */


	Register = BREG_Read32_isr(hHDMI->hRegister, BCHP_HDMI_MAI_FORMAT) ;
	switch ((Register >> 16) & 0xFF)
	{
	case 0xc8:
		/* High Bit Rate (HBR) Audio */
		MaiChannel0 = 0 ; 
		MaiChannel1 = 1 ;
		MaiChannel2 = 2 ; 
		MaiChannel3 = 3 ;
		MaiChannel4 = 4 ; 
		MaiChannel5 = 5 ;
		MaiChannel6 = 6 ; 
		MaiChannel7 = 7 ;
		break ;		
    
	default:
		/* N Channel PCM or 2ch Compressed 
		   requires a swap to convert from FMM ordering to HDMI ordering */

		MaiChannel0 = 0 ; 
		MaiChannel1 = 1 ;
		MaiChannel2 = 4 ; 
		MaiChannel3 = 5 ;
		MaiChannel4 = 3 ; 
		MaiChannel5 = 2 ;
		MaiChannel6 = 6 ; 
		MaiChannel7 = 7 ;
		break;
	}
		

	Register = 0 ;
		Register |= 
		  BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_0_MAP, MaiChannel0) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_1_MAP, MaiChannel1) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_2_MAP, MaiChannel2) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_3_MAP, MaiChannel3) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_4_MAP, MaiChannel4) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_5_MAP, MaiChannel5) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_6_MAP, MaiChannel6) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_7_MAP, MaiChannel7) ;

	BREG_Write32_isr(hHDMI->hRegister, BCHP_HDMI_MAI_CHANNEL_MAP, Register) ;

#if BHDM_CONFIG_DEBUG_MAI_CHANNEL_MAP
	BDBG_WRN(("MAI Channel  0   1   2   3   4   5   6   7")) ;
	BDBG_WRN((" Mapping   %3d %3d %3d %3d %3d %3d %3d %3d", 
		MaiChannel0, 	MaiChannel1, MaiChannel2, MaiChannel3,
		MaiChannel4, MaiChannel5, MaiChannel6, MaiChannel7)) ;
	BDBG_WRN(("Updating MAI Channel Map to %#x", Register));
#endif

	/* Parse Sample Rate from MAI_FORMAT */
	Register = BREG_Read32_isr(hHDMI->hRegister, BCHP_HDMI_MAI_FORMAT) ;
	switch ( (Register >> 8) & 0xFF )
	{
	default:
	case 0x00:
		/* Not Indicated or Invalid */
		eSampleRate = BAVC_AudioSamplingRate_eUnknown;
		break;

	case 0x07:
		eSampleRate = BAVC_AudioSamplingRate_e32k;
		break;

	case 0x08:
		eSampleRate = BAVC_AudioSamplingRate_e44_1k;
		break;

	case 0x09:
		eSampleRate = BAVC_AudioSamplingRate_e48k;
		break;

	case 0xC:
		eSampleRate = BAVC_AudioSamplingRate_e96k;
		break;

	case 0xD:
		eSampleRate = BAVC_AudioSamplingRate_e128k;
		break;

	case 0xE:
		eSampleRate = BAVC_AudioSamplingRate_e176_4k;
		break;

	case 0xF:
		eSampleRate = BAVC_AudioSamplingRate_e192k;
		break;
	}

	/* Update Audio Rate */
	if ( eSampleRate != BAVC_AudioSamplingRate_eUnknown )
	{
		BAVC_Audio_Info AudioInfo;

		AudioInfo.eAudioSamplingRate = eSampleRate;
		BHDM_AudioVideoRateChangeCB_isr(hHDMI, BHDM_Callback_Type_eAudioChange, &AudioInfo);
	}
}


#if BHDM_CONFIG_BTMR_SUPPORT
static BERR_Code BHDM_P_CreateTimer(BHDM_Handle hHDMI, BTMR_TimerHandle * timerHandle, uint8_t timerId) 
{
	BERR_Code rc ;
	BTMR_Settings timerSettings  ;

	BDBG_ENTER(BHDM_P_CreateTimer) ;
	
	BDBG_MSG(("Create BHDM_P_TIMER_eNNN ID %d", timerId)) ;

	/* create OTP Calculation Check expiration timer */
	BTMR_GetDefaultTimerSettings(&timerSettings) ;
		timerSettings.type =  BTMR_Type_eCountDown ;
		timerSettings.cb_isr = (BTMR_CallbackFunc) BHDM_P_TimerExpiration_isr ;
		timerSettings.pParm1 = hHDMI ;
		timerSettings.parm2 = timerId ;
		timerSettings.exclusive = false ;		
	rc = BTMR_CreateTimer(hHDMI->hTMR, timerHandle, &timerSettings) ;

	if (rc != BERR_SUCCESS)
	{
		rc = BERR_TRACE(BERR_LEAKED_RESOURCE);
	}
	
	BDBG_LEAVE(BHDM_P_CreateTimer) ;
	return rc ;
}


static BERR_Code BHDM_P_DestroyTimer(BHDM_Handle hHDMI, BTMR_TimerHandle timerHandle, uint8_t timerId) 
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_P_DestroyTimer) ;

	BDBG_MSG(("Destroy BHDM_P_TIMER_eNNN ID %d", timerId)) ;
	rc = BTMR_DestroyTimer(timerHandle) ;

	BSTD_UNUSED(hHDMI) ;	
	BDBG_LEAVE(BHDM_P_DestroyTimer) ;
	return rc ;
}


static void BHDM_P_TimerExpiration_isr (BHDM_Handle hHDMI, int parm2) 
{
	switch (parm2)
	{
	case BHDM_P_TIMER_eHotPlug :
		BHDM_P_Hotplug_isr(hHDMI) ;
		
		break ;

	case BHDM_P_TIMER_eRxSense :
		
		break ;

	case BHDM_P_TIMER_eFormatDetection :
#if BHDM_CONFIG_FORMAT_CHANGE_DETECTION_SUPPORT
		BHDM_DEBUG_P_Monitor_FormatChangesCheck_isr(hHDMI) ;
#endif		

		break ;
		
	default :
		BDBG_ERR(("hHDM %p Timer %d not handled", hHDMI, parm2)) ;
	} 
}
#endif

