/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt.c $
 * $brcm_Revision: Hydra_Software_Devel/52 $
 * $brcm_Date: 6/20/12 2:58p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core40nm/bxpt.c $
 * 
 * Hydra_Software_Devel/52   6/20/12 2:58p gmullen
 * SW7429-186: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7429-186/1   6/20/12 2:48p gmullen
 * SW7429-186: Added B0 support
 * 
 * Hydra_Software_Devel/51   5/7/12 3:52p gmullen
 * SW7425-2682: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2682/1   3/20/12 5:06p gmullen
 * SW7425-2682: Initialized playback parser IDs
 * 
 * Hydra_Software_Devel/50   5/7/12 8:57a gmullen
 * SW7346-502: Fixed compiler warning: hXpt not used
 * 
 * Hydra_Software_Devel/49   4/2/12 2:17p gmullen
 * SW7435-81: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-81/1   4/2/12 1:38p gmullen
 * SW7435-81: Shared DRAM for unused XC and RS buffers
 * 
 * Hydra_Software_Devel/48   3/8/12 3:41p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2497/2   3/8/12 3:19p gmullen
 * SW7425-2497: Added BXPT_HAS_PIPELINE_ERROR_REPORTING around code
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/47   2/3/12 12:01p jtna
 * SW7425-2248: add BXPT_P_ParserClients.ToMpodRs
 * 
 * Hydra_Software_Devel/46   12/20/11 1:29p gmullen
 * SW7358-176: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7358-176/1   12/20/11 1:05p gmullen
 * SW7358-175: Added per-client control of XC and RS buffer alloc
 * 
 * Hydra_Software_Devel/45   12/12/11 12:17p jtna
 * SW7425-1863: release SRAM resource on BXPT_Close()
 * 
 * Hydra_Software_Devel/44   12/2/11 12:12p jtna
 * SW7425-1863: refactor xpt standby power management
 * 
 * Hydra_Software_Devel/43   11/14/11 9:49a gmullen
 * SW7231-451: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7231-451/1   11/14/11 9:35a gmullen
 * SW7231-451: Removed writes to START_ADDR_ regs in RAVE, added BDBG_MSGs
 * 
 * Hydra_Software_Devel/42   11/8/11 4:02p gmullen
 * SW7346-502: Added virtual to physical parser mapping for
 * Get/SetParserConfig()
 * 
 * Hydra_Software_Devel/41   10/26/11 11:41a gmullen
 * SW7231-403: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7231-403/1   10/26/11 11:35a gmullen
 * SW7231-403: Original PID channel to buffer mapping interfered with
 * subsequent mappings
 * 
 * Hydra_Software_Devel/40   10/20/11 8:37a gmullen
 * SW7425-1491: Added check for older MTSIF cores.
 * 
 * Hydra_Software_Devel/39   10/19/11 8:06p gmullen
 * SW7425-1491: Removed API.
 * 
 * Hydra_Software_Devel/38   10/19/11 5:40p gmullen
 * SW7425-1491: Fixed define usage.
 * 
 * Hydra_Software_Devel/37   10/19/11 5:27p gmullen
 * SW7425-1491: Added BXPT_P_RxMode()
 * 
 * Hydra_Software_Devel/36   10/14/11 8:09a gmullen
 * SW7425-1487: Fixed compile error
 * 
 * Hydra_Software_Devel/SW7425-1487/1   10/13/11 3:57p gmullen
 * SW7425-1487: Fixed compiler error
 * 
 * Hydra_Software_Devel/35   10/12/11 5:26p gmullen
 * SW7425-1487: Inverted RxClockPol for certain parts
 * 
 * Hydra_Software_Devel/34   10/11/11 10:24a gmullen
 * SW7425-1470: Fixed coverity issues
 * 
 * Hydra_Software_Devel/33   10/5/11 4:25p gmullen
 * SW7346-502: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7346-502/1   10/5/11 4:03p gmullen
 * SW7346-502: Added AcceptAdapt00 support
 * 
 * Hydra_Software_Devel/32   9/30/11 3:10p gmullen
 * SW7425-1183: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1183/1   8/30/11 9:05a gmullen
 * SW7425-1183: Acquire the CLK216 resource when entering standby
 * 
 * Hydra_Software_Devel/31   9/30/11 10:49a gmullen
 * SW7552-109: Do not update CC enables when all-pass is in use; update
 * when CC is disabled
 * 
 * Hydra_Software_Devel/30   9/28/11 11:47a gmullen
 * SW7425-1323: Cached RAVE settings in DRAM to reduce overhead
 * 
 * Hydra_Software_Devel/29   9/12/11 4:36p gmullen
 * SW7344-193: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7344-193/1   9/7/11 4:34p gmullen
 * SW7344-193: Added tri-state logic to jitter API
 * 
 * Hydra_Software_Devel/28   9/2/11 2:41p gmullen
 * SW7231-319: Merge workaround to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/1   9/1/11 11:12a gmullen
 * SW7231-319: Temporary workaround for incorrect SRAM addresses
 * 
 * Hydra_Software_Devel/27   7/27/11 9:01a gmullen
 * SW7346-338: Fixed compiler warning about unused var
 * 
 * Hydra_Software_Devel/26   7/25/11 4:50p gmullen
 * SW7346-338: Added compile-time check to fix Coverity warning
 * 
 * Hydra_Software_Devel/25   7/13/11 2:49p jtna
 * SW7231-296: added missing non-sram register range. removed comments
 * 
 * Hydra_Software_Devel/24   7/12/11 3:52p jtna
 * SW7231-296: initial non-sram pm work
 * 
 * Hydra_Software_Devel/23   7/11/11 5:39p gmullen
 * SW7346-184: BXPT_ParserAllPassMode() must check if parser is
 * transitioning from all-pass to NON all-pass
 * 
 * Hydra_Software_Devel/22   7/1/11 2:45p gmullen
 * SW7231-186: Enable SRAM power management for 7231 only, for now
 * 
 * Hydra_Software_Devel/21   6/27/11 2:39p gmullen
 * SW7231-186: Merged power management code to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-186/1   6/13/11 5:59p gmullen
 * SW7231-186: Implemented SRAM save and restore for XPT standby/resume
 * 
 * Hydra_Software_Devel/20   5/12/11 4:59p gmullen
 * SW7231-128: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7231-128/2   5/12/11 1:24p gmohile
 * SW7231-128 : Fix MTSIF code
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/10/11 1:40p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/18   4/18/11 1:31p gmullen
 * SW7425-313: Added PID parser mapping to other 40nm chips
 * 
 * Hydra_Software_Devel/17   4/15/11 4:49p gmullen
 * SW7425-313: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-313/2   4/15/11 4:32p gmullen
 * SW7425-313: BXPT_GetParserMapping() did not initialize all parser
 * mappings
 * 
 * Hydra_Software_Devel/SW7425-313/1   4/13/11 2:05p gmullen
 * SW7425-313: Added parser remapping support
 * 
 * Hydra_Software_Devel/16   4/11/11 9:15a gmullen
 * SW7346-119: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7346-119/1   4/7/11 3:43p gmullen
 * SW7346-119: Disable CC checking when enabling all-pass mode. Restore
 * when exiting all-pass
 * 
 * Hydra_Software_Devel/15   3/28/11 12:05p jtna
 * SW7346-119: remove error from BXPT_ParserAllPassMode
 * 
 * Hydra_Software_Devel/14   3/23/11 5:12p gmullen
 * SW7346-124: Do not allocate XC buffers for unused remux blocks
 * 
 * Hydra_Software_Devel/13   3/22/11 1:42p gmullen
 * SW7425-223: Added code to correctly map STC broadcasts
 * 
 * Hydra_Software_Devel/12   2/17/11 4:22p gmullen
 * SW7422-266: Clear MSB of parser input select
 * 
 * Hydra_Software_Devel/11   2/16/11 5:21p gmullen
 * SW7422-266: MSB for parser data source had moved
 * 
 * Hydra_Software_Devel/10   2/15/11 8:36a gmullen
 * SWBLURAY-23703: Incorrect iterator in BXPT_GetDefaultSettings
 * 
 * Hydra_Software_Devel/9   1/28/11 9:34a gmullen
 * SW7425-15: Set Pid2Buff mapping to true by default
 * 
 * Hydra_Software_Devel/8   12/30/10 1:36p gmullen
 * SW7425-15: Only update pacing and pause maps for playback channels
 * involve in muxing
 * 
 * Hydra_Software_Devel/7   12/3/10 3:20p gmullen
 * SW7425-15: Allow setting the initial CC for generation
 * 
 * Hydra_Software_Devel/6   11/11/10 5:28p gmullen
 * SW7422-20: Access SUN_TOP_CTRL_SW_INIT_0_SET through BREG_Write32
 * 
 * Hydra_Software_Devel/5   11/9/10 2:57p gmullen
 * SW7422-20: Fixed register overwrite in BXPT_Open(). Changed RS and XC
 * defines; buffer init was not called
 * 
 * Hydra_Software_Devel/4   11/6/10 4:56p gmullen
 * SW7422-20: Use atomic access for reset registers
 * 
 * Hydra_Software_Devel/3   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/2   10/25/10 5:32p gmullen
 * SW7422-20: Changed _P_MAX defines to _NUM
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/25/10 9:11a gmullen
 * SW7425-15: Updated headers and added stub versions of C files
 * 
 * Hydra_Software_Devel/111   8/23/10 6:04p gmullen
 * SW7420-977: Updated SCB client mapping for usage 1 in RTS spreadsheet
 * 
 * Hydra_Software_Devel/110   8/23/10 2:03p gmullen
 * SW35230-2: Updated SCB assignments to match RTS spreadsheet
 * 
 * Hydra_Software_Devel/109   8/20/10 3:01p gmullen
 * SW35230-2: Fixed compilation error
 * 
 * Hydra_Software_Devel/108   8/20/10 10:22a gmullen
 * SW7403-924: Missed a register read in BXPT_P_SetPidChannelDestination()
 * 
 * Hydra_Software_Devel/107   8/17/10 4:52p gmullen
 * SW7403-924: Merged to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7403-924/3   8/17/10 4:42p gmullen
 * SW7403-924: Reduce MAX_PID_CHANNELS when PSUB PID channels are not
 * changed
 * 
 * Hydra_Software_Devel/SW7403-924/2   8/11/10 10:53a gmullen
 * SW7403-924: New method: change band_num in PSUB
 * 
 * Hydra_Software_Devel/SW7403-924/1   8/9/10 9:07a gmullen
 * SW7403-924: See the JIRA for the workaround notes
 * 
 * Hydra_Software_Devel/106   8/5/10 3:43p gmullen
 * SW7400-2889: Merged fix
 * 
 * Hydra_Software_Devel/SW7400-2889/1   8/5/10 3:38p gmullen
 * SW7400-2889: SetChannelEnable() should not disable a PID channel when
 * playback muxing requests it be enabled
 * 
 * Hydra_Software_Devel/105   7/8/10 2:14p gmullen
 * SW7630-81: Fixed typo in BCHP_VER_ for 7630B2
 * 
 * Hydra_Software_Devel/104   7/4/10 5:33p gmullen
 * SW7630-81: Added support for QUICK
 * 
 * Hydra_Software_Devel/103   6/5/10 10:43a gmullen
 * SW35230-2: Workaround for ICAM support and different endian strap
 * register
 * 
 * Hydra_Software_Devel/102   3/25/10 3:53p gmullen
 * SW35230-127: Updated code for requirements in this JIRA
 * 
 * Hydra_Software_Devel/101   2/17/10 11:47a gmullen
 * SW3548-2787: Use InputBandConfig->ParallelInputSel for arg checking
 * 
 * Hydra_Software_Devel/100   2/16/10 5:26p gmullen
 * SW3548-2697: SetInputBandConfig returned early for 3548/3556
 * 
 * Hydra_Software_Devel/99   2/12/10 4:49p gmullen
 * SW3548-2697: Return error if user tries to configure IB5 in serial mode
 * 
 * Hydra_Software_Devel/98   2/11/10 10:46a jgarrett
 * SW7408-71: Adding critical section around soft reset of XPT block
 * 
 * Hydra_Software_Devel/97   1/14/10 2:57p gmullen
 * SW3548-2697: Fixed compilation bug
 * 
 * Hydra_Software_Devel/96   1/4/10 8:42p gmullen
 * SW3548-2697: Return ParallelInputSel = true for parallel-only inputs.
 * 
 * Hydra_Software_Devel/95   12/29/09 1:07p piyushg
 * HW7408-91: Revert changes made in Rev 93. To be reviewed and
 * enable later.
 * 
 * Hydra_Software_Devel/94   11/30/09 10:06a gmullen
 * SW7405-3507: Fixed timebase check in BXPT_GetParserConfig()
 * 
 * Hydra_Software_Devel/93   11/18/09 9:27a gmullen
 * HW7408-91: Init PCR Offset block
 * 
 * Hydra_Software_Devel/92   11/17/09 3:07p gmullen
 * SW7408-9: Finished adding 7408 support to XPT
 * 
 * Hydra_Software_Devel/91   11/2/09 10:53a gmullen
 * SW3548-2589: Use reference counting when deciding to disable PID
 * channels
 * 
 * Hydra_Software_Devel/90   10/30/09 3:48p ronchan
 * SW7340-77: set 7340 input bands to parallel
 * 
 * Hydra_Software_Devel/89   9/23/09 2:26p gmullen
 * SW7405-2994: Merged fix to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/88   9/22/09 4:47p gmullen
 * SW7342-40: Changed bounds checking logic. Arg is 1-based, not 0-based.
 * 
 * Hydra_Software_Devel/87   9/3/09 1:37p gmullen
 * SW7125-7:Do not force parallel enabled on IB 2, 3, and 5. DS0 and DS1
 * are serial interfaced to XPT.
 * 
 * Hydra_Software_Devel/86   8/31/09 3:23p gmullen
 * SW7342-32:Added BXPT_SetPidChannelSwitchedISR()
 * 
 * Hydra_Software_Devel/85   8/19/09 12:14p piyushg
 * PR56771: Add support for 7342. Hence the previous usage
 * of constant PARSER_REG_STEPSIZE does not work for this chip.
 * Added new function to calculate offset.
 * 
 * Hydra_Software_Devel/84   8/11/09 10:38a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/83   8/5/09 4:51p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/82   7/31/09 3:26p piyushg
 * PR56771: Add support for 7342.
 * 
 * Hydra_Software_Devel/81   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/80   7/16/09 1:40p gmullen
 * PR56110: RAVE_CX_MODE skipped during BXPT_Open for 7550. Fixed.
 * 
 * Hydra_Software_Devel/79   7/14/09 10:20a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/78   6/18/09 11:11a gmullen
 * PR56110: Checked for mesg support.
 * 
 * Hydra_Software_Devel/77   6/18/09 8:07a gmullen
 * PR56110: Fixed build errors for other chips.
 * 
 * Hydra_Software_Devel/76   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/75   6/8/09 6:04p gmullen
 * PR55700: Added BXPT_IsDataPresent().
 * 
 * Hydra_Software_Devel/74   6/8/09 2:13p gmullen
 * PR55817: Duplicate PID check now considers only channels that are
 * enabled.
 * 
 * Hydra_Software_Devel/73   6/3/09 10:01a gmullen
 * PR55511: Removed check for PID channel destination. Interferes with
 * nexus playback mux support.
 * 
 * Hydra_Software_Devel/72   4/19/09 9:28a gmullen
 * PR53662: Added hook to determine if XPT can be powered down.
 * 
 * Hydra_Software_Devel/71   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/70   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/69   12/15/08 2:36p gmullen
 * PR48908: Removed power management code from XPT PI.
 * 
 * Hydra_Software_Devel/68   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/67   11/6/08 11:24a gmullen
 * PR46544: Added public PI call to incr/decr power reference count.
 * 
 * Hydra_Software_Devel/66   10/29/08 9:11a gmullen
 * PR46544: Fixed compile error.
 * 
 * Hydra_Software_Devel/65   10/28/08 6:18p gmullen
 * PR46544: Default power management to ON. Added compile-time support to
 * disable power management.
 * 
 * Hydra_Software_Devel/64   10/28/08 4:58p gmullen
 * PR46544: Added power management support, default to disabled for now.
 * 
 * Hydra_Software_Devel/63   9/22/08 2:53p gmullen
 * PR47074: Do not disable PID channel if its used by the PCR Offset
 * block.
 * 
 * Hydra_Software_Devel/62   9/4/08 10:06a vishk
 * PR 46315: Sample code for PID remapping
 * PR 46545: Reallocation of max bitrate for all pass parser band only
 * reallocates XC bw for rave. Rate increase for RMX1 and MSG still needs
 * to be added.
 * 
 * Hydra_Software_Devel/61   9/3/08 7:38p vishk
 * PR 46315: Sample code for PID remapping
 * PR 46545: Reallocation of max bitrate for all pass parser band only
 * reallocates XC bw for rave. Rate increase for RMX1 and MSG still needs
 * to be added.
 * 
 * Hydra_Software_Devel/60   8/18/08 3:58p gmullen
 * PR45362: Number of playback channels that support muxing should be
 * programmable.
 * 
 * Hydra_Software_Devel/59   8/15/08 10:48a gmullen
 * PR45362: Removed calls to BKNI_Enter/LeaveCriticalSectionISR()
 * 
 * Hydra_Software_Devel/58   8/8/08 5:47p gmullen
 * PR45362: Merged SanJose_CDI_Devel changes to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/4   8/4/08 11:11a bhsu
 * PR45362 : Fixed Kernel oops for AM21.
 * 
 * Hydra_Software_Devel/57   7/16/08 3:43p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/56   7/14/08 8:47p gmullen
 * PR44803: Used atomic access to set/clear xpt reset in sundry
 * 
 * Hydra_Software_Devel/56   7/14/08 8:44p gmullen
 * PR44803: Used atomic update to set/clear xpt reset.
 * 
 * Hydra_Software_Devel/55   6/20/08 1:53p gmullen
 * PR43907: Incorrect band rate passed to XC buff PI.
 * 
 * Hydra_Software_Devel/54   6/17/08 5:55p gmullen
 * PR43710: Fixed parser all-pass mode.
 * 
 * Hydra_Software_Devel/53   6/16/08 11:55a piyushg
 * PR39234: Change some comments and API names for setting up
 * multiple message buffers for same PID channel.
 * 
 * Hydra_Software_Devel/52   5/15/08 3:29p gmullen
 * PR42738: Added PSUB and MSG_WR to arbiter setup for 3563.
 * 
 * Hydra_Software_Devel/51   5/7/08 10:47a gmullen
 * PR42443: Removed call to change band rates when changing parser mode.
 * 
 * Hydra_Software_Devel/50   3/26/08 11:23a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/49   3/17/08 1:03p gmullen
 * PR40675: Added support to route all mesg filter data on R-pipe.
 * 
 * Hydra_Software_Devel/48   3/11/08 9:04a gmullen
 * PR40463: Update arbiter config on BXPT_Open()
 * 
 * Hydra_Software_Devel/47   3/7/08 4:54p gmullen
 * PR38618: Added 7118C0 support
 * 
 * Hydra_Software_Devel/46   3/6/08 3:47p piyushg
 * PR39234: Initial checkin for API's to support PID Duplication.
 * 
 * Hydra_Software_Devel/45   2/22/08 2:23p gmullen
 * PR39816: Enabled BXPT_P_ConfigArbiter() for 7400.
 * 
 * Hydra_Software_Devel/44   1/24/08 9:29a gmullen
 * PR38941: Cleared IB_ERROR_INPUT_EN in BXPT_SetInputBandConfig().
 * 
 * Hydra_Software_Devel/43   12/21/07 10:53a agin
 * PR37975: Set proper input bands for BCM7335 ) || ( BCHP_CHIP == 7336 .
 * 
 * Hydra_Software_Devel/42   12/19/07 9:01a gmullen
 * PR38274: Updated PI for final version of D0 RDB.
 * 
 * Hydra_Software_Devel/41   12/5/07 10:56p agin
 * PR37869: 7325 band 2 set to parallel only.
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/2   12/17/07 6:27p shuang
 * PR37867:Merge Jethead patch in order to support DirecTV AM21 project
 * which ATSC data will input through USB interface.
 * Merge Magnum Phase 7.0.
 * 
 * Hydra_Software_Devel/40   11/28/07 10:33a gmullen
 * PR36900: Added 7335 support.
 * 
 * Hydra_Software_Devel/39   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/38   9/28/07 11:16a gmullen
 * PR35427: Added BXPT_GetDefaultParserConfig() and
 * BXPT_GetDefaultInputBandConfig()
 * 
 * Hydra_Software_Devel/37   9/13/07 1:36p gmullen
 * PR32868: Changes to support bmsglib
 * 
 * Hydra_Software_Devel/36   8/1/07 4:50p gmullen
 * PR33710: Custom arbiter setup for 7440.
 * 
 * Hydra_Software_Devel/35   7/31/07 9:05a gmullen
 * PR33639: Disabled PID channel version checking
 * 
 * Hydra_Software_Devel/34   7/24/07 11:59a gmullen
 * PR33366: Do not write identical config to a PID channel register.
 * 
 * Hydra_Software_Devel/33   7/9/07 3:42p gmullen
 * PR27642: Updated for new RDB names of xmemif arbiters.
 * 
 * Hydra_Software_Devel/32   5/17/07 2:17p gmullen
 * PR30877: Added support for dedicated playback heap handle.
 * 
 * Hydra_Software_Devel/31   5/16/07 4:30p gmullen
 * PR30726: Fixed compiler warning.
 * 
 * Hydra_Software_Devel/30   5/16/07 9:38a gmullen
 * PR30726: BXPT_P_SetPidChannelDestination should increase XC rate only
 * when both R and G pipes are enabled.
 * 
 * Hydra_Software_Devel/29   2/14/07 1:09p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/28   1/16/07 5:39p gmullen
 * PR26109: Removed arbiter workaround.
 * 
 * Hydra_Software_Devel/27   1/9/07 4:38p gmullen
 * PR26109: Workaround for arbiter issue
 * 
 * Hydra_Software_Devel/26   11/22/06 4:58p gmullen
 * PR26109: Updated for 7403.
 * 
 * Hydra_Software_Devel/25   11/17/06 5:34p gmullen
 * PR26034: Fixed BXPT_Remux_AllocPidMap().
 * 
 * Hydra_Software_Devel/24   11/15/06 3:22p gmullen
 * PR15309: Added BXPT_P_EnablePidChannel.
 * 
 * Hydra_Software_Devel/23   11/2/06 4:31p gmullen
 * PR25402: Added support for secure heap to RS and XC buffer code.
 * 
 * Hydra_Software_Devel/22   11/2/06 8:40a gmullen
 * PR23189: Added 3563 support.
 * 
 * Hydra_Software_Devel/21   10/27/06 10:31a katrep
 * PR25024: Configure the arbiter for 7401. Remove after CFE supports
 * this.
 * 
 * Hydra_Software_Devel/20   9/26/06 1:33p katrep
 * PR20624: Fixed message buffer endian issue for 7400 big endian.
 * 
 * Hydra_Software_Devel/19   9/15/06 3:04p gmullen
 * PR15309: Fixed compiler warnings re unused variables.
 * 
 * Hydra_Software_Devel/18   9/1/06 10:20a gmullen
 * PR20624: Fixed mesg buffer endian setup issue.
 * 
 * Hydra_Software_Devel/17   8/23/06 4:26p gmullen
 * PR23861: Check for duplicated channels on enable.
 * 
 * Hydra_Software_Devel/16   8/17/06 6:01p katrep
 * PR23114: Added Support for 7440 chip
 * 
 * Hydra_Software_Devel/15   8/11/06 2:07p katrep
 * PR23349: RS buffer valid for ony input parser bands.
 * 
 * Hydra_Software_Devel/14   8/10/06 7:21p katrep
 * PR23349: Increase RS/XC buffer bandwidth during All pass record and
 * Increase XC buffer BW when Rave R and G pipe destinations are enabled.
 * 
 * Hydra_Software_Devel/13   8/10/06 2:51p gmullen
 * PR23189: Added defines for 3563.
 * 
 * Hydra_Software_Devel/12   8/7/06 2:02p gmullen
 * PR18998: BXPT_Disa
 blePidChannel() incorrectly checked for pid channel
 * diestination.
 * 
 * Hydra_Software_Devel/11   7/27/06 10:58a gmullen
 * PR15309: BXPT_ConfigurePidChannel() now checks for duplicated PID
 * channels
 * 
 * Hydra_Software_Devel/10   7/21/06 11:52a gmullen
 * PR18998: Default XC buffer for mesg block was 200kb, should be 8kb.
 * 
 * Hydra_Software_Devel/9   7/17/06 5:18p gmullen
 * PR20624: Added code for 7118.
 * 
 * Hydra_Software_Devel/8   7/13/06 11:37a katrep
 * PR20316: Added support to set/unset PSI setting per pid channel basis
 * as it's supported by new HW arch.PR20315:Also added support for 32
 * byte filters.Default filter size is 16 bytes.32 Bytes filters can be
 * enabled at compile time by -DBXPT_FILTER_32
 * 
 * Hydra_Software_Devel/7   7/12/06 5:54p gmullen
 * PR21042: XC and RS buffers can be customized to reduce total memory
 * needs.
 * 
 * Hydra_Software_Devel/6   6/22/06 12:59p katrep
 * PR20999: Diabled rewrite of the PID_TABLE_i reg if PID channel is
 * already enabled and request is to enable it again.This prevents
 * PID_VERSION from changing.
 * 
 * Hydra_Software_Devel/5   6/5/06 6:27p katrep
 * PR20631: disabled endian toggling as 7401 B0 xpt configures itself to
 * correct endianess.
 * 
 * Hydra_Software_Devel/4   6/5/06 5:54p gmullen
 * PR18998: Added support for control of ACCEPT_ADAPT_00 in parser regs.
 * 
 * Hydra_Software_Devel/3   2/17/06 3:31p gmullen
 * PR18998: Fixed memory overwrite in InitContext().
 * 
 * Hydra_Software_Devel/2   2/14/06 11:48a gmullen
 * PR18998: BXPT_Close() deallocated buffers for channels that didn't have
 * them. Fixed,
 * 
 * Hydra_Software_Devel/12   1/6/06 2:22p gmullen
 * PR18489: Added PID destination workaround for hardware PR 18919.
 * 
 * Hydra_Software_Devel/11   11/23/05 9:18a gmullen
 * PR15309: More updates.
 * 
 * Hydra_Software_Devel/10   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/9   9/1/05 10:36a gmullen
 * PR15309: Fixed compiler warnings on release builds.
 * 
 * Hydra_Software_Devel/8   8/31/05 10:58a gmullen
 * PR15309: Fixed bugs in PSI extraction and MPEG audio support.
 * 
 * Hydra_Software_Devel/7   8/26/05 2:00p gmullen
 * PR15309: Added DirecTV support, RAVE video support, cleaned up RS and
 * XC buffer code.
 * 
 * Hydra_Software_Devel/6   8/18/05 9:58a gmullen
 * PR15309: Added more DirecTV support.
 * 
 * Hydra_Software_Devel/5   8/12/05 8:57a gmullen
 * PR15309: Added PCR, PCR Offset, DirecTV, RAVE video, and ITB/CDB
 * endianess support. Tested same.
 * 
 * Hydra_Software_Devel/4   8/4/05 8:14a gmullen
 * PR15309: Added PCR support.
 * 
 * Hydra_Software_Devel/3   7/26/05 5:48p gmullen
 * PR15309: Fixed buffer size issue in bxpt_rave.c and added buffer
 * shutdown code to other blocks.
 * 
 * Hydra_Software_Devel/2   7/19/05 1:27p gmullen
 * PR15309: Fixed buffer alloc bugs.
 * 
 * Hydra_Software_Devel/1   7/15/05 9:03a gmullen
 * PR15309: Inita i
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bkni.h"
#include "bint.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#include "bxpt_pwr_mgmt_priv.h"
#endif

#if BXPT_HAS_FIXED_RSBUF_CONFIG
	#include "bxpt_rsbuf_priv.h"
#endif

#if BXPT_HAS_FIXED_XCBUF_CONFIG 
	#include "bxpt_xcbuf_priv.h"
#endif

#include "bmem.h" 
#include "bxpt_rave.h"
#include "bxpt_pcr_offset.h"

#include "bchp_sun_top_ctrl.h"
#include "bchp_xpt_fe.h"

#if BXPT_HAS_MESG_BUFFERS
    #include "bchp_xpt_msg.h"
#endif	

#if BXPT_HAS_PLAYBACK_PARSERS
	#include "bchp_xpt_pb0.h"
	#include "bchp_xpt_pb1.h"
	#define PB_PARSER_REG_STEPSIZE	( BCHP_XPT_PB1_CTRL1 - BCHP_XPT_PB0_CTRL1 )
#endif

#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_xmemif.h"
#include "bchp_xpt_rave.h"

#if BXPT_HAS_PACKETSUB
    #include "bchp_xpt_psub.h"
    #include "bxpt_packetsub.h"
	#define PACKET_SUB_REGISTER_STEP	( BCHP_XPT_PSUB_PSUB1_CTRL0 - BCHP_XPT_PSUB_PSUB0_CTRL0 )
#endif

#if BXPT_HAS_TSMUX
    #include "bxpt_tsmux.h"
#endif

#if BXPT_HAS_REMUX
#include "bxpt_remux.h"
#endif

/* Distance between Item X regs and Item X+1 */
#define PID_CHNL_STEPSIZE 		( 4 )
#define SPID_CHNL_STEPSIZE 		( 4 )

/* Locally defined interrupt IDs. The RDB structure makes it impossible to generate these automatically at the moment. */
#define BCHP_INT_ID_XPT_MSG_INTR_FLAG         BCHP_INT_ID_CREATE( BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31_INTR_FLAG_SHIFT )
#define BCHP_INT_ID_XPT_MSG_OVFL_INTR_FLAG    BCHP_INT_ID_CREATE( BCHP_XPT_MSG_BUF_OVFL_INTR_00_31, BCHP_XPT_MSG_BUF_OVFL_INTR_00_31_INTR_FLAG_SHIFT )
#define MESG_FILTER_ARRAY_ELEMENTS	( 512 )

#if BXPT_HAS_MTSIF
#define MTSIF_STEPSIZE ( BCHP_XPT_FE_MTSIF_RX1_CTRL1 - BCHP_XPT_FE_MTSIF_RX0_CTRL1 )
#endif

#if( BDBG_DEBUG_BUILD == 1 )
	BDBG_MODULE( xpt );
#endif

static void SetChannelEnable( BXPT_Handle hXpt, unsigned int PidChannelNum, bool EnableIt );
static bool PidChannelHasDestination( BXPT_Handle hXpt, unsigned int PidChannelNum );
static bool IsPidDuplicated( BXPT_Handle hXpt, unsigned int PidChannelNum );
static void BXPT_P_ConfigArbiter(BREG_Handle hReg);		 
void BXPT_Mesg_SetPid2Buff( BXPT_Handle hXpt, bool SetPid2Buff );
static unsigned int GetParserIndex( BXPT_Handle hXpt, unsigned ParserNum );

static void BXPT_P_FreeSharedXcRsBuffer( 
    BXPT_Handle hXpt 
    )
{
    void *Buffer;

    BMEM_ConvertOffsetToAddress( hXpt->hMemory, hXpt->SharedXcRsBufferOffset, ( void ** ) &Buffer );
    BMEM_Free( hXpt->hMemory, Buffer );
}

BERR_Code BXPT_P_AllocSharedXcRsBuffer(
    BXPT_Handle hXpt
    )
{       
    BERR_Code ExitCode = BERR_SUCCESS;
    uint32_t Offset = 0;
    void *Buffer;

    Buffer = BMEM_AllocAligned( hXpt->hMemory, BXPT_P_MINIMUM_BUF_SIZE, 8, 0 ); 
    if( !Buffer )
    {
        BDBG_ERR(( "Shared XC/RS buffer alloc failed!" ));
        ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
        goto Done;
    }
    BMEM_ConvertAddressToOffset( hXpt->hMemory, Buffer, &Offset );

    Done:
    hXpt->SharedXcRsBufferOffset = Offset;
    return ExitCode;
}

BERR_Code BXPT_GetDefaultSettings(
	BXPT_DefaultSettings *Defaults,	/* [out] Defaults to use during init.*/
	BCHP_Handle hChip			 	/* [in] Handle to used chip. */
	)
{
    unsigned ii;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( Defaults );
	BDBG_ASSERT( hChip );

	BSTD_UNUSED( hChip );
	BSTD_UNUSED( ii );		/* Might not be used if no XC and RS buffers are instantiated. */

	#if BXPT_HAS_FIXED_XCBUF_CONFIG || BXPT_HAS_FIXED_RSBUF_CONFIG
	#if BXPT_HAS_IB_PID_PARSERS
	for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ )
	{
#if defined(HUMAX_PLATFORM_BASE)
            Defaults->BandwidthConfig.MaxInputRate[ ii ] = 54000000;
#if defined(CONFIG_REMUX_EXT_CLOCK)			
            if (ii == 9)
            {
                Defaults->BandwidthConfig.MaxInputRate[ ii ] = 108000000;
            }
#endif
#else
            Defaults->BandwidthConfig.MaxInputRate[ ii ] = BXPT_P_INITIAL_BUF_BITRATE;
#endif
            Defaults->BandwidthConfig.IbParserClients[ ii ].ToRave = true;
            Defaults->BandwidthConfig.IbParserClients[ ii ].ToMsg = true;
            #if BXPT_HAS_REMUX
            {
                unsigned remuxNum;

                for( remuxNum = 0; remuxNum < BXPT_NUM_REMULTIPLEXORS; remuxNum++ )
                {
                    Defaults->BandwidthConfig.IbParserClients[ ii ].ToRmx[ remuxNum ] = true;
                }
            }
            #endif
            Defaults->BandwidthConfig.IbParserClients[ ii ].ToMpodRs = false;
		}
		#endif

		#if BXPT_HAS_PLAYBACK_PARSERS
		for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ )
		{
#if defined(HUMAX_PLATFORM_BASE)
			Defaults->BandwidthConfig.MaxPlaybackRate[ ii ] = 54000000;
#else
			Defaults->BandwidthConfig.MaxPlaybackRate[ ii ] = BXPT_P_INITIAL_BUF_BITRATE;
#endif
            Defaults->BandwidthConfig.PlaybackParserClients[ ii ].ToRave = true;
            Defaults->BandwidthConfig.PlaybackParserClients[ ii ].ToMsg = true;
            #if BXPT_HAS_REMUX
            {
                unsigned remuxNum;

                for( remuxNum = 0; remuxNum < BXPT_NUM_REMULTIPLEXORS; remuxNum++ )
                {
                    Defaults->BandwidthConfig.PlaybackParserClients[ ii ].ToRmx[ remuxNum ] = true;
                }
            }
            #endif
		}
		#endif
	#endif

    #if BXPT_HAS_REMUX
    for( ii = 0; ii < BXPT_NUM_REMULTIPLEXORS; ii++ )
    {
        Defaults->BandwidthConfig.RemuxUsed[ ii ] = true;
    }
    #endif

    Defaults->hRHeap = NULL;
    Defaults->hPbHeap = NULL;
    Defaults->MesgDataOnRPipe = false;

    #if BXPT_HAS_MTSIF
    for( ii = 0; ii < BXPT_NUM_MTSIF; ii++ ) 
    {
        Defaults->MtsifConfig[ ii ].RxInterfaceWidth = 8; /* 8 bits wide */
    	Defaults->MtsifConfig[ ii ].RxClockPolarity = 0;    /* Neg edge */
        Defaults->MtsifConfig[ ii ].Enable = false;
    }
    #endif

    return( ExitCode );
}

