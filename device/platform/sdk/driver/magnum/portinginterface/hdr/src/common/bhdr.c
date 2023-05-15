/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr.c $
 * $brcm_Revision: Hydra_Software_Devel/187 $
 * $brcm_Date: 7/17/12 5:45p $
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr.c $
 * 
 * Hydra_Software_Devel/187   7/17/12 5:45p rgreen
 * SW7425-73: Merge Changes
 * 
 * Hydra_Software_Devel/SW7435-73/2   7/17/12 5:14p rgreen
 * SW7435-73:  Disable reset of clock/data channels when PLL state changes
 * 
 * Hydra_Software_Devel/SW7435-73/1   7/13/12 2:57p rgreen
 * SW7425-73: Filter DVI/HDMI mode changes
 * 
 * Hydra_Software_Devel/186   7/5/12 1:18p vle
 * SW7425-1988: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1988/1   6/26/12 7:06p vle
 * SW7425-1988: Reset Rx HDCP register to prevent incorrect report of
 * BStatus and BCaps.
 * 
 * Hydra_Software_Devel/185   5/31/12 7:20p rgreen
 * SW7425-3152: Reduce console debug messages
 * 
 * Hydra_Software_Devel/184   5/25/12 10:45a rgreen
 * SW7425-2075: Apply MAI Channel ordering to both HBR and PCM not just
 * PCM.
 * 
 * Hydra_Software_Devel/183   5/1/12 3:39p rgreen
 * SW7425-1988: Update BStatus variable as needed.
 * 
 * Hydra_Software_Devel/182   4/24/12 2:31p rgreen
 * SW7425-2075: Merge Changes
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/12/12 11:58a jgarrett
 * SW7425-2075: Removing MAI Channel Reordering in HBR mode
 * 
 * Hydra_Software_Devel/181   2/8/12 11:22a rgreen
 * SW7425-2343: Configure audio channel map and and speaker allocation.
 * 
 * Hydra_Software_Devel/180   1/30/12 4:33p rgreen
 * SW7429-54,SW7435-34: Add workaround to update MAI bus configuration on
 * sample rate changes detected at vsync.  Update BHDR_GetAudioSampleFreq
 * to an iisr declaration
 * 
 * Hydra_Software_Devel/179   1/16/12 1:03p rgreen
 * SW7429-43: Add debug info for channel count and sample width
 * 
 * Hydra_Software_Devel/178   1/13/12 11:03a rgreen
 * SW7429-43: Update MAI bus channel count for PCM mode with Audio
 * Infoframe info.  Add support for 6 and 8 channel PCM
 * 
 * Hydra_Software_Devel/177   1/12/12 3:28p rgreen
 * SW7429-43: Add HBR audio support;
 * 
 * Hydra_Software_Devel/176   11/3/11 5:25p rgreen
 * SW7125-1138:  Clear Deep Color Mode when clearing HDMI mode at the HDMI
 * Rx
 * 
 * Hydra_Software_Devel/175   8/30/11 1:40p rgreen
 * SW7425-991: Default BHDR_Settings to zero before setting individual
 * values; Fix checkin log
 * 
 * Hydra_Software_Devel/174   8/19/11 3:29p rgreen
 * SWDTV-5441: Merge virtual TMR support back to main branch
 * 
 * Hydra_Software_Devel/SWDTV-5441/3   8/19/11 3:06p rgreen
 * SWDTV-5441: Merge Changes
 * 
 * Hydra_Software_Devel/SWDTV-5441/2   8/18/11 5:25p rgreen
 * SWDTV-8288,SW7425-991:  Merge Changes
 * 
 * Hydra_Software_Devel/SWDTV-5441/1   8/2/11 11:28a rgreen
 * SWDTV-5441: Remove depracated code for shared timer.  Use dedicated DVP
 * timer vs shared timer.. Clear HDMI mode once packet errors have
 * stopped
 * 
 * Hydra_Software_Devel/173   8/17/11 3:38p rgreen
 * SWDTV-8288,SW7425-991:   Add BHDR_Get/SetSettings; Move 3548 phy
 * specific code to private files; Add hw passthrough support for 7425
 * 
 * Hydra_Software_Devel/172   8/11/11 11:18a rgreen
 * SWDTV-5441:  Free TMR resources on exit; include timer code for
 * required platforms only; add static declarations
 * 
 * Hydra_Software_Devel/171   7/11/11 8:30p rgreen
 * SWDTV-6898: Explicitly assign BHDR default settings vs declaration
 * 
 * Hydra_Software_Devel/170   7/7/11 5:02p rgreen
 * SW7425-833: Merge changes to use BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/SW7425-833/1   7/1/11 1:53p rgreen
 * SW7425-833: Add BDBG_OBJECT_ASSERT support
 * 
 * Hydra_Software_Devel/169   6/29/11 3:21p rgreen
 * SWDTV-6898,SW7425-136: Clear all callback data for PacetChange Callback
 * 
 * Hydra_Software_Devel/168   6/23/11 2:50p rgreen
 * SWDTV-6771: Incorporate review comments for  ISRC packets
 * 
 * Hydra_Software_Devel/167   6/22/11 4:45p rgreen
 * SWDTV-6771: Add stub for BHDR_GetISRCData
 * 
 * Hydra_Software_Devel/166   6/22/11 11:13a rgreen
 * SWDTV-6771: Add compile time option support for ISRC packets
 * 
 * Hydra_Software_Devel/165   6/10/11 3:49p rgreen
 * SWDTV-6867,SWDTV-7021: Merge Changes
 * 
 * Hydra_Software_Devel/164   5/20/11 4:11p rgreen
 * SWDTV-7092: Remove excess enc enable/disable messages for debugging Ri
 * counts
 * 
 * Hydra_Software_Devel/163   5/12/11 2:33p rgreen
 * SWDTV-7092: reduce debug messages
 * 
 * Hydra_Software_Devel/162   4/29/11 10:47a rgreen
 * SWDTV-186:  Fix 3548 compilation for HDCP Key Set verification
 * 
 * Hydra_Software_Devel/hdr_35233/2   6/1/11 3:43p rbshah
 * SWDTV-6867: 35233: Clean up after bring up
 * 
 * Hydra_Software_Devel/hdr_35233/1   5/12/11 4:21p rbshah
 * SWDTV-7021: 35233: Setup TVMicro for proper hotplug handling.
 * 
 * Hydra_Software_Devel/161   4/25/11 11:00a rgreen
 * SWDTV-6184: Apply FE channel reset due to excessive packet errors on
 * 35230 generation chips only
 * 
 * Hydra_Software_Devel/160   4/21/11 6:33p rgreen
 * SWDTV-6184: Replace explicit register writes to reset Fe channels which
 * private API call
 * 
 * Hydra_Software_Devel/159   4/20/11 4:59p rgreen
 * SWDTV-6184: Remove unused code for warning of hdmi rx clock stopped;
 * channel interrupt will print warning
 * 
 * Hydra_Software_Devel/158   4/19/11 11:25a rgreen
 * SWDTV-6184: Remove 35233 macro references; wait for review/merge
 * changes
 * 
 * Hydra_Software_Devel/157   4/18/11 11:42a rgreen
 * SW7422-186: Merge changes for HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/3   4/13/11 1:54p rgreen
 * SW7422-186: Update HDCP Status change callback
 * 
 * Hydra_Software_Devel/SW7422-186/2   4/11/11 2:54p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/1   3/31/11 2:36p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/156   3/30/11 4:53p rbshah
 * SWDTV-6184: 35233: Get HDR PI to build for the 35233 DTV chip.
 * 
 * Hydra_Software_Devel/155   3/15/11 11:03a rgreen
 * SW7425-170: Update GCP Mode setting for 742x chips; Add more
 * descriptive macro name BHDR_CONFIG_GCP_LEGACY_MODE; keep older name
 * BHDR_CONFIG_GCP_MODE1 for backwards compatibilty
 * 
 * Hydra_Software_Devel/154   3/14/11 11:18a rgreen
 * SWDTV-5935,SW3548-1730: Use audio sample rate from channel status bits
 * to configure MAI bus; always re-configure MAI when switching to HDMI
 * mode
 * 
 * Hydra_Software_Devel/153   3/4/11 3:53p rgreen
 * SWDTV-4664: Fix compilation for HDCP value/status debugging
 * 
 * Hydra_Software_Devel/152   3/3/11 11:48a rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 * OTP ROM based chips
 * 
 * Hydra_Software_Devel/151   2/3/11 11:18a rgreen
 * SW35230-2882: Restore support for parsing Gamut Meta Data packet;
 * Support is disabled by default (see bhdr_config.h to enable)   Fix
 * naming convention for isr functions
 * 
 * Hydra_Software_Devel/150   1/31/11 7:20p rgreen
 * SW35230-2374,SW7422-129: Distinguish between HDCP Key OTP ROM
 * verification vs OTP RAM loading
 * 
 * Hydra_Software_Devel/149   1/13/11 1:42p rgreen
 * SW35230-568:Update GCP Mode setting for additional receiver chips
 * 
 * Hydra_Software_Devel/148   12/16/10 4:44p rgreen
 * SW35230-2416: Increase the delay timer for releasing the FE reset from
 * 10ms to 15ms
 * 
 * Hydra_Software_Devel/147   12/7/10 7:31p rgreen
 * SW35230-2416:  Enable HDCP Key Check and Symbol Loss reset for 35230
 * Only
 * 
 * Hydra_Software_Devel/146   12/7/10 4:59p rgreen
 * SW35230-2416: Add timer support to delay reset of front end;  Update
 * all timers to use timer prefix
 * 
 * Hydra_Software_Devel/145   12/2/10 2:43p rgreen
 * SW35230-2374:Initial support for HDCP Key OTP check
 * 
 * Hydra_Software_Devel/144   11/9/10 10:51a rgreen
 * SW35125-12: Add support for 35125
 * 
 * Hydra_Software_Devel/143   11/5/10 4:32p rgreen
 * SW35230-1723: Fix compilation when BHDR_CONFIG_DEBUG_GCP_XXX options
 * are set for Deep Color and AvMute
 * 
 * Hydra_Software_Devel/142   11/5/10 3:17p rgreen
 * SW35230-1598,SW35230-2116: Reset HDCP core when switching to HDMI mode
 * Free allocated timer when closing BHDR handle to fix memory leak
 * 
 * Hydra_Software_Devel/141   10/18/10 5:04p rgreen
 * SW35230-1598: Fix Hdcp Reset function for 3548 and 742x compilation
 * 
 * Hydra_Software_Devel/140   10/12/10 10:26p pntruong
 * SW35230-1598: Fixed 3548/3556 build errors.
 *
 * Hydra_Software_Devel/139   10/12/10 5:42p rgreen
 * SW35230-1598: Add Hdcp Reset function.  Update audio Enable/Disable
 * calls for AvMute
 * 
 * Hydra_Software_Devel/138   10/1/10 1:52p rgreen
 * SW35230-1194,SW3548-3095: Clear HDMI Mode whenever the Tx is
 * disconnected from our Rx
 * 
 * Hydra_Software_Devel/137   9/29/10 5:17p rgreen
 * SW3548-3078: Allow sw to update HPD hw state only when HPD is
 * disconnected
 * 
 * Hydra_Software_Devel/136   9/28/10 2:28p rgreen
 * SW35230-1502: Mute audio path immediately on Symbol Loss Cleanup/Remove
 * unused legacy code
 * 
 * Hydra_Software_Devel/135   9/21/10 11:09a rgreen
 * SW35230-1194,SW3548-2795: callback/reset VSI to 'No Addl HDMI format'
 * when clearing HDMI mode
 * 
 * Hydra_Software_Devel/134   9/13/10 4:01p rgreen
 * SW35230-1193,SW35230-802: add BHDR_CONFIG_DVP_HR_TMR macro for 35230
 * only
 * 
 * Hydra_Software_Devel/133   9/10/10 2:45p rgreen
 * SW35230-1193,SW35230-802: Continually clear/ignore packets/interrupts
 * until front end is locked/stable.  Display Audio (MAI format) debug
 * messages only when BHDR_CONFIG_DEBUG_AUDIO_FORMAT macro is set
 * 
 * Hydra_Software_Devel/132   9/2/10 3:45p rgreen
 * SW35230-1194,SW3548-2795: unify all needed callbacks for clearing Hdmi
 * Mode
 * 
 * Hydra_Software_Devel/131   8/31/10 2:28p rgreen
 * SW35230-1194: Use AvMute callback on Symbol Loss if AvMute is enabled
 * 
 * Hydra_Software_Devel/130   8/27/10 6:56p rgreen
 * SW35230-802: Respond to channel status update interrupts to correctly
 * configure MAI bus
 * 
 * Hydra_Software_Devel/129   8/27/10 9:50a rgreen
 * SW3548-3061,SW3548-3070: Update to use isr functions in isr context.
 * Correct naming convention
 * 
 * Hydra_Software_Devel/128   8/26/10 7:04p rgreen
 * SW35230-790: Fix compilation error and warnings
 * 
 * Hydra_Software_Devel/127   8/26/10 3:34p rgreen
 * SW35230-790: Reset bHdmiMode state variable when clearing hdmi mode
 * Clean up register accesses
 * 
 * Hydra_Software_Devel/126   8/20/10 5:22p rgreen
 * SW3548-3061: Use isr based BTMR functions in isr functions
 * 
 * Hydra_Software_Devel/125   8/18/10 7:10p rgreen
 * SW3548-3053,SW3548-2951: Remove warning message for deep color config
 * 
 * Hydra_Software_Devel/124   8/16/10 5:22p rgreen
 * SW3548-3053,SW3548-2951: Configure both Master and Slave channel for
 * Deep Color
 * 
 * Hydra_Software_Devel/123   8/5/10 11:20a rgreen
 * SW35230-839, SW35230-810, SW35230-806, SW35230-807: enabled bhdr_fe
 * interrupts when symbol loss occurs
 * 
 * Hydra_Software_Devel/122   7/29/10 2:01p rgreen
 * SW35230-807,SW35230-810: Update format detection processing when Symbol
 * Loss detected
 * 
 * Hydra_Software_Devel/121   7/28/10 4:28p rgreen
 * SW35230-736: Enable  ConfigureAfterHotPlug API
 * 
 * Hydra_Software_Devel/120   7/28/10 3:59p rgreen
 * SW35230-802: Use channel status bits to update MAI bus audio format
 * type
 * 
 * Hydra_Software_Devel/119   7/27/10 8:55p rgreen
 * SW35230-568,SW35230-502: Disable packet processing on stopped or error
 * packet ; Update GCP/pixel phase processing (35230); Reset FE channels
 * on excessive packet errors (35230); Disable HDR interrupts while until
 * PLL timer has expired (35230);
 * 
 * Hydra_Software_Devel/118   7/22/10 4:54p rgreen
 * SW35230-422,SW3548-1540: Enable hw retrieval of the channel status bits
 * on the 35230 Copy the calculated Sampling Frequency to the Hdmi Rx
 * status
 * 
 * Hydra_Software_Devel/117   7/21/10 7:04p rgreen
 * SW35230-568: Improve front end auto frequency range detection
 * 
 * Hydra_Software_Devel/116   7/19/10 4:05p rgreen
 * SW3548-3015: Adjust tolerance level for checking valid number of audio
 * packets
 * 
 * Hydra_Software_Devel/115   7/15/10 1:48p rgreen
 * SW3548-2795: Merge Changes
 * 
 * Hydra_Software_Devel/SW3548-2795/2   7/8/10 5:13p rgreen
 * SW3548-2795: remove reduntant clearing of stored packet memory
 * 
 * Hydra_Software_Devel/SW3548-2795/1   7/7/10 1:35p rgreen
 * SW3548-2795: Clear Packet RAM and locks when exiting 3D mode Clear
 * Packet RAM prior to removing lock Downgrade ERR messages to WRN
 * 
 * Hydra_Software_Devel/114   7/14/10 3:53p rgreen
 * SW35230-558: disable unused LAYOUT_UPDATE interrupt  mute audio at HDMI
 * initialization reset calculation of audio sample rate when low/stopped
 * packet count Update ambiguous CONNECTED/REMOVED message for
 * ConfigureAfterHotPlug disable audio on invalid sample rate
 * 
 * Hydra_Software_Devel/113   7/6/10 11:41p rgreen
 * SW35230-451,SW35230-517: Reset freqency range detection settings on
 * connect/disconnect
 * 
 * Hydra_Software_Devel/112   7/6/10 4:25p rgreen
 * SW35230-502: Enable default pixel packing
 * 
 * Hydra_Software_Devel/111   7/1/10 7:06p rgreen
 * SW35230-451: Update frontend range detection
 * 
 * Hydra_Software_Devel/110   6/22/10 11:34a rgreen
 * SW3548-2954: For 3D, add HDCP Ri consistency check at each link
 * integrity check
 * 
 * Hydra_Software_Devel/109   6/7/10 5:17p rgreen
 * SW35230-128: Separate phy specific code to separate file
 * 
 * Hydra_Software_Devel/108   5/28/10 11:14a rgreen
 * SW3548-2951: For debugging, always display current color mode
 * 
 * Hydra_Software_Devel/107   5/21/10 6:03p rgreen
 * SW3548-2013:Update debug messages to show Deep Color mode when
 * BHDR_CONFIG_DEBUG_GCP_DC macro is enabled
 * 
 * Hydra_Software_Devel/106   5/14/10 3:25p rgreen
 * SW35230-15: Merge support for 35230
 * 
 * Hydra_Software_Devel/105   5/12/10 4:26p rgreen
 * SW3548-2795,SW3548-2642: Merge Changes
 * 
 * Hydra_Software_Devel/SW3548-2642/4   4/20/10 4:10p rgreen
 * SW3548-2795: do not re-configure core while in slave mode for special
 * configurations
 * 
 * Hydra_Software_Devel/SW3548-2642/3   4/1/10 10:14a rgreen
 * SW3548-2708: Call VSI callback to indicate VSI Packet stopped when
 * switched back to DVI mode
 * 
 * Hydra_Software_Devel/SW3548-2642/2   3/11/10 7:02p rgreen
 * SW3548-2642: do not disable slaved interrupts
 * 
 * Hydra_Software_Devel/SW3548-2642/1   3/8/10 5:23p rgreen
 * SW3548-2642:Add message indicating HDCP Consistency Success.  Reset
 * Symbol Loss Count at HP
 * 
 * Hydra_Software_Devel/104   5/3/10 5:14p rgreen
 * SW3548-2914: Fix incorrect offset for configuring HDMI_13_Features
 * register
 * 
 * Hydra_Software_Devel/103   2/19/10 2:45p rgreen
 * SW3548-2642: add HDCP consistency check when both cores are used for
 * 3D.  Disable interrupts on the core slaved for 3D operation
 * 
 * Hydra_Software_Devel/102   1/13/10 4:47p rgreen
 * SW3548-2708: Extend support for Vendor Specific InfoFrame
 * 
 * Hydra_Software_Devel/101   1/8/10 4:18p rgreen
 * SW3548-2642: Move packet processing to separate bhdr_packet_priv.c/h
 * file
 * 
 * Hydra_Software_Devel/100   12/31/09 12:47p rgreen
 * SW3548-2611,SW3548-2013: Perform format change cb before clearing
 * clear/reset of packet RAM
 * Add missing copy of raw packet data to HDR handle for packets
 * Move debug event counter code to bhdr_debug file.  Reset counters only
 * once at Vertical Blank End.
 * Add  BHDR_DEBUG_P_ConfigureEventCounter function
 * Always update 'HDCP Ri Updating' status regardless of bhdr_config
 * setting
 * add debug messages
 * 
 * Hydra_Software_Devel/99   12/29/09 4:37p rgreen
 * SW3548-2670: change ChannelSettings member from 'DeviceSettings' to
 * 'settings'
 * 
 * Hydra_Software_Devel/98   12/21/09 5:05p rgreen
 * SW3556-809: Update list of supported audio formats for MAI format
 * configuration
 * 
 * Hydra_Software_Devel/97   12/7/09 1:46p rgreen
 * SW3548-2013: Add string terminator to SPD Vendor and Description fields
 * 
 * Hydra_Software_Devel/96   11/25/09 1:15p rgreen
 * SW35230-15: Add support for 35230
 * 
 * Hydra_Software_Devel/95   11/20/09 4:19p rgreen
 * SW3548-2625: Modify default AvMute configuration
 * 
 * Hydra_Software_Devel/94   10/30/09 4:58p rgreen
 * SW3548-2013:  Add debug support for Audio Clock Regeneration Packet
 * 
 * Hydra_Software_Devel/93   10/15/09 10:56a rgreen
 * SW3548-2013: Fix enumeration of  AUDIO_TYPE interrupt in table
 * 
 * Hydra_Software_Devel/92   10/14/09 3:22p rgreen
 * SW3548-2503, SW3548-2520: Clear HDMI mode on symbol loss condition;
 * clearing will force AvMute notiy callback to unset AvMute flag at
 * higher api  layers.
 * 
 * Hydra_Software_Devel/91   10/13/09 1:09p rgreen
 * SW3548-2013: Add Channel ID to all WRN and ERR messages;  Add support
 * for SPD InfoFrame debugging
 * 
 * Hydra_Software_Devel/90   10/7/09 5:09p rgreen
 * SW3548-2013: Add Channel ID to all debug messages
 * 
 * Hydra_Software_Devel/89   10/6/09 3:56p rgreen
 * SW3548-2520: Execute callback for AvMute whenever switching back to DVI
 * mode so higher levels will correctly track AvMute status.  Run
 * ProcessModeChange before running SymbolLoss SM
 * 
 * Hydra_Software_Devel/88   10/5/09 10:31a rgreen
 * SW3548-2013: change packet type debug messages from numbers to text
 * 
 * Hydra_Software_Devel/87   9/30/09 10:33p rgreen
 * SW3548-2013:Eliminate repetitive Hdmi Rx Status messages
 * 
 * Hydra_Software_Devel/86   9/29/09 5:42p rgreen
 * SW3548-2013: Add option to enable/disable AUDIO_TYPE debug messages.
 * 
 * Hydra_Software_Devel/85   9/18/09 3:29p rgreen
 * SW3548-2485: do not use a  counter (over 3 frames) to determine valid
 * Symbol Lock status, consider Symbol Locked  based on current status
 * read from the hardware
 * Change location of ConfigureAfterHotPlug Interrupt Status debug message
 * 
 * Hydra_Software_Devel/84   9/15/09 11:42p rgreen
 * SW3548-2476:Fix incorrect callback parameter for VideoFormatChange
 * callback
 * 
 * Hydra_Software_Devel/83   8/31/09 2:04p rgreen
 * SW3548-2013: Add channel numbers to specific debug messages to
 * distinguish source of message
 * Default SymbolLoss to true in BHDR_GetHdmiRxStatus in case status
 * checking fails prior to setting.
 * 
 * Hydra_Software_Devel/82   8/20/09 6:13p rgreen
 * PR57890,PR54000:
 * Report debug warnings on inconsistencies with external switches and
 * equalizer settings
 * Add Symbol Loss SM for debugging
 * Reduce repetitive debug messages; Remove unused variables
 * 
 * Fix Packet debug messages to display when BHDR_CONFIG_DEBUG_X options
 * are set
 * 
 * Hydra_Software_Devel/81   8/12/09 5:30p rgreen
 * PR48585,PR54387: Initialize/Clear DC threshold at BHDR_Open
 * Eliminate excessive messages for Audio SR debugging
 * 
 * Hydra_Software_Devel/80   8/10/09 5:38p rgreen
 * PR57526: Fix bug with ePacketStatus; Update status after parsing
 * received packet
 * 
 * Hydra_Software_Devel/79   6/18/09 10:45a rgreen
 * PR56097:  Add flag to indicate validity of channel status information
 * returned by BHDR_GetHdmiRxStatus()
 * 
 * Hydra_Software_Devel/78   6/10/09 11:38a rgreen
 * PR54000: Add additional comments/debug code for General Control Packet
 * Set/Clear AvMute values
 * 
 * Hydra_Software_Devel/77   6/9/09 10:42a rgreen
 * PR51113: Fix BHDR_Open failure Resource Leak; Coverity Issue
 * 
 * Hydra_Software_Devel/76   6/5/09 6:57p rgreen
 * PR55586: Disable HDMI Frontend and Receiver interrupts when input is
 * not selected
 * 
 * Hydra_Software_Devel/75   5/14/09 11:15a rgreen
 * PR55148: Remove compilation warning
 * 
 * Hydra_Software_Devel/74   5/11/09 4:10p rgreen
 * PR54000: Update parsing of HDMI Audio Infoframe to support debug
 * information
 * 
 * Hydra_Software_Devel/73   5/8/09 3:28p rgreen
 * PR54924: Change HDMI_Detected to bHdmiMode to match RDB name
 * 
 * Hydra_Software_Devel/72   5/7/09 11:31a rgreen
 * PR54916:Add register offset to properly address 2nd HDMI Port
 * 
 * Hydra_Software_Devel/71   5/4/09 6:03p rgreen
 * PR54810: Remove references to unused Gamut Meta Data Packet
 * 
 * Hydra_Software_Devel/70   5/1/09 4:16p rgreen
 * PR54000: Move debug functions to separate bhdr_debug.c file.
 * Add options to dump Infoframes, monitor format changes, monitor values
 * etc
 * 
 * Hydra_Software_Devel/69   4/20/09 3:01p rgreen
 * PR54387: Eliminate repetitive Sample Rate debug messages on fluctuating
 * calculations.
 * 
 * Hydra_Software_Devel/68   4/8/09 10:03a rgreen
 * PR54000: Add debug information for AVI Infoframe
 * 
 * Hydra_Software_Devel/67   3/31/09 10:50p rgreen
 * PR53692,PR53750:
 * Fixed a potential race condition in BHDR_P_HandleInterrupt_isr(). If
 * the ISR is called before the HDR FrontEnd handle is attached
 * Add Critical Sections when installing callbacks
 * 
 * Hydra_Software_Devel/66   3/24/09 6:56p rgreen
 * PR53557: Disable Auto Clear AvMute.  Add option to mute audio
 * immediately from HDMI Rx core
 * 
 * Hydra_Software_Devel/65   3/19/09 10:44a rgreen
 * PR53396: Add BHDR_CONFIG_DEBUG_INPUT_SIGNAL macro to enable debug
 * messages.  Disable by default
 * 
 * Hydra_Software_Devel/64   3/16/09 11:24a rgreen
 * PR53247: Report ValidStatus false, if PLL is not locked or symbol loss
 * is true
 * 
 * Hydra_Software_Devel/63   3/6/09 11:08a rgreen
 * PR51307: HDCP Repeater Support changes
 * 
 * Hydra_Software_Devel/62   3/2/09 5:50p rgreen
 * PR52652: Add separate isr function to handle DVI vs HDMI mode
 * detection.  Process on vertical blank end and mode change interrupts
 * 
 * Hydra_Software_Devel/61   2/26/09 7:56p rgreen
 * PR51307: Add compile time option (BHDR_HDCP_REPEATER_SUPPORT) for HDCP
 * Repeater
 * 
 * Hydra_Software_Devel/60   2/26/09 4:00p rgreen
 * PR52257:
 * Enable Packet Stop detection for ACP packet
 * On DVI mode or Packet Ram initialization, disable any set packet stop
 * detection and clear internal packet memory
 * Remove clearing of AudioContentProtection packet in
 * BHDR_GetAudioContentProtection
 * Correctly enable  Packet Stop detection
 * 
 * Hydra_Software_Devel/59   2/25/09 10:47a rgreen
 * PR44505,PR52050,PR52206: Update AvMute auto clear functionality
 * Use SysClock to calculate Sample Rate instead of N/CTS values, update
 * Mai Format register accordingly
 * Disable SR interrupt, use SR calulations instead
 * Eliminate excessive warning messages for HDMI Get Status functions
 * Update BREG register reads/writes to use hRegister vs hHDR->hRegister
 * 
 * Hydra_Software_Devel/58   2/9/09 3:50p rgreen
 * PR51752,PR51898:
 * combine HDMI_TO_DVI, DVI_TO_HDMI interrupt processing
 * 
 * Read status registers (GetHdmiRxStatus, GetPixelClockEstimate,
 * GetHdmiRxDetectedVideoFormat, etc)
 * at interrupt time vs task time
 * BHDR_GetPixelClockEstimate moved from bhdr_fe.c to bhdr.c
 * Add private isr function for retrieving video format
 * 
 * Use generic BTMR to handle auto clear AvMute
 * 
 * Correct naming convention for isr functions
 * Add debug support to disable AvMute Callback
 * Add debug function for received HDCP values
 * 
 * Hydra_Software_Devel/57   1/27/09 2:25p rgreen
 * PR50940: Fix bug with re-enabling audio
 * 
 * Hydra_Software_Devel/56   1/26/09 5:22p rgreen
 * PR50940, PR51601: Mute audio until N, CTS values are stable
 * Add Hot Plug isr callback
 * 
 * Hydra_Software_Devel/55   1/20/09 1:46p rgreen
 * PR48775: Fix bug with VSync Counter update.
 * 
 * Hydra_Software_Devel/54   12/18/08 6:30p rgreen
 * PR48775:Add changes for Sample Rate calculation
 * 
 * Hydra_Software_Devel/53   12/17/08 4:13p rgreen
 * PR48775: Use BTMR for calculating time between audio packet counts
 * 
 * Hydra_Software_Devel/52   12/16/08 2:22p rgreen
 * PR49443,PR50412: Add setting top BHDR_Open to enable/disable I2C pullup
 * resistor.
 * Disable the VSYNC_LEAD_EDGE interrupt
 * 
 * Hydra_Software_Devel/51   12/9/08 11:24a rgreen
 * PR49943: Add support for i2c pullup configuration for B0 only
 * 
 * Hydra_Software_Devel/50   12/5/08 6:54p rgreen
 * PR49943,PR48226: Add support for i2c pullup configuration
 * Use VerticalBlankEnd vs Leading Edge for checking Deep Color
 * 
 * Hydra_Software_Devel/49   12/3/08 3:33p rgreen
 * PR49898: Fix BHDR_GetHdmiRxStatus to provide correct audio information
 * 
 * Hydra_Software_Devel/48   12/3/08 3:29p rgreen
 * PR45106: Report Symbol Loss on first interrupt, rather than wait for 3
 * consecutive
 * 
 * Hydra_Software_Devel/PR49898/1   12/3/08 7:26p honglang
 * PR49898: BHDR_GetHdmiRxStatus doesn't provide correct audio information
 * 
 * Hydra_Software_Devel/47   12/1/08 4:24p rgreen
 * PR49368: Fix un-initialized variable coverity issue
 * 
 * Hydra_Software_Devel/46   11/14/08 4:17p rgreen
 * PR46509,PR46176: Fix RDB write to CLEAR_HDMI; Add offset for Port1
 * 
 * Hydra_Software_Devel/45   11/14/08 3:58p rgreen
 * PR48775: PR48775: use N and CTS to calculate audio sample rate
 * 
 * Hydra_Software_Devel/PR48775/1   11/10/08 4:49p honglang
 * PR48775: use N and CTS to calculate audio sample rate
 * 
 * Hydra_Software_Devel/44   11/7/08 11:07a rgreen
 * PR48444,PR46176: Fix bug with register update for
 * AuDIO_CHANNEL_STATUS config
 * 
 * Hydra_Software_Devel/43   11/6/08 2:41p rgreen
 * PR48585: Update GCP Stop detection fix for Deep Color
 * 
 * Hydra_Software_Devel/42   11/6/08 11:24a rgreen
 * PR48585: Fix GCP Stop detection for Deep Color
 * 
 * Hydra_Software_Devel/41   11/3/08 11:37a rgreen
 * PR48444,PR46509: Fix A0 compilation
 * 
 * Hydra_Software_Devel/40   10/31/08 2:06p rgreen
 * PR48444,PR46509: Update RDB setting for Channel Status config
 * Fix DCP Compliance test issue by switching back to DVI mode on loss of
 * HDMI stream
 * Add B0 debug interrupts
 * 
 * Hydra_Software_Devel/39   10/21/08 8:39p rgreen
 * PR48206: Fix MAI FORMAT register to match raptor
 * 
 * Hydra_Software_Devel/38   10/16/08 5:26p rgreen
 * PR47954,PR48007:
 * Disable   EDID RAM on both ports; Add new method to  Initialize Packet
 * RAM
 * 
 * Hydra_Software_Devel/37   10/15/08 7:02p rgreen
 * PR47954: Disable EDID RAM
 * 
 * Hydra_Software_Devel/36   10/3/08 6:09p rgreen
 * PR45761:Fix debug reporting for GC packets containing no AvMute status
 * 
 * Hydra_Software_Devel/35   10/1/08 9:16p rgreen
 * PR46176: Add support for power management
 * 
 * Hydra_Software_Devel/34   9/29/08 2:46p rgreen
 * PR45761:Always parse GCP packet so processed information can be
 * returned to higher lever.  Use BHDR_CONFIG_DEBUG_GCP_DC (default =
 * OFF) macro to print Deep Color Debug messages
 * 
 * Hydra_Software_Devel/33   9/25/08 1:43p rgreen
 * PR46109: Restore default Window of Opportunity setting
 * 
 * Hydra_Software_Devel/32   9/23/08 2:52p rgreen
 * PR46109: Adjust Window of Opportunity setting
 * 
 * Hydra_Software_Devel/31   9/23/08 2:35p rgreen
 * PR47275: Enable Packet Stop for each packet type when a valid packet of
 * that type is received
 * Update Ram Packet debug messages
 * 
 * Hydra_Software_Devel/30   9/17/08 6:16p rgreen
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application
 * 
 * Hydra_Software_Devel/PR45761/1   9/17/08 5:18p honglang
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application
 * 
 * Hydra_Software_Devel/29   9/3/08 12:01p rgreen
 * PR45441: Fix Gamut Packet Debug compile warning
 * 
 * Hydra_Software_Devel/28   8/14/08 2:46p rgreen
 * PR45106: Monitor Audio Change and Layout Updates
 * Fix GCP Deep Color debug support
 * 
 * Hydra_Software_Devel/27   8/7/08 5:19p rgreen
 * PR45106: Report Symbol Loss Interrupt on SYMBOL_LOSS_CNT_MAX
 * consecutive interrupts
 * 
 * Hydra_Software_Devel/26   7/29/08 5:21p rgreen
 * PR44697:Use PLL Lock status to determine if a vald Pixel Clock can be
 * calculated
 * 
 * Hydra_Software_Devel/25   7/10/08 11:51a rgreen
 * PR44523,PR44505: Add support for Vendor Specific Infoframe
 * Initial changes for update of AvMute timeout
 * Add BHDR_GetVendorSpecificInfoFrameData function
 * 
 * Hydra_Software_Devel/PR44523/2   7/9/08 11:44p rgreen
 * PR44697:Use PLL Lock status to determine if a vald Pixel Clock can be
 * calculated
 * 
 * Hydra_Software_Devel/PR44523/1   7/8/08 4:35p rgreen
 * PR44523,PR44505: Add support for Vendor Specific Infoframe
 * Initial changes for update of AvMute timeout
 * Add BHDR_GetVendorSpecificInfoFrameData function
 * 
 * Hydra_Software_Devel/24   7/10/08 11:06a rgreen
 * PR43750: Fix warning
 * 
 * Hydra_Software_Devel/23   6/27/08 10:53a rgreen
 * PR44260:  Default AUTO_CLEAR_AVMUTE to 1
 * 
 * Hydra_Software_Devel/22   6/25/08 4:24p rgreen
 * PR42977: Remove references to RcvdPicture... no longer used when
 * parsing AviInfoframe;
 * Rename GcpData to GeneralControlPacket
 * 
 * Hydra_Software_Devel/21   6/24/08 7:58p rgreen
 * PR44129: Clear lock only on the updated RAM packet
 * 
 * Hydra_Software_Devel/20   6/24/08 3:56p rgreen
 * PR43750: PR42977:
 * Remove references to HDCP Key loading; done directly from OTP
 * Remove unused references; Manual SyncToVideo control no longer needed
 * PR43750: Set ePacketStatus to stopped regardless of whether or not a
 * callback is installed
 * PR43750: Enable Stop Packet Detection
 * PR43750: clear packet ram when switching to DVI mode or packet stop
 * PR43750: clear packet ram after packet is stop
 * 
 * Hydra_Software_Devel/19   6/24/08 10:45a rgreen
 * PR39463: Remove references to bhdr_hdcp.c; OTP used to load keys
 * 
 * Hydra_Software_Devel/PR43750/5   6/24/08 3:10p rgreen
 * PR42977:
 * Remove references to HDCP Key loading; done directly from OTP
 * Remove unused references; Manual SyncToVideo control no longer needed
 * 
 * Hydra_Software_Devel/PR43750/4   6/23/08 5:37p rgreen
 * PR43750: Set ePacketStatus to stopped regardless of whether or not a
 * callback is installed
 * 
 * Hydra_Software_Devel/PR43750/3   6/23/08 5:25p rgreen
 * PR43750: Enable Stop Packet Detection
 * 
 * Hydra_Software_Devel/PR43750/2   6/20/08 5:36p honglang
 * PR43750: clear packet ram when switching to DVI mode or packet stop
 * 
 * Hydra_Software_Devel/18   6/18/08 7:00p rgreen
 * PR43431: PR43750: Merge Changes
 * 
 * Hydra_Software_Devel/PR43431/4   6/18/08 6:57p rgreen
 * PR43750: Use Video Format Change callback to fix color for DVI mode
 * 
 * Hydra_Software_Devel/PR43431/3   6/17/08 8:19p rgreen
 * PR43431: Clear Packet Error flag when no more packet errors in stream
 * 
 * Hydra_Software_Devel/PR43431/2   6/17/08 4:16p rgreen
 * PR43431:Use ErrorFreePacketFrames vs PacketErrors
 * 
 * Hydra_Software_Devel/PR43431/1   6/17/08 5:34p honglang
 * PR43431: bring up hdmi input on 93549
 * 
 * Hydra_Software_Devel/17   6/16/08 6:25p rgreen
 * PR38947:Remove critical section from processing GBD packet information;
 * now handled at isr time. Remove Install/Uninstall HDCP Key loader
 * function.  Using OTP instead
 * 
 * Hydra_Software_Devel/16   6/16/08 5:58p rgreen
 * PR38947:Correct default setting for deep color
 * Disable counter check for Packet Errors.. disable Packet Error check
 * 
 * Hydra_Software_Devel/15   6/13/08 6:31p rgreen
 * PR38947:Remove unused interrupts; Modify ParseAudioContentProtection
 * for use at isr time
 * 
 * Hydra_Software_Devel/14   6/13/08 3:46p rgreen
 * PR38947: Remove auto switch to RGB; use Stop Detection to auto switch
 * to RGB
 * 
 * Hydra_Software_Devel/13   6/13/08 11:45a rgreen
 * PR38947:Init Packet RAM before enabling interrupts;
 * Disable counter check for Packet Errors
 * remove clearing of AVI Packet... use Stop Detection instead
 * add enum for SET_AV_MUTE interrupt
 * enable Packet Updates and Interrupts
 * verify attached Front End is valid before use
 * 
 * Hydra_Software_Devel/12   6/12/08 12:51a rgreen
 * PR38947:Enable HDCP i2c for initial bringup
 * 
 * Hydra_Software_Devel/11   6/11/08 7:31p rgreen
 * PR38947: Remove 3563 workarounds; check for valid callbacks before
 * calling
 * 
 * Hydra_Software_Devel/10   6/9/08 4:52p rgreen
 * PR38947: Add workarounds for 93548 Bring Up
 * 
 * Hydra_Software_Devel/9   6/7/08 5:52p rgreen
 * PR38947:Initialize HDCP Key Loading address
 * 
 * Hydra_Software_Devel/8   5/20/08 2:29p rgreen
 * PR42489:Remove multiple callback installations
 * 
 * Hydra_Software_Devel/7   5/15/08 5:44p rgreen
 * PR38947,PR39463: Replace Packet Update/Stopped events with new private
 * functions (BHDR_P_ProcessReceivedPackets_isr and
 * BHDR_P_ProcessStoppedPackets_isr)
 * to process received and stopped packets.  New functions will call
 * installed callbacks and return Packet Data and BAVC_HDMI_PacketStatus
 * indicating
 * the state of the packet e.g Updated, Stopped, etc.
 * 
 * Rename ACP to AudioContentProtection
 * 
 * Hydra_Software_Devel/6   5/7/08 1:51p rgreen
 * PR38957,PR39463:
 * Replace BHDR_FE_GetEventHandle with Install/Uninstall Callbacks for
 * Front End Events (Hotplug and CEC)
 * Move HP Initialization for bhdr_fe file
 * Update BHDR_ConfigureAfterHotPlug to update core after HP Event
 * 
 * Hydra_Software_Devel/5   5/6/08 3:54p rgreen
 * PR38957,PR39463: Fix declaration naming conventions; remove AudioError
 * callbacks/events
 * 
 * Hydra_Software_Devel/4   5/6/08 2:02p rgreen
 * PR38947,PR39463:
 * Remove Get/SetAssigned Channel APIs;Use Attach/Detach
 * Change name from hChannel to hFeChannel
 * 
 * Hydra_Software_Devel/3   4/22/08 8:37p rgreen
 * PR40780: Add Pixel Repeat value to HDMI Rx Status
 * 
 * Hydra_Software_Devel/2   4/7/08 11:54p rgreen
 * PR38947: Add HDR 3548/56 Support
 *
 ***************************************************************************/

