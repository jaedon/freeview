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
 * $brcm_Revision: Hydra_Software_Devel/133 $
 * $brcm_Date: 7/24/12 1:04p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core65nm/bxpt.c $
 * 
 * Hydra_Software_Devel/133   7/24/12 1:04p gmullen
 * SW7405-540: Fixed compile error for 7342
 * 
 * Hydra_Software_Devel/132   5/22/12 11:27a gmullen
 * SW7405-540: Fail if app requests input parallel mode that hw does not
 * support
 * 
 * Hydra_Software_Devel/131   4/24/12 8:20a gmullen
 * SW7420-2127: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7420-2127/1   4/10/12 4:18p gmullen
 * SW7420-2127: Stored user bandrate in BXPT_XcBuf_SetBandDataRate() and
 * BXPT_RsBuf_SetBandDataRate()
 * 
 * Hydra_Software_Devel/130   3/8/12 3:45p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2497/2   3/8/12 3:20p gmullen
 * SW7425-2497: Added BXPT_HAS_PIPELINE_ERROR_REPORTING around code
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/129   9/13/11 8:53a gmullen
 * SW7408-284: Fixed compilation bug
 * 
 * Hydra_Software_Devel/128   9/12/11 5:03p gmullen
 * SW7408-284: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7408-284/2   9/7/11 4:35p gmullen
 * SW7408-284: Changed jitter settings to an array
 * 
 * Hydra_Software_Devel/SW7408-284/1   9/7/11 1:51p gmullen
 * SW7408-284: Changed jitter adjust API to 'tri-date'
 * 
 * Hydra_Software_Devel/127   5/10/11 10:01a gmullen
 * SW7405-5284: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7405-5284/1   5/10/11 9:41a gmullen
 * SW7405-5284: Added BDGB_MSGs to certain API calls
 * 
 * Hydra_Software_Devel/126   5/2/11 1:46p gmullen
 * SW7420-1828: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7420-1828/1   4/27/11 2:06p gmullen
 * SW7420-1828: Record original blockout before enabling both pipes.
 * 
 * Hydra_Software_Devel/125   4/21/11 5:34p gmullen
 * SW7420-1828: Increased bandwidth for QAM256/PAL case
 * 
 * Hydra_Software_Devel/124   3/25/11 10:40a gmullen
 * SWDTV-5976: Added 35233 support
 * 
 * Hydra_Software_Devel/123   3/23/11 2:38p gmullen
 * SW35125-19: Added missing BCHP_CHIP checks for 35125
 * 
 * Hydra_Software_Devel/122   3/2/11 4:41p gmohile
 * SW7420-1320 : Fix warnings
 * 
 * Hydra_Software_Devel/121   3/1/11 11:08a jtna
 * SW7420-1320: make BXPT_Standby dependent on .Running, not .Opened
 * 
 * Hydra_Software_Devel/120   2/17/11 11:38a jtna
 * SW7420-1320: add BXPT_Standby/BXPT_Resume
 * 
 * Hydra_Software_Devel/119   2/15/11 10:28p jtna
 * SW7420-1320: merge XPT power management
 * 
 * Hydra_Software_Devel/118   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/117   12/10/10 10:01a gmullen
 * SW7408-9: Fixed compilation warning: FeSel not used
 * 
 * Hydra_Software_Devel/116   12/6/10 7:24p gmullen
 * SW7335-895: Fixed interference between this workaround and playback
 * muxing
 * 
 * Hydra_Software_Devel/115   11/22/10 8:03p spothana
 * SW7420-1078:Update the SCB clients' values for BCM97409
 * 
 * Hydra_Software_Devel/114   11/22/10 9:08a gmullen
 * SW7335-895: Workaround for trick-mode flushing
 * 
 * Hydra_Software_Devel/113   11/10/10 5:15p gmullen
 * SW7420-1231: Ported to mainline
 * 
 * Hydra_Software_Devel/112   11/1/10 5:52p gmullen
 * SW7342-241: Ported fix to LATEST. Define SW7432_241_WORKAROUND to
 * enable it
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
#endif

#if BXPT_HAS_RSBUF
#include "bchp_xpt_rsbuff.h"
#include "bxpt_rsbuf.h"
#endif

#if BXPT_HAS_XCBUF
#include "bchp_xpt_xcbuff.h"
#include "bxpt_xcbuf.h"
#endif

#include "bmem.h" 
#include "bxpt_rave.h"
#include "bxpt_pcr_offset.h"
#include "bxpt_pcr.h"

#include "bchp_sun_top_ctrl.h"
#include "bchp_xpt_fe.h"

#if BXPT_HAS_MESG_BUFFERS
    #include "bchp_xpt_msg.h"
#endif	

#include "bchp_xpt_pb0.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_rave.h"

#if BXPT_HAS_PACKETSUB
    #include "bchp_xpt_psub.h"
    #include "bxpt_packetsub.h"
#endif

#if BXPT_HAS_REMUX
#include "bxpt_remux.h"
#endif


#if (BCHP_CHIP == 7408)
    /* Workaround for HW7408-91. */
    #include "bchp_xpt_pcroffset.h"
#endif

#if( BCHP_CHIP == 7405 ) || ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336 ) || \
    ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 3556 ) || ( BCHP_CHIP == 7550 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125) || \
	( BCHP_CHIP == 7340 ) || ( BCHP_CHIP == 35230 ) || ( BCHP_CHIP == 7420 ) || ( BCHP_CHIP == 35125 )
#include "bchp_xpt_xmemif.h"
#endif

#if ( BCHP_CHIP == 3563 )
    /* 3563 has only 1 playback. */
    #define PB_PARSER_REG_STEPSIZE	( 0x44 )    
#else
    /* Everybody else has at least 2 */
    #include "bchp_xpt_pb1.h"
    #define PB_PARSER_REG_STEPSIZE	( BCHP_XPT_PB1_CTRL1 - BCHP_XPT_PB0_CTRL1 )
#endif

#ifdef BCHP_XPT_PSUB_PSUB1_CTRL0 
#define PACKET_SUB_REGISTER_STEP	( BCHP_XPT_PSUB_PSUB1_CTRL0 - BCHP_XPT_PSUB_PSUB0_CTRL0 )
#else
#define PACKET_SUB_REGISTER_STEP	( BCHP_XPT_PSUB_PSUB0_STAT2 - BCHP_XPT_PSUB_PSUB0_CTRL0 )
#endif

/* Distance between Item X regs and Item X+1 */
#define PARSER_REG_STEPSIZE		( BCHP_XPT_FE_PARSER1_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1 )
#define IB_REG_STEPSIZE 		( 8 )
#define PID_CHNL_STEPSIZE 		( 4 )
#define SPID_CHNL_STEPSIZE 		( 4 )

/* Locally defined interrupt IDs. The RDB structure makes it impossible to generate these automatically at the moment. */
#define BCHP_INT_ID_XPT_MSG_INTR_FLAG         BCHP_INT_ID_CREATE( BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31_INTR_FLAG_SHIFT )
#define BCHP_INT_ID_XPT_MSG_OVFL_INTR_FLAG    BCHP_INT_ID_CREATE( BCHP_XPT_MSG_BUF_OVFL_INTR_00_31, BCHP_XPT_MSG_BUF_OVFL_INTR_00_31_INTR_FLAG_SHIFT )
#define MESG_FILTER_ARRAY_ELEMENTS	( 512 )

#include "bdbg.h"
BDBG_MODULE( xpt );
BDBG_OBJECT_ID(bxpt_t);

static void SetChannelEnable( BXPT_Handle hXpt, unsigned int PidChannelNum, bool EnableIt );
static bool PidChannelHasDestination( BXPT_Handle hXpt, unsigned int PidChannelNum );
static bool IsPidDuplicated( BXPT_Handle hXpt, unsigned int PidChannelNum );
static void BXPT_P_ConfigArbiter(BREG_Handle hReg);		 