BERR_Code BXPT_Open(
	BXPT_Handle *hXpt, 			 			/* [out] Transport handle. */
	BCHP_Handle hChip,			 			/* [in] Handle to used chip. */
	BREG_Handle hRegister,		 			/* [in] Handle to access regiters. */
	BMEM_Handle hMemory,		 			/* [in] Handle to memory heap to use. */
	BINT_Handle hInt,		 				/* [in] Handle to interrupt interface to use. */
	const BXPT_DefaultSettings *Defaults	/* [in] Defaults to use during init.*/
	)
{
 	BXPT_Handle lhXpt;
 	unsigned i;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ENTER( BXPT_Open );

	/* Sanity check on the handles we've been given. */
	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hRegister );
	BDBG_ASSERT( hMemory );
	BDBG_ASSERT( hInt );
	BDBG_ASSERT( Defaults );

	/* Alloc memory from the system heap. */
	lhXpt = BKNI_Malloc( sizeof( BXPT_P_TransportData ) );
	if( lhXpt == NULL )
	{
		BDBG_ERR(( "BKNI_Malloc() failed!" ));
		ExitCode = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
		goto done;
	}
		
	BKNI_Memset( (void *) lhXpt, 0, sizeof( BXPT_P_TransportData ));			 
	lhXpt->hChip = hChip;
	lhXpt->hRegister = hRegister;
	lhXpt->hMemory = hMemory;
	lhXpt->hInt = hInt;

	lhXpt->hRHeap = Defaults->hRHeap;
	lhXpt->hPbHeap = Defaults->hPbHeap;
	lhXpt->MesgDataOnRPipe = Defaults->MesgDataOnRPipe;