#include "bhdr_config.h"
#include "bhdr_debug.h"

#include "bchp_dvp_hr.h"
#if BHDR_CONFIG_DVP_HR_TMR
#include "bchp_dvp_hr_tmr.h"
#endif

#include "bhdr_fe.h"
#include "bhdr_fe_priv.h"

#include "bhdr.h"
#include "bhdr_priv.h"
#include "bhdr_packet_priv.h"

#include "bhdr_phy_priv.h"


#include "bfmt.h"
#include "bkni.h"

#include "bacm_spdif.h"


BDBG_MODULE(BHDR) ;
BDBG_OBJECT_ID(BHDR_P_Handle);

#define	BHDR_CHECK_RC( rc, func )	          \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)



/******************************************************************************
Summary:
Array of BFMT_VideoFmts; indexes match the corresponding CEA-861 Video ID Codes 
*******************************************************************************/
static const uint8_t VIC_VideoFmtTable[BAVC_HDMI_MAX_VIDEO_ID_CODES] =
{
	BFMT_VideoFmt_eMaxCount,      /* 00 - not used / unsupported */
	BFMT_VideoFmt_eDVI_640x480p,  /* 01 - Default VIC Code  */
	BFMT_VideoFmt_e480p,          /* 02 HD 480p 4:3 */
	BFMT_VideoFmt_e480p,          /* 03 HD 480p 16:9 */
	BFMT_VideoFmt_e720p,          /* 04 HD 720p */
	BFMT_VideoFmt_e1080i,         /* 05 HD 1080i 4:3 */

	BFMT_VideoFmt_eNTSC,          /* 06 480i, NSTC-M for North America */
	BFMT_VideoFmt_eNTSC,          /* 07 480i, NSTC-M for North America */

	BFMT_VideoFmt_eMaxCount,      /* 08 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 09 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 10 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 11 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 12 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 13 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 14 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 15 - not used / unsupported */
	BFMT_VideoFmt_e1080p,      /* 16 - 1080p  */
		
	BFMT_VideoFmt_e576p_50Hz,     /* 17 HD 576p 50Hz  4:3 */
	BFMT_VideoFmt_e576p_50Hz,     /* 18 HD 576p 50Hz 16:9 */

	BFMT_VideoFmt_e720p_50Hz,     /* 19 HD 720p 50Hz  */
	BFMT_VideoFmt_e1080i_50Hz,    /* 20 HD 1080i 50Hz */

	BFMT_VideoFmt_ePAL_B,         /* 21 any PAL format */
	BFMT_VideoFmt_ePAL_B,         /* 22 any PAL format */
	
	BFMT_VideoFmt_e576p_50Hz,     /* 23 HD 576p 50Hz  4:3 */
	BFMT_VideoFmt_e576p_50Hz,     /* 24 HD 576p 50Hz 16:9 */
	
	BFMT_VideoFmt_ePAL_B,         /* 25 any PAL format 4:3 */
	BFMT_VideoFmt_ePAL_B,         /* 26 any PAL format 16:9 */

	BFMT_VideoFmt_eMaxCount,      /* 27 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 28 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 29 - not used / unsupported */
	BFMT_VideoFmt_eMaxCount,      /* 30 - not used / unsupported */
	BFMT_VideoFmt_e1080p_50Hz,      /* 31 - 1080p 50 Hz */
	BFMT_VideoFmt_e1080p_24Hz,      /* 32 - 1080p 24 Hz  */
	BFMT_VideoFmt_e1080p_25Hz,      /* 33 - 1080p 25 Hz  */
	BFMT_VideoFmt_e1080p_30Hz,      /* 34 - 1080p 30 Hz  */	
} ;


/* create lookup table for Channel Status, MAI Format values */
typedef struct CSBitsMaiFormatLookup
{
	uint32_t AudioSampleRateHz ;
	uint32_t MinFreq ; 
	uint32_t MaxFreq ;
	uint8_t ChannelStatusBits ;
	uint8_t MaiSampleRate ;
} CSBitsMaiFormatLookup ;


#define BHDR_P_CSBits_48kHz 0x02
#define BHDR_P_CSBits_32kHz 0x03
#define BHDR_P_CSBits_44_1kHz 0x00
#define BHDR_P_CSBits_88_2kHz 0x08
#define BHDR_P_CSBits_96kHz 0x0A
#define BHDR_P_CSBits_176_4kHz 0x0C
#define BHDR_P_CSBits_192kHz 0x0E
#define BHDR_P_CSBits_768kHz 0x09

#define BHDR_P_MaiSampleRate_NotIndicated 0x00


#define BHDR_P_MaiSampleRate_48kHz     0x09
#define BHDR_P_MaiSampleRate_32kHz     0x07
#define BHDR_P_MaiSampleRate_44_1kHz 0x08
#define BHDR_P_MaiSampleRate_88_2kHz    0xB
#define BHDR_P_MaiSampleRate_96kHz     0xC
#define BHDR_P_MaiSampleRate_176_4kHz  0xE
#define BHDR_P_MaiSampleRate_192kHz      0xF