BERR_Code BXPT_GetDefaultSettings(
	BXPT_DefaultSettings *Defaults,	/* [out] Defaults to use during init.*/
	BCHP_Handle hChip			 	/* [in] Handle to used chip. */
	)
{
    unsigned ii;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( Defaults );
	BSTD_UNUSED( hChip );

    BKNI_Memset( ( void * ) &Defaults->DramBuffers, 0, sizeof( BXPT_DramBufferCfg ) );

#if BXPT_HAS_IB_PID_PARSERS
    for( ii = 0; ii < BXPT_P_MAX_PID_PARSERS; ii++ )
        Defaults->DramBuffers.IbParserRsSize[ ii ] = 200;
#endif

    for( ii = 0; ii < BXPT_P_MAX_PLAYBACKS; ii++ )
        Defaults->DramBuffers.PbParserRsSize[ ii ] = 8;

#if BXPT_HAS_XCBUF
    for( ii = 0; ii < BXPT_P_MAX_PID_PARSERS; ii++ )
        Defaults->DramBuffers.RaveXcCfg.IbParserXcSize[ ii ] = 200;

    for( ii = 0; ii < BXPT_P_MAX_PLAYBACKS; ii++ )
        Defaults->DramBuffers.RaveXcCfg.PbParserXcSize[ ii ] = 8;

    for( ii = 0; ii < BXPT_P_MAX_PID_PARSERS; ii++ )
        Defaults->DramBuffers.MesgBufXcCfg.IbParserXcSize[ ii ] = 8;

    for( ii = 0; ii < BXPT_P_MAX_PLAYBACKS; ii++ )
        Defaults->DramBuffers.MesgBufXcCfg.PbParserXcSize[ ii ] = 8;

    #if !( BCHP_CHIP == 7440 )
        for( ii = 0; ii < BXPT_P_MAX_REMULTIPLEXORS; ii++ )
        {
            Defaults->DramBuffers.RemuxXcCfg[ ii ].BandAXcSize = 200;
            Defaults->DramBuffers.RemuxXcCfg[ ii ].BandBXcSize = 200;
        }
    #endif
#endif

#if ( BCHP_CHIP == 3563 )
    Defaults->EnableMpodOut = false;
#endif

    Defaults->hRHeap = NULL;
    Defaults->hPbHeap = NULL;
    Defaults->MesgDataOnRPipe = false;

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

	uint32_t Reg = 0;
	uint32_t RegAddr = 0;
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
    BDBG_OBJECT_SET(lhXpt, bxpt_t);

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
#endif

	/* Reset the hardware. */
	BXPT_P_ResetTransport( hRegister );

    /* set arbiter settings for 7401, remove it when CFE sets these values for 7401 */
#if ( BCHP_CHIP == 7401 ) || ( BCHP_CHIP == 7440 ) || ( BCHP_CHIP == 7400 ) || ( BCHP_CHIP == 3563 ) \
	|| ( BCHP_CHIP == 35230 ) || ( BCHP_CHIP == 7420 ) || (BCHP_CHIP == 7340) || (  BCHP_CHIP == 7342 ) || ( BCHP_CHIP == 35125 )
    BXPT_P_ConfigArbiter( hRegister );
#else
    BSTD_UNUSED( BXPT_P_ConfigArbiter );
#endif

	/* Set the number of resources this transport has. */
	lhXpt->MaxPlaybacks = BXPT_P_MAX_PLAYBACKS; 
	lhXpt->MaxPidChannels = BXPT_P_MAX_PID_CHANNELS; 
#if BXPT_HAS_IB_PID_PARSERS
	lhXpt->MaxPidParsers = BXPT_P_MAX_PID_PARSERS; 
	lhXpt->MaxInputBands = BXPT_P_MAX_INPUT_BANDS; 
#endif
	lhXpt->MaxTpitPids = BXPT_P_MAX_TPIT_PIDS; 
#if BXPT_HAS_MESG_BUFFERS
	lhXpt->MaxFilterBanks = BXPT_P_MAX_FILTER_BANKS; 
	lhXpt->MaxFiltersPerBank = BXPT_P_MAX_FILTERS_PER_BANK; 
#endif
#if BXPT_HAS_PACKETSUB
	lhXpt->MaxPacketSubs = BXPT_P_MAX_PACKETSUBS;
#endif
#if BXPT_HAS_DPCRS
	lhXpt->MaxPcrs = BXPT_P_MAX_PCRS;
#endif
	lhXpt->MaxRaveContexts = BXPT_P_MAX_RAVE_CONTEXTS; 

#ifdef ENABLE_PLAYBACK_MUX
	/* By default, use one playback block for muxing. */
	lhXpt->NumPlaybackMuxes = 1; 
#endif
 
 	/* Create and init the PID channel table. */
	for( i = 0; i < lhXpt->MaxPidChannels; i++ )
	{
		/* 
		** On some devices, not all PID channels have a message buffer.	HasMessageBuffer 
		** will be updated below, when we init the message buffer table
		*/
#ifdef ENABLE_PLAYBACK_MUX
		PidChannelTableEntry InitEntry = { false, false, false, 0, 0, false, false, false, 0 };
#else /*ENABLE_PLAYBACK_MUX*/
		PidChannelTableEntry InitEntry = { false, false, false, 0, 0, false, 0 };
#endif /*ENABLE_PLAYBACK_MUX*/

		lhXpt->PidChannelTable[ i ] = InitEntry;

		BREG_Write32( hRegister, BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( 4 * i ), 1 << 28 );
		BREG_Write32( hRegister, BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
	}

#if BXPT_HAS_IB_PID_PARSERS
	/* Init the PID parsers. */
	for( i = 0; i < lhXpt->MaxPidParsers; i++ )
	{
		#if (BCHP_CHIP == 7342)
		BREG_Write32( hRegister, BCHP_XPT_FE_PARSER0_CTRL2 + BXPT_P_GetParserRegOffset(i), 0 );
		#else
		BREG_Write32( hRegister, BCHP_XPT_FE_PARSER0_CTRL2 + ( PARSER_REG_STEPSIZE * i ), 0 );
		#endif
	}
#endif

#if BXPT_HAS_MESG_BUFFERS
	/* Create and init the message buffer table. */
	for( i = 0; i <	BXPT_P_MAX_MESG_BUFFERS; i++ )
	{
		MessageBufferEntry InitEntry = { false, 0, 0 };
		lhXpt->MessageBufferTable[ i ] = InitEntry;	

   		lhXpt->MesgBufferIsInitialized[ i ] = false;

		lhXpt->PidChannelTable[ i ].HasMessageBuffer = true;
   		lhXpt->PidChannelTable[ i ].MessageBuffercount = 0;

		lhXpt->MesgIntrCallbacks[ i ].Callback = ( BINT_CallbackFunc ) NULL;
		lhXpt->OverflowIntrCallbacks[ i ].Callback = ( BINT_CallbackFunc ) NULL;

#if BXPT_HAS_PID2BUF_MAPPING
		BREG_Write32( hRegister, BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
#else
		BREG_Write32( hRegister, BCHP_XPT_MSG_PID_CTRL1_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_PID_CTRL2_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
#endif
		BREG_Write32( hRegister, BCHP_XPT_MSG_DMA_BP_TABLE_i_ARRAY_BASE + ( 4 * i ), 0 );
		BREG_Write32( hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( 4 * i ), 0 );

        /* for normal legacy mode set false, set true to override settings */
        lhXpt->PidChannelParserConfigOverride[i] = false;
   	}

	lhXpt->Pid2BuffMappingOn = false;

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

#if BXPT_HAS_IB_PID_PARSERS
	/* 
	** All parser bands initially do NOT modify PSI messages in the DMA buffers. 
	** This is the hardware default.
	*/
	for( i = 0; i < BXPT_P_MAX_PID_PARSERS; i++ )
	{
#if BXPT_HAS_MESG_BUFFERS
		ParserConfig ParserInit = { false, false, false, BXPT_PsiMesgModModes_eNoMod };

		lhXpt->IbParserTable[ i ] = ParserInit;
#endif

		/* Map all PIDs to the RAVE based on PID channel number. */
		#if (BCHP_CHIP == 7342)                                           
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL2 + BXPT_P_GetParserRegOffset(i);
		#else                                                             
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL2 + ( i * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( lhXpt->hRegister, RegAddr );
		Reg &= ~( BCHP_MASK( XPT_FE_PARSER0_CTRL2, RAVE_CX_MODE ) );
		Reg |= ( BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL2, RAVE_CX_MODE, 1 ));
		BREG_Write32( lhXpt->hRegister, RegAddr, Reg );
	}                        
#endif

	/* Each playback has a hard-wired PID parser. So init the table for those too. */
	for( i = 0; i < BXPT_P_MAX_PLAYBACKS; i++ )
	{
		ParserConfig ParserInit = { false, false, false, BXPT_PsiMesgModModes_eNoMod };

		lhXpt->PbParserTable[ i ] = ParserInit;

		/* Map all PIDs to the RAVE based on PID channel number. */
		RegAddr = BCHP_XPT_PB0_PARSER_CTRL2 + ( i * PB_PARSER_REG_STEPSIZE );
		Reg = BREG_Read32( lhXpt->hRegister, RegAddr );
		Reg &= ~( BCHP_MASK( XPT_PB0_PARSER_CTRL2, RAVE_CX_MODE ) );
		Reg |= ( BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL2, RAVE_CX_MODE, 1 ));
		BREG_Write32( lhXpt->hRegister, RegAddr, Reg );
	}

	/* Init the RAVE structure in the xpt handle. */
	for( i = 0; i < BXPT_P_MAX_RAVE_CHANNELS; i++ )
	{
		lhXpt->RaveChannels[ i ].Allocated = false;
	}

	/* Init the RAVE structure in the xpt handle. */
	for( i = 0; i < BXPT_P_MAX_PCR_OFFSET_CHANNELS; i++ )
	{
		lhXpt->PcrOffsets[ i ].Handle = NULL;
		lhXpt->PcrOffsets[ i ].Allocated = false;
	}
                                    
#if BCHP_CHIP == 7400 && BCHP_VER == BCHP_VER_A0 && BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
    /* 7400A0 is the oddball. It does NOT follow the chip-level endianess signal. Everyone else does. */
	Reg = BREG_Read32( hRegister, BCHP_XPT_MSG_MSG_CTRL1 );
	Reg |= BCHP_FIELD_DATA( XPT_MSG_MSG_CTRL1, ENDIAN_CTRL, 1 );
	BREG_Write32( hRegister, BCHP_XPT_MSG_MSG_CTRL1, Reg );
#endif

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

#if BXPT_HAS_RSBUF
	ExitCode = BERR_TRACE( BXPT_P_RsBuf_Init( lhXpt, &Defaults->DramBuffers ) );
	if( ExitCode != BERR_SUCCESS )
	{
		BDBG_ERR(("Rate smoothing buffer init FAILED!"));
		goto done;
	}
#endif

#if BXPT_HAS_XCBUF
	ExitCode = BERR_TRACE( BXPT_P_XcBuf_Init( lhXpt, &Defaults->DramBuffers ) );
	if( ExitCode != BERR_SUCCESS )
	{
		BDBG_ERR(("XPT client buffer init FAILED!"));
		goto done;
	}
#endif

#if ( BCHP_CHIP == 3563 )
	Reg = BREG_Read32( lhXpt->hRegister, BCHP_XPT_BUS_IF_MISC_CTRL0 );
	Reg &= ~( BCHP_MASK( XPT_BUS_IF_MISC_CTRL0, RMXP_MPOD_MUX_SEL ) );
	Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_MISC_CTRL0, RMXP_MPOD_MUX_SEL, Defaults->EnableMpodOut == true ? 1 : 0 ));
	BREG_Write32( lhXpt->hRegister, BCHP_XPT_BUS_IF_MISC_CTRL0, Reg );
#endif

#if ( BCHP_CHIP == 35230 ) || ( BCHP_CHIP == 35125 )
/* This bit isn't in the RDB */
{
    uint32_t reg = BREG_Read32( lhXpt->hRegister, 0x00770000 + 0x00000048 );
    BREG_Write32( lhXpt->hRegister, 0x00770000 + 0x00000048, reg & 0xFFFDFFFF );
}
#endif

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    {
        unsigned Index;

        for( Index = 0; Index < BXPT_P_MAX_PCRS; Index++ )
        {
            lhXpt->JitterTimestamp[ Index ] = BXPT_PCR_JitterTimestampMode_eAuto; 
            lhXpt->PbJitterDisable[ Index ] = BXPT_PCR_JitterCorrection_eAuto;
            lhXpt->LiveJitterDisable[ Index ] = BXPT_PCR_JitterCorrection_eAuto;
        }
    }
#endif

	done:
	*hXpt = lhXpt;
	BDBG_LEAVE( BXPT_Open );

#ifdef BCHP_PWR_RESOURCE_XPT
    /* turn off only the 216M */
    BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_XPT_XMEMIF);

    /* a failed BXPT_Open powers down the 108M as well */
    if (ExitCode!=BERR_SUCCESS) {
        BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_XPT_108M);
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