#ifdef BCHP_PWR_RESOURCE_XPT
	/* turn on the 108M and 216M for BXPT_Open */
	BCHP_PWR_AcquireResource(hChip, BCHP_PWR_RESOURCE_XPT_108M);
	BCHP_PWR_AcquireResource(hChip, BCHP_PWR_RESOURCE_XPT_XMEMIF);

	#ifdef BCHP_PWR_RESOURCE_XPT_SRAM
	BCHP_PWR_AcquireResource(hChip, BCHP_PWR_RESOURCE_XPT_SRAM);
    
	lhXpt->SRAM_Backup = BKNI_Malloc(XPT_SRAM_LIST_TOTAL_SIZE);
	if (lhXpt->SRAM_Backup == NULL) 
	{
		ExitCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto done;
	}
	BDBG_MSG(("XPT SRAM storage uses %u bytes", XPT_SRAM_LIST_TOTAL_SIZE));

	lhXpt->Reg_Backup = BKNI_Malloc(XPT_REG_SAVE_LIST_TOTAL_SIZE);
	if (lhXpt->Reg_Backup == NULL) 
	{
		ExitCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto done;
	}
	BDBG_MSG(("XPT REG storage uses %u bytes", XPT_REG_SAVE_LIST_TOTAL_SIZE));
	#endif
#endif

	/* Reset the hardware. Make sure the SCB client mapping is correct. */
	BXPT_P_ResetTransport( hRegister );
	BXPT_P_ConfigArbiter( hRegister );

	/* Set the number of resources this transport has. */
	lhXpt->MaxPlaybacks = BXPT_NUM_PLAYBACKS; 
	lhXpt->MaxPidChannels = BXPT_NUM_PID_CHANNELS; 

#if BXPT_HAS_IB_PID_PARSERS
	lhXpt->MaxPidParsers = BXPT_NUM_PID_PARSERS; 
	lhXpt->MaxInputBands = BXPT_NUM_INPUT_BANDS; 
#endif

	lhXpt->MaxTpitPids = BXPT_NUM_TPIT_PIDS; 

#if BXPT_HAS_MESG_BUFFERS
	lhXpt->MaxFilterBanks = BXPT_NUM_FILTER_BANKS; 
	lhXpt->MaxFiltersPerBank = BXPT_NUM_FILTERS_PER_BANK; 
#endif

#if BXPT_HAS_PACKETSUB
	lhXpt->MaxPacketSubs = BXPT_NUM_PACKETSUBS;
#endif

#if BXPT_HAS_DPCRS
	lhXpt->MaxPcrs = BXPT_NUM_PCRS;
#endif

	lhXpt->MaxRaveContexts = BXPT_NUM_RAVE_CONTEXTS; 

#ifdef ENABLE_PLAYBACK_MUX
	/* By default, use one playback block for muxing. */
	lhXpt->NumPlaybackMuxes = 1; 
#endif
 
#if BXPT_HAS_MTSIF
    for( i = 0; i < BXPT_NUM_MTSIF; i++ ) 
    {
        uint32_t Reg, RegAddr;
        unsigned ClockPol;

        unsigned IfWidth = 3;   /* 8 bit wide */

        if( Defaults->MtsifConfig[ i ].RxInterfaceWidth != 8 ) 
        {   
            BDBG_ERR(( "Support for other than 8-bit wide MTSIF needs to be added" ));
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
            BKNI_Free( lhXpt );
            lhXpt = NULL;
            goto done;
        }
	
        if( Defaults->MtsifConfig[ i ].RxClockPolarity > 1 ) 
        {   
            BDBG_ERR(( "Invalid MTSIF RX clock edge. Valid values are 0 and 1" ));
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
            BKNI_Free( lhXpt );
            lhXpt = NULL;
            goto done; 
        }
       
    /* SW7425-1487: Invert the polarity on some parts. */
    #if (BCHP_CHIP == 7425 && BCHP_VER < BCHP_VER_B0) \
    || (BCHP_CHIP == 7346 && BCHP_VER < BCHP_VER_B0) \
    || (BCHP_CHIP == 7344 && BCHP_VER < BCHP_VER_B0) \
    || (BCHP_CHIP == 7422) \
    || (BCHP_CHIP == 7358 && BCHP_VER <= BCHP_VER_A1) \
    || (BCHP_CHIP == 7231 && BCHP_VER < BCHP_VER_B0) 
        ClockPol = Defaults->MtsifConfig[ i ].RxClockPolarity;  /* Some parts do *NOT* need the flop. */
    #else   
        ClockPol = Defaults->MtsifConfig[ i ].RxClockPolarity ? 0 : 1;  /* Some parts do. */
    #endif

        RegAddr = BCHP_XPT_FE_MTSIF_RX0_CTRL1 + i * MTSIF_STEPSIZE;
        Reg = BREG_Read32( hRegister, RegAddr );
        Reg &= ~(
             BCHP_MASK( XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_IF_WIDTH ) |
             BCHP_MASK( XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_CLOCK_POL_SEL ) |
             BCHP_MASK( XPT_FE_MTSIF_RX0_CTRL1, PARSER_ENABLE ) 
        );
        Reg |= ( 
            BCHP_FIELD_DATA( XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_IF_WIDTH, IfWidth ) |
            BCHP_FIELD_DATA( XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_CLOCK_POL_SEL, ClockPol ) |
            BCHP_FIELD_DATA( XPT_FE_MTSIF_RX0_CTRL1, PARSER_ENABLE, Defaults->MtsifConfig[ i ].Enable == true ? 1 : 0 ) 
        );
        BREG_Write32( hRegister, RegAddr, Reg );

        #ifdef BCHP_XPT_FE_MTSIF_RX0_SECRET_WORD
        BREG_Write32( hRegister, BCHP_XPT_FE_MTSIF_RX0_SECRET_WORD + (i * MTSIF_STEPSIZE), 0x829eecde ); 
        #endif
    }