static const CSBitsMaiFormatLookup FsLookupTable[] =
{
	{ 48000, 47000,   49000,   BHDR_P_CSBits_48kHz,    BHDR_P_MaiSampleRate_48kHz},
	{ 32000, 31000,   33000,   BHDR_P_CSBits_32kHz,    BHDR_P_MaiSampleRate_32kHz},
	{ 44100, 43000,   45100,   BHDR_P_CSBits_44_1kHz,    BHDR_P_MaiSampleRate_44_1kHz},
	{ 96000, 95000,   97000,   BHDR_P_CSBits_96kHz,    BHDR_P_MaiSampleRate_96kHz},
	{ 88200, 87200,   89200,   BHDR_P_CSBits_88_2kHz,    BHDR_P_MaiSampleRate_88_2kHz},
	{176400, 175400,  177400,   BHDR_P_CSBits_176_4kHz,    BHDR_P_MaiSampleRate_176_4kHz},
	{192000, 191000,  193000,   BHDR_P_CSBits_192kHz,    BHDR_P_MaiSampleRate_192kHz},
	{768000, 797000,  769000,   BHDR_P_CSBits_768kHz,    BHDR_P_MaiSampleRate_192kHz},      /* HBR is sent as 4x 192kHz on MAI */
	{0, 0, 0,  0x00, 0x00}
} ;


#define BHDR_P_MaiAudioFormat_PCM_2CH 0xFE
#define BHDR_P_MaiAudioFormat_PCM_3CH 0x03
#define BHDR_P_MaiAudioFormat_PCM_6CH 0x86
#define BHDR_P_MaiAudioFormat_PCM_8CH 0x88

#define BHDR_P_MaiAudioFormat_COMPRESSED 0xFF
#define BHDR_P_MaiAudioFormat_HBR 0xC8



/*Audio sample width look up table*/

#define BHDR_AUDIO_SAMPLE_LENGTH_NUMBER 6

static const int audioSampleLengthTable[][BHDR_AUDIO_SAMPLE_LENGTH_NUMBER] = 
{
	/*20 bits*/
	{16, 18, 0, 19, 20, 17} ,

	/*24 bits*/
	{20, 22, 0, 23, 24, 21} 
} ;


#define BHDR_P_PACKET_WORDS 9 

static void BHDR_P_ProcessModeChange_isr(BHDR_Handle hHDR) ;
static void BHDR_P_ProcessVerticalBlankEnd_isr(BHDR_Handle hHDR) ;

static BERR_Code BHDR_P_WriteRxI2cRegisterSpace_isr(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length) ;  

static BERR_Code BHDR_P_GetChannelStatusBits_isr(
	BHDR_Handle hHDR, BACM_SPDIF_ChannelStatus *stChannelStatus) ;

static void BHDR_P_GetVideoFormat_isr(BHDR_Handle hHDR) ;
#if BHDR_CONFIG_AVMUTE_AUDIO_IMMEDIATELY
static void BHDR_P_EnableAudio_isr(BHDR_Handle hHDR, bool enable) ;
#endif

static void BHDR_P_ClearHdmiMode_isr(BHDR_Handle hHDR) ;


static BERR_Code BHDR_P_CreateTimer(
	BHDR_Handle hHDR, BTMR_TimerHandle * timerHandle, uint8_t timerId) ;

static BERR_Code BHDR_P_DestroyTimer(
	BHDR_Handle hHDR, BTMR_TimerHandle timerHandle, uint8_t timerId) ;

static void BHDR_P_TimerExpiration_isr (BHDR_Handle hHDR, int parm2) ;


/******************************************************************************
Summary:
Interrupt Callback Table to describe interrupt Names, isrs, and Masks 
*******************************************************************************/
typedef struct BHDR_P_InterruptCbTable
{
	BINT_Id       	  IntrId;
	int               iParam2; 
	bool              enable ; /* debug purposes */
} BHDR_P_InterruptCbTable ;

static const BHDR_P_InterruptCbTable BHDR_Interrupts0[MAKE_INTR_ENUM(LAST)] =
{
	/* 00 */   { BCHP_INT_ID_HDMI_RX_INTR2_0_SET_AV_MUTE, BHDR_INTR_eSET_AV_MUTE, true },
	
	
	/* 01 */   { BCHP_INT_ID_HDMI_RX_INTR2_0_AV_MUTE_UPDATE , BHDR_INTR_eAV_MUTE_UPDATE, true },
	

	/* 02 */   { BCHP_INT_ID_HDMI_RX_INTR2_0_DVI_TO_HDMI, BHDR_INTR_eDVI_TO_HDMI, true },
	
	/* 03 */   { BCHP_INT_ID_HDMI_RX_INTR2_0_HDMI_TO_DVI, BHDR_INTR_eHDMI_TO_DVI, true },
	
	/* 04 */  { BCHP_INT_ID_HDMI_RX_INTR2_0_AKSV_UPDATE, BHDR_INTR_eAKSV_UPDATE, true },
	
	/* 05 */   { BCHP_INT_ID_HDMI_RX_INTR2_0_REQUEST_KEYS, BHDR_INTR_eREQUEST_KEYS, true },
	
	/* 06 */  { BCHP_INT_ID_HDMI_RX_INTR2_0_REQUEST_KSVS, BHDR_INTR_eREQUEST_KSVS, BHDR_CONFIG_HDCP_REPEATER},
	
	
	/* 07 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_I2C_TRANSACTION_COMPLETE,		
		 BHDR_INTR_eI2C_TRANSACTION_COMPLETE, (bool) BHDR_CONFIG_DEBUG_I2C },
		 
	
	/* 08 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_PJ_UPDATE, 		
		 BHDR_INTR_ePJ_UPDATE, false },
		 
	
	/* 09 */  { BCHP_INT_ID_HDMI_RX_INTR2_0_SYMBOL_LOSS, BHDR_INTR_eSYMBOL_LOSS, true }, 
	
	/* 10 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_INVALID_DATA_ISLAND_LENGTH,
		 BHDR_INTR_eINVALID_DATA_ISLAND_LENGTH, true },
		 
	/* 11 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_CHANNEL_STATUS_UPDATE,
		 BHDR_INTR_eCHANNEL_STATUS_UPDATE, BHDR_CONFIG_HW_CHANNEL_STATUS },
		 
	/* 12 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_AUDIO_VALIDITY_BIT_UPDATE,
		 BHDR_INTR_eAUDIO_VALIDITY_BIT_UPDATE, true },

	/* 13 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_RAM_PACKET_UPDATE,   BHDR_INTR_eRAM_PACKET_UPDATE, true },

	
	/* 14 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_RAM_PACKET_STOP,   BHDR_INTR_eRAM_PACKET_STOP, true },
	
	/* 15 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_PACKET_SYNC_ERROR,   BHDR_INTR_ePACKET_SYNC_ERROR, true },
	
	/* 16 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_LAYOUT_UPDATE,   BHDR_INTR_eLAYOUT_UPDATE, false },
	
	/* 17 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_AUDIO_TYPE_CHANGE,   
		BHDR_INTR_eAUDIO_TYPE_CHANGE, false },

#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)) && (BCHP_VER >= BCHP_VER_B0)
	/* 18 */ {BCHP_INT_ID_HDMI_RX_INTR2_0_RGB_UNDER_RANGE, BHDR_INTR_eRGB_UNDER_RANGE, false},
	
	/* 20 */ {BCHP_INT_ID_HDMI_RX_INTR2_0_AUDIO_FIFO_OVER_FLOW, BHDR_INTR_eAUDIO_FIFO_OVER_FLOW, false},
	
	/* 21 */ {BCHP_INT_ID_HDMI_RX_INTR2_0_AUDIO_FIFO_UNDER_FLOW, BHDR_INTR_eAUDIO_FIFO_UNDER_FLOW, false},
#endif	

	/* 27 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_VSYNC_LEAD_EDGE,  	BHDR_INTR_eVSYNC_LEAD_EDGE, false },
	
	/* 28 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_VERTICAL_BLANK_END,  	BHDR_INTR_eVERTICAL_BLANK_END, true },
	
	
	/* 29 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_EXCESSIVE_PACKET_ERRORS, BHDR_INTR_eEXCESSIVE_PACKET_ERRORS, true },
	
	/* 30 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_FORMAT_DETECT_COUNT_SATURATED, BHDR_INTR_eFORMAT_DETECT_COUNT_SATURATED, true },
	
	
	/* 31 */ { BCHP_INT_ID_HDMI_RX_INTR2_0_ERROR_INTERRUPT, BHDR_INTR_eERROR_INTERRUPT, false }
} ;


#if BHDR_HAS_MULTIPLE_PORTS 
static const BHDR_P_InterruptCbTable BHDR_Interrupts1[MAKE_INTR_ENUM(LAST)] =
{
	/* 00 */   { BCHP_INT_ID_HDMI_RX_INTR2_1_SET_AV_MUTE, BHDR_INTR_eSET_AV_MUTE, true },
	
	
	/* 01 */   { BCHP_INT_ID_HDMI_RX_INTR2_1_AV_MUTE_UPDATE , BHDR_INTR_eAV_MUTE_UPDATE, true },
	

	/* 02 */   { BCHP_INT_ID_HDMI_RX_INTR2_1_DVI_TO_HDMI, BHDR_INTR_eDVI_TO_HDMI, true },
	
	/* 03 */   { BCHP_INT_ID_HDMI_RX_INTR2_1_HDMI_TO_DVI, BHDR_INTR_eHDMI_TO_DVI, true },
	
	/* 04 */  { BCHP_INT_ID_HDMI_RX_INTR2_1_AKSV_UPDATE, BHDR_INTR_eAKSV_UPDATE, true },
	
	/* 05 */   { BCHP_INT_ID_HDMI_RX_INTR2_1_REQUEST_KEYS, BHDR_INTR_eREQUEST_KEYS, true },
	
	/* 06 */  { BCHP_INT_ID_HDMI_RX_INTR2_1_REQUEST_KSVS, BHDR_INTR_eREQUEST_KSVS, BHDR_CONFIG_HDCP_REPEATER },
	
	
	/* 07 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_I2C_TRANSACTION_COMPLETE,		
		 BHDR_INTR_eI2C_TRANSACTION_COMPLETE, (bool) BHDR_CONFIG_DEBUG_I2C },
		 
	
	/* 08 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_PJ_UPDATE, 		
		 BHDR_INTR_ePJ_UPDATE, false },
		 
	
	/* 09 */  { BCHP_INT_ID_HDMI_RX_INTR2_1_SYMBOL_LOSS, BHDR_INTR_eSYMBOL_LOSS, true }, 
	
	/* 10 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_INVALID_DATA_ISLAND_LENGTH,
		 BHDR_INTR_eINVALID_DATA_ISLAND_LENGTH, true },
		 
	/* 11 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_CHANNEL_STATUS_UPDATE,
		 BHDR_INTR_eCHANNEL_STATUS_UPDATE, BHDR_CONFIG_HW_CHANNEL_STATUS},
		 
	/* 12 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_AUDIO_VALIDITY_BIT_UPDATE,
		 BHDR_INTR_eAUDIO_VALIDITY_BIT_UPDATE, true },

	/* 13 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_RAM_PACKET_UPDATE,   BHDR_INTR_eRAM_PACKET_UPDATE, true },

	
	/* 14 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_RAM_PACKET_STOP,   BHDR_INTR_eRAM_PACKET_STOP, true },
	
	/* 15 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_PACKET_SYNC_ERROR,   BHDR_INTR_ePACKET_SYNC_ERROR, true },
	
	/* 16 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_LAYOUT_UPDATE,   BHDR_INTR_eLAYOUT_UPDATE, false },
	
	/* 17 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_AUDIO_TYPE_CHANGE,   
		BHDR_INTR_eAUDIO_TYPE_CHANGE, false },

#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)) && (BCHP_VER >= BCHP_VER_B0)
	/* 18 */ {BCHP_INT_ID_HDMI_RX_INTR2_1_RGB_UNDER_RANGE, BHDR_INTR_eRGB_UNDER_RANGE, false},
	
	/* 20 */ {BCHP_INT_ID_HDMI_RX_INTR2_1_AUDIO_FIFO_OVER_FLOW, BHDR_INTR_eAUDIO_FIFO_OVER_FLOW, false},
	
	/* 21 */ {BCHP_INT_ID_HDMI_RX_INTR2_1_AUDIO_FIFO_UNDER_FLOW, BHDR_INTR_eAUDIO_FIFO_UNDER_FLOW, false},
#endif	

	/* 27 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_VSYNC_LEAD_EDGE,  	BHDR_INTR_eVSYNC_LEAD_EDGE, false },
	
	/* 28 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_VERTICAL_BLANK_END,  	BHDR_INTR_eVERTICAL_BLANK_END, true },
	
	
	/* 29 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_EXCESSIVE_PACKET_ERRORS, BHDR_INTR_eEXCESSIVE_PACKET_ERRORS, true },
	
	/* 30 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_FORMAT_DETECT_COUNT_SATURATED, BHDR_INTR_eFORMAT_DETECT_COUNT_SATURATED, true },
	
	
	/* 31 */ { BCHP_INT_ID_HDMI_RX_INTR2_1_ERROR_INTERRUPT, BHDR_INTR_eERROR_INTERRUPT, false }
} ;
#endif


/*******************************************************************************
BERR_Code BHDR_GetDefaultSettings
Summary: Get the default settings for the HDMI device.
*******************************************************************************/
BERR_Code BHDR_GetDefaultSettings(
   BHDR_Settings *pDefault  /* [in] pointer to memory to hold default settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	BDBG_ENTER(BHDR_GetDefaultSettings) ;

	BDBG_ASSERT(pDefault) ;

	BKNI_Memset(pDefault, 0, sizeof(BHDR_Settings)) ;

	pDefault->eCoreId = 0 ;
	pDefault->bParseAVI = true ;  /* HDR PI should process AVI Packets; disable if app is to do processing */
	pDefault->hTmr = (BTMR_Handle) NULL ;

	/* bDisableI2cPadSclPullup = disable I2C Pad SCL Pull Up */
	pDefault->bDisableI2cPadSclPullup = true ; 
	
	/* bDisableI2cPadSdaPullup = disable I2C Pad SDA Pull Up */
	pDefault->bDisableI2cPadSdaPullup =	true ; 

	pDefault->hHDRMaster = NULL ;  /* Master HDR handle for 3D, if exists... */
	
#if BHDR_CONFIG_HW_PASSTHROUGH_SUPPORT	
	pDefault->bHdmiHardwarePassthrough = false ;
#endif

	BDBG_LEAVE(BHDR_GetDefaultSettings) ;
	return rc ;
}



/******************************************************************************
BERR_Code BHDR_Open
Summary: Open/Initialize the HDMI device 
*******************************************************************************/
BERR_Code BHDR_Open(
   BHDR_Handle *phHDR,       /* [out] HDMI handle */
   BCHP_Handle hChip,         /* [in] Chip handle */
   BREG_Handle hRegister,     /* [in] Register Interface to HDMI Tx Core */
   BINT_Handle hInterrupt,    /* [in] Interrupt handle */
   const BHDR_Settings  *pSettings /* [in] default HDMI settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	BHDR_Handle hHDR = NULL ;
	BHDR_P_HdrCoreId eCoreId ;
	
	uint32_t ulOffset, Register ;
	uint8_t i ;

	const BHDR_P_InterruptCbTable *pInterrupts ;
	BTMR_Settings timerSettings  ;
	BHDR_DEBUG_P_EventCounter stEventCounter ;
	
	BDBG_ENTER(BHDR_Open) ;
	
	/* verify parameters */
	BDBG_ASSERT(hChip) ;
	BDBG_ASSERT(hRegister) ;
	BDBG_ASSERT(hInterrupt) ;

	/* create the HDMI Handle */
	hHDR = (BHDR_Handle) BKNI_Malloc(sizeof(BHDR_P_Handle)) ;
	if (!hHDR)
	{
		BDBG_ERR(("Unable to allocate memory for HDMI Handle")) ;
		rc = BERR_OUT_OF_SYSTEM_MEMORY ;
		goto done ;
	}
	
	/* zero out all memory associated with the HDMI Device Handle before using */
	BKNI_Memset(hHDR, 0, sizeof(BHDR_P_Handle)) ;
	BDBG_OBJECT_SET(hHDR, BHDR_P_Handle) ;

	/* assign the handles passed in as parameters */
	hHDR->hChip      = hChip ;     
	hHDR->hRegister  = hRegister ; 
	hHDR->hInterrupt = hInterrupt ;

	if ((!pSettings) || (!pSettings->hTmr))
	{
		BDBG_ERR(("BTMR Device must be available")) ;
		rc = BERR_NOT_INITIALIZED ;
		goto done ;
	}
	
 	hHDR->hTimerDevice = pSettings->hTmr ;

	hHDR->DeviceSettings = *pSettings ;

	eCoreId = pSettings->eCoreId ;
	BDBG_ASSERT(eCoreId < BHDR_P_eHdrCoreIdMax) ;
	hHDR->eCoreId = eCoreId ;


	hHDR->AvMute = 0x0 ;

	hHDR->bPacketErrors = false ;
	hHDR->ErrorFreePacketFrames = BHDR_P_MAX_ERROR_FREE_PACKET_FRAMES ;

	hHDR->AudioClockRegenerationPacket.N = 1 ;
	hHDR->AudioClockRegenerationPacket.CTS = 1 ;

	hHDR->uiColorDepthFrameCounter = 0 ;
	hHDR->bDeepColorMode = false ;

#if BHDR_CONFIG_DVP_HR_TMR
	hHDR->bWaitForTimer = false ;
#endif	

#if BHDR_CONFIG_SYMBOL_LOSS_SM
	BHDR_P_SymbolLossSM_Initialize(hHDR) ;
#endif	

	hHDR->bPreviousStatus = true ;
	
	BTMR_GetDefaultTimerSettings(&timerSettings) ;
    	timerSettings.type =  BTMR_Type_eSharedFreeRun ;
	rc = BTMR_CreateTimer(hHDR->hTimerDevice, &hHDR->timerGeneric,  &timerSettings) ;
	if(rc != BERR_SUCCESS)
	{
		rc = BERR_TRACE(BERR_LEAKED_RESOURCE);
		goto done ;
	}

#if BHDR_CONFIG_HDCP_KEY_OTP_ROM 
#if ((HDMI_RX_GEN == 35230) || (HDMI_RX_GEN == 7422))
	BHDR_P_CreateTimer(hHDR, 
		&hHDR->timerOtpCrcCalc, BHDR_P_TIMER_eOtpCrcCalculation) ;
#endif
#endif

#if BHDR_CONFIG_RESET_FE_ON_SYMBOL_LOSS
	BHDR_P_CreateTimer(hHDR, 
		&hHDR->timerHdmiFeReset, BHDR_P_TIMER_eHdmiFeReset) ;
#endif

	BHDR_P_CreateTimer(hHDR, 
		&hHDR->timerDviHdmiModeChange, BHDR_P_TIMER_eDviHdmiModeChange) ;

#if BHDR_CONFIG_DEBUG_TIMER_S
	/* initialize debug timestamp if used */
	BTMR_ReadTimer(hHDR->timerGeneric, &hHDR->DebugTimeStamp) ;
#endif 	
	
	/* settings for recalculation of SR up to BHDR_CONFIG_CONSECUTIVE_SR_CALCS times */
	hHDR->uiConsecutiveSampleRateCalculations = 0 ;	
	hHDR->AudioSampleRateHz = 0 ;

	

 	/* Register offset from HDR */
	hHDR->ulOffset = ulOffset = 0 
#if BHDR_HAS_MULTIPLE_PORTS 
	+ BHDR_P_GET_REG_OFFSET(hHDR->eHdrCoreId,
		BCHP_HDMI_RX_0_REG_START, BCHP_HDMI_RX_1_REG_START) 
#endif
	;
	
#if BHDR_CONFIG_AVMUTE_AUDIO_IMMEDIATELY
	/* Initialize HDMI Rx Audio to OFF; */
	/* Avoids noise,pops, etc until valid SR is received */
	BHDR_P_EnableAudio_isr(hHDR, false) ;
#endif

	/* settings for recalculation of SR up to BHDR_CONFIG_CONSECUTIVE_SR_CALCS times */
	hHDR->uiConsecutiveSampleRateCalculations = 0 ;	
	hHDR->uiPreviousMaiSampleRate = 0 ;
	hHDR->uiPreviousMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_2CH ;
	hHDR->uiPreviousMaiSampleWidth = 16 ;

	
	hHDR->VSyncCounter = 0 ;

#if BHDR_CONFIG_DEBUG_DISPLAY_HDMI_RX_CONFIG
	/* display version information */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_CORE_REV + ulOffset);
	BDBG_WRN(("*****************************************")) ;
	BDBG_WRN(("HDMI Receiver Core %d", eCoreId)) ;
	BDBG_WRN(("   Register Offset %#x; Version 0x%08X", ulOffset, Register)) ;
	BDBG_WRN(("$brcm_Workfile: bhdr.c $")) ;
 	BDBG_WRN(("$brcm_Revision: Hydra_Software_Devel/187 $")) ;
 	BDBG_WRN(("$brcm_Date: 7/17/12 5:45p $")) ;
 	BDBG_WRN(("Compiled: %s %s",  __DATE__, __TIME__)) ;
#if	BHDR_CONFIG_SYMBOL_LOSS_SM
	BDBG_WRN(("Symbol Loss SM: %d", BHDR_CONFIG_SYMBOL_LOSS_SM)) ;
#endif
	BDBG_WRN(("*****************************************")) ;
#endif	
	
	/* Create Events for use with Interrupts */
	BHDR_CHECK_RC(rc, BKNI_CreateEvent(&(hHDR->BHDR_Event_HdmiToDvi))) ;
	BHDR_CHECK_RC(rc, BKNI_CreateEvent(&(hHDR->BHDR_Event_DviToHdmi))) ;
	BHDR_CHECK_RC(rc, BKNI_CreateEvent(&(hHDR->BHDR_Event_AudioChannelStatusUpdate))) ;
	BHDR_CHECK_RC(rc, BKNI_CreateEvent(&(hHDR->BHDR_Event_VBlankEnd))) ;

	if (hHDR->eCoreId == BHDR_P_eHdrCoreId0)
	{
		pInterrupts = BHDR_Interrupts0 ;
		hHDR->HdcpKeyLoadOffset = 0x0 ;
	}
#if BHDR_HAS_MULTIPLE_PORTS 
	else if (hHDR->eCoreId == BHDR_P_eHdrCoreId1)
	{
		pInterrupts = BHDR_Interrupts1 ;
		hHDR->HdcpKeyLoadOffset = 0x28 ;
	}
#endif
	else
	{
		BDBG_ERR(("Unkown Core ID: %d", hHDR->eCoreId)) ;
		BDBG_ASSERT(false) ;
		return BERR_INVALID_PARAMETER ;
	}

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_CONFIG, KEY_LOAD_BASE_ADDRESS) ;
	
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDCP_CONFIG, KEY_LOAD_BASE_ADDRESS, 
		hHDR->HdcpKeyLoadOffset) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset,  Register) ;

#if BHDR_CONFIG_HDCP_REPEATER
	BHDR_HDCP_P_EnableRepeater(hHDR) ;
#endif

#if BHDR_CONFIG_HW_PASSTHROUGH_SUPPORT
	BHDR_P_SetHdmiPassthroughMode(hHDR, pSettings->bHdmiHardwarePassthrough) ;