#if BXPT_HAS_IB_PID_PARSERS
    for( Index = 0; Index < hXpt->MaxPidParsers; Index++ )
	    BXPT_SetParserEnable( hXpt, Index, false );
#endif

#if BXPT_HAS_RSBUF
	Res = BXPT_P_RsBuf_Shutdown( hXpt );
	BDBG_ASSERT( Res == BERR_SUCCESS );
#endif

#if BXPT_HAS_XCBUF
	Res = BXPT_P_XcBuf_Shutdown( hXpt );
	BDBG_ASSERT( Res == BERR_SUCCESS );
#endif

	for( Index = 0; Index < BXPT_P_MAX_RAVE_CHANNELS; Index++ )
		if( hXpt->RaveChannels[ Index ].Allocated == true )
		{
			Res = BXPT_Rave_CloseChannel( ( BXPT_Rave_Handle ) hXpt->RaveChannels[ Index ].Handle );
			BDBG_ASSERT( Res == BERR_SUCCESS );
		}
	
	for( Index = 0; Index < BXPT_P_MAX_PCR_OFFSET_CHANNELS; Index++ )
		if( hXpt->PcrOffsets[ Index ].Allocated == true )  
		{
			Res = BXPT_PcrOffset_Close( ( BXPT_PcrOffset_Handle ) hXpt->PcrOffsets[ Index ].Handle );
			BDBG_ASSERT( Res == BERR_SUCCESS );
		}

	for ( Index = 0; Index < BXPT_P_MAX_PLAYBACKS; Index++ ) {
		if ( hXpt->PlaybackHandles[Index].Opened == true ) {
			BXPT_Playback_CloseChannel( (BXPT_Playback_Handle) &hXpt->PlaybackHandles[Index] );
		}
	}

#if BXPT_HAS_PACKETSUB
	for ( Index = 0; Index < BXPT_P_MAX_PACKETSUBS; Index++ ) {
		if ( hXpt->PacketSubHandles[Index].Opened == true ) {
			BXPT_PacketSub_CloseChannel( (BXPT_PacketSub_Handle) &hXpt->PacketSubHandles[Index]);            
		}
	}
#endif

#if BXPT_HAS_REMUX
	for ( Index = 0; Index < BXPT_P_MAX_REMULTIPLEXORS; Index++ ) {
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
	for( Index = 0; Index < BXPT_P_MAX_MESG_BUFFERS; Index++ )
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
    /* set arbiter settings for 7401, remove it when CFE sets these values for 7401 */
#if ( BCHP_CHIP == 7401 )
    BXPT_P_ConfigArbiter( hXpt->hRegister );
#endif

#ifdef BCHP_PWR_RESOURCE_XPT
	/* release the 108M after BXPT_P_ResetTransport */
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_108M);
#endif

	/* Dont need the transport handle any more. */
    BDBG_OBJECT_DESTROY(hXpt, bxpt_t);
	BKNI_Free( hXpt );
}

void BXPT_GetDefaultStandbySettings(
    BXPT_StandbySettings *pSettings
    )
{
	BSTD_UNUSED(pSettings);
}