#endif

 	/* Create and init the PID channel table. */
	for( i = 0; i < lhXpt->MaxPidChannels; i++ )
	{
		/* 
		** On some devices, not all PID channels have a message buffer.	HasMessageBuffer 
		** will be updated below, when we init the message buffer table
		*/
#ifdef ENABLE_PLAYBACK_MUX
    #if BXPT_SW7425_1323_WORKAROUND
		PidChannelTableEntry InitEntry = { false, false, false, 0, 0, false, false, false, 0, false, false };
    #else
		PidChannelTableEntry InitEntry = { false, false, false, 0, 0, false, false, false, 0 };
    #endif
#else /*ENABLE_PLAYBACK_MUX*/
    #if BXPT_SW7425_1323_WORKAROUND
		PidChannelTableEntry InitEntry = { false, false, false, 0, 0, false, 0, false, false  };
    #else
		PidChannelTableEntry InitEntry = { false, false, false, 0, 0, false, 0 };
    #endif
#endif /*ENABLE_PLAYBACK_MUX*/

        BXPT_PidChannel_CC_Config DefaultCcConfig = { true, true, false, 0 };

        lhXpt->PidChannelTable[ i ] = InitEntry;

		BREG_Write32( hRegister, BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( 4 * i ), 1 << 28 );
		BREG_Write32( hRegister, BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );

        lhXpt->CcConfigBeforeAllPass[ i ] = DefaultCcConfig;
        BXPT_SetPidChannel_CC_Config( lhXpt, i, &DefaultCcConfig );
	}

#if BXPT_HAS_MESG_BUFFERS
	/* Create and init the message buffer table. */
	for( i = 0; i <	BXPT_NUM_MESG_BUFFERS; i++ )
	{
		MessageBufferEntry InitEntry = { false, 0, 0 };
		lhXpt->MessageBufferTable[ i ] = InitEntry;	

   		lhXpt->MesgBufferIsInitialized[ i ] = false;

		lhXpt->PidChannelTable[ i ].HasMessageBuffer = true;
   		lhXpt->PidChannelTable[ i ].MessageBuffercount = 0;

		lhXpt->MesgIntrCallbacks[ i ].Callback = ( BINT_CallbackFunc ) NULL;
		lhXpt->OverflowIntrCallbacks[ i ].Callback = ( BINT_CallbackFunc ) NULL;

		BREG_Write32( hRegister, BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( 4 * i ), 0 );

        /* for normal legacy mode set false, set true to override settings */
        lhXpt->PidChannelParserConfigOverride[i] = false;
   	}

	/* Mark all the PSI filters as not allocated ( or unused ). */
	for( i = 0; i < lhXpt->MaxFilterBanks; i++ )
    {
 	    unsigned j;

		for( j = 0; j < lhXpt->MaxFiltersPerBank; j++ )
		{
			lhXpt->FilterTable[ i ][ j ].IsAllocated = false;
		}
    }

	/* Clear the mesg filter banks. */
	for( i = 0; i < MESG_FILTER_ARRAY_ELEMENTS; i++ )
	{
		BREG_Write32( hRegister, BCHP_XPT_MSG_GEN_FILT_COEF_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_GEN_FILT_MASK_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_GEN_FILT_EXCL_i_ARRAY_BASE + ( 4 * i ), 0 );
	}
#endif

#if BXPT_HAS_IB_PID_PARSERS && BXPT_HAS_MESG_BUFFERS
	/* 
	** All parser bands initially do NOT modify PSI messages in the DMA buffers. 
	** This is the hardware default.
	*/
	for( i = 0; i < BXPT_NUM_PID_PARSERS; i++ )
	{
		ParserConfig ParserInit = { false, false, false, BXPT_PsiMesgModModes_eNoMod };

		lhXpt->IbParserTable[ i ] = ParserInit;
        lhXpt->IsParserInAllPass[ i ] = false;
    }                      
#endif

	/* Each playback has a hard-wired PID parser. So init the table for those too. */
	for( i = 0; i < BXPT_NUM_PLAYBACKS; i++ )
	{
		ParserConfig ParserInit = { false, false, false, BXPT_PsiMesgModModes_eNoMod };

		lhXpt->PbParserTable[ i ] = ParserInit;
	}

	/* Init the RAVE structure in the xpt handle. */
	for( i = 0; i < BXPT_NUM_RAVE_CHANNELS; i++ )
	{
		lhXpt->RaveChannels[ i ].Allocated = false;
	}

	/* Init the RAVE structure in the xpt handle. */
	for( i = 0; i < BXPT_NUM_PCR_OFFSET_CHANNELS; i++ )
	{
		lhXpt->PcrOffsets[ i ].Handle = NULL;
		lhXpt->PcrOffsets[ i ].Allocated = false;
	}
                                    
#if BXPT_HAS_MESG_BUFFERS
  	/* Register our message and overflow interrupts */
	ExitCode = BERR_TRACE( BINT_CreateCallback( &lhXpt->hMsgCb, lhXpt->hInt, BCHP_INT_ID_XPT_MSG_INTR_FLAG, (BINT_CallbackFunc) BXPT_P_Interrupt_MsgVector_isr, lhXpt, 0 ));
	if( ExitCode != BERR_SUCCESS )
	{
		BDBG_ERR(("Unabled to create XPT Message Interrupt Callback!"));
		goto done;
	}
	
	ExitCode = BERR_TRACE( BINT_CreateCallback( &lhXpt->hMsgOverflowCb, lhXpt->hInt, BCHP_INT_ID_XPT_MSG_OVFL_INTR_FLAG, (BINT_CallbackFunc) BXPT_P_Interrupt_MsgOverflowVector_isr, lhXpt, 0 ));
	if( ExitCode != BERR_SUCCESS )
	{
		BDBG_ERR(("Unabled to create XPT Message Overflow Callback!"));
		goto done;
	}
#endif

    lhXpt->SharedXcRsBufferOffset = 0;  /* init to NULL here. In BXPT_Close(), check for non-NULL and free if it is */

#if BXPT_HAS_FIXED_RSBUF_CONFIG
	ExitCode = BERR_TRACE( BXPT_P_RsBuf_Init( lhXpt, &Defaults->BandwidthConfig ) );
	if( ExitCode != BERR_SUCCESS )
	{
		BDBG_ERR(("Rate smoothing buffer init FAILED!"));
		goto done;
	}
#endif

#if BXPT_HAS_FIXED_XCBUF_CONFIG
	ExitCode = BERR_TRACE( BXPT_P_XcBuf_Init( lhXpt, &Defaults->BandwidthConfig ) );
	if( ExitCode != BERR_SUCCESS )
	{
		BDBG_ERR(("XPT client buffer init FAILED!"));
		goto done;
	}
#endif

    BXPT_P_PcrOffset_ModuleInit( lhXpt );

#if BXPT_HAS_MESG_BUFFERS
    BXPT_Mesg_SetPid2Buff( lhXpt, true );
#endif

#if BXPT_HAS_TSMUX
    BXPT_TsMux_P_ResetBandPauseMap( lhXpt );
    BXPT_TsMux_P_ResetPacingPauseMap( lhXpt );
#endif

#if BXPT_HAS_PARSER_REMAPPING
    {
        BXPT_ParserBandMapping ParserMap;
        unsigned ii;
        uint32_t Reg, RegAddr;
        
        /* Default virtual parser to equal the underlying physical parser. */
        for( ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++ ) 
        {
            ParserMap.FrontEnd[ ii ].VirtualParserBandNum = ii;
            ParserMap.FrontEnd[ ii ].VirtualParserIsPlayback = false;
        }

        for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ ) 
        {
            ParserMap.Playback[ ii ].VirtualParserBandNum = ii;
            ParserMap.Playback[ ii ].VirtualParserIsPlayback = true;
        }

        BXPT_SetParserMapping( lhXpt, &ParserMap );

        /* Set the defaults, since the register contents don't reflect them after chip reset */
        for( ii = 0; ii < BXPT_NUM_PLAYBACKS; ii++ ) 
        {
            RegAddr = BCHP_XPT_PB0_PLAYBACK_PARSER_BAND_ID + ii * PB_PARSER_REG_STEPSIZE;

            Reg = BREG_Read32( hRegister, RegAddr );
            Reg &= ~( 
                BCHP_MASK( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_SEL ) |
                BCHP_MASK( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_BAND_ID ) 
                );
            Reg |= (
                BCHP_FIELD_DATA( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_SEL, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_BAND_ID, ii )
            );  
            BREG_Write32( hRegister, RegAddr, Reg );
        }
    }
#endif

#if BXPT_HAS_DPCRS
    {
        unsigned Index;

        for( Index = 0; Index < BXPT_NUM_PCRS; Index++ )
        {
            lhXpt->JitterTimestamp[ Index ] = BXPT_PCR_JitterTimestampMode_eAuto; 
            lhXpt->PbJitterDisable[ Index ] = BXPT_PCR_JitterCorrection_eAuto;
            lhXpt->LiveJitterDisable[ Index ] = BXPT_PCR_JitterCorrection_eAuto;
        }
    }
#endif

#if BXPT_HAS_MPOD_RSBUF
    for( i = 0; i < lhXpt->MaxPidParsers; i++ )
    {
        /* must be called after BXPT_SetParserMapping has been called above */
        uint32_t Addr = BXPT_P_GetParserCtrlRegAddr( lhXpt, i, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL2 );
        uint32_t Reg = BREG_Read32( hRegister, Addr );
        Reg &= ~(
            BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL2, MPOD_MODE_SEL )
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL2, MPOD_MODE_SEL, 
                Defaults->BandwidthConfig.IbParserClients[ i ].ToMpodRs ? 1 : 0 )
        );
        BREG_Write32( hRegister, Addr, Reg );
    }
#endif

	done:
	*hXpt = lhXpt;
	BDBG_LEAVE( BXPT_Open );

#ifdef BCHP_PWR_RESOURCE_XPT
	/* a failed BXPT_Open powers down the 216 and 108M */
	if (ExitCode!=BERR_SUCCESS) {	
		BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_XPT_XMEMIF);
		BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_XPT_108M);

        #ifdef BCHP_PWR_RESOURCE_XPT_SRAM
        	BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_XPT_SRAM);
        #endif	
	}
#endif

	return( ExitCode );
}

void BXPT_Close(
	BXPT_Handle hXpt		/* [in] Handle for the Transport to be closed. */ 
	)
{
	unsigned int Index;

	BERR_Code Res;

	BDBG_ASSERT( hXpt );

	/* Shutdown PID parsers and playbacks before XC and RS buffers. */
#if BXPT_HAS_IB_PID_PARSERS
    for( Index = 0; Index < hXpt->MaxPidParsers; Index++ )
	    BXPT_SetParserEnable( hXpt, Index, false );
#endif

#if BXPT_HAS_FIXED_RSBUF_CONFIG
	Res = BXPT_P_RsBuf_Shutdown( hXpt );
	BDBG_ASSERT( Res == BERR_SUCCESS );
#endif

#if BXPT_HAS_FIXED_XCBUF_CONFIG
	Res = BXPT_P_XcBuf_Shutdown( hXpt );
	BDBG_ASSERT( Res == BERR_SUCCESS );
#endif

    if( hXpt->SharedXcRsBufferOffset )
    {
        BXPT_P_FreeSharedXcRsBuffer( hXpt );
    }

	for( Index = 0; Index < BXPT_NUM_RAVE_CHANNELS; Index++ )
		if( hXpt->RaveChannels[ Index ].Allocated == true )
		{
			Res = BXPT_Rave_CloseChannel( ( BXPT_Rave_Handle ) hXpt->RaveChannels[ Index ].Handle );
			BDBG_ASSERT( Res == BERR_SUCCESS );
		}
	
	for( Index = 0; Index < BXPT_NUM_PCR_OFFSET_CHANNELS; Index++ )
		if( hXpt->PcrOffsets[ Index ].Allocated == true )  
		{
			Res = BXPT_PcrOffset_Close( ( BXPT_PcrOffset_Handle ) hXpt->PcrOffsets[ Index ].Handle );
			BDBG_ASSERT( Res == BERR_SUCCESS );
		}
	
	for ( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ ) {
		if ( hXpt->PlaybackHandles[Index].Opened == true ) {
			BXPT_Playback_CloseChannel( (BXPT_Playback_Handle) &hXpt->PlaybackHandles[Index] );
		}
	}

#if BXPT_HAS_PACKETSUB
	for ( Index = 0; Index < BXPT_NUM_PACKETSUBS; Index++ ) {
		if ( hXpt->PacketSubHandles[Index].Opened == true ) {
			BXPT_PacketSub_CloseChannel( (BXPT_PacketSub_Handle) &hXpt->PacketSubHandles[Index]);            
		}
	}
#endif

#if BXPT_HAS_REMUX
	for ( Index = 0; Index < BXPT_NUM_REMULTIPLEXORS; Index++ ) {
		if ( hXpt->RemuxHandles[Index].Opened == true ) {
			BXPT_Remux_CloseChannel( (BXPT_Remux_Handle) &hXpt->RemuxHandles[Index]);
		}
	}
#endif

#if BXPT_HAS_MESG_BUFFERS
	/*
	** Free any message buffers we've allocated. If the user has allocated any, its
	** the their responsibility to free them. 
	*/
	for( Index = 0; Index < BXPT_NUM_MESG_BUFFERS; Index++ )
	{
		MessageBufferEntry *Buffer = &hXpt->MessageBufferTable[ Index ];
		  
		/* Free the old buffer, if there is one. */
		if( Buffer->IsAllocated == true )
		{
			if( Buffer->Address )
				BMEM_Free( hXpt->hMemory, ( void * ) Buffer->Address );
			Buffer->IsAllocated = false;
		}
	}

	BINT_DestroyCallback(hXpt->hMsgCb);
	BINT_DestroyCallback(hXpt->hMsgOverflowCb);
#endif

	/* Reset the core, thus stopping any unwanted interrupts. */
	BXPT_P_ResetTransport( hXpt->hRegister );

#ifdef BCHP_PWR_RESOURCE_XPT
	#ifdef BCHP_PWR_RESOURCE_XPT_SRAM
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_SRAM);
	BKNI_Free(hXpt->SRAM_Backup);
	BKNI_Free(hXpt->Reg_Backup);
	#endif
    
	/* release the 261 and 108M after BXPT_P_ResetTransport */
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_XMEMIF);
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_108M);
#endif

	/* Dont need the transport handle any more. */
	BKNI_Free( hXpt );
}