#endif


	hHDR->FrameCount = 0 ;
	hHDR->PreviousEncryptedFrame = 0xFF ;
	
	/* ... configure EVENT COUNTERS as needed */

	BKNI_Memset(&stEventCounter, 0, sizeof(BHDR_DEBUG_P_EventCounter)) ;

	stEventCounter.bBchEvent = false ;
	stEventCounter.uiCounter = 0 ;
	stEventCounter.ulEventBitMask31_00 = 0 ;
	stEventCounter.ulEventBitMask63_32 = (uint32_t ) (1 << (55 - 32)) ;  /* 55: enc_en_p COUNT_0 */
	BHDR_DEBUG_P_ConfigureEventCounter(hHDR, &stEventCounter) ;

	stEventCounter.uiCounter = 1 ;
	stEventCounter.ulEventBitMask31_00 = 0 ;
	stEventCounter.ulEventBitMask63_32 = (uint32_t ) (1 << (56 - 32)) ;  /* 56: enc_dis_p COUNT_1 */
	BHDR_DEBUG_P_ConfigureEventCounter(hHDR, &stEventCounter) ;

	stEventCounter.uiCounter = 2 ;
	stEventCounter.bBchEvent = false ;
	stEventCounter.ulEventBitMask31_00 = 0 ;
	stEventCounter.ulEventBitMask63_32 = 
		  (uint32_t ) (1 << (33 - 32))   /* 33: pr_audio_sample_sp0_p */
		| (uint32_t ) (1 << (34 - 32))   /* 34: pr_audio_sample_sp1_p */
		| (uint32_t ) (1 << (35 - 32))   /*  35: pr_audio_sample_sp2_p */
		| (uint32_t ) (1 << (36 - 32))   /*  36: pr_audio_sample_sp3_p */
		| (uint32_t ) (1 << (37 - 32))   /*  37: pr_audio_sample_p */
		| (uint32_t ) (1 << (38 - 32)) ; /*  38: audio_packet_p */         /* Audio Packets COUNT_2 */
	BHDR_DEBUG_P_ConfigureEventCounter(hHDR, &stEventCounter) ;

	BHDR_DEBUG_P_ResetAllEventCounters_isr(hHDR) ;

	/* HDMI_TODO for BringUp always enable HDCP need API to enable/disable when system start up complete*/
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_RX_I2C_MISC_CFG_2 + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_RX_I2C_MISC_CFG_2, I2C_ENABLE) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDCP_RX_I2C_MISC_CFG_2, I2C_ENABLE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_RX_I2C_MISC_CFG_2 + ulOffset,  Register) ;
	
	BHDR_InitializePacketRAM(hHDR) ;
	
	/* Register/enable interrupt callbacks */
	for (i = 0; i < MAKE_INTR_ENUM(LAST) ; i++) 
	{
 		BHDR_CHECK_RC( rc, BINT_CreateCallback(&(hHDR->hCallback[i]), 
			hHDR->hInterrupt, pInterrupts[i].IntrId, 
			BHDR_P_HandleInterrupt_isr, (void *) hHDR, i ));

		/* clear interrupt callback */
		BHDR_CHECK_RC(rc, BINT_ClearCallback( hHDR->hCallback[i])) ;

		/* skip interrupt if not enabled in table...  */	
		if (!pInterrupts[i].enable)
			continue ;

#if 0
		/* enable interrupts only when input is active */
		BHDR_CHECK_RC( rc, BINT_EnableCallback( hHDR->hCallback[i] ) );
#endif
	}

	/* clear locked packets  */
	for (i = 0 ; i < BHDR_P_NUM_PACKETS; i++)
	{
		BHDR_CHECK_RC(rc, BHDR_P_ClearPacketRAMLock_isr(hHDR, i)) ;
	}
	
	/* always start configuration in DVI mode */
	BKNI_EnterCriticalSection() ;
		BHDR_P_ClearHdmiMode_isr(hHDR) ;
	BKNI_LeaveCriticalSection() ;
		

	/* clear deep color threshold */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, GCP_CD_FRAME_COUNT_THRESHOLD) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset,  Register) ;		

#if BHDR_CONFIG_GCP_LEGACY_MODE	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_2 + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_2, NO_GCP_COUNT_THRESHOLD) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_2 , NO_GCP_COUNT_THRESHOLD, 0xFF) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_2 + ulOffset,  Register) ;		
#endif
	
	

	/* set the threshold for packet errors */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_BCH_ECC_CFG + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_BCH_ECC_CFG, PACKET_ERR_THRESHOLD) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_BCH_ECC_CFG, PACKET_ERR_THRESHOLD,
		BHDR_P_PACKET_ERROR_THRESHOLD) ;
	BREG_Write32(hRegister,  BCHP_HDMI_RX_0_BCH_ECC_CFG + ulOffset, Register) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_AUDIO_CHANNEL_STATUS_CONFIG + ulOffset) ;
	Register |= BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_STATUS_CONFIG, SAVE_CH_STATUS_EVEN_IF_MISMATCH) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_AUDIO_CHANNEL_STATUS_CONFIG + ulOffset, Register) ;


	/* default to h/w handling Deep Color */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset) ;

	Register &= ~BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, ENABLE_FORCE_COLOR_DEPTH) ;	
	Register &= ~BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, BLOCK_DEFAULT_MODE) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, AUTO_CLEAR_DEEPCOLOR) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, AUTO_CLEAR_DEEPCOLOR, 1) ;		

#if BHDR_CONFIG_DC_PACKING_PHASE_ENABLE
	Register |= BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, FORCE_DEFAULT_MODE) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, BLOCK_DEFAULT_MODE) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, PACKING_PHASE_OFFSET) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, PACKING_PHASE_OFFSET, 1) ;
#else	
	Register &= ~BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, FORCE_DEFAULT_MODE) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, BLOCK_DEFAULT_MODE, 1) ;	

#if BHDR_CONFIG_GCP_LEGACY_MODE
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, GCP_MODE0, 1) ;	
#endif
#endif
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset, Register) ;	


	/* set the correct default for HDMI DETECT MODE */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CFG_1 + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_DIGITAL_FRONT_END_CFG_1, HDMI_DETECT_MODE) ;	
	BREG_Write32(hRegister,  BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CFG_1 + ulOffset, Register) ;

	/* default internal HdmiMode to unknown */
	hHDR->bHdmiMode = 0xFF;

	/* enable the AUTO_CLEAR_AVMUTE  */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_AUDIO_MUTE_CFG + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_AUDIO_MUTE_CFG, AUTO_CLEAR_AVMUTE) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_AUDIO_MUTE_CFG, ALWAYS_ALLOW_AVMUTE_UPDATE) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_MUTE_CFG, AUTO_CLEAR_AVMUTE, 1) ;	
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_MUTE_CFG, ALWAYS_ALLOW_AVMUTE_UPDATE, 1) ;
	BREG_Write32(hRegister,  BCHP_HDMI_RX_0_AUDIO_MUTE_CFG + ulOffset, Register) ;

#if BHDR_CONFIG_I2C_PAD_CONTROL
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_I2C_PAD_CONTROL + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_I2C_PAD_CONTROL, SCL_PUP) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_I2C_PAD_CONTROL, SDA_PUP) ;

	/* 0 = enabled; 1 = disabled ; setting must be inverted */
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_I2C_PAD_CONTROL, SCL_PUP, 
		pSettings->bDisableI2cPadSclPullup) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_I2C_PAD_CONTROL, SDA_PUP, 
		pSettings->bDisableI2cPadSdaPullup)  ;
	
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_I2C_PAD_CONTROL + ulOffset, Register) ;
#endif

	/* clear the format detection registers */
	Register = 0xFFFFFFFF;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_FORMAT_DET_UPDATE_CLEAR + ulOffset, Register) ;
	
	Register = 0;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_FORMAT_DET_UPDATE_CLEAR + ulOffset, Register) ;

#if BHDR_CONFIG_DISABLE_EDID_RAM
	/* Disable EDID RAM device */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_EDID_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_EDID_CONTROL, ENABLE_EDID_DEVICE) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_EDID_CONTROL + ulOffset, Register) ;
#endif

	/* keep created pointer */
	*phHDR = hHDR ;	



done:
	if( (rc != BERR_SUCCESS) && (hHDR))
	{
		BKNI_Free(hHDR) ;
		*phHDR = NULL  ;
	}
	
	BDBG_LEAVE(BHDR_Open) ;
	return rc ;
} /* end BHDR_Open */


/******************************************************************************
BERR_Code BHDR_Close
Summary: Close the HDMI connection to the HDMI Rx.
*******************************************************************************/
BERR_Code BHDR_Close(
   BHDR_Handle hHDR  
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t i ;

	BDBG_ENTER(BHDR_Close) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	/* Disable and Destroy the HDMI Callbacks */
	for( i = 0; i < MAKE_INTR_ENUM(LAST); i++ )
	{
		/* all interrupts are now created; disable and destroy all on close */
		
		if (BINT_DisableCallback( hHDR->hCallback[i]) != BERR_SUCCESS) 
		{
			BDBG_ERR(("Error Disabling Callback %d", i)) ;
			rc = BERR_UNKNOWN ;
		}
		
		if  (BINT_DestroyCallback( hHDR->hCallback[i]) != BERR_SUCCESS)
		{
			BDBG_ERR(("Error Destroying Callback %d", i)) ;
			rc = BERR_UNKNOWN ;
		}
	}

	BTMR_DestroyTimer(hHDR->timerGeneric) ;

#if BHDR_CONFIG_HDCP_KEY_OTP_ROM 
#if ((HDMI_RX_GEN == 35230) || (HDMI_RX_GEN == 7422))
	BHDR_P_DestroyTimer(hHDR, 
		hHDR->timerOtpCrcCalc, BHDR_P_TIMER_eOtpCrcCalculation) ;
#endif
#endif
	
#if BHDR_CONFIG_RESET_FE_ON_SYMBOL_LOSS
	BHDR_P_DestroyTimer(hHDR, 
		hHDR->timerHdmiFeReset, BHDR_P_TIMER_eHdmiFeReset) ;
#endif
	
	BHDR_P_DestroyTimer(hHDR, 
		hHDR->timerDviHdmiModeChange, BHDR_P_TIMER_eDviHdmiModeChange) ;

	BKNI_DestroyEvent(hHDR->BHDR_Event_HdmiToDvi) ;
	BKNI_DestroyEvent(hHDR->BHDR_Event_DviToHdmi) ;
	BKNI_DestroyEvent(hHDR->BHDR_Event_AudioChannelStatusUpdate) ;
	BKNI_DestroyEvent(hHDR->BHDR_Event_VBlankEnd) ;

 	/* free memory associated with the HDMI handle */
	BKNI_Memset(hHDR, 0, sizeof(BHDR_P_Handle)) ;
	BDBG_OBJECT_DESTROY(hHDR, BHDR_P_Handle);
	BKNI_Free( (void *) hHDR) ;
	
	BDBG_LEAVE(BHDR_Close) ;
	return rc ;
}

/*******************************************************************************
BERR_Code BHDR_GetSettings
Summary: Get the current HDMI Rx settings 
*******************************************************************************/
BERR_Code BHDR_GetSettings(
	BHDR_Handle hHDR,            /* [in] HDMI Rx Handle */
	BHDR_Settings *pHdrSettings  /* [in] pointer to memory to hold current settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	BDBG_ENTER(BHDR_GetSettings) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BKNI_Memcpy(pHdrSettings, &hHDR->DeviceSettings, sizeof(BHDR_Settings)) ;


	BDBG_LEAVE(BHDR_GetSettings) ;
	return rc ;
}



/*******************************************************************************
BERR_Code BHDR_SetSettings
Summary: Get the current HDMI Rx settings 
*******************************************************************************/
BERR_Code BHDR_SetSettings(
	BHDR_Handle hHDR,            /* [in] HDMI Rx Handle */
	BHDR_Settings *pHdrSettings  /* [in] pointer to memory to hold current settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS;
	BDBG_ENTER(BHDR_SetSettings) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BKNI_Memcpy(&hHDR->DeviceSettings, pHdrSettings, sizeof(BHDR_Settings)) ;

#if BHDR_CONFIG_HW_PASSTHROUGH_SUPPORT
	BHDR_P_SetHdmiPassthroughMode(hHDR, pHdrSettings->bHdmiHardwarePassthrough) ;
#endif

	BDBG_LEAVE(BHDR_SetSettings) ;
	return rc ;
}



/***************************************************************************
BERR_Code BHDR_GetEventHandle
Summary: Get the event handle for checking HDMI events.
****************************************************************************/
BERR_Code BHDR_GetEventHandle(
   BHDR_Handle hHDR,           
   BHDR_EventType eEventType,
   BKNI_EventHandle *pBHDREvent	/* [out] event handle */
)
{
	BERR_Code      rc = BERR_SUCCESS;

	BDBG_ENTER(BHDR_GetEventHandle) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);


	switch (eEventType)
	{
	case BHDR_Event_HdmiToDvi :
		*pBHDREvent = hHDR->BHDR_Event_HdmiToDvi ;
		break ;
		
	case BHDR_Event_DviToHdmi :
		*pBHDREvent = hHDR->BHDR_Event_DviToHdmi ;
		break ;
	
	case BHDR_Event_AudioChannelStatusUpdate :
		*pBHDREvent = hHDR->BHDR_Event_AudioChannelStatusUpdate ;
		break ;
	
	case BHDR_Event_VBlankEnd :
		*pBHDREvent = hHDR->BHDR_Event_VBlankEnd ;
		break ;

	default :
		BDBG_ERR(("Invalid Event Type: %d", eEventType)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}


done:
	BDBG_LEAVE(BHDR_GetEventHandle) ;
	return rc ;
}



BERR_Code BHDR_WriteRxI2cRegisterSpace(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length)
{

	BERR_Code rc ;

	BDBG_ENTER(BHDR_WriteRxI2cRegisterSpace) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	BKNI_EnterCriticalSection() ;
		rc = BHDR_P_WriteRxI2cRegisterSpace_isr(hHDR, offset, pData, Length) ;
	BKNI_LeaveCriticalSection() ;

	BDBG_LEAVE(BHDR_WriteRxI2cRegisterSpace) ;

	return  rc ;
}


/***************************************************************************
Summary:
	Write Data to Rx I2c Space 
***************************************************************************/
static  BERR_Code BHDR_P_WriteRxI2cRegisterSpace_isr(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length)
{
	BREG_Handle hRegister ;
	uint32_t rc = BERR_SUCCESS ;
	uint32_t Register ;
	uint32_t ulOffset  ;
	
	uint8_t i, I2cBusy ;
	uint8_t msTimeout ;
	uint8_t addr = offset ;
	
	BDBG_ENTER(BHDR_P_WriteRxI2cRegisterSpace_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;
	
	msTimeout = 100 ;
	do
	{
		Register = BREG_Read32( hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset) ;
		I2cBusy = (uint8_t) BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_PEEK_POKE_BUSY) ;

		if (!I2cBusy)
			break ;
			
		BKNI_Delay(100) ;
			
	} while (I2cBusy && msTimeout--) ;
	
	if (I2cBusy)
	{
		rc = BERR_TIMEOUT ;
		goto done ;
	}

			
	/* Cleared for loading ; set the busy bit */
	Register |= BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_PEEK_POKE_BUSY) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
	
	for (i = 0 ; i < Length; i++)
	{
#if BHDR_CONFIG_DEBUG_I2C		
		BDBG_WRN(("I2C Offset %#x = 0x%02x", addr + i, *(pData+i))) ;
#endif		
		Register &= ~(
			  BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_WRITE_DATA) 
			| BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_OFFSET)) ;
		Register |=  
			  BCHP_FIELD_DATA(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_WRITE_DATA, *(pData+i)) 
			| BCHP_FIELD_DATA(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_OFFSET, addr + i) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
		
		/* write the byte */
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_WRITE_DATA_P, 1) ; 
		BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
		
		Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_WRITE_DATA_P) ; 
		BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
	}	
	
	/* Done loading; clear the busy bit */
	Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_PEEK_POKE_BUSY) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
	
done:	
	BDBG_LEAVE(BHDR_P_WriteRxI2cRegisterSpace_isr) ;
	return rc ;
}



/***************************************************************************
Summary:
	Read Data from Rx I2c Space 
***************************************************************************/
BERR_Code BHDR_ReadRxI2cRegisterSpace(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length)
{

	BREG_Handle hRegister ;
	uint32_t rc = BERR_SUCCESS ;
	uint32_t Register ;
	uint32_t ulOffset  ;
	
	uint8_t i, I2cBusy ;
	uint8_t msTimeout ;
	uint8_t addr = offset ;

	BDBG_ENTER(BHDR_ReadRxI2cRegisterSpace) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;
	
	msTimeout = 100;
	do
	{
		Register = BREG_Read32( hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset) ;
		I2cBusy = (uint8_t) BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_PEEK_POKE_BUSY) ;
			
		if (!I2cBusy)
			break ;
			
		BKNI_Delay(100) ;
			
	} while (I2cBusy && msTimeout--) ;
	
	if (I2cBusy)
	{
		rc = BERR_TIMEOUT ;
		goto done ;
	}

			
	/* Cleared for reading ; set the busy bit */
	Register |= BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_PEEK_POKE_BUSY) ;
	BREG_Write32(hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
	
	
	for (i = 0 ; i < Length; i++)
	{
		Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_OFFSET) ;
		Register |=  BCHP_FIELD_DATA(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_OFFSET, addr + i) ;
		BREG_Write32(hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
		
		/* read the byte */
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_WRITE_DATA_P, 1) ; 
		BREG_Write32(hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
		
		Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_WRITE_DATA_P) ; 
		BREG_Write32(hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
		
		Register = BREG_Read32( hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset) ;
		*(pData+i) = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_READ_DATA) ;
		
		BDBG_MSG(("read i2c offset %#x = %#x", addr, *(pData+i))) ;
	}	
	
	/* Done loading; clear the busy bit */
	Register &= ~BCHP_MASK(HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_PEEK_POKE_BUSY) ;
	BREG_Write32(hHDR->hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE + ulOffset, Register) ;
	
done:	
	BDBG_LEAVE(BHDR_ReadRxI2cRegisterSpace) ;
	return rc ;
}


BERR_Code BHDR_P_CalculateSampleRate_isr(BHDR_Handle hHDR)
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint32_t uiHdmiSysClkCnt ;
	uint32_t NewAudioSampleRateHz ;
	
	BDBG_ENTER(BHDR_P_CalculateSampleRate_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;	
	
	/* COUNTER 2 - Audio Packets  */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_EVENT_COUNTER_2_3_COUNT + ulOffset) ;
	hHDR->HdmiAudioPackets =
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_EVENT_COUNTER_2_3_COUNT, COUNT_2) ;

	uiHdmiSysClkCnt = BREG_Read32(hRegister, BCHP_HDMI_RX_0_SYS_CLK_CNTR + ulOffset) ;

	if ((!uiHdmiSysClkCnt) || (hHDR->HdmiAudioPackets < 250))
	{
#if BHDR_CONFIG_DEBUG_AUDIO_SR
		/* debug number of packets */
		BDBG_WRN(("Invalid Audio Info> SysClkCount: %d Packets: %d SR Calcs: %d",
			uiHdmiSysClkCnt, hHDR->HdmiAudioPackets, 
			hHDR->uiConsecutiveSampleRateCalculations)) ;
#endif

		/* restart the search for consistent, consecutive SR calculations */
		hHDR->uiConsecutiveSampleRateCalculations = 0 ;
		
		goto BHDR_P_CalculateSampleRate_isr_Done ;
	}

#if BHDR_CONFIG_DEBUG_AUDIO_SR
	/* debug number of packets */
	BDBG_WRN(("Audio Info> SysClkCount: %d Packets: %d Consecutive: %d",
		uiHdmiSysClkCnt, hHDR->HdmiAudioPackets, 
		hHDR->uiConsecutiveSampleRateCalculations)) ;
#endif 
		
	NewAudioSampleRateHz = (uint32_t) 
		   (BHDR_P_HDMI_SYS_CLK_RATE * ((uint64_t) ( hHDR->HdmiAudioPackets)) 
		/ ((uint64_t) (uiHdmiSysClkCnt))) ;

	if ((hHDR->AudioSampleRateHz > (NewAudioSampleRateHz + 1000))  /* NewRate + 1000Hz from previous */
	|| ((hHDR->AudioSampleRateHz + 1000) < NewAudioSampleRateHz)) /* NewRate - 1000Hz from previous */
	{
#if BHDR_CONFIG_DEBUG_AUDIO_SR
		/* for debugging ; 
		    inform only once of Audio SR change;
		    uiConsecutiveSampleRateCalculations will continually be set to 1 if 
		    Sample Rate calculation continuously fluctuates by more than 1000Hz 
		*/
		if (hHDR->uiConsecutiveSampleRateCalculations != 1)		
		{
			BDBG_WRN(("CH%d ******* Est Audio SR changed from %d Hz  to %d Hz; SysClkCount: %d ; Audio Packets: %d",
				hHDR->eCoreId, hHDR->AudioSampleRateHz, NewAudioSampleRateHz,
				uiHdmiSysClkCnt, hHDR->HdmiAudioPackets)) ;
		}
#endif 

		hHDR->AudioSampleRateHz = NewAudioSampleRateHz ;		
		
 		/* check for one time dither in values; so updates are not continuous */
		if (hHDR->uiConsecutiveSampleRateCalculations > BHDR_CONFIG_CONSECUTIVE_SR_CALCS)
		{
			hHDR->uiConsecutiveSampleRateCalculations-- ;
			goto BHDR_P_CalculateSampleRate_isr_Done ;
		}
		
		/* two consecutive changes in the Sample Rate... wait for stable value */
		hHDR->uiConsecutiveSampleRateCalculations = 0 ;
	}
	
		
	/* monitor for stable calculations */
	if (hHDR->uiConsecutiveSampleRateCalculations < BHDR_CONFIG_CONSECUTIVE_SR_CALCS)
	{
		hHDR->uiConsecutiveSampleRateCalculations++ ;

#if BHDR_CONFIG_DEBUG_AUDIO_SR
		BDBG_WRN(("CH%d SAMPLE RATE Calculation %d of %d", 
			hHDR->eCoreId, hHDR->uiConsecutiveSampleRateCalculations,
			BHDR_CONFIG_CONSECUTIVE_SR_CALCS)) ;
#endif
		
		goto BHDR_P_CalculateSampleRate_isr_Done ;
	}

	/* Sample Rate calculations are stable update the Sample Rate */
	if (hHDR->uiConsecutiveSampleRateCalculations == BHDR_CONFIG_CONSECUTIVE_SR_CALCS)
	{
		BHDR_P_ConfigureAudioMaiBus_isr(hHDR) ;
		
		/* prevent wrap around of SampleRateCalculations */
		hHDR->uiConsecutiveSampleRateCalculations = BHDR_CONFIG_CONSECUTIVE_SR_CALCS + 1 ;
	}

BHDR_P_CalculateSampleRate_isr_Done:	
	BDBG_LEAVE(BHDR_P_CalculateSampleRate_isr) ;
	return BERR_SUCCESS ;
}

void BHDR_P_ClearCoreAuthentication_isr(BHDR_Handle hHDR)
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset  ;

	BDBG_ENTER(BHDR_P_ClearCoreAuthentication_isr) ;	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	/* clear HDCP authenticated status */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_DEBUG + ulOffset) ;	
	
	Register &= ~ BCHP_MASK(HDMI_RX_0_HDCP_DEBUG, CLEAR_RX_AUTHENTICATED_P) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_DEBUG + ulOffset, Register) ;
	
	Register |= BCHP_MASK(HDMI_RX_0_HDCP_DEBUG, CLEAR_RX_AUTHENTICATED_P) ; 
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_DEBUG  + ulOffset, Register)  ;

	BDBG_LEAVE(BHDR_P_ClearCoreAuthentication_isr) ;	

}