BERR_Code BXPT_Standby(
	BXPT_Handle hXpt,
	BXPT_StandbySettings *pSettings
	)
{
#ifdef BCHP_PWR_RESOURCE_XPT
	unsigned int Index;
#endif
	BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BSTD_UNUSED(pSettings);    

#ifdef BCHP_PWR_RESOURCE_XPT
	if (hXpt->bStandby) {
		BDBG_ERR(("Already in standby"));
		return BERR_UNKNOWN;
	}
    
	/* check if XPT is still in use. if so, we cannot enter standby */

#if BXPT_HAS_IB_PID_PARSERS  
	for( Index = 0; Index < hXpt->MaxPidParsers; Index++ ) {
	        uint32_t Reg, RegAddr;
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(Index);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( Index * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		if (BCHP_GET_FIELD_DATA(Reg, XPT_FE_PARSER0_CTRL1, PARSER_ENABLE)) {
			BDBG_ERR(("One or more parsers still enabled. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}
#endif

	for ( Index = 0; Index < BXPT_P_MAX_PLAYBACKS; Index++ ) {
		if ( hXpt->PlaybackHandles[Index].Running == true ) {
			BDBG_ERR(("One or more playback channels still running. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}

#if BXPT_HAS_PACKETSUB
	for ( Index = 0; Index < BXPT_P_MAX_PACKETSUBS; Index++ ) {
		if ( hXpt->PacketSubHandles[Index].Running == true ) {
			BDBG_ERR(("One or more packetsub channels still running. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}
#endif

#if BXPT_HAS_REMUX
	for ( Index = 0; Index < BXPT_P_MAX_REMULTIPLEXORS; Index++ ) {
		if ( hXpt->RemuxHandles[Index].Running == true ) {
			BDBG_ERR(("One or more remux channels still running. Cannot enter standby"));
			return BERR_UNKNOWN;
		}
	}
#endif

	/* if we get to this point, then XPT is not in use, and all clocks except the 108M 
	   were dynamically turned off earlier. XPT can now release the 108M */
	BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_108M);
	hXpt->bStandby = true;
#endif /* #ifdef BCHP_PWR_RESOURCE_XPT */

	return BERR_SUCCESS;
}

BERR_Code BXPT_Resume(
	BXPT_Handle hXpt
	)
{
	BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

#ifdef BCHP_PWR_RESOURCE_XPT
	if (!hXpt->bStandby) {
		BDBG_ERR(("Not in standby"));
		return BERR_UNKNOWN;        
	}
	BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_108M);
	hXpt->bStandby = false;
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
	Capability->MaxPlaybacks = BXPT_P_MAX_PLAYBACKS; 
	Capability->MaxPidChannels = BXPT_P_MAX_PID_CHANNELS; 
#if BXPT_HAS_IB_PID_PARSERS
	Capability->MaxPidParsers = BXPT_P_MAX_PID_PARSERS; 
	Capability->MaxInputBands = BXPT_P_MAX_INPUT_BANDS; 
#endif
	Capability->MaxTpitPids = BXPT_P_MAX_TPIT_PIDS;
#if BXPT_HAS_MESG_BUFFERS	 
	Capability->MaxFilterBanks = BXPT_P_MAX_FILTER_BANKS;                        
	Capability->MaxFiltersPerBank = BXPT_P_MAX_FILTERS_PER_BANK; 
#endif
#if BXPT_HAS_PACKETSUB
	Capability->MaxPacketSubs = BXPT_P_MAX_PACKETSUBS;
#endif
#if BXPT_HAS_DPCRS
	Capability->MaxPcrs = BXPT_P_MAX_PCRS;
#endif
	Capability->MaxRaveContexts = BXPT_P_MAX_RAVE_CONTEXTS; 
}


#if BXPT_HAS_IB_PID_PARSERS

#if (BCHP_CHIP == 7342)
int BXPT_P_GetParserRegOffset(int parserIndex)
{
	switch (parserIndex) {
	case 0:
		return(BCHP_XPT_FE_PARSER0_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1);
	case 1:
		return(BCHP_XPT_FE_PARSER1_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1);
	case 2:
        return(BCHP_XPT_FE_PARSER2_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1);
	case 3:
		return(BCHP_XPT_FE_PARSER3_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1);
	case 4:
		return(BCHP_XPT_FE_PARSER4_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1);
	case 5:
		return(BCHP_XPT_FE_PARSER5_CTRL1 - BCHP_XPT_FE_PARSER0_CTRL1);
	default:
		BDBG_ERR(("Parser# %d does not exist in RDB", parserIndex));
		return 0;
	}
}
#endif

BERR_Code BXPT_GetParserConfig(
	BXPT_Handle hXpt,				/* [in] Handle for the transport to access. */
	unsigned int ParserNum,				/* [in] Which parser band to access. */
	BXPT_ParserConfig *ParserConfig	/* [out] The current settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( ParserConfig );

	/* Is the parser number within range? */
	if( ParserNum >= hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );                                                 
	}
	else
	{
		uint32_t Timebase = 0;

		/* The parser config registers are at consecutive addresses. */
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 +  BXPT_P_GetParserRegOffset(ParserNum);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserNum * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		ParserConfig->ErrorInputIgnore = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE );
		ParserConfig->ContCountIgnore = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_CONT_COUNT_IGNORE );
		ParserConfig->AcceptNulls = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT );
		ParserConfig->AcceptAdapt00 = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_ACCEPT_ADAPT_00 );

		/* The location of these bits moved from the 7038. So we store flags in the handle. */
		ParserConfig->SaveShortPsiMsg = hXpt->IbParserTable[ ParserNum ].SaveShortPsiMsg;
		ParserConfig->SaveLongPsiMsg = hXpt->IbParserTable[ ParserNum ].SaveLongPsiMsg;
		ParserConfig->PsfCrcDis = hXpt->IbParserTable[ ParserNum ].PsfCrcDis;
		ParserConfig->PsiMod = hXpt->IbParserTable[ ParserNum ].PsiMsgMod;

		ParserConfig->TsMode = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE );

		Timebase = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_TIMEBASE_SEL );
		if( Timebase == 0 )
			ParserConfig->UsePcrTimeBase	= false; 
		else if( Timebase > BXPT_P_MAX_PCRS )
		{
		 	BDBG_ERR(( "Invalid timebase %u configured in hardware.", ( unsigned long ) Timebase ));
			ParserConfig->UsePcrTimeBase	= false; 
		}
		else
		{
			ParserConfig->UsePcrTimeBase = true; 
			ParserConfig->WhichPcrToUse = Timebase - 1; 
		}
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( ParserConfig );

	/* Is the parser number within range? */
	if( ParserNum >= hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		ParserConfig->ErrorInputIgnore = false;
		ParserConfig->ContCountIgnore = false;
		ParserConfig->AcceptNulls = false;
		ParserConfig->AcceptAdapt00 = false;

		ParserConfig->UsePcrTimeBase = false; 
		ParserConfig->WhichPcrToUse = 0;

		/* The location of these bits moved from the 7038. So we store flags in the handle. */
		ParserConfig->SaveShortPsiMsg = false;
		ParserConfig->SaveLongPsiMsg = false;
		ParserConfig->PsfCrcDis = false;
		ParserConfig->PsiMod = BXPT_PsiMesgModModes_eNoMod;
		ParserConfig->TsMode = BXPT_ParserTimestampMode_eAutoSelect;
	}

	return( ExitCode );
}

BERR_Code BXPT_SetParserConfig(
	BXPT_Handle hXpt,						/* [in] Handle for the transport to access. */
	unsigned int ParserNum,				/* [in] Which parser band to access. */
	const BXPT_ParserConfig *ParserConfig	/* [in] The new settings */
	)
{
	uint32_t Reg, RegAddr, TimeBaseSel;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( ParserConfig );

	/* Is the parser number within range? */
	if( ParserNum >= hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(ParserNum);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserNum * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		/* Clear all the bits we are about to change. */
		Reg &= ~(
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE ) |
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_CONT_COUNT_IGNORE ) |
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE ) |
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_TIMEBASE_SEL ) |
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT ) |
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_ACCEPT_ADAPT_00 ) 
		);

		/* Now set the new values. */
		Reg |= (
			BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE, ParserConfig->ErrorInputIgnore ) |
			BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_CONT_COUNT_IGNORE, ParserConfig->ContCountIgnore ) |
			BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE, ParserConfig->TsMode ) |
			BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_ACCEPT_NULL_PKT, ParserConfig->AcceptNulls ) |
			BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_ACCEPT_ADAPT_00, ParserConfig->AcceptAdapt00 ) 
		);

        /* Store the ContCountIgnore setting only if we're in MPEG mode. */
        if( !BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_PACKET_TYPE ) )
        {
            hXpt->InputParserContCountIgnore[ ParserNum ] = ParserConfig->ContCountIgnore;
        }

		if( ParserConfig->UsePcrTimeBase == false )
		{
			/* Use the free running 27 MHz clock. */
			TimeBaseSel = 0;
		}
		else
		{
			TimeBaseSel = ParserConfig->WhichPcrToUse + 1;
		}
		Reg |= (
			BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_TIMEBASE_SEL, TimeBaseSel ) 
		);

		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_MESG_BUFFERS
		hXpt->IbParserTable[ ParserNum ].SaveShortPsiMsg = ParserConfig->SaveShortPsiMsg;
		hXpt->IbParserTable[ ParserNum ].SaveLongPsiMsg = ParserConfig->SaveLongPsiMsg; 
		hXpt->IbParserTable[ ParserNum ].PsfCrcDis = ParserConfig->PsfCrcDis;
		hXpt->IbParserTable[ ParserNum ].PsiMsgMod = ParserConfig->PsiMod;
		
		/* 
		** The PSI settings above are now done in the message filter block. Make sure all the
		** filters get the new settings.
		*/
		BXPT_P_ApplyParserPsiSettings( hXpt, ParserNum, false );
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( InputBandConfig );

	/* Is the parser number within range? */
	if( BandNum >= hXpt->MaxInputBands )
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

        InputBandConfig->ParallelInputSel = false;