void BXPT_GetDefaultStandbySettings(
    BXPT_StandbySettings *pSettings
    )
{
	pSettings->UseWakeupPacket = false;
	pSettings->S3Standby = false;
}

BERR_Code BXPT_Standby(
	BXPT_Handle hXpt,
	BXPT_StandbySettings *pSettings
	)
{
#ifdef BCHP_PWR_RESOURCE_XPT
	unsigned int Index;
#endif
	BDBG_ASSERT(pSettings);    

#ifdef BCHP_PWR_RESOURCE_XPT
	if (hXpt->bStandby) {
		BDBG_ERR(("Already in standby"));
		return BERR_UNKNOWN;
	}
    
	/* check if XPT is still in use. if so, we cannot enter standby */

#if BXPT_HAS_IB_PID_PARSERS  
	for( Index = 0; Index < hXpt->MaxPidParsers; Index++ ) {
	        uint32_t Reg, RegAddr;
		RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, Index, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		if (BCHP_GET_FIELD_DATA(Reg, XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE)) {
			BDBG_ERR(("One or more parsers still enabled. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}
#endif

	for ( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ ) {
		if ( hXpt->PlaybackHandles[Index].Running == true ) {
			BDBG_ERR(("One or more playback channels still running. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}

#if BXPT_HAS_PACKETSUB
	for ( Index = 0; Index < BXPT_NUM_PACKETSUBS; Index++ ) {
		if ( hXpt->PacketSubHandles[Index].Running == true ) {
			BDBG_ERR(("One or more packetsub channels still running. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}
#endif

#if BXPT_HAS_REMUX
	for ( Index = 0; Index < BXPT_NUM_REMULTIPLEXORS; Index++ ) {
		if ( hXpt->RemuxHandles[Index].Running == true ) {
			BDBG_ERR(("One or more remux channels still running. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}
#endif

    /* if we get to this point, then XPT is not in use */

#ifdef BCHP_PWR_RESOURCE_XPT_SRAM
	if( pSettings->S3Standby ) 
	{
		BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
		BXPT_P_RegisterToMemory(hXpt->hRegister, hXpt->Reg_Backup, XPT_REG_SAVE_LIST);
		BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
	}
	hXpt->bS3Standby = pSettings->S3Standby;
    
	BXPT_P_RegisterToMemory(hXpt->hRegister, hXpt->SRAM_Backup, XPT_SRAM_LIST);
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_SRAM);
#endif

	/* XPT can now release the 216 and 108M */
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_XMEMIF);
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_108M);

#ifdef BCHP_PWR_RESOURCE_XPT_WAKEUP
    /* Keep the 216 clock running for the XPT_WAKEUP block */
    if( true == pSettings->UseWakeupPacket && !hXpt->WakeupEnabled )
    {
        BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_WAKEUP);
        hXpt->WakeupEnabled = true;
    }
#endif /* BCHP_PWR_RESOURCE_XPT_WAKEUP */

	hXpt->bStandby = true;
#else
	BSTD_UNUSED( hXpt );
#endif /* #ifdef BCHP_PWR_RESOURCE_XPT */

    return BERR_SUCCESS;
}

BERR_Code BXPT_Resume(
	BXPT_Handle hXpt
	)
{
#ifdef BCHP_PWR_RESOURCE_XPT
	if (!hXpt->bStandby) {
		BDBG_ERR(("Not in standby"));
		return BERR_UNKNOWN;        
	}
	BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_XMEMIF);
	BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_108M);

#ifdef BCHP_PWR_RESOURCE_XPT_SRAM
	BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_SRAM);
	BXPT_P_MemoryToRegister(hXpt->hRegister, hXpt->SRAM_Backup, XPT_SRAM_LIST);

	if( hXpt->bS3Standby ) 
	{
		BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
		BXPT_P_MemoryToRegister(hXpt->hRegister, hXpt->Reg_Backup, XPT_REG_SAVE_LIST);
		BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
	}
    
	BXPT_P_RaveRamInit( (BXPT_Rave_Handle) hXpt->vhRave );
#endif

#ifdef BCHP_PWR_RESOURCE_XPT_WAKEUP
    if( hXpt->WakeupEnabled )
    {
        BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_WAKEUP);
        hXpt->WakeupEnabled = false;
    }
#endif /* BCHP_PWR_RESOURCE_XPT_WAKEUP */

	hXpt->bStandby = false;
#else
    BSTD_UNUSED( hXpt );
#endif /* #ifdef BCHP_PWR_RESOURCE_XPT */

	return BERR_SUCCESS;
}

void BXPT_GetCapability(
	BXPT_Handle hXpt, 			/* [in] Which transport to get data. */
	BXPT_Capability *Capability	/* [out] Where to put the capability data. */
	)
{
	BSTD_UNUSED( hXpt );
	BDBG_ASSERT( Capability );

	/* 
	** Set the values in the caller's structure.  
	*/
#if BXPT_HAS_PLAYBACK_PARSERS
	Capability->MaxPlaybacks = BXPT_NUM_PLAYBACKS; 
	Capability->MaxPidChannels = BXPT_NUM_PID_CHANNELS; 
#endif

#if BXPT_HAS_IB_PID_PARSERS
	Capability->MaxPidParsers = BXPT_NUM_PID_PARSERS; 
	Capability->MaxInputBands = BXPT_NUM_INPUT_BANDS; 
#endif

	Capability->MaxTpitPids = BXPT_NUM_TPIT_PIDS;

#if BXPT_HAS_MESG_BUFFERS	 
	Capability->MaxFilterBanks = BXPT_NUM_FILTER_BANKS;                        
	Capability->MaxFiltersPerBank = BXPT_NUM_FILTERS_PER_BANK; 
#endif

#if BXPT_HAS_PACKETSUB
	Capability->MaxPacketSubs = BXPT_NUM_PACKETSUBS;
#endif

#if BXPT_HAS_DPCRS
	Capability->MaxPcrs = BXPT_NUM_PCRS;
#endif

	Capability->MaxRaveContexts = BXPT_NUM_RAVE_CONTEXTS; 
}


#if BXPT_HAS_IB_PID_PARSERS

BERR_Code BXPT_GetParserConfig(
	BXPT_Handle hXpt,				/* [in] Handle for the transport to access. */
	unsigned int ParserNum,				/* [in] Which parser band to access. */
	BXPT_ParserConfig *ParserConfig	/* [out] The current settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ParserConfig );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );                                                 
	}
	else
	{
        RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

		ParserConfig->ErrorInputIgnore = BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE );
		ParserConfig->TsMode = BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE );
		ParserConfig->AcceptNulls = BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD );

#if BCHP_XPT_FULL_PID_PARSER_IBP_ACCEPT_ADAPT_00_PARSER0_ACCEPT_ADP_00_MASK != 0x00000001 || BXPT_NUM_PID_PARSERS > 32
    #error "PI NEEDS UPDATING"
#else
		Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FULL_PID_PARSER_IBP_ACCEPT_ADAPT_00 );
		ParserConfig->AcceptAdapt00 = (Reg >> GetParserIndex (hXpt, ParserNum)) & 0x01 ? true : false;
#endif
	}

	return( ExitCode );
}

BERR_Code BXPT_GetDefaultParserConfig(
	BXPT_Handle hXpt,				/* [in] Handle for the transport to access. */
	unsigned int ParserNum,				/* [in] Which parser band to access. */
	BXPT_ParserConfig *ParserConfig	/* [out] The current settings */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ParserConfig );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		ParserConfig->ErrorInputIgnore = false;
		ParserConfig->TsMode = BXPT_ParserTimestampMode_eAutoSelect;
		ParserConfig->AcceptNulls = false;
        ParserConfig->AcceptAdapt00 = false;
	}

	return( ExitCode );
}

BERR_Code BXPT_SetParserConfig(
	BXPT_Handle hXpt,						/* [in] Handle for the transport to access. */
	unsigned int ParserNum,				/* [in] Which parser band to access. */
	const BXPT_ParserConfig *ParserConfig	/* [in] The new settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ParserConfig );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		/* Clear all the bits we are about to change. */
		Reg &= ~(
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE ) |
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE ) |
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD ) 
		);

		/* Now set the new values. */
		Reg |= (
			BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE, ParserConfig->ErrorInputIgnore ) |
			BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE, ParserConfig->TsMode ) |
			BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD, ParserConfig->AcceptNulls )
		);

		BREG_Write32( hXpt->hRegister, RegAddr, Reg );


#if BCHP_XPT_FULL_PID_PARSER_IBP_ACCEPT_ADAPT_00_PARSER0_ACCEPT_ADP_00_MASK != 0x00000001 || BXPT_NUM_PID_PARSERS > 32
    #error "PI NEEDS UPDATING"
#else
		Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FULL_PID_PARSER_IBP_ACCEPT_ADAPT_00 );
		Reg &= ~(0x01 << ParserNum);
        if( ParserConfig->AcceptAdapt00 ) 
            Reg |= (0x01 << GetParserIndex (hXpt, ParserNum));
		BREG_Write32( hXpt->hRegister, BCHP_XPT_FULL_PID_PARSER_IBP_ACCEPT_ADAPT_00, Reg );
#endif
	}

	return( ExitCode );
}

BERR_Code BXPT_GetDefaultInputBandConfig(
	BXPT_Handle hXpt,						/* [in] Handle for the transport to access. */
	unsigned int BandNum,						/* [in] Which input band to access. */
	BXPT_InputBandConfig *InputBandConfig	/* [in] The current settings */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( InputBandConfig );

	/* Is the parser number within range? */
	if( BandNum > hXpt->MaxInputBands )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "BandNum %lu is out of range!", BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		InputBandConfig->ClockPolSel = BXPT_Polarity_eActiveHigh;
		InputBandConfig->SyncPolSel = BXPT_Polarity_eActiveHigh;
		InputBandConfig->DataPolSel = BXPT_Polarity_eActiveHigh;
		InputBandConfig->ValidPolSel = BXPT_Polarity_eActiveHigh;
		InputBandConfig->ForceValid = false;
		InputBandConfig->SyncDetectEn = false;
		InputBandConfig->LsbFirst = false;
		InputBandConfig->UseSyncAsValid = false;
		InputBandConfig->ErrorPolSel = BXPT_Polarity_eActiveHigh;
		InputBandConfig->EnableErrorInput = false;
		InputBandConfig->IbPktLength = 188;     /* Default to MPEG */

		if ( BandNum == 4 ) 
			InputBandConfig->ParallelInputSel = true;
		else 
			InputBandConfig->ParallelInputSel = false;
	}

	return( ExitCode );
}

BERR_Code BXPT_GetInputBandConfig(
	BXPT_Handle hXpt,						/* [in] Handle for the transport to access. */
	unsigned int BandNum,						/* [in] Which input band to access. */
	BXPT_InputBandConfig *InputBandConfig	/* [in] The current settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( InputBandConfig );

	/* Is the parser number within range? */
	if( BandNum > hXpt->MaxInputBands )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "BandNum %lu is out of range!", BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		RegAddr =  BCHP_XPT_FE_IB0_CTRL + ( BandNum * IB_REG_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		InputBandConfig->ClockPolSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_CLOCK_POL_SEL ) ? 
			BXPT_Polarity_eActiveLow : BXPT_Polarity_eActiveHigh;

		InputBandConfig->SyncPolSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_SYNC_POL_SEL ) ? 
			BXPT_Polarity_eActiveLow : BXPT_Polarity_eActiveHigh;

		InputBandConfig->DataPolSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_DATA_POL_SEL ) ? 
			BXPT_Polarity_eActiveLow : BXPT_Polarity_eActiveHigh;

		InputBandConfig->ValidPolSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_VALID_POL_SEL ) ? 
			BXPT_Polarity_eActiveLow : BXPT_Polarity_eActiveHigh;

		InputBandConfig->ForceValid = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_FORCE_VALID ) ?
			true : false;

		InputBandConfig->SyncDetectEn = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_SYNC_DETECT_EN ) ?
			true : false;

		InputBandConfig->LsbFirst = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_LSB_FIRST ) ?
			true : false;

		InputBandConfig->UseSyncAsValid = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_USE_SYNC_AS_VALID ) ?
			true : false;

		InputBandConfig->ErrorPolSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_ERROR_POL_SEL ) ? 
			BXPT_Polarity_eActiveLow : BXPT_Polarity_eActiveHigh;

		InputBandConfig->EnableErrorInput = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_ERROR_INPUT_EN ) ?
			true : false;

		InputBandConfig->IbPktLength = ( int ) BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_PKT_LENGTH );

		InputBandConfig->ParallelInputSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) ? true : false;
	}

	return( ExitCode );
}