/******************************************************************************
void BHDR_P_ProcessModeChange_isr
Summary: Proces changes between HDMI and DVI modes  
*******************************************************************************/
void BHDR_P_ProcessModeChange_isr(BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset  ;
	uint8_t HdmiMode ;
	uint16_t BStatus ;
	
	BDBG_ENTER(BHDR_P_ProcessModeChange_isr) ;	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CFG_1 + ulOffset) ;
	HdmiMode = BCHP_GET_FIELD_DATA(
		Register, HDMI_RX_0_DIGITAL_FRONT_END_CFG_1, HDMI_MODE) ;

	if (HdmiMode == hHDR->bHdmiMode)
		return ;

#if BHDR_CONFIG_DEBUG_INPUT_SIGNAL
	BDBG_WRN(("CH%d Hdmi Mode Changed from %d to %d", 
		hHDR->eCoreId, hHDR->bHdmiMode, HdmiMode)) ;
#endif
	
	/* process changes only */
	hHDR->bHdmiMode = HdmiMode ;
	
	if (hHDR->bHdmiMode == 0) /* DVI MODE */
	{
		BDBG_WRN(("CH%d ***DVI MODE***", hHDR->eCoreId)) ;

		/* mask packet stop interrupts... no need to be aware of packet stops  in DVI mode */
		BREG_Write32(hRegister, BCHP_HDMI_RX_0_MASK_PACKET_STOP_DETECT + ulOffset, 0xFFFFFFF) ;
		
		/* clear any enabled packet stop detection  */
		BREG_Write32(hRegister, BCHP_HDMI_RX_0_ENABLE_PACKET_STOP_DETECT + ulOffset, 0)  ;

		/* clear any HDCP authentication */
		BHDR_P_ClearCoreAuthentication_isr(hHDR) ;

		BHDR_ReadRxI2cRegisterSpace(hHDR,  BAVC_HDMI_HDCP_RX_BSTATUS, (uint8_t *) &BStatus, 2) ;
	 		BStatus = BStatus & 0xEFFF ;
#if BHDR_CONFIG_DEBUG_HDCP_KEY_LOADING
	 		BDBG_WRN(("HDCP BStatus Updated to Indicate DVI Mode:  %X ",  BStatus)) ;
#endif
		BHDR_P_WriteRxI2cRegisterSpace_isr(hHDR,  BAVC_HDMI_HDCP_RX_BSTATUS, (uint8_t *) &BStatus, 2) ;
							
		/* clear the AvMute Status, packet stop etc.  */
		BHDR_P_ClearHdmiMode_isr(hHDR) ;

 		BKNI_SetEvent(hHDR->BHDR_Event_HdmiToDvi) ;

		BHDR_P_ClearPacketMemory_isr(hHDR) ;

		/* clear all RAM Packets when back to HDMI mode; Packet Update interrupts will fire */
		{
			uint8_t PacketNum ;
			
			for (PacketNum = 0 ; PacketNum < BHDR_P_NUM_PACKETS; PacketNum++)
			{
				BHDR_P_ClearPacketRAM_isr(hHDR, PacketNum) ;
			}
		}
			

	}
	else /* HDMI MODE */
	{
		BDBG_WRN(("CH%d ***HDMI MODE***", hHDR->eCoreId)) ;
		BHDR_P_ResetHdcp_isr(hHDR) ;

		/* make sure MAI bus is configured to match current channel status information */		
		BHDR_P_ConfigureAudioMaiBus_isr(hHDR) ;
		
		BKNI_SetEvent(hHDR->BHDR_Event_DviToHdmi) ;
		
		BHDR_ReadRxI2cRegisterSpace(hHDR,  BAVC_HDMI_HDCP_RX_BSTATUS, (uint8_t *) &BStatus, 2) ;
	 		BStatus = BStatus | (uint16_t) BAVC_HDMI_HDCP_RxStatus_eHdmiMode ;
#if BHDR_CONFIG_DEBUG_HDCP_KEY_LOADING
	 		BDBG_WRN(("HDCP BStatus Updated to Indicate HDMI Mode:  %X ",  BStatus)) ;
#endif
		BHDR_P_WriteRxI2cRegisterSpace_isr(hHDR,  BAVC_HDMI_HDCP_RX_BSTATUS, (uint8_t *) &BStatus, 2) ;
	}
	BDBG_LEAVE(BHDR_P_ProcessModeChange_isr) ;	
}



/******************************************************************************
void BHDR_P_ProcessVerticalBlankEnd_isr
Summary: Proces Vertical Blank End events 
*******************************************************************************/
static void BHDR_P_ProcessVerticalBlankEnd_isr(BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset  ;
	
	uint8_t HdmiMode ;
	bool bRiChange = false ;
	uint16_t ucHdcpRi ;
	uint8_t EncryptedFrame, ClearFrame ;

#if BHDR_CONFIG_USE_ACRP_EVERY_N_VSYNCS
	uint8_t packetNum = 0;
#endif

	BDBG_ENTER(BHDR_P_ProcessVerticalBlankEnd_isr) ;	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	/* reset all counters at start of vertical blank end */
	BHDR_DEBUG_P_ResetAllEventCounters_isr(hHDR) ;		

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CFG_1 + ulOffset) ;
	HdmiMode = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_DIGITAL_FRONT_END_CFG_1, HDMI_MODE) ;

	if (HdmiMode != hHDR->bHdmiMode)
	{
#if BHDR_CONFIG_DELAY_MODE_CHANGE_MS
		/* set all mode changes in the ProcessModeChange function*/
		if (!hHDR->timerDviHdmiModeChange)
		{
#if BHDR_CONFIG_DEBUG_MODE_CHANGE			
			BDBG_WRN(("Start Delay Mode Change Timer for %d ms... %d", 
				BHDR_CONFIG_DELAY_MODE_CHANGE_MS, 	__LINE__)) ;
#endif
			BTMR_StartTimer_isr(hHDR->timerDviHdmiModeChange, 
				BHDR_P_MILLISECOND * BHDR_CONFIG_DELAY_MODE_CHANGE_MS) ;
		}
#else	
		BHDR_P_ProcessModeChange_isr(hHDR) ;
#endif
	}	
	
	hHDR->FrameCount++ ;  /* always increment the frame count */

	/* get status of frame: clear or encrypted */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_EVENT_COUNTER_0_1_COUNT + ulOffset) ;
	EncryptedFrame = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_EVENT_COUNTER_0_1_COUNT, COUNT_0) ;
	ClearFrame     = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_EVENT_COUNTER_0_1_COUNT, COUNT_1) ;

	if (ClearFrame)
	{
		/* reset Encrypted Frame Counter  whenever we have a clear frame */
		hHDR->FrameCountEncrypted = 0 ;
		hHDR->bHdcpRiUpdating = false ; /* HDCP Ri is not updating */
	}
	else if (EncryptedFrame)
	{
		hHDR->FrameCountEncrypted++ ;
		hHDR->bHdcpRiUpdating = true ;	 /* HDCP Ri values are updating */
	}
	else /* Cannot detect enc_en/enc_dis; assume clear */
	{
		hHDR->bHdcpRiUpdating = false ;
	}
	
	 /*
	 -- HDCP Ri value *may not update* if the transmitter chooses not to send encrypted frames... 
	 -- HDCP link can be authenticated, but not send encrypted frames; if  encrypted; Ri will change every 128 Vsyncs; 
	 */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_STATUS + ulOffset) ;
	ucHdcpRi = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_STATUS, RX_RI) ;
	if (ucHdcpRi != hHDR->HDCP_RxRi)
	{
		bRiChange = true ;
		hHDR->HDCP_RxRi = ucHdcpRi ;
	}
	
#if BHDR_CONFIG_DEBUG_HDCP_RI_UPDATES
	if ((hHDR->PreviousEncryptedFrame != EncryptedFrame)
	|| ( bRiChange))
	{
		BDBG_WRN(("CH%d Frame #: %8d  %7s  EncCount: %3d HDCP Ri %02x", 
			hHDR->eCoreId, hHDR->FrameCount, 
			EncryptedFrame ? "enc_en" : "enc_dis", hHDR->FrameCountEncrypted,
			hHDR->HDCP_RxRi)) ;
		hHDR->PreviousEncryptedFrame	= EncryptedFrame ;
	}
#endif		

#if BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT
	if ((hHDR->eUsage == BHDR_Usage_eMaster) 
	&& (bRiChange)
	&& (!hHDR->SymbolLossIntrCount))
	{
		BHDR_P_VerifyHdcpMasterSlave_isr(hHDR) ;
	}	
#endif

	if (hHDR->FrameCountEncrypted == 128)
		hHDR->FrameCountEncrypted = 0 ;
	
	if (hHDR->SymbolLossIntrCount)
	{
		uint8_t bSymbolLock ;
		
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_TST_CFG + ulOffset) ;
		bSymbolLock = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_DIGITAL_FRONT_END_TST_CFG, SYMBOL_LOCK) ;
		if (bSymbolLock)
		{
			BHDR_P_ResetHdcp_isr(hHDR);
			hHDR->SymbolLossIntrCount = 0  ;
		}
		
#if BHDR_CONFIG_DEBUG_INPUT_SIGNAL
		BDBG_WRN(("CH%d Symbol Lock Status: %d ; Symbol Loss detect count %d", 
			hHDR->eCoreId, bSymbolLock, hHDR->SymbolLossIntrCount)) ;
#endif

#if BHDR_CONFIG_SYMBOL_LOSS_SM
		if (bSymbolLock)
		{
			hHDR->eSymbolLossState = BHDR_P_SYMBOL_LOSS_SM_eLocked ;
			BDBG_WRN(("CH%d SYMBOL LOCKED", hHDR->eCoreId)) ;
		}

		/* report changes in state for debug purposes; eliminate repeating messages */
		if (hHDR->eSymbolLossState != hHDR->ePreviousSymbolLossState)
		{
			BDBG_ERR(("^^^CH%d SL SM (Proc VBlank): State changed from %s (%d) to %s (%d)", 
				hHDR->eCoreId,
				BHDR_P_SymbolLossSM_StateToStr(hHDR->ePreviousSymbolLossState),
				hHDR->ePreviousSymbolLossState,
				BHDR_P_SymbolLossSM_StateToStr(hHDR->eSymbolLossState),
				hHDR->eSymbolLossState)) ;
			
			hHDR->ePreviousSymbolLossState = hHDR->eSymbolLossState ;
		}
#endif
	}
	
	/* count down frames with packet errors - EXCESSIVE_ERROR interrupt resets */
	if (hHDR->ErrorFreePacketFrames)
		hHDR->ErrorFreePacketFrames --;
	else if (hHDR->bPacketErrors)  /* report packet errors stopped once */
	{
		BDBG_WRN(("EXCESSIVE Packet errors have stopped...\n\n\n")) ;
		hHDR->bPacketErrors = false ;

#if 0
		BHDR_P_ClearHdmiMode_isr(hHDR) ;
		hHDR->AvMute = false ;
#endif
	}

 
#if BHDR_CONFIG_CLEAR_AVMUTE_AFTER_N_S
	if  (hHDR->AvMute)
	{
		uint32_t timestamp ;
		BTMR_ReadTimer_isr(hHDR->timerGeneric, &timestamp) ;
		if ((timestamp - hHDR->AvMuteTimeStamp) > (BHDR_CONFIG_CLEAR_AVMUTE_AFTER_N_S * BHDR_P_SECOND)) 
		{
			BDBG_WRN(("CH%d AvMute timer has expired;  force Un-Mute... %d", 
				hHDR->eCoreId, hHDR->AvMute)) ; 

			Register = BREG_Read32_isr(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset) ;
			Register |= BCHP_MASK(HDMI_RX_0_HDCP_CONFIG, CLEAR_AV_MUTE) ;
			BREG_Write32_isr(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset, Register) ;

			Register &= ~ BCHP_MASK(HDMI_RX_0_HDCP_CONFIG, CLEAR_AV_MUTE) ;
			BREG_Write32_isr(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset, Register) ;
			
			hHDR->AvMute = false ;

			/* call the callback functions for AvMute (on/off) notification */
			if (hHDR->pfAvMuteNotifyCallback)
			{
				hHDR->pfAvMuteNotifyCallback(
					hHDR->pvAvMuteNotifyParm1, hHDR->iAvMuteNotifyParm2,  &hHDR->AvMute) ;
			}
		}
	} 
#endif	


	/* check for video format changes */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_UPDATE_STATUS	+ ulOffset) ;

	/* ignore unreliable saturated bits to determine video format change */ 
	Register &= ~ (
		BCHP_MASK(HDMI_RX_0_FORMAT_DET_UPDATE_STATUS, SATURATED_VBP)
		|BCHP_MASK(HDMI_RX_0_FORMAT_DET_UPDATE_STATUS,SATURATED_HSYNC_DELAY)) ;
	
	if (Register)  /* format has changed */
	{
#if BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY || BHDR_CONFIG_DEBUG_DETECTED_FORMAT_DETAIL
		BDBG_WRN(("CH%d HDMI Input Format Change Detected : 0x%08X", 
			hHDR->eCoreId, Register));
#endif
		BHDR_P_GetVideoFormat_isr(hHDR) ;	
		
		/* Update the pixel clock rate as well */
		BHDR_FE_P_GetPixelClockEstimate_isr(
			hHDR->hFeChannel, &hHDR->hFeChannel->EstimatedPixelClockRate) ;		
	}

#if BHDR_CONFIG_UPDATE_MAI_ON_VSYNC
	{
		BACM_SPDIF_ChannelStatus stChannelStatus ;
		uint8_t uiChannelStatusSampleFreq ;

		BHDR_P_GetChannelStatusBits_isr(hHDR, &stChannelStatus) ;

		uiChannelStatusSampleFreq = BCHP_GET_FIELD_DATA(hHDR->uiChannelStatus30,
			HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_3_0,
			CHANNEL_STATUS_27_24_AUDIO_FREQ) ;

		if (uiChannelStatusSampleFreq != hHDR->uiChannelStatusSampleFreq)
		{
#if BHDR_CONFIG_DEBUG_AUDIO_FORMAT
			BDBG_WRN(("Audio Sample Rate Change Detected %x", uiChannelStatusSampleFreq)) ;
#endif
			hHDR->uiChannelStatusSampleFreq = uiChannelStatusSampleFreq ;
			BHDR_P_ConfigureAudioMaiBus_isr(hHDR) ;				
		}
	}
#endif

	/* increment Vsync Counter */
	hHDR->VSyncCounter++ ;	
	
	if (!HdmiMode)
	{
		hHDR->bPacketErrors = false ;
		return ;
	}


#if BHDR_CONFIG_USE_ACRP_EVERY_N_VSYNCS
	/* Clear the Audio Clock Regeneration Packet  (ACRP) lock every N VSyncs */
	if (!(hHDR->VSyncCounter % BHDR_CONFIG_USE_ACRP_EVERY_N_VSYNCS))
    	{
		BHDR_P_GetPacketRamIndexByType(hHDR, 
			BAVC_HDMI_PacketType_eAudioClockRegeneration, &packetNum) ;
		BHDR_P_ClearPacketRAMLock_isr(hHDR, packetNum) ;
    	}

	BHDR_P_CalculateSampleRate_isr(hHDR) ;

#endif

#if BHDR_CONFIG_DEBUG_TIMER_S
	{
		uint32_t timestamp ;
		BTMR_ReadTimer_isr(hHDR->timerGeneric, &timestamp) ;
		if ((timestamp - hHDR->DebugTimeStamp) > (BHDR_CONFIG_DEBUG_TIMER_S  * BHDR_P_SECOND)) 
		{
			BHDR_P_DebugMonitorHdmiRx_isr (hHDR) ;
			hHDR->DebugTimeStamp = timestamp ;
		}
	}
#endif		
	BDBG_LEAVE(BHDR_P_ProcessVerticalBlankEnd_isr) ;	
	
 }


void BHDR_P_ResetHdcp_isr(BHDR_Handle hHDR) 
{
	uint32_t Register ;
	uint32_t ulOffset ;
	BREG_Handle hRegister ;

	BDBG_ENTER(BHDR_P_ResetHdcp_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

#if HDMI_RX_GEN == 35230 || HDMI_RX_GEN == 7422
#if ((BCHP_CHIP == 35230) || (BCHP_CHIP == 35125))
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_RESET_CONTROL + ulOffset) ;
	Register |= BCHP_MASK(HDMI_RX_0_RESET_CONTROL, RESET_HDCP) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_RESET_CONTROL + ulOffset, Register) ;

	Register &= ~ BCHP_MASK(HDMI_RX_0_RESET_CONTROL, RESET_HDCP) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_RESET_CONTROL + ulOffset, Register) ;
#else

	/* HDCP SW_INIT resets Bcaps, DFE SW_INIT resets BStatus */
	Register = BREG_Read32(hRegister, BCHP_DVP_HR_HDMI_RX_0_SW_INIT + ulOffset) ;

	Register |= BCHP_MASK(DVP_HR_HDMI_RX_0_SW_INIT, HDCP)
			| BCHP_MASK(DVP_HR_HDMI_RX_0_SW_INIT, DFE);
			
	BREG_Write32(hRegister, BCHP_DVP_HR_HDMI_RX_0_SW_INIT + ulOffset, Register) ;

	Register &= ~ (BCHP_MASK(DVP_HR_HDMI_RX_0_SW_INIT, HDCP)
				| BCHP_MASK(DVP_HR_HDMI_RX_0_SW_INIT, DFE));
	BREG_Write32(hRegister, BCHP_DVP_HR_HDMI_RX_0_SW_INIT + ulOffset, Register) ;
#endif

#else

	BSTD_UNUSED(hHDR) ;
	BSTD_UNUSED(Register) ;

#endif

	BDBG_LEAVE(BHDR_P_ResetHdcp_isr) ;
}


static void BHDR_P_ClearHdmiMode_isr(BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset   ;


	BDBG_ENTER(BHDR_P_ClearHdmiMode_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	/* as long as symbol loss occurs, force core back to DVI mode */		
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CTL + ulOffset) ;
	Register |= BCHP_MASK(HDMI_RX_0_DIGITAL_FRONT_END_CTL, CLEAR_HDMI) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CTL + ulOffset, Register) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_DIGITAL_FRONT_END_CTL, CLEAR_HDMI) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_DIGITAL_FRONT_END_CTL + ulOffset, Register) ;

	hHDR->bHdmiMode = 0 ;

	/* Clear hdcp register */
	BHDR_P_ResetHdcp_isr(hHDR);

	/* disable AvMute if it is currently enabled */
	if (hHDR->AvMute)
	{
		/* clear the AvMute Status if enabled */
		BDBG_WRN(("Clearing HDMI mode; callback to clear AvMute...")) ;
		hHDR->AvMute = 0 ;
		
		if (hHDR->pfAvMuteNotifyCallback)
		{
			hHDR->pfAvMuteNotifyCallback(
				hHDR->pvAvMuteNotifyParm1, hHDR->iAvMuteNotifyParm2,  &hHDR->AvMute) ;
		}
	}

	/* clear Deep Color mode flags */
	hHDR->bDeepColorMode = false ;
	hHDR->GeneralControlPacket.eColorDepth = BAVC_HDMI_GCP_ColorDepth_e24bpp ;
	
	/* disable local audio  */
	BHDR_P_EnableAudio_isr(hHDR, false) ;

	/* inform of format change so default to DVI/RGB  mode */
	if (hHDR->pfVideoFormatChangeCallback)
	{
		/* inform of DVI mode to force RGB colorspace */
		hHDR->AviInfoFrame.ePacketStatus = BAVC_HDMI_PacketStatus_eStopped ;
		hHDR->pfVideoFormatChangeCallback(hHDR->pvVideoFormatChangeParm1, 
			BAVC_HDMI_PacketType_eAviInfoFrame, &hHDR->AviInfoFrame) ;
	}

	/* if needed, inform of VSI change to 2D (default) mode  */
	if (hHDR->VSInfoFrame.eHdmiVideoFormat != BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone)
	{
		hHDR->VSInfoFrame.ePacketStatus = BAVC_HDMI_PacketStatus_eStopped ;
		hHDR->VSInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone ;

		if (hHDR->pfPacketChangeCallback)
		{
			hHDR->pfPacketChangeCallback(hHDR->pvPacketChangeParm1, 			
				BAVC_HDMI_PacketType_eVendorSpecificInfoframe, &hHDR->VSInfoFrame) ;
		}
	}
	BDBG_LEAVE(BHDR_P_ClearHdmiMode_isr) ;

}



static void BHDR_P_ConfigureDeepColor_isr(BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint8_t uiThreshold, uiPreviousThreshold  ;

	BAVC_HDMI_GcpData GcpData;	

	BDBG_ENTER(BHDR_P_ConfigureDeepColor_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;
	BHDR_GetGeneralControlPacketData(hHDR, &GcpData);

	/* shift the bit counter that tracks frames with Gcp Info */
	hHDR->uiColorDepthFrameCounter = (hHDR->uiColorDepthFrameCounter << 1) & 0x0FFFFFFF ;

	switch (GcpData.eColorDepth)
	{
	case BAVC_HDMI_GCP_ColorDepth_eDepthNotIndicated :
	case BAVC_HDMI_GCP_ColorDepth_e24bpp :
		break ;
		
	case BAVC_HDMI_GCP_ColorDepth_e30bpp :
	case BAVC_HDMI_GCP_ColorDepth_e36bpp :
	case BAVC_HDMI_GCP_ColorDepth_e48bpp :
		/* Color Depth Info found */
		hHDR->uiColorDepthFrameCounter++ ;
		break ;
		
	default :
		if (hHDR->GeneralControlPacket.eColorDepth != GcpData.eColorDepth)
		{
			BDBG_ERR(("CH%d Unknown Color Depth Mode: %#X", 
				hHDR->eCoreId, GcpData.eColorDepth)) ;
		}
	}


	if (hHDR->uiColorDepthFrameCounter)  /* DC received in last N Frames */
	{
		uiThreshold = 4 ;
#if BHDR_CONFIG_DEBUG_GCP_DC
		if (!hHDR->bDeepColorMode)
		{
			BDBG_WRN(("CH%d ENTER Deep Color Mode;  Last frames wth DC info (see set bits):  %#x",
				hHDR->eCoreId, hHDR->uiColorDepthFrameCounter)) ;	
		}
#endif
		hHDR->bDeepColorMode = true ;	
	}
	else /* no DC receieved in last N Frames */
	{
		if (hHDR->bDeepColorMode) /* if in DC mode, then clear  */
		{
			uiThreshold = 0 ;
			hHDR->bDeepColorMode = false ;	
#if BHDR_CONFIG_DEBUG_GCP_DC
			BDBG_WRN(("CH%d EXIT Deep Color Mode; %#x....", 
				hHDR->eCoreId, hHDR->uiColorDepthFrameCounter)) ;
#endif
		}
		else /* otherwise stay in DC mode */
		{
			uiThreshold = 4 ;					
		} 
	}

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset) ;
	uiPreviousThreshold = 
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDMI_13_FEATURES_CFG_1, GCP_CD_FRAME_COUNT_THRESHOLD) ;

	/* update changes only */
	if (uiPreviousThreshold != uiThreshold)
	{
		Register &= ~ BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, GCP_CD_FRAME_COUNT_THRESHOLD) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, GCP_CD_FRAME_COUNT_THRESHOLD, 
			uiThreshold) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset,  Register) ;		
	
#if BHDR_CONFIG_DEBUG_GCP_DC
		BDBG_WRN(("CH%d ColorDepth : %d; Threshold: %d",
			hHDR->eCoreId, GcpData.eColorDepth, uiThreshold)) ;	  
#endif
	}

