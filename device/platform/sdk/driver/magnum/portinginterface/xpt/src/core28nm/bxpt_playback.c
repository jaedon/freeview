/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_playback.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/1/12 10:00a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core28nm/bxpt_playback.c $
 * 
 * Hydra_Software_Devel/1   10/1/12 10:00a gmullen
 * SW7445-17: Created core28nm file tree
 * 
 * Hydra_Software_Devel/36   8/30/12 11:05a gmullen
 * SW7425-3617: LastDescriptor incorrectly set
 * 
 * Hydra_Software_Devel/35   8/10/12 11:26a gmullen
 * SW7425-3701: Changed handling in _Open()
 * 
 * Hydra_Software_Devel/34   8/9/12 4:52p gmullen
 * SW7425-3719: Checked BINT return code
 * 
 * Hydra_Software_Devel/33   8/8/12 9:06a gmullen
 * SW7425-3701: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3701/1   8/7/12 5:26p gmullen
 * SW7425-3701: Added BXPT_Playback__IsAvailable(). Fail if either PB 13
 * or 14 is requested on 7425 and 7346
 * 
 * Hydra_Software_Devel/32   8/2/12 4:31p gmullen
 * SW7425-3617: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3617/1   8/1/12 4:04p gmullen
 * SW7425-3617: PI will access device RAM through cached addresses
 * 
 * Hydra_Software_Devel/31   5/15/12 5:17p gmullen
 * SW7425-2630: Incorrectly added all-pass PID channel
 * 
 * Hydra_Software_Devel/30   5/3/12 3:47p gmullen
 * SW7358-281: Fixed coverity defect
 * 
 * Hydra_Software_Devel/29   3/15/12 2:13p gmullen
 * SW7425-2630: Wrong PID channel used for playback all-pass
 * 
 * Hydra_Software_Devel/28   1/23/12 3:58p gmullen
 * SW7346-568: Forced resync flag not cleared when packetization was
 * disabled
 * 
 * Hydra_Software_Devel/27   11/14/11 9:49a gmullen
 * SW7231-451: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7231-451/1   11/14/11 9:35a gmullen
 * SW7231-451: Removed writes to START_ADDR_ regs in RAVE, added BDBG_MSGs
 * 
 * Hydra_Software_Devel/26   11/10/11 12:25p jtna
 * SW7425-1672: merge to hydra
 * 
 * Hydra_Software_Devel/SW7425-1672/2   11/10/11 8:45a gmullen
 * SW7425-1672: Added check for looped descriptor list
 * 
 * Hydra_Software_Devel/SW7425-1672/1   11/9/11 5:50p jtna
 * SW7425-1672: set FORCE_RESYNC flag on descriptors when packetizing ES
 * 
 * Hydra_Software_Devel/25   11/3/11 3:47p gmullen
 * SW7425-1323: Fixed B0/B1 binary compatability issue
 * 
 * Hydra_Software_Devel/24   10/5/11 4:25p gmullen
 * SW7346-502: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7346-502/1   10/5/11 4:03p gmullen
 * SW7346-502: Added AcceptAdapt00 support
 * 
 * Hydra_Software_Devel/23   9/21/11 10:17a gmullen
 * SW7425-1323: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1323/1   9/21/11 10:02a gmullen
 * SW7425-1323: Potential workaround.
 * 
 * Hydra_Software_Devel/22   9/12/11 10:50a gmullen
 * SWDTV-8330: Ported fix to 40nm chips
 * 
 * Hydra_Software_Devel/21   8/11/11 2:07p gmullen
 * SW7346-382: Removed check on LastDesc pointer in _AddDescriptors()
 * 
 * Hydra_Software_Devel/20   8/4/11 5:06p gmullen
 * SW7346-382: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7346-382/2   8/4/11 4:51p gmullen
 * SW7346-382: Force resync on first descriptor when in bypass mode
 * 
 * Hydra_Software_Devel/SW7346-382/1   8/2/11 11:12a gmullen
 * SW7346-382: Use the hw designer's recommended config for ES
 * 
 * Hydra_Software_Devel/19   8/4/11 10:00a gmullen
 * SW7231-308: Merged fix to Hydra
 * 
 * Hydra_Software_Devel/SW7231-308/1   8/4/11 9:55a gmullen
 * SW7231-308: Temporarily disable pacing during channel stop
 * 
 * Hydra_Software_Devel/18   8/3/11 10:44a gmullen
 * SW7346-392: Changed index check to fix Coverity warning
 * 
 * Hydra_Software_Devel/17   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/1   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/16   5/12/11 4:59p gmullen
 * SW7231-128: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/10/11 1:40p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/15   4/11/11 9:15a gmullen
 * SW7346-119: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7346-119/1   4/7/11 3:43p gmullen
 * SW7346-119: Disable CC checking when enabling all-pass mode. Restore
 * when exiting all-pass
 * 
 * Hydra_Software_Devel/14   3/29/11 3:45p gmullen
 * SW7125-785: Re-arm pacing, if enabled, when resuming from micro pause
 * 
 * Hydra_Software_Devel/13   3/11/11 1:07p gmullen
 * SW7346-112: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7346-112/1   3/11/11 9:39a gmullen
 * SW7346-112: Recalc blockout when packet length is changed
 * 
 * Hydra_Software_Devel/12   3/7/11 3:09p gmullen
 * SW7425-153: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-153/1   3/7/11 3:07p gmullen
 * SW7425-153: Fixed blockout calc and disabled usage of spare bandwidth
 * 
 * Hydra_Software_Devel/11   1/28/11 9:41a gmullen
 * SW7425-15: Allow spare bandwidth in playback during packetization. PR
 * 29510 is fixed in the 40nm cores
 * 
 * Hydra_Software_Devel/10   12/21/10 9:22a gmullen
 * SW7425-15: Set LLD type in SetSettings()
 * 
 * Hydra_Software_Devel/9   12/16/10 4:41p gmullen
 * SW7425-15: Fixed compilation error on 7344 builds
 * 
 * Hydra_Software_Devel/8   12/16/10 3:17p gmullen
 * SW7425-15: Added PES pacing to GetSettings call
 * 
 * Hydra_Software_Devel/6   12/6/10 10:01a gmullen
 * SW7422-117: Disable pausing from XC buffer to flush data on playback
 * stopping
 * 
 * Hydra_Software_Devel/5   11/12/10 3:28p gmullen
 * SW7425-15: Removed 1-byte buffer restriction
 * 
 * Hydra_Software_Devel/4   11/12/10 11:11a gmullen
 * SW7425-15: Changes to descriptor handling. Also disable CC checking and
 * enable CC generation during packetization
 * 
 * Hydra_Software_Devel/3   10/28/10 6:01p gmullen
 * SW7425-15: Ported files
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/115   10/19/10 10:08a gmullen
 * SW7125-480: Fixed incorrect macro usage. Was clearing all pending
 * interrupts
 * 
 * Hydra_Software_Devel/114   9/7/10 5:36p gmullen
 * SW7420-1045: Updated RS buffer blockouts in _SetBitRate() to handle
 * MPOD usage
 * 
 * Hydra_Software_Devel/113   8/30/10 5:24p gmullen
 * SW7403-924: Protected access to the PID and SPID table from other
 * modules in the PI
 * 
 * Hydra_Software_Devel/112   7/28/10 1:12p gmullen
 * SW7630-81: Default RaveOutputOnly = true, since this part doesn't have
 * XC buffers
 * 
 * Hydra_Software_Devel/111   7/27/10 5:11p gmullen
 * SW3548-3013: Added error checking to BMEM allocs
 * 
 * Hydra_Software_Devel/110   7/4/10 5:33p gmullen
 * SW7630-81: Added support for QUICK
 * 
 * Hydra_Software_Devel/109   5/13/10 12:07p gmullen
 * SWBLURAY-20102: Fixed typo
 * 
 * Hydra_Software_Devel/108   5/13/10 9:45a gmullen
 * SWBLURAY-20102: Use interrupt status reg to get out-of-sync status
 * 
 * Hydra_Software_Devel/107   4/20/10 8:08a gmullen
 * SW7125-350: Merged fix to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7125-350/1   4/19/10 10:42a gmullen
 * SW7125-350: Reset playback parser when packetizing is disabled
 * 
 * Hydra_Software_Devel/106   3/26/10 3:37p gmullen
 * SW7405-1883: Use both DESC_NOT_DONE and FINISHED to determine if a
 * descriptor chain has finished
 * 
 * Hydra_Software_Devel/105   2/16/10 6:05p gmullen
 * SW7405-3926: Restore sync mode if channel stopping times out
 * 
 * Hydra_Software_Devel/104   1/14/10 3:34p piyushg
 * SW7335-664: Restoring the BAND_HOLD_EN values for RAVE contexts
 * after BXPT_Playback_StopChannel processing.
 * 
 * Hydra_Software_Devel/103   12/16/09 3:26p gmullen
 * SW7325-665: Added support for 32-bit timestamp mode
 * 
 * Hydra_Software_Devel/102   11/20/09 3:20p gmullen
 * SW7630-54: Update BCHP checks in BXPT_Playback_DisablePacketizers for
 * the 7630 and 7635
 * 
 * Hydra_Software_Devel/101   11/19/09 5:12p gmullen
 * SW7408-9: Fixed macro name again
 * 
 * Hydra_Software_Devel/100   11/19/09 4:39p gmullen
 * SW7408-9: Fixed macro name
 * 
 * Hydra_Software_Devel/99   11/18/09 5:11p gmullen
 * SW7408-9: Enabled descriptor read chicken bit
 * 
 * Hydra_Software_Devel/98   11/17/09 3:07p gmullen
 * SW7408-9: Finished adding 7408 support to XPT
 * 
 * Hydra_Software_Devel/97   11/11/09 11:01a gmullen
 * SW7550-16: Enabled AGRESSIVE_DATA_READ chicken-bit.
 * 
 * Hydra_Software_Devel/96   11/3/09 9:33a gmullen
 * SW7405-3308: Swapped interrupt macros for TEI and Parser Length
 * interrupts
 * 
 * Hydra_Software_Devel/95   10/22/09 9:30a gmullen
 * SW7630-46: Added DVD change for BXPT_Playback_CheckHeadDescriptor
 * 
 * Hydra_Software_Devel/94   10/1/09 11:12a gmullen
 * SW7405-3102: ResetPacing in Setting struct incorrectly set if pacing is
 * enabled
 * 
 * Hydra_Software_Devel/93   9/29/09 11:58a gmullen
 * SW7405-3102: Pacing auto reset can be disabled through
 * Get/SetChannelSettings()
 * 
 * Hydra_Software_Devel/92   9/16/09 1:42p gmullen
 * SW35230-2: Fixed compilation errors on 35230 builds.
 * 
 * Hydra_Software_Devel/91   9/9/09 8:06a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/90   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/89   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/88   7/31/09 3:25p piyushg
 * PR56771: Add support for 7342.
 * 
 * Hydra_Software_Devel/87   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/86   7/15/09 3:05p gmullen
 * PR56760: Fixed warning in non-debug builds.
 * 
 * Hydra_Software_Devel/85   7/14/09 10:22a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/84   7/13/09 10:01a gmullen
 * PR56760: Added dummy descriptor and wait until dummy desc is consumed.
 * 
 * Hydra_Software_Devel/83   6/18/09 11:11a gmullen
 * PR56110: Checked for mesg support.
 * 
 * Hydra_Software_Devel/82   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/81   6/2/09 10:29a gmullen
 * PR51821: Merged workaround to mainline.
 * 
 * Hydra_Software_Devel/80   5/28/09 2:11p gmullen
 * PR55549: Added support for PCR-based pacing.
 * 
 * Hydra_Software_Devel/79   4/20/09 2:31p gmullen
 * PR48905: Disable band hold temporarily at playback stop.
 * 
 * Hydra_Software_Devel/PR48905/1   4/20/09 12:51p gmullen
 * PR48905: Disable band hold temporarily at playback stop.
 * 
 * Hydra_Software_Devel/78   4/16/09 3:39p gmullen
 * PR54222: Merged 7002-specific changes to mainline. Created symlinks for
 * 7002 files back to 7400.
 * 
 * Hydra_Software_Devel/77   4/13/09 5:00p gmullen
 * PR54061: Enable PES SID, SubSID, and SID Extension extraction for all
 * types of PES filtering.
 * 
 * Hydra_Software_Devel/76   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/75   2/18/09 10:49a piyushg
 * PR52189: Added hooks for Get/Set PACING_OFFSET_ADJ_DIS bit.
 * 
 * Hydra_Software_Devel/74   2/5/09 5:53p piyushg
 * PR43563: Keeping the PARSER_PKT_LENGTH as 0xBC
 * during ES packetization.
 * 
 * Hydra_Software_Devel/73   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/72   12/15/08 2:36p gmullen
 * PR48908: Removed power management code from XPT PI.
 * 
 * Hydra_Software_Devel/71   12/9/08 4:42p gmullen
 * PR47755: Fixed compilation error in packetizer code.
 * 
 * Hydra_Software_Devel/70   11/26/08 4:15p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/69   11/13/08 10:49a gmullen
 * PR48835: Fixed typo in BDBG message
 * 
 * Hydra_Software_Devel/68   11/12/08 6:37p piyushg
 * PR48187: Reverting back the changes done in Rev. 63 (PR47968).
 * This fix was introducing other bugs and is not a clean solution
 * for back to back playback issue. Various "time out" related bugs
 * should not happen now.
 * 
 * Hydra_Software_Devel/68   11/12/08 6:35p piyushg
 * PR48187: Reverting back the changes done in Rev. 63 (PR47968).
 * This fix was introducing other bugs and is not a clean solution
 * for back to back playback issue. Various "time out" related bugs
 * should not happen now.
 * 
 * Hydra_Software_Devel/67   11/12/08 3:10p gmullen
 * PR48835: BXPT_Playback_SetChannelPacketSettings now takes a struct.
 * 
 * Hydra_Software_Devel/66   11/3/08 9:44a gmullen
 * PR48616: Enabled callbacks in _SetChannelSettings.
 * 
 * Hydra_Software_Devel/65   10/29/08 1:59p gmullen
 * PR48511: Disable packetizers on channel close.
 * 
 * Hydra_Software_Devel/64   10/28/08 4:58p gmullen
 * PR46544: Added power management support, default to disabled for now.
 * 
 * Hydra_Software_Devel/63   10/17/08 9:42a gmullen
 * PR47968: Wait for BUSY to clear before starting channel
 * 
 * Hydra_Software_Devel/62   10/9/08 11:35a gmullen
 * PR47745: Added BXPT_Playback_GetPacketizerDefaults().
 * 
 * Hydra_Software_Devel/61   9/18/08 9:15a gmullen
 * PR47063: Fixed compiler complaint: Done label was unused.
 * 
 * Hydra_Software_Devel/60   9/17/08 5:31p gmullen
 * PR47063: Merge changes to mainline
 * 
 * Hydra_Software_Devel/wince_7400/1   4/1/08 10:23a gmullen
 * PR41135: Removed incorrect arugment check in
 * BXPT_Playback_PacketizeStream
 * 
 * Hydra_Software_Devel/59   9/17/08 5:02p gmullen
 * PR47065: Added PCR-based pacing support,
 * 
 * Hydra_Software_Devel/58   8/12/08 12:37p gmullen
 * PR45610: Returned BERR_NOT_SUPPORTED on certain chips if buffer length
 * is 0 or 1.
 * 
 * Hydra_Software_Devel/57   8/12/08 8:00a gmullen
 * PR45618: Enabled support for ForceRestamping bool in parser config.
 * 
 * Hydra_Software_Devel/56   7/16/08 3:44p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/55   7/8/08 6:40p gmullen
 * PR44037: Cleared program stream mode and enable bits before loading new
 * values.
 * 
 * Hydra_Software_Devel/54   6/5/08 6:07a gmullen
 * PR43325: Used BXPT_MAX_PLAYBACK_RATE define in SetBitRate()
 * 
 * Hydra_Software_Devel/53   5/27/08 10:27a gmullen
 * PR43039: Fixed un-initialized AllPass bool.
 * 
 * Hydra_Software_Devel/52   4/10/08 3:36p gmullen
 * PR38954: Updated number of playbacks, added HDDVI support to PCR API,
 * and added packetizer support.
 * 
 * Hydra_Software_Devel/51   3/13/08 11:53a piyushg
 * PR40136: Keeping the legacy context packetizing for ES streams only.
 * 
 * Hydra_Software_Devel/50   2/19/08 5:03p gmullen
 * PR39728: Memset ChannelSettings struct to 0.
 * 
 * Hydra_Software_Devel/49   1/9/08 11:17a mward
 * PR37062: Increased time to drain internal buffers helps 7118 when
 * stopping DVD VOB playback.
 * 
 * Hydra_Software_Devel/48   1/8/08 1:25p katrep
 * PR36546: Reset the SPID table only for the pid channnels connected to
 * the current playback parser during BXPT_Playback_DisablePacketizers
 * call.Diabled legacy context packetzing on the new chips.
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/2   12/17/07 6:38p shuang
 * PR37867:Merge Jethead patch in order to support DirecTV AM21 project
 * which ATSC data will input through USB interface.
 * Merge Magnum Phase 7.0.
 * 
 * Hydra_Software_Devel/47   12/5/07 4:52p gmullen
 * PR37062: Pause playback before stopping to drain internal buffers.
 * 
 * Hydra_Software_Devel/46   12/4/07 6:23p mward
 * PR37908: Use 7401 DVD LPCM support for 7118.
 * 
 * Hydra_Software_Devel/45   11/30/07 5:38p gmullen
 * PR37062: BXPT_Playback_DisablePacketizers() uses SetDvdMode() in
 * addition to normal packeizer shutdown for 7401
 * 
 * Hydra_Software_Devel/44   11/30/07 11:49a gmullen
 * PR37062: Changed LPCM support to DVD Mode.
 * 
 * Hydra_Software_Devel/43   11/28/07 9:01a gmullen
 * PR37062: Added support for LPCM on the 7401.
 * 
 * Hydra_Software_Devel/42   11/27/07 3:02p piyushg
 * PR35926: Initializing the ResetPacing field in the call to
 * BXPT_Playback_GetChannelDefaultSettings. Fix for the Coverity
 * Defect ID:4053
 * 
 * Hydra_Software_Devel/42   11/27/07 2:59p piyushg
 * PR35926: Initializing the ResetPacing field in the call to
 * BXPT_Playback_GetChannelDefaultSettings. Fix for the
 * Coverity Defect ID:4053
 * 
 * Hydra_Software_Devel/41   11/15/07 7:19p piyushg
 * PR35829: Toggle PACING_START bit when TS_RANGE_ERROR interupt
 * occurs. Earlier PACING_EN and PACING_AUTO_EN were also being
 * reset and set which was causing a brief marcoblock in the TTS
 * stream. This also fixes PR36443.
 * 
 * Hydra_Software_Devel/40   9/27/07 2:05p gmullen
 * PR35380: 7400 support was missing from
 * BXPT_Playback_DisablePacketizers()
 * 
 * Hydra_Software_Devel/39   9/7/07 6:53p katrep
 * PR27642: Use the new stream filtering using spid for 7405. utilize the
 * new hw features.
 * 
 * Hydra_Software_Devel/38   8/31/07 3:50p gmullen
 * PR34504: Added substream filtering.
 * 
 * Hydra_Software_Devel/37   8/21/07 5:21p gmullen
 * PR34222: Added DisableTimestampParityCheck bool to
 * BXPT_Playback_ChannelSettings struct.
 * 
 * Hydra_Software_Devel/36   8/2/07 12:18p gmullen
 * PR33743: Fixed timebase comparison.
 * 
 * Hydra_Software_Devel/35   8/2/07 11:36a katrep
 * PR29510: For 7440 allow to allocated extra bandwidth & disable spare BW
 * flag to hide the HW issue.
 * 
 * Hydra_Software_Devel/34   8/1/07 3:38p gmullen
 * PR33710: pepSpecial arbiter settings for 7440. Also support playback
 * data direct to RAVE.
 * 
 * Hydra_Software_Devel/33   7/26/07 1:58a katrep
 * PR29510: For HD/BD PD spare BW is needed, backed out the changes made
 * in #30 , only for 7440
 * 
 * Hydra_Software_Devel/32   7/13/07 4:09p piyushg
 * PR32218: PACING_START bit is reset whenever
 * TS_RANGE_ERROR interrupt occurs.
 * 
 * Hydra_Software_Devel/32   7/13/07 4:03p piyushg
 * PR32218: Reset PACING_START bit when TS_RANGE_ERROR
 * interupt occurs.
 * 
 * Hydra_Software_Devel/31   7/1/07 1:09p katrep
 * PR31880: Cleanup packetization state for 7440 on channel close.
 * 
 * Hydra_Software_Devel/30   6/1/07 3:21p gmullen
 * PR29510: Disabled spare bandwidth usage when packetizer is enabled.
 * 
 * Hydra_Software_Devel/29   5/17/07 2:17p gmullen
 * PR30877: Added support for dedicated playback heap handle.
 * 
 * Hydra_Software_Devel/28   4/25/07 4:22p gmullen
 * PR29688: Added AcceptNulls and AcceptAdapt00 support to parser config
 * 
 * Hydra_Software_Devel/27   4/23/07 9:32a gmullen
 * PR30083: Removed check of BUSY bit from BXPT_Playback_StopChannel
 * 
 * Hydra_Software_Devel/26   3/15/07 7:23p katrep
 * PR28320: Fixed the previous version.
 *
 * Hydra_Software_Devel/25   3/15/07 5:12p katrep
 * PR28320: Expose PsMode and PackHdrConfig for all the chips which
 * support it.
 * 
 * Hydra_Software_Devel/24   1/9/07 11:38a gmullen
 * PR26962: Added compile directive for 7403 to
 * BXPT_Playback_DisablePacketizers.
 * 
 * Hydra_Software_Devel/23   12/12/06 10:27a katrep
 * PR23114: Fixed bug with BXPT_Playback_PacketizeStream for 7440.
 * 
 * Hydra_Software_Devel/22   11/22/06 4:58p gmullen
 * PR26109: Updated for 7403.
 * 
 * Hydra_Software_Devel/21   11/21/06 3:59p gmullen
 * PR26109: Ported files to 7403.
 * 
 * Hydra_Software_Devel/20   11/2/06 8:40a gmullen
 * PR23189: Added 3563 support.
 * 
 * Hydra_Software_Devel/19   10/4/06 3:34p katrep
 * PR23114: Use Pcketize pid table for PES all mode. Do not use the
 * context as it will be dropped in the future chips.
 * 
 * Hydra_Software_Devel/18   10/2/06 11:07a gmullen
 * PR24504: Added BXPT_Playback_GetIntId().
 * 
 * Hydra_Software_Devel/17   9/19/06 10:36a katrep
 * PR23114: fixed compile error for 7401
 * 
 * Hydra_Software_Devel/16   9/18/06 4:43p katrep
 * PR23114: Include/drop pack hdr and select the insersion in TS payload
 * or adaptaion field.
 * 
 * Hydra_Software_Devel/15   9/18/06 12:12p katrep
 * PR23114: Disable context enable for PES as context pt is used.
 * 
 * Hydra_Software_Devel/14   9/15/06 4:28p katrep
 * PR23114: Added PsMode to Channel Settings ,updated SyncMode comments.
 * 
 * Hydra_Software_Devel/13   9/5/06 1:24p katrep
 * PR23114: Fixed ES and PES MAP all playback for 7440
 * 
 * Hydra_Software_Devel/12   8/29/06 10:15a gmullen
 * PR20624: Fixed endian bug on 7118.
 * 
 * Hydra_Software_Devel/11   8/17/06 6:43p katrep
 * PR23114: Removed compiler warning for non 7440 platfroms
 * 
 * Hydra_Software_Devel/10   8/17/06 6:01p katrep
 * PR23114: Added Support for 7440 chip
 * 
 * Hydra_Software_Devel/9   7/20/06 6:31p katrep
 * PR18998: Fixed compiler warning. Unused variable
 * 
 * Hydra_Software_Devel/8   7/20/06 5:30p katrep
 * PR22365: Added better mangement of XC buffer client BW for playback.XC
 * buf for PB is given max BW & BW throtle is done at PB engine.Spare BW
 * enabllag is used to provide more BW for PB if available.
 * 
 * Hydra_Software_Devel/7   7/10/06 9:32a gmullen
 * PR18998: Fixed void * dereference warning.
 * 
 * Hydra_Software_Devel/6   6/27/06 7:33p katrep
 * PR21798: Do not reset pb parser configurations when the packetizer is
 * disabled.Coz the settings may be required for DircTV PB.
 * 
 * Hydra_Software_Devel/5   6/20/06 6:24p katrep
 * PR20184: Adding fixes for PR 20597,20184
 * 
 * Hydra_Software_Devel/4   6/7/06 2:49p katrep
 * PR20631: disbaled endian control bit toggle for 7401B0+ chips
 * 
 * Hydra_Software_Devel/3   4/19/06 5:36p gmullen
 * PR21119: Added BXPT_Playback_DisablePacketizers()
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/7   12/13/05 4:47p jgarrett
 * PR 18388: Fixing ES/PES -> TS Playback problem.  Packetizer was being
 * left on due to a logic bug in the packetizer disable routine of BXPT.
 * 
 * Hydra_Software_Devel/6   11/28/05 3:42p gmullen
 * PR17861: StopChannel() was corrupting buffer endianness control. Fixed.
 * 
 * Hydra_Software_Devel/5   10/20/05 11:14a gmullen
 * PR15309: Added support for PES and ES playback.
 * 
 * Hydra_Software_Devel/4   10/17/05 10:37a gmullen
 * PR15309: Added ES support and AllPass mode for PB parsers.
 * 
 * Hydra_Software_Devel/3   10/6/05 11:23a gmullen
 * PR15309: Fixed audio support.
 * 
 * Hydra_Software_Devel/2   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bxpt_priv.h"