BERR_Code BXPT_SetInputBandConfig(
	BXPT_Handle hXpt,							/* [in] Handle for the transport to access. */
	unsigned int BandNum,						/* [in] Which input band to access. */
	const BXPT_InputBandConfig *InputBandConfig	/* [in] The new settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;
	unsigned short ParallelInputEn = 0;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( InputBandConfig );

	/* Is the parser number within range? */
	if( BandNum > hXpt->MaxInputBands )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "BandNum %lu is out of range!", BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		RegAddr =  BCHP_XPT_FE_IB0_CTRL + ( BandNum * IB_REG_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		/* Clear all the bits we are about to change. */
		Reg &= ~(
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_CLOCK_POL_SEL ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_SYNC_POL_SEL ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_DATA_POL_SEL ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_VALID_POL_SEL ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_FORCE_VALID ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_SYNC_DETECT_EN ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_LSB_FIRST ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_USE_SYNC_AS_VALID ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_ERROR_POL_SEL ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_ERROR_INPUT_EN ) |
			BCHP_MASK( XPT_FE_IB0_CTRL, IB_PKT_LENGTH ) 
		);

		/* Only band 4 has parallel input support. */
		if( BandNum == 4 )
			ParallelInputEn = InputBandConfig->ParallelInputSel == true ? 1 : 0;
		else
		{
			if( InputBandConfig->ParallelInputSel == true )
			{
				BDBG_ERR(( "Parallel input is not supported on IB %u", BandNum ));
				goto Done;
			}
		}
		
	
		/* Now set the new values. */
		Reg |= (	
			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL, ParallelInputEn ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_CLOCK_POL_SEL, 
				InputBandConfig->ClockPolSel == BXPT_Polarity_eActiveLow ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_SYNC_POL_SEL, 
				InputBandConfig->SyncPolSel == BXPT_Polarity_eActiveLow ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_DATA_POL_SEL, 
				InputBandConfig->DataPolSel == BXPT_Polarity_eActiveLow ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_VALID_POL_SEL, 
				InputBandConfig->ValidPolSel == BXPT_Polarity_eActiveLow ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_FORCE_VALID, 
				InputBandConfig->ForceValid == true ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_SYNC_DETECT_EN, 
				InputBandConfig->SyncDetectEn == true ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_LSB_FIRST, 
				InputBandConfig->LsbFirst == true ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_USE_SYNC_AS_VALID, 
				InputBandConfig->UseSyncAsValid == true ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_ERROR_POL_SEL, 
				InputBandConfig->ErrorPolSel == BXPT_Polarity_eActiveLow ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_ERROR_INPUT_EN, 
				InputBandConfig->EnableErrorInput == true ? 1 : 0 ) |

			BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_PKT_LENGTH, InputBandConfig->IbPktLength ) 
		);

		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	Done:

	return( ExitCode );
}

BERR_Code BXPT_ParserAllPassMode( 
	BXPT_Handle hXpt,	/* [in] Handle for the transport to access. */
	unsigned int ParserNum,						/* [in] Which input band to access. */
	bool AllPass		/* [in] All-pass enabled if true, or not if false. */
	)
{
	uint32_t ParserReg, ParserRegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
        ParserRegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		ParserReg = BREG_Read32( hXpt->hRegister, ParserRegAddr );
		ParserReg &= ~(
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD ) 
		);

        /* Also need to update the Primary and Secondary CC checking: in all-pass mode, the will see the multitude of
        ** different IDs as so many CC errors, and drop 'duplicated' PIDs. In all-pass, the PIDs for accepted by parser 
        ** X are all mapped to PID channel X.  
        */ 

		/* Now set the new values. */
        if( AllPass && false == hXpt->IsParserInAllPass[ ParserNum ] )
        {
            BXPT_PidChannel_CC_Config AllCcDisabled = { false, false, false, 0 };

            ParserReg |= BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD, 1 );
            /* RS and XC blockouts are set at BXPT_Open() */

            /* Remember the config before entering all-pass. */
            BXPT_GetPidChannel_CC_Config( hXpt, ParserNum, &hXpt->CcConfigBeforeAllPass[ ParserNum ] );

            /* Now disable the CC checks */
            BXPT_SetPidChannel_CC_Config( hXpt, ParserNum, &AllCcDisabled );
            hXpt->IsParserInAllPass[ ParserNum ] = true;
            BREG_Write32( hXpt->hRegister, ParserRegAddr, ParserReg );
        }
        else if( false == AllPass && true == hXpt->IsParserInAllPass[ ParserNum ] ) 
        {
            /* Restore the config we had before entering all-pass. */
            BXPT_SetPidChannel_CC_Config( hXpt, ParserNum, &hXpt->CcConfigBeforeAllPass[ ParserNum ] );
            hXpt->IsParserInAllPass[ ParserNum ] = false;
            BREG_Write32( hXpt->hRegister, ParserRegAddr, ParserReg );
        }
	}

	return( ExitCode );
}

BERR_Code BXPT_SetParserDataSource( 
	BXPT_Handle hXpt, 				/* [in] Handle for this transport */
	unsigned int ParserNum, 			/* [in] Which parser to configure */
	BXPT_DataSource DataSource, 	/* [in] The data source. */
	unsigned int WhichSource 			/* [in] Which instance of the source */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	/* Is the requested DataSource valid? */
	else if( DataSource == BXPT_DataSource_eInputBand && WhichSource > hXpt->MaxInputBands )
	{
		/* Requested an input band we dont support. Complain. */
		BDBG_ERR(( "WhichSource %lu is out of range!. Not enough input bands.", WhichSource ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
        RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		/* Clear all the bits we are about to change. */
		Reg &= ~( 
             BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL_MSB ) |
             BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL ) 
        );

		/* Now set the new values. */																											 
		switch( DataSource )
		{
			/* Values for input band selection start at 0 and are sequential. */
			case BXPT_DataSource_eInputBand: 
			Reg |= ( BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL, WhichSource ) );
			break;

			/* Values for remux feedback selection start at 0x0E and are sequential. */
			case BXPT_DataSource_eRemuxFeedback: 
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL_MSB, 1 ) |
                BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL, WhichSource ) 
            );
			break;

			default:
			BDBG_ERR(( "Unsupported DataSource %lu!", ( unsigned long ) DataSource ));
			ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
			break;
		} 

		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	return( ExitCode );
}

BERR_Code BXPT_GetParserDataSource( 
	BXPT_Handle hXpt, 				/* [in] Handle for this transport */
	unsigned int ParserNum, 			/* [in] Which parser to configure */
	BXPT_DataSource *DataSource, 	/* [out] The data source. */
	unsigned int *WhichSource 			/* [out] Which instance of the source */
	)
{
	uint32_t Reg, RegAddr, InputBits;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
        RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		InputBits = BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_INPUT_SEL );
		
		if( InputBits < 0x10)
		{
			*DataSource	= BXPT_DataSource_eInputBand;
			*WhichSource = InputBits;
		}
		else if( InputBits >= 0x10 )
		{
			#if BXPT_HAS_REMUX
				*DataSource	= BXPT_DataSource_eRemuxFeedback;
				*WhichSource = InputBits - 0x10;
			#else
				BDBG_ERR(( "DataSource %lu is out of range!", InputBits ));
				ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
			#endif
		}
	}

	return( ExitCode );
}

BERR_Code BXPT_SetParserEnable( 
	BXPT_Handle hXpt, 				/* [in] Handle for this transport */
	unsigned int ParserNum, 			/* [in] Which parser to configure */
	bool Enable					/* [in] Parser enabled if true, disabled if false. */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

#ifdef BCHP_PWR_RESOURCE_XPT_PARSER
	unsigned wasEnabled;
#endif

	BDBG_ASSERT( hXpt );

	/* Is the parser number within range? */
	if( ParserNum > hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
#ifdef BCHP_PWR_RESOURCE_XPT_PARSER
		wasEnabled = BCHP_GET_FIELD_DATA(Reg, XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE);
		/* only change refcnt if changing state */
		if (!wasEnabled && Enable) {
			BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_PARSER);
		}
#endif

		/* Clear all the bits we are about to change. */
		Reg &= ~( BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE ) );
		Reg |= ( BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ENABLE, Enable == true ? 1 : 0 ) );
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#ifdef BCHP_PWR_RESOURCE_XPT_PARSER
		if (wasEnabled && !Enable) {
			BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_PARSER);
		}
#endif
	}

	return( ExitCode );
}

#endif

BERR_Code BXPT_EnablePidChannel( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum			/* [in] Which PID channel. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if( IsPidDuplicated( hXpt, PidChannelNum ) )
    {
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    } 
	else
	{
        BXPT_P_EnablePidChannel( hXpt, PidChannelNum );
	}

	return( ExitCode );
}

BERR_Code BXPT_P_EnablePidChannel(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum			/* [in] Which PID channel. */
    )
{ 
	BERR_Code ExitCode = BERR_SUCCESS;

#ifdef ENABLE_PLAYBACK_MUX
	/*gain access to the pid table*/
	BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

    hXpt->PidChannelTable[ PidChannelNum ].EnableRequested = true;

    if( PidChannelHasDestination( hXpt, PidChannelNum ) )
        SetChannelEnable( hXpt, PidChannelNum, true );

#ifdef ENABLE_PLAYBACK_MUX
	/*leave pid table protected area*/
	BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

	return( ExitCode );
}

BERR_Code BXPT_DisablePidChannel( 
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum			/* [in] Which PID channel. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else                                                    
	{
#ifdef ENABLE_PLAYBACK_MUX
		/*gain access to the pid table*/
		BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

		hXpt->PidChannelTable[ PidChannelNum ].EnableRequested = false;
/* PR 55511: Need to allow nexus to shut off the PID channel even when there are still consumers attached.      
So, don't do the destination check before calling SetChannelEnable().
		if( PidChannelHasDestination( hXpt, PidChannelNum ) == false )     */
			SetChannelEnable( hXpt, PidChannelNum, false );

#ifdef ENABLE_PLAYBACK_MUX
		/*leave pid table protected area*/
		BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/
	}

	return( ExitCode );
}

BERR_Code BXPT_AllocPidChannel( 
	BXPT_Handle hXpt, 			/* [in] Handle for this transport */
	bool NeedMessageBuffer, 	/* [in] Is a message buffer required? */
	unsigned int *PidChannelNum 	/* [out] The allocated channel number. */
	)
{
	unsigned int i = 0;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Search for channels with buffers, if thats what they asked for. */
	if( NeedMessageBuffer == true )
	{
		for( i = 0; i < hXpt->MaxPidChannels; i++ )
		{
			if( hXpt->PidChannelTable[ i ].IsAllocated == false 
			&& hXpt->PidChannelTable[ i ].HasMessageBuffer ==  true )
			{
				hXpt->PidChannelTable[ i ].IsAllocated = true;
				break;
			}
		}
	}
	else
	{
		/* Otherwise, grab the first free channel we find. */
		for( i= 0; i < hXpt->MaxPidChannels; i++ )
		{
			if( hXpt->PidChannelTable[ i ].IsAllocated == false )
			{
				hXpt->PidChannelTable[ i ].IsAllocated = true;
				break;
			}
		}
	}

	*PidChannelNum = i;

	return( ExitCode );
}

BERR_Code BXPT_ConfigurePidChannel( 
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to configure. */
	unsigned int Pid, 				/* [in] PID to use. */
	unsigned int Band 				/* [in] The parser band to use. */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_TSMUX
    unsigned int BXPT_PB_P_GetPbBandId( BXPT_Handle hXpt, unsigned int Band );
#endif
#endif

	BDBG_ASSERT( hXpt );
    BDBG_MSG(( "BXPT_ConfigurePidChannel: PidChannel %u, PID 0x%04X, %s parser %u", 
               PidChannelNum, Pid, BXPT_P_IS_PB( Band ) ? "Playback" : "Live", Band & 0x7FFF));

	if( Pid >= 0x2000 )
	{
		/* Bad PID. Complain. */
		BDBG_ERR(( "Pid %lu is out of range!", ( unsigned long ) Pid ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else							 		  
	{
#if BXPT_HAS_IB_PID_PARSERS
		unsigned FeSel = 0;
		unsigned OldFeSel = 255;
        unsigned OldPid = 0x2000;
        unsigned OldBand = 0x2000;     /* Never have this many parser bands. */

  		PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ PidChannelNum ];

#ifdef ENABLE_PLAYBACK_MUX
		/*gain access to the pid table*/
		BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

		Entry->Pid = Pid;
		Entry->Band = Band;
		Entry->IsAllocated = true;


		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

	 	if( BXPT_P_IS_PB( Band ) )
		{
			BXPT_P_CLEAR_PB_FLAG( Band );	/* Remove the PB parser flag. */
			FeSel = 1;
		}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_TSMUX
    /* Band ID in playback may have been remapped. */
	if( FeSel )
    {
        unsigned OldBand = Band;

        Band = BXPT_PB_P_GetPbBandId( hXpt, Band );
        BDBG_MSG(( "Remapping PID channel %u PB %u to PB %u", (unsigned) PidChannelNum, OldBand, Band ));
    }
#endif
#endif

#if BXPT_SW7425_1323_WORKAROUND
    	Entry->IsPb = FeSel;
#endif
             
        OldFeSel = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL );
        OldBand = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );

#if BXPT_HAS_DIRECTV_SUPPORT
		/* Need to preserve the HD filter bits, if enabled */
		if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) )
		{
            OldPid = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_EN_PID_CHANNEL_SCID );

			Reg &= ~( 
				BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
				BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
				BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_EN_PID_CHANNEL_SCID ) 
			);
		 
			Reg |= ( 
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL, FeSel ) |
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT, Band ) |
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, HD_FILT_EN_PID_CHANNEL_SCID, Pid ) 
			);
		}
		else
		{
            OldPid = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID );

			Reg &= ~( 
				BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
				BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
				BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID ) 
			);
		 
			Reg |= ( 
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL, FeSel ) |
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT, Band ) |
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID, Pid ) 
			);
		}
#else
        OldPid = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PID_CHANNEL_PID );

		Reg &= ~( 
			BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
			BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
			BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_PID ) 
		);
	 
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL, FeSel ) |
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT, Band ) |
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PID_CHANNEL_PID, Pid ) 
		);
#endif

        /* Write to the PID table ONLY if the configuration has changed. */
        if( OldFeSel != FeSel || OldBand != Band || OldPid != Pid )
        {
    		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
        }

		Entry->IsPidChannelConfigured = true;
#ifdef ENABLE_PLAYBACK_MUX
		/*leave pid table protected area*/
		BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

#else /* BXPT_HAS_IB_PID_PARSERS */
  		PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ PidChannelNum ];
		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if (BCHP_CHIP == 7630 && BCHP_VER >= BCHP_VER_B2 )
		Reg &= ~( 
			BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
			BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_INPUT_SELECT ) |
			BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID ) 
		);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL, 1 ) |
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PID_CHANNEL_INPUT_SELECT, Band ) |
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID, Pid ) 
		);