#if BHDR_CONFIG_DC_PACKING_PHASE_ENABLE
	{
		uint8_t uiphase  ;
		/* get current Phase Packing */
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_STATUS_1 + ulOffset) ;
		uiphase = 
			BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDMI_13_FEATURES_STATUS_1, GCP_PACKING_PHASE) ;

		/* update phase setting */
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset) ;
		Register &= ~ BCHP_MASK(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, PACKING_PHASE_OFFSET) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_HDMI_13_FEATURES_CFG_1, PACKING_PHASE_OFFSET, uiphase) ;

		BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDMI_13_FEATURES_CFG_1 + ulOffset, Register) ;	
	}
#endif
	BDBG_LEAVE(BHDR_P_ConfigureDeepColor_isr) ;
}


/******************************************************************************
void BHDR_P_HandleInterrupt_isr
Summary: Handle interrupts from the HDMI core.
*******************************************************************************/
void BHDR_P_HandleInterrupt_isr(
	void *pParam1,						/* [in] Device handle */
	int parm2							/* [in] not used */
)
{
	BHDR_Handle hHDR ;
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint16_t BStatus = 0 ;

	BDBG_ENTER(BHDR_P_HandleInterrupt_isr) ;

	hHDR = (BHDR_Handle) pParam1 ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	/* make sure FrontEnd Channel has been attached */
	if (!hHDR->hFeChannel) 
		goto done  ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

#if BHDR_CONFIG_DVP_HR_TMR
	/* ignore interrupts until FE is taken out of reset */
	BTMR_ReadTimer_isr(hHDR->timerHdmiFeReset, &Register) ;
	if (Register)
	{
		/* clear Packet RAM so we can begin processing packets again */
		/* once the PLL Lock Timer has expired */
		BHDR_P_InitializePacketRAM_isr(hHDR) ;
		if (!hHDR->bWaitForTimer)  /* print warning only once */
		{
			BDBG_WRN(("CH%d Wait for FE TIMER expiration before processing HDR interrupts...",
				hHDR->eCoreId)) ;
		}
		hHDR->bWaitForTimer = true ;
		goto done ;
	}
	hHDR->bWaitForTimer = false ;
#endif
	
	switch (parm2)
	{
	case MAKE_INTR_ENUM(AKSV_UPDATE) :
		BDBG_WRN(("CH%d HDCP Authentication Request; Tx Aksv Received", 
						hHDR->eCoreId)) ;

		if (hHDR->bHdmiMode)
			BStatus = BAVC_HDMI_HDCP_RxStatus_eHdmiMode ;
		else
			BStatus = 0 ;
		BHDR_P_WriteRxI2cRegisterSpace_isr(hHDR,  BAVC_HDMI_HDCP_RX_BSTATUS, (uint8_t *) &BStatus, 2) ;

#if BHDR_CONFIG_DEBUG_HDCP_KEY_LOADING
		/* read back value */
		BHDR_ReadRxI2cRegisterSpace(hHDR,  BAVC_HDMI_HDCP_RX_BSTATUS, (uint8_t *) &BStatus, 2) ;
		BDBG_WRN(("BSTATUS After Ksv Update: %x", BStatus)) ;
#endif

		if (hHDR->pfHdcpStatusChangeCallback)
		{
			hHDR->pfHdcpStatusChangeCallback(hHDR->pvHdcpStatusChangeParm1, 0, NULL) ;
		}
		
		break ;

		    
	case MAKE_INTR_ENUM(REQUEST_KEYS) :
		{
		
#if BHDR_CONFIG_DEBUG_HDCP_VALUES
			BHDR_P_DebugHdcpValues_isr(hHDR) ;
#endif

#if BHDR_CONFIG_SYMBOL_LOSS_SM
			BHDR_P_SymbolLossSM_HdcpInit_isr( hHDR)  ;
#endif		

#if BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT
			/* check for R0 consistency when using master/slave */
			if (hHDR->eUsage == BHDR_Usage_eMaster)
			{
				BDBG_WRN(("Master/Slave config: Check for HDCP R0 consistency")) ;
				BHDR_P_VerifyHdcpMasterSlave_isr(hHDR) ;
			}
#endif			

			hHDR->stHdcpStatus.eAuthState = BHDR_HDCP_AuthState_eWaitForKeyloading ;
			if (hHDR->pfHdcpStatusChangeCallback)
			{
				hHDR->pfHdcpStatusChangeCallback(hHDR->pvHdcpStatusChangeParm1, 0, NULL) ;
			}
		}		
			
		break ;
		
#if BHDR_CONFIG_HDCP_REPEATER		 
	case MAKE_INTR_ENUM(REQUEST_KSVS) :   
		/* for HDCP Repeater Applications */
		BDBG_WRN(("CH%d HDCP Ksv FIFO Request; Repeater Setting %d", 
			hHDR->eCoreId, hHDR->stHdcpSettings.bRepeater)) ;
 		if (hHDR->stHdcpSettings.bRepeater)
 		{
			hHDR->stHdcpStatus.eAuthState = BHDR_HDCP_AuthState_eWaitForDownstreamKsvs ;
			if (hHDR->pfHdcpStatusChangeCallback)
			{
				hHDR->pfHdcpStatusChangeCallback(hHDR->pvHdcpStatusChangeParm1, 0, NULL) ;
			}
 		}
		break ;		
#endif		
	
#if BHDR_CONFIG_DEBUG_I2C
	case MAKE_INTR_ENUM(I2C_TRANSACTION_COMPLETE) :
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_I2C_PEEK_POKE) ;
		Register = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_I2C_PEEK_POKE, I2C_OFFSET ) ;
		BDBG_WRN(("CH%d I2C Transaction Complete at Offset: %d",  
			hHDR->eCoreId, Register)) ;
		break ;
#endif		

	case MAKE_INTR_ENUM(PJ_UPDATE) : 
		break ;
	
	case MAKE_INTR_ENUM(SYMBOL_LOSS) :   /* Int 09 */
		
		/* mute the audio path immediately */
		BHDR_P_EnableAudio_isr(hHDR, false) ; 
		
#if BHDR_CONFIG_REENABLE_INTRS
		BINT_EnableCallback_isr( hHDR->hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0)] ) ;
		BINT_EnableCallback_isr( hHDR->hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(CLOCK_STOP_0)] ) ;
#endif		
		if (hHDR->SymbolLossIntrCount < BHDR_P_MAX_SYMBOL_LOSS_COUNT)
		{
			hHDR->SymbolLossIntrCount++ ;
#if BHDR_CONFIG_DEBUG_INPUT_SIGNAL			
			BDBG_WRN(("CH%d SYMBOL_LOSS Interrupt #%d", 
				hHDR->eCoreId, hHDR->SymbolLossIntrCount )) ;
#endif
		}

#if BHDR_CONFIG_RESET_FE_ON_SYMBOL_LOSS

		BHDR_FE_P_ResetFrontEnd_isr(hHDR->hFeChannel) ;
		
		/* hold reset for 15 ms before releasing */
		BTMR_StartTimer_isr(hHDR->timerHdmiFeReset, BHDR_P_MILLISECOND * 15) ;

		BHDR_P_ClearHdmiMode_isr(hHDR) ;

#if BHDR_CONFIG_DELAY_MODE_CHANGE_MS

#if BHDR_CONFIG_DEBUG_MODE_CHANGE
		BDBG_WRN(("Start Delay Mode Change Timer for %d ms... %d", 
			BHDR_CONFIG_DELAY_MODE_CHANGE_MS, 	__LINE__)) ;
#endif
		BTMR_StartTimer_isr(hHDR->timerDviHdmiModeChange, 
			BHDR_P_MILLISECOND * BHDR_CONFIG_DELAY_MODE_CHANGE_MS) ;
#else 
		BHDR_P_ProcessModeChange_isr(hHDR) ;
#endif

#else

		BHDR_P_ClearHdmiMode_isr(hHDR) ;

#if BHDR_CONFIG_SYMBOL_LOSS_SM
		BHDR_P_SymbolLossSM_isr(hHDR) ;
#endif

#endif

		break ;
		
	case MAKE_INTR_ENUM(INVALID_DATA_ISLAND_LENGTH) :   /* Int 10 */
#if BHDR_CONFIG_DEBUG_STREAM_ERRORS
		BDBG_WRN(("CH%d HDMI Invalid Data Island Length!", hHDR->eCoreId )) ;
#endif

		/* fall through */
		
	case MAKE_INTR_ENUM(AUDIO_VALIDITY_BIT_UPDATE) :     /* Int 12 */
#if BHDR_CONFIG_DEBUG_STREAM_ERRORS
		BDBG_WRN(("CH%d HDMI Audio Validity Bit Update!", hHDR->eCoreId) ;
#endif

	break ;


	case MAKE_INTR_ENUM(CHANNEL_STATUS_UPDATE) :
#if BHDR_CONFIG_UPDATE_MAI_ON_VSYNC == 0 
		BHDR_P_GetChannelStatusBits_isr(hHDR, &hHDR->stChannelStatus) ;
#endif

#if BHDR_CONFIG_USE_ACRP_EVERY_N_VSYNCS == 0
		BHDR_P_ConfigureAudioMaiBus_isr(hHDR) ;
#endif
 		BKNI_SetEvent(hHDR->BHDR_Event_AudioChannelStatusUpdate) ;
		
		break ;
	
 			
	case MAKE_INTR_ENUM(RAM_PACKET_UPDATE) :    
		if (hHDR->bPacketErrors)
		{
			BHDR_P_InitializePacketRAM_isr(hHDR) ;
			goto done ;
		}
			
		BHDR_P_ProcessReceivedPackets_isr(hHDR) ;
		break ;		
		
	case MAKE_INTR_ENUM(RAM_PACKET_STOP) :    
		if (hHDR->bPacketErrors)
		{
			BHDR_P_InitializePacketRAM_isr(hHDR) ;
			goto done ;
		}
		BHDR_P_ProcessStoppedPackets_isr(hHDR) ;
		break ;

		
	case MAKE_INTR_ENUM(PACKET_SYNC_ERROR) :    
		BDBG_WRN(("CH%d Packet Error", hHDR->eCoreId)) ;
		break ;
		
	case MAKE_INTR_ENUM(LAYOUT_UPDATE) :
		break ;
		
	case MAKE_INTR_ENUM(AUDIO_TYPE_CHANGE) :
		break;
		
	/* cases to shut off audio */
	case MAKE_INTR_ENUM(SET_AV_MUTE) :  
		break ;
		
	case MAKE_INTR_ENUM(AV_MUTE_UPDATE) :  
	{
		uint8_t AvMute, AvMuteMode ;
		uint8_t AvMuteChange ;
		
		/*
		   AV_MUTE_MODE
  		   0- use AvMute from HDCP 
  		   1- use AvMute from Packet Processor 
		*/
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset); 
		AvMuteMode = (uint8_t) BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_CONFIG, AV_MUTE_MODE) ;

		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_MISC_STATUS + ulOffset) ;
		AvMute = (uint8_t) BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_0_MISC_STATUS, PKT_PROCESSOR_AV_MUTE) ;

#if BHDR_CONFIG_AVMUTE_AUDIO_IMMEDIATELY
		/* enable/disable audio accordingly */
		BHDR_P_EnableAudio_isr(hHDR, !AvMute) ;
#endif		
		BDBG_MSG(("CH%d AvMuteMode: %d AV MUTE Update from %d to %d", 
			hHDR->eCoreId, AvMuteMode, hHDR->AvMute, AvMute)) ;

		AvMuteChange =  (hHDR->AvMute != AvMute) ;
		hHDR->AvMute = AvMute ;


		
		/* call the callback functions for AvMute (on/off) notification */
		if (hHDR->pfAvMuteNotifyCallback)
		{
			hHDR->pfAvMuteNotifyCallback(hHDR->pvAvMuteNotifyParm1, hHDR->iAvMuteNotifyParm2,  &hHDR->AvMute) ;
		}
		else
		{
			BDBG_WRN(("CH%d No AvMute callback installed...",
				hHDR->eCoreId)) ;
		}

#if BHDR_CONFIG_CLEAR_AVMUTE_AFTER_N_S
		/* if AvMute has been activated start the 5s timer */		
		if (AvMuteChange && AvMute)
		{
			BTMR_ReadTimer_isr(hHDR->timerGeneric, &hHDR->AvMuteTimeStamp) ;
			BDBG_MSG(("CH%d START auto Clear_AVMUTE timer!", 
				hHDR->eCoreId));
		}
		else /* disable the timer since we are no longer muted */
		{
			BDBG_MSG(("CH%d STOP auto Clear_AVMUTE timer!",
				hHDR->eCoreId));
		}
#endif
 		break ;
	}

	case MAKE_INTR_ENUM(VSYNC_LEAD_EDGE):
		BDBG_ERR(("VSYNC LEAD EDGE interrupt should not be used")) ;
		break ;

		
	case MAKE_INTR_ENUM(VERTICAL_BLANK_END):
		BHDR_P_ConfigureDeepColor_isr(hHDR) ;
		BHDR_P_ProcessVerticalBlankEnd_isr(hHDR) ;

#if BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT
		if ((hHDR->eUsage == BHDR_Usage_eMaster) 
		&& (!hHDR->SymbolLossIntrCount))
		{
			BHDR_P_ConfigureDeepColor_isr(hHDR->hHDRSlave) ;
		}
#endif		
		break ;

	case MAKE_INTR_ENUM(DVI_TO_HDMI) :    
	case MAKE_INTR_ENUM(HDMI_TO_DVI) :    

#if BHDR_CONFIG_DELAY_MODE_CHANGE_MS

#if BHDR_CONFIG_DEBUG_MODE_CHANGE
		BDBG_WRN(("Start Delay Mode Change Timer for %d ms... %d", 
			BHDR_CONFIG_DELAY_MODE_CHANGE_MS, 	__LINE__)) ;
#endif
		BTMR_StartTimer_isr(hHDR->timerDviHdmiModeChange, 
			BHDR_P_MILLISECOND * BHDR_CONFIG_DELAY_MODE_CHANGE_MS) ;

#else	
		BHDR_P_ProcessModeChange_isr(hHDR) ;
#endif		
		break ;
		
		
	case MAKE_INTR_ENUM(EXCESSIVE_PACKET_ERRORS) :  
		if (!hHDR->bPacketErrors)
		{
			BDBG_WRN(("EXCESSIVE Packet errors detected...")) ;
		}
		
		hHDR->bPacketErrors = true ;

#if HDMI_RX_GEN == 35230
		BHDR_FE_P_ResetFeDataChannels_isr(hHDR->hFeChannel) ;
#endif
		if (hHDR->ErrorFreePacketFrames < BHDR_P_MAX_ERROR_FREE_PACKET_FRAMES)
			hHDR->ErrorFreePacketFrames = BHDR_P_MAX_ERROR_FREE_PACKET_FRAMES ;
		
		break ;
		
	case MAKE_INTR_ENUM(FORMAT_DETECT_COUNT_SATURATED) :  
#if BHDR_CONFIG_DEBUG_DETECTED_FORMAT_DETAIL
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_FORMAT_DET_UPDATE_STATUS + ulOffset) ;
		BDBG_WRN(("CH%d FORMAT DETECTION COUNT SATURATION %X", 
			hHDR->eCoreId, Register)) ;
#endif
		break ;

	case MAKE_INTR_ENUM(ERROR_INTERRUPT) :
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_ERROR_INTERRUPT_STATUS + ulOffset) ;
		BDBG_ERR(("CH%d ERROR_INTERRUPT: %d", hHDR->eCoreId, Register)) ;
		break ;
		
		
	default	:
		BDBG_ERR(("CH%d Unknown Interrupt ID:%d",  hHDR->eCoreId, parm2)) ;
	} ;

	/* L2 interrupts are reset automatically */
done:
	BDBG_LEAVE(BHDR_P_HandleInterrupt_isr) ;
	return ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_InitializePacketRAM(
   BHDR_Handle hHDR		
) 
{
	BREG_Handle hRegister  ;
	uint32_t ulOffset  ;
	
	uint32_t rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDR_InitializePacketRAM) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;

	BKNI_EnterCriticalSection() ;
	BHDR_P_InitializePacketRAM_isr(hHDR) ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_InitializePacketRAM) ;
	return rc ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetAviInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_AviInfoFrame *AviInfoFrame)
{
	BKNI_Memcpy(AviInfoFrame, &hHDR->AviInfoFrame, sizeof(BAVC_HDMI_AviInfoFrame)) ;
	return BERR_SUCCESS ;
}

/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetGeneralControlPacketData(BHDR_Handle hHDR,
	BAVC_HDMI_GcpData * Gcpdata)
{
	BKNI_Memcpy(Gcpdata, &hHDR->GeneralControlPacket, sizeof(BAVC_HDMI_GcpData)) ;
	return BERR_SUCCESS ;
}
	
/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetAudioClockRegenerationData(BHDR_Handle hHDR,
	BAVC_HDMI_AudioClockRegenerationPacket * AudioClockRegeneraionPacket)
{
	BKNI_Memcpy(AudioClockRegeneraionPacket, &hHDR->AudioClockRegenerationPacket, sizeof(BAVC_HDMI_AudioClockRegenerationPacket)) ;
	return BERR_SUCCESS ;
}	
	
/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetAudioInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_AudioInfoFrame *AudioInfoFrame)
{
	BKNI_Memcpy(AudioInfoFrame, &hHDR->AudioInfoFrame, sizeof(BAVC_HDMI_AudioInfoFrame)) ;
	return BERR_SUCCESS ;
}

/******************************************************************************
Summary:
**7*****************************************************************************/
BERR_Code BHDR_GetSPDInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_SPDInfoFrame *SPDInfoFrame)
{
	BKNI_Memcpy(SPDInfoFrame, &hHDR->SPDInfoFrame, sizeof(BAVC_HDMI_SPDInfoFrame)) ;
	return BERR_SUCCESS ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetVendorSpecificInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_VendorSpecificInfoFrame *VendorSpecificInfoFrame)
{
	BKNI_Memcpy(VendorSpecificInfoFrame, &hHDR->VSInfoFrame, sizeof(BAVC_HDMI_VendorSpecificInfoFrame)) ;
	return BERR_SUCCESS ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetISRCData(BHDR_Handle hHDR,
	BAVC_HDMI_ISRC *pISRC)
{
#if BHDR_CONFIG_ISRC_PACKET_SUPPORT 
	BKNI_Memcpy(pISRC, &hHDR->ISRC, sizeof(pISRC)) ;
	return BERR_SUCCESS ;
#else
	BSTD_UNUSED(hHDR) ;
	BSTD_UNUSED(pISRC) ;
	BDBG_WRN(("ISRC Support disabled...")) ;
	return BERR_NOT_SUPPORTED  ;
#endif
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetAudioContentProtection(BHDR_Handle hHDR,
	BAVC_HDMI_ACP *AudioContentProtection)
{
	BKNI_Memcpy(AudioContentProtection, &hHDR->AudioContentProtection, sizeof(BAVC_HDMI_ACP)) ;
	return BERR_SUCCESS ;
}


BERR_Code BHDR_GetRawPacketData(BHDR_Handle hHDR, 
	BAVC_HDMI_PacketType ePacketType,
	BAVC_HDMI_Packet *pPacketBytes)
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDR_GetRawPacketData) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	
	BKNI_EnterCriticalSection();
	switch (ePacketType)
	{
	case BAVC_HDMI_PacketType_eAviInfoFrame :
		BKNI_Memcpy(pPacketBytes, &hHDR->AviInfoFrame.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;
		
	case BAVC_HDMI_PacketType_eAudioInfoFrame :
		BKNI_Memcpy(pPacketBytes, &hHDR->AudioInfoFrame.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;
		
	case BAVC_HDMI_PacketType_eSpdInfoFrame :
		BKNI_Memcpy(pPacketBytes, &hHDR->SPDInfoFrame.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;
		
	case BAVC_HDMI_PacketType_eVendorSpecificInfoframe:
		BKNI_Memcpy(pPacketBytes, &hHDR->VSInfoFrame.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;
		
	case BAVC_HDMI_PacketType_eAudioContentProtection :
		BKNI_Memcpy(pPacketBytes, &hHDR->AudioContentProtection.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;

	case BAVC_HDMI_PacketType_eAudioClockRegeneration :
		BKNI_Memcpy(pPacketBytes, &hHDR->AudioClockRegenerationPacket.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;

	case BAVC_HDMI_PacketType_eGeneralControl :
		BKNI_Memcpy(pPacketBytes, &hHDR->GeneralControlPacket.stPacket, 
			sizeof(BAVC_HDMI_Packet)) ;
		break ;

	default :
		BDBG_ERR(("Raw Packet Copy not supported")) ;
		rc = BERR_NOT_SUPPORTED ;
	}
	BKNI_LeaveCriticalSection();		

	BDBG_LEAVE(BHDR_GetRawPacketData) ;
	return rc ;
}



/******************************************************************************
Summary:
*******************************************************************************/
static void BHDR_P_GetAudioSampleFreq_isr(BHDR_Handle hHDR,
	uint8_t *AudioSampleFreq)
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset  ;

	BDBG_ENTER(BHDR_P_GetAudioSampleFreq_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;
	
	
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_3_0 + ulOffset) ;
	*AudioSampleFreq = BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_3_0, CHANNEL_STATUS_27_24_AUDIO_FREQ) ;

	BDBG_LEAVE(BHDR_P_GetAudioSampleFreq_isr) ;
}


/******************************************************************************
Summary: install Packet Change Callback to notify of packet changes or stopped packets
*******************************************************************************/
BERR_Code BHDR_InstallPacketChangeCallback(
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code			rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_InstallPacketChangeCallback) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}	

	BKNI_EnterCriticalSection() ;
		hHDR->pfPacketChangeCallback = pfCallback_isr ;
		hHDR->pvPacketChangeParm1 = pvParm1 ;
		hHDR->iPacketChangeParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_InstallPacketChangeCallback);
	
	return rc ;
}



/******************************************************************************
Summary: Uninstall Packet Change Callback
*******************************************************************************/
BERR_Code BHDR_UnInstallPacketChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) /* [in] cb for format changes */
{
	BERR_Code rc = BERR_SUCCESS ;	
	BSTD_UNUSED(pfCallback_isr) ;

	BDBG_ENTER(BHDR_UnInstallPacketChangeCallback) ;
	
	BKNI_EnterCriticalSection() ;
		hHDR->pfPacketChangeCallback = (BHDR_CallbackFunc) NULL  ;
		hHDR->pvPacketChangeParm1 = NULL ;
		hHDR->iPacketChangeParm2 = 0 ;
	BKNI_LeaveCriticalSection();
	
	BDBG_LEAVE(BHDR_UnInstallPacketChangeCallback) ;
	return rc;
}