#include "bxpt_playback.h"
#include "bxpt.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_playback );
#endif

#define BXPT_P_PLAYBACK_DEFAULT_USE_PCR_TIMEBASE        false
#define BXPT_P_PLAYBACK_DEFAULT_TIMESTAMP_MODE          BXPT_TimestampMode_e30_2U_Mod300    
#define BXPT_PB_MAX_SYNC_LENGTH                         ( 256 )
#define FLUSH_COUNTDOWN                                 ( 10 )      


BERR_Code BXPT_Playback_GetTotalChannels(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int *TotalChannels     /* [out] The number of playback channels. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );
    
    *TotalChannels = hXpt->MaxPlaybacks;

    return( ExitCode );
}


BERR_Code BXPT_Playback_GetChannelDefaultSettings(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int ChannelNo,         /* [in] Which channel to get defaults from. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [out] The defaults */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( ChannelSettings );

    if( ChannelNo >= hXpt->MaxPlaybacks )
    {
        /* Bad PID channel number. Complain. */
        BDBG_ERR(( "ChannelNo %lu is out of range!", ( unsigned long ) ChannelNo ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        BKNI_Memset( (void *) ChannelSettings, 0, sizeof(*ChannelSettings) );
        ChannelSettings->UsePcrTimeBase = BXPT_P_PLAYBACK_DEFAULT_USE_PCR_TIMEBASE;
        ChannelSettings->WhichPcrToUse = 0;
        ChannelSettings->TimestampEn = false;
        ChannelSettings->TimestampMode = BXPT_P_PLAYBACK_DEFAULT_TIMESTAMP_MODE;
        ChannelSettings->PacketLength = 188;
        ChannelSettings->SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
        ChannelSettings->RaveOutputOnly = false;
        ChannelSettings->DisableTimestampParityCheck = false;
        ChannelSettings->SwapBytes = false;     /* Input file is also big-endian. Don't swap */
    }

    ChannelSettings->PcrBasedPacing = false;   /* Use legacy 4-byte timestamp pacing */
    ChannelSettings->PcrPacingPid = 0x1FFF;
    ChannelSettings->Use32BitTimestamps = false;	

#if BXPT_HAS_TSMUX
    /* Defaults, to keep existing behavior. */
    ChannelSettings->PesBasedPacing = false;   
    ChannelSettings->Use8WordDesc = false;   
#endif

    return( ExitCode );
}

BERR_Code BXPT_Playback_OpenChannel(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    BXPT_Playback_Handle *PlaybackHandle,   /* [out] Handle for opened record channel */
    unsigned int ChannelNo,                         /* [in] Which channel to open. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [in] The defaults to use */
    )
{
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( PlaybackHandle );
    BSTD_UNUSED( ChannelNo );
    BSTD_UNUSED( ChannelSettings );
    return BERR_SUCCESS;
}

void BXPT_Playback_CloseChannel(
    BXPT_Playback_Handle PlaybackHandle /* [in] Handle for the channel to close*/
    )
{
    BSTD_UNUSED( PlaybackHandle );
}

BERR_Code BXPT_Playback_SetChannelSettings(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel. */
    const BXPT_Playback_ChannelSettings *ChannelSettings /* [in] New settings to use */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( ChannelSettings );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_GetChannelSettings(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [out] The current settings  */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( ChannelSettings );
    return BERR_SUCCESS;
}

#ifdef ENABLE_PLAYBACK_MUX
BERR_Code BXPT_Playback_SetChannelPacketSettings(
	BXPT_Playback_Handle hPb,								   /* [in] Handle for the playback channel. */
    const BXPT_Playback_ChannelPacketSettings *ChannelSettings /* [in] New settings to use */
	)
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( ChannelSettings );
    return BERR_SUCCESS;
}
#endif /*ENABLE_PLAYBACK_MUX*/
      
BERR_Code BXPT_Playback_GetChannelStatus(
    BXPT_Playback_Handle hPb,           /* [in] Handle for the playback channel. */
    BXPT_Playback_ChannelStatus *Status /* [out] Channel status. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Status );
    return BERR_SUCCESS;
}
      
BERR_Code BXPT_Playback_GetCurrentBufferAddress(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    uint32_t *Address                       /* [out] The address read from hardware. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Address );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_GetCurrentDescriptorAddress(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor **LastDesc       /* [in] Address of the current descriptor. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( LastDesc );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_P_CreateDesc( 
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    BXPT_PvrDescriptor * const Desc,        /* [in] Descriptor to initialize */  
    uint8_t *Buffer,                        /* [in] Data buffer. */
    uint32_t BufferLength,                  /* [in] Size of buffer (in bytes). */
    bool IntEnable,                         /* [in] Interrupt when done? */
    bool ReSync,                            /* [in] Re-sync extractor engine? */
    BXPT_PvrDescriptor * const NextDesc     /* [in] Next descriptor, or NULL */
    )
{
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( Desc );
    BSTD_UNUSED( Buffer );
    BSTD_UNUSED( BufferLength );
    BSTD_UNUSED( IntEnable );
    BSTD_UNUSED( ReSync );
    BSTD_UNUSED( NextDesc );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_CreateDesc( 
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    BXPT_PvrDescriptor * const Desc,        /* [in] Descriptor to initialize */  
    uint8_t *Buffer,                        /* [in] Data buffer. */
    uint32_t BufferLength,                  /* [in] Size of buffer (in bytes). */
    bool IntEnable,                         /* [in] Interrupt when done? */
    bool ReSync,                            /* [in] Re-sync extractor engine? */
    BXPT_PvrDescriptor * const NextDesc     /* [in] Next descriptor, or NULL */
    )
{
    BDBG_ASSERT( Desc );
    return BXPT_Playback_P_CreateDesc( hXpt, Desc, Buffer, BufferLength, IntEnable, ReSync, NextDesc );
}

void BXPT_SetLastDescriptorFlag(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    BXPT_PvrDescriptor * const Desc     /* [in] Descriptor to initialize */  
    )
{
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( Desc );
}

BERR_Code BXPT_Playback_AddDescriptors( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *LastDesc,   /* [in] Last descriptor in new chain */
    BXPT_PvrDescriptor *FirstDesc   /* [in] First descriptor in new chain */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( LastDesc );
    BSTD_UNUSED( FirstDesc );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_StartChannel( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    BSTD_UNUSED( PlaybackHandle );
    return BERR_SUCCESS;
}


BERR_Code BXPT_Playback_StopChannel( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    BSTD_UNUSED( PlaybackHandle );
    return BERR_SUCCESS;
}


BERR_Code BXPT_Playback_Pause( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    BSTD_UNUSED( PlaybackHandle );
    return BERR_SUCCESS;
}


BERR_Code BXPT_Playback_Resume( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    BSTD_UNUSED( PlaybackHandle );
    return BERR_SUCCESS;
}                                                              

BERR_Code BXPT_Playback_SetBitRate( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    uint32_t BitRate            /* [in] Rate, in bits per second. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( BitRate );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_CheckHeadDescriptor( 
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *Desc,       /* [in] Descriptor to check. */
    bool *InUse,                    /* [out] Is descriptor in use? */
    uint32_t *BufferSize            /* [out] Size of the buffer (in bytes). */
    )
{
    BSTD_UNUSED( PlaybackHandle );
    BSTD_UNUSED( Desc );
    BSTD_UNUSED( InUse );
    BSTD_UNUSED( BufferSize );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_GetTimestampUserBits(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    unsigned int *Bits                          /* [out] The user bits read from hardware. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Bits );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_ConfigPacing(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PacingControl Mode                 /* [in] New mode for pacing. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Mode );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_SetPacingErrorBound(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel */
    unsigned long MaxTsError        /* [in] Maximum timestamp error. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( MaxTsError );
    return BERR_SUCCESS;
}

void BXPT_Playback_GetPacketizerDefaults(
    const BXPT_Playback_PacketizeConfig *Cfg   /* [in] Config to initialize */
    )
{
    BDBG_ASSERT( Cfg );
}

BERR_Code BXPT_Playback_PacketizeStream(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned Context,                           /* [in] Which context to map the packets to. */
    const BXPT_Playback_PacketizeConfig *Cfg,   /* [in] Configuration for this context */
    bool Enable                                 /* [in] Start or stop packetization. */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Context );
    BSTD_UNUSED( Cfg );
    BSTD_UNUSED( Enable );
    return BERR_SUCCESS;
}

 
BERR_Code BXPT_Playback_DisablePacketizers(
    BXPT_Playback_Handle hPb                    /* [in] Handle for the playback channel */
    )
{ 
    BSTD_UNUSED( hPb );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_GetParserConfig(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_ParserConfig *ParserConfig    /* [out] The current settings */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( ParserConfig );
    return BERR_SUCCESS;
}

BERR_Code BXPT_Playback_SetParserConfig(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    const BXPT_Playback_ParserConfig *ParserConfig  /* [in] The new settings */
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( ParserConfig );
    return BERR_SUCCESS;
}

void BXPT_Playback_P_WriteReg(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    uint32_t Pb0RegAddr,
    uint32_t RegVal
    )
{
    BSTD_UNUSED( PlaybackHandle );
    BSTD_UNUSED( Pb0RegAddr );
    BSTD_UNUSED( RegVal );
}       


uint32_t BXPT_Playback_P_ReadReg(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    uint32_t Pb0RegAddr
    )
{
    BSTD_UNUSED( PlaybackHandle );
    BSTD_UNUSED( Pb0RegAddr );
    return 0;
}


#ifdef ENABLE_PLAYBACK_MUX
void BXPT_Playback_SetDescBuf( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	BXPT_PvrDescriptor * const Desc,		/* [in] Descriptor to initialize */  
	uint8_t *Buffer, 						/* [in] Data buffer. */
	uint32_t BufferLength 					/* [in] Size of buffer (in bytes). */
	)
{
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( Desc );
    BSTD_UNUSED( Buffer );
    BSTD_UNUSED( BufferLength );
}
#endif /*ENABLE_PLAYBACK_MUX*/

BINT_Id BXPT_Playback_GetIntId(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_Int PbInt
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( PbInt );
    return (BINT_Id) 0;
}

#if BXPT_HAS_TSMUX

void BXPT_Playback_GetMuxingInfo(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_MuxingInfo *Info
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Info );
}

BERR_Code BXPT_Playback_P_SetPacingSpeed( 
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned Speed
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( Speed );
    return BERR_SUCCESS;
}

void BXPT_Playback_P_SetPacingCount(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned PacingLoadMap,
    unsigned PacingCount
    )
{
    BSTD_UNUSED( hPb );
    BSTD_UNUSED( PacingLoadMap );
    BSTD_UNUSED( PacingCount );
}

#endif

bool BXPT_Playback_IsAvailable(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int ChannelNo                      /* [in] Which channel to check. */
    )
{
    BSTD_UNUSED( hXpt );
    BSTD_UNUSED( ChannelNo );
    return true;
}

void BXPT_Playback_P_Init(
	BXPT_Handle hXpt 	   					/* [in] Handle for this transport */
    )
{
    BSTD_UNUSED( hXpt );

    #if 0
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
    #endif
}