#else
		Reg &= ~( 
			BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
			BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
			BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID ) 
		);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL, 1 ) |
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT, Band ) |
			BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID, Pid ) 
		);
#endif

   		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_SW7425_1323_WORKAROUND
    	Entry->IsPb = true;
#endif
		Entry->IsPidChannelConfigured = true;
#endif /* BXPT_HAS_IB_PID_PARSERS */
	}

	return( ExitCode );
}

#ifdef ENABLE_PLAYBACK_MUX

BERR_Code BXPT_SetPidChannelSwitched(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool IsSwitched
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

    /*gain access to the pid table*/
    BKNI_EnterCriticalSection();
    
    ExitCode = BXPT_SetPidChannelSwitched_isr( hXpt, PidChannelNum, IsSwitched );
    
    /*leave pid table protected area*/
    BKNI_LeaveCriticalSection();

    return ExitCode;
}

BERR_Code BXPT_SetPidChannelSwitched_isr(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool IsSwitched
	)
{
	uint32_t Reg, RegAddr;
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* Set the PID channels enable bit. */
		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

		if(IsSwitched)
		{
			/*ignore pid version so our transport buffers aren't flushed*/
			Reg |= BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, IGNORE_PID_VERSION, 1 );
		}
		else
		{
			/*look at pid versions to behave like normal*/
			Reg &= ~BCHP_MASK( XPT_FE_PID_TABLE_i, IGNORE_PID_VERSION );
		}

   		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	return(ExitCode);
}

bool BXPT_SwitchPidChannel(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool EnableIt
	)
{
	uint32_t Reg, RegAddr;

	bool result = true;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		return(false);
	}
	else if(!hXpt->PidChannelTable[PidChannelNum].IsAllocated)
	{
		return(false);
	}

	/*gain access to the pid table*/
	BKNI_EnterCriticalSection();

	/*set whether the muxed pid is enabled or not*/
	hXpt->PidChannelTable[PidChannelNum].MuxEnable = EnableIt;

#if BXPT_SW7425_1323_WORKAROUND
	hXpt->PidChannelTable[PidChannelNum].IsEnabled = EnableIt;
#endif

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	Reg &= ~BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE );

	/*check if we're supposed to enable the channel*/
	if(EnableIt)
	{
		/*need to enable the channel*/
		if(!hXpt->PidChannelTable[PidChannelNum].EnableRequested)
		{
			result = false;
		}
		else if(!PidChannelHasDestination( hXpt, PidChannelNum ))
		{
			result = false;
		}
		else
		{
			Reg |= BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE, 1 );
		}
	}

	BREG_Write32( hXpt->hRegister, RegAddr, Reg );

	/*leave pid table protected area*/
	BKNI_LeaveCriticalSection();

	return(result);
}

bool BXPT_SwitchPidChannelISR(
	BXPT_Handle hXpt, 	   				/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool EnableIt
	)
{
	uint32_t Reg, RegAddr;
	bool result = true;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		return(false);
	}
	else if(!hXpt->PidChannelTable[PidChannelNum].IsAllocated)
	{
		return(false);
	}

	/*set whether the muxed pid is enabled or not*/
	hXpt->PidChannelTable[PidChannelNum].MuxEnable = EnableIt;

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	Reg &= ~BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE );

#if BXPT_SW7425_1323_WORKAROUND
	hXpt->PidChannelTable[PidChannelNum].IsEnabled = EnableIt;
#endif

	/*check if we're supposed to enable the channel*/
	if(EnableIt)
	{
		/*need to enable the channel*/
		if(!hXpt->PidChannelTable[PidChannelNum].EnableRequested)
		{
			result = false;
		}
		else if(!PidChannelHasDestination( hXpt, PidChannelNum ))
		{
			result = false;
		}
		else
		{
			Reg |= BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE, 1 );
		}
	}
       
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

	return(result);
}

BERR_Code BXPT_SetNumPlaybackMux(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int Num 				/* [in] Number of playback mux blocks*/
	)
{
	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. The 'Num' argument is 1-based, not 0-based like everything else. */
	if( Num > BXPT_NUM_PLAYBACKS )
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "Num %u is out of range!", Num ));
		return(BERR_INVALID_PARAMETER);
	}

	hXpt->NumPlaybackMuxes = Num;

	return(BERR_SUCCESS);
}

#endif /*ENABLE_PLAYBACK_MUX*/
  
BERR_Code BXPT_GetPidChannelConfig(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get config for. */
	unsigned int *Pid, 				/* [out] The PID its using. */
	unsigned int *Band,				/* [out] The parser band the channel is mapped to. */
    bool *IsPlayback                /* [out] true if band is a playback parser, false if input */
    )
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Pid );
	BDBG_ASSERT( Band );

	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
	    uint32_t Reg, RegAddr;
    
		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        *IsPlayback = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) ? true : false;
        *Band = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );

		if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) )
		{
            *Pid = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_EN_PID_CHANNEL_SCID );
		}
		else
		{
            *Pid = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID );
		}
	}	
	
    return ExitCode;						 		  
}

BERR_Code BXPT_FreePidChannel( 
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int PidChannelNum		/* [in] PID channel to free up. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

   	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* Must be done BEFORE disabling the PID channel. */
		BXPT_P_ClearAllPidChannelDestinations( hXpt, PidChannelNum );

   		BXPT_DisablePidChannel( hXpt, PidChannelNum );
   		hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = false;
		hXpt->PidChannelTable[ PidChannelNum ].Pid = 0x2000;
		hXpt->PidChannelTable[ PidChannelNum ].IsPidChannelConfigured = false;
#ifdef ENABLE_PLAYBACK_MUX
		hXpt->PidChannelTable[ PidChannelNum ].MuxEnable = false;
		hXpt->PidChannelTable[ PidChannelNum ].HasDestination = false;
#endif /*ENABLE_PLAYBACK_MUX*/
	}

	return( ExitCode );
}

void BXPT_FreeAllPidChannels( 
	BXPT_Handle hXpt 			/* [in] Handle for this transport */
	)
{
	unsigned int i;

	BDBG_ASSERT( hXpt );

	for( i= 0; i < hXpt->MaxPidChannels; i++ )
	{
		/* Must be done BEFORE disabling the PID channel. */
		BXPT_P_ClearAllPidChannelDestinations( hXpt, i );

   		BXPT_DisablePidChannel( hXpt, i );
		hXpt->PidChannelTable[ i ].IsAllocated = false;
		hXpt->PidChannelTable[ i ].Pid = 0x2000;
		hXpt->PidChannelTable[ i ].IsPidChannelConfigured = false;
#ifdef ENABLE_PLAYBACK_MUX
		hXpt->PidChannelTable[ i ].MuxEnable = false;
		hXpt->PidChannelTable[ i ].HasDestination = false;
#endif /*ENABLE_PLAYBACK_MUX*/
	}
}

BERR_Code BXPT_RequestPidChannel(
	BXPT_Handle hXpt, 		/* [Input] Handle for this transport */
	unsigned int PidChannelNum 	/* [Output] The channel number the user wants. */ 
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )	   
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		if( hXpt->PidChannelTable[ PidChannelNum ].IsAllocated == false )
			hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = true;		
		else
			ExitCode = BXPT_ERR_PID_ALREADY_ALLOCATED;
	}

	return( ExitCode );
}


void BXPT_P_ResetTransport( 
	BREG_Handle hReg		 
	)
{
	uint32_t Reg;

	BDBG_ASSERT( hReg );

    BKNI_EnterCriticalSection();

    /* Assert the transport core reset */
	BREG_Write32( hReg, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
                             BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_xpt_sw_init_MASK );

    /* Now clear the reset. */     
	BREG_Write32( hReg, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, 
                             BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_xpt_sw_init_MASK );

	Reg = BREG_Read32( hReg, BCHP_SUN_TOP_CTRL_FINAL_SW_INIT_0_MONITOR );
	if( BCHP_GET_FIELD_DATA( Reg, SUN_TOP_CTRL_FINAL_SW_INIT_0_MONITOR, xpt_sw_init ) )
	{
		BDBG_ERR(( "XPT did not leave init state!" ));
	}

    BKNI_LeaveCriticalSection();
}

void SetChannelEnable(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool EnableIt
	)
{
	uint32_t Reg, RegAddr;

#ifdef ENABLE_PLAYBACK_MUX
	if(EnableIt && hXpt->PidChannelTable[PidChannelNum].Band > 
		BXPT_PB_PARSER((BXPT_NUM_PLAYBACKS - hXpt->NumPlaybackMuxes)))
	{
		return;
	}

	/* If the channel is enabled for playback muxing, don't disable it here. BXPT_SwitchPidChannel() will do it instead. */
	if( hXpt->PidChannelTable[PidChannelNum].MuxEnable )
	{
		return;
	}
#endif /*ENABLE_PLAYBACK_MUX*/

#if BXPT_SW7425_1323_WORKAROUND
	hXpt->PidChannelTable[PidChannelNum].IsEnabled = EnableIt;
#endif

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
   	Reg = BREG_Read32( hXpt->hRegister, RegAddr );

    /* if the pid channel is already enabled, and request is to enable it again 
       then make sure that we do not write the PID table register, this will prevent
       PID version from changing to a new value hence the buffers will not be flushed 
       by the XPT HW.This step is imortant to get a glitch free av when record.
     */
    if((Reg & BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE )) && EnableIt)
        return;

   	Reg &= ~BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE );
	if( EnableIt )
	{
		Reg |= BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE, 1 );
	}

    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
}

bool PidChannelHasDestination( 
	BXPT_Handle hXpt,                            
	unsigned int PidChannelNum 
	)
{
	uint32_t Reg, RegAddr;

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );

	if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_SPID_TABLE_i, PID_DESTINATION ) )
		return true;
	else
		return false;
}

BERR_Code BXPT_P_ClearAllPidChannelDestinations( 
	BXPT_Handle hXpt,
	unsigned int PidChannelNum
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

#ifdef ENABLE_PLAYBACK_MUX
	BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

	/* Disable PID channel */
	SetChannelEnable( hXpt, PidChannelNum, false );

	RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
   	Reg &= ~BCHP_MASK( XPT_FE_SPID_TABLE_i, PID_DESTINATION );
	BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#ifdef ENABLE_PLAYBACK_MUX
	/*leave pid table protected area*/
	BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/
	return ExitCode;
}

BERR_Code BXPT_P_SetPidChannelDestination( 
	BXPT_Handle hXpt, 
	unsigned int PidChannelNum, 
	unsigned SelectedDestination,
	bool EnableIt 
	)
{
	uint32_t Reg, RegAddr, CurrentDestinations;

	BERR_Code ExitCode = BERR_SUCCESS;

	RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	CurrentDestinations = BCHP_GET_FIELD_DATA( Reg, XPT_FE_SPID_TABLE_i, PID_DESTINATION );
   	Reg &= ~BCHP_MASK( XPT_FE_SPID_TABLE_i, PID_DESTINATION );

	if( EnableIt )
	{
		CurrentDestinations |= ( 1ul << SelectedDestination );	/* Enable the pipe */
		Reg |= BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, PID_DESTINATION, CurrentDestinations );
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

		/* TODO: Enable PID channel if it was requested */
		if( hXpt->PidChannelTable[ PidChannelNum ].EnableRequested == true )
			SetChannelEnable( hXpt, PidChannelNum, true );
	}
	else
	{
		CurrentDestinations &= ~( 1ul << SelectedDestination );	/* Clear pipe enables */	

		/* Disable PID channel if there are no other destinations selected */
		if( !CurrentDestinations )
			SetChannelEnable( hXpt, PidChannelNum, false );

		Reg |= BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, PID_DESTINATION, CurrentDestinations );
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	return ExitCode;
}

bool IsPidDuplicated(
	BXPT_Handle hXpt, 	   		/* [Input] Handle for this transport */
	unsigned int PidChannelNum 	
	)
{
	unsigned int i, TargetChannelCfg, ThisChannelCfg, ThisChannelEnable;
 	uint32_t RegAddr, Reg;

	bool IsDuplicated = false;

#ifdef ENABLE_PLAYBACK_MUX
	if(hXpt->PidChannelTable[PidChannelNum].Band > 
		BXPT_PB_PARSER((BXPT_NUM_PLAYBACKS - hXpt->NumPlaybackMuxes)))
	{
		/*there are allowed to be duplicated pids on the muxed band*/
		return(false);
	}

	/*gain access to the pid table*/
	BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

 	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
    TargetChannelCfg = BREG_Read32( hXpt->hRegister, RegAddr );

	/* This mask covers all the bitfields we need to compare. */
    TargetChannelCfg &= ( 
        BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID )
    );

	for( i = 0; i < hXpt->MaxPidChannels; i++ )
	{
		/* Skip the legitimate channel assignment. */
		if( i == PidChannelNum )
			continue;

		if( hXpt->PidChannelTable[ i ].IsPidChannelConfigured == true )
		{
 	        RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( i * PID_CHNL_STEPSIZE );
            ThisChannelCfg = Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            ThisChannelCfg &= ( 
                BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
                BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
                BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
                BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID )
            );

            /* We need to know if the channel has been enabled or has just been *requested* to be enabled */
            ThisChannelEnable = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE ) || hXpt->PidChannelTable[ i ].EnableRequested == true;

            if( TargetChannelCfg == ThisChannelCfg && ThisChannelEnable )
    		{
	        	BDBG_ERR(( "Pid channels %lu and %lu are duplicates!", i, PidChannelNum ));
				IsDuplicated = true;
				break;
			} 
		}
	}

#ifdef ENABLE_PLAYBACK_MUX
	/*leave pid table protected area*/
	BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

	return( IsDuplicated );
}

static void BXPT_P_ConfigArbiter(BREG_Handle hReg)
{
    /* Nothing specific needed yet. */
    BSTD_UNUSED( hReg ); 
}

BERR_Code BXPT_GetPidChannel_CC_Config(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get config for. */
	BXPT_PidChannel_CC_Config *Cfg
    )
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Cfg );

	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
        /* All-pass mode overrides the user settings, so return the last ones they gave before all-pass was enabled */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
        if( hXpt->IsParserInAllPass[ PidChannelNum ] )
        {
            *Cfg = hXpt->CcConfigBeforeAllPass[ PidChannelNum ];
        }