/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_InstallVideoFormatChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for format changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code			rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_InstallVideoFormatChangeCallback) ;

	/* Check if this is a valid function */
	if (( pfCallback_isr == NULL ) || (pvParm1 == NULL) )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}	
	
	BKNI_EnterCriticalSection() ;
		hHDR->pfVideoFormatChangeCallback = pfCallback_isr ;
		hHDR->pvVideoFormatChangeParm1 = pvParm1 ;
		hHDR->iVideoFormatChangeParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;

	BDBG_LEAVE(BHDR_InstallVideoFormatChangeCallback);
	
	return rc;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_UnInstallVideoFormatChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) /* [in] cb for format changes */
{
	BERR_Code rc = BERR_SUCCESS ;
	BSTD_UNUSED(pfCallback_isr) ;

	BDBG_ENTER(BHDR_UnInstallVideoFormatChangeCallback);
	
	BKNI_EnterCriticalSection() ;
		hHDR->pfVideoFormatChangeCallback = (BHDR_CallbackFunc) NULL  ;
		hHDR->pvVideoFormatChangeParm1 = NULL ;
		hHDR->iVideoFormatChangeParm2 = 0 ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_UnInstallVideoFormatChangeCallback);
	return rc;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_InstallAudioFormatChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for format changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code			rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_InstallAudioFormatChangeCallback) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}	
	
	BKNI_EnterCriticalSection() ;
		hHDR->pfAudioFormatChangeCallback = pfCallback_isr ;
		hHDR->pvAudioFormatChangeParm1 = pvParm1 ;
		hHDR->iAudioFormatChangeParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_InstallAudioFormatChangeCallback) ;
	
	return rc ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_UnInstallAudioFormatChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) /* [in] cb for format changes */
{
	BERR_Code rc = BERR_SUCCESS ;
	BSTD_UNUSED(pfCallback_isr) ;
	
	BDBG_ENTER(BHDR_UnInstallAudioFormatChangeCallback) ;

	BKNI_EnterCriticalSection() ;
		hHDR->pfAudioFormatChangeCallback = (BHDR_CallbackFunc) NULL  ;
		hHDR->pvAudioFormatChangeParm1 = NULL ;
		hHDR->iAudioFormatChangeParm2 = 0 ;
	BKNI_LeaveCriticalSection();
	
	BDBG_LEAVE(BHDR_UnInstallAudioFormatChangeCallback) ;
	return rc;
}




/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_InstallAvMuteNotifyCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for format changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code			rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_InstallAvMuteNotifyCallback) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}	
	

	BKNI_EnterCriticalSection() ;
#if BHDR_CONFIG_DEBUG_DISABLE_AVMUTE_CB
		if (pfCallback_isr)
		{
			BDBG_WRN(("CH%d AV MUTE NOTIFY CALLBACK NOT INSTALLED", hHDR->eCoreId)) ;
		}
		else
		{
			BDBG_WRN(("CH%d AV MUTE NOTIFY CALLBACK IS NULL", hHDR->eCoreId )) ;
		}
		hHDR->pfAvMuteNotifyCallback = NULL ;
#else
		hHDR->pfAvMuteNotifyCallback = pfCallback_isr;
#endif
		hHDR->pvAvMuteNotifyParm1 = pvParm1 ;
		hHDR->iAvMuteNotifyParm2 = iParm2;	
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_InstallAvMuteNotifyCallback);
	
	return rc ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_UnInstallAvMuteNotifyCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) /* [in] cb for Av Mute Notification */
{
	BERR_Code rc = BERR_SUCCESS ;
	BSTD_UNUSED(pfCallback_isr) ;
	
	BDBG_ENTER(BHDR_UnInstallAvMuteNotifyCallback);
	
	BKNI_EnterCriticalSection() ;
		hHDR->pfAvMuteNotifyCallback = (BHDR_CallbackFunc) NULL ;
		hHDR->pvAvMuteNotifyParm1 = NULL ;
		hHDR->iAvMuteNotifyParm2 = 0 ;	
	BKNI_LeaveCriticalSection() ;

	BDBG_LEAVE(BHDR_UnInstallAvMuteNotifyCallback);
	return rc;
}


/******************************************************************************
Summary: Install Callback used to Notify for Packet Errors
*******************************************************************************/
BERR_Code BHDR_InstallPacketErrorChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet error changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code			rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_InstallPacketErrorChangeCallback) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}
		
	BKNI_EnterCriticalSection() ;
		hHDR->pfPacketErrorCallback = pfCallback_isr;
		hHDR->pvPacketErrorParm1 = pvParm1 ;
		hHDR->iPacketErrorParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_InstallPacketErrorChangeCallback);
	
	return rc ;
}


/******************************************************************************
Summary: Uninstall Callback used to Notify for Packet Errors
*******************************************************************************/
BERR_Code BHDR_UnInstallPacketErrorChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) /* [in] cb for Packet Error change Notification */
{
	BERR_Code rc = BERR_SUCCESS ;
	BSTD_UNUSED(pfCallback_isr) ;

	BDBG_ENTER(BHDR_UnInstallPacketErrorChangeCallback);
	BKNI_EnterCriticalSection() ;
	
		hHDR->pfPacketErrorCallback = (BHDR_CallbackFunc) NULL ;
		hHDR->pvPacketErrorParm1 = NULL  ;
		hHDR->iPacketErrorParm2 = 0 ;

	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_UnInstallPacketErrorChangeCallback);
	
	return rc;
}


static BERR_Code BHDR_P_GetChannelStatusBits_isr(
	BHDR_Handle hHDR, BACM_SPDIF_ChannelStatus *stChannelStatus) 
{
	BREG_Handle hRegister  ;
	uint32_t ulOffset ;
	uint32_t Register ;
	uint8_t auiChannelStatus[8] ;
	bool change = false ;

	BDBG_ENTER(BHDR_P_GetChannelStatusBits_isr) ;	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;


	/* get the audio spdif channel bits */
	Register = BREG_Read32(hRegister, 
		BCHP_HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_3_0 + ulOffset) ;
	if (Register != hHDR->uiChannelStatus30)
	{
		change = true ;
		
		auiChannelStatus[0] = (uint8_t) (Register & 0x000000FF) ;
		auiChannelStatus[1] = (uint8_t) ((Register & 0x0000FF00) >>  8) ;
		auiChannelStatus[2] = (uint8_t) ((Register & 0x00FF0000) >> 16) ;
		auiChannelStatus[3] = (uint8_t) ((Register & 0xFF000000) >> 24) ;
		
		hHDR->uiChannelStatus30 = Register ;
	}
			
	Register = BREG_Read32(hRegister, 
		BCHP_HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_7_4 + ulOffset) ;
	if (Register != hHDR->uiChannelStatus74)
	{
		change = true ;
		
		auiChannelStatus[4] = (uint8_t) (Register & 0x000000FF) ;
		auiChannelStatus[5] = (uint8_t) ((Register & 0x0000FF00) >>  8) ;
		auiChannelStatus[6] = (uint8_t) ((Register & 0x00FF0000) >> 16) ;
		auiChannelStatus[7] = (uint8_t) ((Register & 0xFF000000) >> 24) ;

		hHDR->uiChannelStatus74 = Register ;
	}

	if (change)
	{
		/* initialze the Channel Status Structure; parse the data */
		BACM_SPDIF_InitChannelStatus(stChannelStatus) ;
		BACM_SPDIF_ParseChannelStatus(stChannelStatus, 8, auiChannelStatus) ;
	
#if BHDR_CONFIG_DEBUG_CHANNEL_STATUS
		BDBG_WRN(("ChannelStatus %08x - %08x",
 			hHDR->uiChannelStatus74, hHDR->uiChannelStatus30)) ;

		BDBG_WRN(("Copyright %d", stChannelStatus->bCopyrighted)) ;
		BDBG_WRN(("eApplication %d", stChannelStatus->eApplication)) ;
		BDBG_WRN(("eChannelNumber %d", stChannelStatus->eChannelNumber)) ;
		BDBG_WRN(("Mode %d", stChannelStatus->eChannelStatusMode)) ;
		BDBG_WRN(("Clock Accuracy %d", stChannelStatus->eClockAccuracy)) ;
		BDBG_WRN(("OSamplingFreq %d", stChannelStatus->eOriginalSamplingFrequency)) ;
		BDBG_WRN(("eSamplingFreq %d", stChannelStatus->eSamplingFrequency)) ;
		BDBG_WRN(("eSourceNumber %d", stChannelStatus->eSourceNumber)) ;
		BDBG_WRN(("eStreamType %d", stChannelStatus->eStreamType)) ;
		BDBG_WRN(("eWordLength %d", stChannelStatus->eWordLength)) ;
		BDBG_WRN(("uAdditionalFormatInfo %d", stChannelStatus->uAdditionalFormatInfo)) ;
		BDBG_WRN(("Category Code %d", stChannelStatus->uiCategoryCode)) ;
#endif
	}	
	BDBG_LEAVE(BHDR_P_GetChannelStatusBits_isr) ;	
	return BERR_SUCCESS ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetPixelClockEstimate(BHDR_Handle hHDR,
	uint32_t *EstimatedPixelClockRate 
)
{
	uint32_t rc = BERR_SUCCESS ;
	BDBG_ENTER(BHDR_GetPixelClockEstimate) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	/* report warning message once */
	if ((!hHDR->hFeChannel->bTxDeviceAttached) 
	&&  (hHDR->hFeChannel->bTxDeviceAttached != hHDR->hFeChannel->bPreviousTxDeviceAttached))
	{
		*EstimatedPixelClockRate = 0 ;
		BDBG_WRN(("CH%d Unable to Get Pixel Clock; No HDMI Input Source", 
			hHDR->eCoreId)) ;
		hHDR->hFeChannel->bPreviousTxDeviceAttached = false ;
	}
	else
	{
		* EstimatedPixelClockRate = hHDR->hFeChannel->EstimatedPixelClockRate ;
	}

	BDBG_LEAVE(BHDR_GetPixelClockEstimate) ;
	return rc ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetHdmiRxStatus(
	BHDR_Handle hHDR,           /* [in] HDMI Rx Handle */
	BHDR_Status *pHdmiStatus   /* struct Status Info */
)
{
	BREG_Handle hRegister  ;
	uint32_t ulOffset ;
 	
	BDBG_ENTER(BHDR_GetHdmiRxStatus) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;

	/* default/ initialize pHdmiStatus */
	BKNI_Memset(pHdmiStatus, 0, sizeof(*pHdmiStatus)) ;
	/* default symbol loss to true  */
	pHdmiStatus->bSymbolLoss = 	true ;
			
	if (!hHDR->hFeChannel)
	{
		BDBG_WRN(("No Front End attached to HDMI_RX_%d", hHDR->eCoreId)) ;
		goto BHDR_GetHdmiRxStatus_Done ;
	}

	/* report warning message once */
	if ((!hHDR->hFeChannel->bTxDeviceAttached) 
	&&  (hHDR->hFeChannel->bTxDeviceAttached != hHDR->hFeChannel->bPreviousTxDeviceAttached))
	{
		BDBG_WRN(("CH%d Unable to Get HDMI Status; NO DEVICE CONNECTED", 
			hHDR->eCoreId)) ;
		
		hHDR->hFeChannel->bPreviousTxDeviceAttached = false ;
		
		goto BHDR_GetHdmiRxStatus_Done ;
	}

 	pHdmiStatus->PllLocked = hHDR->hFeChannel->bPllLocked ;

	pHdmiStatus->bAvmute = hHDR->AvMute ;

	pHdmiStatus->HdmiMode	      = hHDR->bHdmiMode ;
	pHdmiStatus->DeviceAttached = hHDR->hFeChannel->bTxDeviceAttached ;

	pHdmiStatus->bPacketErrors   = hHDR->bPacketErrors ;

	pHdmiStatus->uiAudioPackets =  hHDR->HdmiAudioPackets ;
	pHdmiStatus->VSyncCounter = hHDR->VSyncCounter ;
	pHdmiStatus->bHdcpRiUpdating = hHDR->bHdcpRiUpdating ;

	pHdmiStatus->bSymbolLoss = 	(bool) (hHDR->SymbolLossIntrCount > 0) ;

	/* HDMI_TODO Use Format Detection Registers to indicate Stable Format for now use bPacketErrors */
	pHdmiStatus->bFormatStable = hHDR->bPacketErrors ;
	
	pHdmiStatus->PixelRepeat = hHDR->AviInfoFrame.PixelRepeat ;

	pHdmiStatus->uiAudioSampleRateHz = hHDR->AudioSampleRateHz ;

	/* Reported Channel Status cannot be used */
	BKNI_Memcpy(&pHdmiStatus->stChannelStatus, &hHDR->stChannelStatus, 
		sizeof(BACM_SPDIF_ChannelStatus)) ;
	pHdmiStatus->bValidChannelStatus = BHDR_CONFIG_HW_CHANNEL_STATUS ;

	/* set status to ok if PLL is locked and there is no symbol loss */
	/* otherwise the information is unreliable */
	if ((pHdmiStatus->PllLocked) && (!pHdmiStatus->bSymbolLoss))
	{
		pHdmiStatus->bValidStatus = true  ;
	}
	else if (hHDR->bPreviousStatus)  /* report bad status only once */
	{
#if BHDR_CONFIG_DEBUG_INPUT_SIGNAL		
		BDBG_WRN(("CH%d PLL Locked: %d; SymbolLossCount: %d", 
			hHDR->eCoreId,
			pHdmiStatus ->PllLocked, hHDR->SymbolLossIntrCount)) ;
#endif
	}
	
	hHDR->bPreviousStatus = pHdmiStatus->bValidStatus ;

BHDR_GetHdmiRxStatus_Done:
	BDBG_LEAVE(BHDR_GetHdmiRxStatus) ;
	return BERR_SUCCESS  ;
}


/******************************************************************************
Summary: Get the status of the HDMI Error Packet Status 
return : 1 Error exist on input; 0 No error on input
*******************************************************************************/
void BHDR_GetErrorPacketStatus(BHDR_Handle hHDR, bool *isError) 
{
	BDBG_ENTER(BHDR_GetErrorPacketStatus) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	*isError = false;
	if( hHDR->ErrorFreePacketFrames != 0)
	{
		*isError = true;
	}
	BDBG_LEAVE(BHDR_GetErrorPacketStatus) ;
}


static void BHDR_P_GetVideoFormat_isr(BHDR_Handle hHDR) 
{
	
	uint32_t Register ;
   	BREG_Handle hRegister  ;
	uint32_t ulOffset  ;

	BDBG_ENTER(BHDR_P_GetVideoFormat_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;
	
	Register = 0xFFFFFFFF;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_FORMAT_DET_UPDATE_CLEAR + ulOffset, Register) ;
	
	Register = 0;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_FORMAT_DET_UPDATE_CLEAR + ulOffset, Register) ;

	/* read format detection registers */

	/*read HDMI_RX_0_FORMAT_DET_1 - VID_FORMAT_1 */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_1  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uHsyncPolarity = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_1, UUT_HPOL) ;
	hHDR->stHdmiVideoFormat.uHsyncPulsePixels = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_1, UUT_HSP) ;
	hHDR->stHdmiVideoFormat.uHsyncBackporchPixels = BCHP_GET_FIELD_DATA (Register, HDMI_RX_0_FORMAT_DET_1, UUT_HBP);

	/*read HDMI_RX_0_FORMAT_DET_2 - VID_FORMAT_2 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_2  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uHorizontalActivePixels = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_2, UUT_HAP) ;
	hHDR->stHdmiVideoFormat.uHorizontalFrontporchPixels = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_2, UUT_HFP) ;
	
	/*read HDMI_RX_0_FORMAT_DET_3 - VID_FORMAT_3 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_3  + ulOffset) ;
	hHDR->stHdmiVideoFormat.uDelayedFieldVersion = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_3, UUT_FIELD_LATCHED) ;
	hHDR->stHdmiVideoFormat.uProgressive = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_3, PROGRESSIVE) ;
	hHDR->stHdmiVideoFormat.uInterlaced = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_3, INTERLACED) ;
	hHDR->stHdmiVideoFormat.uVsyncPolarity = BCHP_GET_FIELD_DATA (Register, HDMI_RX_0_FORMAT_DET_3, UUT_VPOL);
	hHDR->stHdmiVideoFormat.uField = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_3, UUT_FIELD) ;
	hHDR->stHdmiVideoFormat.uVerticalDe = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_3, UUT_VDE) ;
	hHDR->stHdmiVideoFormat.uVerticalFrontporchLinesField1 = BCHP_GET_FIELD_DATA (Register, HDMI_RX_0_FORMAT_DET_3, UUT_VFP0);
	hHDR->stHdmiVideoFormat.uVerticalSyncpulseLinesField1 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_3, UUT_VSP0) ;

	/*read HDMI_RX_0_FORMAT_DET_4 - VID_FORMAT_4 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_4  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uVerticalBackporchLinesField1 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_4, UUT_VBP0) ;
	hHDR->stHdmiVideoFormat.uVerticalSyncPulsePixelsField1 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_4, UUT_VSPO0) ;


	/*read HDMI_RX_0_FORMAT_DET_5 - VID_FORMAT_5 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_5  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uActiveLinesField2 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_5, UUT_VAL1) ;
	hHDR->stHdmiVideoFormat.uVerticalFrontPorchlinesField2 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_5, UUT_VFP1) ;

	/*read HDMI_RX_0_FORMAT_DET_6 - VID_FORMAT_6 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_6  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uVerticalBackporchLinesField2 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_6, UUT_VBP1) ;
	hHDR->stHdmiVideoFormat.uVerticalSyncPulsePixelsField2 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_6, UUT_VSPO1) ;

	/*read HDMI_RX_0_FORMAT_DET_7 - VID_FORMAT_6 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_7  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uVdeCopy = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_7, UUT_VDE_COPY) ;
	hHDR->stHdmiVideoFormat.uCurrentReveivedVideoLine = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_7, UUT_CURRENT_LINE_COUNT) ;

	/*read HDMI_RX_0_FORMAT_DET_8 - VID_FORMAT_6 Register */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_0_FORMAT_DET_8  + ulOffset) ;	
	hHDR->stHdmiVideoFormat.uActivelinesField1 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_8, UUT_VAL0) ;
	hHDR->stHdmiVideoFormat.uVerticalSyncPulseLinesField2 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_FORMAT_DET_8, UUT_VSP1) ;

#if BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY
	BDBG_WRN(("CH%d Detected Format: %d[%s] x %d/%d[%s]", 
		hHDR->eCoreId,
		hHDR->stHdmiVideoFormat.uHorizontalActivePixels,  
		hHDR->stHdmiVideoFormat.uHsyncPolarity ? "+": "-", 
		hHDR->stHdmiVideoFormat.uActivelinesField1, hHDR->stHdmiVideoFormat.uActiveLinesField2, 
		hHDR->stHdmiVideoFormat.uVsyncPolarity ? "+" : "-"));
#elif BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY_WITH_VSYNC_RATE
        {
        uint32_t VsyncRateWhole ;
        uint32_t VsyncRateFraction ;

        BHDR_FE_P_VsyncCountToRate(hHDR->hFeChannel, &VsyncRateWhole, &VsyncRateFraction) ;

	BDBG_WRN(("CH%d Detected Format: %d[%s] x %d/%d[%s]: %d.%02d Hz", 
		hHDR->eCoreId,
		hHDR->stHdmiVideoFormat.uHorizontalActivePixels,  
		hHDR->stHdmiVideoFormat.uHsyncPolarity ? "+": "-", 
		hHDR->stHdmiVideoFormat.uActivelinesField1, hHDR->stHdmiVideoFormat.uActiveLinesField2, 
                hHDR->stHdmiVideoFormat.uVsyncPolarity ? "+" : "-",
                VsyncRateWhole, VsyncRateFraction));
        }
#endif

#if BHDR_CONFIG_DEBUG_DETECTED_FORMAT_DETAIL
	BDBG_WRN(("\tHSync Pulse (pixels): %d; Back Porch: %d; Front Porch: %d",
		hHDR->stHdmiVideoFormat.uHsyncPulsePixels,
		hHDR->stHdmiVideoFormat.uHsyncBackporchPixels,
		hHDR->stHdmiVideoFormat.uHorizontalFrontporchPixels));

	BDBG_WRN(("\t                  Field1         Field2")) ;
	BDBG_WRN(("\tVert Front Porch:    %3d            %3d", 
		hHDR->stHdmiVideoFormat.uVerticalFrontporchLinesField1, 
		hHDR->stHdmiVideoFormat.uVerticalFrontPorchlinesField2)); /* The integer number of lines in the vertical front porch of field 1*/ 
		
	BDBG_WRN(("\tVert Back Porch:     %3d            %3d", 
		hHDR->stHdmiVideoFormat.uVerticalBackporchLinesField1, 
		hHDR->stHdmiVideoFormat.uVerticalBackporchLinesField2)) ; /* The number of lines in the vertical back porch of field 1*/
	BDBG_WRN(("\tVert Sync Pulse:     %3d            %3d", 
		hHDR->stHdmiVideoFormat.uVerticalSyncpulseLinesField1, 
		hHDR->stHdmiVideoFormat.uVerticalSyncPulseLinesField2)) ; /* The number of lines in the vertical sync pulse of field 1*/
	BDBG_WRN(("\tVert Sync Pulse:     %3d            %3d", 
		hHDR->stHdmiVideoFormat.uVerticalSyncPulsePixelsField1,  
		hHDR->stHdmiVideoFormat.uVerticalSyncPulsePixelsField2)) ; /* The number of pixels the leading edge of the vertical sync pulse is offset from the horizontal sync pulse in field 1*/
	BDBG_WRN(("\tDelayed Field Version: %d", 
		hHDR->stHdmiVideoFormat.uDelayedFieldVersion)); /* Delayed version of UUT_FIELD. Updates on the first active line following a vertical blank*/ 
	BDBG_WRN(("\tActive Field: %d", hHDR->stHdmiVideoFormat.uField)) ; /* This denotes the currently active field, It is updated every leading edge of the vertical sync*/
	BDBG_WRN(("\tData Enable: %d", hHDR->stHdmiVideoFormat.uVerticalDe)) ; /* vertical de*/
	BDBG_WRN(("\tCurrent Received Video Line :%d", 
		hHDR->stHdmiVideoFormat.uCurrentReveivedVideoLine)) ; /*The current line of video being recieved, Line 1 is the first line of active video, The count continues into the blanking period*/ 
#endif 
	
	BDBG_LEAVE(BHDR_P_GetVideoFormat_isr) ;
}


BERR_Code BHDR_GetHdmiRxDetectedVideoFormat(BHDR_Handle hHDR, BAVC_HDMI_VideoFormat *pHdmiFmt)
{
	BDBG_ENTER(BHDR_GetHdmiRxDetectedVideoFormat) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BKNI_Memset(pHdmiFmt, 0, sizeof(BAVC_HDMI_VideoFormat)) ;
	
	/* report warning message once */
	if ((!hHDR->hFeChannel->bTxDeviceAttached) 
	&&  (hHDR->hFeChannel->bTxDeviceAttached != hHDR->hFeChannel->bPreviousTxDeviceAttached))
	{
		BDBG_WRN(("CH%d Unable to Get Video Format; No HDMI Input Source", 
			hHDR->eCoreId)) ;
		
		hHDR->hFeChannel->bPreviousTxDeviceAttached = false ;
		
		goto BHDR_GetHdmiRxDetectedVideoFormat_Done ;
	}

	BKNI_Memcpy(pHdmiFmt, &hHDR->stHdmiVideoFormat, sizeof(BAVC_HDMI_VideoFormat)) ;

BHDR_GetHdmiRxDetectedVideoFormat_Done:
	BDBG_LEAVE(BHDR_GetHdmiRxDetectedVideoFormat) ;
	return BERR_SUCCESS  ;
}