#if ( BCHP_CHIP == 7325 )
    if ( BandNum == 2 )
        InputBandConfig->ParallelInputSel = true;
#elif ( BCHP_CHIP == 7340 )
    if (( BandNum == 2 ) || ( BandNum == 3 ))
        InputBandConfig->ParallelInputSel = true;
#elif ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  )  
    if (( BandNum == 2 ) || ( BandNum == 3 ) || ( BandNum == 5 ))
        InputBandConfig->ParallelInputSel = true;
#elif ( BCHP_CHIP == 7342 )  
    if ( BandNum == 4 )
        InputBandConfig->ParallelInputSel = true;
#endif
	}

	return( ExitCode );
}

static bool GetParallelSetting( 
	unsigned int BandNum, 
    uint32_t Reg
    )
{
#if ( BCHP_CHIP == 7325 )
		if ( BandNum == 2 || BandNum == 4 )        
			return BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) ? true : false;
        else 
            return false;
#elif ( BCHP_CHIP == 7340 )
		if (( BandNum == 2 ) || ( BandNum == 3 ))
			return BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) ? true : false;
        else 
            return false;
#elif ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  )  
		if (( BandNum >= 2 ) || ( BandNum <= 5 ))
			return BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) ? true : false;
        else 
            return false;
#elif ( BCHP_CHIP == 7342 )
        BSTD_UNUSED( Reg );
		if( BandNum == 4 )	/* Only band 4 has parallel input support. */ 
            return true;    /* Parallel is forced on by hardware */
        else 
            return false;
#else
		if( BandNum == 4 )	/* Only band 4 has parallel input support. */ 
			return BCHP_GET_FIELD_DATA( Reg, XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) ? true : false;
        else 
            return false;
#endif
}

BERR_Code BXPT_GetInputBandConfig(
	BXPT_Handle hXpt,						/* [in] Handle for the transport to access. */
	unsigned int BandNum,						/* [in] Which input band to access. */
	BXPT_InputBandConfig *InputBandConfig	/* [in] The current settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( InputBandConfig );

	/* Is the parser number within range? */
	if( BandNum >= hXpt->MaxInputBands )
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

        InputBandConfig->ParallelInputSel = GetParallelSetting( BandNum, Reg );
	}

	return( ExitCode );
}

static void SetParallelMode( 
	unsigned int BandNum, 
    uint32_t *Reg,
	const BXPT_InputBandConfig *IbCfg
    )
{
#if ( BCHP_CHIP != 7342 )
	*Reg &= ~( BCHP_MASK( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL ) );
#endif

#if ( BCHP_CHIP == 7342 )
    if ( 4 == BandNum )
    {
        if( !IbCfg->ParallelInputSel )
        {
		    BDBG_ERR(( "BandNum %lu does not support serial mode", BandNum ));
            BKNI_Fail();
        }
    }
    else 
    {
        if( IbCfg->ParallelInputSel )
        {
		    BDBG_ERR(( "BandNum %lu does not support parallel mode", BandNum ));
            BKNI_Fail();
        }
    }
#elif ( BCHP_CHIP == 7325 )
    if ( 2 == BandNum || 4 == BandNum )
    {
        *Reg |= BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL, IbCfg->ParallelInputSel == true ? 1 : 0 );
    }
    else 
    {
        if( IbCfg->ParallelInputSel )
        {
		    BDBG_ERR(( "BandNum %lu does not support parallel mode", BandNum ));
            BKNI_Fail();
        }
    }
#elif ( BCHP_CHIP == 7340 )
    if ( BandNum >= 2 || BandNum <= 4 )
    {
        *Reg |= BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL, IbCfg->ParallelInputSel == true ? 1 : 0 );
    }
    else 
    {
        if( IbCfg->ParallelInputSel )
        {
		    BDBG_ERR(( "BandNum %lu does not support parallel mode", BandNum ));
            BKNI_Fail();
        }
    }
#elif ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  )  
	if (( 2 == BandNum ) || ( 3 == BandNum ) || ( 5 == BandNum ))
    {
        *Reg |= BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL, IbCfg->ParallelInputSel == true ? 1 : 0 );
    }
    else 
    {
        if( IbCfg->ParallelInputSel )
        {
		    BDBG_ERR(( "BandNum %lu does not support parallel mode", BandNum ));
            BKNI_Fail();
        }
    }
#else
	if( BandNum == 4 )	/* Only band 4 has parallel input support. */ 
    {
        *Reg |= BCHP_FIELD_DATA( XPT_FE_IB0_CTRL, IB_PARALLEL_INPUT_SEL, IbCfg->ParallelInputSel == true ? 1 : 0 );
    }
    else 
    {
        if( IbCfg->ParallelInputSel )
        {
		    BDBG_ERR(( "BandNum %lu does not support parallel mode", BandNum ));
            BKNI_Fail();
        }
    }
#endif
}

BERR_Code BXPT_SetInputBandConfig(
	BXPT_Handle hXpt,							/* [in] Handle for the transport to access. */
	unsigned int BandNum,						/* [in] Which input band to access. */
	const BXPT_InputBandConfig *InputBandConfig	/* [in] The new settings */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( InputBandConfig );

	/* Is the parser number within range? */
	if( BandNum >= hXpt->MaxInputBands )
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

        SetParallelMode( BandNum, &Reg, InputBandConfig );

		/* Now set the new values. */
		Reg |= (	
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

	return( ExitCode );
}

BERR_Code BXPT_ParserAllPassMode( 
	BXPT_Handle hXpt,	/* [in] Handle for the transport to access. */
	unsigned int ParserNum,						/* [in] Which input band to access. */
	bool AllPass		/* [in] All-pass enabled if true, or not if false. */
	)
{
	uint32_t Reg, RegAddr,PacketLen;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(ParserNum);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserNum * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

        if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_PACKET_TYPE)== 1)
            PacketLen = 130;
        else                                                               
            PacketLen = 188;
		
		/* Clear all the bits we are about to change. */
		Reg &= ~(
			BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL ) 
		);

		/* Now set the new values. */
        if(AllPass)
        {
            Reg |= BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL, 1 );

#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF 
            /* Save the old blockout values */
            hXpt->RsBufBO[ ParserNum ] = BXPT_P_RsBuf_GetBlockout( hXpt, ParserNum );
            hXpt->XcBufBO[ BXPT_XcBuf_Id_RAVE_IBP0 + ParserNum ] = BXPT_P_XcBuf_GetBlockout( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserNum );

            BXPT_P_RsBuf_SetBlockout( hXpt, ParserNum, BXPT_P_RsBuf_ComputeBlockOut( 81000000, PacketLen ) );
            BXPT_P_XcBuf_SetBlockout( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserNum, BXPT_P_RsBuf_ComputeBlockOut( 81000000, PacketLen ) );

#if ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  ) ||  ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 3556 )	|| ( BCHP_CHIP == 7342 )\
    || ( BCHP_CHIP == 7340 )
            BXPT_P_XcBuf_SetBlockout( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserNum, BXPT_P_RsBuf_ComputeBlockOut( 81000000, PacketLen ) );