#else
		unsigned Band = hXpt->PidChannelTable[ PidChannelNum ].Band;
		bool IsAllPass = false;
		struct BXPT_P_PbHandle *hPb = NULL;

		if( BXPT_P_IS_PB(Band)==false ) {
			IsAllPass = hXpt->IsParserInAllPass[ Band ];
		}
		else {
			hPb = &hXpt->PlaybackHandles[ BXPT_P_GET_PB_BAND_NUM( Band ) ];
			IsAllPass = hPb->IsParserInAllPass;
		}

		if( IsAllPass ) {
			if( BXPT_P_IS_PB(Band)==false ) {
			    *Cfg = hXpt->CcConfigBeforeAllPass[ PidChannelNum ];
			}
			else {
				*Cfg = hPb->CcConfigBeforeAllPass;
			}
		}
#endif
        else
        {
	        uint32_t Reg, RegAddr;
    
		    RegAddr = BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_0_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		    Cfg->Primary_CC_CheckEnable = BCHP_GET_FIELD_DATA( Reg, XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PCC_ERROR_EN ) ? true : false;
		    Cfg->Secondary_CC_CheckEnable = BCHP_GET_FIELD_DATA( Reg, XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, SCC_ERROR_EN ) ? true : false;
		    Cfg->Generate_CC_Enable = BCHP_GET_FIELD_DATA( Reg, XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PROC_CC ) ? true : false;

		    RegAddr = BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_3_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		    Cfg->Initial_CC_Value = BCHP_GET_FIELD_DATA( Reg, XPT_FULL_PID_PARSER_STATE_CONFIG_3_i, PKT_INSERTED );
        }
	}	
	
    return ExitCode;						 		  
}

BERR_Code BXPT_SetPidChannel_CC_Config(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get config for. */
	const BXPT_PidChannel_CC_Config *Cfg
    )
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Cfg );

	if( PidChannelNum > hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
        /* All-pass must override the new settings, so store them until all-pass is disabled */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
        if( hXpt->IsParserInAllPass[ PidChannelNum ] )
        {
            hXpt->CcConfigBeforeAllPass[ PidChannelNum ] = *Cfg;
        }
#else
		unsigned Band = hXpt->PidChannelTable[ PidChannelNum ].Band;
		bool IsAllPass = false;
		struct BXPT_P_PbHandle *hPb = NULL;

		if( BXPT_P_IS_PB(Band)==false ) {
			IsAllPass = hXpt->IsParserInAllPass[ Band ];
		}
		else {
			hPb = &hXpt->PlaybackHandles[ BXPT_P_GET_PB_BAND_NUM(Band) ];
			IsAllPass = hPb->IsParserInAllPass;
		}

		if( IsAllPass ) {
			if( BXPT_P_IS_PB(Band)==false ) {
				hXpt->CcConfigBeforeAllPass[ PidChannelNum ] = *Cfg;
			}
			else {
				hPb->CcConfigBeforeAllPass = *Cfg;
			}
		}
#endif
        else
        {
	        uint32_t Reg, RegAddr;
    
		    RegAddr = BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_0_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		    Reg &= ~(
			    BCHP_MASK( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PCC_ERROR_EN ) |
			    BCHP_MASK( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, SCC_ERROR_EN ) |
			    BCHP_MASK( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PROC_CC ) 
		    );
		    Reg |= (
			    BCHP_FIELD_DATA( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PCC_ERROR_EN, Cfg->Primary_CC_CheckEnable ? 1 : 0 ) |
			    BCHP_FIELD_DATA( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, SCC_ERROR_EN, Cfg->Secondary_CC_CheckEnable ? 1 : 0 ) |
			    BCHP_FIELD_DATA( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PROC_CC, Cfg->Generate_CC_Enable ? 1 : 0 ) 
		    );
		    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

		    RegAddr = BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_3_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		    Reg &= ~(
			    BCHP_MASK( XPT_FULL_PID_PARSER_STATE_CONFIG_3_i, PKT_INSERTED ) 
		    );
		    Reg |= (
			    BCHP_FIELD_DATA( XPT_FULL_PID_PARSER_STATE_CONFIG_3_i, PKT_INSERTED, Cfg->Initial_CC_Value ) 
		    );
		    BREG_Write32( hXpt->hRegister, RegAddr, Reg );
        }
	}	
	
    return ExitCode;						 		  
}

#if BXPT_HAS_PARSER_REMAPPING

void BXPT_GetParserMapping(
    BXPT_Handle hXpt,           /* [in] Handle for the Transport. */ 
    BXPT_ParserBandMapping *ParserMap
    )
{
    uint32_t Reg;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ParserMap );

    *ParserMap = hXpt->BandMap;

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID );
    ParserMap->FrontEnd[ 0 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID );
    ParserMap->FrontEnd[ 0 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 1 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID );
    ParserMap->FrontEnd[ 1 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 2 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID );
    ParserMap->FrontEnd[ 2 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 3 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID );
    ParserMap->FrontEnd[ 3 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_PARSER_SEL ) ? true : false;

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID );
    ParserMap->FrontEnd[ 4 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_BAND_ID );
    ParserMap->FrontEnd[ 4 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 5 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_BAND_ID );
    ParserMap->FrontEnd[ 5 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 6 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_BAND_ID );
    ParserMap->FrontEnd[ 6 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 7 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_BAND_ID );
    ParserMap->FrontEnd[ 7 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_PARSER_SEL ) ? true : false;

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID );
    ParserMap->FrontEnd[ 4 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_BAND_ID );
    ParserMap->FrontEnd[ 4 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 5 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_BAND_ID );
    ParserMap->FrontEnd[ 5 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 6 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_BAND_ID );
    ParserMap->FrontEnd[ 6 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 7 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_BAND_ID );
    ParserMap->FrontEnd[ 7 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_PARSER_SEL ) ? true : false;

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID );
    ParserMap->FrontEnd[ 8 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_BAND_ID );
    ParserMap->FrontEnd[ 8 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 9 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_BAND_ID );
    ParserMap->FrontEnd[ 9 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_PARSER_SEL ) ? true : false;
#if BXPT_NUM_REMAPPABLE_FE_PARSERS > 10
    ParserMap->FrontEnd[ 10 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_BAND_ID );
    ParserMap->FrontEnd[ 10 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_PARSER_SEL ) ? true : false;
    ParserMap->FrontEnd[ 11 ].VirtualParserBandNum = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_BAND_ID );
    ParserMap->FrontEnd[ 11 ].VirtualParserIsPlayback = 
        BCHP_GET_FIELD_DATA( Reg, XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_PARSER_SEL ) ? true : false;
#endif

    /* ToDo: Add playback support. */
}

BERR_Code BXPT_SetParserMapping(
    BXPT_Handle hXpt,           /* [in] Handle for the Transport. */ 
    const BXPT_ParserBandMapping *ParserMap
    )
{
    uint32_t Reg;
    unsigned ii;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ParserMap );

    /* Only certain parsers can be remapped. Check for attempted remapping of parsers that don't support it. */
#if BXPT_NUM_REMAPPABLE_FE_PARSERS < BXPT_NUM_PID_PARSERS 
    for( ii = BXPT_NUM_REMAPPABLE_FE_PARSERS; ii < BXPT_NUM_PID_PARSERS; ii++ ) 
    {
        const BXPT_BandMap *Mapping;

        Mapping = &ParserMap->FrontEnd[ ii ];
        if( Mapping->VirtualParserBandNum != ii || Mapping->VirtualParserIsPlayback ) 
        {
            BDBG_ERR(( "ParserNum %lu cannot be remapped", ii ));
            return BERR_TRACE( BERR_INVALID_PARAMETER );
        }
    }
#else
	BSTD_UNUSED( ii );
#endif

    hXpt->BandMap = *ParserMap;

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID );
    Reg &= ~( 
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_PARSER_SEL ) 
    );
    Reg |= ( 
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_BAND_ID, ParserMap->FrontEnd[ 0 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER0_PARSER_SEL, ParserMap->FrontEnd[ 0 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_BAND_ID, ParserMap->FrontEnd[ 1 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER1_PARSER_SEL, ParserMap->FrontEnd[ 1 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_BAND_ID, ParserMap->FrontEnd[ 2 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER2_PARSER_SEL, ParserMap->FrontEnd[ 2 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_BAND_ID, ParserMap->FrontEnd[ 3 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, PARSER3_PARSER_SEL, ParserMap->FrontEnd[ 3 ].VirtualParserIsPlayback ? 1 : 0 ) 
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER0_TO_PARSER3_BAND_ID, Reg );

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID );
    Reg &= ~( 
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_PARSER_SEL ) 
    );
    Reg |= ( 
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_BAND_ID, ParserMap->FrontEnd[ 4 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER4_PARSER_SEL, ParserMap->FrontEnd[ 4 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_BAND_ID, ParserMap->FrontEnd[ 5 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER5_PARSER_SEL, ParserMap->FrontEnd[ 5 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_BAND_ID, ParserMap->FrontEnd[ 6 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER6_PARSER_SEL, ParserMap->FrontEnd[ 6 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_BAND_ID, ParserMap->FrontEnd[ 7 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, PARSER7_PARSER_SEL, ParserMap->FrontEnd[ 7 ].VirtualParserIsPlayback ? 1 : 0 ) 
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER4_TO_PARSER7_BAND_ID, Reg );

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID );
    Reg &= ~( 
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_PARSER_SEL ) 

#if BXPT_NUM_REMAPPABLE_FE_PARSERS > 10
        |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_PARSER_SEL ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_BAND_ID ) |
        BCHP_MASK( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_PARSER_SEL ) 
#endif
    );
    Reg |= ( 
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_BAND_ID, ParserMap->FrontEnd[ 8 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER8_PARSER_SEL, ParserMap->FrontEnd[ 8 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_BAND_ID, ParserMap->FrontEnd[ 9 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER9_PARSER_SEL, ParserMap->FrontEnd[ 9 ].VirtualParserIsPlayback ? 1 : 0 ) 
#if BXPT_NUM_REMAPPABLE_FE_PARSERS > 10
        |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_BAND_ID, ParserMap->FrontEnd[ 10 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER10_PARSER_SEL, ParserMap->FrontEnd[ 10 ].VirtualParserIsPlayback ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_BAND_ID, ParserMap->FrontEnd[ 11 ].VirtualParserBandNum ) |
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, PARSER11_PARSER_SEL, ParserMap->FrontEnd[ 11 ].VirtualParserIsPlayback ? 1 : 0 ) 
#endif
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_FE_MINI_PID_PARSER8_TO_PARSER11_BAND_ID, Reg );

	return BERR_SUCCESS;
}

#endif


uint32_t BXPT_P_GetParserCtrlRegAddr(
	BXPT_Handle hXpt,				/* [in] Handle for the transport to access. */
    unsigned ParserNum,
    unsigned Reg0
    )
{
#if BXPT_HAS_PARSER_REMAPPING
    {
        if( BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 == Reg0 || BCHP_XPT_FE_MINI_PID_PARSER0_CTRL2 == Reg0 ) 
        {
            unsigned ii;

            for (ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++) 
            {
                BXPT_BandMap *pMap = &hXpt->BandMap.FrontEnd[ ii ];

                if( pMap->VirtualParserBandNum == ParserNum && !pMap->VirtualParserIsPlayback )
                {
                    return Reg0 + ( ii * PARSER_REG_STEPSIZE );
                }
            }

            /* Had to be an invalid band number to get this far. */
            BDBG_ERR(( "Invalid virtual parser band number" ));
            return 0;
        }
        else
        {
            /* Read or write of addr 0 will probably cause a segfault, but we were passed an unsupported Reg0 anyway. */
            BDBG_ERR(( "Parser reg %lu is unsupported", Reg0 ));
            return 0;
        }
    }
#else
    BSTD_UNUSED( hXpt );
    return Reg0 + ( ParserNum * PARSER_REG_STEPSIZE );
#endif
}

static unsigned int GetParserIndex(
    BXPT_Handle hXpt,                               /* [in] Handle for the transport to access. */
    unsigned ParserNum )
{
    unsigned ii;

#if BXPT_HAS_PARSER_REMAPPING
    for (ii = 0; ii < BXPT_NUM_PID_PARSERS; ii++)
    {
        BXPT_BandMap *pMap = &hXpt->BandMap.FrontEnd[ ii ];

        if( pMap->VirtualParserBandNum == ParserNum && !pMap->VirtualParserIsPlayback )
        {
            break;
        }
    }

    if (ii == BXPT_NUM_PID_PARSERS)
            BDBG_ERR(( "Invalid virtual parser band number" ));
#else
    BSTD_UNUSED( hXpt );
    ii = ParserNum;
#endif
    return ii;
}

#if BXPT_HAS_PIPELINE_ERROR_REPORTING
BERR_Code BXPT_CheckPipelineErrors(
    BXPT_Handle hXpt
    )
{
	BERR_Code status = BERR_SUCCESS;

#if BXPT_HAS_FIXED_RSBUF_CONFIG
    status |= BXPT_P_RsBuf_ReportOverflows( hXpt );
#endif
#if BXPT_HAS_FIXED_XCBUF_CONFIG
    status |= BXPT_P_XcBuf_ReportOverflows( hXpt );
#endif

    return status ? BXPT_ERR_DATA_PIPELINE : status;
}
#endif

#if BXPT_HAS_MTSIF
bool BXPT_IsMtsifDecryptionEnabled(
    BXPT_Handle hXpt,
    unsigned channelNo
    )
{
#ifdef BCHP_XPT_FE_MTSIF_RX0_CTRL1_MTSIF_RX_DECRYPTION_ENABLE_STATUS_MASK
    uint32_t RegAddr, Reg;

    RegAddr = BCHP_XPT_FE_MTSIF_RX0_CTRL1 + channelNo * MTSIF_STEPSIZE;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    return BCHP_GET_FIELD_DATA( Reg, XPT_FE_MTSIF_RX0_CTRL1, MTSIF_RX_DECRYPTION_ENABLE_STATUS ) ? true : false;
#else
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( channelNo );
    return false;
#endif
}
#endif


/* end of file */