#if BHDR_CONFIG_AVMUTE_AUDIO_IMMEDIATELY
/* Handle Audio - mute the audio here */
void BHDR_P_EnableAudio_isr(BHDR_Handle hHDR, bool enable) 
{
	
	/* Set  Audio Mute flag for the Audio Core */
	uint32_t Register ;
   	BREG_Handle hRegister  ;
	uint32_t ulOffset  ;

	BDBG_ENTER(BHDR_P_EnableAudio_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BDBG_MSG(("Turn Audio %s", enable ? "ON" : "OFF")) ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_AUDIO_MUTE_CFG + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_AUDIO_MUTE_CFG, ENABLE_AUDIO_MUTE) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_MUTE_CFG, ENABLE_AUDIO_MUTE, !enable) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_AUDIO_MUTE_CFG + ulOffset, Register) ;


	/* disable the MAI Bus */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CFG + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_CFG, DISABLE_MAI_BUS) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_CFG, DISABLE_MAI_BUS, !enable) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CFG + ulOffset, Register) ;
	BDBG_LEAVE(BHDR_P_EnableAudio_isr) ;
	
}
#endif


/******************************************************************************
BHDR_P_HotPlugConnect_isr
Summary: Hot Plug Connection Initialization
*******************************************************************************/
void BHDR_P_HotPlugConnect_isr(BHDR_Handle hHDR)
{

	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);

	BHDR_P_InitializePacketRAM_isr(hHDR) ;

	BHDR_DEBUG_P_ResetAllEventCounters_isr(hHDR) ;
}


/******************************************************************************
BHDR_P_HotPlugRemove_isr
Summary: Hot Plug DisConnect  Processing
*******************************************************************************/
void BHDR_P_HotPlugRemove_isr(BHDR_Handle hHDR)
{
	uint32_t Register ;
	uint32_t ulOffset ;
	BREG_Handle hRegister  ;

	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	/* get Register Handle and offset for Front End */
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;
		
	/* reset the HDMI MAI format register */
	Register = 
		  BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, SAMPLE_WIDTH, 16)
		|BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, AUDIO_FORMAT, 
			BHDR_P_MaiAudioFormat_PCM_2CH) 
		|BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, SAMPLE_RATE, 0) 
		|BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, STREAM_ID, 0) 
		|BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, MAI_VERSION, 0) ;
	BREG_Write32_isr(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT + ulOffset, Register) ;

	hHDR->uiConsecutiveSampleRateCalculations = 0 ;

	/* Handle Audio */
	/* settings for recalculation of SR up to BHDR_CONFIG_CONSECUTIVE_SR_CALCS times */
	hHDR->uiConsecutiveSampleRateCalculations = 0 ;
	
	hHDR->uiPreviousMaiSampleRate = 0 ;
	hHDR->uiPreviousMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_2CH ;
	hHDR->uiPreviousMaiSampleWidth = 16 ;

	/* clear out all packet RAM */
	BHDR_P_InitializePacketRAM_isr(hHDR) ;	

	/* clear all the packet data stored in the BHDR handle; new data will be stored on resumption */
	BHDR_P_ClearPacketMemory_isr(hHDR) ;

	BHDR_P_ClearHdmiMode_isr(hHDR) ;
	
	
}


BERR_Code BHDR_ConfigureAfterHotPlug(BHDR_Handle hHDR, bool bHotplugStatus)
{
	uint32_t rc = BERR_SUCCESS ;

	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	if (!hHDR->hFeChannel)
	{
		BDBG_WRN(("No Front End attached to HDMI Rx Core: %x", hHDR->ulOffset )) ;
		return BERR_NOT_INITIALIZED ;
	}

#if BHDR_CONFIG_DEBUG_INPUT_SIGNAL
	BDBG_WRN(("CH%d ConfigureAfterHotPlug: %d", 
		hHDR->eCoreId, bHotplugStatus)) ;
#endif

#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556))	
	{
		uint32_t Register ;
		uint8_t 
			uiLoopFilterHighThreshold, 
			uiLoopFilterLowThreshold ;
		Register = BREG_Read32(hHDR->hRegister, BCHP_HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2 
			+ hHDR->hFeChannel->ulOffset) ;

		uiLoopFilterHighThreshold = BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2, LOOP_FILTER_PD_HIGH_THRESHOLD) ;
		uiLoopFilterLowThreshold = BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2, LOOP_FILTER_PD_LOW_THRESHOLD) ;
		
		if ((uiLoopFilterHighThreshold != BHDR_CONFIG_LOOP_FILTER_PD_HIGH_THRESHOLD_WITH_SWITCH)
		|| (uiLoopFilterLowThreshold !=  BHDR_CONFIG_LOOP_FILTER_PD_LOW_THRESHOLD_WITH_SWITCH))
		{
			BDBG_ERR(("********************************************************************************")) ;
			BDBG_ERR(("LOOP FILTER SETTINGS are not consistent with external switch;  Please review Front End settings")) ;
			BDBG_ERR(("OVERRIDE PD HIGH Threshold to %x", BHDR_CONFIG_LOOP_FILTER_PD_HIGH_THRESHOLD_WITH_SWITCH)) ;
			BDBG_ERR(("OVERRIDE PD LOW  Threshold to %x", BHDR_CONFIG_LOOP_FILTER_PD_LOW_THRESHOLD_WITH_SWITCH)) ;
			BDBG_ERR(("********************************************************************************")) ;

			Register &= ~ (
				  BCHP_MASK(HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2, LOOP_FILTER_PD_HIGH_THRESHOLD)
				| BCHP_MASK(HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2, LOOP_FILTER_PD_LOW_THRESHOLD)) ;

			Register |= 
				BCHP_FIELD_DATA(HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2, 
				   LOOP_FILTER_PD_HIGH_THRESHOLD, BHDR_CONFIG_LOOP_FILTER_PD_HIGH_THRESHOLD_WITH_SWITCH)
				| BCHP_FIELD_DATA(HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2, 
				   LOOP_FILTER_PD_LOW_THRESHOLD, BHDR_CONFIG_LOOP_FILTER_PD_LOW_THRESHOLD_WITH_SWITCH) ;

			BREG_Write32(hHDR->hRegister, BCHP_HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_2 
				+  hHDR->hFeChannel->ulOffset, Register) ;
		}

		if (!hHDR->hFeChannel->settings.bExternalSwitch)
		{
			BDBG_ERR(("%************************************************************")) ;
			BDBG_ERR(("HDMI_RX_FE_%d external switch setting not enabled; must be enabled if switch is used",
				hHDR->hFeChannel->eChannel)) ;
			BDBG_ERR(("%************************************************************")) ;
		}
	}
#endif

	BKNI_EnterCriticalSection() ;

		BHDR_ConfigureAfterHotPlug_isr(hHDR, bHotplugStatus) ;
	
	BKNI_LeaveCriticalSection() ;

	return rc ;
}


BERR_Code BHDR_ConfigureAfterHotPlug_isr(BHDR_Handle hHDR, bool bHotplugStatus)
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

#if BHDR_CONFIG_DEBUG_INPUT_SIGNAL
	BDBG_WRN(("ConfigureAfterHotPlug Interrupt Status: HDMI_RX_%d %s", 
		hHDR->eCoreId, bHotplugStatus ? "ENABLED" : "DISABLED")) ;
#endif

#if BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT
	if (hHDR->eUsage == BHDR_Usage_eSlave)	
	{
		BDBG_WRN(("HDMI_RX_%d is used as slave; do not modify interrupt handling",
			hHDR->eCoreId)) ;
		return BERR_SUCCESS ;
	}
#endif

	/* reset Frame Counter */
	hHDR->FrameCount = 0 ;

	/* reset Symbol Loss Count */
	hHDR->SymbolLossIntrCount = 0 ;

	if (!hHDR->hFeChannel)
	{
		BDBG_WRN(("No Front End attached to HDMI Rx Core: %x", hHDR->ulOffset )) ;
		return BERR_NOT_INITIALIZED ;
	}

	/* remember the previous state */
	hHDR->hFeChannel->bPreviousTxDeviceAttached = 
		hHDR->hFeChannel->bTxDeviceAttached ;

	if (hHDR->hFeChannel->settings.bHpdDisconnected)
		hHDR->hFeChannel->bTxDeviceAttached = bHotplugStatus ;

	if (bHotplugStatus)
	{
		BHDR_P_HotPlugConnect_isr(hHDR);
		BHDR_FE_P_EnableInterrupts_isr(hHDR->hFeChannel, true) ;
		BHDR_P_EnableInterrupts_isr(hHDR, true) ;
	}
	else
	{
		BHDR_P_EnableInterrupts_isr(hHDR, false) ;
		BHDR_FE_P_EnableInterrupts_isr(hHDR->hFeChannel, false) ;		
		BHDR_P_HotPlugRemove_isr(hHDR);
	}
	
#if BHDR_CONFIG_MANUAL_PLL_RANGE_SETTING 
	BHDR_FE_P_AdjustFreqRange_isr(hHDR->hFeChannel) ;
#endif
	
#if BHDR_CONFIG_SYMBOL_LOSS_SM
	BHDR_P_SymbolLossSM_ConfigureAfterClockChange_isr(hHDR, bHotplugStatus) ;
#endif

	return rc ;
}


BERR_Code BHDR_P_ConfigureAudioMaiBus_isr(BHDR_Handle hHDR)
{
	uint8_t uiChannelStatusBits ;
	uint8_t uiSpeakerAllocation ;
	uint16_t uiMaiSampleRate ;
	uint16_t uiMaiAudioFormat ;
	uint16_t uiMaiSampleWidth = 0 ;
	
	uint8_t uiMaxWordLength  ;
	uint8_t uiWordLength  ;
	
	uint8_t i ;
	
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset   ;
	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle);
	ulOffset = hHDR->ulOffset ;
	hRegister = hHDR->hRegister ;

	uiChannelStatusBits = BHDR_P_CSBits_48kHz ; /* Default value */

#if BHDR_CONFIG_HW_CHANNEL_STATUS
	BHDR_P_GetAudioSampleFreq_isr(hHDR,  &uiChannelStatusBits) ;
	
	/* 
	Use the calculated Sample Rate (see ProcessVerticalBlankEnd) 
	to determine the corresponding channel status bit values
	*/
	i = 0 ;
	while (FsLookupTable[i].MinFreq)
	{
		if (uiChannelStatusBits == FsLookupTable[i].ChannelStatusBits)
		{
			hHDR->AudioSampleRateHz  = FsLookupTable[i].AudioSampleRateHz ;
			break ;
		}
		i++ ;
	}
	
#else

	/* 
	Use the calculated Sample Rate (see ProcessVerticalBlankEnd) 
	to determine the corresponding channel status bit values
	*/
	i = 0 ;
	while (FsLookupTable[i].MinFreq)
	{
		if ((hHDR->AudioSampleRateHz >= FsLookupTable[i].MinFreq)
		&& (hHDR->AudioSampleRateHz <= FsLookupTable[i].MaxFreq))
		{
			uiChannelStatusBits =  FsLookupTable[i].ChannelStatusBits ; 
			break ;
		}
		i++ ;
	}
#endif

	/* WARN if frequency not found in any range */
	if (!FsLookupTable[i].MinFreq)
	{
		BDBG_WRN(("CH%d Error getting Channel Status bits for %dHz (no Update)",
			hHDR->eCoreId, hHDR->AudioSampleRateHz)) ;

		/* disable audio since we can't determine a valid SR */
		BHDR_P_EnableAudio_isr(hHDR, false) ;
		
		return BERR_TIMEOUT;
	}
	
#if BHDR_CONFIG_DEBUG_AUDIO_FORMAT
	BDBG_WRN(("CH%d Configure MAI bus for Audio Sample Rate %dHz", 
		hHDR->eCoreId, hHDR->AudioSampleRateHz)) ;
#endif

	/* 
	Use the Channel Status Bits to look up the Sample Rate 
	to configure the MAI Audio Bus; default to Not Indicated 
	*/
	uiMaiSampleRate = BHDR_P_MaiSampleRate_NotIndicated ;
	i = 0 ;
	while (FsLookupTable[i].MinFreq)
	{
		if (uiChannelStatusBits == FsLookupTable[i].ChannelStatusBits)
		{
			uiMaiSampleRate = FsLookupTable[i].MaiSampleRate ;
			break ;
		}
		i++ ;
	}

	/* If we are receiving HBR packets, Indicate HBR */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_AUDIO_PACKET_TYPE + ulOffset) ;
	if ( BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_PACKET_TYPE, CURRENT_AUDIO_TYPE)
	  == BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_PACKET_TYPE, HBR_AUDIO_PACKET_TYPE) )
	{
		uiMaiAudioFormat = BHDR_P_MaiAudioFormat_HBR ;
	}
	else
	{
		/* get audio format from channel status */
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_3_0 + ulOffset) ;
		uiMaiAudioFormat = (Register & 0x2) 
			? BHDR_P_MaiAudioFormat_COMPRESSED
			: BHDR_P_MaiAudioFormat_PCM_2CH ;  
	}

#if BHDR_CONFIG_DEBUG_AUDIO_FORMAT
	BDBG_WRN(("Configure MAI Bus for Audio Packet Type (%d): %s", 
		hHDR->AudioPacketType,
		BAVC_HDMI_PacketTypeToStr(hHDR->AudioPacketType))) ;

	BDBG_WRN(("Audio Channel Status Bits [31..0] : %#x ; MAI Format: %#x  ", 
		Register, uiMaiAudioFormat)) ;

	BDBG_WRN(("CH%d Configure MAI bus for Audio Format: %s", 
		hHDR->eCoreId, uiMaiAudioFormat == BHDR_P_MaiAudioFormat_PCM_2CH 
			? "PCM" : "Compressed")) ;
#endif

#if BHDR_CONFIG_FORCE_MAI_BUS_FORMAT
	/*
	Workaround : always set audio format PCM to MAI_FORMAT register 
	to make sure Audio CHANNEL STATUS register works
	*/
	uiMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_2CH ;
#endif

	/* default speaker allocation is 0x03 */
	uiSpeakerAllocation = 0x03 ;

	if (uiMaiAudioFormat == BHDR_P_MaiAudioFormat_PCM_2CH)
	{
		switch(hHDR->AudioInfoFrame.ChannelCount)
		{
		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e2Channels :
			uiMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_2CH ;
			uiSpeakerAllocation = 0x03 ;
			break ;
		
		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e3Channels :
			uiMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_3CH ;
			uiSpeakerAllocation = 0x07 ;
			break ;
		
		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e6Channels :
			uiMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_6CH ;
			uiSpeakerAllocation = 0x3F ;
			break ;

		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e8Channels :
			uiMaiAudioFormat = BHDR_P_MaiAudioFormat_PCM_8CH ;
			uiSpeakerAllocation = 0xFF ;
			break ;
			
		case BAVC_HDMI_AudioInfoFrame_ChannelCount_eReferToStreamHeader :
			BDBG_MSG(("Audio Infoframe Channel Count set to Refer To Stream Header")) ;
			break ;

		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e4Channels :
		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e5Channels :			
		case BAVC_HDMI_AudioInfoFrame_ChannelCount_e7Channels :
		default :
			uiSpeakerAllocation = 0xFF ;
			/* do nothing */
			break ;
		}

		
		/* If audio is PCM, the sample width will be valid, according to IEC60958-3 Table 2 */
		Register = BREG_Read32_isr(hRegister, 
			BCHP_HDMI_RX_0_PACKET_PROCESSOR_CHN_STAT_7_4 + ulOffset) ;

		uiMaxWordLength = Register & 0x1 ;
		uiWordLength = (Register & 0xe) >> 1 ;

		if (uiWordLength > BHDR_AUDIO_SAMPLE_LENGTH_NUMBER)
		{
			BDBG_WRN(("CH%d Unknown/Unsupported audio sample width: %#x", 
				hHDR->eCoreId, uiWordLength)) ;
		}
		else
		{
			uiMaiSampleWidth = audioSampleLengthTable[uiMaxWordLength][uiWordLength - 1] ;
		}

		/* display format debug info */
#if BHDR_CONFIG_DEBUG_AUDIO_FORMAT
		BDBG_WRN((" ")) ;
		BDBG_WRN(("PCM Channel Count: %s", 
			BAVC_HDMI_AudioInfoFrame_ChannelCountToStr(
				hHDR->AudioInfoFrame.ChannelCount))) ;

		BDBG_WRN(("PCM Sample Width: %d", uiMaiSampleWidth)) ;
		BDBG_WRN(("Speaker Allocation: %x", uiSpeakerAllocation)) ;
		BDBG_WRN(("AudioInfoFrame Speaker Allocation: %x", uiSpeakerAllocation,  
			hHDR->AudioInfoFrame.SpeakerAllocation)) ;
#endif		
	}

 #if HDMI_RX_GEN == 7422
 	/* update channel mapping */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_AUDIO_CHANNEL_MAP + ulOffset) ;
		Register &= ~(
			  BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_7)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_6)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_5)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_4)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_3)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_2)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_1)
			| BCHP_MASK(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_0)) ;

		if ( uiMaiAudioFormat == BHDR_P_MaiAudioFormat_HBR )
		{
			/* Pass HBR straight through */
			Register |= 
			  BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_7, 7)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_6, 6)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_5, 5)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_4, 4)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_3, 3)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_2, 2)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_1, 1)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_0, 0) ;            
		}
		else
		{
			/* Reorder PCM to match the FMM channel layout */
			Register |= 
			  BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_7, 7)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_6, 6)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_5, 2)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_4, 3)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_3, 5)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_2, 4)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_1, 1)
			| BCHP_FIELD_DATA(HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_0, 0) ;
		}
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_AUDIO_CHANNEL_MAP + ulOffset, Register) ;			
		
#if BHDR_CONFIG_DEBUG_AUDIO_FORMAT
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_AUDIO_CHANNEL_MAP + ulOffset) ;
	BDBG_WRN(("Audio Channel Map: %x %x %x %x %x %x %x %x ", 
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_7),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_6),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_5),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_4),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_3),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_2),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_1),
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_AUDIO_CHANNEL_MAP, MAP_CHANNEL_0))) ;
#endif		
#endif

	/* configure the MAI bus */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT + ulOffset) ;
		Register &= ~ BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, SAMPLE_RATE) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, 
			SAMPLE_RATE, (uint32_t) uiMaiSampleRate) ;

		Register &= ~ BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, AUDIO_FORMAT) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, 
			AUDIO_FORMAT, (uint32_t) uiMaiAudioFormat ) ;

		Register &= ~ BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, SAMPLE_WIDTH) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT, 
			SAMPLE_WIDTH, (uint32_t) uiMaiSampleWidth ) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_MAI_FORMAT + ulOffset, Register) ;

	/* set the active speaker/channels	*/
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CFG + ulOffset) ;
		Register &= ~ BCHP_MASK(HDMI_RX_0_PACKET_PROCESSOR_CFG, SPEAKER_ACTIVE) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_0_PACKET_PROCESSOR_CFG, 
			SPEAKER_ACTIVE, (uint32_t) uiSpeakerAllocation) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PACKET_PROCESSOR_CFG + ulOffset, Register) ;

	/* every thing is valid enable audio */
	BHDR_P_EnableAudio_isr(hHDR, !hHDR->AvMute) ;

	return BERR_SUCCESS ;
}

 
void BHDR_P_EnableInterrupts_isr(BHDR_Handle hHDR, bool enable)
{
	BERR_Code rc ;
	const BHDR_P_InterruptCbTable *pInterrupts ;
	uint8_t i ;
	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	if (hHDR->eCoreId == BHDR_P_eHdrCoreId0)
		pInterrupts = BHDR_Interrupts0 ;
#if BHDR_HAS_MULTIPLE_PORTS 
	else if (hHDR->eCoreId == BHDR_P_eHdrCoreId1)
		pInterrupts = BHDR_Interrupts1 ;
#endif 	
	else
	{
		BDBG_ERR(("Unknown Core ID: %d", hHDR->eCoreId)) ;
		pInterrupts = BHDR_Interrupts0 ;
	}

	/* Register/enable interrupt callbacks */
	for (i = 0; i < MAKE_INTR_ENUM(LAST) ; i++) 
	{
		/* clear interrupt callback */
		rc = BINT_ClearCallback_isr( hHDR->hCallback[i]) ;
		if (rc) {BDBG_ASSERT(false) ;}


		/* skip interrupt if not enabled in table or slaved core */	
		if ((!pInterrupts[i].enable) /*|| (hHDR->eUsage == BHDR_Usage_eSlave)*/)
		{
			continue ;
		}

		if (enable)
			rc = BINT_EnableCallback_isr( hHDR->hCallback[i] ) ;
		else
			rc = BINT_DisableCallback_isr( hHDR->hCallback[i] ) ;
		if (rc) {BDBG_ASSERT(false) ;}
	}
}


static BERR_Code BHDR_P_CreateTimer(BHDR_Handle hHDR, BTMR_TimerHandle * timerHandle, uint8_t timerId) 
{
	BERR_Code rc ;
	BTMR_Settings timerSettings  ;

	BDBG_ENTER(BHDR_P_CreateTimer) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	BDBG_MSG(("Create BHDR_P_TIMER_eNNN ID %d", timerId)) ;

	/* create OTP Calculation Check expiration timer */
	BTMR_GetDefaultTimerSettings(&timerSettings) ;
		timerSettings.type =  BTMR_Type_eCountDown ;
		timerSettings.cb_isr = (BTMR_CallbackFunc) BHDR_P_TimerExpiration_isr ;
		timerSettings.pParm1 = hHDR ;
		timerSettings.parm2 = timerId ;
		timerSettings.exclusive = false ;		
	rc = BTMR_CreateTimer(hHDR->hTimerDevice, timerHandle, &timerSettings) ;

	if (rc != BERR_SUCCESS)
	{
		rc = BERR_TRACE(BERR_LEAKED_RESOURCE);
	}
	
	BDBG_LEAVE(BHDR_P_CreateTimer) ;
	return rc ;
}


static BERR_Code BHDR_P_DestroyTimer(BHDR_Handle hHDR, BTMR_TimerHandle timerHandle, uint8_t timerId) 
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDR_P_DestroyTimer) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BDBG_MSG(("Destroy BHDR_P_TIMER_eNNN ID %d", timerId)) ;
	rc = BTMR_DestroyTimer(timerHandle) ;
	
	BDBG_LEAVE(BHDR_P_DestroyTimer) ;
	return rc ;
}


static void BHDR_P_TimerExpiration_isr (BHDR_Handle hHDR, int parm2) 
{
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	switch (parm2)
	{
#if BHDR_CONFIG_HDCP_KEY_OTP_ROM
#if ((HDMI_RX_GEN == 35230) || (HDMI_RX_GEN == 7422))
	case BHDR_P_TIMER_eOtpCrcCalculation :
		BDBG_WRN(("HDCP Keyset CRC Calculation is done...")) ;
		BHDR_HDCP_P_VerifyOtpCalculation_isr(hHDR) ;
		/* notify Keyset Calculation Done */
		if (hHDR->pfHdcpStatusChangeCallback)
		{
			hHDR->pfHdcpStatusChangeCallback(hHDR->pvHdcpStatusChangeParm1, 0, NULL) ;
		}
		
		break ;
#endif
#endif

#if BHDR_CONFIG_RESET_FE_ON_SYMBOL_LOSS
	case BHDR_P_TIMER_eHdmiFeReset :
		/* FE has been held in reset; unreset */
		BDBG_WRN(("UnReset HDMI Frontend ...")) ;
		BHDR_FE_P_UnResetFrontEnd_isr(hHDR->hFeChannel) ;
		
		break ;
#endif

	case BHDR_P_TIMER_eDviHdmiModeChange :
#if BHDR_CONFIG_DEBUG_MODE_CHANGE
		BDBG_WRN(("DVI/HDMI Mode Change timer expired....")) ;
#endif
		BHDR_P_ProcessModeChange_isr(hHDR) ;
		
		break ;

	default :
		BDBG_ERR(("hHDR %p Timer %d not handled", hHDR, parm2)) ;
	} 
}