#endif

            BDBG_WRN(("***************************************************************************************"));
            BDBG_WRN(("* WARNING: The max bitrate for the all-pass parser band has been increased to 81 Mbps. "));
            BDBG_WRN(("* Check your usage mode.Software will decrease this when all pass mode is disabled. "));
            BDBG_WRN(("***************************************************************************************"));
        }
        else
        {
            /* Adjust Rave data read BW */
            BXPT_P_RsBuf_SetBlockout( hXpt, ParserNum, hXpt->RsBufBO[ ParserNum ] );
            BXPT_P_XcBuf_SetBlockout( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserNum, hXpt->XcBufBO[ BXPT_XcBuf_Id_RAVE_IBP0 + ParserNum ] );
#endif
        }

		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
        
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Is the parser number within range? */
	if( ParserNum >= hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	/* Is the requested DataSource valid? */
	else if( DataSource == BXPT_DataSource_eInputBand && WhichSource >= hXpt->MaxInputBands )
	{
		/* Requested an input band we dont support. Complain. */
		BDBG_ERR(( "WhichSource %lu is out of range!. Not enough input bands.", WhichSource ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(ParserNum);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserNum * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		
		/* Clear all the bits we are about to change. */
		Reg &= ~( BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_INPUT_SEL ) );

		/* Now set the new values. */																											 
		switch( DataSource )
		{
			/* Values for input band selection start at 0 and are sequential. */
			case BXPT_DataSource_eInputBand: 
			Reg |= ( BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_INPUT_SEL, WhichSource ) );
			break;

			/* Values for remux feedback selection start at 0x0E and are sequential. */
			case BXPT_DataSource_eRemuxFeedback: 
			Reg |= ( BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_INPUT_SEL, 0x0E + WhichSource ) );
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Is the parser number within range? */
	if( ParserNum >= hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(ParserNum);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserNum * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		InputBits = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_INPUT_SEL );
		
		switch( InputBits )
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			*DataSource	= BXPT_DataSource_eInputBand;
			*WhichSource = InputBits;
			break;

			/* Remux feedback paths start at 0x0E in the RDB */
			case 0x0E:
			case 0x0F:
			*DataSource	= BXPT_DataSource_eRemuxFeedback;
			*WhichSource = InputBits - 0x0E;
			break;

			default:
			BDBG_ERR(( "DataSource %lu is out of range!", InputBits ));
			ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
			break;
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Is the parser number within range? */
	if( ParserNum >= hXpt->MaxPidParsers )
	{
		/* Bad parser number. Complain. */
		BDBG_ERR(( "ParserNum %lu is out of range!", ParserNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(ParserNum);
		#else
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserNum * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#ifdef BCHP_PWR_RESOURCE_XPT_PARSER
		wasEnabled = BCHP_GET_FIELD_DATA(Reg, XPT_FE_PARSER0_CTRL1, PARSER_ENABLE);
		/* only change refcnt if changing state */
		if (!wasEnabled && Enable) {
			BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_PARSER);
		}
#endif
		
		/* Clear all the bits we are about to change. */
		Reg &= ~( BCHP_MASK( XPT_FE_PARSER0_CTRL1, PARSER_ENABLE ) );
		Reg |= ( BCHP_FIELD_DATA( XPT_FE_PARSER0_CTRL1, PARSER_ENABLE, Enable == true ? 1 : 0 ) );
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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

    unsigned FeSel = 0;
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    BDBG_MSG(( "Configuring PID chnl %u for PID 0x%04lX on %s parser %u", 
               PidChannelNum, Pid, BXPT_P_IS_PB( Band ) ? "PB" : "IB", Band & 0xFF ));

	if( Pid >= 0x2000 )
	{
		/* Bad PID. Complain. */
		BDBG_ERR(( "Pid %lu is out of range!", ( unsigned long ) Pid ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if( PidChannelNum >= hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else							 		  
	{
#if BXPT_HAS_IB_PID_PARSERS
		unsigned OldFeSel = 255;
        unsigned OldPid = 0x2000;
        unsigned OldBand = 0x2000;     /* Never have this many parser bands. */

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif
  		PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ PidChannelNum ];

#ifdef ENABLE_PLAYBACK_MUX
		/*gain access to the pid table*/
		BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/
		Entry->Pid = Pid;
		Entry->Band = Band;
		Entry->IsAllocated = true;

		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	 	if( BXPT_P_IS_PB( Band ) )
		{
			BXPT_P_CLEAR_PB_FLAG( Band );	/* Remove the PB parser flag. */
			FeSel = 1;
		}

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
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
        }

		Entry->IsPidChannelConfigured = true;
#ifdef ENABLE_PLAYBACK_MUX
		/*leave pid table protected area*/
		BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

#else /* BXPT_HAS_IB_PID_PARSERS */
  		PidChannelTableEntry *Entry = &hXpt->PidChannelTable[ PidChannelNum ];

		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
        BSTD_UNUSED( FeSel );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
        
		Entry->IsPidChannelConfigured = true;
#endif /* BXPT_HAS_IB_PID_PARSERS */

#ifdef ENABLE_PLAYBACK_MUX
    #ifdef SW7335_895_WORKAROUND    
        /* For this workaround, enable PID Version Checking ONLY for channels not use by the playback mux. */
        if (Band < BXPT_P_MAX_PLAYBACKS - hXpt->NumPlaybackMuxes && FeSel ) 
        {
        #if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_P_PacketSubCfg TempPsubCfg;
    
                BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &TempPsubCfg );
        #endif
    
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
            Reg &= ~(1 << 28);
            BREG_Write32( hXpt->hRegister, RegAddr, Reg );
    
        #if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
                    BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &TempPsubCfg );
        #endif
        }
    #endif
#endif
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		/* Set the PID channels enable bit. */
		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif

	bool result = true;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )
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

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
            BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_P_PacketSubCfg PsubCfg;
#endif

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )
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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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
       
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	return(result);
}

BERR_Code BXPT_SetNumPlaybackMux(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int Num 				/* [in] Number of playback mux blocks*/
	)
{
    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. The 'Num' argument is 1-based, not 0-based like everything else. */
	if( Num > BXPT_P_MAX_PLAYBACKS )
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
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_P_PacketSubCfg PsubCfg;
#endif

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( Pid );
	BDBG_ASSERT( Band );

	if( PidChannelNum >= hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
	    uint32_t Reg, RegAddr;
    
		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

        *IsPlayback = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) ? true : false;

#if (BCHP_CHIP == 7630 && BCHP_VER >= BCHP_VER_B2 )
        *Band = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PID_CHANNEL_INPUT_SELECT );
#else
        *Band = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );
#endif

#ifdef BCHP_XPT_FE_PID_TABLE_i_HD_FILT_DIS_PID_CHANNEL_PID_SHIFT
		if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) )
		{
            *Pid = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_EN_PID_CHANNEL_SCID );
		}
		else
		{
            *Pid = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID );
		}
#else
        *Pid = (unsigned int) BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PID_CHANNEL_PID );
#endif
	}	
	
    return ExitCode;						 		  
}

BERR_Code BXPT_FreePidChannel( 
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int PidChannelNum		/* [in] PID channel to free up. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

   	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )
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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )	   
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
    BDBG_ASSERT( hReg );
    BKNI_EnterCriticalSection();

#if BCHP_CHIP == 35233
    /* Assert the transport core reset */
    BREG_AtomicUpdate32_isr(hReg, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, 0,
        BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, xpt_sw_init, 1 ));

    /* Now clear the reset. */     
    BREG_AtomicUpdate32_isr(hReg, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, 0,
        BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, xpt_sw_init, 1 ));
#else
    /* Assert the transport core reset */
    BREG_AtomicUpdate32_isr(hReg, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
        BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, xpt_sw_reset, 1 ));

    /* Now clear the reset. */     
    BREG_AtomicUpdate32_isr(hReg, BCHP_SUN_TOP_CTRL_SW_RESET,
        BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, xpt_sw_reset, 1 ),0);
#endif

    BKNI_LeaveCriticalSection();
}

static void SetChannelEnable(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool EnableIt
	)
{
	uint32_t Reg, RegAddr;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif

#ifdef ENABLE_PLAYBACK_MUX
	if(EnableIt && hXpt->PidChannelTable[PidChannelNum].Band >= 
		BXPT_PB_PARSER((BXPT_P_MAX_PLAYBACKS - hXpt->NumPlaybackMuxes)))
	{
		return;
	}

	/* If the channel is enabled for playback muxing, don't disable it here. BXPT_SwitchPidChannel() will do it instead. */
	if( hXpt->PidChannelTable[PidChannelNum].MuxEnable )
	{
		return;
	}
#endif /*ENABLE_PLAYBACK_MUX*/

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    	Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    /* if the pid channel is already enabled, and request is to enable it again 
       then make sure that we do not write the PID table register, this will prevent
       PID version from changing to a new value hence the buffers will not be flushed 
       by the XPT HW.This step is imortant to get a glitch free av when record.
     */
    if((Reg & BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE ))&&EnableIt)
        return;

   	Reg &= ~BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE );
	if( EnableIt )
	{
		Reg |= BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE, 1 );
	}

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

        BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
}

static bool PidChannelHasDestination( 
	BXPT_Handle hXpt,                            
	unsigned int PidChannelNum 
	)
{
	uint32_t Reg, RegAddr;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif

	/* Set the PID channels enable bit. */
	RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
	
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

#ifdef ENABLE_PLAYBACK_MUX
	BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/
	/* Disable PID channel */
	SetChannelEnable( hXpt, PidChannelNum, false );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
   	Reg &= ~BCHP_MASK( XPT_FE_SPID_TABLE_i, PID_DESTINATION );
	BREG_Write32( hXpt->hRegister, RegAddr, Reg );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

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

#if BXPT_HAS_XCBUF
	uint32_t ParserBand, PacketSize;
#endif

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
    BXPT_P_PacketSubCfg PsubCfg;
#endif

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
	Reg = BREG_Read32( hXpt->hRegister, RegAddr );
	CurrentDestinations = BCHP_GET_FIELD_DATA( Reg, XPT_FE_SPID_TABLE_i, PID_DESTINATION );
   	Reg &= ~BCHP_MASK( XPT_FE_SPID_TABLE_i, PID_DESTINATION );

	if( EnableIt )
	{
		CurrentDestinations |= ( 1ul << SelectedDestination );	/* Enable the pipe */
		Reg |= BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, PID_DESTINATION, CurrentDestinations );
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

		/* ToDo: Enable PID channel if it was requested */
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

    /* determine the parser band */
    RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

#ifndef SW7342_241_WORKAROUND
	#if BXPT_HAS_XCBUF
		if (! BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ))
		{
			ParserBand = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) ;
			RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( ParserBand * PARSER_REG_STEPSIZE );
			Reg = BREG_Read32( hXpt->hRegister, RegAddr );
			if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_PACKET_TYPE)== 1)
				PacketSize = 130;
			else
				PacketSize = 188;
			
			/* Increase the rate only when both G and R pipe are enabled. */
			if (( CurrentDestinations & 0x30 ) == 0x30 )
			{
                #define MAX_QAM_RATE    104000000

                unsigned long BlockOut = BXPT_P_XcBuf_GetBlockout( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserBand );

                if( BlockOut != MAX_QAM_RATE )
                    hXpt->XcBufBO[ BXPT_XcBuf_Id_RAVE_IBP0 + ParserBand ] = BlockOut;

				/* Adjust Rave data read BW */
				BXPT_XcBuf_SetBandDataRate( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserBand, MAX_QAM_RATE, PacketSize );  
			}
            else
            {
                BXPT_P_XcBuf_SetBlockout( hXpt, BXPT_XcBuf_Id_RAVE_IBP0 + ParserBand, hXpt->XcBufBO[ BXPT_XcBuf_Id_RAVE_IBP0 + ParserBand ] );
            }
		}
	#endif
#endif

	return ExitCode;
}

static bool IsPidDuplicated(
	BXPT_Handle hXpt, 	   		/* [Input] Handle for this transport */
	unsigned int PidChannelNum 	
	)
{
	unsigned int i, TargetChannelCfg, ThisChannelCfg, ThisChannelEnable;
 	uint32_t RegAddr, Reg;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif

	bool IsDuplicated = false;

#ifdef ENABLE_PLAYBACK_MUX
	if(hXpt->PidChannelTable[PidChannelNum].Band >= 
		BXPT_PB_PARSER((BXPT_P_MAX_PLAYBACKS - hXpt->NumPlaybackMuxes)))
	{
		/*there are allowed to be duplicated pids on the muxed band*/
		return(false);
	}

	/*gain access to the pid table*/
	BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

 	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    TargetChannelCfg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

    /* This mask covers all the bitfields we need to compare. */
#ifdef BCHP_XPT_FE_PID_TABLE_i_HD_FILT_DIS_PID_CHANNEL_PID_SHIFT

#if (BCHP_CHIP == 7630 && BCHP_VER >= BCHP_VER_B2 )
    TargetChannelCfg &= ( 
        BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_INPUT_SELECT ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID )
    );
#else
    TargetChannelCfg &= ( 
        BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID )
    );
#endif

#else
    TargetChannelCfg &= ( 
        BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
        BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_PID )
    );
#endif

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

	for( i = 0; i < hXpt->MaxPidChannels; i++ )
	{
		/* Skip the legitimate channel assignment. */
		if( i == PidChannelNum )
			continue;

		if( hXpt->PidChannelTable[ i ].IsPidChannelConfigured == true )
		{
 	        RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( i * PID_CHNL_STEPSIZE );
            ThisChannelCfg = Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#ifdef BCHP_XPT_FE_PID_TABLE_i_HD_FILT_DIS_PID_CHANNEL_PID_SHIFT
    #if (BCHP_CHIP == 7630 && BCHP_VER >= BCHP_VER_B2 )
                ThisChannelCfg &= ( 
                    BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
                    BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_INPUT_SELECT ) |
                    BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
                    BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID )
                );
    #else
                ThisChannelCfg &= ( 
                    BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
                    BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
                    BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
                    BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_DIS_PID_CHANNEL_PID )
                );
    #endif
#else
            ThisChannelCfg &= ( 
                BCHP_MASK( XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) |
                BCHP_MASK( XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT ) |
                BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
                BCHP_MASK( XPT_FE_PID_TABLE_i, PID_CHANNEL_PID )
            );
#endif

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

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

#ifdef ENABLE_PLAYBACK_MUX
	/*leave pid table protected area*/
	BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/
	return( IsDuplicated );
}

#ifdef SW7342_241_WORKAROUND

static void BXPT_P_ConfigArbiter(BREG_Handle hReg)
{
    uint32_t Reg = 0; 

#if ( BCHP_CHIP == 7340 ) || ( BCHP_CHIP == 7342 )

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, Reg );
 	 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, Reg );
	 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, Reg );
	 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, Reg );

#else
	BSTD_UNUSED( Reg );
    #error "SW7342-241 Port needed"                                                                                           
#endif
}

#else

static void BXPT_P_ConfigArbiter(BREG_Handle hReg)
{
    uint32_t Reg = 0; 

#if( BCHP_CHIP == 7405 ) || ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  ) || \
    ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 3556 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125) || ( BCHP_CHIP == 7340 ) 

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, Reg );

#elif ( BCHP_CHIP == 7440 ) 

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB0 );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_PB0, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_PB0, ARB_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB0, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB1 );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_PB1, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_PB1, ARB_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB1, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB2 );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_PB2, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_PB2, ARB_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB2, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, ARB_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, ARB_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, Reg );
 
#elif ( BCHP_CHIP == 3563 ) 

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RSBUFF_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RSBUFF_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RSBUFF_WR, ARB_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RSBUFF_WR, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RSBUFF_RD );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RSBUFF_RD, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RSBUFF_RD, ARB_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RSBUFF_RD, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_XCBUFF_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_XCBUFF_WR, ARB_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_WR, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_RMX0_RD );                                                                      
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_XCBUFF_RMX0_RD, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_XCBUFF_RMX0_RD, ARB_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_RMX0_RD, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_MSG_RD );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_XCBUFF_MSG_RD, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_XCBUFF_MSG_RD, ARB_SEL, 3 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_MSG_RD, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD, ARB_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, ARB_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, ARB_SEL, 3 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB0 );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_PB0, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_PB0, ARB_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PB0, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PSUB );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_PSUB, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_PSUB, ARB_SEL, 3 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_PSUB, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_MSG_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_MSG_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_MSG_WR, ARB_SEL, 3 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_MSG_WR, Reg );

#elif (BCHP_CHIP == 7118) || (BCHP_CHIP == 7400 ) || (BCHP_CHIP == 7401 ) || (BCHP_CHIP == 7403 )

    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RSBUFF_RD );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RSBUFF_RD, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RSBUFF_RD, ARB_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RSBUFF_RD, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_XCBUFF_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_XCBUFF_WR, ARB_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_WR, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD, ARB_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_XCBUFF_RAVE_RD, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, ARB_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_CDB_WR, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR );
    Reg &= ~( BCHP_MASK( XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, ARB_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, ARB_SEL, 3 ));
    BREG_Write32( hReg, BCHP_XPT_BUS_IF_ARB_SEL_RAVE_ITB_WR, Reg );

#elif ( BCHP_CHIP == 35230 ) || ( BCHP_CHIP == 35125 ) 

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, Reg );

#elif ( BCHP_CHIP == 7420 ) 
    uint32_t ulChipIdReg=0;
    ulChipIdReg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PROD_REVISION);
    ulChipIdReg &= 0xffff0000;
    ulChipIdReg >>= 16;
    BDBG_MSG(("Chip ID %x",ulChipIdReg));
    switch(ulChipIdReg)
    {
    case 0x7420:
         {
          BDBG_MSG((" SCB client mapping for 7420 transport h/w "));
	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, SCB_WR_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL, 0 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, Reg );

	Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL, 1 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, Reg );
 
    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB6 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB7 );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, Reg );

    Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB );
    Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL, 2 ));
    BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, Reg );
       }
        break;
    case 0x7410:
        {
          BDBG_MSG((" SCB client mapping for 7410 transport h/w "));
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, SCB_WR_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL, 1 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL, 1));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL, 1 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL, 1 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5 );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB6 );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB7 );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, Reg );
        }
        break;
    case 0x7409:
        {
          BDBG_MSG(("SCB client mapping for 7409 transport h/w "));
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, SCB_WR_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RSBUFF, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, SCB_WR_CLIENT_SEL, 1 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_XCBUFF, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, SCB_WR_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_CDB, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, SCB_WR_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_RAVE_ITB, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, SCB_WR_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_WR_ARB_SEL_MSG, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, SCB_RD_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_RSBUFF, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, SCB_RD_CLIENT_SEL, 0 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RAVE, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, SCB_RD_CLIENT_SEL, 1));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_MSG, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, SCB_RD_CLIENT_SEL, 1 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX0, Reg );
	      Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, SCB_RD_CLIENT_SEL, 1 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_XCBUFF_RMX1, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB0, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB1, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB2, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB3, Reg );
          Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4 );
          Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL ) );
          Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, SCB_RD_CLIENT_SEL, 2 ));
          BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB4, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5 );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB5, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB6 );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB6, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB7 );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PB7, Reg );
         Reg = BREG_Read32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB );
         Reg &= ~( BCHP_MASK( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL ) );
         Reg |= ( BCHP_FIELD_DATA( XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, SCB_RD_CLIENT_SEL, 2 ));
         BREG_Write32( hReg, BCHP_XPT_XMEMIF_SCB_RD_ARB_SEL_PSUB, Reg );
        }
        break;
    default:
        BDBG_ERR(("SCB client mapping for transport h/w module not defined"));
        BDBG_ERR(("Using the chip reset values"));
        break;
    }
#else 
    BSTD_UNUSED( Reg ); 
    BSTD_UNUSED( hReg ); 
    /* Nothing specific needed yet. */
#endif
}

#endif

#if BXPT_HAS_IB_PID_PARSERS

static bool AllInputParsersAreOff(
	BXPT_Handle hXpt
	)
{
    uint32_t RegAddr, Reg;
    unsigned Index;

    for( Index = 0; Index < BXPT_P_MAX_PID_PARSERS; Index++ )
    {
		#if (BCHP_CHIP == 7342)
		RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + BXPT_P_GetParserRegOffset(Index);
		#else
    	RegAddr = BCHP_XPT_FE_PARSER0_CTRL1 + ( Index * PARSER_REG_STEPSIZE );
		#endif
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PARSER0_CTRL1, PARSER_ENABLE ) )
            return false;
    }

    return true;
}

#endif

static bool AllPlaybackChannelsAreOff(
	BXPT_Handle hXpt
	)
{
    uint32_t RegAddr, Reg;
    unsigned Index;

    for( Index = 0; Index < BXPT_P_MAX_PLAYBACKS; Index++ )
    {
    	RegAddr = BCHP_XPT_PB0_CTRL1 + ( Index * PB_PARSER_REG_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        if( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, BUSY ) )
            return false;
    }

    return true;
}

#if BXPT_HAS_PACKETSUB

static bool AllPacketSubChannelsAreOff(
	BXPT_Handle hXpt
	)
{
    uint32_t RegAddr, Reg;
    unsigned Index;

    for( Index = 0; Index < BXPT_P_MAX_PACKETSUBS; Index++ )
    {
    	RegAddr = BCHP_XPT_PSUB_PSUB0_STAT0 + ( Index * PACKET_SUB_REGISTER_STEP );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        if( BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, BUSY ) )
            return false;
    }

    return true;
}

#endif

bool BXPT_P_CanPowerDown( 
	BXPT_Handle hXpt
	)
{
    uint32_t RaveStatus;
    unsigned ItbIdle, CdbIdle, DmaBusy, AvMuxBufferDepth;
    unsigned WaitCount;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

#if BXPT_HAS_IB_PID_PARSERS
    /* All parser bands must be off */
    if( !AllInputParsersAreOff( hXpt ) )
    {
        BDBG_MSG(( "At least 1 parser band is enabled. XPT can't power down" ));
        return false;
    }
#endif

    /* All playback channels must be off */
    if( !AllPlaybackChannelsAreOff( hXpt ) )
    {
        BDBG_MSG(( "At least 1 playback channel is enabled. XPT can't power down" ));
        return false;
    }

#if BXPT_HAS_PACKETSUB
    /* All packet substitution channels must be off */
    if( !AllPacketSubChannelsAreOff( hXpt ) )
    {
        BDBG_MSG(( "At least 1 packet substitution channel is enabled. XPT can't power down" ));
        return false;
    }
#endif

#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF
    {
        unsigned RsBufDataRdy, XcBufDataRdy;

        /* 
        ** If we got this far, we can wait for the RS and XC buffers to clear in a 
        ** reasonable amount of time. What's reasonable? Start with 500uS.
        */
        WaitCount = 500;
        do
        {
            RsBufDataRdy = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_DATA_RDY );
            XcBufDataRdy = BREG_Read32( hXpt->hRegister, BCHP_XPT_XCBUFF_DATA_RDY );

            if( RsBufDataRdy || XcBufDataRdy )  
            {
                WaitCount--;
                if( !WaitCount )
                {
                    BDBG_MSG(("RS/XC buffers haven't finished memory transactions. XPT can't power down" )) ;
                    return false;
                }

                BKNI_Delay( 1 );    /* Busy wait for 1 uS */
            }
        }
        while( RsBufDataRdy || XcBufDataRdy );
    }
#endif

    /* Make sure that RAVE has finished all memory transactions */
    WaitCount = 500;
    do
    {     
        RaveStatus = BREG_Read32( hXpt->hRegister, BCHP_XPT_RAVE_AV_STATUS );
        ItbIdle = BCHP_GET_FIELD_DATA( RaveStatus, XPT_RAVE_AV_STATUS, ITB_MEM_STATE_IDLE );
        CdbIdle = BCHP_GET_FIELD_DATA( RaveStatus, XPT_RAVE_AV_STATUS, CDB_MEM_STATE_IDLE );
        DmaBusy = BCHP_GET_FIELD_DATA( RaveStatus, XPT_RAVE_AV_STATUS, DMA_BUSY );
        AvMuxBufferDepth = BCHP_GET_FIELD_DATA( RaveStatus, XPT_RAVE_AV_STATUS, AV_MUX_BUFFER_DEPTH );

        if( !ItbIdle || !CdbIdle || DmaBusy || AvMuxBufferDepth )
        {

            WaitCount--;
            if( !WaitCount )
            {
                BDBG_MSG(("RAVE hasn't finished memory transactions. Can't power down" )) ;
                return false;
            }

            BKNI_Sleep( 1 );
        }
    }
    while( !ItbIdle || !CdbIdle || DmaBusy );

    /* All the above tests had to pass to get this far. */
    BKNI_Delay( 500 );      /* Wait for XMEMIF buffers to clear. No way to tell when that happens, so wait */
    return true;
}

#if BXPT_HAS_RSBUF
BERR_Code BXPT_IsDataPresent(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get status for. */
    bool *IsDataPresent
    )
{
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( IsDataPresent );

	if( PidChannelNum >= hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
        *IsDataPresent = false;
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
    {
	    uint32_t Reg, RegAddr, DataReady;
        unsigned IsPlayback, ParserBand, BitMask;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_P_PacketSubCfg PsubCfg;
#endif
    
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_SaveCfg( hXpt, PidChannelNum, &PsubCfg );
#endif
		RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PidChannelNum * PID_CHNL_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
        BXPT_PacketSub_P_RestoreCfg( hXpt, PidChannelNum, &PsubCfg );
#endif

        IsPlayback = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL );
        ParserBand = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );

		DataReady = BREG_Read32( hXpt->hRegister, BCHP_XPT_RSBUFF_DATA_RDY );
        BitMask = (IsPlayback ? 0x0100 : 0x001) << ParserBand;
        *IsDataPresent = BitMask & DataReady ? true : false;
    }

    return ExitCode;
}
#endif

#if BXPT_HAS_PIPELINE_ERROR_REPORTING
BERR_Code BXPT_CheckPipelineErrors(
    BXPT_Handle hXpt
    )
{
    BSTD_UNUSED( hXpt );
    BDBG_WRN(( "Data pipeline error checking is not supported" ));
    return BERR_NOT_SUPPORTED;
}
#endif

/* end of file */



